/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include <qmi_idl_lib_internal.h>
#include <qmi_client_instance_defs.h>
/* peripheral manager */
#include <pm-service.h>
#include <mdm_detect.h>

#include "debug.h"
#include "wlfw_qmi_client.h"
#include "wlan_firmware_service_v01.h"
#include "device_management_service_v01.h"

#ifndef BIT
#define BIT(x) (1 << (x))
#endif

#define WLFW_CLIENT_ID		0x444d4f4e
#define WLFW_TIMEOUT_MS		(10000)
#define WLFW_MAX_FILE_LEN	(512*1024)
#define MAX_FILE_NAME_LEN	15
#define MAX_FILE_PATH		100
#define DEFAULT_BDF_FILE	"bdwlan.bin"
#define BDF_FILE		"bdwlan."
#define CAL_FILE		"/data/misc/wifi/wlfw_cal_"
#define FALSE			1
#define WLFW_MAX_BDF_RETRY	2
#define CNSS_WLFW_QMI_CONNECTED		BIT(0)
#define CNSS_MSA_READY			BIT(1)
#define CNSS_BDF_DOWNLOAD_DONE		BIT(2)

#define CNSS_IS_WLFW_QMI_CONNECTED(_state) ((_state) & CNSS_WLFW_QMI_CONNECTED)
#define CNSS_IS_MSA_READY(_state) ((_state) & CNSS_MSA_READY)
#define CNSS_IS_BDF_DOWNLOAD_DONE(_state) ((_state) & CNSS_BDF_DOWNLOAD_DONE)

#define WLAN_BOARD_ID_INDEX 0x100
#define QMI_ERR_PLAT_CCPM_CLK_INIT_FAILED 0x77

const char *bdf_dir_list[] = {"/firmware/image/", "/vendor/firmware/", NULL};

static int svc_running = 0;

struct ind_data {
	unsigned int msg_id;
	union wlfw_inds {
		wlfw_initiate_cal_download_ind_msg_v01 init_cal_download_ind;
		wlfw_initiate_cal_update_ind_msg_v01 init_cal_update_ind;
	} data;
	struct ind_data *next;
};

struct cal_data {
	wlfw_cal_temp_id_enum_v01 cal_id;
	uint32_t total_size;
	uint8_t *data;
};

struct wlan_mac_addr {
	uint8_t mac_addr_valid;
	uint8_t mac_addr[QMI_WLFW_MAC_ADDR_SIZE_V01];
};

struct wlfw_client_data {
	qmi_idl_service_object_type svc_obj;
	qmi_client_type clnt_handler;
	void *pm_handle;
	uint32_t state;
	pthread_t thread_id;
	pthread_t thread_id_dms;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	pthread_cond_t cond_rsp;
	pthread_mutex_t dms_mutex;
	pthread_cond_t dms_cond;
	struct ind_data *head;
	struct ind_data *tail;
	struct cal_data cal_data_array[QMI_WLFW_MAX_NUM_CAL_V01];
	wlfw_rf_chip_info_s_v01 chip_info;
	wlfw_rf_board_info_s_v01 board_info;
	wlfw_soc_info_s_v01 soc_info;
	wlfw_fw_version_info_s_v01 fw_version_info;
	struct wlan_mac_addr mac_addr;
} gdata;

int pm_vote(void)
{
	int ret = 0;

	if (!gdata.pm_handle) {
		wsvc_printf_err("%s: pm_handle is NULL", __func__);
		ret = -1;
		goto out;
	}

	wsvc_printf_dbg("%s: Vote for internal modem", __func__);

	ret = pm_client_connect(gdata.pm_handle);
	if (ret)
		wsvc_printf_err("%s: Failed to vote, ret = %d",
				__func__, ret);

out:
	return ret;
}

void pm_release_vote(void)
{
	if (!gdata.pm_handle) {
		wsvc_printf_err("%s: pm_handle is NULL", __func__);
		return;
	}

	wsvc_printf_dbg("%s: Release vote for internal modem", __func__);

	pm_client_disconnect(gdata.pm_handle);
}

void pm_event_notifier(void *client_cookie, enum pm_event event)
{
	wsvc_printf_dbg("%s: PM event %d received", __func__, event);
	(void *)client_cookie;
	pm_client_event_acknowledge(gdata.pm_handle, event);
}

int pm_init(void)
{
	struct dev_info devinfo;
	int i, ret;
	enum pm_event event;
	int found = 0;

	wsvc_printf_dbg("%s: Starting pm service", __func__);
	ret = get_system_info(&devinfo);
	if (ret != RET_SUCCESS) {
		wsvc_printf_err("%s: Failed to get device info, ret = %d",
				__func__, ret);
		goto out;
	}

	wsvc_printf_dbg("%s: devinfo.num_modems = %d\n",
			__func__, devinfo.num_modems);
	for (i = 0; i < devinfo.num_modems; i++) {
		if (devinfo.mdm_list[i].type == MDM_TYPE_INTERNAL) {
			wsvc_printf_dbg("%s: Found internal modem: %s",
					__func__, devinfo.mdm_list[i].mdm_name);
			ret = pm_client_register(pm_event_notifier,
						 gdata.pm_handle,
						 devinfo.mdm_list[i].mdm_name,
						 "cnss-daemon",
						 &event,
						 &gdata.pm_handle);
			if (ret != PM_RET_SUCCESS) {
				wsvc_printf_err("%s: Failed to register pm client, ret = %d",
						__func__, ret);
				continue;
			}
			found = 1;
			break;
		}
	}

	if (found == 0) {
		ret = -1;
		goto out;
	}

	ret = pm_vote();

out:
	return ret;
}

void pm_deinit(void)
{
	wsvc_printf_dbg("%s: Exit pm service\n", __func__);

	pm_release_vote();

	if (gdata.pm_handle)
		pm_client_unregister(gdata.pm_handle);
	gdata.pm_handle = NULL;
}

void ind_data_enqueue(struct ind_data *i_data)
{
	if (i_data == NULL) {
		wsvc_printf_err("%s: Invalid data", __func__);
		return;
	}

	pthread_mutex_lock(&(gdata.mutex));
	if (gdata.head == NULL) {
		gdata.head = i_data;
		gdata.tail = i_data;
	} else {
		(gdata.tail)->next = i_data;
		gdata.tail = i_data;
	}
	pthread_mutex_unlock(&(gdata.mutex));
}

