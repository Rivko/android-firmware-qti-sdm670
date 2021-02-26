/*===========================================================================
  FILE:         pdc_driver.c
  
  OVERVIEW:     This file contains the necessary PDC API functions to interface
                with the PDC interrupt controller

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
===============================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/pdc/interrupt/src/client_root/pdc_driver.c#1 $
$DateTime: 2018/07/30 01:21:36 $
=============================================================================*/ 
#include "comdef.h"
#include "CoreVerify.h"
#include "pdcInt.h"
#include "pdcGpio.h"
#include "pdc_types.h"
#include "pdc_internal.h"
#include "pdcLog.h"
#include "pdc_target.h"
#include "pdcHal.h"
#include "pdcMap.h"

/*===========================================================================
 *                               GLOBAL VARIABLES
 *==========================================================================*/
pdc_interrupts  g_pdcInterrupts;
pdc_gpios       g_pdcGpios;
  
/*============================================================================
 *                           PRIVATE INTERRUPT FUNCTIONS
 *===========================================================================*/
/*
 * pdcInt_findEntry 
 */
uint32 pdcInt_findEntry(uint32 interrupt, uint32 *bitNum)
{
  pdc_status_type status = PDC_SUCCESS;

  *bitNum = 0;

  /* Determine the register bit to configure */
  while((*bitNum < g_pdcInterrupts.numInt) &&
        (interrupt != g_pdcInterrupts.map[*bitNum].subsystem_interrupt))
  {
    (*bitNum)++;
  }

  if(*bitNum >= g_pdcInterrupts.numInt)
  {
    status = PDC_INVALID_SS_INT;  
  }

  return status;
  
}

/*============================================================================
 *                           PUBLIC INTERRUPT FUNCTIONS
 *===========================================================================*/
/*
 * pdcInt_config
 */
pdc_status_type pdcInt_config(uint32              subsystemInterrupt,
                              pdc_trigger_config  *config)
{
  pdc_trigger_config  *localConfig;
  pdc_status_type     status;
  uint32              bitNum;
  
  if(NULL == config) 
  {
    return PDC_INVALID_CONFIG;
  }

  status = pdcInt_findEntry(subsystemInterrupt, &bitNum);

  if(PDC_SUCCESS != status)
  {
    return status;
  }
  else
  {
    pdcLog_printf(3, 
                  "Config subsystem int %u "
                  "(PDC bit: %u) (trigger: %d)",
                  subsystemInterrupt,
                  bitNum,
                  config->trig_type); 
  }

  localConfig = &g_pdcInterrupts.map[bitNum].trig_config;

  /* Update table data to current configuration */
  localConfig->trig_type  = config->trig_type;
  localConfig->drv_num    = config->drv_num;
  
  PDC_LOCK();  
  status = pdcHAL_setTriggerConfig(bitNum, PDC_IRQ, config);
  PDC_UNLOCK();

  return status;
}

/*
 *  pdcInt_enable
 */
pdc_status_type pdcInt_enable(uint32 subsystemInterrupt)
{
  pdc_status_type     status;
  uint32              bitNum;

  status = pdcInt_findEntry(subsystemInterrupt, &bitNum);

  if(PDC_SUCCESS != status)
  {
    return status;
  }

  pdcLog_printf(1, "Enable subsystem int %u", subsystemInterrupt);

  PDC_LOCK();
  status = pdcHAL_enable(bitNum, PDC_IRQ);
  PDC_UNLOCK();

  return status;
}

/*
 * pdcInt_disable
 */
pdc_status_type pdcInt_disable(uint32 subsystemInterrupt)
{
  pdc_status_type status;
  uint32          bitNum;

  status = pdcInt_findEntry(subsystemInterrupt, &bitNum);

  if(PDC_SUCCESS != status)
  {
    return status;
  }
 
  pdcLog_printf(1, "Disable subsystem int %u", subsystemInterrupt);

  PDC_LOCK();
  status = pdcHAL_disable(bitNum, PDC_IRQ);
  PDC_UNLOCK();

  return status;
}

