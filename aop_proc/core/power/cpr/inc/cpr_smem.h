/**
 * @file:  cpr_smem.h
 * @brief:
 *
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 *
 * $DateTime: 2018/02/16 04:48:18 $
 * $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/power/cpr/inc/cpr_smem.h#1 $
 * $Change: 15485400 $
 */
#ifndef CPR_SMEM_H
#define CPR_SMEM_H

#include "cpr_cfg.h"
#include "cpr_data.h"
#include "cpr_logs.h"
#include "cpr_image_defs.h"

//******************************************************************************
// V2 Defines
//******************************************************************************
typedef struct __attribute__ ((__packed__))
{
    uint32 mode;  // cpr_voltage_mode
    int16  decodedFuseSteps;
    int16  decodedFuseOffset;
    uint16 enableCount;
    uint16 targetsCount;
    uint16 submodesCount;
    int16  totalMarginMV;
} cpr_smem_mode_state;

typedef struct __attribute__ ((__packed__))
{
    uint16  size;              // in bytes
    uint32  id;                // cpr_rail_id
    uint32  controlMode;       // cpr_control_mode
    int32   marginAdjustments[CPR_MARGIN_ADJUSTMENT_MAX];
    int16   decodedFuseAging;
    int8    hasVPdata;         // Flag to indicate prsence of complete VP for rail.
    uint32  activeVoltageMode; // cpr_voltage_mode
    uint8   modeStateCount;
} cpr_smem_rail_hdr;

typedef struct __attribute__ ((__packed__))
{
    uint8   version;
    uint8   railCount;
    uint16  dataSize;       // Data size written to SMEM to get offset.
    uint16  smemSize;       // SMEM buffer size
} cpr_smem_hdr;


void cpr_smem_deserialize_config(cpr_rail* rail, cpr_rail_state* state);

#endif
