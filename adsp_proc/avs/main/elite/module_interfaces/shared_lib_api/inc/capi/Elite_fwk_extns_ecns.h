#ifndef ELITE_FWK_EXTNS_ECNS_H
#define ELITE_FWK_EXTNS_ECNS_H

/* ======================================================================== */
/**
@file Elite_fwk_extns_ecns.h

@brief Frame work extensions for Echo canceller and Noise suppression (ECNS)

  This file defines a framework extensions and corresponding private propeties
  needed for ECNS in voice.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF. The description
      after the Edit History below is pulled in by the Elite_CAPIv2_mainpage.
      dox file and is displayed in the PDF. Contact Tech Pubs for assistance.
===========================================================================*/
/* =========================================================================
  Copyright (c) 2014-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
  ========================================================================== */
/* =========================================================================
                             Edit History

   when       who     what, where, why
   --------   ---     ------------------------------------------------------
   08/28/17    sw     (Tech Pubs) Edited Doxygen comments for AVS.2.8 (SDM845).
   07/16/16    sw     (Tech Pubs) Merged Doxygen comments from 8953.2.7;
                      edited Doxygen markup/comments for 8998.2.8.
   05/14/15    sw      (Tech Pubs) Edited Doxygen markup/comments for 8952.2.6.
   09/25/14   abhayg      Initial Version.
   ========================================================================= */

/** @cond OEM_only */

/** @weakgroup weakf_capiv2_ext_ecns_intro

The #FWK_EXTN_ECNS framework extension is used by modules that support the echo
cancellation and noise suppression (ECNS) feature.

Echo cancellation and noise suppression is a fundamental part of voice uplink
processing. When a person uses a phone to make a voice call, the sound played
back on the speaker is echoed back to the microphone electrically and
acoustically. This echo can be perceived by the far end and can vary from
mildly annoying to unacceptable, depending on how much coupling exists.

EC algorithms cancel this echoed signal from the microphone input with an
adaptive filter that models the path taken by the echo. When this model is
combined with the signal played on the speaker, a replica of the echo can be
created, which is then subtracted from the microphone signal. The noise
suppressor suppresses the near end noise.
*/


#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/


#include "Elite_CAPI_V2_types.h"


/** @addtogroup capiv2_ext_ecns
@{ */

/** Unique identifier of the custom framework extension for ECNS. */
#define FWK_EXTN_ECNS 0x00010E54

/*------------------------------------------------------------------------------
 * Property definitions - Needs implementation in module/library
 *----------------------------------------------------------------------------*/

/** Property used to statically query for ECNS output capabilities.

     @msgpayload{ecns_output_capabilities_t}
     @tablens{weak__ecns__output__capabilities__t} @newpage
 */
#define CAPI_V2_PROPERTY_ID_ECNS_OUTPUT_CAPABILITIES 0x00010E55

/** Property used to statically query for ECNS monitoring capabilities.

     @msgpayload{ecns_monitoring_capabilities_t}
     @tablens{weak__ecns__monitoring__capabilities__t}
 */
#define CAPI_V2_PROPERTY_ID_ECNS_MONITORING_CAPABILITIES 0x00010E56

/** Property used to statically query for ECNS VP3 capabilities.

     @msgpayload{ecns_vp3_capabilities_t}
     @tablens{weak__ecns__vp3__capabilities__t} @newpage
 */
#define CAPI_V2_PROPERTY_ID_ECNS_VP3_CAPABILITIES 0x00010E57

/** Property used to statically query for ECNS rate matching capabilities.

     @msgpayload{ecns_rate_matching_capabilities_t}
     @tablens{weak__ecns__rate__matching__capabilities__t}
 */
#define CAPI_V2_PROPERTY_ID_ECNS_RATE_MATCHING_CAPABILITIES 0x00010E58

/** Property used to statically query for ECNS Source Tracking Trio
     capabilities.

     @msgpayload{ecns_stt_capabilities_t}
     @tablens{weak__ecns__stt__capabilities__t} @newpage
 */
