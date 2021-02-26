#ifndef __CVD_CAL_LOG_I_H__
#define __CVD_CAL_LOG_I_H__

/*
   Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/avs.adsp/2.8.5/vsd/inc/private/cvd_cal_log_i.h#1 $
  $Author: pwbldsvc $
*/

#include "log.h"
#include "apr_comdef.h"
#include "vss_public_if.h"
#include "vss_private_if.h"

/****************************************************************************
 * DEFINES                                                                  *
 ****************************************************************************/

/* Log code for logging the Per Vocoder Calibration Data
   This macro should be defined in log.h.
*/
#ifndef LOG_ADSP_CVD_CAL_DATA_C 
  #define LOG_ADSP_CVD_CAL_DATA_C ( 0x1FFE )
#endif

/* Log code for logging the VP3 Calibration Data.
   This macro should be defined in log.h.
*/
#ifndef LOG_ADSP_CVD_VP3_C 
  #define LOG_ADSP_CVD_VP3_C ( 0x1FFF )
#endif

/* Voice Container ID */
#define CVD_LOG_VOICE_CONTAINER_ID ( 0x00012ED5 )

/* Control Container ID */
#define CVD_LOG_SEGMENT_HEADER_CONTAINER_ID ( 0x00012ECF )

/* Auxillary Information Container ID */
#define CVD_LOG_AUX_INFO_CONTAINER_ID ( 0x00012ED0 )

/* Max. VP3 data length in bytes */
#define CVD_MAX_VP3_DATA_LEN ( 1024 )

/* CVD Calibration Logging Use Case IDs*/
#define CVD_CAL_LOG_STREAM_STATIC_TABLE ( 0x00013102 )
#define CVD_CAL_LOG_VOCPROC_STATIC_TABLE ( 0x00013103 )
#define CVD_CAL_LOG_VOCPROC_DYNAMIC_TABLE ( 0x00013104 )
#define CVD_CAL_LOG_DEVICE_CONFIG_TABLE ( 0x00013105 )
#define CVD_CAL_LOG_SYSTEM_CONFIG ( 0x00013106 )
#define CVD_CAL_LOG_RAW_CAL_DATA_OUTPUT ( 0x00013107 )
#define CVD_CAL_LOG_STREAM_STATIC_OUTPUT ( 0x00013130 )
#define CVD_CAL_LOG_VOCPROC_STATIC_OUTPUT ( 0x00013131 )
#define CVD_CAL_LOG_VOCPROC_DYNAMIC_OUTPUT ( 0x00013132 )
#define CVD_CAL_LOG_SOUNDFOCUS_ACTIVITY_DATA ( 0x00013137 )

#define CVD_CAL_LOG_DATA_CONTAINER_TABLE ( 0x00013108 )
#define CVD_CAL_LOG_DATA_CONTAINER_SYSTEM_CONFIG ( 0x00013109 )
#define CVD_CAL_LOG_DATA_CONTAINER_RAW_CAL_OUTPUT ( 0x0001310A )
#define CVD_CAL_LOG_DATA_CONTAINER_RAW_VP3 ( 0x000131D6 )

/* VP3 Calibration Logging Use Case IDs */
#define CVD_CAL_LOG_RAW_VP3_DATA_INPUT_TAP_POINT ( 0x00013152 )
#define CVD_CAL_LOG_RAW_VP3_DATA_OUTPUT_TAP_POINT ( 0x00013153 )


/*****************************************************************************
 * DEFINITIONS                                                               *
 ****************************************************************************/

typedef struct cvd_cal_log_segment_header_t cvd_cal_log_segment_header_t;

#include "apr_pack_begin.h"
struct cvd_cal_log_segment_header_t
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

typedef struct cvd_cal_log_aux_info_header_t cvd_cal_log_aux_info_header_t;

#include "apr_pack_begin.h"
struct cvd_cal_log_aux_info_header_t
{
  uint16_t container_size;
  uint32_t container_id;
  uint16_t minor_version;
  uint32_t instance;
  uint32_t call_num;
  uint64_t timestamp;
  uint32_t usecase_id;
}
#include "apr_pack_end.h"
;

typedef struct cvd_cal_log_data_container_t cvd_cal_log_data_container_t;

#include "apr_pack_begin.h"
struct cvd_cal_log_data_container_t
{
  uint16_t container_size;
  uint32_t container_id;
  uint16_t minor_version;

/* Payload contains the corresponding data container header
     followed by actual data. */
#if __STDC_VERSION__ >= 199901L
  uint8_t payload[];
#endif /* __STDC_VERSION__ >= 199901L */
}
#include "apr_pack_end.h"
;

