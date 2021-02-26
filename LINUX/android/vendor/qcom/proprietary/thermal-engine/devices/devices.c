/*===========================================================================

  Copyright (c) 2012-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <stringl.h>

#include "devices_manager_internal.h"
#include "devices_actions.h"
#include "devices.h"
#include "thermal.h"
#include "inc/thermal_ioctl_interface.h"

/* CPU Frequency Scaling Action */

#define FREQ_AVAIL_NODE "/cpufreq/scaling_available_frequencies"
#define FREQ_BOOST_NODE "/cpufreq/scaling_boost_frequencies"
#define POSSIBLE_CPUS   (CPU_SYSFS_DIR "/possible")
#define VOLTAGE_DELTA_MV 20
struct tmd_generic_dev_info_t {
	char *name;
	uint32_t num_of_lvls;
	int32_t min_lvl;
	device_action action;
	void *data;
	enum device_type dev_type;
};

static int wlan_action(struct devices_manager_dev *dev_mgr);
static int battery_action(struct devices_manager_dev *dev_mgr);
static int lcd_action(struct devices_manager_dev *dev_mgr);
static int hotplug_action(struct devices_manager_dev *dev_mgr);
static int ts_shutdown_action(struct devices_manager_dev *dev_mgr);
static int ts_report_action(struct devices_manager_dev *dev_mgr);
static int report_rule_action(struct devices_manager_dev *dev_mgr);
static int cpu_action(struct devices_manager_dev *dev_mgr);
static int cpu_all_action(struct devices_manager_dev *dev_mgr);
static int cluster_action(struct devices_manager_dev *dev_mgr);
static int gpu_action(struct devices_manager_dev *dev_mgr);
static int modem_action(struct devices_manager_dev *dev_mgr);
static int fusion_modem_action(struct devices_manager_dev *dev_mgr);
static int cpr_band_action(struct devices_manager_dev *dev_mgr);
static int vdd_restriction_action(struct devices_manager_dev *dev_mgr);
static int kernel_mitigation_action(struct devices_manager_dev *dev_mgr);
static int camera_action(struct devices_manager_dev *dev_mgr);
static int camcorder_action(struct devices_manager_dev *dev_mgr);
static int vdd_dig_automode_diable_action(struct devices_manager_dev *dev_mgr);
static int optimum_current_request_action(struct devices_manager_dev *dev_mgr);
static int mdp_action(struct devices_manager_dev *dev_mgr);
static int venus_action(struct devices_manager_dev *dev_mgr);
static int modem_cx_limit_action(struct devices_manager_dev *dev_mgr);
static int profile_switch_action(struct devices_manager_dev *dev_mgr);
static int modem_proc_action(struct devices_manager_dev *dev_mgr);
static int modem_proc_current_action(struct devices_manager_dev *dev_mgr);
static int modem_bw_action(struct devices_manager_dev *dev_mgr);
static int modem_skin_action(struct devices_manager_dev *dev_mgr);
static int hvx_hw_action(struct devices_manager_dev *dev_mgr);
static int hvx_sw_action(struct devices_manager_dev *dev_mgr);
static int cpu_current_op(struct devices_manager_dev *dev_mgr, int *lvl);
static int cpu_all_current_op(struct devices_manager_dev *dev_mgr, int *lvl);
static int cluster_current_op(struct devices_manager_dev *dev_mgr, int *lvl);
static int gpu_current_op(struct devices_manager_dev *dev_mgr, int *lvl);
static void generic_dev_release(struct devices_manager_dev *dev_mgr);
static int generic_dev_add(struct tmd_generic_dev_info_t *gen_info);

static device_clnt_handle cpu_clnt[MAX_CPUS];
static device_clnt_handle hotplug_dev_clnt[MAX_CPUS];

static struct tmd_generic_dev_info_t gen_dev_list[] = {
	{
		.name = "wlan",
		.num_of_lvls = MAX_WLAN_MITIGATION_LEVEL + 1,
		.action = wlan_action,
	},
	{
		.name = "profile_switch",
		.num_of_lvls = MAX_PROFILE_LEVEL + 1,
		.action = profile_switch_action,
		.min_lvl = PERF_DEFAULT_PROFILE,
	},
	{
		.name = BATTERY_DEV_NAME,
		.num_of_lvls = MAX_BATTERY_MITIGATION_LEVEL + 1,
		.action = battery_action,
	},
	{
		.name = LCD_DEV_NAME,
		.num_of_lvls = INT32_MAX,
		.action = lcd_action,
		.dev_type = DEVICE_OP_VALUE_TYPE,
	},
	{
		.name = "shutdown",
		.num_of_lvls = UINT32_MAX,
		.action = ts_shutdown_action,
		.dev_type = DEVICE_DIRECT_ACTION_TYPE,
	},
	{
		.name = "none",
		.dev_type = DEVICE_NONE_TYPE,
	},
	{
		.name = "report",
		.dev_type = DEVICE_DIRECT_ACTION_TYPE,
		.action = ts_report_action,
	},
	{
		.name = "report_rule",
		.dev_type = DEVICE_DIRECT_ACTION_TYPE,
		.action = report_rule_action,
	},
	{
		.name = MODEM_DEV_NAME,
		.num_of_lvls = 4,
		.action = modem_action,
	},
	{
		.name = FUSION_DEV_NAME,
		.num_of_lvls = 4,
		.action = fusion_modem_action,
	},
	{
		.name = VDD_RESTRICT_DEV_NAME,
		.num_of_lvls = 2,
		.action = vdd_restriction_action,
	},
	{
		.name = "kernel",
		.num_of_lvls = 2,
		.action = kernel_mitigation_action,
	},
	{
		.name = "camera",
		.num_of_lvls = MAX_CAMERA_MITIGATION_LEVEL + 1,
		.action = camera_action,
	},
	{
		.name = "camcorder",
		.num_of_lvls = MAX_CAMCORDER_MITIGATION_LEVEL + 1,
		.action = camcorder_action,
	},
	{
		.name = "vdd_dig_swmode",
		.num_of_lvls = 2,
		.action = vdd_dig_automode_diable_action,
	},
	{
		.name = "opt_curr_req",
		.num_of_lvls = 2,
		.action = optimum_current_request_action,
	},
	{
		.name = "mdp",
		.num_of_lvls = MAX_MDP_MITIGATION_LEVEL + 1,
		.action = mdp_action,
	},
	{
		.name = "venus",
		.num_of_lvls = MAX_VENUS_MITIGATION_LEVEL + 1,
		.action = venus_action,
	},
	{
		.name = MODEM_CX_DEV_NAME,
		.num_of_lvls = 4,
		.action = modem_cx_limit_action,
	},
	{
		.name = MODEM_PROC_DEV_NAME,
		.num_of_lvls = 4,
		.action = modem_proc_action,
	},
	{
		.name = MODEM_PROC_CURRENT_DEV_NAME,
		.num_of_lvls = 4,
		.action = modem_proc_current_action,
	},
	{
		.name = CPR_BAND_DEV_NAME,
		.num_of_lvls = MAX_CPR_TEMPERATURE_BAND,
		.action = cpr_band_action,
	},
	{
		.name = MODEM_BW_DEV_NAME,
		.num_of_lvls = 3,
		.action = modem_bw_action,
	},
	{
		.name = MODEM_SKIN_DEV_NAME,
		.num_of_lvls = 4,
		.action = modem_skin_action,
	},
	{
		.name = HVX_HW_DEV_NAME,
		.num_of_lvls = MAX_HVX_HW_MITIGATION_LEVEL + 1,
		.action = hvx_hw_action,
	},
	{
		.name = HVX_SW_DEV_NAME,
		.num_of_lvls = MAX_HVX_SW_MITIGATION_LEVEL + 1,
		.action = hvx_sw_action,
	},

};

