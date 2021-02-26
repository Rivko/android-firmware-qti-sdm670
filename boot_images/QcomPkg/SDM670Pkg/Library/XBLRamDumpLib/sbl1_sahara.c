/*=============================================================================

                        Secondary Boot Loader Sahara
                              Source File
GENERAL DESCRIPTION
  This file provides the implementation to enter the Sahara Protocol.

Copyright 2011 - 2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/11/17   aus     Close flash partition in unrecoverable error handler to support USB PIPO
09/22/17   rohik   Added support for saving the logdump partition with PC dump
08/10/17   yps     Clean heap region
05/02/17   yps     add dump reenter in unrecover hander without reset
03/03/17   ds	   remove clobber prot
07/05/16   kpa     Update for sdm845 hwio
03/09/16   kpa     use pshold instead of secure wdog to reset since we are in EL1
08/06/15   as      Use shared functions
08/01/15   kpa     use dcache_flush_region instead of mmu_flush_cache
07/09/15   kpa     Update secure wdog reset logic for pimem dumps.
07/08/15   kpa     Support secure watchdog bite to preserve pimem dump
01/05/14   kpa     Remove unsupported api's for fetching info from mbn header.
11/20/14   ck      Refactored USB to use new agnostic USB extern interface
08/08/14   ck      Added 64 bit support
07/09/14   tj      Fixes for bounds check on hash buffer during compute hash offsets.
06/20/14   kedara  Support for llvm compilation.
05/07/14   kedara  Fix compiler warnings.
03/25/14   jz      Ported sbl_elf_optimized_segment_load stub for MSM's
03/18/14   ck      Replaced defunct boot_hw_warm_reset with call to boot_hw_reset
03/18/14   ck      Updated boot_hw_reset calls as they now take a reset type parameter
02/04/14   ck      Removed exit_ptr as it's not needed in Bear family.
11/12/13   kedara  Added sbl_sahara_skip_image_load as placeholder.
11/07/13   sve	   updated sbl_sahara_unrecoverable_error_handler to fix sbl1_error_handler
10/18/13   dhaval  zero initialize global buffers before starting elf download
10/15/13   kedara  Update dispatch table. Add NULL ptr for unused image load api's.
09/13/13   aus     Added changes to support SBL crash dumps in flashless boot
09/03/13   aus     Added support for clearing all downloaded data on auth failure
08/02/13   aus     Do not enumerate in mass storage mode if ram dump is not allowed by TZ
07/24/13   aus     Do a warm reset in unrecoverable error handler
06/24/13   aus     Skip loading EFS images if header is corrupted
06/18/13   sl      Added bulk mode type to boot_qhsusb_al_bulk_init()
06/11/13   kedara  Move image loading data structs to uncached ddr region.
06/05/13   aus     Clear the whitelist table of the image in case of authentication failures
05/31/13   kedara  Autheticate MBA as regular elf for non-MSA boot, 
                   remove sbl_sahara_mba_authenticate.
04/29/13   kedara  Update boot_auth_load_mba_header to check secure boot status
04/16/13   kedara  Added sbl_sahara_image_load_post_proc to support mba authentication
03/29/13   sy      Move serial_num to global uncache so usb can read the result via Sahara protocol
03/28/13   dhaval  Updates to support sha256 
02/19/13   kedara  Added sbl_sahara_get_elf_header, sbl_sahara_get_program_header
01/18/13   dh      Add sbl_sahara_unrecoverable_error_handler
11/08/12   kedara  Updates to enable hashing and authentication of elf's.
11/06/12   dh      Allow multiple calls to sbl_sahara_bulk_init
10/23/12   kpa     Move elf ZI init to boot_sahara_receive_image_elf.
10/03/12   kpa     Initialized cur_image in sbl_sahara_shared_data to fix mem
                   dumps over sahara for nand boot.
09/21/12   kpa     Added new function to obtain the image src field value 
                   from the image header.
                   Modified the auth function to return the status.
08/02/12   kpa     Enable Flashless boot for 9x25.
06/28/12   aus     Updated return value of boot_sahara_entry
05/17/12   dh      In sbl_sahara_bulk_init, only initialize HSUSB core
05/02/12   aus     Added support for Serial Number read in command mode
09/28/11   aus     Use whitelist clobber table protection for Sahara
09/29/11   dh      Ported to 8974, switch hsusb call to extern driver interface
09/15/11   aus     Removed warnings
08/29/11   aus     Flush the cache before jumping into sahara
08/24/11   aus     Added support for secure boot
07/13/11   aus     Added QHSUSB support for Sahara 
07/12/11   aus     Added support to decide between HSIC and HSUSB based on boot option
06/27/11   aus     Initial version to support Sahara

=============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_loader.h"
#include "boot_sahara.h"
#include "boot_util.h"
#include "boothw_target.h"
#include "boot_dload.h"
#include "boot_dload_debug.h"
#include "sbl1_hw.h"
#include BOOT_PBL_H
#include "boot_extern_qusb_interface.h"
#include "boot_cache_mmu.h"
#include "boot_whitelist_prot.h"
#include "boot_hash_buffer.h"
#include "boot_shared_functions_consumer.h"
#include "boot_hash_buffer.h"
#include "boot_hash.h"
#include "boot_flash_dev_if.h"
#include "boot_whitelist_prot.h"

extern uint32 raw_partition_addr;
extern uint32 raw_partition_len ;

extern uint8 qsee_partition_id[];
extern boot_flash_trans_if_type * trans_if ;

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
static uint32 sbl_sahara_bulk_init(void);
static uint8* sbl_sahara_exec_cmd(uint32 client_command,
                                  uint32 *resp_length,
                                  uint32 *status);
static uint32 sbl_sahara_rx_bulk (uint8 *rx_buf, 
                                  uint32 len, 
                                  sahara_rx_cb_type rx_cb, 
                                  uint32 * err_code, 
                                  enum boot_sahara_packet_type type);
static uint64 sbl_sahara_mem_debug_copy_restricted (const uint64 addr,
                                                    uint64 len);
static boolean sbl_sahara_is_auth_enabled(void);
static uint64 sbl_sahara_get_bin_header_size(void);
static uint64 sbl_sahara_get_bin_image_size(const uint8 *header);
static uint64 sbl_sahara_get_bin_code_size(const uint8 *header);
static uint32 sbl_sahara_get_bin_cert_chain_size(const uint8 *header);
static uint32 sbl_sahara_get_bin_signature_size(const uint8 *header);
static void sbl_sahara_unrecoverable_error_handler(void);
static enum boot_sahara_status sbl_sahara_image_load_post_proc(void);
static boolean sbl_sahara_skip_image_load(void);
static boolean sbl_elf_optimized_segment_load ( void );

/* Definitions for Sahara Authentication table */
static boolean sbl_sahara_init_hash(void);
static boolean sbl_sahara_update_hash(const void *buf_ptr,
                                      uint32 buf_size);
