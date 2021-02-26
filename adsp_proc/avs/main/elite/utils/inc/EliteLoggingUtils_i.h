/*========================================================================
                                 
                E l i t e  L o g g i n g  D e f i n i t i o n s

*/
/** @file EliteLoggingUtils_i.h
  @brief  This file contains the Elite logging utility definitions.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF. 
      The description that displays in the PDF is located in the
      Elite_mainpage.dox file. Contact Tech Pubs for assistance.
===========================================================================*/

/*========================================================================
Copyright (c) 2013-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*========================================================================
                             Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/utils/inc/EliteLoggingUtils_i.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/25/15   sw      (Tech Pubs) General edits in Doxygen comments.
09/05/14   sw      (Tech Pubs) Merged Doxygen markup from 2.4.
03/20/13   sw      (Tech Pubs) Updated Doxygen markup/comments for 2.0.

========================================================================== */

/* =======================================================================
                     INCLUDE FILES FOR MODULE
========================================================================== */

#ifndef _ELITE_LOGGING_UTILS_I_H_
#define _ELITE_LOGGING_UTILS_I_H_

#if defined(__qdsp6__)

#include "qurt_elite.h"
#include "audio_basic_op.h"
#include "audio_basic_op_ext.h"
#include "EliteLoggingUtils.h"

/** @addtogroup log_util_constants
@{ */
/** Maximum size of the log packet. The maximum packet size that can be
  allocated is 6176 bytes.

  Sometimes, as per on-target logging, buffer allocation fails if this size
  is used. In these cases, set the log buffer size to one of the following
  values:

  @indent 4 x LCM(1, 2, 3, 4, 5, 6, 7, 8) = 3360 bytes

  The log buffer size must never be more than 6176 bytes per log packet.
*/
#define ELITE_MAX_LOG_PKT_SIZE     (3360)

/** Magic number for indicating the segmented packet. @newpage */
#define ELITE_SEG_PKT                (0xDEADBEEF)

/** @} */ /* end_addtogroup log_util_constants */

/*==========================================================================
  Structure definitions
  ========================================================================== */

/* Struct to be used by the log utility users */
typedef struct pcm_data_info pcm_data_info;

/** @addtogroup log_util_datatypes
@{ */
/** PCM data information for the logging utility user.
*/
struct pcm_data_info
{
    uint32_t        sampling_rate;
    /**< PCM sampling rate.

         @values 8000 Hz, 48000 Hz, etc. */

    uint16_t        num_channels;
    /**< Number of channels in the PCM stream. */

    uint8_t        bits_per_sample;
    /**< Bits per sample for the PCM data. */

    uint8_t        interleaved;
    /**< Specifies whether the data is interleaved. */

    uint8_t        *channel_mapping;
    /**< Array of channel mappings. */
};

typedef union log_data_info log_data_info;

/** Format of the data being logged: PCM or bit stream.
*/
union log_data_info
{
    pcm_data_info        pcm_data_fmt;  /**< Format of the PCM data. */
    uint32_t             media_fmt_id;  /**< Format of the bit stream data. */
};

typedef struct elite_log_info elite_log_info;

/** Log header and data payload information for the logging utility user.
*/
struct elite_log_info
{
    elite_qxdm_log_code         qxdm_log_code;
    /**< QXDM Professional\tm (QXDM Pro) log code for the log packet.
    
         @values See #elite_qxdm_log_code. */

    uint8_t                        *buf_ptr;
    /**< Pointer to the buffer to be logged. */

    uint32_t                        buf_size;
    /**< Size of the payload to be logged, in bytes. */

    uint32_t                        session_id;
    /**< Session ID for the log packet. */

    uint32_t                        log_tap_id;
    /**< GUID for the tap point. */

    uint64_t                        log_time_stamp;
    /**< Timestamp in microseconds. */

    elite_log_data_format    data_fmt;
    /**< Data format for the log packet. */

    log_data_info                data_info;
    /**< Pointer to the data packet information. */
};

/** @} */ /* end_addtogroup log_util_datatypes */

/*==========================================================================
  Function Declarations
  ========================================================================== */

/** @ingroup elitelog_func_alloc_buff
  Allocates a log packet for PCM/bit stream data logging.

  @datatypes
  #elite_qxdm_log_code \n
  #elite_log_data_format

  @param[in] buf_Size   Size of the data payload, excluding the log header.
  @param[in] log_code   Log code for this log packet.
  @param[in] data_fmt   PCM or bit stream data format.

  @return
  Pointer to the allocated log packet. Returns NULL if buffer allocation fails
  or log code is disabled.

  @dependencies
  None.
*/
void* log_alloc_buffer (uint32_t buf_Size, elite_qxdm_log_code log_code, 
                        elite_log_data_format data_fmt);


/** @ingroup elitelog_func_log_buff
  Populates the log header and data payload of an allocated log packet and
  commits to the DIAG service.

  @datatypes
  #elite_log_info

  @param[in] log_pkt_ptr   Pointer to the allocated log packet.
  @param[in] log_info_ptr  Pointer to the object containing the log header and
                           data payload information for the logging utility
                           user.

  @return
  ADSP_EOK -- Success.
  @par
  Error code -- Any failure.

  @dependencies
  None
*/
ADSPResult elite_log_buffer(void *log_pkt_ptr, elite_log_info  *log_info_ptr);

/** @ingroup elitelog_func_alloc_buf_log_data
  Allocates the log packet, populates the log header and data payload, and
  commits the packet to the DIAG task for logging.

  @datatypes
  #elite_log_info

  @param[in] log_info_ptr  Pointer to the object containing the log header and
                           data payload information for the logging utility
                           client.

  @return
  ADSP_EOK -- Success.
  @par
  Error code -- Any failure.

  @dependencies
  None
*/
ADSPResult elite_allocbuf_and_log_data_pkt(elite_log_info * log_info_ptr);

#if defined(SIM)

/*
  Reads the configuration file
  (aDSPTest/TestConfig/qdsp6/root/adsp_file_logging.cfg)
  and initializes the static data structure.

  @return
  ADSP_EOK -- Success.
  @par
  Error code -- Any failure.
  
  @dependencies
  None
*/
ADSPResult elite_sim_init_file_logging();

#endif


#endif /* #if defined(__qdsp6__) */

#endif /* #ifndef _ELITE_LOGGING_UTILS_I_H_ */

