/*========================================================================
                                 
                E l i t e  L o g g i n g  D e f i n i t i o n s

*/
/** @file EliteLoggingUtils.h
  @brief  This file contains the Elite logging utility definitions.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF. 
      The description that displays in the PDF is located in the
      Elite_mainpage.dox file. Contact Tech Pubs for assistance.
===========================================================================*/

/*========================================================================
Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*========================================================================
                             Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/utils/inc/EliteLoggingUtils.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
09/05/14   sw      (Tech Pubs) Updated Doxygen markup/comments for 2.6.
03/20/13   sw      (Tech Pubs) Updated Doxygen markup/comments for 2.0.

========================================================================== */

/* =======================================================================
                     INCLUDE FILES FOR MODULE
========================================================================== */

#ifndef _ELITE_LOGGING_UTILS_H_
#define _ELITE_LOGGING_UTILS_H_

#if defined(__qdsp6__)

/*==========================================================================
  Define constants
========================================================================== */

/** @addtogroup log_util_constants
@{ */

/* Log container tags */
/** Container tag for the log header. */
#define AUDIOLOG_CONTAINER_LOG_HEADER                    0x0001

/** Container tag for the user session. */
#define AUDIOLOG_CONTAINER_USER_SESSION                0x0002

/** Container tag for the PCM data format. */
#define AUDIOLOG_CONTAINER_PCM_DATA_FORMAT            0x0004

/** Container tag for the bit stream data format. */
#define AUDIOLOG_CONTAINER_BS_DATA_FORMAT                0x0005

/** Maximum number of channels supported for multichannel PCM logging. */
#define ELITE_MAX_NUM_CH            16

/** QXDM Pro log codes for audio/voice data path logging.
*/
typedef enum 
{
    QXDM_LOG_CODE_AUD_DEC_IN     = 0x152E,
    /**< Logs bit stream data/PCM stream at the audio decoder input. */

    QXDM_LOG_CODE_AUD_POPP_IN    = 0x152F,
    /**< Logs PCM data at the audio POPP input. */

    QXDM_LOG_CODE_AUD_COPP_IN    = 0x1531,
    /**< Logs PCM data at the audio COPP input. */

    QXDM_LOG_CODE_AUD_POPREP_IN  = 0x1534,
    /**< Logs PCM data at the audio POPREP input. */

    QXDM_LOG_CODE_AUD_COPREP_IN  = 0x1532,
    /**< Logs PCM data at the audio COPREP input. */

    QXDM_LOG_CODE_AUD_MTMX_RX_IN = 0x1530,
    /**< Logs PCM data at the audio matrix Rx input. */

    QXDM_LOG_CODE_AUD_MTMX_TX_IN = 0x1533,
    /**< Logs PCM data at the audio matrix Tx input. */

    QXDM_LOG_CODE_AUD_ENC_IN     = 0x1535,
    /**< Logs PCM data at the audio encoder input. */

    QXDM_LOG_CODE_AUD_ENC_OUT    = 0x1536,
    /**< Logs PCM data at the audio encoder output. */

    QXDM_LOG_CODE_AFE_RX_TX_OUT  = 0x1586,
    /**< Logs PCM data at the AFE Rx output and AFE Tx input. */

    QXDM_LOG_CODE_AFE_ALGO_CALIB  = 0x17D8,
    /**< Logs run-time parameters of various algorithms inside the AFE. */

	QXDM_LOG_CODE_LSM_OUTPUT  = 0x1882,
    /**< Logs PCM data at the LSM output. */

	QXDM_LOG_CODE_AUD_DEC_OUT = 0x19AF,
	/**< Logs PCM data at the audio Decoder output. */

	QXDM_LOG_CODE_AUD_COPP_OUT    = 0x19B0,
    /**< Logs PCM data at the audio COPP output. */

	QXDM_LOG_CODE_AUD_POPP_OUT    = 0x19B1
    /**< Logs PCM data at the audio POPP output. */

}elite_qxdm_log_code;


/** Log data formats.
*/
typedef enum
{
    ELITE_LOG_DATA_FMT_PCM = 0,       /**< PCM data format. */
    ELITE_LOG_DATA_FMT_BITSTREAM      /**< bit stream data format. */
}elite_log_data_format;

/** @} */ /* end_addtogroup log_util_constants */

#if !defined(SIM)
/* For PCM logging */
#include "log.h"
#include "diagpkt.h"
/*==========================================================================
  Structure definitions
  ========================================================================== */
