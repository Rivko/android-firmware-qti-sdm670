/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _CMDSARHANDLER_H_
#define _CMDSARHANDLER_H_

#if defined(__cplusplus) || defined(__cplusplus__)
extern "C" {
#endif

#if defined(WIN32) || defined(WIN64)
#pragma pack (push, 1)
#endif //WIN32 || WIN64

typedef struct sar_parms {
    A_UINT16	cmdId;
    A_UINT8	CCK2gLimit;
    A_UINT8	Ofdm2gLimit;
    A_UINT8	Ofdm5gLimit;
    A_UINT8	chain;
    A_UINT8	index8;
    A_UINT8	pad[1];
} __ATTRIB_PACK CMD_SAR_PARMS;

typedef struct sarrsp_parms {
    A_UINT8	status;
    A_UINT8	CCK2gLimit;
    A_UINT8	Ofdm2gLimit;
    A_UINT8	Ofdm5gLimit;
} __ATTRIB_PACK CMD_SARRSP_PARMS;

typedef void (*SAR_OP_FUNC)(void *pParms);
typedef void (*SARRSP_OP_FUNC)(void *pParms);

// Exposed functions

void* initSAROpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict);
A_BOOL SAROp(void *pParms);

void* initSARRSPOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict);
A_BOOL SARRSPOp(void *pParms);

#if defined(WIN32) || defined(WIN64)
#pragma pack(pop)
#endif //WIN32 || WIN64


#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif

#endif //_CMDSARHANDLER_H_
