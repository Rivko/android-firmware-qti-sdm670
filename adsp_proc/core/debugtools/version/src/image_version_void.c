/*===========================================================================

GENERAL DESCRIPTION
  This file contains functions and other defines used for
  image version reporting feature.

Copyright 2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1/debugtools/version/src/image_version_void.c#1 $

=============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/

#include "comdef.h"
#include "smem.h"
#include "string.h"
#include "image_version.h"

/*=============================================================================

                              FUNCTION DEFINITIONS

=============================================================================*/

/*===========================================================================

**  Function :  image_version_populate_version

** ==========================================================================
*/
/*!
*
* @brief
*   This function populates the image_version in image_versoin_table in smem
*   based on image index/allocated-entry.
*
* @param[in] image_index index in the smem_image_version_table
*
* @par Dependencies
*   smem must have been initialized.
*
* @retval
*   returns status: 0 value represents no error, for other errors review
*   image_version_error_type
*
* @par Side Effects
*   None
*
*/
uint32 image_version_populate_version(image_index_type image_index)
{
   return IMG_VER_ERROR_NONE;
}

/*===========================================================================

**  Function :  image_version_initialize_version_table

** ==========================================================================
*/
/*!
*
* @brief
*   This function initializes the image_version_table in smem with spaces.
*
* @param
*   None
*
* @par Dependencies
*   smem must have been initialized and smem element for image_version_table
*   allocated.
*
* @retval
*   returns status: 0 value represents no error, for other errors review
*   image_version_error_type
*
* @par Side Effects
*   Initializes whole SMEM_IMAGE_VERSION_TABLE with spaces
*   This API should only be called once right after smem_boot_init by the image
*   doing smem_boot_init.
*
*/
uint32 image_version_initialize_version_table(void)
{
   return IMG_VER_ERROR_NONE;
}
