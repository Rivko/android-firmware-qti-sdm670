/*===========================================================================

JtagProgrammer_ufs.c

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
2015-09-24   rh      Adding firmware update support
2014-12-09   whc     inital version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/ 
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "JtagProgrammer.h"
#include "JtagProgrammerParam.h"
#include "ufs_api.h"
#include "JtagProgrammerBsp.h"
#include <Library/BaseMemoryLib.h>


/*===========================================================================

                           DEFINITIONS

===========================================================================*/

static struct ufs_info_type current_mem_info_ufs;

struct ufs_config_descr storage_extras;

/* device handle */
#define MAX_PARTITIONS 9
byte sector_buffer[4096]; //TODO: Make this size SCons-defined
unsigned int sector_buffer_offset = 0;
unsigned int max_partitions = MAX_PARTITIONS;
struct ufs_handle *partition_config;           /* User partition handle */
struct ufs_handle *partition_handles[MAX_PARTITIONS];
unsigned int current_handle_index = 0;
int lun_to_grow = -1;

char *partition_names[MAX_PARTITIONS] = {
    "LUN 0",
    "LUN 1",
    "LUN 2",
    "LUN 3",
    "LUN 4",
    "LUN 5",
    "LUN 6",
    "LUN 7",
   "WLUN BOOT"
};

char *err_strings[] = {
     "UFS_ERR_NO_ERROR",
     "UFS_ERR_NO_VALID_ENTRY",
     "UFS_ERR_IO",
     "UFS_ERR_INSUFFICIENT_MEMORY",
     "UFS_ERR_DEVICE_BUSY",
     "UFS_ERR_DEVICE_NOT_FOUND",
     "UFS_ERR_INVALID",
     "UFS_ERR_PROTOCAL",
     "UFS_ERR_TIME_OUT",
     "UFS_ERR_UNDEFINED"
};

UINT32 blocks_per_alloc_unit = 1024;

/*===========================================================================

                      FUNCTION PROTOTYPES

===========================================================================*/

void open_handles(void) {
    unsigned int i;
    for (i = 0; i < max_partitions - 1; i++)
    {
        partition_handles[i] = ufs_open(0, i);
    }
   partition_handles[i] = ufs_open(0, UFS_WLUN_BOOT);
}


