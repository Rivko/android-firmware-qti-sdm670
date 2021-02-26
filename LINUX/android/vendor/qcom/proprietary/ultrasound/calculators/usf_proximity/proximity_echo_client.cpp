/*===========================================================================
                           proximity_echo_client.cpp

DESCRIPTION: Implements the proximity echo client class.

INITIALIZATION AND SEQUENCING REQUIREMENTS: None

Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#define LOG_TAG "ProximityEchoClient"

/*----------------------------------------------------------------------------
Include files
----------------------------------------------------------------------------*/
#include "proximity_echo_client.h"

/*----------------------------------------------------------------------------
  Defines
----------------------------------------------------------------------------*/
#define CLIENT_NAME "proximity";
#define DAEMON_NAME "usf_proximity"
#define LINK_CFG_FILE_LOCATION "/data/usf/proximity/usf_proximity.cfg"
#define FRAME_FILE_DIR_PATH "/data/usf/proximity/rec/"

/*-----------------------------------------------------------------------------
  Static Variable Definitions
-----------------------------------------------------------------------------*/
/**
  This struct represents a single proximity event generated
  from the proximity library in the dsp.
*/
struct proximity_dsp_event
{
  int                        timestamp;
  int                        seq_num;
  QcUsProximityLibResultType result;
  int distance;
};

/**
 * The name of the file containg the pid of the daemon
 */
static const char* PID_FILE_NAME = "usf_proximity.pid";

static const char* POSTMORTEM_RAW_A_FILENAME      =   "/data/usf/proximity/rec/PostMortem_rawFile_A";
static const char* POSTMORTEM_RAW_B_FILENAME      =   "/data/usf/proximity/rec/PostMortem_rawFile_B";
static const char* POSTMORTEM_EVENTS_A_FILENAME   =   "/data/usf/proximity/rec/PostMortem_eventsFile_A";
static const char* POSTMORTEM_EVENTS_B_FILENAME   =   "/data/usf/proximity/rec/PostMortem_eventsFile_B";

/*------------------------------------------------------------------------------
  Function definitions
------------------------------------------------------------------------------*/

/*============================================================================
  FUNCTION:  get_post_mortem_raw_file_a_name
============================================================================*/
/**
 * See function description in header file
 */
char *ProximityEchoClient::get_post_mortem_raw_file_a_name()
{
  return (char *)POSTMORTEM_RAW_A_FILENAME;
}

/*============================================================================
  FUNCTION:  get_post_mortem_events_file_a_name
============================================================================*/
/**
 * See function description in header file
 */
char *ProximityEchoClient::get_post_mortem_events_file_a_name()
{
  return (char *)POSTMORTEM_EVENTS_A_FILENAME;
}

/*============================================================================
  FUNCTION:  get_post_mortem_raw_file_b_name
============================================================================*/
/**
 * See function description in header file
 */
char *ProximityEchoClient::get_post_mortem_raw_file_b_name()
{
  return (char *)POSTMORTEM_RAW_B_FILENAME;
}

/*============================================================================
  FUNCTION:  get_post_mortem_events_file_b_name
============================================================================*/
/**
 * See function description in header file
 */
char *ProximityEchoClient::get_post_mortem_events_file_b_name()
{
  return (char *)POSTMORTEM_EVENTS_B_FILENAME;
}
/*============================================================================
  FUNCTION:  get_daemon_name
============================================================================*/
/**
 * See function description in header file
 */
char *ProximityEchoClient::get_daemon_name()
{
  return (char *)DAEMON_NAME;
}

/*============================================================================
  FUNCTION:  get_client_name
============================================================================*/
/**
 * See function description in header file
 */
char *ProximityEchoClient::get_client_name()
{
  return (char *)CLIENT_NAME;
}

/*============================================================================
  FUNCTION:  get_dsp_event_size
============================================================================*/
/**
 * See function description in header file
 */
uint32_t ProximityEchoClient::get_dsp_event_size()
{
  return sizeof(struct proximity_dsp_event);
}

/*==============================================================================
  FUNCTION:  get_pid_file_name
==============================================================================*/
/**
 * See function description in header file
 */
