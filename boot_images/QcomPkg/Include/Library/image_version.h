#ifndef IMAGE_VERSION_H
#define IMAGE_VERSION_H

/*===========================================================================

GENERAL DESCRIPTION
  This file contains declaration of functions and other defines used for
  image version reporting feature.

Copyright 2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.xf/2.1/QcomPkg/Include/Library/image_version.h#1 $

============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "comdef.h" // client use requires inclusion of comdef.h

typedef enum
{
   IMG_VER_ERROR_NONE = 0,
   IMG_VER_ERROR,
   IMG_VER_ERROR_SMEM_ALLOC_FAILED,
   IMG_VER_ERROR_TABLE_NOT_INITIALIZED,
   IMG_VER_ERROR_TABLE_SIZE_WRONG,
   IMG_VER_ERROR_OUT_OF_BOUND_INDEX,
   IMG_VER_ERROR_SMEM_STRUCT_SIZE_MISMATCH,
   IMG_VER_ERROR_ENUM_SIZE = 0x7FFFFFFF

} img_ver_error_type;

typedef enum
{
   IMAGE_INDEX_SBL = 0,
   IMAGE_INDEX_TZ = 1,
   IMAGE_INDEX_TZSECAPP = 2,
   IMAGE_INDEX_RPM = 3,
   IMAGE_INDEX_SDI = 4,
   IMAGE_INDEX_HYPERVISOR = 5,
   IMAGE_INDEX_RESERVED6 = 6,
   IMAGE_INDEX_RESERVED7 = 7,
   IMAGE_INDEX_RESERVED8 = 8,
   IMAGE_INDEX_APPSBL = 9,
   IMAGE_INDEX_APPS = 10,
   IMAGE_INDEX_MPSS = 11,
   IMAGE_INDEX_ADSP = 12,
   IMAGE_INDEX_WCNS = 13,
   IMAGE_INDEX_VENUS = 14,
   IMAGE_INDEX_RESERVED15 = 15,
   IMAGE_INDEX_RESERVED16 = 16,
   IMAGE_INDEX_RESERVED17 = 17,
   IMAGE_INDEX_RESERVED18 = 18,
   IMAGE_INDEX_RESERVED19 = 19,
   IMAGE_INDEX_RESERVED20 = 20,
   IMAGE_INDEX_RESERVED21 = 21,
   IMAGE_INDEX_RESERVED22 = 22,
   IMAGE_INDEX_RESERVED23 = 23,
   IMAGE_INDEX_RESERVED24 = 24,
   IMAGE_INDEX_RESERVED25 = 25,
   IMAGE_INDEX_RESERVED26 = 26,
   IMAGE_INDEX_RESERVED27 = 27,
   IMAGE_INDEX_RESERVED28 = 28,
   IMAGE_INDEX_RESERVED29 = 29,
   IMAGE_INDEX_RESERVED30 = 30,
   IMAGE_INDEX_RESERVED31 = 31,
   IMAGE_INDEX_MAX = 32,
   IMAGE_INDEX_ENUM_SIZE = 99 // facilitate compiler optimization

} image_index_type;

#define SMEM_IMAGE_VERSION_TABLE_SIZE		4096
#define SMEM_IMAGE_VERSION_ENTRY_SIZE		128

#define IMAGE_INDEX_LENGTH                2
#define IMAGE_SEP1_LENGTH                 1
#define IMAGE_QC_VERSION_STRING_LENGTH		72
#define IMAGE_VARIANT_STRING_LENGTH       20
#define IMAGE_SEP2_LENGTH                 1
#define IMAGE_OEM_VERSION_STRING_LENGTH   32

// sizeof(struct image_version_entry) == 128

struct image_version_entry
{
   char image_index[IMAGE_INDEX_LENGTH];
   char image_colon_sep1[IMAGE_SEP1_LENGTH];
   char image_qc_version_string[IMAGE_QC_VERSION_STRING_LENGTH];
   char image_variant_string[IMAGE_VARIANT_STRING_LENGTH];
   char image_colon_sep2[IMAGE_SEP2_LENGTH];
   char image_oem_version_string[IMAGE_OEM_VERSION_STRING_LENGTH];
};

// convenience manifest constants to facilitate locations for known seperators
// and intentional space padding of fields if desired

#define ZERO_SEP                          '0'
#define SPACE_SEP                         ' '
#define COLON_SEP                         ':'
#define EQUAL_SEP                         '='

#define SPACE_PAD_STRING                  "                                                                " \
                                          "                                                                "

// automatic updated character string arrays used in this implementation

extern char QC_IMAGE_VERSION_STRING_AUTO_UPDATED[];
extern char IMAGE_VARIANT_STRING_AUTO_UPDATED[];
extern char OEM_IMAGE_VERSION_STRING_AUTO_UPDATED[];
extern char OEM_IMAGE_UUID_STRING_AUTO_UPDATED[];

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
);

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
uint32 image_version_initialize_version_table
(
  void
);

/*===========================================================================

**  Function :  image_version_init

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
*   None
*
* @par Side Effects
*   This API should only be called once right after smem_boot_init by the image
*   doing smem_boot_init.
*
*/
void image_version_init(void);

void image_version_uuid_init(void);

#endif
