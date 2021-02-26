#ifndef _ADSP_AVS_COMMON_API_H
#define _ADSP_AVS_COMMON_API_H

/*========================================================================*/
/**
@file adsp_avs_common_api.h
This file contains AVS common API commands and events structures definitions.
*/

/*===========================================================================
Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================ */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/api/common/inc/adsp_avs_common_api.h#3 $

when        who    what, where, why
--------    ---    -------------------------------------------------------
09/03/17    sw     (Tech Pubs) Edited Doxygen comments for AVS 2.8 (SDM845).
08/31/16    sw     (Tech Pubs) Edited Doxygen comments for AVS 2.8 (MSM8998).
05/03/16    KR     Created file
02/07/18  npendeka  Modified
========================================================================== */
#include "adsp_avs_common_api_prop.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/


/** @ingroup avscom_version
    Version of the Hexagon AVS common API.

    @versiontable{0.3,4.7,
    1  &  Initial version @tblendline}
*/
#define ADSP_AVS_COMMON_API_VERSION    0x00000001

/** @addtogroup avscom_mod_generic_encoder
@{ */
/** ID of the Generic Encoder module.

    Opcode -- AVS_MODULE_ID_ENCODER

    This module supports the following parameter IDs:
     - #AVS_ENCODER_PARAM_ID_ENC_FMT_ID (cannot be set at run time)
     - #AVS_ENCODER_PARAM_ID_ENC_CFG_BLK (can be set at run time)
    @if OEM_only
     - #AVS_ENCODER_PARAM_ID_ENC_BITRATE (can be set at run time)
    @endif
     - #AVS_ENCODER_PARAM_ID_PACKETIZER_ID (cannot be set at run time)

    The AFE_PORT_CMD_SET_PARAM_V2 command supports this module ID.
 */
#define AVS_MODULE_ID_ENCODER         0x00013229

/** Macro for defining the compressed over PCM (COP) packetizer ID. */
#define AVS_MODULE_ID_PACKETIZER_COP 0x0001322A

/** Encoder format ID parameter for the #AVS_MODULE_ID_ENCODER module.
    This parameter cannot be set at run time.

    @msgpayload{avs_enc_fmt_id_param_t}
    @table{weak__avs__enc__fmt__id__param__t} @newpage
 */
#define AVS_ENCODER_PARAM_ID_ENC_FMT_ID 0x0001322B

