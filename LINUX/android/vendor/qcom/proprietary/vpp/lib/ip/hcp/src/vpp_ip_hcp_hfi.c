/*!
 * @file vpp_ip_hcp_hfi.c
 *
 * @cr
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services
 */

#include <sys/types.h>
#include <string.h>
#include <inttypes.h>

#define VPP_LOG_TAG     VPP_LOG_MODULE_HCP_TAG
#define VPP_LOG_MODULE  VPP_LOG_MODULE_HCP
#include "vpp_dbg.h"
#include "vpp_utils.h"

#include "hcp_hfi.h"
#include "hcp_rpc.h"

#include "vpp_ip_hcp_priv.h"
#include "vpp_ip_hcp_hfi.h"
#include "vpp_ip_hcp_dbg.h"

#ifndef OMX_EXTRADATA_DOES_NOT_EXIST
#include "OMX_QCOMExtns.h"
#endif

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

static t_EHcpHfiFmts eVppIpHcp_GetFwFormat(enum vpp_color_format eFmt)
{
    VPP_RET_EQ(eFmt, VPP_COLOR_FORMAT_NV12_VENUS, FMT_NV12);
    VPP_RET_EQ(eFmt, VPP_COLOR_FORMAT_NV21_VENUS, FMT_NV21);
    VPP_RET_EQ(eFmt, VPP_COLOR_FORMAT_P010, FMT_P010);
    VPP_RET_EQ(eFmt, VPP_COLOR_FORMAT_UBWC_NV12, FMT_NV12_UBWC);
    VPP_RET_EQ(eFmt, VPP_COLOR_FORMAT_UBWC_NV21, FMT_NV21_UBWC);
    VPP_RET_EQ(eFmt, VPP_COLOR_FORMAT_UBWC_TP10, FMT_TP10_UBWC);
    return FMT_INVALID;
}

static uint32_t u32VppIpHcp_HwGetInfo(remote_handle64 hndl,
                                      hcp_rpc_ts_strings_t *pstStr,
                                      hcp_rpc_lock_action_t eAction,
                                      uint32_t bPrintMsg)
{
    AEEResult res;
    uint32_t u32Ret = VPP_OK;

    VPP_RET_IF_NULL(pstStr, VPP_ERR_PARAM);
    if (hndl == HCP_HNDL_UNINIT)
    {
        LOGE("HwGetInfo: received invalid handle");
        return VPP_ERR_PARAM;
    }

    res = hcp_rpc_get_info(hndl, pstStr, eAction);
    if (res == AEE_SUCCESS)
    {
        if (bPrintMsg)
            LOGD("HwGetInfo: dyn: (%s), stat: (%s)",
                 pstStr->dyn_img_ts, pstStr->static_img_ts);
    }
    else
    {
        u32Ret = VPP_ERR_HW;
        LOGE("unable to hw get_info, res=%d", res);
    }

    return u32Ret;
}

static uint32_t u32VppIpHcp_HwSendPkt(remote_handle64 hndl,
                                      hcp_rpc_pkt_t *pstPkt)
{
    AEEResult res;
    uint32_t u32Ret = VPP_OK;

    VPP_RET_IF_NULL(pstPkt, VPP_ERR_PARAM);
    if (hndl == HCP_HNDL_UNINIT)
    {
        LOGE("HwSendPkt: received invalid handle");
        return VPP_ERR_PARAM;
    }

    LOGD("HwSendPkt: content=%p, contentLen=%u",
         pstPkt->content, pstPkt->contentLen);

    res = hcp_rpc_send_pkt(hndl, pstPkt);
    if (res == AEE_SUCCESS)
    {
        LOGI("sent packet to hw successfully, res=%d", res);
    }
    else
    {
        u32Ret = VPP_ERR_HW;
        LOGE("unable to send packet to hw, res=%d", res);
    }

    return u32Ret;
}

static uint32_t u32VppIpHcp_HwSendCmd(remote_handle64 hndl,
                                      hcp_rpc_pkt_t *pstPkt,
                                      t_StHcpHfiCmdPkt *pstCmdPkt)
{
    pstPkt->content = (unsigned char *)pstCmdPkt;
    pstPkt->contentLen = pstCmdPkt->stHdr.u32Size;

    LOGD("HwSendCmd: eCmdId=%u (%s), s32FwId=%d, u32Sz=%u",
         pstCmdPkt->stHdr.eCmdId, pcGetFwCmdStr(pstCmdPkt->stHdr.eCmdId),
         pstCmdPkt->stHdr.s32SessId, pstCmdPkt->stHdr.u32Size);

    return u32VppIpHcp_HwSendPkt(hndl, pstPkt);
}

static uint32_t u32VppIpHcp_HwSendCmdNop(remote_handle64 hndl,
                                         hcp_rpc_pkt_t *pstPkt,
                                         t_StHcpHfiCmdPkt *pstCmdPkt)
{
    VPP_RET_IF_NULL(pstPkt, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pstCmdPkt, VPP_ERR_PARAM);

    pstCmdPkt->stHdr.eCmdId = CMD_ID_NOOP;
    pstCmdPkt->stHdr.s32SessId = SESSION_ID_INVALID;
    pstCmdPkt->stHdr.u32Size = sizeof(t_StHcpHfiCmdPktHdr);

    return u32VppIpHcp_HwSendCmd(hndl, pstPkt, pstCmdPkt);
}

static uint32_t u32VppIpHcp_HwSessSetProp(t_StVppIpHcpSessionCb *pstSessCb)
{
    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);

    t_StHcpHfiCmdSetProp *pstProp =
        (t_StHcpHfiCmdSetProp *)HCP_SESS_CMD_BDY(pstSessCb);
    t_StHcpHfiCmdPktHdr *pstHdr = HCP_SESS_CMD_HDR(pstSessCb);

    pstHdr->eCmdId = CMD_ID_SET_PROPERTY;
    pstHdr->s32SessId = HCP_SESS_GET_HW_ID(pstSessCb);
    pstHdr->u32Size = sizeof(t_StHcpHfiCmdPktHdr);
    pstHdr->u32Size += sizeof(t_StHcpHfiCmdSetProp);

    LOGD("HwSessSetProp: propId=%u (%s)",
         pstProp->propId, pcGetPropIdStr(pstProp->propId));

    HCP_STAT(pstSessCb, FwTxSetProp);
    return u32VppIpHcp_HwSendCmd(HCP_HNDL_SESS(pstSessCb),
                                 HCP_SESS_RAW_PKT(pstSessCb, 0),
                                 HCP_SESS_CMD_PKT(pstSessCb));
}

static void vVppIpHcp_HwSessSetKnob(t_StVppIpHcpSessionCb *pstSessCb,
                                    t_StHcpHfiCmdSetProp *pstProp,
                                    uint32_t *pu32Ret)
{
    uint32_t u32;

    HCP_STAT(pstSessCb, FwTxPropSetKnob);

    u32 = u32VppIpHcp_HwSessSetProp(pstSessCb);
    if (u32 != VPP_OK)
    {
        LOGE("set knob failed for eKnobId=%u (%s), u32=%u",
             pstProp->stKnobs.eKnobId,
             pcGetKnobIdStr(pstProp->stKnobs.eKnobId), u32);

        if (pu32Ret)
            *pu32Ret = VPP_ERR_HW;
    }
}

