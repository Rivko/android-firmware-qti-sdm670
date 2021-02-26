/*========================================================================
  This file contains AFE Sample rate conversion related apis

  Copyright (c) 2009-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.
 
  $Header: //components/rel/avs.adsp/2.8.5/afe/services/static/inc/AFESampRateConv.h#2 $
 ====================================================================== */
#ifndef _SAMP_RATE_CONV_H_
#define _SAMP_RATE_CONV_H_

/*==========================================================================
  Include files
  ========================================================================== */
#include "qurt_elite.h"
#include "EliteMsg.h"
#include "AFEInterface.h"
#include "capi_v2_dynamic_resampler.h"
#include "capi_v2_voice_resampler.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*==========================================================================
	  Macro Defines
========================================================================== */
#define SIZE_OF_ARRAY(a) (sizeof(a)/sizeof((a)[0]))

#define TWO_BYTES_PER_SAMPLE       2
#define RS_INTERNAL_BUFFER_SCALING 2
/* This is the scaling used for internal rs buffer (for fractional rs only) */

#define INSTANCE_MEM 1
#define CHANNEL_MEM  0

typedef enum
{
	FIR_BASED_RESAMPLER = 0,
	IIR_BASED_RESAMPLER = 1,
	INVALID_RESAMP_TYPE = 0xFFFF
} samp_rate_conv_type_t;

typedef enum
{
   RS_NO_BUFFERING   = 0,
   /* No Buffering needed */
   RS_INPUT_BUFFERING = 1,
   /* RS_INPUT_BUFFERING should be done when frac rate is at the input, we would start with one full buffered frame and
    * use until it is empty. */
   RS_OUTPUT_BUFFERING   = 2,
   /* RS_OUTPUT_BUFFERING should be done when frac rate is at the output, we would start with empty buffer and use it
    * till it is full i.e. one complete frame is buffered. */
} resamp_buff_scheme_t;


typedef enum
{
   RS_NORMAL     = 0,
   /* Read/Send buffers normally for any buffering scheme as well as no resampler involved */
   RS_BUFF_EMPTY = 1,
   /* Read twice(Rx) or don't send(Tx) in respective paths. It indicates that RS internal buffer is out of samples and need to buffer
    * up one input frame before proceeding forward.
    * Used in RS_INPUT_BUFFERING */
   RS_BUFF_FULL  = 2,
   /* Dont read (Rx) or send two(Tx) in respective paths. It indicates that RS internal buffer is completely filled by samples
    * and therefore need to send this extra frame out before proceeding forward.
    * Used in RS_OUTPUT_BUFFERING */
} resamp_buff_flag_t;

typedef struct samp_rate_converter
{
   void                  *resample_instance_ptr;
   /* resampler instance pointer*/
   uint32_t              frame_size_in;
   /* Number of samples into the resampler*/
   uint32_t              frame_size_out;
   /* Expected samples out of the resampler*/
   samp_rate_conv_type_t resamp_type;
   /* Represents the resampler type in use*/
   uint32_t              bytes_per_sample;
   /* bytes_per_sample of resampler */
   uint32_t              in_ch_spacing_in_bytes;
   /* channel spacing at input rate */
   uint32_t              out_ch_spacing_in_bytes;
   /* channel spacing at output rate */
   uint32_t              delay_in_us;
   /* delay in micro seconds based on input sampling rate*/
   uint32_t              kpps;
   /* resampler KPPS */
   uint32_t              bw;
   /* resampler BW */

   resamp_buff_scheme_t  buff_scheme;
   /* buffering scheme for fractional resampling only */
   uint32_t              rs_buff_ch_spacing_in_bytes;
   /* channel spacing of the internal buffer */
   int32_t               rs_buffered_samples_in_bytes;
   /* Buffered samples remaining for tracking the size of the buffer.
    * For RS_BUFFERING_UP, we would track the buffer space already filled bytes per ch
    * For RS_BUFFERING_DOWN, we would track the buffer space remaining bytes per ch
    */
   int8_t                *rs_buff_ptr;
   /* Pointer to the resampler internal buffer used in fractional resampling to copy to the Output
    * Its size RS_INTERNAL_BUFFER_SCALING times the size to be copied(Rx) or read(Tx) from DMA
    */
   resamp_buff_flag_t    rs_buff_flag;
   /* For tracking the RS process calls in order to reset the buffered samples close to this boundary.
    * Needed for upsampling case, for ex, 8 -> 176.4 Khz can generate upto OSR = (176.4/8) = 22 samples extra at anytime
    * Thats why we need to track this and reset the buffered samples at the boundary otherwise the next interrupt would
    * generate less samples than needed.
    *
    * Only needed for RS_BUFFERING_UP, as for RS_BUFFERING_DOWN the internal buffer gets totally consumed at the 440th
    * RS process call, thats why we dont need to track it. */
   uint32_t              rs_num_process;
   /* This is for tracking the number of times the rs_process has been called, see prev comment regarding why it is needed. */
   uint32_t              rs_buff_delay_us;
   /* delay in micro seconds based on buffering needs for fractional resampling */
   uint32_t              port_latency_mode;
   /** AFE port operating latency mode*/
}samp_rate_conv_t;

