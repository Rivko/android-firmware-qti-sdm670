/*
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

 * 2015-2016 Qualcomm Atheros, Inc.
 *
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */

/*
 * hostapd / VLAN initialization
 * Copyright 2003, Instant802 Networks, Inc.
 * Copyright 2005-2006, Devicescape Software, Inc.
 * Copyright (c) 2009, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

/*
 * Copyright (c) 2013-2014 The Linux Foundation. All rights reserved.
 *
 * Previously licensed under the ISC license by Qualcomm Atheros, Inc.
 *
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all
 * copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * This file was originally distributed by Qualcomm Atheros, Inc.
 * under proprietary terms before Copyright ownership was assigned
 * to the Linux Foundation.
 */



#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <getopt.h>
#include <limits.h>
#include <asm/types.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include "netlink/genl/genl.h"
#include "netlink/genl/family.h"
#include "netlink/genl/ctrl.h"
#include <netlink-private/object-api.h>
#include <netlink-private/types.h>
#include <netlink/netlink.h>
#include <net/if.h>
#include <netlink/socket.h>
#ifdef CNSS_DIAG_PLATFORM_WIN
#include <syslog.h>
#endif
#ifndef CNSS_DIAG_PLATFORM_WIN
#include <sys/capability.h>
#endif
#include <sys/prctl.h>
#include <sys/statvfs.h>
#include <dirent.h>
#include <linux/prctl.h>
#include <pwd.h>
#ifdef ANDROID
#include <private/android_filesystem_config.h>
#endif
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/wireless.h>

#ifndef NO_DIAG_SUPPORT
#include "event.h"
#include "msg.h"

#include "diag_lsm.h"
#include "diagcmd.h"
#include "diag.h"
#endif

#include "cld-diag-parser.h"

#define CNSS_INTF "wlan0"
#define IFACE_UP 1
#define IFACE_DOWN 0
#define DELAY_IN_S 3
#define FREE_MEMORY_THRESHOLD 100
#define STRINGIFY(x) #x
#define STR(x) STRINGIFY(x)
#define MAX_STR_LEN 255

#ifdef CNSS_DIAG_PLATFORM_WIN
#define ALOGE printf
#endif

const char options[] =
"Options:\n\
-f, --logfile(Currently file path is fixed)\n\
-c, --console (prints the logs in the console)\n\
-s, --silent (No print will come when logging)\n\
-S, --syslog (Prints will logs in syslog)\n\
-q, --qxdm  (prints the logs in the qxdm)\n\
-x, --qxdm_sync (QXDM log packet format)\n\
-l, --qxdm_sync_log_file (QXDM log packet format QMDL2 file)\n\
-d, --debug  (more prints in logcat, check logcat \n\
-s ROME_DEBUG, example to use: -q -d or -c -d)\n\
-b --buffer_size ( example to use : -b 64(in KBs)\n\
-m --cnss_diag_config_file_loc ( example to use : -m /data/misc/cnss_diag.conf)\n\
-u, --Write fw/host logs to a local buffer\n\
-w, --This option along with '-u' facilitates to write decoded fw/host logs to buffer\n\
The options can also be given in the abbreviated form --option=x or -o x. \
The options can be given in any order";

char *log_file_name_prefix[LOG_FILE_MAX] = {
	[HOST_LOG_FILE] = "host_driver_logs_",
	[FW_LOG_FILE] = "cnss_fw_logs_",
	[HOST_QXDM_LOG_FILE] = "host_qxdm_driver_logs_",
	[FW_QXDM_LOG_FILE] = "cnss_fw_qxdm_logs_",
	[PKTLOG_FILE] = "txrx_pktlog_"};
char *log_file_name_extn[LOG_FILE_MAX] = {
	[HOST_LOG_FILE] = "txt",
	[FW_LOG_FILE] = "txt",
	[HOST_QXDM_LOG_FILE] = "qmdl2",
	[FW_QXDM_LOG_FILE] = "qmdl2",
	[PKTLOG_FILE] = "dat"};

struct sockaddr_nl src_addr, dest_addr;
struct nlmsghdr *nlh = NULL;
struct iovec iov;
static int sock_fd = -1;
struct msghdr msg;
char hw_version[MAX_SIZE];

const char *progname;
int32_t optionflag = 0;
int log_path_flag = WRITE_TO_INTERNAL_SDCARD;
int delayed_count = 0;

int avail_space = 0;
int max_file_size = MAX_FILE_SIZE;
int max_archives = MAX_FILE_INDEX;
int max_pktlog_archives = DEFAULT_PKTLOG_ARCHIVES;

unsigned int configured_buffer_size = 0;
int free_mem_threshold = FREE_MEMORY_THRESHOLD;
char wlan_log_dir_path[MAX_FILENAME_SIZE];

int max_no_buffer = 0;
int max_buff_size = DEFAULT_BUFFER_SIZE_MAX;
uint32_t max_pktlog_buff_size = DEFAULT_PKTLOG_BUFFER_SIZE;

struct cnss_log_file_data log_file[LOG_FILE_MAX];
uint8_t gwlan_dev = CNSS_DIAG_WLAN_DEV_UNDEF;
char *cnss_diag_wlan_dev_name[CNSS_DIAG_WLAN_DEV_MAX] = {
	[CNSS_DIAG_WLAN_DEV_UNDEF] = "X_X",
	[CNSS_DIAG_WLAN_ROM_DEV] = "QCA6174",
	[CNSS_DIAG_WLAN_TUF_DEV] = "QCA93",
	[CNSS_DIAG_WLAN_HEL_DEV] = "WCN3990",
	[CNSS_DIAG_WLAN_NAP_DEV] = "XXX_XXX"};

char *cnssdiag_config_file = "/data/misc/wifi/cnss_diag.conf";
char log_capture_loc[MAX_SIZE] = "/sdcard/wlan_logs/";
char hbuffer_log_file[MAX_FILENAME_SIZE] =
		 "/sdcard/wlan_logs/buffered_cnsshost_log.txt";
char fbuffer_log_file[MAX_FILENAME_SIZE] =
		 "/sdcard/wlan_logs/buffered_cnssfw_log.txt";



boolean isDriverLoaded = FALSE;
#ifdef CONFIG_CLD80211_LIB
struct cld80211_ctx *cldctx = NULL;
#endif

char *line_trim(char *);
static void
usage(void)
{
	fprintf(stderr, "Usage:\n%s options\n", progname);
	fprintf(stderr, "%s\n", options);
	exit(-1);
}
/* function to find whether file exists or not */
int doesFileExist(const char *filename) {
	struct stat st;
	int result = stat(filename, &st);
	return result == 0;
}

static uint32_t get_le32(const uint8_t *pos)
{
	return pos[0] | (pos[1] << 8) | (pos[2] << 16) | (pos[3] << 24);
}


/* Opens a directory wlan_logs and searches the same for the presence of
 * host and firmware log files. Sets the index of the file which is used
 * to store the logs before the reboot.
 */


