/** @file DDRTargetLibNull.c
  
  Stub functions for DDRTargetLib

  Copyright (c) 2014-2017, Qualcomm Technologies, Inc. All rights reserved.
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 05/18/17   vk      Add missing stubs
 06/20/16   kpa     Added ddr training stubs
 05/12/15   plc     Updated with sdi_exit/init api's
 09/23/14   ck      Removed ddr_restore_from_wdog_reset.  Not needed in HoneyBadger
 05/30/14   kedara  Initial revision

=============================================================================*/


/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "ddr_drivers.h"
#include "ddr_common.h"
#include "ddr_test.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

ddr_size_partition ddr_partitions_stub;
ddr_info ddr_info_stub;
ddr_size_info ddr_size_info_stub;

/* ============================================================================
**  Function : ddr_set_params
** ============================================================================
*/
/**
*   @brief
*   This function sets the DDR driver's device table to an external device table.
*   It will also update the interleaving mode information according to the parameter
*   header that's passed in.
*   User should append ddr_device_params_header_v1 header in front of their own DDR device 
*   table and pass  a pointer points to the beginning of this header via the this API to 
*   force DDR driver to utilize the user defined parameters.
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
*   @sa
*   None
*
*   @sa
*   None
*/
boolean ddr_set_params(void *ddr_params_ptr, uint32 ddr_params_size)
{
  return TRUE;
}

/* ============================================================================
**  Function : ddr_initialize_device
** ============================================================================
*/
/*!
*   @brief
*   This function will setup the ddr parameter in HAl and initialize the ddr
*   
*   @details
*   This function will first pass the currently used ddr device parameter
*   to the HAL. Then it will initialize the ddr as per specified speed
*   
*   @param clkspeed   -  [IN] clk speed to initialize for
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/

void ddr_initialize_device(uint32 clk_speed, 
                           boolean dload_mode)
{
}

/* ============================================================================
**  Function : ddr_get_size
** ============================================================================
*/
/*!
*   @brief
*   Returns the ddr size and base address
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
*   @retval ddr_size_infor contains the ddr size and base address
*   
*   @sa None
*/
ddr_size_info ddr_get_size(void)
{
  return ddr_size_info_stub;
}

/* ============================================================================
**  Function : ddr_get_partition
** ============================================================================
*/
/*!
*   @brief
*   Returns the ddr partitions and base address
*   
*   @details
*	This API will return the number of partitions on each interface along with the 
*	base address
*   
*  @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval ddr_size_partition contains the ddr partitions and base address
*   
*   @sa None
*/
ddr_size_partition ddr_get_partition(void)
{
  return ddr_partitions_stub;
}

/* =============================================================================
**  Function : ddr_params_is_training_required
** =============================================================================
*/
/**
*   @brief
*   Indicate whether DDR parameter training is required or not. Training is
*   required if and only if DDR itself (e.g. PCDDR3) requires parameter training
*   and DDR parameter partition is invalid. LPDDR3 will always require training syncronization
*   to be done between rpm and sbl
*
*   @param  None
*
*   @retval  TRUE   Training required
*   @retval  FALSE  Training not required
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
boolean ddr_params_is_training_required( void )
{
  return FALSE;
}

/* =============================================================================
**  Function : ddr_params_get_training_data
** =============================================================================
*/
/**
*   @brief
*   Get DDR parameter partition: partition base is returned, and partition size
*   is updated through pointer.
*
*   @param[out]  size  DDR parameter partition size
*
*   @return
*   DDR parameter partition base
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
void *ddr_params_get_training_data(uint32 *size)
{
  return NULL;
}

/* =============================================================================
**  Function : ddr_params_set_training_data
** =============================================================================
*/
/**
*   @brief
*   Set DDR parameter partition.
*
*   @param[in]  base  DDR parameter partition base
*   @param[in]  size  DDR parameter partition size
*
*   @retval  TRUE   Partition set successfully
*   @retval  FALSE  Partition set unsuccessfully
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
boolean ddr_params_set_training_data(void *base, uint32 size)
{
  return TRUE;
}

/* ============================================================================
**  Function : ddr_post_init
** ============================================================================
*/
/*!
*   @brief
*   This function is called after ddr is initialized. It will take care of any
*   post initialization activities such as ddr training.
*
*   @details
*   This function is called after ddr is initialized. It will take care of any
*   post initialization activities such as ddr training.
*
*   @param
*   boolean -
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/
boolean ddr_post_init()
{
  return TRUE;
}

/* ============================================================================
**  Function : ddr_get_info
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
*   @retval ddr_size_infor contains the ddr size and base address
*
*   @sa None
*/

ddr_info ddr_get_info(void)
{
  return ddr_info_stub;
}

