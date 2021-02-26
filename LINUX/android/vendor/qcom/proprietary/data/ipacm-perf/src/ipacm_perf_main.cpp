/******************************************************************************

                        IPACM_PERF_MAIN.C

******************************************************************************/

/******************************************************************************

  @file    ipacm_perf_main.c
  @brief   Main function of ipacm_perf module

  DESCRIPTION
  Implementation of IPACM performance manager.

  ---------------------------------------------------------------------------
  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------

******************************************************************************/


/******************************************************************************

                      EDIT HISTORY FOR FILE

when        who       what, where, why
--------   -----      -------------------------------------------------------
03/05/15   shihuanl   Initial version

******************************************************************************/

#include <unistd.h>
#include "ipacm_perf.h"
#include "ipacm_perf_netlink.h"
#include "ipacm_perf_qmi.h"
#include "ipacm_perf_rm.h"

int main(int argc, char **argv)
{
	int ret;
	pthread_t netlink_thread = 0;

	ipacm_perf_rm* rm_ptr = ipacm_perf_rm::get_instance();
	ipacm_perf_qmi* qmi_ptr = ipacm_perf_qmi::get_instance();

	ret = pthread_create(&netlink_thread, NULL, netlink_start, NULL);
	if (ret != 0)
	{
		IPACM_PERF_ERR("Failed to create netlink thread\n");
		return ret;
	}
	IPACM_PERF_DBG("Created netlink thread\n");

	pthread_join(netlink_thread, NULL);

	ipacm_perf_rm::delete_instance();
	ipacm_perf_qmi::delete_instance();

	return IPACM_PERF_SUCCESS;
}
