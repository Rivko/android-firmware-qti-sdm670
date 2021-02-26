/**
 * @file ddi_test_list.c
 * @brief
 * DDI test list.
 */
/*=============================================================================

                                DDI Test cases
                                Source File
GENERAL DESCRIPTION
This file defines the DDI test cases available

  Copyright (c) 2016-2017 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary.
===============================================================================

===============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

===============================================================================
when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/19/17   na      Initial version
==============================================================================*/

/*==============================================================================
                                  INCLUDES
==============================================================================*/

#include "ddi_testing.h"
#include "ddi_test_cases.h"

/*==============================================================================
                                  DATA
==============================================================================*/

test_case_func test_case_list[] = {
    ddi_run_command_rd,
    ddi_run_command_wr,
    ddi_run_command_rdwr,
    ddi_run_command_delay,
    ddi_run_command_freq,
    ddi_run_command_delay, //dummy placeholder
    ddi_run_command_plot,
    ddi_run_command_delay, //dummy placeholder
    ddi_run_command_delay, //dummy placeholder
    ddi_read_ecdt_parameters,
};
uint8 ddi_number_of_cmds = sizeof(test_case_list)/sizeof(test_case_list[0]);