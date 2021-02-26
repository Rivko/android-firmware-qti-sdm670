#ifndef __SDCC_PRIV_H
#define __SDCC_PRIV_H

/**********************************************************************
 * sdcc_priv.h
 *
 * This file provides SDCC internal definitions.
 *
 * Copyright (c) 2014-2015,2017 by Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 **********************************************************************/
/*======================================================================

                        EDIT HISTORY FOR MODULE


when         who     what, where, why
----------   ---     ----------------------------------------------------
2017-05-12   sb      Add eMMC support for SDM670  
2015-10-14   rm      move busy wait to SdccCommon.h
2015-07-15   bn      Code clean up
2015-02-10   bn      Added busywait() to use as delay function
2015-01-26   rm      Integraged HS200/HS400 support for eMMC from UEFI 3.0
2014-09-24   bn      Code cleanup
2014-06-18   bn      Initial version. Branched from 8994 SBL

==================================================================*/

/**********************************************************/
/*                    INCLUDES                            */
/**********************************************************/
#ifdef DPRINTF
#undef DPRINTF
#define DPRINTF(x)
#endif

#include "sdcc_api.h"
#include "SdccCommon.h"
#include <Library/DebugLib.h>
#include <Library/SdccCommonLib/SdccHalLib/SdccHal.h>

/*************************************************************/
/*                     DEFINES                               */
/*************************************************************/
#ifndef SDCC_MAX_NUM_CONTROLLER 
#define SDCC_MAX_NUM_CONTROLLER   4
#endif

/* Useful for slot validation */
#define SDCC_INVALID_SLOT SDCC_MAX_NUM_CONTROLLER

#define SDCC_CMD8_RETRIES           3

/* Number of times to attempt to clear the status register                 */
/* before timeout occurs. This number is obtained though an educated       */
/* guess process.  The number of trys depending on the speed difference    */
/* between the MBUS and PBUS speed.  The worse case will be when the       */
/* MBUS is slow and PBUS is fast.  Assume a MBUS speed of 150kHz and       */
/* PBUS speed of 100MHz, it will take 666 PBUS cycle for the reset         */
/* to sync up.  A while loop used to reset the status register is about    */
/* 10 instruction in length, so it would normall take 70 retries to clear  */
/* the status register.  Since we normally would not want the timeout      */
/* to occur very often, it is reasonable to multiply this number by a      */
/* large constant, say 10.                                                 */
#define SDCC_CLR_STATUS_RETRIES     (1000)

/* Per section 6.4.1 in SD2.0 System Specification, */
/* time out value for initialization process is 1 second. */
#define SDCC_SD_DETECT_MAX_BUSY_MS           1000
/* time interval (in ms) between the ACMD41 retry */
/* Refer to section 3.6 of SD Specifications Part A2 SD Host Controller */
/* Standard Specifrication Version 2.00 */
#define SDCC_SD_DETECT_BUSY_RETRY_INTERVAL   5

/* Time (in ms) to wait until the card can start accepting commands */
/* Refer to section 6.4 of SD Specifications Part 1 Physical Layer */
/* Specification Version 2.00 */
#define SDCC_SUPPLY_RAMP_UP_TIME_MS   1

/* Time (in ms) to wait after changing the clock frequency before */
/* clock is stable.  This value is currently an conservative guess. */
#define  SDCC_CLK_RAMP_TIME_MS        1

/* Check if driveno is good */
#define SDCC_DRIVENO_IS_VALID(x) ((x) < SDCC_MAX_NUM_CONTROLLER)

/* translate the physical partition number */
#define SDCC_XLATE_PARTITION_NUM(n) \
   ((SDCC_HANDLE_PHY_PARTITION_ENTIRE_MEM_DEVICE == (n)) ? 0 : (n))

