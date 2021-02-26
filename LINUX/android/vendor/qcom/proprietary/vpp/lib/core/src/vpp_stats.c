/*!
 * @file vpp_stats.c
 *
 * @cr
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * @services
 */

#include <sys/types.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

#define VPP_LOG_MODULE     VPP_LOG_MODULE_PROFILE
#define VPP_LOG_TAG        VPP_LOG_MODULE_PROFILE_TAG

#include "vpp.h"
#include "vpp_def.h"
#include "vpp_ctx.h"
#include "vpp_dbg.h"
#include "vpp_utils.h"
#include "vpp_stats.h"

/************************************************************************
 * Local definitions
 ***********************************************************************/

/************************************************************************
 * Local static variables
 ***********************************************************************/

/************************************************************************
 * Forward Declarations
 ************************************************************************/

/************************************************************************
 * Local Functions
 ***********************************************************************/
static inline uint32_t bIsHandleValid(t_StVppCtx *pstCtx, void *pvHdl)
{
    if (!pstCtx || !pvHdl)
        return VPP_FALSE;
    return (((t_StVppStatBlock *)pvHdl)->pstCtx == pstCtx) ? VPP_TRUE : VPP_FALSE;
}

static uint32_t u32VppStats_InsertBlock(t_StVppStatsCb *pstCb,
                                        t_StVppStatBlock *pstStatBlk)
{
    if (!pstCb || !pstStatBlk)
        return VPP_ERR_PARAM;

    pstStatBlk->pNext = pstCb->pstBlockStart;
    pstCb->pstBlockStart = pstStatBlk;
    pstCb->u32BlockCnt += 1;

    return VPP_OK;
}

static uint32_t u32VppStats_RemoveBlock(t_StVppStatsCb *pstCb,
                                        t_StVppStatBlock *pstStatBlk)
{
    t_StVppStatBlock *pstTmp, *pstPrev;

    if (!pstCb || !pstStatBlk)
        return VPP_ERR_PARAM;

    if (!pstCb->u32BlockCnt)
        return VPP_ERR;

    pstPrev = NULL;
    pstTmp = pstCb->pstBlockStart;
    while (pstTmp && pstTmp != pstStatBlk)
    {
        pstPrev = pstTmp;
        pstTmp = pstTmp->pNext;
    }

    if (pstTmp)
    {
        if (pstPrev)
            pstPrev->pNext = pstTmp->pNext;
        else
            pstCb->pstBlockStart = pstTmp->pNext;

        pstCb->u32BlockCnt -= 1;
        return VPP_OK;
    }

    return VPP_ERR;
}

static t_StVppStat *pstVppStats_FindStat(t_StVppStatBlock *pstBlk,
                                         uint32_t u32Key)
{
    if (!pstBlk)
        return NULL;

    uint32_t i;
    for (i = 0; i < pstBlk->u32StatCnt; i++)
    {
        if (pstBlk->pstStat[i].stCfg.u32Key == u32Key)
            return &pstBlk->pstStat[i];
    }
    return NULL;
}
/************************************************************************
 * Global Functions
 ***********************************************************************/

uint32_t u32VppStats_Init(t_StVppCtx *pstCtx)
{
    int rc;
    t_StVppStatsCb *pstCb;

    if (!pstCtx)
        return VPP_ERR_PARAM;

    pstCb = calloc(1, sizeof(t_StVppStatsCb));
    if (!pstCb)
        return VPP_ERR_NO_MEM;

    pstCtx->pstStats = pstCb;

    rc = pthread_mutex_init(&pstCb->mutex, NULL);
    if (rc)
    {
        free(pstCb);
        LOGE("ERROR: unable to initialze mutex, rc=%d, (%s)", rc, strerror(rc));
        return VPP_ERR;
    }

    return VPP_OK;
}

uint32_t u32VppStats_Term(t_StVppCtx *pstCtx)
{
    int rc;
    t_StVppStatsCb *pstCb;
    t_StVppStatBlock *pstStatBlk;

    if (!pstCtx || !pstCtx->pstStats)
        return VPP_ERR_PARAM;

    pstCb = pstCtx->pstStats;

    // If there are any stats that have not been unregistered, remove them
    // from the list and free the memory (the same ops that would be done as
    // if unregistered were in fact called on the block)
    uint32_t u32Cnt = 0;
    while ((pstStatBlk = pstCb->pstBlockStart) != NULL)
    {
        LOGE("WARNING: term called while blocks still registered (%u)", u32Cnt++);
        u32VppStats_RemoveBlock(pstCb, pstStatBlk);
        free(pstStatBlk->pstStat);
        free(pstStatBlk);
    }

    rc = pthread_mutex_destroy(&pstCb->mutex);
    LOGE_IF(rc, "ERROR: unable to destroy mutex, %d (%s)", rc, strerror(rc));
    free(pstCb);

    return VPP_OK;
}

