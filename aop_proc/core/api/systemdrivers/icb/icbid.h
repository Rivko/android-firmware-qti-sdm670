#ifndef __ICBID_H__
#define __ICBID_H__
/**
 * @file icbid.h 
 * @note Private header file. Defines Internal Chip Bus interface ids
 *  
 *  
 *                REVISION  HISTORY
 *  
 * This section contains comments describing changes made to this file. Notice
 * that changes are listed in reverse chronological order.
 * 
 * $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/api/systemdrivers/icb/icbid.h#1 $ 
 * $DateTime: 2018/02/16 04:48:18 $ 
 * $Author: pwbldsvc $ 
 * 
 * when          who     what, where, why
 * -----------   ---     ---------------------------------------------------
 *  06/22/2017   sds     Alias QUP to BLSP
 *  04/24/2017   sds     Add camnoc virt ports
 *  03/30/2017   sds     Add split camnoc port
 *  01/04/2017   sds     Add ports for sdx24.
 *  07/14/2016   sds     Add ports for SDM845.
 *  08/04/2015   sds     Add ports for 8998.
 *  08/01/2015   jc      Add ports for 9x55.
 *  03/12/2015   sds     Add ports for 8996 CE.
 *  10/14/2014   sds     Add ports for 8996v2.
 *  07/30/2014   tb      Add ports for 8909.
 *  01/27/2014   sds     Add ports for 9x45.
 *  01/27/2014   sds     Add ports for 8936.
 *  11/17/2013   jc      Add ports for 8994.
 *  09/06/2013   jc      Add ports for 8092.
 *  06/28/2013   sds     Add ports for 9x35.
 *  02/20/2013   sds     Add ports for 8084.
 *  02/15/2013   jc      Added 2nd MDP master port.
 *  11/27/2012   jc      Added MIPI-DSI master and slave.
 *  07/06/2012   jc      Added new masters and slaves.
 *  03/02/2012   sds     Correct slave name for RPM message RAM.
 *  04/02/2010   sds     Initial revision
 *  
 *  
 * Copyright (c) 2010-2017 by QUALCOMM Technologies, Incorporated.
 *                      All rights reserved. 
 *                   QUALCOMM Proprietary/GTDR
 */

/**
 * Master Identifiers 
 */ 
