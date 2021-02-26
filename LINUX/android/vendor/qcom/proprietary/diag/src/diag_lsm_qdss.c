/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

              Diag QDSS support

GENERAL DESCRIPTION

Implementation of configuring diag over qdss using diag command request/responses
and reading data from qdss, writing qdss data to file.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
#include <stdlib.h>
#include "comdef.h"
#include "stdio.h"
#include "stringl.h"
#include "diag_lsmi.h"
#include "./../include/diag_lsm.h"
#include "diag_lsmi.h"
#include "diag_shared_i.h"
#include "stdio.h"
#include "string.h"
#include <malloc.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <ctype.h>
#include <limits.h>
#include <pthread.h>
#include <stdint.h>
#include "diagcmd.h"
#include "errno.h"

#ifndef FEATURE_LE_DIAG
#include <cutils/log.h>
#endif

#define QDSS_RSP_BUF_SIZE 100
#define QDSS_CMD_REQ_BUF_SIZE	50

#define BLOCK_SIZE "/sys/bus/coresight/devices/coresight-tmc-etr/block_size"
#define MEM_TYPE "/sys/bus/coresight/devices/coresight-tmc-etr/mem_type"
#define MEM_SIZE "/sys/bus/coresight/devices/coresight-tmc-etr/mem_size"

#define BLOCK_SIZE_VALUE "65536"
#define MEM_TYPE_CONTIG "contig"
#define MEM_SIZE_CONTIG "0x100000"
#define MEM_TYPE_SG "sg"

static unsigned int qdss_mask;
char qdss_file_name_curr[NUM_PROC][FILE_NAME_LEN];
unsigned char qdss_read_buffer[READ_BUF_SIZE];
unsigned char qdss_cmd_req_buf[50];
pthread_t qdss_read_thread_hdl; /* Diag Read thread handle */
pthread_t qdss_write_thread_hdl;    /* Diag disk write thread handle */
pthread_t qdss_config_thread_hdl;
static int qdss_count_written_bytes;
int in_wait_for_qdss_peripheral_status = 0;
int qdss_diag_fd_md[NUM_PROC] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
int qdss_diag_fd_dev = -1;
int diag_qdss_node_fd = -1;
int qdss_state = 0;
int qdss_init_done = 0;

/* Static array for workaround */
static unsigned char qdss_static_buffer[2][DISK_BUF_SIZE];

#define DIAG_DIAG_STM				0x214
#define DIAG_QDSS_TRACE_SINK		0x101
#define DIAG_QDSS_FILTER_STM		0x103

#define DIAG_QDSS_FILTER_SWTRACE	0x06
#define DIAG_QDSS_FILTER_ENTITY		0x08

#define DIAG_QDSS_PROCESSOR_APPS	0x0100
#define DIAG_QDSS_PROCESSOR_MODEM	0x0200
#define DIAG_QDSS_PROCESSOR_WCNSS	0x0300
#define DIAG_QDSS_PROCESSOR_LPASS	0x0500
#define DIAG_QDSS_PROCESSOR_SENSOR	0x0800

#define DIAG_STM_MODEM	0x01
#define DIAG_STM_LPASS	0x02
#define DIAG_STM_WCNSS	0x04
#define DIAG_STM_APPS	0x08
#define DIAG_STM_SENSORS 0x10
#define DIAG_STM_WDSP 0x20
#define DIAG_STM_CDSP 0x40
static int qdss_curr_write;
static int qdss_curr_read;
static int qdss_write_in_progress;
int qdss_in_write = 0;
int qdss_kill_thread = 0;
extern unsigned long max_file_size;
extern unsigned long min_file_size;
char qdss_peripheral_name[FILE_NAME_LEN];
struct qdss_read_buf_pool {
	unsigned char* rsp_buf;
	int data_ready;
	pthread_mutex_t read_rsp_mutex;
	pthread_mutex_t write_rsp_mutex;
	pthread_cond_t write_rsp_cond;
	pthread_cond_t read_rsp_cond;
};

static struct qdss_read_buf_pool qdss_read_buffer_pool[2];
pthread_mutex_t qdss_diag_mutex;
pthread_mutex_t qdss_config_mutex;
pthread_cond_t qdss_diag_cond;
pthread_cond_t qdss_config_cond;

volatile int qdss_curr_read_idx = 0;
volatile int qdss_curr_write_idx = 0;
typedef PACK(struct) {
	unsigned int peripheral_mask;
	unsigned int peripheral_type;
} qdss_peripheral_info;

qdss_peripheral_info qdss_periph_info;
typedef PACK(struct) {
	uint8 cmd_code;
	uint8 subsys_id;
	uint16 subsys_cmd_code;
	uint8 version;
	uint8 processor_mask;
	uint8 stm_cmd;
} diag_qdss_config_req;

typedef PACK(struct) {
	uint8 cmd_code;
	uint8 subsys_id;
	uint16 subsys_cmd_code;
	uint8 state;
} diag_enable_qdss_tracer_req;

typedef PACK(struct) {
	uint8 cmd_code;
	uint8 subsys_id;
	uint16 subsys_cmd_code;
	uint8 state;
	uint8 entity_id;
} diag_enable_qdss_diag_tracer_req;

typedef PACK(struct) {
	uint8 cmd_code;
	uint8 subsys_id;
	uint16 subsys_cmd_code;
	uint8 state;
} diag_enable_qdss_req;

typedef PACK(struct) {
	uint8 cmd_code;
	uint8 subsys_id;
	uint16 subsys_cmd_code;
	uint8 sink;
} diag_set_out_mode;

struct buffer_pool qdss_pools[] = {
	[0] = {
		.free		=	1,
		.data_ready	=	0,
	},
	[1] = {
		.free		=	1,
		.data_ready	=	0,
	},

};
static void* qdss_read_thread(void* param);
static void* qdss_write_thread(void* param);
void* qdss_config_thread(void* param);

void diag_notify_qdss_thread(int peripheral_type, int peripheral_mask)
{
	qdss_periph_info.peripheral_type = peripheral_type;
	qdss_periph_info.peripheral_mask = peripheral_mask;
	pthread_cond_signal(&qdss_diag_cond);
}

static int wait_for_response()
{
	struct timespec time;
	struct timeval now;
	int rt = 0;

	/****************************************************************
	 * Wait time is 10 sec while setting the QDSS environment		*
	 ****************************************************************/

	gettimeofday(&now, NULL);
	time.tv_sec = now.tv_sec + 10000 / 1000;
	time.tv_nsec = now.tv_usec + (10000 % 1000) * 1000000;
	pthread_mutex_lock(&(qdss_read_buffer_pool[qdss_curr_write_idx].write_rsp_mutex));
	if (!qdss_read_buffer_pool[qdss_curr_write_idx].data_ready)
		rt = pthread_cond_timedwait(&(qdss_read_buffer_pool[qdss_curr_write_idx].write_rsp_cond), &(qdss_read_buffer_pool[qdss_curr_write_idx].write_rsp_mutex), &time);
	return rt;
}

static int wait_for_kill_response()
{
	struct timespec time;
	struct timeval now;
	int rt = 0;

	/****************************************************************
	 * Wait time is 5 sec while resetting the QDSS environment		*
	 ****************************************************************/
	gettimeofday(&now, NULL);
	time.tv_sec = now.tv_sec + 5000 / 1000;
	time.tv_nsec = now.tv_usec + (5000 % 1000) * 1000000;
	pthread_mutex_lock(&(qdss_read_buffer_pool[qdss_curr_write_idx].write_rsp_mutex));
	if (!qdss_read_buffer_pool[qdss_curr_write_idx].data_ready)
		rt = pthread_cond_timedwait(&(qdss_read_buffer_pool[qdss_curr_write_idx].write_rsp_cond), &(qdss_read_buffer_pool[qdss_curr_write_idx].write_rsp_mutex), &time);
	return rt;
}

