/*
 * Copyright (c) 2016 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */

// This is an auto-generated file from input/sysCmdPHYDBGdump.s
#include "tlv2Inc.h"
#include "sysCmdPHYDBGdump.h"

void* initPHYDBGDUMPOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict)
{
    int i; 	//for initializing array parameter
    SYSCMD_PHYDBGDUMP_PARMS  *pPHYDBGDUMPParms = (SYSCMD_PHYDBGDUMP_PARMS *)pParmsCommon;

    if (pParmsCommon == NULL) return (NULL);

    i = 0;	//assign a number to avoid warning in case i is not used

    // Populate the parm structure with initial values
    pPHYDBGDUMPParms->PHYDBGdumpOption = pParmDict[SYSPARM_PHYDBGDUMPOPTION-SYSPARM_FIRST_IDX].v.valU32;
    pPHYDBGDUMPParms->phyId = pParmDict[SYSPARM_PHYID-SYSPARM_FIRST_IDX].v.valU32;

    // Make up ParmOffsetTbl
    resetParmOffsetFields();
    fillParmOffsetTbl((A_UINT32)SYSPARM_PHYDBGDUMPOPTION, (A_UINT32)(((A_UINT8 *)&(pPHYDBGDUMPParms->PHYDBGdumpOption)) - (A_UINT8 *)pPHYDBGDUMPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)SYSPARM_PHYID, (A_UINT32)(((A_UINT8 *)&(pPHYDBGDUMPParms->phyId)) - (A_UINT8 *)pPHYDBGDUMPParms), pParmsOffset);
    return((void*) pPHYDBGDUMPParms);
}

static PHYDBGDUMP_OP_FUNC PHYDBGDUMPOpFunc = NULL;

TLV2_API void registerPHYDBGDUMPHandler(PHYDBGDUMP_OP_FUNC fp)
{
    PHYDBGDUMPOpFunc = fp;
}

A_BOOL PHYDBGDUMPOp(void *pParms)
{
    SYSCMD_PHYDBGDUMP_PARMS *pPHYDBGDUMPParms = (SYSCMD_PHYDBGDUMP_PARMS *)pParms;

#if 0 //for debugging, comment out this line, and uncomment the line below
//#ifdef _DEBUG
    int i; 	//for initializing array parameter
    i = 0;	//assign a number to avoid warning in case i is not used

    A_PRINTF("PHYDBGDUMPOp: PHYDBGdumpOption %u\n", pPHYDBGDUMPParms->PHYDBGdumpOption);
    A_PRINTF("PHYDBGDUMPOp: phyId %u\n", pPHYDBGDUMPParms->phyId);
#endif //_DEBUG

    if (NULL != PHYDBGDUMPOpFunc) {
        (*PHYDBGDUMPOpFunc)(pPHYDBGDUMPParms);
    }
    return(TRUE);
}

void* initPHYDBGDUMPRSPOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict)
{
    int i; 	//for initializing array parameter
    SYSCMD_PHYDBGDUMPRSP_PARMS  *pPHYDBGDUMPRSPParms = (SYSCMD_PHYDBGDUMPRSP_PARMS *)pParmsCommon;

    if (pParmsCommon == NULL) return (NULL);

    i = 0;	//assign a number to avoid warning in case i is not used

    // Populate the parm structure with initial values
    pPHYDBGDUMPRSPParms->status32 = pParmDict[SYSPARM_STATUS32-SYSPARM_FIRST_IDX].v.valU32;
    pPHYDBGDUMPRSPParms->phyId = pParmDict[SYSPARM_PHYID-SYSPARM_FIRST_IDX].v.valU32;
    memset(pPHYDBGDUMPRSPParms->data1, 0, sizeof(pPHYDBGDUMPRSPParms->data1));
    memset(pPHYDBGDUMPRSPParms->data2, 0, sizeof(pPHYDBGDUMPRSPParms->data2));
    memset(pPHYDBGDUMPRSPParms->data3, 0, sizeof(pPHYDBGDUMPRSPParms->data3));
    memset(pPHYDBGDUMPRSPParms->data4, 0, sizeof(pPHYDBGDUMPRSPParms->data4));

    // Make up ParmOffsetTbl
    resetParmOffsetFields();
    fillParmOffsetTbl((A_UINT32)SYSPARM_STATUS32, (A_UINT32)(((A_UINT8 *)&(pPHYDBGDUMPRSPParms->status32)) - (A_UINT8 *)pPHYDBGDUMPRSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)SYSPARM_PHYID, (A_UINT32)(((A_UINT8 *)&(pPHYDBGDUMPRSPParms->phyId)) - (A_UINT8 *)pPHYDBGDUMPRSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)SYSPARM_DATA1, (A_UINT32)(((A_UINT8 *)&(pPHYDBGDUMPRSPParms->data1)) - (A_UINT8 *)pPHYDBGDUMPRSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)SYSPARM_DATA2, (A_UINT32)(((A_UINT8 *)&(pPHYDBGDUMPRSPParms->data2)) - (A_UINT8 *)pPHYDBGDUMPRSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)SYSPARM_DATA3, (A_UINT32)(((A_UINT8 *)&(pPHYDBGDUMPRSPParms->data3)) - (A_UINT8 *)pPHYDBGDUMPRSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)SYSPARM_DATA4, (A_UINT32)(((A_UINT8 *)&(pPHYDBGDUMPRSPParms->data4)) - (A_UINT8 *)pPHYDBGDUMPRSPParms), pParmsOffset);
    return((void*) pPHYDBGDUMPRSPParms);
}

static PHYDBGDUMPRSP_OP_FUNC PHYDBGDUMPRSPOpFunc = NULL;

TLV2_API void registerPHYDBGDUMPRSPHandler(PHYDBGDUMPRSP_OP_FUNC fp)
{
    PHYDBGDUMPRSPOpFunc = fp;
}

A_BOOL PHYDBGDUMPRSPOp(void *pParms)
{
    SYSCMD_PHYDBGDUMPRSP_PARMS *pPHYDBGDUMPRSPParms = (SYSCMD_PHYDBGDUMPRSP_PARMS *)pParms;

#if 0 //for debugging, comment out this line, and uncomment the line below
//#ifdef _DEBUG
    int i; 	//for initializing array parameter
    i = 0;	//assign a number to avoid warning in case i is not used

    A_PRINTF("PHYDBGDUMPRSPOp: status32 %u\n", pPHYDBGDUMPRSPParms->status32);
    A_PRINTF("PHYDBGDUMPRSPOp: phyId %u\n", pPHYDBGDUMPRSPParms->phyId);
    for (i = 0; i < 8 ; i++) // can be modified to print up to 1024 entries
    {
        A_PRINTF("PHYDBGDUMPRSPOp: data1 0x%x\n", pPHYDBGDUMPRSPParms->data1[i]);
    }
    for (i = 0; i < 8 ; i++) // can be modified to print up to 1024 entries
    {
        A_PRINTF("PHYDBGDUMPRSPOp: data2 0x%x\n", pPHYDBGDUMPRSPParms->data2[i]);
    }
    for (i = 0; i < 8 ; i++) // can be modified to print up to 1024 entries
    {
        A_PRINTF("PHYDBGDUMPRSPOp: data3 0x%x\n", pPHYDBGDUMPRSPParms->data3[i]);
    }
    for (i = 0; i < 8 ; i++) // can be modified to print up to 1024 entries
    {
        A_PRINTF("PHYDBGDUMPRSPOp: data4 0x%x\n", pPHYDBGDUMPRSPParms->data4[i]);
    }
#endif //_DEBUG

    if (NULL != PHYDBGDUMPRSPOpFunc) {
        (*PHYDBGDUMPRSPOpFunc)(pPHYDBGDUMPRSPParms);
    }
    return(TRUE);
}