typedef enum {
  ICBID_MASTER_APPSS_PROC = 0,
  ICBID_MASTER_MSS_PROC,
  ICBID_MASTER_MNOC_BIMC,
  ICBID_MASTER_MNOC_HF_MEM_NOC = ICBID_MASTER_MNOC_BIMC,
  ICBID_MASTER_SNOC_BIMC,
  ICBID_MASTER_SNOC_BIMC_0 = ICBID_MASTER_SNOC_BIMC,
  ICBID_MASTER_SNOC_GC_MEM_NOC = ICBID_MASTER_SNOC_BIMC,
  ICBID_MASTER_CNOC_MNOC_MMSS_CFG,
  ICBID_MASTER_CNOC_MNOC_CFG,
  ICBID_MASTER_GFX3D,
  ICBID_MASTER_JPEG,
  ICBID_MASTER_MDP,
  ICBID_MASTER_MDP0 = ICBID_MASTER_MDP,
  ICBID_MASTER_MDPS = ICBID_MASTER_MDP,
  ICBID_MASTER_VIDEO,
  ICBID_MASTER_VIDEO_P0 = ICBID_MASTER_VIDEO,
  ICBID_MASTER_VIDEO_P1,
  ICBID_MASTER_VFE,
  ICBID_MASTER_VFE0 = ICBID_MASTER_VFE,
  ICBID_MASTER_CNOC_ONOC_CFG,
  ICBID_MASTER_JPEG_OCMEM,
  ICBID_MASTER_MDP_OCMEM,
  ICBID_MASTER_VIDEO_P0_OCMEM,
  ICBID_MASTER_VIDEO_P1_OCMEM,
  ICBID_MASTER_VFE_OCMEM,
  ICBID_MASTER_LPASS_AHB,
  ICBID_MASTER_QDSS_BAM,
  ICBID_MASTER_SNOC_CFG,
  ICBID_MASTER_BIMC_SNOC,
  ICBID_MASTER_BIMC_SNOC_0 = ICBID_MASTER_BIMC_SNOC,
  ICBID_MASTER_MEM_NOC_SNOC = ICBID_MASTER_BIMC_SNOC,
  ICBID_MASTER_CNOC_SNOC,
  ICBID_MASTER_CRYPTO,
  ICBID_MASTER_CRYPTO_CORE0 = ICBID_MASTER_CRYPTO,
  ICBID_MASTER_CRYPTO_CORE1,
  ICBID_MASTER_LPASS_PROC,
  ICBID_MASTER_MSS,
  ICBID_MASTER_MSS_NAV,
  ICBID_MASTER_OCMEM_DMA,
  ICBID_MASTER_PNOC_SNOC,
  ICBID_MASTER_WCSS,
  ICBID_MASTER_QDSS_ETR,
  ICBID_MASTER_USB3,
  ICBID_MASTER_USB3_0 = ICBID_MASTER_USB3,
  ICBID_MASTER_SDCC_1,
  ICBID_MASTER_SDCC_3,
  ICBID_MASTER_SDCC_2,
  ICBID_MASTER_SDCC_4,
  ICBID_MASTER_TSIF,
  ICBID_MASTER_BAM_DMA,
  ICBID_MASTER_BLSP_2,
  ICBID_MASTER_QUP_2 = ICBID_MASTER_BLSP_2,
  ICBID_MASTER_USB_HSIC,
  ICBID_MASTER_BLSP_1,
  ICBID_MASTER_QUP_1 = ICBID_MASTER_BLSP_1,
  ICBID_MASTER_USB_HS,
  ICBID_MASTER_USB_HS1 = ICBID_MASTER_USB_HS,
  ICBID_MASTER_PNOC_CFG,
  ICBID_MASTER_SNOC_PNOC,
  ICBID_MASTER_RPM_INST,
  ICBID_MASTER_RPM_DATA,
  ICBID_MASTER_RPM_SYS,
  ICBID_MASTER_DEHR,
  ICBID_MASTER_QDSS_DAP,
  ICBID_MASTER_SPDM,
  ICBID_MASTER_TIC,
  ICBID_MASTER_SNOC_CNOC,
  ICBID_MASTER_GFX3D_OCMEM,
  ICBID_MASTER_GFX3D_GMEM = ICBID_MASTER_GFX3D_OCMEM,
  ICBID_MASTER_OVIRT_SNOC,
  ICBID_MASTER_SNOC_OVIRT,
  ICBID_MASTER_SNOC_GVIRT = ICBID_MASTER_SNOC_OVIRT,
  ICBID_MASTER_ONOC_OVIRT,
  ICBID_MASTER_USB_HS2,
  ICBID_MASTER_QPIC,
  ICBID_MASTER_IPA,
  ICBID_MASTER_DSI,
  ICBID_MASTER_MDP1,
  ICBID_MASTER_MDPE = ICBID_MASTER_MDP1,
  ICBID_MASTER_VPU_PROC,
  ICBID_MASTER_VPU,
  ICBID_MASTER_VPU0 = ICBID_MASTER_VPU,
  ICBID_MASTER_CRYPTO_CORE2,
  ICBID_MASTER_PCIE_0,
  ICBID_MASTER_PCIE_1,
  ICBID_MASTER_SATA,
  ICBID_MASTER_UFS,
  ICBID_MASTER_UFS_CARD = ICBID_MASTER_UFS,
  ICBID_MASTER_USB3_1,
  ICBID_MASTER_VIDEO_OCMEM,
  ICBID_MASTER_VPU1,
  ICBID_MASTER_VCAP,
  ICBID_MASTER_EMAC,
  ICBID_MASTER_BCAST,
  ICBID_MASTER_MMSS_PROC,
  ICBID_MASTER_SNOC_BIMC_1,
  ICBID_MASTER_SNOC_SF_MEM_NOC = ICBID_MASTER_SNOC_BIMC_1,
  ICBID_MASTER_SNOC_PCNOC,
  ICBID_MASTER_AUDIO,
  ICBID_MASTER_MM_INT_0,
  ICBID_MASTER_MM_INT_1,
  ICBID_MASTER_MM_INT_2,
  ICBID_MASTER_MM_INT_BIMC,
  ICBID_MASTER_MSS_INT,
  ICBID_MASTER_PCNOC_CFG,
  ICBID_MASTER_PCNOC_INT_0,
  ICBID_MASTER_PCNOC_INT_1,
  ICBID_MASTER_PCNOC_M_0,
  ICBID_MASTER_PCNOC_M_1,
  ICBID_MASTER_PCNOC_S_0,
  ICBID_MASTER_PCNOC_S_1,
  ICBID_MASTER_PCNOC_S_2,
  ICBID_MASTER_PCNOC_S_3,
  ICBID_MASTER_PCNOC_S_4,
  ICBID_MASTER_PCNOC_S_6,
  ICBID_MASTER_PCNOC_S_7,
  ICBID_MASTER_PCNOC_S_8,
  ICBID_MASTER_PCNOC_S_9,
  ICBID_MASTER_QDSS_INT,
  ICBID_MASTER_SNOC_INT_0,
  ICBID_MASTER_SNOC_INT_1,
  ICBID_MASTER_SNOC_INT_BIMC,
  ICBID_MASTER_TCU_0,
  ICBID_MASTER_TCU_1,
  ICBID_MASTER_BIMC_INT_0,
  ICBID_MASTER_BIMC_INT_1,
  ICBID_MASTER_CAMERA,
  ICBID_MASTER_RICA,
  ICBID_MASTER_SNOC_BIMC_2,
  ICBID_MASTER_BIMC_SNOC_1,
  ICBID_MASTER_A0NOC_SNOC,
  ICBID_MASTER_ANOC_SNOC = ICBID_MASTER_A0NOC_SNOC,
  ICBID_MASTER_A1NOC_SNOC,
  ICBID_MASTER_A2NOC_SNOC,
  ICBID_MASTER_PIMEM,
  ICBID_MASTER_SNOC_VMEM,
  ICBID_MASTER_CPP,
  ICBID_MASTER_CNOC_A1NOC,
  ICBID_MASTER_PNOC_A1NOC,
  ICBID_MASTER_HMSS,
  ICBID_MASTER_PCIE_2,
  ICBID_MASTER_ROTATOR,
  ICBID_MASTER_VENUS_VMEM,
  ICBID_MASTER_DCC,
  ICBID_MASTER_MCDMA,
  ICBID_MASTER_PCNOC_INT_2,
  ICBID_MASTER_PCNOC_INT_3,
  ICBID_MASTER_PCNOC_INT_4,
  ICBID_MASTER_PCNOC_INT_5,
  ICBID_MASTER_PCNOC_INT_6,
  ICBID_MASTER_PCNOC_S_5,
  ICBID_MASTER_SENSORS_AHB,
  ICBID_MASTER_SENSORS_PROC,
  ICBID_MASTER_QSPI,
  ICBID_MASTER_VFE1,
  ICBID_MASTER_SNOC_INT_2,
  ICBID_MASTER_SMMNOC_BIMC,
  ICBID_MASTER_CRVIRT_A1NOC,
  ICBID_MASTER_XM_USB_HS1,
  ICBID_MASTER_XI_USB_HS1,
  ICBID_MASTER_PCNOC_BIMC_1,
  ICBID_MASTER_BIMC_PCNOC,
  ICBID_MASTER_XI_HSIC,
  ICBID_MASTER_SGMII,
  ICBID_MASTER_SPMI_FETCHER,
  ICBID_MASTER_GNOC_BIMC,
  ICBID_MASTER_GNOC_MEM_NOC = ICBID_MASTER_GNOC_BIMC,
  ICBID_MASTER_CRVIRT_A2NOC,
  ICBID_MASTER_CNOC_A2NOC,
  ICBID_MASTER_WLAN,
  ICBID_MASTER_MSS_CE,
  ICBID_MASTER_A1NOC_CFG,
  ICBID_MASTER_A2NOC_CFG,
  ICBID_MASTER_GNOC_CFG,
  ICBID_MASTER_GNOC_SNOC,
  ICBID_MASTER_CDSP_HCP_TCM,
  ICBID_MASTER_CDSP_HCP,
  ICBID_MASTER_CDSP_DMA_TCM,
  ICBID_MASTER_CDSP_DMA,
  ICBID_MASTER_AOSS,
  ICBID_MASTER_CDSP_NOC_CFG,
  ICBID_MASTER_MEM_NOC_CFG,
  ICBID_MASTER_VAPSS_NOC_CFG,
  ICBID_MASTER_SHRM,
  ICBID_MASTER_CNOC_DC_NOC,
  ICBID_MASTER_CDC_SNOC_GC,
  ICBID_MASTER_CDC_SNOC_SF,
  ICBID_MASTER_CDSP,
  ICBID_MASTER_LPASS_ANOC,
  ICBID_MASTER_MEM_NOC_GNOC,
  ICBID_MASTER_MEM_NOC_PCIE_SNOC,
  ICBID_MASTER_MNOC_SF_MEM_NOC,
  ICBID_MASTER_ANOC_PCIE_SNOC,
  ICBID_MASTER_CAMNOC_HF,
  ICBID_MASTER_CAMNOC_HF0 = ICBID_MASTER_CAMNOC_HF,
  ICBID_MASTER_CAMNOC_SF,
  ICBID_MASTER_CDSP_PROC,
  ICBID_MASTER_SP,
  ICBID_MASTER_VAPSS_CDSP_0,
  ICBID_MASTER_VAPSS_CDSP_1,
  ICBID_MASTER_VIDEO_PROC,
  ICBID_MASTER_UFS_MEM,
  ICBID_MASTER_GIC,
  ICBID_MASTER_LLCC,
  ICBID_MASTER_ANOC_IPA,
  ICBID_MASTER_IPA_PCIE,
  ICBID_MASTER_IPA_CORE,
  ICBID_MASTER_CAMNOC_HF1,
  ICBID_MASTER_CAMNOC_HF0_UNCOMP,
  ICBID_MASTER_CAMNOC_HF1_UNCOMP,
  ICBID_MASTER_CAMNOC_SF_UNCOMP,  ICBID_MASTER_LPASS_CORE,  ICBID_MASTER_Q6,  ICBID_MASTER_QUPV3,  ICBID_MASTER_SDC,  ICBID_MASTER_SNOC_LPASS,  ICBID_MASTER_EMMC,
  ICBID_MASTER_COUNT,
  ICBID_MASTER_NONE = ICBID_MASTER_COUNT,
  ICBID_MASTER_SIZE = 0x7FFFFFFF /**< Force to 32 bits */
} ICBId_MasterType;

