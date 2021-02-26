/*===========================================================================

Copyright (c) 2014-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/

#include "thermal.h"
#include "thermal_config.h"

#define SHUTDOWN_MONITOR_THRESH 120000
#define SHUTDOWN_MONITOR_THRESH_CLR 115000
#define VDD_RESTRICTION_THRESH 5000
#define VDD_RESTRICTION_THRESH_CLR 10000

static struct setting_info tm_cfgs_8916[] =
{
	{
		.desc = "CPU0-1_MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "cpu0-1",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 97000,
				.lvl_clr =  92000,
				.num_actions = 1,
				.actions[0] = {
					.device = "shutdown",
					.info = 0,
				},
			},
		},
	},
	{
		.desc = "CPU2-3_MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "cpu2-3",
			.sampling_period_ms = 1000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 97000,
				.lvl_clr = 92000,
				.num_actions = 1,
				.actions[0] = {
					.device = "shutdown",
					.info = 0,
				},
			},
		},
	},
};

static struct setting_info tm_cfgs_8939[] =
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
		.desc = "CPU3_HOTPLUG_MONITOR",
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
				.lvl_trig = 85000,
				.lvl_clr = 55000,
				.num_actions = 1,
				.actions[0] = {
					.device = "hotplug_3",
					.info = 1,
				},
			}
		},
	},
	{
		.desc = "CAMERA_CAMCORDER_MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor1",
			.sampling_period_ms = 1000,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = 85000,
				.lvl_clr = 80000,
				.num_actions = 2,
				.actions[0] = {
					.device = "camera",
					.info = 1,
				},
				.actions[1] = {
					.device = "camcorder",
					.info = 1,
				},
			},
			.t[1] = {
				.lvl_trig = 90000,
				.lvl_clr = 85000,
				.num_actions = 2,
				.actions[0] = {
					.device = "camera",
					.info = 2,
				},
				.actions[1] = {
					.device = "camcorder",
					.info = 2,
				},
			}
		},
	},
};

#ifndef USE_THERMAL_FRAMEWORK
static struct setting_info tm_cfgs_8909[] =
{
	{
		.desc = "CPU3_HOTPLUG_MONITOR",
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
				.lvl_trig = 85000,
				.lvl_clr = 80000,
				.num_actions = 1,
				.actions[0] = {
					.device = "hotplug_3",
					.info = 1,
				},
			}
		},
	},
	{
		.desc = "CPU2_HOTPLUG_MONITOR",
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
				.lvl_trig = 88000,
				.lvl_clr = 85000,
				.num_actions = 1,
				.actions[0] = {
					.device = "hotplug_2",
					.info = 1,
				},
			}
		},
	},
	{
		.desc = "CPU1_HOTPLUG_MONITOR",
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
				.lvl_trig = 92000,
				.lvl_clr = 85000,
				.num_actions = 1,
				.actions[0] = {
					.device = "hotplug_1",
					.info = 1,
				},
			}
		},
	},
	{
		.desc = "CAMERA_CAMCORDER_MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor1",
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
			}
		},
	},
	{
		.desc = "CX_MITIGATION_MONITOR_TSENS2",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor2",
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
					.device = "venus",
					.info = 1,
				},
				.actions[3] = {
					.device = "mdp",
					.info = 1,
				},
				.actions[4] = {
					.device = "wlan",
					.info = 1,
				},
				.actions[5] = {
					.device = "camera",
					.info = 0,
				},
				.actions[6] = {
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
					.device = "venus",
					.info = 3,
				},
				.actions[3] = {
					.device = "mdp",
					.info = 3,
				},
				.actions[4] = {
					.device = "wlan",
					.info = 4,
				},
				.actions[5] = {
					.device = "camera",
					.info = 10,
				},
				.actions[6] = {
					.device = "camcorder",
					.info = 10,
				},
			}
		},
	},
	{
		.desc = "CX_MITIGATION_MONITOR_TSENS3",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor3",
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
					.device = "venus",
					.info = 1,
				},
				.actions[3] = {
					.device = "mdp",
					.info = 1,
				},
				.actions[4] = {
					.device = "wlan",
					.info = 1,
				},
				.actions[5] = {
					.device = "camera",
					.info = 0,
				},
				.actions[6] = {
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
					.device = "venus",
					.info = 3,
				},
				.actions[3] = {
					.device = "mdp",
					.info = 3,
				},
				.actions[4] = {
					.device = "wlan",
					.info = 4,
				},
				.actions[5] = {
					.device = "camera",
					.info = 10,
				},
				.actions[6] = {
					.device = "camcorder",
					.info = 10,
				},
			}
		},
	},
	{
		.desc = "CX_MITIGATION_MONITOR_TSENS4",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor4",
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
					.device = "venus",
					.info = 1,
				},
				.actions[3] = {
					.device = "mdp",
					.info = 1,
				},
				.actions[4] = {
					.device = "wlan",
					.info = 1,
				},
				.actions[5] = {
					.device = "camera",
					.info = 0,
				},
				.actions[6] = {
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
					.device = "venus",
					.info = 3,
				},
				.actions[3] = {
					.device = "mdp",
					.info = 3,
				},
				.actions[4] = {
					.device = "wlan",
					.info = 4,
				},
				.actions[5] = {
					.device = "camera",
					.info = 10,
				},
				.actions[6] = {
					.device = "camcorder",
					.info = 10,
				},
			}
		},
	},
};

static struct setting_info tm_cfgs_8909_qrd_hq[] =
{
	{
		.desc = "BATTERY-CHARGE-MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor3",
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
			}
		},
	},
	{
		.desc = "CPU3_HOTPLUG_MONITOR",
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
				.lvl_trig = 85000,
				.lvl_clr = 80000,
				.num_actions = 1,
				.actions[0] = {
					.device = "hotplug_3",
					.info = 1,
				},
			}
		},
	},
	{
		.desc = "CPU2_HOTPLUG_MONITOR",
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
				.lvl_trig = 88000,
				.lvl_clr = 85000,
				.num_actions = 1,
				.actions[0] = {
					.device = "hotplug_2",
					.info = 1,
				},
			}
		},
	},
	{
		.desc = "CPU1_HOTPLUG_MONITOR",
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
				.lvl_trig = 92000,
				.lvl_clr = 85000,
				.num_actions = 1,
				.actions[0] = {
					.device = "hotplug_1",
					.info = 1,
				},
			}
		},
	},
	{
		.desc = "CAMERA_CAMCORDER_MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor1",
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
			}
		},
	},
	{
		.desc = "CX_MITIGATION_MONITOR_TSENS2",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor2",
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
					.device = "venus",
					.info = 1,
				},
				.actions[3] = {
					.device = "mdp",
					.info = 1,
				},
				.actions[4] = {
					.device = "wlan",
					.info = 1,
				},
				.actions[5] = {
					.device = "camera",
					.info = 0,
				},
				.actions[6] = {
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
					.device = "venus",
					.info = 3,
				},
				.actions[3] = {
					.device = "mdp",
					.info = 3,
				},
				.actions[4] = {
					.device = "wlan",
					.info = 4,
				},
				.actions[5] = {
					.device = "camera",
					.info = 10,
				},
				.actions[6] = {
					.device = "camcorder",
					.info = 10,
				},
			}
		},
	},
	{
		.desc = "CX_MITIGATION_MONITOR_TSENS3",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor3",
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
					.device = "venus",
					.info = 1,
				},
				.actions[3] = {
					.device = "mdp",
					.info = 1,
				},
				.actions[4] = {
					.device = "wlan",
					.info = 1,
				},
				.actions[5] = {
					.device = "camera",
					.info = 0,
				},
				.actions[6] = {
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
					.device = "venus",
					.info = 3,
				},
				.actions[3] = {
					.device = "mdp",
					.info = 3,
				},
				.actions[4] = {
					.device = "wlan",
					.info = 4,
				},
				.actions[5] = {
					.device = "camera",
					.info = 10,
				},
				.actions[6] = {
					.device = "camcorder",
					.info = 10,
				},
			}
		},
	},
	{
		.desc = "CX_MITIGATION_MONITOR_TSENS4",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "tsens_tz_sensor4",
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
					.device = "venus",
					.info = 1,
				},
				.actions[3] = {
					.device = "mdp",
					.info = 1,
				},
				.actions[4] = {
					.device = "wlan",
					.info = 1,
				},
				.actions[5] = {
					.device = "camera",
					.info = 0,
				},
				.actions[6] = {
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
					.device = "venus",
					.info = 3,
				},
				.actions[3] = {
					.device = "mdp",
					.info = 3,
				},
				.actions[4] = {
					.device = "wlan",
					.info = 4,
				},
				.actions[5] = {
					.device = "camera",
					.info = 10,
				},
				.actions[6] = {
					.device = "camcorder",
					.info = 10,
				},
			}
		},
	},
};

static struct setting_info tm_cfgs_8909_pm8916[] =
{
	{
		.desc = "CPU3_HOTPLUG_MONITOR",
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
				.lvl_trig = 85000,
				.lvl_clr = 80000,
				.num_actions = 1,
				.actions[0] = {
					.device = "hotplug_3",
					.info = 1,
				},
			}
		},
	},
	{
		.desc = "CPU2_HOTPLUG_MONITOR",
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
				.lvl_trig = 88000,
				.lvl_clr = 85000,
				.num_actions = 1,
				.actions[0] = {
					.device = "hotplug_2",
					.info = 1,
				},
			}
		},
	},
	{
		.desc = "CPU1_HOTPLUG_MONITOR",
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
				.lvl_trig = 92000,
				.lvl_clr = 85000,
				.num_actions = 1,
				.actions[0] = {
					.device = "hotplug_1",
					.info = 1,
				},
			}
		},
	},
};
#endif

static struct setting_info tm_cfgs_8952[] =
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
		.desc = "CPU3_HOTPLUG_MONITOR",
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
				.lvl_trig = 85000,
				.lvl_clr = 80000,
				.num_actions = 1,
				.actions[0] = {
					.device = "hotplug_3",
					.info = 1,
				},
			}
		},
	},
};

static struct setting_info tm_cfgs_8976[] =
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
};

static struct setting_info tm_cfgs_9607[] =
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

static struct setting_info tm_cfgs_8937[] =
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
};

static struct setting_info tm_cfgs_8937_qrd[] =
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
		.desc = "BATTERY_CHARGING_CTL",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "case_therm",
			.sampling_period_ms = 10000,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = 41000,
				.lvl_clr = 39000,
				.num_actions = 1,
				.actions[0] = {
					.device = "battery",
					.info = 1,
				},
			},
			.t[1] = {
				.lvl_trig = 45000,
				.lvl_clr = 41000,
				.num_actions = 1,
				.actions[0] = {
					.device = "battery",
					.info = 2,
				},
			}
		},
	},
};

static struct setting_info tm_cfgs_8940_qrd[] =
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
		.desc = "BATTERY_CHARGING_CTL",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "case_therm",
			.sampling_period_ms = 10000,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = 41000,
				.lvl_clr = 39000,
				.num_actions = 1,
				.actions[0] = {
					.device = "battery",
					.info = 1,
				},
			},
			.t[1] = {
				.lvl_trig = 45000,
				.lvl_clr = 41000,
				.num_actions = 1,
				.actions[0] = {
					.device = "battery",
					.info = 2,
				},
			}
		},
	},
	{
		.desc = "MODEM_PROC_TEMP_MITIGATION",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "case_therm",
			.sampling_period_ms = 10000,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = 44000,
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
		.desc = "MODEM_PA_MITIGATION",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "case_therm",
			.sampling_period_ms = 10000,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = 48000,
				.lvl_clr = 43000,
				.num_actions = 1,
				.actions[0] = {
					.device = "modem",
					.info = 2,
				},
			},
			.t[1] = {
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

};

#ifndef USE_THERMAL_FRAMEWORK
static struct setting_info tm_cfgs_8953[] =
{
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
};

static struct setting_info tm_cfgs_8953_qrd[] =
{
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
		.desc = "BATTERY_CHARGING_CTL",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "case_therm",
			.sampling_period_ms = 10000,
			.num_thresholds = 4,
			._n_thresholds = 4,
			._n_to_clear = 4,
			._n_actions = 4,
			._n_action_info = 4,
			.t[0] = {
				.lvl_trig = 38000,
				.lvl_clr = 35000,
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
				.lvl_trig = 43000,
				.lvl_clr = 40000,
				.num_actions = 1,
				.actions[0] = {
					.device = "battery",
					.info = 4,
				},
			},
			.t[3] = {
				.lvl_trig = 48000,
				.lvl_clr = 43000,
				.num_actions = 1,
				.actions[0] = {
					.device = "battery",
					.info = 5,
				},
			}
		},
	},
	{
		.desc = "MODEM_PROC_TEMP_MITIGATION",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "case_therm",
			.sampling_period_ms = 10000,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = 45000,
				.lvl_clr = 43000,
				.num_actions = 1,
				.actions[0] = {
					.device = "modem_proc",
					.info = 1,
				},
			},
			.t[1] = {
				.lvl_trig = 54000,
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
		.desc = "MODEM_PA_MITIGATION",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "case_therm",
			.sampling_period_ms = 10000,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = 48000,
				.lvl_clr = 45000,
				.num_actions = 1,
				.actions[0] = {
					.device = "modem",
					.info = 2,
				},
			},
			.t[1] = {
				.lvl_trig = 54000,
				.lvl_clr = 50000,
				.num_actions = 1,
				.actions[0] = {
					.device = "modem",
					.info = 3,
				},
			},
		},
	},
};
#endif

static struct setting_info tm_cfgs_8917[] =
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
};

static struct setting_info tm_cfgs_8917_qrd[] =
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
		.desc = "BATTERY_CHARGING_CTL",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "case_therm",
			.sampling_period_ms = 10000,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = 41000,
				.lvl_clr = 39000,
				.num_actions = 1,
				.actions[0] = {
					.device = "battery",
					.info = 1,
				},
			},
			.t[1] = {
				.lvl_trig = 45000,
				.lvl_clr = 41000,
				.num_actions = 1,
				.actions[0] = {
					.device = "battery",
					.info = 2,
				},
			}
		},
	},
};

static struct setting_info tm_cfgs_8920_qrd[] =
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
		.desc = "BATTERY_CHARGING_CTL",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "case_therm",
			.sampling_period_ms = 10000,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = 41000,
				.lvl_clr = 39000,
				.num_actions = 1,
				.actions[0] = {
					.device = "battery",
					.info = 1,
				},
			},
			.t[1] = {
				.lvl_trig = 45000,
				.lvl_clr = 41000,
				.num_actions = 1,
				.actions[0] = {
					.device = "battery",
					.info = 2,
				},
			}
		},
	},
	{
		.desc = "MODEM_PROC_TEMP_MITIGATION",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "case_therm",
			.sampling_period_ms = 10000,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = 42000,
				.lvl_clr = 38000,
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
		.desc = "MODEM_PA_MITIGATION",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "case_therm",
			.sampling_period_ms = 10000,
			.num_thresholds = 3,
			._n_thresholds = 3,
			._n_to_clear = 3,
			._n_actions = 3,
			._n_action_info = 3,
			.t[0] = {
				.lvl_trig = 43000,
				.lvl_clr = 40000,
				.num_actions = 1,
				.actions[0] = {
					.device = "modem",
					.info = 1,
				},
			},
			.t[1] = {
				.lvl_trig = 45000,
				.lvl_clr = 42000,
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
};

#ifndef USE_THERMAL_FRAMEWORK
static struct setting_info tm_cfgs_8909w[] =
{
	{
		.desc = "MODEM_PA_MITIGATION",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "quiet_therm",
			.sampling_period_ms = 5000,
			.num_thresholds = 2,
			._n_thresholds = 2,
			._n_to_clear = 2,
			._n_actions = 2,
			._n_action_info = 2,
			.t[0] = {
				.lvl_trig = 47000,
				.lvl_clr = 45000,
				.num_actions = 1,
				.actions[0] = {
					.device = "modem",
					.info = 2,
				}
			},
			.t[1] = {
				.lvl_trig = 55000,
				.lvl_clr = 53000,
				.num_actions = 1,
				.actions[0] = {
					.device = "modem",
					.info = 3,
				}
			}
		},
	},
	{
		.desc = "CX_MITIGATION_SKIN_TEMP_CTL",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "quiet_therm",
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
					.device = "venus",
					.info = 1,
				},
				.actions[3] = {
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
					.device = "venus",
					.info = 3,
				},
				.actions[3] = {
					.device = "mdp",
					.info = 3,
				}
			}
		},
	},
	{
		.desc = "CPU1_HOTPLUG_MONITOR",
		.algo_type = MONITOR_ALGO_TYPE,
		.data.tm = {
			.sensor = "quiet_therm",
			.sampling_period_ms = 5000,
			.num_thresholds = 1,
			._n_thresholds = 1,
			._n_to_clear = 1,
			._n_actions = 1,
			._n_action_info = 1,
			.t[0] = {
				.lvl_trig = 49000,
				.lvl_clr = 46000,
				.num_actions = 1,
				.actions[0] = {
					.device = "hotplug_1",
					.info = 1,
				},
			}
		},
	},
};
#endif

void thermal_monitor_init_data(struct thermal_setting_t *setting)
{
	unsigned int i;
	struct setting_info *cfg = NULL;
	unsigned int arr_size = 0;

	switch (therm_get_msm_id()) {
		case THERM_MSM_8916:
			cfg = tm_cfgs_8916;
			arr_size = ARRAY_SIZE(tm_cfgs_8916);
			break;
		case THERM_MSM_8939:
		case THERM_MSM_8936:
		case THERM_MSM_8929:
			cfg = tm_cfgs_8939;
			arr_size = ARRAY_SIZE(tm_cfgs_8939);
			break;
		case THERM_MSM_8909:
#ifndef USE_THERMAL_FRAMEWORK
			switch (therm_get_pmic_model()) {
				case THERM_PMIC_PM8916:
					cfg = tm_cfgs_8909_pm8916;
					arr_size =
					ARRAY_SIZE(tm_cfgs_8909_pm8916);
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
#endif
			break;
		case THERM_MSM_8909W:
#ifndef USE_THERMAL_FRAMEWORK
			if (therm_get_hw_platform_subtype() ==
				THERM_PLATFORM_SUB_WEARABLE_COMPAL) {
				cfg = tm_cfgs_8909w;
				arr_size = ARRAY_SIZE(tm_cfgs_8909w);
			}
#endif
			break;
		case THERM_MSM_8009:
			break;
		case THERM_MSM_8952:
			cfg = tm_cfgs_8952;
			arr_size = ARRAY_SIZE(tm_cfgs_8952);
			break;
		case THERM_MSM_8976:
		case THERM_MSM_8956:
			cfg = tm_cfgs_8976;
			arr_size = ARRAY_SIZE(tm_cfgs_8976);
			break;
		case THERM_MDM_9607:
			cfg = tm_cfgs_9607;
			arr_size = ARRAY_SIZE(tm_cfgs_9607);
			break;
		case THERM_MSM_8937:
			switch (therm_get_hw_platform()) {
			case THERM_PLATFORM_MTP:
				cfg = tm_cfgs_8937;
				arr_size =
				ARRAY_SIZE(tm_cfgs_8937);
				break;
			default:
				cfg = tm_cfgs_8937_qrd;
				arr_size =
				ARRAY_SIZE(tm_cfgs_8937_qrd);
				break;
			}
			break;
		case THERM_MSM_8940:
			switch (therm_get_hw_platform()) {
			case THERM_PLATFORM_MTP:
				cfg = tm_cfgs_8937;
				arr_size =
				ARRAY_SIZE(tm_cfgs_8937);
				break;
			default:
				cfg = tm_cfgs_8940_qrd;
				arr_size =
				ARRAY_SIZE(tm_cfgs_8940_qrd);
				break;
			}
			break;
		case THERM_MSM_8953:
#ifndef USE_THERMAL_FRAMEWORK
			switch (therm_get_hw_platform()) {
			case THERM_PLATFORM_MTP:
				cfg = tm_cfgs_8953;
				arr_size = ARRAY_SIZE(tm_cfgs_8953);
				break;
			default:
				cfg = tm_cfgs_8953_qrd;
				arr_size = ARRAY_SIZE(tm_cfgs_8953_qrd);
				break;
			}
#endif
			break;
		case THERM_MSM_8917:
			switch (therm_get_hw_platform()) {
			case THERM_PLATFORM_MTP:
				cfg = tm_cfgs_8917;
				arr_size = ARRAY_SIZE(tm_cfgs_8917);
				break;
			default:
				cfg = tm_cfgs_8917_qrd;
				arr_size =
				ARRAY_SIZE(tm_cfgs_8917_qrd);
				break;
			}
			break;
		case THERM_MSM_8920:
			switch (therm_get_hw_platform()) {
			case THERM_PLATFORM_MTP:
				cfg = tm_cfgs_8917;
				arr_size = ARRAY_SIZE(tm_cfgs_8917);
				break;
			default:
				cfg = tm_cfgs_8920_qrd;
				arr_size =
				ARRAY_SIZE(tm_cfgs_8920_qrd);
				break;
			}
			break;
		default:
			msg("%s: Uknown Bear family device", __func__);
			break;
	}

	for (i = 0; i < arr_size; i++) {
		add_setting(setting, &cfg[i]);
	}
}
