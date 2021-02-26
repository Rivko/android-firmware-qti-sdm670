/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: adsppm_configdata_670.c
@brief: Contains all 670-specific data definitions for Config Manager.

$Header: //components/rel/core.qdsp6/2.1.c4/settings/power/qdsp_pm/config/670/adsp/qdsp_pm_configdata_670_adsp.c#2 $
*/

#include "adsppm_settings.h"
#include "asic_settings.h"
#include "asic_internal_settings.h"
#include "adsppm_utils_settings.h"
#include "icbid.h"
#include "DDIIPCInt.h"
#include "qdsp_pm_clocks_hwio_670_adsp.h"
#include "msmhwiobase.h"
#include "limits.h"
#include "systemcache/systemcache.h"
#include "qdsp_pm_configdata_arch_670_adsp.h"

AsicHwioRegRangeType lpassRegRange_670 = {LPASS_BASE, LPASS_BASE_PHYS, LPASS_BASE_SIZE};

// L2 configuration registers base address = TCM_BASE + 1.5 Meg + 128K
// Reference: Fig 8-3 in document 80-V9418-22 Rev C
#define TCM_BASE 0x17400000
#define L2_CONFIG_BASE (TCM_BASE + 0x00180000 + 0x00020000)
AsicHwioRegRangeType l2ConfigRegRange_670 = {NULL, NULL, 0};

/**
 * Array of 670 Core Clocks per core.
 */
const AdsppmClkIdType coreClocks_SlimBus_670[] = {AdsppmClk_Slimbus_Core, AdsppmClk_Slimbus2_Core};
const AdsppmClkIdType coreClocks_HwRsp_670[]   = {AdsppmClk_HwRsp_Core};
const AdsppmClkIdType coreClocks_Avsync_670[]   = {AdsppmClk_Avtimer_core};
const AdsppmClkIdType coreClocks_SLPI_SDC_670[] = {AdsppmClk_Sdc_Core};
const AdsppmClkIdType coreClocks_SLPI_QUP_670[] = {AdsppmClk_Qup_Core};
const AdsppmClkIdType coreClocks_SLPI_SRAM_670[] = {AdsppmClk_Sram_Core};
const AdsppmClkIdType coreClocks_SLPI_CCD_670[] = {AdsppmClk_Ccd_Core};

/**
 * Array of 670 Master Bus Ports per core.
 */
const AdsppmBusPortIdType masterPorts_ADSP_670[]    = {AdsppmBusPort_Adsp_Master};
const AdsppmBusPortIdType masterPorts_DML_670[]     = {AdsppmBusPort_Dml_Master};
const AdsppmBusPortIdType masterPorts_AIF_670[]     = {AdsppmBusPort_Aif_Master};
const AdsppmBusPortIdType masterPorts_SlimBus_670[] = {AdsppmBusPort_Slimbus_Master, AdsppmBusPort_Slimbus2_Master};
const AdsppmBusPortIdType masterPorts_HWRSMP_670[]  = {AdsppmBusPort_HwRsmp_Master};
const AdsppmBusPortIdType masterPorts_Hdmitx_670[]  = {AdsppmBusPort_Hdmitx_Master};
const AdsppmBusPortIdType masterPorts_DPAudio_670[] = {AdsppmBusPort_DPAudio_Master};
const AdsppmBusPortIdType masterPorts_SLPI_SDC_670[] = {AdsppmBusPort_Sdc_Master};
const AdsppmBusPortIdType masterPorts_SLPI_QUP_670[] = {AdsppmBusPort_Qup_Master};

/**
 * Array of 670 Slave Bus Ports per core.
 */
const AdsppmBusPortIdType slavePorts_ADSP_670[]    = {AdsppmBusPort_Adsp_Slave};
const AdsppmBusPortIdType slavePorts_LPM_670[]     = {AdsppmBusPort_Lpm_Slave};
const AdsppmBusPortIdType slavePorts_DML_670[]     = {AdsppmBusPort_Dml_Slave};
const AdsppmBusPortIdType slavePorts_AIF_670[]     = {AdsppmBusPort_Aif_Slave};
const AdsppmBusPortIdType slavePorts_SlimBus_670[] = {AdsppmBusPort_Slimbus_Slave, AdsppmBusPort_Slimbus2_Slave};
const AdsppmBusPortIdType slavePorts_AVsync_670[]  = {AdsppmBusPort_AvSync_Slave};
const AdsppmBusPortIdType slavePorts_HWRSMP_670[]  = {AdsppmBusPort_HwRsmp_Slave};
const AdsppmBusPortIdType slavePorts_Hdmitx_670[]  = {AdsppmBusPort_Hdmitx_Slave};
const AdsppmBusPortIdType slavePorts_DPAudio_670[] = {AdsppmBusPort_DPAudio_Slave};
const AdsppmBusPortIdType slavePorts_SLPI_SRAM_670[]    = {AdsppmBusPort_Slpi_Sram_Slave};

/**
 * Array of 670 core descriptors.
 */
