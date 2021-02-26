/*
 * Copyright (c) 2014-2015,2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <qmi_idl_lib_internal.h>
#include <qmi_client_instance_defs.h>

#include "nl_loop.h"
#include "debug.h"
#include "cnss_qmi_client.h"
#include "wireless_lan_proxy_service_v01.h"

#define MAX_VDEV_NUM 8

struct cnss_client_data{
	qmi_idl_service_object_type wlan_service_object;
	qmi_client_type client_notifier_handler;
	qmi_client_type client_handler;
	pthread_t thread_id;
	int service_active;
	struct wlan_status_data current_status_data[MAX_VDEV_NUM];
	struct wlan_version_data current_version_data;
	int data_valid;
} cnss_client;

static struct ind_handlers {
	int ind;
	nl_loop_ind_handler handler;
	void *user_data;
} ind_handlers [] = {
	{
		.ind = WLAN_MSG_WLAN_STATUS_IND,
		.handler = wlan_service_process_msg,
	},
	{
		.ind = WLAN_MSG_WLAN_VERSION_IND,
		.handler = wlan_service_process_msg,
	},
};

int wlan_send_version_msg(struct wlan_version_data *data)
{
	int rc;
	wlps_update_client_version_req_msg_v01 req;
	wlps_update_client_version_resp_msg_v01 resp;

	wsvc_printf_dbg("%s: Sending wlan version msg: chip_id 0x%x, chip_name %s, chip_from %s, host_version %s, fw_version %s",
			__func__, data->chip_id, data->chip_name,
			data->chip_from, data->host_version, data->fw_version);

	memset(&req, 0, sizeof(req));
	memset(&resp, 0, sizeof(resp));
	req.chip_id = data->chip_id;
	strlcpy(req.chip_name, data->chip_name, QMI_WLPS_MAX_STR_LEN_V01);
	strlcpy(req.chip_from, data->chip_from, QMI_WLPS_MAX_STR_LEN_V01);
	strlcpy(req.host_version,
		data->host_version,
		QMI_WLPS_MAX_STR_LEN_V01);
	strlcpy(req.fw_version, data->fw_version, QMI_WLPS_MAX_STR_LEN_V01);

	rc = qmi_client_send_msg_sync(cnss_client.client_handler,
				      QMI_WLPS_UPDATE_CLIENT_VERSION_REQ_V01,
				      &req,
				      sizeof(req),
				      &resp,
				      sizeof(resp),
				      TIMEOUT_MS);
	if (rc < 0) {
		wsvc_printf_err("%s: Send client version req failed %d\n",
				__func__, rc);
		return rc;
	}

	if (resp.resp.result != QMI_RESULT_SUCCESS_V01) {
		wsvc_printf_err("%s: client version req rejected, result %d, error %d",
				__func__,
				resp.resp.result,
				resp.resp.error);
		rc = resp.resp.result;
		return rc;
	}
	return 0;
}

int wlan_send_status_msg(struct wlan_status_data *data)
{
	int rc;
	wlps_update_client_status_req_msg_v01 req;
	wlps_update_client_status_resp_msg_v01 resp;
	uint32_t i;

	wsvc_printf_dbg("%s: Sending wlan status msg: fw_adsp_support 0x%x, is_on 0x%x",
			__func__, data->lpss_support, data->is_on);

	memset(&req, 0, sizeof(req));
	memset(&resp, 0, sizeof(resp));

	req.fw_adsp_support = data->lpss_support;
	req.is_on = data->is_on;
	if (req.fw_adsp_support) {
		req.vdev_info_valid = 1;
		req.vdev_info.vdev_id = data->vdev_id;
		req.vdev_info.vdev_mode = data->vdev_mode;
		req.vdev_conn_info_valid = 1;
		req.vdev_conn_info.is_connected = data->is_connected;
		req.vdev_conn_info.rssi = data->rssi;
		req.vdev_conn_info.ssid_len = data->ssid_len;
		req.vdev_conn_info.freq = data->freq;
		req.channel_info_valid = 1;
		req.channel_info_len = data->num_channels;
		memcpy(req.vdev_conn_info.country_code,
		       data->country_code,
		       QMI_WLPS_COUNTRY_CODE_LEN_V01);
		memcpy(req.vdev_conn_info.ssid,
		       data->ssid,
		       QMI_WLPS_MAX_SSID_LEN_V01);
		memcpy(req.vdev_conn_info.bssid,
		       data->bssid,
		       QMI_WLPS_MAX_BSSID_LEN_V01);
		memcpy(req.channel_info,
		       data->channel_list,
		       QMI_WLPS_MAX_NUM_CHAN_V01);
		req.rtt_oem_channel_info_valid = 1;
		if (data->num_channels < WLAN_MSG_MAX_NUM_CHAN)
			req.rtt_oem_channel_info_len = data->num_channels;
		else
			req.rtt_oem_channel_info_len =
				WLAN_MSG_MAX_NUM_CHAN;
		for (i = 0; i < req.rtt_oem_channel_info_len; i++) {
			req.rtt_oem_channel_info[i].chan_number =
				data->channel_info[i].chan_number;
			req.rtt_oem_channel_info[i].mhz =
				data->channel_info[i].mhz;
			req.rtt_oem_channel_info[i].band_center_freq1 =
				data->channel_info[i].band_center_freq1;
			req.rtt_oem_channel_info[i].band_center_freq2 =
				data->channel_info[i].band_center_freq2;
			req.rtt_oem_channel_info[i].info =
				data->channel_info[i].info;
			req.rtt_oem_channel_info[i].reg_info_1 =
				data->channel_info[i].reg_info_1;
			req.rtt_oem_channel_info[i].reg_info_2 =
				data->channel_info[i].reg_info_2;
		}
	}

	rc = qmi_client_send_msg_sync(cnss_client.client_handler,
				      QMI_WLPS_UPDATE_CLIENT_STATUS_REQ_V01,
				      &req,
				      sizeof(req),
				      &resp,
				      sizeof(resp),
				      TIMEOUT_MS);
	if (rc < 0) {
		wsvc_printf_err("%s: Send client status req failed %d\n",
				__func__, rc);
		return rc;
	}

	if (resp.resp.result != QMI_RESULT_SUCCESS_V01) {
		wsvc_printf_err("%s: client status req rejected, result %d, error %d",
				__func__,
				resp.resp.result,
				resp.resp.error);
		rc = resp.resp.result;
		return rc;
	}
	return 0;
}

static void wlan_qmi_err_cb(qmi_client_type user_handle,
		   qmi_client_error_type error,
		   void *err_cb_data)
{
	UNUSED(err_cb_data);
	wsvc_printf_info("%s: WLPS service disconnect, called with error %d for client %pK",
		     __func__, error, (void *)user_handle);
	if (user_handle == NULL)
		return;
	(void *)err_cb_data;
	wlan_service_shutdown();
	wlan_service_init();
}

void * wlan_service_request()
{
	int rc;
	qmi_cci_os_signal_type os_params;
	qmi_service_info info;
	int i;

	wsvc_printf_dbg("%s: Start the pthread", __func__);

	/* release any old handlers for client */
	if (cnss_client.client_notifier_handler) {
		qmi_client_release(cnss_client.client_notifier_handler);
		cnss_client.client_notifier_handler = NULL;
	}
	if (cnss_client.client_handler) {
		qmi_client_release(cnss_client.client_handler);
		cnss_client.client_handler = NULL;
	}

	cnss_client.wlan_service_object = wlps_get_service_object_v01();

	rc = qmi_client_notifier_init(cnss_client.wlan_service_object,
				      &os_params,
				      &(cnss_client.client_notifier_handler));
	if (rc != QMI_NO_ERR) {
		wsvc_printf_err("Failed to initialize notifier");
		goto exit;
	}

	QMI_CCI_OS_SIGNAL_WAIT(&os_params, 0);
	QMI_CCI_OS_SIGNAL_CLEAR(&os_params);

	wsvc_printf_info("WLPS service connected");

	rc = qmi_client_get_any_service(cnss_client.wlan_service_object,
					&info);
	if (rc != QMI_NO_ERR) {
		wsvc_printf_err("Failed to get service instance");
		goto exit;
	}

	rc = qmi_client_init(&info,
			     cnss_client.wlan_service_object,
			     NULL,
			     NULL,
			     &os_params,
			     &(cnss_client.client_handler));
	if (rc != QMI_NO_ERR) {
		wsvc_printf_err("Failed to initialize client");
		goto exit;
	}
	qmi_client_release(cnss_client.client_notifier_handler);
	cnss_client.client_notifier_handler = NULL;

	rc = qmi_client_register_error_cb(cnss_client.client_handler,
					  wlan_qmi_err_cb,
					  &cnss_client);
	if (rc != QMI_NO_ERR) {
		wsvc_printf_err("Failed to register error cb");
		return NULL;
	}

	if (cnss_client.data_valid) {
		for (i = 0; i < MAX_VDEV_NUM; i++) {
			if (cnss_client.current_status_data[i].is_on)
				wlan_send_status_msg(&(cnss_client.current_status_data[i]));
		}
		wlan_send_version_msg(&(cnss_client.current_version_data));
	}

	cnss_client.service_active = 1;
	return NULL;

