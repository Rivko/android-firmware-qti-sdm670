/*===========================================================================

JtagProgrammer.c

 This is the Jtag programmer tool implementation.

 Copyright (c) 2014-2017  Qualcomm Technologies, Inc.
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
2015-03-31   whc     SPI NOR support
2015-02-01   bh      Changed to redirect_offset for variables shared with .cmm
2015-01-01   bh      Include alltypes.h
2014-12-09   whc     Reorganized to merge UFS
2014-11-06   bn      Initial version, ported from jtagprogrammer.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/ 
#include <bits/alltypes.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "JtagProgrammerParam.h"
#include "JtagProgrammer.h"
#include "JtagProgrammerBsp.h"
#include <Library/BaseMemoryLib.h>

/*===========================================================================

                           DEFINITIONS

===========================================================================*/
extern UINT8* Image$$JTAG_PROGRAMMER_ZI$$Start;
extern UINT8* Image$$JTAG_PROGRAMMER_ZI$$End;

/* Output buffer */
byte output_buffer[JSDCC_OUTPUT_BUFFER_SIZE];
UINTN output_buffer_length;

UINT32 redirect_offset __attribute__((used)) = 0;
UINT32 redirect_partition __attribute__((used)) = 0;
UINT32 card_slot_in_use __attribute__((used)) = 0;
UINT32 sector_writing __attribute__((used)) = 0;
UINT32 BLOCK_SIZE __attribute__((used)) = 4096;

/* enum to indicate status of probe */
probe_code_type probe_status = MEM_DEVICE_NOT_PROBED;
mem_device_type device_type = MEM_DEVICE_NONE;
struct struct_jtag_prog_param jtag_prog_param;

/*===========================================================================

                      FUNCTION PROTOTYPES

===========================================================================*/

UINT32 do_probe(void);
UINT32 (*jsdcc_mem_read)(void);
UINT32 (*jsdcc_mem_write)(void);
UINT32 (*jsdcc_mem_erase)(void);
UINT32 (*jsdcc_mem_compute_crc)(void);
UINT32 (*jsdcc_mem_print_info)(void);
UINT32 (*jsdcc_mem_program)(void);
UINT32 (*jsdcc_mem_chip_erase)(void);
UINT32 (*jsdcc_mem_set_hw_partition)(void);
UINT32 (*jsdcc_mem_set_active_boot)(void);
UINT32 (*jsdcc_extras_program)(void);
UINT32 (*jsdcc_gpp_program)(void);
UINT32 (*jsdcc_mem_erase_partition)();
UINT32 (*jsdcc_mem_firmware_update)(void);

void outputMessage(const char* format, ...) {
    int retval;
    byte *buffer = &output_buffer[output_buffer_length];
    va_list args;
    va_start (args, format);

    UINTN buffer_space = sizeof(output_buffer) - output_buffer_length;

    retval = vsnprintf((char *)buffer,
                       buffer_space,
                       format,
							  args);

    output_buffer_length += MIN(buffer_space, retval);
}

UINT32 redirect_firmware_update = 0;