const AsicCoreDescType cores_array_670[] =
{
        {   //Adsppm_Core_Id_ADSP
                Adsppm_Core_Id_ADSP, //Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_Power_ADSP, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_Latency, //Sleep Latency
                        AsicRsc_MIPS_Clk | AsicRsc_MIPS_BW, //MIPS
                        AsicRsc_BW_Internal | AsicRsc_BW_External, //BW
                        AsicRsc_Thermal //Thermal
                }, //HwResource IDs
                AsicPowerDomain_Adsp, //Power Domain
                {0, NULL}, //Core clock instances
                {ADSPPM_ARRAY(masterPorts_ADSP_670)}, //Master port instances
                {ADSPPM_ARRAY(slavePorts_ADSP_670)}, //Slave port instances
                Adsppm_Instance_Id_0 //Number of core instances
        },
        {   //Adsppm_Core_Id_LPASS_Core
                Adsppm_Core_Id_LPASS_Core, //Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_None, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_None, //Sleep Latency
                        AsicRsc_None, //MIPS
                        AsicRsc_None, //BW
                        AsicRsc_None //Thermal
                }, //HwResource IDs
                AsicPowerDomain_LpassCore, //Power Domain
                {0, NULL}, //Core clock instances
                {0, NULL}, //Master port instances
                {0, NULL}, //Slave port instances
                Adsppm_Instance_Id_0 //Number of core instances
        },
        {   //Adsppm_Core_Id_LPM
                Adsppm_Core_Id_LPM, //Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_Power_Mem, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_Latency, //Sleep Latency
                        AsicRsc_MIPS_Clk | AsicRsc_MIPS_BW, //MIPS
                        AsicRsc_BW_Internal | AsicRsc_BW_External, //BW
                        AsicRsc_Thermal //Thermal
                }, //HwResource IDs
                AsicPowerDomain_LpassCore, //Power Domain
                {0, NULL}, //Core clock instances
                {0, NULL}, //Master port instances
                {ADSPPM_ARRAY(slavePorts_LPM_670)}, //Slave port instances
                Adsppm_Instance_Id_0 //Number of core instances
        },
        {   //Adsppm_Core_Id_DML
                Adsppm_Core_Id_DML, //Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_Power_Core, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_Latency, //Sleep Latency
                        AsicRsc_MIPS_Clk | AsicRsc_MIPS_BW, //MIPS
                        AsicRsc_BW_Internal | AsicRsc_BW_External, //BW
                        AsicRsc_Thermal //Thermal
                }, //HwResource IDs
                AsicPowerDomain_LpassCore, //Power Domain
                {0, NULL}, //Core clock instances
                {ADSPPM_ARRAY(masterPorts_DML_670)}, //Master port instances
                {ADSPPM_ARRAY(slavePorts_DML_670)}, //Slave port instances
                Adsppm_Instance_Id_0 //Number of core instances
        },
        {   //Adsppm_Core_Id_AIF
                Adsppm_Core_Id_AIF, //Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_Power_Core, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_Latency, //Sleep Latency
                        AsicRsc_MIPS_Clk | AsicRsc_MIPS_BW, //MIPS
                        AsicRsc_BW_Internal | AsicRsc_BW_External, //BW
                        AsicRsc_Thermal //Thermal
                }, //HwResource IDs
                AsicPowerDomain_LpassCore, //Power Domain
                {0, NULL}, //Core clock instances
                {ADSPPM_ARRAY(masterPorts_AIF_670)}, //Master port instances
                {ADSPPM_ARRAY(slavePorts_AIF_670)}, //Slave port instances
                Adsppm_Instance_Id_0 //Number of core instances
        },
        {   //Adsppm_Core_Id_SlimBus
                Adsppm_Core_Id_SlimBus, //Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_Power_Core, //Power
                        AsicRsc_Core_Clk, //Core Clock
                        AsicRsc_Latency, //Sleep Latency
                        AsicRsc_MIPS_Clk | AsicRsc_MIPS_BW, //MIPS
                        AsicRsc_BW_Internal | AsicRsc_BW_External, //BW
                        AsicRsc_Thermal //Thermal
                }, //HwResource IDs
                AsicPowerDomain_LpassCore, //Power Domain
                {ADSPPM_ARRAY(coreClocks_SlimBus_670)}, //Core clock instances
                {ADSPPM_ARRAY(masterPorts_SlimBus_670)}, //Master port instances
                {ADSPPM_ARRAY(slavePorts_SlimBus_670)}, //Slave port instances
                Adsppm_Instance_Id_1 //Number of core instances
        },
        {   //Adsppm_Core_Id_AVsync
                Adsppm_Core_Id_AVsync, //Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_Power_Core, //Power
                        AsicRsc_Core_Clk, //Core Clock
                        AsicRsc_Latency, //Sleep Latency
                        AsicRsc_MIPS_Clk | AsicRsc_MIPS_BW, //MIPS
                        AsicRsc_BW_Internal | AsicRsc_BW_External, //BW
                        AsicRsc_Thermal //Thermal
                }, //HwResource IDs
                AsicPowerDomain_LpassCore, //Power Domain
                {ADSPPM_ARRAY(coreClocks_Avsync_670)}, //Core clock instances
                {0, NULL}, //Master port instances
                {ADSPPM_ARRAY(slavePorts_AVsync_670)}, //Slave port instances
                Adsppm_Instance_Id_0 //Number of core instances
        },
        {   //Adsppm_Core_Id_HWRSMP
                Adsppm_Core_Id_HWRSMP, //Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_Power_Core, //Power
                        AsicRsc_Core_Clk, //Core Clock
                        AsicRsc_Latency, //Sleep Latency
                        AsicRsc_MIPS_Clk | AsicRsc_MIPS_BW, //MIPS
                        AsicRsc_BW_Internal | AsicRsc_BW_External, //BW
                        AsicRsc_Thermal //Thermal
                }, //HwResource IDs
                AsicPowerDomain_LpassCore, //Power Domain
                {ADSPPM_ARRAY(coreClocks_HwRsp_670)}, //Core clock instances
                {ADSPPM_ARRAY(masterPorts_HWRSMP_670)}, //Master port instances
                {ADSPPM_ARRAY(slavePorts_HWRSMP_670)}, //Slave port instances
                Adsppm_Instance_Id_0 //Number of core instances
        },
        {   //Adsppm_Core_Id_Hdmitx
                Adsppm_Core_Id_Hdmitx, //Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_Power_Core, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_Latency, //Sleep Latency
                        AsicRsc_MIPS_Clk | AsicRsc_MIPS_BW, //MIPS
                        AsicRsc_BW_Internal | AsicRsc_BW_External, //BW
                        AsicRsc_Thermal //Thermal
                }, //HwResource IDs
                AsicPowerDomain_LpassCore, //Power Domain
                {0, NULL}, //Core clock instances
                {ADSPPM_ARRAY(masterPorts_Hdmitx_670)}, //Master port instances
                {ADSPPM_ARRAY(slavePorts_Hdmitx_670)}, //Slave port instances
                Adsppm_Instance_Id_0 //Number of core instances
        },
        {   //Adsppm_Core_Id_DPAudio
                Adsppm_Core_Id_DPAudio, //Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_Power_Core, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_Latency, //Sleep Latency
                        AsicRsc_MIPS_Clk | AsicRsc_MIPS_BW, //MIPS
                        AsicRsc_BW_Internal | AsicRsc_BW_External, //BW
                        AsicRsc_Thermal //Thermal
                }, //HwResource IDs
                AsicPowerDomain_LpassCore, //Power Domain
                {0, NULL}, //Core clock instances
                {ADSPPM_ARRAY(masterPorts_DPAudio_670)}, //Master port instances
                {ADSPPM_ARRAY(slavePorts_DPAudio_670)}, //Slave port instances
                Adsppm_Instance_Id_0 //Number of core instances
        },
		{   //Adsppm_Core_Id_Slpi_SDC
                Adsppm_Core_Id_Slpi_SDC, //Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_None, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_None, //Sleep Latency
                        AsicRsc_MIPS_Clk | AsicRsc_MIPS_BW, //MIPS
                        AsicRsc_BW_Internal | AsicRsc_BW_External, //BW
                        AsicRsc_None //Thermal
                }, //HwResource IDs
                AsicPowerDomain_AON, //Power Domain
                {ADSPPM_ARRAY(coreClocks_SLPI_SDC_670)}, //Core clock instances
                {ADSPPM_ARRAY(masterPorts_SLPI_SDC_670)}, //Master port instances
                {0, NULL}, //Slave port instances
                Adsppm_Instance_Id_0 //Number of core instances
        },
        {   //Adsppm_Core_Id_Slpi_Qup
                Adsppm_Core_Id_Slpi_QUP, //Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_None, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_None, //Sleep Latency
                        AsicRsc_MIPS_Clk | AsicRsc_MIPS_BW, //MIPS
                        AsicRsc_BW_Internal | AsicRsc_BW_External, //BW
                        AsicRsc_None //Thermal
                }, //HwResource IDs
                AsicPowerDomain_AON, //Power Domain
                {ADSPPM_ARRAY(coreClocks_SLPI_QUP_670)}, //Core clock instances
                {ADSPPM_ARRAY(masterPorts_SLPI_QUP_670)}, //Master port instances
                {0, NULL}, //Slave port instances
                Adsppm_Instance_Id_0 //Number of core instances//will be 6 if DSPPM had to manage
        },
                {
                Adsppm_Core_Id_Slpi_CCD, //Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_None, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_None, //Sleep Latency
                        AsicRsc_MIPS_Clk | AsicRsc_MIPS_BW, //MIPS
                        AsicRsc_None, //BW
                        AsicRsc_None //Thermal
                }, //HwResource IDs
                AsicPowerDomain_AON, //Power Domain
                {ADSPPM_ARRAY(coreClocks_SLPI_CCD_670)}, //Core clock instances
                {0, NULL}, //Master port instances
                {0, NULL}, //Slave port instances
                Adsppm_Instance_Id_0 //Number of core instances
        },
        {
                Adsppm_Core_Id_Slpi_SRAM, //Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_Power_Mem, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_None, //Sleep Latency
                        AsicRsc_None,
                        AsicRsc_BW_Internal, //BW
                        AsicRsc_None, //Thermal
                }, //HwResource IDs
                AsicPowerDomain_AON, //Power Domain
                {ADSPPM_ARRAY(coreClocks_SLPI_SRAM_670)}, //Core clock instances
                {0, NULL}, //Master port instances
                {ADSPPM_ARRAY(slavePorts_SLPI_SRAM_670)}, //Slave port instances
                Adsppm_Instance_Id_0 //Number of core instances
        },
};

