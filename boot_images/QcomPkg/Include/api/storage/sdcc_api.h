#ifndef __SDCC_API_H
#define __SDCC_API_H
/******************************************************************************
 * sdcc_api.h
 *
 * This file provides SDCC external definitions.
 *
 * Copyright (c) 2014-2015
 * Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 *****************************************************************************/
/*=============================================================================

                        EDIT HISTORY FOR MODULE


when         who     what, where, why
----------   ---     ---------------------------------------------------------- 
2015-10-13   rm      Added BlockIO2 functions
2015-04-15   bn      Added sdcc_get_emmc_info_raw_data()
2014-11-06   bn      Added eMMC FW Update support
2014-10-14   bn      Added sdcc_sdhci_mode_enable()
2014-08-19   bn      Added XBL Core support
2014-06-18   bn      Initial version. Branched from 8994 SBL
=============================================================================*/

#include "sdcc_errno.h"

/** \mainpage SD Card Controller API Overview
 *
 * SD Card Controller (SDCC) has several levels of public APIs for different
 * card types, different kinds of data request, and specific needs.\n
 *
 * For card types, SDCC provides two types of APIs - memory specific and SDIO
 * APIs.
 *
 * For data request, SDCC provides two types of APIs - legacy sequential code
 * path and I/O vector for a list of data transfers.
 *
 * SDCC also provides Raw Command APIs (see \ref sdcc_raw_cmd_api) to
 * encapsulate commands and data transfer.
 */

/**
 * @defgroup sdcc_driver SD Card Controller (SDCC)
 *
 * The SDCC driver provides block-oriented data transfer to or from the memory
 * device.
 */

/*@{*/

/*SDCC_EXTRACT_COMMON_BEGIN*/
/**
 * The drive numbers are of INT16 type.  SDCC_DRIVENO_0 is the default card
 * slot.
 */
#define SDCC_DRIVENO_0  0
#define SDCC_DRIVENO_1  1
#define SDCC_DRIVENO_2  2
#define SDCC_DRIVENO_3  3

/**
 * This flag denotes that \ref sdcc_get_config_info API is available.
 */
#define FEATURE_SDCC_HAVE_CONFIGURATION_API

/**
 * This flag indicates support for getting the slot type information 
 * (emmc/sd and internal/external)
 */
#define FEATURE_SDCC_GET_SLOT_TYPE_SUPPORTED

/**
 * This flag denotes that \ref sdcc_slot_has_support API exists.
 */
#define HAVE_SDCC_SLOT_HAS_SUPPORT

/**
 * This flag advertises existence of \ref sdcc_activity_timeout_event API.
 */
#define HAVE_SDCC_POWER_COLLAPSE

/**
 * This flag denotes that \ref sdcc_mem_get_device_info API is available.
 * This API retrieves card identification information including manufacturer
 * ID, product name, product serial number, etc.  Also, this API gets the
 * card type, card size (in sectors), speed class, and whether card supports
 * reliable write information.
 */
#define HAVE_SDCC_GET_MEMORY_DEVICE_INFO 

/**
 * A physical partition number denotes the entire memory device.  This value
 * is driver specific and it is used in \ref sdcc_handle_open if it is desired
 * to obtain a handle representing the whole memory device.
 */
#define SDCC_HANDLE_PHY_PARTITION_ENTIRE_MEM_DEVICE 0xFFFFFFFF


/**
 * Following are the memory device slot types that are supported by the SDCC driver.
 */
typedef enum
{
   SDCC_SLOT_UNKNOWN = 0, /**< error. unknown slot type */
   SDCC_SLOT_EMMC    = 1, /**< eMMC slot */
   SDCC_SLOT_SD      = 2 /**< external SD card slot */
} sdcc_slot_mem_type;

/**
 * Following are the slot access (external or internal) configuations 
 * that are supported by the SDCC driver.
 */
typedef enum
{
   SDCC_SLOT_INVALID  = 0, /**< error. invalid slot */
   SDCC_SLOT_INTERNAL = 1, /**< Internal slot */
   SDCC_SLOT_EXTERNAL = 2  /**< External slot */
} sdcc_slot_access_type;

/**
 * Following are the card types that are supported by the SDCC driver.
 * Non-zero enum value indicates card is inserted in a card slot.
 */
typedef enum
{
   SDCC_CARD_UNKNOWN = 0, /**< Card cannot be detected */
   SDCC_CARD_SD      = 1, /**< SD card with card size <= 2GB */
   SDCC_CARD_MMC     = 2, /**< MMC card with card size <= 2GB */
   SDCC_CARD_COMB    = 3, /**< Comb card that supports both SDIO and memory */
   SDCC_CARD_SDIO    = 4, /**< SDIO card */
   SDCC_CARD_SDHC    = 5, /**< SD high capacity card with card size > 2GB */
   SDCC_CARD_MMCHC   = 6  /**< MMC high capacity card with card size > 2GB */
} SDCC_CARD_TYPE;
/*SDCC_EXTRACT_COMMON_END*/

