/*==============================================================================

FILE:      icbcfg_data.c

DESCRIPTION: This file implements the ICB Configuration driver.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
Edit History

$Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Settings/ICB/loader/icbcfg_data.c#8 $ 
$DateTime: 2018/07/11 09:12:28 $
$Author: pwbldsvc $
$Change: 16618960 $ 

When        Who    What, where, why
----------  ---    -----------------------------------------------------------
2017/11/28  sds    Update QM throttle config for GPU, use throttle
2017/09/12  sds    Update QM throttle config for GPU, modem danger
2017/07/19  sds    Update QM throttle config
2017/06/14  sds    OSDC configuration
2017/06/12  sds    Initial QM config
2017/05/08  sds    Initial v2 config
2016/11/18  sds    Created

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
                             All Rights Reserved.
                          QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "icbcfg_hwio.h"
#include "../../../../Library/ICBLib/icbcfgi.h"
#include "../../../Library/ICBTargetLib/icbcfg_hwio_target_v1.h"
#include "../../../Library/ICBTargetLib/icbcfg_hwio_target_v2.h"

/*---------------------------------------------------------------------------*/
/*          Macro and constant definitions                                   */
/*---------------------------------------------------------------------------*/
#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))

/*============================================================================
                        DEVICE CONFIG PROPERTY DATA
============================================================================*/

/*---------------------------------------------------------------------------*/
/*          Properties data for device ID  = "icbcfg/boot"                   */
/*---------------------------------------------------------------------------*/

/* ICBcfg Boot Configuration Data*/

