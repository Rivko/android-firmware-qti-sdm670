/*===========================================================================
                           sync_gesture_echo_client.cpp

DESCRIPTION: Implements the gesture bus adapter class.

INITIALIZATION AND SEQUENCING REQUIREMENTS: None

Copyright (c) 2012-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/
#define LOG_TAG "SyncGestureEchoClient"

/*----------------------------------------------------------------------------
Include files
----------------------------------------------------------------------------*/
#include "sync_gesture_echo_client.h"

/*----------------------------------------------------------------------------
  Defines
----------------------------------------------------------------------------*/
#define CLIENT_NAME "sync_gesture";
#define DAEMON_NAME "usf_sync_gesture"
#define LINK_CFG_FILE_LOCATION "/data/usf/sync_gesture/usf_sync_gesture.cfg"
#define FRAME_FILE_DIR_PATH "/data/usf/sync_gesture/rec/"

/*-----------------------------------------------------------------------------
  Typedefs
-----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------
  Classes
-----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
  Static Variable Definitions
-----------------------------------------------------------------------------*/
/**
  This struct represents a single sync gesture event generated
  from the gesture library in the dsp.
*/
struct gesture_dsp_event
{
  int                      timestamp;
  int                      seq_num;
  QcUsGestureLibResultType result;
  int velocity;
};

/**
 * The name of the file containg the pid of the daemon
 */
static const char* PID_FILE_NAME = "usf_sync_gesture.pid";

/*------------------------------------------------------------------------------
  Function definitions
------------------------------------------------------------------------------*/

/*============================================================================
  CONSTRUCTOR:
============================================================================*/

/*==============================================================================
  FUNCTION:  get_directions_subset
==============================================================================*/
/**
 * See function description in header file
 */
int SyncGestureEchoClient::get_directions_subset(EchoContext *echo_context)
{
  // Gets property value with "0" as default
  char prop_val[PROPERTY_VALUE_MAX];
  property_get("debug.gesture_subset",
               prop_val,
               "0");
  if (strcmp("0", prop_val) < 0)
  {
    int directions = 0;
    sscanf(prop_val, "%d", &directions);
    LOGD("%s: Subset defined by property, %d",
         __FUNCTION__,
         directions);
    return directions;
  }

  if (NULL != echo_context->m_adapter)
  {
    LOGD("%s: Subset defined by adapter, %d",
         __FUNCTION__,
         (uint32_t)echo_context->m_adapter->get_config().sub_mode);
    return (uint32_t)echo_context->m_adapter->get_config().sub_mode;
  }

  return 0;
}

/*============================================================================
  FUNCTION:  get_daemon_name
============================================================================*/
/**
 * See function description in header file
 */
char *SyncGestureEchoClient::get_daemon_name()
{
  return (char *)DAEMON_NAME;
}

/*============================================================================
  FUNCTION:  get_client_name
============================================================================*/
/**
 * See function description in header file
 */
char *SyncGestureEchoClient::get_client_name()
{
  return (char *)CLIENT_NAME;
}

/*============================================================================
  FUNCTION:  get_dsp_event_size
============================================================================*/
/**
 * See function description in header file
 */
uint32_t SyncGestureEchoClient::get_dsp_event_size()
{
  return sizeof(struct gesture_dsp_event);
}

/*============================================================================
  FUNCTION:  get_post_mortem_raw_file_a_name
============================================================================*/
/**
 * See function description in header file
 */
char *SyncGestureEchoClient::get_post_mortem_raw_file_a_name()
{
  return (char *)(NULL);
}


/*============================================================================
  FUNCTION:  get_post_mortem_events_file_a_name
============================================================================*/
/**
 * See function description in header file
 */
char *SyncGestureEchoClient::get_post_mortem_events_file_a_name()
{
  return (char *)(NULL);
}

/*============================================================================
  FUNCTION:  get_post_mortem_raw_file_b_name
============================================================================*/
/**
 * See function description in header file
 */
char *SyncGestureEchoClient::get_post_mortem_raw_file_b_name()
{
  return (char *)(NULL);
}


/*============================================================================
  FUNCTION:  get_post_mortem_events_file_b_name
============================================================================*/
/**
 * See function description in header file
 */
