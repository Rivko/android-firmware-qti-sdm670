#ifndef _BOOT_TARGET_H
#define _BOOT_TARGET_H

/*===========================================================================

                                boot_target
                                Header File

GENERAL DESCRIPTION
  This header file contains target specific declarations and definitions

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2014 - 2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/XBLLoaderLib/boot_target.h#5 $
  $DateTime: 2018/02/12 08:41:19 $ 
  $Author: pwbldsvc $

when       who          what, where, why
--------   --------     ----------------------------------------------------------
09/21/17   aus          Added SSC region defines
08/24/17   yps          Increased crash dump  table entries
07/12/17   ds           added remaining aop msg ram regions
06/08/17   ds           V6 memmap changes
05/08/17   kpa          update STI_IMG_WHITELIST
05/09/17   sj           define pmic device index
04/27/17   kpa          enable dumping DCC SRAM region
04/13/17   kpa          Added MAX_CRASHDUMP_TABLE_ENTRIES
04/08/17   kpa          remove SHRM_IMG_WHITELIST
03/03/17   ds	        update aop whtlist
02/22/17   ds           fix abl and devcfg whitelist area
01/27/16   digants      Added Defines for whitelist address ranges for each image
07/14/16   kedara       Initial version for SDM845
============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#ifndef _ARM_ASM_
#include "boot_comdef.h"
#endif /* _ARM_ASM_ */

/*===========================================================================
              DEFINES FOR TARGET (equivalent to <target>.builds src)
===========================================================================*/

/*===========================================================================
                   DEFINES FOR MEMORY MAP, COMMON FEATURE DEFINES
===========================================================================*/
#include "Target_cust.h"

/*===========================================================================
                      DEFINES FOR RAMDUMPS / FEATURE ENABLE 
===========================================================================*/
#define FEATURE_DLOAD_MEM_DEBUG
#define FEATURE_BOOT_RAMDUMPS_TO_SD_CARD
#define CONVERT_TO_MB_SHIFT 20
#define SD_PATH "/mmc1/"
#define SD_RAM_DUMP_PATH SD_PATH "ram_dump/"

/* Maximum number of entries for static tables that hold ramdump regions */
#define MAX_CRASHDUMP_TABLE_ENTRIES 200

#define MEMORY_REGION1  {SCL_IMEM_BASE, SCL_IMEM_SIZE, "OCIMEM", "OCIMEM.BIN"}

