#ifndef _TARGET_CUST_H
#define _TARGET_CUST_H

/*===========================================================================

                                Target_cust
                                Header File

GENERAL DESCRIPTION
  This header file contains target specific declarations and definitions

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2014 - 2017  2020 by Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who          what, where, why
--------   --------     ----------------------------------------------------------
02/12/20   anjir        increase mem size
09/22/17   rohik        Added raw partition size and buffer
08/14/17   yps         Increase 1KB for  boot log ram buffer
08/10/17   yps         Increase 6KB for  SCL_SBL1_DATA_ZI_SIZE and SCL_SBL1_DATA_SIZE 
07/12/17   ds           add remaining aop msg ram regions
07/07/17   rp           DCB buffer changes
07/05/17   dcf          Increase SCL_SBL1_DATA_ZI_SIZE by 4k.
06/21/17   yps          Added SBL_IMEM_RANGES_TABLE
06/08/17   ds			V6 memmap changes
06/06/17   ds			Define PBL region for unmapping
05/23/17   vk           Move size macro to new file
03/06/17   kpa          Increase SCL_SBL1_DATA_ZI_SIZE
03/02/17   ds		    shrm mem and ipa memory updates
03/01/17   kpa          Added SCL_SBL1_VECTOR_TABLE_SIZE
02/28/17   ds           update AOP ram sizes
02/22/17   ds           ddr apps region for abl loading
02/15/17   rp           Added a data buffer for XBL CFG inside DSF section and adjusted sizes
01/30/17   ds           unused region define for unmapping to avoid speculative access
09/19/16   kpa          Update OCIMEM buffer size
07/19/16   kpa          Initial version for SDM845 with memory map update
============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/*===========================================================================
              DEFINES FOR TARGET (equivalent to <target>.builds src)
===========================================================================*/

#include "../../XBLLoader/Include/SizeInfo.h"

#define SCL_IMEM_BASE                    0x14680000
#define SCL_IMEM_SIZE                    SIZE_256KB

#define EXCEPTION_HANDLER_SIZE            SIZE_128B
#define EXCEPTION_HANDLER_NUM             16
#define SCL_SBL1_VECTOR_TABLE_SIZE       (EXCEPTION_HANDLER_NUM * EXCEPTION_HANDLER_SIZE)

/* AOP region defines */
#define SCL_AOP_CODE_RAM_BASE            0x0B000000
#define SCL_AOP_CODE_RAM_SIZE            SIZE_96KB

#define SCL_AOP_DATA_RAM_BASE            0x0B0E0000
#define SCL_AOP_DATA_RAM_SIZE            SIZE_32KB

#define SCL_AOP_MSG_RAM0_BASE            0x0C300000
#define SCL_AOP_MSG_RAM1_BASE            SCL_AOP_MSG_RAM0_BASE + SIZE_64KB
#define SCL_AOP_MSG_RAM2_BASE            SCL_AOP_MSG_RAM1_BASE + SIZE_64KB
#define SCL_AOP_MSG_RAM3_BASE            SCL_AOP_MSG_RAM2_BASE + SIZE_64KB
#define SCL_AOP_MSG_RAM4_BASE            SCL_AOP_MSG_RAM3_BASE + SIZE_64KB
#define SCL_AOP_MSG_RAM5_BASE            SCL_AOP_MSG_RAM4_BASE + SIZE_64KB
#define SCL_AOP_MSG_RAM6_BASE            SCL_AOP_MSG_RAM5_BASE + SIZE_64KB
#define SCL_AOP_MSG_RAM7_BASE            SCL_AOP_MSG_RAM6_BASE + SIZE_64KB
#define SCL_AOP_MSG_RAM8_BASE            SCL_AOP_MSG_RAM7_BASE + SIZE_64KB
#define SCL_AOP_MSG_RAM9_BASE            SCL_AOP_MSG_RAM8_BASE + SIZE_64KB
#define SCL_AOP_MSG_RAM10_BASE           SCL_AOP_MSG_RAM9_BASE + SIZE_64KB
#define SCL_AOP_MSG_RAM11_BASE           SCL_AOP_MSG_RAM10_BASE + SIZE_64KB
#define SCL_AOP_MSG_RAM12_BASE           SCL_AOP_MSG_RAM11_BASE + SIZE_64KB
#define SCL_AOP_MSG_RAM13_BASE           SCL_AOP_MSG_RAM12_BASE + SIZE_64KB
#define SCL_AOP_MSG_RAM14_BASE           SCL_AOP_MSG_RAM13_BASE + SIZE_64KB
#define SCL_AOP_MSG_RAM15_BASE           SCL_AOP_MSG_RAM14_BASE + SIZE_64KB
#define SCL_AOP_MSG_RAMX_SIZE            SIZE_1KB

