/*******************************************************************************
 *
 * Copyright (c) 2020 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 *
 * THIS IS AUTO-GENERATED CPR CONFIG FILE FOR 670.
 *
 * DateTime: 2020/10/30 05:36:45
 *
 *******************************************************************************/

#include "cpr_data.h"
#include "cpr_image_target_init.h"
#include "cpr_target_hwio.h"


//hash value of Voltage Plan file (extracted all cell values)
static const char voltage_plan_hash_value[] = "9A88E92380797471190C14A66619E11F";


/*******************************************************************************
********************************************************************************
**
** Voltage Plan Data
**
********************************************************************************
********************************************************************************/

static const cpr_margin_cfg margins_510 = {
    .count = 1,
    .data = (const cpr_margin_data[]) { { .cprRevMin = 0, .cprRevMax = 255, .openLoop = 0, .closedLoop = 0, .maxFloorToCeil = 0 } }
};


static const cpr_fuse_data  fuses_1237584514 =
{
    .volt       = (const cpr_fuse[]) { {.count = 1, .data = (const struct raw_fuse_data[]) { CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW2_LSB, CPR0_TARG_VOLT_SVS2) } } },
    .quot       = NULL,
    .rosel      = NULL,
    .quotOffset = NULL,
    .voltOffset = NULL,
};


static const cpr_fuse_data  fuses_1584637373 =
{
    .volt       = (const cpr_fuse[]) { {.count = 1, .data = (const struct raw_fuse_data[]) { CPR_FUSE_MAPPING(QFPROM_CORR_CALIB_ROW2_LSB, CPR0_TARG_VOLT_SVS) } } },
    .quot       = NULL,
    .rosel      = NULL,
    .quotOffset = NULL,
    .voltOffset = NULL,
};


/*
 * TURING_LDO Voltage Plan
 */
static cpr_voltage_plan voltage_plan_10877832379 =
{
    .modesCount      = 4,
    .tempAdjCfg      = NULL,
    .kvCfg           = NULL,
    .supportedModes  = (const cpr_voltage_mode[]) { CPR_VOLTAGE_MODE_MIN_SVS,
                                                    CPR_VOLTAGE_MODE_LOW_SVS,
                                                    CPR_VOLTAGE_MODE_SVS,
                                                    CPR_VOLTAGE_MODE_SVS_L1 },
    .modes           = (struct cpr_voltage_data[]) {
        // MIN_SVS
        {.fref = 520000, .freqDelta = 0, .fuses = &fuses_1237584514, .quotients = NULL, .margins = &margins_510,
         .marginTempBands = {0},
         .subModesCount   = 1,
         .subModes        = (const cpr_freq_data[]){ {.ceiling =  616000, .floor =  520000, .freq =       0, .interplEnabled = false} } },

        // LOW_SVS
        {.fref = 576000, .freqDelta = 0, .fuses = &fuses_1237584514, .quotients = NULL, .margins = &margins_510,
         .marginTempBands = {0},
         .subModesCount   = 1,
         .subModes        = (const cpr_freq_data[]){ {.ceiling =  680000, .floor =  520000, .freq =       0, .interplEnabled = false} } },

        // SVS
        {.fref = 624000, .freqDelta = 0, .fuses = &fuses_1584637373, .quotients = NULL, .margins = &margins_510,
         .marginTempBands = {0},
         .subModesCount   = 1,
         .subModes        = (const cpr_freq_data[]){ {.ceiling =  736000, .floor =  536000, .freq =       0, .interplEnabled = false} } },

        // SVS_L1
        {.fref = 680000, .freqDelta = 0, .fuses = &fuses_1584637373, .quotients = NULL, .margins = &margins_510,
         .marginTempBands = {0},
         .subModesCount   = 1,
         .subModes        = (const cpr_freq_data[]){ {.ceiling =  800000, .floor =  584000, .freq =       0, .interplEnabled = false} } }},
};


static const cpr_version  ver_ANY_1_0_1_255 = { .foundry = CPR_FOUNDRY_ANY, .min = CPR_CHIPINFO_VERSION(1,0), .max = CPR_CHIPINFO_VERSION(1,255) };


/*
 * Aging configs
 */

/*
 * Versioned Voltage Plans
 */

