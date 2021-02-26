#ifndef __UFS_API_H
#define __UFS_API_H
/******************************************************************************
 * ufs_api.h
 *
 * This file provides UFS external API definitions.
 *
 * Copyright (c) 2012-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 *****************************************************************************/
/*=============================================================================

                        EDIT HISTORY FOR MODULE


when         who     what, where, why
----------   ---     ---------------------------------------------------
2017-05-22   jt      Add background initialization APIs 
2016-09-02   jt      Add write protect flags  
2015-09-22   rh      Adding the FFU interface API
2015-01-01   bh      Adding com_dtypes.h
2014-10-03   rh      Adding API for RPMB operation
2014-10-01   rh      Expose the info string reading API
2014-04-22   rh      Adding the pre-prog config option
2014-01-08   rh      Adding supported memory type bit field
2013-11-05   rh      Adding the format API
2013-09-09   rh      Adding a bitfield to indicate which LUN is enabled
2013-09-04   rh      Adding enhanced area capacity adjustment factor
2012-12-18   rh      Initial creation

=============================================================================*/

#include "com_dtypes.h"

/** \mainpage UFS Card Controller API Overview
 *
 * Universal Flash Storage (UFS) provides a simple set of public APIs for 
 * reading and writing to the memory device.  A second group of APIs provides 
 * access to Enhanced card capabilities.
 *
 */

/**
 * @defgroup ufs_driver UFS Card Driver 
 *
 * The UFS driver provides block-oriented data transfer to or from the memory
 * device. 
 */

/*@{*/

/**
 * The drive id are of int32 type.  UFS_DRIVEID_0 is the default card slot.
 */
#define UFS_DRIVEID_0   0

/**
 * The maximum transfer size per request
 */
#define UFS_MAX_BLOCK_TRANSFERS  16383  //SCSI_(10) 16K blocks @ 4K/block

/**
 * The maximum number of LUNs
 */
 #define UFS_MAX_LUNS            8

/**
 * Define all the basic WLUN type
 */
#define UFS_WLUN_REPORT          0x81
#define UFS_WLUN_DEVICE          0xD0
#define UFS_WLUN_BOOT            0xB0
#define UFS_WLUN_RPMB            0xC4

/**
 * Define all LUN that can be set as BOOTABLE
 */
#define UFS_LUN_BOOT_DISABLE     0x00
#define UFS_LUN_BOOT_A           0x01
#define UFS_LUN_BOOT_B           0x02

/**
 * Define the error code used for the public API
 */
#define UFS_EOK                  0
#define UFS_ENOENT               2
#define UFS_EIO                  5
#define UFS_ENOMEM               12
#define UFS_EBUSY                16
#define UFS_ENODEV               19
#define UFS_EINVAL               22
#define UFS_EPROTO               100
#define UFS_ETIME                101

/**
 * Define the supported memory type field
 */
#define SUPPORTED_MEM_TYPE_NORMAL            (1)
#define SUPPORTED_MEM_TYPE_CODE              (1 << 1)
#define SUPPORTED_MEM_TYPE_NON_PERSIST       (1 << 2)
#define SUPPORTED_MEM_TYPE_ENH1              (1 << 3)
#define SUPPORTED_MEM_TYPE_ENH2              (1 << 4)
#define SUPPORTED_MEM_TYPE_ENH3              (1 << 5)
#define SUPPORTED_MEM_TYPE_ENH4              (1 << 6)
#define SUPPORTED_MEM_TYPE_RPMB              (1 << 15)

/**
 * UFS device handel is a private data structure.  This opaque structure type is
 * declared so the APIs can access the memory device via a pointer to this
 * ufs_handle structure.
 */
// A data stucture used by the caller of the UFS driver as 
// a handle to access the APIs
typedef struct ufs_handle {
   uint32_t       id_magic;               // Magic # must be last
} ufs_handle_t;

