/*
 * Copyright (c) 2016 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */

// This is an auto-generated file from input/sysCmdtlvCaptureCtrl.s
#ifndef _SYSCMDTLVCAPTURECTRL_H_
#define _SYSCMDTLVCAPTURECTRL_H_

#if defined(__cplusplus) || defined(__cplusplus__)
extern "C" {
#endif

#if defined(WIN32) || defined(WIN64)
#pragma pack (push, 1)
#endif //WIN32 || WIN64

typedef struct tlvcapturectrl_parms {
    A_UINT32	tlvCaptureCtrl_cmdID;
    A_UINT32	tlvCaptureCtrl_01;
    A_UINT32	tlvCaptureCtrl_02;
    A_UINT32	tlvCaptureCtrl_03;
    A_UINT32	tlvCaptureCtrl_04;
    A_UINT32	phyId;
} __ATTRIB_PACK SYSCMD_TLVCAPTURECTRL_PARMS;

typedef struct tlvcapturectrlrsp_parms {
    A_UINT32	tlvCaptureCtrl_cmdID;
    A_UINT32	tlvCaptureCtrl_01;
    A_UINT32	tlvCaptureCtrl_02;
    A_UINT32	tlvCaptureCtrl_03;
    A_UINT32	tlvCaptureCtrl_04;
    A_UINT32	phyId;
} __ATTRIB_PACK SYSCMD_TLVCAPTURECTRLRSP_PARMS;

typedef void (*TLVCAPTURECTRL_OP_FUNC)(void *pParms);
typedef void (*TLVCAPTURECTRLRSP_OP_FUNC)(void *pParms);

// Exposed functions

void* initTLVCAPTURECTRLOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict);
A_BOOL TLVCAPTURECTRLOp(void *pParms);

void* initTLVCAPTURECTRLRSPOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict);
A_BOOL TLVCAPTURECTRLRSPOp(void *pParms);

#if defined(WIN32) || defined(WIN64)
#pragma pack(pop)
#endif //WIN32 || WIN64


#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif

#endif //_SYSCMDTLVCAPTURECTRL_H_