void readDir(const char *dirName, enum fileType type) {
	struct DIR *fdir;
	struct dirent *dirent;
	int *files = NULL;
	char file_name[32];
	int i = 0, found = 0;
	int archives = 0;

	if (type == PKTLOG_FILE) {
		archives = max_pktlog_archives;
	} else {
		archives = max_archives;
	}
	files = (int *)malloc(sizeof(int) * archives);
	if (NULL == files) {
	    debug_printf("%s: failed to allocate memory to host_files\n", __func__);
	    return;
	}

	memset(files, 0, (sizeof(int) * archives));
	fdir = opendir(dirName);
	if (NULL == fdir) {
	    debug_printf("%s: fdir is NULL\n", __func__);
	    free(files);
	    return;
	}
	chdir(dirName);
	while ((dirent = readdir(fdir)) != NULL) {
		found = 0;
		for (i = 0; i < archives; i++) {
			snprintf(file_name, sizeof(file_name), "%s%03d.%s",
					log_file_name_prefix[type], i, log_file_name_extn[type]);

			if ((0 == (strcmp(dirent->d_name, file_name)))) {
				files[i] = 1;
				found = 1;
			}
			if (found)
				break;
		}
	}
/*
 * value 0 at index 'i' indicates, host log file current.txt will be renamed
 * with the filename at 'i'th index.
  */
	i = 0;
	while (i < archives) {
		if (!files[i]) {
			log_file[type].index = i;
			break;
		}
		i++;
	}
	debug_printf("%s: File Index: HOST_LOG_FILE: %d, HOST_QXDM_LOG_FILE: %d, FW_LOG_FILE: %d\n",
		 __func__, log_file[HOST_LOG_FILE].index, log_file[HOST_QXDM_LOG_FILE].index, log_file[FW_LOG_FILE].index);
	free(files);
	closedir(fdir);
}

/*
 * rename host/firmware current.txt logfile with the corresponding
 * host/firmware log file with proper index and delete its next
 * index file to identify the last file name used to store the logs
 * after a reboot.
 */


void backup_file(enum fileType type)
{
	char newFileName[100];
	char delFileName[100];
	int ret =0;
	int archives = 0;

	if (type >= LOG_FILE_MAX)
		return;

	if (type == PKTLOG_FILE) {
		archives = max_pktlog_archives;
	} else {
		archives = max_archives;
	}
	snprintf(newFileName, sizeof(newFileName), "%s%s%03d.%s",
			wlan_log_dir_path, log_file_name_prefix[type],
			log_file[type].index, log_file_name_extn[type]);
	errno = 0;
	ret = rename(log_file[type].name, newFileName);
	log_file[type].fp = NULL;
	log_file[type].index++;
	if (log_file[type].index > archives)
		log_file[type].index = 0;
	snprintf(delFileName, sizeof(delFileName), "%s%s%03d.%s",
			wlan_log_dir_path, log_file_name_prefix[type],
			log_file[type].index, log_file_name_extn[type]);
	unlink(delFileName);
}

static void cleanup(void)
{
	int i;
	if (sock_fd)
		close(sock_fd);
	for (i = HOST_LOG_FILE; i < LOG_FILE_MAX; i++) {
		if (i == FW_QXDM_LOG_FILE)
			buffer_fw_logs_log_pkt("", TRUE);
		if(log_file[i].fp) {
	                fwrite(log_file[i].buf, sizeof(char), (log_file[i].buf_ptr - log_file[i].buf), log_file[i].fp);
			fflush(log_file[i].fp);
			fclose(log_file[i].fp);
		}
		if (log_file[i].buf) {
			free(log_file[i].buf);
			log_file[i].buf = NULL;
		}
	}
}

static void clean_all_buff(struct cnss_log_file_data *lfd)
{
	t_buffer *next_buff = NULL, *start_buff;

	pthread_mutex_lock(&lfd->buff_lock);
	start_buff = (t_buffer *)lfd->buf;

	while (start_buff)
	{
		next_buff = start_buff->next;
		free(start_buff->start);
		free(start_buff);
		if (start_buff == next_buff)
			break;
		start_buff = next_buff;
	}
	pthread_mutex_unlock(&lfd->buff_lock);
}

static void stop(int32_t signum)
{
	UNUSED(signum);
#ifdef CONFIG_CLD80211_LIB
	exit_cld80211_recv(cldctx);
#endif
	if (optionflag & LOG_BUFF_FLAG) {
		printf("free all buffers\n ");
		clean_all_buff(&log_file[BUFFER_HOST_FILE]);
		log_file[BUFFER_HOST_FILE].buf = NULL;
		clean_all_buff(&log_file[BUFFER_FW_FILE]);
		log_file[BUFFER_FW_FILE].buf = NULL;
	}
	if(optionflag & LOGFILE_FLAG){
		printf("Recording stopped\n");
		cleanup();
	}

	if (pthread_mutex_destroy(&log_file[BUFFER_HOST_FILE].buff_lock))
		ALOGE("Failed to destroy host buff_lock");

	if (pthread_mutex_destroy(&log_file[BUFFER_FW_FILE].buff_lock))
		ALOGE("Failed to destroy firmware buff_lock");

	exit(0);
}

static void logbuffer_to_file(struct cnss_log_file_data *lfd)
{
	t_buffer *start_pos = (t_buffer *)lfd->buf_ptr;
	t_buffer *buffer_log;
	FILE *fp;
	size_t len;

	if (start_pos == NULL)
		return;
	fp = fopen(lfd->name, "w+");
	if (fp == NULL) {
		android_printf("Failed to open file %s\n", lfd->name);
		return;
	}

	pthread_mutex_lock(&lfd->buff_lock);
	buffer_log = start_pos->next;

	while (1) {
		len = fwrite(buffer_log->start, sizeof(char),
			     buffer_log->end - buffer_log->start, fp);
		if (len != (size_t)(buffer_log->end - buffer_log->start)) {
			android_printf("fwrite failed with len = %zu\n", len);
			break;
		}

		if (buffer_log == start_pos)
			break;
		buffer_log = buffer_log->next;
	}
	pthread_mutex_unlock(&lfd->buff_lock);
	fclose(fp);
}

static void update_buff_to_file(int32_t signum)
{
	UNUSED(signum);

	logbuffer_to_file(&log_file[BUFFER_HOST_FILE]);
	logbuffer_to_file(&log_file[BUFFER_FW_FILE]);

	android_printf("Written buffers successfully into files\n");
}

static void default_handler_sigusr1(int32_t signum)
{
	android_printf("SIGUSR1: %d is reserved for buffer logging\n", signum);
}

void process_cnss_log_file(uint8_t *dbgbuf)
{
	uint16_t length = 0;
	uint32_t dropped = 0;
	uint32_t timestamp = 0;
	uint32_t res =0;
	struct dbglog_slot *slot = (struct dbglog_slot *)dbgbuf;
	if (NULL != log_file[FW_LOG_FILE].fp)
		fseek(log_file[FW_LOG_FILE].fp, ftell(log_file[FW_LOG_FILE].fp), SEEK_SET);
	timestamp = get_le32((uint8_t *)&slot->timestamp);
	length = get_le32((uint8_t *)&slot->length);
	dropped = get_le32((uint8_t *)&slot->dropped);
	if (!((optionflag & SILENT_FLAG) == SILENT_FLAG)) {
		/* don't like this have to fix it */
		printf("Read bytes %ld timestamp=%u length=%u fw dropped=%u\n",
		    (log_file[FW_LOG_FILE].fp != NULL )? ftell(log_file[FW_LOG_FILE].fp) : 0, timestamp, length, dropped);
	}
	if (NULL != log_file[FW_LOG_FILE].fp) {
		if ((res = fwrite(dbgbuf, RECLEN, 1, log_file[FW_LOG_FILE].fp)) != 1) {
			perror("fwrite");
			return;
		}
		fflush(log_file[FW_LOG_FILE].fp);
	}
}

/*
 * This function trims any leading and trailing white spaces
 */