/*---------------------------------------------------------------------------*/
/* SDM845 v1                                                                 */
/*---------------------------------------------------------------------------*/
icbcfg_data_type icbcfg_boot_data_sdm845_v1[] = 
{
  /* Force SSID 13 for TCU accesses */
  ICBCFG_HWIOF_DW(MEMNOC_TCU_DYN_ATTR_CACHEINDEXVAL_LOW, CACHEINDEXVAL, 13),
  ICBCFG_HWIOF_DW(MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW, CACHEINDEXOV, 0x1),

  /* DSAT core registers */
  ICBCFG_HWIOF_DW(GCC_MMSS_QM_CORE_CBCR, CLK_ENABLE, 0x1),
  ICBCFG_WAIT_HWIOF_DW(GCC_MMSS_QM_CORE_CBCR, CLK_OFF, 0x0),
  ICBCFG_HWIOF_DW(GCC_MMSS_QM_AHB_CBCR, CLK_ENABLE, 0x1),
  ICBCFG_WAIT_HWIOF_DW(GCC_MMSS_QM_AHB_CBCR, CLK_OFF, 0x0),
  ICBCFG_HWIO_DW(QM_CORE_CLK_CGC_CNTRL, 0x1),
  ICBCFG_HWIO_DW(QM_PRE_STALL_TIMEOUT_CNT_URG_0, 0x60),
  ICBCFG_HWIO_DW(QM_PRE_STALL_TIMEOUT_CNT_URG_1, 0x27),
  ICBCFG_HWIO_DW(QM_PRE_STALL_TIMEOUT_CNT_URG_2, 0x27),
  ICBCFG_HWIO_DW(QM_PRE_STALL_TIMEOUT_CNT_URG_3, 0x27),
  ICBCFG_HWIO_DW(QM_POST_STALL_TIMEOUT_CNT_URG_0, 0x60),
  ICBCFG_HWIO_DW(QM_POST_STALL_TIMEOUT_CNT_URG_1, 0x27),
  ICBCFG_HWIO_DW(QM_POST_STALL_TIMEOUT_CNT_URG_2, 0x27),
  ICBCFG_HWIO_DW(QM_POST_STALL_TIMEOUT_CNT_URG_3, 0x27),
  ICBCFG_HWIO_DW(QM_POST_STALL_WDW_OVERLAP_CNTL, 0x1),
  ICBCFG_HWIO_DW(QM_THROTTLE_LEVEL_LEGACY_SELECT, 0x0),
  ICBCFG_HWIOI_DW(QM_THROTTLE_LEVEL_QOS_n, 7, 0x7FC),
  ICBCFG_HWIO_DW(QM_THROTTLE_LEVEL_SU_CFG, 0x7F),
  ICBCFG_HWIO_DW(QM_CLNT_8_DANGER_AGGR_CNTRL, 0x3),
  ICBCFG_HWIO_DW(QM_CLNT_8_DANGER_AGGR_MODEM_CNTRL, 0x0),
  ICBCFG_HWIOI_DW(QM_CLNT_8_THROTTLE_LEVEL_QOS_n, 3, 0xEE000003),
  ICBCFG_HWIOI_DW(QM_CLNT_8_THROTTLE_LEVEL_QOS_n, 7, 0xAA000000),
  ICBCFG_HWIO_DW(QM_CLNT_8_THROTTLE_LEVEL_OUTPUT_EN, 0x1),
  ICBCFG_HWIO_DW(QM_CLNT_9_DANGER_AGGR_CNTRL, 0x3),
  ICBCFG_HWIO_DW(QM_CLNT_9_DANGER_AGGR_MODEM_CNTRL, 0x0),
  ICBCFG_HWIOI_DW(QM_CLNT_9_THROTTLE_LEVEL_QOS_n, 1, 0xEE000001),
  ICBCFG_HWIOI_DW(QM_CLNT_9_THROTTLE_LEVEL_QOS_n, 2, 0xEE000002),
  ICBCFG_HWIOI_DW(QM_CLNT_9_THROTTLE_LEVEL_QOS_n, 3, 0xEE000003),
  ICBCFG_HWIOI_DW(QM_CLNT_9_THROTTLE_LEVEL_QOS_n, 7, 0xAA000000),
  ICBCFG_HWIO_DW(QM_CLNT_9_THROTTLE_LEVEL_OUTPUT_EN, 0x1),
  ICBCFG_HWIOI_DW(QM_CLNT_10_THROTTLE_LEVEL_QOS_n, 1, 0xEE000001),
  ICBCFG_HWIOI_DW(QM_CLNT_10_THROTTLE_LEVEL_QOS_n, 2, 0xEE000002),
  ICBCFG_HWIOI_DW(QM_CLNT_10_THROTTLE_LEVEL_QOS_n, 3, 0xEE000003),
  ICBCFG_HWIOI_DW(QM_CLNT_10_THROTTLE_LEVEL_QOS_n, 7, 0xAA000000),
  ICBCFG_HWIO_DW(QM_CLNT_10_THROTTLE_LEVEL_OUTPUT_EN, 0x1),
  ICBCFG_HWIO_DW(QM_DSP_DANGER_AGGR_CNTRL, 0x3),
  ICBCFG_HWIO_DW(QM_DSP_DANGER_AGGR_MODEM_CNTRL, 0x0),
  ICBCFG_HWIO_DW(QM_DSP_PRIORITY_MODE_SEL, 0x1),
  ICBCFG_HWIO_DW(QM_DSP_SYSTEM_DANGER_AGGR_MODEM_CNTRL, 0x0),
  ICBCFG_HWIO_DW(QM_DSP_SYSTEM_DANGER_OUTPUT_EN, 0x0),
  ICBCFG_HWIOF_READ_DW(CABO0_CABO_GLOBAL_DDR_CLK_PERIOD_CFG,DDR_CLK_PERIOD),
  ICBCFG_HWIOF_WRITE_DW(QM_PERIOD_BUS_CFG,PERIOD_BUS_SW),
  ICBCFG_HWIOF_DW(QM_PERIOD_BUS_CFG, PERIOD_BUS_LOAD_SW, 0x1),
  ICBCFG_HWIOF_DW(QM_PERIOD_BUS_CFG, PERIOD_BUS_SW_OVERRIDE, 0x0),
  ICBCFG_HWIOI_DW(QOS_FREQ_BAND_BNDRY_n, 0, 0x97B),
  ICBCFG_HWIOI_DW(QOS_FREQ_BAND_BNDRY_n, 1, 0x0),
  ICBCFG_HWIOI_DW(QOS_FREQ_BAND_BNDRY_n, 2, 0x0),
  ICBCFG_HWIO_DW(QM_AGGR_SAFE_OVERRIDE_CNTRL, 0x1),
  ICBCFG_HWIO_DW(QM_DANGER_SAFE_CNTRL,0x1),

  /* AP throttles and shapers */
  ICBCFG_HWIOF_DW(APP_BWMON_THROTTLE_0_CGC_CNTRL, THROTTLE_CGC_EN, 0x1),
  ICBCFG_HWIOF_DW(APP_BWMON_THROTTLE_0_GRANT_PERIOD, GRANT_PERIOD, 0x3e8),
  ICBCFG_HWIO_DW(APP_BWMON_THROTTLE_0_THRESHOLD_03, 0xFFFFFFFF),
  ICBCFG_HWIO_DW(APP_BWMON_THROTTLE_0_THRESHOLD_02, 0xFFFF0064),
  ICBCFG_HWIO_DW(APP_BWMON_THROTTLE_0_THRESHOLD_01, 0xFFFFFFFF),
  ICBCFG_HWIO_DW(APP_BWMON_THROTTLE_0_THRESHOLD_00, 0xFFFFFFFF),
  ICBCFG_HWIOF_DW(APP_BWMON_THROTTLE_0_PEAK_ACCUM_CREDIT, PEAK_ACCUM_CREDIT, 0x100),
  ICBCFG_HWIO_DW(APP_BWMON_THROTTLE_0_CNTRL, 0x0E01),

  ICBCFG_HWIOF_DW(APP_BWMON_THROTTLE_1_CGC_CNTRL, THROTTLE_CGC_EN, 0x1),
  ICBCFG_HWIOF_DW(APP_BWMON_THROTTLE_1_GRANT_PERIOD, GRANT_PERIOD, 0x3e8),
  ICBCFG_HWIO_DW(APP_BWMON_THROTTLE_1_THRESHOLD_03, 0xFFFFFFFF),
  ICBCFG_HWIO_DW(APP_BWMON_THROTTLE_1_THRESHOLD_02, 0xFFFF0064),
  ICBCFG_HWIO_DW(APP_BWMON_THROTTLE_1_THRESHOLD_01, 0xFFFFFFFF),
  ICBCFG_HWIO_DW(APP_BWMON_THROTTLE_1_THRESHOLD_00, 0xFFFFFFFF),
  ICBCFG_HWIOF_DW(APP_BWMON_THROTTLE_1_PEAK_ACCUM_CREDIT, PEAK_ACCUM_CREDIT, 0x100),
  ICBCFG_HWIO_DW(APP_BWMON_THROTTLE_1_CNTRL, 0x0E01),

  ICBCFG_HWIOF_DW(MEMNOC_APPS_RD_QOSBOX_MAINCTL_LOW, SAFESHAPING, 0x2),
  ICBCFG_HWIOF_DW(MEMNOC_APPS_RDWR_QOSBOX_MAINCTL_LOW, SAFESHAPING, 0x2),

  /* CDSP throttle and shaper */
  ICBCFG_HWIOF_DW(CDSP_BWMON_THROTTLE_0_CGC_CNTRL, THROTTLE_CGC_EN, 0x1),
  ICBCFG_HWIOF_DW(CDSP_BWMON_THROTTLE_0_GRANT_PERIOD, GRANT_PERIOD, 0x3e8),
  ICBCFG_HWIO_DW(CDSP_BWMON_THROTTLE_0_THRESHOLD_03, 0xFFFFFFFF),
  ICBCFG_HWIO_DW(CDSP_BWMON_THROTTLE_0_THRESHOLD_02, 0xFFFF0064),
  ICBCFG_HWIO_DW(CDSP_BWMON_THROTTLE_0_THRESHOLD_01, 0xFFFFFFFF),
  ICBCFG_HWIO_DW(CDSP_BWMON_THROTTLE_0_THRESHOLD_00, 0xFFFFFFFF),
  ICBCFG_HWIOF_DW(CDSP_BWMON_THROTTLE_0_PEAK_ACCUM_CREDIT, PEAK_ACCUM_CREDIT, 0x100),
  ICBCFG_HWIO_DW(CDSP_BWMON_THROTTLE_0_CNTRL, 0x0E01),

  ICBCFG_HWIOF_DW(MEMNOC_CDSP_QOSBOX_MAINCTL_LOW, SAFESHAPING, 0x2),

  /* GPU throttles and shapers */
  ICBCFG_HWIOF_DW(GPU_BWMON_THROTTLE_0_CGC_CNTRL, THROTTLE_CGC_EN, 0x1),
  ICBCFG_HWIOF_DW(GPU_BWMON_THROTTLE_0_GRANT_PERIOD, GRANT_PERIOD, 0x3e8),
  ICBCFG_HWIO_DW(GPU_BWMON_THROTTLE_0_THRESHOLD_03, 0xFFFFFFFF),
  ICBCFG_HWIO_DW(GPU_BWMON_THROTTLE_0_THRESHOLD_02, 0xFFFF0064),
  ICBCFG_HWIO_DW(GPU_BWMON_THROTTLE_0_THRESHOLD_01, 0xFFFFFFFF),
  ICBCFG_HWIO_DW(GPU_BWMON_THROTTLE_0_THRESHOLD_00, 0xFFFFFFFF),
  ICBCFG_HWIOF_DW(GPU_BWMON_THROTTLE_0_PEAK_ACCUM_CREDIT, PEAK_ACCUM_CREDIT, 0x100),
  ICBCFG_HWIO_DW(GPU_BWMON_THROTTLE_0_CNTRL, 0x0E01),

  ICBCFG_HWIOF_DW(GPU_BWMON_THROTTLE_1_CGC_CNTRL, THROTTLE_CGC_EN, 0x1),
  ICBCFG_HWIOF_DW(GPU_BWMON_THROTTLE_1_GRANT_PERIOD, GRANT_PERIOD, 0x3e8),
  ICBCFG_HWIO_DW(GPU_BWMON_THROTTLE_1_THRESHOLD_03, 0xFFFFFFFF),
  ICBCFG_HWIO_DW(GPU_BWMON_THROTTLE_1_THRESHOLD_02, 0xFFFF0064),
  ICBCFG_HWIO_DW(GPU_BWMON_THROTTLE_1_THRESHOLD_01, 0xFFFFFFFF),
  ICBCFG_HWIO_DW(GPU_BWMON_THROTTLE_1_THRESHOLD_00, 0xFFFFFFFF),
  ICBCFG_HWIOF_DW(GPU_BWMON_THROTTLE_1_PEAK_ACCUM_CREDIT, PEAK_ACCUM_CREDIT, 0x100),
  ICBCFG_HWIO_DW(GPU_BWMON_THROTTLE_1_CNTRL, 0x0E01),

  ICBCFG_HWIOF_DW(MEMNOC_GPU0_QOSBOX_MAINCTL_LOW, SAFESHAPING, 0x2),
  ICBCFG_HWIOF_DW(MEMNOC_GPU1_QOSBOX_MAINCTL_LOW, SAFESHAPING, 0x2),
};

