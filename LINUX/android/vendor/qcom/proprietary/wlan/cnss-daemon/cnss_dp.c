/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <dlfcn.h>
#include "nl_loop.h"
#include "cnss_dp.h"
#include "debug.h"
#include "wlan_msg.h"
#include <cutils/properties.h>
#include <cutils/sockets.h>

#define MAX_IFACE_NUM_QUEUE      4
#define MAX_IFACE_NAME_BUF_SIZE  16
#define MAX_FILE_NAME_BUF_SIZE   80
#define SWAP_NIBBLE(data) (((data << 4) | (data >> 4)) & 0xff)

enum wlan_tp_level {
	WLAN_SVC_TP_NONE,
	WLAN_SVC_TP_LOW,
	WLAN_SVC_TP_MEDIUM,
	WLAN_SVC_TP_HIGH,
};

/**
 * struct wlan_rps_data - structure to send RPS info to cnss-daemon
 * @ifname:         interface name for which the RPS data belongs to
 * @num_queues:     number of rx queues for which RPS data is being sent
 * @cpu_map_list:   array of cpu maps for different rx queues supported by
                    the wlan driver
 *
 * The structure specifies the format of data exchanged between wlan
 * driver and cnss-daemon. On receipt of the data, cnss-daemon is expected
 * to apply the 'cpu_map' for each rx queue belonging to the interface 'ifname'
 */
struct wlan_rps_data {
	char ifname[IFNAMSIZ];
	uint16_t num_queues;
	uint16_t cpu_map_list[WLAN_SVC_IFACE_NUM_QUEUES];
};

/*  tcp_limit_output_bytes for low throughput scenarios */
#define WLAN_TCP_LIMIT_OUTPUT_BYTES_LOW "506072"

/*  tcp_limit_output_bytes for high throughput scenarios */
#define WLAN_TCP_LIMIT_OUTPUT_BYTES_HIGH "4048579"
static char *rps_cpu_map[MAX_IFACE_NUM_QUEUE] = {"e", "e", "e", "e"};

/* Indicate to enable TCP delayed ack in TPUT indication */
#define TCP_DEL_ACK_IND	(1 << 0)
/* Indicate to enable TCP advance window scaling in TPUT indication */
#define TCP_ADV_WIN_SCL	(1 << 1)

/* network performance tuner support */
#define MAX_NPT_CMD_LEN                 128
#define MAX_NPT_RESP_LEN                128
#define NPT_CLIENT_NAME                 "wlan"
#define NPT_DEFAULT_SEQUENCE_NUM        0
#define NPT_MIN_RESULT_CODE             200
#define NPT_MAX_RESULT_CODE             600
#define NPT_OK_RESULT_CODE              200
#define NPT_RESULT_CODE_STR_LEN         3

static int has_npt;

struct wlan_rx_tp_data {
	enum wlan_tp_level level;
	uint16_t rx_tp_flags;
};

static int wlan_service_process_dp_msg(int type, void *data, int len);
static struct ind_handlers {
	int ind;
	nl_loop_ind_handler handler;
	void *user_data;
} dp_handlers[] = {
	{
		.ind = WLAN_MSG_WLAN_TP_IND,
		.handler = wlan_service_process_dp_msg,
	},
	{
		.ind = WLAN_MSG_WLAN_TP_TX_IND,
		.handler = wlan_service_process_dp_msg,
	},
	{
		.ind = WLAN_MSG_RPS_ENABLE_IND,
		.handler = wlan_service_process_dp_msg,
	},
	{
		.ind = WLAN_MSG_CORE_MINFREQ,
		.handler = wlan_service_process_dp_msg,
	}
};

/**
 * struct wlan_core_minfreq - msg to [re]set the min freq of a set of cores
 * @magic:            signature token: 0xBABA
 * @reserved:         unused for now
 * @coremask:         bitmap of cores (16 bits) bit0=CORE0, bit1=CORE1, ...
 *                     coremask is ONLY valid for set command
 *                     valid values: 0xf0, or 0x0f
 * @freq:             frequency in KH
 *                     >  0: "set to the given frequency"
 *                     == 0: "free; remove the lock"
 *
 * Msg structure passed by the driver to cnss-daemon.
 *
 * Semantical Alert:
 *   There can be only one outstanding lock, even for different masks.
 */
struct wlan_core_minfreq {
	uint16_t magic;
	uint16_t reserved;
	uint16_t coremask;
	uint16_t freq;
};

