/*
 * Copyright (c) 2016 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */

// This is an auto-generated file from input/cmdCommitOtpStream.s
#include "tlv2Inc.h"
#include "cmdCommitOtpStream.h"

void* initCOMMITOTPSTREAMOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict)
{
    UNUSED(pParmsCommon);
    UNUSED(pParmsOffset);
    UNUSED(pParmDict);
    return(NULL);
}

static COMMITOTPSTREAM_OP_FUNC COMMITOTPSTREAMOpFunc = NULL;

TLV2_API void registerCOMMITOTPSTREAMHandler(COMMITOTPSTREAM_OP_FUNC fp)
{
    COMMITOTPSTREAMOpFunc = fp;
}

A_BOOL COMMITOTPSTREAMOp(void *pParms)
{
    UNUSED(pParms);
    if (NULL != COMMITOTPSTREAMOpFunc) {
        (*COMMITOTPSTREAMOpFunc)(NULL);
    }
    return(TRUE);
}

void* initCOMMITOTPSTREAMRSPOpParms(A_UINT8 *pParmsCommon, PARM_OFFSET_TBL *pParmsOffset, PARM_DICT *pParmDict)
{

    UNUSED(pParmsCommon);
    UNUSED(pParmsOffset);
    UNUSED(pParmDict);
    return(NULL);
}

static COMMITOTPSTREAMRSP_OP_FUNC COMMITOTPSTREAMRSPOpFunc = NULL;

TLV2_API void registerCOMMITOTPSTREAMRSPHandler(COMMITOTPSTREAMRSP_OP_FUNC fp)
{
    COMMITOTPSTREAMRSPOpFunc = fp;
}

A_BOOL COMMITOTPSTREAMRSPOp(void *pParms)
{
    UNUSED(pParms);
    if (NULL != COMMITOTPSTREAMRSPOpFunc) {
        (*COMMITOTPSTREAMRSPOpFunc)(NULL);
    }
    return(TRUE);
}
