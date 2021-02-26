#ifndef C_MP3_ENCODER_LIB_H
#define C_MP3_ENCODER_LIB_H

/* ========================================================================
   Mp3 encoder library wrapper header file

  *//** @file CMp3EncoderLib.h
  This is a wrapper code for Mp3 Core encoder library.

Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
  *//*====================================================================== */

/* =========================================================================
                             Edit History

   when       who            what, where, why
   --------   --------     -- ----------------------------------------------
   02/25/13    WJ           Initial creation

   ========================================================================= */


/* =======================================================================
 *                       DEFINITIONS AND DECLARATIONS
 * ====================================================================== */
#include "Elite_CAPI.h"


class CMp3EncoderLib : public ICAPI
{

   private:
      void* m_enc_state_ptr;
       
      /*Default constructor private*/
      CMp3EncoderLib ( );

      EncRtrnCodeType mp3_encoder_mem_info_alloc(void* encoder_object_ptr);

   public:
   /* =======================================================================
    *                          Public Function Declarations
    * ======================================================================= */
       enum Mp3EncParamIdx
       {
           eMp3BitRate = (eIcapiMaxParamIndex+100),
		   eMp3EndOfStream
       };

      /**
       * Constructor of CMp3EncoderLib that creats an instance of the 
       * core encoder lib 
       */
      CMp3EncoderLib ( ADSPResult  &nRes );

      /**
       * Destructor of CMp3EncoderLib
       */
      virtual ~CMp3EncoderLib ( );

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
       * Re initialize the core encoder library in the case of 
       * repositioning or when full initialization is not required 
       *
       * @return     success/failure is returned
       */
      virtual int CDECL ReInit ( CAPI_Buf_t* pParams );

      /**
       * Gracefully exit the core enoder library
       *
       * @return     success/failure is returned
       */
      virtual int CDECL End ( void );

      /**
       * Get the value of the Mp3 encoder parameters
       *
       * @param[in]   nParamIdx      Enum value of the parameter of interest
       * @param[out]  pnParamVal     Desired value of the parameter of interest
       *
       * @return   Success/fail
       */
      virtual int CDECL GetParam ( int nParamIdx, int *pnParamVal );

      /**
       * Get the value of the Mp3 encoder parameters
       *
       * @param[in]   nParamIdx      Enum value of the parameter of interest
       * @param[out]  nPrarmVal      Desired value of the parameter of interest
       *
       * @return   Success/fail
       */
      virtual int CDECL SetParam ( int nParamIdx, int nParamVal );

      /**
       * Encode one frame of samples
       *
       * @param[in]   pInBitStream     Pointer to input bit stream
       * @param[out]  pOutSamples      Pointer to output samples
       * @param[out]  pOutParams       Pointer to output parameters
       *
       * @return     Success/failure
       */
      virtual int CDECL Process ( const CAPI_BufList_t* pInSamples,
                                  CAPI_BufList_t* pOutBitStream,
                                  CAPI_Buf_t* pOutParams );
};

#endif /* C_MP3_ENCODER_LIB_H */

