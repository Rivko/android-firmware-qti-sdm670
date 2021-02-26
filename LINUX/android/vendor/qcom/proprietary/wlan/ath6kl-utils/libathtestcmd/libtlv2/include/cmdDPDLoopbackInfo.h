/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 *
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

// This is an auto-generated file from input\cmdDPDLoopbackInfo.s
#ifndef _CMDDPDLOOPBACKINFO_H_
#define _CMDDPDLOOPBACKINFO_H_

#if defined(__cplusplus) || defined(__cplusplus__)
extern "C" {
#endif

#if defined(WIN32) || defined(WIN64)
#pragma pack (push, 1)
#endif //WIN32 || WIN64

typedef struct dpdloopbackinfo_parms {
    A_UINT8	phyId;
    A_UINT16	chainMask;
    A_UINT8	pad[1];
} __ATTRIB_PACK CMD_DPDLOOPBACKINFO_PARMS;

typedef struct dpdloopbackinforsp_parms {
    A_UINT8	status;
    A_UINT8	phyId;
    A_UINT16	chainMask;
    A_UINT8	dpd_lb_fail;
    A_UINT8	rx_gain;
    A_UINT8	max_bin;
    A_INT8	max_bin_db;
    A_INT16	agc2_pwr;
    A_INT16	dc_i;
    A_INT16	dc_q;
    A_UINT16	sq;
    A_UINT8	sq_idx;
    A_UINT8	pad[3];
} __ATTRIB_PACK CMD_DPDLOOPBACKINFORSP_PARMS;

typedef void (*DPDLOOPBACKINFO_OP_FUNC)(void *pParms);
typedef void (*DPDLOOPBACKINFORSP_OP_FUNC)(void *pParms);

// Exposed functions

void* initDPDLOOPBACKINFOOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict);
A_BOOL DPDLOOPBACKINFOOp(void *pParms);

void* initDPDLOOPBACKINFORSPOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict);
A_BOOL DPDLOOPBACKINFORSPOp(void *pParms);

#if defined(WIN32) || defined(WIN64)
#pragma pack(pop)
#endif //WIN32 || WIN64


#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif

#endif //_CMDDPDLOOPBACKINFO_H_
