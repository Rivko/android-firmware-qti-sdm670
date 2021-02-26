#ifndef __PMICTESTCOINCELL_H__
#define __PMICTESTCOINCELL_H__

/*! @file PMICTESTPON.h
 *
 *  PMIC Test CoinCell
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
07/20/16   sarpit      Added CoinCell Header file

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
    EFI_PM_TEST_COINCELL_ENABLE,
    EFI_PM_TEST_COINCELL_GET_STATUS,
    EFI_PM_TEST_COINCELL_SET_CHARGE_CURRENT_LIMIT_RESISTOR, 
    EFI_PM_TEST_COINCELL_SET_CHARGING_VOLTAGE,

}EFI_PM_TEST_DRIVER_COINCELL_TYPE_FUNCTION;


typedef enum 
{
  EFI_PM_COINCELL_RSET_2K1,
  EFI_PM_COINCELL_RSET_1K7,
  EFI_PM_COINCELL_RSET_1K2,
  EFI_PM_COINCELL_RSET_0K8,
  EFI_PM_COINCELL_RSET_INVALID,
}EFI_PM_COINCELL_CHG_RSET_TYPE;

/*! \enum pm_coincell_chg_vset_type
 *	\brief Select Charging volatage value
*/
typedef enum 
{
  EFI_PM_COINCELL_VSET_2V5,
  EFI_PM_COINCELL_VSET_3V2,
  EFI_PM_COINCELL_VSET_3V1,
  EFI_PM_COINCELL_VSET_3V0,
  EFI_PM_COINCELL_VSET_INVALID,
}EFI_PM_COINCELL_CHG_VSET_TYPE;


typedef struct _EFI_PM_TEST_COINCELL_ENABLE_PARAM
{
    UINT8       DeviceIndex;
    pm_on_off_type enable;
}EFI_PM_TEST_COINCELL_ENABLE_PARAM;

typedef struct _EFI_PM_TEST_COINCELL_GET_STATUS_PARAM
{
    UINT8       DeviceIndex;
    BOOLEAN     *status;

}EFI_PM_TEST_COINCELL_GET_STATUS_PARAM;

typedef struct _EFI_PM_TEST_COINCELL_SET_CHARGE_CURRENT_LIMIT_RESISTOR_PARAM
{
    UINT8       DeviceIndex;
    UINT32     COIN_CHG_RSET_VALUE;

}EFI_PM_TEST_COINCELL_SET_CHARGE_CURRENT_LIMIT_RESISTOR_PARAM;

typedef struct _EFI_PM_TEST_COINCELL_SET_CHARGING_VOLTAGE_PARAM
{
    UINT8       DeviceIndex;
    UINT32     COIN_CHG_VSET_VALUE;

}EFI_PM_TEST_COINCELL_SET_CHARGING_VOLTAGE_PARAM;

typedef union _EFI_PM_TEST_COINCELL_FUNCTION_TYPE
{
    EFI_PM_TEST_COINCELL_ENABLE_PARAM                	                CoincellEnable;
    EFI_PM_TEST_COINCELL_GET_STATUS_PARAM                               CoincellStatus; 
    EFI_PM_TEST_COINCELL_SET_CHARGE_CURRENT_LIMIT_RESISTOR_PARAM        CoincellRsetValue;
    EFI_PM_TEST_COINCELL_SET_CHARGING_VOLTAGE_PARAM                     CoincellVsetValue;

}EFI_PM_TEST_COINCELL_FUNCTION_TYPE;

#endif