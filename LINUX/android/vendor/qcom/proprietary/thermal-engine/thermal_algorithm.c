/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/
#include "thermal_algorithm.h"
#include "thermal.h"
#include "sensors_manager.h"
#include "devices_manager.h"
#include "devices/devices_actions.h"
#include "tb_algorithm.h"
#include <signal.h>
#include <pthread.h>
#include <sys/time.h>
#include <float.h>
#include <string.h>
#include <stdlib.h>

#define MAX_SAMPLE_PERIOD (1000000U)
#define TIMER_EV  0x1
#define THRESH_EV 0x2
#define OVERRIDE_EV 0x4
#define CONFIG_SET_EV 0x8
#define MAX_INSTANCES_SUPPORTED 32
#define THERMAL_LOW_MIN_THRESHOLD 1

#define TIMER_SAMPLING_SET(s_grp, sampling) { \
	(s_grp->timer_val.it_value.tv_sec = (sampling / 1000)); \
	(s_grp->timer_val.it_value.tv_nsec = ((sampling % 1000) * 1000000)); \
}

#define ALL_DEVICES_SAMPGROUPS (~(0U))

struct thermal_start_data {
	const struct thermal_algo_info*  info;
	const struct thermal_setting_t*  setting;
};

struct thermal_thread_info {
	struct thermal_start_data*       start_data;
	pthread_t*                       thread;
};

struct thermal_thread_info_node {
	struct thermal_thread_info*      info;
	struct thermal_thread_info_node* next;
};

enum thermal_timer_restart_t {
	TIMER_DO_NOT_RESTART,
	TIMER_DO_RESTART
};

enum thermal_device_freq_applied_t {
	THERMAL_DEVICE_FREQ_NOT_APPLIED,
	THERMAL_DEVICE_FREQ_APPLIED
};

static struct thermal_thread_info_node* thermal_threads;

static inline enum thermal_algo_ret_t thermal_settimer(
	struct thermal_algo_sampgroup* sampgroup)
{
	int err_code = 0;
	int ret_val;

	THERMAL_ERRCHK((sampgroup == NULL), handle_err, err_code);

	ret_val = clock_gettime(CLOCK_MONOTONIC, &sampgroup->timer_start);
	THERMAL_ERRCHK((ret_val), handle_err, err_code);

	ret_val = timer_settime(sampgroup->timer_id, 0,
		&sampgroup->timer_val, NULL);
	THERMAL_ERRCHK((ret_val), handle_err, err_code);

	dbgmsg("%s: Start timer %d.%03d(sec)", __func__,
		(int)sampgroup->timer_val.it_value.tv_sec,
		(int)sampgroup->timer_val.it_value.tv_nsec / 1000000);

	return THERMAL_ALGO_SUCCESS;

handle_err:
	THERMAL_ERRMSG(err_code);

	return THERMAL_ALGO_FAILURE;
}

/* called when a high threshold is crossed */
static void thermal_thresh_notify(
	sensor_clnt_handle clnt,
	enum sensor_notify_event_type event,
	int reading,
	void *data)
{
	int err_code = 0;

	THERMAL_ERRCHK((clnt == NULL), handle_err, err_code);

	struct thermal_algo_client* thresh_client =
		(struct thermal_algo_client*)data;
	THERMAL_ERRCHK((thresh_client == NULL), handle_err, err_code);

	struct thermal_algo_device* thresh_device = thresh_client->device;
	THERMAL_ERRCHK((thresh_device == NULL), handle_err, err_code);

	struct thermal_algo_sampgroup* thresh_sampgroup = thresh_device->sampgroup;
	THERMAL_ERRCHK((thresh_sampgroup == NULL), handle_err, err_code);

	THERMAL_ERRCHK((thresh_client->parent_algo == NULL), handle_err, err_code);

	dbgmsg("%s: algo %s, In thresh notify for client %s, device %s,"
		" sampling group %d",__func__,
		thresh_client->parent_algo->info->algo_name,
		thresh_client->name, thresh_device->name, thresh_sampgroup->sampling);

	THERMAL_ERRCHK(
		(pthread_mutex_lock(&thresh_client->parent_algo->wait_mutex) != 0),
		handle_err, err_code);

	/* Note a threshold AND timer interrupt, to enable faster response */
	thresh_client->parent_algo->ev_mask |= THRESH_EV;
	thresh_client->parent_algo->ev_mask |= TIMER_EV;
	thresh_client->parent_algo->sampgroup_mask |= thresh_sampgroup->timer_mask;
	thresh_client->parent_algo->device_thresh_mask |= thresh_device->mask;

	THERMAL_ERRCHK(
		(pthread_cond_broadcast(&thresh_client->parent_algo->wait_cond) != 0),
		handle_err_unlock, err_code);

	THERMAL_ERRCHK(
		(pthread_mutex_unlock(&thresh_client->parent_algo->wait_mutex) != 0),
		handle_err, err_code);

	/* If successful, just return */
	return;

handle_err_unlock:
	pthread_mutex_unlock(&thresh_client->parent_algo->wait_mutex);

handle_err:
	THERMAL_ERRMSG(err_code);

	return;
}

