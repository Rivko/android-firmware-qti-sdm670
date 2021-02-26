/*===========================================================================

                              Minidump table parser

GENERAL DESCRIPTION
  This module scans the SMEM location to read address/size pair and populates 
  them it to memory dump table  

Copyright 2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who        what, where, why
--------   ---        -------------------------------------------------------
12/12/17   aus     Updated structures as packed
11/02/17   aus     Added support for seq_num append
10/12/17   aus     Updated functions and structures as per design
02/02/17   bd      Call smem_init for minidump case
01/17/17   bd      Initial version

=============================================================================*/


/*==========================================================================

                   INCLUDE FILES

===========================================================================*/
#include "boot_minidump.h"
#include "boot_dload_debug.h"
#include "boot_shared_imem_cookie.h"
#include "boot_error_if.h"
#include "boot_shared_functions.h"
#include "smem.h"
#include "smem_type.h"
#include "LoaderUtils.h"
#include "boot_logger.h"
#include "boot_ddr_info.h"
#include "cil_ramdump.h"
#include "cil_loader.h"

static md_global_toc  *md_global_toc_smem_base = NULL;
static uint64 ddr_end_addr = 0;

/*=============================================================================

        LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

void dload_debug_encr_all_regions(uint32 start_index, uint32 num_regions)
{
  uint32 i = 0;
  uint64 r_base, r_len ;
   
  for(i = start_index; i < num_regions; i++)
  {    
    r_base = dload_mem_debug_mem_base(i);
    r_len = dload_mem_debug_mem_length(i);   
    cil_md_encrypt_region(r_base, r_len, i);
  } 
}

void dload_debug_crypto_init(void)
{
  cil_md_crypto_init();
}

void dload_debug_add_unencr_regions(void)
{
  cil_md_add_unencr_regions();
}

void dload_debug_check_oem_key(uint32 start_index, uint32 num_regions)
{
  int i ;
  uint64 r_base, r_len ;
  
  if(!boot_check_oem_key())
  {
    // if OEM key is absent zero out all regions
    for(i = start_index; i < num_regions; i++)
    {
      r_base = dload_mem_debug_mem_base(i);
      r_len = dload_mem_debug_mem_length(i);      
      memset((void*)r_base, 0, r_len);
    } 
  }
}

/*===========================================================================

FUNCTION add_one_region_to_dump

DESCRIPTION
  This function adds a region  dump table 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void itoa(char *p, uint32 x)
{
   uint32 rem, j, i = 0;
   char temp;
	
    // Process individual digits
    while (x != 0)
    {
        rem = x % 10;
        p[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        x = x/10;
    }	

	// Append string terminator	
    p[i] = '\0'; 
	
    // Reverse the string
    j = i - 1;
    i = 0; 
    while (i < j) {
      temp = p[i];
      p[i] = p[j];
      p[j] = temp;
      i++;
      j--;
    }	
}

static void add_one_region_to_dump(md_ss_region *r)
{
  char dump_region_name[DLOAD_DEBUG_STRLEN_BYTES] = "md_";
  char dump_region_bin_name[DLOAD_DEBUG_STRLEN_BYTES];
  char seq_num[5] = {0};
  
  strlcat(dump_region_name, (const char *)r->region_name, MD_REGION_NAME_LENGTH);
  
  if((r->seq_num > 0) && (r->seq_num < 10000)) 
  {
	itoa(seq_num, r->seq_num); 
	strlcat(dump_region_name, (const char *)seq_num, MD_REGION_NAME_LENGTH);	  
  }
  
  strlcpy(dump_region_bin_name, dump_region_name, DLOAD_DEBUG_STRLEN_BYTES); 
  strlcat(dump_region_bin_name, ".BIN", DLOAD_DEBUG_STRLEN_BYTES);
  
  dload_debug_add_region(OPTIONAL_DEF_SAVE,
          (r->region_base_address), (r->region_size),
          dump_region_name, dump_region_bin_name);
}

/*===========================================================================

FUNCTION md_get_smem_base_address

DESCRIPTION
  This function initializes the smem base address for minidump region 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void md_get_smem_base_address(void)
{
  uint32 md_smem_size = sizeof(md_global_toc);

  smem_init();
  md_global_toc_smem_base = (md_global_toc *)smem_get_addr( SBL_MINIDUMP_SMEM_ID, &md_smem_size);
}

/*===========================================================================

FUNCTION md_is_valid_smem_entry

DESCRIPTION
  This function does boundary checks for address and size 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean md_is_valid_smem_entry(uint64 base, uint64 size)
{
  if((base > 0) && (base < ddr_end_addr) 
    && (size != 0x0) && ((base+size) < ddr_end_addr))
  {
    return TRUE;
  }

  return FALSE;
}  

/*===========================================================================

FUNCTION add_minidump_regions

DESCRIPTION
  This function adds all minidump regions into dump table

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void add_minidump_regions(void)
{
  uint32 index = 0, count = 0, start_index = 0;
  boot_boolean is_auth_enabled = FALSE;
  md_ss_toc    ss_info;
  md_ss_region *region_info = NULL;
  sbl_if_shared_ddr_device_info_type *available_ddr;
 	
  md_get_smem_base_address();
  
  /* If MD global TOC is not initialized */
  if( NULL == md_global_toc_smem_base )
    return ;

  available_ddr = boot_get_ddr_info();
  ddr_end_addr = available_ddr->ddr_info[0].cs_addr;

  while(index < available_ddr->noofddr)
  {
    ddr_end_addr += available_ddr->ddr_info[index].ramsize << CONVERT_TO_MB_SHIFT;    
    index++;
  } 
  
  boot_is_auth_enabled(&is_auth_enabled);
  
  if(is_auth_enabled)  
    dload_debug_crypto_init();
  
  /* Check every SS MD regions and add to the dump table */ 
  for (index = 0; index < MAX_NUM_OF_SS; index++)
  {	  
    ss_info = md_global_toc_smem_base->md_ss_toc[index];
	
	if(ss_info.md_ss_toc_init == 0)
		continue;
		
	/* Check if XBL needs to encrypt the regions */
    if((ss_info.md_ss_enable_status == MD_SS_ENABLED) &&
	   (ss_info.encryption_required == MD_SS_ENCR_REQ))
    {
       region_info = (md_ss_region *)ss_info.md_ss_smem_regions_baseptr;
	   count       = ss_info.ss_region_count;
	   start_index = dload_mem_debug_num_ent();
	   
       while(count > 0)
       {
           if((region_info->md_valid == MD_REGION_VALID) && 
		      md_is_valid_smem_entry(region_info->region_base_address, region_info->region_size) &&
			  (region_info->region_name != NULL))
              add_one_region_to_dump(region_info);
	   
           region_info++;
		   count--;
       }

	   if(is_auth_enabled)
	   {
		   dload_debug_check_oem_key(start_index,dload_mem_debug_num_ent());
		   dload_debug_encr_all_regions(start_index,dload_mem_debug_num_ent());
	   }
	}		
	   
	/* Check if MD is enabled and encryption status is complete */
	else if ((ss_info.md_ss_enable_status == MD_SS_ENABLED) && 
	        ((is_auth_enabled && (ss_info.encryption_status == MD_SS_ENCR_DONE)) || 
	        (!is_auth_enabled))) 
	{	
       region_info = (md_ss_region *)ss_info.md_ss_smem_regions_baseptr;
	   count       = ss_info.ss_region_count; 
       while(count > 0)
       {
           if((region_info->md_valid == MD_REGION_VALID) && 
		      md_is_valid_smem_entry(region_info->region_base_address, region_info->region_size) &&
			  (region_info->region_name != NULL))
              add_one_region_to_dump(region_info);
    
           region_info++;
		   count--;
       }
	}
  }  
}
