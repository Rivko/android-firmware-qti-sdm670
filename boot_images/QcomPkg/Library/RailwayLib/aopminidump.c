/*===========================================================================
                             aopminidump.c

DESCRIPTION:
  

Copyright (c) 2018 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

//===========================================================================
//                      Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
//  Include Files
//---------------------------------------------------------------------------

#include <string.h>
#include <stdlib.h>
#include "CoreVerify.h"
#include "boot_minidump.h"
#include "stringl/stringl.h"
#include "LoaderUtils.h"
#include "smem.h"
#include "CoreVerify.h"

#define AOP_MD_SS_TOC_MAGIC               (0xDEEDDEED)
#define NUM_OF_REGIONS_COUNT 			  18
#define AOP_MINIDUMP_SMEM_ID 			  614
#define MD_SS_SLPI						  6  // Using SLPI region for AOP regions.

//---------------------------------------------------------------------------
//  Internal Type Definitions
//---------------------------------------------------------------------------

const md_ss_region aop_md_ss_regions[NUM_OF_REGIONS_COUNT] =
{
    {
    /* MSG_RAM */
      "MSGRAM0",
      0,                    
      MD_REGION_VALID,      
      0x0C300000,                
      0x400,
  },
      {
    /* MSG_RAM */
      "MSGRAM",
      1,                    
      MD_REGION_VALID,      
      0x0C310000,                
      0x400,
  },
      {
    /* MSG_RAM */
      "MSGRAM",
      2,                    
      MD_REGION_VALID,      
      0x0C320000,                
      0x400,
  },
      {
    /* MSG_RAM */
      "MSGRAM",
      3,                    
      MD_REGION_VALID,      
      0x0C330000,                
      0x400,
  },
      {
    /* MSG_RAM */
      "MSGRAM",
      4,                    
      MD_REGION_VALID,      
      0x0C340000,                
      0x400,
  },
      {
    /* MSG_RAM */
      "MSGRAM",
      5,                    
      MD_REGION_VALID,      
      0x0C350000,                
      0x400,
  },
      {
    /* MSG_RAM */
      "MSGRAM",
      6,                    
      MD_REGION_VALID,      
      0x0C360000,                
      0x400,
  },
      {
    /* MSG_RAM */
      "MSGRAM",
      7,                    
      MD_REGION_VALID,      
      0x0C370000,                
      0x400,
  },
      {
    /* MSG_RAM */
      "MSGRAM",
      8,                    
      MD_REGION_VALID,      
      0x0C380000,                
      0x400,
  },
      {
    /* MSG_RAM */
      "MSGRAM",
      9,                    
      MD_REGION_VALID,      
      0x0C390000,                
      0x400,
  },
      {
    /* MSG_RAM */
      "MSGRAM",
      10,                    
      MD_REGION_VALID,      
      0x0C3A0000,                
      0x400,
  },
      {
    /* MSG_RAM */
      "MSGRAM",
      11,                    
      MD_REGION_VALID,      
      0x0C3B0000,                
      0x400,
  },
      {
    /* MSG_RAM */
      "MSGRAM",
      12,                    
      MD_REGION_VALID,      
      0x0C3C0000,                
      0x400,
  },
      {
    /* MSG_RAM */
      "MSGRAM",
      13,                    
      MD_REGION_VALID,      
      0x0C3D0000,                
      0x400,
  },
      {
    /* MSG_RAM */
      "MSGRAM",
      14,                    
      MD_REGION_VALID,      
      0x0C3E0000,                
      0x400,
  },
    {
    /* MSG_RAM */
      "MSGRAM",
      15,                    
      MD_REGION_VALID,      
      0x0C3F0000,                
      0x400,
  },
  {
    /* CODERAM */
      "CODERAM",
      0,                    
      MD_REGION_VALID,      
      0xB000000,                
      0x18000,
  },
    {
    /* DATARAM */
      "DATARAM",
      0,                    
      MD_REGION_VALID,      
      0x0B0E0000,                
      0x8000,
  }
};

void aop_mini_dump_init(void)
{
   md_global_toc *md_gtoc = NULL;
   md_ss_toc *ss_toc =  NULL;
   uint64 smem_global_addr;
   uint64 *aop_smem_base = NULL;
   smem_global_addr = (*((uint32* )MD_SHARED_IMEM_BASE));
   aop_smem_base = (uint64 *)smem_alloc( AOP_MINIDUMP_SMEM_ID, sizeof(md_ss_region)* NUM_OF_REGIONS_COUNT);
   CORE_VERIFY(aop_smem_base != NULL);
   memscpy(aop_smem_base, sizeof(md_ss_region)*NUM_OF_REGIONS_COUNT, &aop_md_ss_regions, sizeof(md_ss_region)*NUM_OF_REGIONS_COUNT);
    
   md_gtoc = (md_global_toc*)smem_global_addr;
   
   if (md_gtoc->md_toc_init != TRUE)
		CORE_VERIFY(0);
	
   ss_toc = &md_gtoc->md_ss_toc[MD_SS_SLPI];
   ss_toc->md_ss_toc_init             = AOP_MD_SS_TOC_MAGIC;
   ss_toc->md_ss_enable_status        = MD_SS_ENABLED;
   /*qsee md blobs will be encrypted in SDI pass#1.*/
   ss_toc->encryption_required        = MD_SS_ENCR_REQ;
   ss_toc->encryption_status          = MD_SS_ENCR_START;
   ss_toc->ss_region_count            = NUM_OF_REGIONS_COUNT;
   ss_toc->md_ss_smem_regions_baseptr = (uint64)aop_smem_base;
   return;
}
