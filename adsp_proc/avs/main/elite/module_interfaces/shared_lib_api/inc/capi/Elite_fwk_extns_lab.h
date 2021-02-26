#ifndef ELITE_FWK_EXTNS_LAB_H
#define ELITE_FWK_EXTNS_LAB_H

/**
  @file Elite_fwk_extns_lab.h

  @brief Parameters required to be implemented for Look Ahead Buffering(LAB)

  This file defines a framework extensions and corresponding private messages
  for Look Ahead Buffering(LAB).
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF. The description
      after the Edit History below is pulled in by the Elite_CAPIv2_mainpage.
      dox file and is displayed in the PDF. Contact Tech Pubs for assistance.
===========================================================================*/
/*=============================================================================
  Copyright (c) 2014 Qualcomm Technologies, Inc.  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
=============================================================================*/
/*=============================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.5/elite/module_interfaces/shared_lib_api/inc/capi/Elite_fwk_extns_lab.h#1 $

  when        who      what, where, why
  --------    ---      -------------------------------------------------------
  11/23/14    sw      (Tech Pubs) Edited Doxygen markup/comments for 2.6.
  08/05/14    SivaNaga   New
=============================================================================*/

/** @cond OEM_only */

/** @weakgroup weakf_capiv2_ext_lab_intro

The #FWK_EXTN_LAB framework extension is used by modules that support the Look
Ahead Buffer (LAB) feature. The LAB feature is a buffering mechanism that
enables delayed destination initialization.

@note1hang The LSM in the aDSP supports this extension. For details on LSM
           parameters and events, refer to@xrefcond{Q6,80-NF774-5,80-NA610-5}.

During module initialization, the module populates the
#PARAM_ID_KW_END_MAX_BUFFER_REQ parameter and uses
#CAPI_V2_EVENT_DATA_TO_DSP_SERVICE to raise an event to the aDSP service.

During processing, the module populates the #PARAM_ID_KW_END_POSITION parameter
and raises an event to the aDSP service via #CAPI_V2_EVENT_DATA_TO_DSP_SERVICE.
*/


#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/*------------------------------------------------------------------------------
 * Include Files
 *----------------------------------------------------------------------------*/
#include "Elite_CAPI_V2_types.h"


/** @addtogroup capiv2_ext_look_ahead_buf
@{ */

/** Unique identifier used to represent the LAB framework extension for the
    Listen Stream Manager feature.

    This extension supports the #PARAM_ID_KW_END_POSITION and
    #PARAM_ID_KW_END_MAX_BUFFER_REQ parameters.
*/
#define FWK_EXTN_LAB 0x00012C11

/*------------------------------------------------------------------------------
 * Parameter definitions - Needs implementation in module/library
 *----------------------------------------------------------------------------*/
 
/** ID of the parameter that indicates the keyword end position (in samples)
    from the end of the buffer for which detection happened.

    For example, in the Snapdragon@tm Voice Activation (SVA) solution, this
    parameter is used to omit the keyword before sending sending buffers to the
    HLOS. Other algorithms can use this extension in a similar way.

    @msgpayload{kw_end_position_t}
    @table{weak__kw__end__position__t}
*/
#define PARAM_ID_KW_END_POSITION   0x00012C12

/** @weakgroup weak_kw_end_position_t
@{ */
/* Data structure used when sending a keyword end position.
*/
typedef struct
{
  uint32_t kw_end_position_samples;
  /**< Keyword end position in samples.

       @values 0 to kw_end_max_delay_t::kw_end_max_delay_samples */

}kw_end_position_t;
/** @} *//* end_weakgroup weak_kw_end_position_t */


/** ID of the parameter that sends the required maximum buffer size of the
    keyword end position to the framework. The LAB uses this parameter to
    query for the maximum number of samples that it must buffer.

    @msgpayload{kw_end_max_delay_t}
    @table{weak__kw__end__max__delay__t}
*/
#define PARAM_ID_KW_END_MAX_BUFFER_REQ 0x00012C13

/** @weakgroup weak_kw_end_max_delay_t
@{ */
/* Data structure used when querying for the maximum number of samples that
    the Look Ahead Buffer needs to buffer.
*/
typedef struct
{
  uint32_t kw_end_max_delay_samples;
  /**< Maximum delay between the keyword end position and keyword detection in
       the stream.

       This delay is calculated in terms of samples starting from the frame for
       which a keyword is detected. */

}kw_end_max_delay_t;
/** @} *//* end_weakgroup weak_kw_end_max_delay_t */

/** @} *//* end_addtogroup capiv2_ext_look_ahead_buf */

/** @endcond */

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* #ifndef ELITE_FWK_EXTNS_LAB_H*/
