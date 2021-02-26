/*===========================================================================

JtagProgrammer.c

 This is the Jtag programmer tool implementation.

 Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Qualcomm Confidential and Proprietary

============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when         who     what, where, why
----------   ---     ----------------------------------------------------------
2015-02-18   bn      Workaround for 8996
2014-12-09   whc     Reorganized to merge
2014-11-06   bn      Initial version, ported from jtagprogrammer.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/ 
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "JtagProgrammer.h"
#include "JtagProgrammerParam.h"
#include "sdcc_api.h"
#include "JtagProgrammerBsp.h"
#include <Library/BaseMemoryLib.h>

/*===========================================================================

                           DEFINITIONS

===========================================================================*/



/* These variables are used to redirect the programming routine to partition 
 * aware programming routine */

static sdcc_mem_info_type current_mem_info_emmc;

/* SDCC device handle */
#define MAX_SDCC_PARTITIONS 7
struct sdcc_device *hsdev_user;           /* User partition handle */
struct sdcc_device *hsdev_handles[MAX_SDCC_PARTITIONS+1];
struct sdcc_device *hsdev_current;        /* Current operating partition handle */

char *sdcc_err_string[] = {
    "SDCC_NO_ERROR",
    "SDCC_ERR_UNKNOWN",
    "SDCC_ERR_CMD_TIMEOUT",
    "SDCC_ERR_TIMEOUT",
    "SDCC_ERR_CMD_CRC_FAIL",
    "SDCC_ERR_DATA_CRC_FAIL",
    "SDCC_ERR_CMD_SENT",
    "SDCC_ERR_PROG_DONE",
    "SDCC_ERR_CARD_READY",
    "SDCC_ERR_INVALID_TX_STATE",
    "SDCC_ERR_SET_BLKSZ",
    "SDCC_ERR_SDIO_R5_RESP",
    "SDCC_ERR_DMA",
    "SDCC_ERR_READ_FIFO",
    "SDCC_ERR_WRITE_FIFO",
    "SDCC_ERR_ERASE",
    "SDCC_ERR_SDIO",
    "SDCC_ERR_SDIO_READ",
    "SDCC_ERR_SDIO_WRITE",
    "SDCC_ERR_SWITCH",
    "SDCC_ERR_INVALID_PARAM",
    "SDCC_ERR_CARD_UNDETECTED",
    "SDCC_ERR_FEATURE_UNSUPPORTED",
    "SDCC_ERR_SECURE_COMMAND_IN_PROGRESS",
    "SDCC_ERR_READ_SEC_CMD_NOT_ALLOWED",
    "SDCC_ERR_ABORT_READ_SEC_CMD",
    "SDCC_ERR_CARD_INIT",
    "SDCC_ERR_CARD_REMOVED",
    "SDCC_ERR_PWR_ON_WRITE_PROT",
    "SDCC_ERR_WP_VIOLATION",
    "SDCC_ERR_SPS_MODE_USED",
    "SDCC_ERR_DML_INIT",
    "SDCC_ERR_SPS_GET_EVENT",
    "SDCC_ERR_SPS_WRITING_DESCRIPTOR"
};


