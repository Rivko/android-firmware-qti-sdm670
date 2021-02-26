/** 
 @file PcieGpioConfig.c

  Implements GPIO init routines

  Copyright (c) 2016, Qualcomm Technologies Inc. All rights reserved.

**/


/*=============================================================================
                              EDIT HISTORY


 when       who      what, where, why
 --------   ----    ----------------------------------------------------------
 07/15/16   ts      Formatted strings to %a in debug prints
 06/28/16   ts      First checkin for 8998, spunoff from PciLink.c
=============================================================================*/


#include <Library/PcieConfigLib.h>
#include <Protocol/EFIPmicGpio.h>
#include <Protocol/EFITlmm.h>
#include "PcieLink.h"
#include "PcieBsp.h"

/***************************************************************************//**
 * @fn PcieConfigLibConfigureTLMMGPIO
 * @brief Configure the TLMM GPIOs
 *
 * Configure the TLMM GPIOs based on the input
 *
 * @param[in] Pointer to gpioPinConfig_t type
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
STATIC EFI_STATUS PcieConfigLibConfigureTLMMGPIO(
    CONST gpioPinConfig_t * pTLMMConfigData)
{
  EFI_TLMM_PROTOCOL       *TLMMProtocol     = NULL;
  EFI_STATUS              Status;

  Status = gBS->LocateProtocol(&gEfiTLMMProtocolGuid,
                               NULL, (VOID **)&TLMMProtocol);
  if EFI_ERROR (Status)
  {
    DEBUG(( EFI_D_WARN, "%a: TLMM Locate Protocol failed\r\n", __func__));
    return Status;
  }

  Status = TLMMProtocol->ConfigGpio(
                EFI_GPIO_CFG(
                  pTLMMConfigData->gpio_cfg_param.TlmmGpioParam.GpioNum,
                  pTLMMConfigData->gpio_cfg_param.TlmmGpioParam.Func,
                  pTLMMConfigData->gpio_cfg_param.TlmmGpioParam.Direction,
                  pTLMMConfigData->gpio_cfg_param.TlmmGpioParam.Pull,
                  pTLMMConfigData->gpio_cfg_param.TlmmGpioParam.DriveStrength),
                TLMM_GPIO_ENABLE);
  if EFI_ERROR (Status)
  {
    DEBUG(( EFI_D_WARN, "%a: ConfigGpio failed\r\n", __func__));
    return Status;
  }

  if (pTLMMConfigData->gpio_cfg_param.TlmmGpioParam.Direction == GPIO_OUTPUT)
  {
    Status = TLMMProtocol->GpioOut(
                  EFI_GPIO_CFG(
                    pTLMMConfigData->gpio_cfg_param.TlmmGpioParam.GpioNum,
                    pTLMMConfigData->gpio_cfg_param.TlmmGpioParam.Func,
                    pTLMMConfigData->gpio_cfg_param.TlmmGpioParam.Direction,
                    pTLMMConfigData->gpio_cfg_param.TlmmGpioParam.Pull,
                    pTLMMConfigData->gpio_cfg_param.TlmmGpioParam.DriveStrength),
                  pTLMMConfigData->gpio_cfg_param.TlmmGpioParam.value);
    if EFI_ERROR (Status)
    {
      DEBUG(( EFI_D_WARN, "%a: GpioOut failed\r\n"));
      return Status;
    }
  }

  return Status;
}

/***************************************************************************//**
 * @fn PcieConfigLibConfigurePMICGPIO
 * @brief Configure the PMIC GPIOs
 *
 * Configure the PMIC GPIOs based on the input
 *
 * @param[in] Pointer to gpioPinConfig_t type
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
STATIC EFI_STATUS PcieConfigLibConfigurePMICGPIO(
    CONST gpioPinConfig_t * pPMICConfigData)
{
  EFI_QCOM_PMIC_GPIO_PROTOCOL   *PmicGpioProtocol = NULL;
  EFI_STATUS                    Status;

  Status = gBS->LocateProtocol(&gQcomPmicGpioProtocolGuid, NULL, (VOID **)&PmicGpioProtocol);
  if EFI_ERROR (Status)
  {
    DEBUG(( EFI_D_WARN, "%a: PMIC Locate Protocol failed\r\n", __func__));
    return Status;
  }
  Status = PmicGpioProtocol->ConfigDigitalOutput(
                pPMICConfigData->gpio_cfg_param.PmicGpioParam.PmicIndex,
                pPMICConfigData->gpio_cfg_param.PmicGpioParam.GpioNum,
                pPMICConfigData->gpio_cfg_param.PmicGpioParam.OutBuffConfig ,
                pPMICConfigData->gpio_cfg_param.PmicGpioParam.VSrc,
                pPMICConfigData->gpio_cfg_param.PmicGpioParam.Source,
                pPMICConfigData->gpio_cfg_param.PmicGpioParam.BufferStrength,
                pPMICConfigData->gpio_cfg_param.PmicGpioParam.inversion);
  if EFI_ERROR (Status)
  {
    DEBUG(( EFI_D_WARN, "%a: ConfigDigitalOutput failed\r\n", __func__));
    return Status;
  }

  return Status;
}

/***************************************************************************//**
 * @fn PcieConfigLibConfigureGPIOs
 * @brief Configure the GPIOs
 *
 * Configure the GPIOs based on the input
 *
 * @param[in] Pointer to gpioPinConfig_t type
 *
 * @return EFI_SUCCESS or appropriate error value
 ******************************************************************************/
EFI_STATUS PcieConfigLibConfigureGPIOs(
    CONST gpioPinConfig_t * pGPIOConfigData)
{
  EFI_STATUS  Status = EFI_SUCCESS;

  switch (pGPIOConfigData->type)
  {
  case MSM_GPIO:
    Status = PcieConfigLibConfigureTLMMGPIO(pGPIOConfigData);
    break;
  case PMIC_GPIO:
    Status = PcieConfigLibConfigurePMICGPIO(pGPIOConfigData);
    break;
  case DELAY:
    gBS->Stall(pGPIOConfigData->gpio_cfg_param.DelayValue);
    break;
  case NONE:
  case PMIC_MPP:
  default:
    break;
  }

  return Status;
}
