/* Copyright (c) 2017 Qualcomm Technologies, Inc.
   All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <time.h>
#include <pthread.h>
#include "limits.h"
#include "csd_hal.h"
#include "csd_server.h"

pthread_mutex_t hal_cond_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  hal_condition = PTHREAD_COND_INITIALIZER;


struct csd_hal_common_data csd_hal_common_data = {0};

#define DEV_ROUTE_LENGTH 15
#define KV_PAIR_LENGTH 40
#define NUM_OF_ROUTINGS 4
#define NUM_OF_SESSIONS 4
#define NAMES_PER_SESSION 2
#define NUM_OF_MUTE_TYPES 4

typedef struct {
  uint32_t tx_id;
  uint32_t rx_id;
  char routing[DEV_ROUTE_LENGTH];
} routing_info_t;

typedef struct {
  char sess_id[NAMES_PER_SESSION][SESSION_NAME_MAX_LEN_V01 + 1];
  char sess_id_call_state[KV_PAIR_LENGTH];
}session_info_t;

typedef struct {
  uint8_t enable;
  csd_hal_mute_direction direction;
  char mute_state[KV_PAIR_LENGTH];
}mute_info_t;

static routing_info_t routing_info[NUM_OF_ROUTINGS] = {
  {8, 10, "routing=4" },
  {4, 7, "routing=1" },
  {16, 17, "routing=4" },
  {11, 14, "routing=2" },
  {11, 135, "routing=2" }
};

static session_info_t session_info[NUM_OF_SESSIONS] = {
  {{"default mmodevoice1", "11C05000"}, "vsid=297816064;call_state=2"},
  {{"default mmodevoice2", "11DC5000"}, "vsid=299651072;call_state=2"},
  {{"default modem voice", "10C01000"}, "vsid=281022464;call_state=2"},
  {{"default volte voice", "10C02000"}, "vsid=281026560;call_state=2"}
};

static mute_info_t mute_info[NUM_OF_MUTE_TYPES] = {
  {1, HAL_VOICE_MUTE_DIR_TX, "device_mute=true;direction=tx" },
  {0, HAL_VOICE_MUTE_DIR_TX, "device_mute=false;direction=tx" },
  {1, HAL_VOICE_MUTE_DIR_RX, "device_mute=true;direction=rx" },
  {0, HAL_VOICE_MUTE_DIR_RX, "device_mute=false;direction=rx" },
};

char * csd_hal_get_hal_device(uint32_t tx_dev, uint32_t rx_dev )
{
  int i = 0;
  char* ret = "routing=4";

  for (; i < NUM_OF_ROUTINGS; i++)
  {
    if ((tx_dev == routing_info[i].tx_id) && (rx_dev == routing_info[i].rx_id))
    {
      ret = routing_info[i].routing;
      LOGD(" tx_id = %d, rx_id = %d \n", tx_dev, rx_dev);
      break;
    }
  }
    LOGD(" device route %s \n", ret);
    return ret;
}

char * csd_hal_get_sess_id_call_state(const char * session_id)
{
  int i = 0;
  int j = 0;
  char* ret = "vsid=281022464;call_state=2";

  for(i = 0; i < NUM_OF_SESSIONS; i++)
  {
    for(j = 0; j < NAMES_PER_SESSION; j++)
    {
      if(!strcmp(session_id,session_info[i].sess_id[j]))
      {
        ret = session_info[i].sess_id_call_state;
        LOGD("session_id %s\n", session_id);
        break;
      }
    }
  }
  LOGD("sess_id_call_state %s\n", ret);
  return ret;
}

char * csd_hal_get_device_mute_info(uint8_t enable, csd_hal_mute_direction direction)
{
  int i = 0;
  char* ret = NULL;

for (; i < NUM_OF_MUTE_TYPES; i++)
{
  if ((enable == mute_info[i].enable) && (direction == mute_info[i].direction))
  {
    ret = mute_info[i].mute_state;
    LOGD(" enable = %d, direction = %d \n", enable, direction);
    break;
  }
}
  LOGD(" mute_state %s \n", ret == NULL ? "null" : ret);

  return ret;
}

static int csd_hal_start_voice(void)
{
  int rc = 0;
  char * device_route;
  char * session_id;
  qmi_csi_error resp_err;
  uint32_t resp_len;
  qmi_csd_voice_cmd_resp_msg resp_msg = {0};

  session_id = csd_hal_get_sess_id_call_state(csd_hal_common_data.session_id);

  // set vsid
  rc = csd_hal_common_data.device->set_parameters(csd_hal_common_data.device, session_id);
  if (rc) {
    LOGE("setting vsid failed %d \n",rc);
    goto EXIT;
  }

  //set in call mode
  rc = csd_hal_common_data.device->set_mode(csd_hal_common_data.device, AUDIO_MODE_IN_CALL );
   if (rc) {
    LOGE("set call mode failed %d \n",rc);
    goto EXIT;
  }

  //set device and start call
  device_route = csd_hal_get_hal_device(csd_hal_common_data.device_data.devices[0].dev_id,
                                        csd_hal_common_data.device_data.devices[1].dev_id);

  rc = csd_hal_common_data.out_stream->common.set_parameters(&csd_hal_common_data.out_stream->common, device_route);
  if (rc) {
    LOGE("set params failed %d \n",rc);
  }

EXIT:
    LOGD("%s csd_hal_start_voice() rc=%d\n",csd_log_time(),rc);
    if (rc != 0) {
        resp_msg.u.qmi_csd_voice_resp.resp.result = QMI_RESULT_FAILURE_V01;
        resp_msg.u.qmi_csd_voice_resp.resp.error = QMI_ERR_GENERAL_V01;
        resp_msg.u.qmi_csd_voice_resp.qmi_csd_status_code_valid = true;
        resp_msg.u.qmi_csd_voice_resp.qmi_csd_status_code =
                                                            QMI_CSD_EFAILED_V01;
        resp_msg.u.qmi_csd_voice_resp.cmd_token_valid = false;
    } else {
        resp_msg.u.qmi_csd_voice_resp.qmi_csd_status_code_valid = false;
        resp_msg.u.qmi_csd_voice_resp.cmd_token_valid = true;
        resp_msg.u.qmi_csd_voice_resp.cmd_token =
                                                csd_hal_common_data.command.token;
    }

    resp_len = sizeof (resp_msg.u.qmi_csd_voice_resp);
    resp_err = qmi_csi_send_resp(csd_hal_common_data.command.req_handle,
                                  csd_hal_common_data.command.msg_id,
                                  &resp_msg, resp_len);
    if (resp_err != QMI_CSI_NO_ERR)
        rc = QMI_CSI_CB_INTERNAL_ERR;
    LOGD("%s csd_hal_start_voice() OUT\n",csd_log_time());

    return rc;
}

static int csd_hal_end_voice(void)
{
  int rc = 0;
  qmi_csi_error resp_err;
  uint32_t resp_len;
  qmi_csd_voice_cmd_resp_msg resp_msg = {0};

  // set vsid
  rc = csd_hal_common_data.device->set_parameters(csd_hal_common_data.device, "call_state=1");
  if (rc) {
    LOGE("setting call state failed %d \n",rc);
    goto EXIT;
  }

  //set in call mode
  rc = csd_hal_common_data.device->set_mode(csd_hal_common_data.device, AUDIO_MODE_NORMAL );
   if (rc) {
    LOGE("set call mode failed %d \n",rc);
    goto EXIT;
  }

  EXIT:
    LOGD("%s csd_hal_end_voice() rc=%d\n",csd_log_time(),rc);
    if (rc != 0) {
        resp_msg.u.qmi_csd_voice_resp.resp.result = QMI_RESULT_FAILURE_V01;
        resp_msg.u.qmi_csd_voice_resp.resp.error = QMI_ERR_GENERAL_V01;
        resp_msg.u.qmi_csd_voice_resp.qmi_csd_status_code_valid = true;
        resp_msg.u.qmi_csd_voice_resp.qmi_csd_status_code =
                                                            QMI_CSD_EFAILED_V01;
        resp_msg.u.qmi_csd_voice_resp.cmd_token_valid = false;
    } else {
        resp_msg.u.qmi_csd_voice_resp.qmi_csd_status_code_valid = false;
        resp_msg.u.qmi_csd_voice_resp.cmd_token_valid = true;
        resp_msg.u.qmi_csd_voice_resp.cmd_token =
                                                csd_hal_common_data.command.token;
    }

    resp_len = sizeof (resp_msg.u.qmi_csd_voice_resp);
    resp_err = qmi_csi_send_resp(csd_hal_common_data.command.req_handle,
                                  csd_hal_common_data.command.msg_id,
                                  &resp_msg, resp_len);
    if (resp_err != QMI_CSI_NO_ERR)
        rc = QMI_CSI_CB_INTERNAL_ERR;
    LOGD("%s csd_hal_end_voice() OUT\n",csd_log_time());

    return rc;
}

static int csd_hal_voice_config()
{
  return 0;
}

static int csd_hal_device_switch()
{
  int rc = 0;
  char * device_route;
  qmi_csi_error resp_err;
  uint32_t resp_len;
  qmi_csd_voice_cmd_resp_msg resp_msg = {0};

  //set device and start call
  device_route = csd_hal_get_hal_device(csd_hal_common_data.device_data.devices[0].dev_id,
                                        csd_hal_common_data.device_data.devices[1].dev_id);

  rc = csd_hal_common_data.out_stream->common.set_parameters(&csd_hal_common_data.out_stream->common, device_route);
  if (rc) {
      LOGE("set params failed %d \n",rc);
  }

  EXIT:
    LOGD("%s csd_hal_device_switch() rc=%d\n",csd_log_time(),rc);
    if (rc != 0) {
        resp_msg.u.qmi_csd_voice_resp.resp.result = QMI_RESULT_FAILURE_V01;
        resp_msg.u.qmi_csd_voice_resp.resp.error = QMI_ERR_GENERAL_V01;
        resp_msg.u.qmi_csd_voice_resp.qmi_csd_status_code_valid = true;
        resp_msg.u.qmi_csd_voice_resp.qmi_csd_status_code =
                                                    QMI_CSD_EFAILED_V01;
        resp_msg.u.qmi_csd_voice_resp.cmd_token_valid = false;
    } else {
        resp_msg.u.qmi_csd_voice_resp.qmi_csd_status_code_valid = false;
        resp_msg.u.qmi_csd_voice_resp.cmd_token_valid = true;
        resp_msg.u.qmi_csd_voice_resp.cmd_token =
                                     csd_hal_common_data.command.token;
    }

    resp_len = sizeof (resp_msg.u.qmi_csd_voice_resp);
    resp_err = qmi_csi_send_resp(csd_hal_common_data.command.req_handle,
                                 csd_hal_common_data.command.msg_id,
                                 &resp_msg, resp_len);
    if (resp_err != QMI_CSI_NO_ERR)
        rc = QMI_CSI_CB_INTERNAL_ERR;
    LOGD("%s csd_hal_device_switch() OUT\n",csd_log_time());

  return rc;
}

static int csd_hal_volume_control()
{
  int rc = 0;
  qmi_csi_error resp_err;
  uint32_t resp_len;
  qmi_csd_voice_cmd_resp_msg resp_msg = {0};
  float volume = 2 * 0.1 * csd_hal_common_data.volume_data.volume_index;

  // set volume
  rc = csd_hal_common_data.device->set_voice_volume(csd_hal_common_data.device, volume);
  if (rc) {
      LOGE("set voice volume failed %d \n",rc);
      goto EXIT;
  }

  EXIT:
    LOGD("%s csd_hal_volume_control() rc=%d\n",csd_log_time(),rc);
    if (rc != 0) {
        resp_msg.u.qmi_csd_voice_resp.resp.result = QMI_RESULT_FAILURE_V01;
        resp_msg.u.qmi_csd_voice_resp.resp.error = QMI_ERR_GENERAL_V01;
        resp_msg.u.qmi_csd_voice_resp.qmi_csd_status_code_valid = true;
        resp_msg.u.qmi_csd_voice_resp.qmi_csd_status_code =
                                                    QMI_CSD_EFAILED_V01;
        resp_msg.u.qmi_csd_voice_resp.cmd_token_valid = false;
  } else {
        resp_msg.u.qmi_csd_voice_resp.qmi_csd_status_code_valid = false;
        resp_msg.u.qmi_csd_voice_resp.cmd_token_valid = true;
        resp_msg.u.qmi_csd_voice_resp.cmd_token =
                                     csd_hal_common_data.command.token;
  }

  resp_len = sizeof (resp_msg.u.qmi_csd_voice_resp);
  resp_err = qmi_csi_send_resp(csd_hal_common_data.command.req_handle,
                                 csd_hal_common_data.command.msg_id,
                                 &resp_msg, resp_len);
  if (resp_err != QMI_CSI_NO_ERR)
        rc = QMI_CSI_CB_INTERNAL_ERR;
  LOGD("%s csd_hal_volume_control() OUT\n",csd_log_time());

return rc;
}

static int csd_hal_voice_mute()
{
  int rc = 0;
  char * device_mute;
  qmi_csi_error resp_err;
  uint32_t resp_len;
  qmi_csd_voice_cmd_resp_msg resp_msg = {0};

  device_mute = csd_hal_get_device_mute_info(csd_hal_common_data.mute_data.enable, csd_hal_common_data.mute_data.direction);

  // set device mute
  if (device_mute != NULL) {
    rc = csd_hal_common_data.device->set_parameters(csd_hal_common_data.device, device_mute);
  } else {
    LOGE("set voice mute failed \n");
    rc = -1;
    goto EXIT;
  }

  EXIT:
    LOGD("%s csd_hal_voice_mute() rc=%d\n",csd_log_time(),rc);
    if (rc != 0) {
        resp_msg.u.qmi_csd_voice_resp.resp.result = QMI_RESULT_FAILURE_V01;
        resp_msg.u.qmi_csd_voice_resp.resp.error = QMI_ERR_GENERAL_V01;
        resp_msg.u.qmi_csd_voice_resp.qmi_csd_status_code_valid = true;
        resp_msg.u.qmi_csd_voice_resp.qmi_csd_status_code =
                                                    QMI_CSD_EFAILED_V01;
        resp_msg.u.qmi_csd_voice_resp.cmd_token_valid = false;
    } else {
        resp_msg.u.qmi_csd_voice_resp.qmi_csd_status_code_valid = false;
        resp_msg.u.qmi_csd_voice_resp.cmd_token_valid = true;
        resp_msg.u.qmi_csd_voice_resp.cmd_token =
                                     csd_hal_common_data.command.token;
    }

  resp_len = sizeof (resp_msg.u.qmi_csd_voice_resp);
  resp_err = qmi_csi_send_resp(csd_hal_common_data.command.req_handle,
                                 csd_hal_common_data.command.msg_id,
                                 &resp_msg, resp_len);
  if (resp_err != QMI_CSI_NO_ERR)
        rc = QMI_CSI_CB_INTERNAL_ERR;
  LOGD("%s csd_hal_voice_mute() OUT\n",csd_log_time());

  return rc;
}

static int process_csd_to_hal_command(void)
{
    int rc = 0;

    LOGD("In Hal command processing cmd: %d \n", csd_hal_common_data.command.msg_id);
    switch (csd_hal_common_data.command.msg_id) {
    case QMI_CSD_VOICE_START_REQ_V01:
      csd_hal_start_voice();
      break;
    case QMI_CSD_VOICE_END_REQ_V01:
      csd_hal_end_voice();
      break;
    case QMI_CSD_VOICE_CONFIG_REQ_V01:
      csd_hal_voice_config();
      break;
    case QMI_CSD_VOICE_DEVICE_SWITCH_REQ_V01:
      csd_hal_device_switch();
      break;
    case QMI_CSD_VOICE_VOLUME_CONTROL_REQ_V01:
      csd_hal_volume_control();
      break;
    case QMI_CSD_VOICE_MUTE_REQ_V01:
      csd_hal_voice_mute();
    break;
    case QMI_CSD_AFE_LOOPBACK_REQ_V01:
    case QMI_CSD_DTMF_GENERATION_REQ_V01:
    case QMI_CSD_VOICE_DTMF_DETECTION_REQ_V01:
    case QMI_CSD_SET_VOICE_FEATURE_REQ_V01:
    case QMI_CSD_VOICE_RECORD_START_REQ_V01:
    case QMI_CSD_VOICE_RECORD_END_REQ_V01:
    case QMI_CSD_VOICE_PLAYBACK_START_REQ_V01:
    case QMI_CSD_VOICE_PLAYBACK_END_REQ_V01:
        LOGD("%s process_csd_to_hal_command() unsupported command:0x%08X\n",
              csd_log_time(),csd_hal_common_data.command.msg_id);
        break;
    default:
        LOGD("%s process_csd_to_hal_command() Invalid command:0x%08X\n",
              csd_log_time(),csd_hal_common_data.command.msg_id);
        break;
    }
    return rc;
}

void *hal_thread_main(void *data)
{
    struct timespec ts;

    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 1;

    while (csd_hal_common_data.thread_state != HAL_STATE_EXIT_THREAD) {
        pthread_mutex_lock(&hal_cond_mutex);
        if (csd_hal_common_data.thread_state == HAL_STATE_PROCESS_CMD) {
            LOGD("%s processing cmd in hal thread\n",csd_log_time());
            process_csd_to_hal_command();
            csd_hal_common_data.thread_state = HAL_STATE_WAIT;
        }
        pthread_cond_timedwait(&hal_condition, &hal_cond_mutex, &ts);
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += 1;
        pthread_mutex_unlock(&hal_cond_mutex);
    }

    pthread_exit(NULL);
    return NULL;
}

int csd_hal_init(void)
{
  int rc = 0;
  const char* stream_name = "voice_stream";
  audio_io_handle_t handle = 0x999;
  audio_devices_t output_device = AUDIO_DEVICE_OUT_WIRED_HEADSET;
  audio_output_flags_t flags = AUDIO_OUTPUT_FLAG_PRIMARY;
  audio_config_t config;
  csd_hal_common_data.module = NULL;
  /* Load audio HAL */

  LOGD("%s csd_hal_init: IN  \n",csd_log_time());
  rc = hw_get_module_by_class(AUDIO_HARDWARE_MODULE_ID, "primary",
                              &csd_hal_common_data.module);
  if (rc) {
    LOGE("Could not get primary hw module,%d \n",rc);
    return rc;
  }

  LOGD(" Module name is,%s\n", csd_hal_common_data.module->name);
  // Open audio device.
  csd_hal_common_data.device = NULL;
  rc = audio_hw_device_open(csd_hal_common_data.module, &csd_hal_common_data.device);
  if (rc) {
    LOGE("Could not open hw device.%d \n",rc);
    return -1;
  }

  // Open audio output stream.
  csd_hal_common_data.out_stream = NULL;
  LOGD("calling open_out_put_stream:\n");
  rc = csd_hal_common_data.device->open_output_stream(csd_hal_common_data.device, handle, output_device,
                                        flags, &config, &csd_hal_common_data.out_stream,
                                        stream_name);
  if (rc) {
    LOGE("Could not open output stream.%d \n",rc);
    return -1;
  }

  csd_hal_common_data.thread_state = HAL_STATE_WAIT;
  rc = pthread_create(&(csd_hal_common_data).thread, 0, hal_thread_main,
                 NULL);

    if (rc != 0) {
      LOGE("%s Failed to start csd_alsa thread rc=%d\n",csd_log_time(),rc);
    }
  return rc;
}

int csd_hal_deinit(void)
{
  int rc = 0;

  // close audio output stream.
  LOGD("calling close_out_put_stream:\n");
  csd_hal_common_data.device->close_output_stream(csd_hal_common_data.device,
                                                       csd_hal_common_data.out_stream);

  rc = audio_hw_device_close(csd_hal_common_data.device);

  if (rc) {
    LOGE("Could not close hw device.%d \n",rc);
  }
  return rc;
}
