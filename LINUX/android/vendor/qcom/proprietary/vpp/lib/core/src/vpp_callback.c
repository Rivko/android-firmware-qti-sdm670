/*!
 * @file vpp_callback.c
 *
 * @cr
 * Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.

 * @services
 *
 *
 */

#include <sys/types.h>

#include "vpp.h"

#define VPP_LOG_TAG     VPP_LOG_MODULE_CB_TAG
#define VPP_LOG_MODULE  VPP_LOG_MODULE_CB
#include "vpp_dbg.h"

#include "vpp_ctx.h"
#include "vpp_callback.h"

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
uint32_t u32VppIp_CbBufDone(t_StVppCallback *pstCb, enum vpp_port ePort,
                            t_StVppBuf *pstBuf)
{
    struct vpp_buffer *pBuf = pstBuf ? pstBuf->pBuf : NULL;

    if (ePort == VPP_PORT_INPUT && pstCb->input_buffer_done)
    {
        LOGI("issuing input_buffer_done, pstBuf=%p, pBuf=%p", pstBuf, pBuf);
        pstCb->input_buffer_done(pstCb->pv, pstBuf);
        return VPP_OK;
    }
    else if (ePort == VPP_PORT_OUTPUT && pstCb->output_buffer_done)
    {
        LOGI("issuing output_buffer_done, pstBuf=%p, pBuf=%p", pstBuf, pBuf);
        pstCb->output_buffer_done(pstCb->pv, pstBuf);
        return VPP_OK;
    }
    LOGE("unable to send callback, port=%d, buf=%p", ePort, pstBuf);
    return VPP_ERR;
}

uint32_t u32VppIpCbEvent(t_StVppCallback *pstCb, t_StVppEvt stEvt)
{
    if (pstCb->event)
    {
        LOGI("issuing event callback, evtType=%u", stEvt.eType);
        pstCb->event(pstCb->pv, stEvt);
        return VPP_OK;
    }
    LOGE("unable to send event callback, evtType=%u", stEvt.eType);
    return VPP_ERR;
}

void vVppIpCbLog(t_StVppCallback *pstCb, t_StVppBuf *pstBuf, t_EVppBufIpLogId eId)
{
    if (pstCb->log)
        pstCb->log(pstCb->pv, pstBuf, eId);
}
