/* Copyright (c) 2012-2015, 2017 Qualcomm Technologies Inc.
   All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <stdbool.h>
#include <csd_server.h>
#include "csd_hal.h"

#define CSD_DRIVER_VER ((CSD_VER_MAIN<<24)+(CSD_VER_MINOR<<16)+CSD_VER_REV)

extern pthread_mutex_t hal_cond_mutex;
extern pthread_cond_t  hal_condition;
extern struct csd_hal_common_data csd_hal_common_data;

static int csd_server_get_csd_version()
{
 LOGD("%s : version is %d\n",csd_log_time(), CSD_DRIVER_VER);
 return CSD_DRIVER_VER;
}

static qmi_csi_cb_error csd_server_get_version(struct client_data *clnt_data,
                                               qmi_req_handle req_handle,
                                               int32_t msg_id,
                                               void *req_c_struct)
{
    qmi_csi_cb_error qmi_rc = QMI_CSI_CB_NO_ERR;
    qmi_csi_error resp_err;
    qmi_csd_query_driver_version_resp_msg_v01 resp = {0};
    uint32_t  rc = 0;

    resp.csd_version = csd_server_get_csd_version();
    resp.csd_version_valid = true;

    resp_err = qmi_csi_send_resp(req_handle, msg_id, &resp, sizeof(resp));
    if (resp_err != QMI_CSI_NO_ERR) {
        LOGD("%s csd_server_get_version qmi_csi_send_resp error=0x%x\n",csd_log_time(), resp_err);
        rc = QMI_CSI_CB_INTERNAL_ERR;
    }

    LOGD("%s csd_server: csd_server_get_version EXIT\n",csd_log_time());
    return rc;
}

static qmi_csi_cb_error csd_server_init(struct client_data *clnt_data,
                                        qmi_req_handle req_handle,
                                        int32_t msg_id,
                                        void *req_c_struct)
{
    qmi_csi_cb_error qmi_rc = QMI_CSI_CB_NO_ERR;
    qmi_csi_error resp_err;
    qmi_csd_init_resp_msg_v01 resp = {0};
    uint32_t  rc = 0;

    LOGD("%s csd_server: csd_server_init IN  \n",csd_log_time());
    if (qmi_csd_init  != QMI_CSD_INIT_DONE) {
        rc = csd_hal_init();
    } else {
        LOGD("%s csd_server: CSD already initialized\n",csd_log_time());
    }
EXIT:
    LOGD("%s csd_server:csd_alsa_init rc=%d\n",csd_log_time(),rc);

    if (rc != 0) {
        resp.qmi_csd_status_code_valid = true;
        resp.qmi_csd_status_code =  QMI_CSD_EFAILED_V01;
        resp.resp.result = QMI_RESULT_FAILURE_V01;
        resp.resp.error = QMI_ERR_GENERAL_V01;
        qmi_csd_init  = QMI_CSD_INIT_FAILED; /* CSD init failed  */
    } else {
        qmi_csd_init  = QMI_CSD_INIT_DONE;
    }
    resp_err = qmi_csi_send_resp(req_handle, msg_id, &resp, sizeof(resp));

    if (resp_err != QMI_CSI_NO_ERR) {
        LOGD("%s csd_server:csd_server_init => qmi_csi_send_resp returned\n"
             "error=0x%x\n",csd_log_time(),resp_err);
        rc = QMI_CSI_CB_INTERNAL_ERR;
    } else {
        rc = QMI_CSI_CB_NO_ERR;
    }
    LOGD("%s csd_server:csd_server_init => msg_id=0x%x EXIT rc=%d\n",
         csd_log_time(),msg_id, rc);
    return rc;
}

static qmi_csi_cb_error csd_server_deinit(struct client_data *clnt_data,
                                          qmi_req_handle req_handle,
                                          int32_t msg_id,
                                          void *req_c_struct)
{
    qmi_csi_cb_error qmi_rc = QMI_CSI_CB_NO_ERR;
    qmi_csi_error resp_err;
    qmi_csd_deinit_resp_msg_v01 resp;
    uint32_t  rc;

    LOGD("%s csd_server: csd_server_deinit IN  \n",csd_log_time());
    rc = csd_hal_deinit();
    if (rc != 0) {
        qmi_csd_init  = QMI_CSD_DINIT_FAILED;
        LOGE("%s csd_alsa_deinit() failed rc=%d\n",csd_log_time(),rc);
    }

    qmi_csd_init  = QMI_CSD_DINIT_DONE;

EXIT:
    LOGD("%s csd_server_deinit() error=0x%x\n",csd_log_time(), rc);
    memset(&resp, 0, sizeof(resp));

    if (rc != 0) {
        resp.qmi_csd_status_code_valid = true;
        resp.qmi_csd_status_code = QMI_CSD_EFAILED_V01;
        resp.resp.result = QMI_RESULT_FAILURE_V01;
        resp.resp.error = QMI_ERR_GENERAL_V01;
        qmi_csd_init  = QMI_CSD_DINIT_FAILED;  /* CSD De Init failed  */
    } else {
        qmi_csd_init  = QMI_CSD_DINIT_DONE;
    }

    resp_err = qmi_csi_send_resp(req_handle,
                                 msg_id,
                                 &resp,
                                 sizeof(qmi_csd_deinit_resp_msg_v01));

    if (resp_err != QMI_CSI_NO_ERR) {
        LOGD("%s csd_server:csd_server_deinit => qmi_csi_send_resp returned\n"
             "error=0x%x\n",csd_log_time(), resp_err);
        rc = QMI_CSI_CB_INTERNAL_ERR;
    }
    LOGD("%s csd_server: csd_server_deinit EXIT  \n",csd_log_time());
    return rc;
}