/*===========================================================================

DESCRIPTION
  This function calls the appropriate routine based on what TRACE32 told us.

DEPENDENCIES
   Assumes jtag_flash_param (global variable) has valid parameters.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void main_c(void)
{
   UINT32 ret_code = STATUS_FAIL;
   output_buffer_length = 0;

   /* Call probe function.  It will only call actual probe function once */
   /* Skip probe function for RX test because it does not require the card
    * to be initialized */
   if (probe_status == MEM_DEVICE_NOT_PROBED)
   {
      ret_code = do_probe();
      if (ret_code != STATUS_SUCCESS)
      {
        outputMessage("ERROR: device not found \n");
        jtag_prog_param.cmd_status = ret_code;
		return;
      }
   }

	if (redirect_firmware_update == 0x43211234) 
	{
		jtag_prog_param.cmd_status = JTAG_CMD_FIRMWARE_UPDATE;
	}

   switch ( jtag_prog_param.cmd_status )
   {
      case JTAG_CMD_PROGRAM:
         outputMessage("Executing PROGRAM\n");
         ret_code = jsdcc_mem_program();
         break;
         
      case JTAG_CMD_WRITE_SECTOR:
         outputMessage("Executing WRITE_SECTOR\n");
         ret_code = jsdcc_mem_write();
         break;

      case JTAG_CMD_READ_SECTOR:
         outputMessage("Executing READ_SECTOR\n");
         ret_code = jsdcc_mem_read();
         break;
         
      case JTAG_CMD_ERASE_GENERIC:
         outputMessage("Executing ERASE_SECTOR\n");
         ret_code = jsdcc_mem_erase();
         break;

      case JTAG_CMD_COMPUTE_CRC:
         outputMessage("Executing CRC Compute\n");
         ret_code = jsdcc_mem_compute_crc();
         break;

      case JTAG_CMD_PRINT_INFO:
         outputMessage("Executing PRINT_INFO \n");
         ret_code = jsdcc_mem_print_info();
         break;

      case JTAG_CMD_CHIP_ERASE:
         outputMessage("Executing CHIP_ERASE\n");
         ret_code = jsdcc_mem_chip_erase();
         break;

      case JTAG_CMD_SET_HW_PARTITION:
         outputMessage("Executing SET_HW_PARTITION\n");
         ret_code = jsdcc_mem_set_hw_partition();
         break;

      case JTAG_CMD_SET_ACTIVE_BOOT:
         outputMessage("Executing SET_ACTIVE_BOOT\n");
         ret_code = jsdcc_mem_set_active_boot();
         break;

	  case JTAG_CMD_ERASE_PARTITION:
		 outputMessage("Executing ERASE_PARTITION\n");
		 ret_code = jsdcc_mem_erase_partition();
		 break;

      case JTAG_CMD_PROG_GPP:
         outputMessage("Executing PROGRAM GPP\n");
         ret_code = jsdcc_gpp_program();
         break;

      case JTAG_CMD_PROG_EXTRAS:
         outputMessage("Executing EXTRA PROGRAM\n");
         ret_code = jsdcc_extras_program();
         break;

      case JTAG_CMD_FIRMWARE_UPDATE:
         outputMessage("Executing FIRMWARE UPDATE\n");
         ret_code = jsdcc_mem_firmware_update();
         break;

      default:
         ret_code = STATUS_FAIL;
         outputMessage ("ERROR: Invalid command 0x%x\n", jtag_prog_param.cmd_status);
         break;
   }
   /* return error code back to TRACE32, 0 = success, 0x100 = fail */
   jtag_prog_param.cmd_status = ret_code;

} /* main_c */

typedef struct {
  UINT8  *base;
  UINT8  *len;
} scatter_zi_t;

scatter_zi_t zi_data[] =
{
   {
     (UINT8*)&Image$$JTAG_PROGRAMMER_ZI$$Start,   
     (UINT8*)&Image$$JTAG_PROGRAMMER_ZI$$End
   },
   {NULL, NULL}
};

void jsdcc_init(void)
{
   UINT32 length = (zi_data[0].len - zi_data[0].base);

   /* clear out ZI data */
   SetMem(zi_data[0].base, length, 0x0);

   while (1)
   {
      main_c();
   }
}