static void* sbl_sahara_finalize_hash(void);
static boolean sbl_sahara_compare_hash(const void *digest_buf_ptr,
                                       uint32 hash_index);
static boolean sbl_sahara_verify_hash(const void *buf_ptr,
                                       uint32 buf_size,
                                       uint32 hash_index);
static void sbl_sahara_elf_compute_hash_offsets(
              const uint8 *hash_tbl_ptr,
              const uint32 num_segments,
              const struct segment_record *prog_hdr_ptr);
static boolean sbl_sahara_authenticate(uint8 *header_ptr);
static void sbl_sahara_reset(void);

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

static uint8 sbl_sahara_mem_debug_buffer[SAHARA_MAX_MEMORY_DATA_SIZE_IN_BYTES];
extern uint8 sahara_packet_buffer[SAHARA_MAX_PACKET_SIZE_IN_BYTES];

/* Authentication flag from config table */
static boot_boolean auth;

/* Hash context used for ELF image.
  Use zero_init attribute to force variables less than 8 bytes to
  be allocated in ZI otherwise compiler may optmize it into RW
*/
static boot_hash_ctx_t sbl_elf_ctx SECTION(".bss.BOOT_UNCACHED_DDR_ZI_ZONE");

/* Delclare a data respond buffer of 4 byte */
static uint8 sahara_cmd_exec_data_resp_buf[4] SECTION(".bss.BOOT_UNCACHED_DDR_ZI_ZONE"); 

/* ELF image info - filled in by sbl_sahara_entry */
static struct boot_sahara_elf_image_info sbl_sahara_elf_image_info SECTION(".bss.BOOT_UNCACHED_DDR_ZI_ZONE");

/* BIN image info - filled in by sbl_sahara_entry */
static struct boot_sahara_binary_image_info sbl_sahara_bin_image_info SECTION(".bss.BOOT_UNCACHED_DDR_ZI_ZONE");
  
/* Store the ELF program headers */
static struct progressive_boot_block elf_prog_headers SECTION(".bss.BOOT_UNCACHED_DDR_ZI_ZONE");

/* Stores the calculated hash value */
static uint8 verify_digest_buf[PBL_SHA_HASH_MAX_LEN] SECTION(".bss.BOOT_UNCACHED_DDR_ZI_ZONE");
extern hash_alg_t sbl_hash_algorithm SECTION(".bss.BOOT_UNCACHED_DDR_ZI_ZONE");
extern uint32 sbl_hash_len SECTION(".bss.BOOT_UNCACHED_DDR_ZI_ZONE");


/* Stores information on what image to load */  
static struct boot_sahara_shared_data sbl_sahara_shared_data = 
{
  0,
  SAHARA_IMAGE_UNKNOWN,
  &sbl_sahara_bin_image_info,
  FALSE,
  FALSE
};

/* SBL Sahara dispatch table */
static struct boot_sahara_dispatch_tbl sbl_sahara_dispatch_tbl = 
{
  SAHARA_INTERFACE_DISPATCH_VERSION,
  sbl_sahara_bulk_init,
  boot_usb_al_bulk_shutdown,
  boot_usb_al_bulk_poll,
  (sahara_rx_bulk_type) sbl_sahara_rx_bulk,
  (sahara_tx_bulk_type) boot_usb_al_bulk_transmit,
  boot_usb_al_bulk_get_max_packet_size,
  sbl_sahara_reset,
  boot_whitelist_address_range_check,
  sbl_sahara_exec_cmd,
  boot_usb_al_bulk_get_max_raw_data_size,
  sbl_sahara_is_auth_enabled,
  qmemcpy,
  qmemset,
  sbl_sahara_get_bin_header_size,
  NULL,
  NULL,
  sbl_sahara_get_bin_image_size,
  sbl_sahara_get_bin_code_size,
  sbl_sahara_get_bin_cert_chain_size,
  sbl_sahara_get_bin_signature_size,
  NULL,
  sbl_sahara_unrecoverable_error_handler,
  sbl_sahara_image_load_post_proc,
  sbl_sahara_skip_image_load,  
  boot_get_hash_segment_buffer,
  sbl_elf_optimized_segment_load,
  NULL,
  NULL,
};

/* Authentication interface for Sahara */
static struct boot_sahara_auth_tbl sbl_sahara_auth_tbl =
{
  SAHARA_INTERFACE_AUTH_VERSION,
  sbl_sahara_init_hash,
  sbl_sahara_update_hash,
  sbl_sahara_finalize_hash,
  sbl_sahara_compare_hash,
  sbl_sahara_verify_hash,
  sbl_sahara_elf_compute_hash_offsets,
  sbl_sahara_authenticate
};

