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
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      Elite_CAPI_mainpage.dox file. Contact Tech Pubs for assistance.
===========================================================================*/

 /*======================================================================
  Copyright (c) 2008, 2010, 2014 Qualcomm Technologies, Inc.
  All rights reserved.
  Proprietary and Confidential - Qualcomm Technologies, Inc.
 ====================================================================== */


/* =========================================================================
                             Edit History

   when       who     what, where, why
   --------   ---     ------------------------------------------------------
   10/17/14   sw      (Tech Pubs) Merged Doxgen comments from 2.4; edited
                      Doxygen comments for 2.6.
   10/28/10   sw      (Tech Pubs) Edited/added Doxygen comments and markup.
   05/28/08   mwc     Created file.
   ========================================================================= */


#include "qurt_elite_types.h"

/** @addtogroup error_codes
@{ */
/** @xreflabel{hdr:DecRtrnCodeTypeEnum}
    Common status/error codes returned by all decoders.
*/
typedef enum
{
  DEC_SUCCESS=0x0,
  /**< Decoding was successful. */

  DEC_NEED_MORE=0x1000,
  /**< More input data is required to decode the frame.
       @xreflabel{hdr:DecNeedMoreError} */

  DEC_RECOVERY_SUCCESS,
  /**< Header parsing failed; the next synchronization word is found in the
       bitstream.*/

  DEC_RECOVERY_FAILURE,
  /**< Tried to recover; no synchronization word is found in the input buffer. */

  DEC_BADPARAM_FAILURE,
  /**< Invalid parameters detected during decoding. */

  DEC_BUFFERTOOSMALL_FAILURE,
  /**< Output buffer is too small and cannot hold a frame.
       @xreflabel{hdr:DecBufSmallError} */

  DEC_NOMEMORY_FAILURE,
  /**< Insufficient memory for allocation. */

  DEC_NOTIMPL_FAILURE,
  /**< Decoder is not implemented; only a stub exists. */

  DEC_FAILURE,
  /**< All other CAPI non-specific failures.

       The complete buffer is discarded. No output is produced in this call. */

  DEC_MAX_ERROR=0x7fffffff
  /**< Maximum number of decoder error codes. */

}DecRtrnCodeType;

/** @xreflabel{hdr:EncRtrnCodeTypeEnum}
    Common status/error codes returned by all encoders.
*/
typedef enum
{
  ENC_SUCCESS=0x0,
  /**< Encoding was successful. */

  ENC_NEED_MORE=0x2000,
  /**< More input data is required to encode the frame.
       @xreflabel{hdr:EncNeedMoreError} */

  ENC_BADPARAM_FAILURE,
  /**< Invalid parameters detected during encoding. */

  ENC_BUFFERTOOSMALL_FAILURE,
  /**< Output buffer is too small and cannot hold a frame.
       @xreflabel{hdr:EncBufSmallError} */

  ENC_NOMEMORY_FAILURE,
  /**< Insufficient memory for allocation. */

  ENC_NOTIMPL_FAILURE,
  /**< Encoder is not implemented; only a stub exists. */

  ENC_FAILURE,
  /**< All other CAPI non-specific failures.

       The complete buffer is discarded. No output is produced in this call. */

  ENC_MAX_ERROR=0x7fffffff
  /**< Maximum number of encoder error codes. */

}EncRtrnCodeType;

/** @} */ /* end_addtogroup error_codes */

