#ifndef PM_TARGET_INFORMATION_H
#define PM_TARGET_INFORMATION_H

/*! \file pm_target_information.h 
*  \n  
*  \brief This file contains target specific PMIC settings common across processors.
*  \n   
*  \n &copy; Copyright 2012-2015 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

                EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module over time.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/25/15   al      removing loader specific API from common file
04/14/15   al      Adding changes for Hawker support
10/17/14   aab     Added pm_target_information_spmi_chnl_cfg
07/16/14   akm     Comm change Updates
03/31/14   akm     Updated to the latest PMIC driver architecture  
09/19/13   aab     Added support for SBL_REG_CONFIG and PBS_RAM_CONFIG
12/03/12   hw      Architecture change from Module to Peripheral based driver
12/08/10   wra     Creating file for MSM8960
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"
#include "pm_dal_prop_ids.h"
#include "pm_utils.h"
#include "pm_comm.h"
#include "device_info.h"
#include "pm_config_target.h"
/*===========================================================================

                     FUNCTION DECLARATION 

===========================================================================*/

/*! \brief This function initializes the target/PMIC related information.
 *  \param[in] None
 *
 *  \return None.
 *
 *  <b>Dependencies</b>
 *  \li DAL framework.
 */
pm_err_flag_type pm_target_information_init(void);

/*! \brief This function returns the common props information for the parameter specified property id.
 *
 *  \param[in] prop_id: Property id.
 *
 *  \return A pointer to the information.
 *
 *  <b>Dependencies</b>
 *  \li pm_init() .
 */ 
void* pm_target_information_get_common_info(uint32 prop_id);

/*! \brief This function returns the target specific props information for the parameter specified property id.
 *
 *  \param[in] prop_id: Property id.
 *
 *  \return A pointer to the information.
 *
 *  <b>Dependencies</b>
 *  \li pm_init() .
 */
void* pm_target_information_get_specific_info(uint32 prop_id);

/*! \brief This function returns the target specific props count info for the parameter specified property id.
 *
 *  \param[in] prop_id: Property id.
 *
 *  \return count value.
 *
 *  <b>Dependencies</b>
 *  \li pm_init() .
 */
uint32 pm_target_information_get_periph_count_info(uint32 prop_id, uint8 pmic_index);

/*! \brief This function reads the parameter specified peripheral information.
 *  \param[in] comm_ptr The Comm Channel.
 *
 *  \param[in] peripheral_info A pointer to where the peripheral information is going to be saved.
 *
 *  \return Error flag.
 *
 *  <b>Dependencies</b>
 *  \li pm_processor_init_comms() .
 */
pm_err_flag_type pm_target_information_read_peripheral_rev(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info);

#endif //PM_TARGET_INFORMATION_H


