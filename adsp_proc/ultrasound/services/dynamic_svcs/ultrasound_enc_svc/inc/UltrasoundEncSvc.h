/**
@file UltrasoundEncSvc.h

@brief Ultrasound encdoder service public header.

This file contains Ultrasound encoder service belonging to the 
Elite framework. 
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The elite_ultrasound_mainpage.dox file contains all file/group descriptions
      that are in the output PDF generated using Doxygen and Latex. To edit or
      update any of the file/group text in the PDF, edit the
      elite_ultrasound_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the "ultrasound_stream" group
      description in the elite_ultrasound_mainpage.dox file.
===========================================================================*/

/*========================================================================
Edit History

$Header:

when       who     what, where, why
--------   ---     -------------------------------------------------------
10/24/10   NR      File created 
01/04/12   LB      Added support for signal detection mode 
07/22/13   NR      Ultrasound encoder re-architecture 
==========================================================================*/

/*-----------------------------------------------------------------------
   Copyright (c) 2010 - 2013 Qualcomm Technologies, Incorporated.  All rights reserved.
   Qualcomm Confidential and Proprietary.
-----------------------------------------------------------------------*/

#ifndef ELITE_ULTRASOUND_ENCODERSVC_H
#define ELITE_ULTRASOUND_ENCODERSVC_H

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

/**< @brief Data structure for initializing encoder service on open command */
typedef struct
{
    uint32_t                config_id;          /**< Media format ID of the
                                                encoder to be created */
    usm_callback_handle_t   *callback_data_ptr; /**< USM callback handle
                                                for event generation/ack */
} ultrasound_enc_svc_init_params_t;

/**< @briefID for internal SET PARAM command for encoder config param */
#define ELITEMSG_CUSTOM_USM_SET_PARAM_ULTRASOUND_ENC_CFG     0x00170001

/**< @brief Data structure for encoder config param */
typedef struct 
{
    elite_msg_custom_header_t custom_header; /**< Common fields in any 
                                                  #ELITE_CUSTOM_MSG message payload.*/
    int32_t       *param_data_ptr;      /**< Pointer to parameter value that
                                        are to be get/set*/
} elite_msg_ultrasound_enc_param_cfg_t; 

/**
  Creates an instance of the Elite Ultrasound encoder service.

  @param [in] inputParam   Pointer to ultrasound_enc_svc_init_params_t, expressed
                           as a uint32_t parameter.
  @param [out] handle      Service entry handle returned to the caller.

  @return
  Success or failure of the instance creation.

  @dependencies
  None.
*/

ADSPResult ultrasound_enc_svc_create (uint32_t inputParam, void **handle);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef ELITE_ULTRASOUND_ENCODERSVC_H