void ind_data_dequeue(struct ind_data **i_data)
{
	pthread_mutex_lock(&(gdata.mutex));
	if (gdata.head == NULL) {
		*i_data = NULL;
	} else if ((gdata.head)->next == NULL) {
		*i_data = gdata.head;
		gdata.head = NULL;
		gdata.tail = NULL;
	} else {
		*i_data = gdata.head;
		gdata.head = (gdata.head)->next;
	}
	pthread_mutex_unlock(&(gdata.mutex));
}

void ind_data_free_queue(void)
{
	struct ind_data *i_data = gdata.head;

	pthread_mutex_lock(&(gdata.mutex));
	while (i_data != NULL) {
		i_data = gdata.head;
		gdata.head = (gdata.head)->next;
		free(i_data);
	}
	gdata.head = NULL;
	gdata.tail = NULL;
	pthread_mutex_unlock(&(gdata.mutex));
}

static int wlfw_send_ind_register_req(uint64_t *fw_status)
{
	int rc;
	wlfw_ind_register_req_msg_v01 req;
	wlfw_ind_register_resp_msg_v01 resp;
	unsigned int i;

	wsvc_printf_dbg("%s", __func__);

	memset(&req, 0, sizeof(req));
	memset(&resp, 0, sizeof(resp));

	req.client_id_valid = 1;
	req.client_id = WLFW_CLIENT_ID;
	req.fw_ready_enable_valid = 1;
	req.fw_ready_enable = 1;
	req.initiate_cal_download_enable_valid = 1;
	req.initiate_cal_download_enable = 1;
	req.initiate_cal_update_enable_valid = 1;
	req.initiate_cal_update_enable = 1;
	req.msa_ready_enable_valid = 1;
	req.msa_ready_enable = 1;

	rc = qmi_client_send_msg_sync(gdata.clnt_handler,
				      QMI_WLFW_IND_REGISTER_REQ_V01,
				      &req,
				      sizeof(req),
				      &resp,
				      sizeof(resp),
				      WLFW_TIMEOUT_MS);

	if ((rc != QMI_NO_ERR) ||
	    (resp.resp.result != QMI_RESULT_SUCCESS_V01)) {
		wsvc_printf_err("%s: rc %d, result %d, error %d",
				__func__,
				rc,
				resp.resp.result,
				resp.resp.error);
		if (resp.resp.result)
			rc = -resp.resp.result;
		goto out;
	}

	if (resp.fw_status_valid)
		*fw_status = resp.fw_status;

	wsvc_printf_dbg("%s: result %d, error %d, fw_status_valid %u, fw_status 0x%lx",
			__func__,
			resp.resp.result,
			resp.resp.error,
			resp.fw_status_valid,
			resp.fw_status);
out:
	return rc;
}

static int wlfw_send_cap_req(void)
{
	int rc;
	wlfw_cap_req_msg_v01 req;
	wlfw_cap_resp_msg_v01 resp;

	wsvc_printf_dbg("%s", __func__);

	memset(&resp, 0, sizeof(resp));

	rc = qmi_client_send_msg_sync(gdata.clnt_handler,
				      QMI_WLFW_CAP_REQ_V01,
				      &req,
				      sizeof(req),
				      &resp,
				      sizeof(resp),
				      WLFW_TIMEOUT_MS);

	if ((rc != QMI_NO_ERR) ||
	    (resp.resp.result != QMI_RESULT_SUCCESS_V01)) {
		wsvc_printf_err("%s: rc %d, result %d, error %d",
				__func__,
				rc,
				resp.resp.result,
				resp.resp.error);
		if (resp.resp.error == QMI_ERR_PLAT_CCPM_CLK_INIT_FAILED) {
			wsvc_printf_err("RF card Not Present");
			rc = -EAGAIN;
			goto out;
		}
		if (resp.resp.result)
			rc = -resp.resp.result;
		goto out;
	}

	wsvc_printf_dbg("%s: result %d, error %d", __func__,
				resp.resp.result,
				resp.resp.error);

	/* store cap locally */
	if (resp.chip_info_valid)
		gdata.chip_info = resp.chip_info;
	if (resp.board_info_valid)
		gdata.board_info = resp.board_info;
	else
		gdata.board_info.board_id = 0xFF;
	if (resp.soc_info_valid)
		gdata.soc_info = resp.soc_info;
	if (resp.fw_version_info_valid)
		gdata.fw_version_info = resp.fw_version_info;

	wsvc_printf_info("%s: chip_id: 0x%0x, chip_family 0x%x,"
			 " board_id: 0x%0x, soc_id: 0x%0x,"
			 " fw_version: 0x%0x, fw_build_timestamp: %s",
			 __func__,
			 gdata.chip_info.chip_id,
			 gdata.chip_info.chip_family,
			 gdata.board_info.board_id,
			 gdata.soc_info.soc_id,
			 gdata.fw_version_info.fw_version,
			 gdata.fw_version_info.fw_build_timestamp);

out:
	return rc;
}

