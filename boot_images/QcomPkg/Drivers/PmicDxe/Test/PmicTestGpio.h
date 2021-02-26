#ifndef __PMICTESTGPIO_H__
#define __PMICTESTGPIO_H__

/*! @file PmicTestGpio.h
 *
 *  PMIC Test Gpio
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
07/15/15   RM      Added Pmic Test Gpio Header file

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
    //EFI_PM_GPIO_STATUS_GET,  /**< GPIO status get*/ 
    EFI_PM_GPIO_CONFIG_BIAS_VOLTAGE, /**<GPIO Config Bias Voltage*/
    EFI_PM_GPIO_SET_VOLTAGE_SOURCE, /**<GPIO Set Voltage source */
    EFI_PM_GPIO_CONFIG_DIGITALINPUT, /**<GPIO Config Digital input */
    EFI_PM_GPIO_CONFIG_DIGITALOUTPUT, /**<GPIO Config Digital output */
    EFI_PM_GPIO_CONFIG_DIGITAL_INOUT, /**<GPIO Config Digital input/output */
    EFI_PM_GPIO_SET_INVERSION_CONFIG, /**<GPIO Config set inversion */
    EFI_PM_GPIO_SET_CURRENT_SOURCE_PULLS, /**<GPIO set current source*/
    EFI_PM_GPIO_SET_EXT_PIN_CONFIG, /**<GPIO set external pin config*/
    EFI_PM_GPIO_SET_OUT_BUFFER_DRIVE_STRENGTH, /**<GPIO set buffer drive strength*/
    EFI_PM_GPIO_SET_SOURCE_CONFIG, /**<GPIO set source config*/
    EFI_PM_GPIO_SET_OUTPUT_BUFFER_CONFIG, /**<GPIO set buffer config*/
    EFI_PM_GPIO_ENABLE,
}EFI_PM_TEST_DRIVER_GPIO_TYPE_FUNCTION;

typedef struct _EFI_PM_GPIO_TYPE_INDEX
{
    UINT8                   GPIOIndex;
    EFI_PM_GPIO_WHICH_TYPE  GPIOType;
} EFI_PM_TEST_GPIO_TYPE_INDEX;

typedef struct _EFI_PM_GPIO_VOLTAGE_SOURCE_PARAM
{
    EFI_PM_TEST_GPIO_TYPE_INDEX         GPIOTypeIndex;
    EFI_PM_GPIO_VOLTAGE_SOURCE_TYPE     VoltageSource;
} EFI_PM_TEST_GPIO_SET_VOLTAGE_SOURCE_PARAM, EFI_PM_TEST_GPIO_CONFIG_BIAS_VOLTAGE_PARAM;

typedef struct _EFI_PM_GPIO_CONFIG_DIGITAL_INOUT_PARAM
{
    EFI_PM_TEST_GPIO_TYPE_INDEX                 GPIOTypeIndex;
    EFI_PM_GPIO_SOURCE_CONFIG_TYPE              GPIOSrc;
    EFI_PM_GPIO_CURRENT_SOURCE_PULLS_TYPE       GPIOPulls;
    EFI_PM_GPIO_VOLTAGE_SOURCE_TYPE             VoltageSource;
    EFI_PM_GPIO_OUT_BUFFER_CONFIG_TYPE          BufferConfig;
    EFI_PM_GPIO_OUT_BUFFER_DRIVE_STRENGTH_TYPE  BufferStrength;
} EFI_PM_TEST_GPIO_CONFIG_DIGITAL_INOUT_PARAM;

typedef struct _EFI_PM_GPIO_SET_EXT_PIN_CONFIG_PARAM
{
    EFI_PM_TEST_GPIO_TYPE_INDEX         GPIOTypeIndex;
    EFI_PM_GPIO_EXT_PIN_CONFIG_TYPE     ConfigType;
} EFI_PM_TEST_GPIO_SET_EXT_PIN_CONFIG_PARAM;

