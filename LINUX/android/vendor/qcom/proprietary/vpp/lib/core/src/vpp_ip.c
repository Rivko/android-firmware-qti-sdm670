/*!
 * @file vpp_ip.c
 *
 * @cr
 * Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services
 */

#include <sys/types.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#include "vpp_dbg.h"
#include "vpp_queue.h"
#include "vpp_reg.h"
#include "vpp_ip.h"
#include "vpp_utils.h"

/************************************************************************
 * Local definitions
 ***********************************************************************/
#define IF_STR(type, str, u32)  if ((type)u32 == str) return (char *)#str

/************************************************************************
 * Local static variables
 ***********************************************************************/

/************************************************************************
 * Forward Declarations
 ************************************************************************/

/************************************************************************
 * Local Functions
 ***********************************************************************/

/************************************************************************
 * Global Functions
 ***********************************************************************/

uint32_t u32VppIp_ValidateCallbacks(t_StVppCallback *pstCbs)
{
    VPP_RET_IF_NULL(pstCbs, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pstCbs->input_buffer_done, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pstCbs->output_buffer_done, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pstCbs->event, VPP_ERR_PARAM);
    return VPP_OK;
}

uint32_t u32VppIp_SupportsAlgo(const t_StVppIpBlock *pstIp,
                               enum hqv_control_type type)
{
    uint32_t i;
    for (i = 0; i < pstIp->algo_cap.u32AlgoCnt; i++)
    {
        if (pstIp->algo_cap.algos[i].ctrl == type)
            return VPP_TRUE;
    }
    return VPP_FALSE;
}

uint32_t u32VppIp_GetRequiredMetadata(t_StVppIpBlock *pstIp,
                                      enum hqv_control_type ctrl,
                                      uint32_t *o_pu32Idx,
                                      uint32_t **o_peMeta)
{
    uint32_t i;

    if (!pstIp)
        return VPP_ERR_PARAM;

    for (i = 0; i < pstIp->algo_cap.u32AlgoCnt; i++)
    {
        if (pstIp->algo_cap.algos[i].ctrl == ctrl)
        {
            *o_pu32Idx = pstIp->algo_cap.algos[i].u32MetaCnt;
            *o_peMeta = pstIp->algo_cap.algos[i].meta;
            break;
        }
    }
    return VPP_OK;
}

const char *vVppIp_GetStateStr(uint32_t u32)
{
    IF_STR(t_EVppIpState, VPP_IP_STATE_NULL, u32);
    IF_STR(t_EVppIpState, VPP_IP_STATE_INITED, u32);
    IF_STR(t_EVppIpState, VPP_IP_STATE_ACTIVE, u32);
    IF_STR(t_EVppIpState, VPP_IP_STATE_OPENING, u32);
    IF_STR(t_EVppIpState, VPP_IP_STATE_CLOSING, u32);
    IF_STR(t_EVppIpState, VPP_IP_STATE_MAX, u32);
    return "";
}

void vVppIp_LogCmd(const char *pc, t_EVppIpCmd eCmd)
{
    switch (eCmd)
    {
        LOG_CASE(pc, VPP_IP_CMD_NULL);
        LOG_CASE(pc, VPP_IP_CMD_THREAD_EXIT);
        LOG_CASE(pc, VPP_IP_CMD_INIT);
        LOG_CASE(pc, VPP_IP_CMD_TERM);
        LOG_CASE(pc, VPP_IP_CMD_OPEN);
        LOG_CASE(pc, VPP_IP_CMD_CLOSE);
        LOG_CASE(pc, VPP_IP_CMD_FLUSH);
        LOG_CASE(pc, VPP_IP_CMD_DRAIN);
        LOG_CASE(pc, VPP_IP_CMD_STOP);
        LOG_CASE(pc, VPP_IP_CMD_UPDATE_GLOBAL_PARAM);
        LOG_CASE(pc, VPP_IP_CMD_RECONFIGURE);
        LOG_CASE(pc, VPP_IP_CMD_MAX);
    }
}

