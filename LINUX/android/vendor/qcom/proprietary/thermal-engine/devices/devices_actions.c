/*===========================================================================

  device_actions.c

  DESCRIPTION
  Thermal mitigation action implementations, excluding modem actions which
  are implemented in modem_mitigation_* files.

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  If present, action _init functions (e.g. cpufreq_init) should be called
  before the action functions. Each action function should keep its own
  state to avoid undesired repeating of mitigation actions at the same
  mitigation level.

  Copyright (c) 2010-2015,2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/


#include <unistd.h>
#include <sys/reboot.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <poll.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <linux/types.h>
#include <linux/netlink.h>
#include <errno.h>
#include <dirent.h>
#ifdef ANDROID
#include <cutils/uevent.h>
#include <cutils/properties.h>
#endif
#include <stringl.h>
#include "thermal.h"
#include "devices_actions.h"
#include "thermal_server.h"
#include "devices_manager.h"
#include "devices_manager_internal.h"
#include "devices.h"
#include "sysmonhvxthrottle.h"
#include "inc/thermal_ioctl_interface.h"
#include <dlfcn.h>

#define SENSOR(x)   (((x >= 0) && (x < SENSOR_IDX_MAX)) ? \
			sensor_names[x] : "unknown")
#define NUM_US_IN_MS        (1000)
#define CPU_BUF_MAX         (50)
#define HOTPLUG_BUF_MAX     (1024)

#if defined(SENSORS_8625) || defined(SENSORS_7x30)
	#define SHUTDOWN_CMD RB_AUTOBOOT
#else
	#define SHUTDOWN_CMD RB_POWER_OFF
#endif

/*===========================================================================
FUNCTION report_action

Action function to report msg to local socket.

ARGUMENTS
	type - type of socket either log or rule type
	msg  - msg to report

RETURN VALUE
	0 on success, negative on failure.
===========================================================================*/
int report_action(enum socket_type type, const char *msg)
{
	/* best effort report, no one may be listening on socket */
	write_to_local_socket(type, msg, REPORT_MSG_MAX);
	return 0;
}

/*===========================================================================
FUNCTION shutdown_action

Action function to shutdown the target on crossing of thermal threshold.

ARGUMENTS
	delay - delay before shutdown in milliseconds

RETURN VALUE
	0 on success, -1 on failure with errno set appropriately.
===========================================================================*/
int shutdown_action(int delay)
{
	static int shutdown_requested = 0;
	int ret = 0;

	if (delay < 0) {
		msg("Invalid shutdown delay value %d", delay);
		return -1;
	}

	if (!shutdown_requested) {
		info("ACTION: SHUTDOWN - shutdown requested "
		     "with %d millisecond delay\n", delay);

		usleep((uint32_t)delay * NUM_US_IN_MS);

		/* commit buffers to disk and shutdown */
		sync();
		ret = reboot(SHUTDOWN_CMD);

		if (ret)
			msg("Shutdown request failed");
		else
			shutdown_requested = 1;

	}
	return ret;
}

#define FREQ_STR_MAX		32

static char freq_max[MAX_CPUS][FREQ_STR_MAX];
static int freq_req[MAX_CPUS];

/* variable indicating last cpu max frequency requested per cpu,
   0 value also indicates maximum frequency requested */
static int prev_cpu_max[MAX_CPUS];
static int online[MAX_CPUS];
static pthread_mutex_t cpufreq_set_mtx = PTHREAD_MUTEX_INITIALIZER;

/*===========================================================================
FUNCTION cpufreq_set

Helper function to set the frequency of the cpu.

ARGUMENTS
	cpu - cpu id
	frequency - frequency in Hz.
	cdev_id - cooling device ID

RETURN VALUE
	0 on success, else on failure.
===========================================================================*/
int cpufreq_set(int cpu, int frequency, int cdev_id)
{
	char freq_buf[FREQ_STR_MAX] = {0};
	int  freq_min = 0;
	char node_buf[MAX_PATH] = {0};

	if (!online[cpu]) {
		dbgmsg("CPU[%d] offline. Not setting requested frequency %d\n",
				cpu, frequency);
		return 0;
	}

	snprintf(freq_buf, FREQ_STR_MAX - 1, "%d", frequency);
	if (cdev_id != -1) {
		snprintf(node_buf, MAX_PATH, CDEV_CUR_STATE, cdev_id);
	} else {
		/* Lower the minimum frequency if necessary */
		snprintf(node_buf, MAX_PATH, CPU_SYSFS(FREQ_MIN_NODE), cpu);
		read_line_from_file(node_buf, freq_buf, FREQ_STR_MAX - 1);
		freq_min = (int)strtol(freq_buf, NULL, 10);

		if (freq_min > frequency)
			write_to_file(node_buf, freq_buf, strlen(freq_buf));
		snprintf(node_buf, MAX_PATH, CPU_SYSFS(FREQ_MAX_NODE), cpu);
	}

	if (write_to_file(node_buf, freq_buf, strlen(freq_buf)) > 0) {
		dbgmsg("CPU[%d] frequency limited to %d\n", cpu, frequency);
		thermalmsg(LOG_LVL_DBG,
				(LOG_LOGCAT | LOG_LOCAL_SOCKET | LOG_TRACE),
				"%s:CPU[%d]:%d Khz\n", MITIGATION, cpu,
				frequency);
		return 1;
	}

	msg("Failed to set desired CPU[%d] frequency limit to %d\n",
			cpu, frequency);
	return 0;
}

/*===========================================================================
FUNCTION cpufreq_get

Helper function to get the current frequency of the cpu.

ARGUMENTS
	cpu - cpu id
	frequency - pointer to current operating frequency in Khz

RETURN VALUE
	0 on success, -1 on failure
===========================================================================*/
int cpufreq_get(int cpu, int *frequency)
{
	char freq_buf[FREQ_STR_MAX] = {0};
	char node_buf[MAX_PATH] = {0};
	int online = 0, ret = 0;

	/* First, make sure that the core is online, else return 0 */
	snprintf(node_buf, MAX_PATH, CPU_SYSFS(HOTPLUG_NODE), cpu);
	if (read_line_from_file(node_buf, freq_buf, FREQ_STR_MAX - 1) <= 0) {
		dbgmsg("Unable to read online val for cpu %d\n", cpu);
		ret = -1;
		goto exit_cpufreq;
	} else {
		online = (int)strtol(freq_buf, NULL, 10);
		dbgmsg("Reading CPU %d activity, online: %d", cpu, online);
	}

	if (online) {
		snprintf(node_buf, MAX_PATH, CPU_SYSFS(FREQ_RPT_NODE), cpu);
		if (read_line_from_file(
				node_buf, freq_buf, FREQ_STR_MAX - 1) <= 0) {
			dbgmsg("Unable to read current frequency for cpu %d",
				cpu);
			ret = -1;
			goto exit_cpufreq;
		} else {
			*frequency = (int)strtol(freq_buf, NULL, 10);
		}
	} else {
		ret = -1;
		goto exit_cpufreq;
	}

exit_cpufreq:
	return ret;
}

/*===========================================================================
FUNCTION cpufreq_current_op

Query function to request current CPU operating frequency.

ARGUMENTS
	cpu - cpu id
	frequency - pointer to cpu current operating frequence to be returned

RETURN VALUE
	0 on success, -1 on failure.
===========================================================================*/
int cpufreq_current_op(int cpu, int *frequency)
{

	return cpufreq_get(cpu, frequency);
}

#define CPU_TOPOLOGY_SYSFS "/sys/module/msm_thermal/cluster_info"
#define MAX_CLUSTER_INFO_LEN 128

struct cluster_info {
	int cluster_id;
	int cpu_bits;
	int sync;
	struct cluster_info *next;
};

struct cpu_topology {
	int num_cluster;
	struct cluster_info cluster[MAX_NUM_CLUSTERS];
};

static struct cpu_topology cpu_info;

/*===========================================================================
FUNCTION get_cluster_id_num_from_index

Function to retrieve cluster id corresponding to a cluster index.

ARGUMENTS
	idx - cluster index from 0-MAX_NUM_CLUSTERS-1.

RETURN VALUE
	cluster id. -1 on failure.
===========================================================================*/
int get_cluster_id_num_from_index(int idx)
{
	if (idx < 0 || idx >= cpu_info.num_cluster) {
		msg ("Invalid cluster index %d\n", idx);
		return -1;
	}
	return cpu_info.cluster[idx].cluster_id;
}

/*===========================================================================
FUNCTION get_cluster_id

Function to retrive cluster id correspoding to a cpu core.

ARGUMENTS
        cpu - cpu core id.

RETURN VALUE
        cluster id. -1 on failure.
===========================================================================*/
int get_cluster_id(int cpu)
{
	int i;

	if (cpu_info.num_cluster <= 0)
		return -1;

	if (cpu < 0 || cpu >= num_cpus) {
		msg ("Invalid cpu[%d]\n", cpu);
		return -1;
	}
	for (i = 0;i < cpu_info.num_cluster;i++) {
		if (cpu_info.cluster[i].cpu_bits & (1 << cpu))
			return cpu_info.cluster[i].cluster_id;
	}
	msg("cpu[%d] not found in any cluster\n", cpu);

	return -1;
}

/*===========================================================================
FUNCTION is_cluster_sync

Function to find if all cores in a cluster is synchronous.

ARGUMENTS
        cluster_id - cluster id.

RETURN VALUE
        1 if  sync. 0 if async. -1 on failure.
===========================================================================*/
int is_cluster_sync(int cluster_id)
{
	int idx;
	int ret = -1;

	if (cluster_id < 0 || cluster_id >= MAX_NUM_CLUSTERS) {
		msg ("Invalid cluster id:%d\n", cluster_id);
		goto exit_cluster_sync;
	}
	for (idx = 0;idx < cpu_info.num_cluster;idx++) {
		if (cpu_info.cluster[idx].cluster_id == cluster_id) {
			ret = cpu_info.cluster[idx].sync;
			goto exit_cluster_sync;
		}
	}
	msg ("Cluster id not found:%d\n", cluster_id);

exit_cluster_sync:
	return ret;
}

