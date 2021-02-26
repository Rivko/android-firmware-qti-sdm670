/*========================================================================
 *//** @file stream_dev_drift_calc.cpp
This file contains functions for Stream to Device Drift Calculation Library

Copyright (c) 2013, 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
  *//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/utils/stream_dev_drift_calc/lib/src/stream_dev_drift_calc.cpp#1 $


when       who     what, where, why
--------   ---     -------------------------------------------------------
10/31/13    DG      Created file.
========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "stream_dev_drift_calc.h"
#ifndef TST_STREAM_DEV_DRIFT_CALC
#include "qurt_elite.h"
#include "AFEAvtDrift.h"
#endif
#include <stdlib.h>

/* =======================================================================
Structures
========================================================================== */
struct stream_to_avt_t
{
	int32_t drift;
	uint64_t expected_ts;
	bool_t sync_to_ts_required;
};

struct dev_to_avt_t
{
	int32_t current_drift_value;
	int32_t initial_drift_value;
	uint32_t prev_frame_count;
	const volatile uint64_t *dev_drift_ptr;
};

struct sd_dc_t
{
	volatile int32_t drift_val;
	stream_to_avt_t str_drift_obj;
	dev_to_avt_t dev_drift_obj;
};

/* =======================================================================
FUNCTIONS
========================================================================== */
// Functions for stream_to_avt_t
static void s_to_avt_init(stream_to_avt_t *obj_ptr);
static void s_to_avt_deinit(stream_to_avt_t *obj_ptr);
static void s_to_avt_calc(stream_to_avt_t *obj_ptr, uint64_t ts, uint32_t duration, bool_t *reset_needed_ptr);
static int32_t s_to_avt_get_drift(stream_to_avt_t *obj_ptr);
static void s_to_avt_reset(stream_to_avt_t *obj_ptr);

// Functions for dev_to_avt_t
static void d_to_avt_init(dev_to_avt_t *obj_ptr);
static void d_to_avt_deinit(dev_to_avt_t *obj_ptr);
static void d_to_avt_calc(dev_to_avt_t *obj_ptr, bool_t *reset_needed_ptr); // When reset_needed_ptr is returned TRUE, the stream side object must be reset.
static void d_to_avt_set_drift_ptr(dev_to_avt_t *obj_ptr, const volatile uint64_t *drift_ptr);
static int32_t d_to_avt_get_drift(dev_to_avt_t *obj_ptr);
static void d_to_avt_reset(dev_to_avt_t *obj_ptr);

sd_dc_t* sd_drift_calc_create(void)
{
	MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "SD Drift Calc: Creating object.");

	sd_dc_t *obj_ptr = (sd_dc_t*)qurt_elite_memory_malloc(sizeof(sd_dc_t), QURT_ELITE_HEAP_DEFAULT);
	if (NULL == obj_ptr)
	{
		MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "SD Drift Calc: Failed to allocate memory for object.");
		return NULL;
	}

	obj_ptr->drift_val = 0;
	s_to_avt_init(&obj_ptr->str_drift_obj);
	d_to_avt_init(&obj_ptr->dev_drift_obj);


	return obj_ptr;
}

void sd_drift_calc_destroy(sd_dc_t* obj_ptr)
{
	MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "SD Drift Calc: Destroying object.");

	s_to_avt_deinit(&obj_ptr->str_drift_obj);
	d_to_avt_deinit(&obj_ptr->dev_drift_obj);
	qurt_elite_memory_free(obj_ptr);
}

volatile const int32_t *sd_drift_calc_get_drift_pointer(sd_dc_t *obj_ptr)
{
	return &obj_ptr->drift_val;
}

void sd_drift_calc_set_device_drift_pointer(sd_dc_t* obj_ptr, const volatile uint64_t* dev_drift_ptr)
{
	MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "SD Drift Calc: Setting device drift pointer.");

	d_to_avt_set_drift_ptr(&obj_ptr->dev_drift_obj, dev_drift_ptr);
	s_to_avt_reset(&obj_ptr->str_drift_obj);
	obj_ptr->drift_val = s_to_avt_get_drift(&obj_ptr->str_drift_obj) - d_to_avt_get_drift(&obj_ptr->dev_drift_obj);
}

void sd_drift_calc_update_drift(sd_dc_t* obj_ptr, uint64_t ts, uint32_t duration_in_us)
{
	bool_t reset_needed = FALSE;
	d_to_avt_calc(&obj_ptr->dev_drift_obj, &reset_needed);
	if (reset_needed)
	{
		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "SD Drift Calc: Resetting drift because of clock discontinuity on device.");
		s_to_avt_reset(&obj_ptr->str_drift_obj);
	}

	reset_needed = FALSE;
	s_to_avt_calc(&obj_ptr->str_drift_obj, ts, duration_in_us, &reset_needed);
	if (reset_needed)
	{
		MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "SD Drift Calc: Resetting drift because of timestamp discontinuity on stream.");
		d_to_avt_reset(&obj_ptr->dev_drift_obj);
	}

	obj_ptr->drift_val = s_to_avt_get_drift(&obj_ptr->str_drift_obj) - d_to_avt_get_drift(&obj_ptr->dev_drift_obj);
}

