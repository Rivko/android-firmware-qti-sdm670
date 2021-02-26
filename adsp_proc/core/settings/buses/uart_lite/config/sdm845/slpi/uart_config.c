/*==================================================================================================

FILE: uart_slpi_uimage_[target].c

DESCRIPTION: This module provides the os based functionalities for the UART.

Copyright (c) 2009-2015,2017 Qualcomm Technologies, Inc.
        All Rights Reserved.
Qualcomm Technologies, Inc. Confidential and Proprietary.

==================================================================================================*/
/*==================================================================================================
                                            DESCRIPTION
====================================================================================================


==================================================================================================*/
/*==================================================================================================
Edit History

$Header: //components/rel/core.qdsp6/2.1/settings/buses/uart_lite/config/sdm845/slpi/uart_config.c#2 $

when       who     what, where, why
--------   --------     --------------------------------------------------------
07/31/17   RC            Updated the configurations as per the latest code base.
08/12/15   stranche     Added irq numbers to structs
01/26/15   VV      Initial revision.

==================================================================================================*/

/*-------------------------------------------------------------------------
* Include Files
* ----------------------------------------------------------------------*/
#include "uart.h"
#include "uart_defs.h"


const char *se_clocks_str [] =
{
    "scc_qupv3_se0_clk",
    "scc_qupv3_se1_clk",
    "scc_qupv3_se2_clk",
    "scc_qupv3_se3_clk",
    "scc_qupv3_se4_clk",
    "scc_qupv3_se5_clk",
};

UART_PROPERTIES uart_devices[] =
{
   {
      UART_INSTANCE_04,
      0x05A8C000,    // uart_base_phy
      0,
      0x2001C0C1,  // gpio_tx_config
      0x200080D1,  // gpio_rx_config
      0,           // gpio_cts_config
      0,           // gpio_rfr_config
      0,           // clock_id_index
      (void*)0,        // bus_clock_id
      (void*)(se_clocks_str + 3), // core_clock_id
      34,          // irq number
	   0,   //TCSR base
      0,  // TCSR offset
      0,           // TCSR value
   },
   {
      UART_INSTANCE_05,      
      0x05A90000,  // uart_base
      0,
      0x2001C0E1,  // gpio_tx_config
      0x200080F1,  // gpio_rx_config
      0,           // gpio_cts_config
      0,           // gpio_rfr_config
      0,           // clock_id_index
      (void*)0,        // bus_clock_id
      (void*)(se_clocks_str + 4), // core_clock_id
      35,          // irq number
      0,   //TCSR base
      0,  // TCSR offset
      0,           // TCSR value
   },
   {
      UART_INSTANCE_06,      
      0x05A94000,  // uart_base
      0,
      0x2001C101,  // gpio_tx_config
      0x20008111,  // gpio_rx_config
      0,           // gpio_cts_config
      0,           // gpio_rfr_config
      0,           // clock_id_index
      (void*)0,    // bus_clock_id
      (void*)(se_clocks_str + 5), // core_clock_id
      36,          // irq number
      0,   //TCSR base
      0,  // TCSR offset
      0,           // TCSR value
   },
   {
      UART_DEBUG_INSTANCE,      
      0x05A90000,  // uart_base
      0,
      0x2001C0E1,  // gpio_tx_config
      0x200080F1,  // gpio_rx_config
      0,           // gpio_cts_config
      0,           // gpio_rfr_config
      0,           // clock_id_index
      (void*)0,        // bus_clock_id
      (void*)(se_clocks_str + 4), // core_clock_id
      35,          // irq number
	   0,   //TCSR base
      0,  // TCSR offset
      0,           // TCSR value
   },
};