/**
 * Slave Identifiers
 */ 
typedef enum {
  ICBID_SLAVE_EBI1 = 0,
  ICBID_SLAVE_APPSS_L2,
  ICBID_SLAVE_BIMC_SNOC,
  ICBID_SLAVE_BIMC_SNOC_0 = ICBID_SLAVE_BIMC_SNOC,
  ICBID_SLAVE_MEM_NOC_SNOC = ICBID_SLAVE_BIMC_SNOC,
  ICBID_SLAVE_CAMERA_CFG,
  ICBID_SLAVE_DISPLAY_CFG,
  ICBID_SLAVE_OCMEM_CFG,
  ICBID_SLAVE_CPR_CFG,
  ICBID_SLAVE_CPR_XPU_CFG,
  ICBID_SLAVE_MISC_CFG,
  ICBID_SLAVE_MISC_XPU_CFG,
  ICBID_SLAVE_VENUS_CFG,
  ICBID_SLAVE_GFX3D_CFG,
  ICBID_SLAVE_MMSS_CLK_CFG,
  ICBID_SLAVE_MMSS_CLK_XPU_CFG,
  ICBID_SLAVE_MNOC_MPU_CFG,
  ICBID_SLAVE_ONOC_MPU_CFG,
  ICBID_SLAVE_MNOC_BIMC,
  ICBID_SLAVE_MNOC_HF_MEM_NOC = ICBID_SLAVE_MNOC_BIMC,
  ICBID_SLAVE_SERVICE_MNOC,
  ICBID_SLAVE_OCMEM,
  ICBID_SLAVE_GMEM = ICBID_SLAVE_OCMEM,
  ICBID_SLAVE_SERVICE_ONOC,
  ICBID_SLAVE_APPSS,
  ICBID_SLAVE_LPASS,
  ICBID_SLAVE_USB3,
  ICBID_SLAVE_USB3_0 = ICBID_SLAVE_USB3,
  ICBID_SLAVE_WCSS,
  ICBID_SLAVE_SNOC_BIMC,
  ICBID_SLAVE_SNOC_BIMC_0 = ICBID_SLAVE_SNOC_BIMC,
  ICBID_SLAVE_SNOC_MEM_NOC_GC = ICBID_SLAVE_SNOC_BIMC,
  ICBID_SLAVE_SNOC_CNOC,
  ICBID_SLAVE_IMEM,
  ICBID_SLAVE_OCIMEM = ICBID_SLAVE_IMEM,
  ICBID_SLAVE_SNOC_OVIRT,
  ICBID_SLAVE_SNOC_GVIRT = ICBID_SLAVE_SNOC_OVIRT,
  ICBID_SLAVE_SNOC_PNOC,
  ICBID_SLAVE_SNOC_PCNOC = ICBID_SLAVE_SNOC_PNOC,
  ICBID_SLAVE_SERVICE_SNOC,
  ICBID_SLAVE_QDSS_STM,
  ICBID_SLAVE_SDCC_1,
  ICBID_SLAVE_SDCC_3,
  ICBID_SLAVE_SDCC_2,
  ICBID_SLAVE_SDCC_4,
  ICBID_SLAVE_TSIF,
  ICBID_SLAVE_BAM_DMA,
  ICBID_SLAVE_BLSP_2,
  ICBID_SLAVE_QUP_2 = ICBID_SLAVE_BLSP_2,
  ICBID_SLAVE_USB_HSIC,
  ICBID_SLAVE_BLSP_1,
  ICBID_SLAVE_QUP_1 = ICBID_SLAVE_BLSP_1,
  ICBID_SLAVE_USB_HS,
  ICBID_SLAVE_USB_HS1 = ICBID_SLAVE_USB_HS,
  ICBID_SLAVE_PDM,
  ICBID_SLAVE_PERIPH_APU_CFG,
  ICBID_SLAVE_PNOC_MPU_CFG,
  ICBID_SLAVE_PRNG,
  ICBID_SLAVE_PNOC_SNOC,
  ICBID_SLAVE_PCNOC_SNOC = ICBID_SLAVE_PNOC_SNOC,
  ICBID_SLAVE_SERVICE_PNOC,
  ICBID_SLAVE_CLK_CTL,
  ICBID_SLAVE_CNOC_MSS,
  ICBID_SLAVE_PCNOC_MSS = ICBID_SLAVE_CNOC_MSS,
  ICBID_SLAVE_SECURITY,
  ICBID_SLAVE_TCSR,
  ICBID_SLAVE_TLMM,
  ICBID_SLAVE_CRYPTO_0_CFG,
  ICBID_SLAVE_CRYPTO_1_CFG,
  ICBID_SLAVE_IMEM_CFG,
  ICBID_SLAVE_MESSAGE_RAM,
  ICBID_SLAVE_BIMC_CFG,
  ICBID_SLAVE_BOOT_ROM,
  ICBID_SLAVE_CNOC_MNOC_MMSS_CFG,
  ICBID_SLAVE_PMIC_ARB,
  ICBID_SLAVE_SPDM_WRAPPER,
  ICBID_SLAVE_DEHR_CFG,
  ICBID_SLAVE_MPM,
  ICBID_SLAVE_QDSS_CFG,
  ICBID_SLAVE_RBCPR_CFG,
  ICBID_SLAVE_RBCPR_CX_CFG = ICBID_SLAVE_RBCPR_CFG,
  ICBID_SLAVE_RBCPR_QDSS_APU_CFG,
  ICBID_SLAVE_CNOC_MNOC_CFG,
  ICBID_SLAVE_SNOC_MPU_CFG,
  ICBID_SLAVE_CNOC_ONOC_CFG,
  ICBID_SLAVE_PNOC_CFG,
  ICBID_SLAVE_SNOC_CFG,
  ICBID_SLAVE_EBI1_DLL_CFG,
  ICBID_SLAVE_PHY_APU_CFG,
  ICBID_SLAVE_EBI1_PHY_CFG,
  ICBID_SLAVE_RPM,
  ICBID_SLAVE_CNOC_SNOC,
  ICBID_SLAVE_SERVICE_CNOC,
  ICBID_SLAVE_OVIRT_SNOC,
  ICBID_SLAVE_OVIRT_OCMEM,
  ICBID_SLAVE_USB_HS2,
  ICBID_SLAVE_QPIC,
  ICBID_SLAVE_IPS_CFG,
  ICBID_SLAVE_DSI_CFG,
  ICBID_SLAVE_USB3_1,
  ICBID_SLAVE_PCIE_0,
  ICBID_SLAVE_PCIE_1,
  ICBID_SLAVE_PSS_SMMU_CFG,
  ICBID_SLAVE_CRYPTO_2_CFG,
  ICBID_SLAVE_PCIE_0_CFG,
  ICBID_SLAVE_PCIE_1_CFG,
  ICBID_SLAVE_SATA_CFG,
  ICBID_SLAVE_SPSS_GENI_IR,
  ICBID_SLAVE_UFS_CFG,
  ICBID_SLAVE_UFS_CARD_CFG = ICBID_SLAVE_UFS_CFG,
  ICBID_SLAVE_AVSYNC_CFG,
  ICBID_SLAVE_VPU_CFG,
  ICBID_SLAVE_USB_PHY_CFG,
  ICBID_SLAVE_RBCPR_MX_CFG,
  ICBID_SLAVE_PCIE_PARF,
  ICBID_SLAVE_VCAP_CFG,
  ICBID_SLAVE_EMAC_CFG,
  ICBID_SLAVE_BCAST_CFG,
  ICBID_SLAVE_KLM_CFG,
  ICBID_SLAVE_DISPLAY_PWM,
  ICBID_SLAVE_GENI,
  ICBID_SLAVE_SNOC_BIMC_1,
  ICBID_SLAVE_SNOC_MEM_NOC_SF = ICBID_SLAVE_SNOC_BIMC_1,
  ICBID_SLAVE_AUDIO,
  ICBID_SLAVE_CATS_0,
  ICBID_SLAVE_CATS_1,
  ICBID_SLAVE_MM_INT_0,
  ICBID_SLAVE_MM_INT_1,
  ICBID_SLAVE_MM_INT_2,
  ICBID_SLAVE_MM_INT_BIMC,
  ICBID_SLAVE_MMU_MODEM_XPU_CFG,
  ICBID_SLAVE_MSS_INT,
  ICBID_SLAVE_PCNOC_INT_0,
  ICBID_SLAVE_PCNOC_INT_1,
  ICBID_SLAVE_PCNOC_M_0,
  ICBID_SLAVE_PCNOC_M_1,
  ICBID_SLAVE_PCNOC_S_0,
  ICBID_SLAVE_PCNOC_S_1,
  ICBID_SLAVE_PCNOC_S_2,
  ICBID_SLAVE_PCNOC_S_3,
  ICBID_SLAVE_PCNOC_S_4,
  ICBID_SLAVE_PCNOC_S_6,
  ICBID_SLAVE_PCNOC_S_7,
  ICBID_SLAVE_PCNOC_S_8,
  ICBID_SLAVE_PCNOC_S_9,
  ICBID_SLAVE_PRNG_XPU_CFG,
  ICBID_SLAVE_QDSS_INT,
  ICBID_SLAVE_RPM_XPU_CFG,
  ICBID_SLAVE_SNOC_INT_0,
  ICBID_SLAVE_SNOC_INT_1,
  ICBID_SLAVE_SNOC_INT_BIMC,
  ICBID_SLAVE_TCU,
  ICBID_SLAVE_BIMC_INT_0,
  ICBID_SLAVE_BIMC_INT_1,
  ICBID_SLAVE_RICA_CFG,
  ICBID_SLAVE_SNOC_BIMC_2,
  ICBID_SLAVE_BIMC_SNOC_1,
  ICBID_SLAVE_PNOC_A1NOC,
  ICBID_SLAVE_SNOC_VMEM,
  ICBID_SLAVE_A0NOC_SNOC,
  ICBID_SLAVE_ANOC_SNOC = ICBID_SLAVE_A0NOC_SNOC,
  ICBID_SLAVE_A1NOC_SNOC,
  ICBID_SLAVE_A2NOC_SNOC,
  ICBID_SLAVE_A0NOC_CFG,
  ICBID_SLAVE_ANOC_MPU_CFG = ICBID_SLAVE_A0NOC_CFG,
  ICBID_SLAVE_A0NOC_MPU_CFG,
  ICBID_SLAVE_A0NOC_SMMU_CFG,
  ICBID_SLAVE_A1NOC_CFG,
  ICBID_SLAVE_A1NOC_MPU_CFG,
  ICBID_SLAVE_A1NOC_SMMU_CFG,
  ICBID_SLAVE_A2NOC_CFG,
  ICBID_SLAVE_A2NOC_MPU_CFG,
  ICBID_SLAVE_A2NOC_SMMU_CFG,
  ICBID_SLAVE_AHB2PHY,
  ICBID_SLAVE_CAMERA_THROTTLE_CFG,
  ICBID_SLAVE_DCC_CFG,
  ICBID_SLAVE_DISPLAY_THROTTLE_CFG,
  ICBID_SLAVE_DSA_CFG,
  ICBID_SLAVE_DSA_MPU_CFG,
  ICBID_SLAVE_SSC_MPU_CFG,
  ICBID_SLAVE_HMSS_L3,
  ICBID_SLAVE_LPASS_SMMU_CFG,
  ICBID_SLAVE_MMAGIC_CFG,
  ICBID_SLAVE_PCIE20_AHB2PHY,
  ICBID_SLAVE_PCIE_2,
  ICBID_SLAVE_PCIE_2_CFG,
  ICBID_SLAVE_PIMEM,
  ICBID_SLAVE_PIMEM_CFG,
  ICBID_SLAVE_QDSS_RBCPR_APU_CFG,
  ICBID_SLAVE_RBCPR_CX,
  ICBID_SLAVE_RBCPR_MX,
  ICBID_SLAVE_SMMU_CPP_CFG,
  ICBID_SLAVE_SMMU_JPEG_CFG,
  ICBID_SLAVE_SMMU_MDP_CFG,
  ICBID_SLAVE_SMMU_ROTATOR_CFG,
  ICBID_SLAVE_SMMU_VENUS_CFG,
  ICBID_SLAVE_SMMU_VFE_CFG,
  ICBID_SLAVE_SSC_CFG,
  ICBID_SLAVE_VENUS_THROTTLE_CFG,
  ICBID_SLAVE_VMEM,
  ICBID_SLAVE_VMEM_CFG,
  ICBID_SLAVE_QDSS_MPU_CFG,
  ICBID_SLAVE_USB3_PHY_CFG,
  ICBID_SLAVE_IPA_CFG,
  ICBID_SLAVE_PCNOC_INT_2,
  ICBID_SLAVE_PCNOC_INT_3,
  ICBID_SLAVE_PCNOC_INT_4,
  ICBID_SLAVE_PCNOC_INT_5,
  ICBID_SLAVE_PCNOC_INT_6,
  ICBID_SLAVE_PCNOC_S_5,
  ICBID_SLAVE_QSPI,
  ICBID_SLAVE_A1NOC_MS_MPU_CFG,
  ICBID_SLAVE_A2NOC_MS_MPU_CFG,
  ICBID_SLAVE_MODEM_Q6_SMMU_CFG,
  ICBID_SLAVE_MSS_MPU_CFG,
  ICBID_SLAVE_MSS_PROC_MS_MPU_CFG,
  ICBID_SLAVE_SKL,
  ICBID_SLAVE_SNOC_INT_2,
  ICBID_SLAVE_SMMNOC_BIMC,
  ICBID_SLAVE_CRVIRT_A1NOC,
  ICBID_SLAVE_SGMII,
  ICBID_SLAVE_QHS4_APPS,
  ICBID_SLAVE_BIMC_PCNOC,
  ICBID_SLAVE_PCNOC_BIMC_1,
  ICBID_SLAVE_SPMI_FETCHER,
  ICBID_SLAVE_MMSS_SMMU_CFG,
  ICBID_SLAVE_WLAN,
  ICBID_SLAVE_CRVIRT_A2NOC,
  ICBID_SLAVE_CNOC_A2NOC,
  ICBID_SLAVE_GLM,
  ICBID_SLAVE_GNOC_BIMC,
  ICBID_SLAVE_GNOC_MEM_NOC = ICBID_SLAVE_GNOC_BIMC,
  ICBID_SLAVE_GNOC_SNOC,
  ICBID_SLAVE_QM_CFG,
  ICBID_SLAVE_TLMM_EAST,
  ICBID_SLAVE_TLMM_NORTH,
  ICBID_SLAVE_TLMM_WEST,
  ICBID_SLAVE_TLMM_SOUTH,
  ICBID_SLAVE_SPSS_CFG,
  ICBID_SLAVE_AOP,
  ICBID_SLAVE_AOSS,
  ICBID_SLAVE_CDSP_CFG,
  ICBID_SLAVE_CNOC_DDRSS,
  ICBID_SLAVE_DDRSS_CFG,
  ICBID_SLAVE_DDRSS_XPU_CFG,
  ICBID_SLAVE_LLCC_CFG,
  ICBID_SLAVE_MC_CFG,
  ICBID_SLAVE_MCCC,
  ICBID_SLAVE_MCCC_MASTER,
  ICBID_SLAVE_MEM_NOC_CFG,
  ICBID_SLAVE_SOUTH_PHY_CFG,
  ICBID_SLAVE_QM_MPU_CFG,
  ICBID_SLAVE_QMIP_0_CFG,
  ICBID_SLAVE_QMIP_1_CFG,
  ICBID_SLAVE_QMIP_2_CFG,
  ICBID_SLAVE_SHRM_CFG,
  ICBID_SLAVE_SHRM_MEM,
  ICBID_SLAVE_UFS_MEM_CFG,
  ICBID_SLAVE_VSENSE_CTRL_CFG,
  ICBID_SLAVE_MNOC_SF_MEM_NOC,
  ICBID_SLAVE_MEM_NOC_GNOC,
  ICBID_SLAVE_CDC_SNOC_GC,
  ICBID_SLAVE_CDC_SNOC_SF,
  ICBID_SLAVE_CDC_TCU,
  ICBID_SLAVE_LLCC,
  ICBID_SLAVE_LPASS_SNOC,
  ICBID_SLAVE_MEM_NOC_PCIE_SNOC,
  ICBID_SLAVE_ANOC_PCIE_SNOC,
  ICBID_SLAVE_CDSP_MEM_NOC,
  ICBID_SLAVE_BOOT_IMEM,
  ICBID_SLAVE_CDSP_TCM,
  ICBID_SLAVE_CDSP_HCP_VAPSS,
  ICBID_SLAVE_CDSP_DMA_VAPSS,
  ICBID_SLAVE_SERVICE_A1NOC,
  ICBID_SLAVE_SERVICE_A2NOC,
  ICBID_SLAVE_SERVICE_CDSP_NOC,
  ICBID_SLAVE_SERVICE_VAPSS_NOC,
  ICBID_SLAVE_SERVICE_DC_NOC,
  ICBID_SLAVE_SERVICE_GNOC,
  ICBID_SLAVE_SERVICE_MEM_NOC,
  ICBID_SLAVE_ANOC_IPA,
  ICBID_SLAVE_ECC_CFG,
  ICBID_SLAVE_MCDMA_MS_MPU_CFG,
  ICBID_SLAVE_PCNOC_QHS6,
  ICBID_SLAVE_SNOC_QHS0,
  ICBID_SLAVE_IPA_CORE,
  ICBID_SLAVE_CAMNOC_UNCOMP,
  ICBID_SLAVE_ANOC_PCIE_A1NOC_SNOC,  ICBID_SLAVE_CCD,  ICBID_SLAVE_LPASS_CORE,  ICBID_SLAVE_MCC,  ICBID_SLAVE_PRAM,  ICBID_SLAVE_Q6,  ICBID_SLAVE_Q6_MPU_CFG,  ICBID_SLAVE_QTIMER,  ICBID_SLAVE_QUPV3,  ICBID_SLAVE_RSCC,  ICBID_SLAVE_SDC,  ICBID_SLAVE_SDC_MPU_CFG,  ICBID_SLAVE_VOLTAGE_SENSOR_CX,  ICBID_SLAVE_VOLTAGE_SENSOR_MX,  ICBID_SLAVE_LPASS_ANOC,  ICBID_SLAVE_SERVIC_SSC_NOC,  ICBID_SLAVE_SERVIC_SSC_LPASS_CORE,  ICBID_SLAVE_DEBUG,
  ICBID_SLAVE_COUNT,
  ICBID_SLAVE_NONE = ICBID_SLAVE_COUNT,
  ICBID_SLAVE_SIZE = 0x7FFFFFFF /**< Force to 32 bits */
} ICBId_SlaveType;

#endif /* __ICBID_H__ */
