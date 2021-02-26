/*===========================================================================

                         Boot SD RAM Dump Feature File

GENERAL DESCRIPTION
  This feature file contains definitions  for  functions for taking memory
  dumps to SD card.

Copyright 2010-2014, 2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who             what, where, why
--------   ---         --------------------------------------------------
09/22/17   rohik	   Added support for dumping the logdump partition into sd card
09/21/17   rohik	   Fix SD Card dump length for 64bit
09/06/17   yps         umount coldplug device after operation done
08/24/17   yps         Fixed SD card dump issue for 6GB DDR
06/22/17   ds          coldplug changes
04/19/17   kpa         ensure dump table is big enough to accept all entries
04/14/15   ao          adjust baseaddr for 64bit in function boot_process_sd_dumps()
01/01/15   bh          Added stdlib header files
10/01/14   ck          Added boot_extern_hotplug_interface header
07/18/14   ck          Removed bl_shared_data from parameter list
06/20/14   kedara      Remove std lib header inlcudes for compilation using llvm.
05/30/14   kedara      Replace snprintf by qsnprintf.
04/02/14   ck          Fixed KW issues
03/18/14   ck          Updated boot_hw_reset calls as they now take a reset type parameter
10/18/13   jz          Updated to support the new HotPlug API to poll the device
10/02/13   wg          Updated MAX_SIZE_FOLDER_STRING size for up to 999 folders
					   for RamDump 
07/11/13   dh          Rewrite ram dump code. Add logic to generate the ram dump
                       header file(ramdump.bin) on the same directory.
07/04/13   hh          Move ram dump to directory /ram_dump
05/15/13   dh          Reset device at the end of sd card ram dump if emmc raw ramdump
                        cookie is set
05/07/13   wg          fix klocwork warnings
04/22/13   dh          switch to use boot_efs_opendir, boot_efs_closedir and boot_befs_mkdir
04/17/13   plc         Make Cumulative Ram Dumps feature replace legacy
04/02/13   dh          Call dload_mem_debug_supported to determine if dump is allowed
03/29/13   plc         Add logic to dump multiple times, and toggle LED
05/02/12   dh          change all local char array to global static variable
04/09/12   dh          Use boot_efs_errno instead of efs_errno
11/09/11   kedar       Skip DLOAD mode after copying system dumps to Sdcard if 
                       reset cookie file present. 
06/09/11   kedar       Replace unsafe string functions and use strlcpy, strlcat
02/01/11   dh          add feature flag to protect dload_mem_debug_init
                       and only execute it if auth is disabled.
01/21/11   dh          fix klocwok warning
7/22/10   Kedar        Initial version  

===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_sd_ramdump.h"
#include "boot_dload_debug.h"
#include "boot_sdcc.h"
#include "boot_util.h"
#include "boot_extern_coldplug_interface.h"
#include "boot_extern_efs_interface.h"
#include "boothw_target.h"
#include "boot_cache_mmu.h"
#include "boot_visual_indication.h"
#include "boot_shared_imem_cookie.h"
#include "boot_raw_partition_ramdump.h"
#include <string.h>
#include <stdio.h>
#include "LoaderUtils.h"
#include "boot_target.h"
#include "boot_extern_pmic_interface.h"
#include "boot_whitelist_prot.h"
#include "boot_flash_dev_if.h"


/* Buffer to store filenames */
static char cookiefilepath[MAX_FILE_NAME];
static char cookiefilename[MAX_FILE_NAME];
static char filename[MAX_FILE_NAME];
static char errmax_filename[MAX_FILE_NAME];
static char err_filename[MAX_FILE_NAME];
static char foldername[MAX_FILE_NAME];
static char foldernumber[MAX_FILE_NAME];

static boot_ramdump_errlog_data_type boot_sd_rd_errdata; 

static uint32 max_region_entries;
/* 512 Bytes buffer if the base address is 0*/
static uint8 sector_buf[SECTOR_SIZE]; 

extern uint8 log_dump_partition_id[];

#define MAX_BUF_SIZE 1024
static uint8 memory_buffer[MAX_BUF_SIZE];

void boot_dump_logdump_partition( void );
  
/* Max number of cumulative dumps supported */
#define MAXDUMPS 100
#define MAX_SIZE_FOLDER_STRING 5 /* foldernames range from 1-100, needed for same qsnprintf */

#define SD_DUMP_HEADER_NAME "rawdump.bin"

/*=========================================================================
                            
                       FUNCTION DEFINITIONS

=========================================================================*/