/*===========================================================================

                      FUNCTION PROTOTYPES

===========================================================================*/
UINT32 do_probe_emmc(void);
UINT32 jsdcc_mem_read_emmc(void);
UINT32 jsdcc_mem_write_emmc(void);
UINT32 jsdcc_mem_erase_emmc(void);
UINT32 jsdcc_mem_compute_crc_emmc(void);
UINT32 jsdcc_mem_print_info_emmc(void);
UINT32 jsdcc_mem_program_emmc(void);
UINT32 jsdcc_mem_chip_erase_emmc(void);
UINT32 jsdcc_mem_set_hw_partition_emmc(void);
UINT32 jsdcc_mem_select_card_slot_emmc(void);
UINT32 jsdcc_mem_set_active_boot_emmc(void);
UINT32 jsdcc_gpp_program_emmc(void);


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
UINT32 do_probe_emmc(void)
{
   UINT32 ret_code = STATUS_FAIL;
   //UINT32 card_slot = 0;
   unsigned int i;

   BLOCK_SIZE = 512;

   outputMessage("\n\nSDCC do_probe(card_slot_in_use=%d)\n",card_slot_in_use);

   /* Kill the watchdog so it won't bark */
   jsdcc_bsp_kill_watchdog();
   
   /* Only probe device if it has not yet been probed. */
   switch (probe_status)
   {
      case MEM_DEVICE_NOT_PROBED:
         /* Configure the clock if is required */
         jsdcc_bsp_tool_config_pll();   
         /* Enable the PMIC SSBI bus clock */
         jsdcc_bsp_tool_enable_pmic_ssbi_clk();
         /* Before issue open command, make sure SSBI clock is on */

        /*
        while(card_slot<=3)							// let's show what we find neatly here
        {
            outputMessage("Slot %d ---> ",card_slot);
            hsdev_user  = sdcc_handle_open(card_slot, 0);

            if(hsdev_user != NULL)
                outputMessage("responded (SD/MMC device exists).\n");
            else
                outputMessage("not present\n");

            card_slot++;
        }
        */

        outputMessage("\nUsing 'user specified' card slot %d\n",card_slot_in_use);

         for (i = 0; i <= MAX_SDCC_PARTITIONS; i++)
         {
            if (i==3) /* Skip opening RPMB partition */
                hsdev_handles[i] = NULL;
            else
                hsdev_handles[i] = sdcc_handle_open(card_slot_in_use, i);
         }
         hsdev_user = hsdev_handles[0];
         if(hsdev_user != NULL)
         {
            ret_code = STATUS_SUCCESS;
            probe_status = MEM_DEVICE_FOUND;
         }
         else
         {
            ret_code = STATUS_FAIL;
            probe_status = MEM_DEVICE_UNKNOWN;
            outputMessage("Warning: Unable to open USER partition.\n");	// can't find boot partitions OR this (people ask about this)
            outputMessage("Possible causes: card not present? No power or clock to the card?\n");
         }

         hsdev_current = hsdev_user;
         break;
      case MEM_DEVICE_UNKNOWN:
		 outputMessage("\nMEM_DEVICE_UNKNOWN\n");
         ret_code = STATUS_FAIL;
         break;
      case MEM_DEVICE_FOUND:
		 outputMessage("\nMEM_DEVICE_FOUND\n");
         ret_code = STATUS_SUCCESS;
         break;
      default:
		 outputMessage("\nMEM_DEVICE_DEFAULT_STATUS_FAIL\n");
         ret_code = STATUS_FAIL;
         break;
  }

  return ret_code;
}  /* do_probe */


/*===========================================================================

DESCRIPTION
  This function is a wrapper for the real memory read routine. It extracts
  the required parameters from TRACE32 and passes it onto the real thing.

DEPENDENCIES
  Assumes jtag_prog_param (global variable) has valid parameters.

RETURN VALUE
  If success, return STATUS_SUCCESS else STATUS_FAIL

SIDE EFFECTS

===========================================================================*/
UINT32 jsdcc_mem_read_emmc(void)
{

   UINT8 *pdata;
   SDCC_STATUS rc;

   pdata = jtag_prog_param.data;
   rc = sdcc_handle_read(hsdev_current, jtag_prog_param.addr, 
                         pdata, jtag_prog_param.data_length);

   if (rc != SDCC_NO_ERROR) 
   {
     outputMessage("ERROR 0x%X: %s\n", rc, sdcc_err_string[rc]);
     return STATUS_FAIL;
   }
   return STATUS_SUCCESS;
}  /* jsdcc_mem_read */


/*===========================================================================

DESCRIPTION
  This function is a wrapper for the real memory write routine. It extracts
  the required parameters from TRACE32 and passes it onto the real thing.

DEPENDENCIES
  Assumes jtag_prog_param (global variable) has valid parameters.

RETURN VALUE
  If success, return STATUS_SUCCESS else STATUS_FAIL

SIDE EFFECTS

===========================================================================*/
UINT32 jsdcc_mem_write_emmc(void)
{

   UINT8 *pdata;
   SDCC_STATUS rc;

   //outputMessage("Sector Write begins: \n");
   //outputMessage("Addr %x, Size %x, Offset %x \n", 
   //         jtag_prog_param.addr, jtag_prog_param.data_length, 
   //         jtag_prog_param.mod_addr);
   
   pdata = jtag_prog_param.data;
   rc = sdcc_handle_write(hsdev_current, jtag_prog_param.addr, 
                         pdata, jtag_prog_param.data_length);

   if (rc != SDCC_NO_ERROR) 
   {
     outputMessage("ERROR 0x%X: %s\n", rc, sdcc_err_string[rc]);
     return STATUS_FAIL;
   }
   return STATUS_SUCCESS;

}  /* jsdcc_mem_write */


