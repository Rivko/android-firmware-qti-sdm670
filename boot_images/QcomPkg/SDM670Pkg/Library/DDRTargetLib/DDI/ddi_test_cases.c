/**
 * @file ddi_test_cases.c
 * @brief
 * DDI test cases.
 */
/*=============================================================================

                                DDI Test cases
                                Source File
GENERAL DESCRIPTION
This file defines the DDI test cases

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

#include "ddi_flash_params.h"
#include "ddi_testing.h"
#include "ddi_test_cases.h"
#include "ddr_params.h"
#include "boot_util.h"
#include "boot_emergency_dload.h"
#include "DALSys.h"
#include "ddr_drivers.h" 

/*==============================================================================
                                  DATA
==============================================================================*/

#if DDI_PRINT_ENABLE
extern char ddi_log_string[50];
#endif

uint8 ddi_int_out[DDI_INT_OUT_BUF_SIZE];

uint32 swsys_mul_wid = 0xFCA01979;
uint32 swsys_mul_zid = 0xBA031903;

struct blocked_area BLOCKED_AREAS[] = {
  //{.start = 0x106000000, .end = 0x107000000}
};

/*==============================================================================
                                  FUNCTIONS
==============================================================================*/


/* ============================================================================
**  Function : ddi_malloc
** ============================================================================
*/
void* ddi_malloc(size_t size)
{
	DALResult ret;
	void* mem_p;
	ret = DALSYS_Malloc( (uint32)size,(void**)&mem_p);
	if (ret != DAL_SUCCESS)
    {
       // DALSYS_Malloc doesn't guarantee NULL pOut like normal malloc
       mem_p = NULL;
    }
	return mem_p;
}
/* ============================================================================
**  Function : ddi_free
** ============================================================================
*/
void ddi_free(void* buffer_p)
{
	DALSYS_Free(buffer_p);
}