/* Overall log packet structure 
  For PCM data, the containers are in following order:
         AUDIOLOG_HEADER
         AUDIOLOG_USER_SESSION
         AUDIOLOG_PCM_DATA_FORMAT
 
    For bitsream data,
         AUDIOLOG_HEADER
         AUDIOLOG_USER_SESSION
         AUDIOLOG_BITSTREAM_DATA_FORMAT
  */

/** @addtogroup log_util_pkt_structs
@{ */
/** Container for the user session.
*/
typedef PACK(struct) 
{
    uint16_t                  size;
    /**< Size information for this container. */

    uint16_t                  tag;
    /**< Tag information for this container. */

    uint32_t                            user_session_id;
    /**< User session ID. Currently not used; set this parameter to zero. */

    uint64_t                            time_stamp;
    /**< Timestamp (in microseconds) for the log packet. */

} audiolog_user_session;

 
/** Container for the data format of the audio log PCM.
*/
typedef PACK(struct) 
{
    uint16_t                      size;
    /**< Size information for this container. */

    uint16_t                      tag;
    /**< Tag information for this container. */

    uint32_t                            log_tap_id;
    /**< GUID for the tap point. */

    uint32_t                            sampling_rate;
    /**< PCM sampling rate (8000 Hz, 48000 Hz, etc.). */

    uint16_t                   num_channels;
    /**< Number of channels in the PCM stream. */

    uint8_t                            pcm_width;
    /**< Bits per sample for the PCM data. */

    uint8_t                            interleaved;
    /**< Specifies whether the data is interleaved. */

    uint8_t                             channel_mapping[ELITE_MAX_NUM_CH];
    /**< Array of channel mappings. @newpagetable */

} audiolog_pcm_data_format;


/** Container for the data format of the audio log bit stream.
*/
typedef PACK(struct)
{
    uint16_t                      size;
    /**< Size information for this container. */

    uint16_t                      tag;
    /**< Tag information for this container. */

    uint32_t                            log_tap_id;
    /**< GUID for the tap point. */

    uint32_t                            media_fmt_id;
    /**< Media format ID for the audio/voice encoder/decoder. */

} audiolog_bitstream_data_format;
 
 
/** Common header structure for PCM/bit stream data.
*/
typedef PACK(struct) 
{
    uint16_t                      size;
    /**< Size information for this container. */

    uint16_t                      tag;
    /**< Tag information for this container. */

    uint32_t                            log_session_id;
    /**< Log session ID. */

    uint32_t                            log_seg_number;
    /**< Log segment number. Currently not used; set this field to zero. */

    uint32_t                    segment_size;
    /**< Size in bytes of the payload, excluding the header. */

    uint32_t                            fragment_offset;
    /**< Fragment offset. Currently not used. */

    audiolog_user_session        user_session_info;
    /**< Audio log user session. */

}audiolog_header_cmn;


/** Audio log header for PCM data.
*/
typedef PACK(struct) 
{
    audiolog_header_cmn            cmn_struct;
    /**< Common header structure for PCM/bit stream data. */

    audiolog_pcm_data_format    pcm_data_fmt;
    /**< Audio log PCM data format. */
    
} audiolog_header_pcm_data;


/** Audio log header for bit stream data.
*/
typedef PACK(struct) 
{
    audiolog_header_cmn                    cmn_struct;
    /**< Common header structure for PCM/bit stream data. */

    audiolog_bitstream_data_format    bs_data_fmt;
    /**< Audio log bit stream data format. */
    
} audiolog_header_bitstream_data;


/** Complete log packet structure for logging PCM data.
*/
typedef PACK(struct)
{
    log_hdr_type                         hdr;
    /**< Length of the packet in bytes, including this header, the QXDM Pro log
         code, and system timestamp. Each log packet must begin with a 
         member of type log_hdr_type. */

    audiolog_header_pcm_data        log_header_pcm;
    /**< Audio log header for PCM data. */

    uint8                           payload[1];
    /**< Logged PCM data. */

}log_pkt_pcm_data;


/** Complete log packet structure for logging bit stream data.
*/
typedef PACK(struct)
{
    log_hdr_type                                     hdr;
    /**< Length of the packet in bytes including this header, the QXDM Pro log
         code, and system timestamp. Each log packet must begin with a 
         member of type log_hdr_type. */

    audiolog_header_bitstream_data            log_header_bs;
    /**< Audio log header for bit stream data. */

    uint8                                       payload[1];
    /**< Logged bit stream data. */

}log_pkt_bitstream_data;

/** @} */ /* end_addtogroup log_util_pkt_structs */

#endif // !defined(SIM)

#endif /* #if defined(__qdsp6__) */

#endif /* #ifndef _ELITE_LOGGING_UTILS_H_ */
