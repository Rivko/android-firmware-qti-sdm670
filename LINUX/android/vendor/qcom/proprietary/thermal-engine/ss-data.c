/*===========================================================================

Copyright (c) 2013-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/

#include "thermal.h"
#include "thermal_config.h"
#include "ss_algorithm.h"

static struct setting_info ss_cfgs_8974[] =
{
	{
		.desc = "SS-CPU0",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu0",
			.device = "cpu",
			.sampling_period_ms = 65,
			.set_point = 90000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU1",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu1",
			.device = "cpu",
			.sampling_period_ms = 65,
			.set_point = 90000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU2",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu2",
			.device = "cpu",
			.sampling_period_ms = 65,
			.set_point = 90000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU3",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu3",
			.device = "cpu",
			.sampling_period_ms = 65,
			.set_point = 90000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-POPMEM",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "pop_mem",
			.device = "cpu",
			.sampling_period_ms = 65,
			.set_point = 80000,
			.set_point_clr = 55000,
			.time_constant = 16,
		},
	},
};

static struct setting_info ss_cfgs_8974_pro_ac[] =
{
	{
		.desc = "SS-CPU0",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu0",
			.device = "cpu",
			.sampling_period_ms = 65,
			.set_point = 90000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU1",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu1",
			.device = "cpu",
			.sampling_period_ms = 65,
			.set_point = 90000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU2",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu2",
			.device = "cpu",
			.sampling_period_ms = 65,
			.set_point = 90000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU3",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu3",
			.device = "cpu",
			.sampling_period_ms = 65,
			.set_point = 90000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-POPMEM",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "pop_mem",
			.device = "cpu",
			.sampling_period_ms = 65,
			.set_point = 80000,
			.set_point_clr = 55000,
			.time_constant = 16,
		},
	},
	{
		.desc = "SS-GPU",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "tsens_tz_sensor10",
			.device = "gpu",
			.sampling_period_ms = 250,
			.set_point = 100000,
			.set_point_clr = 65000,
		},
	},
};

static struct setting_info ss_cfgs_8226_v1[] =
{
	{
		.desc = "SS-CPU2-3",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu2-3",
			.device = "cpu",
			.sampling_period_ms = 1000,
			.set_point = 90000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-POPMEM",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "pop_mem",
			.device = "cpu",
			.sampling_period_ms = 1000,
			.set_point = 50000,
			.set_point_clr = 45000,
			.time_constant = 5,
		},
	},
};

static struct setting_info ss_cfgs_8226[] =
{
	{
		.desc = "SS-CPU0-1",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu0-1",
			.device = "cpu",
			.sampling_period_ms = 250,
			.set_point = 90000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU2-3",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu2-3",
			.device = "cpu",
			.sampling_period_ms = 250,
			.set_point = 90000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-POPMEM",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "pop_mem",
			.device = "cpu",
			.sampling_period_ms = 1000,
			.set_point = 60000,
			.set_point_clr = 45000,
			.time_constant = 2,
		},
	},
};

static struct setting_info ss_cfgs_8610[] =
{
	{
		.desc = "SS-CPU0-1-2-3",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu0-1-2-3",
			.device = "cpu",
			.sampling_period_ms = 250,
			.set_point = 80000,
			.set_point_clr = 65000,
		},
	},
};

static struct setting_info ss_cfgs_8916[] =
{
	{
		.desc = "SS-CPU0-1",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu0-1",
			.device = "cpu",
			.sampling_period_ms = 250,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU2-3",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu2-3",
			.device = "cpu",
			.sampling_period_ms = 250,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-POPMEM",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "pop_mem",
			.device = "cpu",
			.sampling_period_ms = 1000,
			.set_point = 70000,
			.set_point_clr = 45000,
			.time_constant = 2,
		},
	},
};

static struct setting_info ss_cfgs_9900[] =
{
	{
		.desc = "SS-CPU0",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu0",
			.device = "cpu",
			.sampling_period_ms = 65,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU1",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu1",
			.device = "cpu",
			.sampling_period_ms = 65,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU2",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu2",
			.device = "cpu",
			.sampling_period_ms = 65,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU3",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu3",
			.device = "cpu",
			.sampling_period_ms = 65,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
};

static struct setting_info ss_cfgs_8939[] =
{
	{
		.desc = "SS-CPU0",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu0",
			.device = "cluster1",
			.sampling_period_ms = 50,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU1",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu1",
			.device = "cluster1",
			.sampling_period_ms = 50,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU2",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu2",
			.device = "cluster1",
			.sampling_period_ms = 50,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU3",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu3",
			.device = "cluster1",
			.sampling_period_ms = 50,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU4-5-6-7",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu4-5-6-7",
			.device = "cluster0",
			.sampling_period_ms = 50,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-POPMEM",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "pop_mem",
			.device = "cluster1",
			.sampling_period_ms = 250,
			.set_point = 70000,
			.set_point_clr = 55000,
			.time_constant = 2,
		},
	},
};

static struct setting_info ss_cfgs_8936[] =
{
	{
		.desc = "SS-CPU0",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu0",
			.device = "cluster0",
			.sampling_period_ms = 50,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU1",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu1",
			.device = "cluster0",
			.sampling_period_ms = 50,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU2",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu2",
			.device = "cluster0",
			.sampling_period_ms = 50,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU3",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu3",
			.device = "cluster0",
			.sampling_period_ms = 50,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-POPMEM",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "pop_mem",
			.device = "cluster0",
			.sampling_period_ms = 250,
			.set_point = 70000,
			.set_point_clr = 55000,
			.time_constant = 2,
		},
	},
};

static struct setting_info ss_cfgs_8994[] =
{
	{
		.desc = "SS-CPU0",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu0",
			.device = "cluster0",
			.sampling_period_ms = 10,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU1",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu1",
			.device = "cluster0",
			.sampling_period_ms = 10,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU2",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu2",
			.device = "cluster0",
			.sampling_period_ms = 10,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU3",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu3",
			.device = "cluster0",
			.sampling_period_ms = 10,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU4",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu4",
			.device = "cluster1",
			.sampling_period_ms = 10,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU5",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu5",
			.device = "cluster1",
			.sampling_period_ms = 10,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU6",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu6",
			.device = "cluster1",
			.sampling_period_ms = 10,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU7",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu7",
			.device = "cluster1",
			.sampling_period_ms = 10,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-POPMEM",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "pop_mem",
			.device = "cluster1",
			.sampling_period_ms = 10,
			.set_point = 80000,
			.set_point_clr = 55000,
			.time_constant = 16,
		},
	},
	{
		.desc = "SS-GPU",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "gpu",
			.device = "gpu",
			.sampling_period_ms = 250,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
};

static struct setting_info ss_cfgs_8992[] =
{
	{
		.desc = "SS-CPU0",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu0-1",
			.device = "cluster0",
			.sampling_period_ms = 10,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU2",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu2",
			.device = "cluster0",
			.sampling_period_ms = 10,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU3",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu3",
			.device = "cluster0",
			.sampling_period_ms = 10,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU4",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu4",
			.device = "cluster1",
			.sampling_period_ms = 10,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU5",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu5",
			.device = "cluster1",
			.sampling_period_ms = 10,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-POPMEM",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "pop_mem",
			.device = "cluster1",
			.sampling_period_ms = 10,
			.set_point = 80000,
			.set_point_clr = 55000,
			.time_constant = 16,
		},
	},
	{
		.desc = "SS-GPU",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "gpu",
			.device = "gpu",
			.sampling_period_ms = 250,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
};

static struct setting_info ss_cfgs_8996AU[] =
{
	{
		.desc = "SS-CPUS-ALL",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "VIRTUAL-CPUS",
			.device = "cpu_voltage",
			.sampling_period_ms = 10,
			.set_point = 105000,
			.set_point_clr = 85000,
			.device_perf_floor = 1036800,
		},
	},
	{
		.desc = "SS-POPMEM",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "pop_mem",
			.device = "cluster1",
			.sampling_period_ms = 10,
			.set_point = 105000,
			.set_point_clr = 75000,
			.time_constant = 16,
		},
	},
	{
		.desc = "SS-GPU",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "gpu",
			.device = "gpu",
			.sampling_period_ms = 250,
			.set_point = 100000,
			.set_point_clr = 70000,
		},
	},
};

static struct setting_info ss_cfgs_8996[] =
{
	{
		.desc = "SS-CPUS-ALL",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "VIRTUAL-CPUS",
			.device = "cpu_voltage",
			.sampling_period_ms = 10,
			.set_point = 95000,
			.set_point_clr = 65000,
			.device_perf_floor = 1036800,
		},
	},
	{
		.desc = "SS-POPMEM",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "pop_mem",
			.device = "cluster1",
			.sampling_period_ms = 10,
			.set_point = 95000,
			.set_point_clr = 65000,
			.time_constant = 16,
		},
	},
	{
		.desc = "SS-GPU",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "gpu",
			.device = "gpu",
			.sampling_period_ms = 250,
			.set_point = 95000,
			.set_point_clr = 65000,
		},
	},
};

static struct setting_info ss_cfgs_8952[] =
{
	{
		.desc = "SS-CPU0",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "tsens_tz_sensor5",
			.device = "cluster1",
			.sampling_period_ms = 50,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU1",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "tsens_tz_sensor6",
			.device = "cluster1",
			.sampling_period_ms = 50,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU2",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "tsens_tz_sensor7",
			.device = "cluster1",
			.sampling_period_ms = 50,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU3",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "tsens_tz_sensor8",
			.device = "cluster1",
			.sampling_period_ms = 50,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU4-5-6-7",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "tsens_tz_sensor9",
			.device = "cluster0",
			.sampling_period_ms = 50,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-POPMEM",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "pop_mem",
			.device = "cluster1",
			.sampling_period_ms = 250,
			.set_point = 70000,
			.set_point_clr = 55000,
			.time_constant = 2,
			.device_mtgn_max_limit = 960000,
		},
	},
};

static struct setting_info ss_cfgs_8976[] =
{
	{
		.desc = "SS-CPU0-1-2-3",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu0-1-2-3",
			.device = "cluster0",
			.sampling_period_ms = 50,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU4",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu4",
			.device = "cluster1",
			.sampling_period_ms = 50,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU5",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu5",
			.device = "cluster1",
			.sampling_period_ms = 50,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU6",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu6",
			.device = "cluster1",
			.sampling_period_ms = 50,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU7",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu7",
			.device = "cluster1",
			.sampling_period_ms = 50,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-L2_CACHE_1",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "L2_cache_1",
			.device = "cluster1",
			.sampling_period_ms = 50,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-POPMEM",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "pop_mem",
			.device = "cluster1",
			.sampling_period_ms = 250,
			.set_point = 70000,
			.set_point_clr = 55000,
			.time_constant = 2,
		},
	},
};

static struct setting_info ss_cfgs_8956[] =
{
	{
		.desc = "SS-CPU0-1-2-3",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu0-1-2-3",
			.device = "cluster0",
			.sampling_period_ms = 50,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU4",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu4",
			.device = "cluster1",
			.sampling_period_ms = 50,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU5",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "cpu5",
			.device = "cluster1",
			.sampling_period_ms = 50,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-L2_CACHE_1",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "L2_cache_1",
			.device = "cluster1",
			.sampling_period_ms = 50,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-POPMEM",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "pop_mem",
			.device = "cluster1",
			.sampling_period_ms = 250,
			.set_point = 70000,
			.set_point_clr = 55000,
			.time_constant = 2,
		},
	},
};

static struct setting_info ss_cfgs_cobalt_qrd[] =
{
	{
		.desc = "SS-POPMEM",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "pop_mem",
			.device = "cluster1",
			.sampling_period_ms = 10,
			.set_point = 95000,
			.set_point_clr = 65000,
			.time_constant = 16,
		},
	},
	{
		.desc = "SS-GPU-1",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "tsens_tz_sensor12",
			.device = "gpu",
			.sampling_period_ms = 10,
			.set_point = 95000,
			.set_point_clr = 65000,
		},
	},
	{
		.desc = "SS-GPU-2",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "tsens_tz_sensor13",
			.device = "gpu",
			.sampling_period_ms = 10,
			.set_point = 95000,
			.set_point_clr = 65000,
		},
	},
	{
		.desc = "SKIN-LOW-FLOOR",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "quiet_therm",
			.device = "cluster1",
			.sampling_period_ms = 2000,
			.set_point = 40000,
			.set_point_clr = 37000,
			.device_mtgn_max_limit = 1804800,
		},
	},
	{
		.desc = "VIRTUAL-GPU-SS",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "VIRTUAL-GPU",
			.device = "gpu",
			.sampling_period_ms = 1000,
			.set_point = 52000,
			.set_point_clr = 48000,
			.device_perf_floor = 414000,
		},
	},
};

static struct setting_info ss_cfgs_cobalt[] =
{
	{
		.desc = "SS-POPMEM",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "pop_mem",
			.device = "cluster1",
			.sampling_period_ms = 10,
			.set_point = 95000,
			.set_point_clr = 65000,
			.time_constant = 16,
		},
	},
	{
		.desc = "SS-GPU-1",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "tsens_tz_sensor12",
			.device = "gpu",
			.sampling_period_ms = 10,
			.set_point = 95000,
			.set_point_clr = 65000,
		},
	},
	{
		.desc = "SS-GPU-2",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "tsens_tz_sensor13",
			.device = "gpu",
			.sampling_period_ms = 10,
			.set_point = 95000,
			.set_point_clr = 65000,
		},
	},
};

#ifndef USE_THERMAL_FRAMEWORK
static struct setting_info ss_cfgs_8937[] =
{
	{
		.desc = "SS-CPUS",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "VIRTUAL-CPUS",
			.device = "cpu_voltage",
			.sampling_period_ms = 50,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-POPMEM",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "pop_mem",
			.device = "cpu_voltage",
			.sampling_period_ms = 250,
			.set_point = 70000,
			.set_point_clr = 55000,
			.time_constant = 2,
		},
	},
	{
		.desc = "SS-GPU",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "gpu",
			.device = "gpu",
			.sampling_period_ms = 250,
			.set_point = 95000,
			.set_point_clr = 65000,
		},
	}
};

static struct setting_info ss_cfgs_8937_qrd[] =
{
	{
		.desc = "SS-CPUS",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "VIRTUAL-CPUS",
			.device = "cpu_voltage",
			.sampling_period_ms = 50,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CASE-THERM",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "case_therm",
			.device = "cpu_voltage",
			.sampling_period_ms = 1000,
			.set_point = 44000,
			.set_point_clr = 41000,
			.time_constant = 3,
			.device_perf_floor = 1094400,
		},
	},
	{
		.desc = "SS-GPU",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "gpu",
			.device = "gpu",
			.sampling_period_ms = 250,
			.set_point = 95000,
			.set_point_clr = 65000,
		},
	},
	{
		.desc = "SS-GPU-CASE-THERM",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "case_therm",
			.device = "gpu",
			.sampling_period_ms = 10000,
			.set_point = 50000,
			.set_point_clr = 45000,
			.device_mtgn_max_limit = 375000000,
		},
	}
};

static struct setting_info ss_cfgs_8940_qrd[] =
{
	{
		.desc = "SS-CPUS",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "VIRTUAL-CPUS",
			.device = "cpu_voltage",
			.sampling_period_ms = 50,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CASE-THERM",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "case_therm",
			.device = "cpu_voltage",
			.sampling_period_ms = 1000,
			.set_point = 46000,
			.set_point_clr = 42000,
			.time_constant = 3,
			.device_perf_floor = 1094400,
		},
	},
	{
		.desc = "SS-GPU",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "gpu",
			.device = "gpu",
			.sampling_period_ms = 250,
			.set_point = 95000,
			.set_point_clr = 65000,
		},
	},
	{
		.desc = "SS-GPU-CASE-THERM",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "case_therm",
			.device = "gpu",
			.sampling_period_ms = 10000,
			.set_point = 52000,
			.set_point_clr = 45000,
			.device_mtgn_max_limit = 375000000,
		},
	}
};

static struct setting_info ss_cfgs_8917[] =
{
	{
		.desc = "SS-CPUS",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "VIRTUAL-CPUS",
			.device = "cluster1",
			.sampling_period_ms = 50,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-POPMEM",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "pop_mem",
			.device = "cluster1",
			.sampling_period_ms = 250,
			.set_point = 70000,
			.set_point_clr = 55000,
			.time_constant = 2,
			.device_mtgn_max_limit = 960000,
		},
	},
	{
		.desc = "SS-GPU",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "gpu",
			.device = "gpu",
			.sampling_period_ms = 250,
			.set_point = 95000,
			.set_point_clr = 65000,
		},
	},
};

static struct setting_info ss_cfgs_8917_qrd[] =
{
	{
		.desc = "SS-CPUS",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "VIRTUAL-CPUS",
			.device = "cluster1",
			.sampling_period_ms = 50,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CASE-THERM",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "case_therm",
			.device = "cluster1",
			.sampling_period_ms = 1000,
			.set_point = 44000,
			.set_point_clr = 41000,
			.time_constant = 3,
			.device_mtgn_max_limit = 960000,
		},
	},
	{
		.desc = "SS-GPU",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "gpu",
			.device = "gpu",
			.sampling_period_ms = 250,
			.set_point = 95000,
			.set_point_clr = 65000,
		},
	},
	{
		.desc = "SS-GPU-CASE-THERM",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "case_therm",
			.device = "gpu",
			.sampling_period_ms = 10000,
			.set_point = 50000,
			.set_point_clr = 45000,
			.device_mtgn_max_limit = 400000000,
		},
	}
};

static struct setting_info ss_cfgs_8909_qrd_lc[] =
{
	{
		.desc = "SS-CPU0-2",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "tsens_tz_sensor3",
			.device = "cpu",
			.sampling_period_ms = 65,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU1-3",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "tsens_tz_sensor4",
			.device = "cluster0",
			.sampling_period_ms = 65,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-EXT_THERM_XO",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "xo_therm",
			.device = "cluster0",
			.sampling_period_ms = 1000,
			.set_point = 53000,
			.set_point_clr = 48000,
			.time_constant = 3,
			.device_mtgn_max_limit = 533000,
		},
	},
	{
		.desc = "SS-GPU",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "tsens_tz_sensor2",
			.device = "gpu",
			.sampling_period_ms = 250,
			.set_point = 80000,
			.set_point_clr = 65000,
		},
	},
};

static struct setting_info ss_cfgs_8909_qrd_hq[] =
{
	{
		.desc = "SS-CPU0-2",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "tsens_tz_sensor3",
			.device = "cluster0",
			.sampling_period_ms = 65,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU1-3",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "tsens_tz_sensor4",
			.device = "cluster0",
			.sampling_period_ms = 65,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-EXT_THERM",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "case_therm",
			.device = "cluster0",
			.sampling_period_ms = 1000,
			.set_point = 45000,
			.set_point_clr = 40000,
			.time_constant = 3,
			.device_mtgn_max_limit = 533000,
		},
	},
	{
		.desc = "SS-GPU",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "tsens_tz_sensor2",
			.device = "gpu",
			.sampling_period_ms = 250,
			.set_point = 80000,
			.set_point_clr = 65000,
		},
	},
};

static struct setting_info ss_cfgs_8909[] =
{
	{
		.desc = "SS-CPU0-2",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "tsens_tz_sensor3",
			.device = "cluster0",
			.sampling_period_ms = 65,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU1-3",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "tsens_tz_sensor4",
			.device = "cluster0",
			.sampling_period_ms = 65,
			.set_point = 85000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-POPMEM",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "pop_mem",
			.device = "cluster0",
			.sampling_period_ms = 250,
			.set_point = 75000,
			.set_point_clr = 55000,
			.time_constant = 2,
		},
	},
	{
		.desc = "SS-GPU",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "tsens_tz_sensor2",
			.device = "gpu",
			.sampling_period_ms = 250,
			.set_point = 80000,
			.set_point_clr = 65000,
		},
	},
};

static struct setting_info ss_cfgs_8909w[] =
{
	{
		.desc = "SS-CPU0-2",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "tsens_tz_sensor3",
			.device = "cluster0",
			.sampling_period_ms = 65,
			.set_point = 60000,
			.set_point_clr = 45000,
		},
	},
	{
		.desc = "SS-CPU1-3",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "tsens_tz_sensor4",
			.device = "cluster0",
			.sampling_period_ms = 65,
			.set_point = 60000,
			.set_point_clr = 45000,
		},
	},
	{
		.desc = "SS-GPU",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "tsens_tz_sensor2",
			.device = "gpu",
			.sampling_period_ms = 250,
			.set_point = 70000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CPU_SKIN_MITIGATION",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "quiet_therm",
			.device = "cluster0",
			.sampling_period_ms = 5000,
			.set_point = 40000,
			.set_point_clr = 35000,
			.time_constant = 1,
			.device_mtgn_max_limit = 800000,
		},
	},
};
#endif

static struct setting_info ss_cfgs_9607[] =
{
	{
		.desc = "SS-CPU",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "tsens_tz_sensor4",
			.device = "cpu",
			.sampling_period_ms = 50,
			.set_point = 105000,
			.set_point_clr = 75000,
		},
	},
};

#ifndef USE_THERMAL_FRAMEWORK
static struct setting_info ss_cfgs_8953[] =
{
	{
		.desc = "SS-CPUS-ALL",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "VIRTUAL-CPUS",
			/* Since all cores are synchronous cores using
			   a single cpufreq policy, use "cluster0" device
			   for all core/L2 tsens */
			.device = "cluster0",
			.sampling_period_ms = 50,
			.set_point = 95000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-POPMEM",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "pop_mem",
			.device = "cluster0",
			.sampling_period_ms = 250,
			.set_point = 70000,
			.set_point_clr = 55000,
			.time_constant = 2,
		},
	},
	{
		.desc = "SS-GPU",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "gpu",
			.device = "gpu",
			.sampling_period_ms = 250,
			.set_point = 95000,
			.set_point_clr = 65000,
		},
	},
};

