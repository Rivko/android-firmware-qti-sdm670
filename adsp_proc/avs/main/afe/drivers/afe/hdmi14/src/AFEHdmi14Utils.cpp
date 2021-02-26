/** @file AFEDalHdmiHelp.cpp
    This module implements the help functions for HDMI.

    Copyright (c) 2009 Qualcomm Technologies, Incorporated.  All Rights Reserved.
    QUALCOMM Proprietary.  Export of this technology or software is regulated
    by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
                               Edit History

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/hdmi14/src/AFEHdmi14Utils.cpp#1 $

when       who     what, where, why
------     ---    -------------------------------------------------------
10/15/10   FZ      Add channel mapping support
11/18/09   FZ      Created file

========================================================================== */



#include "qurt_elite.h"
#include "AFEHdmi14Utils.h"


#define HDMI_BLOCK_SIZE   192   //one HDMI block has 192 frames, one HDMI frame has two subframes, and one subframe has 32 time slots.

#define HDMI_2_CHANNELS                2
#define HDMI_3_CHANNELS                3
#define HDMI_4_CHANNELS                4
#define HDMI_5_CHANNELS                5
#define HDMI_6_CHANNELS                6
#define HDMI_7_CHANNELS                7
#define HDMI_8_CHANNELS                8


/* Preamble bits  */
#define HDMI_IEC60958_PREAMBLE_M_VAL		0x02    //0b0010
#define HDMI_IEC60958_PREAMBLE_W_VAL		0x04    //0b0100
#define HDMI_IEC60958_PREAMBLE_B_VAL		0x08    //0b1000

#define HDMI_IEC60958_PREAMBLE_MASK         0x0F


#define HDMI_IEC60958_V_BIT_MASK    0x1
#define HDMI_IEC60958_U_BIT_MASK    0x2
#define HDMI_IEC60958_C_BIT_MASK    0x4
#define HDMI_IEC60958_P_BIT_MASK    0x8

#define IEC60958_GET_FLAGS_INFO(x)    ((x >> 28) & 0x0F)
#define IEC60958_GET_AUDIO_SAMPLE(x)  (x & 0x0FFFFFF0)

#define HDMI_IEC60958_MSB_ALIGN_BITS_MASK  0x4

#define IEC60958_NUM_SAMPLING_RATES       16
#define SAMPLE_RATE_RESERVED              -1
#define SAMPLE_RATE_NOT_INDICATED         0
#define IEC60958_SAMPLE_RATE_INFO_FRAME   27


#define HDMI_IEC60958_P_BIT31          31
#define HDMI_IEC60958_C_BIT30          30
#define HDMI_IEC60958_U_BIT29          29
#define HDMI_IEC60958_V_BIT28          28

#define HDMI_IEC60958_PREAMBLE_BIT3    3
#define HDMI_IEC60958_PREAMBLE_BIT2    2
#define HDMI_IEC60958_PREAMBLE_BIT1    1
#define HDMI_IEC60958_PREAMBLE_BIT0    0

#define HDMI_IEC60958_AUX_START_BIT4   4
#define HDMI_IEC60958_MSB_ALIGN_BIT8   8


#define HDMI_DATA_SHIFT_LEFT           0 
#define HDMI_DATA_SHIFT_RIGHT          1


//#define HDMI_TOTAL_CA_CNT 0x20

//extern hdmi14_channel_alloc_array[HDMI_TOTAL_CA_CNT][9];

#ifdef SIM
static void afe_hdmi14_fill_IEC60958_2chdata(hdmi14_port_state_t *p_dev_state, uint16_t* src, uint32_t* dst);
static void afe_hdmi14_fill_IEC60958_8chdata(hdmi14_port_state_t *p_dev_state, uint16_t* src, uint32_t* dst);
static void afe_hdmi14_fill_IEC60958_data(hdmi14_port_state_t *p_dev_state, uint16_t* src, uint32_t* dst, uint32_t frame);
#else
static void afe_hdmi14_align_IEC60958_2chdata(hdmi14_port_state_t *p_dev_state, uint16_t* src, uint32_t* dst);
static void afe_hdmi14_align_IEC60958_8chdata(hdmi14_port_state_t *p_dev_state, uint16_t* src, uint32_t* dst);
static void afe_hdmi14_align_IEC60958_data(hdmi14_port_state_t *p_dev_state, uint16_t* src, uint32_t* dst);
#endif

