/*!
 * @file vpp_stats.h
 *
 * @cr
 * Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services
 */
#ifndef _VPP_STATS_H_
#define _VPP_STATS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/time.h>
#include <pthread.h>
#include "vpp_ctx.h"

#define VPP_STAT_CB_NM                  stStatsBlk
#define VPP_STATS(_pCtx, _stat)         _pCtx->VPP_STAT_CB_NM.u32##_stat++
#define VPP_STATS_GET(_pCtx, _stat)     _pCtx->VPP_STAT_CB_NM.u32##_stat
#define VPP_STAT_PRINT(_pCtx, _stat)    LOGD("# " #_stat"=%u", _pCtx->VPP_STAT_CB_NM.u32##_stat)
#define STAT_DECL(stat)                 uint32_t u32##stat

#define VPP_PROF_DECL(__stat, __freq, __en) \
    { #__stat, __stat, __freq, __en }

#define VPP_LOG_PROF                    LOGI

#define VPP_STATS_CNT(_array)           (sizeof(_array)/sizeof(t_StVppStatsConfig))

#define VPP_STAT_MAKE_ENUM(_V, _F, _EN) _V,
#define VPP_STAT_MAKE_ENTRY(__stat, __freq, __en) \
    { #__stat, __stat, __freq, __en },

#define VPP_STAT_ENUM(_which) \
    enum { \
        _which(VPP_STAT_MAKE_ENUM) \
    };

#define VPP_STAT_DECL(_which, nm) \
    static const t_StVppStatsConfig ast##nm[] = { \
        _which(VPP_STAT_MAKE_ENTRY) \
    };

#define VPP_STAT_NM(nm) ast##nm
#define VPP_STAT_CNT(nm) VPP_STATS_CNT(ast##nm)

typedef struct {
    const char * pcName;
    uint32_t u32Key;
    uint32_t u32Freq;
    uint32_t bEnabled;
} t_StVppStatsConfig;

typedef struct {
    t_StVppStatsConfig stCfg;

    uint32_t u32Cnt;
    uint32_t u32TotalTime;

    struct timeval tv_start;
    struct timeval tv_end;

    uint32_t u32Average;
    uint32_t u32Max;
    uint32_t u32Min;
} t_StVppStat;

typedef struct StVppStatBlock {
    struct StVppStatBlock *pNext;

    t_StVppCtx *pstCtx;

    uint32_t u32StatCnt;     // size of pstStat
    t_StVppStat *pstStat;    // array of individual stats
} t_StVppStatBlock;

typedef struct StVppStatsCb {
    uint32_t u32BlockCnt;
    t_StVppStatBlock *pstBlockStart;
    uint32_t u32En;
    pthread_mutex_t mutex;
} t_StVppStatsCb;

/***************************************************************************
 * Function Prototypes
 ***************************************************************************/

uint32_t u32VppStats_Init(t_StVppCtx *pstCtx);
uint32_t u32VppStats_Term(t_StVppCtx *pstCtx);

uint32_t u32VppStats_SetEnable(t_StVppCtx *pstCtx, uint32_t u32En);

uint32_t u32VppStats_Register(t_StVppCtx *pstCtx,
                              const t_StVppStatsConfig *pstStatsConfig,
                              uint32_t u32ConfigCnt,
                              void **ppv);
uint32_t u32VppStats_Unregister(t_StVppCtx *pstCtx, void *pvHdl);

uint32_t u32VppStats_Start(t_StVppCtx *pstCtx, void *pvHdl, uint32_t u32Key);
uint32_t u32VppStats_Stop(t_StVppCtx *pstCtx, void *pvHdl, uint32_t u32Key);

uint32_t u32VppStats_ResetBlock(t_StVppCtx *pstCtx, void *pvHdl);
uint32_t u32VppStats_ResetStat(t_StVppCtx *pstCtx, void *pvHdl, uint32_t u32Key);

#ifdef __cplusplus
 }
#endif

#endif /* _VPP_STATS_H_ */
