#ifndef ELITE_INTF_EXTNS_CHANGE_MEDIA_FMT_H
#define ELITE_INTF_EXTNS_CHANGE_MEDIA_FMT_H

/**
  @file Elite_intf_extns_change_media_fmt.h

  @brief intf_extn IDs to change media format

  This file defines interface extensions that would help modules relay information
  to the framework about fields of media format which they can modify.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description below the Edit History displays in the PDF.
      Contact Tech Pubs for assistance.
===========================================================================*/
/*==============================================================================
  Copyright (c) 2016 Qualcomm Technologies, Inc.  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
==============================================================================*/

/*==============================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.5/elite/module_interfaces/shared_lib_api/inc/capi/Elite_intf_extns_change_media_fmt.h#1 $

  when        who       what, where, why
  --------    ---       -------------------------------------------------------
  08/20/16    sw      (Tech Pubs) Edited Doxygen markup/comments for AVS 2.8.
  03/04/16    shridhar  created file
==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/*------------------------------------------------------------------------------
 * Include Files
 *----------------------------------------------------------------------------*/
#include "Elite_CAPI_V2_types.h"

/** @weakgroup weakf_capiv2_ext_change_media_fmt_intro

The #INTF_EXTN_CHG_MF framework extension allows modules to relay information
to the framework about media format fields that the modules can modify.

The CAPIv2 pre/postprocessing module can use  INTF_EXTN_CHG_MF if the module
can change a media format based on a setting sent by the framework.

The module can decide not to share the information with the framework, even if
it can change the media format. If it shares information, the framework can
use the module to change the media format of the topology based on the shared
information.

Currently, this extension is used for audio processing modules.
*/


/** @addtogroup capiv2_ext_change_media_format
@{ */

/** Unique identifier of the interface extension that supports the Change Media
  Format feature. A module adds this ID if it can change the output media
  format.

  This extension supports the #INTF_EXTN_PARAM_ID_CHG_MF_PROPOSE_OP_MF
  parameter.

  @msgpayload{intf_extn_chg_mf_t}
  @table{weak__intf__extn__chg__mf__t}
 */
#define INTF_EXTN_CHG_MF                                     0x00010925

/*------------------------------------------------------------------------------
 * Parameter IDs
 *----------------------------------------------------------------------------*/

/** @weakgroup weak_intf_extn_chg_mf_t
@{ */
/* Payload for the #INTF_EXTN_CHG_MF extension.
 */
typedef struct intf_extn_chg_mf_t
{
   uint32_t    version;
   /**< Payload version.

        @values 0

        If new fields of media format are added, this value must be
        incremented. */

   bool_t      sampling_rate;
   /**< Indicates whether the module can change the sampling rate.

        @values
        - TRUE -- The module can change the rate
        - FALSE -- Otherwise @tablebulletend */

   bool_t      channel_info;
   /**< Indicates whether the module can change the channel information.

        @values
        - TRUE -- The module can change the information
        - FALSE -- Otherwise @tablebulletend */

   bool_t      bits_per_sample;
   /**< Indicates whether the module can change the bits per sample.

        @values
        - TRUE -- The module can change the bits
        - FALSE -- Otherwise @tablebulletend */

}/** @cond */intf_extn_chg_mf_t/** @endcond */;
/** @} *//* end_weakgroup weak_intf_extn_chg_mf_t */


/** ID of the parameter used to set single or multiple fields of output media
    format to a custom value.

    This parameter is optional; modules can ignore it.

    @msgpayload{intf_extn_param_id_chg_mf_propose_op_mf_t}
    @tablens{weak__intf__extn__param__id__chg__mf__propose__op__mf__t}
 */
#define INTF_EXTN_PARAM_ID_CHG_MF_PROPOSE_OP_MF              0x00013C12

/** @weakgroup weak_intf_extn_param_id_chg_mf_propose_op_mf_t
@{ */
/* Data Structure to send info to the module. */
/** This structure can be extended if new fields are to be added.
 */
typedef struct intf_extn_param_id_chg_mf_propose_op_mf_t
{
   bool_t       default_sampling_rate;
   /**< Indicates whether the module uses a default value instead of
        sampling_rate.

        @values
        - TRUE -- The module ignores sampling_rate and sets a default value
        - FALSE -- Otherwise  @tablebulletend*/

   uint32_t     sampling_rate;
   /**< Sampling rate in samples per second. */

   bool_t       default_channel_info;
   /**< Indicates whether the module uses default values instead of
        num_channels and channel_map.

        @values
        - TRUE -- The module ignores the channel information and sets default
          values
        - FALSE -- Otherwise @tablebulletend*/

   uint32_t     num_channels;
   /**< Number of output channels for the module. */

   uint16_t     channel_map[16];
   /**< Channel mapping array of size 16.

        Channel[i] mapping describes channel i. Each element i of the array
        describes channel i inside the buffer where 0 @le i < num_channels.
        Only the first num_channels elements are valid.

        @values See <i>PCM Channel Definitions</i> in
        @xrefcond{Q3,80-NF774-1,80-NA610-1} */

   bool_t       default_bits_per_sample;
   /**< Indicates whether the module uses a default value instead of
        bits_per_sample.

        @values
        - TRUE -- The module ignores bits_per_sample and sets a default value
        - FALSE -- Otherwise  @tablebulletend*/

   uint32_t     bits_per_sample;
   /**< Number bits per sample for the module. */

}/** @cond */intf_extn_param_id_chg_mf_propose_op_mf_t/** @endcond */;
/** @} *//* end_weakgroup weak_intf_extn_param_id_chg_mf_propose_op_mf_t */

/** @} *//* end_addtogroup capiv2_ext_change_media_format */

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* #ifndef ELITE_INTF_EXTNS_CHANGE_MEDIA_FMT_H*/
