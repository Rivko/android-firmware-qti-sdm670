#ifndef PM_BOB_DRIVER__H
#define PM_BOB_DRIVER__H

/*! \file pm_bob_driver.h
 *  \n
 *  \brief This file contains BOOST BYPASS peripheral driver related function prototypes,
 *         enums and driver data structure type.   
 *  \n  
 *  \n &copy; Copyright 2014 QUALCOMM Technologies Incorporated, All Rights Reserved
 */
/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/drivers/bob/src/pm_bob_driver.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/14   kt      Created
========================================================================== */

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_err_flags.h"
#include "pm_pwr_alg.h"
#include "pm_comm.h"

/*===========================================================================

                     BOOST BYPASS TYPES AND STRUCTURES 

===========================================================================*/

typedef struct
{
    pm_comm_info_type *comm_ptr;
    pm_pwr_data_type  pm_pwr_data;
} pm_bob_data_type;

/*===========================================================================

                     FUNCTION DECLARATION 

===========================================================================*/
void pm_bob_driver_init(pm_comm_info_type *comm_ptr, peripheral_info_type *peripheral_info, uint8 pmic_index);

pm_bob_data_type* pm_bob_get_data(uint8 pmic_index);

uint8 pm_bob_get_num_peripherals(uint8 pmic_index);


#endif // PM_BOB_DRIVER__H
