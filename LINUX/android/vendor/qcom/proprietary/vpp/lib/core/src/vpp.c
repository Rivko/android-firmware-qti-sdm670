/*!
 * @file vpp.c
 *
 * @cr
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services    Implements the VPP library interface
 */

#include <sys/types.h>
#include <cutils/properties.h>
#include <stdlib.h>
#include <string.h>

#define VPP_LOG_TAG     VPP_LOG_MODULE_CORE_TAG
#define VPP_LOG_MODULE  VPP_LOG_MODULE_CORE
#include "vpp.h"
#include "vpp_dbg.h"
#include "vpp_utils.h"
#include "vpp_ion.h"

#include "vpp_ctx.h"
#include "vpp_stats.h"
#include "vpp_core.h"
#include "vpp_uc.h"
#include "vpp_pipeline.h"

/************************************************************************
 * Local definitions
 ***********************************************************************/
#define VPP_CTX_GET(ctx) (ctx ? (t_StVppCtx *)ctx : NULL)
#define VPP_CB_GET(ctx) (ctx ? ((t_StVppCtx *)ctx)->pstVppCb : NULL)

#define VPP_STATE_EQUAL(cb, state) ((uint32_t)(cb->eState == state))

#define ABS_DIFF(a, b) (((a) <= (b)) ? ((b) - (a)) : ((a) - (b)))
#define US_TO_FPS_NUMERATOR 1000000

#define VPP_LOG_LVL_PIPELINE     VPP_LOG_LVL_ERROR
#define VPP_LOG_LVL_GPU          VPP_LOG_LVL_ERROR
#define VPP_LOG_LVL_HVX          VPP_LOG_LVL_ERROR
#define VPP_LOG_LVL_ION          VPP_LOG_LVL_ERROR
#define VPP_LOG_LVL_BUF          VPP_LOG_LVL_ERROR
#define VPP_LOG_LVL_CB           VPP_LOG_LVL_ERROR
#define VPP_LOG_LVL_CORE         VPP_LOG_LVL_ERROR
#define VPP_LOG_LVL_UC           VPP_LOG_LVL_ERROR
#define VPP_LOG_LVL_FRC_ME       VPP_LOG_LVL_ERROR
#define VPP_LOG_LVL_FRC_MC       VPP_LOG_LVL_ERROR
#define VPP_LOG_LVL_FRC_CORE     VPP_LOG_LVL_ERROR
#define VPP_LOG_LVL_OTHER        VPP_LOG_LVL_ERROR
#define VPP_LOG_LVL_HCP          VPP_LOG_LVL_ERROR

#define VPP_LOG(which) (VPP_LOG_LVL_##which << (VPP_LOG_FLD_WID*VPP_LOG_MODULE_##which))
#define VPP_LOG_DEFAULT (VPP_LOG(PIPELINE) | VPP_LOG(GPU) | VPP_LOG(HVX) | \
                         VPP_LOG(ION) | VPP_LOG(BUF) | VPP_LOG(CB) | \
                         VPP_LOG(CORE) | VPP_LOG(OTHER) | VPP_LOG(FRC_ME) | \
                         VPP_LOG(FRC_MC) | VPP_LOG(FRC_CORE) | VPP_LOG(HCP))

typedef struct StVppSession {
    t_StVppCtx *pstCtx;
    struct StVppSession *pstNext;
} t_StVppSession;

typedef struct {
    pthread_mutex_t mutex;
    t_StVppSession *pstHead;
    uint32_t bBooted;
} t_StVppGlobalCb;

/************************************************************************
 * Local static variables
 ***********************************************************************/
uint64_t u64LogLevel = VPP_LOG_DEFAULT;

static t_StVppGlobalCb stVppGlobal = {
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .pstHead = NULL,
    .bBooted = VPP_FALSE,
};

/************************************************************************
 * Forward Declarations
 ************************************************************************/
#ifdef VPP_CRC_BUF_ENABLE
#define VPP_CRC_BUF(_pstCb,_pBuf) u32Vpp_CrcBuffer(_pBuf, eVppBuf_Pixel,\
                                 (_pBuf->stPixel.u32FilledLen / 2),\
                                 (_pBuf->stPixel.u32FilledLen / 8),\
                                 _pstCb->VPP_STAT_CB_NM.u32QInputBuf, "VPP_CRC_BUF")
#else
#define VPP_CRC_BUF(_pstCb,_pBuf)
#endif

/************************************************************************
 * Local Functions
 ***********************************************************************/
void vVpp_StatsDump(t_StVppCb *pstCb)
{
    LOGD("###### Dumping Stats ######");
    VPP_STAT_PRINT(pstCb, QInputBuf);
    VPP_STAT_PRINT(pstCb, QOutputBuf);
    VPP_STAT_PRINT(pstCb, IBD);
    VPP_STAT_PRINT(pstCb, OBD);
    LOGD("###########################");
}

void vVpp_DumpControl(struct hqv_control ctrl)
{
    LOGI("mode=%u, ctrl_type=%u", ctrl.mode, ctrl.ctrl_type);
    LOG_CADE(I, &ctrl.cade);
    LOG_TNR(I, &ctrl.tnr);
    LOG_CNR(I, &ctrl.cnr);
    LOG_AIE(I, &ctrl.aie);
    LOG_DI(I, &ctrl.di);
    LOG_FRC(I, &ctrl.frc);
    LOG_EAR(D, &ctrl.ear);
    LOG_QBR(D, &ctrl.qbr);
}

static void vVpp_SetState(t_StVppCb *pstCb, t_EVppState eState)
{
    LOGI("State Change: old=%u, new=%u", pstCb->eState, eState);
    pstCb->eState = eState;
}

static void vVpp_InputBufferDone(void *pv, t_StVppBuf *pstBuf)
{
    uint32_t u32;
    struct vpp_buffer *pVppExtBuf;

    t_StVppCtx *pstCtx;
    t_StVppCb *pstCb;

    pstCtx = VPP_CTX_GET(pv);
    VPP_RET_VOID_IF_NULL(pstCtx);

    pstCb = VPP_CB_GET(pstCtx);
    VPP_RET_VOID_IF_NULL(pstCb);

    VPP_STATS(pstCb, IBD);

    pstBuf->eReturnPort = VPP_PORT_INPUT;

    u32 = u32VppBuf_InternalPut(pstCtx, pstBuf, &pVppExtBuf, VPP_FALSE);
    if (u32 != VPP_OK)
        return;

    if (pstCb->stCb.input_buffer_done)
        pstCb->stCb.input_buffer_done(pstCb->stCb.pv, pVppExtBuf);

    LOGI("Vpp input_buffer_done, pstBuf=%p, pBuf=%p", pstBuf, pstBuf->pBuf);
}

