/*! \file  pm_config_ssc_npa_pam.c
 *  
 *  \brief  File Contains the PMIC NPA CMI Code
 *  \details  This file contains the needed definition and enum for PMIC NPA layer.
 *  
 *    PMIC code generation Version: 1.0.0.0
 *    PMIC code generation Locked Version: SSC_MSM8998_PMI8998_0p3 - LOCKED

 *    This file contains code for Target specific settings and modes.
 *  
 *  &copy; Copyright 2017 Qualcomm Technologies, All Rights Reserved
 */
/*===========================================================================

                EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module over time.

$Header: //components/rel/core.qdsp6/2.1/settings/pmic/pm/config/slpi/sdm845/pm_config_pam.c#1 $ 
$DateTime: 2017/07/21 22:10:47 $  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/07/17   akm     Updated PAM changes
===========================================================================*/

/*===========================================================================

                        INCLUDE HEADER FILES

===========================================================================*/

#include "pmapp_npa.h"
#include "pm_prm_device_defs.h"
#include "npa_remote_resource.h"

/*===========================================================================

                        MACRO DEFINITIONS

===========================================================================*/

#define PMIC_NPA_CLIENT_NODE_SENSOR_VDD     "/node/pmic/client/sensor_vdd"
#define PMIC_NPA_CLIENT_NODE_SENSOR_VDD_2   "/node/pmic/client/sensor_vdd_2"
#define PMIC_NPA_CLIENT_NODE_SENSOR_VDDIO   "/node/pmic/client/sensor_vddio"
#define PMIC_NPA_CLIENT_NODE_SENSOR_VDDIO_2 "/node/pmic/client/sensor_vddio_2"
#define PMIC_NPA_CLIENT_NODE_TOUCH_SCREEN   "/node/pmic/client/touch_screen"

/*===========================================================================

                        VARIABLES DEFINITION

===========================================================================*/

/* SENSOR_VDD Client */
pm_prm_vreg_data_type
pm_pam_sensor_vdd_a_ldo19 [] =
{
    /* mode                 voltage(mV)    hr(mV)   en */
    {PM_PRM_VREG_MODE_LPM,       0,          0,     PM_PRM_DEV_DISABLE},    // Mode: SENSOR_POWER_OFF
    {PM_PRM_VREG_MODE_LPM,    3008,          0,     PM_PRM_DEV_ENABLE},     // Mode: SENSOR_LPM
    {PM_PRM_VREG_MODE_NPM,    3008,          0,     PM_PRM_DEV_ENABLE},     // Mode: SENSOR_POWER_ON

};

pm_prm_pam_client_cfg_type
pm_pam_sensor_vdd_rails_info [] =
{
   {(void*)pm_pam_sensor_vdd_a_ldo19, PM_PRM_RSRC_VRM_VREG},
};

/* SENSOR_VDD_2 Client */
pm_prm_vreg_data_type
pm_pam_sensor_vdd_2_a_ldo19 [] =
{
    /* mode                 voltage(mV)    hr(mV)   en */
    {PM_PRM_VREG_MODE_LPM,       0,          0,     PM_PRM_DEV_DISABLE},      // Mode: SENSOR_POWER_OFF
    {PM_PRM_VREG_MODE_LPM,    3008,          0,     PM_PRM_DEV_ENABLE},       // Mode: SENSOR_LPM
    {PM_PRM_VREG_MODE_NPM,    3008,          0,     PM_PRM_DEV_ENABLE},       // Mode: SENSOR_POWER_ON

};

pm_prm_pam_client_cfg_type
pm_pam_sensor_vdd_2_rails_info [] =
{
    {(void*)pm_pam_sensor_vdd_2_a_ldo19, PM_PRM_RSRC_VRM_VREG} ,
};

/* SENSOR_VDDIO Client */
pm_prm_vreg_data_type
pm_pam_sensor_vddio_a_vs2 [] =
{
    /* mode volt hr enable */
    {0, 0, 0, PM_PRM_DEV_DISABLE},      // Mode: SENSOR_POWER_OFF
    {0, 0, 0, PM_PRM_DEV_ENABLE},       // Mode: SENSOR_LPM
    {0, 0, 0, PM_PRM_DEV_ENABLE},       // Mode: SENSOR_POWER_ON

};

pm_prm_pam_client_cfg_type
pm_pam_sensor_vddio_rails_info [] =
{
    {(void*)pm_pam_sensor_vddio_a_vs2, PM_PRM_RSRC_VRM_VREG},
};

