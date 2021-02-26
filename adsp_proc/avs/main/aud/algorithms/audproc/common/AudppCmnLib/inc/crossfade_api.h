#ifndef CROSSFADE_API_H
#define CROSSFADE_API_H

/*============================================================================
  FILE:          crossfade_api.h

  OVERVIEW:      This file has the configaration and data structures, API for cross fade algorithm.

  DEPENDENCIES:  None

Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/avs.adsp/2.8.5/aud/algorithms/audproc/common/AudppCmnLib/inc/crossfade_api.h#1 $

  when       who     what, where, why
  ---------- ------- ---------------------------------------------------------
  2011-05-05 sivab   Initial revision.  
  2012-10-10 hesuh   1.0.1 revision. 
  2013-03-17 juihuaj 1.2.0 rev. for unified API update
============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/



#include "crossfade_calibration_api.h"






#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */




/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef enum CROSS_FADE_INPUT 
{
    CUR_INPUT = 0,
    NEW_INPUT,
    TOTAL_INPUT
}CROSS_FADE_INPUT;

typedef enum CROSS_FADE_RESULT 
{
   CROSS_FADE_SUCCESS = 0,
   CROSS_FADE_FAILURE,
   CROSS_FADE_MEMERROR
}CROSS_FADE_RESULT;


typedef struct cross_fade_static_t
{
    uint32      sample_rate;  
    uint32      data_width;
} cross_fade_static_t;



typedef struct cross_fade_lib_t
{ 
   
    void* cross_fade_lib_mem_ptr; // ptr to the total chunk of lib mem
    
} cross_fade_lib_t;



// cross fade lib mem req
typedef struct cross_fade_lib_mem_req_t
{ 
   
    uint32 cross_fade_lib_mem_size; 
    uint32 cross_fade_lib_stack_size; 
} cross_fade_lib_mem_req_t;

 

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/


// get lib mem req
// cross_fade_lib_mem_req_ptr: [out] Pointer to lib mem requirements structure
// cross_fade_static_ptr: [in] Pointer to static structure.
CROSS_FADE_RESULT audio_cross_fade_get_mem_req(cross_fade_lib_mem_req_t *cross_fade_lib_mem_req_ptr, cross_fade_static_t* cross_fade_static_ptr);



// partition and init the mem with default values
// cross_fade_lib_ptr: [in, out] Pointer to lib structure
// cross_fade_static_ptr: [in] Pointer to static structure
// mem_ptr: [in] Pointer to lib memory
// mem_size: [in] Size of the memory pointed by mem_ptr
CROSS_FADE_RESULT audio_cross_fade_init_memory(cross_fade_lib_t *cross_fade_lib_ptr, cross_fade_static_t* cross_fade_static_ptr, int8* mem_ptr, uint32 mem_size);




// retrieve params from lib mem
// cross_fade_lib_ptr: [in] Pointer to lib structure
// param_id: [in] ID of the param
// mem_ptr: [out] Pointer to the memory where params are to be stored
// mem_size:[in] Size of the memory pointed by mem_ptr
// param_size_ptr: [out] Pointer to param size which indicates the size of the retrieved param(s)
CROSS_FADE_RESULT audio_cross_fade_get_param(cross_fade_lib_t *cross_fade_lib_ptr, uint32 param_id, int8* mem_ptr, uint32 mem_size, uint32 *param_size_ptr);



// set the params in the lib mem with those pointed by mem_ptr
// cross_fade_lib_ptr: [in, out] Pointer to lib structure
// param_id: [in] ID of the param
// mem_ptr: [in] Pointer to the memory where the values stored are used to set up the params in the lib memory
// mem_size:[in] Size of the memory pointed by mem_ptr
CROSS_FADE_RESULT audio_cross_fade_set_param(cross_fade_lib_t *cross_fade_lib_ptr, uint32 param_id, int8* mem_ptr, uint32 mem_size);



// cross fade processing of single-channel audio signal
// cross_fade_lib_ptr: [in] Pointer to lib structure
// out_ptr: [out] Pointer to single-channel output PCM samples
// in_ptr[TOTAL_INPUT]: [in] Pointer to two single-channel input PCM samples. in_ptr[CUR_INPUT] is the one fading out and in_ptr[NEW_INPUT] is the one fading in.
// block_size: [in] Number of samples to be processed. block_size <= total period(in samples) must be true to ensure correct cross fade processing
CROSS_FADE_RESULT audio_cross_fade_process(cross_fade_lib_t *cross_fade_lib_ptr, int8 *out_ptr, int8 *in_ptr[TOTAL_INPUT], uint32 block_size);




#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* #ifndef CROSSFADE_API_H */