/** @addtogroup index_values
@{ */
/** @xreflabel{hdr:CommonCapiParamsEnum}
    Common index values for all encoder and decoder parameters. These values
    are to be used with the ICAPIVtbl::SetParam() and ICAPIVtbl::GetParam()
    functions.
*/
typedef enum
{
   eIcapiNumChannels=0x0,
   /**< For decoders, used with a GetParam() call to get the number of channels
        in the last decoded output buffer.

        If this parameter is queried before decoding any frame, it returns 0.
        For encoders, it used with a SetParam() call to set the number of
        channels in the latest raw input buffer. */

   eIcapiSamplingRate,
   /**< For decoders, used with a GetParam() call to get the sampling rate (in
        Hertz) of the last decoded output buffer.

        For encoders, it is used with a SetParam() call to convey the sampling
        rate of the latest raw input buffer. */

   eIcapiSamplesPerFrame,
   /**< Used with a GetParam() call to get the number of samples per frame per
        channel. */

   eIcapiInputBufferSize,
   /**< Used with a GetParam() call to get the codec input buffer size (in
        bytes) required by the codec. */

   eIcapiOutputBufferSize,
   /**< Used with a GetParam() call to get the output buffer size (in bytes)
        required by the codec. */

   eIcapiThreadStackSize,
   /**< Used with a GetParam() call to get the stack size (in bytes) required by
        the encoder/decoder service. */

   eIcapiWaitForFormatBlock,
   /**< For decoders only; used with a GetParam() call to deterimine if a
        format block is required. Zero indicates that a format block is not
        required; otherwise, a format block is required. */

   eIcapiOutputChanMap,
   /**< Sets/gets the channel mapping at the decoder output.
   
        The corresponding parameter value is a pointer to a #CAPI_ChannelMap_t
        type. This type contains number of channels and an array of speaker
        positions. */

   eIcapiBitsPerSample,
   /**< Sets/gets the bits per sample at the decoder output and encoder input. */

   eIcapiKCPSRequired,
   /**< Gets the kilocycles per second required to run the core library. */ 

   eIcapiMaxMetaDataSize,
   /**< Used with the GetParam() function to get the maximum size of metadata
        that CAPI returns. Zero indicates that CAPI does not support
        metadata. */

   eIcapiMetaDataPtr,
   /**< Used with the GetParam() call to query the pointer to the metadata.

        CAPI packs metadata as groups of asm_stream_param_data_t
		(defined in @xhyperref{Q2,[Q2]}) 
		in the memory provided in pnParamVal (second argument to GetParam()).
		Memory is structured as follows: \n
        @txtbullet First asm_stream_param_data_t \n
        @txtbullet First metadata \n
        @txtbullet Second asm_stream_param_data_t \n
        @txtbullet Second metadata \n
        @txtbullet Etc. */

	/* asm_stream_param_data_t is defined in adsp_asm_stream_commands.h */

   eIcapiActualMetaDataSize,
   /**<  Used with the GetParam() call to query the total size of the data
         pointed to by eIcapiMetaDataPtr.

        This value must be nonzero only when CAPI generates output
        (metadata does not exist without data). @newpage */

   eIcapiMediaInfoPtr,
   /**< Used with the SetParam() call to support media information blocks with
        custom CAPIs.

        For example: \n
        @txtbullet Media format updates for custom decoders \n
        @txtbullet Encoder configuration block for custom encoders

        The value of the parameter is a pointer to the media information block
        (media fmt/enc cfg), type casted to uint32_t. */

   eIcapiBitRate,
   /**< Used with the Set/GetParam() call to set/query the bitrate. */

   eIcapiInputStreamType,
	/**< Used with the SetParam() call to set the stream type for multiple
         input/multiple output decoders. */
	
	eIcapiDecoderDelay,
   /**< Gets the decoder delay from the decoder.

        This delay is used only with gapless playback. */

   eIcapiTrailingDelay,
   /**< Gets the trailing delay from the decoder.

        This delay is used only with gapless playback. */
	
   eIcapiMaxParamIndex = 100
   /**< Individual codec parameter indices begin with this value. This is only
        a placeholder and must not be used with GetParam() and SetParam()
        functions. */

}CommonCAPIParams;

/* These speaker positions should be consistent with the definition in adsp_media_fmt.h */

/** @cond */
/** Channel indices (standard and codec-specific) to be used for channel
    mapping. For detailed information on the speaker positions, refer to
    @xhyperref{Q2,[Q2]}.
*/ 
typedef enum 
{
    /*-------standard speaker positions-------------------*/
    eIcapiUnused = 0x0,   /**< Standard -- Maintained for backward
                               compatibility. Currently, no devices support
                               this type. */
    eIcapiLeft,           /**< Standard -- Front left channel. */
    eIcapiRight,          /**< Standard -- Front rightt channel. */
    eIcapiCentre,         /**< Standard -- Front center channel. */
    eIcapiLeftSurround,   /**< Standard -- Left surround channel. */
    eIcapiRightSurround,  /**< Standard -- Right surround channel. */
    eIcapiLFE,            /**< Standard -- Low frequency effect channel. */
    eIcapiCenterSurround, /**< Standard -- Center surround channel; rear
                               center channel. */
    eIcapiLeftBack,       /**< Standard -- Left back channel; rear left
                               channel. */
    eIcapiRightBack,      /**< Standard -- Right back channel; rear right
                               channel. */
    eIcapiTopSurround,    /**< Standard -- Top surround channel. */
    eIcapiCenterVerticalHeight, /**< Standard -- Center vertical height
                                      channel. */
    eIcapiMonoSurround,       /**< Standard -- Mono surround channel. */
    eIcapiFrontLeftOfCenter,  /**< Standard -- Front left of center channel. */
    eIcapiFrontRightOfCenter, /**< Standard -- Front right of center channel. */
    eIcapiRearLeftofCenter,   /**< Standard -- Rear left of center channel. */
    eIcapiRearRightofCenter,  /**< Standard -- Rear right of center channel. */

   /*-------codec specific speaker positions---------------*/
    eIcapiCenterBack,       /**< Codec-specific -- Center back channel. */
    eIcapiTopCenter,        /**< Codec-specific -- Top center channel. */
    eIcapiTopFrontLeft,     /**< Codec-specific -- Top front left channel. */
    eIcapiTopFrontRight,    /**< Codec-specific -- Top front right channel. */
    eIcapiTopFrontCenter,   /**< Codec-specific -- Top front center channel. */
    eIcapiTopBackLeft,      /**< Codec-specific -- Top back left channel. */
    eIcapiTopBackRight,     /**< Codec-specific -- Top back right channel. */
    eIcapiTopBackCenter     /**< Codec-specific -- Top back center channel. */

} SpeakerPosition;
/** @endcond */