AsicCoreDescriptorArrayType cores_670 = {ADSPPM_ARRAY(cores_array_670)};

const AsicMemDescriptorType memories_array_670[] = //TODOs: As the feature is disabled: can be removed
{
        {   //Adsppm_Mem_Lpass_LPM
                Adsppm_Mem_Lpass_LPM, //Mem ID
                AsicPowerDomain_Lpm //Power Domain
        },
        {   //Adsppm_Mem_Sram
                Adsppm_Mem_Sram, //Mem ID
                AsicPowerDomain_SRam //Power Domain
        }
};

AsicMemDescriptorArrayType memories_670 = {ADSPPM_ARRAY(memories_array_670)};

/**
 * Array of 670 core clock descriptors. Arranged by core clock ID
 */
const AsicClkDescriptorType clocks_array_670[AdsppmClk_EnumMax] =
{
        {
                AdsppmClk_Adsp_Core, //Clk ID
                AsicClk_TypeNpa, //Clk Type
                AsicClk_CntlSW, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "/clk/cpu", //Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Ahb_Root, //Clk ID
                AsicClk_TypeDalFreqSet, //Clk Type
                AsicClk_CntlAlwaysON, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "lpass_audio_core_core_clk", //Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_AhbI_Hclk, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlAlwaysON, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "lpass_audio_core_core_clk", //Name
                AdsppmClk_Ahb_Root, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Slimbus_Hclk, //Clk ID
                AsicClk_TypeInternalCGC, //Clk Type
                AsicClk_CntlSW_DCG, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.hwioInfo =
                {
                        HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CGCR_OFFS, //CGCR Offset
                        HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CGCR_CLK_ENABLE_BMSK, //Enable mask
                        HWIO_LPASS_AUDIO_CORE_AUD_SLIMBUS_CORE_CGCR_HW_CTL_BMSK, //HW control mask
                        0 //Status mask
                },
                AdsppmClk_Slimbus_cbc, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Slimbus_cbc, //Clk ID
                AsicClk_TypeCBC, //Clk Type
                AsicClk_CntlSW, //Cntl Type
                AsicClk_MemCntlAlwaysRetain, //MemCtrl Type
                .clkInfo.clkName = "lpass_audio_core_aud_slimbus_core_clk", //Name
                AdsppmClk_Ahb_Root, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Slimbus2_Hclk, //Clk ID
                AsicClk_TypeInternalCGC, //Clk Type
                AsicClk_CntlSW_DCG, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.hwioInfo =
                {
                        HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CGCR_OFFS, //CGCR Offset
                        HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CGCR_CLK_ENABLE_BMSK, //Enable mask
                        HWIO_LPASS_AUDIO_CORE_QCA_SLIMBUS_CORE_CGCR_HW_CTL_BMSK, //HW control mask
                        0 //Status mask
                },
                AdsppmClk_Slimbus2_cbc, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Slimbus2_cbc, //Clk ID
                AsicClk_TypeCBC, //Clk Type
                AsicClk_CntlSW, //Cntl Type
                AsicClk_MemCntlAlwaysRetain, //MemCtrl Type
                .clkInfo.clkName = "lpass_audio_core_qca_slimbus_core_clk", //Name
                AdsppmClk_Ahb_Root, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Lpm_cbc, //Clk ID
                AsicClk_TypeCBC, //Clk Type
                AsicClk_CntlSW, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "lpass_audio_core_lpm_core_clk", //Name
                AdsppmClk_Ahb_Root, //Source
                Adsppm_Mem_Lpass_LPM //Memory ID
        },
        {
                AdsppmClk_Smmu_Adsp_Hclk, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "", //Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
		{/* gcc clock for AHB2AXI bridge */ //TODOs : Make this SW controlled
                AdsppmClk_Sysnoc_Hclk, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlSW, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "gcc_lpass_core_axim_clk", //Name
                AdsppmClk_Ahb_Root, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Sysnoc_cbc, //Clk ID
                AsicClk_TypeCBC, //Clk Type
                AsicClk_CntlSW, //Cntl Type	
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "lpass_audio_core_sysnoc_mport_core_clk", //Name
                AdsppmClk_Ahb_Root, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_AhbE_Hclk, //Clk ID
                AsicClk_TypeDalFreqSet, //Clk Type
                AsicClk_CntlAlwaysON, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "scc_noc_bus_clk", //Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Adsp_Hmclk, //Clk ID  //TODOs : Need to make SW cntrl
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlAlwaysON, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "scc_q6_ahbm_clk", //Name
                AdsppmClk_AhbE_Hclk, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Adsp_Hsclk, //Clk ID //TODOs : Need to make SW cntrl
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlAlwaysON, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "scc_q6_ahbs_clk", //Name
                AdsppmClk_AhbE_Hclk, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Sram_Hclk, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlSW, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "scc_smem_clk", //Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
		{
                AdsppmClk_Security_Hclk, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlAlwaysON, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "lpass_audio_core_sysnoc_sway_core_clk", //Name
                AdsppmClk_AhbE_Hclk, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Wrapper_Br_Hclk, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlAlwaysON, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "scc_q6_xpu2_client_clk", //Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_HwRsp_Core, //Clk ID
                AsicClk_TypeDalFreqSet, //Clk Type
                AsicClk_CntlSW, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "lpass_audio_core_resampler_clk", //Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Slimbus_Core, //Clk ID
                AsicClk_TypeDalFreqSet, //Clk Type
                AsicClk_CntlSW, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "lpass_audio_core_aud_slimbus_clk", //Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Slimbus2_Core, //Clk ID
                AsicClk_TypeDalFreqSet, //Clk Type
                AsicClk_CntlSW, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "lpass_audio_core_qca_slimbus_clk", //Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Avtimer_core, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlSW, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "lpass_audio_core_avsync_stc_clk", //Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Atime_core, //Clk ID
                AsicClk_TypeDalFreqSet, //Clk Type
                AsicClk_CntlSW, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "lpass_audio_core_avsync_atime_clk", //Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
		{
                AdsppmClk_Sdc_Core, //Clk ID
                AsicClk_TypeDalFreqSet, //Clk Type
                AsicClk_CntlSW, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "scc_sdc_proc_fclk_clk", //Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Qup_Core, //Clk ID
                AsicClk_TypeDalFreqSet, //Clk Type
                AsicClk_CntlSW, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "scc_qupv3_2xcore_clk", //Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Sram_Core, //Clk ID
                AsicClk_TypeDalFreqSet, //Clk Type
                AsicClk_CntlSW, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "scc_smem_clk", //Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Ccd_Core, //Clk ID
                AsicClk_TypeDalFreqSet, //Clk Type
                AsicClk_CntlSW, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "scc_ccd_clk", //Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Sdc_Hclk, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlSW, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "scc_sdc_proc_hclk_clk", //Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Qup_Hclk, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlSW, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "scc_qupv3_m_hclk_clk", //Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
};

AsicClockDescriptorArrayType clocks_670 = {ADSPPM_ARRAY(clocks_array_670)};

/**
 * Array of 670 Bus port descriptors arranged by Bus port ID
 */
const AsicBusPortDescriptorType busPorts_array_670[] =
{
        {/* Don't remove the AdsppmBusPort_None, as used as accessport busMgrAggregateRequest()*/
                AdsppmBusPort_None, //ID
                0, //Connection
                AdsppmClk_None, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbMaster = ICBID_MASTER_APPSS_PROC}, //icbarb ID
                AdsppmBusPort_None //Access port
        },
        {
                AdsppmBusPort_Adsp_Master, //ID
                AsicBusPort_AhbE_M | AsicBusPort_Ext_M,  //Connection
                AdsppmClk_Adsp_Hmclk, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbMaster = ICBID_MASTER_LPASS_PROC}, //icbarb ID
                AdsppmBusPort_Adsp_Master //Access port
        },
		//Node for split voting
        {
                AdsppmBusPort_Snoc_Master, //ID
                AsicBusPort_Ext_M,  //Connection
                AdsppmClk_None, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbMaster = ICBID_MASTER_SNOC_SF_MEM_NOC}, //icbarb ID
                AdsppmBusPort_Adsp_Master //Access port
        },
        //Node for split voting
        {
                AdsppmBusPort_Snoc_Ahb_Master, //ID
                AsicBusPort_Ext_M,  //Connection
                AdsppmClk_None, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbMaster = ICBID_MASTER_SNOC_SF_MEM_NOC}, //icbarb ID
                AdsppmBusPort_Ext_Ahb_Master //Access port
        },
        {
                AdsppmBusPort_Dml_Master, //ID
                AsicBusPort_AhbI_M, //Connection
                AdsppmClk_Ahb_Root, //AdsppmClk_Dml_Hclk, //Bus clock TODOs
                {0, NULL}, //Array of reg prog clocks
                {.icbarbMaster = ICBID_MASTER_LPASS_AHB}, //icbarb ID
                AdsppmBusPort_Ext_Ahb_Master //Access port
        },
        {
                AdsppmBusPort_Aif_Master, //ID
                AsicBusPort_AhbI_M, //Connection
                AdsppmClk_Ahb_Root, //AdsppmClk_Aif_Hclk, //Bus clock //TODOs
                {0, NULL}, //Array of reg prog clocks
                {.icbarbMaster = ICBID_MASTER_LPASS_AHB}, //icbarb ID
                AdsppmBusPort_Ext_Ahb_Master //Access port
        },
        {
                AdsppmBusPort_Slimbus_Master, //ID
                AsicBusPort_AhbI_M, //Connection
                AdsppmClk_Slimbus_Hclk, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbMaster = ICBID_MASTER_LPASS_AHB}, //icbarb ID
                AdsppmBusPort_Ext_Ahb_Master //Access port
        },
        {
                AdsppmBusPort_Slimbus2_Master, //ID
                AsicBusPort_AhbI_M, //Connection
                AdsppmClk_Slimbus2_Hclk, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbMaster = ICBID_MASTER_LPASS_AHB}, //icbarb ID
                AdsppmBusPort_Ext_Ahb_Master //Access port
        },
        {
                AdsppmBusPort_HwRsmp_Master, //ID
                AsicBusPort_AhbI_M, //Connection
                AdsppmClk_Ahb_Root,//AdsppmClk_HwRsp_Hclk, //Bus clock TODOs
                {0, NULL}, //Array of reg prog clocks
                {.icbarbMaster = ICBID_MASTER_LPASS_AHB}, //icbarb ID
                AdsppmBusPort_Ext_Ahb_Master //Access port
        },
        {
                AdsppmBusPort_Ext_Ahb_Master, //ID
                AsicBusPort_Ext_M, //Connection
                AdsppmClk_Sysnoc_cbc, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbMaster = ICBID_MASTER_LPASS_CORE}, //icbarb ID
                AdsppmBusPort_Ext_Ahb_Master //Access port
        },
        {
                AdsppmBusPort_Hdmitx_Master, //ID
                AsicBusPort_AhbI_M, //Connection
                AdsppmClk_Ahb_Root, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbMaster = ICBID_MASTER_LPASS_AHB}, //icbarb ID
                AdsppmBusPort_Ext_Ahb_Master //Access port
        },
        {
                AdsppmBusPort_DPAudio_Master, //ID
                AsicBusPort_AhbI_M, //Connection
                AdsppmClk_Ahb_Root, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbMaster = ICBID_MASTER_LPASS_AHB}, //icbarb ID
                AdsppmBusPort_Ext_Ahb_Master //Access port
        },
		{
                AdsppmBusPort_Sdc_Master, //ID
                AsicBusPort_AhbE_M,  //Connection
                AdsppmClk_Sdc_Hclk, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbMaster = ICBID_MASTER_SENSORS_AHB}, //icbarb ID
                AdsppmBusPort_Ext_Ahb_Master //Access port
        },
        {
                AdsppmBusPort_Qup_Master, //ID
                AsicBusPort_AhbE_M,  //Connection
                AdsppmClk_Qup_Hclk, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbMaster = ICBID_MASTER_SENSORS_AHB}, //icbarb ID
                AdsppmBusPort_Ext_Ahb_Master //Access port
        },
        {
                AdsppmBusPort_Dml_Slave, //ID
                AsicBusPort_AhbI_S, //Connection
                AdsppmClk_Ahb_Root, //AdsppmClk_Dml_Hclk, //Bus clock TODOs
                {0, NULL}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_LPASS}, //icbarb ID
                AdsppmBusPort_Ext_Ahb_Slave //Access port
        },
        {
                AdsppmBusPort_Aif_Slave, //ID
                AsicBusPort_AhbI_S, //Connection
                AdsppmClk_Ahb_Root, //AdsppmClk_Aif_Csr_Hclk, //Bus clock TODOs
                {0, NULL}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_LPASS}, //icbarb ID
                AdsppmBusPort_Ext_Ahb_Slave //Access port
        },
        {
                AdsppmBusPort_Slimbus_Slave, //ID
                AsicBusPort_AhbI_S, //Connection
                AdsppmClk_Slimbus_Hclk, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_LPASS}, //icbarb ID
                AdsppmBusPort_Ext_Ahb_Slave //Access port
        },
        {
                AdsppmBusPort_Slimbus2_Slave, //ID
                AsicBusPort_AhbI_S, //Connection
                AdsppmClk_Slimbus2_Hclk, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_LPASS}, //icbarb ID
                AdsppmBusPort_Ext_Ahb_Slave //Access port
        },
        {
                AdsppmBusPort_HwRsmp_Slave, //ID
                AsicBusPort_AhbI_S, //Connection
                AdsppmClk_Ahb_Root, //AdsppmClk_HwRsp_Hclk, //Bus clock TODOs
                {0, NULL}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_LPASS}, //icbarb ID
                AdsppmBusPort_Ext_Ahb_Slave //Access port
        },
        {
                AdsppmBusPort_AvSync_Slave, //ID
                AsicBusPort_AhbI_S, //Connection
                AdsppmClk_Ahb_Root, //AdsppmClk_AvSync_Hclk, //Bus clock TODOs
                {0, NULL}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_LPASS}, //icbarb ID
                AdsppmBusPort_Ext_Ahb_Slave //Access port
        },
        {
                AdsppmBusPort_Lpm_Slave, //ID
                AsicBusPort_AhbI_S, //Connection
                AdsppmClk_Lpm_cbc, //AdsppmClk_Lpm_Hclk, //Bus clock TODOs
                {0, NULL}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_LPASS}, //icbarb ID
                AdsppmBusPort_Ext_Ahb_Slave //Access port
        },
        {
                AdsppmBusPort_Sram_Slave, //ID //TODOs toremove
                0, //Connection
                AdsppmClk_None, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_LPASS}, //icbarb ID
                AdsppmBusPort_None //Access port
        },
        {
                AdsppmBusPort_Ext_Ahb_Slave, //ID
                AsicBusPort_Ext_S, //Connection
                AdsppmClk_Security_Hclk, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_LPASS}, //icbarb ID
                AdsppmBusPort_Ext_Ahb_Slave //Access port
        },
        {
                AdsppmBusPort_Ddr_Slave, //ID
                AsicBusPort_Ext_S, //Connection
                AdsppmClk_Sysnoc_Hclk, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_SNOC_MEM_NOC_SF}, //icbarb ID
                AdsppmBusPort_Ext_Ahb_Master //Access port
        },
        {
                AdsppmBusPort_Snoc_Slave, //ID
                AsicBusPort_Ext_S, //Connection
                AdsppmClk_None, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_EBI1 }, //icbarb ID
                AdsppmBusPort_Ext_Ahb_Master //Access port
        },        
        {
                AdsppmBusPort_Hdmitx_Slave, //ID
                AsicBusPort_AhbI_S, //Connection
                AdsppmClk_Ahb_Root,// AdsppmClk_Hdmitx_Hclk, //Bus clock TODOs
                {0, NULL}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_LPASS}, //icbarb ID
                AdsppmBusPort_Ext_Ahb_Slave //Access port
        },
        {
                AdsppmBusPort_DPAudio_Slave, //ID
                AsicBusPort_AhbI_S, //Connection
                AdsppmClk_Ahb_Root, //Bus clock //TODOs
                {0, NULL}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_LPASS}, //icbarb ID
                AdsppmBusPort_Ext_Ahb_Slave //Access port
        },
		{
                AdsppmBusPort_Slpi_Sram_Slave, //ID
                AsicBusPort_AhbE_S, //Connection
                AdsppmClk_Sram_Hclk, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbSlave =  ICBID_SLAVE_COUNT}, //icbarb ID
                AdsppmBusPort_Slpi_Sram_Slave //Access port
        },
        {
                AdsppmBusPort_Adsp_Slave, //ID
                AsicBusPort_AhbE_S, //Connection
                AdsppmClk_Adsp_Hsclk, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_LPASS}, //icbarb ID
                AdsppmBusPort_Adsp_Master //Access port
        }
		
};