/*===========================================================================
FUNCTION get_cores_in_cluster

Function to retrive all cores in a cluster.

ARGUMENTS
        cluster_id - cluster id.

RETURN VALUE
        core bit-map in a cluster. -1 on failure.
===========================================================================*/
int get_cores_in_cluster(int cluster_id)
{
	int idx;
	int ret =-1;

	if (cluster_id < 0 || cluster_id >= MAX_NUM_CLUSTERS) {
		msg ("Invalid cluster id:%d\n", cluster_id);
		goto exit_get_cores;
	}
	for (idx = 0;idx < cpu_info.num_cluster;idx++) {
		if (cpu_info.cluster[idx].cluster_id == cluster_id) {
			ret = cpu_info.cluster[idx].cpu_bits;
			goto exit_get_cores;
		}
	}
	msg ("Cluster id not found:%d\n", cluster_id);

exit_get_cores:
	return ret;
}

/*===========================================================================
FUNCTION get_num_clusters

Function to retrive num of cpu clusters.

ARGUMENTS
        None.

RETURN VALUE
        retun number of clusters.
===========================================================================*/
int get_num_clusters()
{
	return cpu_info.num_cluster;
}

/*===========================================================================
FUNCTION update_cpu_topology

Function to update local structure with cpu topology from thermal sysfs node.

ARGUMENTS
        none

RETURN VALUE
        0 on success.
===========================================================================*/

static int update_cpu_topology(void)
{
	int ret_val = 0;
	char buf[MAX_CLUSTER_INFO_LEN];
	char *pcluster_info = NULL, *psave1 = NULL;

	cpu_info.num_cluster = 0; /* indication to revert to older algo */

	if (read_line_from_file(CPU_TOPOLOGY_SYSFS, buf, sizeof(buf)) <= 0) {
		info("%s: Cluster info node not found%s\n",
			__func__, CPU_TOPOLOGY_SYSFS);
		return -1;
	}
	pcluster_info = strtok_r(buf, " ", &psave1);
	while (pcluster_info != NULL && cpu_info.num_cluster < MAX_NUM_CLUSTERS) {
		char *pcluster = NULL;
		char *psave2 = NULL;

		pcluster = strtok_r(pcluster_info, ":", &psave2);
		if (pcluster == NULL)
			goto next_cluster;
		cpu_info.cluster[cpu_info.num_cluster].cluster_id = (int)strtol(pcluster, NULL, 10);

		pcluster = strtok_r(NULL, ":", &psave2);
		if (pcluster == NULL)
			goto next_cluster;
		cpu_info.cluster[cpu_info.num_cluster].cpu_bits = (int)strtol(pcluster, NULL, 10);

		pcluster = strtok_r(NULL, ":", &psave2);
		if (pcluster == NULL)
			goto next_cluster;
		cpu_info.cluster[cpu_info.num_cluster].sync = (int)strtol(pcluster, NULL, 10);

		msg("%s: Cluster Info[%d]. Cluster Id: %d cpu_bits:0x%x sync:%d",
			__func__, cpu_info.num_cluster,
			cpu_info.cluster[cpu_info.num_cluster].cluster_id,
			cpu_info.cluster[cpu_info.num_cluster].cpu_bits,
			cpu_info.cluster[cpu_info.num_cluster].sync);
		cpu_info.num_cluster++;
next_cluster:
		pcluster_info = strtok_r(NULL, " ", &psave1);
	}

	return ret_val;

}

/*===========================================================================
FUNCTION clusterfreq_init

Initialization function for clusterfreq.
Needs to be called before clusterfreq_request() can be used.

ARGUMENTS
	None.

RETURN VALUE
	0 on success, else on failure.
===========================================================================*/
void clusterfreq_init(void)
{
	update_cpu_topology();
}

/*===========================================================================
FUNCTION cpufreq_init

Initialization function for cpufreq.
Needs to be called before cpufreq_request() can be used.

ARGUMENTS
	None.

RETURN VALUE
	0 on success, else on failure.
===========================================================================*/
int cpufreq_init(void)
{
	int cpu;
	char fmaxinfo_buf[MAX_PATH] = {0};

	num_cpus = get_num_cpus();
	if (num_cpus == -1) {
		msg("get_num_cpus() failed, default to MAX_CPUs %d", MAX_CPUS);
		num_cpus = MAX_CPUS;
	}
	if (!thermal_ioctl_defined())
		return 0;
	if (num_cpus > MAX_CPUS) {
		msg("BUG: num_cpus > MAX_CPUS");
		return -(EFAULT);
	}
	info("Number of cpus :%d\n", num_cpus);
	memset(online, 0, MAX_CPUS);

	for (cpu = 0; cpu < num_cpus; cpu++) {
		snprintf(fmaxinfo_buf, MAX_PATH, CPU_SYSFS(FMAX_INFO_NODE), cpu);
		/* Store max CPU frequency */
		if (read_line_from_file(fmaxinfo_buf,
					freq_max[cpu], FREQ_STR_MAX) > 0) {
			dbgmsg("Maximum CPU[%d] frequency %ld KHz\n", cpu,
					strtol(freq_max[cpu], NULL, 10));
			online[cpu] = 1;

		}
		else {
			info("CPUFreq not enabled for CPU[%d]\n", cpu);
		}

		freq_req[cpu] = -1;
	}

	return 0;
}

/*===========================================================================
FUNCTION cpu_voltage_request

Action function to request voltage scaling.  Used for single voltage rail systems.

ARGUMENTS
	void * data
	voltage to apply to all cpu clusters.

RETURN VALUE
	0 on success, else negative value on failure.
===========================================================================*/
int cpu_voltage_request(void *data, uint32_t voltage)
{
	struct tmd_cpu_voltage_dev_table_t *ptr_data =
		(struct tmd_cpu_voltage_dev_table_t *)data;
	unsigned int idx = 0;
	uint32_t cluster_num = 0;
	int ret = 0;
	union device_request req;

	for (idx = 0; idx < ptr_data->length; idx++) {
		if (ptr_data->voltage_table[ptr_data->table_max_cluster_num][idx]
			== (unsigned int)voltage)
			break;
	}
	if (idx == ptr_data->length) {
		msg("Error, voltage not found\n");
		ret = -1;
		goto exit_voltage_request;
	}
	for (cluster_num = 0; cluster_num < ptr_data->num_clusters;
		cluster_num++) {
		req.value = (int)ptr_data->cluster_freq[cluster_num][idx];
		device_clnt_request(ptr_data->dev_clnt[cluster_num], &req);
		dbgmsg("ACTION: CLUSTER - "
			"Setting CLUSTER[%d] to voltage %d freq = %u\n", cluster_num ,
			(uint32_t)ptr_data->voltage_table[cluster_num][idx],
			(uint32_t)ptr_data->cluster_freq[cluster_num][idx]);
		thermalmsg(LOG_LVL_DBG,
			(LOG_LOGCAT | LOG_LOCAL_SOCKET | LOG_TRACE),
			"%s:CLUSTER[%d]: %d mV freq = %u kHz\n", MITIGATION, cluster_num,
			(uint32_t)ptr_data->voltage_table[cluster_num][idx],
			(uint32_t)ptr_data->cluster_freq[cluster_num][idx]);
		ptr_data->current_level = idx;
	}

exit_voltage_request:
	return ret;
}

/*===========================================================================
FUNCTION clusterfreq_request

Action function to request cluster frequency scaling.

ARGUMENTS
	cluster - cluster id
	frequency - cluster frequency to scale to, -1 for maximum cluster frequency

RETURN VALUE
	0 on success, else on failure.
===========================================================================*/
int clusterfreq_request(int cluster, int frequency)
{
	dbgmsg("ACTION: CLUSTER - "
	     "Setting CLUSTER[%d] to %d\n", cluster, frequency);
	thermalmsg(LOG_LVL_DBG,
			(LOG_LOGCAT | LOG_LOCAL_SOCKET | LOG_TRACE),
			"%s:CLUSTER[%d]:%d Khz\n",
			MITIGATION, cluster, frequency);

	if (thermal_ioctl_set_cluster_frequency((uint32_t)cluster,
		(uint32_t)frequency, 1U) < 0)
		return -1;

	return 0;
}

/*===========================================================================
FUNCTION clusterfreq_get

Query function to request curent cluster operating frequency.

ARGUMENTS
	cluster - cluster id
	frequency - pointer to return current operating cluster frequency in Khz

RETURN VALUE
	0 on success, -1 on failure.
===========================================================================*/
int clusterfreq_get(int cluster_id, int *frequency)
{
	int cluster_core_bits = 0, i;

	cluster_core_bits = get_cores_in_cluster(cluster_id);
	for (i = 0; i < MAX_CPUS; i++)
		if (cluster_core_bits & (1 << i))
			if (!cpufreq_get(i, frequency))
				break;

	if (i >= MAX_CPUS)
		return -1;

	return 0;
}

/*===========================================================================
FUNCTION cpu_voltage_current_op

Query function to request current cluster operating voltage.

ARGUMENTS
	void * contains private data for this device.
	voltage_level - pointer to cluster current operating frequency to be returned

RETURN VALUE
	0 on success, -1 on failure.
===========================================================================*/
int cpu_voltage_current_op(void *data, uint32_t *voltage_level)
{
	struct tmd_cpu_voltage_dev_table_t *ptr_data = (struct tmd_cpu_voltage_dev_table_t *)data;
	*voltage_level = ptr_data->voltage_table[0][ptr_data->current_level];
	return 0;
}

/*===========================================================================
FUNCTION clusterfreq_current_op

Query function to request current cluster operating frequency.

ARGUMENTS
	cluster_id - cluster id
	frequency - pointer to cluster current operating frequence to be returned

RETURN VALUE
	0 on success, -1 on failure.
===========================================================================*/
int clusterfreq_current_op(int cluster_id, int *frequency)
{
	return clusterfreq_get(cluster_id, frequency);
}

