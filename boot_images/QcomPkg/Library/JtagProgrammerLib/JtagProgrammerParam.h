#ifndef JTAG_PROGRAMMER_PARAM_H
#define JTAG_PROGRAMMER_PARAM_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


             SDCC Jtag Programming Support via Lauterbach JTAG ICD

GENERAL DESCRIPTION
  This file exist so that we can explicitly place our jtag_prog_param
  data in memory since ARM doesn't allow this within a file but rather by
  file basis.

 Copyright (c) 2014 - 2015 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Qualcomm Confidential and Proprietary

============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



when         who     what, where, why
----------   ---     ----------------------------------------------------------
2015-09-24   rh      Adding firmware update support
2015-03-31   whc     SPI NOR support
2014-12-09   whc     added UFS command define
2014-09-15   bn      Initial version, ported from boot build jtagprogrammer.

===========================================================================*/

#define PARAM_BUFFER_SIZE  0x8000
#define JSDCC_OUTPUT_BUFFER_SIZE 0x800

/* This structure is used to communcate between TRACE32 Debugger and our FLASH plug-in. */
struct struct_jtag_prog_param {
   UINT64 mod_addr;           // in: Flash module address
   UINT64 reserved;           // in: Not used
   UINT64 width;              // in: Data width - Not used
   UINT64 offset;             // in: Not used
   UINT64 addr;               // in: address of the datacard to write/erase
   UINT64 data_length;        // in: size of data to write/erase
   UINT64 timing;             // in: Clock frequency - Not used
   UINT64 cmd_status;         // in: 1=program, 2=erase, 5=erase, out: 0=o.k., 0x100=fail
   UINT8  data[PARAM_BUFFER_SIZE];  // give 32K worth of buffer - defined in sconscript
                              // this is where TRACE32 will store data that
                              // we'll be writing to the datacard
};

/* command code from struct_jtag_prog_param.cmd */
#define JTAG_CMD_PROGRAM               0x00000001
#define JTAG_CMD_ERASE                 0x00000002
#define JTAG_CMD_CHIP_ERASE            0x00000005
#define JTAG_CMD_READ_SECTOR           0x00000009
#define JTAG_CMD_WRITE_SECTOR          0x0000000A
#define JTAG_CMD_PRINT_INFO            0x00000020
#define JTAG_CMD_SET_HW_PARTITION      0x00000031
#define JTAG_CMD_SET_ACTIVE_BOOT       0x00000035
#define JTAG_CMD_PROG_GPP              0x00000036
#define JTAG_CMD_ERASE_GENERIC         0x00000040
#define JTAG_CMD_COMPUTE_CRC           0x00000041
#define JTAG_CMD_PROG_EXTRAS           0x00000042
#define JTAG_CMD_ERASE_PARTITION       0x00000043
#define JTAG_CMD_FIRMWARE_UPDATE       0x00000044


// return code for type_jtag_flash_param.status
#define STATUS_SUCCESS                 0
#define STATUS_FAIL                    0x00000100
#define STATUS_NOT_IMPLEMENTED         0x00000100

#endif  /* JTAG_PROGRAMMER_PARAM_H */

