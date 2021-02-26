/*
  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All rights reserved.
  Qualcomm Technologies, Inc. Confidential and Proprietary

  $Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvd_utils/src/cvd_version.c#1 $
  $Author: pwbldsvc $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/
#include "cvd_adsp_version.h"
#include "adsp_core_api.h"
#include "aprv2_ids_services.h"
#include "adsp_error_codes.h"


 /****************************************************************************
 * FUNCTION DEFINITIONS                                                      *
 ****************************************************************************/

/** Returns MVM Version

    @param [out] svc_info_ptr pointer to the service info structure
    @return ADSP_EOK
*/
ADSPResult mvm_svc_get_api_version(avs_svc_api_info_t *svc_info_ptr)
{
  svc_info_ptr->service_id = APRV2_IDS_SERVICE_ID_ADSP_MVM_V;
  svc_info_ptr->api_version = MVM_API_VERSION;
  svc_info_ptr->api_branch_version = MVM_API_BRANCH_VERSION;

  return ADSP_EOK;
}

/** Returns CVS Version

    @param [out] svc_info_ptr pointer to the service info structure
    @return ADSP_EOK
*/
ADSPResult cvs_svc_get_api_version(avs_svc_api_info_t *svc_info_ptr)
{
  svc_info_ptr->service_id = APRV2_IDS_SERVICE_ID_ADSP_CVS_V;
  svc_info_ptr->api_version = CVS_API_VERSION;
  svc_info_ptr->api_branch_version = CVS_API_BRANCH_VERSION;

  return ADSP_EOK;
}


/** Returns CVP Version

    @param [out] svc_info_ptr pointer to the service info structure
    @return ADSP_EOK
*/
ADSPResult cvp_svc_get_api_version(avs_svc_api_info_t *svc_info_ptr)
{
  svc_info_ptr->service_id = APRV2_IDS_SERVICE_ID_ADSP_CVP_V;
  svc_info_ptr->api_version = CVP_API_VERSION;
  svc_info_ptr->api_branch_version = CVP_API_BRANCH_VERSION;

  return ADSP_EOK;
}