static int diag_set_coresight_sysfs(const char *block_file_path, unsigned char *val, const char *str)
{
	int block_fd, ret;
	char value[10];

	if (!block_file_path || (strlen(val) >= sizeof(value))) {
		return -1;
	}

	memset(value, '\0', sizeof(value));

	block_fd = open(block_file_path, O_WRONLY);
	if (block_fd < 0) {
		DIAG_LOGE("%s open fail: %s error: %s\n", str, block_file_path, strerror(errno));
		return -1;
	}

	ret = write(block_fd, val, strlen(val) + 1);
	if (ret < 0) {
		DIAG_LOGE("%s write fail: %s error: %s\n", str, block_file_path, strerror(errno));
		close(block_fd);
		return -1;
	}
	close(block_fd);

	block_fd = open(block_file_path, O_RDONLY);
	if (block_fd < 0) {
		DIAG_LOGE("%s open fail: %s error: %s\n", str, block_file_path, strerror(errno));
		return -1;
	}

	ret = read(block_fd, value, strlen(val));
	if (ret < 0) {
		DIAG_LOGE("%s read fail: %s error: %s\n", str, block_file_path, strerror(errno));
		close(block_fd);
		return -1;
	}
	strlcat(value, "\0", 1);
	DIAG_LOGE("diag: Value set to %s = %s\n", block_file_path, value);

	close(block_fd);

	return 0;
}

