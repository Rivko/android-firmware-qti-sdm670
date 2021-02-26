/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <qmi_idl_lib_internal.h>
#include <qmi_csi.h>

#include "wlan_firmware_service_v01.h"

#define CNSS_QMI_SERVER_INSTANCE_ID  0x2

struct qmi_client {
	qmi_client_handle clnt;
	unsigned int client_id;
	struct qmi_client *next;
};

static qmi_csi_service_handle service_handle;
static struct qmi_client *qmi_client;

static void add_client(qmi_client_handle handle)
{
	struct qmi_client *client, *curr;

	if (!handle) {
		printf("%s: client handle is NULL\n", __func__);
		return;
	}

	client = malloc(sizeof(struct qmi_client));
	if (!client) {
		printf("%s: fail to alloc memory for new client\n",
		       __func__);
		return;
	}
	memset(client, 0, sizeof(struct qmi_client));
	client->clnt = handle;

	if (!qmi_client) {
		qmi_client = client;
		return;
	}

	curr = qmi_client;
	while (curr->next)
		curr = curr->next;

	curr->next = client;
}

static void remove_client(qmi_client_handle handle)
{
	struct qmi_client *curr, *prev;

	if (handle == NULL) {
		printf("%s: client handle is NULL\n", __func__);
		return;
	}

	if (!qmi_client) {
		printf("%s: client list is empty\n", __func__);
		return;
	}

	curr = qmi_client;
	if (qmi_client->clnt == handle) {
		qmi_client = qmi_client->next;
		free(curr);
		return;
	}
	while (curr) {
		if (curr->clnt == handle) {
			prev->next = curr->next;
			free(curr);
			return;
		}
		prev = curr;
		curr = curr->next;
	}

	printf("%s: fail to find handle %p in client list\n",
			__func__, handle);
}

static qmi_csi_cb_error wlfw_connect_cb(qmi_client_handle client_handle,
						void *service_cookie,
						void **connection_handle)
{
	struct qmi_client *client;

	printf("%s: client %p connected\n", __func__, client_handle);

	if (!connection_handle)
		return QMI_CSI_CB_CONN_REFUSED;

	add_client(client_handle);
	*connection_handle = client_handle;

	return QMI_CSI_CB_NO_ERR;
}

static void wlfw_disconnect_cb(void *connection_handle,
				       void *service_cookie)
{
	qmi_client_handle client_handle = connection_handle;

	printf("%s: client %p disconnected\n", __func__, client_handle);

	if (client_handle)
		remove_client(client_handle);
}

static qmi_csi_cb_error wlfw_fw_ready_ind_send(void)
{
	wlfw_fw_ready_ind_msg_v01 ind;
	qmi_csi_error resp_err;
	qmi_csi_cb_error rc = QMI_CSI_CB_NO_ERR;
	struct qmi_client *client = NULL;

	memset(&ind, 0, sizeof(ind));

	client = qmi_client;
	while (client) {
		printf("Send fw_ready_ind to client %p\n", client->clnt);
		resp_err = qmi_csi_send_ind(client->clnt,
					    QMI_WLFW_FW_READY_IND_V01,
					    &ind,
					    sizeof(ind));
		if (resp_err != QMI_CSI_NO_ERR) {
			printf("Fail to send fw_ready_ind, err %d\n",
			       resp_err);
			rc = QMI_CSI_CB_INTERNAL_ERR;
		}
		client = client->next;
	}

	return rc;
}

static qmi_csi_cb_error wlfw_msa_ready_ind_send(void)
{
	wlfw_msa_ready_ind_msg_v01 ind;
	qmi_csi_error resp_err;
	qmi_csi_cb_error rc = QMI_CSI_CB_NO_ERR;
	struct qmi_client *client = NULL;

	memset(&ind, 0, sizeof(ind));

	client = qmi_client;
	while (client) {
		printf("Send msa_ready_ind to client %p\n", client->clnt);
		resp_err = qmi_csi_send_ind(client->clnt,
					    QMI_WLFW_MSA_READY_IND_V01,
					    &ind,
					    sizeof(ind));
		if (resp_err != QMI_CSI_NO_ERR) {
			printf("Fail to send msa_ready_ind, err %d\n",
			       resp_err);
			rc = QMI_CSI_CB_INTERNAL_ERR;
		}
		client = client->next;
	}

	return rc;
}

