/*=============================================================================
 * FILE:                 sysmon_configdata_670_adsp.c
 *
 * DESCRIPTION:
 *    Sysmon device configuration data for 670/605 ADSP
 *
 * Copyright (c) 2018 QUALCOMM Technologies, Incorporated.
 * All Rights Reserved.
 * QUALCOMM Proprietary.
  ===========================================================================*/

/*=============================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 * when         who          what, where, why
 * ----------   ------     ------------------------------------------------
 ============================================================================*/

#include <stdlib.h>
#include "DALDeviceId.h"
#include "DDIChipInfo.h"
#include "sysmon_devcfg_int.h"

const sd_devcfg_bus_coeffs_t bus_coeffs[]=
{
        {
        /* C0 * 1000 */             1000,
        /* C1 * 1000 */             2858,
        /* C2 */                    2,
        }, //HW version removed as this is applicable across versions and chipsets (670/605)
};

const sd_devcfg_bus_clk_table_t bus_clk_table[]=
{
    //LPASS Agnoc East tile width:  8 bytes @ 19.2 (MinSVS) 75 (MinSVS) 150 (LowSVS) 200 (SVS) 403 (Nominal) 533 (Turbo)
    //SNOC width:                  16 bytes @ 19.2 (MinSVS) 75 (MinSVS) 100 (LowSVS) 200 (SVS) 403 (Nominal) 533 (Turbo)
        {
        /* freqkHz */               19200,
        /* freqKHz_parent */        19200,
        /* vRegLevel */             SYSMON_CLOCK_VREG_LEVEL_LOW_MIN,
        },
        {
        /* freqkHz */               75000,
        /* freqKHz_parent */        75000,
        /* vRegLevel */             SYSMON_CLOCK_VREG_LEVEL_LOW_MIN,
        },
        {
        /* freqkHz */               150000,
        /* freqKHz_parent */        100000,
        /* vRegLevel */             SYSMON_CLOCK_VREG_LEVEL_LOW_MINUS,
        },
        {
        /* freqkHz */               200000,
        /* freqKHz_parent */        200000,
        /* vRegLevel */             SYSMON_CLOCK_VREG_LEVEL_LOW,
        },
        {
        /* freqkHz */               403000,
        /* freqKHz_parent */        403000,
        /* vRegLevel */             SYSMON_CLOCK_VREG_LEVEL_NOMINAL,
        },
        {
        /* freqkHz */               533000,
        /* freqKHz_parent */        533000,
        /* vRegLevel */             SYSMON_CLOCK_VREG_LEVEL_HIGH,
        },
};

const sd_devcfg_bus_clk_desc_t bus_clk_descriptor =
{
        sizeof(bus_clk_table)/sizeof(sd_devcfg_bus_clk_table_t),
        (sd_devcfg_bus_clk_table_t *)bus_clk_table,
        sizeof(bus_coeffs) / sizeof(sd_devcfg_bus_coeffs_t),
        (sd_devcfg_bus_coeffs_t*)bus_coeffs,
        0,
        0,
};

const sd_devcfg_core_clk_table_t core_clk_table[]=
{
        {
        /* freqkHz */               300000,
        /* vRegLevel */             SYSMON_CLOCK_VREG_LEVEL_LOW_MIN,
        },
        {
        /* freqkHz */               400000,
        /* vRegLevel */             SYSMON_CLOCK_VREG_LEVEL_LOW_MINUS,
        },
        {
        /* freqkHz */               600000,
        /* vRegLevel */             SYSMON_CLOCK_VREG_LEVEL_LOW,
        },
        {
        /* freqkHz */               844800,
        /* vRegLevel */             SYSMON_CLOCK_VREG_LEVEL_NOMINAL,
        },
        {
        /* freqkHz */               940800,
        /* vRegLevel */             SYSMON_CLOCK_VREG_LEVEL_NOMINAL,
        },
        {
        /* freqkHz */               1036800,
        /* vRegLevel */             SYSMON_CLOCK_VREG_LEVEL_HIGH,
        },
        {
        /* freqkHz */               1190400,
        /* vRegLevel */             SYSMON_CLOCK_VREG_LEVEL_HIGH,
        },
};

const sd_devcfg_core_clk_desc_t core_clk_descriptor =
{
        sizeof(core_clk_table)/sizeof(sd_devcfg_core_clk_table_t),
        (sd_devcfg_core_clk_table_t *)core_clk_table
};

