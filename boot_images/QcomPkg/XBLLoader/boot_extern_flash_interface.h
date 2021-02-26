#ifndef BOOT_EXTERN_FLASH_INTERFACE_H
#define BOOT_EXTERN_FLASH_INTERFACE_H
/*===========================================================================

                    BOOT EXTERN FLASH DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external flash drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2011-2012, 2014 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/23/14   ck      Removed feature flag as API is always enabled in XBL
06/05/12   kedara  Added boot_flash_boot_ops_get_parti_tbl.
09/08/11   dh      Initial Creation.

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/

#include "flash.h"


/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 


/*===========================================================================

**  Function :  boot_flash_open_partition

** ==========================================================================
*/
/*!
 * Opens a given partition on Flash
 *
 * @param handle [IN]
 *   DAL interface handle
 *
 * @param partition_name [IN]
 *   Name of the partition to open.
 *
 * @return int [OUT]
 *   Result of the operation.
 *
 */
int boot_flash_open_partition(flash_handle_t handle,
                              const unsigned char * partition_name);


/*===========================================================================

**  Function :  boot_flash_read_pages

** ==========================================================================
*/
/*!
 * Flash read page API. This API allows to read single/multiple pages
 * within a block.
 *
 * @param handle [IN]
 *   DAL interface handle
 *
 * @param read_type [IN]
 *   Flash page read type
 *
 * @param buffer_desc [OUT]
 *   Buffers for flash read operation. This buffer descriptor holds
 *   main and spare data buffers for the flash page read operation.
 *   The 'buffer_desc' structure has 4 fields,
 *         - VirtualAddr
 *         - PhysicalAddr
 *         - size
 *         - user
 *
 * This argument points to a set of buffer descriptors. The flash_read_pages
 * API assume that there are at least 2 buffer descriptors. A client can
 * have up to 128 buffer descriptors in an API call.
 * @par
 * The main buffer descriptor points to the main data buffer and the spare buffer
 * descriptor points to the spare user data buffer. Clients can have one buffer
 * descriptor for more than one page. The 'user' field in the buffer descriptor
 * need to be initialized with the number of pages corresponding to the buffer.
 * @par
 * For example: If the main buffer descriptor points to a 10K buffer, then the client
 * can set the user field in the buffer descriptor to 5 (in the case of a 2K page
 * device). Remember to initialize the main and spare user fields with
 * same value.
 * @par
 * For optimum performance, pass DMA friendly (physically contiguous,
 * non-cached) buffer. If the passed memory is not DMA friendly, then
 * the DAL will allocate DMA friendly memory and may result in
 * low performance.
 *
 * @param page_iovec [IN]
 *   I/O vector for flash write. Each element in the vector
 *   contains size of main and spare data to operate,
 *   and logical start page with number of pages to write.
 *
 * @return int [OUT]
 *   Result of the operation.
 *
 * @note
 *   IMPORTANT: The 'page_iovec' allows clients to specify number of
 *   pages read from a given start page. Ensure that the given
 *   range falls in one block. It is NOT allowed to read pages that span
 *   across multiple blocks in a single API call.
 *
 */
int boot_flash_read_pages(flash_handle_t handle,
                          enum page_read_opcode read_type, 
                          dalsys_mem_desc_list * buffer_desc,
                          struct flash_page_iovec * page_iovec);


/*===========================================================================

**  Function :  boot_flash_write_pages

** ==========================================================================
*/
/*!
 * Flash write page API. This API allows writing single/multiple pages
 * within a block.
 *
 * @param handle [IN]
 *   DAL interface handle
 *
 * @param write_type [IN]
 *   Flash page write type
 *
 * @param buffer_desc [OUT]
 *   Buffers for flash read operation. This buffer descriptor holds
 *   main and spare data buffers for the flash page read operation.
 *   The 'buffer_desc' structure has 4 fields,
 *         - VirtualAddr
 *         - PhysicalAddr
 *         - size
 *         - user
 *
 * This argument points to a set of buffer descriptors. The flash_write_pages
 * API assumes that there are at least 2 buffer descriptors. A client can
 * have up to 128 buffer descriptors in an API call. @n
 * @par
 * The main buffer descriptor points to the main data buffer and the spare buffer
 * descriptor points to the spare user data buffer. Clients can have one buffer
 * descriptor for more than one page. The 'user' field in the buffer descriptor
 * needs to be initialized with the number of pages corresponding to the buffer. @n
 * @par
 * For Example: If the main buffer descriptor points to a 10K buffer, the client
 * can set the user field in the buffer descriptor to 5 (in the case of a 2K page
 * device). Remember to initialize the main and spare user fields with the
 * same value. @n
 * @par
 * For optimum performance, pass DMA friendly (physically contiguous,
 * non-cached) buffer. If the passed memory is not DMA friendly,
 * the DAL will allocate DMA friendly memory and may result in
 * low performance. @n
 *
 * @param page_iovec [IN]
 *   I/O vector for flash write. Each element in the vector
 *   contains size of main and spare data to operate,
 *   and logical start page with number of pages to write.
 *
 * @return int [OUT]
 *   Result of the operation.
 *
 * @note
 *   IMPORTANT: The 'page_iovec' allows clients to specify number of
 *   pages to write from a given start page. Ensure that the given
 *   range falls in one block. It is NOT allowed to write pages that span
 *   across multiple blocks in a single API call.
 *
 */
