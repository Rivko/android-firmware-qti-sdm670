/*! \file  pm_config_aop_pam.c
 *  
 *  \brief  File Contains the PMIC NPA CMI Code
 *  \details  This file contains the needed definition and enum for PMIC NPA layer.
 *  
 *    PMIC code generation Version: 1.0.0.0
 *    PMIC code generation Locked Version: AOP_SDM845_PM8998_0p1 - LOCKED

 *    This file contains code for Target specific settings and modes.
 *  
 *  &copy; Copyright 2017 Qualcomm Technologies, All Rights Reserved
 */
/*===========================================================================

                EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module over time.

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/pm/config/sdm670/pm_config_aop_pam.c#1 $ 
$DateTime: 2018/02/16 04:48:18 $  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/22/18   as      Increasing threshold for vddq(S5B),vdd2(GPIO12B),cx(S3B) (CR#2176243)
01/16/18   as      PAM table updated for LDO1B( CR#2138657 )
11/10/17   as      Updating PAM table as per as DDR freq Plan (CR#2135566)
07/10/17   as      Updating PAM changes for SDM670 (CR#2069199)
01/07/17   akm     Updated PAM changes
===========================================================================*/

/*===========================================================================

                        INCLUDE HEADER FILES

===========================================================================*/

#include "pmapp_npa.h"
#include "pm_prm_device_defs.h"

/*===========================================================================

                        MACRO DEFINITIONS

===========================================================================*/

#define PMIC_NPA_CLIENT_NODE_DDR "/pm/ddr"
/*===========================================================================

                        VARIABLES DEFINITION

===========================================================================*/


/*************************** DDR Client ***************************/

const pm_prm_vreg_data_type
pm_pam_ddr_a_ldo1 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_LPM,  1200, 40, PM_PRM_DEV_DISABLE },  // DDR_CFG_0
    { PM_PRM_VREG_MODE_NPM,  1200, 40, PM_PRM_DEV_ENABLE },  // DDR_CFG_1
    { PM_PRM_VREG_MODE_NPM,  1200, 40, PM_PRM_DEV_ENABLE },  // DDR_CFG_2
    { PM_PRM_VREG_MODE_NPM,  1200, 40, PM_PRM_DEV_ENABLE },  // DDR_CFG_3
    { PM_PRM_VREG_MODE_NPM,  1200, 40, PM_PRM_DEV_ENABLE },  // DDR_CFG_4
    { PM_PRM_VREG_MODE_NPM,  1200, 40, PM_PRM_DEV_ENABLE },  // DDR_CFG_5
    { PM_PRM_VREG_MODE_NPM,  1200, 40, PM_PRM_DEV_ENABLE },  // DDR_CFG_6
    { PM_PRM_VREG_MODE_NPM,  1200, 40, PM_PRM_DEV_ENABLE },  // DDR_CFG_7
    { PM_PRM_VREG_MODE_NPM,  1200, 40, PM_PRM_DEV_ENABLE },  // DDR_CFG_8
    { PM_PRM_VREG_MODE_NPM,  1200, 40, PM_PRM_DEV_ENABLE },  // DDR_CFG_9
    { PM_PRM_VREG_MODE_NPM,  1200, 40, PM_PRM_DEV_ENABLE },  // DDR_CFG_10
};

const pm_prm_vreg_data_type
pm_pam_ddr_b_smps1 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_AUTO, 0, 0, PM_PRM_DEV_ENABLE },  // DDR_CFG_0
    { PM_PRM_VREG_MODE_AUTO, 0, 0, PM_PRM_DEV_ENABLE },  // DDR_CFG_1
    { PM_PRM_VREG_MODE_AUTO, 0, 0, PM_PRM_DEV_ENABLE },  // DDR_CFG_2
    { PM_PRM_VREG_MODE_AUTO, 0, 0, PM_PRM_DEV_ENABLE },  // DDR_CFG_3
    { PM_PRM_VREG_MODE_AUTO, 0, 0, PM_PRM_DEV_ENABLE },  // DDR_CFG_4
    { PM_PRM_VREG_MODE_AUTO, 0, 0, PM_PRM_DEV_ENABLE },  // DDR_CFG_5
    { PM_PRM_VREG_MODE_NPM,  0, 0, PM_PRM_DEV_ENABLE },  // DDR_CFG_6
    { PM_PRM_VREG_MODE_NPM,  0, 0, PM_PRM_DEV_ENABLE },  // DDR_CFG_7
    { PM_PRM_VREG_MODE_NPM,  0, 0, PM_PRM_DEV_ENABLE },  // DDR_CFG_8
    { PM_PRM_VREG_MODE_NPM,  0, 0, PM_PRM_DEV_ENABLE },  // DDR_CFG_9
    { PM_PRM_VREG_MODE_NPM,  0, 0, PM_PRM_DEV_ENABLE },  // DDR_CFG_10
};