static uint32_t u32VppIpHcp_HwSessGetProp(t_StVppIpHcpSessionCb *pstSessCb)
{
    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);

    t_StHcpHfiCmdGetProp *pstProp =
        (t_StHcpHfiCmdGetProp *)HCP_SESS_CMD_BDY(pstSessCb);
    t_StHcpHfiCmdPktHdr *pstHdr = HCP_SESS_CMD_HDR(pstSessCb);

    pstHdr->eCmdId = CMD_ID_GET_PROPERTY;
    pstHdr->s32SessId = HCP_SESS_GET_HW_ID(pstSessCb);
    pstHdr->u32Size = sizeof(t_StHcpHfiCmdPktHdr);
    pstHdr->u32Size += sizeof(t_StHcpHfiCmdGetProp);

    LOGD("HwSessGetProp: propId=%u (%s)",
         pstProp->propId, pcGetPropIdStr(pstProp->propId));

    HCP_STAT(pstSessCb, FwTxGetProp);
    return u32VppIpHcp_HwSendCmd(HCP_HNDL_SESS(pstSessCb),
                                 HCP_SESS_RAW_PKT(pstSessCb, 0),
                                 HCP_SESS_CMD_PKT(pstSessCb));
}

static void vVppIpHcp_PopulateUbwcStats(t_StVppBuf *pstBuf,
                                        t_StUbwcStats *pstStats)
{
    t_StVppUbwcStats *pstVppStat;

    if (!pstBuf || !pstStats)
        return;

    pstVppStat = &pstBuf->stUbwcStats[eVppUbwcStatField_P];

    if (pstVppStat->eVersion != eVppUbwcStatVer_2p0)
        return;

    pstStats->eValid = pstVppStat->bValid ? 1 : 0;
    pstStats->u32Num32BComp = pstVppStat->stats.st2p0.u32Stat32;
    pstStats->u32Num64BComp = pstVppStat->stats.st2p0.u32Stat64;
    pstStats->u32Num96BComp = pstVppStat->stats.st2p0.u32Stat96;
    pstStats->u32Num128BComp = pstVppStat->stats.st2p0.u32Stat128;
    pstStats->u32Num160BComp = pstVppStat->stats.st2p0.u32Stat160;
    pstStats->u32Num192BComp = pstVppStat->stats.st2p0.u32Stat192;
    pstStats->u32Num256BComp = pstVppStat->stats.st2p0.u32Stat256;
}

static void vVppIpHcp_PopulateVideoMetadata(t_StVppIpHcpSessionCb *pstSessCb,
                                            t_StVppBuf *pstBuf,
                                            t_StSessVdecParam *pstVdecParam)
{
    VPP_RET_VOID_IF_NULL(pstSessCb);
    VPP_RET_VOID_IF_NULL(pstBuf);
    VPP_RET_VOID_IF_NULL(pstVdecParam);

    memset(pstVdecParam, 0, sizeof(t_StSessVdecParam));
    pstVdecParam->nFrameRate = pstBuf->u32OperatingRate;
    pstVdecParam->eCodec = pstSessCb->stDrvCtx.u32CodecType;
    pstVdecParam->ePicType = PICTURETYPE_MAX;

#ifndef OMX_EXTRADATA_DOES_NOT_EXIST
    OMX_QCOM_EXTRADATA_QP *pstQp = (OMX_QCOM_EXTRADATA_QP *)
        vpVppBuf_FindExtradata(pstBuf, OMX_ExtraDataQP, EXTRADATA_BUFFER_FORMAT_OMX);

    OMX_QCOM_EXTRADATA_FRAMEINFO *pstInfo = (OMX_QCOM_EXTRADATA_FRAMEINFO *)
        vpVppBuf_FindExtradata(pstBuf, OMX_ExtraDataFrameInfo, EXTRADATA_BUFFER_FORMAT_OMX);

    if (pstQp)
    {
        pstVdecParam->nQPSum = pstQp->nQPSum;
        pstVdecParam->nQPSumCnt = pstQp->nTotalNumBlocks;
        pstVdecParam->nSkipQPSum = pstQp->nSkipQPSum;
        pstVdecParam->nSkipQPSumCnt = pstQp->nSkipNumBlocks;
    }

    if (pstInfo)
    {
        switch (pstInfo->ePicType)
        {
            case OMX_VIDEO_PictureTypeI:
                pstVdecParam->ePicType = PICTURETYPE_I;
                break;
            case OMX_VIDEO_PictureTypeP:
                pstVdecParam->ePicType = PICTURETYPE_P;
                break;
            case OMX_VIDEO_PictureTypeB:
                pstVdecParam->ePicType = PICTURETYPE_B;
                break;
            default:
                break;
        }
    }
#endif

    LOGI("meta: fd=%d, fr=%u, codec=%u, ftype=%u, qp={sum=%u, sumCnt=%u, skipSum=%u, skipCnt=%u}",
         pstBuf->stPixel.fd, pstVdecParam->nFrameRate, pstVdecParam->eCodec,
         pstVdecParam->ePicType, pstVdecParam->nQPSum, pstVdecParam->nQPSumCnt,
         pstVdecParam->nSkipQPSum, pstVdecParam->nSkipQPSumCnt);
}

