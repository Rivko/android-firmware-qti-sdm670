#ifndef __FLASH_PROG_ERROR_H__
#define __FLASH_PROG_ERROR_H__

/*==================================================================
 *
 * FILE:        flash_prog_error.h
 *
 * SERVICES:    Flash Programming Plug-In for Lauterbach JTAG/ICD TRACE32
 *
 * DESCRIPTION: Extended Error Codes for NAND CMM scripts
 *
 * Copyright (c) 2008-2009, 2011 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *==================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 08/18/11     sv      Update comment for ERR_DEV_MISSING
 * 03/18/11     jz      Added error code ERR_PARTI_TOO_BIG
 * 07/23/09     mm      Added extended partition error code
 * 10/29/08     mm      Initial Revision
 *==================================================================*/

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*  The format of this enum definition is VERY critical.  It is parsed by
 *  a Perl script to create nand_ext_error.cmm which allows verbose
 *  error reporting and prints out these human readable comments.
 *
 *  Format is:
 *
 *  ERR_something, where something is all caps
 *  no spaces, followed by =hex value, with no spaces
 *  whitespace separates enum from comment
 *  Comment must be on one line and C style
 *
 *  All enums must be explicitly defined by number, do not allow auto
 *  assignment by compiler.
 *
 *  Do not put any quotes in descriptions.
 */

