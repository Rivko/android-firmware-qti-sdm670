#ifndef ELITE_CAPI_H
#define ELITE_CAPI_H

/* ========================================================================*/
/**
@file Elite_CAPI.h
Common Audio Processing Interface header file

  This file defines a generalized C/C++ interface that can wrap a wide
  variety of audio processing modules, so that they can be treated the
  same way by control code.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The Elite_Core_mainpage.dox file contains all file/group descriptions that
      are in the output PDF generated using Doxygen and Latex. To edit or update
      any of the file/group text in the PDF, edit the Elite_Core_mainpage.dox
      file or contact Tech Pubs.

      The above description for this file is part of the "capi" group
      description in the Elite_Core_mainpage.dox file.
===========================================================================*/

 /*======================================================================
  Copyright (c) 2008, 2010, 2013, 2014 Qualcomm Technologies Incorporated.
  All rights reserved. Qualcomm Proprietary and Confidential.
 ====================================================================== */

#include "mmdefs.h"
#include "adsp_error_codes.h"

/** @addtogroup capi
@{ */

/** Common errors returned by all decoders.
*/
typedef enum
{
  DEC_SUCCESS=0x0,             /**< Decoding was successful. */
  DEC_NEED_MORE=0x1000,        /**< More input data is required to decode the
                                    frame. */
  DEC_RECOVERY_SUCCESS,        /**< Header parsing failed; next synchronization
                                    word is found in the bitstream.*/
  DEC_RECOVERY_FAILURE,        /**< Tried to recover; no synchronization word
                                    is found in the input buffer. */
  DEC_BADPARAM_FAILURE,        /**< Invalid parameters detected during decoding. */
  DEC_BUFFERTOOSMALL_FAILURE,  /**< Output buffer is too small and cannot hold
                                    a frame. */
  DEC_NOMEMORY_FAILURE,        /**< Insufficient memory for allocation. */
  DEC_NOTIMPL_FAILURE,         /**< Decoder is not implemented; only a stub
                                    exists. */
  DEC_FAILURE,                 /**< All other CAPI non-specific failures. */
  DEC_MAX_ERROR=0x7fffffff     /**< Maximum number of decoder error codes. */
}DecRtrnCodeType;

/** Common errors returned by all encoders.
*/
typedef enum
{
  ENC_SUCCESS=0x0,             /**< Encoding was successful. */
  ENC_NEED_MORE=0x2000,        /**< More input data is required to encode the
                                    frame. */
  ENC_BADPARAM_FAILURE,        /**< Invalid parameters detected during encoding. */
  ENC_BUFFERTOOSMALL_FAILURE,  /**< Output buffer is too small and cannot hold
                                    a frame. */
  ENC_NOMEMORY_FAILURE,        /**< Insufficient memory for allocation. */
  ENC_NOTIMPL_FAILURE,         /**< Encoder is not implemented; only a stub
                                    exists. */
  ENC_FAILURE,                 /**< All other CAPI non-specific failures. */
  ENC_MAX_ERROR=0x7fffffff     /**< Maximum number of encoder error codes */
}EncRtrnCodeType;

