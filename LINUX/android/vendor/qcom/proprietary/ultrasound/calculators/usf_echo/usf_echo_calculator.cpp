/*===========================================================================
                           usf_sync_echo.cpp

DESCRIPTION: Implementation of the Synchronized Echo daemon.


INITIALIZATION AND SEQUENCING REQUIREMENTS:
  If not started through java app then make sure to have
  correct /data/usf/sync_echo/usf_sync_echo.cfg file linked to the wanted cfg file
  placed in /data/usf/sync_echo/cfg/.

Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#define LOG_TAG "usf_sync_echo"

/*----------------------------------------------------------------------------
  Include files
----------------------------------------------------------------------------*/
#include "usf_log.h"
#include <ual.h>
#include <cutils/properties.h>
#include <ual_util.h>
#include <stdlib.h>
#include <errno.h>
#include "ual_util_frame_file.h"
#include "us_adapter_factory.h"
#include "echo_client.h"
#include <semaphore.h>
#include "usf_property_thread.h"

/*----------------------------------------------------------------------------
  Defines
----------------------------------------------------------------------------*/
#define STATISTIC_PRINT_INTERVAL 500
#define SOCKET_PROBLEM_MSG_INTERVAL 50
#define MAX_SUPPORTED_MIC 8
#define SIZE_32_OFFSET 31
#define SIZE_32_MASK 0xffffffe0
#define BYTE_OFFSET_OF_GROUP_IN_TX_TRANSPARENT 2
#define BYTE_OFFSET_OF_SKIP_IN_TX_TRANSPARENT 0
#define RX_RELEASE_TIMEOUT_SEC 2
#define NAME_LENGTH 500
#define UAL_READ_DEFAULT_TIMEOUT_SEC 1
#define POSTMORTEM_BUFSIZE_MAX            10485760  /* 10MB */
#define POSTMORTEM_FRAMENUM_MAX           51200     /* max file size contains
                                   51200 frames = 100MB/2048B(bytes per frame)*/

enum echo_event_dest
{
  DEST_UAL = 0x01,
  DEST_SOCKET = 0x02,
};

/*-----------------------------------------------------------------------------
  Typedefs
-----------------------------------------------------------------------------*/

/**
 * Ptr to the frame file recording.
 */
FILE* frame_file = NULL;

/*-----------------------------------------------------------------------------
  Static Variable Definitions
-----------------------------------------------------------------------------*/
/**
  Ptr to the cfg file. The ptr is global bacause we want to
  close the file before exit in the function echo_exit.
  This function is called also from the signal_handler function
  which is not familiar with the cfg file.
*/
static FILE *cfgFile = NULL;

/**
  Byte offset of "sequence number" field in the US data frame
*/
static const uint16_t ECHO_FRAME_SEQNUM_OFFSET = 8;

/**
  Byte offset of "sequence number" field in the DSP event
*/
static const uint16_t DSP_EVENT_SEQNUM_OFFSET = 4;

/**
  Sync Echo calculator version
*/
static const char* CLIENT_VERSION = "1.0";

/**
  The daemon running control
*/
static volatile bool sb_run = true;

/**
  The daemon's main thread
*/
static pthread_t s_main_thread;

/**
  Indication whether we have a pending request to stop using Rx
  because of Rx arbitration
*/
static volatile bool sb_rx_release_requested = false;

/**
  Sempahore used to signal when Rx is released on Rx arbitration
*/
static sem_t s_rx_release_complete;

/**
  Store the current active rx audio device mask for
  notifications to socket adapter
 */
static audio_devices_t s_rx_active_devices_mask = AUDIO_DEVICE_NONE;

/**
  The daemon running control. Includes allocating and freeing
  resources, and is mainly used for the adapter control.
*/
static volatile bool daemon_run = true;

/**
  EchoContext will hold all the information needed for Sync
  Echo calculation.
*/
static EchoContext echo_context;

/**
  This thread is intended to periodically save the persistent calibration data
  to the file system.
*/
static pthread_t s_calib_thread = 0;

/**
  This semaphore is used to control the flow of the calib_thread by the main
  thread.
*/
static sem_t s_calib_thread_sem;

/*------------------------------------------------------------------------------
  Function definitions
------------------------------------------------------------------------------*/

/*==============================================================================
  FUNCTION:  echo_rx_device_state_cb
==============================================================================*/
/**
  Callback function for rx device state change (arbitration).
*/
static void echo_rx_device_state_cb(
  audio_devices_t device,
  aud_dev_arbi_event_t evt,
  aud_dev_arbi_event_cb_resp_flags_t*)
{
  UNUSED(device);
  LOGD("%s:  Got event %d",
        __FUNCTION__,
        evt);

  switch(evt)
  {
  case AUD_DEV_ARBI_EVENT_DEVICE_REQUESTED:
    {
      if(ual_rx_dev_arbi_is_topology_switch_needed())
      {
        sb_rx_release_requested = true;
        sb_run = false;

        int res = pthread_kill(s_main_thread, SIGALRM);

        if (res!= 0)
        {
          LOGE("%s: error sending signal to main thread, res=%d, tid=%lu",
                 __FUNCTION__,
                 res,
                 s_main_thread);
        }

        struct timespec ts;
        if (clock_gettime(CLOCK_REALTIME, &ts) < 0)
        {
            LOGE("%s: error getting time, %d, %s",
                 __FUNCTION__,
                 errno,
                 strerror(errno));

            break;
        }

        ts.tv_sec += RX_RELEASE_TIMEOUT_SEC;

        // Wait for Rx release by daemon main thread
        int rc = sem_timedwait(&s_rx_release_complete, &ts);
        if (rc < 0)
        {
            LOGE("%s: timeout waiting for RX release semaphore",
                       __FUNCTION__);
            break;
        }
      }
    }
    break;
  case AUD_DEV_ARBI_EVENT_DEVICE_RELEASED:
    // We don't wait for device release since we can adapt by selecting
    // a different mixer path
    break;
  default:
    {
      LOGW("%s: invalid event code", __FUNCTION__);
    }
  }

  LOGD("%s:  returning from evt %d",
        __FUNCTION__,
        evt);
}

/*==============================================================================
  FUNCTION:  echo_rx_notify_active_earpiece_state
==============================================================================*/
/**
 Check if the earpiece active state changed by audio and notify
 listener via socket adapter (if force is true, always notify)
 */
static void echo_rx_notify_active_earpiece_state
(
  us_all_info *paramsStruct,
  bool force
)
{
  if (!paramsStruct->usf_rx_dev_arbitration)
  {
    return;
  }

  audio_devices_t current_active_devices =
    ual_rx_dev_arbi_get_active_audio_device_mask();
  bool current_earpiece_active =
    current_active_devices & AUDIO_DEVICE_OUT_EARPIECE;
  bool prev_earpiece_active =
    s_rx_active_devices_mask & AUDIO_DEVICE_OUT_EARPIECE;
  if (force || current_earpiece_active != prev_earpiece_active)
  {
    s_rx_active_devices_mask = current_active_devices;
    echo_context.m_echo_client->send_keep_alive(&echo_context,
                                                current_earpiece_active,
                                                paramsStruct);
  }
}