int diag_qdss_init()
{
	int ret;

	pthread_mutex_init(&qdss_diag_mutex, NULL);
	pthread_mutex_init(&qdss_config_mutex, NULL);
	pthread_mutex_init(&(qdss_read_buffer_pool[0].read_rsp_mutex), NULL);
	pthread_mutex_init(&(qdss_read_buffer_pool[1].read_rsp_mutex), NULL);
	pthread_mutex_init(&(qdss_read_buffer_pool[0].write_rsp_mutex), NULL);
	pthread_mutex_init(&(qdss_read_buffer_pool[1].write_rsp_mutex), NULL);
	pthread_cond_init(&(qdss_read_buffer_pool[0].read_rsp_cond), NULL);
	pthread_cond_init(&(qdss_read_buffer_pool[0].write_rsp_cond), NULL);
	pthread_cond_init(&(qdss_read_buffer_pool[1].read_rsp_cond), NULL);
	pthread_cond_init(&(qdss_read_buffer_pool[1].write_rsp_cond), NULL);

	qdss_read_buffer_pool[0].rsp_buf = malloc(QDSS_RSP_BUF_SIZE);
	if (!qdss_read_buffer_pool[0].rsp_buf){
		DIAG_LOGE("%s:failed to create rsp buffer zero\n", __func__);
		return FALSE;
	}
	qdss_read_buffer_pool[1].rsp_buf = malloc(QDSS_RSP_BUF_SIZE);
	if (!qdss_read_buffer_pool[1].rsp_buf){
		DIAG_LOGE("%s:failed to create rsp buffer one\n", __func__);
		free(qdss_read_buffer_pool[0].rsp_buf);
		return FALSE;
	}
	qdss_read_buffer_pool[0].data_ready = 0;
	qdss_read_buffer_pool[1].data_ready = 0;
	pthread_cond_init(&qdss_diag_cond, NULL);
	pthread_cond_init(&qdss_config_cond, NULL);
	pthread_mutex_init(&(qdss_pools[0].write_mutex), NULL);
	pthread_cond_init(&(qdss_pools[0].write_cond), NULL);
	pthread_mutex_init(&(qdss_pools[0].read_mutex), NULL);
	pthread_cond_init(&(qdss_pools[0].read_cond), NULL);
	pthread_mutex_init(&(qdss_pools[1].write_mutex), NULL);
	pthread_cond_init(&(qdss_pools[1].write_cond), NULL);
	pthread_mutex_init(&(qdss_pools[1].read_mutex), NULL);
	pthread_cond_init(&(qdss_pools[1].read_cond), NULL);

	qdss_periph_info.peripheral_type |= DIAG_MSM_MASK;
	qdss_periph_info.peripheral_mask = qdss_mask;

	qdss_pools[0].buffer_ptr[0] = qdss_static_buffer[0];
	qdss_pools[1].buffer_ptr[0] = qdss_static_buffer[1];

	qdss_pools[0].bytes_in_buff[0] = 0;
	qdss_pools[1].bytes_in_buff[0] = 0;

	/****************************************************************
	 * Necessary to set Block size before accessing /dev/byte-cntr  *
	 ****************************************************************/

	ret = diag_set_coresight_sysfs(BLOCK_SIZE, BLOCK_SIZE_VALUE, "byte_cntr - block_size");
	if (ret) {
		DIAG_LOGE(" %s: block size write failed\n", __func__);
		free(qdss_read_buffer_pool[0].rsp_buf);
		free(qdss_read_buffer_pool[1].rsp_buf);
		return -1;
	}
	diag_get_peripheral_name_from_mask(qdss_peripheral_name,
							FILE_NAME_LEN,
							qdss_mask);

	pthread_create(&qdss_read_thread_hdl, NULL, qdss_read_thread, NULL);
	if (qdss_read_thread_hdl == 0) {
		DIAG_LOGE("%s: Failed to create read thread", __func__);
		goto failure_case;
	}

	pthread_create(&qdss_config_thread_hdl, NULL, qdss_config_thread, NULL);
	if (qdss_config_thread_hdl == 0) {
		DIAG_LOGE("%s: Failed to create config thread", __func__);
		goto failure_case;
	}

	pthread_create(&qdss_write_thread_hdl, NULL, qdss_write_thread, NULL);
	if (qdss_write_thread_hdl == 0) {
		DIAG_LOGE("%s: Failed to create write thread", __func__);
		goto failure_case;
	}

	pthread_mutex_lock(&qdss_config_mutex);
	pthread_cond_wait(&qdss_config_cond, &qdss_config_mutex);
	if (!qdss_init_done)
		goto failure_case;

	return 0;

 failure_case:
	close(diag_qdss_node_fd);
	free(qdss_read_buffer_pool[0].rsp_buf);
	free(qdss_read_buffer_pool[1].rsp_buf);
	return -1;

}
static void request_qdss_diag_read_buffer()
{
	pthread_mutex_lock(&(qdss_read_buffer_pool[qdss_curr_read_idx].write_rsp_mutex));
	pthread_mutex_lock(&(qdss_read_buffer_pool[qdss_curr_read_idx].read_rsp_mutex));
	if (qdss_read_buffer_pool[qdss_curr_read_idx].data_ready) {
		pthread_mutex_unlock(&(qdss_read_buffer_pool[qdss_curr_read_idx].write_rsp_mutex));
		pthread_cond_wait(&(qdss_read_buffer_pool[qdss_curr_read_idx].read_rsp_cond),
						  &(qdss_read_buffer_pool[qdss_curr_read_idx].read_rsp_mutex));
		pthread_mutex_lock(&(qdss_read_buffer_pool[qdss_curr_read_idx].write_rsp_mutex));
	}
	pthread_mutex_unlock(&(qdss_read_buffer_pool[qdss_curr_read_idx].read_rsp_mutex));
}
static int diag_qdss_reset_read_buffer()
{
	qdss_read_buffer_pool[qdss_curr_write_idx].data_ready = 0;
	pthread_mutex_lock(&(qdss_read_buffer_pool[qdss_curr_write_idx].read_rsp_mutex));
	pthread_cond_signal(&(qdss_read_buffer_pool[qdss_curr_write_idx].read_rsp_cond));
	pthread_mutex_unlock(&(qdss_read_buffer_pool[qdss_curr_write_idx].read_rsp_mutex));
	pthread_mutex_unlock(&(qdss_read_buffer_pool[qdss_curr_write_idx].write_rsp_mutex));
	qdss_curr_write_idx = !qdss_curr_write_idx;
	return 0;
}
static int diag_set_diag_transport(int peripheral_type, int peripheral, uint8 stm_cmd)
{
	int offset = 0, length = 0, ret = 0;
	diag_qdss_config_req* req = NULL;
	unsigned char* ptr = qdss_cmd_req_buf;

	if (!ptr)
		return FALSE;

	if ((peripheral_type < MSM || peripheral_type > MDM) ||
		(peripheral <  DIAG_MODEM_PROC || peripheral >  DIAG_CDSP_PROC)) {
		DIAG_LOGE("diag:%s cmd sent failed for peripheral = %d, peripheral_type = %d\n", __func__, peripheral, peripheral_type);
		return FALSE;
	}

	*(int*)ptr = USER_SPACE_RAW_DATA_TYPE;
	offset += sizeof(int);
	if (peripheral_type == MDM) {
		*(int*)(ptr + offset) = MDM_TOKEN;
		offset += sizeof(int);
	} else if (peripheral_type == QSC) {
		*(int*)(ptr + offset) = QSC_TOKEN;
		offset += sizeof(int);
	}
	ptr = ptr + offset;
	req = (diag_qdss_config_req*)ptr;

	/*************************************************
	 * 	4B 12 214 02 X Y							 *
	 * 												 *
	 * 	X = STM Processor Mask						 *
	 * 	Y = STM Enable(1)/Disable(0)				 *
	 *************************************************/
	req->cmd_code = DIAG_SUBSYS_CMD_F;
	req->subsys_id = DIAG_SUBSYS_DIAG_SERV;
	req->subsys_cmd_code = DIAG_DIAG_STM;
	req->version = 2;
	switch (peripheral) {
	case DIAG_MODEM_PROC :
		req->processor_mask = DIAG_STM_MODEM;
		break;
	case DIAG_LPASS_PROC :
		req->processor_mask = DIAG_STM_LPASS;
		break;
	case DIAG_WCNSS_PROC :
		req->processor_mask = DIAG_STM_WCNSS;
		break;
	case DIAG_APPS_PROC :
		req->processor_mask = DIAG_STM_APPS;
		break;
	default:
		DIAG_LOGE("diag:%s Invalid peripheral = %d, peripheral_type = %d\n", __func__, peripheral, peripheral_type);
		return FALSE;
	}
	req->stm_cmd = stm_cmd;
	length = sizeof(diag_qdss_config_req);

	if (length + offset <= QDSS_CMD_REQ_BUF_SIZE) {
		ret = diag_send_data(qdss_cmd_req_buf, offset + length);
		if (ret)
			return FALSE;
	}

	return TRUE;

}
static int diag_set_diag_qdss_tracer(int peripheral_type, int peripheral, uint8 state)
{
	int offset = 0, length = 0, ret = 0;
	unsigned char* ptr = qdss_cmd_req_buf;
	diag_enable_qdss_tracer_req* req = NULL;

	if (!ptr)
		return FALSE;

	if ((peripheral_type < MSM || peripheral_type > MDM) ||
		(peripheral <  DIAG_MODEM_PROC || peripheral >  DIAG_CDSP_PROC)) {
		DIAG_LOGE("diag:%s cmd sent failed for peripheral = %d, peripheral_type = %d\n", __func__, peripheral, peripheral_type);
		return FALSE;
	}

	*(int*)ptr = USER_SPACE_RAW_DATA_TYPE;
	offset += sizeof(int);
	if (peripheral_type == MDM) {
		*(int*)(ptr + offset) = MDM_TOKEN;
		offset += sizeof(int);
	} else if (peripheral_type == QSC) {
		*(int*)(ptr + offset) = QSC_TOKEN;
		offset += sizeof(int);
	}
	ptr = ptr + offset;
	req = (diag_enable_qdss_tracer_req*)ptr;

	/*************************************************
	 * 	4B 5A 06 X Y 0D								 *
	 * 												 *
	 * 	X = QDSS PROCESSOR MASK						 *
	 * 	Y = State Enable(1)/Disable(0)				 *
	 *************************************************/
	req->cmd_code = DIAG_SUBSYS_CMD_F;
	req->subsys_id = DIAG_SUBSYS_QDSS;
	req->subsys_cmd_code = DIAG_QDSS_FILTER_SWTRACE;

	switch (peripheral) {
	case DIAG_APPS_PROC :
		req->subsys_cmd_code = DIAG_QDSS_PROCESSOR_APPS;
		break;
	case DIAG_MODEM_PROC :
		req->subsys_cmd_code += DIAG_QDSS_PROCESSOR_MODEM;
		break;
	case DIAG_WCNSS_PROC :
		req->subsys_cmd_code += DIAG_QDSS_PROCESSOR_WCNSS;
		break;
	case DIAG_LPASS_PROC :
		req->subsys_cmd_code += DIAG_QDSS_PROCESSOR_LPASS;
		break;
	case DIAG_SENSORS_PROC :
		req->subsys_cmd_code += DIAG_QDSS_PROCESSOR_SENSOR;
		break;
	default:
		DIAG_LOGE("diag:%s support for peripheral = %d, peripheral_type = %d not present yet\n", __func__, peripheral, peripheral_type);
		return FALSE;
	}

	req->state = state;
	length = sizeof(diag_enable_qdss_tracer_req);

	if (length + offset <= QDSS_CMD_REQ_BUF_SIZE) {
		ret = diag_send_data(qdss_cmd_req_buf, offset + length);
		if (ret)
			return FALSE;
	}

	return TRUE;

}

