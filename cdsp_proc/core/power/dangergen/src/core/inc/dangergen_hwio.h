/*
* Copyright (c) 2013-2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * dangergen_hwio.h
 *
 *  Created on: Jan 22, 2015
 *      Author: kingt
 */

#ifndef DANGERGEN_HWIO_H
#define DANGERGEN_HWIO_H

#include "dangergen_internal.h"

void dangergen_hwio_initialize_module(void);
void dangergen_hwio_initialize(dangergen_generator_type* generator);
void dangergen_hwio_configure(dangergen_generator_type* generator);
void dangergen_hwio_enable(dangergen_generator_type* generator);
void dangergen_hwio_disable(dangergen_generator_type* generator);
void dangergen_hwio_subtract(dangergen_generator_type* generator);
void dangergen_hwio_reset(dangergen_generator_type* generator);
void dangergen_hwio_set_qos_ctl(bool enabled);
void dangergen_hwio_set_qos_noc(bool enabled);
void dangergen_hwio_interrupt_configure(dangergen_generator_type* generator);
bool dangergen_hwio_interrupt_select_mode_status (uint32 ver);
bool dangergen_hwio_hw_enable_mode_status (uint32 ver);
void dangergen_hwio_switch_mode(dangergen_generator_type* generator, dangergen_generator_enable_mode mode);

#endif

