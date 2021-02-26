/*! \file vrm_hal_rescfg.c
*
*  \brief Implementation file for VRM RESCFG hal layer interface.
*  \n
*  &copy; Copyright 2016 - 2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/vrm/hal/vrm_hal_rescfg.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/16   kt      Initial version
===========================================================================*/

#include "vrm_hal.h"

//******************************************************************************
// Macros / Definitions / Constants
//******************************************************************************

//******************************************************************************
// Global Data
//******************************************************************************


//******************************************************************************
// Local Helper Functions
//******************************************************************************
__attribute__((section("pm_cram_reclaim_pool")))
static vrm_result vrm_hal_rescfg_xob_enable_init(vrm_info* vrm_ptr, vrm_rsrc_data* rsrc_data)
{
   vrm_result result = VRM_SUCCESS;

   HWIO_OUTI(RPMH_VRM_XOB_SETTLING_TIME_CSR_RESr, rsrc_data->rsrc_id, rsrc_data->enable_fixed_time);

   // update XOB enable CRS, WRS, DRS, ARS registers with current resource state
   HWIO_OUTI(RPMH_VRM_ENABLE_CRS_CSR_XOBr, rsrc_data->rsrc_id, rsrc_data->enable_current_state);
   HWIO_OUTI(RPMH_VRM_ENABLE_WRS_CSR_XOBr, rsrc_data->rsrc_id, rsrc_data->enable_current_state);
   HWIO_OUTI(RPMH_VRM_ENABLE_DRS_CSR_XOBr, rsrc_data->rsrc_id, rsrc_data->enable_current_state);
   HWIO_OUTI(RPMH_VRM_ENABLE_ARS_CSR_XOBr, rsrc_data->rsrc_id, rsrc_data->enable_current_state);

   // check if dependency has to be enabled
   HWIO_OUTI(RPMH_VRM_ENABLE_DEP_ENABLE_CSR_XOBr, rsrc_data->rsrc_id, rsrc_data->dep_enable);

   // update enable sequence id
   HWIO_OUTI(RPMH_VRM_ENABLE_SEQ_ID_CSR_XOBr, rsrc_data->rsrc_id, rsrc_data->enable_seq_id);

   return result;
}

__attribute__((section("pm_cram_reclaim_pool")))
static vrm_result vrm_hal_rescfg_vreg_voltage_init(vrm_info* vrm_ptr, vrm_rsrc_data* rsrc_data)
{
   vrm_result result = VRM_SUCCESS;

   // update voltage CRS, WRS, DRS, ARS registers with current resource state
   HWIO_OUTI(RPMH_VRM_VOLTAGE_CRS_CSR_REGr, rsrc_data->rsrc_id, rsrc_data->volt_current_state);
   HWIO_OUTI(RPMH_VRM_VOLTAGE_WRS_CSR_REGr, rsrc_data->rsrc_id, rsrc_data->volt_current_state);
   HWIO_OUTI(RPMH_VRM_VOLTAGE_DRS_CSR_REGr, rsrc_data->rsrc_id, rsrc_data->volt_current_state);
   HWIO_OUTI(RPMH_VRM_VOLTAGE_ARS_CSR_REGr, rsrc_data->rsrc_id, rsrc_data->volt_current_state);

   // update max voltage resource state

   // program max voltage field to 1mV higher than the desired max voltage to work-around HW issue
   // where back to back requests on max voltage will fail on Napali V1. This will be fixed on Napali v2.
   HWIO_OUTI(RPMH_VRM_MAX_VOLTAGE_CSR_REGr, rsrc_data->rsrc_id, rsrc_data->volt_max_state + 1);

   // check if dependency has to be enabled
   if (rsrc_data->dep_enable)
   {
      result |= vrm_hal_rescfg_vreg_dep_enable(vrm_ptr, rsrc_data->rsrc_id, VRM_VOLTAGE, TRUE);
   }

   // update voltage sequence id
   HWIO_OUTI(RPMH_VRM_VOLTAGE_SEQ_ID_CSR_REGr, rsrc_data->rsrc_id, rsrc_data->volt_seq_id);

   return result;
}

