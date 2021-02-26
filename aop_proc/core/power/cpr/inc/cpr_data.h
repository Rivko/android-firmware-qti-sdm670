/**
 * @file:  cpr_data.h
 * @brief:
 *
 * Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 *
 * $DateTime: 2018/02/16 04:48:18 $
 * $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/power/cpr/inc/cpr_data.h#1 $
 * $Change: 15485400 $
 */
#ifndef CPR_DATA_H
#define	CPR_DATA_H

#include "cpr_types.h"
#include "cpr_cfg.h"

typedef struct
{
    uint16 floor;
    uint16 ceiling;
    uint16 up;
    uint16 down;
    uint16 ignored;
} cpr_debug_info;

typedef struct {
    uint32 freq;
    uint32 ceiling;
    uint32 floor;
    uint32 current;
    uint32 min;
    cpr_debug_info debug;
} cpr_submode_settings;

typedef struct
{
    cpr_voltage_mode       mode;

    uint8                  targetsCount;

    int16                  decodedFuseSteps;
    int16                  decodedFuseOffset;
    int16                  totalMarginMV;

    cpr_submode_settings*  activeSubMode;

    uint32                 enableCount;

    uint32                 subModesCount;
    cpr_submode_settings   *subModes;

    cpr_quotient*          targets;
} cpr_mode_settings;

typedef struct cpr_aging_cfg
{
    cpr_voltage_mode   modeToRun;
    uint8              sensorID;
    uint8              bypassSensorIDsCount;
    uint8              fusePackingFactor; /* used to multiply the final fuse value by this value */
    uint32             kv; /* x100 */
    uint32             scalingFactor; /* x10 */
    int32              marginLimit;
    uint8              *bypassSensorIDs;
} cpr_aging_cfg;

typedef struct
{
    cpr_domain_id       id;
    cpr_control_mode    cMode;
    uint8               modeSettingsCount;
    int32               marginAdjustments[CPR_MARGIN_ADJUSTMENT_MAX];
    int32               decodedFuseAging;
    uint32              disableVotes;
    cpr_voltage_mode    mode;
    cpr_mode_settings*  activeMode;
    cpr_mode_settings*  previousMode;
    cpr_mode_settings*  modeSettings;
} cpr_rail_state;

/*
 * Struct for all VP info and Enablement Config Info in auto-generated file.
 */
typedef struct
{
    const char                        *hashValue;
    uint8                              railsCount;
} cpr_settings;

typedef struct
{
    boolean              dataInit; /* set up config and global ceiling/floor voltages */
    boolean              cfgInit; /* completed init function */
    boolean              fullInit; /* completed enable function */
    boolean              processingIsr;
    cpr_foundry_id       foundry;
    uint32               cprRev;
    uint32               chipVersion;
    cpr_rail_state      *railStates;
} cpr_information;


extern cpr_information cpr_info; // defined in cpr.c

#endif