icbcfg_prop_type icbcfg_boot_prop_sdm845_v1 = 
{
  /* Length of the config  data array */
  ARRAY_SIZE(icbcfg_boot_data_sdm845_v1),
  /* Pointer to config data array */ 
  icbcfg_boot_data_sdm845_v1
};

icbcfg_data_type icbcfg_post_data_sdm845_v1[] = 
{
  /* Enable ODSC on NoC slaveways.
   * EXCLUDE the following due to HW CR QCTDD03324760:
   * a1_noc_cfg
   * a2_noc_cfg
   * ddrss_cfg
   * mnoc_cfg
   * snoc_cfg
   * ssc_cfg
   * ssc_mpu_cfg
   * tcsr
   * prng
   * EXCLUDE the following because the cores use the AHB for logic in addition to config:
   * pcie_0_cfg
   * pcie_0_slv
   * pcie_1_cfg
   * pcie_1_slv
   * tlmm_north
   * tlmm_south
   * EXCLUDE the following, because they'll be off:
   * boot_rom
   * */
  ICBCFG_HWIOF_DW(GCC_AHB2PHY_SOUTH_CBCR, HW_CTL, 0x1),
//  ICBCFG_HWIOF_DW(GCC_CAMERA_AHB_CBCR, HW_CTL, 0x1), // Crash in TZ
  ICBCFG_HWIOF_DW(GCC_CE1_AHB_CBCR, HW_CTL, 0x1),
//  ICBCFG_HWIOF_DW(GCC_DISP_AHB_CBCR, HW_CTL, 0x1), // Crash in TZ
  ICBCFG_HWIOF_DW(GCC_GLM_AHB_CBCR, HW_CTL, 0x1),
//  ICBCFG_HWIOF_DW(GCC_GPU_CFG_AHB_CBCR, HW_CTL, 0x1), // Crash in TZ
  ICBCFG_HWIOF_DW(GCC_IPA_AHB_CBCR, HW_CTL, 0x1),
//  ICBCFG_HWIOF_DW(GCC_LPASS_SWAY_CBCR, HW_CTL, 0x1), // Crash in TZ
//  ICBCFG_HWIOF_DW(GCC_MSS_CFG_AHB_CBCR, HW_CTL, 0x1), // Modem PIL fails (likely TZ issue)
  ICBCFG_HWIOF_DW(GCC_PDM_AHB_CBCR, HW_CTL, 0x1),
  ICBCFG_HWIOF_DW(GCC_QDSS_CFG_AHB_CBCR, HW_CTL, 0x1),
  ICBCFG_HWIOF_DW(GCC_QMIP_CAMERA_AHB_CBCR, HW_CTL, 0x1),
  ICBCFG_HWIOF_DW(GCC_QMIP_DISP_AHB_CBCR, HW_CTL, 0x1),
  ICBCFG_HWIOF_DW(GCC_QMIP_VIDEO_AHB_CBCR, HW_CTL, 0x1),
//  ICBCFG_HWIOF_DW(GCC_QUPV3_WRAP_0_S_AHB_CBCR, HW_CTL, 0x1), // Crash in Android
//  ICBCFG_HWIOF_DW(GCC_QUPV3_WRAP_1_S_AHB_CBCR, HW_CTL, 0x1), // Crash in Android
  ICBCFG_HWIOF_DW(GCC_RBCPR_CX_AHB_CBCR, HW_CTL, 0x1),
  ICBCFG_HWIOF_DW(GCC_RBCPR_MX_AHB_CBCR, HW_CTL, 0x1),
  ICBCFG_HWIOF_DW(GCC_SEC_CTRL_AHB_CBCR, HW_CTL, 0x1),
  ICBCFG_HWIOF_DW(GCC_SPDM_CFG_AHB_CBCR, HW_CTL, 0x1),
  ICBCFG_HWIOF_DW(GCC_SP_CFG_AHB_CBCR, HW_CTL, 0x1),
//  ICBCFG_HWIOF_DW(GCC_SYS_NOC_SF_TCU_CBCR, HW_CTL, 0x1), // Crash in TZ
//  ICBCFG_HWIOF_DW(GCC_TURING_CFG_AHB_CBCR, HW_CTL, 0x1), // Android fails to load CDSP
  ICBCFG_HWIOF_DW(GCC_UFS_CARD_AHB_CBCR, HW_CTL, 0x1),
//  ICBCFG_HWIOF_DW(GCC_UFS_PHY_AHB_CBCR, HW_CTL, 0x1), // Crash in Android
//  ICBCFG_HWIOF_DW(GCC_USB_PHY_CFG_AHB2PHY_CBCR, HW_CTL, 0x1), // Crash in Android
  ICBCFG_HWIOF_DW(GCC_VIDEO_AHB_CBCR, HW_CTL, 0x1),
  ICBCFG_HWIOF_DW(GCC_VS_CTRL_AHB_CBCR, HW_CTL, 0x1),
};