#define MEMORY_REGION2  {SCL_AOP_CODE_RAM_BASE, SCL_AOP_CODE_RAM_SIZE, "AOP Code RAM region", "CODERAM.BIN"}
#define MEMORY_REGION3  {SCL_AOP_DATA_RAM_BASE, SCL_AOP_DATA_RAM_SIZE, "AOP Data RAM region", "DATARAM.BIN"}
#define MEMORY_REGION4  {SCL_AOP_MSG_RAM0_BASE, SCL_AOP_MSG_RAMX_SIZE, "AOP MSGRAM0 region", "MSGRAM0.BIN"}
#define MEMORY_REGION5  {SCL_AOP_MSG_RAM1_BASE, SCL_AOP_MSG_RAMX_SIZE, "AOP MSGRAM1 region", "MSGRAM1.BIN"}
#define MEMORY_REGION6  {SCL_AOP_MSG_RAM2_BASE, SCL_AOP_MSG_RAMX_SIZE, "AOP MSGRAM2 region", "MSGRAM2.BIN"}
#define MEMORY_REGION7  {SCL_AOP_MSG_RAM3_BASE, SCL_AOP_MSG_RAMX_SIZE, "AOP MSGRAM3 region", "MSGRAM3.BIN"}
#define MEMORY_REGION8  {SCL_AOP_MSG_RAM4_BASE, SCL_AOP_MSG_RAMX_SIZE, "AOP MSGRAM4 region", "MSGRAM4.BIN"}
#define MEMORY_REGION9  {SCL_AOP_MSG_RAM5_BASE, SCL_AOP_MSG_RAMX_SIZE, "AOP MSGRAM5 region", "MSGRAM5.BIN"}
#define MEMORY_REGION10 {SCL_AOP_MSG_RAM6_BASE, SCL_AOP_MSG_RAMX_SIZE, "AOP MSGRAM6 region", "MSGRAM6.BIN"}
#define MEMORY_REGION11 {SCL_AOP_MSG_RAM7_BASE, SCL_AOP_MSG_RAMX_SIZE, "AOP MSGRAM7 region", "MSGRAM7.BIN"}
#define MEMORY_REGION12 {SCL_AOP_MSG_RAM8_BASE, SCL_AOP_MSG_RAMX_SIZE, "AOP MSGRAM8 region", "MSGRAM8.BIN"}
#define MEMORY_REGION13 {SCL_AOP_MSG_RAM9_BASE, SCL_AOP_MSG_RAMX_SIZE, "AOP MSGRAM9 region", "MSGRAM9.BIN"}
#define MEMORY_REGION14 {SCL_AOP_MSG_RAM10_BASE, SCL_AOP_MSG_RAMX_SIZE, "AOP MSGRAM10 region", "MSGRAM10.BIN"}
#define MEMORY_REGION15 {SCL_AOP_MSG_RAM11_BASE, SCL_AOP_MSG_RAMX_SIZE, "AOP MSGRAM11 region", "MSGRAM11.BIN"}
#define MEMORY_REGION16 {SCL_AOP_MSG_RAM12_BASE, SCL_AOP_MSG_RAMX_SIZE, "AOP MSGRAM12 region", "MSGRAM12.BIN"}
#define MEMORY_REGION17 {SCL_AOP_MSG_RAM13_BASE, SCL_AOP_MSG_RAMX_SIZE, "AOP MSGRAM13 region", "MSGRAM13.BIN"}
#define MEMORY_REGION18 {SCL_AOP_MSG_RAM14_BASE, SCL_AOP_MSG_RAMX_SIZE, "AOP MSGRAM14 region", "MSGRAM14.BIN"}
#define MEMORY_REGION19 {SCL_AOP_MSG_RAM15_BASE, SCL_AOP_MSG_RAMX_SIZE, "AOP MSGRAM15 region", "MSGRAM15.BIN"}

#define MEMORY_REGION20 {SCL_IPA_IRAM_BASE, SCL_IPA_IRAM_SIZE, "IPA IRAM region", "IPA_IRAM.BIN"}
#define MEMORY_REGION21 {SCL_IPA_DRAM_BASE, SCL_IPA_DRAM_SIZE, "IPA DRAM region", "IPA_DRAM.BIN"}
#define MEMORY_REGION22 {SCL_IPA_SRAM_BASE, SCL_IPA_SRAM_SIZE, "IPA SRAM region", "IPA_SRAM.BIN"}
#define MEMORY_REGION23 {SCL_IPA_HRAM_BASE, SCL_IPA_HRAM_SIZE, "IPA HRAM region", "IPA_HRAM.BIN"}
#define MEMORY_REGION24 {SCL_IPA_MBOX_BASE, SCL_IPA_MBOX_SIZE, "IPA MBOX region", "IPA_MBOX.BIN"}
#define MEMORY_REGION25 {SCL_IPA_GSI1_BASE, SCL_IPA_GSI1_SIZE, "IPA GSI1 region", "IPA_GSI1.BIN"}
#define MEMORY_REGION26 {SCL_IPA_SEQ_BASE,  SCL_IPA_SEQ_SIZE,  "IPA SEQ region",  "IPA_SEQ.BIN"}

#define MEMORY_REGION27 {SCL_SHRM_MEM_SHRM_BASE, SCL_SHRM_MEM_SHRM_SIZE, "SHRM MEM region", "SHRM_MEM.BIN"}

//add DCC SRAM once SDI is available
#define MEMORY_REGION28 {SCL_DCC_LL_SRAM_BASE, SCL_DCC_LL_SRAM_SIZE, "DCC SRAM region", "DCC_SRAM.BIN"}