static int wlfw_send_bdf_download_req(void)
{
	int i, rc = FALSE;
	wlfw_bdf_download_req_msg_v01 req;
	wlfw_bdf_download_resp_msg_v01 resp;
	FILE *file = NULL;
	unsigned char *buffer, *temp;
	unsigned int file_len, remaining;
	char filename[MAX_FILE_NAME_LEN];
	char filefullpath[MAX_FILE_PATH];

	wsvc_printf_dbg("%s", __func__);

	/* retrive bdf file name from capability */
	if (0xFF == gdata.board_info.board_id) {
		snprintf(filename, sizeof(filename),
			 DEFAULT_BDF_FILE);
	} else {
		if (gdata.board_info.board_id >= WLAN_BOARD_ID_INDEX) {
			snprintf(filename, sizeof(filename),
				 BDF_FILE"%03x",
				 gdata.board_info.board_id);
		} else {
			snprintf(filename, sizeof(filename),
				 BDF_FILE"b%02x",
				 gdata.board_info.board_id);
		}
	}
	wsvc_printf_dbg("%s: BDF file : %s", __func__, filename);

	for (i = 0; bdf_dir_list[i]; i++) {
		snprintf(filefullpath, sizeof(filefullpath),
			"%s%s", bdf_dir_list[i], filename);
		wsvc_printf_dbg("%s: Looking for BDF file: %s",
			__func__, filefullpath);
		file = fopen(filefullpath, "rb");
		if (file != NULL) {
			wsvc_printf_dbg("%s: BDF file found: %s",
			__func__, filefullpath);
			break;
		}
	}
	if (!file) {
		wsvc_printf_err("%s: Failed to find BDF file %s", __func__,
			filename);
		rc = -1;
		goto end;
	}

	fseek(file, 0, SEEK_END);
	file_len = ftell(file);
	fseek(file, 0, SEEK_SET);

	buffer = (unsigned char *)malloc(file_len);
	if (buffer == NULL) {
		wsvc_printf_err("%s: Fail to alloc memory for BDF file",
					 __func__);
		rc = -1;
		goto memory_alloc_fail;
	}
	if (fread(buffer, 1, file_len, file) != file_len) {
		wsvc_printf_err("%s: Fail to read BDF file", __func__);
		rc = -1;
		goto read_fail;
	}
	fclose(file);

	memset(&req, 0, sizeof(req));
	memset(&resp, 0, sizeof(resp));

	temp = buffer;
	remaining = file_len;
	while (remaining) {
		req.valid = 1;
		/* file id should retrive from capability */
		req.file_id_valid = 1;
		req.file_id = 0;
		req.total_size_valid = 1;
		req.total_size = file_len;
		req.seg_id_valid = 1;
		req.data_valid = 1;
		req.end_valid = 1;
		if (remaining > QMI_WLFW_MAX_DATA_SIZE_V01) {
			req.data_len = QMI_WLFW_MAX_DATA_SIZE_V01;
		} else {
			req.data_len = remaining;
			req.end = 1;
		}
		memcpy(req.data, temp, req.data_len);

		rc = qmi_client_send_msg_sync(gdata.clnt_handler,
					      QMI_WLFW_BDF_DOWNLOAD_REQ_V01,
					      &req,
					      sizeof(req),
					      &resp,
					      sizeof(resp),
					      WLFW_TIMEOUT_MS);
		if (rc != QMI_NO_ERR) {
			wsvc_printf_err("%s: rc %d, result %d, error %d",
					__func__,
					rc,
					resp.resp.result,
					resp.resp.error);
			rc = FALSE;
			goto out;
		}

		remaining -= req.data_len;
		temp += req.data_len;
		req.seg_id++;
	}

	wsvc_printf_dbg("%s: result %d, error %d", __func__,
				resp.resp.result,
				resp.resp.error);
out:
        free(buffer);
        buffer = NULL;
        if (QMI_ERR_INVALID_ARG_V01 == resp.resp.error) {
		wsvc_printf_dbg("%s: Response failure : 0x%0x",
				__func__, resp.resp.error);
		rc = FALSE;
        }
	return rc;

read_fail:
	free(buffer);
	buffer = NULL;
memory_alloc_fail:
	fclose(file);
end:
	return rc;
}

static int wlfw_send_cal_report_req(void)
{
	int rc;
	wlfw_cal_report_req_msg_v01 req;
	wlfw_cal_report_resp_msg_v01 resp;
	int i, j = 0;

	wsvc_printf_dbg("%s", __func__);

	memset(&req, 0, sizeof(req));
	memset(&resp, 0, sizeof(resp));

	for(i = 0; i < QMI_WLFW_MAX_NUM_CAL_V01; i++) {
		if (gdata.cal_data_array[i].total_size &&
		    gdata.cal_data_array[i].data) {
			req.meta_data[j] = gdata.cal_data_array[i].cal_id;
			j++;
		}
	}
	req.meta_data_len = j;

	rc = qmi_client_send_msg_sync(gdata.clnt_handler,
				      QMI_WLFW_CAL_REPORT_REQ_V01,
				      &req,
				      sizeof(req),
				      &resp,
				      sizeof(resp),
				      WLFW_TIMEOUT_MS);
	if ((rc != QMI_NO_ERR) ||
	    (resp.resp.result != QMI_RESULT_SUCCESS_V01)) {
		wsvc_printf_err("%s: rc %d, result %d, error %d",
				__func__,
				rc,
				resp.resp.result,
				resp.resp.error);
		if (resp.resp.result)
			rc = -resp.resp.result;
		goto out;
	}

	wsvc_printf_dbg("%s: result %d, error %d", __func__,
				resp.resp.result,
				resp.resp.error);

out:
	return rc;
}

static int wlfw_send_mac_addr_req(void)
{
	int rc;
	wlfw_mac_addr_req_msg_v01 req;
	wlfw_mac_addr_resp_msg_v01 resp;

	memset(&req, 0, sizeof(req));
	memset(&resp, 0, sizeof(resp));

	req.mac_addr_valid = 1;
	memcpy(req.mac_addr, gdata.mac_addr.mac_addr,
	       QMI_WLFW_MAC_ADDR_SIZE_V01);

	wsvc_printf_mac_addr(MSG_DEBUG, "Sending MAC address to FW",
			     gdata.mac_addr.mac_addr);

	rc = qmi_client_send_msg_sync(gdata.clnt_handler,
				      QMI_WLFW_MAC_ADDR_REQ_V01,
				      &req,
				      sizeof(req),
				      &resp,
				      sizeof(resp),
				      WLFW_TIMEOUT_MS);
	if ((rc != QMI_NO_ERR) ||
	    (resp.resp.result != QMI_RESULT_SUCCESS_V01)) {
		wsvc_printf_err("Failed to send MAC address: rc %d, result %d, error %d",
				rc,
				resp.resp.result,
				resp.resp.error);
		if (resp.resp.result)
			rc = -resp.resp.result;
		goto out;
	}

	wsvc_printf_dbg("%s: result %d, error %d", __func__,
			resp.resp.result,
			resp.resp.error);

out:
	return rc;
}

static struct cal_data *find_cal_file(wlfw_cal_temp_id_enum_v01 cal_id)
{
	int i;

	for (i = 0; i < QMI_WLFW_MAX_NUM_CAL_V01; i++) {
		if (gdata.cal_data_array[i].cal_id == cal_id) {
			if (gdata.cal_data_array[i].total_size &&
			    gdata.cal_data_array[i].data)
				return &gdata.cal_data_array[i];
		}
	}

	return NULL;
}

static void free_cal_file(void)
{
	int i;

	for (i = 0; i < QMI_WLFW_MAX_NUM_CAL_V01; i++) {
		gdata.cal_data_array[i].cal_id = 0;
		gdata.cal_data_array[i].total_size = 0;
		if (gdata.cal_data_array[i].data) {
			free(gdata.cal_data_array[i].data);
			gdata.cal_data_array[i].data = NULL;
		}
	}
}

