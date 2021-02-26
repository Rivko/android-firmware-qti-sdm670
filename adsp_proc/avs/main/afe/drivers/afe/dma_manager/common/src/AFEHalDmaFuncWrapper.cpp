/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/dma_manager/common/src/AFEHalDmaFuncWrapper.cpp#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     hal_dma_manager.cpp

DESCRIPTION: VFR Interrupt Registration / Handling functions

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright (c) 2013 - 2017 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/dma_manager/common/src/AFEHalDmaFuncWrapper.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
7/24/2013  RP      created
============================================================================*/

/*==========================================================================
  Include Files
========================================================================== */
#include "common.h"
#include <assert.h>
#include "qurt_elite.h"
#include "AFEDmaManager.h"
#include "AFEDmaManager_i.h"
#include "AFEHalDmaManager_i.h"
#include "AFEHalDmaManagerCommon.h"
#include "AFEHalDump.h"
#include "lpasshwio_devcfg.h"


static afe_hal_dma_func_def_t  afe_hal_func_def[LPASS_HW_DMA_MAX_TYPES];


/**
  @brief hal dma init 

  @param[in] h_dma_ptr pointer to DMA manager handle
  @param[in] open_params_ptr pointer to the parameter list DMA Manager registration

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_hal_dma_init(afe_dma_mgr_driver_state_t *afe_dma_mgr_ptr)
{
   ADSPResult  result = ADSP_EOK;
   uint32_t    dma_type;
   lpasshwio_prop_afe_dma_struct_t *dma_hw_info_ptr = NULL;

   memset(&afe_hal_func_def, 0, sizeof(afe_hal_dma_func_def_t) * LPASS_HW_DMA_MAX_TYPES);

   for (dma_type = 0; dma_type < LPASS_HW_DMA_MAX_TYPES; dma_type++)
   {
      dma_hw_info_ptr = &afe_dma_mgr_ptr->dma_hw_info[dma_type];

      if (0 != dma_hw_info_ptr->hw_revision)
      {
         switch (dma_type)
         {
            case LPASS_HW_DMA_TYPE0:
            {
               if ((LPASS_HW_VER_3_8_0 == dma_hw_info_ptr->hw_revision) ||
                   (LPASS_HW_VER_3_3_0 == dma_hw_info_ptr->hw_revision))
               {
                  hal_dma_init_type0_v1(dma_hw_info_ptr->int_output_index, &afe_hal_func_def[LPASS_HW_DMA_TYPE0],
                                        afe_dma_mgr_ptr->dma_virt_addr[dma_type], dma_hw_info_ptr);
               }
               else if (LPASS_HW_VER_4_0_0 == dma_hw_info_ptr->hw_revision)
               {
                  hal_dma_init_type0_v2(dma_hw_info_ptr->int_output_index, &afe_hal_func_def[LPASS_HW_DMA_TYPE0],
                                        afe_dma_mgr_ptr->dma_virt_addr[dma_type], dma_hw_info_ptr);
               }
               else if (LPASS_HW_VER_3_10_0 == dma_hw_info_ptr->hw_revision)
               {
                  hal_dma_init_type0_v3(dma_hw_info_ptr->int_output_index, &afe_hal_func_def[LPASS_HW_DMA_TYPE0],
                                        afe_dma_mgr_ptr->dma_virt_addr[dma_type], dma_hw_info_ptr);
               }
               else if (LPASS_HW_VER_4_0_2 == dma_hw_info_ptr->hw_revision)
               {
                  hal_dma_init_type0_v4(dma_hw_info_ptr->int_output_index, &afe_hal_func_def[LPASS_HW_DMA_TYPE0],
                                        afe_dma_mgr_ptr->dma_virt_addr[dma_type], dma_hw_info_ptr);
               }
               else if (LPASS_HW_VER_4_0_3 == dma_hw_info_ptr->hw_revision)
               {
                  hal_dma_init_type0_v5(dma_hw_info_ptr->int_output_index, &afe_hal_func_def[LPASS_HW_DMA_TYPE0],
                                        afe_dma_mgr_ptr->dma_virt_addr[dma_type], dma_hw_info_ptr);
               }
               else if (LPASS_HW_VER_4_1_0 == dma_hw_info_ptr->hw_revision)
               {
                  hal_dma_init_type0_v6(dma_hw_info_ptr->int_output_index, &afe_hal_func_def[LPASS_HW_DMA_TYPE0],
                                        afe_dma_mgr_ptr->dma_virt_addr[dma_type], dma_hw_info_ptr);
               }
               else if (LPASS_HW_VER_4_2_0 == dma_hw_info_ptr->hw_revision)
               {
                 /*warlock*/
                 hal_dma_init_type0_v7(dma_hw_info_ptr->int_output_index, &afe_hal_func_def[LPASS_HW_DMA_TYPE0],
                                        afe_dma_mgr_ptr->dma_virt_addr[dma_type], dma_hw_info_ptr);
               }
               else
               {
                  MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unkonwn dma Type 0 hw version [0x%x] ", dma_hw_info_ptr->hw_revision);
               }

               break;
            }
            case LPASS_HW_DMA_TYPE1:
            {
               int32_t dma_virt_offset = (afe_dma_mgr_ptr->dma_virt_addr[dma_type] - dma_hw_info_ptr->dma_reg_base) >> 2;

               if (LPASS_HW_VER_4_0_0 == dma_hw_info_ptr->hw_revision)
               {
                  hal_dma_init_type1(&afe_hal_func_def[LPASS_HW_DMA_TYPE1],
                                     dma_hw_info_ptr->dma_reg_base, dma_virt_offset);
               }
               else if ((LPASS_HW_VER_4_0_2 == dma_hw_info_ptr->hw_revision) ||
                        (LPASS_HW_VER_4_1_0 == dma_hw_info_ptr->hw_revision) ||
                        (LPASS_HW_VER_4_2_0 == dma_hw_info_ptr->hw_revision))
               {
                  hal_dma_init_type1_v2(&afe_hal_func_def[LPASS_HW_DMA_TYPE1],
                                        dma_hw_info_ptr->dma_reg_base, dma_virt_offset);
               }
               else
               {
                  MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "NOT SUPPORT dma Type 1 hw version [0x%x] ",
                        dma_hw_info_ptr->hw_revision);
               }
               break;
            }
            default:
            {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Unkonwn dma Type: [%lu]", dma_type);
               break;
            }
         } /* switch (dma_type) */
      } /* if (0 != dma_hw_info_ptr->hw_revision) */
   } /* End of for(;;)*/

   return result;
}