/*===========================================================================

DESCRIPTION
  This function is a wrapper for the real memory program routine. It extracts
  the required parameters from TRACE32 and passes it onto the real thing.  
  Program is very similar to write, except the length of the data is divided
  by BLOCK_SIZE.

DEPENDENCIES
  Assumes jtag_prog_param (global variable) has valid parameters.

RETURN VALUE
  If success, return STATUS_SUCCESS else STATUS_FAIL

SIDE EFFECTS

===========================================================================*/
UINT32 jsdcc_mem_program_emmc(void)
{

   UINT8 *pdata;
   SDCC_STATUS rc;

   if(sector_writing) {
      jtag_prog_param.addr = redirect_offset + SIZE_IN_BLOCKS(jtag_prog_param.addr);
      jtag_prog_param.data_length = jtag_prog_param.data_length < redirect_partition ? jtag_prog_param.data_length : redirect_partition;
      if (jtag_prog_param.data_length == 0) {
        return STATUS_SUCCESS;
      }
      redirect_partition -= jtag_prog_param.data_length;
   }

   /* Print out debugging information */
   /*
   outputMessage("Addr %x, Size %x, Offset %x \n", 
            jtag_prog_param.addr, jtag_prog_param.data_length, 
            jtag_prog_param.offset);*/

   pdata = jtag_prog_param.data;
   rc = sdcc_handle_write(hsdev_current, jtag_prog_param.addr, 
                         pdata, SIZE_IN_BLOCKS(jtag_prog_param.data_length));

   if (rc != SDCC_NO_ERROR) 
   {
     outputMessage("ERROR 0x%X: %s\n", rc, sdcc_err_string[rc]);
     return STATUS_FAIL;
   }
   return STATUS_SUCCESS;
}  /* jsdcc_mem_program */


