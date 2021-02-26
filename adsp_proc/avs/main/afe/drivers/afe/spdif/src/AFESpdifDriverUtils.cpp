/*========================================================================
This file contains util functions for the AFE Spdif driver

  Copyright (c) 2009-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 
  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/spdif/src/AFESpdifDriverUtils.cpp#1 $
 ====================================================================== */



#include "AFESpdifDriver_i.h"
#include "AFESpdifDriverUtils_i.h"
#include "halspdif.h"
#include "LPASS_ADDRESS_FILE_SPDIF.h"
#include "avtimer_drv_api.h"

#define SPDIF_BUFFER_USE_DDR 0x1
#define NUM_OF_SUB_BUFF      0x2

#define SPDIF_BUFFER_4BYTE_ALIGNMENT  0x3     //for 4 bytes alignment
#define SPDIF_BUFFER_16BYTE_ALIGNMENT 0xF     //for 16 bytes alignment
#define SPDIF_BUFFER_32BYTE_ALIGNMENT 0x1F    //for 32 bytes alignment
#define SPDIF_ALIGN(r,a) ( ((r)+((a))) & ~((uint32)(a)) )

#define NUM_SAMPLING_RATES 14  

uint32 spdif_sampling_rate_info[NUM_SAMPLING_RATES][2] = {
                            {8000, 4194304000U}, // 1000000 / 8000 = 125 Q25 =xxx xxxx. xxxxxxxxxxxxx => 0xfa00 0000 = 4194304000
                            {11025,3043485895U}, // 7bit integer 125.00000 (25 bits fraction.) // to get fraction part. divide by 2 and then take out the integer part till the end.
                            {12000,2796202667U},
                            {16000,2097152000U},
                            {22050,1521742948U},
                            {24000,1398101333U},
                            {32000,1048576000U},
                            {44100,760871474U},
                            {48000,699050667U},  //20.83333333..... = 20.83333333            8333333 / 2 => 4 
                            {88200,380435737U},  //for HDMI only
                            {96000,349525333U},  //for HDMI only
                            {144000,233016889U}, //for HDMI only
                            {176400,190217868U}, //for HDMI only
                            {192000,174762667U}, //for HDMI only
                            };


ADSPResult afe_spdif_device_cfg_init(lpasshwio_prop_spdif_struct_t * p_spdiftx_hw_info)
{
   DALSYSPropertyVar                prop_var;
   lpasshwio_prop_spdif_struct_t    *p_spdif_prop_dev_cfg = NULL;
   const char                        *dev_id="SPDIFTX";

   DALSYS_PROPERTY_HANDLE_DECLARE(p_handle);
  
   /* Get device handle */
   if( DAL_SUCCESS != DALSYS_GetDALPropertyHandleStr(dev_id, p_handle) )
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_SPDIF DevCfg: Error getting VFR device handle");
      return ADSP_EFAILED;
   }
   else
   {
      /* Read property ptr */
      if( DAL_SUCCESS != DALSYS_GetPropertyValue(p_handle, "SpdifTxPropStructPtr", 0, &prop_var) )
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_SPDIF DevCfg: Error getting VFR Property Struct Ptr");
         return ADSP_EFAILED;
      }   
      else
      {
         p_spdif_prop_dev_cfg = (lpasshwio_prop_spdif_struct_t *)prop_var.Val.pStruct;

         p_spdiftx_hw_info->hw_revision = p_spdif_prop_dev_cfg->hw_revision;
         /* VFR hw revision */
         if (0 == p_spdiftx_hw_info->hw_revision)
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_SPDIF DevCfg: Not HW supported");
            return ADSP_EUNSUPPORTED;
         }
         /* SPDIF register base address */
         p_spdiftx_hw_info->base_reg_phy_addr = p_spdif_prop_dev_cfg->base_reg_phy_addr;
         /* SPDIF register page size */
         p_spdiftx_hw_info->reg_size = p_spdif_prop_dev_cfg->reg_size;
         /* SPDIF reset register base address */
         p_spdiftx_hw_info->reset_reg_phy_addr = p_spdif_prop_dev_cfg->reset_reg_phy_addr;
         /* SPDIF reset register page size */
         p_spdiftx_hw_info->reset_reg_size = p_spdif_prop_dev_cfg->reset_reg_size;
         /* SPDIF num of hw buffer */
         p_spdiftx_hw_info->num_hw_buf = p_spdif_prop_dev_cfg->num_hw_buf; 
         /* SPDIF interrupt # */
         p_spdiftx_hw_info->int_irq_no = p_spdif_prop_dev_cfg->int_irq_no;
      }
   }
   
   return ADSP_EOK;
}

