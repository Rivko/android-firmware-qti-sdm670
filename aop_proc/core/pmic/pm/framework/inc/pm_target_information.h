#ifndef PM_TARGET_INFORMATION_H
#define PM_TARGET_INFORMATION_H

/*! \file pm_target_information.h
*  \n
*  \brief 
*  \n
*  \n &copy; Copyright 2012-2016 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/framework/inc/pm_target_information.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/11/13   hs      Adding settling time for regulators.
05/29/13   aks     Clk driver input check (do not disable critical clocks)
05/20/13   aks     clk driver code re-architecture
04/23/13   hs      Fixed the naming convention in \config.
04/12/13   hs      Code refactoring.
03/21/13   hs      Code refactoring.
02/27/13   hs      Code refactoring.
12/06/12   aks     Integrating Henry's changes - Rearchitecturing module driver to peripheral driver
12/08/10   wra     Creating file for MSM8960
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_err_flags.h"
#include "pm_resources_and_types.h"
#include "pm_dal_prop_ids.h"
#include "pm_utils.h"
#include "pm_comm.h"
#include "pm_vrm.h"

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
void pm_target_information_init(void);

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
void* pm_target_information_get_specific_info(uint32 prop_id );

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

#endif // PM_TARGET_INFORMATION_H