/* SDCC_SD_DETECT_MAX_RETRIES is derived from the the maximum time out value */
/* of the initialization process.  The time after the end of the first */
/* ACMD41 to card ready is 1 second. */
/* That is 1000 milli-second = 20 x 50 milli-seond where 50 milli-second is */
/* wait time between each ACMD41 retry. */
/* Refer to section 6.4.1 in SD2.0 System Specification */
#define SDCC_SD_DETECT_MAX_RETRIES \
   (SDCC_SD_DETECT_MAX_BUSY_MS / SDCC_SD_DETECT_BUSY_RETRY_INTERVAL)

#define SDCC_STATUS_POLL_MAX        0x80000

/* Wait for PROG_DONE bit timeout. Unit is in micro second */
#define SDCC_PROG_DONE_MAX          5000000
#define SDCC_DEFAULT_BLK_LENGTH_SIZE 512

#define SDCC_CARD_ERASE_FAIL        0xFDFF8000   /* erase failure mask */
#define SDCC_NEGOTIATE_OCR          0x00FF8000   /* voltage 2.7-3.6 */

#define SDCC_CARD_STATUS_BMSK       0x0000000F
#define SDCC_CARD_STATUS_SHFT       9

#define SDCC_CARD_STATUS_BLKLEN_ERR 0x20000000

/* number of bits to be shifted for RCA in CMD13 */
/* Refer to section 4.7.4 in SD2.0 Spec. */
#define SDCC_SEND_STATUS_RCA_SHFT   16
#define SDCC_ARG_RCA(rca) ((UINT32)(rca) << SDCC_SEND_STATUS_RCA_SHFT)

/* The following macro is used for the memory access commands such as */
/* CMD17/18, CMD24/25, CMD28.  It examines the card type and returns the */
/* command argument according to the two notes below. */
/* For media <= 2GB card size, the data address is in 32bit byte address. */
/* For media > 2GB, the data address is in a 32bit sector (512Byte) address. */
/* Refer to section 7.10.4 in JEDEC Standard No. 84-A44 or */
/* section 4.3.14 in SD2.0 System Specification */
/* c represents the card type. */
/* a represents the data address. */
/* bl represents the block length. */
#define SDCC_MEM_ACCESS_CMD_ARG(c, a, bl) \
   (((c) == SDCC_CARD_SDHC || (c) == SDCC_CARD_MMCHC) ? (a) : ((a) * (bl)))

#define SDCC_DATA_READ              0x0UL
#define SDCC_DATA_WRITE             0x1UL

/* There is a very small chance that between 'DM setup */
/* and start read from SD card', the CPU can get busy  */
/* doing other high priority things, the HW timer started initially can */
/* timeout, and subsequently the transfer will not proceed. This is observed */
/* in few customer setups. We’ve never seen this situation before in L4 */
/* targets, but this was noticed once in Windows Phone, CR 255489 . */
#define SDCC_READ_TIMEOUT_MS        5000
#define SDCC_WRITE_TIMEOUT_MS       400

/* MMC card can have a much longer timeout */
#define SDCC_MMC_WRITE_TIMEOUT_MS   2000

#define SDCC_CMD_ARGU_NULL          0x0

/* MMC partition isuse mask bit-wise flag */
#define SDCC_BMSK_USER_PARTITION    0x01
#define SDCC_BMSK_BOOT1_PARTITION   0x02
#define SDCC_BMSK_BOOT2_PARTITION   0x04
#define SDCC_BMSK_RPMB_PARTITION    0x08
#define SDCC_BMSK_GP1_PARTITION     0x10
#define SDCC_BMSK_GP2_PARTITION     0x20
#define SDCC_BMSK_GP3_PARTITION     0x40
#define SDCC_BMSK_GP4_PARTITION     0x80
#define SDCC_XLATE_PARTITION_ID_TO_MASK(x)     (0x01 << x)

/* The following definitions are used for checking the card status. */
/* They are used in function sdcc_poll_card_status. */
/* Refer to section 7.12 in JEDEC Standard No. 84-A43 */
#define SDCC_MEM_CURRENT_STATE_SHFT         9
#define SDCC_MEM_CURRENT_STATE_BITMASK      0x0000000F
#define SDCC_MEM_READY_FOR_DATA_SHFT        8
#define SDCC_MEM_READY_FOR_DATA_BITMASK     0x00000001
#define SDCC_MEM_READY_FOR_DATA             1
#define SDCC_POLL_CARD_MAX                  0xF000

