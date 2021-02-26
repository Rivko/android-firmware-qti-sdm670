#pragma once
/*******************************************************************************
 * Copyright (c) 2017, Semtech
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     3. Neither the name of Semtech nor the
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


#include <stdint.h>
#include "sns_sensor_instance.h"
#include "sns_data_stream.h"
#include "sns_stream_service.h"

struct sns_stream_service;
struct sns_data_stream;
struct sx931x_instance_state;

typedef enum
{
    IDLE,
    STREAM_STARTING,
    STREAMING,
    STREAM_STOPPING,
} sx931x_dae_if_state;

typedef struct
{
    struct sns_data_stream *stream;
    const char             *nano_hal_vtable_name;
    sx931x_dae_if_state    state;
    bool                   stream_usable;
    bool                   flushing_hw;
    bool                   flushing_data;
} sx931x_dae_stream;

typedef struct sx931x_dae_if_info
{
    sx931x_dae_stream   sar;
    sx931x_dae_stream   temp;
} sx931x_dae_if_info;

bool sx931x_dae_if_available(sns_sensor_instance *this);

sns_rc sx931x_dae_if_init(sns_sensor_instance *const this, struct sns_stream_service  *stream_mgr, sns_sensor_uid *dae_suid);

void sx931x_dae_if_deinit(struct sx931x_instance_state *state, struct sns_stream_service     *stream_mgr);

bool sx931x_dae_if_stop_streaming(sns_sensor_instance *this);

bool sx931x_dae_if_start_streaming(sns_sensor_instance *this);

bool sx931x_dae_if_flush_hw(sns_sensor_instance *this);

bool sx931x_dae_if_flush_samples(sns_sensor_instance *this);

void sx931x_dae_if_process_events(sns_sensor_instance *this);

