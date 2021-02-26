/*========================================================================

*//** @file voice_multi_ch_circ_buf.cpp

Copyright (c) 2010-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

This file provides interfaces to create and control multi channel circular buffers for voice threads.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/voice/services/voice_proc_tx/src/voice_multi_ch_circ_buf.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
10/29/10           Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "voice_multi_ch_circ_buf.h"

circbuf_status_enum voice_multi_ch_circ_buf_init(voice_multi_ch_circ_buf_t *structPtr, 
      int32_t num_channels, int32_t max_samples, int32_t bitsPerChannel)
{
   circbuf_status_enum  result = CIRCBUF_SUCCESS;
   if (MAX_CIRCBUF_CHANNELS >= num_channels)
   {
      structPtr->num_channels = num_channels;
      for (int32_t i = 0; i < num_channels; i++)
      {
         result = voice_circbuf_init((circbuf_struct *)&(structPtr->circ_buf[i]),
               (int8_t *)(structPtr->buf_ptr[i]), max_samples, MONO_VOICE,
               bitsPerChannel);
         if (CIRCBUF_SUCCESS != result)
         {
            break;
         }
      }
   }
   else
   {
      structPtr->num_channels = 0;
      return CIRCBUF_FAIL;
   }
   return result;
}

/*
 * This function can be used to prefill multichannel circular buffer with numsamples worth of samples for each channel
 * The data will be filled ahead of already existing samples in the circular buffer
 */
circbuf_status_enum voice_multi_ch_circ_buf_prefill(voice_multi_ch_circ_buf_t *structPtr,int8_t *(*fromPtr)[MAX_CIRCBUF_CHANNELS], int32_t numSamples)
{
    circbuf_status_enum  result = CIRCBUF_SUCCESS;
    uint32_t unread_samples = structPtr->circ_buf[0].unread_samples;
    uint32_t max_samples = structPtr->circ_buf[0].max_samples;
    if (numSamples > (max_samples-unread_samples))
    {
        MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: voice_multi_ch_circ_buf_prefill will result in loss of data, numSamples(%ld), available space(%ld)",
                numSamples,(max_samples-unread_samples));
    }
    int16* backup_ptr = (int16*) qurt_elite_memory_malloc(max_samples<<1, QURT_ELITE_HEAP_DEFAULT );
    if (!backup_ptr)
    {
        MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO," VCP: memory unavailable...reset the buffer %ld ");

        return CIRCBUF_FAIL;
    }
    //memset(backup_ptr,0,max_samples<<1);  no need to memset
    for (int32 i=0;i<structPtr->num_channels;i++)
    {
        voice_circbuf_read((circbuf_struct *)&(structPtr->circ_buf[i]),
                (int8*)backup_ptr, unread_samples, max_samples<<1);
        voice_circbuf_write((circbuf_struct *)&(structPtr->circ_buf[i]),
                (int8*)(*fromPtr)[i], numSamples);
        voice_circbuf_write((circbuf_struct *)&(structPtr->circ_buf[i]),
                (int8*)backup_ptr, unread_samples);
    }

    qurt_elite_memory_free(backup_ptr);
    return result;

}

circbuf_status_enum voice_multi_ch_circ_buf_read(voice_multi_ch_circ_buf_t *structPtr,
      int8_t *(*toPtr)[MAX_CIRCBUF_CHANNELS], int32_t numSamples)
{
   circbuf_status_enum  result = CIRCBUF_SUCCESS;
   for (int32_t i = 0; i < structPtr->num_channels; i++)
   {
      result = voice_circbuf_read((circbuf_struct *)&(structPtr->circ_buf[i]),
            (int8_t *)(*toPtr)[i], numSamples, numSamples << 1);
      if (CIRCBUF_SUCCESS != result)
      {
         MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: voice_multi_ch_circ_buf_read error, ptr(%p) num_channels(%ld), index(%ld)",structPtr,structPtr->num_channels,i);
         break;
      }
   }
   return result;
}