/*===========================================================================

DESCRIPTION
  This function calls the mem_get_device_info API to get the information of 
  the memory device attached.

DEPENDENCIES
  Assumes jtag_prog_param (global variable) has valid parameters.

RETURN VALUE
  If success, return STATUS_SUCCESS else STATUS_FAIL

SIDE EFFECTS

===========================================================================*/
UINT32 jsdcc_mem_print_info_emmc(void)
{
   SDCC_STATUS rc;

   if (do_probe_emmc() != STATUS_SUCCESS)
   {
     outputMessage("ERROR: MMC device not found \n");
     return STATUS_FAIL;
   }


   outputMessage("\n==============================\n");
   outputMessage("=====       INFO         =====\n");
   outputMessage("==============================\n");

   if(hsdev_current == NULL)
   {
	 outputMessage("\nERROR: No device detected on that slot\n");
     return STATUS_FAIL;
   }

   rc = sdcc_handle_mem_get_device_info(hsdev_current, &current_mem_info_emmc);

   if (rc != SDCC_NO_ERROR) 
   {
     outputMessage("SDCC GET INFO ERROR 0x%X: %s\n", rc, sdcc_err_string[rc]);
     return STATUS_FAIL;
   }

   /* Format and print out the memory information */
   switch(current_mem_info_emmc.card_type)
   {
      case SDCC_CARD_UNKNOWN:
         outputMessage("MEMORY TYPE: UNKNOWN\n");
         break;
      case SDCC_CARD_SD:      
         outputMessage("MEMORY TYPE: SD\n");
         break;
      case SDCC_CARD_MMC:     
         outputMessage("MEMORY TYPE: MMC\n");
         break;
      case SDCC_CARD_COMB:    
         outputMessage("MEMORY TYPE: COMB\n");
         break;
      case SDCC_CARD_SDIO:   
         outputMessage("MEMORY TYPE: SDIO\n");
         break;
      case SDCC_CARD_SDHC:    
         outputMessage("MEMORY TYPE: SDHC\n");
         break;
      case SDCC_CARD_MMCHC:   
         outputMessage("MEMORY TYPE: MMCHC\n");
         break;
   }

   outputMessage("Size (sectors): %d\n", current_mem_info_emmc.card_size_in_sectors);
   outputMessage("Block Length (bytes): %d\n", current_mem_info_emmc.block_len);
   outputMessage("Size (kB): %d\n", current_mem_info_emmc.card_size_in_sectors / 1024 * current_mem_info_emmc.block_len);
   if(current_mem_info_emmc.card_type == SDCC_CARD_SDHC)
   {
      outputMessage("Speed Class: %d\n", 
               current_mem_info_emmc.speed_class.sd_speed_class == SDCC_SD_SPEED_CLASS_0 ? 0 :
               current_mem_info_emmc.speed_class.sd_speed_class == SDCC_SD_SPEED_CLASS_2 ? 2 :
               current_mem_info_emmc.speed_class.sd_speed_class == SDCC_SD_SPEED_CLASS_4 ? 4 :
               current_mem_info_emmc.speed_class.sd_speed_class == SDCC_SD_SPEED_CLASS_6 ? 6 :
               0
              );
   }
   else if(current_mem_info_emmc.card_type == SDCC_CARD_MMCHC)
   {
      outputMessage("Speed Class: %c\n", 
               current_mem_info_emmc.speed_class.mmc_speed_class == SDCC_MMC_SPEED_CLASS_LOWEST ? '0' :
               current_mem_info_emmc.speed_class.mmc_speed_class == SDCC_MMC_SPEED_CLASS_A ? 'A' :
               current_mem_info_emmc.speed_class.mmc_speed_class == SDCC_MMC_SPEED_CLASS_B ? 'B' :
               current_mem_info_emmc.speed_class.mmc_speed_class == SDCC_MMC_SPEED_CLASS_C ? 'C' :
               current_mem_info_emmc.speed_class.mmc_speed_class == SDCC_MMC_SPEED_CLASS_D ? 'D' :
               current_mem_info_emmc.speed_class.mmc_speed_class == SDCC_MMC_SPEED_CLASS_E ? 'E' :
               current_mem_info_emmc.speed_class.mmc_speed_class == SDCC_MMC_SPEED_CLASS_F ? 'F' :
               current_mem_info_emmc.speed_class.mmc_speed_class == SDCC_MMC_SPEED_CLASS_G ? 'G' :
               current_mem_info_emmc.speed_class.mmc_speed_class == SDCC_MMC_SPEED_CLASS_H ? 'H' :
               current_mem_info_emmc.speed_class.mmc_speed_class == SDCC_MMC_SPEED_CLASS_J ? 'J' :
               current_mem_info_emmc.speed_class.mmc_speed_class == SDCC_MMC_SPEED_CLASS_K ? 'K' :
               current_mem_info_emmc.speed_class.mmc_speed_class == SDCC_MMC_SPEED_CLASS_M ? 'M' :
               current_mem_info_emmc.speed_class.mmc_speed_class == SDCC_MMC_SPEED_CLASS_O ? 'O' :
               current_mem_info_emmc.speed_class.mmc_speed_class == SDCC_MMC_SPEED_CLASS_R ? 'R' :
               current_mem_info_emmc.speed_class.mmc_speed_class == SDCC_MMC_SPEED_CLASS_T ? 'T' :
               'x'
              );
   }

   outputMessage("Reliable write sector count: %d\n", current_mem_info_emmc.reliable_write_sector_count);
   outputMessage("Physical partition count: %d\n", current_mem_info_emmc.num_phy_partition_created);
   outputMessage("Manufacture ID: %x\n", current_mem_info_emmc.mfr_id);
   //outputMessage("OEM/Application ID: %x\n", current_mem_info_emmc.oem_id);
   outputMessage("Product Name: %c%c%c%c%c%c\n", current_mem_info_emmc.prod_name[0], 
                                            current_mem_info_emmc.prod_name[1],
                                            current_mem_info_emmc.prod_name[2],
                                            current_mem_info_emmc.prod_name[3],
                                            current_mem_info_emmc.prod_name[4],
                                            current_mem_info_emmc.prod_name[5]
                                            );
   outputMessage("Product revision: %x\n", current_mem_info_emmc.prod_rev);
   outputMessage("Product serial #: %x\n", current_mem_info_emmc.prod_serial_num);
   current_mem_info_emmc.mfr_date[7] = 0;

/* NOTE: Remove to workaround 8996 eMMC Jtag programmer.*/
/* Need to Fix it properly */
//   outputMessage("Manufacturing Date: %s\n", current_mem_info_emmc.mfr_date);

   jtag_prog_param.data_length = current_mem_info_emmc.card_size_in_sectors;
   BLOCK_SIZE = 512;
   return STATUS_SUCCESS;
}  /* jsdcc_mem_print_info */