static struct tmd_generic_dev_info_t hotplug_dev_list[] = {
	{
		.name = "hotplug_0",
		.num_of_lvls = 2,
		.action = hotplug_action,
		.data   = (void *)0,
	},
	{
		.name = "hotplug_1",
		.num_of_lvls = 2,
		.action = hotplug_action,
		.data   = (void *)1,
	},
	{
		.name = "hotplug_2",
		.num_of_lvls = 2,
		.action = hotplug_action,
		.data   = (void *)2,
	},
	{
		.name = "hotplug_3",
		.num_of_lvls = 2,
		.action = hotplug_action,
		.data   = (void *)3,
	},
	{
		.name = "hotplug_4",
		.num_of_lvls = 2,
		.action = hotplug_action,
		.data   = (void *)4,
	},
	{
		.name = "hotplug_5",
		.num_of_lvls = 2,
		.action = hotplug_action,
		.data   = (void *)5,
	},
	{
		.name = "hotplug_6",
		.num_of_lvls = 2,
		.action = hotplug_action,
		.data   = (void *)6,
	},
	{
		.name = "hotplug_7",
		.num_of_lvls = 2,
		.action = hotplug_action,
		.data   = (void *)7,
	},
};

static void init_cooling_device( struct devices_manager_dev *dev)
{
	char cdev_name[DEVICES_MAX_PATH] = {0};
	char path[DEVICES_MAX_PATH] = {0};
	char freq_buf[DEVICES_MAX_PATH] = {0};
	int cpu_idx = 0;

	dev->dev_info.cooling_dev_id = -1;
	if (!strncmp(dev->dev_info.name, LCD_DEV_NAME,
				strlen(dev->dev_info.name))) {
		snprintf(cdev_name, sizeof(cdev_name),
				LCD_COOLING_DEV_NAME);
		dev->dev_info.cooling_dev_id = get_cdevn(cdev_name);
	} else if (!strncmp(dev->dev_info.name, BATTERY_DEV_NAME,
				strlen(dev->dev_info.name))) {
		snprintf(cdev_name, sizeof(cdev_name),
				BATTERY_COOLING_DEV_NAME);
		dev->dev_info.cooling_dev_id = get_cdevn(cdev_name);
	} else if (!strncmp(dev->dev_info.name, GPU_DEV_NAME,
				strlen(dev->dev_info.name))) {
		snprintf(cdev_name, sizeof(cdev_name),
				GPU_COOLING_DEV_NAME);
		dev->dev_info.cooling_dev_id = get_cdevn(cdev_name);
	} else if (!strncmp(dev->dev_info.name, "hotplug",
				strlen("hotplug"))) {
		if (sscanf(dev->dev_info.name, "hotplug_%d", &cpu_idx) <= 0)
			return;
		hotplug_dev_clnt[cpu_idx] = NULL;
		snprintf(cdev_name, DEVICES_MAX_PATH,
				CPU_COOLING_DEV_NAME, cpu_idx);
		dev->dev_info.cooling_dev_id = get_cdevn(cdev_name);
		/* No cooling device. so use KTM to mitigate. */
		if (dev->dev_info.cooling_dev_id == -1)
			return;
		thermalmsg(LOG_LVL_DBG,
			(LOG_LOGCAT | LOG_LOCAL_SOCKET | LOG_TRACE),
			"%s: found in cdev id:%d\n", dev->dev_info.name,
			dev->dev_info.cooling_dev_id);
		snprintf(cdev_name, DEVICES_MAX_PATH, "cpu%d", cpu_idx);
		hotplug_dev_clnt[cpu_idx] =
				devices_manager_reg_clnt(cdev_name);
		/*
		 * If there is cpu device, vote for hotplug using that
		 * interface, so that it will be aggregated with other
		 * frequency request
		 */
		if (hotplug_dev_clnt[cpu_idx])
			thermalmsg(LOG_LVL_DBG,
				(LOG_LOGCAT | LOG_LOCAL_SOCKET | LOG_TRACE),
				"%s: registered for %s device.\n",
				dev->dev_info.name, cdev_name);
	}
	if (dev->dev_info.cooling_dev_id != -1) {
		/* Clear any previous mitigation on cooling device*/
		snprintf(path, DEVICES_MAX_PATH, CDEV_CUR_STATE,
				dev->dev_info.cooling_dev_id);
		snprintf(freq_buf, DEVICES_MAX_PATH, "0");
		write_to_file(path, freq_buf, strlen(freq_buf));
	}
}

static void tmd_release_cluster_dev(struct devices_manager_dev *dev_mgr)
{
	free(dev_mgr->lvl_info);
	free(dev_mgr);
}

#ifdef ENABLE_CPU_VOLTAGE_DEV
static struct tmd_cpu_voltage_dev_table_t cpu_voltage_freq_table;

static void tmd_release_cpu_voltage_dev(struct devices_manager_dev *dev_mgr)
{
	free(dev_mgr->lvl_info);
	free(dev_mgr->data);
	free(dev_mgr);
}


void print_voltage_device_info(void)
{
	uint32_t arr_idx = 0;
	uint32_t cluster_id = 0;
	uint32_t num_clusters = cpu_voltage_freq_table.num_clusters;

	if (num_clusters > MAX_NUM_CLUSTERS ||
		num_clusters == 0 ||
		cpu_voltage_freq_table.length == 0) {
		dbgmsg("Error printing voltage device info \n");
		return;
	}

	printf("\n# CPU voltage table:\n");
	for (cluster_id = 0; cluster_id < num_clusters; cluster_id++) {
		struct devices_manager_dev *dev_mgr =
			cpu_voltage_freq_table.dev_clnt[cluster_id]->dev_mgr;
		for (arr_idx = 0; arr_idx < cpu_voltage_freq_table.length;
			arr_idx++) {
			printf("# %s[%d] voltage = %d mV freq = %d kHz\n",
				dev_mgr->dev_info.name, arr_idx,
				cpu_voltage_freq_table.voltage_table[cluster_id][arr_idx],
				cpu_voltage_freq_table.cluster_freq[cluster_id][arr_idx]);
		}
	}
}

static int cpu_voltage_action(struct devices_manager_dev *dev_mgr)
{
	return cpu_voltage_request(dev_mgr->data,
		(uint32_t)dev_mgr->active_req.value);
}


