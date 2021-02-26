/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 *
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

// This is an auto-generated file from input\cmdDPDLoopbackInfo.s
#include "tlv2Inc.h"
#include "cmdDPDLoopbackInfo.h"

void* initDPDLOOPBACKINFOOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict)
{
    int i, j; 	//for initializing array parameter
    CMD_DPDLOOPBACKINFO_PARMS  *pDPDLOOPBACKINFOParms = (CMD_DPDLOOPBACKINFO_PARMS *)pParmsCommon;

    if (pParmsCommon == NULL) return (NULL);

    i = j = 0;	//assign a number to avoid warning in case i and j are not used

    // Populate the parm structure with initial values
    pDPDLOOPBACKINFOParms->phyId = pParmDict[PARM_PHYID].v.valU8;
    pDPDLOOPBACKINFOParms->chainMask = pParmDict[PARM_CHAINMASK].v.valU16;

    // Make up ParmOffsetTbl
    resetParmOffsetFields();
    fillParmOffsetTbl((A_UINT32)PARM_PHYID, (A_UINT32)(((A_UINT32)&(pDPDLOOPBACKINFOParms->phyId)) - (A_UINT32)pDPDLOOPBACKINFOParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_CHAINMASK, (A_UINT32)(((A_UINT32)&(pDPDLOOPBACKINFOParms->chainMask)) - (A_UINT32)pDPDLOOPBACKINFOParms), pParmsOffset);
    return((void*) pDPDLOOPBACKINFOParms);
}

static DPDLOOPBACKINFO_OP_FUNC DPDLOOPBACKINFOOpFunc = NULL;

TLV2_API void registerDPDLOOPBACKINFOHandler(DPDLOOPBACKINFO_OP_FUNC fp)
{
    DPDLOOPBACKINFOOpFunc = fp;
}

A_BOOL DPDLOOPBACKINFOOp(void *pParms)
{
    CMD_DPDLOOPBACKINFO_PARMS *pDPDLOOPBACKINFOParms = (CMD_DPDLOOPBACKINFO_PARMS *)pParms;

#if 0 //for debugging, comment out this line, and uncomment the line below
//#ifdef _DEBUG
    int i; 	//for initializing array parameter
    i = 0;	//assign a number to avoid warning in case i is not used

    A_PRINTF("DPDLOOPBACKINFOOp: phyId %u\n", pDPDLOOPBACKINFOParms->phyId);
    A_PRINTF("DPDLOOPBACKINFOOp: chainMask 0x%x\n", pDPDLOOPBACKINFOParms->chainMask);
#endif //_DEBUG

    if (NULL != DPDLOOPBACKINFOOpFunc) {
        (*DPDLOOPBACKINFOOpFunc)(pDPDLOOPBACKINFOParms);
    }
    return(TRUE);
}

void* initDPDLOOPBACKINFORSPOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict)
{
    int i, j; 	//for initializing array parameter
    CMD_DPDLOOPBACKINFORSP_PARMS  *pDPDLOOPBACKINFORSPParms = (CMD_DPDLOOPBACKINFORSP_PARMS *)pParmsCommon;

    if (pParmsCommon == NULL) return (NULL);

    i = j = 0;	//assign a number to avoid warning in case i and j are not used

    // Populate the parm structure with initial values
    pDPDLOOPBACKINFORSPParms->status = pParmDict[PARM_STATUS].v.valU8;
    pDPDLOOPBACKINFORSPParms->phyId = pParmDict[PARM_PHYID].v.valU8;
    pDPDLOOPBACKINFORSPParms->chainMask = pParmDict[PARM_CHAINMASK].v.valU16;
    pDPDLOOPBACKINFORSPParms->dpd_lb_fail = pParmDict[PARM_DPD_LB_FAIL].v.valU8;
    pDPDLOOPBACKINFORSPParms->rx_gain = pParmDict[PARM_RX_GAIN].v.valU8;
    pDPDLOOPBACKINFORSPParms->max_bin = pParmDict[PARM_MAX_BIN].v.valU8;
    pDPDLOOPBACKINFORSPParms->max_bin_db = pParmDict[PARM_MAX_BIN_DB].v.valS8;
    pDPDLOOPBACKINFORSPParms->agc2_pwr = pParmDict[PARM_AGC2_PWR].v.valS16;
    pDPDLOOPBACKINFORSPParms->dc_i = pParmDict[PARM_DC_I].v.valS16;
    pDPDLOOPBACKINFORSPParms->dc_q = pParmDict[PARM_DC_Q].v.valS16;
    pDPDLOOPBACKINFORSPParms->sq = pParmDict[PARM_SQ].v.valU16;
    pDPDLOOPBACKINFORSPParms->sq_idx = pParmDict[PARM_SQ_IDX].v.valU8;

    // Make up ParmOffsetTbl
    resetParmOffsetFields();
    fillParmOffsetTbl((A_UINT32)PARM_STATUS, (A_UINT32)(((A_UINT32)&(pDPDLOOPBACKINFORSPParms->status)) - (A_UINT32)pDPDLOOPBACKINFORSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_PHYID, (A_UINT32)(((A_UINT32)&(pDPDLOOPBACKINFORSPParms->phyId)) - (A_UINT32)pDPDLOOPBACKINFORSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_CHAINMASK, (A_UINT32)(((A_UINT32)&(pDPDLOOPBACKINFORSPParms->chainMask)) - (A_UINT32)pDPDLOOPBACKINFORSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_DPD_LB_FAIL, (A_UINT32)(((A_UINT32)&(pDPDLOOPBACKINFORSPParms->dpd_lb_fail)) - (A_UINT32)pDPDLOOPBACKINFORSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_RX_GAIN, (A_UINT32)(((A_UINT32)&(pDPDLOOPBACKINFORSPParms->rx_gain)) - (A_UINT32)pDPDLOOPBACKINFORSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_MAX_BIN, (A_UINT32)(((A_UINT32)&(pDPDLOOPBACKINFORSPParms->max_bin)) - (A_UINT32)pDPDLOOPBACKINFORSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_MAX_BIN_DB, (A_UINT32)(((A_UINT32)&(pDPDLOOPBACKINFORSPParms->max_bin_db)) - (A_UINT32)pDPDLOOPBACKINFORSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_AGC2_PWR, (A_UINT32)(((A_UINT32)&(pDPDLOOPBACKINFORSPParms->agc2_pwr)) - (A_UINT32)pDPDLOOPBACKINFORSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_DC_I, (A_UINT32)(((A_UINT32)&(pDPDLOOPBACKINFORSPParms->dc_i)) - (A_UINT32)pDPDLOOPBACKINFORSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_DC_Q, (A_UINT32)(((A_UINT32)&(pDPDLOOPBACKINFORSPParms->dc_q)) - (A_UINT32)pDPDLOOPBACKINFORSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_SQ, (A_UINT32)(((A_UINT32)&(pDPDLOOPBACKINFORSPParms->sq)) - (A_UINT32)pDPDLOOPBACKINFORSPParms), pParmsOffset);
    fillParmOffsetTbl((A_UINT32)PARM_SQ_IDX, (A_UINT32)(((A_UINT32)&(pDPDLOOPBACKINFORSPParms->sq_idx)) - (A_UINT32)pDPDLOOPBACKINFORSPParms), pParmsOffset);
    return((void*) pDPDLOOPBACKINFORSPParms);
}

static DPDLOOPBACKINFORSP_OP_FUNC DPDLOOPBACKINFORSPOpFunc = NULL;

TLV2_API void registerDPDLOOPBACKINFORSPHandler(DPDLOOPBACKINFORSP_OP_FUNC fp)
{
    DPDLOOPBACKINFORSPOpFunc = fp;
}

A_BOOL DPDLOOPBACKINFORSPOp(void *pParms)
{
    CMD_DPDLOOPBACKINFORSP_PARMS *pDPDLOOPBACKINFORSPParms = (CMD_DPDLOOPBACKINFORSP_PARMS *)pParms;

#if 0 //for debugging, comment out this line, and uncomment the line below
//#ifdef _DEBUG
    int i; 	//for initializing array parameter
    i = 0;	//assign a number to avoid warning in case i is not used

    A_PRINTF("DPDLOOPBACKINFORSPOp: status %u\n", pDPDLOOPBACKINFORSPParms->status);
    A_PRINTF("DPDLOOPBACKINFORSPOp: phyId %u\n", pDPDLOOPBACKINFORSPParms->phyId);
    A_PRINTF("DPDLOOPBACKINFORSPOp: chainMask 0x%x\n", pDPDLOOPBACKINFORSPParms->chainMask);
    A_PRINTF("DPDLOOPBACKINFORSPOp: dpd_lb_fail %u\n", pDPDLOOPBACKINFORSPParms->dpd_lb_fail);
    A_PRINTF("DPDLOOPBACKINFORSPOp: rx_gain %u\n", pDPDLOOPBACKINFORSPParms->rx_gain);
    A_PRINTF("DPDLOOPBACKINFORSPOp: max_bin %u\n", pDPDLOOPBACKINFORSPParms->max_bin);
    A_PRINTF("DPDLOOPBACKINFORSPOp: max_bin_db %u\n", pDPDLOOPBACKINFORSPParms->max_bin_db);
    A_PRINTF("DPDLOOPBACKINFORSPOp: agc2_pwr %u\n", pDPDLOOPBACKINFORSPParms->agc2_pwr);
    A_PRINTF("DPDLOOPBACKINFORSPOp: dc_i %u\n", pDPDLOOPBACKINFORSPParms->dc_i);
    A_PRINTF("DPDLOOPBACKINFORSPOp: dc_q %u\n", pDPDLOOPBACKINFORSPParms->dc_q);
    A_PRINTF("DPDLOOPBACKINFORSPOp: sq %u\n", pDPDLOOPBACKINFORSPParms->sq);
    A_PRINTF("DPDLOOPBACKINFORSPOp: sq_idx %u\n", pDPDLOOPBACKINFORSPParms->sq_idx);
#endif //_DEBUG

    if (NULL != DPDLOOPBACKINFORSPOpFunc) {
        (*DPDLOOPBACKINFORSPOpFunc)(pDPDLOOPBACKINFORSPParms);
    }
    return(TRUE);
}
