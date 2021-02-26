/*===========================================================================

  Copyright (c) 2012-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/

#ifndef __DEVICES_H__
#define __DEVICES_H__

struct tmd_cpu_voltage_dev_table_t{
	uint32_t *cluster_freq[MAX_NUM_CLUSTERS];
	uint32_t *voltage_table[MAX_NUM_CLUSTERS];
	uint32_t length;
	uint32_t *actual_freq_table[MAX_NUM_CLUSTERS];
	uint32_t *actual_voltage_table[MAX_NUM_CLUSTERS];
	uint32_t actual_table_length[MAX_NUM_CLUSTERS];
	uint32_t num_clusters;
	device_clnt_handle dev_clnt[MAX_NUM_CLUSTERS];
	uint32_t current_level;
	uint32_t table_max_cluster_num;
};

/* Init platform target devices */
int devices_init(int minimum_mode);

/* Release platfom target devices */
void devices_release(void);

#ifdef ENABLE_CPU_VOLTAGE_DEV
int tmd_init_voltage_devs(void);
void print_voltage_device_info(void);
#else
static inline int tmd_init_voltage_devs(void)
{
	return 0;
}
static inline void print_voltage_device_info(void){}
#endif

#endif /* __DEVICES_H__ */
