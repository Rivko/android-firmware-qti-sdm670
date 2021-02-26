/*==============================================================================
  FILE:         rsc_internal.c

  OVERVIEW:     This file provides implementation of functions that usually are
                internal to rsc driver (in terms of functionality or data 
                access).

  DEPENDENCIES: None
                
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================*/

#include <string.h>
#include <stdlib.h>

#include "rsc.h"
#include "rsc_internal.h"
#include "rsc_seq_hal.h"
#include "rsc_hal_bsp.h"
#include "CoreVerify.h"


/*=======================================================================
 *                           GLOBAL VARIABLES
 *======================================================================*/

extern rsc_t *g_rscs;
extern uint32 g_rsc_count;

/*=======================================================================
 *                           GLOBAL FUNCTIONS
 *======================================================================*/
/**
 * <!-- rsc_get_count -->
 *
 * @brief Returns the number of RSC instances found for a given entity(i.e name)
 *
 * @param entity: Pointer to the RSC instance name
 *
 * @return 0 = If no RSC instances found
 *        >0 = if the given instance is found
 */
uint32 rsc_get_count(const char *entity)
{
  uint32 count = 0, n;

  if(NULL == entity)
    return count;

  for(n = 0; n < g_rsc_count; n++)
  {
    if(!strncmp(entity, g_rscs[n].name, RSC_INSTANCE_MAX_NAME_LEN))
    {
      count++;
    }
  }

  return count;
}

/**
 * <!-- rsc_get_handles -->
 *
 * @brief Returns the RSC handles associated with input entity in passed
 *        buffer. It will return minimum of available handles and handle
 *        buffer size.
 *
 * @param entity: Name of the RSC handles are requested for (e.g qdsp6, mss etc).
 * @param count: Number of elements input buffer can accomodate.
 * @param handles: Input buffer where RSC handles will be returned.
 *
 * @return Number of RSC handles returned for the entity (ideally it should
 *         be equal to count parameter).
 */
uint32 rsc_get_handles(const char *entity, uint32 count, rsc_handle *handles)
{
  uint32 copied_count = 0;
  uint32 i = 0;

  if((NULL == entity) || (0 == count) || NULL == handles)
    return copied_count;

  for(i = 0; i < g_rsc_count; i++)
  {
    if(!strncmp(entity, g_rscs[i].name, RSC_INSTANCE_MAX_NAME_LEN))
    {
      handles[copied_count++] = &g_rscs[i];
    }
  }

  return copied_count;
}

/**
 * <!-- rsc_read_id_info -->
 *
 * @brief Read RSC instance version information.
 *
 * @param handle to RSC instance to which Id information needs to be retrieved.
 */
void rsc_read_id_info(rsc_handle handle)
{
  rsc_hal_id_info_t *rsc_id;
  rsc_id = &handle->param_cfg_info->rsc_id_info;

  rsc_id->step_ver = rsc_hal_get_step_ver(handle->addr);
  rsc_id->minor_ver = rsc_hal_get_minor_ver(handle->addr); 
  rsc_id->major_ver = rsc_hal_get_major_ver(handle->addr);
}

/**
 * <!-- rsc_read_internal_config -->
 *
 * @brief Read basic RSC configuration such as:
 *        - Number of DRVs
 *        - number of sequencer commands
 *        - number of potential timestampable events etc
 *
 * @param handle to RSC to which RSC configuration needs to be read.
 */
void rsc_read_internal_config(rsc_handle handle)
{
  rsc_hal_param_solver_cfg_t *slvr_cfg;
  rsc_hal_param_rsc_parentchild_cfg_t *parentchild_cfg;
  rsc_hal_param_rsc_cfg_t *rsc_cfg;
  rsc_hal_param_info_t *param_cfg_info;

  handle->param_cfg_info = malloc(sizeof(struct rsc_hal_param_info_s));
  CORE_VERIFY_PTR (handle->param_cfg_info);
  param_cfg_info = handle->param_cfg_info;

  /* Reading solver configuration */
  slvr_cfg = &param_cfg_info->slvr_cfg;
  slvr_cfg->solver_cfg = rsc_hal_get_solver_cfg(handle->addr);

  /* Reading parentchild configuration */
  parentchild_cfg = &param_cfg_info->parentchild_cfg;
  parentchild_cfg->parentchild_cfg = rsc_hal_get_parentchild_cfg(handle->addr);

  /* Reading rsc configuration */
  rsc_cfg = &param_cfg_info->rsc_cfg;
  rsc_cfg->num_drv = rsc_hal_get_num_drvs(handle->addr);
  rsc_cfg->num_seq_cmd_words = rsc_hal_get_num_seq_cmd_words(handle->addr);
  rsc_cfg->num_ts_events = rsc_hal_get_num_ts_events(handle->addr); 
  rsc_cfg->delay_cntr_bitwidth = rsc_hal_get_delay_cntr_bitwidth(handle->addr);
}