static void vVpp_OutputBufferDone(void *pv, t_StVppBuf *pstBuf)
{
    uint32_t u32;
    struct vpp_buffer *pVppExtBuf;

    t_StVppCtx *pstCtx;
    t_StVppCb *pstCb;

    pstCtx = VPP_CTX_GET(pv);
    VPP_RET_VOID_IF_NULL(pstCtx);

    pstCb = VPP_CB_GET(pstCtx);
    VPP_RET_VOID_IF_NULL(pstCb);

    VPP_STATS(pstCb, OBD);

    pstBuf->eReturnPort = VPP_PORT_OUTPUT;

    u32 = u32VppBuf_InternalPut(pstCtx, pstBuf, &pVppExtBuf, VPP_TRUE);
    if (u32 != VPP_OK)
        return;

    if (pstCb->stCb.output_buffer_done)
        pstCb->stCb.output_buffer_done(pstCb->stCb.pv, pVppExtBuf);
    LOGI("Vpp output_buffer_done, pstBuf=%p, pBuf=%p", pstBuf, pstBuf->pBuf);
}

static void vVpp_FramerateReset(t_StVppFrameRate *pstFrameRate)
{
    if (!pstFrameRate)
        return;

    memset(pstFrameRate, 0, sizeof(t_StVppFrameRate));
}

static void vVpp_FramerateUpdate(void *ctx, uint32_t u32Timestamp, uint32_t u32Flags,
                                 t_StVppBuf *pstIntBuf)
{
    uint32_t u32FrameRate, u32TempIndex, u32PrevTS, u32PrevFR, u32Index, u32Count, u32Temp;
    uint32_t i, u32NewFrameRate;
    uint32_t u32AvgFrameRate = 0;
    t_StVppFrameRate *pstFrameRate;
    t_StVppUsecase *pstUc;
    t_StVppCb *pstCb;
    t_EVppBufType ePrevBufType;
    struct video_property vid_prop;
    uint32_t u32Ret = VPP_OK;

    if (!ctx)
        return;

    pstCb = VPP_CB_GET(ctx);
    pstFrameRate = &pstCb->stFrameRate;
    if (pstCb->pstUcFb == NULL)
        pstUc = pstCb->pstUc;
    else
        pstUc = pstCb->pstUcFb;

    if (!pstUc)
        return;

    if (u32Flags & VPP_BUFFER_FLAG_EOS)
        vVpp_FramerateReset(pstFrameRate);

    u32PrevTS = pstFrameRate->u32PrevTimestamp;
    u32PrevFR = pstFrameRate->u32PrevFrameRate;
    u32Index = pstFrameRate->u32Index;
    u32Count = pstFrameRate->u32Entries;
    ePrevBufType = pstFrameRate->ePrevBufType;
    pstFrameRate->ePrevBufType = pstIntBuf->eBufType;

    if ((u32Timestamp > u32PrevTS) && (u32Count > 0))
        u32FrameRate = US_TO_FPS_NUMERATOR / (u32Timestamp - u32PrevTS);
    else
        u32FrameRate = u32VppUsecase_GetDefaultFps(pstUc); // 1st frame or timestamp didn't increase

    u32FrameRate = RANGE_CHECK(u32FrameRate, VPP_MIN_FRAME_RATE, VPP_MAX_FRAME_RATE);

    pstFrameRate->u32PrevFrameRate = u32FrameRate;

    if (u32Count > 0)
    {
        if (u32Index == 0)
            u32TempIndex = VPP_FRAMERATE_HISTO_SZ - 1;
        else
            u32TempIndex = u32Index - 1;

        if (u32Count == 1)
        {
            // First time a real frame rate is calculated. Replace default one.
            pstFrameRate->u32aFrameRates[u32TempIndex] = u32FrameRate;
        }
        for (i = 0; i < u32Count; i++)
        {
            u32AvgFrameRate += pstFrameRate->u32aFrameRates[u32TempIndex];
            if (u32TempIndex == 0)
                u32TempIndex = VPP_FRAMERATE_HISTO_SZ - 1;
            else
                u32TempIndex--;
        }

        u32Temp = u32AvgFrameRate / u32Count;

        if (((ABS_DIFF(u32FrameRate, u32PrevFR)) > VPP_FRAME_RATE_DELTA_MAX) &&
            ((ABS_DIFF(u32FrameRate, u32Temp)) > VPP_FRAME_RATE_DELTA_MAX))
        {
            // Could be a glitch.  Do weighted average with existing values.
            u32FrameRate = (u32AvgFrameRate + u32FrameRate) / (u32Count + 1);
        }
    }


    u32AvgFrameRate = (u32AvgFrameRate + u32FrameRate) / (u32Count + 1);


    pstFrameRate->u32aFrameRates[u32Index] = u32FrameRate;
    pstFrameRate->u32PrevTimestamp = u32Timestamp;
    pstFrameRate->u32Index++;
    if (pstFrameRate->u32Index >= VPP_FRAMERATE_HISTO_SZ)
        pstFrameRate->u32Index = 0;
    if (pstFrameRate->u32Entries < VPP_FRAMERATE_HISTO_SZ)
        pstFrameRate->u32Entries++;

    pstFrameRate->u32AvgFrameRate = u32AvgFrameRate;

    vid_prop.property_type = VID_PROP_NON_REALTIME;
    u32VppUtils_GetVidProp(ctx, &vid_prop);
    if (vid_prop.non_realtime.bNonRealtime)
    {
        // Non-Realtime session, ignore calculated framerate and force default
        u32NewFrameRate = u32VppUsecase_GetDefaultFps(pstUc);
        u32Count++;
    }
    else
    {
        vid_prop.property_type = VID_PROP_OPERATING_RATE;
        u32VppUtils_GetVidProp(ctx, &vid_prop);
        if (vid_prop.operating_rate.u32OperatingRate)
        {
            // Realtime session with client set framerate. Ignore calculated and use client value
            u32NewFrameRate = vid_prop.operating_rate.u32OperatingRate;
            u32Count++;
        }
        else
            u32NewFrameRate = u32AvgFrameRate; // Realtime session, use calculated framerate
    }
    if ((((ABS_DIFF(u32NewFrameRate, pstFrameRate->u32ProgrammedFrameRate)) >=
        VPP_FRAME_RATE_UPDATE_THRESHOLD) || (ePrevBufType != pstIntBuf->eBufType))
        && (u32Count > 0))
    {
        u32Ret = u32VppUsecase_UpdateFramerate(ctx, pstUc, u32NewFrameRate, pstIntBuf->eBufType);
        if (u32Ret != VPP_OK)
        {
            pstFrameRate->u32Bypass = 1;
            LOGD("New frame rate %dFPS not supported. In bypass.", u32NewFrameRate);
        }
        else
        {
            pstFrameRate->u32Bypass = 0;
            LOGD("New frame rate %dFPS supported.", u32NewFrameRate);
        }

        pstFrameRate->u32ProgrammedFrameRate = u32NewFrameRate;
    }

    if (pstFrameRate->u32Bypass)
        VPP_FLAG_SET(pstIntBuf->u32InternalFlags, VPP_BUF_FLAG_BYPASS);

    // Tag buffer with calculated frame rate and programmed frame rate
    pstIntBuf->u32TimestampFrameRate = u32AvgFrameRate;
    pstIntBuf->u32OperatingRate = u32NewFrameRate;

    LOGI("Using framerate %d. Calculated timestamp rate %d", u32NewFrameRate, u32AvgFrameRate);
}

