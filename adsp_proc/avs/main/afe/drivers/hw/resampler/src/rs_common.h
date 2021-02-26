/*===========================================================================* 
 * Copyright (c) 2005 by Qualcomm Technologies INC.  All rights reserved.                 * 
 * All data and information contained in or disclosed by this document is    * 
 * confidential and proprietary information of Qualcomm Technologies INC.  And, all       * 
 * rights therein are expressly reserved.  By accepting this material the    * 
 * recipient agrees that this material and the information contained therein * 
 * is held in confidence and in trust and will not be used, copyied,         * 
 * reproducedin whole or in part, nor its contents revealed in any manner to * 
 * others without the expressed written premission of Qualcomm Technologies INC.          * 
 *===========================================================================*/
/* FILE NAME: rs_common.h                                                    *
 * DESCRIPTION:                                                              *
 *   Header file for resampler project.  Contains typedefs, macros,          *
 *   variables and function declarations used in resampler project.          *
 *===========================================================================*/
/* REVISION HISTORY:                                                         *
 *                                                                           *
 *   when        who      what, where, why                                   *
 *   --------    ------   ---------------------------------------------------*
 *   08/30/05    swang    initial release of the file.                       *
 *   09/30/05    swang    modified for second release.                       *
 *                        removed MIN_FREQ, added Q18_SHIFT                  *
 *   05/03/10    swang    adding more functions for 32 bit support           *
 *   08/05/11    swang    Release for 16/32 bit resampler.                   *
 *===========================================================================*/

#ifndef  _RS_COMMON_H_
#define  _RS_COMMON_H_

#include "basic_op.h"

/*===========================================================================*
 *           TYPE DEFINITIONS                                                *
 *===========================================================================*/


/*===========================================================================*
 *           constants                                                       *
 *===========================================================================*/
#define     Q15_SHIFT           15
#define     Q20_SHIFT           20
#define     Q23_SHIFT           23
#define     MAX_Q15             32767
#define     MIN_Q15             -32768


/*===========================================================================*
 *           EXTERNAL VARIABLES                                              *
 *===========================================================================*/
extern const int16 oneQ15, halfQ15;
extern const int32 oneQ23, halfQ23;

/*===========================================================================*
 *           STRUCTURES                                                      *
 *===========================================================================*/
typedef struct
{
    int32   curFracPh;      /* current fractional phase                      */
    int32   fracPhStep;     /* fractional phase update step size             */
    int32   inFreq;         /* input sampling rate                           */
    int32   outFreq;        /* output sampling rate                          */
    uint32  fracConst32;    /* constant used to evaluate fractional phase    */
    uint16  fracConst;      /* constant used to evaluate fractional phase    */
//    int16   ptrStep;        /* memory increase step size                     */
    int16   curIntPh;       /* current integer phase                         */
    int16   intPhStep;      /* integer phase  update step size               */
    int16   fmemIndex;      /* filter memory index                           */
    int16   totalIntPh;     /* total integer phase                           */
    int16   shiftConst;     /* shift constant for evaluating fractional phase*/
    int16   interpMode;     /* interpolation mode: 0 -- linear, 1-quadratic  */
    int16   missSamp;       /* missing sample indicator                      */
    int16   asynch;         /* asynchronous indicator                        */
}   USPhase;

typedef struct
{
    int32   inFreq;         /* input sampling rate                          */
    int32   medFreq;        /* intermediate sampling rate                   */
    int32   outFreq;        /* output sampling rate                         */
    int16   dnSampFac;      /* down-sampling factor                         */
    int16   dnSampStep;     /* down-sampling step size, used for loading    */
                            /* filter memory                                */
    int16   convIndex;      /* filter index for convolution                 */
    int16   loadIndex;      /* filter index for loading filter memory       */
}   DSPhase;

typedef struct
{
    int32   halfQfac;       /* one half for a given Q factor                */
    void    *pMem;          /* point to filter memory                       */
    void    *pCoef;         /* point to filter coefficient                  */
    int16   coefLen;        /* length of filter coefficient                 */
    int16   coefLen2;       /* half length of filter coefficient if filter  */
                            /* coefficients is symmetric.                   */
    int16   coefStep;       /* convolution step size of filter coefficient  */
    int16   qFac;           /* Qfactor of the filter coefficients           */
    int16   memLen;         /* length of filter memory                      */
    int16   memStep;        /* convolution step size of filter memmory      */
}   Filters;

/*===========================================================================*
 *           FUNCTION DECLARATIONS                                           *
 *===========================================================================*/
int16 int_div_16(int16 num16, int16 den16, int16* rem16);
int32 int_div_32(int32 num32, int32 den32, int32* rem32);
int64 int_div_64(int64 num64, int64 den64, int64* rem64);
int32 frac_div_32(int32 num, int32 den, int16 Qfac);
int16 int_phase_update(USPhase*, int16);
int16 frac_phase_update(USPhase*);
void  prev_int_phase_update(USPhase*, int16*, int16*, int16);

#endif /* _RS_COMMON_H_ */
/*===========================================================================* 
 * End of rs_common.h                                                        * 
 *===========================================================================*/
