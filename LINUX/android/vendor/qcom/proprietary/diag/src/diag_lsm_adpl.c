/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

              Diag adpl support

GENERAL DESCRIPTION

Implementation of configuring diag over adpl using diag command request/responses
and reading data from adpl, writing adpl data to file.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
#include <stdlib.h>
#include "comdef.h"
#include "stdio.h"
#include "stringl.h"
#include "diag_lsmi.h"
#include "diag_lsm.h"
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
#include <log/log.h>
#endif

#define ADPL_CMD_REQ_BUF_SIZE 50
#define ADPL_SUBSYS_CMD_CODE 16384

#ifndef IPA_IOC_MAGIC
#define IPA_IOC_MAGIC 0xCF
#endif

#ifndef IPA_IOCTL_ODL_GET_AGG_BYTE_LIMIT
#define IPA_IOCTL_ODL_GET_AGG_BYTE_LIMIT 62
struct odl_agg_pipe_info {
	uint16_t agg_byte_limit;
};
#define IPA_IOC_ODL_GET_AGG_BYTE_LIMIT _IOWR(IPA_IOC_MAGIC, \
				IPA_IOCTL_ODL_GET_AGG_BYTE_LIMIT, \
				struct odl_agg_pipe_info *)
#endif

char adpl_file_name_curr[NUM_PROC][FILE_NAME_LEN];
unsigned char adpl_read_buffer[READ_BUF_SIZE];
unsigned char adpl_cmd_req_buf[50];
pthread_t adpl_read_thread_hdl;
pthread_t adpl_write_thread_hdl;
pthread_t adpl_config_thread_hdl;
static int adpl_count_written_bytes;
int adpl_diag_fd_md[NUM_PROC] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
int adpl_diag_fd_dev = -1;
int diag_adpl_node_fd = -1;
int adpl_state = 0;
int adpl_config_done = 0;

/* Static array for workaround */
static unsigned char adpl_static_buffer[2][DISK_BUF_SIZE];

static int adpl_curr_write;
static int adpl_curr_read;
static int adpl_write_in_progress;
int in_wait_for_adpl_status = 0;
int adpl_in_write = 0;
int adpl_kill_thread = 0;
int adpl_enabled = 0;
int adpl_modem_down;
extern unsigned long max_file_size;
extern unsigned long min_file_size;
struct adpl_read_buf_pool {
	int data_ready;
	pthread_mutex_t read_rsp_mutex;
	pthread_mutex_t write_rsp_mutex;
	pthread_cond_t write_rsp_cond;
	pthread_cond_t read_rsp_cond;
};

typedef PACK(struct) {
	uint8 cmd_code;
	uint8 subsys_id;
	uint16 subsys_cmd_code;
	uint8 dpl_version;
	uint8 agg_pkt_limit;
	uint16 agg_byte_limit;
	uint8 dpl_cmd;
	uint8 res[3];
} diag_adpl_req;

uint16 agg_byte_limit = 0;
static struct adpl_read_buf_pool adpl_read_buffer_pool[2];
pthread_mutex_t adpl_diag_mutex;
pthread_mutex_t adpl_config_mutex;
pthread_cond_t adpl_diag_cond;
pthread_cond_t adpl_config_cond;

volatile int adpl_curr_read_idx = 0;
volatile int adpl_curr_write_idx = 0;

struct buffer_pool adpl_pools[] = {
	[0] = {
		.free		=	1,
		.data_ready	=	0,
	},
	[1] = {
		.free		=	1,
		.data_ready	=	0,
	},

};
static void* adpl_read_thread(void* param);
static void* adpl_write_thread(void* param);
void* adpl_config_thread(void* param);

void diag_notify_adpl_thread()
{
	DIAG_LOGE("%s: Signalling ADPL config thread\n", __func__);
	pthread_cond_signal(&adpl_diag_cond);
}

