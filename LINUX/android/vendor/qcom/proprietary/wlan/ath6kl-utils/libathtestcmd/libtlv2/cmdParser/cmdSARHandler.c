/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "tlv2Inc.h"
#include "cmdSARHandler.h"

void* initSAROpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict)
{
    int i, j; 	//for initializing array parameter
    CMD_SAR_PARMS  *pSARParms = (CMD_SAR_PARMS *)pParmsCommon;

    if (pParmsCommon == NULL) return (NULL);

    i = j = 0;	//assign a number to avoid warning in case i and j are not used

    // Populate the parm structure with initial values
    pSARParms->cmdId = pParmDict[PARM_CMDID].v.valU16;
    pSARParms->CCK2gLimit = pParmDict[PARM_CCK2GLIMIT].v.valU8;
    pSARParms->Ofdm2gLimit = pParmDict[PARM_OFDM2GLIMIT].v.valU8;
    pSARParms->Ofdm5gLimit = pParmDict[PARM_OFDM5GLIMIT].v.valU8;
    pSARParms->chain = pParmDict[PARM_CHAIN].v.valU8;
    pSARParms->index8 = pParmDict[PARM_INDEX8].v.valU8;

    // Make up ParmOffsetTbl
    resetParmOffsetFields();
    fillParmOffsetTbl((A_UINT32)PARM_CMDID, (A_UINT32)(((A_UINT8 *)&(pSARParms->cmdId)) - (A_UINT8 *)pSARParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_CCK2GLIMIT, (A_UINT32)(((A_UINT8 *)&(pSARParms->CCK2gLimit)) - (A_UINT8 *)pSARParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_OFDM2GLIMIT, (A_UINT32)(((A_UINT8 *)&(pSARParms->Ofdm2gLimit)) - (A_UINT8 *)pSARParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_OFDM5GLIMIT, (A_UINT32)(((A_UINT8 *)&(pSARParms->Ofdm5gLimit)) - (A_UINT8 *)pSARParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_CHAIN, (A_UINT32)(((A_UINT8 *)&(pSARParms->chain)) - (A_UINT8 *)pSARParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_INDEX8, (A_UINT32)(((A_UINT8 *)&(pSARParms->index8)) - (A_UINT8 *)pSARParms), pParmsOffset);
    return((void*) pSARParms);
}

static SAR_OP_FUNC SAROpFunc = NULL;

TLV2_API void registerSARHandler(SAR_OP_FUNC fp)
{
    SAROpFunc = fp;
}

A_BOOL SAROp(void *pParms)
{
    CMD_SAR_PARMS *pSARParms = (CMD_SAR_PARMS *)pParms;

#if 0 //for debugging, comment out this line, and uncomment the line below
//#ifdef _DEBUG
    int i; 	//for initializing array parameter
    i = 0;	//assign a number to avoid warning in case i is not used

    A_PRINTF("SAROp: cmdId %u\n", pSARParms->cmdId);
    A_PRINTF("SAROp: CCK2gLimit %u\n", pSARParms->CCK2gLimit);
    A_PRINTF("SAROp: Ofdm2gLimit %u\n", pSARParms->Ofdm2gLimit);
    A_PRINTF("SAROp: Ofdm5gLimit %u\n", pSARParms->Ofdm5gLimit);
    A_PRINTF("SAROp: chain %u\n", pSARParms->chain);
    A_PRINTF("SAROp: index8 %u\n", pSARParms->index8);
#endif //_DEBUG

    if (NULL != SAROpFunc) {
        (*SAROpFunc)(pSARParms);
    }
    return(TRUE);
}

void* initSARRSPOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict)
{
    int i, j; 	//for initializing array parameter
    CMD_SARRSP_PARMS  *pSARRSPParms = (CMD_SARRSP_PARMS *)pParmsCommon;

    if (pParmsCommon == NULL) return (NULL);

    i = j = 0;	//assign a number to avoid warning in case i and j are not used

    // Populate the parm structure with initial values
    pSARRSPParms->status = pParmDict[PARM_STATUS].v.valU8;
    pSARRSPParms->CCK2gLimit = pParmDict[PARM_CCK2GLIMIT].v.valU8;
    pSARRSPParms->Ofdm2gLimit = pParmDict[PARM_OFDM2GLIMIT].v.valU8;
    pSARRSPParms->Ofdm5gLimit = pParmDict[PARM_OFDM5GLIMIT].v.valU8;

    // Make up ParmOffsetTbl
    resetParmOffsetFields();
    fillParmOffsetTbl((A_UINT32)PARM_STATUS, (A_UINT32)(((A_UINT8 *)&(pSARRSPParms->status)) - (A_UINT8 *)pSARRSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_CCK2GLIMIT, (A_UINT32)(((A_UINT8 *)&(pSARRSPParms->CCK2gLimit)) - (A_UINT8 *)pSARRSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_OFDM2GLIMIT, (A_UINT32)(((A_UINT8 *)&(pSARRSPParms->Ofdm2gLimit)) - (A_UINT8 *)pSARRSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_OFDM5GLIMIT, (A_UINT32)(((A_UINT8 *)&(pSARRSPParms->Ofdm5gLimit)) - (A_UINT8 *)pSARRSPParms), pParmsOffset);
    return((void*) pSARRSPParms);
}

static SARRSP_OP_FUNC SARRSPOpFunc = NULL;

TLV2_API void registerSARRSPHandler(SARRSP_OP_FUNC fp)
{
    SARRSPOpFunc = fp;
}

A_BOOL SARRSPOp(void *pParms)
{
    CMD_SARRSP_PARMS *pSARRSPParms = (CMD_SARRSP_PARMS *)pParms;

#if 0 //for debugging, comment out this line, and uncomment the line below
//#ifdef _DEBUG
    int i; 	//for initializing array parameter
    i = 0;	//assign a number to avoid warning in case i is not used

    A_PRINTF("SARRSPOp: status %u\n", pSARRSPParms->status);
    A_PRINTF("SARRSPOp: CCK2gLimit %u\n", pSARRSPParms->CCK2gLimit);
    A_PRINTF("SARRSPOp: Ofdm2gLimit %u\n", pSARRSPParms->Ofdm2gLimit);
    A_PRINTF("SARRSPOp: Ofdm5gLimit %u\n", pSARRSPParms->Ofdm5gLimit);
#endif //_DEBUG

    if (NULL != SARRSPOpFunc) {
        (*SARRSPOpFunc)(pSARRSPParms);
    }
    return(TRUE);
}
