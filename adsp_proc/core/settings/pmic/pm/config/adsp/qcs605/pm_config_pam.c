
/*! \file  pm_config_pam.c
 *  
 *  \brief  File Contains the PMIC NPA PAM Data
 *  \details  This file contains the PAM data for the PMIC NPA layer.
 *  
 *    Platform: QCS605.XX.1.0
 *    Revision: QCS605.XX.1.0-SENSORS-00001
 *    Template Version: 
 *    $Header: //components/rel/core.qdsp6/2.1/settings/pmic/pm/config/adsp/qcs605/pm_config_pam.c#1 $ 
 *    $DateTime: 2017/12/20 00:08:21 $  $Author: pwbldsvc $
 *  
 *  &copy; Copyright Nov. 27, 2017 Qualcomm Technologies Incorporated, All Rights Reserved
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
/*===========================================================================

                        VARIABLES DEFINITION

===========================================================================*/

/*************************** SENSOR_VDD Client ***************************/

const pm_prm_vreg_data_type
qcs605_pm_pam_sensor_vdd_a_ldo16 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_LPM, 0, 0, PM_PRM_DEV_DISABLE },  // SENSOR_POWER_OFF
    { PM_PRM_VREG_MODE_LPM, 3008, 0, PM_PRM_DEV_ENABLE },  // SENSOR_LPM
    { PM_PRM_VREG_MODE_NPM, 3008, 0, PM_PRM_DEV_ENABLE }  // SENSOR_POWER_ON
};

const pm_prm_pam_client_cfg_type
qcs605_pm_pam_sensor_vdd_rails_info [] = 
{
    { (void*)qcs605_pm_pam_sensor_vdd_a_ldo16, PM_PRM_RSRC_VRM_VREG },
}; 

npa_node_dependency
qcs605_pm_pam_deps_sensor_vdd [] =
{
    PM_PRM_CLIENT_VREG_DEP ( a,  ldo, 16 ),
};

/*************************** SENSOR_VDDIO Client ***************************/

const pm_prm_vreg_data_type
qcs605_pm_pam_sensor_vddio_a_ldo14 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_NPM, 0, 0, PM_PRM_DEV_DISABLE },  // SENSOR_POWER_OFF
    { PM_PRM_VREG_MODE_LPM, 1800, 0, PM_PRM_DEV_ENABLE },  // SENSOR_LPM
    { PM_PRM_VREG_MODE_NPM, 1800, 0, PM_PRM_DEV_ENABLE }  // SENSOR_POWER_ON
};

const pm_prm_pam_client_cfg_type
qcs605_pm_pam_sensor_vddio_rails_info [] = 
{
    { (void*)qcs605_pm_pam_sensor_vddio_a_ldo14, PM_PRM_RSRC_VRM_VREG },
}; 

npa_node_dependency
qcs605_pm_pam_deps_sensor_vddio [] =
{
    PM_PRM_CLIENT_VREG_DEP ( a,  ldo, 14 ),
};

/*************************** NPA NODE RESOURCE ***************************/

const pm_prm_node_resource_info
qcs605_pm_prm_pam_rsrcs [] =
{
   {
     PMIC_NPA_GROUP_ID_SENSOR_VDD,  // Resource Name
    PMIC_NPA_MODE_ID_SENSOR_POWER_ON,   // Maximum Value
    NPA_RESOURCE_SINGLE_CLIENT | NPA_RESOURCE_DRIVER_UNCONDITIONAL, // Resource Attribute
    (void*) qcs605_pm_pam_sensor_vdd_rails_info, // Resource User Data
    PMIC_NPA_CLIENT_NODE_SENSOR_VDD, // Node Name
    NPA_NODE_DEFAULT,  // Node Attributes
    NULL,
    qcs605_pm_pam_deps_sensor_vdd,  // Node Dependency
    NPA_ARRAY_SIZE(qcs605_pm_pam_deps_sensor_vdd),  // Deps Count
   },
   {
     PMIC_NPA_GROUP_ID_SENSOR_VDDIO,  // Resource Name
    PMIC_NPA_MODE_ID_SENSOR_POWER_ON,   // Maximum Value
    NPA_RESOURCE_SINGLE_CLIENT | NPA_RESOURCE_DRIVER_UNCONDITIONAL, // Resource Attribute
    (void*) qcs605_pm_pam_sensor_vddio_rails_info, // Resource User Data
    PMIC_NPA_CLIENT_NODE_SENSOR_VDDIO, // Node Name
    NPA_NODE_DEFAULT,  // Node Attributes
    NULL,
    qcs605_pm_pam_deps_sensor_vddio,  // Node Dependency
    NPA_ARRAY_SIZE(qcs605_pm_pam_deps_sensor_vddio),  // Deps Count
   },
};

uint32 qcs605_num_of_pam_rsrcs [] = { sizeof(qcs605_pm_prm_pam_rsrcs) / sizeof(pm_prm_node_resource_info) };


/*************************** Device Layer Resources ***************************/

pm_prm_rsrc_data_type
qcs605_pm_prm_device_rsrc_data [] =
{
    PM_PRM_DEV_RSRC_VREG ( a, ldo, 14, PM_PRM_RSRC_LDO,  PM_PRM_RPMH, PM_LDO_14),
    PM_PRM_DEV_RSRC_VREG ( a, ldo, 16, PM_PRM_RSRC_LDO,  PM_PRM_RPMH, PM_LDO_16),
};

pm_prm_resource_type
qcs605_pm_prm_device_rsrcs [] =
{
    {
        qcs605_pm_prm_device_rsrc_data, 
        sizeof(qcs605_pm_prm_device_rsrc_data)/sizeof(pm_prm_rsrc_data_type), //Number of Resources
        2
    }
};

/*************************** Published resource ***************************/

char *qcs605_pm_prm_pub_rsrc_names[] =
{
    PMIC_NPA_GROUP_ID_SENSOR_VDD, 
    PMIC_NPA_GROUP_ID_SENSOR_VDDIO, 
};

pm_prm_pam_rsrc_pub_pd_type  
qcs605_pm_prm_pub_pam_rsrcs = 
{
   NPA_ALL_PDS,
   qcs605_pm_prm_pub_rsrc_names,
   2
};