/* Called when a threshold is crossed, up or down */
static enum thermal_algo_ret_t thermal_handle_thresh(
	enum thermal_device_state_t transition,
	uint32_t device_mask,
	struct thermal_algo* algo)
{
	int err_code = 0;

	THERMAL_ERRCHK((algo == NULL), handle_err, err_code);
	THERMAL_ERRCHK((transition >= THERMAL_DEVICE_STATE_UNKNOWN),
		handle_err, err_code);

	dbgmsg("In thermal handle thresh sig for algorithm: %s with transition: %d"
		" and mask: %d", algo->info->algo_name, transition, device_mask);

	struct thermal_list_node* algo_devices = algo->devices;
	while(algo_devices) {
		if (((algo_devices->elem.device->mask & device_mask) == 0)
			|| (algo_devices->elem.device->disabled == THERMAL_DISABLED)) {
			algo_devices = algo_devices->next;
			continue;
		}

		/* turn on mitigation */
		if (transition == THERMAL_DEVICE_MITIGATION_ON) {
			TIMER_SAMPLING_SET(algo_devices->elem.device->sampgroup,
				algo_devices->elem.device->sampgroup->sampling);

			algo_devices->elem.device->params->mitigation =
				THERMAL_DEVICE_MITIGATION_ON;
			dbgmsg("%s: algo %s, turning on mitigation for device %s", __func__,
				algo->info->algo_name, algo_devices->elem.device->name);

			/* Initialize curr_level to 0, assuming we're starting at max freq.
			 * this will work in general, but it's on the algorithms themselves
			 * to deal with discrepancies since the framework can't be platform
			 * specific. See token bucket for example
			 * */
			algo_devices->elem.device->curr_lvl = 0;

			THERMAL_ERRCHK(
				(algo->info->thresh_turn_on(algo_devices->elem.device, algo)
				!= THERMAL_ALGO_SUCCESS ), handle_err, err_code);
		} else {
			/* Cancel previous pending requests on init */
			if (transition == THERMAL_DEVICE_INIT)
				device_clnt_cancel_request(algo_devices->elem.device->dev_clnt);

			union device_request req;
			req.value = THERMAL_GET_MAX_FREQ(algo_devices->elem.device);
			THERMAL_ERRCHK(
				(device_clnt_request(algo_devices->elem.device->dev_clnt, &req)
				!= req.value), handle_err, err_code);

			algo_devices->elem.device->params->mitigation =
				THERMAL_DEVICE_MITIGATION_OFF;
			dbgmsg("%s: algo %s, turning off mitigation for device %s",
				__func__, algo->info->algo_name,
				algo_devices->elem.device->name);

			THERMAL_ERRCHK(
				(algo->info->thresh_turn_off(algo_devices->elem.device, algo)
				!= THERMAL_ALGO_SUCCESS ), handle_err, err_code);
		}

		/* Loop over all clients and set thresholds accordingly */
		struct thermal_list_node* device_clients =
			algo_devices->elem.device->clients;
		while (device_clients) {
			if (device_clients->elem.client->disabled == THERMAL_DISABLED) {
				device_clients = device_clients->next;
				continue;
			}

			struct sensor_thresh_req thresh_request;
			memset(&thresh_request, 0x0, sizeof(struct sensor_thresh_req));
			thresh_request.notify_cb_func = thermal_thresh_notify;
			thresh_request.notify_cb_data = (void *)device_clients->elem.client;

			if ((transition == THERMAL_DEVICE_MITIGATION_OFF)
				|| (transition == THERMAL_DEVICE_INIT)) {
				thresh_request.thresh.high_valid = 1;
				thresh_request.thresh.high =
					device_clients->elem.client->active_set_point;
				thresh_request.thresh.low_valid = 0;
				thresh_request.thresh.low = 0;
			} else {
				thresh_request.thresh.high_valid = 0;
				thresh_request.thresh.high = 0;
				thresh_request.thresh.low_valid = 1;
				thresh_request.thresh.low = THERMAL_LOW_MIN_THRESHOLD;
			}
			sensors_manager_set_thresh_lvl(
				device_clients->elem.client->sensor_clnt, &thresh_request);
			device_clients = device_clients->next;
		}
		algo_devices = algo_devices->next;
	}

	dbgmsg("%s: algo %s completed successfully", __func__,
		algo->info->algo_name);

	return THERMAL_ALGO_SUCCESS;

handle_err:
	THERMAL_ERRMSG(err_code);

	return THERMAL_ALGO_FAILURE;
}

