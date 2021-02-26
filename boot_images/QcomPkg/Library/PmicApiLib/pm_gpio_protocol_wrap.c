/*! \file
*  
*  \brief  pm_gpio_protocol_wrap.c ----File contains the implementation of the public APIs for GPIO resource type.
*  \details Implementation file for GPIO resourece type.
*  Each of the APIs checks for access and then if necessary directs
*  the call to Driver implementation or to RPC function for the master processor.

*    This file contains code for Target specific settings and modes.
*  
*  &copy; Copyright 2016 QUALCOMM Technologies, Inc, All Rights Reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/08/16   al      New file 
===========================================================================*/

/*===========================================================================

INCLUDE FILES 

===========================================================================*/
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>


#include <api/pmic/pm/pm_gpio.h>
#include <Protocol/EFIPmicGpio.h>
/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/

STATIC EFI_QCOM_PMIC_GPIO_PROTOCOL            *PmicGpioProtocol  = NULL;

pm_err_flag_type pm_gpio_protocol_init(void)
{
  EFI_STATUS  Status = EFI_SUCCESS;
  pm_err_flag_type         err_flag = PM_ERR_FLAG_SUCCESS;

  if (NULL == PmicGpioProtocol)
  {
    Status = gBS->LocateProtocol(&gQcomPmicGpioProtocolGuid, NULL, (VOID **)&PmicGpioProtocol);
    err_flag = (Status == EFI_SUCCESS) ? PM_ERR_FLAG_SUCCESS : PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }

  return err_flag;
}

pm_err_flag_type pm_gpio_set_ext_pin_config(uint8 pmic_chip, pm_gpio_perph_index gpio, pm_gpio_ext_pin_config_type  ext_pin_config)
{
  pm_err_flag_type       err_flag = PM_ERR_FLAG_SUCCESS;
  EFI_STATUS  Status = EFI_SUCCESS;

  if (PM_ERR_FLAG_SUCCESS == pm_gpio_protocol_init())
  {
    Status = PmicGpioProtocol->ExtPinConfig(pmic_chip, (EFI_PM_GPIO_WHICH_TYPE)gpio, (EFI_PM_GPIO_EXT_PIN_CONFIG_TYPE)ext_pin_config);
    err_flag = (Status == EFI_SUCCESS) ? PM_ERR_FLAG_SUCCESS : PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }
  else
  {
    err_flag = PM_ERR_FLAG_FEATURE_NOT_SUPPORTED;
  }

  return err_flag;
}