/*===========================================================================
FUNCTION cpufreq_request

Action function to request CPU frequency scaling.

ARGUMENTS
	cpu - cpu id
	frequency - cpu frequency to scale to, -1 for maximum cpu frequency
	cdev_id - cooling device ID

RETURN VALUE
	0 on success, else on failure.
===========================================================================*/
int cpufreq_request(int cpu, int frequency, int cdev_id)
{
	/* Determine whether to use IOCTL or sysfs interface to mitigate
	the cpu frequency*/
	if (!thermal_ioctl_defined()) {
		/*thermal_ioctl_defined() returns 0 if IOCTL is successfully
		initialized*/
		dbgmsg("ACTION: CPU - "
		     "Setting CPU[%d] to %d\n", cpu, frequency);
		thermalmsg(LOG_LVL_DBG,
				(LOG_LOGCAT | LOG_LOCAL_SOCKET | LOG_TRACE),
				"%s:CPU[%d]:%d Khz\n",
				MITIGATION, cpu, frequency);

		if (thermal_ioctl_set_frequency((uint32_t)cpu, (uint32_t)frequency, 1U) < 0)
			return -1;
		dbgmsg("CPU[%d] frequency limited to %d\n", cpu, frequency);
	} else if (cdev_id != -1) {
		pthread_mutex_lock(&cpufreq_set_mtx);

		freq_req[cpu] = frequency;

		if (!prev_cpu_max[cpu])
			prev_cpu_max[cpu] = INT32_MAX;

		if (frequency != prev_cpu_max[cpu]) {
			thermalmsg(LOG_LVL_INFO,
				(LOG_LOGCAT | LOG_TRACE | LOG_LOCAL_SOCKET),
				"ACTION: CPU - Setting CPU[%d] to %d\n",
				cpu, frequency);
			cpufreq_set(cpu, frequency, cdev_id);
			prev_cpu_max[cpu] = frequency;
		} else {
			dbgmsg("Using previous CPU[%d] frequency.\n", cpu);
		}

		pthread_mutex_unlock(&cpufreq_set_mtx);
	} else {
		int max_freq = (int)strtol(freq_max[cpu], NULL, 10);

		pthread_mutex_lock(&cpufreq_set_mtx);

		freq_req[cpu] = frequency;

		if (!prev_cpu_max[cpu])
			prev_cpu_max[cpu] = max_freq;

		if ((freq_req[cpu] > 0) && (freq_req[cpu] < max_freq))
			max_freq = freq_req[cpu];

		/* CPU was offline on thermald start, save frequency */
		if (max_freq == 0) {
			max_freq = (frequency == -1) ? 0 : frequency;
		}

		if (max_freq != prev_cpu_max[cpu]) {
			info("ACTION: CPU - "
			     "Setting CPU[%d] to %d\n", cpu, max_freq);
			cpufreq_set(cpu, max_freq, cdev_id);
			prev_cpu_max[cpu] = max_freq;
		} else {
			dbgmsg("Using previous CPU[%d] frequency.\n", cpu);
		}

		pthread_mutex_unlock(&cpufreq_set_mtx);
	}
	return 0;
}

/* LCD Backlight brightness control action */

#define LCD_MAX_BRIGHTNESS "/sys/class/leds/lcd-backlight/max_brightness"
#define LCD_CURR_BRIGHTNESS "/sys/class/leds/lcd-backlight/brightness"
#define LCD_DEFAULT_BRIGHTNESS 255
static pthread_mutex_t lcd_mtx = PTHREAD_MUTEX_INITIALIZER;
static int max_brightness;
static int use_default_brightness;

void lcd_device_init(void)
{
	char buf[UINT_BUF_MAX] = {0};
	char *end_ptr;

	if (read_line_from_file(LCD_MAX_BRIGHTNESS, buf,
			UINT_BUF_MAX) <= 0) {
		msg("Error reading %s\n", LCD_MAX_BRIGHTNESS);
		max_brightness = LCD_DEFAULT_BRIGHTNESS;
		use_default_brightness = 1;
		return;
	}
	max_brightness = (int)strtol(buf, &end_ptr, 10);
	if (max_brightness == 0 || end_ptr == buf
		|| *end_ptr != '\n') {
		msg("Max brightness invalid value %d\n",
				max_brightness);
		max_brightness = LCD_DEFAULT_BRIGHTNESS;
		use_default_brightness = 1;
		return;
	}
	dbgmsg("Max brightness:%d\n", max_brightness);
}

/*===========================================================================
LOCAL FUNCTION restore_lcd_brightness

Helper function to restore LCD brightness.

ARGUMENTS
	value - 1 to max brightness value for LCD brightness, -1 for no
	throttling

RETURN VALUE
	None.
===========================================================================*/
static void restore_lcd_brightness(int restore_brightness,
				  int current_brightness)
{
	char buf[UINT_BUF_MAX] = {0};

	/* Don't restore if LCD is currently off */
	if (restore_brightness > 0 && current_brightness > 0) {
		snprintf(buf, UINT_BUF_MAX, "%d",
			 restore_brightness);
		if (write_to_file(LCD_CURR_BRIGHTNESS, buf,
				  strlen(buf)) > 0) {
			dbgmsg("Setting curr LCD brightness to %d\n",
			     restore_brightness);
		} else {
			msg("Unable to set LCD brightness to %d\n",
			    restore_brightness);
		}
	}
}

/*===========================================================================
FUNCTION lcd_brightness_request

Action function to request LCD brightness throttling.

ARGUMENTS
	value - 1-max brightness value for LCD brightness, -1 for no
	throttling
	cdev_id - cooling device ID

RETURN VALUE
	0 on success, -1 on failure.
===========================================================================*/
int lcd_brightness_request(int value, int cdev_id)
{
	char buf[UINT_BUF_MAX] = {0};
	static int prev;
	static int in_mitigation = 0;
	static int restore_brightness = -1;
	int current_brightness = -1, max_state = -1;
	char * end_ptr;
	char node_buf[MAX_PATH] = {0};

	if (value < -1 || value == 0) {
		msg("Incorrect value %d value for LCD brightness\n", value);
		return -1;
	}

	pthread_mutex_lock(&lcd_mtx);

	if (use_default_brightness && cdev_id != -1) {
		snprintf(node_buf, MAX_PATH, CDEV_MAX_STATE, cdev_id);
		if (read_line_from_file(node_buf, buf, UINT_BUF_MAX) <= 0) {
			msg("Error reading lcd max state\n");
		} else {
			max_state = (int)strtol(buf, &end_ptr, 10);
			if (max_state == 0 && (end_ptr == buf
				|| *end_ptr != '\n')) {
				msg("Invalid lcd max state\n");
			} else {
				max_brightness = max_state;
				use_default_brightness = 0;
			}
		}
	}

	if ((value == -1  || value == INT32_MAX) && cdev_id != -1) {
		value = 0;
	} else if (value == -1 || value > max_brightness) {
		value = max_brightness;
	}

	/* get current brightness */
	if (read_line_from_file(LCD_CURR_BRIGHTNESS, buf, UINT_BUF_MAX) > 0) {
		current_brightness = (int)strtol(buf, &end_ptr, 10);

		if (current_brightness == 0 &&
		    (end_ptr == buf || *end_ptr != '\n')) {
			dbgmsg("Failed to parse current brightness");
			current_brightness = -1;
		} else if (current_brightness < 0 ||
			   current_brightness > max_brightness) {
			dbgmsg("Current brightness read invalid, value %d",
			       current_brightness);
			current_brightness = -1;
		} else if (in_mitigation == 0 && value < max_brightness) {
			/* if entering mitigation, save current brightness
			   unless LCD is off */
			if (current_brightness == 0) {
				dbgmsg("LCD off at start of mitigation, "
				       "do not save restore value");
			} else {
				restore_brightness = current_brightness;
				dbgmsg("Brightness at start of mitigation is %d\n",
				       current_brightness);
			}
		}
	} else {
		msg("Error reading current brightness.\n");
	}

	if (value == max_brightness || (value == 0 && cdev_id != -1)) {
		in_mitigation = 0;
	} else {
		in_mitigation = 1;
	}

	if (prev != value) {

		if (cdev_id != -1)
			snprintf(node_buf, MAX_PATH, CDEV_CUR_STATE, cdev_id);
		else
			snprintf(node_buf, MAX_PATH, LCD_MAX_BRIGHTNESS);
		snprintf(buf, UINT_BUF_MAX, "%d", value);
		if (write_to_file(node_buf, buf, strlen(buf)) > 0) {
			dbgmsg("ACTION: LCD - "
			     "Setting max LCD brightness to %d\n", value);
			thermalmsg(LOG_LVL_DBG,
				(LOG_LOGCAT | LOG_LOCAL_SOCKET | LOG_TRACE),
				"%s:LCD:%d\n", MITIGATION, value);
			prev = value;

			if (enable_restore_brightness)
				restore_lcd_brightness(restore_brightness,
						       current_brightness);
		} else {
			msg("Unable to set max LCD brightness to %d\n", value);
		}

		if (in_mitigation == 0) {
			restore_brightness = -1;
		}
	}

	pthread_mutex_unlock(&lcd_mtx);
	return 0;
}

#ifdef ANDROID
static void *therm_profile_handle;
static int (*profile_switch)(int handle, int profile);
static int profile_switch_handle, curr_profile;
static pthread_mutex_t profile_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

int profile_switch_init(void)
{
#ifdef ANDROID
	char opt_lib_path[PATH_MAX] = {0};

	if (property_get("ro.vendor.extension_library", opt_lib_path, NULL)
		!= 0) {
		if((therm_profile_handle = dlopen(opt_lib_path, RTLD_NOW)) == NULL) {
			msg("Unable to acquire profile handle\n");
			return -ENOSYS;
		} else {
			profile_switch = (int(*)(int, int))
					dlsym(therm_profile_handle,
					"perf_lock_use_profile");
			if (!profile_switch) {
				msg("Unable to acquire profile switch API\n");
				return -ENOSYS;
			}
			curr_profile = PERF_DEFAULT_PROFILE;
		}
	}
#endif
	return 0;
}

