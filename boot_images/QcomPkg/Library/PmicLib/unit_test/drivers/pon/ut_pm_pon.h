#ifndef __UT_PM_PON_H__
#define __UT_PM_PON_H__
/*! \file pm_pon.h
 *  \n
 *  \brief This file contains functions and variable declarations to support 
 *   the testing pon driver module.
 *
 *  \n &copy; Copyright (c) 2017 by Qualcomm Technologies Inc.  All Rights Reserved.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/09/17   al      Created PON API.
===========================================================================*/
/*===========================================================================

                        HEADER FILES

===========================================================================*/
#include "pm_err_flags.h"
#include "com_dtypes.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/

/*Test API to call the PON APIs for basic sanity testing*/
pm_err_flag_type pon_test(void);

#endif // __UT_PM_PON_H__