char *ProximityEchoClient::get_pid_file_name()
{
  return (char *)PID_FILE_NAME;
}
/*============================================================================
  FUNCTION:  get_num_keyboard_keys
============================================================================*/
/**
 * See function description in header file
 */
int ProximityEchoClient::get_num_keyboard_keys()
{
  return ((QC_US_PROXIMITY_LIB_RESULT_DETECTED - QC_US_PROXIMITY_LIB_RESULT_IDLE) + 1);
}

/*============================================================================
  FUNCTION:  get_num_events
============================================================================*/
/**
 * See function description in header file
 */
int ProximityEchoClient::get_num_events()
{
  return US_MAX_EVENTS;
}

/*============================================================================
  FUNCTION:  get_keyboard_key_base
============================================================================*/
/**
 * See function description in header file
 */
int ProximityEchoClient::get_keyboard_key_base()
{
  return QC_US_PROXIMITY_LIB_RESULT_IDLE;
}

/*==============================================================================
  FUNCTION:  set_algo_dynamic_params
==============================================================================*/
/**
 * See function description in header file
 */
void ProximityEchoClient::set_algo_dynamic_params(us_all_info *paramsStruct,
                                                  EchoContext *echo_context)
{
  UNUSED(echo_context);
  us_stream_param_type stream_param;
  stream_param.module_id = USM_SYNC_PROXIMITY_LIB_MODULE_ID;
  stream_param.param_id = USM_PARAM_ID_SYNC_PROXIMITY_ALGO_CONFIG;
  stream_param.buf_size = paramsStruct->usf_algo_transparent_data_size;
  stream_param.pbuf = (uint8_t *)paramsStruct->usf_algo_transparent_data;

  if (!ual_set_TX_param(stream_param))
  {
    LOGE("%s: ual_set_TX_param first call failed",
         __FUNCTION__);
    echo_exit(EXIT_FAILURE);
  }

  if ((paramsStruct->usf_output_type & OUTPUT_TYPE_RAW_MASK)&& !paramsStruct->usf_app_lib_bypass)
  {
    // don't do persist restore with ADSP if using apps because ...why?
    LOGE("%s: bypassing call", __FUNCTION__);
    return;
  }

  stream_param.module_id = USM_SYNC_PROXIMITY_LIB_MODULE_ID;
  stream_param.param_id = USM_PARAM_ID_SYNC_PROXIMITY_SET_PERSISTENT_DATA;
  stream_param.pbuf =
      (uint8_t*) ual_util_malloc_read(paramsStruct->usf_persist_file,
                                      stream_param.buf_size);
  if (stream_param.pbuf == NULL)
  {
    LOGI("%s: unable to read from '%s', assuming no persistent data from last run",
         __FUNCTION__,
         paramsStruct->usf_persist_file);
    return;
  }

  uint32_t* data = (uint32_t*) stream_param.pbuf;
  LOGI("%s: writing persistent data to ADSP", __FUNCTION__);
  int retval = ual_set_TX_param(stream_param);
  free(stream_param.pbuf);
  if (!retval)
  {
    LOGE("%s: setting persist data call failed",
         __FUNCTION__);
    echo_exit(EXIT_FAILURE);
  }
}

/*==============================================================================
  FUNCTION:  get_cfg_path
==============================================================================*/
/**
 * See function description in header file
 */
char *ProximityEchoClient::get_cfg_path()
{
  return (char *)LINK_CFG_FILE_LOCATION;
}

/*==============================================================================
  FUNCTION:  get_frame_file_path
==============================================================================*/
/**
 * See function description in header file
 */
char *ProximityEchoClient::get_frame_file_path()
{
  return (char *)FRAME_FILE_DIR_PATH;
}

/*==============================================================================
  FUNCTION:  echo_lib_init
==============================================================================*/
/**
 * See function description in header file
 */