uint32_t u32VppIp_CmdQueueInit(t_StVppIpCmdQueue *pstQ, uint32_t u32Cnt)
{
    int rc;
    uint32_t u32Err = VPP_OK;

    if (!pstQ || !u32Cnt)
        return VPP_ERR_PARAM;

    pstQ->u32Cnt = u32Cnt;
    pstQ->pstCmdNode = calloc(u32Cnt, sizeof(t_StVppIpCmd));
    if (!pstQ->pstCmdNode)
    {
        LOGE("ERROR: unable to allocate queue node memory for q=%p", pstQ);
        u32Err = VPP_ERR_NO_MEM;
        goto ERR_CALLOC;
    }

    rc = pthread_mutex_init(&pstQ->mutex, NULL);
    if (rc)
    {
        LOGE("ERROR: unable to initialize mutex for cmd_q=%p", pstQ);
        u32Err = VPP_ERR;
        goto ERR_MUTEX_INIT;
    }

    rc = pthread_cond_init(&pstQ->cond, NULL);
    if (rc)
    {
        LOGE("ERROR: unable to initialize cond for cmd_q=%p", pstQ);
        u32Err = VPP_ERR;
        goto ERR_COND_INIT;
    }

    rc = vpp_queue_init(&pstQ->stCmdQ, u32Cnt);
    if (rc != VPP_OK)
    {
        LOGE("ERROR: unable to initialize queue for cmd_q=%p", pstQ);
        u32Err = VPP_ERR;
        goto ERR_QUEUE_INIT;
    }

    pstQ->bThreadSafe = VPP_TRUE;
    return VPP_OK;

ERR_QUEUE_INIT:
    pthread_cond_destroy(&pstQ->cond);

ERR_COND_INIT:
    pthread_mutex_destroy(&pstQ->mutex);

ERR_MUTEX_INIT:
    free(pstQ->pstCmdNode);
    pstQ->pstCmdNode = NULL;

ERR_CALLOC:
    return u32Err;
}

uint32_t u32VppIp_CmdQueueTerm(t_StVppIpCmdQueue *pstQ)
{
    int rc;

    if (!pstQ)
        return VPP_ERR_PARAM;

    if (!pstQ->bThreadSafe)
    {
        LOGE("non thread safe queue passed to term");
        return VPP_ERR_PARAM;
    }

    rc = vpp_queue_term(&pstQ->stCmdQ);
    if (rc)
        LOGE("ERROR: unable to terminate Q, rc=%d", rc);

    rc = pthread_cond_destroy(&pstQ->cond);
    if (rc)
        LOGE("ERROR: unable to destroy cond, rc=%d, err=%s", rc, strerror(rc));

    rc = pthread_mutex_destroy(&pstQ->mutex);
    if (rc)
        LOGE("ERROR: unable to destroy mutex, rc=%d, err=%s", rc, strerror(rc));

    if (pstQ->pstCmdNode)
        free(pstQ->pstCmdNode);

    pstQ->pstCmdNode = NULL;
    pstQ->u32Cnt = 0;

    return VPP_OK;
}

uint32_t u32VppIp_CmdGet(t_StVppIpCmdQueue *pstQ, t_StVppIpCmd *pstCmd,
                         uint32_t u32Blocking)
{
    int32_t idx;

    if (!pstQ || !pstCmd)
        return VPP_ERR_PARAM;

    if (!pstQ->bThreadSafe)
    {
        LOGE("non thread safe queue passed to get");
        return VPP_ERR_PARAM;
    }

    pthread_mutex_lock(&pstQ->mutex);

    if (u32Blocking)
    {
        while ((idx = vpp_queue_dequeue(&pstQ->stCmdQ)) < 0)
            pthread_cond_wait(&pstQ->cond, &pstQ->mutex);
    }
    else
    {
        idx = vpp_queue_dequeue(&pstQ->stCmdQ);
        if (idx < 0)
        {
            pthread_mutex_unlock(&pstQ->mutex);
            return VPP_ERR;
        }
    }

    *pstCmd = pstQ->pstCmdNode[idx];
    LOG_CMD("GetCmd", pstCmd->eCmd);

    pthread_mutex_unlock(&pstQ->mutex);
    return VPP_OK;
}

uint32_t u32VppIp_CmdPut(t_StVppIpCmdQueue *pstQ, t_StVppIpCmd stCmd)
{
    int32_t idx;
    uint32_t u32Ret = VPP_OK;

    if (!pstQ)
        return VPP_ERR_PARAM;

    if (!pstQ->bThreadSafe)
    {
        LOGE("non thread safe queue passed to put");
        return VPP_ERR_PARAM;
    }

    pthread_mutex_lock(&pstQ->mutex);

    LOG_CMD("InsertCmd", stCmd.eCmd);
    idx = vpp_queue_enqueue(&pstQ->stCmdQ);
    if (idx < 0)
    {
        u32Ret = VPP_ERR;
    }
    else
    {
        pstQ->pstCmdNode[idx] = stCmd;
    }

    pthread_mutex_unlock(&pstQ->mutex);
    pthread_cond_signal(&pstQ->cond);

    return u32Ret;
}

uint32_t u32VppIp_CmdCnt(t_StVppIpCmdQueue *pstQ)
{
    uint32_t u32Cnt = 0;

    if (!pstQ)
        return 0;

    if (!pstQ->bThreadSafe)
    {
        LOGE("non thread safe queue passed to cnt");
        return VPP_ERR_PARAM;
    }

    pthread_mutex_lock(&pstQ->mutex);
    u32Cnt = vpp_queue_count(&pstQ->stCmdQ);
    pthread_mutex_unlock(&pstQ->mutex);

    return u32Cnt;
}

