/*
 * ----------------------------------------------------------------------------
 *  Copyright (c) 2012-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 *  Qualcomm Technologies Proprietary and Confidential.
 *  ---------------------------------------------------------------------------
 */

#include "mdm9k_esoc.h"
#include "mdm_img_transfer.h"

static int configure_flashless_boot_dev(struct mdm_device *dev, int mode);
static int mdm_hsic_peripheral_cmd(struct mdm_device *dev, int cmd);
static void* override_thread(void *arg);
static int global_power_fd = -1;
#define MDM_BOOT_TIMEOUT_DISABLE_PROP "persist.mdm_boot_timeout_disable"
#define MDM_BOOT_TIMEOUT_SEC 60
#define DEBUG_MODE_FLAG "/persist/debug_mode"
#define MANUAL_MODE_FLAG "/persist/manual_mode"
#define TIMEOUT_DISABLE_FLAG "/persist/timeout_disable"
#define VOLD_DECRYPT_PROP "vold.decrypt"
#define RESTART_FRAMEWORK_VAL "trigger_restart_framework"
#define RESTART_FRAMEWORK_MIN_VAL "trigger_restart_framework_min"

struct mdm_private_data private_data_9x25_hsic = {
	.reset_device_before_ramdumps = 1,
	.peripheral_cmd = mdm_hsic_peripheral_cmd,
	.flashless_boot_device = "/dev/ks_hsic_bridge",
	.efs_sync_device = "/dev/efs_hsic_bridge",
	.transport_bind_node = "/sys/bus/platform/drivers/xhci_msm_hsic/bind",
	.transport_unbind_node = "/sys/bus/platform/drivers/xhci_msm_hsic/unbind",
	.transport_bind_command = "msm_hsic_host",
	.transport_unbind_command = "msm_hsic_host",
	.ram_dump_file_prefix = NULL,
	.efs_sync_file_prefix = "mdm1",
	.efs_ks_pid = 0,
	.efs_file_dir = "/dev/block/bootdevice/by-name/",
	.partition_list = {
		{
			{
				{16, TEMP_FILE_DIR "m9k0efs1.tmp"},
				"efs1.mbn",
				TEMP_FILE_DIR "m9k0efs1bin.tmp"
			},
			NULL,
			0,
		},
		{
			{
				{17, TEMP_FILE_DIR "m9k0efs2.tmp"},
				"efs2.mbn",
				TEMP_FILE_DIR "m9k0efs2bin.tmp"
			},
			NULL,
			0,
		},
		{
			{
				{20, TEMP_FILE_DIR "m9k0efs3.tmp"},
				"efs3.mbn",
				TEMP_FILE_DIR "m9k0efs3bin.tmp"
			},
			NULL,
			0,
		},
	},
	.other_prepend_images = {
		{
			{29, TEMP_FILE_DIR "m9k0acdb.tmp"},
			NULL,
			NULL,
		},
	},
	.image_list = {
		{30, "sdi.mbn"},
		{8, "qdsp6sw.mbn"},
		{28, "dsp2.mbn"},
		{0, NULL},
	},
};
static int manual_mode_enabled;
static int debug_mode_enabled;
static int timeout_disable;

struct mdm_private_data private_data_9x45 = {
	.reset_device_before_ramdumps = 1,
	.peripheral_cmd = mdm_hsic_peripheral_cmd,
	.flashless_boot_device = "/dev/bhi",
	.efs_sync_device = "/dev/mhi_pipe_10",
	.transport_bind_node = "/sys/bus/platform/drivers/xhci_msm_hsic/bind",
	.transport_unbind_node = "/sys/bus/platform/drivers/xhci_msm_hsic/unbind",
	.transport_bind_command = "N/A",
	.transport_unbind_command = "N/A",
	.ram_dump_file_prefix = NULL,
	.efs_sync_file_prefix = "mdm1",
	.efs_ks_pid = 0,
	.efs_file_dir = "/dev/block/bootdevice/by-name/",
	.partition_list = {
		{
			{
				{16, TEMP_FILE_DIR "m9k0efs1.tmp"},
				"efs1.mbn",
				TEMP_FILE_DIR "m9k0efs1bin.tmp"
			},
			NULL,
			0,
		},
		{
			{
				{17, TEMP_FILE_DIR "m9k0efs2.tmp"},
				"efs2.mbn",
				TEMP_FILE_DIR "m9k0efs2bin.tmp"
			},
			NULL,
			0,
		},
		{
			{
				{20, TEMP_FILE_DIR "m9k0efs3.tmp"},
				"efs3.mbn",
				TEMP_FILE_DIR "m9k0efs3bin.tmp"
			},
			NULL,
			0,
		},
	},
	.other_prepend_images = {
		{
			{29, TEMP_FILE_DIR "m9k0acdb.tmp"},
			NULL,
			NULL,
		},
	},
	.image_list = {
		{30, "sdi.mbn"},
		{8, "qdsp6sw.mbn"},
		{28, "dsp2.mbn"},
		{0, NULL},
	},
};

