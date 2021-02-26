/******************************************************************************
**+-------------------------------------------------------------------------+**
**|                                                                         |**
**|                          Qualcomm Technologies INCORPORATED                          |** 
**|                                                                         |**
**|                                                                         |**
**|                Copyright (c) 2014-2017 Qualcomm Technologies, Inc      |**
**|                           ALL RIGHTS RESERVED                           |**
**|                                                                         |**
**|        No rights to reproduce, use, or disseminate this computer        |**
**|        program, whether in part or in whole, are granted.               |**
**|                                                                         |**
**+-------------------------------------------------------------------------+**
******************************************************************************/

#ifndef PBL_SBL_SHARED_H
#define PBL_SBL_SHARED_H
 /**===========================================================================
 **
 **                        PRIMARY BOOT LOADER
 **                        ------------------- 
 ** FILE 
 **     pbl_sbl_shared.h
 ** 
 ** GENERAL DESCRIPTION 
 **     This header file contains PBL-SBL shared data structure
 **     specific details.
 ** 
 **     Any new shared data that has to be passed to SBL should be
 **     added in this file. Any dependencies, like enums, defines etc ...
 **     that are supposed to be passed in this structure should be present
 **     in this file.
 ** 
 **     SBL code relies heavily on this structure and hence
 **     everytime there is a change in the shared data-structure
 **     (additions/deletions of fields/new values etc) ... structure
 **     version should be incremented appropriately.
 ** 
 **     Any new shared data field that has to be added has to be
 **     at the end of the structure.
 **
 **     NOTE:
 **     -----
 **
 **     ANY CHANGES INT THIS FILE OR ANY FIELDS RELATED TO THE 
 **     SHARED DATA STRUCTURE MUST BE REVIEWED WITH SBL IMAGE
 **     OWNERS.
 **
 **==========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module..
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/XBLLoaderLib/pbl_sbl_shared.h#1 $

  when       who         what, where, why
  --------   -------     ---------------------------------------------------
  05/03/17   kpa         Updates to support v2 bootrom shared data
  04/24/17   ds	         pbl_clock_apps_freq_type
  11/18/15   kpa         Updates to boot_apps_shared_data_type for PTO bootrom
  10/29/15   kpa         Updated for sdm845 pbl shared data structure
  10/29/15   plc         Updated for 8998 pbl shared data structure
  11/20/14   kpa         Added FORCE_DLOAD_MASK_V1 for dload cookie bit on chip version 1
  11/20/14   ck          Replaced PBL_TIMESTAMP_TIMER_FREQ_HZ with PS_PER_PBL_TIMESTAMP_TICK
  09/30/14   ck          Added boot_pbl_log_milestones
  09/30/14   ck          Added PBL_TIMESTAMP_TIMER_FREQ_HZ
  08/24/14   as          Initial version created
 
============================================================================*/


/******************************************************************************
                                MODULE INCLUDES
                         ADD NEW ONES UNDER THIS LINE
******************************************************************************/
#include "sec_img_auth.h"
#include "secboot_hw.h"
#include "XBLRamDump/boot_sahara.h"


/******************************************************************************
                             PUBLIC MACROS/DEFINES
                         ADD NEW ONES UNDER THIS LINE  
******************************************************************************/
/* Shared data structure Version */
/* Version Format: 0xMMMMNNNN
          MMMM : Major Version 
          NNNN : Minor Version */
#define PBL_APPS_SHARED_DATA_REVISION       0x00010002

/* Definitons for PBL to enter HS-USB or SD download mode. */
#define FORCE_DLOAD_BOOT_NONE       0x0
#define FORCE_DLOAD_BOOT_USB        0x1
#define FORCE_DLOAD_BOOT_SD_USB     0x2
#define FORCE_DLOAD_BOOT_SD         0x3

/* MASK for reading force download boot option. */
#define FORCE_DLOAD_MASK 0xF
#define FORCE_DLOAD_MASK_V1 0x000F0000

