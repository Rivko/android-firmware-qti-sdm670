/*!
 * @file vpp_ip_hcp.c
 *
 * @cr
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services
 */

#include <sys/types.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <inttypes.h>

#define VPP_LOG_TAG     VPP_LOG_MODULE_HCP_TAG
#define VPP_LOG_MODULE  VPP_LOG_MODULE_HCP
#include "vpp_dbg.h"

#include "vpp.h"
#include "vpp_reg.h"
#include "vpp_queue.h"
#include "vpp_ion.h"
#include "vpp_ip.h"
#include "vpp_ip_hcp_priv.h"
#include "vpp_ip_hcp_hfi.h"
#include "vpp_ip_hcp_dbg.h"
#include "vpp_ip_hcp.h"
#include "vpp_ip_hcp_tunings.h"
#include "vpp_utils.h"
#include "vpp_stats.h"

#include "hcp_hfi.h"
#include "hcp_rpc.h"

/************************************************************************
 * Local definitions
 ***********************************************************************/

#define HCP_GET_GLOBAL()        &stGlobalCb

#define HCP_SIGNAL(_p)          pthread_cond_signal(&(_p)->cond)
#define HCP_BROADCAST(_p)       pthread_cond_broadcast(&(_p)->cond)
#define HCP_WAIT(_p)            pthread_cond_wait(&(_p)->cond, &(_p)->mutex)
#define HCP_LOCK(_p)            pthread_mutex_lock(&(_p)->mutex)
#define HCP_UNLOCK(_p)          pthread_mutex_unlock(&(_p)->mutex)
#define MUTEX_LOCK(_p)          pthread_mutex_lock((_p))
#define MUTEX_UNLOCK(_p)        pthread_mutex_unlock((_p))

#define HCP_MEM_ERR_GOTO(_p, _goto) \
    if (!(_p)) { LOGE("unable to allocate memory for: "#_p); goto _goto; }

#define HCP_MAX(_a, _b)         ((_a) > (_b) ? (_a) : (_b))
#define HCP_MIN(_a, _b)         ((_a) > (_b) ? (_b) : (_a))
#define HCP_CLAMP               RANGE_CHECK
#define HCP_OOB(x, min, max)    ((x) < (min) || (x) > (max))

#ifdef HCP_DBG_GLOBAL_STARTUP
#define HCP_DUMP_GLOBAL_FLAG_STATE() LOGI("global flag state=0x%x", stGlobalCb.u.u32);
#else
#define HCP_DUMP_GLOBAL_FLAG_STATE()
#endif

#define DO_LOG_RSP_MSG(_l, _pcpfx, _phdr) \
    LOG##_l("%s: eMsgId=%u (%s), hw_id=%d, u32Size=%u, s32Err=%d", \
            _pcpfx ? _pcpfx : "", \
            (_phdr)->eMsgId, pcGetMsgIdStr((_phdr)->eMsgId), (_phdr)->s32SessId, \
            (_phdr)->u32Size, (_phdr)->s32Err)

#define LOG_RSP_MSG(_err, _pcpfx, _phdr) ({ \
    if (_err) { DO_LOG_RSP_MSG(E, _pcpfx, _phdr); } \
    else { DO_LOG_RSP_MSG(D, _pcpfx, _phdr); } \
})

#define DO_LOG_RSP_MSG_ARGS(_l, _pcpfx, _phdr, _fmt, _args...) \
    LOG##_l("%s: eMsgId=%u (%s), hw_id=%d, u32Size=%u, s32Err=%d, " _fmt, \
            _pcpfx ? _pcpfx : "", \
            (_phdr)->eMsgId, pcGetMsgIdStr((_phdr)->eMsgId), (_phdr)->s32SessId, \
            (_phdr)->u32Size, (_phdr)->s32Err, ##_args)

#define LOG_RSP_MSG_ARGS(_err, _pcpfx, _phdr, _fmt, _args...) ({ \
    if (_err) { DO_LOG_RSP_MSG_ARGS(E, _pcpfx, _phdr, _fmt, _args); } \
    else { DO_LOG_RSP_MSG_ARGS(D, _pcpfx, _phdr, _fmt, _args); } \
})

typedef union {
    struct {
        // input bypass
        uint32_t bWakeBypass        : 1; // if buffer has INTERNAL_BYPASS set
        uint32_t bOpModeByp         : 1; // if op mode is bypass
        uint32_t bBypFlag           : 1; // if bypass flag is set
        uint32_t bInputFlZero       : 1; // if input filled_len == 0
        uint32_t bInputInvSz        : 1; // if input has invalid buffer size
        uint32_t bInputInvFldType   : 1; // if input is not progressive
        uint32_t bFwWouldByp        : 1; // if firmware would bypass it

        // output bypass
        uint32_t bOutputInvSz       : 1; // if output buffer is too small

        // Normal processing mode
        uint32_t bBufReqMet         : 1; // if buffer requirements are met
        uint32_t bWakeNormal        : 1; // normal operation

        // Result
        uint32_t bCanProcess        : 1;
        uint32_t bCanBypassIn       : 1;
        uint32_t bCanBypassOut      : 1;
    };
    uint32_t u32Res;
} t_UVppIpHcpCanProcRes;

typedef struct {
    uint32_t eBufType;
    t_StVppIpHcpSessionCb *pstSessCb;
    uint32_t u32CL;
    uint32_t u32CH;
} t_StVppIpHcpExpBuf;

/************************************************************************
 * Local static variables
 ***********************************************************************/

VPP_STAT_DECL(HCP_STATS, HcpStats);

static t_StVppIpHcpGlobalCb stGlobalCb = {
    .cond = PTHREAD_COND_INITIALIZER,
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .hndl = HCP_HNDL_UNINIT,
};

/************************************************************************
 * Forward Declarations
 ************************************************************************/

/************************************************************************
 * Local Functions
 ***********************************************************************/

static uint32_t u32VppIpHcp_MatchBuf(t_StVppBuf *pstBuf, void *pv)
{
    uint32_t u32CL, u32CH;
    t_StVppIpHcpSessionCb *pstSessCb;
    t_StVppIpHcpExpBuf *pstExp = (t_StVppIpHcpExpBuf *)pv;

    if (!pstExp || !pstBuf)
        return VPP_FALSE;

    if (pstExp->eBufType == BUFF_TYPE_PIXEL)
    {
        u32CH = HCP_P_COOKIE_H(pstExp->pstSessCb, pstBuf);
        u32CL = HCP_P_COOKIE_L(pstExp->pstSessCb, pstBuf);
    }
    else if (pstExp->eBufType == BUFF_TYPE_MEAS)
    {
        u32CH = HCP_M_COOKIE_H(pstExp->pstSessCb, pstBuf);
        u32CL = HCP_M_COOKIE_L(pstExp->pstSessCb, pstBuf);
    }
    else
    {
        LOGE("cannot match invalid type=%u, cl=0x%x, ch=0x%x",
             pstExp->eBufType, pstExp->u32CL, pstExp->u32CH);
        return VPP_FALSE;
    }

    if (u32CH == pstExp->u32CH && u32CL == pstExp->u32CL)
    {
        LOGD("found matching buffer, type=%u, cl=0x%x, ch=0x%x, pstBuf=%p",
             pstExp->eBufType, pstExp->u32CL, pstExp->u32CH, pstBuf);
        return VPP_TRUE;
    }

    LOGI("did not find matching buffer, type=%u, cl=0x%x, ch=0x%x, pstBuf=%p",
         pstExp->eBufType, pstExp->u32CL, pstExp->u32CH, pstBuf);

    return VPP_FALSE;
}

void vVppIpHcp_DumpFrame(t_StVppIpHcpSessionCb *pstCb, t_StVppBuf *pstBuf,
                         enum vpp_port port)
{
    char cPath[HCP_DUMP_NM_LEN];
    struct vpp_port_param *pstParam;

    VPP_RET_VOID_IF_NULL(pstCb);
    VPP_RET_VOID_IF_NULL(pstBuf);

    if (pstCb->stBase.bSecureSession)
        return;

    if (port == VPP_PORT_INPUT)
    {
        snprintf(cPath, HCP_DUMP_NM_LEN, HCP_DUMP_BUF_IN_NM, pstCb);
        pstParam = &pstCb->stInput.stParam;
    }
    else
    {
        snprintf(cPath, HCP_DUMP_NM_LEN, HCP_DUMP_BUF_OUT_NM, pstCb);
        pstParam = &pstCb->stOutput.stParam;
    }

    u32VppBuf_Dump(pstCb->stBase.pstCtx, pstBuf, *pstParam, cPath);
}

static void vVppIpHcp_DumpRspMsg(t_StHcpHfiMsgPkt *pstMsg, uint32_t bErr,
                                 const char *pcPrefix)
{
    t_StHcpHfiMsgPktHdr *pstHdr = &pstMsg->stHdr;
    switch (pstHdr->eMsgId)
    {
        case MSG_ID_SET_PROPERTY_DONE:
        {
            uint32_t u32Prop;
            t_StHcpHfiMsgSetProp *pstMsgSetProp = &pstMsg->stMsgSetProp;
            u32Prop = pstMsgSetProp->propId;
            LOG_RSP_MSG_ARGS(bErr, pcPrefix, pstHdr,
                             "propId=%u (%s)", u32Prop, pcGetPropIdStr(u32Prop));
            break;
        }
        case MSG_ID_PROPERTY:
        {
            uint32_t u32Prop;
            t_StHcpHfiMsgGetProp *pstMsgGetProp = &pstMsg->stMsgGetProp;
            u32Prop = pstMsgGetProp->propId;
            LOG_RSP_MSG_ARGS(bErr, pcPrefix, pstHdr,
                             "propId=%u (%s)", u32Prop, pcGetPropIdStr(u32Prop));
            break;
        }
        case MSG_ID_BUFF_RELEASED:
        {
            t_StHcpHfiBufAttr *p = &pstMsg->stMsgBuf.bufAttr;
            if (p->eBufType == BUFF_TYPE_PIXEL)
            {
                t_StHcpHfiPixBufAttr *pa = &p->stPixBufAttr;
#ifdef HCP_DBG_PA_CNTS
                LOG_RSP_MSG_ARGS(bErr, pcPrefix, pstHdr,
                                 "eBufType=%u, cl=0x%x, ch=0x%x, op=%u, dir=%u, "
                                 "pa=0x%x, irq=0x%x, err_irq=0x%x, cnt=%u",
                                 p->eBufType, p->u32Cookie_l, p->u32Cookie_h,
                                 p->u32Operation, pa->eDirection, p->u32Pa,
                                 p->u32IrqStatus, p->u32ErrIrqStatus, p->u32Counts);
#else
                LOG_RSP_MSG_ARGS(bErr, pcPrefix, pstHdr,
                                 "eBufType=%u, cl=0x%x, ch=0x%x, op=%u, dir=%u, "
                                 "cnt=%u",
                                 p->eBufType, p->u32Cookie_l, p->u32Cookie_h,
                                 p->u32Operation, pa->eDirection, p->u32Counts);
#endif
            }
            else
            {
#ifdef HCP_DBG_PA_CNTS
                LOG_RSP_MSG_ARGS(bErr, pcPrefix, pstHdr,
                                 "eBufType=%u, cl=0x%x, ch=0x%x, "
                                 "pa=0x%x, irq=0x%x, err_irq=0x%x, cnt=%u",
                                 p->eBufType, p->u32Cookie_l, p->u32Cookie_h, p->u32Pa,
                                 p->u32IrqStatus, p->u32ErrIrqStatus, p->u32Counts);
#else
                LOG_RSP_MSG_ARGS(bErr, pcPrefix, pstHdr,
                                 "eBufType=%u, cl=0x%x, ch=0x%x, cnt=%u",
                                 p->eBufType, p->u32Cookie_l, p->u32Cookie_h,
                                 p->u32Counts);
#endif
            }
            break;
        }
        case MSG_ID_EVENT_NOTIFY:
            // t_StHcpHfiMsgEvt
            break;

        case MSG_ID_CLOSE_DONE:
        case MSG_ID_NOOP:
            LOG_RSP_MSG(0, pcPrefix, pstHdr);
            break;
        default:
            LOG_RSP_MSG(VPP_TRUE, pcPrefix, pstHdr);
            break;
    }
}

static void vVppIpHcp_ReadSessProps(t_StVppIpHcpSessionCb *pstSessCb)
{
    VPP_RET_VOID_IF_NULL(pstSessCb);

    u32VppUtils_ReadPropertyU32(HCP_PROPERTY_FWLOGFLAGS,
                                &pstSessCb->stDbgCfg.u32LogFlags, "0");

    LOGI("HCP debug levels: u32LogFlags=%u", pstSessCb->stDbgCfg.u32LogFlags);
}

static uint32_t u32VppIpHcp_ValidateCallbacks(t_StVppCallback *pstCbs)
{
    VPP_RET_IF_NULL(pstCbs, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pstCbs->input_buffer_done, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pstCbs->output_buffer_done, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pstCbs->event, VPP_ERR_PARAM);
    return VPP_OK;
}

static uint32_t bVppIpHcp_PortEqual(struct vpp_port_param stPortLeft,
                                    struct vpp_port_param stPortRight)
{
    return (stPortLeft.width == stPortRight.width &&
            stPortLeft.height == stPortRight.height &&
            stPortLeft.stride == stPortRight.stride &&
            stPortLeft.scanlines == stPortRight.scanlines &&
            stPortLeft.fmt == stPortRight.fmt);
}

static uint32_t u32VppIpHcp_GetMaxStride(enum vpp_color_format fmt)
{
    switch (fmt)
    {
        // Intentional fall-through.
        case VPP_COLOR_FORMAT_NV12_VENUS:
        case VPP_COLOR_FORMAT_NV21_VENUS:
            return HCP_WIDTH_MAX;
        case VPP_COLOR_FORMAT_P010:
            return HCP_WIDTH_MAX * 2;
        // Intentional fall-through.
        case VPP_COLOR_FORMAT_UBWC_NV12:
        case VPP_COLOR_FORMAT_UBWC_NV21:
        case VPP_COLOR_FORMAT_UBWC_TP10:
            return u32VppUtils_CalcStrideForPlane(HCP_WIDTH_MAX, fmt, 1);
        default:
            return 0;
    }
}

static uint32_t u32VppIpHcp_ValidatePort(enum vpp_port ePort,
                                         struct vpp_port_param stParam)
{
    uint32_t u32Ret = VPP_OK;

    VPP_UNUSED(ePort);

    if (stParam.fmt != VPP_COLOR_FORMAT_NV12_VENUS &&
        stParam.fmt != VPP_COLOR_FORMAT_NV21_VENUS &&
        stParam.fmt != VPP_COLOR_FORMAT_P010 &&
        stParam.fmt != VPP_COLOR_FORMAT_UBWC_NV12 &&
        stParam.fmt != VPP_COLOR_FORMAT_UBWC_NV21 &&
        stParam.fmt != VPP_COLOR_FORMAT_UBWC_TP10)
    {
        LOGE("ValidatePort() failed, invalid color format, fmt=%u",
             stParam.fmt);
        return VPP_ERR_PARAM;
    }

    if (stParam.width < HCP_WIDTH_MIN ||
        stParam.width > HCP_WIDTH_MAX ||

        stParam.height < HCP_HEIGHT_MIN ||
        stParam.height > HCP_HEIGHT_MAX ||

        stParam.stride < HCP_STRIDE_MIN ||
        stParam.stride > u32VppIpHcp_GetMaxStride(stParam.fmt) ||

        stParam.scanlines < HCP_SCANLINES_MIN ||
        stParam.scanlines > HCP_SCANLINES_MAX)
    {
        LOGE("ValidatePort() failed, w/h/str/scl exceeded bounds");
        return VPP_ERR_PARAM;
    }

    if (!u32VppUtils_IsFmtUbwc(stParam.fmt))
    {
        if (stParam.width > stParam.stride)
        {
            LOGE("ValidatePort() failed, width=%u > stride=%u", stParam.width,
                 stParam.stride);
            return VPP_ERR_PARAM;
        }

        if (stParam.height > stParam.scanlines)
        {
            LOGE("ValidatePort() failed, height=%u > scanlines=%u", stParam.height,
                 stParam.scanlines);
            return VPP_ERR_PARAM;
        }
    }

    if (stParam.width % HCP_WIDTH_MULTIPLE)
    {
        LOGE("ValidatePort() failed, width not multiple of %u",
             HCP_WIDTH_MULTIPLE);
        return VPP_ERR_PARAM;
    }

    if (stParam.height % HCP_HEIGHT_MULTIPLE)
    {
        LOGE("ValidatePort() failed, height not multiple of %u",
             HCP_HEIGHT_MULTIPLE);
        return VPP_ERR_PARAM;
    }

    if (stParam.stride % HCP_STRIDE_MULTIPLE)
    {
        LOGE("ValidatePort() failed, stride not multiple of %u",
             HCP_STRIDE_MULTIPLE);
        return VPP_ERR_PARAM;
    }

    if (stParam.scanlines % HCP_SCANLINES_MULTIPLE)
    {
        LOGE("ValidatePort() failed, scanlines not multiple of %u",
             HCP_SCANLINES_MULTIPLE);
        return VPP_ERR_PARAM;
    }

    return VPP_OK;
}

static uint32_t u32VppIpHcp_ValidateCtrl(struct hqv_control ctrl)
{
    struct hqv_ctrl_aie aie;
    struct hqv_ctrl_ear ear;

    if (ctrl.mode != HQV_MODE_OFF &&
        ctrl.mode != HQV_MODE_AUTO &&
        ctrl.mode != HQV_MODE_MANUAL)
    {
        LOGE("ValidateCtrl() failed, invalid mode, ctrl.mode=%u", ctrl.mode);
        return VPP_ERR_PARAM;
    }

    if (ctrl.mode == HQV_MODE_MANUAL)
    {
        if (ctrl.ctrl_type != HQV_CONTROL_AIE &&
            ctrl.ctrl_type != HQV_CONTROL_EAR &&
            ctrl.ctrl_type != HQV_CONTROL_MEAS &&
            ctrl.ctrl_type != HQV_CONTROL_GLOBAL_DEMO)
        {
            LOGE("ValidateCtrl() failed, invalid type, ctrl.ctrl_type=%u",
                 ctrl.ctrl_type);
            return VPP_ERR_PARAM;
        }

        if (ctrl.ctrl_type == HQV_CONTROL_AIE)
        {
            aie = ctrl.aie;
            if (aie.mode != HQV_MODE_OFF && aie.mode != HQV_MODE_AUTO &&
                aie.mode != HQV_MODE_MANUAL)
            {
                LOGE("ValidateCtrl() failed, invalid aie mode, aie.mode=%u",
                     aie.mode);
                return VPP_ERR_PARAM;
            }

            if (aie.mode == HQV_MODE_MANUAL)
            {
                if (HCP_OOB(aie.cade_level, HCP_DE_GAIN_MIN, HCP_DE_GAIN_MAX) ||
                    HCP_OOB(aie.ltm_sat_gain, HCP_SAT_GAIN_MIN, HCP_SAT_GAIN_MAX) ||
                    HCP_OOB(aie.ltm_sat_offset, HCP_SAT_OFFSET_MIN, HCP_SAT_OFFSET_MAX) ||
                    HCP_OOB(aie.ltm_ace_str, HCP_ACE_STR_MIN, HCP_ACE_STR_MAX) ||
                    HCP_OOB(aie.ltm_ace_bright_l, HCP_ACE_BRI_L_MIN, HCP_ACE_BRI_L_MAX) ||
                    HCP_OOB(aie.ltm_ace_bright_h, HCP_ACE_BRI_H_MIN, HCP_ACE_BRI_H_MAX))
                {
                    LOGE("ValidateCtrl() failed, aie levels oob, cade_level=%u, "
                         "ltm_sat_gain=%u, ltm_sat_off=%u, ltm_ace_str=%u, "
                         "ltm_ace_bright_l=%u, ltm_ace_bright_h=%u", aie.cade_level,
                         aie.ltm_sat_gain, aie.ltm_sat_offset, aie.ltm_ace_str,
                         aie.ltm_ace_bright_l, aie.ltm_ace_bright_h);
                    return VPP_ERR_PARAM;
                }
            }
        }

        if (ctrl.ctrl_type == HQV_CONTROL_EAR)
        {
            ear = ctrl.ear;
            if (ear.mode >= HQV_EAR_MODE_MAX)
            {
                LOGE("ValidateCtrl() failed, ear mode out of bounds, mode=%u",
                     ear.mode);
                return VPP_ERR_PARAM;
            }
        }

        if (ctrl.ctrl_type == HQV_CONTROL_GLOBAL_DEMO)
        {
            if (HCP_OOB(ctrl.demo.process_percent, HCP_SPLIT_MIN, HCP_SPLIT_MAX))
            {
                LOGE("ValidateCtrl() failed, split percent=%u exceeds bounds",
                     ctrl.demo.process_percent);
                return VPP_ERR_PARAM;
            }
        }
    }
    return VPP_OK;
}

static uint32_t u32VppIpHcp_ValidateConfigRaw(t_EVppIpHcpOpMode eOpMode,
                                              struct vpp_port_param stInParam,
                                              struct vpp_port_param stOutParam)
{
    uint32_t u32, u32Ret = VPP_OK;

    u32 = u32VppIpHcp_ValidatePort(VPP_PORT_INPUT, stInParam);
    if (u32 != VPP_OK)
    {
        LOGE("ValidatePort(input) failed, u32=%u", u32);
        u32Ret = VPP_ERR_PARAM;
    }

    if (eOpMode == HCP_OP_MODE_PROC)
    {
        u32 = u32VppIpHcp_ValidatePort(VPP_PORT_OUTPUT, stOutParam);
        if (u32 != VPP_OK)
        {
            LOGE("ValidatePort(output) failed, u32=%u", u32);
            u32Ret = VPP_ERR_PARAM;
        }

        u32 = bVppIpHcp_PortEqual(stInParam, stOutParam);
        if (!u32)
        {
            LOGE("ValidatePort(equal) failed, u32=%u", u32);
            u32Ret = VPP_ERR_PARAM;
        }
    }

    return u32Ret;
}

static uint32_t u32VppIpHcp_ValidateConfig(t_StVppIpHcpSessionCb *pstSessCb)
{
    uint32_t u32Ret = VPP_OK;

    LOG_PARAM_PTR(I, &pstSessCb->stInput.stParam, &pstSessCb->stOutput.stParam);

    u32Ret = u32VppIpHcp_ValidateConfigRaw(pstSessCb->stCfg.eOpMode,
                                           pstSessCb->stInput.stParam,
                                           pstSessCb->stOutput.stParam);

    LOGE_IF(u32Ret != VPP_OK, "ValidateConfigRaw failed, u32=%u", u32Ret);

    return u32Ret;
}

static t_EVppIpHcpOpMode eVppIpHcp_GetOpModeForEnMsk(uint32_t u32EnableMask)
{
    uint32_t u32MeasEn, u32ProcEn;
    t_EVppIpHcpOpMode eOpMode = HCP_OP_MODE_BYPASS;

    u32ProcEn = VPP_FLAG_IS_SET(u32EnableMask, HCP_FLAG_ALGO_PROC);
    u32MeasEn = VPP_FLAG_IS_SET(u32EnableMask, HCP_FLAG_ALGO_MEAS);

    if ((u32MeasEn && u32ProcEn) || (!u32MeasEn && !u32ProcEn))
    {
        eOpMode = HCP_OP_MODE_BYPASS;
    }
    else if (u32MeasEn)
    {
        eOpMode = HCP_OP_MODE_MEAS;
    }
    else if (u32ProcEn)
    {
        eOpMode = HCP_OP_MODE_PROC;
    }

    LOGI("eOpMode=%u for u32EnableMask=0x%x", eOpMode, u32EnableMask);
    return eOpMode;
}

static void vVppIpHcp_ComputeAieOff(t_StVppIpHcpSessionCb *pstSessCb)
{
    // LTM Sat
    pstSessCb->stDrvCtx.u32SatGain = HCP_SAT_GAIN_OFF;
    pstSessCb->stDrvCtx.u32SatOff = HCP_SAT_OFFSET_OFF;

    // LTM ACE
    pstSessCb->stDrvCtx.u32AceStrCon =  HCP_ACE_STR_OFF;
    pstSessCb->stDrvCtx.u32AceStrBriL = HCP_ACE_BRI_L_OFF;
    pstSessCb->stDrvCtx.u32AceStrBriH = HCP_ACE_BRI_H_OFF;

    // DE
    pstSessCb->stDrvCtx.u32DeGain = HCP_DE_GAIN_OFF;
}

static void vVppIpHcp_ComputeEarOff(t_StVppIpHcpSessionCb *pstSessCb)
{
    // EAR
    pstSessCb->stDrvCtx.u32EarMode = HCP_EAR_MODE_OFF;
}

