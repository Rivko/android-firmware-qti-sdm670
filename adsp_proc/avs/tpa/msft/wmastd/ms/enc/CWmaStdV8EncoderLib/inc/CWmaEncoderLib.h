#ifndef C_WMA_ENCODER_LIB_H
#define C_WMA_ENCODER_LIB_H

/* ========================================================================
   Wma encoder library wrapper header file

  *//** @file CWmaEncoder.h
  This is a wrapper code for WMA Core encoder library.
  the function in this file are called by the CWmaEncoder media module.
  It is derived from CAudioProcLib class

  Copyright (c) 2012 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  *//*====================================================================== */

/* =========================================================================
                             Edit History

   when       who     what, where, why
   --------   ---     ------------------------------------------------------
   11/09/11   SG     Created file.

   ========================================================================= */
#include "Elite_CAPI.h"
/* =======================================================================
 *                       DEFINITIONS AND DECLARATIONS
 * ====================================================================== */

	class CWmaEncoderLib : public ICAPI
	{

	public:
		
		void *m_pEncState;
		
		enum WMAEncParamIdx
       {
           eWmaEncodeMono = eIcapiMaxParamIndex,
           eWmaBitRate,
		   eWmaEndOfStream,
           eWmaNumOutBytes           
       };
	   
		/* =======================================================================
		*                          Public Function Declarations
		* ======================================================================= */

		/**
		* Constructor of CWmaEncoderLib
		*/
		CWmaEncoderLib ( ADSPResult &result );

		/**
		* Destructor of CWmaEncoderLib
		*/
		virtual ~CWmaEncoderLib ( );

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
		* Get the value of the WMA encoder parameters
		*
		* @param[in]   nParamIdx      Enum value of the parameter of interest
		* @param[out]  pnParamVal     Desired value of the parameter of interest
		*
		* @return   Success/fail
		*/
		virtual int CDECL GetParam ( int nParamIdx, int *pnParamVal );


		/**
		* Set the value of the WMA encoder parameters
		*
		* @param[in]   nParamIdx      Enum value of the parameter of interest
		* @param[out]  nPrarmVal      Desired value of the parameter of interest
		*
		* @return   Success/fail
		*/
		virtual int CDECL SetParam ( int nParamIdx, int nParamVal );

		/**
		* Encode audio bitstream and produce one frame worth of samples
		*
		* @param[in]   pInBitStream     Pointer to input bit stream
		* @param[out]  pOutSamples      Pointer to output samples
		* @param[out]  pOutParams       Pointer to output parameters
		*
		* @return     Success/failure
		*/
		virtual int CDECL Process ( const CAPI_BufList_t* pInSamples,
								   CAPI_BufList_t*       pOutBitStream,
								   CAPI_Buf_t*       pOutParams );

};

#endif /* C_WMA_ENCODER_LIB_H */