static void thermal_timer_expired(union sigval sval)
{
	int err_code = 0;
	struct thermal_algo_sampgroup* timer_sampgroup =
		(struct thermal_algo_sampgroup*)sval.sival_ptr;

	THERMAL_ERRCHK((timer_sampgroup == NULL), handle_err, err_code);
	THERMAL_ERRCHK((timer_sampgroup->parent_algo == NULL),
		handle_err, err_code);
	THERMAL_ERRCHK(
		(pthread_mutex_lock(&timer_sampgroup->parent_algo->wait_mutex) != 0),
		handle_err, err_code);

	timer_sampgroup->parent_algo->ev_mask |= TIMER_EV;
	timer_sampgroup->parent_algo->sampgroup_mask |= timer_sampgroup->timer_mask;

	THERMAL_ERRCHK(
		(pthread_cond_broadcast(&timer_sampgroup->parent_algo->wait_cond) != 0),
		handle_err_unlock, err_code);
	THERMAL_ERRCHK(
		(pthread_mutex_unlock(&timer_sampgroup->parent_algo->wait_mutex) != 0),
		handle_err, err_code);

	return;

handle_err_unlock:
	pthread_mutex_unlock(&timer_sampgroup->parent_algo->wait_mutex);

handle_err:
	THERMAL_ERRMSG(err_code);

	return;
}

static enum thermal_algo_ret_t thermal_handle_timer(
	uint32_t sampling_group_mask,
	struct thermal_algo* algo)
{
	int err_code = 0;
	THERMAL_ERRCHK((algo == NULL), handle_err, err_code);

	/* loop over sampgroups */
	struct thermal_list_node* algo_sampgroups = algo->sampgroups;
	while (algo_sampgroups) {
		algo_sampgroups->elem.sampgroup->restart = TIMER_DO_NOT_RESTART;

		/* if this is sampgroup which needs servicing */
		if (sampling_group_mask & algo_sampgroups->elem.sampgroup->timer_mask) {

			/* loop over devices in sampgroup */
			struct thermal_list_node* sampgroup_devices =
				algo_sampgroups->elem.sampgroup->devices;
			while (sampgroup_devices) {

				/* if this is not an active device */
				if ((sampgroup_devices->elem.device->disabled ==
					THERMAL_DISABLED)
					|| (sampgroup_devices->elem.device->params->mitigation ==
					THERMAL_DEVICE_MITIGATION_OFF)) {
					sampgroup_devices = sampgroup_devices->next;
					continue;
				}

				dbgmsg("%s: algo %s, beginning timer interrupt for device: %s",
					__func__, algo->info->algo_name,
					sampgroup_devices->elem.device->name);

				/* loop over device clients */
				struct thermal_list_node* device_clients =
					sampgroup_devices->elem.device->clients;
				while (device_clients) {
					device_clients->elem.client->temp =
						sensors_manager_read(
							device_clients->elem.client->sensor_clnt);
					device_clients = device_clients->next;
				}

				THERMAL_ERRCHK(
					(algo->info->mitigate(sampgroup_devices->elem.device, algo)
					!= THERMAL_ALGO_SUCCESS), handle_err, err_code);

				/* if algo-specific mitigation requested freq change */
				if ((sampgroup_devices->elem.device->params->change_freq ==
					THERMAL_DEVICE_CHANGE_FREQ)
					|| (sampgroup_devices->elem.device->freq_applied ==
					THERMAL_DEVICE_FREQ_NOT_APPLIED)) {

					dbgmsg("%s: algo %s, Requesting that device %s be changed "
						"to frequency %d", __func__, algo->info->algo_name,
						sampgroup_devices->elem.device->name,
						sampgroup_devices->elem.device->params->new_freq);

					union device_request req;
					req.value =
						MAX(sampgroup_devices->elem.device->params->new_freq,
						THERMAL_GET_MIN_FREQ(sampgroup_devices->elem.device));
					int applied_lvl =
						device_clnt_request(
							sampgroup_devices->elem.device->dev_clnt, &req);

					if (applied_lvl !=
						sampgroup_devices->elem.device->params->new_freq) {
						sampgroup_devices->elem.device->freq_applied =
							THERMAL_DEVICE_FREQ_NOT_APPLIED;
					} else {
						sampgroup_devices->elem.device->freq_applied =
							THERMAL_DEVICE_FREQ_APPLIED;

						sampgroup_devices->elem.device->curr_lvl =
							THERMAL_GET_NUM_LVLS(
								sampgroup_devices->elem.device);

						uint32_t curr_lvl;
						for (curr_lvl = 0; curr_lvl <
							THERMAL_GET_NUM_LVLS(
								sampgroup_devices->elem.device);
							curr_lvl++) {
							if (THERMAL_GET_LVL_FREQ(
								sampgroup_devices->elem.device,
								curr_lvl) == applied_lvl) {
									sampgroup_devices->elem.device->curr_lvl =
										curr_lvl;
									break;
							}
						}
						THERMAL_ERRCHK(
							(sampgroup_devices->elem.device->curr_lvl ==
							THERMAL_GET_NUM_LVLS(
								sampgroup_devices->elem.device)),
							handle_err, err_code);
					}
					sampgroup_devices->elem.device->params->change_freq =
						THERMAL_DEVICE_NO_CHANGE;
				}

				/* if we still need mitigation, restart timers */
				if ((sampgroup_devices->elem.device->params->mitigation ==
					THERMAL_DEVICE_MITIGATION_ON) ||
					(sampgroup_devices->elem.device->freq_applied ==
					THERMAL_DEVICE_FREQ_NOT_APPLIED)) {
					algo_sampgroups->elem.sampgroup->restart = TIMER_DO_RESTART;
				} else if (sampgroup_devices->elem.device->params->mitigation ==
					THERMAL_DEVICE_MITIGATION_OFF) {
					thermal_handle_thresh(THERMAL_DEVICE_MITIGATION_OFF,
						sampgroup_devices->elem.device->mask, algo);
				}
				sampgroup_devices = sampgroup_devices->next;
			}
		}

		if (algo_sampgroups->elem.sampgroup->restart == TIMER_DO_RESTART) {
			THERMAL_ERRCHK((thermal_settimer(algo_sampgroups->elem.sampgroup) ==
				THERMAL_ALGO_FAILURE ), handle_err, err_code);

			dbgmsg("%s: algo %s, Successfully restarted timer for %d ms",
				__func__, algo->info->algo_name,
				algo_sampgroups->elem.sampgroup->sampling);
		}
		algo_sampgroups = algo_sampgroups->next;
	}

