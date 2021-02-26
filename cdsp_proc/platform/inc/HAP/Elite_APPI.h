#ifndef ELITE_APPI_H
#define ELITE_APPI_H

/*============================================================================*/
/**
@file Elite_APPI.h

@brief Audio Post Processor Interface header file

  This file defines a generalized C interface that can wrap a wide
  variety of audio postprocessing modules, so that they can be treated the
  same way by control code.
*/

/*==============================================================================
NOTE: The @brief description above does not appear in the PDF.

      The Elite_APPI_mainpage.dox file contains the group/module descriptions that
      are displayed in the output PDF generated using Doxygen and LaTeX. To
      edit or update any of the group/module text in the PDF, edit the
      Elite_APPI_mainpage.dox file or contact Tech Pubs.
==============================================================================*/

/*==============================================================================
  Copyright (c) 2010-2011, 2013 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

#include "mmdefs.h"
#include "adsp_error_codes.h"

#define APPI_PARAM_ID_ALGORITHMIC_RESET 0x00010707
#define APPI_PARAM_ID_ALGORITHMIC_DELAY 0x00010708
#define APPI_PARAM_ID_PROCESS_CHECK     0x00010709

#define APPI_PARAM_ID_KPPS               0x00010776
/**< Unique identifier for KPPS (Kilo Packets Per Second)Queries, 
  the value is an unsigned 32-bit integer (same structure as used for
  APPI_PARAM_ID_ALGORITHMIC_DELAY). */

#define APPI_PARAM_ID_PEER               0x00010777
/**< Unique identifier to set the handle of the peer service. 
 For example, Receive path on device serves as peer to Transmit path on device 
 in the case of Voice and vice versa.*/

#define APPI_PARAM_ID_MODULE_CAL         0x00010778
/**< Unique identifier to indicate to set_param function that the param structure 
  being sent down must be read in the voice_param_data_t calibration payload format. 
  For example:

  CALLER
      appi_buf_t params_buf;
      params_buf.data_ptr = (int8_t*)payload_address;
      params_buf.actual_data_len = payload_size;
      params_buf.max_data_len = payload_size;
      (void)appi_ptr->vtbl_ptr->set_param (
         so.appi_ptr, 
         APPI_PARAM_ID_MODULE_CAL, 
         &params_buf);

  CALLEE
      // Read data_ptr and max_data_len 
      while (payload_size)
      {
          param_ptr = (voice_param_data_t *)payload_address;
          payload_ptr= (int8_t *)param_ptr + sizeof(voice_param_data_t);

          // payload_ptr contains the exact tuning information.
          // Adjust payload_size
      };
      // Since set_param contains input buffer, callee must write back 
      // actual_data_len with however much was consumed.
 */

#define APPI_PROP_ID_DATA_FORMAT        0x00010779
/**< Unique identifier to set the data format property. Content is associated 
  with the appi_prop_t structure. */

#define APPI_PROP_ID_FRAME_SIZE         0x0001077A
/**< Unique identifier to set the processing frame size property. Frame size
 is an unsigned 32-bit integer that represents the size (in bytes) per channel of 
 the primary data input. */

#define APPI_PARAM_ID_KBPS               0x000108C8
/**< Unique identifier for KBPS (Kilo bits Per Second). This value need to 
 be provided by considering cache size as zero which gives worst kbps.
Queries the value is an unsigned 32-bit integer (same structure as used for
APPI_PARAM_ID_ALGORITHMIC_DELAY). */

/**<
 * This indicates the format of the compressed data in IEC61937. Eg. AC3, EAC3, DTS etc */
#define APPI_PARAM_ID_COMPRESSED_FORMAT 0x0001076D

#define APPI_PARAM_ID_CLIENT_INFO 0x000108FF
/**< Unique identifier to send the custom client information to the algorithm appi framework to use it for passing custom data between two appi modules on different threads. 
  For example:

  CALLER
      appi_buf_t params_buf;
      params_buf.data_ptr = (int8_t*)cust_client_list_ptr;
      params_buf.actual_data_len = sizeof(bufQList*);
      params_buf.max_data_len = sizeof(bufQList*);
      (void)appi_ptr->vtbl_ptr->set_param (
         so.appi_ptr, 
         APPI_PARAM_ID_CLIENT_INFO, 
         &params_buf);

  CALLEE
      // Read data_ptr and max_data_len 
	 // Read and Cache the cust_client_list_ptr for future use
 
	Refer to /adsp_proc/avs/afe/services/static/inc/AFEInternal.h for the definition of bufQList
 */


