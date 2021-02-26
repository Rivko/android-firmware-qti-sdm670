#ifndef ELITE_FWK_EXTNS_SERVICE_STATE_H
#define ELITE_FWK_EXTNS_SERVICE_STATE_H

/**
  @file Elite_fwk_extns_service_state.h

  @brief Parameters required to to know the service state.

  This file defines a framework extension for service state.
*/

/*==============================================================================
  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
==============================================================================*/

/*==============================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.5/elite/module_interfaces/shared_lib_api/inc/capi/Elite_fwk_extns_service_state.h#1 $

  when        who      what, where, why
  --------    ---      -------------------------------------------------------
  07/16/16    sw       (Tech Comm) Merged Doxygen comments from 8953.2.7.
  11/26/15    kn       Publishing as a framework extension in
                                    elite/module_interfaces/api/inc/
==============================================================================*/

/*------------------------------------------------------------------------------
 * Include Files
 *----------------------------------------------------------------------------*/
#include "qurt_elite_types.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/** @addtogroup capiv2_ext_svc_state
@{ */

/** Unique identifier of the Service State framework extension for a module. */
#define FWK_EXTN_SERVICE_STATE 0x00010E92

/*------------------------------------------------------------------------------
 * Parameter definitions - Needs implementation in module/library
 *----------------------------------------------------------------------------*/

/** ID of the parameter used to send service state information from an aDSP
  service to a module.
  The default state of the module should be the Stop state.

  @msgpayload{service_state_payload_t}
  @table{weak__service__state__payload__t}
 */
#define PARAM_ID_SERVICE_STATE 0x00010E93

/** @weakgroup weak_service_state_payload_t
@{ */
/* Data structure to send service state information from service to module
 */
typedef struct service_state_payload_t
{
  uint16_t main_state;
  /**< Indicates the main state of the service.

       @values See #service_main_state_id_t */

  uint16_t sub_state;
  /**< Reserved for future use. The service sets this parameter to 0. */

  /* Declare new sub state in service_sub_state_id_t */

}service_state_payload_t;
/** @} *//* end_weakgroup weak_service_state_payload_t */


/** Valid IDs for the main state of the service.
 */
typedef enum service_main_state_id_t
{
   SERVICE_STOP_STATE = 0,   /**< Service is in the Stop state. */
   SERVICE_RUN_STATE,        /**< Service is in the Run state. */
   SERVICE_MAX_MAIN_STATE = 0xFFFF /** Maximum number of states. */

}/** @cond */service_main_state_id_t/** @endcond */;


/** Valid IDs for the substate of the service.
 */
typedef enum service_sub_state_id_t
{
   SERVICE_MAX_SUB_STATE = 0xFFFF   /**< Maximum number of substates. */

}/** @cond */service_sub_state_id_t/** @endcond */;

/** @} *//* end_addtogroup capiv2_ext_svc_state */

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* #ifndef ELITE_FWK_EXTNS_SERVICE_STATE_H */
