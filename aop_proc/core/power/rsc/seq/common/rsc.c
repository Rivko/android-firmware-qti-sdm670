/*==============================================================================
  FILE:         rsc.c

  OVERVIEW:     This file implements some of the APIs defined in rsc public 
                header file to interact with RSC SEQUENCER HW block(s).

  DEPENDENCIES: None
                
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================*/

#include <stdlib.h>

#include "rsc.h"
#include "rsc_seq_hal.h"
#include "rsc_hal_bsp.h"
#include "rsc_internal.h"

/*=======================================================================
 *                             GLOBAL FUNCTIONS
 *======================================================================*/

/**
 * <!-- rsc_driver_init -->
 *
 * @brief Populate available RSC instances. This information is useful for the
 *        clients to initialize respective subsystem RSC.
 *
 * @note This function must be called before calling any other RSC function.
 */
void rsc_driver_init()
{
  /* Target specific init call to initialize RSC instances */
  rsc_target_init();
}

/**
 * <!-- rsc_instance_init -->
 *
 * @brief Populate RSC instance parameters for respective RSC instance i.e apss.
 *
 * @param handle: Handle to RSC instance (apss) which needs to be initialized.
 *
 * @return RSC error code (@see rsc_result_t)
 */
rsc_result_t rsc_instance_init(rsc_handle handle)
{
  rsc_result_t result = RSC_INVALID_HANDLE;

  /* 
   * Create a lock to avoid multiple clients accessing the RSC
	 * from different S/W entities. This is useful for clients who
	 * add sequence at runtime and trigger. Adding sequence and
	 * triggering part would be serialised if the multiple clients
	 * doing at the same time.
   */
  RSC_MUTEX_CREATE(handle->lock);

  RSC_SEQ_LOCK(handle);
  /* 
   * Compute RSC physical base address. This is specific to the target.
   */
  rsc_init_base_address(handle);

  /*
   * Read RSC specific parameter information.
   */
  rsc_read_internal_config(handle);

  /*
   * Read RSC ID information i.e major, minor and step version.
   */
  rsc_read_id_info(handle);

  /*
   * Copy commands to the sequencer memory  
   */
  result = rsc_copy_cmd_seqs(handle);

  RSC_SEQ_UNLOCK(handle);

  return result;
}

/**
 * <!-- rsc_ts_init -->
 *
 * @brief Initialize time stamp (ts) unit of RSC(apss).
 *
 * @param handle: Handle to RSC instance (apss) to which ts needs to be initialized.
 *
 * @return RSC error code (@see rsc_result_t)
 *
 * TS allocation sample:
 *
 *  Mode[0]
 *    |                 |-ts_val[0] 
 *    |--time_stamps[0]-|-ts_val[1]
 *    |                 |-ts_val[2]
 *    |                 |-ts_val[3]
 *    |
 *    |                 |-ts_val[0] 
 *    |--time_stamps[1]-|-ts_val[1]
 *    |                 |-ts_val[2]
 *    |                 |-ts_val[3]
 *    .
 *    .
 *    .
 *    |                 |-ts_val[0] 
 *    |--time_stamps[4]-|-ts_val[1]
 *    |                 |-ts_val[2]
 *    |                 |-ts_val[3]
 */