icbcfg_prop_type icbcfg_post_prop_sdm845_v1 = 
{
  /* Length of the config  data array */
  ARRAY_SIZE(icbcfg_post_data_sdm845_v1),
  /* Pointer to config data array */ 
  icbcfg_post_data_sdm845_v1                                    
};

icbcfg_mem_region_type map_ddr_regions_sdm845_v1[] =
{
  { 0x080000000ULL, 0x400000000ULL },
};

/* Translation block base addresses. */
uint8 *trans_bases_sdm845_v1[] = 
{
  (uint8 *)HWIO_ADDR(MEMNOC_LLCC0_ADDR_TRANSLATOR_SWID_LOW),
  (uint8 *)HWIO_ADDR(MEMNOC_LLCC1_ADDR_TRANSLATOR_SWID_LOW),
  (uint8 *)HWIO_ADDR(MEMNOC_LLCC2_ADDR_TRANSLATOR_SWID_LOW),
  (uint8 *)HWIO_ADDR(MEMNOC_LLCC3_ADDR_TRANSLATOR_SWID_LOW)
};

icbcfg_device_config_type sdm845_v1 =
{
  /* Chip version information for this device data. */
  DALCHIPINFO_FAMILY_SDM845,   /**< Chip family */
  false,                       /**< Exact match for version? */
  0,                           /**< Chip version */

  /* Device information. */
  ARRAY_SIZE(map_ddr_regions_sdm845_v1), /**< Number of regions in the DDR map */
  map_ddr_regions_sdm845_v1,             /**< Array of mappable DDR regions */
  6,                                     /**< Number of mapping segments. */
  4,                                     /**< Number of channels. */
  36,                                    /**< Address width in bits. */
  ICBCFG_ADDR_TRANS_NOC,                 /**< Address translator hardware type. */
  trans_bases_sdm845_v1,                 /**< Translation block base addresses. */

  /* Prop lists. */
  &icbcfg_boot_prop_sdm845_v1,           /**< Init time prop list. */
  &icbcfg_post_prop_sdm845_v1,           /**< Post-init time prop list. */
};

/*---------------------------------------------------------------------------*/
/* SDM845 v2                                                                 */
/*---------------------------------------------------------------------------*/
/* Translation block base addresses. */
uint8 *trans_bases_sdm845_v2[] = 
{
  (uint8 *)HWIO_ADDR(LLCC0_LLCC_BEAC_ADDR_TRANSLATOR_CFG),
  (uint8 *)HWIO_ADDR(LLCC1_LLCC_BEAC_ADDR_TRANSLATOR_CFG),
  (uint8 *)HWIO_ADDR(LLCC2_LLCC_BEAC_ADDR_TRANSLATOR_CFG),
  (uint8 *)HWIO_ADDR(LLCC3_LLCC_BEAC_ADDR_TRANSLATOR_CFG),
};

icbcfg_device_config_type sdm845_v2 =
{
  /* Chip version information for this device data. */
  DALCHIPINFO_FAMILY_SDM845,   /**< Chip family */
  false,                       /**< Exact match for version? */
  CHIPINFO_VERSION(2,0),       /**< Chip version */

  /* Device information.
   * Reused from v1 when identical. */
  ARRAY_SIZE(map_ddr_regions_sdm845_v1), /**< Number of regions in the DDR map */
  map_ddr_regions_sdm845_v1,             /**< Array of mappable DDR regions */
  6,                                     /**< Number of mapping segments. */
  4,                                     /**< Number of channels. */
  36,                                    /**< Address width in bits. */
  ICBCFG_ADDR_TRANS_LLCC,                /**< Address translator hardware type. */
  trans_bases_sdm845_v2,                 /**< Translation block base addresses. */

  /* Prop lists. */
  &icbcfg_boot_prop_sdm845_v1,           /**< Init time prop list. */
  &icbcfg_post_prop_sdm845_v1,           /**< Post-init time prop list. */
};