/*===========================================================================
FUNCTION profile_switch_request

Action function to request perf lock action to configure the CPU governor.

ARGUMENTS
	level - profile level

RETURN VALUE
	0 on success, -1 on failure.
===========================================================================*/
int profile_switch_request(int level)
{
	int ret = 0;

#ifdef ANDROID
	if (!therm_profile_handle) {
		ret = profile_switch_init();
		if (ret) {
			msg("FAILED to acquire perf lock\n");
			return ret;
		}
	}
	pthread_mutex_lock(&profile_mutex);
	level = MAX(level, PERF_DEFAULT_PROFILE);
	level = MIN(level, MAX_PROFILE_LEVEL);

	if (level == curr_profile)
		goto unlock_and_exit;
	if (profile_switch_handle) {
		profile_switch(profile_switch_handle, PERF_DEFAULT_PROFILE);
		thermalmsg(LOG_LVL_DBG,
			(LOG_LOGCAT | LOG_LOCAL_SOCKET | LOG_TRACE),
			"%s:Clearing Profile:%d\n", MITIGATION,
			curr_profile);
		profile_switch_handle = 0;
		curr_profile = PERF_DEFAULT_PROFILE;
		if (level == PERF_DEFAULT_PROFILE)
			goto unlock_and_exit;
	}
	profile_switch_handle = profile_switch(profile_switch_handle, level);
	thermalmsg(LOG_LVL_DBG,
			(LOG_LOGCAT | LOG_LOCAL_SOCKET | LOG_TRACE),
			"%s:Profile Switch:%d\n", MITIGATION, level);
	curr_profile = level;

unlock_and_exit:
	pthread_mutex_unlock(&profile_mutex);
#endif
	return ret;
}

static pthread_mutex_t battery_mtx = PTHREAD_MUTEX_INITIALIZER;

#if defined (SENSORS_8960)
#define BATTERY_MITIGATION_SYSFS "/sys/module/pm8921_charger/parameters/thermal_mitigation"
#else
#define BATTERY_MITIGATION_SYSFS "/sys/class/power_supply/battery/system_temp_level"
#endif

/*===========================================================================
FUNCTION battery_request

Action function to request battery charging current throttling action.

ARGUMENTS
	level - 0 - MAX_BATTERY_MITIGATION_LEVEL throttling level for
	        battery charging current
	cdev_id - cooling device ID

RETURN VALUE
	0 on success, -1 on failure.
===========================================================================*/
int battery_request(int level, int cdev_id)
{
	int ret = -1;
	char buf[UINT_BUF_MAX] = {0};
	char node_buf[MAX_PATH] = {0};
	static int prev;

	if (level < 0)
		level = 0;

	if (level > MAX_BATTERY_MITIGATION_LEVEL)
		level = MAX_BATTERY_MITIGATION_LEVEL;

	pthread_mutex_lock(&battery_mtx);

	if (level != prev) {
		if (cdev_id != -1)
			snprintf(node_buf, MAX_PATH, CDEV_CUR_STATE, cdev_id);
		else
			snprintf(node_buf, MAX_PATH, BATTERY_MITIGATION_SYSFS);

		snprintf(buf, UINT_BUF_MAX, "%d", level);
		if (write_to_file(node_buf, buf, strlen(buf)) > 0) {
			dbgmsg("ACTION: BATTERY - "
			     "Setting battery charging mitigation to %d\n", level);
			thermalmsg(LOG_LVL_DBG,
				(LOG_LOGCAT | LOG_LOCAL_SOCKET | LOG_TRACE),
				"%s:Battery:%d\n", MITIGATION, level);
			prev = level;
			ret = 0;
		} else {
			msg("Unable to set battery charging mitigation to %d\n", level);
		}
	} else {
		dbgmsg("Battery charging mitigation already at level %d\n", level);
		ret = 0;
	}

	pthread_mutex_unlock(&battery_mtx);
	return ret;
}

void battery_mitigation_init(void)
{
	char buf[UINT_BUF_MAX] = "0";

	if (write_to_file(BATTERY_MITIGATION_SYSFS, buf, strlen(buf)) > 0)
		thermalmsg(LOG_LVL_DBG, (LOG_LOGCAT | LOG_LOCAL_SOCKET | LOG_TRACE),
				"%s:Battery init level 0\n", MITIGATION);
	else
		msg("Unable to init battery charging mitigation to 0\n");
}

#define MAX_GPUS              (4)       /* Supporting up to 4 core systems */
#define MAX_GPU_FREQ          2147483647
#define GPU_FREQ_STR_MAX		32

#define DEVFREQ_DIR   "/sys/class/devfreq/"
#define DEVFREQ_GPU_NODE  "qcom,kgsl-3d0"
#define DEVFREQ_AVAIL_FREQ "available_frequencies"

static char gpu_freq_max[MAX_GPUS][GPU_FREQ_STR_MAX];
static int  gpu_freq_req[MAX_GPUS];
static char gpu_devfreq_avail_freq[MAX_PATH] = {0};

/* variable indicating last cpu max frequency requested per gpu,
   0 value also indicates maximum frequency requested */
static int prev_gpu_max[MAX_GPUS];
static pthread_mutex_t gpufreq_set_mtx = PTHREAD_MUTEX_INITIALIZER;

/*===========================================================================
FUNCTION gpufreq_set

Helper function to set the frequency of the GPU.

ARGUMENTS
	GPU - gpu id
	frequency - frequency in Hz.
	cdev_id - cooling device ID

RETURN VALUE
	0 on success, else on failure.
===========================================================================*/
int gpufreq_set(int gpu, int frequency, int cdev_id)
{
	char freq_buf[GPU_FREQ_STR_MAX] = {0};
	char node_buf[MAX_PATH] = {0};

	snprintf(freq_buf, GPU_FREQ_STR_MAX - 1, "%d", frequency);

	/* Write out the max frequency */
	if (cdev_id != -1)
		snprintf(node_buf, MAX_PATH, CDEV_CUR_STATE, cdev_id);
	else
		snprintf(node_buf, MAX_PATH, GPU_SYSFS(GPU_FREQ_MAX_NODE), gpu);

	if (write_to_file(node_buf, freq_buf, strlen(freq_buf)) > 0) {
		dbgmsg("GPU[%d] frequency limited to %d\n", gpu, frequency);
		thermalmsg(LOG_LVL_DBG,
			(LOG_LOGCAT | LOG_LOCAL_SOCKET | LOG_TRACE),
			"%s:GPU[%d]:%d Hz\n", MITIGATION, gpu, frequency);
		return 0;
	}

	msg("Failed to set desired GPU[%d] frequency limit to %d\n",
			gpu, frequency);
	return 1;
}

/*===========================================================================
FUNCTION gpufreq_get

Helper function to read the operating frequency of the GPU.

ARGUMENTS
	GPU - gpu id
	frequency - pointer to return operating frequency of GPU in Hz

RETURN VALUE
	0 on success, -1 on failure.
===========================================================================*/
int gpufreq_get(int gpu, int *frequency)
{
	char freq_buf[GPU_FREQ_STR_MAX] = {0};
	char node_buf[MAX_PATH] = {0};

	/* Write out the max frequency */
	snprintf(node_buf, MAX_PATH, GPU_SYSFS(GPU_FREQ_RPT_NODE), gpu);

	if (read_line_from_file(node_buf, freq_buf, FREQ_STR_MAX - 1) <= 0) {
		dbgmsg("Unable to read GPU frequency");
		return -1;
	} else {
		*frequency = (int)strtol(freq_buf, NULL, 10);
	}

	return 0;
}

static int reset_gpu_cooling_device(void)
{
	int cdev_id = -1;

	cdev_id = get_cdevn(GPU_COOLING_DEV_NAME);
	if (cdev_id < 0) {
		msg("Could not find cooling device\n");
		return cdev_id;
	}
	gpufreq_set(0, 0, cdev_id);

	return 0;
}

int get_gpu_cooling_device_max_state(void)
{
	char buf[UINT_BUF_MAX] = {0};
	char * end_ptr;
	char node_buf[MAX_PATH] = {0};
	int cdev_id = -1, max_state = -1;

	cdev_id = get_cdevn(GPU_COOLING_DEV_NAME);
	if (cdev_id < 0) {
		msg("Could not find %s cooling device\n", GPU_COOLING_DEV_NAME);
		return cdev_id;
	}

	snprintf(node_buf, MAX_PATH, CDEV_MAX_STATE, cdev_id);
	if (read_line_from_file(node_buf, buf, UINT_BUF_MAX) <= 0) {
		msg("Could not read %s max state\n", GPU_COOLING_DEV_NAME);
		return -1;
	}

	max_state = (int)strtol(buf, &end_ptr, 10);
	if (max_state == LONG_MIN || max_state == LONG_MAX ||
		(max_state == 0 && (end_ptr == buf || *end_ptr != '\n'))) {
		msg("Invalid %s max state\n", GPU_COOLING_DEV_NAME);
		return -1;
	}
	return max_state;
}

int get_devfreq_gpu_freq_list(int *arr, uint32_t arr_sz)
{
	if (gpu_devfreq_avail_freq[0] == '\0')
		return -EINVAL;
	return read_int_list_from_file(gpu_devfreq_avail_freq, arr, arr_sz);
}

static int check_devfreq_gpu_node(void)
{
	char temp[MAX_PATH] = {0};
	int ngpus = 0;

	ngpus = search_sysfs_dir_name(temp, DEVFREQ_DIR, DEVFREQ_GPU_NODE);
	if (!ngpus) {
		ngpus = 1;
		snprintf(gpu_devfreq_avail_freq, MAX_PATH, "%s%s", temp,
			 DEVFREQ_AVAIL_FREQ);
	} else {
		dbgmsg("Failed to get gpu node:%s from %s\n",
			DEVFREQ_GPU_NODE, DEVFREQ_DIR);
	}

	return ngpus;
}

static int get_num_gpus(void)
{
	DIR *tdir = NULL;
	int ngpus = 0;
	char cwd[MAX_PATH] = {0};
	int gpu = 0, ret = 0;
	struct dirent *gpu_dirent;

	if (!getcwd(cwd, sizeof(cwd)))
		return -1;

	/* Change dir to read the entries */
	ret = chdir(GPU_SYSFS_DIR);
	if (ret) {
		info("Unable to change directory to %s\n", GPU_SYSFS_DIR);
		return 0;
	}
	tdir = opendir(GPU_SYSFS_DIR);
	if (!tdir) {
		info("Unable to open %s\n", GPU_SYSFS_DIR);
		return 0;
	}

	while ((gpu_dirent = readdir(tdir))) {
		if (!strstr(gpu_dirent->d_name, "kgsl-3d"))
			continue;
		if (!sscanf(gpu_dirent->d_name, "kgsl-3d%d", &gpu))
			continue;
		ngpus++;
	}

	closedir(tdir);
	ret = chdir(cwd); /* Restore current working dir */

	dbgmsg("Number of GPU cores %d\n", ngpus);

	return ngpus;
}

