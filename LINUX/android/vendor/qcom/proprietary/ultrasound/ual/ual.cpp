/*===========================================================================
                           ual.cpp

DESCRIPTION: Ultrasound Abstract Layer (UAL) implementation

Copyright (c) 2011-2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/


/*----------------------------------------------------------------------------
  Include files
----------------------------------------------------------------------------*/
#define LOG_TAG "UAL"
#include "usf_log.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <sys/stat.h>
#include <semaphore.h>
#include "usf.h"
#include "ual.h"

/*----------------------------------------------------------------------------
  Macros and constants
----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
  Static Variable Definitions
-----------------------------------------------------------------------------*/
// The required prefix of the calculator name
static  const char* UAL_CALC_NAME_PREFIX = "USF_";
// The calculator name
static  char s_calc_name[USF_MAX_CLIENT_NAME_SIZE];

// UAL version
static const char* UAL_VERSION = "1.7.1";

// Supported USF version
static const char* SUPPORTED_USF_VERSION = "1.7";

/* Version buffer size */
static const uint16_t USF_VERSION_SIZE = 20;

// The current USF version
static  char s_usf_version[USF_VERSION_SIZE];

// "IF with the USF" file descriptor
static  int s_usf_fd = -1;

// Supported mode of the USF work activity
// It mainly used for USF power consuption studies
static  ual_work_mode_type  s_work_mode = UAL_MODE_STANDARD;

// Sleep time (sec) in the case of "UAL_MODE_IDLE_USF_DATA_PATH" or
// "UAL_MODE_IDLE_ALL_DATA_PATH" work modes
// during "read data"
static const uint16_t  UAL_MODE_IDLE_DATA_SLEEP_TIME = 30;

// Selected TX sound device ID
static  int s_tx_dev_id = -1;

// One buffer (in the cyclic queue) is for one group of the US frames
// The US data buffer size (bytes) in the cyclic queue
static uint32_t  s_tx_buf_size = 0;
// Number of the buffers in the cyclic queue
static uint16_t  s_tx_buf_number = 0;

// TX US data memory (shared with the USF) size
static uint32_t s_tx_alloc_size = 0;

// TX US data memory, shared with the USF
static uint8_t* s_tx_user_addr = NULL;

// Free region (in the cyclic queue) for writing by the USF
// Pointer (read index) to the end of available region
// in the shared US data memory
static uint16_t s_tx_read_index = 0;

// Ready region (in the cyclic queue) with US data for a client
// Pointer (write index) to the end of ready US data region
// in the shared memory
static uint16_t s_tx_write_index = 0;

// Selected RX sound device ID
static  int s_rx_dev_id = -1;

// One buffer (in the cyclic queue) is for one group of the US frames
// The US data buffer size (bytes) in the cyclic queue
static uint32_t  s_rx_buf_size = 0;
// Number of the buffers in the cyclic queue
static uint16_t  s_rx_buf_number = 0;
// RX ramp down timeout
static int s_rx_ramp_down_timeout = 0;

// RX US data memory (shared with the USF) size
static uint32_t s_rx_alloc_size = 0;
// RX US data memory, shared with the USF
static uint8_t* s_rx_user_addr = NULL;

// Free region (in the cyclic queue) for writing by client
static uint16_t s_rx_read_index = 0;
// Ready region (in the cyclic queue) for US data transmition
static uint16_t s_rx_write_index = 0;

// Events (from conflicting devs) to be disabled/enabled
static uint16_t s_event_filters;

// The UAL status
static uint32_t s_ual_status = 0;

// RX path control mutex
static pthread_mutex_t s_ual_RX_control_mutex = PTHREAD_MUTEX_INITIALIZER;

// TX path control mutex
static pthread_mutex_t s_ual_TX_control_mutex = PTHREAD_MUTEX_INITIALIZER;

//  Mutex, controlling the usf driver open/close
static pthread_mutex_t s_ual_control_mutex = PTHREAD_MUTEX_INITIALIZER;

// Number of audio devices we support for RX arbitration
static const uint32_t RX_DEV_ARBI_NUM_SUPPORTED_DEVICES = 3;

// Array of audio devices we can support for RX arbitration
// some or all of these devices can be enabled for arbitration
// using configuration parameters.
static const audio_devices_t s_rx_dev_arbi_supported_devices[
  RX_DEV_ARBI_NUM_SUPPORTED_DEVICES] =
  {AUDIO_DEVICE_OUT_EARPIECE, AUDIO_DEVICE_OUT_SPEAKER,
   AUDIO_DEVICE_OUT_WIRED_HEADPHONE};

// Array of RX mixer suffixes for adjusting mixer path based on active audio
// device. Must match the devices in s_rx_dev_arbi_supported_devices above
static const char *s_rx_dev_arbi_topology_suffixes[
  RX_DEV_ARBI_NUM_SUPPORTED_DEVICES] = {"_earpiece", "_speaker", "_headphones"};

// array of audio devices enabled for RX arbitration
static audio_devices_t s_rx_dev_arbi_enabled_devices[
  RX_DEV_ARBI_NUM_SUPPORTED_DEVICES];

// number of enabled audio devices for RX arbitration
// stored in the above s_rx_dev_arbi_enabled_devices array
static uint32_t s_rx_dev_arbi_num_enabled_devices = 0;

// The audio device mixer path used by default when there are no active audio
// devices
static audio_devices_t s_rx_dev_arbi_default_topology_device =
    AUDIO_DEVICE_OUT_EARPIECE;

// Current active audio devices. used for RX arbitration book-keeping
static audio_devices_t s_rx_dev_arbi_active_devices = AUDIO_DEVICE_NONE;

// The audio device that the current ultrasound mixer path is compatible with,
// used for device switch decision
static audio_devices_t s_rx_dev_arbi_compat_us_device = AUDIO_DEVICE_NONE;

// Pointer to RX arbitration state callback supplied by daemon
// if NULL RX arbitration is disabled
static aud_dev_arbi_event_cb_t s_rx_dev_arbi_state_evt_cb = NULL;

/*------------------------------------------------------------------------------
  Function declarations
------------------------------------------------------------------------------*/
/*==============================================================================
  FUNCTION:  ual_alsa_open_device
==============================================================================*/
/**
  The function opens sound device - configures the codec
*/
extern int ual_alsa_open_device
(
  // Configuration struct
  us_xx_info_type& us_xx_info,
  // Direction (Tx, Rx)
  ual_direction_type direction
);

/*==============================================================================
  FUNCTION:  ual_alsa_open_device_with_mux
==============================================================================*/
/**
  The function opens sound device - configures the codec
  Call this function in case of MUXed setup. Pass the number of physical channels
  (before deMUXing) in num_actual_channels. -1 means use the port_cnt in
  us_xx_info
*/
extern int ual_alsa_open_device_with_mux
(
  // Configuration struct
  us_xx_info_type& us_xx_info,
  // Direction (Tx, Rx)
  ual_direction_type direction,
  // Number of physical TX channels before deMUXing
  int32_t num_actual_channels
);