/*============================================================================
 *                            PRIVATE GPIO FUNCTIONS
 *===========================================================================*/
/**
 * gpio_getFreeMux 
 *  
 * @brief Finds and returns the index of a free MUX
 *  
 * @param muxNum: Index of first free mux
 *  
 * @return status
 */
static pdc_status_type gpio_getFreeMux(uint32 *muxNum)
{
  pdc_gpio_mapping *map;

  for(*muxNum = 0; *muxNum < g_pdcGpios.numMux; (*muxNum)++)
  {
    map = &g_pdcGpios.map.mux_map[*muxNum];

    /* Find the first free MUX and return it */
    if(NULL == map->gpio_tbl_ptr)
    {
      return PDC_SUCCESS;
    }
  }

  return PDC_NO_FREE_GPIO_MUX;
}

/**
 * gpio_findEntry 
 *  
 * @brief Verifies the given GPIO number is valid for the subsystem and returns
 *        a pointer to the table entry for that GPIO.
 *  
 * @param gpio:             Desired subsystem GPIO
 * @param muxInputNumber:   Returned mux input bit number
 *                          Parameter can be NULL if caller doesn't need this and is
 *                          only populated if a valid table entry was found
 *  
 * @return Table entry pointer or NULL if invalid GPIO
 */
static pdc_gpio_inputs* gpio_findEntry(uint32 gpio, uint32 *muxInputNumber)
{
  uint32 nGPIO;

  /* Determine if the GPIO is already configured */
  for(nGPIO=0; nGPIO < g_pdcGpios.numGpio; nGPIO++)
  {
    /* Search the valid GPIO's for the requested one */
    if(gpio == g_pdcGpios.map.gpio_inputs[nGPIO].gpio_num)
    {
      if(NULL != muxInputNumber)
      {
        /* Table corresponds to ordered MUX input bit, fill in bit number of requested GPIO */
        *muxInputNumber = nGPIO;
      }

      return &g_pdcGpios.map.gpio_inputs[nGPIO];
    }
  }

  return NULL;
}

/*============================================================================
 *                            PUBLIC GPIO FUNCTIONS
 *===========================================================================*/
/*
 * pdcGpio_getNumberSupported
 */
uint32 pdcGpio_getNumberSupported(void)
{
  return g_pdcGpios.numGpio;
}

/*
 * pdcGpio_isSupported
 */
boolean pdcGpio_isSupported(uint32 gpioNumber)
{
  if(NULL != gpio_findEntry(gpioNumber, NULL))
  {
    return TRUE;
  }
  
  return FALSE;
}

/*
 * gpio_findEntryByVector
 */
pdc_status_type gpio_findEntryByVector(uint32 intVector, uint32 *bitNum)
{
  uint32 nGPIO;

  /* Determine if the GPIO is already configured */
  for(nGPIO=0; nGPIO < g_pdcGpios.numGpio; nGPIO++)
  {
    /* Search the valid GPIO's for the requested one */
    if(intVector == g_pdcGpios.map.mux_map[nGPIO].subsystem_interrupt)
    {
      /* Calculate the correct offset for GPIO's which are after the interrupts */
      *bitNum = nGPIO + g_pdcInterrupts.numInt; 
      return PDC_SUCCESS;
    }
  }

  return PDC_INVALID_SS_INT;
}

/*
 * pdcGpio_unconfig
 */
