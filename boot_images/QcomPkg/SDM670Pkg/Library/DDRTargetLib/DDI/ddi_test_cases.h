/** 
  @file  ddi_test_cases.h
  @brief defines DDI test case APIs
*/
/*=============================================================================

                                DDI Test Case PARAMETERS
                                Header File
GENERAL DESCRIPTION
This file defines the DDI test case APIs

  Copyright (c) 2016-2017 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary.
===============================================================================

===============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/19/17   na      Initial version.
==============================================================================*/
#ifndef __DDI_TEST_CASES_H__
#define __DDI_TEST_CASES_H__

/*==============================================================================
                               INCLUDES
==============================================================================*/

#include "ddi_flash_params.h"

/*==============================================================================
                                  TYPES & DEFINES
==============================================================================*/

struct blocked_area {
  uint64 start;
  uint64 end;
};
#define BLOCKED_AREAS_SIZE (sizeof(BLOCKED_AREAS)/sizeof(struct blocked_area))

/*==========================================================================
                               FUNCTION DEFINITIONS
===========================================================================*/

// Topoligy functions

/* ============================================================================
**  Function : ddi_get_rank
** ============================================================================
*/
/*!
*   @brief
*   Based on address, get rank
*
*   @param addr        -   [IN] address to translate
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval uint64
*
*   @sa None
*/
uint64 ddi_get_rank (uint64* addr);

/* ============================================================================
**  Function : ddi_get_channel
** ============================================================================
*/
/*!
*   @brief
*   Based on address, get channel
*
*   @param addr        -   [IN] address to translate
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval uint64
*
*   @sa None
*/
uint64 ddi_get_channel (uint64* addr);

/* ============================================================================
**  Function : ddi_get_row
** ============================================================================
*/
/*!
*   @brief
*   Based on address, get row
*
*   @param addr        -   [IN] address to translate
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval uint64
*
*   @sa None
*/
uint64 ddi_get_row (uint64* addr);

/* ============================================================================
**  Function : ddi_get_cs0_start
** ============================================================================
*/
/*!
*   @brief
*   get starting cs0 address
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval uint64*
*
*   @sa None
*/
uint64* ddi_get_cs0_start();

/* ============================================================================
**  Function : ddi_get_cs0_end
** ============================================================================
*/
/*!
*   @brief
*   get ending cs0 address
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval uint64*
*
*   @sa None
*/
uint64* ddi_get_cs0_end();

/* ============================================================================
**  Function : ddi_get_cs1_start
** ============================================================================
*/
/*!
*   @brief
*   get starting cs1 address
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval uint64*
*
*   @sa None
*/
uint64* ddi_get_cs1_start();

/* ============================================================================
**  Function : ddi_get_cs1_end
** ============================================================================
*/
/*!
*   @brief
*   get ending cs1 address
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval uint64*
*
*   @sa None
*/
uint64* ddi_get_cs1_end();

// random generator functions

/* ============================================================================
**  Function : swsys_srand_uefi
** ============================================================================
*/
/*!
*   @brief
*   set seed for random data generation
*
*   @details
*   set seed for random data generation
*   if seen value is 0, then a predefined seed value will be used
*
*   @param seed        -   [IN] seed value to set
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
void swsys_srand_uefi (uint32 seed);

/* ============================================================================
**  Function : swsys_srand_uefi_32
** ============================================================================
*/
/*!
*   @brief
*   get random int32
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval int32
*
*   @sa None
*/
int32 swsys_rand_uefi_32 ();

/* ============================================================================
**  Function : swsys_srand_uefi_64
** ============================================================================
*/
/*!
*   @brief
*   get random int64
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval int64
*
*   @sa None
*/
int64 swsys_rand_uefi_64 ();

/* ============================================================================
**  Function : ddi_get_frequency_plan_index
** ============================================================================
*/
/*!
*   @brief
*   get plan index matching the frequency value
*
*   @details
*   get plan index matching the frequency value
*
*   @param freq_khz  -   [IN] pointer to data comparison parameters
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval index or -1 if the frequency is not supported by the plan
*
*   @sa None
*/
int8 ddi_get_frequency_plan_index(uint32 freq_khz);

// data moving functions

