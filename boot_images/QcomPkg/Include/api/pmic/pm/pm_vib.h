#ifndef __PM_VIB_H__
#define __PM_VIB_H__

/*! \file  pm_uefi_vib.h 
*  \n
*  \brief   PMIC-VIB MODULE RELATED DECLARATION
*  \details  This file contains functions prototypes and variable/type/constant
*  declarations for supporting Vibrator motor driver for the Qualcomm PMIC 
*  chip set.
*
*
*  \n &copy; Copyright 2012-2017 QUALCOMM Technologies Incorporated, All Rights Reserved.
*/

/* =======================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/30/14   dm      Changes for Slew rate configuration
05/08/14   dm      Changes for 8994 Haptics support
07/27/12   al	   Renamed all Enums,added device index and resource index
12/19/11   sm      Port to UEFI
========================================================================== */

/*===========================================================================
                INCLUDE FILES FOR MODULE
===========================================================================*/
#include "com_dtypes.h"
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"


/*=============================================================================
                    LOCAL VARIABLE DEFINITIONS
=============================================================================*/

typedef enum
{
  PM_VIB_DRV_VOLT__1200MV = 0X0C,
  PM_VIB_DRV_VOLT__1300MV,
  PM_VIB_DRV_VOLT__1400MV,
  PM_VIB_DRV_VOLT__1500MV,
  PM_VIB_DRV_VOLT__1600MV,
  PM_VIB_DRV_VOLT__1700MV,
  PM_VIB_DRV_VOLT__1800MV,
  PM_VIB_DRV_VOLT__1900MV,
  PM_VIB_DRV_VOLT__2000MV,
  PM_VIB_DRV_VOLT__2100MV,
  PM_VIB_DRV_VOLT__2200MV,
  PM_VIB_DRV_VOLT__2300MV,
  PM_VIB_DRV_VOLT__2400MV,
  PM_VIB_DRV_VOLT__2500MV,
  PM_VIB_DRV_VOLT__2600MV,
  PM_VIB_DRV_VOLT__2700MV,
  PM_VIB_DRV_VOLT__2800MV,
  PM_VIB_DRV_VOLT__2900MV,
  PM_VIB_DRV_VOLT__3000MV,
  PM_VIB_DRV_VOLT__3100MV,
  PM_VIB_DRV_VOLT__INVALID
} pm_vib_drv_volt_type;

typedef struct 
{
  uint8  pm_vib_drv_r_w__mask;                            
  uint8  pm_vib_drv_r_w__drv_vib_sel_mask;     
  uint8  pm_vib_drv_r_w__vib_logic_mask;
  uint8  pm_vib_drv_r_w__vib_ensel_mask;                                 
}pm_vib_drv_mask_type;

typedef enum
{
  PM_VIB__1,
  PM_VIB__INVALID
}pm_vib_which_type;

typedef enum
{
  PM_VIB_MODE__MANUAL,
  PM_VIB_MODE__DBUS1,
  PM_VIB_MODE__DBUS2,
  PM_VIB_MODE__DBUS3,
  PM_VIB_MODE__INVALID
}pm_vib_mode_type;

typedef enum
{
  PM_VIB_POL__ACTIVE_HIGH,
  PM_VIB_POL__ACTIVE_LOW,
  PM_VIB_POL__INVALID
}pm_vib_pol_type;

typedef struct pm_dev_status_type
{
  boolean         enabled[PM_VIB_MODE__INVALID];
  uint16          voltage;
  pm_vib_pol_type polarity;
} pm_vib_status_type;

//Macros for 8994 Haptics support
#define LRA_AUTORES_CALIB_SBL_ONLY 1

#define VIB_CFG1_ACTUATOR_TYPE_VALUE             0      //0-LRA,1-ERM (Bit - 0), reg - 0x1C04C 
#define VIB_CFG1_REG                             0xC04C //Reg Address without Slave ID
#define VIB_CFG1_REG_MASK                        0x01

#define VIB_CFG2_LRA_RES_TYPE_VALUE              1      //0 - SINUSOID (Bit - 0) reg - 0x1C04D
                                                        //1 - SQUARE
#define VIB_CFG2_REG                             0xC04D //Reg Address Without Slave Id
#define VIB_CFG2_REG_MASK                        0x01

#define VIB_AUTO_RES_CFG_LRA_AUTO_RES_MODE_BIT_SHIFT 4

#define VIB_AUTO_RES_CFG_REG                     0xC04F //Reg Address without Slave Id
#define VIB_AUTO_RES_CFG_LRA_NO_AUTO_RES_VAL     0
#define VIB_AUTO_RES_CFG_LRA_AUTO_RES_MODE_VAL   4 //0 - No_Auto_Res (Bits - 6:4) reg - 0x1C04F
                                                   //1 - ZXD
                                                   //2 - QWD
                                                   //3 - MAX_QWD
                                                   //4 - ZXD_EOP
#define VIB_AUTO_RES_CFG_LRA_AUTO_RES_MODE_MASK  0x70


#define VIB_AUTO_RES_CFG_LRA_HIGH_Z_VAL          3 //0 - NO_HIGHZ (Bits - 3:2) reg - 0x1C04F
                                                   //1 - HIGHZ_OPT1
                                                   //2 - HIGHZ_OPT2
                                                   //3 - HIGHZ_OPT3
#define VIB_AUTO_RES_CFG_LRA_HIGH_Z_MASK         0x0C
#define VIB_AUTO_RES_CFG_LRA_HIGH_Z_BIT_SHIFT    2