static int tmd_add_cpu_voltage_dev_data(void)
{
	int ret_val = 0;
	struct devices_manager_dev *dev_mgr = NULL;
	uint32_t arr_idx_voltage = 0;
	uint32_t arr_idx = 0;
	uint32_t cluster_id = 0;
	uint32_t table_len = 0;
	uint32_t num_cluster = 0;
	uint32_t write_idx = 0;
	uint32_t idx1 = 0;
	struct device_info dev_info;
	uint32_t table_max_cluster_num = 0;
	uint32_t *arr_cpufreq_final[MAX_NUM_CLUSTERS] = {NULL};
	uint32_t *arr_voltage_final[MAX_NUM_CLUSTERS] = {NULL};
	char name[DEVICES_MAX_NAME_LEN];
	uint32_t num_voltages[MAX_NUM_CLUSTERS] = {0};
	uint32_t num_freqs[MAX_NUM_CLUSTERS] = {0};
	uint32_t *arr_voltage[MAX_NUM_CLUSTERS] = {NULL};
	uint32_t *arr_cpufreq[MAX_NUM_CLUSTERS] = {NULL};

	dev_mgr = malloc(sizeof(struct devices_manager_dev));
	if (dev_mgr == NULL) {
		msg("%s: malloc failed.\n", __func__);
		ret_val = -(ENOMEM);
		goto handle_error;
	}
	memset(dev_mgr, 0, sizeof(struct devices_manager_dev));

	num_cluster = (uint32_t)get_num_clusters();
	if (num_cluster <= 0) {
		info("%s: No clusters found\n", __func__);
		ret_val = -(EFAULT);
		goto handle_error;
	}
	/* Register Device manager clients for CPU cluster control */
	for (cluster_id = 0; cluster_id < num_cluster; cluster_id++) {
		snprintf(name, DEVICES_MAX_NAME_LEN, "cluster%d",
			cluster_id);
		cpu_voltage_freq_table.dev_clnt[cluster_id] =
			devices_manager_reg_clnt(name);
		if (cpu_voltage_freq_table.dev_clnt[cluster_id] == NULL) {
			msg("%s: Device clnt create fail %s\n", __func__,
				name);
			ret_val = -(EFAULT);
			goto handle_error;
		}

		ret_val = devices_manager_get_info(name, &dev_info);
		if (ret_val) {
			msg("%s: Can not grab dev info %s\n", __func__,
				name);
			goto handle_error;
		}
	}
	cpu_voltage_freq_table.num_clusters = num_cluster;
	for (cluster_id = 0; cluster_id < num_cluster; cluster_id++) {
		if (thermal_ioctl_get_cluster_voltage_plan(
			(int)cluster_id, NULL,
			&num_voltages[cluster_id])) {
			msg("%s: error reading cluster[%d] voltage plan len\n",
				__func__, cluster_id);
			ret_val = -(EFAULT);
			goto handle_error;
		}
		dbgmsg("Cluster id %d voltage plan len = %u\n",
			cluster_id, num_voltages[cluster_id]);
		if (thermal_ioctl_get_cluster_freq_plan(
			(int)cluster_id, arr_cpufreq[cluster_id],
			&num_freqs[cluster_id])) {
			msg("%s: error reading cluster[%d] freq plan len\n",
				__func__, cluster_id);
			ret_val = -(EFAULT);
			goto handle_error;
		}
		if (num_voltages[cluster_id] > 0 &&
			num_freqs[cluster_id] == num_voltages[cluster_id]) {
			arr_voltage[cluster_id] = malloc(sizeof(uint32_t)
				* num_voltages[cluster_id]);
			if (arr_voltage[cluster_id] == NULL) {
				msg("%s: malloc failed.\n", __func__);
				ret_val = -(ENOMEM);
				goto handle_error;
			}
			if (thermal_ioctl_get_cluster_voltage_plan(
				(int)cluster_id, arr_voltage[cluster_id],
				&num_voltages[cluster_id])) {
				msg("%s: error in get cluster[%d] voltage\n",
					__func__, cluster_id);
				ret_val = -(EFAULT);
				goto handle_error;
			}
			arr_cpufreq[cluster_id] = malloc(sizeof(unsigned int)
				* num_freqs[cluster_id]);
			if (arr_cpufreq[cluster_id] == NULL) {
				msg("%s: malloc failed.\n", __func__);
				ret_val = -(ENOMEM);
				goto handle_error;
			}
			if (thermal_ioctl_get_cluster_freq_plan(
				(int)cluster_id, arr_cpufreq[cluster_id],
				&num_freqs[cluster_id])) {
				msg("%s: error in get cluster[%d] freq\n",
					__func__, cluster_id);
				ret_val = -(EFAULT);
				goto handle_error;
			}
		} else {
			msg("%s: Non matching levels. voltage:%d frequency:%d\n"
				, __func__, num_voltages[cluster_id],
				num_freqs[cluster_id]);
			ret_val = -(EFAULT);
			goto handle_error;
		}
		/* Sort voltage in descending order */
		sort_int_arr((int *)arr_voltage[cluster_id],
			num_voltages[cluster_id], 0);
		/* Sort frequency in descending order */
		sort_int_arr((int *)arr_cpufreq[cluster_id],
			num_freqs[cluster_id], 0);

		/* Find cluster with the highest number of entries */
		if (num_voltages[cluster_id] > table_len) {
			table_max_cluster_num = cluster_id;
			table_len = num_voltages[cluster_id];
		}
	}

	dbgmsg("Initial table before sorting by voltage\n");
	for (cluster_id = 0; cluster_id < num_cluster; cluster_id++) {
		arr_voltage_final[cluster_id] = malloc(sizeof(uint32_t)
			* table_len);
		if (arr_voltage_final[cluster_id] == NULL) {
			msg("%s: malloc failed.\n", __func__);
			ret_val = -(ENOMEM);
			goto handle_error;
		}
		arr_cpufreq_final[cluster_id] = malloc(sizeof(uint32_t)
			* table_len);
		if (arr_cpufreq_final[cluster_id] == NULL) {
			msg("%s: malloc failed.\n", __func__);
			ret_val = -(ENOMEM);
			goto handle_error;
		}
		for (arr_idx_voltage = 0; arr_idx_voltage <
			num_freqs[cluster_id]; arr_idx_voltage++) {
			dbgmsg("%s: %s cluster%d[%d] voltage =%d freq %d", __func__,
				   dev_mgr->dev_info.name, cluster_id, arr_idx_voltage,
				   arr_voltage[cluster_id][arr_idx_voltage],
				   arr_cpufreq[cluster_id][arr_idx_voltage]);
			if (cluster_id == table_max_cluster_num) {
				/* Keep final max voltage entry cluster and build other
				 * tables from it*/
				arr_voltage_final[cluster_id][arr_idx_voltage] =
					arr_voltage[cluster_id][arr_idx_voltage];
				arr_cpufreq_final[cluster_id][arr_idx_voltage] =
					arr_cpufreq[cluster_id][arr_idx_voltage];
			}
			continue;
		}
	}

	cpu_voltage_freq_table.table_max_cluster_num = table_max_cluster_num;
	for (cluster_id = 0; cluster_id < num_cluster; cluster_id++) {
		if (cluster_id == table_max_cluster_num)
			continue;

		/* Keep highest freq voltage pair */
		write_idx = 0;
		arr_voltage_final[cluster_id][write_idx] =
					arr_voltage[cluster_id][0];
		arr_cpufreq_final[cluster_id][write_idx] =
					arr_cpufreq[cluster_id][0];
		write_idx++;
		for (arr_idx_voltage = 1; arr_idx_voltage < table_len;
			arr_idx_voltage++) {
			int match_found = 0;

			for (idx1 = 0; idx1 < num_voltages[cluster_id];
				idx1++) {
				int voltage_delta =
					(int)arr_voltage_final[table_max_cluster_num]
					[arr_idx_voltage] - (int)arr_voltage
					[cluster_id][idx1];

				if (voltage_delta < 0 ) {
					/* Keep looking for a match */
					continue;
				} else if (voltage_delta > VOLTAGE_DELTA_MV) {
					/* Use the previous element, delta too large */
					if (idx1 > 0)
						idx1 -= 1;
				}
				arr_voltage_final[cluster_id][write_idx] =
						arr_voltage[cluster_id][idx1];
				arr_cpufreq_final[cluster_id][write_idx] =
						arr_cpufreq[cluster_id][idx1];
				match_found = 1;
				write_idx++;
				break;
			}
			if (match_found == 0) {
				/* Best to have something in place */
				arr_voltage_final[cluster_id][write_idx] =
					arr_voltage[cluster_id][num_voltages[cluster_id]-1];
				arr_cpufreq_final[cluster_id][write_idx] =
					arr_cpufreq[cluster_id][num_voltages[cluster_id]-1];
				write_idx++;
			}
		}
	}

	for (cluster_id = 0; cluster_id < num_cluster; cluster_id++) {
		for (arr_idx_voltage = 0; arr_idx_voltage <
				table_len; arr_idx_voltage++) {
			dbgmsg("%s: %s Sorted table cluster%d[%d] voltage =%d freq %d\n",
				__func__, dev_mgr->dev_info.name, cluster_id, arr_idx_voltage,
				arr_voltage_final[cluster_id][arr_idx_voltage],
				arr_cpufreq_final[cluster_id][arr_idx_voltage]);
		}
	}

	for (cluster_id = 0; cluster_id < num_cluster; cluster_id++) {
		cpu_voltage_freq_table.cluster_freq[cluster_id] =
			arr_cpufreq_final[cluster_id];
		cpu_voltage_freq_table.voltage_table[cluster_id] =
			arr_voltage_final[cluster_id];
		cpu_voltage_freq_table.actual_freq_table[cluster_id] =
			arr_cpufreq[cluster_id];
		cpu_voltage_freq_table.actual_voltage_table[cluster_id] =
			arr_voltage[cluster_id];
		cpu_voltage_freq_table.actual_table_length[cluster_id] =
			num_voltages[cluster_id];
	}
	cpu_voltage_freq_table.length = table_len;

	dev_mgr->lvl_info =
		malloc(sizeof(struct device_lvl_info) * table_len);
	if (dev_mgr->lvl_info == NULL) {
		msg("%s: malloc failed.\n", __func__);
		ret_val = -(ENOMEM);
		goto handle_error;
	}

	snprintf(dev_mgr->dev_info.name, DEVICES_MAX_NAME_LEN,
		"cpu_voltage");
	dev_mgr->data   = (void *)&cpu_voltage_freq_table;
	dev_mgr->action = cpu_voltage_action;
	dev_mgr->current_op = NULL;

	memset(dev_mgr->lvl_info, 0, sizeof(struct device_lvl_info) * table_len);

	for (arr_idx = 0; arr_idx < table_len; arr_idx++) {
		dev_mgr->lvl_info[arr_idx].lvl.value =
			(int)arr_voltage_final[table_max_cluster_num][arr_idx];
		dev_mgr->lvl_info[arr_idx].perf_lvl =
			(int)arr_cpufreq_final[table_max_cluster_num][arr_idx];
		dbgmsg("%s: %s lvl_info[%u]=%d perf_lvl[%u]=%d", __func__,
			dev_mgr->dev_info.name, arr_idx,
			(int)arr_voltage_final[table_max_cluster_num][arr_idx],
			arr_idx,
			(int)arr_cpufreq_final[table_max_cluster_num][arr_idx]);
	}

	dev_mgr->dev_info.num_of_levels = table_len;
	dev_mgr->dev_info.dev_type = DEVICE_OP_VALUE_TYPE;
	dev_mgr->dev_info.max_dev_op_value_valid = 1;
	dev_mgr->dev_info.max_dev_op_value = dev_mgr->lvl_info[0].lvl.value;
	dev_mgr->dev_info.min_dev_op_value_valid = 1;
	dev_mgr->dev_info.min_dev_op_value =
		dev_mgr->lvl_info[table_len - 1].lvl.value;
	dev_mgr->active_req.value = dev_mgr->dev_info.max_dev_op_value;
	dev_mgr->release = tmd_release_cpu_voltage_dev;
	dev_mgr->dev_info.device_units = UNITS_VOLTAGE;
	strlcpy(dev_mgr->dev_info.device_units_name,
		UNIT_NAME_VOLTS, MAX_UNIT_NAME_SIZE);
	if (devices_manager_add_dev(dev_mgr) != 0) {
		msg("%s: Can't add device\n", __func__);
		ret_val = -(EFAULT);
		goto handle_error;
	}

handle_error:
	if (ret_val < 0) {
		/* Error clean up */
		if (dev_mgr) {
			if (dev_mgr->lvl_info)
				free(dev_mgr->lvl_info);
			free(dev_mgr);
		}

		for (arr_idx = 0; arr_idx < MAX_NUM_CLUSTERS; arr_idx++) {
			if (arr_voltage[arr_idx] != NULL)
				free(arr_voltage[arr_idx]);

			if (arr_voltage_final[arr_idx] != NULL)
				free(arr_voltage_final[arr_idx]);

			if (arr_cpufreq[arr_idx] != NULL)
				free(arr_cpufreq[arr_idx]);

			if (arr_cpufreq_final[arr_idx] != NULL)
				free(arr_cpufreq_final[arr_idx]);

			if (cpu_voltage_freq_table.dev_clnt[arr_idx] != NULL)
				devices_manager_dereg_clnt(
					cpu_voltage_freq_table.dev_clnt[arr_idx]);
		}
	}
	return ret_val;
}

