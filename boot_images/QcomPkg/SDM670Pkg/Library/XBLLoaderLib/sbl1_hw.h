#ifndef SBL1_HW_H
#define SBL1_HW_H

/*=============================================================================

                         SBL1 Hardware Initialization

GENERAL DESCRIPTION
  This file does basic hardware initialization at power up.

Copyright 2010-2017 by Qualcomm Technologies Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Library/XBLLoaderLib/sbl1_hw.h#2 $

when       who       what, where, why
--------   ---       ----------------------------------------------------------
06/16/17   c_vkuppa  Add sbl1_hw_play_vibr to play power on vibration
03/07/17   ds		 added sbl1_pm_aop_pre_init_wrapper
11/07/16   kpa       Added sbl1_do_ddr_training
06/10/15   rp        Updated sbl1_ddr_init prototype to have bl_shared_data as function parameter
05/25/15   kpa       Update sbl1_wait_for_ddr_training prototype
02/18/15   kpa       Rename sbl1_hw_pmic_init to sbl1_hw_pre_ddr_init
01/28/15   kpa       Added sbl1_ddr_set_default_params
10/14/14   kpa       Added sbl1_hw_pmic_init() for pmic image loading support
06/06/13   dh        Add sbl1_hw_get_reset_status
04/03/13   sl        Remove DDR_SPEED_IN_KHZ
08/01/12   dh        Add sbl1_hw_deinit
07/23/12   dh        Add sbl1_set_ddr_training_data and sbl1_ddr_training
06/01/12   dh        Add sbl1_check_device_temp
05/29/12   dh        Remove sbl1_clk_regime_apps_init
10/18/11   dh        Initial revision
=============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_comdef.h"
#include "boot_sbl_shared.h"
#include "sbl1_ddr_training.h"


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

**  Function :  sbl1_hw_init

** ==========================================================================
*/
/*!
* 
* @brief
*   This function performs hardware initialization.
* 
*   Only common hardware that is needed for flash
*   initialization is initialized here.
* 
* 
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_hw_init(void);


/*===========================================================================

**  Function :  sbl1_ddr_init

** ==========================================================================
*/
/*!
* 
* @brief
*   Initialize DDR device.
* * 
* @par Dependencies 
*  Following Api's need to be called first:
*  	sbl2_hw_init : To have ddr clocks setup.
*  	sbl2_ddr_set_params : To have DDR config parameters setup.
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_ddr_init(bl_shared_data_type *bl_shared_data);

/*===========================================================================

**  Function :  sbl1_do_ddr_training

** ==========================================================================
*/
/*!
*
* @brief
*   Train DDR if applicable.
* *
* @par Dependencies
*  Following Api's need to be called first:
*    sbl1_ddr_init : To setup ddr.
*
* @retval
*   None
*
* @par Side Effects
*   None
*
*/
void sbl1_do_ddr_training(bl_shared_data_type *bl_shared_data);

