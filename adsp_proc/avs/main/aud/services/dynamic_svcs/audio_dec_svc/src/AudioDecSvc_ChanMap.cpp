/** @file AudioDecSvc_ChanMap.cpp
This file contains utility functions for Elite Audio Decoder service.

Copyright (c) 2011-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/** 
========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/dynamic_svcs/audio_dec_svc/src/AudioDecSvc_ChanMap.cpp#1 $

when       who     what, where, why 
--------   ---     -------------------------------------------------------
07/22/11   rkc     Created file.

========================================================================== 
*/ 

#include "qurt_elite.h"
#include "AudioDecSvc_ChanMap.h"
#include "AudioDecSvc_Structs.h"
#include "audio_basic_op.h"
#include "audio_basic_op_ext.h"
#include "AudioDecSvc_CapiV2Util.h"
#include "AudioDecSvc_Util.h"

#define NUM_ARRAY_ELEM(a)        (sizeof(a)/sizeof((a)[0]))

/* Static function declarations */
static bool_t is_entry_present(uint8_t *list, uint32_t size, uint8_t entry);
static void route_inp_to_out(dec_out_chan_map_t *out_map, 
                             uint8_t inp_ch, 
                             uint8_t out_ch);
static bool_t is_duplicate_present(uint8_t *list, uint32_t size);

/*
 *Function Name:AudioDecSvc_GetOutChanMap
 *
 *Parameters: in_chan_map: input bitstream media format 
 *            out_chan_map: client specified channel mapping
 *                          and actual output channel mapping
 *
 *Description: Compute the actual output channel map 
 *based on the channel mapping provided by the client and 
 *the input media format. 
 *
 *Returns: Success/failure code
 *
 */