#define SCL_AOP_IMG_DDR_BASE             0x85FC0000
#define SCL_AOP_IMG_DDR_SIZE             SIZE_128KB

#define SCL_AOP_CMD_DB_BASE              0x85FE0000
#define SCL_AOP_CMD_DB_SIZE              SIZE_128KB

/* SDC region defines */
#define SCL_SDC_CODE_RAM_BASE            SCL_AOP_CODE_RAM_BASE  //TODO:_FIX_ get correct addr
#define SCL_SDC_CODE_RAM_SIZE            SIZE_160KB
#define SCL_SDC_DATA_RAM_BASE            SCL_AOP_CODE_RAM_BASE  //TODO:_FIX_ get correct addr
#define SCL_SDC_DATA_RAM_SIZE            SIZE_80KB
#define SCL_SDC_PRAM_BASE                SCL_AOP_CODE_RAM_BASE  //TODO:_FIX_ get correct addr
#define SCL_SDC_PRAM_SIZE                SIZE_26KB

/* BOOT IMEM region defines */
#define SCL_BOOT_IMEM_BASE_ADDR          0x14800000
#define SCL_BOOT_IMEM_BASE_SIZE          SIZE_1_5_MB
#define SCL_BOOTROM_MEMORY_REGION        SIZE_204KB
#define SCL_PRE_DDR_PAGE_TABLE_SIZE      SIZE_48KB
#define SCL_SBL1_IMAGE_BASE              (SCL_BOOT_IMEM_BASE_ADDR + SCL_BOOTROM_MEMORY_REGION + SCL_PRE_DDR_PAGE_TABLE_SIZE)
#define SCL_SBL1_IMAGE_SIZE              SIZE_512KB
#define SCL_SBL1_TOTAL_SIZE              SCL_SBL1_IMAGE_SIZE
                                         
/* XBL Loader DDR DSF Driver memory map defines */
#define SCL_SBL1_DDR_DSF_ROM_BASE        (SCL_SBL1_IMAGE_BASE + SCL_SBL1_IMAGE_SIZE )
#define SCL_SBL1_DSF_ROM_SIZE            SIZE_92KB
#define SCL_SBL1_DDI_ROM_BASE            (SCL_SBL1_DDR_DSF_ROM_BASE + SCL_SBL1_DSF_ROM_SIZE)
#define SCL_SBL1_DDI_ROM_SIZE            SIZE_12KB
#define SCL_SBL1_DDR_DSF_ROM_SIZE        (SCL_SBL1_DSF_ROM_SIZE + SCL_SBL1_DDI_ROM_SIZE)
#define SCL_SBL1_DDR_DSF_DATA_BASE       (SCL_SBL1_DDR_DSF_ROM_BASE + SCL_SBL1_DDR_DSF_ROM_SIZE)
#define SCL_SBL1_DSF_DATA_SIZE           SIZE_32KB
#define SCL_SBL1_DDI_DATA_BASE           (SCL_SBL1_DDR_DSF_DATA_BASE + SCL_SBL1_DSF_DATA_SIZE)
#define SCL_SBL1_DDI_DATA_SIZE           SIZE_2KB
#define SCL_SBL1_DDR_DSF_DATA_SIZE       (SCL_SBL1_DSF_DATA_SIZE + SCL_SBL1_DDI_DATA_SIZE)
#define SCL_SBL1_DDR_STRUCT_BASE         (SCL_SBL1_DDR_DSF_DATA_BASE + SCL_SBL1_DDR_DSF_DATA_SIZE)
#define SCL_SBL1_DDR_STRUCT_SIZE         SIZE_6KB
#define SCL_SBL1_DDR_DSF_SEGMENT_SIZE    SIZE_148KB
#define SCL_SBL1_DDR_SHRM_STRUCT_BASE    0x13E0000
#define SCL_SBL1_DDR_SHRM_STRUCT_SIZE    SIZE_4KB
#define SCL_SBL1_DDR_DSF_CFG_BUF_BASE    SCL_AOP_CODE_RAM_BASE
#define SCL_SBL1_DDR_DSF_CFG_BUF_SIZE    SIZE_20KB