/* =============================================================================
**  Function : ddr_test_all
** =============================================================================
*/
/**
*   @brief
*   Test DDR located at each interface/CS.
*
*   This function tests DDR with the test suite specified by 'test_suit':
*
*   DDR_TEST_SUITE_EMPTY: empty test suite;
*   DDR_TEST_SUITE_ACCESS: test suite for DDR access;
*   DDR_TEST_SUITE_SELF_REFRESH: test suite for DDR self refresh;
*   DDR_TEST_SUITE_DEEP_POWER_DOWN: test suite for DDR deep power down.
*
*   In addition, for each test suite, test level is specified by 'test_level' to
*   tune how stressful test suite would be. Level 0 is the least stressful.
*
*   @param[in]  test_suite     DDR test suite
*   @param[in]  test_level     DDR test level
*   @param[in]  clk_speed      Clock speed (in KHz) DDR runs at
*
*   @retval  TRUE     Test passed for all attached DDR
*   @retval  FALSE    Test failed for some attached DDR
*
*   @dependencies
*   None
*
*   @sideeffects
*   Memory is corrupted after this function is called. Error is logged if any.
*
*   @sa
*   None
*/
boolean ddr_test_all
(
  ddr_test_suite  test_suite,
  uint32          test_level,
  uint32          clk_speed
)
{
  return TRUE;
}

/* =============================================================================
**  Function : ddr_test
** =============================================================================
*/
/**
*   @brief
*   Test DDR located at specific interface/CS.
*
*   This function tests DDR with the test suite specified by 'test_suit':
*
*   DDR_TEST_SUITE_EMPTY: empty test suite;
*   DDR_TEST_SUITE_ACCESS: test suite for DDR access;
*   DDR_TEST_SUITE_SELF_REFRESH: test suite for DDR self refresh;
*   DDR_TEST_SUITE_DEEP_POWER_DOWN: test suite for DDR deep power down.
*
*   In addition, for each test suite, test level is specified by 'test_level' to
*   tune how stressful test suite would be. Level 0 is the least stressful.
*
*   @param[in]  test_suite     DDR test suite
*   @param[in]  test_level     DDR test level
*   @param[in]  interface      Interface where DDR is located
*   @param[in]  chip_select    Chip select where DDR is located
*   @param[in]  clk_speed      Clock speed (in KHz) DDR runs at
*
*   @retval  TRUE     Test passed or no DDR attached at the interface/CS
*   @retval  FALSE    Test failed
*
*   @dependencies
*   None
*
*   @sideeffects
*   Memory is corrupted after this function is called. Error is logged if any.
*
*   @sa
*   None
*/
boolean ddr_test
(
  ddr_test_suite    test_suite,
  uint32            test_level,
  DDR_CHANNEL       interface,
  DDR_CHIPSELECT    chip_select,
  uint32            clk_speed
)
{
  return TRUE;
}

/* =============================================================================
**  Function : ddr_test_exit
** =============================================================================
*/
/**
*   @brief
*   Exit point of DDR Test Framework: an infinite loop.
*
*   @param  None
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

void ddr_test_exit( void )
{
}

/* =============================================================================
**  Function : ddr_do_phy_training
** =============================================================================
*/
/**
*   @brief
*   Indicates that PHY training needs to be done in SBL1.
*
*   @param  None
*
*   @retval  TRUE   Training required
*   @retval  FALSE  Training not required
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
boolean ddr_do_phy_training( void )
{
  return FALSE;
}

void ddr_sdi_exit(void)
{
  return;
}

void ddr_sdi_init(void)
{
  return;
}


/* =============================================================================
**  Function : ddr_remapper
** =============================================================================
*/
/**
*   @brief
*   configures ddr to be remapped to lower 32bit addresss
*
*   @param void
*
*   @retval  void
*
*   @dependencies
*   ddr initialization has already finished
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
void ddr_remapper(void)
{
}

/* =============================================================================
**  Function : ddr_do_phy_training_init
** =============================================================================
*/
/**
*   @brief
*   Indicates that PHY training needs to be done in SBL1.
*
*   @param  None
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
void ddr_do_phy_training_init( void )
{
}

/* =============================================================================
**  Function : ddr_do_phy_training_restore
** =============================================================================
*/
/**
*   @brief
*   Restore training data results, if phy training is already done
*
*   @param  None
*
*   @retval  TRUE   Training required
*   @retval  FALSE  Training not required
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
boolean ddr_do_phy_training_restore( void )
{
  return TRUE;
}

/* =============================================================================
**  Function : ddr_post_training
** =============================================================================
*/
/**
*   @brief
*   This function is called after ddr training. It will take care of all post
*   training activities such as computing checksum over training data.
*
*   @param  None
*
*   @retval  TRUE If one time training is enabled
*   @retval  FALSE If one time training is disabled
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
boolean ddr_post_training(void)
{
  return TRUE;
}


ddr_clock_plan ddr_clock_query(ddr_clock_query_type plan)
{
 ddr_clock_plan current_plan = {0,0};
 return current_plan;
}


void ddr_pre_clock_switch(uint32 curr_clk, uint32 new_clk , DDR_CHANNEL interface_name)
{
}

void ddr_clock_switch(uint32 curr_clk, uint32 new_clk , DDR_CHANNEL interface_name)
{
}

void ddr_post_clock_switch(uint32 curr_clk, uint32 new_clk, DDR_CHANNEL interface_name)
{
}

