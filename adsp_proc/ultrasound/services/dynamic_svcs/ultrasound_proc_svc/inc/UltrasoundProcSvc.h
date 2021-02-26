/*========================================================================*/
/**
@file UltrasoundProcSvc.h

@brief Ultrasound processor service public header.

This file contains Ultrasound decoder service belonging to the Elite framework.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The elite_ultrasound_mainpage.dox file contains all file/group descriptions
      that are in the output PDF generated using Doxygen and Latex. To edit or
      update any of the file/group text in the PDF, edit the
      elite_Ultrasound_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the "Ultrasound_decoder" group
      description in the Elite_Core_mainpage.dox file.
===========================================================================*/

/*========================================================================
Copyright (c) 2012 Qualcomm Technologies, Incorporated.  All rights reserved.
Qualcomm Confidential and Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/ultrasound.adsp/2.7/services/dynamic_svcs/ultrasound_proc_svc/inc/UltrasoundProcSvc.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
06/17/12   NR      Initial version
========================================================================== */
#ifndef ELITE_ULTRASOUND_PROCSVC_H
#define ELITE_ULTRASOUND_PROCSVC_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/

/* System */
#include "qurt_elite.h"

/* Ultrasound and Audio */
#include "Elite.h"
#include "UltrasoundStreamMgr.h"
#include "adsp_usm_stream_commands.h"

/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */

/**< @briefID for internal SET PARAM command for encoder config param */
#define ELITEMSG_SET_PARAM_ULTRASOUND_PROC_CFG     0x00170001

/**< @brief Data structure for encoder config param */
typedef struct 
{
    qurt_elite_queue_t *buffer_return_q_ptr;  /**< This is the queue to which this
                                         payload buffer needs to be returned */
    qurt_elite_queue_t *response_q_ptr;      /**< This is the queue to send the ACK,
                                        NULL indicates response isn't required */
    uint32_t      client_token;         /**< Token that should be given
                                        in the ACK */
    uint32_t       respons_result;      /**< This is to be filled with ACK
                                        results by the client */
    uint32_t       param_id;            /**< This is the parameter ID. The rest
                                        of payload is derived based on this ID. */
    int32_t       *param_data_ptr;      /**< Pointer to parameter value that
                                        are to be get/set*/
} elite_msg_ultrasound_proc_param_cfg_t; 

/**
  Creates an instance of the Elite Ultrasound processor service.

  @param [in] inputParam   Pointer to 
                           ultrasound_proc_svc_init_params_t,
                           expressed as a uint32_t parameter.
  @param [out] handle      Service entry handle returned to the caller.

  @return
  Success or failure of the instance creation.

  @dependencies
  None.
*/

ADSPResult ultrasound_proc_svc_create (uint32_t inputParam, void **handle);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef ELITE_ULTRASOUND_PROCSVC_H