/*==============================================================================
  FUNCTION:  echo_free_resources
==============================================================================*/
/**
  Clean daemon parameters.
*/
static void echo_free_resources (bool error_state)
{
  int rc = ual_close(error_state);
  LOGD("%s: ual_close: rc=%d;",
       __FUNCTION__,
       rc);

  if (NULL != echo_context.m_echo_workspace)
  {
    free(echo_context.m_echo_workspace);
    echo_context.m_echo_workspace = NULL;
  }

  if (NULL != echo_context.m_pattern)
  {
    free(echo_context.m_pattern);
    echo_context.m_pattern = NULL;
  }

  if (NULL != echo_context.m_events)
  {
    free(echo_context.m_events);
    echo_context.m_events = NULL;
  }

  if (NULL != echo_context.m_keys)
  {
    free(echo_context.m_keys);
    echo_context.m_keys = NULL;
  }
}

/*==============================================================================
  FUNCTION:  echo_exit
==============================================================================*/
/**
  Perform clean exit of the daemon.
*/
int echo_exit (int status)
{
  bool error_state = (status != EXIT_SUCCESS);

  if (s_calib_thread > 0)
  {
    // Post on semaphore to terminate calib_thread
    sem_post(&s_calib_thread_sem);

    pthread_join(s_calib_thread,
      NULL);
    s_calib_thread = 0;
    sem_destroy(&s_calib_thread_sem);
  }

  if (NULL != echo_context.m_adapter)
  {
    if (error_state)
    {
      echo_context.m_adapter->on_error();
    }
    echo_context.m_adapter->disconnect();

    destroy_adapter();
  }

  echo_free_resources(error_state);

  if (NULL != cfgFile)
  {
    fclose(cfgFile);
    cfgFile = NULL;
  }

  ual_util_close_and_sync_file(frame_file);
  frame_file = NULL;

  int ret = ual_util_remove_declare_pid(echo_context.m_echo_client->get_pid_file_name());
  if (0 != ret)
  {
    LOGW("%s: Removing pid file failed",
         __FUNCTION__);
  }

  LOGI("%s: Sync Echo end. status=%d",
       __FUNCTION__,
       status);

  property_thread_exit(echo_context.m_echo_client->get_daemon_name());

  _exit(status);
}

/*==============================================================================
  FUNCTION:  echo_post_mortem_get_raw_file_handle_a
==============================================================================*/
static FILE* echo_post_mortem_get_raw_file_handle_a(EchoContext* echo_context)
{
  FILE* retVal=NULL;

  char* filename;

  filename = echo_context->m_echo_client->get_post_mortem_raw_file_a_name();
  if (NULL == filename)
  {
    LOGE("%s: Can't get post mortem raw file A name",__FUNCTION__);
    return(retVal);
  }

  if ((retVal = fopen(filename, "wb")) == NULL)
  {
    LOGE("%s: Could not open post mortem raw file A file", __FUNCTION__);
  }

  return(retVal);
}
/*==============================================================================
  FUNCTION:  echo_post_mortem_get_raw_file_handle_b
==============================================================================*/
static FILE* echo_post_mortem_get_raw_file_handle_b(EchoContext* echo_context)
{
  FILE* retVal=NULL;

  char* filename;

  filename = echo_context->m_echo_client->get_post_mortem_raw_file_b_name();
  if (NULL == filename)
  {
    LOGE("%s: Can't get post mortem raw file B name",__FUNCTION__);
    return(retVal);
  }

  if ((retVal = fopen(filename, "wb")) == NULL)
  {
    LOGE("%s: Could not open post mortem raw file B file", __FUNCTION__);
  }

  return(retVal);
}

/*==============================================================================
  FUNCTION:  echo_post_mortem_get_event_file_handle_a
==============================================================================*/
static FILE* echo_post_mortem_get_event_file_handle_a(EchoContext* echo_context)
{
  FILE* retVal=NULL;

  char* filename;

  filename = echo_context->m_echo_client->get_post_mortem_events_file_a_name();
  if (NULL == filename)
  {
    LOGE("%s: Can't get post mortem events file A name",__FUNCTION__);
    return(retVal);
  }

  if ((retVal = fopen(filename, "wb")) == NULL)
  {
    LOGE("%s: Could not open post mortem event file A file", __FUNCTION__);
  }

  return(retVal);
}

/*==============================================================================
  FUNCTION:  echo_post_mortem_get_event_file_handle_b
==============================================================================*/
static FILE* echo_post_mortem_get_event_file_handle_b(EchoContext* echo_context)
{
  FILE* retVal=NULL;

  char* filename;

  filename = echo_context->m_echo_client->get_post_mortem_events_file_b_name();
  if (NULL == filename)
  {
    LOGE("%s: Can't get post mortem events file B name",__FUNCTION__);
    return(retVal);
  }

  if ((retVal = fopen(filename, "wb")) == NULL)
  {
    LOGE("%s: Could not open post mortem event file B file", __FUNCTION__);
  }

  return(retVal);
}

/*==============================================================================
  FUNCTION:  echo_post_mortem_close
==============================================================================*/
static void echo_post_mortem_close(EchoContext* echo_context)
{
  if (NULL != echo_context->m_post_mortem_raw_file_handle_a)
  {
    ual_util_close_and_sync_file(echo_context->m_post_mortem_raw_file_handle_a);
    echo_context->m_post_mortem_raw_file_handle_a = NULL;
  }

  if (NULL != echo_context->m_post_mortem_raw_file_handle_b)
  {
    ual_util_close_and_sync_file(echo_context->m_post_mortem_raw_file_handle_b);
    echo_context->m_post_mortem_raw_file_handle_b = NULL;
  }

  if (NULL != echo_context->m_post_mortem_event_file_handle_a)
  {
    ual_util_close_and_sync_file(echo_context->m_post_mortem_event_file_handle_a);
    echo_context->m_post_mortem_event_file_handle_a = NULL;
  }

  if (NULL != echo_context->m_post_mortem_event_file_handle_b)
  {
    ual_util_close_and_sync_file(echo_context->m_post_mortem_event_file_handle_b);
    echo_context->m_post_mortem_event_file_handle_b = NULL;
  }

  echo_context->m_post_mortem_enable = false;
  echo_context->m_post_mortem_current_frame_count = 0;
  echo_context->m_post_mortem_max_frame_count = 0;
  echo_context->m_post_mortem_active_file = 0;

  LOGD("%s, post mortem closed.", __FUNCTION__);
}