/*===========================================================================

DESCRIPTION
  This function erases a specified number of sectors from the card
  starting from the specified start sector

DEPENDENCIES
  Assumes jtag_prog_param (global variable) has valid parameters.

RETURN VALUE
  If success, return STATUS_SUCCESS else STATUS_FAIL

SIDE EFFECTS

===========================================================================*/
UINT32 jsdcc_mem_erase_emmc(void)
{
   SDCC_STATUS rc;
   UINT8 *pdata;
   UINT32 erase_data;
   
   while(jtag_prog_param.data_length > 0)
   {
       erase_data = jtag_prog_param.data_length < (PARAM_BUFFER_SIZE/BLOCK_SIZE) ? jtag_prog_param.data_length : (PARAM_BUFFER_SIZE/BLOCK_SIZE);

       /*rc = sdcc_handle_erase(hsdev_user, BLOCK_SIZE*jtag_prog_param.addr, jtag_prog_param.data_length);*/
       if (hsdev_current != NULL) {
         pdata = jtag_prog_param.data;
         SetMem(pdata, BLOCK_SIZE*erase_data, 0x00);
         rc = sdcc_handle_write(hsdev_current, jtag_prog_param.addr, pdata, erase_data);
       }
       else {
         outputMessage("No current handle to erase");
         return STATUS_FAIL;
       }
       if (rc != SDCC_NO_ERROR) 
       {
         outputMessage("ERROR 0x%X: %s\n", rc, sdcc_err_string[rc]);
         return STATUS_FAIL;
       }
       jtag_prog_param.addr += erase_data;
       jtag_prog_param.data_length -= erase_data;
   }
   outputMessage("ERASE: Success! \n");
   return STATUS_SUCCESS;
}

/*===========================================================================

DESCRIPTION
  This function computes the CRC32 over a buffer

DEPENDENCIES
  Assumes jtag_prog_param (global variable) has valid parameters.

RETURN VALUE
  If success, return STATUS_SUCCESS else STATUS_FAIL

SIDE EFFECTS

===========================================================================*/
UINT32 jsdcc_mem_compute_crc_emmc(void)
{ 
   UINT8 *pdata;
   SDCC_STATUS rc;

   UINT32 sectors_to_read;
   sectors_to_read = jtag_prog_param.data_length / BLOCK_SIZE;
   if (jtag_prog_param.data_length % BLOCK_SIZE != 0)
        sectors_to_read++;

   pdata = jtag_prog_param.data;
   rc = sdcc_handle_read(hsdev_current, jtag_prog_param.addr, 
                        pdata, sectors_to_read);

   if (rc != SDCC_NO_ERROR) 
   {
     outputMessage("READ ERROR 0x%X: %s\n", rc, sdcc_err_string[rc]);
     return STATUS_FAIL;
   }

   jtag_prog_param.data_length = calc_crc32((UINT8 *)pdata, jtag_prog_param.data_length);

   outputMessage("CRC Compute: Success! \n");
   return STATUS_SUCCESS;
}

/*===========================================================================

DESCRIPTION
  This function is a wrapper for the real memory erase routine.  
  The step to erase the entire memory device is to check the size
  of each physical partition and erase the partition by issue the 
  block erase command.

RETURN VALUE
  If success, return STATUS_SUCCESS else STATUS_FAIL

SIDE EFFECTS

===========================================================================*/
UINT32 jsdcc_mem_chip_erase_emmc(void)
{
   SDCC_STATUS rc;
   sdcc_mem_info_type mem_info;

   rc = sdcc_handle_mem_get_device_info(hsdev_current, &mem_info);
   if (rc != SDCC_NO_ERROR) 
   {
     outputMessage("GET INFO ERROR 0x%X: %s\n", rc, sdcc_err_string[rc]);
     return STATUS_FAIL;
   }

   //outputMessage("Issuing the eMMC ERASE command which erases the USER partition.\n");
   rc = sdcc_handle_erase(hsdev_current, 0, mem_info.card_size_in_sectors - 1);
   if (rc != SDCC_NO_ERROR) 
   {
      outputMessage("CHIP_ERASE ERROR 0x%X: %s\n", rc, sdcc_err_string[rc]);
      return STATUS_FAIL;
   }

   outputMessage("Success! \n");
   return STATUS_SUCCESS;
}  /* jsdcc_mem_chip_erase */