void ProximityEchoClient::echo_lib_init(us_all_info *paramsStruct,
                                        EchoContext *echo_context)
{
  ProximityCfg *proximity_config = NULL;
  uint32_t workspace_size = 0, pattern_size_samples = 0;

  echo_context->m_echo_workspace = NULL;
  proximity_config = (ProximityCfg *) malloc(sizeof(ProximityCfg) +
                                  paramsStruct->usf_algo_transparent_data_size);
  if (NULL == proximity_config)
  {
    LOGE("%s: Failed to allocate.",
         __FUNCTION__);
    echo_exit(EXIT_FAILURE);
  }

  // Fill in the requested algorithm configuration
  proximity_config->TxTransparentDataSize = paramsStruct->usf_algo_transparent_data_size;
  memcpy(proximity_config->TxTransparentData,
         paramsStruct->usf_algo_transparent_data,
         proximity_config->TxTransparentDataSize);

  int ret = QcUsProximityLibGetSizes(proximity_config,
                                     &workspace_size,
                                     &pattern_size_samples);
  if (ret)
  {
    free(proximity_config);
    LOGE("%s: Error while getting size from proximity library",
         __FUNCTION__);
    echo_exit(EXIT_FAILURE);
  }

  echo_context->m_pattern_size = pattern_size_samples *
    (paramsStruct->usf_tx_sample_width / BYTE_WIDTH);

  echo_context->m_echo_workspace =
    (signed char *)malloc(workspace_size * sizeof(signed char));
  if (NULL == echo_context->m_echo_workspace)
  {
    free(proximity_config);
    LOGE("%s: Failed to allocate %d bytes.",
         __FUNCTION__,
         workspace_size);
    echo_exit(EXIT_FAILURE);
  }

  echo_context->m_pattern = (int16_t *) malloc(echo_context->m_pattern_size);
  if (NULL == echo_context->m_pattern)
  {
    free(proximity_config);
    LOGE("%s: Failed to allocate %d bytes",
         __FUNCTION__,
         echo_context->m_pattern_size);
    echo_exit(EXIT_FAILURE);
  }

  ret = QcUsProximityLibInit(proximity_config,
                             echo_context->m_echo_workspace,
                             workspace_size);
  free(proximity_config);
  if (ret)
  {
    LOGE("%s: Init algorithm failed.",
         __FUNCTION__);
    echo_exit(0);
  }

  // load the persistent data into the library, if it exists
  uint32_t persistent_data_size;
  void *pBuf = ual_util_malloc_read(paramsStruct->usf_persist_file,
                                    persistent_data_size);
  if (pBuf != NULL) {
    proximity_config = (ProximityCfg *) malloc(sizeof(ProximityCfg) +
                                               persistent_data_size);
    if (proximity_config == NULL) {
      LOGE("%s: couldn't allocate for persistent data size %d", __FUNCTION__, persistent_data_size);
      echo_exit(EXIT_FAILURE);
    }
    memcpy(proximity_config->TxTransparentData,
           pBuf,
           persistent_data_size);
    free(pBuf);
    proximity_config->TxTransparentDataSize = persistent_data_size;
    ret = QcUsProximityLibInit(proximity_config,
                               echo_context->m_echo_workspace,
                               workspace_size);
    free(proximity_config);
    if (ret)
    {
      LOGE("%s: Set persistent data failed",
           __FUNCTION__);
      echo_exit(0);
    }
  }

  ret = QcUsProximityLibGetPattern(echo_context->m_pattern,
                                   pattern_size_samples);
  if (ret)
  {
    LOGE("%s: QcUsProximityLibGetPattern failed.",
         __FUNCTION__);
    echo_exit(0);
  }

  LOGI("%s: Received pattern from library with %d samples",
       __FUNCTION__,
       pattern_size_samples);

  LOGI("%s: Proximity lib init completed.",
       __FUNCTION__);
}

/*==============================================================================
  FUNCTION:  get_event_output_type
==============================================================================*/
/**
 * See function description in header file
 */
uint32_t ProximityEchoClient::get_event_output_type()
{
  return OUTPUT_TYPE_PROXIMITY_EVENT_MASK;
}

/*==============================================================================
  FUNCTION:  get_proximity_output_decision
==============================================================================*/
/**
 * See function description in header file
 */