static void vVppIpHcp_Compute(t_StVppIpHcpSessionCb *pstSessCb, uint32_t bLocked)
{
    uint32_t u32ReqSz;
    uint32_t bIsAuto, bAieAuto, bEarAuto;
    t_StVppIpHcpCfg *pstCfg;
    t_StVppIpHcpDrvCtx stTmpCtx;

    VPP_RET_VOID_IF_NULL(pstSessCb);

    pstCfg = &pstSessCb->stCfg;

    if (!bLocked)
        HCP_LOCK(pstSessCb);

    if (!pstCfg->u32ComputeMask)
    {
        if (!bLocked)
            HCP_UNLOCK(pstSessCb);
        return;
    }

    stTmpCtx = pstSessCb->stDrvCtx;

    if (VPP_FLAG_IS_SET(pstCfg->u32ComputeMask, HCP_FLAG_ALGO_PROC) ||
        VPP_FLAG_IS_SET(pstCfg->u32ComputeMask, HCP_FLAG_ALGO_MEAS))
    {
        pstCfg->eOpMode = eVppIpHcp_GetOpModeForEnMsk(pstCfg->u32EnableMask);

        if (VPP_FLAG_IS_SET(pstCfg->u32EnableMask, HCP_FLAG_ALGO_PROC) &&
            !VPP_FLAG_IS_SET(pstCfg->u32EnableMask, HCP_FLAG_ALGO_MEAS))
        {
            bIsAuto = VPP_FLAG_IS_SET(pstCfg->u32EnableMask, HCP_FLAG_ALGO_AUTO);
            bAieAuto = bIsAuto || pstCfg->stAie.mode == HQV_MODE_AUTO;
            bEarAuto = bIsAuto;

            // EAR
            if (bEarAuto)
            {
                pstSessCb->stDrvCtx.u32EarMode = HCP_EAR_MODE_DFLT;
            }
            else
            {
                uint32_t u32EarMode;
                switch (pstCfg->stEar.mode) {
                    case HQV_EAR_MODE_LOW:             u32EarMode = 1; break;
                    case HQV_EAR_MODE_MEDIUM:          u32EarMode = 2; break;
                    case HQV_EAR_MODE_HIGH:            u32EarMode = 3; break;
                    case HQV_EAR_MODE_STREAM_ADAPTIVE: u32EarMode = 4; break;
                    case HQV_EAR_MODE_FRAME_ADAPTIVE:  u32EarMode = 5; break;
                    case HQV_EAR_MODE_OFF:
                    case HQV_EAR_MODE_BYPASS:
                    default:
                                                       u32EarMode = 0; break;
                }
                pstSessCb->stDrvCtx.u32EarMode = u32EarMode;
            }

            // AIE
            if (bAieAuto)
            {
                // LTM Sat
                pstSessCb->stDrvCtx.u32SatGain = HCP_SAT_GAIN_DFLT;
                pstSessCb->stDrvCtx.u32SatOff = HCP_SAT_OFFSET_DFLT;

                // LTM ACE
                pstSessCb->stDrvCtx.u32AceStrCon = HCP_ACE_STR_DFLT;
                pstSessCb->stDrvCtx.u32AceStrBriL = HCP_ACE_BRI_L_DFLT;
                pstSessCb->stDrvCtx.u32AceStrBriH = HCP_ACE_BRI_H_DFLT;

                // DE
                pstSessCb->stDrvCtx.u32DeGain = HCP_DE_GAIN_DFLT;
            }
            else if (pstCfg->stAie.mode == HQV_MODE_OFF)
            {
                vVppIpHcp_ComputeAieOff(pstSessCb);
            }
            else
            {
                // LTM Sat
                pstSessCb->stDrvCtx.u32SatGain =
                    HCP_CLAMP(pstCfg->stAie.ltm_sat_gain,
                              HCP_SAT_GAIN_MIN, HCP_SAT_GAIN_MAX);
                pstSessCb->stDrvCtx.u32SatOff =
                    HCP_CLAMP(pstCfg->stAie.ltm_sat_offset,
                              HCP_SAT_OFFSET_MIN, HCP_SAT_OFFSET_MAX);

                // LTM ACE
                pstSessCb->stDrvCtx.u32AceStrCon =
                    HCP_CLAMP(pstCfg->stAie.ltm_ace_str,
                              HCP_ACE_STR_MIN, HCP_ACE_STR_MAX);
                pstSessCb->stDrvCtx.u32AceStrBriL =
                    HCP_CLAMP(pstCfg->stAie.ltm_ace_bright_l,
                              HCP_ACE_BRI_L_MIN, HCP_ACE_BRI_L_MAX);
                pstSessCb->stDrvCtx.u32AceStrBriH =
                    HCP_CLAMP(pstCfg->stAie.ltm_ace_bright_h,
                              HCP_ACE_BRI_H_MIN, HCP_ACE_BRI_H_MAX);

                // DE
                pstSessCb->stDrvCtx.u32DeGain =
                    HCP_CLAMP(pstCfg->stAie.cade_level,
                              HCP_DE_GAIN_MIN, HCP_DE_GAIN_MAX);
            }
        }
        else
        {
            vVppIpHcp_ComputeEarOff(pstSessCb);
            vVppIpHcp_ComputeAieOff(pstSessCb);
        }

        if (!pstSessCb->stDrvCtx.u32KnobsLoaded ||
            stTmpCtx.u32EarMode != pstSessCb->stDrvCtx.u32EarMode)
            VPP_FLAG_SET(pstCfg->u32LoadMask, HCP_FLAG_ALGO_EAR);

        if (!pstSessCb->stDrvCtx.u32KnobsLoaded ||
            stTmpCtx.u32SatGain != pstSessCb->stDrvCtx.u32SatGain ||
            stTmpCtx.u32SatOff != pstSessCb->stDrvCtx.u32SatOff)
            VPP_FLAG_SET(pstCfg->u32LoadMask, HCP_FLAG_ALGO_LTM_SAT);

        if (!pstSessCb->stDrvCtx.u32KnobsLoaded ||
            stTmpCtx.u32AceStrCon != pstSessCb->stDrvCtx.u32AceStrCon ||
            stTmpCtx.u32AceStrBriL != pstSessCb->stDrvCtx.u32AceStrBriL ||
            stTmpCtx.u32AceStrBriH != pstSessCb->stDrvCtx.u32AceStrBriH)
            VPP_FLAG_SET(pstCfg->u32LoadMask, HCP_FLAG_ALGO_LTM_ACE);

        if (!pstSessCb->stDrvCtx.u32KnobsLoaded ||
            stTmpCtx.u32DeGain != pstSessCb->stDrvCtx.u32DeGain)
            VPP_FLAG_SET(pstCfg->u32LoadMask, HCP_FLAG_ALGO_DE);

        VPP_FLAG_CLR(pstCfg->u32ComputeMask, HCP_FLAG_ALGO_PROC);
        VPP_FLAG_CLR(pstCfg->u32ComputeMask, HCP_FLAG_ALGO_MEAS);

        VPP_FLAG_SET(pstCfg->u32ComputeMask, HCP_FLAG_PORT_PARAM);
    }

    if (VPP_FLAG_IS_SET(pstCfg->u32ComputeMask, HCP_FLAG_ALGO_DEMO))
    {
        // Global demo
        uint32_t u32Per = HCP_CLAMP(pstCfg->stDemo.process_percent,
                                    HCP_SPLIT_MIN, HCP_SPLIT_MAX);
        t_EHcpSplitScreenRegion eReg = HCP_REGION_LEFT;
        switch (pstCfg->stDemo.process_direction)
        {
            case HQV_SPLIT_RIGHT_TO_LEFT: eReg = HCP_REGION_RIGHT;  break;
            case HQV_SPLIT_TOP_TO_BOTTOM: eReg = HCP_REGION_TOP;    break;
            case HQV_SPLIT_BOTTOM_TO_TOP: eReg = HCP_REGION_BOT;    break;
            case HQV_SPLIT_LEFT_TO_RIGHT:
            case HQV_SPLIT_MAX:
            default:                      eReg = HCP_REGION_LEFT;   break;
        }

        pstSessCb->stDrvCtx.uSplitScreen.st.eProcessedRegion = eReg;
        pstSessCb->stDrvCtx.uSplitScreen.st.u32ProcessedPercent = u32Per;

        if (!pstSessCb->stDrvCtx.u32KnobsLoaded ||
            stTmpCtx.uSplitScreen.u32 != pstSessCb->stDrvCtx.uSplitScreen.u32)
            VPP_FLAG_SET(pstCfg->u32LoadMask, HCP_FLAG_ALGO_DEMO);

        VPP_FLAG_CLR(pstCfg->u32ComputeMask, HCP_FLAG_ALGO_DEMO);
    }

    pstSessCb->stDrvCtx.u32KnobsLoaded = VPP_TRUE;

    if (pstSessCb->stCfg.eOpMode == HCP_OP_MODE_BYPASS)
    {
        pstSessCb->stBufReqIn.u32MinCnt = 1;
        pstSessCb->stBufReqOut.u32MinCnt = 0;
    }
    else
    {
        pstSessCb->stBufReqIn.u32MinCnt = pstSessCb->stDrvCtx.req.u32InCntMin;
        pstSessCb->stBufReqOut.u32MinCnt = pstSessCb->stDrvCtx.req.u32OutCntMin;
    }

    if (VPP_FLAG_IS_SET(pstCfg->u32ComputeMask, HCP_FLAG_PORT_PARAM))
    {
        u32ReqSz = u32VppUtils_GetPxBufferSize(&pstSessCb->stInput.stParam);
        pstSessCb->stBufReqIn.u32PxSz = u32ReqSz;
        pstSessCb->stBufReqIn.u32ExSz = 0;

        u32ReqSz = u32VppUtils_GetPxBufferSize(&pstSessCb->stOutput.stParam);
        if (pstSessCb->stCfg.eOpMode == HCP_OP_MODE_PROC)
            pstSessCb->stBufReqOut.u32PxSz = u32ReqSz;
        pstSessCb->stBufReqOut.u32ExSz = 0;

        VPP_FLAG_CLR(pstCfg->u32ComputeMask, HCP_FLAG_PORT_PARAM);
    }

    LOGD("Compute(): eOpMode=%u, u32EnableMask=0x%x, u32LoadMask=0x%x",
         pstCfg->eOpMode, pstCfg->u32EnableMask, pstCfg->u32LoadMask);

    if (!bLocked)
        HCP_UNLOCK(pstSessCb);
}

static void vVppIpHcp_InitDefaultCtrls(t_StVppIpHcpSessionCb *pstSessCb)
{
    VPP_RET_VOID_IF_NULL(pstSessCb);

    memset(&pstSessCb->stCfg.stAie, 0, sizeof(struct hqv_ctrl_aie));
    memset(&pstSessCb->stCfg.stEar, 0, sizeof(struct hqv_ctrl_ear));
    memset(&pstSessCb->stCfg.stMeas, 0, sizeof(struct hqv_ctrl_meas));
    memset(&pstSessCb->stCfg.stDemo, 0, sizeof(struct hqv_ctrl_global_demo));

    pstSessCb->stCfg.stDemo.process_percent = HCP_SPLIT_DFLT;
    pstSessCb->stCfg.stDemo.process_direction = HQV_SPLIT_LEFT_TO_RIGHT;

    pstSessCb->stCfg.u32EnableMask = 0;
    VPP_FLAG_SET(pstSessCb->stCfg.u32ComputeMask, HCP_FLAG_ALGO_MEAS);
    VPP_FLAG_SET(pstSessCb->stCfg.u32ComputeMask, HCP_FLAG_ALGO_PROC);
    VPP_FLAG_SET(pstSessCb->stCfg.u32ComputeMask, HCP_FLAG_ALGO_DEMO);
}

static void vVppIpHcp_InitDefaultParams(t_StVppIpHcpSessionCb *pstSessCb)
{
    LOG_ENTER();

    VPP_RET_VOID_IF_NULL(pstSessCb);

    pstSessCb->stDrvCtx.eOpModeActive = HCP_OP_MODE_BYPASS;

    vVppIpHcp_InitDefaultCtrls(pstSessCb);

    vVppIpHcp_Compute(pstSessCb, VPP_FALSE);
    pstSessCb->stIntBufs.bDirty = VPP_TRUE;

    LOG_EXIT_RET();
}

static void vVppIpHcp_InitStaticParams(t_StVppIpHcpSessionCb *pstSessCb)
{
    uint32_t u32;
    struct video_property stVidProp;

    VPP_RET_VOID_IF_NULL(pstSessCb);

    memset(&stVidProp, 0, sizeof(struct video_property));
    stVidProp.property_type = VID_PROP_CODEC;

    u32 = u32VppUtils_GetVidProp(pstSessCb->stBase.pstCtx, &stVidProp);
    if (u32 == VPP_OK)
        pstSessCb->stDrvCtx.u32CodecType =
            u32VppIpHcp_VppToHfiCodec(stVidProp.codec.eCodec);
    else
        pstSessCb->stDrvCtx.u32CodecType =
            u32VppIpHcp_VppToHfiCodec(VPP_CODEC_TYPE_MAX);
}

static uint32_t u32VppIpHcp_GetNewEnableMask(t_StVppIpHcpCfg stCfg,
                                             struct hqv_control ctrl,
                                             t_EVppIpState eState,
                                             uint32_t *pu32NewMask)
{
    uint32_t u32OldMask = stCfg.u32EnableMask;

    if (u32VppIpHcp_ValidateCtrl(ctrl))
        return VPP_ERR_PARAM;

    if (!pu32NewMask)
        return VPP_ERR_PARAM;

    if (ctrl.mode == HQV_MODE_OFF)
    {
        VPP_FLAG_CLR(u32OldMask, HCP_FLAG_ALGO_MEAS);
        VPP_FLAG_CLR(u32OldMask, HCP_FLAG_ALGO_PROC);
        VPP_FLAG_CLR(u32OldMask, HCP_FLAG_ALGO_AUTO);
    }
    else if (ctrl.mode == HQV_MODE_AUTO)
    {
        VPP_FLAG_CLR(u32OldMask, HCP_FLAG_ALGO_MEAS);
        VPP_FLAG_SET(u32OldMask, HCP_FLAG_ALGO_PROC);
        VPP_FLAG_SET(u32OldMask, HCP_FLAG_ALGO_AUTO);
    }
    else if (ctrl.mode == HQV_MODE_MANUAL)
    {
        if (ctrl.ctrl_type == HQV_CONTROL_AIE ||
            ctrl.ctrl_type == HQV_CONTROL_EAR)
        {
            VPP_FLAG_CLR(u32OldMask, HCP_FLAG_ALGO_AUTO);

            if (ctrl.ctrl_type == HQV_CONTROL_AIE)
            {
                stCfg.stAie = ctrl.aie;
            }
            else if (ctrl.ctrl_type == HQV_CONTROL_EAR)
            {
                stCfg.stEar = ctrl.ear;
            }

            if (stCfg.stAie.mode != HQV_MODE_OFF ||
                stCfg.stEar.mode != HQV_EAR_MODE_OFF)
            {
                VPP_FLAG_CLR(u32OldMask, HCP_FLAG_ALGO_MEAS);
                VPP_FLAG_SET(u32OldMask, HCP_FLAG_ALGO_PROC);
            }
            else if (eState == VPP_IP_STATE_INITED)
            {
                VPP_FLAG_CLR(u32OldMask, HCP_FLAG_ALGO_PROC);
            }
        }
        else if (ctrl.ctrl_type == HQV_CONTROL_MEAS)
        {
            if (ctrl.meas.enable)
            {
                VPP_FLAG_CLR(u32OldMask, HCP_FLAG_ALGO_PROC);
                VPP_FLAG_SET(u32OldMask, HCP_FLAG_ALGO_MEAS);
            }
            else
            {
                VPP_FLAG_CLR(u32OldMask, HCP_FLAG_ALGO_MEAS);
            }
        }
        else if (ctrl.ctrl_type == HQV_CONTROL_GLOBAL_DEMO)
        {
            // empty - explicitly valid case
        }
    }

    *pu32NewMask = u32OldMask;

    return VPP_OK;
}

static inline void vVppIpHcp_SignalClient(t_StVppIpHcpSessionCb *pstCb)
{
    pthread_cond_signal(&pstCb->condToClient);
}

static inline void vVppIpHcp_WaitClient(t_StVppIpHcpSessionCb *pstCb)
{
    pthread_cond_wait(&pstCb->condToClient, &pstCb->mutex);
}

static void vVppIpHcp_SignalWorkerStart(t_StVppIpHcpSessionCb *pstCb)
{
    HCP_LOCK(pstCb);
    pstCb->flags.bWorkerStarted = VPP_TRUE;
    LOGI("signalling worker start");
    HCP_UNLOCK(pstCb);
    vVppIpHcp_SignalClient(pstCb);
}

static void vVppIpHcp_WaitWorkerStart(t_StVppIpHcpSessionCb *pstCb)
{
    HCP_LOCK(pstCb);

    LOGI("waiting for worker to start");

    while (!pstCb->flags.bWorkerStarted)
        vVppIpHcp_WaitClient(pstCb);

    LOGI("done waiting for worker to start");
    HCP_UNLOCK(pstCb);
}

static uint32_t u32VppIpHcp_IntBufIsOurs(t_StVppIpHcpIntBuf *pstIntBuf)
{
    return (pstIntBuf->eBufState == HCP_BUF_OWNER_IP ||
            pstIntBuf->eBufState == HCP_BUF_UNALLOCATED);
}

static uint32_t u32VppIpHcp_IntPxIsOurs(t_StVppIpHcpSessionCb *pstSessCb)
{
    return u32VppIpHcp_IntBufIsOurs(&pstSessCb->stIntBufs.stIntBufPx);
}

static uint32_t u32VppIpHcp_IntMeasIsOurs(t_StVppIpHcpSessionCb *pstSessCb)
{
    return u32VppIpHcp_IntBufIsOurs(&pstSessCb->stIntBufs.stIntBufMeas);
}


static uint32_t u32VppIpHcp_AllocIntBuf(t_StVppIpHcpSessionCb *pstSessCb,
                                        t_StVppIpHcpIntBuf *pstIntBuf,
                                        uint32_t bSecure)
{
    uint32_t u32;

    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pstIntBuf, VPP_ERR_PARAM);

    u32 = u32VppIon_Alloc(pstSessCb->stBase.pstCtx, pstIntBuf->u32Sz,
                          bSecure, &pstIntBuf->stIon);

    pstIntBuf->bSecure = bSecure;
    if (u32)
    {
        pstIntBuf->eBufState = HCP_BUF_UNALLOCATED;
        LOGE("unable to allocate ion: u32Sz=%u, bSecure=%u, u32=%u",
             pstIntBuf->u32Sz, bSecure, u32);
    }
    else
    {
        pstIntBuf->eBufState = HCP_BUF_OWNER_IP;
        LOGI("allocated ion: u32Sz=%u, bSecure=%u, u32=%u",
             pstIntBuf->u32Sz, bSecure, u32);
    }

    return u32;
}

static uint32_t u32VppIpHcp_FreeIntBuf(t_StVppIpHcpSessionCb *pstSessCb,
                                       t_StVppIpHcpIntBuf *pstIntBuf)
{
    uint32_t u32;

    LOG_ENTER();

    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pstIntBuf, VPP_ERR_PARAM);

    if (pstIntBuf->eBufState != HCP_BUF_OWNER_IP)
    {
        LOGE("trying to free buffers that we don't own, eBufState=%u",
             pstIntBuf->eBufState);
        return VPP_ERR_PARAM;
    }

    u32 = u32VppIon_Free(pstSessCb->stBase.pstCtx, &pstIntBuf->stIon);
    if (u32 != VPP_OK)
    {
        LOGE("unable to free ion: u32Sz=%u, u32=%u", pstIntBuf->u32Sz, u32);
    }

    pstIntBuf->eBufState = HCP_BUF_UNALLOCATED;

    LOG_EXIT();

    return u32;
}

static uint32_t u32VppIpHcp_AllocIntBufs(t_StVppIpHcpSessionCb *pstSessCb)
{
    uint32_t u32, u32Ret = VPP_OK;
    t_StVppIpHcpIntBufs *pstIntBufs;

    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);

    pstIntBufs = &pstSessCb->stIntBufs;

    if (pstIntBufs->bDirty)
    {
        LOGE("ERROR: allocating int. bufs, but bDirty is true!");
    }

    if (pstIntBufs->stIntBufPx.eBufState != HCP_BUF_UNALLOCATED ||
        pstIntBufs->stIntBufMeas.eBufState != HCP_BUF_UNALLOCATED)
    {
        LOGE("ERROR: allocating, but... px.eBufState=%u, meas.eBufState=%u",
             pstIntBufs->stIntBufPx.eBufState,
             pstIntBufs->stIntBufMeas.eBufState);
    }

    u32 = u32VppIpHcp_AllocIntBuf(pstSessCb, &pstSessCb->stIntBufs.stIntBufPx,
                                  pstSessCb->stBase.bSecureSession);
    if (u32 != VPP_OK)
    {
        u32Ret = VPP_ERR_NO_MEM;
        LOGE("unable to allocate int. px buf, u32=%u", u32);
        goto ERR_ALLOC_INT_PX;
    }

    if (pstSessCb->stCfg.eOpMode == HCP_OP_MODE_PROC)
    {
        u32 = u32VppIpHcp_AllocIntBuf(pstSessCb,
                                      &pstSessCb->stIntBufs.stIntBufMeas,
                                      VPP_FALSE);
        if (u32 != VPP_OK)
        {
            u32Ret = VPP_ERR_NO_MEM;
            LOGE("unable to allocate int. meas buf, u32=%u", u32);
            goto ERR_ALLOC_MEAS;
        }
    }

    return u32Ret;

ERR_ALLOC_MEAS:
    u32 = u32VppIpHcp_FreeIntBuf(pstSessCb, &pstSessCb->stIntBufs.stIntBufPx);
    LOGE_IF(u32 != VPP_OK, "FreeIntBuf (int px) failed, u32=%u", u32);

ERR_ALLOC_INT_PX:
    return u32Ret;
}

static uint32_t u32VppIpHcp_FreeIntBufs(t_StVppIpHcpSessionCb *pstSessCb)
{
    uint32_t u32, u32Ret = VPP_OK;
    t_EVppIpHcpBufState eBufState;

    LOG_ENTER();

    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);

    eBufState = pstSessCb->stIntBufs.stIntBufPx.eBufState;
    if (eBufState == HCP_BUF_OWNER_IP)
    {
        u32 = u32VppIpHcp_FreeIntBuf(pstSessCb, &pstSessCb->stIntBufs.stIntBufPx);
        if (u32)
        {
            u32Ret = VPP_ERR;
            LOGE("unable to free int. px buf, u32=%u", u32);
        }
    }
    else if (eBufState != HCP_BUF_UNALLOCATED)
    {
        u32Ret = VPP_ERR;
        LOGE("unable to free int. px buf, eBufState=%u", eBufState);
    }

    eBufState = pstSessCb->stIntBufs.stIntBufMeas.eBufState;
    if (eBufState == HCP_BUF_OWNER_IP)
    {
        u32 = u32VppIpHcp_FreeIntBuf(pstSessCb, &pstSessCb->stIntBufs.stIntBufMeas);
        if (u32)
        {
            u32Ret = VPP_ERR;
            LOGE("unable to free int. meas buf, u32=%u", u32);
        }
    }
    else if (eBufState != HCP_BUF_UNALLOCATED)
    {
        u32Ret = VPP_ERR;
        LOGE("unable to free int. meas buf, eBufState=%u", eBufState);
    }

    LOG_EXIT();
    return u32Ret;
}

static uint32_t u32VppIpHcp_CanReuseIntBuf(t_StVppIpHcpSessionCb *pstSessCb)
{
    uint32_t u32Ret = VPP_TRUE;
    t_StVppIpHcpIntBuf *pstPx, *pstMeas;

    pstPx = &pstSessCb->stIntBufs.stIntBufPx;
    pstMeas = &pstSessCb->stIntBufs.stIntBufMeas;

    if (pstPx->eBufState == HCP_BUF_UNALLOCATED ||
        pstPx->stIon.len < pstPx->u32Sz)
    {
        LOGD("unable to reuse int. px buf, eBufState=%u, reqSz=%u, allocSz=%u",
             pstPx->eBufState, pstPx->u32Sz, pstPx->stIon.len);
        u32Ret = VPP_FALSE;
    }

    if (pstSessCb->stCfg.eOpMode == HCP_OP_MODE_PROC)
    {
        if (pstMeas->eBufState == HCP_BUF_UNALLOCATED ||
            pstMeas->stIon.len < pstMeas->u32Sz)
        {
            LOGD("unable to reuse int. meas buf, eBufState=%u, reqSz=%u, allocSz=%u",
                 pstMeas->eBufState, pstMeas->u32Sz, pstMeas->stIon.len);
            u32Ret = VPP_FALSE;
        }
    }

    if (u32Ret)
        LOGI("can reuse intermediate buffers");

    return u32Ret;
}