static uint32_t u32Vpp_HandleReconfigure(t_StVppCtx *pstCtx)
{
    uint32_t u32Ret, u32Err = VPP_OK;
    t_StVppCb *pstCb = VPP_CB_GET(pstCtx);
    t_StVppUsecase *pstNewUc = NULL;
    uint32_t bBypass = VPP_FALSE;

    if (!pstCb)
    {
        LOGE("%s(): null cb from ctx passed from", __func__);
        return VPP_ERR_PARAM;
    }


    // Always check to see if the original use case is supported. Note that the
    // original use case can be null (in which case the validate call will
    // return an error). This would indicate that the pipeline should continue
    // in bypass mode.
    u32Ret = u32VppUsecase_Validate(&pstCb->stCtrl, pstCb->pstUc,
                                    &pstCb->stParam, &pstCb->pstUcFb);
    if (u32Ret == VPP_OK)
    {
        pstNewUc = pstCb->pstUc;
        pstCb->pstUcFb = NULL;
    }
    else
    {
        pstNewUc = pstCb->pstUcFb;
    }

    vVpp_FramerateReset(&pstCb->stFrameRate);
    LOGD("HandleReconfigure: pstCb->pstUc=%p, pstCb->pstUcFb=%p, pstNewUc=%p",
         pstCb->pstUc, pstCb->pstUcFb, pstNewUc);

    u32Ret = u32VppUsecase_Configure(pstCtx, pstNewUc, &pstCb->stCtrl,
                                     &pstCb->stParam);
    if (u32Ret != VPP_OK)
    {
        LOGE("unable to reconfigure with uc manager, u32Ret=%u", u32Ret);
        pstNewUc = NULL;
    }
    u32Ret = u32VppPipeline_Reconfigure(pstCtx, pstNewUc, &pstCb->stParam,
                                        &pstCb->stCtrl);
    if (u32Ret != VPP_OK)
    {
        // Error reconfiguring pipeline. Pipeline put itself into bypass.
        // Clear the credits.
        LOGE("pipeline reconfigure error, u32Ret=%u", u32Ret);
        u32Ret = u32VppUsecase_Configure(pstCtx, NULL, &pstCb->stCtrl,
                                         &pstCb->stParam);
        pstNewUc = NULL;
    }

    u32Ret = u32VppBuf_SetCurrentUsecase(pstCtx, pstNewUc);
    if (u32Ret != VPP_OK)
    {
        LOGE("SetCurrentUsecase failed, u32Ret=%u", u32Ret);
    }

    u32Ret = u32VppBuf_SetPortParams(pstCtx,
                                     pstCb->stParam.input_port,
                                     pstCb->stParam.output_port);
    if (u32Ret != VPP_OK)
    {
        LOGE("SetPortParams failed, u32Ret=%u", u32Ret);
    }
    return u32Err;
}

static void vVpp_Event(void *pv, t_StVppEvt stEvt)
{
    struct vpp_event evt;
    uint32_t bIssueCb = VPP_FALSE;
    uint32_t u32Ret;
    t_StVppCtx *pstCtx = VPP_CTX_GET(pv);
    t_StVppCb *pstCb = VPP_CB_GET(pstCtx);

    if (!pstCb)
    {
        LOGE("unable to get cb from ctx");
        return;
    }

    if (stEvt.eType == VPP_EVT_FLUSH_DONE)
    {
        evt.type = VPP_EVENT_FLUSH_DONE;
        evt.flush_done.port = stEvt.flush.ePort;
        bIssueCb = VPP_TRUE;
    }
    else if (stEvt.eType == VPP_EVT_RECONFIG_RDY)
    {
        uint32_t bNeedReconfig = VPP_FALSE;

        pthread_mutex_lock(&pstCb->mutex);
        if (VPP_FLAG_IS_SET(pstCb->u32VppFlags, VPP_FLAG_RECONFIG_PENDING))
        {
            VPP_FLAG_CLR(pstCb->u32VppFlags, VPP_FLAG_RECONFIG_PENDING);
            VPP_FLAG_SET(pstCb->u32VppFlags, VPP_FLAG_RECONFIG_IN_PROGRESS);
            bNeedReconfig = VPP_TRUE;
            pstCb->stParam = pstCb->stReconfigParam;
        }
        else
        {
            LOGE("Received VPP_EVT_RECONFIG_RDY but VPP_FLAG_RECONFIG_PENDING "
                 "not set!, flags=0x%x", pstCb->u32VppFlags);
        }
        pthread_mutex_unlock(&pstCb->mutex);

        if (bNeedReconfig)
        {
            // handle the reconfig here
            u32Vpp_HandleReconfigure(pstCtx);
        }
    }
    else if (stEvt.eType == VPP_EVT_RECONFIG_DONE)
    {
        pthread_mutex_lock(&pstCb->mutex);
        if (!VPP_FLAG_IS_SET(pstCb->u32VppFlags, VPP_FLAG_RECONFIG_IN_PROGRESS))
        {
            LOGE("Received VPP_EVT_RECONFIG_DONE but "
                 "VPP_FLAG_RECONFIG_IN_PROGRESS not set!, flags=0x%x",
                 pstCb->u32VppFlags);
        }
        VPP_FLAG_CLR(pstCb->u32VppFlags, VPP_FLAG_RECONFIG_IN_PROGRESS);

        if (VPP_FLAG_IS_SET(pstCb->u32VppFlags, VPP_FLAG_RECONFIG_PENDING))
        {
            u32Ret = u32VppPipeline_RequestReconfigure(pstCtx);
            LOGE_IF(u32Ret != VPP_OK, "RequestReconfigure failed in %s(), "
                    "u32Ret=%u", __func__, u32Ret);
        }
        else
        {
            u32Ret = u32VppUsecase_GetRequirements(stEvt.reconfig.pstUc,
                                                   &evt.port_reconfig_done.req);
            LOGE_IF(u32Ret != VPP_OK, "GetRequirements failed in %s, u32Ret=%u",
                    __func__, u32Ret);

            evt.type = VPP_EVENT_RECONFIG_DONE;
            evt.port_reconfig_done.reconfig_status = stEvt.reconfig.u32Err;
            bIssueCb = VPP_TRUE;
        }

        pthread_mutex_unlock(&pstCb->mutex);
    }

    if (bIssueCb == VPP_TRUE && pstCb->stCb.vpp_event)
        pstCb->stCb.vpp_event(pstCb->stCb.pv, evt);
}