/** @} */ /* end_addtogroup index_values */

/* Forward declaration for the CAPI_Buf_t structure. */
typedef struct CAPI_Buf_t CAPI_Buf_t;

/* Forward declaration for the CAPI_BufList_t structure. */
typedef struct CAPI_BufList_t CAPI_BufList_t;

/* Forward declaration for the ICAPIVtbl structure. */
typedef struct ICAPIVtbl ICAPIVtbl;

/* Forward declaration for the CAPI_ChannelMap_t structure.*/
typedef struct CAPI_ChannelMap_t CAPI_ChannelMap_t;

/** @addtogroup data_types
@{ */
/** @xreflabel{hdr:CapiBufTStruct}
    Input buffers, output buffers, and parameters that are passed into
    the CAPI functions.
  */
struct CAPI_Buf_t
{
   char* Data;          /**< Data pointer to the raw data. */
   int nActualDataLen;  /**< Length of the valid data (in bytes). */
   int nMaxDataLen;     /**< Total allocated size of the buffer (in bytes). */
};

/** Channel positions mapped by the encoder/decoder.
                                                               */
struct CAPI_ChannelMap_t
{
   unsigned int nChannels;        /**< Number of channels mapped. */
   unsigned char nChannelMap[16]; /**< Channel positions mapped. This is an
                                       array of 16 elements: nChannelMap[16]. */
};

/** List of CAPI buffers (CAPI_Buf_t). This can be used
    for functions that require multiple input or output buffers.
  */
struct CAPI_BufList_t
{
   CAPI_Buf_t* pBuf;    /**< Pointer to an array of CAPI buffer elements. */
   int nBufs;           /**< Number of buffer elements in the array. */
};
/** @} */ /* end_addtogroup data_types */
#if (defined __hexagon__) || (defined __qdsp6__)


/** @ingroup capi_virt_func_table
    Function table structure for plain C implementations of
    CAPI-compliant objects.

    Objects that are written in plain C must have a pointer to a function table
    as the first element in their instance structure. This is the function
    table type for all such objects. No additional inheritance is allowed.
  */
struct ICAPIVtbl
{
   /** Placeholder for a dummy virtual destructor. */
   int VirtualDestructorDummyEntry1;

   /** Placeholder for a dummy virtual destructor. */
   int VirtualDestructorDummyEntry2;

   /** @ingroup capi_virt_func_set_param
       Sets a parameter value or a parameter structure containing multiple
       parameters.

       @param[in] _pif       Pointer to the table of virtual functions.
       @param[in] nParamIdx  Identifier of the parameter ID.
       @param[in] nParamVal  Value of the parameter. Depending on the parameter
                             ID, it can be interpreted as a pointer. The caller
                             and callee must have a mutual understanding of
                             whether this is an immediate value or a pointer.

       @detdesc
       This function sets the value of the parameter whose index is passed as
       in nParamIdx and whose value is provided in nParamVal. The function
       returns a success or failure based on its validity with respect to the
       underlying module.
       @par
       For nParamIdx, choose one of the common enumerated indices defined in
       #CommonCAPIParams.
       @par
       If a new index is to be declared, it must be present in the
       corresponding CAPI declaration.
       @par
       The CAPI implementation is not required to implement all of the
       CommonCAPIParams.

       @return
       Status/error code for decoders:
       - #DEC_SUCCESS
       - #DEC_NOTIMPL_FAILURE
       - #DEC_BADPARAM_FAILURE
       - #DEC_FAILURE
       @par
       Status/error code for encoders:
       - #ENC_SUCCESS
       - #ENC_NOTIMPL_FAILURE
       - #ENC_BADPARAM_FAILURE
       - #ENC_FAILURE

       @dependencies
       None.
    */
   int (*SetParam)(ICAPIVtbl* _pif, int nParamIdx, int nParamVal);

   /** @ingroup capi_virt_func_get_param
       Gets a parameter value or a parameter structure containing multiple
       parameters.

       @param[in] _pif       Pointer to the table of virtual functions.
       @param[in] nParamIdx  Identifier of the parameter ID.
       @param[in] nParamVal  Value of the parameter. Depending on the parameter
                             ID, it can be interpreted as a pointer. The caller
                             and callee must have a mutual understanding of
                             whether this is an immediate value or a pointer.

       @detdesc
       This function returns the value of the parameter in nParamVal whose
       index is passed as an argument in nParamIdx. The function returns a
       success or failure if the argument is not supported.
       @par
       For nParamIdx, choose one of the common enumerated indices defined in
       #CommonCAPIParams. If a new index is to be declared, it must be
       present in the corresponding CAPI declaration.
       @par
       All decoder CAPIs are required to provide an interface for getting the
       flag that indicates if a format block is required. The corresponding
       parameter index for this interface is #eIcapiWaitForFormatBlock.
       @par
       All encoder and decoder CAPIs are required to provide an interface for
       the following mandatory parameters:
       - #eIcapiSamplingRate
       - #eIcapiOutputChanMap
       - #eIcapiInputBufferSize
       - #eIcapiOutputBufferSize
       - #eIcapiThreadStackSize

       @return
       Status/error code for decoders:
       - #DEC_SUCCESS    
       - #DEC_NOTIMPL_FAILURE    
       - #DEC_BADPARAM_FAILURE    
       - #DEC_FAILURE
       @par
       @newpage Status/error code for encoders:
       - #ENC_SUCCESS    
       - #ENC_NOTIMPL_FAILURE    
       - #ENC_BADPARAM_FAILURE    
       - #ENC_FAILURE

       @dependencies
       None.
    */
   int (*GetParam)(ICAPIVtbl* _pif, int nParamIdx, int *pnParamVal);

