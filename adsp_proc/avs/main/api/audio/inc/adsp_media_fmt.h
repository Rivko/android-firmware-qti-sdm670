/*========================================================================*/
/**
@file adsp_media_fmt.h

This file contains the structure definitions for the media format
blocks used in client-ASM communications.
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

$Header: //components/rel/avs.adsp/2.8.5/api/audio/inc/adsp_media_fmt.h#1 $

when       who     what, where, why
--------   ---      -------------------------------------------------------
07/16/16   sw      (Tech Pubs) Edited Doxygen comments for 8998.2.8.
10/22/15   sw      (Tech Pubs) Edited Doxygen comments for 8996.
05/08/15   sw      (Tech Pubs) Edited Doxygen comments for 8952
09/10/14   sw      (Tech Pubs) Merged Doxygen comments from 2.4; edited
                    Doxygen comments/markup for 2.6.
09/06/13   sw      (Tech Pubs) Merged Doxygen comments/markup from 2.0; edited
                    new Doxygen comments for 2.2.
08/06/12   sw      (Tech Pubs) Edited comments; updated Doxygen markup to
                   Rev D.2 templates; updated legal statements for QTI.
05/30/11   sw      (Tech Pubs) Updated Doxygen comments for Interface Spec doc.
10/12/10   leo     (Tech Pubs) Edited/added Doxygen comments and markup.
04/15/10   rkc      Created file.
========================================================================== */

#ifndef _ADSP_MEDIA_FMT_H_
#define _ADSP_MEDIA_FMT_H_

#include "mmdefs.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/


/** @ingroup asmstrm_mediafmt_eg
    Media format example for clients to try out a dummy encoder or decoder.
    This format is for illustration purposes only.

    @par Example format block
    No format block is required.

    @par Example encode configuration block
    No format block is required.
*/
#define ASM_MEDIA_FMT_EXAMPLE                   0x00010C4D

/** @addtogroup asmstrm_mediafmt_pcm_ch_defs
@{ */

/** Front left channel. */
#define PCM_CHANNEL_L    1

/** Front right channel. */
#define PCM_CHANNEL_R    2

/** Front center channel. */
#define PCM_CHANNEL_C    3

/** Left surround channel.

    This speaker position enumeration is maintained for backward compatibility.
    Currently, there are no devices that support this type. The client is
    responsible for ensuring that valid and existing channel mapping
    configurations corresponding to real speaker positions are used to ensure
    proper user experience without audio/data losses.
*/
#define PCM_CHANNEL_LS   4

/** Right surround channel.

    This speaker position enumeration is maintained for backward compatibility.
    Currently, there are no devices that support this type. The client is
    responsible for ensuring that valid and existing channel mapping
    configurations corresponding to real speaker positions are used to ensure
    proper user experience without audio/data losses.
*/
#define PCM_CHANNEL_RS   5

/** Low frequency effect channel. */
#define PCM_CHANNEL_LFE  6

/** Center surround channel; rear center channel. */
#define PCM_CHANNEL_CS   7

/** Center back channel. */
#define PCM_CHANNEL_CB  PCM_CHANNEL_CS

/** Left back channel; rear left channel. */
#define PCM_CHANNEL_LB   8

/** Right back channel; rear right channel. */
#define PCM_CHANNEL_RB   9

/** Top surround channel.

    This speaker position enumeration is maintained for backward compatibility.
    Currently, there are no devices that support this type. The client is
    responsible for ensuring that valid and existing channel mapping
    configurations corresponding to real speaker positions are used to ensure
    proper user experience without audio/data losses.
*/
#define PCM_CHANNEL_TS   10

/** Center vertical height channel.

    This speaker position enumeration is maintained for backward compatibility.
    Currently, there are no devices that support this type. The client is
    responsible for ensuring that valid and existing channel mapping
    configurations corresponding to real speaker positions are used to ensure
    proper user experience without audio/data losses.
*/
#define PCM_CHANNEL_CVH  11

/** Top front center channel.

    Currently, there are no devices that support this type. The client is
    responsible for ensuring that valid and existing channel mapping
    configurations corresponding to real speaker positions are used to ensure
    proper user experience without audio/data losses.
*/
#define PCM_CHANNEL_TFC  PCM_CHANNEL_CVH