/*==============================================================================
  FUNCTION:  echo_post_mortem_fileWriteSwap_util
==============================================================================*/
static bool echo_post_mortem_fileWriteSwap_util(EchoContext* echo_context)
{
  bool retVal = true;

  // Parameter check
  if (NULL == echo_context)
  {
    LOGE("%s, Invalid arguments.",
         __FUNCTION__);
    return false;
  }

  if (0 == echo_context->m_post_mortem_active_file)
  {
    // close active files (A)
    ual_util_close_and_sync_file(echo_context->m_post_mortem_raw_file_handle_a);
    ual_util_close_and_sync_file(echo_context->m_post_mortem_event_file_handle_a);
    echo_context->m_post_mortem_raw_file_handle_a = NULL;
    echo_context->m_post_mortem_event_file_handle_a = NULL;

    //open new active files (B)
    echo_context->m_post_mortem_raw_file_handle_b =
        echo_post_mortem_get_raw_file_handle_b(echo_context);
    if (NULL == echo_context->m_post_mortem_raw_file_handle_b)
    {
      LOGE("%s: Could not open post mortem raw file B file", __FUNCTION__);
      retVal = false;
    }
    echo_context->m_post_mortem_event_file_handle_b =
        echo_post_mortem_get_event_file_handle_b(echo_context);
    if (NULL == echo_context->m_post_mortem_event_file_handle_b)
    {
      LOGE("%s: Could not open post mortem event file B file", __FUNCTION__);
      retVal = false;
    }
    echo_context->m_post_mortem_active_file = 1;
    LOGD("%s: Swap to file B", __FUNCTION__);

  }
  else
  {
    // close active files (B)
    ual_util_close_and_sync_file(echo_context->m_post_mortem_raw_file_handle_b);
    ual_util_close_and_sync_file(echo_context->m_post_mortem_event_file_handle_b);
    echo_context->m_post_mortem_raw_file_handle_b = NULL;
    echo_context->m_post_mortem_event_file_handle_b = NULL;

    // open new active files (A)
    echo_context->m_post_mortem_raw_file_handle_a =
        echo_post_mortem_get_raw_file_handle_a(echo_context);
    if (NULL == echo_context->m_post_mortem_raw_file_handle_a)
    {
      LOGE("%s: Could not open post mortem raw file A file", __FUNCTION__);
      retVal = false;
    }
    echo_context->m_post_mortem_event_file_handle_a =
        echo_post_mortem_get_event_file_handle_a(echo_context);
    if (NULL == echo_context->m_post_mortem_event_file_handle_a)
    {
      LOGE("%s: Could not open post mortem event file A file", __FUNCTION__);
      retVal = false;
    }
    echo_context->m_post_mortem_active_file = 0;
    LOGD("%s: Swap to file A", __FUNCTION__);
  }

  if (false == retVal)
  {
    echo_post_mortem_close(echo_context);
    LOGE("%s: post mortem swap files failed. Recording is disabled", __FUNCTION__);
  }

  return(retVal);
}

/*==============================================================================
  FUNCTION:  echo_post_mortem_init
==============================================================================*/
static bool echo_post_mortem_init (EchoContext* echo_context,us_all_info* paramsStruct)
{
  bool retVal = true;
  FILE* fileHandle = NULL;

  echo_context->m_post_mortem_current_frame_count = 0;
  echo_context->m_post_mortem_max_frame_count = 0;
  echo_context->m_post_mortem_active_file = 0;
  echo_context->m_post_mortem_enable = false;
  echo_context->m_post_mortem_raw_file_handle_a = NULL;
  echo_context->m_post_mortem_raw_file_handle_b = NULL;
  echo_context->m_post_mortem_event_file_handle_a = NULL;
  echo_context->m_post_mortem_event_file_handle_b = NULL;

  /* parameters check - start */

  if (1 != paramsStruct->usf_post_mortem_enable)
  {
    return(retVal);
  }
  else
  {
    LOGI("%s: post mortem is enabled = %d",
         __FUNCTION__,paramsStruct->usf_post_mortem_enable);
    LOGI("%s: usf_post mortem_frame count is %d",
         __FUNCTION__,paramsStruct->usf_post_mortem_frame_count);
  }

  echo_context->m_post_mortem_max_frame_count =
      paramsStruct->usf_post_mortem_frame_count;
  if (echo_context->m_post_mortem_max_frame_count > POSTMORTEM_FRAMENUM_MAX)
  {
    echo_context->m_post_mortem_max_frame_count = POSTMORTEM_FRAMENUM_MAX;
    LOGW("%s, post mortem frame count = %d to large, truncated to =  %d",
         __FUNCTION__ ,paramsStruct->usf_post_mortem_frame_count,
         echo_context->m_post_mortem_max_frame_count);
  }
  /* parameters check - end */

  // get files handle
  echo_context->m_post_mortem_raw_file_handle_a =
  echo_post_mortem_get_raw_file_handle_a(echo_context);
  if (NULL == echo_context->m_post_mortem_raw_file_handle_a)
  {
    retVal = false;
  }

  echo_context->m_post_mortem_raw_file_handle_b =
  echo_post_mortem_get_raw_file_handle_b(echo_context);
  if (NULL == echo_context->m_post_mortem_raw_file_handle_b)
  {
    retVal = false;
  }

  echo_context->m_post_mortem_event_file_handle_a =
  echo_post_mortem_get_event_file_handle_a(echo_context);
  if (NULL == echo_context->m_post_mortem_event_file_handle_a)
  {
    retVal = false;
  }

  echo_context->m_post_mortem_event_file_handle_b =
  echo_post_mortem_get_event_file_handle_b(echo_context);
  if (NULL == echo_context->m_post_mortem_event_file_handle_b)
  {
    retVal = false;
  }

  if (true == retVal)
  {
    // leave open only A files
    ual_util_close_and_sync_file(echo_context->m_post_mortem_raw_file_handle_b);
    ual_util_close_and_sync_file(echo_context->m_post_mortem_event_file_handle_b);
    echo_context->m_post_mortem_raw_file_handle_b = NULL;
    echo_context->m_post_mortem_event_file_handle_b = NULL;

    echo_context->m_post_mortem_enable = true;
    LOGD("%s: post mortem init PASSED ", __FUNCTION__);
  }
  else
  {
    ual_util_close_and_sync_file(echo_context->m_post_mortem_raw_file_handle_a);
    ual_util_close_and_sync_file(echo_context->m_post_mortem_raw_file_handle_b);
    ual_util_close_and_sync_file(echo_context->m_post_mortem_event_file_handle_a);
    ual_util_close_and_sync_file(echo_context->m_post_mortem_event_file_handle_b);
    LOGD("%s: post mortem init FAILED ", __FUNCTION__);
  }

  return(retVal);
}