uint32_t u32VppIp_NtsCmdQueueInit(t_StVppIpCmdQueue *pstQ, uint32_t u32Cnt)
{
    int rc;
    uint32_t u32Err = VPP_OK;

    if (!pstQ || !u32Cnt)
        return VPP_ERR_PARAM;

    pstQ->u32Cnt = u32Cnt;
    pstQ->pstCmdNode = calloc(u32Cnt, sizeof(t_StVppIpCmd));
    if (!pstQ->pstCmdNode)
    {
        LOGE("ERROR: unable to allocate queue node memory for q=%p", pstQ);
        u32Err = VPP_ERR_NO_MEM;
        goto ERR_CALLOC;
    }

    rc = vpp_queue_init(&pstQ->stCmdQ, u32Cnt);
    if (rc != VPP_OK)
    {
        LOGE("ERROR: unable to initialize queue for cmd_q=%p", pstQ);
        u32Err = VPP_ERR;
        goto ERR_QUEUE_INIT;
    }

    pstQ->bThreadSafe = VPP_FALSE;
    return VPP_OK;

ERR_QUEUE_INIT:
    free(pstQ->pstCmdNode);
    pstQ->pstCmdNode = NULL;

ERR_CALLOC:
    return u32Err;
}

uint32_t u32VppIp_NtsCmdQueueTerm(t_StVppIpCmdQueue *pstQ)
{
    int rc;

    if (!pstQ)
        return VPP_ERR_PARAM;

    rc = vpp_queue_term(&pstQ->stCmdQ);
    if (rc)
        LOGE("ERROR: unable to terminate Q, rc=%d", rc);

    if (pstQ->pstCmdNode)
        free(pstQ->pstCmdNode);

    pstQ->pstCmdNode = NULL;
    pstQ->u32Cnt = 0;

    return VPP_OK;
}

uint32_t u32VppIp_NtsCmdGet(t_StVppIpCmdQueue *pstQ, t_StVppIpCmd *pstCmd,
                            pthread_mutex_t *pstMutex)
{
    int32_t idx;

    if (!pstQ || !pstCmd)
        return VPP_ERR_PARAM;

    if (pstMutex)
        pthread_mutex_lock(pstMutex);

    idx = vpp_queue_dequeue(&pstQ->stCmdQ);
    if (idx < 0)
    {
        if (pstMutex)
            pthread_mutex_unlock(pstMutex);

        return VPP_ERR;
    }

    *pstCmd = pstQ->pstCmdNode[idx];
    LOG_CMD("GetCmd", pstCmd->eCmd);

    if (pstMutex)
        pthread_mutex_unlock(pstMutex);

    return VPP_OK;
}

uint32_t u32VppIp_NtsCmdPut(t_StVppIpCmdQueue *pstQ, t_StVppIpCmd stCmd,
                            pthread_mutex_t *pstMutex, pthread_cond_t *pstCond)
{
    int32_t idx;
    uint32_t u32Ret = VPP_OK;

    if (!pstQ)
        return VPP_ERR_PARAM;

    if (pstMutex)
        pthread_mutex_lock(pstMutex);

    LOG_CMD("InsertCmd", stCmd.eCmd);
    idx = vpp_queue_enqueue(&pstQ->stCmdQ);
    if (idx < 0)
    {
        u32Ret = VPP_ERR;
    }
    else
    {
        pstQ->pstCmdNode[idx] = stCmd;
    }

    if (pstMutex)
        pthread_mutex_unlock(pstMutex);

    if (pstCond)
        pthread_cond_signal(pstCond);

    return u32Ret;
}

uint32_t u32VppIp_NtsCmdCnt(t_StVppIpCmdQueue *pstQ, pthread_mutex_t *pstMutex)
{
    uint32_t u32Cnt = 0;

    if (!pstQ)
        return 0;

    if (pstMutex)
        pthread_mutex_lock(pstMutex);

    u32Cnt = vpp_queue_count(&pstQ->stCmdQ);

    if (pstMutex)
        pthread_mutex_unlock(pstMutex);

    return u32Cnt;
}

uint32_t u32VppIp_PortInit(t_StVppIpPort *pstPort)
{
    VPP_RET_IF_NULL(pstPort, VPP_ERR_PARAM);
    memset(pstPort, 0, sizeof(t_StVppIpPort));
    return u32VppBufPool_Init(&pstPort->stPendingQ);
}

