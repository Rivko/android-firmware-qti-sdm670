/*! \file vrm_hal_interrupt.c
*
*  \brief Implementation file for VRM INTERRUPT hal layer interface.
*  \n
*  &copy; Copyright 2016 - 2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/vrm/hal/vrm_hal_interrupt.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/16   kt      Initial version
===========================================================================*/

#include "vrm_hal.h"

//******************************************************************************
// Macros / Definitions / Constants
//******************************************************************************

#define VRM_32BIT_REG_MASK  0xffffffff

//******************************************************************************
// Global Data
//******************************************************************************


//******************************************************************************
// Local Helper Functions
//******************************************************************************

static void vrm_hal_interrupt_split_mask(uint64 mask, uint32 *mask0, uint32 *mask1)
{
   *mask0 = (mask & VRM_32BIT_REG_MASK);
   *mask1 = ((mask >> 32) & VRM_32BIT_REG_MASK);

   return;
}

__attribute__((section("pm_cram_reclaim_pool")))
static vrm_result vrm_hal_interrupt_vreg_init(vrm_info* vrm_ptr, vrm_rsrc_data* rsrc_data)
{
   vrm_result result = VRM_SUCCESS;
   uint32 mask0 = 0;
   uint32 mask1 = 0;
   uint64 bit_mask = 0x1;
   uint64 mask = bit_mask << rsrc_data->rsrc_id;

   // split the mask since there are 2 different registers
   vrm_hal_interrupt_split_mask(mask, &mask0, &mask1);

   if (mask0 != 0)
   {
      HWIO_OUTM(RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_ENABLE_0_CSR, mask0, mask0);
   }
   if (mask1 != 0)
   {
      HWIO_OUTM(RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_ENABLE_1_CSR, mask1, mask1);
   }

   return result;
}

__attribute__((section("pm_cram_reclaim_pool")))
static vrm_result vrm_hal_interrupt_xob_init(vrm_info* vrm_ptr, vrm_rsrc_data* rsrc_data)
{
   vrm_result result = VRM_SUCCESS;

   return result;
}

//******************************************************************************
// Public API Functions
//******************************************************************************
__attribute__((section("pm_cram_reclaim_pool")))
vrm_result vrm_hal_interrupt_init(vrm_info* vrm_ptr, vrm_rsrc_data* rsrc_data)
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
            result |= vrm_hal_interrupt_vreg_init(vrm_ptr, rsrc_data);
            break;

         case VRM_RSRC_XO_BUFFER:
            result |= vrm_hal_interrupt_xob_init(vrm_ptr, rsrc_data);
            break;

         default:
            result = VRM_INVALID_PARAM;
            break;
      }
   }

   return result;
}

vrm_result vrm_hal_interrupt_dep_irq_enable(vrm_info* vrm_ptr, uint32 rsrc_id, uint32 setting, boolean enable)
{
   vrm_result result = VRM_SUCCESS;
   uint32 mask0 = 0;
   uint32 mask1 = 0;
   uint32 data0 = 0;
   uint32 data1 = 0;
   uint64 bit_mask = 0x1;
   uint64 mask = bit_mask << rsrc_id;

   // split the mask since there are 2 different registers
   vrm_hal_interrupt_split_mask(mask, &mask0, &mask1);

   if (enable == TRUE)
   {
      data0 = mask0;
      data1 = mask1;
   }

   switch ((vrm_settings)setting)
   {
      case VRM_ENABLE:
         if (mask0 != 0)
         {
            HWIO_OUTM(RPMH_VRM_ENABLE_DEP_IRQ_ENABLE_0_CSR, mask0, data0);
         }
         if (mask1 != 0)
         {
            HWIO_OUTM(RPMH_VRM_ENABLE_DEP_IRQ_ENABLE_1_CSR, mask1, data1);
         }
         break;
      case VRM_VOLTAGE:
         if (mask0 != 0)
         {
            HWIO_OUTM(RPMH_VRM_VOLTAGE_DEP_IRQ_ENABLE_0_CSR, mask0, data0);
         }
         if (mask1 != 0)
         {
            HWIO_OUTM(RPMH_VRM_VOLTAGE_DEP_IRQ_ENABLE_1_CSR, mask1, data1);
         }
         break;
      case VRM_HEADROOM:
         if (mask0 != 0)
         {
            HWIO_OUTM(RPMH_VRM_HEADROOM_DEP_IRQ_ENABLE_0_CSR, mask0, data0);
         }
         if (mask1 != 0)
         {
            HWIO_OUTM(RPMH_VRM_HEADROOM_DEP_IRQ_ENABLE_1_CSR, mask1, data1);
         }
         break;
      case VRM_MODE:
         if (mask0 != 0)
         {
            HWIO_OUTM(RPMH_VRM_MODE_DEP_IRQ_ENABLE_0_CSR, mask0, data0);
         }
         if (mask1 != 0)
         {
            HWIO_OUTM(RPMH_VRM_MODE_DEP_IRQ_ENABLE_1_CSR, mask1, data1);
         }
         break;
      default:
         result = VRM_INVALID_PARAM;
         break;
   }

   return result;
}

