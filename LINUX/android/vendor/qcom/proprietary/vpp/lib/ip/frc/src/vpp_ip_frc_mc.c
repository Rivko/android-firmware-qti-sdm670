/*!
 * @file vpp_ip_frc_mc.c
 *
 * @cr
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services
 *
 *
 */

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <malloc.h>
#include <inttypes.h>

#include <media/msm_vidc.h>

#include "AEEStdErr.h"

#include "vpp.h"
#include "vpp_buf.h"
#include "vpp_callback.h"
#include "vpp_utils.h"
#include "vpp_ip_frc_mc.h"
#include "vpp_stats.h"

#define VPP_LOG_TAG     VPP_LOG_MODULE_FRC_MC_TAG
#define VPP_LOG_MODULE  VPP_LOG_MODULE_FRC_MC
#include "vpp_dbg.h"
 #include "hvx_debug.h"

/************************************************************************
 * Local definitions
 ************************************************************************/

// #define DUMP_PROCESSING_PARAMS

//#define FRCMC_FORCE_DUMP_BUF
#define FRCMC_DUMP_BUF_IN_NM                  "frcmc_%p_in.yuv"
#define FRCMC_DUMP_BUF_OUT_NM                 "frcmc_%p_out.yuv"

#ifdef FRCMC_LOG_FRAMEDESC
#define FRCMC_DUMP_FRAME_DESC(pstCb, pstBuf, str, port) vVppIpFrcMc_DumpFrameDesc(pstCb, pstBuf, str, port)
#else
#define FRCMC_DUMP_FRAME_DESC(pstCb, pstBuf, str, port)
#endif
#define FRCMC_CBLOG(pstCb, buf, logid) if (buf) vVppIpCbLog(pstCb, buf, logid);
#define FRCMC_REG_BUF(pstCb, buf) \
    if (buf) \
    { \
        vVppIpHvxCore_RegisterBuffer(pstCb, &(buf)->stPixel); \
    }
#define FRCMC_UNREG_BUF(pstCb, buf) \
    if (buf && VPP_FLAG_IS_SET(buf->stPixel.u32IntBufFlags, VPP_BUF_FLAG_HVX_REGISTERED)) \
    { \
        vVppIpHvxCore_UnregisterBuffer(pstCb, &(buf)->stPixel); \
    }
//When a session has updated global parameters using set_custom_ctrl,
//other running sessions also need to be informed and update global parameters
#define MAX_SESSIONS                    32
#define FRCMC_NUM_IN_BUFFERS            (FRC_NUM_IN_PIX_BUFFS + FRC_NUM_IN_MBI_BUFFS)
#define FRCMC_PORT_PARAM_WIDTH_MAX      RES_FHD_MAX_W
#define FRCMC_PORT_PARAM_WIDTH_MIN      324
#define FRCMC_PORT_PARAM_HEIGHT_MAX     RES_FHD_MAX_H
#define FRCMC_PORT_PARAM_HEIGHT_MIN     128

#define FRCMC_MC_QUALITY_DEFAULT        4
#define FRCMC_MC_NO_FRAME_COPY_DEFAULT  1
#define FRCMC_INTERP_FRAMES_DEFAULT     1
#define FRCMC_INTERP_FRAMES_MAX         3
// #define VPP_FRCMC_DEBUG_MBI

enum
{
    FRCMC_BLOCK_MC,
    FRCMC_BLOCK_MAX,
};

typedef enum {
    eFrcMcSetCustomFlag_Global,
    eFrcMcSetCustomFlag_Local
} t_EFrcMcSetCustomFlags;

enum {
    FRCMC_STAT_PROC,
    FRCMC_STAT_WORKER,
    FRCMC_STAT_WORKER_SLEEP,
};

/************************************************************************
 * Local static variables
 ************************************************************************/
static pthread_mutex_t frcmc_global_param_mutex = PTHREAD_MUTEX_INITIALIZER;

static t_StVppIpFrcMcCb *pStSetCustomCb[MAX_SESSIONS] = {NULL};//all init to NULL

static t_StCustomFrcMcParams stGlobalFrcMcParams =
{
    .mode = FRC_MODE_HFR,
    .mc_quality = FRCMC_MC_QUALITY_DEFAULT,
    .NUM_INTERP_FRAMES = FRCMC_INTERP_FRAMES_DEFAULT,
    .interp_cnt = 0,
    .no_frame_copy = FRCMC_MC_NO_FRAME_COPY_DEFAULT,
    .RepeatMode_repeatPeriod = 5,
    .TH_MOTION = 60,
    .TH_MOTION_LOW = 60,
    .TH_MVOUTLIER_COUNT = 30,
    .TH_MVOUTLIER_COUNT_LOW = 25,
    .TH_OCCLUSION = 10,
    .TH_OCCLUSION_LOW = 10,
    .TH_MOTION00 = 63,
    .TH_MOTION00_LOW = 63,
    .TH_MVOUTLIER_VARIANCE_COUNT = 30,
    .TH_MVOUTLIER_VARIANCE_COUNT_LOW = 25,
    .TH_SCENECUT = 75,
    .TH_VARIANCE = 1,
    .TH_SAD_FR_RATIO = 9,
};

static const t_StVppStatsConfig astFrcMcStatsCfg[] = {
    VPP_PROF_DECL(FRCMC_STAT_PROC, 10, 1),
    VPP_PROF_DECL(FRCMC_STAT_WORKER, 1, 0),
    VPP_PROF_DECL(FRCMC_STAT_WORKER_SLEEP, 1, 0),
};

static const uint32_t u32FrcMcStatCnt = VPP_STATS_CNT(astFrcMcStatsCfg);

static const uint32_t au32FrcMcMaxOutRate[VPP_RESOLUTION_MAX] = {33, 33, 33, 0};
static const uint32_t au32FrcMcMaxSloMoInterp[VPP_RESOLUTION_MAX] = {4, 4, 2, 1};

/************************************************************************
 * Forward Declarations
 *************************************************************************/
static uint32_t u32VppIpFrcMc_CmdPut(t_StVppIpFrcMcCb *pstCb, t_StVppIpCmd stCmd);
static void vVppIpFrcMc_InitParam(t_StVppIpFrcMcCb *pstCb);
/************************************************************************
 * Local Functions
 ************************************************************************/
static void vVppIpFrcMc_PutCmdUpdateGlobal(t_StVppIpFrcMcCb *pstCb)
{
    LOGI("%s\n", __func__);

    t_StVppIpCmd stCmd;

    stCmd.eCmd = VPP_IP_CMD_UPDATE_GLOBAL_PARAM;
    //ID and sub ID not used for frcmc
    //stCmd.stGlobalCustomFlag.u32Id = u32GlobalCustomId;
    //stCmd.stGlobalCustomFlag.u32SubId = u32GlobalCustomSubId;
    u32VppIpFrcMc_CmdPut(pstCb, stCmd);

    return;
}

static uint32_t u32VppIpFrcMc_RegisterGlobalParameterUpdating(t_StVppIpFrcMcCb *pstCb)
{
    uint32_t u32Ret = VPP_OK, i;
    LOGD("In %s", __func__);
    pthread_mutex_lock(&frcmc_global_param_mutex);
    for (i = 0 ; i < MAX_SESSIONS; i++)
    {
        if (pStSetCustomCb[i] != NULL)
            continue;
        pStSetCustomCb[i] = pstCb;
        break;
    }
    pthread_mutex_unlock(&frcmc_global_param_mutex);

    if (i < MAX_SESSIONS)
        u32Ret = VPP_OK;
    else
    {
        LOGE("No free slot found in pStSetCustomCb[].");
        u32Ret = VPP_ERR;
    }
    return u32Ret;
}

static uint32_t u32VppIpFrcMc_UnregisterGlobalParameterUpdating(t_StVppIpFrcMcCb *pstCb)
{
    uint32_t u32Ret = VPP_OK, i;
    LOGD("In %s()", __func__);
    pthread_mutex_lock(&frcmc_global_param_mutex);
    for (i = 0 ; i < MAX_SESSIONS; i++)
    {
        if (pStSetCustomCb[i] == pstCb)
        {
            pStSetCustomCb[i] = NULL;
            break;
        }
    }
    pthread_mutex_unlock(&frcmc_global_param_mutex);

    if (i < MAX_SESSIONS)
        u32Ret = VPP_OK;
    else
    {
        LOGE("In %s(): slot not found in pStSetCustomCb[].", __func__);
        u32Ret = VPP_ERR;
    }
    return u32Ret;
}

//For all these reistered sessions except myself, call vVppIpFrcMc_PutCmdUpdateGlobal()
static void vVppIpFrcMc_SetGlobalParameterUpdating(t_StVppIpFrcMcCb *pstCb)
{
    uint32_t i;

    LOGD("In %s()", __func__);

    for (i = 0 ; i < MAX_SESSIONS; i++)
    {
        //if(pStSetCustomCb[i].pstCb != NULL)
        //    LOGD("In %s pStSetCustomCb[i]=%p pstCb=%p", __func__, pStSetCustomCb[i], pstCb);
        if (pStSetCustomCb[i] == NULL || pStSetCustomCb[i] == pstCb)
            continue;
        LOGD("In %s(),To call vVppIpFrcMc_PutCmdUpdateGlobal i=%d", __func__, i);

        //put update global para cmd so other sessions know they need to update global para
        vVppIpFrcMc_PutCmdUpdateGlobal(pStSetCustomCb[i]);
    }

    return;
}

//copy global param to local one according to pu32GlobalCustomSetFlag
static void vVppIpFrcMc_UpdateLocalParamFromGlobal(t_StVppIpFrcMcCb *pstCb)
{
    LOGD("in %s()", __func__);

    void* pvSrc = NULL;
    void* pvDst = NULL;
    uint32_t u32ToBeCopied = 0;

    t_StCustomFrcMcParams *pstLclParams = &pstCb->stLocalFrcMcParams;
    t_StCustomFrcMcParams *pstGblParams = &stGlobalFrcMcParams;

    pvDst = (void*)pstLclParams;
    pvSrc = (void*)pstGblParams;
    u32ToBeCopied = sizeof(t_StCustomFrcMcParams);
    memcpy(pvDst, pvSrc, u32ToBeCopied);
    vVppIpFrcMc_InitParam(pstCb);

    return;
}

//custom->data format:
//Byte 0: Low 8-bit of data_length
//Byte 1: High 8-bit of data_length
//Byte 2: Low 8-bit of ID
//Byte 3: High 8-bit of ID
//Byte 4 to Byte (data_length-4-1): Data
//ID:
//Bit-0: 1-bit fGlobalLocal, 1 local, and 0 global
//Bit-1 to bit-6: 6-bit u32ParamId
//Bit-7 to bit-12:6-bit u32ParamSubId
//Bit-13 to bit-15 : reserved
//
static uint32_t u32VppIpFrcMc_SetCustomCtrl(t_StVppIpFrcMcCb *pstCb, struct hqv_ctrl_custom *custom)
{
    uint32_t ret = VPP_OK;
    uint32_t u32ThisId = 0;
    uint32_t u32ThisLen = 0;
    char *cPtr = custom->data;
    t_EFrcMcSetCustomFlags fGlobalLocal = eFrcMcSetCustomFlag_Local;
    //uint32_t u32ParamId = 0; //not used for frcmc
    //uint32_t u32ParamSubId = 0; //not used for frcmc
    t_StCustomFrcMcParams *pstCustomFrcMcParams;

    LOGD("in %s(), custom->len=%d", __func__, custom->len);

    pthread_mutex_lock(&frcmc_global_param_mutex);

    //Get u32ThisLen
    u32ThisLen = *cPtr++;
    u32ThisLen += (*cPtr) << 8; //16-bit u32ThisLen, first byte as low 8-bit and second byte as high 8-bit
    cPtr++;
    LOGD("in %s(), u32ThisLen=%d", __func__, u32ThisLen);
    if (u32ThisLen <= FRCMC_CUSTOM_HEADER_SIZE)
    {
        LOGE("%s(): invallid parameter length, u32ThisLen=%d and less than FRCMC_CUSTOM_HEADER_SIZE.", __func__, u32ThisLen);
        ret = VPP_ERR;
        pthread_mutex_unlock(&frcmc_global_param_mutex);
        return ret;
    }

    //Get u32ThisId
    u32ThisId = *cPtr++;
    u32ThisId |= (*cPtr) << 8; //16-bit u32ThisId, first byte as low 8-bit and second byte as high 8-bit
    cPtr++;
    fGlobalLocal = (t_EFrcMcSetCustomFlags) (u32ThisId & FRCMC_GLOBAL_LOCAL_BIT_MASK); //1-bit fGlobalLocal, 1 local, and 0 global

    //u32ParamId = (u32ThisId >> FRCMC_GLOBAL_LOCAL_BIT_LENGTH) & FRCMC_PARAMID_BIT_MASK; //6-bit u32ParamId
    //u32ParamSubId = (u32ThisId >> (FRCMC_PARAMID_BIT_LENGTH + FRCMC_GLOBAL_LOCAL_BIT_LENGTH)) & FRCMC_PARAMID_BIT_MASK; //6-bit u32ParamSubId
    //LOGD("in %s(), u32ThisId=0x%x fGlobalLocal=%d u32ParamSubId=%d", __func__, u32ThisId, fGlobalLocal, u32ParamSubId);

    if (fGlobalLocal == eFrcMcSetCustomFlag_Local)
        pstCustomFrcMcParams = &pstCb->stLocalFrcMcParams;
    else
        pstCustomFrcMcParams = &stGlobalFrcMcParams;

    void* pvSrc = (void*)cPtr;
    void* pvDst = NULL;
    uint32_t u32ToBeCopied = 0;
    ret = VPP_OK;

    pvDst = (void*)pstCustomFrcMcParams;
    u32ToBeCopied = sizeof(t_StCustomFrcMcParams);

    //ret is VPP_OK when we get here
    //Check it is a vallid value
    if (u32ToBeCopied != (u32ThisLen - FRCMC_CUSTOM_HEADER_SIZE))
    {
        LOGE("%s() Error:u32ToBeCopied=%d u32ThisLen - FRCMC_CUSTOM_HEADER_SIZE=%d.",
                __func__, u32ToBeCopied, u32ThisLen - FRCMC_CUSTOM_HEADER_SIZE);
        ret = VPP_ERR;
        pthread_mutex_unlock(&frcmc_global_param_mutex);
        return ret;
    }
    else
    {
        memcpy(pvDst, pvSrc, u32ToBeCopied);
        cPtr += u32ToBeCopied;

        if (fGlobalLocal == eFrcMcSetCustomFlag_Global)
        {
            vVppIpFrcMc_UpdateLocalParamFromGlobal(pstCb);
            vVppIpFrcMc_SetGlobalParameterUpdating(pstCb);
        }
    }

    if (ret == VPP_OK && fGlobalLocal == eFrcMcSetCustomFlag_Local) //Last time local param changed
    {
        vVppIpFrcMc_InitParam(pstCb);
    }
    pthread_mutex_unlock(&frcmc_global_param_mutex);
    return ret;
}