static int wait_for_mdm_boot(struct mdm_device *dev, int verbose)
{
	int status;
	int log_count = 0;
	int retry_count = 0;

	if (timeout_disable || manual_mode_enabled || debug_mode_enabled) {
		ALOGI("%s: MDM_BOOT_TIMEOUT is disabled",
				dev->mdm_name);
	}
	do {
		if (!timeout_disable && retry_count >= MDM_BOOT_TIMEOUT_SEC) {
			ALOGE("%s: Failed to boot within timeout",
					dev->mdm_name);
			return RET_FAILED;
		}
		if(ioctl(dev->device_descriptor,
					ESOC_GET_STATUS,
					&status) < 0) {
			ALOGE("%s: ESOC_GET_STATUS ioctl failed",
					dev->mdm_name);
			goto error;
		}
		if(status == 1) {
			ALOGI("%s: MDM2AP_STATUS is now high", dev->mdm_name);
			return RET_SUCCESS;
		}
		if (verbose || log_count++ == 0)
			ALOGI("%s: Waiting for mdm boot", dev->mdm_name);
		usleep(1000000);
		retry_count++;
	} while(1);
error:
	return RET_FAILED;
}

//After this function completes the mdm-driver will
//no longer allow additional powerups to happen and
//will stop monitoring error fatal/status lines.
static int setup_mdm_manual_mode(struct mdm_device *dev)
{
	int cmd_mask_fd = -1;
	int notifier_mask_fd = -1;
	int pcie_debugfs_fd = -1;
	int rcode = 0;

	cmd_mask_fd = open(ESOC_CMD_MASK_FILE, O_WRONLY);
	if (cmd_mask_fd < 0) {
		ALOGE("%s: Manaual mode open fail: %s",
				dev->mdm_name,
				strerror(errno));
		goto error;
	}
	rcode = write(cmd_mask_fd,
			PON_MASK_CMD,
			sizeof(PON_MASK_CMD));
	if (rcode < 0) {
		ALOGE("%s: Manual mode(cmd_mask) set fail: %s",
				dev->mdm_name,
				strerror(errno));
		goto error;
	}
	close(cmd_mask_fd);
	cmd_mask_fd = -1;
	notifier_mask_fd = open(ESOC_NOTIFIER_MASK_FILE, O_WRONLY);
	if (notifier_mask_fd < 0) {
		ALOGE("%s: Manual mode(notifier_mask) open fail : %s",
				dev->mdm_name,
				strerror(errno));
		goto error;
	}
	rcode = write(notifier_mask_fd,
			PANIC_NOTIFIER_MASK,
			sizeof(PANIC_NOTIFIER_MASK));
	if (rcode < 0) {
		ALOGE("%s: Manual mode(cmd_mask) set fail: %s",
				dev->mdm_name,
				strerror(errno));
		goto error;
	}
	close(notifier_mask_fd);
	pcie_debugfs_fd = open(PCIE_DEBUGFS_NODE, O_WRONLY);
	if (pcie_debugfs_fd < 0) {
		ALOGW("Failed to open PCIe debugfs(ep_wakeirq) node: %s",
				strerror(errno));
		goto error;
	}
	rcode = write(pcie_debugfs_fd, PCIE_DEBUG_ENABLE_VAL,
			sizeof(PCIE_DEBUG_ENABLE_VAL));
	if (rcode < 0) {
		ALOGE("Manual mode(ep_wakeirq) node write failed: %s\n",
				strerror(errno));
		goto error;
	}
	close(pcie_debugfs_fd);
	ALOGI("%s : Manual mode setup successful",
			dev->mdm_name);
	return RET_SUCCESS;
error:
	if (cmd_mask_fd >= 0)
		close(cmd_mask_fd);
	if (notifier_mask_fd >= 0)
		close(notifier_mask_fd);
	if (pcie_debugfs_fd >= 0)
		close(pcie_debugfs_fd);
	return RET_FAILED;
}

//Send a notification to the mdm-driver
static int send_esoc_notification(struct mdm_device *dev, int cmd)
{
	if (!dev) {
		ALOGE("NULL device descriptor passed to send_esoc_notification");
		goto error;
	}
	if (ioctl(dev->device_descriptor, ESOC_NOTIFY, &cmd) < 0) {
		ALOGE("%s: Failed to send notification",
				dev->mdm_name);
		goto error;
	}
	return RET_SUCCESS;
error:
	return RET_FAILED;
}

