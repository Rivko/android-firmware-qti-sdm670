/*! \file pm_config_target.c
 *
 *  \brief This file contains customizable target specific driver settings & PMIC registers.
 *         This file info is taken from Target Component Resource Specific Settings from PDM
 *         that is maintained for each of the targets separately.
 *
 *  &copy; Copyright 2013 - 2017 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/config/qcs605/pm_config_target.c#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/01/18   as      Replacing DRV mask of L7a to ARC (CR#2343369)
03/27/18   as      Keeping minpwrmode of LDO16a to NPM ( CR#2213345 )
12/19/14   as      Created.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "pm_target_information.h"
#include "pm_config_target.h"
#include "pm_vrm.h"

/*AccessAllowed, AlwaysOn, MinMode, SafetyHR, MinVoltage, MaxVoltage, EnableFixedTime, EnableStepperWarmUpTime, VoltFixedUpTime, VoltFixedDownTime, VoltStepperExtraUpTime, 
  VoltStepperExtraDownTime, VoltDischargeTime, ModeFixedTime, VregReadyEn, VregReadyErrEn, VregReadyMultiplier, Aop, 4 SeqTypes, DrvOwnMask, Reserved */
__attribute__((section("pm_ddr_reclaim_pool")))
pm_vrm_vreg_rail_info_type ldo_rail_a[] = 
{
  {PM_ACCESS_ALLOWED,    PM_OFF, PM_NPM_MODE_VAL, 40, 800, 800,  300, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ,  VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_MDMS,0  }, // LDO1 N600_STEP 
  {PM_ACCESS_ALLOWED,    PM_OFF, PM_LPM_MODE_VAL, 24, 1144, 1256, 300, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_HLOS,0}, // LDO2 HT_N1200_STEP
  {PM_ACCESS_ALLOWED,    PM_OFF, PM_LPM_MODE_VAL, 40, 1200, 1352, 300, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_MDMS,0}, // LDO3 MT_N600_STEP
  {PM_ACCESS_DISALLOWED, PM_ON,  PM_LPM_MODE_VAL, 36,  500, 1084, 300, 0, 0, 0,20,20,100,30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_2X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_ARC ,0 },  // LDO4 Not defined for Groot
  {PM_ACCESS_ALLOWED,    PM_OFF, PM_LPM_MODE_VAL, 24, 1200, 1304, 300, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_MDMS,0 }, // LDO5 HT_N600_STEP
  {PM_ACCESS_ALLOWED,    PM_OFF, PM_LPM_MODE_VAL, 24,  880,  880, 300, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_MDMS,0 }, // LDO6 HT_N600
  {PM_ACCESS_ALLOWED,    PM_ON,  PM_LPM_MODE_VAL, 24,  352,  952, 300, 0, 0, 0, 20,20,50,30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_ARC,0 },  // LDO7 HT_N1200 LPI_CX
  {PM_ACCESS_ALLOWED,    PM_ON,  PM_LPM_MODE_VAL, 40, 1696, 1952, 300, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_HLOS,0 }, // LDO8 MT_LV_P600 TODO:RBSC OFF
  {PM_ACCESS_ALLOWED,    PM_OFF, PM_NPM_MODE_VAL, 24, 1616, 1984, 300, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_AOP,0  }, // LDO9 HT_P150
  {PM_ACCESS_ALLOWED,    PM_OFF, PM_LPM_MODE_VAL, 24, 1696, 1952, 300, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_HLOS,0 }, // LDO10 HT_P300
  {PM_ACCESS_ALLOWED,    PM_OFF, PM_LPM_MODE_VAL, 40, 1800, 1904, 300, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_HLOS,0 }, // LDO11 MT_P150_STEP
  {PM_ACCESS_ALLOWED,    PM_OFF, PM_LPM_MODE_VAL, 24, 1616, 1984, 300, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_HLOS,0 }, // LDO12 HT_LV_P300
  {PM_ACCESS_ALLOWED,    PM_ON,  PM_NPM_MODE_VAL, 40, 1696, 1904, 300, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_AOP ,0}, // LDO13 MT_LV_P600 
  {PM_ACCESS_ALLOWED,    PM_OFF, PM_LPM_MODE_VAL, 40, 1696, 1904, 300, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_SEN ,0 }, // LDO14 MT_LV_P150
  {PM_ACCESS_ALLOWED,    PM_OFF, PM_LPM_MODE_VAL, 128,2896, 3000, 300, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_AOP ,0 }, // LDO15 HT_LV_P150
  {PM_ACCESS_ALLOWED,    PM_OFF, PM_NPM_MODE_VAL, 128,2896, 3104, 300, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_SEN,0  }, // LDO16 MT_P150
  {PM_ACCESS_ALLOWED,    PM_OFF, PM_LPM_MODE_VAL, 128,2920, 3232, 300, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_HLOS ,0}, // LDO17 MT_P150_STEP
  {PM_ACCESS_ALLOWED,    PM_OFF, PM_LPM_MODE_VAL, 128,1800, 3000, 300, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_HLOS,0 }, // LDO18 MT_P50
  {PM_ACCESS_ALLOWED,    PM_OFF, PM_NPM_MODE_VAL, 128,2944, 3304, 300, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_HLOS,0 }, // LDO19 MT_P600
};