static uint32_t u32VppIpHcp_FlushPort(t_StVppIpHcpSessionCb *pstCb,
                                      enum vpp_port ePort,
                                      t_StVppIpPort *pstPort)
{
    t_StVppBuf *pBuf;

    while (u32VppIp_PortBufGet(pstPort, &pBuf, &pstCb->mutex) == VPP_OK)
    {
        VPP_FLAG_SET(pBuf->u32InternalFlags, VPP_BUF_FLAG_FLUSHED);
        if (ePort == VPP_PORT_OUTPUT)
            pBuf->stPixel.u32FilledLen = 0;
        vVppIpCbLog(&pstCb->stBase.stCb, pBuf, eVppLogId_IpBufDone);
        u32VppIp_CbBufDone(&pstCb->stBase.stCb, ePort, pBuf);
    }

    return VPP_OK;
}

static void vVppIpHcp_GetFwCnts(t_StVppIpHcpSessionCb *pstSessCb,
                                uint32_t *pu32In, uint32_t *pu32Out)
{
    // mutex must be locked prior to calling this function
    if (pu32In)
        *pu32In = u32VppBufPool_Cnt(&pstSessCb->stFwIn.stPendingQ);
    if (pu32Out)
        *pu32Out = u32VppBufPool_Cnt(&pstSessCb->stFwOut.stPendingQ);
}

static uint32_t u32VppIpHcp_MsgQInit(t_StVppIpHcpMsgQ *pstQ)
{
    VPP_RET_IF_NULL(pstQ, VPP_ERR_PARAM);
    memset(pstQ, 0, sizeof(t_StVppIpHcpMsgQ));
    return VPP_OK;
}

static uint32_t u32VppIpHcp_MsgQTerm(t_StVppIpHcpMsgQ *pstQ)
{
    return u32VppIpHcp_MsgQInit(pstQ);
}

static uint32_t u32VppIpHcp_MsgQPut(t_StVppIpHcpMsgQ *pstQ,
                                    t_StVppIpHcpMsgQNode *pstNode,
                                    pthread_mutex_t *pstMutex,
                                    pthread_cond_t *pstCond)
{
    VPP_RET_IF_NULL(pstQ, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pstNode, VPP_ERR_PARAM);

    if (pstMutex)
        MUTEX_LOCK(pstMutex);

    pstNode->pNext = NULL;

    if (!pstQ->u32Cnt)
    {
        pstQ->pstHead = pstNode;
        pstQ->pstTail = pstNode;
    }
    else
    {
        pstQ->pstTail->pNext = pstNode;
        pstQ->pstTail = pstNode;
    }

    pstQ->u32Cnt += 1;

    if (pstCond)
        pthread_cond_signal(pstCond);

    if (pstMutex)
        MUTEX_UNLOCK(pstMutex);

    return VPP_OK;
}

static t_StVppIpHcpMsgQNode *pstVppIpHcp_MsgQGet(t_StVppIpHcpMsgQ *pstQ,
                                                 pthread_mutex_t *pstMutex)
{
    t_StVppIpHcpMsgQNode *pstTmp = NULL;

    if (!pstQ || !pstQ->pstHead)
        return NULL;

    if (pstMutex)
        MUTEX_LOCK(pstMutex);

    pstTmp = pstQ->pstHead;
    pstQ->pstHead = pstQ->pstHead->pNext;
    pstQ->u32Cnt -= 1;

    if (!pstQ->u32Cnt)
        pstQ->pstTail = NULL;

    if (pstMutex)
        MUTEX_UNLOCK(pstMutex);

    return pstTmp;
}

static uint32_t u32VppIpHcp_MsgQCnt(t_StVppIpHcpMsgQ *pstQ,
                                    pthread_mutex_t *pstMutex)
{
    uint32_t u32Cnt = 0;

    VPP_RET_IF_NULL(pstQ, 0);

    if (pstMutex)
        MUTEX_LOCK(pstMutex);

    u32Cnt = pstQ->u32Cnt;

    if (pstMutex)
        MUTEX_UNLOCK(pstMutex);

    return u32Cnt;
}

static uint32_t u32VppIpHcp_RegisterSession(t_StVppIpHcpGlobalCb *pstGlobal,
                                            t_StVppIpHcpSessionCb *pstSess)
{
    uint32_t u32Ret = VPP_OK;
    uint32_t i, u32Free = HCP_MAX_SESSIONS;

    VPP_RET_IF_NULL(pstGlobal, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pstSess, VPP_ERR_PARAM);

    HCP_LOCK(pstGlobal);

    for (i = 0; i < HCP_MAX_SESSIONS; i++)
    {
        if (u32Free == HCP_MAX_SESSIONS &&
            pstGlobal->sessions.apstSessions[i] == NULL)
        {
            u32Free = i;
            continue;
        }
        else if (pstGlobal->sessions.apstSessions[i] == pstSess)
        {
            u32Ret = VPP_ERR_RESOURCES;
            LOGE("session has already been registered at i=%u", i);
            break;
        }
    }

    if (u32Ret == VPP_OK && u32Free < HCP_MAX_SESSIONS)
    {
        pstGlobal->sessions.apstSessions[u32Free] = pstSess;
        pstGlobal->sessions.u32Cnt += 1;
        pstSess->u32SessId = (u32Free + 1) << 2;
        pstSess->pstGlobal = pstGlobal;
        LOGI("RegisterSession, at i=%u, pstSess=%p, u32Cnt=%u, u32SessId=%u",
             u32Free, pstSess, pstGlobal->sessions.u32Cnt, pstSess->u32SessId);
    }
    else
    {
        u32Ret = VPP_ERR_RESOURCES;
        LOGE("RegisterSession, max sessions reached");
    }

    HCP_UNLOCK(pstGlobal);

    return u32Ret;
}

static uint32_t u32VppIpHcp_UnregisterSession(t_StVppIpHcpGlobalCb *pstGlobal,
                                              t_StVppIpHcpSessionCb *pstSess)
{
    uint32_t i, j;
    uint32_t u32Ret = VPP_OK;

    VPP_RET_IF_NULL(pstGlobal, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pstSess, VPP_ERR_PARAM);

    HCP_LOCK(pstGlobal);

    i = (pstSess->u32SessId >> 2) - 1;
    if (pstSess == pstGlobal->sessions.apstSessions[i])
    {
        LOGI("found requested session in at i=%u, pstSess=%p", i, pstSess);
        pstGlobal->sessions.apstSessions[i] = NULL;
        pstGlobal->sessions.u32Cnt -= 1;
    }
    else
    {
        LOGE("specified session id does not match registered id, "
             "requested=%p, i=%u, found@i=%p", pstSess, i,
             pstGlobal->sessions.apstSessions[i]);
        u32Ret = VPP_ERR_STATE;
        for (j = 0; j < HCP_MAX_SESSIONS; j++)
        {
            if (pstGlobal->sessions.apstSessions[j] == pstSess)
            {
                LOGE("found requested session (%p) at index=%u, removing",
                     pstSess, j);
                pstGlobal->sessions.apstSessions[j] = NULL;
                pstGlobal->sessions.u32Cnt -= 1;
            }
        }
    }

    HCP_UNLOCK(pstGlobal);

    return u32Ret;
}

static t_StVppIpHcpSessionCb *pstVppIpHcp_FindSession(t_StVppIpHcpGlobalCb *pstGlobal,
                                                      int32_t s32SessId)
{
    uint32_t i;
    t_StVppIpHcpSessionCb *pstSess;

    VPP_RET_IF_NULL(pstGlobal, NULL);

    HCP_LOCK(pstGlobal);

    for (i = 0; i < HCP_MAX_SESSIONS; i++)
    {
        pstSess = pstGlobal->sessions.apstSessions[i];

        if (!pstSess)
            continue;

        if (HCP_SESS_GET_HW_ID(pstSess) == s32SessId)
        {
            HCP_UNLOCK(pstGlobal);
            return pstSess;
        }
    }

    HCP_UNLOCK(pstGlobal);

    LOGE("FindSession: unable to find session with hw id=%d", s32SessId);
    return NULL;
}

static uint32_t u32VppIpHcp_HandleSystemMessage(t_StVppIpHcpGlobalCb *pstGlobal,
                                                t_StHcpHfiMsgPkt *pstMsg,
                                                uint32_t *bNopReceived)
{
    uint32_t u32PropId;
    uint32_t u32 = VPP_OK;

    VPP_UNUSED(pstGlobal);
    VPP_RET_IF_NULL(bNopReceived, VPP_ERR_PARAM);

    *bNopReceived = VPP_FALSE;

    if (pstMsg->stHdr.eMsgId == MSG_ID_SET_PROPERTY_DONE)
    {
        u32PropId = pstMsg->stMsgSetProp.propId;
        LOGI("HandleSystemMessage, prop=%u (%s)",
             u32PropId, pcGetPropIdStr(u32PropId));

        if (u32PropId == PROP_ID_SYS_DIAG)
        {
        }
        else
        {
            u32 = VPP_ERR_PARAM;
            LOGE("unknown system property, prop=%u (%s)",
                 u32PropId, pcGetPropIdStr(u32PropId));
        }
    }
    else if (pstMsg->stHdr.eMsgId == MSG_ID_BUFF_RELEASED)
    {
        if ((pstMsg->stMsgBuf.bufAttr.eBufType == BUFF_TYPE_GENERIC) &&
            (pstMsg->stMsgBuf.bufAttr.u32Cookie_h == HCP_SYS_TUNE_COOKIE_H) &&
            (pstMsg->stMsgBuf.bufAttr.u32Cookie_l == HCP_SYS_TUNE_COOKIE_L) &&
            (pstMsg->stMsgBuf.bufAttr.stGenBufAttr.eUsage == BUFF_USAGE_TUNE))
        {
            u32 = u32VppIpHcp_TuningProcBuffReleasedMsg(pstGlobal, pstMsg);
            LOGE_IF(u32 != VPP_OK, "unable to proc tuning msg, u32=%u", u32);
        }
        else
        {
            u32 = VPP_ERR_PARAM;
            LOGE("unknown system buffer. type=%u, cookie_h=0x%x, cookie_l=0x%x",
                 pstMsg->stMsgBuf.bufAttr.eBufType,
                 pstMsg->stMsgBuf.bufAttr.u32Cookie_h,
                 pstMsg->stMsgBuf.bufAttr.u32Cookie_l);
        }
    }
    else if (pstMsg->stHdr.eMsgId == MSG_ID_NOOP)
    {
        *bNopReceived = VPP_TRUE;
    }

    return u32;
}

static uint32_t u32VppIpHcp_MsgIsForSession(t_StHcpHfiMsgPktHdr *pstHdr)
{
    VPP_RET_IF_NULL(pstHdr, VPP_FALSE);

    if (pstHdr->s32SessId == SESSION_ID_INVALID ||
        pstHdr->s32SessId == (int32_t) SESSION_ID_ALL)
    {
        return VPP_FALSE;
    }

    return VPP_TRUE;
}

static uint32_t u32VppIpHcp_ShouldFetchCmd(t_StVppIpHcpSessionCb *pstSessCb)
{
    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);

    if (pstSessCb->eExecutingCmd == VPP_IP_CMD_NULL &&
        pstSessCb->eCmdState == HCP_CMD_STATE_NULL &&
        u32VppIp_NtsCmdCnt(&pstSessCb->stCmdQ, NULL))
        return VPP_TRUE;

    return VPP_FALSE;
}

static uint32_t bVppIpHcp_FwWouldBypass(t_StVppIpHcpSessionCb *pstSessCb)
{
    const t_StVppIpHcpDrvCtx *pstDrv;

    VPP_RET_IF_NULL(pstSessCb, VPP_TRUE);

    pstDrv = &pstSessCb->stDrvCtx;

    if (pstSessCb->stCfg.eOpMode == HCP_OP_MODE_PROC &&
        (pstDrv->uSplitScreen.st.u32ProcessedPercent == 0 ||
         (pstDrv->u32EarMode == HCP_EAR_MODE_OFF &&
          pstDrv->u32DeGain == HCP_DE_GAIN_OFF &&
          pstDrv->u32SatGain == HCP_SAT_GAIN_OFF &&
          pstDrv->u32SatOff == HCP_SAT_OFFSET_OFF &&
          pstDrv->u32AceStrCon == HCP_ACE_STR_OFF &&
          pstDrv->u32AceStrBriL == HCP_ACE_BRI_L_OFF &&
          pstDrv->u32AceStrBriH == HCP_ACE_BRI_H_OFF)))
        return VPP_TRUE;

    return VPP_FALSE;
}

static uint32_t bVppIpHcp_CanProcess(t_StVppIpHcpSessionCb *pstSessCb,
                                     t_UVppIpHcpCanProcRes *pstRes)
{
    uint32_t u32InSz, u32OutSz;
    uint32_t u32FwIn, u32FwOut;
    uint32_t u32Required;
    t_EVppIpHcpOpMode eOpMode;
    t_StVppBuf *pstBuf;

    VPP_RET_IF_NULL(pstSessCb, VPP_FALSE);
    VPP_RET_IF_NULL(pstRes, VPP_FALSE);

    pstRes->u32Res = 0;

    u32InSz = u32VppBufPool_Cnt(&pstSessCb->stInput.stPendingQ);
    u32OutSz = u32VppBufPool_Cnt(&pstSessCb->stOutput.stPendingQ);
    eOpMode = pstSessCb->stCfg.eOpMode;

    vVppIpHcp_GetFwCnts(pstSessCb, &u32FwIn, &u32FwOut);

    pstBuf = pstVppIp_PortBufPeek(&pstSessCb->stInput, 0, NULL);
    if (pstBuf)
    {
        if (bVppIpHcp_FwWouldBypass(pstSessCb))
            pstRes->bFwWouldByp = VPP_TRUE;

        if (VPP_FLAG_IS_SET(pstBuf->u32InternalFlags, VPP_BUF_FLAG_BYPASS))
            pstRes->bBypFlag = VPP_TRUE;

        if (!pstBuf->stPixel.u32FilledLen)
            pstRes->bInputFlZero = VPP_TRUE;

        if (pstBuf->eBufType != eVppBufType_Progressive)
            pstRes->bInputInvFldType = VPP_TRUE;

        u32Required = u32VppUtils_GetPxBufferSize(&pstSessCb->stInput.stParam);
        if (pstBuf->stPixel.u32ValidLen < u32Required)
        {
            LOGE("CanProc: in: u32Valid=%u, u32Required=%u",
                 pstBuf->stPixel.u32ValidLen, u32Required);
            pstRes->bInputInvSz = VPP_TRUE;
        }
    }

    pstBuf = pstVppIp_PortBufPeek(&pstSessCb->stOutput, 0, NULL);
    if (pstBuf)
    {
        if (eOpMode == HCP_OP_MODE_PROC)
            u32Required = u32VppUtils_GetPxBufferSize(&pstSessCb->stOutput.stParam);
        else
            u32Required = pstSessCb->stBufReqOut.u32PxSz;

        if (pstBuf->stPixel.u32ValidLen < u32Required)
        {
            LOGE("CanProc: out: u32Valid=%u, u32Required=%u",
                 pstBuf->stPixel.u32ValidLen, u32Required);
            pstRes->bOutputInvSz = VPP_TRUE;
        }
    }

    pstRes->bOpModeByp = eOpMode == HCP_OP_MODE_BYPASS && u32InSz;

    pstRes->bBufReqMet = (u32InSz >= pstSessCb->stBufReqIn.u32MinCnt &&
                          u32OutSz >= pstSessCb->stBufReqOut.u32MinCnt);

    pstRes->bWakeNormal = ((eOpMode == HCP_OP_MODE_PROC ||
                            eOpMode == HCP_OP_MODE_MEAS) &&
                           pstRes->bBufReqMet);

    // Results
    pstRes->bCanProcess = pstRes->bWakeNormal;

    pstRes->bCanBypassIn = (pstRes->bWakeBypass || pstRes->bOpModeByp ||
                            pstRes->bBypFlag || pstRes->bInputFlZero ||
                            pstRes->bInputInvSz || pstRes->bInputInvFldType ||
                            pstRes->bFwWouldByp);

    pstRes->bCanBypassOut = pstRes->bOutputInvSz;

    LOGD("CanProc: eOpMode=%u, u32InSz=%u, u32OutSz=%u, u32InMin=%u, "
         "u32OutMin=%u, bCanProcess=%u, bCanBypassIn=%u, bCanBypassOut=%u, "
         "u32FwIn=%u, u32FwOut=%u, flags=0x%x",
         eOpMode,  u32InSz, u32OutSz, pstSessCb->stBufReqIn.u32MinCnt,
         pstSessCb->stBufReqOut.u32MinCnt, pstRes->bCanProcess,
         pstRes->bCanBypassIn, pstRes->bCanBypassOut, u32FwIn, u32FwOut,
         pstRes->u32Res);

    return pstSessCb->eExecutingCmd == VPP_IP_CMD_INTERNAL_DRAIN ? VPP_FALSE :
        pstRes->bCanProcess || pstRes->bCanBypassIn || pstRes->bCanBypassOut;
}

static uint32_t u32VppIpHcp_WorkerShouldSleep(t_StVppIpHcpSessionCb *pstSessCb)
{
    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);

    // session mutex is locked when this function is called.

    uint32_t u32CmdQSz, u32MsgQSz, u32InSz, u32OutSz;
    uint32_t u32FwInCnt, u32FwOutCnt;
    t_EVppIpHcpOpMode eOpMode;

    u32CmdQSz = u32VppIp_NtsCmdCnt(&pstSessCb->stCmdQ, NULL);
    u32MsgQSz = u32VppIpHcp_MsgQCnt(&pstSessCb->stMsgQ, NULL);

    u32InSz = u32VppBufPool_Cnt(&pstSessCb->stInput.stPendingQ);
    u32OutSz = u32VppBufPool_Cnt(&pstSessCb->stOutput.stPendingQ);

    vVppIpHcp_GetFwCnts(pstSessCb, &u32FwInCnt, &u32FwOutCnt);

    eOpMode = pstSessCb->stCfg.eOpMode;

    if (u32MsgQSz)
    {
        LOGI("wake because u32MsgQSz=%u", u32MsgQSz);
        return VPP_FALSE;
    }

    if (u32VppIpHcp_ShouldFetchCmd(pstSessCb))
    {
        LOGI("wake because ShouldFetchCmd, u32CmdQSz=%u", u32CmdQSz);
        return VPP_FALSE;
    }

    // check if buffers can be processed
    if (VPP_IP_STATE_EQUAL(pstSessCb, VPP_IP_STATE_ACTIVE))
    {
        if (pstSessCb->eExecutingCmd == VPP_IP_CMD_INTERNAL_DRAIN &&
            (u32FwInCnt || u32FwOutCnt))
        {
            // internal drain is triggered by ProcessBuffer() when it needs to
            // bypass buffers and needs to synchronize with firmware
            LOGI("sleeping because int_drain pending, u32FwIn=%u, u32FwOut=%u",
                 u32FwInCnt, u32FwOutCnt);
            return VPP_TRUE;
        }

        if (pstSessCb->eExecutingCmd == VPP_IP_CMD_FLUSH &&
            pstSessCb->eCmdState == HCP_CMD_STATE_FLUSH_WAIT_FW &&
            (u32FwInCnt || u32FwOutCnt))
        {
            // Flush is requested but we are waiting for firmware to return all
            // of its buffers before we flush out the queues
            LOGI("sleeping because flush pending, u32FwIn=%u, u32FwOut=%u",
                 u32FwInCnt, u32FwOutCnt);
            return VPP_TRUE;
        }

        t_UVppIpHcpCanProcRes stProcRes;
        if (bVppIpHcp_CanProcess(pstSessCb, &stProcRes))
        {
            LOGI("wake because eOpMode=%u, u32InSz=%u, u32OutSz=%u, "
                 "bCanProcess=%u, bCanBypassIn=%u, bCanBypassOut=%u",
                 eOpMode, u32InSz, u32OutSz, stProcRes.bCanProcess,
                 stProcRes.bCanBypassIn, stProcRes.bCanBypassOut);
            return VPP_FALSE;
        }
    }

    LOGI("sleeping, eIpState=%u, u32CmdQSz=%u, u32MsgQSz=%u, eExecutingCmd=%u, "
         "u32InSz=%u, u32OutSz=%u", pstSessCb->eState, u32CmdQSz, u32MsgQSz,
         pstSessCb->eExecutingCmd, u32InSz, u32OutSz);

    return VPP_TRUE;
}

static t_StVppIpHcpMsgQNode *pstVppIpHcp_GetHcpMessage(t_StVppIpHcpSessionCb *pstSessCb)
{
    VPP_RET_IF_NULL(pstSessCb, NULL);
    return pstVppIpHcp_MsgQGet(&pstSessCb->stMsgQ, &pstSessCb->mutex);
}

static void vVppIpHcp_SetCmdState(t_StVppIpHcpSessionCb *pstSessCb,
                                  t_EVppIpHcpCmdState eNewState)
{
    VPP_RET_VOID_IF_NULL(pstSessCb);

    t_EVppIpHcpCmdState ePrevState = pstSessCb->eCmdState;
    pstSessCb->eCmdState = eNewState;

    LOGD("cmd state changed from (%s) to (%s) for session %p",
         pcGetCmdStateStr(ePrevState), pcGetCmdStateStr(eNewState), pstSessCb);
}

static void vVppIpHcp_SetExecutingCmd(t_StVppIpHcpSessionCb *pstSessCb,
                                      t_EVppIpCmd eCmd)
{
    t_EVppIpCmd ePrevCmd;

    VPP_RET_VOID_IF_NULL(pstSessCb);

    ePrevCmd = pstSessCb->eExecutingCmd;
    pstSessCb->eExecutingCmd = eCmd;

    LOGD("Executing command changed from (%u) to (%u) for session %p",
         ePrevCmd, eCmd, pstSessCb);
}

static uint32_t bVppIpHcp_ValidateHwSession(t_StVppIpHcpSessionCb *pstSessCb)
{
    return HCP_SESS_GET_HW_ID(pstSessCb) >= 0;
}

static uint32_t u32VppIpHcp_ProcCmdInit(t_StVppIpHcpSessionCb *pstSessCb)
{
    t_StVppIpCmd stCmd;
    uint32_t u32, u32Ret = VPP_OK;
    uint32_t bSignalWorkerStart = VPP_FALSE;

    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);

    u32 = u32VppIpHcp_HwOpen(pstSessCb);
    if (u32 != VPP_OK)
    {
        LOGE("HwOpen failed, u32=%u", u32);

        stCmd.eCmd = VPP_IP_CMD_THREAD_EXIT;
        u32 = u32VppIp_NtsCmdPut(&pstSessCb->stCmdQ, stCmd,
                                 &pstSessCb->mutex, NULL);
        LOGE_IF(u32 != VPP_OK, "unable to insert thread term, u32=%u", u32);

        u32Ret = VPP_ERR_HW;
        bSignalWorkerStart = VPP_TRUE;
    }
    else
    {
        u32 = u32VppIpHcp_HwSessSetPropDiagLevel(pstSessCb,
                                                 pstSessCb->stDbgCfg.u32LogFlags);
        LOGE_IF(u32, "failed to set diag log levels, u32=%u", u32);

        u32 = u32VppIpHcp_HwSessSetPropCp(pstSessCb,
                                          pstSessCb->stBase.bSecureSession);
        LOGE_IF(u32 != VPP_OK, "ERROR: HwSessSetPropCp, u32=%u", u32);

        if (u32 == VPP_OK)
        {
            u32 = u32VppIpHcp_HwSessSetPropOpMode(pstSessCb, HCP_OP_MODE_PROC);
            LOGE_IF(u32 != VPP_OK, "ERROR, HwSessSetPropOpMode, u32=%u", u32);
        }

        if (u32 == VPP_OK)
        {
            u32 = u32VppIpHcp_HwSessGetPropBufReq(pstSessCb);
            LOGE_IF(u32 != VPP_OK, "ERROR: HwSessGetProp, u32=%u", u32);
        }

        if (u32 != VPP_OK)
        {
            u32Ret = VPP_ERR_HW;
            bSignalWorkerStart = VPP_TRUE;
        }
        else
        {
            HCP_LOCK(pstSessCb);
            vVppIpHcp_SetExecutingCmd(pstSessCb, VPP_IP_CMD_INIT);
            HCP_UNLOCK(pstSessCb);
        }
    }

    if (bSignalWorkerStart)
    {
        pstSessCb->u32WorkerErr = u32Ret;
        vVppIpHcp_SignalWorkerStart(pstSessCb);
    }

    return u32Ret;
}

