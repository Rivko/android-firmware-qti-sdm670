#ifndef __SDCC_MMC_UTIL_H
#define __SDCC_MMC_UTIL_H
/**********************************************************************
 * File: sdcc_mmc_util.h
 *
 * Services: 
 *    This file provides SDCC util definitions for MMC device.
 *
 * Description:
 *    This file contains the MMC specific utility function prototypes.
 *
 * Copyright (c) 2014-2017 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 **********************************************************************/
/*======================================================================

                        EDIT HISTORY FOR MODULE


when         who     what, where, why
----------   ---     -----------------------------------------------
2017-05-12   sb      Add has400 enhanced strobe mode support
2016-01-18   eo      Reduced sdcc code size
2015-04-15   bn      added GetStorageInfo support
2015-01-26   rm      Integraged HS200/HS400 support for eMMC from UEFI 3.0
2014-11-06   bn      Added eMMC FW Update support
2014-06-18   bn      Initial version. Branched from 8994 SBL

==================================================================*/

/**********************************************************/
/*                    INCLUDES                            */
/**********************************************************/
#include "sdcc_priv.h"
#include "SdccCommon.h"

/*SDCC_EXTRACT_COMMON_BEGIN*/

/*************************************************************/
/*               DEFINITIONS AND CONSTANTS                   */
/*************************************************************/
/** 
 * buffers for eMMC device info raw data.
 * */
#define SDCC_MMC_EXT_CSD_SIZE 512
#define SDCC_MMC_CID_SIZE     4
#define SDCC_MMC_CSD_SIZE     16
#define EMMC_RAW_DATA_SIZE (SDCC_MMC_EXT_CSD_SIZE + SDCC_MMC_CID_SIZE + SDCC_MMC_CSD_SIZE)

/* Per section 9.3 in MMC v4.2 Specification, the card shall complete */
/* initialization within 1 second from the first CMD1 to card ready. */
#define SDCC_MMC_DETECT_MAX_BUSY_MS           1000

/* time interval (in ms) between the CMD1 retry */
#define SDCC_MMC_DETECT_BUSY_RETRY_INTERVAL   2

/* SDCC_MMC_DETECT_MAX_RETRIES is derived from the initialization within */
/* 1 second from the first CMD1 to card ready. */
/* That is 1000 milli-second = 20 x 50 milli-seond where 50 milli-second is */
/* wait time between each CMD1 retry. */
/* Refer to section 9.3 in MMC v4.2 Specification */
#define SDCC_MMC_DETECT_MAX_RETRIES \
   (SDCC_MMC_DETECT_MAX_BUSY_MS / SDCC_MMC_DETECT_BUSY_RETRY_INTERVAL)

/* voltage 2.7-3.6 and high capacity support */
/* Refer to section 5.1 in MMC v4.2 Specification */
#define SDCC_MMC_HCS_NEGOTIATE_OCR          0x40FF8000

/* MMC 2GB card size (i.e., 2 * 1024 * 1024 * 1024) */
/* This 2GB card size constant is used to check against the MMC card size */
/* calculated from CSD register.  If this calculated MMC card size is less */
/* than or equal to 2GB, SEC_COUNT from EXT_CSD register is ignored. */
/* Otherwise, SEC_COUNT is used to calculate the MMC card size if SEC_COUNT */
/* is non-zero from the EXT_CSD register. */
#define SDCC_MMC_2GB_CARD_SIZE              0x80000000

/* the device type for MMC */
/* Refer to section 8.2 in JEDEC Standard No. 84-A44 */
#define SDCC_MMC_DEVICE_TYPE_REMOVABLE_CARD 0
#define SDCC_MMC_DEVICE_TYPE_BGA            1
#define SDCC_MMC_DEVICE_TYPE_POP            2

/* start year of manufacturing date in CID register */
/* Refer to section 8.2 in JEDEC Standard No. 84-A43 */
#define SDCC_MMC_CID_MDT_START_YEAR         1997

/* MMC system specification version supported by the card */
/* Refer to section 8.3 in JEDEC Standard No. 84-A44 */
/* spec version 3x denotes version 3.1 - 3.2 - 3.31 */
#define SDCC_MMC_SPEC_VERS_3X               3
/* spec version 4x denotes version 4.0 - 4.1 - 4.2 - 4.3 */
#define SDCC_MMC_SPEC_VERS_4X               4

/* maximum number of pre-defined block count for CMD23 SET_BLOCK_COUNT */
/* Refer to section 7.9.4 in JEDEC Standard No. 84-A43 */
#define SDCC_EMMC_MAX_PREDEFINED_NUM_BLOCKS     65535