int tmd_init_voltage_devs(void)
{
	int ret_val = 0;

	info("adding voltage device\n");
	/* Add  voltage device for cpu single rail mitigation */
	if (tmd_add_cpu_voltage_dev_data() != 0) {
		msg("%s: Error adding cpu voltage device\n", __func__);
		ret_val = -(EFAULT);
	}

	return ret_val;
}
#endif /* ENABLE_CPU_VOLTAGE_DEV */

static int tmd_add_clusterfreq_dev_data(int cluster_id)
{
	int ret_val = 0;
	uint32_t num_freqs = 0;
	struct devices_manager_dev *dev_mgr = NULL;
	unsigned int *arr = NULL;
	uint32_t arr_idx;

	dev_mgr = malloc(sizeof(struct devices_manager_dev));
	if (dev_mgr == NULL) {
		msg("%s: malloc failed.\n", __func__);
		ret_val = -(ENOMEM);
		goto handle_error;
	}
	memset(dev_mgr, 0, sizeof(struct devices_manager_dev));

	if (thermal_ioctl_get_cluster_freq_plan(
		cluster_id, arr, &num_freqs)) {
		msg("%s: error reading cluster[%d] freq plan len\n",
			__func__, cluster_id);
		ret_val = -(EFAULT);
		goto handle_error;
	}

	if (num_freqs > 0) {
		arr = malloc(sizeof(unsigned int) * num_freqs);
		if (arr == NULL) {
			msg("%s: malloc failed.\n", __func__);
			ret_val = -(ENOMEM);
			goto handle_error;
		}
		if (thermal_ioctl_get_cluster_freq_plan(
			cluster_id, arr, &num_freqs)) {
			msg("%s: error in get cluster[%d] freq\n",
				__func__, cluster_id);
			ret_val = -(EFAULT);
			goto handle_error;
		}
	} else {
		msg("%s: Invalid number for freqs:%d\n", __func__, num_freqs);
		ret_val = -(EFAULT);
		goto handle_error;
	}

	/* Sort in descending order */
	sort_int_arr((int *)arr, num_freqs, 0);

	dev_mgr->lvl_info =
		malloc(sizeof(struct device_lvl_info) * num_freqs);
	if (dev_mgr->lvl_info == NULL) {
		msg("%s: malloc failed.\n", __func__);
		ret_val = -(ENOMEM);
		goto handle_error;
	}

	snprintf(dev_mgr->dev_info.name, DEVICES_MAX_NAME_LEN, "cluster%d",
		 cluster_id);
	dev_mgr->data   = (void *)(intptr_t)cluster_id;
	dev_mgr->action = cluster_action;
	dev_mgr->current_op = cluster_current_op;

	memset(dev_mgr->lvl_info, 0,
	       sizeof(struct device_lvl_info) * num_freqs);
	for (arr_idx = 0; arr_idx < num_freqs; arr_idx++) {
		dbgmsg("%s: %s lvl_info[%d]=%d", __func__,
		       dev_mgr->dev_info.name, arr_idx, arr[arr_idx]);
		dev_mgr->lvl_info[arr_idx].lvl.value = (int)arr[arr_idx];
		dev_mgr->lvl_info[arr_idx].perf_lvl = (int)arr[arr_idx];
	}

	dev_mgr->dev_info.num_of_levels = num_freqs;

	dev_mgr->dev_info.dev_type = DEVICE_OP_VALUE_TYPE;

	dev_mgr->dev_info.max_dev_op_value_valid = 1;
	dev_mgr->dev_info.max_dev_op_value = dev_mgr->lvl_info[0].lvl.value;
	dev_mgr->dev_info.min_dev_op_value_valid = 1;
	dev_mgr->dev_info.min_dev_op_value =
		dev_mgr->lvl_info[num_freqs - 1].lvl.value;
	dev_mgr->active_req.value = dev_mgr->dev_info.max_dev_op_value;
	dev_mgr->release = tmd_release_cluster_dev;
	strlcpy(dev_mgr->dev_info.device_units_name,
		UNIT_NAME_FREQ_KHZ, MAX_UNIT_NAME_SIZE);
	if (devices_manager_add_dev(dev_mgr) != 0) {
		msg("%s: Can't add device\n", __func__);
		ret_val = -(EFAULT);
		goto handle_error;
	}

handle_error:
	if (ret_val < 0) {
		/* Error clean up */
		if (dev_mgr) {
			if (dev_mgr->lvl_info)
				free(dev_mgr->lvl_info);
			free(dev_mgr);
		}
		if (arr)
			free(arr);
	}
	return ret_val;
}

