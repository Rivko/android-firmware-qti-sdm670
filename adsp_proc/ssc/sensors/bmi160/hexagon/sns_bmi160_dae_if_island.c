/*******************************************************************************
 * Copyright (c) 2017, Bosch Sensortec GmbH
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     3. Neither the name of Bosch Sensortec GmbH nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/

/**
 * @file sns_bmi160_dae_if.c
 *
 * LSM6DS3 - DAE sensor interface
 *
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 **/

#include "sns_mem_util.h"
#include "sns_service_manager.h"
#include "sns_stream_service.h"
#include "sns_rc.h"
#include "sns_request.h"
#include "sns_time.h"
#include "sns_sensor_event.h"
#include "sns_types.h"

#include "sns_bmi160_hal.h"
#include "sns_bmi160_sensor.h"
#include "sns_bmi160_sensor_instance.h"

bool bmi160_dae_if_available(sns_sensor_instance *this)
{
    UNUSED_VAR(this);

    return false;
}

sns_rc bmi160_dae_if_init(
  sns_sensor_instance           *const this,
  struct sns_stream_service     *stream_mgr,
  sns_sensor_uid                *dae_suid
  )
{
    UNUSED_VAR(this);
    UNUSED_VAR(stream_mgr);
    UNUSED_VAR(dae_suid);


    return SNS_RC_SUCCESS;
}

void bmi160_dae_if_deinit(
  struct bmi160_instance_state  *istate,
  struct sns_stream_service     *stream_mgr)
{
    UNUSED_VAR(istate);
    UNUSED_VAR(stream_mgr);
}

bool bmi160_dae_if_start_streaming(sns_sensor_instance *this)
{
    UNUSED_VAR(this);

    return false;
}

bool bmi160_dae_if_stop_streaming(sns_sensor_instance *this)
{
    UNUSED_VAR(this);

    return false;
}


bool bmi160_dae_if_flush_samples(sns_sensor_instance *this)
{
    UNUSED_VAR(this);
    return false;
}

void bmi160_dae_if_process_events(sns_sensor_instance *this)
{
    UNUSED_VAR(this);
}

