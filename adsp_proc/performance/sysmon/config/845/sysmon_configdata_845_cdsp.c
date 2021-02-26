/*=============================================================================
 * FILE:                 sysmon_configdata_845_cdsp.c
 *
 * DESCRIPTION:
 *    Sysmon device configuration data for 845 CDSP
 *
 * Copyright (c) 2017 QUALCOMM Technologies, Incorporated.
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
        /* C0 * 1000 */             2580,
        /* C1 * 1000 */             2858,
        /* C2 */                    2,
        /* hwVersion */             {SD_HW_VER(0x01,0x00,0x02,0x00), DALCHIPINFO_FAMILY_SDM845}
    },
    {
        /* C0 * 1000 */             2260,
        /* C1 * 1000 */             2858,
        /* C2 */                    2,
        /* hwVersion */             {SD_HW_VER(0x02,0x00,0xff,0x00), DALCHIPINFO_FAMILY_SDM845}
    },
};

const sd_devcfg_bus_coeffs_t bus_coeffs_ddr[]=
{
    {
        /* C0 */                    1,       
        /* C1 * 1000 */             1429,
        /* C2 */                    4,          //Multiplication factor of 4 for Ib due to 4 byte channel width vs 16 byte for all channels
        /* hwVersion */             {SD_HW_VER(0x01,0x00,0xff,0x00), DALCHIPINFO_FAMILY_SDM845}
    },
};

