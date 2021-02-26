#ifndef __VCCM_API_H__
#define __VCCM_API_H__

/*
  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/avs.adsp/2.8.5/vsd/inc/private/vccm_api.h#1 $
  $Author: pwbldsvc $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "apr_comdef.h"
#include "apr_errcodes.h"

/****************************************************************************
 * DEFINES                                                                  *
 ****************************************************************************/
/**
 * VCCM - Voice Clock Control Module client IDs.
 * Currently support is only available for only CVD Services( CVS/CVP/MVM).
 */
#define VCCM_CLIENT_ID_CVS ( 0 )
#define VCCM_CLIENT_ID_CVP ( 1 )
#define VCCM_CLIENT_ID_MVM ( 2 )

/****************************************************************************
 * PROTOTYPES                                                               *
 ****************************************************************************/

/**
 * Registers the VCCM client with ADSPPM for clock voting and devoting.
 * Must be called before any other VCCM APIs can be called, and must be
 * called only once.
 *
 * @param[in] VCCM client-Id.
 *
 * @return APR_EOK when successful.
 */
APR_INTERNAL uint32_t vccm_mmpm_register ( 
  uint32_t client_id
);


/**
 * De-registers VCCM client from ADSPPM.
 *
 * @param[in] VCCM client-Id.
 *
 * @return APR_EOK when successful.
 */
APR_INTERNAL uint32_t vccm_mmpm_deregister (
  uint32_t client_id
);

/**
 * Request for Minimal core/bus clocks and maximum bandwidth during session creation.
 * This API is not meant to be used during any other usecase, MVM_CCM
 * module takes care of the overall usecase voting required to run voice 
 * processing.
 *
 * @param[in] VCCM client_id.
 * @param[in] mpps.
 *
 * @return APR_EOK when successful.
 */
APR_INTERNAL uint32_t vccm_mmpm_request_clocks (
  uint32_t client_id,
  uint32_t mpps_req
);


/**
 * Release core/bus clocks when session is successfully created.
 * This API is not meant to be used during any other usecase, MVM_CCM
 * module takes care of the overall usecase voting required to run voice 
 * processing.
 *
 * @param[in] VCCM client-Id.
 *
 * @return APR_EOK when successful.
 */
APR_INTERNAL uint32_t vccm_mmpm_release_clocks ( 
  uint32_t client_id
);


#endif /* __VCCM_API_H__ */

