#ifndef DECIMATE_BLOCK_H
#define DECIMATE_BLOCK_H
/* ======================================================================== */
/**
   @file decimate_block.c

   C source file to implement decimation.
 */

/*==============================================================================
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

#include "mmdefs.h"

#ifdef __cplusplus
extern "C" {
#endif

void decimate_block(
		int16_t* in_ptr,
		uint32_t input_num_samples,
		uint32_t* output_num_samples,
		int32_t decimate_factor,
		int16_t* out_ptr);

#ifdef __cplusplus
}
#endif

#endif // DECIMATE_BLOCK_H

