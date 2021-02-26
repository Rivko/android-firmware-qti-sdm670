#ifndef _ELITE_FWK_EXTNS_AVC_RVE_IMC_H_
#define _ELITE_FWK_EXTNS_AVC_RVE_IMC_H_

/**
  @file Elite_fwk_extns_avc_rve_imc.h

  @brief Framework extension for AVC/RVE Tx and Rx intermodule communication(IMC).

  This file defines a framework extensions and corresponding private messages
  for AVC/RVE Tx and Rx module communication.
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

  $Header: //components/rel/avs.adsp/2.8.5/elite/module_interfaces/shared_lib_api/inc/capi/Elite_fwk_extns_avc_rve_imc.h#1 $

  when        who      what, where, why
  --------    ---      -------------------------------------------------------
  05/14/15    sw       (Tech Pubs) Edited Doxygen markup/comments for 8952.2.6.
  10/29/2014  sabdagir   Created
=============================================================================*/

/** @cond OEM_only */

/** @weakgroup weakf_capiv2_ext_avcrve_imc_intro

The #FWK_EXTN_AVC_RVE_IMC_SOURCE and #FWK_EXTN_AVC_RVE_IMC_DESTINATION
framework extensions are used by modules that support Automatic Voice
Control/Receive Voice Enhancement (AVC/RVE) Tx and Rx intermodule
communication (IMC).

The AVR/RVE module automatically adjusts the loudness of the Rx voice in
response to the near-end noise level on the Tx path. This adjustment
improves the intelligibility of the receive path voice signal. Thus, it
also improves both the loudness and the intelligibility of the downlink.

The AVC/RVE Tx module estimates background noise and then sends it to the
AVC/RVE Rx module. The intermodule communication framework (defined in
Section @xref{dox:ImcLibrary}) is used for this communication.
*/


/*------------------------------------------------------------------------------
 * Include Files
 *----------------------------------------------------------------------------*/
#include "Elite_CAPI_V2_types.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/


/** @addtogroup capiv2_ext_avc_rve_imc
@{ */

/*------------------------------------------------------------------------------
 * Framework extensions
 *----------------------------------------------------------------------------*/

/** Unique identifier of the custom framework extension for the AVC/RVE Tx
    (source) module.
 */
#define FWK_EXTN_AVC_RVE_IMC_SOURCE  (0x00010E59)

/** Unique identifier of the custom framework extension for the AVC/RVE Rx
    (destination) module 
 */
#define FWK_EXTN_AVC_RVE_IMC_DESTINATION (0x00010E44)


/*------------------------------------------------------------------------------
 * Communication Id and Param Id/payload for sending to modules
 *----------------------------------------------------------------------------*/

/** Unique communication ID for intermodule communication between the AVC/RVE
    Tx and AVC/RVE Rx modules.
 */
#define COMM_ID_AVC_RVE_IMC   (0x00010E5A)

/** ID of the parameter used to send a unique key (blob of data) to the source
    and destination modules for intermodule communication.

    @msgpayload{avc_rve_imc_key_t}
    @table{weak__avc__rve__imc__key__t}
*/
#define PARAM_ID_AVC_RVE_IMC_KEY   (0x00010E46)

/** @weakgroup weak_avc_rve_imc_key_t
@{ */
/* Key structure for IMC */
typedef struct avc_rve_imc_key_t
{
   uint32_t comm_id;
   /**< Unique identifier for AVC/RVE intermodule communication.

        @values #COMM_ID_AVC_RVE_IMC */

   uint32_t dest_mod_id;
   /**< Unique identifier of the destination module that supports
        #FWK_EXTN_AVC_RVE_IMC_DESTINATION. */

   uint32_t src_mod_id;
   /**< Unique identifier of the source module that supports
        #FWK_EXTN_AVC_RVE_IMC_SOURCE. */

   uint32_t session_num;
   /**< Unique identifier that distinguishes different instances of
        #COMM_ID_AVC_RVE_IMC communication. The service identifier that creates
        source or destination modules can be used for this session number. */

}/** @cond */avc_rve_imc_key_t/** @endcond */;
/** @} *//* end_weakgroup weak_avc_rve_imc_key_t */


/*------------------------------------------------------------------------------
 * AVC Rx/Tx module IMC data payload
 *----------------------------------------------------------------------------*/

/** Payload header structure used by the parameter IDs. The payload of a
    parameter structure is followed by this header.

    The source module must update this structure before sending it to the
    destination module.
*/
typedef struct avc_rve_imc_payload_header_t
{
   uint32_t param_id;
   /**< Parameter ID of the payload.

        @values
        - #PARAM_ID_RVE_NOISE_EST
        - #PARAM_ID_AVC_GAIN @tablebulletend */

   uint32_t param_size;
   /**< Size (in bytes) for the payload specified in param_id. */

}/** @cond */avc_rve_imc_payload_header_t/** @endcond */;

/** Payload structure used by the parameter IDs.
*/
typedef struct avc_rve_imc_payload_t
{
   avc_rve_imc_payload_header_t param_header;
   /**< Parameter payload header. The actual payload follows this header. */

}/** @cond */avc_rve_imc_payload_t/** @endcond */;


/** ID of the parameter used to send noise estimation from the source module
    to the destination module.

    @msgpayload{avc_rve_noise_est_payload_t}
    @table{weak__avc__rve__noise__est__payload__t}
*/
#define PARAM_ID_RVE_NOISE_EST                (0x00010E5B)

/** @weakgroup weak_avc_rve_noise_est_payload_t
@{ */
/* payload for RVE_NOISE param Id */
typedef struct avc_rve_noise_est_payload_t
{
   uint32_t version;
   /**< Payload version.

        @values 0 */

   /* Start of parameters for version 0*/

   int16_t smNoise[4];
   /**< Noise estimates. */

   int16_t update_noise;
   /**< Update noise flag. */

   int16_t reserved;
   /**< This field must be set to 0. */

   /* End of parameters for version 0*/

   int16_t smNoiseFB[3];
   /**< Noise estimates for higher bands. */

   int16_t gainAdaptNumBands;
   /**< Number of independent gain subbands.*/

   /* End of parameters for version 1*/

}/** @cond */avc_rve_noise_est_payload_t/** @endcond */;
/** @} *//* end_weakgroup weak_avc_rve_noise_est_payload_t */


/** ID of the parameter used to send the gain parameter from the source module
    to the destination module.

    @msgpayload{avc_rve_gain_payload_t}
    @table{weak__avc__rve__gain__payload__t}
*/
#define PARAM_ID_AVC_GAIN                 (0x00010E5C)

/** @weakgroup weak_avc_rve_gain_payload_t
@{ */
/* Payload structure used by PARAM_ID_AVC_GAIN.
*/
typedef struct avc_rve_gain_payload_t
{
   uint32_t version;
   /**< Payload version.

        @values 0 */

   /* Start of parameters for version 0*/

   int16_t gain_L16Q12;
   /**< Gain value to be applied by destination module. */

   int16_t reserved;
   /**< This field must be set to 0. */

   /* End of parameters for version 0*/

}/** @cond */avc_rve_gain_payload_t/** @endcond */;
/** @} *//* end_weakgroup weak_avc_rve_gain_payload_t */

/** @} *//* end_addtogroup capiv2_ext_avc_rve_imc */

/** @endcond *//* OEM_only */

#ifdef __cplusplus
}
#endif //__cplusplus
#endif /* _ELITE_FWK_EXTNS_AVC_RVE_IMC_H_ */