#define CAPI_V2_PROPERTY_ID_ECNS_STT_CAPABILITIES 0x00010E60

/** Property used to statically query for ECNS multichannel reference
     capabilities.

     @msgpayload{ecns_multichannel_ref_capabilities_t}
     @table{weak__ecns__multichannel__ref__capabilities__t}
 */
#define CAPI_V2_PROPERTY_ID_MULTICHANNEL_REF_CAPABILITIES 0x00010EA0

/** Property used to statically query for ECNS speech activity detection
     capabilities.

     @msgpayload{ecns_speech_act_det_capabilities_t}
     @tablens{weak__ecns__speech__act__det__capabilities__t}
 */
#define CAPI_V2_PROPERTY_ID_SPEECH_ACT_DET_CAPABILITIES 0x00010EA2

/** Supports dynamic enabling/disabling of ECNS audio effects.
    This parameter supersedes ACDB calibration.

    Upon receiving this payload, the ECNS module internally enables or disables
    ECNS submodules.

    During RTC, the aDSP returns the Effective Mode word after the effect mask
    has been applied. The Effective Mode word also overrides the RTC Set
    Parameter Mode word.

    @msgpayload{ecns_effects_payload_t}
    The payload size is 4 bytes.
    @tablens{weak__ecns__effects__payload__t}
*/
//#define GLOBAL_EFFECT_PARAM_ENABLE           (0x00010EAF)

/** @weakgroup weak_ecns_effects_payload_t
@{ */
/* payload definition for GLOBAL_EFFECT_PARAM_ENABLE param Id for ECNS module */
typedef struct ecns_effects_payload_t
{
  uint32_t ecns_effect;
  /**< Specifies how ECNS global effects are enabled.

       @values
       - 0 -- Both EC and NS are disabled
       - 1 -- Only EC is enabled
       - 2 -- Only NS is enabled
       - 3 -- Both EC and NS are enabled @tablebulletend */

}/** @cond */ecns_effects_payload_t/** @endcond */;
/** @} *//* end_weakgroup weak_ecns_effects_payload_t */

/** Sample slip parameter ID used to set the instantaneous drift for all ECNS
     ports.

     @msgpayload{instantaneous_drift_t}
     @table{weak__instantaneous__drift__t}
 */
#define SS_PARAM_INST_DRIFT (0x00010E4F)


/** @weakgroup weak_instantaneous_drift_t
@{ */
/* payload for SS_PARAM_INST_DRIFT param Id */
typedef struct instantaneous_drift_t
{
  int32_t*  inst_drift_in_us;  /**<  Array of instantaneous drift in
                                     microseconds for each port. */
}/** @cond */instantaneous_drift_t/** @endcond */;
/** @} *//* end_weakgroup weak_instantaneous_drift_t */


/** Sample slip parameter ID used to get the drift (in samples) to be
     corrected for all ECNS ports.

     @msgpayload{drift_samples_t}
     @table{weak__drift__samples__t}
 */
#define SS_PARAM_DRIFT_SAMPLES (0x00010E50)


/** @weakgroup weak_drift_samples_t
@{ */
/* payload for SS_PARAM_DRIFT_SAMPLES param Id */
typedef struct drift_samples_t
{
  int32_t*  drift_in_samples;  /**< Array of drift in samples for each port. */
}/** @cond */drift_samples_t/** @endcond */;
/** @} *//* end_weakgroup weak_drift_samples_t */


/** Sample slip parameter ID used to reset a specific port or all ports.

     @msgpayload
     None.
 */
#define SS_PARAM_RESET (0x00010E51)


/** @weakgroup weak_ecns_output_capabilities_t
@{ */
/* structure for querying ECNS output capabilities */
/** The ECNS module must provide this information to the aDSP service.
*/
typedef struct ecns_output_capabilities_t
{
   bool_t       output_lec;
   /**< Indicates whether the ECNS module provides Linear Echo Cancelled
        (LEC) output.

        @values
        - TRUE -- Module outputs LEC
        - FALSE -- Otherwise @tablebulletend */

   bool_t       output_nr;
   /**< Indicates whether the ECNS module provides Noise Reference (NR) output.

        @values
        - TRUE -- Module outputs NR
        - FALSE -- Otherwise @tablebulletend */

}/** @cond */ecns_output_capabilities_t/** @endcond */;
/** @} *//* end_weakgroup weak_ecns_output_capabilities_t */