ADSPResult afe_spdif_start(afe_spdif_driver_t *p_spdif_driver, spdif_state_struct_t *p_dev_state)
{
   ADSPResult result = ADSP_EOK;
   afe_spdif_hw_buf_info_t *p_hw_buf_info = &p_spdif_driver->spdif_hw_buf_info;

   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"INFO: SPDIF Programming registers");

   hal_spdif_soft_reset();

   //clear all subbuffers
   hal_spdif_tx_clear_all();

   //configure bypass mode if it is non-linear 
   if(AFE_NON_LINEAR_DATA == p_dev_state->data_format)
   {
        //set bypass mode.
         hal_spdif_write_tx_nonlinear_bypass(TRUE);
   }

   //configure frame size
   spdif_tx_cfg_t tx_cfg;
   memset(&tx_cfg,0,sizeof(spdif_tx_cfg_t));
   tx_cfg.frame_size = p_hw_buf_info->hw_buf_length_per_int; /* based on the buffer size info*/
   tx_cfg.en_inh_wtts = FALSE;
   tx_cfg.drop_late_frames = FALSE;
   hal_spdif_write_tx_cfg(tx_cfg);

   //fill the channel status buffer always. The selection will be done by port selection configuratoin..
      hal_spdif_write_tx_chsts_a(p_dev_state->tx_ch_sts_buf_a);
      hal_spdif_write_tx_chsts_b(p_dev_state->tx_ch_sts_buf_b);
   
   // configure spdif port proporty : channel status, user data, parity 
   spdif_port_sel_t tx_port_sel;
   memset(&tx_port_sel,0,sizeof(spdif_port_sel_t));
   if((AFE_CH_STATUS_A | AFE_CH_STATUS_B) == p_dev_state->channel_status_type)
   {
      tx_port_sel.ch_sts_e = SPDIF_CH1_STS_A_CH2_STS_B;
   }
   else if(AFE_CH_STATUS_A == p_dev_state->channel_status_type)
   {
      tx_port_sel.ch_sts_e = SPDIF_CH1_AND_CH2_STS_A;
   }
   else if(AFE_CH_STATUS_B == p_dev_state->channel_status_type)
   {
      tx_port_sel.ch_sts_e = SPDIF_CH1_AND_CH2_STS_B;
   }
   else //if there is no channel status config has been called, then use ch status buffer A as default.
   {
      tx_port_sel.ch_sts_e = SPDIF_CH1_AND_CH2_STS_A;
   }
   
   tx_port_sel.usr_data_e = SPDIF_USR_DATA_UNUSED;
   tx_port_sel.parity_e = TRUE;
   hal_spdif_write_tx_portsel(tx_port_sel);

   // configure spdif port proporty : channel status, user data, parity 
   spdif_tx_port_cfg_t tx_port_cfg; 
   memset(&tx_port_cfg,0,sizeof(spdif_tx_port_cfg_t));
   if(24 == p_dev_state->bit_width)
   {
      tx_port_cfg.bit_width = SPDIF_BIT_WIDTH_24;
   }
   else if(16 == p_dev_state->bit_width)
   {
      tx_port_cfg.bit_width = SPDIF_BIT_WIDTH_16;
   }

   if(1 == p_dev_state->num_channels) // default value is stereo
   {
      tx_port_cfg.mono = TRUE; //mono or stereo number of channel;
   }
   
   hal_spdif_write_tx_portcfg(tx_port_cfg);
   
   /*memory packing in spdif tx
       *16bit stereo
       * MSB (31:16) LSB (15:0)
       * left1              right1
       *16bit mono
       * MSB (31:16) LSB (15:0)
       * Sample 1       Sample2
       * 24bit stereo
       * Left1 
       * Right1
       * 24bit mono
       *Sample 1
       * Sample 2 */

   spdif_fifo_cntl_t tx_fifo_cntl;  
   memset(&tx_fifo_cntl,0,sizeof(spdif_fifo_cntl_t));
   tx_fifo_cntl.word_byte_swap = FALSE;  // no use case.

   if(16 == p_dev_state->bit_width)
   {
      tx_fifo_cntl.dword_word_swap = TRUE; // need to swap for 16bit width for both mono and stereo.
   }
   else //for 24bit case, it is the same order as hardware sample packing for both mono and stereo.
   {
      tx_fifo_cntl.dword_word_swap = FALSE; 
   }

   // The watermark is only working after underflow so that we need a workaround to resubmit buffer in case of small chuck of buffer.
   if(p_hw_buf_info->hw_buf_length_per_int <= SPDIF_FIFO_WATER_MARK_16)
   {
      tx_fifo_cntl.watermark = SPDIF_FIFO_WATER_MARK_1;
   }
   else
   {
      tx_fifo_cntl.watermark = SPDIF_FIFO_WATER_MARK_8;
   }
   tx_fifo_cntl.fifo_pre_buf_en = TRUE;
   tx_fifo_cntl.fifo_jump_rd_ptr = FALSE;
   tx_fifo_cntl.left_justified = TRUE; //left justified
   hal_spdif_write_tx_fifocntl(tx_fifo_cntl);

   //init & clear interrupt stat and configure interrupt.
   p_spdif_driver->int_stat.tx_block_done_int = 0;
   p_spdif_driver->int_stat.fifo_uflow_int= 0;
   p_spdif_driver->int_stat.ts_diff_int = 0;
   p_spdif_driver->int_stat.mc_err_int= 0;


   spdif_int_cnt_t tx_int_cntl;
   memset(&tx_int_cntl,0,sizeof(spdif_int_cnt_t));
   tx_int_cntl.tx_block_done_int_e = TRUE;
   tx_int_cntl.fifo_uflow_int_e = TRUE;
   tx_int_cntl.ts_diff_int_e = TRUE;
   tx_int_cntl.mc_err_int_e= TRUE;
   tx_int_cntl.int_mask= SPDIF_INT_MUX_Q6_EN_APPS_DIS;;
   hal_spdif_write_tx_int_cntl(tx_int_cntl);

   hal_spdif_write_int_sta_ack(SPDIF_ALL_INT);

   //preload all zero buffers to start the interface.
   p_spdif_driver->buffer_index = 0;

   uint32 * sub_buf_pntr = NULL;

   p_spdif_driver->start_timestamp = 0;

   for( p_spdif_driver->buffer_index = 0;  p_spdif_driver->buffer_index < p_hw_buf_info->num_sub_buf; p_spdif_driver->buffer_index++)
   {
      sub_buf_pntr = p_hw_buf_info->hw_buf_phys_addr + p_hw_buf_info->hw_buf_length_per_int*p_spdif_driver->buffer_index;
      hal_spdif_write_tx_subbuf_fifo(sub_buf_pntr, 0, 0);
      //TS DIFF in spdif is not working for now.
      hal_spdif_write_timestamp(p_spdif_driver->start_timestamp);
   }
   p_spdif_driver->buffer_index = 0;
   
   //enable spdif port.
   hal_spdif_tx_enable();

   return result;
}

