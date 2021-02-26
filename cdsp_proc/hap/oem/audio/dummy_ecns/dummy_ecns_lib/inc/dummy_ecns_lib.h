/**
 @brief This contains functions for DUMMY ECNS core lib

 */

/*========================================================================

 Edit History

 when       who     what, where, why
 --------   ---     -------------------------------------------------------


 ==========================================================================*/

/*-----------------------------------------------------------------------
 Copyright (c) 2012-2015 Qualcomm  Technologies, Inc.  All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.
 -----------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#ifndef DUMMY_ECNS_LIB
#define DUMMY_ECNS_LIB

/*Exact-width integer types */
typedef signed char int8_t;
typedef unsigned char   uint8_t;
typedef short  int16_t;
typedef unsigned short  uint16_t;
typedef long  int32_t;
typedef unsigned long   uint32_t;
typedef long long  int64_t;
typedef unsigned long long   uint64_t;


#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

void dummy_ecns_lib_init();

void dummy_ecns_lib_reset();

void dummy_ecns_lib_process (int16_t *ouput_buffer, uint32_t size_output, const int16_t *input_buffer, uint32_t nSamples_input, int16_t *far_ref_ptr, uint32_t nSamples_far);

#if defined(__cplusplus)
}
#endif // __cplusplus
#endif // DUMMY_ECNS_LIB