static int setup_sbl_logger(struct mdm_device *dev)
{
	char prop_buffer[PROPERTY_VALUE_MAX] = {0};
	//File from where SBL logs are to be read.
	char infile[PROPERTY_VALUE_MAX + 9] = {0};
	//Location to where log is outputted.
	char outfile[PROPERTY_VALUE_MAX + 9] = {0};
	//How long to sleep between node retries
	char retry_interval[PROPERTY_VALUE_MAX + 9] = {0};
	//Number of retries
	char retry_count[PROPERTY_VALUE_MAX + 9] = {0};
	//Read timeout
	char read_timeout[PROPERTY_VALUE_MAX + 9] = {0};
	char *sbl_logger_args[MAX_SBL_LOG_ARGS];
	char infile_buf[PATH_MAX] = {0};
	struct stat st;
	int pid = -1;

	if (!dev) {
		ALOGE("Invalid dev struct passed to setup_sbl_logger");
		goto error;
	}

	if (stat(SBL_LOGGER, &st) < 0) {
		ALOGE("%s: Failed to locate sbl logger : %s",
				dev->mdm_name,
				strerror(errno));
		goto error;
	}
	sbl_logger_args[ARG_SBL_LOGGER_BINARY] = SBL_LOGGER;
	//Infile
	sbl_logger_args[ARG_INFILE] = INFILE_STRING;
	property_get(SBL_LOG_INFILE_PROP, prop_buffer, "N/A");
	if (!strncmp(prop_buffer, "N/A", 3)) {
		if (!strncmp(dev->mdm_link_extra_data, "N/A", 3))
			snprintf(infile_buf, sizeof(infile_buf) - 1,
					"%s",
					DEFAULT_INFILE);
		else
			snprintf(infile_buf, sizeof(infile_buf) - 1,
					"/dev/mhi_%s%s",
					dev->mdm_link_extra_data,
					MHI_PIPE_SBL_LOGGER);
		ALOGI("%s: SBL log read file not defined. Using default",
				dev->mdm_name);
		snprintf(infile, sizeof(infile),
				"--infile %s",
				infile_buf);
	} else {
		ALOGI("Setting infile to %s", infile);
		snprintf(infile,
				sizeof(infile) - 1,
				"%s",
				prop_buffer);
	}
	sbl_logger_args[ARG_INFILE + 1] = infile;
	memset(prop_buffer, '\0', sizeof(prop_buffer));
	//Outfile
	property_get(SBL_LOG_OUTFILE_PROP, prop_buffer, "N/A");
	if (!strncmp(prop_buffer, "N/A", 3)) {
		//Nothing set..Use logcat as defualt
		sbl_logger_args[ARG_OUTFILE] = OUTFILE_LOGCAT_STRING;
		snprintf(outfile,
				sizeof(outfile) - 1,
				"%s",
				DEFAULT_LOG_TAG);
	} else {
		sbl_logger_args[ARG_OUTFILE] = OUTFILE_STRING;
		snprintf(outfile,
				sizeof(outfile) - 1,
				"%s",
				prop_buffer);
	}
	sbl_logger_args[ARG_OUTFILE + 1] = outfile;
	memset(prop_buffer, '\0', sizeof(prop_buffer));
	//Retry interval
	property_get(SBL_LOG_RETRY_INTERVAL_PROP, prop_buffer, "N/A");
	sbl_logger_args[ARG_RETRY_INTERVAL] = RETRY_INTERVAL_STRING;
	if (!strncmp(prop_buffer, "N/A", 3)) {
		//Use default sleep interval
		snprintf(retry_interval,
				sizeof(retry_interval) - 1,
				"%s",
				DEFAULT_RETRY_INTERVAL_MS);
	} else {
		snprintf(retry_interval,
				sizeof(retry_interval) - 1,
				"%s",
				prop_buffer);
	}
	sbl_logger_args[ARG_RETRY_INTERVAL + 1] = retry_interval;
	memset(prop_buffer, '\0', sizeof(prop_buffer));
	//Retry count
	property_get(SBL_LOG_NUM_RETRIES_PROP, prop_buffer, "N/A");
	sbl_logger_args[ARG_NUM_RETRIES] = RETRY_STRING;
	if (!strncmp(prop_buffer, "N/A", 3)) {
		//Use default retry count
		snprintf(retry_count,
				sizeof(retry_count) - 1,
				"%s",
				DEFAULT_SBL_LOG_NUM_RETRIES);
	} else {
		snprintf(retry_count,
				sizeof(retry_count) - 1,
				"%s",
				prop_buffer);
	}
	sbl_logger_args[ARG_NUM_RETRIES + 1] = retry_count;
	memset(prop_buffer, '\0', sizeof(prop_buffer));
	//Read timeout
	property_get(SBL_LOG_READ_TIMEOUT, prop_buffer, "N/A");
	sbl_logger_args[ARG_READ_TIMEOUT] = READ_TIMEOUT_STRING;
	if (!strncmp(prop_buffer, "N/A", 3)) {
		snprintf(read_timeout,
				sizeof(read_timeout) - 1,
				"%s",
				DEFAULT_SBL_LOG_READ_TIMEOUT);
	} else {
		snprintf(read_timeout,
				sizeof(read_timeout) - 1,
				"%s",
				prop_buffer);
	}
	sbl_logger_args[ARG_READ_TIMEOUT + 1] = read_timeout;
	sbl_logger_args[ARG_SBL_LOG_FINAL] = NULL;
	//Run the logger thread to collect sbl logs
	ALOGI("%s: Launching sbl logger thread",
			dev->mdm_name);
	pid = fork();
	if (pid < 0) {
		ALOGE("%s : Failed to create sbl logger thread",
				dev->mdm_name);
	} else if (pid == 0) {
		if (execve(SBL_LOGGER, sbl_logger_args, NULL) < 0) {
			ALOGE("%s: Failed to exec sbl logger", dev->mdm_name);
			_exit(127);
		}
	}
	return RET_SUCCESS;
error:
	return RET_FAILED;
}

