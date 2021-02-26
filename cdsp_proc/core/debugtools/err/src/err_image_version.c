/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         E R R O R     R E P O R T I N G    S E R V I C E S

GENERAL DESCRIPTION
  This module updates image versioning info to coredump

Copyright (c) 2015 - 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/err/src/err_image_version.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/19/14   din     Adapted from err.c

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "erri.h"
#include "stringl.h"

/*===========================================================================

                 Defines and variable declarations for module

===========================================================================*/
/* Image Versioning data for coredump */
extern char QC_IMAGE_VERSION_STRING_AUTO_UPDATED[];
extern char IMAGE_VARIANT_STRING_AUTO_UPDATED[];
extern char OEM_IMAGE_VERSION_STRING_AUTO_UPDATED[];
extern char OEM_IMAGE_UUID_STRING_AUTO_UPDATED[];

extern coredump_type coredump;

/*===========================================================================

                              Function definitions

===========================================================================*/

/*===========================================================================

FUNCTION ERR_UPDATE_IMAGE_VERSIONING_INFO

DESCRIPTION
  Updates Image versioning info to coresump

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void err_update_image_versioning_info (void)
{
  strlcpy(coredump.image.qc_image_version_string, 
          QC_IMAGE_VERSION_STRING_AUTO_UPDATED, ERR_IMAGE_VERSION_SIZE); 
  strlcpy(coredump.image.image_variant_string, 
          IMAGE_VARIANT_STRING_AUTO_UPDATED, ERR_IMAGE_VERSION_SIZE);
  strlcpy(coredump.image.oem_image_version_string, 
          OEM_IMAGE_VERSION_STRING_AUTO_UPDATED,ERR_IMAGE_VERSION_SIZE);
  /* strlcpy(coredump.image.oem_image_uuid_string,
          OEM_IMAGE_UUID_STRING_AUTO_UPDATED, ERR_IMAGE_VERSION_SIZE);  */
  

} /* err_update_image_versioning_info */


