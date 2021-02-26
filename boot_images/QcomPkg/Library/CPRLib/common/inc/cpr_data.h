/**
 * @file:  cpr_data.h
 * @brief:
 *
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 *
 * $DateTime: 2017/09/18 09:26:13 $
 * $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/CPRLib/common/inc/cpr_data.h#1 $
 * $Change: 14402555 $
 */
#ifndef CPR_DATA_H
#define	CPR_DATA_H

#include "cpr_types.h"
#include "cpr_cfg.h"
#include "cpr_voltage_plan.h"
#include "cpr_logs.h"

#define CPR_DISABLE_VOTE_ISR           (1<<0)
#define CPR_DISABLE_VOTE_MODE_SWITCH   (1<<1)
#define CPR_DISABLE_VOTE_RELINQUISH    (1<<2)
#define CPR_DISABLE_VOTE_SLEEP         (1<<3)

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

typedef struct
{
    cpr_domain_id       id;
    cpr_control_mode    cMode;
    uint8               modeSettingsCount;
    int32               marginAdjustments[CPR_MARGIN_ADJUSTMENT_MAX];
    int32               decodedFuseAging;
    uint8               railCPRRev;
    uint32              disableVotes;
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

    uint8                              railsCount; /* number of railVPs and rails */
    uint8                              enablementsCount;
    uint8                              controllersCount;

    cpr_versioned_voltage_plan       **railVVPs;
    cpr_hal_controller               **controllers;
    cpr_rail                         **rails;
    cpr_enablement                   **enablements;
    cpr_misc_cfg                      *miscCfg;
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
    const cpr_settings  *settings;
    cpr_rail_state      *railStates;
} cpr_information;


extern cpr_information cpr_info; // defined in cpr.c

#endif
