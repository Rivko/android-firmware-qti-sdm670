#ifndef _ELITE_FWK_EXTN_AVCTX_ECNS_RX_DRC_VOL_IMC_H_
#define _ELITE_FWK_EXTN_AVCTX_ECNS_RX_DRC_VOL_IMC_H_

/**
  @file Elite_fwk_extns_avctx_ecns_rx_drc_vol_imc.h

  @brief This file defines a framework extensions and corresponding private
  messages for AVC/RVE Tx and ECNS/DRC Rx/VOL Rx intermodule communication
  (IMC).
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

  $Header: //components/rel/avs.adsp/2.8.5/elite/module_interfaces/shared_lib_api/inc/capi/Elite_fwk_extns_avctx_ecns_rx_drc_vol_imc.h#1 $

  when        who      what, where, why
  --------    ---      -------------------------------------------------------
  05/14/15    sw       (Tech Pubs) Edited Doxygen markup/comments for 8952.2.6.
  10/29/2014  sabdagir   Created
=============================================================================*/

/** @cond OEM_only */

/** @weakgroup weakf_capiv2_ext_avcrve_ecnsdrcvol_intro

The #FWK_EXTN_AVC_RVE_TX_IMC_SOURCE and #FWK_EXTN_AVC_RVE_TX_IMC_DESTINATION
framework extensions are used by modules that support AVC/RVE Tx and
ECNS/DRC Rx/VOL Rx intermodule communication.

 - Source modules      -- ECNS, DRC Rx, Volume Rx
 - Destination modules -- AVC/RVE Tx

The AVC/RVE Tx module estimates background noise and then sends it to the
AVC/RVE Rx module. The Tx module requires wind score (from ECNS), down compression
ratio (from DRC Rx), and the Rx volume (from Volume Rx). The
intermodule communication framework (defined in Section @xref{dox:ImcLibrary})
is used for this communication. 
*/


/*------------------------------------------------------------------------------
 * Include Files
 *----------------------------------------------------------------------------*/
#include "Elite_CAPI_V2_types.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/


/** @addtogroup capiv2_ext_avc_rve_ecns_drc_imc
@{ */

/*------------------------------------------------------------------------------
 * Framework extensions
 *----------------------------------------------------------------------------*/

/** Unique identifier of the custom framework extension for the AVC/RVE Tx
    intermodule communication. */
#define FWK_EXTN_AVC_RVE_TX_IMC_SOURCE      (0x00010E47)

/** Unique identifier of the custom framework extension for the AVC/RVE Tx
    (destination) module. */
#define FWK_EXTN_AVC_RVE_TX_IMC_DESTINATION (0x00010E48)


/*------------------------------------------------------------------------------
 * Communication Id and Param Id/payload for sending it to modules
 *----------------------------------------------------------------------------*/

/** Communication ID for AVC/RVE Tx intermodule communication. */
#define COMM_ID_AVC_RVE_TX_IMC   (0x00010E49)

/** ID of the parameter used to send a unique key (blob of data) to the source
    and destination modules for intermodule communication.

    @note1hang The source ID is not part of the key because intermodule
               communication involves multiple sources.

    @msgpayload{avc_rve_tx_imc_key_t}
    @table{weak__avc__rve__tx__imc__key__t} @newpage
*/
#define PARAM_ID_AVC_RVE_TX_IMC_KEY  (0x00010E4A)

/** @weakgroup weak_avc_rve_tx_imc_key_t
@{ */
/* Key structure for IMC */
typedef struct avc_rve_tx_imc_key_t
{
   uint32_t comm_id;
   /**< Unique identifier for AVC/RVE intermodule communication.

        @values #COMM_ID_AVC_RVE_TX_IMC */

   uint32_t dest_mod_id;
   /**< Unique identifier of the destination module that supports
        #FWK_EXTN_AVC_RVE_TX_IMC_DESTINATION. */

   uint32_t session_num;
   /**< Unique identifier that distinguishes different instances of
        #COMM_ID_AVC_RVE_IMC communication. The service identifier that creates
        source or destination modules can be used for this session number. */

}/** @cond */avc_rve_tx_imc_key_t/** @endcond */;
/** @} *//* end_weakgroup weak_weak_avc_rve_tx_imc_key_t */


/*------------------------------------------------------------------------------
 * AVC Tx IMC data payload
 *----------------------------------------------------------------------------*/