vrm_result vrm_hal_interrupt_change_irq_enable(vrm_info* vrm_ptr, uint32 rsrc_id, uint32 setting, boolean enable)
{
   vrm_result result = VRM_SUCCESS;
   uint32 mask0 = 0;
   uint32 mask1 = 0;
   uint32 data0 = 0;
   uint32 data1 = 0;
   uint64 bit_mask = 0x1;
   uint64 mask = bit_mask << rsrc_id;
   vrm_log_event log_event;

   // split the mask since there are 2 different registers
   vrm_hal_interrupt_split_mask(mask, &mask0, &mask1);

   if (enable == TRUE)
   {
      data0 = mask0;
      data1 = mask1;
   }

   switch ((vrm_settings)setting)
   {
      case VRM_ENABLE:
         if (mask0 != 0)
         {
            HWIO_OUTM(RPMH_VRM_ENABLE_CHANGE_IRQ_ENABLE_0_CSR, mask0, data0);
         }
         if (mask1 != 0)
         {
            HWIO_OUTM(RPMH_VRM_ENABLE_CHANGE_IRQ_ENABLE_1_CSR, mask1, data1);
         }
         break;
      case VRM_VOLTAGE:
         if (mask0 != 0)
         {
            HWIO_OUTM(RPMH_VRM_VOLTAGE_CHANGE_IRQ_ENABLE_0_CSR, mask0, data0);
         }
         if (mask1 != 0)
         {
            HWIO_OUTM(RPMH_VRM_VOLTAGE_CHANGE_IRQ_ENABLE_1_CSR, mask1, data1);
         }
         break;
      case VRM_HEADROOM:
         if (mask0 != 0)
         {
            HWIO_OUTM(RPMH_VRM_HEADROOM_CHANGE_IRQ_ENABLE_0_CSR, mask0, data0);
         }
         if (mask1 != 0)
         {
            HWIO_OUTM(RPMH_VRM_HEADROOM_CHANGE_IRQ_ENABLE_1_CSR, mask1, data1);
         }
         break;
      case VRM_MODE:
         if (mask0 != 0)
         {
            HWIO_OUTM(RPMH_VRM_MODE_CHANGE_IRQ_ENABLE_0_CSR, mask0, data0);
         }
         if (mask1 != 0)
         {
            HWIO_OUTM(RPMH_VRM_MODE_CHANGE_IRQ_ENABLE_1_CSR, mask1, data1);
         }
         break;
      default:
         result = VRM_INVALID_PARAM;
         break;
   }

   if (mask0 != 0)
   {
      log_event = VRM_EVENT_CHANGE_IRQ_DISABLE;
      if (data0 != 0)
      {
         log_event = VRM_EVENT_CHANGE_IRQ_ENABLE;
      }
      vrm_log(vrm_ptr, VRM_RSRC_REGULATOR, rsrc_id, setting, &mask0, result, log_event);
   }

   if (mask1 != 0)
   {
      log_event = VRM_EVENT_CHANGE_IRQ_DISABLE;
      if (data1 != 0)
      {
         log_event = VRM_EVENT_CHANGE_IRQ_ENABLE;
      }
      vrm_log(vrm_ptr, VRM_RSRC_REGULATOR, rsrc_id, setting, &mask1, result, log_event);
   }

   return result;
}