__attribute__((section("pm_cram_reclaim_pool")))
static vrm_result vrm_hal_rescfg_vreg_enable_init(vrm_info* vrm_ptr, vrm_rsrc_data* rsrc_data)
{
   vrm_result result = VRM_SUCCESS;

   // update enable CRS, WRS, DRS, ARS registers with current resource state
   HWIO_OUTI(RPMH_VRM_ENABLE_CRS_CSR_REGr, rsrc_data->rsrc_id, rsrc_data->enable_current_state);
   HWIO_OUTI(RPMH_VRM_ENABLE_WRS_CSR_REGr, rsrc_data->rsrc_id, rsrc_data->enable_current_state);
   HWIO_OUTI(RPMH_VRM_ENABLE_DRS_CSR_REGr, rsrc_data->rsrc_id, rsrc_data->enable_current_state);
   HWIO_OUTI(RPMH_VRM_ENABLE_ARS_CSR_REGr, rsrc_data->rsrc_id, rsrc_data->enable_current_state);

   // check if dependency has to be enabled
   if (rsrc_data->dep_enable)
   {
      result |= vrm_hal_rescfg_vreg_dep_enable(vrm_ptr, rsrc_data->rsrc_id, VRM_ENABLE, TRUE);
   }

   // update enable sequence id
   HWIO_OUTI(RPMH_VRM_ENABLE_SEQ_ID_CSR_REGr, rsrc_data->rsrc_id, rsrc_data->enable_seq_id);

   return result;
}

__attribute__((section("pm_cram_reclaim_pool")))
static vrm_result vrm_hal_rescfg_vreg_mode_init(vrm_info* vrm_ptr, vrm_rsrc_data* rsrc_data)
{
   vrm_result result = VRM_SUCCESS;

   // update mode CRS, WRS, DRS, ARS registers with current resource state
   HWIO_OUTI(RPMH_VRM_MODE_CRS_CSR_REGr, rsrc_data->rsrc_id, rsrc_data->mode_current_state);
   HWIO_OUTI(RPMH_VRM_MODE_WRS_CSR_REGr, rsrc_data->rsrc_id, rsrc_data->mode_current_state);
   HWIO_OUTI(RPMH_VRM_MODE_DRS_CSR_REGr, rsrc_data->rsrc_id, rsrc_data->mode_current_state);
   HWIO_OUTI(RPMH_VRM_MODE_ARS_CSR_REGr, rsrc_data->rsrc_id, rsrc_data->mode_current_state);

   // update mode sequence id
   HWIO_OUTI(RPMH_VRM_MODE_SEQ_ID_CSR_REGr, rsrc_data->rsrc_id, rsrc_data->mode_seq_id);

   return result;
}

__attribute__((section("pm_cram_reclaim_pool")))
static vrm_result vrm_hal_rescfg_vreg_headroom_init(vrm_info* vrm_ptr, vrm_rsrc_data* rsrc_data)
{
   vrm_result result = VRM_SUCCESS;

   // update headroom CRS, WRS, DRS, ARS registers with current resource state
   HWIO_OUTI(RPMH_VRM_HEADROOM_CRS_CSR_REGr, rsrc_data->rsrc_id, 0);
   HWIO_OUTI(RPMH_VRM_HEADROOM_WRS_CSR_REGr, rsrc_data->rsrc_id, 0);
   HWIO_OUTI(RPMH_VRM_HEADROOM_DRS_CSR_REGr, rsrc_data->rsrc_id, 0);
   HWIO_OUTI(RPMH_VRM_HEADROOM_ARS_CSR_REGr, rsrc_data->rsrc_id, 0);

   // check if dependency has to be enabled
   if (rsrc_data->dep_enable)
   {
      result |= vrm_hal_rescfg_vreg_dep_enable(vrm_ptr, rsrc_data->rsrc_id, VRM_HEADROOM, TRUE);
   }

   // update headroom sequence id
   HWIO_OUTI(RPMH_VRM_HEADROOM_SEQ_ID_CSR_REGr, rsrc_data->rsrc_id, rsrc_data->hr_seq_id);

   return result;
}

__attribute__((section("pm_cram_reclaim_pool")))
static vrm_result vrm_hal_rescfg_vreg_init(vrm_info* vrm_ptr, vrm_rsrc_data* rsrc_data)
{
   vrm_result result = VRM_SUCCESS;

   // settling and settling error enable
   if (rsrc_data->settling_en)
   {
      HWIO_OUTI(RPMH_VRM_VREG_READY_ENABLE_CSR_REGr, rsrc_data->rsrc_id, rsrc_data->settling_en);
   }

   // check if dependency interrupt has to be enabled
   if (rsrc_data->dep_enable)
   {
      // enable dependency interrupt for voltage, enable, mode, headroom with one API call
      result |= vrm_hal_interrupt_dep_irq_enable(vrm_ptr, rsrc_data->rsrc_id, VRM_ENABLE, TRUE);
      result |= vrm_hal_interrupt_dep_irq_enable(vrm_ptr, rsrc_data->rsrc_id, VRM_VOLTAGE, TRUE);
      result |= vrm_hal_interrupt_dep_irq_enable(vrm_ptr, rsrc_data->rsrc_id, VRM_HEADROOM, TRUE);
   }

   // always enable voltage<->enable settling dependency enable for all the resources
   HWIO_OUTI(RPMH_VRM_VOLTAGE_ENABLE_SETTLING_DEP_ENABLE_CSR_REGr, rsrc_data->rsrc_id, 0x1);

   // configure type id for the resource
   HWIO_OUTI(RPMH_VRM_TYPE_CSR_REGr, rsrc_data->rsrc_id, rsrc_data->type_id);

   // update VRM PMIC arb id
   HWIO_OUTI(RPMH_VRM_BASE_ADDR_CSR_REGr, rsrc_data->rsrc_id, rsrc_data->arb_id);

   result |= vrm_hal_rescfg_vreg_enable_init(vrm_ptr, rsrc_data);
   result |= vrm_hal_rescfg_vreg_voltage_init(vrm_ptr, rsrc_data);
   result |= vrm_hal_rescfg_vreg_mode_init(vrm_ptr, rsrc_data);
   result |= vrm_hal_rescfg_vreg_headroom_init(vrm_ptr, rsrc_data);

   return result;
}

