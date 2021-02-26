/*===========================================================================

Copyright (c) 2012-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/

#include "thermal.h"
#include "thermal_config.h"

#define HOTPLUG_HIGH_THRESHOLD (100000)
#define HOTPLUG_LOW_THRESHOLD  (HOTPLUG_HIGH_THRESHOLD - 20000)
#define HOTPLUG_HIGH_THRESHOLD_8064 (115000)
#define HOTPLUG_LOW_THRESHOLD_8064  (HOTPLUG_HIGH_THRESHOLD_8064 - 20000)
#define HOTPLUG_HIGH_THRESHOLD_8930 (110000)
#define HOTPLUG_LOW_THRESHOLD_8930  (HOTPLUG_HIGH_THRESHOLD_8930 - 20000)
#define HOTPLUG_HIGH_THRESHOLD_8992 (90000)
#define HOTPLUG_LOW_THRESHOLD_8992  (HOTPLUG_HIGH_THRESHOLD_8992 - 15000)

#define HYBRID_POLLING_THRESH 65000
#define HYBRID_POLLING_THRESH_CLR 62000

#define HYBRID_POLLING_THRESH_8064 70000
#define HYBRID_POLLING_THRESH_CLR_8064 67000

#define VDD_RESTRICTION_THRESH 5000
#define VDD_RESTRICTION_THRESH_CLR 10000

#define VDD_MIN_THRESH 17500
#define VDD_MIN_THRESH_CLR 20000

#define OPT_CURRENT_REQ_THRESH 85000
#define OPT_CURRENT_REQ_THRESH_CLR 75000

#define VDD_DIG_AUTOMODE_DISABLE_THRESH 85000
#define VDD_DIG_AUTOMODE_DISABLE_THRESH_CLR 75000

/* PMIC 8821 alarms */
/* State 1 Alarm thresholds */
#define PMIC_ALARM_STAGE_1_TEMP_TRIG  107000
#define PMIC_ALARM_STAGE_1_TEMP_CLR  103000

/* Stage 2 Alarm thresholds */
#define PMIC_ALARM_STAGE_2_TEMP_TRIG  127000
#define PMIC_ALARM_STAGE_2_TEMP_CLR  123000