vrm_result vrm_hal_interrupt_change_irq_clear(vrm_info* vrm_ptr, uint32 rsrc_id, uint32 setting)
{
   vrm_result result = VRM_SUCCESS;
   uint32 mask0 = 0;
   uint32 mask1 = 0;
   uint64 bit_mask = 0x1;
   uint64 mask = bit_mask << rsrc_id;

   // split the mask since there are 2 different registers
   vrm_hal_interrupt_split_mask(mask, &mask0, &mask1);

   switch ((vrm_settings)setting)
   {
      case VRM_ENABLE:
         if (mask0 != 0)
         {
            HWIO_OUT(RPMH_VRM_ENABLE_CHANGE_IRQ_CLEAR_0_CSR, mask0);
         }
         if (mask1 != 0)
         {
            HWIO_OUT(RPMH_VRM_ENABLE_CHANGE_IRQ_CLEAR_1_CSR, mask1);
         }
         break;
      case VRM_VOLTAGE:
         if (mask0 != 0)
         {
            HWIO_OUT(RPMH_VRM_VOLTAGE_CHANGE_IRQ_CLEAR_0_CSR, mask0);
         }
         if (mask1 != 0)
         {
            HWIO_OUT(RPMH_VRM_VOLTAGE_CHANGE_IRQ_CLEAR_1_CSR, mask1);
         }
         break;
      case VRM_HEADROOM:
         if (mask0 != 0)
         {
            HWIO_OUT(RPMH_VRM_HEADROOM_CHANGE_IRQ_CLEAR_0_CSR, mask0);
         }
         if (mask1 != 0)
         {
            HWIO_OUT(RPMH_VRM_HEADROOM_CHANGE_IRQ_CLEAR_1_CSR, mask1);
         }
         break;
      case VRM_MODE:
         if (mask0 != 0)
         {
            HWIO_OUT(RPMH_VRM_MODE_CHANGE_IRQ_CLEAR_0_CSR, mask0);
         }
         if (mask1 != 0)
         {
            HWIO_OUT(RPMH_VRM_MODE_CHANGE_IRQ_CLEAR_1_CSR, mask1);
         }
         break;
      default:
         result = VRM_INVALID_PARAM;
         break;
   }

   return result;
}

