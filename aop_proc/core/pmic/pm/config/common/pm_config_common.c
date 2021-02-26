/*! \file pm_config_common.c
 *  
 *  \brief  This file contains peripheral related settings that are common across PMICs.
 *  \n  
 *  &copy; Copyright 2013-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/config/common/pm_config_common.c#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/11/13   hs      Adding settling time for regulators.
05/08/13   aks     Clk driver code re-factoring 
04/23/13   hs      Fixed the naming convention in \config.
03/04/13   hs      Created.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_pwr_alg.h"
#include "pm_clk_driver.h"

uint32 unified_rails_stepper_rate[] = {19200, 9600, 4800, 2400}; // {stepper rate in uV/uS}

uint32 bob_stepper_rate[] = {1200}; // {stepper rate in uV/uS}

pm_clk_register_info_type clk_reg[1] = 
{
    {0x5000, 0x100, 0x046, 0x04C }
};

pm_pwr_register_info_type smps_reg[1] = 
{
    {0x1400, 0x300, 0x008, 0x011, 0x012, 0x013, 0x014, 0x015, 0x016, 0x040, 0x041, 0x042, 0x043, 0x045, 0x046, 0x047, 0x060, 0x061, 0x066, 0x039, 0x068, 0x0D0}
};

pm_pwr_register_info_type ldo_reg[1] = 
{
    {0x4000, 0x100, 0x008, 0x011, 0x012, 0x013, 0x014, 0x015, 0x016, 0x040, 0x041, 0x042, 0x043, 0x045, 0x046, 0x047, 0x060, 0x061, 0x066, 0x039, 0x068, 0x0D0}
};

pm_pwr_register_info_type vs_reg[1] = 
{
    {0x8000, 0x100, 0x008, 0x011, 0x012, 0x013, 0x014, 0x015, 0x016, 0x040, 0x041, 0x042, 0x043, 0x045, 0x046, 0x000, 0x060, 0x061, 0x000, 0x039, 0x068, 0x0D0}
};

pm_pwr_register_info_type bob_reg[1] = 
{
    {0xA000, 0x100, 0x008, 0x011, 0x012, 0x013, 0x014, 0x015, 0x016, 0x040, 0x041, 0x042, 0x043, 0x045, 0x046, 0x047, 0x060, 0x061, 0x066, 0x039, 0x068, 0x0D0}
};

pm_clk_type clk_common[PM_MAX_NUM_CLKS] = 
{
   PM_CLK_XO,
   PM_CLK_BB_1, 
   PM_CLK_BB_2, 
   PM_CLK_BB_3, 
   PM_CLK_RF_1, 
   PM_CLK_RF_2, 
   PM_CLK_RF_3, 
   PM_CLK_DIFF_1, 
   PM_CLK_LN_BB, 
   PM_CLK_DIST, 
   PM_CLK_SLEEP, 
   PM_CLK_DIV_1, 
   PM_CLK_DIV_2, 
   PM_CLK_DIV_3,
};