static uint32_t u32VppIpHcp_PopulatePxBufAttr(t_StVppIpHcpSessionCb *pstSessCb,
                                              enum vpp_port ePort,
                                              t_StVppBuf *pstBuf,
                                              t_StHcpHfiBufAttr *pstBufAttr)
{
    uint32_t i, u32Str, u32Scl, u32PlSz;
    struct vpp_port_param *pstPort;
    uint32_t w, h;
    enum vpp_color_format f;
    t_StHcpHfiPixBufAttr *pstPxAttr;
    t_StHcpHfiVideo *pstVidParam;

    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pstBuf, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pstBufAttr, VPP_ERR_PARAM);

    if (ePort == VPP_PORT_INPUT)
        pstPort = &pstSessCb->stInput.stParam;
    else if (ePort == VPP_PORT_OUTPUT)
        pstPort = &pstSessCb->stOutput.stParam;
    else
        return VPP_ERR_PARAM;

    w = pstPort->width;
    h = pstPort->height;
    f = pstPort->fmt;

    pstPxAttr = &pstBufAttr->stPixBufAttr;
    pstVidParam = &pstPxAttr->stVideoParam;

    pstBufAttr->eBufType = BUFF_TYPE_PIXEL;
    pstBufAttr->u32Cookie_h = HCP_P_COOKIE_H(pstSessCb, pstBuf);
    pstBufAttr->u32Cookie_l = HCP_P_COOKIE_L(pstSessCb, pstBuf);
    pstBufAttr->u32Operation = 0;

    pstPxAttr->offset = 0;
    pstPxAttr->eDirection = (ePort == VPP_PORT_INPUT) ? BUFDIR_INPUT : BUFDIR_OUTPUT;

    pstPxAttr->u32NumPlanes = u32VppUtils_GetNumPlanes(f);
    for (i = 0; i < pstPxAttr->u32NumPlanes && i < MAX_NUM_PLANES; i++)
    {

        u32Str = u32VppUtils_CalcStrideForPlane(w, f, i);
        u32Scl = u32VppUtils_CalcScanlinesForPlane(h, f, i);
        u32PlSz = u32VppUtils_GetPlaneSize(w, h, f, i);

        LOGI("%s: plane[%u], w=%u, h=%u, str=%u, scl=%u, sz=%u",
             ePort == VPP_PORT_INPUT ? "in" : "out", i,
             w, h, u32Str, u32Scl, u32PlSz);

        pstPxAttr->stPlaneInfo[i].u32Stride = u32Str;
        pstPxAttr->stPlaneInfo[i].u32Sizebytes = u32PlSz;
    }

    pstVidParam->u32ImgWidth = w;
    pstVidParam->u32ImgHeight = h;
    pstVidParam->eFmtId = eVppIpHcp_GetFwFormat(f);

    // Populate metadata
    if (ePort == VPP_PORT_INPUT)
    {
        vVppIpHcp_PopulateVideoMetadata(pstSessCb, pstBuf, &pstPxAttr->stVdecParam);

        if (u32VppUtils_IsFmtUbwc(f))
            vVppIpHcp_PopulateUbwcStats(pstBuf, &pstPxAttr->stUbwcStats);
    }


    LOGI("a={eBufType=%u, ch=0x%x, cl=0x%x} px={off=%u, eDir=%u}, p={w=%u, h=%u, fmt=%u}",
         pstBufAttr->eBufType, pstBufAttr->u32Cookie_h, pstBufAttr->u32Cookie_l,
         pstPxAttr->offset, pstPxAttr->eDirection,
         pstVidParam->u32ImgWidth, pstVidParam->u32ImgHeight,
         pstVidParam->eFmtId);
    return VPP_OK;
}

static uint32_t u32VppIpHcp_PopulateMeasBufAttr(t_StVppIpHcpSessionCb *pstSessCb,
                                                t_StVppBuf *pstBuf,
                                                t_StHcpHfiBufAttr *pstBufAttr)
{
    t_StHcpHfiMeasBufAttr *pstMeasAttr;

    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pstBuf, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pstBufAttr, VPP_ERR_PARAM);

    pstMeasAttr = &pstBufAttr->stMeasBufAttr;

    pstBufAttr->eBufType = BUFF_TYPE_MEAS;
    pstBufAttr->u32Cookie_h = HCP_M_COOKIE_H(pstSessCb, pstBuf);
    pstBufAttr->u32Cookie_l = HCP_M_COOKIE_L(pstSessCb, pstBuf);
    pstBufAttr->u32Operation = 0;

    pstMeasAttr->u32Size = pstBuf->stPixel.u32ValidLen;

    return VPP_OK;
}


/************************************************************************
 * Global Functions
 ***********************************************************************/

uint32_t u32VppIpHcp_HwGetHandle(t_StVppIpHcpGlobalCb *pstGlobal)
{
    int res;
    remote_handle64 hndl = HCP_HNDL_UNINIT;

    LOG_ENTER();

    VPP_RET_IF_NULL(pstGlobal, VPP_ERR_PARAM);

    // creating handle, so we better not have a valid handle
    if (HCP_HNDL_GLOB_IS_VALID(pstGlobal))
        return VPP_ERR_STATE;

    res = hcp_rpc_open(hcp_rpc_URI CDSP_DOMAIN, &hndl);
    if (res != 0 || hndl == HCP_HNDL_UNINIT)
    {
        LOGE("hcp_rpc_open returned err, res=%d, hndl=%"PRIu64, res, hndl);
        return VPP_ERR_HW;
    }

    LOGD("hcp_rpc_open returned hndl=%"PRIu64, hndl);

    HCP_HNDL_GLOB(pstGlobal) = hndl;

    LOG_EXIT();

    return VPP_OK;
}

uint32_t u32VppIpHcp_HwReleaseHandle(t_StVppIpHcpGlobalCb *pstGlobal)
{
    int res;
    uint32_t u32 = VPP_OK;

    LOG_ENTER();

    VPP_RET_IF_NULL(pstGlobal, VPP_ERR_PARAM);

    if (!HCP_HNDL_GLOB_IS_VALID(pstGlobal))
        return VPP_ERR_STATE;

    res = hcp_rpc_close(HCP_HNDL_GLOB(pstGlobal));
    if (res != 0)
    {
        LOGE("hcp_rpc_close returned err, res=%d", res);
        u32 = VPP_ERR_HW;
    }

    HCP_HNDL_GLOB(pstGlobal) = HCP_HNDL_UNINIT;

    LOG_EXIT_RET(u32);
}

uint32_t u32VppIpHcp_HwGlobalGetInfo(t_StVppIpHcpGlobalCb *pstGlobal,
                                     hcp_rpc_lock_action_t eAction)
{
    VPP_RET_IF_NULL(pstGlobal, VPP_ERR_PARAM);
    return u32VppIpHcp_HwGetInfo(HCP_HNDL_GLOB(pstGlobal), pstGlobal->st.pstTs,
                                 eAction, VPP_TRUE);
}

uint32_t u32VppIpHcp_HwSessGetInfo(t_StVppIpHcpSessionCb *pstSessCb,
                                   hcp_rpc_lock_action_t eAction)
{
    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);

    VPP_UNUSED(eAction);
    VPP_NOT_IMPLEMENTED();

    return VPP_ERR;
}

uint32_t u32VppIpHcp_HwOpen(t_StVppIpHcpSessionCb *pstSessCb)
{
    uint32_t u32Ret = VPP_OK;
    int s32Id;
    AEEResult res;

    LOG_ENTER();

    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);

    if (!HCP_HNDL_SESS_IS_VALID(pstSessCb))
        return VPP_ERR_PARAM;

    VPP_IP_PROF_START(&pstSessCb->stBase, HCP_STAT_HW_OPEN);
    res = hcp_rpc_sess_open(HCP_HNDL_SESS(pstSessCb), &s32Id);
    VPP_IP_PROF_STOP(&pstSessCb->stBase, HCP_STAT_HW_OPEN);
    if (res == AEE_SUCCESS && s32Id >= 0)
    {
        HCP_SESS_GET_HW_ID(pstSessCb) = s32Id;
        vVppIpHcp_HwSetState(pstSessCb, HCP_HW_STATE_ALLOC);
        LOGI("opened hw session, s32Id=%d, res=%d", s32Id, res);
    }
    else
    {
        u32Ret = VPP_ERR_HW;
        LOGE("unable to open hw session, s32Id=%d, res=%d", s32Id, res);
    }

    LOG_EXIT_RET(u32Ret);
}