static void vVpp_ReadProperties(t_StVppCtx *pstCtx, t_StVppDbgProp *pstProp)
{
    char property_value[PROPERTY_VALUE_MAX] = {0};
    uint32_t u32Tmp;
    uint64_t u64Tmp;

    if (!pstCtx || !pstProp)
        return;

    memset(pstProp, 0, sizeof(t_StVppDbgProp));

    property_get(VPP_PROPERTY_LOG_CORE, property_value, "0");
    pstProp->u64LogLevelCore = strtoull(property_value, NULL, 0);

    property_get(VPP_PROPERTY_LOG_PERFMASK, property_value, "0");
    pstProp->u32LogPerfMask = strtoul(property_value, NULL, 0);

    property_get(VPP_PROPERTY_DUMP_BUFFER_ENABLE, property_value, "0");
    pstProp->u32BufDumpEnable = strtoul(property_value, NULL, 0);

    property_get(VPP_PROPERTY_STATS_LEVEL, property_value, "0");
    pstProp->u32StatsEn = strtoul(property_value, NULL, 0);

    property_get(VPP_PROPERTY_DUMP_BUFFER_PATH, pstProp->cBufDumpPath, "0");

    // Set any globals
    u64LogLevel = pstProp->u64LogLevelCore;

    // Log after setting global (in case defined log level disables this log
    LOGD("%s=0x%llx", VPP_PROPERTY_LOG_CORE,
         (long long unsigned int)pstProp->u64LogLevelCore);
    LOGD("%s=0x%u", VPP_PROPERTY_LOG_PERFMASK, pstProp->u32LogPerfMask);
}

static void vVpp_InitCtrl(t_StVppCb *pstCb)
{
    pstCb->stCtrl.demo.process_percent = SPLIT_PERCENT_DEFAULT;
}

static uint32_t u32Vpp_SessionAdd(t_StVppCtx *pstCtx)
{
    t_StVppSession *pstNew;

    VPP_RET_IF_NULL(pstCtx, VPP_ERR_PARAM);

    pstNew = calloc(1, sizeof(t_StVppSession));
    if (!pstNew)
        return VPP_ERR_NO_MEM;

    pstNew->pstCtx = pstCtx;
    if (stVppGlobal.pstHead == NULL)
    {
        stVppGlobal.pstHead = pstNew;
        pstNew->pstNext = NULL;
    }
    else
    {
        pstNew->pstNext = stVppGlobal.pstHead;
        stVppGlobal.pstHead = pstNew;
    }

    return VPP_OK;
}

static uint32_t u32Vpp_SessionRemove(t_StVppCtx *pstCtx)
{
    t_StVppSession *pstCurrent, *pstPrev;

    VPP_RET_IF_NULL(pstCtx, VPP_ERR_PARAM);

    pstCurrent = stVppGlobal.pstHead;
    pstPrev = stVppGlobal.pstHead;

    while (pstCurrent != NULL)
    {
        if (pstCurrent->pstCtx == pstCtx)
        {
            if (pstCurrent == stVppGlobal.pstHead)
            {
                stVppGlobal.pstHead = pstCurrent->pstNext;
            }
            else
            {
                pstPrev->pstNext = pstCurrent->pstNext;
            }
            free(pstCurrent);
            return VPP_OK;
        }
        pstPrev = pstCurrent;
        pstCurrent = pstCurrent->pstNext;
    }

    LOGE("Session with context %p not found!", pstCtx);
    return VPP_ERR_PARAM;
}

/************************************************************************
 * Global Functions
 ***********************************************************************/

uint32_t vpp_boot()
{
    uint32_t u32Ret;

    LOG_ENTER();

    pthread_mutex_lock(&stVppGlobal.mutex);
    if (stVppGlobal.pstHead == NULL && stVppGlobal.bBooted == VPP_FALSE)
        u32Ret = u32VppUseCase_BootIPs();
    else
    {
        LOGE("Can't boot. At least one session still open pstHead=%p or already booted bBooted=%u",
             stVppGlobal.pstHead, stVppGlobal.bBooted);
        u32Ret = VPP_ERR_STATE;
    }

    if (u32Ret == VPP_OK)
        stVppGlobal.bBooted = VPP_TRUE;
    pthread_mutex_unlock(&stVppGlobal.mutex);

    LOG_EXIT_RET(u32Ret);
}

uint32_t vpp_shutdown()
{
    uint32_t u32Ret;

    LOG_ENTER();

    pthread_mutex_lock(&stVppGlobal.mutex);
    if (stVppGlobal.pstHead == NULL && stVppGlobal.bBooted)
        u32Ret = u32VppUseCase_ShutdownIPs();
    else
    {
        LOGE("Can't shutdown. At least one session still open pstHead=%p or not booted bBooted=%u",
             stVppGlobal.pstHead, stVppGlobal.bBooted);
        u32Ret = VPP_ERR_STATE;
    }

    if (u32Ret == VPP_OK)
        stVppGlobal.bBooted = VPP_FALSE;
    pthread_mutex_unlock(&stVppGlobal.mutex);

    LOG_EXIT_RET(u32Ret);
}

