/*============================================================================== 
  FILE:         pdcQdi_root.c

  OVERVIEW:     Implements the QDI root PD layer for PDC driver.
                This file is compiled as part of the root PD, but is places in the
                user directory to allow for commonality/seperation of targets that do
                not support or need multi-PD

  DEPENDENCIES: Kernel/target QDI support for multi-PD
  
                Copyright (c) 2016-2018 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/pdc/interrupt/src/client_user/pdcQdi_root.c#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include <stdlib.h>
#include "comdef.h"
#include "CoreVerify.h"
#include "pdcQdi_root.h"
#include "qurt.h"
#include "pdcGpio.h"
#include "pdcInt.h"
#include "pdc_types.h"
#include "pdcLog.h"

/*==============================================================================
                             GLOBAL VARIABLES
 =============================================================================*/
/* Statically defined max number of interrupts and GPIOs */
static uint32 g_pdcQdiMaxGpio = 0;
static uint32 g_pdcQdiMaxInt  = 0;

/* Client QDI opener object */
const qurt_qdi_obj_t g_pdcOpenerData = {pdcQdi_invoke, QDI_REFCNT_PERM, 0};

/*==============================================================================
                            QDI INTERNAL FUNCTIONS
 =============================================================================*/
/*
 * pdcQdi_release
 */ 
void pdcQdi_release(qurt_qdi_obj_t *obj)
{
  uint32          nMsk, nBit;
  pdc_qdi_opener  *pQdiObj = (pdc_qdi_opener *)obj;

  pdcLog_printf(1, "QDI release (Opener: 0x%x)",
                   (uint32)pQdiObj);

  for(nMsk = 0; nMsk < g_pdcQdiMaxInt; nMsk++)
  {
    if(pQdiObj->pIntMask[nMsk] != 0)
    {
      nBit = 0;

      pdcLog_printf(2, " Int QDI release (nMsk: %u) (Value: 0x%x)",
                    nMsk, pQdiObj->pIntMask[nMsk]);

      while(pQdiObj->pIntMask[nMsk] != 0 && nBit < 32)
      {
        if(pQdiObj->pIntMask[nMsk] & (0x1 << nBit))
        {
          pdcInt_disable((nMsk << 5) + nBit);
          pQdiObj->pIntMask[nMsk] &= ~(0x1 << nBit);
        }
        nBit += 1;
      }
    }
  }

  for(nMsk = 0; nMsk < g_pdcQdiMaxGpio; nMsk++)
  {
    if(pQdiObj->pGpioMask[nMsk] != 0)
    {
      nBit = 0;

      pdcLog_printf(2, " GPIO QDI release (nMsk: %u) (Value: 0x%x)",
                    nMsk, pQdiObj->pGpioMask[nMsk]);

      while(pQdiObj->pGpioMask[nMsk] != 0 && nBit < 32)
      {
        if(pQdiObj->pGpioMask[nMsk] & (0x1 << nBit))
        {
          pdcGpio_unconfig((nMsk << 5) + nBit);  /* Disable is called in here as well */
          pQdiObj->pGpioMask[nMsk] &= ~(0x1 << nBit);
        }
        nBit += 1;
      }
    }
  }

  free(pQdiObj->pGpioMask);
  free(pQdiObj->pIntMask);
  free(pQdiObj);

  return;
}

/*
 * pdcQdi_initialize
 */
void pdcQdi_initialize(void)
{
  int status = qurt_qdi_register_devname(PDCINT_QDI_DRIVER_NAME, &g_pdcOpenerData);
  CORE_VERIFY(0 == status);

  /* Get max IRQ and GPIO*/
  pdc_getSettings(&g_pdcQdiMaxGpio, &g_pdcQdiMaxInt);

  g_pdcQdiMaxGpio = ((g_pdcQdiMaxGpio - 1 ) / 32 ) + 1;
  g_pdcQdiMaxInt = ((g_pdcQdiMaxInt - 1 ) / 32 ) + 1;

  return;
}

