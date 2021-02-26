/**
 * @file:  cpr_image_target.h
 * @brief: image-specific and target-specific CPR functions.
 *
 * Copyright (c) 2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
 *
 * $DateTime: 2018/07/30 01:21:36 $
 * $Header: //components/rel/core.qdsp6/2.1.c4/power/cpr/image/cdsp/inc/cpr_image_target.h#1 $
 * $Change: 16759227 $
 */
#ifndef CPR_IMAGE_TARGET_H
#define CPR_IMAGE_TARGET_H

#include "cpr.h"
#include "cpr_types.h"
#include "cpr_data.h"

#include "voltage_level.h"

const cpr_settings* cpr_image_target_get_settings(void);

boolean cpr_image_target_is_turbo_mode_only(void);

/**
*
* <!-- cpr_image_target_get_ldo_voltage -->
*
* @brief Calculate the LDO voltage for a corner
*
* @param info : pointer to the CPR domain info specifing the corner/mode (SVS, NOM, etc)
*
* @return eLDO voltage
*
*/
uint32 cpr_image_target_get_ldo_voltage(cpr_domain_info* info);

/**
*
* <!-- cpr_image_target_get_foundry -->
*
* @brief Get the foundry and return a foundry enum that our CPR driver understands
*
*/
cpr_foundry_id cpr_image_target_get_foundry(void);

/**
*
* <!-- cpr_image_target_vlvl_corner_to_cpr_mode -->
*
* @brief Converts vlvl corner enum to CPR voltage mode enum
*
*/
cpr_voltage_mode cpr_image_target_vlvl_corner_to_cpr_mode(rail_voltage_level corner);

/**
*
* <!-- cpr_image_target_cpr_mode_to_vlvl_corner -->
*
* @brief Converts CPR voltage mode enum to vlvl corner enum
*
*/
rail_voltage_level cpr_image_target_cpr_mode_to_vlvl_corner(cpr_voltage_mode mode);

#endif /* CPR_IMAGE_TARGET_H */
