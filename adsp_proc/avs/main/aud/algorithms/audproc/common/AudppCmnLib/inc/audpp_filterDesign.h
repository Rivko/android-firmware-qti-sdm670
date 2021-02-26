/*======================= COPYRIGHT NOTICE ==================================*]
Copyright (c) 2008, 2012-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
[*===========================================================================*]
[*****************************************************************************]
[* FILE NAME:   audpp_filterDesign.h              TYPE: C-header file              *]
[* DESCRIPTION: Header file for filter design, adapted from QSound code.     *]
[*   when       who     what, where, why                                     *]
[*   --------   ---     -----------------------------------------------------*]
[*   03/30/06   xyp     Initial revision                                     *]
[*****************************************************************************/
#ifndef _AUDPP_FILTERDESIGN_H_
#define _AUDPP_FILTERDESIGN_H_
#include "audpp_util.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*=============================================================================
      Function Declarations 
=============================================================================*/
int32 Q23_cosine_norm_freq
(
    int32       freqHz,                     /* input frequency in Hz         */
    int32       sampleRate                  /* sampling rate                 */
);

/* Design functions.  1st order filters need three coefficients, biquads     */
/* need five.  Some filters have zeros which improve signal rejection        */
/* (e.g. a low-pass filter with zeros will be better at filtering out high   */
/* frequencies than one that doesn't) but run a little more slowly.          */

void designFirstOrderLowpassCoeffs
(
    int32       coeffsL32Q23[3],            /* coeffs in L32Q23              */
    int16       mB,                         /* attenuation at cutoff freq    */
    int32       freqHz,                     /* cutoff frequency              */
    int32       sampleRate,                 /* sampling rate                 */
    boolean     withZero                    /* with a zero or not at fs/2    */
);

void designFirstOrderHighpassCoeffs
(
    int32       coeffsL32Q23[3],            /* coeffs in L32Q23              */
    int16       mB,                         /* attenuation at cutoff freq    */
    int32       freqHz,                     /* cutoff frequency              */
    int32       sampleRate,                 /* sampling rate                 */
    boolean     withZero                    /* with a zero or not at fs/2    */
);

/* QSound version */
void DesignFirstOrderCoeffs_Zero
(
    int32 coeffs[3],                        /* coeffs in L32Q23              */
    int32 dc_mb,                            /* dc gain in mB                 */  
    int32 nyquist_mb,                       /* gain at fs/2 in mB            */
    int32 mb,                               /* gain at the following freq    */  
    int32 freqHz,                           /* freq for the above gain       */
    int32 sampleRate                        /* sampling rate                 */
);

void designBiquadLowpassCoeffs
(
    int32       coeffsL32Q23[5],            /* coeffs in L32Q23              */
    int16       mB,                         /* attenuation at cutoff freq    */
    int32       freqHz,                     /* cutoff frequency              */
    int32       sampleRate,                 /* sampling rate                 */
    boolean     withZero                    /* with a zero or not at fs/2    */
);

void designBiquadBandpassCoeffs
(
    int32       coeffsL32Q23[5],            /* coeffs in L32Q23              */
    int16       mB,                         /* attenuation at cutoff freq    */
    int32       freqHz,                     /* cutoff frequency              */
    int32       sampleRate,                 /* sampling rate                 */
    boolean     withZero                    /* with a zero or not            */
);

void designBiquadHighpassCoeffs
(
    int32       coeffsL32Q23[5],            /* coeffs in L32Q23              */
    int16       mB,                         /* attenuation at cutoff freq    */
    int32       freqHz,                     /* cutoff frequency              */
    int32       sampleRate                  /* sampling rate                 */
);

void cascadeFirstOrderFilters
(
    int32 coeffsL32Q23[5],                  /* biquad coeffs, L32Q23         */
    int32 uL32Q23[3],                       /* first-order filter coeffs     */
    int32 vL32Q23[3]                        /* first-order filter coeffs     */
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _FILTERDESIGN_H_ */
