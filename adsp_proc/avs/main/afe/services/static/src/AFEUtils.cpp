/*========================================================================
   This file contains AFE utilities implementation

  Copyright (c) 2013 QUALCOMM Technologies, Inc. (QTI).  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/services/static/src/AFEUtils.cpp#2 $
 ====================================================================== */

/*==========================================================================
  Include files
  ========================================================================== */
#include "audio_basic_op.h"
#include "audio_basic_op_ext.h"
#include "Interleaver.h"
#include "AFEPortManagement.h"
#include "AFETdmDriver.h"
#include "dsdUtil.h"
#include "AFERivaDriver.h"

/*==========================================================================
  Function Definitions
========================================================================== */

/*=============================================================================
FUNCTION      void afe_port_multiply_saturate

DESCRIPTION   apply gain (Q13) to the input buffer (Q15), shift  and place the 
              result in the output buffer (Q15). Optimized for Q6 by loading 2
              values of input at the same time.
OUTPUTS

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS
===============================================================================*/
void afe_port_multiply_saturate(int16_t *outptr,   /* Pointer to output */
                                int16_t *inptr,    /* Pointer to input */
                                uint16_t gain,     /* Gain in Q13 format */
                                int32_t samples)   /* No of samples to which the gain is to be applied */
{
#ifdef __qdsp6__
   int32_t nGainQ16 = (Q6_R_zxth_R(gain)) << 3;   /* scale the gain up by 4 bit to become Q16 for ease of truncation*/
   int64_t nCombinedGain = Q6_P_combine_RR(nGainQ16, nGainQ16); /* combine int32_t into int64_t for vector operation*/

   int64_t nInputCombinedP1, nInputCombinedP2, nInputCombinedP3, nInputCombinedP4;
   int64_t temp1, temp2, temp3, temp4;

   // if output buffer doesn't start with 8 uint8_t (or 4 half-word) boundary, it must start with 2 uint8_t boundary
   while ((0 != (((uint32_t)outptr) & 0x6)) && (samples))
   {
      // when output buffer address doesn't end with 000, such as in 0b...xy0, x|y=1
      *outptr++ = s16_saturate_s32((int32_t)s64_mult_s16_u16_shift(*inptr++, gain, -13));
      samples--;
   }

   // The rest will be computed with two vector mults in a row, ie, 2 samples a packet, 4 samples per iteration
   int64_t *pALGN8OutPtr = (int64_t *)outptr;
   while (samples >= 8)
   {
      // apply gain to left channel
      nInputCombinedP1 = Q6_P_combine_RR(inptr[1], inptr[0]);
      nInputCombinedP2 = Q6_P_combine_RR(inptr[3], inptr[2]);
      nInputCombinedP3 = Q6_P_combine_RR(inptr[5], inptr[4]);
      nInputCombinedP4 = Q6_P_combine_RR(inptr[7], inptr[6]);
      samples -= 8;
      inptr += 8;

      temp1 = Q6_P_vmpyweh_PP_sat(nCombinedGain, nInputCombinedP1);
      temp2 = Q6_P_vmpyweh_PP_sat(nCombinedGain, nInputCombinedP2);
      temp3 = Q6_P_vmpyweh_PP_sat(nCombinedGain, nInputCombinedP3);
      temp4 = Q6_P_vmpyweh_PP_sat(nCombinedGain, nInputCombinedP4);

      *pALGN8OutPtr++ = Q6_P_combine_RR(Q6_R_vsatwh_P(temp2), Q6_R_vsatwh_P(temp1));
      *pALGN8OutPtr++ = Q6_P_combine_RR(Q6_R_vsatwh_P(temp4), Q6_R_vsatwh_P(temp3));
   }
   outptr =  (int16_t *)pALGN8OutPtr;
   // if there are less than 4 samples left,
   while (samples--)
   {
      *outptr++ = s16_saturate_s32((int32_t)s64_mult_s16_u16_shift(*inptr++, gain, -13));
   }
#else
   /*--------------------------------------- Unoptimized non q6 vesion-----------------------------------------------*/
   while (samples--)
   {
      *outptr++ = s16_saturate_s32((int32_t)s64_mult_s16_u16_shift(*inptr++, gain, -13));
   }
#endif
}

