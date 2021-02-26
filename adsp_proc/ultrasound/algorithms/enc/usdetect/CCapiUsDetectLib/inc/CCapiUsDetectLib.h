#ifndef C_CAPI_US_DETECT_LIB_H
#define C_CAPI_US_DETECT_LIB_H

/* ========================================================================
   Ultrasound signal detection library header file

  *//** @file CCapiUsDetectLib.h
  This is the ultrasound signal detection library.

  Copyright (c) 2012, 2013 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*//*====================================================================== */

/* =========================================================================
                             Edit History

   when       who           what, where, why
   --------   --------      ------------------------------------------------
   04/15/12   NR            Update ultrasound signal detection algorithm to 
                            filter narrow band interference  
   01/04/12   lb            Created this file

   ========================================================================= */


/* =======================================================================
 *                       DEFINITIONS AND DECLARATIONS
 * ====================================================================== */
#include <Elite_CAPI.h>
#include "typedef_ultrasound.h"
#include "us_capi_common.h"

//Structure to be used in INIT Params 
typedef struct UsDetectInitParamsStruct
{
   uint32 lNumberOfChannels;
   uint32 lChannelIdx;
   uint32 lChFrameSizeSamples;
   uint32 lBitsPerSample;
   uint32 lSamplingRate;
   uint32 lFrmAccumCntSamples;
   uint32 lFrmSkipCntMs;
   uint32 lStartFreqKHz;
   uint32 lEndFreqKHz;
   uint32 lEnergyThreshold;
   uint32 lNarrowBandLimit;
   uint32 lAlgoCfgFlags;

} UsDetectInitParamsType;


// signal detection result structure
typedef struct UsDetectResultStruct
{
   uint32 detectionDone;	// was signal detection done for the current frame?
   uint32 lSignalPresent;	// if signal detection was done, this is the result

} UsDetectResultType;

class CCapiUsDetectLib : public IUsCAPI
{

public:
     enum UsDetectIdx
      {
         // if you add parameters here, the first one should start at eUsCapiEncoderCommonMaxParamIndex
         // (us_capi_common.h)
         _QIDL_PLACEHOLDER_UsDetectIdx = 0x7fffffff
      };

private:

     uint32 m_lNumberOfChannels;
     uint32 m_lChannelIdx;
     uint32 m_lChFrameSizeSamples;
     uint32 m_lBitsPerSample;
     uint32 m_lSamplingRate;
     uint32 m_lFrmAccumCntSamples;
     uint32 m_lFrmAccumCnt;
     uint32 m_lFrmSkipCnt;
     uint32 m_lStartFreqKHz;
     uint32 m_lEndFreqKHz;
     uint32	m_lEnergyThreshold;
     uint32 m_lNarrowBandLimit;
     uint32 m_lEnableFFTScaling;

public:
   /* =======================================================================
    *                          Public Function Declarations
    * ======================================================================= */

   /**
    * Default Constructor of CCapiUsDetectLib
    */
   CCapiUsDetectLib ( );

   /**
    * Constructor of CCapiUsDetectLib that creates an instance of the library
    */
    CCapiUsDetectLib ( ADSPResult &nRes );

   /**
    * Destructor of CCapiUsDetectLib
    */
   virtual ~CCapiUsDetectLib ( );

   /*************************************************************************
    * CAudioProcLib Methods
    *************************************************************************/

   /**
    * Initialize the core encoder library
    *
    * @return     success/failure is returned
    */
   virtual int CDECL Init ( CAPI_Buf_t* pParams );

   /**
    * Re initialize the core encoder library in the case of repositioning or
    * when full initialization is not required
    *
    * @return     success/failure is returned
    */
   virtual int CDECL ReInit ( CAPI_Buf_t* pParams );

   /**
    * Gracefully exit the core encoder library
    *
    * @return     success/failure is returned
    */
   virtual int CDECL End ( void );

   /**
    * Get the value of the encoder parameters
    *
    * @param[in]   nParamIdx      Enum value of the parameter of interest
    * @param[out]  pnParamVal      Desired value of the parameter of interest
    *
    * @return  Success/fail
    */
   virtual int CDECL GetParam ( int nParamIdx, int *pParamVal );

   /**
    * Get the value of the encoder parameters
    *
    * @param[in]   nParamIdx      Enum value of the parameter of interest
    * @param[out]  nParamVal      Desired value of the parameter of interest
    *
    * @return  Success/fail
    */
   virtual int CDECL SetParam ( int nParamIdx, int nParamVal );

   /**
    * Process one frame of data. If enough samples are collected, this
    * will result in spectrum analysis as well. Otherwise, samples are only
    * collected.
    *
    * @param[in]   pInBitStream     Pointer to input bit stream
    * @param[out]  pOutSamples      Pointer to output samples
    * @param[out]  pOutParams       Pointer to output parameters
    *
    * @return     Success/failure
    */
   virtual int CDECL Process ( const CAPI_BufList_t* pIndata,
                               CAPI_BufList_t*       pOutData,
                               CAPI_Buf_t*       pOutParams );

private:

    void FreeAllocatedBuffers();

    static const uint32 FIRST_CHANNEL_MASK = 0xFF;
    static const uint32 FIRST_CHANNEL_SHIFT = 0;
    static const uint32 SECOND_CHANNEL_MASK = 0xFF00;
    static const uint32 SECOND_CHANNEL_SHIFT = 8;
    static const uint32 THIRD_CHANNEL_MASK = 0xFF0000;
    static const uint32 THIRD_CHANNEL_SHIFT = 16;
    static const uint32 CHANNELS_PER_BUFFER = 4;

    // Bit 0 of lAlgoCfgFlags is used to enable/disable fft scaling
    static const uint32 ALGO_CFG_FFT_SCALING_ENABLE_MASK = 0x1;
    static const uint32 ALGO_CFG_FFT_SCALING_ENABLE_SHIFT = 0;

    CWord2x16*  m_fftInBuf;
    CWord2x16*  m_fftOutBuf;
    uint32   	m_lFrmCtr;
    uint32   	m_lFftInputSizeSamples;
    uint32   	m_lFftOutputSizeSamples;

};

#endif /* C_CAPI_US_DETECT_LIB_H */