int diag_adpl_init()
{
	int ret;

	pthread_mutex_init(&adpl_diag_mutex, NULL);
	pthread_mutex_init(&adpl_config_mutex, NULL);
	pthread_mutex_init(&(adpl_read_buffer_pool[0].read_rsp_mutex), NULL);
	pthread_mutex_init(&(adpl_read_buffer_pool[1].read_rsp_mutex), NULL);
	pthread_mutex_init(&(adpl_read_buffer_pool[0].write_rsp_mutex), NULL);
	pthread_mutex_init(&(adpl_read_buffer_pool[1].write_rsp_mutex), NULL);
	pthread_cond_init(&(adpl_read_buffer_pool[0].read_rsp_cond), NULL);
	pthread_cond_init(&(adpl_read_buffer_pool[0].write_rsp_cond), NULL);
	pthread_cond_init(&(adpl_read_buffer_pool[1].read_rsp_cond), NULL);
	pthread_cond_init(&(adpl_read_buffer_pool[1].write_rsp_cond), NULL);

	adpl_read_buffer_pool[0].data_ready = 0;
	adpl_read_buffer_pool[1].data_ready = 0;
	pthread_cond_init(&adpl_diag_cond, NULL);
	pthread_cond_init(&adpl_config_cond, NULL);
	pthread_mutex_init(&(adpl_pools[0].write_mutex), NULL);
	pthread_cond_init(&(adpl_pools[0].write_cond), NULL);
	pthread_mutex_init(&(adpl_pools[0].read_mutex), NULL);
	pthread_cond_init(&(adpl_pools[0].read_cond), NULL);
	pthread_mutex_init(&(adpl_pools[1].write_mutex), NULL);
	pthread_cond_init(&(adpl_pools[1].write_cond), NULL);
	pthread_mutex_init(&(adpl_pools[1].read_mutex), NULL);
	pthread_cond_init(&(adpl_pools[1].read_cond), NULL);

	adpl_pools[0].buffer_ptr[0] = adpl_static_buffer[0];
	adpl_pools[1].buffer_ptr[0] = adpl_static_buffer[1];

	adpl_pools[0].bytes_in_buff[0] = 0;
	adpl_pools[1].bytes_in_buff[0] = 0;
	adpl_modem_down = 0;

	pthread_create(&adpl_config_thread_hdl, NULL, adpl_config_thread, NULL);
	if (adpl_config_thread_hdl == 0) {
		DIAG_LOGE("%s: Failed to create config thread", __func__);
		goto failure_case;
	}

	pthread_mutex_lock(&adpl_config_mutex);
	pthread_cond_wait(&adpl_config_cond, &adpl_config_mutex);

	pthread_create(&adpl_read_thread_hdl, NULL, adpl_read_thread, NULL);
	if (adpl_read_thread_hdl == 0) {
		DIAG_LOGE("%s: Failed to create read thread", __func__);
		goto failure_case;
	}

	pthread_create(&adpl_write_thread_hdl, NULL, adpl_write_thread, NULL);
	if (adpl_write_thread_hdl == 0) {
		DIAG_LOGE("%s: Failed to create write thread", __func__);
		goto failure_case;
	}

	if (!adpl_config_done)
		goto failure_case;

	return 0;

 failure_case:
	close(diag_adpl_node_fd);
	diag_adpl_node_fd = DIAG_INVALID_HANDLE;
	return -1;

}

