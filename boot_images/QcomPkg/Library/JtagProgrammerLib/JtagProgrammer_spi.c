/*===========================================================================

JtagProgrammer_spi.c

 This is the Jtag programmer tool for SPI_NOR implementation.

 Copyright (c) 2015 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
 All Rights Reserved.
 Qualcomm Confidential and Proprietary

============================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



when         who     what, where, why
----------   ---     ----------------------------------------------------------
2015-03-30   whc  initial version to support SPI-NOR

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/ 
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "DALDeviceId.h"
#include "DalDevice.h"
#include "DALHeap.h"
#include "DALSys.h"
#include "flash.h"
#include "JtagProgrammer.h"
#include "JtagProgrammerParam.h"
#include <Library/BaseMemoryLib.h>
#include "JtagProgrammerBsp.h"
#include "ClockBoot.h"


/*===========================================================================

                           DEFINITIONS

===========================================================================*/
#define SIZE_64K 65536
#define SIZE_32K 32768
#define SIZE_4K  4096

/* enum to indicate status of probe */
//static probe_code_type probe_status = MEM_DEVICE_NOT_PROBED;

DALDEVICEID dev_id= DALDEVICEID_FLASH_DEVICE_1;

/* NOR device structure */
flash_handle_t handle_flash;

struct flash_info dev_info;

/* NOR device parameters */
UINT32 total_block_count;  /* number of erasable units in flash */
UINT32 page_size_bytes;    /* number of bytes in a page         */
UINT32 total_pages;        /* number of pages in the device     */
char device_name[32];
DALBOOL dev_is_initialized = FALSE;

//DAL heap
UINT8 dalheap[0x4000];

typedef struct
{
   UINT32 block_size_bytes;
   UINT32 pages_per_block;  
   UINT32 block_count;  
   UINT32 total_block_count;  /* number of erasable units in flash */
   UINT32 page_size_bytes;	  /* number of bytes in a page		   */
   UINT32 total_pages;		  /* number of pages in the device	   */
   struct flash_nor_erase_region_info erase_region_info;
   char device_name[32];
} NOR_DEVICE_INFO;

static NOR_DEVICE_INFO nor_device_info;

#define SIZE_IN_PAGES(x) ( ((x % nor_device_info.page_size_bytes) == 0) ? x/nor_device_info.page_size_bytes : x/nor_device_info.page_size_bytes+1 )

 /*===========================================================================

                     MACRO DEFINITIONS FOR MODULE

===========================================================================*/


/*===========================================================================

                      FUNCTION PROTOTYPES

===========================================================================*/
UINT32 jsdcc_mem_read_spi(void);
UINT32 jsdcc_mem_write_spi(void);
UINT32 jsdcc_mem_erase_spi(void);
UINT32 jsdcc_mem_compute_crc_spi(void);
UINT32 jsdcc_mem_print_info_spi(void);
UINT32 jsdcc_mem_program_spi(void);
UINT32 jsdcc_mem_chip_erase_spi(void);

void nor_utils_strcpy(UINT8 *dest, const UINT8 *src, UINT32 len)
{
  memcpy(dest, src, len);
  dest[len-1] ='\0';
}

void flash_utils_deinit(void)
{
  if (dev_is_initialized )
  {
    flash_device_close(handle_flash);
    flash_device_detach(handle_flash);
    dev_is_initialized = FALSE;
  }
}