static int do_initial_setup(struct mdm_device *dev)
{
	u32 request;
	if (!dev) {
		ALOGE("Invalid dev passed to do_initial_Setup");
		goto error;
	}
	ALOGI("%s: Initializing environment", dev->mdm_name);
	dev->device_descriptor = open(dev->mdm_port, O_RDONLY | O_NONBLOCK);
	if (dev->device_descriptor < 0) {
		ALOGE("%s: Failed to open mdm device node: %s",
				dev->mdm_name,
				strerror(errno));
		goto error;
	}
	if (manual_mode_enabled) {
		if (setup_mdm_manual_mode(dev) != RET_SUCCESS)
			goto error;
	}
	if (ioctl(dev->device_descriptor, ESOC_REG_REQ_ENG) < 0) {
		ALOGE("%s: Failed to set thread as request engine",
				dev->mdm_name);
		goto error;
	}
	if (ioctl(dev->device_descriptor,
				ESOC_WAIT_FOR_REQ, &request) < 0) {
		ALOGE("%s: REQ_ENG: ESOC_WAIT_FOR_REQ ioctl failed",
				dev->mdm_name);
		goto error;
	}
	//If in manual mode somthing else is taking care of booting the
	//modem. Once that entity is done we signal the esoc driver
	//that boot is complete(so that it can propogate that info to
	//additional APSS clients) and put the state machine
	//into monitoring mode.
	if (manual_mode_enabled) {
		ALOGI("%s: Ignoring req from req engine(Manual mode)",
				dev->mdm_name);
		if (wait_for_mdm_boot(dev, 0) == RET_SUCCESS) {
			send_esoc_notification(dev, ESOC_BOOT_DONE);
			return RET_SUCCESS;
		} else {
			goto error;
		}
	}
	if (request != ESOC_REQ_IMG) {
		ALOGE("Expecting ESOC_REQ_IMG. Recieved : %u", request);
		goto error;
	}
	return RET_SUCCESS;
error:
	return RET_FAILED;
}