qmi_csi_cb_error csd_server_open_device_control(struct client_data *clnt_data,
                                                qmi_req_handle req_handle,
                                                int32_t msg_id,
                                                void *req_c_struct)
{
  qmi_csi_error resp_err;
  uint32_t dev_handle;
  qmi_csd_open_device_control_resp_msg_v01 resp;
  int rc = 0;

  memset(&resp, 0, sizeof(qmi_csd_open_device_control_resp_msg_v01));
  LOGD("%s csd_create_device_handle() rc:%d dev_handle:%08X\n",csd_log_time(), rc, dev_handle);

  if (rc != 0) {
      LOGD("%s csd_server:csd_open_device_control_req_handler Error\n",csd_log_time());
      resp.resp.result = QMI_RESULT_FAILURE_V01;
      resp.resp.error = QMI_ERR_GENERAL_V01;
      resp.open_status_valid = true;
      resp.open_status = (qmi_csd_status_v01) (QMI_CSD_EFAILED_V01);
      resp.qmi_csd_device_handle_valid = false;
  } else {
      /* open succeed  */
      resp.open_status_valid = false;
      resp.qmi_csd_device_handle_valid = true;
      resp.qmi_csd_device_handle = dev_handle;
  }

  resp_err = qmi_csi_send_resp(req_handle, msg_id, &resp, sizeof(resp));
  if (resp_err != QMI_CSI_NO_ERR) {
      LOGE("%s csd_server:csd_open_device_control qmi_csi_send_resp error=%d\n",
             csd_log_time(),resp_err);
      rc = QMI_CSI_CB_INTERNAL_ERR;
  } else {
      rc = QMI_CSI_CB_NO_ERR;
  }
  LOGD("%s csd_server_open_device_control() EXIT rc=%d\n",csd_log_time(), rc);
  return rc;
}

