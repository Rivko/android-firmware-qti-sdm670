/*============================================================================
*
*@file omx_amr_dec_test.c
*This module contains an openamx test application for AMR component.
*
*Copyright (c) 2010, 2014, 2016-2018 Qualcomm Technologies, Inc.
*All Rights Reserved.
*Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*========================================================================*/

//////////////////////////////////////////////////////////////////////////////
//                             Include Files
//////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <linux/ioctl.h>
#include <linux/msm_audio.h>

#include "OMX_Core.h"
#include "OMX_Component.h"
#include "QOMX_AudioExtensions.h"
#include "QOMX_AudioIndexExtensions.h"
#include "adec_svr.h"

uint32_t tunnel      = 0;
uint32_t filewrite   = 0;
uint32_t allocate_buf_flag  = 0;
uint32_t write_buff_size  = 70;
uint32_t amrnb_enable = 0;
uint32_t enc_mode = 0;
uint32_t parse_first_frame = 0;


int                          m_pcmdrv_fd =-1;


/************************************************************************/
/*                              #DEFINES                                */
/************************************************************************/
#define false 0
#define true 1

#define CONFIG_VERSION_SIZE(param) \
    param.nVersion.nVersion = CURRENT_OMX_SPEC_VERSION;\
param.nSize = sizeof(param);

#define FAILED(result) (result != OMX_ErrorNone)

#define SUCCEEDED(result) (result == OMX_ErrorNone)

/************************************************************************/
/*                              GLOBAL DECLARATIONS                     */
/************************************************************************/

pthread_mutex_t lock;
pthread_mutex_t lock1;
pthread_mutexattr_t lock1_attr;
pthread_cond_t cond;
pthread_mutex_t elock;
pthread_cond_t econd;
pthread_cond_t fcond;
FILE * inputBufferFile;
FILE * outputBufferFile;
OMX_PARAM_PORTDEFINITIONTYPE inputportFmt;
OMX_PARAM_PORTDEFINITIONTYPE outputportFmt;
OMX_AUDIO_PARAM_AMRTYPE amrparam;
OMX_PORT_PARAM_TYPE portParam;
OMX_ERRORTYPE error;



/* http://ccrma.stanford.edu/courses/422/projects/WaveFormat/ */

#define ID_RIFF 0x46464952
#define ID_WAVE 0x45564157
#define ID_FMT  0x20746d66
#define ID_DATA 0x61746164


#define FORMAT_PCM 1
#define AMRNB_HEADER 6
#define AMRWB_HEADER 9


static int bFileclose = 0;

struct wav_header {
    uint32_t riff_id;
    uint32_t riff_sz;
    uint32_t riff_fmt;
    uint32_t fmt_id;
    uint32_t fmt_sz;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;       /* sample_rate * num_channels * bps / 8 */
    uint16_t block_align;     /* num_channels * bps / 8 */
    uint16_t bits_per_sample;
    uint32_t data_id;
    uint32_t data_sz;
};

static unsigned totaldatalen = 0;
QOMX_AUDIO_STREAM_INFO_DATA streaminfoparam;
/************************************************************************/
/*                          GLOBAL INIT                                 */
/************************************************************************/

unsigned int input_buf_cnt = 0;
unsigned int output_buf_cnt = 0;
int used_ip_buf_cnt = 0;
volatile int event_is_done = 0;
volatile int ebd_event_is_done = 0;
volatile int fbd_event_is_done = 0;
int ebd_cnt;
int fbd_cnt;
int bOutputEosReached = 0;
int bInputEosReached = 0;
static int etb_done = 0;
int bFlushing = false;
int bPause    = false;
const char *in_filename;
const char *out_filename;
OMX_U8* pBuffer_tmp = NULL;

long long int timeStampLfile = 0;
long int timestampInterval = 0x10000;

//* OMX Spec Version supported by the wrappers. Version = 1.1 */
const OMX_U32 CURRENT_OMX_SPEC_VERSION = 0x00000101;
OMX_COMPONENTTYPE* amr_dec_handle = 0;

OMX_BUFFERHEADERTYPE  **pInputBufHdrs = NULL;
OMX_BUFFERHEADERTYPE  **pOutputBufHdrs = NULL;

/************************************************************************/
/*                              GLOBAL FUNC DECL                        */
/************************************************************************/
int Init_Decoder(OMX_STRING audio_component);
int Play_Decoder();

OMX_STRING aud_comp;

/**************************************************************************/
/*                              STATIC DECLARATIONS                       */
/**************************************************************************/

static int open_audio_file ();
static size_t Read_Buffer(OMX_BUFFERHEADERTYPE  *pBufHdr );
static OMX_ERRORTYPE Allocate_Buffer (OMX_BUFFERHEADERTYPE  ***pBufHdrs,
        OMX_U32 nPortIndex,
        unsigned int bufCntMin, unsigned int bufSize);

static OMX_ERRORTYPE Use_Buffer (OMX_BUFFERHEADERTYPE  ***pBufHdrs,
        OMX_U32 nPortIndex,
        unsigned int bufCntMin, unsigned int bufSize);

static OMX_ERRORTYPE EventHandler(OMX_IN OMX_HANDLETYPE hComponent,
        OMX_IN OMX_PTR pAppData,
        OMX_IN OMX_EVENTTYPE eEvent,
        OMX_IN OMX_U32 nData1, OMX_IN OMX_U32 nData2,
        OMX_IN OMX_PTR pEventData);
static OMX_ERRORTYPE EmptyBufferDone(OMX_IN OMX_HANDLETYPE hComponent,
        OMX_IN OMX_PTR pAppData,
        OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);

static OMX_ERRORTYPE FillBufferDone(OMX_IN OMX_HANDLETYPE hComponent,
        OMX_IN OMX_PTR pAppData,
        OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);
static void write_devctlcmd(int fd, const void *buf, int param);