/*==============================================================================
  FUNCTION:  echo_post_mortem_write_frame_recording
==============================================================================*/
static void echo_post_mortem_write_frame_recording(EchoContext* echo_context,
                                                   uint8_t* dsp_raw_frame_ptr,
                                                   uint32_t frame_size_in_bytes,
                                                   uint8_t* dsp_event_frame_ptr,
                                                   uint32_t  dsp_event_size_in_bytes)
{
  FILE* raw_file;
  FILE* event_file;

 // Parameter check
  if (NULL == echo_context ||
      NULL == dsp_raw_frame_ptr ||
      NULL == dsp_event_frame_ptr)
  {
    LOGE("%s , Invalid arguments: "
         "echo_context = %p , dsp_raw_frame_ptr = %p ,dsp_event_frame_ptr = %p",
         __FUNCTION__ ,echo_context ,dsp_raw_frame_ptr, dsp_event_frame_ptr);
    if (echo_context != NULL) {
        echo_post_mortem_close(echo_context);
    }
    return;
  }
  if (frame_size_in_bytes > POSTMORTEM_BUFSIZE_MAX ||
     dsp_event_size_in_bytes > dsp_event_size_in_bytes)
  {
      LOGE("%s , Invalid arguments : frame_size_in_bytes = %d , dsp_event_size_in_bytes = %d",
            __FUNCTION__ , frame_size_in_bytes , dsp_event_size_in_bytes);
      echo_post_mortem_close(echo_context);
      return;
  }

  // check handles and set active files
  if (0 == echo_context->m_post_mortem_active_file)
  {
    if (NULL == echo_context->m_post_mortem_raw_file_handle_a ||
       NULL == echo_context->m_post_mortem_event_file_handle_a)
    {
      LOGE("%s , Invalid arguments : rawFileHandle_A = %p , evetFileHandle_A = %p",
           __FUNCTION__ , echo_context->m_post_mortem_raw_file_handle_a ,
           echo_context->m_post_mortem_event_file_handle_a);
      echo_post_mortem_close(echo_context);
      return;
    }
    raw_file = echo_context->m_post_mortem_raw_file_handle_a;
    event_file = echo_context->m_post_mortem_event_file_handle_a;
  }
  else
  {
    if (NULL == echo_context->m_post_mortem_raw_file_handle_b ||
       NULL == echo_context->m_post_mortem_event_file_handle_b)
    {
      LOGE("%s , Invalid arguments : rawFileHandle_B = %p , evetFileHandle_B = %p",
            __FUNCTION__ , echo_context->m_post_mortem_raw_file_handle_b ,
           echo_context->m_post_mortem_event_file_handle_b);
      echo_post_mortem_close(echo_context);
      return;
    }
    raw_file = echo_context->m_post_mortem_raw_file_handle_b;
    event_file = echo_context->m_post_mortem_event_file_handle_b;
  }

  // Write buffers to files
  if (fwrite(dsp_raw_frame_ptr,
             sizeof(uint8_t),
             frame_size_in_bytes,
             raw_file)<frame_size_in_bytes)
  {
    // no need to break since only one frame will be missing
    // and the user will get the logcat notification as well
    LOGE("%s: Could not write to recording file", __FUNCTION__);
  }

  if (fwrite(dsp_event_frame_ptr,
             sizeof(uint8_t),
             dsp_event_size_in_bytes,
             event_file)<dsp_event_size_in_bytes)
  {
    // no need to break since only one event will be missing
    // and the user will get the logcat notification as well
    LOGE("%s: Could not write to recording file", __FUNCTION__);
  }


  // check and switch between A and B bufers
  (echo_context->m_post_mortem_current_frame_count)++;
  if (echo_context->m_post_mortem_current_frame_count >= echo_context->m_post_mortem_max_frame_count)
  {
    echo_context->m_post_mortem_current_frame_count = 0;
    if (0 == echo_post_mortem_fileWriteSwap_util(echo_context))
    {
      LOGE("%s: post mortem swap files failed ", __FUNCTION__);
    }
  }
}

/*==============================================================================
  FUNCTION:  echo_params_init
==============================================================================*/
/**
  Init echoParam struct.
*/
static void echo_params_init (us_all_info *paramsStruct)
{
  uint32_t port;
  char *temp = NULL, *ip = NULL;
  int ret;

  if (paramsStruct->usf_event_dest & DEST_UAL)
  {
    echo_context.m_send_points_to_ual = true;
  }
  else
  {
    echo_context.m_send_points_to_ual = false;
  }

  if (paramsStruct->usf_event_dest & DEST_SOCKET)
  {
    echo_context.m_send_points_to_socket = true;
  }
  else
  {
    echo_context.m_send_points_to_socket = false;
  }

  echo_context.m_next_frame_seq_num = -1;
  echo_context.m_events = (usf_event_type *)malloc(sizeof(usf_event_type) *
                                           echo_context.m_echo_client->get_num_events());
  if (NULL == echo_context.m_events)
  {
    LOGE("%s: Failed to allocate",
         __FUNCTION__);
    echo_exit(EXIT_FAILURE);
  }

  echo_context.m_s_eventCounter = 0;

  echo_context.m_keys = (uint8_t *)malloc(sizeof(uint8_t) *
                                         echo_context.m_echo_client->get_num_keyboard_keys());
  if (NULL == echo_context.m_keys)
  {
    free(echo_context.m_events);
    LOGE("%s: Failed to allocate",
         __FUNCTION__);
    echo_exit(EXIT_FAILURE);
  }

  echo_context.m_echoStats.m_nPointsCalculated = 0;
  echo_context.m_echoStats.m_nTotalFrames = 0;
  echo_context.m_echoStats.m_nLostFrames = 0;
  echo_context.m_echoStats.m_nOutOfOrderErrors = 0;

  // Echo keys
  char usf_keys_cpy[FILE_PATH_MAX_LEN];
  memcpy(usf_keys_cpy,
         paramsStruct->usf_keys,
         FILE_PATH_MAX_LEN);

  if (false == ual_util_parse_string2array(echo_context.m_echo_client->get_num_keyboard_keys(),
                                           usf_keys_cpy,
                                           echo_context.m_keys))
  {
    echo_exit(EXIT_FAILURE);
  }

  echo_context.m_last_event_seq_num = 0;
  echo_context.m_stub = false;

  if (false == echo_post_mortem_init(&echo_context,paramsStruct))
  {
    LOGE("%s: Failed to init post mortem tool",__FUNCTION__);
    echo_exit(EXIT_FAILURE);
  }



  LOGI("%s: echo_params_init finished.",
       __FUNCTION__);
}



/*============================================================================
  FUNCTION:  add_event_key
============================================================================*/
/**
  Creates event and add it to the echo_context.m_events[].
*/
void add_event_key (int key,
                    int nPressure)
{
  // fill in the usf_event_type struct
  if (US_MAX_EVENTS <= echo_context.m_s_eventCounter)
  {
    LOGW("%s: Queue is full=%d, event would not be sent",
         __FUNCTION__,
         US_MAX_EVENTS);
    return;
  }

  usf_event_type *pEvent = &echo_context.m_events[echo_context.m_s_eventCounter];

  memset (pEvent, 0, sizeof (usf_event_type));

  pEvent->seq_num = echo_context.m_last_event_seq_num ++;
  pEvent->timestamp = (uint32_t)(clock ());
  pEvent->event_type_ind = USF_KEYBOARD_EVENT_IND;
  pEvent->event_data.key_event.key = key;
  pEvent->event_data.key_event.key_state = nPressure;

  LOGD("%s: Added event [%d]: key[%d] nPressure[%d]",
       __FUNCTION__,
       echo_context.m_s_eventCounter,
       key,
       nPressure);

  ++echo_context.m_s_eventCounter;
}