/* The following two macros translate the value of the CURRENT_STATE bits */
/* and READY_FOR_DATA bit specified in section 7.12 in JEDEC Standard */
/* No. 84-A43 */
#define SDCC_XLATE_CARD_CURRENT_STATE(x) \
   (((x) >> SDCC_MEM_CURRENT_STATE_SHFT) & SDCC_MEM_CURRENT_STATE_BITMASK)
#define SDCC_XLATE_READY_FOR_DATA(x) \
   (((x) >> SDCC_MEM_READY_FOR_DATA_SHFT) & SDCC_MEM_READY_FOR_DATA_BITMASK)

/* The following definitions are used to examine if a write violation */
/* occurs.  That is if there is an attempt to program a write protected block */
/* Refer to section 7.13 in JEDEC Standard No. 84-A441 */
#define SDCC_WP_VIOLATION_SHFT    26
#define SDCC_WP_VIOLATION_BITMASK 0x00000001
#define SDCC_WP_VIOLATION_ERROR   1

/* The following macro translates the value of WP_VIOLATION bit specified in */
/* section 7.13 in JEDEC Standard No. 84-A441. */
#define SDCC_XLATE_WP_VIOLATION(x) \
   (((x) >> SDCC_WP_VIOLATION_SHFT) & SDCC_WP_VIOLATION_BITMASK)

/*************************************************************/
/*             SDHC SPECIFIC DEFINES                         */
/*************************************************************/
/* CSD Version 2.0 for High Capacity SD memory card */
#define SDCC_CSD_TAAC        0x0E /* data read access-time is 1ms */
#define SDCC_CSD_NSAC        0x00 /* data read access-time in CLK cycles */
#define SDCC_CSD_R2W_FACTOR  0x02 /* block program time as a mult. of the read access time */
#define SDCC_CSD_READ_BL_LEN 0x9 /* max. read data block length is 512 bytes */

#define SDCC_DEVICE_RDY             0x80000000   /* card is ready */
#define SDCC_MEM_BLK_MODE           0x1
#define SDCC_MEM_BYTE_MODE          0x0

#define SDCC_POLL_CARD_MAX          0xF000

/*************************************************************/
/*             CLKREGIM SPECIFIC DEFINES                         */
/*************************************************************/
#define SDCC_DEFAULT_CLK_KHZ           50000