void wait_for_event(void)
{
    pthread_mutex_lock(&lock);
    DEBUG_PRINT("%s: event_is_done=%d", __FUNCTION__, event_is_done);
    while (event_is_done == 0) {
        pthread_cond_wait(&cond, &lock);
    }
    event_is_done = 0;
    pthread_mutex_unlock(&lock);
}

void event_complete(void )
{
    pthread_mutex_lock(&lock);
    if (event_is_done == 0)
    {
        event_is_done = 1;
        pthread_cond_broadcast(&cond);
    }
    pthread_mutex_unlock(&lock);
}

OMX_ERRORTYPE EventHandler(OMX_IN OMX_HANDLETYPE hComponent,
        OMX_IN OMX_PTR pAppData,
        OMX_IN OMX_EVENTTYPE eEvent,
        OMX_IN OMX_U32 nData1, OMX_IN OMX_U32 nData2,
        OMX_IN OMX_PTR pEventData)
{
    unsigned int bufCnt=0;
    DEBUG_PRINT("Function %s \n", __FUNCTION__);

    if(hComponent == NULL)
    {
        pAppData = pAppData;
        eEvent = OMX_EventMax;
        pEventData = pEventData;
    }
    switch(eEvent)
    {
        case OMX_EventCmdComplete:
            DEBUG_PRINT("*********************************************\n");
            DEBUG_PRINT("\n OMX_EventCmdComplete \n");
            DEBUG_PRINT("*********************************************\n");
            if(OMX_CommandPortDisable == (OMX_COMMANDTYPE)nData1)
            {
                DEBUG_PRINT("******************************************\n");
                DEBUG_PRINT("Recieved DISABLE Event Command Complete[%d]\n", (int)nData2);
                DEBUG_PRINT("******************************************\n");
            }
            else if(OMX_CommandPortEnable == (OMX_COMMANDTYPE)nData1)
            {
                DEBUG_PRINT("*********************************************\n");
                DEBUG_PRINT("Recieved ENABLE Event Command Complete[%d]\n", (int)nData2);
                DEBUG_PRINT("*********************************************\n");
            }
            else if(OMX_CommandFlush== (OMX_COMMANDTYPE)nData1)
            {
                DEBUG_PRINT("*********************************************\n");
                DEBUG_PRINT("Recieved FLUSH Event Command Complete[%d]\n", (int)nData2);
                DEBUG_PRINT("*********************************************\n");
            }
            event_complete();
            break;
        case OMX_EventError:
            DEBUG_PRINT("*********************************************\n");
            DEBUG_PRINT("\n OMX_EventError \n");
            DEBUG_PRINT("*********************************************\n");
            if(OMX_ErrorInvalidState == (OMX_ERRORTYPE)nData1)
            {
                DEBUG_PRINT("\n OMX_ErrorInvalidState \n");
                for(bufCnt=0; bufCnt < input_buf_cnt; ++bufCnt)
                {
                    if(!allocate_buf_flag)
                    {
                        unsigned char *pBuffer = pInputBufHdrs[bufCnt]->pBuffer;
                        DEBUG_PRINT(" deallocate use buffer %p\n", pBuffer);
                        free(pBuffer);
                    }
                    OMX_FreeBuffer(amr_dec_handle, 0, pInputBufHdrs[bufCnt]);
                }
                if(tunnel == 0)
                {
                    for(bufCnt=0; bufCnt < output_buf_cnt; ++bufCnt)
                    {
                        if(!allocate_buf_flag)
                        {
                            unsigned char *pBuffer = pOutputBufHdrs[bufCnt]->pBuffer;
                            DEBUG_PRINT(" deallocate use buffer %p\n", pBuffer);
                            free(pBuffer);
                        }
                        OMX_FreeBuffer(amr_dec_handle, 1, pOutputBufHdrs[bufCnt]);
                    }
                }

                DEBUG_PRINT("*********************************************\n");
                DEBUG_PRINT("\n Component Deinitialized \n");
                DEBUG_PRINT("*********************************************\n");
                exit(0);
            }
            else if(OMX_ErrorComponentSuspended == (OMX_ERRORTYPE)nData1)
            {
                DEBUG_PRINT("*********************************************\n");
                DEBUG_PRINT("\n Component Received Suspend Event \n");
                DEBUG_PRINT("*********************************************\n");
            }
            break;

        case OMX_EventPortSettingsChanged:
            DEBUG_PRINT("*********************************************\n");
            DEBUG_PRINT("\n OMX_EventPortSettingsChanged \n");
            DEBUG_PRINT("*********************************************\n");
            event_complete();
            break;
        case OMX_EventBufferFlag:
            DEBUG_PRINT("*********************************************\n");
            DEBUG_PRINT("\n OMX_Bufferflag \n");
            DEBUG_PRINT("*********************************************\n");
            if(tunnel)
            {
                bInputEosReached = true;
            }
            else
            {
                bOutputEosReached = true;
            }
            event_complete();
            break;
        case OMX_EventComponentResumed:
            DEBUG_PRINT("*********************************************\n");
            DEBUG_PRINT("\n Component Received Suspend Event \n");
            DEBUG_PRINT("*********************************************\n");
            break;
        default:
            DEBUG_PRINT("\n Unknown Event \n");
            break;
    }
    return OMX_ErrorNone;
}