/*==============================================================================
  FUNCTION:  ual_alsa_close_device
==============================================================================*/
/**
  The function closes the device - releases the codec resources
*/
extern bool ual_alsa_close_device
(
  // Direction (Tx, Rx)
  ual_direction_type direction,
  // RX Ramp down timeout
  int rx_ramp_down_timeout
);

/*==============================================================================
  FUNCTION:  ual_alsa_open
==============================================================================*/
/**
  The function opens the ual_alsa sub-system
*/
extern bool ual_alsa_open();

/*==============================================================================
  FUNCTION:  ual_alsa_close
==============================================================================*/
/**
  The function closes the ual_alsa sub-system - releases the local resources
*/
extern void ual_alsa_close();

/*==============================================================================
  FUNCTION:  ual_alsa_set_rx_topology_suffix
==============================================================================*/
/**
  The function sets a suffix that will be added to RX topology name when
  mixer path is configured. This allows selecting a different RX mixer path
  based on runtime configuration. Set to NULL or empty string to avoid adding
  any suffix (NULL is the default value)
  It is part of the earpiece concurrency feature
 */
extern void ual_alsa_set_rx_topology_suffix(const char *suffix);

/*==============================================================================
  FUNCTION:  ual_aud_dev_arbi_init
==============================================================================*/
extern int ual_aud_dev_arbi_init
(
  aud_dev_arbi_event_cb_t evt_cb
);

/*==============================================================================
  FUNCTION:  ual_aud_dev_arbi_deinit
==============================================================================*/
extern int ual_aud_dev_arbi_deinit
(
  void
);

/*==============================================================================
  FUNCTION:  ual_aud_dev_arbi_register
==============================================================================*/
extern int ual_aud_dev_arbi_register
(
  aud_dev_arbi_client_t client,
  audio_devices_t aud_dev
);

/*==============================================================================
  FUNCTION:  ual_aud_dev_arbi_acquire
==============================================================================*/
extern int ual_aud_dev_arbi_acquire
(
  audio_devices_t aud_dev,
  aud_dev_arbi_client_cmd_result_cb_t result_cb,
  uint32_t user_data
);

/*==============================================================================
  FUNCTION:  ual_aud_dev_arbi_release
==============================================================================*/
extern int ual_aud_dev_arbi_release
(
  audio_devices_t aud_dev,
  aud_dev_arbi_client_cmd_result_cb_t result_cb,
  uint32_t user_data
);

/*==============================================================================
  FUNCTION:  ual_rx_dev_arbi_is_ultrasound_topology_possible
==============================================================================*/
static bool ual_rx_dev_arbi_is_ultrasound_topology_possible
(
  audio_devices_t activeDevices
);

/*==============================================================================
  FUNCTION:  ual_rx_dev_arbi_state_cb_wrapper
==============================================================================*/
static void ual_rx_dev_arbi_state_cb_wrapper
(
  audio_devices_t device,
  aud_dev_arbi_event_t evt,
  aud_dev_arbi_event_cb_resp_flags_t *respFlags
);

/*==============================================================================
  FUNCTION:  ual_rx_dev_arbi_acquire_cmd_result_cb
==============================================================================*/
static void ual_rx_dev_arbi_acquire_cmd_result_cb
(
  audio_devices_t device,
  bool success,
  uint32_t userData
);

/*------------------------------------------------------------------------------
  Function definitions
------------------------------------------------------------------------------*/
/*==============================================================================
  FUNCTION:  ual_get_status
==============================================================================*/
/**
  Get UAL status: TX(1), RX(2)
*/
uint32_t ual_get_status
(
  void
)
{
  return s_ual_status;
}

/*==============================================================================
  FUNCTION:  ual_get_write_buf
==============================================================================*/
static uint8_t *ual_get_write_buf
(
  void
)
{
  uint8_t *pBuf = NULL;

  if (NULL == s_rx_user_addr)
  {
    LOGE("%s:  Write buf is NULL",
         __FUNCTION__);
    return NULL;
  }

  int next_write_ind = s_rx_write_index + 1;
  if (next_write_ind == s_rx_buf_number)
  {
    next_write_ind = 0;
  }

  // queue (1 gap) isn't full
  if (next_write_ind != s_rx_read_index)
  {
    pBuf = s_rx_user_addr + s_rx_write_index * s_rx_buf_size;
    LOGD("%s:  Returning write_index %d at 0x%p",
         __FUNCTION__,
         s_rx_write_index,
         pBuf);
    s_rx_write_index = next_write_ind;
  }

  return pBuf;
}

/*==============================================================================
  FUNCTION:  ual_open
==============================================================================*/
/**
  See documentation in header file
*/
bool ual_open
(
  // The session configuration
  ual_cfg_type *cfg
)
{
  bool rc = true;

  if (NULL == cfg)
  {
    LOGE("%s:  cfg is NULL",
         __FUNCTION__);
    return false;
  }

  LOGD("%s:  usf_dev_id = %d, ual_mode = %d",
       __FUNCTION__,
       cfg->usf_dev_id,
       cfg->ual_mode);

  (void)pthread_mutex_lock(&s_ual_control_mutex);

  if (-1 == s_usf_fd)
  {
    // Successful Open USF is accomplished only one time
    char device_name[20];
    snprintf(device_name,
             sizeof(device_name),
             "/dev/usf%d",
             cfg->usf_dev_id);

    s_usf_fd = open(device_name,
                    O_RDWR);

    if (0 > s_usf_fd)
    {
      rc = false;
      LOGE("%s:  Could not open usf device %s",
           __FUNCTION__,
           device_name);
    }
    else
    {
      us_version_info_type version_info =
      {
        sizeof(s_usf_version),
        s_usf_version
      };

      int ret = ioctl(s_usf_fd,
                      US_GET_VERSION,
                      &version_info);
      if (ret < 0)
      {
        LOGE("%s:  US_GET_VERSION failed. ret = %d (%s)",
             __FUNCTION__,
             ret,
             strerror(errno));
        close(s_usf_fd);
        s_usf_fd = -1;
        rc = false;
      }
      else
      {
        int len = strlen(SUPPORTED_USF_VERSION);
        LOGD("%s: USF version[%s]; supported one[%s]",
               __FUNCTION__,
               s_usf_version,
               SUPPORTED_USF_VERSION);
        if (strncmp(s_usf_version, SUPPORTED_USF_VERSION, len))
        {
          LOGE("%s: USF version[%s] != supported one[%s]",
                 __FUNCTION__, s_usf_version, SUPPORTED_USF_VERSION);
          close(s_usf_fd);
          s_usf_fd = -1;
          rc = false;
        }
        else
        {
          if (!ual_alsa_open())
          {
            close(s_usf_fd);
            s_usf_fd = -1;
            rc = false;
          }
          else
          {
            s_work_mode = cfg->ual_mode;
          }
        }
      } // the current and supported usf versions compare
    } // usf is opened
  } // usf open block

  (void)pthread_mutex_unlock(&s_ual_control_mutex);

  return rc;
} // ual_open