/*************************************************************/
/*             ENUMERATIONS                                  */
/*************************************************************/
/* Standard commands supported by the Multimedia Card */
enum SD_CMD
{
   SD_CMD0_GO_IDLE_STATE = 0,
   SD_CMD1_SEND_OP_COND,
   SD_CMD2_ALL_SEND_CID,
   SD_CMD3_SET_RELATIVE_ADDR,
   SD_CMD4_SET_DSR,
   SD_CMD5_IO_SEND_OP_COND,
   SD_CMD6_SWITCH_FUNC,                         /* SD CMD6 adtc cmd type */
   SD_CMD6_SWITCH = SD_CMD6_SWITCH_FUNC,        /* MMC CMD6 ac cmd type */
   SD_CMD7_SELECT_CARD,
   SD_CMD8_SEND_IF_COND,                        /* SD CMD8 bcr cmd type */
   SD_CMD8_SEND_EXT_CSD = SD_CMD8_SEND_IF_COND, /* MMC CMD8 adtc cmd type */
   SD_CMD9_SEND_CSD,
   SD_CMD10_SEND_CID,
   SD_CMD11_READ_DAT_UNTIL_STOP,
   SD_CMD12_STOP_TRANSMISSION,
   SD_CMD13_SEND_STATUS,
   SD_CMD14,
   SD_CMD15_GO_INACTIVE_STATE,
   SD_CMD16_SET_BLOCKLEN,
   SD_CMD17_READ_BLOCK,
   SD_CMD18_READ_MULTIPLE_BLOCK,
   SD_CMD19,
   SD_CMD20_WRITE_DAT_UNTIL_STOP,
   SD_CMD21,
   SD_CMD22,
   SD_CMD23_SET_BLOCK_COUNT, /* eMMC command index */
   SD_CMD24_WRITE_BLOCK,
   SD_CMD25_WRITE_MULTIPLE_BLOCK,
   SD_CMD26,
   SD_CMD27_PROGRAM_CSD,
   SD_CMD28_SET_WRITE_PROT,
   SD_CMD29_CLR_WRITE_PROT,
   SD_CMD30_SEND_WRITE_PROT,
   SD_CMD31,
   SD_CMD32_TAG_SECTOR_START,
   SD_CMD33_TAG_SECTOR_END,
   SD_CMD34_READ_SEC_CMD = 34,     /* SD Mc-EX specific READ_SEC_CMD */
   SD_CMD34_UNTAG_SECTOR = 34,     /* MMC command index */
   SD_CMD35_WRITE_SEC_CMD = 35,         /* SD Mc-EX specific WRITE_SEC_CMD */
   SD_CMD35_TAG_ERASE_GROUP_START = 35, /* MMC command index with ac cmd type */
   SD_CMD36_SEND_PSI = 36,            /* SD Mc-EX specific SEND_PSI */
   SD_CMD36_TAG_ERASE_GROUP_END = 36, /* MMC command index with ac cmd type */
   SD_CMD37_CONTROL_TRM = 37,       /* SD Mc-EX specific CONTROL_TRM */
   SD_CMD37_UNTAG_ERASE_GROUP = 37, /* MMC command index */
   SD_CMD38_ERASE,
   SD_CMD39_FAST_IO,
   SD_CMD40_GO_IRQ_STATE,
   SD_CMD41,
   SD_CMD42_LOCK_UNLOCK,
   SD_CMD43_SELECT_PARTITION,         /* eSD SELECT_PARTITION */
   SD_CMD50_DIRECT_SECURE_READ = 50,  /* SD Mc-EX specific DIRECT_SECURE_READ */
   SD_CMD52_IO_RW_DIRECT = 52,
   SD_CMD53_IO_RW_EXTENDED,
   SD_CMD55_APP_CMD = 55,
   SD_CMD56_GEN_CMD,
   SD_CMD57_DIRECT_SECURE_WRITE = 57 /* SD Mc-EX specific DIRECT_SECURE_WRITE */
};

/* Application-specific commands supported by all SD cards */
enum SD_ACMD
{
   SD_ACMD6_SET_BUS_WIDTH=6,
   SD_ACMD13_SD_STATUS=13,
   SD_ACMD18_SECURE_READ_MULTI_BLOCK=18,
   SD_ACMD22_SEND_NUM_WR_BLOCKS=22,
   SD_ACMD23_SET_WR_BLK_ERASE_COUNT=23,
   SD_ACMD25_SECURE_WRITE_MULTI_BLOCK=25,
   SD_ACMD26_SECURE_WRITE_MKB=26,
   SD_ACMD38_SECURE_ERASE=38,
   SD_ACMD41_SD_APP_OP_COND=41,
   SD_ACMD42_SET_CLR_CARD_DETECT=42,
   SD_ACMD43_GET_MKB=43,
   SD_ACMD44_GET_MID=44,
   SD_ACMD45_SET_CER_RN1=45,
   SD_ACMD46_GET_CER_RN2=46,
   SD_ACMD47_SET_CER_RES2=47,
   SD_ACMD48_GET_CER_RES1=48,
   SD_ACMD49_CHANGE_SECURE_AREA=49,
   SD_ACMD51_SEND_SCR=51
};

/******************************************************************/
/*                            TYPES                               */
/******************************************************************/

/**
 * Erase type supported
 */
typedef enum
{
   EMMC_ERASE = 0,
   EMMC_TRIM = 1,
} emmc_erase_type;

