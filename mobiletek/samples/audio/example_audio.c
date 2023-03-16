#include "mbtk_type.h"
#include <fcntl.h>
#include <stdint.h>
#include <limits.h>
#include <termios.h>
#include <stdarg.h>
// #include "ql_at.h"
#include "ql_audio.h"
// #include "mopen_tts.h"

#define MBTK_AUD_DEMO_WAV "/data/demo.wav"

static int record_fd = 0;

int Ql_cb_playback(int hdl, int result)
{
    printf("%s: hdl=%d, result=%d\n\r", __func__, hdl, result);
    if (result == AUD_PLAYER_FINISHED || result == AUD_PLAYER_NODATA)
    {
        printf("%s: play finished\n\r", __func__);
    }
    return 0;
}

void record_cb_func(int cb_result, char* databuf, unsigned int len)
{
    int rc;

    if(NULL != databuf && len > 0 && record_fd > 0)
    {
        //for debug:save into file
        rc = write(record_fd, databuf, len);
        if (rc < 0) {
            printf("%s: error writing to file!\n", __FUNCTION__);
        } else if (rc < len) {
            printf("%s: wrote less the buffer size!\n", __FUNCTION__);
        }
    }
}
int MBTK_wav_pcm16Le_check(int fd)
{
    struct wav_header hdr;

    if (fd <= 0)
        return -1;

    if (read(fd, &hdr, sizeof(hdr)) != sizeof(hdr))
    {
        printf("\n%s: cannot read header\n", __FUNCTION__);
        return -1;
    }

    if ((hdr.riff_id != ID_RIFF)
            || (hdr.riff_fmt != ID_WAVE)
            || (hdr.fmt_id != ID_FMT))
    {
        printf("\n%s: is not a riff/wave file\n", __FUNCTION__);
        return -1;
    }

    if ((hdr.audio_format != FORMAT_PCM) || (hdr.fmt_sz != 16)) {
        printf("\n%s: is not pcm format\n", __FUNCTION__);
        return -1;
    }

    if (hdr.bits_per_sample != 16) {
        printf("\n%s: is not 16bit per sample\n", __FUNCTION__);
        return -1;
    }

    return 0;
}

