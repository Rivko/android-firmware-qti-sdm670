/*===========================================================================*
Copyright (c) 2011-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
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

#ifndef __NEWGENERICRESAMPLER_H_
#define __NEWGENERICRESAMPLER_H_

/*===========================================================================*
 *           INCLUDE HEADER FILES                                            *
 *===========================================================================*/
#include "rs_common.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/* structure for up-sampler */
typedef struct _typeUpSampStruct
{
  USPhase usPhase;            /* phase information of up-sampler             */
  Filters usFilt   ;          /* filter data structure for up-sampler        */
  int16   is32Bit;            /* flag for 32 bit operation                   */
  void*   pUsCoeffTempMem;  
  int32 specialMode;          // Special mode flag
  int32 dummy;                // added for 8-byte alignment
} typeUpSampStruct;

/* down-sampler memory structure for down-by-n */
typedef struct _typeDnMemStruct
{
  Filters  dsFilt;
  DSPhase  dsPhase;
  int32    scaleL32Q23;      /* Scale factor after up-sampling              */
  int16    qFacSca;          /* Q factor of the scale factor                */
  int32    maxInBufLen;      /* used to determine the maximum length of     */
                             /* input buffer that won't overflow inter-     */
                             /* mediate output buffer.                      */
  int16    is32Bit;          /* 32 bit flag                                 */
  int32	   dnSampLoopCount;  /* convolution loop count						*/
} typeDnMemStruct;

/* down-sampler structure */
typedef struct _typeDnSampStruct
{
  /* down-sampler's structure.  The down-sampler reusesup-sampler to
   * up-sample input signal to 2, 3, 4, 6, 12, 24 times output frequency,
   * then down-sample to desired output frequency */
  typeUpSampStruct        upSampStruct;
  typeDnMemStruct         dnMem;
  void*                  medBuf;
  int32                   medBufLen;
  void*   				  pDsCoeffTempMem;  
} typeDnSampStruct;	

typedef struct _GenericResamplerLib24SRC
{
	/* memories */
	
	/* Variables */	
	void  *pUsMem, *pDsMem, *pScrMem;
	void  *pChannelStruct, *pInstanceMem; 	
	int16 usStructSize, dsStructSize, usCoefSize, dsCoefSize, usMemSize, dsMemSize, scrMemSize,isUpsamp;	
	typeDnSampStruct dnSampStruct;
} GenericResamplerLib24SRC;

/*===========================================================================*
 *           FUNCTION DECLARATIONS FOR RESAMPLER                            *
 *===========================================================================*/
ResampReturnType resample(GenericResamplerLib24SRC* ptr, int32 *inBuf, int32 *outBuf, int32 inCnt, int32 outCnt, uint32 *samplesprocessed, int16 mode);
ResampReturnType resampler_initialize(GenericResamplerLib24SRC* ptr, int32 inFreq,int32 outFreq,int16 mode, int8* pCoeffMem);
ResampReturnType resample_getInstInputSize(GenericResamplerLib24SRC* ptr, uint32 uiNrOutSamps, uint32* retInpSize);
ResampReturnType resample_getInstOutputSize(GenericResamplerLib24SRC* ptr, uint32 uiNrInSamps, uint32* retOutSize);
ResampReturnType resample_getLibSize(int16 resampQuality, uint8	bytesPerSample, uint32* pLibSize, uint8 memType);
uint32 resample_getDelayInNumInputSamples(GenericResamplerLib24SRC* ptr);
#ifdef __cplusplus
}
#endif //__cplusplus

#endif  //__NEWGENERICRESAMPLER_H_