/* Training data size needs to be defined to be used during ramdump */
#define SCL_DDR_TRAINING_DATA_BUF_SIZE   SIZE_8KB

/* Cache flush table before XBL Loader mmu initialization */
#define SBL_IMEM_RANGES_TABLE  \
    {\
     {SCL_SBL1_OCIMEM_DATA_BASE,    SCL_SBL1_OCIMEM_DATA_SIZE},\
     {SCL_BOOT_IMEM_BASE_ADDR,     SCL_BOOT_IMEM_BASE_SIZE}\
    }

/* XBL Loader TCM memory map defines */
#define SCL_SBL1_CODE_BASE               SCL_SBL1_IMAGE_BASE
#define SCL_SBL1_CODE_SIZE               SIZE_328KB
                                         
#define SCL_SBL1_VECTOR_BASE             SCL_SBL1_CODE_BASE
#define SCL_SBL1_VECTOR_SIZE             0x00000020
                                         
#define SCL_SBL1_STACK_BASE              (SCL_SBL1_CODE_BASE + SCL_SBL1_CODE_SIZE)
#define SCL_SBL1_STACK_SIZE              SIZE_12KB

//Base address of stack is higher end-address of stack region
#define SBL1_STACK_BASE                  (SCL_SBL1_STACK_BASE + SCL_SBL1_STACK_SIZE)

#define SCL_SBL1_DATA_BASE               (SCL_SBL1_STACK_BASE + SCL_SBL1_STACK_SIZE)
#define SCL_SBL1_DATA_SIZE               SIZE_48KB
#define SCL_SBL1_DATA_ZI_BASE            (SCL_SBL1_DATA_BASE + SCL_SBL1_DATA_SIZE)
#define SCL_SBL1_DATA_ZI_SIZE            SIZE_80KB
                                         
#define SCL_SBL1_PAGE_TABLE_BASE         (SCL_SBL1_DATA_ZI_BASE + SCL_SBL1_DATA_ZI_SIZE)
#define SCL_SBL1_PAGE_TABLE_SIZE         SIZE_20KB

#define SCL_SBL1_SHARED_FUNCTIONS_TABLE_BASE (SCL_SBL1_PAGE_TABLE_BASE + SCL_SBL1_PAGE_TABLE_SIZE)
#define SCL_SBL1_SHARED_FUNCTIONS_TABLE_SIZE  SIZE_4KB