char *line_trim(char *str)
{
	char *ptr;

	if(*str == '\0') return str;

	/* Find the first non white-space */
	for (ptr = str; i_isspace(*ptr); ptr++);
	if (*ptr == '\0')
	    return str;

	/* This is the new start of the string*/
	str = ptr;

	/* Find the last non white-space and null terminate the string */
	ptr += strlen(ptr) - 1;
	for (; ptr != str && i_isspace(*ptr); ptr--);
	ptr[1] = '\0';

	return str;
}

void read_config_file(void) {

	FILE *fp = NULL;
	char line_string[MAX_STR_LEN + 1];
	char *line;
	char string[MAX_STR_LEN + 1];
	static int path_flag = 0;
	static int size_flag = 0;
	int archive_flag = 0;
	int pktlog_archive_flag = 0;
	int log_buff_flag = 0;
	int pktlog_log_buff_flag = 0;
	int host_log_flag = 0;
	int fw_log_flag = 0;
	int memory_threshold_flag = 0;

	int log_storage = 0;

	fp = fopen(cnssdiag_config_file, "a+");
	if (NULL != fp) {
		fseek(fp, 0, SEEK_SET);
		while (!feof(fp)) {
			fgets(line_string, sizeof(line_string), fp);
			line = line_string;
			line = line_trim(line);
			if (*line == '#')
				continue;
			else {
				sscanf(line, "%"STR(MAX_STR_LEN)"s", string);
				if (strcmp(string, "LOG_PATH_FLAG") == 0) {
					sscanf((line + strlen("LOG_PATH_FLAG")
						+ FLAG_VALUE_OFFSET),
							"%"STR(MAX_STR_LEN)"s", string);
					log_path_flag = (int)strtol(string, (char **)NULL, 10);
					path_flag = 1;
					debug_printf("file_path=%d\n", log_path_flag);
				}
				else if (strcmp(string, "MAX_LOG_FILE_SIZE") == 0) {
					sscanf((line +	strlen("MAX_LOG_FILE_SIZE") +
						FLAG_VALUE_OFFSET),
							 "%"STR(MAX_STR_LEN)"s", string);
					max_file_size = (int)strtol(string, (char **)NULL, 10);
					if ((max_file_size > 0) &&
						(max_file_size <= MAX_FILE_SIZE_FROM_USER_IN_MB)) {
						max_file_size = max_file_size * (1024) * (1024);
					} else {
						max_file_size = 0;
					}
					size_flag = 1;
					debug_printf("max_file_size=%d\n", max_file_size);
				}
				else if (strcmp(string, "MAX_ARCHIVES") == 0) {
					sscanf((line +	strlen("MAX_ARCHIVES") +
						FLAG_VALUE_OFFSET),
							 "%"STR(MAX_STR_LEN)"s", string);
					max_archives = (int)strtol(string, (char **)NULL, 10);
					if (max_archives >= 50)
						max_archives = 50;
					archive_flag = 1;
					debug_printf("max_archives=%d\n", max_archives);
				}
				else if (strcmp(string, "MAX_PKTLOG_ARCHIVES") == 0) {
					sscanf((line + strlen("MAX_PKTLOG_ARCHIVES") +
						FLAG_VALUE_OFFSET),
							 "%99s", string);
					max_pktlog_archives = (int)strtol(string, (char **)NULL, 10);
					pktlog_archive_flag = 1;
					debug_printf("max_pktlog_archives=%d\n", max_pktlog_archives);
				}
				else if (strcmp(string, "AVAILABLE_MEMORY_THRESHOLD") == 0) {
					sscanf((line +	strlen("AVAILABLE_MEMORY_THRESHOLD") +
						FLAG_VALUE_OFFSET), "%"STR(MAX_STR_LEN)"s", string);
					free_mem_threshold = (int)strtol(string, (char **)NULL, 10);
					memory_threshold_flag = 1;
					debug_printf("free_mem_threshold=%d\n", free_mem_threshold);
				} else if (strcmp(string, "LOG_STORAGE_PATH") == 0) {
					sscanf((line +	strlen("LOG_STORAGE_PATH") +
						FLAG_VALUE_OFFSET), "%"STR(MAX_STR_LEN)"s", string);
					if (strlen(string) != 0)
						strlcpy(log_capture_loc, string, sizeof(log_capture_loc));
					android_printf("log_capture_location  = %s", log_capture_loc);
					log_storage = 1;

				} else if (strcmp(string, "MAX_LOG_BUFFER") == 0) {
					sscanf((line +
						strlen("MAX_LOG_BUFFER") +
						FLAG_VALUE_OFFSET),
						"%"STR(MAX_STR_LEN)"s", string);
					max_buff_size = (int)strtol(string, (char **)NULL, 10);
					if ((max_buff_size > 0) &&
						(max_buff_size <= MAX_FILE_SIZE_FROM_USER_IN_MB)) {
						max_buff_size = max_buff_size * (1024) * (1024);
					} else {
						max_buff_size = 0;
					}
					log_buff_flag = 1;
					debug_printf("max_buff_size=%d\n",
						     max_buff_size);
				} else if (strcmp(string, "MAX_PKTLOG_BUFFER") == 0) {
					sscanf((line +
						strlen("MAX_PKTLOG_BUFFER") +
						FLAG_VALUE_OFFSET),
						"%99s", string);
					max_pktlog_buff_size = (int)strtol(string, (char **)NULL, 10);
					if ((max_pktlog_buff_size > 0) &&
						(max_pktlog_buff_size <= MAX_PACKETLOG_BUFFER_LIMIT)) {
						max_pktlog_buff_size = max_pktlog_buff_size * (1024) * (1024);
					} else {
						max_pktlog_buff_size = 0;
					}
					pktlog_log_buff_flag = 1;
					debug_printf("max_pktlog_buff_size=%d\n",
						     max_pktlog_buff_size);
				} else if (strcmp(string, "HOST_LOG_FILE") == 0) {
					sscanf((line +
						strlen("HOST_LOG_FILE") +
						FLAG_VALUE_OFFSET), "%"STR(MAX_STR_LEN)"s",
						string);
					if (strlen(string) != 0)
						strlcpy(hbuffer_log_file,
							string,
							sizeof(hbuffer_log_file));

					android_printf("Host_logs_location  = %s",
							 hbuffer_log_file);
					host_log_flag = 1;
				} else if (strcmp(string, "FIRMWARE_LOG_FILE") == 0) {
					sscanf((line +
						strlen("FIRMWARE_LOG_FILE") +
						FLAG_VALUE_OFFSET),
					       "%"STR(MAX_STR_LEN)"s", string);
					if (strlen(string) != 0)
						strlcpy(fbuffer_log_file,
							string,
							sizeof(fbuffer_log_file));

					android_printf("firmware_logs_location  = %s",
						       fbuffer_log_file);
					fw_log_flag = 1;
				} else
					continue;
				}
				if ((1 == path_flag) && (1 == size_flag) &&
				    (archive_flag == 1) && (pktlog_archive_flag == 1)
				     && (memory_threshold_flag) && log_storage &&
				    log_buff_flag && host_log_flag &&
				    fw_log_flag && pktlog_log_buff_flag) {
					break;
				}
			}
			if (!path_flag)
				fprintf(fp, "LOG_PATH_FLAG = %d\n", log_path_flag);
			if (!size_flag)
				fprintf(fp, "MAX_LOG_FILE_SIZE = %d\n", MAX_FILE_SIZE /((1024) * (1024)));
			if (!archive_flag)
				fprintf(fp, "MAX_ARCHIVES = %d\n", MAX_FILE_INDEX);
			if (!pktlog_archive_flag)
				fprintf(fp, "MAX_PKTLOG_ARCHIVES = %d\n", max_pktlog_archives);
			if (! log_storage)
				fprintf(fp, "LOG_STORAGE_PATH = %s\n", log_capture_loc);
			if (!memory_threshold_flag)
				fprintf(fp, "AVAILABLE_MEMORY_THRESHOLD = %d\n", FREE_MEMORY_THRESHOLD);
			if (!log_buff_flag)
				fprintf(fp, "MAX_LOG_BUFFER = %d\n",
					DEFAULT_BUFFER_SIZE_MAX/((1024) * (1024)));
			if (!pktlog_log_buff_flag)
				fprintf(fp, "MAX_PKTLOG_BUFFER = %d\n",
					DEFAULT_PKTLOG_BUFFER_SIZE/((1024) * (1024)));
			if (!host_log_flag)
				fprintf(fp, "HOST_LOG_FILE = %s\n",
					hbuffer_log_file);
			if (!fw_log_flag)
				fprintf(fp, "FIRMWARE_LOG_FILE = %s\n",
					fbuffer_log_file);
	}
	else {
		debug_printf("%s(%s): Configuration file not present "
				"set defualt log file path to internal "
				"sdcard\n", __func__, strerror(errno));
	}
	if (fp)
		fclose(fp);
}