/* ============================================================================
**  Function : ddi_cmp_set_failure
** ============================================================================
*/
/*!
*   @brief
*   entry point for logging a data comparison failure
*
*   @details
*   run when a data comparison mismatch is seen when validating a test case
*
*   @param cmp_params  -   [IN] pointer to data comparison parameters
*   @param addr        -   [IN] failing address
*   @param expected_val-   [IN] expected data
*   @param read_val    -   [IN] read data
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
boolean ddi_cmp_set_failure(ddi_cmp_params* cmp_params, uint64* addr, uint64 expected_val, uint64 read_val);

/* ============================================================================
**  Function : ddr_setpattern
** ============================================================================
*/
/*!
*   @brief
*   sets a user defined pattern to a data region
*
*   @param start       -   [IN] starting address of data region
*   @param end         -   [IN] ending address of data region
*   @param pattern     -   [IN] pointer to pattern array
*   @param pattern_size-   [IN] size of pattern array
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval ddi_error
*
*   @sa None
*/
ddi_error ddr_setpattern(uint64* start, uint64* end, uint64* pattern, uint64 pattern_size);

/* ============================================================================
**  Function : ddr_cmppattern
** ============================================================================
*/
/*!
*   @brief
*   compare data region against a user defined pattern
*
*   @param start       -   [IN] starting address of data region
*   @param end         -   [IN] ending address of data region
*   @param pattern     -   [IN] pointer to pattern array
*   @param pattern_size-   [IN] size of pattern array
*   @param cmp_params  -   [IN] pointer to data comparison parameters
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval ddi_error
*
*   @sa None
*/
ddi_error ddr_cmppattern(uint64* start, uint64* end, uint64* pattern, uint64 pattern_size, ddi_cmp_params* cmp_params);

/* ============================================================================
**  Function : ddr_setrand
** ============================================================================
*/
/*!
*   @brief
*   set data region with random data
*
*   @param start       -   [IN] starting address of data region
*   @param end         -   [IN] ending address of data region
*   @param rand_stride -   [IN] random data stride length
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval ddi_error
*
*   @sa None
*/
ddi_error ddr_setrand(uint64* start, uint64* end, uint64 rand_stride);

/* ============================================================================
**  Function : ddr_cmprand
** ============================================================================
*/
/*!
*   @brief
*   set data region with random data
*
*   @param start       -   [IN] starting address of data region
*   @param end         -   [IN] ending address of data region
*   @param rand_stride -   [IN] random data stride length
*   @param cmp_params  -   [IN] pointer to data comparison parameters
*
*   @par Dependencies
*   random seed must be set to original as when setrand was first called
*
*   @par Side Effects
*   None
*
*   @retval ddi_error
*
*   @sa None
*/
ddi_error ddr_cmprand(uint64* start, uint64* end, uint64 rand_stride, ddi_cmp_params* cmp_params);

/* ============================================================================
**  Function : ddr_setvalue
** ============================================================================
*/
/*!
*   @brief
*   sets a value to a data region
*
*   @param start       -   [IN] starting address of data region
*   @param end         -   [IN] ending address of data region
*   @param value       -   [IN] value to set
*   @param invert_row  -   [IN] option to invert the data on every DRAM row
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval ddi_error
*
*   @sa None
*/
ddi_error ddr_setvalue(uint64* start, uint64* end, uint64 value, boolean invert_row);

/* ============================================================================
**  Function : ddr_cmpvalue
** ============================================================================
*/
/*!
*   @brief
*   compares a value with a data region
*
*   @param start       -   [IN] starting address of data region
*   @param end         -   [IN] ending address of data region
*   @param value       -   [IN] value to set
*   @param cmp_params  -   [IN] pointer to data comparison parameters
*   @param invert_row  -   [IN] option to invert the data on every DRAM row
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval ddi_error
*
*   @sa None
*/
ddi_error ddr_cmpvalue(uint64* start, uint64* end, uint64 value, ddi_cmp_params* cmp_params, boolean invert_row);