/**
 * perf client code:
 *
 * cnssdaemon is a client for QC perf daemon. Through perf daamon cnssd can
 * vote for system performance parameters supported by perfd.
 * Overall scheme is as follows:
 * - wlan.ko (the driver module) detects a condition where a system parameter
 *   needs to be modified.
 * - it sends a netlink message to cnssdaemon. Note that the driver does not
 *   get to know whether or not the message has been processed successfully
 *   at the receiving end (or even whether or not the message was delivered)
 * - cnss-daemon acts as a client to qc perf daemon and sends a request to
 *   perf daemon. perf daemon consolidates all such votes and makes the
 *   appropriate request through sysfs.
 *
 * All perf-client code may be moved into its own file in the future.
 *
 * The way a perf client works:
 * - during initialization:
 *   initializes perf_lock_acq and perf_lock_rel symbols through dlsym
 * - when the "set request" arrives:
 *   calls perf_lock_acq to send the request to perfd. This call also
 *   initializes perf_lock_handle, which will be used tpo release/remove
 *   the call,
 * - when the "reset request" arrives:
 *   releases the perf_lock, using the handle from the _acq call.
 *
 * Current use-cases:
 * 1- When the driver in in high-throughput mode in datapath (NAPI variant)
 *    it requests for the core frequency floor to be set to a higher than
 *    default number. This is because current DCVS algorithm does not work
 *    well with power-optimized NAPI execution pattern and thinks that the CPU
 *    is not busy enough to kick the frequency up.
 *    When the driver goes to low-throughput mode (from high-throughput mode)
 *    it sends another message and releases the lock.
 */
static int (*perf_lock_acq)(int handle, int duration, int list[], int numargs);
static int (*perf_lock_rel)(int handle);

static int cnssd_perfd_client_start()
{
	int rc = 0;
	char opt_lib_path[PATH_MAX] = {0};
	static void *qcopt_handle;

	wsvc_printf_dbg("-> %s", __func__);
	if (!property_get("ro.vendor.extension_library", opt_lib_path, NULL)) {
		wsvc_printf_err("%s: failed to get vendor lib", __func__);
		rc = -1;
		goto pcs_ret;
	}
	qcopt_handle = dlopen(opt_lib_path, RTLD_NOW);
	if (!qcopt_handle) {
		wsvc_printf_err("%s: failed to open dllib", __func__);
		rc = -2;
		goto pcs_ret;
	}
	perf_lock_acq = dlsym(qcopt_handle, "perf_lock_acq");
	if (!perf_lock_acq) {
		wsvc_printf_err("%s: failed to dlsym acq", __func__);
		rc = -3;
		goto pcs_ret;
	}
	perf_lock_rel = dlsym(qcopt_handle, "perf_lock_rel");
	if (!perf_lock_rel) {
		wsvc_printf_err("%s: failed to dlsym rel", __func__);
		rc = -4;
	}

pcs_ret:
	wsvc_printf_dbg("%d <- %s", rc, __func__);
	return rc;
}

int wlan_dp_service_start()
{
	int i;
	for (i = 0; i < (int)ARRAY_SIZE(dp_handlers); i++) {
		if (nl_loop_register(dp_handlers[i].ind,
				     dp_handlers[i].handler,
				     NULL)) {
			wsvc_printf_err("Failed to register: %d, %x", i,
					dp_handlers[i].ind);
			return -1;
		}
	}

	if (cnssd_perfd_client_start()) {
		wsvc_printf_err("Failed to become a perfd client; proceeding.");
		/* continue even if this op fails */
	}

	has_npt = property_get_bool("persist.vendor.wigig.npt.enable", 0);
	return 0;
}

void wlan_dp_service_stop(void)
{
	int i = 0;
	for (i = 0; i < (int)ARRAY_SIZE(dp_handlers); i++)
		nl_loop_unregister(dp_handlers[i].ind);
}

/**
 * wlan_service_update_sys_param()
 * @path: path on the file system to be modified
 * @str:  value to be written to the path
 *
 * Generic function to update a system parameter
 * Return: 0 if write is successful
 *        -1 if write is failure
 */
