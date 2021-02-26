
/*! \file  pm_config_pam.c
 *  
 *  \brief  File Contains the PMIC NPA PAM Data
 *  \details  This file contains the PAM data for the PMIC NPA layer.
 *  
 *    Platform: SDM670.XX.1.0
 *    Revision: NO REVISIONS (Working Set)
 *    Template Version: 
 *    $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Settings/PMIC/core/pm_config_pam.c#3 $ 
 *    $DateTime: 2017/12/19 05:41:04 $  $Author: pwbldsvc $
 *  
 *  &copy; Copyright Dec. 12, 2017 Qualcomm Technologies Incorporated, All Rights Reserved
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

#define PMIC_NPA_CLIENT_NODE_DISP_EXT_DP "/node/pmic/client/disp_ext_dp"
#define PMIC_NPA_CLIENT_NODE_DISP_PRIM "/node/pmic/client/disp_prim"
#define PMIC_NPA_CLIENT_NODE_DISP_PRIM_AMOLED "/node/pmic/client/disp_prim_amoled"
#define PMIC_NPA_CLIENT_NODE_PCIE "/node/pmic/client/pcie"
#define PMIC_NPA_CLIENT_NODE_USB_SS1 "/node/pmic/client/usb_ss1"
#define PMIC_NPA_CLIENT_NODE_USB_SS2 "/node/pmic/client/usb_ss2"
/*===========================================================================

                        VARIABLES DEFINITION

===========================================================================*/


/*************************** DISP_EXT_DP Client ***************************/

const pm_prm_vreg_data_type
pm_pam_disp_ext_dp_a_ldo1 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_LPM, 0, 0, PM_PRM_DEV_DISABLE },  // GENERIC_STANDBY
    { PM_PRM_VREG_MODE_NPM, 1200, 0, PM_PRM_DEV_ENABLE }  // GENERIC_ACTIVE
};

const pm_prm_vreg_data_type
pm_pam_disp_ext_dp_b_ldo1 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_LPM, 0, 0, PM_PRM_DEV_DISABLE },  // GENERIC_STANDBY
    { PM_PRM_VREG_MODE_NPM, 880, 0, PM_PRM_DEV_ENABLE }  // GENERIC_ACTIVE
};

const pm_prm_vreg_data_type
pm_pam_disp_ext_dp_b_ldo7 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_LPM, 0, 0, PM_PRM_DEV_DISABLE },  // GENERIC_STANDBY
    { PM_PRM_VREG_MODE_NPM, 3088, 0, PM_PRM_DEV_ENABLE }  // GENERIC_ACTIVE
};

const pm_prm_pam_client_cfg_type
pm_pam_disp_ext_dp_rails_info [] = 
{
    { (void*)pm_pam_disp_ext_dp_a_ldo1, PM_PRM_RSRC_VRM_VREG },
    { (void*)pm_pam_disp_ext_dp_b_ldo1, PM_PRM_RSRC_VRM_VREG },
    { (void*)pm_pam_disp_ext_dp_b_ldo7, PM_PRM_RSRC_VRM_VREG },
}; 

npa_node_dependency
pm_pam_deps_disp_ext_dp [] =
{
    PM_PRM_CLIENT_VREG_DEP ( a,  ldo, 1 ),
    PM_PRM_CLIENT_VREG_DEP ( b,  ldo, 1 ),
    PM_PRM_CLIENT_VREG_DEP ( b,  ldo, 7 ),
};

/*************************** DISP_PRIM Client ***************************/

const pm_prm_vreg_data_type
pm_pam_disp_prim_a_ldo1 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_LPM, 0, 0, PM_PRM_DEV_DISABLE },  // GENERIC_STANDBY
    { PM_PRM_VREG_MODE_NPM, 1200, 0, PM_PRM_DEV_ENABLE }  // GENERIC_ACTIVE
};

const pm_prm_vreg_data_type
pm_pam_disp_prim_b_ldo1 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_LPM, 0, 0, PM_PRM_DEV_DISABLE },  // GENERIC_STANDBY
    { PM_PRM_VREG_MODE_NPM, 880, 0, PM_PRM_DEV_ENABLE }  // GENERIC_ACTIVE
};

