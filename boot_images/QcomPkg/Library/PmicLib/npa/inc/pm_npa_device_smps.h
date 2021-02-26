#ifndef PM_NPA_DEVICE_SMPS__H
#define PM_NPA_DEVICE_SMPS__H
/*===========================================================================


P M    N P A   S M P S   D E V I C E    H E A D E R    F I L E

DESCRIPTION
This file contains prototype definitions npa device layer

Copyright (c) 2012-2013, 2015 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
===========================================================================*/

/*===========================================================================

EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/04/14   viveka  Add floor key.
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

    // [AUTO (default), IPEAK, NPM] - max aggregation (left to right)
    unsigned smps_sw_mode_key; // PM_NPA_KEY_SMPS_SOFTWARE_MODE
    unsigned smps_sw_mode_size; // 4
    unsigned smps_sw_mode_value;

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

    // 	[X uA] — max aggregation
    unsigned current_key; // PM_NPA_KEY_CURRENT
    unsigned current_size; // 4
    unsigned current_value;

    // [xx MHz] — max within a priority group
    // — Allows one execution environments vote to be higher or lower priority relative to another execution environment
    unsigned frequency_key; // PM_NPA_KEY_FREQUENCY
    unsigned frequency_size; // 4
    unsigned frequency_value;

    //Frequency_Reason — Optional	[xx] — Distinct indicators per execution environment
    //    One execution environment will not be allowed to use the indicators of another
    //    - Allows interspersed prioritization between execution environments for
    //    frequency aggregation
    //    — Ex: Freq4 BT -> Freq4 GPS -> Freq4 WLAN -> Freq 4 WAN (lowest to highest priority
    unsigned frequency_reason_key; // PM_NPA_KEY_FREQUENCY_REASON
    unsigned frequency_reason_size; // 4
    unsigned frequency_reason_value;

    // [None, Quiet, Super Quiet] — max aggregation (left to right)  
    // Mask that allows SMPS to follow global quiet mode signal
    unsigned follow_quiet_mode_key; // PM_NPA_KEY_FOLLOW_QUIET_MODE
    unsigned follow_quiet_mode_size; // 4
    unsigned follow_quiet_mode_value;

    // [ALLOWED, DISALLOWED] - MAX value left to right
    unsigned bypass_allowed_key; // PM_NPA_KEY_BYPASS_ALLOWED_KEY
    unsigned bypass_allowed_size; // 4
    unsigned bypass_allowed_value;

    // [NONE, MODE1, MODE2, MODE3, MODE4, MODE5] - MAX value left to right
    unsigned corner_key; // PM_NPA_KEY_CORNER_VOLTAGE_KEY
    unsigned corner_size; // 4
    unsigned corner_value;

    // [x uV] - max aggregation
    unsigned head_room_voltage_key; // PM_NPA_KEY_HEAD_ROOM
    unsigned head_room_voltage_size; // 4
    unsigned head_room_voltage_value;

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

}pm_npa_smps_kvps;

npa_resource_plugin *pm_npa_get_smps_kvps_plugin(void);

extern unsigned pm_mx_corners[6];
extern unsigned pm_cx_corners[6];

#ifdef PMIC_OFFTARGET_TESTING
#define PMIC_DEV_RMT_RSRC_SMPS( chipid, rtype, index, rpm_name_index )  \
{                                                                       \
    PMIC_DEV_RSRC_NAME_VEC_IN( chipid, rtype, index ),                  \
    rpm_name_index,                                                     \
    PMIC_NPA_RMT_PROTOCOL_RPM_TYPE,                                     \
    &pm_npa_smps_kvps_plugin,                                           \
    npa_remote_resource_local_aggregation_driver_fcn,                   \
    "kvps",                                                             \
    sizeof(pm_npa_smps_kvps)/sizeof(npa_resource_state),                \
    NPA_RESOURCE_VECTOR_STATE | NPA_RESOURCE_SUPPORTS_SUPPRESSIBLE      \
}
#else
    #define PMIC_DEV_RMT_RSRC_SMPS( chipid, rtype, index, rpm_name_index )   \
    {                                                                        \
        PMIC_DEV_RSRC_NAME_VEC_IN( chipid, rtype, index ),                   \
        rpm_name_index,                                                      \
        PMIC_NPA_RMT_PROTOCOL_RPM_TYPE,                                      \
        &pm_npa_smps_kvps_plugin,                                            \
        npa_remote_resource_local_aggregation_no_initial_request_driver_fcn, \
        "kvps",                                                              \
        sizeof(pm_npa_smps_kvps)/sizeof(npa_resource_state),                 \
        NPA_RESOURCE_VECTOR_STATE | NPA_RESOURCE_SUPPORTS_SUPPRESSIBLE       \
    }
#endif

pm_err_flag_type
pm_npa_smps_kvps_init (pm_npa_smps_kvps *kvps);

#endif // PM_NPA_DEVICE_SMPS__H