int wlan_service_update_sys_param(const char* path, const char* str)
{
	int ret = 0;
	FILE *fp;
	fp = fopen(path, "w");

	if (fp == NULL) {
		wsvc_printf_err("%s: unable to open %s",
			__func__, path);
		return -1;
	}

	wsvc_printf_dbg("%s: %s %s", __func__,  path, str);

	ret = fputs(str, fp);
	fclose(fp);

	if (ret < 0) {
		wsvc_printf_err("%s: failed to write %s to %s",
			__func__, str, path);
		return -1;
	}

	return 0;
}

/**
 * wlan_service_run_npt_cmd(): Runs a network performance tuner
 * command using socket
 * @cmd: the command
 *
 * Return: 0 command completed successfully
 *         negative number on failure
 */
static int wlan_service_run_npt_cmd(const char *cmd)
{
	int sock, rc, i, offset, code = 0;
	char ncmd[MAX_NPT_CMD_LEN],
	     resp[MAX_NPT_RESP_LEN + NPT_RESULT_CODE_STR_LEN + 1],
	     tmp[NPT_RESULT_CODE_STR_LEN + 1];

	sock = socket_local_client("wigignpt",
				   ANDROID_SOCKET_NAMESPACE_RESERVED,
				   SOCK_STREAM);
	if (sock < 0) {
		wsvc_printf_err("%s failed to create npt socket, error %d",
				__func__, sock);
		return sock;
	}

	/* use sequence number 0 as we only send one command on the socket */
	snprintf(ncmd, MAX_NPT_CMD_LEN, "%d %s",
		 NPT_DEFAULT_SEQUENCE_NUM, cmd);
	if (write(sock, ncmd, strlen(ncmd) + 1) < 0) {
		rc = -errno;
		close(sock);
		wsvc_printf_err("%s failed to write npt socket, error %d",
				__func__, rc);
		return rc;
	}

	rc = read(sock, resp, MAX_NPT_RESP_LEN);
	if (rc < 0) {
		rc = -errno;
		close(sock);
		wsvc_printf_err("%s failed to read npt socket, error %d",
				__func__, rc);
		return rc;
	}

	close(sock);

	/* look for response - line beginning with a number between 200-600 */
	offset = 0;
	for (i = 0; i < rc; i++) {
		if (resp[i] == '\0') {
			strlcpy(tmp, resp + offset, sizeof(tmp));
			tmp[NPT_RESULT_CODE_STR_LEN] = 0;
			code = atoi(tmp);
			if (code >= NPT_MIN_RESULT_CODE &&
			    code <= NPT_MAX_RESULT_CODE)
				break;
			offset = i + 1;
		}
	}

	return (code == NPT_OK_RESULT_CODE) ? 0 : -1;
}

static int wlan_service_set_tuning_parameter(const char* param,
					     const char *value)
{
	char cmd[MAX_NPT_CMD_LEN];

	snprintf(cmd, sizeof(cmd), "perftuner set %s %s %s\n",
		param, NPT_CLIENT_NAME, value);
	return wlan_service_run_npt_cmd(cmd);
}

int wlan_service_set_tcp_use_userconfig(const char *str)
{

	return has_npt ?
		wlan_service_set_tuning_parameter(
			"tcp_use_userconfig", str) :
		wlan_service_update_sys_param(
			"/proc/sys/net/ipv4/tcp_use_userconfig", str);
}

int wlan_service_set_adv_win_scale(const char *str)
{
	return wlan_service_update_sys_param(
		"/proc/sys/net/ipv4/tcp_adv_win_scale",
		str);
}

int wlan_service_set_tcp_delack_seg(const char *str)
{
	return has_npt ?
		wlan_service_set_tuning_parameter(
			"tcp_delack_seg", str) :
		wlan_service_update_sys_param(
			"/proc/sys/net/ipv4/tcp_delack_seg", str);
}

int wlan_service_set_tcp_limit_output_bytes (const char* str)
{
	return has_npt ?
		wlan_service_set_tuning_parameter(
			"tcp_limit_output_bytes", str) :
		wlan_service_update_sys_param (
			"/proc/sys/net/ipv4/tcp_limit_output_bytes", str);
}