/* reliable write request support in CMD23 SET_BLOCK_COUNT */
/* Refer to section 7.9.4 in JEDEC Standard No. 84-A43 */
#define SDCC_EMMC_RELIABLE_WRITE_REQUEST    0x80000000

/* extended CSD revision */
/* revision 1.5 was added in JEDEC Standard No. 84-A44 */
/* Refer to section 8.4 in JEDEC Standard No. 84-A44 */
#define SDCC_MMC_EXT_CSD_REV_1_5            5
#define SDCC_MMC_EXT_CSD_REV_1_3            3

/* CMD6 Switch command to set field value in EXT_CSD register */
/* Refer to section 7.9.4 in JEDEC Standard No. 84-A44 */
#define SDCC_MMCPLUS_CHANGE_BUSWIDTH   0x03B70000
#define SDCC_MMCPLUS_ENABLE_HS_TIMING  0x03B90000
#define SDCC_MMCPLUS_HIGH_SPEED        0x100
#define SDCC_MMCPLUS_HIGH_HS200        0x200
#define SDCC_MMCPLUS_HIGH_HS400        0x300
#define SDCC_EMMC_SET_BOOT_PARTITION_ENABLE 0x03B30000
#define SDCC_EMMC_SET_PARTITION_ACCESS 0x03B30000
#define SDCC_EMMC_ENABLE_ERASE_GROUP_DEF 0x03AF0000

/* Used by CMD6 to set the US_PWR_WP_EN and B_PWR_WP_EN bits of the  */
/* EXT_CSD BOOT_WP [173] and USER_WP [171] registers. */
/* Bit[24:25] of the CMD6 argument indicates the Access Mode. */
/* Use Set Bits Access Mode (b01) to set only the PWR_WP_EN bit. Other bits */
/* in the registers are not affected */
#define SDCC_EMMC_ENABLE_BOOT_PWR_WP   0x01AD0100
#define SDCC_EMMC_ENABLE_USER_PWR_WP   0x01AB0100

#define SDCC_EMMC_SANITIZE_START       0x03A50100
#define SDCC_EMMC_ENABLE_HW_RESET      0x03A20100
#define SDCC_EMMC_SET_GPP1_SIZE        0x038F0000
#define SDCC_EMMC_SET_GPP2_SIZE        0x03920000
#define SDCC_EMMC_SET_GPP3_SIZE        0x03950000
#define SDCC_EMMC_SET_GPP4_SIZE        0x03980000
#define SDCC_EMMC_SET_ENH_SIZE         0x038C0000
#define SDCC_EMMC_SET_ENH_START_ADDR   0x03880000
#define SDCC_EMMC_CFG_BYTE1_OFFSET     0x00010000
#define SDCC_EMMC_CFG_BYTE2_OFFSET     0x00020000
#define SDCC_EMMC_CFG_BYTE3_OFFSET     0x00030000
#define SDCC_EMMC_PARTITION_SET_DONE   0x039B0100
#define SDCC_EMMC_SET_PARTITION_ATTR   0x039C0000
#define SDCC_MMC_SET_MODE_CONFIG_FFU   0x031E0100
#define SDCC_MMC_SET_MODE_CONFIG_NORMAL  0x031E0000

/* SDC4_MCLK_SEL */
#define SDCC_FREE_RUNNING_MCLK_DIV_2   0x3

/* Modes selectable from API layer */
enum sdcc_mmc_mode_config
{
   SDCC_MMC_MODE_CONFIG_NORMAL = 0x0,
   SDCC_MMC_MODE_CONFIG_FFU
};

/* FW UPdate Status codes */
enum sdcc_mmc_ffu_status
{
   SDCC_MMC_FFU_NO_ERROR = 0x0,
   SDCC_MMC_FFU_GENERAL_ERROR = 0x10,
   SDCC_MMC_FFU_FW_INSTALL_ERROR = 0x11,
  SDCC_MMC_FFU_FW_DOWNLOAD_ERROR = 0x12
};

#define SDCC_MMC_SET_DATA_VAL(va)      (((va) << 8) & 0x0000ff00)

/* The following definitions are the physical partitions that can be */
/* accessed. */
/* Refer to section 8.4 in JEDEC Standard No. 84-A44 */
#define SDCC_EMMC_PHY_PARTITION_USER_AREA    0
#define SDCC_EMMC_PHY_PARTITION_BOOT_PART_1  1
#define SDCC_EMMC_PHY_PARTITION_BOOT_PART_2  2
#define SDCC_EMMC_PHY_PARTITION_RPMB         3
#define SDCC_EMMC_PHY_PARTITION_GPP_1        4
#define SDCC_EMMC_PHY_PARTITION_GPP_2        5
#define SDCC_EMMC_PHY_PARTITION_GPP_3        6
#define SDCC_EMMC_PHY_PARTITION_GPP_4        7

