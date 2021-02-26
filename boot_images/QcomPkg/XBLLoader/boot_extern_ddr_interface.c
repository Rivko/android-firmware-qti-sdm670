/*===========================================================================

                    BOOT EXTERN DDR DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external ddr drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2011-2016 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/01/17   rp      Added boot_ddr_share_data_to_aop
05/25/17   dai     Added intialization function for shared ddr details
05/05/17   rp      Added dload mode parameter to boot_ddr_set_params
04/27/17   rp      Added dload mode parameter to boot_ddr_initialize_device
05/04/16   rp      Cleaned up boot_ddr_params_is_training_on_rpm_required API
02/26/15   ck      Removed include of TARGET_DEF_H.
02/12/15   kpa     Move boot_ddr_do_ddr_training to a separate lib.
10/21/14   ck      Updated to support 64 bit addressing
10/06/14   kpa     Include Target_cust.h instead of boot_target_scl.h
09/23/14   ck      Removed boot_ddr_restore_from_wdog_reset.  Not needed in HoneyBadger
02/24/14   ck      Added boot_ddr_restore_from_wdog_reset
08/27/13   sr      All param function declarations reside in ddr_common_params.h
08/15/13   sl      Add boot_ddr_debug
06/20/13   sl      Remove sanity DDR test
03/26/13   dh      Call mmu_get_page_table_base to get page table base in ddr test
02/07/13   dh      Disable ddr_test_all for now
11/12/12   sl      Allow boot_ddr_test to make DDR region uncacheable
10/16/12   sl      Add boot_ddr_test
10/09/12   dh      Add boot_ddr_get_info
09/17/12   jz      Remove unused boot_ddr_param_interleaved
07/23/12   dh      Add DDR training API
10/18/11   dh      Initial Creation.
=============================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "ddr_common.h"
#include "ddr_drivers.h"
#include "ddr_common_params.h"
#include "ddr_test.h"
#include "ddr_log.h"
#include "boot_cache_mmu.h"
#include "boot_page_table_armv7.h"
#include "boot_error_if.h"

extern boolean ddr_post_init(void);

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

/* ============================================================================
**  Function : boot_ddr_set_params
** ============================================================================
*/
/**
*   @brief
*   This function sets the DDR driver's device table to an external device table.
*   It will also update the interleaving mode information according to the parameter
*   header that's passed in.
*   User should append ddr_device_params_header_v1 header in front of their own DDR device 
*   table and pass  a pointer points to the beginning of this header via the this API to 
*   force DDR driver to uiltize the user defined parameters.
*   The user defined ddr device table must contain a minimal number of entries to 
*   match the system's ddr devices.
*   For example if a system has two ddr devices attached then the external table must 
*   define at least two entries to match those two devices.
*   However, more entries can be added to the end of table to support other ddr devices
*   with different parameters.
*
*   @param[in]  ddr_params_ptr   Pointer to the external ddr parameters. It should
*                                point to the user defined ddr device table with ddr
*                                parameters header appended in front. This pointer must
*                                points to a 4 bytes aligned address.
*                         
*
*   @return
*   TRUE if DDR device table is updated, FALSE if an error happens and this operation
*   is aborted
*
*   @dependencies
*   Must be called prior to ddr_initialize_device. 
*   This API will take no effect after ddr_initialize_device
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
boolean boot_ddr_set_params(boolean dload_mode)
{
   return ddr_set_params(dload_mode);
}

/*=============================================================================
    Function : boot_ddr_initialize_device
=============================================================================*/

/**
  Sets up the DDR parameters in the HAL and initializes the DDR device.

  This function passes the currently used DDR parameters to the HAL and
  then initializes the DDR device per the specified clock speed.

  @param[in] clk_speed            Clock speed.
             dload_mode           Dload mode

  @return
  None.

  @dependencies
  None.
*/
void boot_ddr_initialize_device(uint32 clk_speed, boolean dload_mode)
{
  /* Initialize DDR */
  ddr_initialize_device(clk_speed, dload_mode);
}


/*=============================================================================
    Function : boot_ddr_get_size
=============================================================================*/

/**
  Gets the DDR size and base address.

  @return
  DDR size and base address.

  @dependencies
  None.
*/
ddr_size_info boot_ddr_get_size( void )
{
  return ddr_get_size();
}