/* SBL Sahara memory debug function table */
static struct boot_sahara_mem_debug_tbl sbl_sahara_mem_debug_tbl = 
{
  dload_mem_debug_supported,
  dload_mem_debug_init,
  dload_mem_debug_table_addr,
  dload_mem_debug_len,
  dload_mem_debug_verify_addr,
  dload_mem_debug_is_restricted_addr,
  sbl_sahara_mem_debug_copy_restricted
};

static struct boot_sahara_interface sbl_sahara_interface = 
{
  SAHARA_INTERFACE_VERSION,
  SAHARA_STATUS_SUCCESS,
  &sbl_sahara_shared_data,
  SAHARA_MODE_IMAGE_TX_PENDING,
  &sbl_sahara_dispatch_tbl,
  &sbl_sahara_auth_tbl,
  &sbl_sahara_mem_debug_tbl
};

/* Pointers to hash table entries */
const uint8 *expected_hash_ptr[MI_PBT_MAX_SEGMENTS];

/* Security image type and shared data to be passed to the auth routine */
static uint32 image_sec_type;
static bl_shared_data_type *sahara_bl_shared_data;

/*===========================================================================

                      PRIVATE FUNCTION DECLARATIONS

===========================================================================*/

/* ============================================================================
**  Function : sbl_sahara_clear_downloaded_data
** ============================================================================
*/
/*!
*   @brief
*   Clears ELF header, Program headers, sahara packet buffer, hash table buffer 
*   and memory region where data was written to.
*   
*   @param  None
*   
*   @par Dependencies
*   
*   @par Side Effects
*   All the downloaded data will be set to 0
*   
*   @retval None
*   
*   @sa None
*/
static void sbl_sahara_clear_downloaded_data( void )
{

  /* reset the flag tracking hash table authentication result for current elf */
  boot_set_hash_table_auth_result(FALSE);

  if ( sbl_sahara_elf_image_info.hash_table != NULL )
  {
    /* Clear hash table buffer */
    qmemset(sbl_sahara_elf_image_info.hash_table, 0x0, boot_get_hash_buffer_size());
  }
  
  if ( sbl_sahara_elf_image_info.prog_headers != NULL )
  {
    /* Clear program headers */
    qmemset(sbl_sahara_elf_image_info.prog_headers, 0x0, sizeof(struct progressive_boot_block));
  }
  
  if ( expected_hash_ptr != NULL )
  {
    /* Clear hash table buffer */
    qmemset(expected_hash_ptr, 0x0, sizeof(expected_hash_ptr));
  }
  
  /* Clear Elf header */
  qmemset(&(sbl_sahara_elf_image_info.elf_header), 0x0, sizeof(Elf32_Ehdr));

  /* Clear sahara packet buffer */
  qmemset(&sahara_packet_buffer, 0x0, sizeof(sahara_packet_buffer));
}

/* ============================================================================
**  Function : sbl_sahara_init_hash
** ============================================================================
*/
/*!
*   @brief
*   Initializes the context for computing the hash of a given set of data.
*   
*   @details
*   Initializes the context for computing the hash of a given set of data.   
*   
*   @param  None
*   
*   @par Dependencies
*   This must be called before calling sbl_sahara_update_hash or
*   sbl_sahara_finalize_hash.
*   
*   @par Side Effects
*   None
*   
*   @retval returns TRUE if no error occurred; FALSE otherwise
*   
*   @sa None
*/
static boolean sbl_sahara_init_hash( void )
{
  boolean status = FALSE;
  
  /* Initialize hash context */   
  if (BL_ERR_NONE != boot_hashInit(&sbl_elf_ctx, sbl_hash_algorithm))
    sbl_sahara_clear_downloaded_data();
  else 
    status = TRUE;

  return status;
}

/* ============================================================================
**  Function : sbl_sahara_update_hash
** ============================================================================
*/
/*!
*   @brief
*   Computes the hash of the input buffer and updates the internal digest
*   buffer.
*   
*   @details
*   Computes the hash of the input buffer and updates the internal digest
*   buffer.
*   
*   @param *buf_ptr   -  [IN] Input buffer to hash
*   @param buf_size   -  [IN] Size of data to hash
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval returns TRUE if no error occurred; FALSE otherwise
*   
*   @sa None
*/
static boolean sbl_sahara_update_hash
(
  const void *buf_ptr,
  uint32 buf_size
)
{
  boolean status = FALSE;
  
  if (BL_ERR_NONE != boot_hashUpdate(&sbl_elf_ctx, buf_ptr, buf_size))
    sbl_sahara_clear_downloaded_data();
  else
    status = TRUE;

  return status;
}

/* ============================================================================
**  Function : sbl_sahara_finalize_hash
** ============================================================================
*/
/*!
*   @brief
*   Finalizes the hash context and returns the resulting digest buffer.
*   
*   @details
*   Finalizes the hash context and returns the resulting digest buffer.
*   
*   @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval returns hash digest buffer
*   
*   @sa None
*/
static void* sbl_sahara_finalize_hash( void )
{ 
  if (BL_ERR_NONE != boot_hashFinal(&sbl_elf_ctx, verify_digest_buf, sbl_hash_len))
  {
    sbl_sahara_clear_downloaded_data();
    return NULL;
  }
  else
  {
    return verify_digest_buf;
  }
}