/*============================================================================================*/

/**
  @brief allocate and init  IEC60958 channel status table

  @param[in] p_dev_state pointer for AFE dev port state structure

  @return  ADSP_EOK on success, an error code on error
*/
ADSPResult afe_hdmi14_init_IEC60958_ch_status_table(hdmi14_port_state_t *p_dev_state)
{
   ADSPResult result = ADSP_EOK;

   //allocate memory for IEC60958 Channel status format table
   /* pointer of IEC 60958-3 Mode 0 Channel Status General Format for Consumer User table */ 
   p_dev_state->IEC60958_ch_status_table_ptr = (uint32_t *)qurt_elite_memory_malloc(sizeof(uint32_t) * HDMI_BLOCK_SIZE,
		   QURT_ELITE_HEAP_DEFAULT);
   if(NULL == p_dev_state->IEC60958_ch_status_table_ptr)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "hdmi14 Error in allocating memory for IEC60958 channel status table");
      return ADSP_ENORESOURCE;
   }
   memset(p_dev_state->IEC60958_ch_status_table_ptr, 0, sizeof(uint32_t) * HDMI_BLOCK_SIZE);
   p_dev_state->IEC60958_frame_index  = 0;


   /**
   * The IEC60958 channel status table configured based on sampleRate, pcmDataType, and bitWidth  
   * Mode 0 channel status are 192 bits but we are using 192 Dwords representing the bits.
   * NOTE: Preamble B only take care. Preamble M and W is set to 0.
   */

   p_dev_state->IEC60958_ch_status_table_ptr[0] = 1UL << HDMI_IEC60958_PREAMBLE_BIT3;  //block start bit in preamble bit 3 and consumer use.

   if(AFE_NON_LINEAR_DATA_PACKED_60958 == p_dev_state->data_format) //used for sim test.  //TODO: it will be used for I2S HDMI input
   {
      //The purpose of channel status bit 1 is to indicate if IEC 60958 is used to convey
      //linear PCM or non-linear PCM. This bit shall be set to '1' when IEC 60958 is used
      //to convey non-linear PCM encoded audio bitstreams
      p_dev_state->IEC60958_ch_status_table_ptr[1] = 1UL << HDMI_IEC60958_C_BIT30;
   }

   switch (p_dev_state->sample_rate)
   {
      case 32000:  // 1100 in 24..27
         p_dev_state->IEC60958_ch_status_table_ptr[24] = 1UL << HDMI_IEC60958_C_BIT30;
         p_dev_state->IEC60958_ch_status_table_ptr[25] = 1UL << HDMI_IEC60958_C_BIT30;
         break;
      case 44100:  // 0000 in 24..27
         break;
      case 88200:  // 0001 in 24..27
         p_dev_state->IEC60958_ch_status_table_ptr[27] = 1UL << HDMI_IEC60958_C_BIT30;
         break;
      case 176400: // 0011 in 24..27
         p_dev_state->IEC60958_ch_status_table_ptr[26] = 1UL << HDMI_IEC60958_C_BIT30;
         p_dev_state->IEC60958_ch_status_table_ptr[27] = 1UL << HDMI_IEC60958_C_BIT30;
         break;
      case 48000:  // 0100 in 24..27
         p_dev_state->IEC60958_ch_status_table_ptr[25] = 1UL << HDMI_IEC60958_C_BIT30;
         break;
      case 96000:  // 0101 in 24..27
         p_dev_state->IEC60958_ch_status_table_ptr[25] = 1UL << HDMI_IEC60958_C_BIT30;
         p_dev_state->IEC60958_ch_status_table_ptr[27] = 1UL << HDMI_IEC60958_C_BIT30;
         break;
      case 192000: // 0111 in 24..27
         p_dev_state->IEC60958_ch_status_table_ptr[25] = 1UL << HDMI_IEC60958_C_BIT30;
         p_dev_state->IEC60958_ch_status_table_ptr[26] = 1UL << HDMI_IEC60958_C_BIT30;
         p_dev_state->IEC60958_ch_status_table_ptr[27] = 1UL << HDMI_IEC60958_C_BIT30;
      break;
      default:
      break;
  }

    //why do we set the bit width as 24 bits always?
    if(24 <= p_dev_state->bit_width)
    {
       //bit 32: set to 1 to indicate the maximum audio sample word length as 24 bits
       p_dev_state->IEC60958_ch_status_table_ptr[32] = 1UL << HDMI_IEC60958_C_BIT30;
       //bit 33 to 35: set to "101" to indicate 24 bits 
       p_dev_state->IEC60958_ch_status_table_ptr[33] = 1UL << HDMI_IEC60958_C_BIT30;
       p_dev_state->IEC60958_ch_status_table_ptr[35] = 1UL << HDMI_IEC60958_C_BIT30;
    }

    return result;
}