char *SyncGestureEchoClient::get_post_mortem_events_file_b_name()
{
  return (char *)(NULL);
}
/*==============================================================================
  FUNCTION:  get_pid_file_name
==============================================================================*/
/**
 * See function description in header file
 */
char *SyncGestureEchoClient::get_pid_file_name()
{
  return (char *)PID_FILE_NAME;
}
/*============================================================================
  FUNCTION:  get_num_keyboard_keys
============================================================================*/
/**
 * See function description in header file
 */
int SyncGestureEchoClient::get_num_keyboard_keys()
{
  return ((QC_US_GESTURE_LIB_RESULT_REPEAT - QC_US_GESTURE_LIB_RESULT_SELECT) + 1);
}

/*============================================================================
  FUNCTION:  get_num_events
============================================================================*/
/**
 * See function description in header file
 */
int SyncGestureEchoClient::get_num_events()
{
  return US_MAX_EVENTS;
}

/*============================================================================
  FUNCTION:  get_keyboard_key_base
============================================================================*/
/**
 * See function description in header file
 */
int SyncGestureEchoClient::get_keyboard_key_base()
{
  return QC_US_GESTURE_LIB_RESULT_SELECT;
}

/*==============================================================================
  FUNCTION:  set_algo_dynamic_params
==============================================================================*/
/**
 * See function description in header file
 */
void SyncGestureEchoClient::set_algo_dynamic_params(us_all_info *paramsStruct,
                                                    EchoContext *echo_context)
{
  us_stream_param_type stream_param;
  stream_param.module_id = USM_SYNC_GESTURE_LIB_MODULE_ID;
  stream_param.param_id = USM_PARAM_ID_SYNC_GESTURE_ALGO_CONFIG;
  stream_param.buf_size = paramsStruct->usf_algo_transparent_data_size;
  stream_param.pbuf = (uint8_t *)paramsStruct->usf_algo_transparent_data;
  if (!ual_set_TX_param(stream_param))
  {
    LOGE("%s: ual_set_TX_param failed",
         __FUNCTION__);
    echo_exit(EXIT_FAILURE);
  }

  stream_param.param_id = USM_PARAM_ID_SYNC_GESTURE_DYNAMIC_CONFIG;
  usm_param_id_sync_gesture_dynanic_cfg_t dynamic_config_payload;
  dynamic_config_payload.sync_gesture_dynamic_cfg_version =
    USM_API_VERSION_SYNC_GESTURE_DYNAMIC_CONFIG;
  dynamic_config_payload.directions = get_directions_subset(echo_context);
  stream_param.buf_size = sizeof(dynamic_config_payload);
  stream_param.pbuf = (uint8_t *)(&dynamic_config_payload);
  if (0 < dynamic_config_payload.directions)
  {
    if (!ual_set_TX_param(stream_param))
    {
      LOGE("%s: ual_set_TX_param failed",
           __FUNCTION__);
      echo_exit(EXIT_FAILURE);
    }
  }
}

/*==============================================================================
  FUNCTION:  get_cfg_path
==============================================================================*/
/**
 * See function description in header file
 */
char *SyncGestureEchoClient::get_cfg_path()
{
  return (char *)LINK_CFG_FILE_LOCATION;
}

/*==============================================================================
  FUNCTION:  get_frame_file_path
==============================================================================*/
/**
 * See function description in header file
 */
char *SyncGestureEchoClient::get_frame_file_path()
{
  return (char *)FRAME_FILE_DIR_PATH;
}

/*==============================================================================
  FUNCTION:  echo_lib_init
==============================================================================*/
/**
 * See function description in header file
 */