void cnss_open_log_file(int max_size_reached, enum fileType type)
{
	struct stat st;
	int ret;

	if (log_path_flag == WRITE_TO_FILE_DISABLED) {
		optionflag &= ~(LOGFILE_FLAG);
		debug_printf("%s: write to file flag is disabled\n", __func__);
	}

	do {
		if (!max_size_reached)
			log_file[type].index = 0;

		if(stat(wlan_log_dir_path, &st) == 0 &&
			S_ISDIR(st.st_mode)) {
			android_printf("%s: directory %s created",
					__func__, wlan_log_dir_path);
		}
		else {
			ret = mkdir(wlan_log_dir_path, 755);
			android_printf("%s: create directory %s "
					"ret = %d errno= %d", __func__,
					wlan_log_dir_path, ret, errno);
		}
		readDir(wlan_log_dir_path, type);

		if (NULL == log_file[type].fp) {
			if (max_size_reached) {
				log_file[type].fp = fopen(log_file[type].name, "w");
			} else {
				log_file[type].fp = fopen(log_file[type].name, "a+");
				if ((log_file[type].fp != NULL) &&
						(ftell(log_file[type].fp) >=
							max_file_size) && (type != PKTLOG_FILE)) {
					if ((avail_space  < free_mem_threshold) &&
							(log_path_flag ==
							WRITE_TO_INTERNAL_SDCARD)) {
						android_printf("Device free memory is insufficient");
						break;
					}
					fflush(log_file[type].fp);
					fclose(log_file[type].fp);
					backup_file(type);
					log_file[type].fp = fopen(log_file[type].name, "w");
				} else {
					android_printf("failed to open file a+ mode or file"
						" size %ld is less than max_file_size %d\n",
						(log_file[type].fp != NULL)?
						ftell(log_file[type].fp) : 0,
						max_file_size);
				}
			}
			if (NULL == log_file[type].fp) {
				android_printf("Failed to open file %s: %d\n",
						log_file[type].name, errno);
			}
		}

		if (NULL == log_file[type].fp) {
			if (MAX_RETRY_COUNT != delayed_count) {
				debug_printf("%s: Sleep and poll again for %s "
						" sdcard\n", __func__,
						(log_path_flag == 1) ? "internal" : "external");
				sleep(DELAY_IN_S);
				delayed_count++;
			}
			else {
				delayed_count = 0;
				if (log_path_flag == WRITE_TO_EXTERNAL_SDCARD) {
					log_path_flag = WRITE_TO_INTERNAL_SDCARD;
					debug_printf("%s: External sdcard not mounted try for"
							" internal sdcard ", __func__);
					continue;
				}
				else {
					debug_printf("%s: Internal sdcard not yet mounted"
						" Disable writing logs to a file\n", __func__);
					log_path_flag = WRITE_TO_FILE_DISABLED;
					break;
				}
			}
		} else
			break;
	} while(1);
	return;
}
/*
 * Process FW debug, FW event and FW log messages
 * Read the payload and process accordingly.
 *
 */
void process_cnss_diag_msg(int cmd, tAniCLDHdr *wnl)
{
	uint8_t *dbgbuf;
	uint8_t *eventbuf = ((uint8_t *)wnl + sizeof(wnl->radio));
	uint16_t diag_type = 0;
	uint32_t event_id = 0;
	uint16_t length = 0;
	struct dbglog_slot *slot;
	uint32_t dropped = 0;

	dbgbuf = eventbuf;
	diag_type = *(uint16_t *)eventbuf;
	eventbuf += sizeof(uint16_t);

	length = *(uint16_t *)eventbuf;
	eventbuf += sizeof(uint16_t);

	if (cmd == WLAN_NL_MSG_CNSS_HOST_MSG) {
		if ((wnl->wmsg.type == ANI_NL_MSG_LOG_HOST_MSG_TYPE) ||
			(wnl->wmsg.type == ANI_NL_MSG_LOG_MGMT_MSG_TYPE)) {
			if ((optionflag & LOGFILE_FLAG) && (!doesFileExist(log_file[HOST_LOG_FILE].name))
				&& (log_path_flag == WRITE_TO_INTERNAL_SDCARD)&& log_file[HOST_LOG_FILE].fp) {
				if (fclose(log_file[HOST_LOG_FILE].fp) == EOF)
					perror("Failed to close host file ");
				log_file[HOST_LOG_FILE].index = 0;
				log_file[HOST_LOG_FILE].fp = fopen(log_file[HOST_LOG_FILE].name, "w");
				if (log_file[HOST_LOG_FILE].fp == NULL) {
					debug_printf("Failed to create a new file");
				}
			}
			process_cnss_host_message(wnl, optionflag);
		}
		else if (wnl->wmsg.type == ANI_NL_MSG_LOG_FW_MSG_TYPE) {
			if ((optionflag & LOGFILE_FLAG) && (!doesFileExist(log_file[FW_LOG_FILE].name))
				&&(log_path_flag == WRITE_TO_INTERNAL_SDCARD)&& log_file[FW_LOG_FILE].fp) {
				if (fclose(log_file[FW_LOG_FILE].fp) == EOF)
					perror("Failed to close fw file ");
				log_file[FW_LOG_FILE].index = 0;
				log_file[FW_LOG_FILE].fp = fopen(log_file[FW_LOG_FILE].name, "w");
				if(log_file[FW_LOG_FILE].fp == NULL) {
					debug_printf("Failed to create a new file");
				}
			}
			process_pronto_firmware_logs(wnl, optionflag);
		}
	} else if (cmd == WLAN_NL_MSG_CNSS_HOST_EVENT_LOG &&
		   (wnl->wmsg.type == ANI_NL_MSG_LOG_HOST_EVENT_LOG_TYPE)) {
		process_cnss_host_diag_events_log(
		    (char *)((char *)&wnl->wmsg.length +
			      sizeof(wnl->wmsg.length)),
		    optionflag);
	} else {
		if (diag_type == DIAG_TYPE_FW_EVENT) {
			eventbuf += sizeof(uint32_t);
			event_id = *(uint32_t *)eventbuf;
			eventbuf += sizeof(uint32_t);
#ifndef NO_DIAG_SUPPORT
			if (optionflag & QXDM_FLAG) {
				if (length)
					event_report_payload(event_id, length,
							     eventbuf);
				else
					event_report(event_id);
			}
#endif

		} else if (diag_type == DIAG_TYPE_FW_LOG) {
			/* Do nothing for now */
		} else if (diag_type == DIAG_TYPE_FW_DEBUG_MSG) {
			slot =(struct dbglog_slot *)dbgbuf;
			length = get_le32((uint8_t *)&slot->length);
			dropped = get_le32((uint8_t *)&slot->dropped);
			dbglog_parse_debug_logs(&slot->payload[0],
				    length, dropped, wnl->radio);
		} else if (diag_type == DIAG_TYPE_FW_MSG) {
			uint32_t version = 0;
			slot = (struct dbglog_slot *)dbgbuf;
			length = get_32((uint8_t *)&slot->length);
			version = get_le32((uint8_t *)&slot->dropped);
			if ((optionflag & LOGFILE_FLAG) && (!doesFileExist(log_file[FW_LOG_FILE].name))
				&&(log_path_flag == WRITE_TO_INTERNAL_SDCARD)&& log_file[FW_LOG_FILE].fp) {
				if (fclose(log_file[FW_LOG_FILE].fp) == EOF)
					perror("Failed to close fw file ");
				log_file[FW_LOG_FILE].index = 0;
				log_file[FW_LOG_FILE].fp = fopen(log_file[FW_LOG_FILE].name, "w");
				if(log_file[FW_LOG_FILE].fp == NULL) {
					debug_printf("Failed to create a new file");
				}
			}
			process_diagfw_msg(&slot->payload[0], length,
			    optionflag, version, sock_fd, wnl->radio);
		} else if (diag_type == DIAG_TYPE_HOST_MSG) {
			slot = (struct dbglog_slot *)dbgbuf;
			length = get_32((uint8_t *)&slot->length);
			process_diaghost_msg(slot->payload, length);
		} else {
			/* Do nothing for now */
		}
	}
}