/**
 * ufs_info_type data stucture is used to return UFS device specific
 * information back to the caller of the ufs_get_device_info API. The
 * name of each enteries in this data structure is identical to the one
 * used in the UFS specification for easy cross reference.
 */
struct ufs_info_type {
   uint8     fInitialized;           /**< 1 for initialized hardware */
   uint8     bLUN;                   /**< Current LUN number, BOOT LU is translated to normal LU */
   uint8     bNumberLu;              /**< Total number of active LU */
   uint16    wManufacturerID;        /**< Manufacturer ID */
   uint8     bBootEnable;            /**< 1 for device with BOOT partition enabled */
   uint64    qTotalRawDeviceCapacity;/**< Total RAW capacity of the device in unit of 512 bytes */
   uint8     bMinAddrBlockSize;      /**< Minimum block size in unit of 512 bytes */
   uint32    dSegmentSize;           /**< Erase block size */
   uint8     bAllocationUnitSize;    /**< Value expressed in number of segments */
   uint32    bRPMB_ReadWriteSize;    /**< Maximum RPMB transfer for each command, unit of 256 bytes */
   uint32    dNumAllocUnits;         /**< Number of allocation unit for this LU */
   uint8     bLogicalBlockSize;      /**< See Section 13.2.3 of UFS standard */
   uint8     bProvisioningType;      /**< >0 for enabling of thin provisioning */
   uint8     bLUWriteProtect;        /**< 1 for power on WP, 2 for permanent WP */
   uint8     bBootLunID;             /**< Which boot LUN is this LU mapped to */
   uint8     bMemoryType;            /**< >0 for enhanced memory type */
   uint32    dLuTotalBlocks;         /**< Calculated from dNumAllocUnits, unit in blocks */
   uint16    wSupportedMemoryTypes;  /**< Types of memory supported by device */
   uint32    dEnhanced1MaxNAllocU;   /**< Max allocation for Enhanced memory type 1 */
   uint16    wEnhanced1CapAdjFac;    /**< Capacity adjustment factor, Enh Memory type 1 */
   uint32    dEnhanced2MaxNAllocU;   /**< Max allocation for Enhanced memory type 2 */
   uint16    wEnhanced2CapAdjFac;    /**< Capacity adjustment factor, Enh Memory type 2 */
   uint32    dEnhanced3MaxNAllocU;   /**< Max allocation for Enhanced memory type 3 */
   uint16    wEnhanced3CapAdjFac;    /**< Capacity adjustment factor, Enh Memory type 3 */
   uint32    dEnhanced4MaxNAllocU;   /**< Max allocation for Enhanced memory type 4 */
   uint16    wEnhanced4CapAdjFac;    /**< Capacity adjustment factor, Enh Memory type 4 */
   uint32    dLunEnabled;            /**< Bit mask of which LUN is enabled, LSB = LU0 */
   uint8     iManufacturerName;      /**< Index for descriptor string - Manufacturer */
   uint8     iProductName;           /**< Index for descriptor string - ProductName */
   uint8     iSerialNumber;          /**< Index for descriptor string - SerialNumber */
   uint8     iOemID;                 /**< Index for descriptor string - OemID */
   uint8     inquiry_str[29];        /**< Output of inquiry command, include VID and PID and Rev */
   uint64    qLogicalBlockCount;     /**< Total number of addressable logic blocks */
   uint8     bConfigDescrLock;       /**< 1 for locked configuration descriptor */
   uint64    qHostBaseAddr;          /**< host base address */
   uint8     fPermanentWPEn;         /**< Permanent Write Protection Enable */
   uint8     fPowerOnWPEn;           /**< Power On Write Protection Enable */
   uint8     bDeviceEmbedded;        /**< 0- removable, 1 - embedded */
   uint8     bDeviceBootable;        /**< 0- non-bootable 1 - bootable */
};

/**
 * ufs_config_descr data structure reflects the UFS configuration descriptor
 * layout described by the UFS specification.  The structure contains
 * the user configurable field of the UFS configuration descriptor field
 * which is programmed once in the device's life to configure the LU layout
 */
