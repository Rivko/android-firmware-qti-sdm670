#ifndef PMAPP_NPA_H
#define PMAPP_NPA_H

/*! \file  pmapp_npa.h
 *  
 *  \brief  File Contains the PMIC NPA CMI Code
 *  \details  This file contains the needed definition and enum for PMIC NPA layer.
 *  
 *    PMIC code generation Version: 1.0.0.0
 *    PMIC code generation Locked Version: APPSBL_MSM8998_PMI8998_0p6 - LOCKED

 *    This file contains code for Target specific settings and modes.
 *  
 *  &copy; Copyright 2016 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
 */

/*===========================================================================

                EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module over time.

$Header: //components/rel/boot.xf/2.1/QcomPkg/Include/api/pmic/pm/pmapp_npa.h#2 $ 
$DateTime: 2019/03/08 09:58:25 $  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 

===========================================================================*/

/*===========================================================================

                        DEFINITION

===========================================================================*/

#define PMIC_NPA_GROUP_ID_ANT1 "/pmic/client/ant1"
#define PMIC_NPA_GROUP_ID_BOOST "/pmic/client/boost"
#define PMIC_NPA_GROUP_ID_CAMERA_BACK  "/pmic/client/camera_back"
#define PMIC_NPA_GROUP_ID_CAMERA_FRONT "/pmic/client/camera_front"
#define PMIC_NPA_GROUP_ID_DISP_EXT_DP "/pmic/client/disp_ext_dp"
#define PMIC_NPA_GROUP_ID_DISP_EXT_HDMI "/pmic/client/disp_ext_hdmi"
#define PMIC_NPA_GROUP_ID_DISP_PRIM "/pmic/client/disp_prim"
#define PMIC_NPA_GROUP_ID_DISP_PRIM_AMOLED "/pmic/client/disp_prim_amoled"
#define PMIC_NPA_GROUP_ID_DISP_PRIM_EDP "/pmic/client/disp_prim_edp"
#define PMIC_NPA_GROUP_ID_DISP_SEC "/pmic/client/disp_sec"
#define PMIC_NPA_GROUP_ID_EMMC "/pmic/client/emmc"
#define PMIC_NPA_GROUP_ID_PCIE "/pmic/client/pcie"
/**
 * PMIC_NPA_GROUP_ID -- PMIC_NPA_GROUP_ID_MEM_UVOL
 */
#define PMIC_NPA_GROUP_ID_MEM_UVOL "/pmic/client/vdd_mem_uvol"
#define PMIC_NPA_GROUP_ID_NFC "/pmic/client/nfc"
#define PMIC_NPA_GROUP_ID_RAIL_CX "/pmic/client/rail_cx"
#define PMIC_NPA_GROUP_ID_RAIL_MX "/pmic/client/rail_mx"
#define PMIC_NPA_GROUP_ID_SD "/pmic/client/sd"
#define PMIC_NPA_GROUP_ID_TSCREEN "/pmic/client/touch_screen"
#define PMIC_NPA_GROUP_ID_USB_HS1 "/pmic/client/usb_hs1"
#define PMIC_NPA_GROUP_ID_USB_HS2 "/pmic/client/usb_hs2"
#define PMIC_NPA_GROUP_ID_USB_HSIC "/pmic/client/usb_hsic"
#define PMIC_NPA_GROUP_ID_USB_SS1 "/pmic/client/usb_ss1"
#define PMIC_NPA_GROUP_ID_USB_SS2 "/pmic/client/usb_ss2"
#define PMIC_NPA_GROUP_ID_USB_SSMUX1 "/pmic/client/usb_ssmux1"
#define PMIC_NPA_GROUP_ID_USB_SSMUX2 "/pmic/client/usb_ssmux2"

/*===========================================================================

                        ENUMERATION

===========================================================================*/

enum
{
PMIC_NPA_MODE_ID_GENERIC_STANDBY,
PMIC_NPA_MODE_ID_GENERIC_ACTIVE,
PMIC_NPA_MODE_ID_GENERIC_MAX,
};

enum
{
PMIC_NPA_GROUP_ID_eMMC_OFF,
PMIC_NPA_GROUP_ID_eMMC_ACTIVE,
PMIC_NPA_GROUP_ID_eMMC_SLEEP,
PMIC_NPA_GROUP_ID_eMMC_MAX,
};

enum
{
PMIC_NPA_MODE_ID_PCIE_OFF,
PMIC_NPA_MODE_ID_PCIE_SUSPEND,
PMIC_NPA_MODE_ID_PCIE_ACTIVE_NOMINAL,
PMIC_NPA_MODE_ID_PCIE_ACTIVE_SVS,
PMIC_NPA_MODE_ID_PCIE_ACTIVE_TURBO,
PMIC_NPA_MODE_ID_PCIE_MAX,
};

enum
{
PMIC_NPA_MODE_ID_SD_MODE_OFF,
PMIC_NPA_MODE_ID_SD_MODE_1,
PMIC_NPA_MODE_ID_SD_MODE_2,
PMIC_NPA_MODE_ID_SD_MODE_MAX,
};

enum
{
PMIC_NPA_MODE_ID_USB_OFF,
PMIC_NPA_MODE_ID_USB_PERPH_SUSPEND,
PMIC_NPA_MODE_ID_USB_PERPH_ACTIVE,
PMIC_NPA_MODE_ID_USB_HOST_SUSPEND,
PMIC_NPA_MODE_ID_USB_HOST_ACTIVE,
PMIC_NPA_MODE_ID_USB_MAX,
};

enum
{
PMIC_NPA_MODE_ID_TOUCH_MODE_OFF,
PMIC_NPA_MODE_ID_TOUCH_MODE_LPM,
PMIC_NPA_MODE_ID_TOUCH_MODE_ACTIVE,
PMIC_NPA_MODE_ID_TOUCH_MODE_MAX,
};

enum
{
PMIC_NPA_MODE_ID_ANT_MODE_0,
PMIC_NPA_MODE_ID_ANT_MODE_1,
PMIC_NPA_MODE_ID_ANT_MODE_2,
PMIC_NPA_MODE_ID_ANT_MAX,
};

#endif // PM_CONFIG_APPSBL_NPA_PAM_C