/*
 * Open the socket and bind the socket with src
 * address. Return the socket fd if sucess.
 *
 */
static int32_t create_nl_socket()
{
	int32_t ret;
	int32_t sock_fd;

	sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_USERSOCK);
	if (sock_fd < 0) {
		fprintf(stderr, "Socket creation failed sock_fd 0x%x \n",
		        sock_fd);
		return -1;
	}

	memset(&src_addr, 0, sizeof(src_addr));
	src_addr.nl_family = AF_NETLINK;
	src_addr.nl_groups = 0x01;
	src_addr.nl_pid = getpid(); /* self pid */

	ret = bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr));
	if (ret < 0)
		{
		close(sock_fd);
		return ret;
	}
	return sock_fd;
}

static int initialize()
{
	char *mesg = "Hello";

	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.nl_family = AF_NETLINK;
	dest_addr.nl_pid = 0; /* For Linux Kernel */
	dest_addr.nl_groups = 0; /* unicast */

	if (nlh) {
		free(nlh);
		nlh = NULL;
	}
	nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_MSG_SIZE));
	if (nlh == NULL) {
		android_printf("%s Cannot allocate memory for nlh",
			__func__);
		return -1;
	}
	memset(nlh, 0, NLMSG_SPACE(MAX_MSG_SIZE));
	nlh->nlmsg_len = NLMSG_SPACE(MAX_MSG_SIZE);
	nlh->nlmsg_pid = getpid();
	nlh->nlmsg_type = WLAN_NL_MSG_CNSS_DIAG;
	nlh->nlmsg_flags = NLM_F_REQUEST;

	memcpy(NLMSG_DATA(nlh), mesg, strlen(mesg));

	iov.iov_base = (void *)nlh;
	iov.iov_len = nlh->nlmsg_len;
	msg.msg_name = (void *)&dest_addr;
	msg.msg_namelen = sizeof(dest_addr);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	return 1;
}

int init_log_file()
{
	boolean enable_log_file[LOG_FILE_MAX] = {FALSE, FALSE, FALSE};
	int i;

	if (optionflag & LOGFILE_FLAG) {
		enable_log_file[HOST_LOG_FILE] = TRUE;
		enable_log_file[FW_LOG_FILE] = TRUE;
	}
	if (max_pktlog_archives > 0)
		enable_log_file[PKTLOG_FILE] = TRUE;
	if (optionflag & LOGFILE_QXDM_FLAG) {
		enable_log_file[HOST_QXDM_LOG_FILE] = TRUE;
		enable_log_file[FW_QXDM_LOG_FILE] = TRUE;
	}
	if (log_path_flag == WRITE_TO_EXTERNAL_SDCARD) {
		snprintf(wlan_log_dir_path, sizeof(wlan_log_dir_path),
				"%s", "/mnt/media_rw/sdcard1/wlan_logs/");
	} else if (log_path_flag == WRITE_TO_INTERNAL_SDCARD) {
		snprintf(wlan_log_dir_path, sizeof(wlan_log_dir_path),
				"%s", log_capture_loc);
	}

	for (i = HOST_LOG_FILE; i < LOG_FILE_MAX; i++) {
		if ((i == BUFFER_HOST_FILE) || (i == BUFFER_FW_FILE)) {
			if (optionflag & LOG_BUFF_FLAG) {
				t_buffer *buff_ctx = NULL;
				if (i == BUFFER_HOST_FILE)
					snprintf(log_file[i].name,
						 sizeof(log_file[i].name),
						 "%s", hbuffer_log_file);
				else
					snprintf(log_file[i].name,
						 sizeof(log_file[i].name),
						 "%s", fbuffer_log_file);
				log_file[i].buf = (char*) malloc(sizeof(t_buffer));
				if (!log_file[i].buf)
					goto free_bufs;

				memset(log_file[i].buf, 0x00, (sizeof(t_buffer)));
				buff_ctx = (t_buffer *)log_file[i].buf;
				buff_ctx->start = (unsigned char *) malloc(EACH_BUF_SIZE);
				if (buff_ctx->start == NULL) {
					free(buff_ctx);
					goto free_bufs;
				}
				buff_ctx->end = buff_ctx->start;
				buff_ctx->next = buff_ctx;

				log_file[i].buf_ptr = log_file[i].buf;
				log_file[i].fp = NULL;
				if (pthread_mutex_init(&log_file[i].buff_lock,
						       NULL)) {
					android_printf("Failed to initialize buff_lock");
					goto free_bufs;
				}
			}
		} else {
			if (enable_log_file[i] == FALSE)
				continue;
			snprintf(log_file[i].name, sizeof(log_file[i].name),
				"%s%scurrent.%s", wlan_log_dir_path, log_file_name_prefix[i], log_file_name_extn[i]);
			if (!(optionflag & BUFFER_SIZE_FLAG))
				configured_buffer_size = DEFAULT_LOG_BUFFER_LIMIT;
			log_file[i].free_buf_mem = configured_buffer_size;

			log_file[i].buf = (char*) malloc(configured_buffer_size * sizeof(char));
			if (!log_file[i].buf) {
				goto free_bufs;
			}
			memset(log_file[i].buf, 0x00, (configured_buffer_size * sizeof(char)));
			log_file[i].buf_ptr = log_file[i].buf;
			cnss_open_log_file(FALSE, i);
		}
	}

	if (optionflag & LOGFILE_QXDM_FLAG) {
		struct qmdl_file_hdr file_hdr;
		file_hdr.hdr_len = sizeof(struct qmdl_file_hdr);
		file_hdr.version = 1;
		file_hdr.data_type = 0;
		file_hdr.guid_list_count = 0;
		cnss_write_buf_logs(sizeof(struct qmdl_file_hdr), (char *)&file_hdr, HOST_QXDM_LOG_FILE);
		cnss_write_buf_logs(sizeof(struct qmdl_file_hdr), (char *)&file_hdr, FW_QXDM_LOG_FILE);
	}
	return 0;

free_bufs:
	android_printf("malloc failed, free bufs allocated so far");
	for (; i >= 0; i--) {
		if (log_file[i].buf) {
			if ((i == BUFFER_HOST_FILE) || (i == BUFFER_FW_FILE)) {
				t_buffer *buff_ctx = (t_buffer *)log_file[i].buf;
				if (buff_ctx->start)
					free(buff_ctx->start);
			}
			free(log_file[i].buf);
		}
	}
	return -1;
}