/* ============================================================================
**  Function : ddr_op_value
** ============================================================================
*/
/*!
*   @brief
*   data operations when setting or comparing a value
*
*   @param start       -   [IN] starting address of data region
*   @param end         -   [IN] ending address of data region
*   @param value       -   [IN] value to set
*   @param cmp_params  -   [IN] pointer to data comparison parameters
*   @param invert_row  -   [IN] option to invert the data on every DRAM row
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval ddi_error
*
*   @sa None
*/
ddi_error ddr_op_value(uint64* start, uint64* end, uint64 value, ddi_cmp_params* cmp_params, boolean invert_row);

/* ============================================================================
**  Function : ddr_op_pattern
** ============================================================================
*/
/*!
*   @brief
*   data operations when setting or comparing a user defined pattern
*
*   @param start       -   [IN] starting address of data region
*   @param end         -   [IN] ending address of data region
*   @param pattern     -   [IN] pointer to pattern array
*   @param pattern_size-   [IN] size of pattern array
*   @param cmp_params  -   [IN] pointer to data comparison parameters
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval ddi_error
*
*   @sa None
*/
ddi_error ddr_op_pattern(uint64* start, uint64* end, uint64* pattern, uint64 pattern_size, ddi_cmp_params* cmp_params);

/* ============================================================================
**  Function : ddr_op_rand
** ============================================================================
*/
/*!
*   @brief
*   data operations when setting or comparing random data
*
*   @param start       -   [IN] starting address of data region
*   @param end         -   [IN] ending address of data region
*   @param rand_stride -   [IN] random data stride length
*   @param cmp_params  -   [IN] pointer to data comparison parameters
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval ddi_error
*
*   @sa None
*/
ddi_error ddr_op_rand(uint64* start, uint64* end, uint64 rand_stride, ddi_cmp_params* cmp_params);

/* ============================================================================
**  Function : ddr_op_read
** ============================================================================
*/
/*!
*   @brief
*   data operations when generating read traffic
*
*   @param start       -   [IN] starting address of data region
*   @param end         -   [IN] ending address of data region
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval ddi_error
*
*   @sa None
*/
ddi_error ddr_op_read(uint64* start, uint64* end);

/* ============================================================================
**  Function : ddr_mem_op
** ============================================================================
*/
/*!
*   @brief
*   entry point for data operations
*
*   @param start       -   [IN] starting address of data region
*   @param end         -   [IN] ending address of data region
*   @param invert_row  -   [IN] flag to invert row or not
*   @param rand        -   [IN] flag to use random data or not
*   @param rand_stride -   [IN] random data stride length
*   @param pattern     -   [IN] pointer to pattern array
*   @param pattern_size-   [IN] size of pattern array
*   @param op_type     -   [IN] data operation type
*   @param cmp_params  -   [IN] pointer to data comparison parameters
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval ddi_error
*
*   @sa None
*/
ddi_error ddr_mem_op(uint64* start, uint64* end, boolean invert_row, boolean rand, uint64 rand_stride, uint64* pattern, uint64 pattern_size, ddi_mem_op_type op_type, ddi_cmp_params* cmp_params);

// abstracted memort access functions

/* ============================================================================
**  Function : ddr_mem_op
** ============================================================================
*/
/*!
*   @brief
*   entry point for data operations where blocked memory regions are avoided
*   filter the user input to set start and end based on mapped data region
*
*   @param start       -   [IN] starting address of data region
*   @param end         -   [IN] ending address of data region
*   @param invert_row  -   [IN] flag to invert row or not
*   @param rand        -   [IN] flag to use random data or not
*   @param rand_stride -   [IN] random data stride length
*   @param pattern     -   [IN] pointer to pattern array
*   @param pattern_size-   [IN] size of pattern array
*   @param op_type     -   [IN] data operation type
*   @param cmp_params  -   [IN] pointer to data comparison parameters
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval ddi_error
*
*   @sa None
*/
ddi_error ddi_mem_op(uint64* start, uint64* end, boolean invert_row, boolean random, uint64 random_stride, uint64* pattern, uint64 pattern_size, ddi_mem_op_type op_type, ddi_cmp_params* cmp_params);

