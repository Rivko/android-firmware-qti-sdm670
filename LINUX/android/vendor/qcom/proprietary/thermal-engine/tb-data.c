/*===========================================================================

Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/

#include "thermal.h"
#include "thermal_config.h"
#include "tb_algorithm.h"

static struct setting_info tb_cfgs_8992[] =
{
	{
		.disable = 1,
		.desc = "TB-CPU0",
		.algo_type = TB_ALGO_TYPE,
		.data.tb =
		{
			.sensor = "cpu0-1",
			.device = "cluster0",
			.sampling_period_ms = 10,
			.set_point = 85000,
			.set_point_clr = 55000,
			/* tuning params */
			.up_loop_gain = 2,
			.down_loop_gain = 3,
			.auto_penalty = 1.0,
			.auto_reward = 0.0,
			.temp_scale_factor = 1000,
			.freq_scale_factor = 1,
			.quadratic_reward = 1,
			.quadratic_penalty = 1,
			.use_timeout = 0,
			.timeout = 1
		},
	},
	{
		.disable = 1,
		.desc = "TB-CPU2",
		.algo_type = TB_ALGO_TYPE,
		.data.tb =
		{
			.sensor = "cpu2",
			.device = "cluster0",
			.sampling_period_ms = 10,
			.set_point = 85000,
			.set_point_clr = 55000,
			/* tuning params */
			.up_loop_gain = 2,
			.down_loop_gain = 3,
			.auto_penalty = 1.0,
			.auto_reward = 0.0,
			.temp_scale_factor = 1000,
			.freq_scale_factor = 1,
			.quadratic_reward = 1,
			.quadratic_penalty = 1,
			.use_timeout = 0,
			.timeout = 1
		},
	},
	{
		.disable = 1,
		.desc = "TB-CPU3",
		.algo_type = TB_ALGO_TYPE,
		.data.tb =
		{
			.sensor = "cpu3",
			.device = "cluster0",
			.sampling_period_ms = 10,
			.set_point = 85000,
			.set_point_clr = 55000,
			/* tuning params */
			.up_loop_gain = 2,
			.down_loop_gain = 3,
			.auto_penalty = 1.0,
			.auto_reward = 0.0,
			.temp_scale_factor = 1000,
			.freq_scale_factor = 1,
			.quadratic_reward = 1,
			.quadratic_penalty = 1,
			.use_timeout = 0,
			.timeout = 1
		},
	},
	{
		.disable = 1,
		.desc = "TB-CPU4",
		.algo_type = TB_ALGO_TYPE,
		.data.tb =
		{
			.sensor = "cpu4",
			.device = "cluster1",
			.sampling_period_ms = 10,
			.set_point = 85000,
			.set_point_clr = 55000,
			/* tuning params */
			.up_loop_gain = 2,
			.down_loop_gain = 3,
			.auto_penalty = 1.0,
			.auto_reward = 0.0,
			.temp_scale_factor = 1000,
			.freq_scale_factor = 1,
			.quadratic_reward = 1,
			.quadratic_penalty = 1,
			.use_timeout = 0,
			.timeout = 1
		},
	},
	{
		.disable = 1,
		.desc = "TB-CPU5",
		.algo_type = TB_ALGO_TYPE,
		.data.tb =
		{
			.sensor = "cpu5",
			.device = "cluster1",
			.sampling_period_ms = 10,
			.set_point = 85000,
			.set_point_clr = 55000,
			/* tuning params */
			.up_loop_gain = 2,
			.down_loop_gain = 3,
			.auto_penalty = 1.0,
			.auto_reward = 0.0,
			.temp_scale_factor = 1000,
			.freq_scale_factor = 1,
			.quadratic_reward = 1,
			.quadratic_penalty = 1,
			.use_timeout = 0,
			.timeout = 1
		},
	},
	{
		.disable = 1,
		.desc = "TB-POPMEM",
		.algo_type = TB_ALGO_TYPE,
		.data.tb =
		{
			.sensor = "pop_mem",
			.device = "cluster1",
			.sampling_period_ms = 10,
			.set_point = 85000,
			.set_point_clr = 55000,
			/* tuning params */
			.up_loop_gain = 2,
			.down_loop_gain = 3,
			.auto_penalty = 1.0,
			.auto_reward = 0.0,
			.temp_scale_factor = 1000,
			.freq_scale_factor = 1,
			.quadratic_reward = 1,
			.quadratic_penalty = 1,
			.use_timeout = 0,
			.timeout = 1
		},
	},
	{
		.disable = 1,
		.desc = "TB-GPU",
		.algo_type = TB_ALGO_TYPE,
		.data.tb =
		{
			.sensor = "gpu",
			.device = "gpu",
			.sampling_period_ms = 20,
			.set_point = 85000,
			.set_point_clr = 55000,
			/* tuning params */
			.up_loop_gain = 2,
			.down_loop_gain = 3,
			.auto_penalty = 1.0,
			.auto_reward = 0.0,
			.temp_scale_factor = 1000,
			.freq_scale_factor = 1000, /* Make 1 if 8992 GPU freq not in Hz */
			.quadratic_reward = 1,
			.quadratic_penalty = 1,
			.use_timeout = 0,
			.timeout = 1
		},
	},
};

