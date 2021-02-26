#ifndef ELITE_FWK_EXTNS_COMPR_OVER_PCM_H
#define ELITE_FWK_EXTNS_COMPR_OVER_PCM_H

/**
  @file Elite_fwk_extns_compr_over_pcm.h

  @brief Parameters required to use compressed over PCM.

  This file defines a framework extension for compressed over PCM.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description below the Edit History displays in the PDF.
      Contact Tech Pubs for assistance.
===========================================================================*/

/*==============================================================================
  Copyright (c) 2018 Qualcomm Technologies, Inc.  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
==============================================================================*/

/*==============================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.5/elite/module_interfaces/shared_lib_api/inc/capi/Elite_fwk_extns_compr_over_pcm.h#2 $

  when        who      what, where, why
  --------    ---      -------------------------------------------------------
  08/20/16    sw      (Tech Pubs) Edited Doxygen markup/comments for AVS 2.8.
  4/11/2016   rbhatnk  created file
  02/07/18    npendeka  Modified
==============================================================================*/

/*------------------------------------------------------------------------------
 * Include Files
 *----------------------------------------------------------------------------*/
#include "qurt_elite_types.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/** @weakgroup weakf_capiv2_ext_compressed_over_pcm_intro

Modules can use the #FWK_EXTN_COMPR_OVER_PCM framework extension as follows:
 - To implement a packetizer that packetizes compressed data to look like PCM
 - To implement a depacketizer that extracts encoded data from packetized data

The packetizer or depacketizer that handles compressed over PCM (COP) data
implements this extension. By default, a packetizer converts compressed data
(encoded data) as follows:
 - The packetized data and original PCM data have an equal number of
   samples per second.
 - The packetized data is 16 bits per sample.
 - If frame_length is configured differently, the packetizer can output a
   different number of bytes than the original PCM.

A depacketizer extracts encoded data from COP data by stripping off the COP
headers and any zero padding.

In the topology, the packetizer module that implements this framework extension
is placed immediately after the encoder and depacketizer, and before the
decoder.
*/


/** @addtogroup capiv2_ext_compressed_over_pcm
@{ */

/** Unique identifier used to represent a module that supports the Compressed
    Over PCM framework extension.

    This extension supports the following parameters and event:
    - #FWK_EXTN_PARAM_ID_COMPR_OVER_PCM_PCM_FRAME_LENGTH
    - #FWK_EXTN_PARAM_ID_COMPR_OVER_PCM_PCM_MEDIA_FORMAT
    - #FWK_EXTN_PARAM_ID_COMPR_OVER_PCM_CONFIG_ZERO_PADDING
    - #FWK_EXTN_EVENT_ID_COMPR_OVER_PCM_COP_FRAME_LENGTH
*/
#define FWK_EXTN_COMPR_OVER_PCM (0x12F3F)


/*------------------------------------------------------------------------------
 * Parameter definitions - Needs implementation in module/library
 *----------------------------------------------------------------------------*/

/** ID of the parameter that provides the number of PCM samples (per channel)
    required to produce one encoded frame.

    A PCM sample is defined by a media format set through
    #FWK_EXTN_PARAM_ID_COMPR_OVER_PCM_PCM_MEDIA_FORMAT. If this parameter is not
    set, the packetizer will not be able to process data.

    The depacketizer does not support this parameter.

    @msgpayload{fwk_extn_param_id_compr_over_pcm_pcm_frame_length_payload_t}
    @table{weak__fwk__extn__param__id__compr__over__pcm__pcm__frame__length__payload__t}
 */
#define FWK_EXTN_PARAM_ID_COMPR_OVER_PCM_PCM_FRAME_LENGTH (0x12F40)

/** @weakgroup weak_fwk_extn_param_id_compr_over_pcm_pcm_frame_length_payload_t
@{ */
/* Payload of the FWK_EXTN_PARAM_ID_COMPR_OVER_PCM_PCM_FRAME_LENGTH
 */
typedef struct fwk_extn_param_id_compr_over_pcm_pcm_frame_length_payload_t
{
   uint32_t pcm_frame_length_per_ch;
   /**< Length of the frame in PCM samples. */

}/** @cond */fwk_extn_param_id_compr_over_pcm_pcm_frame_length_payload_t/** @endcond */;
/** @} *//* end_weakgroup weak_fwk_extn_param_id_compr_over_pcm_pcm_frame_length_payload_t */


/** ID of the parameter that provides the media format of the PCM data used for
    encoding. If this parameter is not set, the packetizer cannot process data.

    The depacketizer does not support this parameter.

    @msgpayload{fwk_extn_param_id_compr_over_pcm_pcm_media_format_t}
    @table{weak__fwk__extn__param__id__compr__over__pcm__pcm__media__format__t}
    @newpage
 */
#define FWK_EXTN_PARAM_ID_COMPR_OVER_PCM_PCM_MEDIA_FORMAT (0x13209)

/** @weakgroup weak_fwk_extn_param_id_compr_over_pcm_pcm_media_format_t
@{ */
/* Payload of FWK_EXTN_PARAM_ID_COMPR_OVER_PCM_PCM_MEDIA_FORMAT
 */
