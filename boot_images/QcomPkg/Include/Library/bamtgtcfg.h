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


when       who     what, where, why
--------   ---     ------------------------------------------------------------
02/11/14   RL      64-bit support
04/01/13   SS      Added force init option support.
02/05/13   SS      Added Multi VMID support for pipe access control.
01/14/13   SS      Added Multi VMID support for BAM ctrl group
02/08/12   NK      Added shared bam support
04/16/11   MK      Created

===============================================================================
                   Copyright (c) 2011-2014 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "DALStdDef.h"

/**@name Target Config Flags 
   Various options in a bitmask format to configure the BAM
   @{ */

#define BAM_TGT_CFG_SECURE             0x1 /**< Whether to do secure config or not */
#define BAM_TGT_CFG_NO_INIT            0x2 /**< use to skip BAM initialization for EE0 */
#define BAM_TGT_CFG_SHARABLE           0x4 /**< Whether the bam can have multiple clients or not */ 
#define BAM_TGT_CFG_LCK_PGRPONLY       0x8 /**< Lock pipes not in pipe group only, don't consider EEs */
#define BAM_TGT_CFG_LAZYINIT           0x10 /**< Whether to do a lazy init (initialize bare minimum) */ 
#define BAM_TGT_CFG_DISABLE_CLK_GATING 0x20 /**< Disable local clock gating for BAM device */
#define BAM_TGT_CFG_SW_DEBUG_INIT      0x40 /**< Initalize bam as pure SW init for debug use only */
#define BAM_TGT_CFG_FORCEINIT          0x80 /**< Initialize the BAM even if EE index is not EE0.Used in SBL boot*/

/** @} */ /* end_namegroup */

/** Maximum number of execution environments supported by a BAM */
#define BAM_MAX_EES 8

/** Maximum number of supergroups supported by a BAM */
#define BAM_MAX_SGS 32

/** Maximum size of BAM memory-mapped region */
#define BAM_MAX_MMAP 0x40000

/** A non-existent EE that is used to indicate
    that only secure config is to be done
  */
#define BAM_EE_TRUST 0xFF 

/* last entry marker for target_config array */
#define BAM_TGT_CFG_LAST    0x0

/** BAM device control group access mask   */
#define BAM_SEC_CTRL_MASK 0x80000000
/** BAM device control group VMID support Macro */
#define TZBSP_CTRL_VMID_INVALID  0xFFFFFFFF 
/** Section to place specific functions in non-paged IMEM memory */
#ifdef BAM_API_NOPAGED_IMEM
#define BAM_API_NON_PAGED   __attribute__((section("NON_PAGED")))
#else
#define BAM_API_NON_PAGED                  
#endif 
/** @brief Security Configuration for a EE
 *  
 *  This struct defines a BAM device's security configuration for a given EE
 *  vmid_ac - this vmid bit mask provide BAM pipe access to BAM
 *  device.
 *  pipe_mask_ac - pipemask for whom BAM device need pipe access
 *  control.
 *  
 *  
 *  @note This configuration is usually performed by the SROT at boot,
 *  instead of the driver load time.
 */
typedef struct
{
   uint32 pipe_mask; /**< -- Bitmask indicating pipes allocated to this EE */
   uint32 vmid; /**< -- VMID allocated for pipes transaction & access control */
   uint32 pipe_mask_ac; /**< Bitmask indicating pipes allocated for access contol*/
   uint32 vmid_ac; /**< -- VMID allocated for pipe access control*/
} bam_ee_sec_config_type;

/** @brief Security Configuration for a SG
 *  
 *  This struct defines a BAM device's security configuration for a given SG
 *  
 *  @note This configuration is usually performed by the SROT at boot,
 *  instead of the driver load time.
 */
typedef struct
{
   uint32 pipe_mask; /**< -- Bitmask indicating pipes allocated to this SG */
} bam_sg_sec_config_type;

/** @brief Security Configuration for a BAM
 *  
 *  This struct defines a BAM device's security configuation for all EEs.
 *  
 *  @note This configuration is usually performed by the SROT at boot,
 *  instead of the driver load time.
 */

typedef struct
{
   bam_ee_sec_config_type ee[BAM_MAX_EES]; /**< -- Security configuration of 
                                                   all EEs */
   bam_sg_sec_config_type sg[BAM_MAX_SGS]; /**< -- Security configuration of 
                                                   all SGs */
   uint32 ctrl_vmid;  /**< -- VMID for control and debug group */
   uint32 bam_irq_ee;  /**< -- Top level intruppt will routed to this EE */
} bam_sec_config_type;

/** @brief Bam Configuration
    
    Provides default Bam configuration.
 */
typedef struct _bamtgtcfg
{
    uint64 bam_pa;                   /**< -- Physical address of the BAM */
    uint32 options;                  /**< -- Options to configure; see BAM_TGT_CFG */
    uint32 cfg_bits;                 /**< -- BAM chicken bits */
    uint8  ee;                       /**< -- Local execution environment index */
    bam_sec_config_type *sec_config; /**< -- Security config for the BAM */
    uint32 size;                     /**< -- Size of the register space */
}bam_target_config_type;


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
const bam_target_config_type* bam_tgt_getcfg(uint64 bam_pa);

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
uint32 bam_tgt_getpipelockgroup(uint64 bam_pa, uint32 pipe_num);

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
uint64 bam_tgt_getpipememstartaddr(void);

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
uint64 bam_tgt_getpipememendaddr(void);

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