/*==============================================================================
  FUNCTION:  ual_configure_TX
==============================================================================*/
/**
  See documentation in header file
*/
bool ual_configure_TX
(
  // US Tx device (& stream) configuration
  us_tx_info_type *tx_info
)
{
  return ual_configure_TX_with_mux(tx_info, -1);
}

/*==============================================================================
  FUNCTION:  ual_configure_TX_with_mux
==============================================================================*/
/**
  See documentation in header file
*/
bool ual_configure_TX_with_mux
(
  // US Tx device (& stream) configuration
  us_tx_info_type *tx_info,
  // number of physical TX channels, -1 to use port_count (un-muxed setup)
  int num_actual_channels
)
{
  bool rc = true;
  uint8_t  port_id_backup[USF_MAX_PORT_NUM];

 (void)pthread_mutex_lock(&s_ual_TX_control_mutex);

  if ((NULL == tx_info) ||
      (-1 == s_usf_fd))
  {
    LOGE("%s:  Wrong tx_info or s_usf_fd (%d)",
         __FUNCTION__,
         s_usf_fd);
    (void)pthread_mutex_unlock(&s_ual_TX_control_mutex);
    return false;
  }

  // The required port_ids backup
  (void)memcpy(port_id_backup, tx_info->us_xx_info.port_id, USF_MAX_PORT_NUM);

  // Obtain audio device ID
  s_tx_dev_id = ual_alsa_open_device_with_mux(tx_info->us_xx_info,
                                     UAL_TX_DIRECTION,
                                     num_actual_channels);
  if (0 > s_tx_dev_id)
  {
    LOGE("%s: Could not obtain audio device ID (port=%d)",
         __FUNCTION__,
         tx_info->us_xx_info.port_id[0]);
    (void)pthread_mutex_unlock(&s_ual_TX_control_mutex);
    return false;
  }

  LOGD("%s:  Got audio device ID %d",
       __FUNCTION__,
       s_tx_dev_id);

  // Device switch only
  if (UAL_MODE_IDLE_ALL_DATA_PATH == s_work_mode)
  {
    LOGD("%s:  Only device switch",
         __FUNCTION__);
    // The required port_ids restore
    (void)memcpy(tx_info->us_xx_info.port_id,port_id_backup,USF_MAX_PORT_NUM);
    (void)pthread_mutex_unlock(&s_ual_TX_control_mutex);
    return false;
  }

  tx_info->us_xx_info.dev_id = s_tx_dev_id;

  // In Tx the calculator's name should have specific prefix
  if (strncmp(tx_info->us_xx_info.client_name,
              UAL_CALC_NAME_PREFIX,
              strlen(UAL_CALC_NAME_PREFIX)))
  {
    // The name has no required prefix - add it
    snprintf(s_calc_name,
             sizeof(s_calc_name),
             "%s%s",
             UAL_CALC_NAME_PREFIX,
             tx_info->us_xx_info.client_name);
    tx_info->us_xx_info.client_name = s_calc_name;
  }

  LOGD("TX_INFO rate = %d, bsize = %d, bnum = %d, bps = %d, " \
       "max_param_bsize = %d, name = %s",
       tx_info->us_xx_info.sample_rate,
       tx_info->us_xx_info.buf_size,
       tx_info->us_xx_info.buf_num,
       tx_info->us_xx_info.bits_per_sample,
       tx_info->us_xx_info.max_get_set_param_buf_size,
       tx_info->us_xx_info.client_name);

  int ret = ioctl(s_usf_fd,
                  US_SET_TX_INFO,
                  tx_info);

  // The required port_ids restore
  (void)memcpy(tx_info->us_xx_info.port_id,port_id_backup,USF_MAX_PORT_NUM);

  if (0 > ret)
  {
    LOGE( "ioctl(US_SET_TX_INFO) failed:  ret = %d err = %d (%s)",
          ret,
          errno,
          strerror(errno));
    (void)pthread_mutex_unlock(&s_ual_TX_control_mutex);
    return false;
  }

  s_tx_buf_size = tx_info->us_xx_info.buf_size;
  s_tx_buf_number = tx_info->us_xx_info.buf_num;
  s_tx_alloc_size = s_tx_buf_size * s_tx_buf_number;

  if (0 < s_tx_alloc_size)
  { // Data path is required
    // request memory mapping
    s_tx_user_addr = static_cast<uint8_t*>(mmap(0,
                                                s_tx_alloc_size,
                                                PROT_READ, MAP_SHARED,
                                                s_usf_fd,
                                                0));

    LOGD("%s:  Received 0x%p from mmap; size = %d",
         __FUNCTION__,
         s_tx_user_addr,
         s_tx_alloc_size);

    if (MAP_FAILED == s_tx_user_addr)
    {
      LOGE("%s:  mmap failed():  err = %d (%s)",
           __FUNCTION__,
           errno,
           strerror(errno));

      s_tx_user_addr = NULL;
      s_tx_alloc_size = 0;
      s_tx_buf_size = 0;
      s_tx_buf_number = 0;
      rc = false;
    }
  }

  if (rc)
  {
    s_ual_status |= UAL_TX_STATUS_ON;
  }

 (void)pthread_mutex_unlock(&s_ual_TX_control_mutex);

  return rc;
}