int diag_send_cmds_to_config_adpl(int type, int enable)
{
	int offset = 0, length = 0;
	diag_adpl_req* req = NULL;
	unsigned char *ptr = adpl_cmd_req_buf;

	*(int *)ptr = USER_SPACE_RAW_DATA_TYPE;
	offset += sizeof(int);
	if (type == MDM) {
		*(int *)(ptr + offset) = MDM_TOKEN;
		offset += sizeof(int);
	} else if (type == QSC) {
		*(int *)(ptr + offset) = QSC_TOKEN;
		offset += sizeof(int);
	}

	ptr = ptr + offset;
	req = (diag_adpl_req*)ptr;

	/****************************************************************
	 * 	0x4B, 0x64, 0, 0x40, 0x02, 0, 0, 0x3C, X, 0, 0, 0			*
	 *												 				*
	 * 	X = 1 To Enable ADPL							 			*
	 * 	X = 0 To Disable ADPL						 				*
	 ****************************************************************/

	req->cmd_code = DIAG_SUBSYS_CMD_F;
	req->subsys_id = DIAG_SUBSYS_DS_IPA;
	req->subsys_cmd_code = ADPL_SUBSYS_CMD_CODE;
	req->dpl_version = 0x02;
	req->agg_pkt_limit = 0x00;
	req->agg_byte_limit = agg_byte_limit;
	if(enable)
		req->dpl_cmd = 0x01;
	else
		req->dpl_cmd = 0x00;
	memset(req->res, 0x00, sizeof(req->res));

	/* Send ADPL Command */
	length = sizeof(diag_adpl_req);
	if (length + offset <= ADPL_CMD_REQ_BUF_SIZE) {
		return diag_send_data(adpl_cmd_req_buf, offset + length);
	}
	return 0;
}

int diag_send_cmds_to_disable_adpl(int in_ssr)
{
	int ret = 0;
	if(!in_ssr && (diag_send_cmds_to_config_adpl(MSM,0) < 0)) {
		DIAG_LOGE(" %s: Failed to send disable adpl command\n", __func__);
	}
	DIAG_LOGE(" %s: Closing IPA node to adpl driver\n", __func__);
	ret = close(diag_adpl_node_fd);
	diag_adpl_node_fd = DIAG_INVALID_HANDLE;
	DIAG_LOGE(" %s: Closed IPA node, ret: %d\n", __func__, ret);
	return 0;
}

void* adpl_config_thread(void* param)
{
	(void)param;
	int err = 0, retry_cnt;
	/* Open /dev/ipa_adpl node */
	diag_adpl_node_fd = open("/dev/ipa_adpl", O_RDONLY);
	if (diag_adpl_node_fd == DIAG_INVALID_HANDLE) {
		DIAG_LOGE(" %s: Failed to open IPA node to adpl driver, error = %d\n", __func__, errno);
		adpl_config_done = 0;
		pthread_cond_signal(&adpl_config_cond);
		return 0;
	}
	while (1) {
		/* Call ioctl for getting buffer size, sleep for some time for ipa to get configured */
		sleep(2);
		retry_cnt = 0;
retry_ioctl:
		err = ioctl(diag_adpl_node_fd, IPA_IOC_ODL_GET_AGG_BYTE_LIMIT , &agg_byte_limit);
		if (err) {
			DIAG_LOGE("diag: In %s, unable to get adpl agg byte info, ret: %d, error: %d, retry_cnt = %d\n",
				__func__, err, errno, retry_cnt);
			if (retry_cnt < 5) {
				sleep(2);
				retry_cnt++;
				goto retry_ioctl;
			}
			close(diag_adpl_node_fd);
			diag_adpl_node_fd = DIAG_INVALID_HANDLE;
			adpl_config_done = 0;
			pthread_cond_signal(&adpl_config_cond);
			return 0;
		}
		/* Send diag command to enable adpl */
		if(diag_send_cmds_to_config_adpl(MSM, 1) < 0) {
			DIAG_LOGE(" %s: Failed to send enable adpl command\n", __func__);
			close(diag_adpl_node_fd);
			diag_adpl_node_fd = DIAG_INVALID_HANDLE;
			adpl_config_done = 0;
			pthread_cond_signal(&adpl_config_cond);
			return 0;
		}
		DIAG_LOGE(" %s: Successfully configured ADPL ODL Logging\n", __func__);
		adpl_config_done = 1;
		pthread_cond_signal(&adpl_config_cond);
		pthread_mutex_lock(&adpl_diag_mutex);
		in_wait_for_adpl_status = 1;
		pthread_cond_wait(&adpl_diag_cond, &adpl_diag_mutex);
		in_wait_for_adpl_status = 0;
		pthread_mutex_unlock(&adpl_diag_mutex);
		if (adpl_kill_thread)
			return 0;
		if (adpl_modem_down)
			adpl_modem_down = 0;
	}
}
void fill_adpl_buffer(void* ptr, int len)
{
	unsigned char* buffer = NULL;
	unsigned int* bytes_in_buff = NULL;
	buffer = adpl_pools[adpl_curr_read].buffer_ptr[0];
	bytes_in_buff = &adpl_pools[adpl_curr_read].bytes_in_buff[0];
	if (!buffer || !bytes_in_buff)
		return;

	if (len >= (DISK_BUF_SIZE - *bytes_in_buff)) {
		adpl_pools[adpl_curr_read].data_ready = 1;
		adpl_pools[adpl_curr_read].free = 0;
		pthread_cond_signal(&adpl_pools[adpl_curr_read].write_cond);
		pthread_mutex_unlock(&(adpl_pools[adpl_curr_read].write_mutex));
		adpl_curr_read = !adpl_curr_read;
		pthread_mutex_lock(&(adpl_pools[adpl_curr_read].read_mutex));
		if (!adpl_pools[adpl_curr_read].free) {
			pthread_mutex_unlock(&(adpl_pools[adpl_curr_read].write_mutex));
			pthread_cond_wait(&(adpl_pools[adpl_curr_read].read_cond),
				&(adpl_pools[adpl_curr_read].read_mutex));
			pthread_mutex_lock(&(adpl_pools[adpl_curr_read].write_mutex));
		}
		pthread_mutex_unlock(&(adpl_pools[adpl_curr_read].read_mutex));
		buffer = adpl_pools[adpl_curr_read].buffer_ptr[0];
		bytes_in_buff =
		&adpl_pools[adpl_curr_read].bytes_in_buff[0];
	}
	if (len > 0) {
		memcpy(buffer + *bytes_in_buff, ptr, len);
		*bytes_in_buff += len;
		ptr += len;
	}
}