const sd_devcfg_bus_clk_table_t bus_clk_table[]=
{
    //V1: Turing NOC width: 32 bytes @19.2 (MinSVS) 120 (LowSVS) 200 (SVS) 233.333 (SVS_L1) 233.333 (Nominal) 268.667 (Turbo)
    //V1: MEMNOC width: 64 bytes @19.2, 150 (MinSVS) 300 (LowSVS) 426.4 (SVS) 533 (SVS_L1) 600 (Nominal) 700 (Turbo)
    //V2: Turing NOC width: 32 bytes @19.2 (MinSVS) 150 (LowSVS) 207.33 (SVS) 300 (SVS_L1) 403 (Nominal) 533 (Turbo)
    //V2: MEMNOC width: 64 bytes @19.2, 150 (MinSVS) 300 (LowSVS) 466.5 (SVS) 600 (SVS_L1) 806 (Nominal) 933 (Turbo)
    //V2: DDR width: 16 bytes @19.2, 100, 200, 300 (MinSVS), 451.2 (SVS2), 547.2, 681.6, 825.6 (SVS), 1017.6 (SVS_L1), 1296, 1555.2 (Nominal), 1804.8 (Turbo)
    //V2: CPU L3 frequencies: 576 (SVS2), 806.4 (SVS), 940.8 (SVS_L1), 1209.6 (NOM), 1401.6 (NOM_L1), 1478.4(TURBO), 1593.6(TURBO_L1)?
    {   //V1
        /* freqkHz */               19200,
        /* freqkHz_parent */        150000,
        /* vRegLevel */             SYSMON_CLOCK_VREG_LEVEL_LOW_MIN,
        /* hwVersion */             {SD_HW_VER(0x01,0x00,0x02,0x00), DALCHIPINFO_FAMILY_SDM845},
        /* minVote */               {380000000, 260387000},
        /* maxVote */               {1140000000, 260387000},
        /* freqKHz_ddr */           300000,
    },
    {   //V2
        /* freqkHz */               19200,
        /* freqkHz_parent */        150000,
        /* vRegLevel */             SYSMON_CLOCK_VREG_LEVEL_LOW_MIN,
        /* hwVersion */             {SD_HW_VER(0x02,0x00,0xff,0x00), DALCHIPINFO_FAMILY_SDM845},
        /* minVote */               {380000000, 297255956},
        /* maxVote */               {1140000000, 297255956},
        /* freqKHz_ddr */           300000,
    },
    {   //V1
        /* freqkHz */               120000,
        /* freqkHz_parent */        300000,
        /* vRegLevel */             SYSMON_CLOCK_VREG_LEVEL_LOW_MINUS,
        /* hwVersion */             {SD_HW_VER(0x01,0x00,0x02,0x00), DALCHIPINFO_FAMILY_SDM845},
        /* minVote */               {1714560000, 520774000},
        /* maxVote */               {1714560000, 520774000},
        /* freqKHz_ddr */           451200,
    },
    {   //V2
        /* freqkHz */               150000,
        /* freqkHz_parent */        300000,
        /* vRegLevel */             SYSMON_CLOCK_VREG_LEVEL_LOW_MINUS,
        /* hwVersion */             {SD_HW_VER(0x02,0x00,0xff,0x00), DALCHIPINFO_FAMILY_SDM845},
        /* minVote */               {1714560000, 594511912},
        /* maxVote */               {1714560000, 594511912},
        /* freqKHz_ddr */           451200,
    },
    {   //V1
        /* freqkHz */               200000,
        /* freqkHz_parent */        426400,
        /* vRegLevel */             SYSMON_CLOCK_VREG_LEVEL_LOW,
        /* hwVersion */             {SD_HW_VER(0x01,0x00,0x02,0x00), DALCHIPINFO_FAMILY_SDM845},
        /* minVote */               {2079360000, 740193446},
        /* maxVote */               {2918400000, 740193446},
        /* freqKHz_ddr */           768000,
    },
    {   //V2
        /* freqkHz */               207330,
        /* freqkHz_parent */        466500,
        /* vRegLevel */             SYSMON_CLOCK_VREG_LEVEL_LOW,
        /* hwVersion */             {SD_HW_VER(0x02,0x00,0xff,0x00), DALCHIPINFO_FAMILY_SDM845},
        /* minVote */               {2079360000, 924466023},
        /* maxVote */               {2918400000, 924466023},
        /* freqKHz_ddr */           768000,
    },
    {   //V1
        /* freqkHz */               233330,
        /* freqkHz_parent */        533000,
        /* vRegLevel */             SYSMON_CLOCK_VREG_LEVEL_LOW_PLUS,
        /* hwVersion */             {SD_HW_VER(0x01,0x00,0x02,0x00), DALCHIPINFO_FAMILY_SDM845},
        /* minVote */               {3866880000, 925241807},
        /* maxVote */               {3866880000, 925241807},
        /* freqKHz_ddr */           1017600,
    },
    {   //V2
        /* freqkHz */               300000,
        /* freqkHz_parent */        600000,
        /* vRegLevel */             SYSMON_CLOCK_VREG_LEVEL_LOW_PLUS,
        /* hwVersion */             {SD_HW_VER(0x02,0x00,0xff,0x00), DALCHIPINFO_FAMILY_SDM845},
        /* minVote */               {3866880000, 1189023824},
        /* maxVote */               {3866880000, 1189023824},
        /* freqKHz_ddr */           1017600,
        /* l3FreqVoteKHz */         576000,
        /* l3FreqDCVSVoteKHz */     806400,
        /* l3FreqMinQ6 */           SYSMON_CLOCK_VREG_LEVEL_NOMINAL
    },
    {   //V1
        /* freqkHz */               233330,
        /* freqkHz_parent */        600000,
        /* vRegLevel */             SYSMON_CLOCK_VREG_LEVEL_NOMINAL,
        /* hwVersion */             {SD_HW_VER(0x01,0x00,0x02,0x00), DALCHIPINFO_FAMILY_SDM845},
        /* minVote */               {4924800000, 1041548001},
        /* maxVote */               {4924800000, 1041548001},
        /* freqKHz_ddr */           1296000,
    },
    {   //V2
        /* freqkHz */               403000,
        /* freqkHz_parent */        806000,
        /* vRegLevel */             SYSMON_CLOCK_VREG_LEVEL_NOMINAL,
        /* hwVersion */             {SD_HW_VER(0x02,0x00,0xff,0x00), DALCHIPINFO_FAMILY_SDM845},
        /* minVote */               {4924800000, 1597255337},
        /* maxVote */               {5909760000, 1597255337},
        /* freqKHz_ddr */           1555200,
        /* l3FreqVoteKHz */         576000,
        /* l3FreqDCVSVoteKHz */     940800,
        /* l3FreqMinQ6 */           SYSMON_CLOCK_VREG_LEVEL_NOMINAL
    },
    {   //V1
        /* freqkHz */               268670,
        /* freqkHz_parent */        700000,
        /* vRegLevel */             SYSMON_CLOCK_VREG_LEVEL_HIGH,
        /* hwVersion */             {SD_HW_VER(0x01,0x00,0x02,0x00), DALCHIPINFO_FAMILY_SDM845},
        /* minVote */               {5909760000, 1215139334},
        /* maxVote */               {6858240000, 1215139334},
        /* freqKHz_ddr */           1804800,
    },
    {   //V2
        /* freqkHz */               533000,
        /* freqkHz_parent*/         933000,
        /* vRegLevel */             SYSMON_CLOCK_VREG_LEVEL_HIGH,
        /* hwVersion */             {SD_HW_VER(0x02,0x00,0xff,0x00), DALCHIPINFO_FAMILY_SDM845},
        /* minVote */               {6858240000, 1848932046},
        /* maxVote */               {6858240000, 1848932046},
        /* freqKHz_ddr */           1804800,
        /* l3FreqVoteKHz */         576000,
        /* l3FreqDCVSVoteKHz */     1209600, 
        /* l3FreqMinQ6 */           SYSMON_CLOCK_VREG_LEVEL_NOMINAL
    },
};

