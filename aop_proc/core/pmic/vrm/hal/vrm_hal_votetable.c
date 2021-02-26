/*! \file vrm_hal_votetable.c
*
*  \brief Implementation file for VRM VOTETABLE hal layer interface.
*  \n
*  &copy; Copyright 2016 - 2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/vrm/hal/vrm_hal_votetable.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/16   kt      Initial version
===========================================================================*/

#include "vrm_hal.h"

//******************************************************************************
// Macros / Definitions / Constants
//******************************************************************************
#define VRM_SLAVE_ID_ADDR_OFF_SHFT 16

//******************************************************************************
// Global Data
//******************************************************************************


//******************************************************************************
// Local Helper Functions
//******************************************************************************
__attribute__((section("pm_cram_reclaim_pool")))
static vrm_result vrm_hal_votetable_xob_enable_init(vrm_info* vrm_ptr, vrm_rsrc_data* rsrc_data)
{
   vrm_result result = VRM_SUCCESS;
   uint32 drv_id = 0;

   // update XOB enable min settings DRV's votetable register with minimum resource state
   HWIO_OUTI2(RPMH_VRM_ENABLE_VOTE_DRVd_XOBr, vrm_ptr->config->min_settings_drv, rsrc_data->rsrc_id, rsrc_data->enable_min_state);

   while (drv_id < VRM_RSC_DRV_MAX)
   {
      if ((rsrc_data->drv_owner_mask >> drv_id) & 0x1)
      {
         // update XOB enable votetable register with current resource state
         HWIO_OUTI2(RPMH_VRM_ENABLE_VOTE_DRVd_XOBr, drv_id, rsrc_data->rsrc_id, rsrc_data->enable_current_state);
      }

      drv_id = drv_id + 1;
   }

   return result;
}

__attribute__((section("pm_cram_reclaim_pool")))
static vrm_result vrm_hal_votetable_vreg_voltage_init(vrm_info* vrm_ptr, vrm_rsrc_data* rsrc_data)
{
   vrm_result result = VRM_SUCCESS;
   uint32 drv_id = 0;

   // update voltage min settings DRV's votetable register with minimum resource state
   HWIO_OUTI2(RPMH_VRM_VOLTAGE_VOTE_DRVd_REGr, vrm_ptr->config->min_settings_drv, rsrc_data->rsrc_id, rsrc_data->volt_min_state);

   while (drv_id < VRM_RSC_DRV_MAX)
   {
      if ((rsrc_data->drv_owner_mask >> drv_id) & 0x1)
      {
         // update voltage votetable register with current resource state
         HWIO_OUTI2(RPMH_VRM_VOLTAGE_VOTE_DRVd_REGr, drv_id, rsrc_data->rsrc_id, rsrc_data->volt_current_state);
      }

      drv_id = drv_id + 1;
   }

   return result;
}

__attribute__((section("pm_cram_reclaim_pool")))
static vrm_result vrm_hal_votetable_vreg_enable_init(vrm_info* vrm_ptr, vrm_rsrc_data* rsrc_data)
{
   vrm_result result = VRM_SUCCESS;
   uint32 drv_id = 0;

   // update enable min settings DRV's votetable register with minimum resource state
   HWIO_OUTI2(RPMH_VRM_ENABLE_VOTE_DRVd_REGr, vrm_ptr->config->min_settings_drv, rsrc_data->rsrc_id, rsrc_data->enable_min_state);

   while (drv_id < VRM_RSC_DRV_MAX)
   {
      if ((rsrc_data->drv_owner_mask >> drv_id) & 0x1)
      {
         // update enable votetable register with current resource state
         HWIO_OUTI2(RPMH_VRM_ENABLE_VOTE_DRVd_REGr, drv_id, rsrc_data->rsrc_id, rsrc_data->enable_current_state);
      }

      drv_id = drv_id + 1;
   }

   return result;
}

__attribute__((section("pm_cram_reclaim_pool")))
static vrm_result vrm_hal_votetable_vreg_mode_init(vrm_info* vrm_ptr, vrm_rsrc_data* rsrc_data)
{
   vrm_result result = VRM_SUCCESS;
   uint32 drv_id = 0;

   // update mode min settings DRV's votetable register with minimum resource state
   HWIO_OUTI2(RPMH_VRM_MODE_VOTE_DRVd_REGr, vrm_ptr->config->min_settings_drv, rsrc_data->rsrc_id, rsrc_data->mode_min_state);

   while (drv_id < VRM_RSC_DRV_MAX)
   {
      if ((rsrc_data->drv_owner_mask >> drv_id) & 0x1)
      {
         // update mode votetable register with current resource state
         HWIO_OUTI2(RPMH_VRM_MODE_VOTE_DRVd_REGr, drv_id, rsrc_data->rsrc_id, rsrc_data->mode_current_state);
      }

      drv_id = drv_id + 1;
   }

   return result;
}