/*---------------------------------------------------------------------------*/
/* SDM670                                                                 */
/*---------------------------------------------------------------------------*/

icbcfg_data_type icbcfg_boot_data_sdm670[] = 
{
  /* Force SSID 13 for TCU accesses */
  ICBCFG_HWIOF_DW(MEMNOC_TCU_DYN_ATTR_CACHEINDEXVAL_LOW, CACHEINDEXVAL, 13),
  ICBCFG_HWIOF_DW(MEMNOC_TCU_DYN_ATTR_MAINCTL_LOW, CACHEINDEXOV, 0x1),
#if 1
  /* DSAT core registers */
  ICBCFG_HWIOF_DW(GCC_MMSS_QM_CORE_CBCR, CLK_ENABLE, 0x1),
  ICBCFG_WAIT_HWIOF_DW(GCC_MMSS_QM_CORE_CBCR, CLK_OFF, 0x0),
  ICBCFG_HWIOF_DW(GCC_MMSS_QM_AHB_CBCR, CLK_ENABLE, 0x1),
  ICBCFG_WAIT_HWIOF_DW(GCC_MMSS_QM_AHB_CBCR, CLK_OFF, 0x0),
  ICBCFG_HWIO_DW(QM_CORE_CLK_CGC_CNTRL, 0x1),
  ICBCFG_HWIO_DW(QM_PRE_STALL_TIMEOUT_CNT_URG_0, 0x60),
  ICBCFG_HWIO_DW(QM_PRE_STALL_TIMEOUT_CNT_URG_1, 0x27),
  ICBCFG_HWIO_DW(QM_PRE_STALL_TIMEOUT_CNT_URG_2, 0x27),
  ICBCFG_HWIO_DW(QM_PRE_STALL_TIMEOUT_CNT_URG_3, 0x27),
  ICBCFG_HWIO_DW(QM_POST_STALL_TIMEOUT_CNT_URG_0, 0x60),
  ICBCFG_HWIO_DW(QM_POST_STALL_TIMEOUT_CNT_URG_1, 0x27),
  ICBCFG_HWIO_DW(QM_POST_STALL_TIMEOUT_CNT_URG_2, 0x27),
  ICBCFG_HWIO_DW(QM_POST_STALL_TIMEOUT_CNT_URG_3, 0x27),
  ICBCFG_HWIO_DW(QM_POST_STALL_WDW_OVERLAP_CNTL, 0x1),
  ICBCFG_HWIO_DW(QM_THROTTLE_LEVEL_LEGACY_SELECT, 0x0),
  ICBCFG_HWIOI_DW(QM_THROTTLE_LEVEL_QOS_n, 7, 0x7FC),
  ICBCFG_HWIO_DW(QM_THROTTLE_LEVEL_SU_CFG, 0x7F),
  ICBCFG_HWIO_DW(QM_CLNT_8_DANGER_AGGR_CNTRL, 0x3),
  ICBCFG_HWIO_DW(QM_CLNT_8_DANGER_AGGR_MODEM_CNTRL, 0x0),
  ICBCFG_HWIOI_DW(QM_CLNT_8_THROTTLE_LEVEL_QOS_n, 3, 0xEE000003),
  ICBCFG_HWIOI_DW(QM_CLNT_8_THROTTLE_LEVEL_QOS_n, 7, 0xAA000000),
  ICBCFG_HWIO_DW(QM_CLNT_8_THROTTLE_LEVEL_OUTPUT_EN, 0x1),
  ICBCFG_HWIO_DW(QM_CLNT_9_DANGER_AGGR_CNTRL, 0x3),
  ICBCFG_HWIO_DW(QM_CLNT_9_DANGER_AGGR_MODEM_CNTRL, 0x0),
  ICBCFG_HWIOI_DW(QM_CLNT_9_THROTTLE_LEVEL_QOS_n, 1, 0xEE000001),
  ICBCFG_HWIOI_DW(QM_CLNT_9_THROTTLE_LEVEL_QOS_n, 2, 0xEE000002),
  ICBCFG_HWIOI_DW(QM_CLNT_9_THROTTLE_LEVEL_QOS_n, 3, 0xEE000003),
  ICBCFG_HWIOI_DW(QM_CLNT_9_THROTTLE_LEVEL_QOS_n, 7, 0xAA000000),
  ICBCFG_HWIO_DW(QM_CLNT_9_THROTTLE_LEVEL_OUTPUT_EN, 0x1),
  ICBCFG_HWIOI_DW(QM_CLNT_10_THROTTLE_LEVEL_QOS_n, 1, 0xEE000001),
  ICBCFG_HWIOI_DW(QM_CLNT_10_THROTTLE_LEVEL_QOS_n, 2, 0xEE000002),
  ICBCFG_HWIOI_DW(QM_CLNT_10_THROTTLE_LEVEL_QOS_n, 3, 0xEE000003),
  ICBCFG_HWIOI_DW(QM_CLNT_10_THROTTLE_LEVEL_QOS_n, 7, 0xAA000000),
  ICBCFG_HWIO_DW(QM_CLNT_10_THROTTLE_LEVEL_OUTPUT_EN, 0x1),
  ICBCFG_HWIO_DW(QM_DSP_DANGER_AGGR_CNTRL, 0x3),
  ICBCFG_HWIO_DW(QM_DSP_DANGER_AGGR_MODEM_CNTRL, 0x0),
  ICBCFG_HWIO_DW(QM_DSP_PRIORITY_MODE_SEL, 0x1),
  ICBCFG_HWIO_DW(QM_DSP_SYSTEM_DANGER_AGGR_MODEM_CNTRL, 0x0),
  ICBCFG_HWIO_DW(QM_DSP_SYSTEM_DANGER_OUTPUT_EN, 0x0),
  ICBCFG_HWIOF_READ_DW(CABO0_CABO_GLOBAL_DDR_CLK_PERIOD_CFG,DDR_CLK_PERIOD),
  ICBCFG_HWIOF_WRITE_DW(QM_PERIOD_BUS_CFG,PERIOD_BUS_SW),
  ICBCFG_HWIOF_DW(QM_PERIOD_BUS_CFG, PERIOD_BUS_LOAD_SW, 0x1),
  ICBCFG_HWIOF_DW(QM_PERIOD_BUS_CFG, PERIOD_BUS_SW_OVERRIDE, 0x0),
  ICBCFG_HWIOI_DW(QOS_FREQ_BAND_BNDRY_n, 0, 0x97B),
  ICBCFG_HWIOI_DW(QOS_FREQ_BAND_BNDRY_n, 1, 0x0),
  ICBCFG_HWIOI_DW(QOS_FREQ_BAND_BNDRY_n, 2, 0x0),
  ICBCFG_HWIO_DW(QM_AGGR_SAFE_OVERRIDE_CNTRL, 0x1),
  ICBCFG_HWIO_DW(QM_DANGER_SAFE_CNTRL,0x1),

  /* AP throttles and shapers */
  ICBCFG_HWIOF_DW(APP_BWMON_THROTTLE_0_CGC_CNTRL, THROTTLE_CGC_EN, 0x1),
  ICBCFG_HWIOF_DW(APP_BWMON_THROTTLE_0_GRANT_PERIOD, GRANT_PERIOD, 0x3e8),
  ICBCFG_HWIO_DW(APP_BWMON_THROTTLE_0_THRESHOLD_03, 0xFFFFFFFF),
  ICBCFG_HWIO_DW(APP_BWMON_THROTTLE_0_THRESHOLD_02, 0xFFFF0064),
  ICBCFG_HWIO_DW(APP_BWMON_THROTTLE_0_THRESHOLD_01, 0xFFFFFFFF),
  ICBCFG_HWIO_DW(APP_BWMON_THROTTLE_0_THRESHOLD_00, 0xFFFFFFFF),
  ICBCFG_HWIOF_DW(APP_BWMON_THROTTLE_0_PEAK_ACCUM_CREDIT, PEAK_ACCUM_CREDIT, 0x100),
  ICBCFG_HWIO_DW(APP_BWMON_THROTTLE_0_CNTRL, 0x0E01),

  ICBCFG_HWIOF_DW(APP_BWMON_THROTTLE_1_CGC_CNTRL, THROTTLE_CGC_EN, 0x1),
  ICBCFG_HWIOF_DW(APP_BWMON_THROTTLE_1_GRANT_PERIOD, GRANT_PERIOD, 0x3e8),
  ICBCFG_HWIO_DW(APP_BWMON_THROTTLE_1_THRESHOLD_03, 0xFFFFFFFF),
  ICBCFG_HWIO_DW(APP_BWMON_THROTTLE_1_THRESHOLD_02, 0xFFFF0064),
  ICBCFG_HWIO_DW(APP_BWMON_THROTTLE_1_THRESHOLD_01, 0xFFFFFFFF),
  ICBCFG_HWIO_DW(APP_BWMON_THROTTLE_1_THRESHOLD_00, 0xFFFFFFFF),
  ICBCFG_HWIOF_DW(APP_BWMON_THROTTLE_1_PEAK_ACCUM_CREDIT, PEAK_ACCUM_CREDIT, 0x100),
  ICBCFG_HWIO_DW(APP_BWMON_THROTTLE_1_CNTRL, 0x0E01),

  ICBCFG_HWIOF_DW(MEMNOC_APPS_RD_QOSBOX_MAINCTL_LOW, SAFESHAPING, 0x2),
  ICBCFG_HWIOF_DW(MEMNOC_APPS_RDWR_QOSBOX_MAINCTL_LOW, SAFESHAPING, 0x2),

  /* CDSP throttle and shaper */
  ICBCFG_HWIOF_DW(CDSP_BWMON_THROTTLE_0_CGC_CNTRL, THROTTLE_CGC_EN, 0x1),
  ICBCFG_HWIOF_DW(CDSP_BWMON_THROTTLE_0_GRANT_PERIOD, GRANT_PERIOD, 0x3e8),
  ICBCFG_HWIO_DW(CDSP_BWMON_THROTTLE_0_THRESHOLD_03, 0xFFFFFFFF),
  ICBCFG_HWIO_DW(CDSP_BWMON_THROTTLE_0_THRESHOLD_02, 0xFFFF0064),
  ICBCFG_HWIO_DW(CDSP_BWMON_THROTTLE_0_THRESHOLD_01, 0xFFFFFFFF),
  ICBCFG_HWIO_DW(CDSP_BWMON_THROTTLE_0_THRESHOLD_00, 0xFFFFFFFF),
  ICBCFG_HWIOF_DW(CDSP_BWMON_THROTTLE_0_PEAK_ACCUM_CREDIT, PEAK_ACCUM_CREDIT, 0x100),
  ICBCFG_HWIO_DW(CDSP_BWMON_THROTTLE_0_CNTRL, 0x0E01),

  ICBCFG_HWIOF_DW(MEMNOC_CDSP_QOSBOX_MAINCTL_LOW, SAFESHAPING, 0x2),

  /* GPU throttles and shapers */
  ICBCFG_HWIOF_DW(GPU_BWMON_THROTTLE_0_CGC_CNTRL, THROTTLE_CGC_EN, 0x1),
  ICBCFG_HWIOF_DW(GPU_BWMON_THROTTLE_0_GRANT_PERIOD, GRANT_PERIOD, 0x3e8),
  ICBCFG_HWIO_DW(GPU_BWMON_THROTTLE_0_THRESHOLD_03, 0xFFFFFFFF),
  ICBCFG_HWIO_DW(GPU_BWMON_THROTTLE_0_THRESHOLD_02, 0xFFFF0032),
  ICBCFG_HWIO_DW(GPU_BWMON_THROTTLE_0_THRESHOLD_01, 0xFFFFFFFF),
  ICBCFG_HWIO_DW(GPU_BWMON_THROTTLE_0_THRESHOLD_00, 0xFFFFFFFF),
  ICBCFG_HWIOF_DW(GPU_BWMON_THROTTLE_0_PEAK_ACCUM_CREDIT, PEAK_ACCUM_CREDIT, 0x100),
  ICBCFG_HWIO_DW(GPU_BWMON_THROTTLE_0_CNTRL, 0x0E01),

  ICBCFG_HWIOF_DW(GPU_BWMON_THROTTLE_1_CGC_CNTRL, THROTTLE_CGC_EN, 0x1),
  ICBCFG_HWIOF_DW(GPU_BWMON_THROTTLE_1_GRANT_PERIOD, GRANT_PERIOD, 0x3e8),
  ICBCFG_HWIO_DW(GPU_BWMON_THROTTLE_1_THRESHOLD_03, 0xFFFFFFFF),
  ICBCFG_HWIO_DW(GPU_BWMON_THROTTLE_1_THRESHOLD_02, 0xFFFF0032),
  ICBCFG_HWIO_DW(GPU_BWMON_THROTTLE_1_THRESHOLD_01, 0xFFFFFFFF),
  ICBCFG_HWIO_DW(GPU_BWMON_THROTTLE_1_THRESHOLD_00, 0xFFFFFFFF),
  ICBCFG_HWIOF_DW(GPU_BWMON_THROTTLE_1_PEAK_ACCUM_CREDIT, PEAK_ACCUM_CREDIT, 0x100),
  ICBCFG_HWIO_DW(GPU_BWMON_THROTTLE_1_CNTRL, 0x0E01),

  ICBCFG_HWIOF_DW(MEMNOC_GPU0_QOSBOX_MAINCTL_LOW, SAFESHAPING, 0x2),
  ICBCFG_HWIOF_DW(MEMNOC_GPU1_QOSBOX_MAINCTL_LOW, SAFESHAPING, 0x2),
#endif
};

