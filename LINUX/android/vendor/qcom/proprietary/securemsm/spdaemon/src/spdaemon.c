/*=============================================================================
Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

/**
 * @file spdaemon.c
 * @brief - Secure Processor Daemon (spdaemon)
 *
 * This driver is responsible for loading SP Applications that
 * doesn't have an owner HLOS Application.
 */

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include <stdlib.h>    /* malloc() */
#include <stdio.h>     /* fopen() */
#include <fcntl.h>     /* O_RDONLY */
#include <unistd.h>    /* sleep() / usleep() */
#include <errno.h>     /* ENODEV */
#include <memory.h>
#include <pthread.h>
#include <time.h>

#include <cutils/log.h>        /* SLOGE() */

#include <spcomlib.h>

#include "mdm_detect.h"
#include "pm-service.h"
#include <cutils/properties.h> // property_get()

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/
#ifdef PRINT_LOG_TO_STDOUT
    #define LOGD(fmt, x...) printf("spdaemon: dbg: %s: " fmt, __func__, ##x)
    #define LOGE(fmt, x...) printf("spdaemon: err: %s: " fmt, __func__, ##x)
#else /* print to system log a.k.a logcat */
    #undef LOG_TAG
    #undef LOGD
    #undef LOGE
    #define LOG_TAG "spdaemon"
    #define LOGD SLOGD
    #define LOGE SLOGE
#endif

#define SZ_1K (1024)
#define SZ_8K (8 * 1024)
#define SZ_1M (1024 * 1024)

#define KEYMASTER_ION_BUF_SIZE (4 * SZ_1K)

#ifndef OFFSET_OF
    #define OFFSET_OF(field, base) ((int)((char*)(field) - (char*)(base)))
#endif

/** Command Opcode */
#define KEYMASTER_INIT_CMD_ID  0xAA

#define FILE_PATH_SIZE 256
/*-------------------------------------------------------------------------
 * Structures and enums
 * ----------------------------------------------------------------------*/

struct sp_app_info {
    const char *ch_name;
    const char *base_name;
    int swap_size;
};

struct keymaster_init_request {
    uint32_t cmd_opcode;
    uint64_t ion_buf_virt_addr;
} __attribute__((packed));

struct keymaster_init_response {
    uint32_t cmd_opcode;
    uint32_t error_code;
}  __attribute__((packed));

/* Note: The request and response structure format should be packed */

/*-------------------------------------------------------------------------
 * Global Variables
 * ----------------------------------------------------------------------*/

enum spss_firmware_type {
    SPSS_FW_TYPE_DEV = 'd',
    SPSS_FW_TYPE_TEST = 't',
    SPSS_FW_TYPE_PROD = 'p',
};

enum spss_soc_type {
    SOC_MSM8998 = 8998,
    SOC_SDM845  = 845,
};

static enum spss_firmware_type firmware_type = SPSS_FW_TYPE_TEST;

/* SPSS Apps - Dev */
static const struct sp_app_info keymaster_app_info = {
        .ch_name = "sp_keymaster",
        .base_name = "keym", /* FAT 8.3 chars format */
        .swap_size = 256 * SZ_1K,
};

static const struct sp_app_info cryptoapp_app_info = {
        .ch_name = "cryptoapp",
        .base_name = "crypt",
        .swap_size = 256 * SZ_1K,
};

static const struct sp_app_info macchiato_app_info = {
        .ch_name = "macchiato",
        .base_name = "macch",
        .swap_size = 256 * SZ_1K,
};

/* The spss_utils kernel driver sysfs path depends on the linux kernel version */
static const char *firmware_name_path =  "./sys/devices/platform/soc/soc:qcom,spss_utils/firmware_name";
static const char *firmware_name_path2 = "./sys/devices/soc/soc:qcom,spss_utils/firmware_name";

static int keymaster_ion_fd = -1;
static void *keymaster_ion_vbuf = NULL;
static struct spcom_client *ssr_spcom_client = NULL;

static pthread_mutex_t pthread_mutex;
static pthread_cond_t  pthread_cond;
static bool sp_reset_detected = false;

static void *pm_spss_handle = NULL;

static const char *machine_name_path = "/sys/devices/soc0/machine";
static char machine_name[20] = {}; // i.e. "MSM8998" , "SDM845"

static enum spss_soc_type soc_type = SOC_MSM8998;
static int soc_version = 0;

/*-------------------------------------------------------------------------
 * Function Implementations
 * ----------------------------------------------------------------------*/