#define APPI_PARAM_ID_BGT_INFO 0x00010900
/**< Unique identifier to send the back ground thread handler information to the algorithm appi framework for the use of instantiating background thread for algorithm processing. 
  Content is associated with appi_bgt_info_t structure
  For example:

  CALLER
      appi_buf_t params_buf;
      appi_bgt_info_t bgt_info;
      afe_dev_port_t *pDevPort;
 
	  bgt_info.intf = pDevPort->intf;
	  bgt_info.bgt_handle = &pDevPort->bgt_handle;
	
      params_buf.data_ptr = (int8_t*)&bgt_info;
      params_buf.actual_data_len = sizeof(appi_bgt_info_t);
      params_buf.max_data_len = sizeof(appi_bgt_info_t);
      (void)appi_ptr->vtbl_ptr->set_param (
         so.appi_ptr, 
         APPI_PARAM_ID_BGT_INFO, 
         &params_buf);

  CALLEE
      // Read data_ptr and max_data_len
	  while (payload_size)
      {
		  // Cache the bgt handle information for future use

      };
 
	  Refer to /adsp_proc/avs/afe/services/static/inc/AFEInternal.h for the definition of afe_dev_port_t
 */

/** @ingroup data_types
  Structure for a back ground thread information that is passed
  into APPI functions. 
   
  This background information is used by APPI modules to use 
  self destructing background thread. 
 */
typedef struct appi_bgt_info_t
{
	int16_t             intf;
						/**< AFE port interface which contains the back ground thread handler */
	void				**bgt_handle;
						/**< pointer to background thread handler */

}appi_bgt_info_t;

#define APPI_PARAM_ID_FB_PATH_INFO 0x00010901
/**< Unique identifier to send the feedback path information to the algorithm appi framework for the use of feedback path between two appi modules on different threads
  For example:

  CALLER
      appi_buf_t params_buf;
      
      params_buf.data_ptr = (int8_t*)&feedback_path_info;
      params_buf.actual_data_len = sizeof(afe_port_feedback_info_t);
      params_buf.max_data_len = sizeof(afe_port_feedback_info_t);
      (void)appi_ptr->vtbl_ptr->set_param (
         so.appi_ptr, 
         APPI_PARAM_ID_FB_PATH_INFO, 
         &params_buf);

  CALLEE
      // Read data_ptr and max_data_len
  	  while (payload_size)
      {
		  // Cache the feedback path information for future use

      };
	  Refer to /adsp_proc/avs/afe/services/static/inc/AFEFeedback.h for the definition of afe_port_feedback_info_t
 */

#define APPI_PARAM_ID_INT_INFO 0x00010902
/**< Unique identifier to send the Interrrupt information to the appi framework
  For example:

  CALLER
      appi_buf_t params_buf;
      
      params_buf.data_ptr = (int8_t*)&int_info;
      params_buf.actual_data_len = sizeof(interrupt data type);
      params_buf.max_data_len = sizeof(interrupt data type);
      (void)appi_ptr->vtbl_ptr->set_param (
         so.appi_ptr, 
         APPI_PARAM_ID_INT_INFO, 
         &params_buf);

  CALLEE
      // Read data_ptr and max_data_len 
      // Read and Pass the interrupt information to algorithm
 */


/** Gets the IDs of the custom framework extensions to be used for this module.

  @par Example
  Caller:
  @par
@code
appi_buf_t fwk_extn_buf;
fwk_extn_t fwk_extn;
fwk_extn_buf.data_ptr = (uint32_t *) &fwk_extn;
fwk_extn_buf.max_data_len = sizeof(fwk_extn_t);

appi_ptr->vtbl_ptr->get_param(appi_ptr, APPI_PARAM_ID_FWK_EXTN, &fwk_extn_buf);
@endcode
*/
#define APPI_PARAM_ID_FWK_EXTNS 0x00010905