OMX_ERRORTYPE FillBufferDone(OMX_IN OMX_HANDLETYPE hComponent,
        OMX_IN OMX_PTR pAppData,
        OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{
    unsigned int i = 0;
    size_t bytes_writen = 0;
    static int count = 0;
    pAppData = pAppData;
    static int copy_done = 0;
    static int start_done = 0;
    static unsigned int length_filled = 0;
    static unsigned int spill_length = 0;
    static unsigned int pcm_buf_size = 4800;
    static unsigned int pcm_buf_count = 2;
    struct msm_audio_config drv_pcm_config;
    fbd_cnt++;
    if(count == 0 && !filewrite)
    {
        DEBUG_PRINT(" open pcm device \n");
        m_pcmdrv_fd = open("/dev/msm_pcm_out", O_RDWR);
        if (m_pcmdrv_fd < 0)
        {
            DEBUG_PRINT("Cannot open audio device\n");
            return -1;
        }
        else
        {
            DEBUG_PRINT("Open pcm device successfull\n");
            if(0 > ioctl(m_pcmdrv_fd, (int)AUDIO_GET_CONFIG, &drv_pcm_config))
                DEBUG_PRINT("%s:%d:ioctl AUDIO_GET_CONFIG:failed\n",__func__,__LINE__);
            DEBUG_PRINT("default drv_pcm_config.buffer_count %d \n",
                    drv_pcm_config.buffer_count);
            DEBUG_PRINT("default drv_pcm_config.buffer_size %d \n",
                    drv_pcm_config.buffer_size);
            drv_pcm_config.sample_rate = 16000;
            /* 1-> mono 2-> stereo*/
            drv_pcm_config.channel_count = 1;
            if(0 > ioctl(m_pcmdrv_fd, AUDIO_SET_CONFIG, &drv_pcm_config))

                DEBUG_PRINT("%s:%d:ioctl AUDIO_SET_CONFIG:failed\n",__func__,__LINE__);
            DEBUG_PRINT("Configure Driver for PCM playback \n");
            if(0 > ioctl(m_pcmdrv_fd, (int)AUDIO_GET_CONFIG, &drv_pcm_config))

                DEBUG_PRINT("%s:%d:ioctl AUDIO_GET_CONFIG:failed\n",__func__,__LINE__);
            DEBUG_PRINT("set drv_pcm_config.buffer_count %d \n",
                    drv_pcm_config.buffer_count);
            DEBUG_PRINT("set drv_pcm_config.buffer_size %d \n",
                    drv_pcm_config.buffer_size);
            pcm_buf_size = drv_pcm_config.buffer_size;
            pcm_buf_count = drv_pcm_config.buffer_count;
            count++;
        }
        pBuffer_tmp= (OMX_U8*)malloc(pcm_buf_count*sizeof(OMX_U8)*pcm_buf_size);
        if (pBuffer_tmp == NULL)
        {
            return -1;
        }
        else
        {
            memset(pBuffer_tmp, 0, pcm_buf_count*pcm_buf_size);
        }
    }
    DEBUG_PRINT(" FillBufferDone size %d\n",(int)pBuffer->nFilledLen);
    if(bOutputEosReached)
    {
        return OMX_ErrorNone;
    }

    if( (filewrite == 1))
    {
        bytes_writen =
            fwrite((pBuffer->pBuffer + pBuffer->nOffset),1,pBuffer->nFilledLen,
                    outputBufferFile);
        totaldatalen = totaldatalen + (unsigned int)bytes_writen;
    }
    DEBUG_PRINT("file_write:totaldatalen=%d\n",totaldatalen);
    if(!filewrite)
    {
        if(pBuffer->nFilledLen)
        {
            if(start_done == 0)
            {
                if((length_filled+pBuffer->nFilledLen)>=
                        (pcm_buf_count*pcm_buf_size))
                {
                    spill_length = (pBuffer->nFilledLen-
                            (pcm_buf_count*pcm_buf_size)+length_filled);
                    memcpy (pBuffer_tmp+length_filled,
                            (pBuffer->pBuffer + pBuffer->nOffset),
                            ((pcm_buf_count*pcm_buf_size)-length_filled));
                    length_filled = (pcm_buf_count*pcm_buf_size);
                    copy_done = 1;
                }
                else
                {
                    memcpy (pBuffer_tmp+length_filled, (pBuffer->pBuffer +
                                pBuffer->nOffset), pBuffer->nFilledLen);
                    length_filled +=pBuffer->nFilledLen;
                }
                if (copy_done == 1)
                {
                    for (i=0; i<pcm_buf_count; i++)
                    {
                        if (write(m_pcmdrv_fd, pBuffer_tmp+i*pcm_buf_size,
                                    pcm_buf_size ) != pcm_buf_size)
                        {
                            DEBUG_PRINT("FillBufferDone: Write data to \
                                    PCM failed\n");
                            return -1;
                        }
                    }
                    DEBUG_PRINT("AUDIO_START called for PCM \n");
                    if (0> ioctl(m_pcmdrv_fd, AUDIO_START, 0))
                        DEBUG_PRINT("%s:%d:AUDIO_START failed:\n",__func__,__LINE__);
                    if (spill_length != 0)
                    {
                        if (write(m_pcmdrv_fd, (pBuffer->pBuffer +
                                        pBuffer->nOffset)+((pBuffer->nFilledLen)-
                                            spill_length), spill_length) != spill_length)
                        {
                            DEBUG_PRINT("FillBufferDone: Write data to PCM \
                                    failed\n");
                            return -1;
                        }
                    }
                    if (pBuffer_tmp)
                    {
                        free(pBuffer_tmp);
                        pBuffer_tmp =NULL;
                    }
                    copy_done = 0;
                    start_done = 1;
                }
            }
            else
            {
                if (write(m_pcmdrv_fd, (pBuffer->pBuffer + pBuffer->nOffset),
                            pBuffer->nFilledLen ) != (signed)pBuffer->nFilledLen)
                {
                    DEBUG_PRINT("FillBufferDone: Write data to PCM failed\n");
                    return OMX_ErrorNone;
                }
            }
        }
    }
    if(pBuffer->nFlags != OMX_BUFFERFLAG_EOS)
    {
        OMX_FillThisBuffer(hComponent,pBuffer);
    }
    else
    {
        DEBUG_PRINT(" FBD EOS REACHED...........\n");
        bOutputEosReached = true;
    }
    return OMX_ErrorNone;
}

OMX_ERRORTYPE EmptyBufferDone(OMX_IN OMX_HANDLETYPE hComponent,
        OMX_IN OMX_PTR pAppData,
        OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{
    size_t readBytes =0;
    pBuffer->nFlags = pBuffer->nFlags & (unsigned)~OMX_BUFFERFLAG_EOS;
    pAppData = pAppData;

    ebd_cnt++;
    used_ip_buf_cnt--;
    pthread_mutex_lock(&lock1);
    if(!etb_done)
    {
        DEBUG_PRINT("\n*********************************************\n");
        DEBUG_PRINT("Wait till first set of buffers are given to component\n");
        DEBUG_PRINT("\n*********************************************\n");
        etb_done++;
        pthread_mutex_unlock(&lock1);
        wait_for_event();
    }
    else
    {
        pthread_mutex_unlock(&lock1);
    }

    if(bInputEosReached)
    {
        DEBUG_PRINT("\n*********************************************\n");
        DEBUG_PRINT("   EBD::EOS on input port\n ");
        DEBUG_PRINT("   TBD:::De Init the open max here....!!!\n");
        DEBUG_PRINT("*********************************************\n");
        return OMX_ErrorNone;
    }
    else if (bFlushing == true)
    {
        DEBUG_PRINT("omx_amr_adec_test: bFlushing is set to TRUE \
                used_ip_buf_cnt=%d\n",used_ip_buf_cnt);
        if (used_ip_buf_cnt == 0)
        {
            fseek(inputBufferFile, 0, 0);
            bFlushing = false;
        } else {
            DEBUG_PRINT("omx_amr_adec_test: more buffer to come back\n");
            return OMX_ErrorNone;
        }
    }
    if((readBytes = Read_Buffer(pBuffer)) > 0)
    {
        pBuffer->nTimeStamp = timeStampLfile;
        timeStampLfile += timestampInterval;
        pBuffer->nFilledLen = (OMX_U32)readBytes;

        DEBUG_PRINT("readBytes=%d write_buff_size=%d \n",readBytes,write_buff_size);
        if(readBytes != write_buff_size) {
            DEBUG_PRINT("insert write_buff_size aligned %zu\n", readBytes);
            pBuffer->nFilledLen = write_buff_size;
        }
        used_ip_buf_cnt++;
        DEBUG_PRINT("OMX_EmptyThisBuffer:length %u \n Timestamp %lld\n",
                pBuffer->nFilledLen, pBuffer->nTimeStamp);
        OMX_EmptyThisBuffer(hComponent,pBuffer);
    }
    else{
        pBuffer->nTimeStamp = timeStampLfile;
        pBuffer->nFlags |= OMX_BUFFERFLAG_EOS;
        bInputEosReached = true;
        pBuffer->nFilledLen = 0;
        OMX_EmptyThisBuffer(hComponent,pBuffer);
        DEBUG_PRINT("EBD..Either EOS or Some Error while reading file\n");
    }
    return OMX_ErrorNone;
}

void signal_handler(int sig_id) {

    /* Flush */

    if (sig_id == SIGUSR1)
    {
        DEBUG_PRINT("%s Initiate flushing\n", __FUNCTION__);
        bFlushing = true;
        OMX_SendCommand(amr_dec_handle, OMX_CommandFlush, OMX_ALL, NULL);
    } else if (sig_id == SIGUSR2) {
        if (bPause == true) {
            DEBUG_PRINT("%s resume playback\n", __FUNCTION__);
            bPause = false;
            OMX_SendCommand(amr_dec_handle, OMX_CommandStateSet,
                    OMX_StateExecuting, NULL);
        } else {
            DEBUG_PRINT("%s pause playback\n", __FUNCTION__);
            bPause = true;
            OMX_SendCommand(amr_dec_handle, OMX_CommandStateSet,
                    OMX_StatePause, NULL);
        }
    }
}

int main(int argc, char **argv)
{
    unsigned int bufCnt=0;
    OMX_ERRORTYPE result;
    struct sigaction sa;

    struct wav_header hdr;
    size_t bytes_writen = 0;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &signal_handler;
    sigaction(SIGABRT, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    pthread_cond_init(&cond, 0);
    pthread_mutex_init(&lock, 0);

    pthread_mutexattr_init(&lock1_attr);
    pthread_mutex_init(&lock1, &lock1_attr);

    if (argc == 6)
    {
        in_filename = argv[1];
        tunnel  = (uint32_t)atoi(argv[2]);
        filewrite = (uint32_t)atoi(argv[3]);
        out_filename = argv[4];
        amrnb_enable = (uint32_t)atoi(argv[5]);

        if (tunnel == 1)
        {
            filewrite = 0;  /* File write not supported in tunnel mode */
        }
    }
    else
    {
        DEBUG_PRINT(" invalid format: \n");
        DEBUG_PRINT("ex: mm_adec_omxamr_test INPUTFILE TUNNEL FILEWRITE \
                     OUTPUTFILE amrnb_enable\n");
        DEBUG_PRINT( "TUNNEL = 1 (PLAYBACK IN TUNNELED MODE)\n");
        DEBUG_PRINT( "TUNNEL = 0 (PLAYBACK IN NONTUNNELED MODE)\n");
        DEBUG_PRINT( "FILEWRITE = 1 (ENABLES FILEWRITE IN \
            NONTUNNELED MODE ONLY)\n");
        DEBUG_PRINT("amrnb_enable: 0-disable 1-enable\n");
        return 0;
    }
    if(!amrnb_enable)
    {
        if(tunnel == 0)
        {
            aud_comp = "OMX.qcom.audio.decoder.amrwb";
        }
        else
        {
            aud_comp = "OMX.qcom.audio.decoder.tunneled.amrwb";
        }
    }
    else
    {
        if(tunnel == 0)
        {
            aud_comp = "OMX.qcom.audio.decoder.amrnb";
        }
        else
        {
            aud_comp = "OMX.qcom.audio.decoder.tunneled.amrnb";
        }
    }

    DEBUG_PRINT(" OMX test app : aud_comp = %s\n",aud_comp);

    if(Init_Decoder(aud_comp)!= 0x00)
    {
        DEBUG_PRINT("Decoder Init failed\n");
        return -1;
    }

    if(Play_Decoder() != 0x00)
    {
        DEBUG_PRINT("Play_Decoder failed\n");
        return -1;
    }

    // Wait till EOS is reached...
    wait_for_event();
    if(bOutputEosReached || (tunnel && bInputEosReached))
    {
        if((tunnel == 0) && filewrite)
        {
            hdr.riff_id = ID_RIFF;
            hdr.riff_fmt = ID_WAVE;
            hdr.fmt_id = ID_FMT;
            hdr.fmt_sz = 16;
            hdr.audio_format = FORMAT_PCM;
            hdr.num_channels = 1;
            hdr.sample_rate = 16000;
            hdr.bits_per_sample = 16;
            hdr.byte_rate = (16000 * hdr.bits_per_sample) / 8;
            hdr.block_align = (hdr.bits_per_sample) / 8;
            hdr.data_id = ID_DATA;
            hdr.data_sz = 0;
            hdr.riff_sz = hdr.data_sz + 44 - 8;

            DEBUG_PRINT("output file closed and EOS reached total \
                decoded data length %d\n",totaldatalen);
            hdr.data_sz = totaldatalen;
            hdr.riff_sz = totaldatalen + 8 + 16 + 8;
            fseek(outputBufferFile, 0L , SEEK_SET);
            bytes_writen = fwrite(&hdr,1,sizeof(hdr),outputBufferFile);
            if (bytes_writen <= 0) {
                DEBUG_PRINT("Invalid Wav header write failed\n");
            }
            bFileclose = 1;
            fclose(outputBufferFile);
        }

        DEBUG_PRINT("\nMoving the decoder to idle state \n");
        OMX_SendCommand(amr_dec_handle,
                OMX_CommandStateSet, OMX_StateIdle,0);
        wait_for_event();
        DEBUG_PRINT("\nMoving the decoder to loaded state \n");
        OMX_SendCommand(amr_dec_handle,
                OMX_CommandStateSet, OMX_StateLoaded,0);
        DEBUG_PRINT("\nFillBufferDone: Deallocating i/p buffers \n");
        for(bufCnt=0; bufCnt < input_buf_cnt; ++bufCnt)
        {
            if(!allocate_buf_flag)
            {
                unsigned char *pBuffer = pInputBufHdrs[bufCnt]->pBuffer;
                DEBUG_PRINT(" deallocate use buffer %p\n", pBuffer);
                free(pBuffer);
            }
            OMX_FreeBuffer(amr_dec_handle, 0, pInputBufHdrs[bufCnt]);
        }
        if(tunnel == 0)
        {
            DEBUG_PRINT("\nFillBufferDone: Deallocating o/p buffers \n");
            for(bufCnt=0; bufCnt < output_buf_cnt; ++bufCnt)
            {
                if(!allocate_buf_flag)
                {
                    unsigned char *pBuffer = pOutputBufHdrs[bufCnt]->pBuffer;
                    DEBUG_PRINT(" deallocate use buffer %p\n", pBuffer);
                    free(pBuffer);
                }
                OMX_FreeBuffer(amr_dec_handle, 1, pOutputBufHdrs[bufCnt]);
            }
        }
        DEBUG_PRINT( "TOTAL EBD = %d\n TOTAL FBD = %d\n", ebd_cnt,fbd_cnt);
        ebd_cnt=0;
        fbd_cnt=0;
        bInputEosReached = false;
        wait_for_event();
        bOutputEosReached = false;
        result = OMX_FreeHandle(amr_dec_handle);
        if (result != OMX_ErrorNone)
        {
            DEBUG_PRINT("\nOMX_FreeHandle error. Error code: %d\n", result);
        }
        amr_dec_handle = NULL;
        /* Deinit OpenMAX */
        OMX_Deinit();
        pthread_cond_destroy(&cond);
        pthread_mutex_destroy(&lock);
        pthread_mutexattr_destroy(&lock1_attr);
        pthread_mutex_destroy(&lock1);
        etb_done = 0;
        DEBUG_PRINT("*****************************************\n");
        DEBUG_PRINT("******...TEST COMPLETED...***************\n");
        DEBUG_PRINT("*****************************************\n");
    }

    return 0;
}

int Init_Decoder(OMX_STRING audio_component)
{
    DEBUG_PRINT("Inside %s \n", __FUNCTION__);
    OMX_ERRORTYPE omxresult;
    typedef OMX_U8* OMX_U8_PTR;

    static OMX_CALLBACKTYPE call_back = {
        &EventHandler,&EmptyBufferDone,&FillBufferDone
    };

    DEBUG_PRINT("Inside Init_Decoder - tunnel = %d\n", tunnel);
    DEBUG_PRINT("Inside Init_Decoder - filewrite = %d\n", filewrite);

    /* Init. the OpenMAX Core */
    DEBUG_PRINT("\nInitializing OpenMAX Core....\n");
    omxresult = OMX_Init();

    if(OMX_ErrorNone != omxresult)
    {
        DEBUG_PRINT("\n Failed to Init OpenMAX core");
        return -1;
    }
    else {
        DEBUG_PRINT("\nOpenMAX Core Init Done\n");
    }


    omxresult = OMX_GetHandle((OMX_HANDLETYPE*)(&amr_dec_handle),
            audio_component, NULL, &call_back);
    if (FAILED(omxresult))
    {
        DEBUG_PRINT("\nFailed to Load the component = %s\n",audio_component);
        return -1;
    }
    else
    {
        DEBUG_PRINT("\n%s:Component is in LOADED state\n",__func__);
    }

    /* Get the port information */
    CONFIG_VERSION_SIZE(portParam);
    omxresult = OMX_GetParameter(amr_dec_handle, OMX_IndexParamAudioInit,
            (OMX_PTR)&portParam);

    if(FAILED(omxresult))
    {
        DEBUG_PRINT("\nFailed to get Port Param\n");
        return -1;
    }
    else
    {
        DEBUG_PRINT("\nportParam.nPorts:%d\n", (int)portParam.nPorts);
        DEBUG_PRINT("\nportParam.nStartPortNumber:%d\n",
                (int)portParam.nStartPortNumber);
    }
    return 0;
}

int Play_Decoder()
{
    unsigned int i;
    OMX_U32 Size=0;
    OMX_ERRORTYPE ret;
    OMX_INDEXTYPE index;

    DEBUG_PRINT("Inside %s \n", __FUNCTION__);
#ifdef __LP64__
    DEBUG_PRINT("sizeof[%ld]\n", sizeof(OMX_BUFFERHEADERTYPE));
#else
    DEBUG_PRINT("sizeof[%d]\n", sizeof(OMX_BUFFERHEADERTYPE));
#endif

    /* open the i/p and o/p files based on the video file format passed */
    if(open_audio_file())
    {
        DEBUG_PRINT("\n open_audio_file returning -1");
        return -1;
    }
    /* Query the decoder input min buf requirements */
    CONFIG_VERSION_SIZE(inputportFmt);

    /* Port for which the Client needs to obtain info */
    inputportFmt.nPortIndex = portParam.nStartPortNumber;

    OMX_GetParameter(amr_dec_handle,OMX_IndexParamPortDefinition,
            &inputportFmt);
    DEBUG_PRINT("\nDec: Input Buffer Count %d\n",
            (int)inputportFmt.nBufferCountMin);
    DEBUG_PRINT("\nDec: Input Buffer Size %d\n",
            (int)inputportFmt.nBufferSize);

    if(OMX_DirInput != inputportFmt.eDir)
    {
        DEBUG_PRINT("\nDec: Expect Input Port\n");
        return -1;
    }
    if (inputportFmt.nBufferSize < write_buff_size)
        //inputportFmt.nBufferSize = write_buff_size;
        inputportFmt.nBufferSize = 8192;

    inputportFmt.nBufferCountActual = inputportFmt.nBufferCountMin ;
    OMX_SetParameter(amr_dec_handle,OMX_IndexParamPortDefinition,
            &inputportFmt);


    if(tunnel == 0)
    {
        /* Query the decoder outport's min buf requirements */
        CONFIG_VERSION_SIZE(outputportFmt);
        /* Port for which the Client needs to obtain info */
        outputportFmt.nPortIndex = portParam.nStartPortNumber + 1;

        OMX_GetParameter(amr_dec_handle,OMX_IndexParamPortDefinition,
                &outputportFmt);
        DEBUG_PRINT("\nDec: Output Buffer Count %d\n",
                (int)outputportFmt.nBufferCountMin);
        DEBUG_PRINT("\nDec: Output Buffer Size %d\n",
                (int)outputportFmt.nBufferSize);

        if(OMX_DirOutput != outputportFmt.eDir)
        {
            DEBUG_PRINT("\nDec: Expect Output Port\n");
            return -1;

        }

        outputportFmt.nBufferCountActual = outputportFmt.nBufferCountMin;
        OMX_SetParameter(amr_dec_handle,OMX_IndexParamPortDefinition,
                &outputportFmt);

    }
    OMX_GetExtensionIndex(amr_dec_handle,"OMX.Qualcomm.index.audio.sessionId",
            &index);
    OMX_GetParameter(amr_dec_handle,index,&streaminfoparam);
    CONFIG_VERSION_SIZE(amrparam);
    amrparam.nPortIndex   =  0;
    amrparam.nChannels    =  1;  /* 1-> mono */
    //amrparam.eAMRBandMode    =  enc_mode;  /* 1-> mono */
    OMX_SetParameter(amr_dec_handle,OMX_IndexParamAudioAmr,&amrparam);
    DEBUG_PRINT("\nOMX_SendCommand Decoder -> IDLE\n");
    OMX_SendCommand(amr_dec_handle, OMX_CommandStateSet, OMX_StateIdle,0);
    /* wait_for_event(); should not wait here event complete status will
       not come until enough buffer are allocated */

    input_buf_cnt = inputportFmt.nBufferCountMin;
    DEBUG_PRINT("Transition to Idle State succesful...\n");
    if(allocate_buf_flag)
    {
        /* Allocate buffer on decoder's i/p port */
        error = Allocate_Buffer(&pInputBufHdrs, inputportFmt.nPortIndex,
                input_buf_cnt, inputportFmt.nBufferSize);
        if (error != OMX_ErrorNone)
        {
            DEBUG_PRINT("\nOMX_AllocateBuffer Input buffer error\n");
            return -1;
        }
        else {
            DEBUG_PRINT("\nOMX_AllocateBuffer Input buffer success\n");
        }

        if(tunnel == 0)
        {
            output_buf_cnt = outputportFmt.nBufferCountMin;
            /* Allocate buffer on decoder's O/Pp port */
            error = Allocate_Buffer(&pOutputBufHdrs, outputportFmt.nPortIndex,
                    output_buf_cnt, outputportFmt.nBufferSize);
            if (error != OMX_ErrorNone)
            {
                DEBUG_PRINT("\nOMX_AllocateBuffer Output buffer error\n");
                return -1;
            }
            else {
                DEBUG_PRINT("\nOMX_AllocateBuffer Output buffer success\n");
            }
        }
    }
    else
    {
        /* Use buffer on decoder's i/p port */
        error = Use_Buffer(&pInputBufHdrs, inputportFmt.nPortIndex,
                input_buf_cnt, inputportFmt.nBufferSize);
        if (error != OMX_ErrorNone)
        {
            DEBUG_PRINT("\nOMX_UseBuffer Input buffer error\n");
            return -1;
        }
        else {
            DEBUG_PRINT("\nOMX_UseBuffer Input buffer success\n");
        }

        if(tunnel == 0)
        {
            output_buf_cnt = outputportFmt.nBufferCountMin;
            /* Allocate buffer on decoder's O/Pp port */
            error = Use_Buffer(&pOutputBufHdrs, outputportFmt.nPortIndex,
                    output_buf_cnt, outputportFmt.nBufferSize);
            if (error != OMX_ErrorNone)
            {
                DEBUG_PRINT("\nOMX_UseBuffer Output buffer error\n");
                return -1;
            }
            else {
                DEBUG_PRINT("\nOMX_UseBuffer Output buffer success\n");
            }
        }
    }
    wait_for_event();

    if (tunnel == 1)
    {
        DEBUG_PRINT("\nOMX_SendCommand to enable TUNNEL MODE during IDLE\n");
        OMX_SendCommand(amr_dec_handle, OMX_CommandPortDisable,1,0);
        wait_for_event();
    }

    DEBUG_PRINT("\nOMX_SendCommand Decoder -> Executing\n");
    OMX_SendCommand(amr_dec_handle, OMX_CommandStateSet, OMX_StateExecuting,0);
    wait_for_event();


    if(tunnel == 0)
    {
        DEBUG_PRINT(" Start sending OMX_FILLthisbuffer\n");

        if (pOutputBufHdrs == NULL)
        {
            DEBUG_PRINT("\npOutputBufHdrs is NULL\\n");
            return -1;
        }
        for(i=0; i < output_buf_cnt; i++) {
            DEBUG_PRINT("\nOMX_FillThisBuffer on output buf no.%d\n",i);
            if (pOutputBufHdrs[i] != NULL)
            {
                pOutputBufHdrs[i]->nOutputPortIndex = 1;
                pOutputBufHdrs[i]->nFlags =
                    pOutputBufHdrs[i]->nFlags & (unsigned) ~OMX_BUFFERFLAG_EOS;
                ret = OMX_FillThisBuffer(amr_dec_handle, pOutputBufHdrs[i]);
                if (OMX_ErrorNone != ret)
                {
                    DEBUG_PRINT("OMX_FillThisBuffer failed with result %d\n", ret);
                }
                else {
                    DEBUG_PRINT("OMX_FillThisBuffer success!\n");
                }
            }
        }
    }

    DEBUG_PRINT(" Start sending OMX_emptythisbuffer\n");
    if (pInputBufHdrs == NULL)
    {
        DEBUG_PRINT("\n pInputBufHdrs is NULL \n");
        return -1;
    }
    for (i = 0;i < input_buf_cnt;i++)
    {

        DEBUG_PRINT("\nOMX_EmptyThisBuffer on Input buf no.%d\n",i);
        if (pInputBufHdrs[i] == NULL)
        {
            DEBUG_PRINT("\n pInputBufHdrs[%d] is NULL\n",i);
            return -1;
        }
        else
        {
            pInputBufHdrs[i]->nInputPortIndex = 0;
            Size = (OMX_U32)Read_Buffer(pInputBufHdrs[i]);
            if(Size <=0 )
            {
                DEBUG_PRINT("NO DATA READ\n");
                bInputEosReached = true;
                pInputBufHdrs[i]->nFlags= OMX_BUFFERFLAG_EOS;
            }
            pInputBufHdrs[i]->nTimeStamp = timeStampLfile;
            timeStampLfile += timestampInterval;
            pInputBufHdrs[i]->nFilledLen = Size;
            pInputBufHdrs[i]->nInputPortIndex = 0;
            used_ip_buf_cnt++;
            DEBUG_PRINT("OMX_EmptyThisBuffer:length %u \nTimestamp %lld\n",
                    pInputBufHdrs[i]->nFilledLen,
                    pInputBufHdrs[i]->nTimeStamp);
            ret = OMX_EmptyThisBuffer(amr_dec_handle, pInputBufHdrs[i]);
            if (OMX_ErrorNone != ret)
            {
                DEBUG_PRINT("OMX_EmptyThisBuffer failed with result %x\n", ret);
            }
            else {
                DEBUG_PRINT("OMX_EmptyThisBuffer success!\n");
            }
            if(Size <=0 )
            {
                break;//eos reached
            }
        }
    }
    pthread_mutex_lock(&lock1);
    if(etb_done)
    {
        DEBUG_PRINT("\n****************************\n");
        DEBUG_PRINT("Component is waiting for EBD to be releases, BC signal\n");
        DEBUG_PRINT("\n****************************\n");
        event_complete();
    }
    else
    {
        DEBUG_PRINT("\n****************************\n");
        DEBUG_PRINT("EBD not yet happened ...\n");
        DEBUG_PRINT("\n****************************\n");
        etb_done++;
    }
    pthread_mutex_unlock(&lock1);
    return 0;
}

static OMX_ERRORTYPE Allocate_Buffer (OMX_BUFFERHEADERTYPE  ***pBufHdrs,
        OMX_U32 nPortIndex,
        unsigned int bufCntMin, unsigned int bufSize)
{
    DEBUG_PRINT("Inside %s \n", __FUNCTION__);
    OMX_ERRORTYPE error=OMX_ErrorNone;
    unsigned int bufCnt=0;

    *pBufHdrs= (OMX_BUFFERHEADERTYPE **)
        malloc(sizeof(OMX_BUFFERHEADERTYPE*)*bufCntMin);

    for(bufCnt=0; bufCnt < bufCntMin; ++bufCnt) {
        DEBUG_PRINT("\n OMX_AllocateBuffer No %d \n", (int)bufCnt);
        error = OMX_AllocateBuffer(amr_dec_handle, &((*pBufHdrs)[bufCnt]),
                nPortIndex, NULL, bufSize);
    }

    return error;
}

static OMX_ERRORTYPE Use_Buffer (OMX_BUFFERHEADERTYPE  ***pBufHdrs,
        OMX_U32 nPortIndex,
        unsigned int bufCntMin, unsigned int bufSize)
{
    DEBUG_PRINT("Inside %s \n", __FUNCTION__);
    OMX_ERRORTYPE error=OMX_ErrorNone;
    unsigned int bufCnt=0;
    unsigned char *pBuffer;

    *pBufHdrs= (OMX_BUFFERHEADERTYPE **)
        malloc(sizeof(OMX_BUFFERHEADERTYPE*)*bufCntMin);

    for(bufCnt=0; bufCnt < bufCntMin; ++bufCnt) {
        DEBUG_PRINT("\n Use_Buffer %d \n", bufCnt);
        pBuffer = malloc((size_t)bufSize);
        if(!pBuffer)
        {
            DEBUG_PRINT("\n Use_Buffer error allocating memory %d\n", (int)bufCnt);
            return -1;
        }
        DEBUG_PRINT(" allocate use buffer %p\n", pBuffer);
        error = OMX_UseBuffer(amr_dec_handle, &((*pBufHdrs)[bufCnt]),
                nPortIndex, NULL, bufSize, pBuffer);
    }

    return error;
}

static size_t Read_Buffer (OMX_BUFFERHEADERTYPE  *pBufHdr )
{
    uint32_t write_buff = write_buff_size;

    DEBUG_PRINT("%s:frame length:%d enc_mode=%d\n",__func__,write_buff,enc_mode);

    if(parse_first_frame == 0) {
       parse_first_frame =1;
       if(!amrnb_enable)
          fseek(inputBufferFile, AMRWB_HEADER, SEEK_SET);
       else
          fseek(inputBufferFile, AMRNB_HEADER, SEEK_SET);
    }
    size_t bytes_read=0;
    pBufHdr->nFilledLen = 0;
    pBufHdr->nFlags |= OMX_BUFFERFLAG_EOS;

    size_t read_bytes = (pBufHdr->nAllocLen >  write_buff) ? write_buff : pBufHdr->nAllocLen;
    bytes_read = fread(pBufHdr->pBuffer, 1, read_bytes , inputBufferFile);

    pBufHdr->nFilledLen = (OMX_U32)bytes_read;
    if(bytes_read == 0)
    {
        pBufHdr->nFlags |= OMX_BUFFERFLAG_EOS;
        DEBUG_PRINT("\nBytes read zero\n");
    }
    else
    {
        pBufHdr->nFlags = pBufHdr->nFlags & (unsigned)~OMX_BUFFERFLAG_EOS;
        DEBUG_PRINT("\nBytes read is Non zero=%zu\n",bytes_read);
    }
    return bytes_read;;
}

static int open_audio_file ()
{
    int error_code = 0;
    struct wav_header hdr;
    size_t header_len = 0;
    int Size = 0;
    char header[9];
    uint8_t mode;
    int amrwb_frames[9] = {18,24,33,37,41,47,51,59,61};
    int amrnb_frames[8] = {13,14,16,18,20,21,27,32};

    memset(&hdr,0,sizeof(hdr));

    hdr.riff_id = ID_RIFF;
    hdr.riff_fmt = ID_WAVE;
    hdr.fmt_id = ID_FMT;
    hdr.fmt_sz = 16;
    hdr.audio_format = FORMAT_PCM;
    hdr.num_channels = 1;
    hdr.sample_rate = 16000;
    hdr.bits_per_sample = 16;
    hdr.byte_rate = (16000 * hdr.bits_per_sample) / 8;
    hdr.block_align = (hdr.bits_per_sample) / 8;
    hdr.data_id = ID_DATA;
    hdr.data_sz = 0;
    hdr.riff_sz = hdr.data_sz + 44 - 8;

    inputBufferFile = fopen (in_filename, "rb");
    if (inputBufferFile != NULL) {
        if (!amrnb_enable) {
            fread(header, 1, AMRWB_HEADER, inputBufferFile);
            if(!(strncmp(header, "#!AMR-WB\n", AMRWB_HEADER))) {
                DEBUG_PRINT("Input file is amr-wb type:\n");
            }
            else {
                DEBUG_PRINT("Not amrwb file:header is %s:\n",header);
                return -1;
            }
            fread(&mode,1,1,inputBufferFile);
            enc_mode = (mode >>3) & (0x0f);
            write_buff_size = amrwb_frames[enc_mode];
            DEBUG_PRINT("%s: enc_mode=%d write_buff_size=%d\n", __func__, enc_mode,write_buff_size);
        }
        else
        {
            fread(header, 1, AMRNB_HEADER, inputBufferFile);
            if(!(strncmp(header, "#!AMR\n", AMRNB_HEADER))){
                DEBUG_PRINT("Input file is amr-nb type:\n");
            }
            else {
                DEBUG_PRINT("Not a amrnb file:header is %s:\n",header);
                return -1;
            }
            fread(&mode,1,1,inputBufferFile);
            enc_mode = (mode >>3) & (0x0f);
            write_buff_size = amrnb_frames[enc_mode];
            DEBUG_PRINT("%s: enc_mode=%d write_buff_size=%d\n", __func__, enc_mode,write_buff_size);
        }
        fseek(inputBufferFile,0,SEEK_SET);
    }

    /* Non Tunnel mode file write */
    if((tunnel == 0) && (filewrite == 1))
    {
        DEBUG_PRINT("output file is opened\n");
        outputBufferFile = fopen(out_filename,"wb");
        if (outputBufferFile == NULL)
        {
            DEBUG_PRINT("\no/p file %s could NOT be opened\n",
                    out_filename);
            return -1;
        }
        header_len = fwrite(&hdr,1,sizeof(hdr), outputBufferFile);

        if (header_len <= 0)
        {
            DEBUG_PRINT("Invalid Wav header \n");
        }
        DEBUG_PRINT("Length wav header is %zu \n",header_len );
    }
    return error_code;
}


static void write_devctlcmd(int fd, const void *buf, int param){
    size_t nbytes;
    ssize_t nbytesWritten;
    char cmdstr[128];
    snprintf(cmdstr, 128, "%s%d\n", (char *)buf, param);
    nbytes = strlen(cmdstr);
    nbytesWritten = write(fd, cmdstr, nbytes);

    if(nbytes != (size_t)nbytesWritten)
        DEBUG_PRINT("Failed to write string \"%s\" to omx_devmgr\n", cmdstr);
}