icbcfg_prop_type icbcfg_boot_prop_sdm670 = 
{
  /* Length of the config  data array */
  ARRAY_SIZE(icbcfg_boot_data_sdm670),
  /* Pointer to config data array */ 
  icbcfg_boot_data_sdm670
};

icbcfg_data_type icbcfg_post_data_sdm670[] = 
{
 // ICBCFG_DEFAULT(),
#if 1
  /* Enable ODSC on NoC slaveways.
   * EXCLUDE the following due to HW CR QCTDD03324760:
   * a1_noc_cfg
   * a2_noc_cfg
   * ddrss_cfg
   * mnoc_cfg
   * snoc_cfg
   * ssc_cfg
   * ssc_mpu_cfg
   * tcsr
   * prng
   * EXCLUDE the following because the cores use the AHB for logic in addition to config:
   * pcie_0_cfg
   * pcie_0_slv
   * pcie_1_cfg
   * pcie_1_slv
   * tlmm_north
   * tlmm_south
   * tlmm west added in WL
   * EXCLUDE the following, because they'll be off:
   * boot_rom
   * */
  ICBCFG_HWIOF_DW(GCC_AHB2PHY_SOUTH_CBCR, HW_CTL, 0x1),
//  ICBCFG_HWIOF_DW(GCC_CAMERA_AHB_CBCR, HW_CTL, 0x1), // Crash in TZ
  ICBCFG_HWIOF_DW(GCC_CE1_AHB_CBCR, HW_CTL, 0x1),
//  ICBCFG_HWIOF_DW(GCC_DISP_AHB_CBCR, HW_CTL, 0x1), // Crash in TZ
  ICBCFG_HWIOF_DW(GCC_GLM_AHB_CBCR, HW_CTL, 0x1),
//  ICBCFG_HWIOF_DW(GCC_GPU_CFG_AHB_CBCR, HW_CTL, 0x1), // Crash in TZ
  ICBCFG_HWIOF_DW(GCC_IPA_AHB_CBCR, HW_CTL, 0x1),
//  ICBCFG_HWIOF_DW(GCC_LPASS_SWAY_CBCR, HW_CTL, 0x1), // Crash in TZ
//  ICBCFG_HWIOF_DW(GCC_MSS_CFG_AHB_CBCR, HW_CTL, 0x1), // Modem PIL fails (likely TZ issue)
  ICBCFG_HWIOF_DW(GCC_PDM_AHB_CBCR, HW_CTL, 0x1),
  ICBCFG_HWIOF_DW(GCC_QDSS_CFG_AHB_CBCR, HW_CTL, 0x1),
  ICBCFG_HWIOF_DW(GCC_QMIP_CAMERA_AHB_CBCR, HW_CTL, 0x1),
  ICBCFG_HWIOF_DW(GCC_QMIP_DISP_AHB_CBCR, HW_CTL, 0x1),
  ICBCFG_HWIOF_DW(GCC_QMIP_VIDEO_AHB_CBCR, HW_CTL, 0x1),
//  ICBCFG_HWIOF_DW(GCC_QUPV3_WRAP_0_S_AHB_CBCR, HW_CTL, 0x1), // Crash in Android
//  ICBCFG_HWIOF_DW(GCC_QUPV3_WRAP_1_S_AHB_CBCR, HW_CTL, 0x1), // Crash in Android
  ICBCFG_HWIOF_DW(GCC_RBCPR_CX_AHB_CBCR, HW_CTL, 0x1),
  ICBCFG_HWIOF_DW(GCC_RBCPR_MX_AHB_CBCR, HW_CTL, 0x1),
  ICBCFG_HWIOF_DW(GCC_SEC_CTRL_AHB_CBCR, HW_CTL, 0x1),
  ICBCFG_HWIOF_DW(GCC_SPDM_CFG_AHB_CBCR, HW_CTL, 0x1),
  //ICBCFG_HWIOF_DW(GCC_SP_CFG_AHB_CBCR, HW_CTL, 0x1), //Not in WL
//  ICBCFG_HWIOF_DW(GCC_SYS_NOC_SF_TCU_CBCR, HW_CTL, 0x1), // Crash in TZ
//  ICBCFG_HWIOF_DW(GCC_TURING_CFG_AHB_CBCR, HW_CTL, 0x1), // Android fails to load CDSP
  //ICBCFG_HWIOF_DW(GCC_UFS_CARD_AHB_CBCR, HW_CTL, 0x1), //Not in WL
//  ICBCFG_HWIOF_DW(GCC_UFS_PHY_AHB_CBCR, HW_CTL, 0x1), // Crash in Android
//  ICBCFG_HWIOF_DW(GCC_USB_PHY_CFG_AHB2PHY_CBCR, HW_CTL, 0x1), // Crash in Android
  ICBCFG_HWIOF_DW(GCC_VIDEO_AHB_CBCR, HW_CTL, 0x1),
  ICBCFG_HWIOF_DW(GCC_VS_CTRL_AHB_CBCR, HW_CTL, 0x1),
  ICBCFG_HWIOF_DW(GCC_TLMM_WEST_AHB_CBCR, HW_CTL, 0x1), //Added in WL
#endif
};

