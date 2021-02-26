#ifndef __PMICTESTAC_H__
#define __PMICTESTAC_H__

/*! @file PmicTestAC.h
 *
 *  PMIC Test AccessControl
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
08/22/16   pb      Added Pmic Test AC Header file

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/


/*===========================================================================
                     MACRO DEFINATIONS
===========================================================================*/


/*===========================================================================
                     TYPE DECLARATIONS
===========================================================================*/
typedef enum
{
    EFI_PM_AC_WRITE,

}EFI_PM_TEST_DRIVER_AC_TYPE_FUNCTION;


//SlaveID, address, data
typedef struct _EFI_PM_TEST_AC_WRITE_PARAM
{
    UINT16         SlaveID;
    UINT16         Address;
    UINT8          Data;
} EFI_PM_TEST_AC_WRITE_PARAM;

typedef union _EFI_PM_TEST_AC_FUNCTION_TYPE
{
    EFI_PM_TEST_AC_WRITE_PARAM       AccessControlWrite;

}EFI_PM_TEST_AC_FUNCTION_TYPE;

/*===========================================================================
                       FUNCTION PROTOTYPES
===========================================================================*/



#endif // __PMICTESTAC_H__




