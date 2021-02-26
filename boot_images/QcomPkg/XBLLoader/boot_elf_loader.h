#ifndef BOOT_ELF_LOADER_H
#define BOOT_ELF_LOADER_H

/*=============================================================================

                              Boot Loader

GENERAL DESCRIPTION
  This module performs binary image and AMSS loading.

Copyright 2010-2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
=============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/10/17   kpa     Initial revision
============================================================================*/

/*===========================================================================

                           INCLUDE FILES
						   
===========================================================================*/
#include "MIheader.h"
#include "boot_comdef.h"
#include "boot_extern_sec_img_interface.h"
#include "boot_whitelist_prot.h"

/*=============================================================================

                              DEFINITIONS

=============================================================================*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
**  Function :  boot_populate_sbl_info
** ==========================================================================
*/
/*!
* 
* @brief
*   This function loads elf image info
*
* @par Dependencies
*   None 
* 
* @retval
*   SEC_IMG_AUTH_SUCCESS - If ELF images is verified successfully
*   ERROR VALUES - In case of failure.
* 
*/
uint32 boot_populate_sbl_info 
(
  sec_img_auth_elf_info_type *sbl_elf_info,
  sec_img_auth_whitelist_area_param_t *sbl_white_list_param,
  whitelst_tbl_entry_type * config_entry_whitelist_table
);

/*===========================================================================
**  Function :  boot_load_elf_image
** ==========================================================================
*/
/*!
* 
* @brief
*   This function will load an ELF formatted image from the requested 
*   partition_id. The QC header located at the beginning of the hash segment
*   will also be loaded into the location pointed to by header_ptr
*
* @param[in] 
*  elf_segment_whitelist - Pointer to the whitelist table specifying segment
*                          regions to load.  NULL means all segments are loaded.
*
*  image_id              - ID of the image to be loaded
*
* @par Dependencies
*   BOOT_FEATURE_LOAD_ELF must be defined 
* 
* @retval
*   None
* 
*/
void boot_load_elf_image( 
  whitelst_tbl_entry_type * elf_segment_whitelist,
  uint32 image_id,
  boot_boolean auth
);

/*===========================================================================
**  Function :  boot_elf_verify_elf_hdr
** ==========================================================================
*/
/*!
* 
* @brief
*   This function verifies hash of elf_hdr with an expected
*   hash value. If ehdr+phdr hash verification fails then this function  
*   doesn't return and enters boot_error_handler if hash check fails.
*   
*
* @par Dependencies
*   first hash in the hash table always represents hash of ehdr+phdr 
* 
* @retval
*   None
* 
*/
void boot_elf_verify_elf_hdr
(
  void
);


/*===========================================================================
**  Function :  boot_elf_verify_image_header
** ==========================================================================
*/
/*!
* 
* @brief
*   This function validates the elf header in the partition 
*
* @param[in] partition_id - Pointer to the partition ID information
* @param[int] header_ptr  - Pointer to the QC header buffer that must be cleared
*
* @par Dependencies
*   None 
* 
* @retval
*   Boolean TRUE is valid header, FALSE otherwise
* 
*/
boolean boot_elf_verify_image_header
(   
 uint8 * partition_id,
 mi_boot_image_header_type * header_ptr
);

/*===========================================================================
**  Function :  boot_elf_load_init_hash_segment
** ==========================================================================
*/
/*!
* 
* @brief
*   This function initializes all global variables within the module which are
*   necessary to perform ELF loading. This function will also load the 
*   hash data segment into memory, read in the QC header, and assign 
*   hash_segment_start to point to the start of the hash entries. 
*
* @param[in] partition_id - Pointer to the partition ID information
* @param[out] header_ptr - Pointer to the QC header that is read from the image
*
* @par Dependencies
*   None 
* 
* @retval
*   None
* 
*/
void boot_elf_load_init_hash_segment
(
 uint8 * partition_id, 
 mi_boot_image_header_type *header_ptr
);


/*===========================================================================
**  Function :  boot_elf_ziinit_structures
** ==========================================================================
*/
/*!
* 
* @brief
*   This function zero-initializes all global variables within the module which
*   are necessary to perform ELF loading.  
*
* @param[out] header_ptr - Pointer to the QC header that is read from the image
*
* @par Dependencies
*   None 
* 
* @retval
*   None
* 
*/
void boot_elf_ziinit_structures(mi_boot_image_header_type *header_ptr);


/*===========================================================================
**  Function :  boot_elf_load_deinit
** ==========================================================================
*/
/*!
* 
* @brief
*   This function deinitializes the necessary variables when ELF loading is 
*   complete. The flash translation interface is closed, and reset to NULL
*   so that it may be re-used again by the module for a new target image. 
*
* @par Dependencies
*   None 
* 
*/
void boot_elf_load_deinit(void);



/*===========================================================================
**  Function :  boot_get_elf_entry_address
** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns the value of the elf header entry pointer. 
*
* @par Dependencies
*   None 
* 
*/
uint64 boot_get_elf_entry_address
( void );


/*===========================================================================
**  Function :  boot_set_elf_and_prog_header
** ==========================================================================
*/
/*!
* 
* @brief
*  This function sets the global elf header and program header variables 
*  with provided data. 
*
* @param[in]
*   elf_hdr_ptr - Pointer to elf header
*
* @param[in]
*   prog_hdr_ptr - Pointer to Program header
*
* @par Dependencies
*   Elf header global struct is assumed to be zero initialized before
*  invoking the api. 
* 
* @retval
*   Boolean TRUE if success, FALSE otherwise
*/
boolean boot_set_elf_and_prog_header(
  void *elf_hdr_ptr,
  void *prog_hdr_ptr
);

