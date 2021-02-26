/*!
 * @file vpp_ip_hcp.h
 *
 * @cr
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services
 */

#ifndef _VPP_IP_HCP_H_
#define _VPP_IP_HCP_H_

#include "vpp_ctx.h"
#include "vpp_reg.h"


/***************************************************************************
 * Function Prototypes
 ***************************************************************************/

uint32_t u32VppIpHcp_Boot();
uint32_t u32VppIpHcp_Shutdown();

void * vpVppIpHcp_Init(t_StVppCtx *pstCtx, uint32_t u32Flags, t_StVppCallback cbs);
void vVppIpHcp_Term(void *ctx);

uint32_t u32VppIpHcp_Open(void *ctx);
uint32_t u32VppIpHcp_Close(void *ctx);

uint32_t u32VppIpHcp_SetParam(void *ctx, enum vpp_port ePort,
                              struct vpp_port_param stParam);
uint32_t u32VppIpHcp_SetCtrl(void *ctx, struct hqv_control ctrl);
uint32_t u32VppIpHcp_ResetCtrl(void *ctx);

uint32_t u32VppIpHcp_GetBufferRequirements(void *ctx,
                                           t_StVppIpBufReq *pstInputBufReq,
                                           t_StVppIpBufReq *pstOutputBufReq);
uint32_t u32VppIpHcp_QueueBuf(void *ctx, enum vpp_port ePort,
                              t_StVppBuf *pBuf);
uint32_t u32VppIpHcp_Flush(void *ctx, enum vpp_port ePort);
uint32_t u32VppIpHcp_Drain(void *ctx);
uint32_t u32VppIpHcp_Reconfigure(void *ctx,
                                 struct vpp_port_param in_param,
                                 struct vpp_port_param out_param);

#endif /* _VPP_IP_HCP_H_ */