/**
  @brief deallocate and deinit  IEC60958 channel status table

  @param[in] p_dev_state pointer for AFE dev port state structure

  @return  ADSP_EOK on success, an error code on error
*/
ADSPResult afe_hdmi14_deinit_IEC60958_ch_status_table(hdmi14_port_state_t *p_dev_state)
{
   ADSPResult result = ADSP_EOK;

   //free the IEC60958 table 
	qurt_elite_memory_free(p_dev_state->IEC60958_ch_status_table_ptr);
   p_dev_state->IEC60958_ch_status_table_ptr = NULL;
   p_dev_state->IEC60958_frame_index = 0;

   return result;
}



//To-Do: we assume the upper layer always use int16 pointer to pass data in even the source data is 20 or 24 bit width.
// If upper layer could not fit this requirement, the necessary code expansion is needed here.
//# of channel for non-linaer is always configured as 2 channels and 16 bits always by upper layer.

/**
  @brief pack the data for IEC60958 format and copy it to dma buffer.
     data format is IEC60958 Subframe format and there is no null subframe within a sample for i2s

  @param[in] p_dev_state pointer for AFE dev port state structure
  @param[in] src client buffer
  @param[in] dst dma buffer

*/

void afe_hdmi14_copy_IEC60958_data(hdmi14_port_state_t *p_dev_state, uint16_t* src, uint32_t* dst)
{

#ifndef SIM
      //hw packetizing is enabled so only need to do alignment
      if(HDMI_2_CHANNELS == p_dev_state->num_channels)
      {
         afe_hdmi14_align_IEC60958_2chdata(p_dev_state, src, dst);
      }
      else
      {
         // 4 or 6 or 8 channels
         afe_hdmi14_align_IEC60958_8chdata(p_dev_state, src, dst);
      }
#else //sw packetizing 
      if(HDMI_2_CHANNELS == p_dev_state->num_channels)
      {
         afe_hdmi14_fill_IEC60958_2chdata(p_dev_state, src, dst); 
      }
      else
      {
         // 4 or 6 or 8 channels
         afe_hdmi14_fill_IEC60958_8chdata(p_dev_state, src, dst);
      }
#endif
}

#ifdef SIM