/** \details
 * sdcc_slot_supports lists the kinds of devices that may be used in a slot.
 * This enum is used to associate a slot with the device type it supports.
 */
enum sdcc_slot_supports
{
   SDCC_SLOT_SDIO,
   SDCC_SLOT_MEMORY
};

/**
 * @ingroup sdcc_api
 * Supported bus widths
 */
typedef enum
{
   SDCC_BUSWIDTH_1BIT = 0,
   SDCC_BUSWIDTH_4BIT = 1,
   SDCC_BUSWIDTH_8BIT = 2,
   SDCC_BUSWIDTH_TYPE_COUNT /**< Used to check if a valid bus width value 
                                      is passed*/
} sdcc_buswidth_type;

/**
 * @ingroup sdio_api
 * SD/MMC clock definitions
 */
typedef enum  {
   SDCC_SD_DATA_TRANSFER_0_144_MHZ = 0,
   SDCC_SD_DATA_TRANSFER_0_400_MHZ,
   SDCC_SD_DATA_TRANSFER_20_MHZ,
   SDCC_SD_DATA_TRANSFER_25_MHZ,
   SDCC_SD_DATA_TRANSFER_49_152_MHZ
   /* While adding an enumeration, the sorted order must be maintained.*/
} sdcc_sd_data_transfer_clock_type;

/**
 * @ingroup sdcc_api
 * speed class for SD card
 * Refer to SD2.0 Part 1 Physical Layer Specification, Section 4.13.6 for
 * detailed speed class definition
 */
typedef enum
{
   SDCC_SD_SPEED_CLASS_0,
   SDCC_SD_SPEED_CLASS_2,
   SDCC_SD_SPEED_CLASS_4,
   SDCC_SD_SPEED_CLASS_6
} sdcc_sd_speed_class_type;

/**
 * @ingroup sdcc_api
 * speed class for MMC/eMMC card
 * Refer to JEDEC Standard No.\ 84-A43, Sections 7.8.1 and 8.4 for
 * detailed speed class definition
 */
typedef enum
{
   SDCC_MMC_SPEED_CLASS_LOWEST = 0x00, /**< For cards not reaching the 2.4MB/s
                                        * value */
   SDCC_MMC_SPEED_CLASS_A      = 0x08, /**< Lower bus category classes (26MHz
                                        * clock with 4bit data bus operation) */
   SDCC_MMC_SPEED_CLASS_B      = 0x0A,
   SDCC_MMC_SPEED_CLASS_C      = 0x0F,
   SDCC_MMC_SPEED_CLASS_D      = 0x14,
   SDCC_MMC_SPEED_CLASS_E      = 0x1E,
   SDCC_MMC_SPEED_CLASS_F      = 0x28, /**< Mid bus category classes (26MHz
                                        * clock with 8bit data bus or 52MHz
                                        * clock with 4bit data bus operation) */
   SDCC_MMC_SPEED_CLASS_G      = 0x32,
   SDCC_MMC_SPEED_CLASS_H      = 0x3C,
   SDCC_MMC_SPEED_CLASS_J      = 0x46,
   SDCC_MMC_SPEED_CLASS_K      = 0x50, /**< High bus category classes (52MHz
                                        * clock with 8bit data bus operation) */
   SDCC_MMC_SPEED_CLASS_M      = 0x64,
   SDCC_MMC_SPEED_CLASS_O      = 0x78,
   SDCC_MMC_SPEED_CLASS_R      = 0x8C,
   SDCC_MMC_SPEED_CLASS_T      = 0xA0
} sdcc_mmc_speed_class_type;

/**
 * @ingroup sdcc_iovec_api
 * The following enumeration type is to indicate the transfer type for the
 * scatter/gather data transfer.
 */
typedef enum
{
   SDCC_MEMORY_IOVEC_REGULAR_DATA_XFER = 0,
      /**< regular read or write data transfer */
   SDCC_MEMORY_IOVEC_RELIABLE_WRITE = 1
      /**< reliable write data transfer
       * refer to section 7.5.7 in JEDEC Standard No.\ 84-A43 */
} sdcc_memory_iovec_transfer_type;

/**
 * @ingroup sdio_api
 */

/**
 * Hardware/target specific information will be returned upon query via
 * \ref sdcc_get_config_info API
 */
typedef struct sdcc_config_info
{
   UINT16   num_slots;          /**< total number of card slots */
   UINT16   blk_size;           /**< number of bytes in a block supported by
                                 * SDCC controller */
   UINT32   max_blks;           /**< maximum number of read/write blocks the
                                 * SDCC controller supports per each data
                                 * transaction */
} sdcc_config_info_type;