const sd_devcfg_bus_clk_desc_t bus_clk_descriptor =
{
    sizeof(bus_clk_table) / sizeof(sd_devcfg_bus_clk_table_t),
    (sd_devcfg_bus_clk_table_t*)bus_clk_table,
    sizeof(bus_coeffs) / sizeof(sd_devcfg_bus_coeffs_t),
    (sd_devcfg_bus_coeffs_t*)bus_coeffs,
    sizeof(bus_coeffs_ddr) / sizeof(sd_devcfg_bus_coeffs_t),
    (sd_devcfg_bus_coeffs_t*)bus_coeffs_ddr,
};

const sd_devcfg_core_clk_table_t core_clk_table[]=
{
    {
        /* freqkHz */               249600,
        /* vRegLevel */             SYSMON_CLOCK_VREG_LEVEL_LOW_MIN,
        /* hwVersion */             {SD_HW_VER(0x01,0x00,0x02,0x00), DALCHIPINFO_FAMILY_SDM845}
    },
    {
        /* freqkHz */               288000,
        /* vRegLevel */             SYSMON_CLOCK_VREG_LEVEL_LOW_MIN,
        /* hwVersion */             {SD_HW_VER(0x02,0x00,0xff,0x00), DALCHIPINFO_FAMILY_SDM845}
    },
    {
        /* freqkHz */               384000,
        /* vRegLevel */             SYSMON_CLOCK_VREG_LEVEL_LOW_MINUS,
        /* hwVersion */             {SD_HW_VER(0x01,0x00,0x02,0x00), DALCHIPINFO_FAMILY_SDM845}
    },
    {
        /* freqkHz */               460800,
        /* vRegLevel */             SYSMON_CLOCK_VREG_LEVEL_LOW_MINUS,
        /* hwVersion */             {SD_HW_VER(0x02,0x00,0xff,0x00), DALCHIPINFO_FAMILY_SDM845}
    },
    {
        /* freqkHz */               499200,
        /* vRegLevel */             SYSMON_CLOCK_VREG_LEVEL_LOW,
        /* hwVersion */             {SD_HW_VER(0x01,0x00,0x02,0x00), DALCHIPINFO_FAMILY_SDM845}
    },
    {
        /* freqkHz */               614400,
        /* vRegLevel */             SYSMON_CLOCK_VREG_LEVEL_LOW,
        /* hwVersion */             {SD_HW_VER(0x02,0x00,0xff,0x00), DALCHIPINFO_FAMILY_SDM845}
    },
    {
        /* freqkHz */               691200,
        /* vRegLevel */             SYSMON_CLOCK_VREG_LEVEL_LOW_PLUS,
        /* hwVersion */             {SD_HW_VER(0x01,0x00,0x02,0x00), DALCHIPINFO_FAMILY_SDM845}
    },
    {
        /* freqkHz */               787200,
        /* vRegLevel */             SYSMON_CLOCK_VREG_LEVEL_LOW_PLUS,
        /* hwVersion */             {SD_HW_VER(0x02,0x00,0xff,0x00), DALCHIPINFO_FAMILY_SDM845}
    },
    {
        /* freqkHz */               787200,
        /* vRegLevel */             SYSMON_CLOCK_VREG_LEVEL_NOMINAL,
        /* hwVersion */             {SD_HW_VER(0x01,0x00,0x02,0x00), DALCHIPINFO_FAMILY_SDM845}
    },
    {
        /* freqkHz */               940800,
        /* vRegLevel */             SYSMON_CLOCK_VREG_LEVEL_NOMINAL,
        /* hwVersion */             {SD_HW_VER(0x02,0x00,0xff,0x00), DALCHIPINFO_FAMILY_SDM845}
    },
    {
        /* freqkHz */               998400,
        /* vRegLevel */             SYSMON_CLOCK_VREG_LEVEL_HIGH,
        /* hwVersion */             {SD_HW_VER(0x01,0x00,0x02,0x00), DALCHIPINFO_FAMILY_SDM845}
    },
    {
        /* freqkHz */               1190400,
        /* vRegLevel */             SYSMON_CLOCK_VREG_LEVEL_HIGH,
        /* hwVersion */             {SD_HW_VER(0x02,0x00,0xff,0x00), DALCHIPINFO_FAMILY_SDM845}
    },
};