__attribute__((section("pm_cram_reclaim_pool")))
static vrm_result vrm_hal_rescfg_xob_init(vrm_info* vrm_ptr, vrm_rsrc_data* rsrc_data)
{
   vrm_result result = VRM_SUCCESS;

   // configure type id for the resource
   HWIO_OUTI(RPMH_VRM_TYPE_CSR_XOBr, rsrc_data->rsrc_id, rsrc_data->type_id);

   // update VRM PMIC arb id
   HWIO_OUTI(RPMH_VRM_BASE_ADDR_CSR_XOBr, rsrc_data->rsrc_id, rsrc_data->arb_id);

   result |= vrm_hal_rescfg_xob_enable_init(vrm_ptr, rsrc_data);

   return result;
}

//******************************************************************************
// Public API Functions
//******************************************************************************

__attribute__((section("pm_cram_reclaim_pool")))
vrm_result vrm_hal_rescfg_init(vrm_info* vrm_ptr, vrm_rsrc_data* rsrc_data)
{
   vrm_result result = VRM_SUCCESS;

   if(rsrc_data == NULL)
   {
      result = VRM_NULL_PTR_ERROR;
   }
   else
   {
      switch (rsrc_data->rsrc_category) 
      {
         case VRM_RSRC_REGULATOR:
            result |= vrm_hal_rescfg_vreg_init(vrm_ptr, rsrc_data);
            break;

         case VRM_RSRC_XO_BUFFER:
            result |= vrm_hal_rescfg_xob_init(vrm_ptr, rsrc_data);
            break;

         default:
            result = VRM_INVALID_PARAM;
            break;
      }
   }

   return result;
}

vrm_result vrm_hal_rescfg_ars_read(vrm_info* vrm_ptr, uint32 rsrc_id, uint32 setting, uint32* data)
{
   vrm_result result = VRM_SUCCESS;

   if (data == NULL)
   {
      result = VRM_NULL_PTR_ERROR;
   }
   else
   {
      switch ((vrm_settings)setting)
      {
         case VRM_ENABLE:
            *data = HWIO_INI(RPMH_VRM_ENABLE_ARS_CSR_REGr, rsrc_id);
            break;
         case VRM_VOLTAGE:
            *data = HWIO_INI(RPMH_VRM_VOLTAGE_ARS_CSR_REGr, rsrc_id);
            break;
         case VRM_HEADROOM:
            *data = HWIO_INI(RPMH_VRM_HEADROOM_ARS_CSR_REGr, rsrc_id);
            break;
         case VRM_MODE:
            *data = HWIO_INI(RPMH_VRM_MODE_ARS_CSR_REGr, rsrc_id);
            break;
         default:
            result = VRM_INVALID_PARAM;
            break;
      }
   }

   vrm_log(vrm_ptr, VRM_RSRC_REGULATOR, rsrc_id, setting, data, result, VRM_EVENT_ARS_READ);

   return result;
}