/*AccessAllowed, AlwaysOn, MinMode, SafetyHR, MinVoltage, MaxVoltage, EnableFixedTime, EnableStepperWarmUpTime, VoltFixedUpTime, VoltFixedDownTime, VoltStepperExtraUpTime, 
  VoltStepperExtraDownTime, VoltDischargeTime, ModeFixedTime, VregReadyEn, VregReadyErrEn, VregReadyMultiplier, Aop, 4 SeqTypes, DrvOwnMask, Reserved */
__attribute__((section("pm_ddr_reclaim_pool")))
pm_vrm_vreg_rail_info_type smps_rail_a[] = 
{
  {PM_ACCESS_DISALLOWED,PM_OFF, PM_AUTO_MODE_VAL, 0,  344, 952, 500, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ, PM_DRV_HLOS, 0}, //FTS1 APC0/APC1 
  {PM_ACCESS_ALLOWED,   PM_ON,  PM_LPM_MODE_VAL,  0,  488, 952, 500, 0, 0, 0,20,20,50, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_ARC, 0},//FTS2 VDD_MX
  {PM_ACCESS_ALLOWED,   PM_OFF, PM_AUTO_MODE_VAL, 0,  568, 648, 500, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_AOP, 0}, //FTS3 DDR
  {PM_ACCESS_ALLOWED,   PM_ON,  PM_AUTO_MODE_VAL, 0, 1640,2040, 500, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_AOP, 0}, //HFS4 Parent
  {PM_ACCESS_ALLOWED,   PM_ON,  PM_AUTO_MODE_VAL, 0,  376,1000, 500, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_AOP, 0}, //HFS5 Parent
  {PM_ACCESS_ALLOWED,   PM_OFF, PM_AUTO_MODE_VAL, 0, 1168,1392, 500, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_AOP, 0}, //HFS6 Parent
};

/*AccessAllowed, AlwaysOn, MinMode, SafetyHR, MinVoltage, MaxVoltage, EnableFixedTime, EnableStepperWarmUpTime, VoltFixedUpTime, VoltFixedDownTime, VoltStepperExtraUpTime, 
  VoltStepperExtraDownTime, VoltDischargeTime, ModeFixedTime, VregReadyEn, VregReadyErrEn, VregReadyMultiplier, Aop, 4 SeqTypes, Reserved, DrvOwnMask, DrvPriMask */ 
__attribute__((section("pm_ddr_reclaim_pool")))
pm_vrm_vreg_rail_info_type smps_rail_c[] = 
{
  {PM_ACCESS_ALLOWED,   PM_OFF, PM_AUTO_MODE_VAL, 0,  344, 1008, 500, 0, 0, 0,20,20,50,30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_ARC, 0}, //FTS1 VDD_CX S1c is Ganged Leader
  {PM_ACCESS_ALLOWED,   PM_ON,  PM_LPM_MODE_VAL,  0, 1000,1200, 500, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_AOP, 0}, //FTS2 DDR
  {PM_ACCESS_ALLOWED,   PM_OFF, PM_AUTO_MODE_VAL, 0,  344,1072, 500, 0, 0, 0,20,20,50,30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_ARC, 0}, //FTS3 GFX
  {PM_ACCESS_DISALLOWED,PM_ON,  PM_AUTO_MODE_VAL, 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_ARC , 0}, //FTS4
};