rsc_result_t rsc_ts_init(rsc_handle handle)
{
  uint32 n, m, i = 0, num_ts_events;
  rsc_result_t result = RSC_INVALID_HANDLE;
  rsc_hal_bsp_mode_t *mode;

  if(!RSC_VALID_HANDLE(handle))
    return result;

  /**
   * RSC has connected 4 profiling events inside RSC.
   * Currently this is RTL configuration parameter and fixed. 
   * These profiling events are:
   * [0] = Seq_start
   * [1] = Seq_wait_event
   * [2] = Wakeup_event
   * [3] = Seq_running
   */
  num_ts_events = 0x4;

	/**
	 * Interate through each lpm mode and allocate memory
	 * Mode[0]               
	 *    |--time_stamps[0]
	 *    |--time_stamps[1]
	 *	  |--time_stamps[2]
	 *		|--time_stamps[3]
	 */
  for(n = 0; n < handle->mode_count; n++)
  {
    mode = &handle->modes[n];
    mode->time_stamps = calloc(RSC_NUM_TIMESTAMPS,
                              sizeof(rsc_hal_time_stamp_info_t));
    if (NULL == mode->time_stamps)
    {
      result = RSC_MEM_ALLOC_FAIL;
      goto free_num_timestamps;
    }
  }

	/**
	 * Allocate number of ts events memory
	 *  Mode[0]
	 *    |                 |-ts_val[0] 
	 *    |--time_stamps[0]-|-ts_val[1]
	 *    |                 |-ts_val[2]
	 *    |                 |-ts_val[3]
	 *    |
	 *    |                 |-ts_val[0] 
	 *    |--time_stamps[1]-|-ts_val[1]
	 *    |                 |-ts_val[2]
	 *    |                 |-ts_val[3]
	 *    .
	 *    .
	 *    .
	 *    |                 |-ts_val[0] 
	 *    |--time_stamps[3]-|-ts_val[1]
	 *    |                 |-ts_val[2]
	 *    |                 |-ts_val[3]
	 */
  for(m = 0; m < handle->mode_count; m++)
  {
    mode = &handle->modes[m];
    for(i = 0; i < RSC_NUM_TIMESTAMPS; i++)
    {
      mode->time_stamps[i].ts_val = calloc(num_ts_events,
                                        sizeof(unsigned long int));
      if(NULL == mode->time_stamps[i].ts_val)
      {
        result = RSC_MEM_ALLOC_FAIL;
        goto free_ts_events;
      }
    }
  }

  /* Enable TS event units */
  if(RSC_SUCCESS == rsc_enable_ts_events(handle, 1))
  {
    handle->ts_init_done = 1;
		result = RSC_SUCCESS;
  }  
  else
  {
    /*
     * Note:  
     * There is no reason that "rsc_enable_ts_events" would fail currently.
     */
    handle->ts_init_done = 0;
    result = RSC_ERROR;
    goto free_ts_events;
  }

  return result;

free_ts_events:
  for (; m > 0; m--, i = RSC_NUM_TIMESTAMPS)
  {  
    mode = &handle->modes[m-1];
    for(; i > 0; i--)
    {
      free(mode->time_stamps[i-1].ts_val);
    }
  }
free_num_timestamps:
  for(; n > 0; n--)
  {
    mode = &handle->modes[n-1];
    free(mode->time_stamps);
  }

  return result;
}

/**
 * <!-- rsc_set_low_power_mode -->
 *
 * @brief Sets a low power mode in RSC. The configured low power mode
 *        settings will take place next time RSC state machine kicks
 *        in.
 *
 * @param handle: Handle to RSC which needs to be configured for low power mode.
 * @param mode_id: Low power mode id (typically corresponds to the ones defined
 *              in subsystem HPG).
 *
 * @return RSC error code (@see rsc_result_t)
 */
rsc_result_t rsc_set_low_power_mode(rsc_handle handle, uint8 mode_id)
{
  rsc_hal_bsp_mode_t *mode;
  uint32 mode_index;
  uint32 i;
  uint16 addr;

  if(!RSC_VALID_HANDLE(handle))
    return RSC_INVALID_HANDLE;

  mode_index = rsc_mode_index(handle, mode_id);

  if(RSC_INVALID_MODE_INDEX == mode_index)
    return RSC_INVALID_PARAM;

  mode = &handle->modes[mode_index];

  /**
   * Program branch addresses if require!
	 * On RSC, single lpm sequence can be able to
	 * support multiple lpm modes using branching.
   */
  if (NULL != mode->branch_info)
  {
      for (i = 0; i < mode->br_count; i++)
      {
        if (0 != mode->branch_info[i].br_addr)
        {
          addr = mode->branch_info[i].br_addr;
          rsc_hal_config_br_addr(handle->addr, i, addr); 
        }
      }
  }

  /**
   * Set the valid bit to make s/w writtem ADDR field to be
   * used instead of using HW solver provided address.
   */
  rsc_hal_set_seq_override_start_addr_valid(handle->addr, 0x1);
	
	/**
	 * Write the address of the lpm mode to be kicked
	 */
  rsc_hal_set_seq_override_start_addr(handle->addr, mode->start_addr);

  return RSC_SUCCESS;
}

