#ifndef __CVD_VERSION_H__
#define __CVD_VERSION_H__

/*
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/avs.adsp/2.8.5/vsd/inc/protected/cvd_version.h#1 $
  $Author: pwbldsvc $
*/

#include "adsp_core_api.h"
#include "adsp_error_codes.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/** Returns MVM Version

    @param [out] svc_info_ptr pointer to the service info structure
    @return ADSP_EOK
*/
ADSPResult mvm_svc_get_api_version(avs_svc_api_info_t *svc_info_ptr);

/** Returns CVS Version

    @param [out] svc_info_ptr pointer to the service info structure
    @return ADSP_EOK
*/
ADSPResult cvs_svc_get_api_version(avs_svc_api_info_t *svc_info_ptr);

/** Returns CVP Version

    @param [out] svc_info_ptr pointer to the service info structure
    @return ADSP_EOK
*/
ADSPResult cvp_svc_get_api_version(avs_svc_api_info_t *svc_info_ptr);

#ifdef __cplusplus
}
#endif //__cplusplus


#endif  /* __CVD_VERSION_H__ */
