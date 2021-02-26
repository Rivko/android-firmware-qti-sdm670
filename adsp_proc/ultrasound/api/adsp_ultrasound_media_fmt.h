/*========================================================================*/
/**
@file adsp_ultrasound_media_fmt.h

@brief This file contains the structure definitions for the media format
blocks used in client-USM communications.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The Ultrasound_mainpage.dox file contains all file/group 
      descriptions that are in the output PDF generated using Doxygen and 
      Latex. To edit or update any of the file/group text in the PDF, edit 
      the Ultrasound_mainpage.dox file or contact Tech Pubs.
===========================================================================*/

/*===========================================================================
  Copyright (c) 2010-2014 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*========================================================================
Edit History

$Header: //components/rel/ultrasound.adsp/2.7/api/adsp_ultrasound_media_fmt.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
10/13/10   NR      File created
10/18/11   SW      (Tech Pubs) Edited Doxygen markup/comments for new Interface Spec.
02/14/12   SW      (Tech Pubs) Edited Doxygen markup/comments for Rev B of the IS.
05/09/12   IM      API changes required for supporting Ultrasound Proximity detection algorithm.
10/06/12   SW      (Tech Pubs) Updated legal statements (lines 20 & 22) for QTI;
                   updated Doxygen markup for D.2 templates.
12/24/12   IM      Decided proximity decision according to 2 frames
04/28/13   IM      Add support for proximity motion detection
05/12/13   IM      Add support for Rx path High Pass Filter used in the proximity library
05/20/13   IM      Add support for double notch filter used in proximity library 
05/28/13   IM      Added support for up to 8 channels input 
09/08/13   LD      Added sync free form media format (TX-RX synchronization) 
10/20/13   LD      Added sync gesture media format 
11/12/14   RD      Added sync proximity media format  
========================================================================== */

#ifndef _ADSP_ULSTRASOUND_MEDIA_FMT_H_
#define _ADSP_ULSTRASOUND_MEDIA_FMT_H_

#include "mmdefs.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/


#define USM_MEDIA_FMT_OUTPUT_TYPE_RAW_DATA                     0x1
#define USM_MEDIA_FMT_OUTPUT_TYPE_PROXIMITY_EVENT              0x2
#define USM_MEDIA_FMT_OUTPUT_TYPE_MOTION_EVENT                 0x4
#define USM_MEDIA_FMT_OUTPUT_TYPE_PROXIMITY_MOTION_EVENT       ((USM_MEDIA_FMT_OUTPUT_TYPE_MOTION_EVENT)|(USM_MEDIA_FMT_OUTPUT_TYPE_PROXIMITY_EVENT))
#define USM_MEDIA_FMT_OUTPUT_TYPE_ALL                          ((USM_MEDIA_FMT_OUTPUT_TYPE_RAW_DATA)|(USM_MEDIA_FMT_OUTPUT_TYPE_MOTION_EVENT)|(USM_MEDIA_FMT_OUTPUT_TYPE_PROXIMITY_EVENT))
#define USM_MEDIA_FMT_OUTPUT_TYPE_GESTURE_EVENT                0x8

#define USM_MEDIA_FMT_ALGORITHM_TYPE_MASK ( 0x0000000F )
#define USM_MEDIA_FMT_ALGORITHM_TYPE_SHFT ( 0 )
#define USM_MEDIA_FMT_DOUBLE_FRAME_PROX_DECISION_MASK ( 0x00000010 )
#define USM_MEDIA_FMT_DOUBLE_FRAME_PROX_DECISION_SHFT ( 4 )
#define USM_MEDIA_FMT_HPF_BYPASS_MASK ( 0x00000020 )
#define USM_MEDIA_FMT_HPF_BYPASS_SHFT ( 5 )
#define USM_MEDIA_FMT_JAMMER_DETECTION_FFT_ENA_MASK ( 0x00000040 )
#define USM_MEDIA_FMT_JAMMER_DETECTION_FFT_ENA_SHFT ( 6 )
#define USM_MEDIA_FMT_RX_HPF_ENA_MASK ( 0x00000080 )
#define USM_MEDIA_FMT_RX_HPF_ENA_SHFT ( 7 )
#define USM_MEDIA_FMT_DOUBLE_NOTCH_FILTER_ENA_MASK ( 0x00000100 )
#define USM_MEDIA_FMT_DOUBLE_NOTCH_FILTER_ENA_SHFT ( 8 )

#define USM_MEDIA_FMT_MAX_SUPPORTED_INPUT_CHANNELS (8)
#define USM_MEDIA_FMT_MAX_SUPPORTED_OUTPUT_CHANNELS (8)

/** @ingroup usm_media_format_free_form
    Media format for ultrasound free-form gesture recognition and hovering.

  @enc_cfg_blk{usm_free_form_enc_cfg_t}
    @table{weak__usm__free__form__enc__cfg__t}

  @par Encoder mapping vector
    The encoder mapping vector is used to inform the encoder in what order
    to arrange the output stream. Two parameters influence this mapping:
    @par
    - The channel order in which the data is received from the codec (four
      channels are received).
    - The channel order in which the upper layers expect to receive the
      data (only three channels are passed).
    @par
    The mapping vector describes how to create the output from the input.
    @par
    Figure @xref{fig:EncMapVec} illustrates an example of how to
    create the output from the input.
    @inputfigcap{1,encoder_mapping_vector.png,Encoder mapping vector,EncMapVec}

  @dec_cfg_blk{usm_free_form_dec_cfg_t}
    @table{weak__usm__free__form__dec__cfg__t}
*/
#define USM_MEDIA_FMT_FREE_FORM                0x0001127C