static inline void msleep(int msec)
{
    usleep(msec * 1000);
}

static char *get_machine_name(void)
{
	int fd;
	int ret;
	char *p;

	fd = open(machine_name_path, O_RDONLY);
	if (fd < 0) {
		LOGD("open() file [%s] Failed, ret =%d.\n", machine_name_path, fd);
		return NULL;
	}

	ret = read(fd, machine_name, sizeof(machine_name));
	if (ret < 0) {
		LOGE("read() file [%s] Failed, ret =%d.\n", machine_name_path, ret);
		return NULL;
	}

	p = strstr(machine_name,"\n");
	if (p != NULL)
		*p = 0; // remove new line char

	LOGD("machine_name [%s]\n", machine_name);

	return machine_name;
}

static enum spss_soc_type get_soc_type(void)
{
	char *name;

	name = get_machine_name();
	if (name == NULL)
		return SOC_MSM8998; // default

	if (strstr(name, "8998"))
		return SOC_MSM8998;

	if (strstr(name, "845"))
		return SOC_SDM845;

	return SOC_MSM8998; // default
}

static void get_app_file_path(char *path, const char *base_name, enum spss_soc_type soc, enum spss_firmware_type fw_type, int soc_version)
{
	const char *base_path = "/firmware/image/";
	char type[2] = {0};

	type[0]= (char) fw_type;

	strlcpy(path, base_path, FILE_PATH_SIZE);
	strlcat(path, base_name, FILE_PATH_SIZE);

	if (soc == SOC_MSM8998) {
		strlcat(path, "_", FILE_PATH_SIZE);
	} else {
		if (soc_version == 1)
			strlcat(path, "1", FILE_PATH_SIZE);
		else
			strlcat(path, "2", FILE_PATH_SIZE);
	}

	strlcat(path, type, FILE_PATH_SIZE);
	strlcat(path, ".sig", FILE_PATH_SIZE);
}


static enum spss_firmware_type get_firmware_type(void)
{
    int fd;
    int ret;
    char name[10] = {}; // up to 8 chars , example "spss2t".
    const char *path = firmware_name_path;
    char soc_ver;
    char fw_type;

    fd = open(path, O_RDONLY);
    if (fd < 0) {
	LOGD("open() file [%s] Failed, ret [%d] try another path.\n", path, fd);
	path = firmware_name_path2; /* Alternative path */

	fd = open(path, O_RDONLY);
	if (fd < 0) {
		LOGD("open() file [%s] Failed, ret =%d.\n", path, fd);
		return SPSS_FW_TYPE_TEST;
	}
    }

    ret = read(fd, name, sizeof(name));
    if (ret < 0) {
        LOGE("read() file [%s] Failed, ret =%d.\n", path, ret);
    } else {
	char *p;

	p = strstr(name,"\n");
	if (p != NULL)
		*p = 0; // remove new line char
	LOGD("Firmware Name [%s].\n", name);
	if (strstr(name, "spss") == 0) {
		LOGE("Not SPSS firmware name.\n");
		return SPSS_FW_TYPE_TEST;
        }

	soc_ver = name[4];
	fw_type = name[5];

	if (soc_ver == '1' || soc_ver == '2')
		soc_version = soc_ver - '0';

	if (fw_type == 'd') {
		LOGD("Dev FW.\n");
		return SPSS_FW_TYPE_DEV;
	}
        if (fw_type == 't') {
		LOGD("Test FW.\n");
		return SPSS_FW_TYPE_TEST;
	}
	if (fw_type == 'p') {
		LOGD("Prod FW.\n");
		return SPSS_FW_TYPE_PROD;
        }
    }

    return SPSS_FW_TYPE_TEST;
}

static void suspend_me(void)
{
    pthread_mutex_lock(&pthread_mutex);
    pthread_cond_wait(&pthread_cond, &pthread_mutex);
}

static void resume_me(void)
{
    pthread_cond_signal(&pthread_cond);
    pthread_mutex_unlock(&pthread_mutex);
}

