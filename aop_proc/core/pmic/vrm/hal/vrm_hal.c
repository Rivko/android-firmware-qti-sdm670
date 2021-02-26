/*! \file vrm_hal.c
*
*  \brief Implementation file for generic VRM hal layer interface.
*  \n
*  &copy; Copyright 2016 - 2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/vrm/hal/vrm_hal.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/16   kt      Initial version
===========================================================================*/

#include "vrm_hal.h"

//******************************************************************************
// Macros / Definitions / Constants
//******************************************************************************

/**
 * @brief Task id HW values specific to each VRM setting.
 */
typedef enum 
{
    VRM_TASK_VOLTAGE = 0,
    VRM_TASK_ENABLE = 1,
    VRM_TASK_MODE = 2,
    VRM_TASK_HEADROOM = 3,
    VRM_NUM_TASKS,
}vrm_task_id;

//******************************************************************************
// Global Data
//******************************************************************************

//******************************************************************************
// Local Helper Functions
//******************************************************************************

static vrm_result vrm_hal_task_id_to_setting(uint32 task_id, uint32* setting)
{
   vrm_result result = VRM_SUCCESS;

   if (setting == NULL)
   {
      return VRM_NULL_PTR_ERROR;
   }

   switch ((vrm_task_id)task_id)
   {
      case VRM_TASK_ENABLE:
         *setting = VRM_ENABLE;
         break;
      case VRM_TASK_VOLTAGE:
         *setting = VRM_VOLTAGE;
         break;
      case VRM_TASK_HEADROOM:
         *setting = VRM_HEADROOM;
         break;
      case VRM_TASK_MODE:
         *setting = VRM_MODE;
         break;
      default:
         result = VRM_INVALID_PARAM;
         break;
   }

   return result;
}

//******************************************************************************
// Public API Functions
//******************************************************************************
__attribute__((section("pm_cram_reclaim_pool")))
vrm_result vrm_hal_init(vrm_info* vrm_ptr)
{
   vrm_result result = VRM_SUCCESS;

   // get vrm hw version
   vrm_ptr->version = HWIO_IN(RPMH_VRM_VRM_VERSION);

   return result;
}

__attribute__((section("pm_cram_reclaim_pool")))
vrm_result vrm_hal_cfg_vrm_enable(vrm_info* vrm_ptr, uint32 slave_id)
{
   vrm_result result = VRM_SUCCESS;

   // write VRM slave id and enable VRM
   HWIO_OUT(RPMH_VRM_VRM_CFG, ((slave_id<<HWIO_RPMH_VRM_VRM_CFG_SLAVE_ID_SHFT)|0x1));

   return result;
}

vrm_result vrm_hal_cfg_unblock_cmd_syn(vrm_info* vrm_ptr, uint32 rsrc_id, uint32 setting)
{
   vrm_result result = VRM_SUCCESS;
   uint32 data = 0;
   uint32 task = 0;

   switch ((vrm_settings)setting)
   {
      case VRM_ENABLE:
         task = VRM_TASK_ENABLE;
         break;
      case VRM_VOLTAGE:
         task = VRM_TASK_VOLTAGE;
         break;
      case VRM_HEADROOM:
         task = VRM_TASK_HEADROOM;
         break;
      case VRM_MODE:
         task = VRM_TASK_MODE;
         break;
      default:
         result = VRM_INVALID_PARAM;
         break;
   }

   if (result == VRM_SUCCESS)
   {
      data = (task << 16) | rsrc_id;
      HWIO_OUT(RPMH_VRM_CMD_SYN_UNBLOCK, data);
   }

   vrm_log(vrm_ptr, VRM_RSRC_REGULATOR, rsrc_id, setting, &data, result, VRM_EVENT_CMDSYN_UNBLOCK);

   return result;
}

vrm_result vrm_hal_settling_wdog_error_enable(vrm_info* vrm_ptr, uint32 seq_id, boolean enable)
{
   vrm_result result = VRM_SUCCESS;

   if (seq_id >= VRM_MAX_NUM_SEQCERS)
   {
      result = VRM_OUT_OF_BOUND_ERROR;
   }
   else
   {
      // this is per sequencer though the register definition indicates it's per regulator
      HWIO_OUTI(RPMH_VRM_VREG_READY_WDOG_ERROR_ENABLE_CSR_REGr, seq_id, enable);
   }

   return result;
}

vrm_result vrm_hal_seq_data_read(vrm_info* vrm_ptr, uint32 seq_id, vrm_hal_seq_data_type* seq_data)
{
   vrm_result result = VRM_SUCCESS;
   vrm_rsrc_category rsrc_category = VRM_RSRC_INVALID_CATEGORY;
   uint32 data = 0;
   uint32 rsrc_id = 0;
   uint32 task_id = 0;
   uint32 setting = 0;
   uint32 settling_time_vrm = 0;
   uint32 settling_time_usec = 0;

   if (seq_data == NULL)
   {
      result = VRM_NULL_PTR_ERROR;
   }
   else if (seq_id >= VRM_MAX_NUM_SEQCERS)
   {
      result = VRM_OUT_OF_BOUND_ERROR;
   }
   else
   {
      data = HWIO_INI(RPMH_VRM_SEQ_DATA_0_SEQCERSr, seq_id);

      rsrc_id = VRM_HWIO_FIELD_VALUE(data, HWIO_RPMH_VRM_SEQ_DATA_0_SEQCERSr_RESOURCE_ID);
      task_id = VRM_HWIO_FIELD_VALUE(data, HWIO_RPMH_VRM_SEQ_DATA_0_SEQCERSr_TASK_ID);
      settling_time_vrm = VRM_HWIO_FIELD_VALUE(data, HWIO_RPMH_VRM_SEQ_DATA_0_SEQCERSr_SETTLING_TIME);

      result |= vrm_get_time_usec(settling_time_vrm, &settling_time_usec);
      result |= vrm_hal_task_id_to_setting(task_id, &setting);

      if (rsrc_id < VRM_NUM_VREG)
      {
         rsrc_category = VRM_RSRC_REGULATOR;
      }

      seq_data->rsrc_category = rsrc_category;
      seq_data->rsrc_id = rsrc_id;
      seq_data->setting = setting;
      seq_data->settling_time_usec = settling_time_usec;

   }

   vrm_log(vrm_ptr, rsrc_category, rsrc_id, setting, &data, result, VRM_EVENT_SEQ_DATA_READ);

   return result;
}