int wlfw_save_file(int cal_id, unsigned char *data, int len)
{
	unsigned char *pcal;
	int len_written = 0;
	char filename[MAX_FILE_PATH];
	FILE *fp = NULL;

	wsvc_printf_dbg("%s", __func__);

	if (len > WLFW_MAX_FILE_LEN) {
		wsvc_printf_err("%s: Unexpected file len %d", __func__, len);
		return -1;
	}

	snprintf(filename, sizeof(filename), CAL_FILE"%02d.bin", cal_id);

	wsvc_printf_dbg("%s: Storing Cal data: %s ", __func__, filename);

	fp = fopen(filename, "wb");
	if (fp == NULL) {
		wsvc_printf_err("%s: Failed to open file %s", __func__, filename);
		return -1;
	}

	len_written = fwrite(data, 1, len, fp);
	if (len_written != len) {
		wsvc_printf_err("%s: invalid write %d: %d", __func__,
			len_written, len);
		fclose(fp);
		return -1;
	}
	fclose(fp);

	return len_written;
}

int wlfw_read_file(char *filename, unsigned char **p_cal_read)
{
	unsigned char *pcal;
	int len;
	int len_read = 0;
	FILE *fp = NULL;

	if (access(filename, F_OK) == -1) {
		wsvc_printf_info("%s: No such file %s", __func__, filename);
		return -1;
	}

	fp = fopen(filename, "rb");
	if (fp == NULL) {
		wsvc_printf_err("%s: Failed to open file %s", __func__, filename);
		return -1;
	}

	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	if (len > WLFW_MAX_FILE_LEN) {
		wsvc_printf_err("%s: Unexpected file len %d", __func__, len);
		fclose(fp);
		return -1;
	}

	pcal = malloc(len);
	if (pcal == NULL) {
		wsvc_printf_err("%s: Failed to alloc mem", __func__);
		fclose(fp);
		return -1;
	}

	len_read = fread(pcal, 1, len, fp);
	if (len_read != len) {
		wsvc_printf_err("%s: invalid read %d: %d", __func__,
			len_read, len);
		fclose(fp);
		return -1;
	}
	fclose(fp);

	*p_cal_read = pcal;
	return len_read;
}

static int wlfw_send_cal_download_req(wlfw_cal_temp_id_enum_v01 cal_id)
{
	int rc = QMI_NO_ERR;
	wlfw_cal_download_req_msg_v01 req;
	wlfw_cal_download_resp_msg_v01 resp;
	struct cal_data *p_cal_data = NULL;
	unsigned char *temp;
	unsigned int remaining;

	wsvc_printf_dbg("%s", __func__);

	memset(&req, 0, sizeof(req));
	memset(&resp, 0, sizeof(resp));

	p_cal_data = find_cal_file(cal_id);
	if (p_cal_data == NULL) {
		req.total_size_valid = 1;
		rc = qmi_client_send_msg_sync(gdata.clnt_handler,
					      QMI_WLFW_CAL_DOWNLOAD_REQ_V01,
					      &req,
					      sizeof(req),
					      &resp,
					      sizeof(resp),
					      WLFW_TIMEOUT_MS);
		if (rc != QMI_NO_ERR)
			wsvc_printf_err("%s: rc %d, result %d, error %d",
					__func__,
					rc,
					resp.resp.result,
					resp.resp.error);

		wsvc_printf_dbg("%s: result %d, error %d", __func__,
					resp.resp.result,
					resp.resp.error);
		return rc;
	}

	temp = p_cal_data->data;
	remaining = p_cal_data->total_size;
	while (remaining) {
		req.valid = 1;
		req.file_id_valid = 1;
		req.file_id = p_cal_data->cal_id;
		req.total_size_valid = 1;
		req.total_size = p_cal_data->total_size;
		req.seg_id_valid = 1;
		req.data_valid = 1;
		req.end_valid = 1;
		if (remaining > QMI_WLFW_MAX_DATA_SIZE_V01) {
			req.data_len = QMI_WLFW_MAX_DATA_SIZE_V01;
		} else {
			req.data_len = remaining;
			req.end = 1;
		}
		memcpy(req.data, temp, req.data_len);

		rc = qmi_client_send_msg_sync(gdata.clnt_handler,
					      QMI_WLFW_CAL_DOWNLOAD_REQ_V01,
					      &req,
					      sizeof(req),
					      &resp,
					      sizeof(resp),
					      WLFW_TIMEOUT_MS);
		if (rc != QMI_NO_ERR) {
			wsvc_printf_err("%s: rc %d, result %d, error %d",
					__func__,
					rc,
					resp.resp.result,
					resp.resp.error);

			return rc;
		}

		remaining -= req.data_len;
		temp += req.data_len;
		req.seg_id++;
	}

	wsvc_printf_dbg("%s: result %d, error %d", __func__,
				resp.resp.result,
				resp.resp.error);
	return rc;
}