/** Payload header structure used by the parameter IDs. The payload of a
    parameter structure is followed by this header.

    The source module must update this structure before sending it to the
    destination module.
*/
typedef struct avc_rve_tx_imc_payload_header_t
{
   uint32_t param_id;
   /**< Parameter ID of the payload.

        @values
        - #PARAM_ID_AVC_RVE_TX_ECNS
        - #PARAM_ID_AVC_RVE_TX_DRC_RX
        - #PARAM_ID_AVC_RVE_TX_VOL_RX @tablebulletend */

   uint32_t param_size;
   /**< Size (in bytes) for the payload specified in param_id. */

}/** @cond */avc_rve_tx_imc_payload_header_t/** @endcond */;

/** Payload structure used by the parameter IDs.
*/
typedef struct avc_rve_tx_imc_payload_t
{
   avc_rve_tx_imc_payload_header_t param_header;
   /**< Parameter payload header. The actual payload follows this header. */

}/** @cond */avc_rve_tx_imc_payload_t/** @endcond */;


/** ID of the parameter that provides a windscore value from the ECNS to
    the AVC/RVE Tx module.

    @msgpayload{rve_tx_windscore_payload_t}
    @table{weak__rve__tx__windscore__payload__t}
*/
#define PARAM_ID_AVC_RVE_TX_ECNS           (0x00010E4B)

/** @weakgroup weak_rve_tx_windscore_payload_t
@{ */
/* Payload for ecns -> avc/rve tx communication */
typedef struct rve_tx_windscore_payload_t
{
   uint32_t version;
   /**< Payload version.

        @values 0 */

   /* Start of parameters for version 0*/

   int32_t wind_score_L32Q30;
   /**< Windscore value in Q30 format. */

   /* End of parameters for version 0*/

}/** @cond */rve_tx_windscore_payload_t/** @endcond */;
/** @} *//* end_weakgroup weak_rve_tx_windscore_payload_t */


/** ID of the parameter for DRC Rx module-to-AVC/RVE Tx module communication.

    @msgpayload{avc_rve_tx_drc_rx_payload_t}
    @table{weak__avc__rve__tx__drc__rx__payload__t}
*/
#define PARAM_ID_AVC_RVE_TX_DRC_RX         (0x00010E4C)

/** @weakgroup weak_avc_rve_tx_drc_rx_payload_t
@{ */
/* payload for drc rx -> avc/rve tx communication */
typedef struct avc_rve_tx_drc_rx_payload_t
{
   uint32_t version;
   /**< Payload version.

        @values 0 */

   /* Start of parameters for version 0*/

   int16_t drc_rx_enable;
   /**< Specifies whether DRC Rx module is enabled. 

        @values
        - 0 -- Disabled
        - 1 -- Enabled @tablebulletend */

   int16_t down_comp_threshold_L16Q7;
   /**< DRC down-compression ratio in Q7 format. */

   /* End of parameters for version 0*/

}/** @cond */avc_rve_tx_drc_rx_payload_t/** @endcond */;
/** @} *//* end_weakgroup weak_avc_rve_tx_drc_rx_payload_t */


/** ID of the parameter that provides the Rx volume from the VOL Rx module to
    the AVC Tx module.

    @msgpayload{avc_rve_tx_vol_rx_payload_t}
    @table{weak__avc__rve__tx__vol__rx__payload__t} @newpage
*/
#define PARAM_ID_AVC_RVE_TX_VOL_RX          (0x00010E4D)

/** @weakgroup weak_avc_rve_tx_vol_rx_payload_t
@{ */
/* payload for vol rx -> avc/rve tx param Id */
typedef struct avc_rve_tx_vol_rx_payload_t
{
   uint32_t version;
   /**< Payload version.

        @values 0 */

   /* Start of parameters for version 0*/

   uint16_t rx_volume_L16Q13;
   /**< Rx volume in Q13 format. */

   int16_t reserved;
   /**< This field must be set to 0. */

   /* End parameters for version 0*/

}/** @cond */avc_rve_tx_vol_rx_payload_t/** @endcond */;
/** @} *//* end_weakgroup weak_avc_rve_tx_vol_rx_payload_t */

/** @} *//* end_addtogroup capiv2_ext_avc_rve_ecns_drc_imc */

/** @endcond *//* OEM_only */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif /* _ELITE_FWK_EXTN_AVCTX_ECNS_RX_DRC_VOL_IMC_H_ */