AsicBusPortDescriptorArrayType busPorts_670 = {ADSPPM_ARRAY(busPorts_array_670)};

/**
 * List of supported external bus routes
 */
const AdsppmBusRouteType extBusRoutes_array_670[] =
{
        {
                AdsppmBusPort_Adsp_Master, //master
                AdsppmBusPort_Ddr_Slave //slave
        },
        {
                AdsppmBusPort_Snoc_Master, //master
                AdsppmBusPort_Snoc_Slave //slave
        },     
        {
                AdsppmBusPort_Ext_Ahb_Master, //master
                AdsppmBusPort_Ddr_Slave //slave
        },
        {
                AdsppmBusPort_Snoc_Ahb_Master, //master
                AdsppmBusPort_Snoc_Slave //slave
        },
};

AsicBusRouteArrayType extBusRoutes_670 = {ADSPPM_ARRAY(extBusRoutes_array_670)};

const AdsppmBusRouteType mipsBwRoutes_array_670[] =
{
        {
                AdsppmBusPort_Adsp_Master,
                AdsppmBusPort_Ddr_Slave
        },
        {
                AdsppmBusPort_Snoc_Master, //master
                AdsppmBusPort_Snoc_Slave //slave
        },
};

AsicBusRouteArrayType mipsBwRoutes_670 = {ADSPPM_ARRAY(mipsBwRoutes_array_670)};

