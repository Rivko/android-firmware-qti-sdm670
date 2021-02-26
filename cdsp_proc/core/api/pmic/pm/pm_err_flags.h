#ifndef __PM_ERR_FLAGS_H__
#define __PM_ERR_FLAGS_H__
/*! \file 
 *  \n
 *  \brief  pm_err_flags.h ---- PMIC library error codes 
 *  \n
 *  \n This file is defines error codes returned by PMIC library APIs 
 *  \n
 *  \n &copy; Copyright 2017 QUALCOMM Technologies Incorporated, All Rights Reserved
 */
/* ======================================================================= */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/10/17   akm     New file
=============================================================================*/

/**
 * These are all the error codes returned by PMIC APIs
 *
 * PMIC APIs most commonly return errors if the caller has tried
 * to use parameters that are out of range or if the feature is
 * not supported by the PMIC model detected at run-time.
 */
typedef enum
{
    PM_ERR_FLAG__SUCCESS,  
    PM_ERR_FLAG__INPUT_INVALID,    
    PM_ERR_FLAG__PMIC_NOT_SUPPORTED,
    PM_ERR_FLAG__INVALID_POINTER,
    PM_ERR_FLAG__INVALID

} pm_err_flag_type;

#endif