typedef enum
{
  ERR_OK=0x00,              /* Success. Send an acknowledgement.            */
  ERR_BAD_CMD=0x01,         /* Failure: Not a valid command from JTAG ICD   */
  ERR_DEV_MISSING=0x02,     /* Failure: No Flash device found.               */
  ERR_REQ_BLK_BAD=0x03,     /* Failure: One of the required blocks bad      */
  ERR_OUT_OF_NAND=0x04,     /* Failure: Ran past end of NAND flash          */
  ERR_SPANS_TOO_BIG=0x05,   /* Failure: Number of spans > span buffer size  */
  ERR_PAGE_TOO_BIG=0x06,    /* Failure: Size of page > page buffer          */
  ERR_PAST_SPANS_END=0x07,  /* Failure: More spans than will fit in block   */
  ERR_PAST_NAND_END=0x08,   /* Failure: Image+ bad blocks > flash avail     */
  ERR_ERASE_FAIL=0x09,      /* Failure: Erase of good block failed          */
  ERR_WRITE_FAIL=0x0A,      /* Failure: Write of good page failed           */
  ERR_READ_FAIL=0x0B,       /* Failure: Read page failed                    */
  ERR_VERIFY_FAIL=0x0C,     /* Failure: Verify after readback failed        */
  ERR_COMPARE_FAIL=0x0D,    /* Failure: Compare failed in self test verify  */
  ERR_CRC_FAIL=0x0E,        /* Failure: CRC failed during flash verify      */
  ERR_BIBCRC_FAIL=0x0F,     /* Failure: CRC passed to selftest did not match*/
  ERR_MAGIC1_FAIL=0x10,     /* Failure: Magic1 not correct in boot_info     */
  ERR_MAGIC2_FAIL=0x11,     /* Failure: Magic2 not correct in boot_info     */
  ERR_BIBVER_FAIL=0x12,     /* Failure: VIB version not correct in boot_info*/
  ERR_OVER_CONSUME=0x13,    /* Failure: Consumed too much of input buffer   */
  ERR_PAGE_UNIT_CALC=0x14,  /* Failure: Off by one error in whole page calc */
  ERR_BIB_TOO_BIG=0x15,     /* Failure: BIB + spans too long for one block  */
  ERR_BIB_BAD=0x16,         /* Failure: BIB block was bad                   */
  ERR_SPAN_DIVIDE=0x17,     /* Failure: Partial span remainder              */
  ERR_SPAN_NOTALIGN=0x18,   /* Failure: Span not aligned on a page boundary */
  ERR_ADDR_REVERSE=0x19,    /* Failure: Current code address < previous     */
  ERR_PPI_TOO_BIG=0x1A,     /* Failure: Partial page index > page_size      */
  ERR_ECC_READ=0x1B,        /* Failure: Uncorrectable ECC error             */
  ERR_ECC=0x1C,             /* Success: Correctable ECC error               */
  ERR_ECC_SELF=0x1D,        /* Failure: Error in the ECC itself             */
  ERR_BAD_BLOCK=0x1E,       /* Failure: Block identified as bad             */
  ERR_BAD_BOOT=0x1F,        /* Failure: Boot Block identified as bad        */
  ERR_GOOD_BOOT=0x20,       /* Success: Boot Block identified as good       */
  ERR_PAST_BLOCK_END=0x21,  /* Failure: Single block image too big for block*/
  ERR_NO_MIBIB_BLOCK=0x22,  /* Failure: MIBIB block not found               */
  ERR_PARTI_NOMATCH=0x23,   /* Failure: Partition tbl mismatch, no override */
  ERR_PARTI_NOT_RCVD=0x24,  /* Failure: No partition table received         */
  ERR_PARTI_NO_QCSBL=0x25,  /* Failure: No QCSBL partition in table         */
  ERR_PARTI_NO_OEMSBL=0x26, /* Failure: No OEMSBL partition in table        */
  ERR_PARTI_NO_AMSS=0x27,   /* Failure: No AMSS partition in table          */
  ERR_PARTI_INVALID=0x28,   /* Failure: Table format not recognized         */
  ERR_SECURE_PBL=0x29,      /* Failure: PBL disallowed in Secure Boot mode  */
  ERR_SECURE_BAD_MODE=0x2A, /* Failure: Unknown Security Mode               */
  ERR_SECURE_MODE_CHG=0x2B, /* Failure: Cannot change in single invocation  */
  ERR_SEC_BAD_ORDER=0x2C,   /* Failure: Security mode must be first         */
  ERR_ALLOC_EXCEEDED=0x2D,  /* Failure: Image size exceeds allocation for it*/
  ERR_PARTI_NO_EFS2=0x2E,   /* Failure: No EFS2 partition in table          */
  ERR_PARTI_NO_FOTA=0x2F,   /* Failure: No FOTA partition in table          */
  ERR_MIBIB_DAMAGED=0x30,   /* Failure: Damaged MIBIB found                 */
  ERR_NOT_ERASED=0x31,      /* Failure: Cannot write page not erased        */
  ERR_NO_BBT_BLOCK=0x32,    /* Failure: Cannot find bad block table         */
  ERR_BBT_MAGIC1_FAIL=0x33, /* Failure: incorrect Magic1 in bad block table */
  ERR_BBT_MAGIC2_FAIL=0x34, /* Failure: incorrect Magic2 in bad block table */
  ERR_USR_PARTI_NOMATCH=0x35,/* Failure: Usr Prtn. tbl miamatch, no override */
  ERR_PARTI_CNV_FAIL=0x36,  /* Failure: Partition table conversion failed   */
  ERR_MIBIB_VALID_FAIL=0x37,/* Failure: MIBIB block validation failed       */
  ERR_SYS_PARTI_RCVD=0x38,  /* Failure: Sys parti table sent in place of Usr*/
  ERR_PARTI_NO_APPSBL=0x39, /* Failure: No APPSBL partition in table        */
  ERR_PARTI_NO_APPS=0x3A ,  /* Failure: No APPS partition in table          */
  ERR_PARTI_NO_EFS2_APPS=0x3B, /* Failure: No APPS EFS2 partition in table  */
  ERR_FLSH_BIN_NO_BUFF=0x3C, /* Failure: Inadequate gap for bad-block       */
  ERR_FLSH_BIN_INV_ADR=0x3D, /* Failure: Addr mismatch in flash.bin         */
  ERR_FLSH_BIN_INV_LEN=0x3E, /* Failure: Record Length Mismatch             */
  ERR_PG_SZ_NOT_SUPP=0x3F,   /* Failure: Page size is not supported         */
  ERR_UNKNWN_PARTI=0x40,     /* Failure: No such partition on flash         */
  ERR_PARTI_NO_DSP1=0x41,    /* Failure: No DSP1 partition in table         */
  ERR_PARTI_NO_DSP2=0x42,    /* Failure: No DSP2 partition in table         */
  ERR_ECC_STATE=0x43,        /* Failure: SET_ECC_STATE failed               */
  ERR_IMG_VER_FAIL= 0x44,    /* Failure: Factory Image version is not correct */
  ERR_IMG_CNF_NOMATCH=0x45,  /* Failure: Factory Image flash config mismatch  */
  ERR_IMG_PARTI_TOO_BIG=0x46,/* Failure: Factory Image Partition tbl too big  */
  ERR_IMG_FMT_NOT_SUPPORTED=0x47, /* Failure: Factory Image format unsupported */
  ERR_EXTENDED_PARTI_INVALID=0x48, /* Failure: Invalid extended partition      */
  ERR_PARTI_TOO_BIG=0x49,    /* Failure: Partition tbl too big  */
  ERR_UNKNOWN_IMG=0x4A        /* Failure: Unknown Image                   */

} fatal_code_type;

#define SET_ERR(x) prog_param.reserved=((uint32)(x))

#endif  /* __FLASH_PROG_ERROR_H__ */

