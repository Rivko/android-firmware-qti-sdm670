#ifndef JTAG_PROGRAMMER_H
#define JTAG_PROGRAMMER_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


             SDCC Jtag Programming Support via Lauterbach JTAG ICD

GENERAL DESCRIPTION
  This file exist so that we can explicitly place our jtag_prog_param
  data in memory since ARM doesn't allow this within a file but rather by
  file basis.

 Copyright (c) 2014 - 2017 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Qualcomm Confidential and Proprietary

============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



when         who     what, where, why
----------   ---     ----------------------------------------------------------
2017-01-26   yps     Fixed LLVM384 compiling issue
2015-09-24   rh      Adding firmware update support
2015-03-31   whc      SPI NOR support
2014-12-09   whc      Initial version, ported from boot build jtagprogrammer.

===========================================================================*/

/* Enumerated type to see if device probed OK so we only probe once */
typedef enum
{
  MEM_DEVICE_NOT_PROBED,    /* Not yet probed               */
  MEM_DEVICE_FOUND,         /* Successful probe             */
  MEM_DEVICE_UNKNOWN        /* Failed to probe successfully */
}
probe_code_type;

typedef enum
{
  MEM_DEVICE_UFS = 0,
  MEM_DEVICE_EMMC,
  MEM_DEVICE_SPINOR,
  MEM_DEVICE_NONE
}
mem_device_type;

#define SIZE_IN_BLOCKS(x)         ( ((x % BLOCK_SIZE) == 0) ? x/BLOCK_SIZE : x/BLOCK_SIZE+1 )

/* This structure is used to communicate between ourselves and TRACE32. */
extern struct struct_jtag_prog_param jtag_prog_param;

extern UINT32 redirect_offset;
extern UINT32 redirect_partition;
extern UINT32 card_slot_in_use;
extern UINT32 sector_writing;

extern probe_code_type probe_status;
extern mem_device_type device_type;
extern UINT32 BLOCK_SIZE;


/* Calculate a CRC value for the given data */
extern int reflect(int data, const UINT32 len);
extern UINT32 calc_crc32(const UINT8 *data_in, const UINT32 nbytes_in);
extern void outputMessage(const char* format, ...);

extern UINT32 do_probe_emmc(void);
extern UINT32 jsdcc_mem_read_emmc(void);
extern UINT32 jsdcc_mem_write_emmc(void);
extern UINT32 jsdcc_mem_erase_emmc(void);
extern UINT32 jsdcc_mem_compute_crc_emmc(void);
extern UINT32 jsdcc_mem_print_info_emmc(void);
extern UINT32 jsdcc_mem_program_emmc(void);
extern UINT32 jsdcc_mem_chip_erase_emmc(void);
extern UINT32 jsdcc_mem_set_hw_partition_emmc(void);
extern UINT32 jsdcc_mem_select_card_slot_emmc(void);
extern UINT32 jsdcc_mem_set_active_boot_emmc(void);
extern UINT32 jsdcc_gpp_program_emmc(void);

extern UINT32 do_probe_ufs(void);
extern UINT32 jsdcc_mem_read_ufs(void);
extern UINT32 jsdcc_mem_write_ufs(void);
extern UINT32 jsdcc_mem_erase_ufs(void);
extern UINT32 jsdcc_mem_compute_crc_ufs(void);
extern UINT32 jsdcc_mem_print_info_ufs(void);
extern UINT32 jsdcc_mem_program_ufs(void);
extern UINT32 jsdcc_mem_chip_erase_ufs(void);
extern UINT32 jsdcc_mem_set_hw_partition_ufs(void);
extern UINT32 jsdcc_mem_set_active_boot_ufs(void);
extern UINT32 jsdcc_extras_program_ufs(void);
extern UINT32 jsdcc_mem_firmware_update_ufs(void);


extern UINT32 do_probe_spi(void);
extern UINT32 jsdcc_mem_read_spi(void);
extern UINT32 jsdcc_mem_write_spi(void);
extern UINT32 jsdcc_mem_erase_spi(void);
extern UINT32 jsdcc_mem_compute_crc_spi(void);
extern UINT32 jsdcc_mem_print_info_spi(void);
extern UINT32 jsdcc_mem_program_spi(void);
extern UINT32 jsdcc_mem_chip_erase_spi(void);
extern UINT32 jsdcc_mem_erase_partition_spi(void);
extern UINT32 jsdcc_mem_set_active_boot_spi(void);

#endif