/**
 * @ingroup sdcc_api
 * The following device info structure is used in
 * \ref sdcc_boot_set_device_info API.  This structure provides device
 * specific information from Boot to avoid complete enumeration
 * of eMMC card
 */
typedef struct _sdcc_boot_device_info
{
   INT16                            driveno;   /**< drive number */
   SDCC_CARD_TYPE                   card_type; /**< card type SD or MMC */
   UINT16                           rca;       /**< relative card address */
   sdcc_buswidth_type               bus_width; /**< data width of SD or MMC */
   sdcc_sd_data_transfer_clock_type clock;     /**< data transfer clock */
   BOOLEAN                          is_raw_valid; /**< is below data valid */
   UINT32                           raw_cid[4];/**< CID register */
   UINT32                           raw_csd[4];/**< CSD register */
   UINT8                            raw_ext_csd[512];/**< EXT CSD register */
} sdcc_boot_device_info_type;

/**
 * @ingroup sdcc_api
 * memory card specific information will be returned upon query
 */
typedef struct _sdcc_mem_info
{
   SDCC_CARD_TYPE       card_type; /**< card type such as SD or MMC */
   UINT32               card_size_in_sectors;
      /**< If the sdcc device handle represents the entire device, card size in
       * sectors denotes the size for the card.\  If the sdcc device handle
       * represents a physical partition, card size in sectors denotes the size
       * of the physical partition.
       * Each sector has 512 bytes */
   UINT32               write_protect_group_size_in_sectors;
      /**< size of the write protect group in sectors
       * Each sector has 512 bytes.\  If this is 0, write protect is not
       * supported.\  This write protect group size can vary from 512 bytes to
       * 33292800 kilobytes.\  Refer to sections 8.3 and 8.4 in JEDEC Standard
       * No. 84-A44 for write protect group size calculation details. */
   UINT32               block_len; /**< number of bytes per block */
   union {
      sdcc_sd_speed_class_type   sd_speed_class;
         /**< speed class for SD card */
      sdcc_mmc_speed_class_type  mmc_speed_class;
         /**< speed class for MMC card */
   } speed_class;
   UINT32               reliable_write_sector_count;
      /**< For eMMC only: the number of reliable write sector count supported */
   BOOLEAN              is_embedded_device;
      /**< flag to indicate if device is embedded */
   UINT32               num_phy_partition_created;
      /**< number of physical partition created */
   UINT32               phy_partition_type;
      /**< type of physical partition to indicate if partition is the user
       * area, boot partition, general purpose partition, etc. */
   BOOLEAN              is_phy_partition_bootable;
      /**< flag to indicate if the physical partition is bootable */
   UINT16               mfr_id;    /**< manufacturer ID */
   UINT16               oem_id;    /**< OEM/application ID */
   UINT8                 prod_name[7];
      /**< product name with NULL terminator
       * For SD card, product name is 5 characters long
       * For MMC card, product name is 6 characters long */
   UINT8                prod_rev;  /**< product revision */
   UINT32               prod_serial_num; /**< product serial number */
   UINT8                mfr_date[8];
      /**< manufacturing date according to SD Physical Layer Specification and
       * MMC/eMMC System Specification
       * Manufacturing date is in mm/yyyy format. */
       
   UINT32              rpmb_parti_size_in_byte;   

   /* HW Reset Function. RST_n_ENABLE bits of EXT_CSD[162] */
   UINT8               hw_reset_function;
   /* Flag indicates support for Write Protection feature */
   BOOLEAN             write_prot_support;  
   /* size of the erase group in sectors */ 
   UINT32               erase_group_size_in_sectors;       
} sdcc_mem_info_type;

/**
 * @ingroup sdcc_api
 * The following structure is used to describe the configuration of the General
 * Purpose Partition and Enhanced User Data Area for eMMC memory devices.  The 
 * structure is expected to be populated with configuration information to be 
 * passed to the sdcc_prog_emmc_gpp_enh function to program the GPP and ENH 
 * configuration information.  Setting the size to 0 indicate the partition
 * is not used.
 */
typedef struct sdcc_emmc_gpp_enh_desc
{
   UINT32  GPP_size[4];        /**< Size of the GPP partitions in sectors */
   UINT32  ENH_size;           /**< Size of enhanced area in sectors      */
   UINT32  ENH_start_addr;     /**< Start of the enhanced area in sectors  */
   UINT8   GPP_enh_flag;       /**< Enhanced area flag         */
} sdcc_emmc_gpp_enh_desc_type;

#define SDCC_ENH_FLAG_USER_AREA     0x01   /**< Enhanced attribute for user area */
#define SDCC_ENH_FLAG_GPP_1         0x02   /**< Enhanced attribute for GPP 1 */
#define SDCC_ENH_FLAG_GPP_2         0x04   /**< Enhanced attribute for GPP 2 */
#define SDCC_ENH_FLAG_GPP_3         0x08   /**< Enhanced attribute for GPP 3 */
#define SDCC_ENH_FLAG_GPP_4         0x10   /**< Enhanced attribute for GPP 4 */