/* maximum number of general purpose area partitions */
/* Refer to section 7.2.1 in JEDEC Standard No. 84-A44 */
#define SDCC_EMMC_MAX_GPP_NUM   4

/* extended CSD slice number for USER_WP, BOOT_WP, PARTITION_CONFIG */
/* Refer to section 8.4 in JEDEC Standard No. 84-A44 */
#define SDCC_EMMC_EXT_CSD_SLICE_USER_WP           171
#define SDCC_EMMC_EXT_CSD_SLICE_BOOT_WP           173
#define SDCC_EMMC_EXT_CSD_SLICE_PARTITION_CONFIG  179

/* bit position for fields in USER_WP */
/* Refer to section 8.4 in JEDEC Standard No. 84-A44 */
#define SDCC_EMMC_BIT_US_PWR_WP_DIS       3
#define SDCC_EMMC_BIT_US_PERM_WP_EN       2

/* bit position for fields in BOOT_WP */
/* Refer to section 8.4 in JEDEC Standard No. 84-A44 */
#define SDCC_EMMC_BIT_BOOT_PWR_WP_DIS       6
#define SDCC_EMMC_BIT_BOOT_PERM_WP_EN       2
#define SDCC_EMMC_BIT_BOOT_PWR_WP_EN        0

/* Write Protection bits definition */
/* JEDEC Standard No. 84-A441, Table 26 */
#define SDCC_EMMC_WP_NO_PROTECTION          0x00
#define SDCC_EMMC_WP_TEMP_PROTECTION        0x01
#define SDCC_EMMC_WP_POWER_ON_PROTECTION    0x02
#define SDCC_EMMC_WP_PERM_PROTECTION        0x03

/* bit position, number of bits and boot data for fields in PARTITION_CONFIG */
/* Refer to section 8.4 in JEDEC Standard No. 84-A44 */
#define SDCC_EMMC_BIT_BOOT_PARTITION_ENABLE       3
#define SDCC_EMMC_NUM_BITS_BOOT_PARTITION_ENABLE  3
#define SDCC_EMMC_BOOT_DATA_NOT_BOOT_ENABLED      0
#define SDCC_EMMC_BOOT_DATA_BOOT_PART_1_ENABLED   1
#define SDCC_EMMC_BOOT_DATA_BOOT_PART_2_ENABLED   2
#define SDCC_EMMC_BOOT_DATA_USER_AREA_ENABLED     7

/* Revision of EXT_CSD that indicates if device is 5.0 */
#define SDCC_MMC_EXT_CSD_REV_FOR_5_0       7

/* Bit position, number of bits for fields in SUPPORTED_MODES */
/* Refer to section 7.4 in JEDEC Standard No. 84-B451 */
#define SDCC_MMC_BIT_FFU_SUPPORT       0
#define SDCC_MMC_NUM_BITS_FFU_SUPPORT  1

/* Bit position, number of bits for fields in FW_CONFIG */
/* Refer to section 7.4 in JEDEC Standard No. 84-B451 */
#define SDCC_MMC_BIT_FW_UPDATE_DISABLE           0
#define SDCC_MMC_NUM_BITS_FW_UPDATE_DISABLE      1

/* Number of bytes for fields in Firmware version and FFU Argument */
/* Refer to section 7.4 in JEDEC Standard No. 84-B451 */
#define SDCC_MMC_NUM_BYTES_FW_VER        8
#define SDCC_MMC_NUM_BYTES_FFU_ARG       4

/* For cards supporting version 4.0, 4.1 and 4.2 of the JEDEC Standard */
/* No. 84-A43, the TRAN_SPEED value in CSD register shall be 20MHz (0x2A). */
/* For cards supporting version 4.3, the TRAN_SPEED value in CSD register */
/* shall be 26MHz (0x32). */
/* Refer to section 8.3 in JEDEC Standard No. 84-A43 */
#define SDCC_MMC_TRAN_SPEED_20MHZ 0x2A
#define SDCC_MMC_TRAN_SPEED_26MHZ 0x32

/* MMC 4.X compliant card types known from EXT_CSD */
/* Refer to section 8.4 in MMC v4.4 Specification */
#define SDCC_MMC_26MHZ                     (1 << 0)
#define SDCC_MMC_52MHZ                     (1 << 1)
#define SDCC_MMC_DDR_52MHZ                 (1 << 2)
#define SDCC_MMC_DDR_52MHZ_1_2V            (1 << 3)
#define SDCC_HS200_SDR_200MHZ_1_8V         (1 << 4)
#define SDCC_HS200_SDR_200MHZ_1_2V         (1 << 5)
#define SDCC_HS400_MMC_DDR_200MHZ_1_8V     (1 << 6)
#define SDCC_HS400_MMC_DDR_200MHZ_1_2V     (1 << 7)