/*==============================================================================
  FUNCTION:  ual_configure_RX
==============================================================================*/
/**
  See documentation in header file
*/
bool ual_configure_RX
(
  // US RX device (& stream) configuration
  us_rx_info_type *rx_info,
  // RX audio device arbitration configuration (NULL to disable)
  ual_rx_dev_arbi_config_type *rx_dev_arbi_config,
  // RX ramp down timeout
  int ramp_down_timeout
)
{
  bool rc = true;

  (void)pthread_mutex_lock(&s_ual_RX_control_mutex);
  if ((NULL == rx_info) ||
      (-1 == s_usf_fd))
  {
    LOGE("%s:  Wrong rx_info or s_usf_fd[%d]",
         __FUNCTION__, s_usf_fd);
    (void)pthread_mutex_unlock(&s_ual_RX_control_mutex);
    return false;
  }

  if (rx_dev_arbi_config != NULL && rx_dev_arbi_config->rx_state_evt_cb != NULL)
  {
    uint32_t enabled_audio_devices =
        rx_dev_arbi_config->rx_dev_arbi_enabled_audio_devices;
    uint32_t default_audio_device =
        rx_dev_arbi_config->rx_dev_arbi_default_audio_device;
    if (enabled_audio_devices == 0)
    {
      // override 0 default value - enable all possible audio devices
      enabled_audio_devices = AUDIO_DEVICE_OUT_EARPIECE |
          AUDIO_DEVICE_OUT_SPEAKER | AUDIO_DEVICE_OUT_WIRED_HEADPHONE;
      LOGD("%s: override default enabled_audio_devices to value %d",
           __FUNCTION__, (int)enabled_audio_devices);
    }
    if (default_audio_device == 0)
    {
      // override 0 default value - earpiece default device
      default_audio_device = AUDIO_DEVICE_OUT_EARPIECE;
      LOGD("%s: override default default_audio_device to value %d",
           __FUNCTION__, (int)default_audio_device);
    }

    // build and validate array of enabled audio devices for RX arbitration
    bool default_audio_device_valid = false;
    s_rx_dev_arbi_num_enabled_devices = 0;

    for (uint32_t i = 0; i < RX_DEV_ARBI_NUM_SUPPORTED_DEVICES; i++)
    {
      if (enabled_audio_devices & s_rx_dev_arbi_supported_devices[i])
      {
        s_rx_dev_arbi_enabled_devices[s_rx_dev_arbi_num_enabled_devices] =
          s_rx_dev_arbi_supported_devices[i];
        s_rx_dev_arbi_num_enabled_devices++;
        if (default_audio_device == s_rx_dev_arbi_supported_devices[i])
        {
          default_audio_device_valid = true;
        }
      }
    }
    if (s_rx_dev_arbi_num_enabled_devices == 0)
    {
      LOGE("%s: no audio devices selected for RX arbitration", __FUNCTION__);
      (void)pthread_mutex_unlock(&s_ual_RX_control_mutex);
      return false;
    }
    if (!default_audio_device_valid)
    {
      LOGE("%s: no valid default audio device for RX arbitration",
           __FUNCTION__);
      (void)pthread_mutex_unlock(&s_ual_RX_control_mutex);
      return false;
    }


    s_rx_dev_arbi_default_topology_device = default_audio_device;
    if (ual_aud_dev_arbi_init(ual_rx_dev_arbi_state_cb_wrapper) < 0)
    {
      LOGE("%s:  Could not initialize audio device arbitration client",
        __FUNCTION__);
      (void)pthread_mutex_unlock(&s_ual_RX_control_mutex);
      return false;
    }

    // Register for all supported audio devices for arbitration
    // build a mask for active devices so we can choose an appropriate
    // topology to go to our RX device (earpiece mainly)
    s_rx_dev_arbi_active_devices = AUDIO_DEVICE_NONE;
    for (uint32_t i = 0; i < s_rx_dev_arbi_num_enabled_devices; i++)
    {
        if (ual_aud_dev_arbi_register(AUD_DEV_ARBI_EVENT_CLIENT_UAL,
                                      s_rx_dev_arbi_enabled_devices[i]) < 0)
        {
          LOGE("%s:  Cannot register for device",
            __FUNCTION__);
          (void)pthread_mutex_unlock(&s_ual_RX_control_mutex);
          return false;
        }
        // Device mask will be updated in the callback
        if (ual_aud_dev_arbi_acquire(s_rx_dev_arbi_enabled_devices[i],
                                  ual_rx_dev_arbi_acquire_cmd_result_cb, 0) < 0)
        {
          LOGD("%s: audio device %d is active",
            __FUNCTION__, s_rx_dev_arbi_enabled_devices[i]);
        }
    }

    s_rx_dev_arbi_state_evt_cb = rx_dev_arbi_config->rx_state_evt_cb;

    if (!ual_rx_dev_arbi_is_ultrasound_topology_possible(
       s_rx_dev_arbi_active_devices))
    {
        LOGE("%s: ultrasound RX device topology not possible",
             __FUNCTION__);
        (void)pthread_mutex_unlock(&s_ual_RX_control_mutex);
        return false;
    }

    // Calculate topology suffix for RX device
    const char *suffix = NULL;
    s_rx_dev_arbi_compat_us_device = s_rx_dev_arbi_default_topology_device;
    for (uint32_t i = 0; i < RX_DEV_ARBI_NUM_SUPPORTED_DEVICES; i++)
    {
        if (s_rx_dev_arbi_active_devices & s_rx_dev_arbi_supported_devices[i])
        {
            suffix = s_rx_dev_arbi_topology_suffixes[i];
            s_rx_dev_arbi_compat_us_device =
                s_rx_dev_arbi_supported_devices[i];
            break;
        }
    }
    ual_alsa_set_rx_topology_suffix(suffix);

  }

  // obtain audio device ID
  s_rx_dev_id = ual_alsa_open_device(rx_info->us_xx_info,
                                         UAL_RX_DIRECTION);
  if (0 > s_rx_dev_id)
  {
    LOGE("%s:  Could not obtain audio device ID. Stop",
         __FUNCTION__);
    (void)pthread_mutex_unlock(&s_ual_RX_control_mutex);
    return false;
  }
  LOGI("%s:  Got audio device ID %d",
       __FUNCTION__, s_rx_dev_id);

   // Only device switch
  if (UAL_MODE_IDLE_ALL_DATA_PATH == s_work_mode)
  {
    LOGD("%s:  Only device switch",
         __FUNCTION__);
    (void)pthread_mutex_unlock(&s_ual_RX_control_mutex);
    return false;
  }

  rx_info->us_xx_info.dev_id = s_rx_dev_id;

  LOGD("%s:  RX_INFO s_rate = %d, buf_size = %d buf_num = %d " \
       "bits_per_sample = %d, max_param_bsize = %d",
       __FUNCTION__,
       rx_info->us_xx_info.sample_rate,
       rx_info->us_xx_info.buf_size,
       rx_info->us_xx_info.buf_num,
       rx_info->us_xx_info.bits_per_sample,
       rx_info->us_xx_info.max_get_set_param_buf_size);

  int ret = ioctl(s_usf_fd,
                  US_SET_RX_INFO,
                  rx_info);
  if (0 > ret)
  {
    LOGE("%s:  ioctl(US_SET_RX_INFO) failed. ret = %d err = %d (%s)",
         __FUNCTION__,
          ret,
         errno,
         strerror(errno));
    (void)pthread_mutex_unlock(&s_ual_RX_control_mutex);
    return false;
  }

  s_rx_buf_size = rx_info->us_xx_info.buf_size;
  s_rx_buf_number = rx_info->us_xx_info.buf_num;
  s_rx_alloc_size = s_rx_buf_size * s_rx_buf_number;
  s_rx_ramp_down_timeout = ramp_down_timeout;

  // Data path is required --> Request memory mapping
  if (s_rx_alloc_size > 0)
  {
    s_rx_user_addr = static_cast<uint8_t*>(mmap(0,
                                                s_rx_alloc_size,
                                                PROT_WRITE,
                                                MAP_SHARED,
                                                s_usf_fd,
                                                0));

    LOGD("%s:  Received 0x%p from mmap; size = %d",
         __FUNCTION__,
         s_rx_user_addr,
         s_rx_alloc_size);

    if(MAP_FAILED == s_rx_user_addr)
    {
      LOGE("%s:  mmap failed(); err = %d (%s)",
           __FUNCTION__,
           errno,
           strerror(errno));

      s_rx_user_addr = NULL;
      s_rx_alloc_size = 0;
      s_rx_buf_size = 0;
      s_rx_buf_number = 0;
      rc = false;
    }
  }

  (void)pthread_mutex_unlock(&s_ual_RX_control_mutex);

  return rc;
}