static void afe_hdmi14_fill_IEC60958_2chdata(hdmi14_port_state_t *p_dev_state, uint16_t* src, uint32_t* dst)
{
    uint32_t   i = 0;
    int32_t   num_ch;
    uint32_t  packed_data, packed_data1;
    uint32_t  align_shift;
    uint32_t  data_mask;
    uint32_t  data_shift_dir = HDMI_DATA_SHIFT_LEFT;
    uint32_t cur_frame = p_dev_state->IEC60958_frame_index;

    int32_t *src32 = (int32_t *)src;

    //always 16bits for non-linear use case.
    switch (p_dev_state->bit_width)
    {
      case 20:  //data from AFE SVC is left justified (bit 31:12 valid)
         // msb aligned so that it should be same as 24bit case.
         align_shift  = HDMI_IEC60958_MSB_ALIGN_BIT8 - 4;
         data_mask    = 0xFFFFF000;
         data_shift_dir = HDMI_DATA_SHIFT_RIGHT;
         break;
      case 24:  //data from AFE SVC is left justified (bit 31:8 valid) 
      case 32:
         align_shift = HDMI_IEC60958_MSB_ALIGN_BIT8 - 4;
         data_mask   = 0xFFFFFF00;        
         data_shift_dir = HDMI_DATA_SHIFT_RIGHT;
         break;
      case 16:
      default:
         align_shift = HDMI_IEC60958_MSB_ALIGN_BIT8 + HDMI_IEC60958_AUX_START_BIT4;
         data_mask   = 0x0000FFFF;
         break;
    }

    //numChannels for non-linear case is always 2.
    while (i < p_dev_state->int_samples_per_period * p_dev_state->num_channels)  //numSampleToCopy = number of sample for one channel
    {
        num_ch = p_dev_state->num_channels;
        i += num_ch;
        while (num_ch--)
        {
            packed_data = (p_dev_state->bit_width <= 16) ? (uint32)(*src++) & data_mask :(uint32)(*src32++) & data_mask;

            if(HDMI_DATA_SHIFT_RIGHT == data_shift_dir)
            {
                packed_data >>= align_shift;
            }
            else
            {
                packed_data <<= align_shift;
            }

            //Channel status is identical for both subframes of the interface.
            packed_data |= p_dev_state->IEC60958_ch_status_table_ptr[cur_frame];

            if(AFE_NON_LINEAR_DATA == p_dev_state->data_format) //TODO: it will be used for I2S HDMI input
            {
               packed_data |= 1UL << HDMI_IEC60958_V_BIT28;  //set validity flag to 1 for non-linear PCM only
               // 1 if is not reliable. 
               //NOTE For transmissions not using a linear PCM coding, this bit may be set.
               // This is intended to prevent accidental decoding of non-audio data to analogue before a cmplete channel status block is received
            }

            // calculate parity (optimized version)
            // parity bit carries from bit 4 to 31 inclusive, carry an even number of ones and an even number of zeros (even parity)
            packed_data1  = packed_data & 0xFFFFFFF0;
            packed_data1 ^= packed_data1 >> 1;
            packed_data1 ^= packed_data1 >> 2;
            packed_data1  = (packed_data1 & 0x11111111U) * 0x11111111U;

            // fill P bit, Parity value at bit28
            *dst++ = packed_data | ((packed_data1 << 3) & 0x80000000);
        }

        // HDMI channel status cycle every 192 frames
        cur_frame++;
        if (HDMI_BLOCK_SIZE == cur_frame)
        {
            cur_frame = 0;
        }
    }

    p_dev_state->IEC60958_frame_index = cur_frame;
}

/*
* afeDalFillHdmiData8Ch
* src will be copied to only valid channel data.
* dst have data slot even for invalid channel data which will be
* actual sample will be presented by Present bits based on CA.
*/
static void afe_hdmi14_fill_IEC60958_8chdata(hdmi14_port_state_t *p_dev_state, uint16_t* src, uint32_t* dst)
{
    uint32_t i = 0, k = 0, map_loop = 0;
    int32_t* src32 = (int32*)src;
    uint8_t* ch_map_ptr = (uint8_t*)&hdmi14_channel_alloc_array[p_dev_state->hdmi_ca][0];
    uint32_t cur_frame = p_dev_state->IEC60958_frame_index;

    while (i < p_dev_state->int_samples_per_period * HDMI_8_CHANNELS)
    {
       //The data stream from upper layer are all valid
       //No NULL channel info in bit stream data from upper layer
       //Take ca = 0x9 as example here
       //
       //Stream channel mapping or media format passed to AFE DAL is
       // {FL,
       //  FR,
       //  LFE,
       //  RL,
       //  RR}
       //
       //The stream data passed to AFE DAL is
       // {FL,
       //  FR,
       //..LFE,
       //..RL,
       //..RR}
       //
       //The data in DMA buffer (out of AFE DAL) is
       // {FL,
       //  FR,
       //  LFE,
       //  NULL,
       //  RL,
       //  RR,
       //  NULL,
       //  NULL}
       //
       //The dst data pointer is always moved in 8 DWs or 8 channel based
       //The src data pointer is moved based on the ch # of input data stream

       //reset the next dst 8 DWs to zero
       //AfeOsalMemset( (void*)&dst[i], 0, HDMI_8_CHANNELS * sizeof(int32) );

       for(map_loop = 0; map_loop < ch_map_ptr[HDMI_8_CHANNELS]; map_loop++)
       {
          if(16 == p_dev_state->bit_width)
          {
             afe_hdmi14_fill_IEC60958_data(p_dev_state, &src[k+map_loop], &dst[i+ch_map_ptr[map_loop]], cur_frame);
          }
          else
          {
             afe_hdmi14_fill_IEC60958_data(p_dev_state, (uint16_t *)&src32[k+map_loop], &dst[i+ch_map_ptr[map_loop]], cur_frame);
          }
       }

       i += 8;
       k += ch_map_ptr[HDMI_8_CHANNELS];  //source does not include NULL channel data

       cur_frame++;
       if (HDMI_BLOCK_SIZE == cur_frame)
       {
           cur_frame = 0;
       }
    }

    p_dev_state->IEC60958_frame_index =  cur_frame; // the number of blocks we filled
}