   /** @ingroup capi_virt_func_process
       Processes an input buffer and generates an output buffer.

       @datatypes
       CAPI_BufList_t \n
       CAPI_Buf_t

       @param[in]     _pif    Pointer to the table of virtual functions.
       @param[in,out] input   Pointer to a list of input buffer descriptors.
                              The function modifies the ActualDataLen
                              field to indicate how many bytes were consumed.
       @param[out]    output  Pointer to a list of output buffer descriptors.
                              The function sets the ActualDataLen field
                              to indicate how many bytes were generated.
       @param[in,out] params  Optional pointer to a parameter buffer.
                              Parameters can be used as input or modified by
                              the library.

       @detdesc
       This function can used to either encode a block of PCM samples or
       decode a frame of compressed audio bitstream. It calls the core 
       processing logic of the audio library.
       @par
       This function requires a list of input buffers and a list of
       output buffers along with an optional set of parameters. In the
       simplest case, it is passed a single input buffer and a single output
       buffer.
       - For decoders, the input buffer must contain enough bitstream data to
         decode a single frame. The output buffer must be large enough to
         hold one frame of decoded PCM data.
       - For encoders, the input buffer must contain enough PCM samples to
         encode one frame. The output buffer must be large enough to
         accommodate at least one frame of encoded data.
       @par
       For more information on usage of the input and output buffer data
       types, see Section @xref{sec:EncDecProcessCallUsage}.

       @return
       Status/error code for decoders:
       - #DEC_SUCCESS    
       - #DEC_NEED_MORE    
       - #DEC_RECOVERY_SUCCESS    
       - #DEC_RECOVERY_FAILURE    
       - #DEC_BADPARAM_FAILURE    
       - #DEC_BUFFERTOOSMALL_FAILURE    
       - #DEC_NOTIMPL_FAILURE    
       - #DEC_FAILURE
       @par
       Status/error code for encoders:
       - #ENC_SUCCESS    
       - #ENC_NEED_MORE    
       - #ENC_BADPARAM_FAILURE    
       - #ENC_BUFFERTOOSMALL_FAILURE    
       - #ENC_NOTIMPL_FAILURE    
       - #ENC_FAILURE

       @dependencies
       None.
    */
   int (*Process)(ICAPIVtbl* _pif, const CAPI_BufList_t* input,
       CAPI_BufList_t* output, CAPI_Buf_t* params);

   /** @ingroup capi_virt_func_init
       Initializes the CAPI library.

       @datatypes
       CAPI_Buf_t

       @param[in]     _pif    Pointer to the table of virtual functions.
       @param[in,out] params  Optional pointer to a parameter buffer.
                              Parameters can be used as input or modified by
                              the library.

       @detdesc
       This function allocates all memories (if they were not already allocated
       in the constructor) and sets reasonable initial values.
       @par
       This function must be called only once per session.
       @par
       For C CAPIs, memory allocations required by the CAPI module are to be
       performed in %Init(). For C++ CAPIs, memory allocations are to be
       performed in the constructor.

       @return
       Status/error code for decoders:
       - #DEC_SUCCESS    
       - #DEC_BADPARAM_FAILURE    
       - #DEC_NOMEMORY_FAILURE    
       - #DEC_NOTIMPL_FAILURE    
       - #DEC_FAILURE
       @par
       Status/error code for encoders:
       - #ENC_SUCCESS    
       - #ENC_BADPARAM_FAILURE    
       - #ENC_NOMEMORY_FAILURE    
       - #ENC_NOTIMPL_FAILURE    
       - #ENC_FAILURE

       @dependencies
       %Init() must always be called before Process() is called.
    */
   int (*Init)(ICAPIVtbl* _pif, CAPI_Buf_t* params);

   /** @ingroup capi_virt_func_reinit
       Reinitializes the CAPI library.

       @datatypes
       CAPI_Buf_t

       @param[in]     _pif     Pointer to the table of virtual functions.
       @param[in,out] params   Pointer to a parameter of tData type. The byte
                               pointer points to a list of parameters that
                               will either be modified or read by the library
                               or application.

       @detdesc
       This function does not allocate any memory. It simply resets the library
       to the reinitialized state.
       @par
       This function can be called after a Flush is issued by the client. It
       can also be used after certain CAPI parameters are modified through a
       SetParam() call and certain library reinitializations need to be
       performed with the new value of the parameter. In such cases, the
       parameters that must be retained throughout the playback/record 
       operation must not be reset.
       @par
       This function does not allocate any memory. It simply resets the
       library to the reinitialized state.

       @return
       Status/error code for decoders:
       - #DEC_SUCCESS    
       - #DEC_BADPARAM_FAILURE    
       - #DEC_NOTIMPL_FAILURE    
       - #DEC_FAILURE
       @par
       Status/error code for encoders:
       - #ENC_SUCCESS    
       - #ENC_BADPARAM_FAILURE    
       - #ENC_NOTIMPL_FAILURE    
       - #ENC_FAILURE

       @dependencies
       None.
    */
   int (*ReInit)(ICAPIVtbl* _pif, CAPI_Buf_t* params);