ProximityOutput ProximityEchoClient::get_proximity_output_decision(short *packet,
                                                                   us_all_info *paramsStruct)
{
  QcUsProximityLibResultType dsp_result = QC_US_PROXIMITY_LIB_RESULT_IDLE;
  ProximityOutput proximity_lib_output;
  proximity_lib_output.proximity = QC_US_PROXIMITY_LIB_RESULT_IDLE;
  proximity_lib_output.distance = 0;
  bool use_apps_lib = (paramsStruct->usf_output_type & OUTPUT_TYPE_RAW_MASK) &&
                      !paramsStruct->usf_app_lib_bypass;

  if (paramsStruct->usf_output_type & OUTPUT_TYPE_PROXIMITY_EVENT_MASK)
  { // Events handling
    struct proximity_dsp_event *dsp_event = (struct proximity_dsp_event *) packet;
    dsp_result = dsp_event->result;
    proximity_lib_output.distance = dsp_event->distance;
    // Update pointer to point to next raw data place
    packet += get_dsp_event_size() / sizeof(short);
  }

  if (use_apps_lib)
  { // Raw data handling
    int rc = QcUsProximityLibEngine(packet,
                                    &proximity_lib_output);
    if (rc)
    {
      LOGE("%s: QcUsProximityLibEngine failed.",
           __FUNCTION__);
      echo_exit(EXIT_FAILURE);
    }
  }

  /* TODO: uncomment when dsp and apps libraries report accurate events.
  if (QC_US_GESTURE_LIB_RESULT_IDLE != proximity_lib_output.gesture &&
      QC_US_GESTURE_LIB_RESULT_IDLE != dsp_result &&
      proximity_lib_output.gesture != dsp_result)
  {
    LOGW("%s: Library result and DSP result are not the same, lib: %d, dsp: %d",
         __FUNCTION__,
         proximity_lib_output.gesture,
         dsp_result);
  }
  */

  // APPS result has the priority over DSP result
  if (QC_US_PROXIMITY_LIB_RESULT_IDLE != proximity_lib_output.proximity)
  {
    return proximity_lib_output;
  }
  else if ((QC_US_PROXIMITY_LIB_RESULT_IDLE != dsp_result) && !use_apps_lib)
  {
    proximity_lib_output.proximity = dsp_result;
    return proximity_lib_output;
  }
  proximity_lib_output.proximity = QC_US_PROXIMITY_LIB_RESULT_IDLE;
  return proximity_lib_output;
}

/*==============================================================================
  FUNCTION:  get_points
==============================================================================*/
/**
 * See function description in header file
 */
int ProximityEchoClient::get_points(EchoContext *echo_context,
                                    short *packet,
                                    us_all_info *paramsStruct)
{
  int rc = 1;
  ProximityOutput proximity_output;
  proximity_output = get_proximity_output_decision(packet,
                                                   paramsStruct);

  int mapped_proximity = (proximity_output.proximity > 0) ?
                         echo_context->m_keys[proximity_output.proximity - echo_context->m_echo_client->get_keyboard_key_base()] : 0;

  event_source_t event_source = ((paramsStruct->usf_output_type & OUTPUT_TYPE_RAW_MASK) &&
                                 !paramsStruct->usf_app_lib_bypass) ?
                       EVENT_SOURCE_APSS : EVENT_SOURCE_DSP;

  // Send socket event
  if (echo_context->m_send_points_to_socket &&
      proximity_output.proximity != QC_US_PROXIMITY_LIB_RESULT_NO_CHANGE)
  {
    LOGD("%s: proximity[%d], distance: %d",
         __FUNCTION__,
         proximity_output.proximity,
         proximity_output.distance);

    if (NULL != echo_context->m_adapter)
    {
      int event = (echo_context->m_adapter->get_event_mapping() == MAPPED) ?
        // Framework requests mapped events
                  mapped_proximity :
        // Framework requests raw events
                  proximity_output.proximity;
      if (1 == echo_context->m_adapter->send_event(event, event_source, proximity_output.distance))
      {
        // TODO: client is ignoring the broken pipe here; next write will cause SIG_PIPE
        LOGE("%s: adapter send_event failed.",
             __FUNCTION__);
      }
    }
  }

  if ((proximity_output.proximity > 0) &&
      (proximity_output.proximity <= (int)(sizeof(echo_context->m_keys) /
                                sizeof(echo_context->m_keys[0])) ) )
  {
    // There is a proximity.
    // Assumption: a key duration << time between 2 proximities.

    // Send start press to UAL
    if (echo_context->m_send_points_to_ual)
    {
      add_event_key (mapped_proximity, 1);
      add_event_key (mapped_proximity, 0);
    }

    echo_context->m_echoStats.m_nPointsCalculated++;
  }

  return echo_context->m_s_eventCounter;
}

