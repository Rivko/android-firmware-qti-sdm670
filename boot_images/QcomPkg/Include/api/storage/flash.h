#ifndef FLASH_H
#define FLASH_H

/**********************************************************************
 * flash.h
 *
 * Public interface declaration for the Flash Drivers.
 *
 * This file describes the Flash DAL external interface, common to all
 * targets.
 *
 * Copyright (c) 2008-2010,2012, 2015 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *
 **********************************************************************/
/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *
 *
 * when         who     what, where, why
 * ----------   ---     ------------------------------------------------
 * 2008-07-15   dp      Initial version
 *=======================================================================*/

/**  \mainpage Flash DAL Functional Overview
 *
 * The Flash driver module consists of three main components :
 * - Flash device adaptation layer common to all OSs (Flash DAL)
 * - Flash hardware abstraction layer (Flash Hardware Abstraction Layer (HAL))
 * - Flash target specific configuration data
 *
 * \n
 *
 * The Flash driver provides a high-level interface for clients, such
 * as bootloaders, file systems, and tools to interact with the flash devices. It
 * handles the Flash device specifics and abstracts the client from the device-
 * specific algorithms, interfaces, and OS adaptations. The Flash HAL layer
 * also contains the HAL interface and its implementation. The HAL provides a
 * standard API to the Flash DAL, independent of the underlying Mobile Station
 * Modem&tm; &nbsp;(MSM&tm;) ASIC hardware and Flash device. \n
 *
 * \n
 *
 */
#include "com_dtypes.h"

#define FLASH_USES_DAL  /**< Indicates that DAL-based flash driver is in use */

#define FLASH_HAVE_ERASE_BLK_BY_SIZE /**< Indicates block erase by size API is 
                                          supported */

/** Flash DAL operation status */
#define FLASH_DEVICE_DONE                 0  /**< Operation passed */
#define FLASH_DEVICE_FAIL                (-1) /**< Operation failed */
#define FLASH_DEVICE_NOT_SUPPORTED       (-2) /**< Device/operation not supported */
#define FLASH_DEVICE_INVALID_PARAMETER   (-3) /**< API parameters invalid */
#define FLASH_DEVICE_NO_MEM              (-4) /**< Insufficient memory */
#define FLASH_DEVICE_OP_INPROGRESS       (-5) /**< Operation in progress */

typedef int FLASH_STATUS;                    /**< Error status values used in FLASH driver */ 

#define FLASH_NOR_ERASE_REGION_MAX       5   /**< Maximum number of erase block
                                                  regions in NOR device */
#define FLASH_NOR_BLOCK_ERASE_CONFIG_MAX 5   /**< Maximum number of erase block
                                                  configuration info in NOR device */

/** Flash Device Technology */
enum flash_type
{
  FLASH_NOR,      /**< NOR Device */
  FLASH_NOR_SPI,  /**< SPI NOR Device */
  FLASH_UNKNOWN = 0x7FFFFFFF /* Force 32-bit enum */
};

/** Flash device width */
enum flash_width
{
  FLASH_x8  = 8,    /**< x8 Interface Width Device */
  FLASH_x16 = 16,   /**< x16 Interface Width Device */
  FLASH_WIDTH_UNKNOWN = 0x7FFFFFFF /* Force 32-bit enum */
};

/** Flash write style description of how write grouping happens, and what is necessary to
 * recover from partial and failed writes. */
enum flash_write_style
{
  FLASH_WRITES_SIMPLE, /**<  NOR devices where arbitrary bits can be set
    to zero. SLC NOR devices generally fall in this category. */
  FLASH_WRITES_PAGED,    /**<  NAND devices requiring entire pages to be
    written. Also it can only be written to once. These devices do not
    allow 'partial_write'. */
  FLASH_WRITES_PAIRED_BITS,  /**<  NOR devices where adjacent pairs of
    bits are written together. A pair can be written from '11' to a value,
    and from a value to '00'.  If recovery from failed writes is needed,
    the only two values that should be used are '11' and '00'.  After a
    failed write, it may not be possible to write the pair to any value
    other than '00'. */
  FLASH_WRITES_UNSUPPORTED = 0x1000,    /**<  Indicates a NOR device with
    an unsupported write style.  No assumptions can be made about how the
    data is written. Pages must be written in their entirety, and pages that
    have bad powerdowns cannot be used. */
  FLASH_WRITES_STYLE_UNKNOWN = 0x7FFFFFFF /* Force 32-bit enum */
};

