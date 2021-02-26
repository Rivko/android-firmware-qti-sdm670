/*========================================================================*/
/**
@file adsp_asm_stream_commands_prop.h

This file contains ASM stream commands and events structures definitions.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      audio_mainpage.dox file. Contact Tech Pubs for assistance.
===========================================================================*/
/*===========================================================================
Copyright (c) 2010-2016 Qualcomm Technologies, Inc.
All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
======================================================================== */
/*========================================================================
Edit History

when       who     what, where, why
--------   ---      -------------------------------------------------------
08/23/16   sw      (Tech Pubs) Merged Doxygen comments from AVS 2.7;
                    edited Doxygen comments for AVS 2.8.
08\17\16    HB      Create File for properietary information.
========================================================================== */

#ifndef _ADSP_ASM_STREAM_COMMANDS_PROP_H_
#define _ADSP_ASM_STREAM_COMMANDS_PROP_H_

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/* ========================================================================== 
 NOTE: These comments are for asm_stream_cmd_open_transcode_loopback_t and
       asm_stream_cmd_open_write_compressed_t
 ========================================================================== */
/* asm_stream_cmd_open_transcode_loopback_t::src_format_id
Specifies the media format of the input audio stream.
*/
/** @weakgroup weakf_src_format_id
@{ @vertspace{-5}
- #ASM_MEDIA_FMT_DTS
- #ASM_MEDIA_FMT_EAC3
- #ASM_MEDIA_FMT_AC3 @tablebulletend
@} */

/* asm_stream_cmd_open_transcode_loopback_t::sink_format_id
Specifies the media format of the output stream.
*/
/** @weakgroup weakf_sink_format_id
@{ @vertspace{-5}
- #ASM_MEDIA_FMT_DTS (not supported in Low Latency mode)
- #ASM_MEDIA_FMT_EAC3 (not supported in Low Latency mode)
- #ASM_MEDIA_FMT_AC3 (not supported in Low Latency mode) @tablebulletend
@} */

/* asm_stream_cmd_open_write_compressed_t::fmt_id
   Specifies the media type of the HDMI stream to be opened.
*/
/** @weakgroup weakf_fmt_id
@{
For the formats indicated by *, only IEC 61937 Pass-through mode is
currently supported.

@values
- #ASM_MEDIA_FMT_AC3
- #ASM_MEDIA_FMT_EAC3
- #ASM_MEDIA_FMT_DTS
- #ASM_MEDIA_FMT_ATRAC *
- #ASM_MEDIA_FMT_MAT *
- #ASM_MEDIA_FMT_AAC_V2 *
- #ASM_MEDIA_FMT_MP3 *
- #ASM_MEDIA_FMT_WMA_V10PRO_V2 * @tablebulletend
@} */

/* ========================================================================== */


/** @ingroup asmstrm_cmd_open_tcode_lpbk
    Converter mode is DDP-to-DD. */
#define ASM_DDP_DD_CONVERTER_MODE   ASM_CONVERTER_MODE_1

/** @addtogroup asmstrm_cmd_set_encdec_params
@{ */
/** Parameter ID of the AAC decoder SBR/PS Enable flag in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    @msgpayload{asm_aac_sbr_ps_flag_param_t}
    @table{weak__asm__aac__sbr__ps__flag__param__t}
*/
#define ASM_PARAM_ID_AAC_SBR_PS_FLAG                                0x00010C63

/** Flag to turn off both SBR and PS processing, if they are present in the
    bit stream.
*/
#define ASM_AAC_SBR_OFF_PS_OFF                                      (2)

/** Flag to turn on SBR but turn off PS processing,if they are present in the
    bit stream.
*/
#define ASM_AAC_SBR_ON_PS_OFF                                       (1)

/** Flag to turn on both SBR and PS processing, if they are present in the
    bit stream (default behavior). @newpage
*/
#define ASM_AAC_SBR_ON_PS_ON                                        (0)

/** @} */ /* end_addtogroup asmstrm_cmd_set_encdec_params */