const pm_prm_vreg_data_type
pm_pam_disp_prim_a_ldo11 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_LPM, 0, 0, PM_PRM_DEV_DISABLE },  // GENERIC_STANDBY
    { PM_PRM_VREG_MODE_NPM, 1800, 0, PM_PRM_DEV_ENABLE }  // GENERIC_ACTIVE
};

const pm_prm_pam_client_cfg_type
pm_pam_disp_prim_rails_info [] = 
{
    { (void*)pm_pam_disp_prim_a_ldo1, PM_PRM_RSRC_VRM_VREG },
    { (void*)pm_pam_disp_prim_b_ldo1, PM_PRM_RSRC_VRM_VREG },
    { (void*)pm_pam_disp_prim_a_ldo11, PM_PRM_RSRC_VRM_VREG },
}; 

npa_node_dependency
pm_pam_deps_disp_prim [] =
{
    PM_PRM_CLIENT_VREG_DEP ( a,  ldo, 1 ),
    PM_PRM_CLIENT_VREG_DEP ( b,  ldo, 1 ),
    PM_PRM_CLIENT_VREG_DEP ( a,  ldo, 11 ),
};

/*************************** DISP_PRIM_AMOLED Client ***************************/

const pm_prm_vreg_data_type
pm_pam_disp_prim_amoled_a_ldo1 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_LPM, 0, 0, PM_PRM_DEV_DISABLE },  // GENERIC_STANDBY
    { PM_PRM_VREG_MODE_NPM, 1200, 0, PM_PRM_DEV_ENABLE }  // GENERIC_ACTIVE
};

const pm_prm_vreg_data_type
pm_pam_disp_prim_amoled_b_ldo1 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_LPM, 0, 0, PM_PRM_DEV_DISABLE },  // GENERIC_STANDBY
    { PM_PRM_VREG_MODE_NPM, 900, 0, PM_PRM_DEV_ENABLE }  // GENERIC_ACTIVE
};

const pm_prm_vreg_data_type
pm_pam_disp_prim_amoled_b_ldo6 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_LPM, 0, 0, PM_PRM_DEV_DISABLE },  // GENERIC_STANDBY
    { PM_PRM_VREG_MODE_NPM, 3300, 0, PM_PRM_DEV_ENABLE }  // GENERIC_ACTIVE
};

const pm_prm_vreg_data_type
pm_pam_disp_prim_amoled_a_ldo11 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_LPM, 0, 0, PM_PRM_DEV_DISABLE },  // GENERIC_STANDBY
    { PM_PRM_VREG_MODE_NPM, 1800, 0, PM_PRM_DEV_ENABLE }  // GENERIC_ACTIVE
};

const pm_prm_pam_client_cfg_type
pm_pam_disp_prim_amoled_rails_info [] = 
{
    { (void*)pm_pam_disp_prim_amoled_a_ldo1, PM_PRM_RSRC_VRM_VREG },
    { (void*)pm_pam_disp_prim_amoled_b_ldo1, PM_PRM_RSRC_VRM_VREG },
    { (void*)pm_pam_disp_prim_amoled_b_ldo6, PM_PRM_RSRC_VRM_VREG },
    { (void*)pm_pam_disp_prim_amoled_a_ldo11, PM_PRM_RSRC_VRM_VREG },
}; 

npa_node_dependency
pm_pam_deps_disp_prim_amoled [] =
{
    PM_PRM_CLIENT_VREG_DEP ( a,  ldo, 1 ),
    PM_PRM_CLIENT_VREG_DEP ( b,  ldo, 1 ),
    PM_PRM_CLIENT_VREG_DEP ( b,  ldo, 6 ),
    PM_PRM_CLIENT_VREG_DEP ( a,  ldo, 11 ),
};

/*************************** PCIE Client ***************************/

