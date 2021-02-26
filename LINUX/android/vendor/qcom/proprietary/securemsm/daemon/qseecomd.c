/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        HLOS QSEECom Daemon

GENERAL DESCRIPTION
	QSEECom Daemon starts the listeners that are used for interacting
with QSEECom.

EXTERNALIZED FUNCTIONS
	None

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2012-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <dirent.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/capability.h>
#include <QSEEComAPI.h>
#ifdef ANDROID
#include <utils/Log.h>
#include <common_log.h>
#include <android/log.h>
#include <cutils/properties.h>
#endif
#include <sys/prctl.h>

#ifdef ANDROID_CHANGES

#include <private/android_filesystem_config.h>
#else
#define AID_SYSTEM 1000
#endif

/** adb log */
#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "QSEECOMD: "
#endif

#define LOG_NDDEBUG 0 //Define to enable LOGD

#ifdef LOG_NDEBUG
#undef LOG_NDEBUG
#define LOG_NDEBUG  0 //Define to enable LOGV
#endif

/* List of services id's */
#define FILE_SERVICE 10
#define TIME_SERVICE 11
#define RPMB_SERVICE 0x2000
#define SSD_SERVICE  0x3000
#define SECURE_UI_SERVICE  0x4000
#define ML_SERVICE 0x5000 //Reserving the number here to avoid future conflicts. The listener will be started from the ML daemon.
#define MDTP_SERVICE 0x6000
#define GPFILE_SERVICE 0x7000
#define TLOC_SERVICE 0x8000 //TLOC Daemon listener service ID
#define QIS_SERVICE  0x9000 // QSEE Interrupt Service ID
#define SECCAM_SERVICE 0xA000   // Secure camera listener is implemented as a stand alone daemon
#define SP_SERVICE   0xB000   // Secure Processor (SP) listener service ID
#define GPREQCANCEL_SERVICE  0x1100 // GP Request Cancellation Service ID

/* End of list */

#ifdef RPMB_LISTENER_ENABLED
#define RPMB_LISTENER_INDEX 0
#define SSD_LISTENER_INDEX 1
#define SEC_UI_LISTENER_INDEX 2
#else
#define SSD_LISTENER_INDEX 0
#define SEC_UI_LISTENER_INDEX 1
#endif

#ifdef OE
#include <syslog.h>
#define LOGI(...) syslog(LOG_NOTICE, "INFO:" __VA_ARGS__)
#define LOGV(...) syslog(LOG_NOTICE,"VERB:" __VA_ARGS__)
#define LOGD(...) syslog(LOG_DEBUG,"DBG:" __VA_ARGS__)
#define LOGE(...) syslog(LOG_ERR,"ERR:" __VA_ARGS__)
#define LOGW(...) syslog(LOG_WARNING,"WRN:" __VA_ARGS__)
#endif

enum qseecomd_hierarchy {
	QSEECOMD_PARENT_PROCESS,
	QSEECOMD_CHILD_PROCESS
};

struct qseecom_listener_services {
	char *service_name;
	int  id;
	char *file_name;
	char *file_start;
	char *file_stop;
	void *qhandle;
	char *file_init;
};