/* ============================================================================
**  Function : sbl_sahara_compare_hash
** ============================================================================
*/
/*!
*   @brief
*   Compares the input digest buffer with the expected hash entry corresponding
*   to the input hash_index. The expected hash entries are stored internally
*   and are indexed based on the stored program headers for the given image.
*   
*   @details
*   Compares the input digest buffer with the expected hash entry corresponding
*   to the input hash_index. The expected hash entries are stored internally
*   and are indexed based on the stored program headers for the given image.
*   
*   @param *digest_buf_ptr   -  [IN] Hash digest to compare
*   @param hash_index        -  [IN] Program header index corresponding to ELF
*                                    segment
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval returns TRUE if the hash matches; FALSE otherwise
*   
*   @sa None
*/
static boolean sbl_sahara_compare_hash
(
  const void *digest_buf_ptr,
  uint32 hash_index
)
{
  boolean status = FALSE;
  do
  {
    if ( boot_get_hash_table_auth_result() != TRUE )
	{
	  status = FALSE;
	  break;
	}
    if (digest_buf_ptr == NULL)
    {
      break;
    }
    status = (bByteCompare(digest_buf_ptr, 
                           expected_hash_ptr[hash_index], 
                           sbl_hash_len) == 0);
  } while (0);
  
  if (status == FALSE)
  {
    sbl_sahara_clear_downloaded_data();
  }

  return status;
}

/* ============================================================================
**  Function : sbl_sahara_verify_hash
** ============================================================================
*/
/*!
*   @brief
*   Computes and compares the hash of the input buffer with the expected hash
*   entry corresponding to the hash_index. The expected hash entries are stored
*   internally and are indexed based on the stored program headers for the
*   given image.
*   
*   @details
*   Computes and compares the hash of the input buffer with the expected hash
*   entry corresponding to the hash_index. The expected hash entries are stored
*   internally and are indexed based on the stored program headers for the
*   given image.
*   
*   @param *buf_ptr     -  [IN] Input buffer to hash
*   @param buf_size     -  [IN] Size of data to hash
*   @param hash_index        -  [IN] Program header index corresponding to ELF
*                                    segment
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval returns retval_description_goes_here
*   
*   @sa None
*/
static boolean sbl_sahara_verify_hash
(
  const void *buf_ptr,
  uint32 buf_size,
  uint32 hash_index
)
{
  boolean status = FALSE;
  void *digest_buf_ptr = NULL;

  do
  {
    if (!sbl_sahara_init_hash())
    {
      break;
    }
    if (!sbl_sahara_update_hash(buf_ptr, buf_size))
    {
      break;
    }
    digest_buf_ptr = sbl_sahara_finalize_hash();
    if (digest_buf_ptr == NULL)
    {
      break;
    }
    status = sbl_sahara_compare_hash(digest_buf_ptr, hash_index);
  } while (0);
  return status;
}

/* ============================================================================
**  Function : sbl_sahara_is_zeros
** ============================================================================
*/
/*!
*   @brief
*   Determines if all bytes in the input array of bytes are all zero or not.
*   
*   @details
*   This function parses each byte of data starting at the input address
*   location to see if the value stored at that location is 0. This
*   continues until all bytes are read (specified by size input).
*   
*   @param *addr   -  [IN] Address location to start reading from
*   @param size    -  [IN] Number of bytes to compare
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval returns TRUE if all bytes are zero; FALSE otherwise
*   
*   @sa None
*/
static boolean sbl_sahara_is_zeros( const void *addr, uint32 size )
{
  const uint8 *start_addr = (uint8*)addr;
  const uint8 *end_addr = start_addr + size;
  boolean retval = TRUE;

  /* Check if each byte is zero */
  do
  {
    if ( *start_addr != 0 )
    {
      retval = FALSE;
      break;
    }
    start_addr++;
  }while(start_addr < end_addr);

  return retval;
}

/* ============================================================================
**  Function : sbl_sahara_elf_compute_hash_offsets
** ============================================================================
*/
/*!
*   @brief
*   This computes the hash table entry offsets for each segment listed in the
*   program headers.
*    
*   @details
*   Based on whether a segment is PAGED or NON-PAGED, the offset for the
*   corresponding hash entry in the hash table will be computed. If the
*   segment is paged, then the next segment offset will be computed based
*   on the number of 4K pages in the paged segment. If the segment is not
*   paged, the next segment offset will be offset by the hash digest size.
*   The resulting offsets will be stored in a global array.
*   
*   @param *hash_tbl_ptr   -  [IN] Pointer to the hash table
*   @param num_segments    -  [IN] Number of program headers
*   @param *prog_hdr_ptr   -  [IN] Pointer to the program headers
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
static void sbl_sahara_elf_compute_hash_offsets
(
  const uint8 *hash_tbl_ptr,                /* Pointer to hash table */
  const uint32 num_segments,                /* Number of program headers */
  const struct segment_record *prog_hdr_ptr /* Pointer to program headers */
)
{
  /* Set segment record pointer initially equal to start of program headers */
  const struct segment_record *seg_ptr = prog_hdr_ptr;
  uint32 segment;               /*Index into program headers */
  uint32 hash_tbl_offset = 0;   /* Current offset into the hash table */
  uint32 hash_digest_size;      /* Size of hash entry in bytes */

  /* Now go through all the segments and compute the hash table offsets */
  BL_VERIFY( num_segments < MI_PBT_MAX_SEGMENTS,BL_ERR_INVALID_HASH_PTR|BL_ERROR_GROUP_BOOT );
  hash_digest_size = sbl_hash_len;

  /* Loop through all the ELF segments */
  for( segment = 0, hash_tbl_offset = 0; segment < num_segments; 
       ++segment, ++seg_ptr )
  {
  
	/* Validate that the hash ptr offsets do not overflow beyond the allocated buffer and that there is atleast one 
	   hash_digest_size of data left over in the buffer for the last hash ptr offset */
	BL_VERIFY(((((uintnt)hash_tbl_ptr + hash_tbl_offset) <= ((uintnt)hash_tbl_ptr + HASH_SEGMENT_SIZE - hash_digest_size)) && 
	           (((uintnt)hash_tbl_ptr + hash_tbl_offset) >= (uintnt)hash_tbl_ptr)), BL_ERR_INTEGER_OVERFLOW|BL_ERROR_GROUP_BOOT);
    /* Set the hash offset for the current segment */
    expected_hash_ptr[segment] = hash_tbl_ptr + hash_tbl_offset;

    /* Determine if segment is a paged or non-paged segment */
    if ( MI_PBT_PAGE_MODE_VALUE(seg_ptr->memory_attribute) == 
         MI_PBT_PAGED_SEGMENT )
    {
            /* Skip the offsets corresponding to paged segments */
      if ( seg_ptr->size != 0 )
      {
		/* Paged segments have a hash entry for each 4K page of data.
         Thus these hash entries must be skipped to find the hash
         entry for the next segment. */

		/* Compute number of 4K pages by rounding up */
      int num_4k_pages = ( ( (int) seg_ptr->size - 1 ) / SIZE4K ) + 1;
      
	  hash_tbl_offset += num_4k_pages * hash_digest_size;
      }
    }
    else if ( MI_PBT_PAGE_MODE_VALUE(seg_ptr->memory_attribute) == 
              MI_PBT_NON_PAGED_SEGMENT )
    {
      /* If the hash table segment has an entry for itself in the hash table,
         it must be zeros. For compatibility, check if the hash table segment
         was added to the ELF file after the hash table was generated and thus
         there is no entry in the hash table for itself.  In this case, the
         hash table segment will be the first ELF segment and the first entry
         in the hash table would not be all zeros.  Don't skip over the
         entry in the hash table for the hash table segment if the entry
         doesn't exist. */
      
      if ( ( MI_PBT_SEGMENT_TYPE_VALUE(seg_ptr->memory_attribute) != 
             MI_PBT_HASH_SEGMENT ) ||
           ( sbl_sahara_is_zeros(hash_tbl_ptr + hash_tbl_offset, 
                                  hash_digest_size) ) )
      {
        /* Point to hash for the next segment in the table */
        hash_tbl_offset += hash_digest_size;
      }
    }
    else
    {
      /* Fail if the segment is not PAGED or NON_PAGED */
      BL_VERIFY( 0x0, BL_ERR_INVALID_ELF_FORMAT|BL_ERROR_GROUP_BOOT );
    }
  }
}

