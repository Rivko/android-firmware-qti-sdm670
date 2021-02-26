#ifndef C_CAPI_ADPCM_ENCODER_LIB_H
#define C_CAPI_ADPCM_ENCODER_LIB_H

/* ========================================================================
   ADPCM encoder library wrapper header file

  *//** @file CADPCMEncoderLib.h
  This is a CAPI wrapper code for ADPCM Core encoder library.
  the function in this file are called by the CCapiADPCMEncoder media module.
  It is derived from AudioProcLib class

Copyright (c) 2010, 2012-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
  *//*====================================================================== */

/* =========================================================================
                             Edit History

   when       who            what, where, why
   --------   --------     -- ----------------------------------------------
   10/07/08   AP           Created this file for CAPI ADPCM Encoder

   ========================================================================= */


/* =======================================================================
 *                       DEFINITIONS AND DECLARATIONS
 * ====================================================================== */
#include <Elite_CAPI.h>


   class CADPCMEncoderLib : public ICAPI
   {

public:
      void *  m_pEncState; 

      enum ADPCMParamIdx
      {
         eADPCMNumBlockSize = eIcapiMaxParamIndex,
         eADPCMBitsPerSample,
         _QIDL_PLACEHOLDER_ADPCMParamIdx = 0x7fffffff
      };

   /* =======================================================================
    *                          Public Function Declarations
    * ======================================================================= */

   /**
    * Default Constructor of CADPCMEncoderLib
    */
   CADPCMEncoderLib ( );

   /**
    * Constructor of CADPCMEncoderLib that creats an instance of encoder lib
    */
   CADPCMEncoderLib ( ADSPResult    &nRes );

   /**
    * Destructor of CADPCMEncoderLib
    */
   virtual ~CADPCMEncoderLib ( );

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
    * Get the value of the ADPCM encoder parameters
    *
    * @param[in]   nParamIdx      Enum value of the parameter of interest
    * @param[out]  pnParamVal      Desired value of the parameter of interest
    *
    * @return  Success/fail
    */
   virtual int CDECL GetParam ( int nParamIdx, int *pParamVal );

   /**
    * Get the value of the ADPCM encoder parameters
    *
    * @param[in]   nParamIdx      Enum value of the parameter of interest
    * @param[out]  nParamVal      Desired value of the parameter of interest
    *
    * @return  Success/fail
    */
   virtual int CDECL SetParam ( int nParamIdx, int nParamVal );

   /**
    * Decode audio bitstream and produce one frame worth of samples
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

};

#endif /* C_CAPI_ADPCM_ENCODER_LIB_H */