ADSPResult AudioDecSvc_GetOutChanMap(DecChannelMap_t *in_chan_map,
                                     dec_out_chan_map_t *out_chan_map)
{
   //intitialize output chan map params
   out_chan_map->num_out_chan = 0;
   memset(out_chan_map->in_chan_map, 0, sizeof(out_chan_map->in_chan_map));

   if(!out_chan_map->is_out_chan_map_received)
   {
      for(uint8_t i = 0; i < in_chan_map->nChannels; i++)
      {
         route_inp_to_out(out_chan_map,
               in_chan_map->nChannelMap[i],
               in_chan_map->nChannelMap[i]);
      }
   }
   else
   {
      for(uint8_t i = 0; i < in_chan_map->nChannels; i++)
      {
         //first check if the same channel exists in the output map also
         if(is_entry_present(out_chan_map->max_map.nChannelMap,
               out_chan_map->max_map.nChannels,
               in_chan_map->nChannelMap[i]))
         {
            route_inp_to_out(out_chan_map, in_chan_map->nChannelMap[i], in_chan_map->nChannelMap[i]);
            continue;
         }

         //otherwise check if the input channel can be routed to alternative output channels
         switch(in_chan_map->nChannelMap[i])
         {
            case PCM_CHANNEL_L:
            case PCM_CHANNEL_R:
               //check if we have FC channel
               if(is_entry_present(out_chan_map->max_map.nChannelMap,
                     out_chan_map->max_map.nChannels,
                     PCM_CHANNEL_C))
               {
                  route_inp_to_out(out_chan_map, in_chan_map->nChannelMap[i], PCM_CHANNEL_C);
               }
               //drop it otherwise
               break;
            case PCM_CHANNEL_C:
               //ideally we would want to route FC to both FL and FR if they are present in the output.
               //but force FC in the output in this case. this will avoid 2x output buffer size requirement
               //in the common case of:
               //1. a voice codec which always produces FC
               //2. the output channel mapping specifies a stereo device with FL and FR
               //we will let the matrix svc handle this special case and keep things simple here
               route_inp_to_out(out_chan_map, in_chan_map->nChannelMap[i], in_chan_map->nChannelMap[i]);
               break;
            case PCM_CHANNEL_LS:
               //check if we have rear left or rear left of center
               if(is_entry_present(out_chan_map->max_map.nChannelMap,
                     out_chan_map->max_map.nChannels,
                     PCM_CHANNEL_LB))
               {
                  route_inp_to_out(out_chan_map, in_chan_map->nChannelMap[i], PCM_CHANNEL_LB);
               }
               else if(is_entry_present(out_chan_map->max_map.nChannelMap,
                     out_chan_map->max_map.nChannels,
                     PCM_CHANNEL_RLC))
               {
                  route_inp_to_out(out_chan_map, in_chan_map->nChannelMap[i], PCM_CHANNEL_RLC);
               }
               break;
            case PCM_CHANNEL_RS:
               //check if we have rear right or rear right of center
               if(is_entry_present(out_chan_map->max_map.nChannelMap,
                     out_chan_map->max_map.nChannels,
                     PCM_CHANNEL_RB))
               {
                  route_inp_to_out(out_chan_map, in_chan_map->nChannelMap[i], PCM_CHANNEL_RB);
               }
               else if(is_entry_present(out_chan_map->max_map.nChannelMap,
                     out_chan_map->max_map.nChannels,
                     PCM_CHANNEL_RRC))
               {
                  route_inp_to_out(out_chan_map, in_chan_map->nChannelMap[i], PCM_CHANNEL_RRC);
               }
               break;
            case PCM_CHANNEL_CS:
               //can route to both RLC and RRC if present.
               //but avoid upmixing so as not to increase output buf size requirement
               break;
            case PCM_CHANNEL_TS:
            case PCM_CHANNEL_CVH:
            case PCM_CHANNEL_MS:
            case PCM_CHANNEL_LFE:
               //no suitable alternative channels to mix them to. drop them
               break;
            case PCM_CHANNEL_LB:
               //check if we have rear left of center or left surround
               if(is_entry_present(out_chan_map->max_map.nChannelMap,
                     out_chan_map->max_map.nChannels,
                     PCM_CHANNEL_RLC))
               {
                  route_inp_to_out(out_chan_map, in_chan_map->nChannelMap[i], PCM_CHANNEL_RLC);
               }
               else if(is_entry_present(out_chan_map->max_map.nChannelMap,
                     out_chan_map->max_map.nChannels,
                     PCM_CHANNEL_LS))
               {
                  route_inp_to_out(out_chan_map, in_chan_map->nChannelMap[i], PCM_CHANNEL_LS);
               }
               break;
            case PCM_CHANNEL_RB:
               //check if we have rear right of center or right surround
               if(is_entry_present(out_chan_map->max_map.nChannelMap,
                     out_chan_map->max_map.nChannels,
                     PCM_CHANNEL_RRC))
               {
                  route_inp_to_out(out_chan_map, in_chan_map->nChannelMap[i], PCM_CHANNEL_RRC);
               }
               else if(is_entry_present(out_chan_map->max_map.nChannelMap,
                     out_chan_map->max_map.nChannels,
                     PCM_CHANNEL_RS))
               {
                  route_inp_to_out(out_chan_map, in_chan_map->nChannelMap[i], PCM_CHANNEL_RS);
               }
               break;
            case PCM_CHANNEL_FLC:
            case PCM_CHANNEL_FRC:
               //not mixing with FL, FR or FC so as not to affect the volume levels of
               //these very common channels.
               break;
            case PCM_CHANNEL_RLC:
               //check if we have rear center
               if(is_entry_present(out_chan_map->max_map.nChannelMap,
                     out_chan_map->max_map.nChannels,
                     PCM_CHANNEL_CS))
               {
                  route_inp_to_out(out_chan_map, in_chan_map->nChannelMap[i], PCM_CHANNEL_CS);
               }
               //check if we have rear left
               else if(is_entry_present(out_chan_map->max_map.nChannelMap,
                     out_chan_map->max_map.nChannels,
                     PCM_CHANNEL_LB))
               {
                  route_inp_to_out(out_chan_map, in_chan_map->nChannelMap[i], PCM_CHANNEL_LB);
               }
               break;
            case PCM_CHANNEL_RRC:
               //check if we have rear center
               if(is_entry_present(out_chan_map->max_map.nChannelMap,
                     out_chan_map->max_map.nChannels,
                     PCM_CHANNEL_CS))
               {
                  route_inp_to_out(out_chan_map, in_chan_map->nChannelMap[i], PCM_CHANNEL_CS);
               }
               //check if we have rear right
               else if(is_entry_present(out_chan_map->max_map.nChannelMap,
                     out_chan_map->max_map.nChannels,
                     PCM_CHANNEL_RB))
               {
                  route_inp_to_out(out_chan_map, in_chan_map->nChannelMap[i], PCM_CHANNEL_RB);
               }
               break;
            default:
               //if anything else, just drop it
               break;
         }
      } //end of for(uint32_t i = 0; i < in_chan_map->nChannels; i++)
   }//Else - if(out_chan_map->is_out_chan_map_received)

   //make sure that the output map doesnt exceed 8 channels
   QURT_ELITE_ASSERT(out_chan_map->num_out_chan <= PCM_FORMAT_MAX_NUM_CHANNEL);

   //check if we have all the input channels in output map
   //if not all are present, downmix will be required
   out_chan_map->downmix_required = false;
   for(uint8_t i = 0; i < in_chan_map->nChannels; i++)
   {
      if(!is_entry_present(out_chan_map->out_chan, 
            out_chan_map->num_out_chan,
            in_chan_map->nChannelMap[i]))
      {
         out_chan_map->downmix_required = true;
         break;
      }
   }

   return ADSP_EOK;
}

