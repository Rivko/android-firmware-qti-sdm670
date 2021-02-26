/*

 * Copyright (c) 2014-15 Qualcomm Technologies, Inc.  All Rights Reserved.

 * Qualcomm Technologies Proprietary and Confidential.

 *

 *

[*****************************************************************************]

[* FILE NAME:   audio_apiir_df1_opt.h                TYPE: C-header file     *]

[* DESCRIPTION: Contains global variables and fn declarations for equalizer. *]

[*   when       who     what, where, why                                     *]

[*   --------   ---     -----------------------------------------------------*]

[*   07/2015    hxia    Initial revision                                     *]

[*****************************************************************************/



#ifndef _AUDIO_IIRDF1_H_

#define _AUDIO_IIRDF1_H_



#include "AEEStdDef.h"



/*=============================================================================

      Function Declarations 

=============================================================================*/



#ifdef __cplusplus

extern "C" {

#endif /* __cplusplus */



/* To use the following function, it should be ensured that the Q-factor of den and */

/* num coefs should be <= Q27, in order to avoid overflow                           */

/* The suggested Q-factors are:                                                     */

/* Q1.27, Q2.26, Q3.25, Q4.24...                                                    */



/* This is the df1 allpass biquad iir function										*/

/* Only b0 and b1 is needed, using transfer function:								*/

/* H_ap(z) = (b0 + b1*z^-1 + z^-2)/(1 + b1*z^-1 + b0*z^-2)							*/

void iirDF1_32_ap(int32 *inp,

	int32 *out,

	int32 samples,

	int32 *numcoefs,

	int32 *mem,

	int16 qfactor);





#ifdef __cplusplus

}

#endif /* __cplusplus */



#endif  /* _AUDIO_IIRDF1_H_ */

