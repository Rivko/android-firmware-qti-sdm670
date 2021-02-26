/** @file ImageVersionLibNull.c
  
  Stub functions for ImageVersionLib

  Copyright (c) 2014, Qualcomm Technologies, Inc. All rights reserved.
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 05/27/15   ck      Updated image_uuid to image_version_uuid_init
 05/30/14   kedara  Initial revision

=============================================================================*/


/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "image_version.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

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
uint32 image_version_populate_version
(
  image_index_type image_index
)
{
  return 0;
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
  return 0;
}

/*===========================================================================

**  Function :  image_version_uuid_init

** ==========================================================================
*/
/*!
* 
* @brief
*   This function populates the UUID in smem   
* 
* 
* @par Dependencies
*   smem must have been initialized.
*   
* @retval
*   None                                                              
* 
* @par Side Effects
*   None
* 
*/
void image_version_uuid_init(void)
{
}