icbcfg_prop_type icbcfg_post_prop_sdm670 = 
{
  /* Length of the config  data array */
  ARRAY_SIZE(icbcfg_post_data_sdm670),
  /* Pointer to config data array */ 
  icbcfg_post_data_sdm670                                    
};

icbcfg_mem_region_type map_ddr_regions_sdm670[] =
{
  { 0x080000000ULL, 0x400000000ULL },
};

uint8 *trans_bases_sdm670[] = 
{
  (uint8 *)HWIO_ADDR(LLCC0_LLCC_BEAC_ADDR_TRANSLATOR_CFG),
  (uint8 *)HWIO_ADDR(LLCC1_LLCC_BEAC_ADDR_TRANSLATOR_CFG),
};
icbcfg_device_config_type sdm670 =
{
  /* Chip version information for this device data. */
  CHIPINFO_FAMILY_SDM670,   /**< Chip family */
  false,                       /**< Exact match for version? */
  CHIPINFO_VERSION(0,0),       /**< Chip version */

  /* Device information. */
  ARRAY_SIZE(map_ddr_regions_sdm670), /**< Number of regions in the DDR map */
  map_ddr_regions_sdm670,             /**< Array of mappable DDR regions */
  6,                                     /**< Number of mapping segments. */
  2,                                     /**< Number of channels. */
  36,                                    /**< Address width in bits. */
  ICBCFG_ADDR_TRANS_LLCC,                /**< Address translator hardware type. */
  trans_bases_sdm670,                 /**< Translation block base addresses. */

  /* Prop lists. */
  &icbcfg_boot_prop_sdm670,           /**< Init time prop list. */
  &icbcfg_post_prop_sdm670,           /**< Post-init time prop list. */
};