typedef enum {
  DEVICE_IDLE,
  TRANSFER_DONE,
  DEVICE_BUSY,
  DEVICE_ERROR
} DEVICE_STATUS;

typedef enum {
  READ,
  WRITE
} OPERATION_TYPE;

/**
 * SDCC device is a private data structure.  This opaque structure type is
 * declared so the APIs can access the memory device via a pointer to this
 * sdcc_device structure.
 */
struct sdcc_device;

/** \details
 * This function queries the SDCC slot information
 *
 * @param[in] driveno
 * @param[out] sdcc_slot_mem_type
 *      pointer to memory slot type. Slot is emmc or sd memory device
 * @param[out] sdcc_slot_access_type
 *      pointer to slot access info. Slot is for external or internal device
 *
 * @return SDCC_STATUS: SDCC_NO_ERROR or SDCC_ERR_UNKNOWN
 */
SDCC_STATUS sdcc_get_slot_type( INT16 driveno, sdcc_slot_mem_type* slot_mem_type,
                                sdcc_slot_access_type* slot_access_type );
								
/** \details
 * This function gets the hardware/target specific information such as total
 * number of card slots, block size, and maximum number of blocks that the
 * target and SDCC controller support.
 *
 * @param[out] sdcc_pconfig_info
 *    pointer to the structure to save the hardware/target specific information
 *
 * @return SDCC_STATUS: SDCC_NO_ERROR or SDCC_ERR_UNKNOWN
 */
SDCC_STATUS sdcc_get_config_info( sdcc_config_info_type *sdcc_pconfig_info );

/** \details
 * This function checks if the specified drive number can be used for a certain
 * type of device as passed in parameter stype.
 *
 * @param[in] driveno
 *    drive number
 *
 * @param[in] stype
 *    type of slot (memory or IO)
 *
 * @return BOOLEAN TRUE if the driveno supports stype device type.
 *    FALSE otherwise
 */
BOOLEAN sdcc_slot_has_support( INT16 driveno,
                               enum sdcc_slot_supports stype );

/** \details
 * This function tells SDCC that the slot specified by driveno has not been
 * accessed for a certain period of time.  If we have the ability to detect
 * card removal/re-insertion, we shutdown the slot, otherwise do nothing.
 *
 * @param[in] driveno
 *    drive number
 *
 * @return SDCC_NO_ERROR if successful, error code otherwise
 */
SDCC_STATUS sdcc_activity_timeout_event( INT16 driveno );
/*@}*/ /* group sdcc_driver */

/**
 * @defgroup sdcc_api SDCC API Overview
 * @ingroup sdcc_driver
 *
 * The SDCC API provides a list of public functions to SDCC clients such as
 * Hotplug that perform data transfer to/from memory device such as SD or MMC
 * card.
 */
/*@{*/

/** \details
 * This function sets \c device_info structure, obtained from boot
 * to be used for optimized device enumeration at boot.
 *
 * @param[in] device_info
 *    pointer to device information from boot
 */
void sdcc_boot_set_device_info( sdcc_boot_device_info_type *device_info );

/*@}*/ /* group sdcc_api */

/**
 * @defgroup sdcc_handle_api SDCC Handle API
 * @ingroup sdcc_driver
 *
 * The SDCC Handle API provides a list of public functions to SDCC clients
 * such as Hotplug that perform data transfer to/from memory device such as
 * SD or MMC card.
 */
/*@{*/
/** \details
 * This function first initializes the SDCC controller such as turning on the
 * clock, configuring the GPIOs, and turning on the power supply to the device.
 * It returns NULL if there is no memory device attached.  If there
 * is a memory device, it will initialize the device and take it out of the
 * identification state.  It then sets up the device for data transfer.
 * A non-null pointer to the SDCC device is returned.
 *
 * @param[in] driveno
 *    a valid drive number between 0 and the total number of card slots - 1
 *
 * @param[in] phy_partition_num
 *    physical partition number.  Use
 *    \ref SDCC_HANDLE_PHY_PARTITION_ENTIRE_MEM_DEVICE to get a handle
 *    representing the entire memory device.  For eMMC v4.3, the valid boot
 *    partition numbers are 1 and 2.  User area partition number is 0.
 *
 * @return A pointer to the SDCC device if successful.  NULL if failed.
 *
 * @note
 *    sdcc_init is called in sdcc_handle_open
 */
struct sdcc_device *sdcc_handle_open( INT16 driveno, UINT32 phy_partition_num );

