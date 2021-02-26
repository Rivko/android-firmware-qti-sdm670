#ifndef __PMICTESTBOB_H__
#define __PMICTESTBOB_H__

/*! @file PmicTestBOB.h
 *
 *  PMIC Test BOB
 *
 *  Copyright (c) 2017-2017 Qualcomm Technologies, Inc.  All Rights Reserved. 
 *  Qualcomm Technologies Proprietary and Confidential.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/27/17   pb      Added Pmic Test BOB Header file

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include <Include/api/pmic/pm/pm_bob.h>

/*===========================================================================
                     MACRO DEFINATIONS
===========================================================================*/


/*===========================================================================
                     TYPE DECLARATIONS
===========================================================================*/
typedef enum
{
   EFI_PM_BOB_VOLT_LEVEL_STATUS, /**<BOB voltage level status*/
}EFI_PM_TEST_DRIVER_BOB_TYPE_FUNCTION;


typedef struct _EFI_PM_BOB_VOLT_LEVEL_STATUS_PARAM
{
   UINT8 PmicChip;
   UINT8 PeripheralIndex;
   pm_volt_level_type *VoltLevel;
   
} EFI_PM_TEST_BOB_VOLT_LEVEL_STATUS_PARAM;

typedef union _EFI_PM_TEST_BOB_FUNCTION_TYPE
{
   EFI_PM_TEST_BOB_VOLT_LEVEL_STATUS_PARAM          BobVoltLevelStatus;
}EFI_PM_TEST_BOB_FUNCTION_TYPE;

/*===========================================================================
                       FUNCTION PROTOTYPES
===========================================================================*/



#endif // __PMICTESTBOB_H__