static uint32_t u32VppIpHcp_ProcCmdTerm(t_StVppIpHcpSessionCb *pstSessCb)
{
    uint32_t u32, u32Ret = VPP_OK;

    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);

    u32 = u32VppIpHcp_HwClose(pstSessCb);
    if (u32 != VPP_OK)
    {
        u32Ret = VPP_ERR_HW;
        LOGE("HwClose failed, u32=%u", u32);
    }
    else
    {
        HCP_LOCK(pstSessCb);
        vVppIpHcp_SetExecutingCmd(pstSessCb, VPP_IP_CMD_TERM);
        HCP_UNLOCK(pstSessCb);
    }

    return u32Ret;
}

static uint32_t u32VppIpHcp_ProcCmdOpen(t_StVppIpHcpSessionCb *pstSessCb)
{
    uint32_t u32 = VPP_OK, u32Ret = VPP_OK;
    t_EVppIpHcpHwState eHwState;

    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);

    if (pstSessCb->eExecutingCmd != VPP_IP_CMD_NULL &&
        pstSessCb->eExecutingCmd != VPP_IP_CMD_OPEN)
    {
        return VPP_ERR_STATE;
    }

    if (pstSessCb->eCmdState == HCP_CMD_STATE_NULL)
    {
        vVppIpHcp_Compute(pstSessCb, VPP_FALSE);

        if (u32VppIpHcp_ValidateConfig(pstSessCb) != VPP_OK)
        {
            vVppIpHcp_SetCmdState(pstSessCb, HCP_CMD_STATE_OPEN_SIGNAL_CLIENT);
            u32Ret = VPP_ERR_PARAM;
        }

        if (pstSessCb->stDrvCtx.eHwState == HCP_HW_STATE_NULL)
        {
            // This case only happens if there is an error during
            // ProcCmdOpen() and Open() is called again by the client.
            u32 = u32VppIpHcp_HwOpen(pstSessCb);
            if (u32 != VPP_OK)
            {
                u32Ret = VPP_ERR_HW;
                LOGE("HwOpen failed, u32=%u", u32);
                vVppIpHcp_SetCmdState(pstSessCb, HCP_CMD_STATE_OPEN_SIGNAL_CLIENT);
            }
        }
    }

    if (pstSessCb->eCmdState == HCP_CMD_STATE_NULL)
    {
        vVppIpHcp_SetExecutingCmd(pstSessCb, VPP_IP_CMD_OPEN);
        VPP_IP_STATE_SET(pstSessCb, VPP_IP_STATE_OPENING);

        if (pstSessCb->stCfg.eOpMode == HCP_OP_MODE_BYPASS)
        {
            LOGD("opening hcp module in bypass");
            vVppIpHcp_SetCmdState(pstSessCb, HCP_CMD_STATE_OPEN_SIGNAL_CLIENT);
        }
        else if (pstSessCb->stIntBufs.bDirty)
        {
            u32Ret = VPP_ERR_STATE;
            vVppIpHcp_SetCmdState(pstSessCb, HCP_CMD_STATE_OPEN_SIGNAL_CLIENT);
            LOGE("cannot process open with dirty buffer requirements");
        }
        else
        {
            eHwState = pstSessCb->stDrvCtx.eHwState;
            if (eHwState == HCP_HW_STATE_ALLOC)
            {
                vVppIpHcp_SetCmdState(pstSessCb, HCP_CMD_STATE_OPEN_REALLOC_BUFS);
            }
            else
            {
                if (eHwState != HCP_HW_STATE_RDY)
                {
                    u32Ret = VPP_ERR_STATE;
                    LOGE("can not process open with invalid hw state=%u (%s)",
                         eHwState, pcGetHwStateStr(eHwState));
                }
                vVppIpHcp_SetCmdState(pstSessCb, HCP_CMD_STATE_OPEN_SIGNAL_CLIENT);
            }
        }
    }

    if (pstSessCb->eCmdState == HCP_CMD_STATE_OPEN_REALLOC_BUFS)
    {
        uint32_t our_px, our_meas;
        our_px = u32VppIpHcp_IntPxIsOurs(pstSessCb);
        our_meas = u32VppIpHcp_IntMeasIsOurs(pstSessCb);
        if (!our_px || !our_meas)
        {
            u32Ret = VPP_ERR_STATE;
            LOGE("ERROR: intermediate buffers held by firmware, "
                 "our_px=%u, our_meas=%u", our_px, our_meas);
            vVppIpHcp_SetCmdState(pstSessCb, HCP_CMD_STATE_OPEN_SIGNAL_CLIENT);
        }
    }

    if (pstSessCb->eCmdState == HCP_CMD_STATE_OPEN_REALLOC_BUFS)
    {
        if (u32VppIpHcp_CanReuseIntBuf(pstSessCb))
        {
            vVppIpHcp_SetCmdState(pstSessCb, HCP_CMD_STATE_OPEN_SEND_TO_FW);
        }
        else
        {
            u32 = u32VppIpHcp_FreeIntBufs(pstSessCb);
            if (u32 != VPP_OK)
            {
                u32Ret = VPP_ERR_NO_MEM;
                vVppIpHcp_SetCmdState(pstSessCb, HCP_CMD_STATE_OPEN_SIGNAL_CLIENT);
                LOGE("unable to free intermediate buffers, u32=%u", u32);
            }
            else
                vVppIpHcp_SetCmdState(pstSessCb, HCP_CMD_STATE_OPEN_ALLOC_BUFS);
        }
    }

    if (pstSessCb->eCmdState == HCP_CMD_STATE_OPEN_ALLOC_BUFS)
    {
        u32 = u32VppIpHcp_AllocIntBufs(pstSessCb);
        if (u32 != VPP_OK)
        {
            u32Ret = VPP_ERR_NO_MEM;
            vVppIpHcp_SetCmdState(pstSessCb, HCP_CMD_STATE_OPEN_SIGNAL_CLIENT);
            LOGE("unable to allocate intermediate buffers, u32=%u", u32);
        }
        else
            vVppIpHcp_SetCmdState(pstSessCb, HCP_CMD_STATE_OPEN_SEND_TO_FW);
    }

    if (pstSessCb->eCmdState == HCP_CMD_STATE_OPEN_SEND_TO_FW)
    {
        // send the buffers to firmware
        u32 = u32VppIpHcp_HwSessSendSessBuf(pstSessCb);
        LOGE_IF(u32 != VPP_OK, "unable to send int. buffers to fw, u32=%u", u32);

        // send video hints
        if (u32 == VPP_OK)
        {
            u32 = u32VppIpHcp_HwSessSetPropResHints(pstSessCb);
            LOGE_IF(u32 != VPP_OK, "unable to send res. hints, u32=%u", u32);
        }

        // send knobs
        if (u32 == VPP_OK)
        {
            u32 = u32VppIpHcp_HwSessSetPropKnobs(pstSessCb);
            LOGE_IF(u32 != VPP_OK, "unable to set knobs to fw, u32=%u", u32);
        }

        // handle errors if necessary
        if (u32 == VPP_OK)
        {
            vVppIpHcp_SetCmdState(pstSessCb, HCP_CMD_STATE_OPEN_WAIT_RSP_FW);
        }
        else
        {
            u32 = u32VppIpHcp_HwClose(pstSessCb);
            if (u32 == VPP_OK)
            {
                vVppIpHcp_SetCmdState(pstSessCb, HCP_CMD_STATE_OPEN_WAIT_CLOSE_DONE);
            }
            else
            {
                u32Ret = VPP_ERR_HW;
                LOGE("unable to send close to firmware, u32=%u", u32);
                vVppIpHcp_SetCmdState(pstSessCb, HCP_CMD_STATE_OPEN_SIGNAL_CLIENT);
            }
        }
    }

    if (pstSessCb->eCmdState == HCP_CMD_STATE_OPEN_WAIT_CLOSE_DONE)
    {
        if (HCP_SESS_GET_HW_ID(pstSessCb) == SESSION_ID_INVALID &&
            pstSessCb->stDrvCtx.eHwState == HCP_HW_STATE_NULL)
        {
            u32Ret = VPP_ERR_HW;
            vVppIpHcp_SetCmdState(pstSessCb, HCP_CMD_STATE_OPEN_SIGNAL_CLIENT);
        }
    }

    if (pstSessCb->eCmdState == HCP_CMD_STATE_OPEN_WAIT_RSP_FW)
    {
        uint32_t u32TxKnob, u32RxKnob, u32TxResHint, u32RxResHint;

        u32TxKnob = HCP_STAT_GET(pstSessCb, FwTxPropSetKnob);
        u32RxKnob = HCP_STAT_GET(pstSessCb, FwRxPropSetKnob);
        u32TxResHint = HCP_STAT_GET(pstSessCb, FwTxPropResHints);
        u32RxResHint = HCP_STAT_GET(pstSessCb, FwRxPropResHints);

        LOGD("u32TxKnob=%u, u32RxKnob=%u, u32TxResHint=%u, u32RxResHint=%u",
             u32TxKnob, u32RxKnob, u32TxResHint, u32RxResHint);

        if (u32TxKnob == u32RxKnob && u32TxResHint == u32RxResHint)
        {
            vVppIpHcp_HwSetState(pstSessCb, HCP_HW_STATE_RDY);
            vVppIpHcp_SetCmdState(pstSessCb, HCP_CMD_STATE_OPEN_SIGNAL_CLIENT);
        }
    }

    if (pstSessCb->eCmdState == HCP_CMD_STATE_OPEN_SIGNAL_CLIENT)
    {
        if (u32Ret != VPP_OK)
        {
            u32 = u32VppIpHcp_FreeIntBufs(pstSessCb);
            LOGE_IF(u32 != VPP_OK, "unable to free intermediate buffers");

            pstSessCb->flags.bOpened = VPP_FALSE;
            VPP_IP_STATE_SET(pstSessCb, VPP_IP_STATE_INITED);
        }
        else
        {
            pstSessCb->flags.bOpened = VPP_TRUE;
            VPP_IP_STATE_SET(pstSessCb, VPP_IP_STATE_ACTIVE);
        }

        HCP_LOCK(pstSessCb);
        pstSessCb->flags.bOpenSignal = VPP_TRUE;
        pstSessCb->async_res.u32OpenRet = u32Ret;
        HCP_UNLOCK(pstSessCb);

        vVppIpHcp_SetExecutingCmd(pstSessCb, VPP_IP_CMD_NULL);
        vVppIpHcp_SetCmdState(pstSessCb, HCP_CMD_STATE_NULL);
        vVppIpHcp_SignalClient(pstSessCb);
    }

    return u32Ret;
}

static uint32_t u32VppIpHcp_ProcCmdClose(t_StVppIpHcpSessionCb *pstSessCb)
{
    uint32_t u32In, u32Out;

    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);

    if (pstSessCb->eExecutingCmd != VPP_IP_CMD_NULL &&
        pstSessCb->eExecutingCmd != VPP_IP_CMD_CLOSE)
    {
        return VPP_ERR_STATE;
    }

    HCP_LOCK(pstSessCb);

    u32In = pstSessCb->stDrvCtx.u32InBufCnt;
    u32Out = pstSessCb->stDrvCtx.u32OutBufCnt;

    if (u32In || u32Out)
    {
        if (pstSessCb->eExecutingCmd != VPP_IP_CMD_CLOSE)
        {
            // only set these once
            vVppIpHcp_SetExecutingCmd(pstSessCb, VPP_IP_CMD_CLOSE);
            vVppIpHcp_SetCmdState(pstSessCb, HCP_CMD_STATE_INTERNAL_DRAIN);
        }
        LOGD("close pending, u32In=%u, u32Out=%u", u32In, u32Out);
    }
    else
    {
        if (pstSessCb->eExecutingCmd == VPP_IP_CMD_CLOSE)
        {
            vVppIpHcp_SetExecutingCmd(pstSessCb, VPP_IP_CMD_NULL);
            vVppIpHcp_SetCmdState(pstSessCb, HCP_CMD_STATE_NULL);
        }

        VPP_IP_STATE_SET(pstSessCb, VPP_IP_STATE_INITED);

        pstSessCb->flags.bOpened = VPP_FALSE;
        pstSessCb->async_res.u32CloseRet = VPP_OK;
        vVppIpHcp_SignalClient(pstSessCb);
    }

    HCP_UNLOCK(pstSessCb);

    return VPP_OK;
}

static uint32_t u32VppIpHcp_ProcCmdFlush(t_StVppIpHcpSessionCb *pstSessCb,
                                         t_StVppIpCmd *pstCmd)
{
    uint32_t u32, u32Ret = VPP_OK;
    t_StVppEvt stEvt;
    t_StVppIpPort *pstPort;
    uint32_t u32FwInCnt, u32FwOutCnt;

    HCP_LOCK(pstSessCb);
    if (pstSessCb->eExecutingCmd != VPP_IP_CMD_NULL &&
        pstSessCb->eExecutingCmd != VPP_IP_CMD_FLUSH)
    {

        HCP_UNLOCK(pstSessCb);
        return VPP_ERR_STATE;
    }

    if (pstSessCb->eExecutingCmd == VPP_IP_CMD_NULL)
    {
        if (!pstCmd)
        {
            HCP_UNLOCK(pstSessCb);
            return VPP_ERR_PARAM;
        }
        pstSessCb->eFlushingPort = pstCmd->flush.ePort;

        vVppIpHcp_SetExecutingCmd(pstSessCb, VPP_IP_CMD_FLUSH);
        vVppIpHcp_SetCmdState(pstSessCb, HCP_CMD_STATE_FLUSH_WAIT_FW);
    }

    vVppIpHcp_GetFwCnts(pstSessCb, &u32FwInCnt, &u32FwOutCnt);

    if (pstSessCb->eCmdState == HCP_CMD_STATE_FLUSH_WAIT_FW)
    {
        if ((pstSessCb->eFlushingPort == VPP_PORT_INPUT && !u32FwInCnt) ||
            (pstSessCb->eFlushingPort == VPP_PORT_OUTPUT && !u32FwOutCnt) )
        {
            vVppIpHcp_SetCmdState(pstSessCb, HCP_CMD_STATE_FLUSH_INTERNAL);
        }
        LOGD("flushing, port=%u, u32FwInCnt=%u, u32FwOutCnt=%u",
             pstSessCb->eFlushingPort, u32FwInCnt, u32FwOutCnt);
    }

    HCP_UNLOCK(pstSessCb);

    if (pstSessCb->eCmdState == HCP_CMD_STATE_FLUSH_INTERNAL)
    {
        pstPort = pstSessCb->eFlushingPort == VPP_PORT_INPUT ?
            &pstSessCb->stInput : &pstSessCb->stOutput;

        u32 = u32VppIpHcp_FlushPort(pstSessCb, pstSessCb->eFlushingPort, pstPort);
        LOGE_IF(u32 != VPP_OK, "FlushPort failed, u32=%u, port=%u",
                u32, pstSessCb->eFlushingPort);

        vVppIpHcp_SetCmdState(pstSessCb, HCP_CMD_STATE_FLUSH_DONE);
    }

    if (pstSessCb->eCmdState == HCP_CMD_STATE_FLUSH_DONE)
    {
        stEvt.eType = VPP_EVT_FLUSH_DONE;
        stEvt.flush.ePort = pstSessCb->eFlushingPort;

        HCP_LOCK(pstSessCb);
        pstSessCb->eFlushingPort = VPP_PORT_MAX;
        vVppIpHcp_SetExecutingCmd(pstSessCb, VPP_IP_CMD_NULL);
        vVppIpHcp_SetCmdState(pstSessCb, HCP_CMD_STATE_NULL);
        HCP_UNLOCK(pstSessCb);

        u32VppIpCbEvent(&pstSessCb->stBase.stCb, stEvt);
    }

    return u32Ret;
}

static uint32_t u32VppIpHcp_ProcCmdDrain(t_StVppIpHcpSessionCb *pstSessCb)
{
    uint32_t u32Ret = VPP_OK;
    uint32_t u32FwIn, u32FwOut, u32IntCnt;
    t_StVppEvt stEvt;

    if (pstSessCb->eExecutingCmd != VPP_IP_CMD_NULL &&
        pstSessCb->eExecutingCmd != VPP_IP_CMD_DRAIN)
    {
        return VPP_ERR_STATE;
    }

    HCP_LOCK(pstSessCb);

    if (pstSessCb->eExecutingCmd != VPP_IP_CMD_DRAIN)
    {
        vVppIpHcp_SetExecutingCmd(pstSessCb, VPP_IP_CMD_DRAIN);
        vVppIpHcp_SetCmdState(pstSessCb, HCP_CMD_STATE_DRAIN_PENDING_INPUT);
    }

    if (pstSessCb->eCmdState == HCP_CMD_STATE_DRAIN_PENDING_INPUT)
    {
        u32IntCnt = u32VppBufPool_Cnt(&pstSessCb->stInput.stPendingQ);
        vVppIpHcp_GetFwCnts(pstSessCb, &u32FwIn, &u32FwOut);
        if (!u32IntCnt && !u32FwIn && !u32FwOut)
        {
            vVppIpHcp_SetCmdState(pstSessCb, HCP_CMD_STATE_DRAIN_DONE);
        }
        LOGI("drain pending, u32Internal=%u, u32FwIn=%u", u32IntCnt, u32FwIn);
    }

    HCP_UNLOCK(pstSessCb);

    if (pstSessCb->eCmdState == HCP_CMD_STATE_DRAIN_DONE)
    {
        stEvt.eType = VPP_EVT_DRAIN_DONE;

        HCP_LOCK(pstSessCb);
        vVppIpHcp_SetCmdState(pstSessCb, HCP_CMD_STATE_NULL);
        vVppIpHcp_SetExecutingCmd(pstSessCb, VPP_IP_CMD_NULL);
        HCP_UNLOCK(pstSessCb);

        u32VppIpCbEvent(&pstSessCb->stBase.stCb, stEvt);
    }

    return u32Ret;
}

static uint32_t u32VppIpHcp_ProcCmdResetFw(t_StVppIpHcpSessionCb *pstSessCb)
{
    uint32_t u32, u32Ret = VPP_OK;
    t_StVppIpHcpDrvCtx *pstDrvCtx;
    t_EVppIpHcpHwState eHwState;

    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);

    pstDrvCtx = &pstSessCb->stDrvCtx;

    if (pstSessCb->eExecutingCmd != VPP_IP_CMD_NULL &&
        pstSessCb->eExecutingCmd != VPP_IP_CMD_RESET_FW)
    {
        LOGE("ProcCmdResetFw called with invalid state, eExecutingCmd=%u",
             pstSessCb->eExecutingCmd);
        u32Ret = VPP_ERR_STATE;
    }

    if (pstSessCb->eExecutingCmd != VPP_IP_CMD_RESET_FW)
    {
        vVppIpHcp_SetExecutingCmd(pstSessCb, (t_EVppIpCmd)VPP_IP_CMD_RESET_FW);

        eHwState = pstDrvCtx->eHwState;
        if (eHwState == HCP_HW_STATE_NULL)
        {
            // can directly open
            vVppIpHcp_SetCmdState(pstSessCb, HCP_CMD_STATE_RESET_OPEN_FW);
        }
        else if (eHwState == HCP_HW_STATE_ALLOC || eHwState == HCP_HW_STATE_RDY)
        {
            // need to close and open
            u32 = u32VppIpHcp_HwClose(pstSessCb);
            if (u32 != VPP_OK)
            {
                u32Ret = VPP_ERR_HW;
                vVppIpHcp_SetCmdState(pstSessCb,
                                      HCP_CMD_STATE_RESET_SIGNAL_CLIENT);
                LOGE("HwClose failed, u32=%u", u32);
            }
            else
            {
                vVppIpHcp_SetCmdState(pstSessCb,
                                      HCP_CMD_STATE_RESET_FW_WAIT_CLOSE);
            }
        }
        else
        {
            // hardware is in some invalid state - what do?
            vVppIpHcp_SetCmdState(pstSessCb,
                                  HCP_CMD_STATE_RESET_SIGNAL_CLIENT);
        }
    }

    if (pstSessCb->eCmdState == HCP_CMD_STATE_RESET_FW_WAIT_CLOSE)
    {
        eHwState = pstDrvCtx->eHwState;
        if (eHwState == HCP_HW_STATE_NULL)
        {
            // can directly open
            vVppIpHcp_SetCmdState(pstSessCb, HCP_CMD_STATE_RESET_OPEN_FW);
        }
    }

    if (pstSessCb->eCmdState == HCP_CMD_STATE_RESET_OPEN_FW)
    {
        u32 = u32VppIpHcp_HwOpen(pstSessCb);
        if (u32 != VPP_OK)
        {
            u32Ret = VPP_ERR_HW;
            LOGE("HwOpen failed, u32=%u", u32);
        }

        vVppIpHcp_SetCmdState(pstSessCb, HCP_CMD_STATE_RESET_SIGNAL_CLIENT);
    }

    if (pstSessCb->eCmdState == HCP_CMD_STATE_RESET_SIGNAL_CLIENT)
    {
        vVppIpHcp_SetCmdState(pstSessCb, HCP_CMD_STATE_NULL);
        vVppIpHcp_SetExecutingCmd(pstSessCb, VPP_IP_CMD_NULL);
        pstSessCb->async_res.u32ResetFwRet = u32Ret;
        HCP_LOCK(pstSessCb);
        pstSessCb->flags.bResetFwPending = VPP_FALSE;
        HCP_UNLOCK(pstSessCb);
        vVppIpHcp_SignalClient(pstSessCb);
    }

    return u32Ret;
}

static uint32_t u32VppIpHcp_ProcCmdSetOpMode(t_StVppIpHcpSessionCb *pstSessCb)
{
    uint32_t u32, u32Ret = VPP_OK;
    uint32_t bSignalClient;
    t_EVppIpHcpOpMode eOpModeNew;
    t_EVppIpHcpOpMode eOpModeActive;

    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);

    if (pstSessCb->eExecutingCmd != VPP_IP_CMD_NULL &&
        pstSessCb->eExecutingCmd != VPP_IP_CMD_SET_OP_MODE)
    {
        LOGE("ProcCmdOpMode called with invalid state, eExecutingCmd=%u",
             pstSessCb->eExecutingCmd);
        u32Ret = VPP_ERR_STATE;
    }

    if (pstSessCb->eExecutingCmd != VPP_IP_CMD_SET_OP_MODE)
    {
        vVppIpHcp_SetExecutingCmd(pstSessCb, (t_EVppIpCmd)VPP_IP_CMD_SET_OP_MODE);

        vVppIpHcp_Compute(pstSessCb, VPP_FALSE);

        eOpModeNew = pstSessCb->stCfg.eOpMode;

        if (eOpModeNew != HCP_OP_MODE_PROC && eOpModeNew != HCP_OP_MODE_MEAS)
        {
            // no need to change the op mode.
            vVppIpHcp_SetCmdState(pstSessCb,
                                  HCP_CMD_STATE_OP_MODE_SIGNAL_CLIENT);
        }
        else if (pstSessCb->stDrvCtx.eOpModeActive != eOpModeNew)
        {
            u32 = u32VppIpHcp_HwSessSetPropOpMode(pstSessCb, eOpModeNew);
            LOGE_IF(u32 != VPP_OK, "ERROR, HwSessSetPropOpMode, u32=%u", u32);

            if (u32 == VPP_OK)
            {
                u32 = u32VppIpHcp_HwSessGetPropBufReq(pstSessCb);
                LOGE_IF(u32 != VPP_OK, "ERROR, HwSessGetPropBufReq, u32=%u", u32);
            }

            if (u32 == VPP_OK)
            {
                vVppIpHcp_SetCmdState(pstSessCb,
                                      HCP_CMD_STATE_OP_MODE_WAIT_FW_RSP);
            }
            else
            {
                vVppIpHcp_SetCmdState(pstSessCb,
                                      HCP_CMD_STATE_OP_MODE_SIGNAL_CLIENT);
                u32Ret = VPP_ERR_HW;
                LOGE("unable set op_mode / get_buf_req, u32=%u", u32);
            }
        }
    }

    if (pstSessCb->eCmdState == HCP_CMD_STATE_OP_MODE_WAIT_FW_RSP)
    {
        if (HCP_STAT_EQ(pstSessCb, FwTxPropOpMode, FwRxPropOpMode) &&
            HCP_STAT_EQ(pstSessCb, FwTxGetPropBufReq, FwRxGetPropBufReq))
        {
            eOpModeActive = pstSessCb->stDrvCtx.eOpModeActive;
            if (eOpModeActive != pstSessCb->stCfg.eOpMode)
            {
                u32Ret = VPP_ERR_HW;
                LOGE("SetPropOpMode, expected eOpMode=%u, but eOpModeActive=%u",
                     pstSessCb->stCfg.eOpMode, eOpModeActive);
            }
            vVppIpHcp_SetCmdState(pstSessCb,
                                  HCP_CMD_STATE_OP_MODE_SIGNAL_CLIENT);
        }
    }

    if (pstSessCb->eCmdState == HCP_CMD_STATE_OP_MODE_SIGNAL_CLIENT)
    {
        vVppIpHcp_SetCmdState(pstSessCb, HCP_CMD_STATE_NULL);
        vVppIpHcp_SetExecutingCmd(pstSessCb, VPP_IP_CMD_NULL);
        pstSessCb->async_res.u32OpModeRet = u32Ret;
        HCP_LOCK(pstSessCb);
        pstSessCb->flags.bOpModePending = VPP_FALSE;
        HCP_UNLOCK(pstSessCb);
        vVppIpHcp_SignalClient(pstSessCb);
    }

    return u32Ret;
}