/** Common index values for all encoder and decoder paramters
    To be used with CAPI GetParam and SetParam routines
*/
typedef enum
{
   eIcapiNumChannels=0x0,        /**< For decoders this enumerator is used with GetParam call to get the number
                                      of channels in the last decoded output buffer. If this paramter is queried
                                      before decoding any frame it would return 0. For encoders it used with SetParam call
                                      to set the number of channels in the latest raw input buffer*/
   eIcapiSamplingRate,           /**< For decoders this enumerator is used with GetParam call to get the sampling rate
                                      of the last decoded output buffer. For encoders it is used with the SetParam call to
                                      convey the sampling rate of the latest raw input buffer*/
   eIcapiSamplesPerFrame,        /**< This enumerator is used with GetParam call to get the number of samples per
                                      frame per channel*/
   eIcapiInputBufferSize,        /**< Used with GetParam to get the codec internal buffer size required by
                                      codec in bytes. */
   eIcapiOutputBufferSize,       /**< Used with GetParam to get the output buffer size required by
                                      codec in bytes. Only for decoders*/
   eIcapiThreadStackSize,        /**< Used with GetParam to get stack size required by
                                      encoder/decoder svc in bytes*/
   eIcapiWaitForFormatBlock,     /**< This parameter is only applicable for decoders with GetParam call
                                      to find out if a format block is required. If this value is 0, format
                                      block is not required, else format block is required*/
   eIcapiOutputChanMap,          /**< Used for setting and getting the channel map at the output of the CAPI */ 
   eIcapiBitsPerSample,          /**< Used for setting and getting the bits per sample at the output of CAPI */ 
   eIcapiKCPSRequired,           /**< Used for getting the KCPS required to run the core library */ 

   eIcapiMaxMetaDataSize,
   /**< Used with GetParam call to get the maximum size of metadata that the CAPI will return.
      zero indicates the CAPI doesn't support metadata. */

   eIcapiMetaDataPtr,
   /**< Used with GetParam to query the pointer to the metadata.
    * The CAPI should pack metadata as groups of asm_stream_param_data_t (defined in adsp_asm_stream_commands.h)
    * in the memory provided in pnParamVal (second argument to GetParam() ).
    * memory is structured as follows:
    * 1st asm_stream_param_data_t
    * 1st metadata
    * 2nd asm_stream_param_data_t
    * 2nd metadata
    * and so on. */

   eIcapiActualMetaDataSize,
   /**<  Used with the GetParam() call to query the total size of the data
         pointed to by eIcapiMetaDataPtr.
         This value must be nonzero only when the CAPI generates an output
         (metadata does not exist without data). */
   eIcapiMediaInfoPtr,
      /**<  Used with the SetParam() call
       * to support media info blocks with custom CAPIS, for example:
       * media format updates for custom decoders
       * encoder configuration block for custom encoders

      The value of the Param is the pointer to the
      media info block(media fmt/enc cfg),
      type casted to uint32_t*/
   eIcapiBitRate,
   /**<  Used with the Set/GetParam() call to set/query the bit rate  */
   eIcapiInputStreamType,
	/**< used with the setparam() call to set streamtype in mimo decoder case */
	
	eIcapiDecoderDelay,
   /**< Used to get decoder delay from decoder, used only with gapless playback */
   eIcapiTrailingDelay,
   /**< Used to get trailing delay from decoder, used only with gapless playback */
	
   eIcapiMaxParamIndex = 100
   /**< Individual codec parameter indices begin with this value. This is only
        a placeholder and should not be used with GetParam()/SetParam()
        functions. */

}CommonCAPIParams;

/** Channel indices to be used for channel mapping.
    These should be consistent with the definition in
    adsp_media_fmt.h
*/ 

typedef enum 
{
    /*-------standard speaker positions-------------------*/
    eIcapiUnused = 0x0, 
    eIcapiLeft,
    eIcapiRight,
    eIcapiCentre,
    eIcapiLeftSurround,
    eIcapiRightSurround,
    eIcapiLFE,
    eIcapiCenterSurround,
    eIcapiLeftBack,
    eIcapiRightBack,
    eIcapiTopSurround,
    eIcapiCenterVerticalHeight,
    eIcapiMonoSurround,
    eIcapiFrontLeftOfCenter,
    eIcapiFrontRightOfCenter,
    eIcapiRearLeftofCenter,
    eIcapiRearRightofCenter, 
   /*-------codec specific speaker positions---------------*/
    eIcapiCenterBack,
    eIcapiTopCenter,
    eIcapiTopFrontLeft,
    eIcapiTopFrontRight,
    eIcapiTopFrontCenter,
    eIcapiTopBackLeft,
    eIcapiTopBackRight,
    eIcapiTopBackCenter

} SpeakerPosition;

/** @} */ /* end_addtogroup capi */
/** @addtogroup capi
@{ */

/** Forward declaration for the CAPI_Buf_t structure.
*/
typedef struct CAPI_Buf_t CAPI_Buf_t;

/** Forward declaration for the CAPI_BufList_t structure.
*/
typedef struct CAPI_BufList_t CAPI_BufList_t;