/*==============================================================================
  FUNCTION:  ual_start_TX
==============================================================================*/
/**
  See documentation in header file
*/
bool ual_start_TX(void)
{
  bool  rc = true;

 (void)pthread_mutex_lock(&s_ual_TX_control_mutex);

  if (-1 == s_usf_fd)
  {
    LOGE("%s:  UAL is not opened",
         __FUNCTION__);

    rc = false;
  }
  else
  {
    int ret = ioctl(s_usf_fd,
                    US_START_TX,
                    NULL);
    rc = (ret >= 0);
  }

  LOGD("%s:  rc = %d",
       __FUNCTION__,
       rc);

  (void)pthread_mutex_unlock(&s_ual_TX_control_mutex);

  return rc;
}

/*==============================================================================
  FUNCTION:  ual_start_RX
==============================================================================*/
/**
  See documentation in header file
*/
bool ual_start_RX(void)
{
  bool  rc = true;

  (void)pthread_mutex_lock(&s_ual_RX_control_mutex);
  if (- 1== s_usf_fd)
  {
    LOGE("%s:  UAL is not opened",
         __FUNCTION__);

    rc = false;
  }
  else
  {
    int ret = ioctl(s_usf_fd,
                    US_START_RX,
                    NULL);

    rc = (ret >= 0);
  }

  LOGD("%s:  rc = %d",
       __FUNCTION__,
       rc);

  (void)pthread_mutex_unlock(&s_ual_RX_control_mutex);
  return rc;
}

/*==============================================================================
  FUNCTION:  ual_stop_TX
==============================================================================*/
/**
  See documentation in header file
*/
bool ual_stop_TX(void)
{
  bool  rc = true;

  (void)pthread_mutex_lock(&s_ual_TX_control_mutex);

  if (-1 == s_usf_fd)
  {
    LOGE("%s:  UAL is not opened",
         __FUNCTION__);

    rc = false;
  }
  else
  {
    // Release Tx sound device
    if (s_tx_dev_id >= 0)
    {
      int ret = ioctl(s_usf_fd,
                      US_STOP_TX,
                      NULL);

      LOGD("%s:  Done US_STOP_TX ret = %d; errno = %d",
           __FUNCTION__,
           ret,
           errno);

      rc = (ret >= 0);

      // Release Tx shared US data memory
      if (NULL != s_tx_user_addr)
      {
        ret = munmap(s_tx_user_addr,
                     s_tx_alloc_size);

        rc = rc && (ret == 0);

        LOGD("%s:  Done munmap (%p, %d) ret = %d, errno = %d",
             __FUNCTION__,
             s_tx_user_addr,
             s_tx_alloc_size,
             ret,
             errno);
      }

      bool rc1 = ual_alsa_close_device(UAL_TX_DIRECTION, 0);
      if (!rc1)
      {
        LOGD("%s:  ual_alsa_close_device(TX) failed, but continue",
             __FUNCTION__);
      }

      s_ual_status &= ~UAL_TX_STATUS_ON;
    }

    s_tx_user_addr = NULL;
    s_tx_alloc_size = 0;
    s_tx_buf_number = 0;
    s_tx_buf_size = 0;
    s_tx_dev_id = -1;
    s_tx_read_index = 0;
    s_tx_write_index = 0;
  }

  (void)pthread_mutex_unlock(&s_ual_TX_control_mutex);

  return rc;
}

/*==============================================================================
  FUNCTION:  ual_stop_RX
==============================================================================*/
/**
  See documentation in header file
*/
bool ual_stop_RX(
  // Whether to notify media server that Rx was released
  bool notify_device_release)
{
  bool  rc = true;

  (void)pthread_mutex_lock(&s_ual_RX_control_mutex);

  if (-1 == s_usf_fd)
  {
    LOGE("%s:  UAL is not opened",
         __FUNCTION__);

    rc = false;
  }
  else
  {
    // Release Rx sound device
    if (s_rx_dev_id >= 0)
    {
      bool rc1 = ual_alsa_close_device(UAL_RX_DIRECTION, s_rx_ramp_down_timeout);
      if (!rc1)
      {
        LOGD("%s:  ual_alsa_close_device(RX) failed, but continue",
             __FUNCTION__);
      }

      int ret = ioctl(s_usf_fd,
                      US_STOP_RX,
                      NULL);

      LOGD("%s:  Done US_STOP_RX ret = %d, errno = %d",
           __FUNCTION__,
           ret,
           errno);

      rc = (ret >= 0);

      // Release Rx shared US data memory
      if (NULL != s_rx_user_addr)
      {
        ret = munmap(s_rx_user_addr,
                     s_rx_alloc_size);

        rc = rc && (ret == 0);
        LOGD("%s:  Done munmap (%p, %d) ret = %d, errno = %d",
             __FUNCTION__,
             s_rx_user_addr,
             s_rx_alloc_size,
             ret,
             errno);
      }

      s_ual_status &= ~UAL_RX_STATUS_ON;
    }

    s_rx_user_addr = NULL;
    s_rx_alloc_size = 0;
    s_rx_buf_number = 0;
    s_rx_buf_size = 0;
    s_rx_dev_id = -1;
    s_rx_read_index = 0;
    s_rx_write_index = 0;
  }

  if (s_rx_dev_arbi_state_evt_cb != NULL)
  {
    s_rx_dev_arbi_state_evt_cb = NULL;
    s_rx_dev_arbi_compat_us_device = AUDIO_DEVICE_NONE;
    if (notify_device_release)
    {
      for (uint32_t i = 0; i < s_rx_dev_arbi_num_enabled_devices; i++)
      {
        if (ual_aud_dev_arbi_release(
           s_rx_dev_arbi_enabled_devices[i], NULL, 0) < 0)
        {
          LOGE("%s:  Error notifying media server that device %d is released",
               __FUNCTION__, s_rx_dev_arbi_enabled_devices[i]);
        }
      }
      s_rx_dev_arbi_num_enabled_devices = 0;
    }
  }

  (void)pthread_mutex_unlock(&s_ual_RX_control_mutex);

  return rc;
}


