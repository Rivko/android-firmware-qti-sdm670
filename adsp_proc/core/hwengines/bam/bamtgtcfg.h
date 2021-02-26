#ifndef BAMTGTCFG_H_
#define BAMTGTCFG_H_

/**
  @file bamtgtcfg.h
  @brief
  This file contains definitions of constants, data structures, and
  interfaces that provide target specific configuration data to the 
  BAM driver.

*/
/*
===============================================================================

                             Edit History

 $Header: //components/rel/core.qdsp6/2.1/hwengines/bam/bamtgtcfg.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/01/13   SS      Added force init option support.
02/05/13   SS      Added Multi VMID support for pipe access control.
01/14/13   SS      Added Multi VMID support for BAM ctrl group
02/08/12   NK      Added shared bam support
04/16/11   MK      Created

===============================================================================
                   Copyright (c) 2011-2016 Qualcomm Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "DALStdDef.h"
#include "bam.h"
#include "bam_targetcfg.h"
/** Section to place specific functions in non-paged IMEM memory */
#ifdef BAM_API_NOPAGED_IMEM
#define BAM_API_NON_PAGED   __attribute__((section("NON_PAGED")))
#else
#define BAM_API_NON_PAGED                  
#endif 


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
const bam_target_config_type* bam_tgt_getcfg(uint32 bam_pa);

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
uint32 bam_tgt_getnumdevs(void);

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
uint32 bam_tgt_getpipelockgroup(uint32 bam_pa, uint32 pipe_num);

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
void bam_tgt_initpipemem(void);

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
uint32 bam_tgt_getpipememstartaddr(void);

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
uint32 bam_tgt_getpipememendaddr(void);

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
void* bam_tgt_getpipememptaddr(void);

#endif /* BAMTGTCFG_H_ */