static int setup_efs_sync(struct mdm_device *dev)
{
	struct mdm_private_data *pdata;
	char *efs_sync_dev = NULL;
	char buf[PATH_MAX] = {0};
	if (!dev) {
		ALOGE("Invalid device structure passed to %s",
				__func__);
		return RET_FAILED;
	}
	if (!dev->private_data) {
		ALOGE("%s: Private data not found", dev->mdm_name);
		return RET_FAILED;
	}
	pdata = (struct mdm_private_data *)dev->private_data;
	if (!strncmp(dev->mdm_link_extra_data, "N/A", 3))
		efs_sync_dev = pdata->efs_sync_device;
	else {
		snprintf(buf, sizeof(buf) - 1, "/dev/mhi_%s%s",
				dev->mdm_link_extra_data,
				MHI_PIPE_EFS_SYNC);
		efs_sync_dev = buf;
	}

	char *efs_sync_args[] = {
		KS_PATH,"-m",
		"-p", efs_sync_dev,
		"-w", pdata->efs_file_dir,
		"-t", "-1",
		"-l",
		(pdata->efs_sync_file_prefix == NULL) ? NULL : "-g",
		pdata->efs_sync_file_prefix,
		NULL };
	//Wait for efs sync port to appear
	if(efs_sync_dev) {
		if(WaitForCOMport(efs_sync_dev,
					NUM_COMPORT_RETRIES, 0) !=
				RET_SUCCESS) {
			ALOGE("%s: Could not detect EFS sync port",
				dev->mdm_name);
			goto error;
		}
	} else {
		ALOGE("%s: No efs_sync_device specified for target",
				dev->mdm_name);
		goto error;
	}
	pdata->efs_ks_pid = fork();
	if (pdata->efs_ks_pid  < 0) {
		ALOGE("%s: Failed to create efs sync process", dev->mdm_name);
		goto error;
	} else if(pdata->efs_ks_pid == 0) {
		//exec efs sync process
		if (global_power_fd > 0)
			close(global_power_fd);
		if (execve(KS_PATH, efs_sync_args, NULL) < 0) {
			ALOGE("%s: Failed to exec KS process for efs sync",
					dev->mdm_name);
			_exit(127);
		}
	}
	return RET_SUCCESS;
error:
	return RET_FAILED;
}
int mdm9k_powerup(struct mdm_device *dev)
{
	static int power_on_count = 0;
	u32 request;
	u32 cmd;
	int pid;
	struct stat st;
	int image_xfer_complete = 0;
	char debug_mode_string[PROPERTY_VALUE_MAX] = {0};
	char data_partition_state[PROPERTY_VALUE_MAX];
	if (!dev) {
		ALOGE("Invalid device structure passed to %s",
				__func__);
		return RET_FAILED;
	}
	if (!dev->private_data) {
		ALOGE("%s: Private data not found", dev->mdm_name);
		return RET_FAILED;
	}
	if (!stat(DEBUG_MODE_FLAG, &st)) {
		ALOGI("%s: debug mode enabled", dev->mdm_name);
		debug_mode_enabled = 1;
		timeout_disable = 1;
	}
	if (!stat(MANUAL_MODE_FLAG, &st)) {
		ALOGI("%s: Manual boot mode",
				dev->mdm_name);
		manual_mode_enabled = 1;
		timeout_disable = 1;
	}
	if (!stat(TIMEOUT_DISABLE_FLAG, &st)) {
		timeout_disable = 1;
	}
	if (power_on_count == 0) {
		if (do_initial_setup(dev) != RET_SUCCESS)
			goto error;
		if (manual_mode_enabled) {
			power_on_count++;
			return RET_SUCCESS;
		}
	}
	//setup flashless boot device
	if (configure_flashless_boot_dev(dev, MODE_BOOT) != RET_SUCCESS) {
		ALOGE("%s: Link setup failed", dev->mdm_name);
		goto error;
	}
	setup_sbl_logger(dev);
	if (debug_mode_enabled) {
		ALOGI("%s:mdm boot paused.set persist.mdm_boot_debug to resume",
				dev->mdm_name);
		do {
			property_get(DEBUG_MODE_PROPERTY,
					debug_mode_string,
					"false");
			usleep(1000000);
		} while(strncmp(debug_mode_string, "resume",
					6));
		property_set(DEBUG_MODE_PROPERTY, "true");
		ALOGI("%s: resuming mdm boot", dev->mdm_name);
	}
	if (!strncmp(debug_mode_string, "resume_rma", 10)) {
		ALOGI("rma boot...reinitializing boot link");
		if (configure_flashless_boot_dev(dev, MODE_BOOT) !=
				RET_SUCCESS) {
			ALOGE("%s: Link setup failed", dev->mdm_name);
			goto error;
		}
	}
	char *extra_link_data = NULL;
	if (!strncmp(dev->mdm_link_extra_data, "N/A", 3))
		extra_link_data = NULL;
	else
		extra_link_data = dev->mdm_link_extra_data;
	if (power_on_count == 0) {
		//Wait for the vold to get done with decryption of the data
		// partition
		do {
			property_get(VOLD_DECRYPT_PROP,
					data_partition_state,
					"N/A");
			if(!strncmp(data_partition_state,
						RESTART_FRAMEWORK_VAL,
						sizeof(RESTART_FRAMEWORK_VAL))||
				!strncmp(data_partition_state,
					RESTART_FRAMEWORK_MIN_VAL,
					sizeof(RESTART_FRAMEWORK_MIN_VAL))){
				ALOGE("Data partition ready");
				break;
			} else {
				usleep(1000000);
			}
		} while(1);
		if (mdm_img_transfer(
					dev->mdm_name,
					dev->mdm_link,
					extra_link_data,
					dev->ram_dump_path,
					((struct mdm_private_data *)dev->private_data)\
					->ram_dump_file_prefix,
					MODE_DEFAULT) != true) {
			ALOGE("%s: Failed to load image/collect logs",
					dev->mdm_name);
			goto error;
		}
	} else {
		if (mdm_img_transfer(
					dev->mdm_name,
					dev->mdm_link,
					extra_link_data,
					dev->ram_dump_path,
					((struct mdm_private_data *)\
					 dev->private_data)->\
					ram_dump_file_prefix,
					MODE_IGNORE_RAM_DUMPS) != true) {
			ALOGE("%s: Failed to load images", dev->mdm_name);
			goto error;
		}
	}
	image_xfer_complete = 1;
	if (send_esoc_notification(dev, ESOC_IMG_XFER_DONE) != RET_SUCCESS) {
		ALOGE("%s: Failed to send IMG_XFER_DONE notification",
				dev->mdm_name);
		goto error;
	}
	image_xfer_complete = 1;
	//Wait for MDM2AP_STATUS to go high.
	if (wait_for_mdm_boot(dev, 1) != RET_SUCCESS) {
		ALOGE("%s: Failed to boot",
				dev->mdm_name);
		goto error;
	}
	//If the link is HSIC then we need to reset the link and send
	//the driver the status up notification
	// 9x45 does not have HSIC and does not need this hack
	if (strncmp(dev->mdm_name, "MDM9x45", 7)) {
		if(!strncmp(dev->mdm_link, LINK_HSIC, 5)) {
			//Configure the hsic to enumerate as a efs sync device
			if (configure_flashless_boot_dev(dev, MODE_RUNTIME) !=
					RET_SUCCESS) {
				ALOGE("%s: Link setup failed", dev->mdm_name);
				goto error;
			}
		}
	}
	if (setup_efs_sync(dev) != RET_SUCCESS)
		goto error;
	if (send_esoc_notification(dev, ESOC_BOOT_DONE) != RET_SUCCESS) {
		ALOGE("%s: Failed to send ESOC_BOOT_DONE notification",
				dev->mdm_name);
		return RET_FAILED;
	}
	power_on_count++;
	return RET_SUCCESS;
error:
	if (manual_mode_enabled || debug_mode_enabled) {
		ALOGW("Power up function error: Looping forever");
		do {
			sleep(5000);
		} while(1);
	}
	if (!image_xfer_complete)
		cmd = ESOC_IMG_XFER_FAIL;
	else
		cmd = ESOC_BOOT_FAIL;
	if (dev) {
		if (send_esoc_notification(dev, cmd) != RET_SUCCESS)
			ALOGE("%s: Failed to send IMG_XFER_FAIL notification",
					dev->mdm_name);
	}
	return RET_FAILED;
}