/**
  @brief hal dma reset 

  @param[in] h_dma_ptr pointer to DMA manager handle
  @param[in] open_params_ptr pointer to the parameter list DMA Manager registration

  @return  ADSP_EOK on success, an error code on error

 */
ADSPResult afe_hal_dma_reset(uint32_t dma_type, uint32_t dma_dir, uint32_t dma_index)
{
   ADSPResult result = ADSP_EOK;

   if (NULL != afe_hal_func_def[dma_type].afe_hal_dma_reset_fptr)
   {
      afe_hal_func_def[dma_type].afe_hal_dma_reset_fptr(dma_dir, dma_index);
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Dma Type [%d] Function Pointer afe_hal_dma_enable_dma_channel_fptr is Null", dma_type);
      result = ADSP_EUNEXPECTED;
   }

   return result;

}


ADSPResult afe_hal_dma_enable_dma_channel(uint32_t dma_type, uint32_t dma_dir, uint32_t dma_index)
{
   ADSPResult result = ADSP_EOK;

   if (NULL != afe_hal_func_def[dma_type].afe_hal_dma_enable_dma_channel_fptr)
   {
      afe_hal_func_def[dma_type].afe_hal_dma_enable_dma_channel_fptr(dma_dir, dma_index);
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Dma Type [%d] Function Pointer afe_hal_dma_enable_dma_channel_fptr is Null", dma_type);
      result = ADSP_EUNEXPECTED;
   }

   return result;
}

