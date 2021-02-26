/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/

#ifndef __THERMAL_ALGORITHM_H__
#define __THERMAL_ALGORITHM_H__

#include "thermal_config.h"
#include "sensors_manager.h"
#include "devices_manager.h"
#include <signal.h>
#include <pthread.h>

enum thermal_algo_ret_t {
	THERMAL_ALGO_SUCCESS = 0,
	THERMAL_ALGO_FAILURE = 1
};

enum thermal_enabled_t {
	THERMAL_ENABLED = 0,
	THERMAL_DISABLED = 1
};

enum thermal_device_change_t {
	THERMAL_DEVICE_NO_CHANGE,
	THERMAL_DEVICE_CHANGE_FREQ
};

enum thermal_device_state_t {
	THERMAL_DEVICE_MITIGATION_OFF,
	THERMAL_DEVICE_MITIGATION_ON,
	THERMAL_DEVICE_INIT,
	THERMAL_DEVICE_STATE_UNKNOWN
};

enum thermal_next_freq_t {
	THERMAL_NEXT_HIGH_FREQ,
	THERMAL_NEXT_LOW_FREQ
};

/* For dealing with device's level array. Do not use
 * array directly, use these
 */
#define THERMAL_GET_NUM_LVLS(device) device->num_of_lvls
#define THERMAL_GET_MAX_FREQ(device) device->lvl_arr[0].lvl.value
#define THERMAL_GET_MIN_LVL(device) (device->num_of_lvls - 1)
#define THERMAL_GET_MIN_FREQ(device) \
	device->lvl_arr[THERMAL_GET_MIN_LVL(device)].lvl.value
#define THERMAL_GET_LVL_FREQ(device, level) device->lvl_arr[(level)].lvl.value
#define THERMAL_GET_CURR_FREQ(device) \
	THERMAL_GET_LVL_FREQ(device, device->curr_lvl)
#define THERMAL_GET_HIGHER_FREQ_FROM_CURR_LVL(device, offset) \
	THERMAL_GET_LVL_FREQ(device, \
	((((int)device->curr_lvl - (int)(offset)) < 0) ? \
		0 : ((int)device->curr_lvl - (int)(offset))))
#define THERMAL_GET_LOWER_FREQ_FROM_CURR_LVL(device, offset) \
	THERMAL_GET_LVL_FREQ(device, \
	((((int)device->curr_lvl + (int)(offset)) > (int)THERMAL_GET_MIN_LVL(device)) ? \
		(int)THERMAL_GET_MIN_LVL(device) : ((int)device->curr_lvl + (int)(offset))))

/* Use this to check for error statement, and if statement jump
 * to goto. Increment the count variable to eliminate need
 * for lots of debug messages
 */
#define THERMAL_ERRCHK(statement, label, err_count) \
	if (statement) goto label; (err_count++)
#define THERMAL_ERRMSG(err_code) \
	msg("%s finishing with error code %d", __func__, err_code)

struct thermal_algo_client;
struct thermal_algo_device;
struct thermal_algo_sampgroup;
struct thermal_list_node;
struct thermal_algo;

union thermal_list_elem
{
	struct thermal_algo_client*         client;
	struct thermal_algo_device*         device;
	struct thermal_algo_sampgroup*      sampgroup;
	int                                 val;
};

struct thermal_list_node
{
	union thermal_list_elem       elem;
	struct thermal_list_node*     next;
};

struct thermal_algo_client {
	const char*                   name;
	sensor_clnt_handle            sensor_clnt;
	enum thermal_enabled_t        disabled;
	/* all temps are in mC, unless otherwise specified */
	int                           active_set_point;
	int                           active_set_point_clr;
	int                           temp;
	const struct setting_info*    setting;
	const char*                   devicename;
	struct thermal_algo_device*   device;
	struct thermal_algo*          parent_algo;
};

struct thermal_algo_device_algo_params
{
	enum thermal_device_state_t         mitigation;
	enum thermal_device_change_t        change_freq;
	int                                 new_freq;
};

struct thermal_algo_device
{
	device_clnt_handle                  dev_clnt;
	struct device_lvl_info*             lvl_arr;
	uint32_t                            num_of_lvls;
	const char*                         name;
	struct thermal_list_node*           clients;
	int                                 freq_applied;
	uint32_t                            mask;
	uint32_t                            curr_lvl;
	enum thermal_enabled_t              disabled;
	struct thermal_algo_sampgroup*      sampgroup;
	/* each algo will put its own data here */
	void*                               data;
	struct thermal_algo*                parent_algo;
	uint32_t                            sampling_period; /* milliseconds */
	/* this field is used to pass data back and forth between
	 * algo and framework.
	 */
	struct thermal_algo_device_algo_params*  params;

};

struct thermal_algo_sampgroup
{
	uint32_t                      sampling; /* milliseconds */
	enum thermal_enabled_t        disabled;
	struct sigevent               sig_evt;
	timer_t                       timer_id;
	struct timespec               timer_start;
	struct itimerspec             timer_val;
	struct thermal_list_node*     devices;
	uint32_t                      timer_mask;
	int                           restart;
	struct thermal_algo*          parent_algo;
};

struct thermal_algo_info {
	enum thermal_algo_ret_t            (*init_client)(
		struct thermal_algo_client*,
		const struct setting_info*);
	enum thermal_algo_ret_t            (*init_device)(
		struct thermal_algo_device*,
		const struct thermal_algo_client*);
	enum thermal_algo_ret_t            (*init_sampgroup)(
		struct thermal_algo_sampgroup*,
		const struct thermal_algo_client*);
	enum thermal_algo_ret_t            (*mitigate)(
		const struct thermal_algo_device*,
		const struct thermal_algo*);
	enum thermal_algo_ret_t            (*thresh_turn_on)(
		const struct thermal_algo_device*,
		const struct thermal_algo*);
	enum thermal_algo_ret_t            (*thresh_turn_off)(
		const struct thermal_algo_device*,
		const struct thermal_algo*);
	enum thermal_algo_ret_t            (*algo_init)(
		struct thermal_algo*);
	enum thermal_algo_ret_t            (*algo_destroy)(
		struct thermal_algo*);

	char*                              algo_name;
	int                                algo_type;
};

struct thermal_algo {

	const struct thermal_algo_info*  info;

	struct thermal_list_node*  clients;
	struct thermal_list_node*  devices;
	struct thermal_list_node*  sampgroups;

	int                              client_count;
	int                              device_count;
	int                              sampgroup_count;

	uint32_t                         init_device_mask;
	uint32_t                         init_sampgroup_mask;

	uint32_t                         ev_mask;
	uint32_t                         sampgroup_mask;
	uint32_t                         device_thresh_mask;

	pthread_mutex_t                  wait_mutex;
	pthread_cond_t                   wait_cond;
};

enum thermal_algo_ret_t thermal_algo_start(
	const struct thermal_algo_info* algo_info,
	const struct thermal_setting_t* setting);
enum thermal_algo_ret_t thermal_algo_framework_init(void);
enum thermal_algo_ret_t thermal_algo_framework_cleanup(void);

#endif /* __THERMAL_ALGORITHM_H__ */