exit:
	if (cnss_client.client_notifier_handler) {
		qmi_client_release(cnss_client.client_notifier_handler);
		cnss_client.client_notifier_handler = NULL;
	}

	return NULL;
}

int wlan_service_init(void)
{
	int rc;

	rc = pthread_create(&(cnss_client.thread_id), NULL,
			    wlan_service_request, NULL);
	if (0 != rc) {
		wsvc_printf_err("Failed to request wlan service");
		return -1;
	}

	return 0;
}

int wlan_service_start(void)
{
	int rc,i;
	for (i = 0; i < (int)ARRAY_SIZE(ind_handlers); i++) {
		if (nl_loop_register(ind_handlers[i].ind,
				     ind_handlers[i].handler,
				     &cnss_client)) {
			wsvc_printf_err("Failed to register: %d, %x", i,
					ind_handlers[i].ind);
			return -1;
		}
	}

	rc = wlan_service_init();
	if (0 != rc)
		while (--i >= 0)
			nl_loop_unregister(ind_handlers[i].ind);
	return rc;
}

void wlan_service_release(void)
{
	pthread_join(cnss_client.thread_id, NULL);
	cnss_client.service_active = 0;
	if (cnss_client.client_handler) {
		qmi_client_release(cnss_client.client_handler);
		cnss_client.client_handler = NULL;
	}
	if (cnss_client.client_notifier_handler) {
		qmi_client_release(cnss_client.client_notifier_handler);
		cnss_client.client_notifier_handler = NULL;
	}
}