static int wlfw_send_cal_update_req(wlfw_cal_temp_id_enum_v01 cal_id,
				    unsigned int total_size)
{
	int rc = QMI_NO_ERR;
	wlfw_cal_update_req_msg_v01 req;
	wlfw_cal_update_resp_msg_v01 resp;
	unsigned char *p_cal_data_temp, *p_cal_data = NULL;
	unsigned int remaining;

	wsvc_printf_dbg("%s", __func__);

	memset(&req, 0, sizeof(req));
	memset(&resp, 0, sizeof(resp));

	if (cal_id < 0 || cal_id >= QMI_WLFW_MAX_NUM_CAL_V01) {
		wsvc_printf_err("%s: Invalid Cal ID %u", __func__, cal_id);
		rc = -1;
		goto end;
	}

	p_cal_data = (unsigned char *)malloc(total_size);
	if (p_cal_data == NULL) {
		wsvc_printf_err("%s: Fail to alloc mem for cal file", __func__);
		rc = -1;
		goto end;
	}

	remaining = total_size;
	p_cal_data_temp = p_cal_data;
	while (remaining && resp.end == 0) {
		req.cal_id = cal_id;
		rc = qmi_client_send_msg_sync(gdata.clnt_handler,
					      QMI_WLFW_CAL_UPDATE_REQ_V01,
					      &req,
					      sizeof(req),
					      &resp,
					      sizeof(resp),
					      WLFW_TIMEOUT_MS);
		if (rc != QMI_NO_ERR) {
			wsvc_printf_err("%s: rc %d, result %d, error %d",
					__func__,
					rc,
					resp.resp.result,
					resp.resp.error);

			goto fail;
		}

		wsvc_printf_dbg("%s: result %d, error %d", __func__,
					resp.resp.result,
					resp.resp.error);

		if ((resp.file_id_valid == 1 && resp.file_id == cal_id)
		   && (resp.total_size_valid == 1 && resp.total_size == total_size)
		   && (resp.seg_id_valid == 1 && resp.seg_id == req.seg_id)
		   && (resp.data_valid == 1 && resp.data_len <= QMI_WLFW_MAX_DATA_SIZE_V01)) {
			memcpy(p_cal_data_temp, resp.data, resp.data_len);
		} else {
			wsvc_printf_err("%s: Unmatched cal data, "
					"Expect file_id %u, total_size %u, "
					"seg_id %u, Recv file_id_valid %u, "
					"file_id %u, total_size_valid %u, "
					"total_size %u, seg_id_valid %u, "
					"seg_id %u, data_len_valid %u, "
					"data_len %u",
					__func__,
					cal_id, total_size, req.seg_id,
					resp.file_id_valid,
					resp.file_id,
					resp.total_size_valid,
					resp.total_size,
					resp.seg_id_valid,
					resp.seg_id,
					resp.data_valid,
					resp.data_len);
			rc = -1;
			goto fail;
		}

		remaining -= resp.data_len;
		p_cal_data_temp += resp.data_len;
		req.seg_id++;
	}

	if (remaining == 0 && (resp.end_valid && resp.end)) {
		gdata.cal_data_array[cal_id].cal_id = cal_id;
		gdata.cal_data_array[cal_id].total_size = total_size;
		if (gdata.cal_data_array[cal_id].data)
			free(gdata.cal_data_array[cal_id].data);
		gdata.cal_data_array[cal_id].data = p_cal_data;
		wlfw_save_file(cal_id, p_cal_data, total_size);
	} else {
		wsvc_printf_err("%s: Cal file corrupted: remaining %u, "
				"end_valid %u, end %u",	__func__,
				remaining, resp.end_valid, resp.end);
		rc = -1;
		goto fail;
	}

fail:
	free(p_cal_data);
end:
	return rc;
}

static void wlfw_handle_initiate_cal_download_ind(void *ind_buf,
					 unsigned int ind_buf_len)
{
	qmi_client_error_type rc;
	wlfw_initiate_cal_download_ind_msg_v01 decoded_ind_buf;
	struct ind_data *i_data = NULL;

	wsvc_printf_dbg("%s", __func__);

	memset(&decoded_ind_buf, 0, sizeof(decoded_ind_buf));
	rc = qmi_client_message_decode(gdata.clnt_handler,
				       QMI_IDL_INDICATION,
				       QMI_WLFW_INITIATE_CAL_DOWNLOAD_IND_V01,
				       ind_buf,
				       ind_buf_len,
				       &decoded_ind_buf,
				       sizeof(decoded_ind_buf));
	if (rc != QMI_NO_ERR) {
		wsvc_printf_err("%s: fail to decode msg", __func__);
		return;
	}

	/* Get meta_data index, fill in ind_data, enqueue it to ind list
	 * and wake up the other thread */
	i_data = malloc(sizeof(*i_data));
	if (i_data == NULL) {
		wsvc_printf_err("%s: failed to alloc memory for i_data",
				__func__);
		return;
	}
	memset(i_data, 0, sizeof(*i_data));
	i_data->msg_id = QMI_WLFW_INITIATE_CAL_DOWNLOAD_IND_V01;
	i_data->data.init_cal_download_ind = decoded_ind_buf;
	i_data->next = NULL;

	ind_data_enqueue(i_data);

	pthread_mutex_lock(&(gdata.mutex));
	pthread_cond_signal(&(gdata.cond));
	pthread_mutex_unlock(&(gdata.mutex));

	return;
}

static void wlfw_handle_initiate_cal_update_ind(void *ind_buf,
				       unsigned int ind_buf_len)
{
	qmi_client_error_type rc;
	wlfw_initiate_cal_update_ind_msg_v01 decoded_ind_buf;
	struct ind_data *i_data = NULL;

	wsvc_printf_dbg("%s", __func__);

	memset(&decoded_ind_buf, 0, sizeof(decoded_ind_buf));
	rc = qmi_client_message_decode(gdata.clnt_handler,
				       QMI_IDL_INDICATION,
				       QMI_WLFW_INITIATE_CAL_UPDATE_IND_V01,
				       ind_buf,
				       ind_buf_len,
				       &decoded_ind_buf,
				       sizeof(decoded_ind_buf));
	if (rc != QMI_NO_ERR) {
		wsvc_printf_err("%s: fail to decode msg", __func__);
		return;
	}

	i_data = malloc(sizeof(*i_data));
	if (i_data == NULL) {
		wsvc_printf_err("%s: failed to alloc memory for i_data",
				__func__);
		return;
	}
	memset(i_data, 0, sizeof(*i_data));
	i_data->msg_id = QMI_WLFW_INITIATE_CAL_UPDATE_IND_V01;
	i_data->data.init_cal_update_ind = decoded_ind_buf;
	i_data->next = NULL;

	ind_data_enqueue(i_data);

	pthread_mutex_lock(&(gdata.mutex));
	pthread_cond_signal(&(gdata.cond));
	pthread_mutex_unlock(&(gdata.mutex));

	return;
}