#define APPI_PARAM_ID_CLIP_PCM_INFO 0x00010903
/**< Unique identifier to send the clip pcm levels information to the appi algo framework to handle pcm levels when clip interrupt occurs (specific to feedback speaker protection)
  For example:

* @} */ /* end_addtogroup appi_macros */

/** @addtogroup appi_data_types
@{ */
/**
  Buffer that is passed into an APPI function in a structure that includes its
  allocated size and number of valid bytes. This structure can also be used
  to pass tuning parameters and state information to/from the libraries.

  The actual_data_len field has multiple uses, depending on the context:
  - When the APPI buffer is an input to a function, the caller specifies
    the number of valid bytes using this field. The callee overwrites this
    field with the number of bytes that were consumed.
  - When the APPI buffer is the output of a function, the caller does not
    use this field. The callee fills this field with the number of
    generated valid bytes in the output buffer.

  Within the function call, the actual_data_len field is updated by the
  APPI to reflect the number of valid bytes still remaining in the buffer.
  For example, before the call:

@code
pIn->data_ptr = &(start of in buffer);
pIn->actual_data_len = 2000;           // bytes
pIn->max_data_len = 2000;              // bytes
pOut->data_ptr = &(start of out buffer);
pOut->actual_data_len = 0;             // bytes
pOut->max_data_len = 8192;             // bytes
@endcode

  After the call (400 bytes consumed, 800 bytes produced):

@code
pIn->data_ptr = &(start of in buffer);
pIn->actual_data_len = 400;            // bytes
pIn->max_data_len = 2000;              // bytes
pOut->data_ptr = &(start of out buffer);
pOut->actual_data_len = 0+800;         // bytes
pOut->max_data_len = 8192;             // bytes
@endcode
  @newpage
*/
typedef struct
{
   int8_t* data_ptr;
   /**< Data pointer to the raw data. */

   uint32_t actual_data_len;
   /**< Length of the valid data (in bytes). */

   uint32_t max_data_len;
   /**< Total allocated size (in bytes) of the buffer.

        When a pointer of type %appi_buf_t is passed as an input argument of a
        function, this parameter specifies the allocated size of the buffer
        and is untouched by the callee. */

} appi_buf_t;

/**
  List of APPI buffers. This structure can be used for functions that require
  multiple input or output buffers.

  This structure consists of a pointer to an array of type appi_buf_t. For
  multichannel deinterleaved data, each buffer contains data from all 
  the channels in sequence.

  Following are typical usage conventions (where M = number of channels and
  N = number of samples):
  - Mono input and output
    - Input buffer contains N PCM samples (bufs_num = 1)
    - Output buffer contains N PCM samples (bufs_num = 1)
  - Multichannel (M channels) input and output, deinterleaved:
    - Input buffer contains N/M PCM samples per channel -- deinterleaved 
      (bufs_num = M)
    - Output buffer contains N/M PCM samples per channel -- deinterleaved 
      (bufs_num = M)
 */
typedef struct
{
   appi_buf_t* buf_ptr;     /**< Array of APPI buffer elements. */
   uint32_t bufs_num;       /**< Number of elements in the array. */
} appi_buflist_t;

/**
  Property that can be sent to a module.

  Although not explicitly used as a function parameter, this structure can be
  embedded in the prop_ptr field of the appi_proplist_t structure, thus
  providing the ability to send a list of properties. These properties can be
  used to determine the memory requirements of the module, for example.

  See appi_getsize_v2_f() for an implementation example.
*/
typedef struct
{
   uint32_t id;
   /**< Unique identifier of the property that is being sent. */

   uint32_t size;
   /**< Size of the property data that is being sent. */

   int8_t* info_ptr;
   /**< Pointer to the property information (metadata) that is understood 
        by the module and its client. */

} appi_prop_t;

/**
  List of APPI properties.

  This structure can be used to send a list of properties to the APPI.
  It consists of a pointer to an array of type appi_prop_t. 
 */
typedef struct
{
   appi_prop_t* prop_ptr;   /**< Array of APPI property elements. */
   uint32_t props_num;      /**< Number of elements in the array. */
} appi_proplist_t;