ADSPResult afe_spdif_stop(void)
{

   ADSPResult result = ADSP_EOK;

   //disable all the interrupt and clear interrupt stat
   spdif_int_cnt_t tx_int_cntl;
   memset(&tx_int_cntl,0,sizeof(spdif_int_cnt_t));
   hal_spdif_write_tx_int_cntl(tx_int_cntl);

   hal_spdif_write_int_sta_ack(SPDIF_ALL_INT);

   //disable tx port.
   hal_spdif_tx_disable();

   return result;
}

void afe_spdif_deallocate_hardware_buf(afe_spdif_hw_buf_info_t *p_hw_buf_info)
{
   qurt_elite_memory_aligned_free((void *)p_hw_buf_info->hw_buf_virt_addr);
   p_hw_buf_info->allocInDDRFlag = 0;
   p_hw_buf_info->hw_buf_virt_addr = 0;
   p_hw_buf_info->hw_buf_phys_addr = 0;
   p_hw_buf_info->hw_buf_length_per_int = 0;
   p_hw_buf_info->num_samples_per_int = 0;
   p_hw_buf_info->hw_buf_total_length = 0;

}


ADSPResult afe_spdif_allocate_hardware_buf(spdif_state_struct_t *p_dev_state, afe_spdif_hw_buf_info_t *p_hw_buf_info)
{
   ADSPResult result = ADSP_EOK;
   int32 sample_width_inbytes = 0;
   uint32 int_samples_per_period = p_dev_state->int_samples_per_period;
   uint32 num_channels = p_dev_state->num_channels;
   int32 smaples_in_bytes = 0;
   uint32 align_size_inbyte = SPDIF_BUFFER_32BYTE_ALIGNMENT + 1;
   uint32_t buf_size_inbytes = 0;

   do
   {
      sample_width_inbytes =   (p_dev_state->bit_width > 16) ? 4 : 2;
      smaples_in_bytes = sample_width_inbytes * int_samples_per_period * num_channels;

      if( smaples_in_bytes & SPDIF_BUFFER_4BYTE_ALIGNMENT)
      {
         result = ADSP_EBADPARAM;
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE_SPDIF: memory alignement has failed, aborting");  
         break;
      }
      
      p_hw_buf_info->num_samples_per_int = int_samples_per_period;
      p_hw_buf_info->hw_buf_length_per_int = smaples_in_bytes >> 2; // Dword
      p_hw_buf_info->hw_buf_total_length = p_hw_buf_info->hw_buf_length_per_int * p_hw_buf_info->num_sub_buf; // double buffer scheme
      //p_hw_buf_info->num_sub_buf = NUM_OF_SUB_BUFF;
   }while(0);

   if(ADSP_EOK == result)
   {
      buf_size_inbytes = (p_hw_buf_info->hw_buf_total_length << 2);

      //make sure if the size of buffer is aligned to cache line size
      buf_size_inbytes = SPDIF_ALIGN(buf_size_inbytes, SPDIF_BUFFER_32BYTE_ALIGNMENT);
      
      p_hw_buf_info->hw_buf_virt_addr = (uint32*)qurt_elite_memory_aligned_malloc((uint32_t)buf_size_inbytes, align_size_inbyte, QURT_ELITE_HEAP_DEFAULT);

      if(NULL == p_hw_buf_info->hw_buf_virt_addr)
      {
         result = ADSP_ENORESOURCE;
      }
      else
      {
         p_hw_buf_info->allocInDDRFlag = SPDIF_BUFFER_USE_DDR;
         // have to query the phy address for LPM.
         p_hw_buf_info->hw_buf_phys_addr  = (uint32 *) qurt_lookup_physaddr( ( unsigned int )p_hw_buf_info->hw_buf_virt_addr );
         if(NULL == p_hw_buf_info->hw_buf_phys_addr)
         {
               result =  ADSP_EFAILED;
         }
         //init the whole buffer to zero.
         memset(p_hw_buf_info->hw_buf_virt_addr, 0, buf_size_inbytes);
         if(QURT_EOK != qurt_mem_cache_clean((qurt_addr_t)p_hw_buf_info->hw_buf_virt_addr, buf_size_inbytes, QURT_MEM_CACHE_FLUSH, QURT_MEM_DCACHE))
         {
            result = ADSP_EFAILED;
         }
      }
   }
   
   return result;
}