// P | C | U | V | DATA20 | AUX4 | PREAMBLE4
static void afe_hdmi14_fill_IEC60958_data(hdmi14_port_state_t *p_dev_state, uint16_t* src, uint32_t* dst, uint32_t frame)

{
   uint32_t packed_data, packed_data1;
   uint32_t parity;
   uint32_t align_shift;
   uint32_t data_mask;
   uint32_t data_shift_dir = HDMI_DATA_SHIFT_LEFT;
   int32_t *src32 = (int32_t *) src;

   switch (p_dev_state->bit_width)
   {
      case 20:
         align_shift  = HDMI_IEC60958_MSB_ALIGN_BIT8;
         data_mask    = 0xFFFFF000;
         data_shift_dir = HDMI_DATA_SHIFT_RIGHT;
         break;
      case 24:
      case 32:
         align_shift = HDMI_IEC60958_MSB_ALIGN_BIT8 - 4;
         data_mask   = 0xFFFFFF00;
         data_shift_dir = HDMI_DATA_SHIFT_RIGHT;
         break;
      case 16:
      default:
         align_shift = HDMI_IEC60958_MSB_ALIGN_BIT8 + HDMI_IEC60958_AUX_START_BIT4;
         data_mask   = 0x0000FFFF;
         break;
   }

    packed_data = (p_dev_state->bit_width <= 16) ? (uint32)(*src++) & data_mask :(uint32)(*src32++) & data_mask;

    if(HDMI_DATA_SHIFT_RIGHT == data_shift_dir)
    {
       packed_data >>= align_shift;
    }
    else
    {
       packed_data <<= align_shift;
    }

    packed_data |= p_dev_state->IEC60958_ch_status_table_ptr[frame];

    // calculate parity
    //Bit 31 (parity bit) carries a parity bit such that bits 4 to 31 inclusive carry an even number of ones and an even number of zeros
    packed_data1  = packed_data & 0xFFFFFFF0;
    packed_data1 ^= packed_data1 >> 1;
    packed_data1 ^= packed_data1 >> 2;
    packed_data1 = (packed_data1 & 0x11111111U) * 0x11111111U;
    // Parity value at bit28
    parity = (packed_data1 << 3) & 0x80000000;

    // fill P from generated parity
    *dst++ = (int32)(packed_data | parity);
   
}

#else

static void afe_hdmi14_align_IEC60958_2chdata(hdmi14_port_state_t *p_dev_state, uint16_t* src, uint32_t* dst)
{
    uint32_t  i = 0;
    int32_t  numch;
    uint32_t packed_data, align_shift, data_mask;
    uint32_t data_shift_dir = HDMI_DATA_SHIFT_LEFT;

    int32_t *src32 = (int32_t *)src;

    //hw packetizing: MSB need to align to bit 23
    switch (p_dev_state->bit_width)
    {
      case 20:  //data from AFE SVC is left justified (bit 31:12 valid) 
         align_shift  = HDMI_IEC60958_MSB_ALIGN_BIT8;
         data_mask    = 0xFFFFF000;
         data_shift_dir = HDMI_DATA_SHIFT_RIGHT;
         break;
      case 24:  //data from AFE SVC is left justified (bit 31:8 valid) 
      case 32:
         align_shift = HDMI_IEC60958_MSB_ALIGN_BIT8;
         data_mask   = 0xFFFFFF00;        
         data_shift_dir = HDMI_DATA_SHIFT_RIGHT;
         break;
      case 16:
      default:
         align_shift = HDMI_IEC60958_MSB_ALIGN_BIT8;
         data_mask   = 0x0000FFFF;
         break;
    }

    while (i < p_dev_state->int_samples_per_period * p_dev_state->num_channels)  //numSampleToCopy = number of sample for one channel
    {
        numch = p_dev_state->num_channels;
        i += numch;
        while (numch--)
        {
            packed_data = (p_dev_state->bit_width <= 16) ? (uint32)(*src++) & data_mask :(uint32)(*src32++) & data_mask;

            if(HDMI_DATA_SHIFT_RIGHT == data_shift_dir)
            {
                *dst++ = packed_data >> align_shift;
            }
            else
            {
                *dst++ = packed_data << align_shift;
            }
        }
    }
}

