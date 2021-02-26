#ifndef C_CAPI_EPOS_STYLUS_ENCODER_LIB_H
#define C_CAPI_EPOS_STYLUS_ENCODER_LIB_H

/* ========================================================================
   Adpcm encoder library wrapper header file

  *//** @file CCapiEposStylusEncoderLib.h
  This is a CAPI wrapper code for Adpcm Core encoder library.
  the function in this file are called by the CCapiAdpcmEncoder media module.
  It is derived from AudioProcLib class

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved. Qualcomm Proprietary and Confidential.
  *//*====================================================================== */

/* =========================================================================
                             Edit History

   when       who            what, where, why
   --------   --------     -- ----------------------------------------------
   01/04/12   lb           Added support for signal detection mode
   10/07/08   AP           Created this file for CAPI Adpcm Encoder
   11/01/12   NR           Fix CR 416796
   02/17/13   NR           Add support for LF + 4mics
   05/28/13   IM           Added support for up to 8 channels input.
   11/26/13   LD           Fix QUS-616: periodic packet loss with group factor 1
   ========================================================================= */


/* =======================================================================
 *                       DEFINITIONS AND DECLARATIONS
 * ====================================================================== */
#include <Elite_CAPI.h>
#include "us_capi_common.h"

#define MAX_SUPPORTED_CHANNELS 8
//Structure to be used in INIT Params 
typedef struct EposStylusInitParamsStruct
{
   uint32 usTypeID;        //codec ID
   uint32 usConfigBlkSize; //size of structure for error check
   void* usConfigBlk;      //Encoder COnfiguratoin block or Decoder format block

}EposStylusInitParamsType;


   class CCapiEposStylusEncoderLib : public IUsCAPI
   {

public:
     static const uint32 NUM_CHANNELS_FOR_AFE = 4;
     enum EposStylusIdx
      {
         eEposStylusFormatTag = eUsCapiEncoderCommonMaxParamIndex,
         eEposStylusNumberOfChannels,
         eEposStylusBitsPerSample,
         eEposStylusSamplingRate,
         eEposStylusEncoderMapping,
		 eEposStylusSkipFactor,
         eEposStylusGroupFactor,
         eEposStylusFrameSizeBytes,  // packet output size, includes headers
		 eEposStylusSamplesPerFrame,
         _QIDL_PLACEHOLDER_EposStylusIdx = 0x7fffffff
      };

   private:

     int32             m_lFormatTag;                 // Media Format Tag
     int16             m_sNumberOfChannels;          // Number of granules
     int16             m_sBitsPerSample;             // Bits Per Sample
     int32             m_lSamplingRate;              // Sampling Rate
     uint16            m_sSkipFactor;
     uint16            m_sGroupFactor;
   public:
   /* =======================================================================
    *                          Public Function Declarations
    * ======================================================================= */

   /**
    * Default Constructor of CCapiEposStylusEncoderLib
    */
   CCapiEposStylusEncoderLib ( );

   /**
    * Constructor of CCapiEposStylusEncoderLib that creats an instance of encoder lib
    */
    CCapiEposStylusEncoderLib ( ADSPResult    &nRes );

   /**
    * Destructor of CCapiEposStylusEncoderLib
    */
   virtual ~CCapiEposStylusEncoderLib ( );

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
    * Get the value of the Adpcm encoder parameters
    *
    * @param[in]   nParamIdx      Enum value of the parameter of interest
    * @param[out]  pnParamVal      Desired value of the parameter of interest
    *
    * @return  Success/fail
    */
   virtual int CDECL GetParam ( int nParamIdx, int *pParamVal );

   /**
    * Get the value of the Adpcm encoder parameters
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
   int Drop ( const CAPI_BufList_t* pInSamples);
   private:

    int CreateLeftOverBuffers ();
    void DestroyLeftOverBuffers ();


    static const uint32 PACKET_HEADER = 0xFFFFFFFF;
    static const uint32 PACKET_LENGTH_DWORD = 261;
    static const uint32 SAMPLES_PER_FRAME = 512;
    
    static const uint32 FIRST_CHANNEL_MASK = 0xFF;
    static const uint32 FIRST_CHANNEL_SHIFT = 0;
    static const uint32 SECOND_CHANNEL_MASK = 0xFF00;
    static const uint32 SECOND_CHANNEL_SHIFT = 8;
    static const uint32 THIRD_CHANNEL_MASK = 0xFF0000;
    static const uint32 THIRD_CHANNEL_SHIFT = 16;
    static const uint32 FOURTH_CHANNEL_MASK = 0xFF000000;
    static const uint32 FOURTH_CHANNEL_SHIFT = 24;


    struct EposHeaderType
    {
      uint32 packetHeader;
      uint32 packetLengthDWords;
      uint32 packetType;
      uint32 packetNumber;

    public:
      EposHeaderType() : packetHeader(0), packetLengthDWords(0), packetType(0), packetNumber(0)
      {
    	  // default constructor
      }

    } m_eposHeader;

    uint32** m_leftOverBuffers;
    uint32   m_leftOverSize; 
    uint32   m_lframeCounter;
    uint32   m_lFrameSizeBytes; // single channel including header
    uint32   m_lSuperFrameSizeBytes; // super-frame (all channels, including headers)
    uint32   m_lSamplesPerFrame;
    uint32   m_lDataBytesPerFrame;
    uint8   m_channelOrder[MAX_SUPPORTED_CHANNELS];
    uint32   m_lChannelSizeInSamples;
    uint32   m_lConsecutiveDroppedPacktes;
};

#endif /* C_CAPI_EPOS_STYLUS_ENCODER_LIB_H */