const pm_prm_vreg_data_type
pm_pam_pcie_a_ldo1 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_LPM, 0, 0, PM_PRM_DEV_DISABLE },  // PCIE_OFF
    { PM_PRM_VREG_MODE_LPM, 880, 0, PM_PRM_DEV_ENABLE },  // PCIE_SUSPEND
    { PM_PRM_VREG_MODE_NPM, 880, 0, PM_PRM_DEV_ENABLE },  // PCIE_ACTIVE_NOMINAL
    { PM_PRM_VREG_MODE_NPM, 880, 0, PM_PRM_DEV_ENABLE },  // PCIE_ACTIVE_SVS
    { PM_PRM_VREG_MODE_NPM, 880, 0, PM_PRM_DEV_ENABLE }  // PCIE_ACTIVE_TURBO
};

const pm_prm_pam_client_cfg_type
pm_pam_pcie_rails_info [] = 
{
    { (void*)pm_pam_pcie_a_ldo1, PM_PRM_RSRC_VRM_VREG },
}; 

npa_node_dependency
pm_pam_deps_pcie [] =
{
    PM_PRM_CLIENT_VREG_DEP ( a,  ldo, 1 ),
};

/*************************** USB_SS1 Client ***************************/

const pm_prm_vreg_data_type
pm_pam_usb_ss1_a_ldo1 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_LPM, 0, 0, PM_PRM_DEV_DISABLE },  // USB_OFF
    { PM_PRM_VREG_MODE_LPM, 1200, 0, PM_PRM_DEV_DISABLE },  // USB_PERPH_SUSPEND
    { PM_PRM_VREG_MODE_NPM, 1200, 0, PM_PRM_DEV_ENABLE },  // USB_PERPH_ACTIVE
    { PM_PRM_VREG_MODE_LPM, 1200, 0, PM_PRM_DEV_DISABLE },  // USB_HOST_SUSPEND
    { PM_PRM_VREG_MODE_NPM, 1200, 0, PM_PRM_DEV_ENABLE }  // USB_HOST_ACTIVE
};

const pm_prm_vreg_data_type
pm_pam_usb_ss1_b_ldo1 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_LPM, 0, 0, PM_PRM_DEV_DISABLE },  // USB_OFF
    { PM_PRM_VREG_MODE_LPM, 880, 0, PM_PRM_DEV_DISABLE },  // USB_PERPH_SUSPEND
    { PM_PRM_VREG_MODE_NPM, 880, 0, PM_PRM_DEV_ENABLE },  // USB_PERPH_ACTIVE
    { PM_PRM_VREG_MODE_LPM, 880, 0, PM_PRM_DEV_DISABLE },  // USB_HOST_SUSPEND
    { PM_PRM_VREG_MODE_NPM, 880, 0, PM_PRM_DEV_ENABLE }  // USB_HOST_ACTIVE
};

const pm_prm_vreg_data_type
pm_pam_usb_ss1_b_ldo7 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_LPM, 0, 0, PM_PRM_DEV_DISABLE },  // USB_OFF
    { PM_PRM_VREG_MODE_LPM, 3088, 0, PM_PRM_DEV_DISABLE },  // USB_PERPH_SUSPEND
    { PM_PRM_VREG_MODE_NPM, 3088, 0, PM_PRM_DEV_ENABLE },  // USB_PERPH_ACTIVE
    { PM_PRM_VREG_MODE_LPM, 3088, 0, PM_PRM_DEV_DISABLE },  // USB_HOST_SUSPEND
    { PM_PRM_VREG_MODE_NPM, 3088, 0, PM_PRM_DEV_ENABLE }  // USB_HOST_ACTIVE
};

const pm_prm_vreg_data_type
pm_pam_usb_ss1_a_ldo10 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_LPM, 0, 0, PM_PRM_DEV_DISABLE },  // USB_OFF
    { PM_PRM_VREG_MODE_LPM, 1800, 0, PM_PRM_DEV_DISABLE },  // USB_PERPH_SUSPEND
    { PM_PRM_VREG_MODE_NPM, 1800, 0, PM_PRM_DEV_ENABLE },  // USB_PERPH_ACTIVE
    { PM_PRM_VREG_MODE_LPM, 1800, 0, PM_PRM_DEV_DISABLE },  // USB_HOST_SUSPEND
    { PM_PRM_VREG_MODE_NPM, 1800, 0, PM_PRM_DEV_ENABLE }  // USB_HOST_ACTIVE
};