/* ============================================================================
**  Function : ddi_get_frequency_plan_index
** ============================================================================
*/
int8 ddi_get_frequency_plan_index(uint32 freq_khz)
{
	int8 i;
	int8 freq_plan_index=-1;
	
	if (freq_khz == 0)
	{
		return -1;
	}
	
	for (i=0;i<(sizeof(ddrsns_share_data->aop_params.mc_states)/sizeof(ddrsns_share_data->aop_params.mc_states[0]));i++)
	{
		if (freq_khz == ddrsns_share_data->aop_params.mc_states[i].freq_khz)
		{
			freq_plan_index = i;
			break;
		}
	}
    return freq_plan_index;
}

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
uint64 ddi_get_rank (uint64* addr) {
  return (uint64)addr >> 32 & 0x1;
}

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
uint64 ddi_get_channel (uint64* addr) {
  return (uint64)addr >> 9 & 0x3;
}

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
uint64 ddi_get_row (uint64* addr) {
  return (uint64)addr >> 16 & 0xFFFF;
}

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
uint64* ddi_get_cs0_start()
{
  return (uint64*)ddrsns_share_data->shrm_ddr_ptr->ddr_size_info.ddr_cs0_remapped_addr[0];
}

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
uint64* ddi_get_cs0_end()
{
  uint32 i;
  void* ret = (void*)ddrsns_share_data->shrm_ddr_ptr->ddr_size_info.ddr_cs0_remapped_addr[0];
  for (i = 0; i < ddrsns_share_data->shrm_ddr_ptr->misc.num_channel; i++) {
    ret += ddrsns_share_data->shrm_ddr_ptr->ddr_size_info.ddr_cs0_mb[i] << 20;
  }
  return (uint64*)ret;
}

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
uint64* ddi_get_cs1_start()
{
  uint64* ret = (uint64*)ddrsns_share_data->shrm_ddr_ptr->ddr_size_info.ddr_cs1_remapped_addr[0];
  if (ret == 0) {
    return ddi_get_cs0_end();
  } else {
    return ret;
  }
}

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
uint64* ddi_get_cs1_end()
{
  uint32 i;
  void* ret = (void*)ddrsns_share_data->shrm_ddr_ptr->ddr_size_info.ddr_cs1_remapped_addr[0];
  for (i = 0; i < ddrsns_share_data->shrm_ddr_ptr->misc.num_channel; i++) {
    ret += ddrsns_share_data->shrm_ddr_ptr->ddr_size_info.ddr_cs1_mb[0] << 20;
  }
  if (ret == 0) {
    return ddi_get_cs0_end();
  } else {
    return (uint64*)ret;
  }
}

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
void swsys_srand_uefi (uint32 seed)
{
  if (seed != 0 && seed != 0x464FFFFF) {
    swsys_mul_wid = seed;
  }
  else {
    swsys_mul_wid = 0xFCA01979;
  }
  swsys_mul_zid = 0xBA031903;
}

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
int32 swsys_rand_uefi_32 ()
{
  swsys_mul_zid = 36969 * (swsys_mul_zid & 65535) + (swsys_mul_zid >> 16);
  swsys_mul_wid = 18000 * (swsys_mul_wid & 65535) + (swsys_mul_wid >> 16);
  return ((swsys_mul_zid << 16) + swsys_mul_wid);
}

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
int64 swsys_rand_uefi_64 ()
{
  return ((int64)swsys_rand_uefi_32() << 32) + swsys_rand_uefi_32();
}

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
boolean ddi_cmp_set_failure(ddi_cmp_params* cmp_params, uint64* addr, uint64 expected_val, uint64 read_val) {
  return cmp_params->handle_fail(cmp_params->handle_fail_params, addr, expected_val, read_val, cmp_params->output_iter);
}

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
ddi_error ddr_setpattern(uint64* start, uint64* end, uint64* pattern, uint64 pattern_size)
{
  uint64 size = end - start;
  uint8* ptr = (uint8*)start;
  while (ptr < (uint8*)end) {

    if ((pattern_size*sizeof(uint64)) < size) {
      size = pattern_size*sizeof(uint64);
    }
    qmemscpy(ptr, size, pattern, size);
    ptr += size;
    size = (uint8*)end - ptr;
  }
  return DDI_CMD_COMPLETE;
}

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
ddi_error ddr_cmppattern(uint64* start, uint64* end, uint64* pattern, uint64 pattern_size, ddi_cmp_params* cmp_params)
{
  ddi_error ret = DDI_CMD_COMPLETE;
  uint64 i;
  uint64 size = end - start;
  for (i = 0; i < size; i++) {
    // TODO: validate that expected_val is not a read failure
    uint64 expected_val = pattern[i % pattern_size]; // may cause analigned access
    uint64 read_val = start[i];
    if (read_val != expected_val) {
      ret |= DDI_FAIL;
      if (ddi_cmp_set_failure(cmp_params, &start[i], expected_val, read_val)) {
        return ret;
      }
    }
  }
  return ret;
}

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
ddi_error ddr_setrand(uint64* start, uint64* end, uint64 rand_stride)
{
  ddi_error ret = DDI_CMD_COMPLETE;
  uint64 i;
  uint64 size = end - start;
  boolean use_rand_stride = FALSE;
  if (rand_stride && rand_stride < size) {
    // set the first rand_stride bytes to random data,
    // then use it as a pattern for the rest of the buffer
    use_rand_stride = TRUE;
    size = rand_stride;
  }
  for (i = 0; i < size; i++) {
    start[i] = swsys_rand_uefi_64();
  }
  if (use_rand_stride) {
    uint64* start_pattern = start + rand_stride;
    ret = ddr_setpattern(start_pattern, end, start, rand_stride);
  }
  return ret;
}

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
ddi_error ddr_cmprand(uint64* start, uint64* end, uint64 rand_stride, ddi_cmp_params* cmp_params)
{
  uint64 i;
  ddi_error ret = DDI_CMD_COMPLETE;
  uint64 size = end - start;
  boolean use_rand_stride = FALSE;
  if (rand_stride && rand_stride < size) {
    // in this case, we should regenerate the random data for
    // the first stride, then do a standard pattern compare
    // against it. This should avoid subsequent strides from
    // reporting corruptions if the first stride has a corruption.
    use_rand_stride = TRUE;
    size = rand_stride;
  }
  for (i = 0; i < size; i++) {
    uint64 expected_val = swsys_rand_uefi_64();
    uint64 read_val = start[i];
    if (read_val != expected_val) {
      ret |= DDI_FAIL;
      if (ddi_cmp_set_failure(cmp_params, &start[i], expected_val, read_val)) {
        return ret;
      }
    }
  }
  if (use_rand_stride) {
    uint64* start_pattern = start + rand_stride;
    ret |= ddr_cmppattern(start_pattern, end, start, rand_stride, cmp_params);
  }
  return ret;
}

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
ddi_error ddr_setvalue(uint64* start, uint64* end, uint64 value, boolean invert_row)
{
  uint64* ptr;
  uint64 cur_value;
  for (ptr = start; ptr < end; ptr++) {
    if (invert_row && (ddi_get_row(ptr) & 0x1))
      cur_value = ~value;
    else
      cur_value = value;

    *ptr = cur_value;
  }

  return DDI_CMD_COMPLETE;
}

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
ddi_error ddr_cmpvalue(uint64* start, uint64* end, uint64 value, ddi_cmp_params* cmp_params, boolean invert_row)
{
  ddi_error ret = DDI_CMD_COMPLETE;
  uint64* ptr;
  uint64 cur_value;

  for (ptr = start; ptr < end; ptr++) {
    uint64 read_val = *ptr;
    if (invert_row && ddi_get_row(ptr) & 0x1)
      cur_value = ~value;
    else
      cur_value = value;

    if (read_val != cur_value) {
      ret |= DDI_FAIL;
      if (ddi_cmp_set_failure(cmp_params, ptr, read_val, value)) {
        return ret;
      }
    }
  }
  return ret;
}

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
ddi_error ddr_op_value(uint64* start, uint64* end, uint64 value, ddi_cmp_params* cmp_params, boolean invert_row)
{
  if (cmp_params) {
    return ddr_cmpvalue(start, end, value, cmp_params, invert_row);
  }
  else {
    return ddr_setvalue(start, end, value, invert_row);
  }
}

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
ddi_error ddr_op_pattern(uint64* start, uint64* end, uint64* pattern, uint64 pattern_size, ddi_cmp_params* cmp_params)
{
  if (cmp_params) {
    return ddr_cmppattern(start, end, pattern, pattern_size, cmp_params);
  }
  else {
    return ddr_setpattern(start, end, pattern, pattern_size);
  }
}

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
ddi_error ddr_op_rand(uint64* start, uint64* end, uint64 rand_stride, ddi_cmp_params* cmp_params)
{
  if (cmp_params) {
    return ddr_cmprand(start, end, rand_stride, cmp_params);
  }
  else {
    return ddr_setrand(start, end, rand_stride);
  }
}

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
ddi_error ddr_op_read(uint64* start, uint64* end)
{
  // TODO: we can probably optimize this for more stress
  volatile uint64 val = 0;
  while (start < end) {
    val = *start;
    start++;
  }
  return DDI_CMD_COMPLETE;
}

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
ddi_error ddr_mem_op(uint64* start, uint64* end, boolean invert_row, boolean rand, uint64 rand_stride, uint64* pattern, uint64 pattern_size, ddi_mem_op_type op_type, ddi_cmp_params* cmp_params)
{
  if ((end > start) && !rand && !pattern_size) {
    rand = TRUE;
  }
  if (op_type == DDI_MEM_OP_RD) {
    return ddr_op_read(start, end);
  }
  else if (rand) {
    return ddr_op_rand(start, end, rand_stride, cmp_params);
  }
  else if (pattern_size == 1) {
    return ddr_op_value(start, end, *pattern, cmp_params, invert_row);
  }
  else {
    return ddr_op_pattern(start, end, pattern, pattern_size, cmp_params);
  }
}

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
ddi_error ddi_mem_op_block_filter(uint64* start, uint64* end, boolean invert_row, boolean random, uint64 random_stride, uint64* pattern, uint64 pattern_size, ddi_mem_op_type op_type, ddi_cmp_params* cmp_params)
{
  uint64* i = start;
  ddi_error err = DDI_CMD_COMPLETE;
  uint64 ba_num;
  for (ba_num = 0; ba_num < BLOCKED_AREAS_SIZE; ba_num++) {
    uint64 ba_start = BLOCKED_AREAS[ba_num].start;
    uint64 ba_end = BLOCKED_AREAS[ba_num].end;
    // TODO: handle gap between ranks as well
    if ((uint64)i < ba_start) {
      if ((uint64)end >= ba_start) {
        err |= ddr_mem_op(i, (uint64*)ba_start, invert_row, random, random_stride, pattern, pattern_size, op_type, cmp_params);
      }
      else {
        err |= ddr_mem_op(i, end, invert_row, random, random_stride, pattern, pattern_size, op_type, cmp_params);
      }
    }
    i = (uint64*)ba_end;
  }
  if (i < end) {
    // fill the remainder after all blocked areas
    err |= ddr_mem_op(i, end, invert_row, random, random_stride, pattern, pattern_size, op_type, cmp_params);
  }
  return err;
}

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
ddi_error get_start_end(uint64* start, uint64* end) {
  uint64 cs0_start = (uint64)ddi_get_cs0_start();
  uint64 cs1_end = (uint64)ddi_get_cs1_end();

  // set the boundaries to the ddr region
  if (*start == 0) {
    *start = cs0_start;
  }
  if (*end == 0) {
    *end = cs1_end;
  }

  if (*start > *end || *start < cs0_start || *end > cs1_end) {
    return DDI_INVALID_INPUT;
  }

  #if DDI_PRINT_ENABLE
  snprintf(ddi_log_string, sizeof(ddi_log_string), "start: 0x%08x%08x", *start>>32, *start&0xFFFFFFFF);
  boot_log_message(ddi_log_string);
  snprintf(ddi_log_string, sizeof(ddi_log_string), "end: 0x%08x%08x", *end>>32, *end&0xFFFFFFFF);
  boot_log_message(ddi_log_string);
  #endif

  return DDI_CMD_COMPLETE;
}

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
ddi_error ddi_mem_op(uint64* start, uint64* end, boolean invert_row, boolean random, uint64 random_stride, uint64* pattern, uint64 pattern_size, ddi_mem_op_type op_type, ddi_cmp_params* cmp_params)
{
  uint64* cs0_end = ddi_get_cs0_end();
  uint64* cs1_start = ddi_get_cs1_start();
  ddi_error err = DDI_CMD_COMPLETE;

  ddr_unlock_xpu_protected_mem_region();
  
  // requirements:
  // start is inclusive
  // end is exclusive
  // start and end must be 8 bytes apart
  // start and end must be uint64 aligned
  if (end - start < 8 || 
      (uint64)end % 8 != 0 || 
      (uint64)start % 8 != 0
  ) {
    return DDI_INVALID_INPUT;
  }

  // skip the gap between ranks if there is one
  if (start < cs0_end) {
    if (end < cs0_end) {
      err |= ddi_mem_op_block_filter(start, end, invert_row, random, random_stride, pattern, pattern_size, op_type, cmp_params);
    }
    else {
      err |= ddi_mem_op_block_filter(start, cs0_end, invert_row, random, random_stride, pattern, pattern_size, op_type, cmp_params);
      if (cs1_start != 0 && end > cs1_start) {
        err |= ddi_mem_op_block_filter(cs1_start, end, invert_row, random, random_stride, pattern, pattern_size, op_type, cmp_params);
      }
    }
  }
  else if (start <= cs1_start) {
    if (end <= cs1_start) {
      return DDI_INVALID_INPUT;
    }
    else {
      err |= ddi_mem_op_block_filter(cs1_start, end, invert_row, random, random_stride, pattern, pattern_size, op_type, cmp_params);
    }
  }
  else {
    err |= ddi_mem_op_block_filter(start, end, invert_row, random, random_stride, pattern, pattern_size, op_type, cmp_params);
  }
  return err;
}

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
ddi_error ddi_mem_set(uint64* start, uint64* end, boolean invert_row, boolean random, uint64 random_stride, uint64* pattern, uint64 pattern_size)
{
  return ddi_mem_op(start, end, invert_row, random, random_stride, pattern, pattern_size, DDI_MEM_OP_WR_CMP, NULL);
}

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
ddi_error ddi_mem_cmp(uint64* start, uint64* end, boolean invert_row, boolean random, uint64 random_stride, uint64* pattern, uint64 pattern_size, ddi_cmp_params* cmp_params)
{
  return ddi_mem_op(start, end, invert_row, random, random_stride, pattern, pattern_size, DDI_MEM_OP_WR_CMP, cmp_params);
}

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
ddi_error ddi_mem_read(uint64* start, uint64* end, boolean random, uint64 random_stride, uint64* pattern, uint64 pattern_size)
{
  return ddi_mem_op(start, end, FALSE, random, random_stride, pattern, pattern_size, DDI_MEM_OP_RD, NULL);
}

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
void ddi_delay(uint64 delay_us)
{
  uint64 i;
  uint64 timer_loop_us = 1000000;
  uint64 timer_loop_max = delay_us / timer_loop_us;
  for (i = 0; i < timer_loop_max; i++) {
    seq_wait(timer_loop_us, US);
  }
  seq_wait(delay_us % timer_loop_us, US);
}

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

