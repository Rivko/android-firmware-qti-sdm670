#ifndef __PM_APP_VREG__H__
#define __PM_APP_VREG__H__

/*! \file pm_app_vreg.h
 *  \n
 *  \brief  PMIC VREG APP API support functions
 *  \details  This file contains functions and variable declarations to support PMIC VREG APP Level APIs
 *
 *
 *  \n &copy; Copyright (c) 2016-2017  by QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================
                                Edit History
This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

 
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/28/16   aab     Creation
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_err_flags.h"


/*=========================================================================== 
 
                     GLOBAL DEFINES
 
===========================================================================*/


/*=========================================================================== 
 
                     GLOBAL TYPE DEFINITIONS
 
===========================================================================*/
  

/*===========================================================================

                     PROTOTYPES 

===========================================================================*/
/**
 * @brief Vreg OCP status Reset configuration
 *  
 * @details Vreg OCP status Reset configuration for each Buck and LDOs in all PMICs available on target
 *  
 * @param none
 *  
 *                   
 * @return pm_err_flag_type PM_ERR_FLAG_SUCCESS = SUCCESS else ERROR.
 */
pm_err_flag_type
pm_app_vreg_clear_ocp_status(void);


#endif  //__PM_APP_VREG__H__