/*===========================================================================
FUNCTION gpufreq_init

Initialization function for gpufreq.
Needs to be called before gpufreq_request() can be used.

ARGUMENTS
	None.

RETURN VALUE
	0 on success, else on failure.
===========================================================================*/
int gpufreq_init(void)
{
	int gpu;

	num_gpus = get_num_gpus();
	if (num_gpus <= 0) {
		num_gpus = check_devfreq_gpu_node();
		if (num_gpus <= 0) {
			msg("get_num_gpus() failed, default to MAX_GPUs %d",
				MAX_GPUS);
			num_gpus = MAX_GPUS;
		} else {
			if (reset_gpu_cooling_device() < 0) {
				msg("Failed to find gpu cdev, disabling gpu\n");
				num_gpus = 0;
			}
		}
	}
	if (num_gpus > MAX_GPUS) {
		msg("BUG: num_gpus > MAX_GPUS");
		return -1;
	}
	info("Number of gpus :%d\n", num_gpus);

	for (gpu = 0; gpu < num_gpus; gpu++) {
		gpu_freq_req[gpu] = -1;
		snprintf(gpu_freq_max[gpu], GPU_FREQ_STR_MAX - 1, "%d", MAX_GPU_FREQ);
	}

	return 0;
}

/*===========================================================================
FUNCTION gpufreq_request

Action function to request GPU frequency scaling.

ARGUMENTS
	gpu - gpu id
	frequency - gpu frequency to scale to, -1 for maximum gpu frequency
	cdev_id - cooling device ID

RETURN VALUE
	0 on success, else on failure.
===========================================================================*/
int gpufreq_request(int gpu, int frequency, int cdev_id)
{
	int max_freq;

	if (gpu >= num_gpus) {
		msg("gpufreq_request() Invalid GPU ID %d", gpu);
		return -(EFAULT);
	}

	if (cdev_id != -1) {
		pthread_mutex_lock(&gpufreq_set_mtx);

		gpu_freq_req[gpu] = frequency;

		if (!prev_gpu_max[gpu])
			prev_gpu_max[gpu] = INT32_MAX;

		if (frequency != prev_gpu_max[gpu]) {
			thermalmsg(LOG_LVL_INFO,
				(LOG_LOGCAT | LOG_TRACE | LOG_LOCAL_SOCKET),
				"ACTION: GPU - Setting GPU[%d] to %d\n",
				gpu, frequency);
			gpufreq_set(gpu, frequency, cdev_id);
			prev_gpu_max[gpu] = frequency;
		} else {
			dbgmsg("Using previous GPU[%d] frequency.\n", gpu);
		}
		pthread_mutex_unlock(&gpufreq_set_mtx);
		return 0;
	}
	max_freq = (int)strtol(gpu_freq_max[gpu], NULL, 10);

	pthread_mutex_lock(&gpufreq_set_mtx);

	gpu_freq_req[gpu] = frequency;

	if (!prev_gpu_max[gpu])
		prev_gpu_max[gpu] = max_freq;

	if ((gpu_freq_req[gpu] > 0) && (gpu_freq_req[gpu] < max_freq))
		max_freq = gpu_freq_req[gpu];

	if (max_freq != prev_gpu_max[gpu]) {
		info("ACTION: GPU - "
		     "Setting GPU[%d] to %d\n", gpu, max_freq);
		gpufreq_set(gpu, max_freq, cdev_id);
		prev_gpu_max[gpu] = max_freq;
	} else {
		dbgmsg("Using previous GPU[%d] frequency.\n", gpu);
	}

	pthread_mutex_unlock(&gpufreq_set_mtx);
	return 0;
}

/*===========================================================================
FUNCTION gpufreq_current_op

Query function to request current GPU operating frequency.

ARGUMENTS
	gpu - gpu id
	frequency - pointer to gpu current operating frequence to be returned

RETURN VALUE
	0 on success, else on failure.
===========================================================================*/
int gpufreq_current_op(int gpu, int *frequency)
{

	if (gpu >= num_gpus) {
		msg("%s: Invalid GPU ID %d", __func__, gpu);
		return -(EFAULT);
	}
	return gpufreq_get(gpu, frequency);
}

static pthread_mutex_t wlan_mtx = PTHREAD_MUTEX_INITIALIZER;

#if defined (SENSORS_8960)
#define WLAN_MITIGATION_SYSFS "/sys/devices/platform/wcnss_wlan.0/thermal_mitigation"
#else
#define WLAN_MITIGATION_SYSFS NULL
#endif

static char wlan_sysfs_node[MAX_PATH] = {0};

static void wlan_init_node_name(void)
{
	char temp[MAX_PATH] = {0};
	int ret = 0;

	if (WLAN_MITIGATION_SYSFS) {
		strlcpy(wlan_sysfs_node, WLAN_MITIGATION_SYSFS, MAX_PATH);
		return;
	}

	ret = get_sysdev_dt_dir_name(temp, "qcom,wcnss-wlan");
	if (!ret) {
		snprintf(wlan_sysfs_node, MAX_PATH, "%s%s", temp,
			 "thermal_mitigation");
	}
}

/*===========================================================================
FUNCTION wlan_request

Action function to request wlan throttling action.

ARGUMENTS
	level - 0 - MAX_WLAN_MITIGATION_LEVEL throttling level

RETURN VALUE
	0 on success, -1 on failure.
===========================================================================*/
int wlan_request(int level)
{
	int ret = -1;
	char buf[UINT_BUF_MAX] = {0};
	static int prev;
	static uint8_t init_done;

	if (!init_done) {
		wlan_init_node_name();
		init_done = 1;
	}

	if ('\0' == wlan_sysfs_node[0]) {
		msg("%s: Unsupported action on current target", __func__);
		return -1;
	}

	if (level < 0)
		level = 0;

	if (level > MAX_WLAN_MITIGATION_LEVEL)
		level = MAX_WLAN_MITIGATION_LEVEL;

	pthread_mutex_lock(&wlan_mtx);

	if (level != prev) {
		snprintf(buf, UINT_BUF_MAX, "%d", level);
		if (write_to_file(wlan_sysfs_node, buf, strlen(buf)) > 0) {
			dbgmsg("ACTION: WLAN - "
			     "Setting WLAN mitigation to %d\n", level);
			thermalmsg(LOG_LVL_DBG,
				(LOG_LOGCAT | LOG_LOCAL_SOCKET | LOG_TRACE),
				"%s:WLAN:%d\n", MITIGATION, level);
			prev = level;
			ret = 0;
		} else {
			msg("Unable to set WLAN mitigation to %d\n", level);
		}
	} else {
		dbgmsg("WLAN mitigation already at level %d\n", level);
		ret = 0;
	}

	pthread_mutex_unlock(&wlan_mtx);
	return ret;
}

#define HOTPLUG_KTM_NODE  "/sys/module/msm_thermal/core_control/cpus_offlined"
#define HOTPLUG_CPU_OFFLINE(cpu) (uint32_t)((offline_cpus >> cpu) & 0x1U)
#define HOTPLUG_CPU_UPDATE_BIT(cpu, offline) (((~(0x1U << cpu)) & offline_cpus) | \
					     ((offline)?(0x1U << cpu):(0)))

static pthread_mutex_t hotplug_mtx = PTHREAD_MUTEX_INITIALIZER;

static uint8_t hotplug_ktm_supported;
static uint8_t hotplug_ktm_disabled;
static device_clnt_handle ktm_dev_clnt;
static uint32_t offline_cpus;

/*===========================================================================
FUNCTION hotplug_ktm_request


Function for hotplugging CPUs.  This function should only be called
in one thread per hotplug core.

ARGUMENTS
	cpu
	offline

RETURN VALUE
	0 on success, -1 on failure.
===========================================================================*/
static int hotplug_ktm_request(int cpu, int offline)
{
	char buf[UINT_BUF_MAX] = {0};
	int ret = 0;
	uint32_t mask = HOTPLUG_CPU_UPDATE_BIT(cpu, offline);

	if (hotplug_ktm_disabled == 0) {
		info("%s: Defer call until KTM work loop is disabled.",
		       __func__);
		return 0;
	}

	dbgmsg("%s: write out %x\n", __func__, mask);

	snprintf(buf, UINT_BUF_MAX, "%d", mask);
	if (write_to_file(HOTPLUG_KTM_NODE, buf, strlen(buf)) !=
	    (int) strlen(buf)) {
		dbgmsg("%s: Error writing to KTM node\n", __func__);
		ret = -1;
	}
	return ret;
}

/*===========================================================================
FUNCTION hotplug_ktm_disable_cb


Function for syncing hotplug states between thermal engine and KTM after
KTM is disabled by thermal-engine.

ARGUMENTS
	clnt
	req
	notify_cb_data


RETURN VALUE
	None
===========================================================================*/
static void hotplug_ktm_disable_cb(device_clnt_handle clnt,
				   union device_request *req,
				   void *notify_cb_data)
{
	char buf[UINT_BUF_MAX] = {0};

	dbgmsg("%s: req.value %d.", __func__, req->value);

	/* We only care about the transition to 0*/
	if (req->value)
		return;

	pthread_mutex_lock(&hotplug_mtx);
	hotplug_ktm_disabled = 1;
	info("%s: write out %x\n", __func__, offline_cpus);

	snprintf(buf, UINT_BUF_MAX, "%d", offline_cpus);
	/* Make call to sync up all cores with KTM */
	if (write_to_file(HOTPLUG_KTM_NODE, buf, strlen(buf)) !=
	    (int) strlen(buf)) {
		msg("%s: Error writing to KTM node\n", __func__);
	}

	pthread_mutex_unlock(&hotplug_mtx);

	/* De-reg cb func */
	devices_manager_reg_notify(ktm_dev_clnt, NULL, NULL);
}