// right now the function in CSoftVolumeControlsLib is for Q12, not Q13 as we need it.
/*=============================================================================
FUNCTION      void afe_port_multiply_saturate_int32

DESCRIPTION   apply gain (Q13) to the input buffer (32-bit PCM), 
              output buffer (32-bit). 
OUTPUTS

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS
===============================================================================*/
void afe_port_multiply_saturate_int32(int32_t *outptr,   /* Pointer to output */
                                      int32_t *inptr,    /* Pointer to input */
                                      uint16_t gain,     /* Gain in Q13 format */
                                      int32_t samples)   /* No of samples to which the gain is to be applied */
{
   while (samples--)
   {
      *outptr++ = (int32_t)s32_saturate_s64(s64_mult_s16_u16_shift(*inptr++, gain, -13));
   }
}


/**
 * adds left to right and divides by 2 and result will be in monoOut.
 * doesn't modify left,right, but monoOut is modified.
 */
void afe_port_stereo2mono_by_avg(int8_t *left, int8_t *right, int8_t *monoOut, uint32_t numSamples, uint16_t bytes_per_channel)
{
   int32_t temp32;
   int64_t temp64;
   int16_t * left_ptr16,*right_ptr16,*mono_ptr16;
   int32_t * left_ptr32,*right_ptr32,*mono_ptr32;

   if (2 == bytes_per_channel)
   {
      left_ptr16 = (int16_t *)left;
      right_ptr16 = (int16_t *)right;
      mono_ptr16 = (int16_t *)monoOut;

      while (numSamples--)
      {
         temp32 = (int32_t)(*left_ptr16++);
         temp32 += (int32_t)(*right_ptr16++);
         (*mono_ptr16++) = (int16_t)(temp32 >> 1);
      }
   }
   else
   {
      left_ptr32 = (int32_t *)left;
      right_ptr32 = (int32_t *)right;
      mono_ptr32 = (int32_t *)monoOut;

      while (numSamples--)
      {
         temp64 = (int64_t)(*left_ptr32++);
         temp64 += (int64_t)(*right_ptr32++);
         (*mono_ptr32++) = (int32_t)(temp64 >> 1);
      }
   }
}

/*
 * interleaves samples in srcAddr and keeps it in dstAddr
 * interleave into as many numChannels
 * for a 4 channel 5 sample/channel signal : each literal is a sample.
 * interleaved :    L1 L2 R1 R2   L1 L2 R1 R2   L1 L2 R1 R2   L1 L2 R1 R2   L1 L2 R1 R2
 * deinterleaved :  L1 L1 L1 L1 L1   L2 L2 L2 L2 L2   R1 R1 R1 R1 R1   R2 R2 R2 R2 R2
 *
 */
void afe_port_interleave_samples(int8_t *src_addr, int8_t *dst_addr, int16_t num_channels,
                                 uint32_t num_samples_per_ch, int16_t bytes_per_channel)
{
   int16_t     nChan;
   uint32_t    sampIndex;
   int16_t     * src_addr_ptr16,*dst_addr_ptr16;
   int32_t     * src_addr_ptr32,*dst_addr_ptr32;

   if (1 == num_channels)
   {
      memscpy(dst_addr,
              num_samples_per_ch * bytes_per_channel,
              src_addr,
              num_samples_per_ch * bytes_per_channel);
   }
   else
   {
      if (2 == bytes_per_channel)
      {
         src_addr_ptr16 = (int16_t *)src_addr;
         dst_addr_ptr16 = (int16_t *)dst_addr;

         if (2 == num_channels)
         {
            Interleave_16(src_addr_ptr16, (src_addr_ptr16 + num_samples_per_ch), dst_addr_ptr16, num_samples_per_ch);
         }
         else
         {
            //general code
            for (nChan = 0; nChan < num_channels; nChan++)
            {
               for (sampIndex = 0; sampIndex < num_samples_per_ch; sampIndex++)
               {
                  *(dst_addr_ptr16 + sampIndex * num_channels + nChan) = *(src_addr_ptr16 + nChan * num_samples_per_ch + sampIndex);
               }
            }
         }
      }
      else
      {
         src_addr_ptr32 = (int32_t *)src_addr;
         dst_addr_ptr32 = (int32_t *)dst_addr;

         if (num_channels == 2)
         {
            Interleave_32(src_addr_ptr32, (src_addr_ptr32 + num_samples_per_ch), dst_addr_ptr32, num_samples_per_ch);
         }
         else
         {
            //general code
            for (nChan = 0; nChan < num_channels; nChan++)
            {
               for (sampIndex = 0; sampIndex < num_samples_per_ch; sampIndex++)
               {
                  *(dst_addr_ptr32 + sampIndex * num_channels + nChan) = *(src_addr_ptr32 + nChan * num_samples_per_ch + sampIndex);
               }
            }
         }
      }
   }
}

