/*========================================================================
  ELite

  This file contains AFE TDM slot operation

  Copyright (c) 2009-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 *//*====================================================================== */

/*========================================================================
  Edit History

    $Header: //source/qcom/qct/multimedia2/Audio/elite/static_svcs/AFESvc/...

    when       who         what, where, why
    --------   ---         -------------------------------------------------------
   01/20/14    FZ       initial version

  ========================================================================== */
/*==========================================================================
  Include files
  ========================================================================== */

#include "AFEPortManagement.h" 
#include "AFETdmSlot.h"

#include "AFEPortManager.h"



/* ============================================================================
    Constant / Define Declarations
============================================================================ */

/*==========================================================================
  Function Definitions
========================================================================== */
ADSPResult afe_slot_mapping_rx(void *p_dev_port, bool_t *p_header_filled)
{
   uint32_t  num_port_bytes_all_channel;
   uint16_t  i;
   int8_t    *p_dst_buf_8, *p_src_buf_8;
   int8_t    *p_dst_buf_8_cur, *p_src_buf_8_cur;
   int32_t   *p_src_buf_32;
   int32_t   temp_data;
   int8_t    *p_temp_data_8;
   uint16_t  samp_cnt = 0;
   uint16_t  size_in_byte;
  
   afe_dev_port_t *afe_port_ptr = (afe_dev_port_t *)p_dev_port;

   afe_group_device_state_struct_t   *p_group_device = (afe_group_device_state_struct_t *)afe_port_ptr->p_group_device_state;

   num_port_bytes_all_channel = (afe_port_ptr->channels * afe_port_ptr->bytes_per_channel);

/* Port interleaved output in pDevPort->mem_start_addr_copy buffer */

   //1st frame in group
   //2nd frame in group
   //1st frame in group
   //2nd frame in group
   //1st frame in group
   //2nd frame in group

   //Data layout in p_group_io_buf
   
   //_____________________
   //|     frame 0        | samp_cnt = 0
   //|____________________|
   //|     frame 1        |
   //|____________________|
   //|     frame 0        | samp_cnt = 1
   //|____________________|
   //|     frame 1        |
   //|____________________|
   //|     frame 0        | samp_cnt = 2
   //|____________________|
   //|     frame 1        | 
   //|____________________|
   //|     frame 0        | samp_cnt = 3
   //|____________________|
   //|     frame 1        |
   //|____________________|

   //Copy from AFE Port mem_start_addr_copy to p_group_device->p_group_io_buf
   //SOURCE::The bitwidth from bit_width@afe_port_i2s_cfg_t (-->afe_port_ptr->bytes_per_channel) is used to count PCM data for one channel from AFE port memory.
   //        It supports 16 and 24.
   //
   //DEST::afe_port_ptr->slot_mapping_info.slot_bytes_per_channel is used to count PCM data for one channel to p_group_io_buf
   //      It supports 16, 24, 32.
   //
   //SOURCE bit width = 16                                    DEST bit width = 16
   // _____________________                                    _____________________
   //|MSB 16 bit|LSB 16 bit|                                  |MSB 16 bit|LSB 16 bit|
   //|__________|__________|                                  |__________|__________|
   //
   //SOURCE bit width = 24                                    DEST bit width = 32
   // _____________________                                    _____________________
   //|  Q Format 24 bit    |                                  |MSB 24 bit       |0  |
   //|_____________________|                                  |_________________|___|
   //
   //SOURCE bit width = 24                                    DEST bit width = 24
   // _____________________                                              ____________
   //|Q Format 24 bit(SP1) |                                            | 16 BIT    |  SP1, MSB 16 bit
   //|_____________________|                                            |___________|
   //|Q Format 24 bit(SP2) |                                            | SP2 | SP1 |  8 bit LSB of SP1 + 8 bit MSB of SP2
   //|_____________________|                                            |_____|_____|
   //                                                                   | 16 BIT    |  SP2, LSB 16 bit
   //                                                                   |___________|
   // 


   //point to the right frame or SD lines
   //(afe_port_ptr->frame_idx) ---> point to which frame. 0 - frame_0; 1 - frame_1, ..
   
   //Point to start position in p_group_io_buf for this frame
   p_dst_buf_8 = p_group_device->p_group_io_buf + (afe_port_ptr->frame_idx * p_group_device->frame_size);  


   //Point to the start of source
   p_src_buf_8 = afe_port_ptr->working_buf_1;
   p_src_buf_32 = (int32_t*)afe_port_ptr->working_buf_1;


   for (samp_cnt = 0; samp_cnt < afe_port_ptr->int_samples_per_period; samp_cnt++)
   {
      if(2 == afe_port_ptr->bytes_per_channel)
      {
         //bitwidth is 16 bits
         //THE SOURCE DATA IS ALWAYS ONE-TO-ONE MAPPED WITH DST 
         //SOURCE memory is always CONTIGOUS
         //DST memory can be non-contiguous in channel based

         p_src_buf_8_cur = p_src_buf_8;

         for(i = 0; i < afe_port_ptr->channels; i++)
         {
            //PARI II  - copy LSB first
            if(0 != afe_port_ptr->slot_mapping_info.offset_info[i].copy_width_1)
            {
               p_dst_buf_8_cur = p_dst_buf_8 + afe_port_ptr->slot_mapping_info.offset_info[i].offset_1;
               size_in_byte = afe_port_ptr->slot_mapping_info.offset_info[i].copy_width_1;
               memscpy(p_dst_buf_8_cur, size_in_byte, p_src_buf_8_cur, size_in_byte); 

               //advance source point
               p_src_buf_8_cur += size_in_byte;
            }

            //PART I
            p_dst_buf_8_cur = p_dst_buf_8 + afe_port_ptr->slot_mapping_info.offset_info[i].offset_0;
            size_in_byte = afe_port_ptr->slot_mapping_info.offset_info[i].copy_width_0;
            memscpy(p_dst_buf_8_cur, size_in_byte, p_src_buf_8_cur, size_in_byte); 

            //advance source point
            p_src_buf_8_cur += size_in_byte;
         }
   
         p_src_buf_8 += num_port_bytes_all_channel;
      }
      else
      {
         //bitwidth is 24 bits
         //need to handle q_factor_shift

         //Support non-contiguous slot mapping case also

         //SOURCE memory is always 32 bit based CONTIGOUS

         for(i = 0; i < afe_port_ptr->channels; i++)
         {
            //shift data from AFE Port memory to MSB
            temp_data = (*p_src_buf_32) << afe_port_ptr->q_format_shift_factor;
            //temp_data has valid MSB 24 bit data

            if(4 == afe_port_ptr->slot_mapping_info.slot_bytes_per_channel)
            {
               //dst is 32 bit and src is only 24 bit valid
               //slot_mapping to copy in is 32 bitwidth case

               if(AFE_SLOT_MAPPING_DATA_ALIGN_LSB == afe_port_ptr->slot_mapping_info.data_align_type)
               {
                  //shift valid data to 24 bit LSB
                  temp_data = temp_data >> 8;
               }

               p_dst_buf_8_cur = p_dst_buf_8 + afe_port_ptr->slot_mapping_info.offset_info[i].offset_0;

               if(4 == afe_port_ptr->slot_mapping_info.offset_info[i].copy_width_0)
               {
                  memscpy(p_dst_buf_8_cur, 4, (int8_t*)&temp_data, 4);
               }
               else
               {
                  //need to handle split case
                  //TBD - handle DST address not two-byte alignmnet case
                  p_temp_data_8 = (int8_t*)&temp_data;     //point to source data

                  
                  p_src_buf_8_cur = p_temp_data_8; 

                  //copy LSB first
                  //PARI II
                  if(0 != afe_port_ptr->slot_mapping_info.offset_info[i].copy_width_1)
                  {
                     p_dst_buf_8_cur = p_dst_buf_8 + afe_port_ptr->slot_mapping_info.offset_info[i].offset_1;
                     size_in_byte = afe_port_ptr->slot_mapping_info.offset_info[i].copy_width_1;
                     memscpy(p_dst_buf_8_cur, size_in_byte, p_src_buf_8_cur, size_in_byte);

                     p_src_buf_8_cur += size_in_byte;
                  }

                  //PART I
                  p_dst_buf_8_cur = p_dst_buf_8 + afe_port_ptr->slot_mapping_info.offset_info[i].offset_0;
                  size_in_byte = afe_port_ptr->slot_mapping_info.offset_info[i].copy_width_0;
                  memscpy(p_dst_buf_8_cur, size_in_byte, p_src_buf_8_cur, size_in_byte); 

                  p_src_buf_8_cur += size_in_byte;
               }
            }
            else
            {
               //slot_mapping to copy in is 24 bitwidth case

               //shift valid data to LSB 3 bytes as memscpy() copy from LSB
	       if(AFE_BITS_PER_SAMPLE_24 == afe_port_ptr->bit_width)
	       {
                  temp_data = temp_data >> 8;
	       }

               if(3 == afe_port_ptr->slot_mapping_info.offset_info[i].copy_width_0)
               {
                  p_dst_buf_8_cur = p_dst_buf_8 + afe_port_ptr->slot_mapping_info.offset_info[i].offset_0;
                  memscpy(p_dst_buf_8_cur, 3, (int8_t*)&temp_data, 3);  //copy LSB 3 bytes from a 4 byte source data
               }
               else
               {
                  //need to handle split case
                  p_temp_data_8 = (int8_t*)&temp_data;     //point to source data

                  size_in_byte = afe_port_ptr->slot_mapping_info.offset_info[i].copy_width_0;

                  if(2 == size_in_byte)
                  {
                     p_dst_buf_8_cur = p_dst_buf_8 + afe_port_ptr->slot_mapping_info.offset_info[i].offset_0;
                     p_dst_buf_8_cur[0] = p_temp_data_8[1];
                     p_dst_buf_8_cur[1] = p_temp_data_8[2];

                     p_dst_buf_8_cur = p_dst_buf_8 + afe_port_ptr->slot_mapping_info.offset_info[i].offset_1;
                     p_dst_buf_8_cur[0] = p_temp_data_8[0];
                  }
                  else
                  {
                     p_dst_buf_8_cur = p_dst_buf_8 + afe_port_ptr->slot_mapping_info.offset_info[i].offset_0;
                     p_dst_buf_8_cur[0] = p_temp_data_8[2];

                     p_dst_buf_8_cur = p_dst_buf_8 + afe_port_ptr->slot_mapping_info.offset_info[i].offset_1;
                     p_dst_buf_8_cur[1] = p_temp_data_8[1];
                     p_dst_buf_8_cur[0] = p_temp_data_8[0];
                  }

               }
            }

            p_src_buf_32++;     //src pointer moves one DW or one channel
         }
      }

      //Move dst pointer
      p_dst_buf_8 += (p_group_device->frame_size * p_group_device->num_frame_in_group);
   }

   //handle custom header
   //limit to below case to avoid addtional check for normal cases
   if((afe_port_ptr->custom_header_info.is_cfg_received) &&  (FALSE  == *p_header_filled))
    {
       afe_tdm_custom_header_handle(p_dev_port);
       //set 
       *p_header_filled = TRUE;
    }

   return ADSP_EOK;
}

