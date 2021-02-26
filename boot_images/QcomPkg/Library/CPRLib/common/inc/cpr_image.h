/**
 * @file:  cpr_image.h
 * @brief:
 *
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 *
 * $DateTime: 2017/09/18 09:26:13 $
 * $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/CPRLib/common/inc/cpr_image.h#1 $
 * $Change: 14402555 $
 */
#ifndef CPR_IMAGE_H
#define CPR_IMAGE_H

#include "cpr_types.h"
#include "cpr_data.h"
#include "cpr_image_defs.h"

#ifndef CPR_IMAGE_LOCK_CORE
    #define CPR_IMAGE_LOCK_CORE(...)
    #define CPR_IMAGE_UNLOCK_CORE(...)
#endif

typedef void (*cpr_image_isr)(void* ctx);
typedef void (*cpr_therm_cb)(cpr_thermal_region region);

void cpr_image_register_thermal_cb(cpr_therm_cb cb);
void cpr_image_register_isr(cpr_domain_id railId, uint32 interrupt, cpr_image_isr isr, void* ctx);
uint32 cpr_image_get_chip_version(void);
cpr_foundry_id cpr_image_get_foundry(void);
void cpr_image_enable_clock(const char* clkId);
boolean cpr_image_set_rail_mode(cpr_domain_id railId, cpr_domain_info* info);
boolean cpr_image_rail_transition_voltage(cpr_domain_id railId);
void* cpr_image_malloc(uint32 size);
void cpr_image_free(void* buf);
void cpr_image_memscpy(void* dst, const void* src, uint32 size);
void cpr_image_open_remote_cfg(void** cfg, uint32* size);
void cpr_image_close_remote_cfg(void);
void cpr_image_wait(uint32 us);

uint32 cpr_image_get_eldo_voltage(cpr_domain_id railId, cpr_domain_info* info);
void cpr_image_enter_sleep(void);
void cpr_image_exit_sleep(void);
boolean cpr_image_can_resume_control(cpr_domain_id railId);

void cpr_image_enable_measurements(cpr_rail* rail, cpr_measurement_type type);
void cpr_image_disable_measurements(cpr_rail* rail, cpr_measurement_type type);

// DO NOT call this function. Use cpr_utils_get_settings() instead.
const cpr_settings* cpr_image_get_settings(void);
#endif