/* The following definitions are used for checking the card status. */
/* They are used in function sdcc_mmc_set_partition_access. */
/* Refer to section 7.12 in JEDEC Standard No. 84-A43 */
#define SDCC_MMC_SWITCH_ERROR_SHFT    7
#define SDCC_MMC_SWITCH_ERROR_BITMASK 0x00000001
#define SDCC_MMC_SWITCH_ERROR         1

/* The following definitions are used for checking the address out of range */
/* error. */
/* Refer to section 7.13 in JEDEC Standard No. 84-A44 */
#define SDCC_MMC_ADDR_OUT_OF_RANGE_SHFT  31
#define SDCC_MMC_ADDR_OUT_OF_RANGE_ERROR 1

/* The following definition indicates if partition programming is completed */
/* Refer to section 7.4 in JEDEC Standard No. 84-A45 */
#define SDCC_MMC_PARTI_SET_COMPLETED_BMSK  1

/*SDCC_EXTRACT_COMMON_END*/

/*************************************************************/
/*                           MACROS                          */
/*************************************************************/
/* The following macro translates the value of SWITCH_ERROR bit specified in */
/* section 7.12 in JEDEC Standard No. 84-A43. */
#define SDCC_XLATE_SWITCH_ERROR(x) \
   (((x) >> SDCC_MMC_SWITCH_ERROR_SHFT) & SDCC_MMC_SWITCH_ERROR_BITMASK)

/* The following macro translates the value of ADDRESS_OUT_OF_RANGE bit */
/* specified in section 7.13 in JEDEC Standard No. 84-A44. */
#define SDCC_XLATE_ADDR_OUT_OF_RANGE(x) \
   ((x) >> SDCC_MMC_ADDR_OUT_OF_RANGE_SHFT)

/* The following macro calculates the partition size in kBytes into number */
/* of sectors. */
#define SDCC_CALC_PARTITION_SIZE_IN_SECTORS(s) \
   (((s) * 1024) / SDCC_DEFAULT_BLK_LENGTH_SIZE)

/* macro to validate the boot partition value */
/* refer to PARTITION_CONFIG in section 8.4 in JEDEC Standard No. 84-A44 */
#define SDCC_EMMC_IS_BOOT_PARTITION_VAL_VALID(v) \
   !(((v) > SDCC_EMMC_BOOT_DATA_BOOT_PART_2_ENABLED && \
   (v) < SDCC_EMMC_BOOT_DATA_USER_AREA_ENABLED) || \
   (v) > SDCC_EMMC_BOOT_DATA_USER_AREA_ENABLED)

/* macro to map the physical partition number to the value defined for */
/* BOOT_PARTITION_ENABLE in PARTITION_CONFIG register specified in */
/* section 8.4 in JEDEC Standard No. 84-A44 */
#define SDCC_EMMC_MAP_PHY_PART_NUM_TO_BOOT_PART_VAL(n) \
   ((n) == SDCC_EMMC_PHY_PARTITION_USER_AREA ? \
   SDCC_EMMC_BOOT_DATA_USER_AREA_ENABLED : (n))

/* macro to construct the PARTITION_CONFIG value for CMD6 SWITCH */
/* refer to section 8.4 in JEDEC Standard No. 84-A44 */
/* bpe denotes the BOOT_PARTITION_ENABLE value */
/* pa denotes the PARTITION_ACCESS value */
#define SDCC_EMMC_CONSTRUCT_PARTITION_CONFIG_VAL_FOR_SWITCH(bpe, pa) \
   ((((bpe) << SDCC_EMMC_BIT_BOOT_PARTITION_ENABLE) | (pa)) << 8)

/* macro to examine if the queried physical partition number is from the */
/* user area */
#define SDCC_EMMC_IS_FROM_USER_AREA_PART(n) \
   ((n) == SDCC_EMMC_PHY_PARTITION_USER_AREA || \
    (n) == SDCC_EMMC_PHY_PARTITION_GPP_1 || \
    (n) == SDCC_EMMC_PHY_PARTITION_GPP_2 || \
    (n) == SDCC_EMMC_PHY_PARTITION_GPP_3 || \
    (n) == SDCC_EMMC_PHY_PARTITION_GPP_4)

/* macro to examine if the queried physical partition number is from the */
/* boot area */
#define SDCC_EMMC_IS_FROM_BOOT_AREA_PART(n) \
   ((n) == SDCC_EMMC_PHY_PARTITION_BOOT_PART_1 || \
    (n) == SDCC_EMMC_PHY_PARTITION_BOOT_PART_2)

