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

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/config/sdm670/pm_config_target.c#5 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/25/18   as      Keeping LDO8a OFF for UFS Use case Client side will take care of RBSC state ( CR#2320239 )
06/12/18   as      Keeping LPI_CX MinPowerMode to NPM ( CR#2258595 )
05/03/18   as      keeping min Pwr mode of MSS to NPM (CR#2235015)
03/20/18   as      Keeping MinPowerMode of L6a,L9a,L18a,L4b,L8b to LPM mode( CR#2193238 )
02/07/18   as      Remving vrm.soc and vrm.spmi from target specific (CR#2185624 )
01/19/18   as      LDO1A and S6A RBSC OFF ( CR#2143156 )
01/16/18   as      LDO1B RBSC OFF ( CR#2138657 )
01/12/17   as      Increasing max limit of MSS rail ( S5A ) to 1000mV ( CR#2171288 )
01/05/17   as      Adding extra voltage up and down time for CPR controlled Rail (CR#2164987)
11/13/17   as      Changing DrvOwnMask on LDO2B to HLOS (CR#2069199) 
11/10/17   as      Adding DBU2 support (CR#2135566)
07/10/17   as      Updating config changes for SDM670 (CR#2069199)
01/17/14   kt      Created.
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
  {PM_ACCESS_ALLOWED,    PM_OFF,  PM_LPM_MODE_VAL, 40, 1096, 1296,  300, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ, PM_DRV_HLOS,0 }, // LDO1 N600_STEP 
  {PM_ACCESS_ALLOWED,    PM_OFF, PM_LPM_MODE_VAL, 24, 896, 1096, 300, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,   PM_DRV_MDMS,0 }, // LDO2 HT_N1200_STEP
  {PM_ACCESS_ALLOWED,    PM_OFF, PM_LPM_MODE_VAL, 40, 896, 1096, 300, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,   PM_DRV_MDMS,0 }, // LDO3 MT_N600_STEP
  {PM_ACCESS_DISALLOWED, PM_ON,  PM_NPM_MODE_VAL, 36, 500,  1084, 300, 0, 0, 0,20,20,100,30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_2X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_ARC ,0 },  // LDO4 Not defined for Groot
  {PM_ACCESS_ALLOWED,    PM_OFF, PM_NPM_MODE_VAL, 24, 488,  800,  300, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_MDMS,0 }, // LDO5 HT_N600_STEP WCSS_MX/CX
  {PM_ACCESS_ALLOWED,    PM_OFF, PM_LPM_MODE_VAL, 24, 1248, 1352, 300, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_MDMS,0 }, // LDO6 HT_N600
  {PM_ACCESS_ALLOWED,    PM_OFF, PM_LPM_MODE_VAL, 24, 1096, 1304, 300, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_MDMS,0 },  // LDO7 HT_N1200
  {PM_ACCESS_ALLOWED,    PM_OFF,  PM_LPM_MODE_VAL, 40, 1696, 1904, 300, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_HLOS,0 }, // LDO8 MT_LV_P600 
  {PM_ACCESS_ALLOWED,    PM_OFF, PM_LPM_MODE_VAL, 24, 1696, 1904, 300, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_MDMS,0 }, // LDO9 HT_P150
  {PM_ACCESS_ALLOWED,    PM_OFF, PM_LPM_MODE_VAL, 24, 1696, 1904, 50,  0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_1X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_HLOS,0 }, // LDO10 HT_P300
  {PM_ACCESS_ALLOWED,    PM_OFF, PM_LPM_MODE_VAL, 40, 1696, 1904, 300, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_HLOS,0 }, // LDO11 MT_P150_STEP
  {PM_ACCESS_ALLOWED,    PM_OFF, PM_LPM_MODE_VAL, 24, 1696, 1904, 300, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_MDMS,0 }, // LDO12 HT_LV_P300
  {PM_ACCESS_ALLOWED,    PM_ON,  PM_NPM_MODE_VAL, 40, 1800, 1800, 300, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN,VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,   PM_DRV_AOP ,0 }, // LDO13 MT_LV_P600
  {PM_ACCESS_ALLOWED,    PM_OFF, PM_LPM_MODE_VAL, 40, 1696, 1904, 300, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_SEN ,0 }, // LDO14 MT_LV_P150
  {PM_ACCESS_ALLOWED,    PM_OFF, PM_LPM_MODE_VAL, 128,1696, 3000, 300, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_DIS, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_AOP ,0 }, // LDO15 HT_LV_P150
  {PM_ACCESS_ALLOWED,    PM_OFF, PM_LPM_MODE_VAL, 128,2600, 2800, 300, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_MDMS,0 }, // LDO16 MT_P150
  {PM_ACCESS_ALLOWED,    PM_OFF, PM_LPM_MODE_VAL, 128,1696, 3000, 300, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_DIS, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_AOP ,0 }, // LDO17 MT_P150_STEP
  {PM_ACCESS_ALLOWED,    PM_OFF, PM_LPM_MODE_VAL, 128,2600, 2800, 300, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_MDMS,0 }, // LDO18 MT_P50
  {PM_ACCESS_ALLOWED,    PM_OFF, PM_LPM_MODE_VAL, 128,3000, 3400, 300, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_MDMS,0 }, // LDO19 MT_P600
};

  
  /*AccessAllowed, AlwaysOn, MinMode, SafetyHR, MinVoltage, MaxVoltage, EnableFixedTime, EnableStepperWarmUpTime, VoltFixedUpTime, VoltFixedDownTime, VoltStepperExtraUpTime, 
  VoltStepperExtraDownTime, VoltDischargeTime, ModeFixedTime, VregReadyEn, VregReadyErrEn, VregReadyMultiplier, Aop, 4 SeqTypes, DrvOwnMask, Reserved */
__attribute__((section("pm_ddr_reclaim_pool")))
pm_vrm_vreg_rail_info_type ldo_rail_b[] = 
{
  {PM_ACCESS_ALLOWED, PM_OFF,  PM_LPM_MODE_VAL, 40,  696,  928,  300, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ, PM_DRV_MDMS, 0}, // LDO1 MT_N300_STEP
  {PM_ACCESS_ALLOWED, PM_OFF, PM_LPM_MODE_VAL, 128, 1696, 3000, 300, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ, PM_DRV_HLOS, 0}, // LDO2 MT_P50_STEP
  {PM_ACCESS_ALLOWED, PM_OFF, PM_LPM_MODE_VAL, 128, 2944, 3304, 300, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ, PM_DRV_SEN , 0}, // LDO3 MT_P600_STEP
  {PM_ACCESS_ALLOWED, PM_OFF, PM_LPM_MODE_VAL, 128, 2944, 3304, 300, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_2X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ, PM_DRV_HLOS, 0}, // LDO4 MT_P600_STEP
  {PM_ACCESS_ALLOWED, PM_OFF, PM_LPM_MODE_VAL, 128, 2944, 3304, 300, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_DIS,VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ, PM_DRV_HLOS, 0}, // LDO5 MT_P600_STEP Settling_error disable on SD card rail
  {PM_ACCESS_ALLOWED, PM_OFF, PM_LPM_MODE_VAL, 128, 2496, 3300, 300, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ, PM_DRV_HLOS, 0}, // LDO6 MT_P50
  {PM_ACCESS_ALLOWED, PM_OFF, PM_LPM_MODE_VAL, 128, 3088, 3088, 300, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ, PM_DRV_HLOS, 0}, // LDO7 MT_P150
  {PM_ACCESS_ALLOWED, PM_OFF, PM_LPM_MODE_VAL, 128, 3000, 3400, 300, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ, PM_DRV_MDMS, 0}, // LDO8 MT_P600_STEP 
  {PM_ACCESS_ALLOWED, PM_ON,  PM_NPM_MODE_VAL, 24,  352,  992, 300, 0, 0, 0, 20, 20, 50, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN,VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ , PM_DRV_ARC , 0}, //  LDO9 HT_N600 lpi_cx
  {PM_ACCESS_ALLOWED, PM_ON,  PM_LPM_MODE_VAL, 24,  488,  1000, 300, 0, 0, 0, 20, 20, 50, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN,VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ , PM_DRV_ARC , 0}, //  LDO10 HT_N300 scc_mx
};

/*AccessAllowed, AlwaysOn, MinMode, SafetyHR, MinVoltage, MaxVoltage, EnableFixedTime, EnableStepperWarmUpTime, VoltFixedUpTime, VoltFixedDownTime, VoltStepperExtraUpTime, 
  VoltStepperExtraDownTime, VoltDischargeTime, ModeFixedTime, VregReadyEn, VregReadyErrEn, VregReadyMultiplier, Aop, 4 SeqTypes, DrvOwnMask, Reserved */
__attribute__((section("pm_ddr_reclaim_pool")))
pm_vrm_vreg_rail_info_type smps_rail_a[] = 
{
  {PM_ACCESS_DISALLOWED, PM_OFF,PM_AUTO_MODE_VAL, 0,  344, 952, 500, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,   PM_DRV_HLOS , 0}, //FTS1 APC0 Need to be check 
  {PM_ACCESS_DISALLOWED, PM_OFF, PM_LPM_MODE_VAL,  0,    0,    0, 500, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_HLOS , 0},//FTS2
  {PM_ACCESS_DISALLOWED, PM_OFF, PM_AUTO_MODE_VAL, 0,  348,  1200, 500, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ, PM_DRV_HLOS , 0}, //FTS3 APC1 S3A is gang leader
  {PM_ACCESS_ALLOWED,    PM_ON, PM_AUTO_MODE_VAL, 0, 1808, 2040, 500, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_AOP  , 0}, //HFS4 Parent
  {PM_ACCESS_ALLOWED,    PM_OFF, PM_NPM_MODE_VAL, 0, 344, 1000, 500, 0, 0, 0, 20, 20, 50, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_ARC  , 0}, //HFS5  MSS
  {PM_ACCESS_ALLOWED,    PM_OFF, PM_AUTO_MODE_VAL,  0, 1224, 1400, 500, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_AOP ,  0}, //HFS6 Parent
};

/*AccessAllowed, AlwaysOn, MinMode, SafetyHR, MinVoltage, MaxVoltage, EnableFixedTime, EnableStepperWarmUpTime, VoltFixedUpTime, VoltFixedDownTime, VoltStepperExtraUpTime, 
  VoltStepperExtraDownTime, VoltDischargeTime, ModeFixedTime, VregReadyEn, VregReadyErrEn, VregReadyMultiplier, Aop, 4 SeqTypes, Reserved, DrvOwnMask, DrvPriMask */ 
__attribute__((section("pm_ddr_reclaim_pool")))
pm_vrm_vreg_rail_info_type smps_rail_b[] = 
{
  {PM_ACCESS_ALLOWED,   PM_ON , PM_AUTO_MODE_VAL, 0,  488, 996, 500, 0, 0, 0, 20, 20, 50,30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,    PM_DRV_ARC , 0}, //FTS1 VDD_MX
  {PM_ACCESS_ALLOWED,   PM_OFF,  PM_LPM_MODE_VAL,  0,  344, 1068, 500, 0, 0, 0, 20, 20, 50, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_ARC , 0}, //FTS2 GFX
  {PM_ACCESS_ALLOWED,   PM_OFF,  PM_AUTO_MODE_VAL, 0,  344,  996, 500, 0, 0, 0, 20, 20, 50, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_ARC, 0}, //FTS3  VDD_CX S3B is gang leader
  {PM_ACCESS_DISALLOWED,PM_ON,   PM_AUTO_MODE_VAL, 0,    0,    0, 500, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_ARC , 0}, //FTS4 VDD_CX
  {PM_ACCESS_ALLOWED,   PM_OFF,  PM_AUTO_MODE_VAL, 0,  568,  648, 500, 0, 0, 0, 0, 0, 0, 30, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ,  PM_DRV_AOP , 0}, //FTS5  DDR
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


/*AccessAllowed, AlwaysOn, MinMode, SafetyHR, MinVoltage, MaxVoltage, EnableFixedTime, EnableStepperWarmUpTime, VoltFixedUpTime, VoltFixedDownTime, VoltStepperExtraUpTime, 
  VoltStepperExtraDownTime, VoltDischargeTime, ModeFixedTime, VregReadyEn, VregReadyErrEn, VregReadyMultiplier, Aop, 4 SeqTypes, Reserved, DrvOwnMask, DrvPriMask */
__attribute__((section("pm_ddr_reclaim_pool")))
pm_vrm_vreg_rail_info_type bob_rail_b[] = 
{
  {PM_ACCESS_ALLOWED, PM_ON, PM_BOB_MODE_AUTO, 0, 3008, 4000, 800, 0, 0, 0, 0, 0, 0, 100, PM_SETTLING_EN, PM_SETTLING_ERR_EN, VRM_8X_SETTLING_TIMER, 0, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_GENERIC_SEQ, VRM_NO_PMIC_SEQ , PM_DRV_AOP , 0 }, //BOB1 Parent
};

/*Name, PmicIndex, PeriphType, PeriphId, AccessAllowed, SlaveId, BaseAddr, RsrcCategory, EnableAddrOff, EnableFixedTime, EnableSeqType, ModeAddrOff, ModeFixedTime, ModeSeqType, 
  VoltAddrOff, VoltFixedTime, VoltSeqType, MaxVoltage, DrvOwnMask, Reserved*/

__attribute__((section("pm_ddr_reclaim_pool")))
pm_vrm_target_rsrc_info_type target_rsrc_info[] = 
{
  {"pbs", 7, PM_VRM_PERIPH_PBS_CLIENT, 1,{PM_ACCESS_ALLOWED, 0xE, 0x7200, VRM_RSRC_REGULATOR, 0x42, 180, VRM_PMIC_WRITE_SEQ, 0x50, 10, VRM_PMIC_WRITE_SEQ, 0x51, 10, VRM_PMIC_WRITE_SEQ, PM_VRM_MAX_VOLTAGE, PM_DRV_AOP, 0}},
  {"gpiob1",1, PM_VRM_PERIPH_GPIO,     1, {PM_ACCESS_ALLOWED, 0x2, 0xC000, VRM_RSRC_REGULATOR, 0x44, 0, VRM_PMIC_WRITE_SEQ, 0x0, 0, VRM_NO_PMIC_SEQ, 0xD0, 0,  VRM_NO_PMIC_SEQ, 1200, PM_DRV_AOP, 0}},
  {"gpiob12",1, PM_VRM_PERIPH_GPIO,   12,{PM_ACCESS_ALLOWED, 0x2, 0xCB00, VRM_RSRC_REGULATOR, 0x44, 0, VRM_PMIC_WRITE_SEQ, 0x0, 0, VRM_NO_PMIC_SEQ, 0x0, 0,  VRM_NO_PMIC_SEQ, 1200, PM_DRV_AOP, 0}},
};

uint32   num_of_target_rsrc[]= {sizeof(target_rsrc_info)/sizeof(target_rsrc_info[0])} ;

uint32   num_of_ldo[]        = {sizeof(ldo_rail_a)/sizeof(ldo_rail_a[0]), sizeof(ldo_rail_b)/sizeof(ldo_rail_b[0])} ;
uint32   num_of_smps[]       = {sizeof(smps_rail_a)/sizeof(smps_rail_a[0]), sizeof(smps_rail_b)/sizeof(smps_rail_b[0])};
uint32   num_of_bob[]        = {0, sizeof(bob_rail_b)/sizeof(bob_rail_b[0])};
uint32   num_of_vs[]         = { 0, 0 };

__attribute__((section("pm_dram_reclaim_pool")))
pm_vrm_vreg_rail_info_type* ldo_rail[]=
{
    ldo_rail_a, ldo_rail_b
};

__attribute__((section("pm_dram_reclaim_pool")))
pm_vrm_vreg_rail_info_type* smps_rail[]=
{
    smps_rail_a, smps_rail_b
};

__attribute__((section("pm_dram_reclaim_pool")))
pm_vrm_vreg_rail_info_type* vs_rail[]=
{
    NULL, NULL
};

__attribute__((section("pm_dram_reclaim_pool")))
pm_vrm_clk_info_type* clk_info[]=
{
    clk_info_a, NULL
};

__attribute__((section("pm_dram_reclaim_pool")))
pm_vrm_vreg_rail_info_type* bob_rail[]=
{
    NULL, bob_rail_b
};

__attribute__((section("pm_dram_reclaim_pool")))
uint32 rpmh_drv_id[] = {(uint32)RSC_DRV_AOP};