typedef struct _EFI_PM_GPIO_SET_INVERSION_CONFIG_PARAM
{
    EFI_PM_TEST_GPIO_TYPE_INDEX  GPIOTypeIndex;
    boolean                 Inversion;
} EFI_PM_TEST_GPIO_SET_INVERSION_CONFIG_PARAM;

typedef struct _EFI_PM_GPIO_CURRENT_SOURCE_PULLS_PARAM
{
    EFI_PM_TEST_GPIO_TYPE_INDEX             GPIOTypeIndex;
    EFI_PM_GPIO_CURRENT_SOURCE_PULLS_TYPE   CurrentSrcPullType;
} EFI_PM_TEST_GPIO_SET_CURRENT_SOURCE_PULLS_PARAM, EFI_PM_TEST_GPIO_SET_OUT_BUFFER_DRIVE_STRENGTH_PARAM;

typedef struct _EFI_PM_GPIO_SET_SOURCE_CONFIG_TYPE_PARAM
{
    EFI_PM_TEST_GPIO_TYPE_INDEX           GPIOTypeIndex;
    EFI_PM_GPIO_SOURCE_CONFIG_TYPE        SourceConfigType;
} EFI_PM_TEST_GPIO_SET_SOURCE_CONFIG_TYPE_PARAM;

typedef struct _EFI_PM_GPIO_DRIVE_STRENGTH_TYPE_PARAM
{
    EFI_PM_TEST_GPIO_TYPE_INDEX                   GPIOTypeIndex;
    EFI_PM_GPIO_OUT_BUFFER_DRIVE_STRENGTH_TYPE    DriveStrengthType;
} EFI_PM_TEST_GPIO_DRIVE_STRENGTH_TYPE_PARAM;

typedef struct _EFI_PM_GPIO_OUTPUT_BUFFER_CONFIGURATION_PARAM
{
    EFI_PM_TEST_GPIO_TYPE_INDEX                  GPIOTypeIndex;
    EFI_PM_GPIO_OUT_BUFFER_CONFIG_TYPE          BufferConfigType;
} EFI_PM_TEST_GPIO_OUTPUT_BUFFER_CONFIGURATION_PARAM;

typedef struct _EFI_PM_GPIO_ENABLE_PARAM
{
    EFI_PM_TEST_GPIO_TYPE_INDEX                  GPIOTypeIndex;
    BOOLEAN						                 enable;
} EFI_PM_TEST_GPIO_ENABLE_PARAM;

typedef union _EFI_PM_TEST_GPIO_FUNCTION_TYPE
{
    EFI_PM_TEST_GPIO_CONFIG_BIAS_VOLTAGE_PARAM       GpioConfigBiasVoltage;
    EFI_PM_TEST_GPIO_CONFIG_DIGITAL_INOUT_PARAM      GpioConfigDigitalInOut; 
    EFI_PM_TEST_GPIO_SET_EXT_PIN_CONFIG_PARAM        GpioSetExtPinConfig;
    EFI_PM_TEST_GPIO_SET_VOLTAGE_SOURCE_PARAM        GpioSetVoltageSource;
    EFI_PM_TEST_GPIO_SET_INVERSION_CONFIG_PARAM      GpioSetInversionConfig;
    EFI_PM_TEST_GPIO_SET_CURRENT_SOURCE_PULLS_PARAM  GpioSetCurrentSourcePull;
    EFI_PM_TEST_GPIO_SET_SOURCE_CONFIG_TYPE_PARAM    GpioSetSourceConfigType;
    EFI_PM_TEST_GPIO_DRIVE_STRENGTH_TYPE_PARAM       GpioDriveStrengthType;
    EFI_PM_TEST_GPIO_OUTPUT_BUFFER_CONFIGURATION_PARAM GpioOutputBufferConfigType;
    EFI_PM_TEST_GPIO_ENABLE_PARAM					 GpioEnable;
}EFI_PM_TEST_GPIO_FUNCTION_TYPE;

/*===========================================================================
                       FUNCTION PROTOTYPES
===========================================================================*/



#endif // __PMICTESTGPIO_H__


