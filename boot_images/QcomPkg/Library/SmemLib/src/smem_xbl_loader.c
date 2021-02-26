/*=============================================================================

                          S M E M

 Copyright (c)  2016 Qualcomm Technologies, Inc. All Rights Reserved.
 Qualcomm Technologies, Inc. Confidential and Proprietary.

=============================================================================*/

/*=============================================================================

                         EDIT HISTORY FOR FILE

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/SmemLib/src/smem_xbl_loader.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/11/16   rv      Initial version.
===========================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include <Library/PcdLib.h>
#include "DALSys.h"
#include "smem_os.h"
#include "smem_type.h"
#include "smem_toc.h"
#include "smem_boot_targ.h"

/*=============================================================================

                         MACRO DEFINITIONS

=============================================================================*/


/*=============================================================================

                         TYPE DEFINITIONS

=============================================================================*/


/*=============================================================================

                       PUBLIC/GLOBAL DATA DEFINATIONS

=============================================================================*/


/*=============================================================================

                       LOCAL DATA DEFINATIONS

=============================================================================*/
/* Target configuration data */
static uint8 *smem_boot_targ_smem_base_addr;
static uint32 smem_boot_targ_smem_size;
static uint32 smem_boot_targ_cfg_toc_version;
static uint32 smem_boot_targ_cfg_smem_version;
static const  smem_toc_config_type *smem_boot_targ_cfg_toc_tbl;
static uint16 smem_boot_targ_cfg_max_items;

/*----------------------------------------------------------------------------
 * CORE_TOP_CSR
 *--------------------------------------------------------------------------*/
extern const uint32 smem_targ_hwio_tcsr_base;

/*----------------------------------------------------------------------------
 * TCSR_TCSR_REGS 
 *--------------------------------------------------------------------------*/
extern const uint32 smem_targ_hwio_wonce_reg_offsets[];

/* Wonce register addresses */
static volatile uint32 *smem_targ_wonce_reg_addrs[2];

/*=============================================================================

                      LOCAL FUNCTION DEFINATIONS

=============================================================================*/


/*=============================================================================

                     PUBLIC FUNCTION DEFINATION

=============================================================================*/

/*=============================================================================
  FUNCTION  smem_boot_targ_init
=============================================================================*/
/**
 * @param[in]
 *
 * @return
 *
 * @sideeffects
 */
/*===========================================================================*/
void smem_boot_targ_init(void)
{
  DALSYSPropertyVar prop;
  DALResult         result;
  
  /* The handle is a uint32[2], so it won't be null after declaration. */
  DALSYS_PROPERTY_HANDLE_DECLARE(smem_prop_handle);
  
  result = DALSYS_GetDALPropertyHandleStr("/core/mproc/smem", smem_prop_handle);
  if (result != DAL_SUCCESS)
  {
    SMEM_OS_LOG_FATAL(1, "Cannot get DALProp handle, result=%d.", result);
  }
  
  result = DALSYS_GetPropertyValue(smem_prop_handle, "smem_vers", 0, &prop);
  if (result != DAL_SUCCESS)
  {
    SMEM_OS_LOG_FATAL(1, "Cannot get smem_vers DAL prop, result=%d.", result);
  }
  /* The SMEM TOC version will be put in prop.Val.dwVal. */
  smem_boot_targ_cfg_smem_version = prop.Val.dwVal;
  
  result = DALSYS_GetPropertyValue(smem_prop_handle, "smem_toc_vers", 0, &prop);
  if (result != DAL_SUCCESS)
  {
    SMEM_OS_LOG_FATAL(1, "Cannot get smem_toc_vers DAL prop, result=%d.", result);
  }
  /* The SMEM TOC version will be put in prop.Val.dwVal. */
  smem_boot_targ_cfg_toc_version = prop.Val.dwVal;

  result = DALSYS_GetPropertyValue(smem_prop_handle, "smem_partitions", 0, &prop);
  if (result != DAL_SUCCESS)
  {
    SMEM_OS_LOG_FATAL(1, "Cannot get smem_partitions DAL prop. result=%d.", result);
  }
  
  /* The SMEM partition info array will be put in prop.Val.pStruct 
   * (array of smem_toc_config_type's) */
  smem_boot_targ_cfg_toc_tbl = (const smem_toc_config_type *)prop.Val.pStruct;

  result = DALSYS_GetPropertyValue(smem_prop_handle, "smem_max_items", 0, &prop);
  if (result != DAL_SUCCESS)
  {
    SMEM_OS_LOG_FATAL(1, "Cannot get smem_max_items DAL prop. result=%d.", result);
  }

  /* The SMEM Max items to support */
  smem_boot_targ_cfg_max_items = (uint16)prop.Val.dwVal;
  
  /* Read the SMEM base address and size */
  smem_boot_targ_smem_base_addr = (uint8*)(uintptr_t)(PcdGet64(PcdSmemBaseAddress));
  smem_boot_targ_smem_size      = (uint32)PcdGet64(PcdSmemSize);
  
  /* Update the Wonce registers addresses */
  smem_targ_wonce_reg_addrs[0] = (volatile uint32*)(uintptr_t)
            (smem_targ_hwio_tcsr_base + smem_targ_hwio_wonce_reg_offsets[0]);
            
  smem_targ_wonce_reg_addrs[1] = (volatile uint32*)(uintptr_t)
            (smem_targ_hwio_tcsr_base + smem_targ_hwio_wonce_reg_offsets[1]);
  
}

/*=============================================================================
  FUNCTION  smem_boot_targ_base_addr_get
=============================================================================*/
/**
 * @param[in]
 *
 * @return
 *
 * @sideeffects
 */
