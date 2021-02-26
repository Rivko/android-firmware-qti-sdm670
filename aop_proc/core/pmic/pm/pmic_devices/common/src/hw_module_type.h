#ifndef HW_MODULE_TYPE_H
#define HW_MODULE_TYPE_H

/*! \file hw_module_type.h
 *  \n
 *  \brief This file contains SMPS peripheral driver related function prototypes,
 *         enums and driver data structure type.   
 *  \n  
 *  \n &copy; Copyright 2012-2015 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/pmic_devices/common/src/hw_module_type.h#1 $
$Author: pwbldsvc $

when       who     what, where, why
-----------   ------------------------------------------------
06/04/13   aks     Remove hacks for range 
05/08/13   aks     For 8x26 LDO3 supports range 2 only 
04/20/13   hs      Fixed the numeric value of LDO N600 subtype.
04/16/13   hs      Added a fake LDO subtype.
====================================================================*/


typedef enum
{
    PM_HW_MODULE_Reserved = 0,
    PM_HW_MODULE_PON = 1,
    PM_HW_MODULE_HF_BUCK = 3,
    PM_HW_MODULE_LDO = 4,
    PM_HW_MODULE_VS = 5,
    PM_HW_MODULE_CLOCK = 6,
    PM_HW_MODULE_RTC = 7,
    PM_HW_MODULE_ADC = 8,
    PM_HW_MODULE_ALARM = 9,
    PM_HW_MODULE_INTERRUPT = 10,
    PM_HW_MODULE_INTERFACE = 11,
    PM_HW_MODULE_TRIM = 12,
    PM_HW_MODULE_MBG = 14,
    PM_HW_MODULE_GPIO = 16,
    PM_HW_MODULE_MISC = 20,
    PM_HW_MODULE_PBS = 22,
    PM_HW_MODULE_FTS = 28,
    PM_HW_MODULE_ULT_LDO = 33,
    PM_HW_MODULE_ULT_BUCK = 34,
    PM_HW_MODULE_BOB = 41,
    PM_HW_MODULE_INVALID
}pm_hw_module_type;

typedef enum
{
    PM_HW_MODULE_PON_Reserved = 0,
    PM_HW_MODULE_PON_LV_PON = 1,
    PM_HW_MODULE_PON_INVALID
}pm_hw_module_pon_subtype;



typedef enum
{
    PM_HW_MODULE_HF_BUCK_Reserved = 0,
    PM_HW_MODULE_HF_BUCK_PS1X = 1,
    PM_HW_MODULE_HF_BUCK_PS2X = 2,
    PM_HW_MODULE_HF_BUCK_PS3X = 3,
    PM_HW_MODULE_HF_BUCK_PS6X = 6,
    PM_HW_MODULE_HF_BUCK_GP_CTL = 8,
    PM_HW_MODULE_HF_BUCK_RF_CTL = 9,
    PM_HW_MODULE_HF_BUCK_VPWM_CTL = 10,
    PM_HW_MODULE_HF_BUCK_PSLV_0p5A = 16,
    PM_HW_MODULE_HF_BUCK_PSLV_1A = 17,
    PM_HW_MODULE_HF_BUCK_PSLV_1p5A = 18,
    PM_HW_MODULE_HF_BUCK_PSLV_2A = 19,
    PM_HW_MODULE_HF_BUCK_PSLV_2p5A = 20,
    PM_HW_MODULE_HF_BUCK_PSLV_3A = 21,
    PM_HW_MODULE_HF_BUCK_PSLV_3p5A = 22,
    PM_HW_MODULE_HF_BUCK_PSLV_4A = 23,
    PM_HW_MODULE_HF_BUCK_PSHV_0p5A = 32,
    PM_HW_MODULE_HF_BUCK_PSHV_1A = 33,
    PM_HW_MODULE_HF_BUCK_PSHV_1p5A = 34,
    PM_HW_MODULE_HF_BUCK_PSHV_2A = 35,
    PM_HW_MODULE_HF_BUCK_PSHV_2p5A = 36,
    PM_HW_MODULE_HF_BUCK_PSHV_3A = 37,
    PM_HW_MODULE_HF_BUCK_INVALID
}pm_hw_module_hf_buck_subtype;