const pm_prm_vreg_data_type
pm_pam_ddr_b_smps3 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_AUTO, 0, 0, PM_PRM_DEV_DISABLE},  // DDR_CFG_0
    { PM_PRM_VREG_MODE_AUTO, 0, 0, PM_PRM_DEV_DISABLE},  // DDR_CFG_1
    { PM_PRM_VREG_MODE_AUTO, 0, 0, PM_PRM_DEV_DISABLE},  // DDR_CFG_2
    { PM_PRM_VREG_MODE_AUTO, 0, 0, PM_PRM_DEV_DISABLE},  // DDR_CFG_3
    { PM_PRM_VREG_MODE_AUTO, 0, 0, PM_PRM_DEV_DISABLE},  // DDR_CFG_4
    { PM_PRM_VREG_MODE_AUTO, 0, 0, PM_PRM_DEV_DISABLE},  // DDR_CFG_5
    { PM_PRM_VREG_MODE_NPM,  0, 0, PM_PRM_DEV_DISABLE},  // DDR_CFG_6
    { PM_PRM_VREG_MODE_NPM,  0, 0, PM_PRM_DEV_DISABLE},  // DDR_CFG_7
    { PM_PRM_VREG_MODE_NPM,  0, 0, PM_PRM_DEV_DISABLE},  // DDR_CFG_8
    { PM_PRM_VREG_MODE_AUTO,  0, 0, PM_PRM_DEV_DISABLE},  // DDR_CFG_9
    { PM_PRM_VREG_MODE_NPM,  0, 0, PM_PRM_DEV_DISABLE},  // DDR_CFG_10
    
};

const pm_prm_vreg_data_type
pm_pam_ddr_b_smps5 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    {PM_PRM_VREG_MODE_AUTO, 600, 0, PM_PRM_DEV_DISABLE},//DDR_CFG_0
    {PM_PRM_VREG_MODE_AUTO, 600, 0, PM_PRM_DEV_ENABLE}, //DDR_CFG_1
    {PM_PRM_VREG_MODE_AUTO,  600, 0, PM_PRM_DEV_ENABLE}, //DDR_CFG_2
    {PM_PRM_VREG_MODE_NPM,  600, 0, PM_PRM_DEV_ENABLE}, //DDR_CFG_3
    {PM_PRM_VREG_MODE_NPM,  600, 0, PM_PRM_DEV_ENABLE}, //DDR_CFG_4
    {PM_PRM_VREG_MODE_NPM,  600, 0, PM_PRM_DEV_ENABLE}, //DDR_CFG_5
    {PM_PRM_VREG_MODE_AUTO, 600, 0, PM_PRM_DEV_ENABLE}, //DDR_CFG_6
    {PM_PRM_VREG_MODE_NPM,  600, 0, PM_PRM_DEV_ENABLE}, //DDR_CFG_7
    {PM_PRM_VREG_MODE_AUTO, 600, 0, PM_PRM_DEV_ENABLE}, //DDR_CFG_8
    {PM_PRM_VREG_MODE_AUTO,  600, 0, PM_PRM_DEV_ENABLE}, //DDR_CFG_9
    {PM_PRM_VREG_MODE_NPM,  600, 0, PM_PRM_DEV_ENABLE}, //DDR_CFG_10
};

const pm_prm_vreg_data_type
pm_pam_ddr_b_gpio12 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_NONE,   0, 0,  PM_PRM_DEV_DISABLE},  // DDR_CFG_0
    { PM_PRM_VREG_MODE_NONE,   0, 0,  PM_PRM_DEV_DISABLE},  // DDR_CFG_1
    { PM_PRM_VREG_MODE_NONE,   0, 0,  PM_PRM_DEV_DISABLE},  // DDR_CFG_2
    { PM_PRM_VREG_MODE_NONE,   0, 0,  PM_PRM_DEV_DISABLE},  // DDR_CFG_3
    { PM_PRM_VREG_MODE_NONE,   0, 0,  PM_PRM_DEV_DISABLE},  // DDR_CFG_4
    { PM_PRM_VREG_MODE_NONE,   0, 0,  PM_PRM_DEV_DISABLE},  // DDR_CFG_5
    { PM_PRM_VREG_MODE_NONE,   0, 0,  PM_PRM_DEV_ENABLE },  // DDR_CFG_6
    { PM_PRM_VREG_MODE_NONE,   0, 0,  PM_PRM_DEV_ENABLE },  // DDR_CFG_7
    { PM_PRM_VREG_MODE_NONE,   0, 0,  PM_PRM_DEV_ENABLE },  // DDR_CFG_8
    { PM_PRM_VREG_MODE_NONE,   0, 0,  PM_PRM_DEV_DISABLE },  // DDR_CFG_9
    { PM_PRM_VREG_MODE_NONE,   0, 0,  PM_PRM_DEV_ENABLE },  // DDR_CFG_10
};

