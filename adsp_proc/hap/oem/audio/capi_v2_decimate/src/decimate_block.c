/* ======================================================================== */
/**
   @file decimate_block.c

   C source file to implement decimation.
 */

/*==============================================================================
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

#include "decimate_block.h"

/*------------------------------------------------------------------------
  Function name: decimate_block
  Performs block decimation on input signal. All state information must
  be managed by user.
 * -----------------------------------------------------------------------*/
void decimate_block(
		int16_t* in_ptr,
		uint32_t input_num_samples,
		uint32_t* output_num_samples,
		int32_t decimate_factor,
		int16_t* out_ptr)
{
	int32_t i, j=0;
	int32_t output_samples = input_num_samples/decimate_factor;

	for (i=0;i<output_samples;i++)
	{
		out_ptr[i] = in_ptr[j];
		j = j + decimate_factor;
	}
	*output_num_samples= (output_samples);
}