typedef struct cvd_cal_log_cal_container_t cvd_cal_log_cal_container_t;

#include "apr_pack_begin.h"
struct cvd_cal_log_cal_container_t
{
  uint16_t cointainer_size;
  uint32_t container_id;
  uint16_t minor_version;

  cvd_cal_log_segment_header_t segment_header;

  cvd_cal_log_aux_info_header_t aux_info;

  cvd_cal_log_data_container_t data;
}
#include "apr_pack_end.h"
;

typedef struct cvd_cal_log_voice_t cvd_cal_log_voice_t;

#include "apr_pack_begin.h"
struct cvd_cal_log_voice_t
{
  log_hdr_type hdr;
  cvd_cal_log_cal_container_t cal_container;
}
#include "apr_pack_end.h"
;
/* Data Container Header structures.  */

typedef struct cvd_cal_log_table_header_t cvd_cal_log_table_header_t;

/* This structure is used with conatiner ID:
  CVD_CAL_LOG_DATA_CONTAINER_TABLE

  for logging the following cal table use cases:
  CVD_CAL_LOG_STREAM_STATIC_TABLE
  CVD_CAL_LOG_VOCPROC_STATIC_TABLE
  CVD_CAL_LOG_VOCPROC_DYNAMIC_TABLE
  CVD_CAL_LOG_DEVICE_CONFIG_TABLE
*/
struct cvd_cal_log_table_header_t
{
  uint32_t table_handle;
};


typedef struct cvd_cal_log_system_config_header_t cvd_cal_log_system_config_header_t;

/* This structure is used with container ID:
  CVD_CAL_LOG_DATA_CONTAINER_SYSTEM_CONFIG

  for logging the system config usecase
  CVD_CAL_LOG_SYSTEM_CONFIG
*/
struct cvd_cal_log_system_config_header_t
{
  uint32_t table_handle;
  uint32_t cal_query_handle;
};

typedef struct cvd_cal_log_cal_data_header_t cvd_cal_log_cal_data_header_t;

/* This structure is used with container ID:
  CVD_CAL_LOG_DATA_CONTAINER_RAW_CAL_OUTPUT

  for logging the system config usecase
  CVD_CAL_LOG_RAW_CAL_DATA_OUTPUT
*/
struct cvd_cal_log_cal_data_header_t
{
  uint32_t table_handle;
  uint32_t cal_query_handle;
  uint32_t data_seq_num;
};

typedef struct cvd_cal_log_vp3_data_header_t cvd_cal_log_vp3_data_header_t;

/* This structure is used with container ID:
  CVD_CAL_LOG_DATA_CONTAINER_RAW_VP3

  for logging the VP3 Data use-cases
  CVD_CAL_LOG_RAW_VP3_DATA_INPUT_TAP_POINT
  CVD_CAL_LOG_RAW_VP3_DATA_OUTPUT_TAP_POINT
*/
struct cvd_cal_log_vp3_data_header_t
{
  uint32_t direction;
    /**<
     * The direction of the sound device pair. The supported values:\n
     * CVP_SOUND_DEVICE_DIRECTION_RX \n
     * CVP_SOUND_DEVICE_DIRECTION_TX \n
     * CVP_SOUND_DEVICE_DIRECTION_RX_TX
     */
  uint32_t tx_device_id;
    /**<
     * Logical Tx sound device ID. This field is ignored if the sound device
     * direction is CVP_SOUND_DEVICE_DIRECTION_RX.
     */
  uint32_t rx_device_id;
    /**<
     * Logical Rx sound device ID. This field is ignored if the sound device
     * direction is CVP_SOUND_DEVICE_DIRECTION_TX.
     */
};

typedef struct cvd_cal_log_commit_info_t cvd_cal_log_commit_info_t;

struct cvd_cal_log_commit_info_t
{
  uint32_t instance;
  uint32_t call_num;
  uint32_t data_container_id;
  void* data_container_header;
  uint32_t data_container_header_size;
  void* payload_buf;
  uint32_t payload_size;
};

APR_INTERNAL void cvd_cal_log_data (
  log_code_type log_code,
  uint32_t usecase_id,
  void* params,
  uint32_t size
);

int32_t cvd_cal_log_init ( void );

int32_t cvd_cal_log_deinit ( void );

#endif