/* RPM Code RAM memory map defines for DDR training image */
#define SCL_DDR_TRAINING_CODE_BASE       (SCL_SBL1_DDR_DSF_ROM_BASE + SCL_SBL1_DDR_DSF_SEGMENT_SIZE)
#define SCL_DDR_TRAINING_CODE_SIZE       SIZE_66KB
#define SCL_DDR_TRAINING_DATA_BASE       (SCL_DDR_TRAINING_CODE_BASE + SCL_DDR_TRAINING_CODE_SIZE)
#define SCL_DDR_TRAINING_DATA_SIZE       SIZE_8KB
#define SCL_DDR_TRAINING_DATA_ZI_BASE    (SCL_DDR_TRAINING_DATA_BASE + SCL_DDR_TRAINING_DATA_SIZE)
#define SCL_DDR_TRAINING_DATA_ZI_SIZE    SIZE_6KB
#define SCL_DDR_TRAINING_SIZE            SIZE_80KB 
                                         
#define SCL_TZ_OCIMEM_BUFFER             SIZE_184KB
                                         
#define SCL_SYSTEM_DEBUG_BASE            (SCL_IMEM_BASE + SCL_TZ_OCIMEM_BUFFER)
#define SCL_SYSTEM_DEBUG_SIZE            SIZE_16KB
#define SCL_SYSTEM_DEBUG_CO_RO_BASE      (SCL_SYSTEM_DEBUG_BASE)
#define SCL_SYSTEM_DEBUG_CO_RO_SIZE      SIZE_12KB
#define SCL_SYSTEM_DEBUG_DATA_BASE       (SCL_SYSTEM_DEBUG_BASE + SCL_SYSTEM_DEBUG_CO_RO_SIZE)
#define SCL_SYSTEM_DEBUG_QSEE_INFO_SIZE  SIZE_1KB
#define SCL_SYSTEM_DEBUG_DATA_SIZE       (SCL_SYSTEM_DEBUG_SIZE - SCL_SYSTEM_DEBUG_CO_RO_SIZE - SCL_SYSTEM_DEBUG_QSEE_INFO_SIZE )
#define SCL_SYSTEM_DEBUG_QSEE_INFO_BASE  (SCL_SYSTEM_DEBUG_DATA_BASE + SCL_SYSTEM_DEBUG_DATA_SIZE)
                                         
#define SCL_SBL1_OCIMEM_DATA_BASE        (SCL_IMEM_BASE + SCL_TZ_OCIMEM_BUFFER + SCL_SYSTEM_DEBUG_SIZE)
// TBD: To reduce SCL_SBL1_OCIMEM_DATA_SIZE size further
#define SCL_SBL1_OCIMEM_DATA_SIZE        SIZE_24KB

//Define unused address spaces so that we can unmap as required
#define SCL_UNUSED_ADDRESS1_BASE          0x146C0000
#define SCL_UNUSED_ADDRESS1_SIZE          0x140000

#define SCL_TZ_SDI_MAPPED_REGION         0x146AC000
#define SCL_TZ_SDI_MAPPED_REGION_SIZE    0x2000

#define SCL_PBL_REGION_DEFAULT_MAPPED      0x33C000
#define SCL_PBL_REGION_DEFAULT_MAPPED_SIZE 0xC4000