const sd_devcfg_core_clk_desc_t core_clk_descriptor =
{
    sizeof(core_clk_table)/sizeof(sd_devcfg_core_clk_table_t),
    (sd_devcfg_core_clk_table_t *)core_clk_table
};

const sd_devcfg_hvx_throttle_config_table_t hvx_throttle_config_table[]=
{
    {                                          
        /* input_params */          0x05000004,     //Any client = don't care, NPU = Inactive, Camera = Active, HW Interrupt = Inactive,
                                                    //Thermal client = don't care,                      
                                                    //Q6 clock = Turbo, No. of HVX units in use = don't care.
        /* mask_input_params */     0x0F00000E,
        /* throttle_weight */       0xa61686e7,
        /* throttle_cfg */          0xC3E03FF0,     //47% throttle level
        /* q6clk_output */          -1,        
    },        
    {   
        /* input_params */          0x00000000,     //Any client = don't care, NPU = don't care, Camera = don't care, HW Interrupt = don't care,
                                                    //Thermal client = don't care,                   
                                                    //Q6 clock = don't care, No. of HVX units in use = don't care.
        /* mask_input_params */     0x00000000,
        /* throttle_weight */       0xa61686e7,
        /* throttle_cfg */          0xC3E03FF5,     //30% throttle level
        /* q6clk_output */          -1,        
    },    
}; 

const sd_devcfg_hvx_throttle_config_desc_t hvx_throttle_config_descriptor =
{
    sizeof(hvx_throttle_config_table) / sizeof(sd_devcfg_hvx_throttle_config_table_t),
    (sd_devcfg_hvx_throttle_config_table_t*)hvx_throttle_config_table,    
};

const sd_devcfg_hvx_instr_throttle_table_t hvx_instr_throttle_table[]=
{
    {   
        /* throttle_weight */       0xa61686e7,
        /* throttle_cfg */          0xC3E03FF5,     //30% throttle level
    },
    {   
        /* throttle_weight */       0xa61686e7,
        /* throttle_cfg */          0xC3E03FF0,     //47% throttle level
    },
};

