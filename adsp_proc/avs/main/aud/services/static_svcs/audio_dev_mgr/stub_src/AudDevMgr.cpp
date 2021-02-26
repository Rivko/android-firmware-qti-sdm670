/**
@file AudDevMgr.cpp
@brief This file contains the stubbed implementation for AudioDevMgr.

*/

/*========================================================================
Copyright (c) 2010-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "AudDevMgr.h"
#include "adsp_error_codes.h"
#include "adsp_core_api.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus
ADSPResult adm_svc_get_api_version(avs_svc_api_info_t *svc_info_ptr);
#ifdef __cplusplus
}
#endif //__cplusplus

/* =======================================================================
Function Definitions
========================================================================== */

ADSPResult adm_svc_get_api_version(avs_svc_api_info_t *svc_info_ptr)
{
  return ADSP_ENOTIMPL;
}

ADSPResult AudCreateStatAudDevMgr(uint32_t dummy, void **ppHAudDevMgr)
{
    *ppHAudDevMgr = NULL;
    return ADSP_EOK;
}