void sd_drift_update_given_drift(sd_dc_t* obj_ptr, int32_t drift_val)
{

    if(obj_ptr)
    {
        MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "SD Drift Calc: Updating drift value to %d", drift_val);
        obj_ptr->drift_val = drift_val;
    }
}

static void s_to_avt_init(stream_to_avt_t* obj_ptr)
{
	s_to_avt_reset(obj_ptr);
}

static void s_to_avt_deinit(stream_to_avt_t* obj_ptr)
{
	s_to_avt_reset(obj_ptr);
}

static int32_t i32_subtract_u64_u64(uint64_t x, uint64_t y)
{
	if (x >= y)
	{
		uint64_t abs_diff = x - y;
		if (abs_diff > INT32_MAX)
		{
			return INT32_MAX;
		}
		else
		{
			return static_cast<int32_t>(abs_diff);
		}
	}
	else
	{
		uint64_t abs_diff = y - x;
		if (abs_diff >= -static_cast<int64_t>(INT32_MIN))
		{
			return INT32_MIN;
		}
		else
		{
			QURT_ELITE_ASSERT(abs_diff <= INT32_MAX);
			return -static_cast<int32_t>(abs_diff);
		}
	}
}

static void s_to_avt_calc(stream_to_avt_t* obj_ptr, uint64_t ts, uint32_t duration, bool_t *reset_needed_ptr)
{
	if (obj_ptr->sync_to_ts_required)
	{
		obj_ptr->expected_ts = ts + duration;
		obj_ptr->sync_to_ts_required = FALSE;
		obj_ptr->drift = 0;
		*reset_needed_ptr = FALSE;
	}
	else
	{
		// Check for timestamp discontinuity.
		const int32_t JITTER_MARGIN = 750; // us
		int32_t drift = i32_subtract_u64_u64(obj_ptr->expected_ts, ts);

		if ((drift >= JITTER_MARGIN) || (drift <= -JITTER_MARGIN))
		{
			obj_ptr->expected_ts = ts + duration;
			obj_ptr->sync_to_ts_required = FALSE;
			obj_ptr->drift = 0;
			*reset_needed_ptr = TRUE;
		}
		else
		{
			obj_ptr->drift += drift;
			obj_ptr->expected_ts = ts + duration;
		}
	}
}

static int32_t s_to_avt_get_drift(stream_to_avt_t* obj_ptr)
{
	return obj_ptr->drift;
}

static void s_to_avt_reset(stream_to_avt_t* obj_ptr)
{
	obj_ptr->drift = 0;
	obj_ptr->expected_ts = 0;
	obj_ptr->sync_to_ts_required = TRUE;
}

static void d_to_avt_sync(dev_to_avt_t* obj_ptr)
{
	if (NULL != obj_ptr->dev_drift_ptr)
	{
		afe_client_avt_drift_info_t drift_info;
		afe_port_read_avt_drift(obj_ptr->dev_drift_ptr, &drift_info);

		obj_ptr->current_drift_value = 0;
		obj_ptr->initial_drift_value = drift_info.acc_avt_drift_us;
		obj_ptr->prev_frame_count = drift_info.frame_counter;
	}
}

static void d_to_avt_init(dev_to_avt_t* obj_ptr)
{
	obj_ptr->current_drift_value = 0;
	obj_ptr->initial_drift_value = 0;
	obj_ptr->prev_frame_count = 0;
	obj_ptr->dev_drift_ptr = NULL;
}

static void d_to_avt_deinit(dev_to_avt_t* obj_ptr)
{
	obj_ptr->current_drift_value = 0;
	obj_ptr->initial_drift_value = 0;
	obj_ptr->prev_frame_count = 0;
	obj_ptr->dev_drift_ptr = NULL;
}

static void d_to_avt_calc(dev_to_avt_t* obj_ptr, bool_t* reset_needed_ptr)
{
	*reset_needed_ptr = FALSE;
	if (NULL != obj_ptr->dev_drift_ptr)
	{
		afe_client_avt_drift_info_t drift_info;
		afe_port_read_avt_drift(obj_ptr->dev_drift_ptr, &drift_info);

		bool_t frame_counter_jumped = ((drift_info.frame_counter >= obj_ptr->prev_frame_count + 5)
				|| (drift_info.frame_counter < obj_ptr->prev_frame_count)) ? TRUE : FALSE;
		if (frame_counter_jumped)
		{
			*reset_needed_ptr = TRUE;
			d_to_avt_sync(obj_ptr);
		}
		else
		{
			obj_ptr->current_drift_value = drift_info.acc_avt_drift_us - obj_ptr->initial_drift_value;
			obj_ptr->prev_frame_count = drift_info.frame_counter;
		}
	}
}

static void d_to_avt_set_drift_ptr(dev_to_avt_t* obj_ptr, const volatile uint64_t* drift_ptr)
{
	obj_ptr->dev_drift_ptr = drift_ptr;
	d_to_avt_sync(obj_ptr);
}

static int32_t d_to_avt_get_drift(dev_to_avt_t* obj_ptr)
{
	// Invert the sign because we want the dev clock - av timer clock value
	return -obj_ptr->current_drift_value;
}

static void d_to_avt_reset(dev_to_avt_t* obj_ptr)
{
	d_to_avt_sync(obj_ptr);
}