/*============================================================================
  FUNCTION:  send_keep_alive
============================================================================*/
/**
 * See function description in header file
 */
void ProximityEchoClient::send_keep_alive(EchoContext *echo_context,
                    bool keep_alive,
                    us_all_info *paramsStruct)
{
  (void) paramsStruct;
  if (NULL != echo_context->m_adapter)
  {
    if (1 == echo_context->m_adapter->send_event(
        QC_US_PROXIMITY_KEEP_ALIVE,
        EVENT_SOURCE_APSS,
        keep_alive ? 1 : 0))
    {
      // TODO: client is ignoring the broken pipe here; next write will cause SIG_PIPE
      LOGE("%s: adapter send_event failed.",
           __FUNCTION__);
    }
  }
}

/*============================================================================
  FUNCTION:  save_persistent_data
============================================================================*/
/**
 * See function description in header file
 */
void ProximityEchoClient::save_persistent_data(EchoContext *echo_context,
                                               us_all_info *paramsStruct)
{
  // payload returned is 32-bit size followed by persistent data when calling LPASS,
  // but ProximityCfg is updated when calling lib.

  // Use same buffer for both calls, handle fail/free locally.
  const uint32_t payloadSize = sizeof(uint32_t)
                               + paramsStruct->usf_persist_buf_size;
  ProximityCfg *pData = (ProximityCfg *) malloc(sizeof(ProximityCfg)
                                                + payloadSize);
  if (pData == NULL)
  {
    LOGE("%s: Couldn't allocate memory for return buffer", __FUNCTION__);
    return;
  }
  pData->TxTransparentDataSize = payloadSize;

  int retval = 0; // pass

  const bool usingLib = (paramsStruct->usf_output_type & OUTPUT_TYPE_RAW_MASK) &&
      !paramsStruct->usf_app_lib_bypass;

  if (usingLib)
  {
    int retval = QcUsProximityGetPersistentData(echo_context->m_echo_workspace,
                                                pData,
                                                payloadSize);
    if (retval)
    {
      LOGE("%s: error calling QcUsProximityGetPersistentData", __FUNCTION__);
    }
  }
  else
  {
    us_stream_param_type req;
    req.module_id = USM_SYNC_PROXIMITY_LIB_MODULE_ID;
    req.param_id = USM_PARAM_ID_SYNC_PROXIMITY_GET_PERSISTENT_DATA;
    req.buf_size = pData->TxTransparentDataSize;
    req.pbuf = (uint8_t*) pData->TxTransparentData;
    if (!ual_get_TX_param(req))
    {
      LOGE("%s: error getting tx param USM_PARAM_ID_SYNC_PROXIMITY_PERSISTENT_DATA",
           __FUNCTION__);
      retval = 1;
    }
  }
  if (retval)
  {
    free(pData);
    return;
  }

  // validate returned message
  // If using lib, pData was updated with size. If using LPASS, size is first 4 bytes
  const uint32_t returnedDataSizeBytes =
      usingLib ? pData->TxTransparentDataSize :
                 *((uint32_t*) pData->TxTransparentData);
  const uint32_t usableBytes = payloadSize - sizeof(uint32_t);
  if (returnedDataSizeBytes > usableBytes)
  {
    LOGE("%s: size returned by call (%d) greater than usable bytes (%d)",
         __FUNCTION__,
         returnedDataSizeBytes,
         usableBytes);
    free(pData);
    return;
  }

  LOGI("%s: persistent data payload size %d, saving to file",
       __FUNCTION__,
       returnedDataSizeBytes);
  // data starts at beginning of buffer for lib, 4 bytes after for LPASS
  void *pBuf = usingLib ? pData->TxTransparentData : (pData->TxTransparentData + sizeof(uint32_t));
  ual_util_write_file(paramsStruct->usf_persist_file,
                      pBuf,
                      returnedDataSizeBytes,
                      sizeof(uint8_t));
  free(pData);
}