/* macro to extract the specified bit value from a 1-byte register */
/* r denotes the 1-byte register */
/* b denotes the bit position in the register */
/* n denotes number of bits */
/* For example, to extract the bit value of BOOT_PARTITION_ENABLE from */
/* register PARTITION_CONFIG where BOOT_PARTITION_ENABLE is at bit */
/* position [5:3], use the macro as */
/* SDCC_EXTRACT_BIT_VAL_FROM_BYTE_REG(partition_config, 3, 3) */
/* where partition_config is the register, */
/* the 2nd parameter "3" is the start bit position of [5:3], and */
/* the 3rd parameter "3" is the number of bits in [5:3] */
#define SDCC_EXTRACT_BIT_VAL_FROM_BYTE_REG(r, b, n) \
   (((r) >> (b)) & ((1 << (n)) - 1))

#define MCLK_100_112 0
#define MCLK_112_125 1
#define MCLK_125_137 2
#define MCLK_137_150 3
#define MCLK_150_162 4
#define MCLK_162_175 5
#define MCLK_175_187 6
#define MCLK_187_200 7
   
/******************************************************************/
/*                     TYPES DECLARATIONS                         */
/******************************************************************/
/* The Extended CSD register is according to section 8.4 in */
/* JEDEC Standard No.84-A44. */
typedef struct sdcc_ext_csd_t {
   UINT8  s_cmd_set;            /* supported command sets */
   UINT8  ini_timeout_ap;       /* 1st initialization time after partitioning */
   UINT8  pwr_cl_ddr_52_360;    /* power class for 52MHz, DDR at 3.6V */
   UINT8  pwr_cl_ddr_52_195;    /* power class for 52MHz, DDR at 1.95V */
   UINT8  min_perf_ddr_w_8_52;  /* minimum write performance for 8bit at */
                                /* 52MHz in DDR mode */
   UINT8  min_perf_ddr_r_8_52;  /* minimum read performance for 8bit at */
                                /* 52MHz in DDR mode */
   UINT8  trim_mult;            /* TRIM multiplier */
   UINT8  sec_feature_support;  /* secure feature support */
   UINT8  sec_erase_mult;       /* secure erase multiplier */
   UINT8  sec_trim_mult;        /* secure TRIM multiplier */
   UINT8  boot_info;            /* boot information */
   UINT8  boot_size_multi;      /* boot partition size */
   UINT8  acc_size;             /* access size */
   UINT8  hc_erase_grp_size;    /* high-capacity erase unit size */
   UINT8  erase_timeout_mult;   /* high-capacity erase timeout */
   UINT8  rel_wr_sec_c;         /* reliable write sector count */
   UINT8  hc_wp_grp_size;       /* high-capacity write protect group size */
   UINT8  s_c_vcc;              /* sleep current (VCC) */
   UINT8  s_c_vccq;             /* sleep current (VCCQ) */
   UINT8  s_a_timeout;          /* sleep/awake timeout */
   UINT32 sec_count;            /* sector count */
   UINT8  min_perf_w_8_52;      /* min write performance for 8bit @52MHz */
   UINT8  min_perf_r_8_52;      /* min read performance for 8bit @52MHz */
   UINT8  min_perf_w_8_26_4_52; /* min write performance for 8bit @26MHz */
                                /* or 4bit @52MHz */
   UINT8  min_perf_r_8_26_4_52; /* min read performance for 8bit @26MHz */
                                /* or 4bit @52MHz */
   UINT8  min_perf_w_4_26;      /* min write performance for 4bit @26MHz */
   UINT8  min_perf_r_4_26;      /* min read performance for 4bit @26MHz */
   UINT8  pwr_cl_26_360;        /* power class for 26MHz @3.6V */
   UINT8  pwr_cl_52_360;        /* power class for 52MHz @3.6V */
   UINT8  pwr_cl_26_195;        /* power class for 26MHz @1.95V */
   UINT8  pwr_cl_52_195;        /* power class for 52MHz @1.95V */
   UINT8  card_type;            /* card type: 1 means 26MHz, 3 means 52MHz */
   UINT8  csd_structure;        /* CSD structure version */
   UINT8  ext_csd_rev;          /* extended CSD revision */
   UINT8  cmd_set;              /* command set */
   UINT8  cmd_set_rev;          /* command set revision */
   UINT8  power_class;          /* power class */
   UINT8  hs_timing;            /* high speed interface timing */
   UINT8  bus_width;            /* bus width mode */
   UINT8  strobe_support;       /* enhanced strobe mode support */
   UINT8  erased_mem_cont;      /* erased memory content */
   UINT8  partition_config;     /* partition configuration */
   UINT8  boot_bus_width;       /* boot bus width */
   UINT8  erase_group_def;      /* high-density erase group definition */
   UINT8  boot_wp;              /* boot area write protection register */
   UINT8  user_wp;              /* user area write protection register */
   UINT8  supported_modes;      /* Supported device modes*/
   UINT8  fw_config;            /* FW configuration */
   UINT8  mode_config;          /* Mode configuration */
   UINT8  ffu_status;           /* FFU status */
   UINT32 ffu_arg;              /* FFU Argument */
   UINT64 fw_ver;               /* FW Version */
   UINT8  rpmb_size_mult;       /* RPMB size */
   UINT8  rst_n_function;       /* H/W reset function */
   UINT8  partitioning_support; /* partitioning support */
   UINT32 max_enh_size_mult;    /* max enhanced area size */
   UINT8  partitions_attribute; /* partitions attribute */
   UINT8  partition_setting_completed; /* partitioning setting */
   UINT32 gp_size_mult[SDCC_EMMC_MAX_GPP_NUM]; /* general purpose partition */
                                               /* size */
   UINT32 enh_size_mult;        /* enhanced user data area size */
   UINT32 enh_start_addr;       /* enhanced user data start address */
   UINT8  sec_bad_blk_mgmnt;    /* bad block management mode */
} sdcc_ext_csd_type;