#define MEMORY_REGION_LAST {0, 0, NULL, NULL}
#define MEMORY_REGION_TABLE MEMORY_REGION1,MEMORY_REGION2,MEMORY_REGION3,MEMORY_REGION4,MEMORY_REGION5,MEMORY_REGION6,MEMORY_REGION7,MEMORY_REGION8,MEMORY_REGION9,MEMORY_REGION10,MEMORY_REGION11,MEMORY_REGION12,MEMORY_REGION13,MEMORY_REGION14,MEMORY_REGION15,MEMORY_REGION16,MEMORY_REGION17,MEMORY_REGION18,MEMORY_REGION19,MEMORY_REGION27,MEMORY_REGION28,MEMORY_REGION_LAST

#define IPA_MEMORY_REGION_TABLE MEMORY_REGION20,MEMORY_REGION21,MEMORY_REGION22,MEMORY_REGION23,MEMORY_REGION24,MEMORY_REGION25,MEMORY_REGION26,MEMORY_REGION_LAST  

/* ELF Segment Element format: {"Region Base Address","Region Size in Bytes"} */
#define WHITELIST_REGION1   {SCL_XBL_CORE_CODE_BASE, SCL_XBL_CORE_TOTAL_SIZE} 
#define WHITELIST_REGION2   {SCL_SBL1_CODE_BASE, SCL_SBL1_TOTAL_SIZE}
#define WHITELIST_REGION3	{SCL_DDR_TRAINING_CODE_BASE, SCL_DDR_TRAINING_SIZE}
#define WHITELIST_REGION4   {SCL_XBL_CORE_CODE_BASE2, SCL_XBL_CORE_TOTAL_SIZE}
#define WHITELIST_REGION_LAST {0, 0}

/* Whitelist Table note: Must be on one line as '\' operator not defined in .builds environment */
#define XBL_CORE_WHITELIST_REGION_TABLE WHITELIST_REGION1,WHITELIST_REGION4,WHITELIST_REGION_LAST
#define XBL_LOADER_WHITELIST_REGION_TABLE WHITELIST_REGION2,WHITELIST_REGION_LAST
#define DDR_TRAINING_WHITELIST_REGION_TABLE WHITELIST_REGION3,WHITELIST_REGION_LAST
#define RAMDUMP_IMG_WHITELIST {{SCL_RAMDUMP_CODE_BASE,SCL_RAMDUMP_CODE_SIZE},{SCL_RAMDUMP_DLOAD_BUFFERS_BASE,SCL_RAMDUMP_DLOAD_BUFFERS_SIZE},{SCL_RAMDUMP_DATA_BASE,SCL_RAMDUMP_DATA_SIZE},{0,0}}


#define BOOT_IMAGES_NUM_ENTRIES 11

//pmic device index
#define PMIC_DEVICE_INDEX 0
#define DLOAD_PMIC_WDG_S1_VALUE      110
#define DLOAD_PMIC_WDG_S2_VALUE      120

#define FEATURE_QFUSE_PROGRAMMING
#define FEATURE_DLOAD_MEM_DEBUG
#define IMAGE_KEY_SBL1_IMG_DEST_ADDR SCL_SBL1_CODE_BASE
#define FEATURE_TPM_HASH_POPULATE

/* Defines for whitelist table for each image */

#define PMIC_IMG_WHITELIST {{SCL_PMIC_CONFIG_BUF_BASE,SCL_PMIC_CONFIG_BUF_SIZE},{0,0}}
#define AOP_IMG_WHITELIST {{SCL_AOP_CODE_RAM_BASE,SCL_AOP_CODE_RAM_SIZE},{SCL_AOP_DATA_RAM_BASE,SCL_AOP_DATA_RAM_SIZE},{SCL_AOP_IMG_DDR_BASE,SCL_AOP_IMG_DDR_SIZE},{SCL_AOP_CMD_DB_BASE,SCL_AOP_CMD_DB_SIZE},{0,0}}
#define APDP_IMG_WHITELIST {{SCL_pIMEM_BASE,SCL_pIMEM_SIZE},{0,0}}
#define DEVCFG_IMG_WHITELIST {{SCL_TZ_STAT_BASE,SCL_TZ_STAT_BASE_SIZE},{SCL_pIMEM_BASE,SCL_pIMEM_SIZE},{SCL_QHEE_CODE_BASE,SCL_QHEE_TOTAL_SIZE},{0,0}}
#define QSEE_IMG_WHITELIST {{SCL_QSEE_CODE_BASE,SCL_QSEE_TOTAL_SIZE},{SCL_pIMEM_BASE,SCL_pIMEM_SIZE},{0,0}}
#define QHEE_IMG_WHITELIST {{SCL_QHEE_CODE_BASE,SCL_QHEE_TOTAL_SIZE},{0,0}}
#define XBL_CORE_IMG_WHITELIST {{SCL_XBL_CORE_DDR_BASE,SCL_XBL_CORE_DDR_TOTAL_SIZE},{0,0}}
#define ABL_IMG_WHITELIST {{SCL_APSS_REGION_FOR_ABL_BASE,SCL_APSS_REGION_FOR_ABL_SIZE},{SCL_XBL_CORE_DDR_BASE,SCL_XBL_CORE_DDR_TOTAL_SIZE},{0,0}}
#define STI_IMG_WHITELIST {{SCL_LOADERTEST_CODE_BASE ,SCL_LOADERTEST_TOTAL_SIZE},{0,0}}

