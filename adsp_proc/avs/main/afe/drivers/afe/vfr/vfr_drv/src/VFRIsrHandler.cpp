/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/vfr/vfr_drv/src/VFRIsrHandler.cpp#2 $
$DateTime: 2017/11/23 06:25:06 $
$Author: pwbldsvc $
$Change: 14912369 $
$Revision: #2 $

FILE:     VFRIsrHandler.cpp

DESCRIPTION: VFR Interrupt Registration / Handling functions

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright (c) 2013-2014,2017 QUALCOMM Technologies, Incorporated.
All Rights Reserved.
QUALCOMM Technologies Proprietary. 
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/vfr/vfr_drv/src/VFRIsrHandler.cpp#2 $

when       who     what, where, why
--------   ---     -------------------------------------------------------


============================================================================*/

#include "VFRIsrHandler_i.h"
#include "common.h"
#include "AFERtProxyDriver.h"
#include "AfeTimeStampDriver.h"
#include "avtimer_drv_api.h"

/*==========================================================================
  Globals
 ========================================================================== */

/*==========================================================================
  Macro\Constant definitions
  ========================================================================== */
#define VFR_IST_STACK_SIZE_BYTES        1024


/*==========================================================================
  Function definition
  ========================================================================== */

void vfr_src_idx_0_isr_handler(void *arg)
{
   vfr_fastintr_callback((void *)&vfr_drv_global_stat.vfr_state_info[VFR_SRC_0_IDX]);
}

void vfr_src_idx_1_isr_handler(void *arg)
{
   vfr_fastintr_callback((void *)&vfr_drv_global_stat.vfr_state_info[VFR_SRC_1_IDX]);
}

/** The fast interrupt handler is a function taking an integer
    argument (the number of the interrupt that was observed), 
    and with no return value.
 */
void vfr_fastintr_callback(void *intr_cntxt)
{
   vfr_src_state_t *p_vfr_src_state = (vfr_src_state_t *)intr_cntxt;

   if (TRUE == p_vfr_src_state->is_vfr_enabled)
   {
      /* Read the current AV time stamp used for finding the VFR drift */
      if (FALSE == p_vfr_src_state->is_hw_latched)
      {
         p_vfr_src_state->cur_av_time_stamp = avtimer_drv_get_time(p_vfr_src_state->avt_drv_hdl);
      }

      /* Take snap shot of RT proxy port's drifts, s/w latched only */
      afe_rt_proxy_vfr_callback(p_vfr_src_state->vfr_src_index);

      /* Below signal need to be sent every VFR interrupt to send the VFR drift
         message to VFR clients
       */
      qurt_elite_signal_send(&(p_vfr_src_state->vfr_signal));
   }

   return;
}

