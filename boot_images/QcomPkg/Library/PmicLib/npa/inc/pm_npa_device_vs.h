#ifndef PM_NPA_DEVICE_VS__H
#define PM_NPA_DEVICE_VS__H
/*===========================================================================


P M    N P A   V S   D E V I C E    H E A D E R    F I L E

DESCRIPTION
This file contains prototype definitions npa device layer

Copyright (c) 2012-2013 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
===========================================================================*/

/*===========================================================================

EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
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
    // [Disable, Enable] — max aggregation (left to right)
    unsigned sw_enable_key; // PM_NPA_KEY_SOFTWARE_ENABLE
    unsigned sw_enable_size; // 4
    unsigned sw_enable_value;

    //	[NONE, EN1, EN2, EN3, EN4] - ORed value of list
    unsigned pin_ctrl_enable_key; // PM_NPA_KEY_PIN_CTRL_ENABLE
    unsigned pin_ctrl_enable_size; // 4
    unsigned pin_ctrl_enable_value;

    // [X uV] — max aggregation
    unsigned uv_key; // PM_NPA_KEY_MICRO_VOLT
    unsigned uv_size; // 4
    unsigned uv_value;

    // 	[X uA] — max aggregation
    unsigned current_key; // PM_NPA_KEY_CURRENT
    unsigned current_size; // 4
    unsigned current_value;
}pm_npa_vs_kvps;


/*
 * Helper function for retrieving the vs KVPS plugin.
 */
npa_resource_plugin *pm_npa_get_vs_kvps_plugin(void);

#ifdef PMIC_OFFTARGET_TESTING
#define PMIC_DEV_RMT_RSRC_VS( chipid, rtype, index, rpm_name_index )    \
{                                                                       \
    PMIC_DEV_RSRC_NAME_VEC_IN( chipid, rtype, index ),                  \
    rpm_name_index,                                                     \
    PMIC_NPA_RMT_PROTOCOL_RPM_TYPE,                                     \
    &pm_npa_vs_kvps_plugin,                                             \
    npa_remote_resource_local_aggregation_driver_fcn,                   \
    "kvps",                                                             \
    sizeof(pm_npa_vs_kvps)/sizeof(npa_resource_state),                  \
    NPA_RESOURCE_VECTOR_STATE | NPA_RESOURCE_SUPPORTS_SUPPRESSIBLE      \
}
#else
#define PMIC_DEV_RMT_RSRC_VS( chipid, rtype, index, rpm_name_index )     \
{                                                                        \
    PMIC_DEV_RSRC_NAME_VEC_IN( chipid, rtype, index ),                   \
    rpm_name_index,                                                      \
    PMIC_NPA_RMT_PROTOCOL_RPM_TYPE,                                      \
    &pm_npa_vs_kvps_plugin,                                              \
    npa_remote_resource_local_aggregation_no_initial_request_driver_fcn, \
    "kvps",                                                              \
    sizeof(pm_npa_vs_kvps)/sizeof(npa_resource_state),                   \
    NPA_RESOURCE_VECTOR_STATE | NPA_RESOURCE_SUPPORTS_SUPPRESSIBLE       \
}
#endif


pm_err_flag_type
pm_npa_vs_kvps_init (pm_npa_vs_kvps *kvps);

#endif // PM_NPA_DEVICE_VS__H