static struct qseecom_listener_services listeners[] = {

#ifdef RPMB_LISTENER_ENABLED
	 /* Change the value of RPMB_LISTENER_INDEX incase you need to change this order */
        {
		.service_name = "RPMB system services",
		.id = RPMB_SERVICE,
#ifdef OE
		.file_name = "librpmb.so.1",
#else
		.file_name = "librpmb.so",
#endif
		.file_start = "rpmb_start",
		.file_stop = "rpmb_close",
		.qhandle = NULL,
		.file_init = "rpmb_init_service",
	},
#endif
	 /* Change the value of SSD_LISTENER_INDEX incase you need to change this order */
	{
		.service_name = "SSD system services",
		.id = SSD_SERVICE,

#ifdef OE
		.file_name = "libssd.so.1",
#else
		.file_name = "libssd.so",
#endif
		.file_start = "ssd_start",
		.file_stop = "ssd_close",
		.qhandle = NULL,
		.file_init = "ssd_init_service",
	},
	 /* Change the value of SEC_UI_LISTENER_INDEX incase you need to change this order */
#ifndef OE
	{
		.service_name = "secure UI services",
		.id = SECURE_UI_SERVICE,
		.file_name = "libsecureui.so",
		.file_start = "secure_ui_start",
		.file_stop = "secure_ui_close",
		.qhandle = NULL,
		.file_init = "secure_ui_init",
	},
#endif
	{
		.service_name = "time services",
		.id = TIME_SERVICE,

#ifdef OE
        .file_name = "libdrmtime.so.1",
#else
		.file_name = "libdrmtime.so",
#endif
		.file_start = "atime_start",
		.file_stop = "atime_close",
		.qhandle = NULL,
		.file_init = "",
	},
	{
		.service_name = "file system services",
		.id = FILE_SERVICE,
#ifdef OE
		.file_name = "libdrmfs.so.1",
#else
		.file_name = "libdrmfs.so",
#endif
		.file_start = "fs_start",
		.file_stop = "fs_close",
		.qhandle = NULL,
		.file_init = "",
	},
	{
		.service_name = "gpfile system services",
		.id = GPFILE_SERVICE,
#ifdef OE
		.file_name = "libdrmfs.so.1",
#else
		.file_name = "libdrmfs.so",
#endif
		.file_start = "gpfs_start",
		.file_stop = "gpfs_close",
		.qhandle = NULL,
		.file_init = "",
	},
#ifndef OE
	{
		.service_name = "interrupt services",
		.id = QIS_SERVICE,
		.file_name = "libqisl.so",
		.file_start = "qisl_start",
		.file_stop = "", // 1. unknown prototype. 2. never called anyway.
		.qhandle = NULL,
		.file_init = "",
	},
	{
	       .service_name = "secure processor services", // SP-to-TZ interrupt
	       .id         = SP_SERVICE,
	       .file_name  = "libspl.so",
	       .file_start = "spl_start",
	       .file_stop  = "spl_stop",
	       .qhandle    = NULL,
	       .file_init  = "",
	},
	{
		.service_name = "gp request cancellation services",
		.id = GPREQCANCEL_SERVICE,
		.file_name = "libGPreqcancel.so",
		.file_start = "gp_reqcancel_start",
		.file_stop = "gp_reqcancel_close",
		.qhandle = NULL,
		.file_init = "",
	},
#endif  //!OE
};

static int qseecomd_update_cap(void)
{
	int retvalue = 0;
	struct __user_cap_header_struct header;
	struct __user_cap_data_struct cap;

	memset(&header, 0, sizeof(header));
	memset(&cap, 0, sizeof(cap));
	prctl(PR_SET_KEEPCAPS, 1, 0, 0, 0);
	retvalue =  setgid(AID_SYSTEM);
	if(retvalue != 0){
		LOGE("setgid error");
		return retvalue;
	}
	retvalue = setuid(AID_SYSTEM);
	if(retvalue != 0){
		LOGE("setuid error");
		return retvalue;
	}
	header.version = _LINUX_CAPABILITY_VERSION;
	header.pid = 0;
	cap.effective = 1 << CAP_SYS_RAWIO;
	cap.permitted = cap.effective;
	cap.inheritable = 0;
	capset(&header, &cap);
	return 0;
}

/* @brief This function calls all the starting functions of the listeners.
*/