UINT32 ReturnErrorCodeWithinBounds(INT32 rc)
{
   UINT32 ret_val;
   
   switch (rc) {
       case UFS_EOK:
           ret_val = 0;
           break;
       case UFS_ENOENT:
           ret_val = 1;
           break;
       case UFS_EIO:       
           ret_val = 2;
           break;
       case UFS_ENOMEM:
           ret_val = 3;
           break;
       case UFS_EBUSY:
           ret_val = 4;
           break;
       case UFS_ENODEV:
           ret_val = 5;
           break;
       case UFS_EINVAL:
           ret_val = 6;
           break;
       case UFS_EPROTO:
           ret_val = 7;
           break;
       case UFS_ETIME:
           ret_val = 8;
           break;
       default:
         ret_val = 9;
           break;
    }

    return ret_val;
            
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
UINT32 do_probe_ufs(void)
{
   UINT32 ret_code = STATUS_FAIL;
   INT32 rc = 0;
   BLOCK_SIZE = 4096;

   outputMessage("\n\nUFS do_probe(card_slot_in_use=%d)\n",card_slot_in_use);

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

        //outputMessage("\nUsing 'user specified' card slot %d\n",card_slot_in_use);

        partition_config = ufs_open(0, UFS_WLUN_DEVICE);

        // Basic NULL handle check and get required device parameters if any
        if (partition_config != NULL && (rc = ufs_get_device_info(partition_config, &current_mem_info_ufs)) == 0) {
            BLOCK_SIZE = current_mem_info_ufs.bMinAddrBlockSize * 512;
            blocks_per_alloc_unit = (current_mem_info_ufs.dSegmentSize * current_mem_info_ufs.bAllocationUnitSize * 512) / BLOCK_SIZE;

            // not openeing handle when EXTRA programming is used
            if ( jtag_prog_param.cmd_status != JTAG_CMD_PROG_EXTRAS) {
               // Open all handles
               open_handles();
            }

            ret_code = STATUS_SUCCESS;
            probe_status = MEM_DEVICE_FOUND;
        }
        else {
            if (NULL == partition_config) {
                outputMessage("Error: Unable to open CONFIG partition.\n");
            }
            else if (rc != 0) {
                outputMessage("Could not get config info. ERROR 0x%X: %s\n", rc, err_strings[ReturnErrorCodeWithinBounds(rc)]);
            }
            outputMessage("Possible causes: card not present? No power or clock to the card?\n");
            ret_code = STATUS_FAIL;
            probe_status = MEM_DEVICE_UNKNOWN;
        }

        current_handle_index = 0;

        // TODO: remove below few lines, only added since virtio doesn't support the config LUN
        //ret_code = STATUS_SUCCESS;
        //probe_status = MEM_DEVICE_FOUND;
        //current_handle_index = 1;

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
UINT32 jsdcc_mem_read_ufs(void)
{
    UINT8 *pdata;
    INT32 rc = 0;
    struct ufs_handle *partition_current = partition_handles[current_handle_index];

    pdata = jtag_prog_param.data;
    rc = ufs_read(partition_current, pdata, jtag_prog_param.addr, jtag_prog_param.data_length);

    if (rc != 0) 
    {
        outputMessage("Read ERROR 0x%X: %s\n", rc, err_strings[ReturnErrorCodeWithinBounds(rc)]);
        return STATUS_FAIL;
    }
    return STATUS_SUCCESS;
}  // jsdcc_mem_read


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
UINT32 jsdcc_mem_write_ufs(void)
{
    UINT8 *pdata;
    INT32 rc = 0;
    struct ufs_handle *partition_current = partition_handles[current_handle_index];

    outputMessage("Sector Write begins: \n");
    outputMessage("Addr %x, Size %x, Offset %x \n", 
        jtag_prog_param.addr, jtag_prog_param.data_length, 
        jtag_prog_param.mod_addr);
   
    pdata = jtag_prog_param.data;
    rc = ufs_write(partition_current, pdata, jtag_prog_param.addr, jtag_prog_param.data_length);

    if (rc != 0) 
    {
        outputMessage("Write ERROR 0x%X: %s\n", rc, err_strings[ReturnErrorCodeWithinBounds(rc)]);
        return STATUS_FAIL;
    }
    return STATUS_SUCCESS;
}  // jsdcc_mem_write


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
UINT32 jsdcc_mem_program_ufs(void)
{
   INT32 rc = 0;
   UINT8 *pdata = NULL;
   unsigned int blocks_to_write = 0;
   struct ufs_handle *partition_current = partition_handles[current_handle_index];

   if (jtag_prog_param.data_length == 0 || redirect_partition == 0) {
      return STATUS_SUCCESS;
   }

   // Print out debugging information
    //outputMessage("Addr %x, Size %x\n", jtag_prog_param.addr, jtag_prog_param.data_length);

   jtag_prog_param.data_length = jtag_prog_param.data_length < redirect_partition ? jtag_prog_param.data_length : redirect_partition;
   redirect_partition -= jtag_prog_param.data_length;

   if (jtag_prog_param.data_length < BLOCK_SIZE) {
      if (jtag_prog_param.data_length + sector_buffer_offset > BLOCK_SIZE) {
         return STATUS_FAIL;
      }
      memcpy(sector_buffer + sector_buffer_offset, jtag_prog_param.data, jtag_prog_param.data_length);
      sector_buffer_offset += jtag_prog_param.data_length;
      
      if (sector_buffer_offset == BLOCK_SIZE || 0 == redirect_partition) {
         pdata = sector_buffer;
         blocks_to_write = 1;
         sector_buffer_offset = 0;
      }
   }
   else {
      if (jtag_prog_param.data_length % BLOCK_SIZE != 0) {
         return STATUS_FAIL;
      }
      pdata = jtag_prog_param.data;
      blocks_to_write = jtag_prog_param.data_length / BLOCK_SIZE;
   }

   if (NULL != pdata) {
      rc = ufs_write(partition_current, pdata, redirect_offset, blocks_to_write);
      redirect_offset += blocks_to_write;
      if (rc != 0) {
         outputMessage("WRITE ERROR 0x%X: %s\n", rc, err_strings[ReturnErrorCodeWithinBounds(rc)]);
         return STATUS_FAIL;
      }
   }
    return STATUS_SUCCESS;
}  // jsdcc_mem_program


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
UINT32 jsdcc_mem_print_info_ufs(void)
{
   INT32 rc = 0;
   INT32 j = 0;
   char buf[30];
   struct ufs_handle *partition_current = partition_handles[current_handle_index];

   if (NULL == partition_current)
   {
     outputMessage("Using UFS_WLUN_DEVICE");
     partition_current = partition_config;
   }


   outputMessage("\n==============================\n");
   outputMessage(  "=====       INFO         =====\n");
   outputMessage(  "==============================\n");

   if(partition_current == NULL)
   {
    outputMessage("\nERROR: No device detected on that slot\n");
     return STATUS_FAIL;
   }

   rc = ufs_get_device_info(partition_current, &current_mem_info_ufs);

   if (rc != 0) 
   {
     outputMessage("GET INFO ERROR 0x%X: %s\n", rc, err_strings[ReturnErrorCodeWithinBounds(rc)]);
     return STATUS_FAIL;
   }

   // Print out the detail
   outputMessage ("fInitialized: %d\n", current_mem_info_ufs.fInitialized);
   outputMessage ("bLUN: %x\n", current_mem_info_ufs.bLUN);
   outputMessage ("bNumberLu: %d\n", current_mem_info_ufs.bNumberLu);
   outputMessage ("wManufacturerID: %x\n", current_mem_info_ufs.wManufacturerID);
   outputMessage ("bBootEnable: %d\n", current_mem_info_ufs.bBootEnable);
   outputMessage ("qTotalRawDeviceCapacity: %u kB\n", (uint32_t)(current_mem_info_ufs.qTotalRawDeviceCapacity / 2));
   outputMessage ("bMinAddrBlockSize: %d\n", current_mem_info_ufs.bMinAddrBlockSize);
   outputMessage ("dSegmentSize: %d\n", current_mem_info_ufs.dSegmentSize);
   outputMessage ("bAllocationUnitSize: %d\n", current_mem_info_ufs.bAllocationUnitSize);
   outputMessage ("bRPMB_ReadWriteSize: %d\n", current_mem_info_ufs.bRPMB_ReadWriteSize);
   outputMessage ("dNumAllocUnits: %d\n", current_mem_info_ufs.dNumAllocUnits);
   outputMessage ("bLogicalBlockSize: %d\n", current_mem_info_ufs.bLogicalBlockSize);
   outputMessage ("bProvisioningType: %x\n", current_mem_info_ufs.bProvisioningType);
   outputMessage ("bLUWriteProtect: %x\n", current_mem_info_ufs.bLUWriteProtect);
   outputMessage ("bBootLunID: %x\n", current_mem_info_ufs.bBootLunID);
   outputMessage ("bMemoryType: %x\n", current_mem_info_ufs.bMemoryType);
   outputMessage ("dLuTotalBlocks: %d\n", current_mem_info_ufs.dLuTotalBlocks);
   for (j=0;j<28;j++) 
   {
      buf[j] = current_mem_info_ufs.inquiry_str[j] != 0 ? current_mem_info_ufs.inquiry_str[j] : ' ';
   }

   outputMessage ("inquiry_str: %s\n", buf);

   jtag_prog_param.data_length = current_mem_info_ufs.dLuTotalBlocks;
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
UINT32 jsdcc_mem_erase_ufs(void)
{
   INT32 rc = 0;
   UINT8 *pdata;
   UINT32 erase_data;
    struct ufs_handle *partition_current = partition_handles[current_handle_index];
   
   while(jtag_prog_param.data_length > 0)
   {
       erase_data = jtag_prog_param.data_length < (PARAM_BUFFER_SIZE/BLOCK_SIZE) ? jtag_prog_param.data_length : (PARAM_BUFFER_SIZE/BLOCK_SIZE);

       /*rc = sdcc_handle_erase(partition_user, BLOCK_SIZE*jtag_prog_param.addr, jtag_prog_param.data_length);*/
       if (partition_current != NULL) {
         pdata = jtag_prog_param.data;
         memset(pdata, 0x00, BLOCK_SIZE*erase_data);
         //rc = sdcc_handle_write(partition_current, jtag_prog_param.addr, pdata, erase_data);
       }
       else {
         outputMessage("No current handle to erase");
         return STATUS_FAIL;
       }
       if (rc != 0) 
       {
         outputMessage("ERROR 0x%X: %s\n", rc, err_strings[ReturnErrorCodeWithinBounds(rc)]);
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
UINT32 jsdcc_mem_compute_crc_ufs(void)
{ 
   UINT8 *pdata;
   INT32 rc = 0;
    struct ufs_handle *partition_current = partition_handles[current_handle_index];

   UINT32 sectors_to_read;
   sectors_to_read = jtag_prog_param.data_length / BLOCK_SIZE;
   if (jtag_prog_param.data_length % BLOCK_SIZE != 0)
        sectors_to_read++;

   pdata = jtag_prog_param.data;
   rc = ufs_read(partition_current, pdata, jtag_prog_param.addr, sectors_to_read);

   if (rc != 0) 
   {
     outputMessage("READ ERROR 0x%X: %s\n", rc, err_strings[ReturnErrorCodeWithinBounds(rc)]);
     return STATUS_FAIL;
   }

   jtag_prog_param.data_length = calc_crc32((uint8 *)pdata, jtag_prog_param.data_length);

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
UINT32 jsdcc_mem_chip_erase_ufs(void)
{
    INT32 rc = 0;
    struct ufs_handle *partition_current;
    unsigned int i;

    for (i = 0; i < (max_partitions - 1); i++)
    {
        if (i==3) {
            outputMessage("NOT calling ufs_format(LUN %d) since CDT and DDR data \n", i);
            continue;
        }

        partition_current = partition_handles[i];
        if (NULL != partition_current) 
        {
            outputMessage("Calling ufs_format(LUN %d) \n", i);
            rc = ufs_format(partition_current);
            if (rc != 0) 
            {
                outputMessage("CHIP ERASE ufs_format ERROR 0x%X: %s\n", rc, err_strings[ReturnErrorCodeWithinBounds(rc)]);
                //displayErrorString(rc);
                return STATUS_FAIL;
            }
        }
    }

    outputMessage("CHIP_ERASE: Success! \n");
    return STATUS_SUCCESS;
}  // jsdcc_mem_chip_erase

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
UINT32 jsdcc_mem_set_hw_partition_ufs(void)
{
   // Not closing the old handle on purpose - Ensure the clock not getting turned off
   if (jtag_prog_param.addr >= max_partitions) {
      outputMessage("Failed to switch partition.\n");
      outputMessage("ERROR: You requested to switch to partition %d that doesn't exist.\nOnly %d partitions exist.\n", jtag_prog_param.addr, max_partitions);
      probe_status = MEM_DEVICE_UNKNOWN;
      return STATUS_FAIL;
   }
   else
   {
     if(NULL == partition_handles[current_handle_index])
     {
        outputMessage("Unable to open partition %s, partition handle invalid", partition_names[jtag_prog_param.addr]);
       probe_status = MEM_DEVICE_UNKNOWN;
         return STATUS_FAIL;
     }
     
      current_handle_index = jtag_prog_param.addr;
      outputMessage("Successfully switch to partition %s", partition_names[jtag_prog_param.addr]);
   }

   probe_status = MEM_DEVICE_FOUND;
   return STATUS_SUCCESS;
}

/*===========================================================================

DESCRIPTION
  This function call the API to set the active boot partition

DEPENDENCIES

RETURN VALUE
  If success, return the starting sector of the givin partition, if
  failed, return 0

SIDE EFFECTS

===========================================================================*/
UINT32 jsdcc_mem_set_active_boot_ufs(void)
{
   INT32 rc = 0;

   /* Not closing the old handle on purpose - Ensure the clock not getting turned off*/
   if (jtag_prog_param.addr >= max_partitions) {
        outputMessage("Failed to switch partition.\n");
        outputMessage("ERROR: You requested to switch to partition %d that doesn't exist.\nOnly %d partitions exist.\n", jtag_prog_param.addr, max_partitions);
        probe_status = MEM_DEVICE_UNKNOWN;
        return STATUS_FAIL;
   }

   rc = ufs_set_bootable(partition_config, jtag_prog_param.addr);
   if (rc != 0)
   {
     outputMessage("ERROR: 0x%X: %s\n", rc, err_strings[ReturnErrorCodeWithinBounds(rc)]);
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
UINT32 jsdcc_extras_program_ufs(void)
{
   INT32 rc = FALSE;
   size_t alloc_units, units_to_create = 0;
   size_t capacity_to_alloc_factor;
   size_t enhanced1_units = 0, enhanced2_units = 0;
   size_t conversion_ratio = 1;
   int i;

   if(partition_config == NULL)
   {
      outputMessage("Opening UFS device LU failed\n");
      return STATUS_FAIL;
   }

   ufs_set_refclk_freq( partition_config, 19200000);

   rc = ufs_get_device_info(partition_config, &current_mem_info_ufs);
   if (0 != rc) {
        outputMessage("Could not get partition info\n");
        return STATUS_FAIL;
   }
   capacity_to_alloc_factor = (blocks_per_alloc_unit * BLOCK_SIZE) / 512;
   if (current_mem_info_ufs.qTotalRawDeviceCapacity % capacity_to_alloc_factor != 0) {
        outputMessage("Raw capacity not a multiple of alloc unit size\n");
        return FALSE;
   }
   alloc_units = (current_mem_info_ufs.qTotalRawDeviceCapacity / capacity_to_alloc_factor) ;

   units_to_create = 0;
   enhanced1_units = enhanced2_units = 0;
   for (i = 0; i < (max_partitions - 1) && units_to_create <= alloc_units; i++) {
        // Convert dNumAllocUnits, which is currently in blocks, into alloc units
        if ((storage_extras.unit[i].dNumAllocUnits % blocks_per_alloc_unit) == 0) {
            storage_extras.unit[i].dNumAllocUnits /= blocks_per_alloc_unit;
        } else {
            storage_extras.unit[i].dNumAllocUnits = storage_extras.unit[i].dNumAllocUnits / blocks_per_alloc_unit + 1;
        }

        if (0 == storage_extras.unit[i].bMemoryType) {
            units_to_create += storage_extras.unit[i].dNumAllocUnits;
        }
        else if (3 == storage_extras.unit[i].bMemoryType) {
            enhanced1_units += storage_extras.unit[i].dNumAllocUnits;
            storage_extras.unit[i].dNumAllocUnits *= (current_mem_info_ufs.wEnhanced1CapAdjFac / 0x100);
            units_to_create += storage_extras.unit[i].dNumAllocUnits;
        }
        else if (4 == storage_extras.unit[i].bMemoryType) {
            enhanced2_units += storage_extras.unit[i].dNumAllocUnits;
            storage_extras.unit[i].dNumAllocUnits *= (current_mem_info_ufs.wEnhanced1CapAdjFac / 0x100);
            units_to_create += storage_extras.unit[i].dNumAllocUnits;
        }
        else {
            outputMessage("Unsupported memory type %d\n", storage_extras.unit[i].bMemoryType);
            return FALSE;
        }
   }
   if (enhanced1_units > current_mem_info_ufs.dEnhanced1MaxNAllocU) {
        outputMessage("Size %d exceeds max enhanced1 area size %d\n", enhanced1_units, current_mem_info_ufs.dEnhanced1MaxNAllocU);
        return FALSE;
   }
   if (enhanced2_units > current_mem_info_ufs.dEnhanced2MaxNAllocU) {
        outputMessage("Size %d exceeds max enhanced2 area size %d\n", enhanced2_units, current_mem_info_ufs.dEnhanced2MaxNAllocU);
        return FALSE;
   }
   if (units_to_create > alloc_units) {
        outputMessage("Specified size %d exceeds device size %d\n", units_to_create, alloc_units);
        return FALSE;
    }

   if (lun_to_grow != -1) {
        if (0 == storage_extras.unit[i].bMemoryType)
            conversion_ratio = 1;
        else if (3 == storage_extras.unit[i].bMemoryType)
            conversion_ratio = (current_mem_info_ufs.wEnhanced1CapAdjFac / 0x100);
        else if (4 == storage_extras.unit[i].bMemoryType)
            conversion_ratio = (current_mem_info_ufs.wEnhanced2CapAdjFac / 0x100);

        storage_extras.unit[lun_to_grow].dNumAllocUnits += ( (alloc_units - units_to_create) / conversion_ratio );
   }
   
   rc = ufs_configure_device (partition_config, &storage_extras);

   if (rc != 0)
   {
     outputMessage("Status FAIL ERROR %d\n", rc);
     return STATUS_FAIL;
   }
   else
   {
     outputMessage("%d\n", storage_extras.bNumberLU);
     outputMessage("Status SUCCESS\n");
   }
   return STATUS_SUCCESS;
}

/*===========================================================================

DESCRIPTION
   Do field firmware update on UFS devices

DEPENDENCIES
  Assumes jtag_prog_param (global variable) has valid parameters.

RETURN VALUE
  If success, return STATUS_SUCCESS else STATUS_FAIL

SIDE EFFECTS

===========================================================================*/
UINT32 jsdcc_mem_firmware_update_ufs(void)
{
   UINT8 *pdata = NULL;
   INT32 rc = 0;
   unsigned int blocks_to_write = 0;
   struct ufs_handle *partition_current = partition_handles[current_handle_index];

   if (jtag_prog_param.data_length == 0 || redirect_partition == 0) {
      return STATUS_SUCCESS;
   }

   // Print out debugging information
   //outputMessage("Addr %x, Size %x, Remain %x\n", jtag_prog_param.addr, jtag_prog_param.data_length, redirect_partition);

   jtag_prog_param.data_length = jtag_prog_param.data_length < redirect_partition ? jtag_prog_param.data_length : redirect_partition;
   redirect_partition -= jtag_prog_param.data_length;

   if (jtag_prog_param.data_length < BLOCK_SIZE) {
      if (jtag_prog_param.data_length + sector_buffer_offset > BLOCK_SIZE) {
         return STATUS_FAIL;
      }
      memcpy(sector_buffer + sector_buffer_offset, jtag_prog_param.data, jtag_prog_param.data_length);
      sector_buffer_offset += jtag_prog_param.data_length;
      
      if (sector_buffer_offset == BLOCK_SIZE || 0 == redirect_partition) {
         pdata = sector_buffer;
         blocks_to_write = 1;
         sector_buffer_offset = 0;
      }
   }
   else {
      if (jtag_prog_param.data_length % BLOCK_SIZE != 0) {
         return STATUS_FAIL;
      }
      pdata = jtag_prog_param.data;
      blocks_to_write = jtag_prog_param.data_length / BLOCK_SIZE;
   }

   if (NULL != pdata) {
      outputMessage("Write: Addr %x, Size %x\n", redirect_offset, blocks_to_write);
      rc = ufs_fw_update_write (partition_current, blocks_to_write * BLOCK_SIZE, 
                                redirect_offset * BLOCK_SIZE, pdata);
      redirect_offset += blocks_to_write;
      if (rc != 0) {
         outputMessage("WRITE ERROR 0x%X: %s\n", rc, err_strings[ReturnErrorCodeWithinBounds(rc)]);
         return STATUS_FAIL;
      }
   }
   return STATUS_SUCCESS;

}  // jsdcc_mem_firmware_update

