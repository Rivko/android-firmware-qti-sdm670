/*
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <comdef.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include "qmi_idl_lib.h"
#include "qmi_cci_target_ext.h"
#include "service_registry_locator_v01.h"
#include "qmi_client.h"
#include "qmi_cci_target_ext.h"
#include "qmi_client_instance_defs.h"

typedef struct {
	int is_valid;
	char service_name[QMI_SERVREG_LOC_NAME_LENGTH_V01 + 1];
	qmi_servreg_loc_get_domain_list_resp_msg_v01 domain_resp;
} SR_DL_Handle;

enum SR_Result_Enum {
	SR_RESULT_SUCCESS = 0,
	//Generic failure
	SR_RESULT_FAIL,
	//Failure in QMI code
	SR_RESULT_ERR_QMI,
	//Invalid arguments passed to function
	SR_RESULT_ERR_ARG_INVALID,
	//Overflow,out of bounds errors,etc
	SR_RESULT_ERR_OVERFLOW,
};
//Return 1 or 0 depending on if the locater is enabled in the build or
//not
int servreg_is_enabled();

//Allocate handle which is used for future
//interactions with the pdmapper library.
SR_DL_Handle * servreg_alloc_DLHandle();

//Free the handle that was returned by servreg_alloc_DLHandle
void servreg_free_DLHandle(SR_DL_Handle *handle);

//Get the list of domains that provide the service. The service string itself
//is in the form of provider/service. SR_RESULT_SUCCESS would be returned on
//success otherwise SR_RESULT_FAILURE would be returned
enum SR_Result_Enum servreg_get_domainlist(char *service, SR_DL_Handle *handle);

//Get the number of domains that provide the service that was requested by the
//last get_domainlist call. On error -1 is returned.
int servreg_get_numentries(SR_DL_Handle *handle);

//For the n'th entry returned by the last query get the information
//necessary to register with the pd-notifier for notifications regarding
//that entry
//handle: Allocated handle that was passed to servreg_get_domainlist
//notifier_string: Pointer to a location where the notifier_string will
//be generated and stored. This buffer must be freed by the caller
//instance: Location where the instance id will be stored.
//entry_number: The entry number for which this data is required.
enum SR_Result_Enum servreg_get_data_for_notifier(SR_DL_Handle *handle,
		char **notifier_string,
		int *instance,
		uint32_t entry_number);

//Get the n'th entry returned by the last query.
//handle : Allocated handle that was passed to the servreg_get_domainlist
//function whose results are being passed here
//Buffer: Location where the pointer to the "name" string will be placed
//Instance: Location where the instance id should be stored. The instance id
//is required if you need to register for notifications regarding the pd
//state.
//service_data_valid: Some services providers may give additional information
//that the clients can use. This field indicates if they have this additional
//data or not
//service_data: If service_data_valid is true then this field would hold the
//addtional info the service wants to publish.
enum SR_Result_Enum servreg_get_entry(SR_DL_Handle *handle,
		char **buffer,
		int *instance,
		int *service_data_valid,
		int *service_data,
		uint32_t entry_number);
