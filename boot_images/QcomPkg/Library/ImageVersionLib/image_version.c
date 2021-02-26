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

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/ImageVersionLib/image_version.c#1 $

=============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/

#include "comdef.h"
#include "smem.h"
#include "string.h"
#include "stringl/stringl.h"
#include "image_version.h"

// Trace32 Accessible Instrumentation
//
// image_version_table_p
//
// NULL  : Routine Has Not Run
// NULL  : Routine Was Unable to Obtain SMEM Block
// !NULL : Pointer to base area of image version entries

static uint32* image_version_table_p;

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
   char* ptr;
   uint32 index;
   uint32 status;
   uint32 smem_block_size;
   struct image_version_entry version;
   struct image_version_entry* version_p;

   // default status return value indicates error
   status = IMG_VER_ERROR;

   // assign value to static variable which is accessible to trace32
   image_version_table_p = (uint32*)smem_get_addr(SMEM_IMAGE_VERSION_TABLE, &smem_block_size);

   do
   {
      // block from smem must be allocated
      if (NULL == image_version_table_p)
      {
         status = IMG_VER_ERROR_TABLE_NOT_INITIALIZED;
         break;
      }

      // block from smem must be correct size
      if (SMEM_IMAGE_VERSION_TABLE_SIZE != smem_block_size)
         {
         status = IMG_VER_ERROR_TABLE_SIZE_WRONG;           // return result
            break;
         }

      // index must not overflow bounds
      if (IMAGE_INDEX_MAX <= image_index)
         {
         status = IMG_VER_ERROR_OUT_OF_BOUND_INDEX;         // return result
            break;
         }

      // entry structure must be exact size (primarily double checks compilation)
      if (SMEM_IMAGE_VERSION_ENTRY_SIZE != sizeof(struct image_version_entry))
         {
         status = IMG_VER_ERROR_SMEM_STRUCT_SIZE_MISMATCH;  // return result
            break;
         }

      // prepare structure being filled
      memset((void*)&version, 0, sizeof(version));

      // required format: "value", NULL terminated string
      // recommended optional format: "QC_IMAGE_VERSION_STRING_AUTO_UPDATED=value"
      // used for command line image identification (ie, 'strings -a output.mbn | grep QC_IMAGE_VERSION_STRING_AUTO_UPDATED')

      ptr = strchr(QC_IMAGE_VERSION_STRING_AUTO_UPDATED, EQUAL_SEP);    // build process may assign value following EQUAL_SEP

      if (NULL != ptr)
      {
         strlcpy(version.image_qc_version_string, ptr + 1, sizeof(version.image_qc_version_string));
      }
      else
      {
         strlcpy(version.image_qc_version_string, QC_IMAGE_VERSION_STRING_AUTO_UPDATED, sizeof(version.image_qc_version_string));
      }

      // required format: "value", NULL terminated string
      // recommended optional format: "IMAGE_VARIANT_STRING_AUTO_UPDATED=value"
      // used for command line image identification (ie, 'strings -a output.mbn | grep IMAGE_VARIANT_STRING_AUTO_UPDATED')

      ptr = strchr(IMAGE_VARIANT_STRING_AUTO_UPDATED, EQUAL_SEP);       // build process may assign value following EQUAL_SEP

      if (NULL != ptr)
      {
         strlcpy(version.image_variant_string, ptr + 1, sizeof(version.image_variant_string));
      }
      else
      {
         strlcpy(version.image_variant_string, IMAGE_VARIANT_STRING_AUTO_UPDATED, sizeof(version.image_variant_string));
      }

      // required format: "value", NULL terminated string
      // recommended optional format: "OEM_IMAGE_VERSION_STRING_AUTO_UPDATED=value"
      // used for command line image identification (ie, 'strings -a output.mbn | grep OEM_IMAGE_VERSION_STRING_AUTO_UPDATED')

      ptr = strchr(OEM_IMAGE_VERSION_STRING_AUTO_UPDATED, EQUAL_SEP);   // build process may assign value following EQUAL_SEP

      if (NULL != ptr)
      {
         strlcpy(version.image_oem_version_string, ptr + 1, sizeof(version.image_oem_version_string));
      }
      else
      {
         strlcpy(version.image_oem_version_string, OEM_IMAGE_VERSION_STRING_AUTO_UPDATED, sizeof(version.image_oem_version_string));
      }

      // get the base pointer

      version_p = (struct image_version_entry*)((uintnt)image_version_table_p + (SMEM_IMAGE_VERSION_ENTRY_SIZE * (uint32)image_index));

      // set the remaining fields

      index = (uint32)image_index;

      version.image_index[1] = ZERO_SEP + (index % 10);                 // 00 to IMAGE_INDEX_MAX (ie, 32)
      index = index / 10;
      version.image_index[0] = ZERO_SEP + (index % 10);

      version.image_colon_sep1[0] = COLON_SEP;                          // add required seperator

      version.image_colon_sep2[0] = COLON_SEP;                          // add required seperator

      *version_p = version;                                             // 'memcpy' to smem

      status = IMG_VER_ERROR_NONE;                                      // return result

   }while(0);

   return status;
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
   uint32 status = IMG_VER_ERROR_SMEM_ALLOC_FAILED;

   /* allocate and retrieve pointer for SMEM_IMAGE_VERSION_TABLE */
   uint32* table_ptr = smem_alloc(SMEM_IMAGE_VERSION_TABLE, SMEM_IMAGE_VERSION_TABLE_SIZE);

   if (NULL != table_ptr)
   {
      /* initialize image_version_table to a block of NULL */
      memset((void*)table_ptr, 0, SMEM_IMAGE_VERSION_TABLE_SIZE);
      status = IMG_VER_ERROR_NONE;
   }

   return status;
}
