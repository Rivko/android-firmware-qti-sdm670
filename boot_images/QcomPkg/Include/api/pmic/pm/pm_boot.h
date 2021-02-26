#ifndef __PM_BOOT_H__
#define __PM_BOOT_H__

/*! \file 
 *  \n
 *  \brief  
 *  \details  
 *   This header file contains enums and API definitions for PMIC boot
 *   
 *  \n &copy; Copyright 2011,2014-2017 QUALCOMM Technologies Incorporated, All Rights Reserved
 */
/* ======================================================================= */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/30/17   aab     Added pm_aop_pre_init()
02/18/16   aab     Added pm_system_init()  API
10/12/15   al      Adding API to set PMIC ELF start address 
06/23/15   aab     Added pm_device_setup()
05/31/15   aab     Added pre and post function for pm_device_init(), pm_driver_init() and pm_sbl_chg_init()
04/02/15   aab     Added pm_device_programmer_init() API.
02/26/13   kt      Added PMIC SMEM Init API.
12/06/12   hw      Remove comdef.h and use com_dtypes.h
09/25/12   hw      Added support for pmic api by creating pm_driver_init
09/21/11   hs      Added support for TZ(Trust Zone). 
04/21/11   mpt     initial version
=============================================================================*/
/*===========================================================================

                        HEADER FILES

===========================================================================*/
#include "com_dtypes.h"
#include "pm_err_flags.h"

/*===========================================================================

                        API PROTOTYPE

===========================================================================*/

/*! \brief This function executes the pmic device Pre initialization
 *  \return Error flag.
 *
 */
pm_err_flag_type pm_device_pre_init( void );

/*! \brief This function executes the pmic device initialization
 *  \return Error flag.
 *
 */ 
pm_err_flag_type pm_device_init( void );

/*! \brief This function executes the pmic device post initialization
 *  \return Error flag.
 *
 */ 
pm_err_flag_type pm_device_post_init( void );




/*! \brief This function executes the pmic driver pre initialization allowing API access
 *  \return Error flag.
 *
 */
pm_err_flag_type pm_driver_pre_init (void);

 
/*! \brief This function executes the pmic driver initialization allowing API access
 *  \return Error flag.
 *
 */ 
pm_err_flag_type pm_driver_init( void );


/*! \brief This function executes the pmic driver post initialization allowing API access
 *  \return Error flag.
 *
 */
pm_err_flag_type pm_driver_post_init (void);




/*! \brief This function executes the pmic sbl charging Pre initialization
 *  \return Error flag.
 *
 */ 
pm_err_flag_type pm_sbl_chg_pre_init (void);

/*! \brief This function executes the pmic sbl charging initialization
 *  \return Error flag.
 *
 */ 
pm_err_flag_type pm_sbl_chg_init (void);


/*! \brief This function executes the pmic sbl charging Post initialization
 *  \return Error flag.
 *
 */ 
pm_err_flag_type pm_sbl_chg_post_init (void); 


/*! \brief This function executes pmic post initialization
 *  \return Error flag.
 *
 */ 
pm_err_flag_type pm_system_init(void);


/*! \brief This function executes the pmic device programmer initialization
 *  \return Error flag.
 *
 */ 
pm_err_flag_type pm_device_programmer_init ( void );
/**
 * @brief This function executes the pmic device initialization in TZ.
 * 
 * @details
 *  This function executes the pmic device initialization in TZ, such as 
 *  initializing the SSBI module, etc.
 *   
 * @param[in] void No input needed.
 *
 * @return pm_err_flag_type
 *         PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG_SUCCESS           = SUCCESS.
 *
 *
 */
pm_err_flag_type pm_tz_device_init( void );
/**
 * @brief This function enables the powe rail for Qfprom block.
 * 
 * @details
 *  This function enables the powe rail for Qfprom block. 
 *   
 * @param[in] void No input needed.
 *
 * @return pm_err_flag_type
 *         PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG_SUCCESS           = SUCCESS.
 *
 *
 */
pm_err_flag_type pm_enable_qfuse_power_rail(void);
/**
 * @brief This function disables the powe rail for Qfprom block.
 * 
 * @details
 *  This function disables the powe rail for Qfprom block. 
 *   
 * @param[in] void No input needed.
 *
 * @return pm_err_flag_type
 *         PM_ERR_FLAG_FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG_SUCCESS           = SUCCESS.
 *
 *
 */
pm_err_flag_type pm_disable_qfuse_power_rail(void);
/**
 * @brief This function executes the pmic shared memory 
 *        initialization.
 * 
 * @details
 *  This function stores the PON reasons and PBS info in the
 *  shared memory and should be called by boot after smem is
 *  initialized.
 *   
 * @param[in] void No input needed.
 *
 * @return pm_err_flag_type PM_ERR_FLAG_INVALID = ERROR. 
 *         PM_ERR_FLAG_SUCCESS = SUCCESS.
 *
 *
 */
pm_err_flag_type pm_smem_init(void);

/**
 * @brief This function sets the gloabal variable that stores the pmic 
 *        elf starting address 
 * 
 * @details
 *  This function updates the variable that stores PMIC ELF
 *  start address
 *   
 * @param[in] address:  PMIC ELF start address
 *
 * @return pm_err_flag_type PM_ERR_FLAG_INVALID = ERROR. 
 *         PM_ERR_FLAG_SUCCESS = SUCCESS.
 *
 *
 */
pm_err_flag_type pm_psi_set_base_address(uint64 address);

/**
 * @brief This function returns the gloabal variable that stores
 *        the starting address of pmic elf
 * 
 * @details
 *  This function updates the variable that stores PMIC ELF
 *  start address
 *   
 * @param[out] address:  PMIC ELF start address
 *
 * @return pm_err_flag_type PM_ERR_FLAG_INVALID = ERROR. 
 *         PM_ERR_FLAG_SUCCESS = SUCCESS.
 *
 *
 */
pm_err_flag_type pm_psi_get_base_address(uint64* address);

/**
 * @brief This function executes pre aop init
 * 
 * @details
 *  Configures PMIC Resource SPMI Channel access control
 *   PVC, VRM, MGPI and VIOCTL configuation     
 *   
 *
 * @return pm_err_flag_type PM_ERR_FLAG_INVALID = ERROR. 
 *         PM_ERR_FLAG_SUCCESS = SUCCESS.
 *
 */
pm_err_flag_type pm_aop_pre_init(void);

#endif /* __PM_BOOT_H__ */