typedef enum
{
    PM_HW_MODULE_ULT_BUCK_Reserved = 0,
    PM_HW_MODULE_ULT_BUCK_CTL_LV = 13,
    PM_HW_MODULE_ULT_BUCK_CTL2 = 14,
    PM_HW_MODULE_ULT_BUCK_CTL3 = 15,
    PM_HW_MODULE_ULT_BUCK_CTL_MV = 16,
    PM_HW_MODULE_ULT_BUCK_INVALID
}pm_hw_module_ult_buck_subtype;

typedef enum
{
    PM_HW_MODULE_LDO_Reserved          = 0,
    PM_HW_MODULE_LDO_N50               = 1,
    PM_HW_MODULE_LDO_N150              = 2,
    PM_HW_MODULE_LDO_N300              = 3,
    PM_HW_MODULE_LDO_N600              = 4,
    PM_HW_MODULE_LDO_N1200             = 5,
    PM_HW_MODULE_LDO_N600_STEPPER      = 6,
    PM_HW_MODULE_LDO_N1200_STEPPER     = 7,
    PM_HW_MODULE_LDO_P50               = 8,
    PM_HW_MODULE_LDO_P150              = 9,
    PM_HW_MODULE_LDO_P300              = 10,
    PM_HW_MODULE_LDO_P600              = 11,
    PM_HW_MODULE_LDO_P1200             = 12,
    PM_HW_MODULE_LDO_LN_LDO            = 16,
    PM_HW_MODULE_LDO_USB_LDO           = 24,
    PM_HW_MODULE_LDO_LV_P50            = 40,
    PM_HW_MODULE_LDO_LV_P150           = 41,
    PM_HW_MODULE_LDO_LV_P300           = 42,
    PM_HW_MODULE_LDO_LV_P600           = 43,
    PM_HW_MODULE_LDO_LV_P1200          = 44,
    PM_HW_MODULE_LDO_HT_N300_STEPPER   = 48,
    PM_HW_MODULE_LDO_HT_N600_STEPPER   = 49,
    PM_HW_MODULE_LDO_HT_N1200_STEPPER  = 50,
    PM_HW_MODULE_LDO_HT_P50            = 52,
    PM_HW_MODULE_LDO_HT_P150           = 53,
    PM_HW_MODULE_LDO_HT_P300           = 54,
    PM_HW_MODULE_LDO_HT_P600           = 55,
    PM_HW_MODULE_LDO_HT_P1200          = 56,
    PM_HW_MODULE_LDO_HT_LV_P50         = 58,
    PM_HW_MODULE_LDO_HT_LV_P150        = 59,
    PM_HW_MODULE_LDO_HT_LV_P300        = 60,
    PM_HW_MODULE_LDO_HT_LV_P600        = 61,
    PM_HW_MODULE_LDO_HT_LV_P1200       = 62,
    PM_HW_MODULE_LDO_HT_P50_SP         = 64, 
    PM_HW_MODULE_LDO_500_N300_100      = 83,
    PM_HW_MODULE_LDO_INVALID,
}pm_hw_module_ldo_subtype;