/* ============================================================================
**  Function : ddr_mem_set
** ============================================================================
*/
/*!
*   @brief
*   entry point for memory set data operations
*
*   @param start       -   [IN] starting address of data region
*   @param end         -   [IN] ending address of data region
*   @param invert_row  -   [IN] flag to invert row or not
*   @param rand        -   [IN] flag to use random data or not
*   @param rand_stride -   [IN] random data stride length
*   @param pattern     -   [IN] pointer to pattern array
*   @param pattern_size-   [IN] size of pattern array
*   @param op_type     -   [IN] data operation type
*   @param cmp_params  -   [IN] pointer to data comparison parameters
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval ddi_error
*
*   @sa None
*/
ddi_error ddi_mem_set(uint64* start, uint64* end, boolean invert_row, boolean random, uint64 random_stride, uint64* pattern, uint64 pattern_size);

/* ============================================================================
**  Function : ddr_mem_cmp
** ============================================================================
*/
/*!
*   @brief
*   entry point for memory comp data operations
*
*   @param start       -   [IN] starting address of data region
*   @param end         -   [IN] ending address of data region
*   @param invert_row  -   [IN] flag to invert row or not
*   @param rand        -   [IN] flag to use random data or not
*   @param rand_stride -   [IN] random data stride length
*   @param pattern     -   [IN] pointer to pattern array
*   @param pattern_size-   [IN] size of pattern array
*   @param op_type     -   [IN] data operation type
*   @param cmp_params  -   [IN] pointer to data comparison parameters
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval ddi_error
*
*   @sa None
*/
ddi_error ddi_mem_cmp(uint64* start, uint64* end, boolean invert_row, boolean random, uint64 random_stride, uint64* pattern, uint64 pattern_size, ddi_cmp_params* cmp_params);

/* ============================================================================
**  Function : ddr_mem_read
** ============================================================================
*/
/*!
*   @brief
*   entry point for memory read data operations
*
*   @param start       -   [IN] starting address of data region
*   @param end         -   [IN] ending address of data region
*   @param invert_row  -   [IN] flag to invert row or not
*   @param rand        -   [IN] flag to use random data or not
*   @param rand_stride -   [IN] random data stride length
*   @param pattern     -   [IN] pointer to pattern array
*   @param pattern_size-   [IN] size of pattern array
*   @param op_type     -   [IN] data operation type
*   @param cmp_params  -   [IN] pointer to data comparison parameters
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval ddi_error
*
*   @sa None
*/
ddi_error ddi_mem_read(uint64* start, uint64* end, boolean random, uint64 random_stride, uint64* pattern, uint64 pattern_size);

/* ============================================================================
**  Function : ddr_mem_op_block_filter
** ============================================================================
*/
/*!
*   @brief
*   entry point for data operations where blocked memory regions are avoided
*
*   @param start       -   [IN] starting address of data region
*   @param end         -   [IN] ending address of data region
*   @param invert_row  -   [IN] flag to invert row or not
*   @param rand        -   [IN] flag to use random data or not
*   @param rand_stride -   [IN] random data stride length
*   @param pattern     -   [IN] pointer to pattern array
*   @param pattern_size-   [IN] size of pattern array
*   @param op_type     -   [IN] data operation type
*   @param cmp_params  -   [IN] pointer to data comparison parameters
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval ddi_error
*
*   @sa None
*/
ddi_error ddi_mem_op_block_filter(uint64* start, uint64* end, boolean invert_row, boolean random, uint64 random_stride, uint64* pattern, uint64 pattern_size, ddi_mem_op_type op_type, ddi_cmp_params* cmp_params);

// helper functions

/* ============================================================================
**  Function : get_start_end
** ============================================================================
*/
/*!
*   @brief
*   filter the user input to set start and end based on mapped data region
*
*   @param start       -   [IN] starting address of data region
*   @param end         -   [IN] ending address of data region
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval ddi_error
*
*   @sa None
*/
ddi_error get_start_end(uint64* start, uint64* end);

/* ============================================================================
**  Function : ddi_delay
** ============================================================================
*/
/*!
*   @brief
*   API for setting delay values small or large values
*
*   @param delay_us    -   [IN] delay time
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval ddi_error
*
*   @sa None
*/
void ddi_delay(uint64 delay_us);

