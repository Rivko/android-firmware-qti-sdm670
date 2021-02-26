/**
@file AudioStreamMgr.h
@brief This file declares AudioStreamMgr.

This is a service for the Audio Stream Managment. This service
receives and processes all control commands, creates/control/destroy
all audio dynamic services for playback and recording.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The elite_audio_mainpage.dox file contains all file/group descriptions
      that are in the output PDF generated using Doxygen and Latex. To edit or
      update any of the file/group text in the PDF, edit the
      elite_audio_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the "audio_stream" group
      description in the elite_audio_mainpage.dox file.
===========================================================================*/

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/aud/services/static_svcs/audio_stream_mgr/inc/AudioStreamMgr.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
07/26/09    SJ      Created file.
03/18/10   DC      Module renamed to Audio Stream Manager
04/01/10   DC      Rearchtecture to support concurrency
10/20/10   sw      (Tech Pubs) Edited/added Doxygen comments and markup.
==========================================================================*/

/*-----------------------------------------------------------------------
Copyright (c) 2010-2014, 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
-----------------------------------------------------------------------*/

#ifndef AUDIO_STREAM_MGR_H
#define AUDIO_STREAM_MGR_H

/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/

/* System */
#include "qurt_elite.h"

/* Audio */
#include "Elite.h"
#include "adsp_core_api.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/** @addtogroup audio_stream
@{ */

/*-------------------------------------------------------------------------
Preprocessor Definitions and Constants
-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------
Type Declarations
-------------------------------------------------------------------------*/
/**
  @brief Argument passed to a dynamic service for that service to directly
  raise an event with a client or to send an asynchronous event to a static
  service.
*/
typedef struct {

   /*The following is necessary information for direct communication with
client
      and for talking with APR*/
   uint32_t aprHandle;             /**< APR handle that is required in sending
                                        an acknowledgment. */
    elite_apr_addr_t clientAddr;   /**< Client's address (16 bits). */
    elite_apr_port_t clientPort;   /**< Client's port (16 bits). */
    elite_apr_addr_t selfAddr;     /**< Service's address (16 bits). */
    elite_apr_port_t selfPort;     /**< Service's port (16 bits). */

    /* The following is necessary information for communication with ASM. */
   qurt_elite_queue_t *pAsyncRespQ;     /**< Queue for a dynamic service to send an
                                        asynchronous event to a static
                                        service. */
    uint32_t unAsynClientToken;    /**< Token that is to be used to when
                                        sending an asynchrnous event to a
                                        static service. */
} AudioStreamMgr_CallBackHandleType;

/**
  Generic callback function type structure for a dynamic service to make a
  callback to either a client or the static manager.
 */
typedef ADSPResult (*Elite_CbFctType) ( void *pCbHandle,
                             uint32_t eventOpcode,
                             uint32_t token,
                             void *pEventPayload,
                             uint32_t ulPayloadSize);

/**
  @brief Callback structure used when specifying both a callback function and
  callback handle.
*/
typedef struct {
   Elite_CbFctType  pCbFct;        /**< Pointer to the callback function.  */
   void            *pCbHandle;     /**< Pointer to the handle. The dynamic
                                        service makes a copy of this
                                        handle buffer whose size is specified
                                        in ulHandleSize. */
   uint32_t         ulHandleSize;  /**< Size of the callback handle buffer. */
} Elite_CbType;

/*---------------------------------------------------------------------------
Class Definitions
----------------------------------------------------------------------------*/

/** @} */ /* end_addtogroup audio_stream */
/** @addtogroup audio_stream
@{ */

/*---------------------------------------------------------------------------
Function Declarations and Documentation
----------------------------------------------------------------------------*/
/**
 * This function returns the ASM service version
 *
 * @param[out] svc_info_ptr    pointer to service info structure
 */
ADSPResult asm_svc_get_api_version(avs_svc_api_info_t *svc_info_ptr);

/**
  Creates an instance of the AudoStreamMgr service.

  @param[in]  inputParam  unResponseResultis not used.
  @param[out] handle      Service entry handle that is returned to the caller.

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult AudioStreamMgr_Create (uint32_t inputParam, void **handle);



/** @} */ /* end_addtogroup audio_stream */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef AUDIO_STREAM_MGR_H