ADSPResult afe_hal_dma_disable_dma_channel(uint32_t dma_type, uint32_t dma_dir, uint32_t dma_index)
{
   ADSPResult result = ADSP_EOK;

   if (NULL != afe_hal_func_def[dma_type].afe_hal_dma_disable_dma_channel_fptr)
   {
      afe_hal_func_def[dma_type].afe_hal_dma_disable_dma_channel_fptr(dma_dir, dma_index);
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Dma Type [%d] Function Pointer afe_hal_dma_disable_dma_channel_fptr is Null", dma_type);
      result = ADSP_EUNEXPECTED;
   }
   return result;
}

ADSPResult afe_hal_dma_config_dma_channel(uint32_t dma_type, uint32_t dma_dir, uint32_t dma_index, hal_dma_config_t *dma_config_ptr)
{
   ADSPResult result = ADSP_EOK;

   if (NULL != afe_hal_func_def[dma_type].afe_hal_dma_config_dma_channel_fptr)
   {
      afe_hal_func_def[dma_type].afe_hal_dma_config_dma_channel_fptr(dma_dir, dma_index, dma_config_ptr);
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Dma Type [%d] Function Pointer afe_hal_dma_config_dma_channel_fptr is Null", dma_type);
      result = ADSP_EUNEXPECTED;
   }
   return result;
}

ADSPResult afe_hal_dma_get_dma_curr_addr(uint32_t dma_type, uint32_t dma_dir, uint32_t dma_index, uint32_t *curr_addr)
{
   ADSPResult result = ADSP_EOK;

   if (NULL != afe_hal_func_def[dma_type].afe_hal_dma_get_dma_curr_addr_fptr)
   {
      afe_hal_func_def[dma_type].afe_hal_dma_get_dma_curr_addr_fptr(dma_dir, dma_index, curr_addr);
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Dma Type [%d] Function Pointer afe_hal_dma_get_dma_curr_addr_fptr is Null", dma_type);
      result = ADSP_EUNEXPECTED;
   }
   return result;
}

ADSPResult afe_hal_dma_disable_dma_interrupt(uint32_t dma_type, uint32_t dma_dir, uint32_t dma_index)
{
   ADSPResult result = ADSP_EOK;

   if (NULL != afe_hal_func_def[dma_type].afe_hal_dma_disable_dma_interrupt_fptr)
   {
      afe_hal_func_def[dma_type].afe_hal_dma_disable_dma_interrupt_fptr(dma_dir, dma_index);
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Dma Type [%d] Function Pointer afe_hal_dma_disable_dma_interrupt_fptr is Null", dma_type);
      result = ADSP_EUNEXPECTED;
   }
   return result;
}

ADSPResult afe_hal_dma_enable_dma_interrupt(uint32_t dma_type, uint32_t dma_dir, uint32_t dma_index)
{
   ADSPResult result = ADSP_EOK;

   if (NULL != afe_hal_func_def[dma_type].afe_hal_dma_enable_dma_interrupt_fptr)
   {
      afe_hal_func_def[dma_type].afe_hal_dma_enable_dma_interrupt_fptr(dma_dir, dma_index);
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Dma Type [%d] Function Pointer afe_hal_dma_enable_dma_interrupt_fptr is Null", dma_type);
      result = ADSP_EUNEXPECTED;
   }
   return result;
}

ADSPResult afe_hal_dma_clear_dma_interrupt(uint32_t dma_type, uint32_t dma_dir, uint32_t dma_index)
{
   ADSPResult result = ADSP_EOK;

   if (NULL != afe_hal_func_def[dma_type].afe_hal_dma_clear_dma_interrupt_fptr)
   {
      afe_hal_func_def[dma_type].afe_hal_dma_clear_dma_interrupt_fptr(dma_dir, dma_index);
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Dma Type [%d] Function Pointer afe_hal_dma_clear_dma_interrupt_fptr is Null", dma_type);
      result = ADSP_EUNEXPECTED;
   }
   return result;
}


// can not print error message
uint32_t afe_hal_dma_check_dma_interrupt(uint32_t dma_type, uint32_t dma_dir, uint32_t dma_index, uint32_t int_status, uint32_t int_status2)
{
   uint32_t event = 0;

   if (NULL != afe_hal_func_def[dma_type].afe_hal_dma_check_dma_interrupt_fptr)
   {
      event = afe_hal_func_def[dma_type].afe_hal_dma_check_dma_interrupt_fptr(dma_dir, dma_index, int_status, int_status2);
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Dma Type [%d] Function Pointer afe_hal_dma_check_dma_interrupt_fptr is Null", dma_type);
   }

   return event;
}