/******************************************************************************
                         PUBLIC TYPES and TYPE-DEFINES
                         ADD NEW ONES UNDER THIS LINE  
******************************************************************************/
#ifndef _ARM_ASM_

typedef struct pbl_secboot_shared_info_type
{
  secboot_verified_info_type pbl_verified_info;    /**<  */
  secboot_ftbl_type          pbl_secboot_ftbl;     /**< Contains pointers to PBL secboot routines */
  secboot_hw_ftbl_type       pbl_secboot_hw_ftbl;  /**< Contains pointers to PBL secboot hw routines */
  sec_img_auth_ftbl_type     pbl_sec_img_auth_ftbl;/**< Contains pointers to PBL sec_img_auth routines */
} pbl_secboot_shared_info_type;


/*
 * pbl_clock_sdcc_freq_type
 *
 */
typedef enum
{
  PBL_CLOCK_SDCC_FREQ_400KHZ,
  PBL_CLOCK_SDCC_FREQ_19P2MHZ,
  PBL_CLOCK_SDCC_FREQ_25MHZ,
  
  PBL_CLOCK_SDCC_FREQ_TOTAL,
  PBL_CLOCK_SDCC_FREQ_FORCE32BITS = 0x7FFFFFFF
} pbl_clock_sdcc_freq_type;

/*
 * pbl_clock_apps_freq_type
 *
 */
typedef enum
{
  PBL_CLOCK_APPS_FREQ_19P2MHZ  = 0,
  PBL_CLOCK_APPS_FREQ_600MHZ   = 1,
  PBL_CLOCK_APPS_FREQ_1200MHZ  = 2,

  PBL_CLOCK_APPS_FREQ_TOTAL,
  PBL_CLOCK_APPS_FREQ_FORCE32BITS = 0x7FFFFFFF
} pbl_clock_apps_freq_type;

/* Define Force Download Device type.
 
   If this field is anything other than FORCE_DLOAD_NONE
   then it indicates that boot option was ignored and
   corresponding force download option was taken to boot
   into SBL. */

typedef enum
{
  FORCE_DLOAD_NONE  = 0x0,
  FORCE_DLOAD_HS_USB_GPIO,
  FORCE_DLOAD_HS_USB_MAGIC_NUMBER,
  FORCE_DLOAD_SD_USB,
  FORCE_DLOAD_SD,
  FORCE_DLOAD_FORCE32BITS = 0x7FFFFFFF
} force_dload_type;

/* Define the FLASH types.
 
   Based on the boot option that is configured, this field
   will indicate the exact boot media that has been used to
   successfully load the SBL image.
 
   If control gets transferred to SBL then the field will
   not have NO_FLASH*/
typedef enum
{
  NO_FLASH        = 0,
  NOR_FLASH       = 1,
  NAND_FLASH      = 2,
  ONENAND_FLASH   = 3,
  SDC_FLASH       = 4,
  MMC_FLASH       = 5,
  SPI_FLASH       = 6,
  UFS_FLASH       = 7,
  RESERVED_1_FLASH= 8,
  RESERVED_2_FLASH= 9,
  USB_FLASH       =10,           /* this is not a real flash type, it's used only for sw_type */
  SIZE_FLASH_TYPE = 0x7FFFFFFF    /* Force 4 byte alignment */
} boot_flash_type;

/* Define the types of flash controllers that may be in use */
typedef enum
{
  NO_CTRL,                          /* Invalid controller     */
  NOR_CTRL,                         /* NOR controller         */
  NAND_CTRL,                        /* NAND controller        */
  SFLASH_CTRL,                      /* Sync flash controller          */
  SDC_CTRL,                         /* Secure Digital Card controller */
  SPI_CTRL,                         /* SPI controller         */
  UFS_CTRL,                         /* UFS controller         */
  USB_CTRL,                         /* USB controller         */
  SIZE_FLASH_CTRL_TYPE = 0x7FFFFFFF /* Force 4 byte alignment */
} boot_flash_ctrl_type;