/**
 * Array of power domain descriptors
 */
const AsicPowerDomainDescriptorType pwrDomains_array_670[] =
{
        {
                AsicPowerDomain_Adsp, //Id
                "/core/cpu/latency", //Name
                AsicRsc_Power_ADSP, //Type
                AdsppmClk_None, //Clock ID
                DALIPCINT_NUM_INTS, //TZ notify
                0, //TZ Done
                {0, NULL} //array of Security clocks
        },
        {
                AsicPowerDomain_LpassCore, //Id
                "audio_core_gdsc", //Name
                AsicRsc_Power_Core, //Type
                AdsppmClk_None, //Clock ID
                DALIPCINT_NUM_INTS, //TZ notify
                0, //TZ Done
                {0, NULL} //array of Security clocks
        },
};

AsicPwrDescriptorArrayType pwrDomains_670 = {ADSPPM_ARRAY(pwrDomains_array_670)};

const AsicFeatureDescType features_670 [AsicFeatureId_enum_max] =
{
        //AsicFeatureId_Adsp_Clock_Scaling //TODOs
        {
                AsicFeatureState_Enabled,
#ifdef ADSPPM_TEST
                19, //min MHz
#else
                80, //min MHz
#endif
                1190, //max MHz
        },
        //AsicFeatureId_Adsp_LowTemp_Voltage_Restriction
        {
                AsicFeatureState_Enabled,
                0, //min
                0, //max
        },
        //AsicFeatureId_Adsp_PC
        {
                AsicFeatureState_Enabled,
                0, //min
                0, //max
        },
        //AsicFeatureId_Ahb_Scaling	//TODOs
        {
                AsicFeatureState_Enabled,
                76800000, //min bytes per second corresponds to 19.2MHz clock (19.2 * 32/8)
                816000000, //max bytes per second; Update in asic_test_info.h also-MAX_AHB_FREQ //204M
        },
        //AsicFeatureId_Ahb_Sw_CG
        {
                AsicFeatureState_Enabled,
                0, //min
                0, //max
        },
        //AsicFeatureId_Ahb_DCG
        {
                AsicFeatureState_Enabled,
                0, //min
                0, //max
        },
        //AsicFeatureId_LpassCore_PC
        {
                AsicFeatureState_Enabled,
                0, //min
                0, //max
        },
        //AsicFeatureId_LpassCore_PC_TZ_Handshake
        {
                AsicFeatureState_Disabled,
                0, //min
                0, //max
        },
        //AsicFeatureId_Bus_Scaling
        {
                AsicFeatureState_Enabled,
                32768, //min bytes per second
                0x7FFFFFFF, //max bytes per second
        },
        //AsicFeatureId_CoreClk_Scaling
        {
                AsicFeatureState_Enabled,
                0, //min Hz
                307200000, //max Hz
        },
        //AsicFeatureId_Min_Adsp_BW_Vote
        {
                AsicFeatureState_Enabled,
                10000000, //min bytes per second
                0x7FFFFFFF, //max bytes per second
        },
        //AsicFeatureId_InitialState
        {
                AsicFeatureState_Enabled,
                0, //min
                0, //max
        },
        //AsicFeatureId_TimelineOptimisationMips
        {
                AsicFeatureState_Disabled,
                240, //min MIPS for periodic clients
                905  //max MIPS value
        },
        //AsicFeatureId_TimelineOptimisationBw
        {
                AsicFeatureState_Disabled,
                76000000*8, //min ext BW in bytes per second (targeting above 76Mhz SNOC clk)
                0x7FFFFFFF, //max ext BW in bytes per second
        },
        //AsicFeatureId_TimelineOptimisationAhb
        {
                AsicFeatureState_Disabled,
                60000000, //min AHB clock in Hz
                153600000, //max AHB clock in Hz
        },
        //AsicFeatureId_LpassClkSleepOptimization
        {
                AsicFeatureState_Disabled,
                0, //min
                0, //max
        },
        //AsicFeatureId_LPMRetention
        {
                AsicFeatureState_Disabled,
                0, //min
                0, //max
        },
        //AsicFeatureId_DomainCoreClocks
        {
                AsicFeatureState_Disabled,
                0, //min
                0, //max
        },
        // AsicFeatureId_CachePartitionControl
        {
                AsicFeatureState_Enabled,
                0, // min (not used)
                0, // max (not used)
        },
        // AsicFeatureId_DcvsControl
        {
                AsicFeatureState_Enabled,
                0, // min (not used)
                0, // max (not used)
        },
        // AsicFeatureId_AHBE_Q6Scaling 
        {
                AsicFeatureState_Enabled,
                0, // min (not used)
                0, // max (not used)
        },
        // AsicFeatureId_CacheSizeMPPSThreshold
        {
                AsicFeatureState_Enabled,
                0, // min (not used)
                0, // max (not used)
        },
        // AsicFeatureId_CacheSizeBWScaling
        {
                AsicFeatureState_Enabled,
                0, // min (not used)
                0, // max (not used)
        },
        // AsicFeatureId_SystemCacheSupport
        {
                AsicFeatureState_Enabled,
                0, // min (not used)
                0, // max (not used)
        },		
        //AsicFeatureId_ComputeDSP
        {
                AsicFeatureState_Disabled,
                0, //min
                0, //max
        },
        //AsicFeatureId_SlpiDSP
        {
                AsicFeatureState_Disabled,
                0, //min
                0, //max
        },
       //AsicFeatureId_RPMh
        {
                AsicFeatureState_Enabled,
                0, //min
                0, //max
        },
       //AsicFeatureId_Alc
        {
                AsicFeatureState_Disabled,
                0, //min
                0, //max
        },
       //AsicFeatureId_AhbIX_Q6Scaling
        {
                AsicFeatureState_Disabled,
                0, //min
                204800000*4, //max corresponding to Q6 at turbo
        },
		//AsicFeatureId_AhbE_VoteScaling
        {
                AsicFeatureState_Enabled,
                0, //min
                0, //max
        },
		//AsicFeatureId_LpassSlpiDSP
        {
                AsicFeatureState_Enabled,
                0, //min
                0, //max
        },
};

