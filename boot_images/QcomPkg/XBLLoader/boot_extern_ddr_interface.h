#ifndef BOOT_EXTERN_DDR_INTERFACE_H
#define BOOT_EXTERN_DDR_INTERFACE_H
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
05/03/16   rp	   Renamed ddr training APIs
07/28/15   rp      Added boot_ddr_post_training api
06/08/15   rp	   Updated DDR training API to return a boolean
09/23/14   ck      Removed feature flag as API is always enabled in XBL
02/24/14   ck      Added boot_ddr_restore_from_wdog_reset
02/07/14   ck      Readjusted DDR info to match 512MB 8916 spec
01/07/14   ck      Updated DDR info to match 8916 spec
08/15/13   sl      Change feature flag for boot_ddr_debug
06/18/13   sl      Add boot_ddr_debug
05/23/13   tw      removed unused ddr sdram2 in ddr structure
10/16/12   sl      Add boot_ddr_test
10/09/12   dh      Add boot_ddr_get_info
09/14/12   jz      Remove unused boot_ddr_param_interleaved
07/23/12   dh      Add DDR training API
10/18/11   dh      Initial Creation.
===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/

#include "ddr_drivers.h"
#include "ddr_common.h"


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
boolean boot_ddr_set_params(boolean dload_mode);


/*=============================================================================
    Function : boot_ddr_initialize_device
=============================================================================*/

/**
  Sets up the DDR parameters in the HAL and initializes the DDR device.

  This function passes the currently used DDR parameters to the HAL and
  then initializes the DDR device per the specified clock speed.

  @param[in] clk_speed    Clock speed.

  @return
  None.

  @dependencies
  None.
*/
void boot_ddr_initialize_device(uint32 clk_speed, boolean dload_mode);


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
ddr_size_info boot_ddr_get_size( void );


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
ddr_size_partition boot_ddr_get_partition(void);


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
boolean boot_ddr_params_is_training_required(void);


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
void* boot_ddr_params_get_training_data(uint32 *size);


/* ============================================================================
**  Function : boot_ddr_params_set_training_data
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
boolean boot_ddr_params_set_training_data(void *ddr_training_data,
                                          uint32 size);


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
boolean boot_ddr_post_init(void);


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
ddr_info boot_ddr_get_info(void);

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
void boot_ddr_share_data_to_aop(void);



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
void boot_ddr_test(uint32 clk_speed);


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
void boot_ddr_debug( void );

/* =============================================================================
**  Function : boot_ddr_do_phy_training_init
** =============================================================================
*/
/**
*   @brief
*   do DDR PHY training.
*
*   @param  None
*
*   @retval  Returns true if ddr training is done, false if there is no training 
*			 and only values have been restored
*
*   @dependencies
*   Needs to initialize railway and CPR.
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
void boot_ddr_do_phy_training_init(void);

/* =============================================================================
**  Function : boot_ddr_do_phy_training_restore
** =============================================================================
*/
/**
*   @brief
*   do DDR PHY training.
*
*   @param  None
*
*   @retval  Returns true if ddr training is done, false if there is no training 
*			 and only values have been restored
*
*   @dependencies
*   Needs to initialize railway and CPR.
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
boolean boot_ddr_do_phy_training_restore(void);

/* =============================================================================
**  Function : boot_ddr_post_training
** =============================================================================
*/
/**
*   @brief
*   This function takes care of all post training activities.
*
*   @param  None
*
*   @retval  None
*
*   @dependencies
*   Needs to initialize railway and CPR.
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
boolean boot_ddr_post_training(void);

/* =============================================================================
**  Function : boot_ddr_restore_from_wdog_reset
** =============================================================================
*/
/**
*   @brief
*   Used by SDI to bring DDR out of self refresh
*
*   @param [in]
*   clk_speed_khz  Clock speed of DDR in KHz
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
void boot_ddr_restore_from_wdog_reset(uint32 clk_speed_khz);

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
void boot_ddr_remapper(void);

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
void boot_ddr_remap_to_lower_32bit(void);

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
void boot_ddr_check_remapping(void);

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


void boot_get_ddr_details_table (ddr_details_entry_ptr_type ddr_details_entry_ptr);

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

void boot_ddr_post_setup();


#endif /* BOOT_EXTERN_DDR_INTERFACE_H */