struct ufs_unit_descr {
   uint8     bLUEnable;              /**< 1 for enabled LU */
   uint8     bBootLunID;             /**< >0 for using this LU for boot */
   uint8     bLUWriteProtect;        /**< 1 = power on WP, 2 = permanent WP */
   uint8     bMemoryType;            /**< >0 for enhanced memory type */
   uint32    dNumAllocUnits;         /**< Number of allocation unit for this LU */
   uint8     bDataReliability;       /**< >0 for reliable write support */
   uint8     bLogicalBlockSize;      /**< See section 13.2.3 of UFS standard */
   uint8     bProvisioningType;      /**< >0 for thin provisioning */
   uint16    wContextCapabilities;   /**< See Unit Descriptor Description of UFS standard */
};

struct ufs_config_descr {
   uint8     bNumberLU;              /**< Total number of active LUs */
   uint8     bBootEnable;            /**< 1 for enabling device for partial init */
   uint8     bDescrAccessEn;         /**< Can device descriptor be accessed during partial init */
   uint8     bInitPowerMode;         /**< Initial power mode of the device after reset */
   uint8     bHighPriorityLUN;       /**< LUN of the high priority LU */
   uint8     bSecureRemovalType;     /**< Erase configuration necessary for data removal */
   uint8     bInitActiveICCLevel;    /**< ICC level after reset */
   uint16    wPeriodicRTCUpdate;     /**< >0 to set a priodic RTC update rate */
   uint8     bConfigDescrLock;       /**< Set to 1 to lock the Configation Descriptor */
   uint32    qVendorConfigCode;      /**< Vendor specific configuration code */
   struct ufs_unit_descr unit[8];
};


/*@}*/ /* group ufs_driver */

/**
 * @defgroup ufs_api UFS API Overview
 * @ingroup ufs_driver
 *
 * The UFS API provides a list of public functions to UFS clients such as
 * Hotplug that perform data transfer to/from memory devices
 *
 */
/*@{*/

/** \details
 * This function first initializes the UFS controller.
 * It returns NULL if there is no memory device attached.  If there
 * is a memory device, it will initialize the device and take it out of the
 * partial initialized state.  It then sets up the device for data transfer.
 * A non-null pointer to the UFS handle is returned.
 *
 * @param[in] core_id
 *    Which hardware controller to use, in current design, only one UFS core
 *    is supported
 *
 * @param[in] lun
 *    Logic unit number.  The number is related to the LUN number
 *    available on a UFS card.  Each UFS device can have up to 128 LUNs and
 *    up to 128 well known LUNs
 *
 * @return A pointer to the UFS handle if successful.  NULL if failed.
 *
 */
struct ufs_handle *ufs_open( uint32 core_id, uint32 lun );

/** \details
 * This function free up the UFS handle. 
 *
 * @param[in] handle
 *    a pointer to the UFS handle that was returned from \ref ufs_open
 *
 * @return 0 if successful.  Error code if failed.
 */
int32 ufs_close( struct ufs_handle *handle );

/** \details
 * This function gets the memory card specific information.  If there is no
 * memory device attached or device is not initialized, all fields in the card
 * specific information are zero. 
 *
 * @param[in] handle
 *    a pointer to the UFS handle that was returned from \ref ufs_open
 *
 * @param[out] info
 *    pointer to the structure to save the memory card specific information
 *
 * @return 0 on success.  Error code otherwise
 */
int32 ufs_get_device_info( struct ufs_handle *handle, 
                           struct ufs_info_type *info );


/** \details
 * This function progam the one time programmable area of the UFS memory device
 * to configure the LU layout and configurations.  This function can only
 * be called for a blank UFS device that have not been configured before.
 *
 * @param[in] handle
 *    a pointer to the UFS handle that was returned from \ref ufs_open
 *
 * @param[out] cfg
 *    pointer to the structure that contain the one time programmable UFS
 *    device descriptor
 *
 * @return 0 on success.  Error code otherwise
 */