const sd_devcfg_dcvs_params_t dcvsBalancedParams =
{
    /* dcvsTransientDur      = */    200,
    /* dcvsTransientDurMax   = */    200,
    /* q6LoadSafe            = */    70,
    /* q6LoadDanger          = */    90,
    /* q6Load1TSafe          = */    10,
    /* lowerCoreQ6Load       = */    68,
    /* lowerCoreQ6LoadAdj    = */    0,
    /* lowerCoreQ6LoadMin    = */    68,
    /* lowerCoreWinDur       = */    100,
    /* lowerCoreWinHFW       = */    0,
    /* lowerCoreWinDurAdj    = */    100,
    /* lowerCoreWinDurMax    = */    400,
    /* lowerBusWinDur        = */    200,
    /* lowerBusWinHFW        = */    50,
    /* pCppDangerLevel       = */    10,
    /* pCppSafeLevel         = */    5,
    /* coreStallsDangerLevel = */    35,
    /* pCppSafe1TLevel       = */    20,
};

const sd_devcfg_dcvs_params_t dcvsPerformanceParams =
{
    /* dcvsTransientDur      = */    200,
    /* dcvsTransientDurMax   = */    500,
    /* q6LoadSafe            = */    70,
    /* q6LoadDanger          = */    90,
    /* q6Load1TSafe          = */    10,
    /* lowerCoreQ6Load       = */    68,
    /* lowerCoreQ6LoadAdj    = */    10,
    /* lowerCoreQ6LoadMin    = */    48,
    /* lowerCoreWinDur       = */    200,
    /* lowerCoreWinHFW       = */    0,
    /* lowerCoreWinDurAdj    = */    100,
    /* lowerCoreWinDurMax    = */    600,
    /* lowerBusWinDur        = */    200,
    /* lowerBusWinHFW        = */    50,
    /* pCppDangerLevel       = */    10,
    /* pCppSafeLevel         = */    5,
    /* coreStallsDangerLevel = */    35,
    /* pCppSafe1TLevel       = */    20,
};

const sd_devcfg_dcvs_params_t dcvsPowerSaverParams =
{
    /* dcvsTransientDur      = */    200,
    /* dcvsTransientDurMax   = */    500,
    /* q6LoadSafe            = */    90,
    /* q6LoadDanger          = */    98,
    /* q6Load1TSafe          = */    10,
    /* lowerCoreQ6Load       = */    88,
    /* lowerCoreQ6LoadAdj    = */    10,
    /* lowerCoreQ6LoadMin    = */    68,
    /* lowerCoreWinDur       = */    100,
    /* lowerCoreWinHFW       = */    0,
    /* lowerCoreWinDurAdj    = */    100,
    /* lowerCoreWinDurMax    = */    300,
    /* lowerBusWinDur        = */    200,
    /* lowerBusWinHFW        = */    50,
    /* pCppDangerLevel       = */    10,
    /* pCppSafeLevel         = */    5,
    /* coreStallsDangerLevel = */    35,
    /* pCppSafe1TLevel       = */    20,
};

const sd_devcfg_dcvs_params_t dcvsPowerSaverAggressiveParams =
{
    /* dcvsTransientDur      = */    100,
    /* dcvsTransientDurMax   = */    500,
    /* q6LoadSafe            = */    90,
    /* q6LoadDanger          = */    98,
    /* q6Load1TSafe          = */    10,
    /* lowerCoreQ6Load       = */    88,
    /* lowerCoreQ6LoadAdj    = */    10,
    /* lowerCoreQ6LoadMin    = */    78,
    /* lowerCoreWinDur       = */    50,
    /* lowerCoreWinHFW       = */    0,
    /* lowerCoreWinDurAdj    = */    100,
    /* lowerCoreWinDurMax    = */    250,
    /* lowerBusWinDur        = */    100,
    /* lowerBusWinHFW        = */    50,
    /* pCppDangerLevel       = */    10,
    /* pCppSafeLevel         = */    5,
    /* coreStallsDangerLevel = */    35,
    /* pCppSafe1TLevel       = */    20,
};

const sd_vreg_type_t dcvsClientCornerVoteCoreConv[] = {
        SYSMON_CLOCK_VREG_LEVEL_OFF,
        SYSMON_CLOCK_VREG_LEVEL_LOW_MINUS,
        SYSMON_CLOCK_VREG_LEVEL_LOW,
        SYSMON_CLOCK_VREG_LEVEL_LOW,
        SYSMON_CLOCK_VREG_LEVEL_NOMINAL,
        SYSMON_CLOCK_VREG_LEVEL_NOMINAL,
        SYSMON_CLOCK_VREG_LEVEL_HIGH,
};

const sd_vreg_type_t dcvsClientCornerVoteBusConv[] = {
        SYSMON_CLOCK_VREG_LEVEL_OFF,
        SYSMON_CLOCK_VREG_LEVEL_LOW_MINUS,
        SYSMON_CLOCK_VREG_LEVEL_LOW,
        SYSMON_CLOCK_VREG_LEVEL_LOW,
        SYSMON_CLOCK_VREG_LEVEL_NOMINAL,
        SYSMON_CLOCK_VREG_LEVEL_NOMINAL,
        SYSMON_CLOCK_VREG_LEVEL_HIGH,
};
