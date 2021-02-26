
/* ========================================================================*/
/**
 @file EliteData_Util.cpp
 Utility file for data processing across the elite framework
 */

/*======================================================================
 Copyright (c) 2016 QUALCOMM Technologies Incorporated.
 All rights reserved. Qualcomm Proprietary and Confidential.
 ====================================================================== */

/* =========================================================================
 Edit History

 when       who     what, where, why
 --------   ---     ------------------------------------------------------
 06/27/16   rr      Created file.
 ========================================================================= */

#include "shared_aud_cmn_lib.h"
#include "EliteData_Util.h"

/** @ingroup elite_data_get_q_format_factor
  Provides the translation between bit width and Q Format to the caller.
*/
uint32_t elite_data_get_q_format_factor(uint32_t bit_width)
{
    uint32_t q_format;
	switch(bit_width)
	{
		case 16: 
		    q_format = PCM_16BIT_Q_FORMAT;
			break;
		case 24: 
		    q_format = ELITE_32BIT_PCM_Q_FORMAT; 
			break;
		case 32: 
		    q_format = PCM_32BIT_Q_FORMAT;
			break;
		default:
		    q_format = 0; /*RR: Any other default value?*/
	}
	return q_format;
}

/** @ingroup elite_data_get_bytes_per_sample
  Provides the bytes per sample corresponding to the input bit width.
*/
uint32_t elite_data_get_bytes_per_sample(uint32_t bit_width)
{
	uint32_t bytes_per_sample = 0;
	switch(bit_width)
	{
		case 16:
			bytes_per_sample = 2;
			break;
		case 24:
		case 32:
			bytes_per_sample = 4;
			break;
	}
	return bytes_per_sample;
}

