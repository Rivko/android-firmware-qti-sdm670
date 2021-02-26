#ifndef ELITE_FWK_EXTNS_DTMF_GEN_H
#define ELITE_FWK_EXTNS_DTMF_GEN_H

/**
  @file Elite_fwk_extns_dtmf_gen.h

  @brief Parameters required to be implemented by DTMF Gen module

  This file defines a framework extension for DTMF Gen modules.
*/

/*==============================================================================
  Copyright (c) 2016 Qualcomm Technologies, Inc.  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
==============================================================================*/

/*==============================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.5/elite/module_interfaces/shared_lib_api/inc/capi/Elite_fwk_extns_dtmf_gen.h#1 $

  when        who       what, where, why
  --------    ---       -------------------------------------------------------
  07/16/16    sw        (Tech Comm) Merged Doxygen comments from 8953.2.7.
  01/08/16    shridhar  Creation.
==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/*------------------------------------------------------------------------------
 * Include Files
 *----------------------------------------------------------------------------*/
#include "qurt_elite_types.h"

/** @addtogroup capiv2_ext_dtmf_gen
@{ */

/** Unique identifier of the DTMF Detection framework extension for a module. */
#define FWK_EXTN_DTMF_GEN                                 0x00010E9D

/** ID of the parameter used to send the DTMF tone generation payload to the
  module.

  @msgpayload
  vss_istream_cmd_set_dtmf_generation_t (refer to
  @xrefcond{Q8,80-NF774-31,80-NA610-31})
 */
 /* vss_istream_cmd_set_dtmf_generation_t replaces vsm_set_dtmf_generation_t */
#define FWK_EXTN_PARAM_ID_DTMF_GEN_SET_DTMF_GENERATION    0x00010E9E

/*------------------------------------------------------------------------------
 * Events
 *----------------------------------------------------------------------------*/

/** ID of the parameter used to raise a detection status event. The end point
  of this event is the aDSP service.

  @msgpayload
  None.

  @sa #CAPI_V2_EVENT_DATA_TO_DSP_SERVICE
*/
#define FWK_EXTN_EVENT_ID_DTMF_GEN_END_DETECTED           0x00010E9F

/** @} *//* end_addtogroup capiv2_ext_dtmf_gen */

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* #ifndef ELITE_FWK_EXTNS_DTMF_GEN_H */
