/**
@file image_uuid.c
@brief This file contains the API for the Version API 1.0 service.
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2014 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/version/src/image_uuid.c#1 $
$DateTime: 2018/07/30 01:21:36 $
$Author: pwbldsvc $
===============================================================================*/

#include "comdef.h"
#include "smem.h"
#include "string.h"
#include "stringl/stringl.h"
#include "image_version.h"

/*=============================================================================

                              FUNCTION DEFINITIONS

=============================================================================*/

// Trace32 Accessible Instrumentation
//
// image_internal_uuid
//
// NULL  : Routine Has Not Run
// !NULL : Contents of OEM_IMAGE_UUID

char version_internal_uuid[SMEM_IMAGE_VERSION_ENTRY_SIZE];

void image_uuid(void)
{

   secure_memset((void*)version_internal_uuid, NULL, sizeof(version_internal_uuid));

   strlcpy(version_internal_uuid, OEM_IMAGE_UUID_STRING_AUTO_UPDATED, sizeof(version_internal_uuid));
}
