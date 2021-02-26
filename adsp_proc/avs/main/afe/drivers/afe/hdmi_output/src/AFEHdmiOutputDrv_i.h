/*========================================================================
   This file contains declerations to program HDMI hardware registers

  Copyright (c) 2013 Qualcomm Technologies, Inc. (QTI).  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/hdmi_output/src/AFEHdmiOutputDrv_i.h#1 $
 ====================================================================== */

#ifndef AFE_HDMI_OUTPUT_DRV_I_H
#define AFE_HDMI_OUTPUT_DRV_I_H

#include "AFEDmaManager.h"
#include "lpasshwio_devcfg.h"

/*==========================================================================
  Macro Defines
  ========================================================================== */

#define MS_DMA_STREAM_ROUTE_SW_WORKAROUND_FOR_V1
//#undef MS_DMA_STREAM_ROUTE_SW_WORKAROUND_FOR_V1

#define HDMI_DRIVE_INVALID_VALUE                        -1

#define HDMI_8_CHANNELS                8

typedef enum 
{
    HDMI_DATA_SHIFT_INVALID = -1,
    HDMI_DATA_SHIFT_LEFT = 0 ,
    HDMI_DATA_SHIFT_RIGHT
} hdmi_data_shift_type;

typedef enum
{
   HDMI_DRIVER_DEINIT = 0,
   HDMI_DRIVER_INIT
}e_hdmi_driver_state_t;

enum
{
   HDMI_INPUT_LPM_BUFFER,
   HDMI_INPUT_DDR_BUFFER
};


//AFE HDMI DP device states
/**

HDMI DP device right state should be as below:

HDMI_DP_DEVICE_STATE_STOP

HDMI_DP_DEVICE_STATE_START

HDMI_DP_DEVICE_STATE_UNMUTE_START
HDMI_DP_DEVICE_STATE_UNMUTE_DONE

   ..... RUNNING ....

HDMI_DP_DEVICE_STATE_MUTE_START
HDMI_DP_DEVICE_STATE_MUTE_DONE

HDMI_DP_DEVICE_STATE_STOP

**/
typedef enum 
{
   HDMI_DP_DEVICE_STATE_INVALID = -1,
   HDMI_DP_DEVICE_STATE_STOP = 0,         ///<Init state.
   HDMI_DP_DEVICE_STATE_START = 1,

   HDMI_DP_DEVICE_STATE_UNMUTE_START   =  2,    
   HDMI_DP_DEVICE_STATE_UNMUTE_DONE   =  3,    

   HDMI_DP_DEVICE_STATE_MUTE_START   =  4,    
   HDMI_DP_DEVICE_STATE_MUTE_DONE   =  5,    

} hdmi_dp_device_state_t;
/*==========================================================================
  Structure definitions
  ========================================================================== */

typedef struct hdmi_output_dma_global_state
{
   int32                           index;       ///< portIndex associated with this dma channel.
   uint16                          port_id;
   void*                           dev_port_ptr;
   uint32_t                        undr_cnt;
   uint32_t                        err_cnt;
   volatile int64                  sample_cnt_in_period;  ///< sample out count in dwords.   
} hdmi_output_dma_global_state_t;

typedef struct hdmi_output_ch_sts_buf
{
    uint32 ch_sts_data0;
    uint32 ch_sts_data1;
    uint32 ch_sts_data2;
    uint32 ch_sts_data3;
    uint32 ch_sts_data4;
    uint32 ch_sts_data5;

    uint16_t buf_idx;
} hdmi_output_ch_sts_buf_t;

typedef struct hdmi_output_port_state
{
   afe_dev_avt_drift_t  hdmi_output_avt_drift;
   dma_device_handle  dma_dev_hdl;
   uint32_t  is_mute;
   uint32_t  mute_msg_sent;
   uint32_t  muting_int;
   ///cfg input cache
   // sampling rate
   int32 sample_rate;
   // bit width
   int32 bit_width;
   // number of channels
   int32 num_channels;
   //HDMI: channel allocation
   int32 channel_alloc;
   uint16_t data_type;
   uint16_t  stream_id;
   uint16_t  dma_idx;
   ///For HDMI multi-channel
   uint8  hdmi_ch_map[HDMI_8_CHANNELS+1];   ///< map for DMA buffer copy remap. get according to inChMapArray[], hdmiSpkrAlloc and table 20 in CEA-861-D
   ///For channel status
   hdmi_output_ch_sts_buf_t ch_sts_buf_0;   //use two buf swap mechanism for dynamic updating channel status
   hdmi_output_ch_sts_buf_t ch_sts_buf_1;
   uint32_t  channel_status_type;   ///indicate if the channel status is for channel A or B
   uint32_t  undr_cnt;
   uint32_t  err_cnt;
   volatile int64  sample_cnt_in_period;  ///< sample out count in dwords.
   uint32_t num_interrupts_to_ignore; //Indicates the number of interrupts to ignore due to DP engine FIFO
   void* p_ctx;         //packet msg pointer for Ddisplay Port
   hdmi_dp_device_state_t dp_device_state;
   //EVEN dp_device_state can be updated in two threads, i.e. AFESVC main thread and DMA interrupt trigger thread,
   //we do not need mutex protection here as there is no chance that the two threads can change dp_device_state concurrently.
   //This is guranteed by hardware.

} hdmi_output_port_state_t;