/*********************************************************************/
/*                           PROTOTYPES                              */
/*********************************************************************/
/******************************************************************************
* Name: sdcc_find_mmc_device
*
* Description: 
*    This function looks for the MMC device of the specified driveno.
*
* Parameters:
*    driveno [IN] : a valid drive number between 0 and the total number of card
*                   slots minus one
*
* Returns:
*    card type that indicates if device is MMC or unknown
*
******************************************************************************/
SDCC_CARD_TYPE sdcc_find_mmc_device ( struct sdcc_device *handle );

/******************************************************************************
* Name: sdcc_mmc_switch
*
* Description:
*    This function sends CMD6 SWITCH to the memory device to modify the EXT_CSD
*    registers.  It then checks if switch error occurs.
*
* Arguments:
*    sdcc_pdev  [IN] : pointer to device type
*    switch_arg [IN] : command argument for CMD6 SWITCH
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
SDCC_STATUS sdcc_mmc_switch( sdcc_dev_type *sdcc_pdev, UINT32 switch_arg );

/******************************************************************************
* Name: sdcc_config_mmc_modes_segment
*
* Description:
*    This function programs the modes segment in EXT_CSD register of the
*    MMC/eMMC device.  It gets the EXT_CSD information and sends CMD6 SWITCH
*    command to configure the HS_TIMING and BUS_WIDTH.  It also calculates the
*    card size for the device that is higher than 2GB, boot partition size and
*    write protect group size.
*
* Arguments:
*    sdcc_pdev    [IN/OUT] : pointer to device type
*
* Returns:
*    Returns error code
*
* Note:
*    Card is in data transfer mode.
*
******************************************************************************/
SDCC_STATUS sdcc_config_mmc_modes_segment( sdcc_dev_type *sdcc_pdev );

/******************************************************************************
* Name: sdcc_config_mmc_modes_segment_decode
*
* Description:
*    This function decodes the already available data such as EXT_CSD, bus width
*
* Arguments:
*    sdcc_pdev    [IN/OUT] : pointer to device type
*    raw_ext_csd  [IN]     : pointer to raw EXT CSD
*
* Returns:
*    Returns error code
*
* Note:
*    Card is in data transfer mode.
*
******************************************************************************/
SDCC_STATUS 
sdcc_config_mmc_modes_segment_decode( sdcc_dev_type *sdcc_pdev, UINT8 *raw_ext_csd );