/* ============================================================================
**  Function : boot_ddr_get_partition
** ============================================================================
*/
/*!
*   @brief
*   Returns the ddr partitions and base address
*   
*   @details
*
*   
*  @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval ddr_size_partitioning contains the ddr partitions and base address
*   
*   @sa None
*/
ddr_size_partition boot_ddr_get_partition()
{
  return ddr_get_partition();
}


/* ============================================================================
**  Function : boot_ddr_params_is_training_required
** ============================================================================
*/
/*!
*   @brief
*   Returns true of ddr training is required, false if not
*   
*   @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval Returns true of ddr training is required, false if not
*   
*   @sa None
*/
boolean boot_ddr_params_is_training_required(void)
{
  return ddr_params_is_training_required();
}


/* ============================================================================
**  Function : boot_ddr_params_get_training_data
** ============================================================================
*/
/*!
*   @brief
*   Returns a pointer to ddr training data
*   
*   
*  @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval pointer to the ddr training data 
*   
*   @sa None
*/
void* boot_ddr_params_get_training_data(uint32 *size)
{
  return ddr_params_get_training_data(size);
}


/* ============================================================================
**  Function : boot_ddr_param_set_training_data
** ============================================================================
*/
/*!
*   @brief
*   pass ddr training data buffer to ddr driver
*   
*  @param  ddr_training_data pointer to ddr training data
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval None
*   
*   @sa None
*/
boolean boot_ddr_params_set_training_data(void *ddr_training_data, uint32 size)
{
  return ddr_params_set_training_data(ddr_training_data, size);
}


/* ============================================================================
**  Function : boot_ddr_post_init
** ============================================================================
*/
/*!
*   @brief
*   This function does actual DDR training if training is required
*
*   
*  @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval True if ddr training data is updated, false if not
*   
*   @sa None
*/
boolean boot_ddr_post_init(void)
{
  return ddr_post_init();
}


/* ============================================================================
**  Function : boot_ddr_get_info
** ============================================================================
*/
/*!
*   @brief
*   Returns the physical configuration of ddr on target
*
*   @details
*   This function will return the physical configuration of the ddr on target
*
*   @param  None.
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*   None
*
*   @retval ddr_size_info contains the ddr size and base address
*
*   @sa None
*/
ddr_info boot_ddr_get_info(void)
{
	return ddr_get_info();
}

/* ============================================================================
**  Function : boot_ddr_share_data_to_aop
** ============================================================================
*/
/*!
*   @brief
*   Function to share ddr related info to AOP
*
*   @details
*   Shares DDR Info required during runtime to AOP via SMEM
*   SMEM needs to be initialized before calling this function
*
*   @param  None.
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*   None
*
*   @retval None
*
*   @sa None
*/
void boot_ddr_share_data_to_aop(void)
{
 ddr_share_data_to_aop(); 
}