#ifdef FRCMC_DUMP_FRAME_ENABLE
#define FRCMC_DUMP_NM_LEN 256
void vVppIpFrcMc_DumpFrame(t_StVppIpFrcMcCb *pstCb, t_StVppBuf *pstBuf,
                           enum vpp_port port)
{
    char cPath[FRCMC_DUMP_NM_LEN];
    struct vpp_port_param *pstParam;

    if (bVppIpHvxCore_IsSecure(pstCb->pstHvxCore))
        return;

    if (port == VPP_PORT_INPUT)
    {
        snprintf(cPath, FRCMC_DUMP_NM_LEN, FRCMC_DUMP_BUF_IN_NM, pstCb);
        pstParam = &pstCb->stInput.stParam;
    }
    else
    {
        snprintf(cPath, FRCMC_DUMP_NM_LEN, FRCMC_DUMP_BUF_OUT_NM, pstCb);
        pstParam = &pstCb->stOutput.stParam;
    }

    u32VppBuf_Dump(pstCb->stBase.pstCtx, pstBuf, *pstParam, cPath);
}
#endif

void vVppIpFrcMc_DumpFrameDesc(t_StVppIpFrcMcCb *pstCb, t_StVppBuf *pstBuf, char *str,
                               enum vpp_port port)
{
    struct vpp_port_param *pstParam;

    if (port == VPP_PORT_INPUT)
    {
        pstParam = &pstCb->stInput.stParam;
    }
    else
    {
        pstParam = &pstCb->stOutput.stParam;
    }

    LOGI("%s, fmt=%u, w=%u, h=%u, stride=%u, "
         "fd=%d, pvPa=%p, pvBase=%p, uvOff=%u, sz=%u",
         str, pstParam->fmt, pstParam->width, pstParam->height,
         u32VppUtils_GetStride(pstParam), pstBuf->stPixel.fd,
         pstBuf->stPixel.priv.pvPa, pstBuf->stPixel.pvBase,
         u32VppUtils_GetUVOffset(pstParam), pstBuf->stPixel.u32ValidLen);
}

static void vVppIpFrcMc_StateSet(t_StVppIpFrcMcCb *pstCb, t_EVppFrcMcState eState)
{
    t_EVppFrcMcState eStatePrev;

    if (!pstCb)
        return;

    if (pstCb->eFrcMcState != eState)
    {
        eStatePrev = pstCb->eFrcMcState;
        pstCb->eFrcMcState = eState;
        LOGI("MC state change from %u to %u", eStatePrev, eState);
    }
}

static uint32_t u32VppIpFrcMc_ReturnBuffer(t_StVppIpFrcMcCb *pstCb,
                                           enum vpp_port ePort,
                                           t_StVppBuf *pstBuf)
{
    if (ePort == VPP_PORT_OUTPUT)
        pstCb->stats.u32OBDYuvCnt++;
    else
    {
        if (pstBuf->eBufPxType == eVppBufPxDataType_Raw)
            pstCb->stats.u32IBDYuvCnt++;
        else
            pstCb->stats.u32IBDMbiCnt++;
    }

    return u32VppIp_CbBufDone(&pstCb->stBase.stCb, ePort, pstBuf);
}

static void vVppIpFrcMc_InitParam(t_StVppIpFrcMcCb *pstCb)
{
    t_StFrcMcParam *pstFrcMcParams = &pstCb->stFrcMcParams;
    t_StCustomFrcMcParams *pstLocalParams = &pstCb->stLocalFrcMcParams;
    LOGI("%s()", __func__);

    memset(pstFrcMcParams, 0, sizeof(t_StFrcMcParam));

    vpp_svc_frc_params_t *frc_params_p = &pstCb->stFrcMcParams.stFrcParams;

    memset((void*)frc_params_p, 0, sizeof(vpp_svc_frc_params_t));

    frc_params_p->update_flags = 1;
    //Init pstCb->stFrcMcParams using values stored in pstCb->stLocalFrcMcParams
    frc_params_p->mode = pstLocalParams->mode;
    frc_params_p->mc_quality = pstLocalParams->mc_quality;
    frc_params_p->no_frame_copy = pstLocalParams->no_frame_copy;
    frc_params_p->RepeatMode_repeatPeriod = pstLocalParams->RepeatMode_repeatPeriod;
    frc_params_p->TH_MOTION = pstLocalParams->TH_MOTION;
    frc_params_p->TH_MOTION_LOW = pstLocalParams->TH_MOTION_LOW;
    frc_params_p->TH_MVOUTLIER_COUNT = pstLocalParams->TH_MVOUTLIER_COUNT;
    frc_params_p->TH_MVOUTLIER_COUNT_LOW = pstLocalParams->TH_MVOUTLIER_COUNT_LOW;
    frc_params_p->TH_OCCLUSION = pstLocalParams->TH_OCCLUSION;
    frc_params_p->TH_OCCLUSION_LOW = pstLocalParams->TH_OCCLUSION_LOW;
    frc_params_p->TH_MOTION00 = pstLocalParams->TH_MOTION00;
    frc_params_p->TH_MOTION00_LOW = pstLocalParams->TH_MOTION00_LOW;
    frc_params_p->TH_MVOUTLIER_VARIANCE_COUNT = pstLocalParams->TH_MVOUTLIER_VARIANCE_COUNT;
    frc_params_p->TH_MVOUTLIER_VARIANCE_COUNT_LOW = pstLocalParams->TH_MVOUTLIER_VARIANCE_COUNT_LOW;
    frc_params_p->TH_SCENECUT = pstLocalParams->TH_SCENECUT;
    frc_params_p->TH_VARIANCE = pstLocalParams->TH_VARIANCE;
    frc_params_p->TH_SAD_FR_RATIO = pstLocalParams->TH_SAD_FR_RATIO;

    print_vpp_svc_frc_params(frc_params_p);
}

static void vVppIpFrcMc_InitCapabilityResources(t_StVppIpFrcMcCb *pstCb)
{
    uint32_t u32Index = 0;

    //FRC
    u32VppIpHvxCore_AlgoInit(pstCb->pstHvxCore, u32Index, VPP_FUNC_ID_FRC, NULL);
}