static int getAvailableSpace(const char* path) {
	struct statvfs stat;
	if (statvfs(path, &stat) != 0) {
		return -1;
	}
	/* the available size is f_bsize * f_bavail , return in MBs */
	return ((stat.f_bsize * stat.f_bavail) / (1024 * 1024));
}

static int handle_response(struct resp_info *info, char *vendata, int datalen)
{
	int len = 0;
	if (info->subcmd == QCA_NL80211_VENDOR_SUBCMD_GET_WIFI_INFO) {
		struct nlattr *tb_vendor[
				QCA_WLAN_VENDOR_ATTR_WIFI_INFO_GET_MAX + 1];
		nla_parse(tb_vendor, QCA_WLAN_VENDOR_ATTR_WIFI_INFO_GET_MAX,
				(struct nlattr *)vendata, datalen, NULL);
		if (tb_vendor[QCA_WLAN_VENDOR_ATTR_WIFI_INFO_FIRMWARE_VERSION]){
			len = nla_len(tb_vendor[
			QCA_WLAN_VENDOR_ATTR_WIFI_INFO_FIRMWARE_VERSION]);
			if ( len > MAX_SIZE) {
				strlcpy(hw_version, nla_data(tb_vendor[
			      QCA_WLAN_VENDOR_ATTR_WIFI_INFO_FIRMWARE_VERSION]),
			      MAX_SIZE);
			} else {
				strlcpy(hw_version, nla_data(tb_vendor[
			      QCA_WLAN_VENDOR_ATTR_WIFI_INFO_FIRMWARE_VERSION]),
			      len);
			}
		}
	} else {
		android_printf("Unsupported response type: %d", info->subcmd);
	}
	return 0;
}

int ack_handler(struct nl_msg *msg, void *arg)
{
	int *err = (int *)arg;
	*err = 0;
	return NL_STOP;
}

int finish_handler(struct nl_msg *msg, void *arg)
{
	int *ret = (int *)arg;
	*ret = 0;
	return NL_SKIP;
}

int error_handler(struct sockaddr_nl *nla,
        struct nlmsgerr *err, void *arg)
{
	int *ret = (int *)arg;
	*ret = err->error;
	return NL_SKIP;
}

static int no_seq_check(struct nl_msg *msg, void *arg)
{
	return NL_OK;
}

static int response_handler_cnss(struct nl_msg *msg, void *arg)
{
	struct genlmsghdr *mHeader;
	struct nlattr *mAttributes[NL80211_ATTR_MAX_INTERNAL + 1];
	char *vendata = NULL;
	int datalen;
	struct resp_info *info = (struct resp_info *) arg;
	int status;

	mHeader = (struct genlmsghdr *)nlmsg_data(nlmsg_hdr(msg));
	nla_parse(mAttributes, NL80211_ATTR_MAX_INTERNAL,
			  genlmsg_attrdata(mHeader, 0),
			  genlmsg_attrlen(mHeader, 0), NULL);

	if (mAttributes[NL80211_ATTR_VENDOR_DATA]) {
		vendata =
		((char *)nla_data(mAttributes[NL80211_ATTR_VENDOR_DATA]));
		datalen = nla_len(mAttributes[NL80211_ATTR_VENDOR_DATA]);
		if (!vendata) {
			android_printf("Vendor data not found");
			return -1;
		}
		status = handle_response(info, vendata, datalen);

	} else {
		android_printf("NL80211_ATTR_VENDOR_DATA not found");
	status = -1;
	}

	return status;
}

static int send_nlmsg_cnss(struct nl_sock *cmd_sock, struct nl_msg *nlmsg,
			  struct resp_info *info)
{
	int err = 0;

	struct nl_cb *cb = nl_cb_alloc(NL_CB_DEFAULT);
	if (!cb) {
		android_printf("alloc failed ");
		goto out;
	}

	err = nl_send_auto_complete(cmd_sock, nlmsg);	/* send message */
	if (err < 0) {
		android_printf("nl_send_auto_complete failed ");
		goto out;
	}

	err = 1;

	nl_cb_set(cb, NL_CB_SEQ_CHECK, NL_CB_CUSTOM, no_seq_check, NULL);
	nl_cb_err(cb, NL_CB_CUSTOM, error_handler, &err);
	nl_cb_set(cb, NL_CB_FINISH, NL_CB_CUSTOM, finish_handler, &err);
	nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, ack_handler, &err);
	nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, response_handler_cnss, info);

	while (err > 0) {	/* wait for reply */
		int res = nl_recvmsgs(cmd_sock, cb);
		if (res) {
			android_printf("nl80211: %s->nl_recvmsgs failed: %d",
				 __FUNCTION__, res);
		}
	}
out:
	nl_cb_put(cb);
	if (nlmsg)
		nlmsg_free(nlmsg);
	return err;
}

static struct nl_msg *populate_nlmsg(struct nl_msg *nlmsg)
{
	struct nlattr *attr;
	if ((attr = nla_nest_start(nlmsg, NL80211_ATTR_VENDOR_DATA)) != NULL) {
		if (nla_put_u8(nlmsg,QCA_WLAN_VENDOR_ATTR_WIFI_INFO_FIRMWARE_VERSION, 0)) {
			android_printf("Failed to set"
			      "QCA_WLAN_VENDOR_ATTR_WIFI_INFO_FIRMWARE_VERSION");
			nla_nest_end(nlmsg, attr);
			return NULL;
		}
		nla_nest_end(nlmsg, attr);
	} else {
		return NULL;
	}
	return nlmsg;
}

static struct nl_msg *prepare_nlmsg(int process_id, int cmdid)
{
	int res;
	struct nl_msg *nlmsg = nlmsg_alloc();
	int ifindex;

	if (nlmsg == NULL) {
		android_printf("Out of memory");
		return NULL;
	}

	genlmsg_put(nlmsg, /* pid = */ 0, /* seq = */ 0,
			process_id, 0, 0, NL80211_CMD_VENDOR, 0);
	res = nla_put_u32(nlmsg, NL80211_ATTR_VENDOR_ID, OUI_QCA);
	if (res < 0) {
		android_printf("Failed to put vendor id");
		goto cleanup;
	}

	res = nla_put_u32(nlmsg, NL80211_ATTR_VENDOR_SUBCMD, cmdid);
	if (res < 0) {
		android_printf("Failed to put vendor sub command");
		goto cleanup;
	}
	ifindex = if_nametoindex("wlan0");
	if (nla_put_u32(nlmsg, NL80211_ATTR_IFINDEX, ifindex) != 0) {
		android_printf("Failed to get iface index for iface: %s", "wlan0");
		goto cleanup;
	}

	return nlmsg;

cleanup:
	if (nlmsg)
		nlmsg_free(nlmsg);
	return NULL;
}