   /** @ingroup capi_virt_func_end
       Returns the library to the uninitialized state.

       @param[in] _pif    Pointer to the table of virtual functions.

       @return
       Status/error code for decoders:
       - #DEC_SUCCESS    
       - #DEC_NOTIMPL_FAILURE    
       - #DEC_FAILURE
       @par
       Status/error code for encoders:
       - #ENC_SUCCESS    
       - #ENC_NOTIMPL_FAILURE    
       - #ENC_FAILURE

       @dependencies
       The caller must ensure that this function is called only if there was
       a successful call to Init().
    */
   int (*End)(ICAPIVtbl* _pif);
};

#elif defined (_MSC_VER)

/** @ingroup function_tables
    Function table structure for plain C implementations of
    CAPI-compliant objects.

    Objects that are written in plain C must have a pointer to a function
    table as the first element in their instance structure. This is the
    function table type for all such objects. No additional inheritance is
    allowed.
*/
struct ICAPIVtbl
{
   /** Placeholder for a dummy virtual destructor. */
   int VirtualDestructorDummyEntry1;

   /** @ingroup capi_virt_func_set_param
       Sets a parameter value or a parameter structure containing multiple
       parameters.

       @param[in] _pif       Pointer to the table of virtual functions.
       @param[in] nParamIdx  Identifier of the parameter ID.
       @param[in] nParamVal  Value of the parameter. Depending on the parameter
                             ID, it can be interpreted as a pointer. The caller
                             and callee must have a mutual understanding of
                             whether this is an immediate value or a pointer.

       @detdesc
       This function sets the value of the parameter whose index is passed as
       in nParamIdx and whose value is provided in nParamVal. The function
       returns a success or failure based on its validity with respect to the
       underlying module.
       @par
       For nParamIdx, choose one of the common enumerated indices defined in
       #CommonCAPIParams.
       @par
       If a new index is to be declared, it must be present in the
       corresponding CAPI declaration.
       @par
       The CAPI implementation is not required to implement all of the
       CommonCAPIParams.

       @return
       Status/error code for decoders:
       - #DEC_SUCCESS
       - #DEC_NOTIMPL_FAILURE
       - #DEC_BADPARAM_FAILURE
       - #DEC_FAILURE
       @par
       Status/error code for encoders:
       - #ENC_SUCCESS
       - #ENC_NOTIMPL_FAILURE
       - #ENC_BADPARAM_FAILURE
       - #ENC_FAILURE

       @dependencies
       None.
    */
   int (*SetParam)(ICAPIVtbl* _pif, int nParamIdx, int nParamVal);

   /** @ingroup capi_virt_func_get_param
       Gets a parameter value or a parameter structure containing multiple
       parameters.

       @param[in] _pif       Pointer to the table of virtual functions.
       @param[in] nParamIdx  Identifier of the parameter ID.
       @param[in] nParamVal  Value of the parameter. Depending on the parameter
                             ID, it can be interpreted as a pointer. The caller
                             and callee must have a mutual understanding of
                             whether this is an immediate value or a pointer.

       @detdesc
       This function returns the value of the parameter in nParamVal whose
       index is passed as an argument in nParamIdx. The function returns a
       success or failure if the argument is not supported.
       @par
       For nParamIdx, choose one of the common enumerated indices defined in
       #CommonCAPIParams. If a new index is to be declared, it must be
       present in the corresponding CAPI declaration.
       @par
       All decoder CAPIs are required to provide an interface for getting the
       flag that indicates if a format block is required. The corresponding
       parameter index for this interface is #eIcapiWaitForFormatBlock.
       @par
       All encoder and decoder CAPIs are required to provide an interface for
       the following mandatory parameters:
       - #eIcapiSamplingRate
       - #eIcapiOutputChanMap
       - #eIcapiInputBufferSize
       - #eIcapiOutputBufferSize
       - #eIcapiThreadStackSize

       @return
       Status/error code for decoders:
       - #DEC_SUCCESS    
       - #DEC_NOTIMPL_FAILURE    
       - #DEC_BADPARAM_FAILURE    
       - #DEC_FAILURE
       @par
       @newpage Status/error code for encoders:
       - #ENC_SUCCESS    
       - #ENC_NOTIMPL_FAILURE    
       - #ENC_BADPARAM_FAILURE    
       - #ENC_FAILURE

       @dependencies
       None.
    */
   int (*GetParam)(ICAPIVtbl* _pif, int nParamIdx, int *pnParamVal);