/*===========================================================================
FUNCTION hotplug_request


Function for hotplugging CPUs.  This function should only be called
in one thread per hotplug core. Request hotplug manually via sysfs.

ARGUMENTS
	cpu - CPU to hotplug
	offline

RETURN VALUE
	0 on success, -1 on failure.
===========================================================================*/
int hotplug_request(int cpu, int offline)
{
	int ret = 0;

	/* Make sure offline is either 1 or 0. */
	offline = (offline)?(1):(0);

	pthread_mutex_lock(&hotplug_mtx);
	if (((uint32_t)offline) != HOTPLUG_CPU_OFFLINE(cpu)) {
		if (hotplug_ktm_supported) {
			ret = hotplug_ktm_request(cpu, offline);
			if (!ret) {
				dbgmsg("ACTION: Hotplugged %s CPU[%d]\n",
				     offline ? "OFF" : "ON", cpu);
				offline_cpus = HOTPLUG_CPU_UPDATE_BIT(cpu, offline);
			} else {
				msg("Failed to hotplug CPU[%d]\n", cpu);
			}
		} else {
			ret = -1;
			msg("KTM hotplug unavailable\n");
		}
	} else {
		dbgmsg("Hotplug request already at CPU[%d] %s\n", cpu,
		     offline ? "OFF" : "ON");
	}
	pthread_mutex_unlock(&hotplug_mtx);
	return ret;
}

/*===========================================================================
FUNCTION hotplug_init

Hotplug init.  Determine if KTM nodes exist or not.  Register for
notifications on "kernel" device changes.

ARGUMENTS
	None

RETURN VALUE
	None
===========================================================================*/
void hotplug_init(void)
{
	char buf[UINT_BUF_MAX] = {0};
	int ret_val;

	if (read_line_from_file(HOTPLUG_KTM_NODE, buf, UINT_BUF_MAX) > 0) {
		ktm_dev_clnt = devices_manager_reg_clnt("kernel");
		if (ktm_dev_clnt == NULL) {
			msg("%s: Failed to create dev client.", __func__);
			return;
		}
		ret_val = devices_manager_reg_notify(ktm_dev_clnt,
						     hotplug_ktm_disable_cb,
						     NULL);
		if (ret_val < 0) {
			msg("%s: Failed to reg_notify dev client.", __func__);
			return;
		}
		hotplug_ktm_supported = 1;
	}
}

#ifdef ENABLE_MODEM_MITIGATION
static pthread_mutex_t cpr_band_mtx = PTHREAD_MUTEX_INITIALIZER;
#define CPR_BAND_SYSFS "/sys/module/msm_thermal/cpr_band/curr_cpr_band"

static uint8_t cpr_band_sysfs_supported;
static int curr_cpr_band, curr_cpr_lvl;
static int cpr_lvl_band_table[MAX_CPR_TEMPERATURE_BAND][2] = {
		{ 0, 4 }, /* Normal */
		{ 1, 3 }, /* cool */
		{ 2, 2 }, /* cold */
		{ 3, 1 }  /* critical cold */
	};

/*===========================================================================
FUNCTION cpr_band_request

Function for sending CPR temperature band.

ARGUMENTS
	request

RETURN VALUE
	0 on success, -1 on failure.
===========================================================================*/
int cpr_band_request(int request)
{
	int ret = -1, ret_sysfs = 1;
	char buf[UINT_BUF_MAX] = {0};

	if (request < 0)
		request = 0;

	if (request >= MAX_CPR_TEMPERATURE_BAND)
		request = MAX_CPR_TEMPERATURE_BAND - 1;

	pthread_mutex_lock(&cpr_band_mtx);
	if (request == curr_cpr_lvl) {
		dbgmsg("CPR already in temperature band:%d\n", request);
		ret = 0;
		goto handle_return;
	}

	/*
	 * Send the actual temperature band to RPM via KTM and the
	 * temperature level to Modem via QMI
	 */
	if (cpr_band_sysfs_supported) {
		snprintf(buf, UINT_BUF_MAX, "%d",
			cpr_lvl_band_table[request][1]);
		ret_sysfs = write_to_file(CPR_BAND_SYSFS, buf, strlen(buf));
	}
	ret = cpr_band_qmi_request(request);
	if (ret_sysfs > 0 && !ret) {
		thermalmsg(LOG_LVL_DBG,
			(LOG_LOGCAT | LOG_LOCAL_SOCKET | LOG_TRACE),
			"%s:CPR Temp band:%d\n", MITIGATION, request);
		curr_cpr_lvl = request;
		curr_cpr_band = cpr_lvl_band_table[request][1];
		ret = 0;
	} else {
		msg("Unable to set CPR band to %s\n", buf);
		goto handle_return;
	}

handle_return:
	pthread_mutex_unlock(&cpr_band_mtx);

	return ret;
}

/*===========================================================================
FUNCTION cpr_band_init

Function for reading initial CPR band.

ARGUMENTS
	request

RETURN VALUE
	none
===========================================================================*/
void cpr_band_init(void)
{
	char buf[UINT_BUF_MAX] = {0};

	if (read_line_from_file(CPR_BAND_SYSFS, buf, sizeof(buf)) > 0) {
		cpr_band_sysfs_supported = 1;
		dbgmsg("%s: Init CPR KTM temperature band supported: %d",
			__func__, atoi(buf));
		/*
		 * KTM starts conservatively by sending the mitigated
		 * temperature band to RPM. Reset that and then thermal-engine
		 * will determine the right temperature band based on current
		 * temperature and mitigation rule.
		 */
		curr_cpr_lvl = MAX_CPR_TEMPERATURE_BAND;
	} else {
		dbgmsg("%s: Could not read %s", __func__, CPR_BAND_SYSFS);
	}
	cpr_band_request(0);
}
#endif

static pthread_mutex_t vdd_restriction_mtx = PTHREAD_MUTEX_INITIALIZER;
#define VDD_RSTR_SYSFS "/sys/module/msm_thermal/vdd_restriction/enabled"

static uint8_t vdd_rstr_enabled_valid;
static uint8_t vdd_rstr_ktm_disabled;
static int vdd_rstr_enabled;
static device_clnt_handle vdd_rstr_ktm_dev_clnt;

static void vdd_rstr_ktm_disable_cb(device_clnt_handle clnt,
				union device_request *req,
				void *notify_cb_data)
{
	char buf[UINT_BUF_MAX] = {0};

	dbgmsg("%s: req.value %d", __func__, req->value);

	/* We only care about the transition to 0 */
	if (req->value)
		return;

	pthread_mutex_lock(&vdd_restriction_mtx);
	vdd_rstr_ktm_disabled = 1;

	dbgmsg("%s: vdd_rstr_enabled %d\n", __func__, vdd_rstr_enabled);

	snprintf(buf, UINT_BUF_MAX, "%d", vdd_rstr_enabled);
	/* Make call to sync up vdd restriction enable state */
	if (write_to_file(VDD_RSTR_SYSFS, buf, strlen(buf)) !=
			(int) strlen(buf))
		msg("%s: Error writing to KTM node\n", __func__);

	if (vdd_restrict_qmi_request(vdd_rstr_enabled) < 0)
		msg("%s: Error writing to QMI clients\n", __func__);

	pthread_mutex_unlock(&vdd_restriction_mtx);

	/* De-reg cb func */
	devices_manager_reg_notify(vdd_rstr_ktm_dev_clnt, NULL, NULL);

}

/*===========================================================================
FUNCTION vdd_rstr_init


Function for reading initial VDD RESTRICTION parameter.

ARGUMENTS
	request

RETURN VALUE
	none
===========================================================================*/
void vdd_rstr_init(void)
{
	int ret = -1;
	char buf[UINT_BUF_MAX] = {0};

	if (read_line_from_file(VDD_RSTR_SYSFS, buf, sizeof(buf)) > 0) {
		vdd_rstr_ktm_dev_clnt = devices_manager_reg_clnt("kernel");
		if (vdd_rstr_ktm_dev_clnt == NULL) {
			msg("%s: Failed to create dev client.", __func__);
			return;
		}

		ret = devices_manager_reg_notify(vdd_rstr_ktm_dev_clnt,
						vdd_rstr_ktm_disable_cb,
						NULL);
		if (ret < 0) {
			msg("%s: Failed to reg_notify dev client.", __func__);
			return;
		}

		vdd_rstr_enabled_valid = 1;
		info("%s: Init KTM VDD RSTR enabled: %d", __func__, atoi(buf));
	} else
		info("%s: Could not read %s", __func__, VDD_RSTR_SYSFS);

}

/*===========================================================================
FUNCTION vdd_restriction_request


Function for enabling/disabling VDD RESTRICTION.

ARGUMENTS
	request


RETURN VALUE
	0 on success, -1 on failure.
===========================================================================*/
int vdd_restriction_request(int request)
{
	int ret = -1;
	char buf[UINT_BUF_MAX] = {0};

	if (request < 0)
		request = 0;

	if (request != 0)
		request = 1;

	if (vdd_rstr_enabled_valid == 0) {
		msg("VDD_RSTR_SYSFS doesn't exit\n");
		return 0;
	}

	if (vdd_rstr_ktm_disabled == 0) {
		info("%s:Defer call until KTM work loop is disabled. request:%d",
				__func__, request);
		vdd_rstr_enabled = request;
		return 0;
	}
	pthread_mutex_lock(&vdd_restriction_mtx);

	if (request == vdd_rstr_enabled) {
		dbgmsg("VDD_RESTRICTION enabled was:%d\n",
		       request);
		ret = 0;
		goto handle_return;
	}

	snprintf(buf, UINT_BUF_MAX, "%d", request);
	if ((write_to_file(VDD_RSTR_SYSFS, buf, strlen(buf)) > 0) &&
	    (vdd_restrict_qmi_request(request) == 0)) {
		dbgmsg("ACTION: Set VDD_RESTRICTION enabled to %s\n", buf);
		thermalmsg(LOG_LVL_DBG,
			(LOG_LOGCAT | LOG_LOCAL_SOCKET | LOG_TRACE),
			"%s:VDD:%s\n", MITIGATION, buf);
		vdd_rstr_enabled = request;
		ret = 0;
	} else {
		msg("Unable to set VDD_RSTR to %s\n", buf);
		goto handle_return;
	}

handle_return:
	pthread_mutex_unlock(&vdd_restriction_mtx);

	return ret;
}