ADSPResult vfr_register_callback(vfr_drv_state_t *p_vfr_drv, vfr_src_state_t *vfr_state_ptr, vfr_hw_info_t *vfr_hw_info)
{
   ADSPResult           result = ADSP_EOK;
   uint32               l2vic_num;
   char_t               vfr_int_thread_name[16];

   /* Mux configuration through register write, only the h/w latched VFR configures the mux */

   afe_ts_drv_config_vfr_ts_mux(HW_TS_INTR_TYPE_VFR,
                                vfr_state_ptr->vfr_prop.irq_mux_ctl_sel,
                                &vfr_state_ptr->mux_id);

   if (AFE_TS_INVALID_MUX_ID != vfr_state_ptr->mux_id)
   {
      vfr_state_ptr->is_hw_latched = TRUE;

      /* Check if VFR interrupt is default (Non Gated to LPASS) */
      if (IRQ_TYPE_DEFAULT == p_vfr_drv->hw_info.vfr_irq_type)
      {
         /* Fetch the L2VIC # as per the mux assignment, as VFR src listens to VFR mux output
            interrupt */
         l2vic_num = vfr_hw_info->available_l2vic_num[vfr_state_ptr->mux_id];
      }
      else /* VFR interrupt is gated via PDC/PMU */
      {
         /* Fetch the L2VIC # as per the VFR src index. VFR src listens to
            direct l2vic #, which is 1:1 mapped with VFR ID.
            Not registering to mux output l2vic # as interrupt cannot be
            registered against this ID as there is no PMU assigned to it. */
         l2vic_num = vfr_hw_info->available_l2vic_num[vfr_state_ptr->vfr_src_index];
      }
   }
   else
   {
      vfr_state_ptr->is_hw_latched = FALSE;

      /**TODO, populate the l2vic num as per GPIO query for 
       * external VFR, this conditional will be true only for 
       * TSTA kind of usage */
      l2vic_num = 0;
   }

   /* Save interrupt IRQ type in source obj */
   vfr_state_ptr->intr_prop.irq_type = p_vfr_drv->hw_info.vfr_irq_type; 

   /* Check if VFR interrupt is default (Non Gated to LPASS) */
   if (IRQ_TYPE_DEFAULT == vfr_state_ptr->intr_prop.irq_type)
   {
      /* register interrupt handler */
      qurt_elite_strl_cpy(vfr_int_thread_name, "vfrisr000", sizeof(vfr_int_thread_name));
      vfr_int_thread_name[6] = (l2vic_num & 0xF0 >> 4) + '0';
      vfr_int_thread_name[7] = (l2vic_num & 0xF) + '0';

      if (ADSP_FAILED(result = qurt_elite_interrupt_register(&vfr_state_ptr->vfr_isr_info, (uint16_t)l2vic_num,
                                                             vfr_state_ptr->vfr_isr_handler, (void *)NULL, vfr_int_thread_name, VFR_IST_STACK_SIZE_BYTES)))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "Failed to register vfr [%d] interrupt with status %d", l2vic_num, result);

         vfr_state_ptr->l2vic_num = VFR_IRQ_SRC_INVALID;

         return result;
      }
   }
   else /* VFR interrupt is gated via PDC/PMU */
   {
      /* PMU acts as h/w gate to incoming interrupts to LPASS, e.g.
         VFR intr from modem. This h/w config is not present on all the
         targets.
       
         PMU config is abstracted underneath DAL interrupt controller
         API. For VFR interrupt registeration, we need to use
         DAL interrupt controller API*/


      /* Populate the interrupt properties and register the interrupt */

      /* Interrupt callback */
      vfr_state_ptr->intr_prop.intr_cb = vfr_state_ptr->vfr_isr_handler;

      /* Interrupt L2VIC # */
      vfr_state_ptr->intr_prop.l2vic_int_vector = l2vic_num;

      /* Interrupt trigger type */
      vfr_state_ptr->intr_prop.trigger_type = DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER;

      /* For Warlock, IRQ type is PDC. In Warlock Bringup, VFR interrupt
       * started firing properly only after keeping interrupt type as
       * EDGE RISING. So, for PDC, making interrupt type as EDGE RISING */
      if(IRQ_TYPE_PDC == vfr_state_ptr->intr_prop.irq_type)
      {
        vfr_state_ptr->intr_prop.trigger_type = DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER;
      }

      /* Interrupt string, used when l2vic num is not known explicitly
         String num is picked from the IPCAT */
      vfr_state_ptr->intr_prop.p_intr_l2vic_string = NULL;

      if (ADSP_EOK != (result = afe_device_gated_intr_register(&vfr_state_ptr->intr_prop)))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "Failed to register vfr [%d] interrupt with status %d", l2vic_num, result);

         /* De-register to handle the failure */
         afe_device_gated_intr_deregister(&vfr_state_ptr->intr_prop);

         vfr_state_ptr->l2vic_num = VFR_IRQ_SRC_INVALID;

         return result;
      }
   }

   /* Save the l2vic num used for this VFR source object */
   vfr_state_ptr->l2vic_num = l2vic_num;

   return result;
}

ADSPResult vfr_deregister_callback(vfr_src_state_t *vfr_state_ptr)
{
   ADSPResult              result = ADSP_EOK;

   if (TRUE == vfr_state_ptr->is_initialized)
   {
      /* Check if VFR interrupt is default (Non Gated to LPASS) */
      if (IRQ_TYPE_DEFAULT == vfr_state_ptr->intr_prop.irq_type)
      {
         /* deregister interrupt with callback function. */
         if (ADSP_FAILED(result = qurt_elite_interrupt_unregister(&vfr_state_ptr->vfr_isr_info)))
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                  "Failed to unregister vfr [%d] interrupt with status %d", vfr_state_ptr->l2vic_num, result);
         }
      }
      else /* VFR interrupt is gated (via PDC / PMU etc.)*/
      {
         /* Deregister interrupt with callback function. */
         if (ADSP_FAILED(result = afe_device_gated_intr_deregister(&vfr_state_ptr->intr_prop)))
         {
            MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
                  "Failed to unregister vfr [%d] interrupt with status %d", vfr_state_ptr->l2vic_num, result);
         }
      }
   }
   else
   {
      result = ADSP_EFAILED;
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "VFR: Error not initialized [src_idx :%d]", vfr_state_ptr->vfr_src_index);
   }

   return result;
}