/** Maximum number of channels. */
#define APPI_MAX_CHANNELS 16

/**
  Data format that is passed into the APPI.

  The output data format must be the same as the input data format. The
  supported data formats for all of the APPIs are listed in
  @xhyperref{chp:appxA,Appendix A}.
 */
typedef struct
{
   uint32_t num_channels;
   /**< Number of channels. This value is required for IEC61937 data
        (refer to @xhyperref{S1,[S1]}). */

   uint32_t bits_per_sample;
   /**< Number of bits per sample. The value is always 16 for IEC61937
        data. */

   uint32_t sampling_rate;
   /**< Sampling rate in samples per second. This value is required for
        IEC61937 data. */

   uint32_t data_is_signed;
   /**< Specifies whether data is signed. This is not required for IEC61937
        data.
        
        @values
        - 0 -- Unsigned
        - 1 -- Signed @tablebulletend */

   uint32_t data_is_interleaved;
   /**< Specifies whether data is interleaved. This is not required for
        IEC61937 data.

        @values
        - 0 -- Deinterleaved
        - 1 -- Interleaved @tablebulletend */

   uint8_t channel_type[APPI_MAX_CHANNELS];
   /**< Array of channel types for each num_channel. This is not required for
        IEC61937 data.

        @values PCM_CHANNEL_* types defined in @xhyperref{Q3,[Q3]}. @newpagetable */

   /* adsp_media_fmt.h */
   /*Q3 = Hexagon Multimedia: AFE API Interface Spec. for 2.4 (80-NF769-3) */

} appi_format_t;

/** @} */ /* end_addtogroup appi_data_types */


/* appi_t plain C interface wrapper for the vTable */
typedef struct appi_t appi_t;

/** @ingroup appi_virt_func_table
  Virtual function table structure for plain C implementations of
  APPI-compliant objects.

  Objects that are written in plain C must have a pointer to a virtual function
  table as the first element in their instance structure. This %appi_vtbl_t
  structure is the virtual function table type for all such objects, as listed
  below.
 */
