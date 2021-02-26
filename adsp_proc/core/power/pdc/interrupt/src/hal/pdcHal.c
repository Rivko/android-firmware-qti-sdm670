/*============================================================================
  
  FILE:         pdcHal.c
  
  DESCRIPTION:  This is the generic hardware abstraction layer implementation 
                for the PDC interrupt controller.
  
  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
===============================================================================
$Header: //components/rel/core.qdsp6/2.1/power/pdc/interrupt/src/hal/pdcHal.c#2 $
$DateTime: 2017/09/13 08:52:52 $
=============================================================================*/ 
#include "comdef.h"
#include "pdc_hwio.h"
#include "pdc_types.h"
#include "pdc_internal.h"
#include "pdcHal.h"

/*===========================================================================
 *                          EXTERNAL GLOBAL VARIABLES
 *==========================================================================*/
extern pdc_interrupts  g_pdcInterrupts;
extern pdc_gpios       g_pdcGpios;

/*============================================================================
 *                      INTERNAL FUNCTION DECLARATIONS
 *===========================================================================*/
/**
 *  HALInternal_setInterruptState
 * 
 *  @brief enables or disables the desired wakeup interrupt
 * 
 *  @param pdcBitNum: Interrupt or GPIO bit number.
 *                    This value is the absolute number. i.e. if "0" it is the first
 *                    interrupt, or first GPIO
 *  @param pinType:   Interrupt or gpio type
 *  @param enable:    enable or disable value
 * 
 *  @return standard PDC error code
 */
static pdc_status_type HALInternal_setInterruptState(uint32       pdcBitNum,
                                                     pdc_pin_type pinType,
                                                     boolean      enable)
{
  pdc_trigger_config  *localConfig;
  uint32              bank;
  uint32              mask;
  pdc_status_type     status  = PDC_INVALID_CONFIG;
  uint32              state   = (TRUE == enable) ? (uint32)-1 : 0x00;
  
  /* Get the trigger configuraiton */
  localConfig = pdc_getTriggerConfig(pinType, pdcBitNum);

  if(PDC_IRQ != pinType)
  {
    /* Need to offset the GPIO bit by the number of interrupts which are first bits in the
     * registers */
    pdcBitNum += g_pdcInterrupts.numInt;
  }

  if(PDC_DRV_INVALID != localConfig->drv_num)
  {
    bank  = (pdcBitNum / 32);
    mask  = 0x01 << ((pdcBitNum % 32));

    /* Enable the interrupt in the PDC register */
    PDC_HWIO_OUTMI2(IRQ_ENABLE_BANKb_DRVd, localConfig->drv_num, bank, mask, (uint32)(mask & state));
    status = PDC_SUCCESS;
  }

  return status;
}

/*============================================================================
 *                      EXTERNAL FUNCTION DECLARATIONS
 *===========================================================================*/
/*
 *  pdcHAL_enable
 */
pdc_status_type pdcHAL_enable(uint32         pdcBitNum,
                              pdc_pin_type   pinType)
{
  return HALInternal_setInterruptState(pdcBitNum, pinType, TRUE);
}

/* 
 *  pdcHAL_disable
 */
pdc_status_type pdcHAL_disable(uint32         pdcBitNum,
                               pdc_pin_type   pinType)
{
  return HALInternal_setInterruptState(pdcBitNum, pinType, FALSE);
}


/*
 * pdcHAL_setTriggerConfig
 */ 
pdc_status_type pdcHAL_setTriggerConfig(uint32              pdcBitNum,
                                        pdc_pin_type        pinType,
                                        pdc_trigger_config  *requestedConfig)
{
  uint32 bitNum;

  if(PDC_IRQ == pinType)
  {
    bitNum  = pdcBitNum;  /* Static interrupts are lower bit registers */
  }
  else
  {
    bitNum  = pdcBitNum + g_pdcInterrupts.numInt;  /* GPIO's are after static ints */
  }

  /* Set the config in hardware */
  PDC_HWIO_OUTFI2(IRQ_i_CFG_DRVd, requestedConfig->drv_num, bitNum,
                 IRQ_CFG, requestedConfig->trig_type);
                 
  /* Not all subsystems have a DRV owner */                 
#if (0 != PDC_HWIO_RMSK(IRQ_i_OWNER))
  PDC_HWIO_OUTFI(IRQ_i_OWNER, bitNum, IRQ_OWNER, requestedConfig->drv_num);
#endif

  return PDC_SUCCESS;
}

/*
 * pdcHAL_setGPIOConfig
 */ 
pdc_status_type pdcHAL_setGPIOConfig(uint32             pdcBitNum,
                                     uint32             gpioNumber,
                                     pdc_trigger_config *requestedConfig)
{
  /* Set the MUX GPIO input to use */
  PDC_HWIO_OUTFI(GP_IRQ_SELECTgp, pdcBitNum, IRQ_SELECT, gpioNumber);

  /* Setup the trigger type */
  return(pdcHAL_setTriggerConfig(pdcBitNum, PDC_GPIO, requestedConfig));
}

