
/*! \file  pm_config_pam.c
 *  
 *  \brief  File Contains the PMIC NPA PAM Data
 *  \details  This file contains the PAM data for the PMIC NPA layer.
 *  
 *    Platform: SDM670.XX.1.0
 *    Revision: SDM670.XX.1.0-SENSORS-00002
 *    Template Version: 
 *    $Header: //components/rel/core.qdsp6/2.1/settings/pmic/pm/config/adsp/sdm670/pm_config_pam.c#1 $ 
 *    $DateTime: 2017/07/21 22:10:47 $  $Author: pwbldsvc $
 *  
 *  &copy; Copyright May 22, 2017 Qualcomm Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                        INCLUDE HEADER FILES

===========================================================================*/

#include "pmapp_npa.h"
#include "pm_prm_device_defs.h"
#include "npa_remote_resource.h"

/*===========================================================================

                        MACRO DEFINITIONS

===========================================================================*/

#define PMIC_NPA_CLIENT_NODE_SENSOR_VDD "/node/pmic/client/sensor_vdd"
#define PMIC_NPA_CLIENT_NODE_SENSOR_VDDIO "/node/pmic/client/sensor_vddio"
#define PMIC_NPA_CLIENT_NODE_TOUCH_SCREEN "/node/pmic/client/touch_screen"
#define PMIC_NPA_CLIENT_NODE_TOUCH_SCREEN1 "/node/pmic/client/touch_screen1"
/*===========================================================================

                        VARIABLES DEFINITION

===========================================================================*/


/*************************** SENSOR_VDD Client ***************************/

const pm_prm_vreg_data_type
pm_pam_sensor_vdd_b_ldo3 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_LPM, 0, 0, PM_PRM_DEV_DISABLE },  // SENSOR_POWER_OFF
    { PM_PRM_VREG_MODE_LPM, 3008, 0, PM_PRM_DEV_ENABLE },  // SENSOR_LPM
    { PM_PRM_VREG_MODE_NPM, 3008, 0, PM_PRM_DEV_ENABLE }  // SENSOR_POWER_ON
};

const pm_prm_pam_client_cfg_type
pm_pam_sensor_vdd_rails_info [] = 
{
    { (void*)pm_pam_sensor_vdd_b_ldo3, PM_PRM_RSRC_VRM_VREG },
}; 

npa_node_dependency
pm_pam_deps_sensor_vdd [] =
{
    PM_PRM_CLIENT_VREG_DEP ( b,  ldo, 3 ),
};

/*************************** SENSOR_VDDIO Client ***************************/

const pm_prm_vreg_data_type
pm_pam_sensor_vddio_a_ldo14 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_LPM, 0, 0, PM_PRM_DEV_DISABLE },  // SENSOR_POWER_OFF
    { PM_PRM_VREG_MODE_LPM, 1800, 0, PM_PRM_DEV_ENABLE },  // SENSOR_LPM
    { PM_PRM_VREG_MODE_NPM, 1800, 0, PM_PRM_DEV_ENABLE }  // SENSOR_POWER_ON
};

const pm_prm_pam_client_cfg_type
pm_pam_sensor_vddio_rails_info [] = 
{
    { (void*)pm_pam_sensor_vddio_a_ldo14, PM_PRM_RSRC_VRM_VREG },
}; 

npa_node_dependency
pm_pam_deps_sensor_vddio [] =
{
    PM_PRM_CLIENT_VREG_DEP ( a,  ldo, 14 ),
};

/*************************** TOUCH_SCREEN Client ***************************/

const pm_prm_vreg_data_type
pm_pam_touch_screen_a_ldo13 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_LPM, 0, 0, PM_PRM_DEV_DISABLE },  // TOUCH_mode_OFF
    { PM_PRM_VREG_MODE_LPM, 1800, 0, PM_PRM_DEV_ENABLE },  // TOUCH_mode_LPM
    { PM_PRM_VREG_MODE_NPM, 1800, 0, PM_PRM_DEV_ENABLE }  // TOUCH_mode_ACTIVE
};

const pm_prm_pam_client_cfg_type
pm_pam_touch_screen_rails_info [] = 
{
    { (void*)pm_pam_touch_screen_a_ldo13, PM_PRM_RSRC_VRM_VREG },
}; 

npa_node_dependency
pm_pam_deps_touch_screen [] =
{
    PM_PRM_CLIENT_VREG_DEP ( a,  ldo, 13 ),
};

/*************************** TOUCH_SCREEN1 Client ***************************/

const pm_prm_vreg_data_type
pm_pam_touch_screen1_b_ldo3 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_LPM, 0, 0, PM_PRM_DEV_DISABLE },  // TOUCH_mode_OFF
    { PM_PRM_VREG_MODE_LPM, 3008, 0, PM_PRM_DEV_ENABLE },  // TOUCH_mode_LPM
    { PM_PRM_VREG_MODE_NPM, 3008, 0, PM_PRM_DEV_ENABLE }  // TOUCH_mode_ACTIVE
};

const pm_prm_pam_client_cfg_type
pm_pam_touch_screen1_rails_info [] = 
{
    { (void*)pm_pam_touch_screen1_b_ldo3, PM_PRM_RSRC_VRM_VREG },
}; 