typedef enum
{
    PM_HW_MODULE_ULT_LDO_Reserved = 0,
    PM_HW_MODULE_ULT_LDO_N50 = 1,
    PM_HW_MODULE_ULT_LDO_N150 = 2,
    PM_HW_MODULE_ULT_LDO_N300 = 3,
    PM_HW_MODULE_ULT_LDO_N600 = 4,
    PM_HW_MODULE_ULT_LDO_N1200 = 5,
    PM_HW_MODULE_ULT_LDO_N600_STEPPER = 6,
    PM_HW_MODULE_ULT_LDO_N1200_STEPPER = 7,
    PM_HW_MODULE_ULT_LDO_P50 = 8,
    PM_HW_MODULE_ULT_LDO_P150 = 9,
    PM_HW_MODULE_ULT_LDO_P300 = 10,
    PM_HW_MODULE_ULT_LDO_P600 = 11,
    PM_HW_MODULE_ULT_LDO_P1200 = 12,
    PM_HW_MODULE_ULT_LDO_N900 = 19,
    PM_HW_MODULE_ULT_LDO_N900_STEPPER = 20,
    PM_HW_MODULE_ULT_LDO_N300_STEPPER = 21,
    PM_HW_MODULE_ULT_LDO_LV_P50 = 40,
    PM_HW_MODULE_ULT_LDO_LV_P150 = 41,
    PM_HW_MODULE_ULT_LDO_LV_P300 = 42,
    PM_HW_MODULE_ULT_LDO_LV_P600 = 43,
    PM_HW_MODULE_ULT_LDO_LV_P1200 = 44,
    PM_HW_MODULE_ULT_LDO_P450 = 45,
    PM_HW_MODULE_ULT_LDO_INVALID,
}pm_hw_module_ult_ldo_subtype;


typedef enum
{
    PM_HW_MODULE_CLOCK_Reserved = 0,
    PM_HW_MODULE_CLOCK_XO_CORE = 1,
    PM_HW_MODULE_CLOCK_CLK_DIST = 2,
    PM_HW_MODULE_CLOCK_XO_CORE_38P4MHZ = 3,
    PM_HW_MODULE_CLOCK_BB_CLK = 8,
    PM_HW_MODULE_CLOCK_RF_CLK = 9,
    PM_HW_MODULE_CLOCK_DIFF_CLK = 10,
    PM_HW_MODULE_CLOCK_DIV_CLK = 11,
    PM_HW_MODULE_CLOCK_SLP_CLK = 12,
    PM_HW_MODULE_CLOCK_CALRC2 = 14,
    PM_HW_MODULE_CLOCK_INVALID,
}pm_hw_module_clk_subtype;


typedef enum
{
    PM_HW_MODULE_MISC_Reserved = 0,
    PM_HW_MODULE_MISC_MISC8941 = 1,
    PM_HW_MODULE_MISC_MISC8841 = 2,
    PM_HW_MODULE_MISC_INVALID,
}pm_hw_module_misc_subtype;

typedef enum
{
    PM_HW_MODULE_FTS__Reserved = 0,
    PM_HW_MODULE_FTS__PS1X = 1,
    PM_HW_MODULE_FTS__PS0p5X = 2,
    PM_HW_MODULE_FTS__FTS2p5_PS = 3,
    PM_HW_MODULE_FTS__FTS426_PS = 6,
    PM_HW_MODULE_FTS__FTS_CTL = 8,
    PM_HW_MODULE_FTS__FTS2p5_CTL = 9,
    PM_HW_MODULE_FTS__FTS426_CTL = 10,
    PM_HW_MODULE_FTS__FTS510_CTL = 11,
    PM_HW_MODULE_FTS__INVALID,
}pm_hw_module_fts_subtype;

typedef enum
{
    PM_HW_MODULE_PBS_Reserved = 0,
    PM_HW_MODULE_PBS_CORE = 1,
    PM_HW_MODULE_PBS_CLIENT = 8,
    PM_HW_MODULE_PBS_INVALID,
}pm_hw_module_pbs_subtype;


typedef enum
{
    PM_HW_MODULE_BOB_CONFIG = 3,
    PM_HW_MODULE_BOB_MONITORING = 4,
    PM_HW_MODULE_BOB510_CONFIG = 5,
    PM_HW_MODULE_BOB510_MONITORING = 6,
    PM_HW_MODULE_BOB_INVALID,
}pm_hw_module_bob_subtype;

#endif //HW_MODULE_TYPE_H