/*
 * deinterleaves samples in src_addr and keeps it in dst_addr
 * deinterleave into as many numChannels
 */
void afe_port_de_interleave_samples(int8_t *src_addr, int8_t *dst_addr, int16_t num_channels,
                                    uint32_t num_samples_per_ch, int16_t bytes_per_channel)
{
   int16_t     nChan;
   uint32_t    sampIndex;
   int16_t     * src_addr_ptr16,*dst_addr_ptr16;
   int32_t     * src_addr_ptr32,*dst_addr_ptr32;

   if (num_channels == 1)
   {
      memscpy(dst_addr,
              num_samples_per_ch * bytes_per_channel,
              src_addr,
              num_samples_per_ch * bytes_per_channel);
   }
   else
   {
      if (2 == bytes_per_channel)
      {
         src_addr_ptr16 = (int16_t *)src_addr;
         dst_addr_ptr16 = (int16_t *)dst_addr;

         if (num_channels == 2)
         {
            DeInterleave_16(src_addr_ptr16, dst_addr_ptr16, (dst_addr_ptr16 + num_samples_per_ch), num_samples_per_ch);
         }
         else
         {
            /* general code */
            for (nChan = 0; nChan < num_channels; nChan++)
            {
               for (sampIndex = 0; sampIndex < num_samples_per_ch; sampIndex++)
               {
                  *(dst_addr_ptr16 + nChan * num_samples_per_ch + sampIndex) = *(src_addr_ptr16 + sampIndex * num_channels + nChan);
               }
            }
         }
      }
      else //4 bytes per channel
      {
         src_addr_ptr32 = (int32_t *)src_addr;
         dst_addr_ptr32 = (int32_t *)dst_addr;

         if (num_channels == 2)
         {
            DeInterleave_32(src_addr_ptr32, dst_addr_ptr32, (dst_addr_ptr32 + num_samples_per_ch), num_samples_per_ch);
         }
         else
         {
            //general code
            for (nChan = 0; nChan < num_channels; nChan++)
            {
               for (sampIndex = 0; sampIndex < num_samples_per_ch; sampIndex++)
               {
                  *(dst_addr_ptr32 + nChan * num_samples_per_ch + sampIndex) = *(src_addr_ptr32 + sampIndex * num_channels + nChan);
               }
            }
         }
      }
   }
}

/**
 * This function is for up down conversion of bytes per sample 
 * for the data buf pointed to by ptr_src_buf. 
 *  
 * @param[in] ptr_src_buf, ptr to source buf to be converted, 
 *       assumes channel de-interleaved data
 * @param[in] ptr_out_buf, pointer to output buffer, asumes 
 *       de-interleaved data, should be sufficiently large to
 *       hold the output data.
 * @param[in] num_samples_per_ch, number of samples per channel 
 * @param[in] num_channels, Number of channels
 * @param[in] chan_spacing_in, spacing between adjacent channels 
 *       for input buffers
 * @param[in] chan_spacing_out, spacing between adjacent 
 *       channels for output buffers
 * @param[in] conv_mode, up/down conversion mode 
 *  
 * NOTE : Both 16->32 bit conversion and 32->16 bit conversion
 * can be both in-place or non-inplace 
 *  
 * For 16->32 bit conversion, output buffer should be 
 * sufficiently large to hold the up converted data. 
 *  
 * @return none
 */
