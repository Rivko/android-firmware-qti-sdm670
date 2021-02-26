/**
@file UltrasounStreamMgr.h
@brief This file declares UltrasoundStreamMgr.

This is a service for the Ultrasound Stream Managment. This service
receives and processes all control commands, creates/control/destroy
all ultrasound dynamic services for stylus pen and free form use cases.
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

$Header:

when       who     what, where, why
--------   ---     -------------------------------------------------------
10/13/10   NR      File created 
03/10/14   LD      Dynamic service: support separate read and write queues 
==========================================================================*/

/*-----------------------------------------------------------------------
 Copyright (c) 2010 - 2014 Qualcomm Technologies, Inc.
 All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.          
 -----------------------------------------------------------------------*/

#ifndef ULTRASOUND_STREAM_MGR_H
#define ULTRASOUND_STREAM_MGR_H

/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/

/* System */
#include "qurt_elite.h"

/* Audio */
#include "Elite.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/** @addtogroup ultrasound_stream
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

    /* The following is necessary information for direct communication with client
      and for talking with APR */
    uint32_t apr_handle ;          /**< APR handle that is required in sending
                                        an acknowledgment. */
    elite_apr_addr_t client_addr;  /**< Client's address (16bit)*/
    elite_apr_port_t client_port;  /**< Client's port (16bit) */
    elite_apr_addr_t self_addr;    /**< Service's address (16bit) */
    elite_apr_port_t self_port;    /**< Service's port (16bit) */

} usm_callback_handle_t;

/** 
  @brief Structure with initialization parameters for dynamic
         service. Passed to the create function of any usm
         dynamic service
  TODO used only by processor service for now, rest of the
  services use service-specific structure, which should be
  converted to use this generic structure.
 */
typedef struct {
    uint32_t                direction;          /**< Direction of this session: 1=RX 2=TX */
    uint32_t                config_id;          /**< Media format ID of the
                                                encoder and decoder to be created */
    usm_callback_handle_t   *callback_data_ptr; /**< USM callback handle
                                                for event generation/ack */
    uint32_t                *mcps_ptr;          /**< Computational complexity of the service algorithm
                                                     in MIPS. The service should fill in this value during
                                                     its creation. 0 means no special requirements */
    uint64_t                *ddr_bw_total_ptr;      /**< Total memory bandwidth needed by service algorithm in
                                                     bytes per second. The service should fill in this value during
                                                     its creation. 0 means no special requirements */
    uint64_t                *ddr_bw_avg_ptr;        /**< Average memory bandwidth needed by service algorithm in
                                                     bytes per second. The service should fill in this value during
                                                     its creation. 0 means no special requirements */
    qurt_elite_queue_t      *read_dataQ;        /**< The data queue for CMD_READ calls. If NULL, use gpQ
                                                     from svc_handle */
    qurt_elite_queue_t      *write_dataQ;       /**< The data queue for CMD_WRITE calls. If NULL, use gpQ
                                                     from svc_handle */
} usm_dyn_svc_init_params_t;

/*---------------------------------------------------------------------------
Class Definitions
----------------------------------------------------------------------------*/
/** @} */ /* end_addtogroup ultrasound_stream */
/** @addtogroup ultrasound_stream
 * @{ */

/*---------------------------------------------------------------------------
Function Declarations and Documentation
----------------------------------------------------------------------------*/
/**
  Creates an instance of the UltrasoundStreamMgr service.

  @param[in]  inputParam  not used.
  @param[out] handle      Service entry handle that is returned to the caller.

  @return
  Indication of success or failure.

  @dependencies
  None.
*/
ADSPResult usm_create (uint32_t inputParam, void **handle);

 /** @} */ /* end_addtogroup ultrasound_stream */
#ifdef __cplusplus
}
#endif //__cplusplus 

#endif // #ifndef ULTRASOUND_STREAM_MGR_H

