#ifndef __VRM_TYPES_H__
#define __VRM_TYPES_H__

/*! \file vrm_types.h
*
*  \brief This header file contains internal VRM driver related type definitions.
*  \n
*  &copy; Copyright 2016 - 2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/vrm/src/vrm_types.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/16   kt      Initial version
===========================================================================*/

#include "vrm_inc.h"
#include "vrm_config.h"
#include "CoreVerify.h"

//******************************************************************************
// Macros / Definitions / Constants
//******************************************************************************

#define VRM_VERSION(major, minor, step)  (((major) << 16) | ((minor) << 8) | (step))

/**
 * @brief VRM log size.
 */
#define VRM_MAX_NUM_LOGS 10
#define VRM_MAX_NUM_ERR_LOGS 2
#define VRM_MAX_IRQ_ARR_SIZE 3

/**
 * @brief Supported VRM log events.
 */
typedef enum 
{
   VRM_EVENT_UNKNOWN,
   VRM_EVENT_INIT,
   VRM_EVENT_ASSIGN_RSRC_ID,
   VRM_EVENT_ADD_RSRC,
   VRM_EVENT_ADD_RSRC_TYPE,
   VRM_EVENT_POST_INIT,
   VRM_EVENT_ARS_READ,
   VRM_EVENT_WRS_READ,
   VRM_EVENT_CRS_READ,
   VRM_EVENT_CMDSYN_UNBLOCK,
   VRM_EVENT_SEND_VOTE,
   VRM_EVENT_CHANGE_IRQ_ENABLE,
   VRM_EVENT_CHANGE_IRQ_DISABLE,
   VRM_EVENT_SETTLED_IRQ_ENABLE,
   VRM_EVENT_SETTLED_IRQ_DISABLE,
   VRM_EVENT_SEQ_DATA_READ,
   VRM_EVENT_SETTLING,
   VRM_EVENT_PBS_SW_TRIGGER,
   VRM_EVENT_MAX_VOLTAGE_VIOLATION,
   VRM_EVENT_CTT_OVERFLOW,
   VRM_EVENT_PMIC_ARB_FAILURE,
}vrm_log_event;

/* ***********LOG related structs START********** */
/**
 * @brief struct containing VRM log data. 
 */
typedef struct
{
   uint32 rsrc_id;
   uint32 pmic_index;
   pm_vrm_periph_type periph_type;
   uint32 periph_id;
   vrm_settings setting;
   uint32 data;
   vrm_log_event log_event;
   vrm_result result;
   uint64 timestamp;
}vrm_log_data;

/**
 * @brief struct containing VRM log info. 
 */
typedef struct
{
   uint32 log_idx;
   uint32 err_idx;
   vrm_log_data log[VRM_MAX_NUM_LOGS];
   vrm_log_data err[VRM_MAX_NUM_ERR_LOGS];
}vrm_log_info;

/**
 * @brief struct containing all the VRM IRQs triggered related info.
 */
typedef struct
{
   uint32 irq_arr_idx;
   vrm_irq_num irq_num_arr[VRM_MAX_IRQ_ARR_SIZE];
}vrm_irq_info;

/* ***********LOG related structs END********** */

/* ***********RSRC related structs START********** */
/**
 * @brief struct containing VRM rsrc type related data. 
 */
typedef struct
{
    unsigned type_id_assigned:1;
    unsigned enable_addr_offset:8;
    unsigned settling_wdog_timer:11;
    unsigned reserved:12;
    // 32-bit
    unsigned enable_fixed_time:16;
    unsigned enable_stepper_rate:16;
    // 32-bit
    unsigned enable_stepper_warm_up_time:16;
    unsigned volt_addr_offset:16;
    // 32-bit
    unsigned volt_fixed_time:16;
    unsigned volt_fixed_down_time:16;
    // 32-bit
    unsigned volt_stepper_rate:16;
    unsigned volt_stepper_extra_up_time:16;
    // 32-bit
    unsigned volt_stepper_extra_down_time:16;
    unsigned volt_discharge_time:16;
    // 32-bit
    unsigned mode_fixed_time:16;
    unsigned mode_addr_offset:16;
    // 32-bit
    unsigned settling_clr_addr_offset:8;
    unsigned settling_clr_data:8;
    unsigned reserved1:16;
    // 32-bit
}vrm_rsrc_type_data;

/**
 * @brief struct containing VRM rsrc related data. 
 */