const pm_prm_vreg_data_type
pm_pam_ddr_b_ldo1 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_LPM, 880, 40, PM_PRM_DEV_DISABLE },  // DDR_CFG_0
    { PM_PRM_VREG_MODE_NPM, 880, 40, PM_PRM_DEV_ENABLE },  // DDR_CFG_1
    { PM_PRM_VREG_MODE_NPM, 880, 40, PM_PRM_DEV_ENABLE },  // DDR_CFG_2
    { PM_PRM_VREG_MODE_NPM, 880, 40, PM_PRM_DEV_ENABLE },  // DDR_CFG_3
    { PM_PRM_VREG_MODE_NPM, 880, 40, PM_PRM_DEV_ENABLE },  // DDR_CFG_4
    { PM_PRM_VREG_MODE_NPM, 880, 40, PM_PRM_DEV_ENABLE },  // DDR_CFG_5
    { PM_PRM_VREG_MODE_NPM, 880, 40, PM_PRM_DEV_ENABLE },  // DDR_CFG_6
    { PM_PRM_VREG_MODE_NPM, 880, 40, PM_PRM_DEV_ENABLE },  // DDR_CFG_7
    { PM_PRM_VREG_MODE_NPM, 880, 40, PM_PRM_DEV_ENABLE },  // DDR_CFG_8
    { PM_PRM_VREG_MODE_NPM, 880, 40, PM_PRM_DEV_ENABLE },  // DDR_CFG_9
    { PM_PRM_VREG_MODE_NPM, 880, 40, PM_PRM_DEV_ENABLE },  // DDR_CFG_10
};




const pm_prm_pam_client_cfg_type
pm_pam_ddr_rails_info [] = 
{
    { (void*)pm_pam_ddr_b_smps1, PM_PRM_RSRC_VRM_VREG },
    { (void*)pm_pam_ddr_b_smps3, PM_PRM_RSRC_VRM_VREG },
    { (void*)pm_pam_ddr_b_ldo1, PM_PRM_RSRC_VRM_VREG },
    { (void*)pm_pam_ddr_b_smps5, PM_PRM_RSRC_VRM_VREG },
    { (void*)pm_pam_ddr_b_gpio12, PM_PRM_RSRC_VRM_VREG },
    { (void*)pm_pam_ddr_a_ldo1, PM_PRM_RSRC_VRM_VREG },
}; 

pm_prm_npa_node_dependency
pm_pam_deps_ddr [] =
{
    PM_PRM_CLIENT_VREG_DEP ( b,  smp, 1 ),
    PM_PRM_CLIENT_VREG_DEP ( b,  smp, 3 ),
    PM_PRM_CLIENT_VREG_DEP ( b,  ldo,  1 ),
    PM_PRM_CLIENT_VREG_DEP ( b,  smp,  5 ),
    PM_PRM_CLIENT_VREG_DEP ( b,  gpio, 12 ),
    PM_PRM_CLIENT_VREG_DEP ( a,  ldo, 1 ),
};


/*************************** NPA NODE RESOURCE ***************************/

pm_prm_node_resource_info
pm_prm_pam_rsrcs [] =
{   
    {    
        PMIC_NPA_GROUP_ID_DDR,               // Resource Name
        PMIC_NPA_MODE_ID_DDR_CFG_10,   // Maximum Value
        UNPA_RESOURCE_DEFAULT | UNPA_RESOURCE_ASYNC_REQUESTS, // Resource Attribute
        (void*) pm_pam_ddr_rails_info,       // Resource User Data
        PMIC_NPA_CLIENT_NODE_DDR,            // Node Name
        0,                                   // Node Attributes
        NULL,                                         
        pm_pam_deps_ddr,                     // Node Dependency
        NPA_ARRAY_SIZE(pm_pam_deps_ddr),     // Dependency Count 
    },
};

uint32 num_of_pam_rsrcs [] = { sizeof(pm_prm_pam_rsrcs) / sizeof(pm_prm_node_resource_info) };


/*************************** Device Layer Resources ***************************/

pm_prm_rsrc_data_type
pm_prm_device_rsrc_data [] =
{
    PM_PRM_DEV_RSRC_VREG_VOLTAGE ( b, smp, 1 ),
    PM_PRM_DEV_RSRC_VREG_VOLTAGE ( b, smp, 3 ),
};

pm_prm_resource_type
pm_prm_device_rsrcs [] =
{
    {
        pm_prm_device_rsrc_data, 
        sizeof(pm_prm_device_rsrc_data)/sizeof(pm_prm_rsrc_data_type), //Number of Resources
    }
};

