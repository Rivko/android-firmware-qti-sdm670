/*========================================================================
  This file contains high level functions that are shared inside afe
  across drivers and services

  Copyright (c) 2009-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 
  $Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/common/src/AFEChStatusUtils.cpp#1 $
 ====================================================================== */

#include "AFEDeviceDriver.h"
#include "adsp_afe_service_commands.h"
#include "AFEChStatusUtils.h"

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

#define CH_STATUS_DATA_FORMAT_MASK     0x2
#define CH_STATUS_DATA_FORMAT_SHIFT    1
#define CH_STATUS_SAMPLING_RATE_MASK   0xf
#define CH_STATUS_BIT_WIDTH_MASK       0xf


ADSPResult afe_validate_chstatus_cfg(afe_param_id_chstatus_cfg_t *p_spdif_chstatus_ptr, chstatus_check_point_t *p_chsts_check_param)
{
   ADSPResult result = ADSP_EOK;
   uint16 data_format;
   uint32 sampling_freq = 0;
   uint32 word_length = 0;
   uint32 temp_mask;


   do
   {
      if((AFE_CH_STATUS_A != p_spdif_chstatus_ptr->ch_status_type) && (AFE_CH_STATUS_B != p_spdif_chstatus_ptr->ch_status_type))
      {
         result = ADSP_EBADPARAM;
         break;
      }
      
      if(p_spdif_chstatus_ptr->ch_stat_mask[0] & CH_STATUS_DATA_FORMAT_MASK)
      {
         data_format = (uint16) ((p_spdif_chstatus_ptr->ch_stat_buf[0] & CH_STATUS_DATA_FORMAT_MASK) >> CH_STATUS_DATA_FORMAT_SHIFT);
   
         if(data_format != p_chsts_check_param->data_format)
         {
            result = ADSP_EBADPARAM;
            break;
         }
      }

      temp_mask = p_spdif_chstatus_ptr->ch_stat_mask[3] & CH_STATUS_SAMPLING_RATE_MASK;

      if(CH_STATUS_SAMPLING_RATE_MASK == temp_mask)
      {
      
         sampling_freq = (uint32) (p_spdif_chstatus_ptr->ch_stat_buf[3] & CH_STATUS_SAMPLING_RATE_MASK) ;

         if(0x1 != sampling_freq) // 0001 in 27 24 Sampling Frequency not indicated 
         {
            switch (p_chsts_check_param->sample_rate)
            {
               case 22050:  // 0100 in 27 24
                  if(0x4 != sampling_freq)
                  {
                     result = ADSP_EBADPARAM;
                  }   
                  break;
               case 32000:  // 0011 in 27 24
                  if(0x3 != sampling_freq)
                  {
                     result = ADSP_EBADPARAM;
                  }   
                  break;
               case 44100:  // 0000 in 27 24
                  if(0x0 != sampling_freq)
                  {
                     result = ADSP_EBADPARAM;
                  }      
                  break;
               case 88200:  // 1000 in 27..24
                  if(0x8 != sampling_freq)
                  {
                     result = ADSP_EBADPARAM;
                  }
                  break;
               case 176400: // 1100 in 27..24
                  if(0xc != sampling_freq)
                  {
                     result = ADSP_EBADPARAM;
                  }
                  break;
               case 48000:  // 0010 in 27..24 
                  if(0x2 != sampling_freq)
                  {
                     result = ADSP_EBADPARAM;
                  }
                  break;
               case 96000:  // 1010 in 27..24
                  if(0xa != sampling_freq)
                  {
                     result = ADSP_EBADPARAM;
                  }
                  break;
               case 192000: // 1110 in 27..24
                  if(0xe != sampling_freq)
                  {
                     result = ADSP_EBADPARAM;
                  }
                  break;
               default:
                  result = ADSP_EBADPARAM;
                  break;
            }
         }
         
         if(ADSP_EOK != result)
         {
            break;
         } 
      }
      else if(0 != temp_mask) // incorrect mask.
      {
         result = ADSP_EBADPARAM;
         break;
      }
      
      temp_mask = p_spdif_chstatus_ptr->ch_stat_mask[4] & CH_STATUS_BIT_WIDTH_MASK;

      if(CH_STATUS_BIT_WIDTH_MASK == temp_mask)
      {
         word_length = p_spdif_chstatus_ptr->ch_stat_buf[4] & CH_STATUS_BIT_WIDTH_MASK;
      
         if(24 == p_chsts_check_param->bit_width)
         {  // 1  0 1 1
            if(0xb != word_length)
            {
               result = ADSP_EBADPARAM;
               break;
            }
         }
         else if(16 == p_chsts_check_param->bit_width)
         {
            if((0 != word_length) && (0x2 != word_length))  // 0 0 1 0  35..32 16bit case. 
            {
               result = ADSP_EBADPARAM;
               break;
            }
         }
      }
      else if(0 != temp_mask)
      {
         result = ADSP_EBADPARAM;
         break;
      }

   }while(0);

   return result;
}




