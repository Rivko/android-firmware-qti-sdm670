#ifndef __CVD_LOG_I_H__
#define __CVD_LOG_I_H__

/**
  @file  cvd_log_i.h
  @brief This file contains private definitions of cvd log.
*/

/*
   Copyright (C) 2015 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/avs.adsp/2.8.5/vsd/inc/private/cvd_log_i.h#1 $
  $Author: pwbldsvc $
*/

/***************************************************************************
  Include files for Module                                                 *
****************************************************************************/

/* SYSTEM UTILITY APIs. */
#include "mmdefs.h"
#include "log.h"

/* APR APIs. */
#include "apr_comdef.h"


/** Log Codes Definitions supported by CVD log.
 *
 *  Log codes are allocated and maintained by DIAG team. Please refer to 
 *  <root>/adsp_proc/core/api/services/log_codes.h .
 */
#ifndef LOG_ADSP_CVD_STREAM_RX
  #define LOG_ADSP_CVD_STREAM_RX  ( ( 0x805 ) + LOG_1X_BASE_C )
#endif /* LOG_ADSP_CVD_STREAM_RX */

#ifndef LOG_ADSP_CVD_STREAM_TX
  #define LOG_ADSP_CVD_STREAM_TX  ( ( 0x804 ) + LOG_1X_BASE_C )
#endif /* LOG_ADSP_CVD_STREAM_TX */


/****************************************************************************
 * CVD LOG DEFINES                                                          *
 ****************************************************************************/

/** Container ID and Minor Version together identifies the overall structure
 * of cvd log voice container.
 */
#define CVD_LOG_VOICE_CONTAINER_ID ( 0x00012ED5 )
#define CVD_LOG_VOICE_CONTAINER_MINOR_VERSION_V ( 0x0001 )


/** Container ID and Minor Version together identifies the overall structure
 * of cvd log segment container.
 */
#define CVD_LOG_SEGMENT_HEADER_CONTAINER_ID ( 0x00012ECF )
#define CVD_LOG_SEGMENT_HEADER_CONTAINER_MINOR_VERSION_V ( 0x0000 )


/** Container ID and Minor Version together identifies the overall structure
 * of cvd log auxiliary information container.
 */
#define CVD_LOG_AUX_INFO_CONTAINER_ID ( 0x00012ED0 )
#define CVD_LOG_AUX_INFO_CONTAINER_MINOR_VERSION_V ( 0x0000 )

/** Minor Version identifies the overall structure
 * of cvd log data container.
 */
#define CVD_LOG_DATA_CONTAINER_MINOR_VERSION_V ( 0x0001 )

/** Tap Point identifies the origin of the vocoder packet.
 */
#define CVD_LOG_ENC_OUTPUT_TAP_POINT ( 0x00012ED1 )
#define CVD_LOG_DEC_INPUT_TAP_POINT ( 0x00012ED2 )

/** Frame header size coresponding to the frame being logged.
 * Currently all frame headers are of 1 byte. 
 */
#define CVD_LOG_FRAME_HEADER_SIZE_V ( 0x01 )

/*****************************************************************************
 * DEFINITIONS                                                               *
 ****************************************************************************/

/**
 * CVD LOG SEGMENT CONTAINER DEFINITION.
 */
typedef struct cvd_log_segment_header_t cvd_log_segment_header_t;

#include "apr_pack_begin.h"

struct cvd_log_segment_header_t
{
  uint16_t container_size;
  uint32_t container_id;
  uint16_t minor_version;
  uint32_t sequence_num;
  uint32_t total_size;
  uint32_t offset;
}
#include "apr_pack_end.h"
;

/**
 * CVD LOG AUXILIARY INFORMATION CONTAINER DEFINITION.
 */
typedef struct cvd_log_aux_info_t cvd_log_aux_info_t;

#include "apr_pack_begin.h"

struct cvd_log_aux_info_t
{
  uint16_t container_size;
  uint32_t container_id;
  uint16_t minor_version;
  uint32_t instance;
  uint32_t call_num;
  uint64_t timestamp;
  uint32_t tap_id;
}
#include "apr_pack_end.h"
;

/**
 * CVD LOG DATA CONTAINER DEFINITION.
 */
typedef struct cvd_log_raw_data_t cvd_log_raw_data_t;

#include "apr_pack_begin.h"

struct cvd_log_raw_data_t
{
  uint16_t container_size;
  uint32_t container_id;
  uint16_t minor_version;
  uint8_t frame_header;

#if __STDC_VERSION__ >= 199901L
  uint8_t payload[];
#endif /* __STDC_VERSION__ >= 199901L */
}
#include "apr_pack_end.h"
;

/**
 * CVD LOG VOICE CONTAINER DEFINITION.
 */ 
typedef struct cvd_log_voice_container_t cvd_log_voice_container_t;

#include "apr_pack_begin.h"

struct cvd_log_voice_container_t
{
  uint16_t cointainer_size;
  uint32_t container_id;
  uint16_t minor_version;

  cvd_log_segment_header_t segment_header;

  cvd_log_aux_info_t aux_info;

  cvd_log_raw_data_t data;
}
#include "apr_pack_end.h"
;

/**
 * CVD LOG VOICE PACKET DEFINITION.
 */
typedef struct cvd_log_voice_t cvd_log_voice_t;

#include "apr_pack_begin.h"

struct cvd_log_voice_t
{
  log_hdr_type hdr;
  cvd_log_voice_container_t voice;
}
#include "apr_pack_end.h"
;

/**
 * Logs the vocoder data along with the header.
 * 
 * @param[in] log_code.
 * @param[in] sequence_num.
 * @param[in] total_size.    Total size of the Payload with frame header.
 * @param[in] instance.
 * @param[in] call_num.
 * @param[in] timestamp.
 * @param[in] tap_id.
 * @param[in] media_id.
 * @param[in] data.          Reference to header + payload.
 *
 * @return APR_EOK when successful.
 */

APR_INTERNAL int32_t cvd_log_commit_data (
  log_code_type log_code,
  uint32_t sequence_num,
  uint32_t total_size,
  uint32_t instance,
  uint32_t call_num,
  uint64_t timestamp,
  uint32_t tap_id,
  uint32_t media_id,
  uint8_t* data
);

#endif /* __CVD_LOG_I_H__ */

