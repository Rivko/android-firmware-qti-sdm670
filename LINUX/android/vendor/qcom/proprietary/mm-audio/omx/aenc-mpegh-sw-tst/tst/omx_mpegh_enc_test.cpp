/*===========================================================================*]
[* Copyright (c) 2018 Qualcomm Technologies, Inc.                            *]
[* All Rights Reserved.                                                      *]
[* Confidential and Proprietary - Qualcomm Technologies, Inc.                *]
[*===========================================================================*]*/
/*--------------------------------------------------------------------------
Copyright (c) 2018, The Linux Foundation. All rights reserved.
--------------------------------------------------------------------------*/

/*
    An Open max test application ....
*/
#define LOG_TAG "TEST_APP"
#include "omx_aenc_logger.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <time.h>
#include "OMX_Core.h"
#include "OMX_Component.h"
#include "pthread.h"
#include <signal.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include<string.h>
#include <pthread.h>
#include "QOMX_AudioExtensions.h"
#include "QOMX_AudioIndexExtensions.h"
#include "MPEGHEncFileUtil_c.h"

typedef unsigned char uint8;
typedef unsigned char byte;
typedef unsigned int  uint32;
typedef unsigned int  uint16;
QOMX_AUDIO_STREAM_INFO_DATA streaminfoparam;
/* maximum ADTS frame header length                */
void Release_Encoder(int);

#ifdef AUDIOV2
unsigned short session_id;
int device_id;
int control = 0;
const char *device="handset_tx";
#define DIR_TX 2
#endif

uint32_t samplerate = 8000;
uint32_t channels = 1;
uint32_t min_bitrate = 0;
uint32_t max_bitrate = 0;
uint32_t cdmarate = 0;
uint32_t rectime = 0;
uint32_t recpath = 0;
int32_t pcmplayback = 0;
uint32_t format = 1;
unsigned to_idle_transition = 0;
unsigned long total_samples;

/************************************************************************/
/*                GLOBAL INIT                    */
/************************************************************************/

/************************************************************************/
/*                #DEFINES                            */
/************************************************************************/
#define false 0
#define true 1

/************************************************************************/
/*                #DEFINES FROM MPEGH                                   */
/************************************************************************/
#define MPEGH_ENC_FRAME_SIZE_1024_SAMPLES 1024
#define MPEGH_ENC_MAX_INPUT_CHANNELS 49

#define CONFIG_VERSION_SIZE(param) \
    param.nVersion.nVersion = CURRENT_OMX_SPEC_VERSION;\
    param.nSize = sizeof(param);

#define FAILED(result) (result != OMX_ErrorNone)

#define SUCCEEDED(result) (result == OMX_ErrorNone)

#define MPEGH_ENC_DEFAULT_BITRATE 300
#define MPEGH_ENC_DEFAULT_SAMPLES_PER_FRAME 1024
#define MPEGH_ENC_DEFAULT_INDEP_INTERVAL 0


/************************************************************************/
/*                GLOBAL DECLARATIONS                     */
/************************************************************************/

pthread_mutex_t lock;
pthread_cond_t cond;
pthread_mutex_t etb_lock;
pthread_mutex_t etb_lock1;
pthread_cond_t etb_cond;
MPEGHEncFileUtilStruct objFileUtil_c;
FILE * outputBufferFile;
int hoaorder;
int bits_per_sample = 24;
float sampleBuf[MPEGH_ENC_MAX_INPUT_CHANNELS][USAC_FRAME_SIZE];    /* size = 1024 * 50 * 4  = 204800 bytes */

unsigned int mpegh_enc_bitrate = MPEGH_ENC_DEFAULT_BITRATE;
unsigned int mpegh_enc_samples_per_frame = MPEGH_ENC_DEFAULT_SAMPLES_PER_FRAME;
unsigned int mpegh_enc_indepinterval = MPEGH_ENC_DEFAULT_INDEP_INTERVAL;
int samples_per_buffer = MPEGH_ENC_FRAME_SIZE_1024_SAMPLES;
//int samples_per_buffer = 170;
OMX_PARAM_PORTDEFINITIONTYPE inputportFmt;
OMX_PARAM_PORTDEFINITIONTYPE outputportFmt;
QOMX_AUDIO_PARAM_MPEGH_TYPE mpeghparam;
OMX_AUDIO_PARAM_PCMMODETYPE    pcmparam;
OMX_PORT_PARAM_TYPE portParam;
OMX_ERRORTYPE error;

#define ID_RIFF 0x46464952
#define ID_WAVE 0x45564157
#define ID_FMT  0x20746d66
#define ID_DATA 0x61746164

struct enc_meta_out{
        unsigned int offset_to_frame;
        unsigned int frame_size;
        unsigned int encoded_pcm_samples;
        unsigned int msw_ts;
        unsigned int lsw_ts;
        unsigned int nflags;
} __attribute__ ((packed));