#define MMU_POST_RPM_MEM_REGION_BASE     (SCL_SYSTEM_DEBUG_BASE + SCL_SYSTEM_DEBUG_SIZE)
#define MMU_POST_RPM_MEM_REGION_SIZE     (0x0100000 - SCL_RPM_CODE_RAM_SIZE - SCL_SYSTEM_DEBUG_SIZE)
#define SHARED_IMEM_SIZE                 SIZE_4KB
#define SHARED_IMEM_BASE                 (SCL_IMEM_BASE + SCL_IMEM_SIZE - SHARED_IMEM_SIZE)
#define SHARED_IMEM_BOOT_BASE            SHARED_IMEM_BASE
#define SHARED_IMEM_BOOT_SIZE            200
#define SHARED_IMEM_USB_BASE             (SHARED_IMEM_BOOT_BASE + SHARED_IMEM_BOOT_SIZE)
#define SHARED_IMEM_USB_SIZE             200
#define SHARED_IMEM_BOOT_CDT_BASE        (SHARED_IMEM_USB_BASE + SHARED_IMEM_USB_SIZE)
#define SHARED_IMEM_BOOT_CDT_SIZE        0x0000400
#define SHARED_IMEM_DDR_BASE             (SHARED_IMEM_BOOT_CDT_BASE + SHARED_IMEM_BOOT_CDT_SIZE)
#define SHARED_IMEM_DDR_SIZE             200
#define SHARED_IMEM_HLOS_BASE            (SHARED_IMEM_DDR_BASE + SHARED_IMEM_DDR_SIZE)
#define SHARED_IMEM_HLOS_SIZE            200
#define SHARED_IMEM_RPM_BASE             (SHARED_IMEM_HLOS_BASE + SHARED_IMEM_HLOS_SIZE)
#define SHARED_IMEM_RPM_SIZE             8
#define SHARED_IMEM_QDSS_BASE            (SHARED_IMEM_RPM_BASE + SHARED_IMEM_RPM_SIZE)
#define SHARED_IMEM_QDSS_SIZE            12
#define SHARED_IMEM_SECUREMSM_BASE       (SHARED_IMEM_QDSS_BASE + SHARED_IMEM_QDSS_SIZE)
#define SHARED_IMEM_SECUREMSM_SIZE       512
#define SHARED_IMEM_PERIPHERAL_DEBUG_BASE (SHARED_IMEM_SECUREMSM_BASE + SHARED_IMEM_SECUREMSM_SIZE)
#define SHARED_IMEM_PERIPHERAL_DEBUG_SIZE 24
#define SHARED_IMEM_PIL_BASE             (SHARED_IMEM_PERIPHERAL_DEBUG_BASE + SHARED_IMEM_PERIPHERAL_DEBUG_SIZE)
#define SHARED_IMEM_PIL_SIZE             200
#define SHARED_IMEM_TZ_BASE              (SHARED_IMEM_PIL_BASE + SHARED_IMEM_PIL_SIZE)
#define SHARED_IMEM_TZ_SIZE              128
#define SHARED_IMEM_OEM_BASE             (SHARED_IMEM_TZ_BASE + SHARED_IMEM_TZ_SIZE)
#define SHARED_IMEM_OEM_SIZE             100
#define SHARED_IMEM_SDI_BASE             (SHARED_IMEM_OEM_BASE + SHARED_IMEM_OEM_SIZE)
#define SHARED_IMEM_SDI_SIZE             32
#define SHARED_IMEM_MODEM_BASE           (SHARED_IMEM_SDI_BASE + SHARED_IMEM_SDI_SIZE)
#define SHARED_IMEM_MODEM_SIZE           4
#define SHARED_IMEM_UNUSED_SPACE_BASE    (SHARED_IMEM_MODEM_BASE + SHARED_IMEM_MODEM_SIZE)
#define SHARED_IMEM_UNUSED_SPACE_SIZE    1152
#define SHARED_IMEM_BOOTROM_BASE         (SHARED_IMEM_UNUSED_SPACE_BASE + SHARED_IMEM_UNUSED_SPACE_SIZE)
#define SHARED_IMEM_BOOTROM_SIZE         100

#define DDR_MEM_BASE                     0x80000000
#define DDR_MEM_SIZE                     SIZE_8GB

#define SCL_DDR_pIMEM_BASE               0x86300000
#define SCL_DDR_pIMEM_SIZE               SIZE_72MB
#define SCL_pIMEM_BASE                   0x1C000000
#define SCL_pIMEM_SIZE                   SIZE_16MB
#define SCL_pIMEM_MINIMUM_SIZE           SIZE_2MB

/* SBL's DDR section is shared by Loader DDR Data and Ramdump */
#define SCL_SBL1_DDR_BASE                0x85E00000
#define SCL_SBL1_DDR_SIZE                SIZE_1MB