/**
* This function is to select the resampler type for the given 
* case 
*
* @param[in] resamp_type, requests the sampling rate converter 
*       to be used
* @param[in] inFreq, input frequency for the resampler. 
* @param[in] outFreq, output frequency for the resampler. 
* @param[in] bytes_per_sample, number of bytes per sample (Min.
*  		of client and port bytes per sample)
* @return samp_rate_conv_type_t, retuns resampler type
* */ 
samp_rate_conv_type_t get_samp_rate_conv_type(afe_client_resampler_type resamp_type, uint32_t inFreq, uint32_t outFreq, uint32_t  bytes_per_sample);

/**
* This function is the wrapper function for the initialization 
* of various resamplers (Sample rate converters) 
*
* @param[in] pClientInfo, pointer to the client instance
* @param[in] bit_width, number of bits per sample (Min.
*  		of client and port bits per sample)
* @param[in] inFreq, input frequency for the resampler. 
* @param[in] outFreq, output frequency for the resampler. 
* @param[in] samp_rate_conv, pointer to sample rate 
*  		converter (resampler) instance.
* @return ADSPResult, retuns success or failure of the 
*  		  initialization
*  		  Caller should take care of deallocating any memory
*  		  created for this API call in case of failure
* */ 
/* Intialization*/
ADSPResult sample_rate_conv_init( samp_rate_conv_t *samp_rate_conv, uint16_t num_channels,  
	uint32_t bit_width, uint32_t inFreq, uint32_t outFreq, samp_rate_conv_type_t resamp_type);


/**
* This function is the wrapper function for the various 
* resamplers (Sample rate converters) 
*
* @param[in] samp_rate_conv, resampler strcture
* @param[in] in_buf, input buffer for the resampler. 
* @param[in] out_buf, output buffer for the resampler. 
* @param[in] nchan, channel number. 
* @param[out] samples_processed, actual number of samples 
*  		processed by resampler.
* @param[in] mode, resampler mode. * 
* @return ADSPResult 
*/
/* Process */
ADSPResult sample_rate_conv_process(samp_rate_conv_t *samp_rate_conv, int8 *inBuf, int8 *outBuf, uint32 *samplesprocessed, uint16_t nChan);

/**
* This function is for freeing the memory created for the 
* resampler. 
*
* @param[in] resampler, pointer to instance of the resampler
* @param[in] resampler_memory, pointer to instance voice 
*  		resampler memory.
* @param[in] resamplerType, information about the type of the 
*  		resampler.
* @return none
*/
/* De Initialization */
void sample_rate_conv_deinit(samp_rate_conv_t *samp_rate_conv);

/**
* This function is the wrapper function for getting the 
* algorithm delay of various resamplers (Sample rate converters)
*  
* @param[in] samp_rate_conv, pointer to sample rate 
*  		converter (resampler) instance.
* @param[out] delay_us, delay of the resampler in micro sec.  
* */ 
void sample_rate_conv_get_delay(samp_rate_conv_t *samp_rate_conv, uint64_t *delay_us);


/**
* This function is to clear the data memory of voice resampler
* @param[in] samp_rate_conv, pointer to instance of the resampler
* @param[in] num_channels, number of channels
* return ADSPResult
*/
ADSPResult sample_rate_conv_memory_init(samp_rate_conv_t *samp_rate_conv, uint16_t num_channels);

/*This function should be called by connecting afe client, when re-sampler is needed.
 * If FIR re-sampler, gets client resampler kpps and BW through CAPIv2.
 * If IIR re-sampler, gets client resampler kpps from IIR re-sampler predefined table.
 *
 *pDevPort          : Used pDevPort to access mmpm_info_ptr, from which aggregated client kpps/bw is updated/stored
 *psNewClient       : Used psNewClient to access afe_client, to store individual client kpps/bw
 * */
void afe_port_aggregate_client_resampler_kpps_bw(void *pDevPort, void *psNewClient);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif /* #ifndef _SAMP_RATE_CONV_H_ */