/** Mono surround channel.

    This speaker position enumeration is maintained for backward compatibility.
    Currently, there are no devices that support this type. The client is
    responsible for ensuring that valid and existing channel mapping
    configurations corresponding to real speaker positions are used to ensure
    proper user experience without audio/data losses.
*/
#define PCM_CHANNEL_MS   12

/** Front left of center channel. */
#define PCM_CHANNEL_FLC  13

/** Front right of center channel. */
#define PCM_CHANNEL_FRC  14

/** Rear left of center channel. */
#define PCM_CHANNEL_RLC  15

/** Rear right of center channel. */
#define PCM_CHANNEL_RRC  16

/** Secondary low frequency effect channel. */
#define PCM_CHANNEL_LFE2  17

/** Side left channel. */
#define PCM_CHANNEL_SL  18

/** Side right channel. */
#define PCM_CHANNEL_SR  19

/** Top front left channel. */
#define PCM_CHANNEL_TFL  20

/** Left vertical height channel. */
#define PCM_CHANNEL_LVH  PCM_CHANNEL_TFL

/** Top front right channel. */
#define PCM_CHANNEL_TFR 21

/** Right vertical height channel. */
#define PCM_CHANNEL_RVH PCM_CHANNEL_TFR

/** Top center channel. */
#define PCM_CHANNEL_TC  22

/** Top back left channel. */
#define PCM_CHANNEL_TBL  23

/** Top back right channel. */
#define PCM_CHANNEL_TBR  24

/** Top side left channel. */
#define PCM_CHANNEL_TSL  25

/** Top side right channel. */
#define PCM_CHANNEL_TSR  26

/** Top back center channel. */
#define PCM_CHANNEL_TBC  27

/** Bottom front center channel. */
#define PCM_CHANNEL_BFC  28

/** Bottom front left channel. */
#define PCM_CHANNEL_BFL  29

/** Bottom front right channel. */
#define PCM_CHANNEL_BFR  30

/** Left wide channel. */
#define PCM_CHANNEL_LW  31

/** Right wide channel. */
#define PCM_CHANNEL_RW  32

/** Left side direct channel. */
#define PCM_CHANNEL_LSD  33

/** Right side direct channel. */
#define PCM_CHANNEL_RSD  34

/** @} */ /* end_addtogroup asmstrm_mediafmt_pcm_ch_defs */

/** @ingroup asmstrm_mediafmt_mchan_pcm_v2
    Media format ID for multiple channel linear PCM.

    @par Multiple Channel PCM format block (asm_multi_channel_pcm_fmt_blk_v2_t}
    @tablens{weak__asm__multi__channel__pcm__fmt__blk__v2__t}

    @par Multiple Channel PCM encode configuration block (asm_multi_channel_pcm_enc_cfg_v2_t)
    @table{weak__asm__multi__channel__pcm__enc__cfg__v2__t}
*/
#define ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2                0x00010DA5