void afe_port_sample_up_down_convertor(afe_dev_port_t *pDevPort, int8_t *ptr_src_buf,
                                       int8_t *ptr_out_buf, uint32_t num_samp_per_ch,
                                       int16_t num_channels, uint32_t chan_spacing_in,
                                       uint32_t chan_spacing_out, uint32_t client_bit_width,
                                       uint16_t conv_mode)
{
   int16_t   samp, nChan;
   int32_t   * buf_ptr_src1_32,*buf_ptr_32,*buf_ptr_dst_32,*buf_ptr_dst1_32,*buf_ptr_src_32;
   int16_t   * buf_ptr_16,*buf_ptr_src_16,*buf_ptr_dst_16;
   int64_t   *buf_ptr_64;
   uint32_t	 q_format_shift_factor = QFORMAT_SHIFT_FACTOR;
   uint32_t  byte_up_conv_shift_fact = BYTE_UP_CONV_SHIFT_FACT_DEFAULT;
   uint32_t  byte_down_conv_rnd_fact = BYTE_DOWN_CONV_RND_FACT_DEFAULT;
   uint32_t  byte_down_conv_shift_fact = BYTE_DOWN_CONV_SHIFT_FACT_DEFAULT;

   /* if the conversion is from/to 32bit to/from 24bit */
   if (((32 == pDevPort->bit_width) && (24 == client_bit_width)) || ((24 == pDevPort->bit_width) && (32 == client_bit_width)))
   {
      /* update the rnd and shift factors accordingly */
      byte_down_conv_rnd_fact = BYTE_DOWN_CONV_RND_FACT_24_BIT;
      byte_down_conv_shift_fact = BYTE_DOWN_CONV_SHIFT_FACT_24_BIT;
      byte_up_conv_shift_fact = BYTE_UP_CONV_SHIFT_FACT_24_BIT;

      //   MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "32bit to/from 24bit: Port 0x%x: port bit width %d client_bit_width %ld",pDevPort->intf, pDevPort->bit_width, client_bit_width);

      /* 32-to-24 bit conversion */
      if (BYTE_DOWN_CONV == conv_mode)
      {
         buf_ptr_src_32 = (int32_t *)ptr_src_buf;
         buf_ptr_dst_32 = (int32_t *)ptr_out_buf;
#ifdef __qdsp6__

         for (nChan = 0; nChan < num_channels; nChan++)
         {
            buf_ptr_src1_32 = (buf_ptr_src_32 + nChan * chan_spacing_in);            /* Input buffer  */
            buf_ptr_dst1_32 = (buf_ptr_dst_32 + nChan * chan_spacing_out);            /* Output buffer */

            /* Convert from 32bit to 24bit , inplace conversion   */
            for (samp = 0; samp < (int16_t)num_samp_per_ch; samp++)
            {
               *buf_ptr_dst1_32++ = (int32_t)(Q6_R_add_RR_sat((*buf_ptr_src1_32++), byte_down_conv_rnd_fact) >> byte_down_conv_shift_fact);
            }
         }
#else
         /*----------- Non Q6 Version ---------------*/
         for (nChan = 0; nChan < num_channels; nChan++)
         {
            buf_ptr_src1_32 = (buf_ptr_src_32 + nChan * chan_spacing_in);            /* Input buffer  */
            buf_ptr_dst1_32 = (buf_ptr_dst_32 + nChan * chan_spacing_out);                /* Output buffer */

            /* Convert from 32bit to 24bit , inplace conversion   */
            for (samp = 0; samp < num_samp_per_ch; samp++)
            {
               *buf_ptr_dst1_32++ = (int32_t)(((*buf_ptr_src1_32++) + byte_down_conv_rnd_fact) >> byte_down_conv_shift_fact);
            }
         }
#endif /* __qdsp6__ */

      }
      else // 24bit to 32bit, NO inplace conversion,
      {
         buf_ptr_src_32  = (int32_t *)ptr_src_buf;
         buf_ptr_dst_32 = (int32_t *)ptr_out_buf;

         for (nChan = 0; nChan < num_channels; nChan++)
         {
            buf_ptr_src1_32 = (buf_ptr_src_32 + nChan * chan_spacing_in);            /* Input buffer  */
            buf_ptr_dst1_32 = (buf_ptr_dst_32 + nChan * chan_spacing_out);      /* Output buffer */

            /* Convert from Q27 to Q31, conversion output in scratch buffer */
            for (samp = 0; samp < (int16_t)num_samp_per_ch; samp++)
            {
               *buf_ptr_dst1_32++ = s32_shl_s32_sat(*buf_ptr_src1_32++,
                                                    byte_up_conv_shift_fact);
            }
         }
      }
   }
   else
   {
      //  MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "32/24bit to/from 16bit: Port 0x%x: port bit width %d client_bit_width %ld",pDevPort->intf, pDevPort->bit_width, client_bit_width);
      /* 32/24-to-16 bit conversion */
      if (BYTE_DOWN_CONV == conv_mode)
      {
         buf_ptr_src1_32 = (int32_t *)ptr_src_buf;
         buf_ptr_dst_16 = (int16_t *)ptr_out_buf;

         /* Shift factor need to be updated for 32bit usecase */
         if ((32 == pDevPort->bit_width) || (32 == client_bit_width))
         {
            q_format_shift_factor = 0;
         }

         for (nChan = 0; nChan < num_channels; nChan++)
         {
            buf_ptr_32 = (buf_ptr_src1_32 + nChan * chan_spacing_in);            /* Input buffer  */
            buf_ptr_16 = (buf_ptr_dst_16 + nChan * chan_spacing_out);                /* Output buffer */

            /* Convert from Q31/Q28 to Q15 , inplace conversion   */
            for (samp = 0; samp < (int16_t)num_samp_per_ch; samp++)
            {
               *buf_ptr_16++ = (int16_t)(s32_add_s32_s32_sat(s32_shl_s32_sat(*buf_ptr_32++, q_format_shift_factor),
                                                             byte_down_conv_rnd_fact) >> byte_down_conv_shift_fact);
            }
         }
      }
      else // Q15 -> Q28/Q31, inplace conversion,
      {
         buf_ptr_src_16 = (int16_t *)ptr_src_buf;
         buf_ptr_src1_32 = (int32_t *)ptr_out_buf;

         /* Shift factor need to be updated for 32bit usecase */
         if ((32 == pDevPort->bit_width) || (32 == client_bit_width))
         {
            byte_up_conv_shift_fact = BYTE_UP_CONV_SHIFT_FACT_16_BIT;
         }

#ifdef __qdsp6__

         /* If the number of samples per channel is odd, then we will have alignment problem for the 2nd, 4th, etc channel.*/
         if ((num_samp_per_ch & 0x1) || (chan_spacing_in & 0x1) || (chan_spacing_out & 0x1))
         {
            for (nChan = (num_channels - 1); nChan >= 0; nChan--)
            {
               buf_ptr_16 = (buf_ptr_src_16 + nChan * chan_spacing_in + (num_samp_per_ch - 1));            /* Input buffer  */
               buf_ptr_32 = (buf_ptr_src1_32 + nChan * chan_spacing_out + (num_samp_per_ch - 1));      /* Output buffer */

               /* Convert from Q15 to Q28, conversion output in scratch buffer */
               for (samp = num_samp_per_ch; samp > 0; samp--)
               {
                  *buf_ptr_32-- = ((int32_t)(*buf_ptr_16--)) << byte_up_conv_shift_fact;
               }
            }
         }
         else
         {
            for (nChan = (num_channels - 1); nChan >= 0; nChan--)
            {
               buf_ptr_32 = (int32_t *)(buf_ptr_src_16 + nChan * chan_spacing_in + (num_samp_per_ch - 2));            /* Input buffer  */
               buf_ptr_64 = (int64_t *)(buf_ptr_src1_32 + nChan * chan_spacing_out + (num_samp_per_ch - 2));      /* Output buffer */

               /* Convert from Q15 to Q28/Q31, conversion output in scratch buffer  */
               for (samp = num_samp_per_ch; samp >= 2; samp -= 2)
               {
                  /* Sign extend two 16-bit words in to two 32-bit words  */
                  *buf_ptr_64 = Q6_P_vsxthw_R(*buf_ptr_32--);

                  /* Shift left the result to convert it to Q28  */
                  *buf_ptr_64 = Q6_P_vaslw_PR(*buf_ptr_64, byte_up_conv_shift_fact);
                  buf_ptr_64--;
               }
            }
         }
#else
         /*----------- Non Q6 Version ---------------*/
         for (nChan = (num_channels - 1); nChan >= 0; nChan--)
         {
            buf_ptr_16 = (buf_ptr_src_16 + nChan * chan_spacing_in + (num_samp_per_ch - 1));            /* Input buffer  */
            buf_ptr_32 = (buf_ptr_src1_32 + nChan * chan_spacing_out + (num_samp_per_ch - 1));      /* Output buffer */

            /* Convert from Q15 to Q28/Q31, conversion output in scratch buffer */
            for (samp = num_samp_per_ch; samp > 0; samp--)
            {
               *buf_ptr_32-- = ((int32_t)(*buf_ptr_16--)) << byte_up_conv_shift_fact;
            }
         }
#endif /* __qdsp6__ */
      }
   }
}