int32 ufs_configure_device( struct ufs_handle *handle, 
                            struct ufs_config_descr *cfg );


/** \details
 * This function sets active bootable flag to the LUN number indicated
 *
 * @param[in] handle
 *    a pointer to the UFS handle that was returned from \ref ufs_open
 *
 * @param[in] lun
 *    lun to enable for boot: 0 - Disable, 1 - BootA, 2 - BootB
 *
 * @return 0 on success.  Error code otherwise
 */
int32 ufs_set_bootable( struct ufs_handle *handle, uint32 lun );

/** \details
 * This function sets the power-on write protection bit of fPowerOnWPEn
 * Once the bit is set, all LUs marked as power on write protect will be
 * read only
 *
 * @param[in] handle
 *    a pointer to the UFS handle that was returned from \ref ufs_open
 *
 * @return 0 on success.  Error code otherwise
 *
 * @note
 *    After a power failure or hardware reset, the power-on write protection
 *    must be reapplied.
 */
int32 ufs_set_pon_write_protect( struct ufs_handle *handle);

/** \details
 * This function reads \c num_blocks blocks of data from the \c data_address from
 * the device, storing them at the memory location given by \c buff.
 *
 * @param[in] handle
 *    a pointer to the UFS handle that was returned from \ref ufs_open
 *
 * @param[out] buff
 *    pointer to buffer for the incoming data, data buffer must be 32bit aligned
 *
 * @param[in] lba
 *    Logic Block Address of the starting block to read from. 
 *
 * @param[in] num_blocks
 *    number of blocks to be read (4096 bytes per block)
 *
 * @return 0 if successful.  Error code otherwise.
 */
int32 ufs_read( struct ufs_handle *handle,
                uint8             *buff,
                uint64             lba,
                uint32             num_blocks );

/** \details
 * This function writes \c num_blocks blocks of data, obtained from the memory
 * location \c buff, to the \c data_address on the device.
 *
 * @param[in] handle
 *    a pointer to the UFS handle that was returned from \ref ufs_open
 *
 * @param[out] buff
 *    pointer to buffer for the incoming data, data buffer must be 32bit aligned
 *
 * @param[in] lba
 *    Logic Block Address of the starting block to write to.
 *
 * @param[in] num_blocks
 *    number of blocks to be read (4096 bytes per block)
 *
 * @return 0 if successful.  Error code otherwise.
 */
int32 ufs_write( struct ufs_handle *handle,
                 uint8             *buff,
                 uint64             lba,
                 uint32             num_blocks );

/** \details
 * This function is the non-blocking version of the ufs_read
 *
 * @param[in] handle
 *    a pointer to the UFS handle that was returned from \ref ufs_open
 *
 * @param[out] buff
 *    pointer to buffer for the incoming data, data buffer must be 32bit aligned
 *
 * @param[in] lba
 *    Logic Block Address of the starting block to read from. 
 *
 * @param[in] num_blocks
 *    number of blocks to be read (4096 bytes per block)
 *
 * @return 0 if successful.  Error code otherwise.
 *         returns -BUSY for read in progress
 */
int32 ufs_read_ex( struct ufs_handle *handle,
                   uint8             *buff,
                   uint64             lba,
                   uint32             num_blocks );

/** \details
 * This function is the non-blocking version of the ufs_write
 *
 * @param[in] handle
 *    a pointer to the UFS handle that was returned from \ref ufs_open
 *
 * @param[out] buff
 *    pointer to buffer for the incoming data, data buffer must be 32bit aligned
 *
 * @param[in] lba
 *    Logic Block Address of the starting block to write to.
 *
 * @param[in] num_blocks
 *    number of blocks to be read (4096 bytes per block)
 *
 * @return 0 if successful.  Error code otherwise.
 *         returns -BUSY for write in progress
 */
int32 ufs_write_ex( struct ufs_handle *handle,
                    uint8             *buff,
                    uint64             lba,
                    uint32             num_blocks );

