/*
 * ----------------------------------------------------------------------------
 *  Copyright (c) 2012-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 *  Qualcomm Technologies Proprietary and Confidential.
 *  ---------------------------------------------------------------------------
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
//#include <linux/esoc_ctrl.h>
//#include <linux/ioctl.h>
#include <cutils/properties.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <pthread.h>
//#include <mdm_helper.h>
#include "mdm_img_transfer.h"
#define LOG_TAG "libmdmimgload"
#include <cutils/log.h>
#define LOG_NIDEBUG 0

#define MAX_TEMP_BUFFER_LENGTH 256
#define MAX_EXEC_ARGS 64
#define EXEC_STRING_LENGTH 1024
#define LINK_TYPE_HSIC "HSIC"
#define LINK_TYPE_PCIE "PCIe"

#define KS_BINARY_PATH "/system/bin/ks"
#define RAM_DUMP_IMAGE_SBL "21"
#define RAM_DUMP_IMAGE_NONE "-1"
#define HSIC_PORT_NAME "/dev/ks_hsic_bridge"
#define PCIE_PORT_NAME "/dev/mhi_pipe_2"
#define SBL_IMAGE_NAME "sbl1.mbn"
#define MAX_IMAGES 20
#define BLK_DEV_PATH "/dev/block/bootdevice/by-name/"
#define MDMDDR  "9:mdmddr"
typedef struct pcie_bhi_info_t
{
  char* pcie_bhi_device;
  char* pcie_bhi_file;
} pcie_bhi_info_t;

pcie_bhi_info_t pcie_bhi_default =
{
  .pcie_bhi_device = "/dev/bhi",
  .pcie_bhi_file = "/firmware/image/sbl1.mbn"
};

struct sahara_map {
	char mdm_name[MAX_NAME_LEN];
	char mdm_image_root_dir[PATH_MAX];
	//MAX_IMAGES * 2 since there are two entries in the arr
	//for each image
	char *mappings[MAX_IMAGES * 2];
};

struct sahara_map sahara_map_arr[] = {
	{
		.mdm_name = "MDM9x55",
		.mdm_image_root_dir = "/firmware/image/mdm9x55",
		.mappings = {
			"-s", "21:/firmware/image/mdm9x55/sbl1.mbn",
			"-s", "25:/firmware/image/mdm9x55/tz.mbn",
			"-s", "23:/firmware/image/mdm9x55/rpm.mbn",
			"-s", "30:/firmware/image/mdm9x55/sdi.mbn",
			"-s", "29:/firmware/image/mdm9x55/acdb.mbn",
			"-s", "31:/firmware/image/mdm9x55/mba.mbn",
			"-s", "8:/firmware/image/mdm9x55/qdsp6sw.mbn",
			"-s", "28:/firmware/image/mdm9x55/dsp2.mbn",
			"-s", "6:/firmware/image/mdm9x55/apps.mbn",
			"-s", "16:/dev/block/bootdevice/by-name/mdm1m9kefs1",
			"-s", "17:/dev/block/bootdevice/by-name/mdm1m9kefs2",
			"-s", "20:/dev/block/bootdevice/by-name/mdm1m9kefs3",
			"-s", "34:/dev/block/bootdevice/by-name/mdmddr",
			NULL,
		},
	},
	{
		.mdm_name = "SDX50M",
		.mdm_image_root_dir = "/firmware/image/sdx50m",
		.mappings = {
			"-s", "21:/firmware/image/sdx50m/sbl1.mbn",
			"-s", "25:/firmware/image/sdx50m/tz.mbn",
			"-s", "23:/firmware/image/sdx50m/rpm.mbn",
			"-s", "30:/firmware/image/sdx50m/sdi.mbn",
			"-s", "29:/firmware/image/sdx50m/acdb.mbn",
			"-s", "31:/firmware/image/sdx50m/mba.mbn",
			"-s", "8:/firmware/image/sdx50m/qdsp6sw.mbn",
			"-s", "28:/firmware/image/sdx50m/dsp2.mbn",
			"-s", "6:/firmware/image/sdx50m/apps.mbn",
			"-s", "16:/dev/block/bootdevice/by-name/mdm1m9kefs1",
			"-s", "17:/dev/block/bootdevice/by-name/mdm1m9kefs2",
			"-s", "20:/dev/block/bootdevice/by-name/mdm1m9kefs3",
			"-s", "34:/dev/block/bootdevice/by-name/mdmddr",
			NULL,
		},
	},

	//Default should always be the last entry in the array.
	//Any new modem structs should be added on top of this one
	{
		.mdm_name = "DEFAULT",
		.mdm_image_root_dir = "/firmware/image",
		.mappings = {
			"-s", "21:/firmware/image/sbl1.mbn",
			"-s", "25:/firmware/image/tz.mbn",
			"-s", "23:/firmware/image/rpm.mbn",
			"-s", "30:/firmware/image/sdi.mbn",
			"-s", "29:/firmware/image/acdb.mbn",
			"-s", "31:/firmware/image/mba.mbn",
			"-s", "8:/firmware/image/qdsp6sw.mbn",
			"-s", "28:/firmware/image/dsp2.mbn",
			"-s", "6:/firmware/image/apps.mbn",
			"-s", "16:/dev/block/bootdevice/by-name/mdm1m9kefs1",
			"-s", "17:/dev/block/bootdevice/by-name/mdm1m9kefs2",
			"-s", "20:/dev/block/bootdevice/by-name/mdm1m9kefs3",
			"-s", "34:/dev/block/bootdevice/by-name/mdmddr",
			NULL,
		},
	},
};

bool wait_for_com_port(char *filename)
{
  const size_t max_retries = 2000;
  const size_t delay_between_retries_ms = 20;
  struct stat status_buf;
  size_t i;

  ALOGI("Testing if file(port) \"%s\" exists", filename);
  for (i = 0; i < max_retries && stat(filename, &status_buf) < 0; i++) {
      usleep(delay_between_retries_ms * 1000);
  }
  if (i == max_retries) {
      ALOGE("'%s' was not found", filename);
      return false;
  }
  ALOGI("File(port) :%s: located", filename);
  return true;
}


static bool load_pcie_bhi(pcie_bhi_info_t *pcie_bhi_info)
{
  int retval = 0;
  struct stat stat_buf;
  char dd_of[MAX_TEMP_BUFFER_LENGTH];
  char dd_if[MAX_TEMP_BUFFER_LENGTH];
  char dd_bs[MAX_TEMP_BUFFER_LENGTH];
  pid_t process_pid;
  int process_retval = 0;

  if (false == wait_for_com_port(pcie_bhi_info->pcie_bhi_device))
    return false;

  retval = snprintf(dd_of, sizeof(dd_of), "of=%s", pcie_bhi_info->pcie_bhi_device);
  if (retval < 0 || (unsigned int) retval >= sizeof(dd_of)) {
    ALOGE("Error copying string, retval %d", retval);
    return false;
  }

  if ((retval = stat(pcie_bhi_info->pcie_bhi_file, &stat_buf)) < 0) {
    ALOGE("Failed to stat file '%s'. %s",
          pcie_bhi_info->pcie_bhi_file,
          strerror(errno));
    return false;
  }

  retval = snprintf(dd_if, sizeof(dd_if), "if=%s", pcie_bhi_info->pcie_bhi_file);
  if (retval < 0|| (unsigned int) retval >= sizeof(dd_if)) {
    ALOGE("Error copying string, retval %d", retval);
    return false;
  }

  retval = snprintf(dd_bs, sizeof(dd_bs), "bs=%lld", stat_buf.st_size);
  if (retval < 0 || (unsigned int) retval >= sizeof(dd_bs)) {
    ALOGE("Error copying string, retval %d", retval);
    return false;
  }

  ALOGE("Running 'dd %s %s %s'", dd_of, dd_if, dd_bs);

  process_pid = fork();
  process_retval = 0;

  if (process_pid < 0) {
    ALOGE("Forking new process failed");
    return false;
  }
  else if (process_pid == 0) {
    if (execlp("dd", "dd", dd_of, dd_if, dd_bs, NULL) == -1) {
      ALOGE("Spawning process using execvp failed");
      _exit(127);
    }
  }
  if (process_pid > 0) {
    wait(&process_retval);
    ALOGE("Process return value %d", process_retval);
    process_pid = 0;
    if (process_retval == 0) {
      ALOGE("PCIE BHI transfer completed successfully");
      return true;
    }
    else {
      ALOGE("PCIE BHI transfer failed");
      return false;
    }
  }
  return true;
}

static bool load_sahara(char* port_name,
                        char* ram_dump_path,
                        char* saved_file_prefix,
                        img_transfer_mode mode,
                        char* ram_dump_image,
                        char* sahara_mappings[])
{
  char* exec_args[MAX_EXEC_ARGS] = {NULL};
  char exec_string_log[EXEC_STRING_LENGTH] = {0};
  size_t exec_args_index = 0;
  size_t i = 0, written = 0;
  pid_t process_pid;
  int process_retval = 0;

  if (false == wait_for_com_port(port_name))
    return false;

  do {
    exec_args[exec_args_index++] = KS_BINARY_PATH;
    if (exec_args_index >= MAX_EXEC_ARGS) break;

    exec_args[exec_args_index++] = "-p";
    if (exec_args_index >= MAX_EXEC_ARGS) break;
    exec_args[exec_args_index++] = port_name;
    if (exec_args_index >= MAX_EXEC_ARGS) break;

    exec_args[exec_args_index++] = "-w";
    if (exec_args_index >= MAX_EXEC_ARGS) break;
    exec_args[exec_args_index++] = ram_dump_path;
    if (exec_args_index >= MAX_EXEC_ARGS) break;

    if (NULL != saved_file_prefix && '\0' != saved_file_prefix) {
      exec_args[exec_args_index++] = "-g";
      if (exec_args_index >= MAX_EXEC_ARGS) break;
      exec_args[exec_args_index++] = saved_file_prefix;
      if (exec_args_index >= MAX_EXEC_ARGS) break;
    }

    if (MODE_FORCE_RAM_DUMPS == mode) {
      exec_args[exec_args_index++] = "-m";
      if (exec_args_index >= MAX_EXEC_ARGS) break;
    }
    else if (MODE_IGNORE_RAM_DUMPS == mode) {
      exec_args[exec_args_index++] = "-i";
      if (exec_args_index >= MAX_EXEC_ARGS) break;
    }

    if (NULL != ram_dump_image) {
      exec_args[exec_args_index++] = "-r";
      if (exec_args_index >= MAX_EXEC_ARGS) break;
      exec_args[exec_args_index++] = ram_dump_image;
      if (exec_args_index >= MAX_EXEC_ARGS) break;
    }
    exec_args[exec_args_index++] = "--partition_path";
    if (exec_args_index >= MAX_EXEC_ARGS) break;
    exec_args[exec_args_index++] = BLK_DEV_PATH;
    size_t sahara_mapping_index = 0;
    while (NULL != sahara_mappings[sahara_mapping_index] &&
           exec_args_index < MAX_EXEC_ARGS) {
      exec_args[exec_args_index++] = sahara_mappings[sahara_mapping_index++];
    }
    if (exec_args_index >= MAX_EXEC_ARGS) break;
    exec_args[exec_args_index++] = "-a";
    exec_args[exec_args_index++] = MDMDDR;

  } while(0);

  if (exec_args_index >= MAX_EXEC_ARGS - 1) {
    ALOGE("No more room for execve arguments");
    return false;
  }

  exec_args[exec_args_index] = NULL;

  for (i = 0; i < exec_args_index && written < sizeof(exec_string_log); ++i)
    written += snprintf(exec_string_log + written,
                        sizeof(exec_string_log) - written,
                        "%s ",
                        exec_args[i]);

  ALOGE("Running '%s'", exec_string_log);

  process_pid = fork();
  process_retval = 0;

  if (process_pid < 0) {
    ALOGE("Forking new process failed");
    return false;
  }
  else if (process_pid == 0) {
    if (execvp(exec_args[0], exec_args) == -1) {
      ALOGE("Spawning process using execvp failed");
      _exit(127);
    }
  }
  if (process_pid > 0) {
    wait(&process_retval);
    ALOGE("Process return value %d", process_retval);
    process_pid = 0;
    if (process_retval == 0) {
        ALOGE("Sahara transfer completed successfully");
        return true;
    }
    else {
      return false;
    }
  }
  return true;
}

static char** get_sahara_mappings(char *mdm_name)
{
	struct stat st;
	int i;
	for (i = 0; strncmp(sahara_map_arr[i].mdm_name, "DEFAULT", 7); i++)
	{
		if (!strncmp(sahara_map_arr[i].mdm_name, mdm_name,
					strlen(sahara_map_arr[i].mdm_name))) {
			//Found the path where the images should be as per the
			//new naming convention. Check if the path exists.If
			//not they would be at the default path.
			if (stat(sahara_map_arr[i].mdm_image_root_dir, &st)) {
				ALOGI("Unable to find %s.Using default mapping",
						strerror(errno));
				continue;
			}
			break;
		}
	}
	ALOGI("Using sahara mapping for %s", sahara_map_arr[i].mdm_name);
	return sahara_map_arr[i].mappings;
}

static char* get_fw_image_path(char *mdm_name)
{
	struct stat st;
	int i;
	for (i = 0; strncmp(sahara_map_arr[i].mdm_name, "DEFAULT", 7); i++)
	{
		if (!strncmp(sahara_map_arr[i].mdm_name, mdm_name,
					strlen(sahara_map_arr[i].mdm_name))) {
			//Found the path where the images should be as per the
			//new naming convention. Check if the path exists.If
			//not they would be at the default path.
			if (stat(sahara_map_arr[i].mdm_image_root_dir, &st)) {
				ALOGI("Unable to find %s.Using default mapping",
						strerror(errno));
				continue;
			}
			break;
		}
	}
	ALOGI("Returning image path %s for %s",
			sahara_map_arr[i].mdm_image_root_dir,
			sahara_map_arr[i].mdm_name);
	return sahara_map_arr[i].mdm_image_root_dir;
}

bool mdm_img_transfer(char* mdm_name,
                      char* mdm_link,
		      char* mdm_link_extra_info,
                      char* ram_dump_path,
                      char* saved_file_prefix,
                      img_transfer_mode mode)
{
  char bhi_device[PATH_MAX] = {0};
  char pcie_port_path[PATH_MAX] = {0};
  char *fw_image_path = NULL;
  pcie_bhi_info_t pcie_info;
  char **sahara_mappings;
  char sbl_path[PATH_MAX] = {0};

  if (!mdm_name || !mdm_link) {
	  ALOGE("Invalid arguments passed to %s", __func__);
	  return false;
  }
  ALOGI("MDM name '%s', Link type '%s'", mdm_name, mdm_link);
  sahara_mappings = get_sahara_mappings(mdm_name);

  if (!strncmp(mdm_link, LINK_TYPE_HSIC, strlen(LINK_TYPE_HSIC))) {
      return load_sahara(HSIC_PORT_NAME,
                         ram_dump_path,
                         saved_file_prefix,
                         mode,
                         RAM_DUMP_IMAGE_SBL,
                         sahara_mappings);
  }
  else if (!strncmp(mdm_link, LINK_TYPE_PCIE, strlen(LINK_TYPE_PCIE))) {
      if (mdm_link_extra_info) {
	      snprintf(bhi_device,
			      sizeof(bhi_device) - 1,
			      "/dev/bhi_%s",
			      mdm_link_extra_info);
	      snprintf(pcie_port_path,
			      sizeof(pcie_port_path) - 1,
			      "/dev/mhi_%s%s",
			      mdm_link_extra_info,
			      MHI_PIPE_SAHARA);
      } else {
	      snprintf(bhi_device,
			      sizeof(bhi_device) - 1,
			      "%s", pcie_bhi_default.pcie_bhi_device);
	      snprintf(pcie_port_path,
			      sizeof(pcie_port_path) - 1,
			      "%s",
			      PCIE_PORT_NAME);
      }
      fw_image_path = get_fw_image_path(mdm_name);
      snprintf(sbl_path, sizeof(sbl_path) - 1,
		      "%s/%s",
		      fw_image_path,
		      SBL_IMAGE_NAME);

      pcie_info.pcie_bhi_device = bhi_device;
      pcie_info.pcie_bhi_file = sbl_path;
      if (false == load_pcie_bhi(&pcie_info))
        return false;
      return load_sahara(pcie_port_path,
                         ram_dump_path,
                         saved_file_prefix,
                         mode,
                         RAM_DUMP_IMAGE_NONE,
                         sahara_mappings);
  }
  else {
    ALOGE("Invalid MDM and link type");
  }

  return false;
}