/******************************************************************************
* Name: sdcc_get_ext_csd
*
* Description:
*    This function gets the EXT_CSD register as a block of data from the
*    MMCPlus card.  It performs the adtc command with the read operation.
*
* Arguments:
*    sdcc_pdev              [IN] : pointer to device type
*    buff                  [OUT] : pointer to buffer for the incoming data
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS sdcc_get_ext_csd( sdcc_dev_type   *sdcc_pdev,
                              UINT8                  *buff );

/******************************************************************************
* Name: sdcc_decode_extCSD
*
* Description:
*    This function is to parse out the Extended CSD data.
*
* Arguments:
*    data          [IN] : pointer to buffer that contains ext_csd data
*    ext_csd      [OUT] : pointer to the structure to save the ext csd data to
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
BOOLEAN sdcc_decode_extCSD( const UINT8          *data,
                            sdcc_ext_csd_type   *ext_csd );

/******************************************************************************
* Name: sdcc_decode_mmc_cid
*
* Description:
*    This function is to parse out the Card Identification (CID) register.
*
* Arguments:
*    raw_cid        [IN] : pointer to raw CID info
*    sdcc_pdev     [OUT] : pointer to device type
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
BOOLEAN sdcc_decode_mmc_cid( const UINT32   *raw_cid,
                             sdcc_dev_type  *sdcc_pdev );

/******************************************************************************
* Name: sdcc_decode_mmc_mfr_date
*
* Description:
*    This function decodes the manufacturing date as mm/yyyy format.
*
* Arguments:
*    mfr_date       [IN] : raw manufacturing date
*    sdcc_pdev     [OUT] : pointer to device type
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
BOOLEAN sdcc_decode_mmc_mfr_date ( UINT8 mfr_date, sdcc_dev_type *sdcc_pdev);

/******************************************************************************
* Name: sdcc_mmc_set_partition_access
*
* Description:
*    This function sets the partition for read and write operation by
*    sending CMD6 SWITCH to the eMMC device.
*
* Arguments:
*    handle     [IN] : a pointer to the SDCC device that was returned from
*                      sdcc_handle_open()
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
SDCC_STATUS sdcc_mmc_set_partition_access( struct sdcc_device *handle );

/******************************************************************************
* Name: sdcc_mmc_get_phy_partition_info
*
* Description: 
*    This function gets the physical partition information on eMMC device.
*
* Parameters:
*    handle                     [IN] : a pointer to the SDCC device that was
*                                      returned from sdcc_handle_open()
*    partition_size_in_sectors [OUT] : physical partition size in sectors
*    phy_partition_type        [OUT] : type of physical partition to indicate
*                                      if partition is the user area, boot
*                                      partition, general purpose partition,
*                                      etc.
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
SDCC_STATUS sdcc_mmc_get_phy_partition_info(
   struct sdcc_device *handle, UINT32 *partition_size_in_sectors,
   UINT32 *phy_partition_type );

/******************************************************************************
* Name: sdcc_mmc_set_pwr_on_write_prot
*
* Description: 
*    This function sets the power-on period write protection to the physical
*    partition specified in parameter handle.  If the physical partition is
*    the user area, the write protection is applied to the specified parameter
*    data_address.  Parameter num_of_blocks should be aligned with the write
*    protect group size specified in write_protect_group_size_in_sectors in
*    structure sdcc_mem_info_type.  If the physical partition is the boot area,
*    the write protection is enabled to the entire boot partition and both boot
*    areas would get write protected.
*
* Parameters:
*    handle        [IN] : a pointer to the SDCC device that was returned from
*                         sdcc_handle_open()
*    data_address  [IN] : start of the sector to get power-on write protected
*    num_of_blocks [IN] : number of blocks to be power-on write protected
*                         (512 bytes per block)
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
SDCC_STATUS sdcc_mmc_set_pwr_on_write_prot(
   struct sdcc_device *handle, UINT32 data_address, UINT32 num_of_blocks );

/******************************************************************************
* Name: sdcc_mmc_set_active_bootable_partition
*
* Description: 
*    This function sets the BOOT_PARTITION_ENABLE bits specified in parameter
*    handle.  It clears the PARTITION_ACCESS bits if the current active
*    partition is not the user area.  It then sends the SWITCH command to the
*    eMMC device to set the BOOT_PARTITION_ENABLE bits in the EXT_CSD register.
*
* Parameters:
*    handle        [IN] : a pointer to the SDCC device that was returned from
*                         sdcc_handle_open()
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
SDCC_STATUS
sdcc_mmc_set_active_bootable_partition(struct sdcc_device *handle);

/******************************************************************************
* Name: sdcc_mmc_is_phy_partition_bootable
*
* Description: 
*    This function examines if the partition specified in parameter handle is
*    the bootable partition. 
*
* Parameters:
*    handle        [IN] : a pointer to the SDCC device that was returned from
*                         sdcc_handle_open()
*
* Returns:
*    TRUE if partition is bootable.  FALSE otherwise.
*
******************************************************************************/
BOOLEAN sdcc_mmc_is_phy_partition_bootable(struct sdcc_device *handle);

