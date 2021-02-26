/*!
 * @file vpp_ip_hcp_dbg.h
 *
 * @cr
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * @services
 */

#ifndef _VPP_IP_HCP_DBG_H_
#define _VPP_IP_HCP_DBG_H_

#include "vpp_ip_hcp_priv.h"

#ifdef HCP_ENABLE_ASSERTS
#define HCP_ASSERT(_cond)       vVppIpHcpUtils_Assert((_cond), __LINE__, "VPP_ASSERT(" #_cond ")", __FILE__, __func__)
#else
#define HCP_ASSERT(_cond)
#endif

#ifdef HCP_DBG_WRITE_DUMP_BUF
#define HCP_DBG_WB_SIZE         32
typedef enum {
    HCP_FILL_TYPE_COUNTER,
    HCP_FILE_TYPE_AB,
} t_EVppIpHcpDbgFillType;
#endif

/***************************************************************************
 * Function Prototypes
 ***************************************************************************/

const char *pcGetKnobIdStr(uint32_t u32KnobId);
const char *pcGetFwCmdStr(uint32_t u32Cmd);
const char *pcGetMsgIdStr(uint32_t u32MsgId);
const char *pcGetPropIdStr(uint32_t u32PropId);

VPP_DBG_ENUM_DECL(pcGetHwStateStr);
VPP_DBG_ENUM_DECL(pcGetCmdStateStr);

void vVppIpHcpUtils_Assert(int cond, uint32_t line, const char * strCond,
                           const char * strFile, const char * strFunc);

#ifdef HCP_DBG_WRITE_DUMP_BUF
uint32_t u32VppIpHcpUtils_WriteToBuf(t_StVppBuf *pstBuf,
                                     t_EVppIpHcpDbgFillType eFt,
                                     uint32_t u32NumBytes);
uint32_t u32VppIpHcpUtils_DumpFromBuf(t_StVppBuf *pstBuf, uint32_t u32NumBytes);
#endif
#endif /* _VPP_IP_HCP_DBG_H_ */
