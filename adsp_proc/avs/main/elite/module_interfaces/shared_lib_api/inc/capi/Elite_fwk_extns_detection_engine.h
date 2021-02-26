#ifndef ELITE_FWK_EXTNS_DETECTION_ENGINE_H
#define ELITE_FWK_EXTNS_DETECTION_ENGINE_H

/**
  @file Elite_fwk_extns_detection_engine.h

  @brief Parameters required to be implemented by Detection Engine module

  This file defines a framework extensions for Detection Engine modules.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF. The description
      after the Edit History below is pulled in by the Elite_CAPIv2_mainpage.
      dox file and is displayed in the PDF. Contact Tech Pubs for assistance.
===========================================================================*/
/*=============================================================================
  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
=============================================================================*/
/*=============================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.5/elite/module_interfaces/shared_lib_api/inc/capi/Elite_fwk_extns_detection_engine.h#1 $

  when        who      what, where, why
  --------    ---      -------------------------------------------------------
  11/23/14    sw      (Tech Pubs) Edited Doxygen markup/comments for 2.6.
  08/05/14    Unni     Publishing as a framework extension in
                                    elite/module_interfaces/api/inc/
  07/07/14    Unni     derived by splitting listen/services/dynamic_svcs/
                                    listen_proc_svc/inc/ListenProcSvc_CustMsg.h
=============================================================================*/

/** @weakgroup weakf_capiv2_ext_detection_engine_intro

The #FWK_EXTN_DETECTION_ENGINE framework extension is used by modules that
support the detection engine feature. A detection engine is characterized by
its ability to process an audio stream and detect a signature such as a speech
keyword.

When a detection occurs during module processing, the module populates the
#PARAM_ID_DETECTION_ENGINE_STATUS parameter and raises an event to the aDSP
service via #CAPI_V2_EVENT_DATA_TO_DSP_SERVICE.

@note1hang The Listen Stream Manager (LSM) in the aDSP supports this 
           extension. For details on LSM parameters and events, refer 
           to @xrefcond{Q6,80-NF774-5,80-NA610-5}.
*/


#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/*------------------------------------------------------------------------------
 * Include Files
 *----------------------------------------------------------------------------*/
#include "Elite_CAPI_V2_types.h"


/** @addtogroup capiv2_ext_det_engine
@{ */

/** Unique identifier used to represent a module that supports the Detection
    Engine Framework extension.

    This extension supports the #PARAM_ID_DETECTION_ENGINE_STATUS parameter.
*/
#define FWK_EXTN_DETECTION_ENGINE 0x00012C0F


/*------------------------------------------------------------------------------
 * Parameter definitions - Needs implementation in module/library
 *----------------------------------------------------------------------------*/

/** ID of the parameter used to raise a detection engine status event.
    The endpoint of this event is the aDSP service.

    This parameter does the following:
    - Advertises status updates
    - Attaches the variable payload
    - Enables Debug mode

    @if OEM_only
    For example, the LSM and LAB services currently hosted in the aDSP support
    this extension as follows:
    - If a module returns #LSM_DETECTION_STATUS_DETECTED, the LSM service
      notifies LSM clients and votes for a hardware duty cycle reset. \n
      If the LAB session or another LSM session is active and sharing the same
      hardware, the votes are aggregated before they are applied.
    - If a module returns #LSM_DETECTION_STATUS_DISCOVERY and the LAB
      session is active, the data is buffered. This data is later flushed by
      removing the keyword from it once the keyword position is determined by
      #PARAM_ID_KW_END_POSITION.
    - If the module returns #LSM_DETECTION_STATUS_REJECTED, the LSM
      service votes for a hardware duty cycle reset. If the module sets
      is_debug_mode = TRUE, the LSM service notifies the client of the
      rejection status. @newpage
    @endif

    @msgpayload{detection_engine_status_t}
    @table{weak__detection__engine__status__t}

    @sa
    #CAPI_V2_EVENT_DATA_TO_DSP_SERVICE
*/
/* See also Elite_CAPI_V2_events.h */
#define PARAM_ID_DETECTION_ENGINE_STATUS 0x00012C10

/** @cond OEM_only */
/** Status (None) when a voice module is queried while it is initializing.

    This status is not forwarded to the client regardless of the is_debug_mode
    value. */
#define LSM_DETECTION_STATUS_NONE 0x0

/** Status (Detected) sent to notify the aDSP service that a voice module has
    detected a keyword.

    This status is forwarded to the client regardless of the is_debug_mode
    value. */
#define LSM_DETECTION_STATUS_DETECTED 0x2

/** Status (Rejected) sent to notify the aDSP service that a voice module has
    rejected a keyword.

    This status is forwarded to the client only if is_debug_mode is enabled
    (is_debug_mode = 1).*/
#define LSM_DETECTION_STATUS_REJECTED 0x4

/** Status (Discovery) sent to notify the aDSP service that a voice module is
    in Discovery mode. Discovery is a transitional or predetection state of the
    module.

    This status is not forwarded to the client regardless of the is_debug_mode
    value. */
#define LSM_DETECTION_STATUS_DISCOVERY 0x5

/** @endcond */


/** @weakgroup weak_detection_engine_status_t
@{ */
/* Data structure for querying detection engine status */
typedef struct
{
  uint64_t detection_timestamp;
  /**< Timestamp when an event is detected.

       @values Any valid 64-bit timestamp */

  bool_t is_timestamp_valid;
  /**< Specifies whether a timestamp is valid.

       @values
       - 0 -- Not valid
       - 1 -- Valid @tablebulletend*/

  bool_t is_debug_mode;
  /**< Specifies whether debugging is enabled.

       @values
       - 0 -- FALSE
       - 1 -- TRUE

       When set to TRUE, the aDSP service forwards additional events to HLOS
       application clients. */

  uint8_t status;
  /**< Status returned by the algorithm.

       @if OEM_only
       @values
       - #LSM_DETECTION_STATUS_NONE
       - #LSM_DETECTION_STATUS_DETECTED
       - #LSM_DETECTION_STATUS_REJECTED
       - #LSM_DETECTION_STATUS_DISCOVERY @tablebulletend
       @endif */

  uint8_t payload_size;
  /**< Size (in bytes) of the payload blob that the algorithm returns. */

  int8_t *payload_ptr;
  /**< Pointer to the payload blob; interpreted by HLOS application clients. */

}detection_engine_status_t;
/** @} *//* end_weakgroup weak_detection_engine_status_t */

/** @} *//* end_addtogroup capiv2_ext_det_engine */

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* #ifndef ELITE_FWK_EXTNS_DETECTION_ENGINE_H */