static qmi_csi_cb_error wlfw_ind_register_resp_send(qmi_req_handle req_handle,
						    void *data)
{
	wlfw_ind_register_req_msg_v01 *req;
	wlfw_ind_register_resp_msg_v01 resp;
	qmi_csi_error resp_err;
	qmi_csi_cb_error rc = QMI_CSI_CB_NO_ERR;

	req = (wlfw_ind_register_req_msg_v01 *)data;

	memset(&resp, 0, sizeof(resp));

	printf("Send ind_register_resp\n");
	resp_err = qmi_csi_send_resp(req_handle, QMI_WLFW_IND_REGISTER_RESP_V01,
				     &resp, sizeof(resp));
	if (resp_err != QMI_CSI_NO_ERR) {
		printf("Fail to send ind_register_resp, err %d\n", resp_err);
		rc = QMI_CSI_CB_INTERNAL_ERR;
	}

	return rc;
}

static qmi_csi_cb_error wlfw_msa_info_resp_send(qmi_req_handle req_handle,
						void *data)
{
	wlfw_msa_info_req_msg_v01 *req;
	wlfw_msa_info_resp_msg_v01 resp;
	qmi_csi_error resp_err;
	qmi_csi_cb_error rc = QMI_CSI_CB_NO_ERR;

	req = (wlfw_msa_info_req_msg_v01 *)data;

	memset(&resp, 0, sizeof(resp));

	printf("Send msa_info_resp\n");
	resp_err = qmi_csi_send_resp(req_handle, QMI_WLFW_MSA_INFO_RESP_V01,
				     &resp, sizeof(resp));
	if (resp_err != QMI_CSI_NO_ERR) {
		printf("Fail to send msa_info_resp, err %d\n", resp_err);
		rc = QMI_CSI_CB_INTERNAL_ERR;
	}

	return rc;
}

static qmi_csi_cb_error wlfw_cap_resp_send(qmi_req_handle req_handle,
					   void *data)
{
	wlfw_cap_req_msg_v01 *req;
	wlfw_cap_resp_msg_v01 resp;
	qmi_csi_error resp_err;
	qmi_csi_cb_error rc = QMI_CSI_CB_NO_ERR;

	req = (wlfw_cap_req_msg_v01 *)data;

	memset(&resp, 0, sizeof(resp));

	printf("Send cap_resp\n");
	resp_err = qmi_csi_send_resp(req_handle, QMI_WLFW_CAP_RESP_V01,
				     &resp, sizeof(resp));
	if (resp_err != QMI_CSI_NO_ERR) {
		printf("Fail to send cap_resp, err %d\n", resp_err);
		rc = QMI_CSI_CB_INTERNAL_ERR;
	}

	return rc;
}

static qmi_csi_cb_error wlfw_bdf_download_resp_send(qmi_req_handle req_handle,
						    void *data)
{
	wlfw_bdf_download_req_msg_v01 *req;
	wlfw_bdf_download_resp_msg_v01 resp;
	qmi_csi_error resp_err;
	qmi_csi_cb_error rc = QMI_CSI_CB_NO_ERR;

	req = (wlfw_bdf_download_req_msg_v01 *)data;

	memset(&resp, 0, sizeof(resp));

	printf("Send bdf_download_resp\n");
	resp_err = qmi_csi_send_resp(req_handle, QMI_WLFW_BDF_DOWNLOAD_RESP_V01,
				     &resp, sizeof(resp));
	if (resp_err != QMI_CSI_NO_ERR) {
		printf("Fail to send bdf_download_resp, err %d\n", resp_err);
		rc = QMI_CSI_CB_INTERNAL_ERR;
	}

	return rc;
}

static qmi_csi_cb_error wlfw_cal_report_resp_send(qmi_req_handle req_handle,
						  void *data)
{
	wlfw_cal_report_req_msg_v01 *req;
	wlfw_cal_report_resp_msg_v01 resp;
	qmi_csi_error resp_err;
	qmi_csi_cb_error rc = QMI_CSI_CB_NO_ERR;

	req = (wlfw_cal_report_req_msg_v01 *)data;

	memset(&resp, 0, sizeof(resp));

	printf("Send cal_report_resp\n");
	resp_err = qmi_csi_send_resp(req_handle, QMI_WLFW_CAL_REPORT_RESP_V01,
				     &resp, sizeof(resp));
	if (resp_err != QMI_CSI_NO_ERR) {
		printf("Fail to send cal_report_resp, err %d\n", resp_err);
		rc = QMI_CSI_CB_INTERNAL_ERR;
	}

	return rc;
}

static qmi_csi_cb_error wlfw_wlan_cfg_resp_send(qmi_req_handle req_handle,
						void *data)
{
	wlfw_wlan_cfg_req_msg_v01 *req;
	wlfw_wlan_cfg_resp_msg_v01 resp;
	qmi_csi_error resp_err;
	qmi_csi_cb_error rc = QMI_CSI_CB_NO_ERR;

	req = (wlfw_wlan_cfg_req_msg_v01 *)data;

	memset(&resp, 0, sizeof(resp));

	printf("Send wlan_cfg_resp\n");
	resp_err = qmi_csi_send_resp(req_handle, QMI_WLFW_WLAN_CFG_RESP_V01,
				     &resp, sizeof(resp));
	if (resp_err != QMI_CSI_NO_ERR) {
		printf("Fail to send wlan_cfg_resp, err %d\n", resp_err);
		rc = QMI_CSI_CB_INTERNAL_ERR;
	}

	return rc;
}