void SyncGestureEchoClient::echo_lib_init(us_all_info *paramsStruct,
                                          EchoContext *echo_context)
{
  GestureCfg *gesture_config = NULL;
  uint32_t workspace_size = 0, pattern_size_samples = 0;

  echo_context->m_echo_workspace = NULL;
  gesture_config = (GestureCfg *) malloc(sizeof(GestureCfg) +
                                  paramsStruct->usf_algo_transparent_data_size);
  if (NULL == gesture_config)
  {
    LOGE("%s: Failed to allocate.",
         __FUNCTION__);
    echo_exit(EXIT_FAILURE);
  }

  // Fill in the requested algorithm configuration
  gesture_config->TxTransparentDataSize = paramsStruct->usf_algo_transparent_data_size;
  memcpy(gesture_config->TxTransparentData,
         paramsStruct->usf_algo_transparent_data,
         gesture_config->TxTransparentDataSize);

  int ret = QcUsGestureLibGetSizes(gesture_config,
                                   &workspace_size,
                                   &pattern_size_samples);
  if (ret)
  {
    free(gesture_config);
    LOGE("%s: Error while getting size from sync gesture library",
         __FUNCTION__);
    echo_exit(EXIT_FAILURE);
  }

  echo_context->m_pattern_size = pattern_size_samples *
    (paramsStruct->usf_tx_sample_width / BYTE_WIDTH);

  echo_context->m_echo_workspace =
    (signed char *)malloc(workspace_size * sizeof(signed char));
  if (NULL == echo_context->m_echo_workspace)
  {
    free(gesture_config);
    LOGE("%s: Failed to allocate %d bytes.",
         __FUNCTION__,
         workspace_size);
    echo_exit(EXIT_FAILURE);
  }

  echo_context->m_pattern = (int16_t *) malloc(echo_context->m_pattern_size);
  if (NULL == echo_context->m_pattern)
  {
    free(gesture_config);
    LOGE("%s: Failed to allocate %d bytes",
         __FUNCTION__,
         echo_context->m_pattern_size);
    echo_exit(EXIT_FAILURE);
  }

  ret = QcUsGestureLibInit(gesture_config,
                           echo_context->m_echo_workspace,
                           workspace_size);
  if (ret)
  {
    free(gesture_config);
    LOGE("%s: Init algorithm failed.",
         __FUNCTION__);
    echo_exit(0);
  }
  free(gesture_config);

  usm_param_id_sync_gesture_dynanic_cfg_t dynamic_config_payload;
  dynamic_config_payload.sync_gesture_dynamic_cfg_version =
    USM_API_VERSION_SYNC_GESTURE_DYNAMIC_CONFIG;
  dynamic_config_payload.directions = get_directions_subset(echo_context);

  if (0 < dynamic_config_payload.directions)
  {
    ret = QcUsGestureLibSetDynamicConfig(
      (int *)&dynamic_config_payload.sync_gesture_dynamic_cfg_version,
      sizeof(dynamic_config_payload)/sizeof(int));
    if (ret)
    {
      LOGE("%s: Set dynamic config failed.",
           __FUNCTION__);
      echo_exit(0);
    }
  }

  ret = QcUsGestureLibGetPattern(echo_context->m_pattern,
                                 pattern_size_samples);
  if (ret)
  {
    LOGE("%s: QcUsGestureLibGetPattern failed.",
         __FUNCTION__);
    echo_exit(0);
  }

  LOGI("%s: Received pattern from library with %d samples",
       __FUNCTION__,
       pattern_size_samples);

  LOGI("%s: Sync Gesture lib init completed.",
       __FUNCTION__);
}

/*==============================================================================
  FUNCTION:  get_event_output_type
==============================================================================*/
/**
 * See function description in header file
 */
uint32_t SyncGestureEchoClient::get_event_output_type()
{
  return OUTPUT_TYPE_GESTURE_EVENT_MASK;
}

/*==============================================================================
  FUNCTION:  get_gesture_output_decision
==============================================================================*/
/**
 * See function description in header file
 */