// P | C | U | V | DATA20 | AUX4 | PREAMBLE4
static void afe_hdmi14_align_IEC60958_data(hdmi14_port_state_t *p_dev_state, uint16_t* src, uint32_t* dst)
{
   uint32_t packed_data;
   uint32_t align_shift;
   uint32_t data_mask;
   uint32_t data_shift_dir = HDMI_DATA_SHIFT_LEFT;
   int32_t *src32 = (int32_t *) src;

   //hw packetizing: MSB need to align to bit 23
   switch (p_dev_state->bit_width)
   {
      case 20:
         align_shift  = HDMI_IEC60958_MSB_ALIGN_BIT8;
         data_mask    = 0xFFFFF000;
         data_shift_dir = HDMI_DATA_SHIFT_RIGHT;
         break;
      case 24:
      case 32:
         align_shift = HDMI_IEC60958_MSB_ALIGN_BIT8;
         data_mask   = 0xFFFFFF00;
         data_shift_dir = HDMI_DATA_SHIFT_RIGHT;
         break;
      case 16:
      default:
         align_shift = HDMI_IEC60958_MSB_ALIGN_BIT8;
         data_mask   = 0x0000FFFF;
         break;
   }

   packed_data = (p_dev_state->bit_width <= 16) ? (uint32)(*src++) & data_mask :(uint32)(*src32++) & data_mask;

   if(HDMI_DATA_SHIFT_RIGHT == data_shift_dir)
   {
      *dst++ = packed_data >> align_shift;
   }
   else
   {
      *dst++ = packed_data << align_shift;
   }
   
}

static void afe_hdmi14_align_IEC60958_8chdata(hdmi14_port_state_t *p_dev_state, uint16_t* src, uint32_t* dst)
{
    uint32_t i = 0, k = 0, maploop = 0;
    int32_t* src32 = (int32_t*)src;
    uint8_t* ch_map_ptr = (uint8_t*)&hdmi14_channel_alloc_array[p_dev_state->hdmi_ca][0];

    while (i < p_dev_state->int_samples_per_period * HDMI_8_CHANNELS)
    {
       //The data stream from upper layer are all valid
       //No NULL channel info in bit stream data from upper layer
       //Take ca = 0x9 as example here
       //
       //Stream channel mapping or media format passed to AFE DAL is
       // {FL,
       //  FR,
       //  LFE,
       //  RL,
       //  RR}
       //
       //The stream data passed to AFE DAL is
       // {FL,
       //  FR,
       //..LFE,
       //..RL,
       //..RR}
       //
       //The data in DMA buffer (out of AFE DAL) is
       // {FL,
       //  FR,
       //  LFE,
       //  NULL,
       //  RL,
       //  RR,
       //  NULL,
       //  NULL}
       //
       //The dst data pointer is always moved in 8 DWs or 8 channel based
       //The src data pointer is moved based on the ch # of input data stream

       //reset the next dst 8 DWs to zero
       //AfeOsalMemset( (void*)&dst[i], 0, HDMI_8_CHANNELS * sizeof(int32) );

       for(maploop = 0; maploop < ch_map_ptr[HDMI_8_CHANNELS]; maploop++)
       {
          if(16 == p_dev_state->bit_width)
          {
             afe_hdmi14_align_IEC60958_data(p_dev_state, (uint16_t *)&src[k+maploop], &dst[i+ch_map_ptr[maploop]]);
          }
          else
          {
             afe_hdmi14_align_IEC60958_data(p_dev_state, (uint16_t *)&src32[k+maploop], &dst[i+ch_map_ptr[maploop]]);
          }
       }

       i += 8;
       k += ch_map_ptr[HDMI_8_CHANNELS];  //source does not include NULL channel data
    }
}

