/*
 * Copyright (c) 2016 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */

// This is an auto-generated file from input/sysCmdPHYDBGdump.s
#ifndef _SYSCMDPHYDBGDUMP_H_
#define _SYSCMDPHYDBGDUMP_H_

#if defined(__cplusplus) || defined(__cplusplus__)
extern "C" {
#endif

#if defined(WIN32) || defined(WIN64)
#pragma pack (push, 1)
#endif //WIN32 || WIN64

typedef struct phydbgdump_parms {
    A_UINT32	PHYDBGdumpOption;
    A_UINT32	phyId;
} __ATTRIB_PACK SYSCMD_PHYDBGDUMP_PARMS;

typedef struct phydbgdumprsp_parms {
    A_UINT32	status32;
    A_UINT32	phyId;
    A_UINT8	data1[1024];
    A_UINT8	data2[1024];
    A_UINT8	data3[1024];
    A_UINT8	data4[1024];
} __ATTRIB_PACK SYSCMD_PHYDBGDUMPRSP_PARMS;

typedef void (*PHYDBGDUMP_OP_FUNC)(void *pParms);
typedef void (*PHYDBGDUMPRSP_OP_FUNC)(void *pParms);

// Exposed functions

void* initPHYDBGDUMPOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict);
A_BOOL PHYDBGDUMPOp(void *pParms);

void* initPHYDBGDUMPRSPOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict);
A_BOOL PHYDBGDUMPRSPOp(void *pParms);

#if defined(WIN32) || defined(WIN64)
#pragma pack(pop)
#endif //WIN32 || WIN64


#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif

#endif //_SYSCMDPHYDBGDUMP_H_