static void vVppIpFrcMc_Compute(t_StVppIpFrcMcCb *pstCb)
{
    t_StFrcMcParam *pstFrcMcParams;
    uint32_t u32Length, u32Index, u32Width, u32Height, u32Planes;
    uint32_t au32Stride[MAX_NUM_PLANES];
    enum vpp_color_format eFmt;
    uint32_t u32Computed = 0;

    LOGI("%s()", __func__);

    if (!pstCb || !pstCb->pstHvxCore)
    {
        LOGE("%s() Error. FRCMC or HVX Core control block is NULL.", __func__);
        return;
    }

    u32Computed = pstCb->stCfg.u32ComputeMask;
    pstFrcMcParams = &pstCb->stFrcMcParams;
    u32Width = pstCb->stInput.stParam.width;
    u32Height = pstCb->stInput.stParam.height;
    eFmt = pstCb->stInput.stParam.fmt;
    u32Planes = u32VppUtils_GetNumPlanes(eFmt);

    if (pstCb->stCfg.u32ComputeMask & FRCMC_PARAM)
    {

        u32VppIpHvxCore_BufParamInit(pstCb->pstHvxCore, u32Width, u32Height, eFmt);

        if (pstCb->stCfg.u32EnableMask & FRC_ALGO_ROI)
            pstCb->stCfg.u32ComputeMask |= FRC_ALGO_ROI;
        if (pstCb->stCfg.u32EnableMask & FRC_ALGO_MC)
            pstCb->stCfg.u32ComputeMask |= FRC_ALGO_MC;

        pstCb->stCfg.u32ComputeMask &= ~FRCMC_PARAM;
    }

    if (pstCb->stCfg.u32EnableMask & FRC_ALGO_ROI)
    {
        u32VppIpHvxCore_SvcParamSetROI(pstCb->pstHvxCore, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        if (pstCb->stCfg.u32ComputeMask & FRC_ALGO_ROI)
        {
            uint32_t u32XStart, u32YStart, u32XEnd, u32YEnd, u32LineWidth, u32Luma, u32Cr, u32Cb;

            if ((u32Width > RES_FHD_MAX_W) || (u32Height > RES_FHD_MAX_H))
                u32LineWidth = 16;
            else if ((u32Width > RES_HD_MAX_W) || (u32Height > RES_HD_MAX_H))
                u32LineWidth = 8;
            else if ((u32Width > RES_SD_MAX_W) || (u32Height > RES_SD_MAX_H))
                u32LineWidth = 4;
            else
                u32LineWidth = 2;
            u32Luma = 0x00;  // BLACK
            u32Cr = 0x80;
            u32Cb = 0x80;

            switch(pstCb->stCfg.stDemo.process_direction)
            {
                case HQV_SPLIT_RIGHT_TO_LEFT:
                    u32XStart = u32Width - ((pstCb->stCfg.stDemo.process_percent * u32Width) /
                                            FRCMC_ROI_PERCENTAGE_MAX);
                    u32YStart = 0;
                    u32XEnd = u32Width;
                    u32YEnd = u32Height;
                    break;
                case HQV_SPLIT_TOP_TO_BOTTOM:
                    u32XStart = 0;
                    u32YStart = 0;
                    u32XEnd = u32Width;
                    u32YEnd = ((pstCb->stCfg.stDemo.process_percent * u32Height) /
                               FRCMC_ROI_PERCENTAGE_MAX);
                    break;
                case HQV_SPLIT_BOTTOM_TO_TOP:
                    u32XStart = 0;
                    u32YStart = u32Height - ((pstCb->stCfg.stDemo.process_percent * u32Height) /
                                             FRCMC_ROI_PERCENTAGE_MAX);
                    u32XEnd = u32Width;
                    u32YEnd = u32Height;
                    break;
                case HQV_SPLIT_LEFT_TO_RIGHT:
                default:
                    u32XStart = 0;
                    u32YStart = 0;
                    u32XEnd = ((pstCb->stCfg.stDemo.process_percent * u32Width) /
                               FRCMC_ROI_PERCENTAGE_MAX);
                    u32YEnd = u32Height;
                    break;
            }
            u32VppIpHvxCore_SvcParamSetROI(pstCb->pstHvxCore, 1, 1, u32XStart, u32YStart,
                                           u32XEnd, u32YEnd, u32LineWidth, u32Luma, u32Cr, u32Cb);
            pstCb->stCfg.u32ComputeMask &= ~FRC_ALGO_ROI;
            LOGD("ROI X0:%d, Y0:%d, X1:%d, Y1:%d", u32XStart, u32YStart, u32XEnd, u32YEnd);
        }
    }
    else
    {
        u32VppIpHvxCore_SvcParamSetROI(pstCb->pstHvxCore, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    }

    pstCb->u32InputMaxIntoProc  = FRC_NUM_IN_PIX_BUFFS;
    pstCb->u32OutputMaxIntoProc = FRC_NUM_OUT_BUFFS;

    u32Index = 0;
    u32Length = 0;
    if (pstCb->stCfg.u32EnableMask & FRC_ALGO_MC)
    {
        LOGI("Set pstParams->header");
        u32VppIpHvxCore_SvcParamSetHeaderIdxAlgo(pstCb->pstHvxCore, u32Index, VPP_FUNC_ID_FRC);
        u32Index++;
        pstCb->pstFrcParams = (vpp_svc_frc_params_t*)
            (pvVppIpHvxCore_SvcParamGetDataOffsetAddr(pstCb->pstHvxCore, u32Length));

        vpp_svc_frc_params_t* frc_params_p = &pstFrcMcParams->stFrcParams;
        frc_params_p->update_flags = 1;

        if (pstCb->stCfg.u32ComputeMask & FRC_ALGO_MC)
        {
            frc_params_p->in_frame_width = u32Width;
            frc_params_p->in_frame_height = u32Height;

            switch (pstCb->stCfg.stFrc.mode)
            {
                case HQV_FRC_MODE_OFF:
                    frc_params_p->mode = FRC_MODE_PASSTHROUGH;
                    break;
                case HQV_FRC_MODE_SLOMO:
                    frc_params_p->mode = FRC_MODE_SLOMO;
                    break;
                case HQV_FRC_MODE_SMOOTH_MOTION:
                default:
                    frc_params_p->mode = FRC_MODE_HFR;
                    break;
            }

            switch (pstCb->stCfg.stFrc.level)
            {
                case HQV_FRC_LEVEL_OFF:
                    frc_params_p->mc_quality = FRC_MC_QUAL_REPEAT;
                    break;
                case HQV_FRC_LEVEL_LOW:
                    frc_params_p->mc_quality = FRC_MC_QUAL_WEAK;
                    break;
                case HQV_FRC_LEVEL_MED:
                    frc_params_p->mc_quality = FRC_MC_QUAL_MED;
                    break;
                case HQV_FRC_LEVEL_HIGH:
                default:
                    frc_params_p->mc_quality = FRC_MC_QUAL_STRONG;
                    break;
            }

            frc_params_p->NUM_INTERP_FRAMES = FRCMC_INTERP_FRAMES_DEFAULT;
            frc_params_p->interp_cnt = 0;
            if (pstCb->stCfg.stFrc.mode == HQV_FRC_MODE_SLOMO)
            {
                switch (pstCb->stCfg.stFrc.interp)
                {
                    case HQV_FRC_INTERP_2X:
                        frc_params_p->NUM_INTERP_FRAMES = 1;
                        break;
                    case HQV_FRC_INTERP_3X:
                        frc_params_p->NUM_INTERP_FRAMES = 2;
                        break;
                    case HQV_FRC_INTERP_4X:
                        frc_params_p->NUM_INTERP_FRAMES = 3;
                        break;
                    case HQV_FRC_INTERP_1X:
                    default:
                        frc_params_p->NUM_INTERP_FRAMES = 0;
                        break;
                }
            }
            pstCb->stInfo.u32InterpFrames = frc_params_p->NUM_INTERP_FRAMES;
            pstCb->stInfo.u32FrcFactorReq = frc_params_p->NUM_INTERP_FRAMES + 1;
            pstCb->stInfo.u32FrcFactorActual = frc_params_p->NUM_INTERP_FRAMES + 1;
            pstCb->stInfo.u32InterpCnt = frc_params_p->interp_cnt;

            pstCb->stCfg.u32ComputeMask &= ~FRC_ALGO_MC;
        }

        memcpy(pstCb->pstFrcParams, frc_params_p, sizeof(vpp_svc_frc_params_t));
        u32Length += sizeof(vpp_svc_frc_params_t);
    }

    u32VppIpHvxCore_SvcParamSetDataSize(pstCb->pstHvxCore, u32Index, u32Length);

    if (u32Computed)
    {
        u32VppIpHvxCore_BuffInCompute(pstCb->pstHvxCore);
        u32VppIpHvxCore_BuffOutCompute(pstCb->pstHvxCore);

        // MBI buffer doesn't use stride, init to 0
        memset(au32Stride, 0, sizeof(au32Stride));
        u32VppIpHvxCore_BufInSetAttrStride(pstCb->pstHvxCore, FRC_MBI_BUF_IDX, au32Stride,
                                           u32Planes);
    }

    return;
}

static uint32_t u32VppIpFrcMc_CmdGet(t_StVppIpFrcMcCb *pstCb, t_StVppIpCmd *pstCmd)
{
    LOGI("%s() pstCmd->eCmd=%d", __func__, pstCmd->eCmd);
    int32_t idx;
    idx = vpp_queue_dequeue(&pstCb->stCmdQ);

    if (idx < 0)
    {
        LOGI("GetCmd=%d, idx=%d", pstCmd->eCmd, idx);
        return VPP_ERR;
    }
    else
    {
        *pstCmd = pstCb->astCmdNode[idx];
        LOG_CMD("GetCmd", pstCmd->eCmd);
        LOGI("GetCmd idx=%d pstCmd->eCmd=%d", idx, pstCmd->eCmd);
    }

    return VPP_OK;
}

static uint32_t u32VppIpFrcMc_CmdPut(t_StVppIpFrcMcCb *pstCb, t_StVppIpCmd stCmd)
{
    int32_t idx;
    uint32_t u32Ret = VPP_OK;

    pthread_mutex_lock(&pstCb->mutex);

    LOG_CMD("InsertCmd", stCmd.eCmd);
    idx = vpp_queue_enqueue(&pstCb->stCmdQ);
    if (idx < 0)
    {
        u32Ret = VPP_ERR;
    } else
    {
        pstCb->astCmdNode[idx] = stCmd;
        pthread_cond_signal(&pstCb->cond);
    }

    pthread_mutex_unlock(&pstCb->mutex);

    return u32Ret;
}

static void vVppIpFrcMc_SignalWorkerStart(t_StVppIpFrcMcCb *pstCb)
{
    pthread_mutex_lock(&pstCb->mutex);

    pstCb->u32InternalFlags |= IP_WORKER_STARTED;

    pthread_cond_signal(&pstCb->cond);

    pthread_mutex_unlock(&pstCb->mutex);
}

static void vVppIpFrcMc_WaitWorkerStart(t_StVppIpFrcMcCb *pstCb)
{
    pthread_mutex_lock(&pstCb->mutex);

    while (!(pstCb->u32InternalFlags & IP_WORKER_STARTED))
        pthread_cond_wait(&pstCb->cond, &pstCb->mutex);

    pthread_mutex_unlock(&pstCb->mutex);
}

static uint32_t u32BypassBufCheck(t_StVppIpFrcMcCb *pstCb)
{
    uint32_t u32BufReqSz = u32VppUtils_GetPxBufferSize(&pstCb->stInput.stParam);
    uint32_t u32InQSz = u32VppBufPool_Cnt(&pstCb->stInput.stPendingQ);
    t_StVppBuf* pstBufIn0 = pstVppIp_PortBufPeek(&pstCb->stInput, 0, NULL);
    t_StVppBuf* pstBufIn1 = pstVppIp_PortBufPeek(&pstCb->stInput, 1, NULL);

    if (u32InQSz)
    {
        if (pstBufIn0 == NULL)
        {
            LOGE("u32InQSz != 0 but pstBufIn_0 == NULL. u32InQSz=%u", u32InQSz);
            return VPP_FALSE;
        }

        if ((VPP_FLAG_IS_SET(pstBufIn0->pBuf->flags, VPP_BUFFER_FLAG_EOS)) ||
            (VPP_FLAG_IS_SET(pstBufIn0->u32InternalFlags, VPP_BUF_FLAG_BYPASS)) ||
            (VPP_FLAG_IS_SET(pstBufIn0->u32InternalFlags, VPP_BUF_FLAG_INTERNAL_BYPASS)) ||
            (pstBufIn0->eBufType != eVppBufType_Progressive))
        {
            LOGD("%s() First buffer EOS=0x%x, bypass=0x%x, internal_bypass=0x%x, type=%u. Bypass",
                 __func__, (VPP_FLAG_IS_SET(pstBufIn0->pBuf->flags, VPP_BUFFER_FLAG_EOS)),
                 (VPP_FLAG_IS_SET(pstBufIn0->u32InternalFlags, VPP_BUF_FLAG_BYPASS)),
                 (VPP_FLAG_IS_SET(pstBufIn0->u32InternalFlags, VPP_BUF_FLAG_INTERNAL_BYPASS)),
                 pstBufIn0->eBufType);
            return VPP_TRUE;
        }

        if (pstBufIn0->stPixel.u32FilledLen < u32BufReqSz ||
            pstBufIn0->stPixel.u32ValidLen < u32BufReqSz)
        {
            LOGE("First buffer u32FilledLen=%u, u32ValidLen=%u, Required=%u",
                 pstBufIn0->stPixel.u32FilledLen, pstBufIn0->stPixel.u32ValidLen, u32BufReqSz);
            return VPP_TRUE;
        }

        if (!pstBufIn1)
        {
            // Only one input buffer in queue, check if drain is pending
            if (VPP_FLAG_IS_SET(pstCb->u32InternalFlags, IP_DRAIN_PENDING))
            {
                LOGD("%s() One buffer left to drain for FRC_MC. Can bypass it", __func__);
                return VPP_TRUE;
            }
        }
        else
        {
            if ((VPP_FLAG_IS_SET(pstBufIn1->u32InternalFlags, VPP_BUF_FLAG_BYPASS)) ||
                (VPP_FLAG_IS_SET(pstBufIn1->u32InternalFlags, VPP_BUF_FLAG_INTERNAL_BYPASS)) ||
                (pstBufIn1->eBufType != eVppBufType_Progressive))
            {
                LOGD("%s(): Second buffer bypass=%u, internal bypass=%u type=%u. Can bypass first",
                     __func__, VPP_FLAG_IS_SET(pstBufIn1->u32InternalFlags, VPP_BUF_FLAG_BYPASS),
                     VPP_FLAG_IS_SET(pstBufIn1->u32InternalFlags, VPP_BUF_FLAG_INTERNAL_BYPASS),
                     pstBufIn1->eBufType);
                return VPP_TRUE;
            }

            if (pstBufIn1->stPixel.u32FilledLen < u32BufReqSz ||
                pstBufIn1->stPixel.u32ValidLen < u32BufReqSz)
            {
                LOGE("Second buffer u32FilledLen=%u, u32ValidLen=%u, Required=%u",
                     pstBufIn1->stPixel.u32FilledLen, pstBufIn1->stPixel.u32ValidLen, u32BufReqSz);
                return VPP_TRUE;
            }
        }
    }

    return VPP_FALSE;
}

static uint32_t u32ProcBufReqMet(t_StVppIpFrcMcCb *pstCb)
{
    // Determine if the buffers in the ports satisfy the requirements
    // to trigger processing
    uint32_t u32InQSz, u32OutQSz, u32MbiQSz;

    // This function requires that the caller has already locked the mutex
    // which guards these two queues.
    u32InQSz = u32VppBufPool_Cnt(&pstCb->stInput.stPendingQ);
    u32MbiQSz = u32VppBufPool_Cnt(&pstCb->stMbi.stPendingQ);
    u32OutQSz = u32VppBufPool_Cnt(&pstCb->stOutput.stPendingQ);

    LOGI("CHECK: u32InQSz=%u, u32MbiQSz=%u u32OutQSz=%u, InNeed=%u, MbiNeed=%u, OutNeed=%u",
         u32InQSz, u32MbiQSz, u32OutQSz, FRC_NUM_IN_PIX_BUFFS, FRC_NUM_IN_MBI_BUFFS,
         FRC_NUM_OUT_BUFFS);

    if (u32InQSz == 0)
        return VPP_FALSE;

    // If in interp state, only need to peek one input buffer and have output to write to
    if (pstCb->eFrcMcState == FRCMC_STATE_ACTIVE_INTERP && u32OutQSz >= FRC_NUM_OUT_BUFFS)
        return VPP_TRUE;

    if (u32BypassBufCheck(pstCb) == VPP_TRUE)
        return VPP_TRUE;

    if (u32InQSz >= FRC_NUM_IN_PIX_BUFFS && u32OutQSz >= FRC_NUM_OUT_BUFFS &&
        u32MbiQSz >= FRC_NUM_IN_MBI_BUFFS)
        return VPP_TRUE;

    LOGI("CHECK: ReqMet return VPP_FALSE\n");
    return VPP_FALSE;
}

static uint32_t u32WorkerThreadShouldSleep(t_StVppIpFrcMcCb *pstCb)
{
    uint32_t u32Ret = VPP_TRUE;
    uint32_t u32CmdQSz, u32ProcMet;

    // This is a predicate function used for determining if the function worker
    // thread should sleep or not. Worker thread uses a condition variable to
    // wake itself and the mutex which is used is the same as that which guards
    // these functions. Therefore, there is no need to lock a mutex prior to
    // checking the command queues within this context.
    u32CmdQSz = vpp_queue_count(&pstCb->stCmdQ);
    if (u32CmdQSz)
    {
        u32Ret = VPP_FALSE;
        LOGI("CHECK: shouldSleep=VPP_FALSE, u32CmdQSz=%u", u32CmdQSz);
        return u32Ret;
    }

    u32ProcMet = u32ProcBufReqMet(pstCb);
    if ((VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_ACTIVE)) && u32ProcMet)
    {
        u32Ret = VPP_FALSE;
        LOGI("CHECK: shouldSleep=VPP_FALSE, u32ProcMet=%u", u32ProcMet);
        return u32Ret;
    }

    LOGI("CHECK: shouldSleep=%u, u32CmdQSz=%u, u32ProcMet=%u", u32Ret,
         u32CmdQSz, u32ProcMet);

    return u32Ret;
}

static uint32_t u32VppIpFrcMc_ValidatePortParam(struct vpp_port_param stParam)
{
    if (stParam.width < FRCMC_PORT_PARAM_WIDTH_MIN ||
        stParam.width > FRCMC_PORT_PARAM_WIDTH_MAX)
    {
        LOGE("validation failed: width=%u, min=%u, max=%u", stParam.width,
             FRCMC_PORT_PARAM_WIDTH_MIN, FRCMC_PORT_PARAM_WIDTH_MAX);
        return VPP_ERR;
    }
    if (stParam.height < FRCMC_PORT_PARAM_HEIGHT_MIN ||
        stParam.height > FRCMC_PORT_PARAM_HEIGHT_MAX)
    {
        LOGE("validation failed: height=%u, min=%u, max=%u", stParam.height,
             FRCMC_PORT_PARAM_HEIGHT_MIN, FRCMC_PORT_PARAM_HEIGHT_MAX);
        return VPP_ERR;
    }
    if (stParam.fmt != VPP_COLOR_FORMAT_NV12_VENUS &&
        stParam.fmt != VPP_COLOR_FORMAT_NV21_VENUS &&
        stParam.fmt != VPP_COLOR_FORMAT_UBWC_NV12 &&
        stParam.fmt != VPP_COLOR_FORMAT_UBWC_NV21)
    {
        LOGE("validation failed: fmt=%u", stParam.fmt);
        return VPP_ERR;
    }
    if (!u32VppUtils_IsFmtUbwc(stParam.fmt))
    {
        if (stParam.width > stParam.stride)
        {
            LOGE("validation failed: width=%u, stride=%u", stParam.width,
                 stParam.stride);
            return VPP_ERR;
        }
        if (stParam.height > stParam.scanlines)
        {
            LOGE("validation failed: height=%u, scanlines=%u", stParam.height,
                 stParam.scanlines);
            return VPP_ERR;
        }
    }

    return VPP_OK;
}

static uint32_t u32VppIpFrcMc_ValidateConfig(struct vpp_port_param stInput,
                                             struct vpp_port_param stOutput)
{
    if (stInput.height != stOutput.height)
    {
        LOGD("validation failed: height, input: %u, output: %u",
             stInput.height, stOutput.height);
        return VPP_ERR;
    }
    if (stInput.width != stOutput.width)
    {
        LOGD("validation failed: width, input: %u, output: %u",
             stInput.width, stOutput.width);
        return VPP_ERR;
    }
    if (stInput.fmt != stOutput.fmt)
    {
        LOGD("validation failed: fmt, input: %u, output: %u",
             stInput.fmt, stOutput.fmt);
        return VPP_ERR;
    }
    if (u32VppIpFrcMc_ValidatePortParam(stInput))
    {
        LOGE("Input port param validation failed");
        return VPP_ERR;
    }
    if (u32VppIpFrcMc_ValidatePortParam(stOutput))
    {
        LOGE("Output port param validation failed");
        return VPP_ERR;
    }

    return VPP_OK;
}

static uint32_t u32VppIpFrcMc_ValidateCtrl(struct hqv_control stCtrl)
{
    struct hqv_ctrl_frc stFrc;

    if (stCtrl.mode != HQV_MODE_OFF &&
        stCtrl.mode != HQV_MODE_AUTO &&
        stCtrl.mode != HQV_MODE_MANUAL)
    {
        LOGE("Invalid stCtrl.mode=%u", stCtrl.mode);
        return VPP_ERR_PARAM;
    }

    if (stCtrl.mode == HQV_MODE_MANUAL)
    {
        if (stCtrl.ctrl_type != HQV_CONTROL_FRC &&
            stCtrl.ctrl_type != HQV_CONTROL_CUST &&
            stCtrl.ctrl_type != HQV_CONTROL_GLOBAL_DEMO)
        {
            LOGE("Invalid stCtrl.ctrl_type=%u", stCtrl.ctrl_type);
            return VPP_ERR_PARAM;
        }

        if (stCtrl.ctrl_type == HQV_CONTROL_FRC)
        {
            stFrc = stCtrl.frc;
            if (stFrc.mode >= HQV_FRC_MODE_MAX)
            {
                LOGE("Invalid stFrc.mode=%u", stFrc.mode);
                return VPP_ERR_PARAM;
            }

            if (stFrc.mode != HQV_FRC_MODE_OFF)
            {
                if (stFrc.level >= HQV_FRC_LEVEL_MAX)
                {
                    LOGE("Invalid stFrc.level=%u", stFrc.level);
                    return VPP_ERR_PARAM;
                }

                if (stFrc.mode == HQV_FRC_MODE_SLOMO && stFrc.interp >= HQV_FRC_INTERP_MAX)
                {
                    LOGE("Invalid stFrc.interp=%u", stFrc.interp);
                    return VPP_ERR_PARAM;
                }
            }
        }
    }
    return VPP_OK;
}


static uint32_t u32VppIpFrcMc_ProcCmdOpen(t_StVppIpFrcMcCb *pstCb)
{
    uint32_t u32Ret;
    uint32_t vpp_processing_flags = 0;
    uint32_t framesize_bytes;
    uint32_t u32NumBuffers;

    LOG_ENTER_ARGS("pstCb->stBase.pstCtx = %p", pstCb->stBase.pstCtx);

    pthread_mutex_lock(&pstCb->mutex);

    u32NumBuffers = FRCMC_NUM_IN_BUFFERS;
    u32Ret = u32VppIpHvxCore_BuffInInit(pstCb->pstHvxCore, u32NumBuffers);
    if (u32Ret != VPP_OK)
    {
        LOGE("Error allocating bufferIn pixel_data");
        goto ERR_ALLOC_INPUT;
    }

    u32Ret = u32VppIpHvxCore_BuffOutInit(pstCb->pstHvxCore, FRC_NUM_OUT_BUFFS);
    if (u32Ret != VPP_OK)
    {
        LOGE("Error allocating bufferOut pixel_data");
        goto ERR_ALLOC_OUTPUT;
    }

    if (pstCb->stCfg.u32ComputeMask)
        vVppIpFrcMc_Compute(pstCb);

    // ********* vpp_svc_context_t ****
    //Determine which VPP Functions to enable
    vpp_processing_flags |= (1<<(VPP_FUNC_ID_FRC & FUNC_ID_MASK));

    //Determine max frame size for this session
    framesize_bytes = 0;

    u32Ret = u32VppIpHvxCore_Open(pstCb->pstHvxCore, vpp_processing_flags, framesize_bytes);
    if (u32Ret != VPP_OK)
    {
        LOGE("HVX Open failed ret %d", u32Ret);
        goto ERR_HVX_OPEN;
    }

    pthread_mutex_unlock(&pstCb->mutex);

    VPP_IP_STATE_SET(pstCb, VPP_IP_STATE_ACTIVE);

    LOGI("u32VppIpFrcMc_ProcCmdOpen() posting semaphore");
    sem_post(&pstCb->sem);

    pstCb->async_res.u32OpenRet = VPP_OK;
    return pstCb->async_res.u32OpenRet;

ERR_HVX_OPEN:
    vVppIpHvxCore_BuffOutTerm(pstCb->pstHvxCore);

ERR_ALLOC_OUTPUT:
    vVppIpHvxCore_BuffInTerm(pstCb->pstHvxCore);

ERR_ALLOC_INPUT:
    pthread_mutex_unlock(&pstCb->mutex);

    pstCb->async_res.u32OpenRet = u32Ret;

    LOGI("%s() posting semaphore (error)", __func__);
    sem_post(&pstCb->sem);

    return pstCb->async_res.u32OpenRet;
}

static uint32_t u32VppIpFrcMc_ProcCmdClose(t_StVppIpFrcMcCb *pstCb)
{
    uint32_t u32Ret = VPP_OK;

    LOGD("%s()", __func__);

    if (!VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_ACTIVE))
    {
        u32Ret = VPP_ERR_STATE;
    }

    u32Ret = u32VppIpHvxCore_Close(pstCb->pstHvxCore);
    if (u32Ret != VPP_OK)
        LOGE("HVX Core Close Fail u32Ret=%u", u32Ret);

    vVppIpHvxCore_BuffInTerm(pstCb->pstHvxCore);
    vVppIpHvxCore_BuffOutTerm(pstCb->pstHvxCore);

    VPP_IP_STATE_SET(pstCb, VPP_IP_STATE_INITED);

    pstCb->async_res.u32CloseRet = u32Ret;

    LOGI("%s() posting semaphore", __func__);
    sem_post(&pstCb->sem);

    return pstCb->async_res.u32CloseRet;
}