qmi_csi_cb_error csd_server_voice_start(struct client_data *clnt_data,
                                                    qmi_req_handle req_handle,
                                                    int32_t msg_id,
                                                    void *req_c_struct)
{
    qmi_csi_cb_error qmi_rc = QMI_CSI_CB_NO_ERR;
    qmi_csd_voice_cmd_req_msg *qmi_csd_voice_req =
                                     (qmi_csd_voice_cmd_req_msg *)req_c_struct;
    static char sess_id[SESSION_NAME_MAX_LEN_V01 + 1];
    qmi_csd_voice_cmd_resp_msg resp_msg = {0};
    uint32_t resp_msg_size = 0;
    qmi_csi_error resp_err;
    int rc = 0;

    LOGD("%s csd_server_voice_start IN\n",csd_log_time());

    pthread_mutex_lock(&hal_cond_mutex);
    memset(&csd_hal_common_data.command, 0, sizeof(struct csd_hal_command_data));
    csd_hal_common_data.command.token =
                           qmi_csd_voice_req->u.qmi_csd_voice_start_req.cmd_token;
    csd_hal_common_data.command.msg_id = msg_id;
    csd_hal_common_data.command.req_handle = req_handle;
    csd_hal_common_data.thread_state = HAL_STATE_PROCESS_CMD;
    csd_hal_common_data.command.command_client_data = clnt_data;

    csd_hal_common_data.device_data.devices[0].dev_id = qmi_csd_voice_req->u.qmi_csd_voice_start_req.tx_device_info.dev_id;
    csd_hal_common_data.device_data.devices[0].dev_attrib.sample_rate = qmi_csd_voice_req->u.qmi_csd_voice_start_req.tx_device_info.sample_rate;
    csd_hal_common_data.device_data.devices[0].dev_attrib.bits_per_sample = qmi_csd_voice_req->u.qmi_csd_voice_start_req.tx_device_info.bits_per_sample;

    csd_hal_common_data.device_data.devices[1].dev_id = qmi_csd_voice_req->u.qmi_csd_voice_start_req.rx_device_info.dev_id;
    csd_hal_common_data.device_data.devices[1].dev_attrib.sample_rate = qmi_csd_voice_req->u.qmi_csd_voice_start_req.rx_device_info.sample_rate;
    csd_hal_common_data.device_data.devices[1].dev_attrib.bits_per_sample = qmi_csd_voice_req->u.qmi_csd_voice_start_req.rx_device_info.bits_per_sample;

    strlcpy( sess_id, qmi_csd_voice_req->u.qmi_csd_voice_start_req.session_id, sizeof(qmi_csd_voice_req->u.qmi_csd_voice_start_req.session_id) );
    csd_hal_common_data.session_id = sess_id;

    pthread_cond_signal(&hal_condition);
    pthread_mutex_unlock(&hal_cond_mutex);

    if (rc != 0)
        LOGE("%s csd_hal_start_voice FAILED rc=%d\n",csd_log_time(),rc);

EXIT:
    LOGD("%s csd_server_voice_start_voice() rc=%d\n",csd_log_time(),rc);
    if (rc != 0) {
        LOGE("%s CSD_VOICE_START_VOICE FAILED rc=%d\n",csd_log_time(),rc);
        resp_msg.u.qmi_csd_voice_resp.resp.result =
                                                         QMI_RESULT_FAILURE_V01;
        resp_msg.u.qmi_csd_voice_resp.resp.error = QMI_ERR_GENERAL_V01;
        resp_msg.u.qmi_csd_voice_resp.qmi_csd_status_code_valid = true;
        resp_msg.u.qmi_csd_voice_resp.qmi_csd_status_code =
                                                            QMI_CSD_EFAILED_V01;
        resp_msg.u.qmi_csd_voice_resp.cmd_token_valid = false;
        resp_msg_size = sizeof(resp_msg.u.qmi_csd_voice_resp);
        resp_err = qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                                     resp_msg_size);
        if (resp_err != QMI_CSI_NO_ERR) {
            LOGD("%s csd_server_voice_start() send_resp error=%d\n",
                 csd_log_time(),resp_err);
            rc = QMI_CSI_CB_INTERNAL_ERR;
        } else {
            rc = QMI_CSI_CB_NO_ERR;
        }
        LOGD("%s csd_server_voice_start() EXIT rc=%d\n",csd_log_time(),rc);
        return rc;
    }
    LOGD("%s csd_server_voice_start() EXIT \n",csd_log_time());
    return QMI_CSI_CB_NO_ERR;
}

qmi_csi_cb_error csd_server_voice_end(struct client_data *clnt_data,
                                                    qmi_req_handle req_handle,
                                                    int32_t msg_id,
                                                    void *req_c_struct)
{
    qmi_csi_cb_error qmi_rc = QMI_CSI_CB_NO_ERR;
    qmi_csd_voice_cmd_req_msg *qmi_csd_voice_req =
                                     (qmi_csd_voice_cmd_req_msg *)req_c_struct;
    qmi_csd_voice_cmd_resp_msg resp_msg = {0};
    uint32_t resp_msg_size = 0;
    qmi_csi_error resp_err;
    int rc = 0;

    LOGD("%s csd_server_voice_end IN\n",csd_log_time());

    pthread_mutex_lock(&hal_cond_mutex);
    memset(&csd_hal_common_data.command, 0, sizeof(struct csd_hal_command_data));
    csd_hal_common_data.command.token =
                           qmi_csd_voice_req->u.qmi_csd_voice_end_req.cmd_token;
    csd_hal_common_data.command.msg_id = msg_id;
    csd_hal_common_data.command.req_handle = req_handle;
    csd_hal_common_data.thread_state = HAL_STATE_PROCESS_CMD;
    csd_hal_common_data.command.command_client_data = clnt_data;
    pthread_cond_signal(&hal_condition);
    pthread_mutex_unlock(&hal_cond_mutex);

    if (rc != 0)
        LOGE("%s csd_hal_start_end FAILED rc=%d\n",csd_log_time(),rc);

EXIT:
    LOGD("%s csd_server_voice_end_voice() rc=%d\n",csd_log_time(),rc);
    if (rc != 0) {
        LOGE("%s CSD_VOICE_END_VOICE FAILED rc=%d\n",csd_log_time(),rc);
        resp_msg.u.qmi_csd_voice_resp.resp.result =
                                                         QMI_RESULT_FAILURE_V01;
        resp_msg.u.qmi_csd_voice_resp.resp.error = QMI_ERR_GENERAL_V01;
        resp_msg.u.qmi_csd_voice_resp.qmi_csd_status_code_valid = true;
        resp_msg.u.qmi_csd_voice_resp.qmi_csd_status_code =
                                                            QMI_CSD_EFAILED_V01;
        resp_msg.u.qmi_csd_voice_resp.cmd_token_valid = false;
        resp_msg_size = sizeof(resp_msg.u.qmi_csd_voice_resp);
        resp_err = qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                                     resp_msg_size);
        if (resp_err != QMI_CSI_NO_ERR) {
            LOGD("%s csd_server_voice_end() send_resp error=%d\n",
                 csd_log_time(),resp_err);
            rc = QMI_CSI_CB_INTERNAL_ERR;
        } else {
            rc = QMI_CSI_CB_NO_ERR;
        }
        LOGD("%s csd_server_voice_end() EXIT rc=%d\n",csd_log_time(),rc);
        return rc;
    }
    LOGD("%s csd_server_voice_end() EXIT \n",csd_log_time());
    return QMI_CSI_CB_NO_ERR;
}

