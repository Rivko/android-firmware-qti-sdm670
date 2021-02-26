/** 
  @file  EFIPmicTest.h
  @brief PMIC Test EFI Protocol for UEFI.
*/
/*=============================================================================
  Copyright (c) 2015-2017 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 06/05/17   pb      Added BOB and removed MPP
 06/02/15   rm      created

=============================================================================*/

#ifndef __EFIPMICTEST_H__
#define __EFIPMICTEST_H__


/*===========================================================================
  INCLUDE FILES
===========================================================================*/

#include <Include/api/pmic/pm/pm_gpio.h>
#include <Protocol/EFIPmicGpio.h>
#include <Drivers/PmicDxe/Test/PmicTestGpio.h>
#include <Drivers/PmicDxe/Test/PmicTestBSI.h>
#include <Drivers/PmicDxe/Test/PmicTestLPG.h>
#include <Drivers/PmicDxe/Test/PmicTestAC.h>
#include <Drivers/PmicDxe/Test/PmicTestHaptics.h>
#include <Drivers/PmicDxe/Test/PmicTestWLED.h>
#include <Drivers/PmicDxe/Test/PmicTestPON.h>
#include <Drivers/PmicDxe/Test/PmicTestCoincell.h>
#include <Drivers/PmicDxe/Test/PmicTestBOB.h>




/*===========================================================================
  MACRO DECLARATIONS
===========================================================================*/

/**
  Protocol version.
*/
#define PMIC_TEST_PROTOCOL_REVISION 0x0000000000000001

/*  Protocol GUID definition */
/** @ingroup efi_pmicGpio_protocol */
#define EFI_PMIC_DRIVER_TEST_PROTOCOL_GUID \
    { 0xec3aad30, 0x1036, 0x4767, { 0xac, 0x14, 0xe6, 0xf7, 0xa6, 0x9b, 0xbe, 0x1e } }

/*===========================================================================
  EXTERNAL VARIABLES
===========================================================================*/

/**
  External reference to the PMIC GPIO Protocol GUID.
*/
extern EFI_GUID gQcomPmicDriverTestProtocolGuid;

/*===========================================================================
  TYPE DEFINITIONS
===========================================================================*/


/** 
  Driver types.
*/
typedef enum
{
    EFI_PM_GPIO,  /**< GPIO */ 
    EFI_PM_CLK_BUFF,
    EFI_PM_IBB,
    EFI_PM_LAB,
    EFI_PM_LPG,
    EFI_PM_MIPI_BIF,
    EFI_PM_NPA,
    EFI_PM_PWM,
    EFI_PM_PWRON,
    EFI_PM_RGB_LED,
    EFI_PM_RTC_CONVERSION,
    EFI_PM_RTC,
    EFI_PM_SMBB,
    EFI_PM_SMBCHG,
    EFI_PM_VERSION,
    EFI_PM_VIB,
    EFI_PM_VREG,
    EFI_PM_WLED,
    EFI_PM_FG,
    EFI_PM_FG_BATT_PARAMETER,
    EFI_PM_FG_BATT_PROFILE,
    EFI_PM_FG_SRAM,
    EFI_PM_BSI,
    EFI_PM_AC,
    EFI_PM_HAPTICS,
    EFI_PM_PON,
    EFI_PM_COINCELL,
    EFI_PM_BOB
}EFI_PM_TEST_DRIVER_TYPE;

/**
    Driver Type Function
*/
typedef union _EFI_PM_TEST_DRIVER_TYPE_FUNCTION
{
    EFI_PM_TEST_DRIVER_GPIO_TYPE_FUNCTION GpioTypeFunction;
    EFI_PM_TEST_DRIVER_BSI_TYPE_FUNCTION  BsiTypeFunction;
    EFI_PM_TEST_DRIVER_LPG_TYPE_FUNCTION  LPGTypeFunction;
    EFI_PM_TEST_DRIVER_AC_TYPE_FUNCTION  ACTypeFunction;
    EFI_PM_TEST_DRIVER_HAPTICS_TYPE_FUNCTION  HapticsTypeFunction;
    EFI_PM_TEST_DRIVER_WLED_TYPE_FUNCTION  WLEDTypeFunction;
    EFI_PM_TEST_DRIVER_PON_TYPE_FUNCTION  PONTypeFunction;
    EFI_PM_TEST_DRIVER_COINCELL_TYPE_FUNCTION  CoincellTypeFunction;
    EFI_PM_TEST_DRIVER_BOB_TYPE_FUNCTION  BOBTypeFunction;
}EFI_PM_TEST_DRIVER_TYPE_FUNCTION;


/**
  Driver Params.
*/
typedef struct _EFI_PM_TEST_DRIVER_PARAM
{
    union
    {
        EFI_PM_TEST_GPIO_FUNCTION_TYPE       GpioFuntionType;
        EFI_PM_TEST_BSI_FUNCTION_TYPE        BSIFuntionType; 
        EFI_PM_TEST_LPG_FUNCTION_TYPE        LPGFunctionType;
        EFI_PM_TEST_AC_FUNCTION_TYPE         ACFunctionType;                     
        EFI_PM_TEST_HAPTICS_FUNCTION_TYPE    HapticsFunctionType;
   	EFI_PM_TEST_WLED_FUNCTION_TYPE       WLEDFunctionType;
	EFI_PM_TEST_PON_FUNCTION_TYPE        PONFunctionType;
	EFI_PM_TEST_COINCELL_FUNCTION_TYPE    CoincellFunctionType;
      EFI_PM_TEST_BOB_FUNCTION_TYPE        BOBFunctionType;
    } u;    
}EFI_PM_TEST_DRIVER_PARAM;

typedef struct 
{
    boolean Status;
    UINT8 ErrorType;
}EFI_PM_TEST_DRIVER_OUT;

typedef enum
{
    EFI_PM_TEST_DRIVER_SUCCESS,
    EFI_PM_TEST_DRIVER_FAILURE
}EFI_PM_TEST_DRIVER_STATUS;
/**
  Protocol declaration.
*/
typedef struct _EFI_QCOM_PMIC_TEST_PROTOCOL   EFI_QCOM_PMIC_TEST_PROTOCOL;

/** @endcond */

/*===========================================================================
  FUNCTION DEFINITIONS
===========================================================================*/
/* EFI_PM_TEST_PMIC_DRIVER */ 

typedef
EFI_STATUS (EFIAPI *EFI_PM_TEST_PMIC_DRIVER)(
  IN EFI_PM_TEST_DRIVER_TYPE                    DriverType,
  IN EFI_PM_TEST_DRIVER_TYPE_FUNCTION           DriverTypeFunction,
  IN EFI_PM_TEST_DRIVER_PARAM                   *DriverParam,
  OUT EFI_PM_TEST_DRIVER_OUT                    *DriverOut
);

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/

struct _EFI_QCOM_PMIC_TEST_PROTOCOL {
    UINT64                                Revision;
    EFI_PM_TEST_PMIC_DRIVER               TestPmicDriver;
};

#endif  /* __EFIPMICTEST_H__ */