void *vpp_init(uint32_t flags, struct vpp_callbacks cb)
{
    int rc;
    uint32_t u32;
    uint32_t bIsSecure;
    uint32_t u32Ret = VPP_OK;
    t_StVppCtx *pstCtx;
    t_StVppCb *pstCb;
    t_StVppCallback stCb;
    t_StVppDbgProp stDbgProp;

    LOG_ENTER_ARGS("flags=0x%x", flags);

    pstCtx = malloc(sizeof(t_StVppCtx));
    pstCb = malloc(sizeof(t_StVppCb));
    if (!pstCb || !pstCtx)
        goto ERR_MALLOC_CB;
    memset(pstCb, 0, sizeof(t_StVppCb));
    memset(pstCtx, 0, sizeof(t_StVppCtx));

    pthread_mutex_lock(&stVppGlobal.mutex);
    u32Ret = u32Vpp_SessionAdd(pstCtx);
    pthread_mutex_unlock(&stVppGlobal.mutex);
    if (u32Ret != VPP_OK)
    {
        LOGE("Error adding session, u32Ret=%u", u32Ret);
        goto ERR_SESSION_ADD;
    }

    memset(&stDbgProp, 0, sizeof(t_StVppDbgProp));
    vVpp_ReadProperties(pstCtx, &stDbgProp);

    u32 = u32VppStats_Init(pstCtx);
    LOGE_IF(u32 != VPP_OK, "ERROR: unable to initialize stats block, u32=%u", u32);

    u32VppStats_SetEnable(pstCtx, stDbgProp.u32StatsEn);

    LOGI("flags=0x%x", flags);

    u32Ret = u32VppUsecase_Init(pstCtx);
    if (u32Ret != VPP_OK)
        goto ERR_UC_INIT;

    bIsSecure = (flags & VPP_SESSION_SECURE) ? VPP_TRUE : VPP_FALSE;
    if (bIsSecure && u32VppIsCpSupported() != VPP_TRUE)
    {
        LOGE("secure session requested but can not supported");
        goto ERR_CP;
    }

    pstCtx->pstVppCb = pstCb;

    rc = pthread_mutex_init(&pstCb->mutex, NULL);
    if (rc)
    {
        LOGE("unable to initialize mutex");
        goto ERR_MUTEX_INIT;
    }

    // Store the session flags
    pstCb->stParam.u32SessionFlags = flags;
    pstCb->stReconfigParam.u32SessionFlags = flags;

    // External callbacks
    pstCb->stCb = cb;

    // Ion
    u32Ret = u32VppIon_Init(pstCtx);
    if (u32Ret != VPP_OK)
    {
        LOGE("unable to initialize ion");
        goto ERR_ION_INIT;
    }

    // Buffer Pool
    u32Ret = u32VppBuf_Init(pstCtx, bIsSecure, stDbgProp.u32LogPerfMask);
    if (u32Ret != VPP_OK)
    {
        LOGE("unable to initialize buffer pool");
        goto ERR_BUF_INIT;
    }

    // Pipeline
    stCb.pv = (void *)pstCtx;
    stCb.input_buffer_done = vVpp_InputBufferDone;
    stCb.output_buffer_done = vVpp_OutputBufferDone;
    stCb.event = vVpp_Event;
    u32Ret = u32VppPipeline_Init(pstCtx, stCb);
    if (u32Ret != VPP_OK)
    {
        LOGE("unable to initialize pipeline");
        goto ERR_PL_INIT;
    }

    // Utils
    u32Ret = u32VppUtils_Init(pstCtx, flags);
    if (u32Ret != VPP_OK)
    {
        LOGE("unable to initialize utils");
        goto ERR_UTIL_INIT;
    }

    // Buffer dumps
    u32VppUtils_SetBufDumpEnable(pstCtx, stDbgProp.u32BufDumpEnable);
    if(stDbgProp.cBufDumpPath[0] != '0')
        vVppBuf_SetDumpParams(pstCtx, stDbgProp.cBufDumpPath);
    else
        vVppBuf_SetDumpParams(pstCtx, VPP_DEFAULT_DUMP_BUF_PATH);

    vVpp_InitCtrl(pstCb);

    vVpp_SetState(pstCb, VPP_STATE_INITED);

    LOG_EXIT_RET(pstCtx);

ERR_UTIL_INIT:
    u32VppPipeline_Term(pstCtx);

ERR_PL_INIT:
    u32VppBuf_Term(pstCtx);

ERR_BUF_INIT:
    u32VppIon_Term(pstCtx);

ERR_ION_INIT:
    pthread_mutex_destroy(&pstCb->mutex);

ERR_MUTEX_INIT:
ERR_CP:
    vVppUsecase_Term(pstCtx);

ERR_UC_INIT:
    pthread_mutex_lock(&stVppGlobal.mutex);
    u32Vpp_SessionRemove(pstCtx);
    pthread_mutex_unlock(&stVppGlobal.mutex);

ERR_SESSION_ADD:
ERR_MALLOC_CB:
    if (pstCb)
        free(pstCb);
    if (pstCtx)
        free(pstCtx);

    LOG_EXIT_RET(NULL);
}

void vpp_term(void *ctx)
{
    uint32_t u32Ret = VPP_OK;

    t_StVppCb *pstCb;

    LOG_ENTER();

    if (!ctx)
    {
        LOGE("%s called with null ctx", __func__);
        LOG_EXIT();
        return;
    }

    pstCb = VPP_CB_GET(ctx);

    if (VPP_STATE_EQUAL(pstCb, VPP_STATE_ACTIVE))
    {
        u32Ret = u32VppPipeline_Close(VPP_CTX_GET(ctx));
        LOGE_IF(u32Ret != VPP_OK, "error closing pipeline, u32Ret=%u", u32Ret);
    }

    u32Ret = u32VppUtils_Term(VPP_CTX_GET(ctx));
    LOGE_IF(u32Ret != VPP_OK, "error term utils, u32Ret=%u", u32Ret);

    u32Ret = u32VppPipeline_Term(VPP_CTX_GET(ctx));
    LOGE_IF(u32Ret != VPP_OK, "error term pipeline, u32Ret=%u", u32Ret);

    u32Ret = u32VppBuf_Term(VPP_CTX_GET(ctx));
    LOGE_IF(u32Ret != VPP_OK, "error term buffer pool, u32Ret=%u", u32Ret);

    u32Ret = u32VppIon_Term(VPP_CTX_GET(ctx));
    LOGE_IF(u32Ret != VPP_OK, "error ion term, u32Ret=%u", u32Ret);

    vVpp_StatsDump(pstCb);

    pthread_mutex_destroy(&pstCb->mutex);

    vVppUsecase_Term(VPP_CTX_GET(ctx));

    u32Ret = u32VppStats_Term(VPP_CTX_GET(ctx));
    LOGE_IF(u32Ret != VPP_OK, "error stats term, u32Ret=%u", u32Ret);

    pthread_mutex_lock(&stVppGlobal.mutex);
    u32Ret = u32Vpp_SessionRemove(VPP_CTX_GET(ctx));
    pthread_mutex_unlock(&stVppGlobal.mutex);
    LOGE_IF(u32Ret != VPP_OK, "Error session remove, u32Ret=%u", u32Ret);

    if (VPP_CB_GET(ctx))
        free(VPP_CB_GET(ctx));

    if (VPP_CTX_GET(ctx))
        free(VPP_CTX_GET(ctx));

    LOG_EXIT();
}

uint32_t vpp_set_ctrl(void *ctx, struct hqv_control ctrl,
                      struct vpp_requirements *req)
{
    uint32_t u32 = VPP_OK;
    uint32_t bIsCustom = VPP_FALSE;
    t_StVppCb *pstCb;

    LOG_ENTER();

    if (!ctx || !req || ctrl.mode >= HQV_MODE_MAX)
    {
        LOG_EXIT_RET(VPP_ERR_PARAM);
    }

    pstCb = VPP_CB_GET(ctx);

    vVpp_DumpControl(ctrl);

    bIsCustom = ctrl.ctrl_type == HQV_CONTROL_CUST;