static struct setting_info tb_cfgs_8994[] =
{
	{
		.disable = 1,
		.desc = "TB-CPU0",
		.algo_type = TB_ALGO_TYPE,
		.data.tb =
		{
			.sensor = "cpu0",
			.device = "cluster0",
			.sampling_period_ms = 10,
			.set_point = 85000,
			.set_point_clr = 55000,
			/* tuning params */
			.up_loop_gain = 2,
			.down_loop_gain = 3,
			.auto_penalty = 1.0,
			.auto_reward = 0.0,
			.temp_scale_factor = 1000,
			.freq_scale_factor = 1,
			.quadratic_reward = 1,
			.quadratic_penalty = 1,
			.use_timeout = 0,
			.timeout = 1
		},
	},
	{
		.disable = 1,
		.desc = "TB-CPU1",
		.algo_type = TB_ALGO_TYPE,
		.data.tb =
		{
			.sensor = "cpu1",
			.device = "cluster0",
			.sampling_period_ms = 10,
			.set_point = 85000,
			.set_point_clr = 55000,
			/* tuning params */
			.up_loop_gain = 2,
			.down_loop_gain = 3,
			.auto_penalty = 1.0,
			.auto_reward = 0.0,
			.temp_scale_factor = 1000,
			.freq_scale_factor = 1,
			.quadratic_reward = 1,
			.quadratic_penalty = 1,
			.use_timeout = 0,
			.timeout = 1
		},
	},
	{
		.disable = 1,
		.desc = "TB-CPU2",
		.algo_type = TB_ALGO_TYPE,
		.data.tb =
		{
			.sensor = "cpu2",
			.device = "cluster0",
			.sampling_period_ms = 10,
			.set_point = 85000,
			.set_point_clr = 55000,
			/* tuning params */
			.up_loop_gain = 2,
			.down_loop_gain = 3,
			.auto_penalty = 1.0,
			.auto_reward = 0.0,
			.temp_scale_factor = 1000,
			.freq_scale_factor = 1,
			.quadratic_reward = 1,
			.quadratic_penalty = 1,
			.use_timeout = 0,
			.timeout = 1
		},
	},
	{
		.disable = 1,
		.desc = "TB-CPU3",
		.algo_type = TB_ALGO_TYPE,
		.data.tb =
		{
			.sensor = "cpu3",
			.device = "cluster0",
			.sampling_period_ms = 10,
			.set_point = 85000,
			.set_point_clr = 55000,
			/* tuning params */
			.up_loop_gain = 2,
			.down_loop_gain = 3,
			.auto_penalty = 1.0,
			.auto_reward = 0.0,
			.temp_scale_factor = 1000,
			.freq_scale_factor = 1,
			.quadratic_reward = 1,
			.quadratic_penalty = 1,
			.use_timeout = 0,
			.timeout = 1
		},
	},
	{
		.disable = 1,
		.desc = "TB-CPU4",
		.algo_type = TB_ALGO_TYPE,
		.data.tb =
		{
			.sensor = "cpu4",
			.device = "cluster1",
			.sampling_period_ms = 10,
			.set_point = 85000,
			.set_point_clr = 55000,
			/* tuning params */
			.up_loop_gain = 2,
			.down_loop_gain = 3,
			.auto_penalty = 1.0,
			.auto_reward = 0.0,
			.temp_scale_factor = 1000,
			.freq_scale_factor = 1,
			.quadratic_reward = 1,
			.quadratic_penalty = 1,
			.use_timeout = 0,
			.timeout = 1
		},
	},
	{
		.disable = 1,
		.desc = "TB-CPU5",
		.algo_type = TB_ALGO_TYPE,
		.data.tb =
		{
			.sensor = "cpu5",
			.device = "cluster1",
			.sampling_period_ms = 10,
			.set_point = 85000,
			.set_point_clr = 55000,
			/* tuning params */
			.up_loop_gain = 2,
			.down_loop_gain = 3,
			.auto_penalty = 1.0,
			.auto_reward = 0.0,
			.temp_scale_factor = 1000,
			.freq_scale_factor = 1,
			.quadratic_reward = 1,
			.quadratic_penalty = 1,
			.use_timeout = 0,
			.timeout = 1
		},
	},
	{
		.disable = 1,
		.desc = "TB-CPU6",
		.algo_type = TB_ALGO_TYPE,
		.data.tb =
		{
			.sensor = "cpu6",
			.device = "cluster1",
			.sampling_period_ms = 10,
			.set_point = 85000,
			.set_point_clr = 55000,
			/* tuning params */
			.up_loop_gain = 2,
			.down_loop_gain = 3,
			.auto_penalty = 1.0,
			.auto_reward = 0.0,
			.temp_scale_factor = 1000,
			.freq_scale_factor = 1,
			.quadratic_reward = 1,
			.quadratic_penalty = 1,
			.use_timeout = 0,
			.timeout = 1
		},
	},
	{
		.disable = 1,
		.desc = "TB-CPU7",
		.algo_type = TB_ALGO_TYPE,
		.data.tb =
		{
			.sensor = "cpu7",
			.device = "cluster1",
			.sampling_period_ms = 10,
			.set_point = 85000,
			.set_point_clr = 55000,
			/* tuning params */
			.up_loop_gain = 2,
			.down_loop_gain = 3,
			.auto_penalty = 1.0,
			.auto_reward = 0.0,
			.temp_scale_factor = 1000,
			.freq_scale_factor = 1,
			.quadratic_reward = 1,
			.quadratic_penalty = 1,
			.use_timeout = 0,
			.timeout = 1
		},
	},
	{
		.disable = 1,
		.desc = "TB-POPMEM",
		.algo_type = TB_ALGO_TYPE,
		.data.tb =
		{
			.sensor = "pop_mem",
			.device = "cluster1",
			.sampling_period_ms = 10,
			.set_point = 80000,
			.set_point_clr = 55000,
			.time_constant = 16,
			/* tuning params */
			.up_loop_gain = 2,
			.down_loop_gain = 3,
			.auto_penalty = 1.0,
			.auto_reward = 0.0,
			.temp_scale_factor = 1000,
			.freq_scale_factor = 1,
			.quadratic_reward = 1,
			.quadratic_penalty = 1,
			.use_timeout = 0,
			.timeout = 1
		},
	},
	{
		.disable = 1,
		.desc = "TB-GPU",
		.algo_type = TB_ALGO_TYPE,
		.data.tb =
		{
			.sensor = "gpu",
			.device = "gpu",
			.sampling_period_ms = 20,
			.set_point = 85000,
			.set_point_clr = 55000,
			/* tuning params */
			.up_loop_gain = 2,
			.down_loop_gain = 3,
			.auto_penalty = 1.0,
			.auto_reward = 0.0,
			.temp_scale_factor = 1000,
			.freq_scale_factor = 1000, /* GPU freq on 8994 is Hz */
			.quadratic_reward = 1,
			.quadratic_penalty = 1,
			.use_timeout = 0,
			.timeout = 1
		},
	},
};

