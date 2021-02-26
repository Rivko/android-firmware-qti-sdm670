#ifndef __PM_QC_PMIC_H__
#define __PM_QC_PMIC_H__

/*! \file 
 *  \n
 *  \brief  pm_qc_pmic.h ---- P M    H E A D E R    F I L E 
 *  \n
 *  \n This file contains prototype definitions to support interaction
 *  \n with the QUALCOMM Power Management ICs.
 *  \n
 *  \n &copy; Copyright 2003-2017 Qualcomm Technologies Incorporated, All Rights Reserved
 */


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tz/2.0.2/api/pmic/pm/pm_qc_pmic.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/02/17   akm     Moved PMIC,SPMI out to Systemdrivers
03/16/12   hs      Removed obsolete files.
02/10/11   hw      Uncomment pmapp_pmic_events.h header
02/08/11   hw      Merging changes from the PMIC Distributed Driver Arch branch
06/28/03   wra     Created as final PMIC library main include file. Named
                   so that it will not have naming collision with other
                   HLOS header files. Most of the contents in this file
                   were formerly in pm.h
===========================================================================*/

/*===========================================================================

                        PMIC INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "pm_lib_api.h"


/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/
/**
 * @brief This function executes the PMIC device initialization in TZ.
 *
 * @details
 *  This function executes the pmic device initialization in TZ, such as
 *  initializing the SSBI module, etc.
 *
 * @param[in] void No input needed.
 *
 * @return pm_err_flag_type
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *         version of the PMIC.
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 *
 */ 
pm_err_flag_type pm_init(void);

/**
 * @brief This function enable SSC retention and pin control.
 *
 * @details The function sets LDO pin control when bring SPLI out of reset. 
 *             
 *
 * @param[in] boolean active
 *                   TRUE when bringing SLPI out of reset and FALSE when putting SLPI into reset.
 *                    
 *
 * @return pm_err_flag_type
 *         PM_ERR_FLAG__SPMI_OPT_ERR  on SPMI error. 
 *         PM_ERR_FLAG__SUCCESS           = SUCCESS.
 *
 *
 */ 
pm_err_flag_type pm_ssc_config(boolean active);  

#endif    /* PM_QC_PMIC_H */