npa_node_dependency
pm_pam_deps_touch_screen1 [] =
{
    PM_PRM_CLIENT_VREG_DEP ( b,  ldo, 3 ),
};


/*************************** NPA NODE RESOURCE ***************************/

const pm_prm_node_resource_info
pm_prm_pam_rsrcs [] =
{
   {
     PMIC_NPA_GROUP_ID_SENSOR_VDD,  // Resource Name
    PMIC_NPA_MODE_ID_SENSOR_POWER_ON,   // Maximum Value
    NPA_RESOURCE_SINGLE_CLIENT | NPA_RESOURCE_DRIVER_UNCONDITIONAL, // Resource Attribute
    (void*) pm_pam_sensor_vdd_rails_info, // Resource User Data
    PMIC_NPA_CLIENT_NODE_SENSOR_VDD, // Node Name
    NPA_NODE_DEFAULT,  // Node Attributes
    NULL,
    pm_pam_deps_sensor_vdd,  // Node Dependency
    NPA_ARRAY_SIZE(pm_pam_deps_sensor_vdd),  // Deps Count 
   },
   {
     PMIC_NPA_GROUP_ID_SENSOR_VDDIO,  // Resource Name
    PMIC_NPA_MODE_ID_SENSOR_POWER_ON,   // Maximum Value
    NPA_RESOURCE_SINGLE_CLIENT | NPA_RESOURCE_DRIVER_UNCONDITIONAL, // Resource Attribute
    (void*) pm_pam_sensor_vddio_rails_info, // Resource User Data
    PMIC_NPA_CLIENT_NODE_SENSOR_VDDIO, // Node Name
    NPA_NODE_DEFAULT,  // Node Attributes
    NULL,
    pm_pam_deps_sensor_vddio,  // Node Dependency
    NPA_ARRAY_SIZE(pm_pam_deps_sensor_vddio),  // Deps Count 
   },
   {
     PMIC_NPA_GROUP_ID_TSCREEN,  // Resource Name
    PMIC_NPA_MODE_ID_TOUCH_SCREEN_MODE_2,   // Maximum Value
    NPA_RESOURCE_SINGLE_CLIENT | NPA_RESOURCE_DRIVER_UNCONDITIONAL, // Resource Attribute
    (void*) pm_pam_touch_screen_rails_info, // Resource User Data
    PMIC_NPA_CLIENT_NODE_TOUCH_SCREEN, // Node Name
    NPA_NODE_DEFAULT,  // Node Attributes
    NULL,
    pm_pam_deps_touch_screen,  // Node Dependency
    NPA_ARRAY_SIZE(pm_pam_deps_touch_screen),  // Deps Count 
   },
   {
     PMIC_NPA_GROUP_ID_TSCREEN1,  // Resource Name
    PMIC_NPA_MODE_ID_TOUCH_SCREEN_MODE_2,   // Maximum Value
    NPA_RESOURCE_SINGLE_CLIENT | NPA_RESOURCE_DRIVER_UNCONDITIONAL, // Resource Attribute
    (void*) pm_pam_touch_screen1_rails_info, // Resource User Data
    PMIC_NPA_CLIENT_NODE_TOUCH_SCREEN1, // Node Name
    NPA_NODE_DEFAULT,  // Node Attributes
    NULL,
    pm_pam_deps_touch_screen1,  // Node Dependency
    NPA_ARRAY_SIZE(pm_pam_deps_touch_screen1),  // Deps Count 
   },
};

uint32 num_of_pam_rsrcs [] = { sizeof(pm_prm_pam_rsrcs) / sizeof(pm_prm_node_resource_info) };


/*************************** Device Layer Resources ***************************/

pm_prm_rsrc_data_type
pm_prm_device_rsrc_data [] =
{
    PM_PRM_DEV_RSRC_VREG ( b, ldo, 3, PM_PRM_RSRC_LDO,  PM_PRM_RPMH, PM_LDO_3 ),
    PM_PRM_DEV_RSRC_VREG ( a, ldo, 13, PM_PRM_RSRC_LDO,  PM_PRM_RPMH, PM_LDO_13 ),
    PM_PRM_DEV_RSRC_VREG ( a, ldo, 14, PM_PRM_RSRC_LDO,  PM_PRM_RPMH, PM_LDO_14 ),
};

pm_prm_resource_type
pm_prm_device_rsrcs [ ] =
{
    {
        pm_prm_device_rsrc_data, 
        sizeof(pm_prm_device_rsrc_data)/sizeof(pm_prm_rsrc_data_type), //Number of Resources
        3
    }
};

/*************************** Published resource ***************************/

char *pm_prm_pub_rsrc_names[] =
{
    PMIC_NPA_GROUP_ID_SENSOR_VDD, 
    PMIC_NPA_GROUP_ID_SENSOR_VDDIO, 
    PMIC_NPA_GROUP_ID_TSCREEN, 
    PMIC_NPA_GROUP_ID_TSCREEN1
};

pm_prm_pam_rsrc_pub_pd_type  
pm_prm_pub_pam_rsrcs = 
{
   NPA_ALL_PDS,
   pm_prm_pub_rsrc_names,
   4
};