typedef struct
{
   /** @ingroup appi_virt_func_reinit
     Reinitializes the APPI library.

     @datatypes
     appi_t \n
     appi_format_t \n
     appi_buf_t

     @param[in,out] _pif            Pointer to the library object.
     @param[in,out] info_ptr        Pointer to the initialization information
                                    (such as state and media type).
     @param[in]     in_format_ptr   Pointer to the input data format.
     @param[out]    out_format_ptr  Pointer to the output data format to be
                                    returned by the library.

     @detdesc
     If the algorithm does not support a specific data format, the appropriate
     error code must be returned with a debug message. For example:
     @par
@code
MSG_1(MSG_SSID_QDSP6,DBG_ERROR_PRIO,"APPI Libname Unsupported input 
sampling rate %ld", in_format_ptr->sampling_rate);
@endcode
     @par
     This function does not reset any tunable parameters that have been set
     by set_param().
     @par
     Before returning SUCCESS and once the function is complete, a debug
     message is to be printed:
     @par
@code
MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"APPI Libname Renit done with outformat
%lu,%lu,%lu,%lu,%lu",
   out_format_ptr->num_channels,
   out_format_ptr->bits_per_sample,
   out_format_ptr->sampling_rate,
   out_format_ptr->data_is_signed,
   out_format_ptr->data_is_interleaved);
@endcode

     @par Example for out_format_ptr
     All format fields must be filled by %reinit() before returning.
     @par
@code
out_format_ptr->num_channels = in_format_ptr->num_channels;
out_format_ptr->bits_per_sample = in_format_ptr->bits_per_sample;
out_format_ptr->sampling_rate = in_format_ptr->sampling_rate;
out_format_ptr->data_is_signed = in_format_ptr->data_is_signed;
out_format_ptr->data_is_interleaved = in_format_ptr->data_is_interleaved;
@endcode
     @newpage

     @return
     Indication of success or failure.

     @dependencies
     The appi_getsize_f() and appi_init_f() functions must have been executed,
     and memory must have been allocated.
   */
   ADSPResult (*reinit)(appi_t* _pif,
         const appi_format_t* in_format_ptr,
         appi_format_t* out_format_ptr,
         appi_buf_t* info_ptr);

   /** @ingroup appi_virt_func_process
     Processes an input buffer and generates an output buffer.

     @datatypes
     appi_t \n
     appi_buflist_t \n
     appi_buf_t

     @param[in,out] _pif       Pointer to the library object.
     @param[in,out] input      Pointer to the input buffer object. The function
                               must modify the actual_data_len field to
                               indicate how many bytes were consumed.
     @param[out]    output     Pointer to the output buffer object. The
                               function sets the actual_data_len field to
                               indicate how many bytes were generated.
     @param[in,out] info_ptr   Pointer to optional information (such as state
                               and media type). This buffer may contain a list
                               of parameters that will either be modified or
                               read by the library or application.

     @detdesc
     This is a generic processing function. It postprocesses raw PCM audio,
     voice samples, etc., to generate PCM output. In the event of a failure,
     the  appropriate error code is returned.
     @par
     On each call to %process(), a library consumes the entire input buffer or
     fills up the entire output buffer. If the input buffer size is zero, the
     library fills up as much of the output buffer as it can if it has any
     samples ready internally.
     @par
     If the library performs any buffering on the input side, it consumes as
     much of the input buffer as it can. This helps to free the input buffer
     and reduce upstream delay.
     @par
     No debug messages are allowed in this function.
     @par
     The process size is the lower of the actual size of the input available
     and the maximum output buffer size available.

     @return
     Indication of success or failure.

     @dependencies
     The reinit() function must have been executed.
    */
   ADSPResult (*process)(appi_t* _pif,
         const appi_buflist_t* input,
         appi_buflist_t* output,
         appi_buf_t* info_ptr);

   /** @ingroup appi_virt_func_end
     Returns the library to the uninitialized state and frees the memory that
     was allocated by appi_init_f(). This function also frees the virtual
     function table.

     @datatypes
     appi_t

     @param[in,out] _pif    Pointer to the library object.

     @detdesc
     Before returning SUCCESS and once the function is complete, a debug
     message is to be printed:
     @par
@code
MSG(MSG_SSID_QDSP6,DBG_HIGH_PRIO,"APPI Libname End done");
@endcode
     @par
     @note1hang This function undoes all the work that appi_init_f() has done.
                Therefore, sequences such as %appi_init_f() > %end() > %reinit()
                are not allowed.

     @return
     Indication of success or failure.

     @dependencies
     None.
    */
   ADSPResult (*end)(appi_t* _pif);

   /** @ingroup appi_virt_func_set_params
     Sets either a parameter value or a parameter structure containing multiple
     parameters.

     @datatypes
     appi_t \n
     appi_buf_t

     @param[in,out] _pif        Pointer to the library object.
     @param[in]     param_id    Parameter ID of the parameter whose value is
                                being passed in this function. For example: \n
                                - APPI_LIBNAME_ENABLE
                                - APPI_LIBNAME_FILTER_COEFF @tablebulletend
     @param[in]     params_ptr  Pointer to the buffer containing the value of
                                the parameter. The format depends on the
                                implementation.

     @detdesc
     In the event of a failure, the appropriate error code is to be
     returned.
     @par
     The non-tuning param_id field, #APPI_PARAM_ID_ALGORITHMIC_RESET, must be
     supported. When it is exercised, the APPI must clear all of its internal
     buffers and filter states.
     @par
     The actual_data_len field of the parameter pointer must be at least the
     size of the parameter structure. Therefore, the following check must be
     exercised for each tuning parameter ID.
     @par
@code
if (params_ptr->actual_data_len >= sizeof(asm_gain_struct_t))
{
   :
   :
}
else
{
  MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"APPI Libname Set, Bad param size
  %lu",params_ptr->actual_data_len);
  return ADSP_ENEEDMORE;
}

@endcode
     @par
     Optionally, some parameter values can be printed for tuning verification.
     @par
     Before returning SUCCESS and once the function is complete, a debug message
     is to be printed:
     @par
@code
MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"APPI Libname Set param for 0x%lx done",param_id);
@endcode @newpage

     @return
     None.

     @dependencies
     None.
    */
   ADSPResult (*set_param)(appi_t* _pif,
         uint32_t param_id,
         const appi_buf_t *params_ptr);

   /** @ingroup appi_virt_func_get_params
     Gets either a parameter value or a parameter structure containing multiple
     parameters.

     @datatypes
     appi_t \n
     appi_buf_t

     @param[in,out] _pif        Pointer to the library object.
     @param[in]     param_id    Parameter ID of the parameter whose value is
                                being passed in this function. For example: \n
                                - APPI_LIBNAME_ENABLE
                                - APPI_LIBNAME_FILTER_COEFF @tablebulletend
     @param[out]    params_ptr  Pointer to the buffer containing the value of
                                the parameter. The format depends on the
                                implementation.

     @detdesc
     In the event of a failure, the appropriate error code is to be
     returned.
     @par
     The following non-tuning param_ids must be supported:
     - #APPI_PARAM_ID_PROCESS_CHECK -- Specifies whether the processing
       function for the APPI needs to be invoked at all.
       - For example, if the algorithm is disabled, the upper layer need not
         call process() for this APPI.
       - Another example is if the APPI does not support the ongoing sampling
         rate and instead chooses to be a pass-through module; in this case,
         it can return FALSE when the param_id is queried by the upper layer.
     - #APPI_PARAM_ID_ALGORITHMIC_DELAY -- When exercised, the APPI returns the 
       number of zero samples that must be passed into the APPI to get all the
       valid data held in its internal buffers out in its current
       configuration.
       - For example, when the APPI is required to flush out all valid content 
       before terminating a playback session.
     @par
     The max_data_len field of the parameter pointer must be at least the size
     of the parameter structure. Therefore, the following check must be
     exercised for each tuning parameter ID.
     @par
@code
if (params_ptr->max_data_len >= sizeof(asm_gain_struct_t))
{
   :
   :
}
else
@endcode
     @newpage
     @par
@code
{
   MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,"APPI Libname Get, Bad param size
   %lu",params_ptr->max_data_len);
   return ADSP_ENEEDMORE;
}
@endcode
     @par
     Before returning, the actual_data_len field must be filled with the number
     of bytes written into the buffer.
     @par
     Optionally, some parameter values can be printed for tuning verification.
     @par
     Before returning SUCCESS and once the function is complete, a debug message
     is to be printed:
     @par
@code
MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"APPI Libname Get param for 0x%lx done",param_id);
@endcode

     @return
     Indication of success or failure.

     @dependencies
     None.
    */
   ADSPResult (*get_param)(appi_t* _pif,
         uint32_t param_id,
         appi_buf_t *params_ptr);

   /** @ingroup appi_virt_func_get_input
     Gets the number of input samples needed to produce a certain number of
     output samples.

     @datatypes
     appi_t

     @param[in,out] _pif             Pointer to the library object.
     @param[in]     output_size      Number of output samples that are required.
     @param[out]    input_size_ptr   Pointer to the number of input samples
                                     the library requires to produce
                                     output_size samples.
     @detdesc
     If the library has internal buffering, it can take into
     account the state of its buffers and return the appropriate size.
     @par
     No debug messages are allowed in this function.
     @par
     For modules that take compressed data per IEC61937 (refer to
     @xhyperref{S1,[S1]}), this function must set the input size to be the
     same as the output_size to avoid reframing.

     @return
     Indication of success or failure.

     @dependencies
     None.
    */
   ADSPResult (*get_input_req)(appi_t* _pif,
         const uint32_t output_size,
         uint32_t *input_size_ptr);

} appi_vtbl_t;