static uint32_t u32VppIpHcp_ProcCmdInternalDrain(t_StVppIpHcpSessionCb *pstSessCb)
{
    uint32_t u32InCnt, u32OutCnt;

    if (pstSessCb->eExecutingCmd != VPP_IP_CMD_NULL &&
        pstSessCb->eExecutingCmd != VPP_IP_CMD_INTERNAL_DRAIN)
    {
        return VPP_ERR_STATE;
    }

    if (pstSessCb->eExecutingCmd != VPP_IP_CMD_INTERNAL_DRAIN)
    {
        vVppIpHcp_SetExecutingCmd(pstSessCb, (t_EVppIpCmd)VPP_IP_CMD_INTERNAL_DRAIN);
    }

    vVppIpHcp_GetFwCnts(pstSessCb, &u32InCnt, &u32OutCnt);
    if (!u32InCnt && !u32OutCnt)
    {
        vVppIpHcp_SetExecutingCmd(pstSessCb, VPP_IP_CMD_NULL);
    }

    return VPP_OK;
}

static uint32_t u32VppIpBuf_PropGenericOneInOneOut(t_StVppBuf *pstBufIn,
                                                   t_StVppBuf *pstBufOut,
                                                   t_EVppBufType eOutBufType)
{
    uint64_t u64Ts;
    uint32_t u32, u32Ret = VPP_OK;

    if (!pstBufIn || !pstBufOut)
        return VPP_ERR_PARAM;

    u64Ts = u64VppBuf_GetTs(pstBufIn);
    vVppBuf_SetTs(pstBufOut, u64Ts);

    pstBufOut->stPixel.u32FilledLen = pstBufIn->stPixel.u32FilledLen;

    pstBufOut->pBuf->flags = pstBufIn->pBuf->flags;
    VPP_FLAG_CLR(pstBufOut->pBuf->flags, VPP_BUFFER_FLAG_READONLY);

    pstBufOut->pBuf->cookie_in_to_out = pstBufIn->pBuf->cookie_in_to_out;

    // It is not mandatory for the client to pass extra data to vpp, so allow
    // this to fail silently.
    u32VppBuf_PropagateExtradata(pstBufIn, pstBufOut, eOutBufType,
                                 VPP_EXTERNAL_EXTRADATA_TYPE);

    u32 = u32VppBuf_GrallocMetadataCopy(pstBufIn, pstBufOut);
    if (u32)
    {
        LOGE("GrallocMetadataCopy failed, u32=%u", u32);
        u32Ret = VPP_ERR;
    }

    return u32Ret;
}

static void vVppIpHcp_ReturnBuffers(t_StVppIpHcpSessionCb *pstSessCb,
                                    t_StVppBuf **ppstBufIn,
                                    uint32_t u32InCnt,
                                    t_StVppBuf **ppstBufOut,
                                    uint32_t u32OutCnt,
                                    uint32_t bBypassInput,
                                    uint32_t bZeroOutputFl)
{
    uint32_t i;

    VPP_RET_VOID_IF_NULL(pstSessCb);

    if (u32InCnt && ppstBufIn)
    {
        for (i = 0; i < u32InCnt; i++)
        {
            if (!ppstBufIn[i])
                continue;
            enum vpp_port ePort;
            ePort = bBypassInput ? VPP_PORT_OUTPUT : VPP_PORT_INPUT;
            u32VppIp_CbBufDone(&pstSessCb->stBase.stCb, ePort, ppstBufIn[i]);
        }
    }

    if (u32OutCnt && ppstBufOut)
    {
        for (i = 0; i < u32OutCnt; i++)
        {
            if (!ppstBufOut[i])
                continue;
            if (bZeroOutputFl)
                ppstBufOut[i]->stPixel.u32FilledLen = 0;
            u32VppIp_CbBufDone(&pstSessCb->stBase.stCb, VPP_PORT_OUTPUT,
                               ppstBufOut[i]);
        }
    }
}

static uint32_t u32VppIpHcp_ProcessBuffer(t_StVppIpHcpSessionCb *pstSessCb)
{
    uint32_t u32, u32Ret = VPP_OK;
    t_UVppIpHcpCanProcRes stProcRes;
    t_StVppBuf *apstBufIn[HCP_MAX_IN_BUF_PER_TRANS];
    t_StVppBuf *apstBufOut[HCP_MAX_OUT_BUF_PER_TRANS];
    uint32_t u32FwInCnt, u32FwOutCnt;
    t_StVppIpCmd stCmd;

    LOG_ENTER();

    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);

    memset(apstBufIn, 0, sizeof(apstBufIn));
    memset(apstBufOut, 0, sizeof(apstBufOut));

    HCP_LOCK(pstSessCb);

    // Call compute first since changing knobs could result in firmware
    // bypassing. In this case, handle the bypass to avoid sending the buffer.
    vVppIpHcp_Compute(pstSessCb, VPP_TRUE);

    if (!bVppIpHcp_CanProcess(pstSessCb, &stProcRes))
    {
        LOGD("ProcessBuffer has nothing to process");
        HCP_UNLOCK(pstSessCb);
        return VPP_OK;
    }

    if (pstSessCb->stDrvCtx.eHwState == HCP_HW_STATE_RDY)
    {
        u32 = u32VppIpHcp_HwSessSetPropKnobs(pstSessCb);
        if (u32 != VPP_OK)
        {
            LOGE("proc. unable to set knobs to firmware, u32=%u", u32);
        }
    }

    if (stProcRes.bCanBypassIn)
    {
        vVppIpHcp_GetFwCnts(pstSessCb, &u32FwInCnt, &u32FwOutCnt);
        if (u32FwInCnt || u32FwOutCnt)
        {
            stCmd.eCmd = (t_EVppIpCmd)VPP_IP_CMD_INTERNAL_DRAIN;
            u32VppIp_NtsCmdPut(&pstSessCb->stCmdQ, stCmd,
                               NULL, &pstSessCb->cond);
            HCP_UNLOCK(pstSessCb);
            return VPP_OK;
        }

        u32 = u32VppIp_PortBufGet(&pstSessCb->stInput, &apstBufIn[0], NULL);
        HCP_UNLOCK(pstSessCb);

        if (u32 != VPP_OK)
        {
            LOGE("unable to pull buffer from input queue, u32=%u", u32);
            return VPP_ERR;
        }
        vVppIpHcp_ReturnBuffers(pstSessCb, apstBufIn, 1, NULL, 0, VPP_TRUE, VPP_TRUE);
        return VPP_OK;
    }

    if (stProcRes.bCanBypassOut)
    {
        u32 = u32VppIp_PortBufGet(&pstSessCb->stOutput, &apstBufOut[0], NULL);
        HCP_UNLOCK(pstSessCb);

        if (u32 != VPP_OK)
        {
            LOGE("unable to pull buffer from output queue, u32=%u", u32);
            return VPP_ERR;
        }
        apstBufOut[0]->stPixel.u32FilledLen = 0;
        vVppIpHcp_ReturnBuffers(pstSessCb, NULL, 0, apstBufOut, 1, VPP_TRUE, VPP_TRUE);
        return VPP_OK;
    }

    uint32_t u32InputMin = pstSessCb->stBufReqIn.u32MinCnt;
    uint32_t u32OutputMin = pstSessCb->stBufReqOut.u32MinCnt;

    if (u32InputMin != u32OutputMin)
    {
        // TODO: this case should be handled when SESS_BUFREQ is returned
        // from firmware and we detect that we can't handle this.
        VPP_NOT_IMPLEMENTED();
        LOGE("ProcessBuffer failed, can not handle in != out");
    }

    // normal case

    uint32_t u32InCnt = 0, u32OutCnt = 0;
    uint32_t i;

    for (i = 0;  i < u32OutputMin; i++)
    {
        u32 = u32VppIp_PortBufGet(&pstSessCb->stOutput, &apstBufOut[i], NULL);
        if (u32 != VPP_OK || !apstBufOut[i])
        {
            LOGE("ProcessBuffer failed, unable to dq output, u32=%u, i=%u",
                 u32, i);
        }
        u32OutCnt++;
    }

    for (i = 0; i < u32InputMin; i++)
    {
        u32 = u32VppIp_PortBufGet(&pstSessCb->stInput, &apstBufIn[i], NULL);
        if (u32 != VPP_OK || !apstBufIn[i])
        {
            LOGE("ProcessBuffer failed, unable to dq output, u32=%u, i=%u",
                 u32, i);
        }
        u32InCnt++;
    }

    // copy input to output
    for (i = 0; i < u32InputMin; i++)
    {
        u32VppIpBuf_PropGenericOneInOneOut(apstBufIn[i], apstBufOut[i],
                                           eVppBufType_Progressive);
        if (pstSessCb->stCfg.eOpMode == HCP_OP_MODE_MEAS)
            apstBufOut[i]->stPixel.u32FilledLen = pstSessCb->stBufReqOut.u32PxSz;
    }

    HCP_UNLOCK(pstSessCb);

    for (i = 0; i < u32InCnt; i++)
    {
        if (VPP_FLAG_IS_SET(apstBufIn[i]->u32InternalFlags, VPP_BUF_FLAG_DUMP))
            vVppIpHcp_DumpFrame(pstSessCb, apstBufIn[i], VPP_PORT_INPUT);
    }

    // send for processing
    u32 = u32VppIpHcp_HwSessSendBufs(pstSessCb,
                                     apstBufIn, u32InCnt,
                                     apstBufOut, u32OutCnt);
    if (u32 == VPP_ERR_RESOURCES)
    {
        // Check the number of buffers in firmware. If there are buffers and we
        // received VPP_ERR_RESOURCES, then we should try to resubmit the
        // buffers at a later time, since there's a good chance firmware is
        // still alive and returning the buffers now would lead to out of order
        // frames.
        uint32_t u32Tmp = 0;
        vVppIpHcp_GetFwCnts(pstSessCb, &u32Tmp, NULL);
        if (u32Tmp)
        {
            LOGE("failed to submit %u buffers - deferring, u32FwIn=%u",
                 u32InputMin, u32Tmp);
            HCP_LOCK(pstSessCb);
            for (i = u32InCnt; i;)
            {
                i--;
                u32Tmp = u32VppIp_PortBufPutFront(&pstSessCb->stInput,
                                                  apstBufIn[i], NULL, NULL);
                if (u32Tmp != VPP_OK)
                {
                    LOGE("failed to submit and requeue input, bypassing, u32=%u",
                         u32Tmp);
                    HCP_UNLOCK(pstSessCb);
                    vVppIpHcp_ReturnBuffers(pstSessCb, &apstBufIn[i], 1,
                                            NULL, 0, VPP_TRUE, VPP_FALSE);

                    HCP_LOCK(pstSessCb);
                }
            }

            for (i = u32OutCnt; i;)
            {
                i--;
                u32Tmp = u32VppIp_PortBufPutFront(&pstSessCb->stOutput,
                                                  apstBufOut[i], NULL, NULL);
                if (u32Tmp != VPP_OK)
                {
                    HCP_UNLOCK(pstSessCb);
                    LOGE("failed to submit and requeue output, returning, u32=%u",
                         u32Tmp);
                    vVppIpHcp_ReturnBuffers(pstSessCb, NULL, 0, &apstBufOut[i],
                                            1, VPP_FALSE, VPP_TRUE);
                    HCP_LOCK(pstSessCb);
                }
            }
            HCP_UNLOCK(pstSessCb);
            u32 = VPP_OK;
        }
    }
    else if (u32 == VPP_OK)
    {
        // TODO: if this queue to intermediate port fails, should we return
        // the buffer to client? since if fw returns the buffer, we can't get
        // a reference to it, so we wouldn't return this buffer to the client
        // on that message... but what if we return it to client while fw
        // actually holds it, and client does something bad, like free it?
        HCP_LOCK(pstSessCb);
        for (i = 0; i < u32InCnt; i++)
        {
            u32 = u32VppIp_PortBufPut(&pstSessCb->stFwIn, apstBufIn[i],
                                      NULL, NULL);
            LOGE_IF(u32 != VPP_OK, "q to fw_in failed, buffer lost forever! "
                    "u32=%u, in_idx=%u, pstBuf=%p", u32, i, apstBufIn[i]);
        }
        for (i = 0; i < u32OutCnt; i++)
        {
            u32 = u32VppIp_PortBufPut(&pstSessCb->stFwOut, apstBufOut[i],
                                      NULL, NULL);
            LOGE_IF(u32 != VPP_OK, "q to fw_in failed, buffer lost forever! "
                    "u32=%u, in_idx=%u, pstBuf=%p", u32, i, apstBufOut[i]);
        }
        HCP_UNLOCK(pstSessCb);
    }

    if (u32 != VPP_OK)
    {
        vVppIpHcp_ReturnBuffers(pstSessCb, apstBufIn, u32InCnt,
                                apstBufOut, u32OutCnt, VPP_TRUE, VPP_TRUE);
    }

    return VPP_OK;
}

static uint32_t u32VppIpHcp_ProcMsgSetPropDone(t_StVppIpHcpSessionCb *pstSessCb,
                                               t_StHcpHfiMsgPkt *pstMsg)
{
    uint32_t u32PropId;
    uint32_t u32Ret = VPP_OK;
    int32_t s32FwErr;

    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pstMsg, VPP_ERR_PARAM);

    LOG_ENTER();

    s32FwErr = pstMsg->stHdr.s32Err;
    if (s32FwErr)
    {
        u32Ret = VPP_ERR_HW;
        LOGE("SET_PROPERTY_DONE returned error, s32Err=%d", pstMsg->stHdr.s32Err);
    }

    HCP_STAT(pstSessCb, FwRxSetProp);

    u32PropId = pstMsg->stMsgSetProp.propId;
    LOGI("ProcMsgPropDone, prop=%u (%s)",
         u32PropId, pcGetPropIdStr(u32PropId));

    switch (u32PropId)
    {
        case PROP_ID_SYS_DIAG:
        {
            HCP_STAT(pstSessCb, FwRxPropSetDiag);
            break;
        }
        case PROP_ID_SESS_RESOURCE_HINTS:
        {
            HCP_STAT(pstSessCb, FwRxPropResHints);
            break;
        }
        case PROP_ID_SESS_BUFREQ:
        {
            LOGE("ERROR: we don't set buffer requirements...");
            HCP_STAT(pstSessCb, FwRxGetProp);
            break;
        }
        case PROP_ID_SESS_KNOB:
        {
            HCP_STAT(pstSessCb, FwRxPropSetKnob);
            break;
        }
        case PROP_ID_SESS_OPMODE:
        {
            t_StVppIpHcpDrvCtx *pstDrvCtx = &pstSessCb->stDrvCtx;

            HCP_STAT(pstSessCb, FwRxPropOpMode);

            if (s32FwErr != AEE_SUCCESS)
            {
                u32Ret = VPP_ERR_HW;
                pstDrvCtx->eOpModeActive = HCP_OP_MODE_BYPASS;
                LOGE("firmware error in set op mode, s32Err=%d", s32FwErr);
            }
            break;
        }
        case PROP_ID_SESS_CP_STATUS:
        {
            HCP_STAT(pstSessCb, FwRxPropCpMode);
            break;
        }
        default:
            LOGE("unhandled SET_PROPERTY_DONE, u32PropId=%u", u32PropId);
            break;
    }

    LOG_EXIT_RET(u32Ret);
}

static uint32_t u32VppIpHcp_ProcMsgGetPropDone(t_StVppIpHcpSessionCb *pstSessCb,
                                               t_StHcpHfiMsgPkt *pstMsg)
{
    uint32_t u32PropId;
    uint32_t u32Ret = VPP_OK;

    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pstMsg, VPP_ERR_PARAM);

    LOG_ENTER();

    u32PropId = pstMsg->stMsgGetProp.propId;

    switch (u32PropId)
    {
        case PROP_ID_SESS_RESOURCE_HINTS:
            break;
        case PROP_ID_SESS_BUFREQ:
        {
            t_StSessBufReqs *pstBufReq = &pstMsg->stMsgGetProp.stBufReqs;
            uint32_t u32InputMin, u32OutputMin;

            HCP_LOCK(pstSessCb);

            u32InputMin = pstBufReq->u32InBuffsNeeded;
            u32OutputMin = pstBufReq->u32OutBuffsNeeded;

            if (u32InputMin != u32OutputMin ||
                u32InputMin > HCP_MAX_IN_BUF_PER_TRANS ||
                u32OutputMin > HCP_MAX_OUT_BUF_PER_TRANS)
            {
                LOGE("ERROR: fw buf req exceeds ours, "
                     "u32InputMin=%u, u32OutputMin=%u",
                     u32InputMin, u32OutputMin);
                VPP_NOT_IMPLEMENTED();
            }

            pstSessCb->stDrvCtx.req.u32InCntMin = u32InputMin;
            pstSessCb->stDrvCtx.req.u32OutCntMin = u32OutputMin;

            pstSessCb->stBufReqIn.u32MinCnt = u32InputMin;
            pstSessCb->stBufReqOut.u32MinCnt = u32OutputMin;

            if (pstSessCb->stCfg.eOpMode == HCP_OP_MODE_MEAS)
                pstSessCb->stBufReqOut.u32PxSz = pstBufReq->u32MeasBuffSize;

            pstSessCb->stIntBufs.bDirty = VPP_FALSE;

            if (pstBufReq->u32MeasBuffSize)
            {
                pstSessCb->stIntBufs.stIntBufMeas.u32Sz = pstBufReq->u32MeasBuffSize;
            }
            else
            {
                LOGE("firmware returned 0 for int. meas buffer, hardcoding 4k");
                pstSessCb->stIntBufs.stIntBufMeas.u32Sz = 4096;
            }

            if (pstBufReq->u32IntPixBuffSize)
            {
                pstSessCb->stIntBufs.stIntBufPx.u32Sz = pstBufReq->u32IntPixBuffSize;
            }
            else
            {
                LOGE("firmware returned 0 for int. pixel buffer, hardcoding 4k");
                pstSessCb->stIntBufs.stIntBufPx.u32Sz = 4096;
            }

            LOGD("get_buf_reqs: u32InputMin=%u, u32OutputMin=%u, u32MeasSz=%u, "
                 "u32PxSz=%u", pstSessCb->stBufReqIn.u32MinCnt,
                 pstSessCb->stBufReqOut.u32MinCnt,
                 pstSessCb->stIntBufs.stIntBufMeas.u32Sz,
                 pstSessCb->stIntBufs.stIntBufPx.u32Sz);
            HCP_UNLOCK(pstSessCb);

            HCP_STAT(pstSessCb, FwRxGetPropBufReq);
            break;
        }
        case PROP_ID_SESS_KNOB:
            break;
        case PROP_ID_SESS_OPMODE:
            break;
        case PROP_ID_SESS_CP_STATUS:
            break;
        default:
            break;
    }

    LOG_EXIT_RET(u32Ret);
}

static uint32_t u32VppIpHcp_ProcMsgClientBuffReleased(t_StVppIpHcpSessionCb *pstSessCb,
                                                      t_StHcpHfiMsgPkt *pstMsg)
{
    uint32_t u32;
    t_StHcpHfiBufAttr *pstAttr;
    t_StHcpHfiPixBufAttr *pstPixBufAttr;
    enum vpp_port eDstPort;
    t_StVppIpPort *pstPort;
    t_StVppBuf *pstBuf;
    int32_t s32Err;
    uint32_t u32CH, u32CL;
    uint32_t bOutputErr = VPP_FALSE;

    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pstMsg, VPP_ERR_PARAM);

    pstAttr = &pstMsg->stMsgBuf.bufAttr;
    pstPixBufAttr = &pstAttr->stPixBufAttr;

    if (pstAttr->eBufType != BUFF_TYPE_PIXEL &&
        pstAttr->eBufType != BUFF_TYPE_MEAS)
    {
        LOGE("received invalid buffer type from firmware!");
        return VPP_ERR_PARAM;
    }

    eDstPort = VPP_PORT_OUTPUT;
    s32Err = pstMsg->stHdr.s32Err;
    if (pstAttr->eBufType == BUFF_TYPE_PIXEL)
    {
        if (pstPixBufAttr->eDirection == BUFDIR_INPUT)
        {
            if (HCP_STAT_GET(pstSessCb, InRxCnt))
                VPP_IP_PROF_STOP(&pstSessCb->stBase, HCP_STAT_IN_RELEASE_DELTA);
            VPP_IP_PROF_START(&pstSessCb->stBase, HCP_STAT_IN_RELEASE_DELTA);

            HCP_STAT(pstSessCb, InRxCnt);
            pstSessCb->stDrvCtx.u32InBufCnt--;
            pstPort = &pstSessCb->stFwIn;
            eDstPort = VPP_PORT_INPUT;
            if (s32Err != AEE_SUCCESS ||
                pstAttr->u32Operation != BUFF_OP_PROCESSED)
                eDstPort = VPP_PORT_OUTPUT;
        }
        else if (pstPixBufAttr->eDirection == BUFDIR_OUTPUT)
        {
            if (HCP_STAT_GET(pstSessCb, OutRxCnt))
                VPP_IP_PROF_STOP(&pstSessCb->stBase, HCP_STAT_OUT_RELEASE_DELTA);
            VPP_IP_PROF_START(&pstSessCb->stBase, HCP_STAT_OUT_RELEASE_DELTA);

            HCP_STAT(pstSessCb, OutRxCnt);
            pstSessCb->stDrvCtx.u32OutBufCnt--;
            pstPort = &pstSessCb->stFwOut;

            if (s32Err != AEE_SUCCESS ||
                pstAttr->u32Operation != BUFF_OP_PROCESSED)
                bOutputErr = VPP_TRUE;
        }
        else
        {
            LOGE("unrecognized port");
            return VPP_ERR_HW;
        }
    }
    else
    {
        if (HCP_STAT_GET(pstSessCb, OutRxCnt))
            VPP_IP_PROF_STOP(&pstSessCb->stBase, HCP_STAT_OUT_RELEASE_DELTA);
        VPP_IP_PROF_START(&pstSessCb->stBase, HCP_STAT_OUT_RELEASE_DELTA);

        HCP_STAT(pstSessCb, OutRxCnt);
        pstSessCb->stDrvCtx.u32OutBufCnt--;
        pstPort = &pstSessCb->stFwOut;

        if (s32Err != AEE_SUCCESS ||
            pstAttr->u32Operation != BUFF_OP_PROCESSED)
            bOutputErr = VPP_TRUE;
    }

    HCP_LOCK(pstSessCb);

    pstBuf = pstVppIp_PortBufPeek(pstPort, 0, NULL);
    if (!pstBuf)
    {
        LOGE("got an output buffer but failed to find it in fw port");
        HCP_UNLOCK(pstSessCb);
        return VPP_ERR;
    }

    u32CH = pstAttr->eBufType == BUFF_TYPE_PIXEL ?
        HCP_P_COOKIE_H(pstSessCb, pstBuf) : HCP_M_COOKIE_H(pstSessCb, pstBuf);
    u32CL = pstAttr->eBufType == BUFF_TYPE_PIXEL ?
        HCP_P_COOKIE_L(pstSessCb, pstBuf) : HCP_M_COOKIE_L(pstSessCb, pstBuf);

    if (u32CH == pstAttr->u32Cookie_h && u32CL == pstAttr->u32Cookie_l)
    {
        u32 = u32VppIp_PortBufGet(pstPort, &pstBuf, NULL);
    }
    else
    {
        t_StVppIpHcpExpBuf stExp = {
            .eBufType = pstAttr->eBufType,
            .pstSessCb = pstSessCb,
            .u32CL = pstAttr->u32Cookie_l,
            .u32CH = pstAttr->u32Cookie_h,
        };

        u32 = u32VppIp_PortBufGetMatching(pstPort, &pstBuf, NULL,
                                          u32VppIpHcp_MatchBuf, &stExp);
        LOGE("unexpected buffer, exp/got: l=(0x%x/0x%x), h=(0x%x/0x%x), found "
             "matching? pstBuf=%p, u32=%u", u32CL, pstAttr->u32Cookie_l,
             u32CH, pstAttr->u32Cookie_h, pstBuf, u32);
    }

    if (u32 != VPP_OK || !pstBuf)
    {
        LOGE("unable to pull buffer from port, buffer lost, u32=%u", u32);
        HCP_UNLOCK(pstSessCb);
        return VPP_ERR;
    }

#ifdef HCP_DBG_WRITE_DUMP_BUF
    LOGE("dumping %s buffer:", pstPixBufAttr->eDirection == BUFDIR_INPUT ?
         "input" : "output");
    u32VppIpHcpUtils_DumpFromBuf(pstBuf, HCP_DBG_WB_SIZE);