	return THERMAL_ALGO_SUCCESS;

handle_err:
	THERMAL_ERRMSG(err_code);

	return THERMAL_ALGO_FAILURE;
}

/* create a sampgroup. Note: implicit max of 32 per algo
 * due to 32-bit bitmask used to keep track of them
 */
static struct thermal_algo_sampgroup* thermal_create_sampgroup(
	struct thermal_algo_client* client,
	struct thermal_algo* algo)
{
	int err_code = 0;

	THERMAL_ERRCHK((client == NULL || algo == NULL), return_err, err_code);
	THERMAL_ERRCHK((algo->sampgroup_count >= MAX_INSTANCES_SUPPORTED),
		return_err, err_code);

	struct thermal_algo_sampgroup* new_sampgroup =
		(struct thermal_algo_sampgroup*)
		malloc(sizeof(struct thermal_algo_sampgroup));
	THERMAL_ERRCHK((new_sampgroup == NULL), return_err, err_code);

	/* algo-specific initializaion */
	THERMAL_ERRCHK((algo->info->init_sampgroup(new_sampgroup, client)
		!= THERMAL_ALGO_SUCCESS), handle_err_full, err_code);

	dbgmsg("%s: algo %s, creating sampgroup %dms", __func__,
		algo->info->algo_name, new_sampgroup->sampling);

	new_sampgroup->timer_mask = algo->init_sampgroup_mask;
	new_sampgroup->devices = NULL;
	new_sampgroup->parent_algo = algo;
	new_sampgroup->disabled = THERMAL_ENABLED;

	struct thermal_list_node* new_node =
		(struct thermal_list_node*)malloc(sizeof(struct thermal_list_node));
	THERMAL_ERRCHK((new_node == NULL), handle_err_full, err_code);

	new_node->elem.sampgroup = new_sampgroup;
	new_node->next = algo->sampgroups;
	algo->sampgroups = new_node;
	algo->init_sampgroup_mask = algo->init_sampgroup_mask << 1;
	algo->sampgroup_count += 1;

	return new_sampgroup;

handle_err_full:
	free(new_sampgroup);

return_err:
	THERMAL_ERRMSG(err_code);

	return NULL;
}

/* create a device. Note: implicit max of 32 per algo
 * due to 32-bit bitmask used to keep track of them
 */
static struct thermal_algo_device* thermal_create_device(
	struct thermal_algo_client* client,
	struct thermal_algo* algo)
{
	int err_code = 0;

	THERMAL_ERRCHK((client == NULL || algo == NULL), return_err, err_code);
	THERMAL_ERRCHK((algo->device_count >= MAX_INSTANCES_SUPPORTED),
		return_err, err_code);

	struct thermal_algo_sampgroup* device_sampgroup = NULL;
	struct thermal_algo_device* new_device =
		(struct thermal_algo_device*)malloc(sizeof(struct thermal_algo_device));
	THERMAL_ERRCHK((new_device == NULL), return_err, err_code);

	dbgmsg("%s: algo %s, creating device", __func__, algo->info->algo_name);

	new_device->name = client->devicename;
	THERMAL_ERRCHK((new_device->name == NULL), handle_err_free_only, err_code);

	new_device->dev_clnt = devices_manager_reg_clnt(new_device->name);
	THERMAL_ERRCHK((new_device->dev_clnt == NULL),
		handle_err_free_only, err_code);

	struct device_info dev_info;
	THERMAL_ERRCHK((devices_manager_get_info(new_device->name, &dev_info)),
		handle_err_dereg_and_free, err_code);
	THERMAL_ERRCHK(( (dev_info.dev_type != DEVICE_OP_VALUE_TYPE) ||
		(dev_info.num_of_levels == 0) ), handle_err_dereg_and_free,
		err_code);

