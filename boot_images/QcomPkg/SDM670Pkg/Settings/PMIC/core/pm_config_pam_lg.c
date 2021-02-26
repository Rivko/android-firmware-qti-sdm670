
/*! \file  pm_config_pam.c
 *  
 *  \brief  File Contains the PMIC NPA PAM Data
 *  \details  This file contains the PAM data for the PMIC NPA layer.
 *  
 *    Platform: QCS605.XX.1.0
 *    Revision: QCS605.XX.1.0-APPS-00002 (Working Set)
 *    Template Version: 
 *    $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Settings/PMIC/core/pm_config_pam_lg.c#3 $ 
 *    $DateTime: 2019/06/21 02:20:36 $  $Author: pwbldsvc $
 *  
 *  &copy; Copyright May 8, 2019 Qualcomm Technologies Incorporated, All Rights Reserved
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

#define PMIC_NPA_CLIENT_NODE_DISP_EXT_HDMI "/node/pmic/client/disp_ext_hdmi"
#define PMIC_NPA_CLIENT_NODE_DISP_PRIM "/node/pmic/client/disp_prim"
#define PMIC_NPA_CLIENT_NODE_USB_SS1 "/node/pmic/client/usb_ss1"
#define PMIC_NPA_CLIENT_NODE_USB_SS2 "/node/pmic/client/usb_ss2"
/*===========================================================================

                        VARIABLES DEFINITION

===========================================================================*/

/*************************** DISP_EXT_HDMI Client ***************************/

const pm_prm_vreg_data_type
pm_pam_disp_ext_hdmi_a_ldo2 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_LPM, 0, 0, PM_PRM_DEV_DISABLE },  // GENERIC_STANDBY
    { PM_PRM_VREG_MODE_NPM, 1200, 0, PM_PRM_DEV_ENABLE }  // GENERIC_ACTIVE
};

const pm_prm_vreg_data_type
pm_pam_disp_ext_hdmi_a_ldo6 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_LPM, 0, 0, PM_PRM_DEV_DISABLE },  // GENERIC_STANDBY
    { PM_PRM_VREG_MODE_NPM, 880, 0, PM_PRM_DEV_ENABLE }  // GENERIC_ACTIVE
};

const pm_prm_vreg_data_type
pm_pam_disp_ext_hdmi_a_ldo12 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_LPM, 0, 0, PM_PRM_DEV_DISABLE },  // GENERIC_STANDBY
    { PM_PRM_VREG_MODE_NPM, 1800, 0, PM_PRM_DEV_ENABLE }  // GENERIC_ACTIVE
};

const pm_prm_vreg_data_type
pm_pam_disp_ext_hdmi_a_ldo13 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_LPM, 0, 0, PM_PRM_DEV_DISABLE },  // GENERIC_STANDBY
    { PM_PRM_VREG_MODE_NPM, 1800, 0, PM_PRM_DEV_ENABLE }  // GENERIC_ACTIVE
};

const pm_prm_pam_client_cfg_type
pm_pam_disp_ext_hdmi_rails_info [] = 
{
    { (void*)pm_pam_disp_ext_hdmi_a_ldo2, PM_PRM_RSRC_VRM_VREG },
    { (void*)pm_pam_disp_ext_hdmi_a_ldo6, PM_PRM_RSRC_VRM_VREG },
    { (void*)pm_pam_disp_ext_hdmi_a_ldo12, PM_PRM_RSRC_VRM_VREG },
    { (void*)pm_pam_disp_ext_hdmi_a_ldo13, PM_PRM_RSRC_VRM_VREG },
}; 

npa_node_dependency
pm_pam_deps_disp_ext_hdmi [] =
{
    PM_PRM_CLIENT_VREG_DEP ( a,  ldo, 2 ),
    PM_PRM_CLIENT_VREG_DEP ( a,  ldo, 6 ),
    PM_PRM_CLIENT_VREG_DEP ( a,  ldo, 12 ),
    PM_PRM_CLIENT_VREG_DEP ( a,  ldo, 13 ),
};

/*************************** DISP_PRIM Client ***************************/

const pm_prm_vreg_data_type
pm_pam_disp_prim_a_ldo2 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_LPM, 0, 0, PM_PRM_DEV_DISABLE },  // GENERIC_STANDBY
    { PM_PRM_VREG_MODE_NPM, 1200, 0, PM_PRM_DEV_ENABLE }  // GENERIC_ACTIVE
};

const pm_prm_vreg_data_type
pm_pam_disp_prim_a_ldo6 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_LPM, 0, 0, PM_PRM_DEV_DISABLE },  // GENERIC_STANDBY
    { PM_PRM_VREG_MODE_NPM, 880, 0, PM_PRM_DEV_ENABLE }  // GENERIC_ACTIVE
};

const pm_prm_vreg_data_type
pm_pam_disp_prim_a_ldo13 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_LPM, 0, 0, PM_PRM_DEV_DISABLE },  // GENERIC_STANDBY
    { PM_PRM_VREG_MODE_NPM, 1800, 0, PM_PRM_DEV_ENABLE }  // GENERIC_ACTIVE
};