static struct setting_info tm_cfgs_8064[] =
{
	{
		.desc = "HOTPLUG-CPU1",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "cpu1",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = HOTPLUG_HIGH_THRESHOLD_8064,
				.lvl_clr = HOTPLUG_LOW_THRESHOLD_8064,
				.num_actions = 1,
				.actions[0] = {
					.device = "hotplug_1",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "HOTPLUG-CPU2",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "cpu2",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = HOTPLUG_HIGH_THRESHOLD_8064,
				.lvl_clr = HOTPLUG_LOW_THRESHOLD_8064,
				.num_actions = 1,
				.actions[0] = {
					.device = "hotplug_2",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "HOTPLUG-CPU3",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "cpu3",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = HOTPLUG_HIGH_THRESHOLD_8064,
				.lvl_clr = HOTPLUG_LOW_THRESHOLD_8064,
				.num_actions = 1,
				.actions[0] = {
					.device = "hotplug_3",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "HYBRID-THRESH",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor0",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = HYBRID_POLLING_THRESH_8064,
				.lvl_clr = HYBRID_POLLING_THRESH_CLR_8064,
				.num_actions = 1,
				.actions[0] = {
					.device = "none",
					.info = 0,
				},
			},
		},
	},
	{
		.desc = "HYBRID-CPU0-POLLING",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "cpu0",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = HYBRID_POLLING_THRESH_8064,
				.lvl_clr = HYBRID_POLLING_THRESH_CLR_8064,
				.num_actions = 1,
				.actions[0] = {
					.device = "none",
					.info = 0,
				},
			},
		},
	},
	{
		.desc = "HYBRID-CPU1-POLLING",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "cpu1",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = HYBRID_POLLING_THRESH_8064,
				.lvl_clr = HYBRID_POLLING_THRESH_CLR_8064,
				.num_actions = 1,
				.actions[0] = {
					.device = "none",
					.info = 0,
				},
			},
		},
	},
	{
		.desc = "HYBRID-CPU2-POLLING",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "cpu2",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = HYBRID_POLLING_THRESH_8064,
				.lvl_clr = HYBRID_POLLING_THRESH_CLR_8064,
				.num_actions = 1,
				.actions[0] = {
					.device = "none",
					.info = 0,
				},
			},
		},
	},
	{
		.desc = "HYBRID-CPU3-POLLING",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "cpu3",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = HYBRID_POLLING_THRESH_8064,
				.lvl_clr = HYBRID_POLLING_THRESH_CLR_8064,
				.num_actions = 1,
				.actions[0] = {
					.device = "none",
					.info = 0,
				},
			},
		},
	},
	{
		.desc = "HYBRID-POPMEM-POLLING",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "pop_mem",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = HYBRID_POLLING_THRESH_8064,
				.lvl_clr = HYBRID_POLLING_THRESH_CLR_8064,
				.num_actions = 1,
				.actions[0] = {
					.device = "none",
					.info = 0,
				},
			},
		},
	},
	{
		.desc = "PM8821-MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "pm8821_tz",
			.sampling_period_ms = 1000,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = PMIC_ALARM_STAGE_1_TEMP_TRIG,
				.lvl_clr = PMIC_ALARM_STAGE_1_TEMP_CLR,
				.num_actions = 4,
				.actions = {
					[0] = {
						.device = "hotplug_2",
						.info = 0,
					},
					[1] = {
						.device = "hotplug_3",
						.info = 0,
					},
					[2] = {
						.device = "cpu2",
						.info = 918000,
					},
					[3] = {
						.device = "cpu3",
						.info = 918000,
					},
				},
			},
			.t[1] = {
				.lvl_trig = PMIC_ALARM_STAGE_2_TEMP_TRIG,
				.lvl_clr = PMIC_ALARM_STAGE_2_TEMP_CLR,
				.num_actions = 2,
				.actions = {
					[0] = {
						.device = "hotplug_2",
						.info = 1,
					},
					[1] = {
						.device = "hotplug_3",
						.info = 1,
					},
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_8960[] =
{
	{
		.desc = "HOTPLUG-CPU1",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "cpu1",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = HOTPLUG_HIGH_THRESHOLD,
				.lvl_clr = HOTPLUG_LOW_THRESHOLD,
				.num_actions = 1,
				.actions[0] = {
					.device = "hotplug_1",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "HYBRID-THRESH",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor0",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = HYBRID_POLLING_THRESH,
				.lvl_clr = HYBRID_POLLING_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "none",
					.info = 0,
				},
			},
		},
	},
	{
		.desc = "HYBRID-CPU0-POLLING",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "cpu0",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = HYBRID_POLLING_THRESH,
				.lvl_clr = HYBRID_POLLING_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "none",
					.info = 0,
				},
			},
		},
	},
	{
		.desc = "HYBRID-CPU1-POLLING",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "cpu1",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = HYBRID_POLLING_THRESH,
				.lvl_clr = HYBRID_POLLING_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "none",
					.info = 0,
				},
			},
		},
	},
	{
		.desc = "HYBRID-POPMEM-POLLING",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "pop_mem",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = HYBRID_POLLING_THRESH,
				.lvl_clr = HYBRID_POLLING_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "none",
					.info = 0,
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_8930[] =
{
	{
		.desc = "HOTPLUG-CPU1",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "cpu1",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = HOTPLUG_HIGH_THRESHOLD_8930,
				.lvl_clr = HOTPLUG_LOW_THRESHOLD_8930,
				.num_actions = 1,
				.actions[0] = {
					.device = "hotplug_1",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "HYBRID-THRESH",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor0",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = HYBRID_POLLING_THRESH,
				.lvl_clr = HYBRID_POLLING_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "none",
					.info = 0,
				},
			},
		},
	},
	{
		.desc = "HYBRID-CPU0-POLLING",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "cpu0",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = HYBRID_POLLING_THRESH,
				.lvl_clr = HYBRID_POLLING_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "none",
					.info = 0,
				},
			},
		},
	},
	{
		.desc = "HYBRID-CPU1-POLLING",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "cpu1",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = HYBRID_POLLING_THRESH,
				.lvl_clr = HYBRID_POLLING_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "none",
					.info = 0,
				},
			},
		},
	},
	{
		.desc = "HYBRID-POPMEM-POLLING",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "pop_mem",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = HYBRID_POLLING_THRESH,
				.lvl_clr = HYBRID_POLLING_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "none",
					.info = 0,
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_8974[] =
{
	{
		.desc = "VDD_RSTR_MONITOR-TSENS0",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor0",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS1",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor1",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS2",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor2",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS3",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor3",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS4",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor4",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS5",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor5",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS6",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor6",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS7",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor7",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS8",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor8",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS9",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor9",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS10",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor10",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_DIG_SWMODE_MONITOR-TSENS0",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor0",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_DIG_AUTOMODE_DISABLE_THRESH,
				.lvl_clr = VDD_DIG_AUTOMODE_DISABLE_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_dig_swmode",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_DIG_SWMODE_MONITOR-TSENS1",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor1",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_DIG_AUTOMODE_DISABLE_THRESH,
				.lvl_clr = VDD_DIG_AUTOMODE_DISABLE_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_dig_swmode",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_DIG_SWMODE_MONITOR-TSENS2",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor2",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_DIG_AUTOMODE_DISABLE_THRESH,
				.lvl_clr = VDD_DIG_AUTOMODE_DISABLE_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_dig_swmode",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_DIG_SWMODE_MONITOR-TSENS3",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor3",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_DIG_AUTOMODE_DISABLE_THRESH,
				.lvl_clr = VDD_DIG_AUTOMODE_DISABLE_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_dig_swmode",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_DIG_SWMODE_MONITOR-TSENS4",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor4",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_DIG_AUTOMODE_DISABLE_THRESH,
				.lvl_clr = VDD_DIG_AUTOMODE_DISABLE_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_dig_swmode",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_DIG_SWMODE_MONITOR-TSENS5",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor5",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_DIG_AUTOMODE_DISABLE_THRESH,
				.lvl_clr = VDD_DIG_AUTOMODE_DISABLE_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_dig_swmode",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_DIG_SWMODE_MONITOR-TSENS6",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor6",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_DIG_AUTOMODE_DISABLE_THRESH,
				.lvl_clr = VDD_DIG_AUTOMODE_DISABLE_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_dig_swmode",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_DIG_SWMODE_MONITOR-TSENS7",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor7",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_DIG_AUTOMODE_DISABLE_THRESH,
				.lvl_clr = VDD_DIG_AUTOMODE_DISABLE_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_dig_swmode",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_DIG_SWMODE_MONITOR-TSENS8",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor8",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_DIG_AUTOMODE_DISABLE_THRESH,
				.lvl_clr = VDD_DIG_AUTOMODE_DISABLE_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_dig_swmode",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_DIG_SWMODE_MONITOR-TSENS9",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor9",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_DIG_AUTOMODE_DISABLE_THRESH,
				.lvl_clr = VDD_DIG_AUTOMODE_DISABLE_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_dig_swmode",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_DIG_SWMODE_MONITOR-TSENS10",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor10",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_DIG_AUTOMODE_DISABLE_THRESH,
				.lvl_clr = VDD_DIG_AUTOMODE_DISABLE_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_dig_swmode",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "PMIC-ALARM-MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "pm8841_tz",
			.sampling_period_ms = 1000,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = PMIC_ALARM_STAGE_1_TEMP_TRIG,
				.lvl_clr = PMIC_ALARM_STAGE_1_TEMP_CLR,
				.num_actions = 7,
				.actions = {
					[0] = {
						.device = "hotplug_1",
						.info = 0,
					},
					[1] = {
						.device = "hotplug_2",
						.info = 0,
					},
					[2] = {
						.device = "hotplug_3",
						.info = 0,
					},
					[3] = {
						.device = "cpu0",
						.info = 960000,
					},
					[4] = {
						.device = "cpu1",
						.info = 960000,
					},
					[5] = {
						.device = "cpu2",
						.info = 960000,
					},
					[6] = {
						.device = "cpu3",
						.info = 960000,
					},
				},
			},
			.t[1] = {
				.lvl_trig = PMIC_ALARM_STAGE_2_TEMP_TRIG,
				.lvl_clr = PMIC_ALARM_STAGE_2_TEMP_CLR,
				.num_actions = 3,
				.actions = {
					[0] = {
						.device = "hotplug_1",
						.info = 1,
					},
					[1] = {
						.device = "hotplug_2",
						.info = 1,
					},
					[2] = {
						.device = "hotplug_3",
						.info = 1,
					},
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_8974_pro_ab[] =
{
	{
		.desc = "VDD_RSTR_MONITOR-TSENS0",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor0",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS1",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor1",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS2",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor2",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS3",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor3",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS4",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor4",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS5",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor5",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS6",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor6",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS7",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor7",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS8",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor8",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS9",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor9",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS10",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor10",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "PMIC-ALARM-MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "pm8841_tz",
			.sampling_period_ms = 1000,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = PMIC_ALARM_STAGE_1_TEMP_TRIG,
				.lvl_clr = PMIC_ALARM_STAGE_1_TEMP_CLR,
				.num_actions = 7,
				.actions = {
					[0] = {
						.device = "hotplug_1",
						.info = 0,
					},
					[1] = {
						.device = "hotplug_2",
						.info = 0,
					},
					[2] = {
						.device = "hotplug_3",
						.info = 0,
					},
					[3] = {
						.device = "cpu0",
						.info = 960000,
					},
					[4] = {
						.device = "cpu1",
						.info = 960000,
					},
					[5] = {
						.device = "cpu2",
						.info = 960000,
					},
					[6] = {
						.device = "cpu3",
						.info = 960000,
					},
				},
			},
			.t[1] = {
				.lvl_trig = PMIC_ALARM_STAGE_2_TEMP_TRIG,
				.lvl_clr = PMIC_ALARM_STAGE_2_TEMP_CLR,
				.num_actions = 3,
				.actions = {
					[0] = {
						.device = "hotplug_1",
						.info = 1,
					},
					[1] = {
						.device = "hotplug_2",
						.info = 1,
					},
					[2] = {
						.device = "hotplug_3",
						.info = 1,
					},
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_8084[] =
{
	{
		.desc = "VDD_RSTR_MONITOR-TSENS0",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor0",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS1",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor1",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS2",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor2",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS3",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor3",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS4",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor4",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS5",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor5",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS6",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor6",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS7",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor7",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS8",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor8",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS9",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor9",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS10",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor10",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "PMIC-ALARM-MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "pma8084_tz",
			.sampling_period_ms = 1000,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = PMIC_ALARM_STAGE_1_TEMP_TRIG,
				.lvl_clr = PMIC_ALARM_STAGE_1_TEMP_CLR,
				.num_actions = 7,
				.actions = {
					[0] = {
						.device = "hotplug_1",
						.info = 0,
					},
					[1] = {
						.device = "hotplug_2",
						.info = 0,
					},
					[2] = {
						.device = "hotplug_3",
						.info = 0,
					},
					[3] = {
						.device = "cpu0",
						.info = 960000,
					},
					[4] = {
						.device = "cpu1",
						.info = 960000,
					},
					[5] = {
						.device = "cpu2",
						.info = 960000,
					},
					[6] = {
						.device = "cpu3",
						.info = 960000,
					},
				},
			},
			.t[1] = {
				.lvl_trig = PMIC_ALARM_STAGE_2_TEMP_TRIG,
				.lvl_clr = PMIC_ALARM_STAGE_2_TEMP_CLR,
				.num_actions = 3,
				.actions = {
					[0] = {
						.device = "hotplug_1",
						.info = 1,
					},
					[1] = {
						.device = "hotplug_2",
						.info = 1,
					},
					[2] = {
						.device = "hotplug_3",
						.info = 1,
					},
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_8092[] =
{
	{
		.desc = "PMIC-ALARM-MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "pma8084_tz",
			.sampling_period_ms = 1000,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = PMIC_ALARM_STAGE_1_TEMP_TRIG,
				.lvl_clr = PMIC_ALARM_STAGE_1_TEMP_CLR,
				.num_actions = 7,
				.actions = {
					[0] = {
						.device = "hotplug_1",
						.info = 0,
					},
					[1] = {
						.device = "hotplug_2",
						.info = 0,
					},
					[2] = {
						.device = "hotplug_3",
						.info = 0,
					},
					[3] = {
						.device = "cpu0",
						.info = 960000,
					},
					[4] = {
						.device = "cpu1",
						.info = 960000,
					},
					[5] = {
						.device = "cpu2",
						.info = 960000,
					},
					[6] = {
						.device = "cpu3",
						.info = 960000,
					},
				},
			},
			.t[1] = {
				.lvl_trig = PMIC_ALARM_STAGE_2_TEMP_TRIG,
				.lvl_clr = PMIC_ALARM_STAGE_2_TEMP_CLR,
				.num_actions = 3,
				.actions = {
					[0] = {
						.device = "hotplug_1",
						.info = 1,
					},
					[1] = {
						.device = "hotplug_2",
						.info = 1,
					},
					[2] = {
						.device = "hotplug_3",
						.info = 1,
					},
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_8974_pro_ac[] =
{
	{
		.desc = "VDD_RSTR_MONITOR-TSENS0",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor0",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS1",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor1",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS2",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor2",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS3",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor3",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS4",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor4",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS5",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor5",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS6",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor6",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS7",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor7",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS8",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor8",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS9",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor9",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS10",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor10",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "PMIC-ALARM-MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "pma8084_tz",
			.sampling_period_ms = 1000,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = PMIC_ALARM_STAGE_1_TEMP_TRIG,
				.lvl_clr = PMIC_ALARM_STAGE_1_TEMP_CLR,
				.num_actions = 7,
				.actions = {
					[0] = {
						.device = "hotplug_1",
						.info = 0,
					},
					[1] = {
						.device = "hotplug_2",
						.info = 0,
					},
					[2] = {
						.device = "hotplug_3",
						.info = 0,
					},
					[3] = {
						.device = "cpu0",
						.info = 960000,
					},
					[4] = {
						.device = "cpu1",
						.info = 960000,
					},
					[5] = {
						.device = "cpu2",
						.info = 960000,
					},
					[6] = {
						.device = "cpu3",
						.info = 960000,
					},
				},
			},
			.t[1] = {
				.lvl_trig = PMIC_ALARM_STAGE_2_TEMP_TRIG,
				.lvl_clr = PMIC_ALARM_STAGE_2_TEMP_CLR,
				.num_actions = 3,
				.actions = {
					[0] = {
						.device = "hotplug_1",
						.info = 1,
					},
					[1] = {
						.device = "hotplug_2",
						.info = 1,
					},
					[2] = {
						.device = "hotplug_3",
						.info = 1,
					},
				},
			},
		},
	},
	{
		.desc = "OPT_CURR_MONITOR-TSENS0",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor0",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = OPT_CURRENT_REQ_THRESH,
				.lvl_clr = OPT_CURRENT_REQ_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "opt_curr_req",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "OPT_CURR_MONITOR-TSENS1",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor1",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = OPT_CURRENT_REQ_THRESH,
				.lvl_clr = OPT_CURRENT_REQ_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "opt_curr_req",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "OPT_CURR_MONITOR-TSENS2",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor2",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = OPT_CURRENT_REQ_THRESH,
				.lvl_clr = OPT_CURRENT_REQ_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "opt_curr_req",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "OPT_CURR_MONITOR-TSENS3",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor3",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = OPT_CURRENT_REQ_THRESH,
				.lvl_clr = OPT_CURRENT_REQ_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "opt_curr_req",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "OPT_CURR_MONITOR-TSENS4",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor4",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = OPT_CURRENT_REQ_THRESH,
				.lvl_clr = OPT_CURRENT_REQ_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "opt_curr_req",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "OPT_CURR_MONITOR-TSENS5",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor5",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = OPT_CURRENT_REQ_THRESH,
				.lvl_clr = OPT_CURRENT_REQ_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "opt_curr_req",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "OPT_CURR_MONITOR-TSENS6",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor6",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = OPT_CURRENT_REQ_THRESH,
				.lvl_clr = OPT_CURRENT_REQ_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "opt_curr_req",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "OPT_CURR_MONITOR-TSENS7",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor7",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = OPT_CURRENT_REQ_THRESH,
				.lvl_clr = OPT_CURRENT_REQ_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "opt_curr_req",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "OPT_CURR_MONITOR-TSENS8",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor8",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = OPT_CURRENT_REQ_THRESH,
				.lvl_clr = OPT_CURRENT_REQ_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "opt_curr_req",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "OPT_CURR_MONITOR-TSENS9",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor9",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = OPT_CURRENT_REQ_THRESH,
				.lvl_clr = OPT_CURRENT_REQ_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "opt_curr_req",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "OPT_CURR_MONITOR-TSENS10",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor10",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = OPT_CURRENT_REQ_THRESH,
				.lvl_clr = OPT_CURRENT_REQ_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "opt_curr_req",
					.info = 1,
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_8x62[] =
{
	{
		.desc = "VDD_RSTR_MONITOR-TSENS0",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor0",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS1",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor1",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS2",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor2",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS3",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor3",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS4",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor4",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS5",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor5",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS6",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor6",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS7",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor7",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS8",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor8",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS9",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor9",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS10",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor10",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_DIG_SWMODE_MONITOR-TSENS0",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor0",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_DIG_AUTOMODE_DISABLE_THRESH,
				.lvl_clr = VDD_DIG_AUTOMODE_DISABLE_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_dig_swmode",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_DIG_SWMODE_MONITOR-TSENS1",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor1",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_DIG_AUTOMODE_DISABLE_THRESH,
				.lvl_clr = VDD_DIG_AUTOMODE_DISABLE_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_dig_swmode",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_DIG_SWMODE_MONITOR-TSENS2",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor2",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_DIG_AUTOMODE_DISABLE_THRESH,
				.lvl_clr = VDD_DIG_AUTOMODE_DISABLE_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_dig_swmode",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_DIG_SWMODE_MONITOR-TSENS3",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor3",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_DIG_AUTOMODE_DISABLE_THRESH,
				.lvl_clr = VDD_DIG_AUTOMODE_DISABLE_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_dig_swmode",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_DIG_SWMODE_MONITOR-TSENS4",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor4",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_DIG_AUTOMODE_DISABLE_THRESH,
				.lvl_clr = VDD_DIG_AUTOMODE_DISABLE_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_dig_swmode",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_DIG_SWMODE_MONITOR-TSENS5",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor5",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_DIG_AUTOMODE_DISABLE_THRESH,
				.lvl_clr = VDD_DIG_AUTOMODE_DISABLE_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_dig_swmode",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_DIG_SWMODE_MONITOR-TSENS6",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor6",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_DIG_AUTOMODE_DISABLE_THRESH,
				.lvl_clr = VDD_DIG_AUTOMODE_DISABLE_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_dig_swmode",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_DIG_SWMODE_MONITOR-TSENS7",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor7",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_DIG_AUTOMODE_DISABLE_THRESH,
				.lvl_clr = VDD_DIG_AUTOMODE_DISABLE_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_dig_swmode",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_DIG_SWMODE_MONITOR-TSENS8",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor8",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_DIG_AUTOMODE_DISABLE_THRESH,
				.lvl_clr = VDD_DIG_AUTOMODE_DISABLE_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_dig_swmode",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_DIG_SWMODE_MONITOR-TSENS9",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor9",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_DIG_AUTOMODE_DISABLE_THRESH,
				.lvl_clr = VDD_DIG_AUTOMODE_DISABLE_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_dig_swmode",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_DIG_SWMODE_MONITOR-TSENS10",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor10",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_DIG_AUTOMODE_DISABLE_THRESH,
				.lvl_clr = VDD_DIG_AUTOMODE_DISABLE_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_dig_swmode",
					.info = 1,
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_8226_v1[] =
{
	{
		.desc = "VDD_RSTR_MONITOR-TSENS0",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor0",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS1",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor1",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS2",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor2",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS3",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor3",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	/* Disable TSENS4/TSENS5 for vdd min monitor for bring up because
	 * it always returns 0 */
	/*
	{
		.desc = "VDD_RSTR_MONITOR-TSENS4",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor4",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS5",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor5",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	*/
};

static struct setting_info tm_cfgs_8226_v2[] =
{
	{
		.desc = "VDD_RSTR_MONITOR-TSENS0",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor0",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS1",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor1",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS2",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor2",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS3",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor3",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS4",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor4",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS5",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor5",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_8610[] =
{
	{
		.desc = "VDD_RSTR_MONITOR-TSENS0",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor0",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS5",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor5",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_9625[] =
{
	{
		.desc = "VDD_RSTR_MONITOR-TSENS0",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor0",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS1",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor1",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS2",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor2",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS3",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor3",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS4",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor4",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_9645[] =
{
	{
		.desc = "VDD_RSTR_MONITOR-TSENS0",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor0",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS1",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor1",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS2",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor2",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS4",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor4",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS5",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor5",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "MSS_TM",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor2",
			.sampling_period_ms = 300,
			.num_thresholds = 3,
			._n_thresholds = 3,
			._n_to_clear = 3,
			._n_actions = 3,
			._n_action_info = 3,
			.t[0] = {
				.lvl_trig = 95000,
				.lvl_clr = 80000,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "modem_proc",
						.info = 1,
					},
				},
			},
			.t[1] = {
				.lvl_trig = 105000,
				.lvl_clr = 90000,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "modem_proc",
						.info = 2,
					},
				},
			},
			.t[2] = {
				.lvl_trig = 115000,
				.lvl_clr = 90000,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "modem_proc",
						.info = 3,
					},
				},
			},
		},
	},
	{
		.desc = "CPR_BAND_MONITOR-TSENS0",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor0",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_MIN_THRESH,
				.lvl_clr = VDD_MIN_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "cpr_band",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "CPR_BAND_MONITOR-TSENS1",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor1",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_MIN_THRESH,
				.lvl_clr = VDD_MIN_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "cpr_band",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "CPR_BAND_MONITOR-TSENS2",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor2",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_MIN_THRESH,
				.lvl_clr = VDD_MIN_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "cpr_band",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "CPR_BAND_MONITOR-TSENS4",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor4",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_MIN_THRESH,
				.lvl_clr = VDD_MIN_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "cpr_band",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "CPR_BAND_MONITOR-TSENS5",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor5",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_MIN_THRESH,
				.lvl_clr = VDD_MIN_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "cpr_band",
					.info = 1,
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_9900[] =
{
	{
		.desc = "PMIC-ALARM-MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "pma8084_tz",
			.sampling_period_ms = 1000,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = PMIC_ALARM_STAGE_1_TEMP_TRIG,
				.lvl_clr = PMIC_ALARM_STAGE_1_TEMP_CLR,
				.num_actions = 7,
				.actions = {
					[0] = {
						.device = "hotplug_1",
						.info = 0,
					},
					[1] = {
						.device = "hotplug_2",
						.info = 0,
					},
					[2] = {
						.device = "hotplug_3",
						.info = 0,
					},
					[3] = {
						.device = "cpu0",
						.info = 960000,
					},
					[4] = {
						.device = "cpu1",
						.info = 960000,
					},
					[5] = {
						.device = "cpu2",
						.info = 960000,
					},
					[6] = {
						.device = "cpu3",
						.info = 960000,
					},
				},
			},
			.t[1] = {
				.lvl_trig = PMIC_ALARM_STAGE_2_TEMP_TRIG,
				.lvl_clr = PMIC_ALARM_STAGE_2_TEMP_CLR,
				.num_actions = 3,
				.actions = {
					[0] = {
						.device = "hotplug_1",
						.info = 1,
					},
					[1] = {
						.device = "hotplug_2",
						.info = 1,
					},
					[2] = {
						.device = "hotplug_3",
						.info = 1,
					},
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_8994[] =
{
	{
		.desc = "VDD_RSTR_MONITOR-TSENS0",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor0",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS1",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor1",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS2",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor2",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS3",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor3",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS4",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor4",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS5",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor5",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS6",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor6",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS7",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor7",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS8",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor8",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS9",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor9",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS10",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor10",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS11",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor11",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS12",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor12",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS13",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor13",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS14",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor14",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS15",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor15",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "PMIC-ALARM-MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "pm8994_tz",
			.sampling_period_ms = 1000,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = PMIC_ALARM_STAGE_1_TEMP_TRIG,
				.lvl_clr = PMIC_ALARM_STAGE_1_TEMP_CLR,
				.num_actions = 9,
				.actions = {
					[0] = {
						.device = "hotplug_7",
						.info = 0,
					},
					[1] = {
						.device = "hotplug_6",
						.info = 0,
					},
					[2] = {
						.device = "hotplug_5",
						.info = 0,
					},
					[3] = {
						.device = "hotplug_4",
						.info = 0,
					},
					[4] = {
						.device = "hotplug_3",
						.info = 0,
					},
					[5] = {
						.device = "hotplug_2",
						.info = 0,
					},
					[6] = {
						.device = "hotplug_1",
						.info = 0,
					},
					[7] = {
						.device = "cluster1",
						.info = 302400,
					},
					[8] = {
						.device = "cluster0",
						.info = 302400,
					},
				},
			},
			.t[1] = {
				.lvl_trig = PMIC_ALARM_STAGE_2_TEMP_TRIG,
				.lvl_clr = PMIC_ALARM_STAGE_2_TEMP_CLR,
				.num_actions = 7,
				.actions = {
					[0] = {
						.device = "hotplug_7",
						.info = 1,
					},
					[1] = {
						.device = "hotplug_6",
						.info = 1,
					},
					[2] = {
						.device = "hotplug_5",
						.info = 1,
					},
					[3] = {
						.device = "hotplug_4",
						.info = 1,
					},
					[4] = {
						.device = "hotplug_3",
						.info = 1,
					},
					[5] = {
						.device = "hotplug_2",
						.info = 1,
					},
					[6] = {
						.device = "hotplug_1",
						.info = 1,
					},
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_8992[] =
{
	{
		.desc = "HOTPLUG-CPU4",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "cpu4",
			.sampling_period_ms = 4,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = HOTPLUG_HIGH_THRESHOLD_8992,
				.lvl_clr = HOTPLUG_LOW_THRESHOLD_8992,
				.num_actions = 1,
				.actions[0] = {
					.device = "hotplug_4",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "HOTPLUG-CPU5",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "cpu5",
			.sampling_period_ms = 4,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = HOTPLUG_HIGH_THRESHOLD_8992,
				.lvl_clr = HOTPLUG_LOW_THRESHOLD_8992,
				.num_actions = 1,
				.actions[0] = {
					.device = "hotplug_5",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS0",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor0",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS1",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor1",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS2",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor2",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS3",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor3",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS4",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor4",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS5",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor5",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS7",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor7",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS9",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor9",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS10",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor10",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS11",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor11",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS12",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor12",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS13",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor13",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS14",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor14",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "PMIC-ALARM-MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "pm8994_tz",
			.sampling_period_ms = 1000,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = PMIC_ALARM_STAGE_1_TEMP_TRIG,
				.lvl_clr = PMIC_ALARM_STAGE_1_TEMP_CLR,
				.num_actions = 7,
				.actions = {
					[0] = {
						.device = "hotplug_5",
						.info = 0,
					},
					[1] = {
						.device = "hotplug_4",
						.info = 0,
					},
					[2] = {
						.device = "hotplug_3",
						.info = 0,
					},
					[3] = {
						.device = "hotplug_2",
						.info = 0,
					},
					[4] = {
						.device = "hotplug_1",
						.info = 0,
					},
					[5] = {
						.device = "cluster1",
						.info = 302400,
					},
					[6] = {
						.device = "cluster0",
						.info = 302400,
					},
				},
			},
			.t[1] = {
				.lvl_trig = PMIC_ALARM_STAGE_2_TEMP_TRIG,
				.lvl_clr = PMIC_ALARM_STAGE_2_TEMP_CLR,
				.num_actions = 5,
				.actions = {
					[0] = {
						.device = "hotplug_5",
						.info = 1,
					},
					[1] = {
						.device = "hotplug_4",
						.info = 1,
					},
					[2] = {
						.device = "hotplug_3",
						.info = 1,
					},
					[3] = {
						.device = "hotplug_2",
						.info = 1,
					},
					[4] = {
						.device = "hotplug_1",
						.info = 1,
					},
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_8996[] =
{
	{
		.desc = "VDD_RSTR_MONITOR-TSENS0",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor0",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS1",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor1",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS2",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor2",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS3",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor3",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS4",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor4",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS5",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor5",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS6",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor6",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS7",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor7",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS8",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor8",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS9",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor9",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS10",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor10",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS11",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor11",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS12",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor12",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS13",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor13",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS14",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor14",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS15",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor15",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS16",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor16",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS17",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor17",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS18",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor18",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS19",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor19",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS20",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor20",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "PMIC-ALARM-MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "pm8994_tz",
			.sampling_period_ms = 1000,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = PMIC_ALARM_STAGE_1_TEMP_TRIG,
				.lvl_clr = PMIC_ALARM_STAGE_1_TEMP_CLR,
				.num_actions = 5,
				.actions = {
					[0] = {
						.device = "hotplug_3",
						.info = 0,
					},
					[1] = {
						.device = "hotplug_2",
						.info = 0,
					},
					[2] = {
						.device = "hotplug_1",
						.info = 0,
					},
					[3] = {
						.device = "cluster1",
						.info = 302400,
					},
					[4] = {
						.device = "cluster0",
						.info = 302400,
					},
				},
			},
			.t[1] = {
				.lvl_trig = PMIC_ALARM_STAGE_2_TEMP_TRIG,
				.lvl_clr = PMIC_ALARM_STAGE_2_TEMP_CLR,
				.num_actions = 3,
				.actions = {
					[0] = {
						.device = "hotplug_3",
						.info = 1,
					},
					[1] = {
						.device = "hotplug_2",
						.info = 1,
					},
					[2] = {
						.device = "hotplug_1",
						.info = 1,
					},
				},
			},
		},
	},
#ifdef ENABLE_MODEM_FE_SENSOR
	{
		.desc = "MODEM0_PA0_MITIGATION",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "modem0_pa0",
			.sampling_period_ms = 1000,
			.num_thresholds = 3,
			._n_thresholds = 3,
			._n_to_clear = 3,
			._n_actions = 3,
			._n_action_info = 3,
			.t[0] = {
				.lvl_trig = 33000,
				.lvl_clr = 30000,
				.num_actions = 1,
				.actions[0] = {
					.device = "modem",
					.info = 1,
				},
			},
			.t[1] = {
				.lvl_trig = 36000,
				.lvl_clr = 32000,
				.num_actions = 1,
				.actions[0] = {
					.device = "modem",
					.info = 2,
				},
			},
			.t[2] = {
				.lvl_trig = 39000,
				.lvl_clr = 34000,
				.num_actions = 1,
				.actions[0] = {
					.device = "modem",
					.info = 3,
				},
			},
		},
	},
#endif /* ENABLE_MODEM_FE_SENSOR*/
};

static struct setting_info tm_cfgs_9650[] =
{
	{
		.desc = "VDD_RSTR_MONITOR-TSENS0",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor0",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS1",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor1",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS2",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor2",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS3",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor3",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS4",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor4",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "MSS_TM",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor4",
			.sampling_period_ms = 300,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = 95000,
				.lvl_clr = 90000,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "modem_proc",
						.info = 1,
					},
				},
			},
			.t[1] = {
				.lvl_trig = 105000,
				.lvl_clr = 100000,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "modem_proc",
						.info = 3,
					},
				},
			}
		},
	},
	{
		.desc = "CPR_BAND_MONITOR-TSENS0",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor0",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 10000,
				.lvl_clr = 15000,
				.num_actions = 1,
				.actions[0] = {
					.device = "cpr_band",
					.info = 2,
				},
			},
		},
	},
	{
		.desc = "CPR_BAND_MONITOR-TSENS1",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor1",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 10000,
				.lvl_clr = 15000,
				.num_actions = 1,
				.actions[0] = {
					.device = "cpr_band",
					.info = 2,
				},
			},
		},
	},
	{
		.desc = "CPR_BAND_MONITOR-TSENS2",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor2",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 10000,
				.lvl_clr = 15000,
				.num_actions = 1,
				.actions[0] = {
					.device = "cpr_band",
					.info = 2,
				},
			},
		},
	},
	{
		.desc = "CPR_BAND_MONITOR-TSENS3",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor3",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 10000,
				.lvl_clr = 15000,
				.num_actions = 1,
				.actions[0] = {
					.device = "cpr_band",
					.info = 2,
				},
			},
		},
	},
	{
		.desc = "CPR_BAND_MONITOR-TSENS4",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor4",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 10000,
				.lvl_clr = 15000,
				.num_actions = 1,
				.actions[0] = {
					.device = "cpr_band",
					.info = 2,
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_sdx_20[] =
{
	{
		.desc = "VDD_RSTR_MONITOR-TSENS0",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor0",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS1",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor1",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS2",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor2",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS3",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor3",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS4",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor4",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "MSS_TM",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor4",
			.sampling_period_ms = 300,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = 101000,
				.lvl_clr = 98000,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "modem_proc",
						.info = 1,
					},
				},
			},
			.t[1] = {
				.lvl_trig = 105000,
				.lvl_clr = 99000,
				.num_actions = 1,
				.actions = {
					[0] = {
						.device = "modem_proc",
						.info = 3,
					},
				},
			}
		},
	},
};

static struct setting_info tm_cfgs_cobalt[] =
{
	{
		.desc = "VDD_RSTR_MONITOR-TSENS0",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor0",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS1",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor1",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS2",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor2",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS3",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor3",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS4",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor4",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS7",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor7",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS8",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor8",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS9",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor9",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS10",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor10",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS11",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor11",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS12",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor12",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS13",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor13",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS14",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor14",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS15",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor15",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS16",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor16",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS17",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor17",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS18",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor18",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS19",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor19",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS20",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor20",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS21",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor21",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "PMIC-ALARM-MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "pm8998_tz",
			.sampling_period_ms = 1000,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = PMIC_ALARM_STAGE_1_TEMP_TRIG,
				.lvl_clr = PMIC_ALARM_STAGE_1_TEMP_CLR,
				.num_actions = 9,
				.actions = {
					[0] = {
						.device = "hotplug_7",
						.info = 0,
					},
					[1] = {
						.device = "hotplug_6",
						.info = 0,
					},
					[2] = {
						.device = "hotplug_5",
						.info = 0,
					},
					[3] = {
						.device = "hotplug_4",
						.info = 0,
					},
					[4] = {
						.device = "hotplug_3",
						.info = 0,
					},
					[5] = {
						.device = "hotplug_2",
						.info = 0,
					},
					[6] = {
						.device = "hotplug_1",
						.info = 0,
					},
					[7] = {
						.device = "cluster1",
						.info = 302400,
					},
					[8] = {
						.device = "cluster0",
						.info = 302400,
					},
				},
			},
			.t[1] = {
				.lvl_trig = PMIC_ALARM_STAGE_2_TEMP_TRIG,
				.lvl_clr = PMIC_ALARM_STAGE_2_TEMP_CLR,
				.num_actions = 7,
				.actions = {
					[0] = {
						.device = "hotplug_7",
						.info = 1,
					},
					[1] = {
						.device = "hotplug_6",
						.info = 1,
					},
					[2] = {
						.device = "hotplug_5",
						.info = 1,
					},
					[3] = {
						.device = "hotplug_4",
						.info = 1,
					},
					[4] = {
						.device = "hotplug_3",
						.info = 1,
					},
					[5] = {
						.device = "hotplug_2",
						.info = 1,
					},
					[6] = {
						.device = "hotplug_1",
						.info = 1,
					},
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_cobalt_qrd[] =
{
	{
		.desc = "VDD_RSTR_MONITOR-TSENS0",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor0",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS1",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor1",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS2",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor2",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS3",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor3",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS4",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor4",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS7",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor7",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS8",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor8",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS9",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor9",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS10",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor10",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS11",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor11",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS12",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor12",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS13",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor13",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS14",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor14",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS15",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor15",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS16",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor16",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS17",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor17",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS18",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor18",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS19",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor19",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS20",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor20",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS21",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor21",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "PMIC-ALARM-MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "pm8998_tz",
			.sampling_period_ms = 1000,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = PMIC_ALARM_STAGE_1_TEMP_TRIG,
				.lvl_clr = PMIC_ALARM_STAGE_1_TEMP_CLR,
				.num_actions = 9,
				.actions = {
					[0] = {
						.device = "hotplug_7",
						.info = 0,
					},
					[1] = {
						.device = "hotplug_6",
						.info = 0,
					},
					[2] = {
						.device = "hotplug_5",
						.info = 0,
					},
					[3] = {
						.device = "hotplug_4",
						.info = 0,
					},
					[4] = {
						.device = "hotplug_3",
						.info = 0,
					},
					[5] = {
						.device = "hotplug_2",
						.info = 0,
					},
					[6] = {
						.device = "hotplug_1",
						.info = 0,
					},
					[7] = {
						.device = "cluster1",
						.info = 302400,
					},
					[8] = {
						.device = "cluster0",
						.info = 302400,
					},
				},
			},
			.t[1] = {
				.lvl_trig = PMIC_ALARM_STAGE_2_TEMP_TRIG,
				.lvl_clr = PMIC_ALARM_STAGE_2_TEMP_CLR,
				.num_actions = 7,
				.actions = {
					[0] = {
						.device = "hotplug_7",
						.info = 1,
					},
					[1] = {
						.device = "hotplug_6",
						.info = 1,
					},
					[2] = {
						.device = "hotplug_5",
						.info = 1,
					},
					[3] = {
						.device = "hotplug_4",
						.info = 1,
					},
					[4] = {
						.device = "hotplug_3",
						.info = 1,
					},
					[5] = {
						.device = "hotplug_2",
						.info = 1,
					},
					[6] = {
						.device = "hotplug_1",
						.info = 1,
					},
				},
			},
		},
	},
	{
		.desc = "LCD-MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "quiet_therm",
			.sampling_period_ms = 2000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 34000,
				.lvl_clr = 32000,
				.num_actions = 1,
				.actions[0] = {
					.device = "lcd",
					.info = 10,
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_sdm660[] =
{
	{
		.desc = "VDD_RSTR_MONITOR-TSENS0",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor0",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS1",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor1",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS2",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor2",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS3",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor3",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS4",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor4",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS5",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor5",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS6",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor6",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS7",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor7",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS8",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor8",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS9",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor9",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS10",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor10",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS11",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor11",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS12",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor12",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS13",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor13",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_sdm660_qrd[] =
{
	{
		.desc = "VDD_RSTR_MONITOR-TSENS0",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor0",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS1",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor1",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS2",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor2",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS3",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor3",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS4",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor4",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS5",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor5",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS6",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor6",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS7",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor7",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS8",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor8",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS9",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor9",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS10",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor10",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS11",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor11",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS12",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor12",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS13",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor13",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "BATTERY_CHARGING_CTL",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "quiet_therm",
			.sampling_period_ms = 10000,
			.num_thresholds = 4,
			._n_thresholds = 4,
			._n_to_clear = 4,
			._n_actions = 4,
			._n_action_info = 4,
			.t[0] = {
				.lvl_trig = 38000,
				.lvl_clr = 36000,
				.num_actions = 1,
				.actions[0] = {
					.device = "battery",
					.info = 2,
				},
			},
			.t[1] = {
				.lvl_trig = 40000,
				.lvl_clr = 38000,
				.num_actions = 1,
				.actions[0] = {
					.device = "battery",
					.info = 3,
				},
			},
			.t[2] = {
				.lvl_trig = 45000,
				.lvl_clr = 40000,
				.num_actions = 1,
				.actions[0] = {
					.device = "battery",
					.info = 4,
				},
			},
			.t[3] = {
				.lvl_trig = 48000,
				.lvl_clr = 45000,
				.num_actions = 1,
				.actions[0] = {
					.device = "battery",
					.info = 5,
				},
			}
		},
	},
	{
		.desc = "MODEM_PA_MITIGATION",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "quiet_therm",
			.sampling_period_ms = 10000,
			.num_thresholds = 3,
			._n_thresholds = 3,
			._n_to_clear = 3,
			._n_actions = 3,
			._n_action_info = 3,
			.t[0] = {
				.lvl_trig = 45000,
				.lvl_clr = 43000,
				.num_actions = 1,
				.actions[0] = {
					.device = "modem",
					.info = 1,
				},
			},
			.t[1] = {
				.lvl_trig = 47000,
				.lvl_clr = 45000,
				.num_actions = 1,
				.actions[0] = {
					.device = "modem",
					.info = 2,
				},
			},
			.t[2] = {
				.lvl_trig = 55000,
				.lvl_clr = 50000,
				.num_actions = 1,
				.actions[0] = {
					.device = "modem",
					.info = 3,
				},
			},
		},
	},
	{
		.desc = "MODEM_PROC_TEMP_MITIGATION",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "quiet_therm",
			.sampling_period_ms = 10000,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = 42000,
				.lvl_clr = 40000,
				.num_actions = 1,
				.actions[0] = {
					.device = "modem_proc",
					.info = 1,
				},
			},
			.t[1] = {
				.lvl_trig = 55000,
				.lvl_clr = 50000,
				.num_actions = 1,
				.actions[0] = {
					.device = "modem_proc",
					.info = 3,
				},
			},
		},
	},
	{
		.desc = "CAMERA_SKIN_EXIT_MITIGATION",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "quiet_therm",
			.sampling_period_ms = 10000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 50000,
				.lvl_clr = 45000,
				.num_actions = 2,
				.actions[0] = {
					.device = "camera",
					.info = 10,
				},
				.actions[1] = {
					.device = "camcorder",
					.info = 10,
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_sdm630[] =
{
	{
		.desc = "VDD_RSTR_MONITOR-TSENS0",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor0",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS1",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor1",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS2",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor2",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS3",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor3",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS4",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor4",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS5",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor5",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS6",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor6",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS7",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor7",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS8",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor8",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS9",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor9",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS10",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor10",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS11",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor11",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_sdm630_qrd[] =
{
	{
		.desc = "VDD_RSTR_MONITOR-TSENS0",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor0",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS1",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor1",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS2",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor2",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS3",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor3",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS4",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor4",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS5",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor5",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS6",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor6",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS7",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor7",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS8",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor8",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS9",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor9",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS10",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor10",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "VDD_RSTR_MONITOR-TSENS11",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor11",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			.descending_thresh = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = VDD_RESTRICTION_THRESH,
				.lvl_clr = VDD_RESTRICTION_THRESH_CLR,
				.num_actions = 1,
				.actions[0] = {
					.device = "vdd_restriction",
					.info = 1,
				},
			},
		},
	},
	{
		.desc = "BATTERY_CHARGING_CTL",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "quiet_therm",
			.sampling_period_ms = 10000,
			.num_thresholds = 4,
			._n_thresholds = 4,
			._n_to_clear = 4,
			._n_actions = 4,
			._n_action_info = 4,
			.t[0] = {
				.lvl_trig = 38000,
				.lvl_clr = 36000,
				.num_actions = 1,
				.actions[0] = {
					.device = "battery",
					.info = 2,
				},
			},
			.t[1] = {
				.lvl_trig = 40000,
				.lvl_clr = 38000,
				.num_actions = 1,
				.actions[0] = {
					.device = "battery",
					.info = 3,
				},
			},
			.t[2] = {
				.lvl_trig = 45000,
				.lvl_clr = 40000,
				.num_actions = 1,
				.actions[0] = {
					.device = "battery",
					.info = 4,
				},
			},
			.t[3] = {
				.lvl_trig = 48000,
				.lvl_clr = 45000,
				.num_actions = 1,
				.actions[0] = {
					.device = "battery",
					.info = 5,
				},
			}
		},
	},
	{
		.desc = "MODEM_PA_MITIGATION",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "quiet_therm",
			.sampling_period_ms = 10000,
			.num_thresholds = 3,
			._n_thresholds = 3,
			._n_to_clear = 3,
			._n_actions = 3,
			._n_action_info = 3,
			.t[0] = {
				.lvl_trig = 45000,
				.lvl_clr = 43000,
				.num_actions = 1,
				.actions[0] = {
					.device = "modem",
					.info = 1,
				},
			},
			.t[1] = {
				.lvl_trig = 47000,
				.lvl_clr = 45000,
				.num_actions = 1,
				.actions[0] = {
					.device = "modem",
					.info = 2,
				},
			},
			.t[2] = {
				.lvl_trig = 55000,
				.lvl_clr = 50000,
				.num_actions = 1,
				.actions[0] = {
					.device = "modem",
					.info = 3,
				},
			},
		},
	},
	{
		.desc = "MODEM_PROC_TEMP_MITIGATION",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "quiet_therm",
			.sampling_period_ms = 10000,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = 42000,
				.lvl_clr = 40000,
				.num_actions = 1,
				.actions[0] = {
					.device = "modem_proc",
					.info = 1,
				},
			},
			.t[1] = {
				.lvl_trig = 55000,
				.lvl_clr = 50000,
				.num_actions = 1,
				.actions[0] = {
					.device = "modem_proc",
					.info = 3,
				},
			},
		},
	},
	{
		.desc = "CAMERA_SKIN_EXIT_MITIGATION",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "quiet_therm",
			.sampling_period_ms = 10000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 55000,
				.lvl_clr = 45000,
				.num_actions = 2,
				.actions[0] = {
					.device = "camera",
					.info = 10,
				},
				.actions[1] = {
					.device = "camcorder",
					.info = 10,
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_sdm670_qrd[] =
{
	{
		.desc = "CAMERA_SKIN_EXIT_MITIGATION",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "xo-therm-adc",
			.sampling_period_ms = 10000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 50000,
				.lvl_clr = 45000,
				.num_actions = 2,
				.actions[0] = {
					.device = "camera",
					.info = 10,
				},
				.actions[1] = {
					.device = "camcorder",
					.info = 10,
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_sdm632_qrd[] =
{
	{
		.desc = "CAMERA_SKIN_EXIT_MITIGATION",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "quiet-therm-adc",
			.sampling_period_ms = 10000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 55000,
				.lvl_clr = 45000,
				.num_actions = 2,
				.actions[0] = {
					.device = "camera",
					.info = 10,
				},
				.actions[1] = {
					.device = "camcorder",
					.info = 10,
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_qcs605[] =
{
	{
		.desc = "TSENS_HVX_MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "compute-hvx-usr",
			.sampling_period_ms = 10000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 100000,
				.lvl_clr = 95000,
				.num_actions = 1,
				.actions[0] = {
					.device = "hvx_sw",
					.info = 4,
				},
			},
		},
	},
#ifndef ENABLE_QCS605_LC
	{
		.desc = "SKIN_HVX_MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "xo-therm-adc",
			.sampling_period_ms = 10000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 38000,
				.lvl_clr = 33000,
				.num_actions = 1,
				.actions[0] = {
					.device = "hvx_sw",
					.info = 4,
				},
			},
		},
	},
#endif /* ENABLE_QCS605_LC */
};

static struct setting_info tm_cfgs_sdm439_qrd[] =
{
	{
		.desc = "CAMERA_SKIN_EXIT_MITIGATION",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "quiet-therm-adc",
			.sampling_period_ms = 10000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 55000,
				.lvl_clr = 45000,
				.num_actions = 2,
				.actions[0] = {
					.device = "camera",
					.info = 10,
				},
				.actions[1] = {
					.device = "camcorder",
					.info = 10,
				},
			},
		},
	},
};

#ifdef USE_THERMAL_FRAMEWORK
static struct setting_info tm_cfgs_8909[] =
{
	{
		.desc = "CAMERA_CAMCORDER_MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "camera-usr",
			.sampling_period_ms = 250,
			.num_thresholds = 3,
			._n_thresholds = 3,
			._n_to_clear = 3,
			._n_actions = 3,
			._n_action_info = 3,
			.t[0] = {
				.lvl_trig = 80000,
				.lvl_clr = 75000,
				.num_actions = 2,
				.actions[0] = {
					.device = "camera",
					.info = 1,
				},
				.actions[1] = {
					.device = "camcorder",
					.info = 0,
				},
			},
			.t[1] = {
				.lvl_trig = 85000,
				.lvl_clr = 80000,
				.num_actions = 2,
				.actions[0] = {
					.device = "camera",
					.info = 2,
				},
				.actions[1] = {
					.device = "camcorder",
					.info = 0,
				},
			},
			.t[2] = {
				.lvl_trig = 88000,
				.lvl_clr = 85000,
				.num_actions = 2,
				.actions[0] = {
					.device = "camera",
					.info = 10,
				},
				.actions[1] = {
					.device = "camcorder",
					.info = 10,
				},
			},
		},
	},
	{
		.desc = "CX_MITIGATION_MONITOR_TSENS2",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "gpu-usr",
			.sampling_period_ms = 250,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = 92000,
				.lvl_clr = 85000,
				.num_actions = 7,
				.actions[0] = {
					.device = "modem_cx",
					.info = 2,
				},
				.actions[1] = {
					.device = "gpu",
					.info = 200000000,
				},
				.actions[2] = {
					.device = "mdp",
					.info = 1,
				},
				.actions[3] = {
					.device = "camera",
					.info = 0,
				},
				.actions[4] = {
					.device = "camcorder",
					.info = 0,
				},
			},
			.t[1] = {
				.lvl_trig = 100000,
				.lvl_clr = 92000,
				.num_actions = 7,
				.actions[0] = {
					.device = "modem_cx",
					.info = 3,
				},
				.actions[1] = {
					.device = "gpu",
					.info = 200000000,
				},
				.actions[2] = {
					.device = "mdp",
					.info = 3,
				},
				.actions[3] = {
					.device = "camera",
					.info = 10,
				},
				.actions[4] = {
					.device = "camcorder",
					.info = 10,
				},
			},
		},
	},
	{
		.desc = "CX_MITIGATION_MONITOR_TSENS3",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "cpu0-2-usr",
			.sampling_period_ms = 250,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = 92000,
				.lvl_clr = 85000,
				.num_actions = 7,
				.actions[0] = {
					.device = "modem_cx",
					.info = 2,
				},
				.actions[1] = {
					.device = "gpu",
					.info = 200000000,
				},
				.actions[2] = {
					.device = "mdp",
					.info = 1,
				},
				.actions[3] = {
					.device = "camera",
					.info = 0,
				},
				.actions[4] = {
					.device = "camcorder",
					.info = 0,
				},
			},
			.t[1] = {
				.lvl_trig = 100000,
				.lvl_clr = 92000,
				.num_actions = 7,
				.actions[0] = {
					.device = "modem_cx",
					.info = 3,
				},
				.actions[1] = {
					.device = "gpu",
					.info = 200000000,
				},
				.actions[2] = {
					.device = "mdp",
					.info = 3,
				},
				.actions[3] = {
					.device = "camera",
					.info = 10,
				},
				.actions[4] = {
					.device = "camcorder",
					.info = 10,
				},
			},
		},
	},
	{
		.desc = "CX_MITIGATION_MONITOR_TSENS4",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "cpu1-3-usr",
			.sampling_period_ms = 250,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = 92000,
				.lvl_clr = 85000,
				.num_actions = 7,
				.actions[0] = {
					.device = "modem_cx",
					.info = 2,
				},
				.actions[1] = {
					.device = "gpu",
					.info = 200000000,
				},
				.actions[2] = {
					.device = "mdp",
					.info = 1,
				},
				.actions[3] = {
					.device = "camera",
					.info = 0,
				},
				.actions[4] = {
					.device = "camcorder",
					.info = 0,
				},
			},
			.t[1] = {
				.lvl_trig = 100000,
				.lvl_clr = 92000,
				.num_actions = 7,
				.actions[0] = {
					.device = "modem_cx",
					.info = 3,
				},
				.actions[1] = {
					.device = "gpu",
					.info = 200000000,
				},
				.actions[2] = {
					.device = "mdp",
					.info = 3,
				},
				.actions[3] = {
					.device = "camera",
					.info = 10,
				},
				.actions[4] = {
					.device = "camcorder",
					.info = 10,
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_8909_qrd_hq[] =
{
	{
		.desc = "BATTERY-CHARGE-MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "cpu1-3-usr",
			.sampling_period_ms = 1000,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = 50000,
				.lvl_clr = 45000,
				.num_actions = 1,
				.actions[0] = {
					.device = "battery",
					.info = 1,
				},
			},
			.t[1] = {
				.lvl_trig = 55000,
				.lvl_clr = 50000,
				.num_actions = 1,
				.actions[0] = {
					.device = "battery",
					.info = 2,
				},
			},
		},
	},
	{
		.desc = "CAMERA_CAMCORDER_MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "mdm-usr",
			.sampling_period_ms = 250,
			.num_thresholds = 3,
			._n_thresholds = 3,
			._n_to_clear = 3,
			._n_actions = 3,
			._n_action_info = 3,
			.t[0] = {
				.lvl_trig = 80000,
				.lvl_clr = 75000,
				.num_actions = 2,
				.actions[0] = {
					.device = "camera",
					.info = 1,
				},
				.actions[1] = {
					.device = "camcorder",
					.info = 0,
				},
			},
			.t[1] = {
				.lvl_trig = 85000,
				.lvl_clr = 80000,
				.num_actions = 2,
				.actions[0] = {
					.device = "camera",
					.info = 2,
				},
				.actions[1] = {
					.device = "camcorder",
					.info = 0,
				},
			},
			.t[2] = {
				.lvl_trig = 88000,
				.lvl_clr = 85000,
				.num_actions = 2,
				.actions[0] = {
					.device = "camera",
					.info = 10,
				},
				.actions[1] = {
					.device = "camcorder",
					.info = 10,
				},
			},
		},
	},
	{
		.desc = "CX_MITIGATION_MONITOR_TSENS2",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "gpu-usr",
			.sampling_period_ms = 250,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = 92000,
				.lvl_clr = 85000,
				.num_actions = 7,
				.actions[0] = {
					.device = "modem_cx",
					.info = 2,
				},
				.actions[1] = {
					.device = "gpu",
					.info = 200000000,
				},
				.actions[2] = {
					.device = "mdp",
					.info = 1,
				},
				.actions[3] = {
					.device = "camera",
					.info = 0,
				},
				.actions[4] = {
					.device = "camcorder",
					.info = 0,
				},
			},
			.t[1] = {
				.lvl_trig = 100000,
				.lvl_clr = 92000,
				.num_actions = 7,
				.actions[0] = {
					.device = "modem_cx",
					.info = 3,
				},
				.actions[1] = {
					.device = "gpu",
					.info = 200000000,
				},
				.actions[2] = {
					.device = "mdp",
					.info = 3,
				},
				.actions[3] = {
					.device = "camera",
					.info = 10,
				},
				.actions[4] = {
					.device = "camcorder",
					.info = 10,
				},
			},
		},
	},
	{
		.desc = "CX_MITIGATION_MONITOR_TSENS3",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "cpu0-2-usr",
			.sampling_period_ms = 250,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = 92000,
				.lvl_clr = 85000,
				.num_actions = 7,
				.actions[0] = {
					.device = "modem_cx",
					.info = 2,
				},
				.actions[1] = {
					.device = "gpu",
					.info = 200000000,
				},
				.actions[2] = {
					.device = "mdp",
					.info = 1,
				},
				.actions[3] = {
					.device = "camera",
					.info = 0,
				},
				.actions[4] = {
					.device = "camcorder",
					.info = 0,
				},
			},
			.t[1] = {
				.lvl_trig = 100000,
				.lvl_clr = 92000,
				.num_actions = 7,
				.actions[0] = {
					.device = "modem_cx",
					.info = 3,
				},
				.actions[1] = {
					.device = "gpu",
					.info = 200000000,
				},
				.actions[2] = {
					.device = "mdp",
					.info = 3,
				},
				.actions[3] = {
					.device = "camera",
					.info = 10,
				},
				.actions[4] = {
					.device = "camcorder",
					.info = 10,
				},
			},
		},
	},
	{
		.desc = "CX_MITIGATION_MONITOR_TSENS4",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "cpu1-3-usr",
			.sampling_period_ms = 250,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = 92000,
				.lvl_clr = 85000,
				.num_actions = 7,
				.actions[0] = {
					.device = "modem_cx",
					.info = 2,
				},
				.actions[1] = {
					.device = "gpu",
					.info = 200000000,
				},
				.actions[2] = {
					.device = "mdp",
					.info = 1,
				},
				.actions[3] = {
					.device = "camera",
					.info = 0,
				},
				.actions[4] = {
					.device = "camcorder",
					.info = 0,
				},
			},
			.t[1] = {
				.lvl_trig = 100000,
				.lvl_clr = 92000,
				.num_actions = 7,
				.actions[0] = {
					.device = "modem_cx",
					.info = 3,
				},
				.actions[1] = {
					.device = "gpu",
					.info = 200000000,
				},
				.actions[2] = {
					.device = "mdp",
					.info = 3,
				},
				.actions[3] = {
					.device = "camera",
					.info = 10,
				},
				.actions[4] = {
					.device = "camcorder",
					.info = 10,
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_8909w[] =
{
	{
		.desc = "CX_MITIGATION_SKIN_TEMP_CTL",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "quiet-therm-adc",
			.sampling_period_ms = 5000,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = 51000,
				.lvl_clr = 49000,
				.num_actions = 4,
				.actions[0] = {
					.device = "modem_cx",
					.info = 2,
				},
				.actions[1] = {
					.device = "gpu",
					.info = 200000000,
				},
				.actions[2] = {
					.device = "mdp",
					.info = 1,
				}
			},
			.t[1] = {
				.lvl_trig = 55000,
				.lvl_clr = 53000,
				.num_actions = 4,
				.actions[0] = {
					.device = "modem_cx",
					.info = 3,
				},
				.actions[1] = {
					.device = "gpu",
					.info = 200000000,
				},
				.actions[2] = {
					.device = "mdp",
					.info = 3,
				}
			},
		},
	},
};
#endif

void thermal_monitor_init_data(struct thermal_setting_t *setting)
{
	int i;
	int arr_size = 0;

	struct setting_info *cfg = NULL;

	switch (therm_get_msm_id()) {
		case THERM_MSM_8960:
			cfg = tm_cfgs_8960;
			arr_size = ARRAY_SIZE(tm_cfgs_8960);
			break;
		case THERM_MSM_8930:
			cfg = tm_cfgs_8930;
			arr_size = ARRAY_SIZE(tm_cfgs_8930);
			break;
		case THERM_MSM_8064AB:
		case THERM_MSM_8064:
			cfg = tm_cfgs_8064;
			arr_size = ARRAY_SIZE(tm_cfgs_8064);
			break;
		case THERM_MSM_8974:
			cfg = tm_cfgs_8974;
			arr_size = ARRAY_SIZE(tm_cfgs_8974);
			break;
		case THERM_MSM_8x62:
			cfg = tm_cfgs_8x62;
			arr_size = ARRAY_SIZE(tm_cfgs_8x62);
			break;
		case THERM_MSM_8974PRO_AA:
		case THERM_MSM_8974PRO_AB:
			cfg = tm_cfgs_8974_pro_ab;
			arr_size = ARRAY_SIZE(tm_cfgs_8974_pro_ab);
			break;
		case THERM_MSM_8974PRO_AC:
			cfg = tm_cfgs_8974_pro_ac;
			arr_size = ARRAY_SIZE(tm_cfgs_8974_pro_ac);
			break;
		case THERM_MSM_8084:
			cfg = tm_cfgs_8084;
			arr_size = ARRAY_SIZE(tm_cfgs_8084);
			break;
		case THERM_MPQ_8092:
			cfg = tm_cfgs_8092;
			arr_size = ARRAY_SIZE(tm_cfgs_8092);
			break;
		case THERM_MSM_8226:
			switch (therm_get_msm_version()) {
				case THERM_VERSION_V1:
					cfg = tm_cfgs_8226_v1;
					arr_size = ARRAY_SIZE(tm_cfgs_8226_v1);
					break;
				default:
					cfg = tm_cfgs_8226_v2;
					arr_size = ARRAY_SIZE(tm_cfgs_8226_v2);
					break;
			}
			break;
		case THERM_MSM_8926:
			cfg = tm_cfgs_8226_v2;
			arr_size = ARRAY_SIZE(tm_cfgs_8226_v2);
			break;
		case THERM_MSM_8610:
			cfg = tm_cfgs_8610;
			arr_size = ARRAY_SIZE(tm_cfgs_8610);
			break;
		case THERM_MDM_9625:
		case THERM_MDM_9635:
			cfg = tm_cfgs_9625;
			arr_size = ARRAY_SIZE(tm_cfgs_9625);
			break;
		case THERM_FSM_9900:
			cfg = tm_cfgs_9900;
			arr_size = ARRAY_SIZE(tm_cfgs_9900);
			break;
		case THERM_MSM_8994:
			cfg = tm_cfgs_8994;
			arr_size = ARRAY_SIZE(tm_cfgs_8994);
			break;
		case THERM_MDM_9645:
			cfg = tm_cfgs_9645;
			arr_size = ARRAY_SIZE(tm_cfgs_9645);
			break;
		case THERM_MSM_8992:
			cfg = tm_cfgs_8992;
			arr_size = ARRAY_SIZE(tm_cfgs_8992);
			break;
		case THERM_MSM_8996:
		case THERM_MSM_8996AU:
			cfg = tm_cfgs_8996;
			arr_size = ARRAY_SIZE(tm_cfgs_8996);
			break;
		case THERM_MDM_9650:
			cfg = tm_cfgs_9650;
			arr_size = ARRAY_SIZE(tm_cfgs_9650);
			break;
		case THERM_SDX_20:
			cfg = tm_cfgs_sdx_20;
			arr_size = ARRAY_SIZE(tm_cfgs_sdx_20);
			break;
		case THERM_MSM_COBALT:
			switch (therm_get_hw_platform()) {
			case THERM_PLATFORM_MTP:
				cfg = tm_cfgs_cobalt;
				arr_size = ARRAY_SIZE(tm_cfgs_cobalt);
				break;
			default:
				cfg = tm_cfgs_cobalt_qrd;
				arr_size = ARRAY_SIZE(tm_cfgs_cobalt_qrd);
				break;
			}
			break;
		case THERM_SDM_660:
			switch (therm_get_hw_platform()) {
			case THERM_PLATFORM_MTP:
				cfg = tm_cfgs_sdm660;
				arr_size =
					ARRAY_SIZE(tm_cfgs_sdm660);
				break;
			default:
				cfg = tm_cfgs_sdm660_qrd;
				arr_size =
					ARRAY_SIZE(tm_cfgs_sdm660_qrd);
				break;
			}
			break;
		case THERM_SDM_630:
			switch (therm_get_hw_platform()) {
			case THERM_PLATFORM_MTP:
				cfg = tm_cfgs_sdm630;
				arr_size =
					ARRAY_SIZE(tm_cfgs_sdm630);
				break;
			default:
				cfg = tm_cfgs_sdm630_qrd;
				arr_size =
					ARRAY_SIZE(tm_cfgs_sdm630_qrd);
				break;
			}
			break;
		case THERM_SDM_670:
			switch (therm_get_hw_platform()) {
			case THERM_PLATFORM_MTP:
				break;
			default:
				cfg = tm_cfgs_sdm670_qrd;
				arr_size =
					ARRAY_SIZE(tm_cfgs_sdm670_qrd);
				break;
			}
			break;
		case THERM_SDM_632:
			switch (therm_get_hw_platform()) {
			case THERM_PLATFORM_MTP:
				break;
			default:
				cfg = tm_cfgs_sdm632_qrd;
				arr_size =
					ARRAY_SIZE(tm_cfgs_sdm632_qrd);
				break;
			}
			break;
		case THERM_SDM_439:
			switch (therm_get_hw_platform()) {
			case THERM_PLATFORM_MTP:
				break;
			default:
				cfg = tm_cfgs_sdm439_qrd;
				arr_size =
					ARRAY_SIZE(tm_cfgs_sdm439_qrd);
				break;
			}
			break;
#ifdef USE_THERMAL_FRAMEWORK
		case THERM_MSM_8909:
			switch (therm_get_pmic_model()) {
				case THERM_PMIC_PM8916:
					break;
				case THERM_PMIC_PM8909:
				default:
					if (therm_get_hw_platform_subtype() ==
						THERM_PLATFORM_SUB_QRD_HUAQIN) {
						cfg = tm_cfgs_8909_qrd_hq;
						arr_size =
						ARRAY_SIZE(tm_cfgs_8909_qrd_hq);
					} else {
						cfg = tm_cfgs_8909;
						arr_size =
						ARRAY_SIZE(tm_cfgs_8909);
					}
					break;
			}
			break;
		case THERM_MSM_8909W:
			if (therm_get_hw_platform_subtype() ==
				THERM_PLATFORM_SUB_WEARABLE_COMPAL) {
				cfg = tm_cfgs_8909w;
				arr_size = ARRAY_SIZE(tm_cfgs_8909w);
			}
			break;
#endif
		case THERM_QCS_605:
			cfg = tm_cfgs_qcs605;
			arr_size = ARRAY_SIZE(tm_cfgs_qcs605);
			break;
		default:
			msg("%s: ERROR Uknown device\n", __func__);
			cfg = NULL;
			arr_size = 0;
			break;
	}

	if (cfg != NULL) {
		for (i = 0; i < arr_size; i++) {
			add_setting(setting, &cfg[i]);
		}
	}
}
