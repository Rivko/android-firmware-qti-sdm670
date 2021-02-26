#ifndef __PMICTESTWLED_H__
#define __PMICTESTWLED_H__

/*! @file PmicTestWLED.h
 *
 *  PMIC Test WLED
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
06/27/16   sarpit      Added Pmic Test WLED Header File

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include <Protocol/EFIPmicWled.h>

/*===========================================================================
                     MACRO DEFINATIONS
===========================================================================*/


/*===========================================================================
                     TYPE DECLARATIONS
===========================================================================*/

typedef enum
{
    EFI_PM_TEST_WLED_EN_AMOLED,
    EFI_PM_TEST_WLED_CONFIG_SINK_MODULATION,
	EFI_PM_TEST_WLED_SET_CURRENT_SENSE_RATIO,
	
}EFI_PM_TEST_DRIVER_WLED_TYPE_FUNCTION;

typedef struct _EFI_PM_TEST_WLED_EN_AMOLED_PARAM
{
    UINT32       DeviceIndex;
    EFI_PM_WLED_DISPLAY_TYPE display_type;

} EFI_PM_TEST_WLED_EN_AMOLED_PARAM;


typedef struct _EFI_PM_TEST_WLED_CONFIG_SINK_MODULATION_PARAM
{
    UINT32       					DeviceIndex;
	EFI_PM_WLED_MOD_CLK_TYPE       mode_clk;
	BOOLEAN							en_phase_stagger;
	EFI_PM_WLED_MOD_CLK_TYPE       accum_clk;
	BOOLEAN							en_hybrid_dim;
	EFI_PM_WLED_PWM_MODE_TYPE		pwm_mode;
	EFI_PM_WLED_MOD_DIM_MODE_TYPE  dim_mode;
} EFI_PM_TEST_WLED_CONFIG_SINK_MODULATION_PARAM;

typedef struct _EFI_PM_TEST_WLED_SET_CURRENT_SENSE_RATIO_PARAM
{
    UINT32       				DeviceIndex;
    EFI_PM_WLED_DISPLAY_TYPE 	display_type;
	UINT32       				resistance_kohm;
} EFI_PM_TEST_WLED_SET_CURRENT_SENSE_RATIO_PARAM;


typedef union _EFI_PM_TEST_WLED_FUNCTION_TYPE
{
    EFI_PM_TEST_WLED_EN_AMOLED_PARAM                				Wledenamoled;
    EFI_PM_TEST_WLED_CONFIG_SINK_MODULATION_PARAM                   Wledsinkmodulation; 
	EFI_PM_TEST_WLED_SET_CURRENT_SENSE_RATIO_PARAM					Wledcurrentsenseratio;
}EFI_PM_TEST_WLED_FUNCTION_TYPE;

/*===========================================================================
                       FUNCTION PROTOTYPES
===========================================================================*/



#endif // __PMICTESTWLED_H__