/** Forward declaration for the CAPI_ChannelMap_t structure.
*/
typedef struct CAPI_ChannelMap_t CAPI_ChannelMap_t;

/** Forward declaration for the ICAPIVtbl structure.
*/
typedef struct ICAPIVtbl ICAPIVtbl;

/** @brief Structure for a buffer that is passed into CAPI functions.
  */
struct CAPI_Buf_t
{
   char* Data;          /** Data pointer to the raw data. */
   int nActualDataLen;  /** Length of the valid data (in bytes). */
   int nMaxDataLen;     /** Total allocated size of the buffer (in bytes). */
};

/** @brief Structure for the channel positions mapped by the
 *         encoder/decoder.
                                                               */
struct CAPI_ChannelMap_t
{
   unsigned int nChannels;                 /** Number of channels mapped */
   unsigned char nChannelMap[16];          /** channel positions mapped */
};

/**
  @brief Structure for a list of CAPI buffers. It can be used for functions
  that require multiple input or output buffers.
  */
struct CAPI_BufList_t
{
   CAPI_Buf_t* pBuf;    /**< Array of CAPI buffer elements. */
   int nBufs;           /**< Number of elements in the array. */
};

#if (defined __hexagon__) || (defined __qdsp6__)
/**
  @brief Function table structure for plain C implementations of CAPI-compliant
  objects.

  Objects that are written in plain C must have a pointer to a function table as
  the first element in their instance structure. This is the function table type
  for all such objects. No additional inheritance is allowed.
  */
struct ICAPIVtbl
{
   /** Placeholder for a dummy virtual destructor. */
   int VirtualDestructorDummyEntry1;

   /** Placeholder for a dummy virtual destructor. */
   int VirtualDestructorDummyEntry2;

   /** Sets a parameter value or a parameter structure containing multiple
       parameters.

       @param[in] _pif       Pointer to table of virtual functions.
       @param[in] nParamIdx  Identifier of the parameter ID.
       @param[in] nParamVal  Value of the parameter. Depending on the parameter
                             ID, it can be interpreted as a pointer. The caller
                             and callee must have a mutual understanding of
                             whether this is an immediate value or a pointer.

       The CAPI implementation is not required to implement all the CommonCAPIParams.
       @return
       Indication of success or failure.
    */
   int (*SetParam)(ICAPIVtbl* _pif, int nParamIdx, int nParamVal);

   /** Gets a parameter value or a parameter structure containing multiple
       parameters.

       @param[in] _pif       Pointer to table of virtual functions.
       @param[in] nParamIdx  Identifier of the parameter ID.
       @param[in] nParamVal  Value of the parameter. Depending on the parameter
                             ID, it can be interpreted as a pointer. The caller
                             and callee must have a mutual understanding of
                             whether this is an immediate value or a pointer.

       @return
       Indication of success or failure.
    */
   int (*GetParam)(ICAPIVtbl* _pif, int nParamIdx, int *pnParamVal);

   /** Processes an input buffer and generates an output buffer.

       @param[in] _pif       Pointer to table of virtual functions.
       @param[in/out] input  Input buffer. The function is to modify the
                             ActualDataLen field to indicate how many bytes
                             were consumed.
       @param[out] output    Output buffer. The function is to set the
                             ActualDataLen field to indicate how many bytes
                             were generated.
       @param[in,out] param  Optional parameter argument.

       @return
       Indication of success or failure.
    */
   int (*Process)(ICAPIVtbl* _pif, const CAPI_BufList_t* input,
       CAPI_BufList_t* output, CAPI_Buf_t* params);

   /** Initializes the CAPI library. This allocates all memories (if they
       were not already allocated in the constructor) and sets reasonable
       initial values.

       @param[in]     _pif   Pointer to table of virtual functions.
       @param[in,out] param  Optional parameter argument.

       @return
       Indication of success or failure.
    */
   int (*Init)(ICAPIVtbl* _pif, CAPI_Buf_t* params);