qmi_csi_cb_error csd_server_voice_config(struct client_data *clnt_data,
                                                    qmi_req_handle req_handle,
                                                    int32_t msg_id,
                                                    void *req_c_struct)
{
    qmi_csi_cb_error qmi_rc = QMI_CSI_CB_NO_ERR;
    qmi_csd_voice_cmd_req_msg *qmi_csd_voice_req =
                                     (qmi_csd_voice_cmd_req_msg *)req_c_struct;
    qmi_csd_voice_cmd_resp_msg resp_msg = {0};
    uint32_t resp_msg_size = 0;
    qmi_csi_error resp_err;
    int rc = 0;

    LOGD("%s csd_server_voice_config IN\n",csd_log_time());

    pthread_mutex_lock(&hal_cond_mutex);
    memset(&csd_hal_common_data.command, 0, sizeof(struct csd_hal_command_data));
    csd_hal_common_data.command.token =
                           qmi_csd_voice_req->u.qmi_csd_voice_config_req.cmd_token;
    csd_hal_common_data.command.msg_id = msg_id;
    csd_hal_common_data.command.req_handle = req_handle;
    csd_hal_common_data.thread_state = HAL_STATE_PROCESS_CMD;
    csd_hal_common_data.command.command_client_data = clnt_data;

    pthread_cond_signal(&hal_condition);
    pthread_mutex_unlock(&hal_cond_mutex);

    if (rc != 0)
        LOGE("%s csd_hal_voice_config FAILED rc=%d\n",csd_log_time(),rc);

EXIT:
    LOGD("%s csd_server_voice_config_voice() rc=%d\n",csd_log_time(),rc);
    if (rc != 0) {
        LOGE("%s CSD_VOICE_CONFIG FAILED rc=%d\n",csd_log_time(),rc);
        resp_msg.u.qmi_csd_voice_resp.resp.result =
                                                         QMI_RESULT_FAILURE_V01;
        resp_msg.u.qmi_csd_voice_resp.resp.error = QMI_ERR_GENERAL_V01;
        resp_msg.u.qmi_csd_voice_resp.qmi_csd_status_code_valid = true;
        resp_msg.u.qmi_csd_voice_resp.qmi_csd_status_code =
                                                            QMI_CSD_EFAILED_V01;
        resp_msg.u.qmi_csd_voice_resp.cmd_token_valid = false;
        resp_msg_size = sizeof(resp_msg.u.qmi_csd_voice_resp);
        resp_err = qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                                     resp_msg_size);
        if (resp_err != QMI_CSI_NO_ERR) {
            LOGD("%s csd_server_voice_config() send_resp error=%d\n",
                 csd_log_time(),resp_err);
            rc = QMI_CSI_CB_INTERNAL_ERR;
        } else {
            rc = QMI_CSI_CB_NO_ERR;
        }
        LOGD("%s csd_server_voice_config() EXIT rc=%d\n",csd_log_time(),rc);
        return rc;
    }
    LOGD("%s csd_server_voice_config() EXIT \n",csd_log_time());
    return QMI_CSI_CB_NO_ERR;
}

