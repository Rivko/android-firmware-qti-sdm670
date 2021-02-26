/*
 * Copyright (c) 2015 - 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __PD_NOTIFIER_LIB__
#define __PD_NOTIFIER_LIB__
#include <comdef.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include "qmi_idl_lib.h"
#include "qmi_cci_target_ext.h"
#include "service_registry_notifier_v01.h"
#include "qmi_client.h"
#include "qmi_cci_target_ext.h"
#include "qmi_client_instance_defs.h"
#define MAX_CLIENT_NAME_LEN 32
typedef enum pd_event {
	EVENT_PD_DOWN = SERVREG_NOTIF_SERVICE_STATE_DOWN_V01,
	EVENT_PD_UP = SERVREG_NOTIF_SERVICE_STATE_UP_V01,
	EVENT_PD_UNKNOWN = SERVREG_NOTIF_SERVICE_STATE_UNINIT_V01,
} pd_event;

typedef enum pd_rcode {
	PD_NOTIFIER_SUCCESS = 0,
	PD_NOTIFIER_FAIL,
	PD_NOTIFIER_ERR_ARG_INVALID,
	PD_NOTIFIER_ERR_OVERFLOW,
	PD_NOTIFIER_ERR_QMI,
	PD_NOTIFIER_ERR_NO_SERVER,
} pd_rcode;

typedef qmi_servreg_notif_service_state_enum_type_v01 pd_state;
typedef void (*pd_notifier_cb) (void *, enum pd_event);

typedef struct {
	//Basic struct has beein initialized
	int is_valid;
	//Has qmi initialization been done on this handle
	int qmi_initialized;
	//This handle is actively listening for notifications on a QMI channel
	int is_listening;
	//Name of the service whose notifications we are listening for
	char serviceName[QMI_SERVREG_NOTIF_NAME_LENGTH_V01];
	//Instance ID for the service who we want notifications from. The
	//instance id is returned as part of the servreg_get_entry call in
	//the service locator.
	uint32_t instance_id;
	//Callback function which is invoked on notifications
	pd_notifier_cb cb;
	//Optional data that may be required by the user in his callback
	//function
	void *user_data;
	//Flag to determine if debug mode is enabled or not
	int debug;
	//Name of the client
	char clientName[MAX_CLIENT_NAME_LEN];
	//QMI handle
	qmi_client_type pd_notifier_client;
	//Holds the initial state of the PD.
	pd_state initial_state;
	//Thread id for the QMI error callback functions to use
	pthread_t tid;
}PD_Notifier_Handle;

//Returns 1 or 0 depending if the notifier library is enabled or not
int pd_notifier_enabled();
//Allocate handle used for future interactions with the pd-notifier
PD_Notifier_Handle *pd_notifier_alloc(char *serviceName,
		char *clientName,
		uint32_t instance,
		pd_notifier_cb cb,
		void *user_data);

//Start listening for notifications regarding the
//pd providing the service the handle was created
//with
pd_rcode pd_notifier_register(PD_Notifier_Handle *hndl, pd_state *state);

//Stop listening for any further notifications
pd_rcode pd_notifier_deregister(PD_Notifier_Handle *hndl);

//Free the handle allocated by sr_notifier_alloc
pd_rcode pd_notifier_free(PD_Notifier_Handle *hndl);
#endif
