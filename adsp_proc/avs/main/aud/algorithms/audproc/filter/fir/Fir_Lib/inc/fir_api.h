#ifndef FIR_API_H
#define FIR_API_H
/*============================================================================
  @file fir_api.h

  Public api for FIR Filter library.

Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
/*============================================================================

 $Header: //components/rel/avs.adsp/2.8.5/aud/algorithms/audproc/filter/fir/Fir_Lib/inc/fir_api.h#1 $

  when       who     what, where, why
  ---------- ------- ---------------------------------------------------------
  2016-01-28 hesuh   Initial revision.  
 ============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "fir_calibration_api.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef enum FIR_RESULT 
{
   FIR_SUCCESS = 0,
   FIR_FAILURE,
   FIR_MEMERROR
}FIR_RESULT;


//FIR static params structure
typedef struct fir_static_struct_t
{
   
   uint32   data_width;						// 16: 16-bits sample or 32: 32-bits sample
   //uint32   sample_rate;					// Hz   
   uint32	max_num_taps;					// Max FIR filter length
   
} fir_static_struct_t;



// FIR lib structure
typedef struct fir_lib_t
{ 
   
    void* lib_mem_ptr;						// ptr to the total chunk of lib mem
	
} fir_lib_t;


// FIR lib mem requirements structure
typedef struct fir_lib_mem_requirements_t
{ 
   
	uint32 lib_mem_size; // size of the lib mem pointed by lib_mem_ptr
	uint32 lib_stack_size; // stack mem size
} fir_lib_mem_requirements_t;


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
//#define fir_process fir_module_process
// FIR processing of de-interleaved multi-channel input audio signal sample by sample
// FIR_lib_ptr: [in] Pointer to lib structure
// out_ptr: [out] Pointer to de-interleaved multi - channel output PCM samples
// in_ptr: [in] Pointer to de-interleaved multi - channel input PCM samples
// sample_per_channel: [in] Number of samples to be processed per channel
FIR_RESULT fir_module_process(fir_lib_t *fir_lib_ptr, int8 *out_ptr, int8 *in_ptr, uint32 samples);


// get FIR lib mem size
// FIR_lib_mem_requirements_ptr: [out] Pointer to lib mem requirements structure
// static_struct_ptr: [in] Pointer to static structure
FIR_RESULT fir_get_mem_req(fir_lib_mem_requirements_t *fir_lib_mem_requirements_ptr, fir_static_struct_t* fir_static_struct_ptr);

// partition and init the mem with params
// FIR_lib_ptr: [in, out] Pointer to lib structure
// static_struct_ptr: [in] Pointer to static structure
// mem_ptr: [in] Pointer to the lib memory
// mem_size: [in] Size of the memory pointed by mem_ptr
FIR_RESULT fir_init_memory(fir_lib_t *fir_lib_ptr, fir_static_struct_t *fir_static_struct_ptr, int8 *mem_ptr, uint32 mem_size);

// set the params in the lib mem with those pointed by mem_ptr
// FIR_lib_ptr: [in, out] Pointer to lib structure
// param_id: [in] ID of the param
// mem_ptr: [in] Pointer to the memory where the values stored are used to set up the params in the lib memory
// mem_size:[in] Size of the memory pointed by mem_ptr
FIR_RESULT fir_set_param(fir_lib_t *fir_lib_ptr, uint32 param_id, int8 *mem_ptr, uint32 mem_size);

// retrieve params from lib mem
// FIR_lib_ptr: [in] Pointer to lib structure
// param_id: [in] ID of the param
// mem_ptr: [out] Pointer to the memory where params are to be stored
// mem_size:[in] Size of the memory pointed by mem_ptr
// param_size_ptr: [out] Pointer to param size which indicates the size of the retrieved param(s)
FIR_RESULT fir_get_param(fir_lib_t *fir_lib_ptr, uint32 param_id, int8 *mem_ptr, uint32 mem_size, uint32 *param_size_ptr);




#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* #ifndef FIR_API_H */
