#ifndef __PM_TARGET_INFORMATION_H__
#define __PM_TARGET_INFORMATION_H__

/*! \file pm_target_information.h 
*  \n
*  \brief
*  \n  
*  \n &copy; Copyright 2012-2013 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

                EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module over time.

$Header: //components/rel/core.qdsp6/2.1.c4/pmic/pm/framework/inc/pm_target_information.h#1 $ 

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_err_flags.h"
#include "pm_dal_prop_ids.h"
#include "DALStdDef.h"

/*===========================================================================

                     ENUMERATIONS 

===========================================================================*/

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
void* pm_target_information_get_common_info(const char* prop_id);

/*! \brief This function returns the target specific props information for the parameter specified property id.
 *
 *  \param[in] prop_id: Property id.
 *
 *  \return A pointer to the information.
 *
 *  <b>Dependencies</b>
 *  \li pm_init() .
 */
void* pm_target_information_get_specific_info(const char* prop_id);

/*! \brief This function returns the target specific props count info for the parameter specified property id.
 *
 *  \param[in] prop_id: Property id.
 *
 *  \return count value.
 *
 *  <b>Dependencies</b>
 *  \li pm_init() .
 */
uint32 pm_target_information_get_periph_count_info(const char* prop_id,uint8 pmic_index);

#endif //PM_TARGET_INFORMATION_H