#define SEC_DAT_WHITELIST {{TZBSP_SECDAT_BUFFER_BASE,TZBSP_SECDAT_BUFFER_SIZE},{0,0}}
#define DDR_TRAINING_WHITELIST {{SCL_DDR_TRAINING_CODE_BASE,SCL_DDR_TRAINING_SIZE},{0,0}}
#define DDR_STRUCT_WHITELIST {{SCL_SBL1_DDR_STRUCT_BASE,SCL_SBL1_DDR_STRUCT_SIZE},{0,0}}
#define DDI_DATA_WHITELIST {{SCL_SBL1_DDI_DATA_BASE,SCL_SBL1_DDI_DATA_SIZE},{0,0}}

/*===========================================================================
                      SBL1 SCL DEFINES
===========================================================================*/

#define SBL1_WDOG_RESET_VALUE   0x1

/*---------------------------------------------------------------------------
  Define the destination address and size of the SBL1.
---------------------------------------------------------------------------*/
#define SBL1_CODE_BASE       SCL_SBL1_CODE_BASE
#define SBL1_MAX_IMAGE_SIZE  SCL_SBL1_TOTAL_SIZE


#ifndef _ARM_ASM_
/*---------------------------------------------------------------------------
  Symbols generated by linker with "$" instead of "_".  Symbols are renamed
  in an assembler file to have "_" to make them valid "C" identifiers and
  the symbols are located in the boot ROM.  These symbols define the
  Read/Write data area in boot ROM and RAM, and the Zero Init data area in
  RAM.  Base addresses and length in bytes are given.
---------------------------------------------------------------------------*/
extern uintnt  Load$$SBL1_INDIRECT_VEC_TBL$$Base;
extern uintnt  Image$$SBL1_INDIRECT_VEC_TBL$$Base;
extern uintnt  Image$$SBL1_INDIRECT_VEC_TBL$$Length;
extern uintnt  Load$$SBL1_DATA_RW$$Base;
extern uintnt  Image$$SBL1_DATA_RW$$Base;
extern uintnt  Image$$SBL1_DATA_RW$$Length;
extern uintnt  Image$$SBL1_DATA_ZI$$Base;
extern uintnt  Image$$SBL1_DATA_ZI$$ZI$$Length;
extern uintnt  Image$$SBL1_DDR_ZI$$Base;
extern uintnt  Image$$SBL1_DDR_ZI$$ZI$$Length;

extern uintnt  Image$$SBL1_DDR_UNCACHED_ZI$$Base;
extern uintnt  Image$$SBL1_DDR_UNCACHED_ZI$$ZI$$Length;

extern uintnt  Image$$SBL1_OCIMEM_DATA$$Base;
extern uintnt  Image$$SBL1_OCIMEM_DATA$$ZI$$Length;

extern uintnt  Image$$SYSTEM_DEBUG_QSEE_INFO$$Base;

#endif /* _ARM_ASM_ */


#define BOOT_FLASH_TRANS_MAX_INSTANCES 1

