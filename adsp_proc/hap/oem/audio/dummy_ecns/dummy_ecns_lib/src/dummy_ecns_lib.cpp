/**

 @brief Core library implementation of Dummy ECNS example

 */

/*-----------------------------------------------------------------------
 Copyright (c) 2012-2015 Qualcomm  Technologies, Inc.  All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.
 -----------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "dummy_ecns_lib.h"
#include "string.h"

void dummy_ecns_lib_init()
{
  return; // Initialize core processing library
}

void dummy_ecns_lib_reset()
{
  return; // reset core processing library
}

void dummy_ecns_lib_process (int16_t *ouput_buffer, uint32_t size_output, const int16_t *input_buffer, uint32_t nSamples_input, int16_t *far_ref_ptr, uint32_t nSamples_far)
{
	uint32_t  copy_size = (size_output <= nSamples_input)? size_output : nSamples_input;
	memcpy(ouput_buffer, input_buffer, copy_size);
}