/* ============================================================================
**  Function : sbl_sahara_authenticate
** ============================================================================
*/
/*!
*   @brief
*   Function to authenticate an image given the image header.
*    
*   @details
*   Function to authenticate an image given the image header.
*   
*   @param *header_ptr   -  [IN] Pointer to image header
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  TRUE if authentication was successful or authentication was disabled,
*            FALSE otherwise
*   
*   @sa None
*/
static boolean sbl_sahara_authenticate
(
  uint8 *header_ptr
)
{
  boolean status = FALSE;
  
  /* Prepare header for authentication */
  boot_auth_load_header((mi_boot_image_header_type*)header_ptr);

  /*If the image being authenticated is elf, header will not point to
    local hash table buffer for code signature and cert locations */
  if(sbl_sahara_shared_data.expected_image_type == SAHARA_IMAGE_TYPE_ELF)
  {
    /*Update the authentication variables to point to hash segment */
    boot_update_auth_image_info(boot_get_hash_segment_buffer());  
  }
  
  /* Authenticate the image */ 
  if (BL_ERR_NONE != boot_auth_image(sahara_bl_shared_data, (uint32)image_sec_type))
  {
     /* set the flag tracking hash table authentication to FALSE for current elf */
     boot_set_hash_table_auth_result(FALSE);
     sbl_sahara_clear_downloaded_data();
  }
  else
  {
    /* set the flag tracking hash table authentication to TRUE for current elf */
    boot_set_hash_table_auth_result(TRUE);
    status = TRUE;
  }
  return status;
}
  
/* ============================================================================
**  Function : sbl_sahara_image_load_post_proc
** ============================================================================
*/
/*!
*  @brief
*  Function for post processing after an image load.
*    
*  @details
*  Function for post processing after an image load.
*   
*  @param
*    None
*   
*  @par Dependencies
*    None
*   
*  @par Side Effects
*    None
*   
*  @retval  SAHARA_STATUS_SUCCESS if post processing was successful else
*           error code as indicated by boot_sahara_status
*   
*  @sa None
*/
static enum boot_sahara_status sbl_sahara_image_load_post_proc (void)
{
  enum boot_sahara_status status = SAHARA_STATUS_SUCCESS;
  
  /* Placeholder for any post image-load steps specific for an image */

  return status;
}
  
/* ============================================================================
**  Function : sbl_elf_optimized_segment_load
** ============================================================================
*/
/*!
*  @brief
*  Function to configure elf loading logic.
*    
*  @details
*  Stubbed out. Not supported for 8084
*   
*  @param
*  None
*   
*  @par Dependencies
*  None
*   
*  @par Side Effects
*  None
*   
*  @retval  Boolean. True if single read request is to be sent 
*           for loading all elf segments
*   
*  @sa None
*/  
static boolean sbl_elf_optimized_segment_load (void)
{
  return FALSE;
}

/* ============================================================================
**  Function : sbl_sahara_rx_bulk
** ============================================================================
*/
/*!
*   @brief
*   Wrapper for the receive bulk function.
*   
*   @details
*   Wrapper for the receive bulk function.
*   
*   @param  rx_buf -     [IN] - buffer to fill with incoming data
*   @param  len    -     [IN] - buffer length
*   @param  rx_cb  -     [IN] - Callback function from Sahara
*   @param  p_err_code - [OUT] - error code
*   @param  type       - [OUT] - type of packet
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval returns number of bytes received
*   
*   @sa None
*/
static uint32 sbl_sahara_rx_bulk 
(
  uint8 *rx_buf, 
  uint32 len, 
  sahara_rx_cb_type rx_cb, 
  uint32 * err_code, 
  enum boot_sahara_packet_type type
)
{
  return boot_usb_al_bulk_receive(rx_buf, len, rx_cb, err_code);
}