/*==============================================================================
  FUNCTION:  check_adapter_status
==============================================================================*/
/**
 * Checks the adapter status and acts accordingly.
 *
 * @return int 0 - Framework requested to carry on running.
 *             1 - Framework requested termination.
 */
static int check_adapter_status()
{
  if (NULL != echo_context.m_adapter)
  {
    switch (echo_context.m_adapter->get_status())
    {
    case DEACTIVATE:
      sb_run = false;
      return 0;
    case DISCONNECT:
      return 1;
    case ACTIVATE:
      return 0;
    case SHUTDOWN:
      return 1;
    default:
      LOGE("%s: invalid adapter status",
           __FUNCTION__);
      return -1;
    }
  }
  return 0;
}

/*==============================================================================
  FUNCTION:  update_pattern
==============================================================================*/
/**
  Writes pattern to the DSP
*/
static void update_pattern()
{
  LOGD("%s: Update pattern.",
       __FUNCTION__);

  // Pattern is transmitted only once. DSP transmits pattern in loop.
  int rc = ual_write((uint8_t *)echo_context.m_pattern,
                     echo_context.m_pattern_size);
  if (1 != rc)
  {
    LOGE("%s: ual_write failed.",
         __FUNCTION__);
    echo_exit(EXIT_FAILURE);
  }
}

/*==============================================================================
  FUNCTION:  print_transparent_data
==============================================================================*/
/**
  Prints 0-fill_in_index values of transparent data.
*/
static void print_transparent_data(us_all_info paramsStruct, int print_till_index)
{
  for (int i = 0; i < print_till_index; i++)
  {
    LOGD("[%d]%x",
         i,
         paramsStruct.usf_tx_transparent_data[i]);
  }
}

/*==============================================================================
  FUNCTION:  inject_config_params_to_tx_transparent_data
==============================================================================*/
/**
  Injects mic distances, output type and group factor to the tx transparent data.
*/
static void inject_config_params_to_tx_transparent_data(us_all_info *paramsStruct)
{
  // Inject skip and group factor to transparent data
  uint32_t skip_index = BYTE_OFFSET_OF_SKIP_IN_TX_TRANSPARENT;
  uint32_t group_index = BYTE_OFFSET_OF_GROUP_IN_TX_TRANSPARENT;
  if (ual_util_inject_to_trans_data(paramsStruct->usf_tx_transparent_data,
                                    &skip_index,
                                    FILE_PATH_MAX_LEN,
                                    paramsStruct->usf_tx_skip,
                                    sizeof(uint16_t)))
  {
    echo_exit(EXIT_FAILURE);
  }
  if (ual_util_inject_to_trans_data(paramsStruct->usf_tx_transparent_data,
                                    &group_index,
                                    FILE_PATH_MAX_LEN,
                                    paramsStruct->usf_tx_group,
                                    sizeof(uint16_t)))
  {
    echo_exit(EXIT_FAILURE);
  }

  float mic_info[COORDINATES_DIM], spkr_info[COORDINATES_DIM];
  // Required for ual_util_get_mic_config & ual_util_get_speaker_config
  if (-1 == ual_util_prefill_ports_num_and_id(paramsStruct))
  {
    LOGE("%s: ual_util_prefill_ports_num_and_id failed.",
         __FUNCTION__);
    echo_exit(EXIT_FAILURE);
  }

  // Get speaker info - 0 is for the first spkr since we only have one spkr
  if (-1 == ual_util_get_speaker_config(0, spkr_info))
  {
    LOGE("%s: ual_util_get_speaker_config for speaker failed.",
         __FUNCTION__);
    echo_exit(EXIT_FAILURE);
  }

  for (int i = 0; i < paramsStruct->usf_tx_port_count; i++)
  {
    // Get current mic info
    if (-1 == ual_util_get_mic_config(i, mic_info))
    {
      LOGE("%s: get_mic_config for mic failed for mic #%d.",
           __FUNCTION__,
           i);
      echo_exit(EXIT_FAILURE);
    }

    // Calculate distance between speaker and the curr mic
    int distance = (int)(sqrt((mic_info[X_IND] - spkr_info[X_IND]) *
                              (mic_info[X_IND] - spkr_info[X_IND]) +
                              (mic_info[Y_IND] - spkr_info[Y_IND]) *
                              (mic_info[Y_IND] - spkr_info[Y_IND]))/10);
    LOGD("%s: mic #%d distance = %d mm",
         __FUNCTION__,
         i,
         distance);

    // Update TX transparent data
    if (ual_util_inject_to_trans_data(paramsStruct->usf_tx_transparent_data,
                                      &(paramsStruct->usf_tx_transparent_data_size),
                                      FILE_PATH_MAX_LEN,
                                      distance,
                                      sizeof(int)))
    {
      echo_exit(EXIT_FAILURE);
    }
  }

  // Fill in other mic (which do not exist on current configuration) with zeros.
  for (int i = paramsStruct->usf_tx_port_count; i < MAX_SUPPORTED_MIC; i++)
  {
    // Update TX transparent data
    if (ual_util_inject_to_trans_data(paramsStruct->usf_tx_transparent_data,
                                      &(paramsStruct->usf_tx_transparent_data_size),
                                      FILE_PATH_MAX_LEN,
                                      0,
                                      sizeof(int)))
    {
      echo_exit(EXIT_FAILURE);
    }
  }
  // Fill in tx transparent data with echo output type.
  if (ual_util_inject_to_trans_data(paramsStruct->usf_tx_transparent_data,
                                    &paramsStruct->usf_tx_transparent_data_size,
                                    FILE_PATH_MAX_LEN,
                                    paramsStruct->usf_output_type,
                                    sizeof(int)))
  {
    echo_exit(EXIT_FAILURE);
  }

  LOGD("%s: Transparent after injection: size: %d content:",
       __FUNCTION__,
       paramsStruct->usf_tx_transparent_data_size);
  print_transparent_data(*paramsStruct,
                         paramsStruct->usf_tx_transparent_data_size);
}