/*===========================================================================

DESCRIPTION
  This function is a wrapper for the real flash probe routine. 
  It does some checking to ensure not calling multiple times and 
  calls the real probe function.

DEPENDENCIES
  none

RETURN VALUE
  If success, return FLASH_SUCCESS else FLASH_FAIL

SIDE EFFECTS

===========================================================================*/
UINT32 do_probe(void)
{
   UINT32 ret_code = STATUS_FAIL;
   //UINT32 card_slot = 0;
     /*if device is manually selected*/
  if (device_type == MEM_DEVICE_UFS) 
  {
    ret_code = do_probe_ufs();
  }
  else if (device_type == MEM_DEVICE_EMMC) 
  {
	ret_code = do_probe_emmc();
  }
  else if (device_type == MEM_DEVICE_SPINOR)
  {
    ret_code = do_probe_spi();
  }
  else /*do probing for a device and select the whichever first founded*/
  {
    //do nothing for now.  TBD auto detect
	/*ret_code = do_probe_ufs();
	if (ret_code == STATUS_SUCCESS)
	{
		device_type = MEM_DEVICE_UFS;
	}
	else
	{
	  ret_code = do_probe_emmc();
	  if (ret_code == STATUS_SUCCESS)
	  {
		device_type = MEM_DEVICE_EMMC;
	  }
	}*/
    ret_code = STATUS_FAIL;
  }

  if (ret_code != STATUS_SUCCESS)
  {
    return ret_code;
  }

  if (device_type == MEM_DEVICE_UFS)
  {
    jsdcc_mem_read = jsdcc_mem_read_ufs;
    jsdcc_mem_write = jsdcc_mem_write_ufs;
    jsdcc_mem_erase = jsdcc_mem_erase_ufs;
    jsdcc_mem_compute_crc = jsdcc_mem_compute_crc_ufs;
    jsdcc_mem_print_info = jsdcc_mem_print_info_ufs;
    jsdcc_mem_program = jsdcc_mem_program_ufs;
    jsdcc_mem_chip_erase = jsdcc_mem_chip_erase_ufs;
    jsdcc_mem_set_hw_partition = jsdcc_mem_set_hw_partition_ufs;
    jsdcc_mem_set_active_boot = jsdcc_mem_set_active_boot_ufs;
    jsdcc_extras_program = jsdcc_extras_program_ufs;
    jsdcc_mem_firmware_update = jsdcc_mem_firmware_update_ufs;
  }
  else if (device_type == MEM_DEVICE_EMMC)
  {
    jsdcc_mem_read = jsdcc_mem_read_emmc;
    jsdcc_mem_write = jsdcc_mem_write_emmc;
    jsdcc_mem_erase = jsdcc_mem_erase_emmc;
    jsdcc_mem_compute_crc = jsdcc_mem_compute_crc_emmc;
    jsdcc_mem_print_info = jsdcc_mem_print_info_emmc;
    jsdcc_mem_program = jsdcc_mem_program_emmc;
    jsdcc_mem_chip_erase = jsdcc_mem_chip_erase_emmc;
    jsdcc_mem_set_hw_partition = jsdcc_mem_set_hw_partition_emmc;
    jsdcc_mem_set_active_boot = jsdcc_mem_set_active_boot_emmc;
    jsdcc_extras_program = NULL;
    jsdcc_gpp_program = jsdcc_gpp_program_emmc;
    jsdcc_mem_firmware_update = NULL;
  }
  else if (device_type == MEM_DEVICE_SPINOR)
  {
	jsdcc_mem_read = jsdcc_mem_read_spi;
	jsdcc_mem_write = jsdcc_mem_write_spi;
	jsdcc_mem_erase = jsdcc_mem_erase_spi;
	jsdcc_mem_erase_partition = jsdcc_mem_erase_partition_spi;
	jsdcc_mem_compute_crc = jsdcc_mem_compute_crc_spi;
	jsdcc_mem_print_info = jsdcc_mem_print_info_spi;
	jsdcc_mem_program = jsdcc_mem_program_spi;
	jsdcc_mem_chip_erase = jsdcc_mem_chip_erase_spi;
	jsdcc_mem_set_active_boot = jsdcc_mem_set_active_boot_spi;
  }

  return ret_code;
}  /* do_probe */


/*===========================================================================

FUNCTION  calc_crc32

DESCRIPTION
  This function calculate CRC32 on input data.

DEPENDENCIES
  None

RETURN VALUE
  Returns CRC32 of given data

SIDE EFFECTS
  None

===========================================================================*/
UINT32 calc_crc32
(
  const UINT8   *data_in,
  const UINT32  nbytes_in
)
{
   UINT32 k = 8;                   // length of unit (i.e. byte)
   int MSB = 0;
   int gx = 0x04C11DB7;         // IEEE 32bit polynomial
   int regs = 0xFFFFFFFF;       // init to all ones
   int regsMask = 0xFFFFFFFF;   // ensure only 32 bit answer
   int regsMSB = 0;
   UINT32 i, j;
   UINT8 DataByte;

   if ( (data_in == NULL) || (nbytes_in == 0) )
      return 0;

   for( i=0; i < nbytes_in; i++) {
      DataByte = data_in[i];
      DataByte = reflect(DataByte,8);
      for(j=0; j < k; j++) {
         MSB = DataByte >> (k-1);  // get MSB
         MSB &= 1;                 // ensure just 1 bit
         regsMSB = (regs>>31) & 1; // MSB of regs
         regs = regs<<1;           // shift regs for CRC-CCITT
         if(regsMSB ^ MSB) {       // MSB is a 1
            regs = regs ^ gx;       // XOR with generator poly
         }
         regs = regs & regsMask;   // Mask off excess upper bits
           DataByte <<= 1;           // get to next bit
      }
   }

   regs = regs & regsMask;       // Mask off excess upper bits
   return reflect(regs,32) ^ 0xFFFFFFFF;
}

int reflect
(
  int data,
  const UINT32 len
)
{
   int ref = 0;
   UINT32 i;

   for(i=0; i < len; i++) {
      if(data & 0x1) {
         ref |= (1 << ((len - 1) - i));
      }
      data = (data >> 1);
   }

   return ref;
}
