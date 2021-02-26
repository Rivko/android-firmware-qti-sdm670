#ifndef DTMF_DET_API_H
#define DTMF_DET_API_H
/*============================================================================
  @file dtmf_det_api.h

  Public header file for DTMF DETECTION.

        Copyright (c) 2010 Qualcomm Technologies Incorporated.
        All Rights Reserved.
        Qualcomm Confidential and Proprietary
============================================================================*/

/**
  @brief Initialize Dtmf Detection algorithm

  Performs initialization of data structures for the
  FNS algorithm. A pointer to static memory is storage
  is passed for configuring the FNS static configuration
  structure.

  @param params: [in] Pointer to tuning parameter list
  @param pStaticMem: [in,out] Pointer to static memory
  @return static memory size
 */


void dtmf_det_init_default( short *ptrDtmfDetectStaticParams);

/**
  @brief Initializes statics variables with default tuning 

  @param pInL16: [in] Pointer to input frame data
  @param pOutL16: [out] Pointer to output frame data
  @param pStaticMem: [in,out] Pointer to static memory storage
  @param pScratchMem: [in,out] Pointer to scratch memory storage
 */

void dtmf_det_init( short *ptrDtmfDetectStaticParams, short *ptrDtmfDetConfigParams);

/**
  @brief Initializes statics with choosen tuning parameters 
  @param pInL16: [in] Pointer to input frame data
  @param pOutL16: [out] Pointer to output frame data
  @param pStaticMem: [in,out] Pointer to static memory storage
  @param pScratchMem: [in,out] Pointer to scratch memory storage
 */



short dtmf_detect( short *Inpt, short *ptrDtmfDetectStaticParams);

short do_dtmf_debounce(short statusword,short *ptrDtmfDebounceStaticParams);

short dtmf_detect_writeflag( short previous, short current);

#endif /* #ifndef DTMF_DET_API_H */