static void tmd_release_cpufreq_dev(struct devices_manager_dev *dev_mgr)
{
	free(dev_mgr->lvl_info);
	free(dev_mgr);
}

static int tmd_add_cpufreq_dev_data(int cpu_idx)
{
	int ret_val = 0, idx = 0;
	uint32_t num_freqs = 0, boost_freqs = 0;
	struct devices_manager_dev *dev_mgr = NULL;
	char path[DEVICES_MAX_PATH] = {0};
	char boost_path[DEVICES_MAX_PATH] = {0};
	char cdev_name[DEVICES_MAX_PATH] = {0};
	int arr[50], boost_arr[50];
	uint32_t arr_idx;

	dev_mgr = malloc(sizeof(struct devices_manager_dev));
	if (dev_mgr == NULL) {
		msg("%s: malloc failed.\n", __func__);
		ret_val = -(ENOMEM);
		goto handle_error;
	}
	memset(dev_mgr, 0, sizeof(struct devices_manager_dev));

	memset(arr, 0, sizeof(arr));
	memset(boost_arr, 0, sizeof(boost_arr));

	if (cpu_idx == INT32_MAX) {
		/* Use CPU0 frequencies because they are equivalent on all CPU's
		   and sysfs nodes required for generating information are present
		   only in CPU's that are online for synchronous cluster. It is possible
		   that CPU1+ maybe offline when this info set is being generated. */
		snprintf(path, sizeof(path), CPU_SYSFS(FREQ_AVAIL_NODE), 0);
		snprintf(boost_path, sizeof(boost_path),
				CPU_SYSFS(FREQ_BOOST_NODE), 0);
	} else {
		snprintf(path, sizeof(path), CPU_SYSFS(FREQ_AVAIL_NODE), cpu_idx);
		snprintf(boost_path, sizeof(boost_path),
				CPU_SYSFS(FREQ_BOOST_NODE), cpu_idx);
	}
	num_freqs = read_int_list_from_file(path, arr, ARRAY_SIZE(arr));
	if (num_freqs < 1) {
		msg("%s: Invalid number for freqs\n", __func__);
		ret_val = -(EFAULT);
		goto handle_error;
	}
	boost_freqs = read_int_list_from_file(boost_path, boost_arr,
						ARRAY_SIZE(boost_arr));
	if (boost_freqs < 1) {
		msg("%s: Invalid or no boost freqs\n", __func__);
	} else {
		for (idx = 0; idx < boost_freqs
			&& (num_freqs + idx) < ARRAY_SIZE(arr); idx++) {
			arr[num_freqs + idx] = boost_arr[idx];
		}
		num_freqs += boost_freqs;
		if (num_freqs > ARRAY_SIZE(arr))
			num_freqs = ARRAY_SIZE(arr);
	}

	/* Sort in descending order */
	sort_int_arr(arr, num_freqs, 0);
	snprintf(cdev_name, sizeof(cdev_name), CPU_COOLING_DEV_NAME, cpu_idx);
	dev_mgr->dev_info.cooling_dev_id = get_cdevn(cdev_name);
	if (dev_mgr->dev_info.cooling_dev_id != -1) {
		/* Hotplug is integrated as cooling device level. */
		arr[num_freqs] = 0;
		num_freqs++;
	}

	dev_mgr->lvl_info =
		malloc(sizeof(struct device_lvl_info) * num_freqs);
	if (dev_mgr->lvl_info == NULL) {
		msg("%s: malloc failed.\n", __func__);
		ret_val = -(ENOMEM);
		goto handle_error;
	}

	if (cpu_idx == INT32_MAX) {
		snprintf(dev_mgr->dev_info.name, DEVICES_MAX_NAME_LEN, "cpu");
		dev_mgr->action = cpu_all_action;
		dev_mgr->current_op = cpu_all_current_op;
	} else {
		snprintf(dev_mgr->dev_info.name, DEVICES_MAX_NAME_LEN, "cpu%d",
			 cpu_idx);
		dev_mgr->data   = (void *)(intptr_t)cpu_idx;
		dev_mgr->action = cpu_action;
		dev_mgr->current_op = cpu_current_op;
	}

	memset(dev_mgr->lvl_info, 0,
	       sizeof(struct device_lvl_info) * num_freqs);
	for (arr_idx = 0; arr_idx < num_freqs; arr_idx++) {
		dbgmsg("%s: %s lvl_info[%d]=%d", __func__,
		       dev_mgr->dev_info.name, arr_idx, arr[arr_idx]);
		dev_mgr->lvl_info[arr_idx].lvl.value = arr[arr_idx];
		dev_mgr->lvl_info[arr_idx].perf_lvl = arr[arr_idx];
	}

	dev_mgr->dev_info.num_of_levels = num_freqs;

	dev_mgr->dev_info.dev_type = DEVICE_OP_VALUE_TYPE;

	dev_mgr->dev_info.max_dev_op_value_valid = 1;
	dev_mgr->dev_info.max_dev_op_value = dev_mgr->lvl_info[0].lvl.value;
	dev_mgr->dev_info.min_dev_op_value_valid = 1;
	dev_mgr->dev_info.min_dev_op_value =
		dev_mgr->lvl_info[num_freqs - 1].lvl.value;
	dev_mgr->active_req.value = dev_mgr->dev_info.max_dev_op_value;
	dev_mgr->release = tmd_release_cpufreq_dev;
	strlcpy(dev_mgr->dev_info.device_units_name,
		UNIT_NAME_FREQ_KHZ, MAX_UNIT_NAME_SIZE);
	if (devices_manager_add_dev(dev_mgr) != 0) {
		msg("%s: Can't add device\n", __func__);
		ret_val = -(EFAULT);
	}

handle_error:
	if (ret_val < 0) {
		/* Error clean up */
		if (dev_mgr) {
			if (dev_mgr->lvl_info)
				free(dev_mgr->lvl_info);
			free(dev_mgr);
		}
	}
	return ret_val;
}

