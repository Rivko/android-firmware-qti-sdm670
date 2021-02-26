#include "common.h"
#include <assert.h>
#include "qurt_elite.h"
#include "AFEDmaManager.h"
#include "AFEHalDmaManager_i.h"
#include "AFEHalDmaManagerCommon.h"


/*==========================================================================
  Function Definitions
========================================================================== */

void hal_dma_init_type0_v5(uint32_t intr_index, afe_hal_dma_func_def_t *func_table, uint32_t virt_base, lpasshwio_prop_afe_dma_struct_t *dma_hw_info_ptr)
{
  if(NULL == func_table)
  {
    MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "func_table is NULL ====");
    return;
  }

  //func_table->afe_hal_dma_init_fptr;
  func_table->afe_hal_dma_enable_dma_channel_fptr = NULL;
  func_table->afe_hal_dma_disable_dma_channel_fptr = NULL;
  func_table->afe_hal_dma_config_dma_channel_fptr = NULL;
  func_table->afe_hal_dma_get_dma_curr_addr_fptr = NULL;
  func_table->afe_hal_dma_disable_dma_interrupt_fptr = NULL;
  func_table->afe_hal_dma_enable_dma_interrupt_fptr = NULL;
  func_table->afe_hal_dma_clear_dma_interrupt_fptr = NULL;
  func_table->afe_hal_dma_get_dma_interrupt_stc_fptr   = NULL;
  func_table->afe_hal_dma_get_dma_fifo_count_fptr = NULL;

  // interrupt handling
  func_table->afe_hal_dma_check_dma_interrupt_fptr = NULL;
  func_table->afe_hal_dma_read_interrupt_status_fptr = NULL;
  func_table->afe_hal_dma_clear_interrupt_fptr = NULL;

  //ssr handling
  func_table->afe_hal_dma_reset_fptr  = NULL;

  MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "hal_dma_init_type0_v4 stub set to NULL");
}