vrm_result vrm_hal_rescfg_wrs_read(vrm_info* vrm_ptr, uint32 rsrc_id, uint32 setting, uint32* data)
{
   vrm_result result = VRM_SUCCESS;

   if (data == NULL)
   {
      result = VRM_NULL_PTR_ERROR;
   }
   else
   {
      switch ((vrm_settings)setting)
      {
         case VRM_ENABLE:
            *data = HWIO_INI(RPMH_VRM_ENABLE_WRS_CSR_REGr, rsrc_id);
            break;
         case VRM_VOLTAGE:
            *data = HWIO_INI(RPMH_VRM_VOLTAGE_WRS_CSR_REGr, rsrc_id);
            break;
         case VRM_HEADROOM:
            *data = HWIO_INI(RPMH_VRM_HEADROOM_WRS_CSR_REGr, rsrc_id);
            break;
         case VRM_MODE:
            *data = HWIO_INI(RPMH_VRM_MODE_WRS_CSR_REGr, rsrc_id);
            break;
         default:
            result = VRM_INVALID_PARAM;
            break;
      }
   }

   vrm_log(vrm_ptr, VRM_RSRC_REGULATOR, rsrc_id, setting, data, result, VRM_EVENT_WRS_READ);

   return result;
}

vrm_result vrm_hal_rescfg_crs_read(vrm_info* vrm_ptr, uint32 rsrc_id, uint32 setting, uint32* data)
{
   vrm_result result = VRM_SUCCESS;

   if (data == NULL)
   {
      result = VRM_NULL_PTR_ERROR;
   }
   else
   {
      switch ((vrm_settings)setting)
      {
         case VRM_ENABLE:
            *data = HWIO_INI(RPMH_VRM_ENABLE_CRS_CSR_REGr, rsrc_id);
            break;
         case VRM_VOLTAGE:
            *data = HWIO_INI(RPMH_VRM_VOLTAGE_CRS_CSR_REGr, rsrc_id);
            break;
         case VRM_HEADROOM:
            *data = HWIO_INI(RPMH_VRM_HEADROOM_CRS_CSR_REGr, rsrc_id);
            break;
         case VRM_MODE:
            *data = HWIO_INI(RPMH_VRM_MODE_CRS_CSR_REGr, rsrc_id);
            break;
         default:
            result = VRM_INVALID_PARAM;
            break;
      }
   }

   vrm_log(vrm_ptr, VRM_RSRC_REGULATOR, rsrc_id, setting, data, result, VRM_EVENT_CRS_READ);

   return result;
}

vrm_result vrm_hal_rescfg_vreg_dep_enable(vrm_info* vrm_ptr, uint32 rsrc_id, uint32 setting, boolean enable)
{
   vrm_result result = VRM_SUCCESS;
   uint32 dep_enable = 0;

   if (enable == TRUE)
   {
      dep_enable = 1;
   }

   switch ((vrm_settings)setting)
   {
      case VRM_ENABLE:
         HWIO_OUTI(RPMH_VRM_ENABLE_DEP_ENABLE_CSR_REGr, rsrc_id, dep_enable);
         break;
      case VRM_VOLTAGE:
         HWIO_OUTI(RPMH_VRM_VOLTAGE_DEP_ENABLE_CSR_REGr, rsrc_id, dep_enable);
         break;
      case VRM_HEADROOM:
         HWIO_OUTI(RPMH_VRM_HEADROOM_DEP_ENABLE_CSR_REGr, rsrc_id, dep_enable);
         break;
      case VRM_MODE:
         HWIO_OUTI(RPMH_VRM_MODE_DEP_ENABLE_CSR_REGr, rsrc_id, dep_enable);
         break;
      default:
         result = VRM_INVALID_PARAM;
         break;
   }

   return result;
}


vrm_result vrm_hal_rescfg_base_addr_write(vrm_info* vrm_ptr, uint32 rsrc_id, uint8 vrm_arb_id)
{
   vrm_result result = VRM_SUCCESS;

   HWIO_OUTI(RPMH_VRM_BASE_ADDR_CSR_REGr, rsrc_id, vrm_arb_id);

   return result;
}

vrm_result vrm_hal_rescfg_settling_block_enable(vrm_info* vrm_ptr, uint32 rsrc_id, uint32 setting, boolean enable)
{
   vrm_result result = VRM_SUCCESS;
   switch ((vrm_settings)setting)
   {
      case VRM_VOLTAGE:
      HWIO_OUTI(RPMH_VRM_VOLTAGE_SETTLING_BLOCK_ENABLE_CSR_REGr,rsrc_id,enable);
      break;
      case VRM_MODE :
      HWIO_OUTI(RPMH_VRM_MODE_SETTLING_BLOCK_ENABLE_CSR_REGr,rsrc_id,enable);
      break;
      case VRM_HEADROOM:
      HWIO_OUTI(RPMH_VRM_HEADROOM_SETTLING_BLOCK_ENABLE_CSR_REGr,rsrc_id,enable);
      break;
      case VRM_ENABLE:
      HWIO_OUTI(RPMH_VRM_VOLTAGE_SETTLING_BLOCK_ENABLE_CSR_REGr,rsrc_id,enable);
      break;
      default:
      result= VRM_INVALID_PARAM;
      break;
   }
   return result;
}
