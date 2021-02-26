/*==================================================================================================

FILE: uart_os.c

DESCRIPTION: This module provides the os based functionalities for the UART.

Copyright (c) 2009-2015,2017 Qualcomm Technologies, Inc.
        All Rights Reserved.
Qualcomm Technologies, Inc. Confidential and Proprietary.

==================================================================================================*/
/*==================================================================================================
                                            DESCRIPTION
====================================================================================================

GLOBAL FUNCTIONS:
   uart_busy_wait
   uart_clock_open
   uart_clock_close
   uart_get_driver_properties
   uart_get_properties
   uart_interrupt_close
   uart_interrupt_done
   uart_interrupt_open
   uart_tlmm_open
   uart_tlmm_close
   uart_busy_wait

==================================================================================================*/
/*==================================================================================================
Edit History

$Header: //components/rel/core.qdsp6/2.1/buses/uart_lite/src/uart_uimage_os.c#5 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
07/31/17   RC      Added clock and TLMM support.
09/25/13   VV      Initial revision.

==================================================================================================*/

/*-------------------------------------------------------------------------
* Include Files
* ----------------------------------------------------------------------*/
#include "DALSys.h"
#include "uart_os.h"
#include "uart.h"
#include "uart_defs.h"
#include "uart_log.h"
#include "uClock.h"
#include "busywait.h"
#include "string.h"
#include "uTlmm.h"
#include "DDIHWIO.h"
#include "DALDeviceId.h"
#include "DDIInterruptController.h"
#include "DDIChipInfo.h"
#include "qurt_island.h"
#include "qurt.h"
#include "DDIClock.h"
#include "ClockDefs.h"
#include "stdlib.h"
#include "uInterruptController.h"

extern UART_PROPERTIES uart_devices[UART_MAX_PORTS];
static DalDeviceHandle *clock_handle = NULL;
static DalDeviceHandle *dal_hwio = NULL;

#define UART_LOGGING_FILE_ID 40

/*==================================================================================================
                                              TYPEDEFS
==================================================================================================*/
typedef struct
{
   DalDeviceHandle* dal_interrupt_handle;
   uint32  irq_num;
   uint32* uart_tcsr_addr;
   uint32  tcsr_mask;
}UART_INTERRUPT_CONTEXT;

/*==================================================================================================
                                          GLOBAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================

FUNCTION: uart_busy_wait

DESCRIPTION:

==================================================================================================*/
void uart_busy_wait(uint32 usecs)
{
   busywait(usecs);
}

/*==================================================================================================

FUNCTION: uart_clock_close

DESCRIPTION:

==================================================================================================*/
uart_result uart_clock_close(uart_context* h)
{
   DALResult        res;
   ClockIdType      se_id = 0;

   if(!(uClock_DisableClock((uClockIdType)((*((const char **) h->properties->core_clock_id))[12] - 0x30)) &&
        uClock_DisableClock(CLOCK_SCC_QUPV3_S_HCLK_CLK) &&
        uClock_DisableClock(CLOCK_SCC_QUPV3_M_HCLK_CLK) &&
        uClock_DisableClock(CLOCK_SCC_QUPV3_2XCORE_CLK) &&
        uClock_DisableClock(CLOCK_SCC_QUPV3_CORE_CLK)))
   {
      return UART_ERROR;
   }

   if (!qurt_island_get_status ())
   {
      res = DalClock_GetClockId(clock_handle, *((const char **) h->properties->core_clock_id), &se_id);
      if (DAL_SUCCESS != res) { return UART_ERROR; }

      res = DalClock_DisableClock(clock_handle, se_id);
      if (DAL_SUCCESS != res) { return UART_ERROR; }
   }


   return UART_SUCCESS;
}