/*---------------------------------------------------------------------------
  Define the SBL page table base address and size, it's used by both SBL2 and 3  
---------------------------------------------------------------------------*/
//#define SCL_SBL_PAGE_TABLE_BASE  (SCL_SBL2_STACK_BASE-SCL_SBL2_STACK_SIZE)
//#define SCL_SBL_PAGE_TABLE_SIZE  0x6000


/*===========================================================================
                      QDST DEFINES
===========================================================================*/ 
/*Use chip hw id for MSM part value. This would simply preparation for OEM pk
  hash auth Test case. With the change, the same QDST signed image can be used
  to verify OEM pk hash authentication*/
#define BOOT_MSM_PART  0x006B20E1
#define BOOT_MSM_HW_ID  (((uint64)BOOT_MSM_PART) << 32)


/*===========================================================================
  Definitions of address range where IMEM exists on the MSM.
===========================================================================*/
#define IMEM_RANGE_START_ADDR SCL_IMEM_BASE
#define IMEM_RANGE_END_ADDR   (SCL_IMEM_BASE+SCL_IMEM_SIZE)


/*===========================================================================
  Definition for the end address of SDRAM on the MSM.
  MAX value for 48 bits (2^48) -1
===========================================================================*/
#define RANGE_END_ADDR        0xFFFFFFFFFFFFUL


/*===========================================================================
                      BOOT LOGGER DEFINES
===========================================================================*/

/**
 * Time out value for outputing a single char to jtag terminal.
 * 
 * Value used by rpm jtag. moved it to this target specific file
 * from common boot_arm7.c Existing value does not ensure proper
 * output to jtag and has to be updated for 8960
 * JTAG_TERM_TIME_OUT renamed to RPM_JTAG_TERM_TIME_OUT as krait
 * uses similar define.
 */
#define RPM_JTAG_TERM_TIME_OUT       0x500

/*=========================================================================== 
  The clock that is used to track time passing.
===========================================================================*/
#define TIMETICK_CLK  MPM_SLEEP_TIMETICK_COUNT_VAL 

/*=========================================================================== 
  Clock frequency is 32.768 KHz
  The number of microseconds per cycle is:
  1 / (32.768 KHz) = 30.52ms
  The int part is 30
===========================================================================*/
#define TIMETICK_CLK_SPEED_INT       30

/*=========================================================================== 
  MPM Sleep Clock frequency is 32.768 KHz
  The number of microseconds per cycle is:
  1 / (32.768 KHz) = 30.52ms
  we round 0.52 to 1/2.
  Timestamp is calculated as : count*int + count/2
  Floating point arithmetic should not be used to avoid error and speed penalty
===========================================================================*/
#define CALCULATE_TIMESTAMP(COUNT) \
  ((COUNT)*TIMETICK_CLK_SPEED_INT + (COUNT)/2)

/*=========================================================================== 
  Qtimer Clock frequency is CXO 19.2MHz
  The number of microseconds per cycle 1/19.2
===========================================================================*/
#define CALCULATE_QTIMER_TIMESTAMP(COUNT) \
  ((COUNT)/19.2)
  
/*
  we are using 32 bits timestamp, the max counter value is:
  0xFFFFFFFF/31 = 138547732
  after this value the timestamp will overflow
*/
#define LOG_MAX_COUNT_VAL  138547732


/*TODO: _FIX_ for 8996.*/
/*OEM fuse All jtag debug disable status
  1 if OEM all jtag debug is disabled, 0 otherwise*/
#define FUSE0_DISABLE  (HWIO_INM(OEM_CONFIG0,  \
                              HWIO_FMSK(OEM_CONFIG0, ALL_DEBUG_DISABLE)))

/*QC fuse RPM jtag debug disable status
  1 if QC RPM jtag debug is disabled, 0 otherwise
  */
#define FUSE1_DISABLE  (HWIO_INM(FEATURE_CONFIG2, \
                                 HWIO_FMSK(FEATURE_CONFIG2, RPM_DBGEN_DISABLE)))

/*OEM fuse RPM jtag debug disable status
  1 if OEM RPM jtag debug is disabled, 0 otherwise*/
#define FUSE2_DISABLE  (HWIO_INM(OEM_CONFIG0, \
                                 HWIO_FMSK(OEM_CONFIG0, RPM_DBGEN_DISABLE)))