/* Linear multiple channel PCM decoder format block structure. */
typedef struct asm_multi_channel_pcm_fmt_blk_v2_t asm_multi_channel_pcm_fmt_blk_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_multi_channel_pcm_fmt_blk_v2_t
@{ */
/* Payload of the Multiple Channel PCM decoder format block parameters
    ASM_STREAM_CMD_OPEN_WRITE_V2 command. */
/** The data must be in little-endian format, and it must always be
    interleaved.
*/
struct asm_multi_channel_pcm_fmt_blk_v2_t
{
    uint16_t  num_channels;
    /**< Number of channels.

         @values 1 to 8 */

    uint16_t  bits_per_sample;
    /**< Number of bits per sample per channel.

         @values 16, 24

         When used for playback, the client must send 24-bit samples packed in
         32-bit words. The 24-bit samples must be placed in the most
         significant 24 bits of the 32-bit word.

         When used for recording, the aDSP sends 24-bit samples packed in
         32-bit words. The 24-bit samples are placed in the most significant
         24 bits of the 32-bit word. */

    uint32_t  sample_rate;
    /**< Number of samples per second.

         @values 2000 to 48000, 96000, 192000 Hz */

    uint16_t  is_signed;
    /**< Flag that indicates the PCM samples are signed (1). */

    uint16_t  reserved;
    /**< Used for alignment; must be set to 0. */

    uint8_t   channel_mapping[8];
    /**< Channel mapping array of size 8.

         Channel[i] mapping describes channel i. Each element i of the array
         describes channel i inside the buffer where 0 @le i < num_channels.
         An unused channel is set to 0.

         @values See Section @xref{dox:PcmChannelDefs} */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_multi_channel_pcm_fmt_blk_v2_t */

/* Multichannel PCM encoder configuration structure. */
typedef struct asm_multi_channel_pcm_enc_cfg_v2_t asm_multi_channel_pcm_enc_cfg_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_multi_channel_pcm_enc_cfg_v2_t
@{ */
/* Payload of the multichannel PCM encoder configuration parameters in
    the ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2 media format.
*/
struct asm_multi_channel_pcm_enc_cfg_v2_t
{
   uint16_t  num_channels;
    /**< Number of PCM channels.

         @values
         - 0 -- Native mode
         - 1 -- 8 channels

         Native mode indicates that encoding must be performed with the number
         of channels at the input. */

    uint16_t  bits_per_sample;
    /**< Number of bits per sample per channel.

         @values 16, 24 */

    uint32_t  sample_rate;
    /**< Number of samples per second.

         @values 0, 8000 to 48000 Hz

         A value of 0 indicates the native sampling rate. Encoding is
         performed at the input sampling rate. */

    uint16_t  is_signed;
    /**< Flag that indicates the PCM samples are signed (1). Currently, only
         signed samples are supported. */

    uint16_t  reserved;
    /**< Used for alignment; must be set to 0. */

    uint8_t   channel_mapping[8];
    /**< Channel mapping array expected at the encoder output.

         Channel[i] mapping describes channel i inside the buffer, where
         0 @le i < num_channels. All valid used channels must be present at
         the beginning of the array.

         If Native mode is set for the channels, this field is ignored.

         @values See Section @xref{dox:PcmChannelDefs} */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_multi_channel_pcm_enc_cfg_v2_t */

/** @ingroup asmstrm_mediafmt_mchan_pcm_v3
    Media format ID for multiple channel linear PCM. This format has a flag
    that indicates whether samples are packed in 32-bit words or in 24-bit
    words (when bits_per_sample = 24).

    @par Multiple channel PCM format block (asm_multi_channel_pcm_fmt_blk_v3_t}
    @tablens{weak__asm__multi__channel__pcm__fmt__blk__v3__t}

    @par Multiple channel PCM encode configuration block (asm_multi_channel_pcm_enc_cfg_v3_t)
    @table{weak__asm__multi__channel__pcm__enc__cfg__v3__t}
*/
#define ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3                0x00010DDC

/* Linear multiple channel PCM decoder format block structure. */
typedef struct asm_multi_channel_pcm_fmt_blk_v3_t asm_multi_channel_pcm_fmt_blk_v3_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_multi_channel_pcm_fmt_blk_v3_t
@{ */
/* Payload of the Multiple Channel PCM decoder format block parameters
 */
/** The data must be in little-endian format, and it must always be
    interleaved.
*/
struct asm_multi_channel_pcm_fmt_blk_v3_t
{
    uint16_t  num_channels;
    /**< Number of channels.

         @values 1 to 8 */

    uint16_t  bits_per_sample;
    /**< Number of bits per sample per channel.

         @values 16, 24 */

    uint32_t  sample_rate;
    /**< Number of samples per second.

         @values 2000 to 48000, 96000, 192000 Hz */

    uint16_t  is_signed;
    /**< Flag that indicates the PCM samples are signed (1). */

    uint16_t    sample_word_size;
    /**< Size in bits of the word that holds a sample of a channel.

          @values 16, 24, 32

         16-bit samples are always placed in 16-bit words:
         sample_word_size = 16.

         24-bit samples can be placed in 32-bit words or in consecutive
            24-bit words.
         - If sample_word_size = 32, 24-bit samples are placed in the
           most significant 24 bits of a 32-bit word.
         - If sample_word_size = 24, 24-bit samples are placed in
           24-bit words. @tablebulletend */

    uint8_t   channel_mapping[8];
    /**< Channel mapping array of size 8.

         Channel[i] mapping describes channel i. Each element i of the array
         describes channel i inside the buffer where 0 @le i < num_channels.
         An unused channel is set to 0.

         @values See Section @xref{dox:PcmChannelDefs} */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_multi_channel_pcm_fmt_blk_v3_t */

/* Multichannel PCM encoder configuration structure. */
typedef struct asm_multi_channel_pcm_enc_cfg_v3_t asm_multi_channel_pcm_enc_cfg_v3_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_multi_channel_pcm_enc_cfg_v3_t
@{ */
/* Payload of the multichannel PCM encoder configuration parameters in
    the ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V3 media format.
*/
struct asm_multi_channel_pcm_enc_cfg_v3_t
{
   uint16_t  num_channels;
    /**< Number of PCM channels.

         @values
         - 0 -- Native mode
         - 1 -- 8 channels

         Native mode indicates that encoding must be performed with the number
         of channels at the input. */

    uint16_t  bits_per_sample;
    /**< Number of bits per sample per channel.

         @values 16, 24 */

    uint32_t  sample_rate;
    /**< Number of samples per second.

         @values 0, 8000 to 48000 Hz

         A value of 0 indicates the native sampling rate. Encoding is
         performed at the input sampling rate. */

    uint16_t  is_signed;
    /**< Flag that indicates the PCM samples are signed (1). Currently, only
         signed PCM samples are supported. */

    uint16_t    sample_word_size;
    /**< The size in bits of the word that holds a sample of a channel.

         @values 16, 24, 32

         16-bit samples are always placed in 16-bit words:
         sample_word_size = 1.

         24-bit samples can be placed in 32-bit words or in consecutive
         24-bit words.
         - If sample_word_size = 32, 24-bit samples are placed in the
           most significant 24 bits of a 32-bit word.
         - If sample_word_size = 24, 24-bit samples are placed in
           24-bit words. @tablebulletend */

    uint8_t   channel_mapping[8];
    /**< Channel mapping array expected at the encoder output.

         Channel[i] mapping describes channel i inside the buffer, where
         0 @le i < num_channels. All valid used channels must be present at
         the beginning of the array.

         If Native mode is set for the channels, this field is ignored.

         @values See Section @xref{dox:PcmChannelDefs} */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_multi_channel_pcm_enc_cfg_v3_t */

/* Custom encoder configuration structure. */
typedef struct asm_custom_enc_cfg_t asm_custom_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @ingroup asmstrm_mediafmt_custom_id
    Payload of the custom encoder configuration parameters in the default media
    format.

    Following this structure is the payload that is specific to the encoder
    media format. The size of this encoder-specific payload is provided by
    custom_payload_size. The client must ensure 32-bit alignment.
*/
struct asm_custom_enc_cfg_t
{
    uint32_t  sample_rate;
       /**< Number of samples per second.

            @values 0, 8000 to 192000 Hz

            A value of 0 indicates the native sampling rate. Encoding is
            performed at the input sampling rate. */

    uint16_t  num_channels;
    /**< Number of PCM channels.

         @values
         - 0 -- Native mode
         - 1 -- 8 channels

         Native mode indicates that encoding must be performed with the number
         of channels at the input. */

    uint16_t  reserved;
    /**< Used for alignment; must be set to 0. */  /* reserved for 32-bit alignment */

    uint8_t   channel_mapping[8];
    /**< Channel mapping array expected at the encoder output.

         Channel[i] mapping describes channel i inside the buffer, where
         0 @le i < num_channels. All valid used channels must be present at
         the beginning of the array.

         If Native mode is set for the channels, this field is ignored.

         @values See Section @xref{dox:PcmChannelDefs} */

    uint32_t custom_payload_size;
    /**< Size of the custom payload that immediately follows this structure.

         @note1hang This size does not include bytes added for 32-bit
                    alignment. */

}
#include "adsp_end_pack.h"
;

/** @ingroup asmstrm_mediafmt_mchan_pcm_v4
    Media format ID for multiple channel linear PCM.

    This format has the following additions:
    - Support for 32-bit PCM
    - Flag to indicate whether samples are stored in little endian or big
      endian byte order

    The following stream open commands support this media format:
    - #ASM_STREAM_CMD_OPEN_WRITE_V3
    - #ASM_STREAM_CMD_OPEN_READWRITE_V2
    - #ASM_STREAM_CMD_OPEN_READ_V3
    - #ASM_STREAM_CMD_OPEN_LOOPBACK_V2
    - #ASM_STREAM_CMD_OPEN_TRANSCODE_LOOPBACK
    - #ASM_STREAM_CMD_OPEN_PULL_MODE_WRITE
    - #ASM_STREAM_CMD_OPEN_PUSH_MODE_READ

    @par Multiple Channel PCM format block (asm_multi_channel_pcm_fmt_blk_v4_t}
    @table{weak__asm__multi__channel__pcm__fmt__blk__v4__t}

    @par Multiple Channel PCM encode configuration block (asm_multi_channel_pcm_enc_cfg_v4_t)
    @table{weak__asm__multi__channel__pcm__enc__cfg__v4__t}
*/
#define ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V4                0x0001320C

/* Linear multiple channel PCM decoder format block structure. */
typedef struct asm_multi_channel_pcm_fmt_blk_v4_t asm_multi_channel_pcm_fmt_blk_v4_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_multi_channel_pcm_fmt_blk_v4_t
@{ */
/* Payload of the Multiple Channel PCM decoder format block parameters
*/
struct asm_multi_channel_pcm_fmt_blk_v4_t
{
   uint16_t  num_channels;
   /**< Number of channels.

        @values 1 to 8 */

   uint16_t  bits_per_sample;
   /**< Number of bits per sample per channel.

        @values 16, 24, 32 */

   uint32_t  sample_rate;
   /**< Number of samples per second.

        @values 0 to 384000 Hz */

   uint16_t  is_signed;
   /**< Flag that indicates whether the PCM samples are signed (1). */

   uint16_t  sample_word_size;
   /**< Size in bits of the word that holds the sample of a channel.

        @values 16, 24, 32
        - 16-bit sample words always contain 16-bit samples.
        - 24-bit sample words always contain 24-bit samples.
        - 32-bit sample words have two cases:
          - If bits_per_sample = 24, 24-bit samples are placed in the
            most significant 24 bits of a 32-bit word.
          - If bits_per_sample = 32, 32-bit samples are placed in the
            32-bit words @tablebulletend */

   uint8_t  channel_mapping[8];
   /**< Channel mapping array of size 8.

        Channel[i] mapping describes channel i. Each element i of the array
        describes channel i inside the buffer where 0 @le i < num_channels.
        An unused channel is set to 0.

        @values See Section @xref{dox:PcmChannelDefs} */

   uint16_t  endianness;
   /**< Indicates whether PCM samples are stored in little endian or big
        endian format.

        @values
        - 0 -- Little endian
        - 1 -- Big endian @tablebulletend */

   uint16_t  mode;
   /**< Indicates the sample modes.

        Currently, the five LSBs indicate the Q factor of the input signal.
        The rest of the 11 bits are set to 0.

        @values
        - 0  -- Default Q factors \n
          (Q15 for 16 bits, Q23 for 24-bit packed, Q31 for 24-bit unpacked,
          Q31 for 32 bits)
        - 15 -- Both bits_per_sample and sample_word_size are 16 bits
        - 23 -- Either bits_per_sample is 24 and sample_word_size is 32 bits \n
          (8.24 format where 8 MSBs are sign-extended), \n
          or both bits_per_sample and sample_word_size are 24 bits (24-bit
          packed)
        - 31 -- Either bits_per_sample is 24 and sample_word_size is 32 bits \n
          (24-bit unpacked in 24.8 format where 8 LSBs are zeroes), \n
          or both bits_per_sample and sample_word_size are 32 bits
          @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_multi_channel_pcm_fmt_blk_v4_t */

/* Multichannel PCM encoder configuration structure. */
typedef struct asm_multi_channel_pcm_enc_cfg_v4_t asm_multi_channel_pcm_enc_cfg_v4_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_multi_channel_pcm_enc_cfg_v4_t
@{ */
/* Payload of the multichannel PCM encoder configuration parameters in
   the ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V4 media format.
*/
struct asm_multi_channel_pcm_enc_cfg_v4_t
{
  uint16_t  num_channels;
   /**< Number of PCM channels.

        @values
        - 0 -- Native mode
        - 1 to 8 channels

        Native mode indicates that encoding must be performed with the number
        of channels at the input. */

   uint16_t  bits_per_sample;
   /**< Number of bits per sample per channel.

        @values 16, 24, 32 */

   uint32_t  sample_rate;
   /**< Number of samples per second.

        @values 0 to 384000 Hz

        Zero means Native mode, which indicates that encoding must be
        performed with the input sampling rate. */

   uint16_t  is_signed;
   /**< Flag that indicates whether the PCM samples are signed.

        @values 1 -- Currently, only signed PCM values are supported */

   uint16_t  sample_word_size;
   /**< Size in bits of the word that holds the sample of a channel.

        @values 16, 24, 32
        - 16-bit sample words always contain 16-bit samples.
        - 24-bit sample words always contain 24-bit samples.
        - 32-bit sample words can have two cases:
          - If bits_per_sample = 24, 24-bit samples are placed in the
            most significant 24 bits of a 32-bit word
          - If bits_per_sample = 32, 32-bit samples are placed in the
            32-bit word @tablebulletend */

   uint8_t  channel_mapping[8];
   /**< Channel mapping array expected at the encoder output.

        Channel[i] mapping describes channel i inside the buffer, where
        0 @le i < num_channels. All valid used channels must be present at
        the beginning of the array.

        If Native mode is set for the channels, this field is ignored.

        @values See Section @xref{dox:PcmChannelDefs} */

   uint16_t endianness;
    /**< Indicates whether PCM samples are stored in little endian or big
         endian format.

         @values
         - 0 -- Little endian
         - 1 -- Big endian @tablebulletend */

   uint16_t  mode;
   /**< Indicates the sample modes.

        Currently, the five LSBs are used to indicate the Q factor of the input
        signal. The rest of the 11 bits are set to 0.

        @values
        - 0  -- Default Q factors \n
          (Q15 for 16 bits, Q23 for 24-bit packed, Q31 for 24-bit unpacked,
          Q31 for 32 bits)
        - 15 -- Both bits_per_sample and sample_word_size are 16 bits
        - 23 -- Either bits_per_sample is 24 and sample_word_size is 32 bits \n
          (8.24 format where 8 MSBs are sign-extended), \n
          or both bits_per_sample and sample_word_size are 24 bits (24-bit
          packed)
        - 31 -- Either bits_per_sample is 24 and sample_word_size is 32 bits \n
          (24-bit unpacked in 24.8 format where 8 LSBs are zeroes), \n
          or both bits_per_sample and sample_word_size are 32 bits
          @tablebulletend */

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_multi_channel_pcm_enc_cfg_v4_t */

/** @ingroup asmstrm_mediafmt_gen_compr
    Media format ID indicating generic passthrough on a compressed path.
    This passthrough means there is no knowledge of the original media format.

    @par Generic compressed format block (asm_generic_compressed_fmt_blk_t)
    @table{weak__asm__generic__compressed__fmt__blk__t} @newpage
*/
#define ASM_MEDIA_FMT_GENERIC_COMPRESSED                0x00013212

/* generic compressed passthrough block structure. */
typedef struct asm_generic_compressed_fmt_blk_t asm_generic_compressed_fmt_blk_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_asm_generic_compressed_fmt_blk_t
@{ */
/* Payload of the generic compressed passthru media format block parameters in the
       ASM_MEDIA_FMT_GENERIC_COMPRESSED media format.
*/
struct asm_generic_compressed_fmt_blk_t
   {
   uint8_t            channel_mapping[8];
   /**< Channel mapping array of PCM-packetized bit stream output.

        Channel[i] mapping describes channel i inside the buffer, where
        0 @le i < num_channels. All valid used channels must be present at
        the beginning of the array.

        @values See Section @xref{dox:PcmChannelDefs} */

    uint16_t           num_channels;
    /**< Number of channels of the outgoing PCM-packetized bit stream.

         @values 1, 2, 3, 4, 5, 6, 7, 8 */

    uint16_t        bits_per_sample;
    /**< Nominal bits-per-sample value of the outgoing PCM-packetized
         bit stream.

         @values 16, 32 */

    uint32_t        sampling_rate;
    /**< Nominal sampling rate (in Hertz) of the outgoing PCM-packetized bit stream.

         @values 8000, 11025, 16000, 22050, 24000, 32000, 44100, 48000, 88200,
                 96000, 176400, 192000, 352800, 384000 */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_asm_generic_compressed_fmt_blk_t */

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*_ADSP_MEDIA_FMT_H_*/