   /** @ingroup capi_virt_func_process
       Processes an input buffer and generates an output buffer.

       @datatypes
       CAPI_BufList_t \n
       CAPI_Buf_t

       @param[in]     _pif    Pointer to the table of virtual functions.
       @param[in,out] input   Pointer to a list of input buffer descriptors.
                              The function modifies the ActualDataLen
                              field to indicate how many bytes were consumed.
       @param[out]    output  Pointer to a list of output buffer descriptors.
                              The function sets the ActualDataLen field
                              to indicate how many bytes were generated.
       @param[in,out] params  Optional pointer to a parameter buffer.
                              Parameters can be used as input or modified by
                              the library.

       @detdesc
       This function can used to either encode a block of PCM samples or
       decode a frame of compressed audio bitstream. It calls the core 
       processing logic of the audio library.
       @par
       This function requires a list of input buffers and a list of
       output buffers along with an optional set of parameters. In the
       simplest case, it is passed a single input buffer and a single output
       buffer.
       - For decoders, the input buffer must contain enough bitstream data to
         decode a single frame. The output buffer must be large enough to
         hold one frame of decoded PCM data.
       - For encoders, the input buffer must contain enough PCM samples to
         encode one frame. The output buffer must be large enough to
         accommodate at least one frame of encoded data.
       @par
       For more information on usage of the input and output buffer data
       types, see Section @xref{sec:EncDecProcessCallUsage}.

       @return
       Status/error code for decoders:
       - #DEC_SUCCESS    
       - #DEC_NEED_MORE    
       - #DEC_RECOVERY_SUCCESS    
       - #DEC_RECOVERY_FAILURE    
       - #DEC_BADPARAM_FAILURE    
       - #DEC_BUFFERTOOSMALL_FAILURE    
       - #DEC_NOTIMPL_FAILURE    
       - #DEC_FAILURE
       @par
       Status/error code for encoders:
       - #ENC_SUCCESS    
       - #ENC_NEED_MORE    
       - #ENC_BADPARAM_FAILURE    
       - #ENC_BUFFERTOOSMALL_FAILURE    
       - #ENC_NOTIMPL_FAILURE    
       - #ENC_FAILURE

       @dependencies
       None.
    */
   int (*Process)(ICAPIVtbl* _pif, const CAPI_BufList_t* input,
       CAPI_BufList_t* output, CAPI_Buf_t* params);

   /** @ingroup capi_virt_func_init
       Initializes the CAPI library.

       @datatypes
       CAPI_Buf_t

       @param[in]     _pif    Pointer to the table of virtual functions.
       @param[in,out] params  Optional pointer to a parameter buffer.
                              Parameters can be used as input or modified by
                              the library.

       @detdesc
       This function allocates all memories (if they were not already allocated
       in the constructor) and sets reasonable initial values.
       @par
       This function must be called only once per session.
       @par
       For C CAPIs, memory allocations required by the CAPI module are to be
       performed in %Init(). For C++ CAPIs, memory allocations are to be
       performed in the constructor.

       @return
       Status/error code for decoders:
       - #DEC_SUCCESS    
       - #DEC_BADPARAM_FAILURE    
       - #DEC_NOMEMORY_FAILURE    
       - #DEC_NOTIMPL_FAILURE    
       - #DEC_FAILURE
       @par
       Status/error code for encoders:
       - #ENC_SUCCESS    
       - #ENC_BADPARAM_FAILURE    
       - #ENC_NOMEMORY_FAILURE    
       - #ENC_NOTIMPL_FAILURE    
       - #ENC_FAILURE

       @dependencies
       %Init() must always be called before Process() is called.
    */
   int (*Init)(ICAPIVtbl* _pif, CAPI_Buf_t* params);

   /** @ingroup capi_virt_func_reinit
       Reinitializes the CAPI library.

       @datatypes
       CAPI_Buf_t

       @param[in]     _pif     Pointer to the table of virtual functions.
       @param[in,out] params   Pointer to a parameter of tData type. The byte
                               pointer points to a list of parameters that
                               will either be modified or read by the library
                               or application.

       @detdesc
       This function does not allocate any memory. It simply resets the library
       to the reinitialized state.
       @par
       This function can be called after a Flush is issued by the client. It
       can also be used after certain CAPI parameters are modified through a
       SetParam() call and certain library reinitializations need to be
       performed with the new value of the parameter. In such cases, the
       parameters that must be retained throughout the playback/record 
       operation must not be reset.
       @par
       This function does not allocate any memory. It simply resets the
       library to the reinitialized state.

       @return
       Status/error code for decoders:
       - #DEC_SUCCESS    
       - #DEC_BADPARAM_FAILURE    
       - #DEC_NOTIMPL_FAILURE    
       - #DEC_FAILURE
       @par
       Status/error code for encoders:
       - #ENC_SUCCESS    
       - #ENC_BADPARAM_FAILURE    
       - #ENC_NOTIMPL_FAILURE    
       - #ENC_FAILURE

       @dependencies
       None.
    */
   int (*ReInit)(ICAPIVtbl* _pif, CAPI_Buf_t* params);

