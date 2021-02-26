/*!
 * @file vpp_ip_frc_core.h
 *
 * @cr
 * Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services
 */

#ifndef _VPP_IP_FRC_CORE_H_
#define _VPP_IP_FRC_CORE_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 * Function Prototypes
 ***************************************************************************/

void *vpVppIpFrc_Init(t_StVppCtx *pstCtx, uint32_t u32Flags, t_StVppCallback cbs);
void vVppIpFrc_Term(void *ctx);
uint32_t u32VppIpFrc_Open(void *ctx);
uint32_t u32VppIpFrc_Close(void *ctx);
uint32_t u32VppIpFrc_SetParam(void *ctx, enum vpp_port port,
                              struct vpp_port_param param);
uint32_t u32VppIpFrc_SetCtrl(void *ctx, struct hqv_control ctrl);
uint32_t u32VppIpFrc_GetBufferRequirements(void *ctx,
                                           t_StVppIpBufReq *pstInputBufReq,
                                           t_StVppIpBufReq *pstOutputBufReq);
uint32_t u32VppIpFrc_QueueBuf(void *ctx, enum vpp_port ePort,
                              t_StVppBuf *pBuf);
uint32_t u32VppIpFrc_Flush(void *ctx, enum vpp_port ePort);

uint32_t u32VppIpFrc_Drain(void *ctx);
uint32_t u32VppIpFrc_Reconfigure(void *ctx,
                                 struct vpp_port_param in_param,
                                 struct vpp_port_param out_param);

#ifdef __cplusplus
 }
#endif

#endif /* _VPP_IP_FRC_CORE_H_ */