int wlan_service_update_tcp_rx_params(void *data, int len)
{
	int ret = 0;
	struct wlan_rx_tp_data *rx_tp_data = data;
	int level, rx_tp_flags;

	if (len < (int)sizeof(struct wlan_rx_tp_data)) {
		wsvc_printf_err("%s input message of insufficient length %d",
				__func__, len);
		return -1;
	}
	level = rx_tp_data->level;
	rx_tp_flags = rx_tp_data->rx_tp_flags;

	switch (level) {
	case WLAN_SVC_TP_LOW:
		if (rx_tp_flags & TCP_ADV_WIN_SCL)
			wlan_service_set_adv_win_scale("2");

		if (rx_tp_flags & TCP_DEL_ACK_IND)
			wlan_service_set_tcp_use_userconfig("0");
		break;
	case WLAN_SVC_TP_MEDIUM:
	case WLAN_SVC_TP_HIGH:
		if (rx_tp_flags & TCP_ADV_WIN_SCL)
			wlan_service_set_adv_win_scale("1");

		if (rx_tp_flags & TCP_DEL_ACK_IND) {
			ret = wlan_service_set_tcp_use_userconfig("1");
			if (ret == 0)
				ret = wlan_service_set_tcp_delack_seg("20");
		}
		break;
	default:
		wsvc_printf_err("%s: Unsupported throughput value %d",
				__func__, level);
		break;
	}

	return 0;
}

int wlan_service_update_tcp_tx_params(int tp_level)
{
	switch (tp_level) {
	case WLAN_SVC_TP_LOW:
		wlan_service_set_tcp_limit_output_bytes(
			WLAN_TCP_LIMIT_OUTPUT_BYTES_LOW);
		break;
	case WLAN_SVC_TP_MEDIUM:
	case WLAN_SVC_TP_HIGH:
		wlan_service_set_tcp_limit_output_bytes(
			WLAN_TCP_LIMIT_OUTPUT_BYTES_HIGH);
		break;
	default:
		wsvc_printf_err("%s: Unsupported throughput value %d",
				__func__, tp_level);
		break;
	}
	return 0;
}

int wlan_service_rps_enable(void *data, int len)
{
	FILE *fp;
	int ret = 0;
	char file_name_buffer[MAX_FILE_NAME_BUF_SIZE];
	char interface_name[MAX_IFACE_NAME_BUF_SIZE];
	unsigned int num_q, soc_id;
	struct wlan_rps_data *rps_data_ptr = data;

	if (len < (int)sizeof(struct wlan_rps_data)) {
		wsvc_printf_err("%s input message of insufficient length %d",
				__func__, len);
		return -1;
	}

	/* get SoC id details */
	fp = fopen("/sys/devices/soc0/soc_id", "r");
	if (fp != NULL) {
		/* read SoC id here */
		fscanf(fp, "%u", &soc_id);
	} else {
		wsvc_printf_err("%s: unable to open %s",__func__,
			file_name_buffer);
		/* set defalut SoC ID */
		soc_id = 0;
	}

	switch(soc_id){
		case 278: /* 8976 */
		case 251: /* 8992 */
		case 207: /* 8994 */
		case 246: /* 8996 */
		case 305: /* 8996-Pro 1.0 */
			/* Dont do any thing for 8976, 8992, 8994, 8996 */
			break;
		default:
			/* reverse the cpu mask for all other case */
			for (num_q = 0; num_q < rps_data_ptr->num_queues; num_q++) {
				rps_data_ptr->cpu_map_list[num_q] =
						SWAP_NIBBLE(rps_data_ptr->cpu_map_list[num_q]);
			}
			break ;
	}

	if (fp != NULL)
		fclose(fp);

	strlcpy(interface_name, rps_data_ptr->ifname, sizeof(interface_name));
	wsvc_printf_dbg("%s interface name %s len %d num queues %d",
			__func__, interface_name, len,
			rps_data_ptr->num_queues);

	for (num_q = 0; num_q < rps_data_ptr->num_queues; num_q++) {
		ret = snprintf(file_name_buffer, (MAX_FILE_NAME_BUF_SIZE - 1),
			       "/sys/class/net/%s/queues/rx-%d/rps_cpus",
			       interface_name, num_q);
		if (ret < 0 ||
		    (unsigned int) ret >= (MAX_FILE_NAME_BUF_SIZE - 1)) {
			wsvc_printf_err("%s: snprintf() failed with %d, num_q: %u",
					__func__, ret, num_q);
			continue;
		}

		fp = fopen(file_name_buffer, "w");
		if (fp == NULL) {
			wsvc_printf_err(
				"%s: unable to open %s",
				__func__, file_name_buffer);
			continue;
		}

		wsvc_printf_info("%s %s queue %d map 0x%x",
			__func__, file_name_buffer, num_q,
			rps_data_ptr->cpu_map_list[num_q]);
		ret = fprintf(fp, "%x", rps_data_ptr->cpu_map_list[num_q]);
		fclose(fp);
		if (ret < 0) {
			wsvc_printf_err("%s: failed to write %s",
				__func__, file_name_buffer);
			return -1;
		}
	}

	return 0;
}