/* basic driver config info
        Sampling Rate, 
        Bit width, 
        Number of Channel, 
        data format

    Channel Status Information Per Channel
      o  Bit 0 = consumer mode (omitted because it has been fixed value)
      o  Bit 1  = Linear PCM vs. non-linear PCM (overlapped)
      o  Bit 2 = SW copyright asserted or not
      o  Bit 5:3 = more details on linear PCM or non-linear PCM modes
      o  Bit 7:6 = "00" indicates the following Mode0 table (omitted because it has been fixed value)
      o  Bit15:8 = Category code
      o  Bit16:19 = Source number
      o  Bit 23:20 = channel number 
      o  Bit 27:24 = sampling frequency (overlapped)
      o  Bit 32, 35:33 = word length (overlapped)
      o  Bit 42,41 = CGMSA content protection settings
   */

ADSPResult afe_spdif_init_chstatus(spdif_state_struct_t *p_dev_state)
{

   ADSPResult result = ADSP_EOK;
   uint32 data_format = p_dev_state->data_format;
   uint32 sampling_freq = 0;
   uint32 word_length = 0;
   spdif_ch_sts_buf_t *p_tx_ch_sts_buf_a = &p_dev_state->tx_ch_sts_buf_a;
   spdif_ch_sts_buf_t *p_tx_ch_sts_buf_b = &p_dev_state->tx_ch_sts_buf_b;


   switch (p_dev_state->sample_rate)
   {
   case 22050: // 0100
       sampling_freq = 0x4;
       break;      
     case 32000:  // 0011 in 27 24
       sampling_freq = 0x3;
       break;
   case 44100:  // 0000 in 27 24
       sampling_freq = 0x0;
       break;
   case 48000:  // 0010 in 27..24   
       sampling_freq = 0x2;
       break;
   case 88200:  // 1000 in 27..24
       sampling_freq = 0x8;
       break;
   case 96000:  // 1010 in 27..24
       sampling_freq = 0xa;
       break;
   case 176400: // 1100 in 27..24
       sampling_freq = 0xc;
       break;
   case 192000: // 1110 in 27..24
       sampling_freq = 0xe;
       break;
   default:
       break;
   }


   if(24 == p_dev_state->bit_width)
   {  // 1  0 1 1
      word_length = 0xb;
   }
   else // 16bits
   {
      word_length = 0;
   }

   uint32 ch_sts_buf0 = ( ((data_format<< 1) & 0x2) | ((sampling_freq << 24) & 0xf000000));
   uint32 ch_sts_buf1 = (word_length) & 0xf;

   p_tx_ch_sts_buf_a->ch_sts_buf0 = ch_sts_buf0;
   p_tx_ch_sts_buf_a->ch_sts_buf1 = ch_sts_buf1;
   p_tx_ch_sts_buf_a->ch_sts_buf2 = 0x0;
   p_tx_ch_sts_buf_a->ch_sts_buf3 = 0x0;
   p_tx_ch_sts_buf_a->ch_sts_buf4 = 0x0;
   p_tx_ch_sts_buf_a->ch_sts_buf5 = 0x0;
  
   p_tx_ch_sts_buf_b->ch_sts_buf0 = ch_sts_buf0;
   p_tx_ch_sts_buf_b->ch_sts_buf1 = ch_sts_buf1;
   p_tx_ch_sts_buf_b->ch_sts_buf2 = 0x0;
   p_tx_ch_sts_buf_b->ch_sts_buf3 = 0x0;
   p_tx_ch_sts_buf_b->ch_sts_buf4 = 0x0;
   p_tx_ch_sts_buf_b->ch_sts_buf5 = 0x0;

   return result;
}


