/**
@file AdspCoreSvc.h
@brief This file declares AdspCoreSvc.***********************

This is Adsp Core Service. This service receives commands from***
SC and queues them. The commands are picked up from the queue****
and processed sequentially. Commands include getting API*********
versions, mapping and unamapping regions etc********************
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The Elite_Core_mainpage.dox file contains all file/group descriptions that
      are in the output PDF generated using Doxygen and Latex. To edit or update
      any of the file/group text in the PDF, edit the Elite_Core_mainpage.dox
      file or contact Tech Pubs.

      The above description for this file is part of the "core_service" group
      description in the Elite_Core_mainpage.dox file.
===========================================================================*/

/*========================================================================
Edit History

when       who     what, where, why
--------   ---     -------------------------------------------------------
10/28/10   sw      (Tech Pubs) Edited/added Doxygen comments and markup.
05/20/10    KK       Created file.
==========================================================================*/

/*-----------------------------------------------------------------------
   Copyright (c) 2010 Qualcomm Technologies, Incorporated.  All rights reserved.
   Qualcomm Confidential and Proprietary.
-----------------------------------------------------------------------*/

#ifndef ADSP_CORE_SVC_H
#define ADSP_CORE_SVC_H

/*-------------------------------------------------------------------------
Include Files
-------------------------------------------------------------------------*/

/* System */
#include "qurt_elite.h"

/* Audio */
#include "Elite.h"

/* ADSP error codes */
#include "adsp_error_codes.h"

#include "adsp_prv_avcs_api.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/** @addtogroup core_service
@{ */

/* -----------------------------------------------------------------------
** Global definitions/forward declarations
** ----------------------------------------------------------------------- */

/** Total number of static services in the system. */
#define NUMBER_OF_STATIC_SERVICES 9


/**
  Creates an instance of the Core service.

  @param[in] inputParam   Any value or pointer to the structure required by
                          the service to help set up the instance.
  @param[out] svcHandle   Service entry handle returned to the caller.

  @return
  Success/failure of instance creation.

  @dependencies
  None.
*/
ADSPResult AdspCoreSvc_Create(uint32_t inputParam, void **svcHandle);

/** @} */ /* end_addtogroup core_service */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //ADSP_CORE_SVC_H