/** \details
 * This function poll the hardware for transfer complete.  Returns 
 * the status of the pending read or write command.
 *
 * @param[in] handle
 *    a pointer to the UFS handle that was returned from \ref ufs_open
 *
 * @return 0 if successful.  Error code otherwise.
 *         returns -BUSY for operation in progress
 */
int32 ufs_poll_xfer( struct ufs_handle *handle );

/** \details
 * This function is to unmap data on the LU that is thinly provisioned
 *
 * @param[in] handle
 *    a pointer to the UFS handle that was returned from \ref ufs_open
 *
 * @param[in] lba
 *    the address of the first logic block to be unmapped from allocation table
 *
 * @param[in] num_blocks
 *    number of blocks to be unmapped (4096 bytes per block)
 *
 * @return 0 on success.  Error code otherwise
 */
int32 ufs_unmap (struct ufs_handle *handle,
                 uint64             lba,
                 uint32             num_blocks );

/** \details
 * This function is to format a LU and discard all data store on the LU
 *
 * @param[in] handle
 *    a pointer to the UFS handle that was returned from \ref ufs_open
 *
 * @return 0 on success.  Error code otherwise
 */
int32 ufs_format (struct ufs_handle *handle);

/** \details
 * This function sets the reference clock value.  The setting is one time
 * programmable throughout the life of the device
 *
 * @param[in] handle
 *    a pointer to the UFS handle that was returned from \ref ufs_open
 *
 * @param[in] freq
 *    reference clock frequency in hz, there are currently 4 possible value:
 *    19200000, 26000000, 38400000 and 52000000
 *    Use 0 for pulling the default device value base on BSP setting
 *
 * @return 0 on success.  Error code otherwise
 *
 */
int32 ufs_set_refclk_freq( struct ufs_handle *handle, 
                           uint32             freq);


/** \details
 * This function read back the write counter value using the RPMB
 * command packet
 *
 * @param[in] handle
 *    a pointer to the UFS handle that was returned from \ref ufs_open.
 *    RPMB API must use a handle that opened with the RPMB WLUN
 *
 * @param[out] resp
 *    the response packet from the RPMB operation, the response
 *    packet contain the write counter value
 *
 * @return 0 on success.  Error code otherwise
 *
 */
int32 ufs_rpmb_read_cnt_pkt (struct ufs_handle *handle, uint8 *resp);

/** \details
 * This function send a generic command though the RPMB protocal
 *
 * @param[in] handle
 *    a pointer to the UFS handle that was returned from \ref ufs_open.
 *    RPMB API must use a handle that opened with the RPMB WLUN
 *
 * @param[in] cmd_pkt
 *    the command packet sent for the RPMB operation
 *
 * @param[out] rsp_pkt
 *    the response packet from the RPMB operation 
 *
 * @return 0 on success.  Error code otherwise
 *
 */
int32 ufs_rpmb_send_cmd_pkt (struct ufs_handle *handle, 
                             uint8 *cmd_pkt, 
                             uint8 *rsp_pkt);

/** \details
 * Read multiple blocks of data using RPMB operation
 *
 * @param[in] handle
 *    a pointer to the UFS handle that was returned from \ref ufs_open.
 *    RPMB API must use a handle that opened with the RPMB WLUN
 *
 * @param[in] cmd_pkt
 *    the command packet sent for the RPMB operation
 *
 * @param[in] pkt_cnt
 *    number of packet to transfer
 *
 * @param[out] data_pkt
 *    buffer to hold data read from the RPMB operation
 *
 * @return 0 on success.  Error code otherwise
 *
 */
int32 ufs_rpmb_read_blocks (struct ufs_handle *handle, 
                            uint8 *cmd_pkt, 
                            uint32 pkt_cnt, 
                            uint8 *resp_pkt);