typedef struct
{
   uint32_t                             first_samp_byte_pos;
   uint32_t                             next_samp_byte_offset;
} chan_params_t;

/*
 *Function Name:AudioDecSvc_GetOutput
 *
 *Parameters: pMe: decoder service instance
 *            out_buf: output buffer
 *            samples_per_ch: samples per channel
 *            bytes_per_samp: bytes per sample
 *            is_interleaved: interleaving flag;
 *                            if downmixed, always deinterleaved
 *            out_buf_size: ptr to modified output buffer size.
 *
 *Description: The given output buffer will be 
 *modified so as to match the output mapping
 *
 *Returns: Success/failure code
 *
 */
ADSPResult AudioDecSvc_GetOutput(AudioDecSvc_t *pMe,
                              AudioDecSvc_OutStream_t *pOutStrm,
                                 dec_CAPI_container_t *capi_cont,
                                 DecChannelMap_t *in_chan_map,
                                 void *out_buf, 
                                 uint32_t samples_per_ch, 
                                 uint32_t bytes_per_samp,
                                 uint32_t *out_buf_size)
{
   *out_buf_size = in_chan_map->nChannels * samples_per_ch * bytes_per_samp;

   int32_t temp_t;

   if(!pOutStrm->out_chan_map.downmix_required)
   {
      //nothing to do, just return
      return ADSP_EOK;
   }

   int16_t *scratch_buf = (int16_t*) capi_cont->scratch_out_buf;
   int32_t *scratch_buf_32 = (int32_t*) capi_cont->scratch_out_buf;

   for(uint8_t i = 0; i < pOutStrm->out_chan_map.num_out_chan; i++)
   {
      chan_params_t in_chan_params[PCM_FORMAT_MAX_NUM_CHANNEL];
      memset(in_chan_params, 0, sizeof(in_chan_params));

      dec_in_chan_map_t *in_out_chan_map = &pOutStrm->out_chan_map.in_chan_map[i];

      for(uint8_t j = 0; j < in_out_chan_map->num_in_chan; j++)
      {
         uint8_t inp_ch = in_out_chan_map->in_chan[j];

         //find out the position of this channel in the input
         uint8_t k = 0;
         for(k = 0; k < in_chan_map->nChannels; k++)
         {
            if(inp_ch == in_chan_map->nChannelMap[k])
            {
               break;
            }
         }

         QURT_ELITE_ASSERT(k < in_chan_map->nChannels);

         //input is always assumed to be deinterleaved, 
         //downmixed output will be deinterleaved
         in_chan_params[j].first_samp_byte_pos = k * bytes_per_samp * samples_per_ch;
         in_chan_params[j].next_samp_byte_offset = bytes_per_samp;

      } // end of for(uint32_t j = 0; j < pMe->out_chan_map.chan_map[i].num_in_chan; j++)

      // do the downmixing
      //for now assume that samples are 16 bit, revisit for other sample depths later
      for(uint32_t j = 0; j < samples_per_ch; j++)
      {
         int32_t sum = 0;

         if(BYTES_PER_SAMPLE_TWO == bytes_per_samp)
         {
            for(uint8_t k = 0; k < in_out_chan_map->num_in_chan; k++)
            {
               int8_t *samp_byte_ptr = (int8_t *)out_buf	+ in_chan_params[k].first_samp_byte_pos 
                                          + j * in_chan_params[k].next_samp_byte_offset;
   
               sum += *((int16_t *)samp_byte_ptr);
            }

            //do averaging
            sum /= (int32_t) in_out_chan_map->num_in_chan;

            //reduce the 32 bit sum to 16-bit
            *scratch_buf++ = s16_saturate_s32(sum);
         }
         else
         {
            for(uint8_t k = 0; k < in_out_chan_map->num_in_chan; k++)
            {
               int8_t *samp_byte_ptr = (int8_t *)out_buf	+ in_chan_params[k].first_samp_byte_pos 
                                          + j * in_chan_params[k].next_samp_byte_offset;
   
               /* sum += *((int32_t *)samp_byte_ptr); */
               temp_t = *((int32_t *)samp_byte_ptr);

               /* add with sat to ensure result saturates if it overflows beyond 32-bits */
               sum = s32_add_s32_s32_sat(sum, temp_t); 
            }

            //do averaging
            sum /= (int32_t) in_out_chan_map->num_in_chan;

            //After averaging, saturate to Q28
            //if sum has overflowed 29-bits, below left shift operation will saturate the result
            sum = Q6_R_asl_RI_sat(sum, QFORMAT_SHIFT_FACTOR);

            //scale it back to Q28
            *scratch_buf_32++ = (sum >> QFORMAT_SHIFT_FACTOR);
         }
         
      }
   } //end of for(uint32_t i = 0; i < pMe->out_chan_map.num_out_chan; i++)


   //copy back to the output buffer from scratch buffer

   uint32_t bytes_to_copy;

   if(BYTES_PER_SAMPLE_TWO == bytes_per_samp)
   {
      bytes_to_copy = (uint32_t)((int8_t*)scratch_buf - (int8_t*)capi_cont->scratch_out_buf);
   }
   else
   {

      bytes_to_copy = (uint32_t)((int8_t*)scratch_buf_32 - (int8_t*)capi_cont->scratch_out_buf);
   }
   
   memscpy(out_buf, bytes_to_copy, capi_cont->scratch_out_buf, bytes_to_copy);

   //update output buffer size
   *out_buf_size = bytes_to_copy;

   return ADSP_EOK;
}