static void* adpl_read_thread(void* param)
{
	int num_bytes_read = 0;
	(void) param;
	do {
		if(!adpl_kill_thread &&
			(adpl_modem_down || (diag_adpl_node_fd == DIAG_INVALID_HANDLE))) {
			continue;
		}
		num_bytes_read = 0;
		memset(adpl_read_buffer, 0, READ_BUF_SIZE);
		num_bytes_read = read(diag_adpl_node_fd, (void*)adpl_read_buffer,
							  READ_BUF_SIZE);
		if (num_bytes_read <= 0) {
			if (adpl_kill_thread) {
				DIAG_LOGD("diag: In %s, Exit read thread for 0 length packet\n", __func__);
				return 0;
			}
			continue;
		}
		fill_adpl_buffer(adpl_read_buffer, num_bytes_read);

		if (adpl_kill_thread) {
			DIAG_LOGD("diag: In %s, Exit read thread\n", __func__);
			return 0;
		}
	} while (1);

	return 0;
}

static void close_adpl_logging_file(int type)
{
	if (adpl_diag_fd_md[type] > 0)
		close(adpl_diag_fd_md[type]);
	adpl_diag_fd_md[type] = -1;

	if (rename_file_names && adpl_file_name_curr[type][0] != '\0') {
		int status;
		char timestamp_buf[30];
		char new_filename[FILE_NAME_LEN];
		char rename_cmd[RENAME_CMD_LEN];

		get_time_string(timestamp_buf, sizeof(timestamp_buf));

		(void)std_strlprintf(new_filename,
					 FILE_NAME_LEN, "%s%s%s%s",
					 output_dir[type], "/diag_log_ADPL_",
					 timestamp_buf, ".adpl");

		/* Create rename command and issue it */
		(void)std_strlprintf(rename_cmd, RENAME_CMD_LEN, "mv %s %s",
							 adpl_file_name_curr[type], new_filename);

		status = system(rename_cmd);
		if (status == -1) {
			DIAG_LOGE("diag: In %s, File rename error (mv), errno: %d\n",
					  __func__, errno);
			DIAG_LOGE("diag: Unable to rename file %s to %s\n",
					  adpl_file_name_curr[type], new_filename);
		} else {
			/* Update current filename */
			strlcpy(adpl_file_name_curr[type], new_filename, FILE_NAME_LEN);
		}
	}
}