/** @weakgroup weak_ecns_monitoring_capabilities_t
@{ */
/* structure for querying ECNS monitoring capabilities */
/** The ECNS module must provide this information to the aDSP service.
*/
typedef struct ecns_monitoring_capabilities_t
{
   bool_t       is_rtm_supported;
   /**< Indicates whether the ECNS module supports RTM functionality.
        The service uses RTM to moniter AF coefficients and the state of the
        module in real time.

        @values
        - TRUE -- Module supports RTM
        - FALSE -- Otherwise @tablebulletend */

}/** @cond */ecns_monitoring_capabilities_t/** @endcond */;
/** @} *//* end_weakgroup weak_ecns_monitoring_capabilities_t */


/** @weakgroup weak_ecns_vp3_capabilities_t
@{ */
/* structure for querying ECNS VP3 capabilities */
/** The ECNS module must provide this information to the aDSP service.
*/
typedef struct ecns_vp3_capabilities_t
{
   bool_t       is_vp3_supported;
   /**< Indicates whether the ECNS module supports VP3 functionality.

        @values
        - TRUE -- Module supports VP3
        - FALSE -- Otherwise @tablebulletend */

}/** @cond */ecns_vp3_capabilities_t/** @endcond */;
/** @} *//* end_weakgroup weak_ecns_vp3_capabilities_t */


/** @weakgroup weak_ecns_stt_capabilities_t
@{ */
/* structure for querying ECNS Source tracking trio capabilities */
/** The ECNS module must provide this information to the aDSP service.
*/
typedef struct ecns_stt_capabilities_t
{
   bool_t       is_stt_supported;
   /**< Indicates whether the ECNS module supports STT functionality.

        @values
        - TRUE -- Module supports SST
        - FALSE -- Module does not support SST @tablebulletend */

}/** @cond */ecns_stt_capabilities_t/** @endcond */;
/** @} *//* end_weakgroup weak_ecns_stt_capabilities_t */


/** @weakgroup weak_ecns_rate_matching_capabilities_t
@{ */
/** The ECNS module must provide this information to the aDSP service.
*/
typedef struct ecns_rate_matching_capabilities_t
{
   bool_t     is_rate_matching_supported;
   /**< Indicates whether the ECNS module supports internal rate matching
        functionality. If the ECNS supports rate matching, the service provides
        drift information and queries for the number of samples to be
        corrected.

        @values
        - TRUE -- Module supports rate matching
        - FALSE -- Otherwise @tablebulletend */

}/** @cond */ecns_rate_matching_capabilities_t /** @endcond */;
/** @} *//* end_weakgroup weak_ecns_rate_matching_capabilities_t */


/** @weakgroup weak_ecns_multichannel_ref_capabilities_t
@{ */
typedef struct ecns_multichannel_ref_capabilities_t
{
   bool_t is_multichannel_ref_supported;
   /**< Specifies whether the ECNS module supports multichannel far reference
        functionality.

        @values
        - TRUE -- Module supports multichannel far reference
        - FALSE -- Otherwise

        If the module supports this functionality, the service provides an
        available far reference. CAPIv2 must have an appropriate media format
        converter to convert to the number of reference channels that the EC
        library requires.

        If the module does not support this functionality, the service provides
        a mono reference. */

}/** @cond */ecns_multichannel_ref_capabilities_t /** @endcond */;
/** @} *//* end_weakgroup weak_ecns_multichannel_ref_capabilities_t */