pdc_status_type pdcGpio_unconfig(uint32 gpioNumber)
{
  pdc_gpio_inputs   *gpioInput;
  pdc_gpio_mapping  *map;
  pdc_status_type   status;
  uint32            mainIdx;
  uint32            altIdx;

  PDC_LOCK();

  pdcLog_printf(1, "GPIO unconfig (num: %u)", gpioNumber);

  /* Find the table entry for the given GPIO */
  gpioInput = gpio_findEntry(gpioNumber, NULL);

  if(NULL == gpioInput)
  {
    PDC_UNLOCK();
    return PDC_INVALID_SS_INT;
  }

  if(PDC_GPIO_INVALID == gpioInput->mux.number)
  {
    PDC_UNLOCK();
    return PDC_INVALID_CONFIG;
  }

  /* Ensure GPIO is disabled first */
  mainIdx = gpioInput->mux.main;
  altIdx  = gpioInput->mux.alt;

  status = pdcHAL_disable(mainIdx, PDC_GPIO);
  if((PDC_SUCCESS == status) && (PDC_GPIO_IDX_INVALID != altIdx))
  {
    status = pdcHAL_disable(altIdx, PDC_GPIO);
  }
  
  if(PDC_SUCCESS != status)
  {
    PDC_UNLOCK();
    return status;
  }

  /* Set the MUX as free */
  map                       = &g_pdcGpios.map.mux_map[mainIdx];
  map->gpio_tbl_ptr         = NULL;
  map->trig_config.drv_num  = PDC_DRV_INVALID;

  if(PDC_GPIO_IDX_INVALID != altIdx)
  {
    map                       = &g_pdcGpios.map.mux_map[altIdx];
    map->gpio_tbl_ptr         = NULL;
    map->trig_config.drv_num  = PDC_DRV_INVALID;
  }

  /* Unmap the GPIO */
  gpioInput->mux.number = PDC_GPIO_INVALID;

  PDC_UNLOCK();

  return PDC_SUCCESS;
}

/*
 * pdcGpio_config
 */