#endif

    if (pstPixBufAttr->eDirection == BUFDIR_OUTPUT)
    {
        vVppIpHcp_ExtractUbwcStats(pstBuf, pstPixBufAttr);

        if (VPP_FLAG_IS_SET(pstBuf->u32InternalFlags, VPP_BUF_FLAG_DUMP))
        {
            vVppIpHcp_DumpFrame(pstSessCb, pstBuf, VPP_PORT_OUTPUT);
        }
    }

    if (bOutputErr)
    {
        pstBuf->stPixel.u32FilledLen = 0;
    }

    HCP_UNLOCK(pstSessCb);

    u32VppIp_CbBufDone(&pstSessCb->stBase.stCb, eDstPort, pstBuf);

    return VPP_OK;
}

static uint32_t u32VppIpHcp_ProcMsgIntPxBuffReleased(t_StVppIpHcpSessionCb *pstSessCb,
                                                     t_StHcpHfiMsgPkt *pstMsg)
{
    t_StHcpHfiBufAttr *pstAttr;
    t_StVppIpHcpIntBuf *pstIntBufPx;

    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pstMsg, VPP_ERR_PARAM);

    pstAttr = &pstMsg->stMsgBuf.bufAttr;
    pstIntBufPx = &pstSessCb->stIntBufs.stIntBufPx;

    if (pstAttr->u32Cookie_h != HCP_INTP_COOKIE_H(pstSessCb) ||
        pstAttr->u32Cookie_l != HCP_INTP_COOKIE_L(pstSessCb))
    {
        LOGE("ERROR: fw returned int px that we do not own, "
             "got: u32L=0x%x, u32H=0x%x, expected: u32L=0x%x, u32H=0x%x",
             pstAttr->u32Cookie_l, pstAttr->u32Cookie_h,
             HCP_INTP_COOKIE_L(pstSessCb), HCP_INTP_COOKIE_H(pstSessCb));
    }
    else
    {
        pstIntBufPx->eBufState = HCP_BUF_OWNER_IP;
    }

    return VPP_OK;
}

static uint32_t u32VppIpHcp_ProcMsgIntMeasBuffReleased(t_StVppIpHcpSessionCb *pstSessCb,
                                                       t_StHcpHfiMsgPkt *pstMsg)
{
    t_StHcpHfiBufAttr *pstAttr;
    t_StVppIpHcpIntBuf *pstIntBufMeas;

    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pstMsg, VPP_ERR_PARAM);

    pstAttr = &pstMsg->stMsgBuf.bufAttr;
    pstIntBufMeas = &pstSessCb->stIntBufs.stIntBufMeas;

    if (pstAttr->u32Cookie_h != HCP_INTM_COOKIE_H(pstSessCb) ||
        pstAttr->u32Cookie_l != HCP_INTM_COOKIE_L(pstSessCb))
    {
        LOGE("ERROR: fw returned int meas that we do not own, "
             "got: u32L=0x%x, u32H=0x%x, expected: u32L=0x%x, u32H=0x%x",
             pstAttr->u32Cookie_l, pstAttr->u32Cookie_h,
             HCP_INTM_COOKIE_L(pstSessCb), HCP_INTM_COOKIE_H(pstSessCb));
    }
    else
    {
        pstIntBufMeas->eBufState = HCP_BUF_OWNER_IP;
    }

    return VPP_OK;
}

static inline void debug_buf_released(t_StHcpHfiMsgPkt *pstMsg)
{
    t_StHcpHfiMsgPktHdr *ph = &pstMsg->stHdr;
    t_StHcpHfiBufAttr *p = &pstMsg->stMsgBuf.bufAttr;

#ifdef HCP_DBG_PA_CNTS
    LOGD("ProcMsgBuffReleased, s32Err=%d, eBufType=%u, cookie_l=0x%x, "
         "cookie_h=0x%x, op=%u, pa=0x%x, irq=0x%x, err_irq=0x%x, cnt=%u",
         ph->s32Err, p->eBufType, p->u32Cookie_l, p->u32Cookie_h, p->u32Operation,
         p->u32Pa, p->u32IrqStatus, p->u32ErrIrqStatus, p->u32Counts);
#else
    LOGD("ProcMsgBuffReleased, s32Err=%d, eBufType=%u, cookie_l=0x%x, "
         "cookie_h=0x%x, op=%u, cnt=%u", ph->s32Err, p->eBufType, p->u32Cookie_l,
         p->u32Cookie_h, p->u32Operation, p->u32Counts);
#endif
}

static uint32_t u32VppIpHcp_ProcMsgBuffReleased(t_StVppIpHcpSessionCb *pstSessCb,
                                                t_StHcpHfiMsgPkt *pstMsg)
{
    uint32_t u32Ret = VPP_OK;
    t_StHcpHfiBufAttr *pstAttr;

    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pstMsg, VPP_ERR_PARAM);

    LOG_ENTER();

    debug_buf_released(pstMsg);

    pstAttr = &pstMsg->stMsgBuf.bufAttr;

    if (pstAttr->eBufType == BUFF_TYPE_PIXEL)
    {
        u32Ret = u32VppIpHcp_ProcMsgClientBuffReleased(pstSessCb, pstMsg);
    }
    else if (pstAttr->eBufType == BUFF_TYPE_INTERMEDIATE_PIXEL)
    {
        u32Ret = u32VppIpHcp_ProcMsgIntPxBuffReleased(pstSessCb, pstMsg);
    }
    else if (pstAttr->eBufType == BUFF_TYPE_MEAS)
    {
        if (pstAttr->u32Cookie_h == HCP_INTM_COOKIE_H(pstSessCb) &&
            pstAttr->u32Cookie_l == HCP_INTM_COOKIE_L(pstSessCb))
        {
            u32Ret = u32VppIpHcp_ProcMsgIntMeasBuffReleased(pstSessCb, pstMsg);
        }
        else
        {
            u32Ret = u32VppIpHcp_ProcMsgClientBuffReleased(pstSessCb, pstMsg);
        }
    }
    else
    {
        u32Ret = VPP_ERR_PARAM;
    }

    LOG_EXIT_RET(u32Ret);
}

static uint32_t u32VppIpHcp_ProcMsgCloseDone(t_StVppIpHcpSessionCb *pstSessCb,
                                             t_StHcpHfiMsgPkt *pstMsg)
{
    uint32_t u32Ret = VPP_OK;

    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pstMsg, VPP_ERR_PARAM);

    LOG_ENTER();

    VPP_IP_PROF_STOP(&pstSessCb->stBase, HCP_STAT_CLOSE_LATENCY);

    HCP_SESS_GET_HW_ID(pstSessCb) = SESSION_ID_INVALID;
    vVppIpHcp_HwSetState(pstSessCb, HCP_HW_STATE_NULL);
    HCP_STAT(pstSessCb, FwRxClose);

    if (pstMsg->stHdr.s32Err)
    {
        LOGE("close message returned error, s32Err=%d", pstMsg->stHdr.s32Err);
        HCP_STAT(pstSessCb, FwRxCloseErr);
        u32Ret = VPP_ERR_HW;
        VPP_NOT_IMPLEMENTED();
    }

    if (pstSessCb->stDrvCtx.u32InBufCnt || pstSessCb->stDrvCtx.u32OutBufCnt)
    {
        u32Ret = VPP_ERR_HW;
        LOGE("ERROR: CLOSE_DONE, but fw still owns buffers, "
             "u32InBufCnt=%u, u32OutBufCnt=%u", pstSessCb->stDrvCtx.u32InBufCnt,
             pstSessCb->stDrvCtx.u32OutBufCnt);
    }

    if (!u32VppIpHcp_IntPxIsOurs(pstSessCb))
    {
        u32Ret = VPP_ERR_HW;
        LOGE("ERROR: CLOSE_DONE, but we don't own int. px");
    }

    if (!u32VppIpHcp_IntMeasIsOurs(pstSessCb))
    {
        u32Ret = VPP_ERR_HW;
        LOGE("ERROR: CLOSE_DONE, but we don't own int. meas");
    }

    LOG_EXIT_RET(u32Ret);
}

static uint32_t u32VppIpHcp_ProcMsg(t_StVppIpHcpSessionCb *pstSessCb,
                                    t_StHcpHfiMsgPkt *pstMsg)
{
    uint32_t u32Ret = VPP_OK;

    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pstMsg, VPP_ERR_PARAM);

    LOG_ENTER();

    vVppIpHcp_DumpRspMsg(pstMsg, VPP_FALSE, "SessWrk");

    switch (pstMsg->stHdr.eMsgId)
    {
        case MSG_ID_SET_PROPERTY_DONE:
        {
            u32Ret = u32VppIpHcp_ProcMsgSetPropDone(pstSessCb, pstMsg);
            break;
        }
        case MSG_ID_PROPERTY:
        {
            u32Ret = u32VppIpHcp_ProcMsgGetPropDone(pstSessCb, pstMsg);
            break;
        }
        case MSG_ID_BUFF_RELEASED:
        {
            u32Ret = u32VppIpHcp_ProcMsgBuffReleased(pstSessCb, pstMsg);
            break;
        }
        case MSG_ID_CLOSE_DONE:
        {
            u32Ret = u32VppIpHcp_ProcMsgCloseDone(pstSessCb, pstMsg);
            break;
        }
        case MSG_ID_EVENT_NOTIFY:
        {
            break;
        }
        case MSG_ID_NOOP:
            HCP_STAT(pstSessCb, FwRxCmdNop);
            // intentional fall through
        default:
        {
            LOGE("unhandled message in session: sess_id=%u, s32FwId=%d",
                 pstSessCb->u32SessId, HCP_SESS_GET_HW_ID(pstSessCb));
            vVppIpHcp_DumpRspMsg(pstMsg, VPP_TRUE, "SessWrkErr");
        }
    }

    LOG_EXIT_RET(u32Ret);
}

static uint32_t u32VppIpHcp_HandlePendingCommand(t_StVppIpHcpSessionCb *pstSessCb)
{
    uint32_t u32;
    uint32_t u32Ret = VPP_FALSE;

    LOG_ENTER_ARGS("eExecutingCmd=%u", pstSessCb->eExecutingCmd);

    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);

    if (pstSessCb->eExecutingCmd == VPP_IP_CMD_NULL)
    {
    }

    if (pstSessCb->eExecutingCmd == VPP_IP_CMD_INIT)
    {
        if (HCP_STAT_EQ(pstSessCb, FwTxPropCpMode, FwRxPropCpMode) &&
            HCP_STAT_EQ(pstSessCb, FwTxPropOpMode, FwRxPropOpMode) &&
            HCP_STAT_EQ(pstSessCb, FwTxGetPropBufReq, FwRxGetPropBufReq))
        {
            vVppIpHcp_SetExecutingCmd(pstSessCb, VPP_IP_CMD_NULL);
            pstSessCb->u32WorkerErr = VPP_OK;
            vVppIpHcp_SignalWorkerStart(pstSessCb);
        }
        u32Ret = VPP_TRUE;
    }

    if (pstSessCb->eExecutingCmd == VPP_IP_CMD_TERM)
    {
        if (HCP_SESS_GET_HW_ID(pstSessCb) == SESSION_ID_INVALID &&
            pstSessCb->stDrvCtx.eHwState == HCP_HW_STATE_NULL)
        {
            u32 = u32VppIpHcp_FreeIntBufs(pstSessCb);
            LOGE_IF(u32 != VPP_OK, "unable to free int. bufs, u32=%u", u32);

            vVppIpHcp_SetExecutingCmd(pstSessCb, VPP_IP_CMD_NULL);
        }
        u32Ret = VPP_TRUE;
    }

    if (pstSessCb->eExecutingCmd == VPP_IP_CMD_OPEN)
    {
        u32 = u32VppIpHcp_ProcCmdOpen(pstSessCb);
        LOGE_IF(u32 != VPP_OK, "ProcCmdOpen error, u32=%u", u32);
        u32Ret = VPP_TRUE;
    }

    if (pstSessCb->eExecutingCmd == VPP_IP_CMD_CLOSE)
    {
        u32 = u32VppIpHcp_ProcCmdClose(pstSessCb);
        LOGE_IF(u32 != VPP_OK, "ProcCmdClose error, u32=%u", u32);
        u32Ret = VPP_TRUE;
    }

    if (pstSessCb->eExecutingCmd == VPP_IP_CMD_FLUSH)
    {
        u32 = u32VppIpHcp_ProcCmdFlush(pstSessCb, NULL);
        LOGE_IF(u32 != VPP_OK, "ProcCmdFlush error, u32=%u", u32);
        u32Ret = VPP_TRUE;
    }

    if (pstSessCb->eExecutingCmd == VPP_IP_CMD_DRAIN)
    {
        u32 = u32VppIpHcp_ProcCmdDrain(pstSessCb);
        LOGE_IF(u32 != VPP_OK, "ProcCmdDrain error, u32=%u", u32);
        if (pstSessCb->eExecutingCmd == VPP_IP_CMD_NULL)
            u32Ret = VPP_TRUE;
    }

    if (pstSessCb->eExecutingCmd == VPP_IP_CMD_RESET_FW)
    {
        u32 = u32VppIpHcp_ProcCmdResetFw(pstSessCb);
        LOGE_IF(u32 != VPP_OK, "ProcCmdResetFw error, u32=%u", u32);
        u32Ret = VPP_TRUE;
    }

    if (pstSessCb->eExecutingCmd == VPP_IP_CMD_SET_OP_MODE)
    {
        u32 = u32VppIpHcp_ProcCmdSetOpMode(pstSessCb);
        LOGE_IF(u32 != VPP_OK, "ProcCmdSetOpMode error, u32=%u", u32);
        u32Ret = VPP_TRUE;
    }

    if (pstSessCb->eExecutingCmd == VPP_IP_CMD_INTERNAL_DRAIN)
    {
        u32 = u32VppIpHcp_ProcCmdInternalDrain(pstSessCb);
        LOGE_IF(u32 != VPP_OK, "ProcCmdInternalDrain error, u32=%u", u32);
        if (pstSessCb->eExecutingCmd != VPP_IP_CMD_NULL)
            u32Ret = VPP_TRUE;
    }

    LOG_EXIT_ARGS("eExecutingCmd=%u, u32Ret=%u",
                  pstSessCb->eExecutingCmd, u32Ret);

    return u32Ret;
}

static void vVppIpHcp_BroadcastGlobalRes(t_StVppIpHcpGlobalCb *pstGlobal,
                                         uint32_t u32Res)
{
    if (!pstGlobal)
        return;

    HCP_LOCK(pstGlobal);
    pstGlobal->u32AsyncRes = u32Res;
    pstGlobal->u.bRxResult = VPP_TRUE;
    HCP_BROADCAST(pstGlobal);
    HCP_UNLOCK(pstGlobal);
}

static void *vpVppIpHcp_ReceiverWorker(void *pv)
{
    LOG_ENTER();

    uint32_t u32;
    uint32_t bRecycleNode;
    uint32_t bNopRx;
    t_StVppIpHcpMsgQNode *pstMsgQNode;
    t_StHcpHfiMsgPkt *pstMsg;
    t_StHcpHfiMsgPktHdr *pstHdr;

    t_StVppIpHcpSessionCb *pstSess;
    t_StVppIpHcpGlobalCb *pstGlobal = (t_StVppIpHcpGlobalCb *)pv;

    uint32_t u32Exit = VPP_FALSE;

    u32 = u32VppIpHcp_HwGetHandle(pstGlobal);
    if (u32 != VPP_OK)
    {
        LOGE("unable to get hw handle, u32=%u", u32);
        vVppIpHcp_BroadcastGlobalRes(pstGlobal, VPP_ERR_HW);
        goto ERR_GET_HANDLE;
    }

    u32 = u32VppIpHcp_HwGlobalGetInfo(pstGlobal, HCP_HOST_LOCK);
    if (u32 != VPP_OK)
    {
        LOGE("unable to get info w/ lock, u32=%u", u32);
        vVppIpHcp_BroadcastGlobalRes(pstGlobal, VPP_ERR_HW);
        goto ERR_LOCK_HOST;
    }

    u32 = u32VppIpHcp_TuningLoad(pstGlobal);
    LOGE_IF(u32 != VPP_OK, "unable to load tunings. ret=%u", u32);

    vVppIpHcp_BroadcastGlobalRes(pstGlobal, VPP_OK);

    while (1)
    {
#if 0
        HCP_LOCK(pstGlobal);
        if (!pstGlobal->u.bTalkWithFw && !pstGlobal->u.bPendShutdown)
        {
            LOGI("not talking to firmware and not pending shutdown");
            pthread_cond_wait(&pstGlobal->cond, &pstGlobal->mutex);
            LOGI("awake from cond variable");
        }
        HCP_UNLOCK(pstGlobal);
#endif
        if (u32Exit)
        {
            LOGI("breaking out of loop due to u32Exit");
            break;
        }

        HCP_LOCK(pstGlobal);
        while ((pstMsgQNode =
                pstVppIpHcp_MsgQGet(&pstGlobal->stMsgQ, NULL)) == NULL)
        {
            LOGE("no more message nodes available... waiting for one");
            HCP_WAIT(pstGlobal);
        }
        HCP_UNLOCK(pstGlobal);

        pstMsg = &pstMsgQNode->stMsgPkt;
        pstHdr = &pstMsg->stHdr;
        bRecycleNode = VPP_TRUE;

        u32 = u32VppIpHcp_HwGlobalRecvPkt(pstGlobal, pstMsg);
        if (u32 == VPP_OK)
            vVppIpHcp_DumpRspMsg(pstMsg, VPP_FALSE, "Rsp");

        if (u32 != VPP_OK)
        {
            LOGE("HwGlobalRecvPkt returned error, u32=%u", u32);
        }
        else if (u32VppIpHcp_MsgIsForSession(pstHdr))
        {
            pstSess = pstVppIpHcp_FindSession(pstGlobal, pstHdr->s32SessId);
            if (!pstSess)
            {
                LOGE("HcpRxWorker: unable to find session cb for sessId=%d",
                     pstHdr->s32SessId);
                HCP_ASSERT(VPP_FALSE);
            }
            else
            {
                u32 = u32VppIpHcp_MsgQPut(&pstSess->stMsgQ, pstMsgQNode,
                                          &pstSess->mutex, &pstSess->cond);
                if (u32 != VPP_OK)
                {
                    LOGE("unable to queue message to session=%p, s32FwId=%d, "
                         "local_sess_id=%u, u32=%u", pstSess,
                         pstHdr->s32SessId, pstSess->u32SessId, u32);
                }
                else
                {
                    LOGI("successfully queued message to session=%p, s32FwId=%d, "
                         "local_sess_id=%u, u32=%u", pstSess,
                         pstHdr->s32SessId, pstSess->u32SessId, u32);
                    bRecycleNode = VPP_FALSE;
                    continue;
                }
            }
        }
        else
        {
            // system command
            bNopRx = VPP_FALSE;
            u32 = u32VppIpHcp_HandleSystemMessage(pstGlobal, pstMsg, &bNopRx);
            LOGE_IF(u32 != VPP_OK, "unable to handle system message, u32=%u", u32);
            if (bNopRx)
            {
                HCP_LOCK(pstGlobal);
                if (pstGlobal->u.bPendShutdown)
                {
                    u32Exit = VPP_TRUE;
                    LOGI("pending shutdown, received nop, ready to exit...");
                }
                else
                {
                    LOGE("received nop, but not pending shutdown...");
                }
                HCP_UNLOCK(pstGlobal);
            }
        }

        if (bRecycleNode)
        {
            u32 = u32VppIpHcp_MsgQPut(&pstGlobal->stMsgQ, pstMsgQNode,
                                      &pstGlobal->mutex, NULL);
            LOGE_IF(u32 != VPP_OK, "unable to return q node to pool, u32=%u", u32);
        }
    }

    u32 = u32VppIpHcp_HwGlobalGetInfo(pstGlobal, HCP_HOST_UNLOCK);
    if (u32 != VPP_OK)
    {
        VPP_NOT_IMPLEMENTED();
        LOGE("unable to get info w/ unlock, u32=%u", u32);
    }

ERR_LOCK_HOST:
    u32 = u32VppIpHcp_HwReleaseHandle(pstGlobal);
    LOGE_IF(u32 != VPP_OK, "HwReleaseHandle failed, u32=%u", u32);

ERR_GET_HANDLE:
    HCP_LOCK(pstGlobal);
    pstGlobal->u.bPendShutdown = VPP_FALSE;
    HCP_UNLOCK(pstGlobal);

    LOG_EXIT();

    return NULL;
}

static void *vpVppIpHcp_SessionWorker(void *pv)
{
    uint32_t u32;
    t_StVppIpCmd stCmd;

    t_StVppIpHcpGlobalCb *pstGlobal = NULL;
    t_StVppIpHcpSessionCb *pstSessCb;
    t_StVppIpHcpMsgQNode *pstMsg;

    VPP_RET_IF_NULL(pv, NULL);

    pstSessCb = (t_StVppIpHcpSessionCb *)pv;
    if (pstSessCb)
        pstGlobal = pstSessCb->pstGlobal;

    LOG_ENTER_ARGS("pstGlobal=%p, pstSessCb=%p", pstGlobal, pstSessCb);

    if (!pstSessCb || !pstGlobal)
    {
        LOGE_IF(!pstSessCb, "pstSessCb was null");
        LOGE_IF(!pstGlobal, "pstGlobal was null");
        goto HCP_WORKER_EXIT;
    }

    while (1)
    {
        HCP_LOCK(pstSessCb);
        while (u32VppIpHcp_WorkerShouldSleep(pstSessCb))
        {
            LOGI("sleeping worker thread...");
            HCP_WAIT(pstSessCb);
        }
        LOGI("worker thread awake...");
        HCP_UNLOCK(pstSessCb);

        uint32_t u32DbgCnt = 0;
        while ((pstMsg = pstVppIpHcp_GetHcpMessage(pstSessCb)) != NULL)
        {
            LOGI("session worker got hcp message... handling it!");

            u32 = u32VppIpHcp_ProcMsg(pstSessCb, &pstMsg->stMsgPkt);
            LOGE_IF(u32 != VPP_OK, "unable to process message, u32=%u", u32);

            u32 = u32VppIpHcp_MsgQPut(&pstGlobal->stMsgQ, pstMsg,
                                      &pstGlobal->mutex, &pstGlobal->cond);
            LOGE_IF(u32 != VPP_OK, "unable to return message node, u32=%u", u32);

            u32DbgCnt++;

            LOGI("processed %u %s from firmware", u32DbgCnt,
                 u32DbgCnt > 1 ? "messages" : "message");

            HCP_STAT(pstSessCb, FwMsgRx);
        }

        if (u32VppIpHcp_HandlePendingCommand(pstSessCb))
        {
            LOGI("handle pending command...");
            continue;
        }

        if (u32VppIpHcp_ShouldFetchCmd(pstSessCb))
        {
            u32 = u32VppIp_NtsCmdGet(&pstSessCb->stCmdQ, &stCmd,
                                     &pstSessCb->mutex);
            if (u32 == VPP_OK)
            {
                if (stCmd.eCmd == VPP_IP_CMD_THREAD_EXIT)
                    break;

                else if (stCmd.eCmd == VPP_IP_CMD_INIT)
                    u32VppIpHcp_ProcCmdInit(pstSessCb);

                else if (stCmd.eCmd == VPP_IP_CMD_TERM)
                    u32VppIpHcp_ProcCmdTerm(pstSessCb);

                else if (stCmd.eCmd == VPP_IP_CMD_OPEN)
                    u32VppIpHcp_ProcCmdOpen(pstSessCb);

                else if (stCmd.eCmd == VPP_IP_CMD_CLOSE)
                    u32VppIpHcp_ProcCmdClose(pstSessCb);

                else if (stCmd.eCmd == VPP_IP_CMD_FLUSH)
                    u32VppIpHcp_ProcCmdFlush(pstSessCb, &stCmd);

                else if (stCmd.eCmd == VPP_IP_CMD_DRAIN)
                    u32VppIpHcp_ProcCmdDrain(pstSessCb);

                else if (stCmd.eCmd == VPP_IP_CMD_RESET_FW)
                    u32VppIpHcp_ProcCmdResetFw(pstSessCb);

                else if (stCmd.eCmd == VPP_IP_CMD_SET_OP_MODE)
                    u32VppIpHcp_ProcCmdSetOpMode(pstSessCb);

                else if (stCmd.eCmd == VPP_IP_CMD_INTERNAL_DRAIN)
                    u32VppIpHcp_ProcCmdInternalDrain(pstSessCb);

                else
                    LOGE("unknown command pulled from queue");

                continue;
            }
            else
            {
                LOGE("unable to pull command from queue, u32=%u", u32);
            }
        }

        if (!VPP_IP_STATE_EQUAL(pstSessCb, VPP_IP_STATE_ACTIVE))
        {
            LOGE("probably shouldn't be here...");
            continue;
        }

        if (VPP_IP_STATE_EQUAL(pstSessCb, VPP_IP_STATE_ACTIVE))
        {
            u32 = u32VppIpHcp_ProcessBuffer(pstSessCb);
        }
    }

    MUTEX_LOCK(&pstSessCb->mutex);
    pstSessCb->flags.bWorkerEnded = VPP_TRUE;
    MUTEX_UNLOCK(&pstSessCb->mutex);

HCP_WORKER_EXIT:
    LOG_EXIT_RET(NULL);
}

