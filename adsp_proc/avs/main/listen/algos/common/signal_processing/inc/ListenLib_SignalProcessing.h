/*======================= COPYRIGHT NOTICE ==================================*]
[* Copyright (c) 2012-2016,2017 QUALCOMM Technologies, Incorporated.         *]
[* All Rights Reserved.                                                      *]
[* Confidential and Proprietary - Qualcomm Technologies, Inc.                *]
[*===========================================================================*]
[*****************************************************************************]
[* FILE NAME:   ListenLib_SignalProcessing.h   TYPE: C-header file          *]
[* DESCRIPTION: Listen computation functions (for MFCC & etc)                *]
[*   when       who     what, where, why                                     *]
[*   --------   ---     -----------------------------------------------------*]
[*   2012-01-27 minsubl Initial revision                                     *]
[*****************************************************************************/

#ifndef LISTENLIB_SIGNALPROCESSING_H_
#define LISTENLIB_SIGNALPROCESSING_H_

#include "AudioComdef.h"

/**
@brief listenLib_preprocess

apply pre-processing such as zero-mean, pre-emphasis filter, and hamming windowing

@param pFrameL32Q3 : [in] pointer to the input speech frame 
@param frameSize : [in] frame size to preprocess
@param pHammingTableQ13 : [in] pointer to the hamming window table
@param pFrameL32Q3: [out] pointer to the pre-processed speech frame
*/
void listenLib_preprocess(int32 *pFrameL32Q3, int16 frameSize, const int16* pHammingTableQ13);

/**
@brief listenLib_realft

 512pt real FFT 

@param pFrameL32Q3: [in] pointer to preprocessed frame array 
					(size: N_FFT -- 512 (not N_FRAME! latter elements must be 0-padded.))
@param pFftL32: [out] pointer to fft output array (size: N_FFT -- 512)
*/
void listenLib_realft(int32 *pFrameL32Q3, int32 *pFftL32);

/**
@brief listenLib_get_power_spectrum

obtain power spectrum from the fft output (Re^2 + Im^2)

@param pFftL32Q3 : [in] pointer to fft output array [Re0, Im0, Re1, Im1...] (size: N_FFT -- 512)
@param pPowerSpecL32Qn10: [out] pointer to power spectrum array (size: N_H_FFT -- 256)
*/
void listenLib_get_power_spectrum(int32 *pFftL32Q3, int32 *pPowerSpecL32Qn10);

/**
@brief listenLib_get_filter_bank

compute the energy inside each of triangular mel-freq filter bank

@param pPowerSpecL32Qn10 : [in] pointer to power spectrum array (size: N_H_FFT -- 256)
@param pFbankL32Q7: [out] pointer to filter bank energy array (size: NUM_CHAN -- 22)
*/
void listenLib_get_filter_bank(int32 *pPowerSpecL32Qn10, int32 *pFbankL32Q7);

/**
@brief listenLib_log_filter_bank

apply log to the filter bank energy

@param pFbank: [in] pointer to filter bank energy array Q7 (size: NUM_CHAN -- 22)
@param pFbank: [out] pointer to filter bank log-energy array Q11 (size: NUM_CHAN -- 22)
*/
void listenLib_log_filter_bank(int32 *pFbankL32);

/**
@brief listenLib_DCT

convert filter bank log energy to cepstrum by applying IDCT

@param pFbankL32Q11: [in] pointer to filter bank log-energy array (size: N_CHAN -- 22)
@param pFeatureL16Q8: [out] pointer to cepstrum array (size: NUM_CEPS -- 12)
*/
void listenLib_DCT(int32 *pFbankL32Q11, int16 *pFeatureL16Q8);

/**
@brief listenLib_weight_cepstrum

weight the cepstrum by multiplying pre-defined cepstrum weights

@param pCepsL16Q8: [in] pointer to cepstrum array (size: NUM_CEPS -- 12)
@param pCepsL16Q8: [out] pointer to weighted cepstrum array (size: NUM_CEPS -- 12)
*/
void listenLib_weight_cepstrum(int16 *pCepsL16Q8);

/**
@brief listenLib_get_c0

compute & return 0th coefficient of MFCC: sum of log-energies in the 22 filter banks

@param pFbankL32Q11: [in] pointer to filter bank log-energy array (size: N_CHAN -- 22)

@return computed 0th MFCC Q8 
*/
int16 listenLib_get_c0(int32 *pFbankL32Q11);

/*This function added here instead of basic op files, as it is using explictily 
  only for listen algos. */

int32 inv_sqrt_65 (int32 L_x);


#endif /* LISTENLIB_SIGNALPROCESSING_H_ */