typedef struct sdcc_csd_t
{
   UINT8  csd_structure;
   UINT8  spec_vers;
   UINT8  taac;
   UINT8  nsac;
   UINT8  tran_speed;
   UINT16 ccc;
   UINT8  read_bl_len;
   UINT8  read_bl_partial;
   UINT8  write_blk_misalign;
   UINT8  read_blk_misalign;
   UINT8  dsr_imp;
   UINT32 c_size;
   UINT8  vdd_r_curr_min;
   UINT8  vdd_r_curr_max;
   UINT8  vdd_w_curr_min;
   UINT8  vdd_w_curr_max;
   UINT8  c_size_mult;
   union {
      struct { /* MMC system specification version 3.1 */
         UINT8  erase_grp_size;
         UINT8  erase_grp_mult;
      } v31;
      struct { /* MMC system specification version 2.2 */
         UINT8  sector_size;
         UINT8  erase_grp_size;
      } v22;
   } erase;
   UINT8  wp_grp_size;
   UINT8  wp_grp_enable;
   UINT8  default_ecc;
   UINT8  r2w_factor;
   UINT8  write_bl_len;
   UINT8  write_bl_partial;
   UINT8  file_format_grp;
   UINT8  copy;
   UINT8  perm_write_protect;
   UINT8  tmp_write_protect;
   UINT8  file_format;
   UINT8  crc;
} sdcc_csd_type;

typedef struct sdcc_cmd_t {
   UINT32              cmd;
   UINT32              cmd_arg;
   SDCC_CARD_RESP      resp_type;
   UINT32              resp[4];
   UINT32              prog_scan;
   UINT32              status;
   UINT32              flags;
} sdcc_cmd_type;


typedef struct _sdcc_mini_boot_device_info
{
   UINT32                           driveno;   /**< drive number */
   SDCC_CARD_TYPE                   card_type; /**< card type SD or MMC */
   UINT16                           rca;       /**< relative card address */
} sdcc_mini_boot_device_info_type;

/* bit-wise flags indicating transfer type */
#define SDCC_TRANSFER_FLAG_IS_CMD_ONLY        0x0 
#define SDCC_TRANSFER_FLAG_IS_WRITE_TRANSFER  0x1
#define SDCC_TRANSFER_FLAG_IS_READ_TRANSFER   0x2
#define SDCC_TRANSFER_FLAG_IS_APP_COMMAND     0x4

#define SDCC_IS_READ_TRANSFER(flags)                             \
   (flags & SDCC_TRANSFER_FLAG_IS_READ_TRANSFER) 
#define SDCC_IS_WRITE_TRANSFER(flags)                            \
   (flags & SDCC_TRANSFER_FLAG_IS_WRITE_TRANSFER)
#define SDCC_DATA_DIRECTION(flags)                               \
   (SDCC_IS_READ_TRANSFER(flags) ? SDCC_DATA_READ : SDCC_DATA_WRITE)
#define SDCC_IS_APP_COMMAND(flags)                               \
   (flags & SDCC_TRANSFER_FLAG_IS_APP_COMMAND)

#define HS200_INVALID_TUNING_PHASE -1
#define MAX_TUNING_PHASES 16
#define START_PHASE_INDEX 0
#define MAX_PHASE_INDEX 15
#define HZ_TO_MHZ(n) ((n) / 1000000)

typedef enum sdcc_mmc_buswidth_t
{
   SDCC_MMC_BUSWIDTH_1BIT     = 0,
   SDCC_MMC_BUSWIDTH_4BIT     = 1,
   SDCC_MMC_BUSWIDTH_8BIT     = 2,
   SDCC_MMC_BUSWIDTH_DDR_4BIT = 5,
   SDCC_MMC_BUSWIDTH_DDR_8BIT = 6  
} SDCC_MMC_BUSWIDTH;

typedef enum sdcc_speed_mode_t
{
   SDCC_DDR50_MODE     = 0,
   SDCC_HS200_MODE     = 1,
   SDCC_HS400_MODE     = 2,
   SDCC_SDR_MODE       = 3
} SDCC_SPEED_MODE;

#endif /* ifdef __SDCC_PRIV_H */

