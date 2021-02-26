/*==================================================================================================

FILE: uart_os.c

DESCRIPTION: This module provides the os based functionalities for the UART.

Copyright (c) 2006 Qualcomm Technologies, Inc.
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

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/UartQupv3Lib/UartXBLLoaderOs.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
11/7/16    VV       Initial revision

==================================================================================================*/

/*-------------------------------------------------------------------------
* Include Files
* ----------------------------------------------------------------------*/
#include "UartOs.h"
#include "UartApi.h"
#include "UartDefs.h"
#include "UartLog.h"
#include "ClockBoot.h"
#include "DDITlmm.h"
#include "UartSettings.h"
#define UART_LOGGING_FILE_ID 40

/*==================================================================================================
                                              TYPEDEFS
==================================================================================================*/

/*==================================================================================================
                                          GLOBAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================

FUNCTION: uart_busy_wait

DESCRIPTION:

==================================================================================================*/
void uart_busy_wait(uint32 usecs)
{
   return;
}

/*==================================================================================================

FUNCTION: uart_clock_close

DESCRIPTION:

==================================================================================================*/
uart_result uart_clock_close(uart_context* h)
{
   return UART_SUCCESS;
}

/*==================================================================================================

FUNCTION: uart_clock_open

DESCRIPTION:

==================================================================================================*/
uart_result uart_clock_open(uart_context* h, uint32 input_freq)
{
   if(Clock_SetQUPV3Frequency((ClockQUPV3Type)h->properties->core_clock_id, input_freq))
   {
      return UART_SUCCESS;
   }
   return UART_ERROR;
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
   h->properties = uart_settings_get_properties(h->port_id);
   return UART_SUCCESS;

}
/*==================================================================================================

FUNCTION: uart_interrupt_close

DESCRIPTION:

==================================================================================================*/
uart_result uart_interrupt_close(uart_context* h)
{
    return UART_SUCCESS;
}

/*==================================================================================================

FUNCTION: uart_interrupt_done

DESCRIPTION:

==================================================================================================*/
uart_result uart_interrupt_done(uart_context* h)
{
    return UART_SUCCESS;
}

/*==================================================================================================

FUNCTION: uart_interrupt_open

DESCRIPTION:

==================================================================================================*/
uart_result uart_interrupt_open(uart_context* h, void* isr)
{
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
   Tlmm_ConfigGpioGroup(DAL_TLMM_GPIO_ENABLE,(DALGpioSignalType*)&h->properties->gpio_tx_config, 1);
   Tlmm_ConfigGpioGroup(DAL_TLMM_GPIO_ENABLE,(DALGpioSignalType*)&h->properties->gpio_rx_config, 1);
   
   return UART_SUCCESS;
}