void wlan_service_shutdown(void)
{
	pthread_join(cnss_client.thread_id, NULL);
	cnss_client.service_active = 0;
}

void wlan_service_stop(void)
{
	size_t i = 0;
	cnss_client.data_valid = 0;
	wlan_service_release();
	for (i = 0; i < (int)ARRAY_SIZE(ind_handlers); i++)
		     nl_loop_unregister(ind_handlers[i].ind);
	cnss_client.client_handler = NULL;
	cnss_client.client_notifier_handler = NULL;
}

int wlan_service_process_msg(int type, void *data, int len)
{
	int rc = 0;
	struct wlan_status_data *p_data = NULL;
	if (!data) {
		wsvc_printf_err("%s: NULL data pointer",
				__func__);
		return -1;
	}

	wsvc_printf_dbg("%s: Receive msg 0x%x", __func__, type);

	switch (type) {
	case WLAN_MSG_WLAN_STATUS_IND:
		if (len != sizeof(struct wlan_status_data)) {
			wsvc_printf_err("Invalid len %d for %x", len, type);
			return -1;
		}
		p_data = (struct wlan_status_data *)data;
		if (!(p_data->is_on)) {
			memset(&(cnss_client.current_status_data[0]), 0, len * MAX_VDEV_NUM);
			cnss_client.data_valid = 0;
		} else {
			memcpy(&(cnss_client.current_status_data[p_data->vdev_id]), data, len);
			cnss_client.data_valid = 1;
		}
		rc = (cnss_client.service_active ?
		      wlan_send_status_msg((struct wlan_status_data *)data) :
		      0);
		break;
	case WLAN_MSG_WLAN_VERSION_IND:
		if (len != sizeof(struct wlan_version_data)) {
			wsvc_printf_err("Invalid len %d for %x", len, type);
			return -1;
		}
		memcpy(&(cnss_client.current_version_data), data, len);
		rc = (cnss_client.service_active ?
		      wlan_send_version_msg((struct wlan_version_data *)data) :
		      0);
		break;
	default:
		return rc;
	}

	return rc;
}
