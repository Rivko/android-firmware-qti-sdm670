/*! @file PmicGpioProtocol.c 

 *  PMIC-GPIO MODULE RELATED DECLARATION
 *  This file contains functions and variable declarations to support 
 *  the PMIC GPIO module.
 *
 * Copyright (c) 2011-2012 Qualcomm Technologies, Inc.  All Rights Reserved. 
 * Qualcomm Technologies Proprietary and Confidential.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
13/12/13   aa      PmicLib Dec Addition
06/18/13   al      Adding API for enabling/disabling dtest line for dig in
01/22/13   al      Added real time interrupt status
10/25/12   al      File renamed 
07/27/12   al	   Commented out not regquired for 8974 and compiling for 8960
11/29/11   sm      Added GpioGetStatus API
05/11/11   dy      New file.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include "pm_gpio.h"
#include "pm_uefi.h"

#include <Protocol/EFIPmicGpio.h>


/*===========================================================================
                  EXTERNAL FUNCTION DECLARATIONS
===========================================================================*/

/**
  EFI_PmGpioConfigDigitalInput()

  @brief
  ConfigDigitalInput implementation of EFI_QCOM_PMIC_GPIO_PROTOCOL
 */
EFI_STATUS
EFIAPI
EFI_PmGpioConfigDigitalInput
(
  IN UINT32                                   PmicDeviceIndex,
  EFI_PM_GPIO_WHICH_TYPE                      Gpio,
  EFI_PM_GPIO_CURRENT_SOURCE_PULLS_TYPE       ISourcePulls,
  EFI_PM_GPIO_VOLTAGE_SOURCE_TYPE             VoltageSource,
  EFI_PM_GPIO_OUT_BUFFER_DRIVE_STRENGTH_TYPE  OutBufferStrength,
  EFI_PM_GPIO_SOURCE_CONFIG_TYPE              Source
)
{
  pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;

  errFlag = pm_gpio_config_digital_input( PmicDeviceIndex, 
                                          (pm_gpio_perph_index) Gpio,
                                          (pm_gpio_current_src_pulls_type) ISourcePulls,
                                          (pm_gpio_volt_src_type) VoltageSource,
                                          (pm_gpio_out_buffer_drv_strength_type) OutBufferStrength,
                                          (pm_gpio_src_config_type) Source );
  
  if (errFlag != PM_ERR_FLAG__SUCCESS)
  {
      return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}


/**
  EFI_PmGpioConfigDigitalOutput()

  @brief
  ConfigDigitalOutput implementation of EFI_QCOM_PMIC_GPIO_PROTOCOL
 */
EFI_STATUS
EFIAPI
EFI_PmGpioConfigDigitalOutput
(
  IN UINT32                                   PmicDeviceIndex, 
  EFI_PM_GPIO_WHICH_TYPE                      Gpio,
  EFI_PM_GPIO_OUT_BUFFER_CONFIG_TYPE          OutBufferConfig,
  EFI_PM_GPIO_VOLTAGE_SOURCE_TYPE             VoltageSource,
  EFI_PM_GPIO_SOURCE_CONFIG_TYPE              Source,
  EFI_PM_GPIO_OUT_BUFFER_DRIVE_STRENGTH_TYPE  OutBufferStrength,
  BOOLEAN                                     OutInversion
)
{
  pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;
 
  errFlag = pm_gpio_config_digital_output(   PmicDeviceIndex, 
                                             (pm_gpio_perph_index) Gpio,
                                             (pm_gpio_out_buffer_config_type) OutBufferConfig,
                                             (pm_gpio_volt_src_type) VoltageSource,
                                             (pm_gpio_src_config_type) Source,
                                             (pm_gpio_out_buffer_drv_strength_type) OutBufferStrength,
                                             (boolean) OutInversion );
    
  if (errFlag != PM_ERR_FLAG__SUCCESS)
  {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

/**
  EFI_PmGpioGetStatus()

  @brief
  GetStatus implementation of EFI_QCOM_PMIC_GPIO_PROTOCOL
 */
EFI_STATUS
EFIAPI
EFI_PmGpioGetStatus
(
  IN UINT32                     PmicDeviceIndex,
  EFI_PM_GPIO_WHICH_TYPE        Gpio,
  EFI_PM_GPIO_STATUS_TYPE       *GpioStatus
)
{
  pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;

  errFlag = pm_gpio_status_get( PmicDeviceIndex, 
                                (pm_gpio_perph_index)  Gpio,
                                (pm_gpio_status_type *) GpioStatus);
    
  if (errFlag != PM_ERR_FLAG__SUCCESS)
  {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}


/**
  EFI_PmGpioIrqEnable()

  @brief
  GpioIrqEnable implementation of EFI_QCOM_PMIC_GPIO_PROTOCOL
 */
EFI_STATUS
EFIAPI
EFI_PmGpioIrqEnable
(
 IN  UINT32                     PmicDeviceIndex,
 IN  EFI_PM_GPIO_WHICH_TYPE     Gpio,
 IN  BOOLEAN                    Enable
)
{
  pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;

  errFlag = pm_gpio_irq_enable( PmicDeviceIndex,(pm_gpio_perph_index)Gpio, (boolean)Enable);
  if (errFlag != PM_ERR_FLAG__SUCCESS)
  {
    return EFI_DEVICE_ERROR;
  }
  return EFI_SUCCESS;
}



/**
  EFI_PmGpioIrqClear()

  @brief
  GpioIrqClear implementation of EFI_QCOM_PMIC_GPIO_PROTOCOL
 */
EFI_STATUS
EFIAPI
EFI_PmGpioIrqClear
(
 IN  UINT32                    PmicDeviceIndex,
 IN  EFI_PM_GPIO_WHICH_TYPE    Gpio
)
{
  pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;

  errFlag = pm_gpio_irq_clear( PmicDeviceIndex,(pm_gpio_perph_index)Gpio);
  if (errFlag != PM_ERR_FLAG__SUCCESS)
  {
    return EFI_DEVICE_ERROR;
  }
  return EFI_SUCCESS;
}



/**
  EFI_PmGpioIrqSetTrigger()

  @brief
  GpioIrqSetTrigger implementation of EFI_QCOM_PMIC_GPIO_PROTOCOL
 */
EFI_STATUS
EFIAPI
EFI_PmGpioIrqSetTrigger
(
 IN  UINT32                     PmicDeviceIndex,
 IN  EFI_PM_GPIO_WHICH_TYPE     Gpio,
 IN  EFI_PM_IRQ_TRIGGER_TYPE    Trigger
)
{
  pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;

  errFlag = pm_gpio_irq_set_trigger( PmicDeviceIndex,(pm_gpio_perph_index)Gpio,(pm_irq_trigger_type)Trigger);
  if (errFlag != PM_ERR_FLAG__SUCCESS)
  {
    return EFI_DEVICE_ERROR;
  }
  return EFI_SUCCESS;
}


/**
  EFI_PmGpioIrqStatus()

  @brief
  PmGpioIrqStatus implementation of EFI_QCOM_PMIC_GPIO_PROTOCOL
 */
EFI_STATUS
EFIAPI
EFI_PmGpioIrqStatus
(
 IN  UINT32                     PmicDeviceIndex,
 IN  EFI_PM_GPIO_WHICH_TYPE     Gpio,
 IN  EFI_PM_IRQ_STATUS_TYPE     Type,
 OUT BOOLEAN                    *Status
)
{
  pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;

  errFlag = pm_gpio_irq_status( PmicDeviceIndex,(pm_gpio_perph_index)Gpio, (pm_irq_status_type) Type,(boolean *)Status);
  if (errFlag != PM_ERR_FLAG__SUCCESS)
  {
    return EFI_DEVICE_ERROR;
  }
  return EFI_SUCCESS;
}

/**
  EFI_PmGpioSetDigInCtl()

  @brief
  PmGpioSetDigInCtl implementation of   EFI_QCOM_PMIC_GPIO_PROTOCOL 
  */
EFI_STATUS
EFIAPI
EFI_PmGpioSetDigInCtl
(
 IN  UINT32                           PmicDeviceIndex,
 IN  EFI_PM_GPIO_WHICH_TYPE           Gpio,
 IN  EFI_PM_GPIO_DIG_IN_TYPE          DigIn,
 IN  BOOLEAN                          Enable
)
{
  pm_err_flag_type errFlag = PM_ERR_FLAG__SUCCESS;

  errFlag = pm_gpio_set_dig_in_ctl( PmicDeviceIndex,(pm_gpio_perph_index)Gpio, (pm_gpio_dig_in_type)DigIn,(boolean)Enable);
  if (errFlag != PM_ERR_FLAG__SUCCESS)
  {
    return EFI_DEVICE_ERROR;
  }
  return EFI_SUCCESS;
}

/**
  EFI_PmGpioExtPinConfig()

  @brief
  GpioExtPinConfig implementation of EFI_QCOM_PMIC_GPIO_PROTOCOL
 */
EFI_STATUS
EFIAPI
EFI_PmGpioExtPinConfig
(
 IN  UINT32                           PmicDeviceIndex,
 IN  EFI_PM_GPIO_WHICH_TYPE           Gpio,
 IN  EFI_PM_GPIO_EXT_PIN_CONFIG_TYPE  ExtPinConfig
)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

  err_flag = pm_gpio_set_ext_pin_config( PmicDeviceIndex,(pm_gpio_perph_index)Gpio, (pm_gpio_ext_pin_config_type)ExtPinConfig);

  if (err_flag != PM_ERR_FLAG__SUCCESS)
  {
    return EFI_DEVICE_ERROR;
  }
  return EFI_SUCCESS;
}

/**
  PMIC GPIO UEFI Protocol implementation
 */
EFI_QCOM_PMIC_GPIO_PROTOCOL PmicGpioProtocolImplementation = 
{
  PMIC_GPIO_REVISION,
  EFI_PmGpioConfigDigitalInput,
  EFI_PmGpioConfigDigitalOutput,
  EFI_PmGpioGetStatus,
  EFI_PmGpioIrqEnable,
  EFI_PmGpioIrqClear,
  EFI_PmGpioIrqSetTrigger,
  EFI_PmGpioIrqStatus,
  EFI_PmGpioSetDigInCtl,
  EFI_PmGpioExtPinConfig,
};