uint32_t u32VppStats_SetEnable(t_StVppCtx *pstCtx, uint32_t u32En)
{
    if (!pstCtx || !pstCtx->pstStats)
        return VPP_ERR_PARAM;

    pstCtx->pstStats->u32En = u32En;

    return VPP_OK;
}

uint32_t u32VppStats_Register(t_StVppCtx *pstCtx,
                              const t_StVppStatsConfig *pstStatsConfig,
                              uint32_t u32ConfigCnt,
                              void **ppv)
{
    uint32_t u32Err = VPP_OK;
    uint32_t i;
    t_StVppStatsCb *pstCb;
    t_StVppStatBlock *pstStatBlk;

    // Set this first, if possible, because it is possible that the top level
    // stats init has not been called. In this case, when the client calls
    // start/stop, a valid handle will not be detected and we can safely return
    // from the call.
    if (ppv)
        *ppv = NULL;

    if (!pstCtx || !pstCtx->pstStats)
        return VPP_ERR_PARAM;

    if (!ppv || !pstStatsConfig || !u32ConfigCnt)
        return VPP_ERR_PARAM;

    pstCb = pstCtx->pstStats;

    pstStatBlk = calloc(1, sizeof(t_StVppStatBlock));
    if (!pstStatBlk)
        return VPP_ERR_NO_MEM;

    pstStatBlk->pstCtx = pstCtx;
    pstStatBlk->u32StatCnt = u32ConfigCnt;
    pstStatBlk->pstStat = calloc(u32ConfigCnt, sizeof(t_StVppStat));
    if (!pstStatBlk->pstStat)
    {
        LOGE("ERROR: unable to malloc stats list of cnt=%u", u32ConfigCnt);
        u32Err = VPP_ERR_NO_MEM;
        goto ERR_CALLOC_STATS;
    }

    for (i = 0; i < u32ConfigCnt; i++)
        pstStatBlk->pstStat[i].stCfg = pstStatsConfig[i];

    pthread_mutex_lock(&pstCb->mutex);
    if ((u32Err = u32VppStats_InsertBlock(pstCb, pstStatBlk)) != VPP_OK)
    {
        LOGE("ERROR: unable to insert stats block, pstCb=%p, pstStats=%p, u32=%u",
             pstCb, pstStatBlk, u32Err);
        pthread_mutex_unlock(&pstCb->mutex);
        goto ERR_INSERT;
    }
    pthread_mutex_unlock(&pstCb->mutex);

    *ppv = pstStatBlk;
    return u32Err;

ERR_INSERT:
    free(pstStatBlk->pstStat);

ERR_CALLOC_STATS:
    free(pstStatBlk);
    return u32Err;
}

uint32_t u32VppStats_Unregister(t_StVppCtx *pstCtx, void *pvHdl)
{
    uint32_t u32;
    t_StVppStatsCb *pstCb;
    t_StVppStatBlock *pstStatBlk, *pstTmp;

    if (!bIsHandleValid(pstCtx, pvHdl))
        return VPP_ERR_PARAM;

    pstCb = pstCtx->pstStats;
    pstStatBlk = (t_StVppStatBlock *)pvHdl;

    if (!pstCb->u32BlockCnt)
        return VPP_ERR;

    pthread_mutex_lock(&pstCb->mutex);
    if ((u32 = u32VppStats_RemoveBlock(pstCb, pstStatBlk)) != VPP_OK)
    {
        // If we were unable to remove if from the list, it is likely that it
        // has already been in the list and that this is a stale pointer...
        // in which case, we probably shouldn't try to free it
        LOGE("ERROR: unable to remove block from list");
        pthread_mutex_unlock(&pstCb->mutex);
        return VPP_ERR;
    }
    pthread_mutex_unlock(&pstCb->mutex);

    // Explicitly set these so IsHandleValid() will fail if called again (even
    // though the following will free the memory, it could still not cause a
    // segfault)
    pstStatBlk->pstCtx = NULL;
    pstStatBlk->u32StatCnt = 0;

    free(pstStatBlk->pstStat);
    free(pstStatBlk);

    return VPP_OK;
}