int start_listener(int index, enum qseecomd_hierarchy hierarchy)
{
	int ret = 0;
	long (*service_initialize)(int) = NULL;

	LOGE("Listener: index = %d, hierarchy = %d",index,hierarchy);
	listeners[index].qhandle  = dlopen(listeners[index].file_name, RTLD_NOW);
	if (listeners[index].qhandle) {
		LOGD("Init dlopen(%s, RTLD_NOW) succeeds",
					listeners[index].file_name);
		/* call rpmb_init_service or ssd_init_service or secure_ui_service, only if the call is from parent*/
		if (((listeners[index].id == RPMB_SERVICE) ||
			(listeners[index].id == SSD_SERVICE) ||
			(listeners[index].id == SECURE_UI_SERVICE))&&
			(hierarchy == QSEECOMD_PARENT_PROCESS)) {
			*(void **)(&service_initialize)=
						dlsym(listeners[index].qhandle,
							listeners[index].file_init);
			if (service_initialize) {
				LOGD("Init::Init dlsym(g_FSHandle %s) succeeds",
							listeners[index].file_init);
				ret = (*service_initialize)(listeners[index].id);
				LOGD("Init %s ret = %d", listeners[index].file_init, ret);
				if (ret == 0){
					LOGD("%s: init (Listener ID = %d)",
						listeners[index].service_name, listeners[index].id );
				}
				else {
					LOGE("Init dlsym(g_FSHandle, %s) fail",
							listeners[index].file_init);
				}
			} else {
				LOGE("file_init handle failed");
				ret = -1;
			}
			return ret;
		}
		/* Normal file_start for all services */
		*(void **)(&service_initialize)=
					dlsym(listeners[index].qhandle,
						listeners[index].file_start);
		if (service_initialize) {
			LOGD("Init::Init dlsym(g_FSHandle %s) succeeds",
						listeners[index].file_start);
			ret = (*service_initialize)(listeners[index].id);
			LOGD("Init %s ret = %d", listeners[index].file_start, ret);
			if (ret == 0){
				LOGD("%s: Started (Listener ID = %d)",
					listeners[index].service_name, listeners[index].id );
			}
			else
				LOGE("Init dlsym(g_FSHandle, %s) fail",
						listeners[index].file_start);
		} else {
			LOGE("Init dlopen(%s, RLTD_NOW) is failed.... %s\n",
						listeners[index].file_name, dlerror());
			ret = -1;
		}
	} else {
		LOGE("Init dlopen(%s, RLTD_NOW) is failed.... %s\n",
					listeners[index].file_name, dlerror());
		ret = -1;
	}

	return ret;

}

void start_listener_services(void)
{
	int ret = 0;
	int num_listeners = 0;
	int listener_index = 0;

	/* Start the listener services */
	num_listeners = sizeof(listeners)/sizeof(struct qseecom_listener_services);
	LOGD("Total listener services to start = %d", num_listeners);
	for (listener_index = 0; listener_index < num_listeners; listener_index++)
		ret += start_listener(listener_index, QSEECOMD_CHILD_PROCESS);
}

#ifdef HLOS_LOAD_KEYMASTER
static int qseecomd_load_keymaster(void)
{
	struct QSEECom_handle *keymaster_handle = NULL;
	return QSEECom_start_app(&keymaster_handle, "/firmware/image", "keymaste", 1024);
}
#else
static int qseecomd_load_keymaster(void)
{
	return 0;
}
#endif

#ifdef OE
static int property_set(const char *key, const char *value)
{
	return 0;
}
#endif

int main() {
	int ret = 0;
	int status = 0;
	LOGD("qseecom listener services process entry PPID = %d", getppid());

#ifdef RPMB_LISTENER_ENABLED
	ret = start_listener(RPMB_LISTENER_INDEX, QSEECOMD_PARENT_PROCESS);
	if(ret) {
		LOGE("ERROR: RPMB_INIT failed, shall not start listener services");
		return -1;
	}
#endif

#ifndef OE
	ret = start_listener(SSD_LISTENER_INDEX, QSEECOMD_PARENT_PROCESS);
	if(ret) {
		LOGE("ERROR: SSD_INIT failed, shall not start listener services");
		return -1;
	}
	ret = start_listener(SEC_UI_LISTENER_INDEX, QSEECOMD_PARENT_PROCESS);
	if(ret) {
		LOGD("SEC_UI_INIT failed, shall not start secure ui listener");
	}
#endif
	if(0 != qseecomd_update_cap()){
		LOGE("qseecomd permission reset failed, not starting services");
		return -1;
	}

	ret = fork();
	if (ret < 0) {
		LOGE("Failed to create qseecom daemon!! ");
		exit(1);
	}

	if (ret > 0) {
		LOGD("Parent qseecom daemon process paused!! ");
		ret = wait(&status);
		return -1;
	}

	/* listener services (qceecom daemon) continues */
	LOGD("qseecom listener service threads starting!!! ");
	start_listener_services();

	ret = qseecomd_load_keymaster();
	if (ret)
		LOGE("Loading keymaster app failied with ret = %d", ret);
	else
		property_set("sys.listeners.registered", "true");

	LOGD("QSEECOM DAEMON RUNNING ");
	pause();
	LOGD("qseecom listener services process exiting!!! ");
	return -1;
}

#ifdef __cplusplus
}
#endif