uint32_t u32VppIpHcp_HwClose(t_StVppIpHcpSessionCb *pstSessCb)
{
    uint32_t u32Ret = VPP_OK;
    AEEResult res;

    LOG_ENTER();

    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);

    if (!HCP_HNDL_SESS_IS_VALID(pstSessCb))
        return VPP_ERR_PARAM;

    if (HCP_SESS_GET_HW_ID(pstSessCb) < 0)
        return VPP_ERR_PARAM;

    VPP_IP_PROF_START(&pstSessCb->stBase, HCP_STAT_HW_CLOSE);
    VPP_IP_PROF_START(&pstSessCb->stBase, HCP_STAT_CLOSE_LATENCY);
    res = hcp_rpc_sess_close(HCP_HNDL_SESS(pstSessCb),
                             HCP_SESS_GET_HW_ID(pstSessCb));
    VPP_IP_PROF_STOP(&pstSessCb->stBase, HCP_STAT_HW_CLOSE);

    if (res == AEE_SUCCESS)
    {
        HCP_STAT(pstSessCb, FwTxClose);
        vVppIpHcp_HwSetState(pstSessCb, HCP_HW_STATE_CLOSING);
    }
    else
    {
        HCP_STAT(pstSessCb, FwTxCloseErr);
        u32Ret = VPP_ERR_HW;
        LOGE("unable to close hw session, s32Id=%d, res=%d",
             HCP_SESS_GET_HW_ID(pstSessCb), res);
    }

    LOG_EXIT_RET(u32Ret);
}

uint32_t u32VppIpHcp_HwGlobalRecvPkt(t_StVppIpHcpGlobalCb *pstGlobal,
                                     t_StHcpHfiMsgPkt *pstMsg)
{
    AEEResult res;
    uint32_t u32 = VPP_OK;

    VPP_RET_IF_NULL(pstGlobal, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pstMsg, VPP_ERR_PARAM);

    if (!HCP_HNDL_GLOB_IS_VALID(pstGlobal))
        return VPP_ERR_PARAM;

    pstGlobal->st.pstRawRspPkt->content = (unsigned char *)pstMsg;
    pstGlobal->st.pstRawRspPkt->contentLen = sizeof(t_StHcpHfiMsgPkt);

    res = hcp_rpc_recv_pkt(HCP_HNDL_GLOB(pstGlobal),
                           SESSION_ID_ALL, pstGlobal->st.pstRawRspPkt, 1);
    if (res != AEE_SUCCESS)
    {
        LOGE("recv_pkt returned error, res=%d", res);
        u32 = VPP_ERR_HW;
    }

    return u32;
}

uint32_t u32VppIpHcp_HwSessSendCmdNop(t_StVppIpHcpSessionCb *pstSessCb)
{
    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);
    HCP_STAT(pstSessCb, FwTxCmdNop);
    return u32VppIpHcp_HwSendCmdNop(HCP_HNDL_SESS(pstSessCb),
                                    HCP_SESS_RAW_PKT(pstSessCb, 0),
                                    HCP_SESS_CMD_PKT(pstSessCb));
}

uint32_t u32VppIpHcp_HwGlobalSendCmdNop(t_StVppIpHcpGlobalCb *pstGlobal)
{

    VPP_RET_IF_NULL(pstGlobal, VPP_ERR_PARAM);
    return u32VppIpHcp_HwSendCmdNop(HCP_HNDL_GLOB(pstGlobal),
                                    pstGlobal->st.pstRawCmdPkt,
                                    pstGlobal->st.pstCmdPkt);
}

uint32_t u32VppIpHcp_HwSessSetPropCp(t_StVppIpHcpSessionCb *pstSessCb,
                                     uint32_t bSecure)
{
    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);

    uint32_t u32;
    t_StHcpHfiCmdSetProp *pstProp =
        (t_StHcpHfiCmdSetProp *)HCP_SESS_CMD_BDY(pstSessCb);

    pstProp->propId = PROP_ID_SESS_CP_STATUS;

    pstProp->eCpStatus = bSecure ? CP_YES : CP_NO;

    LOGD("HwSessSetPropCp: eCpStatus=%u", pstProp->eCpStatus);

    HCP_STAT(pstSessCb, FwTxPropCpMode);
    VPP_IP_PROF_START(&pstSessCb->stBase, HCP_STAT_HW_SET_PROP_CP);
    u32 = u32VppIpHcp_HwSessSetProp(pstSessCb);
    VPP_IP_PROF_STOP(&pstSessCb->stBase, HCP_STAT_HW_SET_PROP_CP);
    return u32;
}

uint32_t u32VppIpHcp_HwSessSetPropOpMode(t_StVppIpHcpSessionCb *pstSessCb,
                                         t_EVppIpHcpOpMode eOpMode)
{
    uint32_t u32Ret;

    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);

    if (eOpMode !=  HCP_OP_MODE_PROC && eOpMode != HCP_OP_MODE_MEAS)
        return VPP_ERR_PARAM;

    t_StHcpHfiCmdSetProp *pstProp =
        (t_StHcpHfiCmdSetProp *)HCP_SESS_CMD_BDY(pstSessCb);

    pstProp->propId = PROP_ID_SESS_OPMODE;

    if (eOpMode == HCP_OP_MODE_MEAS)
        pstProp->eOpMode = OPMODE_MEASURE;

    else if (eOpMode == HCP_OP_MODE_PROC)
        pstProp->eOpMode = OPMODE_PROCESS;

    LOGD("HwSessSetPropOpMode: eOpMode=%u", pstProp->eOpMode);

    HCP_STAT(pstSessCb, FwTxPropOpMode);

    VPP_IP_PROF_START(&pstSessCb->stBase, HCP_STAT_HW_SET_PROP_OP_MODE);
    u32Ret = u32VppIpHcp_HwSessSetProp(pstSessCb);
    VPP_IP_PROF_STOP(&pstSessCb->stBase, HCP_STAT_HW_SET_PROP_OP_MODE);

    if (u32Ret == VPP_OK)
    {
        pstSessCb->stDrvCtx.eOpModeActive = eOpMode;
    }

    return u32Ret;
}

uint32_t u32VppIpHcp_HwSessSetPropResHints(t_StVppIpHcpSessionCb *pstSessCb)
{
    uint32_t u32;
    struct vpp_port_param *pstPort;
    t_StSessResourceHints *pstRes;

    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);

    t_StHcpHfiCmdSetProp *pstProp =
        (t_StHcpHfiCmdSetProp *)HCP_SESS_CMD_BDY(pstSessCb);

    pstPort = &pstSessCb->stInput.stParam;
    pstRes = &pstProp->stResourceHints;

    pstProp->propId = PROP_ID_SESS_RESOURCE_HINTS;

    pstRes->u32ImgWidth = pstPort->width;
    pstRes->u32ImgHeight = pstPort->height;
    pstRes->eFmtId = eVppIpHcp_GetFwFormat(pstPort->fmt);
    pstRes->u32FrameRate = HCP_DEFAULT_FRAMERATE;

    LOGD("HwSessSetPropResHints: u32ImgWidth=%u, u32ImgHeight=%u, eFmtId=%u, "
         "u32FrameRate=%u", pstRes->u32ImgWidth, pstRes->u32ImgHeight,
         pstRes->eFmtId, pstRes->u32FrameRate);

    HCP_STAT(pstSessCb, FwTxPropResHints);

    VPP_IP_PROF_START(&pstSessCb->stBase, HCP_STAT_HW_SET_PROP_HINTS);
    u32 = u32VppIpHcp_HwSessSetProp(pstSessCb);
    VPP_IP_PROF_START(&pstSessCb->stBase, HCP_STAT_HW_SET_PROP_HINTS);

    return u32;
}

