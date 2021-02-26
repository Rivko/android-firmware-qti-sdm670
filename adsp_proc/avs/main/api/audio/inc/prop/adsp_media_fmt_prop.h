/*========================================================================*/
/**
@file adsp_media_fmt_prop.h

This file contains the structure definitions for the media format
blocks used in client-ASM communications.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      audio_mainpage.dox file. Contact Tech Pubs for assistance.
===========================================================================*/
/*===========================================================================
  Copyright (c) 2016 Qualcomm Technologies, Inc.  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
============================================================================ */
/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/api/audio/inc/prop/adsp_media_fmt_prop.h#1 $

when       who     what, where, why
--------   ---      -------------------------------------------------------
08/20/17   sw      (Tech Pubs) Edited Doxygen comments for AVS 2.8 (SDM845)
08/23/16   sw      (Tech Pubs) Edited Doxygen comments for AVS 2.8 (MSM8998).
08\17\16    HB      Create File for properietary information.
========================================================================== */

#ifndef _ADSP_MEDIA_FMT_PROP_H_
#define _ADSP_MEDIA_FMT_PROP_H_

#include "mmdefs.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/** @addtogroup asmstrm_mediafmt_adpcm
@{ */
/** Media format ID for adaptive differential PCM.

    @par ADPCM format block (asm_adpcm_fmt_blk_t)
    The encoder configuration block is the same as the decoder format block,
    with some differences in the values.
    @tablens{weak__asm__adpcm__fmt__blk__t}
*/
#define ASM_MEDIA_FMT_ADPCM                     0x00010BE7

/* ADPCM decoder format block structure. */
typedef struct asm_adpcm_fmt_blk_t asm_adpcm_fmt_blk_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_adpcm_fmt_blk_t
@{ */
/* Payload of the ADPCM encoder/decoder format block parameters in the
    ASM_MEDIA_FMT_ADPCM media format.
*/
struct asm_adpcm_fmt_blk_t
{
    uint16_t          num_channels;
    /**< Number of PCM channels.

         @values
         - 0 -- Native mode (encoder only)
         - 1 -- Mono (decoder and encoder)
         - 2 -- Stereo (decoder and encoder) @tablebulletend */

    uint16_t          bits_per_sample;
    /**< Number of bits per sample per channel.

         @values
         - 4 -- Decoder
         - 16 -- Encoder @tablebulletend */

    uint32_t          sample_rate;
    /**< Number of samples per second.

         @values{for the decoder}
         - 2000 to 48000 Hz

         @values{for the encoder}
         - 0 -- Native mode
         - 1 -- 2000 to 48000

         Native mode indicates that encoding must be performed with the
         sampling rate at the input. */

    uint32_t          blk_size;
    /**< Block size for the decoder/encoder.

         @tblsubhd{For the decoder}
         - Value that is retrieved from the bit stream by the parser (upper
           layer).
         - The parser must parse it and send it to the aDSP.

         @values Embedded in the bit stream

         @tblsubhd{For the encoder}
         - Size of the block, which is a single unit or a frame of encoded
           samples.
         - Each block is completely independent of the other blocks in the
           stream, and each block contains the information necessary to decode
           a fixed number of PCM samples.

         @values 20 to 4096 bytes */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_adpcm_fmt_blk_t */

/* Encoder configuration block is the same as the ADPCM decoder
    format block. */
typedef asm_adpcm_fmt_blk_t asm_adpcm_enc_cfg_t;

/** @} */ /* end_addtogroup asmstrm_mediafmt_adpcm */

/** @ingroup asmstrm_mediafmt_mp2
    Media format ID for MP2. Following are decoder-specific details:
    - Sampling rates -- 16 to 48 kHz
    - Channels -- 2
    - Bitwidth -- 16/24 bits per sample
    - Bitrates -- 8 to 384 kbps
    - Minimum buffer required per frame -- 1728 bytes
    - External bit stream support -- No

    @par MP2 format block
    No format block is needed. All information is contained in the bit stream.
*/
#define ASM_MEDIA_FMT_MP2                                       0x00010DE9

/** @ingroup asmstrm_mediafmt_mp3
    Media format ID for MP3.

    @par MP3 format block
    No format block is needed. All information is contained in the bit stream.

    @par MP3 encode configuration block (asm_mp3_enc_cfg_t)
    @table{weak__asm__mp3__enc__cfg__t}
*/
#define ASM_MEDIA_FMT_MP3                                       0x00010BE9

/* MP3 encoder configuration structure. */
typedef struct asm_mp3_enc_cfg_t asm_mp3_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_mp3_enc_cfg_t
@{ */
/* Payload of the MP3 encoder configuration parameters in the
    ASM_MEDIA_FMT_MP3 media format.
*/
struct asm_mp3_enc_cfg_t
{
    uint32_t          bit_rate;
    /**< Encoding rate in bits per second.

         @values 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192,
                 224, 256, 320 kbps

         For the best performance, Qualcomm recommends using 64 kbps per
         channel: 64 for mono encoding, and 128 for stereo encoding. */

    uint32_t          channel_cfg;
    /**< Number of channels to encode.

         @note1hang The MP3 encoder supports mono and stereo.

         @values
         - 0 -- Native mode
         - 1 -- Mono
         - 2 -- Stereo
         - Other values are not supported

         Native mode indicates that encoding must be performed with the number
         of channels at the input.

         The number of channels must not change during encoding. */

    uint32_t          sample_rate;
    /**< Number of samples per second.

         @values
         - 16000, 22050, 24000, 32000, 44100, 48000 Hz
         - 0 -- Native mode

         Native mode indicates that encoding must be performed with the
         sampling rate at the input.

         The sampling rate must not change during encoding. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_mp3_enc_cfg_t */

/** @addtogroup asmstrm_mediafmt_aac
@{ */
/** Media format ID for the AAC format block, which supports both MPEG2 and
    MPEG4 AAC formats.

    @note1hang AOT=17 is the ER AAC-LC format, which currently is not
               supported.

    @par AAC format block (asm_aac_fmt_blk_v2_t)
    @tablens{weak__asm__aac__fmt__blk__v2__t}

    @par AAC encode configuration block (asm_aac_enc_cfg_v2_t)
    @table{weak__asm__aac__enc__cfg__v2__t}

    @vertspace{-15}
    @subhead{AAC encoder allowed bitrates}
    @inputtable{Encoder_AAC_allowed_bitrates.tex}

    @subhead{AAC encoder supported sampling rates}
    @inputtable{Encoder_AAC_supported_sampling_rates.tex}
*/
#define ASM_MEDIA_FMT_AAC_V2                                       0x00010DA6

/** Enumeration for the audio data transport stream AAC format. */
#define ASM_MEDIA_FMT_AAC_FORMAT_FLAG_ADTS                      0

/** Enumeration for the low overhead audio stream AAC format. */
#define ASM_MEDIA_FMT_AAC_FORMAT_FLAG_LOAS                      1

/** Enumeration for the audio data interchange format AAC format. */
#define ASM_MEDIA_FMT_AAC_FORMAT_FLAG_ADIF                      2

/** Enumeration for the raw AAC format. */
#define ASM_MEDIA_FMT_AAC_FORMAT_FLAG_RAW                       3

/** Enumeration for the low overhead audio transport multiplex AAC format. */
#define ASM_MEDIA_FMT_AAC_FORMAT_FLAG_LATM                      4

/** Enumeration for the AAC low complexity audio object type:
    - When used for aac_fmt_flag in asm_aac_fmt_blk_v2_t, this parameter
      specifies that the AOT in the AAC stream is LC.
    - When used for enc_mode in asm_aac_enc_cfg_v2_t, this parameter specifies
      that the Encoding mode to be used is LC.
*/
#define ASM_MEDIA_FMT_AAC_AOT_LC                                2

/** Enumeration for the AAC spectral band replication AOT:
    - When used for aac_fmt_flag in asm_aac_fmt_blk_v2_t, this parameter
      specifies that the AOT in the AAC stream is SBR.
    - When used for enc_mode in asm_aac_enc_cfg_v2_t, this parameter specifies
      that the Encoding mode to be used is AAC+.
*/
#define ASM_MEDIA_FMT_AAC_AOT_SBR                               5

/** Enumeration for the AAC parametric stereo AOT:
    - When used for aac_fmt_flag in asm_aac_fmt_blk_v2_t, this parameter
      specifies that the AOT in the AAC stream is PS.
    - When used for enc_mode in asm_aac_enc_cfg_v2_t, this parameter specifies
      that the Encoding mode to be used is eAAC+.
*/
#define ASM_MEDIA_FMT_AAC_AOT_PS                                29

/** Enumeration for the bit-sliced arithmetic coding AOT:
    - When used for aac_fmt_flag in asm_aac_fmt_blk_v2_t, this parameter
      specifies that the AOT in the AAC stream is BSAC.
    - This parameter must not be used for enc_mode in
      asm_aac_enc_cfg_v2_t. BSAC encoding is not supported.
*/
#define ASM_MEDIA_FMT_AAC_AOT_BSAC                              22

/** @} */ /* end_addtogroup asmstrm_mediafmt_aac */

/* AAC decoder format block structure. */
typedef struct asm_aac_fmt_blk_v2_t asm_aac_fmt_blk_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_aac_fmt_blk_v2_t
@{ */
/* Payload of the AAC decoder format block parameters in the
    ASM_MEDIA_FMT_AAC_V2 media format. */
/** If the total size of PCE > 0 bits, the PCE information follows the
    structure. In this case, the client must pad the additional bits for 32-bit
    alignment.
*/
struct asm_aac_fmt_blk_v2_t
{
    uint16_t          aac_fmt_flag;
    /**< Bit stream format option.

         @values
         - #ASM_MEDIA_FMT_AAC_FORMAT_FLAG_ADTS
         - #ASM_MEDIA_FMT_AAC_FORMAT_FLAG_LOAS
         - #ASM_MEDIA_FMT_AAC_FORMAT_FLAG_ADIF
         - #ASM_MEDIA_FMT_AAC_FORMAT_FLAG_RAW
         - #ASM_MEDIA_FMT_AAC_FORMAT_FLAG_LATM @tablebulletend */

    uint16_t          audio_obj_type;
    /**< Audio Object Type (AOT) present in the AAC stream.

         @values
         - #ASM_MEDIA_FMT_AAC_AOT_LC
         - #ASM_MEDIA_FMT_AAC_AOT_SBR
         - #ASM_MEDIA_FMT_AAC_AOT_BSAC
         - #ASM_MEDIA_FMT_AAC_AOT_PS

         Other values are not supported. */

    uint16_t          channel_config;
    /**< Number of channels present in the AAC stream.

         @values
         - 0 -- PCE
         - 1 -- Mono
         - 2 -- Stereo
         - 6 -- 5.1 content @tablebulletend */

    uint16_t          total_size_of_PCE_bits;
    /**< For RAW formats and if channel_config=0 (PCE), the client can send
         the bit stream containing PCE immediately following this structure
         (in band).

         @values @ge 0 (does not include the bits required for 32-bit
                 alignment)

         If this field is set to 0, the PCE information is assumed to be
         available in the audio bit stream and not in band.

         If this field is greater than 0, the PCE information follows this
         structure. Additional bits might be required for 32-bit alignment. */

    uint32_t          sample_rate;
    /**< Number of samples per second.

         @values 8000, 11025, 12000, 16000, 22050, 24000, 32000,
                 44100, 48000, 64000, 88200, 96000 Hz

         This field must be equal to the sample rate of the AAC-LC decoder
         output.
         - For MP4 or 3GP containers, this sample rate is indicated by the
           samplingFrequencyIndex field in the AudioSpecificConfig element.
         - For ADTS format, this sample rate is indicated by the
           samplingFrequencyIndex in the ADTS fixed header.
         - For ADIF format, this sample rate is indicated by the
           samplingFrequencyIndex in the program_config_element present in the
           ADIF header.
           @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_aac_fmt_blk_v2_t */

/* AAC encoder configuration structure. */
typedef struct asm_aac_enc_cfg_v2_t asm_aac_enc_cfg_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_aac_enc_cfg_v2_t
@{ */
/* Payload of the AAC encoder configuration parameters in the
    ASM_MEDIA_FMT_AAC_V2 media format.
*/
struct asm_aac_enc_cfg_v2_t
{
    uint32_t          bit_rate;
    /**< Encoding rate in bits per second. */

    uint32_t          enc_mode;
    /**< Encoding mode.

         @values
         - #ASM_MEDIA_FMT_AAC_AOT_LC
         - #ASM_MEDIA_FMT_AAC_AOT_SBR
         - #ASM_MEDIA_FMT_AAC_AOT_PS @tablebulletend */

    uint16_t          aac_fmt_flag;
    /**< AAC format flag.

         @values
         - #ASM_MEDIA_FMT_AAC_FORMAT_FLAG_ADTS
         - #ASM_MEDIA_FMT_AAC_FORMAT_FLAG_RAW
         - #ASM_MEDIA_FMT_AAC_FORMAT_FLAG_LOAS
         - #ASM_MEDIA_FMT_AAC_FORMAT_FLAG_LATM @tablebulletend */

    uint16_t          channel_cfg;
    /**< Number of channels to encode.

         @note1hang The eAAC+ encoder mode supports only stereo.

         @values
         - 0 -- Native mode
         - 1 -- Mono
         - 2 -- Stereo
         - Other values are not supported

         Native mode indicates that encoding must be performed with the number
         of channels at the input.

         The number of channels must not change during encoding. */

    uint32_t          sample_rate;
    /**< Number of samples per second.

         @values
         - 0 -- Native mode
         - For other values, see
           @xhyperref{tbl:AACencoderSupportedSamplingRates,AAC encoder
           supported sampling rates}

         Native mode indicates that encoding must be performed with the
         sampling rate at the input.

         The sampling rate must not change during encoding. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_aac_enc_cfg_v2_t */

/** @addtogroup asmstrm_mediafmt_amrnb
@{ */
/** Media format ID for adaptive multirate narrowband.

    @par AMR-NB format block
    No format block is needed. All information is contained in the bit stream.

    @par AMR-NB encode configuration block (asm_amrnb_enc_cfg_t)
    @table{weak__asm__amrnb__enc__cfg__t}
*/
#define ASM_MEDIA_FMT_AMRNB_FS                  0x00010BEB

/** Enumeration for 4.75 kbps AMR-NB Encoding mode. */
#define ASM_MEDIA_FMT_AMRNB_FS_ENCODE_MODE_MR475                0

/** Enumeration for 5.15 kbps AMR-NB Encoding mode. */
#define ASM_MEDIA_FMT_AMRNB_FS_ENCODE_MODE_MR515                1

/** Enumeration for 5.90 kbps AMR-NB Encoding mode. */
#define ASM_MEDIA_FMT_AMRNB_FS_ENCODE_MODE_MMR59                2

/** Enumeration for 6.70 kbps AMR-NB Encoding mode. */
#define ASM_MEDIA_FMT_AMRNB_FS_ENCODE_MODE_MMR67                3

/** Enumeration for 7.40 kbps AMR-NB Encoding mode. */
#define ASM_MEDIA_FMT_AMRNB_FS_ENCODE_MODE_MMR74                4

/** Enumeration for 7.95 kbps AMR-NB Encoding mode. */
#define ASM_MEDIA_FMT_AMRNB_FS_ENCODE_MODE_MMR795               5

/** Enumeration for 10.20 kbps AMR-NB Encoding mode. */
#define ASM_MEDIA_FMT_AMRNB_FS_ENCODE_MODE_MMR102               6

/** Enumeration for 12.20 kbps AMR-NB Encoding mode. */
#define ASM_MEDIA_FMT_AMRNB_FS_ENCODE_MODE_MMR122               7

/** Enumeration for disabling AMR-NB DTX mode. */
#define ASM_MEDIA_FMT_AMRNB_FS_DTX_MODE_OFF                     0

/** Enumeration for enabling AMR-NB DTX mode VAD1. */
#define ASM_MEDIA_FMT_AMRNB_FS_DTX_MODE_VAD1                    1

/** Enumeration for enabling AMR-NB DTX mode VAD2. */
#define ASM_MEDIA_FMT_AMRNB_FS_DTX_MODE_VAD2                    2

/** Enumeration for enabling AMR-NB automatic DTX mode. */
#define ASM_MEDIA_FMT_AMRNB_FS_DTX_MODE_AUTO                    3

/** @} */ /* end_addtogroup asmstrm_mediafmt_amrnb */

/* AMR-NB encoder configuration structure. */
typedef struct asm_amrnb_enc_cfg_t asm_amrnb_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_amrnb_enc_cfg_t
@{ */
/* Payload of the AMR-NB encoder configuration parameters in the
    ASM_MEDIA_FMT_AMRNB_FS media format.
*/
struct asm_amrnb_enc_cfg_t
{
    uint16_t          enc_mode;
    /**< AMR-NB encoding rate.

         @values
         - #ASM_MEDIA_FMT_AMRNB_FS_ENCODE_MODE_MR475
         - #ASM_MEDIA_FMT_AMRNB_FS_ENCODE_MODE_MR515
         - #ASM_MEDIA_FMT_AMRNB_FS_ENCODE_MODE_MMR59
         - #ASM_MEDIA_FMT_AMRNB_FS_ENCODE_MODE_MMR67
         - #ASM_MEDIA_FMT_AMRNB_FS_ENCODE_MODE_MMR74
         - #ASM_MEDIA_FMT_AMRNB_FS_ENCODE_MODE_MMR795
         - #ASM_MEDIA_FMT_AMRNB_FS_ENCODE_MODE_MMR102
         - #ASM_MEDIA_FMT_AMRNB_FS_ENCODE_MODE_MMR122 @tbend2 */

    uint16_t          dtx_mode;
    /**< Specifies whether DTX mode is enabled.

         @values
         - 0 -- Disable
         - 1 -- Enable @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_amrnb_enc_cfg_t */


/** @addtogroup asmstrm_mediafmt_amrwb
@{ */
/** Media format ID for adaptive multirate wideband.

    @par AMR-WB format block
    No format block is needed. All information is contained in the bit stream.

    @par AMR-WB encode configuration (asm_amrwb_enc_cfg_t)
    @table{weak__asm__amrwb__enc__cfg__t}
*/
#define ASM_MEDIA_FMT_AMRWB_FS                  0x00010BEC

/** Enumeration for 6.60 kbps AMR-WB Encoding mode. */
#define ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR66                 0

/** Enumeration for 8.85 kbps AMR-WB Encoding mode. */
#define ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR885                1

/** Enumeration for 12.65 kbps AMR-WB Encoding mode. */
#define ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR1265               2

/** Enumeration for 14.25 kbps AMR-WB Encoding mode. */
#define ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR1425               3

/** Enumeration for 15.85 kbps AMR-WB Encoding mode. */
#define ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR1585               4

/** Enumeration for 18.25 kbps AMR-WB Encoding mode. */
#define ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR1825               5

/** Enumeration for 19.85 kbps AMR-WB Encoding mode. */
#define ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR1985               6

/** Enumeration for 23.05 kbps AMR-WB Encoding mode. */
#define ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR2305               7

/** Enumeration for 23.85 kbps AMR-WB Encoding mode. */
#define ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR2385               8

/** @} */ /* end_addtogroup asmstrm_mediafmt_amrwb */

/* AMR-WB encoder configuration structure. */
typedef struct asm_amrwb_enc_cfg_t asm_amrwb_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_amrwb_enc_cfg_t
@{ */
/* Payload of the AMR-WB encoder configuration parameters in the
    ASM_MEDIA_FMT_AMRWB_FS.
*/
struct asm_amrwb_enc_cfg_t
{
    uint16_t          enc_mode;
    /**< AMR-WB encoding rate.

         @values
         - #ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR66
         - #ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR885
         - #ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR1265
         - #ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR1425
         - #ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR1585
         - #ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR1825
         - #ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR1985
         - #ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR2305
         - #ASM_MEDIA_FMT_AMRWB_FS_ENCODE_MODE_MR2385 @tbend2 */

    uint16_t          dtx_mode;
    /**< Specifies whether DTX mode is enabled.

         @values
         - 0 -- Disable
         - 1 -- Enable @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_amrwb_enc_cfg_t */

/** @addtogroup asmstrm_mediafmt_v13k
@{ */
/** Media format ID for V13K Encoding mode.

    @par V13K format block
    No format block is needed. All information is contained in the bit stream.

    @par V13K encode configuration (asm_v13k_enc_cfg_t)
    @table{weak__asm__v13k__enc__cfg__t}
*/
#define ASM_MEDIA_FMT_V13K_FS                      0x00010BED

/** Enumeration for 14.4 kbps V13K Encoding mode. */
#define ASM_MEDIA_FMT_V13K_FS_ENCODE_MODE_MR1440                0

/** Enumeration for 12.2 kbps V13K Encoding mode. */
#define ASM_MEDIA_FMT_V13K_FS_ENCODE_MODE_MR1220                1

/** Enumeration for 11.2 kbps V13K Encoding mode. */
#define ASM_MEDIA_FMT_V13K_FS_ENCODE_MODE_MR1120                2

/** Enumeration for 9.0 kbps V13K Encoding mode. */
#define ASM_MEDIA_FMT_V13K_FS_ENCODE_MODE_MR90                  3

/** Enumeration for 7.2 kbps V13K Encoding mode. */
#define ASM_MEDIA_FMT_V13K_FS_ENCODE_MODE_MR720                 4

/** Enumeration for 1/8 vocoder rate.*/
#define ASM_MEDIA_FMT_VOC_ONE_EIGHTH_RATE          1

/** Enumeration for 1/4 vocoder rate. */
#define ASM_MEDIA_FMT_VOC_ONE_FOURTH_RATE       2

/** Enumeration for 1/2 vocoder rate. */
#define ASM_MEDIA_FMT_VOC_HALF_RATE             3

/** Enumeration for full vocoder rate. */
#define ASM_MEDIA_FMT_VOC_FULL_RATE             4

/** @} */ /* end_addtogroup asmstrm_mediafmt_v13k */

/* V13K encoder configuration structure. */
typedef struct asm_v13k_enc_cfg_t asm_v13k_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_v13k_enc_cfg_t
@{ */
/* Payload of the V13K encoder configuration parameters in the
    ASM_MEDIA_FMT_V13K_FS media format.
*/
struct asm_v13k_enc_cfg_t
{
    uint16_t          max_rate;
    /**< Maximum allowed encoder frame rate.

         @values
         - #ASM_MEDIA_FMT_VOC_ONE_EIGHTH_RATE
         - #ASM_MEDIA_FMT_VOC_ONE_FOURTH_RATE
         - #ASM_MEDIA_FMT_VOC_HALF_RATE
         - #ASM_MEDIA_FMT_VOC_FULL_RATE @tablebulletend */

    uint16_t          min_rate;
    /**< Minimum allowed encoder frame rate.

         @values
         - #ASM_MEDIA_FMT_VOC_ONE_EIGHTH_RATE
         - #ASM_MEDIA_FMT_VOC_ONE_FOURTH_RATE
         - #ASM_MEDIA_FMT_VOC_HALF_RATE
         - #ASM_MEDIA_FMT_VOC_FULL_RATE @tablebulletend */

    uint16_t          reduced_rate_cmd;
    /**< Reduced rate command, used to change the average bitrate of the V13K
         vocoder.

         @values
         - #ASM_MEDIA_FMT_V13K_FS_ENCODE_MODE_MR1440 (Default)
         - #ASM_MEDIA_FMT_V13K_FS_ENCODE_MODE_MR1220
         - #ASM_MEDIA_FMT_V13K_FS_ENCODE_MODE_MR1120
         - #ASM_MEDIA_FMT_V13K_FS_ENCODE_MODE_MR90
         - #ASM_MEDIA_FMT_V13K_FS_ENCODE_MODE_MR720 @tablebulletend */

    uint16_t          rate_mod_cmd;
    /**< Rate modulation command.

         @values 0 (Default)
         - If bit 0=1, rate control is enabled.
         - If bit 1=1, the maximum number of consecutive full rate frames is
           limited with numbers supplied in bits 2 to 10.
         - If bit 1=0, the minimum number of non-full rate frames in between two
           full rate frames is forced to the number supplied in bits 2 to 10.
         - In both cases of bit 1, if necessary, half rate is used to substitute
           full rate.
         - Bits 15 to 10 are reserved and must all be set to 0.
         @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_v13k_enc_cfg_t */

/** @ingroup asmstrm_mediafmt_evrc
    Media format ID for the enhanced variable rate codec.

    @par EVRC format block
    No format block is needed. All information is contained in the bit stream.

    @par EVRC encode configuration (asm_evrc_enc_cfg_t)
    @table{weak__asm__evrc__enc__cfg__t}
*/
#define ASM_MEDIA_FMT_EVRC_FS                   0x00010BEE

/* EVRC encoder configuration structure. */
typedef struct asm_evrc_enc_cfg_t asm_evrc_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_evrc_enc_cfg_t
@{ */
/* Payload of the EVRC encoder configuration parameters in the
    ASM_MEDIA_FMT_EVRC_FS media format.
*/
struct asm_evrc_enc_cfg_t
{
    uint16_t          max_rate;
    /**< Maximum allowed encoder frame rate.

         @values
         - #ASM_MEDIA_FMT_VOC_ONE_EIGHTH_RATE
         - #ASM_MEDIA_FMT_VOC_ONE_FOURTH_RATE
         - #ASM_MEDIA_FMT_VOC_HALF_RATE
         - #ASM_MEDIA_FMT_VOC_FULL_RATE @tablebulletend */

    uint16_t          min_rate;
    /**< Minimum allowed encoder frame rate.

         @values
         - #ASM_MEDIA_FMT_VOC_ONE_EIGHTH_RATE
         - #ASM_MEDIA_FMT_VOC_ONE_FOURTH_RATE
         - #ASM_MEDIA_FMT_VOC_HALF_RATE
         - #ASM_MEDIA_FMT_VOC_FULL_RATE @tablebulletend */

    uint16_t          rate_mod_cmd;
    /**< Rate modulation command.

         @values 0 (Default)
         - If bit 0=1, rate control is enabled.
         - If bit 1=1, the maximum number of consecutive full rate frames is
           limited with numbers supplied in bits 2 to 10.
         - If bit 1=0, the minimum number of non-full rate frames in between
           two full rate frames is forced to the number supplied in bits 2 to
           10.
         - In both cases of bit 1, if necessary, half rate is used to
           substitute full rate.
         - Bits 15 to 10 are reserved and must all be set to 0.
         @tablebulletend */

    uint16_t          reserved;
    /**< Used for alignment; must be set to 0. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_evrc_enc_cfg_t */


/** @addtogroup asmstrm_mediafmt_evcrb
@{ */
/** Media format ID for enhanced variable rate codec B.

    @par EVRC-B format block
    No format block is needed. All information is contained in the bit stream.

    @par EVRC-B encode configuration (asm_evrcb_enc_cfg_t)
    @table{weak__asm__evrcb__enc__cfg__t}
*/
#define ASM_MEDIA_FMT_EVRCB_FS                  0x00010BEF

/** Enumeration for 9.3 kbps EVRC-B Encoding mode. */
#define ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR930        0

/** Enumeration for 8.5 kbps EVRC-B Encoding mode. */
#define ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR850        1

/** Enumeration for 7.5 kbps EVRC-B Encoding mode. */
#define ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR750        2

/** Enumeration for 7.0 kbps EVRC-B Encoding mode. */
#define ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR700        3

/** Enumeration for 6.6 kbps EVRC-B Encoding mode. */
#define ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR660        4

/** Enumeration for 6.2 kbps EVRC-B Encoding mode. */
#define ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR620        5

/** Enumeration for 5.8 kbps EVRC-B Encoding mode. */
#define ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR580        6

/** Enumeration for 4.8 kbps EVRC-B Encoding mode. */
#define ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR480        7

/** @} */ /* end_addtogroup asmstrm_mediafmt_evcrb */

/* EVCR-B encoder configuration structure. */
typedef struct asm_evrcb_enc_cfg_t asm_evrcb_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_evrcb_enc_cfg_t
@{ */
/* Payload of the EVCR-B encoder configuration parameters in the
    ASM_MEDIA_FMT_EVRCB_FS media format.
*/
struct asm_evrcb_enc_cfg_t
{
    uint16_t          max_rate;
    /**< Maximum allowed encoder frame rate.

         @values
         - #ASM_MEDIA_FMT_VOC_ONE_EIGHTH_RATE
         - #ASM_MEDIA_FMT_VOC_ONE_FOURTH_RATE
         - #ASM_MEDIA_FMT_VOC_HALF_RATE
         - #ASM_MEDIA_FMT_VOC_FULL_RATE @tablebulletend */

    uint16_t          min_rate;
    /**< Minimum allowed encoder frame rate.

         @values
         - #ASM_MEDIA_FMT_VOC_ONE_EIGHTH_RATE
         - #ASM_MEDIA_FMT_VOC_ONE_FOURTH_RATE
         - #ASM_MEDIA_FMT_VOC_HALF_RATE
         - #ASM_MEDIA_FMT_VOC_FULL_RATE @tablebulletend */

    uint16_t          dtx_enable;
    /**< Specifies whether DTX mode is enabled.

         @values
         - 0 -- Disable
         - 1 -- Enable @tablebulletend */

    uint16_t          reduced_rate_level;
    /**< Reduced rate level for the average encoding rate.

         @values
         - #ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR930 (Default)
         - #ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR850
         - #ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR750
         - #ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR700
         - #ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR660
         - #ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR620
         - #ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR580
         - #ASM_MEDIA_FMT_EVRCB_FS_ENCODE_MODE_MR480 @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_evrcb_enc_cfg_t */


/** @addtogroup asmstrm_mediafmt_evrcwb
@{ */
/** Media format ID for enhanced variable rate codec wideband.

    @par EVRC-WB format block
    No format block is needed. All information is contained in the bit stream.

    @par EVRC-WB encode configuration (asm_evrcwb_enc_cfg_t)
    @table{weak__asm__evrcwb__enc__cfg__t} @newpage
*/
#define ASM_MEDIA_FMT_EVRCWB_FS                 0x00010BF0

/** Enumeration for 8.5 kbps EVRC-WB Encoding mode.  */
#define ASM_MEDIA_FMT_EVRCWB_FS_ENCODE_MODE_MR850        0

/** Enumeration for 9.3 kbps EVRC-WB Encoding mode. */
#define ASM_MEDIA_FMT_EVRCWB_FS_ENCODE_MODE_MR930        4

/** Enumeration for 4.8 kbps EVRC-WB Encoding mode. */
#define ASM_MEDIA_FMT_EVRCWB_FS_ENCODE_MODE_MR480        7

/** @} */ /* end_addtogroup asmstrm_mediafmt_evrcwb */

/* EVRC-WB encoder configuration structure. */
typedef struct asm_evrcwb_enc_cfg_t asm_evrcwb_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_evrcwb_enc_cfg_t
@{ */
/* Payload of the EVRC-WB encoder configuration parameters in the
    #ASM_MEDIA_FMT_EVRCWB_FS media format.
*/
struct asm_evrcwb_enc_cfg_t
{
    uint16_t          max_rate;
    /**< Maximum allowed encoder frame rate.

         @values
         - #ASM_MEDIA_FMT_VOC_ONE_EIGHTH_RATE
         - #ASM_MEDIA_FMT_VOC_ONE_FOURTH_RATE
         - #ASM_MEDIA_FMT_VOC_HALF_RATE
         - #ASM_MEDIA_FMT_VOC_FULL_RATE @tablebulletend */

    uint16_t          min_rate;
    /**< Minimum allowed encoder frame rate.

         @values
         - #ASM_MEDIA_FMT_VOC_ONE_EIGHTH_RATE
         - #ASM_MEDIA_FMT_VOC_ONE_FOURTH_RATE
         - #ASM_MEDIA_FMT_VOC_HALF_RATE
         - #ASM_MEDIA_FMT_VOC_FULL_RATE @tablebulletend */

    uint16_t          dtx_enable;
    /**< Specifies whether DTX mode is enabled.

         @values
         - 0 -- Disable
         - 1 -- Enable @tablebulletend */

    uint16_t          reduced_rate_level;
    /**< Reduced rate level for the average encoding rate.

         @values
         - #ASM_MEDIA_FMT_EVRCWB_FS_ENCODE_MODE_MR850 (Default)
         - #ASM_MEDIA_FMT_EVRCWB_FS_ENCODE_MODE_MR930
         - #ASM_MEDIA_FMT_EVRCWB_FS_ENCODE_MODE_MR480 @tbend2 */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_evrcwb_enc_cfg_t */

/** @addtogroup asmstrm_mediafmt_midi
@{ */
/** Media format ID for MIDI.

    @par MIDI format block (asm_midi_fmt_blk_t)
    @table{weak__asm__midi__fmt__blk__t}
*/
#define ASM_MEDIA_FMT_MIDI                      0x00010BF1

/** Enumeration for MIDI mode GM. */
#define ASM_MIDI_MODE_GM                        0

/** Enumeration for MIDI mode MA3. */
#define ASM_MIDI_MODE_MA3                       1

/** Enumeration for MIDI mode MA5. */
#define ASM_MIDI_MODE_MA5                       2

/** @} */ /* end_addtogroup asmstrm_mediafmt_midi */

/* MIDI format block structure. */
typedef struct asm_midi_fmt_blk_t asm_midi_fmt_blk_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_midi_fmt_blk_t
@{ */
/* Payload of the MIDI format block parameters in the
    #ASM_MEDIA_FMT_MIDI media format.
*/
struct asm_midi_fmt_blk_t
{
    uint32_t          mode;
    /**< Operating mode that indicates which commercial MIDI device is to be
         emulated.

         @values
         - #ASM_MIDI_MODE_GM
         - #ASM_MIDI_MODE_MA3
         - #ASM_MIDI_MODE_MA5 @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_midi_fmt_blk_t */

/** @addtogroup asmstrm_mediafmt_sbc
@{ */
/** Media format ID for SBC encode configuration.

    @par SBC encode configuration (asm_sbc_enc_cfg_t)
    @table{weak__asm__sbc__enc__cfg__t}
*/
#define ASM_MEDIA_FMT_SBC                       0x00010BF2

/** Enumeration for SBC channel Mono mode. */
#define ASM_MEDIA_FMT_SBC_CHANNEL_MODE_MONO                     1

/** Enumeration for SBC channel Stereo mode. */
#define ASM_MEDIA_FMT_SBC_CHANNEL_MODE_STEREO                   2

/** Enumeration for SBC channel Dual Mono mode. */
#define ASM_MEDIA_FMT_SBC_CHANNEL_MODE_DUAL_MONO                8

/** Enumeration for SBC channel Joint Stereo mode. */
#define ASM_MEDIA_FMT_SBC_CHANNEL_MODE_JOINT_STEREO             9

/** Enumeration for SBC bit allocation method = loudness. */
#define ASM_MEDIA_FMT_SBC_ALLOCATION_METHOD_LOUDNESS            0

/** Enumeration for SBC bit allocation method = SNR.  */
#define ASM_MEDIA_FMT_SBC_ALLOCATION_METHOD_SNR                 1

/** @} */ /* end_addtogroup asmstrm_mediafmt_sbc */

/* SBC encoder configuration structure. */
typedef struct asm_sbc_enc_cfg_t asm_sbc_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_sbc_enc_cfg_t
@{ */
/* Payload of the SBC encoder configuration parameters in the
    #ASM_MEDIA_FMT_SBC media format.
*/
struct asm_sbc_enc_cfg_t
{
    uint32_t          num_subbands;
    /**< Number of subbands.

         @values 4, 8 */

    uint32_t          blk_len;
    /**< Size of the encoded block in samples.

         @values 4, 8, 12, 16 */

    uint32_t          channel_mode;
    /**< Mode used to allocate bits between channels.

         @values
         - 0 (Native mode)
         - #ASM_MEDIA_FMT_SBC_CHANNEL_MODE_MONO
         - #ASM_MEDIA_FMT_SBC_CHANNEL_MODE_STEREO
         - #ASM_MEDIA_FMT_SBC_CHANNEL_MODE_DUAL_MONO
         - #ASM_MEDIA_FMT_SBC_CHANNEL_MODE_JOINT_STEREO

         Native mode indicates that encoding must be performed with the number
         of channels at the input.

         If postprocessing outputs one-channel data, Mono mode is used. If
         postprocessing outputs two-channel data, Stereo mode is used.

         The number of channels must not change during encoding. */

    uint32_t          alloc_method;
    /**< Encoder bit allocation method.

         @values
         - #ASM_MEDIA_FMT_SBC_ALLOCATION_METHOD_LOUDNESS
         - #ASM_MEDIA_FMT_SBC_ALLOCATION_METHOD_SNR @tablebulletend */

    uint32_t          bit_rate;
    /**< Number of encoded bits per second.

         @values
         - Mono channel -- Maximum of 320 kbps
         - Stereo channel -- Maximum of 512 kbps @tablebulletend */

    uint32_t          sample_rate;
    /**< Number of samples per second.

         @values 0 (Native mode), 16000, 32000, 44100, 48000&nbsp;Hz

         Native mode indicates that encoding must be performed with the
         sampling rate at the input.

         The sampling rate must not change during encoding. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_sbc_enc_cfg_t */

/** @ingroup asmstrm_mediafmt_celt
    Media format ID for CELT encode configuration.

    @par CELT encode configuration (asm_celt_enc_cfg_t)
    @table{weak__asm__celt__enc__cfg__t}

    @par Bytes per packet calculation
    The formula is: bytes_per_packet = (bitrate_in_bps * frame_size) /
    (8 * sample_rate).
    @par
    If vbr_flag = 1 (currently, not supported), the bytes per packet calculated
    is used as the upper threshold for encoding. The encoder output size per
    frame can be smaller than the specified bytes per packet.
    @par
    If vbr_flag = 0, the encoder output size per packet is the same as the
    bytes per frame. @newpage
    @par
    The calculated bytes_per_packet is truncated to the nearest lowest integer,
    so the resulting bit rate is either equal to or lesser than the specified
    bitrate. If the bit rate of the encoded stream is to be the same as the
    specified bit rate, ensure that bytes_per_packet calculated using this
    formula is an integer.
    @par
    The CELT encoder supports a minimum of 8 bytes per frame and a maximum of
    1024 bytes per frame. Thus, the minimum and maximum bit rates can be
    calculated:
     - The minimum bit rate of 32000 bps is calculated for a 32 kHz sampling
       frequency (where the frame size is 64 and bytes per frame is 8).
     - The maximum bit rate of 1536000 bps is calculated for 96 kHz (where the
     frame size is 512 and bytes per frame is 1024).
*/
#define ASM_MEDIA_FMT_CELT                        0x00013221

/* CELT encoder configuration structure. */
typedef struct asm_celt_enc_cfg_t asm_celt_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_celt_enc_cfg_t
@{ */
/* Payload of the celt encoder configuration parameters in the
    #ASM_MEDIA_FMT_CELT media format.
*/
struct asm_celt_enc_cfg_t
{
    uint32_t          bit_rate;
    /**< Bit rate used for encoding.

         This parameter is used to calculate the encoder output bytes per frame
         (bytes per packet). See <b>Bytes per packet calculation</b> following
         this table.

         @values 32000 to 1536000 bits per second */

    uint16_t          frame_size;
    /**< Frame size used for encoding.

         @values 64, 128, 256, 512 */

    uint16_t          complexity;
    /**< Complexity of the algorithm.

         Currently, this parameter is used to enable or disable the pitch
         analysis. In the future, it can be scaled to add more levels of
         complexity.

         @values 0 to 10
         - @le 2 -- Disable pitch analysis
         - > 2 -- Enable pitch analysis @tablebulletend */

    uint16_t          prediction_mode;
    /**< Switch variable for the prediction feature.

         This parameter is used to choose between the level of interframe
         predictions allowed while encoding.

         @values
         - 0 -- Independent frames
         - 1 -- Short-term interframe prediction allowed
         - 2 -- Long-term prediction allowed @tablebulletend */

    uint16_t          vbr_flag;
    /**< Variable bit rate flag.

         Currently, this parameter is disabled. In the future, it can be
         enabled if use cases require it.

         @values 0 -- Disabled */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_celt_enc_cfg_t */


/** @addtogroup asmstrm_mediafmt_ldac
@{ */
/** Media format ID for LDAC encode configuration.

    @par LDAC encode configuration (asm_ldac_enc_cfg_t)
    @table{weak__asm__ldac__enc__cfg__t}
*/
#define ASM_MEDIA_FMT_LDAC                            0x00013224

/** Enumeration for LDAC channel Native mode. */
#define ASM_MEDIA_FMT_LDACBT_CHANNEL_MODE_NATIVE               0

/** Enumeration for LDAC channel Mono mode. */
#define ASM_MEDIA_FMT_LDACBT_CHANNEL_MODE_MONO                  4

/** Enumeration for LDAC channel Dual Channel mode. */
#define ASM_MEDIA_FMT_LDACBT_CHANNEL_MODE_DUAL_CHANNEL          2

/** Enumeration for LDAC channel Stereo mode. */
#define ASM_MEDIA_FMT_LDACBT_CHANNEL_MODE_STEREO                1

/** @} */ /* end_addtogroup asmstrm_mediafmt_ldac */


/* LDAC encoder configuration structure. */
typedef struct asm_ldac_enc_cfg_t asm_ldac_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_ldac_enc_cfg_t
@{ */
/* Payload of the LDAC encoder configuration parameters in the
    #ASM_MEDIA_FMT_LDAC media format.
*/
struct asm_ldac_enc_cfg_t
{
    uint32_t          bit_rate;
    /**< Encoding rate in bits per second.

         This field indicates the number of bits per second transmitted by the
         encoder, and it also configures the Encode Quality Mode Index (EQMID).

         @values for fractional sampling rates (44.1 kHz, 88.2 kHz)
         @vertspace{-2}
         - 303000 -- Mobile-use quality
         - 606000 -- Standard quality
         - 909000 -- High quality (in bits per second) for 44.1 and 88.2 kHz
           sampling frequencies

         @values for integer sampling rates (48 kHz, 96 kHz) @vertspace{-2}
         - 330000 -- Mobile-use quality
         - 660000 -- Standard quality
         - 990000 -- High quality (in bits per second) for 48 and 96 kHz
           sampling frequencies @tablebulletend */

    uint16_t          channel_mode;
    /**< Mode used to allocate bits between channels.

         @values
         - #ASM_MEDIA_FMT_LDACBT_CHANNEL_MODE_NATIVE
         - #ASM_MEDIA_FMT_LDACBT_CHANNEL_MODE_MONO
         - #ASM_MEDIA_FMT_LDACBT_CHANNEL_MODE_DUAL_CHANNEL
         - #ASM_MEDIA_FMT_LDACBT_CHANNEL_MODE_STEREO

         Native mode indicates that encoding must be performed with the number
         of channels at the input.

         If postprocessing outputs one-channel data, Mono mode is used. If
         postprocessing outputs two-channel data, Stereo mode is used. The
         number of channels must not change during encoding. */

    uint16_t          mtu;
    /**< Maximum transmission unit.

         @values 679 bytes -- LDACBT_MTU_2DH5

         LDAC is optimized with two DH5 packets.
         One DH5 packet size is 330 bytes of raw encoded data. Two packets
         contain 660 bytes of raw encoded data plus 18 bytes of header.
         An L2CAP implementation for LDAC supports a minimum of 679 bytes. */

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_ldac_enc_cfg_t */


/** @ingroup asmstrm_mediafmt_wmapro10
    Media format ID for the WMA v10 Professional format block.

    @par WMA v10 format block (asm_wmaprov10_fmt_blk_t)
    @table{weak__asm__wmaprov10__fmt__blk__t}
*/
#define ASM_MEDIA_FMT_WMA_V10PRO_V2                0x00010DA7

/* WMA v10 Professional decoder format block structure. */
typedef struct asm_wmaprov10_fmt_blk_v2_t asm_wmaprov10_fmt_blk_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_wmaprov10_fmt_blk_t
@{ */
/* Payload of the WMA Professional v10 decoder format block parameters in the
    ASM_MEDIA_FMT_WMA_V10PRO_V2 media format.
*/
struct asm_wmaprov10_fmt_blk_v2_t
{
    uint16_t          fmt_tag;
    /**< WMA format type.

         @values
         - 0x162 -- WMA 9 Pro
         - 0x163 -- WMA 9 Pro Lossless
         - 0x166 -- WMA 10 Pro
         - 0x167 -- WMA 10 Pro Lossless @tablebulletend */

    uint16_t          num_channels;
    /**< Number of channels encoded in the input stream.

         @values 1 to 8 */

    uint32_t          sample_rate;
    /**< Number of samples per second.

         @values 11025, 16000, 22050, 32000, 44100, 48000, 88200, 96000 Hz */

    uint32_t          avg_bytes_per_sec;
    /**< Bitrate expressed as the average bytes per second.

         @values 2000 to 96000 */

    uint16_t          blk_align;
    /**< Size of the bit stream packet size. WMA Pro files have a payload of one
         block per bit stream packet.

         @values @le 13376 bytes */

    uint16_t          bits_per_sample;
    /**< Number of bits per sample in the encoded WMA stream.

         @values 16, 24 */

    uint32_t          channel_mask;
    /**< Bit-packed double word (32-bits) that indicates the recommended
         speaker positions for each source channel.

         @values See the WMAPRO_CHANNEL_MASK_* macros for popular speaker
                 positions (starting with #WMAPRO_CHANNEL_MASK_RAW) */

    uint16_t          enc_options;
    /**< Bit-packed word with values that indicate whether certain features of
         the bit stream are used.

         @values
         - 0x0001 -- ENCOPT3_PURE_LOSSLESS
         - 0x0006 -- ENCOPT3_FRM_SIZE_MOD
         - 0x0038 -- ENCOPT3_SUBFRM_DIV
         - 0x0040 -- ENCOPT3_WRITE_FRAMESIZE_IN_HDR
         - 0x0080 -- ENCOPT3_GENERATE_DRC_PARAMS
         - 0x0100 -- ENCOPT3_RTMBITS @tablebulletend */

    uint16_t          usAdvancedEncodeOpt;
    /**< Advanced encoding option. This field is ignored. */

    uint32_t          advanced_enc_options2;
    /**< Advanced encoding option 2. This field is ignored. */

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_wmaprov10_fmt_blk_t */


/** @ingroup asmstrm_mediafmt_wma9
    Media format ID for the WMA v9 format block.

    @par WMA v9 format block (asm_wmastdv9_fmt_blk_v2_t)
    @table{weak__asm__wmastdv9__fmt__blk__v2__t}
*/
#define ASM_MEDIA_FMT_WMA_V9_V2                    0x00010DA8

/* WMA v9 decoder format block structure. */
typedef struct asm_wmastdv9_fmt_blk_v2_t asm_wmastdv9_fmt_blk_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_wmastdv9_fmt_blk_v2_t
@{ */
/* Payload of the WMA v9 decoder format block parameters in the
    ASM_MEDIA_FMT_WMA_V9_V2 media format.
*/
struct asm_wmastdv9_fmt_blk_v2_t
{
    uint16_t          fmt_tag;
    /**< WMA format tag.

         @values 0x161 (WMA v9 standard) */

    uint16_t          num_channels;
    /**< Number of channels in the stream.

         @values 1, 2 */

    uint32_t          sample_rate;
    /**< Number of samples per second.

         @values 48000 Hz */

    uint32_t          avg_bytes_per_sec;
    /**< Bitrate expressed as the average bytes per second. */

    uint16_t          blk_align;
    /**< Block align. All WMA files with a maximum packet size of 13376
         are supported. */

    uint16_t          bits_per_sample;
    /**< Number of bits per sample in the output.

         @values 16 */

    uint32_t          channel_mask;
    /**< Channel mask.

         @values
         - 3 -- Stereo (front left/front right)
         - 4 -- Mono (center) @tablebulletend */

    uint16_t          enc_options;
    /**< Options used during encoding. */

    uint16_t          reserved;
    /**< Used for alignment; must be set to 0. */

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_wmastdv9_fmt_blk_v2_t */


/** @ingroup asmstrm_mediafmt_wma8
    Media format ID for the Windows Media Audio (WMA) v8
    format block.

    @par WMA v8 format block (asm_wmastdv8_enc_cfg_t)
    @table{weak__asm__wmastdv8__enc__cfg__t}
*/
#define ASM_MEDIA_FMT_WMA_V8                    0x00010D91

/* WMA v8 encoder configuration structure. */
typedef struct asm_wmastdv8_enc_cfg_t asm_wmastdv8_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_wmastdv8_enc_cfg_t
@{ */
/* Payload of the WMA v8 encoder configuration parameters in the
    ASM_MEDIA_FMT_WMA_V8 media format.
*/
struct asm_wmastdv8_enc_cfg_t
{
    uint32_t          bit_rate;
    /**< Encoding rate in bits per second. */

    uint32_t          sample_rate;
    /**< Number of samples per second.

         @values 0 (Native mode), 22050, 32000, 44100, 48000&nbsp;Hz

         Native mode indicates that encoding must be performed with the
         sampling rate at the input.

         The sampling rate must not change during encoding. */

    uint16_t          channel_cfg;
    /**< Number of channels to encode.

         @values
         - 0 -- Native mode
         - 1 -- Mono
         - 2 -- Stereo
         - Other values are not supported

         Native mode indicates that encoding must be performed with the number
         of channels at the input.

         The number of channels must not change during encoding. */

    uint16_t          reserved;
    /**< Used for alignment; must be set to 0. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_wmastdv8_enc_cfg_t */

/** @ingroup asmstrm_mediafmt_amrwbplus
    Media format ID for the AMR-WB Plus format block.

    @par AMR-WB Plus format block (asm_amrwbplus_fmt_blk_v2_t)
    @table{weak__asm__amrwbplus__fmt__blk__v2__t}
*/
#define ASM_MEDIA_FMT_AMR_WB_PLUS_V2               0x00010DA9

/* AMR-WB Plus decoder format block structure. */
typedef struct asm_amrwbplus_fmt_blk_v2_t asm_amrwbplus_fmt_blk_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_amrwbplus_fmt_blk_v2_t
@{ */
/* Payload of the AMR-WB Plus decoder format block parameters in the
    ASM_MEDIA_FMT_AMR_WB_PLUS_V2 media format.
*/
struct asm_amrwbplus_fmt_blk_v2_t
{
    uint32_t          amr_frame_fmt;
    /**< AMR frame format.

         @values
         - 6 -- Transport Interface Format (TIF); contains a 2-byte header for
           each frame within the superframe
         - Any other value -- File storage format; contains one header per
           superframe @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_amrwbplus_fmt_blk_v2_t */

/** @ingroup asmstrm_mediafmt_g711_alaw
    Media format ID for the G711 A-law algorithm.

    @par G711 A-law format block (asm_g711_alaw_fmt_blk_t)
    @table{weak__asm__g711__alaw__fmt__blk__t}

    @par G711 A-law encode configuration (asm_g711_alaw_enc_cfg_t)
    @table{weak__asm__g711__alaw__enc__cfg__t}
*/
#define ASM_MEDIA_FMT_G711_ALAW_FS                   0x00010BF7

/* G711 A-law encoder configuration structure. */
typedef struct asm_g711_alaw_enc_cfg_t asm_g711_alaw_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_g711_alaw_enc_cfg_t
@{ */
/* Payload of the G711 A-law encoder configuration parameters in the
    ASM_MEDIA_FMT_G711_ALAW_FS media format.
*/
struct asm_g711_alaw_enc_cfg_t
{
    uint32_t            sample_rate;
    /**< Number of samples per second.

         @values 8000, 16000 Hz */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_g711_alaw_enc_cfg_t */

/* G711 A-law decoder format block structure. */
typedef struct asm_g711_alaw_fmt_blk_t asm_g711_alaw_fmt_blk_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_g711_alaw_fmt_blk_t
@{ */
/* Payload of the G711 A-law decoder format block parameters in the
    ASM_MEDIA_FMT_G711_ALAW_FS command.
 */
struct asm_g711_alaw_fmt_blk_t
{
    uint32_t            sample_rate;
    /**< Number of samples per second.

         @values 8000, 16000 Hz */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_g711_alaw_fmt_blk_t */

/** @ingroup asmstrm_mediafmt_g711_mlaw
    Media format ID for the G711 M-law algorithm.

    @par G711 M-law format block (asm_g711_mlaw_fmt_blk_t)
    @table{weak__asm__g711__mlaw__fmt__blk__t}

    @par G711 M-law encode configuration (asm_g711_mlaw_enc_cfg_t)
    @table{weak__asm__g711__mlaw__enc__cfg__t} @newpage
*/
#define ASM_MEDIA_FMT_G711_MLAW_FS                   0x00010C2E

/* G711 m-law encoder configuration structure. */
typedef struct asm_g711_mlaw_enc_cfg_t asm_g711_mlaw_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_g711_mlaw_enc_cfg_t
@{ */
/* Payload of the G711 m-law encoder configuration parameters in the
    ASM_MEDIA_FMT_G711_MLAW_FS media format.
*/
struct asm_g711_mlaw_enc_cfg_t
{
    uint32_t            sample_rate;
    /**< Number of samples per second.

         @values 8000, 16000 Hz */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_g711_mlaw_enc_cfg_t */

/* G711 m-law decoder format block structure. */
typedef struct asm_g711_mlaw_fmt_blk_t asm_g711_mlaw_fmt_blk_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_g711_mlaw_fmt_blk_t
@{ */
/* Payload of the G711 m-law decoder format block parameters in the
    ASM_MEDIA_FMT_G711_MLAW_FS media format.
*/
struct asm_g711_mlaw_fmt_blk_t
{
    uint32_t            sample_rate;
    /**< Number of samples per second.

         @values 8000, 16000 Hz */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_g711_mlaw_fmt_blk_t */

/** @ingroup asmstrm_mediafmt_g729a
    Media format ID for G729A.

    @par G729A format block
    No format block is required.

    @par G729A encode configuration block
    No encoder configuration block is required.
    Currently, this format is not used.
*/
#define ASM_MEDIA_FMT_G729A_FS                 0x00010C2D

/** @ingroup asmstrm_mediafmt_dtmf
    Media format ID for DTMF.
*/
#define ASM_MEDIA_FMT_DTMF                     0x00010C2F

/** @ingroup asmstrm_mediafmt_gsmfr
    Media format ID for GSM-FR.
*/
#define ASM_MEDIA_FMT_FR_FS                     0x00010D6B

/** @ingroup asmstrm_mediafmt_vorbis
    Media format ID for Vorbis.

    @par Vorbis format block (asm_vorbis_fmt_blk_t)
    @table{weak__asm__vorbis__fmt__blk__t}
*/
#define ASM_MEDIA_FMT_VORBIS                    0x00010C15

/* Vorbis decoder format block structure. */
typedef struct asm_vorbis_fmt_blk_t asm_vorbis_fmt_blk_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_vorbis_fmt_blk_t
@{ */
/* Payload of the Vorbis decoder format block parameters in the
    ASM_MEDIA_FMT_VORBIS media format.
*/
struct asm_vorbis_fmt_blk_t
{
    uint32_t          bit_stream_fmt;
    /**< Transcoded bit stream containing the size of the frame as the first
         word in each frame.

         @values
         - 0 -- Raw bit stream
         - 1 -- Transcoded bit stream

         Currently, the Vorbis decoder library supports only a transcoded
         bit stream. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_vorbis_fmt_blk_t */

/** @ingroup asmstrm_mediafmt_flac
    Media format ID for Free Lossless Audio Codec (FLAC).

    @par FLAC format block (asm_flac_fmt_blk_t)
    @table{weak__asm__flac__fmt__blk__t}
*/
#define ASM_MEDIA_FMT_FLAC                      0x00010C16

/* FLAC decoder format block structure. */
typedef struct asm_flac_fmt_blk_t asm_flac_fmt_blk_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_flac_fmt_blk_t
@{ */
/* Payload of the FLAC decoder format block parameters in the
    ASM_MEDIA_FMT_FLAC media format.
*/
struct asm_flac_fmt_blk_t
{
    uint16_t          is_stream_info_present;
    /**< Specifies whether stream information is present in the FLAC format
         block.

         @values
         - 0 -- Stream information is not present in this message
         - 1 -- Stream information is present in this message

         When set to 1, the FLAC bit stream was successfully parsed by the
         client, and other fields in the FLAC format block can be read by the
         decoder to get metadata stream information. */

    uint16_t          num_channels;
    /**< Number of channels for decoding.

         @values 1 to 2 */

    uint16_t          min_blk_size;
    /**< Minimum block size (in samples) used in the stream. It must be less
         than or equal to max_blk_size. */

    uint16_t          max_blk_size;
    /**< Maximum block size (in samples) used in the stream. If the minimum
         block size equals the maximum block size, a fixed block size stream
         is implied. */

    uint16_t          md5_sum[8];
    /**< MD5 signature array of the unencoded audio data. This allows the
         decoder to determine if an error exists in the audio data, even when
         the error does not result in an invalid bit stream. */

    uint32_t          sample_rate;
    /**< Number of samples per second.

         @values 8000 to 48000 Hz */

    uint32_t          min_frame_size;
    /**< Minimum frame size used in the stream.

         @values
         - > 0 bytes
         - 0 -- The value is unknown @tablebulletend */

    uint32_t          max_frame_size;
    /**< Maximum frame size used in the stream.

         @values
         - > 0 bytes
         - 0 -- The value is unknown @tablebulletend */

    uint16_t          sample_size;
    /**< Bits per sample.

         @values  16, 24 */

    uint16_t          reserved;
    /**< Used for alignment; must be set to 0. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_flac_fmt_blk_t */

/** @ingroup asmstrm_mediafmt_dts
    Media format ID for Digital Theater Systems (DTS).

    @par DTS format block
    No format block is needed. All information is contained in the bit stream.

    @par DTS encode configuration block (asm_dts_enc_cfg_t)
    @table{weak__asm__dts__enc__cfg__t}
*/
#define ASM_MEDIA_FMT_DTS                    0x00010D88

/* DTS encoder configuration structure. */
typedef struct asm_dts_enc_cfg_t asm_dts_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_dts_enc_cfg_t
@{ */
/* Payload of the DTS encoder configuration parameters in the
    ASM_MEDIA_FMT_DTS media format.
*/
struct asm_dts_enc_cfg_t
{
    uint32_t          sample_rate;
    /**< Sampling rate at which input is to be encoded.

         @values
         - 44100 -- Encode at 44.1 kHz
         - 48000 -- Encode at 48 kHz @tablebulletend */

    uint32_t          num_channels;
    /**< Number of channels for multichannel encoding.

         @values 1 to 6 */

    uint8_t   channel_mapping[16];
    /**< Channel mapping array of size 16.

         Channel[i] mapping describes channel i. Each element i of the array
         describes channel i inside the buffer where 0 @le i < num_channels.
         An unused channel is set to 0. Only the first num_channels
         elements are valid.

         @values
         - #PCM_CHANNEL_L
         - #PCM_CHANNEL_R
         - #PCM_CHANNEL_C
         - #PCM_CHANNEL_LS
         - #PCM_CHANNEL_RS
         - #PCM_CHANNEL_LFE @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_dts_enc_cfg_t */

/** @ingroup asmstrm_mediafmt_ac3
    Media format ID for the AC3 encoder and decoder (Dolby Digital
    Plus (DDP) encoder/decoder).

    @par DDP decode format block
    No format block is needed. All information is contained in the bit stream.

    @par DDP encode configuration block (asm_ddp_enc_cfg_t)
    @table{weak__asm__ddp__enc__cfg__t}
*/
#define ASM_MEDIA_FMT_AC3                 0x00010DEE

/** @ingroup asmstrm_mediafmt_eac3
    Media format ID for the E-AC3 encoder and decoder (DDP encoder/decoder).

    @par DDP decode format block
    No format block is needed. All information is contained in the bit stream.

    @par DDP encode configuration block (asm_ddp_enc_cfg_t)
    @table{weak__asm__ddp__enc__cfg__t}
 */
#define ASM_MEDIA_FMT_EAC3                0x00010DEF

/* DDP encoder configuration structure. */
typedef struct asm_ddp_enc_cfg_t asm_ddp_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_ddp_enc_cfg_t
@{ */
/* Payload of the dolby encoder configuration parameters in the
    ASM_MEDIA_FMT_AC3 or ASM_MEDIA_FMT_EAC3 media format.
*/
struct asm_ddp_enc_cfg_t
{
    uint32_t          sample_rate;
    /**< Samples at which input is to be encoded.

         @values 48000 -- Encode at 48 kHz */

    uint32_t        num_channels ;
    /**< Number of channels for multichannel encoding.

         @values 1 to 6 */

    uint32_t        bits_per_sample ;
    /**< Number of bits per sample.

         @values 16, 24 */

    uint8_t   channel_mapping[16];
    /**< Channel mapping array of size 16.

         Channel[i] mapping describes channel i. Each element i of the array
         describes channel I inside the buffer where 0 @le I < num_channels.
         An unused channel is set to 0. Only first num_channels elements
         are valid.

         @values
         - #PCM_CHANNEL_L
         - #PCM_CHANNEL_R
         - #PCM_CHANNEL_C
         - #PCM_CHANNEL_LS
         - #PCM_CHANNEL_RS
         - #PCM_CHANNEL_LFE @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_ddp_enc_cfg_t */

/** @ingroup asmstrm_mediafmt_dolbythd
    Media format ID for Dolby True HD.
*/
#define ASM_MEDIA_FMT_DOLBY_TRUE_HD                    0x00013215

/** @ingroup asmstrm_mediafmt_atrac
    Media format ID for adaptive transform acoustic coding.
*/
#define ASM_MEDIA_FMT_ATRAC                  0x00010D89

/** @ingroup asmstrm_mediafmt_mat
    Media format ID for metadata-enhanced audio transmission.
*/
#define ASM_MEDIA_FMT_MAT                    0x00010D8A

/** @ingroup asmstrm_mediafmt_dts_lbr
    Media format ID for the Digital Theater Systems Low Bit Rate (LBR) format.
*/
#define ASM_MEDIA_FMT_DTS_LBR                0x00010DBB

/** @ingroup asmstrm_mediafmt_alac
    Media format ID for the Apple Lossless Audio Codec (ALAC).

    @par ALAC format block (asm_alac_fmt_blk_t)
    @table{weak__asm__alac__fmt__blk__t} @newpage

    @par channelLayoutTag constants
    The following constants are extracted when channel layout information is
    present in the bit stream. These constants describe the channel layout
    (for more information, refer to @xhyperref{S4,ISO/IEC 13818-7}).
@verbatim
enum
  {
    kALACChannelLayoutTag_Mono         = (100<<16) | 1,   // C
    kALACChannelLayoutTag_Stereo       = (101<<16) | 2,   // L R
    kALACChannelLayoutTag_MPEG_3_0_B   = (113<<16) | 3,   // C L R
    kALACChannelLayoutTag_MPEG_4_0_B   = (116<<16) | 4,   // C L R Cs
    kALACChannelLayoutTag_MPEG_5_0_D   = (120<<16) | 5,   // C L R Ls Rs
    kALACChannelLayoutTag_MPEG_5_1_D   = (124<<16) | 6,   // C L R Ls Rs LFE
    kALACChannelLayoutTag_AAC_6_1      = (142<<16) | 7,   // C L R Ls Rs Cs LFE
    kALACChannelLayoutTag_MPEG_7_1_B   = (127<<16) | 8    // C Lc Rc L R Ls Rs LFE
                             (doc: IS-13818-7 MPEG2-AAC)
  };
@endverbatim
*/ /* S4 = ISO/IEC 13818-7, Fourth edition, 2006-01-15 */
#define ASM_MEDIA_FMT_ALAC                0x00012f31

/* ALAC decoder format block structure. */
typedef struct asm_alac_fmt_blk_t asm_alac_fmt_blk_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_alac_fmt_blk_t
@{ */
/* Payload of the ALAC encoder/decoder format block parameters in the
    ASM_MEDIA_FMT_ALAC media format.
*/
struct asm_alac_fmt_blk_t
{
    uint32_t          frameLength;
    /**< Frames per packet when no explicit frames per packet setting is
         present in the packet header.

         The encoder frames per packet can be explicitly set, but for maximum
         compatibility, use the default encoder setting of 4096. */

    uint8_t           compatibleVersion;
    /**< Specifies the compatible version.

         @values 0 */

    uint8_t           bitDepth;
    /**< Bit depth of the source PCM data.

         @values 16, 24 */

    uint8_t           pb;
    /**< Tuning parameter. (Currently not used.)

         @values 40 */

    uint8_t           mb;
    /**< Tuning parameter. (Currently not used.)

         @values 10 */

    uint8_t           kb;
    /**< Tuning parameter. (Currently not used.)

         @values 14 */

    uint8_t           numChannels;
    /**< Number of channels for multichannel decoding.

         @values 1 to 8 channels */

    uint16_t           maxRun;
    /**< Currently not used.

         @values 255  */

    uint32_t           maxFrameBytes;
    /**< Maximum size of an Apple Lossless packet within the encoded stream.

         @values 0 -- Unknown */

    uint32_t           avgBitRate;
    /**< Average bit rate in bits per second of the Apple Lossless stream.

         @values 0 -- Unknown */

    uint32_t           sampleRate;
    /**< Number of samples per second in Hertz.*/

  /* following channel layout of the bitstream is defined in ALACAudioTypes.h)*/
    uint32_t          channelLayoutTag;
    /**< Indicates whether channel layout information is present in the
         bit stream.

         @values
         - 0 -- Channel layout information is not present
         - Constants are extracted when channel layout information is
           present (for details, see <b>channelLayoutTag constants</b>)
           @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_alac_fmt_blk_t */

/** @ingroup asmstrm_mediafmt_mac
    Media format ID for the Monkey's Lossless Audio Codec (MAC/APE).

    @par Monkey decoder format block (asm_ape_fmt_blk_t)
    @table{weak__asm__ape__fmt__blk__t} @newpage
*/
#define ASM_MEDIA_FMT_APE                0x00012f32

/* Monkey decoder format block structure. */
typedef struct asm_ape_fmt_blk_t asm_ape_fmt_blk_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_ape_fmt_blk_t
@{ */
/* Payload of the Monkey encoder/decoder format block parameters in the
    ASM_MEDIA_FMT_APE media format.
*/
struct asm_ape_fmt_blk_t
{
    uint16_t           nVersion;
    /**< Specifies the compatible version.

         @values 0 */

    uint16_t           nCompressionLevel;
    /**< Compression level present in the encoded packet.
         The nBlocksPerFrame parameter value is based on the compression level.

         @values
         - COMPRESSION_LEVEL_FAST       (1000)
         - COMPRESSION_LEVEL_NORMAL     (2000)
         - COMPRESSION_LEVEL_HIGH       (3000)
         - COMPRESSION_LEVEL_EXTRA_HIGH (4000)
         - COMPRESSION_LEVEL_INSANE     (5000) @tablebulletend */

    uint32_t           nFormatFlags;
    /**< Reserved for future use. */

    uint32_t           nBlocksPerFrame;
    /**< Number of audio blocks (channel samples) in one frame present in the
         encoded packet header.

         @values Any value present in the APE header. */

    uint32_t           nFinalFrameBlocks;
    /**< Number of audio blocks (channel samples) in the final frame present
         in the encoded packet header.

         @values Any value present in the APE header. */

    uint32_t           nTotalFrames;
    /**< Total number of frames.

         @values Any value present in the APE header. */

    uint16_t           nBitsPerSample;
    /**< Bit depth of the source PCM data.

         @values 16, 24 */

    uint16_t           nChannels;
    /**< Number of channels for decoding.

         @values 1 or 2 channels */

    uint32_t           nSampleRate;
    /**< Number of samples per second.

         @values 8000 to 192000 Hz */

    uint32_t           nSeekTablePresent;
    /**< Flag used to indicate the presence of the seek table. This flag is
         not part of the APE header.

         @values
         - 0 -- Not present
         - 1 -- Present @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_ape_fmt_blk_t */

/** @ingroup asmstrm_mediafmt_dsd
    Media format ID for the Direct Stream-Digital (DSD) format.

    @par DSD format block (asm_dsd_fmt_blk_t)
    @table{weak__asm__dsd__fmt__blk__t} @newpage
 */
#define ASM_MEDIA_FMT_DSD                0x00012f3e

/* DSD decoder format block structure. */
typedef struct asm_dsd_fmt_blk_t asm_dsd_fmt_blk_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_dsd_fmt_blk_t
@{ */
/* Payload of the DSD encoder/decoder format block parameters in the
    ASM_MEDIA_FMT_DSD media format.
 */
struct asm_dsd_fmt_blk_t
{
    uint16_t           num_version;
    /**< Specifies the compatible version.

         @values 0 */

    uint16_t           is_bitwise_big_endian;
    /**< Specifies the bit-wise endianess of the bit stream within a byte.
         This value denotes whether bit reversal is required.

         @values 1 -- MSB first, 0x01 == 00000001, bit reversal is not required

         @subhead{Unsupported formats:}
         0 -- LSB first, 0x01 == 10000000, bit reversal is required */

    uint16_t           dsd_channel_block_size;
    /**< Specifies how many bytes are available for one DSD channel.

         @values 2 -- Incoming data is 2 bytes per channel; denotes DSD
          passthrough

         @subhead{Unsupported formats:}
         - 1 -- Incoming data is 1 byte per channel
         - 4096 -- Incoming data is 4096 bytes per channel @tablebulletend */

    uint16_t           num_channels;
    /**< Number of channels for decoding.

         @values 2 */

    uint8_t            channel_mapping[8];
    /**< Channel mapping array of DSD bit stream output.

         Channel[i] mapping describes channel i inside the buffer, where
         0 @le i < num_channels. All valid used channels must be present at
         the beginning of the array.

         @values See Section @xref{dox:PcmChannelDefs} */

    uint32_t           dsd_data_rate;
       /**< Number of 1-bit DSD samples per second.

            @values 2822400, 5644800  Hz */

   }
   #include "adsp_end_pack.h"
   ;
   /** @} */ /* end_weakgroup weak_asm_dsd_fmt_blk_t */


#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*_ADSP_MEDIA_FMT_PROP_H_*/