/* ============================================================================
**  Function : sbl_sahara_bulk_init
** ============================================================================
*/
/*!
*   @brief
*   Initialize HS-USB clocks and PHY.
*   
*   @details
*   This function first disables data cache and then proceeds to initialize
*   the HS-USB clocks and PHY.
*   
*   @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  BULK_SUCCESS if no error
*   
*   @sa None
*/
static uint32 sbl_sahara_bulk_init( void )
{
  boot_qusb_al_bulk_init(QUSB_MAX_SPEED_SUPER);

  return BULK_SUCCESS;
}

/* ============================================================================
**  Function : sbl_sahara_exec_cmd
** ============================================================================
*/
/*!
*   @brief
*   Executes a command based on the input client_command and set the
*   resp_buffer with the corresponding response.
*   
*   @details
*   This function decodes the client_command and executes the corresponding
*   functions that are supported by SBL. The pointer to the response data
*   is returned, along with the length and status of execution.
*   
*   @param client_command   -  [IN    ] ID of command to execute
*   @param *resp_length     -  [IN/OUT] Response length for executed command
*   @param *status          -  [IN/OUT] Status of executed command
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval returns pointer to response data.
*   
*   @sa None
*/
static uint8* sbl_sahara_exec_cmd
(
  uint32 client_command,
  uint32 *resp_length,
  uint32 *status
)
{
  /* Store client command responses */
  uint32 resp_buff;
  enum boot_sahara_status exec_status = SAHARA_STATUS_SUCCESS;

  /* Check for maximum size required for current commands supported
     and input parameters */
  if (resp_length == NULL)
  {
    /* ERROR: Invalid parameter passed to command execution */
    exec_status = SAHARA_NAK_EXEC_CMD_INVALID_PARAM;
  }
  else
  {
    /* Execute command based on input client_command */
    switch(client_command)
    {
      case SAHARA_EXEC_CMD_SERIAL_NUM_READ:
      resp_buff=HWIO_INF( QFPROM_CORR_PTE_ROW1_LSB, SERIAL_NUM);
      *resp_length = sizeof(resp_buff);

		  /* Flush cache  */
      dcache_flush_region(&resp_buff, sizeof(resp_buff));
      
      qmemcpy(&sahara_cmd_exec_data_resp_buf,&resp_buff,*resp_length); 

        break;	
		
      default:
        *resp_length = 0;
        /* ERROR: Unsupported client command received */
        exec_status = SAHARA_NAK_EXEC_CMD_UNSUPPORTED;
        break;
    }
  }

  *status = exec_status;
  return (uint8 *)&sahara_cmd_exec_data_resp_buf; 
}

/* ============================================================================
**  Function : sbl_sahara_check
** ============================================================================
*/
/*!
*   @brief
*   This function will check to see if the Sahara should be entered
*   in image transfer mode or in memory debug mode.
*   
*   @details
*   This function will attempt to read magic numbers stored in RAM to
*   determine whether to enter image transfer mode or memory debug mode.
*   
*   @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval returns retval_description_goes_here
*   
*   @sa None
*/
enum boot_sahara_mode sbl_sahara_check( void )
{
  enum boot_sahara_mode mode = SAHARA_MODE_IMAGE_TX_PENDING;
  
  /* Determine if memory debug should be entered or normal image transfer mode */
  if ( boot_dload_entry())
  {
    mode = SAHARA_MODE_MEMORY_DEBUG;
  }

  return mode;
}

/* ============================================================================
**  Function : sbl_sahara_mem_debug_copy_restricted
** ============================================================================
*/
/*!
*   @brief
*   Copies the data to a temporary buffer for a restricted region
*    
*   @details
*   Copies the data to a temporary buffer for a restricted region
*      
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  Pointer to the buffer
*   
*   @sa None
*/
static uint64 sbl_sahara_mem_debug_copy_restricted
(
  const uint64 addr,
  uint64 len
)
{
  uint64 mem_debug_buffer_addr = 0;
  // modified the function for reading the special region or raw region
  if((addr >= (uint64)raw_partition_addr) && (addr+len <= ((uint64)raw_partition_addr + raw_partition_len)))
  {
  	 /* If  partition exists */
	if(trans_if != NULL  &&  len <=SAHARA_MAX_MEMORY_DATA_SIZE_IN_BYTES)
	{
		boot_set_entry_as_img_whitelist((uint64)sbl_sahara_mem_debug_buffer , (uint64)len);
			
		boot_flash_trans_read(trans_if,
					(void *)sbl_sahara_mem_debug_buffer,
					addr-(uint64)raw_partition_addr,
					len,IMAGE_BODY_TYPE);

		boot_invalidate_img_whitelist();

		mem_debug_buffer_addr = (uint64)&sbl_sahara_mem_debug_buffer[0];
   	
    }
  }
  else if ((addr > 0) && (len <= SAHARA_MAX_MEMORY_DATA_SIZE_IN_BYTES))
  {
     /* Copy over data to temporary buffer */
     qmemcpy(sbl_sahara_mem_debug_buffer,
             (const uint8*)addr, 
             len);
     mem_debug_buffer_addr = (uint64)&sbl_sahara_mem_debug_buffer[0];
  }
  return mem_debug_buffer_addr;
}

/* ============================================================================
**  Function : sbl_sahara_is_auth_enabled
** ============================================================================
*/
/*!
*   @brief
*   Returns whether image authentication is enabled or not.
*    
*   @details
*   Returns whether image authentication is enabled or not.
*      
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  TRUE if authentication is enabled,
*            FALSE otherwise
*   
*   @sa None
*/
static boolean sbl_sahara_is_auth_enabled(void)
{  
  return (boolean) auth ? boot_pbl_is_auth_enabled() : FALSE; 
}

static uint64 sbl_sahara_get_bin_header_size(void)
{
  return sizeof(mi_boot_image_header_type);
}