static struct setting_info ss_cfgs_8953_qrd[] =
{
	{
		.desc = "SS-CPUS-ALL",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "VIRTUAL-CPUS",
			/* Since all cores are synchronous cores using
			   a single cpufreq policy, use "cluster0" device
			   for all core/L2 tsens */
			.device = "cluster0",
			.sampling_period_ms = 50,
			.set_point = 95000,
			.set_point_clr = 55000,
		},
	},
	{
		.desc = "SS-CASE-THERM",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "case_therm",
			.device = "cluster0",
			.sampling_period_ms = 10000,
			.set_point = 43000,
			.set_point_clr = 40000,
			.device_mtgn_max_limit = 1689600,
		},
	},
	{
		.desc = "SS-GPU",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "gpu",
			.device = "gpu",
			.sampling_period_ms = 250,
			.set_point = 95000,
			.set_point_clr = 65000,
		},
	},
};
#endif

static struct setting_info ss_cfgs_sdm660[] =
{
	{
		.desc = "SS-GPU",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "gpu",
			.device = "gpu",
			.sampling_period_ms = 10,
			.set_point = 95000,
			.set_point_clr = 65000,
		},
	},
	{
		.desc = "SS-QUIET-THERM-GOLD",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "quiet_therm",
			.device = "cluster1",
			.sampling_period_ms = 2000,
			.set_point = 50000,
			.set_point_clr = 45000,
			.device_mtgn_max_limit = 1747200,
		},
	},
	{
		.desc = "SS-QUIET-THERM-SILVER",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "quiet_therm",
			.device = "cluster0",
			.sampling_period_ms = 2000,
			.set_point = 53000,
			.set_point_clr = 48000,
			.device_mtgn_max_limit = 1536000,
		},
	},
};

