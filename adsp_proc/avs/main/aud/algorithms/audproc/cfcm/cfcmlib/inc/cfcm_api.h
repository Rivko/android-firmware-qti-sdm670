#ifndef _CFCM_API_H_
#define _CFCM_API_H_
/*======================= COPYRIGHT NOTICE ==================================*
Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*===========================================================================*/
/*===========================================================================*]
[* FILE NAME: cfcm_api.h			                                         *]
[* DESCRIPTION:                                                              *]
[*   Public API for Crossover Filtering and Channel Mixing.                  *]
[*  REVISION HISTORY:                                                        *]
[*  when       who     what, where, why                                      *]
[*  ---------- ------- ----------------------------------------------------  *]
[*  2016/09/10 abdula  Initial version.                                      *]
=============================================================================*/
#include "cfcm_calibration_api.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*----------------------------------------------------------------------------
Error code
----------------------------------------------------------------------------*/
typedef enum CFCM_RESULT {
    CFCM_SUCCESS = 0,                           // Success
    CFCM_FAILURE,                               // Failure
    CFCM_MEMERROR,                              // Failure due to insufficient memory
    CFCM_NULL_PTR_ERR,                          // Failure due to null pointer
    CFCM_INVALID_PARAM_ID_ERR,                  // Failure due to invalid Param ID
    CFCM_VALUE_NOT_SUPPORTED_ERR                // Failure due to out-of-range value
} CFCM_RESULT;

#define CFCM_DELAY_USEC (1000) //delay for CFCM in usec

#define CFCM_DATA_WIDTH_16_BITS                 (16)
#define CFCM_DATA_WIDTH_24_BITS                 (32)

/*----------------------------------------------------------------------------
Type Declarations
----------------------------------------------------------------------------*/
typedef struct cfcm_static_vars_t{              // ** static params
    uint32            data_width;               //    supported values: 16 (Q15 PCM) or 24 (Q27 PCM)
    uint32            sample_rate;              //    supported value: 48000
    uint32            num_chs;                  //    supported value: 2
    uint32            frame_size_samples;       //    frame size. this is not max or min value. c-sim will
                                                //    accept this many number of samples every frame.
} cfcm_static_vars_t;

static const cfcm_static_vars_t cfcm_static_vars_default =
{
    16,         // Number of bits for input and output samples, (per channel)
    48000,      // PCM signal sampling rate
    2,	        // Number of channels
    240,	    // processing frame size in terms of number of samples per channel
};

typedef struct cfcm_mem_req_t {              // ** memory requirements
    uint32            mem_size;              //    lib mem size (static and scratch included)
    uint32            stack_size;            //    stack mem size
} cfcm_mem_req_t;

typedef struct cfcm_lib_t {                  // ** library struct
    void*            mem_ptr;               //    mem pointer
} cfcm_lib_t;

/*-----------------------------------------------------------------------------
Function Declarations
-----------------------------------------------------------------------------*/
// ** Process one block of samples (16-bit/24-bit multi channel)
// lib_ptr: [in] pointer to library structure
// out_ptr: [out] pointer to output sample block (16-bit/32-bit multi channel(mono/stereo) double ptr)
// in_ptr: [in] pointer to input sample block (16-bit/32-bit multi channel(mono/stereo) double ptr)
// num_samples: [in] number of samples to be processed per channel. number of input samples = number of output samples
CFCM_RESULT cfcm_process(cfcm_lib_t *lib_ptr, int8 *out_ptr[ ],  int8 *in_ptr[ ], uint32 num_samples );

// ** Get library memory requirements
// mem_req_ptr: [out] pointer to mem requirements structure
// static_vars_ptr: [in] pointer to static variable structure
CFCM_RESULT cfcm_get_mem_req(cfcm_mem_req_t *mem_req_ptr, cfcm_static_vars_t* static_vars_ptr);

// ** Partition, initialize memory, and set default values
// lib_ptr: [in, out] pointer to library structure
// static_vars_ptr: [in] pointer to static variable structure
// mem_ptr: [in] pointer to allocated memory
// mem_size: [in] size of memory allocated
CFCM_RESULT cfcm_init_mem(cfcm_lib_t *lib_ptr, cfcm_static_vars_t* static_vars_ptr, int8* mem_ptr, uint32 mem_size);

// ** Set parameters to library
// lib_ptr: [in, out] pointer to lib structure
// param_ptr: [in] pointer to the memory where the new values are stored
// param_id: [in] parameter ID
// param_size:[in] size of the memory pointed by param_ptr
CFCM_RESULT cfcm_set_param(cfcm_lib_t* lib_ptr, int8* param_ptr, uint32 param_id, uint32 param_size);

// ** Get parameters from library
// lib_ptr: [in] pointer to library structure
// param_id: [in] parameter id
// param_ptr: [out] pointer to the memory where the retrieved value is going to be stored
// param_size:[in] size of the memory pointed by param_ptr
// param_actual_size_ptr: [out] pointer to memory that will hold the actual size of the parameter
CFCM_RESULT cfcm_get_param(cfcm_lib_t* lib_ptr, int8* param_ptr, uint32 param_id, uint32 param_size, uint32 *param_actual_size_ptr);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _CFCM_API_H_ */