#define S_64K (64*1024)

static void write_to_adpl_file(void *buf, int len, int type)
{
	struct stat logging_file_stat;
	char timestamp_buf[30];
	int rc, ret, z;
	if (adpl_diag_fd_md[0] < 0) {
		get_time_string(timestamp_buf, sizeof(timestamp_buf));
		(void)std_strlprintf(adpl_file_name_curr[type],
							FILE_NAME_LEN, "%s%s%s%s",
							output_dir[type], "/diag_log_ADPL_",
							timestamp_buf, ".adpl");

		adpl_diag_fd_md[0] = open(adpl_file_name_curr[type],
								  O_CREAT | O_RDWR,
								  0644);
		adpl_diag_fd_dev = adpl_diag_fd_md[0];
		if (adpl_diag_fd_dev < 0) {
			DIAG_LOGE(" File open error, please check");
			DIAG_LOGE(" memory device %d, errno: %d \n",
					  fd_md[type], errno);
		} else {
			DIAG_LOGE(" creating new file %s \n",
					  adpl_file_name_curr[type]);
		}
	}
	if (adpl_diag_fd_dev != -1) {
		if (!stat(adpl_file_name_curr[type], &logging_file_stat)) {
			ret = write(adpl_diag_fd_dev, (const void *)buf, len);
			if (ret > 0)
				adpl_count_written_bytes = adpl_count_written_bytes + len;
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
					if (adpl_count_written_bytes >
						min_file_size) {
						close_adpl_logging_file(type);
						adpl_diag_fd_dev = adpl_diag_fd_md[type];
						adpl_count_written_bytes = 0;
					} else {
						DIAG_LOGE(" Disk Full "
								  "Continuing with "
								  "same file [%d] \n", type);
					}
					write_to_adpl_file(buf, len,
								type);
					return;
				} else DIAG_LOGE(" failed to write "
							 "to file, device may"
							 " be absent, errno: %d\n",
							 errno);
			}
		} else {
			close(adpl_diag_fd_dev);
			adpl_diag_fd_md[0] = -1;
			ret = -EINVAL;
		}
	}
}

static void* adpl_write_thread(void* param)
{
	unsigned int i;
	int z = 0;
	unsigned int chunks, last_chunk;
	unsigned char *temp_ptr = NULL;;
	(void)param;

	while (1) {
		temp_ptr = adpl_pools[adpl_curr_write].buffer_ptr[0];
		if (!temp_ptr)
			return NULL;
		pthread_mutex_lock(&(adpl_pools[adpl_curr_write].write_mutex));
		if (!adpl_pools[adpl_curr_write].data_ready) {
			adpl_in_write = 1;
			pthread_cond_wait(&(adpl_pools[adpl_curr_write].write_cond),
							  &(adpl_pools[adpl_curr_write].write_mutex));
			adpl_in_write = 0;
			if (adpl_kill_thread) {
				DIAG_LOGD("diag: In %s:Exit write thread after receiving condition\n", __func__);
				return 0;
			}
		}
		adpl_write_in_progress = 1;

		chunks = adpl_pools[adpl_curr_write].bytes_in_buff[0] /
										S_64K;
		last_chunk = adpl_pools[adpl_curr_write].bytes_in_buff[z] %
										S_64K;
		for (i = 0; i < chunks; i++) {
			write_to_adpl_file(
						adpl_pools[adpl_curr_write].buffer_ptr[0],
						S_64K, 0);
			adpl_pools[adpl_curr_write].buffer_ptr[0] +=
			S_64K;
		}
		if (last_chunk > 0) {
			write_to_adpl_file(
						adpl_pools[adpl_curr_write].buffer_ptr[z],
						last_chunk, 0);
		}
		if (adpl_count_written_bytes >= max_file_size) {
			close_adpl_logging_file(0);
			adpl_diag_fd_dev = adpl_diag_fd_md[0];
			adpl_count_written_bytes = 0;
		}

		adpl_write_in_progress = 0;

		/* File pool structure */
		adpl_pools[adpl_curr_write].free = 1;
		adpl_pools[adpl_curr_write].data_ready = 0;
		adpl_pools[adpl_curr_write].bytes_in_buff[0] = 0;
		adpl_pools[adpl_curr_write].buffer_ptr[0] = temp_ptr;
		/* Free Read thread if waiting on same buffer */
		pthread_mutex_lock(&(adpl_pools[adpl_curr_write].read_mutex));
		pthread_cond_signal(&(adpl_pools[adpl_curr_write].read_cond));
		pthread_mutex_unlock(&(adpl_pools[adpl_curr_write].read_mutex));
		pthread_mutex_unlock(&(adpl_pools[adpl_curr_write].write_mutex));

		adpl_curr_write = !adpl_curr_write;

		if (adpl_kill_thread) {
			DIAG_LOGD("diag: In %s: Exit write thread after write completion\n", __func__);
			return 0;
		}
	}
	return NULL;
}