const sd_devcfg_hvx_instr_throttle_desc_t hvx_instr_throttle_descriptor =
{
    sizeof(hvx_instr_throttle_table) / sizeof(sd_devcfg_hvx_instr_throttle_table_t),
    (sd_devcfg_hvx_instr_throttle_table_t*)hvx_instr_throttle_table,    
};

const sd_devcfg_dcvs_params_t dcvsBalancedParams =
{
    /* dcvsTransientDur      = */    200,
    /* dcvsTransientDurMax   = */    500,
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
    /* coreStallsDangerLevel = */    40,
    /* pCppSafe1TLevel       = */    30,
    /* blcLatDangerLevel     = */    90,
    /* l3VoteHighWindowSize  = */     5, 
    /* l3VoteHighWindowAdj   = */    10,
    /* l3VoteHighWindowMax   = */    45,
    /* l3VoteLowWindowSize   = */    50,
    /* l3VoteLowThreshold    = */    88,
    /* l3VoteLowWindowAdj    = */    50,
    /* l3VoteLowWindowMax    = */   200,
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
    /* coreStallsDangerLevel = */    40,
    /* pCppSafe1TLevel       = */    30,
    /* blcLatDangerLevel     = */    80,
    /* l3VoteHighWindowSize  = */     1, 
    /* l3VoteHighWindowAdj   = */     5,
    /* l3VoteHighWindowMax   = */    35,
    /* l3VoteLowWindowSize   = */    50,
    /* l3VoteLowThreshold    = */    70,    
    /* l3VoteLowWindowAdj    = */    50,
    /* l3VoteLowWindowMax    = */   200,
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
    /* coreStallsDangerLevel = */    40,
    /* pCppSafe1TLevel       = */    30,
    /* blcLatDangerLevel     = */    90,
    /* l3VoteHighWindowSize  = */     5, 
    /* l3VoteHighWindowAdj   = */    10,
    /* l3VoteHighWindowMax   = */    46,
    /* l3VoteLowWindowSize   = */    50,
    /* l3VoteLowThreshold    = */    90, 
    /* l3VoteLowWindowAdj    = */    50,
    /* l3VoteLowWindowMax    = */   200,
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
    /* coreStallsDangerLevel = */    40,
    /* pCppSafe1TLevel       = */    30,
    /* blcLatDangerLevel     = */    95,
    /* l3VoteHighWindowSize  = */    10, 
    /* l3VoteHighWindowAdj   = */    10,
    /* l3VoteHighWindowMax   = */    48,
    /* l3VoteLowWindowSize   = */    50,
    /* l3VoteLowThreshold    = */    95,
    /* l3VoteLowWindowAdj    = */    50,
    /* l3VoteLowWindowMax    = */   200,
};

const uint32_t dcvsClientL3CornerVoteConv[] = {
    0,
    576000,
    806400,
    940800,
    1209600,
    1209600,
    1401600,
};

const sd_vreg_type_t dcvsClientCornerVoteCoreConv[] = {
    SYSMON_CLOCK_VREG_LEVEL_OFF,
    SYSMON_CLOCK_VREG_LEVEL_LOW_MINUS,
    SYSMON_CLOCK_VREG_LEVEL_LOW,
    SYSMON_CLOCK_VREG_LEVEL_LOW_PLUS,
    SYSMON_CLOCK_VREG_LEVEL_NOMINAL,
    SYSMON_CLOCK_VREG_LEVEL_NOMINAL,
    SYSMON_CLOCK_VREG_LEVEL_HIGH,
};

const sd_vreg_type_t dcvsClientCornerVoteBusConv[] = {
    SYSMON_CLOCK_VREG_LEVEL_OFF,
    SYSMON_CLOCK_VREG_LEVEL_LOW_MINUS,
    SYSMON_CLOCK_VREG_LEVEL_LOW,
    SYSMON_CLOCK_VREG_LEVEL_LOW_PLUS,
    SYSMON_CLOCK_VREG_LEVEL_NOMINAL,
    SYSMON_CLOCK_VREG_LEVEL_NOMINAL,
    SYSMON_CLOCK_VREG_LEVEL_HIGH,
};