    if (!bIsCustom)
    {
        if (VPP_STATE_EQUAL(pstCb, VPP_STATE_INITED))
        {
            pstCb->stCtrl.mode = ctrl.mode;
        }
        else if (VPP_STATE_EQUAL(pstCb, VPP_STATE_ACTIVE))
        {
            if (pstCb->stCtrl.mode != ctrl.mode)
            {
                LOGE("Unable to set new top level mode during active video, "
                     "prev=%u, new=%u", pstCb->stCtrl.mode, ctrl.mode);
                LOG_EXIT_RET(VPP_ERR_PARAM);
            }

            u32 = u32VppUsecase_IsCtrlSupported(pstCb->pstUc, ctrl);
            if (u32 != VPP_TRUE)
            {
                LOGE("Unable to set control during active video, u32=%u", u32);
                LOG_EXIT_RET(VPP_ERR_PARAM);
            }
        }

        if (ctrl.mode == HQV_MODE_MANUAL ||
            (ctrl.ctrl_type >= HQV_CONTROL_GLOBAL_START &&
             ctrl.ctrl_type < HQV_CONTROL_MAX))
        {
            switch (ctrl.ctrl_type)
            {
                case HQV_CONTROL_CADE:
                    if(ctrl.mode == HQV_MODE_MANUAL)
                    {
                        if(ctrl.cade.mode >= HQV_MODE_MAX)
                        {
                            LOGE("CADE mode %d out of range", ctrl.cade.mode);
                            LOG_EXIT_RET(VPP_ERR_PARAM);
                        }
                        ctrl.cade.cade_level = RANGE_CHECK(ctrl.cade.cade_level, CADE_LEVEL_MIN,
                                                           CADE_LEVEL_MAX);
                        ctrl.cade.contrast = RANGE_CHECK(ctrl.cade.contrast,
                                                         CADE_CONTRAST_MIN, CADE_CONTRAST_MAX);
                        ctrl.cade.saturation = RANGE_CHECK(ctrl.cade.saturation,
                                                           CADE_SATURATION_MIN, CADE_SATURATION_MAX);
                        pstCb->stCtrl.cade = ctrl.cade;
                    }
                    break;
                case HQV_CONTROL_DI:
                    if(ctrl.mode == HQV_MODE_MANUAL)
                    {
                        if(ctrl.di.mode >= HQV_DI_MODE_MAX)
                        {
                            LOGE("DI mode %d out of range", ctrl.di.mode);
                            LOG_EXIT_RET(VPP_ERR_PARAM);
                        }
                        pstCb->stCtrl.di = ctrl.di;
                    }
                    break;
                case HQV_CONTROL_TNR:
                    if(ctrl.mode == HQV_MODE_MANUAL)
                    {
                        if(ctrl.tnr.mode >= HQV_MODE_MAX)
                        {
                            LOGE("TNR mode %d out of range", ctrl.tnr.mode);
                            LOG_EXIT_RET(VPP_ERR_PARAM);
                        }
                        ctrl.tnr.level = RANGE_CHECK(ctrl.tnr.level, TNR_LEVEL_MIN, TNR_LEVEL_MAX);
                        pstCb->stCtrl.tnr = ctrl.tnr;
                    }
                    break;
                case HQV_CONTROL_CNR:
                    if(ctrl.mode == HQV_MODE_MANUAL)
                    {
                        if(ctrl.cnr.mode >= HQV_MODE_MAX)
                        {
                            LOGE("CNR mode %d out of range", ctrl.cnr.mode);
                            LOG_EXIT_RET(VPP_ERR_PARAM);
                        }
                        ctrl.cnr.level = RANGE_CHECK(ctrl.cnr.level, CNR_LEVEL_MIN, CNR_LEVEL_MAX);
                        pstCb->stCtrl.cnr = ctrl.cnr;
                    }
                    break;
                case HQV_CONTROL_AIE:
                    if (ctrl.mode == HQV_MODE_MANUAL)
                    {
                        if (ctrl.aie.mode >= HQV_MODE_MAX)
                        {
                            LOGE("AIE mode %d out of range", ctrl.aie.mode);
                            LOG_EXIT_RET(VPP_ERR_PARAM);
                        }

                        if (ctrl.aie.hue_mode >= HQV_HUE_MODE_MAX)
                        {
                            LOGD("AIE hue mode OOB, setting to off: hue_mode=%u",
                                 ctrl.aie.hue_mode);
                            ctrl.aie.hue_mode = HQV_HUE_MODE_OFF;
                        }

                        ctrl.aie.cade_level = RANGE_CHECK(ctrl.aie.cade_level,
                                                          AIE_CADE_LEVEL_MIN,
                                                          AIE_CADE_LEVEL_MAX);
                        ctrl.aie.ltm_level = RANGE_CHECK(ctrl.aie.ltm_level,
                                                         AIE_LTM_LEVEL_MIN,
                                                         AIE_LTM_LEVEL_MAX);
                        ctrl.aie.ltm_sat_gain = RANGE_CHECK(ctrl.aie.ltm_sat_gain,
                                                            AIE_LTM_SAT_GAIN_MIN,
                                                            AIE_LTM_SAT_GAIN_MAX);
                        ctrl.aie.ltm_sat_offset = RANGE_CHECK(ctrl.aie.ltm_sat_offset,
                                                              AIE_LTM_SAT_OFFSET_MIN,
                                                              AIE_LTM_SAT_OFFSET_MAX);
                        ctrl.aie.ltm_ace_str = RANGE_CHECK(ctrl.aie.ltm_ace_str,
                                                           AIE_LTM_ACE_STR_MIN,
                                                           AIE_LTM_ACE_STR_MAX);
                        ctrl.aie.ltm_ace_bright_l = RANGE_CHECK(ctrl.aie.ltm_ace_bright_l,
                                                                AIE_LTM_ACE_BRI_L_MIN,
                                                                AIE_LTM_ACE_BRI_L_MAX);
                        ctrl.aie.ltm_ace_bright_h = RANGE_CHECK(ctrl.aie.ltm_ace_bright_h,
                                                                AIE_LTM_ACE_BRI_H_MIN,
                                                                AIE_LTM_ACE_BRI_H_MAX);
                        pstCb->stCtrl.aie = ctrl.aie;
                    }
                    break;
                case HQV_CONTROL_FRC:
                    if(ctrl.frc.mode >= HQV_FRC_MODE_MAX)
                    {
                        LOGE("FRC mode %d out of range", ctrl.frc.mode);
                        LOG_EXIT_RET(VPP_ERR_PARAM);
                    }
                    ctrl.frc.level = RANGE_CHECK(ctrl.frc.level, HQV_FRC_LEVEL_OFF,
                                                 HQV_FRC_LEVEL_HIGH);
                    if (ctrl.frc.mode == HQV_FRC_MODE_SLOMO &&
                        ctrl.frc.interp >= HQV_FRC_INTERP_MAX)
                    {
                        LOGD("FRC INTERP OOB=%u, setting to default 2x",
                             ctrl.frc.interp);
                        ctrl.frc.interp = HQV_FRC_INTERP_2X;
                    }
                    pstCb->stCtrl.frc = ctrl.frc;
                    break;
                case HQV_CONTROL_EAR:
                    if (ctrl.ear.mode >= HQV_EAR_MODE_MAX)
                    {
                        LOGE("EAR mode %u out of range", ctrl.ear.mode);
                        LOG_EXIT_RET(VPP_ERR_PARAM);
                    }
                    pstCb->stCtrl.ear = ctrl.ear;
                    break;
                case HQV_CONTROL_QBR:
                    if (ctrl.qbr.mode >= HQV_QBR_MODE_MAX)
                    {
                        LOGE("QBR mode %u out of range", ctrl.qbr.mode);
                        LOG_EXIT_RET(VPP_ERR_PARAM);
                    }
                    pstCb->stCtrl.qbr = ctrl.qbr;
                    break;
                case HQV_CONTROL_GLOBAL_DEMO:
                    ctrl.demo.process_percent = RANGE_CHECK(ctrl.demo.process_percent,
                                                            SPLIT_PERCENT_MIN, SPLIT_PERCENT_MAX);
                    if(ctrl.demo.process_direction >= HQV_SPLIT_MAX)
                        ctrl.demo.process_direction = HQV_SPLIT_LEFT_TO_RIGHT;
                    pstCb->stCtrl.demo = ctrl.demo;
                    break;
                case HQV_CONTROL_NONE:
                    if(ctrl.mode == HQV_MODE_MANUAL)
                    {
                        LOGE("HQV Manual Control Type %d (NONE) not supported", ctrl.ctrl_type);
                        LOG_EXIT_RET(VPP_ERR_PARAM);
                    }
                    break;
                case HQV_CONTROL_CUST:
                default:
                    LOGE("HQV Control %d not supported", ctrl.ctrl_type);
                    LOG_EXIT_RET(VPP_ERR_PARAM);
                    break;
            }
        }
    }

