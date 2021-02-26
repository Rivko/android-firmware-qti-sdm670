/*!
 * @file vpp_ip_hcp_hfi.h
 *
 * @cr
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services
 */

#ifndef _VPP_IP_HCP_HFI_H_
#define _VPP_IP_HCP_HFI_H_

/***************************************************************************
 * Function Prototypes
 ***************************************************************************/

// Handles
uint32_t u32VppIpHcp_HwGetHandle(t_StVppIpHcpGlobalCb *pstGlobal);
uint32_t u32VppIpHcp_HwReleaseHandle(t_StVppIpHcpGlobalCb *pstGlobal);

// GetInfo
uint32_t u32VppIpHcp_HwGlobalGetInfo(t_StVppIpHcpGlobalCb *pstGlobal,
                                     hcp_rpc_lock_action_t eAction);
uint32_t u32VppIpHcp_HwSessGetInfo(t_StVppIpHcpSessionCb *pstSessCb,
                                   hcp_rpc_lock_action_t eAction);

// Session Management
uint32_t u32VppIpHcp_HwOpen(t_StVppIpHcpSessionCb *pstSessCb);
uint32_t u32VppIpHcp_HwClose(t_StVppIpHcpSessionCb *pstSessCb);

// Response
uint32_t u32VppIpHcp_HwGlobalRecvPkt(t_StVppIpHcpGlobalCb *pstGlobal,
                                     t_StHcpHfiMsgPkt *pstMsg);

// Commands
uint32_t u32VppIpHcp_HwSessSendCmdNop(t_StVppIpHcpSessionCb *pstSessCb);
uint32_t u32VppIpHcp_HwGlobalSendCmdNop(t_StVppIpHcpGlobalCb *pstGlobal);

// Properties
uint32_t u32VppIpHcp_HwSessSetPropCp(t_StVppIpHcpSessionCb *pstSessCb,
                                     uint32_t bSecure);
uint32_t u32VppIpHcp_HwSessSetPropOpMode(t_StVppIpHcpSessionCb *pstSessCb,
                                         t_EVppIpHcpOpMode eOpMode);
uint32_t u32VppIpHcp_HwSessSetPropResHints(t_StVppIpHcpSessionCb *pstSessCb);
uint32_t u32VppIpHcp_HwSessSetPropKnobs(t_StVppIpHcpSessionCb *pstSessCb);
uint32_t u32VppIpHcp_HwSessSetPropDiagLevel(t_StVppIpHcpSessionCb *pstSessCb,
                                            uint32_t u32LogFlag);
uint32_t u32VppIpHcp_HwSessGetPropBufReq(t_StVppIpHcpSessionCb *pstSessCb);

// Buffers
uint32_t u32VppIpHcp_HwSessSendSessBuf(t_StVppIpHcpSessionCb *pstSessCb);
uint32_t u32VppIpHcp_HwSessSendBufs(t_StVppIpHcpSessionCb *pstSessCb,
                                    t_StVppBuf **ppstBufIn,
                                    uint32_t u32InCnt,
                                    t_StVppBuf **ppstBufOut,
                                    uint32_t u32OutCnt);
uint32_t u32VppIpHcp_HwGlobalSendSysBuf(t_StVppIpHcpGlobalCb *pstGlobal,
                                        t_StVppIpHcpSysBuf *pstBuf);

// Misc.
void vVppIpHcp_HwSetState(t_StVppIpHcpSessionCb *pstSessCb,
                          t_EVppIpHcpHwState eNewState);

void vVppIpHcp_ExtractUbwcStats(t_StVppBuf *pstBuf,
                                t_StHcpHfiPixBufAttr *pstPxAttr);

uint32_t u32VppIpHcp_VppToHfiCodec(enum vpp_codec_type eCodec);
#endif /* _VPP_IP_HCP_HFI_H_ */
