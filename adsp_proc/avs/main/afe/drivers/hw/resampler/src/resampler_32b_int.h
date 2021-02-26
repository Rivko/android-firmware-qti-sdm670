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
/* FILE NAME: resampler_32b.h                                                *
 * DESCRIPTION:                                                              *
 *   Internal header file for resampler.c file.   Contains typedefs, macros, *
 *   structures, variables and function declarations used in resampler.c file*
 *===========================================================================*/
/* REVISION HISTORY:                                                         *
 *                                                                           *
 *   when        who      what, where, why                                   *
 *   --------    ------   ---------------------------------------------------*
 *   08/05/11    swang    Initial release to support 32 bit processing.      *
 *===========================================================================*/

#ifndef     _RESAMPLER_32B_INT_H_
#define     _RESAMPLER_32B_INT_H_

/*===========================================================================*
 *           INCLUDE HEADER FILES                                            *
 *===========================================================================*/
#include "basic_op.h"
#include "rs_common.h"

/*===========================================================================*
 *           MACROS, CONSTANTS, STRUCTURES, AND FUNCTION DECLARATIONS        *
 *           FOR UP-SAMPLER                                                  *
 *===========================================================================*/

/* structure for up-sampler */
typedef struct 
{
  USPhase usPhase;            /* phase information of up-sampler             */
  Filters usFilt   ;          /* filter data structure for up-sampler        */
  int16   is32Bit;            /* flag for 32 bit operation                   */
  int16   isDynamic;          /* flag for dynamic resampling support          */
  int16   extraDelay;         /* extra delay introduced by resampling support */
} typeUpSampStruct;

/* down-sampler structure for down-by-n */
typedef struct 
{
  Filters  dsFilt;
  DSPhase  dsPhase;
  int32    scaleL32Q23;      /* Scale factor after up-sampling              */
  int16    qFacSca;          /* Q factor of the scale factor                */
  int32    maxInBufLen;      /* used to determine the maximum length of     */
                             /* input buffer that won't overflow inter-     */
                             /* mediate output buffer.                      */
  int16    is32Bit;          /* 32 bit flag                                 */
  int32    scaleHalf;        /* rounding factor                             */
  int16   isDynamic;         /* flag for dynamic resampling support          */
  int16   extraDelay;        /* extra delay introduced by resampling support */
} typeDnMemStruct;

/* down-sampler structure */
typedef struct 
{
  /* up-sampler's structure.  The down-sampler reusesup-sampler to 
   * up-sample input signal to 2, 3, 4, 6, 12, 24 times output frequency, 
   * then down-sample to desired output frequency */
  typeUpSampStruct        upSamp;
  typeDnMemStruct         dnMem;
  int32*                  medBuf;
  int32                   medBufLen;
} typeDnSampStruct;

int32 down_by_n_calc_fixout(DSPhase*, int16, int32);

#endif  /* _RESAMPLER_32B__INT_H_ */
