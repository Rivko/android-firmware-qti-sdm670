/**
 * @file:  cpr_types.h
 * @brief:
 *
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 *
 * $DateTime: 2017/09/18 09:26:13 $
 * $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/CPRLib/common/inc/cpr_types.h#1 $
 * $Change: 14402555 $
 */
#ifndef CPR_TYPES_H
#define CPR_TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include "cpr_defs.h"
#include "cpr_image_defs.h"

typedef enum
{
    CPR_FOUNDRY_TSMC,
    CPR_FOUNDRY_GF,
    CPR_FOUNDRY_SS,
    CPR_FOUNDRY_IBM,
    CPR_FOUNDRY_UMC,
    CPR_FOUNDRY_ANY
} cpr_foundry_id;

typedef enum {
    CPR_THERMAL_REGION_NORMAL,
    CPR_THERMAL_REGION_COOL,
    CPR_THERMAL_REGION_COLD,
    CPR_THERMAL_REGION_CIRITICAL_COLD,
    CPR_THERMAL_REGION_MAX
} cpr_thermal_region;

typedef enum {
    CPR_MARGIN_ADJUSTMENT_AGING,
    CPR_MARGIN_ADJUSTMENT_THERMAL,
    CPR_MARGIN_ADJUSTMENT_TEST,
    CPR_MARGIN_ADJUSTMENT_MAX
} cpr_margin_adjustment_source;

typedef enum {
    CPR_MEASUREMENT_TYPE_SETTLE,
    CPR_MEASUREMENT_TYPE_AGING,
    CPR_MEASUREMENT_TYPE_MAX
} cpr_measurement_type;

typedef enum {
    CPR_CONTROL_NONE,
    CPR_CONTROL_OPEN_LOOP,
    CPR_CONTROL_SW_CLOSED_LOOP,
    CPR_CONTROL_HW_OPEN_LOOP,
    CPR_CONTROL_HW_CLOSED_LOOP
} cpr_control_mode;

typedef enum {
    CPR_CONTROLLER_TYPE_SW_CL_ONLY,
    CPR_CONTROLLER_TYPE_HW_CL_ONLY,
    CPR_CONTROLLER_TYPE_HW_CL_CAPABLE /* APPS CPR only */
} cpr_controller_type;

typedef struct
{
    uint8  ro;
    uint16 kv;
} cpr_kv;

typedef struct
{
    uint8  ro;
    uint16 quotient;
} cpr_quotient;

typedef struct
{
    boolean busy;
    boolean up;
    boolean down;
    uint8   steps;
    uint8   selMax;
    uint8   selMin;
    uint16  quotMax;
    uint16  quotMin;
} cpr_results;

#endif
