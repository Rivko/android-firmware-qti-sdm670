/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/

#ifndef __TB_ALGORITHM_H__
#define __TB_ALGORITHM_H__

#include "thermal_algorithm.h"

#ifdef ENABLE_TB
void tb_init_data(struct thermal_setting_t *setting);
extern struct thermal_algo_info tb_algo_info;
static inline enum thermal_algo_ret_t tb_algo_init(
	struct thermal_setting_t* settings)
{
	return thermal_algo_start(&tb_algo_info, settings);
}
#else
static inline void tb_init_data(struct thermal_setting_t *setting){}
static inline enum thermal_algo_ret_t tb_algo_init(
	struct thermal_setting_t* settings)
{
	return THERMAL_ALGO_SUCCESS;
}
#endif /* ENABLE_TB */

#endif /* __TB_ALGORITHM_H__ */