/*  Payload of the AVS_ENCODER_PARAM_ID_ENC_FMT_ID parameter. */
typedef struct avs_enc_fmt_id_param_t avs_enc_fmt_id_param_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avs_enc_fmt_id_param_t
@{ */
struct avs_enc_fmt_id_param_t
{
    uint32_t                 enc_fmt_id;
    /**< ID of the supported encoder format.

        @values
        @if OEM_only
        - ASM_MEDIA_FMT_SBC
        - ASM_MEDIA_FMT_AAC_V2
        @endif
        - Any OpenDSP-supported values

        For more information, refer to @xrefcond{Q6,80-NF774-1,80-NA610-1}. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avs_enc_fmt_id_param_t */


/** ID of the Encoder Configuration Block parameter for the
    #AVS_MODULE_ID_ENCODER module. This parameter can be set at run time.

    Supported encoder configurations:
    @if OEM_only
     - asm_sbc_enc_cfg_t
     - asm_aac_enc_cfg_v2_t
    @endif
     - asm_custom_enc_cfg_t

    For more information, refer to @xrefcond{Q6,80-NF774-1,80-NA610-1}.

    @msgpayload{avs_enc_cfg_blk_param_t}
    @tablens{weak__avs__enc__cfg__blk__param__t}
 */
#define AVS_ENCODER_PARAM_ID_ENC_CFG_BLK 0x0001322C

typedef struct avs_enc_cfg_blk_param_t avs_enc_cfg_blk_param_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avs_enc_cfg_blk_param_t
@{ */
/* Payload of the AVS_ENCODER_PARAM_ID_ENC_CFG_BLK parameter. */
/** Immediately following this structure is an encoder configuration block of
    size enc_cfg_blk_size bytes.
 */
struct avs_enc_cfg_blk_param_t
{
    uint32_t                  enc_cfg_blk_size;
    /**< Size of the encoder configuration block that follows this member.

         @values @ge 0 bytes */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avs_enc_cfg_blk_param_t */


/** @cond OEM_only */
/** ID of the encoder Bit Rate parameter for the #AVS_MODULE_ID_ENCODER module.
    This parameter can be set at run time.

    @msgpayload{avs_enc_bitrate_param_t}
    @table{weak__avs__enc__bitrate__param__t}
 */
#define AVS_ENCODER_PARAM_ID_ENC_BITRATE 0x0001322D

/*  Payload of the AVS_ENCODER_PARAM_ID_ENC_BITRATE parameter. */
typedef struct avs_enc_bitrate_param_t avs_enc_bitrate_param_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avs_enc_bitrate_param_t
@{ */
struct avs_enc_bitrate_param_t
{
    uint32_t                  enc_bitrate;
    /**< Maximum supported bit rate.

         @values AAC encoder only

         For more information refer to @xrefcond{Q6,80-NF774-1,80-NA610-1}. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avs_enc_bitrate_param_t */
/** @endcond */

/** ID of the Packetizer Type parameter for the #AVS_MODULE_ID_ENCODER module.
    This parameter cannot be set runtime.

    @msgpayload{avs_enc_packetizer_id_param_t}
    @table{weak__avs__enc__packetizer__id__param__t}
 */
#define AVS_ENCODER_PARAM_ID_PACKETIZER_ID 0x0001322E

/* Payload of the AVS_ENCODER_PARAM_ID_PACKETIZER_ID parameter. */
typedef struct avs_enc_packetizer_id_param_t avs_enc_packetizer_id_param_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avs_enc_packetizer_id_param_t
@{ */
struct avs_enc_packetizer_id_param_t
{
    uint32_t                  enc_packetizer_id;
    /**< ID of the supported encoder packetizer.

         @values
          - #AVS_MODULE_ID_PACKETIZER_COP
          - Any OpenDSP-supported values @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avs_enc_packetizer_id_param_t */

/**
 * Packetizer type parameter for the #AVS_MODULE_ID_ENCODER module.
 * This parameter cannot be set runtime.
 */
#define AVS_ENCODER_PARAM_ID_ENABLE_SCRAMBLING     0x0001323C

/**
 * Payload of the AVS_ENCODER_PARAM_ID_ENABLE_SCRAMBLING parameter.
 */
typedef struct  avs_enc_enable_scrambling_id_param_t  avs_enc_enable_scrambling_id_param_t;

#include "adsp_begin_pack.h"
/** @weakgroup weak_avs_enc_enable_scrambling_id_param_t
@{ */
struct avs_enc_enable_scrambling_id_param_t
{
    uint32_t                  enable_scrambler;
    /**< Flag for enabling scrambler.

	         @values
	          - 1 = enable scrambler
	          - 0 = disable scrambler @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avs_enc_enable_scrambling_id_param_t */

/** @cond OEM_only */
/** ID of the encoder frame size control parameter for the #AVS_MODULE_ID_ENCODER module.
    This parameter is set during init time.

    @msgpayload{avs_enc_frame_size_control_param_t}
    @table{weak__avs__enc__frame__size__control__param__t}
 */
#define AVS_ENCODER_PARAM_ID_FRM_SIZE_CONTROL 0x000132EA

/*  Payload of the AVS_ENCODER_PARAM_ID_FRM_SIZE_CONTROL parameter. */
typedef struct avs_enc_frame_size_control_param_t avs_enc_frame_size_control_param_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avs_enc_frame_size_control_param_t
@{ */
struct avs_enc_frame_size_control_param_t
{
    uint32_t                  frame_size_control_type;
    /**< Type of frame size control.

	         @values
	          - 0 = MTU_SIZE
	          - 1 = PEAK_BIT_RATE @tablebulletend */
    uint32_t                  frame_size_control_value;
    /**< Value of frame size control value.

	         @values
	         1) MTU_SIZE: MTU size in bytes as per the 
	         connected BT sink device.
	         NOTE: Expectation is that mtu_size is >= 
	         'frame size as per configured bitrate'.
	         2) PEAK_BIT_RATE: Peak bit rate in bits/second.
	         Eg : 96000 if 96kbps is the peak bitrate to be set.
	         Assumption is that peak bit rate set is > 
	         configured bitrate. */

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avs_enc_frame_size_control_param_t */
/** @endcond */
/** @} */ /* end_addtogroup avscom_mod_generic_encoder */

/** @ingroup avscom_mod_61937_packetizer
    ID of the IEC 61937 Packetizer module.

    This module supports IEC61937 packetization. It also supports the
    pass-through formatter when the input stream is IEC 61937-packetized and
    must be passed through.

    The module detects the pass-through case from the flags field in the
    payload of ASM_STREAM_CMD_OPEN_WRITE_COMPRESSED. This flag indicates
    whether the stream is IEC 61937-packetized.

    Currently, the audio decoder/encoder service uses the format ID to
    instantiate the packetizers. It uses the module ID to instantiate the
    pass-through formatter (because the stream is already packetized and the
    format is not known).

    Other services can use the module ID to instantiate the packetizer.
 */
#define AVS_MODULE_ID_PACKETIZER_IEC61937       0x0001322F

/** @ingroup avscom_mod_61937_depacketizer
    ID of the IEC 61937 Depacketizer module.

    The encoder service uses this module ID to load the depacketizer module.
 */
#define AVS_MODULE_ID_DEPACKETIZER_IEC61937     0x00013230


/** @ingroup avscom_evt_dtmf_det
    Event ID for DTMF detection.

    The client registers this event with the DTMF module using
    ASM_STREAM_CMD_REGISTER_PP_EVENTS or ADM_CMD_REGISTER_PP_EVENTS.
    If DTMF detection occurs, the DTMF detection module responds with the
    payload.

    @note1hang For details on the ASM and ADM events, see
               @xrefcond{Q6,80-NF774-1}{80-NA610-1}.

    @msgpayload{avs_dtmf_detect_event_payload_t}
    @table{weak__avs__dtmf__detect__event__payload__t}
 */
#define AVS_DTMF_DETECTION_EVENT                  0x00013236

typedef struct avs_dtmf_detect_event_payload_t avs_dtmf_detect_event_payload_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avs_dtmf_detect_event_payload_t
@{ */
/* Payload returned by DTMF module in response to the AVS_DTMF_DETECTION_EVENT
 * registered by the client     */
struct avs_dtmf_detect_event_payload_t
{
    uint32_t version;
    /**< Current version (0) of this interface. */

    uint16_t tone_low_freq;
    /**< Low frequency (in Herz) from the pair belonging to set A (where
         A = {697,770,852,941}). */

    uint16_t tone_high_freq;
    /**< High frequency (in Herz) from the pair belonging to set B (where
         B = { 1209,1336,1477}). */
}

#include "adsp_end_pack.h"
;
 /** @} *//* end_weakgroup weak_avs_dtmf_detect_event_payload_t */

/*    Generic decoder module ID.
 *
 *    This module supports the following parameter ID:
 *    - #AVS_DECODER_PARAM_ID_DEPACKETIZER_ID (cannot be set run time)
 *    Opcode -- #AVS_MODULE_ID_DECODER
 *
 *    AFE Command #AFE_PORT_CMD_SET_PARAM_V2 supports this module ID.
 *
 */
#define AVS_MODULE_ID_DECODER         0x00013231

/**
 * De-packetizer type parameter for the #AVS_MODULE_ID_DECODER module.
 * This parameter cannot be set runtime.
 */
#define AVS_DECODER_PARAM_ID_DEPACKETIZER_ID 0x00013235

/**
 * Payload of the #AVS_DECODER_PARAM_ID_DEPACKETIZER_ID parameter.
 */
typedef struct avs_dec_depacketizer_id_param_t avs_dec_depacketizer_id_param_t;

#include "adsp_begin_pack.h"
struct avs_dec_depacketizer_id_param_t
{
	uint32_t                  dec_depacketizer_id;
	/** Supported values:
	 *            #AVS_MODULE_ID_DEPACKETIZER_COP
	 *            #Any OpenDSP supported value (custom).
	 */
}
#include "adsp_end_pack.h"
;
/**
 * Macro for defining the de-packetizer ID: COP.
 */
#define AVS_MODULE_ID_DEPACKETIZER_COP 0x00013233


/** ID of the encoder Bit Rate level map parameter for the #AVS_MODULE_ID_ENCODER module.
    This parameter can be set at run time.
*/
#define AVS_ENCODER_PARAM_ID_BIT_RATE_LEVEL_MAP 0x000132E1

typedef struct bitrate_level_map_t
{
   uint32_t level;
   uint32_t bitrate;
}bitrate_level_map_t;

/*  Payload of the AVS_ENCODER_PARAM_ID_BIT_RATE_LEVEL_MAP parameter
 *
 *  Quality levels are always in ascending order of bit rate (even if we add a new level at the bottom in the future)
 *  i.e. a lower quality level always refers to a lower bit rate
 *
 *  For a given bit rate level, the encoder will never encode at a bit rate higher than that defined by
 *  the corresponding mapping.
 */
typedef struct bitrate_level_map_payload_t
{
   uint32_t num_levels;
#if 0
   bitrate_level_map_t br_lev_tbl[num_levels];
#endif
}bitrate_level_map_payload_t;


/** ID of the encoder Bit Rate level parameter for the #AVS_MODULE_ID_ENCODER module.
    This parameter can be set at run time.
*/
#define AVS_ENCODER_PARAM_ID_BIT_RATE_LEVEL 0x000132E0

/*  Payload of the AVS_ENCODER_PARAM_ID_BIT_RATE_LEVEL parameter */
typedef struct dec_to_enc_comm_bit_rate_level_payload_t
{
   uint32_t level;
}dec_to_enc_comm_bit_rate_level_payload_t;


/** Purpose ID used for IMC communication parameter

    This parameter can be set at run time.
*/
#define AVS_ENCDEC_PURPOSE_ID_BT_INFO 0x000132E2

/** ID of the decoder to encoder communication parameter for
 *  both #AVS_MODULE_ID_ENCODER and #AVS_MODULE_ID_DECODER to set up IMC registration

    This parameter can be set at run time.
*/
#define AVS_ENCDEC_PARAM_ID_DEC_TO_ENC_COMMUNICATION 0x0001323D

/* Enum to decide communication direction */
typedef enum
{
   TRANSMIT = 0,
   RECEIVE
}comm_dir_t;

/*  Payload of the AVS_ENCDEC_PARAM_ID_DEC_TO_ENC_COMMUNICATION parameter */
typedef struct dec_to_enc_communication_payload_t
{
   comm_dir_t comm_dir;

   uint32_t enable;

   uint32_t purpose;

   uint32_t comm_instance;

}dec_to_enc_communication_payload_t;

/*------------------------------------------------------------------------------
 * Rx/Tx module IMC data payload
 *----------------------------------------------------------------------------*/
/** Payload header structure used by the parameter IDs. The payload of a
    parameter structure is followed by this header.

    The source module must update this structure before sending it to the
    destination module.
*/

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _ADSP_AVS_COMMON_API_H */
