/*!
 * @file vpp_callback.h
 *
 * @cr
 * Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.

 * @services
 */
#ifndef _VPP_CALLBACK_H_
#define _VPP_CALLBACK_H_

#include "vpp_buf.h"

#ifdef __cplusplus
extern "C" {
#endif

struct StVppUsecase;

typedef enum {
    VPP_EVT_FLUSH_DONE = 1,
    VPP_EVT_DRAIN_DONE,
    VPP_EVT_RECONFIG_RDY,
    VPP_EVT_RECONFIG_DONE,
    VPP_EVT_ERR_SYSTEM,
    VPP_EVT_ERR_MAX_CLIENTS,
    VPP_EVT_ERR_HW_OVERLOAD,
    VPP_EVT_ERR_HW_UNSUPPORTED,
    VPP_EVT_MAX,
} t_EVppEvtType;


typedef struct {
    t_EVppEvtType eType;
    union {
        struct {
            enum vpp_port ePort;
        } flush;
        struct {
            uint32_t u32Err;
            struct StVppUsecase *pstUc;
        } reconfig;
    };
} t_StVppEvt;

typedef struct {
    void *pv;
    void (*input_buffer_done)(void *pv, t_StVppBuf *pstBuf);
    void (*output_buffer_done)(void *pv, t_StVppBuf *pstBuf);
    void (*event)(void *pv, t_StVppEvt stEvt);
    void (*log)(void *pv, t_StVppBuf *pstBuf, t_EVppBufIpLogId eId);
} t_StVppCallback;

/***************************************************************************
 * Function Prototypes
 ***************************************************************************/
uint32_t u32VppIp_CbBufDone(t_StVppCallback *pstCb, enum vpp_port ePort,
                            t_StVppBuf *pstBuf);

uint32_t u32VppIpCbEvent(t_StVppCallback *pstCb, t_StVppEvt stEvt);

void vVppIpCbLog(t_StVppCallback *pstCb, t_StVppBuf *pstBuf, t_EVppBufIpLogId eId);

#ifdef __cplusplus
 }
#endif

#endif /* _VPP_CALLBACK_H_ */