/*QC fuse APPS jtag debug disable status     
  1 if QC APPS jtag debug is disabled, 0 otherwise*/
#define FUSE3_DISABLE  (HWIO_INM(FEATURE_CONFIG4, \
                               HWIO_FMSK(FEATURE_CONFIG4, QC_APPS_DBGEN_DISABLE)))

/*OEM fuse APPS jtag debug disable status
  1 if OEM APPS jtag debug is disabled, 0 otherwise*/
#define FUSE4_DISABLE  (HWIO_INM(OEM_CONFIG1, \
                                HWIO_FMSK(OEM_CONFIG1, APPS_DBGEN_DISABLE)))

/*QC fuse APPS secure jtag debug disable status
  1 if QC APPS secure jtag debug is disabled, 0 otherwise*/
#define FUSE5_DISABLE  (HWIO_INM(FEATURE_CONFIG5, \
                               HWIO_FMSK(FEATURE_CONFIG5, QC_APPS_SPIDEN_DISABLE)))

/*OEM fuse APPS secure jtag debug disable status
  1 if OEM APPS secure jtag debug is disabled, 0 otherwise*/
#define FUSE6_DISABLE  (HWIO_INM(OEM_CONFIG1,  \
                             HWIO_FMSK(OEM_CONFIG1, APPS_SPNIDEN_DISABLE)))

/*QC fuse APPS 1 jtag debug disable status     
  1 if QC APPS 1 jtag debug is disabled, 0 otherwise*/
#define FUSE7_DISABLE  (HWIO_INM(FEATURE_CONFIG4, \
                               HWIO_FMSK(FEATURE_CONFIG4, QC_APPS_DBGEN_DISABLE)))

#define FUSE8_DISABLE  (HWIO_INM(FEATURE_CONFIG5, \
                                HWIO_FMSK(FEATURE_CONFIG5, QC_APPS_SPIDEN_DISABLE)))
                                
#define FUSE9_DISABLE  (HWIO_INM(OEM_CONFIG1, \
                                HWIO_FMSK(OEM_CONFIG1, APPS_DBGEN_DISABLE)))

/*OEM fuse APPS secure jtag debug disable status
  1 if OEM APPS secure jtag debug is disabled, 0 otherwise*/
#define FUSE10_DISABLE  (HWIO_INM(OEM_CONFIG2,  \
                             HWIO_FMSK(OEM_CONFIG1, APPS_SPNIDEN_DISABLE)))

/* OEM fuse disables the DAP non-invasive debug capabilities */                                
#define DAP_FUSE0_DISABLE  (HWIO_INM(OEM_CONFIG1,  \
                                 HWIO_FMSK(OEM_CONFIG1, DAP_NIDEN_DISABLE)))   
                                 
/* QC fuse disables the DAP non-invasive debug capabilities */                                
#define DAP_FUSE1_DISABLE  (HWIO_INM(FEATURE_CONFIG2,  \
                                 HWIO_FMSK(FEATURE_CONFIG2, QC_DAP_NIDEN_DISABLE)))   
                                 
/* OEM fuse disables the DAP Secure non-invasive debug capabilities.*/                              
#define DAP_FUSE2_DISABLE  (HWIO_INM(OEM_CONFIG1,  \
                                 HWIO_FMSK(OEM_CONFIG1, DAP_SPNIDEN_DISABLE)))
                                 
/* QC fuse disables the DAP Secure non-invasive debug capabilities.*/                              
#define DAP_FUSE3_DISABLE  (HWIO_INM(FEATURE_CONFIG2,  \
                                 HWIO_FMSK(FEATURE_CONFIG2, QC_DAP_SPNIDEN_DISABLE)))   
                                 
/* OEM fuse disables the Disables the DAP invasive debug capabilities.*/
#define DAP_FUSE4_DISABLE  (HWIO_INM(OEM_CONFIG1,  \
                                 HWIO_FMSK(OEM_CONFIG1, DAP_DBGEN_DISABLE))) 
                                 
/* QC fuse disables the Disables the DAP invasive debug capabilities.*/
#define DAP_FUSE5_DISABLE  (HWIO_INM(FEATURE_CONFIG2,  \
                                 HWIO_FMSK(FEATURE_CONFIG2, QC_DAP_DBGEN_DISABLE)))                                  
                             
                                 