   /** Reinitializes the CAPI library. This does not allocate any memory.
       It simply resets the library to the reinitialized state.

       @param[in] _pif       Pointer to table of virtual functions.
       @param[in,out] param  Optional parameter argument.

       @return
       Indication of success or failure.
    */
   int (*ReInit)(ICAPIVtbl* _pif, CAPI_Buf_t* params);

   /** Returns the library to the unitialized state. This frees any memory
       that was allocated in the Init() function.

       @param[in] _pif       Pointer to table of virtual functions.

       @return
       Indication of success or failure.
    */
   int (*End)(ICAPIVtbl* _pif);
};
#else
// Vtables are compiler dependent. Please see how the compiler generates
// vtables for your derived class if you get this error.
#error  COMPILER NOT SUPPORTED ICAPIVtbl (Elite_CAPI.h) is compiler dependent and it needs to be updated/verified for a new compiler.
#endif

/** @} */ /* end_addtogroup capi */
/** @addtogroup capi
@{ */

#if !defined(__cplusplus)

/** Forward declaration for the ICAPI structure. */
typedef struct ICAPI ICAPI;

/** @brief ICAPI plain C interface wrapper for the function table.

  The ICAPI appears to the caller as a function table or an abstract C++ class. In
  C implementations, the function table in the instance structure is followed by
  other structure elements, but they are invisible to the users of the CAPI
  object.

  This ICAPI structure is all that is publicly visible. This allows the caller to
  treat the C object like a C++ object, with the same style of invocations to the
  public methods.
  */
struct ICAPI
{
   ICAPIVtbl *pVtbl;  /**< Virtual function table. */
};

#else /* C++ */

/**
  @brief ICAPI C++ interface wrapper for the function table. For C++ compilation,
  the CAPI interface is simply an abstract class.
  */
class ICAPI
{
public:

/**
  Declares a virtual destructor so the user of CAPI objects can delete ICAPI
  pointers and still reach the derived class destructor.
*/
virtual ~ICAPI() {};

 /**
      Sets either a parameter value or a parameter structure containing multiple
      parameters.

      @param[in] nParamIdx   Identifier of the parameter ID.
      @param[in] nParamVal   Value of the parameter. Depending on the parameter
                             ID, it can be interpreted as a pointer. The caller
                             and callee must have mutual understanding of
                             whether this is an immediate value or a pointer.
      The CAPI implementation is not required to implement all the CommonCAPIParams.
      @return
      Indication of success or failure.
*/
    virtual int SetParam (int nParamIdx, int nParamVal) = 0;

 /**
      Gets a parameter value or a parameter structure containing multiple
      parameters.

      @param[in]  nParamIdx    Identifier of the parameter ID.
      @param[out] pnParamIdx   Value of the parameter.

      @return
      Indication of success or failure.
*/
    virtual int GetParam (int nParamIdx, int *pnParamVal) = 0;

/**
      Processes an input buffer and generate an output buffer.

      @param[in,out] input   Input buffer. The function modifies the
                             ActualDataLen field to indicate how many bytes were
                             consumed.
      @param[out]    output  Output buffer. The function sets the ActualDataLen
field to indicate how many bytes were generated.
      @param[in,out] param   Optional parameter argument.

      @return
      Indication of success or failure.
*/
    virtual int Process (const CAPI_BufList_t* input, CAPI_BufList_t* output, CAPI_Buf_t* params) = 0;

/**
      Initializes the CAPI library. This function allocate all memory if is has
      not been allocated in a constructor, and it sets the initial values.

      @param[in,out] param   Optional parameter argument.

      @return
      Indication of success or failure.
*/
    virtual int Init (CAPI_Buf_t* params) = 0;

/**
      Reinitializes the APP API (APPI) library. This function does not allocate
      memory.

      @param[in,out] param   Optional parameter argument.

      @return
      Indication of success or failure.
*/
    virtual int ReInit (CAPI_Buf_t* params) = 0;

/**
      Returns the library to an uninitialized state. This function frees any
      memory allocated during initialization.

      @param[in,out] param   Optional parameter argument.

      @return
      Indication of success or failure.
*/
    virtual int End (void) = 0;
};
#endif /* !defined(__cplusplus) */

/** @} */ /* end_addtogroup capi */

#endif /* #ifndef CAPI_H */
