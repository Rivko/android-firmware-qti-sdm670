/*!
 * @file vpp_ip_frc_me.h
 *
 * @cr
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * @services: Interface to Motion Estimation from Venus Encoder
 */

#ifndef _VPP_IP_FRC_ME_H_
#define _VPP_IP_FRC_ME_H_

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************
 * Local definitions
 ***********************************************************************/

#define FRC_ME_PORT_BUF_Q_SZ                VPP_INTERNAL_BUF_MAX
#define FRC_ME_CMD_Q_SZ                     128
#define FRC_ME_FRAME_RATE                   60                 // TME Max framerate is 60fps
// ME Buffer requirements
#define FRC_ME_MIN_W                        96
#define FRC_ME_MIN_H                        64
#define FRC_ME_MAX_W                        1920
#define FRC_ME_MAX_H                        1088
#define FRC_ME_NUM_PLANES                   1                  // No extra data plane
#define FRC_ME_BUF_IDX_DATATYPE             uint32_t           // max bits to hold buffer indexes
#define FRC_ME_MAX_IN_BUFS                  (sizeof(FRC_ME_BUF_IDX_DATATYPE) * 8)
#define FRC_ME_MAX_OUT_BUFS                 (sizeof(FRC_ME_BUF_IDX_DATATYPE) * 8)
#define FRC_ME_ERROR_INDEX                  (0xffffffff)
#define FRC_ME_DEFAULT_DELAY                (33*1000)          // 33 msec
#define FRC_ME_V4L2_SECURE_UNMAPPED_BUF_VAL (unsigned long)1
#define FRC_ME_1SEC                         1000000            // 1second in terms of microseconds
#define FRC_ME_POLL_NUM_FDS                 2
#define FRC_ME_NUM_PLANES                   1

#define FRC_ME_STATE_SET(_cb, state)        do { \
                                                t_EVppFrcMeState ePrev = (_cb)->eFrcMeState; \
                                                (_cb)->eFrcMeState = state; \
                                                LOGI("state transition: %u to %u", ePrev, state); \
                                            } while(0)

#define FRC_ME_IS_STATE(_cb, state)         (((_cb)->eFrcMeState == state) ? VPP_TRUE : VPP_FALSE)

// ME Flags
enum {
    eVppIpFlag_ReceiverStarted,
    eVppIpFlag_InpEosQueued,
    eVppIpFlag_InpEosReceived,
    eVppIpFlag_OutEosReceived,
    eVppIpFlag_FlushDrainStarted,
};
#define FRC_ME_RECEIVER_THREAD_STARTED    (1 << (eVppIpFlag_IpStart + eVppIpFlag_ReceiverStarted))
#define FRC_ME_INTERN_INPUT_EOS_QUEUED    (1 << (eVppIpFlag_IpStart + eVppIpFlag_InpEosQueued))
#define FRC_ME_INPUT_EOS_RECEIVED         (1 << (eVppIpFlag_IpStart + eVppIpFlag_InpEosReceived))
#define FRC_ME_OUTPUT_EOS_RECEIVED        (1 << (eVppIpFlag_IpStart + eVppIpFlag_OutEosReceived))
#define FRC_ME_FLUSH_DRAIN_STARTED        (1 << (eVppIpFlag_IpStart + eVppIpFlag_FlushDrainStarted))

// Buffer flags
enum
{
    eMeIpBufFlag_IpEos,
    eMeIpBufFlag_InternalBypass,
};
#define FRC_ME_BUF_INTERN_EOS      (1 << (eVppBufFlag_IpStart + eMeIpBufFlag_IpEos))
#define FRC_ME_BUF_INTERN_BYPASS   (1 << (eVppBufFlag_IpStart + eMeIpBufFlag_InternalBypass))

enum
{
    FRC_ME_STAT_WORKER,
    FRC_ME_STAT_WORKER_SLEEP,
    FRC_ME_STAT_RECEIVER,
    FRC_ME_STAT_RECEIVER_SLEEP,
    FRC_ME_STAT_TME_AQR_RES,
    FRC_ME_STAT_TME_REL_RES
};

typedef enum
{
    FRC_ME_STATE_IDLE,
    FRC_ME_STATE_FEED,
    FRC_ME_STATE_BYPASS,
    FRC_ME_STATE_MAX
}t_EVppFrcMeState;

typedef enum {
    FRC_ME_BUF_ME_YUV_IN,
    FRC_ME_BUF_ME_MBI_OUT,
} t_EVppIpFrcMeDumpBuf;

typedef struct {
    struct vpp_port_param stParam;
    t_StVppBufPool stQ;
} t_StVppIpFrcMePort;