static void vVppIpFrcMc_FlushMcPort(t_StVppIpFrcMcCb *pstCb, t_StVppIpPort *pstMcPort,
                                    enum vpp_port ePort)
{
    t_StVppBuf *pBuf;

    while (u32VppIp_PortBufGet(pstMcPort, &pBuf, &pstCb->mutex) == VPP_OK)
    {
        VPP_FLAG_SET(pBuf->u32InternalFlags, VPP_BUF_FLAG_FLUSHED);
        if (ePort == VPP_PORT_OUTPUT)
            pBuf->stPixel.u32FilledLen = 0;
        vVppIpCbLog(&pstCb->stBase.stCb, pBuf, eVppLogId_IpBufDone);
        u32VppIpFrcMc_ReturnBuffer(pstCb, ePort, pBuf);
    }

}

static uint32_t u32VppIpFrcMc_FlushPort(t_StVppIpFrcMcCb *pstCb, enum vpp_port ePort)
{
    if (ePort == VPP_PORT_INPUT)
    {
        // Flush MBI buf and input buffers
        vVppIpFrcMc_FlushMcPort(pstCb, &pstCb->stMbi, ePort);
        vVppIpFrcMc_FlushMcPort(pstCb, &pstCb->stInput, ePort);

        LOGI("Resetting state after input flush");
        vVppIpFrcMc_StateSet(pstCb, FRCMC_STATE_ACTIVE_START);
        return VPP_OK;
    }
    else if (ePort == VPP_PORT_OUTPUT)
    {
        vVppIpFrcMc_FlushMcPort(pstCb, &pstCb->stOutput, ePort);
        return VPP_OK;
    }
    else
    {
        LOGE("%s(): ePort Not correct", __func__);
        return VPP_ERR_PARAM;
    }
}

static uint32_t u32VppIpFrcMc_ProcCmdFlush(t_StVppIpFrcMcCb *pstCb,
                                           t_StVppIpCmd *pstCmd)
{
    uint32_t u32;
    t_StVppEvt stEvt;

    // Flush Port
    u32 = u32VppIpFrcMc_FlushPort(pstCb, pstCmd->flush.ePort);

    LOGI("%s() u32VppIpFrcMc_FlushPort ret = %d", __func__, u32);

    if (u32 == VPP_OK)
    {
        stEvt.eType = VPP_EVT_FLUSH_DONE;
        stEvt.flush.ePort = pstCmd->flush.ePort;
        u32VppIpCbEvent(&pstCb->stBase.stCb, stEvt);
    }

    return u32;
}

static uint32_t u32VppIpFrcMc_ProcCmdDrain(t_StVppIpFrcMcCb *pstCb)
{
    t_StVppEvt stEvt;

    pthread_mutex_lock(&pstCb->mutex);

    if (u32VppBufPool_Cnt(&pstCb->stInput.stPendingQ) == 0)
    {
        pthread_mutex_unlock(&pstCb->mutex);
        // No more input pixel buffers.
        // There shouldn't be any, but flush any remaining MBI buffers
        u32VppIpFrcMc_FlushPort(pstCb, VPP_PORT_INPUT);
        // Drain complete
        stEvt.eType = VPP_EVT_DRAIN_DONE;
        u32VppIpCbEvent(&pstCb->stBase.stCb, stEvt);
    }
    else
    {
        VPP_FLAG_SET(pstCb->u32InternalFlags, IP_DRAIN_PENDING);
        pthread_mutex_unlock(&pstCb->mutex);
    }

    return VPP_OK;
}

static uint32_t u32VppIpFrcMc_ProcCmdUpdateGlobalParam(t_StVppIpFrcMcCb *pstCb)
{
    LOGD("In %s()", __func__);
    pthread_mutex_lock(&pstCb->mutex);
    pthread_mutex_lock(&frcmc_global_param_mutex);
    vVppIpFrcMc_UpdateLocalParamFromGlobal(pstCb);
    pthread_mutex_unlock(&frcmc_global_param_mutex);
    pstCb->stCfg.u32ComputeMask |= FRCMC_PARAM;
    pthread_mutex_unlock(&pstCb->mutex);

    return VPP_OK;
}

static uint32_t u32VppIpFrcMc_ProcCmdReconfigure(t_StVppIpFrcMcCb *pstCb)
{
    uint32_t u32;

    if (!pstCb)
        return VPP_ERR_PARAM;

    u32 = u32VppIpHvxCore_PrepareCtx(pstCb->pstHvxCore);
    if (u32 != VPP_OK)
    {
        LOGE("ERROR: PrepareCtx returned err, u32=%u", u32);
    }

    pstCb->async_res.u32ReconfigRet = u32;
    LOGI("%s() posting semaphore (reconfig)", __func__);
    sem_post(&pstCb->sem);

    return u32;
}

static uint32_t bVppIpFrcMc_AlgoCtrlsCanBypass(t_StVppIpFrcMcCb *pstCb)
{
    uint32_t bCanBypass = VPP_TRUE;
    t_StVppIpFrcMcCfg *stCfg = &pstCb->stCfg;

    if (VPP_FLAG_IS_SET(stCfg->u32EnableMask, FRC_ALGO_ROI))
    {
        if (stCfg->stDemo.process_percent == 0)
            return VPP_TRUE;
    }
    if (VPP_FLAG_IS_SET(stCfg->u32EnableMask, FRC_ALGO_MC))
    {
        if (stCfg->stFrc.mode == HQV_FRC_MODE_SLOMO)
        {
            if (stCfg->stFrc.level != HQV_FRC_LEVEL_OFF &&
                stCfg->stFrc.interp != HQV_FRC_INTERP_1X)
                bCanBypass = VPP_FALSE;
        }
        else if (stCfg->stFrc.mode != HQV_FRC_MODE_OFF &&
                 stCfg->stFrc.level != HQV_FRC_LEVEL_OFF)
            bCanBypass = VPP_FALSE;
    }

    return bCanBypass;
}