#define SCL_SBL1_DDR_DATA_BASE           SCL_SBL1_DDR_BASE
#define SCL_SBL1_DDR_DATA_SIZE           SIZE_212KB
#define SCL_SBL1_DDR_PAGE_TABLE_BASE     SCL_SBL1_DDR_DATA_BASE
#define SCL_SBL1_DDR_PAGE_TABLE_SIZE     SIZE_64KB
#define SCL_SBL1_DDR_UNCACHED_ZI_BASE    (SCL_SBL1_DDR_PAGE_TABLE_BASE + SCL_SBL1_DDR_PAGE_TABLE_SIZE)
#define SCL_SBL1_DDR_UNCACHED_ZI_SIZE    SIZE_4KB
#define SCL_SBL1_DDR_ZI_BASE             (SCL_SBL1_DDR_UNCACHED_ZI_BASE + SCL_SBL1_DDR_UNCACHED_ZI_SIZE)  
#define SCL_SBL1_DDR_ZI_SIZE  (SCL_SBL1_DDR_DATA_SIZE - SCL_SBL1_DDR_PAGE_TABLE_SIZE - SCL_SBL1_DDR_UNCACHED_ZI_SIZE)

#define SCL_RAMDUMP_TOTAL_SIZE           (SCL_SBL1_DDR_SIZE - SCL_SBL1_DDR_DATA_SIZE)
#define SCL_RAMDUMP_CODE_BASE            (SCL_SBL1_DDR_BASE + SCL_SBL1_DDR_DATA_SIZE)
#define SCL_RAMDUMP_CODE_SIZE            SIZE_392KB
#define SCL_RAMDUMP_DLOAD_BUFFERS_BASE   (SCL_RAMDUMP_CODE_BASE + SCL_RAMDUMP_CODE_SIZE)
#define SCL_RAMDUMP_DLOAD_BUFFERS_SIZE   SIZE_64KB
#define SCL_RAMDUMP_DATA_BASE            (SCL_RAMDUMP_DLOAD_BUFFERS_BASE + SCL_RAMDUMP_DLOAD_BUFFERS_SIZE)
#define SCL_RAMDUMP_DATA_SIZE  (SCL_RAMDUMP_TOTAL_SIZE - SCL_RAMDUMP_CODE_SIZE - SCL_RAMDUMP_DLOAD_BUFFERS_SIZE)
#define SCL_LOADERTEST_TOTAL_SIZE        (SCL_LOADERTEST_CODE_SIZE + SCL_LOADERTEST_DATA_SIZE)
#define SCL_LOADERTEST_CODE_BASE         SCL_XBL_CORE_CODE_BASE
#define SCL_LOADERTEST_CODE_SIZE         SIZE_2MB
#define SCL_LOADERTEST_DATA_BASE         (SCL_XBL_CORE_CODE_BASE + SCL_LOADERTEST_CODE_SIZE)
#define SCL_LOADERTEST_DATA_SIZE         SIZE_2MB
/* Only boot log size is needed as base addresses are automatically placed
   in DDR_ZI */
#define SCL_SBL1_DDR_BOOT_LOG_META_INFO_SIZE    SCL_SBL1_BOOT_LOG_META_INFO_SIZE
#define SCL_SBL1_DDR_BOOT_LOG_BUF_SIZE          SIZE_5KB
#define SCL_SBL1_DDR_BOOT_TIME_MARKER_BUF_SIZE  SIZE_256B

#define SCL_SHARED_RAM_BASE              0x86000000
#define SCL_SHARED_RAM_SIZE              SIZE_2MB

#define SCL_QSEE_CODE_BASE               SCL_IMEM_BASE
#define SCL_QSEE_TOTAL_SIZE              SIZE_100KB
#define SCL_TZ_STAT_BASE                 0x86200000
#define SCL_TZ_STAT_BASE_SIZE            SIZE_1MB

#define SCL_DEVICEPROG_ZI_BASE           SCL_IMEM_BASE
#define SCL_DEVICEPROG_ZI_SIZE           (SIZE_100KB+SIZE_8KB)