static void wlfw_qmi_ind_cb(qmi_client_type user_handle,
			    unsigned int msg_id,
			    void *ind_buf,
			    unsigned int ind_buf_len,
			    void *ind_cb_data)
{
	if ((ind_buf_len > 0) && (NULL == user_handle || NULL == ind_buf)) {
		wsvc_printf_err("%s: Invalid parameter(s) user_handle %pK:"
			"ind_buf %pK: ind_buf_len %d", __func__, user_handle,
			ind_buf, ind_buf_len);
		return;
	}
	(void)ind_cb_data;

	wsvc_printf_dbg("%s: Receive Ind : 0x%x", __func__, msg_id);

	switch (msg_id) {
	case QMI_WLFW_FW_READY_IND_V01:
		/* turn off Adrastea */
		break;
	case QMI_WLFW_INITIATE_CAL_DOWNLOAD_IND_V01:
		wlfw_handle_initiate_cal_download_ind(ind_buf,
					     ind_buf_len);
		break;
	case QMI_WLFW_INITIATE_CAL_UPDATE_IND_V01:
		wlfw_handle_initiate_cal_update_ind(ind_buf,
					   ind_buf_len);
		break;
	case QMI_WLFW_MSA_READY_IND_V01:
		wsvc_printf_dbg("%s: Received MSA Ready Ind", __func__);
		pthread_mutex_lock(&gdata.mutex);
		gdata.state |= CNSS_MSA_READY;
		pthread_cond_signal(&gdata.cond_rsp);
		pthread_mutex_unlock(&gdata.mutex);
		break;
	default:
		wsvc_printf_err("%s: Unsupported Ind %u",
				__func__, msg_id);
		break;
	}
}

static void wlfw_qmi_err_cb(qmi_client_type user_handle,
			    qmi_client_error_type error,
			    void *err_cb_data)
{
	int rc;

	if (user_handle == NULL) {
		wsvc_printf_err("%s: Invalid user_handle", __func__);
		return;
	}
	(void)err_cb_data;

	wsvc_printf_info("%s: Server disconnect, err %d client %pK",
			 __func__, error, (void *)user_handle);

	gdata.state = 0;

	rc = wlfw_stop(SVC_DISCONNECTED);
	if (rc != 0) {
		wsvc_printf_err("%s: Fail to stop wlfw client", __func__);
		return;
	}
	rc = wlfw_start(SVC_RECONNECT);
	if (rc != 0) {
		wsvc_printf_err("%s: Fail to re-start wlfw client", __func__);
		return;
	}
}

void wlfw_handle_ind_data(struct ind_data *i_data)
{
	wsvc_printf_dbg("%s", __func__);

	if (i_data == NULL) {
		wsvc_printf_err("%s: Invalid data", __func__);
		return;
	}

	if ((CNSS_IS_WLFW_QMI_CONNECTED(gdata.state)) == 0) {
		wsvc_printf_err("%s: service disconnected, "
				"ignor this ind_data", __func__);
		return;
	}

	switch (i_data->msg_id) {
	case QMI_WLFW_INITIATE_CAL_DOWNLOAD_IND_V01:
		wsvc_printf_dbg("%s: msg_id 0x%x, cal_id %u",
				__func__, i_data->msg_id,
				i_data->data.init_cal_download_ind.cal_id);
		wlfw_send_cal_download_req(i_data->data.init_cal_download_ind.cal_id);
		break;
	case QMI_WLFW_INITIATE_CAL_UPDATE_IND_V01:
		wsvc_printf_dbg("%s: msg_id 0x%x, cal_id %u, total_size %d",
				__func__, i_data->msg_id,
				i_data->data.init_cal_update_ind.cal_id,
				i_data->data.init_cal_update_ind.total_size);
		wlfw_send_cal_update_req(i_data->data.init_cal_update_ind.cal_id,
					 i_data->data.init_cal_update_ind.total_size);
		break;
	default:
		wsvc_printf_dbg("%s: Invalid msg_id 0x%x",
				__func__, i_data->msg_id);
		break;
	}
}

void wlfw_build_cal_table(void)
{
	char filename[MAX_FILE_PATH];
	int len_read;
	unsigned char *pcal;
	int id;

	for (id = 0; id < QMI_WLFW_MAX_NUM_CAL_V01; id++) {
		snprintf(filename, sizeof(filename), CAL_FILE"%02d.bin", id);
		if ((len_read = wlfw_read_file(filename, &pcal)) <= 0) {
			wsvc_printf_err("%s: not read %s", __func__, filename);
			continue;
		}
		gdata.cal_data_array[id].cal_id = id;
		gdata.cal_data_array[id].total_size = len_read;
		gdata.cal_data_array[id].data = pcal;
	}
	return;
}

void *wlfw_service_request(void *arg)
{
	int rc;
	qmi_client_type clnt;
	qmi_cci_os_signal_type os_params;
	qmi_service_info info;
	struct ind_data *i_data = NULL;
	int16_t bdf_retry = 0;
	uint64_t fw_status = 0;
	(void)arg;

	wsvc_printf_dbg("%s: Start the pthread: %pK", __func__, arg);

	/* release any old handles for qmi client */
	if (gdata.clnt_handler) {
		qmi_client_release(gdata.clnt_handler);
		gdata.clnt_handler = NULL;
	}

	gdata.svc_obj = wlfw_get_service_object_v01();

	rc = qmi_client_init_instance(gdata.svc_obj,
					QMI_CLIENT_INSTANCE_ANY,
					wlfw_qmi_ind_cb,
					NULL,
					&os_params,
					0,
					&clnt);

	if (rc != QMI_NO_ERR) {
		wsvc_printf_err("Failed to init client");
		return NULL;
	}

	rc = qmi_client_register_error_cb(clnt,
					  wlfw_qmi_err_cb,
					  NULL);
	if (rc != QMI_NO_ERR) {
		wsvc_printf_err("Failed to register error cb");
		qmi_client_release(clnt);
		return NULL;
        }

	gdata.clnt_handler = clnt;
	gdata.state |= CNSS_WLFW_QMI_CONNECTED;

	wsvc_printf_info("WLFW service connected");

	wlfw_build_cal_table();

	rc = wlfw_send_ind_register_req(&fw_status);
	if (rc != QMI_NO_ERR) {
		qmi_client_release(clnt);
		gdata.state = 0;
		return NULL;
	}
	if (fw_status & QMI_WLFW_ALREADY_REGISTERED_V01) {
		if (!(fw_status & QMI_WLFW_FW_READY_V01)) {
			wsvc_printf_err("FW in bad state 0x%lx", fw_status);
			goto skip_mac_addr;
		}
		goto skip_cal_report;
	}
	if (fw_status & QMI_WLFW_MSA_READY_V01) {
		wsvc_printf_dbg("MSA is ready");
		gdata.state |= CNSS_MSA_READY;
	}

	pthread_mutex_lock(&gdata.mutex);
	if (!CNSS_IS_MSA_READY(gdata.state))
	{
		wsvc_printf_dbg("Wait for MSA ready Indication from FW");
		pthread_cond_wait(&gdata.cond_rsp, &gdata.mutex);
	}
	pthread_mutex_unlock(&gdata.mutex);

	rc = wlfw_send_cap_req();
	if (rc != QMI_NO_ERR) {
		wsvc_printf_err("Failed to send capability request");
		gdata.state = 0;
		return NULL;
	}

	/* Do BDF retry */
	do {
		rc = wlfw_send_bdf_download_req();
	} while (bdf_retry++ < WLFW_MAX_BDF_RETRY && (FALSE == rc));

	if (rc != QMI_NO_ERR) {
		wsvc_printf_err("Failed to Download the BDF File");
		gdata.state = 0;
		return NULL;
	}

	rc = wlfw_send_cal_report_req();
	if (rc != QMI_NO_ERR) {
		wsvc_printf_err("Failed to send cal_report_request");
		gdata.state = 0;
		return NULL;
	}

skip_cal_report:
	/* Intention here is to not intersect MAC address configuration and BDF
	 * download */
	wsvc_printf_dbg("pthread_cond_signal dms_cond");
	pthread_mutex_lock(&(gdata.dms_mutex));
	gdata.state |= CNSS_BDF_DOWNLOAD_DONE;
	pthread_cond_signal(&(gdata.dms_cond));
	pthread_mutex_unlock(&(gdata.dms_mutex));

skip_mac_addr:
	while (svc_running) {
		pthread_mutex_lock(&(gdata.mutex));
		pthread_cond_wait(&gdata.cond, &gdata.mutex);
		pthread_mutex_unlock(&(gdata.mutex));

		if (!svc_running)
			break;

		ind_data_dequeue(&i_data);
		while (i_data) {
			/* handle ind */
			wlfw_handle_ind_data(i_data);
			free(i_data);
			i_data = NULL;
			ind_data_dequeue(&i_data);
		}
	}

	wsvc_printf_info("%s: Pthread exiting", __func__);

	return NULL;
}