void diag_kill_adpl_threads(void)
{
	int ret = 0, i;

	if (!adpl_enabled)
		return;

	/****************************
	 * Signal the config thread *
	 ****************************/

	adpl_kill_thread = 1;

	/****************************************************
	 * Reset the adpl environment	*
	 ****************************************************/

	diag_send_cmds_to_disable_adpl(0);

	/****************************
	 * Kill the config thread 	*
	 ****************************/

	if (in_wait_for_adpl_status)
		pthread_cond_signal(&adpl_diag_cond);

	ret = pthread_join(adpl_config_thread_hdl, NULL);
	if (ret != 0) {
		DIAG_LOGE("diag: In %s, Error trying to join with adpl config thread: %d\n",
				  __func__, ret);
	}
	DIAG_LOGD("diag: In %s, Successful in killing config thread\n", __func__);

	/****************************
	 * Kill the write thread 	*
	 ****************************/

	if (adpl_in_write)
		pthread_cond_signal(&adpl_pools[adpl_curr_write].write_cond);

	pthread_mutex_destroy(&adpl_diag_mutex);
	pthread_mutex_destroy(&(adpl_read_buffer_pool[0].read_rsp_mutex));
	pthread_mutex_destroy(&(adpl_read_buffer_pool[0].write_rsp_mutex));
	pthread_mutex_destroy(&(adpl_read_buffer_pool[1].read_rsp_mutex));
	pthread_mutex_destroy(&(adpl_read_buffer_pool[1].write_rsp_mutex));
	pthread_cond_destroy(&(adpl_read_buffer_pool[0].read_rsp_cond));
	pthread_cond_destroy(&(adpl_read_buffer_pool[0].write_rsp_cond));
	pthread_cond_destroy(&(adpl_read_buffer_pool[1].read_rsp_cond));
	pthread_cond_destroy(&(adpl_read_buffer_pool[1].write_rsp_cond));

	pthread_cond_destroy(&adpl_diag_cond);

	pthread_mutex_destroy(&(adpl_pools[0].write_mutex));
	pthread_cond_destroy(&(adpl_pools[0].write_cond));
	pthread_mutex_destroy(&(adpl_pools[0].read_mutex));
	pthread_cond_destroy(&(adpl_pools[0].read_cond));
	pthread_mutex_destroy(&(adpl_pools[1].write_mutex));
	pthread_cond_destroy(&(adpl_pools[1].write_cond));
	pthread_mutex_destroy(&(adpl_pools[1].read_mutex));
	pthread_cond_destroy(&(adpl_pools[1].read_cond));


	DIAG_LOGE("diag:In %s finished killing adpl threads\n", __func__);
}