static void cnss_diag_find_wlan_dev()
{
	int i , status;
	char buf[512];
	char *hw_name = NULL;
	size_t buf_len=MAX_SIZE;
	struct resp_info info;
	struct nl_sock *sock_fd = NULL;
	struct nl_msg *nlmsg = NULL;

	sock_fd = nl_socket_alloc();
	if (!sock_fd)
		return;
	info.subcmd = QCA_NL80211_VENDOR_SUBCMD_GET_WIFI_INFO;
	info.reply_buf = buf;
	info.reply_buf_len = buf_len;

	sock_fd->s_flags |= NL_OWN_PORT;
	sock_fd->s_local.nl_pid = getpid();
	if (nl_connect(sock_fd, NETLINK_GENERIC)) {
		android_printf("Could not connect handle \n");
		nl_socket_free(sock_fd);
		return ;
	}
	int id = genl_ctrl_resolve(sock_fd, "nl80211");
	if(id < 0)
	{
		android_printf("id is not valid");
		nl_socket_free(sock_fd);
		return ;
	}
	nlmsg = prepare_nlmsg(id, QCA_NL80211_VENDOR_SUBCMD_GET_WIFI_INFO);
	if (nlmsg) {
		if (populate_nlmsg(nlmsg) == NULL) {
			android_printf("Failed to populate nl message");
			return ;
		}
		status = send_nlmsg_cnss(sock_fd, nlmsg, &info);
		if (status != 0) {
			android_printf("Failed to send nl message");
			nl_socket_free(sock_fd);
			return ;
		}
	}

	hw_name = strstr(hw_version, "HW:");
	if (!hw_name) {
		nl_socket_free(sock_fd);
		sock_fd = NULL;
		return;
	}
	hw_name += strlen("HW:");
	gwlan_dev = CNSS_DIAG_WLAN_ROM_DEV;
	for (i = CNSS_DIAG_WLAN_ROM_DEV; i < CNSS_DIAG_WLAN_DEV_MAX; i++) {
		if (strncmp(hw_name, cnss_diag_wlan_dev_name[i], strlen(cnss_diag_wlan_dev_name[i])) == 0) {
			gwlan_dev = i;
			break;
		}
	}
	if(sock_fd){
		nl_socket_free(sock_fd);
		sock_fd = NULL;
		return;
	}
}

#ifdef CONFIG_CLD80211_LIB
/* Event handlers */
static int response_handler(struct nl_msg *msg, void *arg)
{
    struct genlmsghdr *header;
	struct nlattr *attrs[CLD80211_ATTR_MAX + 1];
	struct nlattr *tb_vendor[CLD80211_ATTR_MAX + 1];
	int result;

	UNUSED(arg);
	header = (struct genlmsghdr *)nlmsg_data(nlmsg_hdr(msg));
	result = nla_parse(attrs, CLD80211_ATTR_MAX, genlmsg_attrdata(header, 0),
		    genlmsg_attrlen(header, 0), NULL);

	if (!result && attrs[CLD80211_ATTR_VENDOR_DATA]) {
		nla_parse(tb_vendor, CLD80211_ATTR_MAX,
				(struct nlattr *)nla_data(attrs[CLD80211_ATTR_VENDOR_DATA]),
				nla_len(attrs[CLD80211_ATTR_VENDOR_DATA]), NULL);

		if (tb_vendor[CLD80211_ATTR_DATA]) {
			android_printf("CLD80211_ATTR_DATA found len: %d",
			               nla_len(tb_vendor[CLD80211_ATTR_DATA]));
		} else {
			android_printf("%s: CLD80211_ATTR_DATA not found", __FUNCTION__);
		}
	} else
		android_printf("No valid data received");

	return NL_OK;
}

static int event_handler(struct nl_msg *msg, void *arg)
{
	struct nlattr *attrs[CLD80211_ATTR_MAX + 1];
	struct genlmsghdr *header;
	struct nlattr *tb_vendor[CLD80211_ATTR_MAX + 1];
	boolean *fetch_free_mem = (boolean *)arg;

	struct  nlmsghdr *nlh = nlmsg_hdr(msg);

	header = (struct genlmsghdr *)nlmsg_data(nlh);
	if (header->cmd == WLAN_NL_MSG_CNSS_HOST_MSG ||
	    (unsigned long)nlh->nlmsg_len > sizeof(struct dbglog_slot)) {
		int result = nla_parse(attrs, CLD80211_ATTR_MAX, genlmsg_attrdata(header, 0),
				genlmsg_attrlen(header, 0), NULL);

		if (result || !attrs[CLD80211_ATTR_VENDOR_DATA]) {
			android_printf("No valid data received");
			return 0;
		}

		nla_parse(tb_vendor, CLD80211_ATTR_MAX,
			  (struct nlattr *)nla_data(attrs[CLD80211_ATTR_VENDOR_DATA]),
			  nla_len(attrs[CLD80211_ATTR_VENDOR_DATA]), NULL);

		if (tb_vendor[CLD80211_ATTR_DATA]) {
			tAniCLDHdr *clh =
			              (tAniCLDHdr *)nla_data(tb_vendor[CLD80211_ATTR_DATA]);
			if (fetch_free_mem && (optionflag & LOGFILE_FLAG)) {
				avail_space = getAvailableSpace("/sdcard");
				if (avail_space != -1)
					fetch_free_mem = FALSE;
			}
#ifndef CNSS_DIAG_PLATFORM_WIN
			//Identify driver once on receiving NL MSG from driver
			if (gwlan_dev == CNSS_DIAG_WLAN_DEV_UNDEF) {
				// Default to Rome (compaitble with legacy device logging)
				cnss_diag_find_wlan_dev();
			}
#endif
			process_cnss_diag_msg(header->cmd, clh);
		}
		else
			android_printf("%s: CLD80211_ATTR_DATA not found", __FUNCTION__);

	}

	return 0;
}


int send_nlmsg(struct cld80211_ctx *ctx, int cmd, void *data, int len)
{
	int ret;
	struct nlattr *nla_data = NULL;
	struct nl_msg *nlmsg;

	nlmsg = cld80211_msg_alloc(ctx, cmd, &nla_data, getpid());
	if (!nlmsg) {
		android_printf("Failed to alloc nlmsg: %s\n", __FUNCTION__);
		return -1;
	}

	ret = nla_put(nlmsg, CLD80211_ATTR_DATA, len, data);
	if (ret != 0) {
		android_printf("Failed to put CLD80211_ATTR_DATA err: %d\n", ret);
		goto cleanup;
	}

	nla_nest_end(nlmsg, nla_data);

	ret = cld80211_send_recv_msg(ctx, nlmsg, response_handler, NULL);
	if (ret != 0) {
		android_printf("Failed to send msg: %d\n", ret);
	}

cleanup:
	nlmsg_free(nlmsg);
	return ret;
}
#endif /* CONFIG_CLD80211_LIB */

