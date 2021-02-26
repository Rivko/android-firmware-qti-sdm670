/*========================================================================
  This file contains AFE pop suppressor related apis

  Copyright (c) 2009-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

  $Header: //components/rel/avs.adsp/2.8.5/afe/algos/pop_suppressor/inc/AFEPopSuppressor_api.h#1 $
 ====================================================================== */
#ifndef _AFE_POPSUPPRESSOR_API_H_
#define _AFE_POPSUPPRESSOR_API_H_

/*==========================================================================
  Include files
  ========================================================================== */

#include "qurt_elite.h"



#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#define SAMPLING_RATE_96K 96000
#define SAMPLING_RATE_48K 48000
#define SAMPLING_RATE_32K 32000
#define SAMPLING_RATE_16K 16000
#define SAMPLING_RATE_8K  8000
#define RAMP_DURATION_IN_MS 1

typedef enum pop_supressor_process
{
   DATA_AVAILABLE = 0,
   DATA_UNAVAILABLE,
   RAMP_DOWN_PROCESS,
}pop_supressor_process_e;



ADSPResult afe_pop_suppressor_end(void** pop_suppressor_ptr);

ADSPResult afe_pop_suppressor_process(int8_t* in_ptr,int8_t** out_ptr,uint32_t input_size,uint32_t* output_size,void* pop_suppressor_instance_ptr,bool_t input_flag,bool_t* output_flag,int8_t process_mode);

ADSPResult afe_pop_suppressor_init(uint32_t client_sampling_rate,uint32_t client_number_of_channel,uint32_t client_bytes_per_channel,
      uint32 port_sampling_rate,void** pop_suppressor_module_ptr,uint32_t client_buf_size);

void afe_pop_suppressor_get_delay(void* pop_suppressor_instance_ptr, uint64_t *delay_us);


#ifdef __cplusplus
}
#endif //__cplusplus

#endif /* #ifndef _AFE_POPSUPPRESSOR_API_H_ */