bool wait_for_sound_card_state(
    const char *req_card_state,
    int req_times_detected,
    int num_retries,
    int sleep_interval)
{
    const char *path = "/proc/asound/card0/state";
    bool state_found = false;
    int times_detected = 0;

    do
    {
        int fd;

        fd = open(path, O_RDONLY);
        if (fd == -1)
        {
            LOGE("Open %s failed : %s", path, strerror(errno));
        }
        else
        {
            const int MAX_CARD_STATE_STR_LEN = 9;

            char rd_buf[MAX_CARD_STATE_STR_LEN];
            read(fd, (void *)rd_buf, sizeof(rd_buf));
            close(fd);

            rd_buf[sizeof(rd_buf)-1]='\0';
            LOGD("%s:  Read from %s : %s",
                 __FUNCTION__,
                 path,
                 rd_buf);

            if(strstr(rd_buf, req_card_state))
            {
                times_detected++;

                LOGD("%s: %s detected %d times in a row, required %d",
                 __FUNCTION__,
                 req_card_state,
                 times_detected,
                 req_times_detected);

                if(times_detected == req_times_detected)
                {
                    state_found = true;
                    break;
                }
            }
            else
            {
                times_detected = 0;
            }
        }

        num_retries--;
        usleep(sleep_interval*1000);
    } while(num_retries);

    return state_found;
}

/*==============================================================================
  FUNCTION:  ual_close
==============================================================================*/
/**
  See documentation in header file
*/
bool ual_close(
    // If true, close will be done under the assumption that an error occured (this
    // affects sequence with lower layers during close)
    bool error_state)
{
  LOGD("%s:  Enter",
       __FUNCTION__);

  if(error_state)
  {
    const char* req_card_state = "ONLINE";

    LOGD("%s:  Waiting for sound card state %s before starting close sequence",
       __FUNCTION__,
         req_card_state);

    if(!wait_for_sound_card_state(req_card_state, 4, 30, 100))
    {
      LOGE("%s:  Sound card state %s not detected, stream might not close correctly",
       __FUNCTION__,
       req_card_state);
    }

    LOGD("%s: Continuing with close sequence",
         __FUNCTION__);
  }

  bool  rc = true;

  if (-1 == s_usf_fd)
  {
    LOGE("%s:  UAL is not opened",
         __FUNCTION__);

    rc = false;
  }
  else
  {
    rc = ual_stop_RX(true);
    bool rc1 = ual_stop_TX();
    bool rc2 = (ual_aud_dev_arbi_deinit() >= 0);

    rc = rc && rc1 && rc2;
    (void)pthread_mutex_lock(&s_ual_control_mutex);
    close(s_usf_fd);
    s_usf_fd = -1;
    ual_alsa_close();
    (void)pthread_mutex_unlock(&s_ual_control_mutex);
  }

  return rc;
}

/*==============================================================================
  FUNCTION:  ual_write
==============================================================================*/
/**
  See documentation in header file
*/
bool ual_write
(
  // US RX data to send
  uint8_t *data,
  // Size of US RX data to send
  uint32_t data_size
)
{
  (void)pthread_mutex_lock(&s_ual_RX_control_mutex);

  if (UAL_MODE_IDLE_ALL_DATA_PATH == s_work_mode)
  {
    LOGD("%s:  Only device switch",
         __FUNCTION__);
    (void)pthread_mutex_unlock(&s_ual_RX_control_mutex);
    return true;
  }

  if ((s_usf_fd == -1 ) ||
      (data == NULL) ||
      (data_size > s_rx_buf_size))
  {
    LOGE("%s:  Wrong parameters: data = %p, data = %d or UAL is not opened (%d)",
         __FUNCTION__,
         data,
         data_size,
         s_usf_fd);

    (void)pthread_mutex_unlock(&s_ual_RX_control_mutex);
    return false;
  }

  uint8_t* pBuf = ual_get_write_buf();
  if (NULL == pBuf)
  {
    LOGE("%s:  Rx path full",
         __FUNCTION__);

    (void)pthread_mutex_unlock(&s_ual_RX_control_mutex);
    return false;
  }

  us_rx_update_info_type update_info;

  memcpy(pBuf,
         data,
         data_size);
  update_info.ready_region = s_rx_write_index;

  update_info.params_data = NULL; // not supported by USF
  update_info.params_data_size = 0; // not supported by USF;

  bool rc = true;
  int ret = ioctl(s_usf_fd,
                  US_SET_RX_UPDATE,
                  &update_info);

  if (ret < 0)
  {
    LOGE("%s:  (US_SET_RX_UPDATE) failed. ret = %d (%s)",
         __FUNCTION__,
         ret,
         strerror(errno));

    rc = false;
  }
  else
  {
    s_rx_read_index = update_info.free_region;
    LOGD("%s:  (US_SET_RX_UPDATE): read(freereg) = %d write(readyreg) = %d",
         __FUNCTION__,
         update_info.free_region,
         update_info.ready_region);
  }

  (void)pthread_mutex_unlock(&s_ual_RX_control_mutex);

  return rc;
}

