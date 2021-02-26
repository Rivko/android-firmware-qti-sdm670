/*
 * Copyright (c) 2015 - 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <comdef.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/capability.h>
#include <sys/prctl.h>
#include <private/android_filesystem_config.h>
#ifndef ANDROID_BUILD
#include <sys/syslog.h>
#else
#define LOG_TAG "pd-mapper-svc"
#include <cutils/log.h>
#endif
#include <cutils/klog.h>
#include <cutils/properties.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <json.h>
#include <dirent.h>
#include <map>
#include <list>
#include <string>
#include <pthread.h>
#include "qmi_idl_lib.h"
#include "qmi_csi.h"
#include "qmi_csi_common.h"
#include "qmi_cci_target_ext.h"
#include "service_registry_locator_v01.h"


//Get rid of this
#include "qmi_client.h"
#include "qmi_cci_target_ext.h"
#include "qmi_client_instance_defs.h"