ADSPResult afe_svc_get_port(uint16 unPortId, afe_dev_port_t **pDevPort)
{
   ADSPResult result = ADSP_EOK;

   if (IS_AUDIO_IF_PORT_AFE_ID(unPortId))
   {
      *pDevPort = afe_svc_global_ptr->ports_audioif_ptr[GET_AUDIOIF_PORT_INDEX(unPortId)];
   }
   else if (IS_AUDIO_IF_TDM_PORT_AFE_ID(unPortId))
   {
      afe_tdm_get_dev_port(unPortId, pDevPort);
   }
   else if (IS_RIVA_PORT_AFE_ID(unPortId))
   {
      *pDevPort = afe_svc_global_ptr->ports_riva_ptr[GET_RIVA_PORT_INDEX(unPortId)];
   }
   else if (IS_SLIMBUS_PORT_AFE_ID(unPortId))
   {
      *pDevPort = afe_svc_global_ptr->ports_slimbus_ptr[GET_SLIMBUS_PORT_INDEX(unPortId)];
   }
   else if (IS_USBA_PORT_AFE_ID(unPortId))
   {
      *pDevPort = afe_svc_global_ptr->ports_usba_ptr[GET_USBA_PORT_INDEX(unPortId)];
   }
   else if (IS_RT_PROXY_PORT_AFE_ID(unPortId))
   {
      *pDevPort = afe_svc_global_ptr->port_rt_proxy_ptr[GET_RT_PROXY_PORT_INDEX(unPortId)];
   }
   else if (IS_PSEUDO_PORT_AFE_ID(unPortId))
   {
      *pDevPort = afe_svc_global_ptr->pseudo_ports_ptr[GET_PSEUDO_PORT_INDEX(unPortId)];
   }
   else if (IS_SPDIF_PORT_AFE_ID(unPortId))
   {
      *pDevPort = afe_svc_global_ptr->ports_spdif_ptr;
   }
   else if (IS_HDMI_OUTPUT_PORT_AFE_ID(unPortId))
   {
      //HDMI_OVER_DP_PORT and HDMI_OUT_PORT share the same port structure memory as they could not be running concurrently
      *pDevPort = afe_svc_global_ptr->port_hdmi_output_ptr[GET_HDMI_OUTPUT_PORT_INDEX(unPortId)];
      (*pDevPort)->intf = unPortId;
   }
   else if (IS_HDMI_OVER_DP_PORT_AFE_ID(unPortId))
   {
      //HDMI_OVER_DP_PORT and HDMI_OUT_PORT share the same port structure memory as they could not be running concurrently
      *pDevPort = afe_svc_global_ptr->port_hdmi_output_ptr[GET_HDMI_OVER_DP_PORT_INDEX(unPortId)];
      (*pDevPort)->intf = unPortId;
   }
   else
   {
      *pDevPort = NULL;
   }

   if (NULL == *pDevPort)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_svc_get_port unsupported port id 0x%x", unPortId);
      result = ADSP_EUNSUPPORTED;
   }

   return result;
}

