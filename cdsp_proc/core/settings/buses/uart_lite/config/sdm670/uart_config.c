/*==================================================================================================

FILE: uart_slpi_uimage_[target].c

DESCRIPTION: This module provides the os based functionalities for the UART.

Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
        All Rights Reserved.
Qualcomm Technologies, Inc. Confidential and Proprietary.

==================================================================================================*/
/*==================================================================================================
                                            DESCRIPTION
====================================================================================================


==================================================================================================*/
/*==================================================================================================
Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/settings/buses/uart_lite/config/sdm670/uart_config.c#1 $

when       who     what, where, why
--------   --------     --------------------------------------------------------
01/31/17   PR            Update Se clock string for Fix of race condition. 
07/31/17   RC            Updated the configurations as per the latest code base.
05/19/17   RC            Initial revision.

==================================================================================================*/

/*-------------------------------------------------------------------------
* Include Files
* ----------------------------------------------------------------------*/
#include "uart.h"
#include "uart_defs.h"


const char *se_clocks_str [] =
{
    "scc_qupv3_se0_clk_src",
    "scc_qupv3_se1_clk_src",
    "scc_qupv3_se2_clk_src",
    "scc_qupv3_se3_clk_src",
    "scc_qupv3_se4_clk_src",
    "scc_qupv3_se5_clk_src",
};

UART_PROPERTIES uart_devices[] =
{
   {
      .port_id          = UART_INSTANCE_04,
      .uart_base_phy    = 0x6268C000,
      .uart_base        = 0,
      .gpio_tx_config   = 0x0001C0C1,
      .gpio_rx_config   = 0x000080D1,
      .gpio_cts_config  = 0,
      .gpio_rfr_config  = 0,
      .bus_clock_id     = (void*)0,
      .core_clock_id    = (void*)(se_clocks_str + 3),
      .irq_num          = 103,
      .tcsr_base        = 0,
      .tcsr_offset      = 0,
      .tcsr_value       = 0,
   },
   {
      .port_id          = UART_INSTANCE_05,
      .uart_base_phy    = 0x62690000,
      .uart_base        = 0,
      .gpio_tx_config   = 0x0001C0E1,
      .gpio_rx_config   = 0x000080F1,
      .gpio_cts_config  = 0,
      .gpio_rfr_config  = 0,
      .bus_clock_id     = (void*)0,
      .core_clock_id    = (void*)(se_clocks_str + 4),
      .irq_num          = 104,
      .tcsr_base        = 0,
      .tcsr_offset      = 0,
      .tcsr_value       = 0,
   },
};
