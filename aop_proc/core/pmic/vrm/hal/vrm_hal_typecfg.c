/*! \file vrm_hal_typecfg.c
*
*  \brief Implementation file for VRM TYPECFG hal layer interface.
*  \n
*  &copy; Copyright 2016 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/vrm/hal/vrm_hal_typecfg.c#1 $

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
static vrm_result vrm_hal_typecfg_enable_init(vrm_info* vrm_ptr, uint32 type_id, vrm_rsrc_type_data* type_data)
{
   vrm_result result = VRM_SUCCESS;
   uint64 bit_mask = 0x1;
   uint64 type_id_mask = (bit_mask << type_id);

   HWIO_OUTI(RPMH_VRM_ENABLE_ADDR_OFFSET_CSR_TYPEr, type_id, type_data->enable_addr_offset);

   if (type_data->enable_fixed_time != 0)
   {
      HWIO_OUTI(RPMH_VRM_ENABLE_FIXED_SETTLING_TIME_CSR_TYPEr, type_id, type_data->enable_fixed_time);
      HWIO_OUTM(RPMH_VRM_ENABLE_UP_SETTLING_TIME_SEL, type_id_mask, type_id_mask);
   }
   else
   {
      HWIO_OUTI(RPMH_VRM_ENABLE_STEPPER_SETTLING_TIME_CSR_TYPEr, type_id, type_data->enable_stepper_rate);
      HWIO_OUTI(RPMH_VRM_ENABLE_WARM_UP_TIME_CSR_TYPEr, type_id, type_data->enable_stepper_warm_up_time);
   }

   HWIO_OUTI(RPMH_VRM_VREG_READY_WDOG_TIMER_MUL_CSR_TYPEr, type_id, type_data->settling_wdog_timer);

   // configure pmic vreg interrupt clear address offset and data
   HWIO_OUTI(RPMH_VRM_VREG_CLR_ADDR_OFFSET_CSR_TYPEr, type_id, type_data->settling_clr_addr_offset);
   HWIO_OUTI(RPMH_VRM_VREG_CLR_DATA_CSR_TYPEr, type_id, type_data->settling_clr_data);

   return result;
}

__attribute__((section("pm_cram_reclaim_pool")))
static vrm_result vrm_hal_typecfg_voltage_init(vrm_info* vrm_ptr, uint32 type_id, vrm_rsrc_type_data* type_data)
{
   vrm_result result = VRM_SUCCESS;
   uint64 bit_mask = 0x1;
   uint64 type_id_mask = (bit_mask << type_id);

   HWIO_OUTI(RPMH_VRM_VOLTAGE_ADDR_OFFSET_CSR_TYPEr, type_id, type_data->volt_addr_offset);

   if (type_data->volt_fixed_time != 0)
   {
      HWIO_OUTI(RPMH_VRM_VOLTAGE_FIXED_UP_SETTLING_TIME_CSR_TYPEr, type_id, type_data->volt_fixed_time);
      HWIO_OUTI(RPMH_VRM_VOLTAGE_FIXED_DOWN_SETTLING_TIME_CSR_TYPEr, type_id, type_data->volt_fixed_down_time);
      HWIO_OUTM(RPMH_VRM_VOLTAGE_UP_SETTLING_TIME_SEL, type_id_mask, type_id_mask);
   }
   else
   {
      HWIO_OUTI(RPMH_VRM_VOLTAGE_STEPPER_SETTLING_TIME_CSR_TYPEr, type_id, type_data->volt_stepper_rate);
      HWIO_OUTI(RPMH_VRM_VOLTAGE_UP_EXTRA_TIME_TYPEr, type_id, type_data->volt_stepper_extra_up_time);
      HWIO_OUTI(RPMH_VRM_VOLTAGE_DOWN_EXTRA_TIME_TYPEr, type_id, type_data->volt_stepper_extra_down_time);
   }

   // Cap discharge time option was added only from VRM v1.1
   if (vrm_ptr->version >= VRM_VERSION(1,1,0))
   {
      // VRM cap discharge time
      HWIO_OUTI(RPMH_VRM_DISCHARGE_TIME_CSR_TYPEr, type_id, type_data->volt_discharge_time);
   }

   return result;
}

__attribute__((section("pm_cram_reclaim_pool")))
static vrm_result vrm_hal_typecfg_mode_init(vrm_info* vrm_ptr, uint32 type_id, vrm_rsrc_type_data* type_data)
{
   vrm_result result = VRM_SUCCESS;

   HWIO_OUTI(RPMH_VRM_MODE_ADDR_OFFSET_CSR_TYPEr, type_id, type_data->mode_addr_offset);
   HWIO_OUTI(RPMH_VRM_MODE_SETTLING_TIME_CSR_TYPEr, type_id, type_data->mode_fixed_time);

   return result;
}

//******************************************************************************
// Public API Functions
//******************************************************************************

__attribute__((section("pm_cram_reclaim_pool")))
vrm_result vrm_hal_typecfg_init(vrm_info* vrm_ptr, uint32 type_id, vrm_rsrc_type_data* type_data)
{
   vrm_result result = VRM_SUCCESS;

   if(type_data == NULL)
   {
      result = VRM_NULL_PTR_ERROR;
   }
   else
   {
      result |= vrm_hal_typecfg_enable_init(vrm_ptr, type_id, type_data);
      result |= vrm_hal_typecfg_voltage_init(vrm_ptr, type_id, type_data);
      result |= vrm_hal_typecfg_mode_init(vrm_ptr, type_id, type_data);
   }

   return result;
}

vrm_result vrm_hal_typecfg_vreg_clr_offset_data_write(vrm_info* vrm_ptr, uint32 type_id, uint8 addr_offset, uint8 data)
{
   vrm_result result = VRM_SUCCESS;

   // configure pmic vreg interrupt clear address offset and data
   HWIO_OUTI(RPMH_VRM_VREG_CLR_ADDR_OFFSET_CSR_TYPEr, type_id, addr_offset);
   HWIO_OUTI(RPMH_VRM_VREG_CLR_DATA_CSR_TYPEr, type_id, data);

   return result;
}