ADSPResult afe_port_id_validation(uint16_t port_id)
{
   ADSPResult result = ADSP_EBADPARAM;

   if ((IS_AUDIO_IF_TDM_PORT_AFE_ID(port_id)) ||
       (IS_AUDIO_IF_PORT_AFE_ID(port_id)) ||
       (IS_RIVA_PORT_AFE_ID(port_id)) ||
       (IS_SLIMBUS_PORT_AFE_ID(port_id)) ||
       (IS_RT_PROXY_PORT_AFE_ID(port_id)) ||
       (IS_PSEUDO_PORT_AFE_ID(port_id)) ||
       (IS_HDMI_OUTPUT_PORT_AFE_ID(port_id)) ||
       (IS_HDMI_OVER_DP_PORT_AFE_ID(port_id)) ||
       (IS_USBA_PORT_AFE_ID(port_id)) ||
       (IS_SPDIF_PORT_AFE_ID(port_id))
       )
   {
      result = ADSP_EOK;
   }

   return result;
}

ADSPResult afe_port_client_data_type_validation(afe_dev_port_t *pDevPort, afe_client_t *pClient)
{
   ADSPResult result = ADSP_EBADPARAM;
   switch (pClient->data_type)
   {
      case LINEAR_PCM:
         if (AFE_UNCOMPRESSED_DATA_PORT == pDevPort->port_data_type)
         {
            result = ADSP_EOK;
         }
         break;
      case IEC_61937_NON_LINEAR:
         if (AFE_IEC61937_COMPRESSED_DATA_PORT == pDevPort->port_data_type)
         {
            result = ADSP_EOK;
         }
         break;
      case DSD_DOP_NON_LINEAR:
         //Note that due to HW properties, AFE must be stereo for DSD native path.
         if (AFE_DSD_DOP_COMPRESSED_DATA_PORT == pDevPort->port_data_type && DSD_DOP_NUM_CHANNELS == pDevPort->channels)
         {
            result = ADSP_EOK;
         }
         break;
      case GENERIC_COMPRESSED:
         if (AFE_GENERIC_COMPRESSED_DATA_PORT == pDevPort->port_data_type)
         {
            result = ADSP_EOK;
         }
         break;
      default:
         //Now that the port data type is not recognized, return failure
         result = ADSP_EBADPARAM;
         break;
   }

   return result;
}