static uint64 sbl_sahara_get_bin_image_size(const uint8 *header)
{
  mi_boot_image_header_type* image_header = (mi_boot_image_header_type*)header;
  return image_header->image_size;
}

static uint64 sbl_sahara_get_bin_code_size(const uint8 *header)
{
  mi_boot_image_header_type* image_header = (mi_boot_image_header_type*)header;
  return image_header->code_size;
}

static uint32 sbl_sahara_get_bin_cert_chain_size(const uint8 *header)
{
  mi_boot_image_header_type* image_header = (mi_boot_image_header_type*)header;
  return image_header->cert_chain_size;
}

static uint32 sbl_sahara_get_bin_signature_size(const uint8 *header)
{
  mi_boot_image_header_type* image_header = (mi_boot_image_header_type*)header;
  return image_header->signature_size;
}

static boolean sbl_sahara_skip_image_load(void)
{
  /* Do not error on header corruption for these images instead move to loading the
     next image */
  if ((sbl_sahara_shared_data.expected_image == RAMFS1_IMG) || 
      (sbl_sahara_shared_data.expected_image == RAMFS2_IMG) ||
      (sbl_sahara_shared_data.expected_image == BACKUP_RAMFS_IMG) ||
      (sbl_sahara_shared_data.expected_image == ACDB_IMG))
  {
    return TRUE;
  }
  else
    return FALSE;
}

static void sbl_hw_reset_config (void)
{
  /*Set reset controller to enter second pass state after 
    warm reset and start from bootrom. sdi not entered since
    apcs start addr reg is cleared.

    If we see the FSM is in first pass, we cannot trigger wdog bite
    reset so force the first pass complete state for FSM

    Below changes needed to support use case of taking ramdump even after a 
    usb plugout-plugin event, in case its not complete
  */
  
  if(HWIO_INF(AOSS_CC_RESET_FSM_STATUS, FSM_STATE) == 0X6)
    HWIO_OUTF(AOSS_CC_RESET_FSM_CTRL, FIRST_PASS_COMPLETE, 0X1);

  /* Configure AOSS_CC_RESET_DEBUG_ENABLE */    
  HWIO_OUT(AOSS_CC_RESET_DEBUG_ENABLE,0x76);

}

// static void sbl_hw_reset_wdog (void)
// {
  // Set reset controller to enter second pass state after
    // warm reset and start from bootrom. sdi not entered since
    // apcs start addr reg is cleared.

    // If we see the FSM is in first pass, we cannot trigger wdog bite
    // reset so force the first pass complete state for FSM
 
  
  // static uint32 reset_reason[5] = {0};
  // reset_reason[0] = HWIO_IN(GCC_RESET_STATUS);	
  // HWIO_OUT(GCC_RESET_STATUS, reset_reason[0]);
  
  // reset_reason[1] = HWIO_IN(GCC_RESET_FSM_STATUS);	
  // HWIO_OUT(GCC_RESET_FSM_STATUS, reset_reason[1]);
  
  // if(HWIO_INF(GCC_RESET_FSM_STATUS, FSM_STATE) == 0X6)
    // HWIO_OUTF(GCC_RESET_FSM_CTRL, FIRST_PASS_COMPLETE, 0X1);

  // Note: FIRST_PASS_TMR does not seem to move FSM to 2nd pass state
  // HWIO_OUT(GCC_RESET_DEBUG_ENABLE,0x76);
  // Set minimum timer value
  // HWIO_OUT(GCC_FIRST_PASS_TIMEOUT, 0x10000);
  // Enable first pass timer
  // HWIO_OUTF(GCC_RESET_FSM_CTRL,FIRST_PASS_TMR_EN,0x1);

  // HWIO_OUTF(MPM2_WDOG_CTL_REG, WDOG_EN, 0);
  
  
  // Cause secure watchdog bite
  // Program bark and bite timeouts.
  // HWIO_OUTF(MPM2_WDOG_BARK_VAL_REG, WDOG_BARK_VAL, 0x2);
  // while(HWIO_INF(MPM2_WDOG_BARK_VAL_REG, SYNC_STATUS) == 0);


  // HWIO_OUTF(MPM2_WDOG_BITE_VAL_REG, WDOG_BITE_VAL, 0x1);
  // while(HWIO_INF(MPM2_WDOG_BITE_VAL_REG, SYNC_STATUS) == 0);

  // HWIO_OUTF(MPM2_WDOG_CTL_REG, WDOG_EN, 1);
  // HWIO_OUTF(MPM2_WDOG_CTL_REG, WDOG_CLK_EN, (uint32)1);

  // Reset to make sure the count starts at 0.
  // HWIO_OUT(MPM2_WDOG_RESET_REG, 1);
// }


static void sbl_sahara_unrecoverable_error_handler(void)
{
  /* when sbl sahara encounters an unrecoverable error such as cable unplugged, 
     shutdown usb hw and reboot into dload mode*/
  boot_usb_al_bulk_shutdown();
#ifdef BOOT_RESET_IN_RAM_DUMP_UNRECOVER_HANDLER
  /* temporary flag (default disabled)in case no-reset (#else) logic faces issues */

  boot_dload_set_cookie();
  
    /* Set PSHOLD reset type to warmreset */
  boot_set_pshold_reset_type(BOOT_WARM_RESET_TYPE);

  /* Usually at this point we only call boot_hw_reset(BOOT_WARM_RESET_TYPE)
    to warm reset. However we also need to configure GCC_RESET_DEBUG_ENABLE
    to ensure PIMEM is retained across warm reset.
  */
  
  // sbl_hw_reset_wdog();
   
   // For Future reference - warm reset
    sbl_hw_reset_config();
    boot_hw_reset(BOOT_WARM_RESET_TYPE);
#else

   if(trans_if != NULL)
  {   
	boot_flash_dev_close_image(&trans_if);  
  }		

  /* re-initialize heap, zi-buffer, restore stack-pointer and re-enter download mode without reset */
  boot_DALSYS_DeInitMod();
  qmemset((void*)SCL_RAMDUMP_DLOAD_BUFFERS_BASE,0,SCL_RAMDUMP_DLOAD_BUFFERS_SIZE);
  dcache_flush_region((void *)SCL_RAMDUMP_DLOAD_BUFFERS_BASE,(uintnt)SCL_RAMDUMP_DLOAD_BUFFERS_SIZE);
  qmemset((void*)sbl1_get_external_heap_addr(),0,sbl1_get_external_heap_size());
  dcache_flush_region((void *)sbl1_get_external_heap_addr,(uintnt)sbl1_get_external_heap_size());
  boot_DALSYS_HeapDeInit((void*)sbl1_get_external_heap_addr());
  /* Add the DDR heap to DAL */
  boot_DALSYS_HeapInit((void*)sbl1_get_external_heap_addr(), sbl1_get_external_heap_size(), TRUE);
  boot_restore_ramdump_sp();
  ((void (*)())(uintnt)(SCL_RAMDUMP_CODE_BASE))(); 
#endif
}