    if (VPP_STATE_EQUAL(pstCb, VPP_STATE_INITED) && !bIsCustom)
    {
        pstCb->pstUc = pstVppUsecase_Find(&pstCb->stCtrl, NULL);
        u32VppUsecase_GetRequirements(pstCb->pstUc, req);

        if (pstCb->stCtrl.mode == HQV_MODE_OFF ||
            (u32GetEnabledControlCount(&pstCb->stCtrl) == 0 &&
             ctrl.ctrl_type >= HQV_CONTROL_GLOBAL_START &&
             ctrl.ctrl_type < HQV_CONTROL_MAX))
            u32 = pstCb->pstUc == NULL ? VPP_OK : VPP_ERR;
        else
            u32 = pstCb->pstUc ? VPP_OK : VPP_ERR_INVALID_CFG;

        LOGE_IF(u32 != VPP_OK, "uc_find returned %u, uc=%p", u32, pstCb->pstUc);
    }
    else if (VPP_STATE_EQUAL(pstCb, VPP_STATE_ACTIVE) || bIsCustom)
    {
        u32 = u32VppPipeline_SetHqvCtrl(VPP_CTX_GET(ctx), ctrl);
        LOGE_IF(u32 != VPP_OK, "Pipeline_SetHqvCtrl u32=%u", u32);
    }

    LOG_EXIT_RET(u32);
}

uint32_t vpp_set_parameter(void *ctx, enum vpp_port port, struct vpp_port_param param)
{
    uint32_t u32Ret, u32Err = VPP_OK;
    t_StVppCb *pstCb;

    t_StVppUsecase *pstUc;
    t_StVppParam *pstParam;
    t_StVppHqvCtrl *pstCtrl;

    LOG_ENTER_ARGS("port=%u, w=%u, h=%u, str=%u, scl=%u, fmt=%u",
                   port, param.width, param.height, param.stride,
                   param.scanlines, param.fmt);

    if (!ctx || (port != VPP_PORT_INPUT && port != VPP_PORT_OUTPUT))
        LOG_EXIT_RET(VPP_ERR_PARAM);

    pstCb = VPP_CB_GET(ctx);
    pstUc = pstCb->pstUc;
    pstParam = &pstCb->stParam;
    pstCtrl = &pstCb->stCtrl;

    if (!VPP_STATE_EQUAL(pstCb, VPP_STATE_INITED))
        LOG_EXIT_RET(VPP_ERR_STATE);

    if (port == VPP_PORT_INPUT)
    {
        pstParam->input_port = param;
        pstCb->u32VppFlags |= VPP_FLAG_INPUT_PORT_SET;
    }
    else if (port == VPP_PORT_OUTPUT)
    {
        pstParam->output_port = param;
        pstCb->u32VppFlags |= VPP_FLAG_OUTPUT_PORT_SET;
    }

    if (pstCb->u32VppFlags & VPP_FLAG_INPUT_PORT_SET &&
        pstCb->u32VppFlags & VPP_FLAG_OUTPUT_PORT_SET)
    {
        // Now that we have port parameters for input and output, validate
        // that they still make a valid usecase
        u32Ret = u32VppUsecase_Validate(pstCtrl, pstCb->pstUc, pstParam, NULL);
        if (u32Ret != VPP_OK)
        {
            pstCb->pstUc = NULL;
            u32Err = VPP_ERR_INVALID_CFG;
            LOGE("Usecase_Validate failed, enabling bypass, u32Ret=%u", u32Ret);
        }

        // Lock the resources
        u32Ret = u32VppUsecase_Configure(VPP_CTX_GET(ctx), pstCb->pstUc, pstCtrl, pstParam);
        if (u32Ret != VPP_OK)
        {
            pstCb->pstUc = NULL;
            if (u32Err == VPP_OK)
                u32Err = VPP_ERR_RESOURCES;
            LOGE("Usecase_Open failed, enabling bypass, u32Ret=%u", u32Ret);
        }

        // Start the pipeline
        u32Ret = u32VppPipeline_Open(VPP_CTX_GET(ctx), pstCb->pstUc, pstParam, pstCtrl);
        if (u32Ret != VPP_OK)
        {
            pstCb->pstUc = NULL;
            if (u32Err == VPP_OK)
                u32Err = u32Ret;
            LOGE("Pipeline_Open failed, enabling bypass, u32Ret=%u", u32Ret);

            u32Ret = u32VppUsecase_Configure(VPP_CTX_GET(ctx), NULL, pstCtrl, pstParam);
            LOGE_IF(u32Ret != VPP_OK, "Usecase_Configure(bypass), u32Ret=%u", u32Ret);
        }

        u32Ret = u32VppBuf_SetCurrentUsecase(VPP_CTX_GET(ctx), pstCb->pstUc);
        if (u32Ret != VPP_OK)
        {
            LOGE("SetCurrentUsecase failed, u32Ret=%u", u32Ret);
        }

        u32Ret = u32VppBuf_SetPortParams(VPP_CTX_GET(ctx),
                                             pstParam->input_port,
                                             pstParam->output_port);
        if (u32Ret != VPP_OK)
        {
            LOGE("SetPortParams failed, u32Ret=%u", u32Ret);
        }
        vVpp_SetState(pstCb, VPP_STATE_ACTIVE);
    }
    LOG_EXIT_RET(u32Err);
}