int dms_get_wlan_address(qmi_client_type clnt)
{
	int rc;
	dms_get_mac_address_req_msg_v01 req;
	dms_get_mac_address_resp_msg_v01 resp;

	wsvc_printf_dbg("Sending DMS get mac address");

	memset(&req, 0, sizeof(req));
	memset(&resp, 0, sizeof(resp));

	req.device = DMS_DEVICE_MAC_WLAN_V01;

	rc = qmi_client_send_msg_sync(clnt,
				      QMI_DMS_GET_MAC_ADDRESS_REQ_V01,
				      &req,
				      sizeof(req),
				      &resp,
				      sizeof(resp),
				      WLFW_TIMEOUT_MS);
	if ((rc != QMI_NO_ERR) ||
	    (resp.resp.result != QMI_RESULT_SUCCESS_V01)) {
		wsvc_printf_err("Send DMS get mac address failed: rc %d, result %d, error %d",
				rc,
				resp.resp.result,
				resp.resp.error);
		if (resp.resp.result)
			rc = -resp.resp.result;
		goto out;
	}

	wsvc_printf_dbg("Read MAC address: valid %u, len %u",
			resp.mac_address_valid,
			resp.mac_address_len);

	if (resp.mac_address_valid &&
	    resp.mac_address_len == QMI_WLFW_MAC_ADDR_SIZE_V01) {
		gdata.mac_addr.mac_addr_valid = 1;
		memcpy(gdata.mac_addr.mac_addr, resp.mac_address,
		       QMI_WLFW_MAC_ADDR_SIZE_V01);
		wsvc_printf_mac_addr(MSG_DEBUG, "Read MAC address",
				     gdata.mac_addr.mac_addr);
	} else {
		gdata.mac_addr.mac_addr_valid = 0;
		memset(gdata.mac_addr.mac_addr, 0,
		       sizeof(uint8_t) * QMI_WLFW_MAC_ADDR_SIZE_V01);
	}

out:
	return rc;
}

int check_modem_compatability(struct dev_info *mdm_detect_info)
{
	char args[MODEM_BASEBAND_PROPERTY_SIZE] = {0};
	int ret = 0;
	/* Get the hardware property */
	ret = property_get(MODEM_BASEBAND_PROPERTY, args, "");
	if (ret > MODEM_BASEBAND_PROPERTY_SIZE) {
		wsvc_printf_err("property [%s] has size [%d] that exceeds max [%d]",
				MODEM_BASEBAND_PROPERTY, ret,
				MODEM_BASEBAND_PROPERTY_SIZE);
		return 0;
	}
	wsvc_printf_err("ro.baseband : [%s]", args);

	/* This will check for the type of hardware, and if the
	   hardware type needs external modem, it will check if the
	   modem type is external*/
	if((!strncmp(MODEM_BASEBAND_VALUE_APQ, args, 3)) ||
	    (!strncmp(MODEM_BASEBAND_VALUE_SDA, args, 3)) ||
	    !strncmp(MODEM_BASEBAND_VALUE_QCS, args, 3)) {
		for (ret = 0; ret < mdm_detect_info->num_modems; ret++) {
			if (mdm_detect_info->mdm_list[ret].type ==
			    MDM_TYPE_EXTERNAL) {
				wsvc_printf_err("Hardware supports external modem");
				return 1;
			}
		}
		wsvc_printf_err("Hardware does not support external modem");
		return 0;
	}
	return 1;
}