/* SENSOR_VDDIO_2 Client */
pm_prm_vreg_data_type
pm_pam_sensor_vddio_2_a_vs2 [] =
{
    /* mode volt hr enable */
    {0, 0, 0, PM_PRM_DEV_DISABLE},      // Mode: SENSOR_POWER_OFF
    {0, 0, 0, PM_PRM_DEV_ENABLE},       // Mode: SENSOR_LPM
    {0, 0, 0, PM_PRM_DEV_ENABLE},       // Mode: SENSOR_POWER_ON
};

pm_prm_pam_client_cfg_type
pm_pam_sensor_vddio_2_rails_info [] =
{
    {(void*)pm_pam_sensor_vddio_2_a_vs2, PM_PRM_RSRC_VRM_VREG},
};

/* TOUCH_SCREEN Client */
pm_prm_vreg_data_type
pm_pam_touch_screen_a_ldo28 [] =
{
    {PM_PRM_VREG_MODE_LPM, 3008,   0, PM_PRM_DEV_DISABLE},  // TOUCH_OFF
    {PM_PRM_VREG_MODE_LPM, 3008,   0, PM_PRM_DEV_ENABLE},   // TOUCH_LPM
    {PM_PRM_VREG_MODE_NPM, 3008,   0, PM_PRM_DEV_ENABLE},   // TOUCH_ACTIVE
};

pm_prm_vreg_data_type
pm_pam_touch_screen_a_smps4 [] =
{
    {PM_PRM_VREG_MODE_RETENTION,  1800, 0, PM_PRM_DEV_ENABLE},    // TOUCH_OFF
    {PM_PRM_VREG_MODE_LPM,        1800, 0, PM_PRM_DEV_ENABLE},    // TOUCH_LPM
    {PM_PRM_VREG_MODE_AUTO,       1800, 0, PM_PRM_DEV_ENABLE},    // TOUCH_ACTIVE
};

pm_prm_pam_client_cfg_type
pm_pam_touch_screen_rails_info [] = 
{
    {(void*)pm_pam_touch_screen_a_ldo28, PM_PRM_RSRC_VRM_VREG},
    {(void*)pm_pam_touch_screen_a_smps4, PM_PRM_RSRC_VRM_VREG},
}; 

/*************************** NODE & DEPENDENCY DEFINITIONS ***************************/

/* SENSOR_VDD Client */
npa_node_dependency
pm_pam_deps_sensor_vdd [] =
{
    PM_PRM_CLIENT_VREG_DEP( a, ldo, 19 ),
};

/* SENSOR_VDD_2 Client */
npa_node_dependency
pm_pam_deps_sensor_vdd_2 [] =
{
    PM_PRM_CLIENT_VREG_DEP( a, ldo, 19 ),
};

/* SENSOR_VDDIO Client */
npa_node_dependency
pm_pam_deps_sensor_vddio [] =
{
    PM_PRM_CLIENT_VREG_DEP( a, vs, 2 ),
};

/* SENSOR_VDDIO_2 Client */
npa_node_dependency
pm_pam_deps_sensor_vddio_2 [] =
{
    PM_PRM_CLIENT_VREG_DEP( a, vs, 2 ),
};

/* TOUCH_SCREEN Client*/
npa_node_dependency
pm_pam_deps_touch_screen [] =
{
    PM_PRM_CLIENT_VREG_DEP ( a,  ldo,  28 ),
    PM_PRM_CLIENT_VREG_DEP( a,  smp,  4 ),
};