#define SCL_QHEE_CODE_BASE                  0x85700000
#define SCL_QHEE_TOTAL_SIZE                 SIZE_6MB
#define SCL_APPSBL_CODE_BASE                0x8EC00000
#define SCL_APPSBL_TOTAL_SIZE               0x0F00000
#define SCL_XBL_CORE_CODE_BASE              0x9FC00000
#define SCL_XBL_CORE_CODE_BASE2             0x80200000
#define SCL_XBL_CORE_TOTAL_SIZE             0x0400000
#define SCL_XBL_CORE_DDR_BASE               0x9F800000
#define SCL_XBL_CORE_DDR_TOTAL_SIZE         0x0800000
#define SCL_SBL1_BOOT_LOG_META_INFO_SIZE    64
#define SCL_SBL1_BOOT_TIME_MARKER_BUF_SIZE  64
#define SCL_SBL1_BOOT_LOG_BUF_SIZE          SIZE_4KB
#define PBL_APPS_ROM_BASE                   0x00100000
#define PBL_APPS_ROM_SIZE                   SIZE_144KB
#define SBL1_DEBUG_COOKIE_ADDR              (SHARED_IMEM_PERIPHERAL_DEBUG_BASE + 0x10)
#define SBL1_DEBUG_COOKIE_VAL               0x53444247
#define SCL_APSS_REGION_FOR_ABL_BASE        0x96200000
#define SCL_APSS_REGION_FOR_ABL_SIZE        SIZE_150MB

#define SCL_IRAM_BASE  SCL_BOOT_IMEM_BASE_ADDR
#define SCL_IRAM_SIZE  SCL_BOOT_IMEM_BASE_SIZE

#define SCL_MODEM_EFS_RAM_SIZE 0x0300000

#define IMAGE_KEY_EMMCBLD_IMG_DEST_ADDR SCL_SBL1_CODE_BASE+80
#define IMAGE_KEY_DEVICEPROGRAMMER_IMG_DEST_ADDR SCL_SBL1_CODE_BASE+80

/* IPA Ramdump memory regions */
#define SCL_IPA_IRAM_BASE  0x01E60000
#define SCL_IPA_IRAM_SIZE  SIZE_16KB
#define SCL_IPA_DRAM_BASE  0x01E64000
#define SCL_IPA_DRAM_SIZE  0x00003F00
#define SCL_IPA_SRAM_BASE  0x01E47000
#define SCL_IPA_SRAM_SIZE  SIZE_8KB
#define SCL_IPA_HRAM_BASE  0x01E4A000
#define SCL_IPA_HRAM_SIZE  0x00006700
#define SCL_IPA_MBOX_BASE  0x01E72000
#define SCL_IPA_MBOX_SIZE  SIZE_256B
#define SCL_IPA_GSI1_BASE  0x01E04000 
#define SCL_IPA_GSI1_SIZE  SIZE_16KB
#define SCL_IPA_SEQ_BASE   0x01E5E000
#define SCL_IPA_SEQ_SIZE   SIZE_4KB

// SHRM memory that needs to be dumped
#define SCL_SHRM_MEM_SHRM_BASE 0x013E0000
#define SCL_SHRM_MEM_SHRM_SIZE SIZE_64KB

//DCC SRAM that needs to be dumped
#define SCL_DCC_LL_SRAM_BASE 0x010A8000
#define SCL_DCC_LL_SRAM_SIZE SIZE_32KB

#define RAW_PARTITION_BUF_ADDR  0x20000000
#define RAW_PARTITION_BUF_SIZE  0x1000

/*===========================================================================
                Defines to support PMIC image loading 
===========================================================================*/

/*   PMIC config base in Boot Imem (0x148D1000) and size  */
#define SCL_PMIC_CONFIG_BUF_BASE  SCL_DDR_TRAINING_CODE_BASE
#define SCL_PMIC_CONFIG_BUF_SIZE  SIZE_64KB