/**
 * Get the time taken to transmit one sample at the sampling rate pointed to
 * by index. The format of the time will be returned in the variable format. If
 * the format is Q25 then format will be set to 25.
 *
 * @param[in]  SampleRate   Index into the samplingRateInfo table 
 * @param[out] pTime   Pointer to samples per ms.
 * @param[out] pFormat Pointer to format
 */
void afe_spdif_gettime_per_sample(uint32 sample_rate, uint32 *p_time, int32 *p_format)
{
   int32 i;
   for(i=0; i < NUM_SAMPLING_RATES; i++)
   {
      if(sample_rate == spdif_sampling_rate_info[i][0])
      {
         *p_time = spdif_sampling_rate_info[i][1];
         *p_format = 25;
      }
   }
}



void afe_spdif_interrupt_callback(void * args )
{
   afe_spdif_driver_t *p_spdif_driver = (afe_spdif_driver_t *) args;
   afe_dev_port_t *afe_port_ptr = (afe_dev_port_t *) p_spdif_driver->dev_port_ptr;
   uint32 int_stat;
   afe_spdif_hw_buf_info_t *p_hw_buf_info = &p_spdif_driver->spdif_hw_buf_info;

   hal_spdif_get_int_sta(&int_stat);
   if (int_stat & SPDIF_UNDERFLOW_INT)
   {
      hal_spdif_write_int_sta_ack(SPDIF_UNDERFLOW_INT);
      p_spdif_driver->int_stat.fifo_uflow_int++;
   }
   
   if (int_stat & SPDIF_MC_ERR_INT)
   {
      hal_spdif_write_int_sta_ack(SPDIF_MC_ERR_INT);
      p_spdif_driver->int_stat.mc_err_int++;
   }

   if(int_stat & SPDIF_TS_DIFF_INT)
   {
      hal_spdif_write_int_sta_ack(SPDIF_TS_DIFF_INT);
      p_spdif_driver->int_stat.ts_diff_int++;
   }
   
   if (int_stat & SPDIF_BLOCK_DONE_INT)
   {
      hal_spdif_write_int_sta_ack(SPDIF_BLOCK_DONE_INT);
      p_spdif_driver->int_stat.tx_block_done_int++;

      if (AFE_PORT_STATE_RUN == afe_port_ptr->port_state)
      {
         afe_port_ptr->dynamic_thread_ptr->isr_context_counter++;
         qurt_elite_signal_send(&(afe_port_ptr->dma_signal));
      } 
      else if(AFE_PORT_STATE_CONFIG == afe_port_ptr->port_state) //if the port state is config state, it will lost the signal so that interrupt handler will submit another zero buffer to work around.
      {
         uint32 * sub_buf_pntr;
         sub_buf_pntr = p_hw_buf_info->hw_buf_phys_addr + p_hw_buf_info->hw_buf_length_per_int*p_spdif_driver->buffer_index;
         hal_spdif_write_tx_subbuf_fifo(sub_buf_pntr, 0, 0);
         //TS DIFF in spdif is not working for now.
         hal_spdif_write_timestamp(p_spdif_driver->start_timestamp);
         
         p_spdif_driver->buffer_index++;
         if(p_spdif_driver->buffer_index ==  p_hw_buf_info->num_sub_buf)
         {
            p_spdif_driver->buffer_index = 0;
         }

      }
   }
}