/*===========================================================================

**  Function :  boot_ramdump_reset

** ==========================================================================
*/
/*!
* 
* @brief :  This routine checks for the reset cookie file on sd card and resets
*  the target if its found. This is done to prevent device from continuing to
*  bootup in dload mode after saving ramdumps to SD card
*
* @param[in]
*  None
* 
* @par Dependencies:
*  Must be called after efs_boot_initialize.
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
static void boot_ramdump_reset( void )
{
  int32 fs_handle = INVALID_FILE_HANDLE;
  
  uint32 str_size=0;

  str_size = strlcpy(cookiefilename, cookiefilepath, MAX_FILE_NAME);
  BL_VERIFY((str_size < MAX_FILE_NAME), BL_ERR_INVALID_FILE_NAME|BL_ERROR_GROUP_BOOT);

  str_size = strlcat(cookiefilename, "rtcookie.txt", MAX_FILE_NAME);
  BL_VERIFY((str_size < MAX_FILE_NAME), BL_ERR_INVALID_FILE_NAME|BL_ERROR_GROUP_BOOT );

  fs_handle = boot_efs_open(cookiefilename,O_RDONLY);

  /* Reset target if Cookie file exists.*/
  if(!(fs_handle < 0))
  {
    /*File exists. Close reset cookie file handle */
    boot_efs_close(fs_handle);

    /* If cache is on, some data might still be in the cache and not 
    written to memory. So flush the cache before causing a watchdog reset */
    mmu_flush_cache();

    /* reset device */
    boot_hw_reset(BOOT_HARD_RESET_TYPE);
  }
}


/*===========================================================================

**  Function :  boot_efs_write_file

** ==========================================================================
*/
/*!
* 
* @brief : Helper function to write certain size of data to a given file
*          boot_sd_rd_errdata.err_no will be set if there's an error
*
* @param[in]
*  None
* 
* @par Dependencies:
*  Must be called after efs_boot_initialize.
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
static void boot_efs_write_file(int32 fs_handle, uint8 *buf_addr, uint32 size)
{
  int32 tbytes_written;
  
  while(size > 0)
  {
    boot_toggle_led();
    tbytes_written = boot_efs_write(fs_handle, buf_addr, size);
    if(tbytes_written < 0)
    {
      /* Break on any EFS write error */
      boot_sd_rd_errdata.err_no = boot_efs_errno;
      break;
    }
    size -= tbytes_written;
    buf_addr += tbytes_written;
  }
}


/*===========================================================================

**  Function :  boot_write_ram_dump_headers

** ==========================================================================
*/
/*!
* 
* @brief : Helper function to write a dump header and section header
*          to the beginning of a file 
*
* @param[in]
*  None
* 
* @par Dependencies:
*  Must be called after efs_boot_initialize.
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
static void boot_write_ram_dump_headers(int32 dump_header_handle)
{
  /*seek to beginning of the file */
  boot_efs_lseek(dump_header_handle, 0, SEEK_SET);
  
  boot_efs_write_file(dump_header_handle,
                 (void *)&raw_dump_header, 
                 DUMP_HEADER_SIZE);
                 
  boot_efs_write_file(dump_header_handle, 
                 (void *)raw_dump_section_header_table, 
                 max_region_entries * SECTION_HEADER_SIZE);
}


/*===========================================================================

**  Function :  find_next_available_index

** ==========================================================================
*/
/*!
* 
* @brief  
*    This routine analyzes the integer incremented folder names in the root 
*   directory of the SD device, and returns the value of the next available
*   folder.
*
*   When no available indexes are left available, "0" is returned as an error
*
* @par Dependencies
*     SDCC Hw and EFS needs to be initialised 
*
* @retval
*   int32 next_avail_index
* 
* @par Side Effects
*   None
* 
*/
static uint32 find_next_available_index( void )
{
  uint32 next_avail_index = 0;
  uint32 i,str_size = 0;
  EFSDIR *dir_ptr = NULL; 
 
  for (i = 1; i <= MAXDUMPS; i++)
  {
    qsnprintf(foldernumber, MAX_SIZE_FOLDER_STRING, "%lu/", (uintnt)i);
    
    str_size = strlcpy(foldername, cookiefilepath, MAX_FILE_NAME);
    BL_VERIFY((str_size < MAX_FILE_NAME), BL_ERR_INVALID_FILE_NAME|BL_ERROR_GROUP_BOOT ); 
  
    str_size = strlcat (foldername, foldernumber, MAX_FILE_NAME);
    BL_VERIFY((str_size < MAX_FILE_NAME), BL_ERR_INVALID_FILE_NAME|BL_ERROR_GROUP_BOOT ); 
  
    /* Attempt to open integer digit named folder to determine 
       current presence.  Break's are placed such that if efs 
       error occurs, we stop attempting to create a folder, and 
       error value is returned. */
    dir_ptr = boot_efs_opendir (foldername);
        
    /* If the folder doesn't exist, create it */
    if (dir_ptr == NULL)
    {
      /* 0666:Provide read/write permissions for all users */
      if (boot_efs_mkdir(foldername, 0666) == 0)
      {
        next_avail_index = i;
      }
      break;
    }
    /* Else, if it already exists, close it and check the next */
    else 
    {
      if (boot_efs_closedir (dir_ptr) != 0)
      {
        break;
      }
    }
  }
  return next_avail_index;
}