	new_device->lvl_arr = (struct device_lvl_info *)
		malloc(sizeof(struct device_lvl_info) * dev_info.num_of_levels);
	THERMAL_ERRCHK((new_device->lvl_arr == NULL), handle_err_dereg_and_free,
		err_code);

	THERMAL_ERRCHK(
		(devices_manager_get_lvl_list(new_device->name, new_device->lvl_arr,
		&dev_info.num_of_levels)), handle_err_full, err_code);

	struct thermal_algo_device_algo_params* new_params =
		(struct thermal_algo_device_algo_params*)malloc(
			sizeof(struct thermal_algo_device_algo_params));
	THERMAL_ERRCHK((new_params == NULL), handle_err_full, err_code);

	new_device->params = new_params;
	new_device->num_of_lvls             = dev_info.num_of_levels;
	new_device->clients                 = NULL;
	new_device->freq_applied            = THERMAL_DEVICE_FREQ_APPLIED;
	new_device->params->mitigation      = THERMAL_DEVICE_MITIGATION_OFF;
	new_device->params->new_freq        = 0;
	new_device->params->change_freq     = THERMAL_DEVICE_NO_CHANGE;
	new_device->mask                    = algo->init_device_mask;
	new_device->curr_lvl                = 0;
	new_device->parent_algo             = algo;
	new_device->disabled                = THERMAL_DISABLED;

	/* Needs to be done AFTER device has its frequencies populated */
	THERMAL_ERRCHK(
		(algo->info->init_device(new_device, client) != THERMAL_ALGO_SUCCESS),
		handle_err_free_only, err_code);

	/* get sampling period and look for a sampgroup
	 * if no sampgroup found, make one
	 */
	new_device->sampling_period =
		MIN(MAX_SAMPLE_PERIOD, new_device->sampling_period);
	struct thermal_list_node* thermal_sampgroups = algo->sampgroups;
	while (thermal_sampgroups) {
		if (thermal_sampgroups->elem.sampgroup->sampling ==
			new_device->sampling_period) {
			device_sampgroup = thermal_sampgroups->elem.sampgroup;
			break;
		}
		thermal_sampgroups = thermal_sampgroups->next;
	}

	/* we did not find a sampgroup */
	if (device_sampgroup == NULL)
		device_sampgroup = thermal_create_sampgroup(client, algo);

	THERMAL_ERRCHK((device_sampgroup == NULL), handle_err_full, err_code);
	new_device->sampgroup = device_sampgroup;

	struct thermal_list_node* new_sampgroup_node =
		(struct thermal_list_node*)malloc(sizeof(struct thermal_list_node));
	THERMAL_ERRCHK((new_sampgroup_node == NULL), handle_err_full, err_code);

	new_sampgroup_node->elem.device = new_device;
	new_sampgroup_node->next = device_sampgroup->devices;
	device_sampgroup->devices = new_sampgroup_node;

	struct thermal_list_node* new_device_node =
		(struct thermal_list_node*)malloc(sizeof(struct thermal_list_node));
	THERMAL_ERRCHK((new_device_node == NULL), handle_err_full, err_code);

	new_device_node->elem.device = new_device;
	new_device_node->next = algo->devices;
	algo->devices = new_device_node;

	algo->init_device_mask = algo->init_device_mask << 1;
	algo->device_count += 1;

	return new_device;

handle_err_full:
	free(new_device->lvl_arr);

handle_err_dereg_and_free:
	devices_manager_dereg_clnt(new_device->dev_clnt);

handle_err_free_only:
	free(new_device);

return_err:
	THERMAL_ERRMSG(err_code);

	return NULL;
}

/* create a client */
static struct thermal_algo_client* thermal_create_client(
	const struct setting_info* cfg,
	struct thermal_algo* algo)
{
	int err_code = 0;

	THERMAL_ERRCHK((cfg == NULL || algo == NULL), return_err, err_code);

	struct thermal_algo_device* client_device = NULL;

	struct thermal_algo_client* new_client =
		(struct thermal_algo_client*)malloc(sizeof(struct thermal_algo_client));

	THERMAL_ERRCHK((new_client == NULL), return_err, err_code);

	dbgmsg("%s: algo %s, creating client %s",
		__func__, algo->info->algo_name, cfg->desc);

	new_client->disabled    = (cfg->disable == 0) ?
		THERMAL_ENABLED : THERMAL_DISABLED;
	new_client->name        = cfg->desc;
	new_client->temp        = 0;
	new_client->setting     = cfg;
	new_client->parent_algo = algo;

	/* algo-specific client init */
	THERMAL_ERRCHK(
		(algo->info->init_client(new_client, cfg) != THERMAL_ALGO_SUCCESS),
		handle_err_free_only, err_code);

	dbgmsg("%s: algo %s, client %s algo-specific initialization worked",
		__func__, algo->info->algo_name, cfg->desc);

	/* find a device for the client */
	struct thermal_list_node* algo_devices = algo->devices;
	while (algo_devices) {
		if (strncmp(new_client->devicename, algo_devices->elem.device->name,
			DEVICES_MAX_NAME_LEN) == 0) {
			dbgmsg("%s: algo %s, found existing device for client %s",
				__func__, algo->info->algo_name, cfg->desc);

			client_device = algo_devices->elem.device;
			break;
		}
		algo_devices = algo_devices->next;
	}