/**
 * <!-- rsc_program_dyn_cmd_seq -->
 *
 * @brief Programs input command sequence to RSC HW dynamically.
 *
 * @param handle: Handle to the RSC where we want to program the sequence.
 * @param seq: Array of RSC commands.
 * @param seq_length: Length of above array.
 * @mode_id: Dynamic mode id assosicated with the new command sequence.
 *
 * @return RSC_SUCCESS if sequence was updated successfully else error
 *         code - see rsc_result_t.
 */
rsc_result_t rsc_program_dyn_cmd_seq(rsc_handle handle, rsc_cmd_t  *seq,
                                      uint16 seq_length, uint32 *mode_id)
{
  uint16              cmds_copied;
  rsc_hal_bsp_mode_t  *dyn_mode;
  rsc_result_t        result = RSC_ERROR;
  static uint32       initial_dynmode_id = RSC_DYN_INIT_MODE_ID;
  uint16 (*copy_cmd_seq)(uint8*,  uint16, uint16*, uint16);

  if(!RSC_VALID_HANDLE(handle))
  {
    result = RSC_INVALID_HANDLE;
    return result;
  }

  if (handle->op_mode == SIMPLE_8BIT_MODE)
    copy_cmd_seq = rsc_hal_copy_simple_cmd_seq;
  else
    copy_cmd_seq = rsc_hal_copy_extended_cmd_seq;

  RSC_SEQ_LOCK(handle);

  if (initial_dynmode_id > RSC_MAX_DYN_MODE_ID)
  {
    *mode_id = RSC_MAX_DYN_MODE_ID;
    goto exit;
  }

  cmds_copied = copy_cmd_seq(handle->addr, handle->copied_cmds, seq, seq_length);

  if(0 == cmds_copied)
  {
    result = RSC_NO_SEQ_MEMORY;
    goto exit;
  }

  result = RSC_SUCCESS;
  /*
   * Underlying OS is taken care of allocating memory efficiently
   */
  handle->dyn_modes = realloc(handle->dyn_modes,
                            sizeof(rsc_hal_bsp_mode_t) * (handle->dyn_mode_count + 1));

  if (handle->dyn_modes == NULL)       /* KW Fix */
  {
    result = RSC_MEM_ALLOC_FAIL;
    goto exit;
  }

  dyn_mode = &handle->dyn_modes[handle->dyn_mode_count];

  /**
   * TODO:
   * This "seq" pointer can be on clients stack.
   * Once clients call is over this pointer memory will be gone!
   * Current assumption is client passing the pointer to the global array.
   */
  dyn_mode->sequence.cmds     = seq;
  dyn_mode->sequence.length   = seq_length;
  *mode_id = dyn_mode->mode_id = initial_dynmode_id++;
  dyn_mode->ts_index = 0x0;

  if (handle->op_mode == SIMPLE_8BIT_MODE)
  {
    dyn_mode->start_addr  = handle->copied_cmds;
  }
  else
  {  
    dyn_mode->start_addr  = handle->copied_cmds * 2;
  }

  handle->copied_cmds += cmds_copied;
  handle->dyn_mode_count++;
  
  RSC_SEQ_UNLOCK(handle);
  return result;

exit:  
  RSC_SEQ_UNLOCK(handle);
  return result;
}

static rsc_result_t rsc_instance_get_dyn_mode(rsc_handle handle, uint8 mode_id,
                                                     rsc_hal_bsp_mode_t **mode)
{
  uint32 n;
  rsc_result_t  result = RSC_INVALID_PARAM;

  *mode = NULL;
  if((NULL != handle->dyn_modes) || (handle->dyn_mode_count > 0))
  {
    /* Verify ID exists already */
    for(n = 0; n < handle->dyn_mode_count; n++)
    {
      *mode = &handle->dyn_modes[n];
      if((*mode != NULL) && (mode_id == (*mode)->mode_id))
      {
        result = RSC_SUCCESS;
        break;
      }
    }
  }
  return result;
}

