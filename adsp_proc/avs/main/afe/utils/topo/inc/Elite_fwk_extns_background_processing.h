#ifndef ELITE_FWK_EXTNS_BACKGROUND_PROCESSING_H
#define ELITE_FWK_EXTNS_BACKGROUND_PROCESSING_H

/* ======================================================================== */
/**
@file Elite_fwk_extns_background_processing.h

@brief Frame work extensions for background processing header file (AFEBgtHandler.h)

  This file defines a framework extensions and corresponding private propeties
  needed for background processing in AFE.
*/

/* =========================================================================
  Copyright (c) 2010-2014 Qualcomm Technologies, Inc.(QTI)
  All rights reserved. Qualcomm Proprietary and Confidential.
  ========================================================================== */

/* =========================================================================
                             Edit History

   when       who     what, where, why
   --------   ---     ------------------------------------------------------
   08/11/14   roverma      Initial Version.
   ========================================================================= */
#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/


#include "Elite_CAPI_V2_types.h"


#define FWK_EXTN_BGT_PROC 0x00010276
/**< Unique identifier to represent custom framework extension for background processing */

#define CAPI_V2_PARAM_ID_BGT_MODULE_INFO 0x00010277
/**< Unique identifier to get the back ground thread handler information from the algorithm capi v2 framework for the use of instantiating background thread for algorithm processing.
  Content is associated with capi_v2_bgt_module_info_t structure
  For example:
  CALLER
    capi_v2_buf_t params_buf;
    capi_v2_bgt_module_info_t bgt_info;

    params_buf.data_ptr = (int8_t*)&bgt_info;
    params_buf.actual_data_len = sizeof(capi_v2_bgt_module_info_t);
    params_buf.max_data_len = sizeof(capi_v2_bgt_module_info_t);
    capi_ptr->vtbl_ptr->get_param (
    so.capi_ptr,
    CAPI_V2_PARAM_ID_BGT_MODULE_INFO,
    &port_info,
    &params_buf);

  CALLEE
      // Read data_ptr and max_data_len
      Fill in the proper requirements to spawn the background thread
 */

#define CAPI_V2_PARAM_ID_BGT_MODULE_INFO_API_VERSION                  0x1
/**< Version information used to handle future additions to background module information
    (for backward compatibility).
*/

typedef void capi_v2_fgt_data_t;
typedef void capi_v2_bgt_data_t;
typedef struct module_bgt_info_t module_bgt_info_t;

typedef struct capi_v2_bgt_module_info_t
{
   uint32_t version;
   /**< version to track the bgt struct for future expansion */
   capi_v2_err_t (*init)(capi_v2_fgt_data_t *fgt_data_ptr, capi_v2_bgt_data_t **bgt_data_pptr);
   /**< Pointer to init function that would be called from fwk during post-CB of Topo Start
    * after the thread is created */
   capi_v2_err_t (*deinit)(capi_v2_bgt_data_t **bgt_data_pptr);
   /**< Pointer to the deinit function that would be called from fwk duting post-CB of Topo Stop */
   capi_v2_err_t (*process)(module_bgt_info_t *bgt_info_ptr, capi_v2_bgt_data_t *bgt_data_ptr);
   /**<  Pointer to the process function, it will be called whenever there is buffer pushed
    * onto the data Q, i.e. whenever the FGT pushes onto the data Q for the BGT to process */
   capi_v2_fgt_data_t *fgt_data_ptr;
   /**<  This is the foreground data structure pointer, ONLY utilized to call the init function(as above)
    * from the framework */
   int32_t thread_prio;
   /**< This is the elite thread priority. Please refer to EliteThread_Priority.h */
   uint32_t stack_size;
   /**< Stack size of the BGT */
} capi_v2_bgt_module_info_t;



#define CAPI_V2_PARAM_ID_BGT_INFO 0x00010278
/**< Unique identifier to send the back ground thread handler information to the algorithm capi v2 framework for the use of instantiating background thread for algorithm processing.
  Content is associated with capi_v2_bgt_info_t structure
  For example:
  CALLER
    capi_v2_buf_t params_buf;
    capi_v2_bgt_info_t bgt_info;

    bgt_info.bgt_handle = &bgt_handle;

    params_buf.data_ptr = (int8_t*)&bgt_info;
    params_buf.actual_data_len = sizeof(capi_v2_bgt_info_t);
    params_buf.max_data_len = sizeof(capi_v2_bgt_info_t);
    capi_ptr->vtbl_ptr->set_param (
    so.capi_ptr,
    CAPI_V2_PARAM_ID_BGT_INFO,
    &port_info,
    &params_buf);

  CALLEE
      // Read data_ptr and max_data_len
	  while (payload_size)
      {
		  // Cache the bgt handle information for future use

      };
 
 */

/** @ingroup data_types
  Structure for a back ground thread information that is passed
  into CAPI V2 functions.
   
  This background information is used by CAPi V2 modules to use
  self destructing background thread. 
 */
typedef struct capi_v2_bgt_info_t
{
   module_bgt_info_t *bgt_info_ptr;
   /**< Handle to background thread */
} capi_v2_bgt_info_t;



#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* #ifndef ELITE_FWK_EXTNS_BACKGROUND_PROCESSING_H */
