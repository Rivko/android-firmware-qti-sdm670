#ifndef DIAG_LSMI_H
#define DIAG_LSMI_H

/*===========================================================================

                   Diag Mapping Layer DLL , internal declarations

DESCRIPTION
  Internal declarations for Diag Service Mapping Layer.

Copyright (c)  2007-2015, 2017-2018  Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header:

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/01/08   sj      Added featurization for WM specific code & CBSP2.0
02/04/08   mad     Created File
===========================================================================*/

#include <pthread.h>

#define DIAG_INVALID_HANDLE -1
#define DIAG_MDLOG_DIR		"/sdcard/diag_logs/"
#define DIAG_MDLOG_PID_FILE	"/sdcard/diag_logs/diag_mdlog_pid"
#define DIAG_MDLOG_PID_FILE_SZ  100
#define NUM_PROC 10
#define HDLC_DISABLE	1
#define READ_BUF_SIZE 100000
#define DISK_BUF_SIZE 1024*140
#define DISK_FLUSH_THRESHOLD  1024*128

#define FILE_LIST_NAME_SIZE 100
#define MAX_FILES_IN_FILE_LIST 100
#define std_strlprintf     snprintf
#define RENAME_CMD_LEN ((2*FILE_NAME_LEN) + 10)
struct buffer_pool {
	int free;
	int data_ready;
	unsigned int bytes_in_buff[NUM_PROC];
	unsigned char *buffer_ptr[NUM_PROC];
	pthread_mutex_t write_mutex;
	pthread_cond_t write_cond;
	pthread_mutex_t read_mutex;
	pthread_cond_t read_cond;
};

enum status {
	NOT_READY,
	READY,
};
extern int rename_file_names;	/* Rename file name on close to current time */
extern int rename_dir_name;	/* Rename directory name to current time when ODL is halted */
extern int diag_fd;
extern int fd_md[NUM_PROC];
extern int gdwClientID;
void log_to_device(unsigned char *ptr, int logging_mode, int size, int type);
void send_mask_modem(unsigned char mask_buf[], int count_mask_bytes);
int diag_has_remote_device(uint16 *remote_mask);
int diag_register_socket_cb(int (*callback_ptr)(void *data_ptr, int socket_id), void *data_ptr);
int diag_set_socket_fd(int socket_id, int socket_fd);
int diag_send_socket_data(int id, unsigned char buf[], int num_bytes);
int diag_get_max_channels(void);
int diag_read_mask_file_list(char *mask_list_file);

/* === Functions dealing with diag wakelocks === */

/* Returns 1 if a wakelock is initialized for this process,
   0 otherwise. */
int diag_is_wakelock_init(void);

/* Opens the wakelock files and initializes the wakelock for
   the current process. It doesn't hold any wakelock. To hold
   a wakelock, call diag_wakelock_acquire. */
void diag_wakelock_init(char *wakelock_name);

/* Closes the wakelock files. It doesn't release the wakelock
   for the current process if held. */
void diag_wakelock_destroy(void);

/* Acquires a wakelock for the current process under the name
   given by diag_wakelock_init. */
void diag_wakelock_acquire(void);

/* Releases the wakelock held by the current process. */
void diag_wakelock_release(void);

/* To convert an integer/hexadecimal string to an integer */
int to_integer(char *str);

/* Request  the kernel diag to turn on/off the hdlc encoding of the data. */
int diag_hdlc_toggle(uint8 hdlc_support);

/* === Functions dealing with qshrink4 === */

/* Creates threads to read the qshrink4 database threads. */
int create_diag_qshrink4_db_parser_thread(unsigned int peripheral_mask);

/* Parses the data for qshrink4 command response */
int parse_data_for_qsr4_db_file_op_rsp(uint8 *ptr, int count_received_bytes);
int parse_data_for_qdss_rsp(uint8* ptr, int count_received_bytes);

/* Add qshrink4 guid information to qmdl2 header */
int add_guid_to_qshrink4_header(unsigned char * guid);

/* Notify parser thread when a PD comes up */
void diag_notify_parser_thread(int type, int peripheral_mask);

void diag_set_peripheral_mask(unsigned int peripheral_mask);
void diag_set_upd_mask(unsigned int pd_mask);
void diag_set_qdss_mask(unsigned int qdss_mask);
void diag_get_peripheral_name_from_mask(char *peripheral_name,
					unsigned int len,
					unsigned int peripheral_mask);
void diag_get_pd_name_from_mask(char *buf,
					unsigned int len,
					unsigned int pd_mask);
void get_time_string(char *buffer, int len);
void diag_kill_qshrink4_threads(void);
void diag_kill_qdss_threads(void);
void diag_kill_adpl_threads(void);
int delete_log(int type);
int diag_qdss_init();
int diag_adpl_init();
void diag_notify_qdss_thread(int peripheral_type, int peripheral_mask);
void diag_notify_adpl_thread();
int diag_send_cmds_to_disable_adpl(int in_ssr);
extern boolean gbRemote;
#define DIAG_LSM_PKT_EVENT_PREFIX "DIAG_SYNC_EVENT_PKT_"
#define DIAG_LSM_MASK_EVENT_PREFIX "DIAG_SYNC_EVENT_MASK_"
#endif /* DIAG_LSMI_H */