/*  Below is temp buffer base for hash segment used by ELF loader in loading images
    before DDR init 
*/
#define HASH_SEGMENT_TEMP_BASE (SCL_PMIC_CONFIG_BUF_BASE + SCL_PMIC_CONFIG_BUF_SIZE)
#define HASH_SEGMENT_SIZE SIZE_20KB

/*===========================================================================
                 Defines to support enhanced boot logging
============================================================================*/
#define BOOT_LOGGER_BOOT_CONFIG_FUSE_ADDRESS           HWIO_BOOT_CONFIG_ADDR
#define BOOT_LOGGER_JTAG_ID_FUSE_ADDRESS               HWIO_JTAG_ID_ADDR
#define BOOT_LOGGER_OEM_ID_FUSE_ADDRESS                HWIO_OEM_ID_ADDR
#define BOOT_LOGGER_SERIAL_NUM_FUSE_ADDRESS            HWIO_QFPROM_CORR_PTE_ROW1_LSB_ADDR
#define BOOT_LOGGER_OEM_CONFIG_ROW_0_FUSE_ADDRESS      HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ADDR
#define BOOT_LOGGER_OEM_CONFIG_ROW_1_FUSE_ADDRESS      HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_ADDR 
#define BOOT_LOGGER_FEATURE_CONFIG_ROW_0_FUSE_ADDRESS  HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ADDR
#define BOOT_LOGGER_FEATURE_CONFIG_ROW_1_FUSE_ADDRESS  HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_ADDR


/*===========================================================================
                      FEATURE DEFINES USED ACROSS DRIVERS
===========================================================================*/

#define DDR_XO_SPEED_IN_KHZ 19200
#define BOOT_TEMP_CHECK_THRESHOLD_DEGC
#undef  FEATURE_USE_PBL_SECBOOT_API
#undef  FEATURE_EMPTY_L1_PAGE_TABLE

/* Enablement flag for asynchronous uart output */
#define BOOT_ASYNC_UART_FEATURE

/* Enablement flags for boot profiler feature below */
#define BOOT_PROFILER_FEATURE  /* Enable boot profiler overall feature can
                                  remove/comment out to completely disable */

/* Set current profiler level
   0 - Profiling messges only
   1 - High level debug messages and level 0
   2 - Low level debug messages and levels 1 and 0 */
#define BOOT_PROFILER_LEVEL 0

/* Enable boot profiler for boot code
   Uncomment the following to enable */
//#define SBL_PROFILER_FEATURE

/* Driver team must define macros which map to boot_profiler* macros. 
   These driver-defined macros to be defined in "[driver]_profile.h".
   After driver has defined and placed these, uncomment the following
   lines to enable for any particular driver(commented out by default) */
//#define EFS_PROFILER_FEATURE

//#define DAL_PROFILER_FEATURE

//#define TLMM_PROFILER_FEATURE

//#define APT_PROFILER_FEATURE

//#define HOTPLUG_PROFILER_FEATURE

//#define SMEM_PROFILER_FEATURE

//#define PLATFORMINFO_PROFILER_FEATURE

//#define PMIC_PROFILER_FEATURE

//#define CLK_PROFILER_FEATURE

//#define VSENSE_PROFILER_FEATURE

//#define HSUSB_PROFILER_FEATURE

//#define BUSYWAIT_PROFILER_FEATURE

//#define BUS_PROFILER_FEATURE

//#define DDR_PROFILER_FEATURE

//#define FLASH_PROFILER_FEATURE

//#define CRYPTO_PROFILER_FEATURE

//#define SECBOOT_PROFILER_FEATURE

//#define UART_PROFILER_FEATURE

//#define POWER_PROFILER_FEATURE

//#define LIMITS_PROFILER_FEATURE

//#define SECIMG_AUTH_PROFILER_FEATURE

#endif  /* _TARGET_CUST_H */