/* Red Dragon device config */
icbcfg_device_config_type qcs605 =
{
  /* Chip version information for this device data. */
  CHIPINFO_FAMILY_QCS605,   /**< Chip family */
  false,                       /**< Exact match for version? */
  CHIPINFO_VERSION(0,0),       /**< Chip version */

  /* Device information. */
  ARRAY_SIZE(map_ddr_regions_sdm670), /**< Number of regions in the DDR map */
  map_ddr_regions_sdm670,             /**< Array of mappable DDR regions */
  6,                                     /**< Number of mapping segments. */
  2,                                     /**< Number of channels. */
  36,                                    /**< Address width in bits. */
  ICBCFG_ADDR_TRANS_LLCC,                /**< Address translator hardware type. */
  trans_bases_sdm670,                 /**< Translation block base addresses. */

  /* Prop lists. */
  &icbcfg_boot_prop_sdm670,           /**< Init time prop list. */
  &icbcfg_post_prop_sdm670,           /**< Post-init time prop list. */
};

icbcfg_device_config_type sxr1130 =
{
  /* Chip version information for this device data. */
  CHIPINFO_FAMILY_SXR1130,   /**< Chip family */
  false,                       /**< Exact match for version? */
  CHIPINFO_VERSION(0,0),       /**< Chip version */

  /* Device information. */
  ARRAY_SIZE(map_ddr_regions_sdm670), /**< Number of regions in the DDR map */
  map_ddr_regions_sdm670,             /**< Array of mappable DDR regions */
  6,                                     /**< Number of mapping segments. */
  2,                                     /**< Number of channels. */
  36,                                    /**< Address width in bits. */
  ICBCFG_ADDR_TRANS_LLCC,                /**< Address translator hardware type. */
  trans_bases_sdm670,                 /**< Translation block base addresses. */

  /* Prop lists. */
  &icbcfg_boot_prop_sdm670,           /**< Init time prop list. */
  &icbcfg_post_prop_sdm670,           /**< Post-init time prop list. */
};

/*---------------------------------------------------------------------------*/
/* TARGET LIST                                                               */
/*---------------------------------------------------------------------------*/
icbcfg_device_config_type *configs_sdm845[] =
{
  &sdm845_v2,
  &sdm845_v1,
  &sdm670,
  &qcs605,
  &sxr1130,
};

/*---------------------------------------------------------------------------*/
/* EXPORTED TARGET DEFINITION                                                */
/*---------------------------------------------------------------------------*/
icbcfg_info_type icbcfg_info =
{
  ARRAY_SIZE(configs_sdm845),
  configs_sdm845,
};