/*
 * The shutdown function here is technically a post
 * shutdown cleanup. The notification will only be
 * recieved by mdm-helper once the 9k has finished
 * powering down. This function essentially exists
 * to clean up the HSIC link.
 */
int mdm9k_shutdown(struct mdm_device *dev)
{
	struct mdm_private_data *pdata = NULL;
	if (!dev) {
		ALOGE("Invalid device structure passed in");
		return RET_FAILED;
	}
	pdata = (struct mdm_private_data*)dev->private_data;
	if (!pdata) {
		ALOGE("private data is NULL");
		return RET_FAILED;
	}
	if(!strncmp(dev->mdm_link, LINK_HSIC, 5)) {
		if (pdata->peripheral_cmd) {
			ALOGI("%s: %s: Initiating HSIC unbind",
					dev->mdm_name,
					__func__);
			pdata->peripheral_cmd(dev,
					PERIPHERAL_CMD_UNBIND);
		}
	}
	mdm9k_monitor(dev);
	if (dev->required_action != MDM_REQUIRED_ACTION_NORMAL_BOOT) {
		ALOGE("%s: Invalid request recieved.Expected Image request",
				dev->mdm_name);
		return RET_FAILED;
	}
	dev->required_action = MDM_REQUIRED_ACTION_NONE;
	return RET_SUCCESS;
}

int mdm9k_monitor(struct mdm_device *dev)
{
	u32 request = 0;
	if (!dev) {
		ALOGE("%s: Invalid device structure passed to %s",
				dev->mdm_name,
				__func__);
		return RET_FAILED;
	}
	ALOGI("%s: Monitoring mdm", dev->mdm_name);
	if (ioctl(dev->device_descriptor,
				ESOC_WAIT_FOR_REQ, &request) < 0) {
		ALOGE("%s: ESOC_WAIT_FOR_REQ ioctl failed",
				dev->mdm_name);
		return RET_FAILED;
	}
	switch(request) {
	case ESOC_REQ_DEBUG:
		ALOGI("%s: Recieved request for ramdump collection",
				dev->mdm_name);
		dev->required_action = MDM_REQUIRED_ACTION_RAMDUMPS;
		break;
	case ESOC_REQ_IMG:
		ALOGI("%s: Recieved request to transfer images",
				dev->mdm_name);
		dev->required_action = MDM_REQUIRED_ACTION_NORMAL_BOOT;
		break;
	case ESOC_REQ_SHUTDOWN:
		ALOGI("%s: Recieved shutdown request",
				dev->mdm_name);
		dev->required_action = MDM_REQUIRED_ACTION_SHUTDOWN;
		break;
	default:
		ALOGE("%s: Unknown request recieved: %u", dev->mdm_name,
				request);
		break;
	}
	return RET_SUCCESS;
}

int mdm9k_ramdump_collect(struct mdm_device *dev)
{
	struct mdm_private_data *pdata = NULL;
	u32 cmd = ESOC_DEBUG_DONE;
	int dump_collection_complete = 0;
	char *extra_link_data = NULL;
	if (!dev) {
		ALOGE("%s: Invalid device structure passed to %s",
				dev->mdm_name,
				__func__);
		return RET_FAILED;
	}
	dev->required_action = MDM_REQUIRED_ACTION_RAMDUMPS;
	pdata = (struct mdm_private_data*)dev->private_data;
	if (configure_flashless_boot_dev(dev, MODE_RAMDUMP) != RET_SUCCESS) {
		ALOGE("%s: Failed to configure hsic port for collecting dumps",
				dev->mdm_name);
		goto error;
	}
	if (!strncmp(dev->mdm_link_extra_data, "N/A", 3))
		extra_link_data = NULL;
	else
		extra_link_data = dev->mdm_link_extra_data;
	if (mdm_img_transfer(
				dev->mdm_name,
				dev->mdm_link,
				extra_link_data,
				dev->ram_dump_path,
				((struct mdm_private_data *)dev->private_data)\
				->ram_dump_file_prefix,
				MODE_FORCE_RAM_DUMPS) != true) {
			ALOGE("%s: Failed to collect dumps", dev->mdm_name);
			return RET_FAILED;
	}
	if (ioctl(dev->device_descriptor, ESOC_NOTIFY,
				&cmd) < 0) {
		ALOGE("%s: :%s: Failed to send debug done notification",
				dev->mdm_name,
				__func__);
		goto error;
	}
	dump_collection_complete = 1;
	if ((mdm9k_monitor(dev) != RET_SUCCESS) &&
			dev->required_action !=
			MDM_REQUIRED_ACTION_NORMAL_BOOT) {
		ALOGE("%s: Monitor function failed/requested invalid action:%d",
				dev->mdm_name,
				dev->required_action);
		goto error;
	}
	return RET_SUCCESS;
error:
	cmd = ESOC_DEBUG_FAIL;
	if (!dump_collection_complete) {
		if (ioctl(dev->device_descriptor, ESOC_NOTIFY,
					&cmd) < 0) {
			ALOGE("%s: :%s: Failed to reset mdm",
					dev->mdm_name,
					__func__);
		}
	}
	return RET_FAILED;
}