/* =============================================================================
**  Function : boot_ddr_test
** =============================================================================
*/
/**
*   @brief
*   This function invokes DDR Test Framework.
*
*   @param[in]  clk_speed  Clock speed (in KHz) DDR runs at
*
*   @retval  None
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
void boot_ddr_test(uint32 clk_speed)
{
  /* These variables are modified by Trace32 */
  static volatile ddr_test_suite    test_suite = DDR_TEST_SUITE_EMPTY;
  static volatile uint32            test_level = 0;
  static volatile boolean           is_dcache_on = FALSE;
  static volatile boolean           is_test_all = TRUE;
  static volatile DDR_CHANNEL       interface = DDR_CH0;
  static volatile DDR_CHIPSELECT    chip_select = DDR_CS0;
  static volatile uint32            test_iterations = 1;
  
  static volatile boolean custom_test = FALSE;
  static volatile uint64 custom_test_addr;
  static volatile uint64 custom_test_size;

  if (test_suite != DDR_TEST_SUITE_EMPTY)
  {
    if (!is_dcache_on)
    {
      struct mem_block sbl_ddr_mem_block =
      {
        0, 0, 0,
        MMU_L1_SECTION_MAPPING, MMU_L1_PAGETABLE_MEM_READ_WRITE,
        MMU_L1_PAGETABLE_MEM_NON_CACHEABLE, MMU_L1_PAGETABLE_NON_EXECUTABLE_REGION,
        MMU_PAGETABLE_DEF_DOMAIN
      };

      ddr_size_info ddr_size = ddr_get_size();

      uintnt base = (ddr_size.ddr_cs0_addr[0] < ddr_size.ddr_cs1_addr[0]) ?
                    ddr_size.ddr_cs0_addr[0] : ddr_size.ddr_cs1_addr[0];

      uint8 i;
      uintnt size = 0;
      for (i = 0; i < DDR_MAX_NUM_CH; i++)
      {
        size += ddr_size.ddr_cs0[i] + ddr_size.ddr_cs1[i];
      }
      
      size = size << 20;

      size -= (SCL_SBL1_DDR_DATA_BASE + SCL_SBL1_DDR_DATA_SIZE) - base;

      base = (SCL_SBL1_DDR_DATA_BASE + SCL_SBL1_DDR_DATA_SIZE);

      sbl_ddr_mem_block.p_base = sbl_ddr_mem_block.v_base = base;
      sbl_ddr_mem_block.size_in_kbytes = size >> 10;
      
      BL_VERIFY(boot_mmu_page_table_map_single_mem_block(mmu_get_page_table_base(), &sbl_ddr_mem_block),
                BL_ERR_MMU_PGTBL_MAPPING_FAIL|BL_ERROR_GROUP_BOOT);
    }
		
    /*Unlock the DDR region locked(XPU protected) by XBL_SEC*/
	ddr_unlock_xpu_protected_mem_region();
	
	if(custom_test)
	{
	   while (test_iterations--)
       {
		 ddr_printf(DDR_NORMAL, "Iteration number : %u\n", test_iterations);
         if (!ddr_custom_test(test_suite, test_level, custom_test_addr, custom_test_size, clk_speed))
         {
           break;
         }
       }
	}
    else if (is_test_all)
    {
      while (test_iterations--)
      {
		ddr_printf(DDR_NORMAL, "Iteration number : %u\n", test_iterations);
        if (!ddr_test_all(test_suite, test_level, clk_speed))
        {
          break;
        }
      }
    }
    else
    {
      while (test_iterations--)
      {
		ddr_printf(DDR_NORMAL, "Iteration number : %u\n", test_iterations);
        if (!ddr_test(test_suite, test_level, interface, chip_select, clk_speed))
        {
          break;
        }
      }
    }
    ddr_test_exit();
  }

} /* boot_ddr_test */

/* =============================================================================
**  Function : boot_ddr_debug
** =============================================================================
*/
/**
*   @brief
*   DDR Debug Mode engine: receive and echo user's inputs, and execute commands
*   if ready.
*
*   @param  None
*
*   @retval  None
*
*   @dependencies
*   Transportation layer (e.g., USB) must be set up, and all data must be moved
*   out of DDR (otherwise would be corrupted).
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
void boot_ddr_debug( void )
{
  ddr_debug();

} /* boot_ddr_debug */

/* ============================================================================
**  Function : boot_ddr_remapper
** ============================================================================
*/
/*!
*   @brief
*   Reconfigures ddr to a different memory map than the physical address
*   
*   @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval void
*   
*   @sa None
*/
void boot_ddr_remapper(void)
{
  ddr_remapper();
}

/* ============================================================================
**  Function : boot_ddr_remap_to_lower_32bit
** ============================================================================
*/
/*!
*   @brief
*   Reconfigures ddr to a different memory map than the physical address
*   
*   @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval void
*   
*   @sa None
*/
void boot_ddr_remap_to_lower_32bit(void)
{
  ddr_remap_to_lower_32bit();
}

/* ============================================================================
**  Function : boot_ddr_check_remapping
** ============================================================================
*/
/*!
*   @brief
*   check to see if ddr has 32bit base address remapping 
*   
*   @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval void
*   
*   @sa None
*/
void boot_ddr_check_remapping(void)
{
  ddr_check_remapping();
}

/*===========================================================================

**  Function :  boot_get_ddr_details_table

** ==========================================================================
*/
/*!
* 
* @brief
*    Function initializes ddr info table in shared memory.
* 
* @par Dependencies
*   None
*   
* @retval. 
*    None
* @par Side Effects
*   None
*/

void boot_get_ddr_details_table (ddr_details_entry_ptr_type ddr_details_entry_ptr)
{
   ddr_get_shared_info_details(ddr_details_entry_ptr);
}

/*===========================================================================

**  Function :  boot_ddr_post_setup

** ==========================================================================
*/
/*!
* 
* @brief
*    Function which gets executed after DDR setup operations are complete
* 
* @par Dependencies
*   None
*   
* @retval. 
*    None
* @par Side Effects
*   None
*/

void boot_ddr_post_setup()
{
   ddr_post_setup();
}