uint32_t afe_hal_dma_check_hdmi_interrupt(uint32_t dma_type, uint32_t int_status)
{
   uint32_t event = 0;

   if (NULL != afe_hal_func_def[dma_type].afe_hal_dma_check_hdmi_interrupt_fptr)
   {
      event = afe_hal_func_def[dma_type].afe_hal_dma_check_hdmi_interrupt_fptr(int_status);
   }

   return event;
}

// can not print error message
ADSPResult afe_hal_dma_read_interrupt_status(uint32_t dma_type, uint32 *int_status_ptr, uint32 *int_status2_ptr)
{
   ADSPResult result = ADSP_EOK;

   if (NULL != afe_hal_func_def[dma_type].afe_hal_dma_read_interrupt_status_fptr)
   {
      afe_hal_func_def[dma_type].afe_hal_dma_read_interrupt_status_fptr(int_status_ptr, int_status2_ptr);
   }
   else
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Dma Type [%d] Function Pointer afe_hal_dma_read_interrupt_status_fptr is Null", dma_type);
      result = ADSP_EUNEXPECTED;
   }
   return result;
}

// can not print error message
ADSPResult afe_hal_dma_clear_interrupt(uint32_t dma_type, uint32_t int_status, uint32_t int_status2)
{
   ADSPResult result = ADSP_EOK;

   if (NULL != afe_hal_func_def[dma_type].afe_hal_dma_clear_interrupt_fptr)
   {
      afe_hal_func_def[dma_type].afe_hal_dma_clear_interrupt_fptr(int_status, int_status2);
   }
   else
   {
      //MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Dma Type [%d] Function Pointer afe_hal_dma_disable_dma_interrupt_fptr is Null", dma_type);
      result = ADSP_EUNEXPECTED;
   }
   return result;
}

ADSPResult afe_hal_dma_get_dma_int_stc(uint32_t dma_type, uint32_t dma_dir, uint16_t dma_idx, uint64_t *p_stc)
{
   ADSPResult result = ADSP_EOK;

   if (NULL != afe_hal_func_def[dma_type].afe_hal_dma_get_dma_interrupt_stc_fptr)
   {
      afe_hal_func_def[dma_type].afe_hal_dma_get_dma_interrupt_stc_fptr(dma_dir, dma_idx, p_stc);
   }
   else
   {
      //MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Dma Type [%d] Function Pointer afe_hal_dma_get_dma_int_stc_fptr is Null", dma_type);
      result = ADSP_EUNEXPECTED;
   }
   return result;
}

ADSPResult afe_hal_dma_get_dma_fifo_count(uint32_t dma_type, uint32_t dma_dir, uint32_t dma_idx, uint32_t *fifo_cnt_ptr)
{
   ADSPResult result = ADSP_EOK;

   if (NULL != afe_hal_func_def[dma_type].afe_hal_dma_get_dma_fifo_count_fptr)
   {
      afe_hal_func_def[dma_type].afe_hal_dma_get_dma_fifo_count_fptr(dma_dir, dma_idx, fifo_cnt_ptr);
   }
   else
   {
      //MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Dma Type [%d] Function Pointer afe_hal_dma_get_dma_fifo_count_fptr is Null", dma_type);
      result = ADSP_EUNEXPECTED;
   }
   return result;
}


#ifdef AUDIOIF_REG_DUMP
ADSPResult afe_hal_dma_dump(uint32_t dma_type, uint32_t dma_dir, uint32_t dma_idx, uint32_t dump_seq)
{
   ADSPResult result = ADSP_EOK;

   if (NULL != afe_hal_func_def[dma_type].afe_hal_dma_dump_fptr)
   {
      afe_hal_func_def[dma_type].afe_hal_dma_dump_fptr(dma_dir, dma_idx, dump_seq);
   }
   else
   {
      //MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Dma Type [%d] Function Pointer afe_hal_dma_get_dma_fifo_count_fptr is Null", dma_type);
      result = ADSP_EUNEXPECTED;
   }
   return result;
}

#endif