int mdm9k_cleanup(struct mdm_device *dev)
{
	char debug_mode_string[PROPERTY_VALUE_MAX];
	if (debug_mode_enabled || manual_mode_enabled) {
		do {
			ALOGE("%s: Reached failed state with dbg mode set",
					dev->mdm_name);
			usleep(1000000000);
		} while(1);
	}
	ALOGE("%s: mdm-helper reached fail state", dev->mdm_name);
	return RET_FAILED;
}

static int configure_flashless_boot_dev(struct mdm_device *dev, int mode)
{
	struct mdm_private_data *pdata = NULL;
	int cmd = ESOC_IMG_XFER_RETRY;
	int req = 0;
	int rcode = RET_FAILED;
	int fd, i;
	struct stat buf;
	if (!dev) {
		ALOGE("Device structure passed in as NULL");
		return RET_FAILED;
	}
	pdata = (struct mdm_private_data*)dev->private_data;
	if(!pdata) {
		ALOGE("%s: %s: Private data is null",
				dev->mdm_name, __func__);
		return RET_FAILED;
	}
	if(!strncmp(dev->mdm_link, LINK_HSIC, 5)) {
		if (mode == MODE_BOOT || mode == MODE_RAMDUMP) {
			ALOGI("%s: Setting up %s boot link",
					dev->mdm_name,
					dev->mdm_link);
			for (i = 0; i < NUM_LINK_RETRIES; ++i) {
				if (pdata->peripheral_cmd) {
					ALOGI("%s: %s: Initiating HSIC unbind",
							dev->mdm_name,
							__func__);
					pdata->peripheral_cmd(dev,
							PERIPHERAL_CMD_UNBIND);
				}
				if (ioctl(dev->device_descriptor, ESOC_NOTIFY,
							&cmd) < 0) {
					ALOGE("%s: :%s: Failed to reset mdm",
							dev->mdm_name,
							__func__);
					return RET_FAILED;
				}
				if (ioctl(dev->device_descriptor,
							ESOC_WAIT_FOR_REQ,
							&req) < 0) {
					ALOGE("%s: %s:wait for image xfer fail",
							dev->mdm_name,
							__func__);
					return RET_FAILED;
				}
				if (req != ESOC_REQ_IMG &&
						(mode != MODE_RAMDUMP)) {
					ALOGE("%s: %s: Unnexpected request: %d",
							dev->mdm_name,
							__func__,
							req);
					continue;
				}
				usleep(500000);
				if (pdata->peripheral_cmd) {
					ALOGI("%s: %s: Initiating HSIC bind",
							dev->mdm_name,
							__func__);
					pdata->peripheral_cmd(dev,
							PERIPHERAL_CMD_BIND);
				}
				rcode = WaitForCOMport(
						pdata->flashless_boot_device,
						5, 0);
				if (rcode == RET_SUCCESS)
					break;
			}
			if (rcode != RET_SUCCESS) {
				ALOGE("%s: %s: Failed to setup HSIC link",
						dev->mdm_name,
						__func__);
				return RET_FAILED;
			}
		} else if (mode == MODE_RUNTIME) {
			ALOGI("%s: Setting up %s link for efs_sync",
					dev->mdm_name,
					dev->mdm_link);
			peripheral_reset(dev);
			ALOGI("%s: Sending boot status notification to HSIC",
					dev->mdm_name);
			for (i = 0; i < MAX_HSIC_NOTIFICATION_RETRIES; i++) {
				fd = open(HSIC_NOTIFICATION_NODE, O_WRONLY);
				if (fd < 0)
				{
					if (i >= MAX_HSIC_NOTIFICATION_RETRIES \
							-1) {
						ALOGE("%s: node open fail: %s",
								dev->mdm_name,
								strerror(errno)
								);
						return RET_FAILED;
					}
					usleep(100000);
				} else {
					rcode = write(fd, "1", sizeof(char));
					close(fd);
					if (rcode < 0) {
						ALOGE("%s:node write err: %s",
								dev->mdm_name,
								strerror(errno)
								);
						return RET_FAILED;
					} else {
						ALOGI("%s: Notification sent",
								dev->mdm_name);
						break;
					}
				}
			}
		}
	} else {
		if (mode == MODE_BOOT || mode == MODE_RAMDUMP) {
			//Check for bhi node
                        char bhi_path[PATH_MAX] = {0};
			if (!strncmp(dev->mdm_link_extra_data, "N/A", 3))
				snprintf(bhi_path, sizeof(bhi_path),
						"%s",
						BHI_NODE_PATH);
			else
				snprintf(bhi_path, sizeof(bhi_path),
						"/dev/bhi_%s",
						dev->mdm_link_extra_data);
			ALOGI("Searching for BHI node: :%s:", bhi_path);
			for (i = 0; i < MAX_BHI_NODE_CHECKS; i++)
			{
				if (stat(bhi_path, &buf) == 0) {
					ALOGI("Found bhi node");
					return RET_SUCCESS;
				}
				usleep(100000);
			}
			ALOGE("Failed to locate BHI node");
			return RET_FAILED;
		}
	}
	return RET_SUCCESS;
}