static struct setting_info tb_cfgs_8996[] =
{
	{
		.disable = 1,
		.desc = "TB-CPU0",
		.algo_type = TB_ALGO_TYPE,
		.data.tb =
		{
			.device = "cluster0",
			.sensor = "tsens_tz_sensor4",
			.sampling_period_ms = 10,
			.set_point = 85000,
			.set_point_clr = 55000,
			/* tuning params */
			.up_loop_gain = 2,
			.down_loop_gain = 3,
			.auto_penalty = 1.0,
			.auto_reward = 0.0,
			.temp_scale_factor = 1000,
			.freq_scale_factor = 1,
			.quadratic_reward = 1,
			.quadratic_penalty = 1,
			.use_timeout = 0,
			.timeout = 1
		},
	},
	{
		.disable = 1,
		.desc = "TB-CPU1",
		.algo_type = TB_ALGO_TYPE,
		.data.tb =
		{
			.device = "cluster0",
			.sensor = "tsens_tz_sensor6",
			.sampling_period_ms = 10,
			.set_point = 85000,
			.set_point_clr = 55000,
			/* tuning params */
			.up_loop_gain = 2,
			.down_loop_gain = 3,
			.auto_penalty = 1.0,
			.auto_reward = 0.0,
			.temp_scale_factor = 1000,
			.freq_scale_factor = 1,
			.quadratic_reward = 1,
			.quadratic_penalty = 1,
			.use_timeout = 0,
			.timeout = 1
		},
	},
	{
		.disable = 1,
		.desc = "TB-CPU2",
		.algo_type = TB_ALGO_TYPE,
		.data.tb =
		{
			.device = "cluster1",
			.sensor = "tsens_tz_sensor9",
			.sampling_period_ms = 10,
			.set_point = 85000,
			.set_point_clr = 55000,
			/* tuning params */
			.up_loop_gain = 2,
			.down_loop_gain = 3,
			.auto_penalty = 1.0,
			.auto_reward = 0.0,
			.temp_scale_factor = 1000,
			.freq_scale_factor = 1,
			.quadratic_reward = 1,
			.quadratic_penalty = 1,
			.use_timeout = 0,
			.timeout = 1
		},
	},
	{
		.disable = 1,
		.desc = "TB-CPU3",
		.algo_type = TB_ALGO_TYPE,
		.data.tb =
		{
			.device = "cluster1",
			.sensor = "tsens_tz_sensor11",
			.sampling_period_ms = 10,
			.set_point = 85000,
			.set_point_clr = 55000,
			/* tuning params */
			.up_loop_gain = 2,
			.down_loop_gain = 3,
			.auto_penalty = 1.0,
			.auto_reward = 0.0,
			.temp_scale_factor = 1000,
			.freq_scale_factor = 1,
			.quadratic_reward = 1,
			.quadratic_penalty = 1,
			.use_timeout = 0,
			.timeout = 1
		},
	},
	{
		.disable = 1,
		.desc = "TB-POPMEM",
		.algo_type = TB_ALGO_TYPE,
		.data.tb =
		{
			.sensor = "pop_mem",
			.device = "cluster1",
			.sampling_period_ms = 10,
			.set_point = 85000,
			.set_point_clr = 55000,
			/* tuning params */
			.up_loop_gain = 2,
			.down_loop_gain = 3,
			.auto_penalty = 1.0,
			.auto_reward = 0.0,
			.temp_scale_factor = 1000,
			.freq_scale_factor = 1,
			.quadratic_reward = 1,
			.quadratic_penalty = 1,
			.use_timeout = 0,
			.timeout = 1
		},
	},
	{
		.disable = 1,
		.desc = "TB-GPU",
		.algo_type = TB_ALGO_TYPE,
		.data.tb =
		{
			.sensor = "gpu",
			.device = "gpu",
			.sampling_period_ms = 10,
			.set_point = 85000,
			.set_point_clr = 55000,
			/* tuning params */
			.up_loop_gain = 2,
			.down_loop_gain = 3,
			.auto_penalty = 1.0,
			.auto_reward = 0.0,
			.temp_scale_factor = 1000,
			.freq_scale_factor = 1000, /* Make 1 if 8996 GPU freq not in Hz */
			.quadratic_reward = 1,
			.quadratic_penalty = 1,
			.use_timeout = 0,
			.timeout = 1
		},
	},
};

void tb_init_data(struct thermal_setting_t *setting)
{
	int i, arr_size;
	struct setting_info *cfg;

	switch (therm_get_msm_id()) {
		case THERM_MSM_8992:
			cfg = tb_cfgs_8992;
			arr_size = ARRAY_SIZE(tb_cfgs_8992);
			break;
		case THERM_MSM_8994:
			cfg = tb_cfgs_8994;
			arr_size = ARRAY_SIZE(tb_cfgs_8994);
			break;
		case THERM_MSM_8996:
		case THERM_MSM_8996AU:
			cfg = tb_cfgs_8996;
			arr_size = ARRAY_SIZE(tb_cfgs_8996);
			break;
		default:
			msg("%s: ERROR Uknown device\n", __func__);
			/* Better to have something in place even if it's wrong. */
			cfg = tb_cfgs_8996;
			arr_size = ARRAY_SIZE(tb_cfgs_8996);
			break;
	}

	for (i = 0; i < arr_size; i++)
		add_setting(setting, &cfg[i]);
}