/* ============================================================================
**  Function : ddi_add_fail_summary
** ============================================================================
*/
/*!
*   @brief
*   This function is called to log a failure to the test case fail summary
*
*   @param fail_summary-   [IN] pointer to test case failure summary
*   @param addr        -   [IN] failing address
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
void ddi_add_fail_summary (ddi_fail_summary* fail_summary, uint64* addr);

/* ============================================================================
**  Function : ddi_handle_fail_rdwr
** ============================================================================
*/
/*!
*   @brief
*   This function is called to log a read/write failure to the flash
*
*   @details
*   This will log a DRAM retention test case failure to the flash
*
*   @param fail_params -   [IN] pointer to test case failure summary
*   @param addr        -   [IN] failing address
*   @param expected    -   [IN] expected data
*   @param read        -   [IN] read data
*   @param output_iter -   [IN] pointer to current DDR partition offset
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
boolean ddi_handle_fail_rdwr(ddi_handle_fail_params* fail_params, uint64* addr, uint64 expected, uint64 read, uint64* output_iter);

// test case entry points

/* ============================================================================
**  Function : ddi_run_command_delay
** ============================================================================
*/
/*!
*   @brief
*   This function is called to run the delay DDI test case
*
*   @param cmd_id      -   [IN] ID of the command. Used for logging to flash.
*   @param params      -   [IN] pointer to test case parameters
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
void ddi_run_command_delay(uint64 cmd_id, void* params, uint64* input_iter, uint64* output_iter);

/* ============================================================================
**  Function : ddi_run_command_rd
** ============================================================================
*/
/*!
*   @brief
*   This function is called to run the read DDI test case
*
*   @param cmd_id      -   [IN] ID of the command. Used for logging to flash.
*   @param params      -   [IN] pointer to test case parameters
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
void ddi_run_command_rd(uint64 cmd_id, void* params, uint64* input_iter, uint64* output_iter);

/* ============================================================================
**  Function : ddi_run_command_wr
** ============================================================================
*/
/*!
*   @brief
*   This function is called to run the write DDI test case
*
*   @param cmd_id      -   [IN] ID of the command. Used for logging to flash.
*   @param params      -   [IN] pointer to test case parameters
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
void ddi_run_command_wr(uint64 cmd_id, void* params, uint64* input_iter, uint64* output_iter);

/* ============================================================================
**  Function : ddi_run_command_rdwr
** ============================================================================
*/
/*!
*   @brief
*   This function is called to run the read/write DDI test case
*
*   @param cmd_id      -   [IN] ID of the command. Used for logging to flash.
*   @param params      -   [IN] pointer to test case parameters
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
void ddi_run_command_rdwr(uint64 cmd_id, void* params, uint64* input_iter, uint64* output_iter);

/* ============================================================================
**  Function : ddi_run_command_freq
** ============================================================================
*/
/*!
*   @brief
*   This function is called to run the frequency switch DDI test case
*
*   @param cmd_id      -   [IN] ID of the command. Used for logging to flash.
*   @param params      -   [IN] pointer to test case parameters
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
void ddi_run_command_freq(uint64 cmd_id, void* params, uint64* input_iter, uint64* output_iter);

/* ============================================================================
**  Function : ddi_run_command_plot
** ============================================================================
*/
/*!
*   @brief
*   This function is called to run the Eye Plot DDI test case
*
*   @details
*   This will call into DSF to plot the CDC vs Vref data eyes.
*   A data eye will be generated for every bit per enabled rank/channel/byte
*
*   @param fail_params -   [IN] pointer to test case failure summary
*   @param addr        -   [IN] failing address
*   @param expected    -   [IN] expected data
*   @param read        -   [IN] read data
*   @param output_iter -   [IN] pointer to current DDR partition offset
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
void ddi_run_command_plot(uint64 cmd_id, void* params, uint64* input_iter, uint64* output_iter);

/* ============================================================================
**  Function : ddi_read_eCDT_parameters
** ============================================================================
*/
/*!
*   @brief
*   This function is called to read eCDT settingd from device
*
*   @param cmd_id      -   [IN] ID of the command. Used for logging to flash.
*   @param params      -   [IN] pointer to test case parameters
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
void ddi_read_ecdt_parameters(uint64 cmd_id, void* params, uint64* input_iter, uint64* output_iter);

#endif /* __DDI_TESTING_H__ */