/* Define the types of ECC Mode being used by NAND Controller */
typedef enum
{
  ECC_4_BIT_MODE         = 0,
  ECC_6_BIT_MODE,
  ECC_8_BIT_MODE,
  ECC_RESERVED_BIT_MODE_0,
  ECC_NOT_ENABLED,
  ECC_BIT_MODE_ENUM_COUNT,
  EBM_SIZE_ENUM          = 0x7FFFFFFF          /* Size enum to 32 bits. */
} boot_ecc_bit_mode_enum_type;

/* Defines the types of Boot Option is configured on the
   device. */
typedef enum
{
  BOOT_DEFAULT_OPTION = 0,              /* UFS HS G1-> SDC -> USB */
  BOOT_SDC_THEN_UFS_OPTION,             /* 1, SDC -> UFS */
  BOOT_SDC_OPTION,                      /* 2, SDC */
  BOOT_USB_OPTION,                      /* 3, USB */
  BOOT_QSPI_OPTION,                     /* 4, QSPI */
  BOOT_SELECT_OPTION_COUNT,
  BOOT_SDC_PORT2_THEN_USB_OPTION = 0x80,/* This is a Force dload option */
  BOOT_SDC_PORT2_OPTION          = 0x81,/* This is not a boot option, it is for error handler*/
  BOOT_OPTION_SIZE_ENUM          = 0x7FFFFFFF          /* Size enum to 32 bits. */
} boot_select_option_type;

/* SDC Flash shared data */
/* This defines the exact SDC device type that is detected */
typedef enum
{
  SDC_CARD_UNKNOWN = 0,
  SDC_CARD_SD      = 1,
  SDC_CARD_MMC     = 2,
  SDC_CARD_SDHC    = 5,
  SDC_CARD_MMCHC   = 6,  /* MMC card with higher than a density of 2GB */
  SDC_CARD_TYPE_SIZE = 0x7FFFFFFF
} boot_flash_sdc_card_type;

/* This defines SDC card states */
typedef enum
{ 
  SDC_CARD_IDLE = 0,
  SDC_CARD_READY,
  SDC_CARD_IDENT,
  SDC_CARD_STBY,
  SDC_CARD_TRAN,
  SDC_CARD_DATA,
  SDC_CARD_RCV,
  SDC_CARD_PRG,
  SDC_CARD_DIS,
  SDC_CARD_BTST,
  SDC_CARD_IGNORE = 0xFF,         /*dummy entry*/
  SDC_CARD_STATE_SIZE = 0x7FFFFFFF
} boot_flash_sdc_card_state_type;

/* This defines the SDC device memory access modes */
typedef enum
{
  SDC_MEM_BYTE_MODE,
  SDC_MEM_BLK_MODE,
  SDC_MEM_MODE_SIZE = 0x7FFFFFFF
} boot_flash_sdc_mem_mode_type;

/* This defines the MMC bus width */
typedef enum
{
  SDC_MMC_BUSWIDTH_1BIT = 0,
  SDC_MMC_BUSWIDTH_4BIT = 1,
  SDC_MMC_BUSWIDTH_8BIT = 2,
  SDC_MMC_BUSWIDTH_COUNT,
  SDC_MMC_BUSWIDTH_SIZE = 0x7FFFFFFF
} pbl_sdc_mmc_buswidth_type;

/* This defines the SDC Port used to boot */
/* !!!Warning!!! order of this port numbers should match with
   sdcc_reg[] array defined in sdcc driver files. sdcc_reg array
   defines base address for particular PORT and if below order
   doesn't match, you could see issues !!! */
typedef enum
{
  SDC_PORT1,
  SDC_PORT2,
  SDC_PORT_INVALID,
  SDC_PORT_SIZE = 0x7FFFFFFF
} boot_flash_sdc_port_type;

/*
 * pbl_clock_qspi_freq_type
 *
 */
