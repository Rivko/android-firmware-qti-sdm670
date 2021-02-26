/*!
 * @file vpp_ip_hcp_tunings.h
 *
 * @cr
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services
 */

#ifndef _VPP_IP_HCP_TUNINGS_H_
#define _VPP_IP_HCP_TUNINGS_H_

#include <pthread.h>

#include "vpp_ip_hcp_priv.h"

#define HCP_TUNINGS_FILE_NAME "/persist/vpp/tunings.txt"

struct StHcpTuning;
typedef uint32_t (*val_fnc)(struct StHcpTuning *pstTuning);

typedef enum {
    TUNING_TYPE_FLOAT,
    TUNING_TYPE_U32,
    TUNING_TYPE_S32,
} t_EHcpTuningType;

typedef union {
    uint32_t U32;
    int32_t S32;
    float FLOAT;
} t_UHcpTuningType;

typedef struct {
    const char *pcId;
    uint32_t u32Id;
    t_EHcpTuningType eType;
    t_UHcpTuningType min;
    t_UHcpTuningType max;
    uint32_t u32Count;
    val_fnc pfVal;
} t_StHcpTuningDef;

typedef struct StHcpTuning {
    struct StHcpTuning *pstNext;
    const t_StHcpTuningDef *pstDef;

    uint32_t u32ExternalCnt; // number of values that were loaded from external

    uint32_t u32Id;
    uint32_t bValid;
    t_UHcpTuningType *pstVal;
} t_StHcpTuning;

typedef enum {
    TUNING_STATE_NULL,
    TUNING_STATE_INITED,
    TUNING_STATE_BUFFER_SEND_PENDING,
    TUNING_STATE_BUFFER_SENT,
    TUNING_STATE_BUFFER_RECV_PENDING,
} t_EHcpTuningState;

typedef struct {
    t_EHcpTuningState eState;
    pthread_mutex_t mutex;

    uint32_t u32ValidTuningCnt;
    t_StHcpTuning *pstTuningList;

    t_StVppIpHcpSysBuf stSysBuf;
} t_StHcpTuningCb;
/***************************************************************************
 * Function Prototypes
 ***************************************************************************/
uint32_t u32VppIpHcp_TuningInit(t_StVppIpHcpGlobalCb *pstGlobal);
uint32_t u32VppIpHcp_TuningTerm(t_StVppIpHcpGlobalCb *pstGlobal);
uint32_t u32VppIpHcp_TuningLoad(t_StVppIpHcpGlobalCb *pstGlobal);
uint32_t u32VppIpHcp_TuningProcBuffReleasedMsg(t_StVppIpHcpGlobalCb *pstGlobal,
                                               t_StHcpHfiMsgPkt *pstMsg);

#endif /* _VPP_IP_HCP_TUNINGS_H_ */
