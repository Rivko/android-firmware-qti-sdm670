#ifndef _RSC_HAL_BSP_H_
#define _RSC_HAL_BSP_H_

/*==============================================================================
  FILE:         rsc_hal_bsp.h

  OVERVIEW:     This file provides typedefs for the target specific data. Usually
                these types emulate underlying rsc HW block.

  DEPENDENCIES: None
                
                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================*/

#include "rsc_common.h"
#include "HALcomdef.h"

/*========================================================================
 *                                 TYPES
 *=======================================================================*/

/**
 * @brief Type to specify an RSC command sequence for a low power mode 
 */

typedef struct rsc_sequence_data_s
{
  rsc_cmd_t *cmds;  /* Pointer to the command sequence */
  uint16    length; /* Length of above command sequence */
}rsc_sequence_data_t;

/**
 * @brief Type to specify branch address information.
 */
typedef struct rsc_hal_br_info_s
{
  uint16 br_addr;            /* branch address */
}rsc_hal_br_info_t;

/**
 * @brief Type to specify time stamp information of each lpm mode.
 */
typedef struct rsc_hal_time_stamp_info_s
{
  uint64 *ts_val;     /* It is an array of "num_ts_events" */
  uint64 time_slept;  /* ts_val[1] - ts_val[2] */
  uint32 count;       /* How many times this mode is executed succefully */
}rsc_hal_time_stamp_info_t;  

/**
 * @brief Type to specify rsc command sequence for a low power mode.
 */
typedef struct rsc_hal_bsp_mode_s
{
  rsc_sequence_data_t sequence;
  uint16 mode_id;                         /* Mode id - usually comes from HPG */ 
  uint16 start_addr;                      /* Sequence's start address in rsc - filled
                                           * during initialization. */
  /* Applicable if the mode using the branching */
  uint8 start_addr_offset;                /* Applicable only if the sequence is shared with other modes */
  uint8 mode_id_same_as;                  /* Same as any previous mode_id number */
  rsc_hal_br_info_t *branch_info;         /* Branch address information */
  uint16  br_count;
  /* end */

  rsc_hal_time_stamp_info_t *time_stamps; /* Dynamically allocated data to maintain time stamp info */
  uint16 ts_index;
}rsc_hal_bsp_mode_t;

/**
 * @brief Type tp specify RSC solver configuration.
 */
typedef struct rsc_hal_param_solver_cfg_s
{
  uint32 solver_cfg;
}rsc_hal_param_solver_cfg_t;

/**
 * @brief Type to specify RSC internal configuration.
 */
typedef struct rsc_hal_param_rsc_cfg_s
{
  uint8 num_drv;
  uint8 num_seq_cmd_words;
  uint8 num_ts_events;
  uint8 delay_cntr_bitwidth;
}rsc_hal_param_rsc_cfg_t;

/**
 * @brief Type to hold the information of parent child relationship of RSCs
 */
typedef struct rsc_hal_param_rsc_parentchild_cfg_s
{
  uint32 parentchild_cfg;
}rsc_hal_param_rsc_parentchild_cfg_t;

/**
 * @brief Type to hold RSC version information.
 */
typedef struct rsc_hal_id_info_s
{
  uint8 step_ver;
  uint8 minor_ver;
  uint8 major_ver;
}rsc_hal_id_info_t;  

/**
 * @brief Type to hold complete configuration about RSC instance.
 */
typedef struct rsc_hal_param_info_s
{
  rsc_hal_id_info_t rsc_id_info;
  rsc_hal_param_solver_cfg_t slvr_cfg;
  rsc_hal_param_rsc_cfg_t rsc_cfg;
  rsc_hal_param_rsc_parentchild_cfg_t parentchild_cfg;
}rsc_hal_param_info_t;
#endif