pdc_status_type pdcGpio_config(uint32              gpioNumber,
                               pdc_trigger_config  *config,
                               uint32              *subsystemInt,
                               uint32              *secondarySubsystemInt)
{
  pdc_gpio_inputs   *gpioInput;
  pdc_gpio_mapping  *map;
  pdc_gpio_mapping  *secMap;
  uint32            muxNum;
  uint32            secMuxNum;
  uint32            muxInputNumber;
  pdc_status_type   status    = PDC_SUCCESS;
  
  if((NULL == config) || (NULL == subsystemInt))
  {
    status = PDC_INVALID_CONFIG;
  }
  else
  {
    /* Find the table entry for the given GPIO */
    gpioInput = gpio_findEntry(gpioNumber, &muxInputNumber);

    if(NULL == gpioInput)
    {
      status = PDC_INVALID_SS_INT;
    }
  }

  /* If config and GPIO are valid */
  if(PDC_SUCCESS == status)
  {
    PDC_LOCK();
    
    /* First time this GPIO is being registered */
    if(PDC_GPIO_INVALID == gpioInput->mux.number)
    {
      /* Get main mux */
      status = gpio_getFreeMux(&muxNum);
      if(status == PDC_SUCCESS)
      {
        map                 = &g_pdcGpios.map.mux_map[muxNum];
        gpioInput->mux.main = muxNum;             /* Update valid GPIO mux number */
        map->gpio_tbl_ptr   = gpioInput;          /* Point mapping table to valid GPIO */
      
        /* Look for secondary MUX in dual edge configuration */
        if(TRIGGER_DUAL_EDGE == config->trig_type)
        {
          status = gpio_getFreeMux(&secMuxNum);
          if(status == PDC_SUCCESS)
          {
            secMap                = &g_pdcGpios.map.mux_map[secMuxNum];
            gpioInput->mux.alt    = secMuxNum;
            secMap->gpio_tbl_ptr  = gpioInput;
          }
        }
      }
    }
    else
    {
      /* Ensure there is a valid mux value */
      CORE_VERIFY(gpioInput->mux.main < g_pdcGpios.numMux);

      /* GPIO already has a MUX assigned, so just reconfigure with new values. 
       * Main index will always be valid */
      map = &g_pdcGpios.map.mux_map[gpioInput->mux.main];

      if(TRIGGER_DUAL_EDGE == config->trig_type)
      {
        /* If original config was not dual edge, and now it is, we must find a free mux and
         * set it up */
        if(PDC_GPIO_IDX_INVALID != gpioInput->mux.alt)
        {
          secMap = &g_pdcGpios.map.mux_map[gpioInput->mux.alt];
        }
        else
        {
          status = gpio_getFreeMux(&secMuxNum);
          if(status == PDC_SUCCESS)
          {
            secMap                = &g_pdcGpios.map.mux_map[secMuxNum];
            gpioInput->mux.alt    = secMuxNum;
            secMap->gpio_tbl_ptr  = gpioInput;
          }
        }
      }
      else if(PDC_GPIO_IDX_INVALID != gpioInput->mux.alt)
      {
        /* Original was dual edge, and now is re-configuring as not. */
        secMap = &g_pdcGpios.map.mux_map[gpioInput->mux.alt];

        /* Ensure GPIO is disabled first */
        status = pdcHAL_disable(gpioInput->mux.alt, PDC_GPIO);

        /* Set the MUX as free */
        secMap->gpio_tbl_ptr         = NULL;
        secMap->trig_config.drv_num  = PDC_DRV_INVALID;

        /* Unmap the GPIO */
        gpioInput->mux.alt = PDC_GPIO_IDX_INVALID;
      }
    }
  
    /* If found a valid MUX(s) */
    if(PDC_SUCCESS == status)
    {
      /* Update table data to current configuration */
      if(TRIGGER_DUAL_EDGE == config->trig_type)
      {
        /* In dual edge registration, manually set the two mux's trigger types */
        map->trig_config.trig_type    = TRIGGER_RISING_EDGE;
        secMap->trig_config.trig_type = TRIGGER_FALLING_EDGE;
        secMap->trig_config.drv_num   = config->drv_num;

        CORE_VERIFY_PTR(secondarySubsystemInt);
        *secondarySubsystemInt = secMap->subsystem_interrupt;
        status = pdcHAL_setGPIOConfig(gpioInput->mux.alt, muxInputNumber, &secMap->trig_config); 
      }
      else
      {
        map->trig_config.trig_type = config->trig_type;
      }
      
      map->trig_config.drv_num = config->drv_num;
      
      /* Tell the caller the subsytem interrupt to use for the requested GPIO */
      *subsystemInt = map->subsystem_interrupt;

      /* (Re)configure the PDC interrupt. */
      if(PDC_SUCCESS == status)
      {
        status = pdcHAL_setGPIOConfig(gpioInput->mux.main, muxInputNumber, &map->trig_config);
      }
    }

    PDC_UNLOCK();
  }
  
  if(PDC_SUCCESS != status)
  {
    pdcLog_printf(2, 
                  "Configuring GPIO %u failed (status: %d)",
                  gpioNumber, status);
  }
  else
  {
    pdcLog_printf(5, 
                  "Configuring GPIO %u "
                  "(Mux Num: 0x%08x) "
                  "(Mux input: %u) "
                  "(Trigger: %d)",
                  gpioNumber,
                  gpioInput->mux.number,
                  muxInputNumber,
                  config->trig_type); 
  }

  return status;
}

/*
 * pdcGpio_enable
 */
pdc_status_type pdcGpio_enable(uint32 gpioNumber)
{
  pdc_gpio_inputs *gpioInput;
  pdc_status_type status;
  uint32          mainIdx;
  uint32          altIdx;

  /* Find the table entry for the given GPIO */
  gpioInput = gpio_findEntry(gpioNumber, NULL);

  if(NULL == gpioInput)
  {
    return PDC_INVALID_SS_INT;
  }

  if(PDC_GPIO_INVALID == gpioInput->mux.number)
  {
    return PDC_INVALID_CONFIG;
  }

  pdcLog_printf(1, "Enable GPIO %u", gpioNumber);

  PDC_LOCK();

  mainIdx = gpioInput->mux.main;
  altIdx  = gpioInput->mux.alt;

  status = pdcHAL_enable(mainIdx, PDC_GPIO);
  if((PDC_SUCCESS == status) && (PDC_GPIO_IDX_INVALID != altIdx))
  {
    status = pdcHAL_enable(altIdx, PDC_GPIO);
  }

  PDC_UNLOCK();

  return status;
}