/* Structure for an AAC SBR PS processing flag. */
typedef struct asm_aac_sbr_ps_flag_param_t asm_aac_sbr_ps_flag_param_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_aac_sbr_ps_flag_param_t
@{ */
/* Payload of the ASM_PARAM_ID_AAC_SBR_PS_FLAG parameter in the
    ASM_STREAM_CMD_SET_ENCDEC_PARAM command.
*/
struct asm_aac_sbr_ps_flag_param_t
{
    uint32_t                  sbr_ps_flag;
    /**< Control parameter that enables or disables SBR/PS processing in the
         AAC bit stream. Changes are applied to the next decoded frame.

         @values
         - #ASM_AAC_SBR_OFF_PS_OFF
         - #ASM_AAC_SBR_ON_PS_OFF
         - #ASM_AAC_SBR_ON_PS_ON (Default)

         All other values are invalid. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_aac_sbr_ps_flag_param_t */

/** @addtogroup asmstrm_cmd_set_encdec_params
@{ */
/** ID of the AAC decoder Dual Mono Channel Mapping parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    This parameter applies only to AAC dual mono bit stream decoding. The
    command can be sent any time after opening a write or read/write stream,
    and the changes take effect from the next decoded frame.

    The same SCE can be routed to both he left and right channels. The
    following table shows the message payload for AAC dual mono channel
    mapping, where all the channels are valid.

    @msgpayload{asm_aac_dual_mono_mapping_param_t}
    @table{weak__asm__aac__dual__mono__mapping__param__t}
*/
#define ASM_PARAM_ID_AAC_DUAL_MONO_MAPPING                      0x00010C64

/** First single channel element in a dual mono bit stream. */
#define ASM_AAC_DUAL_MONO_MAP_SCE_1                                 (1)

/** Second single channel element in a dual mono bit stream. @newpage */
#define ASM_AAC_DUAL_MONO_MAP_SCE_2                                 (2)

/** @} */ /* end_addtogroup asmstrm_cmd_set_encdec_params */

/* Structure for AAC decoder dual mono channel mapping. */
typedef struct asm_aac_dual_mono_mapping_param_t asm_aac_dual_mono_mapping_param_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_aac_dual_mono_mapping_param_t
@{ */
/* Payload of the ASM_PARAM_ID_AAC_DUAL_MONO_MAPPING parameter in the
    ASM_STREAM_CMD_SET_ENCDEC_PARAM command.
*/
struct asm_aac_dual_mono_mapping_param_t
{
    uint16_t                  left_channel_sce;
    /**< Specifies which SCE is connected to the left channel.

         @values
         - #ASM_AAC_DUAL_MONO_MAP_SCE_1 (Default)
         - #ASM_AAC_DUAL_MONO_MAP_SCE_2 @tablebulletend */

    uint16_t                  right_channel_sce;
    /**< Specifies which SCE is connected to the right channel.

         @values
         - #ASM_AAC_DUAL_MONO_MAP_SCE_1 (Default)
         - #ASM_AAC_DUAL_MONO_MAP_SCE_2 @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_aac_dual_mono_mapping_param_t */

/** @addtogroup asmstrm_cmd_set_encdec_params
@{ */
/** ID of the AAC decoder Stereo Mixing parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    This parameter applies only to AAC bit stream decoding. The command is sent
    after opening a write or read/write stream and before actual decoding
    begins. The parameter is used to select the stereo mix coefficients
    between the ISO/IEC and ARIB standards.

    Do not send this command during decoding. Such a change can cause
    glitches in the stereo output.

    @msgpayload{asm_aac_stereo_mix_coeff_selection_param_t}
    @table{weak__asm__aac__stereo__mix__coeff__selection__param__t}
*/
#define ASM_PARAM_ID_AAC_STEREO_MIX_COEFF_SELECTION_FLAG           0x00010DD8

/** Use AAC stereo mix coefficients defined in ISO/IEC. */
#define ASM_AAC_STEREO_MIX_COEFF_ISO                                (0)

/** Use AAC stereo mix coefficients defined in ARIB. @newpage */
#define ASM_AAC_STEREO_MIX_COEFF_ARIB                               (1)

/** @} */ /* end_addtogroup asmstrm_cmd_set_encdec_params */

/* Structure for AAC decoder dual mono channel mapping. */
typedef struct asm_aac_stereo_mix_coeff_selection_param_t asm_aac_stereo_mix_coeff_selection_param_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_aac_stereo_mix_coeff_selection_param_t
@{ */
/* Payload of the ASM_PARAM_ID_AAC_STEREO_MIX_COEFF_SELECTION_FLAG parameter
    in the ASM_STREAM_CMD_SET_ENCDEC_PARAM command.
*/
struct asm_aac_stereo_mix_coeff_selection_param_t
{
    uint32_t      aac_stereo_mix_coeff_flag;
    /**< Specifies which standard is used for AAC stereo mix.

         @values
         - #ASM_AAC_STEREO_MIX_COEFF_ISO (Default)
         - #ASM_AAC_STEREO_MIX_COEFF_ARIB @tablebulletend */

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_aac_stereo_mix_coeff_selection_param_t */

/** @addtogroup asmstrm_cmd_set_encdec_params
@{ */
/** ID of the AAC Decoder Dual Mono Channel Mapping parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    This parameter applies only to the AAC SBR signaling mechanism. The
    command can be sent any time after opening a write or read/write stream,
    and the changes take effect from the next decoded frame.

    @msgpayload{asm_aac_sbr_signalling_param_t}
    @table{weak__asm__aac__sbr__signalling__param__t}
*/
#define ASM_PARAM_ID_AAC_SBR_SIGNALLING                      0x00012F30

/** AAC extension audio object type is Low Complexity (LC). */
#define ASM_AAC_EXTENSION_AOT_LC                               2

/** AAC extension audio object type is SBR. */
#define ASM_AAC_EXTENSION_AOT_SBR                                5

/** AAC extension audio object type is PS. */
#define ASM_AAC_EXTENSION_AOT_PS                                 29

/** @} */ /* end_addtogroup asmstrm_cmd_set_encdec_params */

/* Structure for AAC SBR signalling parameters. */
typedef struct asm_aac_sbr_signalling_param_t asm_aac_sbr_signalling_param_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_aac_sbr_signalling_param_t
@{ */
/* Payload of the ASM_PARAM_ID_AAC_SBR_SIGNALLING parameter in the
    ASM_STREAM_CMD_SET_ENCDEC_PARAM command.
*/
struct asm_aac_sbr_signalling_param_t
{
    uint32_t                  extension_aot;
    /**< Specifies the extension audio object type in a stream.

         @values
         - #ASM_AAC_EXTENSION_AOT_LC (Default)
         - #ASM_AAC_EXTENSION_AOT_SBR
         - #ASM_AAC_EXTENSION_AOT_PS

         If this value is set as ASM_AAC_EXTENSION_AOT_LC, all remaining fields
         are ignored and the decoder operates in implicit signaling mode. */

    uint32_t                  extension_sampling_rate;
    /**< Specifies the extension sampling rate.

         @values
         - Initialized to core sampling rate of decoder (Default)
         - All sampling rates applicable to the core AAC decoder
         @tablebulletend */

    uint32_t                  sbr_present;
    /**< Specifies whether to explicitly assume the presence or absence of the
         SBR payload in the stream.

         @values
         - 0 -- SBR payload is absent (Default)
         - 1 -- SBR payload is present @tablebulletend */

    uint32_t                  ps_present;
    /**< Specifies whether to explicitly assume the presence or absence of the
         PS payload in the stream.

         @values
         - 0 -- PS payload is absent (Default)
         - 1 -- PS payload is present @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_aac_dual_mono_mapping_param_t */


/* The following AC3 parameters make use of the generic
 *  structure asm_ac3_generic_param_t
 */
/** @addtogroup asmstrm_cmd_set_encdec_params
@{ */
/** ID of the AC3 Karaoke Mode parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    This parameter specifies the vocal channels for Karaoke mode, so it is
    relevant only for karaoke streams, and it applies only to the E-AC3
    decoder.

    The command can be sent any time after opening a write or
    read/write stream, and the change takes effect from the next decoded frame.

    @msgpayload
    For the payload format, see asm_ac3_generic_param_t.
    @par
    @values
    - #AC3_KARAOKE_MODE_NO_VOCAL
    - #AC3_KARAOKE_MODE_LEFT_VOCAL
    - #AC3_KARAOKE_MODE_RIGHT_VOCAL
    - #AC3_KARAOKE_MODE_BOTH_VOCAL (Default)
*/
#define ASM_PARAM_ID_AC3_KARAOKE_MODE                               0x00010D73

/** Enumeration for no vocal channels in a karaoke stream. */
#define AC3_KARAOKE_MODE_NO_VOCAL                                   (0)

/** Enumeration for only the left vocal channel in a karaoke stream. */
#define AC3_KARAOKE_MODE_LEFT_VOCAL                                 (1)

/** Enumeration for only the right vocal channel in a karaoke stream. */
#define AC3_KARAOKE_MODE_RIGHT_VOCAL                                (2)

/** Enumeration for both vocal channels in a karaoke stream. (Default) @newpage */
#define AC3_KARAOKE_MODE_BOTH_VOCAL                                 (3)

/** @} */ /* end_addtogroup asmstrm_cmd_set_encdec_params */

/** @addtogroup asmstrm_cmd_set_encdec_params
@{ */
/** ID of the AC3 DRC Mode parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    This parameter applies only to the E-AC3 decoder, and it specifies the
    compression that must be applied. The command can be sent any time after
    opening a write or read/write stream, and the change takes effect from the
    next decoded frame.

    @msgpayload
    For the payload format, see asm_ac3_generic_param_t.
    @par
    @values
    - #AC3_DRC_MODE_CUSTOM_ANALOG
    - #AC3_DRC_MODE_CUSTOM_DIGITAL
    - #AC3_DRC_MODE_LINE_OUT (Default)
    - #AC3_DRC_MODE_RF_REMOD
    - #AC3_DRC_PORTABLE_MODE_8
    - #AC3_DRC_PORTABLE_MODE_11
    - #AC3_DRC_PORTABLE_MODE_14
*/
#define ASM_PARAM_ID_AC3_DRC_MODE                                   0x00010D74

/** Enumeration for the Custom Analog mode. This custom mode is applied when
    DRC is scaled using the DRC Cut/Boost Scale factor.
*/
#define AC3_DRC_MODE_CUSTOM_ANALOG                                  (0)

/** Enumeration for the Custom Digital mode. This custom mode is applied when
    DRC be scaled using the DRC Cut/Boost Scale factor.
*/
#define AC3_DRC_MODE_CUSTOM_DIGITAL                                 (1)

/** Enumeration for the Line Out mode (light compression -- the default).
    This mode is used for most applications. It applies a normal compression
    factor (dynrng) present in the bit stream.
*/
#define AC3_DRC_MODE_LINE_OUT                                       (2)

/** Enumeration for the RF Remodulation mode. This mode is applied if the
    output is used to modulate an RF signal. It applies the heavy compression
    factor (compr) present in the bit stream.
*/
#define AC3_DRC_MODE_RF_REMOD                                       (3)

/** Enumeration for RF Remodulation mode 8, which is applied if 8 dB DRC
    suppression is required when modulating an RF signal. This mode applies
    the heavy compression factor (compr) present in the bit stream.
*/
#define AC3_DRC_PORTABLE_MODE_8                                     (4)

/** Enumeration for RF Remodulation mode 11, which is applied if 11 dB
    DRC suppression is required when modulating an RF signal. This mode applies
    the heavy compression factor (compr) present in the bit stream.
*/
#define AC3_DRC_PORTABLE_MODE_11                                    (5)

/** Enumeration for RF Remodulation mode 14, which is applied if 14 dB
    DRC suppression is required when modulating an RF signal. This mode applies
    the heavy compression factor (compr) present in the bit stream. @newpage
*/
#define AC3_DRC_PORTABLE_MODE_14                                    (6)

/** @} */ /* end_addtogroup asmstrm_cmd_set_encdec_params */

/** @addtogroup asmstrm_cmd_set_encdec_params
@{ */
/** ID of the AC3 Dual Mono Mode parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    This parameter specifies how the dual mono channels are to be played out.
    It is relevant only for dual mono streams, and it applies only to the E-AC3
    decoder.

    The command can be sent any time after opening a write or
    read/write stream, and the change takes effect from the next decoded frame.

    @msgpayload
    For the payload format, see asm_ac3_generic_param_t.
    @par
    @values
    - #AC3_DUAL_MONO_MODE_STEREO (Default)
    - #AC3_DUAL_MONO_MODE_LEFT_MONO
    - #AC3_DUAL_MONO_MODE_RIGHT_MONO
    - #AC3_DUAL_MONO_MODE_MIXED_MONO
*/
#define ASM_PARAM_ID_AC3_DUAL_MONO_MODE                             0x00010D75

/** Enumeration for playing dual mono in Stereo mode. (Default) */
#define AC3_DUAL_MONO_MODE_STEREO                                   (0)

/** Enumeration for playing left mono. */
#define AC3_DUAL_MONO_MODE_LEFT_MONO                                (1)

/** Enumeration for playing right mono. */
#define AC3_DUAL_MONO_MODE_RIGHT_MONO                               (2)

/** Enumeration for mixing and playing both dual mono channels. @newpage */
#define AC3_DUAL_MONO_MODE_MIXED_MONO                               (3)

/** @} */ /* end_addtogroup asmstrm_cmd_set_encdec_params */

/** @addtogroup asmstrm_cmd_set_encdec_params
@{ */
/** ID of the AC3 Stereo Downmix Mode parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    This parameter specifies the type of stereo downmix to be used. It is
    applicable when downmixing to two channels, and it applies only to the
    E-AC3 decoder. The command can be sent any time after opening a write or
    read/write stream, and the change takes effect from the next decoded frame.

    @msgpayload
    For the payload format, see asm_ac3_generic_param_t.
    @par
    @values
    - #AC3_STEREO_DOWNMIX_MODE_AUTO_DETECT (Default).
    - #AC3_STEREO_DOWNMIX_MODE_LT_RT -- LT/RT downmix preserves the
      surround information. It is useful if the downmixed audio is played
      out using a Pro-Logic decoder.
    - #AC3_STEREO_DOWNMIX_MODE_LO_RO -- LO/RO is useful if the stereo
      stream is further downmixed to mono.
*/
#define ASM_PARAM_ID_AC3_STEREO_DOWNMIX_MODE                        0x00010D76

/** Enumeration for using the Downmix mode indicated in the bit stream.
    (Default)
*/
#define AC3_STEREO_DOWNMIX_MODE_AUTO_DETECT                         (0)

/** Enumeration for Surround Compatible mode. This mode preserves the surround
    information. It is useful if the downmixed audio is played out using a
    Pro-Logic decoder.
*/
#define AC3_STEREO_DOWNMIX_MODE_LT_RT                               (1)

/** Enumeration for Mono Compatible mode. This mode is useful if the output is
    to be further downmixed to mono. @newpage
*/
#define AC3_STEREO_DOWNMIX_MODE_LO_RO                               (2)

/** @} */ /* end_addtogroup asmstrm_cmd_set_encdec_params */

/** @addtogroup asmstrm_cmd_set_encdec_params
@{ */
/** ID of the AC3 PCM Scale Factor parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    This parameter is a gain setting for the output audio, and it
    applies only to the E-AC3 decoder. The command can be sent
    any time after opening a write or read/write stream, and the change takes
    effect from the next decoded frame.

    @msgpayload
    Specifies a gain value (0 to 1) in Q31 format applied to all samples
    in the stream. For the payload format, see asm_ac3_generic_param_t.
    @par
    @values 0 to 100 (Default is 100)
*/
#define ASM_PARAM_ID_AC3_PCM_SCALEFACTOR                            0x00010D78

/** ID of the AC3 DRC Boost Scale Factor parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.
    This parameter is used to scale the DRC boost parameter, which is useful
    in controlling the extent to which DRC is applied.

    This parameter applies only to the E-AC3 decoder. The command can be sent
    any time after opening a write or read/write stream, and the change takes
    effect from the next decoded frame.

    @msgpayload
    Specifies a gain value (0 to 1) in Q31 format applied to the DRC
    boost scale factor. By setting this value to 1, full DRC is applied.
    For the payload format, see asm_ac3_generic_param_t.
    @par
    @values 0 to 100 (Default is 100)
*/
#define ASM_PARAM_ID_AC3_DRC_BOOST_SCALEFACTOR                      0x00010D79

/** ID of the AC3 DRC Cut Scale Factor parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.
    This parameter is used to scale the DRC cut parameter, which is useful
    in controlling the extent to which DRC is applied.

    This parameter applies only to the E-AC3 decoder. The command can be sent
    any time after opening a write or read/write stream, and the change takes
    effect from the next decoded frame.

    @msgpayload
    Specifies a gain value (0 to 1) in Q31 format applied to the DRC cut
    scale factor. By setting this value to 1, full DRC is applied.
    For the payload format, see asm_ac3_generic_param_t.
    @par
    @values 0 to 100 (Default is 100) @newpage
*/
#define ASM_PARAM_ID_AC3_DRC_CUT_SCALEFACTOR                        0x00010D7A

/** ID of the AC3 Channel Configuration parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    This parameter applies only to the E-AC3 decoder. The command can be sent
    any time after opening a write or read/write stream, and the change takes
    effect from the next decoded frame.

    The client is responsible for setting the appropriate channel
    configuration, depending on the device settings of write streams or the
    required output format of read/write streams.
    Channel configuration decides the type of downmixing that is applied
    by the decoder to get the output channels.

    @msgpayload
    For the payload format, see asm_ac3_generic_param_t.
    @par
    @values See Sections @xref{hdr:AC3ChannelEnumFirst} through
            @xref{hdr:AC3ChannelEnumLast}
*/
#define ASM_PARAM_ID_AC3_CHANNEL_CONFIG                                0x00010DE0

/** @} */ /* end_addtogroup asmstrm_cmd_set_encdec_params */

/** @addtogroup asmstrm_cmd_set_encdec_params
@{ */
/* Enumeration for a AC3 channel configuration Notation
AC3_CHANNEL_CONFIG_<number of front channels>_<number of surround_channels>_<number of extension channels if they exist>_<channel list>
*/
/** @xreflabel{hdr:AC3ChannelEnumFirst}
    Configuration for Raw mode (no downmixing). Play out all channels in the
    bit stream as is without any downmixing.
*/
#define AC3_CHANNEL_CONFIG_RAW                                      (-1)

/** Reserved configuration. */
#define AC3_CHANNEL_CONFIG_RSVD                                     (0)

/** Configuration for one channel (mono).
    - Speaker front center
*/
#define AC3_CHANNEL_CONFIG_1_0_C                                    (1)

/** Configuration for two channels (stereo, default).
    - Speaker front left
    - Speaker front right
*/
#define AC3_CHANNEL_CONFIG_2_0_L_R                                  (2)

/** Configuration for three channels.
    - Speaker front left
    - Speaker front center
    - Speaker front right
    - No surround
    - No extension
*/
#define AC3_CHANNEL_CONFIG_3_0_L_C_R                                (3)

/** Configuration for three channels.
    - Speaker front left
    - Speaker front right
    - Surround mono
    - No extension
*/
#define AC3_CHANNEL_CONFIG_2_1_L_R_l                                (4)

/** Configuration for four channels.
    - Speaker front left
    - Speaker front center
    - Speaker front right
    - Surround mono
    - No extension
*/
#define AC3_CHANNEL_CONFIG_3_1_L_C_R_l                              (5)

/** Configuration for four channels.
    - Speaker front left
    - Speaker front right
    - Surround left
    - Surround right
    - No extension
*/
#define AC3_CHANNEL_CONFIG_2_2_L_R_l_r                              (6)

/** Configuration for five channels.
    - Speaker front left
    - Speaker front center
    - Speaker front right
    - Surround left
    - Surround right
    - No extension @newpage
*/
#define AC3_CHANNEL_CONFIG_3_2_L_C_R_l_r                            (7)

/** Configuration for four channels.
    - Speaker front left
    - Speaker front center
    - Speaker front right
    - No surround
    - Extension center vertical height
*/
#define AC3_CHANNEL_CONFIG_3_0_1_L_C_R_Cvh                          (8)

/** Configuration for five channels.
    - Speaker front left
    - Speaker front right
    - Surround left
    - Surround right
    - Extension top surround
*/
#define AC3_CHANNEL_CONFIG_2_2_1_L_R_l_r_Ts                         (9)

/** Configuration for six channels.
    - Speaker front left
    - Speaker front center
    - Speaker front right
    - Surround left
    - Surround right
    - Extension top surround
*/
#define AC3_CHANNEL_CONFIG_3_2_1_L_C_R_l_r_Ts                       (10)

/** Configuration for six channels.
    - Speaker front left
    - Speaker front center
    - Speaker front right
    - Surround left
    - Surround right
    - Extension center vertical height @newpage
*/
#define AC3_CHANNEL_CONFIG_3_2_1_L_C_R_l_r_Cvh                      (11)

/** Configuration for five channels.
    - Speaker front left
    - Speaker front center
    - Speaker front right
    - No surround
    - Extension left center
    - Extension right center
*/
#define AC3_CHANNEL_CONFIG_3_0_2_L_C_R_Lc_Rc                        (12)

/** Configuration for six channels.
    - Speaker front left
    - Speaker front right
    - Surround left
    - Surround right
    - Extension left wide
    - Extension right wide
*/
#define AC3_CHANNEL_CONFIG_2_2_2_L_R_l_r_Lw_Rw                      (13)

/** Configuration for six channels.
    - Speaker front left
    - Speaker front right
    - Surround left
    - Surround right
    - Extension left vertical height
    - Extension right vertical height @newpage
*/
#define AC3_CHANNEL_CONFIG_2_2_2_L_R_l_r_Lvh_Rvh                    (14)

/** Configuration for six channels.
    - Speaker front left
    - Speaker front right
    - Surround left
    - Surround right
    - Extension left surround direct
    - Extension right surround direct
*/
#define AC3_CHANNEL_CONFIG_2_2_2_L_R_l_r_Lsd_Rsd                    (15)

/** Configuration for six channels.
    - Speaker front left
    - Speaker front right
    - Surround left
    - Surround right
    - Extension left rear surround
    - Extension right rear surround
*/
#define AC3_CHANNEL_CONFIG_2_2_2_L_R_l_r_Lrs_Rrs                    (16)

/** Configuration for seven channels.
    - Speaker front left
    - Speaker front center
    - Speaker front right
    - Surround left
    - Surround right
    - Extension left center
    - Extension right center @newpage
*/
#define AC3_CHANNEL_CONFIG_3_2_2_L_C_R_l_r_Lc_Rc                    (17)

/** Configuration for seven channels.
    - Speaker front left
    - Speaker front center
    - Speaker front right
    - Surround left
    - Surround right
    - Extension left wide
    - Extension right wide
*/
#define AC3_CHANNEL_CONFIG_3_2_2_L_C_R_l_r_Lw_Rw                    (18)

/** Configuration for seven channels.
    - Speaker front left
    - Speaker front center
    - Speaker front right
    - Surround left
    - Surround right
    - Extension left vertical height
    - Extension right vertical height
*/
#define AC3_CHANNEL_CONFIG_3_2_2_L_C_R_l_r_Lvh_Rvh                  (19)

/** Configuration for seven channels.
    - Speaker front left
    - Speaker front center
    - Speaker front right
    - Surround left
    - Surround right
    - Extension left surround direct
    - Extension right surround direct @newpage
*/
#define AC3_CHANNEL_CONFIG_3_2_2_L_C_R_l_r_Lsd_Rsd                  (20)

/** Configuration for seven channels. (Default)
    - Speaker front left
    - Speaker front center
    - Speaker front right
    - Surround left
    - Surround right
    - Extension left rear surround
    - Extension right rear surround
*/
#define AC3_CHANNEL_CONFIG_3_2_2_L_C_R_l_r_Lrs_Rrs                  (21)

/** @xreflabel{hdr:AC3ChannelEnumLast}
    Configuration for seven channels.
    - Speaker front left
    - Speaker front center
    - Speaker front right
    - Surround left
    - Surround right
    - Extension top surround
    - Extension center vertical height @newpage
*/
#define AC3_CHANNEL_CONFIG_3_2_2_L_C_R_l_r_Ts_Cvh                   (22)

/** @} */ /* end_addtogroup asmstrm_cmd_set_encdec_params */

/** @addtogroup asmstrm_cmd_set_encdec_params
@{ */
/** ID of the AC3 Low Frequency Effects (LFE) mode parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    This parameter applies only to the E-AC3 decoder. The command can be sent
    any time after opening a write or read/write stream, and the change takes
    effect from the next decoded frame.

    The client is responsible for setting this parameter, depending on the
    device settings of write streams or the required output format of
    read/write streams.

    @msgpayload
    Specifies the number of LFE channels in the output.
    For the payload format, see asm_ac3_generic_param_t.
    @par
    @values
    - #AC3_LFE_MODE_NONE
    - #AC3_LFE_MODE_1_LFE (Default)
    - #AC3_LFE_MODE_2_LFE
    @par
    @note1hang DDP streams can contain up to two LFE channels.
*/
#define ASM_PARAM_ID_AC3_LFE_MODE                                    0x00010DE1

/** Enumeration for no LFE in the output. */
#define AC3_LFE_MODE_NONE                                           (0)

/** Enumeration for one LFE channel in the output. (Default) */
#define AC3_LFE_MODE_1_LFE                                          (1)

/** Enumeration for two LFE channels in the output.

    @note1hang Dolby Digital Plus streams can contain up to two LFE
               channels. @newpage
*/
#define AC3_LFE_MODE_2_LFE                                          (2)

/** @} */ /* end_addtogroup asmstrm_cmd_set_encdec_params */

/** @addtogroup asmstrm_cmd_set_encdec_params
@{ */
/** ID of the AC3 Number of Output Channels parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    This parameter applies only to the E-AC3 decoder. The command can be sent
    any time after opening a write or read/write stream, and the change takes
    effect from the next decoded frame.

    The client is responsible for setting the appropriate number of output
    channels, depending on the device settings of write streams or the required
    output format of read/write streams.
    - If more channels are specified here than those specified in the channel
      configuration, the additional channels will contain zero samples.
    - If fewer channels are specified, only the initial channels up to the
      number specified will be present in the output buffer. There will not be
      any additional downmixing.

    @msgpayload
    Specifies the number of channels in the output buffer of the decoder.
    The client must set the number of channels to be equal to the number
    of channels in the channel configuration (#ASM_PARAM_ID_AC3_CHANNEL_CONFIG).
    @par
    For the payload format, see asm_ac3_generic_param_t.
    @par
    @values 1 to 8 (Default = 8)
*/
#define ASM_PARAM_ID_AC3_NUM_OUTPUT_CHANNELS                        0x00010DE2

/** ID of the AC3 Error Conceal parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    When enabled and if an error occurs, this parameter fills output PCM
    buffers with data repeated from the last block that decoded without error
    until the specified count (#ASM_PARAM_ID_AC3_ERROR_MAX_RPTS) is reached.

    This parameter applies only to the E-AC3 decoder. The command can be sent
    any time after opening a write or read/write stream, and the change takes
    effect from the next decoded frame.

    @msgpayload
    Specifies a value that is applied to the Error Conceal parameter.
    For the payload format, see asm_ac3_generic_param_t.
    @par
    @values
    - 0 -- Disable error concealment
    - 1 -- Enable error concealment (Default) @newpage
*/
#define ASM_PARAM_ID_AC3_ERROR_CONCEAL                              0x00010DE3

/** ID of the AC3 Error Maximum Repeats parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    If an error occurs, this parameter is used to specify the maximum number
    of consecutive block repeats that are allowed before the output channels
    are automatically muted.

    This parameter applies only to the E-AC3 decoder. The command can be sent
    any time after opening a write or read/write stream, and the change takes
    effect from the next decoded frame.

    @msgpayload
    Specifies a value (in Q31 format) that is applied to the Error Max repeat
    parameter.
    For the payload format, see asm_ac3_generic_param_t.
    @par
    @values
    - 0 -- Enable mute
    - -1 -- Block repeats continuously for an indefinite period of time
    (Default)
*/
#define ASM_PARAM_ID_AC3_ERROR_MAX_RPTS                             0x00010DE4

/** ID of the AC3 Converter Error Conceal parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    When enabled and if an error occurs, this parameter fills output PCM
    buffers with data repeated from the last block that converted without
    error until the specified count (#ASM_PARAM_ID_AC3_ERROR_MAX_RPTS) is
    reached.

    This parameter applies only to the E-AC3 converter. The command can be sent
    any time after opening a write or read/write stream, and the change takes
    effect from the next decoded frame.

    @msgpayload
    Specifies a value that is applied to the Converter Error Conceal parameter.
    For the payload format, see asm_ac3_generic_param_t.
    @par
    @values
    - 0 -- Disable converter error concealment
    - 1 -- Enable converter error concealment (Default) @newpage
*/
#define ASM_PARAM_ID_AC3_CNV_ERROR_CONCEAL                          0x00010DE5

/** ID of the AC3 Substream Select parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.
    This parameter selects the type of substream that specifies the
    associated program for decoding.  A substream is a subcomponent of the
    overall bit stream, specific to E-AC3, which can be either dependent or
    independent as specified by the associated semantics.

    This parameter applies only to the E-AC3 decoder. The command can be sent
    any time after opening a write or read/write stream, and the change takes
    effect from the next decoded frame.

    @msgpayload
    Specifies a value that is applied to the Substream Select parameter.
    Setting this value to 1, 2, or 3 enables the type substream select.
    For the payload format, see asm_ac3_generic_param_t.
    @par
    @values
    - 1 -- SUBSTREAMID_1
    - 2 -- SUBSTREAMID_2
    - 3 -- SUBSTREAMID_3
*/
#define ASM_PARAM_ID_AC3_SUBSTREAM_SELECT                           0x00010DE6

/** ID of the AC3 Modified Discrete Cosine Transform (MDCT) Band Limiting
    parameter in the #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    MDCT is the commonly used filter bank in audio coding, used to limit the.
    bandwidth.
    This parameter is used to enable or disable Band Limiting mode.

    This parameter applies only to the E-AC3 decoder. The command can be sent
    any time after opening a write or read/write stream, and the change takes
    effect from the next decoded frame.

    @msgpayload
    Specifies a value that is applied to the MDCT band limiting factor.
    For the payload format, see asm_ac3_generic_param_t.
    @par
    @values
    - 0 -- Disable band limiting (Default)
    - 1    -- Enable band limiting @newpage
*/
#define ASM_PARAM_ID_AC3_MDCT_BANDLIMITING_MODE                     0x00010DE7

/** ID of the AC3 DRC Suppression mode parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.
    This parameter specifies whether to suppress DRC.

    This parameter applies only to the E-AC3 decoder. The command can be sent
    any time after opening a write or read/write stream, and the change takes
    effect from the next decoded frame.

    @msgpayload
    Specifies a value that is applied to the DRC Suppression mode.
    For the payload format, see asm_ac3_generic_param_t.
    @par
    @values
    - 0 -- Do not suppress DRC (Default)
    - 1 -- Suppress DRC
*/
#define ASM_PARAM_ID_AC3_DRC_SUPPRESSION_MODE                       0x00010DE8

/** ID of the AC3 External Boost parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    This parameter specifies the external boost value and supports only a
    GetParam functionality.

    This parameter applies only to the E-AC3 decoder. The command can be sent
    any time after opening a write or read/write stream, and the change takes
    effect from the next decoded frame.

    @msgpayload
    The parameter is an unsigned 32-bit integer and returns the external boost
    value. For the payload format, see asm_ac3_generic_param_t. @newpage
*/
#define ASM_PARAM_ID_AC3_EXT_BOOST                                  0x00010DEA

/** ID of the AC3 Audio Coding Mode Changes parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    This parameter is an unsigned 32-bit integer. It provides the state for
    E-AC3 decoder conformance testing, and it applies only to the E-AC3
    decoder. The command can be sent any time after opening a write or
    read/write stream, and the change takes effect from the next decoded frame.

    @msgpayload
    For the payload format, see asm_ac3_generic_param_t.
    @par
    @values
    - 0 -- Enable conformance
    - 1 -- Disable conformance (Default)
*/
#define ASM_PARAM_ID_AC3_ACMOD_CHANGES_STATE                       0x00010DF5

/* Structure for AC3 Generic Parameter. */
typedef struct asm_ac3_generic_param_t asm_ac3_generic_param_t;

#include "adsp_begin_pack.h"

/** Payload of the generic AC3 parameters in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.
*/
struct asm_ac3_generic_param_t
{
    uint32_t                  generic_parameter;
    /**< Generic parameter field used by the following AC3 parameter IDs:

         - #ASM_PARAM_ID_AC3_KARAOKE_MODE
         - #ASM_PARAM_ID_AC3_CHANNEL_CONFIG
         - #ASM_PARAM_ID_AC3_LFE_MODE
         - #ASM_PARAM_ID_AC3_DRC_MODE
         - #ASM_PARAM_ID_AC3_DUAL_MONO_MODE
         - #ASM_PARAM_ID_AC3_STEREO_DOWNMIX_MODE
         - #ASM_PARAM_ID_AC3_PCM_SCALEFACTOR
         - #ASM_PARAM_ID_AC3_DRC_BOOST_SCALEFACTOR
         - #ASM_PARAM_ID_AC3_DRC_CUT_SCALEFACTOR
         - #ASM_PARAM_ID_AC3_NUM_OUTPUT_CHANNELS
         - #ASM_PARAM_ID_AC3_ERROR_CONCEAL
         - #ASM_PARAM_ID_AC3_ERROR_MAX_RPTS
         - #ASM_PARAM_ID_AC3_CNV_ERROR_CONCEAL
         - #ASM_PARAM_ID_AC3_SUBSTREAM_SELECT
         - #ASM_PARAM_ID_AC3_MDCT_BANDLIMITING_MODE
         - #ASM_PARAM_ID_AC3_DRC_SUPPRESSION_MODE
         - #ASM_PARAM_ID_AC3_EXT_BOOST @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asmstrm_cmd_set_encdec_params */

/** @addtogroup asmstrm_cmd_set_encdec_params
@{ */

/** Enumeration for Raw mode (no downmixing), which specifies that all
    channels in the bit stream are to be played out as is without any
    downmixing. (Default)
*/
#define WMAPRO_CHANNEL_MASK_RAW                                      (-1)

/** Enumeration for setting the channel mask to 0. The 7.1 mode (Home
    Theater) is assigned.
*/
#define WMAPRO_CHANNEL_MASK_ZERO                                     0x0000

/** Speaker layout mask for one channel (Home Theater, mono).
    - Speaker front center
*/
#define WMAPRO_CHANNEL_MASK_1_C                                      0x0004

/** Speaker layout mask for two channels (Home Theater, stereo).
    - Speaker front left
    - Speaker front right
*/
#define WMAPRO_CHANNEL_MASK_2_L_R                                    0x0003

/** Speaker layout mask for three channels (Home Theater).
    - Speaker front left
    - Speaker front center
    - Speaker front right
*/
#define WMAPRO_CHANNEL_MASK_3_L_C_R                                  0x0007

/** Speaker layout mask for two channels (stereo).
    - Speaker back left
    - Speaker back right
*/
#define WMAPRO_CHANNEL_MASK_2_Bl_Br                                  0x0030

/** Speaker layout mask for four channels.
    - Speaker front left
    - Speaker front right
    - Speaker back left
    - Speaker back right
*/
#define WMAPRO_CHANNEL_MASK_4_L_R_Bl_Br                              0x0033

/** Speaker layout mask for four channels (Home Theater).
    - Speaker front left
    - Speaker front right
    - Speaker front center
    - Speaker back center
*/
#define WMAPRO_CHANNEL_MASK_4_L_R_C_Bc_HT                            0x0107

/** Speaker layout mask for five channels.
    - Speaker front left
    - Speaker front center
    - Speaker front right
    - Speaker back left
    - Speaker back right
*/
#define WMAPRO_CHANNEL_MASK_5_L_C_R_Bl_Br                            0x0037

/** Speaker layout mask for five channels (5 mode, Home Theater).
    - Speaker front left
    - Speaker front center
    - Speaker front right
    - Speaker side left
    - Speaker side right
*/
#define WMAPRO_CHANNEL_MASK_5_L_C_R_Sl_Sr_HT                         0x0607

/** Speaker layout mask for six channels (5.1 mode).
    - Speaker front left
    - Speaker front center
    - Speaker front right
    - Speaker back left
    - Speaker back right
    - Speaker low frequency
*/
#define WMAPRO_CHANNEL_MASK_5DOT1_L_C_R_Bl_Br_SLF                    0x003F

/** Speaker layout mask for six channels (5.1 mode, Home Theater).
    - Speaker front left
    - Speaker front center
    - Speaker front right
    - Speaker side left
    - Speaker side right
    - Speaker low frequency
*/
#define WMAPRO_CHANNEL_MASK_5DOT1_L_C_R_Sl_Sr_SLF_HT                 0x060F

/** Speaker layout mask for six channels (5.1 mode, no LFE).
    - Speaker front left
    - Speaker front center
    - Speaker front right
    - Speaker back left
    - Speaker back right
    - Speaker back center
*/
#define WMAPRO_CHANNEL_MASK_5DOT1_L_C_R_Bl_Br_Bc                     0x0137

/** Speaker layout mask for six channels (5.1 mode, Home Theater, no LFE).
    - Speaker front left
    - Speaker front center
    - Speaker front right
    - Speaker side left
    - Speaker side right
    - Speaker back center
*/
#define WMAPRO_CHANNEL_MASK_5DOT1_L_C_R_Sl_Sr_Bc_HT                  0x0707

/** Speaker layout mask for seven channels (6.1 mode).
    - Speaker front left
    - Speaker front center
    - Speaker front right
    - Speaker back left
    - Speaker back right
    - Speaker back center
    - Speaker low frequency
*/
#define WMAPRO_CHANNEL_MASK_6DOT1_L_C_R_Bl_Br_Bc_SLF                 0x013F

/** Speaker layout mask for seven channels (6.1 mode, Home Theater).
    - Speaker front left
    - Speaker front center
    - Speaker front right
    - Speaker side left
    - Speaker side right
    - Speaker back center
    - Speaker low frequency
*/
#define WMAPRO_CHANNEL_MASK_6DOT1_L_C_R_Sl_Sr_Bc_SLF_HT              0x070F

/** Speaker layout mask for seven channels (6.1 mode, no LFE).
    - Speaker front left
    - Speaker front center
    - Speaker front right
    - Speaker back left
    - Speaker back right
    - Speaker front left of center
    - Speaker front right of center @newpage
*/
#define WMAPRO_CHANNEL_MASK_6DOT1_L_C_R_Bl_Br_SFLOC_SFROC            0x00F7

/** Speaker layout mask for seven channels (6.1 mode, Home Theater, no LFE).
    - Speaker front left
    - Speaker front center
    - Speaker front right
    - Speaker side left
    - Speaker side right
    - Speaker front left of center
    - Speaker front right of center
*/
#define WMAPRO_CHANNEL_MASK_6DOT1_L_C_R_Sl_Sr_SFLOC_SFROC_HT         0x0637

/** Speaker layout mask for eight channels (7.1 mode).
    - Speaker front left
    - Speaker front center
    - Speaker front right
    - Speaker back left
    - Speaker back right
    - Speaker low frequency
    - Speaker front left of center
    - Speaker front right of center
*/
#define WMAPRO_CHANNEL_MASK_7DOT1_L_C_R_Bl_Br_SLF_SFLOC_SFROC        0x00FF

/** Speaker layout mask for eight channels (7.1 mode, Home Theater).
    - Speaker front left
    - Speaker front center
    - Speaker front right
    - Speaker side left
    - Speaker side right
    - Speaker low frequency
    - Speaker front left of center
    - Speaker front right of center @newpage
*/
#define WMAPRO_CHANNEL_MASK_7DOT1_L_C_R_Sl_Sr_SLF_SFLOC_SFROC_HT     0x063F

/** @} */ /* end_addtogroup asmstrm_cmd_set_encdec_params */

/** @addtogroup asmstrm_cmd_set_encdec_params
@{ */
/** ID of the DTS mix LFE Channel to Front Channels parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.
    If enabled, this parameter mixes the LFE channel to the front while
    downmixing (if necessary).

    @msgpayload
    For the payload format, see asm_dts_generic_param_t.
    @par
    @values
    - 0 -- Disable (Default)
    - 1 -- Enable
*/
#define ASM_PARAM_ID_DTS_MIX_LFE_TO_FRONT                          0x00010DB6

/** ID of the DTS Enable Parse REV2AUX parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.
    This flag enables parsing of the REV2AUX chunk in the bit stream. This chunk
    contains the broadcast metadata.

    @msgpayload
    For the payload format, see asm_dts_generic_param_t.
    @par
    @values
    - 0 -- Disable (Default)
    - 1 -- Enable
*/
#define ASM_PARAM_ID_DTS_ENABLE_PARSE_REV2AUX                         0x00010DB9

/* Structure for DTS Generic Parameter. */
typedef struct asm_dts_generic_param_t asm_dts_generic_param_t;

#include "adsp_begin_pack.h"

/** Payload of the DTS parameters in the #ASM_STREAM_CMD_SET_ENCDEC_PARAM
    command.
*/
struct asm_dts_generic_param_t
{
    int32_t                  generic_parameter;
    /**< Generic parameter used by the following DTS parameter IDs:
         - #ASM_PARAM_ID_DTS_MIX_LFE_TO_FRONT
         - #ASM_PARAM_ID_DTS_ENABLE_PARSE_REV2AUX @tablebulletend 
         @newpagetable */
}
#include "adsp_end_pack.h"
;

/** ID of the DTS LBR Mix LFE Channel to Front Channels parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.
    If enabled, this parameter mixes the LFE channel to the front while
    downmixing (if necessary).

    @msgpayload
    For the payload format, see asm_dts_lbr_generic_param_t.
    @par
    @values
    - 0 -- Disable (Default)
    - 1 -- Enable @newpage
*/
#define ASM_PARAM_ID_DTS_LBR_MIX_LFE_TO_FRONT                          0x00010DBC

/** ID of the DTS LBR Enable Parse REV2AUX parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.
    This flag enables parsing of the REV2AUX chunk in the bit stream. This chunk
    contains the broadcast metadata.

    @msgpayload
    For the payload format, see asm_dts_generic_param_t.
    @par
    @values
    - 0 -- Disable (Default)
    - 1 -- Enable
*/
#define ASM_PARAM_ID_DTS_LBR_ENABLE_PARSE_REV2AUX                    0x00010DBD

/* Structure for DTS LBR Generic Parameter. */
typedef struct asm_dts_lbr_generic_param_t asm_dts_lbr_generic_param_t;

#include "adsp_begin_pack.h"

/** Payload of the DTS LBR parameters in the #ASM_STREAM_CMD_SET_ENCDEC_PARAM
    command.
*/
struct asm_dts_lbr_generic_param_t
{
    int32_t                  generic_parameter;
    /**< Generic parameter used by the following DTS LBR parameter IDs:
         - #ASM_PARAM_ID_DTS_LBR_MIX_LFE_TO_FRONT
         - #ASM_PARAM_ID_DTS_LBR_ENABLE_PARSE_REV2AUX @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asmstrm_cmd_set_encdec_params */

/** @addtogroup asmstrm_cmd_set_encdec_params
@{ */
/** ID of the Data Rate/Bit Rate parameter for the DDP encoder in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    Each acmod has its own data rate. It signals the configuration of main
    audio channels to the decoder.

    @msgpayload
    For the payload format, see asm_ddp_enc_cfg_t.
    @par
    @values
    - For 1/0 acmod -- 192 data rate (Default)
    - For 2/0 acmod -- 192, 256 (Default) data rates
    - For 3/2 acmod -- 384 (Default), 448, 640 data rates
*/
#define ASM_PARAM_ID_DDP_ENC_DATA_RATE    0x00010DF0

/** ID of the DDP LFE Channel Enable parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.
    This parameter is available only when the 3/2 Audio Coding mode is selected
    (acmod = 7).

    @msgpayload
    For the payload format, see asm_ddp_enc_cfg_t.
    @par
    @values
    - 0 -- Disable
    - 1 -- Enable
*/
#define ASM_PARAM_ID_DDP_ENC_LFE          0x00010DF1

/** ID of the 90-degree Phase Shift Filter parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    This parameter conditions the signals that include surround channels.
    It is available only when the 3/2 Audio Coding mode is selected
    (acmod = 7). In this case, the filter default is ON.

    @msgpayload
    For the payload format, see asm_ddp_enc_cfg_t.
    @par
    @values
    - 1 -- ON
    - 2 -- OFF
*/
#define ASM_PARAM_ID_DDP_ENC_PH90_FILT    0x00010DF2

/** ID of the Global DRC Profile parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    To compress the dynamic range of a program, a DDP decoder can use
    DRC to reduce (cut) the level of the loudest sounds and increase (boost)
    the level of quiet sounds.

    @msgpayload
    For the payload format, see asm_ddp_enc_cfg_t.
    @par
    @values
    - 1 -- Film Standard (Default)
    - 3 -- Music Standard
*/
#define ASM_PARAM_ID_DDP_ENC_GBL_DRC_PROF 0x00010DF3

/** ID of the Dialogue Normalization (DialNorm) parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    This parameter represents a known DialNorm reference level (in dBFS) that
    is embedded in the bit stream as DialNorm metadata to ensure that the
    decoded audio can be played:

    @indent Attenuation = 31 + (DialNorm value)

    Examples:
    - If the DialNorm value is -31, attenuation is 0.
    - If the DialNorm value is -27, encoded PCM is 4 db attenuated compared to
      the original PCM (input).

    @msgpayload
    For the payload format, see asm_ddp_enc_cfg_t.
    @par
    @values -1 to -31 (Default setting is -31) @newpage
*/
#define ASM_PARAM_ID_DDP_ENC_DIAL_NORM    0x00010DF4

/* Structure for DDP encoder Generic Parameter. */
typedef struct asm_ddpencoder_generic_param_t asm_ddpencoder_generic_param_t;

#include "adsp_begin_pack.h"

/** Payload of the generic DDP encoder parameters in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.
*/
struct asm_ddpencoder_generic_param_t
{
    uint32_t                  param_val;
    /**< Generic parameter used by the following DDP encoder parameter IDs:
         - #ASM_PARAM_ID_DDP_ENC_DATA_RATE
         - #ASM_PARAM_ID_DDP_ENC_LFE
         - #ASM_PARAM_ID_DDP_ENC_PH90_FILT
         - #ASM_PARAM_ID_DDP_ENC_GBL_DRC_PROF
         - #ASM_PARAM_ID_DDP_ENC_DIAL_NORM @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asmstrm_cmd_set_encdec_params */

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _ADSP_ASM_STREAM_COMMANDS_PROP_H_ */