vrm_result vrm_hal_interrupt_settled_irq_enable(vrm_info* vrm_ptr, uint32 rsrc_id, uint32 setting, boolean enable)
{
   vrm_result result = VRM_SUCCESS;
   uint32 mask0 = 0;
   uint32 mask1 = 0;
   uint32 data0 = 0;
   uint32 data1 = 0;
   uint64 bit_mask = 0x1;
   uint64 mask = bit_mask << rsrc_id;
   vrm_log_event log_event;

   // split the mask since there are 2 different registers
   vrm_hal_interrupt_split_mask(mask, &mask0, &mask1);

   if (enable == TRUE)
   {
      data0 = mask0;
      data1 = mask1;
   }

   switch ((vrm_settings)setting)
   {
      case VRM_ENABLE:
         if (mask0 != 0)
         {
            HWIO_OUTM(RPMH_VRM_ENABLE_SETTLED_IRQ_ENABLE_0_CSR, mask0, data0);
         }
         if (mask1 != 0)
         {
            HWIO_OUTM(RPMH_VRM_ENABLE_SETTLED_IRQ_ENABLE_1_CSR, mask1, data1);
         }
         break;
      case VRM_VOLTAGE:
         if (mask0 != 0)
         {
            HWIO_OUTM(RPMH_VRM_VOLTAGE_SETTLED_IRQ_ENABLE_0_CSR, mask0, data0);
         }
         if (mask1 != 0)
         {
            HWIO_OUTM(RPMH_VRM_VOLTAGE_SETTLED_IRQ_ENABLE_1_CSR, mask1, data1);
         }
         break;
      case VRM_HEADROOM:
         if (mask0 != 0)
         {
            HWIO_OUTM(RPMH_VRM_HEADROOM_SETTLED_IRQ_ENABLE_0_CSR, mask0, data0);
         }
         if (mask1 != 0)
         {
            HWIO_OUTM(RPMH_VRM_HEADROOM_SETTLED_IRQ_ENABLE_1_CSR, mask1, data1);
         }
         break;
      case VRM_MODE:
         if (mask0 != 0)
         {
            HWIO_OUTM(RPMH_VRM_MODE_SETTLED_IRQ_ENABLE_0_CSR, mask0, data0);
         }
         if (mask1 != 0)
         {
            HWIO_OUTM(RPMH_VRM_MODE_SETTLED_IRQ_ENABLE_1_CSR, mask1, data1);
         }
         break;
      default:
         result = VRM_INVALID_PARAM;
         break;
   }

   if (mask0 != 0)
   {
      log_event = VRM_EVENT_SETTLED_IRQ_DISABLE;
      if (data0 != 0)
      {
         log_event = VRM_EVENT_SETTLED_IRQ_ENABLE;
      }
      vrm_log(vrm_ptr, VRM_RSRC_REGULATOR, rsrc_id, setting, &mask0, result, log_event);
   }

   if (mask1 != 0)
   {
      log_event = VRM_EVENT_SETTLED_IRQ_DISABLE;
      if (data1 != 0)
      {
         log_event = VRM_EVENT_SETTLED_IRQ_ENABLE;
      }
      vrm_log(vrm_ptr, VRM_RSRC_REGULATOR, rsrc_id, setting, &mask1, result, log_event);
   }

   return result;
}

vrm_result vrm_hal_interrupt_settled_irq_clear(vrm_info* vrm_ptr, uint32 rsrc_id, uint32 setting)
{
   vrm_result result = VRM_SUCCESS;
   uint32 mask0 = 0;
   uint32 mask1 = 0;
   uint64 bit_mask = 0x1;
   uint64 mask = bit_mask << rsrc_id;

   // split the mask since there are 2 different registers
   vrm_hal_interrupt_split_mask(mask, &mask0, &mask1);

   switch ((vrm_settings)setting)
   {
      case VRM_ENABLE:
         if (mask0 != 0)
         {
            HWIO_OUT(RPMH_VRM_ENABLE_SETTLED_IRQ_CLEAR_0_CSR, mask0);
         }
         if (mask1 != 0)
         {
            HWIO_OUT(RPMH_VRM_ENABLE_SETTLED_IRQ_CLEAR_1_CSR, mask1);
         }
         break;
      case VRM_VOLTAGE:
         if (mask0 != 0)
         {
            HWIO_OUT(RPMH_VRM_VOLTAGE_SETTLED_IRQ_CLEAR_0_CSR, mask0);
         }
         if (mask1 != 0)
         {
            HWIO_OUT(RPMH_VRM_VOLTAGE_SETTLED_IRQ_CLEAR_1_CSR, mask1);
         }
         break;
      case VRM_HEADROOM:
         if (mask0 != 0)
         {
            HWIO_OUT(RPMH_VRM_HEADROOM_SETTLED_IRQ_CLEAR_0_CSR, mask0);
         }
         if (mask1 != 0)
         {
            HWIO_OUT(RPMH_VRM_HEADROOM_SETTLED_IRQ_CLEAR_1_CSR, mask1);
         }
         break;
      case VRM_MODE:
         if (mask0 != 0)
         {
            HWIO_OUT(RPMH_VRM_MODE_SETTLED_IRQ_CLEAR_0_CSR, mask0);
         }
         if (mask1 != 0)
         {
            HWIO_OUT(RPMH_VRM_MODE_SETTLED_IRQ_CLEAR_1_CSR, mask1);
         }
         break;
      default:
         result = VRM_INVALID_PARAM;
         break;
   }

   return result;
}