/** \details
 * This function de-selects the device, turn off the power supply to the device
 * and switches off the controller clock.
 *
 * @param[in] handle
 *    a pointer to the SDCC device that was returned from \ref sdcc_handle_open
 *
 * @return SDCC_NO_ERROR if successful.  Error code if failed.
 */
SDCC_STATUS sdcc_handle_close( struct sdcc_device *handle );

/** \details
 * This function gets the memory card specific information.  If there is no
 * memory device attached or device is not initialized, all fields in the card
 * specific information are zero.
 *
 * @remarks
 *    \ref sdcc_handle_open must be called before getting card info for the
 *    specified SDCC handle.
 *
 * @param[in] handle
 *    a pointer to the SDCC device that was returned from \ref sdcc_handle_open
 *
 * @param[out] sdcc_pmem_info
 *    pointer to the structure to save the memory card specific information
 *
 * @return SDCC_NO_ERROR on success.  Error code otherwise
 */
SDCC_STATUS
sdcc_handle_mem_get_device_info( struct sdcc_device *handle, 
                                 sdcc_mem_info_type *sdcc_pmem_info );

/** \details
 * This function gets the card status of the specified SDCC handle.
 *
 * @param[in] handle
 *    a pointer to the SDCC device that was returned from \ref sdcc_handle_open
 *
 * @return TRUE if memory device is present.  FALSE otherwise.
 */
BOOLEAN sdcc_handle_is_mem_present( struct sdcc_device *handle );

/** \details
 * This function sets a bootable partition on the embedded device specified in
 * the SDCC \c handle.  Only one partition can be set as active bootable
 * partition -- boot partition 1, boot partition 2, or user area.
 *
 * @param[in] handle
 *    a pointer to the SDCC device that was returned from \ref sdcc_handle_open
 *
 * @return SDCC_NO_ERROR on success.  Error code otherwise
 */
SDCC_STATUS
sdcc_handle_set_active_bootable_partition( struct sdcc_device *handle );

/** \details
 * This function sets the power-on write protection bit of the addressed group
 * provided the memory device specified in the SDCC \c handle is an embedded
 * device and write protect feature is supported on the device.  The power-on
 * write protection is set based on the write protect group.  The write protect
 * group size is memory device specific.  This group size information 
 * \ref write_protect_group_size_in_sectors can be queried via
 * \ref sdcc_handle_mem_get_device_info.  For boot area partitions, power-on
 * write protection would be applied to the entire partition and both boot
 * areas would be power-on write protected.
 *
 * @param[in] handle
 *    a pointer to the SDCC device that was returned from \ref sdcc_handle_open
 *
 * @param[in] data_address
 *    start of the sector to get power-on write protected.  This is the sector
 *    unit address.  This is ignored for boot partition.
 *
 * @param[in] num_of_blocks
 *    number of blocks to be power-on write protected (512 bytes per block).
 *    For user area partition, this is based on the write protect group size.
 *    If num_of_blocks < write protect group size, the entire write protect
 *    group is write protected.
 *    This is ignored for boot partition.
 *
 * @return SDCC_NO_ERROR on success.  Error code otherwise
 *
 * @note
 *    After a power failure or hardware reset, the power-on write protection
 *    must be reapplied.
 */
SDCC_STATUS
sdcc_handle_set_power_on_write_protection( struct sdcc_device *handle,
                                           UINT32 data_address,
                                           UINT32 num_of_blocks );

/** \details
* This function retrieves the type of write protection that is set for the different 
* write protection groups. Each set of two protection bits shows the type of protection
* set for each of the write protection groups. The definition of the different bit
* settings are shown below. The last (least significant) two bits of the protection 
* bits correspond to the first addressed group. 
* 
* b00: Write protection group not protected
* b01: Write protection group is protected by temporary write protection
* b10: Write protection group is protected by power-on write protection
* b11: Write protection group is protected by permanent write protection
*
* @param[in] handle
*    a pointer to the SDCC device that was returned from
*    sdcc_handle_open
* 
* @param[in] data_address
*    starting address of the write protection group to retrieve
*    the write protection bits
*                                           
* @param[out] write_prot_type
*    a pointer to an 8-byte array. This array contains 64 write protection bits
*    (representing 32 write protect groups starting at the specified address)
*
* @return  SDCC_NO_ERROR on success.  Error code otherwise
*/
SDCC_STATUS sdcc_handle_get_write_protection_type( struct sdcc_device *handle,
                                                   UINT32 data_address,
                                                   UINT8  *write_prot_type );

/** \details
 * This function reads \c num_blocks blocks of data from the \c data_address in
 * the device, storing them at the memory location given by \c buff.
 *
 * @param[in] handle
 *    a pointer to the SDCC device that was returned from \ref sdcc_handle_open
 *
 * @param[in] data_address
 *    start of the sector.  This is the sector unit address (512 bytes per
 *    sector).
 *
 * @param[out] buff
 *    pointer to buffer for the incoming data
 *
 * @param[in] num_blocks
 *    number of blocks to be read (512 bytes per block)
 *
 * @return SDCC_NO_ERROR if successful.  Error code otherwise.
 *
 * @note
 *    Multiblock transfer is used when more than 1 block data is transfered.
 */