static void vVppIpFrcMc_ReturnOutputBuffer(t_StVppIpFrcMcCb *pstCb, t_StVppBuf *pstBufOut)
{
    LOG_ENTER();
    if (pstBufOut)
    {
        if (pstBufOut->eQueuedPort == VPP_PORT_OUTPUT)
        {
            FRCMC_DUMP_FRAME_DESC(pstCb, pstBufOut, "out", VPP_PORT_OUTPUT);
        }
        else
        {
            FRCMC_DUMP_FRAME_DESC(pstCb, pstBufOut, "in", VPP_PORT_OUTPUT);
        }
#ifdef FRCMC_DUMP_FRAME_ENABLE
        if ((VPP_FLAG_IS_SET(pstBufOut->u32InternalFlags, VPP_BUF_FLAG_DUMP)) &&
            (pstBufOut->stPixel.u32FilledLen != 0))
        {
            vVppIpFrcMc_DumpFrame(pstCb, pstBufOut, VPP_PORT_OUTPUT);
        }
#endif
        VPP_FLAG_CLR(pstBufOut->u32InternalFlags, VPP_BUF_FLAG_FLUSHED);
        vVppIpCbLog(&pstCb->stBase.stCb, pstBufOut, eVppLogId_IpBufDone);

        u32VppIpFrcMc_ReturnBuffer(pstCb, VPP_PORT_OUTPUT, pstBufOut);
    }
}

//Utility function for u32VppIpFrcMc_ProcessBuffer()
static void vVppIpFrcMc_ReturnBuffers(t_StVppIpFrcMcCb *pstCb,
                                      t_StVppBuf *pstBufIn,
                                      t_StVppBuf *pstBufMbi,
                                      t_StVppBuf *pstBufOut,
                                      uint32_t u32BufOutFilledLength)
{
    LOG_ENTER();

    if (pstBufIn)
    {
        vVppIpFrcMc_ReturnOutputBuffer(pstCb, pstBufIn);
    }
    if (pstBufMbi)
    {
        vVppIpCbLog(&pstCb->stBase.stCb, pstBufMbi, eVppLogId_IpBufDone);
        u32VppIpFrcMc_ReturnBuffer(pstCb, VPP_PORT_INPUT, pstBufMbi);
    }
    if (pstBufOut)
    {
        pstBufOut->stPixel.u32FilledLen = u32BufOutFilledLength;
        vVppIpFrcMc_ReturnOutputBuffer(pstCb, pstBufOut);
    }
}

static void vVppIpFrcMc_PrepareInBuffer(t_StVppIpHvxCoreCb *pstHvxCore, t_StVppBuf *pstBufIn,
                                        uint32_t u32Idx, uint32_t u32BufSize,
                                        uint32_t bIsUbwc)
{
    VPP_RET_VOID_IF_NULL(pstHvxCore);
    VPP_RET_VOID_IF_NULL(pstBufIn);

    FRCMC_REG_BUF(pstHvxCore, pstBufIn);
    u32VppIpHvxCore_BufInSetUserDataLen(pstHvxCore, u32Idx, u32BufSize);
    u32VppIpHvxCore_BufInSetUserDataAddr(pstHvxCore, u32Idx, pstBufIn->stPixel.pvBase);
    if (bIsUbwc)
    {
        u32VppIpHvxCore_BufInSetAttrUbwc(pstHvxCore, u32Idx,
                                         &pstBufIn->stUbwcStats[eVppUbwcStatField_P]);
    }
}

static uint32_t u32VppIpFrcMc_UpdateInterpFactor(t_StVppIpFrcMcCb *pstCb, uint32_t u32OperatingRate)
{
    uint32_t u32Res, u32MaxOutRate, u32MaxInterp;
    VPP_RET_IF_NULL(pstCb, VPP_ERR_PARAM);

    u32Res = u32VppUtils_GetVppResolution(&pstCb->stInput.stParam);
    if (u32Res >= VPP_RESOLUTION_MAX)
    {
        LOGE("Error getting resolution, u32Res=%u", u32Res);
        return VPP_ERR;
    }
    u32MaxOutRate = au32FrcMcMaxOutRate[u32Res];
    u32MaxInterp = au32FrcMcMaxSloMoInterp[u32Res] <= pstCb->stInfo.u32FrcFactorReq ?
        au32FrcMcMaxSloMoInterp[u32Res] : pstCb->stInfo.u32FrcFactorReq;

    if (pstCb->stCfg.stFrc.mode == HQV_FRC_MODE_SLOMO)
    {
        if (u32OperatingRate > u32MaxOutRate || u32OperatingRate == 0)
        {
            pstCb->stInfo.u32FrcFactorActual = 1;
            LOGD("Operating Rate=%u is invalid or exceeds Max=%u! Can't interpolate",
                 u32OperatingRate, u32MaxOutRate);
        }
        else
        {
            pstCb->stInfo.u32FrcFactorActual = u32MaxOutRate / u32OperatingRate;
            if (pstCb->stInfo.u32FrcFactorActual > u32MaxInterp)
                pstCb->stInfo.u32FrcFactorActual = u32MaxInterp;
        }
        LOGD("FRC Factor=%u (Requested=%u Max=%u). Operating Rate=%u, Max Output=%u",
             pstCb->stInfo.u32FrcFactorActual, pstCb->stInfo.u32FrcFactorReq,
             au32FrcMcMaxSloMoInterp[u32Res], u32OperatingRate, u32MaxOutRate);
        pstCb->stInfo.u32InterpFrames = pstCb->stInfo.u32FrcFactorActual - 1;
        pstCb->pstFrcParams->NUM_INTERP_FRAMES = pstCb->stInfo.u32InterpFrames;
        pstCb->pstFrcParams->update_flags = 1;
    }

    return VPP_OK;
}