/*AccessAllowed, AlwaysOn, EnableFixedTime, EnableSeqType, Reserved, DrvOwnMask, DrvPriMask */ 
__attribute__((section("pm_ddr_reclaim_pool")))
pm_vrm_clk_info_type clk_info_a[]=
{
//{PM_ACCESS_DISALLOWED,PM_OFF, 0,  VRM_GENERIC_SEQ, 0, PM_DRV_AOP, PM_DRV_NONE},//XO *managed seperately
  {PM_ACCESS_ALLOWED,   PM_OFF, 0,  VRM_GENERIC_SEQ, 0, PM_DRV_ARC,  PM_DRV_NONE},//BB_CLK1
  {PM_ACCESS_ALLOWED,   PM_OFF, 0,  VRM_GENERIC_SEQ, 0, PM_DRV_HLOS, PM_DRV_NONE},//BB_CLK2
  {PM_ACCESS_ALLOWED,   PM_OFF, 0,  VRM_GENERIC_SEQ, 0, PM_DRV_HLOS, PM_DRV_NONE},//BB_CLK3
  {PM_ACCESS_ALLOWED,   PM_OFF, 0,  VRM_GENERIC_SEQ, 0, PM_DRV_MDMS, PM_DRV_NONE},//RF_CLK1
  {PM_ACCESS_ALLOWED,   PM_OFF, 0,  VRM_GENERIC_SEQ, 0, PM_DRV_MDMS, PM_DRV_NONE},//RF_CLK2
  {PM_ACCESS_DISALLOWED,PM_OFF, 0,  VRM_GENERIC_SEQ, 0, PM_DRV_HLOS, PM_DRV_NONE},//RF_CLK3
  {PM_ACCESS_DISALLOWED,PM_OFF, 0,  VRM_GENERIC_SEQ, 0, PM_DRV_HLOS, PM_DRV_NONE},//EMPTY
  {PM_ACCESS_DISALLOWED,PM_OFF, 0,  VRM_GENERIC_SEQ, 0, PM_DRV_HLOS, PM_DRV_NONE},//EMPTY
  {PM_ACCESS_DISALLOWED,PM_OFF, 0,  VRM_GENERIC_SEQ, 0, PM_DRV_HLOS, PM_DRV_NONE},//CLK_DIST *not supported
  {PM_ACCESS_DISALLOWED,PM_OFF, 0,  VRM_GENERIC_SEQ, 0, PM_DRV_HLOS, PM_DRV_NONE},//SLEEP_CLK
  {PM_ACCESS_DISALLOWED,PM_OFF, 0,  VRM_GENERIC_SEQ, 0, PM_DRV_HLOS, PM_DRV_NONE},//DIV_CLK1
  {PM_ACCESS_DISALLOWED,PM_OFF, 0,  VRM_GENERIC_SEQ, 0, PM_DRV_HLOS, PM_DRV_NONE},//DIV_CLK2
  {PM_ACCESS_DISALLOWED,PM_OFF, 0,  VRM_GENERIC_SEQ, 0, PM_DRV_HLOS, PM_DRV_NONE} //DIV_CLK3
};
/*Name, PmicIndex, PeriphType, PeriphId, AccessAllowed, SlaveId, BaseAddr, RsrcCategory, EnableAddrOff, EnableFixedTime, EnableSeqType, ModeAddrOff, ModeFixedTime, ModeSeqType, 
  VoltAddrOff, VoltFixedTime, VoltSeqType, MaxVoltage, DrvOwnMask, Reserved*/

__attribute__((section("pm_ddr_reclaim_pool")))
pm_vrm_target_rsrc_info_type target_rsrc_info[] = 
{
  {"pbs", 7, PM_VRM_PERIPH_PBS_CLIENT, 1,{PM_ACCESS_ALLOWED, 0xE, 0x7200, VRM_RSRC_REGULATOR, 0x42, 180, VRM_PMIC_WRITE_SEQ, 0x50, 10, VRM_PMIC_WRITE_SEQ, 0x51, 10, VRM_PMIC_WRITE_SEQ, PM_VRM_MAX_VOLTAGE, PM_DRV_AOP, 0}},
  {"vrm.spmi",0, PM_VRM_PERIPH_SPMI,  1, {PM_ACCESS_ALLOWED, 0x0, 0x5A00, VRM_RSRC_REGULATOR, 0x00, 0, VRM_NO_PMIC_SEQ, 0x00, 0, VRM_NO_PMIC_SEQ, 0xD0, 0, VRM_NO_PMIC_SEQ, PM_VRM_MAX_VOLTAGE, PM_DRV_AOP, 0}},
  {"vrm.soc", 0, PM_VRM_PERIPH_SOC,   1, {PM_ACCESS_ALLOWED, 0x0, 0x5B00, VRM_RSRC_REGULATOR, 0x00, 0, VRM_NO_PMIC_SEQ, 0x00, 0, VRM_NO_PMIC_SEQ, 0x00, 0, VRM_NO_PMIC_SEQ, 0, PM_DRV_AOP|PM_DRV_HLOS|PM_DRV_SEN|PM_DRV_MDMS, 0}},

};

uint32   num_of_target_rsrc[]= {sizeof(target_rsrc_info)/sizeof(target_rsrc_info[0])} ;

uint32   num_of_ldo[]        = {sizeof(ldo_rail_a)/sizeof(ldo_rail_a[0]), 0 , 0} ;
uint32   num_of_smps[]       = {sizeof(smps_rail_a)/sizeof(smps_rail_a[0]),0 , sizeof(smps_rail_c)/sizeof(smps_rail_c[0])};
uint32   num_of_bob[]        = {0, 0 , 0};
uint32   num_of_vs[]         = { 0, 0 , 0};

__attribute__((section("pm_dram_reclaim_pool")))
pm_vrm_vreg_rail_info_type* ldo_rail[]=
{
    ldo_rail_a, NULL , NULL
};

__attribute__((section("pm_dram_reclaim_pool")))
pm_vrm_vreg_rail_info_type* smps_rail[]=
{
    smps_rail_a, NULL , smps_rail_c
};

__attribute__((section("pm_dram_reclaim_pool")))
pm_vrm_vreg_rail_info_type* vs_rail[]=
{
    NULL, NULL , NULL
};

__attribute__((section("pm_dram_reclaim_pool")))
pm_vrm_clk_info_type* clk_info[]=
{
    clk_info_a, NULL , NULL
};

__attribute__((section("pm_dram_reclaim_pool")))
pm_vrm_vreg_rail_info_type* bob_rail[]=
{
    NULL, NULL , NULL
};

__attribute__((section("pm_dram_reclaim_pool")))
uint32 rpmh_drv_id[] = {(uint32)RSC_DRV_AOP};