/*==================================================================================================

FUNCTION: uart_clock_open

DESCRIPTION:

==================================================================================================*/
uart_result uart_clock_open(uart_context* h, uint32 input_freq)
{
   DALResult        res;
   ClockIdType      se_id = 0;

   if (clock_handle == NULL)
   {
      res = DAL_ClockDeviceAttach(DALDEVICEID_CLOCK, &clock_handle);
      if (DAL_SUCCESS != res) { return UART_ERROR; }

   }

   if (!qurt_island_get_status ())
   {
      res = DalClock_GetClockId(clock_handle, *((const char **) h->properties->core_clock_id), &se_id);
      if (DAL_SUCCESS != res) { return UART_ERROR; }

      res = DalClock_SetClockFrequency(clock_handle, se_id, input_freq, CLOCK_FREQUENCY_HZ_EXACT, NULL);
      if (DAL_SUCCESS != res) { return UART_ERROR; }

   }

   if(uClock_EnableClock(CLOCK_SCC_QUPV3_CORE_CLK) &&
      uClock_EnableClock(CLOCK_SCC_QUPV3_2XCORE_CLK)   &&
      uClock_EnableClock(CLOCK_SCC_QUPV3_M_HCLK_CLK) &&
      uClock_EnableClock(CLOCK_SCC_QUPV3_S_HCLK_CLK) &&
      uClock_EnableClock((uClockIdType)((*((const char **) h->properties->core_clock_id))[12] - 0x30)))
   {
      if (!qurt_island_get_status())
      {
         res = DalClock_EnableClock(clock_handle, se_id);
         if (DAL_SUCCESS != res) { return UART_ERROR; }
      }
      return UART_SUCCESS;
   }
   else
   {
      return UART_ERROR;
   }
}

/*==================================================================================================

FUNCTION: uart_get_driver_propertiess

DESCRIPTION:

==================================================================================================*/
uart_result uart_get_driver_properties( void )
{
   return UART_SUCCESS;
}

/*==================================================================================================

FUNCTION: uart_get_properties

DESCRIPTION:

==================================================================================================*/
uart_result uart_get_properties(uart_context* h)
{
   uint32 i;
   uint32 num_devices = 0;
   uint32 uart_paddr = 0;
   DALSYSMemInfo meminfo;

   DALSYSMemHandle      uart_mem_handle;
   UART_LOG(INFO, "+uart_get_properties");

   num_devices = sizeof(uart_devices)/sizeof(uart_devices[0]);

   for (i = 0; i < num_devices; i++)
   {
      if (uart_devices[i].port_id == h->port_id)
      {
         break;
      }
   }
   if (i == num_devices)
   {
      return UART_ERROR;
   }

   h->properties = &uart_devices[i];

   if (dal_hwio == NULL)
   {
      // Attach to the HWIO device and get the virtual address of the UART base address.
      if(DAL_HWIODeviceAttach(DALDEVICEID_HWIO, &dal_hwio) == DAL_SUCCESS)
      {
         if(DalDevice_Open(dal_hwio, DAL_OPEN_SHARED) != DAL_SUCCESS)
         {
            return UART_ERROR;
         }
      }
      else
      {
         return UART_ERROR;
      }
   }

   uart_paddr = h->properties->uart_base_phy;

   if(!h->properties->uart_base)
   {
      if(  DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_HWIO,
                            DALSYS_MEM_ADDR_NOT_SPECIFIED,
                            (DALSYSMemAddr)uart_paddr,
                            0x4000, &uart_mem_handle, NULL) == DAL_SUCCESS)
      {

         DALSYS_MemInfo(uart_mem_handle, &meminfo);
         h->properties->uart_base = (uint32)meminfo.VirtualAddr;
      }
      else
      {
         return UART_ERROR;
      }
   }

   UART_LOG(INFO, "-uart_get_properties.");
   return UART_SUCCESS;

}
/*==================================================================================================

FUNCTION: uart_interrupt_close

DESCRIPTION:

==================================================================================================*/
uart_result uart_interrupt_close(uart_context* h)
{
   UART_INTERRUPT_CONTEXT* uart_interrupt_context;
   DALResult result = DAL_ERROR;

   UART_LOG(INFO, "+uart_interrupt_close");
   if (h->interrupt_context == NULL)
   {
      goto done;
   }
   uart_interrupt_context = (UART_INTERRUPT_CONTEXT * )h->interrupt_context;

   // De-register the interrupt.
   result = DalInterruptController_Unregister(uart_interrupt_context->dal_interrupt_handle,
                                              (DALInterruptID)uart_interrupt_context->irq_num);

   // Close the DAL interrupt device.
   DalDevice_Close(uart_interrupt_context->dal_interrupt_handle);
   DAL_DeviceDetach(uart_interrupt_context->dal_interrupt_handle);

   DALSYS_Free((void*)uart_interrupt_context);

done:
   UART_LOG(INFO, "-uart_interrupt_close");
   return (result == DAL_SUCCESS) ? (UART_SUCCESS) : (UART_ERROR);
}