UINT32 flash_utils_init(void)
{
  INT32 result = FLASH_DEVICE_DONE;
  struct flash_nor_erase_region_info erase_region_info;
  UINT32 block_size_bytes = 0;
  UINT32 pages_per_block;  
  UINT32 block_count;  
  UINT32 index;

  if (dev_is_initialized)
  {
    return FLASH_DEVICE_DONE;
  }

  if(FLASH_DEVICE_DONE != flash_device_attach(dev_id,
    &handle_flash))                 
  {
    outputMessage("Error attaching\n");
    return FLASH_DEVICE_FAIL;
  }

  if(FLASH_DEVICE_DONE != flash_device_open(handle_flash))                 
  {
    outputMessage("Error Opening\n");
    return FLASH_DEVICE_FAIL;
  }

  result = flash_get_info(handle_flash, FLASH_DEVICE_INFO, &dev_info);

  if(FLASH_DEVICE_DONE != result)
  {
    outputMessage("Get Info Error\n");
    result = FLASH_DEVICE_FAIL;
    goto close_and_detach;
  }

  /* Get some information about the device */
  total_block_count = dev_info.block_count;
  page_size_bytes = dev_info.page_size_bytes;

  nor_utils_strcpy((UINT8 *)&device_name, (UINT8 *)dev_info.device_name, 
    sizeof(device_name));

  nor_utils_strcpy((UINT8 *)&nor_device_info.device_name, (UINT8 *)dev_info.device_name, 
    sizeof(device_name));

  total_pages = 0;

  result = flash_get_info(handle_flash, FLASH_NOR_ERASE_REGION_INFO, 
    &erase_region_info);
  
  if(FLASH_DEVICE_DONE == result)
  {
    for(index = 0; index < erase_region_info.erase_region_count; index++)
    {
      block_size_bytes = 
        erase_region_info.erase_region[index].block_size_in_bytes;
      block_count = 
        erase_region_info.erase_region[index].block_count;
      pages_per_block =  block_size_bytes / page_size_bytes;
      total_pages += pages_per_block * block_count;
    }
  }
  else
  {
    outputMessage("GetInfo error\n");  
    result = FLASH_DEVICE_FAIL;
    goto close_and_detach;
  }

  BLOCK_SIZE = block_size_bytes;
  nor_device_info.total_block_count = total_block_count;
  nor_device_info.page_size_bytes = page_size_bytes;
  nor_device_info.total_pages = total_pages;
  nor_device_info.erase_region_info = erase_region_info ;

  dev_is_initialized = TRUE;

  return result;

close_and_detach:
   flash_device_close(handle_flash);
   flash_device_detach(handle_flash);
   return result;
}

