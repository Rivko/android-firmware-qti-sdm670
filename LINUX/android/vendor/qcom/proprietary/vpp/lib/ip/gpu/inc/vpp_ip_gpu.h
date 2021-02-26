/*!
 * @file vpp_ip_gpu.h
 *
 * @cr
 * Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 * @services
 */

#ifndef _VPP_IP_GPU_H_
#define _VPP_IP_GPU_H_

/***************************************************************************
 * Function Prototypes
 ***************************************************************************/

void * vpVppIpGpu_Init(t_StVppCtx *pstCtx, uint32_t u32Flags, t_StVppCallback cbs);
void vVppIpGpu_Term(void *ctx);
uint32_t u32VppIpGpu_Open(void *ctx);
uint32_t u32VppIpGpu_Close(void *ctx);
uint32_t u32VppIpGpu_SetParam(void *ctx, enum vpp_port port,
                              struct vpp_port_param param);
uint32_t u32VppIpGpu_SetCtrl(void *ctx, struct hqv_control ctrl);
uint32_t u32VppIpGpu_GetBufferRequirements(void *ctx,
                                           t_StVppIpBufReq *pstInputBufReq,
                                           t_StVppIpBufReq *pstOutputBufReq);
uint32_t u32VppIpGpu_QueueBuf(void *ctx, enum vpp_port ePort,
                              t_StVppBuf *pBuf);
uint32_t u32VppIpGpu_Flush(void *ctx, enum vpp_port ePort);

uint32_t u32VppIpGpu_Drain(void *ctx);

uint32_t u32VppIpGpu_Reconfigure(void *ctx,
                                 struct vpp_port_param in_param,
                                 struct vpp_port_param out_param);

#endif /* _VPP_IP_GPU_H_ */