static int diag_set_diag_qdss_diag_tracer(int peripheral_type, int peripheral, uint8 state)
{
	int offset = 0, length = 0, ret = 0;
	unsigned char* ptr = qdss_cmd_req_buf;
	diag_enable_qdss_diag_tracer_req* req = NULL;

	if (!ptr)
		return FALSE;

	if ((peripheral_type < MSM || peripheral_type > MDM) ||
		(peripheral <  DIAG_MODEM_PROC || peripheral >  DIAG_CDSP_PROC)) {
		DIAG_LOGE("diag:%s cmd sent failed for peripheral = %d, peripheral_type = %d\n", __func__, peripheral, peripheral_type);
		return FALSE;
	}

	*(int*)ptr = USER_SPACE_RAW_DATA_TYPE;
	offset += sizeof(int);
	if (peripheral_type == MDM) {
		*(int*)(ptr + offset) = MDM_TOKEN;
		offset += sizeof(int);
	} else if (peripheral_type == QSC) {
		*(int*)(ptr + offset) = QSC_TOKEN;
		offset += sizeof(int);
	}
	ptr = ptr + offset;
	req = (diag_enable_qdss_diag_tracer_req*)ptr;

	/*************************************************
	 * 	4B 5A 08 X Y 0D								 *
	 * 												 *
	 * 	X = QDSS PROCESSOR MASK						 *
	 * 	Y = State Enable(1)/Disable(0)				 *
	 *************************************************/
	req->cmd_code = DIAG_SUBSYS_CMD_F;
	req->subsys_id = DIAG_SUBSYS_QDSS;
	req->subsys_cmd_code = DIAG_QDSS_FILTER_ENTITY;

	switch (peripheral) {
	case DIAG_APPS_PROC :
		req->subsys_cmd_code += DIAG_QDSS_PROCESSOR_APPS;
		break;
	case DIAG_MODEM_PROC :
		req->subsys_cmd_code += DIAG_QDSS_PROCESSOR_MODEM;
		break;
	case DIAG_WCNSS_PROC :
		req->subsys_cmd_code += DIAG_QDSS_PROCESSOR_WCNSS;
		break;
	case DIAG_LPASS_PROC :
		req->subsys_cmd_code += DIAG_QDSS_PROCESSOR_LPASS;
		break;
	case DIAG_SENSORS_PROC :
		req->subsys_cmd_code += DIAG_QDSS_PROCESSOR_SENSOR;
		break;
	default:
		DIAG_LOGE("diag:%s support for peripheral = %d, peripheral_type = %d is not present\n", __func__, peripheral, peripheral_type);
		return FALSE;
	}

	req->state = state;
	req->entity_id = 0x0D;
	length = sizeof(diag_enable_qdss_diag_tracer_req);

	if (length + offset <= QDSS_CMD_REQ_BUF_SIZE) {
		ret = diag_send_data(qdss_cmd_req_buf, offset + length);
		if (ret)
			return FALSE;
	}

	return TRUE;

}
int diag_send_enable_qdss_req(int peripheral_type, int peripheral, uint8 state)
{
	int offset = 0, length = 0, ret = 0;
	diag_enable_qdss_req* req = NULL;
	unsigned char* ptr = qdss_cmd_req_buf;

	if (!ptr)
		return FALSE;

	if ((peripheral_type < MSM || peripheral_type > MDM) ||
		(peripheral <  DIAG_MODEM_PROC || peripheral >  DIAG_CDSP_PROC)) {
		DIAG_LOGE("diag:%s cmd sent failed for peripheral = %d, peripheral_type = %d\n", __func__, peripheral, peripheral_type);
		return FALSE;
	}

	*(int*)ptr = USER_SPACE_RAW_DATA_TYPE;
	offset += sizeof(int);
	if (peripheral_type == MDM) {
		*(int*)(ptr + offset) = MDM_TOKEN;
		offset += sizeof(int);
	} else if (peripheral_type == QSC) {
		*(int*)(ptr + offset) = QSC_TOKEN;
		offset += sizeof(int);
	}
	ptr = ptr + offset;
	req = (diag_enable_qdss_req*)ptr;

	/*************************************************
	 * 	4B 5A 0103 X								 *
	 *												 *
	 * 	X = 1 To enable STM							 *
	 * 	X = 0 To Disable STM						 *
	 *************************************************/
	req->cmd_code = DIAG_SUBSYS_CMD_F;
	req->subsys_id = DIAG_SUBSYS_QDSS;
	req->subsys_cmd_code = DIAG_QDSS_FILTER_STM;
	req->state = state;
	length = sizeof(diag_enable_qdss_req);

	if (length + offset <= QDSS_CMD_REQ_BUF_SIZE) {
		ret = diag_send_data(qdss_cmd_req_buf, offset + length);
		if (ret)
			return FALSE;
	}

	return TRUE;
}

int diag_set_etr_out_mode(int peripheral_type, int peripheral, uint8 sink)
{
	int offset = 0, length = 0, ret = 0;
	diag_set_out_mode* req = NULL;
	unsigned char* ptr = qdss_cmd_req_buf;

	if (!ptr)
		return FALSE;

	if ((peripheral_type < MSM || peripheral_type > MDM) ||
		(peripheral <  DIAG_MODEM_PROC || peripheral >  DIAG_CDSP_PROC)) {
		DIAG_LOGE("diag:%s cmd sent failed for peripheral = %d, peripheral_type = %d\n", __func__, peripheral, peripheral_type);
		return FALSE;
	}

	*(int*)ptr = USER_SPACE_RAW_DATA_TYPE;
	offset += sizeof(int);
	if (peripheral_type == MDM) {
		*(int*)(ptr + offset) = MDM_TOKEN;
		offset += sizeof(int);
	} else if (peripheral_type == QSC) {
		*(int*)(ptr + offset) = QSC_TOKEN;
		offset += sizeof(int);
	}
	ptr = ptr + offset;
	req = (diag_set_out_mode*)ptr;

	/*************************************************
	 * 	4B 5A 0101 X								 *
	 * 												 *
	 * 	X = 03 for USB								 *
	 * 	X = 01 for DDR								 *
	 *************************************************/
	req->cmd_code = DIAG_SUBSYS_CMD_F;
	req->subsys_id = DIAG_SUBSYS_QDSS;
	req->subsys_cmd_code = DIAG_QDSS_TRACE_SINK;
	req->sink = sink;
	length = sizeof(diag_set_out_mode);

	if (length + offset <= QDSS_CMD_REQ_BUF_SIZE) {
		ret = diag_send_data(qdss_cmd_req_buf, offset + length);
		if (ret)
			return FALSE;
	}

	return TRUE;
}