/*Macro to check if any RPM jtag debug feature is disabled
  return 1 if any RPM jtag debug is disabled.
  return 0 if it is enabled
*/
#define DEBUG_DISABLED_RPM (FUSE0_DISABLE| \
                            FUSE1_DISABLE| \
                            FUSE2_DISABLE)

/*Macro to check if any apps jtag debug feature is disabled
  return 1 if any apps jtag debug is disabled.
  return 0 if it is enabled*/
#define DEBUG_DISABLED_APPS (FUSE0_DISABLE|  \
                             FUSE3_DISABLE|  \
                             FUSE4_DISABLE|  \
                             FUSE5_DISABLE|  \
                             FUSE6_DISABLE|  \
                             FUSE7_DISABLE|  \
                             FUSE8_DISABLE|  \
                             FUSE9_DISABLE|  \
                             FUSE10_DISABLE)
                             
/* Macro to check if wdog debug image disable fuses are blown 
   return 1 if any one is blown thus disable wdog debug image loading
   return 0 if none is blown thus enable wdog debug image loading */                            
#define DEBUG_DISABLE_WDT  (DAP_FUSE0_DISABLE | \
                            DAP_FUSE1_DISABLE | \
                            DAP_FUSE2_DISABLE | \
                            DAP_FUSE3_DISABLE | \
                            DAP_FUSE4_DISABLE | \
                            DAP_FUSE5_DISABLE)

/*===========================================================================
                      SSC_SCC_SCC DEFINES FOR MMMU MAPPING
===========================================================================*/ 						
#define SSC_SCC_SCC_REG_BASE 0x62B10000
#define SSC_SCC_SCC_REG_SIZE 0x30000

/*===========================================================================
                      QFPROM DRIVER DEFINES
===========================================================================*/ 
/* Mark QFPROM region as read only: 24KB from SECURITY_CONTROL_BASE */
#define QFPROM_READONLY_BLOCK_SIZE  0x6000


/* SFPB clock defines for Qfprom Driver in khz.
   Clock runs at 64Mhz in sbl3 for 8960. This clock is used
   in Qfprom driver to compute correct timer value.
*/
#define BOOT_QFPROM_SFPB_CLOCK  (0x40 * 1000)


/* EEPROM' DAL device ID */                             
#define EEPROM_DEVICE_ID DALDEVICEID_FLASH_DEVICE_2

/* Macro for EEPROM CDT parsing error action, in this target we go to error handler*/
#define CDT_BL_ERROR(BL_ERR_CODE) \
        BL_VERIFY( FALSE, BL_ERR_CODE );

/* In RPM elf image the entry point is in RPM address space so offset is set to 0 */  
#define APPS_RPM_ADDR_VIEW_OFFSET 0x0

/* Define the SD Ram Dump Indicator LED */
#define RAM_DUMP_LED  PM_RGB_SEGMENT_B

/* Define the DDR training Indicator LED */
#define DDR_TRAINING_LED PM_RGB_SEGMENT_G


/*=============================================================================
                       MMU TARGET SPECIFIC DEFINES
=============================================================================*/
#define MMU_NUM_L1_PT                    8 /* max number of L1 pagetables */
#define MMU_L2_PT_SIZE                 512 /* L2 resolves bits [29:21], 2^9=512 entries. And 1GB/2MB = 512 */
#define MMU_NUM_L2_PT                    1 /* max number of L2 pagetables=4, 1 set of 512 per GB           */
#define MMU_L3_PT_SIZE                 512 /* L3 resolves bits [20:12], 2^9=512 entries. And 2MB/4KB = 512 */
#define MMU_NUM_L3_PT                    3 /* max number of L3 pagetables allocated in boot loader */

#define MMU_NUM_DDR_L1_PT                    8 /* max number of L1 pagetables */
#define MMU_NUM_DDR_L2_PT                    3 /* max number of L2 pagetables=4, 1 set of 512 per GB           */
#define MMU_NUM_DDR_L3_PT                   12 /* max number of L3 pagetables allocated in boot loader */


#endif  /* _BOOT_TARGET_H */