/*
 * pdcQdi_open
 */
static int pdcQdi_open(int client_handle, pdc_qdi_opener *me)
{
   pdc_qdi_opener *pQdiObj;
   uint32 nMsk;

   pQdiObj = (pdc_qdi_opener *)malloc(sizeof(pdc_qdi_opener));
   if(pQdiObj == NULL)
   {
     return -1;
   }

   pQdiObj->pIntMask = (uint32*)malloc(g_pdcQdiMaxInt * sizeof(uint32));
   pQdiObj->pGpioMask = (uint32*)malloc(g_pdcQdiMaxGpio * sizeof(uint32));
   if(pQdiObj->pIntMask == NULL || pQdiObj->pGpioMask == NULL)
   {
     return -1;
   }

   pQdiObj->qdiobj.invoke   = pdcQdi_invoke;
   pQdiObj->qdiobj.refcnt   = QDI_REFCNT_INIT;
   pQdiObj->qdiobj.release  = pdcQdi_release;

   for(nMsk = 0; nMsk < g_pdcQdiMaxInt; nMsk++)
   {
     pQdiObj->pIntMask[nMsk] = 0;
   }
   
   for(nMsk = 0; nMsk < g_pdcQdiMaxGpio; nMsk++)
   {
     pQdiObj->pGpioMask[nMsk] = 0;
   }

   return qurt_qdi_handle_create_from_obj_t(client_handle, 
                                            &pQdiObj->qdiobj);
}

/*==============================================================================
                        QDI INTERNAL GPIO INTERFACE FUNCTIONS
 =============================================================================*/
/**
 * pdcQdi_gpioConfig
 */
static inline pdc_status_type pdcQdi_gpioConfig(int                 clientHandle,
                                                uint32              gpioNumber,
                                                pdc_trigger_config  *config,
                                                uint32              *subsystemInt,
                                                uint32              *secondarySubsystemInt)
{
  uint32          tempInt = 0;
  uint32          altInt  = 0;
  pdc_status_type status;

  status = pdcGpio_config(gpioNumber, config, &tempInt, &altInt);

  qurt_qdi_copy_to_user(clientHandle, subsystemInt, &tempInt, sizeof(uint32));
  
  if(NULL != secondarySubsystemInt)
  {
    qurt_qdi_copy_to_user(clientHandle, secondarySubsystemInt, &altInt, sizeof(uint32));
  }

  return status;
}

/**
 * pdcQdi_gpioEnable
 */
static inline pdc_status_type pdcQdi_gpioEnable(int clientHandle, pdc_qdi_opener *clntobj, uint32 gpioNumber)
{
  pdc_status_type status;

  status = pdcGpio_enable(gpioNumber);

  if(status == PDC_SUCCESS)
  {
    clntobj->pGpioMask[gpioNumber >> 5] |= (0x1 << (gpioNumber & 0x1f));
  }

  return status;
}

/**
 * pdcQdi_gpioDisable
 */
static inline pdc_status_type pdcQdi_gpioDisable(int clientHandle, pdc_qdi_opener *clntobj, uint32 gpioNumber)
{
  pdc_status_type status;

  status = pdcGpio_disable(gpioNumber);

  if(status == PDC_SUCCESS)
  {
    clntobj->pGpioMask[gpioNumber >> 5] &= ~(0x1 << (gpioNumber & 0x1f));
  }

  return status;
}

/**
 * pdcQdi_gpioUnconfig
 */
static inline pdc_status_type pdcQdi_gpioUnconfig(int clientHandle, pdc_qdi_opener *clntobj, uint32 gpioNumber)
{
  pdc_status_type status;

  status = pdcGpio_unconfig(gpioNumber);

  if(status == PDC_SUCCESS)
  {
    clntobj->pGpioMask[gpioNumber >> 5] &= ~(0x1 << (gpioNumber & 0x1f));
  }

  return status;
}