int diag_send_cmds_to_peripheral_init(int peripheral_type, int peripheral)
{
	int rt;
	unsigned char *file_path = NULL;

	qdss_state = 1;

#if 0
	/*************************************************
	 * 		Set mem_size to 1MB				 		 *
	 *************************************************/

	rt = diag_set_coresight_sysfs(MEM_SIZE, MEM_SIZE_CONTIG, "coresight - mem_size");
	if (rt) {
		DIAG_LOGE("diag: Failed to set memsize to 1MB\n");
		pthread_mutex_unlock(&(qdss_read_buffer_pool[qdss_curr_write_idx].write_rsp_mutex));
		qdss_state = 0;
		return -1;
	}

	/*************************************************
	 * 		Set mem_type to Contig			 		 *
	 *************************************************/

	rt = diag_set_coresight_sysfs(MEM_TYPE, MEM_TYPE_CONTIG, "coresight - mem_type");
	if (rt) {
		DIAG_LOGE("diag: Failed to set memtype to contig\n");
		pthread_mutex_unlock(&(qdss_read_buffer_pool[qdss_curr_write_idx].write_rsp_mutex));
		qdss_state = 0;
		return -1;
	}
#endif
	/*************************************
	*		Set ETR routing to DDR	 	 *
	**************************************/
	rt = diag_set_etr_out_mode(peripheral_type, peripheral, 1);
	if (rt == FALSE) {
		qdss_state = 0;
		return -1;
	}
	rt = wait_for_response();
	if (rt == ETIMEDOUT) {
		DIAG_LOGE("diag:%s time out while waiting OUT Mode cmd response p_type:%d peipheral:%d\n", __func__, peripheral_type, peripheral);
		pthread_mutex_unlock(&(qdss_read_buffer_pool[qdss_curr_write_idx].write_rsp_mutex));
		qdss_state = 0;
		return -1;
	}
	diag_qdss_reset_read_buffer();

	/*************************************
	*		Enable QDSS ETM				 *
	**************************************/
	rt = diag_send_enable_qdss_req(peripheral_type, peripheral, 1);
	if (rt == FALSE) {
		qdss_state = 0;
		return -1;
	}
	rt = wait_for_response();
	if (rt == ETIMEDOUT) {
		DIAG_LOGE("diag:%s time out while waiting for enable QDSS cmd response p_type:%d peipheral:%d\n", __func__, peripheral_type, peripheral);
		pthread_mutex_unlock(&(qdss_read_buffer_pool[qdss_curr_write_idx].write_rsp_mutex));
		qdss_state = 0;
		return -1;
	}
	diag_qdss_reset_read_buffer();

	/*************************************
	*	Send command to diag to 		 *
	*	set STM Mask for the peripheral	 *
	**************************************/
	rt = diag_set_diag_transport(peripheral_type, peripheral, 1);
	if (rt == FALSE) {
		qdss_state = 0;
		return -1;
	}
	rt = wait_for_response();
	if (rt == ETIMEDOUT) {
		DIAG_LOGE("diag:%s time out while waiting for set QDSS cmd response for p_type:%d peipheral:%d\n", __func__, peripheral_type, peripheral);
		pthread_mutex_unlock(&(qdss_read_buffer_pool[qdss_curr_write_idx].write_rsp_mutex));
		qdss_state = 0;
		return -1;
	}

	diag_qdss_reset_read_buffer();

	/*************************************
	*	Enable QDSS tracer			 	 *
	**************************************/
	rt = diag_set_diag_qdss_tracer(peripheral_type, peripheral, 1);
	if (rt == FALSE) {
		qdss_state = 0;
		return -1;
	}
	rt = wait_for_response();
	if (rt == ETIMEDOUT) {
		DIAG_LOGE("diag:%s time out while waiting for set diag qdss tracer cmd response for p_type:%d peipheral:%d\n", __func__, peripheral_type, peripheral);
		pthread_mutex_unlock(&(qdss_read_buffer_pool[qdss_curr_write_idx].write_rsp_mutex));
		qdss_state = 0;
		return -1;
	}
	diag_qdss_reset_read_buffer();

	/*************************************
	*	Enable QDSS tracer DIAG entity 	 *
	**************************************/
	rt = diag_set_diag_qdss_diag_tracer(peripheral_type, peripheral, 1);
	if (rt == FALSE) {
		qdss_state = 0;
		return -1;
	}
	rt = wait_for_response();
	if (rt == ETIMEDOUT) {
		DIAG_LOGE("diag:%s time out while waiting for set qdss tracer diag entity cmd response for p_type:%d peipheral:%d\n", __func__, peripheral_type, peripheral);
		pthread_mutex_unlock(&(qdss_read_buffer_pool[qdss_curr_write_idx].write_rsp_mutex));
		qdss_state = 0;
		return -1;
	}
	diag_qdss_reset_read_buffer();

	/*********************************************
	*	Commands to set QDSS environment sent	 *
	**********************************************/
	qdss_state = 0;
	return 0;
}
int diag_send_cmds_to_peripheral_kill(int peripheral_type, int peripheral)
{
	int rt;
	unsigned char *file_path = NULL;

	qdss_state = 1;

#if 0
	/*************************************************
	 * 		Reset mem_type to Scatter Gather		 *
	 *************************************************/
	rt = diag_set_coresight_sysfs(MEM_TYPE, MEM_TYPE_SG, "coresight - mem_type");
	if (rt) {
		qdss_state = 0;
		DIAG_LOGE("diag: Failed to set memtype to sg\n");
		return -1;
	}
#endif
	/*************************************
	 * 		Disable QDSS ETM			 *
	 *************************************/
	rt = diag_send_enable_qdss_req(peripheral_type, peripheral, 0);
	if (rt == FALSE) {
		qdss_state = 0;
		return -1;
	}
	rt = wait_for_kill_response();
	if (rt == ETIMEDOUT) {
		DIAG_LOGE("diag:%s time out while waiting for disable QDSS cmd response p_type:%d peipheral:%d\n", __func__, peripheral_type, peripheral);
		pthread_mutex_unlock(&(qdss_read_buffer_pool[qdss_curr_write_idx].write_rsp_mutex));
		qdss_state = 0;
		return -1;
	}
	diag_qdss_reset_read_buffer();

	/*************************************
	*	Send command to diag to 		 *
	*	Clear STM Mask for the peripheral	 *
	**************************************/
	rt = diag_set_diag_transport(peripheral_type, peripheral, 0);
	if (rt == FALSE) {
		qdss_state = 0;
		return -1;
	}
	rt = wait_for_kill_response();
	if (rt == ETIMEDOUT) {
		DIAG_LOGE("diag:%s time out while waiting for clear QDSS cmd response for p_type:%d peipheral:%d\n", __func__, peripheral_type, peripheral);
		pthread_mutex_unlock(&(qdss_read_buffer_pool[qdss_curr_write_idx].write_rsp_mutex));
		qdss_state = 0;
		return -1;
	}
	diag_qdss_reset_read_buffer();

	/*************************************
	 * Disable QDSS tracer				 *
	 *************************************/
	rt = diag_set_diag_qdss_tracer(peripheral_type, peripheral, 0);
	if (rt == FALSE) {
		qdss_state = 0;
		return -1;
	}
	rt = wait_for_kill_response();
	if (rt == ETIMEDOUT) {
		DIAG_LOGE("diag:%s time out while waiting for clear diag qdss tracer cmd response for p_type:%d peipheral:%d\n", __func__, peripheral_type, peripheral);
		pthread_mutex_unlock(&(qdss_read_buffer_pool[qdss_curr_write_idx].write_rsp_mutex));
		qdss_state = 0;
		return -1;
	}
	diag_qdss_reset_read_buffer();

	/*************************************
	 * Disable QDSS tracer DIAG entity	 *
	 *************************************/
	rt = diag_set_diag_qdss_diag_tracer(peripheral_type, peripheral, 0);
	if (rt == FALSE) {
		qdss_state = 0;
		return -1;
	}
	rt = wait_for_kill_response();
	if (rt == ETIMEDOUT) {
		DIAG_LOGE("diag:%s time out while waiting for clear qdss tracer diag entity cmd response for p_type:%d peipheral:%d\n", __func__, peripheral_type, peripheral);
		pthread_mutex_unlock(&(qdss_read_buffer_pool[qdss_curr_write_idx].write_rsp_mutex));
		qdss_state = 0;
		return -1;
	}
	diag_qdss_reset_read_buffer();

	/*********************************************
	*	Commands to reset QDSS environment sent	 *
	**********************************************/
	qdss_state = 0;
	return 0;
}