void *dms_service_request(void *arg)
{
	int rc;
	qmi_idl_service_object_type svc_obj;
	qmi_client_type clnt;
	qmi_cci_os_signal_type os_params;
	qmi_service_info info;
	struct dev_info mdm_detect_info;
	(void)arg;

	wsvc_printf_dbg("%s: Start DMS pthread: %pK", __func__, arg);

	rc = get_system_info(&mdm_detect_info);
	if (rc > 0) {
		wsvc_printf_err("Failed to get system info, ret %d", rc);
		goto out;
	}

	if (mdm_detect_info.num_modems == 0) {
		wsvc_printf_err("No Modem support for this target"
		      " number of modems is %d", mdm_detect_info.num_modems);
		goto out;
	}

	if(!check_modem_compatability(&mdm_detect_info)) {
		wsvc_printf_err("Target does not have external modem");
		goto out;
	}

	svc_obj = dms_get_service_object_v01();
	rc = qmi_client_init_instance(svc_obj,
				      QMI_CLIENT_INSTANCE_ANY,
				      NULL,
				      NULL,
				      &os_params,
				      0,
				      &clnt);
	if (rc != QMI_NO_ERR) {
		wsvc_printf_err("Failed to init DMS client, ret %d", rc);
		return NULL;
	}

	rc = dms_get_wlan_address(clnt);
	if (rc != QMI_NO_ERR) {
		wsvc_printf_err("Failed to get WLAN MAC address");
	} else if (gdata.mac_addr.mac_addr_valid) {
		int retry = 0;
		pthread_mutex_lock(&(gdata.dms_mutex));
		if (!CNSS_IS_BDF_DOWNLOAD_DONE(gdata.state)) {
			wsvc_printf_dbg("Wait for CNSS_BDF_DOWNLOAD_DONE");
			pthread_cond_wait(&gdata.dms_cond, &gdata.dms_mutex);
		}
		pthread_mutex_unlock(&(gdata.dms_mutex));
		for (retry = 0; retry < 2 && svc_running; retry++)
		{
			rc = wlfw_send_mac_addr_req();
			if (rc == QMI_NO_ERR)
				break;
			usleep(500000);
		}
	}

	qmi_client_release(clnt);
out:
	return NULL;
}

int wlfw_start(enum wlfw_svc_flag flag)
{
	int rc = 0;

	wsvc_printf_dbg("%s: Starting", __func__);

	if (flag == SVC_START)
		pm_init();

	svc_running = 1;

	rc = pthread_mutex_init(&(gdata.mutex), NULL);
	if (0 != rc) {
		wsvc_printf_err("Failed to init mutex, ret %d", rc);
		goto out;
	}
	rc = pthread_cond_init(&(gdata.cond), NULL);
	if (0 != rc) {
		wsvc_printf_err("Failed to init cond, ret %d", rc);
		goto err_fail_cond;
	}
	rc = pthread_cond_init(&(gdata.cond_rsp), NULL);
	if (0 != rc) {
		wsvc_printf_err("Failed to init cond_rsp, ret %d", rc);
		goto err_fail_cond_rsp;
	}
	rc = pthread_cond_init(&(gdata.dms_cond), NULL);
	if (0 != rc) {
		wsvc_printf_err("Failed to init dms_cond, ret %d", rc);
		goto err_fail_dms_cond_rsp;
	}
	rc = pthread_mutex_init(&(gdata.dms_mutex), NULL);
	if (0 != rc) {
		wsvc_printf_err("Failed to init dms_mutex, ret %d", rc);
		goto err_fail_dms_mutex_create;
	}

	rc = pthread_create(&(gdata.thread_id_dms), NULL,
			    dms_service_request, NULL);
	if (rc != 0)
		wsvc_printf_err("Failed to create DMS thread, ret %d", rc);

	rc = pthread_create(&(gdata.thread_id), NULL,
			    wlfw_service_request, NULL);
	if (0 != rc) {
		wsvc_printf_err("Failed to create thread, ret %d", rc);
		goto err_fail_thread_create;
	}

	wsvc_printf_dbg("%s: Start done: %d", __func__, rc);

	return rc;

err_fail_thread_create:
	rc = pthread_mutex_destroy(&(gdata.dms_mutex));
        if (0 != rc)
		wsvc_printf_err("Failed to destroy dms_mutex, ret %d", rc);

err_fail_dms_mutex_create:
	rc = pthread_cond_destroy(&(gdata.dms_cond));
	if (0 != rc)
		wsvc_printf_err("Failed to destroy dms_cond, ret %d", rc);

err_fail_dms_cond_rsp:
	rc = pthread_cond_destroy(&(gdata.cond_rsp));
	if (0 != rc)
		wsvc_printf_err("Failed to destroy cond_rsp, ret %d", rc);

err_fail_cond_rsp:
	rc = pthread_cond_destroy(&(gdata.cond));
	if (0 != rc)
		wsvc_printf_err("Failed to destroy cond, ret %d", rc);

err_fail_cond:
	rc = pthread_mutex_destroy(&(gdata.mutex));
        if (0 != rc)
		wsvc_printf_err("Failed to destroy mutex, ret %d", rc);

out:
	svc_running = 0;
	return rc;
}

int wlfw_stop(enum wlfw_svc_flag flag)
{
	int rc = 0;

	wsvc_printf_dbg("%s: Stopping: %d", __func__, flag);

	svc_running = 0;
	pthread_mutex_lock(&(gdata.mutex));
	pthread_cond_signal(&(gdata.cond_rsp));
	pthread_cond_signal(&(gdata.cond));
	pthread_mutex_unlock(&(gdata.mutex));

	rc = pthread_join(gdata.thread_id, NULL);
	if (0 != rc)
		wsvc_printf_err("Failed to join mutex, ret %d", rc);

	pthread_mutex_lock(&(gdata.dms_mutex));
	pthread_cond_signal(&(gdata.dms_cond));
	pthread_mutex_unlock(&(gdata.dms_mutex));

	pthread_join(gdata.thread_id_dms, NULL);
	if (rc != 0)
		wsvc_printf_err("Failed to join dms mutex, ret %d", rc);

	if (gdata.clnt_handler && (SVC_EXIT == flag)) {
		qmi_client_release(gdata.clnt_handler);
		gdata.clnt_handler = NULL;
	}

	ind_data_free_queue();

	rc = pthread_cond_destroy(&(gdata.dms_cond));
	if (0 != rc)
		wsvc_printf_err("Failed to destroy dms_cond, ret %d", rc);

	rc = pthread_mutex_destroy(&(gdata.dms_mutex));
	if (0 != rc)
		wsvc_printf_err("Failed to destroy dms_mutex, ret %d", rc);

	rc = pthread_cond_destroy(&(gdata.cond_rsp));
	if (0 != rc)
		wsvc_printf_err("Failed to destroy cond_rsp, ret %d", rc);

	rc = pthread_cond_destroy(&(gdata.cond));
	if (0 != rc)
		wsvc_printf_err("Failed to destroy cond, ret %d", rc);

	rc = pthread_mutex_destroy(&(gdata.mutex));
	if (0 != rc)
		wsvc_printf_err("Failed to destroy mutex, ret %d", rc);

	/* free and clear cached CAL data */
	if (flag == SVC_EXIT) {
		free_cal_file();
		pm_deinit();
	}

	wsvc_printf_dbg("%s: Stop done: %d", __func__, rc);

	return rc;
}
