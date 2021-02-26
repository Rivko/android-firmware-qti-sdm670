/*==================================================================================================

FILE: uart_slpi_uimage_[target].c

DESCRIPTION: This module provides the os based functionalities for the UART.

Copyright (c) 2009-2015 Qualcomm Technologies, Inc.
        All Rights Reserved.
Qualcomm Technologies, Inc. Confidential and Proprietary.

==================================================================================================*/
/*==================================================================================================
                                            DESCRIPTION
====================================================================================================


==================================================================================================*/
/*==================================================================================================
Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/settings/buses/uart_lite/config/sdm845/adsp/uart_config.c#1 $

when       who     what, where, why
--------   --------     --------------------------------------------------------
08/12/15   stranche     Added irq numbers to structs
01/26/15   VV      Initial revision.

==================================================================================================*/

/*-------------------------------------------------------------------------
* Include Files
* ----------------------------------------------------------------------*/
#include "uart.h"
#include "uart_defs.h"

UART_PROPERTIES uart_devices[] =
{
   {
      UART_DEBUG_INSTANCE,      
      0x0A88000,  // uart_base
      0,
      0x2001C0C1,  // gpio_tx_config
      0x200080D1,  // gpio_rx_config
      0,           // gpio_cts_config
      0,           // gpio_rfr_config
      0,           // clock_id_index
      (void*)0,        // bus_clock_id
      (void*)0, // core_clock_id
      35,          // irq number
      0,   //TCSR base
      0,  // TCSR offset
      0,           // TCSR value
   },
};