static uint32_t u32VppIpFrcMc_ProcessBuffer(t_StVppIpFrcMcCb *pstCb)
{
    uint32_t u32Ret = VPP_OK;
    t_StVppBuf *pstBufInPull, *pstBufInPeek, *pstBufMbi, *pstBufOut, *pstBufTemp;
    t_StVppIpHvxCoreCb *pstHvxCore;
    uint32_t u32InBufSz, u32BufCount, u32PlSize;
    uint32_t bIsFmtUbwc;
    uint32_t u32OutBufSz, u32Res;
    uint32_t u32MaxOutRate = 0;
    uint32_t u32MaxInterp = 0;
    uint32_t bEnableBypass = 0;
    uint64_t u64TimestampDelta;

    LOGI("%s(), Context: %p", __func__, pstCb);

    VPP_RET_IF_NULL(pstCb, VPP_ERR_PARAM);

    pstHvxCore = pstCb->pstHvxCore;
    u32OutBufSz = u32VppUtils_GetPxBufferSize(&pstCb->stOutput.stParam);
    pstBufInPull = pstBufInPeek = pstBufMbi = pstBufOut = NULL;
    bIsFmtUbwc = u32VppUtils_IsFmtUbwc(pstCb->stInput.stParam.fmt);
    u32Res = u32VppUtils_GetVppResolution(&pstCb->stInput.stParam);
    if (u32Res >= VPP_RESOLUTION_MAX)
    {
        LOGE("Error getting resolution, u32Res=%u. Enabling bypass", u32Res);
        bEnableBypass = 1;
    }
    else
    {
        u32MaxOutRate = au32FrcMcMaxOutRate[u32Res];
        u32MaxInterp = au32FrcMcMaxSloMoInterp[u32Res];
    }

    pthread_mutex_lock(&pstCb->mutex);
    if (pstCb->eFrcMcState == FRCMC_STATE_NULL)
        vVppIpFrcMc_StateSet(pstCb, FRCMC_STATE_ACTIVE_START);

    // Skip bypass and error buffer checks if in middle of interpolation since no new buffers needed
    if (pstCb->eFrcMcState != FRCMC_STATE_ACTIVE_INTERP)
    {
        if (pstCb->stCfg.u32ComputeMask)
            vVppIpFrcMc_Compute(pstCb);

        if (u32BypassBufCheck(pstCb) == VPP_TRUE)
        {
            LOGI("u32BypassBufCheck returned true. Bypassing input buffer to output");
            bEnableBypass = 1;
        }
        else
        {
            // If not bypass, peek at Input and MBI buffers to make sure timestamps match
            pstBufInPull = pstVppIp_PortBufPeek(&pstCb->stInput, 0, NULL);
            pstBufMbi = pstVppIp_PortBufPeek(&pstCb->stMbi, 0, NULL);
            if (pstBufInPull == NULL || pstBufMbi == NULL)
            {
                LOGE("Error peeking at Input %p and MBI %p buffers", pstBufInPull, pstBufMbi);
                pthread_mutex_unlock(&pstCb->mutex);
                return VPP_ERR;
            }
            if (pstBufInPull->pBuf->timestamp != pstBufMbi->pBuf->timestamp)
            {
                // Error case, return the lower timestamp to try to catch up
                if (pstBufInPull->pBuf->timestamp > pstBufMbi->pBuf->timestamp)
                {
                    LOGE("Input timestamp %"PRIu64" greater than MBI %"PRIu64", returning MBI",
                         pstBufInPull->pBuf->timestamp, pstBufMbi->pBuf->timestamp);
                    pstBufInPull = NULL;
                    u32Ret = u32VppIp_PortBufGet(&pstCb->stMbi, &pstBufMbi, NULL);
                    if ((pstBufMbi == NULL) || (u32Ret != VPP_OK))
                    {
                        LOGE("Error getting MBI buffer. pstBufMbi=%p, u32Ret=%u",
                             pstBufMbi, u32Ret);
                        pthread_mutex_unlock(&pstCb->mutex);
                        return VPP_ERR;
                    }
                }
                else
                {
                    LOGE("Input timestamp %"PRIu64" less than MBI %"PRIu64", bypassing input",
                         pstBufInPull->pBuf->timestamp, pstBufMbi->pBuf->timestamp);
                    pstBufMbi = NULL;
                    u32Ret = u32VppIp_PortBufGet(&pstCb->stInput, &pstBufInPull, NULL);
                    if (pstBufInPull == NULL || u32Ret != VPP_OK)
                    {
                        LOGE("u32VppIp_PortBufGet() error. pstBufIn=%p, u32Ret=%u",
                             pstBufInPull, u32Ret);
                        pthread_mutex_unlock(&pstCb->mutex);
                        return VPP_ERR;
                    }
                    if (pstBufInPull->eBufType == eVppBufType_Progressive)
                        u32VppBuf_GrallocFramerateMultiply(pstBufInPull,
                                                           pstCb->stInfo.u32FrcFactorActual);
                }
                goto ERROR_PROCESS;
            }
        }
    }

    if (bEnableBypass)
    {
        // Get buffer to bypass
        u32Ret = u32VppIp_PortBufGet(&pstCb->stInput, &pstBufInPull, NULL);
        if (pstBufInPull == NULL || u32Ret != VPP_OK)
        {
            pthread_mutex_unlock(&pstCb->mutex);

            LOGE("u32VppIp_PortBufGet() error. pstBufIn=%p, u32Ret=%u", pstBufInPull, u32Ret);
            return VPP_ERR;
        }

        vVppIpFrcMc_StateSet(pstCb, FRCMC_STATE_ACTIVE_START);

        if (pstBufInPull->eBufType == eVppBufType_Progressive)
            u32VppBuf_GrallocFramerateMultiply(pstBufInPull, pstCb->stInfo.u32FrcFactorActual);

        pthread_mutex_unlock(&pstCb->mutex);
    }
    else
    {
        for (u32BufCount = 0; u32BufCount < FRCMC_NUM_IN_BUFFERS; u32BufCount++)
        {
            u32VppIpHvxCore_BufInSetUserDataLen(pstHvxCore, u32BufCount, 0);
            u32VppIpHvxCore_BufInSetUserDataAddr(pstHvxCore, u32BufCount, NULL);
            u32VppIpHvxCore_BufInSetAttrUbwc(pstHvxCore, u32BufCount, NULL);
        }

        pstBufInPull = pstBufInPeek = pstBufMbi = NULL;
        if (pstCb->eFrcMcState == FRCMC_STATE_ACTIVE ||
            pstCb->eFrcMcState == FRCMC_STATE_ACTIVE_START)
        {
            u32Ret = u32VppIp_PortBufGet(&pstCb->stInput, &pstBufInPull, NULL);
            if (pstBufInPull == NULL || u32Ret != VPP_OK)
            {
                pthread_mutex_unlock(&pstCb->mutex);

                LOGE("u32VppIp_PortBufGet() error. pstBufIn=%p, u32Ret=%u", pstBufInPull, u32Ret);
                return VPP_ERR;
            }

            pstBufInPeek = pstVppIp_PortBufPeek(&pstCb->stInput, 0, NULL);
            if (!pstBufInPeek)
            {
                LOGE("Error peeking second input buffer, bypass first");
                goto ERROR_PROCESS;
            }

            // Get MBI buffer
            u32Ret = u32VppIp_PortBufGet(&pstCb->stMbi, &pstBufMbi, NULL);
            if (u32Ret != VPP_OK || pstBufMbi == NULL)
            {
                LOGE("Error getting MBI buffers, bypass input");
                goto ERROR_PROCESS;
            }
            // If FRCMC not enabled, bypass pixel and return MBI buffers
            if (bVppIpFrcMc_AlgoCtrlsCanBypass(pstCb))
            {
                LOGD("Settings result in no processing: bypass input and return MBI");
                goto ERROR_PROCESS;
            }
            if (pstCb->stCfg.stFrc.mode == HQV_FRC_MODE_SLOMO &&
                (pstCb->stInfo.u32FrcFactorActual != pstCb->stInfo.u32FrcFactorReq ||
                 pstCb->stInfo.u32FrcFactorActual > u32MaxInterp ||
                 pstCb->stInfo.u32FrcFactorActual * pstBufInPull->u32OperatingRate >
                 u32MaxOutRate))
            {
                // Output rate exceeds max, so try to reduce interpolation factor
                u32Ret = u32VppIpFrcMc_UpdateInterpFactor(pstCb, pstBufInPull->u32OperatingRate);
                if (u32Ret != VPP_OK)
                {
                    LOGE("Error updating interpolation rate, bypass input");
                    goto ERROR_PROCESS;
                }
                if (pstCb->stInfo.u32FrcFactorActual <= 1)
                {
                    LOGD("Operating Rate %u too high to interpolate (Max output rate = %u)",
                         pstBufInPull->u32OperatingRate, u32MaxOutRate);
                    goto ERROR_PROCESS;
                }
            }

            u32VppIpHvxCore_BufParamGetPlaneTotalSize(pstHvxCore, &u32InBufSz);
            u32BufCount = 0;

            if (pstCb->eFrcMcState == FRCMC_STATE_ACTIVE_START)
            {
                vVppIpFrcMc_PrepareInBuffer(pstHvxCore, pstBufInPull, u32BufCount++,
                                            u32InBufSz, bIsFmtUbwc);
            }

            vVppIpFrcMc_PrepareInBuffer(pstHvxCore, pstBufInPeek, u32BufCount++,
                                        u32InBufSz, bIsFmtUbwc);
            if (u32BufCount > FRC_MBI_BUF_IDX)
            {
                LOGE("Error MBI buffer index %u already used by pixel buffer!", FRC_MBI_BUF_IDX);
                goto ERROR_PROCESS;
            }

            u32PlSize = pstBufMbi->stPixel.u32FilledLen;
            vVppIpFrcMc_PrepareInBuffer(pstHvxCore, pstBufMbi, FRC_MBI_BUF_IDX,
                                        u32PlSize, VPP_FALSE);
            u32VppIpHvxCore_BufInSetAttrSize(pstHvxCore, FRC_MBI_BUF_IDX, &u32PlSize, 1);
            LOGI("MBI buf plane size = %u", pstBufMbi->stPixel.u32FilledLen);

            pstCb->stInfo.u32InterpCnt = 0;
            pstCb->pstFrcParams->interp_cnt = 0;
        }
        // Get Output buffer
        u32Ret = u32VppIp_PortBufGet(&pstCb->stOutput, &pstBufOut, NULL);
        if (u32Ret != VPP_OK || pstBufOut == NULL)
        {
            LOGE("Error getting output buffers, bypassing input, returning MBI");
            goto ERROR_PROCESS;
        }

        if (pstBufOut->stPixel.u32ValidLen < u32OutBufSz)
        {
            LOGE("Error need %u but out buffer size %u too small", u32OutBufSz,
                 pstBufOut->stPixel.u32ValidLen);
            goto ERROR_PROCESS;
        }
        FRCMC_REG_BUF(pstCb->pstHvxCore, pstBufOut);
        u32VppIpHvxCore_BufOutSetUserDataAddr(pstHvxCore, 0,
                                              (void*)(pstBufOut->stPixel.pvBase));
        pthread_mutex_unlock(&pstCb->mutex);

        // These log time start calls are intentionally separated from the
        // RegisterBuffer calls, so that the RegisterBuffer calls do not skew the
        // times for the buffers. The time it takes to register a buffer should be
        // included in the overall processing time.
        FRCMC_CBLOG(&pstCb->stBase.stCb, pstBufInPull, eVppLogId_IpProcStart);
        FRCMC_CBLOG(&pstCb->stBase.stCb, pstBufInPeek, eVppLogId_IpProcStart);
        FRCMC_CBLOG(&pstCb->stBase.stCb, pstBufOut, eVppLogId_IpProcStart);

#ifdef DUMP_PROCESSING_PARAMS
        if (VPP_FLAG_IS_SET(pstCb->stCfg.u32EnableMask, FRC_ALGO_MC))
            print_vpp_svc_frc_params(pstCb->pstFrcParams);
#endif

        LOGI("about to call vpp_svc_process, InProcCnt=%u, OutProcCnt=%u, eFrcMcState=%u",
             pstCb->stats.u32InProcCnt, pstCb->stats.u32OutProcCnt, pstCb->eFrcMcState);
        int rc = 0;
        VPP_IP_PROF_START(&pstCb->stBase, FRCMC_STAT_PROC);
        rc = VppIpHvxCore_Process(pstHvxCore);
        VPP_IP_PROF_STOP(&pstCb->stBase, FRCMC_STAT_PROC);

        LOGI("vpp_svc_process returned, rc=%d", rc);
        pstCb->stats.u32InProcCnt++;

        FRCMC_UNREG_BUF(pstCb->pstHvxCore, pstBufInPull);
        FRCMC_UNREG_BUF(pstCb->pstHvxCore, pstBufInPeek);
        FRCMC_UNREG_BUF(pstCb->pstHvxCore, pstBufMbi);
        FRCMC_UNREG_BUF(pstCb->pstHvxCore, pstBufOut);

        FRCMC_CBLOG(&pstCb->stBase.stCb, pstBufInPull, eVppLogId_IpProcDone);
        FRCMC_CBLOG(&pstCb->stBase.stCb, pstBufInPeek, eVppLogId_IpProcDone);
        FRCMC_CBLOG(&pstCb->stBase.stCb, pstBufOut, eVppLogId_IpProcDone);

        if (pstBufInPull)
        {
            u32VppBuf_GrallocFramerateMultiply(pstBufInPull, pstCb->stInfo.u32FrcFactorActual);
            pstBufInPull->u32TimestampFrameRate *= pstCb->stInfo.u32FrcFactorActual;
            pstBufInPull->u32OperatingRate *= pstCb->stInfo.u32FrcFactorActual;
        }

        if (rc != AEE_SUCCESS)
        {
            pstCb->stInfo.u32InterpCnt = 0;
            if (rc == AEE_EVPP_FRMCPYOP)
            {
                // MC asks for frame repeat, no output generated, can recycle output buffers
                LOGD("Frame repeat, no output generated");
                u32VppIp_PortBufPut(&pstCb->stOutput, pstBufOut, &pstCb->mutex, &pstCb->cond);
                pstBufOut = NULL;
                vVppIpFrcMc_StateSet(pstCb, FRCMC_STATE_ACTIVE);
            }
            else
            {
                u32Ret = VPP_ERR;
                LOGE("Error: compute on aDSP failed, return=%d\n",rc);
                vVppIpFrcMc_StateSet(pstCb, FRCMC_STATE_ACTIVE_START);
            }
        }
        else
        {
            LOGD("vpp_svc_process successful! u32InterpCnt=%u. Output buf size=%u",
                 pstCb->stInfo.u32InterpCnt, u32OutBufSz);
            pstCb->stats.u32OutProcCnt++;
            pstCb->stInfo.u32InterpCnt++;
            pstCb->pstFrcParams->update_flags = 0;

            if (pstCb->eFrcMcState != FRCMC_STATE_ACTIVE_INTERP)
            {
                // Source buffer for gralloc, extradata, cookie
                pstBufTemp = pstBufInPull;

                u64TimestampDelta = pstBufInPeek->pBuf->timestamp - pstBufInPull->pBuf->timestamp;
                u64TimestampDelta /= pstCb->stInfo.u32FrcFactorActual;
                pstCb->stInfo.u64TimestampDelta = u64TimestampDelta;
                pstCb->stInfo.u64TimestampBase = pstBufInPull->pBuf->timestamp;
                pstCb->stInfo.u32TimestampFrameRate = pstBufInPull->u32TimestampFrameRate;
                pstCb->stInfo.u32OperatingRate = pstBufInPull->u32OperatingRate;
                if (VPP_FLAG_IS_SET(pstBufInPeek->pBuf->flags, VPP_BUFFER_FLAG_DATACORRUPT))
                {
                    VPP_FLAG_SET(pstBufOut->pBuf->flags, VPP_BUFFER_FLAG_DATACORRUPT);
                    VPP_FLAG_CLR(pstBufInPeek->pBuf->flags, VPP_BUFFER_FLAG_DATACORRUPT);
                }
            }
            else
            {
                // In interp case, first buffer already released, so peek next buffer as source
                // for gralloc, extradata, cookie
                pstBufTemp = pstVppIp_PortBufPeek(&pstCb->stInput, 0, NULL);
            }

            if (!pstBufTemp)
                LOGE("Error, no source buffer for Extradata, Gralloc, and Cookie");
            else
            {
                u32VppBuf_PropagateExtradata(pstBufTemp, pstBufOut,
                                             eVppBufType_Progressive,
                                             VPP_EXTERNAL_EXTRADATA_TYPE);
                u32VppBuf_GrallocMetadataCopy(pstBufTemp, pstBufOut);
                // In interp state, source buffer framerate hasn't been updated yet
                if (pstCb->eFrcMcState == FRCMC_STATE_ACTIVE_INTERP)
                    u32VppBuf_GrallocFramerateMultiply(pstBufOut, pstCb->stInfo.u32FrcFactorActual);
                pstBufOut->pBuf->cookie_in_to_out = pstBufTemp->pBuf->cookie_in_to_out;
            }

            if (bIsFmtUbwc)
            {
                u32VppIpHvxCore_BufOutGetAttrUbwc(pstHvxCore, 0,
                                                  &pstBufOut->stUbwcStats[eVppUbwcStatField_P]);
            }

            pstBufOut->pBuf->timestamp = pstCb->stInfo.u64TimestampBase +
                (pstCb->stInfo.u64TimestampDelta * pstCb->stInfo.u32InterpCnt);
            pstBufOut->u32TimestampFrameRate = pstCb->stInfo.u32TimestampFrameRate;
            pstBufOut->u32OperatingRate = pstCb->stInfo.u32OperatingRate;

            if (pstCb->stInfo.u32InterpCnt >= pstCb->stInfo.u32InterpFrames)
            {
                vVppIpFrcMc_StateSet(pstCb, FRCMC_STATE_ACTIVE);
                pstCb->stInfo.u32InterpCnt = 0;
                pstCb->pstFrcParams->interp_cnt = 0;
                if (pstCb->stInfo.u32InterpFrames > 1)
                    pstCb->pstFrcParams->update_flags = 1;
            }
            else
            {
                vVppIpFrcMc_StateSet(pstCb, FRCMC_STATE_ACTIVE_INTERP);
                pstCb->pstFrcParams->interp_cnt = pstCb->stInfo.u32InterpCnt;
                pstCb->pstFrcParams->update_flags = 1;
            }

        }
    }

    if (pstBufInPull && VPP_FLAG_IS_SET(pstBufInPull->u32InternalFlags, VPP_BUF_FLAG_INTERNAL_BYPASS))
        VPP_FLAG_CLR(pstBufInPull->u32InternalFlags, VPP_BUF_FLAG_INTERNAL_BYPASS);

    if (u32Ret == VPP_OK)
        vVppIpFrcMc_ReturnBuffers(pstCb, pstBufInPull, pstBufMbi, pstBufOut, u32OutBufSz);
    else
        vVppIpFrcMc_ReturnBuffers(pstCb, pstBufInPull, pstBufMbi, pstBufOut, 0);

    return u32Ret;

ERROR_PROCESS:
    FRCMC_UNREG_BUF(pstCb->pstHvxCore, pstBufInPull);
    FRCMC_UNREG_BUF(pstCb->pstHvxCore, pstBufInPeek);
    FRCMC_UNREG_BUF(pstCb->pstHvxCore, pstBufMbi);
    vVppIpFrcMc_StateSet(pstCb, FRCMC_STATE_ACTIVE_START);
    pstCb->stInfo.u32InterpCnt = 0;
    pthread_mutex_unlock(&pstCb->mutex);
    vVppIpFrcMc_ReturnBuffers(pstCb, pstBufInPull, pstBufMbi, pstBufOut, 0);
    return VPP_ERR;
}

static void vVppIpFrcMc_HandlePendingDrain(t_StVppIpFrcMcCb *pstCb)
{
    t_StVppEvt stEvt;

    pthread_mutex_lock(&pstCb->mutex);

    if ((VPP_FLAG_IS_SET(pstCb->u32InternalFlags, IP_DRAIN_PENDING)) &&
        u32VppBufPool_Cnt(&pstCb->stInput.stPendingQ) == 0)
    {
        pthread_mutex_unlock(&pstCb->mutex);
        // No more input pixel buffers. There shouldn't be any, but flush any remaining MBI buffers
        u32VppIpFrcMc_FlushPort(pstCb, VPP_PORT_INPUT);
        // Drain complete
        VPP_FLAG_CLR(pstCb->u32InternalFlags, IP_DRAIN_PENDING);
        stEvt.eType = VPP_EVT_DRAIN_DONE;
        u32VppIpCbEvent(&pstCb->stBase.stCb, stEvt);
    }
    else
        pthread_mutex_unlock(&pstCb->mutex);
}