typedef struct afe_hdmi_output_driver
{
   e_hdmi_driver_state_t   driver_state;
   uint32_t                lpass_reg_virt_addr;           /* hdmi input main register virtual address mapping */
   qurt_mem_region_t       lpass_qmem_region;             /* hdmi input register mapping region */
   uint32_t                lpass_reset_reg_virt_addr;     /* hdmi input main register virtual address mapping */
   qurt_mem_region_t       lpass_reset_qmem_region;       /* hdmi input register mapping region */
   uint32_t                hdmi_core_reg_virt_addr;     /* hdmi input main register virtual address mapping */
   qurt_mem_region_t       hdmi_core_qmem_region;       /* hdmi input register mapping region */
   lpasshwio_prop_hdmi_output_struct_t  hdmi_hw_info; /* information from device config */
   bool_t is_interrupt_enabled;
   bool_t is_last_operation;
   uint32_t mmpm_handle;

   void* group_info_ptr;

   uint16_t dma_cnt;

#if defined(MS_DMA_STREAM_ROUTE_SW_WORKAROUND_FOR_V1)
   uint16_t  cur_stream_id_array[4];
   uint16_t  cur_dma_idx_array[4];
   int      cur_used_idx;
#endif //#if defined(MS_DMA_STREAM_ROUTE_SW_WORKAROUND_FOR_V1)

} afe_hdmi_output_driver_t;


typedef struct afe_hdmi_output_pack_info
{
   int32 in_sample_bit_width;
   int32 num_channels;
   uint16_t data_type;             //liner or non-linear
   uint32 *ch_status_table_ptr;    //not used now
   uint32 ca;
   uint8* hdmi_ch_map_ptr;
   uint16_t q_format_shift_factor;
}afe_hdmi_output_pack_info_t;

/*==========================================================================
  Function Declerations
  ========================================================================== */


/**
  @brief Callback routine for the HDMI Output interrupt

  @param[in]int_id id for the interrupt

  @return  none

*/
void afe_hdmi_output_int_callback(int int_id);


/**
  Copy data from client buffer to the DMA buffer

  @param[in]  dma_mgr_state_ptr         pointer to the DMA manager state information

  @return
  aDSP error code

  @dependencies
  None.
 */
void afe_hdmi_output_write_dma(void *afe_driver_context, uint8_t *src_address, uint32_t *dst_address);


ADSPResult afe_hdmi_output_stream_route_update(uint16_t port_id, uint16_t stream_id);


int32 afe_hdmi_output_fill_data_2ch(afe_hdmi_output_pack_info_t *pack_info_ptr, int32* dst, int16* src, int32 num_samples_to_copy, int32 cur_frame);

int32 afe_hdmi_output_fill_data_8ch(afe_hdmi_output_pack_info_t *pack_info_ptr, int32* dst, int16* src, int32 num_samples_to_copy, int32 cur_frame);

int32 afe_hdmi_output_fill_data_2ch_dp(afe_hdmi_output_pack_info_t *pack_info_ptr, int32* dst, int16* src, int32 num_samples_to_copy, int32 cur_frame);

int32 afe_hdmi_output_fill_data_8ch_dp(afe_hdmi_output_pack_info_t *pack_info_ptr, int32* dst, int16* src, int32 num_samples_to_copy, int32 cur_frame);

ADSPResult afe_hdmi_output_init_chstatus(hdmi_output_port_state_t *p_dev_state);

uint32 afe_hdmi_output_get_present_bits(uint32 ca);


#endif //AFE_HDMI_OUTPUT_DRV_I_H