const pm_prm_pam_client_cfg_type
pm_pam_usb_ss1_rails_info [] = 
{
    { (void*)pm_pam_usb_ss1_a_ldo1, PM_PRM_RSRC_VRM_VREG },
    { (void*)pm_pam_usb_ss1_b_ldo1, PM_PRM_RSRC_VRM_VREG },
    { (void*)pm_pam_usb_ss1_b_ldo7, PM_PRM_RSRC_VRM_VREG },
    { (void*)pm_pam_usb_ss1_a_ldo10, PM_PRM_RSRC_VRM_VREG },
}; 

npa_node_dependency
pm_pam_deps_usb_ss1 [] =
{
    PM_PRM_CLIENT_VREG_DEP ( a,  ldo, 1 ),
    PM_PRM_CLIENT_VREG_DEP ( b,  ldo, 1 ),
    PM_PRM_CLIENT_VREG_DEP ( b,  ldo, 7 ),
    PM_PRM_CLIENT_VREG_DEP ( a,  ldo, 10 ),
};

/*************************** USB_SS2 Client ***************************/

const pm_prm_vreg_data_type
pm_pam_usb_ss2_a_ldo1 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_LPM, 0, 0, PM_PRM_DEV_DISABLE },  // USB_OFF
    { PM_PRM_VREG_MODE_LPM, 1200, 0, PM_PRM_DEV_DISABLE },  // USB_PERPH_SUSPEND
    { PM_PRM_VREG_MODE_NPM, 1200, 0, PM_PRM_DEV_ENABLE },  // USB_PERPH_ACTIVE
    { PM_PRM_VREG_MODE_LPM, 1200, 0, PM_PRM_DEV_DISABLE },  // USB_HOST_SUSPEND
    { PM_PRM_VREG_MODE_NPM, 1200, 0, PM_PRM_DEV_ENABLE }  // USB_HOST_ACTIVE
};

const pm_prm_vreg_data_type
pm_pam_usb_ss2_b_ldo1 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_LPM, 0, 0, PM_PRM_DEV_DISABLE },  // USB_OFF
    { PM_PRM_VREG_MODE_LPM, 880, 0, PM_PRM_DEV_DISABLE },  // USB_PERPH_SUSPEND
    { PM_PRM_VREG_MODE_NPM, 880, 0, PM_PRM_DEV_DISABLE },  // USB_PERPH_ACTIVE
    { PM_PRM_VREG_MODE_LPM, 880, 0, PM_PRM_DEV_DISABLE },  // USB_HOST_SUSPEND
    { PM_PRM_VREG_MODE_NPM, 880, 0, PM_PRM_DEV_DISABLE }  // USB_HOST_ACTIVE
};

const pm_prm_vreg_data_type
pm_pam_usb_ss2_b_ldo7 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_LPM, 0, 0, PM_PRM_DEV_DISABLE },  // USB_OFF
    { PM_PRM_VREG_MODE_LPM, 3088, 0, PM_PRM_DEV_DISABLE },  // USB_PERPH_SUSPEND
    { PM_PRM_VREG_MODE_NPM, 3088, 0, PM_PRM_DEV_ENABLE },  // USB_PERPH_ACTIVE
    { PM_PRM_VREG_MODE_LPM, 3088, 0, PM_PRM_DEV_DISABLE },  // USB_HOST_SUSPEND
    { PM_PRM_VREG_MODE_NPM, 3088, 0, PM_PRM_DEV_ENABLE }  // USB_HOST_ACTIVE
};

