/*
 * Copyright (c) 2015 - 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "libpdnotifier.h"
#ifndef ANDROID_BUILD
#include <sys/syslog.h>
#else
#define LOG_TAG "pd-notifier"
#include <cutils/log.h>
#include <cutils/properties.h>
#endif

#define MAX_ROOT_PD 20
//ms for which we wait before querying for the service list
//again.
#define NOTIFIER_WAIT_TIMEOUT_MS 5000
//Number of times we retry querying for the service list before
//giving up.
#define NOTIFIER_RETRY_COUNT 2
#define PD_NOTIFIER_DEBUG_PROP "persist.pd_locater_debug"
#define UNUSED(name) name __attribute__((unused))

static pd_rcode pd_notifier_int_toggle_registration(
		PD_Notifier_Handle *handle,
		uint8_t enable);
static int debug_mode = 0;
int pd_notifier_enabled()
{
#ifdef LIBPDNOTIFIER_ENABLED
	return 1;
#else
	return 0;
#endif
}
PD_Notifier_Handle *pd_notifier_alloc(char *serviceName,
		char *clientName,
		uint32_t instance,
		pd_notifier_cb callback,
		void *user_data)
{
	PD_Notifier_Handle *handle = NULL;
	char debug_mode_string[PROPERTY_VALUE_MAX] = {'\0'};
	property_get(PD_NOTIFIER_DEBUG_PROP, debug_mode_string, "false");

	if (!strncmp(debug_mode_string, "true", PROPERTY_VALUE_MAX))
		debug_mode = 1;
	if (!pd_notifier_enabled()) {
		if (debug_mode)
			ALOGE("libpdnotifier unsupported");
		return NULL;
	}
	if (!serviceName || !clientName) {
		if (debug_mode)
			ALOGE("Invalid service/client name passed to alloc fn");
		goto error;
	}
	handle = (PD_Notifier_Handle *)malloc(sizeof(PD_Notifier_Handle));
	if (!handle) {
		if (debug_mode)
			ALOGE("Failed to allocate handle in pd_notifier_alloc");
		goto error;
	}
	if (!callback) {
		if (debug_mode)
			ALOGE("Invalid callback function passed to alloc fn");
		goto error;
	}
	memset(handle->serviceName, '\0', sizeof(handle->serviceName));
	strlcpy(handle->serviceName, serviceName,
			sizeof(handle->serviceName));
	memset(handle->clientName, '\0', sizeof(handle->clientName));
	strlcpy(handle->clientName, clientName,
			sizeof(handle->clientName));
	handle->instance_id = instance;
	handle->cb = callback;
	handle->is_valid = 1;
	handle->is_listening = 0;
	handle->user_data = user_data;
        handle->qmi_initialized = 0;
	if (!strncmp(debug_mode_string, "true", PROPERTY_VALUE_MAX))
		handle->debug = 1;
	else
		handle->debug = 0;
	return handle;
error:
	if (handle)
		free(handle);
	return NULL;
}

pd_rcode pd_notifier_free(PD_Notifier_Handle *handle)
{
	pd_rcode rc;

	if (!handle) {
		rc = PD_NOTIFIER_ERR_ARG_INVALID;
		goto error;
	}
	if (!handle->is_valid) {
		rc = PD_NOTIFIER_ERR_ARG_INVALID;
		goto error;
	}
	if (handle->is_listening) {
		//If this flag is set then it means that the client
		//had not called pd_notifier_deregister before calling
		//free. We should call it ourselves in that case to avoid
		//leaks
		if (handle->debug)
			ALOGE("Handle is still registered with QMI server");
		pd_notifier_int_toggle_registration(handle, 0);
	}
	free(handle);
	return PD_NOTIFIER_SUCCESS;
error:
	return rc;
}

static void null_cb(UNUSED(qmi_client_type user_handle),
		UNUSED(unsigned int msg_id),
		UNUSED(void *resp_c_struct),
		UNUSED(unsigned int resp_c_struct_len),
		UNUSED(void *resp_cb_data),
		UNUSED(qmi_client_error_type transp_err))
{
	return;
}
//This function recieves the indication notifications from the
//servreg notifier server. It will also recieve the handle which
//the client used to register with the libpdmapper as a argument.
void pd_notifier_clnt_ind_cb(qmi_client_type user_handle,
		unsigned int msg_id,
		void *ind_buf,
		unsigned int ind_buf_len,
		void *hndl)
{
	PD_Notifier_Handle *handle = hndl;
	qmi_servreg_notif_state_updated_ind_msg_v01 msg;
	int retVal;
	qmi_servreg_notif_set_ack_req_msg_v01 ack = {0};
	qmi_servreg_notif_set_ack_resp_msg_v01 resp = {0};
	qmi_idl_service_object_type serviceObject;
        qmi_txn_handle txn_hndl;

	if (!handle) {
		goto error;
	}
	if (!handle->is_valid) {
		goto error;
	}
	if (!ind_buf) {
		goto error;
	}
	retVal = qmi_client_message_decode(user_handle,
			QMI_IDL_INDICATION,
			msg_id,
			ind_buf,
			ind_buf_len,
			&msg,
			sizeof(msg));
	if (retVal != QMI_NO_ERR) {
		if (handle->debug)
			ALOGE("Failed to decode state change message for(%s)",
					handle->clientName);
		return;
	}
	if (!handle->qmi_initialized || !handle->is_listening) {
		//Something went wrong here. We appear to have
		//deregistered for notifications but we still
		//got one.
		if (handle->debug)
			ALOGE("Unexpected notification for client %s for pd %s",
					handle->clientName,
					handle->serviceName);
		goto error;
	}
	if (handle->debug) {
		ALOGI("Calling cb func for %s for service %s state: %lld id:%x",
				handle->clientName,
				handle->serviceName,
				msg.curr_state,
				msg.transaction_id);
		if (msg.curr_state == EVENT_PD_UP)
			ALOGI("%s recieved UP state transition for %s",
					handle->clientName,
					handle->serviceName);
		else if (msg.curr_state == EVENT_PD_DOWN)
			ALOGI("%s received DOWN state transition for %s",
					handle->clientName,
					handle->serviceName);
		else if (msg.curr_state == EVENT_PD_UNKNOWN)
			ALOGI("%s received UNINITIALIZED state ind for %s",
					handle->clientName,
					handle->serviceName);
		else
			ALOGE("%s : unrecognized state for %s: %lld",
					handle->clientName,
					handle->serviceName,
					msg.curr_state);
	}
	//Call the client supplied callback function
	handle->cb(handle->user_data, msg.curr_state);
	if (handle->debug)
		ALOGI("%s returned from callback function",
				handle->clientName);
	//Send the state change ack to the server
	//Initialize qmi connection
	serviceObject = servreg_notif_get_service_object_v01();
	if (!serviceObject) {
		if (handle->debug)
			ALOGE("cb ack failed: Failed to get obj for notifier");
		goto error;
	}
	//Setup response packet
	ack.transaction_id = msg.transaction_id;
	strlcpy(ack.service_name,
			msg.service_name,
			sizeof(ack.service_name));
	//Send response packet
	retVal = qmi_client_send_msg_async(user_handle,
			QMI_SERVREG_NOTIF_STATE_UPDATED_IND_ACK_REQ_V01,
			&ack,
			sizeof(ack),
			&resp,
			sizeof(resp),
			null_cb,
                        NULL,
                        &txn_hndl);
	if (retVal != QMI_NO_ERR) {
		if (handle->debug)
			ALOGE("cb ack failed: qmi send msg returned err: %d",
					retVal);
		goto error;
	}
	if (handle->debug)
		ALOGI("Client %s successfully handled state change ind for %s.",
				handle->clientName,
				handle->serviceName);
error:
	return;
}

void *root_pd_crash_cleanup(void *hndl)
{
	PD_Notifier_Handle *handle = hndl;
	pd_rcode rc;
	if (!hndl) {
		if (debug_mode)
			ALOGE("%s: Null handle passed", __func__);
		return 0;
	}
	if (!handle->is_valid) {
		if (debug_mode)
			ALOGE("%s: Invalid handle passed", __func__);
		return 0;
	}
	if (handle->is_listening){
		//Call the client supplied callback function
		handle->cb(handle->user_data, EVENT_PD_DOWN);
	}
	//Release the handle
	if(handle->qmi_initialized) {
		qmi_client_release(handle->pd_notifier_client);
		handle->qmi_initialized = 0;
	}
	rc = pd_notifier_int_toggle_registration(handle, 1);
	if (handle->debug && rc != PD_NOTIFIER_SUCCESS)
			ALOGE("%s: Failed to register with root pd. Retrying",
					__func__);
	while (rc != PD_NOTIFIER_SUCCESS) {
		sleep(1);
		rc = pd_notifier_int_toggle_registration(handle, 1);
		if (handle->debug && rc != PD_NOTIFIER_SUCCESS)
			ALOGE("%s: Failed to register with root pd. Retrying",
					__func__);
	}
	if (handle->debug)
		ALOGI("%s : Successfully re-registered with root pd", __func__);
	if (handle->is_listening &&
			handle->initial_state ==
			SERVREG_NOTIF_SERVICE_STATE_UP_V01){
		//Send PD_UP notification to client
		handle->cb(handle->user_data, EVENT_PD_UP);
	}
	return 0;
}

void root_pd_crash_handler(
		UNUSED (qmi_client_type clnt),
		UNUSED (qmi_client_error_type error),
		void *hndl)
{
	PD_Notifier_Handle *handle = hndl;
	int rc;
	pthread_attr_t attr;
	if (!hndl) {
		if (debug_mode)
			ALOGE("%s: Null handle passed", __func__);
		return;
	}
	if (!handle->is_valid) {
		if (debug_mode)
			ALOGE("%s: Invalid handle passed", __func__);
	}
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	rc = pthread_create(&(handle->tid),
			&attr,
			root_pd_crash_cleanup,
			hndl);
	if (rc < 0)
		ALOGE("%s: Failed to create thread to handle pd-crash",
				__func__);

}

static pd_rcode pd_notifier_query_state(
		PD_Notifier_Handle *handle) {
	int retVal = 0;
	qmi_servreg_notif_query_state_req_msg_v01 req = {0};
	qmi_servreg_notif_query_state_resp_msg_v01 resp = {0};

	strlcpy(req.service_name,
			handle->serviceName,
			sizeof(req.service_name));
	retVal = qmi_client_send_msg_sync(handle->pd_notifier_client,
			QMI_SERVREG_NOTIF_QUERY_STATE_REQ_V01,
			&req,
			sizeof(req),
			&resp,
			sizeof(resp),
			5000);
	if (retVal != QMI_NO_ERR) {
		ALOGE("Send reg msg failed for %s :instance:%d rval:%d",
			handle->serviceName,
			handle->instance_id,
			retVal);

		return PD_NOTIFIER_ERR_QMI;
	}

	ALOGE("PD %s query state valid=%d state=0x%08x",
		req.service_name, resp.curr_state_valid, resp.curr_state);

	return PD_NOTIFIER_SUCCESS;
}

static pd_rcode pd_notifier_int_toggle_registration(
		PD_Notifier_Handle *handle,
		uint8_t enable) {
	qmi_idl_service_object_type serviceObject;
	qmi_cci_os_signal_type osParams;
	int retVal = 0;
	pd_rcode rc;
	qmi_servreg_notif_register_listener_req_msg_v01 req = {0};
	qmi_servreg_notif_register_listener_resp_msg_v01 resp = {0};

	//Sanity checks
	if (!handle) {
		return PD_NOTIFIER_ERR_ARG_INVALID;
	}
	if (!handle->is_valid) {
		rc = PD_NOTIFIER_ERR_ARG_INVALID;
		goto error;
	}
	if (!handle->qmi_initialized) {
		//Standard qmi setup
		serviceObject = servreg_notif_get_service_object_v01();
		if (!serviceObject) {
			if (handle->debug)
				ALOGE("Failed to get notifier service object");
			rc = PD_NOTIFIER_ERR_QMI;
			goto error;
		}
		memset(&osParams, '\0', sizeof(osParams));
		retVal = qmi_client_init_instance(
				serviceObject,
				handle->instance_id,
				pd_notifier_clnt_ind_cb,
				handle,
				&osParams,
				5000,
				&(handle->pd_notifier_client));
		if (retVal != QMI_NO_ERR) {
			if (handle->debug)
				ALOGE("qmi_client_init_instance error : %d",
					retVal);
			rc = PD_NOTIFIER_ERR_QMI;
			goto error;
		}
		handle->qmi_initialized = 1;
		qmi_client_register_error_cb(handle->pd_notifier_client,
				root_pd_crash_handler,
				handle);
	}
	//Copy the service name into the request packet
	strlcpy(req.service_name,
			handle->serviceName,
			sizeof(req.service_name));
	req.enable = enable;
	retVal = qmi_client_send_msg_sync(handle->pd_notifier_client,
			QMI_SERVREG_NOTIF_REGISTER_LISTENER_REQ_V01,
			&req,
			sizeof(req),
			&resp,
			sizeof(resp),
			5000);
	if (retVal != QMI_NO_ERR) {
		if (handle->debug)
			ALOGE("Send reg msg failed for %s :instance:%d rval:%d",
				handle->serviceName,
				handle->instance_id,
				retVal);
		rc = PD_NOTIFIER_ERR_QMI;
		goto error;
	}
	handle->is_listening = 1;
	if (resp.curr_state_valid)
		handle->initial_state = resp.curr_state;
	else {
		//If we do not know what the state is just set it to
		//uninitialized
		handle->initial_state = SERVREG_NOTIF_SERVICE_STATE_UNINIT_V01;
	}
	if (handle->debug) {
		ALOGE("registration response:  enable=%d, valid state=%d (state=0x%08x)",
			enable, resp.curr_state_valid, resp.curr_state);
		pd_notifier_query_state(handle);
	}
	if (!enable) {
		qmi_client_release(handle->pd_notifier_client);
		handle->qmi_initialized = 0;
		handle->is_listening = 0;
	}
	return PD_NOTIFIER_SUCCESS;
error:
	if (handle->debug)
		ALOGE("registration failed");

	if (handle->qmi_initialized) {
		qmi_client_release(handle->pd_notifier_client);
		handle->qmi_initialized = 0;
		handle->is_listening = 0;
	}
	return rc;
}

pd_rcode pd_notifier_register(PD_Notifier_Handle *handle, pd_state *state)
{
	pd_rcode rc;

	//Sanity checks
	if (!handle) {
		rc = PD_NOTIFIER_ERR_ARG_INVALID;
		goto error;
	}
	if (!handle->is_valid) {
		rc = PD_NOTIFIER_ERR_ARG_INVALID;
		goto error;
	}
	rc = pd_notifier_int_toggle_registration(handle, 1);
	if (rc == PD_NOTIFIER_SUCCESS)
		*state = handle->initial_state;
error:
	return rc;
}

pd_rcode pd_notifier_deregister(PD_Notifier_Handle *handle)
{
	pd_rcode rc;

	//Sanity checks
	if (!handle) {
		rc = PD_NOTIFIER_ERR_ARG_INVALID;
		goto error;
	}
	if (!handle->is_valid) {
		rc = PD_NOTIFIER_ERR_ARG_INVALID;
		goto error;
	}
	if (!handle->is_listening) {
		//We are already deregistered
		return PD_NOTIFIER_SUCCESS;
	}
	return pd_notifier_int_toggle_registration(handle, 0);
error:
	return rc;
}