qmi_csi_cb_error csd_server_device_switch(struct client_data *clnt_data,
                                                    qmi_req_handle req_handle,
                                                    int32_t msg_id,
                                                    void *req_c_struct)
{
    qmi_csi_cb_error qmi_rc = QMI_CSI_CB_NO_ERR;
    qmi_csd_voice_cmd_req_msg *qmi_csd_voice_req =
                                 (qmi_csd_voice_cmd_req_msg *)req_c_struct;
    qmi_csd_voice_cmd_resp_msg resp_msg = {0};
    uint32_t resp_msg_size = 0;
    qmi_csi_error resp_err;
    int rc = 0;

    LOGD("%s csd_server_voice_device_switch IN\n",csd_log_time());

    pthread_mutex_lock(&hal_cond_mutex);
    memset(&csd_hal_common_data.command, 0, sizeof(struct csd_hal_command_data));
    csd_hal_common_data.command.token =
                        qmi_csd_voice_req->u.qmi_csd_voice_device_switch_req.cmd_token;
    csd_hal_common_data.command.msg_id = msg_id;
    csd_hal_common_data.command.req_handle = req_handle;
    csd_hal_common_data.thread_state = HAL_STATE_PROCESS_CMD;
    csd_hal_common_data.command.command_client_data = clnt_data;

    csd_hal_common_data.device_data.devices[0].dev_id = qmi_csd_voice_req->u.qmi_csd_voice_device_switch_req.tx_device_info.dev_id;
    csd_hal_common_data.device_data.devices[0].dev_attrib.sample_rate = qmi_csd_voice_req->u.qmi_csd_voice_device_switch_req.tx_device_info.sample_rate;
    csd_hal_common_data.device_data.devices[0].dev_attrib.bits_per_sample = qmi_csd_voice_req->u.qmi_csd_voice_device_switch_req.tx_device_info.bits_per_sample;

    csd_hal_common_data.device_data.devices[1].dev_id = qmi_csd_voice_req->u.qmi_csd_voice_device_switch_req.rx_device_info.dev_id;
    csd_hal_common_data.device_data.devices[1].dev_attrib.sample_rate = qmi_csd_voice_req->u.qmi_csd_voice_device_switch_req.rx_device_info.sample_rate;
    csd_hal_common_data.device_data.devices[1].dev_attrib.bits_per_sample = qmi_csd_voice_req->u.qmi_csd_voice_device_switch_req.rx_device_info.bits_per_sample;

    if (qmi_csd_voice_req->u.qmi_csd_voice_device_switch_req.ec_ref_device_info_valid)
    {
        csd_hal_common_data.device_data.devices[2].dev_id = qmi_csd_voice_req->u.qmi_csd_voice_device_switch_req.ec_ref_device_info.dev_id;
        csd_hal_common_data.device_data.devices[2].dev_attrib.sample_rate = qmi_csd_voice_req->u.qmi_csd_voice_device_switch_req.ec_ref_device_info.sample_rate;
        csd_hal_common_data.device_data.devices[2].dev_attrib.bits_per_sample = qmi_csd_voice_req->u.qmi_csd_voice_device_switch_req.ec_ref_device_info.bits_per_sample;
    }

    pthread_cond_signal(&hal_condition);
    pthread_mutex_unlock(&hal_cond_mutex);

    if (rc != 0)
        LOGE("%s csd_server_voice_device_switch FAILED rc=%d\n",csd_log_time(),rc);

EXIT:
    LOGD("%s csd_server_voice_device_switch() rc=%d\n",csd_log_time(),rc);
    if (rc != 0) {
        LOGE("%s CSD_VOICE_DEVICE_SWITCH FAILED rc=%d\n",csd_log_time(),rc);
        resp_msg.u.qmi_csd_voice_resp.resp.result =
                                                QMI_RESULT_FAILURE_V01;
        resp_msg.u.qmi_csd_voice_resp.resp.error = QMI_ERR_GENERAL_V01;
        resp_msg.u.qmi_csd_voice_resp.qmi_csd_status_code_valid = true;
        resp_msg.u.qmi_csd_voice_resp.qmi_csd_status_code =
                                                QMI_CSD_EFAILED_V01;
        resp_msg.u.qmi_csd_voice_resp.cmd_token_valid = false;
        resp_msg_size = sizeof(resp_msg.u.qmi_csd_voice_resp);
        resp_err = qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                                                 resp_msg_size);
        if (resp_err != QMI_CSI_NO_ERR) {
            LOGD("%s csd_server_voice_device_switch() send_resp error=%d\n",
                                   csd_log_time(),resp_err);
            rc = QMI_CSI_CB_INTERNAL_ERR;
        } else {
            rc = QMI_CSI_CB_NO_ERR;
        }
        LOGD("%s csd_server_voice_device_switch() EXIT rc=%d\n",csd_log_time(),rc);
        return rc;
    }
    LOGD("%s csd_server_voice_device_switch() EXIT \n",csd_log_time());
    return QMI_CSI_CB_NO_ERR;
}