SDCC_STATUS sdcc_handle_read( struct sdcc_device *handle,
                              UINT32              data_address,
                              UINT8               *buff,
                              UINT16              num_blocks );

/** \details
 * This function writes \c num_blocks blocks of data, obtained from the memory
 * location \c buff, to the \c data_address on the device.
 *
 * @param[in] handle
 *    a pointer to the SDCC device that was returned from \ref sdcc_handle_open
 *
 * @param[in] data_address
 *    start of the sector.  This is the sector unit address (512 bytes per
 *    sector).
 *
 * @param[in] buff
 *    pointer to buffer for the outgoing data
 *
 * @param[in] num_blocks
 *    number of blocks to be written (512 bytes per block)
 *
 * @return SDCC_NO_ERROR if successful.  Error code otherwise.
 *
 * \note
 *    Multiblock transfer is used when more than 1 block data is transfered.
 */
SDCC_STATUS sdcc_handle_write( struct sdcc_device *handle,
                               UINT32              data_address,
                               UINT8               *buff,
                               UINT16              num_blocks );

/** \details
 * This function is to erase data on the device, the commands used
 * to perform the erase operation should only be used on eMMC cards
 *
 * @param[in] handle
 *    a pointer to the SDCC device that was returned from \ref sdcc_handle_open
 *
 * @param[in] start_addr
 *    the address of the first sector to be erased
 *
 * @param[in] num_blocks
 *    number of blocks to be erased
 *
 * @return SDCC_NO_ERROR on success.  Error code otherwise
 */
SDCC_STATUS sdcc_handle_erase (struct sdcc_device *handle,
                               UINT32 start_addr,
                               UINT32 num_blocks );

/** \details
 *  This function is to securely erase data on the device, the commands used
 *  to perform the secure erase operation should only be used on eMMC cards
 *
 * @param[in] handle
 *    a pointer to the SDCC device that was returned from \ref sdcc_handle_open
 *
 * @param[in] start_addr
 *    the address of the first sector to be erased
 *
 * @param[in] num_blocks
 *    number of blocks to be erased
 *
 * @return SDCC_NO_ERROR on success.  Error code otherwise
 */
SDCC_STATUS sdcc_handle_secure_erase (struct sdcc_device *handle,
                               UINT32 start_addr,
                               UINT32 num_blocks );

/** \details
 * This function is to force erase data on the device, the commands used
 * to perform the erase operation should only be used on Locked cards
 *
 * @param[in] handle
 *    a pointer to the SDCC device that was returned from \ref sdcc_handle_open
 *
 * @return SDCC_NO_ERROR on success.  Error code otherwise
 */
SDCC_STATUS sdcc_handle_force_erase (struct sdcc_device *handle);

/** \details
 * This function configures the General Purpose Partition and 
 * enhanced user data area information
 * Note: This routine can only be used once for the life of each
 * emmc chip!
 *
 * @param[in] handle
 *    a pointer to the SDCC device that was returned from \ref sdcc_handle_open
 *
 * @param[in] gpp_enh_desc
 *    a pointer to the GPP/ENH description struction that contains partition info
 *
 * @return SDCC_NO_ERROR on success.  Error code otherwise
 */
SDCC_STATUS sdcc_config_emmc_gpp_enh (struct sdcc_device *handle,
                                      sdcc_emmc_gpp_enh_desc_type *gpp_enh_desc);

/** \details
 *  This function enable the hardware reset function provided by the MMC
 *  hardware.  Note, this function only work once.  Once reset is enabled
 *  on a MMC device, it can not be disabled in the future.  This is a 
 *  non-volatile update.
 *
 * @param[in] handle
 *    a pointer to the SDCC device that was returned from \ref sdcc_handle_open
 *
 * @return SDCC_NO_ERROR on success.  Error code otherwise
 */
SDCC_STATUS sdcc_enable_hw_reset (struct sdcc_device *handle);


/******************************************************************************
* Name: rpmb_read_counter_pkt
*
* Description:
* This function sends the Read Counter command and retrieves the Read Counter
* Response packet from the eMMC device. The Response packet can be used to
* retrieve the eMMC Write Counter.
*
* Arguments:
*    handle [IN]         : A pointer to the SDCC device that was returned
*                          from sdcc_handle_open()
*    RespPktBuffer [OUT] : A pointer to the Response packet for the  
*                          Read Counter Request
*
* Returns:
*    SDCC_NO_ERROR:             Successfully read the Counter packet
*    SDCC_ERR_INVALID_PARAM:    Bad pointer passed in
*    SDCC_ERR_CARD_UNDETECTED:  No eMMC card detected
*    SDCC_ERR_RPMB_RESPONSE:    Unexpected Response field of the Response packet
*    SDCC_ERR_RPMB_RESULT:      Error in the Result field of the Response packet
* 
******************************************************************************/
SDCC_STATUS rpmb_read_counter_pkt (struct sdcc_device *handle, 
                                   UINT32* RespPktBuffer);


