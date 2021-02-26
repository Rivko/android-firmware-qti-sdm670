/**
 * @file:  cpr_image_target.h
 * @brief:
 *
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 *
 * $DateTime: 2017/12/19 03:28:31 $
 * $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/CPRLib/image/core/inc/cpr_image_target.h#2 $
 * $Change: 15088669 $
 */
#ifndef CPR_IMAGE_TARGET_H
#define CPR_IMAGE_TARGET_H

#include "cpr_data.h"
#include "cpr_defs.h"
#include "railway.h"
#include "voltage_level.h"

const cpr_settings* cpr_image_target_get_settings(void);

boolean cpr_image_target_is_turbo_mode_only(void);

cpr_domain_id cpr_image_target_railway_rail_to_cpr_domain(int railway_rail);
railway_corner cpr_image_target_cpr_mode_to_railway_corner(cpr_voltage_mode mode);
cpr_voltage_mode cpr_image_target_railway_corner_to_cpr_mode(railway_corner corner);

rail_voltage_level cpr_image_target_cpr_mode_to_vlvl(cpr_voltage_mode mode);
void cpr_image_target_railway_rail_to_arc_domain(cpr_domain_id railId, char **res_name);

void cpr_image_target_enable_measurements(cpr_rail* rail, cpr_measurement_type type);
void cpr_image_target_disable_measurements(cpr_rail* rail, cpr_measurement_type type);

#endif /*CPR_IMAGE_TARGET_H*/