static uint32_t u32VppIpHcp_GetCmdPktSz()
{
    uint32_t u32CmdPkt = 0;

    u32CmdPkt = HCP_MAX(u32CmdPkt, sizeof(t_StHcpHfiCmdSetProp)); // CMD_ID_SET_PROPERTY
    u32CmdPkt = HCP_MAX(u32CmdPkt, sizeof(t_StHcpHfiCmdGetProp)); // CMD_ID_GET_PROPERTY
    u32CmdPkt += sizeof(t_StHcpHfiCmdPktHdr);

#if 0
    LOGI("sizeof(t_StHcpHfiCmdSetProp) = %zu", sizeof(t_StHcpHfiCmdSetProp));
    LOGI("sizeof(t_StHcpHfiCmdGetProp) = %zu", sizeof(t_StHcpHfiCmdGetProp));
    LOGI("sizeof(t_StHcpHfiCmdPktHdr) = %zu", sizeof(t_StHcpHfiCmdPktHdr));
    LOGI("total size required for command packet: %u", u32CmdPkt);
#endif

    return u32CmdPkt;
}

static uint32_t u32VppIpHcp_AllocGlobalStructs(t_StVppIpHcpGlobalStructs *pst)
{
    VPP_RET_IF_NULL(pst, VPP_ERR_PARAM);

    // Allocate timestamp string
    pst->pstTs = HCP_ALLOC(sizeof(hcp_rpc_ts_strings_t));
    if (!pst->pstTs)
    {
        LOGE("unable to allocate pstTs");
        goto ERR_ALLOC_TS;
    }

    pst->pstTs->dyn_img_ts = HCP_ALLOC(HCP_DYNAMIC_IMG_TS_SZ);
    pst->pstTs->dyn_img_tsLen = HCP_DYNAMIC_IMG_TS_SZ;
    HCP_MEM_ERR_GOTO(pst->pstTs->dyn_img_ts, ERR_ALLOC_DYN_TS);

    pst->pstTs->static_img_ts = HCP_ALLOC(HCP_STATIC_IMG_TS_SZ);
    pst->pstTs->static_img_tsLen = HCP_STATIC_IMG_TS_SZ;
    HCP_MEM_ERR_GOTO(pst->pstTs->static_img_ts, ERR_ALLOC_STATIC_TS);

    // Allocate global raw response packet
    pst->pstRawRspPkt = HCP_ALLOC(sizeof(hcp_rpc_pkt_t));
    HCP_MEM_ERR_GOTO(pst->pstRawRspPkt, ERR_ALLOC_PST_RSP_PKT);

    // Allocate global raw command packet
    pst->pstRawCmdPkt = HCP_ALLOC(sizeof(hcp_rpc_pkt_t));
    HCP_MEM_ERR_GOTO(pst->pstRawCmdPkt, ERR_ALLOC_PST_CMD_PKT);

    // Allocate message queue nodes that are shared with fw and sessions
    pst->pstMsgQNodes = HCP_ALLOC(sizeof(t_StVppIpHcpMsgQNode) * HCP_MSG_Q_NODES);
    HCP_MEM_ERR_GOTO(pst->pstMsgQNodes, ERR_ALLOC_MSG_PKT);

    // Allocate command packet structure
    pst->u32CmdPktAllocSz = u32VppIpHcp_GetCmdPktSz();
    if (pst->u32CmdPktAllocSz)
        pst->pstCmdPkt = HCP_ALLOC(pst->u32CmdPktAllocSz);
    if (!pst->pstCmdPkt)
        goto ERR_ALLOC_CMD_PKT;

    // Allocate buffer attribute structure
    pst->pstBufAttr = HCP_ALLOC(sizeof(t_StHcpHfiBufAttr));
    if (!pst->pstBufAttr)
        goto ERR_ALLOC_BUF_ATTR;

    return VPP_OK;

ERR:
    HCP_FREE(pst->pstBufAttr);

ERR_ALLOC_BUF_ATTR:
    HCP_FREE(pst->pstCmdPkt);

ERR_ALLOC_CMD_PKT:
    HCP_FREE(pst->pstMsgQNodes);

ERR_ALLOC_MSG_PKT:
    HCP_FREE(pst->pstRawCmdPkt);

ERR_ALLOC_PST_CMD_PKT:
    HCP_FREE(pst->pstRawRspPkt);

ERR_ALLOC_PST_RSP_PKT:
    HCP_FREE(pst->pstTs->static_img_ts);

ERR_ALLOC_STATIC_TS:
    HCP_FREE(pst->pstTs->dyn_img_ts);

ERR_ALLOC_DYN_TS:
    HCP_FREE(pst->pstTs);

ERR_ALLOC_TS:
    memset(pst, 0, sizeof(t_StVppIpHcpGlobalStructs));
    return VPP_ERR_NO_MEM;
}

static uint32_t u32VppIpHcp_FreeGlobalStructs(t_StVppIpHcpGlobalStructs *pst)
{
    VPP_RET_IF_NULL(pst, VPP_ERR_PARAM);

    HCP_FREE(pst->pstBufAttr);
    HCP_FREE(pst->pstCmdPkt);
    HCP_FREE(pst->pstMsgQNodes);
    HCP_FREE(pst->pstRawCmdPkt);
    HCP_FREE(pst->pstRawRspPkt);

    if (pst->pstTs)
    {
        HCP_FREE(pst->pstTs->static_img_ts);
        HCP_FREE(pst->pstTs->dyn_img_ts);
        HCP_FREE(pst->pstTs);
    }

    memset(pst, 0, sizeof(t_StVppIpHcpGlobalStructs));
    return VPP_OK;
}

static uint32_t u32VppIpHcp_AllocSessionStructs(t_StVppIpHcpSessionStructs *pst)
{
    VPP_RET_IF_NULL(pst, VPP_ERR_PARAM);

    memset(pst, 0, sizeof(t_StVppIpHcpSessionStructs));

    pst->pstPkt = HCP_ALLOC(sizeof(hcp_rpc_pkt_t) * HCP_RAW_PKT_PER_SESS);
    if (!pst->pstPkt)
        goto ERR_ALLOC_RAW_PKT;

    pst->u32CmdPktAllocSz = u32VppIpHcp_GetCmdPktSz();
    if (pst->u32CmdPktAllocSz)
        pst->pstCmdPkt = HCP_ALLOC(pst->u32CmdPktAllocSz);
    if (!pst->pstCmdPkt)
        goto ERR_ALLOC_CMD_PKT;

    pst->u32BufAttrAllocSz = sizeof(t_StHcpHfiBufAttr) * HCP_BUF_ATTR_PER_SESS;
    pst->pstBufAttr = HCP_ALLOC(pst->u32BufAttrAllocSz);
    if (!pst->pstBufAttr)
        goto ERR_ALLOC_BUF_ATTR;

    pst->u32RawBufAllocSz = sizeof(hcp_rpc_raw_buffer_t) * HCP_BUF_ATTR_PER_SESS;
    pst->pstRawBuf = HCP_ALLOC(pst->u32RawBufAllocSz);
    if (!pst->pstRawBuf)
        goto ERR_ALLOC_RAW_BUF;

    return VPP_OK;

ERR:
    HCP_FREE(pst->pstRawBuf);

ERR_ALLOC_RAW_BUF:
    HCP_FREE(pst->pstBufAttr);

ERR_ALLOC_BUF_ATTR:
    HCP_FREE(pst->pstCmdPkt);

ERR_ALLOC_CMD_PKT:
    HCP_FREE(pst->pstPkt);

ERR_ALLOC_RAW_PKT:
    memset(pst, 0, sizeof(t_StVppIpHcpSessionStructs));
    return VPP_ERR_NO_MEM;
}

static uint32_t u32VppIpHcp_FreeSessionStructs(t_StVppIpHcpSessionStructs *pst)
{
    VPP_RET_IF_NULL(pst, VPP_ERR_PARAM);

    HCP_FREE(pst->pstRawBuf);
    HCP_FREE(pst->pstBufAttr);
    HCP_FREE(pst->pstCmdPkt);
    HCP_FREE(pst->pstPkt);
    memset(pst, 0, sizeof(t_StVppIpHcpSessionStructs));

    return VPP_OK;
}


static uint32_t u32VppIpHcp_BootModule(t_StVppIpHcpGlobalCb *pstGlobal,
                                       uint32_t bBootLock)
{
    int rc = 0;
    uint32_t u32, i;
    uint32_t u32Ret = VPP_OK;

    LOG_ENTER();

    VPP_RET_IF_NULL(pstGlobal, VPP_ERR_PARAM);

    HCP_LOCK(pstGlobal);

    while (pstGlobal->eState == HCP_GLOBAL_STATE_INITIALIZING ||
           pstGlobal->eState == HCP_GLOBAL_STATE_TERMINATING)
    {
        LOGD("Boot() waiting for another client to finish, eState=%u",
             pstGlobal->eState);
        HCP_WAIT(pstGlobal);
    }

    if (pstGlobal->eState == HCP_GLOBAL_STATE_READY)
    {
        HCP_UNLOCK(pstGlobal);
        u32Ret = VPP_ERR_STATE;
        LOGD("Boot() called but HCP is already initialized");
        goto BOOT_DONE;
    }

    if (bBootLock)
    {
        pstGlobal->u.bBootLock = VPP_TRUE;
    }

    pstGlobal->eState = HCP_GLOBAL_STATE_INITIALIZING;

    HCP_UNLOCK(pstGlobal);

    HCP_DUMP_GLOBAL_FLAG_STATE();

    memset(&pstGlobal->stVppCtx, 0, sizeof(t_StVppCtx));
    u32 = u32VppIon_Init(&pstGlobal->stVppCtx);
    if (u32 != VPP_OK)
    {
        u32Ret = VPP_ERR_RESOURCES;
        LOGE("unable to init ion. u32=%u", u32);
        goto ERR_ION_INIT;
    }

    u32 = u32VppIpHcp_AllocGlobalStructs(&pstGlobal->st);
    if (u32 != VPP_OK)
    {
        u32Ret = VPP_ERR_NO_MEM;
        LOGE("unable to allocate hcp param structures, u32=%u", u32);
        goto ERR_ALLOC_STRUCTS;
    }

    u32VppIpHcp_MsgQInit(&pstGlobal->stMsgQ);
    for (i = 0; i < HCP_MSG_Q_NODES; i++)
    {
        u32 = u32VppIpHcp_MsgQPut(&pstGlobal->stMsgQ,
                                  &pstGlobal->st.pstMsgQNodes[i], NULL, NULL);
        LOGE_IF(u32 != VPP_OK, "unable to enqueue mesage node to message pool");
    }

    u32 = u32VppIpHcp_TuningInit(pstGlobal);
    LOGE_IF(u32 != VPP_OK, "unable to initialize tunings");

    pstGlobal->u32AsyncRes = VPP_OK;
    pstGlobal->u.bRxResult = 0;
    rc = pthread_create(&pstGlobal->threadRsp, NULL, vpVppIpHcp_ReceiverWorker, pstGlobal);
    if (rc)
    {
        LOGE("unable to create hcp receiver thread, rc=%d", rc);
        u32Ret = VPP_ERR_RESOURCES;
        goto ERR_THREAD_CREATE;
    }

    HCP_LOCK(pstGlobal);
    while (!pstGlobal->u.bRxResult)
        HCP_WAIT(pstGlobal);
    HCP_UNLOCK(pstGlobal);

    if (pstGlobal->u32AsyncRes != VPP_OK)
    {
        LOGE("start receiver worker thread failed, u32=%u", pstGlobal->u32AsyncRes);
        u32Ret = pstGlobal->u32AsyncRes;
        goto ERR_RECEIVER_THREAD;
    }

    HCP_LOCK(pstGlobal);
    pstGlobal->eState = HCP_GLOBAL_STATE_READY;
    HCP_UNLOCK(pstGlobal);

    HCP_BROADCAST(pstGlobal);

    LOGI("HCP booted successfully");

    HCP_DUMP_GLOBAL_FLAG_STATE();

BOOT_DONE:
    LOG_EXIT_RET(u32Ret);

ERR_RECEIVER_THREAD:
    rc = pthread_join(pstGlobal->threadRsp, NULL);
    LOGE_IF(rc, "pthread_join failed: %d --> %s", rc, strerror(rc));

ERR_THREAD_CREATE:
    u32 = u32VppIpHcp_TuningTerm(pstGlobal);
    LOGE_IF(u32 != VPP_OK, "unable to terminate tunings. ret=%u", u32);
    u32VppIpHcp_FreeGlobalStructs(&pstGlobal->st);

ERR_ALLOC_STRUCTS:
    u32 = u32VppIon_Term(&pstGlobal->stVppCtx);
    LOGE_IF(u32 != VPP_OK, "unable to term ion. u32=%u", u32);
    memset(&pstGlobal->stVppCtx, 0, sizeof(t_StVppCtx));

ERR_ION_INIT:
    HCP_LOCK(pstGlobal);
    pstGlobal->eState = HCP_GLOBAL_STATE_NULL;
    HCP_UNLOCK(pstGlobal);
    HCP_BROADCAST(pstGlobal);
    LOG_EXIT_RET(u32Ret);
}

static uint32_t u32VppIpHcp_ShutdownModule(t_StVppIpHcpGlobalCb *pstGlobal,
                                           uint32_t bBootLock)
{
    int rc = 0;
    uint32_t u32;
    uint32_t u32Ret = VPP_OK;

    LOG_ENTER();

    VPP_RET_IF_NULL(pstGlobal, VPP_ERR_PARAM);

    HCP_LOCK(pstGlobal);

    while (pstGlobal->eState == HCP_GLOBAL_STATE_INITIALIZING ||
           pstGlobal->eState == HCP_GLOBAL_STATE_TERMINATING)
    {
        LOGD("Shutdown() waiting for another client to finish, eState=%u",
             pstGlobal->eState);
        HCP_WAIT(pstGlobal);
    }

    if (pstGlobal->eState == HCP_GLOBAL_STATE_NULL)
    {
        u32Ret = VPP_ERR_STATE;
        HCP_UNLOCK(pstGlobal);
        LOGD("Boot() called but HCP is already shutdown");
        goto SHUTDOWN_DONE;
    }

    if (pstGlobal->u.bBootLock && !bBootLock)
    {
        u32Ret = VPP_ERR_PARAM;
        HCP_UNLOCK(pstGlobal);
        LOGD("bBootLock is set but unlock is not given");
        goto SHUTDOWN_DONE;
    }

    if (pstGlobal->sessions.u32Cnt)
    {
        u32Ret = VPP_ERR_STATE;
        LOGE("unable to shutdown: active sessions, cnt=%u", pstGlobal->sessions.u32Cnt);
        LOGE_IF(bBootLock, "ERROR: bBootLock shutdown, but clients still registered!");
        HCP_UNLOCK(pstGlobal);
        goto SHUTDOWN_DONE;
    }

    if (pstGlobal->u.bBootLock && bBootLock)
    {
        pstGlobal->u.bBootLock = VPP_FALSE;
    }

    pstGlobal->eState = HCP_GLOBAL_STATE_TERMINATING;

    pstGlobal->u.bPendShutdown = VPP_TRUE;
    HCP_UNLOCK(pstGlobal);

    u32 = u32VppIpHcp_HwGlobalSendCmdNop(pstGlobal);
    LOGE_IF(u32 != VPP_OK, "ERROR: unable to send cmd to hw, may hang..., u32=%u", u32);

    rc = pthread_join(pstGlobal->threadRsp, NULL);
    LOGE_IF(rc, "pthread_join failed: %d --> %s", rc, strerror(rc));

    u32 = u32VppIpHcp_TuningTerm(pstGlobal);
    LOGE_IF(u32 != VPP_OK, "unable to term tunings. ret=%u", u32);

    while (pstVppIpHcp_MsgQGet(&pstGlobal->stMsgQ, NULL) != NULL);
    u32VppIpHcp_MsgQTerm(&pstGlobal->stMsgQ);

    u32VppIpHcp_FreeGlobalStructs(&pstGlobal->st);

    u32 = u32VppIon_Term(&pstGlobal->stVppCtx);
    LOGE_IF(u32 != VPP_OK, "unable to term ion. u32=%u", u32);

    memset(&pstGlobal->stVppCtx, 0, sizeof(t_StVppCtx));

    HCP_LOCK(pstGlobal);
    pstGlobal->u.u32 = 0;
    pstGlobal->eState = HCP_GLOBAL_STATE_NULL;
    HCP_UNLOCK(pstGlobal);
    HCP_BROADCAST(pstGlobal);

SHUTDOWN_DONE:
    LOG_EXIT_RET(u32Ret);
}

/************************************************************************
 * Global Functions
 ***********************************************************************/
uint32_t u32VppIpHcp_Boot()
{
    uint32_t u32 = 0;
    t_StVppIpHcpGlobalCb *pstGlobal = HCP_GET_GLOBAL();

    LOG_ENTER();

    u32 = u32VppIpHcp_BootModule(pstGlobal, VPP_TRUE);
    LOGE_IF(u32 != VPP_OK, "BootModule returned u32=%u", u32);

    LOG_EXIT_RET(u32);
}

uint32_t u32VppIpHcp_Shutdown()
{
    uint32_t u32 = 0;
    t_StVppIpHcpGlobalCb *pstGlobal = HCP_GET_GLOBAL();

    LOG_ENTER();

    u32 = u32VppIpHcp_ShutdownModule(pstGlobal, VPP_TRUE);
    LOGE_IF(u32 != VPP_OK, "ShutdownModule returned u32=%u", u32);

    LOG_EXIT_RET(u32);
}

void * vpVppIpHcp_Init(t_StVppCtx *pstCtx, uint32_t u32Flags, t_StVppCallback cbs)
{
    LOG_ENTER_ARGS("flags=0x%x", u32Flags);

    int rc;
    uint32_t u32;
    t_StVppIpHcpSessionCb *pstSessCb;
    t_StVppIpHcpGlobalCb *pstGlobal = HCP_GET_GLOBAL();
    t_StVppIpCmd stCmd;

    VPP_RET_IF_NULL(pstCtx, NULL);

    if (u32VppIpHcp_ValidateCallbacks(&cbs) != VPP_OK)
        LOG_EXIT_RET(NULL);

    u32 = u32VppIpHcp_BootModule(pstGlobal, VPP_FALSE);
    if (u32 != VPP_OK && u32 != VPP_ERR_STATE)
    {
        LOGE("BootModule returned u32=%u", u32);
        goto ERROR_BOOT_MODULE;
    }

    pstSessCb = calloc(sizeof(t_StVppIpHcpSessionCb), 1);
    if (!pstSessCb)
    {
        LOGE("calloc failed for hcp context");
        goto ERROR_MALLOC;
    }
    LOGD("allocated hcp context for session, pstSessCb=%p", pstSessCb);

    pstSessCb->stDrvCtx.id = SESSION_ID_INVALID;

    u32 = u32VppIpHcp_AllocSessionStructs(&pstSessCb->st);
    if (u32 != VPP_OK)
    {
        LOGE("unable to allocate session structs, u32=%u", u32);
        goto ERROR_ALLOC_SESS_STRUCTS;
    }

    u32 = u32VppIpHcp_RegisterSession(pstGlobal, pstSessCb);
    if (u32 != VPP_OK)
    {
        LOGE("unable to register session, u32=%u", u32);
        goto ERROR_REGISTER_SESSION;
    }

    u32VppIp_SetBase(pstCtx, u32Flags, cbs, &pstSessCb->stBase);

    u32 = VPP_IP_PROF_REGISTER_BY_NM(&pstSessCb->stBase, HcpStats);
    LOGE_IF(u32 != VPP_OK, "ERROR: unable to register stats, u32=%u", u32);

    if((u32 = u32VppIpHcp_MsgQInit(&pstSessCb->stMsgQ)) != VPP_OK)
    {
        LOGE("unable to initialize message queue, u32=%u", u32);
        goto ERR_MSG_Q_INIT;
    }

    if (u32VppIp_PortInit(&pstSessCb->stInput) != VPP_OK)
    {
        LOGE("unable to u32VppBufPool_Init() input queue");
        goto ERROR_PENDING_INPUT_Q_INIT;
    }

    if (u32VppIp_PortInit(&pstSessCb->stOutput) != VPP_OK)
    {
        LOGE("unable to u32VppBufPool_Init() output queue");
        goto ERROR_PENDING_OUTPUT_Q_INIT;
    }

    if (u32VppIp_PortInit(&pstSessCb->stFwIn) != VPP_OK)
    {
        LOGE("unable to u32VppBufPool_Init() firmware input queue");
        goto ERROR_PENDING_FW_INPUT_Q_INIT;
    }

    if (u32VppIp_PortInit(&pstSessCb->stFwOut) != VPP_OK)
    {
        LOGE("unable to u32VppBufPool_Init() firmware output queue");
        goto ERROR_PENDING_FW_OUTPUT_Q_INIT;
    }

    if (u32VppIp_NtsCmdQueueInit(&pstSessCb->stCmdQ, HCP_CMD_Q_SZ) != VPP_OK)
    {
        LOGE("unable to initialize command queue");
        goto ERROR_CMD_QUEUE_INIT;
    }

    rc = pthread_mutex_init(&pstSessCb->mutex, NULL);
    if (rc)
    {
        LOGE("unable to initialize hcp mutex");
        goto ERROR_MUTEX_INIT;
    }

    rc = pthread_cond_init(&pstSessCb->cond, NULL);
    if (rc)
    {
        LOGE("unable to init condition variable");
        goto ERROR_COND_INIT;
    }

    rc = pthread_cond_init(&pstSessCb->condToClient, NULL);
    if (rc)
    {
        LOGE("unable to init condition variable");
        goto ERROR_COND_TO_CLIENT_INIT;
    }

    vVppIpHcp_InitDefaultParams(pstSessCb);
    vVppIpHcp_InitStaticParams(pstSessCb);
    vVppIpHcp_ReadSessProps(pstSessCb);

    stCmd.eCmd = VPP_IP_CMD_INIT;
    u32 = u32VppIp_NtsCmdPut(&pstSessCb->stCmdQ, stCmd, NULL, NULL);
    if (u32 != VPP_OK)
    {
        LOGE("unable to insert init cmd, u32=%u", u32);
        goto ERR_INSERT_INIT;
    }

    rc = pthread_create(&pstSessCb->thread, NULL, vpVppIpHcp_SessionWorker, pstSessCb);
    if (rc)
    {
        LOGE("unable to spawn hcp worker thread");
        goto ERROR_THREAD_CREATE;
    }

    // Wait for the thread to launch before returning
    vVppIpHcp_WaitWorkerStart(pstSessCb);

    if (pstSessCb->u32WorkerErr != VPP_OK)
    {
        LOGE("Worker initialization error, err=%u", pstSessCb->u32WorkerErr);
        goto ERROR_WORKER_START;
    }

    VPP_IP_STATE_SET(pstSessCb, VPP_IP_STATE_INITED);

    LOG_EXIT_RET(pstSessCb);

ERROR_WORKER_START:
    rc = pthread_join(pstSessCb->thread, NULL);
    if (rc)
        LOGE("pthread_join failed: %d --> %s", rc, strerror(rc));

ERROR_THREAD_CREATE:
ERR_INSERT_INIT:
    LOGI("destroying condToClient condition variable");
    pthread_cond_destroy(&pstSessCb->condToClient);

ERROR_COND_TO_CLIENT_INIT:
    LOGI("destroying cond condition variable");
    pthread_cond_destroy(&pstSessCb->cond);

ERROR_COND_INIT:
    LOGI("destroying mutex");
    pthread_mutex_destroy(&pstSessCb->mutex);

ERROR_MUTEX_INIT:
    LOGI("destroying command queue");
    u32VppIp_NtsCmdQueueTerm(&pstSessCb->stCmdQ);

ERROR_CMD_QUEUE_INIT:
    u32VppIp_PortTerm(&pstSessCb->stFwOut);

ERROR_PENDING_FW_OUTPUT_Q_INIT:
    u32VppIp_PortTerm(&pstSessCb->stFwIn);

ERROR_PENDING_FW_INPUT_Q_INIT:
    u32VppIp_PortTerm(&pstSessCb->stOutput);

ERROR_PENDING_OUTPUT_Q_INIT:
    u32VppIp_PortTerm(&pstSessCb->stInput);

ERROR_PENDING_INPUT_Q_INIT:
    u32 = u32VppIpHcp_MsgQTerm(&pstSessCb->stMsgQ);
    LOGE_IF(u32 != VPP_OK, "ERROR: unable to term messge queue, u32=%u", u32);

ERR_MSG_Q_INIT:
    u32 = VPP_IP_PROF_UNREGISTER(&pstSessCb->stBase);
    LOGE_IF(u32 != VPP_OK, "ERROR: unable to unregister stats, u32=%u", u32);

    u32 = u32VppIpHcp_UnregisterSession(pstGlobal, pstSessCb);
    LOGE_IF(u32 != VPP_OK, "ERROR: unable to unregister session, u32=%u", u32);

ERROR_REGISTER_SESSION:
    u32 = u32VppIpHcp_FreeSessionStructs(&pstSessCb->st);
    LOGE_IF(u32 != VPP_OK, "ERROR, unable to free sesison structs, u32=%u", u32);

ERROR_ALLOC_SESS_STRUCTS:
    free(pstSessCb);

ERROR_MALLOC:
    u32 = u32VppIpHcp_ShutdownModule(pstGlobal, VPP_FALSE);
    LOGE_IF(u32 != VPP_OK, "ERROR, unable to shutdown module, u32=%u", u32);

ERROR_BOOT_MODULE:
    LOG_EXIT_RET(NULL);
}