static int load_app(const struct sp_app_info *info)
{
    int ret;
    bool is_loaded = false;
    int timeout_msec = 60 * 1000;
    int time_msec = 0;
    const char *ch_name = info->ch_name;
    char file_path[FILE_PATH_SIZE]={};

    /* check if already loaded */
    is_loaded = spcom_is_app_loaded(ch_name);
    if (is_loaded) {
        LOGD("SP App [%s] already loaded.\n", ch_name);
        return 0;
    }

    LOGD("Load SP App [%s].\n", ch_name);
    get_app_file_path(file_path, info->base_name, soc_type, firmware_type, soc_version);

    /* Load the app */
    ret = spcom_load_app(info->ch_name,
            file_path,
            info->swap_size);
    if (ret < 0) {
        LOGE("Loading SP App [%s] failed. ret [%d].\n", ch_name, ret);
        return ret;
    }

    LOGD("Verify is_app_loaded() is set for app [%s].\n", ch_name);

    while (!is_loaded) {
        is_loaded = spcom_is_app_loaded(ch_name);
        msleep(10);
        time_msec += 10;
        if (time_msec >= timeout_msec) {
            LOGE("Timeout wait for char dev creation.\n");
            return -ETIMEDOUT;
        }
    }

    LOGD("SP App [%s] is loaded successfully.\n", ch_name);

    return 0;
}

/**
 * spcom_notify_ssr_cb() - a callback to notify on SP SubSystem-Reset (SSR).
 *
 * The spcom shall notify upon SP reset.
 * This callback should wait until the SP is up again (LINK is UP),
 * and then re-load the SP Applications and do any init sequence required.
 */
static void spcom_notify_ssr_cb(void)
{
    LOGD("SP subsystem reset detected.\n");

    sp_reset_detected = true;

    resume_me();
}


static int register_ssr_callback(void)
{
    int ret;
    struct spcom_client *client = NULL;
    struct spcom_client_info reg_info = {};
    const char *ch_name = "spdaemon_ssr"; /* use known node for sepolicy */

    reg_info.ch_name = ch_name;
    reg_info.notify_ssr_cb = spcom_notify_ssr_cb;

    /* register to spcom for sending request */
    client = spcom_register_client(&reg_info);
    if (client == NULL) {
        LOGE("spcom register failed.\n");
        return -EFAULT;
    }

    /* Note1: don't unregister spcom for SSR awareness */

    LOGD("SSR callback registered ok.\n");
    ssr_spcom_client = client;

    return 0;
}

static int unregister_ssr_callback(void)
{
    spcom_unregister_client(ssr_spcom_client);

    ssr_spcom_client = NULL;

    return 0;
}

/**
 *  Wait until SP is up and running.
 */
static int wait_for_sp_link_up(int timeout_sec)
{
    bool sp_is_up = false;
    int timeout_msec = timeout_sec * 1000;
    int time_msec = 0;

    while (!sp_is_up) {
        sp_is_up = spcom_is_sp_subsystem_link_up();
        msleep(10);
        time_msec += 10;
        if (time_msec >= timeout_msec) {
            LOGE("Timeout wait for SP link UP.\n");
            return -ETIMEDOUT;
        }
    }


    LOGD("SP LINK is UP in [%d] msec.\n", time_msec);

    return 0;
}

/**
 * Load SP Apps
 */
static int load_sp_apps(void)
{
    int ret;
    int link_up_timeout_sec = 60;

    // Wait for link up
    LOGD("Wait for sp link up.\n");
    ret = wait_for_sp_link_up(link_up_timeout_sec);
    if (ret < 0) {
        LOGE("wait_for_sp_link_up failed. ret [%d].\n", ret);
        return ret;
    }

    // Load cryptoapp
    ret = load_app(&cryptoapp_app_info);
    if (ret < 0) {
        LOGE("Loading SP cryptoapp App failed. ret [%d].\n", ret);
	return ret;
    }

    // Load keymaster
    ret = load_app(&keymaster_app_info);
    if (ret < 0) {
        LOGE("Loading SP keymaster App failed. ret [%d].\n", ret);
         return ret;
    }

    // Load Macchiato
    if (soc_type == SOC_SDM845) {
	    ret = load_app(&macchiato_app_info);
	    if (ret < 0) {
		LOGE("Loading SP macchiato App failed. ret [%d].\n", ret);
		 return ret;
	    }
    }

    LOGD("SP Apps were loaded successfully.\n");

    return 0;
}

static void pm_spss_event_notifier(void *client_data, enum pm_event event)
{
    char *client_name = (char*) client_data;

    LOGD("client [%s] got event [%d] notification", client_name , (int) event);

    /* Don't really care much here about events.Just ack whatever comes in. */
    pm_client_event_acknowledge(pm_spss_handle, event);
}

/*
 * PIL load sp
 */
