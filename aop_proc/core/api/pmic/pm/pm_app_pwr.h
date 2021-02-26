#ifndef PM_APP_PWR_H
#define PM_APP_PWR_H
/*===========================================================================


                  P M    H E A D E R    F I L E

DESCRIPTION
  This file contains prototype definitions to support interaction
  with the QUALCOMM Power Management ICs.

Copyright (c) 2008 - 2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/api/pmic/pm/pm_app_pwr.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/05/18   AS      Adding file to expose app API
===========================================================================*/

/*===========================================================================

                        HEADER FILES

===========================================================================*/
#include "pm_err_flags.h"


/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/

/**
*  @brief pm_app_aop_core_rail_config_update use this parameter to check which rail is it.
 */

typedef enum
{
  WCSS_CX,
  WCSS_MX,
  LPI_MX,
  LPI_CX,
  MX,
  CX,
  INVALID_NAME
}pm_core_rail_name;

/**
*  @brief pm_app_aop_core_rail_config_update use this parameter to check which type of request is it.
 */

typedef enum
{
  ACTIVE,
  RETENTION,
  INVALID
}pm_request_type;


/**
 * @name pm_app_rpmh_core_rail_set_voltage
 *
 * @brief This function calls the target Specifc PMIC calls to write
 *        the Active/Retention Voltage of WCSS/MX rail to SDAM Spare Register
 *
 * @param  rail: WCSS_MX , WCSS_CX,LPI_MX , LPI_CX , MX
 * @param  voltage_uV : Voltage in uV
 * @param  active_ret : Kind of request i.e Active or retention
 * @return PMIC error code.
 *
 * @sideeffects This call will update active/retention voltage in spare register which also done by PBS Routine .
 *              This will overwritten by PBS Seq.
 *
 */

pm_err_flag_type pm_app_rpmh_core_rail_set_voltage( pm_core_rail_name rail ,uint64 voltage_uV, pm_request_type active_ret);


/**
 * @name pm_app_rpmh_core_rail_set_mode
 *
 * @brief This function calls the target Specifc PMIC calls to write
 *        the Active/Retention Mode of WCSS/MX rail to SDAM Spare Register
 *
 * @param  rail: WCSS_MX , WCSS_CX,LPI_MX , LPI_CX , MX
 * @param  core_rail_mode : 6 = auto , 7 = LPM
 * @param  active_ret : Kind of request i.e Active or retention
 * @return PMIC error code.
 *
 * @sideeffects This call will update active/retention voltage in spare register which also done by PBS Routine .
 *              This will overwritten by PBS Seq.
 *
 */
pm_err_flag_type pm_app_rpmh_core_rail_set_mode( pm_core_rail_name rail ,uint64 core_rail_mode , pm_request_type active_ret);


/**
 * @name pm_app_rpmh_core_rail_get_voltage
 *
 * @brief This function calls the target Specifc PMIC calls to read
 *        the Active/Retention Voltage of WCSS/MX rail to SDAM Spare Register
 *
 * @param  rail: WCSS_MX , WCSS_CX,LPI_MX , LPI_CX , MX
 * @param  active_ret : Kind of request i.e Active or retention
 * @return PMIC error code.
 *
 * @sideeffects This call will update active/retention voltage in spare register which also done by PBS Routine .
 *              This will overwritten by PBS Seq.
 *
 */
pm_err_flag_type pm_app_rpmh_core_rail_get_voltage( pm_core_rail_name rail , pm_request_type active_ret , uint64 *voltage_uV);

/**
 * @name pm_app_rpmh_core_rail_get_mode
 *
 * @brief This function calls the target Specifc PMIC calls to read
 *        the Active/Retention Mode of WCSS/MX rail to SDAM Spare Register
 *
 * @param  rail: WCSS_MX , WCSS_CX,LPI_MX , LPI_CX , MX
 * @param  active_ret : Kind of request i.e Active or retention
 * @return PMIC error code.
 *
 * @sideeffects This call will update active/retention voltage in spare register which also done by PBS Routine .
 *              This will overwritten by PBS Seq.
 *
 */

pm_err_flag_type pm_app_rpmh_core_rail_get_mode( pm_core_rail_name rail , pm_request_type active_ret , uint64 *core_rail_mode);


#endif /* PM_APP_PWR_H */