int tmd_init_cluster_devs(void)
{
	int num_cluster = 0;
	int ret_val = 0;
	int idx = 0;
	char name[DEVICES_MAX_NAME_LEN];

	num_cluster = get_num_clusters();
	if (num_cluster <= 0) {
		info("%s: No clusters found\n", __func__);
		return ret_val;
	}

	/* Add  cluster TMD's */
	for (idx = 0; idx < num_cluster; idx++) {
		int cluster_val = get_cluster_id_num_from_index(idx);

		if(cluster_val == -1) {
			msg("%s: Error getting cluster number for index %d \n", __func__,
			    idx);
			return - (EFAULT);
		}
		if (tmd_add_clusterfreq_dev_data(cluster_val) != 0) {
			msg("%s: Error adding cluster%d Device\n", __func__,
			    cluster_val);
			ret_val = -(EFAULT);
			break;
		}

		snprintf(name, DEVICES_MAX_NAME_LEN, "cluster%d", cluster_val);
	}

	return ret_val;
}

int tmd_init_cpu_devs(void)
{
	int num_cpus = 0;
	int ret_val = 0;
	int idx = 0;
	char name[DEVICES_MAX_NAME_LEN];
	int cluster_id = -1;

	num_cpus = get_num_cpus();
	if (num_cpus <= 0) {
		msg("%s: Can't read number of CPUs\n", __func__);
		return -(EIO);
	}
#ifndef USE_THERMAL_FRAMEWORK
	/* CPU device is supported only on targets with just
	   one async cluster */
	if (get_num_clusters() <= 0 ||
		(get_num_clusters() == 1 &&
			is_cluster_sync(get_cluster_id(0)) == 0)) {
		/* Add overall CPU TMD node */
		if (tmd_add_cpufreq_dev_data(INT32_MAX) != 0) {
			msg("%s: Error adding overall CPU Device\n", __func__);
			return -(EFAULT);
		}
	} else {
                dbgmsg("%s: Target doesn't support overall CPU device\n",
			__func__);
	}
#endif
	/* Add individual CPU TMD's */
	for (idx = 0; idx < num_cpus; idx++) {
		cluster_id = get_cluster_id(idx);
		if (cluster_id != -1) {
			if (is_cluster_sync(cluster_id)) {
				dbgmsg(
				"%s: Target doesn't support CPU%d device\n",
				__func__,idx);
				continue;
			}
		}

		snprintf(name, DEVICES_MAX_NAME_LEN, "cpu%d", idx);

		if (tmd_add_cpufreq_dev_data(idx) != 0) {
			msg("%s: Error adding CPU%d Device\n", __func__,
			    idx);
			ret_val = -(EFAULT);
			break;
		}
		cpu_clnt[idx] = devices_manager_reg_clnt(name);
		if (cpu_clnt[idx] == NULL) {
			msg("%s: Error adding adding client for %s\n"
			    , __func__, name);
			ret_val = -(EFAULT);
			break;
		}
	}

	/* Add hotplug devices */
	for (idx = 0; idx < ARRAY_SIZE(hotplug_dev_list); idx++) {
		if ((idx >=  ARRAY_SIZE(hotplug_dev_list)) ||
		    (generic_dev_add(&hotplug_dev_list[idx]) != 0)) {
			msg("%s: Error adding %s\n", __func__,
			    hotplug_dev_list[idx].name);
			ret_val = -(EFAULT);
			break;
		}
	}

	return ret_val;
}
#if 0
static void tmd_release_gpufreq_dev(struct devices_manager_dev *dev_mgr)
{
	free(dev_mgr->lvl_info);
	free(dev_mgr);
}
#endif
int tmd_init_gpu_devs(void)
{
	int ret_val = 0;
	uint32_t num_freqs = 0;
	struct devices_manager_dev *dev = NULL;
	char path[DEVICES_MAX_PATH] = {0};
	int arr[50];
	uint32_t arr_idx;

	/* No gpus found */
	if (num_gpus == 0)
		return ret_val;

	dev = malloc(sizeof(struct devices_manager_dev));
	if (dev == NULL) {
		msg("%s: malloc failed.\n", __func__);
		ret_val = -(ENOMEM);
		goto handle_error;
	}
	memset(dev, 0, sizeof(struct devices_manager_dev));

	memset(arr, 0, sizeof(arr));
	snprintf(path, sizeof(path), GPU_SYSFS(GPU_AVAIL_FREQ_NODE), 0);
	num_freqs = read_int_list_from_file(path, arr, ARRAY_SIZE(arr));
	if (num_freqs < 1) {
		/* There can be non kgsl gpu driver, check devfreq gpu node */
		num_freqs = get_devfreq_gpu_freq_list(arr, ARRAY_SIZE(arr));
		if (num_freqs < 1 || num_freqs !=
				(get_gpu_cooling_device_max_state() + 1)) {
			msg("%s: Invalid number for freqs\n", __func__);
			ret_val = -(EFAULT);
			goto handle_error;
		}
	}
	/* Sort in descending order */
	sort_int_arr(arr, num_freqs, 0);

	dev->lvl_info =
		malloc(sizeof(struct device_lvl_info) * num_freqs);
	if (dev->lvl_info == NULL) {
		msg("%s: malloc failed.\n", __func__);
		ret_val = -(ENOMEM);
		goto handle_error;
	}
	memset(dev->lvl_info, 0, sizeof(struct device_lvl_info) * num_freqs);
	snprintf(dev->dev_info.name, DEVICES_MAX_NAME_LEN, "gpu");

	for (arr_idx = 0; arr_idx < num_freqs; arr_idx++) {
		dbgmsg("%s: %s lvl_info[%d]=%d", __func__, dev->dev_info.name,
		       arr_idx, arr[arr_idx]);
		dev->lvl_info[arr_idx].lvl.value = arr[arr_idx];
		dev->lvl_info[arr_idx].perf_lvl =  arr[arr_idx];
	}

	dev->dev_info.num_of_levels = num_freqs;
	dev->dev_info.dev_type = DEVICE_OP_VALUE_TYPE;

	dev->dev_info.max_dev_op_value_valid = 1;
	dev->dev_info.max_dev_op_value = dev->lvl_info[0].lvl.value;
	dev->active_req.value = dev->dev_info.max_dev_op_value;
	dev->dev_info.min_dev_op_value_valid = 1;
	dev->dev_info.min_dev_op_value =
		dev->lvl_info[num_freqs - 1].lvl.value;
	dev->action = gpu_action;
	dev->release = generic_dev_release;
	dev->current_op = gpu_current_op;
	strlcpy(dev->dev_info.device_units_name,
		UNIT_NAME_FREQ_HZ, MAX_UNIT_NAME_SIZE);
	init_cooling_device(dev);
	if (devices_manager_add_dev(dev) != 0) {
		msg("%s: Can not add device\n", __func__);
		ret_val = -(EFAULT);
	}

handle_error:
	if (ret_val < 0) {
		/* Error clean up */
		if (dev) {
			if (dev->lvl_info)
				free(dev->lvl_info);
			free(dev);
		}
	}
	return ret_val;
}