void* qdss_config_thread(void* param)
{
	(void)param;

	while (1) {
		if (qdss_periph_info.peripheral_type & DIAG_MSM_MASK) {
			if (qdss_periph_info.peripheral_mask & DIAG_CON_MPSS) {
				diag_send_cmds_to_peripheral_init(MSM, DIAG_MODEM_PROC);
				qdss_periph_info.peripheral_mask = qdss_periph_info.peripheral_mask ^ DIAG_CON_MPSS;
				if (qdss_kill_thread)
					return 0;
			}

			if (qdss_periph_info.peripheral_mask & DIAG_CON_LPASS) {
				diag_send_cmds_to_peripheral_init(MSM, DIAG_LPASS_PROC);
				qdss_periph_info.peripheral_mask = qdss_periph_info.peripheral_mask ^ DIAG_CON_LPASS;
				if (qdss_kill_thread)
					return 0;
			}
			if (qdss_periph_info.peripheral_mask & DIAG_CON_WCNSS) {
				diag_send_cmds_to_peripheral_init(MSM, DIAG_WCNSS_PROC);
				qdss_periph_info.peripheral_mask = qdss_periph_info.peripheral_mask ^ DIAG_CON_WCNSS;
				if (qdss_kill_thread)
					return 0;
			}
			if (qdss_periph_info.peripheral_mask & DIAG_CON_SENSORS) {
				diag_send_cmds_to_peripheral_init(MSM, DIAG_SENSORS_PROC);
				qdss_periph_info.peripheral_mask = qdss_periph_info.peripheral_mask ^ DIAG_CON_SENSORS;
				if (qdss_kill_thread)
					return 0;
			}
			if (qdss_periph_info.peripheral_mask & DIAG_CON_WDSP) {
				diag_send_cmds_to_peripheral_init(MSM, DIAG_WDSP_PROC);
				qdss_periph_info.peripheral_mask = qdss_periph_info.peripheral_mask ^ DIAG_CON_WDSP;
				if (qdss_kill_thread)
					return 0;
			}
			if (qdss_periph_info.peripheral_mask & DIAG_CON_CDSP) {
				diag_send_cmds_to_peripheral_init(MSM, DIAG_CDSP_PROC);
				qdss_periph_info.peripheral_mask = qdss_periph_info.peripheral_mask ^ DIAG_CON_CDSP;
				if (qdss_kill_thread)
					return 0;
			}
			if (qdss_periph_info.peripheral_mask & DIAG_CON_APSS) {
				diag_send_cmds_to_peripheral_init(MSM, DIAG_APPS_PROC);
				qdss_periph_info.peripheral_mask = qdss_periph_info.peripheral_mask ^ DIAG_CON_APSS;
				if (qdss_kill_thread)
					return 0;
			}
			qdss_periph_info.peripheral_type = qdss_periph_info.peripheral_type ^ DIAG_MSM_MASK;

			/************************************************************************
			 * Open Device node after setting QDSS environment by sending commands	*
			 * Signal the condition to qdss_init for successful/failure case		*
			 ************************************************************************/

			if (diag_qdss_node_fd < 0) {
				diag_qdss_node_fd = open("/dev/byte-cntr", O_RDONLY);
				if (diag_qdss_node_fd == DIAG_INVALID_HANDLE) {
					DIAG_LOGE(" %s: Failed to open /dev/byte-cntr handle to qdss driver, error = %d\n", __func__, errno);
					qdss_init_done = 0;
					pthread_cond_signal(&qdss_config_cond);
					return 0;
				}
				qdss_init_done = 1;
			}
			pthread_cond_signal(&qdss_config_cond);
		}
		pthread_mutex_lock(&qdss_diag_mutex);
		while (!qdss_periph_info.peripheral_mask) {
			in_wait_for_qdss_peripheral_status = 1;
			pthread_cond_wait(&qdss_diag_cond, &qdss_diag_mutex);
			in_wait_for_qdss_peripheral_status = 0;
			if (qdss_kill_thread)
				return 0;
		}
		pthread_mutex_unlock(&qdss_diag_mutex);
	}
}
void fill_qdss_buffer(void* ptr, int len)
{
	unsigned char* buffer = NULL;
	unsigned int* bytes_in_buff = NULL;
	buffer = qdss_pools[qdss_curr_read].buffer_ptr[0];
	bytes_in_buff = &qdss_pools[qdss_curr_read].bytes_in_buff[0];
	if (!buffer || !bytes_in_buff)
		return;

	if (len >= (DISK_BUF_SIZE - *bytes_in_buff)) {
		qdss_pools[qdss_curr_read].data_ready = 1;
		qdss_pools[qdss_curr_read].free = 0;
		pthread_cond_signal(&qdss_pools[qdss_curr_read].write_cond);
		pthread_mutex_unlock(&(qdss_pools[qdss_curr_read].write_mutex));
		qdss_curr_read = !qdss_curr_read;
		pthread_mutex_lock(&(qdss_pools[qdss_curr_read].read_mutex));
		if (!qdss_pools[qdss_curr_read].free) {
			pthread_mutex_unlock(&(qdss_pools[qdss_curr_read].write_mutex));
			pthread_cond_wait(
							  &(qdss_pools[qdss_curr_read].read_cond),
							  &(qdss_pools[qdss_curr_read].read_mutex));
			pthread_mutex_lock(&(qdss_pools[qdss_curr_read].write_mutex));
		}
		pthread_mutex_unlock(&(qdss_pools[qdss_curr_read].read_mutex));
		buffer = qdss_pools[qdss_curr_read].buffer_ptr[0];
		bytes_in_buff =
		&qdss_pools[qdss_curr_read].bytes_in_buff[0];
	}
	if (len > 0) {
		memcpy(buffer + *bytes_in_buff, ptr, len);
		*bytes_in_buff += len;
		ptr += len;
	}
}

static void* qdss_read_thread(void* param)
{
	int num_bytes_read = 0;
	(void) param;

	do {
		num_bytes_read = read(diag_qdss_node_fd, (void*)qdss_read_buffer,
							  READ_BUF_SIZE);
		if (!num_bytes_read) {
			if (qdss_kill_thread) {
				DIAG_LOGD("diag: In %s, Exit read thread for 0 lenght packet\n", __func__);
				return 0;
			}
			continue;
		}
		fill_qdss_buffer(qdss_read_buffer, num_bytes_read);
		num_bytes_read = 0;
		memset(qdss_read_buffer, 0, READ_BUF_SIZE);

		if (qdss_kill_thread) {
			DIAG_LOGD("diag: In %s, Exit read thread\n", __func__);
			return 0;
		}
	} while (1);

	return 0;
}

static void close_qdss_logging_file(int type)
{
	if (qdss_diag_fd_md[type] > 0)
		close(qdss_diag_fd_md[type]);
	qdss_diag_fd_md[type] = -1;

	if (rename_file_names && qdss_file_name_curr[type][0] != '\0') {
		int status;
		char timestamp_buf[30];
		char new_filename[FILE_NAME_LEN];
		char rename_cmd[RENAME_CMD_LEN];

		get_time_string(timestamp_buf, sizeof(timestamp_buf));

		(void)std_strlprintf(new_filename,
					 FILE_NAME_LEN, "%s%s%s%s%s%s",
					 output_dir[type], "/diag_log",
					 qdss_peripheral_name, "_",
					 timestamp_buf, ".bin");

		/* Create rename command and issue it */
		(void)std_strlprintf(rename_cmd, RENAME_CMD_LEN, "mv %s %s",
							 qdss_file_name_curr[type], new_filename);

		status = system(rename_cmd);
		if (status == -1) {
			DIAG_LOGE("diag: In %s, File rename error (mv), errno: %d\n",
					  __func__, errno);
			DIAG_LOGE("diag: Unable to rename file %s to %s\n",
					  qdss_file_name_curr[type], new_filename);
		} else {
			/* Update current filename */
			strlcpy(qdss_file_name_curr[type], new_filename, FILE_NAME_LEN);
		}
	}
}

void diag_set_qdss_mask(unsigned int diag_qdss_mask)
{
	qdss_mask = diag_qdss_mask;
}

#define S_64K (64*1024)