   /** @ingroup capi_virt_func_end
       Returns the library to the uninitialized state.

       @datatypes
       ICAPIVtbl

       @param[in] _pif    Pointer to the table of virtual functions.

       @return
       Status/error code for decoders:
       - #DEC_SUCCESS    
       - #DEC_NOTIMPL_FAILURE    
       - #DEC_FAILURE
       @par
       Status/error code for encoders:
       - #ENC_SUCCESS    
       - #ENC_NOTIMPL_FAILURE    
       - #ENC_FAILURE

       @dependencies
       The caller must ensure that this function is called only if there was
       a successful call to Init().
    */
   int (*End)(ICAPIVtbl* _pif);
};

#else
// Vtables are compiler dependent. Please see how the compiler generates
// vtables for your derived class if you get this error.
#error  COMPILER NOT SUPPORTED ICAPIVtbl (Elite_CAPI.h) is compiler dependent and it needs to be updated/verified for a new compiler.
#endif

#if !defined(__cplusplus)

/* Forward declaration for the ICAPI structure. */
typedef struct ICAPI ICAPI;

/** @ingroup function_wrappers
    @brief ICAPI plain C interface wrapper for the function table.

    The ICAPI appears to the caller as a function table or an abstract C++
    class. In C implementations, the function table in the instance structure
    is followed by other structure elements, but they are invisible to the
    users of the CAPI object.

    This ICAPI structure is all that is publicly visible. This allows the
    caller to treat the C object like a C++ object, with the same style of
    invocations to the public methods.
  */
struct ICAPI
{
   ICAPIVtbl *pVtbl;  /**< Virtual function table. */
};

#else /* C++ */

/** @ingroup function_wrappers
    ICAPI C++ interface wrapper for the function table. For C++
    compilation, CAPI is simply an abstract class.
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
        Sets a parameter value or a parameter structure containing multiple
      parameters.

      @param[in] nParamIdx   Identifier of the parameter ID.
        @param[in] nParamVal  Value of the parameter. Depending on the
                              parameter ID, it can be interpreted as a pointer.
                              The caller and callee must have a mutual
                              understanding of whether this is an immediate
                              value or a pointer.

        @detdesc
        This function sets the value of the parameter whose index is passed as
        in nParamIdx and whose value is provided in nParamVal. The function
        returns a success or failure based on its validity with respect to the
        underlying module.
        @par
        For nParamIdx, choose one of the common enumerated indices defined in
        #CommonCAPIParams.
        @par
        If a new index is to be declared, it must be present in the corresponding
        CAPI declaration.
        @par
        The CAPI implementation is not required to implement all of the
        CommonCAPIParams.

      @return
        Status/error code for decoders:
        - #DEC_SUCCESS
        - #DEC_NOTIMPL_FAILURE
        - #DEC_BADPARAM_FAILURE
        - #DEC_FAILURE
        @par
        Status/error code for encoders:
        - #ENC_SUCCESS
        - #ENC_NOTIMPL_FAILURE
        - #ENC_BADPARAM_FAILURE
        - #ENC_FAILURE

        @dependencies
        None.
*/
    virtual int CDECL SetParam (int nParamIdx, int nParamVal) = 0;

 /**
      Gets a parameter value or a parameter structure containing multiple
      parameters.

      @param[in]  nParamIdx    Identifier of the parameter ID.
        @param[in] nParamVal  Value of the parameter. Depending on the
                              parameter ID, it can be interpreted as a pointer.
                              The caller and callee must have a mutual
                              understanding of whether this is an immediate value
                              or a pointer.

        @detdesc
        This function returns the value of the parameter in nParamVal whose
        index is passed as an argument in nParamIdx. The function returns a
        success or failure if the argument is not supported.
        @par
        For nParamIdx, choose one of the common enumerated indices defined in
        #CommonCAPIParams. If a new index is to be declared, it must be
        present in the corresponding CAPI declaration.
        @par
        All decoder CAPIs are required to provide an interface for getting the
        flag that indicates if a format block is required. The corresponding
        parameter index for this interface is #eIcapiWaitForFormatBlock.
        @par
        All encoder and decoder CAPIs are required to provide an interface for
        the following mandatory parameters:
        - #eIcapiSamplingRate
        - #eIcapiOutputChanMap
        - #eIcapiInputBufferSize
        - #eIcapiOutputBufferSize
        - #eIcapiThreadStackSize

      @return
        Status/error code for decoders:
        - #DEC_SUCCESS    
        - #DEC_NOTIMPL_FAILURE    
        - #DEC_BADPARAM_FAILURE    
        - #DEC_FAILURE
        @par
        Status/error code for encoders:
        - #ENC_SUCCESS    
        - #ENC_NOTIMPL_FAILURE    
        - #ENC_BADPARAM_FAILURE    
        - #ENC_FAILURE

        @dependencies
        None.
*/
    virtual int CDECL GetParam (int nParamIdx, int *pnParamVal) = 0;