/* NPA NODE RESOURCE */
pm_prm_node_resource_info
pm_prm_pam_rsrcs [] =
{
    {
        PMIC_NPA_GROUP_ID_SENSOR_VDD,                 // Resource Name
        PMIC_NPA_MODE_ID_SENSOR_POWER_ON,             // Maximum Value
        NPA_RESOURCE_SINGLE_CLIENT,                   // Resource Attribute
        (void*) pm_pam_sensor_vdd_rails_info,         // Resource User Data
        PMIC_NPA_CLIENT_NODE_SENSOR_VDD,              // Node Name
        NPA_NODE_DEFAULT,                             // Node Attributes
        NULL, 
        pm_pam_deps_sensor_vdd,                       // Node Dependency
        NPA_ARRAY_SIZE(pm_pam_deps_sensor_vdd),       // Dependency Count
    },     
    {      
        PMIC_NPA_GROUP_ID_SENSOR_VDD_2,               // Resource Name
        PMIC_NPA_MODE_ID_SENSOR_POWER_ON,             // Maximum Value
        NPA_RESOURCE_SINGLE_CLIENT,                   // Resource Attribute
        (void*) pm_pam_sensor_vdd_2_rails_info,       // Resource User Data
        PMIC_NPA_CLIENT_NODE_SENSOR_VDD_2,            // Node Name
        NPA_NODE_DEFAULT,                             // Node Attributes
        NULL, 
        pm_pam_deps_sensor_vdd_2,                     // Node Dependency
        NPA_ARRAY_SIZE(pm_pam_deps_sensor_vdd_2),     // Dependency Count
    },     
    {      
        PMIC_NPA_GROUP_ID_SENSOR_VDDIO,               // Resource Name
        PMIC_NPA_MODE_ID_SENSOR_POWER_ON,             // Maximum Value
        NPA_RESOURCE_SINGLE_CLIENT,                   // Resource Attribute
        (void*) pm_pam_sensor_vddio_rails_info,       // Resource User Data
        PMIC_NPA_CLIENT_NODE_SENSOR_VDDIO,            // Node Name
        NPA_NODE_DEFAULT,                             // Node Attributes
        NULL, 
        pm_pam_deps_sensor_vddio,                     // Node Dependency
        NPA_ARRAY_SIZE(pm_pam_deps_sensor_vddio),     // Dependency Count
    },     
    {      
        PMIC_NPA_GROUP_ID_SENSOR_VDDIO_2,             // Resource Name
        PMIC_NPA_MODE_ID_SENSOR_POWER_ON,             // Maximum Value
        NPA_RESOURCE_SINGLE_CLIENT,                   // Resource Attribute
        (void*) pm_pam_sensor_vddio_2_rails_info,     // Resource User Data
        PMIC_NPA_CLIENT_NODE_SENSOR_VDDIO_2,          // Node Name
        NPA_NODE_DEFAULT,                             // Node Attributes
        NULL, 
        pm_pam_deps_sensor_vddio_2,                   // Node Dependency
        NPA_ARRAY_SIZE(pm_pam_deps_sensor_vddio_2),   // Dependency Count
    },   
    {    
        PMIC_NPA_GROUP_ID_TSCREEN,                    // Resource Name
        PMIC_NPA_MODE_ID_TOUCH_SCREEN_MODE_2,   // Maximum Value
        (NPA_RESOURCE_SINGLE_CLIENT |                 
        NPA_RESOURCE_DRIVER_UNCONDITIONAL),           // Resource Attributes
        (void*) pm_pam_touch_screen_rails_info,       // Resource User Data
        PMIC_NPA_CLIENT_NODE_TOUCH_SCREEN,            // Node Name
        NPA_NODE_DEFAULT,                             // Node Attributes
        NULL,                                         
        pm_pam_deps_touch_screen,                     // Node Dependency
        NPA_ARRAY_SIZE(pm_pam_deps_touch_screen),     // Dependency Count 
    },
};

uint32 num_of_pam_rsrcs [] = 
{sizeof(pm_prm_pam_rsrcs)/sizeof(pm_prm_node_resource_info)};

/*Device Layer Resources*/
pm_prm_rsrc_data_type
pm_prm_device_rsrc_data [] =
{
    PM_PRM_DEV_RSRC_VREG ( a,  ldo,  19, PM_PRM_RSRC_LDO,  PM_PRM_RPMH, PM_LDO_19), 
    PM_PRM_DEV_RSRC_VREG ( a,  ldo,  28, PM_PRM_RSRC_LDO,  PM_PRM_RPMH, PM_LDO_28),
    PM_PRM_DEV_RSRC_VREG ( a,  vs,    2, PM_PRM_RSRC_VS,   PM_PRM_RPMH, PM_VS_LVS_2), 
    PM_PRM_DEV_RSRC_VREG ( a,  smp,   4, PM_PRM_RSRC_SMPS, PM_PRM_RPMH, PM_SMPS_4),
};

pm_prm_resource_type
pm_prm_device_rsrcs [] =
{
    {
        pm_prm_device_rsrc_data, 
        sizeof(pm_prm_device_rsrc_data)/sizeof(pm_prm_rsrc_data_type), //Number of Resources 
        5,                                                             //Number of Nodes
    }
};

/* Published resource */
char *pm_prm_pub_rsrc_names[] =
{  
    PMIC_NPA_GROUP_ID_SENSOR_VDD,
    PMIC_NPA_GROUP_ID_SENSOR_VDD_2,
    PMIC_NPA_GROUP_ID_SENSOR_VDDIO,
    PMIC_NPA_GROUP_ID_SENSOR_VDDIO_2,
    PMIC_NPA_GROUP_ID_TSCREEN,
};

pm_prm_pam_rsrc_pub_pd_type  
pm_prm_pub_pam_rsrcs = 
{
   NPA_ALL_PDS,
   pm_prm_pub_rsrc_names,
   5
};

