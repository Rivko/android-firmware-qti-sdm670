////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file  OMX_Encoder.h
/// @brief Omx Encoder
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __OMX_ENCODER_H__
#define __OMX_ENCODER_H__

#include <unistd.h>
#include <map>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <queue>

#include <cutils/log.h>
#include <HardwareAPI.h>

#include <QComOMXMetadata.h>
#include <OMX_Core.h>
#include <OMX_Video.h>
#include <OMX_IndexExt.h>
#include <OMX_VideoExt.h>
#include <OMX_Component.h>
#include <OMX_QCOMExtns.h>


#include "BufferManager.h"

using namespace std;

enum OmxMsgType {
    LOOP_EXIT,
    READ,
    WRITE,
    EMPTYBUF_TO_OUTPUTIDX,
};

enum OmxQType {
    QTYPE_INFILGHT,
    QTYPE_OUTFILGHT,
};

enum OmxRunState{
    STATE_IDLE,
    STATE_START,
    STATE_LOADED,
};

struct OmxMsgQ {
    enum OmxMsgType      type;
    union{
        struct {
            OMX_BUFFERHEADERTYPE *buffer;
        } bufinfo;

    } u;
};

#ifndef HAL_PIXEL_FORMAT_YCbCr_420_SP_VENUS
#define HAL_PIXEL_FORMAT_YCbCr_420_SP_VENUS 0x7FA30C04
#endif

/*defines */
struct CmdType {
    OMX_EVENTTYPE        eEvent;
    OMX_COMMANDTYPE      eCmd;
    OMX_U32              sCmdData;
    OMX_ERRORTYPE        eResult;
};

typedef struct {
    char *               componentName;
    OMX_U32              inputcolorfmt;
    OMX_VIDEO_CODINGTYPE codec;
    uint32_t             npframe;
    uint32_t             nbframes;
    int                  eprofile;
    int                  elevel;
    uint32_t             bcabac;
    uint32_t             nframerate;
    uint32_t             storemeta;

    /*input port param*/
    uint32_t             input_w;
    uint32_t             input_h;
    uint32_t             input_buf_cnt;

    /*output port param*/
    uint32_t             output_w;
    uint32_t             output_h;
    uint32_t             output_buf_cnt;

    /*bitrate param*/
    uint32_t bitrate;
    uint32_t targetBitrate;
    bool isBitRateConstant;
}omx_config_t;

typedef struct  {
    OMX_U32              nSize;
    OMX_VERSIONTYPE      nVersion;
    OMX_BOOL             bEnable;
}PrependSPSPPSToIDRFramesParams;

#define OMX_SPEC_VERSION 0x00000101
#define OMX_INIT_STRUCT(_s_, _name_)            \
    memset((_s_), 0x0, sizeof(_name_));          \
(_s_)->nSize = sizeof(_name_);               \
(_s_)->nVersion.nVersion = OMX_SPEC_VERSION

#define OMX_INIT_STRUCT_SIZE(_s_, _name_)            \
    (_s_)->nSize = sizeof(_name_);               \
(_s_)->nVersion.nVersion = OMX_SPEC_VERSION

#define Log2(number, power)  { OMX_U32 temp = number; power = 0; while ( (0 == (temp & 0x1)) &&  power < 16) { temp >>=0x1; power++; } }
#define FractionToQ16(q,num,den) { OMX_U32 power; Log2(den,power); q = num << (16 - power); }

static const OMX_U32 PORT_INDEX_IN  = 0;
static const OMX_U32 PORT_INDEX_OUT = 1;
static const OMX_U32 PORT_INDEX_EXTRADATA_IN  = 2;
static const OMX_U32 PORT_INDEX_EXTRADATA_OUT = 3;

/*defines end*/
class QCameraBufferHolder {
public:
    virtual int  Read(OMX_BUFFERHEADERTYPE *buf) = 0;
    virtual OMX_ERRORTYPE Write(OMX_BUFFERHEADERTYPE *buf) = 0;
    virtual OMX_ERRORTYPE EmptyDone(OMX_BUFFERHEADERTYPE *buf) = 0;
protected:
    virtual ~QCameraBufferHolder(){};
};

class QCameraOMX_Encoder {
public:
    static  QCameraOMX_Encoder *getInstance();
    ~QCameraOMX_Encoder();
    OMX_ERRORTYPE setConfig(omx_config_t *config, QCameraBufferHolder *hilder = NULL);
    OMX_ERRORTYPE start();
    void stop();
    void flush();
    int toWait(pthread_cond_t *cond,pthread_mutex_t *mutex,int sec);
    OMX_ERRORTYPE enableMetaMode(OMX_U32 portidx);
    void enqEvent(OmxQType type, struct OmxMsgQ *event);
    void inFilghtFunc(void *);
    void outFilghtFunc(void *);

    OMX_ERRORTYPE onOmxEvent(
        OMX_IN OMX_HANDLETYPE hComponent,
        OMX_IN OMX_EVENTTYPE eEvent,
        OMX_IN OMX_U32 nData1,
        OMX_IN OMX_U32 nData2,
        OMX_IN OMX_PTR pEventData);

    OMX_ERRORTYPE onBufEmptyDone(
        OMX_IN OMX_HANDLETYPE hComponent,
        OMX_IN OMX_BUFFERHEADERTYPE* pBuffer);

    OMX_ERRORTYPE onFillBufDone(
        OMX_OUT OMX_HANDLETYPE hComponent,
        OMX_OUT OMX_BUFFERHEADERTYPE* pBuffer);

private:
    QCameraOMX_Encoder();
    OMX_BOOL CheckColorFormatSupported(OMX_COLOR_FORMATTYPE nColorFormat,
        OMX_VIDEO_PARAM_PORTFORMATTYPE *pVideoPortFmt);

    OMX_ERRORTYPE SetPortParams(OMX_U32 ePortIndex,
        OMX_U32 nWidth, OMX_U32 nHeight, OMX_U32 nBufferCountMin,
        OMX_U32 nFrameRate, OMX_U32 *nBuffersize, OMX_U32 *nBufferCount);

    static void *inFilghtLoop(void *);
    static void *outFilghtLoop(void *);

    omx_config_t              m_Config;
    char                      m_SupportComponents[64][128];
    OMX_HANDLETYPE            m_OmxHandle;
    QCameraBufferHolder *     m_Holder;

    uint32_t                  m_nInputBufferSize;
    uint32_t                  m_nInputBufferCount;

    uint32_t                  m_nOutputBufferSize;
    uint32_t                  m_nOutputBufferCount;

    OMX_BUFFERHEADERTYPE **   m_inbufs;
    OMX_BUFFERHEADERTYPE **   m_outbufs;
    uint32_t                  m_IsInit;

    /*threads*/
    pthread_t                 m_infightthread;
    pthread_t                 m_outfightthread;

    pthread_mutex_t           m_inLock;
    pthread_mutex_t           m_outLock;

    pthread_cond_t            m_inCond;
    pthread_cond_t            m_outCond;

    pthread_mutex_t           m_stateMutex;
    pthread_cond_t            m_stateCond;

    /*queues to thread*/

    queue<struct OmxMsgQ *> *m_infightQ;
    queue<struct OmxMsgQ *> *m_outfightQ;

    /*state of encoder*/
    OmxRunState               m_State;
};
#endif
