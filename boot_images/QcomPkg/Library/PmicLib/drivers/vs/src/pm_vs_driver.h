#ifndef PM_VS_DRIVER__H
#define PM_VS_DRIVER__H

/*! \file pm_vs_driver.h
 *  \n
 *  \brief   
 *  \details  
 *  \n &copy; Copyright 2012-2013 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/06/12   hw      Rearchitecturing module driver to peripheral driver
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_target_information.h"
#include "pm_pwr_alg.h"


/*===========================================================================

                     VS TYPES AND STRUCTURES 

===========================================================================*/

typedef struct
{
    pm_comm_info_type *comm_ptr;
    pm_pwr_data_type  pm_pwr_data;
} pm_vs_data_type;

/*===========================================================================

                     FUNCTION DECLARATION 

===========================================================================*/
void pm_vs_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index);

pm_vs_data_type* pm_vs_get_data(uint8 pmic_index);

uint8 pm_vs_get_num_peripherals(uint8 pmic_index);


#endif // PM_VS_DRIVER__H