qmi_csi_cb_error csd_server_voice_volume_control(struct client_data *clnt_data,
                                                    qmi_req_handle req_handle,
                                                    int32_t msg_id,
                                                    void *req_c_struct)
{
    qmi_csi_cb_error qmi_rc = QMI_CSI_CB_NO_ERR;
    qmi_csd_voice_cmd_req_msg *qmi_csd_voice_req =
                                 (qmi_csd_voice_cmd_req_msg *)req_c_struct;
    qmi_csd_voice_cmd_resp_msg resp_msg = {0};
    uint32_t resp_msg_size = 0;
    qmi_csi_error resp_err;
    int rc = 0;

    LOGD("%s csd_server_voice_volume_control IN\n",csd_log_time());

    pthread_mutex_lock(&hal_cond_mutex);
    memset(&csd_hal_common_data.command, 0, sizeof(struct csd_hal_command_data));
    csd_hal_common_data.command.token =
                        qmi_csd_voice_req->u.qmi_csd_voice_volume_control_req.cmd_token;
    csd_hal_common_data.command.msg_id = msg_id;
    csd_hal_common_data.command.req_handle = req_handle;
    csd_hal_common_data.thread_state = HAL_STATE_PROCESS_CMD;
    csd_hal_common_data.command.command_client_data = clnt_data;

    csd_hal_common_data.volume_data.volume_index = qmi_csd_voice_req->u.qmi_csd_voice_volume_control_req.volume_index;
    csd_hal_common_data.volume_data.ramp_duration_in_ms = qmi_csd_voice_req->u.qmi_csd_voice_volume_control_req.ramp_duration_in_ms;

    pthread_cond_signal(&hal_condition);
    pthread_mutex_unlock(&hal_cond_mutex);

    if (rc != 0)
        LOGE("%s csd_server_voice_volume_control FAILED rc=%d\n",csd_log_time(),rc);

EXIT:
    LOGD("%s csd_server_voice_volume_control() rc=%d\n",csd_log_time(),rc);
    if (rc != 0) {
        LOGE("%s CSD_VOICE_VOLUME_CONTROL FAILED rc=%d\n",csd_log_time(),rc);
        resp_msg.u.qmi_csd_voice_resp.resp.result =
                                                QMI_RESULT_FAILURE_V01;
        resp_msg.u.qmi_csd_voice_resp.resp.error = QMI_ERR_GENERAL_V01;
        resp_msg.u.qmi_csd_voice_resp.qmi_csd_status_code_valid = true;
        resp_msg.u.qmi_csd_voice_resp.qmi_csd_status_code =
                                                QMI_CSD_EFAILED_V01;
        resp_msg.u.qmi_csd_voice_resp.cmd_token_valid = false;
        resp_msg_size = sizeof(resp_msg.u.qmi_csd_voice_resp);
        resp_err = qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                                                 resp_msg_size);
        if (resp_err != QMI_CSI_NO_ERR) {
            LOGD("%s csd_server_voice_volume_control() send_resp error=%d\n",
                                   csd_log_time(),resp_err);
            rc = QMI_CSI_CB_INTERNAL_ERR;
        } else {
            rc = QMI_CSI_CB_NO_ERR;
        }
        LOGD("%s csd_server_voice_volume_control() EXIT rc=%d\n",csd_log_time(),rc);
        return rc;
    }
    LOGD("%s csd_server_voice_volume_control() EXIT \n",csd_log_time());
    return QMI_CSI_CB_NO_ERR;
}

qmi_csi_cb_error csd_server_voice_mute(struct client_data *clnt_data,
                                                    qmi_req_handle req_handle,
                                                    int32_t msg_id,
                                                    void *req_c_struct)
{
    qmi_csi_cb_error qmi_rc = QMI_CSI_CB_NO_ERR;
    qmi_csd_voice_cmd_req_msg *qmi_csd_voice_req =
                                 (qmi_csd_voice_cmd_req_msg *)req_c_struct;
    qmi_csd_voice_cmd_resp_msg resp_msg = {0};
    uint32_t resp_msg_size = 0;
    qmi_csi_error resp_err;
    int rc = 0;

    LOGD("%s csd_server_voice_mute IN\n",csd_log_time());

    pthread_mutex_lock(&hal_cond_mutex);
    memset(&csd_hal_common_data.command, 0, sizeof(struct csd_hal_command_data));
    csd_hal_common_data.command.token =
                        qmi_csd_voice_req->u.qmi_csd_voice_volume_control_req.cmd_token;
    csd_hal_common_data.command.msg_id = msg_id;
    csd_hal_common_data.command.req_handle = req_handle;
    csd_hal_common_data.thread_state = HAL_STATE_PROCESS_CMD;
    csd_hal_common_data.command.command_client_data = clnt_data;

    csd_hal_common_data.mute_data.mode = qmi_csd_voice_req->u.qmi_csd_voice_mute_req.mode;
    csd_hal_common_data.mute_data.enable = qmi_csd_voice_req->u.qmi_csd_voice_mute_req.enable;
    csd_hal_common_data.mute_data.direction = qmi_csd_voice_req->u.qmi_csd_voice_mute_req.direction;
    csd_hal_common_data.mute_data.ramp_duration_in_ms = qmi_csd_voice_req->u.qmi_csd_voice_mute_req.ramp_duration_in_ms;

    pthread_cond_signal(&hal_condition);
    pthread_mutex_unlock(&hal_cond_mutex);

    if (rc != 0)
        LOGE("%s csd_server_voice_mute FAILED rc=%d\n",csd_log_time(),rc);

EXIT:
    LOGD("%s csd_server_voice_mute() rc=%d\n",csd_log_time(),rc);
    if (rc != 0) {
        LOGE("%s CSD_VOICE_MUTE FAILED rc=%d\n",csd_log_time(),rc);
        resp_msg.u.qmi_csd_voice_resp.resp.result =
                                                QMI_RESULT_FAILURE_V01;
        resp_msg.u.qmi_csd_voice_resp.resp.error = QMI_ERR_GENERAL_V01;
        resp_msg.u.qmi_csd_voice_resp.qmi_csd_status_code_valid = true;
        resp_msg.u.qmi_csd_voice_resp.qmi_csd_status_code =
                                                QMI_CSD_EFAILED_V01;
        resp_msg.u.qmi_csd_voice_resp.cmd_token_valid = false;
        resp_msg_size = sizeof(resp_msg.u.qmi_csd_voice_resp);
        resp_err = qmi_csi_send_resp(req_handle, msg_id, &resp_msg,
                                                 resp_msg_size);
        if (resp_err != QMI_CSI_NO_ERR) {
            LOGD("%s csd_server_voice_mute() send_resp error=%d\n",
                                   csd_log_time(),resp_err);
            rc = QMI_CSI_CB_INTERNAL_ERR;
        } else {
            rc = QMI_CSI_CB_NO_ERR;
        }
        LOGD("%s csd_server_voice_mute() EXIT rc=%d\n",csd_log_time(),rc);
        return rc;
    }
    LOGD("%s csd_server_voice_mute() EXIT \n",csd_log_time());
    return QMI_CSI_CB_NO_ERR;
}