static void write_to_qdss_file(void *buf, int len, int type) {
	struct stat logging_file_stat;
	char timestamp_buf[30];
	int rc, ret, z;
	if (qdss_diag_fd_md[0] < 0) {
		get_time_string(timestamp_buf, sizeof(timestamp_buf));
		(void)std_strlprintf(qdss_file_name_curr[type],
							FILE_NAME_LEN, "%s%s%s%s%s%s",
							output_dir[type], "/diag_log",
							qdss_peripheral_name, "_", timestamp_buf, ".bin");
		qdss_diag_fd_md[0] = open(qdss_file_name_curr[type],
								  O_CREAT | O_RDWR | O_SYNC | O_TRUNC,
								  0644);
		qdss_diag_fd_dev = qdss_diag_fd_md[0];
		if (qdss_diag_fd_dev < 0) {
			DIAG_LOGE(" File open error, please check");
			DIAG_LOGE(" memory device %d, errno: %d \n",
					  fd_md[type], errno);
		} else {
			DIAG_LOGE(" creating new file %s \n",
					  qdss_file_name_curr[type]);
		}
	}
	if (qdss_diag_fd_dev != -1) {
		if (!stat(qdss_file_name_curr[type], &logging_file_stat)) {
			ret = write(qdss_diag_fd_dev, (const void *)buf, len);
			if (ret > 0)
				qdss_count_written_bytes = qdss_count_written_bytes + len;
			else {
				DIAG_LOGE("diag: In %s, error writing to sd card, %s, errno: %d\n",
						  __func__, strerror(errno), errno);
				if (errno == ENOSPC) {
				/* Delete oldest file */
					rc = -1;
					for (z = 0; z < NUM_PROC; z++) {
						if (!delete_log(z)) rc = 0;
					}

					if (rc) {
						DIAG_LOGE("Delete failed \n");
						return;
					}

					/* Close file if it is big enough */
					if (qdss_count_written_bytes >
						min_file_size) {
						close_qdss_logging_file(type);
						qdss_diag_fd_dev = qdss_diag_fd_md[type];
						qdss_count_written_bytes = 0;
					} else {
						DIAG_LOGE(" Disk Full "
								  "Continuing with "
								  "same file [%d] \n", type);
					}
					write_to_qdss_file(buf, len,
								type);
					return;
				} else DIAG_LOGE(" failed to write "
							 "to file, device may"
							 " be absent, errno: %d\n",
							 errno);
			}
		} else {
			close(qdss_diag_fd_dev);
			qdss_diag_fd_md[0] = -1;
			ret = -EINVAL;
		}
	}
}

static void* qdss_write_thread(void* param) {
	unsigned int i;
	int z = 0;
	unsigned int chunks, last_chunk;
	unsigned char *temp_ptr = NULL;;
	(void)param;

	while (1) {
		temp_ptr = qdss_pools[qdss_curr_write].buffer_ptr[0];
		if (!temp_ptr)
			return NULL;
		pthread_mutex_lock(&(qdss_pools[qdss_curr_write].write_mutex));
		if (!qdss_pools[qdss_curr_write].data_ready) {
			qdss_in_write = 1;
			pthread_cond_wait(&(qdss_pools[qdss_curr_write].write_cond),
							  &(qdss_pools[qdss_curr_write].write_mutex));
			qdss_in_write = 0;
			if (qdss_kill_thread) {
				DIAG_LOGD("diag: In %s:Exit write thread after receiving condition\n", __func__);
				return 0;
			}
		}
		qdss_write_in_progress = 1;

		chunks = qdss_pools[qdss_curr_write].bytes_in_buff[0] /
										S_64K;
		last_chunk = qdss_pools[qdss_curr_write].bytes_in_buff[z] %
										S_64K;
		for (i = 0; i < chunks; i++) {
			write_to_qdss_file(
						qdss_pools[qdss_curr_write].buffer_ptr[0],
						S_64K, 0);
			qdss_pools[qdss_curr_write].buffer_ptr[0] +=
			S_64K;
		}
		if (last_chunk > 0)
			write_to_qdss_file(
						qdss_pools[qdss_curr_write].buffer_ptr[z],
						last_chunk, 0);
		if (qdss_count_written_bytes >= max_file_size) {
			close_qdss_logging_file(0);
			qdss_diag_fd_dev = qdss_diag_fd_md[0];
			qdss_count_written_bytes = 0;
		}

		qdss_write_in_progress = 0;

		/* File pool structure */
		qdss_pools[qdss_curr_write].free = 1;
		qdss_pools[qdss_curr_write].data_ready = 0;
		qdss_pools[qdss_curr_write].bytes_in_buff[0] = 0;
		qdss_pools[qdss_curr_write].buffer_ptr[0] = temp_ptr;
		/* Free Read thread if waiting on same buffer */
		pthread_mutex_lock(&(qdss_pools[qdss_curr_write].read_mutex));
		pthread_cond_signal(&(qdss_pools[qdss_curr_write].read_cond));
		pthread_mutex_unlock(&(qdss_pools[qdss_curr_write].read_mutex));
		pthread_mutex_unlock(&(qdss_pools[qdss_curr_write].write_mutex));

		qdss_curr_write = !qdss_curr_write;

		if (qdss_kill_thread) {
			DIAG_LOGD("diag: In %s: Exit write thread after write completion\n", __func__);
			return 0;
		}
	}
	return NULL;
}

static int check_for_qdss_cmd(uint8* src_ptr)
{
	uint16 cmd_code = 0;
	uint16 version;
	uint16 opcode;
	unsigned int i ;
	unsigned int offset = 0;
	if (!src_ptr)
		return FALSE;

	if (((*src_ptr == DIAG_SUBSYS_CMD_F && *(src_ptr + 1) == DIAG_SUBSYS_DIAG_SERV) ||
		 (((*src_ptr == DIAG_BAD_CMD_F) || (*src_ptr == DIAG_BAD_LEN_F) || (*src_ptr == DIAG_BAD_PARM_F)) &&
		 *(src_ptr + 1) == DIAG_SUBSYS_CMD_F && *(src_ptr + 2) == DIAG_SUBSYS_DIAG_SERV)) ||
		((*src_ptr == DIAG_SUBSYS_CMD_F && *(src_ptr + 1) == DIAG_SUBSYS_QDSS) ||
		 (((*src_ptr == DIAG_BAD_CMD_F) || (*src_ptr == DIAG_BAD_LEN_F) || (*src_ptr == DIAG_BAD_PARM_F)) &&
		 *(src_ptr + 1) == DIAG_SUBSYS_CMD_F && *(src_ptr + 2) == DIAG_SUBSYS_QDSS)))
	{
		if (*src_ptr == DIAG_SUBSYS_CMD_F) {
			memcpy(&cmd_code, src_ptr + 2, sizeof(cmd_code));
		} else {
			memcpy(&cmd_code,src_ptr + 3, sizeof(cmd_code));
		}

		switch (cmd_code) {
		case DIAG_DIAG_STM:
			break;
		case DIAG_QDSS_FILTER_STM:
			break;
		case DIAG_QDSS_TRACE_SINK:
			break;
		case 0x206:
			break;
		case 0x208:
			break;
		case 0x506:
			break;
		case 0x508:
			break;
		default:
			return FALSE;
		}
		return TRUE;
	}
	else
		return FALSE;
}

static void request_qdss_read_buffer()
{
	pthread_mutex_lock(&(qdss_read_buffer_pool[qdss_curr_read_idx].write_rsp_mutex));
	pthread_mutex_lock(&(qdss_read_buffer_pool[qdss_curr_read_idx].read_rsp_mutex));
	if (qdss_read_buffer_pool[qdss_curr_read_idx].data_ready) {
		pthread_mutex_unlock(&(qdss_read_buffer_pool[qdss_curr_read_idx].write_rsp_mutex));
		pthread_cond_wait(&(qdss_read_buffer_pool[qdss_curr_read_idx].read_rsp_cond),
						  &(qdss_read_buffer_pool[qdss_curr_read_idx].read_rsp_mutex));
		pthread_mutex_lock(&(qdss_read_buffer_pool[qdss_curr_read_idx].write_rsp_mutex));
	}
	pthread_mutex_unlock(&(qdss_read_buffer_pool[qdss_curr_read_idx].read_rsp_mutex));
}

