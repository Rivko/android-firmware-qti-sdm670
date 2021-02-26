#ifndef EVS_DEC_API_H
#define EVS_DEC_API_H


/*============================================================================
  @file evs_dec_api.h

  Public header file for EVS decoder.

        Copyright (c) 2009-2010 Qualcomm Technologies Incorporated.
        All Rights Reserved.
        Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/avs.adsp/2.8.5/voice/algos/vocoder_evs/evs_lib/inc/evs_dec_api.h#1 $ */

/*
 @brief public API for EVS decoder lib
 */

signed long int    get_evs_dec_struct_size(void);

signed long int get_size_of_evs_dec_mem_req(signed long int sampling_rate);

void evs_reinit_dec(void* pEvsInitObject, unsigned short sampling_rate, signed short Voip_flag);

void evs_voc_dec_init (void* pEvsInitObject, unsigned short sampling_rate, void* dynamic_mem_ptr);

void evs_dec_process (void* pEvsInitObject, signed short  *in_ptr, signed short  *out_ptr);

void evs_dec_operating_mode(void* pEvsInitObject, signed short *bwidth_mode);

#endif /* #ifndef EVS_DEC_API_H */