__attribute__((section("pm_cram_reclaim_pool")))
static vrm_result vrm_hal_votetable_vreg_headroom_init(vrm_info* vrm_ptr, vrm_rsrc_data* rsrc_data)
{
   vrm_result result = VRM_SUCCESS;

   return result;
}

//******************************************************************************
// Public API Functions
//******************************************************************************
__attribute__((section("pm_cram_reclaim_pool")))
vrm_result vrm_hal_votetable_init(vrm_info* vrm_ptr, vrm_rsrc_data* rsrc_data)
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
            result |= vrm_hal_votetable_vreg_enable_init(vrm_ptr, rsrc_data);
            result |= vrm_hal_votetable_vreg_voltage_init(vrm_ptr, rsrc_data);
            result |= vrm_hal_votetable_vreg_mode_init(vrm_ptr, rsrc_data);
            result |= vrm_hal_votetable_vreg_headroom_init(vrm_ptr, rsrc_data);
            break;

         case VRM_RSRC_XO_BUFFER:
            result = vrm_hal_votetable_xob_enable_init(vrm_ptr, rsrc_data);
            break;

         default:
            result = VRM_INVALID_PARAM;
            break;
      }
   }

   return result;
}

__attribute__((section("pm_cram_reclaim_pool")))
vrm_result vrm_hal_votetable_vreg_get_addr_offset(vrm_info* vrm_ptr, uint32 rsrc_id, uint32 setting, uint32 vrm_slave_id, uint32* addr_offset)
{
   vrm_result result = VRM_SUCCESS;

   if (addr_offset == NULL)
   {
      result = VRM_NULL_PTR_ERROR;
   }
   else
   {
      switch ((vrm_settings)setting)
      {
         case VRM_ENABLE:
            *addr_offset = HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_REGr_ADDR(0, rsrc_id) - RPMH_VRM_VRM_VOTETABLE_REG_BASE;
            break;
         case VRM_VOLTAGE:
            *addr_offset = HWIO_RPMH_VRM_VOLTAGE_VOTE_DRVd_REGr_ADDR(0, rsrc_id) - RPMH_VRM_VRM_VOTETABLE_REG_BASE;
            break;
         case VRM_HEADROOM:
            *addr_offset = HWIO_RPMH_VRM_HEADROOM_VOTE_DRVd_REGr_ADDR(0, rsrc_id) - RPMH_VRM_VRM_VOTETABLE_REG_BASE;
            break;
         case VRM_MODE:
            *addr_offset = HWIO_RPMH_VRM_MODE_VOTE_DRVd_REGr_ADDR(0, rsrc_id) - RPMH_VRM_VRM_VOTETABLE_REG_BASE;
            break;
         default:
            result = VRM_INVALID_PARAM;
            break;
      }
   }

   if (result == VRM_SUCCESS)
   {
      *addr_offset = (vrm_slave_id << VRM_SLAVE_ID_ADDR_OFF_SHFT) | *addr_offset;
   }

   return result;
}

__attribute__((section("pm_cram_reclaim_pool")))
vrm_result vrm_hal_votetable_xob_get_addr_offset(vrm_info* vrm_ptr, uint32 rsrc_id, uint32 setting, uint32 vrm_slave_id, uint32* addr_offset)
{
   vrm_result result = VRM_SUCCESS;

   if (addr_offset == NULL)
   {
      result = VRM_NULL_PTR_ERROR;
   }
   else
   {
      switch ((vrm_settings)setting)
      {
         case VRM_ENABLE:
            *addr_offset = HWIO_RPMH_VRM_ENABLE_VOTE_DRVd_XOBr_ADDR(0, rsrc_id) - RPMH_VRM_VRM_VOTETABLE_REG_BASE;
            break;
         default:
            result = VRM_INVALID_PARAM;
            break;
      }

      if (result == VRM_SUCCESS)
      {
         *addr_offset = (vrm_slave_id << VRM_SLAVE_ID_ADDR_OFF_SHFT) | *addr_offset;
      }
   }

   return result;
}

vrm_result vrm_hal_votetable_vreg_mode_update(vrm_info* vrm_ptr, uint32 drv_id, uint32 rsrc_id, uint32 mode)
{
   vrm_result result = VRM_SUCCESS;

   if ((drv_id > HWIO_RPMH_VRM_MODE_VOTE_DRVd_REGr_MAXd) || 
       (rsrc_id > HWIO_RPMH_VRM_MODE_VOTE_DRVd_REGr_MAXr) || 
       (mode > HWIO_RPMH_VRM_MODE_VOTE_DRVd_REGr_MODE_BMSK))
   {
      result = VRM_OUT_OF_BOUND_ERROR;
   }
   else
   {
      // update mode votetable register for given drv_id/rsrc_id with given mode data
      HWIO_OUTI2(RPMH_VRM_MODE_VOTE_DRVd_REGr, drv_id, rsrc_id, mode);
   }

   return result;
}