/** @ingroup appi_data_types
  Plain C interface wrapper for the virtual function table, appi_vtbl_t.

  This %appi_t structure appears to the caller as a virtual function table.
  The virtual function table in the instance structure is followed by other
  structure elements, but those are invisible to the users of the APPI object.
  This %appi_t structure is all that is publicly visible.
 */
struct appi_t
{
   appi_vtbl_t *vtbl_ptr;  /**< Pointer to the virtual function table. */
};


/** @ingroup appi_func_get_mem_req
  Gets the memory requirement of the library. Optional input parameters can be
  sent down to the APPI wrapper to calculate worst case memory requirements.

  @datatypes
  appi_buf_t

  @param[in]  params_ptr  Pointer to the buffer containing optional parameters
                          that contribute to the increased memory requirement.
  @param[out] size_ptr    Pointer to the number of bytes of memory being
                          requested.

  @par Example for params_ptr
  Consider a library that filters audio data.
  The upper layer can set a specific number of filter taps as the maximum that
  the library will ever require. Internal to the APPI wrapper, the following
  code can be used to extract the filter taps information and state the amount
  of memory required.
  @par
  After the upper layer allocates the memory, it calls appi_init_f() so the
  memory can be distributed for its taps.
  @par
@code
uint32_t* params = ((uint32_t*)(params_ptr->data_ptr));
uint32_t filter_taps = params[0];
*size_ptr = align_to_8_byte(filter_taps*sizeof(uint32_t));
          // align_to_8_byte() is a utility to round up
          // input integer to the nearest multiple of 8
@endcode

  @return
  Indication of success or failure.

  @dependencies
  None.
 */