/*===========================================================================*/
uint8 *smem_boot_targ_base_addr_get(void)
{
  return smem_boot_targ_smem_base_addr;
}

/*=============================================================================
  FUNCTION  smem_boot_targ_size_get
=============================================================================*/
/**
 * @param[in]
 *
 * @return
 *
 * @sideeffects
 */
/*===========================================================================*/
uint32 smem_boot_targ_size_get(void)
{
  return smem_boot_targ_smem_size;
}

/*===========================================================================
  FUNCTION  smem_boot_targ_version_get
===========================================================================*/
/**

  @return
  None.
*/
/*=========================================================================*/
uint32 smem_boot_targ_version_get(void)
{
  return smem_boot_targ_cfg_smem_version;
}

/*===========================================================================
  FUNCTION  smem_boot_targ_toc_config_tbl_get
===========================================================================*/
/**

  @return
  None.
*/
/*=========================================================================*/
uint32 smem_boot_targ_toc_config_get(const smem_toc_config_type **config_tbl)
{
  *config_tbl = smem_boot_targ_cfg_toc_tbl;

  return smem_boot_targ_cfg_toc_version;
}

/*===========================================================================
  FUNCTION  smem_save_targ_smem_info
===========================================================================*/
/**
  Saves the SMEM base physical address and size in a structure at a set 
  address.  This must be called from the beginning of smem_boot_init().

  The size and address can then be read by each image at the beginning of 
  smem_init.  This allows each other image to have a common binary for all 
  targets, irrespective of the memory map.

  @return
  None.
*/
/*=========================================================================*/
void smem_boot_targ_info_set(void)
{
  /* Bootloaders memory map is 1-to-1.  SMEM target info should be populated
   * in the last 4kB page, directly following the TOC entries. */
  /* Need to look into the TOC to find its num_entries and determine the
   * total size. */
  uint8 *smem_base_addr = smem_os_map_base_va(smem_boot_targ_base_addr_get(),
                                              smem_boot_targ_size_get());
  smem_toc_type *toc = (smem_toc_type *)(smem_base_addr + 
                                         smem_boot_targ_size_get() - 
                                         SMEM_TOC_SIZE);

  uint32 toc_size = sizeof(smem_toc_type) + 
      (sizeof(smem_toc_entry_type) * toc->num_entries);
  /* This should always be true, but ensure that the address for the smem
   * target info is 64-bit aligned, so any architecture will read it the same
   * without needing any assumptions about alignment/packing. */
  uint32 toc_padded_size = (toc_size + 0x7) & (~(0x7));
  uintptr_t smem_targ_info_addr = ((uintptr_t)toc + toc_padded_size);
  smem_targ_info_type *smem_targ_info_ptr = 
      (smem_targ_info_type *)smem_targ_info_addr;

  /* The TOC table + SMEM target info must fit in the last 4kB page. */
  if (toc_padded_size + sizeof(smem_targ_info_type) > SMEM_TOC_SIZE)
  {
    SMEM_OS_LOG_FATAL(3, "SMEM TOC + targ_info too big! TOC: %d B; targ_info: %d B, "
              "max: %d B.",
              toc_size, sizeof(smem_targ_info_type), SMEM_TOC_SIZE);
  }

  /* Fill out the SMEM target info structure. */
  smem_targ_info_ptr->identifier          = SMEM_TARG_INFO_IDENTIFIER;
  smem_targ_info_ptr->smem_size           = smem_boot_targ_size_get();
  smem_targ_info_ptr->smem_base_phys_addr = (uint64)(uintptr_t)
                                            smem_boot_targ_base_addr_get();
  smem_targ_info_ptr->smem_max_items      = smem_boot_targ_cfg_max_items;
  
  /* Save the address of the SMEM target info into HWIO registers
   * HWIO_TCSR_TZ_WONCE_n_ADDR(0++1). This will 
   * affect the value of the define SMEM_TARG_INFO_ADDR for all images. 
   * Bootloaders does not map the HWIO, but merely uses a define. */
  *smem_targ_wonce_reg_addrs[0] = (uint64)smem_targ_info_addr & 0xFFFFFFFF;
  *smem_targ_wonce_reg_addrs[1] = (uint64)smem_targ_info_addr >> 32;
}

/*=============================================================================
  FUNCTION  smem_targ_init
=============================================================================*/
/**
 * @param[in]
 *
 * @return
 *
 * @sideeffects
 */
/*===========================================================================*/
void smem_targ_init(void)
{
   /* This is needed for minidump mode */
  if (smem_boot_targ_smem_base_addr == NULL)
  {
    smem_boot_targ_init();
  }
}

/*=============================================================================
  FUNCTION  smem_targ_base_addr_get
=============================================================================*/
/**
 * @param[in]
 *
 * @return
 *
 * @sideeffects
 */
/*===========================================================================*/
uint8 *smem_targ_base_addr_get(void)
{
  return smem_boot_targ_smem_base_addr;
}

/*=============================================================================
  FUNCTION  smem_targ_size_get
=============================================================================*/
/**
 * @param[in]
 *
 * @return
 *
 * @sideeffects
 */
/*===========================================================================*/
uint32 smem_targ_size_get(void)
{
  return smem_boot_targ_smem_size;
}

/*=============================================================================
  FUNCTION  smem_targ_max_items_get
=============================================================================*/
/**
 * @param[in]
 *
 * @return
 *
 * @sideeffects
 */
/*===========================================================================*/
uint16 smem_targ_max_items_get(void)
{
  return smem_boot_targ_cfg_max_items;
}