/*
 *Function Name:AudioDecSvc_ProcessOutChanMapCmd
 *
 *Parameters:  me: ptr to decoder service instance
 *             cmd: set param command pointer
 *
 *Description: Processes output channel map command sent by ASM
 *
 *Returns: Success/failure
 *
 */

ADSPResult AudioDecSvc_ProcessOutChanMapCmd(AudioDecSvc_t *me, AudioDecSvc_OutStream_t *pOutStrm, asm_dec_out_chan_map_param_t *cmd)
{
   //check for valid parameters
   DecChannelMap_t chan_map;
   memscpy(&chan_map.nChannelMap, MAX_CHAN_MAP_CHANNELS * sizeof(cmd->channel_mapping[0]),
         cmd->channel_mapping, MAX_CHAN_MAP_CHANNELS * sizeof(cmd->channel_mapping[0]) );
   chan_map.nChannels = cmd->num_channels;

   if(chan_map.nChannels > MAX_CHAN_MAP_CHANNELS)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, 
         "Invalid # of channels (%d) in dec output chan map cmd", chan_map.nChannels);

      return ADSP_EBADPARAM;
   }

   //make sure the channel positions are in allowed ranges
   for(uint32_t i = 0; i < chan_map.nChannels; i++)
   {
      if( (chan_map.nChannelMap[i] < PCM_CHANNEL_L)
          || (chan_map.nChannelMap[i] > MAX_CHAN_MAP_CHANNELS) )
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, 
         "Invalid channel position (%d) in dec output chan map cmd", chan_map.nChannelMap[i]);

         return ADSP_EBADPARAM;
      }
   }

   //make sure there are no duplicate entries
   if(is_duplicate_present((uint8_t *)(chan_map.nChannelMap), chan_map.nChannels))
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, 
         "Channel positions are not unique in dec output chan map cmd. Check for duplicates!");

      return ADSP_EBADPARAM;
   }

   //copy the output channel map
   pOutStrm->out_chan_map.max_map = chan_map;

   //zero out previous format block to force recomputation of output channel map
   //at the time of sending data downstream
   memset(&me->mainCapiContainer->PrevFmtBlk, 0, sizeof(me->mainCapiContainer->PrevFmtBlk));

   if( !AudioDecSvc_IsPcmFmt(me->mainCapiContainer->dec_fmt_id))
   {
      //try applying output channel map on the capi
      //result of the set_param doesnt really matter - 
      //if the CAPI cannot do the downmixing (failure code),
      //decoder service will do default downmixing
      aud_dec_svc_set_pcm_out_chan_map(me->mainCapiContainer->capi_ptr,
            (uint16_t)AudioDecSvc_GetOutputStreamIndex(me, pOutStrm), &chan_map);
   }

   pOutStrm->out_chan_map.is_out_chan_map_received = true;

   for(uint32_t in_index = 0; in_index < DEC_SVC_MAX_OUTPUT_STREAMS; in_index++)
   {
      AudioDecSvc_InpStream_t *pInpStrm = me->in_streams_ptr[in_index];
      (void)AudioDecSvc_RecreatePcmBufsBasedOnMediaFmt(me, pInpStrm); //only for PCM this func does something.
   }

   return ADSP_EOK;
}


