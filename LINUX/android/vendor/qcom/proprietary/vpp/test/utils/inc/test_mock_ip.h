/*!
 * @file test_mock_ip.h
 *
 * @cr
 * Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 * @services
 */

#ifndef _TEST_MOCK_IP_
#define _TEST_MOCK_IP_

enum mock_fail_point {
    eMockFail_Init,
};

#define MOCK_FAIL_INIT      (1 << eMockFail_Init)

/***************************************************************************
 * Function Prototypes
 ***************************************************************************/

void vVppIpMock_SetFailPoint(void *ctx, uint32_t u32FailFlags);
void * vpVppIpMock_Init(t_StVppCtx *pstCtx, uint32_t u32Flags, t_StVppCallback cbs);
void vVppIpMock_Term(void *ctx);
uint32_t u32VppIpMock_Open(void *ctx);
uint32_t u32VppIpMock_Close(void *ctx);
uint32_t u32VppIpMock_SetParam(void *ctx, enum vpp_port port,
                              struct vpp_port_param param);
uint32_t u32VppIpMock_SetCtrl(void *ctx, struct hqv_control ctrl);
uint32_t u32VppIpMock_GetBufferRequirements(void *ctx,
                                            t_StVppIpBufReq *pstInputBufReq,
                                            t_StVppIpBufReq *pstOutputBufReq);
uint32_t u32VppIpMock_QueueBuf(void *ctx, enum vpp_port ePort,
                              t_StVppBuf *pBuf);
uint32_t u32VppIpMock_Flush(void *ctx, enum vpp_port ePort);

uint32_t u32VppIpMock_Drain(void *ctx);

#endif /* _TEST_MOCK_IP_ */