int WaitForCOMport(char *DevNode, int max_retries, int attempt_read)
{
	struct stat status_buf;
	int i;

	ALOGI("Testing if port \"%s\" exists", DevNode);
	for (i = 0; i < max_retries && stat(DevNode, &status_buf) < 0; i++) {
		ALOGE("Couldn't find \"%s\", %i of %i", DevNode, i+1,
				max_retries);
		usleep(DELAY_BETWEEN_RETRIES_MS * 1000);
	}
	if (i == max_retries) {
		ALOGE("'%s' was not found", DevNode);
		return RET_FAILED;
	}
	if (attempt_read) {
		FILE *fd;
		ALOGI("Attempting to open port \"%s\" for reading", DevNode);
		for (i=0; i<NUM_COM_PORT_RETRIES && (fd = fopen(DevNode,"r"))==\
				NULL; i++) {
			ALOGE("Couldn't open \"%s\", %i of %i", DevNode, i+1,
					NUM_COM_PORT_RETRIES);
			usleep(DELAY_BETWEEN_RETRIES_MS*1000);
		}
		if (i == NUM_COM_PORT_RETRIES) {
			ALOGE("'%s' could not be opened for reading", DevNode);
			return RET_FAILED;
		}
		fclose(fd);
	}
	return RET_SUCCESS;
}

void peripheral_reset(struct mdm_device *dev)
{
	struct mdm_private_data *pdata = NULL;

	if (!dev)
		return;
	pdata = (struct mdm_private_data*)dev->private_data;
	if (!pdata->peripheral_cmd)
		return;
	pdata->peripheral_cmd(dev, PERIPHERAL_CMD_UNBIND);
	usleep (10000);
	pdata->peripheral_cmd(dev, PERIPHERAL_CMD_BIND);
}

static int mdm_hsic_peripheral_cmd(struct mdm_device *dev, int nCmd)
{
	struct mdm_private_data *pdata = NULL;
	int fd = -1;
	if (!dev) {
		ALOGE("No device specified.");
		return RET_FAILED;
	}
	pdata = (struct mdm_private_data *)dev->private_data;
	if (!pdata) {
		ALOGE("No private data.");
		return RET_FAILED;
	}
	if (!pdata->transport_bind_node ||
			!pdata->transport_unbind_node ||
			!pdata->transport_bind_command ||
			!pdata->transport_unbind_command) {
		ALOGE("Bind/Unbind not supported on this target");
		return 0;
	}
	switch (nCmd) {
	case PERIPHERAL_CMD_BIND:
		fd = open(pdata->transport_bind_node, O_WRONLY);
		if (fd < 0) {
			ALOGE("Failed to open bind node : %s", strerror(errno));
			goto error;
		}
		if(write(fd, pdata->transport_bind_command,
				strlen(pdata->transport_bind_command)) < 0) {
			ALOGE("Failed to write to bind node: %s",
					strerror(errno));
			goto error;
		}
		break;
	case PERIPHERAL_CMD_UNBIND:
		fd = open(pdata->transport_unbind_node, O_WRONLY);
		if (fd < 0) {
			ALOGE("Failed to open bind node : %s", strerror(errno));
			goto error;
		}
		if(write(fd, pdata->transport_unbind_command,
				strlen(pdata->transport_unbind_command)) < 0) {
			ALOGE("Failed to write to bind node: %s",
					strerror(errno));
			goto error;
		}
		break;
	default:
		ALOGE("Unrecognised command.");
		goto error;
	}
	close(fd);
	return 0;
error:
	if (fd >= 0)
		close(fd);
	return -1;
}

static void* override_thread(void *arg)
{
	struct mdm_device *dev = (struct mdm_device*)arg;
	char powerup_node[MAX_PATH_LEN];
	int fd;
	snprintf(powerup_node, sizeof(powerup_node),
			"/dev/subsys_%s",
			dev->esoc_node);
	fd = open(powerup_node, O_RDONLY);
	if (fd < 0) {
		ALOGE("%s: Override thread failed to open esoc node: %s",
				dev->mdm_name,
				powerup_node);
	}
	do {
		sleep(50000);
	} while(1);
	return NULL;
}
