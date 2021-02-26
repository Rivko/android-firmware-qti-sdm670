/*
 * Copyright (c) 2016 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */

// This is an auto-generated file from input\cmdSetPhyRfMode.s
#ifndef _CMDSETPHYRFMODE_H_
#define _CMDSETPHYRFMODE_H_

#if defined(__cplusplus) || defined(__cplusplus__)
extern "C" {
#endif

#if defined(WIN32) || defined(WIN64)
#pragma pack (push, 1)
#endif //WIN32 || WIN64

typedef struct setphyrfmode_parms {
    A_UINT8	phyRfMode;
    A_UINT8	pad[3];
} __ATTRIB_PACK CMD_SETPHYRFMODE_PARMS;

typedef void (*SETPHYRFMODE_OP_FUNC)(void *pParms);

// Exposed functions

void* initSETPHYRFMODEOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict);
A_BOOL SETPHYRFMODEOp(void *pParms);

#if defined(WIN32) || defined(WIN64)
#pragma pack(pop)
#endif //WIN32 || WIN64


#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif

#endif //_CMDSETPHYRFMODE_H_