/******************************************************************************
* Name: rpmb_read_sectors
*
* Description:
* This function reads the sectors from RPMB partition using the passed in cmd
* packet buffer. The data is returned in the DataBuffer.
*
* Arguments:
*    handle [IN]           : a pointer to the SDCC device that was returned
*                            from sdcc_handle_open()
*    CmdPktBuffer [IN]     : Pointer to a formatted packet for Read Request 
*    HalfSectorCount [IN]  : Number of half sectors to read.                        
*    DataBuffer [OUT]      : Pointer to data received from eMMC
* 
* Returns:
*    SDCC_NO_ERROR:             Successfully read the sectors 
*    SDCC_ERR_INVALID_PARAM:    Bad pointer passed in
*    SDCC_ERR_CARD_UNDETECTED:  No eMMC card detected
*    SDCC_ERR_RPMB_RESPONSE:    Unexpected Response field of the Response packet
*    SDCC_ERR_RPMB_NONCE:       The Nonce field of the Response does not
*                               match the Nonce sent in the Request packet
*    SDCC_ERR_RPMB_RESULT:      Error in the Result field of the Response packet
* 
******************************************************************************/
SDCC_STATUS rpmb_read_sectors (struct sdcc_device *handle, UINT32* CmdPktBuffer,
                               UINT32 HalfSectorCount, UINT32* DataBuffer);


/******************************************************************************
* Name: rpmb_write_sectors
*
* Description:
* This function writes the sectors to the RPMB partition. The data and RPMB
* command headers are pre-formated in the CmdPktBuffer. The Response packet
* from the eMMC device is passed back to the caller which may used
* to validate the next RPMB write.
*
* Arguments: 
*    handle [IN]          :  Pointer to the SDCC device that was returned
*                            from sdcc_handle_open()
*    CmdPktBuffer [IN]    :  Pointer to a formatted packet to be sent to eMMC
*    HalfSectorCount [IN] :  Number of half sectors to write
*    WriteType [IN]       :  Indicates if the RPMB write is the key provision
*                            write or data write
*    RespPktBuffer [OUT]  :  Pointer to the response packet retrieved from 
*                            the eMMC device
* 
* Returns:
*    SDCC_NO_ERROR:             Successfully write the sectors 
*    SDCC_ERR_INVALID_PARAM:    Bad pointer passed in
*    SDCC_ERR_CARD_UNDETECTED:  No eMMC card detected
*    SDCC_ERR_RPMB_RESPONSE:    Unexpected Response field of the Response packet
*    SDCC_ERR_RPMB_NONCE:       The Nonce field of the Response does not
*                               match the Nonce sent in the Request packt
*    SDCC_ERR_RPMB_RESULT:      Error in the Result field of the Response packet
******************************************************************************/
SDCC_STATUS rpmb_write_sectors (struct sdcc_device *handle, UINT32* CmdPktBuffer, 
                UINT32 HalfSectorCount, UINT32 WriteType, UINT32 *RespPktBuffer);

/******************************************************************************
 *  Name: sdcc_sdhci_mode_enable
 *
 * Description:
 *    This function enables the Host Controller specification.
 *    If the SDCC is not in SDHCi mode, it resets the SDCC's to SDHCi mode
 *
 * Arguments: None
 *
 * Returns:
 *	  SDCC_NO_ERROR if successful.	Error code otherwise
*
******************************************************************************/
SDCC_STATUS sdcc_sdhci_mode_enable (void);


/******************************************************************************
* Name: sdcc_handle_fw_update
*
* Description:
*    This function performs a firmware update using the fw data supplied.
*
* Arguments:
*    handle           [IN] : a pointer to the SDCC device that was returned
*                            from sdcc_handle_open()
*    buff             [IN] : pointer to buffer containing the firmware binary
*    num_blocks       [IN] : number of blocks to be written (512 bytes per
*                            block)
*
* Returns:
*    SDCC_NO_ERROR if successful
*    Error code otherwise
*
******************************************************************************/
SDCC_STATUS sdcc_handle_fw_update (struct sdcc_device *handle,
                                   UINT8 *buff, UINT16 num_blocks);


