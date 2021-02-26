/*
 * ----------------------------------------------------------------------------
 *  Copyright (c) 2012-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 *  Qualcomm Technologies Proprietary and Confidential.
 *  ---------------------------------------------------------------------------
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <linux/esoc_ctrl.h>
#include <linux/ioctl.h>
#include <cutils/properties.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <pthread.h>
#include <mdm_helper.h>
#include <cutils/android_reboot.h>


#define NUM_ESOC_THREADS 2
#define MDM9K_IMAGE_PATH "/firmware/image/"
#define ESOC_CMD_MASK_FILE "/sys/bus/esoc/drivers/mdm-4x/command_mask"
#define ESOC_NOTIFIER_MASK_FILE "/sys/bus/esoc/drivers/mdm-4x/notifier_mask"
#define PON_MASK_CMD "PON"
#define PANIC_NOTIFIER_MASK "PANIC"
#define NUM_COMPORT_RETRIES 50
#define MAX_PATH_LEN 255
#define HSIC_NOTIFICATION_NODE "/sys/devices/msm_hsic_host/host_ready"
#define MAX_HSIC_NOTIFICATION_RETRIES 50
#define MAX_BHI_NODE_CHECKS 100
#define BHI_NODE_PATH "/dev/bhi"
#define MODE_BOOT 1
#define MODE_RUNTIME 2
#define MODE_RAMDUMP 3
#define TEMP_FILE_DIR "/data/misc/mdmhelperdata/"
#define EFS_FILE_DIR "/dev/block/platform/msm_sdcc.1/by-name/"
#define SBL_LOGGER "/system/bin/qcat_unbuffered"
#define PCIE_DEBUGFS_NODE "/sys/kernel/debug/pci-msm/ep_wakeirq"
#define PCIE_DEBUG_ENABLE_VAL "0"
#define KS_PATH "/system/bin/ks"
#define RAM_DUMP_IMAGE 21
#define NUM_EFS_PARTITIONS 3
#define NUM_OTHER_HEADER_PREPEND_FILES 1
#define NUM_LINK_RETRIES 50
#define DELAY_BETWEEN_RETRIES_MS 500
#define NUM_COM_PORT_RETRIES 50
#define MAX_STATUS_UP_CHECKS 60
#define BOOT_TYPE_NODE "/proc/sys/kernel/cold_boot"
//SBL log exe location index
#define ARG_SBL_LOGGER_BINARY 0
//SBL log dev node path
#define ARG_INFILE 1
#define INFILE_STRING "--infile"
#define SBL_LOG_INFILE_PROP "persist.sbl_log_infile"
#define DEFAULT_INFILE "/dev/mhi_pipe_24"

//SBL log output location
#define ARG_OUTFILE 3
#define OUTFILE_LOGCAT_STRING "--logcat_tag"
#define OUTFILE_STRING "--outfile"
#define SBL_LOG_OUTFILE_PROP "persist.sbl_log_path"
#define DEFAULT_LOG_TAG "MDM_SBL_LOG"

//Time to sleep between each failed attempt to open the input file
#define ARG_RETRY_INTERVAL 5
#define RETRY_INTERVAL_STRING "--open_sleep"
#define SBL_LOG_RETRY_INTERVAL_PROP "persist.sbl_log_retry_interval"
#define DEFAULT_RETRY_INTERVAL_MS "1000"

//Number of retries: Number of times to attempt to open the input file
#define ARG_NUM_RETRIES 7
#define RETRY_STRING "--open_retry"
#define SBL_LOG_NUM_RETRIES_PROP "persist.sbl_log_num_retries"
#define DEFAULT_SBL_LOG_NUM_RETRIES "60"

//Read timeout: Number of milliseconds for no new data to show up before
//exiting
#define ARG_READ_TIMEOUT 9
#define READ_TIMEOUT_STRING "--read_timeout"
#define SBL_LOG_READ_TIMEOUT "persist.sbl_log_read_timeout"
#define DEFAULT_SBL_LOG_READ_TIMEOUT "2000"

#define ARG_SBL_LOG_FINAL 11
#define MAX_SBL_LOG_ARGS 15
typedef __u32 u32;

typedef enum Mdm9kEsocEvents {
	ESOC_EVENT_REQ_ENG_RDY = 0,
	ESOC_EVENT_REQ_XFER_IMG
} Mdm9kEsocEvents;

struct image_id_mapping {
	int image_id;
	char* filename;
};

struct headers_to_prepend {
	struct image_id_mapping file_details;
	char* header_file;
	char* binary_file;
};
struct partitions_to_file_dump {
	struct headers_to_prepend header_info;
	char *partition;
	size_t kb;
};

struct mdm_private_data {
	int reset_device_before_ramdumps;
	int (*peripheral_cmd)(struct mdm_device *dev, int nCmd);
	char* flashless_boot_device;
	char* efs_sync_device;
	char* transport_bind_node;
	char *transport_unbind_node;
	char *transport_bind_command;
	char *transport_unbind_command;
	// Prefix added to files received as RAM dumps, this
	// is used when launching kickstart for flashless boot
	char* ram_dump_file_prefix;
	// Prefix added to files recveived during an EFS sync. This
	// is used when launching kickstart for EFS sync
	char* efs_sync_file_prefix;
	char* efs_file_dir;
	pid_t efs_ks_pid;
	struct partitions_to_file_dump partition_list[NUM_EFS_PARTITIONS];
	struct headers_to_prepend other_prepend_images \
		[NUM_OTHER_HEADER_PREPEND_FILES];
	struct image_id_mapping image_list[];
};

enum {
	PERIPHERAL_CMD_BIND = 1,
	PERIPHERAL_CMD_UNBIND,
};

struct mdm_private_data private_data_9x25_hsic;
struct mdm_private_data private_data_9x25_hsic_emmc_ufs;
struct mdm_private_data private_data_9x45;

int mdm9k_powerup(struct mdm_device *dev);
int mdm9k_shutdown(struct mdm_device *dev);
int mdm9k_monitor(struct mdm_device *);
int mdm9k_ramdump_collect(struct mdm_device *);
int mdm9k_cleanup(struct mdm_device *);
int LoadSahara(struct mdm_device *dev, char* options);
int WaitForCOMport(char *DevNode, int max_retries, int attempt_read);
void peripheral_reset(struct mdm_device *dev);