static int totaldatalen = 0;
static int framecnt = 0;
/************************************************************************/
/*                GLOBAL INIT                    */
/************************************************************************/

unsigned int input_buf_cnt = 0;
unsigned int output_buf_cnt = 0;
int used_ip_buf_cnt = 0;
volatile int event_is_done = 0;
volatile int ebd_event_is_done = 0;
volatile int fbd_event_is_done = 0;
volatile int etb_event_is_done = 0;
int ebd_cnt = 0;
int bInputEosReached = 0;
int bOutputEosReached = 0;
int bInputEosReached_tunnel = 0;
volatile int etb_done = 0;
int bFlushing = false;
int bPause    = false;
char *in_filename = NULL;
const char *out_filename = NULL;

#ifdef USE_LOG4CXX
char *log_config = 0;
#endif

//* OMX Spec Version supported by the wrappers. Version = 1.1 */
const OMX_U32 CURRENT_OMX_SPEC_VERSION = 0x00000101;
OMX_COMPONENTTYPE* mpegh_enc_handle = 0;

OMX_BUFFERHEADERTYPE  **pInputBufHdrs = NULL;
OMX_BUFFERHEADERTYPE  **pOutputBufHdrs = NULL;

/************************************************************************/
/*                GLOBAL FUNC DECL                        */
/************************************************************************/
int Init_Encoder(char*);
int Play_Encoder();
OMX_STRING aud_comp;
/**************************************************************************/
/*                STATIC DECLARATIONS                       */
/**************************************************************************/

