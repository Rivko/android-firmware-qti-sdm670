/*===========================================================================

GENERAL DESCRIPTION
  This file contains functions and other defines used for
  image version reporting feature.

Copyright 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ImageVersionLib/image_uuid.c#1 $

=============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/

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

void image_version_uuid_init(void)
{
   memset((void*)version_internal_uuid, 0, sizeof(version_internal_uuid));

   strlcpy(version_internal_uuid, OEM_IMAGE_UUID_STRING_AUTO_UPDATED , sizeof(version_internal_uuid));
}