static struct setting_info ss_cfgs_sdm660_qrd[] =
{
	{
		.desc = "SS-GPU",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "gpu",
			.device = "gpu",
			.sampling_period_ms = 10,
			.set_point = 95000,
			.set_point_clr = 65000,
			.device_mtgn_max_limit = 588000000,
		},
	},
	{
		.desc = "SS-QUIET-THERM-GOLD",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "quiet_therm",
			.device = "cluster1",
			.sampling_period_ms = 2000,
			.set_point = 45000,
			.set_point_clr = 40000,
			.device_mtgn_max_limit = 1747200,
		},
	},
	{
		.desc = "SS-QUIET-THERM-SILVER",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "quiet_therm",
			.device = "cluster0",
			.sampling_period_ms = 2000,
			.set_point = 47000,
			.set_point_clr = 40000,
			.device_mtgn_max_limit = 1536000,
		},
	},
	{
		.desc = "SS-QUIET-THERM-GPU",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "quiet_therm",
			.device = "gpu",
			.sampling_period_ms = 2000,
			.set_point = 47000,
			.set_point_clr = 40000,
			.device_mtgn_max_limit = 588000000,
		},
	},
};

static struct setting_info ss_cfgs_sdm630[] =
{
	{
		.desc = "SS-GPU",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "gpu",
			.device = "gpu",
			.sampling_period_ms = 10,
			.set_point = 95000,
			.set_point_clr = 65000,
		},
	},
	{
		.desc = "SS-QUIET-THERM",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "quiet_therm",
			.device = "cluster1",
			.sampling_period_ms = 5000,
			.set_point = 50000,
			.set_point_clr = 45000,
			.device_mtgn_max_limit = 1401600,
		},
	},
};