typedef enum
{
  PBL_CLOCK_QSPI_FREQ_19MHZ = 0,
  PBL_CLOCK_QSPI_FREQ_75MHZ,
  PBL_CLOCK_QSPI_FREQ_120MHZ, 
  PBL_CLOCK_QSPI_FREQ_TOTAL,
  PBL_CLOCK_QSPI_FREQ_FORCE32BITS = 0x7FFFFFFF
} pbl_clock_qspi_freq_type;

/* Defines SDC memory information */
typedef struct
{
   uint32                        block_len;       /* Block size (aka the
                                                     sector size, in bytes) */
   uint32                        block_len_shft;  /* Block length shift bits*/
   boot_flash_sdc_mem_mode_type  block_mode;      /* transfer blocks or bytes */
   uint32                        spec_vers;       /* For MMC only: MMC System
                                                     Spec version */
   uint32                        phy_parti_size;  /* number of blocks */
} boot_flash_sdc_mem_type;


typedef struct
{
   uint32             phy_parti_size;  
} boot_flash_spi_mem_type;

typedef enum
{
  SPI_PORT,
  SPI_PORT_INVALID,
  SPI_PORT_SIZE = 0x7FFFFFFF
} boot_flash_spi_port_type;

/* This defines the MMC bus width */ 
typedef enum
{
  SPI_BUSWIDTH_1BIT = 0,
  SPI_BUSWIDTH_4BIT = 1,
  SPI_BUSWIDTH_8BIT = 2,
  SPI_BUSWIDTH_COUNT,
  SPI_BUSWIDTH_SIZE = 0x7FFFFFFF
} pbl_spi_buswidth_type;

/* This is the main structure that contains
   NAND Flash specific shared data */
typedef struct
{
  uint32                      page_size_bytes;
  uint32                      page_size_shift;
  uint32                      block_size_pages;
  uint32                      block_size_shift;
  boot_ecc_bit_mode_enum_type ecc_mode;
} boot_flash_shared_nand_dev_type;

/* This is the main structure that contains
   SDC Flash specific shared data */
typedef struct
{
   boot_flash_sdc_mem_type         mem;         /* info pertaining to memory device */
   boot_flash_sdc_card_type        card_type;   /* card type  */
   boot_flash_sdc_card_state_type  card_state;  /* card state */
   uint32                          rca;         /* the card's relative address */
   pbl_clock_sdcc_freq_type        mclk;        /* current host clock speed */
   boot_flash_sdc_port_type        port;        /* Port/Slot the device was detected */
   uint32                          boot_cfg;    /* Specify boot parition */
   pbl_sdc_mmc_buswidth_type       data_width;  /* data_width ( 1/4/8 bit) */ 
   boolean                         is_gpt_parti;
   boolean                         is_primary_table;
   uint64                          starting_lba;/* starting LBA of SBL partition */
   uint64                          ending_lba;  /* Last LBA of SBL partition */
} boot_flash_shared_sdc_dev_type;

typedef struct
{
  uint32                          chip_select;
  uint32                          addr_len;
  boot_flash_spi_port_type        port;        /* BLSP-QUP SPI Port where device was detected */
  pbl_spi_buswidth_type           data_width;  /* data_width ( 1/4/8 bit) */ 
  boot_flash_spi_mem_type         mem;         /* info pertaining to memory device */
  boolean                         is_gpt_parti;
  boolean                         is_primary_table;
  uint64                          starting_lba;/* starting LBA of SBL partition */
  uint64                          ending_lba;  /* Last LBA of SBL partition */
  pbl_clock_qspi_freq_type        spi_core_clock; /* SPI core clock. spi bus clock is
                                                     (spi core clock)/2 */
} boot_flash_shared_spi_dev_type;