const pm_prm_pam_client_cfg_type
pm_pam_disp_prim_rails_info [] = 
{
    { (void*)pm_pam_disp_prim_a_ldo2, PM_PRM_RSRC_VRM_VREG },
    { (void*)pm_pam_disp_prim_a_ldo6, PM_PRM_RSRC_VRM_VREG },
    { (void*)pm_pam_disp_prim_a_ldo13, PM_PRM_RSRC_VRM_VREG },
}; 

npa_node_dependency
pm_pam_deps_disp_prim [] =
{
    PM_PRM_CLIENT_VREG_DEP ( a,  ldo, 2 ),
    PM_PRM_CLIENT_VREG_DEP ( a,  ldo, 6 ),
    PM_PRM_CLIENT_VREG_DEP ( a,  ldo, 13 ),
};

/*************************** USB_SS1 Client ***************************/

const pm_prm_vreg_data_type
pm_pam_usb_ss1_a_ldo2 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_LPM, 0, 0, PM_PRM_DEV_DISABLE },  // USB_OFF
    { PM_PRM_VREG_MODE_LPM, 1200, 0, PM_PRM_DEV_DISABLE },  // USB_PERPH_SUSPEND
    { PM_PRM_VREG_MODE_NPM, 1200, 0, PM_PRM_DEV_ENABLE },  // USB_PERPH_ACTIVE
    { PM_PRM_VREG_MODE_LPM, 1200, 0, PM_PRM_DEV_DISABLE },  // USB_HOST_SUSPEND
    { PM_PRM_VREG_MODE_NPM, 1200, 0, PM_PRM_DEV_ENABLE }  // USB_HOST_ACTIVE
};

const pm_prm_vreg_data_type
pm_pam_usb_ss1_a_ldo6 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_LPM, 0, 0, PM_PRM_DEV_DISABLE },  // USB_OFF
    { PM_PRM_VREG_MODE_LPM, 880, 0, PM_PRM_DEV_DISABLE },  // USB_PERPH_SUSPEND
    { PM_PRM_VREG_MODE_NPM, 880, 0, PM_PRM_DEV_ENABLE },  // USB_PERPH_ACTIVE
    { PM_PRM_VREG_MODE_LPM, 880, 0, PM_PRM_DEV_DISABLE },  // USB_HOST_SUSPEND
    { PM_PRM_VREG_MODE_NPM, 880, 0, PM_PRM_DEV_ENABLE }  // USB_HOST_ACTIVE
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

const pm_prm_vreg_data_type
pm_pam_usb_ss1_a_ldo17 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_LPM, 0, 0, PM_PRM_DEV_DISABLE },  // USB_OFF
    { PM_PRM_VREG_MODE_LPM, 3088, 0, PM_PRM_DEV_DISABLE },  // USB_PERPH_SUSPEND
    { PM_PRM_VREG_MODE_NPM, 3088, 0, PM_PRM_DEV_ENABLE },  // USB_PERPH_ACTIVE
    { PM_PRM_VREG_MODE_LPM, 3088, 0, PM_PRM_DEV_DISABLE },  // USB_HOST_SUSPEND
    { PM_PRM_VREG_MODE_NPM, 3088, 0, PM_PRM_DEV_ENABLE }  // USB_HOST_ACTIVE
};

const pm_prm_pam_client_cfg_type
pm_pam_usb_ss1_rails_info [] = 
{
    { (void*)pm_pam_usb_ss1_a_ldo2, PM_PRM_RSRC_VRM_VREG },
    { (void*)pm_pam_usb_ss1_a_ldo6, PM_PRM_RSRC_VRM_VREG },
    { (void*)pm_pam_usb_ss1_a_ldo10, PM_PRM_RSRC_VRM_VREG },
    { (void*)pm_pam_usb_ss1_a_ldo17, PM_PRM_RSRC_VRM_VREG },
}; 

npa_node_dependency
pm_pam_deps_usb_ss1 [] =
{
    PM_PRM_CLIENT_VREG_DEP ( a,  ldo, 2 ),
    PM_PRM_CLIENT_VREG_DEP ( a,  ldo, 6 ),
    PM_PRM_CLIENT_VREG_DEP ( a,  ldo, 10 ),
    PM_PRM_CLIENT_VREG_DEP ( a,  ldo, 17 ),
};

/*************************** USB_SS2 Client ***************************/

const pm_prm_vreg_data_type
pm_pam_usb_ss2_a_ldo2 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_LPM, 0, 0, PM_PRM_DEV_DISABLE },  // USB_OFF
    { PM_PRM_VREG_MODE_LPM, 1200, 0, PM_PRM_DEV_DISABLE },  // USB_PERPH_SUSPEND
    { PM_PRM_VREG_MODE_NPM, 1200, 0, PM_PRM_DEV_ENABLE },  // USB_PERPH_ACTIVE
    { PM_PRM_VREG_MODE_LPM, 1200, 0, PM_PRM_DEV_DISABLE },  // USB_HOST_SUSPEND
    { PM_PRM_VREG_MODE_NPM, 1200, 0, PM_PRM_DEV_ENABLE }  // USB_HOST_ACTIVE
};