/*
 *Function Name:AudioDecSvc_IsChanMapChanged
 *
 *Parameters: num_chan_old: old # of channels 
 *            chan_map_old: old channel map
 *            num_chan_new: new # of channels
 *            chan_map_old: new channel map
 *
 *Description: checks if there is a diff in the # of 
 *             channels or if there is a diff in the
 *             order of the channels
 *
 *Returns: true if there are differences, false otherwise
 *
 */

bool_t AudioDecSvc_IsChanMapChanged(uint32_t num_chan_old, uint8_t *chan_map_old,
                                    uint32_t num_chan_new, uint8_t *chan_map_new)
{
   bool_t chan_map_changed = false;

   if(num_chan_old != num_chan_new)
   {
      chan_map_changed = true;
   }
   else
   {
      for(uint32_t i = 0; i < num_chan_old; i++)
      {
         if(chan_map_old[i] != chan_map_new[i])
         {
            chan_map_changed = true;
            break;
         }
      }
   }

   return chan_map_changed;
}

/*
 *Function Name:is_entry_present
 *
 *Parameters: list: the given list 
 *            size: # of entries in the list
 *            entry: entry to search for
 *
 *Description: Helper function to search for an entry in a given list
 *
 *Returns: true if entry is found in the list, false otherwise
 *
 */

static bool_t is_entry_present(uint8_t *list, uint32_t size, uint8_t entry)
{
   bool_t is_present = false;

   for(uint32_t i = 0; i < size; i++)
   {
      if(entry == list[i])
      {
         is_present = true;
         break;
      }
   }

   return is_present;
}

/*
 *Function Name:route_inp_to_out
 *
 *Parameters: out_map: output channel map 
 *            inp_ch: input channel to add
 *            out_ch: output channel into which the inp_ch
 *                    will get mixed.
 *
 *Description: Adds an input channel to the list of 
 *input channels getting mixed into the given output channel  
 *
 *Returns: None.
 *
 */

static void route_inp_to_out(dec_out_chan_map_t *out_map, 
                             uint8_t inp_ch, 
                             uint8_t out_ch)
{
   uint8_t i = 0;
   for(i = 0; i < out_map->num_out_chan; i++)
   {
      if(out_map->out_chan[i] == out_ch)
      {
         break;
      }
   }

   uint32_t num_in_chan = out_map->in_chan_map[i].num_in_chan;
   out_map->in_chan_map[i].in_chan[num_in_chan] = inp_ch;
   out_map->in_chan_map[i].num_in_chan++;
   
   if(i == out_map->num_out_chan)
   {
      out_map->out_chan[i] = out_ch;
      out_map->num_out_chan++;
   }
}

/*
 *Function Name:is_duplicate_present
 *
 *Parameters: list: list to search for duplicates 
 *            size: number of elements in the list 
 *
 *Description: searches the list for any duplicate entries
 *
 *Returns: false if all elements are unique; true otherwise
 *
 */

static bool_t is_duplicate_present(uint8_t *list, uint32_t size)
{
   for(uint32_t i = 0; i < size; i++)
   {
      uint32_t num_inst = 0;
      for(uint32_t j = 0; j < size; j++)
      {
         if(list[i] == list[j])
         {
            num_inst++;
            if(num_inst > 1)
            {
               return true;
            }
         }
      }
   }

   return false;
}