int MBTK_wav_pcm16Le_set(int fd)
{
    struct wav_header hdr;

    if (fd <= 0)
        return -1;

    memset(&hdr, 0, sizeof(struct wav_header));

    hdr.riff_id = ID_RIFF;
    hdr.riff_fmt = ID_WAVE;
    hdr.fmt_id = ID_FMT;
    hdr.fmt_sz = 16;
    hdr.audio_format = FORMAT_PCM;
    hdr.num_channels = 1;
    hdr.sample_rate = 8000;
    hdr.bits_per_sample = 16;
    hdr.byte_rate = (8000 * 1 * hdr.bits_per_sample) / 8;
    hdr.block_align = (hdr.bits_per_sample * 1) / 8;
    hdr.data_id = ID_DATA;
    hdr.data_sz = 0;

    hdr.riff_sz = hdr.data_sz + 44 - 8;
    if (write(fd, &hdr, sizeof(hdr)) != sizeof(hdr)) {
        return -1;
    }

    return 0;
}
void aplay(void)
{
    char operator[10];
    char databuf[1024];
    int opt = 0;
    int fd = 0;
    int size = 0;
    int state;
    int play_hdl = 0;

    while(1)
    {
        printf("=========aplay========\n"
            "\t 0 Open PCM\n"
            "\t 1 Play Stream\n"
            "\t 2 Play file\n"
            "\t 3 Close\n"
            "\t others exit\n\n"
            "operator >> ");

        fflush(stdin);
        fgets(operator, sizeof(operator), stdin);
        opt = atoi(operator);
        switch (opt)
        {
            case 0:
                play_hdl = Ql_AudPlayer_Open(NULL, Ql_cb_playback);
                if(0 == play_hdl)
                    printf("Ql_AudPlayer_Open fail\n");
                break;
            case 1:
               if(0 == play_hdl)
                    continue;

                fd = open(MBTK_AUD_DEMO_WAV, O_RDWR);
                if (fd <= 0)
                    continue;

                if(0 == MBTK_wav_pcm16Le_check(fd))
                {
                    memset(databuf, 0, sizeof(databuf));
                    while(0 < (size = read(fd, databuf, sizeof(databuf))))
                    {
                        if(-1 == Ql_AudPlayer_Play(play_hdl, databuf, size))
                            break;
                    }
                    printf("aplay Stream end \n");
                }

                close(fd);
                break;
            case 2:
                if(0 == play_hdl)
                    continue;

                fd = open(MBTK_AUD_DEMO_WAV, O_RDWR);
                if (fd <= 0)
                    continue;

                if(0 == MBTK_wav_pcm16Le_check(fd))
                {
                    Ql_AudPlayer_PlayFrmFile(play_hdl, fd, 0);
                }
                else
                {
                    printf("aplay file type error\n");
                }
                close(fd);
                break;
            case 3:
                if(0 == play_hdl)
                    continue;
                Ql_AudPlayer_Close(play_hdl);
                break;
            default:
                return;
        }

        sleep(1);
    }

    printf("aplay exit\n");
    return ;
}
void arec(void)
{
    int ret;
    char operator[10];
    int opt;
    int hdl = 0;

    while(1)
    {
        printf("=======arec======\n"
            "\t 0 Open PCM\n"
            "\t 1 Start Record\n"
            "\t 2 Get state\n"
            "\t 3 Pause\n"
            "\t 4 Resume\n"
            "\t 5 Stop\n"
            "\t 6 Close\n"
            "\t others exit\n\n"
            "operator >> ");

        fflush(stdin);
        fgets(operator, sizeof(operator), stdin);
        opt = atoi(operator);
        switch (opt)
        {
            case 0:
                hdl = Ql_AudRecorder_Open(NULL, record_cb_func);
                if (hdl == 0)
                    return ;
                break;
            case 1:
                if(0 == hdl)
                {
                    printf("audio is not initialized yet.\n");
                    continue;
                }

                if(0 != record_fd)
                {
                    printf("audio It's already being recorded.\n");
                    continue;
                }
                record_fd = open(MBTK_AUD_DEMO_WAV, O_RDWR|O_CREAT|O_TRUNC, 0644);
                if (record_fd <= 0)
                    printf("file open error\n");

                if(0 == MBTK_wav_pcm16Le_set(record_fd))
                {
                    ret = Ql_AudRecorder_StartRecord();
                    if(0 != ret)
                    {
                        printf("audio record error: %d\n", ret);
                        close(record_fd);
                        record_fd = 0;
                    }
                }
                else
                {
                    printf("arec set file header error\n");
                    close(record_fd);
                    record_fd = 0;
                }
                break;
            case 2:
                // printf("arec state : %d\n", state);
                break;
            case 3:
                break;
            case 4:
                break;
            case 5:
                break;
            case 6:
                Ql_AudRecorder_Close();
                if(record_fd > 0)
                {
                    close(record_fd);
                    record_fd = 0;
                }
                break;
            default:
                return;
        }

        sleep(1);
    }

    printf("arec exit\n");
    return ;
}
int main(void)
{
    char operator[10];
    int opt;

    while(1)
    {
        printf("=========audio main=========\n"
            "\t0 exit\n"
            "\t1 aplay\n"
            "\t2 arec\n"
            "\t3 set speaker Volume\n"
            "\t4 get speaker Volume\n"
            "\t5 set mic Volume\n"
            "\t6 get mic Volume\n"
            "\t7 tts\n"
            "\t8 tone\n"
            "operator: >> ");

        fgets(operator, sizeof(operator), stdin);
        fflush(stdin);
        opt = atoi(operator);
        switch (opt)
        {
            case 0:
                printf("main exit\n");
                return 0;
            case 1:
                aplay();
                break;
            case 2:
                arec();
                break;
            case 3:
                mbtk_at_rec(NULL);
                break;
            case 4:
                mbtk_at_play(NULL);
                break;
            case 5:
                break;
            case 6:
                break;
            case 7:
                break;
            case 8:
                break;
            default:
                break;
        }

        sleep(1);
    }

    return 0;
}