vrm_result vrm_hal_interrupt_dep_irq_status_read(vrm_info* vrm_ptr, uint32 setting, uint64* status)
{
   vrm_result result = VRM_SUCCESS;
   uint32 status0 = 0;
   uint32 status1 = 0;

   if (status == NULL)
   {
      result = VRM_NULL_PTR_ERROR;
   }
   else
   {
      switch ((vrm_settings)setting)
      {
         case VRM_ENABLE:
            status0 = HWIO_IN(RPMH_VRM_ENABLE_DEP_IRQ_STATUS_0_CSR);
            status1 = HWIO_IN(RPMH_VRM_ENABLE_DEP_IRQ_STATUS_1_CSR);
            break;
         case VRM_VOLTAGE:
            status0 = HWIO_IN(RPMH_VRM_VOLTAGE_DEP_IRQ_STATUS_0_CSR);
            status1 = HWIO_IN(RPMH_VRM_VOLTAGE_DEP_IRQ_STATUS_1_CSR);
            break;
         case VRM_HEADROOM:
            status0 = HWIO_IN(RPMH_VRM_HEADROOM_DEP_IRQ_STATUS_0_CSR);
            status1 = HWIO_IN(RPMH_VRM_HEADROOM_DEP_IRQ_STATUS_1_CSR);
            break;
         case VRM_MODE:
            status0 = HWIO_IN(RPMH_VRM_MODE_DEP_IRQ_STATUS_0_CSR);
            status1 = HWIO_IN(RPMH_VRM_MODE_DEP_IRQ_STATUS_1_CSR);
            break;
         default:
            result = VRM_INVALID_PARAM;
            break;
      }

      if (result == VRM_SUCCESS)
      {
         *status = status1;
         *status = (*status << 32) | (status0);
      }
   }

   return result;
}

vrm_result vrm_hal_interrupt_dep_irq_enable_read(vrm_info* vrm_ptr, uint32 setting, uint64* status)
{
   vrm_result result = VRM_SUCCESS;
   uint32 status0 = 0;
   uint32 status1 = 0;

   if (status == NULL)
   {
      result = VRM_NULL_PTR_ERROR;
   }
   else
   {
      switch ((vrm_settings)setting)
      {
         case VRM_ENABLE:
            status0 = HWIO_IN(RPMH_VRM_ENABLE_DEP_IRQ_ENABLE_0_CSR);
            status1 = HWIO_IN(RPMH_VRM_ENABLE_DEP_IRQ_ENABLE_1_CSR);
            break;
         case VRM_VOLTAGE:
            status0 = HWIO_IN(RPMH_VRM_VOLTAGE_DEP_IRQ_ENABLE_0_CSR);
            status1 = HWIO_IN(RPMH_VRM_VOLTAGE_DEP_IRQ_ENABLE_1_CSR);
            break;
         case VRM_HEADROOM:
            status0 = HWIO_IN(RPMH_VRM_HEADROOM_DEP_IRQ_ENABLE_0_CSR);
            status1 = HWIO_IN(RPMH_VRM_HEADROOM_DEP_IRQ_ENABLE_1_CSR);
            break;
         case VRM_MODE:
            status0 = HWIO_IN(RPMH_VRM_MODE_DEP_IRQ_ENABLE_0_CSR);
            status1 = HWIO_IN(RPMH_VRM_MODE_DEP_IRQ_ENABLE_1_CSR);
            break;
         default:
            result = VRM_INVALID_PARAM;
            break;
      }

      if (result == VRM_SUCCESS)
      {
         *status = status1;
         *status = (*status << 32) | (status0);
      }
   }

   return result;
}

