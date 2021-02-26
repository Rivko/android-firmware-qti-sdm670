/*!
 * @file vpp_hcp_ip_dbg.c
 *
 * @cr
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services
 */

#include <sys/types.h>

#define VPP_LOG_TAG     VPP_LOG_MODULE_HCP_TAG
#define VPP_LOG_MODULE  VPP_LOG_MODULE_HCP
#include "vpp_dbg.h"
#include "vpp_utils.h"

#include "hcp_hfi.h"
#include "hcp_rpc.h"

#include "vpp_ip_hcp_dbg.h"
#include "vpp_ip_hcp_priv.h"
#include "vpp_ip_hcp_hfi.h"

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

/************************************************************************
 * Global Functions
 ***********************************************************************/
const char *pcGetKnobIdStr(uint32_t u32KnobId)
{
    VPP_RET_STR(u32KnobId, KNOB_ID_EAR_NMODE);
    VPP_RET_STR(u32KnobId, KNOB_ID_LTM_FDEGAIN);
    VPP_RET_STR(u32KnobId, KNOB_ID_LTM_NSAT_GAIN);
    VPP_RET_STR(u32KnobId, KNOB_ID_LTM_NSAT_OFF);
    VPP_RET_STR(u32KnobId, KNOB_ID_LTM_NACESTRBRIL);
    VPP_RET_STR(u32KnobId, KNOB_ID_LTM_NACESTRBRIH);
    VPP_RET_STR(u32KnobId, KNOB_ID_LTM_NACESTRCON);
    VPP_RET_STR(u32KnobId, KNOB_ID_APP_SPLITSCREEN);
    return "UNKNOWN KNOB ID";
}

const char *pcGetFwCmdStr(uint32_t u32Cmd)
{
    VPP_RET_STR(u32Cmd, CMD_ID_SET_PROPERTY);
    VPP_RET_STR(u32Cmd, CMD_ID_GET_PROPERTY);
    VPP_RET_STR(u32Cmd, CMD_ID_SESS_SET_BUFFERS);
    VPP_RET_STR(u32Cmd, CMD_ID_SESS_CLOSE);
    VPP_RET_STR(u32Cmd, CMD_ID_NOOP);
    return "UNKNOWN KNOB ID";
}

const char *pcGetMsgIdStr(uint32_t u32MsgId)
{
    VPP_RET_STR(u32MsgId, MSG_ID_SET_PROPERTY_DONE);
    VPP_RET_STR(u32MsgId, MSG_ID_PROPERTY);
    VPP_RET_STR(u32MsgId, MSG_ID_BUFF_RELEASED);
    VPP_RET_STR(u32MsgId, MSG_ID_CLOSE_DONE);
    VPP_RET_STR(u32MsgId, MSG_ID_EVENT_NOTIFY);
    VPP_RET_STR(u32MsgId, MSG_ID_NOOP);
    return "UNKNOWN MSG ID";
}

const char *pcGetPropIdStr(uint32_t u32PropId)
{
    VPP_RET_STR(u32PropId, PROP_ID_SYS_DIAG);
    VPP_RET_STR(u32PropId, PROP_ID_SESS_RESOURCE_HINTS);
    VPP_RET_STR(u32PropId, PROP_ID_SESS_BUFREQ);
    VPP_RET_STR(u32PropId, PROP_ID_SESS_KNOB);
    VPP_RET_STR(u32PropId, PROP_ID_SESS_OPMODE);
    VPP_RET_STR(u32PropId, PROP_ID_SESS_CP_STATUS);
    return "UNKNOWN PROP ID";
}

VPP_DBG_ENUM(t_EVppIpHcpHwState, pcGetHwStateStr, HCP_ENUM_HW_STATE);
VPP_DBG_ENUM(t_EVppIpHcpCmdState, pcGetCmdStateStr, HCP_ENUM_CMD_STATE);

void vVppIpHcpUtils_Assert(int cond, uint32_t line, const char * strCond,
                           const char * strFile, const char * strFunc)
{
    if (!cond)
    {
        LOGE("ASSERT_FAILED: Line: %u File: %s Function: %s", line, strFile, strFunc);
        LOGE("               Cond: %s", strCond);
    }
}

#ifdef HCP_DBG_WRITE_DUMP_BUF
uint32_t u32VppIpHcpUtils_WriteToBuf(t_StVppBuf *pstBuf,
                                     t_EVppIpHcpDbgFillType eFt,
                                     uint32_t u32NumBytes)
{
    uint32_t i;

    if (!pstBuf)
        return VPP_ERR_PARAM;

    if (!pstBuf->stPixel.pvBase)
    {
        LOGE("unable to write to buffer, fd=%d, pv=%p",
             pstBuf->stPixel.fd, pstBuf->stPixel.pvBase);
        return VPP_ERR_PARAM;
    }

    if (u32NumBytes > pstBuf->stPixel.u32ValidLen)
    {
        LOGE("only writing %u bytes, since u32ValidLen=%u",
             pstBuf->stPixel.u32ValidLen, u32NumBytes);
        u32NumBytes = pstBuf->stPixel.u32ValidLen;
    }

    for (i = 0; i < u32NumBytes; i++)
    {
        *(((unsigned char *)pstBuf->stPixel.pvBase) + i) =
            eFt == HCP_FILL_TYPE_COUNTER ? i : 0xab;
    }

    return VPP_OK;
}

uint32_t u32VppIpHcpUtils_DumpFromBuf(t_StVppBuf *pstBuf, uint32_t u32NumBytes)
{
    uint32_t i;

    if (!pstBuf)
        return VPP_ERR_PARAM;

    if (!pstBuf->stPixel.pvBase)
    {
        LOGE("unable to read from buffer, fd=%d, pv=%p",
             pstBuf->stPixel.fd, pstBuf->stPixel.pvBase);
        return VPP_ERR_PARAM;
    }

    if (u32NumBytes > pstBuf->stPixel.u32ValidLen)
    {
        LOGE("only reading %u bytes, since u32ValidLen=%u",
             pstBuf->stPixel.u32ValidLen, u32NumBytes);
        u32NumBytes = pstBuf->stPixel.u32ValidLen;
    }

    for (i = 0; i < u32NumBytes; i++)
    {
        LOGE("buf@[%u] = 0x%x", i, ((unsigned char *)pstBuf->stPixel.pvBase)[i]);
    }

    return VPP_OK;
}
#endif