/*===========================================================================

**  Function :  sbl1_hw_dload_init

** ==========================================================================
*/
/*!
* 
* @brief
*   This function initializes the necessary clocks for dload.
*   We only do so when auth is disabled 
* 
*
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_hw_dload_init(bl_shared_data_type *bl_shared_data);


/*===========================================================================

**  Function :  sbl1_hw_init_secondary

** ==========================================================================
*/
/*!
* 
* @brief
*   This function performs secondary hardware initialization.
* 
* @param[in] bl_shared_data Pointer to shared data
* 
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_hw_init_secondary( bl_shared_data_type *bl_shared_data );

/*===========================================================================

**  Function :  sbl1_hw_pre_ddr_init

** ==========================================================================
*/
/*!
* 
* @brief
*  Initialize PMIC and railway driver. API used for hw initialization that
*  must occur before DDR init.
* 
* @param[in] bl_shared_data Pointer to shared data
* 
* @par Dependencies
*  	This Api needs to be called :
*  Before: sbl1_ddr_init
*
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_hw_pre_ddr_init( bl_shared_data_type *bl_shared_data );

/*===========================================================================

**  Function :  sbl1_hw_get_reset_status

** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns the value of reset status register saved from this boot
* 
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
uint32 sbl1_hw_get_reset_status( void );

/*===========================================================================

**  Function :  sbl1_ddr_set_params

** ==========================================================================
*/
/*!
* 
* @brief
*   Set DDR configuration parameters.
* 
* @param[in] bl_shared_data Pointer to shared data
* 
* @par Dependencies
*  	This Api needs to be called :
*  Before: sbl1_ddr_init, sbl1_ddr_init_for_dload and qsee_exec_func
*  After: boot_config_data_table_init
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_ddr_set_params(  bl_shared_data_type *bl_shared_data  );

/*===========================================================================

**  Function :  sbl1_ddr_set_default_params

** ==========================================================================
*/
/*!
* 
* @brief
*   Set DDR configuration parameters
* 
* @param[in] bl_shared_data Pointer to shared data
* 
* @par Dependencies
*  	This Api needs to be called :
*  Before: sbl1_ddr_init, boot_config_data_table_init
*  After: 
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_ddr_set_default_params(  bl_shared_data_type *bl_shared_data  );

/*===========================================================================

**  Function :  sbl1_check_device_temp

** ==========================================================================
*/
/*!
* 
* @brief
*   This function does a temperature check. If the device is too hot
*   we wait few seconds to allow it to cool below certain threshold.
* 
* @param[in] 
*   None 
*     
* @par Dependencies
*   Must be called after DALSYS_InitMod
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_check_device_temp( void );


/*===========================================================================

**  Function :  sbl1_load_ddr_training_data

** ==========================================================================
*/
/*!
* 
* @brief
*   If ddr training is required, read the ddr training data from partition
*   and pass it to ddr driver.
* 
* @param[in] 
*   None 
*     
* @par Dependencies
*   Must be called before sbl1_ddr_init
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_load_ddr_training_data(void);


/*===========================================================================

**  Function :  sbl1_wait_for_ddr_training

** ==========================================================================
*/
/*!
* 
* @brief
*   Execute DDR training sequence. If ddr training data is updated, save it
*   to storage device.
* 
* @param[in] 
*   bl_shared_data Pointer to shared data 
*     
* @par Dependencies
*   Must be called after sbl1_ddr_init
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_wait_for_ddr_training( bl_shared_data_type *bl_shared_data );


/*===========================================================================

**  Function :  sbl1_hw_deinit

** ==========================================================================
*/
/*!
* 
* @brief
*   This function deinit the hardware that's not needed beyond sbl1
* 
* @param[in] bl_shared_data Pointer to shared data
* 
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_hw_deinit(void);

/*===========================================================================

**  Function :  sbl1_pm_aop_pre_init_wrapper

** ==========================================================================
*/
/*!
*
* @brief
*   Do pre init procedure for AOP.
*
* @param[in]
*   bl_shared_data Pointer to shared data
*
* @par Dependencies
*   Must be called before loading AOP
*
* @retval
*   None
*
* @par Side Effects
*   None
*
*/
void sbl1_pm_aop_pre_init_wrapper( bl_shared_data_type *bl_shared_data );

/*===========================================================================

**  Function :  sbl1_hw_play_vibr

** ==========================================================================
*/
/*!
*
* @brief
*   This function calls PMIC API to play power on vibration
*
* @param[in] bl_shared_data Pointer to shared data
*
* @par Dependencies
*   None
*
* @retval
*   None
*
* @par Side Effects
*   None
*
*/
void sbl1_hw_play_vibr(bl_shared_data_type *bl_shared_data);

/*===========================================================================

**  Function :  sbl1_release_adsp_proc

** ==========================================================================
*/
/*!
* 
* @brief
*  This function brings ADSP proc out of reset
* 
* @param[in] 
*  bl_shared_data Pointer to shared data
*
* @par Dependencies
*  It is assumed that ADSP image is loaded before invoking this api.
* 
* @retval
*  None
* 
* @par Side Effects
*  None
* 
*/
void sbl1_release_adsp_proc( bl_shared_data_type *bl_shared_data );


#endif /* SBL1_HW_H */