/* PCM encoder configuration structure */
typedef struct usm_free_form_enc_cfg_t usm_free_form_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_usm_free_form_enc_cfg_t
@{ */
/* Free-form gesture recognition and hovering encoder configuration
    block used by USM_MEDIA_FMT_FREE_FORM.
*/
struct usm_free_form_enc_cfg_t
{
    uint16_t            num_channels;
    /**< Number of microphones (channels) required for the gesture recognition
         algorithms.

         @values 3 */

    uint16_t            bits_per_sample;
    /**< Number of bits per sample per channel.

         @values 16, 32 */

    uint32_t            sample_rate;
    /**< Number of samples per second.

         @values 96000, 192000 Hz */

    uint32_t            port_id;
    /**< Valid Tx port ID.

         @values Refer to @xhyperref{Q7,[Q7]} */
    /* Q7=Hexagon Multimedia: AFE API Interface Specification (80-N3013-1)*/

    uint32_t            encoder_mapping_vector;
    /**< Order in which to arrange the output stream.
    
         @values
         - Bits 0 to 7 -- First channel
         - Bits 8 to 15 -- Second channel
         - Bits 16 to 23 -- Third channel
         - Bits 24 to 31 -- 0 (currently not applicable)

         For more details, see @xhyperref{fig:EncMapVec,Encoder mapping
         vector}. */

    uint16_t            skip_factor;
    /**< Factor between sent and skipped frames. For more details, see Section
         @xref{sec:FrameDilution}.
    
         @values
         - 0 -- No skipping
         - > 0 -- Skipping factor @tablebulletend */

    uint16_t            grouping_factor;
    /**< Number of packets to group together. For more details, see Section
         @xref{sec:FrameDilution}.
    
         @values
         - 0 -- Invalid
         - 1 -- Send each packet separately
         - > 1 -- Packets are grouped together; the value is the number of
                  grouped packets @tablebulletend */

     uint32_t           samples_per_frame;
     /**< Number of samples per frame.
    
          @values > 0 */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_usm_free_form_enc_cfg_t */

/* PCM decoder configuration structure */
typedef struct usm_free_form_dec_cfg_t usm_free_form_dec_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_usm_free_form_dec_cfg_t
@{ */
/* Free-form gesture recognition and hovering decoder configuration
    block used by USM_MEDIA_FMT_FREE_FORM.
*/
struct usm_free_form_dec_cfg_t
{
    uint16_t            num_speakers;
    /**< Number of microphones (channels) required for the gesture recognition
         algorithms.

         @values 1, 2 */

    uint16_t            bits_per_sample;
    /**< Number of bits per sample per channel.

         @values 16, 32 */

    uint32_t            sample_rate;
    /**< Number of samples per second.

         @values 96000, 192000 Hz */

    uint32_t            port_id;
    /**< Valid Rx port ID.

         @values Refer to @xhyperref{Q7,[Q7]} */

    uint32_t            decoder_mapping_vector;
    /**< Order in which to arrange the input stream.
    
         @values
         - 0 -- DMA port 0
         - 1 -- DMA port 1 @tablebulletend */

    uint16_t            samples_per_speaker;
     /**< Number of samples per speaker.
    
          @values > 0 */

    uint16_t            grouping_factor;
    /**< Number of packets to group together. For more details, see Section
         @xref{sec:FrameDilution}.
    
         @values
         - 0 -- Invalid
         - 1 -- Send each packet separately
         - > 1 -- Packets are grouped together; the value is the number of
                  grouped packets @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_usm_free_form_dec_cfg_t */


/** @ingroup usm_media_format_free_form
    Media format for ultrasound free-form gesture recognition and hovering.

  @enc_cfg_blk{usm_free_form_enc_cfg_v2_t}
    @table{weak__usm__free__form__enc__cfg__v2__t}

  @par Encoder mapping vector
    The encoder mapping vector is used to inform the encoder in what order
    to arrange the output stream. Two parameters influence this mapping:
    @par
    - The channel order in which the data is received from the codec (up to 
      8 channels are received).
    - The channel order in which the upper layers expect to receive the
      data.
    @par
    The mapping vector describes how to create the output from the input.
    @par
    Figure @xref{fig:EncMapVec} illustrates an example of how to
    create the output from the input.
    @inputfigcap{1,encoder_mapping_vector.png,Encoder mapping vector,EncMapVec}

  @dec_cfg_blk{usm_free_form_dec_cfg_v2_t}
    @table{weak__usm__free__form__dec__cfg__v2__t}
*/
#define USM_MEDIA_FMT_FREE_FORM_V2                0x0001272C

/* PCM encoder configuration structure */
typedef struct usm_free_form_enc_cfg_v2_t usm_free_form_enc_cfg_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_usm_free_form_enc_cfg_v2_t
@{ */
/* Free-form gesture recognition and hovering encoder configuration
    block used by USM_MEDIA_FMT_FREE_FORM_V2.
*/
struct usm_free_form_enc_cfg_v2_t
{
    uint16_t            num_channels;
    /**< Number of microphones (channels) required for the gesture recognition
         algorithms.

         @values 3 */

    uint16_t            bits_per_sample;
    /**< Number of bits per sample per channel.

         @values 16, 32 */

    uint32_t            sample_rate;
    /**< Number of samples per second.

         @values 96000, 192000 Hz */

    uint32_t            port_id;
    /**< Valid Tx port ID.

         @values Refer to @xhyperref{Q7,[Q7]} */
    /* Q7=Hexagon Multimedia: AFE API Interface Specification (80-N3013-1)*/

    uint8_t   encoder_mapping_vector[USM_MEDIA_FMT_MAX_SUPPORTED_INPUT_CHANNELS];
    /**< Channel array of size 8. Channel[i] mapping describes channel I. Each
         element i of the array describes channel I inside the buffer where 0
         @le I < num_channels. An unused channel is set to zero.

         @values See Section @xref{hdr:PcmChannelDefs} */


    uint16_t            skip_factor;
    /**< Factor between sent and skipped frames. For more details, see Section
         @xref{sec:FrameDilution}.
    
         @values
         - 0 -- No skipping
         - > 0 -- Skipping factor @tablebulletend */

    uint16_t            grouping_factor;
    /**< Number of packets to group together. For more details, see Section
         @xref{sec:FrameDilution}.
    
         @values
         - 0 -- Invalid
         - 1 -- Send each packet separately
         - > 1 -- Packets are grouped together; the value is the number of
                  grouped packets @tablebulletend */

     uint32_t           samples_per_frame;
     /**< Number of samples per frame.
    
          @values > 0 */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_usm_free_form_enc_cfg_v2_t */

/* PCM decoder configuration structure */
typedef struct usm_free_form_dec_cfg_v2_t usm_free_form_dec_cfg_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_usm_free_form_dec_cfg_v2_t
@{ */
/* Free-form gesture recognition and hovering decoder configuration
    block used by USM_MEDIA_FMT_FREE_FORM_V2.
*/
struct usm_free_form_dec_cfg_v2_t
{
    uint16_t            num_speakers;
    /**< Number of microphones (channels) required for the gesture recognition
         algorithms.

         @values 1, 2 */

    uint16_t            bits_per_sample;
    /**< Number of bits per sample per channel.

         @values 16, 32 */

    uint32_t            sample_rate;
    /**< Number of samples per second.

         @values 96000, 192000 Hz */

    uint32_t            port_id;
    /**< Valid Rx port ID.

         @values Refer to @xhyperref{Q7,[Q7]} */

    uint8_t   decoder_mapping_vector[USM_MEDIA_FMT_MAX_SUPPORTED_OUTPUT_CHANNELS];
    /**< Channel array of size 8. Channel[i] mapping describes channel I. Each
         element i of the array describes channel I inside the buffer where 0
         @le I < num_channels. An unused channel is set to zero.

         @values See Section @xref{hdr:PcmChannelDefs} */

    uint16_t            samples_per_speaker;
     /**< Number of samples per speaker.
    
          @values > 0 */

    uint16_t            grouping_factor;
    /**< Number of packets to group together. For more details, see Section
         @xref{sec:FrameDilution}.
    
         @values
         - 0 -- Invalid
         - 1 -- Send each packet separately
         - > 1 -- Packets are grouped together; the value is the number of
                  grouped packets @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_usm_free_form_dec_cfg_v2_t */



/** @ingroup usm_media_format_epos
    Media format ID for EPOS stylus positioning algorithms.

    @enc_cfg_blk{usm_epos_stylus_enc_cfg_t}
    @table{weak__usm__epos__stylus__enc__cfg__t}
*/
#define USM_MEDIA_FMT_EPOS_STYLUS                0x00012310

/* PCM encoder configuration structure */
typedef struct usm_epos_stylus_enc_cfg_t usm_epos_stylus_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_usm_epos_stylus_enc_cfg_t
@{ */
/* EPOS stylus encoder configuration block used by USM_MEDIA_FMT_EPOS_STYLUS.
*/
struct usm_epos_stylus_enc_cfg_t
{
    uint16_t            num_channels;
    /**< Number of microphones (channels) required for the EPOS algorithms.

         @values 3 */

    uint16_t            bits_per_sample;
    /**< Number of bits per sample per channel.

         @values 16, 32 */

    uint32_t            sample_rate;
    /**< Number of samples per second.

         @values 96000, 192000 Hz */

    uint32_t            port_id;
    /**< Valid Tx port ID.

         @values Refer to @xhyperref{Q7,[Q7]} */

    uint32_t            encoder_mapping_vector;
    /**< Order in which to arrange the output stream.
    
         @values
         - Bits 0 to 7 -- First channel
         - Bits 8 to 15 -- Second channel
         - Bits 16 to 23 -- Third channel
         - Bits 24 to 31 -- 0 (currently not applicable)

         For more details, see @xhyperref{fig:EncMapVec,Encoder mapping
         vector}. */

    uint16_t            skip_factor;
    /**< Factor between sent and skipped frames. For more details, see Section
         @xref{sec:FrameDilution}.
    
         @values
         - 0 -- No skipping
         - > 0 -- Skipping factor @tablebulletend */

    uint16_t            grouping_factor;
    /**< Number of packets to group together. For more details, see Section
         @xref{sec:FrameDilution}.
    
         @values
         - 0 -- Invalid
         - 1 -- Send each packet separately
         - > 1 -- Packets are grouped together; the value is the number of
                  grouped packets @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup usm_epos_stylus_enc_cfg_t */


/** @ingroup usm_media_format_epos
    Media format ID for EPOS stylus positioning algorithms.

    @enc_cfg_blk{usm_epos_stylus_enc_cfg_v2_t}
    @table{weak__usm__epos__stylus__enc__cfg__v2__t}
*/
#define USM_MEDIA_FMT_EPOS_STYLUS_V2                0x0001272D

/* PCM encoder configuration structure */
typedef struct usm_epos_stylus_enc_cfg_v2_t usm_epos_stylus_enc_cfg_v2_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_usm_epos_stylus_enc_cfg_v2_t
@{ */
/* EPOS stylus encoder configuration block used by USM_MEDIA_FMT_EPOS_STYLUS_V2.
*/
struct usm_epos_stylus_enc_cfg_v2_t
{
    uint16_t            num_channels;
    /**< Number of microphones (channels) required for the EPOS algorithms.

         @values 3 */

    uint16_t            bits_per_sample;
    /**< Number of bits per sample per channel.

         @values 16, 32 */

    uint32_t            sample_rate;
    /**< Number of samples per second.

         @values 96000, 192000 Hz */

    uint32_t            port_id;
    /**< Valid Tx port ID.

         @values Refer to @xhyperref{Q7,[Q7]} */

    uint8_t              encoder_mapping_vector[USM_MEDIA_FMT_MAX_SUPPORTED_INPUT_CHANNELS];
    /**< Channel array of size 8. Channel[i] mapping describes channel I. Each
         element i of the array describes channel I inside the buffer where 0
         @le I < num_channels. An unused channel is set to zero.

         @values See Section @xref{hdr:PcmChannelDefs} */

    uint16_t            skip_factor;
    /**< Factor between sent and skipped frames. For more details, see Section
         @xref{sec:FrameDilution}.
    
         @values
         - 0 -- No skipping
         - > 0 -- Skipping factor @tablebulletend */

    uint16_t            grouping_factor;
    /**< Number of packets to group together. For more details, see Section
         @xref{sec:FrameDilution}.
    
         @values
         - 0 -- Invalid
         - 1 -- Send each packet separately
         - > 1 -- Packets are grouped together; the value is the number of
                  grouped packets @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup usm_epos_stylus_enc_cfg_v2_t */


/** @addtogroup usm_strm_media_format_proximity_detection
@{ */

/** @xreflabel{hdr:UsmMediaFmtProximityDetection}
    Media format for ultrasound proximity detection algorithms

    @parspace Proximity detection encoder/decoder configuration block
      @structure{usm_proximity_detection_enc_dec_cfg_t}
      @tablespace
      @inputtable{USM_MEDIA_FMT_PROXIMITY_DETECTION_Encoder_Decoder_config_block.tex}

    @newpage
*/
#define USM_MEDIA_FMT_PROXIMITY_DETECTION                0x00012721


typedef struct usm_proximity_detection_enc_cfg_t usm_proximity_detection_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @brief Proximity detection encode configuration
    block used by the #USM_MEDIA_FMT_PROXIMITY_DETECTION media format.
*/
struct usm_proximity_detection_enc_cfg_t
{
    uint16_t            num_channels;
    /**< Number of microphones (channels) required for algorithms. */

    uint16_t            bits_per_sample;
    /**< Number of bits per sample per channel. 
     
         Supported values: 16, 32 */

    uint32_t            sample_rate;
    /**< Number of samples per second. */
     
    uint32_t            tx_port_id;
    /**< Valid Tx port ID. */

    uint32_t           channel_mapping;
    /**< Order in which to arrange the output stream.   
        Supported values:
        0 – Currently supporting only one channel */


    uint32_t           residual_backoff;    
    /**< Tx frame start residual backoff.
         Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

    uint32_t           hpf_backoff_samples;
    /**<  HPF backoff 
         Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

    uint32_t           notch_check_len_samples;
    /**< Notch detection algorithm duration 
         Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

    uint32_t           notch_backoff_samples;
    /**< Notch Filter backoff
         Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

    uint32_t           direct_path_max_index;
    /**< Direct path last index 
         Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

    uint32_t           indirect_path_max_index;
    /**< Indirect paths last index 
             Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

    uint32_t           direct_threshold;
    /**< Direct path energy detection threshold
         Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

    uint32_t           direct_2_indirect_ratio;
    /**< Direct path energy to Indirect paths energy ratio - Proximity Detection Threshold
         Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

    uint32_t           fine_tuning;
    /**< Fine Tuning 
        Supported values:
            - Bit  0  -- Double frame proximity decision (TRUE/FALSE)*/

    uint32_t           calibration;
    /**< Calibration 
        Supported values:
            - Bits  0 to 15 -- Round trip delay compensation ( < 0x0FFFF )
            - Bits 16 to 31 -- Currently not applicable */

    uint32_t           mic_speaker_diff_mm;
    /**< Distance in mm between microphone and speaker*/

    uint32_t           output_type;
    /** < Output type */
}
#include "adsp_end_pack.h"
;

typedef struct usm_proximity_detection_dec_cfg_t usm_proximity_detection_dec_cfg_t;

#include "adsp_begin_pack.h"

/** @brief Proximity detection decoder configuration
    block used by the #USM_MEDIA_FMT_PROXIMITY_DETECTION media
    format.
*/
struct usm_proximity_detection_dec_cfg_t
{
    uint16_t            num_speakers;
    /**< Number of speakers (channels) required for the Proximity algorithms.
         Supported values: 1 */

    uint16_t            bits_per_sample;
    /**< Number of bits per sample per channel.

         Supported values: 16, 32 */
    uint32_t            sample_rate;
    /**< Number of samples per second. */

/**< Number of samples per second. 
         Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

   uint32_t            rx_port_id;
    /**< Valid Rx port ID. 
         Supported values: Refer to @xhyperref{Q7,[Q7]}
         @newpage */
    /* Q7=Hexagon Multimedia: AFE API Interface Specification (80-N3013-1)*/

   uint32_t           speaker_index;
    /**< Order in which to arrange the input stream.
         Supported values:
         - 0 -- DMA port 0
         - 1 -- DMA port 1 */

   uint32_t           rx_pattern_length_log2;
   /**< Log2 RX pastern length
         Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

   uint32_t           frequency_start_hz;
   /**< RX start frequency in Hz*/

   uint32_t           bandwith_hz;
   /**< Rx bandwidth in Hz */

   uint32_t           samples_per_speaker; 
   /**< Number of samples per frame*/

}
#include "adsp_end_pack.h"
;



/** @addtogroup usm_media_format_proximity_detection
@{ */

/** @xreflabel{hdr:UsmMediaFmtProximityDetection}
    Media format for ultrasound proximity detection algorithms

    @parspace Proximity detection encoder/decoder configuration block
      @structure{usm_proximity_detection_enc_dec_cfg_v2_t}
      @tablespace
      @inputtable{USM_MEDIA_FMT_PROXIMITY_DETECTION_Encoder_Decoder_config_block.tex}

    @newpage
*/

#define USM_MEDIA_FMT_PROXIMITY_DETECTION_V2                0x0001272B

typedef struct usm_proximity_detection_enc_cfg_v2_t usm_proximity_detection_enc_cfg_v2_t;

#include "adsp_begin_pack.h"

/** @brief Proximity detection encode configuration
    block used by the #USM_MEDIA_FMT_PROXIMITY_DETECTION_V2 media format.
*/
struct usm_proximity_detection_enc_cfg_v2_t
{
    uint16_t            num_channels;
    /**< Number of microphones (channels) required for algorithms.

         Supported values: 1 */

    uint16_t            bits_per_sample;
    /**< Number of bits per sample per channel. 
     
         Supported values: 16, 32 */

    uint32_t            sample_rate;
    /**< Number of samples per second. */
     
    uint32_t            tx_port_id;
    /**< Valid Tx port ID. */

    uint32_t           channel_mapping;
    /**< Order in which to arrange the output stream.   
        Supported values:
        0 – Currently supporting only one channel */

     
    int32_t           round_trip_delay_compensation;    
    /**< Round trip delay compensation.
         Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */

    uint32_t           residual_backoff;    
    /**< Tx frame start residual backoff.
         Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

    uint32_t           hpf_backoff_samples;
    /**<  HPF backoff 
         Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

    uint32_t           jammer_check_len_log2; 
    /**< (2^jammer_check_len_log2) is duration in samples of the interval used for jammer detection
         Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

    uint32_t           jammer_skip_frames;
    /**< Number of frames jammer detection algorithm skips to estimate jammer frequency 
         Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/


    uint32_t           jammer_frequency_extra_hz; 
    /**< Extended frequency  band, requred in case of FFT jammer detection.
         Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

    uint32_t           notch_backoff_samples;
    /**< Notch Filter backoff
         Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

    uint32_t           notch_q;
    /**< (notch_q / 128) is notch filter “quality factor” that defines notch filter stop band bandwidth (narrowness)
         Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

    uint32_t           direct_path_max_index;
    /**< Direct path last index 
         Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

    uint32_t           indirect_path_min_index;
    /**< Indirect paths first index 
             Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

    uint32_t           indirect_path_max_index;
    /**< Indirect paths last index 
             Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

    uint32_t           direct_threshold;
    /**< Direct path energy detection threshold
         Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

    uint32_t           direct_2_indirect_ratio;
    /**< Direct path energy to Indirect paths energy ratio - Proximity Detection Threshold
         Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/


    uint32_t           motion_direct_path_max_index;
    /**< Motion Detection direct paths last index 
             Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

    uint32_t           motion_indirect_path_min_index;
    /**< Motion Detection indirect paths first index 
             Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

    uint32_t           motion_indirect_path_max_index;
    /**< Motion Detection Indirect paths last index 
             Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

    uint32_t           motion_direct_2_indirect_ratio;
    /**< Motion Detection Direct to Indirect paths energy ratio
         Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

    uint32_t           fine_tuning;
    /**< Fine Tuning 
             Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

    uint32_t           calibration;
    /**< Calibration 
             Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

    uint32_t           mic_speaker_diff_mm;
    /**< Distance in mm between microphone and speaker*/

    uint32_t           output_type;
    /** < Output type */
}
#include "adsp_end_pack.h"
;

typedef struct usm_proximity_detection_dec_cfg_v2_t usm_proximity_detection_dec_cfg_v2_t;

#include "adsp_begin_pack.h"

/** @brief Proximity detection decoder configuration
    block used by the #USM_MEDIA_FMT_PROXIMITY_DETECTION_V2
    media format.
*/
struct usm_proximity_detection_dec_cfg_v2_t
{
    uint16_t            num_speakers;
    /**< Number of speakers (channels) required for the Proximity algorithms.
         Supported values: 1 */

    uint16_t            bits_per_sample;
    /**< Number of bits per sample per channel.

         Supported values: 16, 32 */
    uint32_t            sample_rate;
/**< Number of samples per second. 
         Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

   uint32_t            rx_port_id;
    /**< Valid Rx port ID. 
         Supported values: Refer to @xhyperref{Q7,[Q7]}
         @newpage */
    /* Q7=Hexagon Multimedia: AFE API Interface Specification (80-N3013-1)*/

   uint32_t           speaker_index;
    /**< Order in which to arrange the input stream.
         Supported values:
         - 0 -- DMA port 0
         - 1 -- DMA port 1 */

   uint32_t           general_config;
   /**< General Config 
       Supported values:
           - Bits  0 to 3 -- Algorithm type ( 0 - Chirp, 1 - Correlation )
           - Bit   4      -- Double frame proximity decision 
           - Bit   5      -- Input HPF bypass 
           - Bit   6      -- Jammer detection FFT algorithm enable 
           - Bit   7      -- RX HPF bypass 
           - Bit   8      -- Double notch filter enable */

   uint32_t           rx_pattern_length_log2;
   /**< Log2 RX pastern length
         Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

   uint32_t           frequency_start_hz;
   /**< RX start frequency in Hz*/

   uint32_t           bandwith_hz;
   /**< Rx bandwidth in Hz */

   uint32_t           gauss_quad_m;
   /**< Number of Gauss sequence in case of correlation algorithm
        is used instead of chirp */

   uint32_t           samples_per_speaker; 
   /**< Number of samples per frame*/

}
#include "adsp_end_pack.h"
;


/** @addtogroup usm_media_format_proximity_detection_v3
@{ */

/** @xreflabel{hdr:UsmMediaFmtProximityDetection}
    Media format for ultrasound proximity detection algorithms

    @parspace Proximity detection encoder/decoder configuration block
      @structure{usm_proximity_detection_enc_dec_cfg_v3_t}
      @tablespace
      @inputtable{USM_MEDIA_FMT_PROXIMITY_DETECTION_Encoder_Decoder_config_block.tex}

    @newpage
*/

#define USM_MEDIA_FMT_PROXIMITY_DETECTION_V3                0x0001272E

typedef struct usm_proximity_detection_enc_cfg_v3_t usm_proximity_detection_enc_cfg_v3_t;

#include "adsp_begin_pack.h"

/** @brief Proximity detection encode configuration
    block used by the #USM_MEDIA_FMT_PROXIMITY_DETECTION_V3
    media format.
*/
struct usm_proximity_detection_enc_cfg_v3_t
{
    uint16_t            num_channels;
    /**< Number of microphones (channels) required for algorithms.

         Supported values: 1 */

    uint16_t            bits_per_sample;
    /**< Number of bits per sample per channel. 
     
         Supported values: 16, 32 */

    uint32_t            sample_rate;
    /**< Number of samples per second. */
     
    uint32_t            tx_port_id;
    /**< Valid Tx port ID. */

    uint8_t   channel_mapping[USM_MEDIA_FMT_MAX_SUPPORTED_INPUT_CHANNELS];
    /**< Channel array of size 8. Channel[i] mapping describes channel I. Each
         element i of the array describes channel I inside the buffer where 0
         @le I < num_channels. An unused channel is set to zero.

         @values See Section @xref{hdr:PcmChannelDefs} */

    int32_t           round_trip_delay_compensation;    
    /**< Round trip delay compensation.
         Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */

    uint32_t           residual_backoff;    
    /**< Tx frame start residual backoff.
         Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

    uint32_t           hpf_backoff_samples;
    /**<  HPF backoff 
         Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

    uint32_t           jammer_check_len_log2; 
    /**< (2^jammer_check_len_log2) is duration in samples of the interval used for jammer detection
         Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

    uint32_t           jammer_skip_frames;
    /**< Number of frames jammer detection algorithm skips to estimate jammer frequency 
         Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/


    uint32_t           jammer_frequency_extra_hz; 
    /**< Extended frequency  band, requred in case of FFT jammer detection.
         Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

    uint32_t           notch_backoff_samples;
    /**< Notch Filter backoff
         Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

    uint32_t           notch_q;
    /**< (notch_q / 128) is notch filter “quality factor” that defines notch filter stop band bandwidth (narrowness)
         Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

    uint32_t           direct_path_max_index;
    /**< Direct path last index 
         Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

    uint32_t           indirect_path_min_index;
    /**< Indirect paths first index 
             Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

    uint32_t           indirect_path_max_index;
    /**< Indirect paths last index 
             Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

    uint32_t           direct_threshold;
    /**< Direct path energy detection threshold
         Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

    uint32_t           direct_2_indirect_ratio;
    /**< Direct path energy to Indirect paths energy ratio - Proximity Detection Threshold
         Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/


    uint32_t           motion_direct_path_max_index;
    /**< Motion Detection direct paths last index 
             Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

    uint32_t           motion_indirect_path_min_index;
    /**< Motion Detection indirect paths first index 
             Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

    uint32_t           motion_indirect_path_max_index;
    /**< Motion Detection Indirect paths last index 
             Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

    uint32_t           motion_direct_2_indirect_ratio;
    /**< Motion Detection Direct to Indirect paths energy ratio
         Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

    uint32_t           fine_tuning;
    /**< Fine Tuning 
             Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

    uint32_t           calibration;
    /**< Calibration 
             Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

    uint32_t           mic_speaker_diff_mm;
    /**< Distance in mm between microphone and speaker*/

    uint32_t           output_type;
    /** < Output type */
}
#include "adsp_end_pack.h"
;

typedef struct usm_proximity_detection_dec_cfg_v3_t usm_proximity_detection_dec_cfg_v3_t;

#include "adsp_begin_pack.h"

/** @brief Proximity detection decoder configuration
    block used by the #USM_MEDIA_FMT_PROXIMITY_DETECTION_V3
    media format.
*/
struct usm_proximity_detection_dec_cfg_v3_t
{
    uint16_t            num_speakers;
    /**< Number of speakers (channels) required for the Proximity algorithms.
         Supported values: 1 */

    uint16_t            bits_per_sample;
    /**< Number of bits per sample per channel.

         Supported values: 16, 32 */
    uint32_t            sample_rate;
    /**< Number of samples per second. 
         Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

   uint32_t            rx_port_id;
    /**< Valid Rx port ID. 
         Supported values: Refer to @xhyperref{Q7,[Q7]}
         @newpage */
    /* Q7=Hexagon Multimedia: AFE API Interface Specification (80-N3013-1)*/

   uint8_t   speaker_index[USM_MEDIA_FMT_MAX_SUPPORTED_OUTPUT_CHANNELS];
   /**< Channel array of size 8. Channel[i] mapping describes channel I. Each
        element i of the array describes channel I inside the buffer where 0
        @le I < num_channels. An unused channel is set to zero.

        @values See Section @xref{hdr:PcmChannelDefs} */

   uint32_t           general_config;
   /**< General Config 
       Supported values:
           - Bits  0 to 3 -- Algorithm type ( 0 - Chirp, 1 - Correlation )
           - Bit   4      -- Double frame proximity decision 
           - Bit   5      -- Input HPF bypass 
           - Bit   6      -- Jammer detection FFT algorithm enable 
           - Bit   7      -- RX HPF bypass 
           - Bit   8      -- Double notch filter enable */

   uint32_t           rx_pattern_length_log2;
   /**< Log2 RX pattern length
         Supported values: Refer to @xhyperref{Q8,[Q8]}
         @newpage */
    /* Q8=Hexagon Multimedia: Ultrasound FW API Interface Spec (80-N7466-1)*/

   uint32_t           frequency_start_hz;
   /**< RX start frequency in Hz*/

   uint32_t           bandwith_hz;
   /**< Rx bandwidth in Hz */

   uint32_t           gauss_quad_m;
   /**< Number of Gauss sequence in case of correlation algorithm
        is used instead of chirp */

   uint32_t           samples_per_speaker; 
   /**< Number of samples per frame*/

}
#include "adsp_end_pack.h"
;

/** @ingroup usm_media_format_sync_free_form
   Free form format with TX-RX synchronization support.
   Some Ultrasound use-cases (like gesture) need the TX stream
   synchrnoized with the RX stream. In such use cases typically
   a pattern is transmitted repeatedly over the RX, and then the
   pattern is received over the TX channel. Assuming ideal
   environment conditions (no noise, blocking objects), the
   algorithm wants the RX pattern to arrive in the beginning of
   each TX frame. This format is similar to the free form
   format(#USM_MEDIA_FMT_FREE_FORM_V2), but the ADSP ensures TX
   frames are synchronized with the RX stream. The
   synchronization is partially based on latency calculations
   (using RX-reference channel in the ADSP and timestamp
   adjustments) and partially on form factor specifications such
   as distance between microphone and speaker. The ADSP adjusts
   the TX stream and delivers synchrnoized frames to application
   processor.
   In order to use the sync free form data format, you need to
   open 2 streams, TX and RX both with the
   #USM_MEDIA_FMT_SYNC_FREE_FORM media format. The
   implementation will automatically link the 2 streams together
   and handle them with a single processor service, that will
   also do the TX-RX synchronization.
   

  @enc_cfg_blk{usm_sync_free_form_enc_cfg_t}
    @table{weak__usm__sync__free__form__enc__cfg__t}

  @par Encoder mapping vector
    The encoder mapping vector is used to inform the encoder in what order
    to arrange the output stream. Two parameters influence this mapping:
    @par
    - The channel order in which the data is received from the codec (up to 
      8 channels are received).
    - The channel order in which the upper layers expect to receive the
      data.
    @par
    The mapping vector describes how to create the output from the input.
    @par
    Figure @xref{fig:EncMapVec} illustrates an example of how to
    create the output from the input.
    @inputfigcap{1,encoder_mapping_vector.png,Encoder mapping vector,EncMapVec}

  @dec_cfg_blk{usm_sync_free_form_dec_cfg_t}
    @table{weak__usm__sync__free__form__dec__cfg__t}
*/
#define USM_MEDIA_FMT_SYNC_FREE_FORM                0x0001272F

/* PCM encoder configuration structure */
typedef struct usm_sync_free_form_enc_cfg_t usm_sync_free_form_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_usm_sync_free_form_enc_cfg_t
@{ */
/* Free-form with TX-RX synchronization encoder configuration
    block used by USM_MEDIA_FMT_SYNC_FREE_FORM.
*/
struct usm_sync_free_form_enc_cfg_t
{
    uint16_t            num_channels;
    /**< Number of microphones (channels) required for the Ultrasound algorithm.

         @values 1-4 */

    uint16_t            bits_per_sample;
    /**< Number of bits per sample per channel.

         @values 16, 32 */

    uint32_t            sample_rate;
    /**< Number of samples per second.

         @values 96000, 192000 Hz */

    uint32_t            port_id;
    /**< Valid Tx AFE port ID.

         @values Refer to @xhyperref{Q7,[Q7]} */
    /* Q7=Hexagon Multimedia: AFE API Interface Specification (80-N3013-1)*/

    uint8_t   encoder_mapping_vector[USM_MEDIA_FMT_MAX_SUPPORTED_INPUT_CHANNELS];
    /**< Channel array of size USM_MEDIA_FMT_MAX_SUPPORTED_INPUT_CHANNELS(currently 8) 
         Channel[i] mapping describes channel I. Each
         element i of the array describes channel I inside the buffer where 0
         @le I < num_channels. An unused channel is set to zero.

         @values See Section @xref{hdr:PcmChannelDefs} */


    uint16_t            skip_factor;
    /**< Factor between sent and skipped frames. For more details, see Section
         @xref{sec:FrameDilution}.
    
         @values
         - 0 -- No skipping
         - > 0 -- Skipping factor @tablebulletend */

    uint16_t            grouping_factor;
    /**< Number of packets to group together. For more details, see Section
         @xref{sec:FrameDilution}.
    
         @values
         - 0 -- Invalid
         - 1 -- Send each packet separately
         - > 1 -- Packets are grouped together; the value is the number of
                  grouped packets @tablebulletend */

     uint32_t           samples_per_frame;
     /**< Number of samples per frame.
    
          @values > 0 */

     int32_t            sync_delay_compensation;
     /**< A constant amount of samples that will be adjusted in each TX channel to sync with 
       the RX channel. This is added to other dynamically calculated values, to compensate
       for platform-specific issues */

     uint32_t           mic_speaker_distance[USM_MEDIA_FMT_MAX_SUPPORTED_INPUT_CHANNELS];
     /**< Distance in mm between speaker and microphone, for each of the input channels. 
       This value affects the TX-RX synchronization for each microphone.
       @values >= 0 */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_usm_sync_free_form_enc_cfg_t */

/* PCM decoder configuration structure */
typedef struct usm_sync_free_form_dec_cfg_t usm_sync_free_form_dec_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_usm_sync_free_form_dec_cfg_t
@{ */
/* Free-form with TX-RX synchronization decoder configuration
    block used by USM_MEDIA_FMT_SYNC_FREE_FORM.
*/
struct usm_sync_free_form_dec_cfg_t
{
    uint16_t            num_speakers;
    /**< Number of speakers (channels) required for the gesture recognition
         algorithms.

         @values 1, 2 */

    uint16_t            bits_per_sample;
    /**< Number of bits per sample per channel.

         @values 16, 32 */

    uint32_t            sample_rate;
    /**< Number of samples per second.

         @values 96000, 192000 Hz */

    uint32_t            port_id;
    /**< Valid Rx AFE port ID.

         @values Refer to @xhyperref{Q7,[Q7]} */

    uint8_t   decoder_mapping_vector[USM_MEDIA_FMT_MAX_SUPPORTED_OUTPUT_CHANNELS];
    /**< Channel array of size USM_MEDIA_FMT_MAX_SUPPORTED_OUTPUT_CHANNELS(currently 8). 
         Channel[i] mapping describes channel I. Each
         element i of the array describes channel I inside the buffer where 0
         @le I < num_channels. An unused channel is set to zero.

         @values See Section @xref{hdr:PcmChannelDefs} */

    uint16_t            samples_per_speaker;
     /**< Number of samples per speaker.
    
          @values > 0 */

    uint16_t            grouping_factor;
    /**< Number of packets to group together. For more details, see Section
         @xref{sec:FrameDilution}.
    
         @values
         - 0 -- Invalid
         - 1 -- Send each packet separately
         - > 1 -- Packets are grouped together; the value is the number of
                  grouped packets @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_usm_sync_free_form_dec_cfg_t */

/** @ingroup usm_media_format_sync_gesture 
   Gesture detection algorithm based on synchrnoized TX-RX streams.
   This format is based on the sync free form format (#USM_MEDIA_FMT_SYNC_FREE_FORM)
   and runs a gesture detection algorithm on the TX stream.
   The media format supports the following output modes, based on the output type
   parameter:
   - events mode - Runs gesture detection algorithm on TX data and output gesture
     detection results to application processor.
   - raw data mode - Audio frames are passed to application processor. This is
     the same behavior as sync free form media format.
   - both mode - Runs gesture detection algorithm on TX data and output both
     gesture detection results and TX data to application processor. Useful
     for reference testing.

  @enc_cfg_blk{usm_sync_free_form_enc_cfg_t}
    @table{weak__usm__sync__free__form__enc__cfg__t}

  @par Encoder mapping vector
    The encoder mapping vector is used to inform the encoder in what order
    to arrange the output stream. Two parameters influence this mapping:
    @par
    - The channel order in which the data is received from the codec (up to 
      8 channels are received).
    - The channel order in which the upper layers expect to receive the
      data.
    @par
    The mapping vector describes how to create the output from the input.
    @par
    Figure @xref{fig:EncMapVec} illustrates an example of how to
    create the output from the input.
    @inputfigcap{1,encoder_mapping_vector.png,Encoder mapping vector,EncMapVec}

  @dec_cfg_blk{usm_sync_free_form_dec_cfg_t}
    @table{weak__usm__sync__free__form__dec__cfg__t}
*/
#define USM_MEDIA_FMT_SYNC_GESTURE                0x00012730

/* PCM encoder configuration structure */
typedef struct usm_sync_gesture_enc_cfg_t usm_sync_gesture_enc_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_usm_sync_gesture_enc_cfg_t
@{ */
/* Sync gesture encoder configuration
    block used by USM_MEDIA_FMT_SYNC_GESTURE.
*/
struct usm_sync_gesture_enc_cfg_t
{
    uint16_t            num_channels;
    /**< Number of microphones (channels) required for the Ultrasound algorithm.

         @values 1-4 */

    uint16_t            bits_per_sample;
    /**< Number of bits per sample per channel.

         @values 16, 32 */

    uint32_t            sample_rate;
    /**< Number of samples per second.

         @values 96000, 192000 Hz */

    uint32_t            port_id;
    /**< Valid Tx AFE port ID.

         @values Refer to @xhyperref{Q7,[Q7]} */
    /* Q7=Hexagon Multimedia: AFE API Interface Specification (80-N3013-1)*/

    uint8_t   encoder_mapping_vector[USM_MEDIA_FMT_MAX_SUPPORTED_INPUT_CHANNELS];
    /**< Channel array of size USM_MEDIA_FMT_MAX_SUPPORTED_INPUT_CHANNELS(currently 8) 
         Channel[i] mapping describes channel I. Each
         element i of the array describes channel I inside the buffer where 0
         @le I < num_channels. An unused channel is set to zero.

         @values See Section @xref{hdr:PcmChannelDefs} */


    uint16_t            skip_factor;
    /**< Factor between sent and skipped frames. For more details, see Section
         @xref{sec:FrameDilution}.
    
         @values
         - 0 -- No skipping
         - > 0 -- Skipping factor @tablebulletend */

    uint16_t            grouping_factor;
    /**< Number of packets to group together. For more details, see Section
         @xref{sec:FrameDilution}.
    
         @values
         - 0 -- Invalid
         - 1 -- Send each packet separately
         - > 1 -- Packets are grouped together; the value is the number of
                  grouped packets @tablebulletend */

     uint32_t           samples_per_frame;
     /**< Number of samples per frame.
    
          @values > 0 */

     int32_t            sync_delay_compensation;
     /**< A constant amount of samples that will be adjusted in each TX channel to sync with 
       the RX channel. This is added to other dynamically calculated values, to compensate
       for platform-specific issues */

     uint32_t           mic_speaker_distance[USM_MEDIA_FMT_MAX_SUPPORTED_INPUT_CHANNELS];
     /**< Distance in mm between speaker and microphone, for each of the input channels. 
       This value affects the TX-RX synchronization for each microphone.
       @values >= 0 */

     uint32_t           output_type;
     /** < Output type. Can be one of:
      *  - USM_MEDIA_FMT_OUTPUT_TYPE_RAW_DATA
      *  - USM_MEDIA_FMT_OUTPUT_TYPE_GESTURE_EVENT
      *  - an OR of above 2 values for "both" mode */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_usm_sync_gesture_enc_cfg_t */

/* PCM decoder configuration structure */
typedef struct usm_sync_gesture_dec_cfg_t usm_sync_gesture_dec_cfg_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_usm_sync_gesture_dec_cfg_t
@{ */
/* Sync gesture decoder configuration
    block used by USM_MEDIA_FMT_SYNC_GESTURE.
*/
struct usm_sync_gesture_dec_cfg_t
{
    uint16_t            num_speakers;
    /**< Number of speakers (channels) required for the gesture recognition
         algorithms.

         @values 1, 2 */

    uint16_t            bits_per_sample;
    /**< Number of bits per sample per channel.

         @values 16, 32 */

    uint32_t            sample_rate;
    /**< Number of samples per second.

         @values 96000, 192000 Hz */

    uint32_t            port_id;
    /**< Valid Rx AFE port ID.

         @values Refer to @xhyperref{Q7,[Q7]} */

    uint8_t   decoder_mapping_vector[USM_MEDIA_FMT_MAX_SUPPORTED_OUTPUT_CHANNELS];
    /**< Channel array of size USM_MEDIA_FMT_MAX_SUPPORTED_OUTPUT_CHANNELS(currently 8). 
         Channel[i] mapping describes channel I. Each
         element i of the array describes channel I inside the buffer where 0
         @le I < num_channels. An unused channel is set to zero.

         @values See Section @xref{hdr:PcmChannelDefs} */

    uint16_t            samples_per_speaker;
     /**< Number of samples per speaker.
    
          @values > 0 */

    uint16_t            grouping_factor;
    /**< Number of packets to group together. For more details, see Section
         @xref{sec:FrameDilution}.
    
         @values
         - 0 -- Invalid
         - 1 -- Send each packet separately
         - > 1 -- Packets are grouped together; the value is the number of
                  grouped packets @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_usm_sync_gesture_dec_cfg_t */

/** @ingroup usm_media_format_proximity_v4 
   Proximity detection algorithm based on synchrnoized TX-RX streams.
   This format is based on the sync free form format (#USM_MEDIA_FMT_SYNC_FREE_FORM)
   and runs a proximity detection algorithm on the TX stream.
   The media format supports the following output modes, based on the output type
   parameter:
   - events mode - Runs proximity detection algorithm on TX data and output proximity
     detection results to application processor.
   - raw data mode - Audio frames are passed to application processor. This is
     the same behavior as sync free form media format.
   - both mode - Runs proximity detection algorithm on TX data and output both
     proximity detection results and TX data to application processor. Useful
     for reference testing.

  @enc_cfg_blk{usm_proximity_enc_cfg_v4_t}
    @table{weak__usm__proximity__enc__cfg__v4__t}

  @par Encoder mapping vector
    The encoder mapping vector is used to inform the encoder in what order
    to arrange the output stream. Two parameters influence this mapping:
    @par
    - The channel order in which the data is received from the codec (up to 
      8 channels are received).
    - The channel order in which the upper layers expect to receive the
      data.
    @par
    The mapping vector describes how to create the output from the input.
    @par
    Figure @xref{fig:EncMapVec} illustrates an example of how to
    create the output from the input.
    @inputfigcap{1,encoder_mapping_vector.png,Encoder mapping vector,EncMapVec}

  @dec_cfg_blk{usm_proximity_dec_cfg_v4_t}
    @table{weak__usm__proximity__dec__cfg__v4__t}
*/
#define USM_MEDIA_FMT_PROXIMITY_DETECTION_V4                0x0001273B

/* PCM encoder configuration structure */
typedef struct usm_sync_gesture_enc_cfg_v4_t usm_sync_gesture_enc_cfg_v4_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_usm_proximity_enc_cfg_v4_t
@{ */
/* Proximity encoder configuration
    block used by USM_MEDIA_FMT_PROXIMITY_DETECTION_V4.
*/
struct usm_proximity_enc_cfg_v4_t
{
    uint16_t            num_channels;
    /**< Number of microphones (channels) required for the Ultrasound algorithm.

         @values 1-4 */

    uint16_t            bits_per_sample;
    /**< Number of bits per sample per channel.

         @values 16, 32 */

    uint32_t            sample_rate;
    /**< Number of samples per second.

         @values 96000, 192000 Hz */

    uint32_t            port_id;
    /**< Valid Tx AFE port ID.

         @values Refer to @xhyperref{Q7,[Q7]} */
    /* Q7=Hexagon Multimedia: AFE API Interface Specification (80-N3013-1)*/

    uint8_t   encoder_mapping_vector[USM_MEDIA_FMT_MAX_SUPPORTED_INPUT_CHANNELS];
    /**< Channel array of size USM_MEDIA_FMT_MAX_SUPPORTED_INPUT_CHANNELS(currently 8) 
         Channel[i] mapping describes channel I. Each
         element i of the array describes channel I inside the buffer where 0
         @le I < num_channels. An unused channel is set to zero.

         @values See Section @xref{hdr:PcmChannelDefs} */


    uint16_t            skip_factor;
    /**< Factor between sent and skipped frames. For more details, see Section
         @xref{sec:FrameDilution}.
    
         @values
         - 0 -- No skipping
         - > 0 -- Skipping factor @tablebulletend */

    uint16_t            grouping_factor;
    /**< Number of packets to group together. For more details, see Section
         @xref{sec:FrameDilution}.
    
         @values
         - 0 -- Invalid
         - 1 -- Send each packet separately
         - > 1 -- Packets are grouped together; the value is the number of
                  grouped packets @tablebulletend */

     uint32_t           samples_per_frame;
     /**< Number of samples per frame.
    
          @values > 0 */

     int32_t            sync_delay_compensation;
     /**< A constant amount of samples that will be adjusted in each TX channel to sync with 
       the RX channel. This is added to other dynamically calculated values, to compensate
       for platform-specific issues */

     uint32_t           mic_speaker_distance[USM_MEDIA_FMT_MAX_SUPPORTED_INPUT_CHANNELS];
     /**< Distance in mm between speaker and microphone, for each of the input channels. 
       This value affects the TX-RX synchronization for each microphone.
       @values >= 0 */

     uint32_t           output_type;
     /** < Output type. Can be one of:
      *  - USM_MEDIA_FMT_OUTPUT_TYPE_RAW_DATA
      *  - USM_MEDIA_FMT_OUTPUT_TYPE_PROXIMITY_EVENT
      *  - an OR of above 2 values for "both" mode */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_usm_proximity_enc_cfg_v4_t */

/* PCM decoder configuration structure */
typedef struct usm_proximity_dec_cfg_v4_t usm_proximity_dec_cfg_v4_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_usm_proximity_dec_cfg_v4_t
@{ */
/* Proximity decoder configuration
    block used by USM_MEDIA_FMT_PROXIMITY_DETECTION_V4.
*/
struct usm_proximity_dec_cfg_v4_t
{
    uint16_t            num_speakers;
    /**< Number of speakers (channels) required for the proximity recognition
         algorithms.

         @values 1, 2 */

    uint16_t            bits_per_sample;
    /**< Number of bits per sample per channel.

         @values 16, 32 */

    uint32_t            sample_rate;
    /**< Number of samples per second.

         @values 96000, 192000 Hz */

    uint32_t            port_id;
    /**< Valid Rx AFE port ID.

         @values Refer to @xhyperref{Q7,[Q7]} */

    uint8_t   decoder_mapping_vector[USM_MEDIA_FMT_MAX_SUPPORTED_OUTPUT_CHANNELS];
    /**< Channel array of size USM_MEDIA_FMT_MAX_SUPPORTED_OUTPUT_CHANNELS(currently 8). 
         Channel[i] mapping describes channel I. Each
         element i of the array describes channel I inside the buffer where 0
         @le I < num_channels. An unused channel is set to zero.

         @values See Section @xref{hdr:PcmChannelDefs} */

    uint16_t            samples_per_speaker;
     /**< Number of samples per speaker.
    
          @values > 0 */

    uint16_t            grouping_factor;
    /**< Number of packets to group together. For more details, see Section
         @xref{sec:FrameDilution}.
    
         @values
         - 0 -- Invalid
         - 1 -- Send each packet separately
         - > 1 -- Packets are grouped together; the value is the number of
                  grouped packets @tablebulletend */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_usm_proximity_dec_cfg_v4_t */

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*_ADSP_ULSTRASOUND_MEDIA_FMT_H_*/