#define LOG_KNOB(_p, _f) \
    LOGD("HwSessSetPropKnobs, eKnobId=%u (%s), "#_f"=%u", \
         (_p)->stKnobs.eKnobId, pcGetKnobIdStr((_p)->stKnobs.eKnobId), \
         (_p)->stKnobs._f)

uint32_t u32VppIpHcp_HwSessSetPropKnobs(t_StVppIpHcpSessionCb *pstSessCb)
{
    uint32_t u32, u32Ret = VPP_OK;
    t_StSessKnobs *pstKnobs;

    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);

    t_StHcpHfiCmdSetProp *pstProp =
        (t_StHcpHfiCmdSetProp *)HCP_SESS_CMD_BDY(pstSessCb);

    pstProp->propId = PROP_ID_SESS_KNOB;

    if (VPP_FLAG_IS_SET(pstSessCb->stCfg.u32LoadMask, HCP_FLAG_ALGO_EAR))
    {
        pstProp->stKnobs.eKnobId = KNOB_ID_EAR_NMODE;
        pstProp->stKnobs.u32EarMode = pstSessCb->stDrvCtx.u32EarMode;
        LOG_KNOB(pstProp, u32EarMode);
        VPP_IP_PROF_START(&pstSessCb->stBase, HCP_STAT_HW_SET_PROP_KNOBS);
        vVppIpHcp_HwSessSetKnob(pstSessCb, pstProp, &u32Ret);
        VPP_IP_PROF_STOP(&pstSessCb->stBase, HCP_STAT_HW_SET_PROP_KNOBS);

        VPP_FLAG_CLR(pstSessCb->stCfg.u32LoadMask, HCP_FLAG_ALGO_EAR);
    }

    if (VPP_FLAG_IS_SET(pstSessCb->stCfg.u32LoadMask, HCP_FLAG_ALGO_LTM_SAT))
    {
        pstProp->stKnobs.eKnobId = KNOB_ID_LTM_NSAT_GAIN;
        pstProp->stKnobs.u32SatGain = pstSessCb->stDrvCtx.u32SatGain;
        LOG_KNOB(pstProp, u32SatGain);
        VPP_IP_PROF_START(&pstSessCb->stBase, HCP_STAT_HW_SET_PROP_KNOBS);
        vVppIpHcp_HwSessSetKnob(pstSessCb, pstProp, &u32Ret);
        VPP_IP_PROF_STOP(&pstSessCb->stBase, HCP_STAT_HW_SET_PROP_KNOBS);

        pstProp->stKnobs.eKnobId = KNOB_ID_LTM_NSAT_OFF;
        pstProp->stKnobs.u32SatOff = pstSessCb->stDrvCtx.u32SatOff;
        LOG_KNOB(pstProp, u32SatOff);
        VPP_IP_PROF_START(&pstSessCb->stBase, HCP_STAT_HW_SET_PROP_KNOBS);
        vVppIpHcp_HwSessSetKnob(pstSessCb, pstProp, &u32Ret);
        VPP_IP_PROF_STOP(&pstSessCb->stBase, HCP_STAT_HW_SET_PROP_KNOBS);

        VPP_FLAG_CLR(pstSessCb->stCfg.u32LoadMask, HCP_FLAG_ALGO_LTM_SAT);
    }

    if (VPP_FLAG_IS_SET(pstSessCb->stCfg.u32LoadMask, HCP_FLAG_ALGO_LTM_ACE))
    {
        pstProp->stKnobs.eKnobId = KNOB_ID_LTM_NACESTRCON;
        pstProp->stKnobs.u32AceStrCon = pstSessCb->stDrvCtx.u32AceStrCon;
        LOG_KNOB(pstProp, u32AceStrCon);
        VPP_IP_PROF_START(&pstSessCb->stBase, HCP_STAT_HW_SET_PROP_KNOBS);
        vVppIpHcp_HwSessSetKnob(pstSessCb, pstProp, &u32Ret);
        VPP_IP_PROF_STOP(&pstSessCb->stBase, HCP_STAT_HW_SET_PROP_KNOBS);

        pstProp->stKnobs.eKnobId = KNOB_ID_LTM_NACESTRBRIL;
        pstProp->stKnobs.u32AceStrBriL = pstSessCb->stDrvCtx.u32AceStrBriL;
        LOG_KNOB(pstProp, u32AceStrBriL);
        VPP_IP_PROF_START(&pstSessCb->stBase, HCP_STAT_HW_SET_PROP_KNOBS);
        vVppIpHcp_HwSessSetKnob(pstSessCb, pstProp, &u32Ret);
        VPP_IP_PROF_STOP(&pstSessCb->stBase, HCP_STAT_HW_SET_PROP_KNOBS);

        pstProp->stKnobs.eKnobId = KNOB_ID_LTM_NACESTRBRIH;
        pstProp->stKnobs.u32AceStrBriH = pstSessCb->stDrvCtx.u32AceStrBriH;
        LOG_KNOB(pstProp, u32AceStrBriH);
        VPP_IP_PROF_START(&pstSessCb->stBase, HCP_STAT_HW_SET_PROP_KNOBS);
        vVppIpHcp_HwSessSetKnob(pstSessCb, pstProp, &u32Ret);
        VPP_IP_PROF_STOP(&pstSessCb->stBase, HCP_STAT_HW_SET_PROP_KNOBS);

        VPP_FLAG_CLR(pstSessCb->stCfg.u32LoadMask, HCP_FLAG_ALGO_LTM_ACE);
    }

    if (VPP_FLAG_IS_SET(pstSessCb->stCfg.u32LoadMask, HCP_FLAG_ALGO_DE))
    {
        pstProp->stKnobs.eKnobId = KNOB_ID_LTM_FDEGAIN;
        pstProp->stKnobs.u32DeGain = pstSessCb->stDrvCtx.u32DeGain;
        LOG_KNOB(pstProp, u32DeGain);
        VPP_IP_PROF_START(&pstSessCb->stBase, HCP_STAT_HW_SET_PROP_KNOBS);
        vVppIpHcp_HwSessSetKnob(pstSessCb, pstProp, &u32Ret);
        VPP_IP_PROF_STOP(&pstSessCb->stBase, HCP_STAT_HW_SET_PROP_KNOBS);

        VPP_FLAG_CLR(pstSessCb->stCfg.u32LoadMask, HCP_FLAG_ALGO_DE);
    }

    if (VPP_FLAG_IS_SET(pstSessCb->stCfg.u32LoadMask, HCP_FLAG_ALGO_DEMO))
    {
        pstProp->stKnobs.eKnobId = KNOB_ID_APP_SPLITSCREEN;
        pstProp->stKnobs.uSplitScreen = pstSessCb->stDrvCtx.uSplitScreen;
        LOG_KNOB(pstProp, uSplitScreen.st.eProcessedRegion);
        LOG_KNOB(pstProp, uSplitScreen.st.u32ProcessedPercent);
        VPP_IP_PROF_START(&pstSessCb->stBase, HCP_STAT_HW_SET_PROP_KNOBS);
        vVppIpHcp_HwSessSetKnob(pstSessCb, pstProp, &u32Ret);
        VPP_IP_PROF_STOP(&pstSessCb->stBase, HCP_STAT_HW_SET_PROP_KNOBS);

        VPP_FLAG_CLR(pstSessCb->stCfg.u32LoadMask, HCP_FLAG_ALGO_DEMO);
    }

    return u32Ret;
}