typedef struct
{
    unsigned rsrc_category:4;
    unsigned rsrc_id:8;
    unsigned type_id:8;
    unsigned arb_id:12;
    // 32-bit
    unsigned pmic_index:8;
    unsigned periph_type:16;
    unsigned periph_id:8;
    // 32-bit
    unsigned dep_enable:1;
    unsigned volt_current_state:13;
    unsigned enable_current_state:13;
    unsigned mode_current_state:4;
    unsigned enable_min_state:1;
    // 32-bit
    unsigned volt_min_state:13;
    unsigned hr_min_state:13;
    unsigned mode_min_state:4;
    unsigned enable_pin_ctrl:1;
    unsigned reserved:1;
    // 32-bit
    unsigned volt_max_state:13;
    unsigned settling_en:1;
    unsigned settling_err_en:1;
    unsigned aop_handling:1;
    unsigned volt_seq_id:8;
    unsigned enable_seq_id:8;
    // 32-bit
    unsigned mode_seq_id:8;
    unsigned hr_seq_id:8;
    unsigned enable_fixed_time:16;
    // 32-bit
    unsigned drv_owner_mask:16;
    unsigned drv_priority_mask:16;
    // 32-bit
}vrm_rsrc_data;

/**
 * @brief struct containing VRM rsrc related names such 
 * as rsrc name, rsrc alias name and rsrc parent name. 
 */
typedef struct
{
   const char* name;
   const char* alias_name;
   const char* parent_name;
}vrm_rsrc_names;

/* ***********RSRC related structs END********** */

/* ***********VREG related structs START********** */

/**
 * @brief struct containing VREG dependency related data.
 */
typedef struct
{
   unsigned children_mask:16;  // should match the active_children_mask in vreg vote struct
   unsigned parent_id:8;
   unsigned child_start_id:8;
   // 32-bit
   unsigned child_end_id:8; 
   unsigned num_children:8;
   unsigned active_parent:1;
   unsigned dirty_parent:1;
   unsigned wait_for_parent:1;
   unsigned wait_for_children:1;
   unsigned parent_initialized:1;
   unsigned is_parent_bob:1;
   unsigned reserved:10;
   // 32-bit
}vrm_vreg_dep_data;

/**
 * @brief struct containing vote related data per VREG.
 */
typedef struct
{
   unsigned active_children_mask:16;  // should match the children_mask in dep struct
   unsigned dep_irq_status:1;
   unsigned change_irq_enabled:1; 
   unsigned settled_irq_enabled:1;
   unsigned ars_data:13;
   // 32-bit
   unsigned wrs_data:13;
   unsigned crs_data:13;
   unsigned reserved1:6;
   // 32-bit
   unsigned curr_agg:13;
   unsigned prev_agg:13;
   unsigned settling_err_en:1;
   unsigned pin_ctrl:1;
   unsigned reserved2:4;
   // 32-bit
   unsigned min_vote:12;
   unsigned addr_offset:20;
   // 32-bit
   unsigned active_vote:13;
   unsigned reserved3:19;
   // 32-bit
}vrm_vreg_vote_data;

/**
 * @brief struct containing specifc data per VREG.
 */
typedef struct
{
   uint32 pmic_index;
   pm_vrm_periph_type periph_type;
   uint32 periph_id;
   vrm_vreg_vote_data vote_data[VRM_NUM_SETTINGS];
}vrm_vreg_specific_data;

/**
 * @brief struct containing dependency data per VREG parent.
 */
typedef struct
{
   char parent_names[VRM_MAX_RSRC_NAME_LEN];
   vrm_vreg_dep_data* dep_data;
}vrm_vreg_dep_info;

/**
 * @brief struct containing all the VREG related info.
 */
typedef struct
{
   vrm_vreg_specific_data vreg[VRM_NUM_VREG];
   vrm_vreg_dep_info dep_info[VRM_MAX_NUM_PARENTS];
   uint64 children_mask;
   uint32 num_vreg;
   uint32 num_parents;
}vrm_vreg_info;

/**
 * @brief struct containing all the VREG SPMI related info.
 */
typedef struct
{
   boolean spmi_flag;
   uint32 type_id;
   uint32 rsrc_id;
   uint32 voltage;
   uint32 max_voltage;
   rpmh_client_handle rpmh_handle;
}vrm_spmi_info;

/**
 * @brief struct containing all the VRM SOC related info.
 */
typedef struct
{
   unsigned soc_flag:1;
   unsigned soc_active:1;
   unsigned rsrc_id:6;
   unsigned pbs_rsrc_id:6;
   unsigned pbs_sleep_arg:5;
   unsigned pbs_wake_arg:5;
   unsigned reserved:8;
}vrm_soc_info;

/* ***********VREG related structs END********** */

/**
 * @brief struct containing all the VRM driver related info.
 */
typedef struct
{
   boolean initialized;
   boolean post_initialized;
   uint32 version;
   vrm_rsrc_type_data type_arr[VRM_NUM_RSRC_TYPE];
   vrm_vreg_info vreg_info;
   vrm_log_info log_info;
   vrm_irq_info irq_info;
   vrm_spmi_info spmi_info;
   vrm_soc_info soc_info;
   vrm_config_data *config;
   rpmh_client_handle rpmh_handle;
}vrm_info;

#endif