ADSPResult afe_slot_mapping_tx(void *p_dev_port)
{
   uint32_t   num_port_bytes_all_channel;
   uint16_t   i;
   int8_t    *p_dst_buf_8, *p_src_buf_8;
   int32_t    *p_dst_buf_32;
   int8_t    *p_src_buf_8_cur;
   int8_t    *p_dst_buf_8_cur;
   int32_t   temp_data;
   int8_t    *p_temp_data_8;
   uint16_t  samp_cnt = 0;
   uint16_t size_in_byte;

   int32_t   frame_sync_offset_cur = 0;

   afe_dev_port_t *afe_port_ptr = (afe_dev_port_t *)p_dev_port;
   afe_group_device_state_struct_t   *p_group_device = (afe_group_device_state_struct_t *)afe_port_ptr->p_group_device_state;

   num_port_bytes_all_channel = (afe_port_ptr->channels * afe_port_ptr->bytes_per_channel);

   p_src_buf_8 = p_group_device->p_group_io_buf + frame_sync_offset_cur + (afe_port_ptr->frame_idx * p_group_device->frame_size); // + afe_port_ptr->slot_mapping_info.slot_offset[0];
   
   p_dst_buf_8 = afe_port_ptr->working_buf_1;
   p_dst_buf_32 = (int32_t*)afe_port_ptr->working_buf_1;

   for (samp_cnt = 0; samp_cnt < afe_port_ptr->int_samples_per_period; samp_cnt++)
   {
      if(2 == afe_port_ptr->bytes_per_channel)
      {
         p_dst_buf_8_cur = p_dst_buf_8;

         for(i = 0; i < afe_port_ptr->channels; i++)
         {
            //copy LSB first
            if(0 != afe_port_ptr->slot_mapping_info.offset_info[i].copy_width_1)
            {
               p_src_buf_8_cur = p_src_buf_8 + afe_port_ptr->slot_mapping_info.offset_info[i].offset_1;
               size_in_byte = afe_port_ptr->slot_mapping_info.offset_info[i].copy_width_1;
               memscpy(p_dst_buf_8_cur, size_in_byte, p_src_buf_8_cur, size_in_byte);

               p_dst_buf_8_cur += size_in_byte;
            }

            p_src_buf_8_cur = p_src_buf_8 + afe_port_ptr->slot_mapping_info.offset_info[i].offset_0;
            size_in_byte = afe_port_ptr->slot_mapping_info.offset_info[i].copy_width_0;
            memscpy(p_dst_buf_8_cur, size_in_byte, p_src_buf_8_cur, size_in_byte); 

            p_dst_buf_8_cur += size_in_byte;
         }

         p_dst_buf_8 += num_port_bytes_all_channel;
      }
      else
      {
         //bitwidth is 24 bits
         //need to handle q_factor_shift
         for(i = 0; i < afe_port_ptr->channels; i++)
         {
            if(4 == afe_port_ptr->slot_mapping_info.slot_bytes_per_channel)
            {
               if(4 == afe_port_ptr->slot_mapping_info.offset_info[i].copy_width_0)
               {
                  p_src_buf_8_cur = p_src_buf_8 + afe_port_ptr->slot_mapping_info.offset_info[i].offset_0;
                  memscpy((int8_t*)&temp_data, 4, p_src_buf_8_cur, 4);
               }
               else
               {
                  temp_data = 0;

                  p_temp_data_8 = (int8_t *)&temp_data;

                  //copy LSB first
                  if(0 != afe_port_ptr->slot_mapping_info.offset_info[i].copy_width_1)
                  {
                     p_src_buf_8_cur = p_src_buf_8 + afe_port_ptr->slot_mapping_info.offset_info[i].offset_1;
                     size_in_byte = afe_port_ptr->slot_mapping_info.offset_info[i].copy_width_1;
                     memscpy(p_temp_data_8, size_in_byte, p_src_buf_8_cur, size_in_byte); 

                     p_temp_data_8 += size_in_byte;
                  }

                  p_src_buf_8_cur = p_src_buf_8 + afe_port_ptr->slot_mapping_info.offset_info[i].offset_0;
                  size_in_byte = afe_port_ptr->slot_mapping_info.offset_info[i].copy_width_0;
                  memscpy(p_temp_data_8, size_in_byte, p_src_buf_8_cur, size_in_byte); 

                  p_temp_data_8 += size_in_byte;
               }

               if(AFE_SLOT_MAPPING_DATA_ALIGN_LSB == afe_port_ptr->slot_mapping_info.data_align_type)
               {
                  //shift valid data to 24 bit MSB
                  temp_data = temp_data << 8;
               }

               *p_dst_buf_32 = temp_data >> afe_port_ptr->q_format_shift_factor;
            }
            else
            {
               if(3 == afe_port_ptr->slot_mapping_info.offset_info[i].copy_width_0)
               {
                  p_src_buf_8_cur = p_src_buf_8 + afe_port_ptr->slot_mapping_info.offset_info[i].offset_0;
                  memscpy((int8_t*)&temp_data, 3, p_src_buf_8_cur, 3);  //Copy 3 bytes from p_src_buf_8_cur to LSB 3 bytes of temp_data
               }
               else
               {
                  p_temp_data_8 = (int8_t *)&temp_data;

                  size_in_byte = afe_port_ptr->slot_mapping_info.offset_info[i].copy_width_0;

                  if(2 == size_in_byte)
                  {
                     p_src_buf_8_cur = p_src_buf_8 + afe_port_ptr->slot_mapping_info.offset_info[i].offset_0;

                     p_temp_data_8[1] = p_src_buf_8_cur[0];
                     p_temp_data_8[2] = p_src_buf_8_cur[1];

                     p_src_buf_8_cur = p_src_buf_8 + afe_port_ptr->slot_mapping_info.offset_info[i].offset_1;
                     p_temp_data_8[0] = p_src_buf_8_cur[0];
                  }
                  else
                  {
                     p_src_buf_8_cur = p_src_buf_8 + afe_port_ptr->slot_mapping_info.offset_info[i].offset_0;
                     p_temp_data_8[2] = p_src_buf_8_cur[0];

                     p_src_buf_8_cur = p_src_buf_8 + afe_port_ptr->slot_mapping_info.offset_info[i].offset_1;
                     p_temp_data_8[1] = p_src_buf_8_cur[1];
                     p_temp_data_8[0] = p_src_buf_8_cur[0];
                  }
               }

               temp_data = temp_data << 8;   //shift valid data to MSB 3 bytes
               *p_dst_buf_32 = temp_data >> afe_port_ptr->q_format_shift_factor;
            }

            p_dst_buf_32++;
         }
      }

      p_src_buf_8 += (p_group_device->frame_size * p_group_device->num_frame_in_group);
   }

   return ADSP_EOK;
}


