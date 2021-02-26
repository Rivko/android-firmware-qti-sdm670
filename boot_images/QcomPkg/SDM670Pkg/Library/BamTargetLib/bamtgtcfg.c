/**
  @file bamtgtcfg.c
  @brief
  This file contains implementation of the interfaces to access
  target specific configuration data of the BAM driver.

*/
/*
===============================================================================

                             Edit History


when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/28/16   ss      Added tgt config support
02/11/14   RL      64-bit support
12/04/12   SS      Added dynamic bam_num_configs & tgt_cgf support for binary compatiblility
08/23/12   NK      Add dalconfig support 
06/29/12   RL      Add implementation of pipe memory related api
02/13/12   NK      Added lazy init to a2 bam so that driver can initialize regardless of hw state
04/15/11   MK      Created

===============================================================================
                   Copyright (c) 2011-2016 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "com_dtypes.h"
#include "bam.h"
#include "bamtgtcfg.h"
const bam_target_config_type *bam_tgt_config_ptr = NULL;
#define MAX_BAM_CONFIG    0xFF
uint32 bam_num_configs = 0;

/* ============================================================================
**  Function : bam_tgt_getcfg
** ============================================================================
*/
/**
  Gets the target specific configuration for the bam.
    
  Provides the target specific configuration for the BAM identified by its
  physical address.
    
  @param[in]  bam_pa    Physical address

  @return
  Returns the target specific data structure.
  
*/
BAM_API_NON_PAGED const bam_target_config_type* bam_tgt_getcfg(uint64 bam_pa)
{
    uint32 index = 0;
    if (bam_tgt_config_ptr == NULL) 
        return NULL;
    for (index = 0;((bam_tgt_config_ptr[index].bam_pa != BAM_TGT_CFG_LAST)&&(index < MAX_BAM_CONFIG)); index++)
    {
        if (bam_tgt_config_ptr[index].bam_pa == bam_pa) 
            return &bam_tgt_config_ptr[index];
    }
    return NULL;
}

/* ============================================================================
**  Function : bam_tgt_getnumdevs
** ============================================================================
*/
/**
  Returns the number of BAM Devices supported.
  
  @param  None

  @return
    Number of BAM Devices supported.
  
*/
uint32 bam_tgt_getnumdevs(void)
{
    if(0 == bam_num_configs)
    {
        /* Compute the number of configs if its loaded from a property file otherwise return it  */
        if (bam_tgt_config_ptr == NULL)
            return bam_num_configs ;
        /* Compute the number of configs  */
        while((bam_tgt_config_ptr[bam_num_configs].bam_pa != BAM_TGT_CFG_LAST )&&(bam_num_configs < MAX_BAM_CONFIG))
            bam_num_configs++;

        if (bam_num_configs >= MAX_BAM_CONFIG)
            bam_num_configs = 0x0;
    }
    return bam_num_configs;
}

/* ============================================================================
**  Function : bam_tgt_getpipelockgroup
** ============================================================================
*/
/**
  Gets the pipe locking group from the target pipelock config.
    
  Gets the pipe locking group from the target pipelock config.
    
  @param[in]   bam_pa             BAM Physical base address 
  @param[in]   pipe_num           Index of the pipe        

  @return
  Group # to which the pipe belongs to.
  
  @note
  This API is supported in Cores v0x6 (BAM), 0x13(BAMLite), 0x20(NDPBAM) and
  above.
  
*/
uint32 bam_tgt_getpipelockgroup(uint64 bam_pa, uint32 pipe_num)
{
    (void)bam_pa;
    (void)pipe_num;
    return 0;
}

/* ============================================================================
**  Function : bam_tgt_initpipemem
** ============================================================================
*/
/**
  Initialize pipe memory.
    
  Initalize the pipe memory partition table.
  It will happen only if executed in trusted domain.
    
  @return
  none.
  
  @dependencies
  Pipe memory must be clocked.
  
*/
void bam_tgt_initpipemem()
{
    return;
}

/* ============================================================================
**  Function : bam_tgt_getpipememstartaddr
** ============================================================================
*/
/**
  Gets the pipe memory start address.
    
  Gets the pipe memory start address.
    
  @return
  start address of pipe memory.
  0 if not available
  
 
*/
uint64 bam_tgt_getpipememstartaddr(void)
{
    return 0;
}

/* ============================================================================
**  Function : bam_tgt_getpipememendaddr
** ============================================================================
*/
/**
  Gets the pipe memory end address.
    
  Gets the pipe memory end address.
    
  @return
  end address of pipe memory.
  0 if not available
  
 
*/
uint64 bam_tgt_getpipememendaddr(void)
{
    return 0;
}

/* ============================================================================
**  Function : bam_tgt_getpipememptaddr
** ============================================================================
*/
/**
  Gets the pipe memory partition table address.
    
  Gets the pipe memory partition table address.
    
  @return
  start address of pipe memory partition table.
  
  
*/
void* bam_tgt_getpipememptaddr()
{
    return NULL;
}