/** @weakgroup weak_ecns_speech_act_det_capabilities_t
@{ */
/*  struture for querying ECNS speech activity detection capabilities */
/** The ECNS module must provide this information to the aDSP service.
*/
typedef struct ecns_speech_act_det_capabilities_t
{
   bool_t       is_speech_activity_supported;
   /**< Specifies whether the ECNS module supports speech activity detection.

        @values
        - TRUE -- Speech activity detection is supported
        - FALSE -- Speech activity detection is not supported

        If the ECNS module supports detection, it must raise the activity event
        when speech activity changes.

        ParamID for raising event: #PARAM_ID_SPEECH_ACTIVITY_UPDATE */

}/** @cond */ecns_speech_act_det_capabilities_t/** @endcond */;
/** @} *//* end_weakgroup weak_ecns_speech_act_det_capabilities_t */


/** Parameter for raising speech activity update event. End point of this
     event is DSP service.

     @msgpayload{speech_activity_update_payload_t}
     @tablens{weak__speech__activity__update__payload__t}

     @sa #CAPI_V2_EVENT_DATA_TO_DSP_SERVICE */
     /* in Elite_CAPI_V2_events.h */
#define PARAM_ID_SPEECH_ACTIVITY_UPDATE 0x00010E81

/** @weakgroup weak_speech_activity_update_payload_t
@{ */
/** Data structure for reporting speech activity. This structure can be
  extended in the future.
*/
typedef struct speech_activity_update_payload_t
{
  uint16_t activity_class;
  /**< Class of main activies supported.

       @values speech_main_activity_t */

  uint16_t sub_activity;
  /**< Provides a subactivity type for the main activity (to support backward
       compatibility).

       If an unknown subactivity is received by the service, it requires
       minimum handling.

       @values
       - #speech_mute_sub_activity_t
       - #speech_break_sub_activity_t @tablebulletend*/

}/** @cond */speech_activity_update_payload_t/** @endcond */;
/** @} *//* end_weakgroup speech_activity_update_payload_t */

/** Valid speech activity class types.
*/
typedef enum speech_activity_class_t
{
   SPEECH_ACTIVITY_CLASS_MUTE = 0,    /**< Speech signal mute detection. */
   SPEECH_ACTIVITY_CLASS_BREAK,       /**< Speech signal break detection. */
   SPEECH_ACTIVITY_CLASS_MAX=0xFFFF   /**< Maximum number of speech activity
                                           classes. */
}/** @cond */speech_activity_class_t/** @endcond */;

/** Valid speech subactivities for mute class types.
*/
typedef enum speech_mute_sub_activity_t
{
   MUTE_ACTIVITY_NOT_DETECTED = 0, /**< Mute activity is not detected by the
                                        module. Raised to revert a previously
                                        raised event for the mute class. */
   MUTE_ACTIVITY_DETECTED,         /**< Mute activity is detected by the module. */
   MUTE_ACTIVITY_MAX=0xFFFF,       /**< Maximum number of subactivities for mute. */
}/** @cond */speech_mute_sub_activity_t/** @endcond */;

/** Valid speech subactivities for break class types.
*/
typedef enum speech_break_sub_activity_t
{
   BREAK_ACTIVITY_NOT_DETECTED = 0, /**< Break activity is not detected by the
                                         module. Raised to revert a previously
                                         raised event for mute class. */
   BREAK_ACTIVITY_DETECTED,         /**< Break activity is detected by the module. */
   BREAK_ACTIVITY_MAX=0xFFFF,       /**< Maximum number of subactivities for break. */
}/** @cond */speech_break_sub_activity_t/** @endcond */;


/** Used by the aDSP service to statically query the ECNS properties for
  their capabilities.
*/
typedef struct capi_v2_get_ecns_property_t
{
   uint32_t                  sec_property_id;
   /**< Secondary property ID of the ECNS property.

        @values 0 */

   void*                     ecns_info;
   /**< Points to the structure that corresponds to sec_property_id.

        The ECNS module updates the structure elements when it provides
        its capabilites to the service. @newpagetable */

}/** @cond */capi_v2_get_ecns_property_t/** @endcond */;

/** @} *//* end_addtogroup capiv2_ext_ecns */

/** @endcond *//* OEM_only */


#ifdef __cplusplus
}
#endif //__cplusplus
#endif