static int wlan_action(struct devices_manager_dev *dev_mgr)
{
	return wlan_request(dev_mgr->active_req.value);
}

static int battery_action(struct devices_manager_dev *dev_mgr)
{
	return battery_request(dev_mgr->active_req.value,
				dev_mgr->dev_info.cooling_dev_id);
}

static int profile_switch_action(struct devices_manager_dev *dev_mgr)
{
	return profile_switch_request(dev_mgr->active_req.value);
}

static int lcd_action(struct devices_manager_dev *dev_mgr)
{
	return lcd_brightness_request(dev_mgr->active_req.value,
				dev_mgr->dev_info.cooling_dev_id);
}

static int hotplug_action(struct devices_manager_dev *dev_mgr)
{
	int cpu = (int)(intptr_t)dev_mgr->data;
	union device_request req;
	uint32_t offline;

	if (hotplug_dev_clnt[cpu]) {
		offline = (dev_mgr->active_req.value) ? 1 : 0;
		req.value = 0;
		dbgmsg("ACTION: Hotplugged %s CPU[%d]\n",
			dev_mgr->active_req.value ? "OFF" : "ON", cpu);
		thermalmsg(LOG_LVL_DBG,
			(LOG_LOGCAT | LOG_LOCAL_SOCKET | LOG_TRACE),
			"%s: Hotplugged %s CPU[%d]\n", MITIGATION,
			dev_mgr->active_req.value ? "OFF" : "ON", cpu);
		(offline) ?
			device_clnt_request(hotplug_dev_clnt[cpu], &req) :
			device_clnt_cancel_request(hotplug_dev_clnt[cpu]);
		return 0;
	} else {
		return hotplug_request((int)(intptr_t)dev_mgr->data,
			       (dev_mgr->active_req.value) ? (1) : (0));
	}
}

static int ts_shutdown_action(struct devices_manager_dev *dev_mgr)
{
	/* Level greater than zero is delay time */
	return shutdown_action((int)dev_mgr->active_req.value);
}

static int ts_report_action(struct devices_manager_dev *dev_mgr)
{
	return report_action(SOCKET_RPT_LOG,
		(char *)dev_mgr->active_req.data);
}

static int report_rule_action(struct devices_manager_dev *dev_mgr)
{
	return report_action(SOCKET_RPT_RULE,
		(char *)dev_mgr->active_req.data);
}

static int cluster_action(struct devices_manager_dev *dev_mgr)
{
	return clusterfreq_request((int)(intptr_t)dev_mgr->data,
			       dev_mgr->active_req.value);
}

static int cpu_action(struct devices_manager_dev *dev_mgr)
{
	int mitig_value = dev_mgr->active_req.value;
	uint32_t lvl_idx = 0;

	if (dev_mgr->dev_info.cooling_dev_id != -1) {
		for (; lvl_idx < dev_mgr->dev_info.num_of_levels; lvl_idx++) {
			if (dev_mgr->lvl_info[lvl_idx].lvl.value <= mitig_value)
				break;
		}
		if (lvl_idx >=  dev_mgr->dev_info.num_of_levels)
			mitig_value = dev_mgr->dev_info.num_of_levels - 1;
		else
			mitig_value = lvl_idx;
		thermalmsg(LOG_LVL_DBG, (LOG_LOGCAT | LOG_TRACE),
				"%s: %s freq=%d state=%d\n", __func__,
				dev_mgr->dev_info.name,
				dev_mgr->active_req.value, mitig_value);
	}
	return cpufreq_request((int)(intptr_t)dev_mgr->data,
			       mitig_value,
			       dev_mgr->dev_info.cooling_dev_id);
}

static int cpu_all_action(struct devices_manager_dev *dev_mgr)
{
	int idx;

	for (idx = 0; idx < num_cpus; idx++)
		device_clnt_request(cpu_clnt[idx], &(dev_mgr->active_req));
	return 0;
}

static int gpu_action(struct devices_manager_dev *dev_mgr)
{
	int mitig_value = dev_mgr->active_req.value;
	uint32_t lvl_idx = 0;

	if (dev_mgr->dev_info.cooling_dev_id != -1) {
		for (; lvl_idx < dev_mgr->dev_info.num_of_levels; lvl_idx++) {
			if (dev_mgr->lvl_info[lvl_idx].lvl.value <= mitig_value)
				break;
		}
		if (lvl_idx >=  dev_mgr->dev_info.num_of_levels)
			mitig_value = dev_mgr->dev_info.num_of_levels;
		else
			mitig_value = lvl_idx;
		thermalmsg(LOG_LVL_DBG, (LOG_LOGCAT | LOG_TRACE),
				"%s: %s freq=%d state=%d\n", __func__,
				dev_mgr->dev_info.name,
				dev_mgr->active_req.value, mitig_value);
	}
	return gpufreq_request(0, mitig_value,
				dev_mgr->dev_info.cooling_dev_id);
}