vrm_result vrm_hal_interrupt_change_irq_status_read(vrm_info* vrm_ptr, uint32 setting, uint64* status)
{
   vrm_result result = VRM_SUCCESS;
   uint32 status0 = 0;
   uint32 status1 = 0;

   if (status == NULL)
   {
      result = VRM_NULL_PTR_ERROR;
   }
   else
   {
      switch ((vrm_settings)setting)
      {
         case VRM_ENABLE:
            status0 = HWIO_IN(RPMH_VRM_ENABLE_CHANGE_IRQ_STATUS_0_CSR);
            status1 = HWIO_IN(RPMH_VRM_ENABLE_CHANGE_IRQ_STATUS_1_CSR);
            break;
         case VRM_VOLTAGE:
            status0 = HWIO_IN(RPMH_VRM_VOLTAGE_CHANGE_IRQ_STATUS_0_CSR);
            status1 = HWIO_IN(RPMH_VRM_VOLTAGE_CHANGE_IRQ_STATUS_1_CSR);
            break;
         case VRM_HEADROOM:
            status0 = HWIO_IN(RPMH_VRM_HEADROOM_CHANGE_IRQ_STATUS_0_CSR);
            status1 = HWIO_IN(RPMH_VRM_HEADROOM_CHANGE_IRQ_STATUS_1_CSR);
            break;
         case VRM_MODE:
            status0 = HWIO_IN(RPMH_VRM_MODE_CHANGE_IRQ_STATUS_0_CSR);
            status1 = HWIO_IN(RPMH_VRM_MODE_CHANGE_IRQ_STATUS_1_CSR);
            break;
         default:
            result = VRM_INVALID_PARAM;
            break;
      }

      if (result == VRM_SUCCESS)
      {
         *status = status1;
         *status = (*status << 32) | (status0);
      }
   }

   return result;
}

vrm_result vrm_hal_interrupt_change_irq_enable_read(vrm_info* vrm_ptr, uint32 setting, uint64* status)
{
   vrm_result result = VRM_SUCCESS;
   uint32 status0 = 0;
   uint32 status1 = 0;

   if (status == NULL)
   {
      result = VRM_NULL_PTR_ERROR;
   }
   else
   {
      switch ((vrm_settings)setting)
      {
         case VRM_ENABLE:
            status0 = HWIO_IN(RPMH_VRM_ENABLE_CHANGE_IRQ_ENABLE_0_CSR);
            status1 = HWIO_IN(RPMH_VRM_ENABLE_CHANGE_IRQ_ENABLE_1_CSR);
            break;
         case VRM_VOLTAGE:
            status0 = HWIO_IN(RPMH_VRM_VOLTAGE_CHANGE_IRQ_ENABLE_0_CSR);
            status1 = HWIO_IN(RPMH_VRM_VOLTAGE_CHANGE_IRQ_ENABLE_1_CSR);
            break;
         case VRM_HEADROOM:
            status0 = HWIO_IN(RPMH_VRM_HEADROOM_CHANGE_IRQ_ENABLE_0_CSR);
            status1 = HWIO_IN(RPMH_VRM_HEADROOM_CHANGE_IRQ_ENABLE_1_CSR);
            break;
         case VRM_MODE:
            status0 = HWIO_IN(RPMH_VRM_MODE_CHANGE_IRQ_ENABLE_0_CSR);
            status1 = HWIO_IN(RPMH_VRM_MODE_CHANGE_IRQ_ENABLE_1_CSR);
            break;
         default:
            result = VRM_INVALID_PARAM;
            break;
      }

      if (result == VRM_SUCCESS)
      {
         *status = status1;
         *status = (*status << 32) | (status0);
      }
   }

   return result;
}