#define VIB_AUTO_RES_CFG_LRA_RES_CALIB_VAL       3 //0 - FOUR_PERIODS (Bits - 1:0) reg - 0x1C04F
                                                   //1 - EIGHT_PERIODS
                                                   //2 - SIXTEEN_PERIODS
                                                   //3 - THIRTYTWO_PERIODS
#define VIB_AUTO_RES_CFG_LRA_RES_CALIB_MASK      0x03



#define VIB_ILIM_SEL_VAL                         0 //0 - ILIM_400MA (Bits - 0) reg - 0x1C052
                                                   //1 - ILIM_800MA
#define VIB_ILIM_REG                             0xC052
#define VIB_ILIM_SEL_MASK                        0x01


#define VIB_PWM_CAP_SEL_VAL                      1 //0 - PWM_CAP_26PF (Bits: 1- 0) reg - 0x1C058
                                                   //1 - PWM_CAP_13PF
                                                   //2 - PWM_CAP_8P7PF
                                                   //3 - PWM_CAP_6P5PF
#define VIB_PWM_CAP_REG                          0xC058
#define VIB_PWM_CAP_SEL_MASK                     0x03


#define VIB_EXTERNAL_PWM_FREQ_SEL_VAL            0 //0 - EXT_PWM_25KHZ (Bits: 1 - 0) reg - 0x1C057
                                                   //1 - EXT_PWM_50KHZ
                                                   //2 - EXT_PWM_75KHZ
                                                   //3 - EXT_PWM_100KHZ
#define VIB_EXTERNAL_PWM_REG                     0xC057
#define VIB_EXTERNAL_PWM_FREQ_SEL_MASK           0x03
                                                   
#define VIB_INTERNAL_PWM_FREQ_SEL_VAL            1 //0 - PWM_253KHZ (Bits: 1 - 0) reg - 0x1C056
                                                   //1 - PWM_505KHZ
                                                   //2 - PWM_739KHZ
                                                   //3 - PWM_1076KHZ
#define VIB_INTERNAL_PWM_REG                     0xC056
#define VIB_INTERNAL_PWM_FREQ_SEL_MASK           0x03

#define VIB_RATE_CFG1_REG                        0xC054 // Bits: 7 - 0; reg - 0x1C054
#define VIB_RATE_CFG1_VAL                        0xCF   // Corresponds to 207, with value 3 in CFG2, it is 205 Hz

#define VIB_RATE_CFG2_REG                        0xC055 // Bits: 7 - 0; reg - 0x1C055
#define VIB_RATE_CFG2_VAL                        0x03   // with value with value 207 in CFG1, it is 205 Hz

#define VIB_CFG2_REG                             0xC04D // Bits: 0; reg - 0x1C04D
#define VIB_CFG2_LRA_RES_TYPE_VAL                1      // Default Square Wave

#define VIB_SLEW_CFG                             0xC050 //Bits: 0; reg - 0x1C050
#define VIB_SLEW_RATE_VAL                        1      //Default 0 - 6 ns; 1- 16 ns


/*===========================================================================

                 VIBRATOR DRIVER FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================

FUNCTION pm_vib_enable

DESCRIPTION
  This function enables a specific mode of the vibrator.

  Enabling manual mode will force the vibrator on. Enabling the DBUS lines
  allows for an external source to enable/disable the vibrator, such as
  a LPG.

PARAMETERS
   1) Name: pmic_device_index
      - Selects which PMIC.
   2) Name: vib
      - Selects which vibrator peripheral.
   3) Name: mode
      - Mode to enable/disable.
   3) Name: enable
      - Boolean to enable/disable the given mode.

RETURN VALUE
  return value type: pm_err_flag_type.
    PM_ERR_FLAG_FEATURE_NOT_SUPPORTED    = Feature not available on this
                                            version of the PMIC.
    PM_ERR_FLAG_INVALID_PERIPHERAL    = The requested vibrator doesn't exist.
    PM_ERR_FLAG_BUS_ERR               = Communication with PM chip failed.
                                            with the PMIC.
    PM_ERR_FLAG_INVALID_PARAMETER        = Input Parameter is out of range.
    PM_ERR_FLAG_SUCCESS                  = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) pm_init()

===========================================================================*/
pm_err_flag_type pm_vib_enable
(
  unsigned          pmic_device_index,
  pm_vib_which_type vib,
  pm_vib_mode_type  mode,
  boolean           enable
);

/*===========================================================================

FUNCTION pm_vib_set_volt

DESCRIPTION
  This function configures the voltage setting for the Vibrator Motor.

PARAMETERS
   1) Name: pmic_device_index
      - Selects which PMIC.
   2) Name: vib
      - Selects which vibrator peripheral.
   3) Name: voltage
      - Voltage in mV to use for the vibrator.
      - Valid inputs: 1200 - 3100

RETURN VALUE
  return value type: pm_err_flag_type.
    PM_ERR_FLAG_FEATURE_NOT_SUPPORTED    = Feature not available on this
                                            version of the PMIC.
    PM_ERR_FLAG_INVALID_PERIPHERAL   = The requested vibrator doesn't exist.
    PM_ERR_FLAG_BUS_ERR              = Communication with PM chip failed.
                                            with the PMIC.
    PM_ERR_FLAG_INVALID_PARAMETER        = Input Parameter is out of range.
    PM_ERR_FLAG_SUCCESS                  = SUCCESS.

DEPENDENCIES
  Call the functions listed bellow before calling this function:
  1) pm_init()

===========================================================================*/
pm_err_flag_type pm_vib_set_volt
(
  unsigned          pmic_device_index,
  pm_vib_which_type vib,
  uint16            voltage
);


#endif /* __PM_VIB_H__ */

