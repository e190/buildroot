#include <stdio.h>
#include "mbtk_mqtt.h"
#include "pthread.h"
#include "string.h"
#include "unistd.h"
#include "sys/stat.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "arpa/inet.h"
#include "fcntl.h"
#include "mbtk_sock2.h"


struct opts_struct opts = {
    (char *)"iot-dev", 0, (char *)"\n", QOS0, "admin", "password", (char *)"localhost", 1883, 0
};//初始化结构体

iot_device_info_t gateway = {
    .iotstatus = IOT_STATUS_LOGIN,
    .model = {"2022"},
    .company = {"/my"}
};//初始化主题

static Cloud_MQTT_t *miot_mqtt;

void set_mqtt_t(Cloud_MQTT_t *piot_mqtt)
{
    miot_mqtt = piot_mqtt;
}
Cloud_MQTT_t * get_mqtt_t()
{
    return miot_mqtt;
}

void mqtt_data_rx_cb(void *pbuf, int len)
{

    printf("data = %s\n", (unsigned char *)pbuf);	//打印接收到的数据
}


void MQTTMessageArrived_Cb(MessageData* md)
{
    MQTTMessage *message = md->message;

    Cloud_MQTT_t *piot_mqtt = get_mqtt_t();

    if (NULL != piot_mqtt->DataArrived_Cb) {
        piot_mqtt->DataArrived_Cb((void *)message->payload, message->payloadlen);//异步消息体
    }
}

void *mqtt_thread(void *arg)
{
    while (gateway.iotstatus != IOT_STATUS_DROP){
        printf("gateway.iotstatus is %d\n",gateway.iotstatus);
        iot_yield(arg,&gateway);									//维持服务器稳定，断开重连
    }
    printf("gateway.iotstatus is IOT_STATUS_DROP");
//    mqtt_device_disconnect(arg);
    pthread_exit(NULL);
    return NULL;
}

int main(int argc, char *argv[])
{
    if(argc >6 || argc < 4)
    {
        printf("input error.\n example:./mqtt_test 192.168.1.1 8080 clientid username password\n");
        return 0;
    }
    int ret, len;
    char will_msg[256] = {"hello world"};						//初始化遗嘱数据
    static int retry_count = 0;
    pthread_t thread_ID;      //定义线程id
    static int i = 0;

    Cloud_MQTT_t *Iot_mqtt;
    Iot_mqtt = (Cloud_MQTT_t *)malloc(sizeof(Cloud_MQTT_t));
    memset(Iot_mqtt,0x0,sizeof(Cloud_MQTT_t));

    iot_mqtt_init(Iot_mqtt,argv[1],atoi(argv[2]),argv[3],argv[4],argv[5],30,3,"aaa","aaa",mqtt_data_rx_cb);									//初始化
    mqtt_will_msg_set(Iot_mqtt, will_msg, strlen(will_msg));	//设置遗嘱
    set_mqtt_t(Iot_mqtt);
    ret = mqtt_device_connect(Iot_mqtt);						//初始化并连接mqtt服务器


    while (ret < 0 && retry_count <= 5) {
        printf("ret = %d\r\n", ret);
        retry_count ++ ;
        printf("retry_count = %d\n",retry_count);
        sleep(3);
        ret = mqtt_device_connect(Iot_mqtt);
    }
    if(ret < 0 && retry_count > 5)
    {
        printf("reconnect times more than 5 exit\n");
        return NULL;
    }
    retry_count = 0;

    printf("connect success\n");
    int rc = mbtk_MQTTSubscribe(&Iot_mqtt->Client, Iot_mqtt->sub_topic, opts.qos, MQTTMessageArrived_Cb);
    if (rc) {
        printf("mqtt subscribe fail \n");
        ret = -105;
        return 0;
    }
    gateway.iotstatus = IOT_STATUS_CONNECT;

    pthread_create(&thread_ID, NULL, &mqtt_thread, (void *)Iot_mqtt);	//创建一个线程执行mqtt客户端
    pthread_detach(thread_ID);	//设置线程结束收尸

    while (1)
    {
        mbtk_MQTTPublish("my yes", 6, 0,&Iot_mqtt->Client,Iot_mqtt->pub_topic,2,0);//循环发布"my yes"
        sleep(3);						//睡眠3s
        i++;
        if (i > 5)
        {
            mbtk_MQTTUnsubscribe(&Iot_mqtt->Client,Iot_mqtt->sub_topic);
            break;
        }
    }
    return 0;
}
