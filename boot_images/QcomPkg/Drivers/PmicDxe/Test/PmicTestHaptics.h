#ifndef __PMICTESTHAPTICS_H__
#define __PMICTESTHAPTICS_H__

/*! @file \PmicTestHaptics.h
 *
 *  PMIC Test Haptics
 *
 *  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.  All Rights Reserved. 
 *  Qualcomm Technologies Proprietary and Confidential.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/23/16   sarpit      Added Pmic Test Haptics Header File

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include <Protocol/EFIPmicHaptics.h>

/*===========================================================================
                     MACRO DEFINATIONS
===========================================================================*/


/*===========================================================================
                     TYPE DECLARATIONS
===========================================================================*/

typedef enum
{
    EFI_PM_TEST_HAPTICS_CONFIG_HAPTICS_RATE,
    EFI_PM_TEST_HAPTICS_AUTO_RES_CONFIG,
}EFI_PM_TEST_DRIVER_HAPTICS_TYPE_FUNCTION;


typedef enum
{
  EFI_PM_HAPTICS_LRA_RES_CALIB_FOUR_PERIODS,      /*AFTER EVERY 4 LRA PERIOD*/
  EFI_PM_HAPTICS_LRA_RES_CALIB_EIGHT_PERIODS,     /*AFTER EVERY 8 LRA PERIOD*/
  EFI_PM_HAPTICS_LRA_RES_CALIB_SIXTEEN_PERIODS,   /*AFTER EVERY 16 LRA PERIOD*/
  EFI_PM_HAPTICS_LRA_RES_CALIB_THIRTYTWO_PERIODS, /*AFTER EVERY 32 LRA PERIOD*/
  EFI_PM_HAPTICS_LRA_RES_CALIB_INVALID,           /*INVALID*/
}EFI_PM_HAPTICS_LRA_RES_CALIB_TYPE; 


typedef enum
{
  EFI_PM_HAPTICS_LRA_HIGH_Z_NO_HIGHZ,   /*no high-z*/
  EFI_PM_HAPTICS_LRA_HIGH_Z_HIGHZ_OPT1, /*2 LRA period (ZXD), 1/8 LRA period (QWD)*/
  EFI_PM_HAPTICS_LRA_HIGH_Z_HIGHZ_OPT2, /*3 LRA period (ZXD), 1/4 LRA period (QWD)*/
  EFI_PM_HAPTICS_LRA_HIGH_Z_HIGHZ_OPT3, /*4 LRA period (ZXD), 1/2 LRA period (QWD)*/
  EFI_PM_HAPTICS_LRA_HIGH_Z_INVALID,    /*Invalid*/
}EFI_PM_HAPTICS_LRA_HIGH_Z_TYPE; 

typedef enum
{
  EFI_PM_HAPTICS_LRA_AUTO_RES_MODE_NO_AUTO_RES, /*no auto resonance*/
  EFI_PM_HAPTICS_LRA_AUTO_RES_MODE_ZXD,         /*Zero crossing based discontinuous method (ZXD)*/
  EFI_PM_HAPTICS_LRA_AUTO_RES_MODE_QWD,         /*Quarter wave drive method (QWD)*/
  EFI_PM_HAPTICS_LRA_AUTO_RES_MODE_MAX_QWD,     /*MAX QWD*/
  EFI_PM_HAPTICS_LRA_AUTO_RES_MODE_ZXD_EOP,     /*Zero crossing based discontinuous method with high z after end of pattern*/
  EFI_PM_HAPTICS_LRA_AUTO_RES_MODE_INVALID,     /*Invalid*/
}EFI_PM_HAPTICS_LRA_AUTO_RES_MODE_TYPE; 


typedef struct _EFI_PM_TEST_HAPTICS_CONFIG_HAPTICS_RATE_PARAM
{
    UINT32       DeviceIndex;
	UINT32       config_rate;
} EFI_PM_TEST_HAPTICS_CONFIG_HAPTICS_RATE_PARAM;

typedef struct _EFI_PM_TEST_HAPTICS_AUTO_RES_CONFIG_PARAM
{
    UINT32       DeviceIndex;
    EFI_PM_HAPTICS_LRA_RES_CALIB_TYPE CalibType;
	EFI_PM_HAPTICS_LRA_HIGH_Z_TYPE ZType;
	EFI_PM_HAPTICS_LRA_AUTO_RES_MODE_TYPE ModeType;

} EFI_PM_TEST_HAPTICS_AUTO_RES_CONFIG_PARAM;

typedef union _EFI_PM_TEST_HAPTICS_FUNCTION_TYPE
{
    EFI_PM_TEST_HAPTICS_CONFIG_HAPTICS_RATE_PARAM                Hapticsconfigrate;
    EFI_PM_TEST_HAPTICS_AUTO_RES_CONFIG_PARAM                    Hapticsautores; 

}EFI_PM_TEST_HAPTICS_FUNCTION_TYPE;

/*===========================================================================
                       FUNCTION PROTOTYPES
===========================================================================*/



#endif // __PMICTESTHAPTICS_H__