/* This defines Flash shared data */
typedef struct
{
  boot_flash_type       type;        /* Type of flash                      */
  uint32                CS_base;     /* Base address of chip select device
                                        is connected to.                   */
  uint32                data_width;  /* Bit width of device ie: 8bit/16bit */
  boot_flash_ctrl_type  ctrl_type;   /* Controller used to interface with
                                        device                             */
  union
  {
    boot_flash_shared_nand_dev_type nand;  /* Shared info for NAND device  */
    boot_flash_shared_sdc_dev_type  sdc;   /* Shared info for SDC device   */
    boot_flash_shared_spi_dev_type  spi;   /* Shared info for SPI device   */
    struct boot_sahara_shared_data* sahara;/* Shared info pointer for Sahara flashless boot/eDL */
  }dev_info;
}boot_flash_shared_dev_info_type;

typedef struct
{
  uint32 pbl_entry_timestamp;
  uint32 bootable_media_detect_entry_timestamp;
  uint32 bootable_media_detect_success_timestamp;
  uint32 elf_loader_entry_timestamp;
  uint32 auth_hash_seg_entry_timestamp;
  uint32 auth_hash_seg_exit_timestamp;
  uint32 elf_segs_Hash_verify_entry_timestamp;
  uint32 elf_segs_Hash_verify_exit_timestamp;
  uint32 auth_xbl_sec_hash_seg_entry_timestamp;
  uint32 auth_xbl_sec_hash_seg_exit_timestamp;
  uint32 xbl_sec_segs_Hash_verify_entry_timestamp;
  uint32 xbl_sec_segs_Hash_verify_exit_timestamp;
  uint32 pbl_exit_timestamp;
} pbl_timestamp_type;

/* Defines the main data structure that is shared
   with the APPs SBL image. */
typedef struct boot_apps_shared_data_type
{
  /* Fields below are filled in by initialization function */
  /* Shared data structure version */
  uint32                          shared_data_version;

  /* PBL version */
  uint32                          pbl_version;
  
  /* Apps processor clock speed set up in PBL */
  uint32                          proc_clk_speed;

  /* PBL Boot option configured in the fuse */
  boot_select_option_type         pbl_boot_option;

  /* Force download option if there was a force dload boot */
  force_dload_type                force_dload_option;
  
  /* Page table base address */
  uint64                          *pbl_page_tbl_base;

  /* Page table size */
  uint32                          pbl_page_tbl_size;

  /* PBL stack base */
  uint8                           *boot_stack_base;

  /* PBL stack size */
  uint32                          boot_stack_size;

  /* The region of memory required by PBL shared data. */
  /* PBL Shared data section base address */
  uint8                           *shared_section_base;

  /* PBL Shared data section size */
  uint32                          shared_section_size;

  /* Fields below are filled in by flash module. This is a pointer
   * that points to a structure in PBL scratch memory. SBL needs to
   *  make its own deep copy of this structure */
  boot_flash_shared_dev_info_type *flash_shared_data;

  /* Fields below are filled in by auth module */
  pbl_secboot_shared_info_type    *secboot_shared_data;

  /*Timestamps at main milestones in PBL*/
  pbl_timestamp_type              timestamps;

  /* This is the pointer to the ELF image imformation */
  sec_img_auth_elf_info_type      elf_meta_info;

  /* XBL loader authentication state */
  uint32                          xbl_loader_auth_enabled;                   

  /* XBL loader hash integrity check state */
  uint32                          hash_integrity_check_enabled;  

  /* XBL-Sec boot flow support state */
  uint32                          xbl_sec_boot_flow_supported;

  /* XBL-Sec authentication state */
  uint32                          xbl_sec_auth_disabled;  

  /* PBL looks for a hash segment in the ELF image and,
       mandates image hash integrity check based on Sec_bootn and Hash integrity check fuse values*/
  uint32                          pbl_mandate_image_hash_integrity_check;
  
  /* PBL Patch version */
  uint32                          pbl_patch_version;
  
  /* Total number of bytes ready by PBL from ELF (XBL + XBL-sec).

     Added for second version (v2) of the chip. please do not dereference for
     v1. [num_of_bytes_read_from_elf is unused in v1. added for v2
     compatibility]. logic does not support dynamically switching structures,
     and we do not plan to add support it, since upcoming chips wont have this issue
  */
  uint32                          num_of_bytes_read_from_elf;  

} boot_pbl_shared_data_type;