void ddi_read_ecdt_parameters(uint64 cmd_id, void* params, uint64* input_iter, uint64* output_iter)
{
  int8 freq_plan_index;
  uint8 number_of_freq;
  void* buffer = NULL;
  size_t buffer_size = 256;
  int ret;
  ddi_out_ecdt_settings * ecdt_output;

  uint64 header_offset = *output_iter;
  *output_iter += sizeof(ddi_output_header);
  ddi_output_header output_header;
  output_header.cmd_id = cmd_id;
  output_header.test_status = DDI_CMD_COMPLETE;

  boot_log_message("DDI: read eCDT");
  do{
	buffer = ddi_malloc(buffer_size);
	if (buffer == NULL)
	{
		//failed to malloc
		output_header.test_status = DDI_FAIL;
		break;
	}
  
	number_of_freq = (sizeof(ddrsns_share_data->aop_params.mc_states)/sizeof(ddrsns_share_data->aop_params.mc_states[0])) - 1;
	save_ddi_data2ddr_partition((uint8*)&number_of_freq, sizeof(number_of_freq), *output_iter);
	*output_iter += sizeof(number_of_freq);
	
	ecdt_output = (ddi_out_ecdt_settings *)buffer;
#ifdef DDI_DEBUG_PRINT_ENABLE
	snprintf(ddi_log_string, sizeof(ddi_log_string), "buf 0x%X , &data 0x%X, data 0x%X", buffer, &(ecdt_output->settings_buf), ecdt_output->settings_buf);
	boot_log_message(ddi_log_string);
#endif //DDI_DEBUG_PRINT_ENABLE
	for (freq_plan_index = 1;freq_plan_index < (sizeof(ddrsns_share_data->aop_params.mc_states)/sizeof(ddrsns_share_data->aop_params.mc_states[0]));freq_plan_index++)
	{
		ecdt_output->freq_khz = ddrsns_share_data->shrm_ddr_ptr->ddr_runtime.clock_plan[freq_plan_index].clk_freq_in_khz;
		ret = HAL_DDR_Read_eCDT_Parameter_Values(ddrsns_share_data, ddrsns_share_data->shrm_ddr_ptr->ddr_runtime.clock_plan[freq_plan_index].clk_freq_in_khz, &(ecdt_output->settings_buf), buffer_size-sizeof(ddi_out_ecdt_settings));
		
#ifdef DDI_DEBUG_PRINT_ENABLE
		snprintf(ddi_log_string, sizeof(ddi_log_string), "read %d bytes", ret);
		boot_log_message(ddi_log_string);
		
		int i;
		uint8* temp = (uint8*)&(ecdt_output->settings_buf);
		for (i=0;i<ret;i+=2)
		{
			snprintf(ddi_log_string, sizeof(ddi_log_string), "setting %d = 0x%X ", *(temp+i), *(temp+i+1));
			boot_log_message(ddi_log_string);
		}
#endif //DDI_DEBUG_PRINT_ENABLE
		
		if ( (ret < 0) || (ret > buffer_size) ) {
			output_header.test_status = DDI_FAIL;
			break;
		}
		ecdt_output->number_settings = (uint8)ret/2;
#ifdef DDI_DEBUG_PRINT_ENABLE
		snprintf(ddi_log_string, sizeof(ddi_log_string), "n settings %d ", ecdt_output->number_settings);
		boot_log_message(ddi_log_string);
#endif //DDI_DEBUG_PRINT_ENABLE
		save_ddi_data2ddr_partition((uint8*)buffer, sizeof(ddi_out_ecdt_settings)- sizeof(ecdt_output->settings_buf) + ret, *output_iter);
		*output_iter += sizeof(ddi_out_ecdt_settings)- sizeof(ecdt_output->settings_buf) + ret;
	}

  }while(0);
  
  if (buffer != NULL){
	  ddi_free(buffer);
	  buffer = NULL;
  }
  
  save_ddi_data2ddr_partition((uint8*)&output_header, sizeof(ddi_output_header), header_offset);
}

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
void ddi_run_command_delay(uint64 cmd_id, void* params, uint64* input_iter, uint64* output_iter)
{
  ddi_delay_params* cparams = (ddi_delay_params*)params;
  *input_iter += sizeof(ddi_delay_params);
  #if DDI_PRINT_ENABLE
  snprintf(ddi_log_string, sizeof(ddi_log_string), "running delay command");
  boot_log_message(ddi_log_string);
  snprintf(ddi_log_string, sizeof(ddi_log_string), "delay_ms: %d", cparams->delay_ms);
  boot_log_message(ddi_log_string);
  #endif

  ddi_delay(cparams->delay_ms * 1000);

  ((ddi_output_header*)ddi_int_out)->cmd_id = cmd_id;
  ((ddi_output_header*)ddi_int_out)->test_status = DDI_CMD_COMPLETE;
  save_ddi_data2ddr_partition(ddi_int_out, sizeof(ddi_output_header), *output_iter);
  *output_iter += sizeof(ddi_output_header);
}

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
void ddi_run_command_rd(uint64 cmd_id, void* params, uint64* input_iter, uint64* output_iter)
{
  ddi_rd_params* cparams = (ddi_rd_params*)params;
  *input_iter += sizeof(ddi_rd_params);
  ddi_error ret = DDI_CMD_COMPLETE;
  uint64* start = (uint64*)cparams->start_addr;
  uint64* end = (uint64*)cparams->end_addr;
  uint64 repeat_count = cparams->repeat_count;
  
  /*Unlock the DDR region locked(XPU protected) by XBL_SEC*/
  ddr_unlock_xpu_protected_mem_region();

  #if DDI_PRINT_ENABLE
  snprintf(ddi_log_string, sizeof(ddi_log_string), "running read command");
  boot_log_message(ddi_log_string);
  #endif
  ret |= get_start_end((uint64*)&start, (uint64*)&end);

  #if DDI_PRINT_ENABLE
  snprintf(ddi_log_string, sizeof(ddi_log_string), "repeat_count: %d", repeat_count);
  boot_log_message(ddi_log_string);
  #endif
  for (uint64 i = 0; i < repeat_count || repeat_count == 0; i++) {
    ret |= ddi_mem_read(start, end, FALSE, 0, NULL, 0);
  }
  ((ddi_output_header*)ddi_int_out)->cmd_id = cmd_id;
  ((ddi_output_header*)ddi_int_out)->test_status = ret;
  save_ddi_data2ddr_partition(ddi_int_out, sizeof(ddi_output_header), *output_iter);
  *output_iter += sizeof(ddi_output_header);
}

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
void ddi_run_command_wr(uint64 cmd_id, void* params, uint64* input_iter, uint64* output_iter)
{
  ddi_wr_params* cparams = (ddi_wr_params*)params;
  *input_iter += sizeof(ddi_wr_params);
  ddi_error ret = DDI_CMD_COMPLETE;
  uint64* start = (uint64*)cparams->start_addr;
  uint64* end = (uint64*)cparams->end_addr;
  uint64 repeat_count = cparams->repeat_count;
  uint8 invert_row = (cparams->flags & DDI_WR_FLAGS_MASK_INVERT_ROW) > DDI_WR_FLAGS_SHIFT_INVERT_ROW;
  uint8 random = (cparams->flags & DDI_WR_FLAGS_MASK_RANDOM) > DDI_WR_FLAGS_SHIFT_RANDOM;
  uint32 random_seed = cparams->random_seed;
  uint64 random_stride = cparams->random_stride;
  uint64 pattern_size = cparams->pattern_size;
  uint64* pattern;
  *input_iter += pattern_size*sizeof(uint64);
  
  pattern = ddi_malloc(pattern_size*sizeof(uint64));
  if (pattern != NULL)
  {
	  qmemscpy(pattern, pattern_size*sizeof(uint64), ((uint8*)cparams + sizeof(ddi_wr_params)), pattern_size*sizeof(uint64));
  }
  else
  {
	  boot_log_message("failed to malloc");
	  pattern = (uint64*)((uint8*)cparams + sizeof(ddi_wr_params));
  }
  
  /*Unlock the DDR region locked(XPU protected) by XBL_SEC*/
  ddr_unlock_xpu_protected_mem_region();
  
  #if DDI_PRINT_ENABLE
  snprintf(ddi_log_string, sizeof(ddi_log_string), "running write command");
  boot_log_message(ddi_log_string);
  snprintf(ddi_log_string, sizeof(ddi_log_string), "repeat_count: %d", repeat_count);
  boot_log_message(ddi_log_string);
  snprintf(ddi_log_string, sizeof(ddi_log_string), "random: %d", random);
  boot_log_message(ddi_log_string);
  #endif
  ret |= get_start_end((uint64*)&start, (uint64*)&end);

  if (random) {
    invert_row = FALSE;
    if (!random_seed) {
      // TODO: get a dynamic random seed, using timestamp?
      random_seed = swsys_rand_uefi_32();
    }
    swsys_srand_uefi(random_seed);

    #if DDI_PRINT_ENABLE  
    snprintf(ddi_log_string, sizeof(ddi_log_string), "random_stride: %d", random_stride);
    boot_log_message(ddi_log_string);
    snprintf(ddi_log_string, sizeof(ddi_log_string), "random_seed: 0x%08x", random_seed);
    boot_log_message(ddi_log_string);
    #endif
  } else {
    #if DDI_PRINT_ENABLE  
    snprintf(ddi_log_string, sizeof(ddi_log_string), "pattern_size: %d", pattern_size);
    boot_log_message(ddi_log_string);
	for (uint64 i=0;i<pattern_size;i++)
	{
		snprintf(ddi_log_string, sizeof(ddi_log_string), "pattern: 0x%016llx", (uint64)pattern[i]);
		boot_log_message(ddi_log_string);
	}
    #endif

    if (pattern_size != 1)
      invert_row = FALSE;
  }
  #if DDI_PRINT_ENABLE
  snprintf(ddi_log_string, sizeof(ddi_log_string), "invert_row: %d", invert_row);
  boot_log_message(ddi_log_string);
  #endif

  for (uint64 i = 0; i < repeat_count || repeat_count == 0; i++) {
    ret |= ddi_mem_set(start, end, invert_row, random, random_stride, pattern, pattern_size);
  }
  
  if ( (pattern != NULL) && (pattern != ((uint64*)((uint8*)cparams + sizeof(ddi_wr_params)))) ) 
  {
	  ddi_free(pattern);
  }
  
  ((ddi_output_header*)ddi_int_out)->cmd_id = cmd_id;
  ((ddi_output_header*)ddi_int_out)->test_status = ret;
  save_ddi_data2ddr_partition(ddi_int_out, sizeof(ddi_output_header), *output_iter);
  *output_iter += sizeof(ddi_output_header);
}

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
void ddi_add_fail_summary (ddi_fail_summary* fail_summary, uint64* addr)
{
  uint32 instance;
  fail_summary->total_fail_count++;
  instance = ddi_get_rank(addr);
  if (instance < fail_summary->num_ranks) {
    fail_summary->rank[instance]++;
  }
  instance = ddi_get_channel(addr);
  if (instance < fail_summary->num_channels) {
    fail_summary->channel[instance]++;
  }
}

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
boolean ddi_handle_fail_rdwr(ddi_handle_fail_params* fail_params, uint64* addr, uint64 expected, uint64 read, uint64* output_iter)
{
  ddi_fail_summary* fail_summary = fail_params->fail_summary;
  uint32 max_fails = fail_params->max_fails;
  ddi_add_fail_summary(fail_summary, addr);
  uint64 fail_count = fail_summary->total_fail_count;

  #if DDI_PRINT_ENABLE
  snprintf(ddi_log_string, sizeof(ddi_log_string), "failure:");
  boot_log_message(ddi_log_string);
  snprintf(ddi_log_string, sizeof(ddi_log_string), "  A:0x%08x%08x", (uint64)addr>>32, (uint64)addr&0xFFFFFFFF);
  boot_log_message(ddi_log_string);
  snprintf(ddi_log_string, sizeof(ddi_log_string), "  E:0x%08x%08x", (uint64)expected>>32, (uint64)expected&0xFFFFFFFF);
  boot_log_message(ddi_log_string);
  snprintf(ddi_log_string, sizeof(ddi_log_string), "  D:0x%08x%08x", (uint64)read>>32, (uint64)read&0xFFFFFFFF);
  boot_log_message(ddi_log_string);
  #endif

  if (fail_count > max_fails) {
    return TRUE;
  }
  fail_summary->num_fail_records++;
  ddi_fail_addr* fail_addr = (ddi_fail_addr*)ddi_int_out;
  fail_addr->fail_addr = (uint64)addr;
  fail_addr->expected_data = expected;
  fail_addr->read_data = read;
  save_ddi_data2ddr_partition(ddi_int_out, sizeof(ddi_fail_addr), *output_iter);
  *output_iter += sizeof(ddi_fail_addr);

  return FALSE;
}

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
void ddi_run_command_rdwr(uint64 cmd_id, void* params, uint64* input_iter, uint64* output_iter)
{
  ddi_rdwr_params* cparams = (ddi_rdwr_params*)params;
  *input_iter += sizeof(ddi_rdwr_params);
  ddi_error ret = DDI_CMD_COMPLETE;
  uint64* start = (uint64*)cparams->start_addr;
  uint64* end = (uint64*)cparams->end_addr;
  uint64 repeat_count = cparams->repeat_count;
  uint8 random = (cparams->flags & DDI_RDWR_FLAGS_MASK_RANDOM) >> DDI_RDWR_FLAGS_SHIFT_RANDOM;
  uint8 verify = (cparams->flags & DDI_RDWR_FLAGS_MASK_VERIFY) >> DDI_RDWR_FLAGS_SHIFT_VERIFY;
  uint8 invert_row = (cparams->flags & DDI_RDWR_FLAGS_MASK_INVERT_ROW) >> DDI_RDWR_FLAGS_SHIFT_INVERT_ROW;
  uint32 random_seed = cparams->random_seed;
  uint64 random_stride = cparams->random_stride;
  uint64 pattern_size = cparams->pattern_size;
  uint64* pattern;
  *input_iter += pattern_size*sizeof(uint64);
  
  pattern = ddi_malloc(pattern_size*sizeof(uint64));
  if (pattern != NULL)
  {
	  qmemscpy(pattern, pattern_size*sizeof(uint64), ((uint8*)cparams + sizeof(ddi_rdwr_params)), pattern_size*sizeof(uint64));
  }
  else
  {
	  boot_log_message("failed to malloc");
	  pattern = (uint64*)((uint8*)cparams + sizeof(ddi_rdwr_params));
  }
  
  /*Unlock the DDR region locked(XPU protected) by XBL_SEC*/
  ddr_unlock_xpu_protected_mem_region();
  
  #if DDI_PRINT_ENABLE
  snprintf(ddi_log_string, sizeof(ddi_log_string), "running read/write command");
  boot_log_message(ddi_log_string);
  snprintf(ddi_log_string, sizeof(ddi_log_string), "repeat_count: %d", repeat_count);
  boot_log_message(ddi_log_string);
  snprintf(ddi_log_string, sizeof(ddi_log_string), "verify: %d", verify);
  boot_log_message(ddi_log_string);
  snprintf(ddi_log_string, sizeof(ddi_log_string), "random: %d", random);
  boot_log_message(ddi_log_string);
  #endif

  ret |= get_start_end((uint64*)&start, (uint64*)&end);
  if (random) {
    invert_row = FALSE;
    if (!random_seed) {
      // TODO: get a dynamic random seed, using timestamp?
      random_seed = swsys_rand_uefi_32();
    }
    #if DDI_PRINT_ENABLE  
    snprintf(ddi_log_string, sizeof(ddi_log_string), "random_stride: %d", random_stride);
    boot_log_message(ddi_log_string);
    snprintf(ddi_log_string, sizeof(ddi_log_string), "random_seed: 0x%08x", random_seed);
    boot_log_message(ddi_log_string);
    #endif
  } else {
    #if DDI_PRINT_ENABLE  
    snprintf(ddi_log_string, sizeof(ddi_log_string), "pattern_size: %d", pattern_size);
    boot_log_message(ddi_log_string);
	for (uint64 i=0;i<pattern_size;i++)
	{
		snprintf(ddi_log_string, sizeof(ddi_log_string), "pattern: 0x%016llx", (uint64)pattern[i]);
		boot_log_message(ddi_log_string);
	}
    #endif

    if (pattern_size != 1)
      invert_row = FALSE;
  }
  #if DDI_PRINT_ENABLE
  snprintf(ddi_log_string, sizeof(ddi_log_string), "invert_row: %d", invert_row);
  boot_log_message(ddi_log_string);
  #endif

  // increment the output_iter early and record output location so we can log stats back to the header
  ddi_rdwr_output rdwr_output;
  qmemset(&rdwr_output, 0, sizeof(ddi_rdwr_output));
  rdwr_output.fail_summary.num_ranks = sizeof(rdwr_output.fail_summary.rank)/sizeof(rdwr_output.fail_summary.rank[0]);
  rdwr_output.fail_summary.num_channels = sizeof(rdwr_output.fail_summary.channel)/sizeof(rdwr_output.fail_summary.channel[0]);
  uint64 rdwr_output_offset = *output_iter;
  *output_iter += sizeof(ddi_output_header)+sizeof(ddi_rdwr_output);

  //swsys_srand_uefi(random_seed);
  for (uint64 i = 0; i < repeat_count || repeat_count == 0; i++) {
    swsys_srand_uefi(random_seed);
    ret |= ddi_mem_set(start, end, invert_row, random, random_stride, pattern, pattern_size);
    if (verify) {
      // set the rdwr_output header
      ddi_handle_fail_params fail_params;
      fail_params.fail_summary = &rdwr_output.fail_summary;
      fail_params.max_fails = (MAX_DDI_OUTPUT_SIZE-(*output_iter))/sizeof(ddi_fail_addr);
      ddi_cmp_params cmp_params;
      cmp_params.handle_fail = ddi_handle_fail_rdwr;
      cmp_params.handle_fail_params = &fail_params;
      cmp_params.output_iter = output_iter;
      swsys_srand_uefi(random_seed);
      ret |= ddi_mem_cmp(start, end, invert_row, random, random_stride, pattern, pattern_size, &cmp_params);
    }
    else {
      ret |= ddi_mem_read(start, end, random, random_stride, pattern, pattern_size);
    }
    random_seed = swsys_rand_uefi_64();
  }
  ddi_output_header output_header;
  output_header.cmd_id = cmd_id;
  output_header.test_status = ret;
  save_ddi_data2ddr_partition((uint8*)&output_header, sizeof(ddi_output_header), rdwr_output_offset);
  save_ddi_data2ddr_partition((uint8*)&rdwr_output, sizeof(ddi_rdwr_output), rdwr_output_offset+sizeof(ddi_output_header));

  #if DDI_PRINT_ENABLE
  snprintf(ddi_log_string, sizeof(ddi_log_string), "total_fail_count: %d", rdwr_output.fail_summary.total_fail_count);
  boot_log_message(ddi_log_string);
  #endif

  if ( (pattern != NULL) && (pattern != ((uint64*)((uint8*)cparams + sizeof(ddi_rdwr_params)))) )
  {
	  ddi_free(pattern);
  }
  
  if (rdwr_output.fail_summary.total_fail_count) {
    boot_dload_transition_pbl_forced_dload();
    volatile int wait = 1;
    while (wait);
  }
}

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
void ddi_run_command_freq(uint64 cmd_id, void* params, uint64* input_iter, uint64* output_iter)
{
  ddi_freq_params* cparams = (ddi_freq_params*)params;
  *input_iter += sizeof(ddi_freq_params);
  #if DDI_PRINT_ENABLE
  snprintf(ddi_log_string, sizeof(ddi_log_string), "running frequency command");
  boot_log_message(ddi_log_string);
  snprintf(ddi_log_string, sizeof(ddi_log_string), "freq: %d", cparams->freq_khz);
  boot_log_message(ddi_log_string);
  #endif

  ddr_external_set_clk_speed(cparams->freq_khz);
  
  ddi_output_header output_header;
  output_header.cmd_id = cmd_id;
  output_header.test_status = DDI_CMD_COMPLETE;
  save_ddi_data2ddr_partition((uint8*)&output_header, sizeof(ddi_output_header), *output_iter);
  *output_iter += sizeof(ddi_output_header);
}

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
void ddi_run_command_plot(uint64 cmd_id, void* params, uint64* input_iter, uint64* output_iter)
{
  int8 freq_plan_index;
  ddi_plot_params* cparams = (ddi_plot_params*)params;
  *input_iter += sizeof(ddi_plot_params);
  #if DDI_PRINT_ENABLE
  snprintf(ddi_log_string, sizeof(ddi_log_string), "running plot command");
  boot_log_message(ddi_log_string);
  snprintf(ddi_log_string, sizeof(ddi_log_string), "type: %d", cparams->type);
  boot_log_message(ddi_log_string);
  snprintf(ddi_log_string, sizeof(ddi_log_string), "freq: %d", cparams->freq_khz);
  boot_log_message(ddi_log_string);
  #endif

  uint64 header_offset = *output_iter;
  *output_iter += sizeof(ddi_output_header);
  ddi_output_header output_header;
  output_header.cmd_id = cmd_id;
  output_header.test_status = DDI_CMD_COMPLETE;

  freq_plan_index = ddi_get_frequency_plan_index(cparams->freq_khz);
  do 
  {
  if (freq_plan_index < 0)
  {
	#if DDI_PRINT_ENABLE
	boot_log_message("DDI: invalid frequency option\r\n");
	#endif
	output_header.test_status = DDI_INVALID_INPUT;
	break;
  }
  
  if (cparams->type == DDI_PLOT_WR)
    HAL_DDR_wr_dqdqs_coarse_fine_plot_pivot(ddrsns_share_data, cparams->freq_khz, output_iter);
  else if (cparams->type == DDI_PLOT_RD)
    HAL_DDR_rd_dqdqs_coarse_fine_plot_pivot(ddrsns_share_data, cparams->freq_khz, output_iter);
  else
    output_header.test_status = DDI_INVALID_INPUT;
  } while(0);
  
  save_ddi_data2ddr_partition((uint8*)&output_header, sizeof(ddi_output_header), header_offset);
  *output_iter += sizeof(ddi_output_header);
}