static qmi_csi_cb_error csd_server_connect_cb(qmi_client_handle client_handle,
                                              void *service_cookie,
                                              void **connection_handle)
{
    /* TO DO: Implement function to handle multiple clients */
    return 0;
}

static void csd_server_disconnect_cb(void *connection_handle,
                                     void *service_cookie)
{
    /* TO DO: Implement function to handle multiple clients */
}

static qmi_csi_cb_error csd_server_handle_req_cb(void *connection_handle,
                                                 qmi_req_handle req_handle,
                                                 unsigned int msg_id,
                                                 void *req_c_struct,
                                                 unsigned int req_c_struct_len,
                                                 void *service_cookie)
{
    qmi_csi_cb_error rc = QMI_CSI_CB_NO_ERR;
    qmi_csi_error resp_err;
    struct client_data *clnt_data = (struct client_data *)connection_handle;
    /*
     * A request is being handled, increment the service_cookie num_requests
     */
    ((service_context_type *)service_cookie)->num_requests++;
    LOGD("%s csd_server_handle_req_cb() IN\n",csd_log_time());

    if (((msg_id != QMI_CSD_INIT_REQ_V01) &&
         (msg_id != QMI_CSD_DEINIT_REQ_V01) &&
         (msg_id != QMI_CSD_QUERY_DRIVER_VERSION_REQ_V01)) &&
        QMI_CSD_INIT_DONE != qmi_csd_init) {
        LOGE("%s Failed CSD_QMI Not Initialized\n",csd_log_time());
        return QMI_CSI_CB_INTERNAL_ERR;
    }

    switch (msg_id) {
      case QMI_CSD_QUERY_DRIVER_VERSION_REQ_V01:
           rc = csd_server_get_version(clnt_data, req_handle, msg_id, req_c_struct);
           break;

       case QMI_CSD_INIT_REQ_V01:
            rc = csd_server_init(clnt_data, req_handle, msg_id, req_c_struct);
            break;

        case QMI_CSD_DEINIT_REQ_V01:
             rc = csd_server_deinit(clnt_data, req_handle, msg_id,
                                    req_c_struct);
             break;
        case QMI_CSD_OPEN_DEVICE_CONTROL_REQ_V01:
             rc = csd_server_open_device_control(clnt_data, req_handle, msg_id,
                                    req_c_struct);
             break;
        case QMI_CSD_VOICE_START_REQ_V01:
             rc = csd_server_voice_start(clnt_data,req_handle,
                                             msg_id, req_c_struct);
            break;
        case QMI_CSD_VOICE_END_REQ_V01:
	        rc = csd_server_voice_end(clnt_data,req_handle,
                                             msg_id, req_c_struct);
            break;
        case QMI_CSD_VOICE_CONFIG_REQ_V01:
            rc = csd_server_voice_config(clnt_data,req_handle,
                                             msg_id, req_c_struct);
            break;
        case QMI_CSD_VOICE_DEVICE_SWITCH_REQ_V01:
            rc = csd_server_device_switch(clnt_data,req_handle,
                                             msg_id, req_c_struct);
            break;
        case QMI_CSD_VOICE_VOLUME_CONTROL_REQ_V01:
            rc = csd_server_voice_volume_control(clnt_data,req_handle,
                                             msg_id, req_c_struct);
            break;
        case QMI_CSD_VOICE_MUTE_REQ_V01:
            rc = csd_server_voice_mute(clnt_data,req_handle,
                                             msg_id, req_c_struct);
            break;
        case QMI_CSD_CLOSE_REQ_V01:
        case QMI_CSD_IOCTL_DEV_CMD_ENABLE_REQ_V01:
        case QMI_CSD_IOCTL_DEV_CMD_AFE_LOOPBACK_REQ_V01:
        case QMI_CSD_OPEN_PASSIVE_CONTROL_VOICE_STREAM_REQ_V01:
        case QMI_CSD_OPEN_VOICE_CONTEXT_REQ_V01:
        case QMI_CSD_OPEN_VOICE_MANAGER_REQ_V01:
        case QMI_CSD_IOCTL_VC_CMD_SET_DEVICE_CONFIG_REQ_V01:
        case QMI_CSD_IOCTL_VC_CMD_ENABLE_REQ_V01:
        case QMI_CSD_IOCTL_VM_CMD_ATTACH_CONTEXT_REQ_V01:
        case QMI_CSD_IOCTL_VM_CMD_SET_WIDEVOICE_REQ_V01:
        case QMI_CSD_IOCTL_VM_CMD_SET_HDVOICE_MODE_REQ_V01:
        case QMI_CSD_IOCTL_VM_CMD_SET_TTY_MODE_REQ_V01:
        case QMI_CSD_IOCTL_VS_CMD_SET_UI_PROPERTY_REQ_V01:
        case QMI_CSD_IOCTL_VM_CMD_START_VOICE_REQ_V01:
        case QMI_CSD_IOCTL_VC_CMD_SET_RX_VOLUME_INDEX_REQ_V01:
        case QMI_CSD_IOCTL_VS_CMD_SET_MUTE_REQ_V01:
        case QMI_CSD_IOCTL_VM_CMD_STOP_VOICE_REQ_V01:
        case QMI_CSD_IOCTL_VM_CMD_DETACH_CONTEXT_REQ_V01:
        case QMI_CSD_IOCTL_VC_CMD_DISABLE_REQ_V01:
        case QMI_CSD_IOCTL_DEV_CMD_DISABLE_REQ_V01:
        case QMI_CSD_IOCTL_VM_CMD_STANDBY_VOICE_REQ_V01:
        default:
            LOGE("%s Message ID:[%08X] not implemented yet.\n",csd_log_time(),msg_id);
            rc = QMI_CSI_CB_INTERNAL_ERR;
    }
    LOGD("%s Message ID:[%08X] rc=[%08X]  EXIT\n",csd_log_time(),msg_id, rc);
    return rc;
}