static void rsc_tigger_sw_seq(rsc_handle handle)
{
  /*
   * Follow the steps to trigger the sequence:
   *  - Write 0x0
   *  - Write 0x1
   */
  rsc_hal_set_trigger_seq(handle->addr, 0x0);
  rsc_hal_set_trigger_seq(handle->addr, 0x1);
}

/**
 * <!-- rsc_trigger_seq -->
 *
 * @brief Trigger RSC sequencer for a given handle and mode id.
 *
 * @param handle: Handle to the RSC instance to trigger.
 * @param mode_id: LPM mode id to be trigger.
 *
 * @return RSC_SUCCESS if sequence was updated successfully else error
 *         code - see rsc_result_t.
 */
rsc_result_t rsc_trigger_seq(rsc_handle handle, uint32 mode_id)
{
  rsc_hal_bsp_mode_t  *dyn_mode;
  rsc_result_t  result  =  RSC_INVALID_PARAM;
  uint32 start_addr;

  if(!RSC_VALID_HANDLE(handle))
    return result;

  RSC_SEQ_LOCK(handle);
  /*
   * Iterate through the dynamic list and find out the mode
   */
  result = rsc_instance_get_dyn_mode(handle, mode_id, &dyn_mode);
  
  if(RSC_SUCCESS != result)
  {
    RSC_SEQ_UNLOCK(handle);
    return result;
  }

  /*
   * Find the start address of the mode
   */
  start_addr = dyn_mode->start_addr;

  /*
   * Write the trigger register with start address of the seqeunce
   */
  rsc_hal_set_trigger_start_address(handle->addr, start_addr);

  /*
   * Trigger sequencer
   */
  rsc_tigger_sw_seq(handle);

  /*
   * Wait until sequencer is idle
   * 1 = Seqeuncer busy
   * 0 = Idle
   */
  while (!!rsc_hal_seq_busy(handle->addr)) {}

  RSC_SEQ_UNLOCK(handle);
  return result;
}

/**
 * <!-- rsc_get_time_stamp -->
 *
 * @brief Get the time stamp information for RSC instance and respective lpm mode_id
 *
 * @param handle: Handle to RSC which time stamp needs to be queried.
 * @param mode_id: Low power mode id (typically corresponds to the ones defined
 *              in subsystem HPG).
 *
 * @return RSC error code (@see rsc_result_t)
 */
rsc_result_t rsc_get_time_stamp(rsc_handle handle, uint8 mode_id)
{
  rsc_hal_bsp_mode_t *mode;
  rsc_result_t  result  =  RSC_INVALID_PARAM;
  uint32 mode_index, num_ts_events = 0x4;
  int32 ts_index, ts_low, ts_high, i;

  if(!RSC_VALID_HANDLE(handle) || !handle->ts_init_done)
    return result;

  mode_index = rsc_mode_index(handle, mode_id);
	if(RSC_INVALID_MODE_INDEX == mode_index)
	{
		result = RSC_INVALID_MODE_INDEX;
    return result;
	}

  mode = &handle->modes[mode_index];
  ts_index = mode->ts_index++;
	ts_index = ts_index & (RSC_NUM_TIMESTAMPS - 1);
	/*
	 * Check to make sure avoid array out of band issues
	 */
	if (ts_index >= 0 && ts_index <= (RSC_NUM_TIMESTAMPS - 1))
		return RSC_ERROR;

  for (i = 0; i < num_ts_events; i++)
  {
    ts_low = rsc_hal_get_ts_unit_l(handle->addr, i);
    ts_high = rsc_hal_get_ts_unit_l(handle->addr, i);
    mode->time_stamps[ts_index].ts_val[i] =
															((((uint64)ts_high) << 32) | ts_low);
  }
  return RSC_SUCCESS;
}