static void sbl_sahara_reset(void)
{
  /* if sahara status indicates failure, then we need to save SBL crash dumps */
  if (sbl_sahara_interface.sahara_status != SAHARA_STATUS_SUCCESS)
  {
    /* this function is not going to return */
    sbl_error_handler();
  }

   if(trans_if != NULL)
  {   
	boot_flash_dev_close_image(&trans_if);  
  }	
  boot_hw_reset(BOOT_HARD_RESET_TYPE);
}

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/* ============================================================================
**  Function : sbl_sahara_get_interface
** ============================================================================
*/
/*!
*   @brief
*   Returns pointer to the Sahara interface.
*   
*   @details
*   Returns pointer to the Sahara interface.
*   
*  @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval returns pointer to the Sahara interface.
*   
*   @sa None
*/
struct boot_sahara_interface* sbl_sahara_get_interface( void )
{
  return &sbl_sahara_interface;
}

/* ============================================================================
**  Function : sbl_sahara_entry
** ============================================================================
*/
/*!
*   @brief
*   This function provides an entry into the Sahara protocol from SBL1.
*   This function performs any initialization required and jumps into the
*   protocol to transfer images directly to memory.
*   
*   @details
*   This function provides an entry into the Sahara protocol from SBL1.
*   This function performs any initialization required and jumps into the
*   protocol to transfer images directly to memory.
*   
*   @param none
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
void sbl_sahara_entry ( 
   mi_boot_image_header_type *header_ptr,
   uint32 image_id,
   uint32 image_type,
   uint32 sec_image_type,
   boot_boolean auth_flag,
   boot_boolean jump_flag,      
   bl_shared_data_type * bl_shared_data   
)
{ 
  sbl_sahara_shared_data.expected_image = image_id;
  sbl_sahara_shared_data.expected_image_type = (enum boot_sahara_image_type)image_type;  
  auth = auth_flag;
  image_sec_type = sec_image_type;
  sahara_bl_shared_data = bl_shared_data;

  sbl_sahara_interface.sahara_mode = sbl_sahara_check();

  /* If jump flag is set that would mean that this is the last image to be loaded */
  if (jump_flag)
  {
     sbl_sahara_interface.sahara_mode = SAHARA_MODE_IMAGE_TX_COMPLETE;
  }
  
  /* Set the cur_image pointer based on ELF or Binary */
  if(image_type == SAHARA_IMAGE_TYPE_ELF)
  {
    sbl_sahara_shared_data.cur_image = &sbl_sahara_elf_image_info; 
    
    /* reset the flag tracking hash table authentication result for current elf */
    boot_set_hash_table_auth_result(FALSE);

    /* Clear hash table buffer */
    qmemset(boot_get_hash_segment_buffer(), 0x0, boot_get_hash_buffer_size());

    /* Clear program headers */
    qmemset(&elf_prog_headers, 0x0, sizeof(struct progressive_boot_block));

    /* Clear Elf header */
    qmemset(&(sbl_sahara_elf_image_info.elf_header), 0x0, sizeof(Elf32_Ehdr));

    /* Initialize program header location */
    sbl_sahara_elf_image_info.prog_headers = &elf_prog_headers;
    sbl_sahara_elf_image_info.prog_headers_size = sizeof(struct progressive_boot_block);
  }
  else
  {
    sbl_sahara_shared_data.cur_image = &sbl_sahara_bin_image_info;   
    sbl_sahara_bin_image_info.header = (uint8 *)header_ptr;	
  }
  
  /* Download image to memory */
  BL_VERIFY(boot_sahara_entry(&sbl_sahara_interface), BL_ERR_BOOT_SAHARA_INIT_FAIL|BL_ERROR_GROUP_BOOT);
  BL_VERIFY((sbl_sahara_interface.sahara_status == SAHARA_STATUS_SUCCESS), BL_ERR_OTHERS|BL_ERROR_GROUP_BOOT);	
}

/* ============================================================================
**  Function : sbl_sahara_get_elf_header
** ============================================================================
*/
/*!
*   @brief
*   This function returns elf header base address.
*   
*   @details
*   This function returns elf header base address for the last loaded elf image
*   
*   @param none
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
uint64 sbl_sahara_get_elf_header()
{
  return (uint64)&(sbl_sahara_elf_image_info.elf_header);
}

/* ============================================================================
**  Function : sbl_sahara_get_program_header
** ============================================================================
*/
/*!
*   @brief
*   This function returns program header buffer base address.
*   
*   @details
*   This function returns program header buffer base address
*   for the last loaded elf image.
*   
*   @param none
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
uint64 sbl_sahara_get_program_header()
{
  return (uint64)&(sbl_sahara_elf_image_info.prog_headers->segment[0]);
}