/*==============================================================================
  FUNCTION:  ual_read
==============================================================================*/
/**
  See documentation in header file
*/
bool ual_read
(
  // Place for received Tx US data
  ual_data_type *data,
  // The calculated events
  usf_event_type *events,
  // The number of calculated events
  uint16_t event_counter,
  // Time (sec) to wait for data
  uint32_t timeout
)
{
  (void)pthread_mutex_lock(&s_ual_TX_control_mutex);

  if (-1 == s_usf_fd)
  {
    (void)pthread_mutex_unlock(&s_ual_TX_control_mutex);
    LOGE("%s:UAL is not opened",
         __FUNCTION__);
    return false;
  }

  us_tx_update_info_type update_info;
  bool rc = true;

  update_info.timeout = timeout;
  if ((events != NULL) &&
      (event_counter >  0) &&
      (s_work_mode != UAL_MODE_NO_INJECT_IN_EVENTS))
  {

    update_info.event = events;
    update_info.event_counter = event_counter;
  }
  else
  {
    update_info.event = NULL;
    update_info.event_counter = 0;
  }

  // ual_read() call means, the region from the previous call
  // may be released (that is, the queue is empty)
  s_tx_read_index = s_tx_write_index;

  if ((s_work_mode == UAL_MODE_IDLE_ALL_DATA_PATH) ||
      (s_work_mode == UAL_MODE_IDLE_USF_DATA_PATH))
  {  // (s_tx_write_index == s_tx_read_index) is invariant
    sleep(UAL_MODE_IDLE_DATA_SLEEP_TIME);
  }
  else
  {
    update_info.params_data = NULL;
    update_info.params_data_size = 0;
    update_info.free_region = s_tx_read_index;
    update_info.event_filters = s_event_filters;

    int ret = ioctl(s_usf_fd,
                    US_GET_TX_UPDATE,
                    &update_info);

    if (ret < 0)
    {
      if (ETIME == errno)
      {
        LOGD("%s:  (US_GET_TX_UPDATE) failed. errno = ETIME; continue..",
             __FUNCTION__);
      }
      else if (EINTR == errno)
      {
        LOGD("%s:  (US_GET_TX_UPDATE) failed. errno = EINTR; likely SUSPEND...",
             __FUNCTION__);
      }
      else
      {
        LOGE("%s:  (US_GET_TX_UPDATE) failed. ret = %d (errno %d: %s)",
             __FUNCTION__,
             ret,
             errno,
             strerror(errno));
        (void)pthread_mutex_unlock(&s_ual_TX_control_mutex);
        return false;
      }
    }
    else
    {
      s_tx_write_index =  update_info.ready_region;
    }
  }

  if (NULL != data)
  {
    /* Data isn't required or the queue is empty or
       UAL_MODE_NO_CALC_IN_EVENTS work mode */
    if ((NULL == s_tx_user_addr) ||
        (s_tx_write_index == s_tx_read_index) ||
        (s_work_mode == UAL_MODE_NO_CALC_IN_EVENTS))
    {
      data->region[0].data_buf = NULL;
      data->region[0].data_buf_size = 0;
      data->region[1].data_buf = NULL;
      data->region[1].data_buf_size = 0;
    }
    else
    {
      if (s_tx_write_index > s_tx_read_index)
      { // 1 region
        data->region[0].data_buf = s_tx_user_addr +
                                   s_tx_buf_size*s_tx_read_index;
        data->region[0].data_buf_size = s_tx_buf_size *
                                        (s_tx_write_index - s_tx_read_index);
        data->region[1].data_buf = NULL;
        data->region[1].data_buf_size = 0;
      }
      else
      { // 2 regions
        data->region[0].data_buf = s_tx_user_addr +
                                   s_tx_buf_size*s_tx_read_index;
        data->region[0].data_buf_size = s_tx_buf_size *
                                        (s_tx_buf_number - s_tx_read_index);
        data->region[1].data_buf = s_tx_user_addr;
        data->region[1].data_buf_size = s_tx_buf_size *
                                        s_tx_write_index;
      }
    }
  }

  (void)pthread_mutex_unlock(&s_ual_TX_control_mutex);

  return rc;
} // ual_read

/*==============================================================================
  FUNCTION:  ual_start_us_detection
==============================================================================*/
/**
  See documentation in header file
*/
bool ual_start_us_detection
(
  // US detection info
  us_detect_info_type& detect_info
)
{
  (void)pthread_mutex_lock(&s_ual_TX_control_mutex);
  if ((-1 == s_usf_fd) ||
      (US_DETECT_DISABLED_MODE == detect_info.us_detect_mode))
  {
    LOGE("%s: UAL is not opened or wrong detect mode (%d)",
         __FUNCTION__,
         detect_info.us_detect_mode);
    (void)pthread_mutex_unlock(&s_ual_TX_control_mutex);
    return false;
  }

  bool rc = true;
  int ret = ioctl(s_usf_fd,
                  US_SET_DETECTION,
                  &detect_info);
  if (ret < 0)
  {
    LOGE("%s:  US_SET_DETECTION failed. ret = %d (%s)",
         __FUNCTION__,
         ret,
         strerror(errno));
    rc = false;
  }

  (void)pthread_mutex_unlock(&s_ual_TX_control_mutex);

  return rc;
} // ual_start_us_detection

/*==============================================================================
  FUNCTION:  ual_stop_us_detection
==============================================================================*/
/**
  See documentation in header file
*/
bool ual_stop_us_detection
(
  // US detection info
  us_detect_info_type detect_info
)
{
  (void)pthread_mutex_lock(&s_ual_TX_control_mutex);
  if ((-1 == s_usf_fd) ||
      (US_DETECT_DISABLED_MODE != detect_info.us_detect_mode))
  {
    LOGE("%s: UAL is not opened or wrong detect mode (%d)",
         __FUNCTION__,
         detect_info.us_detect_mode);
    (void)pthread_mutex_unlock(&s_ual_TX_control_mutex);
    return false;
  }

  bool rc = true;
  int ret = ioctl(s_usf_fd,
                  US_SET_DETECTION,
                  &detect_info);
  if (ret < 0)
  {
    LOGE("%s:  US_SET_DETECTION failed. ret = %d (%s)",
         __FUNCTION__,
         ret,
         strerror(errno));
    rc = false;
  }

  (void)pthread_mutex_unlock(&s_ual_TX_control_mutex);

  return rc;
} // ual_stop_us_detection

/*==============================================================================
  FUNCTION:  ual_get_version
==============================================================================*/
/**
  See documentation in header file
*/
void ual_get_version
(
  us_versions_type& us_versions
)
{
  us_versions.p_usf_version = s_usf_version;
  us_versions.p_ual_version = UAL_VERSION;
}

/*==============================================================================
  FUNCTION:  ual_set_event_filters
==============================================================================*/
/**
  See documentation in header file
*/
void ual_set_event_filters
(
  uint16_t event_filters
)
{
  s_event_filters = event_filters;
}

/*==============================================================================
  FUNCTION:  ual_set_TX_param
==============================================================================*/
/**
  See documentation in header file
*/
bool ual_set_TX_param
(
  // US TX stream configuration
  us_stream_param_type& stream_param
)
{
  (void)pthread_mutex_lock(&s_ual_TX_control_mutex);
  if (-1 == s_usf_fd)
  {
    LOGE("%s: UAL is not opened",
         __FUNCTION__);
    (void)pthread_mutex_unlock(&s_ual_TX_control_mutex);
    return false;
  }

  bool rc = true;
  int ret = ioctl(s_usf_fd,
                  US_SET_TX_STREAM_PARAM,
                  &stream_param);
  if (ret < 0)
  {
    LOGE("%s:  US_SET_TX_STREAM_PARAM failed. ret = %d (%s)",
         __FUNCTION__,
         ret,
         strerror(errno));
    rc = false;
  }

  (void)pthread_mutex_unlock(&s_ual_TX_control_mutex);

  return rc;
} // ual_set_TX_param