static int modem_action(struct devices_manager_dev *dev_mgr)
{
	return modem_request(dev_mgr->active_req.value);
}

static int fusion_modem_action(struct devices_manager_dev *dev_mgr)
{
	return fusion_modem_request(dev_mgr->active_req.value);
}

static int cpr_band_action(struct devices_manager_dev *dev_mgr)
{
	return cpr_band_request(dev_mgr->active_req.value);
}

static int vdd_restriction_action(struct devices_manager_dev *dev_mgr)
{
	return vdd_restriction_request(dev_mgr->active_req.value);
}

static int kernel_mitigation_action(struct devices_manager_dev *dev_mgr)
{
	return kernel_mitigation_request(dev_mgr->active_req.value);
}
static int camera_action(struct devices_manager_dev *dev_mgr)
{
	return camera_request(dev_mgr->active_req.value);
}

static int camcorder_action(struct devices_manager_dev *dev_mgr)
{
	return camcorder_request(dev_mgr->active_req.value);
}

static int vdd_dig_automode_diable_action(struct devices_manager_dev *dev_mgr)
{
	return vdd_dig_sw_mode_request((dev_mgr->active_req.value) ? (2) : (0));
}

static int optimum_current_request_action(struct devices_manager_dev *dev_mgr)
{
	return optimum_current_request(dev_mgr->active_req.value);
}

static int mdp_action(struct devices_manager_dev *dev_mgr)
{
	return mdp_request(dev_mgr->active_req.value);
}

static int venus_action(struct devices_manager_dev *dev_mgr)
{
	return venus_request(dev_mgr->active_req.value);
}

static int modem_cx_limit_action(struct devices_manager_dev *dev_mgr)
{
	return modem_cx_limit_request(dev_mgr->active_req.value);
}

static int modem_proc_action(struct devices_manager_dev *dev_mgr)
{
	return modem_proc_request(dev_mgr->active_req.value);
}

static int modem_proc_current_action(struct devices_manager_dev *dev_mgr)
{
	return modem_proc_current_request(dev_mgr->active_req.value);
}

static int modem_bw_action(struct devices_manager_dev *dev_mgr)
{
	return modem_bw_request(dev_mgr->active_req.value);
}

static int modem_skin_action(struct devices_manager_dev *dev_mgr)
{
	return modem_skin_request(dev_mgr->active_req.value);
}

static int hvx_hw_action(struct devices_manager_dev *dev_mgr)
{
	return hvx_request(dev_mgr->active_req.value, HVX_HW_DEV);
}

static int hvx_sw_action(struct devices_manager_dev *dev_mgr)
{
	return hvx_request(dev_mgr->active_req.value, HVX_SW_DEV);
}

static int cpu_current_op(struct devices_manager_dev *dev_mgr,
			int *lvl)
{
	return cpufreq_current_op((int)(intptr_t)dev_mgr->data, lvl);
}

static int cpu_all_current_op(struct devices_manager_dev *dev_mgr,
			int *lvl)
{
	int idx = 0, ret = -1;

	for (idx = 0; idx < num_cpus && ret; idx++)
		ret = cpufreq_current_op(idx, lvl);

	return ret;
}

static int gpu_current_op(struct devices_manager_dev *dev_mgr,
			int *lvl)
{
	return gpufreq_current_op(0, lvl);
}

static int cluster_current_op(struct devices_manager_dev *dev_mgr,
			int *lvl)
{
	return clusterfreq_current_op((int)(intptr_t)dev_mgr->data, lvl);
}

static void generic_dev_release(struct devices_manager_dev *dev_mgr)
{
	free(dev_mgr);
}

static int generic_dev_add(struct tmd_generic_dev_info_t *gen_info)
{
	int ret_val = 0;
	struct devices_manager_dev *dev = NULL;

	dev = malloc(sizeof(struct devices_manager_dev));
	if (dev == NULL) {
		msg("%s: malloc failed.\n", __func__);
		ret_val = -(ENOMEM);
		goto handle_error;
	}
	memset(dev, 0, sizeof(struct devices_manager_dev));

	dev->dev_info.num_of_levels = gen_info->num_of_lvls;
	dev->dev_info.min_lvl = gen_info->min_lvl;

	strlcpy(dev->dev_info.name, gen_info->name, DEVICES_MAX_NAME_LEN);
	dev->dev_info.dev_type = gen_info->dev_type;

	if (dev->dev_info.dev_type == DEVICE_OP_VALUE_TYPE) {
		dev->dev_info.max_dev_op_value_valid = 1;
		dev->dev_info.max_dev_op_value = (int)gen_info->num_of_lvls;
		dev->active_req.value = dev->dev_info.max_dev_op_value;
	}

	if (gen_info->action)
		dev->action = gen_info->action;

	if (gen_info->data)
		dev->data = gen_info->data;

	dev->release = generic_dev_release;
	strlcpy(dev->dev_info.device_units_name,
		UNIT_NAME_FREQ_KHZ, MAX_UNIT_NAME_SIZE);
	init_cooling_device(dev);
	if (devices_manager_add_dev(dev) != 0) {
		msg("%s: Can not add device\n", __func__);
		ret_val = -(EFAULT);
	}

handle_error:
	if (ret_val < 0) {
		/* Error clean up */
		if (dev)
			free(dev);
	}
	return ret_val;
}

static int init_generic_devs(void)
{
	int ret_val = 0;
	int idx;

	for (idx = 0; idx < ARRAY_SIZE(gen_dev_list); idx++) {
		ret_val = generic_dev_add(&gen_dev_list[idx]);
		if (ret_val) {
			msg("%s: failed to add %s device.\n", __func__,
			    gen_dev_list[idx].name);
			break;
		}
	}
	return ret_val;
}

static int min_mode;

/* TMD init */
int devices_init(int minimum_mode)
{
	int ret_val = 0;

	min_mode = minimum_mode;
	gpufreq_init();
	cpufreq_init();
	clusterfreq_init();
	thermal_ioctl_init();

	if (!min_mode)
		qmi_communication_init();

	ret_val = tmd_init_cpu_devs();
	if (ret_val)
		msg("%s: Init CPU TMD failed %d\n", __func__, ret_val);

	ret_val = tmd_init_cluster_devs();
	if (ret_val)
		msg("%s: Init cluster TMD failed %d\n", __func__, ret_val);

	ret_val = tmd_init_voltage_devs();
	if (ret_val)
		msg("%s: Init voltage TMD failed %d\n", __func__, ret_val);

	ret_val = tmd_init_gpu_devs();
	if (ret_val)
		msg("%s: Init GPU TMD failed %d\n", __func__, ret_val);

	ret_val = init_generic_devs();
	if (ret_val)
		msg("%s: Init generic TMDs failed %d\n", __func__, ret_val);

	/* Functions to execute post devices added to device manager */
	hotplug_init();
	vdd_dig_sw_mode_init();
	vdd_rstr_init();
	opt_curr_req_init();
	profile_switch_init();
	cpr_band_init();
	lcd_device_init();
	battery_mitigation_init();
	hvx_mitigation_init();

	return ret_val;
}

void devices_release(void)
{
	if (!min_mode)
		qmi_communication_release();
	thermal_ioctl_release();
}