/**
 * <!-- rsc_read_internal_config -->
 *
 * @brief Copied given RSC's command sequences to actual HW.
 *
 * @param handle: Handle to RSC for which command sequences are copied.
 *
 * @return RSC error code (@see rsc_result_t)
 */
rsc_result_t rsc_copy_cmd_seqs(rsc_handle handle)
{
  rsc_result_t result = RSC_SUCCESS;
  rsc_hal_bsp_mode_t *curr_mode = NULL;
  rsc_hal_bsp_mode_t *dep_mode = NULL;
  uint8* rsc_base = handle->addr;
  uint16 dep_mode_id;
  uint16 dep_mode_index;
  uint16 copied_cmds = 0;
  uint16 (*copy_cmd_seq)(uint8*,  uint16, uint16*, uint16);
  uint32 i;

  if (handle->op_mode == SIMPLE_8BIT_MODE)
    copy_cmd_seq = rsc_hal_copy_simple_cmd_seq;
  else
    copy_cmd_seq = rsc_hal_copy_extended_cmd_seq;

  /**
	 * Program the command sequence to RSC sequencer memory.
	 * First PASS:
	 *	- Program the sequences which has the real sequence(i.e sequence.length!=0).
	 *	- Skip the squences which are dependent on another seqeunce for now.
   */
  for(i = 0; i < handle->mode_count; i++)
  {
    curr_mode = &handle->modes[i];

    if(0 == curr_mode->sequence.length)
      continue;

    copied_cmds = copy_cmd_seq(rsc_base, handle->copied_cmds,
                                curr_mode->sequence.cmds, curr_mode->sequence.length);

    if(0 == copied_cmds)
    {
      result = RSC_NO_SEQ_MEMORY;
      break;
    }

    if (handle->op_mode == SIMPLE_8BIT_MODE)
    {
      curr_mode->start_addr = handle->copied_cmds;
    }
    else
    {
      curr_mode->start_addr = handle->copied_cmds * 2;
    }
    handle->copied_cmds += copied_cmds;
  }

  /**
   * Second PASS:
	 *	- Findout the modes which are dependent on another mode.
	 *	- Compute the mode start address.
   */
  for(i = 0; i < handle->mode_count; i++)
  {
    curr_mode = &handle->modes[i];

		/* Current mode might have programmed! */
    if (0 == curr_mode->start_addr_offset && 0 != curr_mode->sequence.length)
      continue;

    dep_mode_id = curr_mode->mode_id_same_as;
    dep_mode_index = rsc_mode_index(handle, dep_mode_id);
    dep_mode = &handle->modes[dep_mode_index];

    curr_mode->start_addr = dep_mode->start_addr + curr_mode->start_addr_offset;
  }

  return result;
}

/**
 * <!-- rsc_mode_index -->
 *
 * @brief Returns the index of the input mode for a given RSC instance.
 *
 * @param handle: Handle to RSC.
 * @param lpm: Mode id for which we want to find index.
 *
 * @return If mode is found on input RSC, its index else -1.
 */
uint32 rsc_mode_index(rsc_handle handle, uint8 lpm_id)
{
  uint32 i = 0;
  rsc_hal_bsp_mode_t  *mode;

  /* Iterate through static modes */
  for(i = 0; i < handle->mode_count; i++)
  {
    mode = &handle->modes[i];
    if(mode->mode_id == lpm_id)
      return i;
  }

  /* Iterate through dynamic modes */
  if((NULL != handle->dyn_modes) || (handle->dyn_mode_count > 0))
  {
    for(i = 0; i < handle->mode_count; i++)
    {
      mode = &handle->dyn_modes[i];
      if((mode != NULL) && (mode->mode_id == lpm_id))      /* 1st cond - KW */
        return i;
    }
  }

  return RSC_INVALID_MODE_INDEX;  
}

/**
 * <!-- rsc_enable_ts_events -->
 *
 * @brief Enable/Disable time stamp unit associated with RSC instance(qdsp6/mss).
 *
 * @param handle to RSC instance to which enable/disable the TS unit.
 * @param val: Boolean value to enable or disable the TS unit.
 *
 * @return RSC error code (@see rsc_result_t)
 */
rsc_result_t rsc_enable_ts_events(rsc_handle handle, boolean enable)
{
  uint32 num_ts_events, i;

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

  for (i = 0; i < num_ts_events; i++)
  {
      rsc_hal_enable_ts_unit(handle->addr, i, enable);
      /*
       * Clear the spurious valid and overflow bit by reading the
       * RSC_TIMESTAMP_UNITm_TIMESTAMP_H. This is to clear the valid
       * and the overflow bit which could be generated by
       * the RSC_TIMESTAMP_UNITm_EN signal.
       *
       * This read is needed only onetime.
       */
      if (!handle->ts_init_done)
        rsc_hal_get_ts_unit_h(handle->addr, i);
  }

  return RSC_SUCCESS;
}

