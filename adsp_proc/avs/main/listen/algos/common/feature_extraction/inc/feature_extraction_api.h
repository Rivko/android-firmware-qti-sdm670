/*======================= COPYRIGHT NOTICE ==================================*]
[* Copyright (c) 2012-2016,2017 QUALCOMM Technologies, Incorporated.         *]
[* All Rights Reserved.                                                      *]
[* Confidential and Proprietary - Qualcomm Technologies, Inc.                *]
[*===========================================================================*]
[*****************************************************************************]
[* FILE NAME:   feature_extraction_api.h              TYPE: C-header file    *]
[* DESCRIPTION: feature extraction wrapper.						             *]
[*   when       who     what, where, why                                     *]
[*   --------   ---     -----------------------------------------------------*]
[*   2011-00-00 kyou       Algorithm developed                               *]
[*   2012-01-27 minsubl    Initial revision.                                 *]
[*   2012-01-27 mjin       Static part only.                                 *]
[*   2012-08-27 acho       Code style revision.                              *]
[*   2010-09-20 acho       Add interface for dynamic allocation.             *]
[*   2012-10-11 kyou		Interface revision 2                             *]
[*****************************************************************************/

#ifndef _FEATURE_EXTRACTION_API_H
#define _FEATURE_EXTRACTION_API_H

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "AEEStdDef.h"
#include "listen_result.h"
#include "qurt_elite_types.h"

// For QDSP Simulation
#include "AudioComdef.h"
#include "audio_basic_op.h"
#include "ListenLib_Const.h"



/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

enum {
	BLOCK_SIZE_20MS = 320,
	BLOCK_SIZE_30MS = 480,
	BLOCK_SHIFT_10MS = 160,

	FEAT_DIM_13 = 13,
	FEAT_DIM_39 = 39,

	CIRC_BUF_LEN = 7,		// minimum number for delta/acc window of 2 (forward/backward)
};

// buffer index of FeatureExtractionDataStruct (for dynamic allocation)
enum {
	MMAP_ONEFRAME_L16 = 0,
	MMAP_ONEFRAME_L32,
	MMAP_FFT_L32,
	MMAP_POWERSPEC_L32,
	MMAP_FILTERBANK_L32,
	MMAP_CIRCULARBUF_L16,
	MMAP_NUM_BUF,
};

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/**
@brief Feature extraction parameter struct
*/
typedef struct
{
	int16 nInputSize;			// number of new pcm sample
	int16 nBlockSize;			// unit size of feature extraction processing
	int16 nFeatureDim;						// feature dimension
	const int16* pHammingTableL16Q13;		// hamming table
	boolean bUseDeltaDelta;					// flag to compute delta and acc
	int32 nRmsThresholdL32Q13;				// rms threshold
} FeatureExtractionParamType;

/**
@brief Feature extraction data structure
*/
typedef struct
{
	// configuration parameters
	int16 nInputSize;						// number of new pcm sample
	int16 nBlockSize;						// unit size of feature extraction processing
	int16 nFeatureDim;						// feature dimension
	const int16* pHammingTableL16Q13;		// hamming table
	boolean bUseDeltaDelta;					// flag to compute delta and acc

	//PCM Frame window to compute feature
	int16* aOneFrameL16Q0;
	//32bit PCM Frame window to compute feature
	int32* aOneFrameL32Q3;
	//fft result vector
	int32* aFftL32Q3;
	//power spectrum buffer
	int32* aPowerSpecL32Qn10;
	//filter bank buffer
	int32* aFilterBankL32;		 // Q7 -> Q11 (changed inside 'listenLib_log_filter_bank')
	// circular buffer to compute delta (2-dim array for circular buffer)
	int16* aCircularBufL16Q8;	 // Feature has values between [-32768, 32767] in Q8 format.
	// feature dimension + padding
	int16 nFeatureDimMult4;		// to make buffer size to multiple of 8 = multipleof4 * sizeof(int16)
	// current index of circular buffer
	int16 nCurrentCircularIndex;
	// frame count of input pcm
	int32 nFrameCount;

	int32 *aOneFrameBitRevL32Q3;

	int32 nRmsThresholdL32Q13;				// threshold for rms
	boolean bNeedCheckRms;					// flag to check rms value

} FeatureExtractionDataStruct;

/**
@brief feature extraction initialization

 initialize config and data structures using parameter structure

@param pData: [in] Pointer to data structure
@param pParam: [in] Pointer to parameter structure
@param pBuffer: [in] starting address of allocated dynamic memory
*/
ListenResult FeatureExtraction_init(FeatureExtractionDataStruct *pData, FeatureExtractionParamType *pParam, uint8 *pBuffer);

/**
@brief feature extraction initialization

  just initialize index variables

@param pData : [in] pointer to data structure
*/
ListenResult FeatureExtraction_reinit(FeatureExtractionDataStruct *pData);
#if 0
/**
@brief feature extraction stop

(currently no operations)

@param pData : [in] pointer to data structure
*/
ListenResult  FeatureExtraction_stop(FeatureExtractionDataStruct *pData);
#endif
/**
@brief feature extraction run

Receive new PCM samples and accumulate it, then calculate MFCC

@param pData : [in] pointer to data structure
@param pInputPcm: [in] pointer to audio pcm sample array
@param pFeatureVector: [out] pointer to mfcc feature vector
*/
ListenResult FeatureExtraction_extract_feature(FeatureExtractionDataStruct *pData, int16 *pInputPcm, int16 **pFeatureVector);

/**
@brief Check if the first DCEPS_SIZE MFCC feature is available

@param pData : [in] pointer to data structure
*/
ListenResult FeatureExtraction_first_cepstrum_ready(FeatureExtractionDataStruct *pData);

/**
@brief get specific buffer size for feature extraction

@param pParam : [in] pointer to parameter structure
@param mapIndex: [in] index of buffer to get size
@param outputSize: [out] buffer size (in bytes)
*/
ListenResult FeatureExtraction_getBufferSize(FeatureExtractionParamType *pParam, int16 mapIndex, int32 *outputSize);

/**
@brief get total buffer size for feature extraction

@param pParam : [in] pointer to parameter structure
@param outputSize: [out] total buffer size (in bytes)
*/
ListenResult FeatureExtraction_getTotalBufferSize(FeatureExtractionParamType *pParam, int32 *outputSize);

/**
@brief get specific buffer address inside allocated memory

@param pParam : [in] pointer to parameter structure
@param pMemoryMap : [in] pointer to allocated memory map
@param mapIndex : [in] index of buffer to get address

@return buffer address (in void* type)
*/
void* FeatureExtraction_getBufferPtr(FeatureExtractionParamType *pParam, void *pMemoryMap, int mapIndex);

#endif /*_FEATURE_EXTRACTION_API_H */