uint32_t u32VppStats_Start(t_StVppCtx *pstCtx, void *pvHdl, uint32_t u32Key)
{
    t_StVppStat *pstStat;
    t_StVppStatsCb *pstCb;
    t_StVppStatBlock *pstStatBlk;

    if (!pstCtx || !pstCtx->pstStats || !pvHdl)
        return VPP_ERR_PARAM;

    if (!bIsHandleValid(pstCtx, pvHdl))
        return VPP_ERR;

    if (!pstCtx->pstStats->u32En)
        return VPP_OK;

    pstStatBlk = (t_StVppStatBlock *)pvHdl;

    pstStat = pstVppStats_FindStat(pstStatBlk, u32Key);
    if (!pstStat)
        return VPP_ERR;

    if (pstStat->stCfg.bEnabled)
        gettimeofday(&pstStat->tv_start, NULL);

    return VPP_OK;
}

uint32_t u32VppStats_Stop(t_StVppCtx *pstCtx, void *pvHdl, uint32_t u32Key)
{
    uint32_t u32Diff;
    t_StVppStat *pstStat;
    t_StVppStatsCb *pstCb;
    t_StVppStatBlock *pstStatBlk;

    if (!pstCtx || !pstCtx->pstStats || !pvHdl)
        return VPP_ERR_PARAM;

    if (!bIsHandleValid(pstCtx, pvHdl))
        return VPP_ERR;

    if (!pstCtx->pstStats->u32En)
        return VPP_OK;

    pstStatBlk = (t_StVppStatBlock *)pvHdl;

    pstStat = pstVppStats_FindStat(pstStatBlk, u32Key);
    if (!pstStat)
        return VPP_ERR;

    if (!pstStat->stCfg.bEnabled)
        return VPP_OK;

    gettimeofday(&pstStat->tv_end, NULL);

    u32Diff = u32VppUtils_GetTvDiff(&pstStat->tv_end, &pstStat->tv_start);

    pstStat->u32TotalTime += u32Diff;
    pstStat->u32Cnt += 1;

    if (u32Diff > pstStat->u32Max)
        pstStat->u32Max = u32Diff;

    if (!pstStat->u32Min || u32Diff < pstStat->u32Min)
        pstStat->u32Min = u32Diff;

    if (pstStat->u32Cnt % pstStat->stCfg.u32Freq == 0)
    {
        pstStat->u32Average = pstStat->u32TotalTime / pstStat->u32Cnt;

        VPP_LOG_PROF("PROF: %s -> cnt=%u, diff=%u, avg=%u, min=%u, max=%u\n",
                     pstStat->stCfg.pcName, pstStat->u32Cnt, u32Diff,
                     pstStat->u32Average, pstStat->u32Min, pstStat->u32Max);
    }

    return VPP_OK;
}

uint32_t u32VppStats_ResetBlock(t_StVppCtx *pstCtx, void *pvHdl)
{
    uint32_t i;
    t_StVppStat *pstStat;
    t_StVppStatBlock *pstStatBlk;

    if (!pstCtx || !pstCtx->pstStats || !pvHdl)
        return VPP_ERR_PARAM;

    if (!bIsHandleValid(pstCtx, pvHdl))
        return VPP_ERR;

    pstStatBlk = (t_StVppStatBlock *)pvHdl;

    for (i = 0; i < pstStatBlk->u32StatCnt; i++)
    {
        pstStat = &pstStatBlk->pstStat[i];
        pstStat->u32Cnt = 0;
        pstStat->u32TotalTime = 0;
        pstStat->u32Average = 0;
        pstStat->u32Max = 0;
        pstStat->u32Min = 0;
        pstStat->u32Cnt = 0;
    }

    return VPP_OK;
}

uint32_t u32VppStats_ResetStat(t_StVppCtx *pstCtx, void *pvHdl, uint32_t u32Key)
{
    t_StVppStat *pstStat;
    t_StVppStatBlock *pstStatBlk;

    if (!pstCtx || !pstCtx->pstStats || !pvHdl)
        return VPP_ERR_PARAM;

    if (!bIsHandleValid(pstCtx, pvHdl))
        return VPP_ERR;

    pstStatBlk = (t_StVppStatBlock *)pvHdl;

    pstStat = pstVppStats_FindStat(pstStatBlk, u32Key);
    if (!pstStat)
        return VPP_ERR;

    pstStat->u32Cnt = 0;
    pstStat->u32TotalTime = 0;
    pstStat->u32Average = 0;
    pstStat->u32Max = 0;
    pstStat->u32Min = 0;
    pstStat->u32Cnt = 0;

    return VPP_OK;
}
