/*===========================================================================

Copyright (c) 2013,2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/

#include "thermal.h"
#include "thermal_config.h"
#include "speaker_cal.h"

static char *sensor_list[] = {
	"tsens_tz_sensor1",
	"tsens_tz_sensor2",
	"tsens_tz_sensor3",
	"tsens_tz_sensor4",
	"tsens_tz_sensor7",
	"tsens_tz_sensor8",
	"tsens_tz_sensor9",
	"tsens_tz_sensor10",
};

static char *sensor_list_8994[] = {
	"tsens_tz_sensor1",
	"tsens_tz_sensor2",
	"tsens_tz_sensor3",
	"tsens_tz_sensor4",
	"tsens_tz_sensor5",
	"tsens_tz_sensor11",
	"tsens_tz_sensor12",
};

static char *sensor_list_8996[] = {
	"tsens_tz_sensor1",
	"tsens_tz_sensor2",
	"tsens_tz_sensor13",
	"tsens_tz_sensor14",
	"tsens_tz_sensor15",
};

static char *sensor_list_cobalt[] = {
	"tsens_tz_sensor14",
	"tsens_tz_sensor15",
	"tsens_tz_sensor16",
	"tsens_tz_sensor17",
	"tsens_tz_sensor18",
	"tsens_tz_sensor19",
	"tsens_tz_sensor20",
	"tsens_tz_sensor21",
};

static char *sensor_list_8952[] = {
	"tsens_tz_sensor1",
	"tsens_tz_sensor2",
	"tsens_tz_sensor3",
	"tsens_tz_sensor10",
};

#ifndef USE_THERMAL_FRAMEWORK
static char *sensor_list_8953[] = {
	"tsens_tz_sensor1",
	"tsens_tz_sensor2",
	"tsens_tz_sensor3",
	"tsens_tz_sensor14",
	"tsens_tz_sensor15",
};
#endif

static char *sensor_list_MSM_8917[] = {
	"tsens_tz_sensor1",
	"tsens_tz_sensor2",
	"tsens_tz_sensor3",
	"tsens_tz_sensor9",
};

static char *sensor_list_sdm660[] = {
	"tsens_tz_sensor8",
	"tsens_tz_sensor9",
	"tsens_tz_sensor10",
	"tsens_tz_sensor11",
	"tsens_tz_sensor13",
};

static char *sensor_list_sdm630[] = {
	"tsens_tz_sensor8",
	"tsens_tz_sensor9",
	"tsens_tz_sensor10",
	"tsens_tz_sensor11",
};

static struct setting_info cfgs_8974[] =
{
	{
		.desc = "SPEAKER-CAL",
		.algo_type = EQUILIBRIUM_TYPE,
		.data.eq = {
			.sensor_list = sensor_list,
			.list_cnt = ARRAY_SIZE(sensor_list),
			.list_size = 0,
			.sensor = "pm8941_tz",
			.sensor_delta = 10000,
			.sampling_period_ms = 30000,
			.temp_range = 6000,
			.max_temp = 45000,
			.offset = -4000,
			.long_sampling_period_ms = 30000,
			.long_sampling_cnt = 10,
			.long_sample_range = 2000,
			.pre_cal_delay_ms = 1800000,
			},
	},
};

static struct setting_info cfgs_8084[] =
{
        {
                .desc = "SPEAKER-CAL",
                .algo_type = EQUILIBRIUM_TYPE,
                .data.eq = {
                        .sensor_list = sensor_list,
                        .list_cnt = ARRAY_SIZE(sensor_list),
                        .sensor = "pma8084_tz",
                        .sensor_delta = 10000,
                        .sampling_period_ms = 30000,
                        .temp_range = 6000,
                        .max_temp = 45000,
                        .offset = -4000,
                        .long_sampling_period_ms = 30000,
                        .long_sampling_cnt = 10,
                        .long_sample_range = 2000,
                        .pre_cal_delay_ms = 1800000,
                        },
        },
};

static struct setting_info cfgs_8994[] =
{
        {
                .desc = "SPEAKER-CAL",
                .algo_type = EQUILIBRIUM_TYPE,
                .data.eq = {
                        .sensor_list = sensor_list_8994,
                        .list_cnt = ARRAY_SIZE(sensor_list_8994),
                        .sensor = "pm8994_tz",
                        .sensor_delta = 10000,
                        .sampling_period_ms = 30000,
                        .temp_range = 6000,
                        .max_temp = 45000,
                        .offset = -4000,
                        .long_sampling_period_ms = 30000,
                        .long_sampling_cnt = 10,
                        .long_sample_range = 2000,
                        .pre_cal_delay_ms = 1800000,
                        },
        },
};

static struct setting_info cfgs_8996[] =
{
        {
                .desc = "SPEAKER-CAL",
                .algo_type = EQUILIBRIUM_TYPE,
                .data.eq = {
                        .sensor_list = sensor_list_8996,
                        .list_cnt = ARRAY_SIZE(sensor_list_8996),
                        .sensor = "pm8994_tz",
                        .sensor_delta = 10000,
                        .sampling_period_ms = 30000,
                        .temp_range = 6000,
                        .max_temp = 45000,
                        .offset = -4000,
                        .long_sampling_period_ms = 30000,
                        .long_sampling_cnt = 10,
                        .long_sample_range = 2000,
                        .pre_cal_delay_ms = 1800000,
                        },
        },
};

static struct setting_info cfgs_cobalt[] =
{
        {
                .desc = "SPEAKER-CAL",
                .algo_type = EQUILIBRIUM_TYPE,
                .data.eq = {
                        .sensor_list = sensor_list_cobalt,
                        .list_cnt = ARRAY_SIZE(sensor_list_cobalt),
                        .sensor = "pm8998_tz",
                        .sensor_delta = 10000,
                        .sampling_period_ms = 30000,
                        .temp_range = 6000,
                        .max_temp = 45000,
                        .offset = -4000,
                        .long_sampling_period_ms = 30000,
                        .long_sampling_cnt = 10,
                        .long_sample_range = 2000,
                        .pre_cal_delay_ms = 1800000,
                        },
        },
};

static struct setting_info cfgs_8952[] =
{
	{
		.desc = "SPEAKER-CAL",
		.algo_type = EQUILIBRIUM_TYPE,
		.data.eq = {
			.sensor_list = sensor_list_8952,
			.list_cnt = ARRAY_SIZE(sensor_list_8952),
			.sensor = "pm8950_tz",
			.sensor_delta = 10000,
			.sampling_period_ms = 30000,
			.temp_range = 6000,
			.max_temp = 45000,
			.offset = -4000,
			.long_sampling_period_ms = 30000,
			.long_sampling_cnt = 10,
			.long_sample_range = 2000,
			.pre_cal_delay_ms = 1800000,
			},
	},
};

#ifndef USE_THERMAL_FRAMEWORK
static struct setting_info cfgs_8953[] =
{
	{
		.desc = "SPEAKER-CAL",
		.algo_type = EQUILIBRIUM_TYPE,
		.data.eq = {
			.sensor_list = sensor_list_8953,
			.list_cnt = ARRAY_SIZE(sensor_list_8953),
			.sensor = "pm8953_tz",
			.sensor_delta = 10000,
			.sampling_period_ms = 30000,
			.temp_range = 6000,
			.max_temp = 45000,
			.offset = -4000,
			.long_sampling_period_ms = 30000,
			.long_sampling_cnt = 10,
			.long_sample_range = 2000,
			.pre_cal_delay_ms = 1800000,
		},
	},
};
#endif

static struct setting_info cfgs_8937[] =
{
	{
		.desc = "SPEAKER-CAL",
		.algo_type = EQUILIBRIUM_TYPE,
		.data.eq = {
			.sensor_list = sensor_list_8952,
			.list_cnt = ARRAY_SIZE(sensor_list_8952),
			.sensor = "pm8937_tz",
			.sensor_delta = 10000,
			.sampling_period_ms = 30000,
			.temp_range = 6000,
			.max_temp = 45000,
			.offset = -4000,
			.long_sampling_period_ms = 30000,
			.long_sampling_cnt = 10,
			.long_sample_range = 2000,
			.pre_cal_delay_ms = 1800000,
			},
	},
};

static struct setting_info cfgs_MSM_8917[] =
{
	{
		.desc = "SPEAKER-CAL",
		.algo_type = EQUILIBRIUM_TYPE,
		.data.eq = {
			.sensor_list = sensor_list_MSM_8917,
			.list_cnt = ARRAY_SIZE(sensor_list_MSM_8917),
			.sensor = "pm8937_tz",
			.sensor_delta = 10000,
			.sampling_period_ms = 30000,
			.temp_range = 6000,
			.max_temp = 45000,
			.offset = -4000,
			.long_sampling_period_ms = 30000,
			.long_sampling_cnt = 10,
			.long_sample_range = 2000,
			.pre_cal_delay_ms = 1800000,
			},
	},
};

static struct setting_info cfgs_sdm660[] =
{
	{
		.desc = "SPEAKER-CAL",
		.algo_type = EQUILIBRIUM_TYPE,
		.data.eq = {
			.sensor_list = sensor_list_sdm660,
			.list_cnt = ARRAY_SIZE(sensor_list_sdm660),
			.sensor = "pm660_tz",
			.sensor_delta = 10000,
			.sampling_period_ms = 30000,
			.temp_range = 6000,
			.max_temp = 45000,
			.offset = -4000,
			.long_sampling_period_ms = 30000,
			.long_sampling_cnt = 10,
			.long_sample_range = 2000,
			.pre_cal_delay_ms = 1800000,
		},
	},
};

static struct setting_info cfgs_sdm630[] =
{
	{
		.desc = "SPEAKER-CAL",
		.algo_type = EQUILIBRIUM_TYPE,
		.data.eq = {
			.sensor_list = sensor_list_sdm630,
			.list_cnt = ARRAY_SIZE(sensor_list_sdm630),
			.sensor = "pm660_tz",
			.sensor_delta = 10000,
			.sampling_period_ms = 30000,
			.temp_range = 6000,
			.max_temp = 45000,
			.offset = -4000,
			.long_sampling_period_ms = 30000,
			.long_sampling_cnt = 10,
			.long_sample_range = 2000,
			.pre_cal_delay_ms = 1800000,
		},
	},
};

void speaker_cal_init_data(struct thermal_setting_t *setting)
{
	int i;
	int arr_size = 0;

	struct setting_info *cfg = NULL;

	switch (therm_get_msm_id()) {
		case THERM_MSM_8974:
		case THERM_MSM_8974PRO_AA:
		case THERM_MSM_8974PRO_AB:
		case THERM_MSM_8974PRO_AC:
			cfg = cfgs_8974;
			arr_size = ARRAY_SIZE(cfgs_8974);
			break;
		case THERM_MSM_8084:
		case THERM_MSM_8x62:
			cfg = cfgs_8084;
			arr_size = ARRAY_SIZE(cfgs_8084);
			break;
		case THERM_MSM_8994:
		case THERM_MSM_8992:
			cfg = cfgs_8994;
			arr_size = ARRAY_SIZE(cfgs_8994);
			break;
		case THERM_MSM_8996:
		case THERM_MSM_8996AU:
			cfg = cfgs_8996;
			arr_size = ARRAY_SIZE(cfgs_8996);
			break;
		case THERM_MSM_8952:
		case THERM_MSM_8976:
		case THERM_MSM_8956:
			cfg = cfgs_8952;
			arr_size = ARRAY_SIZE(cfgs_8952);
			break;
		case THERM_MSM_8909:
			break;
		case THERM_MSM_COBALT:
			cfg = cfgs_cobalt;
			arr_size = ARRAY_SIZE(cfgs_cobalt);
			break;
		case THERM_MSM_8953:
#ifndef USE_THERMAL_FRAMEWORK
			cfg = cfgs_8953;
			arr_size = ARRAY_SIZE(cfgs_8953);
#endif
			break;
		case THERM_MSM_8937:
		case THERM_MSM_8940:
			cfg = cfgs_8937;
			arr_size = ARRAY_SIZE(cfgs_8937);
			break;
		case THERM_MSM_8917:
		case THERM_MSM_8920:
			cfg = cfgs_MSM_8917;
			arr_size = ARRAY_SIZE(cfgs_MSM_8917);
			break;
		case THERM_SDM_660:
			cfg = cfgs_sdm660;
			arr_size = ARRAY_SIZE(cfgs_sdm660);
			break;
		case THERM_SDM_630:
			cfg = cfgs_sdm630;
			arr_size = ARRAY_SIZE(cfgs_sdm630);
			break;
		default:
			msg("%s: ERROR Uknown device\n", __func__);
			break;
	}

	for (i = 0; i < arr_size; i++) {
		add_setting(setting, &cfg[i]);
	}
}