uint32_t vpp_queue_buf(void *ctx, enum vpp_port port, struct vpp_buffer *buf)
{
    uint32_t u32Ret, u32Cnt, u32BufDumpEnable;
    t_StVppCb *pstCb;
    t_StVppBuf *pstIntBuf;
    t_StVppCtx *pstCtx;

    LOG_ENTER_ARGS("port=%u, buf=%p", port, buf);

    if (!ctx || (port != VPP_PORT_INPUT && port != VPP_PORT_OUTPUT))
        LOG_EXIT_RET(VPP_ERR_PARAM);

    pstCtx = VPP_CTX_GET(ctx);
    pstCb = VPP_CB_GET(ctx);

    if (!VPP_STATE_EQUAL(pstCb, VPP_STATE_ACTIVE))
        LOG_EXIT_RET(VPP_ERR_STATE);

    if (port == VPP_PORT_INPUT)
    {
        pthread_mutex_lock(&pstCb->mutex);
        if (VPP_FLAG_IS_SET(pstCb->u32VppFlags, VPP_FLAG_RECONFIG_PENDING) ||
            VPP_FLAG_IS_SET(pstCb->u32VppFlags, VPP_FLAG_RECONFIG_IN_PROGRESS))
        {
            pthread_mutex_unlock(&pstCb->mutex);
            LOGE("%s() called during reconfigure", __func__);
            LOG_EXIT_RET(VPP_ERR_STATE);
        }
        pthread_mutex_unlock(&pstCb->mutex);
        VPP_STATS(pstCb, QInputBuf);
        u32Cnt = VPP_STATS_GET(pstCb, QInputBuf);

    }
    else
    {
        VPP_STATS(pstCb, QOutputBuf);
        u32Cnt = VPP_STATS_GET(pstCb, QOutputBuf);
    }

    u32Ret = u32VppBuf_InternalGet(VPP_CTX_GET(ctx), buf, port, &pstIntBuf);
    if (u32Ret != VPP_OK)
    {
        LOGE("unable to get internal buffer, port=%u", port);
        LOG_EXIT_RET(VPP_ERR);
    }

    pstIntBuf->u32BufNum = u32Cnt;
    pstIntBuf->eBufPxType = eVppBufPxDataType_Raw;

    u32VppUtils_GetBufDumpEnable(pstCtx, &u32BufDumpEnable);
    if (port == VPP_PORT_INPUT)
    {
        if(VPP_FLAG_IS_SET(u32BufDumpEnable, VPP_BUFDUMP_INPUT))
            VPP_FLAG_SET(pstIntBuf->u32InternalFlags, VPP_BUF_FLAG_DUMP);
        vVpp_FramerateUpdate(ctx, buf->timestamp, buf->flags, pstIntBuf);
        VPP_CRC_BUF(pstCb, pstIntBuf);
    }
    else //VPP_PORT_OUTPUT
    {
        if(VPP_FLAG_IS_SET(u32BufDumpEnable, VPP_BUFDUMP_OUTPUT))
            VPP_FLAG_SET(pstIntBuf->u32InternalFlags, VPP_BUF_FLAG_DUMP);
    }
    u32Ret = u32VppPipeline_QueueBuf(VPP_CTX_GET(ctx), port, pstIntBuf);
    if (u32Ret != VPP_OK)
    {
        struct vpp_buffer *pstExtBuf;
        u32VppBuf_InternalPut(VPP_CTX_GET(ctx), pstIntBuf, &pstExtBuf, VPP_FALSE);
        LOGE("unable to get queue buffer");
        LOG_EXIT_RET(VPP_ERR);
    }

    LOG_EXIT_RET(VPP_OK);
}

uint32_t vpp_reconfigure(void *ctx,
                         struct vpp_port_param input_param,
                         struct vpp_port_param output_param)
{
    uint32_t u32Ret = VPP_OK;
    t_StVppCb *pstCb;

    LOG_ENTER();

    if (!ctx)
        LOG_EXIT_RET(VPP_ERR_PARAM);

    pstCb = VPP_CB_GET(ctx);

    LOG_PARAM_STR(I, input_param, output_param);

    if (!VPP_STATE_EQUAL(pstCb, VPP_STATE_ACTIVE))
        LOG_EXIT_RET(VPP_ERR_STATE);

    pthread_mutex_lock(&pstCb->mutex);

    pstCb->stReconfigParam.input_port = input_param;
    pstCb->stReconfigParam.output_port = output_param;

    if (!VPP_FLAG_IS_SET(pstCb->u32VppFlags, VPP_FLAG_RECONFIG_PENDING) &&
        !VPP_FLAG_IS_SET(pstCb->u32VppFlags, VPP_FLAG_RECONFIG_IN_PROGRESS))
    {
        u32Ret = u32VppPipeline_RequestReconfigure(VPP_CTX_GET(ctx));
    }

    VPP_FLAG_SET(pstCb->u32VppFlags, VPP_FLAG_RECONFIG_PENDING);
    pthread_mutex_unlock(&pstCb->mutex);

    LOG_EXIT_RET(u32Ret);
}

uint32_t vpp_flush(void *ctx, enum vpp_port port)
{
    uint32_t u32Ret;
    t_StVppCb *pstCb;

    LOG_ENTER_ARGS("port=%u", port);

    if (!ctx || !VPP_CB_GET(ctx))
        LOG_EXIT_RET(VPP_ERR_PARAM);

    pstCb = VPP_CB_GET(ctx);

    if (!VPP_STATE_EQUAL(pstCb, VPP_STATE_ACTIVE))
        LOG_EXIT_RET(VPP_ERR_STATE);

    if (port == VPP_PORT_INPUT)
    {
        VPP_STATS(pstCb, InputFlush);
        vVpp_FramerateReset(&pstCb->stFrameRate);
    }
    else if (port == VPP_PORT_OUTPUT)
        VPP_STATS(pstCb, OutputFlush);
    else
        LOG_EXIT_RET(VPP_ERR_PARAM);

    u32Ret = u32VppPipeline_RequestFlush(VPP_CTX_GET(ctx), port);
    LOGE_IF(u32Ret != VPP_OK, "RequestFlush returned u32Ret=%u", u32Ret);

    LOG_EXIT_RET(u32Ret);
}

uint32_t vpp_set_vid_prop(void *ctx, struct video_property prop)
{
    uint32_t u32 = VPP_OK;
    t_StVppCtx *pstCtx;

    LOG_ENTER();

    if (!ctx)
    {
        LOG_EXIT_RET(VPP_ERR_PARAM);
    }

    pstCtx = VPP_CTX_GET(ctx);

    u32 = u32VppUtils_SetVidProp(pstCtx, prop);

    LOG_EXIT_RET(u32);
}