/*==============================================================================
  FUNCTION:  ual_get_TX_param
==============================================================================*/
/**
  See documentation in header file
*/
bool ual_get_TX_param
(
  // US TX stream configuration
  us_stream_param_type& stream_param
)
{
  (void)pthread_mutex_lock(&s_ual_TX_control_mutex);
  if (-1 == s_usf_fd)
  {
    LOGE("%s: UAL is not opened",
         __FUNCTION__);
    (void)pthread_mutex_unlock(&s_ual_TX_control_mutex);
    return false;
  }

  bool rc = true;
  int ret = ioctl(s_usf_fd,
                  US_GET_TX_STREAM_PARAM,
                  &stream_param);
  if (ret < 0)
  {
    LOGE("%s:  US_GET_TX_STREAM_PARAM failed. ret = %d (%s)",
         __FUNCTION__,
         ret,
         strerror(errno));
    rc = false;
  }

  (void)pthread_mutex_unlock(&s_ual_TX_control_mutex);

  return rc;
} // ual_get_TX_param

/*==============================================================================
  FUNCTION:  ual_set_RX_param
==============================================================================*/
/**
  See documentation in header file
*/
bool ual_set_RX_param
(
  // US TX stream configuration
  us_stream_param_type& stream_param
)
{
  (void)pthread_mutex_lock(&s_ual_RX_control_mutex);
  if (-1 == s_usf_fd)
  {
    LOGE("%s: UAL is not opened",
         __FUNCTION__);
    (void)pthread_mutex_unlock(&s_ual_RX_control_mutex);
    return false;
  }

  bool rc = true;
  int ret = ioctl(s_usf_fd,
                  US_SET_RX_STREAM_PARAM,
                  &stream_param);
  if (ret < 0)
  {
    LOGE("%s:  US_SET_RX_STREAM_PARAM failed. ret = %d (%s)",
         __FUNCTION__,
         ret,
         strerror(errno));
    rc = false;
  }

  (void)pthread_mutex_unlock(&s_ual_RX_control_mutex);

  return rc;
} // ual_set_RX_param

/*==============================================================================
  FUNCTION:  ual_get_RX_param
==============================================================================*/
/**
  See documentation in header file
*/
bool ual_get_RX_param
(
  // US TX stream configuration
  us_stream_param_type& stream_param
)
{
  (void)pthread_mutex_lock(&s_ual_RX_control_mutex);
  if (-1 == s_usf_fd)
  {
    LOGE("%s: UAL is not opened",
         __FUNCTION__);
    (void)pthread_mutex_unlock(&s_ual_RX_control_mutex);
    return false;
  }

  bool rc = true;
  int ret = ioctl(s_usf_fd,
                  US_GET_RX_STREAM_PARAM,
                  &stream_param);
  if (ret < 0)
  {
    LOGE("%s:  US_GET_RX_STREAM_PARAM failed. ret = %d (%s)",
         __FUNCTION__,
         ret,
         strerror(errno));
    rc = false;
  }

  (void)pthread_mutex_unlock(&s_ual_RX_control_mutex);

  return rc;
} // ual_get_RX_param

/*==============================================================================
  FUNCTION:  ual_rx_dev_arbi_update_active_devices
==============================================================================*/
/**
  See documentation in header file
*/
void ual_rx_dev_arbi_update_active_devices(audio_devices_t device,
                                           aud_dev_arbi_event_t evt)
{
  if (evt == AUD_DEV_ARBI_EVENT_DEVICE_REQUESTED)
  {
    LOGD("%s: device %d requested", __FUNCTION__, device);
    s_rx_dev_arbi_active_devices |= device;
  }
  else if (evt == AUD_DEV_ARBI_EVENT_DEVICE_RELEASED)
  {
    LOGD("%s: device %d is released", __FUNCTION__, device);
    s_rx_dev_arbi_active_devices &= ~device;
  }
}

/*==============================================================================
  FUNCTION:  ual_rx_dev_arbi_is_ultrasound_topology_possible
==============================================================================*/
static bool ual_rx_dev_arbi_is_ultrasound_topology_possible
(
  audio_devices_t active_devices
)
{
  if (active_devices == AUDIO_DEVICE_NONE)
  {
    return true;
  }

  for (uint32_t i = 0; i < s_rx_dev_arbi_num_enabled_devices; i++)
  {
    if (active_devices == s_rx_dev_arbi_enabled_devices[i])
    {
      return true;
    }
  }

  // More than one audio device is active, we don't support this yet
  return false;
}

/*==============================================================================
  FUNCTION:  ual_rx_dev_arbi_is_topology_switch_needed
==============================================================================*/
/**
  See documentation in header file
*/
bool ual_rx_dev_arbi_is_topology_switch_needed(void)
{
  if (s_rx_dev_arbi_compat_us_device == AUDIO_DEVICE_NONE)
  {
    return true;
  }
  if (s_rx_dev_arbi_compat_us_device != s_rx_dev_arbi_active_devices)
  {
    LOGD("%s: topology switch needed. compat %d active %d",
         __FUNCTION__,
         s_rx_dev_arbi_compat_us_device,
         s_rx_dev_arbi_active_devices);
    return true;
  }
  return false;
}

/*==============================================================================
  FUNCTION:  ual_rx_dev_arbi_state_evt_cb
==============================================================================*/
/**
  Wrapper for RX arbitration state event callback. Passed to the dev arbitration
  library instead of the daemon-provided callback to allow internal UAL
  book-keeping.the daemon-provided callback will be called from this callback
  if needed
*/
static void ual_rx_dev_arbi_state_cb_wrapper
(
  audio_devices_t device,
  aud_dev_arbi_event_t evt,
  aud_dev_arbi_event_cb_resp_flags_t *respFlags
)
{
  // Update active audio devices mask
  ual_rx_dev_arbi_update_active_devices(device, evt);

  // Call the daemon-provided callback if any
  if (s_rx_dev_arbi_state_evt_cb)
  {
    s_rx_dev_arbi_state_evt_cb(device, evt, respFlags);
  }

  // When server releases device, we automatically re-acquire it
  if (respFlags)
  {
    if (evt == AUD_DEV_ARBI_EVENT_DEVICE_RELEASED)
    {
      *respFlags = AUD_DEV_ARBI_EVENT_RESP_REACQUIRE;
    }
  }
  else
  {
    LOGE("%s: respFlags is NULL", __FUNCTION__);
  }
}

/*==============================================================================
  FUNCTION:  ual_rx_dev_arbi_acquire_cmd_result_cb
==============================================================================*/
/**
  Result callback from acquire command. We use this callback to update
  the device mask in the same thread as other server notifications
*/
static void ual_rx_dev_arbi_acquire_cmd_result_cb
(
  audio_devices_t device,
  bool success,
  uint32_t userData
)
{
  (void) userData;
  if (!success)
  {
    ual_rx_dev_arbi_update_active_devices(device,
                                       AUD_DEV_ARBI_EVENT_DEVICE_REQUESTED);
  }
}

/*==============================================================================
  FUNCTION:  ual_rx_dev_arbi_get_active_audio_device_mask
==============================================================================*/
/**
  See documentation in header file
*/
audio_devices_t ual_rx_dev_arbi_get_active_audio_device_mask
(
  void
)
{
    return s_rx_dev_arbi_active_devices;
}