uint32_t u32VppIpHcp_HwSessSetPropDiagLevel(t_StVppIpHcpSessionCb *pstSessCb,
                                            uint32_t u32LogFlag)
{
    // Even though this is a system property, effectively make it a session
    // property since (i) we want to send it with every session since it is
    // very unlikely that the setprop that enables it would be set before boot,
    // (ii) it's debug specific so it doesn't matter if it causes logs for all
    // sessions

    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);

    uint32_t u32;
    t_StHcpHfiCmdSetProp *pstProp =
        (t_StHcpHfiCmdSetProp *)HCP_SESS_CMD_BDY(pstSessCb);

    pstProp->propId = PROP_ID_SYS_DIAG;

    pstProp->stSysDiag.u32Content = DIAG_CONTENT_LOG;
    pstProp->stSysDiag.u32LogFlag = u32LogFlag;

    LOGD("HwSessSetPropDiagLevel: u32LogFlag=%u", u32LogFlag);

    HCP_STAT(pstSessCb, FwTxPropSetDiag);
    VPP_IP_PROF_START(&pstSessCb->stBase, HCP_STAT_HW_SET_PROP_DIAG_LVL);
    u32 = u32VppIpHcp_HwSessSetProp(pstSessCb);
    VPP_IP_PROF_STOP(&pstSessCb->stBase, HCP_STAT_HW_SET_PROP_DIAG_LVL);

    return u32;
}

uint32_t u32VppIpHcp_HwSessGetPropBufReq(t_StVppIpHcpSessionCb *pstSessCb)
{
    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);

    uint32_t u32;
    t_StHcpHfiCmdGetProp *pstProp =
        (t_StHcpHfiCmdGetProp *)HCP_SESS_CMD_BDY(pstSessCb);

    pstProp->propId = PROP_ID_SESS_BUFREQ;

    HCP_STAT(pstSessCb, FwTxGetPropBufReq);
    VPP_IP_PROF_START(&pstSessCb->stBase, HCP_STAT_HW_GET_BUF_REQ);
    u32 = u32VppIpHcp_HwSessGetProp(pstSessCb);
    VPP_IP_PROF_STOP(&pstSessCb->stBase, HCP_STAT_HW_GET_BUF_REQ);
    return u32;
}

uint32_t u32VppIpHcp_HwSessSendSessBuf(t_StVppIpHcpSessionCb *pstSessCb)
{
    AEEResult res;
    uint32_t u32Cnt = 0;
    uint32_t u32Ret = VPP_OK;
    t_StHcpHfiBufAttr *pstBufAttr;
    t_StHcpHfiMeasBufAttr *pstIntMeasAttr;
    t_StHcpHfiIntermPixBufAttr *pstIntPxAttr;
    t_StVppIpHcpIntBuf *pstIntBufPx, *pstIntBufMeas;

    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);

    if (!HCP_HNDL_SESS_IS_VALID(pstSessCb))
        return VPP_ERR_PARAM;

    // Int. Px.
    pstBufAttr = &pstSessCb->st.pstBufAttr[u32Cnt];
    pstIntPxAttr = &pstBufAttr->stIntermPixBufAttr;
    pstIntBufPx = &pstSessCb->stIntBufs.stIntBufPx;

    pstIntBufPx->eBufState = HCP_BUF_OWNER_FW;

    pstBufAttr->eBufType = BUFF_TYPE_INTERMEDIATE_PIXEL;
    pstBufAttr->u32Cookie_h = HCP_INTP_COOKIE_H(pstSessCb);
    pstBufAttr->u32Cookie_l = HCP_INTP_COOKIE_L(pstSessCb);
    pstBufAttr->u32Operation = 0;

    pstIntPxAttr->u32Size = pstIntBufPx->u32Sz;
    pstIntPxAttr->eCpStatus = pstIntBufPx->bSecure ? CP_YES : CP_NO;

    HCP_SESS_RAW_PKT(pstSessCb, u32Cnt)->content = (unsigned char *)pstBufAttr;
    HCP_SESS_RAW_PKT(pstSessCb, u32Cnt)->contentLen = sizeof(t_StHcpHfiBufAttr);

    LOGD("HwSessSendSessBuf: int_px={eBufType=%u, u32Size=%u, eCpStatus=%u, "
         "fd=%d, u32Sz=%u}", pstBufAttr->eBufType, pstIntPxAttr->u32Size,
         pstIntPxAttr->eCpStatus, pstIntBufPx->stIon.fd_ion_mem, pstIntBufPx->u32Sz);

    VPP_IP_PROF_START(&pstSessCb->stBase, HCP_STAT_HW_SEND_SESS_BUF_PX);
    res = hcp_rpc_send_sess_buf_dma(HCP_HNDL_SESS(pstSessCb),
                                    HCP_SESS_GET_HW_ID(pstSessCb),
                                    pstSessCb->st.pstPkt, 1,
                                    pstIntBufPx->stIon.fd_ion_mem,
                                    0, pstIntBufPx->u32Sz);
    VPP_IP_PROF_STOP(&pstSessCb->stBase, HCP_STAT_HW_SEND_SESS_BUF_PX);

    if (res != AEE_SUCCESS)
    {
        pstIntBufPx->eBufState = HCP_BUF_OWNER_IP;
        LOGE("unable to send int. px, res=%d", res);
        return VPP_ERR_HW;
    }

    // Int Meas.
    if (pstSessCb->stCfg.eOpMode == HCP_OP_MODE_PROC)
    {
        pstBufAttr = &pstSessCb->st.pstBufAttr[u32Cnt];
        pstIntMeasAttr = &pstBufAttr->stMeasBufAttr;
        pstIntBufMeas = &pstSessCb->stIntBufs.stIntBufMeas;

        pstIntBufMeas->eBufState = HCP_BUF_OWNER_FW;

        pstBufAttr->eBufType = BUFF_TYPE_MEAS;
        pstBufAttr->u32Cookie_h = HCP_INTM_COOKIE_H(pstSessCb);
        pstBufAttr->u32Cookie_l = HCP_INTM_COOKIE_L(pstSessCb);
        pstBufAttr->u32Operation = 0;

        pstIntMeasAttr->u32Size = pstIntBufMeas->u32Sz;

        HCP_SESS_RAW_PKT(pstSessCb, u32Cnt)->content = (unsigned char *)pstBufAttr;
        HCP_SESS_RAW_PKT(pstSessCb, u32Cnt)->contentLen = sizeof(t_StHcpHfiBufAttr);

        LOGD("HwSessSendSessBuf: int_meas={eBufType=%u, u32Size=%u, "
             "fd=%d, u32Sz=%u}", pstBufAttr->eBufType, pstIntMeasAttr->u32Size,
             pstIntBufMeas->stIon.fd_ion_mem, pstIntBufMeas->u32Sz);

        VPP_IP_PROF_START(&pstSessCb->stBase, HCP_STAT_HW_SEND_SESS_BUF_MEAS);
        res = hcp_rpc_send_sess_buf_dma(HCP_HNDL_SESS(pstSessCb),
                                        HCP_SESS_GET_HW_ID(pstSessCb),
                                        pstSessCb->st.pstPkt, 1,
                                        pstIntBufMeas->stIon.fd_ion_mem,
                                        0, pstIntBufMeas->u32Sz);
        VPP_IP_PROF_STOP(&pstSessCb->stBase, HCP_STAT_HW_SEND_SESS_BUF_MEAS);
        if (res != AEE_SUCCESS)
        {
            pstIntBufMeas->eBufState = HCP_BUF_OWNER_IP;
            LOGE("unable to send int. meas, res=%d", res);
            return VPP_ERR_HW;
        }

    }

    return u32Ret;
}

