#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>


static void* thrd_func1(void* arg);
static void* thrd_func2(void* arg);

int main(void)
{
    pthread_t tid1;
    pthread_t tid2;
    int ret;
    void* tret;

    printf("Main process: PID: %d, TID: %lu\n\n", getpid(), pthread_self());

    // Create thread 1
    if (pthread_create(&tid1, NULL, thrd_func1, NULL) != 0) {
        printf("Create thread 1 error!\n");
        exit(1);
    }
    printf("Create thread 1, TID: %lu\n", tid1);

    // Create thread 2
    if (pthread_create(&tid2, NULL, thrd_func2, NULL) != 0) {
        printf("Create thread 2 error!\n");
        exit(1);
    }
    printf("Create thread 2, TID: %lu\n", tid2);

    // Wait for the end of thread 1, and put the return value in "tret"
    if (pthread_join(tid1, &tret) != 0) {
        printf("Join thread 1 error!\n");
        exit(1);
    }
    printf("Thread 1 exit code: %s\n", (char*)tret);

    // Wait for the end of thread 2, and put the return value in "tret"
    ret = pthread_join(tid2, &tret);
    if (ret != 0) {
        printf("Join thread 2 error: %d!\n", ret);
        exit(1);
    }
    printf("Thread 2 exit code: %d\n", (int)tret);

    return 0;
}

static void* thrd_func1(void* arg)
{
    int i;
    for (i = 0; i < 3; i++) {
        printf("This is pthread 1 running [PID: %u, TID: %lu]\n", getpid(), pthread_self());
        sleep(1);
    }
    printf("Thread 1 exit\n");
    pthread_exit("mbtk.opencpu.top");
}

static void* thrd_func2(void* arg)
{
    int i;
    for (i = 0; i < 7; i++) {
        printf("This is pthread 2 running [PID: %u, TID: %lu]\n", getpid(), pthread_self());
        sleep(1);
    }
    printf("Thread 2 exit\n");
    pthread_exit((void*)2);   // Explicitly exit the thread, and return (void *)2
}
