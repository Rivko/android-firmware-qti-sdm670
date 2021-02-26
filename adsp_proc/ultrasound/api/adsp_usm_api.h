/*========================================================================*/
/**
@file adsp_usm_api.h

@brief This file contains all other USM header files covering service, stream,
session, data commands and events. Users need only include this file
for USM API.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The Ultrasound_mainpage.dox file contains all file/group 
      descriptions that are in the output PDF generated using Doxygen and 
      Latex. To edit or update any of the file/group text in the PDF, edit 
      the Ultrasound_mainpage.dox file or contact Tech Pubs.
===========================================================================*/

/*===========================================================================
  Copyright (c) 2010-2013 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*========================================================================
Edit History

$Header: //components/rel/ultrasound.adsp/2.7/api/adsp_usm_api.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
01/01/13   NR      Add support for family - B memory APIs  
10/06/12   sw      (Tech Pubs) Updated legal statements (lines 20 & 22) for QTI;
                   updated Doxygen markup for D.2 templates.
02/14/12   sw      (Tech Pubs) Edited Doxygen markup for Rev B of the IS.
10/18/11   sw      (Tech Pubs) Edited Doxygen markup/comments for new Interface Spec.
10/13/10   NR      File created

========================================================================== */

#ifndef _ADSP_USM_API_H_
#define _ADSP_USM_API_H_

/** @ingroup versions
    Version of the aDSP USM service.
    
    API_developer_note: 1,2,...,6 are for A family. 7 onwards
    are for B family.
    @newpage
*/
#define ADSP_USM_VERSION    0x70000000

#include "adsp_usm_service_commands.h"
#include "adsp_usm_stream_commands.h"
#include "adsp_usm_session_commands.h"
#include "adsp_usm_data_commands.h"

#endif /*_ADSP_USM_API_H_*/