circbuf_status_enum voice_multi_ch_circ_buf_block_read(voice_multi_ch_circ_buf_t *structPtr,
      int8_t *toPtr, int32_t numSamples)
{
   circbuf_status_enum  result = CIRCBUF_SUCCESS;
   int32_t num_samples_per_ch = numSamples / structPtr->num_channels; //TODO: avoid division 
   int32_t channel_offset = 0;
   for (int32_t i = 0; i < structPtr->num_channels; i++)
   {
      result = voice_circbuf_read((circbuf_struct *)&(structPtr->circ_buf[i]),
            (int8_t *)((int16_t*)toPtr+channel_offset), num_samples_per_ch, num_samples_per_ch << 1);
      channel_offset += num_samples_per_ch;
      if (CIRCBUF_SUCCESS != result)
      {
         MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: voice_multi_ch_circ_buf_block_read error, ptr(%p) num_channels(%ld), index(%ld)",structPtr,structPtr->num_channels,i);
         break;
      }
   }
   return result;
}

circbuf_status_enum voice_multi_ch_circ_buf_write(voice_multi_ch_circ_buf_t *structPtr,
      int8_t *(*fromPtr)[MAX_CIRCBUF_CHANNELS], int32_t numSamples)
{
   circbuf_status_enum  result = CIRCBUF_SUCCESS;

   for (int32_t i = 0; i < structPtr->num_channels; i++)
   {
      result = voice_circbuf_write((circbuf_struct *)&(structPtr->circ_buf[i]),
            (int8_t *)(*fromPtr)[i], numSamples); // becuase size of each sample is int16_t
      if (CIRCBUF_SUCCESS != result)
      {
         MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: voice_multi_ch_circ_buf_write error, ptr(%p) num_channels(%ld), index(%ld), unread samples(%ld) , max samples(%ld) samples(%ld)",
               structPtr,structPtr->num_channels,i,structPtr->circ_buf[i].unread_samples, structPtr->circ_buf[i].max_samples, numSamples);
      }
      /* Keep writing to other channels even if one channel overflows */
      if (CIRCBUF_FAIL == result)
      {
         break;
      }
   }
   return result;
}

circbuf_status_enum voice_multi_ch_circ_buf_block_write(voice_multi_ch_circ_buf_t *structPtr,
      int8_t *fromPtr, int32_t numSamples)
{
   circbuf_status_enum  result = CIRCBUF_SUCCESS;
   int32_t num_samples_per_ch = numSamples / structPtr->num_channels; //TODO: avoid division 
   int32_t channel_offset = 0;

   for (int32_t i = 0; i < structPtr->num_channels; i++)
   {
      result = voice_circbuf_write((circbuf_struct *)&(structPtr->circ_buf[i]),
            (int8_t *)(((int16_t*)fromPtr)+channel_offset ), num_samples_per_ch); // becuase size of each sample is int16_t
      channel_offset += num_samples_per_ch;
      if (CIRCBUF_SUCCESS != result)
      {
         MSG_6(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"VCP: voice_multi_ch_circ_buf_write error, ptr(%p) num_channels(%ld), index(%ld), unread samples(%ld) , max samples(%ld) samples(%ld)",
               structPtr,structPtr->num_channels,i,structPtr->circ_buf[i].unread_samples, structPtr->circ_buf[i].max_samples, numSamples);
      }
      /* Keep writing to other channels even if one channel overflows */
      if (CIRCBUF_FAIL == result)
      {
         break;
      }
   }
   return result;
}
circbuf_status_enum voice_multi_ch_circ_buf_read_request(voice_multi_ch_circ_buf_t *structPtr,
      int32_t numSamples, uint32_t *availableSamplesPtr)
{
   circbuf_status_enum  result = CIRCBUF_SUCCESS;
   for (int32_t i = 0; i < structPtr->num_channels; i++)
   {
      result = voice_circbuf_read_request((circbuf_struct *)&(structPtr->circ_buf[i]),
            numSamples, availableSamplesPtr);
      if (CIRCBUF_SUCCESS != result)
      {
         break;
      }
   }
   return result;
}

circbuf_status_enum voice_multi_ch_circ_buf_reset(voice_multi_ch_circ_buf_t *structPtr)
{
   circbuf_status_enum  result = CIRCBUF_SUCCESS;
   for (int32_t i = 0; i < structPtr->num_channels; i++)
   {
      result = voice_circbuf_reset((circbuf_struct *)&(structPtr->circ_buf[i]));
      if (CIRCBUF_SUCCESS != result)
      {
         break;
      }
   }
   return result;
}