const pm_prm_vreg_data_type
pm_pam_usb_ss2_a_ldo6 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_LPM, 0, 0, PM_PRM_DEV_DISABLE },  // USB_OFF
    { PM_PRM_VREG_MODE_LPM, 880, 0, PM_PRM_DEV_DISABLE },  // USB_PERPH_SUSPEND
    { PM_PRM_VREG_MODE_NPM, 880, 0, PM_PRM_DEV_DISABLE },  // USB_PERPH_ACTIVE
    { PM_PRM_VREG_MODE_LPM, 880, 0, PM_PRM_DEV_DISABLE },  // USB_HOST_SUSPEND
    { PM_PRM_VREG_MODE_NPM, 880, 0, PM_PRM_DEV_DISABLE }  // USB_HOST_ACTIVE
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

const pm_prm_vreg_data_type
pm_pam_usb_ss2_a_ldo17 [] =
{
     /*     mode    voltage(mV)  headroom(mV)   enable*/ 
    { PM_PRM_VREG_MODE_LPM, 0, 0, PM_PRM_DEV_DISABLE },  // USB_OFF
    { PM_PRM_VREG_MODE_LPM, 3088, 0, PM_PRM_DEV_DISABLE },  // USB_PERPH_SUSPEND
    { PM_PRM_VREG_MODE_NPM, 3088, 0, PM_PRM_DEV_ENABLE },  // USB_PERPH_ACTIVE
    { PM_PRM_VREG_MODE_LPM, 3088, 0, PM_PRM_DEV_DISABLE },  // USB_HOST_SUSPEND
    { PM_PRM_VREG_MODE_NPM, 3088, 0, PM_PRM_DEV_ENABLE }  // USB_HOST_ACTIVE
};

const pm_prm_pam_client_cfg_type
pm_pam_usb_ss2_rails_info [] = 
{
    { (void*)pm_pam_usb_ss2_a_ldo2, PM_PRM_RSRC_VRM_VREG },
    { (void*)pm_pam_usb_ss2_a_ldo6, PM_PRM_RSRC_VRM_VREG },
    { (void*)pm_pam_usb_ss2_a_ldo10, PM_PRM_RSRC_VRM_VREG },
    { (void*)pm_pam_usb_ss2_a_ldo17, PM_PRM_RSRC_VRM_VREG },
}; 

npa_node_dependency
pm_pam_deps_usb_ss2 [] =
{
    PM_PRM_CLIENT_VREG_DEP ( a,  ldo, 2 ),
    PM_PRM_CLIENT_VREG_DEP ( a,  ldo, 6 ),
    PM_PRM_CLIENT_VREG_DEP ( a,  ldo, 10 ),
    PM_PRM_CLIENT_VREG_DEP ( a,  ldo, 17 ),
};

/*************************** NPA NODE RESOURCE ***************************/

const pm_prm_node_resource_info
pm_prm_pam_rsrcs [] =
{
   {
     PMIC_NPA_GROUP_ID_DISP_EXT_HDMI,  // Resource Name
    PMIC_NPA_MODE_ID_GENERIC_ACTIVE,   // Maximum Value
    NPA_RESOURCE_SINGLE_CLIENT | NPA_RESOURCE_DRIVER_UNCONDITIONAL, // Resource Attribute
    (void*) pm_pam_disp_ext_hdmi_rails_info, // Resource User Data
    PMIC_NPA_CLIENT_NODE_DISP_EXT_HDMI, // Node Name
    NPA_NODE_DEFAULT,  // Node Attributes
    NULL,
    pm_pam_deps_disp_ext_hdmi,  // Node Dependency
    NPA_ARRAY_SIZE(pm_pam_deps_disp_ext_hdmi),  // Deps Count
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
    PM_PRM_DEV_RSRC_VREG ( a, ldo, 2, PM_PRM_RSRC_LDO,  PM_PRM_RPMH, PM_LDO_2),
    PM_PRM_DEV_RSRC_VREG ( a, ldo, 6, PM_PRM_RSRC_LDO,  PM_PRM_RPMH, PM_LDO_6),
    PM_PRM_DEV_RSRC_VREG ( a, ldo, 10, PM_PRM_RSRC_LDO,  PM_PRM_RPMH, PM_LDO_10),
    PM_PRM_DEV_RSRC_VREG ( a, ldo, 12, PM_PRM_RSRC_LDO,  PM_PRM_RPMH, PM_LDO_12),
    PM_PRM_DEV_RSRC_VREG ( a, ldo, 13, PM_PRM_RSRC_LDO,  PM_PRM_RPMH, PM_LDO_13),
    PM_PRM_DEV_RSRC_VREG ( a, ldo, 17, PM_PRM_RSRC_LDO,  PM_PRM_RPMH, PM_LDO_17),
};

pm_prm_resource_type
pm_prm_device_rsrcs [] =
{
    {
        pm_prm_device_rsrc_data, 
        sizeof(pm_prm_device_rsrc_data)/sizeof(pm_prm_rsrc_data_type), //Number of Resources
        6
    }
};