/** Flash information type */
enum flash_info_type
{
  FLASH_DEVICE_INFO,               /**< General flash device information */
  FLASH_NOR_ERASE_REGION_INFO,     /**< NOR Erase region information */
  FLASH_NOR_BLOCK_ERASE_CFG_INFO   /**< NOR Block configuration information */
};

/** Different NOR block erase configurations */
struct flash_nor_block_erase_info
{
  uint32   block_count;            /**< Total blocks in this partition  */
  uint32   pages_per_block;        /**< Number of pages in a block */
};

/** Different NOR block erase configurations */
struct flash_nor_block_erase_cfg_info
{
  uint32 erase_cfg_count;       /**< Number of erase block configurations */
  struct flash_nor_block_erase_info block_cfg[FLASH_NOR_BLOCK_ERASE_CONFIG_MAX];
                                /**< Erase block configuration */
};

/** NOR erase block region information */
struct flash_nor_region_info
{
  uint32 block_count;              /**< Number of blocks in this
                                        erase region */
  uint32 block_size_in_bytes;      /**< Size of each block in this
                                        erase region*/
};

/** NOR erase region specific information */
struct flash_nor_erase_region_info
{
  uint32 erase_region_count;       /**< Number of erase block regions
                                        in this device */
  struct flash_nor_region_info erase_region[FLASH_NOR_ERASE_REGION_MAX];
                                   /**< Erase block regions info */
};

/** Flash client device data */
struct flash_info
{
  enum flash_type  type;               /**< Device Technology Type */
  enum flash_width width;              /**< Device Interface Width */
  enum flash_write_style write_style;  /**< Write style specific to Flash */
  uint8  device_name[32];              /**< Device name string */
  uint32 maker_id;                     /**< Manufacturer Identification */
  uint32 device_id;                    /**< Device Identification */
  uint32 version_id;                   /**< Device Version ID, if any */
  uint32 block_count;                  /**< Number of total blocks in device */
  uint32 pages_per_block;              /**< Number of pages in a block */
  uint32 page_size_bytes;              /**< Page size in bytes */
};

/** Type of block size to be used with flash nor erase block API */
enum block_size_opcode
{
  BLOCK_SIZE_4KB = 4,     /**< Erase block size unit of 4KB. */
  BLOCK_SIZE_32KB = 32,   /**< Erase block size unit of 32KB. */
  BLOCK_SIZE_64KB = 64,   /**< Erase block size unit of 64KB. */
  BLOCK_SIZE_UNKNOWN = 0x7FFFFFFF /* Force 32-bit enum */
};

/** Flash power mode */
enum flash_power_mode
{
  FLASH_POWER_LOW_PWR_MODE, /**< Low power mode */
  FLASH_POWER_NORMAL_MODE,  /**< Normal(Default) mode */
  FLASH_POWER_UNKNOWN_MODE = 0x7FFFFFFF /* Force 32-bit enum */
};

typedef struct flash_handle   flash_handle;
typedef flash_handle          *flash_handle_t;

/**\cond */

/**  Wrapper for DAL_DeviceAttach. This registers the flash driver to DAL.
     and loads the driver */
FLASH_STATUS flash_device_attach(uint32 dev_id, flash_handle_t *handle);

/**  Wrapper for DalDevice_Close. Closes the opened device */
FLASH_STATUS flash_device_close(flash_handle_t handle);

/**  Wrapper for DAL_DeviceDetach. De-registers the driver from DAL */
FLASH_STATUS flash_device_detach(flash_handle_t handle);

/**\endcond */

/**
 * Open to initialize device client on Flash
 *
 * @param handle [IN]
 *   FLASH interface handle
 *
 * @param partition_name [IN]
 *   Name of the partition to open.
 *
 * @return int [OUT]
 *   Result of the operation.
 *
 */
FLASH_STATUS flash_device_open(flash_handle_t handle);