static pthread_mutex_t kernel_disable_mtx = PTHREAD_MUTEX_INITIALIZER;
/*===========================================================================
FUNCTION kernel_mitigation_request

Function for requesting kernel mitigation.

ARGUMENTS
	request

RETURN VALUE
	0 on success, -1 on failure.
===========================================================================*/
int kernel_mitigation_request(int request)
{
	int ret = -1;

	pthread_mutex_lock(&kernel_disable_mtx);

	if (request == 0) {
		/* Disable kernel thermal module and take over */
		write_to_file("/sys/module/msm_thermal/parameters/enabled",
			      "N" , strlen("N") + 1);
		dbgmsg("KERNEL mitigation disabled\n");
	} else
		dbgmsg("KERNEL request to keep mitigation enabled\n");

	pthread_mutex_unlock(&kernel_disable_mtx);
	return ret;
}

/*===========================================================================
FUNCTION camera_request

Action function to request camera throttling action.

ARGUMENTS
	level - 0 - MAX_CAMERA_MITIGATION_LEVEL throttling level

RETURN VALUE
	0 on success, -1 on failure.
===========================================================================*/
int camera_request(int level)
{
	int ret = -1;
	static pthread_mutex_t camera_mtx = PTHREAD_MUTEX_INITIALIZER;
	static int prev_camera = 0;

	if (level < 0)
		level = 0;

	if (level > MAX_CAMERA_MITIGATION_LEVEL)
		level = MAX_CAMERA_MITIGATION_LEVEL;

	pthread_mutex_lock(&camera_mtx);

	if (level != prev_camera) {
		if (thermal_server_notify_clients("camera", level) < 0) {
			msg("Unable to set CAMERA mitigation to %d\n", level);
		} else {
			dbgmsg("ACTION: CAMERA - "
			     "Setting CAMERA mitigation to %d\n", level);
			thermalmsg(LOG_LVL_DBG,
				(LOG_LOGCAT | LOG_LOCAL_SOCKET | LOG_TRACE),
				"%s:Camera:%d\n", MITIGATION, level);
			prev_camera = level;
			ret = 0;
		}
	} else {
		dbgmsg("CAMERA mitigation already at level %d\n", level);
		ret = 0;
	}

	pthread_mutex_unlock(&camera_mtx);
	return ret;
}

/*===========================================================================
FUNCTION camcorder_request

Action function to request camera throttling action.

ARGUMENTS
	level - 0 - MAX_CAMCORDER_MITIGATION_LEVEL throttling level

RETURN VALUE
	0 on success, -1 on failure.
===========================================================================*/
int camcorder_request(int level)
{
	int ret = -1;
	static pthread_mutex_t camcorder_mtx = PTHREAD_MUTEX_INITIALIZER;
	static int prev_camcorder = 0;

	if (level < 0)
		level = 0;

	if (level > MAX_CAMCORDER_MITIGATION_LEVEL)
		level = MAX_CAMCORDER_MITIGATION_LEVEL;

	pthread_mutex_lock(&camcorder_mtx);

	if (level != prev_camcorder) {
		if (thermal_server_notify_clients("camcorder", level) < 0) {
			msg("Unable to set CAMCORDER mitigation to %d\n", level);
		} else {
			dbgmsg("ACTION: CAMCORDER - "
			     "Setting CAMCORDER mitigation to %d\n", level);
			thermalmsg(LOG_LVL_DBG,
				(LOG_LOGCAT | LOG_LOCAL_SOCKET | LOG_TRACE),
				"%s:Camcorder:%d\n", MITIGATION, level);
			prev_camcorder = level;
			ret = 0;
		}
	} else {
		dbgmsg("CAMCORDER mitigation already at level %d\n", level);
		ret = 0;
	}

	pthread_mutex_unlock(&camcorder_mtx);
	return ret;
}

#define VDD_DIG_SW_MODE_NODE "/sys/module/msm_thermal/pmic_sw_mode/vdd_dig/mode"
static pthread_mutex_t vdd_dig_sw_mode_mtx = PTHREAD_MUTEX_INITIALIZER;
static uint8_t vdd_dig_sw_mode_supported;
static uint8_t vdd_dig_sw_mode_ktm_disabled;
static device_clnt_handle vdd_dig_sw_mode_dev_clnt;
static uint8_t vdd_dig_sw_mode;

/*===========================================================================
FUNCTION vdd_dig_sw_mode_ktm_disable_cb


Function for syncing vdd_dig sw_mode states between thermal engine and KTM after
KTM is disabled by thermal-engine.

ARGUMENTS
	clnt
	req
	notify_cb_data


RETURN VALUE
	None
===========================================================================*/
static void vdd_dig_sw_mode_ktm_disable_cb(device_clnt_handle clnt,
					   union device_request *req,
					   void *notify_cb_data)
{
	char buf[UINT_BUF_MAX] = {0};

	dbgmsg("%s: req.value %d.", __func__, req->value);

	/* We only care about the transition to 0 */
	if (req->value)
		return;

	pthread_mutex_lock(&vdd_dig_sw_mode_mtx);
	vdd_dig_sw_mode_ktm_disabled = 1;
	info("%s: write out %d\n", __func__, vdd_dig_sw_mode);

	snprintf(buf, UINT_BUF_MAX, "%d", vdd_dig_sw_mode);
	/* Make call to sync up sw_mode */
	if (write_to_file(VDD_DIG_SW_MODE_NODE, buf, strlen(buf)) !=
	    (int) strlen(buf)) {
		msg("%s: Error writing to sw_mode node\n", __func__);
	}
	pthread_mutex_unlock(&vdd_dig_sw_mode_mtx);

	/* De-reg cb func */
	devices_manager_reg_notify(vdd_dig_sw_mode_dev_clnt, NULL, NULL);
}

/*===========================================================================
FUNCTION vdd_dig_sw_mode_request


Function for setting vdd_dig rail sw_mode.

ARGUMENTS
	mode

RETURN VALUE
	0 on success, -1 on failure.
===========================================================================*/
int vdd_dig_sw_mode_request(int mode)
{
	int ret = 0;

	/* Force mode to either 0(automode) or 2(normal). */
	mode = (mode)?(2):(0);

	pthread_mutex_lock(&vdd_dig_sw_mode_mtx);
	if (vdd_dig_sw_mode_ktm_disabled == 0) {
		info("%s: Defer call until KTM work loop is disabled,"
		     " mode %d %s.", __func__, mode, mode ? "NORMAL" : "AUTO");
		vdd_dig_sw_mode = (uint8_t)mode;
		goto handle_return;
	}

	if (((uint8_t)mode) != vdd_dig_sw_mode) {
		char buf[UINT_BUF_MAX] = {0};

		if (vdd_dig_sw_mode_supported) {
			snprintf(buf, UINT_BUF_MAX, "%d", mode);
			if (write_to_file(VDD_DIG_SW_MODE_NODE, buf,
					  strlen(buf)) != (int) strlen(buf))
				ret = -EFAULT;
		} else
			msg("%s: sw_mode node not supported mode %d", __func__,
			    mode);

		if (!ret) {
			info("ACTION: VDD DIG SW_MODE %d %s", mode,
			     mode ? "NORMAL" : "AUTO");
			vdd_dig_sw_mode = (uint8_t)mode;
		} else
			msg("Failed VDD DIG SW_MODE %d %s", mode,
			    mode ? "NORMAL" : "AUTO");
	} else
		info("VDD DIG SW_MODE Already %d %s", mode,
		     mode ? "NORMAL" : "AUTO");

handle_return:
	pthread_mutex_unlock(&vdd_dig_sw_mode_mtx);
	return ret;
}

/*===========================================================================
FUNCTION vdd_dig_sw_mode_init

Determine if sw_mode node exists or not.  Register for notifications on
"kernel" device changes.

ARGUMENTS
	None

RETURN VALUE
	None
===========================================================================*/
void vdd_dig_sw_mode_init(void)
{
	char buf[UINT_BUF_MAX] = {0};
	int ret_val;

	if (read_line_from_file(VDD_DIG_SW_MODE_NODE, buf, UINT_BUF_MAX) > 0) {
		vdd_dig_sw_mode_dev_clnt = devices_manager_reg_clnt("kernel");
		if (vdd_dig_sw_mode_dev_clnt == NULL) {
			msg("%s: Failed to create dev client.", __func__);
			return;
		}
		ret_val = devices_manager_reg_notify(vdd_dig_sw_mode_dev_clnt,
						vdd_dig_sw_mode_ktm_disable_cb,
						NULL);
		if (ret_val < 0) {
			msg("%s: Failed to reg_notify dev client.", __func__);
			return;
		}
		vdd_dig_sw_mode_supported = 1;
	}
}

#define OPT_CURR_REQ_NODE "/sys/module/msm_thermal/opt_curr_req/vdd-dig/mode"
static pthread_mutex_t opt_curr_req_mtx = PTHREAD_MUTEX_INITIALIZER;
static uint8_t opt_curr_req_supported;
static uint8_t opt_curr_req_ktm_disabled;
static device_clnt_handle opt_curr_req_dev_clnt;
static uint8_t opt_curr_req;

/*===========================================================================
FUNCTION opt_curr_req_ktm_disable_cb

Function for syncing optimum current request states between thermal engine
and KTM after KTM is disabled by thermal-engine.

ARGUMENTS
	clnt
	req
	notify_cb_data

RETURN VALUE
	None
===========================================================================*/
static void opt_curr_req_ktm_disable_cb(device_clnt_handle clnt,
					   union device_request *req,
					   void *notify_cb_data)
{
	char buf[UINT_BUF_MAX] = {0};

	dbgmsg("%s: req.value %d.", __func__, req->value);

	/* We only care about the transition to 0 */
	if (req->value)
		return;

	pthread_mutex_lock(&opt_curr_req_mtx);
	opt_curr_req_ktm_disabled = 1;
	info("%s: write out %d\n", __func__, opt_curr_req);

	snprintf(buf, UINT_BUF_MAX, "%d", opt_curr_req);
	/* Make call to sync up optimum current request state*/
	if (write_to_file(OPT_CURR_REQ_NODE, buf, strlen(buf)) !=
	    (int) strlen(buf)) {
		msg("%s: Error writing to opt curr request node\n", __func__);
	}
	pthread_mutex_unlock(&opt_curr_req_mtx);

	/* De-reg cb func */
	devices_manager_reg_notify(opt_curr_req_dev_clnt, NULL, NULL);
}

