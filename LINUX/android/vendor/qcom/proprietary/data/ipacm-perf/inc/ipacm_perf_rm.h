#ifndef _IPACM_PERF_RM_H_
#define _IPACM_PERF_RM_H_
/******************************************************************************

                        IPACM_PERF_RM.H

******************************************************************************/

/******************************************************************************

  @file ipacm_perf_rm.h
  @brief ipacm_perf_rm header file.

  DESCRIPTION
  Header file of ipacm_perf_rm.

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

#include <stdint.h>
#include <pthread.h>
#include <linux/rmnet_ipa_fd_ioctl.h>

#define WWAN_QMI_IOCTL_DEVICE_NAME "/dev/wwan_ioctl"

#define IPACM_PERF_RM_LTE_CAT16_THRESHOLD 950000

#define IPACM_PERF_RM_LTE_CAT16_BW_VOTE_MBPS 1000
#define IPACM_PERF_RM_NON_LTE_CAT16_BW_VOTE_MBPS 100

class ipacm_perf_rm
{
public:

	/* get ipacm_perf_rm instance */
	static ipacm_perf_rm* get_instance();

	/* delete ipacm_perf_rm instance */
	static void delete_instance();

	/* set modem configured throughput */
	void set_modem_configured_throughput(uint32_t ul_tput, uint32_t dl_tput);

	/* set state of monitored interface */
	void set_monitored_iface_state(monitored_iface iface, bool iface_up);

private:

	ipacm_perf_rm();

	~ipacm_perf_rm();

	/* voting for modem DL BW */
	int bw_vote();

	static ipacm_perf_rm* p_instance;

	ipacm_perf_throughput modem_configured_throughput;

	bool is_iface_up[IFACE_MAX];

	pthread_mutex_t tput_mutex;
	pthread_mutex_t iface_mutex;

};

#endif /* _IPACM_PERF_RM_H_ */