vrm_result vrm_hal_interrupt_settled_irq_status_read(vrm_info* vrm_ptr, uint32 setting, uint64* status)
{
   vrm_result result = VRM_SUCCESS;
   uint32 status0 = 0;
   uint32 status1 = 0;

   if (status == NULL)
   {
      result = VRM_NULL_PTR_ERROR;
   }
   else
   {
      switch ((vrm_settings)setting)
      {
         case VRM_ENABLE:
            status0 = HWIO_IN(RPMH_VRM_ENABLE_SETTLED_IRQ_STATUS_0_CSR);
            status1 = HWIO_IN(RPMH_VRM_ENABLE_SETTLED_IRQ_STATUS_1_CSR);
            break;
         case VRM_VOLTAGE:
            status0 = HWIO_IN(RPMH_VRM_VOLTAGE_SETTLED_IRQ_STATUS_0_CSR);
            status1 = HWIO_IN(RPMH_VRM_VOLTAGE_SETTLED_IRQ_STATUS_1_CSR);
            break;
         case VRM_HEADROOM:
            status0 = HWIO_IN(RPMH_VRM_HEADROOM_SETTLED_IRQ_STATUS_0_CSR);
            status1 = HWIO_IN(RPMH_VRM_HEADROOM_SETTLED_IRQ_STATUS_1_CSR);
            break;
         case VRM_MODE:
            status0 = HWIO_IN(RPMH_VRM_MODE_SETTLED_IRQ_STATUS_0_CSR);
            status1 = HWIO_IN(RPMH_VRM_MODE_SETTLED_IRQ_STATUS_1_CSR);
            break;
         default:
            result = VRM_INVALID_PARAM;
            break;
      }

      if (result == VRM_SUCCESS)
      {
         *status = status1;
         *status = (*status << 32) | (status0);
      }
   }

   return result;
}

vrm_result vrm_hal_interrupt_settled_irq_enable_read(vrm_info* vrm_ptr, uint32 setting, uint64* status)
{
   vrm_result result = VRM_SUCCESS;
   uint32 status0 = 0;
   uint32 status1 = 0;

   if (status == NULL)
   {
      result = VRM_NULL_PTR_ERROR;
   }
   else
   {
      switch ((vrm_settings)setting)
      {
         case VRM_ENABLE:
            status0 = HWIO_IN(RPMH_VRM_ENABLE_SETTLED_IRQ_ENABLE_0_CSR);
            status1 = HWIO_IN(RPMH_VRM_ENABLE_SETTLED_IRQ_ENABLE_1_CSR);
            break;
         case VRM_VOLTAGE:
            status0 = HWIO_IN(RPMH_VRM_VOLTAGE_SETTLED_IRQ_ENABLE_0_CSR);
            status1 = HWIO_IN(RPMH_VRM_VOLTAGE_SETTLED_IRQ_ENABLE_1_CSR);
            break;
         case VRM_HEADROOM:
            status0 = HWIO_IN(RPMH_VRM_HEADROOM_SETTLED_IRQ_ENABLE_0_CSR);
            status1 = HWIO_IN(RPMH_VRM_HEADROOM_SETTLED_IRQ_ENABLE_1_CSR);
            break;
         case VRM_MODE:
            status0 = HWIO_IN(RPMH_VRM_MODE_SETTLED_IRQ_ENABLE_0_CSR);
            status1 = HWIO_IN(RPMH_VRM_MODE_SETTLED_IRQ_ENABLE_1_CSR);
            break;
         default:
            result = VRM_INVALID_PARAM;
            break;
      }

      if (result == VRM_SUCCESS)
      {
         *status = status1;
         *status = (*status << 32) | (status0);
      }
   }

   return result;
}

vrm_result vrm_hal_interrupt_settling_wdog_irq_status_read(vrm_info* vrm_ptr, uint64* status)
{
   vrm_result result = VRM_SUCCESS;
   uint32 status0 = 0;
   uint32 status1 = 0;

   if (status == NULL)
   {
      result = VRM_NULL_PTR_ERROR;
   }
   else
   {
      status0 = HWIO_IN(RPMH_VRM_VREG_READY_WDOG_IRQ_STATUS_0_CSR);
      status1 = HWIO_IN(RPMH_VRM_VREG_READY_WDOG_IRQ_STATUS_1_CSR);

      *status = status1;
      *status = (*status << 32) | (status0);
   }

   return result;
}

