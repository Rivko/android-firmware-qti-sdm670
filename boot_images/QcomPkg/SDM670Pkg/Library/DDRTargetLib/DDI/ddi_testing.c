/**
 * @file ddi_testing.c
 * @brief
 * DDI testing entry point
 */
/*=============================================================================

                                DDI Test cases
                                Source File
GENERAL DESCRIPTION
This file defines the entry point for DDI testing

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
#include <string.h>

#include "ddi_testing.h"
#include "ddi_flash_params.h"

#include "boot_util.h"
#include "ddr_external.h"
#include "ddr_params.h"
#include "boot_extern_ddi_interface.h"
#include "boot_emergency_dload.h"

/*==============================================================================
                                  DATA
==============================================================================*/

extern test_case_func test_case_list[];
extern uint8 ddi_number_of_cmds;

// internal output buffer which will be flushed to the flash
uint8 ddi_int_out[DDI_INT_OUT_BUF_SIZE];

#if DDI_PRINT_ENABLE
char ddi_log_string[50];
#endif

#define DDI_OUTPUT_SECTION_START_IN_DDR		SIZE_724KB
/*==============================================================================
                                  FUNCTIONS
==============================================================================*/

/* ============================================================================
**  Function : ddi_run_command
** ============================================================================
*/
/*!
*   @brief
*   based on input command ID, calls DDI test function
*
*   @param cmd         -   [IN] pointer to DCB data for command header
*   @param input_iter  -   [IN] pointer to current DCB offset
*   @param output_iter -   [IN] pointer to current DDR partition offset
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval None
*
*   @sa None
*/
#if defined FEATURE_BOOT_DDI_ENABLE
void ddi_run_command(ddi_command_header* cmd, uint64* input_iter, uint64* output_iter)
{
  test_case_func test;
  if ( cmd->cmd_id >= ddi_number_of_cmds)
  {
	  boot_log_message("DDI invalid cmd\r\n");
  }
  test = test_case_list[cmd->cmd_id];
  test(cmd->cmd_id, (void*)(*input_iter), input_iter, output_iter);
}


/* ============================================================================
**  Function : ddi_run_commands
** ============================================================================
*/
/*!
*   @brief
*   high-level algorithm for running DDI test cases
*
*   @param cmds_hdr    -   [IN] pointer to DCB data for commands header
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval None
*
*   @sa None
*/
void ddi_run_commands(ddi_commands_header* cmds_hdr)
{
  // trackers for input and output offsets
  ddi_command_header* cmd_hdr;
  uint64 input_iter = (uint64)((void*)cmds_hdr + sizeof(ddi_commands_header));
  uint64 output_iter = 0;
  uint32 num_cmds = cmds_hdr->num_cmds;

  sbl1_read_from_ddr_partition(DDI_OUTPUT_SECTION_START_IN_DDR, ddi_int_out, sizeof(ddi_int_out));
  if (*((uint32*)ddi_int_out) == DDI_CMDP_COOKIE)
  {
	  boot_log_message("DDI output present, go to EDL");
	  boot_dload_transition_pbl_forced_dload();
  }
  // mark commands as processed in the beginning of testing
  *((uint32*)ddi_int_out) = DDI_CMDP_COOKIE;
  save_ddi_data2ddr_partition(ddi_int_out, sizeof(uint32), output_iter);
  output_iter += sizeof(uint32);

  do {
    for (uint32 i = 0; i < num_cmds; i++) {
      cmd_hdr = (ddi_command_header*)input_iter;
      input_iter += sizeof(ddi_command_header);
      ddi_run_command(cmd_hdr, &input_iter, &output_iter);
    }
  } while(cmds_hdr->boot_id == DDI_BOOT_ID_REPEAT);

  if (cmds_hdr->boot_id == DDI_BOOT_ID_EDL) {
    #if DDI_PRINT_ENABLE  
    snprintf(ddi_log_string, sizeof(ddi_log_string), "returning to EDL");
    boot_log_message(ddi_log_string);
    boot_dload_transition_pbl_forced_dload();
    #endif
  }
}

/* ============================================================================
**  Function : ddr_ddi_testing
** ============================================================================
*/
/*!
*   @brief
*   checks version number and decides whether test should be run or not
*
*   @param ddi_data    -   [IN] pointer to DCB data for all DDI input
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval boolean
*
*   @sa None
*/
boolean ddr_ddi_testing(void* ddi_data)
{
  if (ddi_data == NULL)
    return FALSE;
  ddi_commands_header* cmds_hdr = (ddi_commands_header*)ddi_data;
  uint32 major = cmds_hdr->ddi_version >> 16;
  uint32 minor = cmds_hdr->ddi_version & 0xFF;
  if (major == DDI_MAJOR_VERSION && minor == DDI_MINOR_VERSION) {
    ddi_run_commands((ddi_commands_header*)ddi_data);
    return TRUE;
  }
  return FALSE;
}

#endif //FEATURE_BOOT_DDI_ENABLE
/* ============================================================================
**  Function : ddi_entry
** ============================================================================
*/
/*!
*   @brief
*   entry point for ddi testing
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval None
*
*   @sa None
*/
void ddi_entry()
{
#if defined FEATURE_BOOT_DDI_ENABLE
  void* ddi_data = ddr_external_read_ddi_binary();
   ddr_ddi_testing(ddi_data);
#endif
}
