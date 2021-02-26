#ifndef ELITE_FWK_EXTNS_DTMF_DETECTION_H
#define ELITE_FWK_EXTNS_DTMF_DETECTION_H

/**
  @file Elite_fwk_extns_dtmf_detection.h

  @brief Parameters required to be implemented by DTMF Detection module

  This file defines a framework extension for DTMF Detection modules.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF. The description
      after the Edit History below is pulled in by the Elite_CAPIv2_mainpage.
      dox file and is displayed in the PDF. Contact Tech Pubs for assistance.
===========================================================================*/
/*=============================================================================
  Copyright (c) 2015 Qualcomm Technologies, Inc. All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
=============================================================================*/
/*=============================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.5/elite/module_interfaces/shared_lib_api/inc/capi/Elite_fwk_extns_dtmf_detection.h#1 $

  when        who      what, where, why
  --------    ---      -------------------------------------------------------
  05/14/15    sw       (Tech Pubs) Edited Doxygen markup/comments for 8952.2.6.
  01/13/15    rojha     Publishing as a framework extension in
                                    elite/module_interfaces/api/inc/
=============================================================================*/

/** @cond OEM_only */

/** @weakgroup weakf_capiv2_ext_dtmf_detection_intro

The #FWK_EXTN_DTMF_DETECTION framework extension is used by modules that
interact with the aDSP service for KPPS and delay information using a unique
event ID (#CAPI_V2_EVENT_DATA_TO_DSP_SERVICE). This information is communicated
to the service via a common callback function registered with each module.

A module can use a generic event ID to interact with either the aDSP service
or the client. For DTMF detection, the event ID can be used notify the service
of a DTMF detection event. The service can further interact with the client
without being affected by CAPIv2 changes. Thus, these changes become backward
compatible.

@latexonly \subsection{DTMF detection event sequence} @endlatexonly

On each path, a tone must persist for at least 26.25 ms before it can be
detected. Once a tone is detected, it must transition through at least 13.125
ms of no-tone status before the next tone is fed into the audio path. A status
of new tone detected is reported only after another 26.25 ms of no-tone in the
audio path.
For more information, refer to @xhyperref{Q7,80-V9393-1}.

Because the CAPIv2 module is called once every 20 ms, every process call
injects 160 samples (for WB, after resampling) to the module. While the module
actually works on 105 samples (13.5 ms), the remaining samples are stored in
the module internal buffer. The module processes twice in some frames when
210 samples are present in its internal circular buffer.

Therefore, every 20 ms there are at most two tones that are put in the payload
to notify the service of the DTMF detection.
*/


#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/*------------------------------------------------------------------------------
 * Include Files
 *----------------------------------------------------------------------------*/
#include "qurt_elite_types.h"


/** @addtogroup capiv2_ext_dtmf_detect
@{ */

/** Unique identifier of the custom framework extension for DTMF detection. */
#define FWK_EXTN_DTMF_DETECTION 0x00010E5E

/*------------------------------------------------------------------------------
 * Parameter definitions - Needs implementation in module/library
 *----------------------------------------------------------------------------*/

/** ID of the parameter used to raise a detection status event. The endpoint
    of this event is the aDSP service.

    @msgpayload{dtmf_detection_payload_t}
    @table{weak__dtmf__detection__payload__t}

    @sa #CAPI_V2_EVENT_DATA_TO_DSP_SERVICE 
 */
#define PARAM_ID_DTMF_TONE_DETECTED 0x00010E5F

/** @weakgroup weak_dtmf_detection_payload_t
@{ */
/* Data structure for querying dtmf tone status */
typedef struct dtmf_detection_payload_t
{
  uint32_t version;
  /**< Version of the payload.

       @values 0 */

  /* Start of parameters for version 0 */

  uint32_t no_of_tones_detected;
  /**< Number of DTMF tone pairs detected by the module. */

  uint16_t low_freq[4];
  /**< Array of low frequencies (0 to 4) detected in each DTMF tone pair. */

  uint16_t high_freq[4];
  /**< Array of high frequencies (0 to 4) detected in each DTMF tone pair. */

}/** @cond */dtmf_detection_payload_t/** @endcond */;
/** @} *//* end_weakgroup weak_dtmf_detection_payload_t */

/** @} *//* end_addtogroup capiv2_ext_dtmf_detect */

/** @endcond *//* OEM_only */

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* #ifndef ELITE_FWK_EXTNS_DTMF_DETECTION_H */