void vVppIpHcp_Term(void *ctx)
{
    int rc;
    uint32_t u32;
    t_StVppIpHcpSessionCb *pstSessCb;
    t_StVppIpCmd stCmd;
    t_StVppIpHcpGlobalCb *pstGlobal = HCP_GET_GLOBAL();

    LOG_ENTER();

    pstSessCb = HCP_CB_GET(ctx);
    VPP_RET_VOID_IF_NULL(pstSessCb);

    stCmd.eCmd = VPP_IP_CMD_TERM;
    u32 = u32VppIp_NtsCmdPut(&pstSessCb->stCmdQ, stCmd,
                             &pstSessCb->mutex, &pstSessCb->cond);
    LOGE_IF(u32 != VPP_OK, "unable to queue TERM, u32=%u", u32);

    stCmd.eCmd = VPP_IP_CMD_THREAD_EXIT;
    u32 = u32VppIp_NtsCmdPut(&pstSessCb->stCmdQ, stCmd,
                             &pstSessCb->mutex, &pstSessCb->cond);
    if (u32 != VPP_OK)
    {
        LOGE("unable to queue THREAD_EXIT, u32=%u, not joining worker", u32);
    }
    else
    {
        rc = pthread_join(pstSessCb->thread, NULL);
        LOGE_IF(rc, "pthread_join failed: %d --> %s", rc, strerror(rc));
    }

    u32 = u32VppIpHcp_FlushPort(pstSessCb, VPP_PORT_INPUT, &pstSessCb->stInput);
    LOGE_IF(u32 != VPP_OK, "FlushPort input failed, u32=%u", u32);

    u32 = u32VppIpHcp_FlushPort(pstSessCb, VPP_PORT_OUTPUT, &pstSessCb->stOutput);
    LOGE_IF(u32 != VPP_OK, "FlushPort output failed, u32=%u", u32);

    u32 = u32VppIpHcp_FlushPort(pstSessCb, VPP_PORT_INPUT, &pstSessCb->stFwIn);
    LOGE_IF(u32 != VPP_OK, "FlushPort fw input failed, u32=%u", u32);

    u32 = u32VppIpHcp_FlushPort(pstSessCb, VPP_PORT_OUTPUT, &pstSessCb->stFwOut);
    LOGE_IF(u32 != VPP_OK, "FlushPort fw output failed, u32=%u", u32);

    rc = pthread_cond_destroy(&pstSessCb->condToClient);
    LOGE_IF(rc, "pthread_cond_destroy (condToClient) failed: %d --> %s",
            rc, strerror(rc));

    rc = pthread_cond_destroy(&pstSessCb->cond);
    LOGE_IF(rc, "pthread_cond_destroy (cond) failed: %d --> %s",
            rc, strerror(rc));

    rc = pthread_mutex_destroy(&pstSessCb->mutex);
    LOGE_IF(rc, "pthread_mutex_destroy failed: %d --> %s", rc, strerror(rc));

    u32 = u32VppIp_NtsCmdQueueTerm(&pstSessCb->stCmdQ);
    LOGE_IF(u32 != VPP_OK, "CmdQueueTerm failed, u32=%u", u32);

    u32 = u32VppIp_PortTerm(&pstSessCb->stFwOut);
    LOGE_IF(u32 != VPP_OK, "PortTerm, fw output failed, u32=%u", u32);

    u32 = u32VppIp_PortTerm(&pstSessCb->stFwIn);
    LOGE_IF(u32 != VPP_OK, "PortTerm, fw input failed, u32=%u", u32);

    u32 = u32VppIp_PortTerm(&pstSessCb->stOutput);
    LOGE_IF(u32 != VPP_OK, "PortTerm, output failed, u32=%u", u32);

    u32 = u32VppIp_PortTerm(&pstSessCb->stInput);
    LOGE_IF(u32 != VPP_OK, "PortTerm, input failed, u32=%u", u32);

    u32 = u32VppIpHcp_MsgQTerm(&pstSessCb->stMsgQ);
    LOGE_IF(u32 != VPP_OK, "ERROR: unable to term messge queue, u32=%u", u32);

    u32 = VPP_IP_PROF_UNREGISTER(&pstSessCb->stBase);
    LOGE_IF(u32 != VPP_OK, "ERROR: unable to unregister stats, u32=%u", u32);

    u32 = u32VppIpHcp_UnregisterSession(pstGlobal, pstSessCb);
    LOGE_IF(u32 != VPP_OK, "ERROR: unable to unregister session, u32=%u", u32);

    u32 = u32VppIpHcp_FreeSessionStructs(&pstSessCb->st);
    LOGE_IF(u32 != VPP_OK, "ERROR, unable to free sesison structs, u32=%u", u32);

    free(pstSessCb);

    u32VppIpHcp_ShutdownModule(pstGlobal, VPP_FALSE);

    LOG_EXIT();
}

uint32_t u32VppIpHcp_Open(void *ctx)
{
    LOG_ENTER();

    uint32_t u32;
    t_StVppIpCmd stCmd;
    t_StVppIpHcpSessionCb *pstSessCb;

    pstSessCb = HCP_CB_GET(ctx);
    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);

    if (!VPP_IP_STATE_EQUAL(pstSessCb, VPP_IP_STATE_INITED))
        return VPP_ERR_STATE;

    if (pstSessCb->flags.bOpened)
        return VPP_ERR_PARAM;

    pstSessCb->flags.bOpenSignal = VPP_FALSE;

    stCmd.eCmd = VPP_IP_CMD_OPEN;
    u32 = u32VppIp_NtsCmdPut(&pstSessCb->stCmdQ, stCmd,
                             &pstSessCb->mutex, &pstSessCb->cond);

    if (u32 != VPP_OK)
    {
        LOGE("unable to queue OPEN, u32=%u", u32);
        return u32;
    }

    HCP_LOCK(pstSessCb);
    while (!pstSessCb->flags.bOpenSignal)
        vVppIpHcp_WaitClient(pstSessCb);
    HCP_UNLOCK(pstSessCb);
    LOGI("async open returned %u", pstSessCb->async_res.u32OpenRet);
    LOG_EXIT_RET(pstSessCb->async_res.u32OpenRet);
}

uint32_t u32VppIpHcp_Close(void *ctx)
{
    LOG_ENTER();

    t_StVppIpHcpSessionCb *pstSessCb;
    t_StVppIpCmd stCmd;
    uint32_t u32;

    pstSessCb = HCP_CB_GET(ctx);
    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);

    if (!VPP_IP_STATE_EQUAL(pstSessCb, VPP_IP_STATE_ACTIVE))
        return VPP_ERR_STATE;

    stCmd.eCmd = VPP_IP_CMD_CLOSE;
    u32 = u32VppIp_NtsCmdPut(&pstSessCb->stCmdQ, stCmd,
                             &pstSessCb->mutex, &pstSessCb->cond);

    if (u32 != VPP_OK)
    {
        LOGE("unable to queue CLOSE, u32=%u", u32);
        return u32;
    }

    HCP_LOCK(pstSessCb);
    while (pstSessCb->flags.bOpened)
        vVppIpHcp_WaitClient(pstSessCb);
    HCP_UNLOCK(pstSessCb);

    LOGI("async close returned %u", pstSessCb->async_res.u32CloseRet);
    LOG_EXIT_RET(pstSessCb->async_res.u32CloseRet);
}

uint32_t u32VppIpHcp_SetParam(void *ctx, enum vpp_port ePort,
                              struct vpp_port_param stParam)
{
    t_StVppIpHcpSessionCb *pstSessCb;
    uint32_t u32, u32Ret = VPP_OK;

    LOG_ENTER();

    pstSessCb = HCP_CB_GET(ctx);

    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);

    if (!VPP_IP_STATE_EQUAL(pstSessCb, VPP_IP_STATE_INITED))
        return VPP_ERR_STATE;

    if (ePort == VPP_PORT_INPUT)
    {
        // Can not validate output port here because if the block is in
        // measurement mode, the output port does not need to have w/h/str/scl
        // set. So, if it is in proc mode, open will be used to validate that
        // the configuration is correct.
        u32 = u32VppIpHcp_ValidatePort(ePort, stParam);
        if (u32 != VPP_OK)
        {
            return VPP_ERR_PARAM;
        }
    }

    HCP_LOCK(pstSessCb);

    if (ePort == VPP_PORT_INPUT)
        pstSessCb->stInput.stParam = stParam;
    else if (ePort == VPP_PORT_OUTPUT)
        pstSessCb->stOutput.stParam = stParam;
    else
        u32Ret = VPP_ERR_PARAM;

    VPP_FLAG_SET(pstSessCb->stCfg.u32ComputeMask, HCP_FLAG_PORT_PARAM);
    HCP_UNLOCK(pstSessCb);
    LOG_EXIT_RET(u32Ret);
}

uint32_t u32VppIpHcp_SetCtrl(void *ctx, struct hqv_control ctrl)
{
    uint32_t u32, u32Ret = VPP_OK;
    uint32_t bWouldChangeIp = VPP_FALSE;
    uint32_t bWouldChangeFw = VPP_FALSE;
    uint32_t bIsGlobalDemo = VPP_FALSE;
    uint32_t u32EnMask;

    t_StVppIpCmd stCmd;
    t_StVppIpHcpSessionCb *pstSessCb;
    t_StVppIpHcpCfg *pstCfg;
    t_EVppIpHcpOpMode eOpModeNew;

    LOG_ENTER();

    pstSessCb = HCP_CB_GET(ctx);
    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);

    HCP_LOCK(pstSessCb);

    if (pstSessCb->flags.bOpModePending ||
        pstSessCb->flags.bResetFwPending)
    {
        u32Ret = VPP_ERR_STATE;
        LOGE("ERROR: SetCtrl called while another is pending, "
             "bOpModePending=%u, bResetFwPending=%u",
             pstSessCb->flags.bOpModePending, pstSessCb->flags.bResetFwPending);
    }

    if (!VPP_IP_STATE_EQUAL(pstSessCb, VPP_IP_STATE_INITED) &&
        !VPP_IP_STATE_EQUAL(pstSessCb, VPP_IP_STATE_ACTIVE))
    {
        u32Ret = VPP_ERR_STATE;
        LOGE("ERROR: SetCtrl called in incorrect state, eState=%u",
             pstSessCb->eState);
    }

    u32 = u32VppIpHcp_ValidateCtrl(ctrl);
    if (u32 != VPP_OK)
    {
        LOGE("ERROR: ValidateCtrl() failed, u32=%u", u32);
        u32Ret = VPP_ERR_PARAM;
    }

    LOGD("SetCtrl: ctrl.mode=%u, ctrl.type=%u", ctrl.mode, ctrl.ctrl_type);

    pstCfg = &pstSessCb->stCfg;
    u32 = u32VppIpHcp_GetNewEnableMask(pstSessCb->stCfg, ctrl,
                                       pstSessCb->eState, &u32EnMask);
    if (u32 != VPP_OK)
    {
        u32Ret = VPP_ERR_PARAM;
        LOGE("ERROR: SetCtrl passed invalid ctrl, u32=%u", u32);
    }

    if (u32Ret != VPP_OK)
    {
        HCP_UNLOCK(pstSessCb);
        LOG_EXIT_RET(u32Ret);
    }

    eOpModeNew = eVppIpHcp_GetOpModeForEnMsk(u32EnMask);
    bWouldChangeIp = (pstSessCb->stCfg.eOpMode != eOpModeNew);
    bWouldChangeFw = (eOpModeNew != HCP_OP_MODE_BYPASS &&
                      eOpModeNew != pstSessCb->stDrvCtx.eOpModeActive);

    if (VPP_IP_STATE_EQUAL(pstSessCb, VPP_IP_STATE_ACTIVE) && bWouldChangeIp)
    {
        LOGE("ERROR: unable to set control that would change ip op mode!");
        HCP_UNLOCK(pstSessCb);
        LOG_EXIT_RET(VPP_ERR_PARAM);
    }

    pstCfg->u32EnableMask = u32EnMask;

    if (ctrl.mode == HQV_MODE_MANUAL)
    {
        if (ctrl.ctrl_type == HQV_CONTROL_AIE)
        {
            pstCfg->stAie = ctrl.aie;
        }
        else if (ctrl.ctrl_type == HQV_CONTROL_EAR)
        {
            pstCfg->stEar = ctrl.ear;
        }
        else if (ctrl.ctrl_type == HQV_CONTROL_MEAS)
        {
            pstCfg->stMeas = ctrl.meas;
        }
        else if (ctrl.ctrl_type == HQV_CONTROL_GLOBAL_DEMO)
        {
            bIsGlobalDemo = VPP_TRUE;
            pstCfg->stDemo = ctrl.demo;
            VPP_FLAG_SET(pstCfg->u32ComputeMask, HCP_FLAG_ALGO_DEMO);
        }
    }

    if (!bIsGlobalDemo)
    {
        VPP_FLAG_SET(pstCfg->u32ComputeMask, HCP_FLAG_ALGO_MEAS);
        VPP_FLAG_SET(pstCfg->u32ComputeMask, HCP_FLAG_ALGO_PROC);
    }

    if (VPP_IP_STATE_EQUAL(pstSessCb, VPP_IP_STATE_INITED) && bWouldChangeFw)
    {
        pstSessCb->async_res.u32ResetFwRet = VPP_OK;
        pstSessCb->async_res.u32OpModeRet = VPP_OK;

        if (pstSessCb->stDrvCtx.eHwState == HCP_HW_STATE_RDY)
        {
            // reset hardware
            stCmd.eCmd = (t_EVppIpCmd)VPP_IP_CMD_RESET_FW;
            u32VppIp_NtsCmdPut(&pstSessCb->stCmdQ, stCmd,
                               NULL, &pstSessCb->cond);

            pstSessCb->flags.bResetFwPending = VPP_TRUE;
        }

        // set op mode
        stCmd.eCmd = (t_EVppIpCmd)VPP_IP_CMD_SET_OP_MODE;
        u32VppIp_NtsCmdPut(&pstSessCb->stCmdQ, stCmd,
                           NULL, &pstSessCb->cond);
        pstSessCb->flags.bOpModePending = VPP_TRUE;

        // mutex already locked
        while (pstSessCb->flags.bOpModePending ||
               pstSessCb->flags.bResetFwPending)
            vVppIpHcp_WaitClient(pstSessCb);

        if (pstSessCb->async_res.u32ResetFwRet ||
            pstSessCb->async_res.u32OpModeRet)
        {
            u32Ret = VPP_ERR_HW;
            LOGE("u32ResetFwRet=%u, u32OpModeRet=%u",
                 pstSessCb->async_res.u32ResetFwRet,
                 pstSessCb->async_res.u32OpModeRet);
        }
    }

    HCP_UNLOCK(pstSessCb);
    LOG_EXIT_RET(u32Ret);
}

uint32_t u32VppIpHcp_ResetCtrl(void *ctx)
{
    t_StVppIpHcpSessionCb *pstSessCb;

    LOG_ENTER();

    pstSessCb = HCP_CB_GET(ctx);
    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);

    HCP_LOCK(pstSessCb);
    if (!VPP_IP_STATE_EQUAL(pstSessCb, VPP_IP_STATE_INITED))
    {
        LOGE("ResetCtrl called in invalid state");
        HCP_UNLOCK(pstSessCb);
        LOG_EXIT_RET(VPP_ERR_STATE);
    }

    vVppIpHcp_InitDefaultCtrls(pstSessCb);

    HCP_UNLOCK(pstSessCb);
    return VPP_OK;
}

uint32_t u32VppIpHcp_GetBufferRequirements(void *ctx,
                                           t_StVppIpBufReq *pstInputBufReq,
                                           t_StVppIpBufReq *pstOutputBufReq)
{
    t_StVppIpHcpSessionCb *pstSessCb;

    LOG_ENTER();

    pstSessCb = HCP_CB_GET(ctx);

    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);

    if (!VPP_IP_STATE_EQUAL(pstSessCb, VPP_IP_STATE_INITED) &&
        !VPP_IP_STATE_EQUAL(pstSessCb, VPP_IP_STATE_ACTIVE))
        LOG_EXIT_RET(VPP_ERR_STATE);

    vVppIpHcp_Compute(pstSessCb, VPP_FALSE);

    if (pstInputBufReq)
        memcpy(pstInputBufReq, &pstSessCb->stBufReqIn, sizeof(t_StVppIpBufReq));

    if (pstOutputBufReq)
        memcpy(pstOutputBufReq, &pstSessCb->stBufReqOut, sizeof(t_StVppIpBufReq));

    return VPP_OK;
}

#define VPP_QBUF_LOG        "u32=%u, pstSessCb=%p, ePort=%u, pBuf=%p, ts=%"PRIu64
#define VPP_QBUF_LOG_ARGS   u32Ret, pstSessCb, ePort, pBuf, u64VppBuf_GetTs(pBuf)
uint32_t u32VppIpHcp_QueueBuf(void *ctx, enum vpp_port ePort,
                              t_StVppBuf *pBuf)
{
    uint32_t u32Ret = VPP_OK;
    uint32_t u32Idx = 0;
    t_StVppIpHcpSessionCb *pstSessCb;

    LOG_ENTER();

    pstSessCb = HCP_CB_GET(ctx);

    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pBuf, VPP_ERR_PARAM);

    HCP_LOCK(pstSessCb);

    if (ePort != VPP_PORT_INPUT && ePort != VPP_PORT_OUTPUT)
    {
        u32Ret = VPP_ERR_PARAM;
        LOGE("QueueBuf failed, invalid port, ePort=%u", ePort);
    }

    if (!VPP_IP_STATE_EQUAL(pstSessCb, VPP_IP_STATE_ACTIVE))
    {
        u32Ret = VPP_ERR_STATE;
        LOGE("QueueBuf failed, invalid state, eState=%u", pstSessCb->eState);
    }

    if (pstSessCb->eExecutingCmd == VPP_IP_CMD_DRAIN && ePort == VPP_PORT_INPUT)
    {
        u32Ret = VPP_ERR_STATE;
        LOGE("QueueBuf failed, input buffer while drain pending");
    }

    HCP_UNLOCK(pstSessCb);

    if (u32Ret != VPP_OK)
        LOG_EXIT_RET(u32Ret);

    VPP_FLAG_CLR(pBuf->u32InternalFlags, VPP_BUF_FLAG_IP_MASK);

    if (ePort == VPP_PORT_INPUT)
    {
        HCP_STAT(pstSessCb, InQCnt);
        u32Idx = HCP_STAT_GET(pstSessCb, InQCnt);

        pBuf->u32Idx = u32Idx;
        u32Ret = u32VppIp_PortBufPut(&pstSessCb->stInput, pBuf,
                                     &pstSessCb->mutex, &pstSessCb->cond);
        if (u32Ret != VPP_OK)
            HCP_STAT(pstSessCb, InQCntFailed);
    }
    else
    {
        HCP_STAT(pstSessCb, OutQCnt);
        u32Idx = HCP_STAT_GET(pstSessCb, OutQCnt);

        pBuf->u32Idx = u32Idx;
        u32Ret = u32VppIp_PortBufPut(&pstSessCb->stOutput, pBuf,
                                     &pstSessCb->mutex, &pstSessCb->cond);
        if (u32Ret != VPP_OK)
            HCP_STAT(pstSessCb, OutQCntFailed);
    }

    if (u32Ret != VPP_OK)
        LOGE("QueueBuf failed, " VPP_QBUF_LOG, VPP_QBUF_LOG_ARGS);
    else
        LOGI("QueueBuf, " VPP_QBUF_LOG, VPP_QBUF_LOG_ARGS);

    LOG_EXIT_RET(u32Ret);
}

uint32_t u32VppIpHcp_Flush(void *ctx, enum vpp_port ePort)
{
    uint32_t u32Ret = VPP_OK;
    t_StVppIpCmd stCmd;
    t_StVppIpHcpSessionCb *pstSessCb;

    LOG_ENTER();

    pstSessCb = HCP_CB_GET(ctx);
    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);

    if (!VPP_IP_STATE_EQUAL(pstSessCb, VPP_IP_STATE_INITED) &&
        !VPP_IP_STATE_EQUAL(pstSessCb, VPP_IP_STATE_ACTIVE))
        LOG_EXIT_RET(VPP_ERR_STATE);

    if (ePort == VPP_PORT_INPUT)
        HCP_STAT(pstSessCb, InFlush);
    else if (ePort == VPP_PORT_OUTPUT)
        HCP_STAT(pstSessCb, OutFlush);
    else
        LOG_EXIT_RET(VPP_ERR_PARAM);

    stCmd.eCmd = VPP_IP_CMD_FLUSH;
    stCmd.flush.ePort = ePort;
    u32Ret = u32VppIp_NtsCmdPut(&pstSessCb->stCmdQ, stCmd,
                                &pstSessCb->mutex, &pstSessCb->cond);

    LOGE_IF(u32Ret != VPP_OK, "Flush failed, u32=%u", u32Ret);

    LOG_EXIT_RET(u32Ret);
}

uint32_t u32VppIpHcp_Drain(void *ctx)
{
    uint32_t u32Ret = VPP_OK;
    t_StVppIpCmd stCmd;
    t_StVppIpHcpSessionCb *pstSessCb;

    LOG_ENTER();

    pstSessCb = HCP_CB_GET(ctx);
    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);

    if (!VPP_IP_STATE_EQUAL(pstSessCb, VPP_IP_STATE_ACTIVE))
        LOG_EXIT_RET(VPP_ERR_STATE);

    HCP_STAT(pstSessCb, Drain);

    stCmd.eCmd = VPP_IP_CMD_DRAIN;
    u32Ret = u32VppIp_NtsCmdPut(&pstSessCb->stCmdQ, stCmd,
                                &pstSessCb->mutex, &pstSessCb->cond);

    LOGE_IF(u32Ret != VPP_OK, "Drain failed, u32=%u", u32Ret);

    LOG_EXIT_RET(u32Ret);
}

uint32_t u32VppIpHcp_Reconfigure(void *ctx,
                                 struct vpp_port_param in_param,
                                 struct vpp_port_param out_param)
{
    uint32_t u32, u32Ret = VPP_OK;
    t_StVppIpHcpSessionCb *pstSessCb;

    LOG_ENTER();

    pstSessCb = HCP_CB_GET(ctx);
    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);

    if (!VPP_IP_STATE_EQUAL(pstSessCb, VPP_IP_STATE_ACTIVE))
        LOG_EXIT_RET(VPP_ERR_STATE);

    HCP_LOCK(pstSessCb);

    LOG_PARAM_PTR(D, &in_param, &out_param);

    u32 = u32VppBufPool_Cnt(&pstSessCb->stInput.stPendingQ);
    if (u32)
    {
        LOGE("Reconfigure failed, in_q_cnt=%u", u32);
        u32Ret = VPP_ERR_STATE;
    }
    else
    {
        u32 = u32VppIpHcp_ValidateConfigRaw(pstSessCb->stCfg.eOpMode,
                                            in_param,
                                            out_param);
        if (u32 == VPP_OK)
        {
            pstSessCb->stInput.stParam = in_param;
            pstSessCb->stOutput.stParam = out_param;
            VPP_FLAG_SET(pstSessCb->stCfg.u32ComputeMask, HCP_FLAG_PORT_PARAM);
            vVppIpHcp_Compute(pstSessCb, VPP_TRUE);
        }
        else
        {
            LOGE("Reconfigure failed, validation error, u32=%u", u32);
            u32Ret = VPP_ERR_PARAM;
        }
    }

    HCP_UNLOCK(pstSessCb);

    LOG_EXIT_RET(u32Ret);
}