static int pil_load_sp(void)
{
    int i, ret = 0;
    struct dev_info devinfo = {};
    enum pm_event event = 0;
    bool spss_found = false;
    void *client_data = "spdaemon";

    LOGD("Starting to get system info");
    ret = get_system_info(&devinfo);
    if (ret != RET_SUCCESS) {
        LOGE("Failed to get_system_info (%d)", ret);
        goto error;
    }

    LOGD("devinfo.num_additional_subsystems = %d",
            devinfo.num_additional_subsystems);
    for (i = 0; i < devinfo.num_additional_subsystems; i++) {
        LOGD("devinfo.subsystem_list[%d].type = %d",
                i, devinfo.subsystem_list[i].type);
        if (devinfo.subsystem_list[i].type == SUBSYS_TYPE_SPSS) {
            spss_found = true;
            LOGD("Found spss subsystem.");
            LOGD("devinfo.subsystem_list[%d].mdm_name = %s",
                    i , devinfo.subsystem_list[i].mdm_name);
            ret = pm_client_register(pm_spss_event_notifier,
                    client_data,
                    devinfo.subsystem_list[i].mdm_name, /* "spss" dev name */
                    "spdaemon", /* client name */
                    &event,
                    &pm_spss_handle);
            if (ret != PM_RET_SUCCESS) {
                LOGE("pm_client_register failed. ret [%d].\n", ret);
                goto error;
            }
            LOGD("pm-spss-thread Voting for spss subsystem");
            ret = pm_client_connect(pm_spss_handle);
            if (ret != PM_RET_SUCCESS) {
                LOGE("pm_client_connect() for spss fail. ret [%d].\n", ret);
                goto error;
            }
        }
    }

    if (!spss_found) {
        LOGE("SUBSYS_TYPE_SPSS not found.\n");
        goto error;
    }

    LOGD("SPSS-PIL completed successfully");

    return 0;
error:
    LOGD("SPSS-PIL failed.");
    return -EFAULT;
}

/**
 *  re-load SP App after SP reset
 */
static int handle_sp_reset_event(void)
{
    int ret = 0;
    
    ret = load_sp_apps();

    /* Re-register SSR callback */
    unregister_ssr_callback();
    register_ssr_callback();
    
    return ret;
}

int main(void)
{
    int ret = -1;
    char bootmode[PROPERTY_VALUE_MAX];
    static const int PIL_RETRIES = 5;

    LOGD("Version 2.3 10-Sep-2017.\n");

    firmware_type = get_firmware_type();
    LOGD("firmware_type [%c].\n", (int) firmware_type);

    soc_type = get_soc_type();
    LOGD("soc_type [%d].\n", (int) soc_type);

    // In FFBM mode - don't load spss
    property_get("ro.bootmode", bootmode, NULL);
    if (!strncmp(bootmode, "ffbm", 4)) {
        LOGD("FFBM mode, SPSS will not be loaded");
        goto load_app_error;
    }

    //start SP PIL
    for (int i=0; i<PIL_RETRIES; i++) {
        ret = pil_load_sp();
        if (ret == 0) { //success
            break;
        }
        LOGD("pil_load_sp failed. ret [%d]. retrying..\n", ret);
        sleep(1);
    }
    if (ret != 0) {
        LOGE("pil_load_sp failed. ret [%d].\n", ret);
        goto pil_error;
    }

    //load sp apps
    ret = load_sp_apps();
    if (ret < 0) {
        LOGE("Loading SP Apps failed. ret [%d].\n", ret);
        goto load_app_error;
    }

    //register SSR
    register_ssr_callback();

    while(1) {
        /* go to sleep , nothing to do now , wake up upon SP reset event */
        LOGD("Go to sleep , nothing to do now , wake up upon SP reset event.\n");
        pthread_cond_init(&pthread_cond, NULL);
        pthread_mutex_init(&pthread_mutex, NULL);

        sp_reset_detected = false;
        suspend_me();
        if (sp_reset_detected)
            handle_sp_reset_event();
    }

    return 0;

pil_error:
    LOGD("SPSS-PIL Load failure, power down the SPSS.\n");
    pm_client_disconnect(pm_spss_handle) ;
    pm_client_unregister(pm_spss_handle) ;

load_app_error:
    /* Avoid exit, since init process will start the daemon again */
    while(1) {
        LOGD("going to sleep for a day.\n");
        sleep(60*60*24); /* sleep 60 sec x 60 min x 24 hours = 1 day */
    }

    return -ENODEV; /* never happens */
}