/*===========================================================================

**  Function :  boot_dump_logdump_partition

** ==========================================================================
*/
/*!
*
* @brief
*   This routine to dump the logdump partition into sd card (FR37098)
*
* @param[in]
*   none
*
* @par Dependencies:
*
* @retval
*   none
*
* @par Side Effects
*   none
*
*/

 void boot_dump_logdump_partition( void )
{
	
	int32  fs_handle =  INVALID_FILE_HANDLE;
	boot_flash_trans_if_type * trans_if = NULL;
	uint64 memory_transfer_size =0;      
	uint64 memory_transfer_size_left=0; 
	uint64 memory_transfer_offset =0; 
   	char filename[MAX_FILE_NAME]={'\0'};
	strlcpy(filename, foldername, MAX_FILE_NAME);
	strlcat(filename, "LOGDUMP.bin", MAX_FILE_NAME);

	/* First try to find the ram dump parition */
	boot_flash_configure_target_image(log_dump_partition_id);
	trans_if = boot_flash_dev_open_image(GEN_IMG);

	/* If this partition exists */
	if(trans_if != NULL)
	{
		memory_transfer_size_left = boot_coldplug_get_partition_size_by_image_id(GEN_IMG);
		
		fs_handle =	boot_efs_open(filename, O_WRONLY|O_CREAT|O_TRUNC,0666);
		boot_efs_lseek(fs_handle,0,SEEK_SET);
					
		while ((memory_transfer_size_left != 0) && (INVALID_FILE_HANDLE != fs_handle))
		{
			if (memory_transfer_size_left > MAX_BUF_SIZE)
			{
				memory_transfer_size = MAX_BUF_SIZE;
			}
			else
			{
				memory_transfer_size = memory_transfer_size_left;
			}
					
			boot_set_entry_as_img_whitelist((uint64)memory_buffer , (uint64)memory_transfer_size);

			boot_flash_trans_read(trans_if,	(void *)memory_buffer,memory_transfer_offset,memory_transfer_size,IMAGE_BODY_TYPE);

			boot_invalidate_img_whitelist();
		
			boot_efs_write_file(fs_handle, (void *) (uintnt)(memory_buffer), memory_transfer_size);	
			boot_efs_lseek(fs_handle,0,SEEK_END);
				
			memory_transfer_offset += memory_transfer_size;
			memory_transfer_size_left -= memory_transfer_size;
					
		}
		 
		if(INVALID_FILE_HANDLE != fs_handle)
		{	 
			 boot_efs_close(fs_handle);
		}	 
		 boot_flash_dev_close_image(&trans_if);
		
	}
}