/*===========================================================================
**  Function :  boot_get_elf_eident_class
** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns the value of the elf e_ident class value.
*   1 = 32 bit elf, 2 = 64 bit elf according to ELF spec.
*
* @par Dependencies
*   None 
* 
*/
uint32 boot_get_elf_eident_class
( void );


/*===========================================================================
**  Function :  boot_get_elf_header
** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns the address of elf header. 
*
* @par Dependencies
*   None 
* 
*/
uint64 boot_get_elf_header
( void );

/*===========================================================================
**  Function :  boot_get_program_header
** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns the address of elf header. 
*
* @par Dependencies
*   None 
* 
*/
uint64 boot_get_program_header
( void );

/*===========================================================================
**  Function :  boot_elf_set_auth_and_ssa_enable
** ==========================================================================
*/
/*!
* 
* @brief
*   This function will set/reset Flag to Turn on/Turn off authentication and
*   Subsystem image loading mode for elf images.
*
* @param[in] 
*  boolean auth_from_cfg_table   - TRUE if config table entry for the image
*                                  enables authentication else FALSE.
*  boolean ssa_value             - TRUE if Subsystem image loading mode to be
*                                  turned on else FALSE
*
* @param[out] 
*  None
*
* @par Dependencies
*  None
* 
* @retval
*   None
* 
*/
void boot_elf_set_auth_and_ssa_enable
( 
  boolean auth_from_cfg_table,
  boolean ssa_value
);

/*===========================================================================
**  Function :  boot_elf_segment_in_whitelist
** ==========================================================================
*/
/*!
* 
* @brief
*   This function will set/reset Flag to Turn on/Turn off authentication and
*   Subsystem image loading mode for elf images.
*
* @param[in] 
*  boolean auth_from_cfg_table   - TRUE if config table entry for the image
*                                  enables authentication else FALSE.
*  boolean ssa_value             - TRUE if Subsystem image loading mode to be
*                                  turned on else FALSE
*
* @param[out] 
*  None
*
* @par Dependencies
*  None
* 
* @retval
*   None
* 
*/
boolean
boot_elf_segment_in_whitelist(void * phdr_entry_ptr,
                              whitelst_tbl_entry_type * elf_segment_whitelist);


/*===========================================================================
**  Function :  boot_get_elf_hash_segment_size
** ==========================================================================
*/
/*!
* 
* @brief
*   This function parses the input program header table to locate hash segment
*   and returns its size in memory.
*
* @param[in] 
*  void *elf_hdr                - Pointer to elf header
*  void *prog_hdr               - Pointer to Program header table
*
*
* @param[out] 
*  uintnt *hash_segment_size      - Size of hash segment if found
*
* @par Dependencies
*  It is assumed the user shall be validating and passing correct
*  elf header and program header table.
* 
* @retval
*   boolean     TRUE if hash segment found, else FALSE
* 
*/
boolean boot_get_elf_hash_segment_size
(
  void *elf_hdr,
  void *prog_hdr,
  uintnt *hash_segment_size
);


/*===========================================================================
**  Function :  boot_get_elf_prog_header_count
** ==========================================================================
*/
/*!
* 
* @brief
*   This function parses the input elf header table to get
*   program header table entry count
*
* @param[in] 
*  void *elf_hdr                - Pointer to elf header
*
*
* @param[out] 
*  uintnt *max_phnum            - Pointer to program header entry count
*
* @par Dependencies
*  It is assumed the user shall be validating and passing correct
*  elf header.
* 
* @retval
*   boolean     TRUE if hash segment found, else FALSE
* 
*/
boolean boot_get_elf_prog_header_count
(
  void *elf_hdr,
  uintnt *max_phnum
);


/*===========================================================================
**  Function :  boot_get_elf_phdr_entry_info
** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns segment details for requested prog header table entry
*
* @param[in] 
*  void *elf_hdr              - Pointer to elf header
*  void *prog_hdr             - Pointer to Program header table
*  uintnt index               - Index of the entry whose details are requested
*
*
* @param[out] 
*  uintnt *seg_file_size_ptr      - Segment file size
*  boolean *is_seg_loadable_ptr   - Flag indicating if the segment is loadable
*  uintnt *file_offset            - offset of segment data on storage
*  uintnt *physical_addr          - destination physical address of segment
*
* @par Dependencies
*  It is assumed the user shall be validating and passing correct
*  elf header.
* 
* @retval
*   boolean     TRUE if hash segment found, else FALSE
* 
*/
boolean boot_get_elf_phdr_entry_info
(
  void *elf_hdr,
  void *prog_hdr,
  uintnt index,
  uintnt *seg_file_size_ptr,
  boolean *is_seg_loadable_ptr,
  uintnt *file_offset,
  uintnt *physical_addr
);

/*===========================================================================
**  Function :  boot_get_elf_segment_data
** ==========================================================================
*/
/*!
* 
* @brief
*   This function loads data from storage
* 
* @param[in] 
*  uintnt file_offset          - Start of data to be loaded in partition from
*                                storage device
*  uintnt file_size            - Size of data to be loaded.
*
* @param[out] 
*   void *buffer              - pointer to buffer, loaded with data
* 
*       
* @par Dependencies
*  Partition is assumed to be opened for image loading with flash lib data struct
*  configured
*   
* @retval
*   xblconfig_status_type.   XBLCONFIG_ERR_NONE if no error else error value
* 
* @par Side Effects
*   None
* 
*/
boolean boot_get_elf_segment_data
(
  uintnt file_offset,
  uintnt file_size,
  void *buffer
);

#endif /* BOOT_ELF_LOADER_H */