typedef struct {
    sem_t sem;
    pthread_t wkThread;            // ME worker thread
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    pthread_t rxThread;            // ME receiver thread
    pthread_cond_t rxCond;

    t_StVppIpBase stBase;
    t_StVppIpPort stInput;         // Me input buffer queue
    t_StVppIpPort stOutput;        // Me output buffer queue
    t_StVppIpPort stTmeIn;
    t_StVppIpPort stTmeOut;
    t_StVppQueue stCmdQ;           // ME command queue
    t_StVppIpCmd astCmdNode[FRC_ME_CMD_Q_SZ];

    int MeFd;                      // Fd for TME device
    int MePollNotification;        // Fd for TME device notifications

    uint32_t eState;               // Holds ME IP state
    t_EVppFrcMeState eFrcMeState;  // Holds current state of ME state machine
    uint32_t u32MeFlags;           // RX_Started, Input_EOS_Rxed, Output_EOS_Rxed, Flush_Drain_Started
                                   // Worker_Started, DRAIN_PENDING, EOS_PENDING, FLUSH_PENDING
    uint32_t u32MeExitReceiver;    // Notifies the Receiver thread to exit
    uint32_t auMeStream[2];        // Status of Steam on/off for both ports
    uint64_t MeLastTimestamp;      // Holds the timestamp of the last input buf sent to TME

    // Buf Size requirement
    uint32_t u32IpBufSize;         // Input buffer size returned by TME driver
    uint32_t u32OpBufSize;         // Ouput buffer size returned by TME driver
    uint32_t u32IpBufsTmeQSize;    // Input buf queue size of TME driver
    uint32_t u32OpBufsTmeQSize;    // Output buf queue size of TME driver

    // ME buffer related counters
    uint32_t u32IpBufsSentToTME;   // In bufs sent to TME
    uint32_t u32IpBufsRxedFromTME; // In bufs received from TME
    uint32_t u32OpBufsSentToTME;   // Out bufs sent to TME
    uint32_t u32OpBufsRxedFromTME; // Out bufs received from TME
    uint32_t u32IpEosSentToTME;    // In EOS sent to TME
    uint32_t u32IpEosRxedFromTME;  // In EOS received from TME
    uint32_t u32IpBufsRxedFromCl;  // In bufs received from Client
    uint32_t u32IpBufsSentToCl;    // In bufs sent to Client
    uint32_t u32OpBufsRxedFromCl;  // Out bufs received from Client
    uint32_t u32OpBufsSentToCl;    // Out bufs sent to Client

    // Internal EOS buffer
    t_StVppBuf stBufEos;
    struct vpp_buffer stVppBufEos;
    t_StVppIonBuf stIonBuf;

    // Internal maps of input/output buffer indexes, sent to TME, each bit field represents a index
    FRC_ME_BUF_IDX_DATATYPE u32TmeIpBufIdxMap;
    FRC_ME_BUF_IDX_DATATYPE u32TmeOpBufIdxMap;

    struct {
        uint32_t u32MeCloseRet;
    } asyncRes;
} t_StVppIpFrcMeCb;

/***************************************************************************
 * Function Prototypes
 ***************************************************************************/

void *vpVppIpFrcMe_Init(t_StVppCtx *pstCtx, uint32_t u32Flags, t_StVppCallback stCb);
void vVppIpFrcMe_Term(void *ctx);
uint32_t u32VppIpFrcMe_Open(void *ctx);
uint32_t u32VppIpFrcMe_Close(void *ctx);
uint32_t u32VppIpFrcMe_SetParam(void *ctx, enum vpp_port port, struct vpp_port_param param);
uint32_t u32VppIpFrcMe_SetCtrl(void *ctx, struct hqv_control stHqvCtrl);
uint32_t u32VppIpFrcMe_GetBufferRequirements(void *ctx, t_StVppIpBufReq *o_pstInputBufReq,
                                             t_StVppIpBufReq *o_pstOutputBufReq);
uint32_t u32VppIpFrcMe_QueueBuf(void *ctx, enum vpp_port ePort, t_StVppBuf *pBuf);
uint32_t u32VppIpFrcMe_Flush(void *ctx, enum vpp_port ePort);
uint32_t u32VppIpFrcMe_Drain(void *ctx);
uint32_t u32VppIpFrcMe_Reconfigure(void *ctx, struct vpp_port_param in_param,
                                   struct vpp_port_param out_param);

#ifdef __cplusplus
 }
#endif

#endif /* _VPP_IP_FRC_ME_H_ */