/*===========================================================================

**  Function :  boot_process_sd_dumps

** ==========================================================================
*/
/*!
* 
* @brief  
*    This routine reads the memory regions and writes to the respective files
*   in the SD card.  It checks the SD card for space availability first, and 
*   without deleting any previous dumps, saves the new dumps under a new, 
*   numerically indexed and named folder.  
*
* @par Dependencies
*     SDCC Hw and EFS needs to be initialised 
*
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
static void boot_process_sd_dumps( void )
{

  int32  fs_handle =  INVALID_FILE_HANDLE;
  int32  errmax_fs_handle = INVALID_FILE_HANDLE;
  int32  err_fs_handle = INVALID_FILE_HANDLE;
  int32  sd_dump_header_handle = INVALID_FILE_HANDLE;
  
  uint32 memregion_index, error_info_size, str_size;
  uint64 baseaddr, length;
  uint32 cur_dump_index = 0;

  char * debug_filename = NULL;
  
  /*ensure string arrays delimited correctly */
  filename[(MAX_FILE_NAME-1)]='\0';
  errmax_filename[(MAX_FILE_NAME-1)]='\0';
  err_filename[(MAX_FILE_NAME-1)]='\0';
  foldername[(MAX_FILE_NAME-1)]='\0';
  cookiefilepath[(MAX_FILE_NAME-1)]='\0';
  
  /*Get number of entries in mem_debug_info table */
  max_region_entries = dload_mem_debug_num_ent();
  
  /* Make sure the number of sections we need to dump doesn't exceed
     the max we support */
  BL_VERIFY((max_region_entries <= MAX_CRASHDUMP_TABLE_ENTRIES)
              ,BL_ERR_RAM_DUMP_FAIL|BL_ERROR_GROUP_BOOT);
  
  str_size = strlcpy (errmax_filename, cookiefilepath, MAX_FILE_NAME);
  BL_VERIFY((str_size < MAX_FILE_NAME), BL_ERR_INVALID_FILE_NAME|BL_ERROR_GROUP_BOOT ); 

  str_size = strlcat (errmax_filename, "errmax.txt", MAX_FILE_NAME);
  BL_VERIFY((str_size < MAX_FILE_NAME), BL_ERR_INVALID_FILE_NAME|BL_ERROR_GROUP_BOOT ); 
  
  /* Create Errmax log file ,if no errors occured this file would be deleted */
  /* 0666:Provide write/execute permissions for User,group and others */
  errmax_fs_handle = boot_efs_open(errmax_filename, O_WRONLY|O_CREAT|O_TRUNC,0666);
  
  cur_dump_index = find_next_available_index();

  /* Initialize the LED handling logic */
  boot_toggle_led_init();
  
  /* if folder could not be created, or maximum number of dumps has been met, just stop */
  if (cur_dump_index == 0)
  {
    boot_efs_close(errmax_fs_handle);
    return;
  }
  else
  {
    boot_efs_close(errmax_fs_handle);
    boot_efs_unlink(errmax_filename);
  }

  str_size = strlcpy(err_filename, foldername, MAX_FILE_NAME);
  BL_VERIFY((str_size < MAX_FILE_NAME), BL_ERR_INVALID_FILE_NAME|BL_ERROR_GROUP_BOOT ); 

  str_size = strlcat(err_filename, "errfile.txt", MAX_FILE_NAME);
  BL_VERIFY((str_size < MAX_FILE_NAME), BL_ERR_INVALID_FILE_NAME|BL_ERROR_GROUP_BOOT ); 

  /* Create Error log file ,if no errors occured during ramdumps this file would be deleted */
  /* 0666:Provide write/execute permissions for User,group and others */
  err_fs_handle = boot_efs_open(err_filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
  if(err_fs_handle == INVALID_FILE_HANDLE)
  {
    return;
  }
  
  do
  {
    /*Open ram dump header file */
    qsnprintf(filename, MAX_FILE_NAME, "%s%s", foldername, SD_DUMP_HEADER_NAME);
    
    sd_dump_header_handle = boot_efs_open (filename, O_WRONLY|O_CREAT|O_TRUNC,0666);
    
    if(sd_dump_header_handle == INVALID_FILE_HANDLE)
    {
      /* log error file name */
      BL_VERIFY((qsnprintf(filename, MAX_FILE_NAME, "%s",SD_DUMP_HEADER_NAME) 
                < MAX_FILE_NAME), 
              BL_ERR_INVALID_FILE_NAME|BL_ERROR_GROUP_BOOT);
      break;         
    }
    
    /* Initialize dump header */
    boot_ram_dump_header_init();
    
    /* write a fresh copy of the headers */
    boot_write_ram_dump_headers(sd_dump_header_handle);  
    if(boot_sd_rd_errdata.err_no != EFS_NO_ERR)
    {
      /* log error file name */
      BL_VERIFY((qsnprintf(filename, MAX_FILE_NAME, "%s",SD_DUMP_HEADER_NAME) 
                < MAX_FILE_NAME), 
              BL_ERR_INVALID_FILE_NAME|BL_ERROR_GROUP_BOOT);
      break;
    }  
    /* Iterate over the mem_debug_info table and dump all the memory regions to sd card. */
    for(memregion_index = 0; memregion_index < max_region_entries; memregion_index++)
    {
      baseaddr = dload_mem_debug_mem_base( memregion_index );
      length = dload_mem_debug_mem_length( memregion_index );
      
      boot_sd_rd_errdata.err_no = EFS_NO_ERR;
      
      /* update section header info for this dump section */
      boot_update_section_header_table_by_index(memregion_index);
      /* write a fresh copy of the headers */
      boot_write_ram_dump_headers(sd_dump_header_handle);
      if(boot_sd_rd_errdata.err_no != EFS_NO_ERR)
      {
        /* log error file name */
         BL_VERIFY((qsnprintf(filename, MAX_FILE_NAME, "%s",SD_DUMP_HEADER_NAME) 
                  < MAX_FILE_NAME), 
                  BL_ERR_INVALID_FILE_NAME|BL_ERROR_GROUP_BOOT);
        break;
      }
      
      str_size = strlcpy(filename, foldername, MAX_FILE_NAME);
      BL_VERIFY((str_size < MAX_FILE_NAME),
                BL_ERR_INVALID_FILE_NAME|BL_ERROR_GROUP_BOOT ); 


      debug_filename = dload_mem_debug_filename(memregion_index);
      BL_VERIFY(debug_filename,
                BL_ERR_INVALID_FILE_NAME|BL_ERROR_GROUP_BOOT);

      str_size = strlcat(filename, debug_filename, MAX_FILE_NAME);
      BL_VERIFY((str_size < MAX_FILE_NAME),
                BL_ERR_INVALID_FILE_NAME|BL_ERROR_GROUP_BOOT ); 

      /* 0666:Provide write/execute permissions for User,group and others */
      fs_handle = boot_efs_open (filename, O_WRONLY|O_CREAT|O_TRUNC,0666);
      boot_sd_rd_errdata.err_no = EFS_NO_ERR;
      
      if(INVALID_FILE_HANDLE != fs_handle)
      {
        /* If the Base address of the Ram address is '0'  copy  512Bytes into local buffer 
          and pass the  local buffer to the EFS to write into SD card. EFS can't handle data 
          writes for Null Pointer */
        if(baseaddr == 0)
        {
          /* Copy the initial 512Bytes (SDCC Sector size)  in temporary buffer */
          qmemcpy(sector_buf, (void *)(uintnt)baseaddr, SECTOR_SIZE);
          
          /* Flush temp buffer to file  */
          boot_efs_write_file(fs_handle, sector_buf, SECTOR_SIZE);
          
          baseaddr += SECTOR_SIZE;
          length -=SECTOR_SIZE;
        }
        
        /* Dump the rest of this memory region data to same file */
        if(boot_sd_rd_errdata.err_no == EFS_NO_ERR)
        {
          boot_efs_write_file(fs_handle, (void *) (uintnt)(baseaddr), length);
        }
        
        if(boot_sd_rd_errdata.err_no != EFS_NO_ERR)
        {
          /*if there is a writte error, delete the partial file */
          boot_efs_close(fs_handle);
          boot_efs_unlink(filename);
          break;
        }
  
        /* Now current dump region has been written to card */
        /* update the validity flag of the section header */ 
        /* Update the actual size and section count we have dumped in overall header */
        boot_efs_close(fs_handle);  
        raw_dump_section_header_table[memregion_index].validity_flag |= RAM_DUMP_VALID_MASK;       
        raw_dump_header.dump_size += length;
        raw_dump_header.sections_count++;
        
        /* write a fresh copy of the headers */
        boot_write_ram_dump_headers(sd_dump_header_handle);
        if(boot_sd_rd_errdata.err_no != EFS_NO_ERR)
        {
          /* log error file name */
          BL_VERIFY((qsnprintf(filename, MAX_FILE_NAME, "%s",SD_DUMP_HEADER_NAME) 
                    < MAX_FILE_NAME), 
                    BL_ERR_INVALID_FILE_NAME|BL_ERROR_GROUP_BOOT);  
          break;
        }
      } /* file handle valid */
      else
      {
        /*we can not open the dump file,terminate the dump */
        break;
      }  
    }
  }while(0);
  /*At the end of dump loop we check for error */
  if(boot_sd_rd_errdata.err_no != EFS_NO_ERR)
  {
    boot_enable_led(RAM_DUMP_LED, TRUE);
    /*generate error file name + newline */
    str_size = qsnprintf(boot_sd_rd_errdata.filename, MAX_FILE_NAME, "%s\n", filename);
    BL_VERIFY((str_size < MAX_FILE_NAME), BL_ERR_INVALID_FILE_NAME|BL_ERROR_GROUP_BOOT );
  
    error_info_size = sizeof(boot_sd_rd_errdata.err_no) + str_size;
    
    /* write the error number and error file name to error log */
    if(err_fs_handle != INVALID_FILE_HANDLE)
    {
      boot_efs_write_file(err_fs_handle, (void *)&boot_sd_rd_errdata, error_info_size);
      boot_efs_close(err_fs_handle);
    }
  }
  else
  {
    /* If no error occured during the Ramdump delete the error file */
    boot_enable_led(RAM_DUMP_LED, FALSE);
    boot_efs_close(err_fs_handle);
    boot_efs_unlink(err_filename);
    
    /* set dump header to valid */
    raw_dump_header.validity_flag |= RAM_DUMP_VALID_MASK;
    /* write a fresh copy of the headers */
    boot_write_ram_dump_headers(sd_dump_header_handle);        
  }
  
  /*close the header handle in the end */
  boot_efs_close(sd_dump_header_handle);
  
} /* boot_process_sd_dumps */