/**
        Processes an input buffer and generates an output buffer.

        @datatypes
        CAPI_BufList_t \n
        CAPI_Buf_t

        @param[in,out] input  Pointer to a list of input buffer descriptors.
                              The function modifies the ActualDataLen field to
                              field to indicate how many bytes were consumed.
        @param[out]    output Pointer to a list of output buffer descriptors.
                              The function sets the ActualDataLen field to
                              indicate how many bytes were generated.
        @param[in,out] params Optional pointer to a parameter buffer.
                              Parameters can be used as input or modified by
                              the library.

        @detdesc
        This function can used to either encode a block of PCM samples or
        decode a frame of compressed audio bitstream. It calls the core 
        processing logic of the audio library.
        @par
        This function requires a list of input buffers and a list of
        output buffers along with an optional set of parameters. In the simplest
        case, it is passed a single input buffer and a single output buffer.
        - For decoders, the input buffer must contain enough bitstream data to
          decode a single frame. The output buffer must be large enough to hold
          hold one frame of decoded PCM data.
        - For encoders, the input buffer must contain enough PCM samples to
          encode one frame. The output buffer must be large enough to
          accommodate at least one frame of encoded data.
        @par
        For more information on usage of the input and output buffer data
        types, see Section @xref{sec:EncDecProcessCallUsage}.

      @return
        Status/error code for decoders:
        - #DEC_SUCCESS    
        - #DEC_NEED_MORE    
        - #DEC_RECOVERY_SUCCESS    
        - #DEC_RECOVERY_FAILURE    
        - #DEC_BADPARAM_FAILURE    
        - #DEC_BUFFERTOOSMALL_FAILURE    
        - #DEC_NOTIMPL_FAILURE    
        - #DEC_FAILURE
        @par
        Status/error code for encoders:
        - #ENC_SUCCESS    
        - #ENC_NEED_MORE    
        - #ENC_BADPARAM_FAILURE    
        - #ENC_BUFFERTOOSMALL_FAILURE    
        - #ENC_NOTIMPL_FAILURE    
        - #ENC_FAILURE

        @dependencies
        None.
*/
    virtual int CDECL Process (const CAPI_BufList_t* input, CAPI_BufList_t* output, CAPI_Buf_t* params) = 0;

/**
        Initializes the CAPI library.

        @datatypes
        CAPI_Buf_t

        @param[in,out] params  Optional pointer to a parameter buffer.
                               Parameters can be used as input or modified by
                               the library.

        @detdesc
        This function allocates all memories (if they were not already allocated
        in the constructor) and sets reasonable initial values.
        @par
        This function must be called only once per session.
        @par
        For C CAPIs, memory allocations required by the CAPI module are to be
        performed in %Init(). For C++ CAPIs, memory allocations are to be
        performed in the constructor.

      @return
        Status/error code for decoders:
        - #DEC_SUCCESS    
        - #DEC_BADPARAM_FAILURE    
        - #DEC_NOMEMORY_FAILURE    
        - #DEC_NOTIMPL_FAILURE    
        - #DEC_FAILURE
        @par
        Status/error code for encoders:
        - #ENC_SUCCESS    
        - #ENC_BADPARAM_FAILURE    
        - #ENC_NOMEMORY_FAILURE    
        - #ENC_NOTIMPL_FAILURE    
        - #ENC_FAILURE

        @dependencies
        %Init() must always be called before Process() is called.
*/
    virtual int CDECL Init (CAPI_Buf_t* params) = 0;

/**
        Reinitializes the CAPI library.

        @datatypes
        CAPI_Buf_t

        @param[in,out] params  Pointer to a parameter of tData type. The byte
                               pointer points to a list of parameters that will
                               either be modified or read by the library or
                               application.

        @detdesc
        This function does not allocate any memory. It simply resets the library
        to the reinitialized state.
        @par
        This function can be called after a Flush is issued by the client.
        It can also be used after certain CAPI parameters are modified through
        a SetParam() call and certain library reinitializations need to be
        performed with the new value of the parameter. In such cases, the
        parameters that must be retained throughout the playback/record
        operation must not be reset.
        @par
        This function does not allocate any memory. It simply resets the
        library to the reinitialized state.

      @return
        Status/error code for decoders:
        - #DEC_SUCCESS    
        - #DEC_BADPARAM_FAILURE    
        - #DEC_NOTIMPL_FAILURE    
        - #DEC_FAILURE
        @par
        Status/error code for encoders:
        - #ENC_SUCCESS    
        - #ENC_BADPARAM_FAILURE    
        - #ENC_NOTIMPL_FAILURE    
        - #ENC_FAILURE

        @dependencies
        None.
*/
    virtual int CDECL ReInit (CAPI_Buf_t* params) = 0;

/**
        Returns the library to the uninitialized state.

      @return
        Status/error code for decoders:
        - #DEC_SUCCESS    
        - #DEC_NOTIMPL_FAILURE    
        - #DEC_FAILURE
        @par
        Status/error code for encoders:
        - #ENC_SUCCESS    
        - #ENC_NOTIMPL_FAILURE    
        - #ENC_FAILURE

        @dependencies
        The caller must ensure that this function is called only if there was
        a successful call to Init().
*/
    virtual int CDECL End (void) = 0;
};
#endif /* !defined(__cplusplus) */

#endif /* #ifndef CAPI_H */