/**
 * Gets flash device information. Flash driver assumes that the memory for returned
 * information is allocated by the driver client. It is
 * client's responsibility to typecast the void* parameter
 * to the appropriate type. \n
 * \n
 *  Info Type                  \t  -  Data Structure Associated\n \n
 *
 *  FLASH_DEVICE_INFO            -  struct flash_info \n
 *  FLASH_NOR_ERASE_REGION_INFO  -  struct nor_erase_region_info \n
 *
 * @param handle [IN]
 *   FLASH interface handle
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
FLASH_STATUS flash_get_info(flash_handle_t handle,
  enum flash_info_type info_type, void *flash_info);

/**
 * Erases given flash blocks
 *
 * @param handle [IN]
 *   FLASH interface handle
 *
 * @param start_block [IN]
 *   A starting block for a number of blocks to erase.
 *
 * @param block_count [IN]
 *   Number of blocks to erase
 *
 * @return int [OUT]
 *   Result of the operation.
 *
 */
FLASH_STATUS flash_erase_blocks(flash_handle_t handle,
  uint32 start_block, uint32 block_count);

/**
 * Flash write page API. This API allows writing single/multiple pages
 * within a block.
 *
 * @param handle [IN]
 *   FLASH interface handle
 *
 * @buffer [IN]
 *   Data buffer for flash write operation.
 */

/**
 * For optimum performance, pass DMA friendly (physically contiguous,
 * non-cached) buffer. If the passed memory is not DMA friendly,
 * the DAL will allocate DMA friendly memory and may result in
 * low performance. @n
*/

/**
 *
 * @param start_page [IN]
 *   Logical start page to begin write operation of data
 *
 * @param page_count [IN]
 *   Number of pages to write data.
 *
 * @return int [OUT]
 *   Result of the operation.
 *
 * @note
 *   IMPORTANT: Ensure that the given start page and page count range
 *   falls in one block. It is NOT allowed to write pages that span
 *   across multiple blocks in a single API call.
 *
 */
FLASH_STATUS flash_write_pages(flash_handle_t handle,
  void *buffer,
  uint32 start_page, uint32 page_count);

/**
 * Write partial data to specified page
 *
 * @param handle [IN]
 *   FLASH interface handle
 *
 * @buffer [IN]
 *   Data buffer for flash partial write operation.
 *   Note: For optimum performance, pass DMA friendly (physically
 *         contiguous, non-cached) buffer.
 *
 * @param page [IN]
 *   Logical page to begin partial write operation of data
 *
 * @param page_offset [IN]
 *   Page offset within a given page to start write data.
 *
 * @param byte_count [IN]
 *   Number of bytes to start write data within a given page.
 *
 * @return int [OUT]
 *   Result of the operation.
 *
 */
FLASH_STATUS flash_write_partial_page(flash_handle_t handle,
  void *buffer,
  uint32 page, uint32 page_offset, uint32 byte_count);

/**
 * Flash read page API. This API allows to read single/multiple pages
 * within a block.
 *
 * @param handle [IN]
 *   FLASH interface handle
 *
 * @param read_type [IN]
 *   Flash page read type
 *
 * @buffer [OUT]
 *   Data buffer for a flash read operation.
 */

/**
 * For optimum performance, pass DMA friendly (physically contiguous,
 * non-cached) buffer. If the passed memory is not DMA friendly, then
 * the DAL will allocate DMA friendly memory and may result in
 * low performance.
*/

/**
 *
 * @param start_page [IN]
 *   Logical start page to begin read operation of data
 *
 * @param page_count [IN]
 *   Number of pages to read data.
 *
 * @return int [OUT]
 *   Result of the operation.
 */
FLASH_STATUS flash_read_pages(flash_handle_t handle,
  void *buffer, 
  uint32 start_page, uint32 page_count);

/**
 * Erases given flash blocks of specified block size
 *
 * @param handle [IN]
 *   FLASH interface handle
 *
 * @param block_size_type [IN]
 *   The block size specified to erase in an erase operation
 *
 * @param start_block [IN]
 *   A starting block for a number of blocks to erase 
 *
 * @param block_count [IN]
 *   Number of blocks to erase
 *
 * @return int [OUT]
 *   Result of the operation.
 *
 */
FLASH_STATUS flash_erase_blocks_by_size(flash_handle_t handle,
  enum block_size_opcode block_size_type, uint32 start_block, uint32 block_count);

/**
 * Set the Power mode for the flash device that supports power mode 
 * selection
 *
 * @param handle [IN]
 *   Pointer to FLASH I/F handle
 *
 * @param power_mode [IN]
 *   The power mode to which the flash hardware needs to be set
 *
 * @return int [OUT]
 *   Result of the operation.
 *
 */

FLASH_STATUS flash_set_power_mode(flash_handle_t handle, 
  enum flash_power_mode power_mode);

#endif /* #ifndef FLASH_H */
