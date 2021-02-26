#ifndef PM_NPA_DEVICE_LDO__H
#define PM_NPA_DEVICE_LDO__H
/*===========================================================================


P M    N P A   L D O   D E V I C E    H E A D E R    F I L E

DESCRIPTION
This file contains prototype definitions npa device layer

Copyright (c) 2012-2013, 2015 by QUALCOMM, Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/13/14   va      Updating HDMI EXT NPA node and PCIE 
01/16/13   umr     Add Suppressible request support.
02/12/12   wra     KVP based device setting initial file 
===========================================================================*/

/*===========================================================================

INCLUDE FILES

===========================================================================*/

#include "pm_npa_device.h"


/*===========================================================================

TYPE DEFINITIONS

===========================================================================*/
typedef struct  
{
    // [Disable (default), Enable] — max aggregation (left to right)
    unsigned sw_enable_key; // PM_NPA_KEY_SOFTWARE_ENABLE
    unsigned sw_enable_size; // 4
    unsigned sw_enable_value;

    // [IPEAK (default), NPM] - max aggregation (left to right)
    unsigned ldo_sw_mode_key; // PM_NPA_KEY_LDO_SOFTWARE_MODE
    unsigned ldo_sw_mode_size; // 4
    unsigned ldo_sw_mode_value;

    //	[NONE, EN1, EN2, EN3, EN4] - ORed value of list
    unsigned pin_ctrl_enable_key; // PM_NPA_KEY_PIN_CTRL_ENABLE
    unsigned pin_ctrl_enable_size; // 4
    unsigned pin_ctrl_enable_value;

    //	[NONE, EN1, EN2, EN3, EN4, SLEEPB] - ORed value of list
    unsigned pin_ctrl_power_mode_key; // PM_NPA_KEY_PIN_CTRL_POWER_MODE
    unsigned pin_ctrl_power_mode_size;
    unsigned pin_ctrl_power_mode_value;

    // [X uV] — max aggregation
    unsigned uv_key; // PM_NPA_KEY_MICRO_VOLT
    unsigned uv_size; // 4
    unsigned uv_value;

    // 	[X mA] — max aggregation
    unsigned current_key; // PM_NPA_KEY_CURRENT
    unsigned current_size; // 4
    unsigned current_value;

    // [x uV] - max aggregation
    unsigned head_room_voltage_key; // PM_NPA_KEY_HEAD_ROOM
    unsigned head_room_voltage_size; // 4
    unsigned head_room_voltage_value;

    // [ALLOWED, DISALLOWED] - MAX value left to right
    unsigned bypass_allowed_key; // PM_NPA_KEY_BYPASS_ALLOWED_KEY
    unsigned bypass_allowed_size; // 4
    unsigned bypass_allowed_value;

    // [NONE, MODE1, MODE2, MODE3, MODE4, MODE5] - MAX value left to right
    unsigned corner_key; // PM_NPA_KEY_CORNER_VOLTAGE_KEY
    unsigned corner_size; // 4
    unsigned corner_value;

    // [NONE, MODE1, MODE2, MODE3, MODE4, MODE5] - MAX value left to right
    unsigned floor_key; // PM_NPA_KEY_ACTIVE_FLOOR
    unsigned floor_size; // 4
    unsigned floor_value;

    // [NONE, MODE1, MODE2, MODE3, MODE4, MODE5] - MAX value left to right
    unsigned corner_voltage_level; // PM_NPA_KEY_ACTIVE_FLOOR
    unsigned corner_voltage_size; // 4
    unsigned corner_voltage_value;

    // [NONE, MODE1, MODE2, MODE3, MODE4, MODE5] - MAX value left to right
    unsigned corner_voltage_floor_level; // PM_NPA_KEY_ACTIVE_FLOOR
    unsigned corner_voltage_floor_size; // 4
    unsigned corner_voltage_floor_value;
}pm_npa_ldo_kvps;

/*
 * Helper function for retrieving the LDO KVPS plugin.
 */
npa_resource_plugin *pm_npa_get_ldo_kvps_plugin(void);

#ifdef PMIC_OFFTARGET_TESTING
#define PMIC_DEV_RMT_RSRC_LDO( chipid, rtype, index, rpm_name_index )   \
{                                                                       \
    PMIC_DEV_RSRC_NAME_VEC_IN( chipid, rtype, index ),                  \
    rpm_name_index,                                                     \
    PMIC_NPA_RMT_PROTOCOL_RPM_TYPE,                                     \
    &pm_npa_ldo_kvps_plugin,                                            \
    npa_remote_resource_local_aggregation_driver_fcn,                   \
    "kvps",                                                             \
    sizeof(pm_npa_ldo_kvps)/sizeof(npa_resource_state),                 \
    NPA_RESOURCE_VECTOR_STATE | NPA_RESOURCE_SUPPORTS_SUPPRESSIBLE      \
}
#else
    #define PMIC_DEV_RMT_RSRC_LDO( chipid, rtype, index, rpm_name_index )    \
    {                                                                        \
        PMIC_DEV_RSRC_NAME_VEC_IN( chipid, rtype, index ),                   \
        rpm_name_index,                                                      \
        PMIC_NPA_RMT_PROTOCOL_RPM_TYPE,                                      \
        &pm_npa_ldo_kvps_plugin,                                             \
        npa_remote_resource_local_aggregation_no_initial_request_driver_fcn, \
        "kvps",                                                              \
        sizeof(pm_npa_ldo_kvps)/sizeof(npa_resource_state),                  \
        NPA_RESOURCE_VECTOR_STATE | NPA_RESOURCE_SUPPORTS_SUPPRESSIBLE       \
    }
#endif

pm_err_flag_type
pm_npa_ldo_kvps_init (pm_npa_ldo_kvps *kvps);

#endif // PM_NPA_DEVICE_LDO__H
