/**
 * @file:  cprf.h
 * @brief:
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 *
 */

#ifndef CPRF_H
#define CPRF_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#include "comdef.h"

typedef enum
{
    CPR_RETURN_SUCCESS = 0,
    CPR_RETURN_ERROR,
} cpr_return_status;

/**
 * <!-- cpr_enable_controller -->
 *
 * @brief API to enable/disable CPR Controller.
 *
 * API to enable/disable the RBCPR controller associated with a rail.
 *
 * @param id     : CPRF id ( = ARC id) used to identify the controller
 *                 to be enabled/disabled.
 * @param enable : TRUE to enable, FALSE to disable the controller.
 *
 * @return CPR_SUCCESS or CPR_ERROR if the 'id' does not match
 *         a controller.
 */
cpr_return_status cpr_enable_controller(uint8 id, boolean enable);

/**
 * <!-- cpr_enable_controller_force -->
 *
 * @brief API to enable/disable CPR Controller.
 *
 * API to enable/disable the RBCPR controller associated with a rail.
 * Will toggle the controller irrespective of what mode the controller
 * was configured to at init.
 *
 * @param id     : CPRF id ( = ARC id) used to identify the controller
 *                 to be enabled/disabled.
 * @param enable : TRUE to enable, FALSE to disable the controller.
 *
 * @return CPR_SUCCESS or CPR_ERROR if the 'id' does not match
 *         a controller.
 */
cpr_return_status cpr_enable_controller_force(uint8 id, boolean enable);

/**
 * <!-- cpr_toggle_cprf_interface_en -->
 *
 * @brief API to toggle CPR Controller CPRF_INTERFACE_ENABLE bit.
 *
 * @param id     : CPRF id ( = ARC id) used to identify the controller
 *                 to be enabled/disabled.
 * @param enable : TRUE to assert, FALSE to de-assert cprf_interface_en
 * bit at the specified controller.
 *
 * @return CPR_SUCCESS or CPR_ERROR if the 'id' does not match
 * any of the rpmh managed (Closed-loop capable) rail's controller.
 */
cpr_return_status cpr_toggle_cprf_interface_en(uint8 id, boolean enable);

/**
 * <!-- cpr_get_ret_voltage -->
 *
 * @brief API to get the retention voltage of a given rail.
 *
 * @param id     : CPRF id ( = ARC id) used to identify the rail using the
 *                 rail whos retention voltage needs to be retrieved.
 * @return       : Retention Voltage of the rail. Returns SAFE_VOLTAGE which
 *                 is the same as VMAX and VMIN for Retention.
 */
uint32 cpr_get_ret_voltage (uint8 id);

/**
 * <!-- cpr_set_ret_voltage -->
 *
 * @brief API to set the retention voltage of a given rail.
 *
 * @param id     : CPRF id ( = ARC id) used to identify the rail using the
 *                 rail whos retention voltage needs to be set.
 *                 Voltage to set the retention level to for the given rail.
 * @return       : CPR_RETURN_SUCCESS on success.
 */

cpr_return_status cpr_set_ret_voltage (uint8 id, uint32 vol);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //CPRF_H
