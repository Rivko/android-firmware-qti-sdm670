#ifndef _IPACM_PERF_H_
#define _IPACM_PERF_H_
/******************************************************************************

                        IPACM_PERF.H

******************************************************************************/

/******************************************************************************

  @file ipacm_perf.h
  @brief ipacm_perf header file.

  DESCRIPTION
  Header file of IPACM performance manager.

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

#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include "ds_util.h"

#define IPACM_PERF_SUCCESS 0
#define IPACM_PERF_FAILURE 1

typedef struct
{
	uint8_t ul_configured_throughput_valid;
	uint32_t ul_configured_throughput;
	uint8_t dl_configured_throughput_valid;
	uint32_t dl_configured_throughput;
} ipacm_perf_throughput;

enum monitored_iface
{
	IFACE_USB,
	IFACE_CPE,
	IFACE_MAX
};

#define IPACM_PERF_ERR(fmt, ...) printf("ERROR: %s:%d %s() " fmt, __FILE__,  __LINE__, __FUNCTION__, ##__VA_ARGS__);\
                                 ds_log_med("ERROR: " fmt, ##__VA_ARGS__);

#define IPACM_PERF_DBG(fmt, ...) printf("%s:%d %s() " fmt, __FILE__,  __LINE__, __FUNCTION__, ##__VA_ARGS__);\
                                 ds_log_med(fmt, ##__VA_ARGS__);

#endif /* _IPACM_PERF_H_ */