int32_t main(int32_t argc, char *argv[])
{
	int res =0;
	int32_t c;
	boolean fetch_free_mem = TRUE;

	progname = argv[0];
	int32_t option_index = 0;
	static struct option long_options[] = {
		{"logfile", 0, NULL, 'f'},
		{"console", 0, NULL, 'c'},
		{"syslog", 0, NULL, 'S'},
		{"qxdm", 0, NULL, 'q'},
		{"qxdm_sync", 0, NULL, 'x'},
		{"qxdm_sync_log_file", 0, NULL, 'l'},
		{"silent", 0, NULL, 's'},
		{"debug", 0, NULL, 'd'},
		{"log_buff", 0, NULL, 'u'},
		{"decoded_log", 0, NULL, 'w'},
		{"buffer_size",required_argument, NULL, 'b'},
		{"config_file",required_argument, NULL, 'm'},
		{ 0, 0, 0, 0}
	};


	while (1) {
		c = getopt_long (argc, argv, "fsScqxluwdb:m:", long_options,
				 &option_index);
		if (c == -1) break;

		switch (c) {
		case 'f':
			optionflag |= LOGFILE_FLAG;
			break;
		case 'b':
			optionflag |= BUFFER_SIZE_FLAG;
			if (optarg != NULL) {
				configured_buffer_size =
					((uint32_t)strtol(optarg, (char **)NULL, 10)) * 1024;
			}
			break;
		case 'c':
			optionflag |= CONSOLE_FLAG;
			break;
#ifdef CNSS_DIAG_PLATFORM_WIN
		case 'S':
			optionflag |= SYSLOG_FLAG;
			break;
#endif
		case 'q':
			optionflag |= QXDM_FLAG;
			break;
		case 'x':
			optionflag |= QXDM_SYNC_FLAG;
			break;
		case 'l':
			optionflag |= LOGFILE_QXDM_FLAG;
			break;
		case 's':
			optionflag |= SILENT_FLAG;
			break;

		case 'd':
			optionflag |= DEBUG_FLAG;
			break;
		case 'u':
			optionflag |= LOG_BUFF_FLAG;
			break;
		case 'w':
			optionflag |= DECODED_LOG_FLAG;
			break;
		case 'm':
			if (optarg != NULL)
				cnssdiag_config_file = optarg;
			break;

		default:
			usage();
		}
	}

	if (!(optionflag & (LOGFILE_FLAG | CONSOLE_FLAG | QXDM_SYNC_FLAG |
			    QXDM_FLAG | SILENT_FLAG | DEBUG_FLAG |
			    LOGFILE_QXDM_FLAG | BUFFER_SIZE_FLAG |
			    LOG_BUFF_FLAG | DECODED_LOG_FLAG
#ifdef CNSS_DIAG_PLATFORM_WIN
			    | SYSLOG_FLAG
#endif
			    ))) {
		usage();
		return -1;
	}
#ifndef NO_DIAG_SUPPORT
	if (optionflag & QXDM_FLAG || optionflag & QXDM_SYNC_FLAG) {
		/* Intialize the fd required for diag APIs */
		if (TRUE != Diag_LSM_Init(NULL))
			{
			perror("Failed on Diag_LSM_Init\n");
			return -1;
		}
		/* Register CALLABACK for QXDM input data */
		DIAGPKT_DISPATCH_TABLE_REGISTER(DIAG_SUBSYS_WLAN,
		    cnss_wlan_tbl);
	}
#endif
#ifdef CONFIG_CLD80211_LIB
	cldctx = cld80211_init();
	if (cldctx != NULL) {
		res = cld80211_add_mcast_group(cldctx, "fw_logs");
		if (res) {
			android_printf("Failed to add mcast group: fw_logs");
			cld80211_deinit(cldctx);
			return -1;
		}
		res = cld80211_add_mcast_group(cldctx, "host_logs");
		if (res) {
			android_printf("Failed to add mcast group: host_logs");
			cld80211_remove_mcast_group(cldctx, "fw_logs");
			cld80211_deinit(cldctx);
			return -1;
		}
		res = cld80211_add_mcast_group(cldctx, "diag_events");
		if (res) {
			android_printf("Failed to add mcast group:diag_events");
			cld80211_remove_mcast_group(cldctx, "host_logs");
			cld80211_remove_mcast_group(cldctx, "fw_logs");
			cld80211_deinit(cldctx);
			return -1;
		}
	} else
#endif /* CONFIG_CLD80211_LIB */
	{
		android_printf("Failed to initialize cld80211 library,"
		               " proceed with legacy procedure\n");
		sock_fd = create_nl_socket();
		if (sock_fd < 0) {
			fprintf(stderr, "Socket creation failed sock_fd 0x%x \n",
				sock_fd);
			return -1;
		}
		if (initialize() < 0)
			return -1;
	}
	read_config_file();
	max_no_buffer = max_buff_size/EACH_BUF_SIZE;
	if (optionflag & LOGFILE_FLAG || optionflag & LOGFILE_QXDM_FLAG ||
	    optionflag & LOG_BUFF_FLAG) {
		avail_space = getAvailableSpace("/sdcard");
		if (avail_space != -1)
			fetch_free_mem = FALSE;
		if (init_log_file())
			goto end;
	}
	signal(SIGINT, stop);
	signal(SIGTERM, stop);
	if (optionflag & LOG_BUFF_FLAG)
		signal(SIGUSR1, update_buff_to_file);
	else
		signal(SIGUSR1, default_handler_sigusr1);

	parser_init();

#ifdef CONFIG_CLD80211_LIB
	if (cldctx) {
		cld80211_recv(cldctx, -1, true, &event_handler, &fetch_free_mem);
	} else
#endif
	{
	while ( 1 )  {
		if ((res = recvmsg(sock_fd, &msg, 0)) < 0)
			continue;
		if ((res >= (int)sizeof(struct dbglog_slot)) ||
		    (nlh->nlmsg_type == WLAN_NL_MSG_CNSS_HOST_EVENT_LOG)) {
			if (fetch_free_mem && (optionflag & LOGFILE_FLAG)) {
				avail_space = getAvailableSpace("/sdcard");
				if (avail_space != -1)
					fetch_free_mem = FALSE;
			}
#ifndef CNSS_DIAG_PLATFORM_WIN
			//Identify driver once on receiving NL MSG from driver
			if (gwlan_dev == CNSS_DIAG_WLAN_DEV_UNDEF) {
				// Default to Rome (compaitble with legacy device logging)
				cnss_diag_find_wlan_dev();
			}
#endif
			process_cnss_diag_msg(nlh->nlmsg_type, (tAniCLDHdr *)(nlh+1));
			memset(nlh,0,NLMSG_SPACE(MAX_MSG_SIZE));
		} else {
			/* Ignore other messages that might be broadcast */
			continue;
		}
	}
	}
end:
#ifndef NO_DIAG_SUPPORT
	/* Release the handle to Diag*/
	if (optionflag & QXDM_FLAG || optionflag & QXDM_SYNC_FLAG)
		Diag_LSM_DeInit();
#endif
	if (optionflag & LOG_BUFF_FLAG) {
		printf("free all buffers\n ");
		clean_all_buff(&log_file[BUFFER_HOST_FILE]);
		log_file[BUFFER_HOST_FILE].buf = NULL;
		clean_all_buff(&log_file[BUFFER_FW_FILE]);
		log_file[BUFFER_FW_FILE].buf = NULL;
	}
	if (optionflag & LOGFILE_FLAG || optionflag & LOGFILE_QXDM_FLAG)
		cleanup();
	if (pthread_mutex_destroy(&log_file[BUFFER_HOST_FILE].buff_lock))
		android_printf("Failed to destroy host buff_lock");

	if (pthread_mutex_destroy(&log_file[BUFFER_FW_FILE].buff_lock))
		android_printf("Failed to destroy firmware buff_lock");

#ifdef CONFIG_CLD80211_LIB
	if (cldctx) {
		cld80211_remove_mcast_group(cldctx, "host_logs");
		cld80211_remove_mcast_group(cldctx, "fw_logs");
		cld80211_remove_mcast_group(cldctx, "diag_events");
		cld80211_deinit(cldctx);
	} else
#endif
	{
		close(sock_fd);
		free(nlh);
	}
	return 0;
}

