/******************************************************************************

                        IPACM_PERF_RM.CPP

******************************************************************************/

/******************************************************************************

  @file    ipacm_perf_rm.cpp
  @brief   The resource manager module for ipacm_perf

  DESCRIPTION
  Implementation of ipacm_perf_rm.

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

#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "ipacm_perf.h"
#include "ipacm_perf_rm.h"

ipacm_perf_rm* ipacm_perf_rm::p_instance = NULL;

ipacm_perf_rm::ipacm_perf_rm()
{
	memset(&modem_configured_throughput, 0, sizeof(modem_configured_throughput));
	memset(is_iface_up, 0, sizeof(is_iface_up));
	tput_mutex = PTHREAD_MUTEX_INITIALIZER;
	iface_mutex = PTHREAD_MUTEX_INITIALIZER;
	return;
}

ipacm_perf_rm::~ipacm_perf_rm()
{
}

/* voting for modem DL BW */
int ipacm_perf_rm::bw_vote()
{
	uint32_t bw_mbps;
	int i, fd, ret;
	bool has_iface_up = false;

	fd = open(WWAN_QMI_IOCTL_DEVICE_NAME, O_RDWR);
	if(fd < 0)
	{
		IPACM_PERF_ERR("Failed to open %s.\n",WWAN_QMI_IOCTL_DEVICE_NAME);
		return IPACM_PERF_FAILURE;
	}

	for(i=0; i<IFACE_MAX; i++)
	{
		if(is_iface_up[i] == true)
		{
			has_iface_up = true;
			break;
		}
	}

	/* if modem DL tput is LTE cat 16 and there is one monitored iface up, vote for high BW (1000Mbps)*/
	if(has_iface_up && modem_configured_throughput.dl_configured_throughput >= IPACM_PERF_RM_LTE_CAT16_THRESHOLD)
	{
		bw_mbps = IPACM_PERF_RM_LTE_CAT16_BW_VOTE_MBPS;
	}
	else /* otherwise vote for low BW (100Mbps) */
	{
		bw_mbps = IPACM_PERF_RM_NON_LTE_CAT16_BW_VOTE_MBPS;
	}
#if 1
	IPACM_PERF_DBG("Voting for BW %d Mbps\n", bw_mbps);

	ret = ioctl(fd, WAN_IOC_VOTE_FOR_BW_MBPS, &bw_mbps);
	if(ret != 0)
	{
		IPACM_PERF_ERR("Failed to vote for BW: ret %d\n", ret);
		close(fd);
		return IPACM_PERF_FAILURE;
	}

	IPACM_PERF_DBG("Succeeded in voting for BW\n");
#endif
	close(fd);
	return IPACM_PERF_SUCCESS;
}

/* set modem configured throughput */
void ipacm_perf_rm::set_modem_configured_throughput(uint32_t ul_tput, uint32_t dl_tput)
{
	if(pthread_mutex_lock(&tput_mutex) != 0)
	{
		IPACM_PERF_ERR("Unable to lock the mutex\n");
		return;
	}

	if(modem_configured_throughput.ul_configured_throughput == ul_tput
		&& modem_configured_throughput.dl_configured_throughput == dl_tput)
	{
		IPACM_PERF_DBG("Modem cfg tput does not change\n");
		pthread_mutex_unlock(&tput_mutex);
		return;
	}

	modem_configured_throughput.ul_configured_throughput = ul_tput;
	modem_configured_throughput.dl_configured_throughput = dl_tput;
	IPACM_PERF_DBG("UL tput: %d, DL tput: %d\n", ul_tput, dl_tput);
	bw_vote();

	if(pthread_mutex_unlock(&tput_mutex) != 0)
	{
		IPACM_PERF_ERR("Unable to unlock the mutex\n");
		return;
	}
	return;
}

/* set state of monitored interface */
void ipacm_perf_rm::set_monitored_iface_state(monitored_iface iface, bool iface_up)
{
	int i;

	if(pthread_mutex_lock(&iface_mutex) != 0)
	{
		IPACM_PERF_ERR("Unable to lock the mutex\n");
		return;
	}

	if(is_iface_up[iface] == iface_up)
	{
		IPACM_PERF_DBG("Iface state does not change\n");
		pthread_mutex_unlock(&iface_mutex);
		return;
	}

	is_iface_up[iface] = iface_up;
	IPACM_PERF_DBG("Iface: %d, is iface up: %d\n", iface, iface_up);
	for(i=0; i<IFACE_MAX; i++)
	{
		if(i != iface && is_iface_up[i] == true)
		{
			IPACM_PERF_DBG("Other iface %d is up\n", i);
			break;
		}
	}

	if(i == IFACE_MAX)	//no other monitored interface is up
	{
		bw_vote();
	}

	if(pthread_mutex_unlock(&iface_mutex) != 0)
	{
		IPACM_PERF_ERR("Unable to unlock the mutex\n");
		return;
	}
	return;
}

/* get ipacm_perf_rm instance */
ipacm_perf_rm* ipacm_perf_rm::get_instance()
{
	if(ipacm_perf_rm::p_instance == NULL)
	{
		ipacm_perf_rm::p_instance = new ipacm_perf_rm();
		IPACM_PERF_DBG("Rm instance pointer: 0x%08x\n", ipacm_perf_rm::p_instance);
	}
	return ipacm_perf_rm::p_instance;
}

/* delete ipacm_perf_rm instance */
void ipacm_perf_rm::delete_instance()
{
	if(ipacm_perf_rm::p_instance != NULL)
	{
		delete ipacm_perf_rm::p_instance;
		IPACM_PERF_DBG("Rm instance is deleted\n");
		ipacm_perf_rm::p_instance = NULL;
	}
	return;
}