typedef ADSPResult (*appi_getsize_f) (
      const appi_buf_t* params_ptr,
      uint32_t* size_ptr);

/** @ingroup appi_func_get_mem_req2
  Gets the memory requirement of the library. Optional input parameters can be
  sent to the APPI wrapper to calculate the worst case memory requirements.

  This function differs from appi_getsize_f() in that it uses a property list
  instead of a buffer list for the info_ptr field.

  @datatypes
  appi_proplist_t

  @param[in]  params_ptr  Pointer to the property list containing optional
                          parameters that contribute to the increased memory
                          requirement. The list enables varying properties to
                          be sent to the module.
  @param[out] size_ptr    Pointer to the number of bytes of memory being
                          requested.

  @detdesc
  The upper layer can create an array of properties that the library parses
  to respond with its memory requirements.
  @par
  After the upper layer allocates the memory, it calls appi_init_v2_f() so the
  memory can be distributed internally.
  @par
@code
// Pseudo code to depict passing properties to APPI's getsize_v2 function.
uint32 i=0;
appi_prop_t* prop_ptr;

// Iterate through number of properties being sent.
while (params_ptr->props_num--)
{
   prop_ptr= (appi_prop_t*) (params_ptr->prop_ptr[i++].data_ptr);
   switch (prop_ptr->id)
   {
      case APPI_PROP_ID_DATA_FORMAT:
         if (prop_ptr->size == sizeof(appi_format_t))
         {
            appi_format_t* fmt_ptr = (appi_format_t*)prop_ptr->info_ptr;
            // Use format pointer thus obtained to calculate final size.
         }
         break;
      default:
         break;
   }
}
@endcode

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
typedef ADSPResult (*appi_getsize_v2_f) (
      const appi_proplist_t* params_ptr,
      uint32_t* size_ptr);

/** @ingroup appi_func_init
  Instantiates the library to set up the virtual function table, and also
  allocates any memory required by the library.

  @datatypes
  appi_t \n
  appi_format_t \n
  appi_buf_t

  @param[in,out] _pif            Pointer to the library object.
  @param[out]    is_inplace_ptr  Pointer to the library that supports in-place
                                 computation (the input and output buffer are
                                 the same).
  @param[in]     in_format_ptr   Pointer to the input data format.
  @param[out]    out_format_ptr  Pointer to the output data format to be
                                 returned by the library.
  @param[in,out] info_ptr        Pointer to the initialization information
                                 (such as state and media type). For example,
                                 as indicated in appi_getsize_f(), this may be
                                 the same buffer that was used to send filter
                                 taps information.

  @detdesc
  States within the library must be initialized at the same time.
  @par
  Input parameters are sent to the APPI wrapper.
  @par
  If the algorithm does not support a specific data format, the appropriate
  error code must be returned with a debug message. For example:
  @par
@code
MSG_1(MSG_SSID_QDSP6,DBG_ERROR_PRIO,"APPI Libname Unsupported input sampling rate %ld", in_format_ptr->sampling_rate);
@endcode
  @par
  Before returning SUCCESS and once the function is complete, a debug message
  is to be printed:
  @par
@code
MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"APPI Libname Renit done with outformat
%lu,%lu,%lu,%lu,%lu",
   out_format_ptr->num_channels,
   out_format_ptr->bits_per_sample,
   out_format_ptr->sampling_rate,
   out_format_ptr->data_is_signed,
   out_format_ptr->data_is_interleaved);
@endcode
  @newpage

  @par Example for out_format_ptr
  All format fields are filled by %appi_init_f() before returning.
  @par
@code
out_format_ptr->num_channels = in_format_ptr->num_channels;
out_format_ptr->bits_per_sample = in_format_ptr->bits_per_sample;
out_format_ptr->sampling_rate = in_format_ptr->sampling_rate;
out_format_ptr->data_is_signed = in_format_ptr->data_is_signed;
out_format_ptr->data_is_interleaved = in_format_ptr->data_is_interleaved;
@endcode

  @return
  Indication of success or failure.

  @dependencies
  The appi_getsize_f() function must have been executed, and any memory
  requested must have been allocated.
*/
typedef ADSPResult (*appi_init_f) (
      appi_t*              _pif,
      bool_t*              is_inplace_ptr,
      const appi_format_t* in_format_ptr,
      appi_format_t*       out_format_ptr,
      appi_buf_t*          info_ptr);