	/* did not find a device, so make one */
	if (client_device == NULL) {
		dbgmsg("%s: algo %s, creating device for client %s",
			__func__, algo->info->algo_name, cfg->desc);

		client_device = thermal_create_device(new_client, algo);

	}
	THERMAL_ERRCHK((client_device == NULL), handle_err_full, err_code);

	new_client->device  = client_device;

	struct thermal_list_node* new_device_node =
		(struct thermal_list_node*)malloc(sizeof(struct thermal_list_node));
	THERMAL_ERRCHK((new_device_node == NULL), handle_err_full, err_code);

	new_device_node->elem.client = new_client;
	new_device_node->next = client_device->clients;
	client_device->clients = new_device_node;

	struct thermal_list_node* new_client_node =
		(struct thermal_list_node*)malloc(sizeof(struct thermal_list_node));
	THERMAL_ERRCHK((new_client_node == NULL), handle_err_full, err_code);

	new_client_node->elem.client = new_client;
	new_client_node->next = algo->clients;
	algo->clients = new_client_node;
	algo->client_count += 1;

	return new_client;

handle_err_full:
	sensors_manager_dereg_clnt(new_client->sensor_clnt);

handle_err_free_only:
	free(new_client);

return_err:
	THERMAL_ERRMSG(err_code);

	return NULL;
}

/* initialize a thermal-engine algorithm instance */
static enum thermal_algo_ret_t thermal_algo_register_instance(
	struct thermal_algo* algo,
	const struct thermal_setting_t *setting )
{
	int err_code = 0;
	struct setting_info *cfg;

	THERMAL_ERRCHK(((algo == NULL) || (setting == NULL)), return_err, err_code);

	dbgmsg("%s: algo %s, beginning initialization", __func__,
		algo->info->algo_name);

	cfg = setting->list;
	/* Import settings */
	while (cfg != NULL) {
		/* If we don't care about this config setting */
		if ((cfg->algo_type != algo->info->algo_type)
			 || (cfg->err_disable) || (cfg->disable)) {
			cfg = cfg->next;
			continue;
		}

		struct thermal_algo_client* new_client =
			thermal_create_client(cfg, algo);
		THERMAL_ERRCHK((new_client == NULL), return_err, err_code);

		cfg = cfg->next;
	}

	dbgmsg("%s: algo %s, successfully created all clients",
		__func__, algo->info->algo_name);

	/* loop over devices, and update based on client data */
	struct thermal_list_node* algo_devices = algo->devices;
	while (algo_devices) {
		/* if any client is enabled, enable the device */
		struct thermal_list_node* device_clients =
			algo_devices->elem.device->clients;
		while (device_clients) {
			if (device_clients->elem.client->disabled == THERMAL_ENABLED) {
				algo_devices->elem.device->disabled = THERMAL_ENABLED;
			}
			device_clients = device_clients->next;
		}
		algo_devices = algo_devices->next;
	}

	/* loop over sampgroups, setting up timer values */
	struct thermal_list_node* algo_sampgroups = algo->sampgroups;
	while (algo_sampgroups) {
		memset(&algo_sampgroups->elem.sampgroup->sig_evt, 0,
			sizeof(struct sigevent));
		algo_sampgroups->elem.sampgroup->sig_evt.sigev_notify = SIGEV_THREAD;
		algo_sampgroups->elem.sampgroup->sig_evt.sigev_notify_function =
			thermal_timer_expired;
		algo_sampgroups->elem.sampgroup->sig_evt.sigev_value.sival_ptr =
			algo_sampgroups->elem.sampgroup;

		THERMAL_ERRCHK((timer_create(CLOCK_MONOTONIC,
			&algo_sampgroups->elem.sampgroup->sig_evt,
			&algo_sampgroups->elem.sampgroup->timer_id)), return_err, err_code);

		memset(&algo_sampgroups->elem.sampgroup->timer_val, 0,
			sizeof(struct itimerspec));
		TIMER_SAMPLING_SET(algo_sampgroups->elem.sampgroup,
			algo_sampgroups->elem.sampgroup->sampling);

		algo_sampgroups = algo_sampgroups->next;
	}

	struct thermal_list_node* algo_clients = algo->clients;
	while (algo_clients) {
		dbgmsg("%s: algo %s, found client -- name: %s, device: %s,"
			" sampling group: %d ms, client disabled: %d, device disabled: %d,"
			" sampling group disabled: %d", __func__, algo->info->algo_name,
			algo_clients->elem.client->name,
			algo_clients->elem.client->device->name,
			algo_clients->elem.client->device->sampgroup->sampling,
			algo_clients->elem.client->disabled,
			algo_clients->elem.client->device->disabled,
			algo_clients->elem.client->device->sampgroup->disabled);

		algo_clients = algo_clients->next;
	}

	return THERMAL_ALGO_SUCCESS;

return_err:
	THERMAL_ERRMSG(err_code);

	return THERMAL_ALGO_FAILURE;
}