/*===========================================================================
FUNCTION optimum_current_request

Function for setting vdd_dig rail sw_mode.

ARGUMENTS
	mode

RETURN VALUE
	0 on success, -1 on failure.
===========================================================================*/
int optimum_current_request(int mode)
{
	int ret = 0;

	mode = (mode)?(!!mode):(0);

	pthread_mutex_lock(&opt_curr_req_mtx);
	if (opt_curr_req_ktm_disabled == 0) {
		info("%s: Defer call until KTM work loop is disabled,"
		     " mode %d.", __func__, mode);
		opt_curr_req = (uint8_t)mode;
		goto handle_return;
	}

	if (((uint8_t)mode) != opt_curr_req) {
		char buf[UINT_BUF_MAX] = {0};

		if (opt_curr_req_supported) {
			snprintf(buf, UINT_BUF_MAX, "%d", mode);
			if (write_to_file(OPT_CURR_REQ_NODE, buf,
					  strlen(buf)) != (int) strlen(buf))
				ret = -EFAULT;
		} else
			msg("%s: opt curr request feature not supported.",
				__func__);

		if (!ret) {
			info("ACTION: OPT_CURR_REQ %d", mode);
			opt_curr_req = (uint8_t)mode;
		} else
			msg("Failed OPT_CURR_REQ %d", mode);
	} else
		info("OPT CURR REQ Already %d", mode);

handle_return:
	pthread_mutex_unlock(&opt_curr_req_mtx);
	return ret;
}

/*===========================================================================
FUNCTION opt_curr_req_init

Determine if opt curr req node exists or not.  Register for notifications on
"kernel" device changes.

ARGUMENTS
	None

RETURN VALUE
	None
===========================================================================*/
void opt_curr_req_init(void)
{
	char buf[UINT_BUF_MAX] = {0};
	int ret_val;

	if (read_line_from_file(OPT_CURR_REQ_NODE, buf, UINT_BUF_MAX) > 0) {
		opt_curr_req_dev_clnt = devices_manager_reg_clnt("kernel");
		if (opt_curr_req_dev_clnt == NULL) {
			msg("%s: Failed to create dev client.", __func__);
			return;
		}
		ret_val = devices_manager_reg_notify(opt_curr_req_dev_clnt,
						opt_curr_req_ktm_disable_cb,
						NULL);
		if (ret_val < 0) {
			msg("%s: Failed to reg_notify dev client.", __func__);
			return;
		}
		opt_curr_req_supported = 1;
	}
}

#define MDP_MITIGATION_SYSFS "/sys/devices/virtual/graphics/fb0/msm_fb_thermal_level"
/*===========================================================================
FUNCTION mdp_request

Action function to request mdp throttling action.

ARGUMENTS
	level - 0 - MAX_MDP_MITIGATION_LEVEL throttling level

RETURN VALUE
	0 on success, -1 on failure.
===========================================================================*/
int mdp_request(int level)
{
	int ret = -1;
	char buf[UINT_BUF_MAX] = {0};
	static int prev;
	static pthread_mutex_t mdp_mtx = PTHREAD_MUTEX_INITIALIZER;

	if (level < 0)
		level = 0;

	if (level > MAX_MDP_MITIGATION_LEVEL)
		level = MAX_MDP_MITIGATION_LEVEL;

	pthread_mutex_lock(&mdp_mtx);

	if (level != prev) {
		snprintf(buf, UINT_BUF_MAX, "%d", level);
		if (write_to_file(MDP_MITIGATION_SYSFS, buf, strlen(buf)) > 0) {
			dbgmsg("ACTION: MDP - "
			     "Setting MDP mitigation to %d\n", level);
			thermalmsg(LOG_LVL_DBG,
				(LOG_LOGCAT | LOG_LOCAL_SOCKET | LOG_TRACE),
				"%s:MDP:%d\n", MITIGATION, level);
			prev = level;
			ret = 0;
		} else {
			msg("Unable to set MDP mitigation to %d\n", level);
		}
	} else {
		dbgmsg("MDP mitigation already at level %d\n", level);
		ret = 0;
	}

	pthread_mutex_unlock(&mdp_mtx);
	return ret;
}

#define VENUS_MITIGATION_NODE  "thermal_level"
#define VENUS_PLATFORM_DRIVER  "qcom,vidc"
/*===========================================================================
FUNCTION venus_request

Action function to request venus throttling action.

ARGUMENTS
	level - 0 - MAX_VENUS_MITIGATION_LEVEL throttling level

RETURN VALUE
	0 on success, -1 on failure.
===========================================================================*/
int venus_request(int level)
{
	int ret = -1;
	char buf[UINT_BUF_MAX] = {0};
	char temp[MAX_PATH] = {0};
	static int prev;
	static uint8_t venus_init_done;
	static pthread_mutex_t venus_mtx = PTHREAD_MUTEX_INITIALIZER;
	static char venus_sysfs_node[MAX_PATH] = {0};

	if (!venus_init_done) {
		ret = get_sysdev_dt_dir_name(temp, VENUS_PLATFORM_DRIVER);
		if (!ret) {
			snprintf(venus_sysfs_node, MAX_PATH, "%s%s", temp,
			VENUS_MITIGATION_NODE);
		} else {
			msg("%s: Unsupported action on current target",
			__func__);
		}
		venus_init_done = 1;
	}

	if (venus_sysfs_node[0] == 0) {
		msg("%s: Unsupported action on current target", __func__);
		return -1;
	}

	if (level < 0)
		level = 0;

	if (level > MAX_VENUS_MITIGATION_LEVEL)
		level = MAX_VENUS_MITIGATION_LEVEL;

	pthread_mutex_lock(&venus_mtx);

	if (level != prev) {
		snprintf(buf, UINT_BUF_MAX, "%d", level);
		if (write_to_file(venus_sysfs_node, buf, strlen(buf)) > 0) {
			dbgmsg("ACTION: VENUS - "
			     "Setting VENUS mitigation to %d\n", level);
			thermalmsg(LOG_LVL_DBG,
				(LOG_LOGCAT | LOG_LOCAL_SOCKET | LOG_TRACE),
				"%s:VENUS:%d\n", MITIGATION, level);
			prev = level;
			ret = 0;
		} else {
			msg("Unable to set VENUS mitigation to %d\n", level);
		}
	} else {
		dbgmsg("VENUS mitigation already at level %d\n", level);
		ret = 0;
	}

	pthread_mutex_unlock(&venus_mtx);
	return ret;
}

#ifdef ENABLE_HVX_MITIGATION

#define HVX_THERM_CLT_ID 2
/*===========================================================================
FUNCTION hvx_mitigation_init

Init function to request initial level to HVX.

ARGUMENTS

RETURN VALUE
===========================================================================*/
void hvx_mitigation_init(void)
{
	int ret = -1;
	ret = sysmonhvxthrottle_update_thermal_enabled(0, 0, HVX_THERM_CLT_ID);
	if (ret)
		msg("Failed to reset HVX mitigation, ret:%d\n", ret);
}

/*===========================================================================
FUNCTION hvx_request

Action function to request HVX throttling action.

ARGUMENTS
	level - 0 - MAX_HVX_HW/SW_MITIGATION_LEVEL throttling level
	dev   - Device type either HVX_HW_DEV or HVX_SW_DEV

RETURN VALUE
	0 on success, -1 on failure.
===========================================================================*/
int hvx_request(int level, enum hvx_dev_type dev)
{
	int ret = -1;
	static unsigned int cur_hvx[HVX_DEV_MAX];
	static pthread_mutex_t hvx_mtx = PTHREAD_MUTEX_INITIALIZER;

	if (level < 0)
		level = 0;

	if (dev == HVX_HW_DEV) {
		if (level > MAX_HVX_HW_MITIGATION_LEVEL)
			level = MAX_HVX_HW_MITIGATION_LEVEL;
	} else {
		if (level > MAX_HVX_SW_MITIGATION_LEVEL)
			level = MAX_HVX_SW_MITIGATION_LEVEL;
	}

	pthread_mutex_lock(&hvx_mtx);

	if ((unsigned int)level != cur_hvx[dev]) {
		if (dev == HVX_HW_DEV)
			ret = sysmonhvxthrottle_update_thermal_enabled(
				(unsigned int)level,
				cur_hvx[HVX_SW_DEV], HVX_THERM_CLT_ID);
		else
			ret = sysmonhvxthrottle_update_thermal_enabled(
				cur_hvx[HVX_HW_DEV], (unsigned int)level,
				HVX_THERM_CLT_ID);

		if (!ret) {
			cur_hvx[dev] = (unsigned int)level;
			dbgmsg("ACTION: %s - "
			     "Setting HVX mitigation to %d\n",
			     dev == HVX_HW_DEV ?
				HVX_HW_DEV_NAME : HVX_SW_DEV_NAME, level);
			thermalmsg(LOG_LVL_DBG,
				(LOG_LOGCAT | LOG_LOCAL_SOCKET | LOG_TRACE),
				"%s:%s:%d\n", MITIGATION,
				dev == HVX_HW_DEV ? HVX_HW_DEV_NAME :
				HVX_SW_DEV_NAME, level);
		} else {
			msg("Unable to set %s mitigation to %d, ret:%d\n",
				dev == HVX_HW_DEV ?
				HVX_HW_DEV_NAME : HVX_SW_DEV_NAME, level, ret);
		}
	} else {
		dbgmsg("%s mitigation already at level %d\n",
			dev == HVX_HW_DEV ? HVX_HW_DEV_NAME : HVX_SW_DEV_NAME,
				level);
		ret = 0;
	}

	pthread_mutex_unlock(&hvx_mtx);

	return ret;
}
#endif /* ENABLE_HVX_MITIGATION */