static void *vpVppIpFrcMc_Worker(void *pv)
{
    LOGI("%s started", __func__);

    t_StVppIpFrcMcCb *pstCb = (t_StVppIpFrcMcCb *)pv;

    // Signal back to main thread that we've launched and are ready to go
    vVppIpFrcMc_SignalWorkerStart(pstCb);

    while (1)
    {
        pthread_mutex_lock(&pstCb->mutex);
        while (u32WorkerThreadShouldSleep(pstCb))
        {
            VPP_IP_PROF_START(&pstCb->stBase, FRCMC_STAT_WORKER_SLEEP);
            pthread_cond_wait(&pstCb->cond, &pstCb->mutex);
            VPP_IP_PROF_STOP(&pstCb->stBase, FRCMC_STAT_WORKER_SLEEP);
        }

        VPP_IP_PROF_START(&pstCb->stBase, FRCMC_STAT_WORKER);

        uint32_t u32Ret;
        t_StVppIpCmd stCmd;
        u32Ret = u32VppIpFrcMc_CmdGet(pstCb, &stCmd);
        if (u32Ret == VPP_OK)
        {
            pthread_mutex_unlock(&pstCb->mutex);

            // Process the command
            LOG_CMD("ProcessCmd", stCmd.eCmd);

            if (stCmd.eCmd == VPP_IP_CMD_THREAD_EXIT)
            {
                VPP_IP_PROF_STOP(&pstCb->stBase, FRCMC_STAT_WORKER);
                break;
            }

            else if (stCmd.eCmd == VPP_IP_CMD_OPEN)
                u32VppIpFrcMc_ProcCmdOpen(pstCb);

            else if (stCmd.eCmd == VPP_IP_CMD_CLOSE)
                u32VppIpFrcMc_ProcCmdClose(pstCb);

            else if (stCmd.eCmd == VPP_IP_CMD_FLUSH)
                u32VppIpFrcMc_ProcCmdFlush(pstCb, &stCmd);

            else if (stCmd.eCmd == VPP_IP_CMD_DRAIN)
                u32VppIpFrcMc_ProcCmdDrain(pstCb);

            else if (stCmd.eCmd == VPP_IP_CMD_UPDATE_GLOBAL_PARAM)
                u32VppIpFrcMc_ProcCmdUpdateGlobalParam(pstCb);

            else if (stCmd.eCmd == VPP_IP_CMD_RECONFIGURE)
                u32VppIpFrcMc_ProcCmdReconfigure(pstCb);

            else
                LOGE("unknown command in queue");

            VPP_IP_PROF_STOP(&pstCb->stBase, FRCMC_STAT_WORKER);
            continue;
        }

        if (!VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_ACTIVE))
        {
            LOGD("got buffer, but state is not active");
            pthread_mutex_unlock(&pstCb->mutex);
            VPP_IP_PROF_STOP(&pstCb->stBase, FRCMC_STAT_WORKER);
            continue;
        }

        if (u32ProcBufReqMet(pstCb))
        {
            pthread_mutex_unlock(&pstCb->mutex);
            u32VppIpFrcMc_ProcessBuffer(pstCb);
            vVppIpFrcMc_HandlePendingDrain(pstCb);
            VPP_IP_PROF_STOP(&pstCb->stBase, FRCMC_STAT_WORKER);
            continue;
        }

        pthread_mutex_unlock(&pstCb->mutex);
        VPP_IP_PROF_STOP(&pstCb->stBase, FRCMC_STAT_WORKER);
    } //while (1)

    LOGI("%s exited", __func__);

    return NULL;
}

/************************************************************************
 * Global Functions
 ************************************************************************/
void *vpVppIpFrcMc_Init(t_StVppCtx *pstCtx, uint32_t u32Flags, t_StVppCallback cbs)
{
    LOGI("%s", __func__);

    int rc;
    uint32_t u32;
    t_StVppIpFrcMcCb *pstCb;
    uint32_t u32Length;
    uint32_t u32Ret;

    VPP_RET_IF_NULL(pstCtx, NULL);

    if (u32VppIp_ValidateCallbacks(&cbs) != VPP_OK)
    {
        LOGE("given invalid callbacks.");
        goto ERROR_CALLBACKS;
    }

    pstCb = calloc(sizeof(t_StVppIpFrcMcCb), 1);
    if (!pstCb)
    {
        LOGE("calloc failed for frc context");
        goto ERROR_MALLOC_CONTEXT;
    }
    LOGD("%s pstCb=%p", __func__,pstCb);

    u32VppIp_SetBase(pstCtx, u32Flags, cbs, &pstCb->stBase);

    vVppIpFrcMc_StateSet(pstCb, FRCMC_STATE_NULL);

    u32 = VPP_IP_PROF_REGISTER(&pstCb->stBase, astFrcMcStatsCfg, u32FrcMcStatCnt);
    LOGE_IF(u32 != VPP_OK, "ERROR: unable to register stats, u32=%u", u32);

    u32Length = sizeof(vpp_svc_frc_params_t);

    pstCb->pstHvxCore = pstVppIpHvxCore_Init(pstCb->stBase.pstCtx, u32Flags,
                                             FRCMC_BLOCK_MAX, u32Length);
    if (!pstCb->pstHvxCore)
    {
        LOGE("Failed to init HVX Core.");
        goto ERROR_CORE_INIT;
    }

    pstCb->pstGlobalFrcMcParams = &stGlobalFrcMcParams;
    u32Ret = u32VppIpFrcMc_RegisterGlobalParameterUpdating(pstCb);
    if (u32Ret != VPP_OK)
    {
        LOGE("u32VppIpFrcMc_RegisterGlobalParameterUpdating() failed for hvx context");
        goto ERROR_CORE_INIT;
    }

    if (u32VppBufPool_Init(&pstCb->stInput.stPendingQ) != VPP_OK)
    {
        LOGE("unable to u32VppBufPool_Init() input queue\n");
        goto ERROR_PENDING_INPUT_Q_INIT;
    }

    if (u32VppBufPool_Init(&pstCb->stMbi.stPendingQ) != VPP_OK)
    {
        LOGE("unable to u32VppBufPool_Init() input queue\n");
        goto ERROR_PENDING_MBI_Q_INIT;
    }

    if (u32VppBufPool_Init(&pstCb->stOutput.stPendingQ) != VPP_OK)
    {
        LOGE("unable to u32VppBufPool_Init() output queue\n");
        goto ERROR_PENDING_OUTPUT_Q_INIT;
    }

    if (vpp_queue_init(&pstCb->stCmdQ, FRCMC_CMD_Q_SZ) != VPP_OK)
    {
        LOGE("unable to vpp_queue_init");
        goto ERROR_CMD_Q_INIT;
    }

    rc = sem_init(&pstCb->sem, 0, 0);
    if (rc)
    {
        LOGE("unable to initialize hvx mutex");
        goto ERROR_SEM_INIT;
    }

    rc = pthread_mutex_init(&pstCb->mutex, NULL);
    if (rc)
    {
        LOGE("unable to initialize hvx mutex");
        goto ERROR_MUTEX_INIT;
    }

    rc = pthread_cond_init(&pstCb->cond, NULL);
    if (rc)
    {
        LOGE("unable to init condition variable");
        goto ERROR_COND_INIT;
    }

    rc = pthread_create(&pstCb->thread, NULL, vpVppIpFrcMc_Worker, pstCb);
    if (rc)
    {
        LOGE("unable to spawn hvx worker thread");
        goto ERROR_THREAD_CREATE;
    }

    //vVppIpFrcMc_SetUseCase();
    vVppIpFrcMc_InitCapabilityResources(pstCb);

    //copy from global to local
    memcpy((void*) &pstCb->stLocalFrcMcParams, (void*) &stGlobalFrcMcParams, sizeof(t_StCustomFrcMcParams));

    vVppIpFrcMc_InitParam(pstCb);

    // Wait for the thread to launch before returning
    vVppIpFrcMc_WaitWorkerStart(pstCb);

    VPP_IP_STATE_SET(pstCb, VPP_IP_STATE_INITED);

    return pstCb;

ERROR_THREAD_CREATE:
    LOGI("destroying condition variable");
    pthread_cond_destroy(&pstCb->cond);

ERROR_COND_INIT:
    LOGI("destroying mutex");
    pthread_mutex_destroy(&pstCb->mutex);

ERROR_MUTEX_INIT:
    LOGI("destroying semaphore");
    sem_destroy(&pstCb->sem);

ERROR_SEM_INIT:
    vpp_queue_term(&pstCb->stCmdQ);

ERROR_CMD_Q_INIT:
    u32VppBufPool_Term(&pstCb->stOutput.stPendingQ);

ERROR_PENDING_OUTPUT_Q_INIT:
    u32VppBufPool_Term(&pstCb->stMbi.stPendingQ);

ERROR_PENDING_MBI_Q_INIT:
    u32VppBufPool_Term(&pstCb->stInput.stPendingQ);

ERROR_PENDING_INPUT_Q_INIT:
    vVppIpHvxCore_Term(pstCb->pstHvxCore);

ERROR_CORE_INIT:
    u32 = VPP_IP_PROF_UNREGISTER(&pstCb->stBase);
    LOGE_IF(u32 != VPP_OK, "ERROR: unable to unregister stats, u32=%u", u32);

    if (pstCb)
        free(pstCb);

ERROR_MALLOC_CONTEXT:
ERROR_CALLBACKS:
    return NULL;
}

void vVppIpFrcMc_Term(void *ctx)
{
    int rc;
    uint32_t u32;
    t_StVppIpFrcMcCb *pstCb;
    t_StVppIpCmd stCmd;

    LOGI("%s\n", __func__);

    VPP_RET_VOID_IF_NULL(ctx);
    pstCb = FRCMC_CB_GET(ctx);

    if (!pstCb)
    {
        LOGD("Try to free NULL pstCb\n");
        return;
    }

    if (VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_ACTIVE))
    {
        LOGD("%s() Still in active state. Closing.", __func__);
        u32VppIpFrcMc_Close(ctx);
    }
    u32VppIpFrcMc_FlushPort(pstCb, VPP_PORT_INPUT);
    u32VppIpFrcMc_FlushPort(pstCb, VPP_PORT_OUTPUT);

    u32VppIpFrcMc_UnregisterGlobalParameterUpdating(pstCb);

    stCmd.eCmd = VPP_IP_CMD_THREAD_EXIT;
    u32VppIpFrcMc_CmdPut(pstCb, stCmd);

    rc = pthread_join(pstCb->thread, NULL);
    if (rc)
        LOGE("pthread_join failed: %d --> %s", rc, strerror(rc));

    rc = pthread_cond_destroy(&pstCb->cond);
    if (rc)
        LOGE("pthread_cond_destroy failed: %d --> %s", rc, strerror(rc));

    rc = pthread_mutex_destroy(&pstCb->mutex);
    if (rc)
        LOGE("pthread_mutex_destroy failed: %d --> %s", rc, strerror(rc));

    rc = sem_destroy(&pstCb->sem);
    if (rc)
        LOGE("sem_destroy failed: %d --> %s", rc, strerror(rc));

    vpp_queue_term(&pstCb->stCmdQ);

    u32VppBufPool_Term(&pstCb->stInput.stPendingQ);
    u32VppBufPool_Term(&pstCb->stMbi.stPendingQ);
    u32VppBufPool_Term(&pstCb->stOutput.stPendingQ);

    vVppIpHvxCore_Term(pstCb->pstHvxCore);

    u32 = VPP_IP_PROF_UNREGISTER(&pstCb->stBase);
    LOGE_IF(u32 != VPP_OK, "ERROR: unable to unregister stats, u32=%u", u32);

    free(pstCb);
}

uint32_t u32VppIpFrcMc_Open(void *ctx)
{
    LOGI("%s\n", __func__);

    t_StVppIpFrcMcCb *pstCb;
    t_StVppIpCmd stCmd;

    VPP_RET_IF_NULL(ctx, VPP_ERR_PARAM);
    pstCb = FRCMC_CB_GET(ctx);

    if (!VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_INITED))
        return VPP_ERR_STATE;

    // Validate that the port configuration is valid
    if (u32VppIpFrcMc_ValidateConfig(pstCb->stInput.stParam, pstCb->stOutput.stParam) != VPP_OK)
        return VPP_ERR_PARAM;

    stCmd.eCmd = VPP_IP_CMD_OPEN;
    u32VppIpFrcMc_CmdPut(pstCb, stCmd);

    LOGI(">> waiting on semaphore");
    sem_wait(&pstCb->sem);
    LOGI(">> got semaphore");

    return pstCb->async_res.u32OpenRet;
}

