#ifndef EVS_ENC_API_H
#define EVS_ENC_API_H
/*============================================================================
  @file evs_enc_api.h

  Public header file for EVS encoder.

        Copyright (c) 2009-2010 Qualcomm Technologies Incorporated.
        All Rights Reserved.
        Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/avs.adsp/2.8.5/voice/algos/vocoder_evs/evs_lib/inc/evs_enc_api.h#1 $ */

/*
 @brief public API for EVS encoder lib
 */
typedef struct evs_enc_param_struct
{
   unsigned int enc_sampling_rate;
   unsigned int enc_bit_rate;
   unsigned int enc_bandwidth;
   unsigned int evs_channel_aware_mode;
   unsigned int fec_offset;
   unsigned int fer_rate;
   unsigned int dtx_enable;
   unsigned int dtx_sid_interval;
}evs_enc_param_struct;

/*
 * Function: get_sizeof_evs_enc_struct.
 *   To be called prior to initializing the encoder, to derive size requirements.
 *
 * Returns: Size requirements of EVS encoder.
 */
signed long int   get_sizeof_evs_enc_struct(void);

/*
 * Function: get_size_of_evs_enc_mem_req.
 *   To be called prior to initializing the encoder, to derive internal memory size requirements.
 *
 * Returns: Internal size requirements of EVS encoder.
 */
signed long int  get_size_of_evs_enc_mem_req(signed long int sampling_rate);

/*
 * Function: evs_reinit_enc
 *   To be called each time select EVS encoder configuration parameters are updated.
 *
 * Returns:
 *    0: SUCCESS
 *   -1: FAILURE
 */
int evs_reinit_enc(void* pEvsInitObject, evs_enc_param_struct * pParamStruct);

/*
 * Function: evs_voc_init_enc
 *   To be called in order to initialize the vocoder.
 *
 * Returns:
 *    0: SUCCESS
 *   -1: FAILURE
 */
int evs_voc_init_enc(void* pEvsInitObject, evs_enc_param_struct * pParamStruct, void* dynamic_mem_ptr);

/*
 * Function: evs_enc_process.
 *   Primary processing routine.
 *
 * Returns: void
 */
void evs_enc_process(void* pEvsInitObject, signed short   *in_ptr, signed short   *out_ptr );


#endif /* #ifndef EVS_ENC_API_H */