/*==============================================================================
  FUNCTION:  calculate_statistics
==============================================================================*/
/**
  Calculates statistics about the frames received, and prints them.
*/
static void calculate_statistics(us_all_info *paramsStruct,
                                 uint8_t* nextFrame,
                                 int *packetCounter)
{
  // Statistics
  int seqNum;
  if (paramsStruct->usf_output_type & echo_context.m_echo_client->get_event_output_type())
  {
    if (paramsStruct->usf_output_type & OUTPUT_TYPE_RAW_MASK)
    {
      // We have echo event followed by raw data, take the sequence number from the raw data.
      // The sequence number in the echo event can be out of order when echo library is busy
      // in the ADSP
      seqNum = *((int*)(nextFrame + echo_context.m_echo_client->get_dsp_event_size() + ECHO_FRAME_SEQNUM_OFFSET));
    }
    else
    {
      // Getting sequence number from event header
      seqNum = *((int *)(nextFrame + DSP_EVENT_SEQNUM_OFFSET));
    }
  }
  else
  { // Getting sequence number from raw data header file
    seqNum = *((int *)(nextFrame + ECHO_FRAME_SEQNUM_OFFSET));
  }
  // If this is the first iteration then the frames
  // counter is -1 and we need to update the frames counter.
  if (echo_context.m_next_frame_seq_num == -1)
  {
    LOGD(" %s First iteration, seq: %d",
         __FUNCTION__,
         seqNum);
    echo_context.m_next_frame_seq_num = seqNum;
  }
  // This is not the first iteration.
  else
  {
    if (echo_context.m_next_frame_seq_num != seqNum)
    {
      // We lost some frames so we add the number of lost frames
      // to the statistics.
      if (echo_context.m_next_frame_seq_num < seqNum)
      {
        echo_context.m_echoStats.m_nLostFrames +=
          (seqNum - echo_context.m_next_frame_seq_num)/
          paramsStruct->usf_tx_skip;
        LOGD("%s Lost frames: expected %d received: %d",
             __FUNCTION__,
             echo_context.m_next_frame_seq_num, seqNum);
      }
      // We got out of order frames so we add the number of
      // out of order frames to the statistics.
      else
      {
        echo_context.m_echoStats.m_nOutOfOrderErrors +=
          (echo_context.m_next_frame_seq_num - seqNum)/
          paramsStruct->usf_tx_skip;
      }

      // Update the frames counter to the correct count.
      echo_context.m_next_frame_seq_num = seqNum;
    }
  }
  echo_context.m_echoStats.m_nTotalFrames++;
  // Update the frames counter to the expected count in the next
  // iteration.
  echo_context.m_next_frame_seq_num += paramsStruct->usf_tx_skip;

  *packetCounter = *packetCounter + 1;
  if (STATISTIC_PRINT_INTERVAL == *packetCounter)
  {
    LOGI("%s: Statistics (printed every %d frames):",
         __FUNCTION__,
         STATISTIC_PRINT_INTERVAL);
    LOGI("Points calculated: %d, total frames: %d, lost frames: %d,"
         "out of order: %d",
         echo_context.m_echoStats.m_nPointsCalculated,
         echo_context.m_echoStats.m_nTotalFrames,
         echo_context.m_echoStats.m_nLostFrames,
         echo_context.m_echoStats.m_nOutOfOrderErrors);
    *packetCounter = 0;
  }
}

/*==============================================================================
  FUNCTION:  signal_handler
==============================================================================*/
/**
  Perform clean exit after receive signal.
*/
static void signal_handler(int sig)
{
  LOGD("%s: Received signal %d; sb_run=%d, tid=%d",
         __FUNCTION__, sig, sb_run, gettid());

  if (SIGALRM == sig)
  {
    return;
  }

  // All supported signals cause the daemon exit
  sb_run = false;
  daemon_run = false;

  // Raise ALARM to unblock UAL read
  alarm(1);
}

/*==============================================================================
  FUNCTION:  setup_signal_handlers
==============================================================================*/
/**
  Configures the signal handlers being used by the echo daemon
*/
static void setup_signal_handlers()
{
  signal(SIGHUP,
         signal_handler);
  signal(SIGTERM,
         signal_handler);
  signal(SIGINT,
         signal_handler);
  signal(SIGQUIT,
         signal_handler);
  signal(SIGALRM,
         signal_handler);
}

/*==============================================================================
  FUNCTION:  save_persistent_data_to_file
==============================================================================*/
/**
 * Reads TX params and saves it to file
 */
static void save_persistent_data_to_file(us_all_info *paramsStruct)
{
  echo_context.m_echo_client->save_persistent_data(&echo_context, paramsStruct);
}

/*============================================================================
  FUNCTION:  calib_save_thread
============================================================================*/
/**
 * A thread that saves persist data periodically
 */
static void *calib_save_thread(void *paramsStructPtr) {
  us_all_info *paramsStruct = (us_all_info*) paramsStructPtr;
  const int SLEEP_TIME_SEC = paramsStruct->usf_persist_save_timeout /* 10 sec */;
  struct timespec wait_time;
  int rc;

  while (sb_run) {
    clock_gettime(CLOCK_REALTIME, &wait_time);
    wait_time.tv_sec = wait_time.tv_sec + SLEEP_TIME_SEC;
    rc = sem_timedwait(&s_calib_thread_sem, &wait_time);

    if (!rc) {
      // posted semaphore is the signal to exit
      break;
    } else if (ETIMEDOUT == errno) {
      save_persistent_data_to_file(paramsStruct);
    } else {
      LOGD("%s: sem_timedwait error, rc: %d, errno: %d",
           __FUNCTION__,
           rc,
           errno);
      break;
    }
  }

  LOGD("%s: calib save thread exit, save persistent data", __FUNCTION__);
  save_persistent_data_to_file(paramsStruct);
  return NULL;
}

/*==============================================================================
  FUNCTION:  echo_init
==============================================================================*/
/**
  Inits the sync echo daemon parameters, signal handlers, adapter etc`.
*/
static void echo_init(us_all_info *paramsStruct)
{
  s_main_thread = pthread_self();

  // Setup signal handling
  setup_signal_handlers();

  if (0 != property_thread_create(echo_context.m_echo_client->get_daemon_name()))
  {
    LOGE("%s: Creating property thread failed",
         __FUNCTION__);
    echo_exit(EXIT_FAILURE);
  }

  if (false == ual_util_is_supported(echo_context.m_echo_client->get_client_name()))
  {
    LOGE("%s: Daemon is not supported",
         __FUNCTION__);
    echo_exit(EXIT_FAILURE);
  }

  if (ual_util_declare_pid(getpid(),
                           echo_context.m_echo_client->get_pid_file_name()))
  {
    LOGE("%s: Declare_pid failed",
         __FUNCTION__);
  }

  if (ual_util_daemon_init(paramsStruct,
                           echo_context.m_echo_client->get_cfg_path(),
                           cfgFile,
                           echo_context.m_echo_client->get_client_name()))
  {
    LOGE("%s: ual_util init failed",
         __FUNCTION__);
    echo_exit(EXIT_FAILURE);
  }

  // Get suitable adapter
  if (0 != create_adapter(&echo_context.m_adapter,
                          paramsStruct->usf_adapter_lib,
                          paramsStruct->usf_gesture_fw_lib))
  {
    LOGE("%s: create_adapter failed",
         __FUNCTION__);
    echo_exit(EXIT_FAILURE);
  }

  if (0 > sem_init(&s_calib_thread_sem, 0, 0))
  {
    LOGE("%s:  Could not init semaphore for calib thread; errno=%d",
         __FUNCTION__,
         errno);
    echo_exit(EXIT_FAILURE);
  }

  if (pthread_create(&s_calib_thread, NULL, calib_save_thread, (void *) paramsStruct))
  {
    LOGE("%s:  calib_thread create failure; errno=%d",
             __FUNCTION__,
             errno);
    sem_destroy(&s_calib_thread_sem);
    echo_exit(EXIT_FAILURE);
  }

  ual_util_print_US_version(echo_context.m_echo_client->get_client_name(),
                            CLIENT_VERSION);

  inject_config_params_to_tx_transparent_data(paramsStruct);

  ual_util_set_buf_size(&paramsStruct->usf_tx_buf_size,
                        paramsStruct->usf_tx_port_data_size,
                        paramsStruct->usf_tx_sample_width,
                        paramsStruct->usf_tx_port_count,
                        paramsStruct->usf_tx_frame_hdr_size,
                        paramsStruct->usf_tx_group,
                        "tx",
                        (paramsStruct->usf_output_type & echo_context.m_echo_client->get_event_output_type()),
                        (paramsStruct->usf_output_type & OUTPUT_TYPE_RAW_MASK),
                        echo_context.m_echo_client->get_dsp_event_size());

  // Build rx_transparent_data manually. In the future (hopefully),
  // this will be removed.
  ual_util_set_echo_rx_transparent_data(paramsStruct);

  ual_util_set_rx_buf_size(paramsStruct);

  if (sem_init(&s_rx_release_complete, 0, 0))
  {
    LOGE("%s: could not initialize rx_release_complete sempahore, %d, %s",
           __FUNCTION__,
           errno,
           strerror(errno));

    echo_exit(EXIT_FAILURE);
  }
}