/** \details
 * Write multiple blocks of data using RPMB operation
 *
 * @param[in] handle
 *    a pointer to the UFS handle that was returned from \ref ufs_open.
 *    RPMB API must use a handle that opened with the RPMB WLUN
 *
 * @param[in] cmd_pkt
 *    the command packet sent for the RPMB operation
 *
 * @param[in] pkt_cnt
 *    number of packet to transfer
 *
 * @param[out] rsp_pkt
 *    the response packet from the RPMB operation 
 *
 * @return 0 on success.  Error code otherwise
 *
 */
int32 ufs_rpmb_write_blocks (struct ufs_handle *handle, 
                             uint8 *cmd_pkt, 
                             uint32 pkt_cnt, 
                             uint8 *resp_pkt);

/** \details
 * This function read the device information string. The string number
 * is obtained from the ufs_get_device_info API.  The available string are:
 * 
 *      iManufacturerName;     
 *      iProductName;          
 *      iSerialNumber;         
 *      iOemID;                
 *
 * @param[in] handle
 *    a pointer to the UFS handle that was returned from \ref ufs_open
 *
 * @param[in] id
 *    a value corresponding to the string ID code
 *
 * @param[in] buf
 *    a pointer to a buffer to hold the content of the string
 *
 * @param[in] size
 *    size of the output string buffer
 *
 * @return 0 on success.  Error code otherwise
 *
 * @note
 *    After a power failure or hardware reset, the power-on write protection
 *    must be reapplied.
 */
int32 ufs_get_device_info_str (struct ufs_handle *handle,
                               uint32           id,
                               char              *buf,
                               uint32           size);

/** \details
 * This function sets the perge enable bit fPurgeEnable and wait
 * until the hardware clears this bit.  Caling this function will 
 * purge all blocks that has been marked for discard
 *
 * @param[in] handle
 *    a pointer to the UFS handle that was returned from \ref ufs_open
 *
 * @return 0 on success.  Error code otherwise
 *
 * @note
 *    After a power failure or hardware reset, the power-on write protection
 *    must be reapplied.
 */
int32 ufs_purge_and_wait (struct ufs_handle *handle);

/** \details
 * This function can be called before the ufs_purge_and_wait function
 * to make the purge operation non-blocking
 *
 * @param[in] handle
 *    a pointer to the UFS handle that was returned from \ref ufs_open
 *
 * @return 0 on success.  -EBUSY if the device is still busy processing
 * a pending command and error code otherwise
 *
 */
int32 ufs_purge_nonblock_setup (struct ufs_handle *handle);

/** \details
 * This function perform the SCSI write buffer operation that is 
 * used for field firmware update
 *
 * @param[in] handle
 *    a pointer to the UFS handle that was returned from \ref ufs_open
 * @param[in] size
 *    size of the data buffer in bytes
 * @param[in] offset
 *    offset of the firmware address to write the current buffer to
 * @param[in] buf
 *    pointer to a buffer holding the new firmware image
 *
 * @return 0 on success.  Error code otherwise
 *
 */
int32 ufs_fw_update_write (struct ufs_handle *handle, int32_t size, 
                           int32_t offset, uint8_t *buf);

/** \details
 * This function starts the background initialization of the UFS
 * device.  
 *
 * @param[in] core_id
 *    Which hardware controller to use, in current design, only
 *    one UFS core is supported
 * @param[in] init_ctxt
 *    Initialization context data
 *
 * @return 0 on success.  Error code otherwise
 *
 */
int32 ufs_background_init_finish(uint32 core_id, uint32 init_ctxt);

/** \details
 * This function finishes the background initialization of the 
 * UFS device.
 *
 * @param[in] core_id 
 *    Which hardware controller to use, in current design, only
 *    one UFS core is supported
 * @param[in] init_ctxt
 *    Initialization context data
 * @return 0 on success.  Error code otherwise
 *
 */
int32 ufs_background_init_start(uint32 core_id, uint32 init_ctxt);

/*@}*/ /* group ufs_driver */
#endif /* __UFS_API_H */

