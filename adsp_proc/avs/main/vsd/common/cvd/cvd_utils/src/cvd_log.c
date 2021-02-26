/*
   DESCRIPTION
   This file contains the defintions of cvd log.

   Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/avs.adsp/2.8.5/vsd/common/cvd/cvd_utils/src/cvd_log.c#1 $
  $Author: pwbldsvc $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

/* SYSTEM UTILITY APIs. */
#include "msg.h"
#include "mmstd.h"

/* APR APIs. */
#include "apr_errcodes.h"

/* CVD APIs. */
#include "cvd_log_i.h"
#include "vss_common_public_if.h"

/****************************************************************************
 * CVD LOG UTILITY FUNCTIONS                                                   *
 ****************************************************************************/

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
)
{
  cvd_log_voice_t* log_ptr;
  uint16_t payload_size = ( uint16_t )total_size - CVD_LOG_FRAME_HEADER_SIZE_V; 
  /**
   * Total size includes the size of frame header and frame. 
   * No fragmentation support yet.
   */
   
  uint8_t* dst_data_ptr;
  log_ptr = ( ( cvd_log_voice_t* )log_alloc( log_code, 
                                             payload_size + 
                                             sizeof( cvd_log_voice_t ) ) );
  
  if( log_ptr != NULL )
  {
    dst_data_ptr = ( ( uint8_t* )log_ptr ) + sizeof( cvd_log_voice_t );
  
    /* Voice Container */
    log_ptr->voice.cointainer_size = sizeof( cvd_log_voice_container_t ) + 
                                     payload_size;
    log_ptr->voice.container_id = CVD_LOG_VOICE_CONTAINER_ID;
    log_ptr->voice.minor_version = CVD_LOG_VOICE_CONTAINER_MINOR_VERSION_V;

    /* Segment Container */
    log_ptr->voice.segment_header.container_size = sizeof( 
                                                   cvd_log_segment_header_t );
    log_ptr->voice.segment_header.container_id = CVD_LOG_SEGMENT_HEADER_CONTAINER_ID;
    log_ptr->voice.segment_header.minor_version = 
                                    CVD_LOG_SEGMENT_HEADER_CONTAINER_MINOR_VERSION_V;
    log_ptr->voice.segment_header.sequence_num = sequence_num;
    log_ptr->voice.segment_header.total_size = total_size;
    /* No fragmentation support yet */
    log_ptr->voice.segment_header.offset = 0; 

    /* Auxiliary Information Container */
    log_ptr->voice.aux_info.container_size = sizeof( cvd_log_aux_info_t );
    log_ptr->voice.aux_info.container_id = CVD_LOG_AUX_INFO_CONTAINER_ID;
    log_ptr->voice.aux_info.minor_version = CVD_LOG_AUX_INFO_CONTAINER_MINOR_VERSION_V;
    log_ptr->voice.aux_info.instance = instance;
    log_ptr->voice.aux_info.call_num = call_num;
    log_ptr->voice.aux_info.timestamp = timestamp;
    log_ptr->voice.aux_info.tap_id = tap_id;

    /* Data Container */
    log_ptr->voice.data.container_size = sizeof( cvd_log_raw_data_t ) + 
                                         payload_size;
    log_ptr->voice.data.container_id = media_id;
    log_ptr->voice.data.minor_version = CVD_LOG_DATA_CONTAINER_MINOR_VERSION_V;

    /* Extract frame header from payload */
    if (VSS_MEDIA_ID_DTMF == media_id)
    {
        log_ptr->voice.data.frame_header = 0;
        ( void ) mmstd_memcpy( dst_data_ptr, payload_size, 
                           data, payload_size );

    }
    else
    {
    log_ptr->voice.data.frame_header = ( uint8_t )*data;
    ( void ) mmstd_memcpy( dst_data_ptr, payload_size, 
                           data + CVD_LOG_FRAME_HEADER_SIZE_V, payload_size );

    }

    log_commit( log_ptr );
  }
  else
  {
    return APR_EFAILED;
  }

  return APR_EOK;
}