/* tear down and clean up an algo instance */
static void thermal_algo_destroy_instance(struct thermal_algo* algo)
{
	if (algo == NULL)
		return;

	/* even if destroy fails, try to clean up memory */
	if (algo->info->algo_destroy(algo))
		dbgmsg("%s: algorithm-specific cleanup failed. ERROR", __func__);

	struct thermal_list_node* algo_clients = algo->clients;
	struct thermal_list_node* prev_client;
	while (algo_clients != NULL) {
		sensors_manager_dereg_clnt(algo_clients->elem.client->sensor_clnt);
		free(algo_clients->elem.client);
		prev_client = algo_clients;
		algo_clients = algo_clients->next;
		free(prev_client);
	}

	struct thermal_list_node* algo_devices = algo->devices;
	struct thermal_list_node* prev_device;
	while (algo_devices != NULL) {
		devices_manager_dereg_clnt(algo_devices->elem.device->dev_clnt);
		free(algo_devices->elem.device->lvl_arr);
		free(algo_devices->elem.device->params);

		struct thermal_list_node* device_clients =
			algo_devices->elem.device->clients;
		while (device_clients != NULL) {
			prev_client = device_clients;
			device_clients = device_clients->next;
			free(prev_client);
		}

		free(algo_devices->elem.device);
		prev_device = algo_devices;
		algo_devices = algo_devices->next;
		free(prev_device);
	}

	struct thermal_list_node* algo_sampgroups = algo->sampgroups;
	struct thermal_list_node* prev_sampgroup;
	while (algo_sampgroups != NULL) {

		struct thermal_list_node* sampgroup_devices =
			algo_sampgroups->elem.sampgroup->devices;
		while (sampgroup_devices != NULL) {
			prev_device = sampgroup_devices;
			sampgroup_devices = sampgroup_devices->next;
			free(prev_device);
		}

		free(algo_sampgroups->elem.sampgroup);
		prev_sampgroup = algo_sampgroups;
		algo_sampgroups = algo_sampgroups->next;
		free(prev_sampgroup);
	}

	free(algo);
}

/* create an instance of an algorithm */
static struct thermal_algo* thermal_algo_create_instance(void)
{
	int err_code = 0;

	struct thermal_algo* instance =
		(struct thermal_algo*)malloc(sizeof(struct thermal_algo));
	THERMAL_ERRCHK((instance == NULL), return_err, err_code);

	THERMAL_ERRCHK((pthread_mutex_init(&instance->wait_mutex, NULL) != 0),
		handle_err, err_code);
	THERMAL_ERRCHK((pthread_cond_init(&instance->wait_cond, NULL) != 0),
		handle_err, err_code);

	instance->sampgroups             = NULL;
	instance->devices                = NULL;
	instance->clients                = NULL;

	instance->client_count           = 0;
	instance->device_count           = 0;
	instance->sampgroup_count        = 0;

	instance->init_device_mask       = 1;
	instance->init_sampgroup_mask    = 1;

	instance->ev_mask                = 0;
	instance->sampgroup_mask         = 0;
	instance->device_thresh_mask     = 0;

	return instance;

handle_err:
	free(instance);

return_err:
	THERMAL_ERRMSG(err_code);

	return NULL;
}

/* Mainloop for the algo monitoring. Waits for an handles events */
static enum thermal_algo_ret_t thermal_algo_monitor(struct thermal_algo* algo)
{
	uint32_t      local_ev_mask = 0;
	uint32_t      local_sampgroup_mask = 0;
	uint32_t      local_device_thresh_mask = 0;
	int err_code = 0;

	THERMAL_ERRCHK((algo == NULL), return_err, err_code);

	thermal_handle_thresh(THERMAL_DEVICE_INIT, ALL_DEVICES_SAMPGROUPS, algo);

	while (1) {
		dbgmsg("%s: algo %s, Wait for EV", __func__, algo->info->algo_name);
		/* get the lock */
		THERMAL_ERRCHK(
			(pthread_mutex_lock(&algo->wait_mutex) != 0), return_err, err_code);
		if (!algo->ev_mask) {
			/* wait for event broadcast */
			THERMAL_ERRCHK(
				(pthread_cond_wait(&algo->wait_cond, &algo->wait_mutex) != 0),
				handle_err_unlock, err_code);
		}

		/* Make local copy and clear. */
		local_ev_mask = algo->ev_mask;
		algo->ev_mask = 0;
		local_sampgroup_mask = algo->sampgroup_mask;
		algo->sampgroup_mask = 0;
		local_device_thresh_mask = algo->device_thresh_mask;
		algo->device_thresh_mask = 0;

		THERMAL_ERRCHK((pthread_mutex_unlock(&algo->wait_mutex) != 0),
			return_err, err_code);

		/* if we got a threshold notification */
		if (local_ev_mask & THRESH_EV) {
			dbgmsg("%s: algo %s, Thresh EVT", __func__, algo->info->algo_name);
			THERMAL_ERRCHK((thermal_handle_thresh(THERMAL_DEVICE_MITIGATION_ON,
				local_device_thresh_mask, algo) != THERMAL_ALGO_SUCCESS),
				return_err, err_code);
		}

		/* if we got a timer notification. Note: this MUST be handled
		 * after threshold notifications, since a threshold event will
		 * spark a timer event immediately after
		 */
		if (local_ev_mask & TIMER_EV) {
			dbgmsg("%s: algo %s, Timer EVT", __func__, algo->info->algo_name);
			THERMAL_ERRCHK((thermal_handle_timer(local_sampgroup_mask, algo)
				!= THERMAL_ALGO_SUCCESS), return_err, err_code);
		}

	}

handle_err_unlock:
	pthread_mutex_unlock(&algo->wait_mutex);

return_err:
	THERMAL_ERRMSG(err_code);
	return THERMAL_ALGO_FAILURE;
}