/*==================================================================================================

FUNCTION: uart_interrupt_done

DESCRIPTION:

==================================================================================================*/
uart_result uart_interrupt_done(uart_context* h)
{
   DALResult result = DAL_ERROR;
   UART_INTERRUPT_CONTEXT* uart_interrupt_context;

   UART_LOG(INFO, "+uart_interrupt_done");

   uart_interrupt_context = (UART_INTERRUPT_CONTEXT*)h->interrupt_context;
   result = DalInterruptController_InterruptDone(uart_interrupt_context->dal_interrupt_handle,
                                                    (DALInterruptID)uart_interrupt_context->irq_num);

   UART_LOG(INFO, "-uart_interrupt_done");
   return (result == DAL_SUCCESS) ? (UART_SUCCESS) : (UART_ERROR);
}

/*==================================================================================================

FUNCTION: uart_interrupt_open

DESCRIPTION:

==================================================================================================*/
uart_result uart_interrupt_open(uart_context* h, void* isr, boolean enable_island_mode)
{
   UART_INTERRUPT_CONTEXT* uart_interrupt_context;
   DalDeviceHandle* interrupt_dal = NULL;
   DALResult result;
   uint32 tcsr_vaddr;

   UART_LOG(INFO, "+uart_interrupt_open");


   result = DALSYS_Malloc(sizeof(UART_INTERRUPT_CONTEXT), (void **)(&uart_interrupt_context));
   if (result != DAL_SUCCESS)
   {
      UART_LOG(ERROR,"DALSYS_Malloc Attach failed");
      return UART_ERROR;
   }
   h->interrupt_context = (void*)uart_interrupt_context;
   result = DAL_DeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER, &interrupt_dal);
   if (result != DAL_SUCCESS)
   {
      UART_LOG(ERROR,"DALDEVICEID_INTERRUPTCONTROLLER Attach failed");
      return UART_ERROR;
   }

   result = DalDevice_Open(interrupt_dal, DAL_OPEN_SHARED);
   if (result != DAL_SUCCESS)
   {
      UART_LOG(ERROR,"DALDEVICEID_INTERRUPTCONTROLLER open failed");
      return UART_ERROR;
   }

   // Get the interrupt specific Properties in to the interrupt context.
   uart_interrupt_context->irq_num = h->properties->irq_num;
   uart_interrupt_context->dal_interrupt_handle  =  interrupt_dal;

   result = DalInterruptController_RegisterISR(uart_interrupt_context->dal_interrupt_handle,
                                               (DALInterruptID)uart_interrupt_context->irq_num,
                                               (DALISR)isr, h,
                                               DALINTRCTRL_ENABLE_CONFIG_EX(DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER, 0, enable_island_mode));
   if (result != DAL_SUCCESS)
   {
      UART_LOG(ERROR,"DalInterruptController_RegisterISR failed");
      return UART_ERROR;
   }

   if (h->properties->tcsr_base != 0)
   {
      if(DalHWIO_MapRegionByAddress(dal_hwio,
                                          (uint8*)h->properties->tcsr_base,
                                          (uint8**)&tcsr_vaddr) != DAL_SUCCESS)
      {
         UART_LOG(INFO, "-uart_interrupt_open");
         return UART_ERROR;
      }

      *(uint32*)(tcsr_vaddr + h->properties->tcsr_offset) = h->properties->tcsr_value;
   }

   UART_LOG(INFO, "-uart_interrupt_open");

   return UART_SUCCESS;

}

/*==================================================================================================

FUNCTION: uart_tlmm_close

DESCRIPTION:

==================================================================================================*/
uart_result uart_tlmm_close(uart_context* h)
{
   return UART_SUCCESS;
}