const pm_prm_vreg_data_type
pm_pam_usb_ss2_a_ldo10 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_LPM, 0, 0, PM_PRM_DEV_DISABLE },  // USB_OFF
    { PM_PRM_VREG_MODE_LPM, 1800, 0, PM_PRM_DEV_DISABLE },  // USB_PERPH_SUSPEND
    { PM_PRM_VREG_MODE_NPM, 1800, 0, PM_PRM_DEV_ENABLE },  // USB_PERPH_ACTIVE
    { PM_PRM_VREG_MODE_LPM, 1800, 0, PM_PRM_DEV_DISABLE },  // USB_HOST_SUSPEND
    { PM_PRM_VREG_MODE_NPM, 1800, 0, PM_PRM_DEV_ENABLE }  // USB_HOST_ACTIVE
};

const pm_prm_pam_client_cfg_type
pm_pam_usb_ss2_rails_info [] = 
{
    { (void*)pm_pam_usb_ss2_a_ldo1, PM_PRM_RSRC_VRM_VREG },
    { (void*)pm_pam_usb_ss2_b_ldo1, PM_PRM_RSRC_VRM_VREG },
    { (void*)pm_pam_usb_ss2_b_ldo7, PM_PRM_RSRC_VRM_VREG },
    { (void*)pm_pam_usb_ss2_a_ldo10, PM_PRM_RSRC_VRM_VREG },
}; 

npa_node_dependency
pm_pam_deps_usb_ss2 [] =
{
    PM_PRM_CLIENT_VREG_DEP ( a,  ldo, 1 ),
    PM_PRM_CLIENT_VREG_DEP ( b,  ldo, 1 ),
    PM_PRM_CLIENT_VREG_DEP ( b,  ldo, 7 ),
    PM_PRM_CLIENT_VREG_DEP ( a,  ldo, 10 ),
};


/*************************** NPA NODE RESOURCE ***************************/