uint32_t u32VppIp_PortTerm(t_StVppIpPort *pstPort)
{
    VPP_RET_IF_NULL(pstPort, VPP_ERR_PARAM);
    return u32VppBufPool_Term(&pstPort->stPendingQ);
}

uint32_t u32VppIp_PortBufGet(t_StVppIpPort *pstPort, t_StVppBuf **ppstBuf,
                             pthread_mutex_t *pMutex)
{
    uint32_t u32Ret = VPP_OK;

    VPP_RET_IF_NULL(pstPort, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(ppstBuf, VPP_ERR_PARAM);

    if (pMutex)
        pthread_mutex_lock(pMutex);

    *ppstBuf = pstVppBufPool_Get(&pstPort->stPendingQ);

    if (*ppstBuf == NULL)
    {
        LOGD("%s: PortBufGet didn't return a buffer", __func__);
        u32Ret = VPP_ERR;
    }

    if (pMutex)
        pthread_mutex_unlock(pMutex);

    return u32Ret;
}

uint32_t u32VppIp_PortBufGetMatching(t_StVppIpPort *pstPort, t_StVppBuf **ppstBuf,
                                     pthread_mutex_t *pMutex, fpBufMatchPred pred,
                                     void *pv)
{
    VPP_RET_IF_NULL(pstPort, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(ppstBuf, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pred, VPP_ERR_PARAM);

    if (pMutex)
        pthread_mutex_lock(pMutex);

    *ppstBuf = pstVppBufPool_RemoveMatching(&pstPort->stPendingQ, pred, pv);

    if (pMutex)
        pthread_mutex_unlock(pMutex);

    if (*ppstBuf)
        return VPP_OK;

    return VPP_ERR;
}

uint32_t u32VppIp_PortBufPut(t_StVppIpPort *pstPort, t_StVppBuf *pstBuf,
                             pthread_mutex_t *pMutex, pthread_cond_t *pCond)
{
    uint32_t u32;
    uint32_t u32Ret = VPP_OK;

    VPP_RET_IF_NULL(pstPort, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pstBuf, VPP_ERR_PARAM);

    if (pMutex)
        pthread_mutex_lock(pMutex);

    u32 = u32VppBufPool_Put(&pstPort->stPendingQ, pstBuf);
    if (u32 != VPP_OK)
    {
        LOGE("ERROR: u32VppBufPool_Put() failed, u32=%u", u32);
        u32Ret = VPP_ERR;
    }
    else
    {
        if (pCond)
            pthread_cond_signal(pCond);
    }

    if (pMutex)
        pthread_mutex_unlock(pMutex);

    return u32Ret;
}

uint32_t u32VppIp_PortBufPutFront(t_StVppIpPort *pstPort, t_StVppBuf *pstBuf,
                                  pthread_mutex_t *pMutex, pthread_cond_t *pCond)
{
    uint32_t u32;
    uint32_t u32Ret = VPP_OK;

    VPP_RET_IF_NULL(pstPort, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pstBuf, VPP_ERR_PARAM);

    if (pMutex)
        pthread_mutex_lock(pMutex);

    u32 = u32VppBufPool_PutFront(&pstPort->stPendingQ, pstBuf);
    if (u32 != VPP_OK)
    {
        LOGE("ERROR: u32VppBufPool_PutFront() failed, u32=%u", u32);
        u32Ret = VPP_ERR;
    }
    else
    {
        if (pCond)
            pthread_cond_signal(pCond);
    }

    if (pMutex)
        pthread_mutex_unlock(pMutex);

    return u32Ret;
}

t_StVppBuf *pstVppIp_PortBufPeek(t_StVppIpPort *pstPort, uint32_t index,
                                 pthread_mutex_t *pMutex)
{
    t_StVppBuf *pstBuf = NULL;

    if (!pstPort)
        return NULL;

    if (pMutex)
        pthread_mutex_lock(pMutex);

    pstBuf = pstVppBufPool_Peek(&pstPort->stPendingQ, index);

    if (pMutex)
        pthread_mutex_unlock(pMutex);

    return pstBuf;
}

uint32_t u32VppIp_SetBase(t_StVppCtx *pstCtx, uint32_t u32Flags,
                          t_StVppCallback cbs, t_StVppIpBase *pstBase)
{
    if (!pstCtx || !pstBase)
        return VPP_ERR_PARAM;

    memset(pstBase, 0, sizeof(t_StVppIpBase));

    pstBase->pstCtx = pstCtx;
    pstBase->u32Flags = u32Flags;
    pstBase->stCb = cbs;

    if (VPP_FLAG_IS_SET(u32Flags, VPP_SESSION_SECURE))
    {
        LOGD("Secure session requested for pstCtx=%p", pstCtx);
        pstBase->bSecureSession = VPP_TRUE;
    }

    return VPP_OK;
}