/*===========================================================================

**  Function :  boot_ram_dump_check_rdcookie

** ==========================================================================
*/
/*!
*
* @brief
*   This routine checks whether file rdcookie.txt exists and records
* the directory path to cookiefilepath.
*
* @param[in]
*   path Pointer to the searching directory.
*
* @par Dependencies:
*
* @retval
*   int Handler for the rdcookie.txt file.
*
* @par Side Effects
*   Set static variable cookiefilepath.
*
*/
static int boot_ram_dump_check_rdcookie(const char *path)
{
    uint32 str_size = 0;
    int fd;

    str_size = strlcpy(cookiefilepath, path, MAX_FILE_NAME);
    BL_VERIFY((str_size < MAX_FILE_NAME), BL_ERR_INVALID_FILE_NAME|BL_ERROR_GROUP_BOOT);

    str_size = strlcpy(cookiefilename, path, MAX_FILE_NAME);
    BL_VERIFY((str_size < MAX_FILE_NAME), BL_ERR_INVALID_FILE_NAME|BL_ERROR_GROUP_BOOT);

    str_size = strlcat(cookiefilename, "rdcookie.txt", MAX_FILE_NAME);
    BL_VERIFY((str_size < MAX_FILE_NAME), BL_ERR_INVALID_FILE_NAME|BL_ERROR_GROUP_BOOT);

    fd = boot_efs_open(cookiefilename, O_RDONLY);

    if (fd >= 0)
    {
        boot_efs_close(fd);
    }

    return fd;
}