/** @ingroup appi_func_init2
  Instantiates the library to set up the virtual function table, and also
  allocates any memory required by the library.

  This function differs from appi_init_f() in that it uses a property list
  instead of a buffer list for the info_ptr field.

  @datatypes
  appi_t \n
  appi_format_t \n
  appi_proplist_t

  @param[in,out] _pif            Pointer to the library object.
  @param[out]    is_inplace_ptr  Pointer to the library that supports in-place
                                 computation (the input and output buffers are
                                 the same).
  @param[in]     in_format_ptr   Pointer to the input data format.
  @param[out]    out_format_ptr  Pointer to the output data format to be
                                 returned by the library.
  @param[in,out] info_ptr        Pointer to the initialization information
                                 (such as state and media type). For example,
                                 as indicated in appi_getsize_v2_f(), this may
                                 be the same property list that was used to
                                 send the properties.

  @detdesc
  Input parameters are sent to the APPI wrapper.
  @par
  States within the library must be initialized at the same time. The property
  list that was used for appi_getsize_v2_f() must be sent back for
  verification.
  @par
  If the algorithm does not support a specific data format, the appropriate
  error code must be returned with a debug message. For example:
  @par
@code
MSG_1(MSG_SSID_QDSP6,DBG_ERROR_PRIO,"APPI Libname Unsupported input sampling rate %ld", in_format_ptr->sampling_rate);
@endcode
  @par
  Before returning SUCCESS and once the function is complete, a debug message
  is to be printed:
  @par
  @code
  MSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"APPI Libname Renit done with outformat
  %lu,%lu,%lu,%lu,%lu",
     out_format_ptr->num_channels,
     out_format_ptr->bits_per_sample,
     out_format_ptr->sampling_rate,
     out_format_ptr->data_is_signed,
     out_format_ptr->data_is_interleaved);
  @endcode

  @par Example for out_format_ptr
  All format fields are filled by this function before returning.
  @par
@code
out_format_ptr->num_channels = in_format_ptr->num_channels;
out_format_ptr->bits_per_sample = in_format_ptr->bits_per_sample;
out_format_ptr->sampling_rate = in_format_ptr->sampling_rate;
out_format_ptr->data_is_signed = in_format_ptr->data_is_signed;
out_format_ptr->data_is_interleaved = in_format_ptr->data_is_interleaved;
@endcode

  @return
  Indication of success or failure.

  @dependencies
  The appi_getsize_v2_f() function must have been executed, and any memory
  requested must have been allocated.
*/
typedef ADSPResult (*appi_init_v2_f) (
      appi_t*              _pif,
      bool_t*              is_inplace_ptr,
      const appi_format_t* in_format_ptr,
      appi_format_t*       out_format_ptr,
      appi_proplist_t*     info_ptr);

#endif /* #ifndef APPI_H */
