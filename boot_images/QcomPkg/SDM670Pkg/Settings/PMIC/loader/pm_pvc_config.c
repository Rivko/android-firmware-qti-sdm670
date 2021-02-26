/*! \file
*  \n
*  \brief  pm_pvc_config.c
*  \n
*  \n This file contains pmic configuration data specific for SPMI Controller's
      Peripheral for SDM845 device..
*  \n
*  \n &copy; Copyright 2015-2017 Qualcomm Technologies Incorporated, All Rights Reserved
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Settings/PMIC/loader/pm_pvc_config.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/18/17   sv     Updated to support SDM670
02/01/17   aab    Supported SDM845 target.  Added SPMI,PVC, MGPI and VIOCTL configurations
08/25/15   vtw    Created.
========================================================================== */

/*-------------------------------------------------------------------------
* Include Files
* ----------------------------------------------------------------------*/

#include "SpmiCfg.h"

/*-------------------------------------------------------------------------
* Preprocessor Definitions and Constants
* ----------------------------------------------------------------------*/

/* Converts a PVC port index to an overall PMIC Arbiter port index */
#define PVC_PORT_TO_PMIC_ARB_PORT(p) (p+1)

/*   Port assignment in 8998   */
#define APPS0_PORT      1   /* APPS Port 0  */
#define APPS1_PORT      2   /* APPS Port 1  */
#define UICC_MGPI_PORT  6   /* UICC MGPI port 6  */

/*-------------------------------------------------------------------------
* Static Variable Definitions
* ----------------------------------------------------------------------*/

/*
 * pm_pvc_apps0_addr
 *
 * PVC port addr for APPS0 Gold cluster.
 */
SpmiCfg_Ppid pm_pvc_apps0_addr[] =
{
  {0x1, 0x1440}, /* PM660, S1A_CTRL_VSET_LB */
  {0x1, 0x1441}, /* PM660, S1A_CTRL_VSET_UB */
  {0x1, 0x1445}, /* PM660, S1A_CTRL_MODE_CTL */
  {0x1, 0x1446}, /* PM660, S1A_CTRL_EN_CTL */
};

const SpmiCfg_PvcPortCfg pm_pvc_apps0_cfg = {APPS0_PORT, SPMI_ACCESS_PRIORITY_LOW, pm_pvc_apps0_addr, sizeof(pm_pvc_apps0_addr)/sizeof(SpmiCfg_Ppid)};

/*
 * pm_pvc_apps1_addr
 *
 * PVC port addr for APPS1 Silver cluster block.
 */
SpmiCfg_Ppid pm_pvc_apps1_addr[] =
{
  {0x1, 0x1A40}, /* PM660, S3A_CTRL_VSET_LB */
  {0x1, 0x1A41}, /* PM660, S3A_CTRL_VSET_UB */
  {0x1, 0x1A45}, /* PM660, S3A_CTRL_MODE_CTL */
  {0x1, 0x1A46}, /* PM660, S3A_CTRL_EN_CTL */
};

const SpmiCfg_PvcPortCfg pm_pvc_apps1_cfg = {APPS1_PORT, SPMI_ACCESS_PRIORITY_LOW, pm_pvc_apps1_addr, sizeof(pm_pvc_apps1_addr)/sizeof(SpmiCfg_Ppid)};


/*
 * pm_pvc_mgpi_addr
 *
 * PVC port addr for UICC MGPI block.
 */
SpmiCfg_Ppid pm_pvc_mgpi_addr[] =
{
  {0x1, 0x4E46}, /* PM660, L15_EN_CTL */
  {0x1, 0x5046}  /* PM660, L17_EN_CTL */
};

const SpmiCfg_PvcPortCfg pm_pvc_uicc_mgpi_cfg = {UICC_MGPI_PORT, SPMI_ACCESS_PRIORITY_LOW, pm_pvc_mgpi_addr, sizeof(pm_pvc_mgpi_addr)/sizeof(SpmiCfg_Ppid)};

/*
 * pm_arb_pvc_cfg
 *
 * PMIC Arbiter PVC ports config.
 */
const SpmiCfg_PvcPortCfg* pm_arb_pvc_port_cfg[] = {&pm_pvc_apps0_cfg, &pm_pvc_apps1_cfg, &pm_pvc_uicc_mgpi_cfg};

uint32 pm_arb_pvc_port_cfg_size[] = {sizeof(pm_arb_pvc_port_cfg) / sizeof(SpmiCfg_PvcPortCfg*)};



/*
 * pm_mgpi_port_cfg
 *
 * PMIC MGPI ports config.
 */
const SpmiBusCfg_MgpiPortCfg pm_mgpi_port_0_cfg = 
{
    0,                 /* MGPI Port id */
    UICC_MGPI_PORT,    /* PVC Port id */
    FALSE,             /* Positive edge enable */
    0,                 /* Positive edge PVC port address index */
    0,                 /* Positive edge data */
    TRUE,              /* Negative edge enable */
    0,                 /* Negative edge PVC port address index */
    0                  /* Negative edge data */
};

const SpmiBusCfg_MgpiPortCfg pm_mgpi_port_1_cfg = 
{
    1,                /* MGPI Port id */
    UICC_MGPI_PORT,   /* PVC Port id */
    FALSE,            /* Positive edge enable */
    0,                /* Positive edge PVC port address index */
    0,                /* Positive edge data */
    TRUE,             /* Negative edge enable */
    1,                /* Negative edge PVC port address index */
    0                 /* Negative edge data */
};
const SpmiBusCfg_MgpiPortCfg* pm_mgpi_port_cfg[] = {&pm_mgpi_port_0_cfg, &pm_mgpi_port_1_cfg};
uint32 pm_mgpi_port_cfg_size[] = {sizeof(pm_mgpi_port_cfg) / sizeof(SpmiBusCfg_MgpiPortCfg*)};




/*
 * pm_vioctl_cfg
 *
 * PMIC VIOCTL config.
 */
const SpmiBusCfg_VioctlCfg pm_vioctl_cfg0 =
{
   TRUE,                 /* Positive edge enable */
   {0x00, 0x4100},       /* Positive edge PPID:  Slave id / Address */
   0x08,                 /* Positive edge periph interrupt mask */
   FALSE,                /* Negative edge enable */
   {0x0, 0x0},           /* Negative edge PPID:  Slave id / Address */
   0,                    /* Negative edge periph interrupt mask */
};
const SpmiBusCfg_VioctlCfg* pm_vioctl_cfg[] = {&pm_vioctl_cfg0 };
uint32 pm_vioctl_cfg_size[] = {sizeof(pm_vioctl_cfg) / sizeof(SpmiBusCfg_VioctlCfg*)};