/*==============================================================================
  FUNCTION:  adapter_control
==============================================================================*/
/**
  Finds the next adapter command and acts accordingly.
*/
static void adapter_control()
{
  int wait_status = (NULL != echo_context.m_adapter) ? echo_context.m_adapter->wait_n_update() : 0;
  switch (wait_status)
  {
  case ACTIVATE:
    sb_run = true;
    break;
  case SHUTDOWN:
    echo_exit(EXIT_SUCCESS);
    break;
  case FAILURE:
  case INTERRUPT:
  default:
    echo_exit(EXIT_FAILURE);
  }
}

/*==============================================================================
  FUNCTION:  init_frame_recording
==============================================================================*/
/**
 * Inits frame file recording
 *
 * @param paramsStruct - Daemon configuration parameters
 * @param frame_file - The file to hold the frame recording
 */
static void init_frame_recording(us_all_info const *paramsStruct)
{
  if (0 >= paramsStruct->usf_frame_count)
  {
    LOGD("%s: usf_frame_count is %d. Frames will not be recorded.",
         __FUNCTION__,
         paramsStruct->usf_frame_count);
    return;
  }
  // frame_file is not yet allocated
  if (NULL == frame_file)
  {
    LOGD("%s, Opening frame file",
         __FUNCTION__);
    // Open frame file from cfg file
    frame_file = ual_util_get_frame_file(paramsStruct,
                                         echo_context.m_echo_client->get_frame_file_path());
    if (NULL == frame_file)
    {
      LOGE("%s: ual_util_get_frame_file failed",
           __FUNCTION__);
      echo_exit(EXIT_FAILURE);
    }
  }
}

/*==============================================================================
  FUNCTION:  write_frame_recording
==============================================================================*/
/**
  Writes frame recording and closes file when finished.
*/
static void write_frame_recording(us_all_info const *paramsStruct,
                                  uint32_t *numOfBytes,
                                  uint32_t bytesWriteToFile,
                                  int group_data_size,
                                  uint8_t *pGroupData)
{
  if (*numOfBytes < bytesWriteToFile)
  {
    uint32_t bytestFromGroup =
      (*numOfBytes + group_data_size <= bytesWriteToFile) ?
      group_data_size :
      bytesWriteToFile - *numOfBytes;

    if (paramsStruct->usf_output_type & echo_context.m_echo_client->get_event_output_type())
    {
      // Skip dsp events when recording
      pGroupData += echo_context.m_echo_client->get_dsp_event_size();
    }

    ual_util_frame_file_write(pGroupData,
                              sizeof(uint8_t),
                              bytestFromGroup,
                              paramsStruct,
                              frame_file);

    *numOfBytes += bytestFromGroup;
    if (*numOfBytes >= bytesWriteToFile)
    {
      ual_util_close_and_sync_file(frame_file);
      frame_file = NULL;
    }
  }
}

/*==============================================================================
  FUNCTION:  echo_run
==============================================================================*/
/**
  Main running loop of the sync echo daemon.
*/
static void echo_run(us_all_info *paramsStruct)
{
  int packetCounter = 0;
  ual_data_type data;
  uint32_t numOfBytes = 0, bytestFromRegion = 0;

  uint8_t* dsp_event_frame_ptr;
  uint8_t* dsp_raw_frame_ptr;
  uint32_t dsp_event_size_in_bytes;


  uint32_t frame_hdr_size_in_bytes = paramsStruct->usf_tx_frame_hdr_size;

  uint32_t packet_size_in_bytes = paramsStruct->usf_tx_port_data_size *
    sizeof(paramsStruct->usf_tx_sample_width);

  uint32_t packets_in_frame = paramsStruct->usf_tx_port_count;

  uint32_t frame_size_in_bytes = packet_size_in_bytes * packets_in_frame +
    frame_hdr_size_in_bytes;

  int combined_frame_size = 0;
  if (paramsStruct->usf_output_type & OUTPUT_TYPE_RAW_MASK)
  {
    combined_frame_size += frame_size_in_bytes;
  }
  if (paramsStruct->usf_output_type & echo_context.m_echo_client->get_event_output_type())
  {
    combined_frame_size += echo_context.m_echo_client->get_dsp_event_size();
  }

  dsp_event_size_in_bytes = echo_context.m_echo_client->get_dsp_event_size();

  int num_of_regions = sizeof(data.region) / sizeof(ual_data_region_type);
  uint32_t bytesWriteToFile = paramsStruct->usf_frame_count *
                              frame_size_in_bytes;

  init_frame_recording(paramsStruct);

  // always send earpiece active state before daemon starts running
  echo_rx_notify_active_earpiece_state(paramsStruct, true);


  LOGD("%s: Start echo calculator internal loop", __FUNCTION__);

  // Must add daemon_run flag as well, in case signal is received
  // right before assigning sb_run = true in the above switch.
  while (sb_run &&
         daemon_run)
  {
    if (0 != check_adapter_status())
    {
      LOGE("%s: Framework requested termination",
           __FUNCTION__);
      echo_exit(EXIT_FAILURE);
    }

    uint8_t* nextFrame = NULL;

    // do not use usf driver default timeout, it causes uninterruptible sleep
    uint32_t timeout = UAL_READ_DEFAULT_TIMEOUT_SEC;
    // If the library work mode is DSP and the output is event only
    // (without raw data), then ual_read is blocking until echo event.
    // If the framework sends disconnect during the ual_read, the daemon
    // will check it only in the next echo event. Therefore a
    // short timeout for ual_read is needed if working with the framework.
    if (!(paramsStruct->usf_output_type & OUTPUT_TYPE_RAW_MASK) &&
        (NULL == echo_context.m_adapter))
    { // In events only mode, need to wait infinitely for events.
      timeout = USF_INFINITIVE_TIMEOUT;
    }

    // in case we need to release RX (arbitration), break here before
    // entering ual_read. This is an optimization
    if(sb_rx_release_requested)
    {
      break;
    }

    if (!ual_read(&data,
                  echo_context.m_events,
                  echo_context.m_s_eventCounter,
                  timeout))
    {
      LOGE("%s: ual read failed",
           __FUNCTION__);
      // Breaking the while will make the daemon re-allocate resources
      break;
    }

    echo_rx_notify_active_earpiece_state(paramsStruct, false);

    echo_context.m_s_eventCounter = 0;
    if (0 == data.region[0].data_buf_size)
    {
      continue;
    }

    // Underlay layer provides US data frames in buffers.
    // Each buffer includes one group of the frames.
    // A number of frames is defined by configurable group factor.
    int numberOfFrames = paramsStruct->usf_tx_group;
    if ((combined_frame_size * numberOfFrames) > (int)paramsStruct->usf_tx_buf_size)
    {
      LOGE("%s: tx_buf_size: %d (aligned) doesn't contain expected size: %d",
           __FUNCTION__,
           paramsStruct->usf_tx_buf_size,
           (combined_frame_size * numberOfFrames));
      echo_exit(EXIT_FAILURE);
    }

    int group_data_size = numberOfFrames * frame_size_in_bytes;

    for (int r = 0; r < num_of_regions; r++)
    {
      int num_of_groups = data.region[r].data_buf_size /
                          paramsStruct->usf_tx_buf_size;
      uint8_t *pGroupData = data.region[r].data_buf;
      for (int g = 0; g < num_of_groups; g++)
      {
        nextFrame =  pGroupData;
        dsp_event_frame_ptr = nextFrame;
        dsp_raw_frame_ptr = nextFrame;
        if (paramsStruct->usf_output_type & echo_context.m_echo_client->get_event_output_type())
        {
          dsp_raw_frame_ptr += dsp_event_size_in_bytes;
        }
        // Recording
        write_frame_recording(paramsStruct,
                              &numOfBytes,
                              bytesWriteToFile,
                              group_data_size,
                              pGroupData);

        for (int f = 0; f < numberOfFrames ; f++)
        {
          // Post mortem recording
          if (true == echo_context.m_post_mortem_enable)
          {
            echo_post_mortem_write_frame_recording(&echo_context,
                                                   dsp_raw_frame_ptr,
                                                   frame_size_in_bytes,
                                                   dsp_event_frame_ptr,
                                                   dsp_event_size_in_bytes);
          }

          calculate_statistics(paramsStruct, nextFrame, &packetCounter);
          // Calculation
          echo_context.m_echo_client->get_points(&echo_context,
                                                (short *)nextFrame,
                                                paramsStruct);
          nextFrame += combined_frame_size;
        } // f (frames) loop
        pGroupData += paramsStruct->usf_tx_buf_size;
      } // g (groups) loop
    } // r (regions) loop
  } // main loop

  echo_post_mortem_close(&echo_context);

}