ADSPResult afe_port_client_validation(afe_dev_port_t *pDevPort, uint16_t direction)
{
   bufQList                 *client_list_ptr;
   ADSPResult           result = ADSP_EOK;
   afe_client_info_t   *client;

   if (RX_DIR == direction)
   {
      client_list_ptr = pDevPort->afe_rx_client_list_ptr;
   }
   else
   {
      client_list_ptr = pDevPort->afe_tx_client_list_ptr;
   }

   while (NULL != client_list_ptr)
   {
      client = (afe_client_info_t *)client_list_ptr->element;
      if (ADSP_EOK != afe_port_client_data_type_validation(pDevPort, &(client->afe_client)))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_port_client_data_type_validation failed on client_id %lu for port 0x%x", client->client_id, pDevPort->intf);
         result = ADSP_EUNSUPPORTED;
         return result;
      }
      else
      {
         client_list_ptr = client_list_ptr->next;
      }
   }

   return result;
}

/**
Based on the sample rate of client/device port and port latency mode, this utility 
would return number of equivalent samples rounded to approximately 1ms/0.5ms time frame.

The client connecting to AFE can use this information to allocate the buffers
This number is the minimum samples and clients are free to chose any
integral multiple of this number.

@param[in] latency_mode - AFE port operating latency mode
@param[in] sample rate - sample rate for which frame size is to be known
@param[in] *num_samples_ptr - pointer to variable which holds frame size (in samples)

@return
ADSP_EOK in case of valid sample rate
ADSP_EUNSUPPORTED in case of invalid sample rate

@dependencies
None
 */

ADSPResult afe_port_get_frame_size(uint32_t latency_mode, uint32_t sample_rate, uint32_t *num_samples_ptr)
{

   ADSPResult result = ADSP_EOK;

   switch (sample_rate)
   {
      case AFE_PORT_SAMPLE_RATE_8K:
      case AFE_PORT_SAMPLE_RATE_11_025K:
      case AFE_PORT_SAMPLE_RATE_12K:
      case AFE_PORT_SAMPLE_RATE_16K:
      case AFE_PORT_SAMPLE_RATE_22_05K:
      case AFE_PORT_SAMPLE_RATE_24K:
      case AFE_PORT_SAMPLE_RATE_32K:
      case AFE_PORT_SAMPLE_RATE_44_1K:
      case AFE_PORT_SAMPLE_RATE_48K:
      case AFE_PORT_SAMPLE_RATE_88_2K:
      case AFE_PORT_SAMPLE_RATE_96K:
      case AFE_PORT_SAMPLE_RATE_176_4K:
      case AFE_PORT_SAMPLE_RATE_192K:
      case AFE_PORT_SAMPLE_RATE_352_8K:
      case AFE_PORT_SAMPLE_RATE_384K:
      {
         elite_svc_get_frame_size(sample_rate, num_samples_ptr);

         switch (latency_mode)
         {
            case AFE_PORT_LOW_LATENCY_MODE:
               if (0x1 != *num_samples_ptr)
               {
                  *num_samples_ptr = (*num_samples_ptr) >> 1;
               }
               break;
            default:
               break;
         }
         break;
      }

      default:
         result = ADSP_EUNSUPPORTED;
         break;
   }
   return result;

}

/*=============================================================================
 FUNCTION      bool_t afe_is_sample_rate_fract afe_port_send_media_t

 DESCRIPTION   Checks if samplerate is integer or fractional
 OUTPUTS	   FALSE or TRUE based on whether samplerate is fractional

 DEPENDENCIES  None

 RETURN VALUE  bool_t

 SIDE EFFECTS
 ===============================================================================*/

bool_t afe_is_sample_rate_fract(uint32_t sample_rate)
{
   bool_t is_sample_rate_fract;
   switch (sample_rate)
   {
      case AFE_PORT_SAMPLE_RATE_11_025K:
      case AFE_PORT_SAMPLE_RATE_22_05K:
      case AFE_PORT_SAMPLE_RATE_44_1K:
      case AFE_PORT_SAMPLE_RATE_88_2K:
      case AFE_PORT_SAMPLE_RATE_176_4K:
      case AFE_PORT_SAMPLE_RATE_352_8K:
         is_sample_rate_fract = TRUE;
         break;
      default:
         is_sample_rate_fract = FALSE;
   }
   return is_sample_rate_fract;
}