static struct setting_info ss_cfgs_sdm630_qrd[] =
{
	{
		.desc = "SS-GPU",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "gpu",
			.device = "gpu",
			.sampling_period_ms = 10,
			.set_point = 95000,
			.set_point_clr = 65000,
			.device_mtgn_max_limit = 588000000,
		},
	},
	{
		.desc = "SS-QUIET-THERM-GOLD",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "quiet_therm",
			.device = "cluster1",
			.sampling_period_ms = 2000,
			.set_point = 46000,
			.set_point_clr = 41000,
			.device_mtgn_max_limit = 1670400,
		},
	},
	{
		.desc = "SS-QUIET-THERM-SILVER",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "quiet_therm",
			.device = "cluster0",
			.sampling_period_ms = 2000,
			.set_point = 50000,
			.set_point_clr = 45000,
			.device_mtgn_max_limit = 1382400,
		},
	},
	{
		.desc = "SS-QUIET-THERM-GPU",
		.algo_type = SS_ALGO_TYPE,
		.data.ss =
		{
			.sensor = "quiet_therm",
			.device = "gpu",
			.sampling_period_ms = 2000,
			.set_point = 48000,
			.set_point_clr = 41000,
			.device_mtgn_max_limit = 588000000,
		},
	},
};

void ss_init_data(struct thermal_setting_t *setting)
{
	int i, arr_size = 0;
	struct setting_info *cfg = NULL;

	switch (therm_get_msm_id()) {
		case THERM_MSM_8974:
		case THERM_MSM_8x62:
		case THERM_MSM_8974PRO_AA:
		case THERM_MSM_8974PRO_AB:
			cfg = ss_cfgs_8974;
			arr_size = ARRAY_SIZE(ss_cfgs_8974);
			break;
		case THERM_MSM_8974PRO_AC:
		case THERM_MSM_8084:
			cfg = ss_cfgs_8974_pro_ac;
			arr_size = ARRAY_SIZE(ss_cfgs_8974_pro_ac);
			break;
		case THERM_MSM_8226:
			switch (therm_get_msm_version()) {
				case THERM_VERSION_V1:
					cfg = ss_cfgs_8226_v1;
					arr_size = ARRAY_SIZE(ss_cfgs_8226_v1);
					break;
				default:
					cfg = ss_cfgs_8226;
					arr_size = ARRAY_SIZE(ss_cfgs_8226);
					break;
			}
			break;
		case THERM_MSM_8926:
			cfg = ss_cfgs_8226;
			arr_size = ARRAY_SIZE(ss_cfgs_8226);
			break;
		case THERM_MSM_8916:
			cfg = ss_cfgs_8916;
			arr_size = ARRAY_SIZE(ss_cfgs_8916);
			break;
		case THERM_MSM_8610:
			cfg = ss_cfgs_8610;
			arr_size = ARRAY_SIZE(ss_cfgs_8610);
			break;
		case THERM_FSM_9900:
			cfg = ss_cfgs_9900;
			arr_size = ARRAY_SIZE(ss_cfgs_9900);
			break;
		case THERM_MSM_8939:
		case THERM_MSM_8929:
			cfg = ss_cfgs_8939;
			arr_size = ARRAY_SIZE(ss_cfgs_8939);
			break;
		case THERM_MSM_8936:
			cfg = ss_cfgs_8936;
			arr_size = ARRAY_SIZE(ss_cfgs_8936);
			break;
		case THERM_MSM_8994:
			cfg = ss_cfgs_8994;
			arr_size = ARRAY_SIZE(ss_cfgs_8994);
			break;
		case THERM_MSM_8909:
#ifndef USE_THERMAL_FRAMEWORK
			switch (therm_get_hw_platform()) {
				case THERM_PLATFORM_QRD:
					if (therm_get_hw_platform_subtype() ==
						THERM_PLATFORM_SUB_QRD_HUAQIN) {
						cfg = ss_cfgs_8909_qrd_hq;
						arr_size =
						ARRAY_SIZE(ss_cfgs_8909_qrd_hq);
					} else {
						cfg = ss_cfgs_8909_qrd_lc;
						arr_size =
						ARRAY_SIZE(ss_cfgs_8909_qrd_lc);
					}
					break;
				default:
					cfg = ss_cfgs_8909;
					arr_size = ARRAY_SIZE(ss_cfgs_8909);
					break;
			}
#endif
			break;
		case THERM_MSM_8909W:
#ifndef USE_THERMAL_FRAMEWORK
			if (therm_get_hw_platform_subtype() ==
				THERM_PLATFORM_SUB_WEARABLE_COMPAL) {
				cfg = ss_cfgs_8909w;
				arr_size = ARRAY_SIZE(ss_cfgs_8909w);
			} else {
				cfg = ss_cfgs_8909;
				arr_size = ARRAY_SIZE(ss_cfgs_8909);
			}
#endif
			break;
		case THERM_MSM_8009:
#ifndef USE_THERMAL_FRAMEWORK
			cfg = ss_cfgs_8909;
			arr_size = ARRAY_SIZE(ss_cfgs_8909);
#endif
			break;
		case THERM_MSM_8992:
			cfg = ss_cfgs_8992;
			arr_size = ARRAY_SIZE(ss_cfgs_8992);
			break;
		case THERM_MSM_8996AU:
			cfg = ss_cfgs_8996AU;
			arr_size = ARRAY_SIZE(ss_cfgs_8996AU);
			break;
		case THERM_MSM_8996:
			cfg = ss_cfgs_8996;
			arr_size = ARRAY_SIZE(ss_cfgs_8996);
			break;
		case THERM_MSM_8952:
			cfg = ss_cfgs_8952;
			arr_size = ARRAY_SIZE(ss_cfgs_8952);
			break;
		case THERM_MSM_8976:
			cfg = ss_cfgs_8976;
			arr_size = ARRAY_SIZE(ss_cfgs_8976);
			break;
		case THERM_MSM_8956:
			cfg = ss_cfgs_8956;
			arr_size = ARRAY_SIZE(ss_cfgs_8956);
			break;
		case THERM_MSM_COBALT:
			switch (therm_get_hw_platform()) {
			case THERM_PLATFORM_MTP:
				cfg = ss_cfgs_cobalt;
				arr_size = ARRAY_SIZE(ss_cfgs_cobalt);
				break;
			default:
				cfg = ss_cfgs_cobalt_qrd;
				arr_size = ARRAY_SIZE(ss_cfgs_cobalt_qrd);
				break;
			}
			break;
		case THERM_MSM_8937:
#ifndef USE_THERMAL_FRAMEWORK
			switch (therm_get_hw_platform()) {
			case THERM_PLATFORM_MTP:
				cfg = ss_cfgs_8937;
				arr_size =
				ARRAY_SIZE(ss_cfgs_8937);
				break;
			default:
				cfg = ss_cfgs_8937_qrd;
				arr_size =
				ARRAY_SIZE(ss_cfgs_8937_qrd);
				break;
			}
#endif
			break;
		case THERM_MSM_8940:
#ifndef USE_THERMAL_FRAMEWORK
			switch (therm_get_hw_platform()) {
			case THERM_PLATFORM_MTP:
				cfg = ss_cfgs_8937;
				arr_size =
				ARRAY_SIZE(ss_cfgs_8937);
				break;
			default:
				cfg = ss_cfgs_8940_qrd;
				arr_size =
				ARRAY_SIZE(ss_cfgs_8940_qrd);
				break;
			}
#endif
			break;
		case THERM_MDM_9607:
			cfg = ss_cfgs_9607;
			arr_size = ARRAY_SIZE(ss_cfgs_9607);
			break;
		case THERM_MSM_8953:
#ifndef USE_THERMAL_FRAMEWORK
			switch (therm_get_hw_platform()) {
			case THERM_PLATFORM_MTP:
				cfg = ss_cfgs_8953;
				arr_size = ARRAY_SIZE(ss_cfgs_8953);
				break;
			default:
				cfg = ss_cfgs_8953_qrd;
				arr_size = ARRAY_SIZE(ss_cfgs_8953_qrd);
				break;
			}
#endif
			break;
		case THERM_MSM_8917:
		case THERM_MSM_8920:
#ifndef USE_THERMAL_FRAMEWORK
			switch (therm_get_hw_platform()) {
			case THERM_PLATFORM_MTP:
				cfg = ss_cfgs_8917;
				arr_size =
					ARRAY_SIZE(ss_cfgs_8917);
				break;
			default:
				cfg = ss_cfgs_8917_qrd;
				arr_size =
					ARRAY_SIZE(ss_cfgs_8917_qrd);
				break;
			}
#endif
			break;
		case THERM_SDM_660:
			switch (therm_get_hw_platform()) {
			case THERM_PLATFORM_MTP:
				cfg = ss_cfgs_sdm660;
				arr_size =
					ARRAY_SIZE(ss_cfgs_sdm660);
				break;
			default:
				cfg = ss_cfgs_sdm660_qrd;
				arr_size =
					ARRAY_SIZE(ss_cfgs_sdm660_qrd);
				break;
			}
			break;
		case THERM_SDM_630:
			switch (therm_get_hw_platform()) {
			case THERM_PLATFORM_MTP:
				cfg = ss_cfgs_sdm630;
				arr_size =
					ARRAY_SIZE(ss_cfgs_sdm630);
				break;
			default:
				cfg = ss_cfgs_sdm630_qrd;
				arr_size =
					ARRAY_SIZE(ss_cfgs_sdm630_qrd);
				break;
			}
			break;
		default:
			msg("%s: ERROR Uknown device\n", __func__);
			/* Hopefully kernel governor's are enabled */
			return;
	}

	for (i = 0; i < arr_size; i++)
		add_setting(setting, &cfg[i]);
}
