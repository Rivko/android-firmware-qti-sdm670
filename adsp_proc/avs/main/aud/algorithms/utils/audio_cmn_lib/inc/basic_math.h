/*===========================================================================* 
Copyright (c) 2009-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
 *===========================================================================*/
/* FILE NAME: basic_math.h                                                   *
 * DESCRIPTION:                                                              *
 *   Header file for basic_math.c file.                                      *
 *===========================================================================*/
/* REVISION HISTORY:                                                         *
 *                                                                           *
 *   when        who      what, where, why                                   *
 *   --------    ------   ---------------------------------------------------*
 *   07/14/08    dineshr  initial release of the file.                       *
 *   10/26/08    dineshr  added a division function div_int32_t() to the list  *
 *===========================================================================*/

#ifndef _BASIC_MATH_H_
#define _BASIC_MATH_H_

#include "mmdefs.h"
#include "adsp_error_codes.h"
#include "audio_basic_op.h"
#include "shared_aud_cmn_lib.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/
/* common voice lib */

// Commonly used Definitions
#define   NBITS		    16             // Number of bits in the voice data

// Cubic polynomial coefficients for the new square-root approximation
#define   SQRT_S0		0x0125         // C0 = 0.0089 in Q.15 format
#define   SQRT_S1		0xFCB45476     // C1 = -0.1029 in Q2.29 format
#define   SQRT_S2		0x15239E51     // C2 = 0.6605 in Q2.29 format
#define   SQRT_S3		0x0DDECD39     // C3 = 0.4334 in Q2.29 format

// Cubic polynomial coefficients for log10 approximation
#define   LOG10_2       0x1344         // log10(2)= 0.3010 in Q.14 format
#define   LOG_C0		0x178E         // C0 = 0.1840 in Q.15 format
#define   LOG_C1		0xDA997212     // C1 = -1.1687 in Q2.29 format
#define   LOG_C2		0x66FFB300     // C2 = 3.2187 in Q2.29 format
#define   LOG_C3		0xB8835AEE     // C3 = -2.2339 in Q2.29 format

// Cubic polynomial coefficients for exp approximation
#define   LOG2_EXP      0x2E2B         // log2(e)=1.4427 in Q2.13 format
#define   EXP_C0		0x09F6         // C0 = 0.0778 in Q.15 format
#define   EXP_C1		0x073A5F9A     // C1 = 0.2258 in Q2.29 format
#define   EXP_C2		0x1646FEAF     // C2 = 0.6962 in Q2.29 format
#define   EXP_C3		0x20000000     // C3 = 1 in Q2.29 format

#define SQRT_NTH_ORD 4
// Nth order polynomial coefficients for the new square-root approximation
#if SQRT_NTH_ORD == 5 // 5th
/* approx sqrt(x), 1<= x <4, by 5th order poly, < -74dB error */
   #define sqrt_compute_5th sqrt_compute_Nth
#endif
#if SQRT_NTH_ORD == 4 // 4th
/* approx sqrt(x), 1<= x <4, by 4th order poly, < -64dB error */
   #define sqrt_compute_4th sqrt_compute_Nth
#endif
#if SQRT_NTH_ORD == 3 // 3rd
/* approx sqrt(x), 1<= x <4, by 3rd order poly, < -52dB error */
   #define sqrt_compute_3th sqrt_compute_Nth
#endif

// Function Prototypes

/** moved to shared:
int16_t div_s (int16_t var1,
            int16_t var2);
*/

int32_t div_compute( int32_t numL32, 
			       int32_t denL32, 
			       int16_t *iqOutL16Ptr );

int32_t sqrt_compute  ( int32_t pwrInL32,
                      int16_t iqInL16,
				      int16_t iqOutL16 );

int32_t log_compute ( int32_t pwrInL32,
                      int16_t iqInL16,
				      int16_t iqOutL16,
                      int16_t logConstL16Q14 );

int32_t exp_compute ( int32_t xInL32,
                    int16_t iqInL16,
				    int16_t iqOutL16,
                    int16_t expConstL16Q13 );

int32_t div_int32_t( int32_t numL32, 
			     int32_t denL32, 
			     int16_t iqL16 );
				 
int32 div_int32( int32 numL32,
   		     int32 denL32,
   		     int16 iqL16
                );
				 
int32 sqrt_compute_Nth ( int32 pwrInL32,
                         int16 iqInL16,
                         int16 iqOutL16);
				 
#ifdef __cplusplus
}
#endif /*__cplusplus*/				 

#endif /* _BASIC_MATH_H_ */
