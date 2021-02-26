/*========================================================================*/
/**<
@file UltrasoundDecSvc.h

@brief Decoder service public header.

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
Copyright (c) 2011 Qualcomm Technologies, Incorporated.  All rights reserved.
Qualcomm Confidential and Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/ultrasound.adsp/2.7/services/dynamic_svcs/ultrasound_dec_svc/inc/UltrasoundDecSvc.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
03/15/11   NR      Created file.
========================================================================== */


#ifndef ULTRASOUNDDECODERSVC_H
#define ULTRASOUNDDECODERSVC_H

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

/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */

/**
  Creates an instance of the Elite Ultrasound Decoder service.

  @param [in] inputParam   Pointer to ultrasound_dec_svc_init_params_t, expressed
                           as a uint32_t parameter.
  @param [out] handle      Service entry handle returned to the caller.

  @return
  Success or failure of the instance creation.

  @dependencies
  None.
*/
ADSPResult ultrasound_dec_svc_create (uint32_t inputParam, void **handle);

/** 
  @brief Data structure for initializing the Ultrasound Decoder service on an open
  command.
*/
typedef struct
{
    uint32_t                format_id;          /**< Media format ID of the
                                                decoder to be created. */
    usm_callback_handle_t   *callback_data_ptr; /**< USM callback
                                                    handle for event generation/ack. */
} ultrasound_dec_svc_init_params_t;

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef ULTRASOUNDDECODERSVC_H

