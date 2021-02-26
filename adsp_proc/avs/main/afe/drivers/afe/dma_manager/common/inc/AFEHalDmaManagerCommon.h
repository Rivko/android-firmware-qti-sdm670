/** @file hal_dma_manager_i.h
    This module contains Audio Hardware interface Abstraction layer function
    declarations. This file publicizes enums defined by the HAL.

    Copyright (c) 2013 Qualcomm Technologies, Inc. (QTI).  All Rights Reserved.
    QUALCOMM Proprietary.  Export of this technology or software is regulated
    by the U.S. Government, Diversion contrary to U.S. law prohibited.
 *//*====================================================================== */

/*========================================================================
                               Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/dma_manager/common/inc/AFEHalDmaManagerCommon.h#1 $

when       who     what, where, why
------     ---    -------------------------------------------------------
07/22/13   RP     Created


========================================================================== */
#ifndef _AFE_HAL_DMA_MANAGER_COMMON_H_
#define _AFE_HAL_DMA_MANAGER_COMMON_H_

#include "AFEDmaManager_i.h"
//#include "AFEHalDmaManager_i.h"
#include "AFEHalDump.h"

#if defined(__cplusplus)
extern "C" {
#endif


  /*==========================================================================
  Structure definitions
  ========================================================================== */
  typedef struct hal_dma_config
  {

    uint32_t *buffer_start_addr;
    uint32_t buffer_len;
    uint32_t dma_int_per_cnt;  ///< number of Dwords transfered per dma interrupt.
    uint32_t wps_count;  ///< Sets the number of 32 bit words per audio sample
    uint32_t watermark;  ///< Set the workmark level for the DMA FIFO
    uint32_t burst_size;
    uint32_t ifconfig_dma_control;   // This is only for audio if which has more than one audio interfaces to be associated with dma. 
    uint32_t ifconfig_dma_control_int; /* This is only for  internal i2s which has more than one audio interfaces to be associated with dma. 
                                        Either iconfig_dma_control or iconfig_dma_control  only will be non zero at a time per dma.
                                        This wont be used for other interaces like PCM IF,, TDM, HDMI etc*/

  } hal_dma_config_t;


  ADSPResult afe_hal_dma_init(afe_dma_mgr_driver_state_t * afe_dma_mgr_ptr);

  ADSPResult afe_hal_dma_reset(uint32_t dma_type, uint32_t dma_dir, uint32_t dma_index);

  ADSPResult afe_hal_dma_enable_dma_channel(uint32_t dma_type, uint32_t dma_dir, uint32_t dma_index);

  ADSPResult afe_hal_dma_disable_dma_channel(uint32_t dma_type, uint32_t dma_dir, uint32_t dma_index);

  ADSPResult afe_hal_dma_config_dma_channel(uint32_t dma_type, uint32_t dma_dir, uint32_t dma_index, hal_dma_config_t *dma_config_ptr);

  ADSPResult afe_hal_dma_get_dma_curr_addr(uint32_t dma_type, uint32_t dma_dir, uint32_t dma_index, uint32_t *curr_addr);

  ADSPResult afe_hal_dma_disable_dma_interrupt(uint32_t dma_type, uint32_t dma_dir, uint32_t dma_index);

  ADSPResult afe_hal_dma_enable_dma_interrupt(uint32_t dma_type, uint32_t dma_dir, uint32_t dma_index);

  uint32_t afe_hal_dma_check_dma_interrupt(uint32_t dma_type, uint32_t dma_dir, uint32_t dma_index, uint32_t int_status, uint32_t int_status2);

  uint32_t afe_hal_dma_check_hdmi_interrupt(uint32_t dma_type, uint32_t int_status);

  ADSPResult afe_hal_dma_clear_dma_interrupt(uint32_t dma_type, uint32_t dma_dir, uint32_t dma_index);

  ADSPResult afe_hal_dma_read_interrupt_status(uint32_t dma_type, uint32 *int_status_ptr, uint32 *int_status2_ptr);

  ADSPResult afe_hal_dma_clear_interrupt(uint32_t dma_type, uint32_t int_status, uint32_t int_status2);

  ADSPResult afe_hal_dma_get_dma_int_stc(uint32_t dma_type, uint32_t dma_dir, uint16_t dma_idx, uint64 *p_stc);

  ADSPResult afe_hal_dma_get_dma_fifo_count(uint32_t dma_type, uint32_t dma_dir, uint32_t dma_idx, uint32 *fifo_cnt_ptr);


#ifdef AUDIOIF_REG_DUMP   
  ADSPResult afe_hal_dma_dump(uint32_t dma_type, uint32_t dma_dir, uint32_t dma_idx, uint32_t dump_seq);
#endif

#ifdef __cplusplus
}
#endif //__cplusplus

#endif /*_AFE_HAL_DMA_MANAGER_COMMON_H_*/