UINT32 do_probe_spi(void)
{
  UINT32 ret_code;

  DALSYS_HeapInit((void *)dalheap, 0x4000, TRUE);
  DALSYS_InitMod(NULL);

  ret_code = flash_utils_init();
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
UINT32 jsdcc_mem_read_spi(void)
{
    UINT32 page, data_length;
    INT32 result = FLASH_DEVICE_DONE;
	byte *pdata;

    pdata = jtag_prog_param.data;
    page = jtag_prog_param.addr * 16;
	data_length = jtag_prog_param.data_length * 16;
    
    outputMessage("Reading Sector 0x%x \n", jtag_prog_param.addr);
       
    result = flash_read_pages(handle_flash, pdata,
       page, data_length);
    
    if (FLASH_DEVICE_DONE != result)
    {
      outputMessage("Failed with status %d\n", result);
      flash_utils_deinit();
	  return STATUS_FAIL;
    }
    else
    {
        outputMessage("Success\n");
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
UINT32 jsdcc_mem_write_spi(void)
{
    UINT32 page, data_length;
	byte *pdata;
    INT32 result = FLASH_DEVICE_DONE;

    page = jtag_prog_param.addr * 16;
    data_length = jtag_prog_param.data_length * 16;
    
    outputMessage("Writing to sector 0x%x\n", jtag_prog_param.addr);
    
	pdata = jtag_prog_param.data;
    result = flash_write_pages(handle_flash, pdata,
      page, data_length);
    
    if (FLASH_DEVICE_DONE != result)
    {
        outputMessage("Failed with status %d\n", result);
        flash_utils_deinit();
		return STATUS_FAIL;
    }
    else
    {
      outputMessage("Success\n");
    }

    return STATUS_SUCCESS;
}  // jsdcc_mem_write



/*===========================================================================

DESCRIPTION
  This function erases specified number of sectors from the card

DEPENDENCIES
  Assumes jtag_prog_param (global variable) has valid parameters.

RETURN VALUE
  If success, return STATUS_SUCCESS else STATUS_FAIL

SIDE EFFECTS

===========================================================================*/

UINT32 jsdcc_mem_erase_partition_spi()
{
   UINT16 index;
   UINT32 start_block;
   
   for(index = 0; index < jtag_prog_param.data_length; index++) 
   {
      start_block = jtag_prog_param.addr + index;

	  if (FLASH_DEVICE_DONE != flash_erase_blocks(handle_flash, start_block, 1))
      {
      	 outputMessage("Erase Failed\n");
      	 return STATUS_FAIL;
      }
   }
   return STATUS_SUCCESS;
}


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
UINT32 jsdcc_mem_program_spi(void)
{
   byte *pdata;
   INT32 result = FLASH_DEVICE_DONE;

   if(sector_writing) {
      jtag_prog_param.addr = redirect_offset * 16 + SIZE_IN_PAGES(jtag_prog_param.addr);
      jtag_prog_param.data_length = jtag_prog_param.data_length < redirect_partition ? jtag_prog_param.data_length : redirect_partition;
      if (jtag_prog_param.data_length == 0) {
        return STATUS_SUCCESS;
      }
      redirect_partition -= jtag_prog_param.data_length;
   }
   pdata = jtag_prog_param.data;
 
   result = flash_write_pages(handle_flash, pdata,
	 jtag_prog_param.addr, SIZE_IN_PAGES(jtag_prog_param.data_length));
   
   if (FLASH_DEVICE_DONE != result)
   {
	 outputMessage("Failed with status %d\n", result);
	 flash_utils_deinit();
	 return STATUS_FAIL;
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
UINT32 jsdcc_mem_print_info_spi(void)
{
   UINT32 index;

   outputMessage("Device Name = %s\n", nor_device_info.device_name); 
   outputMessage("Erase Region Count = %d \n", \
   nor_device_info.erase_region_info.erase_region_count);
   
   for(index = 0; index < nor_device_info.erase_region_info.erase_region_count; index++)
   {
     nor_device_info.block_size_bytes = 
       nor_device_info.erase_region_info.erase_region[index].block_size_in_bytes;
     nor_device_info.block_count = 
       nor_device_info.erase_region_info.erase_region[index].block_count;
     nor_device_info.pages_per_block =  nor_device_info.block_size_bytes / page_size_bytes;

     outputMessage("Region %d block count = %d \n", index, nor_device_info.block_count);
     outputMessage("Region %d block size = %d \n", index, nor_device_info.block_size_bytes);
     outputMessage("Region %d pages per block = %d \n", index, 
       nor_device_info.pages_per_block);
   }
 
   outputMessage("Total Block count is %d \n", nor_device_info.total_block_count);
   outputMessage("Page size  is %d \n", nor_device_info.page_size_bytes);
   outputMessage("Total Pages Avail is %d \n", nor_device_info.total_pages);
   
   return STATUS_SUCCESS;
}  /* jsdcc_mem_print_info */

/*===========================================================================

DESCRIPTION
  This function erases a specified sector from the card

DEPENDENCIES
  Assumes jtag_prog_param (global variable) has valid parameters.

RETURN VALUE
  If success, return STATUS_SUCCESS else STATUS_FAIL

SIDE EFFECTS

===========================================================================*/
UINT32 jsdcc_mem_erase_spi()
{
    UINT32 start_block = jtag_prog_param.addr;

    outputMessage("Erasing Block 0x%x\n", start_block);
    
    /* Ensure block is in range */
    if (start_block >= total_block_count)
    {
      outputMessage("is out of range \n");
      return FLASH_DEVICE_FAIL;
    }
         
    if (FLASH_DEVICE_DONE != flash_erase_blocks(handle_flash, start_block, 1))     
    {
      outputMessage("Failed\n");
      flash_utils_deinit();
	  return STATUS_FAIL;
    }
    else
    {
        outputMessage("Success\n");
    }

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
UINT32 jsdcc_mem_compute_crc_spi(void)
{ 
   UINT32 page, data_length;
   INT32 result = FLASH_DEVICE_DONE;
   byte *pdata;

   pdata = jtag_prog_param.data;
   page = jtag_prog_param.addr * 16;
   data_length = SIZE_IN_PAGES(jtag_prog_param.data_length);
   
   outputMessage("Reading Sector 0x%x \n", jtag_prog_param.addr);
	  
   result = flash_read_pages(handle_flash, pdata,
	  page, data_length);
   
   if (FLASH_DEVICE_DONE != result) 
   {
     outputMessage("Failed with status %d\n", result);
     flash_utils_deinit();
     return STATUS_FAIL;
   }
   else
   {
	   outputMessage("Success\n");
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

UINT32 jsdcc_mem_chip_erase_spi(void)
{
    UINT16 index;
    INT32 result = FLASH_DEVICE_DONE;
	UINT32 block_count; 
	UINT32 start_block;

    block_count = nor_device_info.total_block_count/(SIZE_64K/BLOCK_SIZE);
    /* Erase every block in the flash, one at a time */
    for (index = 0; index < block_count; 
         index++)
    {
      start_block = index; 
      result = flash_erase_blocks_by_size(handle_flash, BLOCK_SIZE_64KB, start_block, 1);  

	  if (result != FLASH_DEVICE_DONE)
      {
        outputMessage("Erase chip failed");
        return STATUS_FAIL;
      }
    }  
	   
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
UINT32 jsdcc_mem_set_active_boot_spi(void)
{
   return STATUS_SUCCESS;
}