/*
 * pdcGpio_disable
 */
pdc_status_type pdcGpio_disable(uint32 gpioNumber)
{
  pdc_gpio_inputs *gpioInput;
  pdc_status_type status;
  uint32          mainIdx;
  uint32          altIdx;

  /* Find the table entry for the given GPIO */
  gpioInput = gpio_findEntry(gpioNumber, NULL);

  if(NULL == gpioInput)
  {
    return PDC_INVALID_SS_INT;
  }

  if(PDC_GPIO_INVALID == gpioInput->mux.number)
  {
    return PDC_INVALID_CONFIG;
  }
 
  pdcLog_printf(1, "Disable GPIO %u", gpioNumber);

  PDC_LOCK();

  mainIdx = gpioInput->mux.main;
  altIdx  = gpioInput->mux.alt;

  status = pdcHAL_disable(mainIdx, PDC_GPIO);
  if((PDC_SUCCESS == status) && (PDC_GPIO_IDX_INVALID != altIdx))
  {
    status = pdcHAL_disable(altIdx, PDC_GPIO);
  }

  PDC_UNLOCK();

  return status;
}

/*============================================================================
 *                      PRIVATE GENERAL DRIVER FUNCTIONS
 *===========================================================================*/
/*
 * pdc_initialize
 */
void pdc_initialize(void)
{
  /* Setup the mapping data */
  CORE_VERIFY(PDC_SUCCESS == pdcMap_getInterruptTable(&g_pdcInterrupts));

  pdcLog_printf(1, 
                " Interrupt configuration "
                "(Number of Interrupts: %u)",
                g_pdcInterrupts.numInt);

  CORE_VERIFY(PDC_SUCCESS == pdcMap_getGpioTable(&g_pdcGpios));

  pdcLog_printf(2, 
                " GPIO configuration "
                "(Number of GPIOs: %u) "
                "(Number of MUXs: %u)",
                g_pdcGpios.numGpio,
                g_pdcGpios.numMux);

  return;
}

/*
 * pdc_getTriggerConfig
 */
pdc_trigger_config *pdc_getTriggerConfig(pdc_pin_type pinType, uint32 index)
{

  if(PDC_IRQ == pinType)
  {
    CORE_VERIFY_PTR(g_pdcInterrupts.map);
    CORE_VERIFY(index < g_pdcInterrupts.numInt);
    return(&g_pdcInterrupts.map[index].trig_config);
  }

  CORE_VERIFY_PTR(g_pdcGpios.map.mux_map);
  CORE_VERIFY(index < g_pdcGpios.numMux);
  return(&g_pdcGpios.map.mux_map[index].trig_config);
}

/*
 * pdc_getSettings
 */
void pdc_getSettings(uint32 *maxGpio, uint32 *maxInt)
{
  uint32 idx;

  CORE_VERIFY_PTR(maxGpio);
  CORE_VERIFY_PTR(maxInt);

  *maxGpio  = 0;
  *maxInt   = 0;

  /* Determine the register bit to configure */
  for(idx = 0; idx < g_pdcInterrupts.numInt; idx++)
  {
    if((g_pdcInterrupts.map[idx].subsystem_interrupt > *maxInt) &&
       (PDC_INT_INVALID != g_pdcInterrupts.map[idx].subsystem_interrupt))
    {
      *maxInt = g_pdcInterrupts.map[idx].subsystem_interrupt;
    }
  }

  /* Determine if the GPIO is already configured */
  for(idx=0; idx < g_pdcGpios.numGpio; idx++)
  {
    /* Search the valid GPIO's for the requested one */
    if((g_pdcGpios.map.gpio_inputs[idx].gpio_num > *maxGpio) &&
       (PDC_MUX_OPEN != g_pdcGpios.map.gpio_inputs[idx].gpio_num))
    {
      *maxGpio = g_pdcGpios.map.gpio_inputs[idx].gpio_num;
    }
  }

  (*maxInt)++;
  (*maxGpio)++;
}