GestureOutput SyncGestureEchoClient:: get_gesture_output_decision(short *packet,
                                                                  us_all_info *paramsStruct)
{
  QcUsGestureLibResultType dsp_result = QC_US_GESTURE_LIB_RESULT_IDLE;
  GestureOutput gesture_lib_output;
  gesture_lib_output.gesture = QC_US_GESTURE_LIB_RESULT_IDLE;
  int velocity = 0;

  if (paramsStruct->usf_output_type & OUTPUT_TYPE_GESTURE_EVENT_MASK)
  { // Events handling
    struct gesture_dsp_event *dsp_event = (struct gesture_dsp_event *) packet;
    dsp_result = dsp_event->result;
    velocity = dsp_event->velocity;
    // Update pointer to point to next raw data place
    packet += get_dsp_event_size() / sizeof(short);
  }

  if ((paramsStruct->usf_output_type & OUTPUT_TYPE_RAW_MASK) &&
      !paramsStruct->usf_app_lib_bypass)
  { // Raw data handling
    int rc = QcUsGestureLibEngine(packet,
                              &gesture_lib_output);
    if (rc)
    {
      LOGE("%s: QcUsGestureLibEngine failed.",
           __FUNCTION__);
      echo_exit(EXIT_FAILURE);
    }
  }

  /* TODO: uncomment when dsp and apps libraries report accurate events.
  if (QC_US_GESTURE_LIB_RESULT_IDLE != gesture_lib_output.gesture &&
      QC_US_GESTURE_LIB_RESULT_IDLE != dsp_result &&
      gesture_lib_output.gesture != dsp_result)
  {
    LOGW("%s: Library result and DSP result are not the same, lib: %d, dsp: %d",
         __FUNCTION__,
         gesture_lib_output.gesture,
         dsp_result);
  }
  */

  // APPS result has the priority over DSP result
  if (QC_US_GESTURE_LIB_RESULT_IDLE != gesture_lib_output.gesture)
  {
    return gesture_lib_output;
  }
  else if (QC_US_GESTURE_LIB_RESULT_IDLE != dsp_result)
  {
    gesture_lib_output.gesture = dsp_result;
    gesture_lib_output.velocity = velocity;
    return gesture_lib_output;
  }
  gesture_lib_output.gesture = QC_US_GESTURE_LIB_RESULT_IDLE;
  return gesture_lib_output;
}

/*==============================================================================
  FUNCTION:  get_points
==============================================================================*/
/**
 * See function description in header file
 */
int SyncGestureEchoClient::get_points(EchoContext *echo_context,
                                      short *packet,
                                      us_all_info *paramsStruct)
{
  int rc = 1;
  GestureOutput gesture_output;

  gesture_output = get_gesture_output_decision(packet,
                                               paramsStruct);

  if (QC_US_GESTURE_LIB_RESULT_IDLE != gesture_output.gesture)
  {
    LOGD("%s: gesture[%d]",
         __FUNCTION__,
         gesture_output.gesture);
  }

  int mapped_gesture = (gesture_output.gesture > 0) ?
                       echo_context->m_keys[gesture_output.gesture - echo_context->m_echo_client->get_keyboard_key_base()] : 0;

  event_source_t event_source = ((paramsStruct->usf_output_type & OUTPUT_TYPE_RAW_MASK) &&
                                 !paramsStruct->usf_app_lib_bypass) ?
                       EVENT_SOURCE_APSS : EVENT_SOURCE_DSP;

  // Send socket event
  if (echo_context->m_send_points_to_socket &&
      QC_US_GESTURE_LIB_RESULT_IDLE != gesture_output.gesture)
  {
    if (NULL != echo_context->m_adapter)
    {
      int event = (echo_context->m_adapter->get_event_mapping() == MAPPED) ?
        // Framework requests mapped events
                  mapped_gesture :
        // Framework requests raw events
                  gesture_output.gesture;
      if (1 == echo_context->m_adapter->send_event(event, event_source, gesture_output.velocity))
      {
        LOGE("%s: adapter send_event failed.",
             __FUNCTION__);
      }
    }
    else
    {
      LOGE("%s: adapter is null as expected.",
             __FUNCTION__);
    }
  }

  if ((gesture_output.gesture > 0) &&
      (gesture_output.gesture <= (int)(sizeof(echo_context->m_keys) /
                                sizeof(echo_context->m_keys[0])) ) )
  {
    // There is a gesture.
    // Assumption: a key duration << time between 2 gestures.

    // Send start press to UAL
    if (echo_context->m_send_points_to_ual)
    {
      add_event_key (mapped_gesture,
                     1);
      add_event_key (mapped_gesture,
                     0);
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
void SyncGestureEchoClient::send_keep_alive(EchoContext *echo_context,
                    bool keep_alive,
                    us_all_info *paramsStruct)
{
  (void) echo_context;
  (void) keep_alive;
  (void) paramsStruct;
  LOGE("%s: stub", __FUNCTION__);
}

void SyncGestureEchoClient::save_persistent_data(EchoContext *echo_context,
                                                 us_all_info *paramsStruct)
{
  (void) echo_context;
  (void) paramsStruct;
  LOGE("%s: stub", __FUNCTION__);
}