/*===========================================================================

DESCRIPTION
  This function is used to switch which HW partition that the current 
  operations are operating on.

DEPENDENCIES
  Assumes jtag_prog_param (global variable) has valid parameters.

RETURN VALUE
  If success, return STATUS_SUCCESS else STATUS_FAIL

SIDE EFFECTS

===========================================================================*/
UINT32 jsdcc_mem_set_hw_partition_emmc(void)
{
   /* Not closing the old handle on purpose - Ensure the clock not getting turned off*/
   if (jtag_prog_param.addr > MAX_SDCC_PARTITIONS || jtag_prog_param.addr == 3) {
        outputMessage("Failed to switch partition.\n");
        if (jtag_prog_param.addr == 3)
            outputMessage("ERROR: We do not support switching to RPMB partition. Please consult SDCC driver team.\n");
        else if (jtag_prog_param.addr > MAX_SDCC_PARTITIONS)
            outputMessage("ERROR: You requested to switch to partition %d that doesn't exist.\nOnly %d partitions exist.\n", jtag_prog_param.addr, MAX_SDCC_PARTITIONS);
        probe_status = MEM_DEVICE_UNKNOWN;
        return STATUS_FAIL;
   }
   else
   {
        hsdev_current = hsdev_handles[jtag_prog_param.addr];
   }

   probe_status = MEM_DEVICE_FOUND;
   return STATUS_SUCCESS;
}

/*===========================================================================

DESCRIPTION
  This function call the SDCC API to set the active boot partition

DEPENDENCIES

RETURN VALUE
  If success, return the starting sector of the givin partition, if
  failed, return 0

SIDE EFFECTS

===========================================================================*/
UINT32 jsdcc_mem_set_active_boot_emmc(void)
{
   SDCC_STATUS rc;

   /* Not closing the old handle on purpose - Ensure the clock not getting turned off*/
   if (jtag_prog_param.addr > MAX_SDCC_PARTITIONS || jtag_prog_param.addr == 3) {
        outputMessage("Failed to switch partition.\n");
        if (jtag_prog_param.addr == 3)
            outputMessage("ERROR: We do not support switching to RPMB partition. Please consult SDCC driver team.\n");
        else if (jtag_prog_param.addr > MAX_SDCC_PARTITIONS)
            outputMessage("ERROR: You requested to switch to partition %d that doesn't exist.\nOnly %d partitions exist.\n", jtag_prog_param.addr, MAX_SDCC_PARTITIONS);

        probe_status = MEM_DEVICE_UNKNOWN;
        return STATUS_FAIL;
   }

   rc = sdcc_handle_set_active_bootable_partition( hsdev_handles[jtag_prog_param.addr] );
   if (rc != SDCC_NO_ERROR) 
   {
     outputMessage("ERROR 0x%X: %s\n", rc, sdcc_err_string[rc]);
     return STATUS_FAIL;
   }

   return STATUS_SUCCESS;
}

/*===========================================================================

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  If success, return STATUS_SUCCESS else STATUS_FAIL

SIDE EFFECTS

===========================================================================*/
UINT32 jsdcc_gpp_program_emmc(void)
{
#ifdef FEATURE_GPP
   BOOLEAN rc = FALSE;
/*   outputMessage("Sizes are: %d, %d, %d, %d\n", *((UINT32*)(jtag_prog_param.data)), *((UINT32*)(jtag_prog_param.data+8)), *((UINT32*)(jtag_prog_param.data+16)), *((UINT32*)(jtag_prog_param.data+24)));*/
   sdcc_emmc_gpp_enh_desc_type desc;
   
   /* Extract the GPP sizes from the data buffer, sizes are stored using %LONG so are 4 bytes each */
   /* Sizes are in KB, so multiply by 2 to convert to sectors */
   desc.GPP_size[0] = (*((UINT32*)(jtag_prog_param.data)))*2;
   desc.GPP_size[1] = (*((UINT32*)(jtag_prog_param.data+4)))*2;
   desc.GPP_size[2] = (*((UINT32*)(jtag_prog_param.data+8)))*2;
   desc.GPP_size[3] = (*((UINT32*)(jtag_prog_param.data+12)))*2;
   desc.ENH_size = 0;
   desc.ENH_start_addr = 0;
   desc.GPP_enh_flag = 0;
   outputMessage("Sector sizes are: %d, %d, %d, %d\n", desc.GPP_size[0], desc.GPP_size[1], desc.GPP_size[2], desc.GPP_size[3]);

   rc = sdcc_config_emmc_gpp_enh (hsdev_current, &desc);

   if (rc != SDCC_NO_ERROR)
   {
     outputMessage("ERROR 0x%X: %s\n", rc, sdcc_err_string[rc]);
     return STATUS_FAIL;
   }
   else
   {
     outputMessage("SUCCESS\n");
   }
   return STATUS_SUCCESS;
#else
   outputMessage("GPP creation not permitted\n");
   return STATUS_FAIL;
#endif
}