#endif

#if 0
DALResult afeDalHandleHdmiChannelMap(int numChannels, uint8* pInChArray, uint32 spkrAlloc, uint32 ca, uint8* pMapArray)
{
   int i;
   if(HDMI_2_CHANNELS == numChannels)  //numChannels is from client. It is either 2 or 8
   {
      //HDMI basic audio case
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "HDMI basic audio case - channel num = %d \n", numChannels);
   }
   else
   {
      if((0 < ca) && (0x20 > ca) && (NULL != pMapArray)) 
      {
         for (i = 0; i <= HDMI_8_CHANNELS; i++)
         {
            pMapArray[i] = (uint8)hdmiChannelAllocArray[ca][i];
         }
      }
      else
      {
         //not provide valid HDMI channel allocation value
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "HDMI Error- channel allocation  ca = %d \n", ca);
      }
   }

  return DAL_SUCCESS;
}
#endif

//According to table in page 158 of HDMI Compliance Test Specification
uint32_t afe_hdmi14_get_present_bits(uint32_t ca)
{
    uint32_t present_bits = 0;

    switch (ca)
    {
        case 0x1:
        case 0x2:
        case 0x3:
        default:
            present_bits = 3;  //0011
            break;
        case 0x4:
        case 0x8:
            present_bits = 5;  //0101
            break;
        case 0x5:
        case 0x6:
        case 0x7:
        case 0x9:
        case 0xa:
        case 0xb:
            present_bits = 7;  //0111
            break;
        case 0xc:
        case 0x10:
        case 0x18:
        case 0x1c:
            present_bits = 0xd;  //1101
        case 0xd:
        case 0xe:
        case 0xf:
        case 0x11:
        case 0x12:
        case 0x13:
        case 0x19:
        case 0x1a:
        case 0x1b:
        case 0x1d:
        case 0x1e:
        case 0x1f:
            present_bits = 0xf;  //1111
            break;
        case 0x14:
            present_bits = 9;  //1001
        case 0x15:
        case 0x16:
        case 0x17:
            present_bits = 0xb;  //1011
            break;
    }

    return present_bits;
}

#if 0
/** 
* afeDalAdjustHdmiLayoutFormat
*
* adjust bitwidth and num of channels for dma buffer allocation based on HDMI hardware interface.
* NOTE: afeDalAdjustHdmiLayoutFormat is assuming 
   that there is no null subframe within a sample for AFEDevDataIEC60958PackedNonLinear and AFEDevDataIEC60958PackedLinearPcm
*/

DALResult afeDalAdjustHdmiLayoutFormat(AFEDevAudIfType audioIf, AFEDevDataFormatType dataFormat, int32 *numChannels,  int32 *bitWidth)
{
   DALResult dalSts = DAL_SUCCESS;

   if((AFEDevDataNonLinear == dataFormat) || \
      (AFEDevDataIEC60958PackedNonLinear == dataFormat))
   {
      //Non-linear PCM audio frame (IEC 61937) is always packed on 2 channels IEC 60958 frames for HDMI
      if(2 != *numChannels)
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFEDAL HDMI Non-Linear PCM # of channel %d is convered to 2\n",*numChannels);
         *numChannels = 2;
      }
   }
   else if(AFEDevDataLinearPcm == dataFormat)
   {
      MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFEDAL HDMI Linear PCM \n");
      if(2 < *numChannels)
      {  
         //LAYOUT1 format the data expected from hardware is always 8 channel no matter what the channel num is 4 or 6 or 8
         if(8 != *numChannels)
         {
            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFEDAL HDMI LAYOUT1 format channel converted to 8 from %d \n",*numChannels);
            *numChannels = 8;
         }
      }
   }
   
   //For HDMI, no matter what the sample width (16 bits or 24 bits) it is, the packed DMA data always 32 bits
   if(32 != *bitWidth)
   {
      MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFEDAL HDMI data Format %d, orignal bitwidth %d\n", dataFormat, *bitWidth);
      *bitWidth = 32;
   }

   return dalSts;
}
#endif