typedef struct fwk_extn_param_id_compr_over_pcm_pcm_media_format_t
{
   uint32_t sampling_rate;
   /**< Number of samples per second.

        @values Any valid number of channels that the framework (or
        ASM_MEDIA_FMT_MULTI_CHANNEL_PCM) supports (refer to
        @xrefcond{Q3,80-NF774-1,80-NA610-1})*/

   uint32_t channels;
   /**< Number of PCM channels.

        @values Any valid number of channels that the framework (or
        ASM_MEDIA_FMT_MULTI_CHANNEL_PCM) supports */

   uint32_t bits_per_sample;
   /**< Number of bits per sample per channel.

        @values any bits per sample that the framework supports */

   uint16_t channel_type[CAPI_V2_MAX_CHANNELS];
   /**< Array of channel types.

        @values Any channel mapping that the framework supports */

}/** @cond */fwk_extn_param_id_compr_over_pcm_pcm_media_format_t/** @endcond */;
/** @} *//* end_weakgroup weak_fwk_extn_param_id_compr_over_pcm_pcm_media_format_t */


/** ID of the parameter that provides zero padding.

    The packetizer supports setting and configuring zero padding.

    The depacketizer does not support this parameter.

    @msgpayload{fwk_extn_param_id_compr_over_pcm_config_zero_padding_t}
    @table{weak__fwk__extn__param__id__compr__over__pcm__config__zero__padding__t}
    @newpage
 */
#define FWK_EXTN_PARAM_ID_COMPR_OVER_PCM_CONFIG_ZERO_PADDING (0x1320A)

/** Macro that disables zero padding. */
static const int32_t FWK_EXTN_CONST_COMPR_OVER_PCM_DISABLE_ZERO_PAD = (int32_t)(0x80000000UL);

/** @weakgroup weak_fwk_extn_param_id_compr_over_pcm_config_zero_padding_t
@{ */
/* Payload of FWK_EXTN_PARAM_ID_COMPR_OVER_PCM_CONFIG_ZERO_PADDING
 */
typedef struct fwk_extn_param_id_compr_over_pcm_config_zero_padding_t
{
   int32_t zero_padding_config;
   /**< Configures zero padding.

        @values (for packetizer set)
        - 0 -- Enable zero padding (Default)
        - #FWK_EXTN_CONST_COMPR_OVER_PCM_DISABLE_ZERO_PAD -- Disable zero
          padding
        - +1/-1 -- Change zero padding by the given amount

        A value of +1 or -1 is useful for rate adjustments. Typically, zero
        padding ensures that the COP frame length is the same as the PCM frame
        length.

        For non-zero padding (a non-0x80000000 value), zero padding is reduced
        or increased by zero_padding_config. For example:
        - If zero_padding_config = +1, zero padding increases by 1 sample per
          channel.
        - If zero_padding_config = -1, zero padding decreases by 1 sample per
          channel.

        A run-time error occurs if zero padding cannot be changed by the
        specified amount:
         - The drift value is at the output sampling rate
         - Other values are not supported
         - Once set, applies to all subsequent frames
         - Corrections occur once every output frame

        @values (for packetizer get command) Returns an error

        @values (for depacketizer) Returns an error */

}/** @cond */fwk_extn_param_id_compr_over_pcm_config_zero_padding_t/** @endcond */;
/** @} *//* end_weakgroup weak_fwk_extn_param_id_compr_over_pcm_config_zero_padding_t */


/** ID of the event used to provide notification of a change in the COP frame
    length.

    The COP frame length is the number of 16-bit words between any two COP
    synchronization words. This length includes zero padding that is accounted
    for in the COP frame as well the padding that is not accounted for.

    The packetizer does not support this event.

    The depacketizer supports this event.

    @msgpayload{fwk_extn_event_id_compr_over_pcm_cop_frame_length_t}
    @table{weak__fwk__extn__event__id__compr__over__pcm__cop__frame__length__t}
 */
#define FWK_EXTN_EVENT_ID_COMPR_OVER_PCM_COP_FRAME_LENGTH (0x1320B)

/** @weakgroup weak_fwk_extn_event_id_compr_over_pcm_cop_frame_length_t
@{ */
/* Payload of FWK_EXTN_EVENT_ID_COMPR_OVER_PCM_COP_FRAME_LENGTH
 */
typedef struct fwk_extn_event_id_compr_over_pcm_cop_frame_length_t
{
   uint32_t cop_frame_length;
   /**< Indicates the length of the COP frame when an event is raised.

        @values (for depacketizer) Any change in the COP frame length

        @values (for packetizer) Not supported */

   uint32_t is_nominal_frame;
   /**< Indicates whether the COP frame length is nominal (no drift correction
        is done by removing or inserting zeros).

        COP packet only -- indicates nominal frames with one frame delay.
        That is, the current frame indicates whether the previous frame is
        nominal. Therefore, this event occurs one frame late. */

} fwk_extn_event_id_compr_over_pcm_cop_frame_length_t;
/** @} *//* end_weakgroup weak_fwk_extn_event_id_compr_over_pcm_cop_frame_length_t */

/** @} *//* end_addtogroup capiv2_ext_compressed_over_pcm */

/** ID of the event used to provide notification of a change in the bit rate level
 * received through link statistics.

    The packetizer does not support this event.

    The depacketizer supports this event.

    @msgpayload{fwk_extn_event_id_compr_over_pcm_cop_frame_length_t}
*/
#define FWK_EXTN_EVENT_ID_COMPR_OVER_PCM_BIT_RATE_LEVEL (0x132E3)

/* Payload of FWK_EXTN_EVENT_ID_COMPR_OVER_PCM_BIT_RATE_LEVEL
 */
typedef struct fwk_extn_event_id_compr_over_pcm_bit_rate_level_t
{
   uint32_t bit_rate_level;
   /**< Indicates bit rate level of the COP frame when an event is raised.

        @values (for depacketizer) Any change in the COP frame length

        @values (for packetizer) Not supported */

} fwk_extn_event_id_compr_over_pcm_bit_rate_level_t;

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* #ifndef ELITE_FWK_EXTNS_COMPR_OVER_PCM_H */