static qmi_csi_cb_error wlfw_wlan_mode_resp_send(qmi_req_handle req_handle,
						 void *data)
{
	wlfw_wlan_mode_req_msg_v01 *req;
	wlfw_wlan_mode_resp_msg_v01 resp;
	qmi_csi_error resp_err;
	qmi_csi_cb_error rc = QMI_CSI_CB_NO_ERR;

	req = (wlfw_wlan_mode_req_msg_v01 *)data;

	memset(&resp, 0, sizeof(resp));

	printf("Send wlan_mode_resp\n");
	resp_err = qmi_csi_send_resp(req_handle, QMI_WLFW_WLAN_MODE_RESP_V01,
				     &resp, sizeof(resp));
	if (resp_err != QMI_CSI_NO_ERR) {
		printf("Fail to send wlan_mode_resp, err %d\n", resp_err);
		rc = QMI_CSI_CB_INTERNAL_ERR;
	}

	return rc;
}

static qmi_csi_cb_error wlfw_process_req(void *connection_handle,
					 qmi_req_handle req_handle,
					 unsigned int msg_id,
					 void *req_c_struct,
					 unsigned int req_c_struct_len,
					 void *service_cookie)
{
	qmi_csi_cb_error rc = QMI_CSI_CB_NO_ERR;
	qmi_client_handle client_handle = connection_handle;

	printf("%s: Recevie msg: 0x%x\n", __func__, msg_id);

	switch (msg_id) {
	case QMI_WLFW_IND_REGISTER_REQ_V01:
		rc = wlfw_ind_register_resp_send(req_handle, req_c_struct);
		break;
	case QMI_WLFW_MSA_INFO_REQ_V01:
		rc = wlfw_msa_info_resp_send(req_handle, req_c_struct);
		if (rc == QMI_CSI_CB_NO_ERR)
			rc = wlfw_msa_ready_ind_send();
		break;
	case QMI_WLFW_CAP_REQ_V01:
		rc = wlfw_cap_resp_send(req_handle, req_c_struct);
		break;
	case QMI_WLFW_BDF_DOWNLOAD_REQ_V01:
		rc = wlfw_bdf_download_resp_send(req_handle, req_c_struct);
		break;
	case QMI_WLFW_CAL_REPORT_REQ_V01:
		rc = wlfw_cal_report_resp_send(req_handle, req_c_struct);
		if (rc == QMI_CSI_CB_NO_ERR)
			rc = wlfw_fw_ready_ind_send();
		break;
	case QMI_WLFW_WLAN_CFG_REQ_V01:
		rc = wlfw_wlan_cfg_resp_send(req_handle, req_c_struct);
		break;
	case QMI_WLFW_WLAN_MODE_REQ_V01:
		rc = wlfw_wlan_mode_resp_send(req_handle, req_c_struct);
		break;
	default:
		printf("%s: unsupported msg\n", __func__);
		rc = QMI_CSI_CB_INTERNAL_ERR;
		break;
	}
	return rc;
}

int main(int argc, char **argv)
{
	qmi_csi_os_params os_params, os_params_in;
	fd_set fds;
	qmi_csi_options qmi_options;

	qmi_idl_service_object_type
		wlfw_service_object = wlfw_get_service_object_v01();
	qmi_csi_error rc = QMI_CSI_INTERNAL_ERR;

	QMI_CSI_OPTIONS_INIT(qmi_options);
	QMI_CSI_OPTIONS_SET_INSTANCE_ID(qmi_options,
					CNSS_QMI_SERVER_INSTANCE_ID);
	printf("Register WLFW Server with instance ID 0x%x\n",
	       CNSS_QMI_SERVER_INSTANCE_ID);
	rc = qmi_csi_register_with_options(wlfw_service_object, wlfw_connect_cb,
					   wlfw_disconnect_cb, wlfw_process_req,
					   &service_handle, &os_params,
					   &qmi_options, &service_handle);
	if (rc != QMI_CSI_NO_ERR)
		return rc;

	printf("WLFW Server is up, waiting for client\n");

	while (1) {
		fds = os_params.fds;
		select(os_params.max_fd+1, &fds, NULL, NULL, NULL);
		os_params_in.fds = fds;
		qmi_csi_handle_event(service_handle, &os_params_in);
	}
	qmi_csi_unregister(service_handle);
	return 0;
}