int perf_lock_handle = -1;
static int wlan_service_core_minfreq(struct wlan_core_minfreq *data, int len)
{
	int rc = 0;
	int resources[2] = {0, 0};

	wsvc_printf_dbg("-> %s(..len=%d)", __func__, len);

	if (data == NULL) {
		wsvc_printf_err("%s: bad msg_ptr", __func__);
		rc = -EINVAL;
		goto minfreq_ret;
	}
	if (data->magic != WLAN_CORE_MINFREQ_MAGIC) {
		wsvc_printf_err("%s: bad magic: 0x%0x", __func__, data->magic);
		rc = -EINVAL;
		goto minfreq_ret;
	}
	if (len != sizeof(struct wlan_core_minfreq)) {
		wsvc_printf_err("%s: bad len: %d", __func__, len);
		rc = -EINVAL;
		goto minfreq_ret;
	}

	if (data->freq == 0) { /* request type: "reset" */
		wsvc_printf_info("%s: RESET", __func__);
		 /* request-type: "reset" */
		if (perf_lock_handle < 0){
			wsvc_printf_err("%s: reset: handle not valid",
					__func__);
			rc = -1;
		} else {
			if (perf_lock_rel) {
				perf_lock_rel(perf_lock_handle);
				wsvc_printf_info("%s: perf_lock_rel(%0d)",
						 __func__, perf_lock_handle);
				perf_lock_handle = -1;
				wsvc_printf_info("%s: RELEASED lock", __func__);
			} else {
				wsvc_printf_err("%s: rel: handle not initialized",
						__func__);
				rc = -5;
			}
		}
		goto minfreq_ret;
	}

	/* request type: "set" */
	wsvc_printf_info("%s: SET", __func__);
	if (perf_lock_handle >= 0){
		wsvc_printf_err("%s: set: handle not free", __func__);
		rc = -2;
		goto minfreq_ret;
	}
	if (data->coremask == 0x00f0) {
		wsvc_printf_info("%s: BIG CL", __func__);
		/* big cluster */
		resources[0] = 0x40800000;
		resources[1] = data->freq;
	} else if (data->coremask == 0x0f) {
		wsvc_printf_info("%s: LITTLE CL", __func__);
		/* little cluster */
		resources[0] = 0x40800100;
		resources[1] = data->freq;
	} else {
		/* only cluster-wide freqs are valid */
		rc = -3;
		goto minfreq_ret;
	}

	if (perf_lock_acq) {
		/* make the perfd calls */
		perf_lock_handle = perf_lock_acq(perf_lock_handle,
						 0,
						 resources,
						 2);
		if (perf_lock_handle < 0)
			rc = -4;
		else
			wsvc_printf_info("%s: ACQUIRED lock %d",
					 __func__, perf_lock_handle);
	} else {
		wsvc_printf_err("%s: set: handle not initialized", __func__);
		rc = -5;
	}
	/* req: set */

minfreq_ret:
	wsvc_printf_dbg("<- %s [rc=%d]", __func__, rc);
	return rc;
}

int wlan_service_process_dp_msg(int type, void *data, int len)
{
	int rc = 0;
	struct wlan_status_data *p_data = NULL;

	if (!data) {
		wsvc_printf_err("%s: NULL data pointer",
				__func__);
		return -1;
	}

	switch (type) {
	case WLAN_MSG_WLAN_TP_IND:
		wsvc_printf_dbg("Throughput LEVEL: %d", *((int *)data));
		rc = wlan_service_update_tcp_rx_params(data, len);
		break;
	case WLAN_MSG_WLAN_TP_TX_IND:
		wsvc_printf_dbg("Throughput LEVEL: %d", *((int *)data));
		rc = wlan_service_update_tcp_tx_params(*((int *)data));
		break;
	case WLAN_MSG_RPS_ENABLE_IND:
		if (len < 0)
			break;
		wlan_service_rps_enable(data, len);
		break;
	case WLAN_MSG_CORE_MINFREQ:
		rc = wlan_service_core_minfreq(data, len);
		break;
	default:
		return rc;
	}

	return rc;
}
