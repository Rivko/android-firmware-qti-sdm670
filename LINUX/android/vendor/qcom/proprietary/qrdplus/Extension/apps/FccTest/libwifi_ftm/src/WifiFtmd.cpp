/*
 * Copyright (c) 2013, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#include "WifiFtmController.h"

#define LOG_TAG "WifiFtm"
#include <stdlib.h>
#include <string.h>
#include <cutils/log.h>
#include <unistd.h>
#include <errno.h>
#include <sys/capability.h>
#include <sys/prctl.h>

#include <private/android_filesystem_config.h>

void initializeDaemonPriviledge()
{
	struct __user_cap_header_struct cap_header;
	struct __user_cap_data_struct cap_data;
	const unsigned int capabilities = (1<< CAP_SYS_MODULE) | (1<<CAP_NET_ADMIN);
	const gid_t groups[] = {AID_INET, AID_NET_ADMIN};

	if (prctl(PR_SET_KEEPCAPS, 1, 0, 0, 0) != 0)
	{
	   ALOGE("%d PR_SET_KEEPCAPS error:%s", __LINE__, strerror(errno));
	   exit(1);
	}

	if (setgroups(sizeof(groups)/sizeof(groups[0]), groups) != 0)
	{
	   ALOGE("setgroups error %s", strerror(errno));
	   exit(1);
	}


	if(setgid(AID_SYSTEM))
	{
	   ALOGE("WIFI FTMD SETGID %s", strerror(errno));
	   exit(1);
	}
	if(setuid(AID_SYSTEM))
	{
	   ALOGE("WIFI FTMD SETUID %s", strerror(errno));
	   exit(1);
	}

	cap_header.pid = 0;
	cap_header.version = _LINUX_CAPABILITY_VERSION;
	memset(&cap_data, 0, sizeof(cap_data));

	/* Assign correct CAP */
	cap_data.effective = capabilities;
	cap_data.permitted = capabilities;
	cap_data.inheritable = capabilities;
	/* Set the capabilities */
	if (capset(&cap_header, &cap_data) < 0)
	{
	   ALOGE("%d failed capset error:%s", __LINE__, strerror(errno));
	   exit(1);
	}


}

int main() {

    WifiFtmController controller("wififtmd_server");

    controller.start();

    return 0;
}