vrm_result vrm_hal_interrupt_settling_wdog_irq_enable_read(vrm_info* vrm_ptr, uint64* status)
{
   vrm_result result = VRM_SUCCESS;
   uint32 status0 = 0;
   uint32 status1 = 0;

   if (status == NULL)
   {
      result = VRM_NULL_PTR_ERROR;
   }
   else
   {
      status0 = HWIO_IN(RPMH_VRM_VREG_READY_WDOG_IRQ_ENABLE_0_CSR);
      status1 = HWIO_IN(RPMH_VRM_VREG_READY_WDOG_IRQ_ENABLE_1_CSR);

      *status = status1;
      *status = (*status << 32) | (status0);
   }

   return result;
}

vrm_result vrm_hal_interrupt_settling_wdog_irq_clear(vrm_info* vrm_ptr, uint32 seq_id)
{
   vrm_result result = VRM_SUCCESS;
   uint32 mask0 = 0;
   uint32 mask1 = 0;
   uint64 bit_mask = 0x1;
   uint64 mask = 0;

   if (seq_id >= VRM_MAX_NUM_SEQCERS)
   {
      result = VRM_OUT_OF_BOUND_ERROR;
   }
   else
   {
      mask = bit_mask << seq_id;

      // split the mask since there are 2 different registers
      vrm_hal_interrupt_split_mask(mask, &mask0, &mask1);

      if (mask0 != 0)
      {
         // this is per sequencer
         HWIO_OUT(RPMH_VRM_VREG_READY_WDOG_IRQ_CLEAR_0_CSR, mask0);
      }
      if (mask1 != 0)
      {
         // this is per sequencer
         HWIO_OUT(RPMH_VRM_VREG_READY_WDOG_IRQ_CLEAR_1_CSR, mask1);
      }
   }

   return result;
}

vrm_result vrm_hal_interrupt_max_volt_irq_status_read(vrm_info* vrm_ptr, uint64* status)
{
   vrm_result result = VRM_SUCCESS;
   uint32 status0 = 0;
   uint32 status1 = 0;

   if (status == NULL)
   {
      result = VRM_NULL_PTR_ERROR;
   }
   else
   {
      status0 = HWIO_IN(RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_STATUS_0_CSR);
      status1 = HWIO_IN(RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_STATUS_1_CSR);

      *status = status1;
      *status = (*status << 32) | (status0);
   }

   return result;
}

vrm_result vrm_hal_interrupt_max_volt_irq_enable_read(vrm_info* vrm_ptr, uint64* status)
{
   vrm_result result = VRM_SUCCESS;
   uint32 status0 = 0;
   uint32 status1 = 0;

   if (status == NULL)
   {
      result = VRM_NULL_PTR_ERROR;
   }
   else
   {
      status0 = HWIO_IN(RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_ENABLE_0_CSR);
      status1 = HWIO_IN(RPMH_VRM_MAX_VOLTAGE_VIOLATION_IRQ_ENABLE_1_CSR);

      *status = status1;
      *status = (*status << 32) | (status0);
   }

   return result;
}

vrm_result vrm_hal_interrupt_ctt_overflow_irq_status_read(vrm_info* vrm_ptr, uint32* status)
{
   vrm_result result = VRM_SUCCESS;

   if (status == NULL)
   {
      result = VRM_NULL_PTR_ERROR;
   }
   else
   {
      *status = HWIO_IN(RPMH_VRM_CTT_OVERFLOW_IRQ_STATUS_CSR);
   }

   return result;
}

vrm_result vrm_hal_interrupt_pmic_arb_failure_irq_status_read(vrm_info* vrm_ptr, uint32* status)
{
   vrm_result result = VRM_SUCCESS;

   if (status == NULL)
   {
      result = VRM_NULL_PTR_ERROR;
   }
   else
   {
      *status = HWIO_IN(RPMH_VRM_S_PORT_FAILURE_IRQ_STATUS_CSR);
   }

   return result;
}