int parse_data_for_qdss_rsp(uint8* ptr, int count_received_bytes)
{
	int count = 0;
	uint8_t* src_ptr = NULL;
	unsigned char* dest_ptr = NULL;
	unsigned int src_length = 0, dest_length = 0;
	unsigned int len = 0;
	unsigned int i;
	uint8_t src_byte;
	int bytes_read = 0;
	uint16_t payload_len = 0;

	if (!ptr)
		return -1;

	while (bytes_read < count_received_bytes) {

		src_ptr = ptr + bytes_read;
		src_length = count_received_bytes - bytes_read;

		if (hdlc_disabled) {
			payload_len = *(uint16_t *)(src_ptr + 2);
			if (check_for_qdss_cmd(src_ptr + 4))
			{
				request_qdss_read_buffer();
				dest_ptr = &(qdss_read_buffer_pool[qdss_curr_read_idx].rsp_buf[0]);
				dest_length = QDSS_RSP_BUF_SIZE;
				if (payload_len <= QDSS_RSP_BUF_SIZE)
					memcpy(dest_ptr, src_ptr + 4, payload_len);
				else
					return -1;
				qdss_read_buffer_pool[qdss_curr_read_idx].data_ready = 1;
				pthread_cond_signal(&(qdss_read_buffer_pool[qdss_curr_read_idx].write_rsp_cond));
				pthread_mutex_unlock(&(qdss_read_buffer_pool[qdss_curr_read_idx].write_rsp_mutex));
				qdss_curr_read_idx = !qdss_curr_read_idx;
				bytes_read += payload_len + 5;

			}
			else
				bytes_read += payload_len + 5;

		} else {
			if (check_for_qdss_cmd(src_ptr)) {
				request_qdss_read_buffer();
				dest_ptr = &(qdss_read_buffer_pool[qdss_curr_read_idx].rsp_buf[0]);
				dest_length = QDSS_RSP_BUF_SIZE;
				for (i = 0; i < src_length; i++) {
					src_byte = src_ptr[i];

					if (src_byte == ESC_CHAR) {
						if (i == (src_length - 1)) {
							i++;
							break;
						} else {
							dest_ptr[len++] = src_ptr[++i]
								^ 0x20;
						}
					} else if (src_byte == CTRL_CHAR) {
						if (i == 0 && src_length > 1)
							continue;
						dest_ptr[len++] = src_byte;
						i++;
						break;
					} else {
						dest_ptr[len++] = src_byte;
					}

					if (len >= dest_length) {
						i++;
						break;
					}
				}
				bytes_read += i;
				i = 0;
				len = 0;
				qdss_read_buffer_pool[qdss_curr_read_idx].data_ready = 1;
				pthread_cond_signal(&(qdss_read_buffer_pool[qdss_curr_read_idx].write_rsp_cond));
				pthread_mutex_unlock(&(qdss_read_buffer_pool[qdss_curr_read_idx].write_rsp_mutex));
				qdss_curr_read_idx = !qdss_curr_read_idx;
			} else {
				for (i = 0; i < src_length; i++) {
					if (src_ptr[i] == CTRL_CHAR) {
						i++;
						break;
					}
				}
				bytes_read += i;
				i = 0;
				len = 0;

			}
		}
	}
	return 0;
}

void diag_kill_qdss_threads(void)
{
	int ret = 0, i;

	if (!qdss_mask)
		return;

	/****************************
	 * Signal the config thread *
	 ****************************/

	qdss_kill_thread = 1;

	/****************************************************
	 * Reset the QDSS environment for the peripheral	*
	 ****************************************************/

	if (qdss_mask & DIAG_CON_MPSS) {
		diag_send_cmds_to_peripheral_kill(MSM, DIAG_MODEM_PROC);
		qdss_mask ^= DIAG_CON_MPSS;
	}
	if (qdss_mask & DIAG_CON_LPASS) {
		diag_send_cmds_to_peripheral_kill(MSM, DIAG_LPASS_PROC);
		qdss_mask ^= DIAG_CON_LPASS;
	}
	if (qdss_mask & DIAG_CON_WCNSS) {
		diag_send_cmds_to_peripheral_kill(MSM, DIAG_WCNSS_PROC);
		qdss_mask ^= DIAG_CON_WCNSS;
	}
	if (qdss_mask & DIAG_CON_SENSORS) {
		diag_send_cmds_to_peripheral_kill(MSM, DIAG_SENSORS_PROC);
		qdss_mask ^= DIAG_CON_SENSORS;
	}
	if (qdss_mask & DIAG_CON_APSS) {
		diag_send_cmds_to_peripheral_kill(MSM, DIAG_APPS_PROC);
		qdss_mask ^= DIAG_CON_APSS;
	}

	/****************************
	 * Kill the config thread 	*
	 ****************************/

	if (in_wait_for_qdss_peripheral_status)
		pthread_cond_signal(&qdss_diag_cond);

	ret = pthread_join(qdss_config_thread_hdl, NULL);
	if (ret != 0) {
		DIAG_LOGE("diag: In %s, Error trying to join with qdss config thread: %d\n",
				  __func__, ret);
	}
	DIAG_LOGD("diag: In %s, Successful in killing config thread\n", __func__);

	/****************************
	 * Kill the write thread 	*
	 ****************************/

	if (qdss_in_write)
		pthread_cond_signal(&qdss_pools[qdss_curr_write].write_cond);

	pthread_mutex_destroy(&qdss_diag_mutex);
	pthread_mutex_destroy(&(qdss_read_buffer_pool[0].read_rsp_mutex));
	pthread_mutex_destroy(&(qdss_read_buffer_pool[0].read_rsp_mutex));
	pthread_mutex_destroy(&(qdss_read_buffer_pool[1].write_rsp_mutex));
	pthread_mutex_destroy(&(qdss_read_buffer_pool[1].write_rsp_mutex));
	pthread_cond_destroy(&(qdss_read_buffer_pool[0].read_rsp_cond));
	pthread_cond_destroy(&(qdss_read_buffer_pool[0].write_rsp_cond));
	pthread_cond_destroy(&(qdss_read_buffer_pool[1].read_rsp_cond));
	pthread_cond_destroy(&(qdss_read_buffer_pool[1].write_rsp_cond));

	pthread_cond_destroy(&qdss_diag_cond);

	pthread_mutex_destroy(&(qdss_pools[0].write_mutex));
	pthread_cond_destroy(&(qdss_pools[0].write_cond));
	pthread_mutex_destroy(&(qdss_pools[0].read_mutex));
	pthread_cond_destroy(&(qdss_pools[0].read_cond));
	pthread_mutex_destroy(&(qdss_pools[1].write_mutex));
	pthread_cond_destroy(&(qdss_pools[1].write_cond));
	pthread_mutex_destroy(&(qdss_pools[1].read_mutex));
	pthread_cond_destroy(&(qdss_pools[1].read_cond));

	if (qdss_read_buffer_pool[0].rsp_buf)
		free(qdss_read_buffer_pool[0].rsp_buf);
	if (qdss_read_buffer_pool[1].rsp_buf)
		free(qdss_read_buffer_pool[1].rsp_buf);

	DIAG_LOGE("diag:In %s finished killing qdss threads\n", __func__);
}