void *csd_server_register_service(qmi_csi_os_params *os_params)
{
    qmi_idl_service_object_type csd_service_object =
                                                  csd_get_service_object_v01();
    qmi_client_os_params qsap_os_params;
    qmi_csi_error rc = QMI_CSI_INTERNAL_ERR;

    rc = qmi_csi_register(csd_service_object, csd_server_connect_cb,
                          csd_server_disconnect_cb, csd_server_handle_req_cb,
                          &service_cookie, os_params,
                          &service_cookie.service_handle);
    LOGD("%s qmi_csi_register() rc=%d\n",csd_log_time(),rc);

    rc |= qmi_sap_register(csd_service_object, &qsap_os_params,
                           &service_cookie.qsap_handle);
    LOGD("%s qmi_sap_register() rc=%d\n",csd_log_time(),rc);

    if (rc != QMI_NO_ERR)
        return NULL;

    return service_cookie.service_handle;
}

char* csd_log_time(void)
{
  static char time_log[50];
  struct timeval time;
  memset(time_log,0,sizeof(time_log));
  gettimeofday(&time,NULL);
  sprintf(time_log,"[%d:%06d]",(time.tv_sec),time.tv_usec);
  return time_log;
}

int main(int argc, char **argv)
{
    /* on-target intiialization code */
    qmi_csi_os_params os_params, os_params_in;
    fd_set fds;
    void *sp;

    LOGD("%s Calling csd_server_register_servicet \n",csd_log_time());
    sp = csd_server_register_service(&os_params);

    if (!sp) {
        LOGE("%s Unable to register service!\n",csd_log_time());
        exit(1);
    } else {
        LOGD("%s csd-server is up\n",csd_log_time());
    }

    while(1) {
        fds = os_params.fds;
        select(os_params.max_fd + 1, &fds, NULL, NULL, NULL);
        os_params_in.fds = fds;
        qmi_csi_handle_event(sp, &os_params_in);
    }
    qmi_csi_unregister(sp);

    return 0;
}/* main */