/*==================================================================================================

FUNCTION: uart_tlmm_open

DESCRIPTION:

==================================================================================================*/
uart_result uart_tlmm_open(uart_context* h)
{
   uTlmm_ConfigGpio ((uTlmmGpioSignalType) h->properties->gpio_tx_config, UTLMM_GPIO_ENABLE);
   uTlmm_ConfigGpio ((uTlmmGpioSignalType) h->properties->gpio_rx_config, UTLMM_GPIO_ENABLE);

   return UART_SUCCESS;
}

/*==================================================================================================

FUNCTION: uart_wakeup_interrupt_register

DESCRIPTION:

==================================================================================================*/

uart_result uart_wakeup_interrupt_register(uart_context *h,
                                           void* isr,
                                           boolean enable_at_pdc,
                                           boolean enable_interrupt,
                                           boolean island_intr)
{
   UART_INTERRUPT_CONTEXT* uart_interrupt_context;
   DALResult result = DAL_SUCCESS;

   UART_LOG(INFO, "+uart_wakeup_interrupt_register. Enable_pdc = %ld", enable_at_pdc);
   if (h->interrupt_context == NULL)
   {
      return UART_ERROR;
   }
   uart_interrupt_context = (UART_INTERRUPT_CONTEXT * )h->interrupt_context;

   // register the interrupt.
   if(enable_at_pdc && (qurt_island_get_status() == 0))
   {
      UART_LOG(INFO, "+uart_wakeup_interrupt_register. call DAL interrupt");

      result = DalInterruptController_RegisterISR(uart_interrupt_context->dal_interrupt_handle,
                                                  (DALInterruptID)h->properties->wakeup_irq_num,
                                                  (DALISR)isr, h,
                                                  DALINTRCTRL_ENABLE_CONFIG_EX(DALINTRCTRL_ENABLE_FALLING_EDGE_TRIGGER,
                                                                               0,
                                                                               island_intr));
   }

   if(island_intr)
   {
      if(!enable_interrupt)
      {
         uInterruptController_Enable(h->properties->wakeup_irq_num, FALSE);
      }

      if(enable_interrupt)
      {
         uInterruptController_Enable(h->properties->wakeup_irq_num, TRUE);
      }
   }

   UART_LOG(INFO, "-uart_wakeup_interrupt_register");

   return((result == DAL_SUCCESS) ? UART_SUCCESS : UART_ERROR);
}

/*==================================================================================================

FUNCTION: uart_wakeup_interrupt_unregister

DESCRIPTION:

==================================================================================================*/

uart_result uart_wakeup_interrupt_unregister(uart_context *h, boolean disable_at_pdc)
{
   UART_INTERRUPT_CONTEXT* uart_interrupt_context;
   DALResult result = DAL_SUCCESS;

   UART_LOG(INFO, "+uart_wakeup_interrupt_unregister");
   if (h->interrupt_context == NULL)
   {
      return UART_ERROR;
   }
   uart_interrupt_context = (UART_INTERRUPT_CONTEXT * )h->interrupt_context;

   if(disable_at_pdc)
   {
      // De-register the interrupt.
      result = DalInterruptController_Unregister(uart_interrupt_context->dal_interrupt_handle,
                                                 (DALInterruptID)h->properties->wakeup_irq_num);
   }
   else
   {
      uInterruptController_Enable(h->properties->wakeup_irq_num, FALSE);
   }

   UART_LOG(INFO, "-uart_wakeup_interrupt_unregister");

   return ((result == DAL_SUCCESS) ? UART_SUCCESS : UART_ERROR);

}

void uart_mutex_init(uart_context* h)
{
   qurt_mutex_init(&h->uart_mutex);
}

void uart_mutex_lock(uart_context* h)
{
   UART_LOG(INFO, "+uart_mutex_lock");

   qurt_mutex_lock(&h->uart_mutex);

   UART_LOG(INFO, "-uart_mutex_lock");

}
void uart_mutex_unlock(uart_context* h)
{
   UART_LOG(INFO, "+uart_mutex_unlock");

   qurt_mutex_unlock(&h->uart_mutex);

   UART_LOG(INFO, "-uart_mutex_unlock");

}

void uart_mutex_destroy(uart_context* h)
{
   qurt_mutex_destroy(&h->uart_mutex);
}