/*==============================================================================
  FUNCTION:  init
==============================================================================*/
/**
  General initialization.
*/
static void init(us_all_info *paramsStruct)
{
  adapter_control();

  if (ual_util_ual_open_retries(paramsStruct))
  {
    LOGE("%s: ual_util_ual_open_retries failed",
          __FUNCTION__);
    echo_exit(EXIT_FAILURE);
  }

  paramsStruct->usf_event_type = (paramsStruct->usf_event_dest & DEST_UAL) ?
                                 USF_KEYBOARD_EVENT :
                                 USF_NO_EVENT;

  if (ual_util_tx_config(paramsStruct,
                         echo_context.m_echo_client->get_client_name()))
  {
    LOGE("%s: ual_util_tx_config failed.",
         __FUNCTION__);
    echo_exit(EXIT_FAILURE);
  }

  echo_context.m_echo_client->set_algo_dynamic_params(paramsStruct,
                                                      &echo_context);

  aud_dev_arbi_event_cb_t rx_arbi_cb = NULL;
  if (paramsStruct->usf_rx_dev_arbitration == 1)
  {
      LOGD("%s: rx arbitration in use", __FUNCTION__);
      rx_arbi_cb = echo_rx_device_state_cb;
  }
  if (ual_util_rx_config(paramsStruct,
                         echo_context.m_echo_client->get_client_name(),
                         rx_arbi_cb))
  {
    LOGE("%s: ual_util_rx_config failed.",
         __FUNCTION__);
    echo_exit(EXIT_FAILURE);
  }

  echo_params_init(paramsStruct);

  if ((paramsStruct->usf_output_type & OUTPUT_TYPE_RAW_MASK) &&
      !paramsStruct->usf_app_lib_bypass)
  {
    echo_context.m_echo_client->echo_lib_init(paramsStruct,
                                              &echo_context);
    update_pattern();
  }
  else
  {
    LOGI("%s: Sync Echo bypass APPS lib",
         __FUNCTION__);
  }
}

/*==============================================================================
  FUNCTION:  handle_rx_release_request
==============================================================================*/
/**
  Release Rx resources and notify initiating thread the Rx is available for
  audio use.
*/
static int handle_rx_release_request()
{
  LOGD("%s: Enter",
       __FUNCTION__);

  // Stop Rx and notify when it is stopped using the sempahore
  if (!ual_stop_RX(false))
  {
    LOGE("%s:  Could not stop RX",
        __FUNCTION__);
  }

  if (sb_rx_release_requested)
  {
    sb_rx_release_requested = false;
    if (sem_post(&s_rx_release_complete) < 0)
    {
        LOGE("%s: error signaling rx release done semaphore",
             __FUNCTION__);
    }
  }

  // Note that we don't call ual_close() since we want to continue
  // monitoring the Rx state
  if (!ual_stop_TX())
  {
    LOGE("%s:  Could not stop TX",
        __FUNCTION__);
  }

  return 0;
}

/*==============================================================================
  FUNCTION:  echo_calculator_start
==============================================================================*/
/**
 * See function description in header file
 */
void echo_calculator_start (EchoClient *echo_client)
{
  int ret;
  static us_all_info paramsStruct;
  bool rc = false;

  echo_context.m_echo_client = echo_client;

  LOGI("%s: Sync Echo start",
       __FUNCTION__);

  echo_init(&paramsStruct);

  while (daemon_run)
  {
    // Clear pending alarm signal that might disrupt this function work.  Alarm
    // signal is used to release daemons from blocking functions.
    alarm(0);

    init(&paramsStruct);

    echo_run(&paramsStruct);

    if (!daemon_run)
    {
      // Exit requested by signal
      echo_exit(EXIT_SUCCESS);
    }

    if ((NULL != echo_context.m_adapter) &&
          (echo_context.m_adapter->get_status() == ACTIVATE))
    {   // Disconnect only while in active state, it's not necessary in other
        // states.
        echo_context.m_adapter->disconnect();
    }

    handle_rx_release_request();

    echo_free_resources(false);

  } // End daemon_run

  echo_exit(EXIT_SUCCESS);
}

/*==============================================================================
  FUNCTION:  echo_calculator_stop
==============================================================================*/
/**
 * See function description in header file
 */
void echo_calculator_stop ()
{
  daemon_run = false;
  raise(SIGALRM); // Raise ALARM to unblock UAL read
}
