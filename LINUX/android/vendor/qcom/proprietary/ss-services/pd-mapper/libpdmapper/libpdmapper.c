/*
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "libpdmapper.h"

#ifndef ANDROID_BUILD
#include <sys/syslog.h>
#else
#define LOG_TAG "pd-mapper-lib"
#include <cutils/log.h>
#include <cutils/properties.h>
#endif
#define UNUSED __attribute__ ((unused))
#define PD_LOCATER_DEBUG_PROP "persist.pd_locater_debug"
//This flag is used to detect if debug mode is enabled or not in locations
//where the handle is not available
static int debug_mode = 0;
#ifndef LIBPDMAPPER_SUPPORTED
int servreg_is_enabled()
{
	return 0;
}

SR_DL_Handle *servreg_alloc_DLHandle()
{
	char debug_mode_string[PROPERTY_VALUE_MAX] = {'\0'};
	property_get(PD_LOCATER_DEBUG_PROP, debug_mode_string, "false");
	if (!strncmp(debug_mode_string, "true", PROPERTY_VALUE_MAX)) {
		debug_mode = 1;
		ALOGE("libpdmapper is not supported on this target");
	}
	return NULL;
}

void servreg_free_DLHandle(SR_DL_Handle *handle UNUSED)
{
	if (debug_mode)
		ALOGE("libpdmapper is not supported on this target");
}

enum SR_Result_Enum servreg_get_domainlist(char *service UNUSED,
		SR_DL_Handle *handle UNUSED)
{
	if (debug_mode)
		ALOGE("libpdmapper is not supported on this target");
	return SR_RESULT_FAIL;
}

int servreg_get_numentries(SR_DL_Handle *handle UNUSED)
{
	if (debug_mode)
		ALOGE("libpdmapper is not supported on this target");
	return -1;
}

enum SR_Result_Enum servreg_get_data_for_notifier(SR_DL_Handle *handle,
		char **notifier_string,
		int *instance,
		uint32_t entry_number)
{
	if (debug_mode)
		ALOGE("libpdmapper is not supported on this target");
	return SR_RESULT_FAIL;
}

enum SR_Result_Enum servreg_get_entry(
		SR_DL_Handle *handle UNUSED,
		char **buffer UNUSED,
		int *instance UNUSED,
		int *service_data_valid UNUSED,
		int *service_data UNUSED,
		uint32_t entry_number UNUSED) {
	if (debug_mode)
		ALOGE("libpdmapper is not supported on this target");
	return SR_RESULT_FAIL;
}
#else
int servreg_is_enabled()
{
	return 1;
}

void servloc_clnt_cb(
		qmi_client_type user_handle UNUSED,
		unsigned int msg_id,
		void *ind_buf UNUSED,
		unsigned int ind_buf_len,
		void *data UNUSED)
{
	if (debug_mode)
		ALOGD("Client recieved msg: id: 0x%x of len %d",
				(unsigned int)msg_id,
				ind_buf_len);
}

SR_DL_Handle* servreg_alloc_DLHandle()
{
	SR_DL_Handle *handle = NULL;
	char debug_mode_string[PROPERTY_VALUE_MAX] = {'\0'};

	property_get(PD_LOCATER_DEBUG_PROP, debug_mode_string, "false");
	handle = (SR_DL_Handle*)malloc(sizeof(SR_DL_Handle));
	if (!handle)
		return NULL;
	memset(handle, '\0',sizeof(SR_DL_Handle));
	if (!strncmp(debug_mode_string, "true", PROPERTY_VALUE_MAX)) {
		debug_mode = 1;
	}
	return handle;
}

void servreg_free_DLHandle(SR_DL_Handle *handle)
{
	if(handle)
		free(handle);
}

enum SR_Result_Enum servreg_get_domainlist(char *service, SR_DL_Handle *handle)
{
	qmi_idl_service_object_type serviceObject;
	qmi_cci_os_signal_type osParams;
	qmi_client_type clnt;
	qmi_service_info info;
	int rc;
	uint32_t num_services = 0;
	uint32_t num_entries = 1;
	int qmi_client_initialized = 0;
	enum SR_Result_Enum rcode;
	qmi_servreg_loc_get_domain_list_req_msg_v01 req;

	bzero(&req, sizeof(qmi_servreg_loc_get_domain_list_req_msg_v01));
	if(!service || !handle) {
		rcode = SR_RESULT_ERR_ARG_INVALID;
		goto error;
	}
	//In case same handle is being used for a differrent request the data
	//currently in the handle is effectively stale, so mark is_valid as 0.
	handle->is_valid = 0;
	memset(handle->service_name, '\0', sizeof(handle->service_name));
	strlcpy(handle->service_name, service,
			sizeof(handle->service_name) - 1);
	serviceObject = servreg_loc_get_service_object_v01();
	if (!serviceObject) {
		if (debug_mode)
			ALOGE("Failed to get service obj for servloc service");
		rcode = SR_RESULT_ERR_QMI;
		goto error;
	}
	rc = qmi_client_notifier_init(serviceObject, &osParams, &clnt);
	if (rc != QMI_NO_ERR) {
		if (debug_mode)
			ALOGE("client_notifier_init returned error : %d", rc);
		rcode = SR_RESULT_ERR_QMI;
		goto error;
	}
	//Wait for the service to come up
	while(1) {
		rc = qmi_client_get_service_list(serviceObject, NULL, NULL,
				&num_services);
		if (rc == QMI_NO_ERR)
			break;
		QMI_CCI_OS_SIGNAL_WAIT(&osParams, 0);
	};
	rc = qmi_client_get_service_list(serviceObject,
			&info,
			&num_entries,
			&num_services);
	if (rc != QMI_NO_ERR) {
		if (debug_mode)
			ALOGE("Failed to get qmi service list for servloc");
		rcode = SR_RESULT_ERR_QMI;
		goto error;
	}
	QMI_CCI_OS_SIGNAL_WAIT(&osParams, 0);
	num_entries = num_services;
	rc = qmi_client_get_service_list(serviceObject,
			&info,
			&num_entries,
			&num_services);
	if (rc != QMI_NO_ERR) {
		if (debug_mode)
			ALOGE("Failed to get qmi service list and update info");
		rcode = SR_RESULT_ERR_QMI;
		goto error;
	}
	rc = qmi_client_init(&info,
			serviceObject,
			servloc_clnt_cb,
			NULL,
			&osParams,
			&clnt);
	if (rc != QMI_NO_ERR) {
		if (debug_mode)
			ALOGE("Failed to do qmi_client_init : %d", rc);
		rcode = SR_RESULT_ERR_QMI;
		goto error;
	}
	qmi_client_initialized = 1;
	strlcpy(req.service_name, service, sizeof(req.service_name));
	memset(&(handle->domain_resp), '\0', sizeof(handle->domain_resp));
	rc = qmi_client_send_msg_sync(clnt,
			QMI_SERVREG_LOC_GET_DOMAIN_LIST_REQ_V01,
			&req,
			sizeof(req),
			&(handle->domain_resp),
			sizeof(handle->domain_resp),
			5000);
	if (rc != QMI_NO_ERR) {
		if (debug_mode)
			ALOGE("Failed to send get_domain_list_request: rc: %d",
					rc);
		rcode = SR_RESULT_ERR_QMI;
		goto error;
	}
	if (handle->domain_resp.total_domains >
			sizeof(handle->domain_resp.domain_list)) {
		//This version of the library does not support handling
		//of overflow requests. Print a warning for now
		if (debug_mode)
			ALOGW("Total domains > size of response packet");
	}
	qmi_client_release(clnt);
	handle->is_valid = 1;
	return SR_RESULT_SUCCESS;
error:
	if (handle)
		handle->is_valid = 0;
	if (qmi_client_initialized)
		qmi_client_release(clnt);
	return rcode;
}

int servreg_get_numentries(SR_DL_Handle *handle)
{
	if (!handle) {
		goto error;
	}
	if (!handle->is_valid) {
		if (debug_mode)
			ALOGE("Uninitialized handle passed to %s", __func__);
		goto error;
	}
	return handle->domain_resp.domain_list_len;
error:
	return -1;
}

enum SR_Result_Enum servreg_get_data_for_notifier(SR_DL_Handle *handle,
		char **notifier_string,
		int *instance,
		uint32_t entry_number)
{
	enum SR_Result_Enum rc;
	if (!handle || !notifier_string  || !instance) {
		rc = SR_RESULT_ERR_ARG_INVALID;
		goto error;
	}
	if (!handle->is_valid) {
		rc = SR_RESULT_ERR_ARG_INVALID;
		goto error;
	}
	if (entry_number > handle->domain_resp.domain_list_len - 1) {
		if (debug_mode)
			ALOGE("Requested index is out of bounds");
		rc = SR_RESULT_ERR_OVERFLOW;
		goto error;
	}
	*notifier_string = malloc(
			( QMI_SERVREG_LOC_NAME_LENGTH_V01 + 1) * 2 *sizeof(char)
			);
	if (!*notifier_string) {
		if (debug_mode)
			ALOGE("Failed to allocate buffer for notifier string");
		rc = SR_RESULT_FAIL;
		goto error;
	}
	memset(*notifier_string, '\0',
			(QMI_SERVREG_LOC_NAME_LENGTH_V01+1)*2*sizeof(char));
	snprintf(*notifier_string,
			(QMI_SERVREG_LOC_NAME_LENGTH_V01+1)*2*sizeof(char),
			"%s/%s",
			handle->domain_resp.domain_list[entry_number].name,
			handle->service_name);
	*instance = handle->domain_resp.domain_list[entry_number].instance_id;
	return SR_RESULT_SUCCESS;
error:
	return rc;

}

enum SR_Result_Enum servreg_get_entry(SR_DL_Handle *handle,
		char **buffer,
		int *instance,
		int *service_data_valid,
		int *service_data,
		uint32_t entry_number)
{
	enum SR_Result_Enum rc;

	if (!handle || !buffer || !instance || !service_data_valid ||
			!service_data) {
		rc = SR_RESULT_ERR_ARG_INVALID;
		goto error;
	}
	if (!handle->is_valid) {
		rc = SR_RESULT_ERR_ARG_INVALID;
		goto error;
	}
	if (entry_number > handle->domain_resp.domain_list_len - 1) {
		if (debug_mode)
			ALOGE("Requested index is out of bounds");
		rc = SR_RESULT_ERR_OVERFLOW;
		goto error;
	}
	*buffer = handle->domain_resp.domain_list[entry_number].name;
	*instance = handle->domain_resp.domain_list[entry_number].instance_id;
	*service_data_valid = handle->domain_resp.domain_list[entry_number].\
			      service_data_valid;
	*service_data = handle->domain_resp.domain_list[entry_number].\
			      service_data;

	return SR_RESULT_SUCCESS;
error:
	return rc;
}
#endif