int boot_flash_write_pages(flash_handle_t handle,
                           enum page_write_opcode write_type, 
                           dalsys_mem_desc_list * buffer_desc,
                           struct flash_page_iovec * page_iovec);


/*===========================================================================

**  Function :  boot_flash_erase_blocks

** ==========================================================================
*/
/*!
 * Erases given flash blocks
 *
 * @param handle [IN]
 *   DAL interface handle
 *
 * @param block_vector [IN-OUT]
 *   Vector for block operation. Each element contains a starting block with
 *   the number of blocks to erase and a result vector to hold each block operation
 *   status
 *
 * @param block_count [IN]
 *   Number of elements in block_vector
 *
 * @return int [OUT]
 *   Result of the operation.
 *
 */
int boot_flash_erase_blocks(flash_handle_t handle,
                            struct flash_block_vector * block_vector, 
                            uint32 block_count);


/*===========================================================================

**  Function :  boot_flash_block_get_state

** ==========================================================================
*/
/*!
 * Gets the current state of a given block.
 *
 * @param handle [IN]
 *   DAL interface handle
 *
 * @param block [IN]
 *   The logical block number
 *
 * @param block_state [OUT]
 *   Pointer to the block state buffer
 *
 * @return int [OUT]
 *   Result of the operation.
 *
 */
int boot_flash_block_get_state(flash_handle_t handle,
                               uint32 block, 
                               enum flash_block_state * block_state);


/*===========================================================================

**  Function :  boot_flash_copy_pages

** ==========================================================================
*/
/*!
 * Copy all specified sequential Page(s) from source page(s) to destination
 * page(s)
 *
 * @param handle [IN]
 *   DAL interface handle
 *
 * @param copy_vec [IN-OUT]
 *   flash_copy_iovec vector for flash copy operation.
 *   Each element in the vector contains logical source page and
 *   logical destination page along with number of pages to copy.
 *
 * @param vec_count [IN]
 *   Number of elements in copy_vec
 *
 * @return int [OUT]
 *   Result of the operation.
 *
 */
int boot_flash_copy_pages(flash_handle_t handle,
                          struct flash_copy_iovec * copy_vec, 
                          uint32 vec_count);
#else
  static __inline int boot_flash_copy_pages
  (
    flash_handle_t handle,
    struct flash_copy_iovec *copy_vec, 
    uint32 vec_count
  )
  {
    return FLASH_DEVICE_FAIL;
  }  
#endif


/*===========================================================================

**  Function :  boot_flash_get_info

** ==========================================================================
*/
/*!
 * Gets flash device information. Flash driver assumes that the memory for returned
 * information is allocated by the driver client. It is
 * client's responsibility to typecast the void* parameter
 * to the appropriate type. \n
 * \n
 *  Info Type                  \t  -  Data Structure Associated\n \n
 *
 *  FLASH_DEVICE_INFO            -  struct flash_info \n
 *  FLASH_ONENAND_REG_INFO       -  struct onenand_reg_info \n
 *  FLASH_NOR_ERASE_REGION_INFO  -  struct nor_erase_region_info \n
 *
 * @param handle [IN]
 *   DAL interface handle
 *
 * @param info_type [IN]
 *   Type of information needed.
 *
 * @param flash_info [OUT]
 *   Pointer to the client's flash device info data.
 *
 * @return int [OUT]
 *   Result of the operation.
 *
 */
int boot_flash_get_info(flash_handle_t handle,
                        enum flash_info_type info_type, 
                        void * flash_info);


/*===========================================================================

**  Function :  boot_flash_boot_ops_get_parti_tbl

** ==========================================================================
*/
/*!
 * Update SMEM with MIBIB partition table
 *
 * @param buffer [IN]
 *   Buffer pointing to partition table data.
 *
 * @param size_in_bytes [IN]
 *   size of buffer.
 *
 * @return void [OUT]
 *
 */
void boot_flash_boot_ops_get_parti_tbl(uint32 * buffer,
                                       uint32 * size_in_bytes);

#endif /* BOOT_EXTERN_FLASH_INTERFACE_H */