uint32_t u32VppIpHcp_HwSessSendBufs(t_StVppIpHcpSessionCb *pstSessCb,
                                    t_StVppBuf **ppstBufIn,
                                    uint32_t u32InCnt,
                                    t_StVppBuf **ppstBufOut,
                                    uint32_t u32OutCnt)
{
    AEEResult res;
    uint32_t u32, i;

    t_StVppBuf *pstBufIn, *pstBufOut;
    t_StHcpHfiBufAttr *pstBufInAttr, *pstBufOutAttr;

    hcp_rpc_pkt_t *pstInAttrStart, *pstOutAttrStart;

    VPP_RET_IF_NULL(pstSessCb, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(ppstBufIn, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(ppstBufOut, VPP_ERR_PARAM);

    if (!HCP_HNDL_SESS_IS_VALID(pstSessCb))
        return VPP_ERR_PARAM;

    // Work around for limitation in dma_buf API
    if (u32InCnt != 1 && u32OutCnt != 1)
    {
        LOGE("HwSessSendBufs failed, u32InCnt=%u, u32OutCnt=%u",
             u32InCnt, u32OutCnt);
        return VPP_ERR_PARAM;
    }

    if (!u32InCnt || !u32OutCnt || (u32InCnt != u32OutCnt))
    {
        LOGE("HwSessSendBufs failed, u32InCnt=%u, u32OutCnt=%u",
             u32InCnt, u32OutCnt);
        return VPP_ERR_PARAM;
    }

#define IN_IDX 0
#define OUT_IDX 1
    for (i = 0; i < u32InCnt; i++)
    {
        pstInAttrStart = HCP_SESS_RAW_PKT(pstSessCb, IN_IDX);
        pstOutAttrStart = HCP_SESS_RAW_PKT(pstSessCb, OUT_IDX);

        // input buffer
        pstBufIn = ppstBufIn[i];
        pstBufInAttr = &pstSessCb->st.pstBufAttr[IN_IDX];

        u32 = u32VppIpHcp_PopulatePxBufAttr(pstSessCb, VPP_PORT_INPUT,
                                            pstBufIn, pstBufInAttr);
        if (u32 != VPP_OK)
        {
            LOGE("PopulatePxBufAttr(input) failed, u32=%u", u32);
            return VPP_ERR_PARAM;
        }

        LOGD("HwSessSendBufs:  in[%u]={num=%u, cookie_l=0x%x, cookie_h=0x%x, "
             "fd=%u, u32Off=%u, u32Valid=%u}",
             i, pstBufIn->u32Idx, pstBufInAttr->u32Cookie_l,
             pstBufInAttr->u32Cookie_h, pstBufIn->stPixel.fd,
             pstBufIn->stPixel.u32Offset, pstBufIn->stPixel.u32ValidLen);

#ifdef HCP_DBG_WRITE_DUMP_BUF
        u32VppIpHcpUtils_WriteToBuf(pstBufIn, HCP_FILL_TYPE_COUNTER, HCP_DBG_WB_SIZE);
#endif

        // output buffer
        pstBufOut = ppstBufOut[i];
        pstBufOutAttr = &pstSessCb->st.pstBufAttr[OUT_IDX];

        if (pstSessCb->stCfg.eOpMode == HCP_OP_MODE_PROC)
        {
            u32 = u32VppIpHcp_PopulatePxBufAttr(pstSessCb, VPP_PORT_OUTPUT,
                                                pstBufOut, pstBufOutAttr);
        }
        else
        {
            u32 = u32VppIpHcp_PopulateMeasBufAttr(pstSessCb, pstBufOut, pstBufOutAttr);
        }

        if (u32 != VPP_OK)
        {
            LOGE("Populate[Meas|Px]BufAttr(output) failed, u32=%u", u32);
            return VPP_ERR_PARAM;
        }

        LOGD("HwSessSendBufs: out[%u]={num=%u, cookie_l=0x%x, cookie_h=0x%x, "
             "fd=%u, u32Off=%u, u32Valid=%u}",
             i, pstBufOut->u32Idx, pstBufOutAttr->u32Cookie_l,
             pstBufOutAttr->u32Cookie_h,
             pstBufOut->stPixel.fd,
             pstBufOut->stPixel.u32Offset, pstBufOut->stPixel.u32ValidLen);

#ifdef HCP_DBG_WRITE_DUMP_BUF
        u32VppIpHcpUtils_WriteToBuf(pstBufOut, HCP_FILE_TYPE_AB, HCP_DBG_WB_SIZE);
#endif

        pstInAttrStart->content = (unsigned char *)pstBufInAttr;
        pstInAttrStart->contentLen = sizeof(t_StHcpHfiBufAttr);
        pstOutAttrStart->content = (unsigned char *)pstBufOutAttr;
        pstOutAttrStart->contentLen = sizeof(t_StHcpHfiBufAttr);

        // send the buffers to firmware
        VPP_IP_PROF_START(&pstSessCb->stBase, HCP_STAT_HW_SEND_BUFS);
        res = hcp_rpc_send_bufs_dma(HCP_HNDL_SESS(pstSessCb),
                                    HCP_SESS_GET_HW_ID(pstSessCb),
                                    pstInAttrStart, 1,
                                    pstBufIn->stPixel.fd,
                                    pstBufIn->stPixel.u32Offset,
                                    pstBufIn->stPixel.u32ValidLen,
                                    pstOutAttrStart, 1,
                                    pstBufOut->stPixel.fd,
                                    pstBufOut->stPixel.u32Offset,
                                    pstBufOut->stPixel.u32ValidLen);
        VPP_IP_PROF_STOP(&pstSessCb->stBase, HCP_STAT_HW_SEND_BUFS);
        if (res != AEE_SUCCESS)
        {
            LOGE("unable to send buffers to firmware, res=%d", res);
            if (res == ERR_ID_NO_RESOURCE)
                return VPP_ERR_RESOURCES;

            return VPP_ERR_HW;
        }
        else
        {
            pstSessCb->stDrvCtx.u32InBufCnt++;
            pstSessCb->stDrvCtx.u32OutBufCnt++;
        }
    }

    return VPP_OK;
}

uint32_t u32VppIpHcp_HwGlobalSendSysBuf(t_StVppIpHcpGlobalCb *pstGlobal,
                                        t_StVppIpHcpSysBuf *pstBuf)
{
    AEEResult res;
    t_StHcpHfiBufAttr *pstBufAttr;
    t_StHcpHfiGenBufAttr *pstGenBufAttr;
    hcp_rpc_pkt_t *pstPkt;

    VPP_RET_IF_NULL(pstGlobal, VPP_ERR_PARAM);
    VPP_RET_IF_NULL(pstBuf, VPP_ERR_PARAM);

    if (!HCP_HNDL_GLOB_IS_VALID(pstGlobal))
        return VPP_ERR_PARAM;

    pstBufAttr = pstGlobal->st.pstBufAttr;
    memset(pstBufAttr, 0, sizeof(t_StHcpHfiBufAttr));
    pstGenBufAttr = &pstBufAttr->stGenBufAttr;

    pstBufAttr->eBufType = BUFF_TYPE_GENERIC;
    pstGenBufAttr->u32Size = pstBuf->u32Sz;
    pstGenBufAttr->eDirection = BUFDIR_NUM; // TODO: this value.

    if (pstBuf->eSysBufType == HCP_BUF_TYPE_DIAG)
    {
    }
    else if (pstBuf->eSysBufType == HCP_BUF_TYPE_TUNING)
    {
        pstBufAttr->u32Cookie_h = HCP_SYS_TUNE_COOKIE_H;
        pstBufAttr->u32Cookie_l = HCP_SYS_TUNE_COOKIE_L;
        pstGenBufAttr->eUsage = BUFF_USAGE_TUNE;
    }
    else
    {
        LOGE("invalid system buffer type. type=%u", pstBuf->eSysBufType);
        return VPP_ERR_PARAM;
    }

    pstPkt = pstGlobal->st.pstRawCmdPkt;
    pstPkt->content = (unsigned char *)pstBufAttr;
    pstPkt->contentLen = sizeof(t_StHcpHfiBufAttr);

    LOGD("HwGlobalSendSysBuf: eBufType=%u, u32Size=%u, eUsage=%u, fd=%d",
         pstBufAttr->eBufType, pstGenBufAttr->u32Size, pstGenBufAttr->eUsage,
         pstBuf->stIon.fd_ion_mem);

    res = hcp_rpc_send_sys_buf_dma(HCP_HNDL_GLOB(pstGlobal), pstPkt, 1,
                                   pstBuf->stIon.fd_ion_mem, 0, pstBuf->u32Sz);
    if (res != AEE_SUCCESS)
    {
        LOGE("unable to send system buffer. res=%d", res);
        return VPP_ERR_HW;
    }

    return VPP_OK;
}

void vVppIpHcp_HwSetState(t_StVppIpHcpSessionCb *pstSessCb,
                          t_EVppIpHcpHwState eNewState)
{
    VPP_RET_VOID_IF_NULL(pstSessCb);

    t_EVppIpHcpHwState ePrevState = pstSessCb->stDrvCtx.eHwState;
    pstSessCb->stDrvCtx.eHwState = eNewState;

    LOGI("hw state changed from (%s) to (%s) for session %p",
         pcGetHwStateStr(ePrevState), pcGetHwStateStr(eNewState), pstSessCb);
}

void vVppIpHcp_ExtractUbwcStats(t_StVppBuf *pstBuf,
                                t_StHcpHfiPixBufAttr *pstPxAttr)
{
    t_StUbwcStats *pstStats;
    t_StVppUbwcStats *pstVppStat;

    if (!pstBuf || !pstPxAttr)
        return;

    pstVppStat = &pstBuf->stUbwcStats[eVppUbwcStatField_P];
    pstStats = &pstPxAttr->stUbwcStats;

    pstVppStat->eVersion = eVppUbwcStatVer_2p0;
    pstVppStat->bValid = pstStats->eValid ? VPP_TRUE : VPP_FALSE;
    pstVppStat->stats.st2p0.u32Stat32 = pstStats->u32Num32BComp;
    pstVppStat->stats.st2p0.u32Stat64 = pstStats->u32Num64BComp;
    pstVppStat->stats.st2p0.u32Stat96 = pstStats->u32Num96BComp;
    pstVppStat->stats.st2p0.u32Stat128 = pstStats->u32Num128BComp;
    pstVppStat->stats.st2p0.u32Stat160 = pstStats->u32Num160BComp;
    pstVppStat->stats.st2p0.u32Stat192 = pstStats->u32Num192BComp;
    pstVppStat->stats.st2p0.u32Stat256 = pstStats->u32Num256BComp;

#ifdef HCP_DBG_UBWC_STATS
    vVppBuf_UbwcStatDump("hcp: ", pstVppStat);
#endif
}

uint32_t u32VppIpHcp_VppToHfiCodec(enum vpp_codec_type eCodec)
{
    t_EHcpHfiCodecType eHfiCodec = CODECTYPE_MAX;
    switch (eCodec)
    {
        case VPP_CODEC_TYPE_AVC:     eHfiCodec = CODECTYPE_H264;     break;
        case VPP_CODEC_TYPE_DIVX4:   eHfiCodec = CODECTYPE_DIVX;     break;
        case VPP_CODEC_TYPE_DIVX:    eHfiCodec = CODECTYPE_DIVX;     break;
        case VPP_CODEC_TYPE_DIVX311: eHfiCodec = CODECTYPE_DIVX_311; break;
        case VPP_CODEC_TYPE_MPEG4:   eHfiCodec = CODECTYPE_MPEG4;    break;
        case VPP_CODEC_TYPE_MPEG2:   eHfiCodec = CODECTYPE_MPEG2;    break;
        case VPP_CODEC_TYPE_VC1:     eHfiCodec = CODECTYPE_VC1;      break;
        case VPP_CODEC_TYPE_H263:    eHfiCodec = CODECTYPE_H263;     break;
        case VPP_CODEC_TYPE_HEVC:    eHfiCodec = CODECTYPE_HEVC;     break;
        case VPP_CODEC_TYPE_VP8:     eHfiCodec = CODECTYPE_VP8;      break;
        case VPP_CODEC_TYPE_VP9:     eHfiCodec = CODECTYPE_VP9;      break;
        case VPP_CODEC_TYPE_WMV:
        case VPP_CODEC_TYPE_UNKNOWN:
        case VPP_CODEC_TYPE_MAX:
        default:                     eHfiCodec = CODECTYPE_MAX;      break;
    }
    return (uint32_t)eHfiCodec;
}