/* XOR to identify if fractional resampling is needed.
 * Only use frac resampling if one of SR are fractional but NOT BOTH.
 * if BOTH then dont need fractional resampling as their ratio is always be integer. */
bool_t afe_is_fractional_resampling_needed(uint32_t sample_rate_1, uint32_t sample_rate_2)
{
   return (afe_is_sample_rate_fract(sample_rate_1) ^ afe_is_sample_rate_fract(sample_rate_2));
}

void afe_port_reset_media_type(afe_dev_port_t *pDevPort)
{
   switch (pDevPort->intf)
   {
      case AFE_PORT_ID_INTERNAL_BT_SCO_RX:
      case AFE_PORT_ID_INTERNAL_BT_SCO_TX:
      {
         /* Do not reset media type, if RIVA connected to LPASS.
          * If reset done, then sample rate set by RIVA through
          * IPC command would be lost. */
         if (afe_is_riva_active(pDevPort))
         {
            return;
         }
         pDevPort->sample_rate = AFE_PORT_SAMPLE_RATE_8K;
         pDevPort->channels = 1;
         pDevPort->dev_sample_rate = AFE_PORT_SAMPLE_RATE_8K;
         pDevPort->dev_channels = 1;
         break;
      }

      case AFE_PORT_ID_INTERNAL_BT_A2DP_RX:
      case AFE_PORT_ID_INTERNAL_FM_RX:
      case AFE_PORT_ID_INTERNAL_FM_TX:
      {
         /* Do not reset media type, if RIVA connected to LPASS.
          * If reset done, then sample rate set by RIVA through
          * IPC command would be lost. */
         if (afe_is_riva_active(pDevPort))
         {
            return;
         }
         pDevPort->sample_rate = AFE_PORT_SAMPLE_RATE_48K;
         pDevPort->channels = 2;
         pDevPort->dev_sample_rate = AFE_PORT_SAMPLE_RATE_48K;
         pDevPort->dev_channels = 2;
         break;
      }

      default:
      {
         pDevPort->sample_rate = AFE_PORT_SAMPLE_RATE_48K;
         pDevPort->channels = 2;
         pDevPort->dev_sample_rate = AFE_PORT_SAMPLE_RATE_48K;
         pDevPort->dev_channels = 2;
         break;
      }
   }
   pDevPort->bytes_per_channel = 2;
   pDevPort->bit_width = 16;
   pDevPort->dev_bytes_per_channel = 2;
   pDevPort->dev_bit_width = 16;
   pDevPort->q_format_shift_factor = QFORMAT_SHIFT_FACTOR;
   pDevPort->dev_data_type = AFE_UNCOMPRESSED_DATA_PORT;
   pDevPort->port_data_type = AFE_UNCOMPRESSED_DATA_PORT;
   pDevPort->is_port_media_type_set = FALSE;
   pDevPort->limiter_enable		= TRUE;  //b default, limiter is enabled for a given port
}

/*=============================================================================
 * FUNCTION         afe_port_copy_media_type_dev_to_port
 *
 * DESCRIPTION    utility function to copy device media type -> port media type (if applicable)
 * Param ID: #AFE_PARAM_ID_PORT_MEDIA_TYPE set on module ID: #AFE_MODULE_PORT
 * is used by HLOS client to explicitly set the port media type.
 * If HLOS client does not set this port media type explicitly, this utility function helps the driver code to
 * overwrite the port media type with the device media type.
 *
 * @param[in]       pDevPort device port instance.
 *
 * DEPENDENCIES  None
 *
 * RETURN VALUE  None
 ===============================================================================*/
void afe_port_copy_media_type_dev_to_port(afe_dev_port_t *pDevPort)
{
   if (FALSE == pDevPort->is_port_media_type_set)
   {
      pDevPort->port_data_type = pDevPort->dev_data_type;
      pDevPort->bytes_per_channel = pDevPort->dev_bytes_per_channel;
      pDevPort->bit_width = pDevPort->dev_bit_width;
      pDevPort->channels = pDevPort->dev_channels;
      pDevPort->sample_rate = pDevPort->dev_sample_rate;
   }
}
