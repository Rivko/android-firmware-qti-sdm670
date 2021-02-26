#ifndef LIMITER_API_H
#define LIMITER_API_H
/*============================================================================
  @file limiter_api.h

  Public API for Low Distortion Limiter, including zero-crossing algorithm
  and peak history (lower distortion) algorithm

Copyright (c) 2013-2014, 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/
#include "limiter_calibration_api.h"

#if ((defined __hexagon__) || (defined __qdsp6__))
#define LIM_ASM
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*----------------------------------------------------------------------------
   Library Version
----------------------------------------------------------------------------*/
#define LIMITER_LIB_VER (0x01000202)   // lib version : 1.2.2.a1
                                       // (major.minor.bug) (8.16.8 bits)

/*----------------------------------------------------------------------------
   Error code
----------------------------------------------------------------------------*/
typedef enum LIMITER_RESULT {
   LIMITER_SUCCESS = 0,
   LIMITER_FAILURE,
   LIMITER_MEMERROR
} LIMITER_RESULT;

/*----------------------------------------------------------------------------
   Type Declarations
----------------------------------------------------------------------------*/
typedef struct limiter_static_vars_t { // ** static params
   int32             data_width;       //    16 (Q15) or 32 (Q27)
   int32             sample_rate;      //    sampling rate, range: all audio sampling rate
   int32             max_block_size;   //    max block size, range: any block size
   int32             num_chs;          //    num of channels
   int32             delay;            //    limiter delay (Q15 value of sec), range: 0 ~ 10 miliseconds
} limiter_static_vars_t;

/*----------------------------------------------------------------------------
   Type Declarations for new limiter v2
----------------------------------------------------------------------------*/
typedef struct limiter_static_vars_v2_t { // ** static params
   int32             data_width;       //    16 (Q15) or 32 (Q27 or Q31)
   int32             sample_rate;      //    sampling rate, range: all audio sampling rate
   int32             max_block_size;   //    max block size, range: any block size
   int32             num_chs;          //    num of channels
   int32             delay;            //    limiter delay (Q15 value of sec), range: 0 ~ 10 miliseconds
   int32             q_factor;         //    Q factor of input/output data
   int32             history_winlen;   //    length of history window (Q15 value of sec), range: 0 ~ 40 miliseconds
                                       //    0 - zero-crossing algo; > 0 - peak history algo
} limiter_static_vars_v2_t;

typedef struct limiter_mem_req_t {     // ** memory requirements
   uint32            mem_size;         //    lib mem size (lib_mem_ptr ->)
   uint32            stack_size;       //    stack mem size
} limiter_mem_req_t;

typedef struct limiter_lib_t {         // ** library struct
    void*            mem_ptr;          //    mem pointer
} limiter_lib_t;

/*-----------------------------------------------------------------------------
   Function Declarations
-----------------------------------------------------------------------------*/

// ** Process one block of samples (multi channel)
// lib_ptr: [in] pointer to library structure
// out_ptr: [out] pointer to output sample block (multi channel double ptr)
// in_ptr: [in] pointer to input sample block (multi channel double ptr)
// samples: [in] number of samples to be processed per channel
LIMITER_RESULT limiter_process(limiter_lib_t *lib_ptr, void **out_ptr, int32 **in_ptr, uint32 samples);

// ** Get library memory requirements
// mem_req_ptr: [out] pointer to mem requirements structure
// limiter_static_vars_t: [in] pointer to static variable structure
LIMITER_RESULT limiter_get_mem_req(limiter_mem_req_t *mem_req_ptr, limiter_static_vars_t* static_vars_ptr);

// ** Get library memory requirements for new limiter v2
// mem_req_ptr: [out] pointer to mem requirements structure
// limiter_static_vars_v2_t: [in] pointer to static variable structure v2
LIMITER_RESULT limiter_get_mem_req_v2(limiter_mem_req_t *mem_req_ptr, limiter_static_vars_v2_t* static_vars_v2_ptr);

// ** Partition, initialize memory, and set default values
// lib_ptr: [in, out] pointer to library structure
// static_vars_ptr: [in] pointer to static variable structure
// mem_ptr: [in] pointer to allocated memory
// mem_size: [in] size of memory allocated
LIMITER_RESULT limiter_init_mem(limiter_lib_t *lib_ptr, limiter_static_vars_t* static_vars_ptr, void* mem_ptr, uint32 mem_size);

// ** Partition, initialize memory, and set default values
// lib_ptr: [in, out] pointer to library structure
// static_vars_v2_ptr: [in] pointer to static variable structure v2
// mem_ptr: [in] pointer to allocated memory
// mem_size: [in] size of memory allocated
LIMITER_RESULT limiter_init_mem_v2(limiter_lib_t *lib_ptr, limiter_static_vars_v2_t* static_vars_v2_ptr, void* mem_ptr, uint32 mem_size);

// ** Set parameters to library
// lib_ptr: [in, out] pointer to lib structure
// param_id: [in] parameter id
// param_ptr: [in] pointer to the memory where the new values are stored
// param_size:[in] size of the memory pointed by param_ptr
LIMITER_RESULT limiter_set_param(limiter_lib_t* lib_ptr, uint32 param_id, void* param_ptr, uint32 param_size);

// ** Get parameters from library
// lib_ptr: [in] pointer to library structure
// param_id: [in] parameter id
// param_ptr: [out] pointer to the memory where the retrieved value is going to be stored
// param_size:[in] size of the memory pointed by param_ptr
// param_actual_size_ptr: [out] pointer to memory that will hold the actual size of the parameter
LIMITER_RESULT limiter_get_param(limiter_lib_t* lib_ptr, uint32 param_id, void* param_ptr, uint32 param_size, uint32 *param_actual_size_ptr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIMITER_API_H */