/*==============================================================================
                        QDI INTERNAL INTERRUPT INTERFACE FUNCTIONS
 =============================================================================*/
/**
 * pdcQdi_intConfig
 */
static inline pdc_status_type pdcQdi_intConfig(int                 clientHandle,
                                               uint32              subSystemInterrupt,
                                               pdc_trigger_config  *config)
{
  pdc_status_type status;

  status = pdcInt_config(subSystemInterrupt, config);
  
  return status;
}

/**
 * pdcQdi_intDisable
 */
static inline pdc_status_type pdcQdi_intDisable(int clientHandle, pdc_qdi_opener *clntobj, uint32 subSystemInterrupt)
{
  pdc_status_type status;

  status = pdcInt_disable(subSystemInterrupt);

  if(status == PDC_SUCCESS)
  {
    clntobj->pIntMask[subSystemInterrupt >> 5] &= ~(0x1 << (subSystemInterrupt & 0x1f));
  }

  return status;
}

/**
 * pdcQdi_intEnable
 */
static inline pdc_status_type pdcQdi_intEnable(int clientHandle, pdc_qdi_opener *clntobj, uint32 subSystemInterrupt)
{
  pdc_status_type status;

  status = pdcInt_enable(subSystemInterrupt);

  if(status == PDC_SUCCESS)
  {
    clntobj->pIntMask[subSystemInterrupt >> 5] |= (0x1 << (subSystemInterrupt & 0x1f));
  }

  return status;
}

/*==============================================================================
                           EXTERNAL FUNCTIONS
 =============================================================================*/
/*
 * pdcQdi_invoke
 */
int pdcQdi_invoke(int             clientHandle,
                  qurt_qdi_obj_t  *pobj,
                  int             method,
                  qurt_qdi_arg_t  a1,
                  qurt_qdi_arg_t  a2,
                  qurt_qdi_arg_t  a3,
                  qurt_qdi_arg_t  a4,
                  qurt_qdi_arg_t  a5,
                  qurt_qdi_arg_t  a6,
                  qurt_qdi_arg_t  a7,
                  qurt_qdi_arg_t  a8,
                  qurt_qdi_arg_t  a9)
{
   pdc_qdi_opener *clntobj = (pdc_qdi_opener *)pobj;
   
   switch(method)
   {
     case QDI_ISLAND_CHECK_ALLOC:
       return TRUE;
        
     case QDI_OPEN:
       return pdcQdi_open(clientHandle, clntobj);

     case PDC_QDI_INT_CONFIG:
       return pdcQdi_intConfig(clientHandle,
                               a1.num,
                               (pdc_trigger_config *)a2.ptr);

     case PDC_QDI_INT_ENABLE:
       return pdcQdi_intEnable(clientHandle, clntobj, a1.num);

     case PDC_QDI_INT_DISABLE:
       return pdcQdi_intDisable(clientHandle, clntobj, a1.num);

     case PDC_QDI_GPIO_CONFIG:
       return pdcQdi_gpioConfig(clientHandle,
                                a1.num,
                                (pdc_trigger_config *)a2.ptr,
                                (uint32 *)a3.ptr,
                                (uint32 *)a4.ptr);

     case PDC_QDI_GPIO_ENABLE:
       return pdcQdi_gpioEnable(clientHandle, clntobj, a1.num);

     case PDC_QDI_GPIO_DISABLE:
       return pdcQdi_gpioDisable(clientHandle, clntobj, a1.num);

     case PDC_QDI_GPIO_UNCONFIG:
       return pdcQdi_gpioUnconfig(clientHandle, clntobj, a1.num);

     default:
       return qurt_qdi_method_default(clientHandle, pobj, method,
                                      a1, a2, a3, a4, a5, a6, a7, a8, a9);
   }
}