/*===========================================================================

**  Function :  boot_ram_dumps_to_sd_card

** ==========================================================================
*/
/*!
* 
* @brief :  This routine initiates the Ramdumps to SD card. Checks for the Ramdump 
* cookie file and go ahead with Ramdumps to SD card if cookie file is present.
* Setup the Ram regions to be taken as dumps.
*
* @param[in]
*   None
* 
* @par Dependencies:
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void boot_ram_dumps_to_sd_card(void)
{
  char mount_path[] = "/mmc1";
  /* dummy while loop which executes only once and used to break on error */
  do
  {
    /* Poll the coldplug device */
    boot_efs_boot_mount_first_fat(COLDPLUG_DEVICE_SD, 0, mount_path);

    /* Return if cookie file doesn't exit.*/
    if((boot_ram_dump_check_rdcookie(SD_RAM_DUMP_PATH) < 0)  &&
       (boot_ram_dump_check_rdcookie(SD_PATH) < 0))
        break;

#ifdef FEATURE_DLOAD_MEM_DEBUG
    /*only perform memory debug operations when it's supported*/
    if(dload_mem_debug_supported())
    {
    /* Initialize the debug memory regions array */
      dload_mem_debug_init();
    }
#endif
    
    /*Store the ramdumps to sd card without deletion*/
    boot_process_sd_dumps();
	
	/*Stores the logdump partition*/
	boot_dump_logdump_partition();

    /* reset target if reset cookie present */
    boot_ramdump_reset();

  } while(0);
  
  /* umount the coldplug device after operation done */
  boot_efs_umount(mount_path);  
  
  /* At the end of sd card ram dump, if we are in raw ram dump mode, reset the device
     so we don't enter sahara */
  if((boot_shared_imem_cookie_ptr != NULL) &&
     (boot_shared_imem_cookie_ptr->uefi_ram_dump_magic == BOOT_RAW_RAM_DUMP_MAGIC_NUM))
  {
      mmu_flush_cache();
      boot_hw_reset(BOOT_WARM_RESET_TYPE);
  }
  
}/* boot_ram_dumps_to_sd_card*/