/******************************************************************************
                            PUBLIC DATA DECLARATION
                         ADD NEW ONES UNDER THIS LINE  
******************************************************************************/
/* Timestamps from PBL are given to SBL as ticks of a timer.  On some
   processors floating point might not be available so calculate
   PS_PER_PBL_TIMESTAMP_TICK (Pico seconds).  The value is not currently passed
   to SBL so it must be hardcoded here. */
#define PS_PER_PBL_TIMESTAMP_TICK   52083  /* 19.2Mhz */


/******************************************************************************
                         PUBLIC FUNCTION DECLARATIONS
                         ADD NEW ONES UNDER THIS LINE  
******************************************************************************/
/*=========================================================================

**  Function :  boot_pbl_get_pbl_version

** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns the PBL version from PBL shared data.
* 
* @par Dependencies
*   None
*   
* @retval
*   uint32
* 
* @par Side Effects
*   None
* 
*/
uint32 boot_pbl_get_pbl_version
(
  void
);

/*=========================================================================

**  Function :  boot_pbl_get_clock_speed

** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns the clock speed set by PBL.
* 
* @par Dependencies
*   None
*   
* @retval
*   uint32
* 
* @par Side Effects
*   None
* 
*/
uint32 boot_pbl_get_clock_speed
(
  void
);


/*===========================================================================

**  Function :  boot_pbl_get_flash_shared_data

** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns the pointer to the flash related information found
*   by PBL.
* 
* @par Dependencies
*   None
* 
* @retval
*   Pointer of boot_flash_shared_dev_info_type type
* 
* @par Side Effects
*   None.
* 
*/
boot_flash_shared_dev_info_type* boot_pbl_get_flash_shared_data
(
  void
);


/*===========================================================================

**  Function :  boot_pbl_is_auth_enabled

** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns whether PBL detected that auth was enabled.
* 
* @par Dependencies
*   None
* 
* @retval
*   boot_boolean
* 
* @par Side Effects
*   None.
* 
*/
boot_boolean boot_pbl_is_auth_enabled
(
  void
);


/*===========================================================================

**  Function :  boot_pbl_get_flash_type

** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns the flash type detected by PBL
* 
* @par Dependencies
*   None
* 
* @retval
*   boot_flash_type
* 
* @par Side Effects
*   None.
* 
*/
boot_flash_type boot_pbl_get_flash_type
(
  void
);


/*===========================================================================

**  Function :  boot_pbl_interface_init

** ==========================================================================
*/
/*!
* 
* @brief
*   Function that will initialize the target interface so that generic SBL
*   code can use it.
* 
* @par Dependencies
*   None
* 
* @retval
*   boot_boolean
* 
* @par Side Effects
*   None
* 
*/
boot_boolean boot_pbl_interface_init
(
  boot_pbl_shared_data_type   *pbl_shared_data  /* Pointer to data coming from PBL */
);


/*===========================================================================

**  Function :  boot_pbl_log_milestones

** ==========================================================================
*/
/*!
* 
* @brief
*   This funcion will parse the PBL timestamp milestones passed to SBL
*   and insert them into the boot log.  Currently PBL's unit of measure is
*   clock ticks.  PBL does not pass the clock frequency yet so it is hard
*   wired to 19.2 Mhz.  Also PBL does not pass the names of each field so for
*   now reference a structure of our own to get the names which will have
*   logic ready for the day PBL starts passing them.
*
* @param[in]
*   boot_pbl_shared_data_type
*
* @par Dependencies
*   None
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_pbl_log_milestones(boot_pbl_shared_data_type * pbl_shared_data);


#endif /* _ARM_ASM_ */

#endif /* PBL_SBL_SHARED_H */
/*=============================================================================
                                  END OF FILE
=============================================================================*/