/* Mainloop for entire algo, should never return. Handles errors
 * by tearing down and attempting to restart algo
 */
static void* thermal_algo_run(void* data)
{
	struct thermal_algo* algo_instance = NULL;
	int err_code = 0;

	THERMAL_ERRCHK((data == NULL), return_err, err_code);

	struct thermal_start_data* start_data = (struct thermal_start_data*)data;

	/* This loop doesn't utilize the standard goto/error-code error handling
	 * mechanism that the rest of the code uses because it is intended
	 * to continue through errors. This is the top level
	 */
	while (1) {
		if (err_code > 1)
			msg("%s had error %d trying to start thermal algorithm",
				__func__, err_code);

		if (algo_instance != NULL)
			thermal_algo_destroy_instance(algo_instance);

		err_code = 2;
		algo_instance = thermal_algo_create_instance();
		if (algo_instance == NULL)
			continue;

		algo_instance->info = start_data->info;

		err_code++;
		if ( algo_instance->info->algo_init(algo_instance)
			!= THERMAL_ALGO_SUCCESS )
			continue;

		err_code++;
		if( thermal_algo_register_instance(algo_instance, start_data->setting)
			!= THERMAL_ALGO_SUCCESS )
			continue;

		err_code++;
		if( thermal_algo_monitor(algo_instance) != THERMAL_ALGO_SUCCESS )
			continue;
	}

return_err:
	THERMAL_ERRMSG(err_code);
	return NULL;
}

enum thermal_algo_ret_t thermal_algo_start(
	const struct thermal_algo_info* algo_info,
	const struct thermal_setting_t* setting)
{
	int err_code = 0;

	THERMAL_ERRCHK((setting == NULL || algo_info == NULL),
		return_err, err_code);

	struct thermal_start_data* start_data =
		(struct thermal_start_data*)malloc(sizeof(struct thermal_start_data));
	THERMAL_ERRCHK((start_data == NULL), return_err, err_code);

	start_data->info = algo_info;
	start_data->setting = setting;

	pthread_t* thread = (pthread_t*)malloc(sizeof(pthread_t));
	THERMAL_ERRCHK((thread == NULL), handle_err_start_data, err_code);

	THERMAL_ERRCHK(
		(pthread_create(thread, NULL, &thermal_algo_run,
		(void*)start_data) != 0), handle_err_thread, err_code);

	struct thermal_thread_info* thread_info = (struct thermal_thread_info*)
		malloc(sizeof(struct thermal_thread_info));
	THERMAL_ERRCHK((thread_info == NULL), handle_err_thread, err_code);

	thread_info->start_data = start_data;
	thread_info->thread = thread;

	struct thermal_thread_info_node* info_node =
		(struct thermal_thread_info_node*)malloc(
			sizeof(struct thermal_thread_info_node));
	THERMAL_ERRCHK((info_node == NULL), handle_err_full, err_code);

	info_node->info = thread_info;
	info_node->next = thermal_threads;
	thermal_threads = info_node;

	return THERMAL_ALGO_SUCCESS;

handle_err_full:
	free(thread_info);

handle_err_thread:
	free(thread);

handle_err_start_data:
	free(start_data);

return_err:
	THERMAL_ERRMSG(err_code);

	return THERMAL_ALGO_FAILURE;
}

/* this MUST be called before starting any thermal algo */
enum thermal_algo_ret_t thermal_algo_framework_init(void)
{
	thermal_threads = NULL;
	return THERMAL_ALGO_SUCCESS;
}

/* this MUST be called to clean up memory used by
 * thermal algo framework at end of life */
enum thermal_algo_ret_t thermal_algo_framework_cleanup(void)
{
	struct thermal_thread_info_node* current_thread = thermal_threads;
	struct thermal_thread_info_node* prev_thread;
	while (current_thread != NULL) {
		free(current_thread->info->thread);
		free(current_thread->info->start_data);
		prev_thread = current_thread;
		current_thread = current_thread->next;
		free(prev_thread);
	}

	return THERMAL_ALGO_SUCCESS;
}