/******************************************************************************
* Name: sdcc_handle_get_ffu_status
*
* Description:
*    This function returns the Status of the Firmware Update Operation.
*
* Parameters:
*    handle        [IN] : a pointer to the SDCC device that was returned from
*                         sdcc_handle_open()
*    ffu_status    [IN/OUT] : Buffer to hold the FFU Operation status
*
* Returns:
*    SDCC_NO_ERROR if successful
*    Error code otherwise
*
******************************************************************************/
SDCC_STATUS sdcc_handle_get_ffu_status  (struct sdcc_device *handle,
                                         int *ffu_status);


/******************************************************************************
* Name: sdcc_handle_get_fw_version
*
* Description:
*    This function returns the device Firmware version.
*
* Parameters:
*    handle        [IN] : a pointer to the SDCC device that was returned from
*                         sdcc_handle_open()
*    fw_version    [IN/OUT] : Buffer to hold the FW Version
*
* Returns:
*    SDCC_NO_ERROR if successful
*    Error code otherwise
*
******************************************************************************/
SDCC_STATUS sdcc_handle_get_fw_version  (struct sdcc_device *handle,
                                         UINT64 *fw_version);

/******************************************************************************
* Name: sdcc_get_storage_info_raw_data
*
* Description:
*   This function gets the raw data of eMMC's Extended CSD, CID and CSD.  
*   If there is no memory device attached or device is not initialized,
*   all fields in the card specific information are zero.
*
* @remarks
*    \ref sdcc_handle_open() must be called before getting card info for the
*    specified SDCC handle.
*
* @param[in] handle
*    a pointer to the SDCC device that was returned from sdcc_handle_open()
* @param[out] emmc_info_raw
*    pointer to the buffer to save the device info raw data
* @param[in/out] *buff_len : pointer to the size of buffer on input.
*                            Upon exit, return the size of the data written
*
* Returns:
*    SDCC_NO_ERROR on success
*    error code otherwise
*
*****************************************?*************************************/
SDCC_STATUS sdcc_get_emmc_info_raw_data( struct sdcc_device *handle,
                                           void *emmc_info_raw, UINT32 *buff_len);

/******************************************************************************
* Name: sdcc_deinit_device
*
* Description:
*  This function de-selects the device, turn off the power supply to the device
*  and switches off the controller clock.
*
* Arguments:
*    driveno           [IN] :  a valid drive number between 0 and the total
*                              number of card slots - 1
*
* Returns:
*    SDCC_NO_ERROR if successful
*    Error code otherwise
*
******************************************************************************/
SDCC_STATUS sdcc_deinit_device (INT16 driveno);

/******************************************************************************
* Name: sdcc_get_device_status
*
* Description:
*    This function checks the device status
*
* Arguments:
*    driveno            [IN] : drive number to check
*    rc                 [OUT] : A pointer to SDCC status
* Returns:
*    return DEVICE_STATUS and SDCC_STATUS type value.
*
******************************************************************************/
DEVICE_STATUS sdcc_get_device_status (
   UINT32        driveno,
   SDCC_STATUS  *rc
);

/******************************************************************************
* Name: sdcc_sdhci_check_transfer_status
*
* Description:
*    This function checks the device status when transfer is in progress
*
* Arguments:
*    driveno            [IN] : drive number to check
*    rc                 [OUT] : A pointer to SDCC status
* Returns:
*    return DEVICE_STATUS and SDCC_STATUS type value.
*
******************************************************************************/
DEVICE_STATUS sdcc_sdhci_check_transfer_status (
   UINT32 driveno,
   SDCC_STATUS *rc
);

/******************************************************************************
* Name: sdcc_handle_request
*
* Description:
*    This function reads num_blocks blocks of data from the data_address in
*    the device, storing them at the memory location given by buff. This is 
*    for Block IO 2 only
*
* Arguments:
*    handle           [IN] : a pointer to the SDCC device that was returned
*                            from sdcc_handle_open()
*    data_address     [IN] : start of the sector.  This is the sector unit
*                            address (512 bytes per block).
*    buff            [OUT] : pointer to buffer for the incoming data
*    num_blocks       [IN] : number of blocks to be read (512 bytes per
*                            block)
*    type            [ IN] : can be read or write
*
* Returns:
*    SDCC_NO_ERROR if successful
*    Error code otherwise
*
* Note:
*    Multiblock transfer is used when more than 1 block data is transfered.
*
******************************************************************************/
SDCC_STATUS
sdcc_handle_request
(
   struct sdcc_device *handle,
   UINT32              data_address,
   UINT8              *buff,
   UINT16              num_blocks,
   OPERATION_TYPE      type
);

/******************************************************************************
 * Name: sdcc_complete_transfer
 *
 * Description:
 *    This function is used to wrap up data transfer and can only used in Block 
 *    IO 2 driver.  
 *
 * Arguments: 
 *
 * Returns:
 *    None
 *
 *****************************************************************************/
void sdcc_complete_transfer (void);
/*@}*/ /* group sdcc_handle_api */
#endif /* __SDCC_API_H */