static const cpr_versioned_voltage_plan turing_ldo_vvp = {
    .rail     = CPR_RAIL_TURING_LDO,
    .list     = (struct voltage_plan_list[]) {
        { .version = &ver_ANY_1_0_1_255, .cfg = &voltage_plan_10877832379 },
     },
    .count    = 1,
    .agingCfg = NULL
};

/*******************************************************************************
********************************************************************************
**
** Enablement Config Data
**
********************************************************************************
********************************************************************************/

/*
 * CPR Controller Thread Config
 */

static const cpr_hal_thread_cfg turing_ldo_thread_cfg = {
    .upThresh     = 2,
    .dnThresh     = 2,
    .consecUp     = 0,
    .consecDn     = 0,
    .sensors      = (const uint8[]){ 33,34,35,36 },
    .sensorsCount = 4,
};


static cpr_hal_thread  turing_ldo_thread = {
    .id   = 0,
    .cfg  = &turing_ldo_thread_cfg,
};


/*
 * CPR Controller Config
 */

static const cpr_hal_controller_cfg turing_ldo_cpr3_controller_cfg = {
    .stepQuotMin         = 8,
    .stepQuotMax         = 12,
    .sensorsTotal        = 4,
    .bypassSensors       = NULL,
    .bypassSensorsCount  = 0,
    .disableSensors      = NULL,
    .disableSensorsCount = 0,
    .refClk              = "clk_rbcpr_ref",
    .ahbClk              = "clk_bus_rbcpr",
};


static cpr_hal_controller  turing_ldo_cpr3_controller = {
    .base                = 0xe73c0000,
    .type                = CPR_CONTROLLER_TYPE_SW_CL_ONLY,
    .cfg                 = &turing_ldo_cpr3_controller_cfg,
    .threads             = (cpr_hal_thread*[]){ &turing_ldo_thread },
    .threadsCount        = 1,
};


/*
 * HAL Rail Config
 */

static cpr_rail cpr_rail_turing_ldo_cfg = {
    .id               = CPR_RAIL_TURING_LDO,
    .name             = "TURING_LDO",
    .hal              = { .controller = &turing_ldo_cpr3_controller, .thread = &turing_ldo_thread },
    .interruptId      = 0,
    .settleModes      = NULL,
    .settleModesCount = 0,
};


/*
 * Rail Enablement Config
 */

static const cpr_version  ver_ANY_0_0_255_255 = { .foundry = CPR_FOUNDRY_ANY, .min = CPR_CHIPINFO_VERSION(0,0), .max = CPR_CHIPINFO_VERSION(255,255) };


static const cpr_enablement cpr_rail_turing_ldo_enablement_0_0_255_255 =
{
    .id                = CPR_RAIL_TURING_LDO,
    .version           = &ver_ANY_0_0_255_255,
    .funcId            = CPR_INIT_NONE,
    .fuseMultiplier    = 10000,
    .stepSize          = 4000,
    .railCPRRevFuse    = {.count = 1, .data = (struct raw_fuse_data[]) {
                   {SECURITY_CONTROL_BASE | (0X780240 & (SECURITY_CONTROL_BASE_SIZE - 1)), 7, (1<<9) | (1<<8) | (1<<7)} } },
};


static const cpr_misc_cfg misc_cfg = {
    .cprRev = {.count = 1, .data = (struct raw_fuse_data[]) { {SECURITY_CONTROL_BASE | (0X780240 & (SECURITY_CONTROL_BASE_SIZE - 1)), 7, (1<<9) | (1<<8) | (1<<7)} } }
};


const cpr_settings cpr_settings_670 =
{
    .hashValue        = voltage_plan_hash_value,

    .railVVPs         = (const cpr_versioned_voltage_plan*[]) { &turing_ldo_vvp },
    .rails            = (cpr_rail*[]) { &cpr_rail_turing_ldo_cfg },
    .railsCount       = 1,

    .controllers      = (cpr_hal_controller*[]) { &turing_ldo_cpr3_controller },
    .controllersCount = 1,

    .enablements      = (const cpr_enablement*[]) { &cpr_rail_turing_ldo_enablement_0_0_255_255 },
    .enablementsCount = 1,

    .miscCfg          = &misc_cfg,
};