uint32_t u32VppIpFrcMc_Close(void *ctx)
{
    LOGI("%s\n", __func__);

    t_StVppIpFrcMcCb *pstCb;
    t_StVppIpCmd stCmd;

    VPP_RET_IF_NULL(ctx, VPP_ERR_PARAM);
    pstCb = FRCMC_CB_GET(ctx);

    if (!VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_ACTIVE))
    {
        LOGD("%s() VPP_ERR_STATE. pstCb->eState=%d", __func__, pstCb->eState);
        return VPP_ERR_STATE;
    }

    stCmd.eCmd = VPP_IP_CMD_CLOSE;
    u32VppIpFrcMc_CmdPut(pstCb, stCmd);

    LOGI(">> waiting on semaphore");
    sem_wait(&pstCb->sem);
    LOGI(">> got semaphore");

    return pstCb->async_res.u32CloseRet;
}

uint32_t u32VppIpFrcMc_SetParam(void *ctx, enum vpp_port ePort,
                                struct vpp_port_param stParam)
{
    LOGI("%s\n", __func__);

    uint32_t u32Ret = VPP_OK;
    t_StVppIpFrcMcCb *pstCb;

    VPP_RET_IF_NULL(ctx, VPP_ERR_PARAM);
    pstCb = FRCMC_CB_GET(ctx);

    if (!VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_INITED))
    {
        LOGD("%s: state is not VPP_IP_STATE_INITED", __func__);
        return VPP_ERR_STATE;
    }

    if (u32VppIpFrcMc_ValidatePortParam(stParam) != VPP_OK)
    {
        LOGE("given invalid port params.");
        return VPP_ERR_PARAM;
    }

    pthread_mutex_lock(&pstCb->mutex);

    if (ePort == VPP_PORT_INPUT)
        pstCb->stInput.stParam = stParam;
    else if (ePort == VPP_PORT_OUTPUT)
        pstCb->stOutput.stParam = stParam;
    else
        u32Ret = VPP_ERR_PARAM;

    pstCb->stCfg.u32ComputeMask |= FRCMC_PARAM;

    pthread_mutex_unlock(&pstCb->mutex);

    return u32Ret;
}

uint32_t u32VppIpFrcMc_SetCtrl(void *ctx, struct hqv_control stCtrl)
{
    LOGI("%s\n", __func__);

    uint32_t u32Ret;
    t_StVppIpFrcMcCb *pstCb;

    VPP_RET_IF_NULL(ctx, VPP_ERR_PARAM);
    pstCb = FRCMC_CB_GET(ctx);

    u32Ret = u32VppIpFrcMc_ValidateCtrl(stCtrl);
    if (u32Ret != VPP_OK)
    {
        LOGE("Control validation failed u32Ret=%u", u32Ret);
        return VPP_ERR_INVALID_CFG;
    }

    pthread_mutex_lock(&pstCb->mutex);
    if (stCtrl.ctrl_type == HQV_CONTROL_GLOBAL_DEMO)
    {
        pstCb->stCfg.stDemo = stCtrl.demo;
        if (stCtrl.demo.process_percent < FRCMC_ROI_PERCENTAGE_MAX)
        {
            pstCb->stCfg.u32EnableMask |= FRC_ALGO_ROI;
            pstCb->stCfg.u32ComputeMask |= FRC_ALGO_ROI;
        }
        else
        {
            pstCb->stCfg.u32EnableMask &= ~FRC_ALGO_ROI;
        }
    }
    else if (stCtrl.mode == HQV_MODE_AUTO)
    {
        pstCb->stCfg.u32AutoHqvEnable = 1;
        pstCb->stCfg.stFrc.mode = HQV_FRC_MODE_SMOOTH_MOTION;
        pstCb->stCfg.stFrc.level = HQV_FRC_LEVEL_HIGH;
        pstCb->stCfg.u32EnableMask |= FRC_ALGO_MC;
        pstCb->stCfg.u32ComputeMask |= FRC_ALGO_MC;

        pstCb->stCfg.u32ComputeMask |= FRCMC_PARAM;
    }
    else if (stCtrl.mode == HQV_MODE_MANUAL)
    {
        pstCb->stCfg.u32AutoHqvEnable = 0;
        if (stCtrl.ctrl_type == HQV_CONTROL_FRC)
        {
            pstCb->stCfg.stFrc = stCtrl.frc;

            if (pstCb->stCfg.stFrc.mode != HQV_FRC_MODE_OFF)
            {
                pstCb->stCfg.u32EnableMask |= FRC_ALGO_MC;
                pstCb->stCfg.u32ComputeMask |= FRC_ALGO_MC;
            }
            else
            {
                pstCb->stCfg.u32EnableMask &= ~FRC_ALGO_MC;
            }
        }
        else if (stCtrl.ctrl_type == HQV_CONTROL_CUST)
        {
            u32Ret = u32VppIpFrcMc_SetCustomCtrl(pstCb, &stCtrl.custom);
            if (u32Ret != VPP_OK)
            {
                LOGE("Failed ctrl_type == HQV_CONTROL_CUST, u32Ret=%u", u32Ret);
                pthread_mutex_unlock(&pstCb->mutex);
                return VPP_ERR_INVALID_CFG;
            }
            LOGD("%s() OK for stCtrl.ctrl_type == HQV_CONTROL_CUST", __func__);
            //To set parameter again
            pstCb->stCfg.u32ComputeMask |= FRCMC_PARAM; //This should be enough
        }
    }
    else if (stCtrl.mode == HQV_MODE_OFF)
    {
        LOGD("%s(): ctrl.mode is HQV_MODE_OFF", __func__);
        pstCb->stCfg.u32EnableMask &= ~FRC_ALGO_MC;
    }
    pthread_mutex_unlock(&pstCb->mutex);

    return VPP_OK;
}

uint32_t u32VppIpFrcMc_GetBufferRequirements(void *ctx,
                                             t_StVppIpBufReq *pstInputBufReq,
                                             t_StVppIpBufReq *pstOutputBufReq)

{
    LOGI("%s\n", __func__);

    uint32_t u32PxSz;
    t_StVppIpFrcMcCb *pstCb;

    VPP_RET_IF_NULL(ctx, VPP_ERR_PARAM);
    pstCb = FRCMC_CB_GET(ctx);

    if (pstInputBufReq)
    {
        pstInputBufReq->u32MinCnt = pstCb->u32InputMaxIntoProc;
    }
    if (pstOutputBufReq)
    {
        u32PxSz = u32VppUtils_GetPxBufferSize(&pstCb->stOutput.stParam);
        pstOutputBufReq->u32PxSz = u32PxSz;
        pstOutputBufReq->u32MinCnt = pstCb->u32OutputMaxIntoProc;
    }

    return VPP_OK;
}

uint32_t u32VppIpFrcMc_QueueBuf(void *ctx, enum vpp_port ePort,
                                t_StVppBuf *pBuf)

{
    uint32_t u32Ret = VPP_OK;
    t_StVppIpFrcMcCb *pstCb;

    VPP_RET_IF_NULL(ctx, VPP_ERR_PARAM);
    pstCb = FRCMC_CB_GET(ctx);

    VPP_RET_IF_NULL(pstCb, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pBuf, VPP_ERR_PARAM);

    if (!VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_ACTIVE))
    {
        LOGE("eState=%d should be VPP_IP_STATE_ACTIVE=%d", pstCb->eState, VPP_IP_STATE_ACTIVE);
        return VPP_ERR_STATE;
    }

    vVppIpCbLog(&pstCb->stBase.stCb, pBuf, eVppLogId_IpQueueBuf);

#ifdef FRCMC_FORCE_DUMP_BUF
    VPP_FLAG_SET(pBuf->u32InternalFlags, VPP_BUF_FLAG_DUMP);
#endif

    if (ePort == VPP_PORT_INPUT)
    {
        VPP_FLAG_CLR(pBuf->u32InternalFlags, VPP_BUF_FLAG_EOS_PROCESSED);
        VPP_FLAG_CLR(pBuf->u32InternalFlags, VPP_BUF_FLAG_IDR_PROCESSED);
        VPP_FLAG_CLR(pBuf->u32InternalFlags, VPP_BUF_FLAG_DC_PROCESSED);
        VPP_FLAG_CLR(pBuf->u32InternalFlags, VPP_BUF_FLAG_INTERNAL_BYPASS);

        if (pBuf->eBufPxType == eVppBufPxDataType_Raw)
        {
            if (pBuf->stPixel.u32FilledLen == 0)
            {
                VPP_FLAG_SET(pBuf->u32InternalFlags, VPP_BUF_FLAG_INTERNAL_BYPASS);
            }
#ifdef FRCMC_DUMP_FRAME_ENABLE
            if ((VPP_FLAG_IS_SET(pBuf->u32InternalFlags, VPP_BUF_FLAG_DUMP)) &&
                (pBuf->stPixel.u32FilledLen != 0))
            {
                vVppIpFrcMc_DumpFrame(pstCb, pBuf, VPP_PORT_INPUT);
            }
#endif
            u32Ret = u32VppIp_PortBufPut(&pstCb->stInput, pBuf,
                                              &pstCb->mutex, &pstCb->cond);
            pstCb->stats.u32InYuvQCnt++;
        }
        else
        {
            u32Ret = u32VppIp_PortBufPut(&pstCb->stMbi, pBuf,
                                         &pstCb->mutex, &pstCb->cond);
            pstCb->stats.u32InMbiQCnt++;
        }
    }
    else if (ePort == VPP_PORT_OUTPUT)
    {
        u32Ret = u32VppIp_PortBufPut(&pstCb->stOutput, pBuf,
                                     &pstCb->mutex, &pstCb->cond);
        pstCb->stats.u32OutYuvQCnt++;
    }
    else
    {
        LOGE("received buffer on invalid port, port=%u", ePort);
        u32Ret = VPP_ERR_PARAM;
    }

    return u32Ret;
}

uint32_t u32VppIpFrcMc_Flush(void *ctx, enum vpp_port ePort)
{
    LOGI("%s\n", __func__);

    uint32_t u32Ret = VPP_OK;
    t_StVppIpFrcMcCb *pstCb;
    t_StVppIpCmd stCmd;

    VPP_RET_IF_NULL(ctx, VPP_ERR_PARAM);
    pstCb = FRCMC_CB_GET(ctx);

    if (ePort != VPP_PORT_INPUT && ePort != VPP_PORT_OUTPUT)
    {
        LOGE("ERROR: calling flush with invalid port, port=%u", ePort);
        return VPP_ERR_PARAM;
    }

    stCmd.eCmd = VPP_IP_CMD_FLUSH;
    stCmd.flush.ePort = ePort;
    u32VppIpFrcMc_CmdPut(pstCb, stCmd);

    return u32Ret;
}

uint32_t u32VppIpFrcMc_Drain(void *ctx)
{
    LOGI("%s\n", __func__);

    uint32_t u32Ret = VPP_OK;
    t_StVppIpFrcMcCb *pstCb;
    t_StVppIpCmd stCmd;

    VPP_RET_IF_NULL(ctx, VPP_ERR_PARAM);
    pstCb = FRCMC_CB_GET(ctx);

    if (!VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_ACTIVE))
    {
        LOGE("ERROR: calling drain in invalid state, expected=%u, actual=%u",
             VPP_IP_STATE_ACTIVE, pstCb->eState);
        return VPP_ERR_STATE;
    }

    stCmd.eCmd = VPP_IP_CMD_DRAIN;
    u32VppIpFrcMc_CmdPut(pstCb, stCmd);

    return u32Ret;
}

uint32_t u32VppIpFrcMc_Reconfigure(void *ctx,
                                   struct vpp_port_param in_param,
                                   struct vpp_port_param out_param)
{
    LOG_ENTER();

    uint32_t u32Ret = VPP_OK;
    t_StVppIpFrcMcCb *pstCb;
    t_StVppIpCmd stCmd;
    uint32_t u32InQCnt;

    VPP_RET_IF_NULL(ctx, VPP_ERR_PARAM);
    pstCb = FRCMC_CB_GET(ctx);

    if (!VPP_IP_STATE_EQUAL(pstCb, VPP_IP_STATE_ACTIVE))
        return VPP_ERR_STATE;

    // Validate that the port configuration is valid
    if (u32VppIpFrcMc_ValidateConfig(in_param, out_param) != VPP_OK)
        return VPP_ERR_PARAM;

    pthread_mutex_lock(&pstCb->mutex);

    u32InQCnt = u32VppBufPool_Cnt(&pstCb->stInput.stPendingQ);
    if (u32InQCnt)
    {
        LOGE("Reconfig but still %u buffers in input queue", u32InQCnt);
        pthread_mutex_unlock(&pstCb->mutex);
        return VPP_ERR_STATE;
    }

    pstCb->stInput.stParam = in_param;
    pstCb->stOutput.stParam = out_param;
    pstCb->stCfg.u32ComputeMask |= FRCMC_PARAM;
    vVppIpFrcMc_Compute(pstCb);
    pthread_mutex_unlock(&pstCb->mutex);

    pstCb->async_res.u32ReconfigRet = VPP_OK;
    stCmd.eCmd = VPP_IP_CMD_RECONFIGURE;
    u32VppIpFrcMc_CmdPut(pstCb, stCmd);

    LOGI(">> waiting on semaphore (reconfig)");
    sem_wait(&pstCb->sem);
    LOGI(">> got semaphore (reconfig)");

    if (pstCb->async_res.u32ReconfigRet != VPP_OK)
    {
        u32Ret = pstCb->async_res.u32ReconfigRet;
        LOGE("ERROR: reconfigure failed, u32=%u", u32Ret);
    }

    // Reset proc stats
    VPP_IP_PROF_RESET_SINGLE(&pstCb->stBase, FRCMC_STAT_PROC);

    LOG_EXIT_RET(u32Ret);
}
