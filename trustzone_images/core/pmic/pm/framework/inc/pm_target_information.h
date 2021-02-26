#ifndef __PM_TARGET_INFORMATION_H__
#define __PM_TARGET_INFORMATION_H__

/*! \file pm_target_information.h 
*  \n  
*  \brief This file contains target specific PMIC settings common across processors.
*  \n   
*  \n &copy; Copyright 2017 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

                EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module over time.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/31/17   akm     Created
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_lib_err.h"
#include "pm_resources_and_types.h"
#include "pm_comm.h"
#include "device_info.h"
#include "DALSys.h"
/*===========================================================================

                     FUNCTION DECLARATION 

===========================================================================*/

/*! \brief This function returns the target specific Dal Handler.
 *
 *  \param[in] prop_id: Property id.
 *
 *  \return A pointer to the information.
 *
 *  <b>Dependencies</b>
 *  \li pm_init() .
 */
pm_err_flag_type pm_target_information_init();



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


/*! \brief This function returns the target specific props information for the parameter specified property id.
 *
 *  \param[in] prop_id: Property id.
 *
 *  \return A pointer to the information.
 *
 *  <b>Dependencies</b>
 *  \li pm_init() .
 */
uint32 pm_target_information_get_uint_info(const char *prop_name);




/*! \brief  Get DAl Device Config property handle.
 *
 *  \param[in] prop_id: Property id.
 *
 *  \return DAL_SUCCESS
 *
 *  <b>Dependencies</b>
 *  \li pm_init() .
 */
DALResult pm_get_dal_prop
(
  const char *dev_name,
  DALSYSPropertyHandle hProps
);

#endif //PM_TARGET_INFORMATION_H