static int open_audio_file ();
static int Read_Buffer(OMX_BUFFERHEADERTYPE  *pBufHdr );
static OMX_ERRORTYPE Allocate_Buffer ( OMX_COMPONENTTYPE *mpegh_enc_handle,
                                       OMX_BUFFERHEADERTYPE  ***pBufHdrs,
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
static OMX_ERRORTYPE  parse_pcm_header();


#define MAX_16 0x00007fffL
#define MIN_16 (-MAX_16-1)
#define INV_MAX_16  (1.0/MAX_16)

#define MAX_24 0x007fffffL
#define MIN_24 (-MAX_24-1)
#define INV_MAX_24 (1.0/MAX_24)


#define MAX_32 0x7fffffffL
#define MIN_32 (-MAX_32-1)
#define INV_MAX_32 (1.0/MAX_32)
static int convert_float_to_pcm(uint8_t *p_buf, size_t *p_byte_ptr, float *input)
{
  size_t byte_ptr = *p_byte_ptr;
  int ret_val = 0;
  int32_t sample = 0;
  double temp_result = 0;
  if(!p_byte_ptr || !input || !p_buf)
  {
    ret_val = -1;
    return ret_val;
  }
  switch(pcmparam.nBitPerSample)
  {
    case 16:
    {
      temp_result = (*input)*MAX_16;
      if(temp_result < MIN_16)
      {
        temp_result = MIN_16;
      }
      if(temp_result > MAX_16)
      {
        temp_result = MAX_16;
      }
      sample = (int32_t)temp_result;
      p_buf[byte_ptr++] = sample&0xFFUL;
      p_buf[byte_ptr++] = (sample&(0xFFUL<<8))>>8;
      break;
    }
    case 24:
    {
      temp_result = (*input)*MAX_24;
      if(temp_result < MIN_24)
      {
        temp_result = MIN_24;
      }
      if(temp_result > MAX_24)
      {
        temp_result = MAX_24;
      }
      sample = (int32_t)temp_result;
      p_buf[byte_ptr++] = sample&0xFF;
      p_buf[byte_ptr++] = (sample&(0xFF<<8))>>8;
      p_buf[byte_ptr++] = (sample&(0xFF<<16))>>16;
            break;
    }
    case 32:
    {
      temp_result = (*input)*MAX_32;
      if(temp_result < MIN_32)
      {
        temp_result = MIN_32;
      }
      if(temp_result > MAX_32)
      {
        temp_result = MAX_32;
      }
      sample = (int32_t)temp_result;
      p_buf[byte_ptr++] = sample&0xFF;
      p_buf[byte_ptr++] = (sample&(0xFF<<8))>>8;
      p_buf[byte_ptr++] = (sample&(0xFF<<16))>>16;
      p_buf[byte_ptr++] = (sample&(0xFF<<24))>>24;
      break;
    }
    default:
    {
      temp_result = 0;
      ret_val = -1;
    }
  }
  *p_byte_ptr = byte_ptr;
  return ret_val;
}

void wait_for_event(void)
{
    pthread_mutex_lock(&lock);
    DEBUG_PRINT("%s: event_is_done=%d", __FUNCTION__, event_is_done);
    while (event_is_done == 0) {
        pthread_cond_wait(&cond, &lock);
    }
    event_is_done--;
    if (event_is_done < 0) {
        event_is_done = 0;
    }
    DEBUG_PRINT("%s EXIT: event_is_done=%d", __FUNCTION__, event_is_done);
    pthread_mutex_unlock(&lock);
}

void event_complete(void )
{
    pthread_mutex_lock(&lock);
    DEBUG_PRINT("%s ENTRY: event_is_done=%d", __FUNCTION__, event_is_done);
    event_is_done++;
    pthread_cond_broadcast(&cond);
    DEBUG_PRINT("%s ENTRY: event_is_done=%d", __FUNCTION__, event_is_done);
    pthread_mutex_unlock(&lock);
}

void etb_wait_for_event(void)
{
    pthread_mutex_lock(&etb_lock1);
    DEBUG_PRINT("%s: etb_event_is_done=%d", __FUNCTION__, etb_event_is_done);
    while (etb_event_is_done == 0) {
        pthread_cond_wait(&etb_cond, &etb_lock1);
    }
    etb_event_is_done = 0;
    pthread_mutex_unlock(&etb_lock1);
}

void etb_event_complete(void )
{
    pthread_mutex_lock(&etb_lock1);
    if (etb_event_is_done == 0) {
        etb_event_is_done = 1;
        pthread_cond_broadcast(&etb_cond);
    }
    pthread_mutex_unlock(&etb_lock1);
}



OMX_ERRORTYPE EventHandler(OMX_IN OMX_HANDLETYPE hComponent __unused,
                           OMX_IN OMX_PTR pAppData __unused,
                           OMX_IN OMX_EVENTTYPE eEvent,
                           OMX_IN OMX_U32 nData1, OMX_IN OMX_U32 nData2,
                           OMX_IN OMX_PTR pEventData __unused)
{
    DEBUG_PRINT("Function %s \n", __FUNCTION__);
    switch(eEvent) {
    case OMX_EventCmdComplete:
        DEBUG_PRINT("\n OMX_EventCmdComplete event=%d data1=%u data2=%u\n",
                   (OMX_EVENTTYPE)eEvent,nData1,nData2);
        event_complete();
        break;
    case OMX_EventError:
        DEBUG_PRINT("\n OMX_EventError \n");
        break;
    case OMX_EventBufferFlag:
        DEBUG_PRINT("\n OMX_EventBufferFlag \n");
        bOutputEosReached = true;
        event_complete();
             break;
    case OMX_EventPortSettingsChanged:
        DEBUG_PRINT("\n OMX_EventPortSettingsChanged \n");
        break;
    default:
        DEBUG_PRINT("\n Unknown Event \n");
        break;
    }
    return OMX_ErrorNone;
}

OMX_ERRORTYPE FillBufferDone(OMX_IN OMX_HANDLETYPE hComponent,
                              OMX_IN OMX_PTR pAppData __unused,
                              OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{
    size_t bytes_writen = 0;
    size_t total_bytes_writen = 0;
    size_t len = 0;
    struct enc_meta_out *meta = NULL;
    OMX_U8 *src = pBuffer->pBuffer;
    unsigned int num_of_frames = 1;

    if(((pBuffer->nFlags & OMX_BUFFERFLAG_EOS) == OMX_BUFFERFLAG_EOS)) {
         DEBUG_PRINT("FBD::EOS on output port\n ");
         bOutputEosReached = true;
         return OMX_ErrorNone;
    }
    if(bInputEosReached_tunnel || bOutputEosReached) {
       DEBUG_PRINT("EOS REACHED NO MORE PROCESSING OF BUFFERS\n");
       return OMX_ErrorNone;
    }
    /* Skip the first bytes */
    src += sizeof(unsigned char);

    meta = (struct enc_meta_out *)src;
    while (num_of_frames > 0) {
        meta = (struct enc_meta_out *)src;
        len = meta->frame_size;
        bytes_writen = fwrite(pBuffer->pBuffer + sizeof(unsigned char) + meta->offset_to_frame,1,len,outputBufferFile);
        if(bytes_writen < len) {
           DEBUG_PRINT("error: invalid MPEGH encoded data \n");
           return OMX_ErrorNone;
        }
        src += sizeof(struct enc_meta_out);
        num_of_frames--;
        total_bytes_writen += len;
    }
    DEBUG_PRINT(" FillBufferDone size writen to file  %zu count %d\n",total_bytes_writen, framecnt);
    totaldatalen = totaldatalen + (int)total_bytes_writen;
    framecnt++;

    DEBUG_PRINT(" FBD calling FTB\n");
    OMX_FillThisBuffer(hComponent,pBuffer);

    return OMX_ErrorNone;
}

OMX_ERRORTYPE EmptyBufferDone(OMX_IN OMX_HANDLETYPE hComponent,
                              OMX_IN OMX_PTR pAppData __unused,
                              OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{
    int readBytes =0;

    ebd_cnt++;
    used_ip_buf_cnt--;
    pthread_mutex_lock(&etb_lock);
    if(!etb_done) {
       DEBUG_PRINT("\n*********************************************\n");
       DEBUG_PRINT("Wait till first set of buffers are given to component\n");
       DEBUG_PRINT("\n*********************************************\n");
       etb_done++;
       pthread_mutex_unlock(&etb_lock);
       etb_wait_for_event();
    }
    else
    {
        pthread_mutex_unlock(&etb_lock);
    }


    if(bInputEosReached) {
       DEBUG_PRINT("\n*********************************************\n");
       DEBUG_PRINT("   EBD::EOS on input port\n ");
       DEBUG_PRINT("*********************************************\n");
       return OMX_ErrorNone;
    } else if (bFlushing == true) {
       DEBUG_PRINT("omx_mpegh13_adec_test: bFlushing is set to TRUE used_ip_buf_cnt=%d\n",used_ip_buf_cnt);
       if(used_ip_buf_cnt == 0) {
          bFlushing = false;
       } else {
          DEBUG_PRINT("omx_mpegh13_adec_test: more buffer to come back used_ip_buf_cnt=%d\n",used_ip_buf_cnt);
        return OMX_ErrorNone;
      }
    }

    if((readBytes = Read_Buffer(pBuffer)) > 0) {
        pBuffer->nFilledLen = (OMX_U32)readBytes;
        used_ip_buf_cnt++;
        OMX_EmptyThisBuffer(hComponent,pBuffer);
    } else {
        pBuffer->nFlags |= OMX_BUFFERFLAG_EOS;
        used_ip_buf_cnt++;
        bInputEosReached = true;
        pBuffer->nFilledLen = 0;
        OMX_EmptyThisBuffer(hComponent,pBuffer);
        DEBUG_PRINT("EBD..Either EOS or Some Error while reading file\n");
    }
    return OMX_ErrorNone;
}

void signal_handler(int sig_id) {

  /* Flush */
  if (sig_id == SIGUSR1) {
    DEBUG_PRINT("%s Initiate flushing\n", __FUNCTION__);
    bFlushing = true;
    OMX_SendCommand(mpegh_enc_handle, OMX_CommandFlush, OMX_ALL, NULL);
  } else if (sig_id == SIGUSR2) {
    if (bPause == true) {
      DEBUG_PRINT("%s resume record\n", __FUNCTION__);
      bPause = false;
      OMX_SendCommand(mpegh_enc_handle, OMX_CommandStateSet, OMX_StateExecuting, NULL);
    } else {
      DEBUG_PRINT("%s pause record\n", __FUNCTION__);
      bPause = true;
      OMX_SendCommand(mpegh_enc_handle, OMX_CommandStateSet, OMX_StatePause, NULL);
    }
  }
}

int main(int argc, char **argv)
{

     unsigned int bufCnt=0;
     OMX_ERRORTYPE result;

    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &signal_handler;
    sigaction(SIGABRT, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    signal(SIGINT, Release_Encoder);

    pthread_cond_init(&cond, 0);
    pthread_mutex_init(&lock, 0);
    pthread_cond_init(&etb_cond, 0);
    pthread_mutex_init(&etb_lock, 0);
    pthread_mutex_init(&etb_lock1, 0);
    in_filename = argv[1];
    out_filename = argv[2];
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-hoaorder")) {
            i++;
            if (i < argc) {
                sscanf(argv[i], "%d", &hoaorder);
            }
        } else if (!strcmp(argv[i], "-indep")) {
              i++;
              if (i < argc) {
                  sscanf(argv[i], "%d", &mpegh_enc_indepinterval);
              }
        } else if (!strcmp(argv[i], "-br")) {
              i++;
              if (i < argc) {
                  sscanf(argv[i], "%d", &mpegh_enc_bitrate);
                  if ((mpegh_enc_bitrate < 96) || (mpegh_enc_bitrate > 1200)) {
                       DEBUG_PRINT_ERROR( "Unsupported bitrate %d\n", mpegh_enc_bitrate);
                       mpegh_enc_bitrate = 0;
                  }
              }
       }
    }
    aud_comp = (char *)"OMX.qcom.audio.encoder.mpegh";
    if(Init_Encoder(aud_comp)!= 0x00)
    {
        DEBUG_PRINT("Encoder Init failed\n");
        return -1;
    }

    fcntl(0, F_SETFL, O_NONBLOCK);

    if(Play_Encoder() != 0x00)
    {
        DEBUG_PRINT("Play_Decoder failed\n");
        return -1;
    }

    // Wait till EOS is reached...

    wait_for_event();

    if((bInputEosReached_tunnel) || ((bOutputEosReached))) {

       DEBUG_PRINT("\nMoving the encoder to idle state \n");
       OMX_SendCommand(mpegh_enc_handle, OMX_CommandStateSet, OMX_StateIdle,0);
       wait_for_event();

       DEBUG_PRINT("\nMoving the encoder to loaded state \n");
       OMX_SendCommand(mpegh_enc_handle, OMX_CommandStateSet, OMX_StateLoaded,0);
       sleep(1);

       DEBUG_PRINT ("\nFillBufferDone: Deallocating o/p buffers \n");
       for(bufCnt=0; bufCnt < output_buf_cnt; ++bufCnt) {
           OMX_FreeBuffer(mpegh_enc_handle, 1, pOutputBufHdrs[bufCnt]);
       }
       wait_for_event();

       result = OMX_FreeHandle(mpegh_enc_handle);
       if (result != OMX_ErrorNone) {
           DEBUG_PRINT ("\nOMX_FreeHandle error. Error code: %d\n", result);
       }

       /* Deinit OpenMAX */
       OMX_Deinit();
       ebd_cnt=0;
       bOutputEosReached = false;
       bInputEosReached_tunnel = false;
       bInputEosReached = 0;
       mpegh_enc_handle = NULL;
       pthread_cond_destroy(&cond);
       pthread_mutex_destroy(&lock);
       fclose(outputBufferFile);
       DEBUG_PRINT("*****************************************\n");
       DEBUG_PRINT("******...MPEGH ENC TEST COMPLETED...***************\n");
       DEBUG_PRINT("*****************************************\n");
    }
    return 0;
}

void Release_Encoder(int)
{
    static int cnt=0;
    OMX_ERRORTYPE result;

    DEBUG_PRINT("END OF MPEGH ENCODING: EXITING PLEASE WAIT\n");
    bInputEosReached_tunnel = 1;
    event_complete();
    cnt++;
    if(cnt > 1){
       /* FORCE RESET  */
       mpegh_enc_handle = NULL;
       ebd_cnt=0;
       bInputEosReached_tunnel = false;

       result = OMX_FreeHandle(mpegh_enc_handle);
       if (result != OMX_ErrorNone) {
           DEBUG_PRINT ("\nOMX_FreeHandle error. Error code: %d\n", result);
       }

       /* Deinit OpenMAX */
       OMX_Deinit();

       pthread_cond_destroy(&cond);
       pthread_mutex_destroy(&lock);
       DEBUG_PRINT("*****************************************\n");
       DEBUG_PRINT("******...MPEGH ENC TEST COMPLETED...***************\n");
       DEBUG_PRINT("*****************************************\n");
        exit(0);
    }
}

int Init_Encoder(OMX_STRING audio_component)
{
    DEBUG_PRINT("Inside %s \n", __FUNCTION__);
    OMX_ERRORTYPE omxresult;
    OMX_U32 total = 0;
    char *role =(char *)"audio_encoder";

    static OMX_CALLBACKTYPE call_back = {
        &EventHandler,&EmptyBufferDone,&FillBufferDone
    };

    /* Init. the OpenMAX Core */
    DEBUG_PRINT("\nInitializing OpenMAX Core....\n");
    omxresult = OMX_Init();

    if(OMX_ErrorNone != omxresult) {
        DEBUG_PRINT("\n Failed to Init OpenMAX core");
          return -1;
    }
    else {
        DEBUG_PRINT("\nOpenMAX Core Init Done\n");
    }

    /* Query for audio decoders*/
    DEBUG_PRINT("Evrc_test: Before entering OMX_GetComponentOfRole");
    OMX_GetComponentsOfRole(role, &total, 0);
    DEBUG_PRINT ("\nTotal components of role=%s :%u", role, total);

    omxresult = OMX_GetHandle((OMX_HANDLETYPE*)(&mpegh_enc_handle),
                        (OMX_STRING)audio_component, NULL, &call_back);
    if (FAILED(omxresult)) {
        DEBUG_PRINT("\nFailed to Load the component:%s\n", audio_component);
    return -1;
    } else {
        DEBUG_PRINT("\nComponent %s is in LOADED state\n", audio_component);
    }

    /* Get the port information */
    CONFIG_VERSION_SIZE(portParam);
    omxresult = OMX_GetParameter(mpegh_enc_handle, OMX_IndexParamAudioInit,
                                (OMX_PTR)&portParam);

    if(FAILED(omxresult)) {
        DEBUG_PRINT("\nFailed to get Port Param\n");
    return -1;
    } else {
        DEBUG_PRINT("\nportParam.nPorts:%u\n", portParam.nPorts);
        DEBUG_PRINT("\nportParam.nStartPortNumber:%u\n",
                                             portParam.nStartPortNumber);
    }

    if(OMX_ErrorNone != omxresult)
    {
        DEBUG_PRINT("Set parameter failed");
    }

    return 0;
}

void etb_step( unsigned int input_buf_cnt, int Size,
    int bInputEosReached, int used_ip_buf_cnt, OMX_ERRORTYPE ret,
    pthread_mutex_t etb_lock,
    OMX_BUFFERHEADERTYPE** pInputBufHdrs,
    OMX_COMPONENTTYPE* mpegh_enc_handle) {
    unsigned int i;
    DEBUG_PRINT(" Start sending OMX_emptythisbuffer\n");
    for (i = 0; i < input_buf_cnt; i++) {
         DEBUG_PRINT("\nOMX_EmptyThisBuffer on Input buf no.%d\n", i);
         pInputBufHdrs[i]->nInputPortIndex = 0;
         Size = Read_Buffer(pInputBufHdrs[i]);
         if (Size <= 0) {
             DEBUG_PRINT("NO DATA READ\n");
             bInputEosReached = true;
             pInputBufHdrs[i]->nFlags = OMX_BUFFERFLAG_EOS;
         }
         pInputBufHdrs[i]->nFilledLen = (OMX_U32) Size;
         pInputBufHdrs[i]->nInputPortIndex = 0;
         used_ip_buf_cnt++;
         ret = OMX_EmptyThisBuffer(mpegh_enc_handle, pInputBufHdrs[i]);
         if (OMX_ErrorNone != ret) {
             DEBUG_PRINT("OMX_EmptyThisBuffer failed with result %d\n", ret);
         } else {
             DEBUG_PRINT("OMX_EmptyThisBuffer success!\n");
         }
         if (Size <= 0) {
             break; //eos reached
         }
    }
    pthread_mutex_lock(&etb_lock);
    if (etb_done) {
        DEBUG_PRINT("Component is waiting for EBD to be released.\n");
        etb_event_complete();
    } else {
        DEBUG_PRINT("\n****************************\n");
        DEBUG_PRINT("EBD not yet happened ...\n");
        DEBUG_PRINT("\n****************************\n");
        etb_done++;
    }
    pthread_mutex_unlock(&etb_lock);
}

unsigned int ftb_step(unsigned int output_buf_cnt,
    OMX_BUFFERHEADERTYPE** pOutputBufHdrs, OMX_ERRORTYPE* ret,
    OMX_COMPONENTTYPE* mpegh_enc_handle) {
    unsigned int i;

    DEBUG_PRINT(" Start sending OMX_FILLthisbuffer\n");
    for (i = 0; i < output_buf_cnt; i++) {
    DEBUG_PRINT("\nOMX_FillThisBuffer on output buf no.%d\n", i);
    pOutputBufHdrs[i]->nOutputPortIndex = 1;
    pOutputBufHdrs[i]->nFlags = pOutputBufHdrs[i]->nFlags
        & (unsigned) ~OMX_BUFFERFLAG_EOS;
    *ret = OMX_FillThisBuffer(mpegh_enc_handle, pOutputBufHdrs[i]);
    if (OMX_ErrorNone != *ret) {
      DEBUG_PRINT("OMX_FillThisBuffer failed with result %d\n", *ret);
    } else {
      DEBUG_PRINT("OMX_FillThisBuffer success!\n");
    }
  }
  return i;
}

int Play_Encoder()
{
    int Size=0;
    DEBUG_PRINT("Inside %s \n", __FUNCTION__);
    OMX_ERRORTYPE ret = OMX_ErrorNone;
    OMX_INDEXTYPE index = OMX_IndexComponentStartUnused;
#ifdef __LP64__
    DEBUG_PRINT("sizeof[%ld]\n", sizeof(OMX_BUFFERHEADERTYPE));
#else
    DEBUG_PRINT("sizeof[%d]\n", sizeof(OMX_BUFFERHEADERTYPE));
#endif

    /* open the i/p and o/p files based on the video file format passed */
    if(open_audio_file()) {
        DEBUG_PRINT("\n Returning -1");
    return -1;
    }

    /* Query the encoder input min buf requirements */
    CONFIG_VERSION_SIZE(inputportFmt);

    /* Port for which the Client needs to obtain info */
    inputportFmt.nPortIndex = portParam.nStartPortNumber;

    OMX_GetParameter(mpegh_enc_handle,OMX_IndexParamPortDefinition,&inputportFmt);
    DEBUG_PRINT ("\nEnc Input Buffer Count %u\n", inputportFmt.nBufferCountMin);
    DEBUG_PRINT ("\nEnc: Input Buffer Size %u\n", inputportFmt.nBufferSize);

    if(OMX_DirInput != inputportFmt.eDir) {
        DEBUG_PRINT ("\nEnc: Expect Input Port\n");
    return -1;
    }

    pcmparam.nPortIndex   = 0;
    pcmparam.nChannels    =  channels;
    pcmparam.nSamplingRate = samplerate;
    pcmparam.bInterleaved = OMX_TRUE;
    pcmparam.nBitPerSample = bits_per_sample;
    pcmparam.eEndian = OMX_EndianLittle;
    OMX_SetParameter(mpegh_enc_handle,OMX_IndexParamAudioPcm,&pcmparam);


    /* Query the encoder outport's min buf requirements */
    CONFIG_VERSION_SIZE(outputportFmt);
    /* Port for which the Client needs to obtain info */
    outputportFmt.nPortIndex = portParam.nStartPortNumber + 1;

    OMX_GetParameter(mpegh_enc_handle,OMX_IndexParamPortDefinition,&outputportFmt);
    DEBUG_PRINT ("\nEnc: Output Buffer Count %u\n", outputportFmt.nBufferCountMin);
    DEBUG_PRINT ("\nEnc: Output Buffer Size %u\n", outputportFmt.nBufferSize);

    if(OMX_DirOutput != outputportFmt.eDir) {
        DEBUG_PRINT ("\nEnc: Expect Output Port\n");
    return -1;
    }


    CONFIG_VERSION_SIZE(mpeghparam);

    mpeghparam.nPortIndex   =  1;
    mpeghparam.nBitRate = mpegh_enc_bitrate;
    mpeghparam.nSamplesPerFrame = mpegh_enc_samples_per_frame;
    mpeghparam.nDFrames = mpegh_enc_indepinterval;
    mpeghparam.nSampleRate = samplerate;
    mpeghparam.nHOAOrder = hoaorder;

    OMX_SetParameter(mpegh_enc_handle,static_cast<OMX_INDEXTYPE>(QOMX_IndexParamAudioMpegh),&mpeghparam);
    OMX_GetParameter(mpegh_enc_handle,index,&streaminfoparam);

    DEBUG_PRINT ("\nOMX_SendCommand Encoder -> IDLE\n");
    OMX_SendCommand(mpegh_enc_handle, OMX_CommandStateSet, OMX_StateIdle,0);
    /* wait_for_event(); should not wait here event complete status will
       not come until enough buffer are allocated */

        input_buf_cnt = inputportFmt.nBufferCountActual; //  inputportFmt.nBufferCountMin + 5;
        DEBUG_PRINT("Transition to Idle State succesful...\n");
        /* Allocate buffer on decoder's i/p port */
        error = Allocate_Buffer(mpegh_enc_handle, &pInputBufHdrs, inputportFmt.nPortIndex,
                            input_buf_cnt, inputportFmt.nBufferSize);
        if (error != OMX_ErrorNone || pInputBufHdrs == NULL) {
            DEBUG_PRINT ("\nOMX_AllocateBuffer Input buffer error\n");
        return -1;

    }
    output_buf_cnt = outputportFmt.nBufferCountMin ;

    /* Allocate buffer on encoder's O/Pp port */
    error = Allocate_Buffer(mpegh_enc_handle, &pOutputBufHdrs, outputportFmt.nPortIndex,
                            output_buf_cnt, outputportFmt.nBufferSize);
    if (error != OMX_ErrorNone || pOutputBufHdrs == NULL) {
        DEBUG_PRINT ("\nOMX_AllocateBuffer Output buffer error\n");
    return -1;
    } else {
        DEBUG_PRINT ("\nOMX_AllocateBuffer Output buffer success\n");
    }

    wait_for_event();

    DEBUG_PRINT ("\nOMX_SendCommand encoder -> Executing\n");
    OMX_SendCommand(mpegh_enc_handle, OMX_CommandStateSet, OMX_StateExecuting,0);
    wait_for_event();

    ftb_step(output_buf_cnt, pOutputBufHdrs, &ret, mpegh_enc_handle);

    etb_step( input_buf_cnt, Size, bInputEosReached, used_ip_buf_cnt, ret,
       etb_lock, pInputBufHdrs, mpegh_enc_handle);

    return 0;
}

static OMX_ERRORTYPE Allocate_Buffer ( OMX_COMPONENTTYPE *avc_enc_handle,
                                       OMX_BUFFERHEADERTYPE  ***pBufHdrs,
                                       OMX_U32 nPortIndex,
                                       unsigned int bufCntMin, unsigned int bufSize)
{
    DEBUG_PRINT("Inside %s \n", __FUNCTION__);
    OMX_ERRORTYPE error=OMX_ErrorNone;
    unsigned int bufCnt=0;

    /* To remove warning for unused variable to keep prototype same */
    (void)avc_enc_handle;
    *pBufHdrs= (OMX_BUFFERHEADERTYPE **)
                   malloc(sizeof(OMX_BUFFERHEADERTYPE*)*bufCntMin);

    for(bufCnt=0; bufCnt < bufCntMin; ++bufCnt) {
        DEBUG_PRINT("\n OMX_AllocateBuffer No %d \n", bufCnt);
        error = OMX_AllocateBuffer(mpegh_enc_handle, &((*pBufHdrs)[bufCnt]),
                                   nPortIndex, NULL, bufSize);
    }

    return error;
}

static int getSamplesPerBuffer(void)
{
  return samples_per_buffer;
}

static int Read_Buffer (OMX_BUFFERHEADERTYPE  *pBufHdr )
{
  size_t bytes_read=0;
  uint32_t samples_read;

  pBufHdr->nFilledLen = 0;
  pBufHdr->nFlags |= OMX_BUFFERFLAG_EOS;

  samples_read = MPEGHEncFileUtil_readInput(&objFileUtil_c, sampleBuf, getSamplesPerBuffer());
  bytes_read = samples_read*objFileUtil_c._numInputChannels*sizeof(float);
  DEBUG_DETAIL("\nHoa Order %u\n", objFileUtil_c._hoaOrder);
  DEBUG_DETAIL("\nnumInputChannels %u\n", objFileUtil_c._numInputChannels);
  DEBUG_DETAIL("\nSamples Read %u\n", samples_read);
  DEBUG_DETAIL("\nBytes Read %zu\n", bytes_read);
  OMX_U8 *flattened_array = pBufHdr->pBuffer;
  size_t byte_ptr = 0;
  // TODO: Fix this mess
  unsigned int d1_init, d1_limit, d2_init, d2_limit, iter_d1, iter_d2, *iter_chs, *iter_samples;
  if(pcmparam.bInterleaved) {
      d1_init = 0;
      d1_limit = samples_read;
      iter_samples = &iter_d1;
      d2_init = 0;
      d2_limit = objFileUtil_c._numInputChannels;
      iter_chs = &iter_d2;
  } else {
      d1_init = 0;
      d1_limit = objFileUtil_c._numInputChannels;
      iter_chs = &iter_d1;
      d2_init = 0;
      d2_limit = samples_read;
      iter_samples = &iter_d2;
  }
  for(iter_d1 = d1_init; iter_d1 < d1_limit; iter_d1++) {
    for(iter_d2 = d2_init; iter_d2 < d2_limit; iter_d2++) {
        convert_float_to_pcm(flattened_array,&byte_ptr,&sampleBuf[*iter_chs] [*iter_samples]);
    }
  }
  DEBUG_DETAIL("\nBytes Converted to PCM %zu\n", byte_ptr);
  pBufHdr->nFilledLen = (OMX_U32)byte_ptr;
  // Time stamp logic
  ((OMX_BUFFERHEADERTYPE *)pBufHdr)->nTimeStamp = \
  (OMX_TICKS) ((total_samples * 1000)/(samplerate));

   DEBUG_PRINT ("--time stamp -- %ld\n",  (unsigned long)((OMX_BUFFERHEADERTYPE *)pBufHdr)->nTimeStamp);
   if(bytes_read == 0) {
      pBufHdr->nFlags |= OMX_BUFFERFLAG_EOS;
      DEBUG_PRINT ("\nBytes read zero\n");
   } else {
        pBufHdr->nFlags = pBufHdr->nFlags & (unsigned)~OMX_BUFFERFLAG_EOS;
        total_samples = (unsigned)(total_samples + samples_read);
   }
   bytes_read = byte_ptr;
   return (int)bytes_read;;
}

//In Encoder this Should Open a PCM or WAV file for input.
static int open_audio_file ()
{
   int error_code = 0;
   hoaorder = -1;
   DEBUG_PRINT("Inside %s filename=%s\n", __FUNCTION__, in_filename);
   /* inputBufferFile = fopen (in_filename, "rb");*/
   error_code = MPEGHEncFileUtil_openMultiChannelInputWavFile(&objFileUtil_c,in_filename, hoaorder);
   if (error_code) {
       DEBUG_PRINT("\ni/p file %s could NOT be opened\n", in_filename);
       error_code = -1;
       return error_code;
   }
   if(parse_pcm_header() != 0x00) {
      DEBUG_PRINT("PCM parser failed \n");
      return -1;
   }

   hoaorder = objFileUtil_c._hoaOrder;


   DEBUG_PRINT("Inside %s filename=%s\n", __FUNCTION__, out_filename);
   outputBufferFile = fopen (out_filename, "wb");
   if (outputBufferFile == NULL) {
       DEBUG_PRINT("\ni/p file %s could NOT be opened\n", out_filename);
       error_code = -1;
       return error_code;
   }
   return error_code;
}

static OMX_ERRORTYPE parse_pcm_header()
{
    struct wav_header const *hdr;

    DEBUG_PRINT("\n***************************************************************\n");

    hdr = MPEGHEncFileUtil_getInputWavHeader(&objFileUtil_c);

    if (hdr->coding != WAVE_FMT_IEEE) {
        DEBUG_PRINT("Wav file is not IEEE 32-bit float format %d\n", hdr->coding);
    }

    DEBUG_PRINT("Samplerate is %d\n", hdr->sampleRate);
    DEBUG_PRINT("Channel Count is %d\n", hdr->numChannels);
    DEBUG_PRINT("hoaorder is %d\n", objFileUtil_c._hoaOrder);
    DEBUG_PRINT("Bitwidth is %d\n", hdr->bitsPerSample);
    DEBUG_PRINT("\n***************************************************************\n");

    samplerate = hdr->sampleRate;
    channels = hdr->numChannels;
    total_samples = 0;

    return OMX_ErrorNone;
}