const pm_prm_node_resource_info
pm_prm_pam_rsrcs [] =
{
   {
     PMIC_NPA_GROUP_ID_DISP_EXT_DP,  // Resource Name
    PMIC_NPA_MODE_ID_GENERIC_ACTIVE,   // Maximum Value
    NPA_RESOURCE_SINGLE_CLIENT | NPA_RESOURCE_DRIVER_UNCONDITIONAL, // Resource Attribute
    (void*) pm_pam_disp_ext_dp_rails_info, // Resource User Data
    PMIC_NPA_CLIENT_NODE_DISP_EXT_DP, // Node Name
    NPA_NODE_DEFAULT,  // Node Attributes
    NULL,
    pm_pam_deps_disp_ext_dp,  // Node Dependency
    NPA_ARRAY_SIZE(pm_pam_deps_disp_ext_dp),  // Deps Count 
   },
   {
     PMIC_NPA_GROUP_ID_DISP_PRIM,  // Resource Name
    PMIC_NPA_MODE_ID_GENERIC_ACTIVE,   // Maximum Value
    NPA_RESOURCE_SINGLE_CLIENT | NPA_RESOURCE_DRIVER_UNCONDITIONAL, // Resource Attribute
    (void*) pm_pam_disp_prim_rails_info, // Resource User Data
    PMIC_NPA_CLIENT_NODE_DISP_PRIM, // Node Name
    NPA_NODE_DEFAULT,  // Node Attributes
    NULL,
    pm_pam_deps_disp_prim,  // Node Dependency
    NPA_ARRAY_SIZE(pm_pam_deps_disp_prim),  // Deps Count 
   },
   {
     PMIC_NPA_GROUP_ID_DISP_PRIM_AMOLED,  // Resource Name
    PMIC_NPA_MODE_ID_GENERIC_ACTIVE,   // Maximum Value
    NPA_RESOURCE_SINGLE_CLIENT | NPA_RESOURCE_DRIVER_UNCONDITIONAL, // Resource Attribute
    (void*) pm_pam_disp_prim_amoled_rails_info, // Resource User Data
    PMIC_NPA_CLIENT_NODE_DISP_PRIM_AMOLED, // Node Name
    NPA_NODE_DEFAULT,  // Node Attributes
    NULL,
    pm_pam_deps_disp_prim_amoled,  // Node Dependency
    NPA_ARRAY_SIZE(pm_pam_deps_disp_prim_amoled),  // Deps Count 
   },
   {
     PMIC_NPA_GROUP_ID_PCIE,  // Resource Name
    PMIC_NPA_MODE_ID_PCIE_ACTIVE_TURBO,   // Maximum Value
    NPA_RESOURCE_SINGLE_CLIENT | NPA_RESOURCE_DRIVER_UNCONDITIONAL, // Resource Attribute
    (void*) pm_pam_pcie_rails_info, // Resource User Data
    PMIC_NPA_CLIENT_NODE_PCIE, // Node Name
    NPA_NODE_DEFAULT,  // Node Attributes
    NULL,
    pm_pam_deps_pcie,  // Node Dependency
    NPA_ARRAY_SIZE(pm_pam_deps_pcie),  // Deps Count 
   },
   {
     PMIC_NPA_GROUP_ID_USB_SS1,  // Resource Name
    PMIC_NPA_MODE_ID_USB_HOST_ACTIVE,   // Maximum Value
    NPA_RESOURCE_SINGLE_CLIENT | NPA_RESOURCE_DRIVER_UNCONDITIONAL, // Resource Attribute
    (void*) pm_pam_usb_ss1_rails_info, // Resource User Data
    PMIC_NPA_CLIENT_NODE_USB_SS1, // Node Name
    NPA_NODE_DEFAULT,  // Node Attributes
    NULL,
    pm_pam_deps_usb_ss1,  // Node Dependency
    NPA_ARRAY_SIZE(pm_pam_deps_usb_ss1),  // Deps Count 
   },
   {
     PMIC_NPA_GROUP_ID_USB_SS2,  // Resource Name
    PMIC_NPA_MODE_ID_USB_HOST_ACTIVE,   // Maximum Value
    NPA_RESOURCE_SINGLE_CLIENT | NPA_RESOURCE_DRIVER_UNCONDITIONAL, // Resource Attribute
    (void*) pm_pam_usb_ss2_rails_info, // Resource User Data
    PMIC_NPA_CLIENT_NODE_USB_SS2, // Node Name
    NPA_NODE_DEFAULT,  // Node Attributes
    NULL,
    pm_pam_deps_usb_ss2,  // Node Dependency
    NPA_ARRAY_SIZE(pm_pam_deps_usb_ss2),  // Deps Count 
   },
};

uint32 num_of_pam_rsrcs [] = { sizeof(pm_prm_pam_rsrcs) / sizeof(pm_prm_node_resource_info) };


/*************************** Device Layer Resources ***************************/

pm_prm_rsrc_data_type
pm_prm_device_rsrc_data [] =
{
    PM_PRM_DEV_RSRC_VREG ( a, ldo, 1, PM_PRM_RSRC_LDO,  PM_PRM_RPMH, PM_LDO_1 ),
    PM_PRM_DEV_RSRC_VREG ( b, ldo, 1, PM_PRM_RSRC_LDO,  PM_PRM_RPMH, PM_LDO_1 ),
    PM_PRM_DEV_RSRC_VREG ( b, ldo, 6, PM_PRM_RSRC_LDO,  PM_PRM_RPMH, PM_LDO_6 ),
    PM_PRM_DEV_RSRC_VREG ( b, ldo, 7, PM_PRM_RSRC_LDO,  PM_PRM_RPMH, PM_LDO_7 ),
    PM_PRM_DEV_RSRC_VREG ( a, ldo, 10, PM_PRM_RSRC_LDO,  PM_PRM_RPMH, PM_LDO_10 ),
    PM_PRM_DEV_RSRC_VREG ( a, ldo, 11, PM_PRM_RSRC_LDO,  PM_PRM_RPMH, PM_LDO_11 ),
};

pm_prm_resource_type
pm_prm_device_rsrcs [ ] =
{
    {
        pm_prm_device_rsrc_data, 
        sizeof(pm_prm_device_rsrc_data)/sizeof(pm_prm_rsrc_data_type), //Number of Resources
        6
    }
};