/******************************************************************************
* Name: sdcc_mmc_get_write_prot_type
*
* Description:
*    This function gets the write protection type applied to the write protection
*    groups starting from the group containing the passed in data_address. If the
*    physical partition is the user area, this function returns an array of
*    8 bytes (64 bits) representing the write protection type of the 32 write
*    protection groups starting from the data_address group. The least significant
*    two bits (i.e bits[1:0] of write_prot_type[7]) correspond to the first
*    addressed group. If the physical partition is the boot area, bits[1:0] of
*    write_prot_type[7] represent the write protection type for both the BOOT1 and
*    BOOT2 partitions.
*
* Parameters:
*    handle [IN]           : a pointer to the SDCC device that was returned from
*                            sdcc_handle_open()
*    data_address [IN]     : start of the sector to get the write protection type
*    write_prot_type [OUT] : a pointer to an 8-byte array. This array contains 64 bits
*                            representing write protection type applied to
*                            the protection groups
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
SDCC_STATUS sdcc_mmc_get_write_prot_type(
   struct sdcc_device *handle, UINT32 data_address, UINT8  *write_prot_type );
/******************************************************************************
* Name: sdcc_mmc_config_gpp_enh
*
* Description:
*    This function configures the General Purpose Partition and 
*    enhanced user data area information
*    Note: This routine can only be used once for the life of each
*    emmc chip!
*
* Arguments:
*    handle     [IN] : a pointer to the SDCC device that was returned from
*                      sdcc_handle_open()
*    desc       [IN] : a pointer to the structure that describes the layout
*                      of the GPP and ENH partition information
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
SDCC_STATUS
sdcc_mmc_config_gpp_enh (struct sdcc_device *handle,
                         sdcc_emmc_gpp_enh_desc_type *desc);

/******************************************************************************
* Name: sdcc_hs200_execute_tuning
*
* Description:
*    This function executes the tuning sequence to find the best phase for HS200 operation 
*    and configure the selected phase.
*
* Arguments:
*    sdcc_pdev  [IN] : a pointer to the SDCC device that was returned from
*                      sdcc_handle_open()
*
* Returns:
*    SDCC_NO_ERROR if successful.  Error code otherwise.
*
******************************************************************************/
SDCC_STATUS
sdcc_hs200_execute_tuning (sdcc_dev_type *sdcc_pdev);                         

/******************************************************************************
* Name: sdcc_mmc_get_ext_csd_rev
*
* Description:
*    This function retrives the EXT_CSD register version which provides
*    information on the MMC Device Standard.
*
* Parameters:
*    handle        [IN] : a pointer to the SDCC device that was returned from
*                         sdcc_handle_open()
*
* Returns:
*    Returns the EXT_CSD Revision.
*
******************************************************************************/
UINT8
sdcc_mmc_get_ext_csd_rev (struct sdcc_device *handle);


/******************************************************************************
* Name: sdcc_mmc_is_ffu_allowed
*
* Description:
*    This function checks if field firmware update supported by the device
*
* Parameters:
*    handle        [IN] : a pointer to the SDCC device that was returned from
*                         sdcc_handle_open()
* Returns:
*    TRUE if FW Update is supported and False otherwise
*
******************************************************************************/
BOOLEAN
sdcc_mmc_is_ffu_allowed (struct sdcc_device *handle);


/******************************************************************************
* Name: sdcc_mmc_set_mode_config
*
* Description:
*    This function sets the devcie mode config to specified mode
*
* Parameters:
*    handle        [IN] : a pointer to the SDCC device that was returned from
*                         sdcc_handle_open()
*    mode          [IN] : mode to set
*
* Returns:
*    SDCC_NO_ERROR if successful set the mode.  Error code otherwise.
*
******************************************************************************/
SDCC_STATUS
sdcc_mmc_set_mode_config (struct sdcc_device *handle,
                          enum sdcc_mmc_mode_config mode);


/******************************************************************************
* Name: sdcc_mmc_get_ffu_argument
*
* Description:
*    This function returns the value of the FFU_ARG field to be used with the
*    program command.
*
* Parameters:
*    handle        [IN] : a pointer to the SDCC device that was returned from
*                         sdcc_handle_open()
* Returns:
*    FFU_ARG field value of the EXT_CSD Register.
*
******************************************************************************/
UINT32
sdcc_mmc_get_ffu_argument (struct sdcc_device *handle);


/******************************************************************************
* Name: sdcc_mmc_get_ffu_status
*
* Description:
*    This function returns the status of previously issued Firmware update.
*
* Parameters:
*    handle        [IN] : a pointer to the SDCC device that was returned from
*                         sdcc_handle_open()
* Returns:
*    Status of the MMC Firmware update.
*
******************************************************************************/
SDCC_STATUS
sdcc_mmc_get_ffu_status  (struct sdcc_device *handle);


/******************************************************************************
* Name: sdcc_mmc_get_fw_version
*
* Description:
*    This function returns the Firmware Version of the device.
*
* Parameters:
*    handle        [IN] : a pointer to the SDCC device that was returned from
*                         sdcc_handle_open()
* Returns:
*    The Firmware Version of the device.
*
******************************************************************************/
UINT64
sdcc_mmc_get_fw_version  (struct sdcc_device *handle);

#endif /* __SDCC_MMC_UTIL_H */

