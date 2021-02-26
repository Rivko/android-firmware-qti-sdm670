/*
 * Copyright (c) 2015 - 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "pd-mapper.h"

//sr_version
#define SR_VERSION_FIELD "sr_version"
#define SR_VERSION_FIELD_MAJOR "major"
#define SR_VERSION_FIELD_MINOR "minor"
#define SR_VERSION_FIELD_PATCH "patch"

//sr_domain
#define SR_DOMAIN_FIELD "sr_domain"
#define SR_DOMAIN_FIELD_SOC "soc"
#define SR_DOMAIN_FIELD_DOMAIN "domain"
#define SR_DOMAIN_FIELD_INSTANCE "qmi_instance_id"
#define SR_DOMAIN_FIELD_SUBDOMAIN "subdomain"

//sr_service
#define SR_SERVICE_FIELD "sr_service"
#define SR_SERVICE_FIELD_PROVIDER "provider"
#define SR_SERVICE_FIELD_SERVICE "service"
#define SR_SERVICE_FIELD_SERVICE_DATA_VALID "service_data_valid"
#define SR_SERVICE_FIELD_SERVICE_DATA "service_data"

#define MAX_JSON_NEST_LEVEL 5
#define JSON_NEST_LEVEL_ROOT 0     //Root node
#define JSON_NEST_LEVEL_FIRST 1    //First level: sr_domain/sr_service
#define JSON_NEST_LEVEL_SECOND 2   //Second level:soc/domain
#define JSON_NEST_LEVEL_THIRD 3    //Third level: provider/service
#ifndef ANDROID_BUILD
#define ALOGI(...) syslog(LOG_DEBUG, __VA_ARGS__);
#define ALOGE(...) syslog (LOG_DEBUG, __VA_ARGS__);
#endif
#define MAX_SOC_NAME_LEN 20
#define MAX_DOMAIN_NAME_LEN 20
#define MAX_SUBDOMAIN_NAME_LEN 20
#define MAX_PROVIDER_NAME_LEN 20
#define MAX_SERVICE_NAME_LEN 20
#define MAX_LOOKUP_STRING_LEN (MAX_PROVIDER_NAME_LEN + MAX_SERVICE_NAME_LEN)
#define RET_SUCCESS 0
#define RET_FAILED 1
#define JSON_FILE_EXT ".jsn"
#define PD_LOCATER_DEBUG_PROP "persist.pd_locater_debug"
#define UNUSED __attribute__ ((unused))
const char *json_dir_list[] = {
	"/firmware/image",
	"/vendor/firmware",
	"/vendor/firmware/image",
	NULL };

#ifdef __USERDEBUG__
// For userdebug variants, enable debug logs by default
static int debug_mode_enabled = 1;
#else
static int debug_mode_enabled = 0;
#endif

using namespace std;

struct serviceInfo {
	string *soc;
	string *domain;
	string *subdomain;
	string *provider;
	uint32_t instance;
	uint32_t service_data_valid;
	uint32_t service_data;
};
int update_map_for_file(char *file);
map<string,list<struct serviceInfo*> > service_map;

qmi_csi_cb_error pdMapperServerConnect(qmi_client_handle qmiClient,
		void *data UNUSED,
		void **qmiConnection)
{
	if (!qmiConnection) {
		ALOGE("qmiConnection was passed in as NULL");
		return QMI_CSI_CB_INTERNAL_ERR;
	}
	*qmiConnection = qmiClient;
	return QMI_CSI_CB_NO_ERR;
}

void pdMapperServerDisconnect(void *qmiClient, void *data UNUSED)
{
	if (!qmiClient) {
		ALOGE("qmiClient was passed in as NULL");
	}
}

qmi_csi_cb_error pdMapperServerProcessRequest(void *__qmiClient,
		qmi_req_handle reqHandle,
		unsigned int msgId,
		void *reqStruct,
		unsigned int reqStructLen UNUSED,
		void *data UNUSED)
{
	qmi_csi_cb_error cbErr = QMI_CSI_CB_NO_ERR;
	qmi_client_handle qmiClient;
	//Request structure for the get domain list request
	qmi_servreg_loc_get_domain_list_req_msg_v01 *req;
	//Request structure for the print error string request
	qmi_servreg_loc_pfr_req_msg_v01 *pfr_req;
	//Response strutcure for the print error string request
	qmi_servreg_loc_pfr_resp_msg_v01 pfr_resp;
	//Response structure for the get domain list request
	qmi_servreg_loc_get_domain_list_resp_msg_v01 response;
	qmi_servreg_loc_indication_register_resp_msg_v01 ind_resp;
	map<string,list<struct serviceInfo*> >::iterator service_map_iterator;
	list<struct serviceInfo*>::iterator provider_list_iterator;
	string lookup_string;
	int rcode;
	uint32_t result_list_size = 0;

	if(!__qmiClient) {
		ALOGE("Invalid qmiClient handle passed to processRequest func");
		return QMI_CSI_CB_INTERNAL_ERR;
	}
	if(!reqStruct) {
		ALOGE("Invalid reqStruct passed to processRequest func");
		return QMI_CSI_CB_INTERNAL_ERR;
	}
	bzero(&response, sizeof(qmi_servreg_loc_get_domain_list_resp_msg_v01));
	qmiClient = (qmi_client_handle) __qmiClient;

	switch(msgId) {
	case QMI_SERVREG_LOC_INDICATION_REGISTER_REQ_V01:
		if (debug_mode_enabled) {
			//This request is unsupported by APSS.
			//The response package will tell the calling
			//entity so.
			ALOGI("Recieved loc indication register req");
		}
		ind_resp.resp.result = QMI_RESULT_FAILURE_V01;
		ind_resp.resp.error = QMI_ERR_NOT_SUPPORTED_V01;
		rcode = qmi_csi_send_resp(reqHandle,
				QMI_SERVREG_LOC_INDICATION_REGISTER_RESP_V01,
				&ind_resp,
				sizeof(ind_resp));
		if (rcode != QMI_CSI_NO_ERR) {
			ALOGE("Failed to send ind register resp:rcode: %d",
					rcode);
			cbErr = QMI_CSI_CB_INTERNAL_ERR;
		}
		return cbErr;
	case QMI_SERVREG_LOC_GET_DOMAIN_LIST_REQ_V01:
		if (debug_mode_enabled)
			ALOGE("Recieved request to get domain list");
		response.domain_list_valid = 1;
		response.total_domains_valid = 1;
		//db_rev_count is not currently used by the APSS. We always
		//pass in the reb as 1 since our database never changes at
		//runtime.
		response.db_rev_count_valid = 1;
		response.db_rev_count = 1;
		response.domain_list_len = 0;
		response.total_domains = 0;
		response.resp.error = QMI_ERR_NONE_V01;
		response.resp.result = QMI_RESULT_SUCCESS_V01;
		req = (qmi_servreg_loc_get_domain_list_req_msg_v01*)reqStruct;
		//NULL terminate the input ourselves just to be extra safe
		(req->service_name)[QMI_SERVREG_LOC_NAME_LENGTH_V01] = '\0';
		if(strlen(req->service_name) > 1) {
			lookup_string = req->service_name;
		} else {
			ALOGE("Invalid string passed for service name");
			response.resp.error = QMI_ERR_MISSING_ARG_V01;
			response.resp.result = QMI_RESULT_FAILURE_V01;
			rcode = qmi_csi_send_resp(reqHandle,
				QMI_SERVREG_LOC_GET_DOMAIN_LIST_RESP_V01,
				&response,
				sizeof(response));
			if (rcode != QMI_CSI_NO_ERR) {
				ALOGE("Invalid rcode from qmi_csi_send_resp:%d",
						rcode);
				cbErr = QMI_CSI_CB_INTERNAL_ERR;
			}
			return cbErr;
		}
		service_map_iterator = service_map.find(lookup_string);
		if (service_map_iterator == service_map.end()) {
			//Nothing provides the service requested.This is not
			//really a error. Return a response packet saying as
			//much.
			response.resp.result = QMI_RESULT_SUCCESS;
			rcode = qmi_csi_send_resp(reqHandle,
				QMI_SERVREG_LOC_GET_DOMAIN_LIST_RESP_V01,
				&response,
				sizeof(response));
			if (rcode != QMI_CSI_NO_ERR) {
				ALOGE("bad rcode from qmi_csi_send_resp: %d",
						rcode);
				cbErr = QMI_CSI_CB_INTERNAL_ERR;
			}
			return cbErr;
		}
		result_list_size = service_map_iterator->second.size();
		//domain offset passed in is invalid
		if (req->domain_offset > result_list_size) {
			ALOGE("Invalid offset for %s :got:%d max ent:%d",
					lookup_string.c_str(),
					req->domain_offset,
					result_list_size);
			response.resp.result = QMI_RESULT_FAILURE_V01;
			response.resp.error = QMI_ERR_GENERAL_V01;
			rcode = qmi_csi_send_resp(reqHandle,
				QMI_SERVREG_LOC_GET_DOMAIN_LIST_RESP_V01,
				&response,
				sizeof(response));
			if (rcode != QMI_CSI_NO_ERR) {
				ALOGE("Failed to send qmi response: rcode: %d",
						rcode);
				cbErr = QMI_CSI_CB_INTERNAL_ERR;
			}
			return cbErr;
		}
		provider_list_iterator = service_map_iterator->second.begin();
		//We have been asked to get the list starting from a specific
		//offset. Seek the iterator to that location.
		if (req->domain_offset > 0)
			std::advance(provider_list_iterator,
					req->domain_offset - 1);
		for (;provider_list_iterator !=
				service_map_iterator->second.end();
				++provider_list_iterator) {
			if (debug_mode_enabled) {
				ALOGI("Provider:[%s]\nDomain:[%s]\nSubDom:[%s]",
					(*provider_list_iterator)->provider->c_str(),
					(*provider_list_iterator)->domain->c_str(),
					(*provider_list_iterator)->subdomain->c_str());
			}
			//The name(technically almost a address) is in the
			//following format
			// soc/domain/subdomain
			snprintf(response.domain_list[\
					response.domain_list_len \
					].name,
					sizeof(response.domain_list\
						[response.domain_list_len].\
						name) - 1,
					"%s/%s/%s",
					(*provider_list_iterator)->soc->c_str(),
					(*provider_list_iterator)->domain->c_str(),
					(*provider_list_iterator)->subdomain->c_str());
			if (debug_mode_enabled)
				ALOGI("Adding :%s: to response",
					response.domain_list
					[response.domain_list_len].name);
			response.domain_list[response.domain_list_len].\
				name[QMI_SERVREG_LOC_NAME_LENGTH_V01 - 1]='\0';
			response.domain_list[response.domain_list_len].service_data_valid =\
				(*provider_list_iterator)->service_data_valid;
			response.domain_list[response.domain_list_len].service_data = \
				(*provider_list_iterator)->service_data;
			response.domain_list[response.domain_list_len].instance_id=\
				(*provider_list_iterator)->instance;
			response.domain_list_len++;
			if (response.domain_list_len >=
					QMI_SERVREG_LOC_LIST_LENGTH_V01 - 1) {
				//We do not have any more space in the current
				//packet to send the entire domain list back.
				//Send back what we have so far. The client
				//will send further requests if he needs any
				//of the additional domains.
				if (debug_mode_enabled)
					ALOGI("Resp truncated : Tot domains: %d\
						Num domains in resp pkt: %d \
						Starting index: %d",
						response.total_domains,
						response.domain_list_len,
						req->domain_offset);
				break;
			}
		}
		response.total_domains = result_list_size;
		rcode = qmi_csi_send_resp(reqHandle,
				QMI_SERVREG_LOC_GET_DOMAIN_LIST_RESP_V01,
				&response,
				sizeof(response));
		if (rcode != QMI_CSI_NO_ERR) {
			ALOGE("invalid resp code from qmi_csi_send_resp: %d",
					rcode);
			cbErr = QMI_CSI_CB_INTERNAL_ERR;
		}
		return cbErr;
	case QMI_SERVREG_LOC_PFR_REQ_V01:
		if (debug_mode_enabled)
			ALOGE("Got request to print subsystem error string");
		pfr_req = (qmi_servreg_loc_pfr_req_msg_v01*)reqStruct;
		//NULL terminate the string ourselves to be extra safe.
		pfr_req->serv_name[QMI_SERVREG_LOC_NAME_LENGTH_V01] = '\0';
		pfr_req->pd_failure_reason[QMI_SERVREG_LOC_PFR_LEN_V01] = '\0';
		if (debug_mode_enabled)
			ALOGE("%s failure reason: %s",
					pfr_req->serv_name,
					pfr_req->pd_failure_reason);
		KLOG_ERROR("PD_ERR", "%s : %s\n",
				pfr_req->serv_name,
				pfr_req->pd_failure_reason);
		pfr_resp.resp.error = QMI_ERR_NONE_V01;
		pfr_resp.resp.result = QMI_RESULT_SUCCESS_V01;
		rcode = qmi_csi_send_resp(reqHandle,
				QMI_SERVREG_LOC_PFR_RESP_V01,
				&pfr_resp,
				sizeof(pfr_resp));
		if (rcode != QMI_CSI_NO_ERR) {
			ALOGE("invalid resp code from qmi_csi_send_resp: %d",
					rcode);
			cbErr = QMI_CSI_CB_INTERNAL_ERR;
		}
		return cbErr;
	default:
		ALOGE("Unsupported request %d recieved by pdMapperServer",
				msgId);
		return QMI_CSI_CB_UNSUPPORTED_ERR;
	}
	return cbErr;
}

void* pd_mapper_qmi_setup(void *data UNUSED)
{
	qmi_idl_service_object_type serviceObject;
	qmi_csi_os_params osParams, osReady;
	qmi_csi_service_handle qmiService = NULL;
	qmi_csi_options options;
	qmi_csi_error error;
	int maxFd, rc;
	int service_registered = 0;

	QMI_CSI_OPTIONS_INIT(options);
	QMI_CSI_OPTIONS_SET_INSTANCE_ID(options,
			QMI_SERVREG_LOC_SERVICE_INSTANCE_APSS_V01);
	serviceObject = servreg_loc_get_service_object_v01();
	FD_ZERO(&osReady.fds);
	if (!serviceObject) {
		ALOGE("Failed to get service locater QMI service obj");
		goto error;
	}
	do {
		error = qmi_csi_register_with_options(serviceObject,
				pdMapperServerConnect,
				pdMapperServerDisconnect,
				pdMapperServerProcessRequest,
				NULL,
				&osParams,
				&options,
				&qmiService);
		if (error == QMI_CSI_NO_ERR) {
			break;
		}
		ALOGE("qmi_csi_register_with_options returned error : %d\n",
				error);
		sleep(2);
	} while(1);
	service_registered = 1;
	maxFd = osParams.max_fd;
	ALOGI("Starting servloc server");
	do {
		osReady = osParams;
		rc = select(maxFd + 1, &osReady.fds, NULL, NULL, NULL);
		switch(rc) {
		case -1:
			ALOGE("Error while starting servloc server");
			continue;
		case 0:
			ALOGE("Timed out while waiting for select");
			continue;
		default:
			break;
		}
		rc = qmi_csi_handle_event(qmiService, &osReady);
		if (rc != QMI_CSI_NO_ERR) {
			ALOGE("Servloc server failed to handle event: %d",rc);
		}

	} while(1);
error:
	ALOGE("pd mapper qmi server exiting");
	if(service_registered)
		qmi_csi_unregister(qmiService);
	return NULL;
}

int drop_permissions()
{
	int i, rcode;
	struct __user_cap_header_struct capheader;
	struct __user_cap_data_struct capdata[2];

	//No need to drop permissions if we are not already running
	//as root
	if(getuid() != AID_ROOT)
		return RET_SUCCESS;
	bzero(&capheader, sizeof(capheader));
	bzero(capdata, sizeof(capdata));
	if (prctl(PR_SET_KEEPCAPS, 1, 0, 0, 0) < 0) {
		ALOGE("PR_SET_KEEPCAPS failed : %s", strerror(errno));
		return RET_FAILED;
	}
	for (i = 0; prctl(PR_CAPBSET_READ, i, 0, 0, 0) >= 0; i++) {
		if (i == CAP_NET_BIND_SERVICE ||
				i == CAP_SETUID ||
				i == CAP_SETGID)
			continue;
		rcode = prctl(PR_CAPBSET_DROP, i, 0, 0, 0);
		if (rcode < 0) {
			ALOGE("Failed to drop capability %d: %s",
					i,
					strerror(errno));
			return RET_FAILED;
		}
	}
	rcode = setgid(AID_SYSTEM);
	if (rcode < 0) {
		ALOGE("Failed to change gid to system: %s",
				strerror(errno));
		return RET_FAILED;
	}
	rcode = setuid(AID_SYSTEM);
	if (rcode < 0) {
		ALOGE("Failed to change uid to system");
		return RET_FAILED;
	}
	capheader.version = _LINUX_CAPABILITY_VERSION_3;
	capheader.pid = 0;
	capdata[CAP_TO_INDEX(CAP_NET_BIND_SERVICE)].permitted |=
		CAP_TO_MASK(CAP_NET_BIND_SERVICE);
	capdata[CAP_TO_INDEX(CAP_NET_BIND_SERVICE)].effective |=
		CAP_TO_MASK(CAP_NET_BIND_SERVICE);
	rcode = capset(&capheader, capdata);
	if (rcode < 0) {
		ALOGE("Failed to set net_bind capability: %s",
				strerror(errno));
		return RET_FAILED;
	}
	return RET_SUCCESS;
}

int main(int argc UNUSED, char *argv[] UNUSED)
{
	map<string,list<struct serviceInfo*> >::iterator service_map_iterator;
	list<struct serviceInfo*>::iterator provider_list_iterator;
	pthread_t serverThread;
	DIR *dir_json_root = NULL;
	struct dirent *de;
	int file_name_len;
	int num_files_parsed = 0;
	char json_file_path[PATH_MAX] = {'\0'};
	char debug_mode_string[PROPERTY_VALUE_MAX] = {'\0'};
	int json_ext_len = strlen(JSON_FILE_EXT);
	uint32_t current_index = 0;
	ALOGI("Starting pd-mapper service");
	if (drop_permissions() != RET_SUCCESS) {
		ALOGE("pd-mapper failed to drop permissions");
		goto error;
	}
	property_get(PD_LOCATER_DEBUG_PROP, debug_mode_string, "false");
	if (!strncmp(debug_mode_string, "true", PROPERTY_VALUE_MAX)) {
		debug_mode_enabled = 1;
	}
        if (debug_mode_enabled) {
                ALOGI("pd-mapper debug mode enabled");
        }
	while(json_dir_list[current_index] != NULL) {
		dir_json_root = opendir(json_dir_list[current_index]);
		if (dir_json_root == NULL) {
			//Not necessarily a error. The path may just not
			//exist on the target.
			if (debug_mode_enabled)
				ALOGI("Unable to open %s: %s",
						json_dir_list[current_index],
						strerror(errno));
			current_index++;
			continue;
		}
		while ((de = readdir(dir_json_root))) {
			file_name_len = strlen(de->d_name);
			if (file_name_len < (json_ext_len + 1))
				continue;
			if (!strncmp(&de->d_name[file_name_len - json_ext_len],
						JSON_FILE_EXT,
						json_ext_len)) {
				memset(json_file_path, '\0',
						sizeof(json_file_path));
				snprintf(json_file_path,
						sizeof(json_file_path) - 1,
						"%s/%s",
						json_dir_list[current_index],
						de->d_name);
				ALOGI("Parsing file :%s:", json_file_path);
				if (update_map_for_file(json_file_path) !=
						RET_SUCCESS) {
					ALOGE("Failed to parse json file %s",
							json_file_path);
					continue;
				}
				num_files_parsed++;
			}
		}
		closedir(dir_json_root);
		dir_json_root = NULL;
		current_index++;
	}
	if (debug_mode_enabled) {
		ALOGI("%d configuration files parsed", num_files_parsed);
		ALOGI("Dumping map contents");
		for (service_map_iterator = service_map.begin();
				service_map_iterator != service_map.end();
				++service_map_iterator) {
			ALOGI("Service : %s Provided by:",
					service_map_iterator->first.c_str());
			for (provider_list_iterator =
					service_map_iterator->second.begin();
					provider_list_iterator !=
					service_map_iterator->second.end();
					++provider_list_iterator) {
				ALOGI("Provider:[%s]\nDom:[%s]\nSubDom:[%s]",
						(*provider_list_iterator)->\
						provider->c_str(),
						(*provider_list_iterator)->\
						domain->c_str(),
						(*provider_list_iterator)->\
						subdomain->c_str());
				ALOGI("service_data_valid: %d\nservice_data:%d",
						(*provider_list_iterator)->\
						service_data_valid,
						(*provider_list_iterator)->\
						service_data);
			}
		}
	}
	pthread_create(&serverThread, NULL, pd_mapper_qmi_setup, NULL);
	do {
		sleep(50000);
	} while(1);
	return RET_SUCCESS;
error:
	if (dir_json_root)
		closedir(dir_json_root);
	return RET_FAILED;
}

int update_map_for_file (char *file) {
	struct json_object *obj[MAX_JSON_NEST_LEVEL];
	map<string,list<struct serviceInfo*> >::iterator service_map_iterator;
	list<struct serviceInfo*>::iterator provider_list_iterator;
	int major,minor,patch,i;
	const char* soc;
	const char *domain;
	const char *subdomain;
	const char *provider;
	const char *service;
	uint32_t instance;
	uint32_t service_data_valid;
	uint32_t service_data;
	char lookup_string[MAX_LOOKUP_STRING_LEN];
	struct serviceInfo *service_info = NULL;

	obj[JSON_NEST_LEVEL_ROOT] =
		json_object_from_file(file);
	if (obj[JSON_NEST_LEVEL_ROOT] == NULL) {
		ALOGE("Failed to parse json file");
		goto error;
	}
	//Get main sr_version
	if (!json_object_object_get_ex(obj[JSON_NEST_LEVEL_ROOT],
				SR_VERSION_FIELD,
				&obj[JSON_NEST_LEVEL_FIRST])) {
		ALOGE("Failed to get version entry");
		goto error;
	}

	//Get major version from sr_version obj
	if (!json_object_object_get_ex(obj[JSON_NEST_LEVEL_FIRST],
			SR_VERSION_FIELD_MAJOR,
			&obj[JSON_NEST_LEVEL_SECOND])) {
		ALOGE("Failed to get major version");
		goto error;
	}
	major = json_object_get_int(obj[JSON_NEST_LEVEL_SECOND]);

	//Get minor version from sr_version obj
	if (!json_object_object_get_ex(obj[JSON_NEST_LEVEL_FIRST],
				SR_VERSION_FIELD_MINOR,
				&obj[JSON_NEST_LEVEL_SECOND])) {
		ALOGE("Failed to get minor version");
		goto error;
	}
	minor = json_object_get_int(obj[JSON_NEST_LEVEL_SECOND]);

	//Get patch version from sr_version obj
	if (!json_object_object_get_ex(obj[JSON_NEST_LEVEL_FIRST],
				SR_VERSION_FIELD_PATCH,
				&obj[JSON_NEST_LEVEL_SECOND])) {
		ALOGE("Failed to get patch version");
		goto error;
	}
	patch = json_object_get_int(obj[JSON_NEST_LEVEL_SECOND]);
	if (debug_mode_enabled)
		ALOGI("File Version : %d.%d.%d",major,minor,patch);
	//Get info about which "domain" the file we are parsing represents

	//Main sr_domain
	if (!json_object_object_get_ex(obj[JSON_NEST_LEVEL_ROOT],
				SR_DOMAIN_FIELD,
				&obj[JSON_NEST_LEVEL_FIRST])) {
		ALOGE("Failed to get domain");
		goto error;
	}

	//Soc field
	if (!json_object_object_get_ex(obj[JSON_NEST_LEVEL_FIRST],
				SR_DOMAIN_FIELD_SOC,
				&obj[JSON_NEST_LEVEL_SECOND])) {
		ALOGE("Failed to get SOC field");
		goto error;
	}
	soc = json_object_get_string(obj[JSON_NEST_LEVEL_SECOND]);
	if (!soc) {
		ALOGE("Failed to get soc string from json obj");
		goto error;
	}
	if (debug_mode_enabled)
		ALOGI("SOC: %s", soc);

	//Domain field
	if (!json_object_object_get_ex(obj[JSON_NEST_LEVEL_FIRST],
				SR_DOMAIN_FIELD_DOMAIN,
				&obj[JSON_NEST_LEVEL_SECOND])) {
		ALOGE("Failed to get domain field");
		goto error;
	}
	domain = json_object_get_string(obj[JSON_NEST_LEVEL_SECOND]);
	if (!domain) {
		ALOGE("Failed to get domain string from json obj");
		goto error;
	}

	if (debug_mode_enabled)
		ALOGI("Domain: %s", domain);
	//Subdomain field
	if (!json_object_object_get_ex(obj[JSON_NEST_LEVEL_FIRST],
				SR_DOMAIN_FIELD_SUBDOMAIN,
				&obj[JSON_NEST_LEVEL_SECOND])) {
		ALOGE("Failed to get subdomain field");
		goto error;
	}
	subdomain = json_object_get_string(obj[JSON_NEST_LEVEL_SECOND]);
	if (!subdomain) {
		ALOGE("Failed to get subdomain string from json obj");
		goto error;
	}
	if (debug_mode_enabled)
		ALOGI("Subdomain: %s", subdomain);

	//Instance id field
	if (!json_object_object_get_ex(obj[JSON_NEST_LEVEL_FIRST],
				SR_DOMAIN_FIELD_INSTANCE,
				&obj[JSON_NEST_LEVEL_SECOND])) {
		ALOGE("Failed to get instance field");
		goto error;
	}
	instance = json_object_get_int(obj[JSON_NEST_LEVEL_SECOND]);
	if (debug_mode_enabled)
		ALOGI("Instance is : %d", instance);

	//Get list of services provided by this "domain"
	if (!json_object_object_get_ex(obj[JSON_NEST_LEVEL_ROOT],
				SR_SERVICE_FIELD,
				&obj[JSON_NEST_LEVEL_FIRST])) {
		ALOGE("Failed to get sr_service array");
		goto error;
	}
	for (i = 0; i < json_object_array_length(obj[JSON_NEST_LEVEL_FIRST]);
			i++) {
		service_info =
			(struct serviceInfo*)malloc(sizeof(struct serviceInfo));
		if (!service_info) {
			ALOGE("Failed to allocate serviceInfo struct");
			goto error;
		}
		obj[JSON_NEST_LEVEL_SECOND] =
			json_object_array_get_idx(obj[JSON_NEST_LEVEL_FIRST],i);
		if (!json_object_object_get_ex(obj[JSON_NEST_LEVEL_SECOND],
					SR_SERVICE_FIELD_PROVIDER,
					&obj[JSON_NEST_LEVEL_THIRD])) {
			ALOGE("Failed to get provider field");
			goto error;
		}
		provider = json_object_get_string(obj[JSON_NEST_LEVEL_THIRD]);
		if (!provider) {
			ALOGE("Failed to get provider value from json obj");
			goto error;
		}

		if (!json_object_object_get_ex(obj[JSON_NEST_LEVEL_SECOND],
					SR_SERVICE_FIELD_SERVICE,
					&obj[JSON_NEST_LEVEL_THIRD])) {
			ALOGE("Failed to get service field");
			goto error;
		}
		service = json_object_get_string(obj[JSON_NEST_LEVEL_THIRD]);
		if (!service) {
			ALOGE("Failed to get service string from json obj");
			goto error;
		}

		if (!json_object_object_get_ex(obj[JSON_NEST_LEVEL_SECOND],
					SR_SERVICE_FIELD_SERVICE_DATA_VALID,
					&obj[JSON_NEST_LEVEL_THIRD])) {
			ALOGE("Failed to get service_data_valid field");
			goto error;
		}
		service_data_valid =
			json_object_get_int(obj[JSON_NEST_LEVEL_THIRD]);

		if (!json_object_object_get_ex(obj[JSON_NEST_LEVEL_SECOND],
					SR_SERVICE_FIELD_SERVICE_DATA,
					&obj[JSON_NEST_LEVEL_THIRD])) {
			ALOGE("Failed to get service_data field");
			goto error;
		}
		service_data =
			json_object_get_int(obj[JSON_NEST_LEVEL_THIRD]);
		memset(lookup_string, '\0', sizeof(lookup_string));
		snprintf(lookup_string,
				sizeof(lookup_string) - 1,
				"%s/%s",
				provider,
				service);
		if (debug_mode_enabled)
			ALOGI("Provider %s found for service %s",
					provider,
					service);
		service_info->provider = new string(provider);
		service_info->soc = new string(soc);
		service_info->domain = new string(domain);
		service_info->subdomain = new string(subdomain);
		service_info->instance = instance;
		service_info->service_data_valid = service_data_valid;
		service_info->service_data = service_data;
		service_map_iterator = service_map.find(lookup_string);
		if (service_map_iterator != service_map.end()) {
			service_map_iterator->second.push_front(service_info);
		} else {
			list<struct serviceInfo*> *service_list =
				new list<struct serviceInfo*>;
			service_list->push_front(service_info);
			service_map[lookup_string] = *service_list;
		}
	}
	return RET_SUCCESS;
error:
	if (service_info)
		free(service_info);
	return RET_FAILED;
}

void servloc_clnt_cb(
		qmi_client_type user_handle UNUSED,
		unsigned int msg_id,
		void *ind_buf UNUSED,
		unsigned int ind_buf_len UNUSED,
		void *data UNUSED)
{
	if (debug_mode_enabled)
		ALOGI("Client recieved msg: id: 0x%x of len %d",
				(unsigned int)msg_id,
				ind_buf_len);
}
