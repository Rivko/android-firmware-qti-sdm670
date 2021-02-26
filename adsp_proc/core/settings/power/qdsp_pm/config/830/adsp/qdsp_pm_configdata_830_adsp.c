/*
* Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: adsppm_configdata_845.c
@brief: Contains all 845-specific data definitions for Config Manager.

$Header: //components/rel/core.qdsp6/2.1/settings/power/qdsp_pm/config/830/adsp/qdsp_pm_configdata_830_adsp.c#1 $
*/

#include "adsppm_settings.h"
#include "asic_settings.h"
#include "asic_internal_settings.h"
#include "adsppm_utils_settings.h"
#include "icbid.h"
#include "DDIIPCInt.h"
#include "qdsp_pm_clocks_hwio_830_adsp.h"
#include "msmhwiobase.h"
#include "limits.h"
#include "systemcache/systemcache.h"

AsicHwioRegRangeType lpassRegRange_845 = {LPASS_BASE, LPASS_BASE_PHYS, LPASS_BASE_SIZE};

// L2 configuration registers base address = TCM_BASE + 1.5 Meg + 128K
// Reference: Fig 8-3 in document 80-V9418-22 Rev C
#define TCM_BASE 0x17400000
#define L2_CONFIG_BASE (TCM_BASE + 0x00180000 + 0x00020000)
AsicHwioRegRangeType l2ConfigRegRange_845 = {L2_CONFIG_BASE, L2_CONFIG_BASE, 0x1004 /* max expected offset needed by ADPSPM */};

/**
 * Array of 845 Core Clocks per core. Arranged by to core ID
 */
const AdsppmClkIdType coreClocks_SlimBus_845[] = {AdsppmClk_Slimbus_Core, AdsppmClk_Slimbus2_Core};
//const AdsppmClkIdType coreClocks_Midi_845[]    = {AdsppmClk_Midi_Core};
const AdsppmClkIdType coreClocks_HwRsp_845[]   = {AdsppmClk_HwRsp_Core};

/**
 * Array of 845 Master Bus Ports per core. Arranged by to core ID
 */
const AdsppmBusPortIdType masterPorts_ADSP_845[]    = {AdsppmBusPort_Adsp_Master};
const AdsppmBusPortIdType masterPorts_DML_845[]     = {AdsppmBusPort_Dml_Master};
const AdsppmBusPortIdType masterPorts_AIF_845[]     = {AdsppmBusPort_Aif_Master};
const AdsppmBusPortIdType masterPorts_SlimBus_845[] = {AdsppmBusPort_Slimbus_Master, AdsppmBusPort_Slimbus2_Master};
//const AdsppmBusPortIdType masterPorts_Midi_845[]    = {AdsppmBusPort_Midi_Master};
const AdsppmBusPortIdType masterPorts_HWRSMP_845[]  = {AdsppmBusPort_HwRsmp_Master};
//const AdsppmBusPortIdType masterPorts_Spdif_845[]   = {AdsppmBusPort_Spdif_Master};
//const AdsppmBusPortIdType masterPorts_Hdmirx_845[]  = {AdsppmBusPort_Hdmirx_Master};
const AdsppmBusPortIdType masterPorts_Hdmitx_845[]  = {AdsppmBusPort_Hdmitx_Master};
//const AdsppmBusPortIdType masterPorts_Sif_845[]     = {AdsppmBusPort_Sif_Master};
const AdsppmBusPortIdType masterPorts_DPAudio_845[] = {AdsppmBusPort_DPAudio_Master};

/**
 * Array of 845 Slave Bus Ports per core. Arranged by to core ID
 */
const AdsppmBusPortIdType slavePorts_ADSP_845[]    = {AdsppmBusPort_Adsp_Slave};
const AdsppmBusPortIdType slavePorts_LPM_845[]     = {AdsppmBusPort_Lpm_Slave};
const AdsppmBusPortIdType slavePorts_DML_845[]     = {AdsppmBusPort_Dml_Slave};
const AdsppmBusPortIdType slavePorts_AIF_845[]     = {AdsppmBusPort_Aif_Slave};
const AdsppmBusPortIdType slavePorts_SlimBus_845[] = {AdsppmBusPort_Slimbus_Slave, AdsppmBusPort_Slimbus2_Slave};
//const AdsppmBusPortIdType slavePorts_Midi_845[]    = {AdsppmBusPort_Midi_Slave};
const AdsppmBusPortIdType slavePorts_AVsync_845[]  = {AdsppmBusPort_AvSync_Slave};
const AdsppmBusPortIdType slavePorts_HWRSMP_845[]  = {AdsppmBusPort_HwRsmp_Slave};
//const AdsppmBusPortIdType slavePorts_SRam_845[]    = {AdsppmBusPort_Sram_Slave};
//const AdsppmBusPortIdType slavePorts_DCodec_845[]  = {AdsppmBusPort_Dcodec_Slave};
//const AdsppmBusPortIdType slavePorts_Spdif_845[]   = {AdsppmBusPort_Spdif_Slave};
//const AdsppmBusPortIdType slavePorts_Hdmirx_845[]  = {AdsppmBusPort_Hdmirx_Slave};
const AdsppmBusPortIdType slavePorts_Hdmitx_845[]  = {AdsppmBusPort_Hdmitx_Slave};
//const AdsppmBusPortIdType slavePorts_Sif_845[]     = {AdsppmBusPort_Sif_Slave};
//const AdsppmBusPortIdType slavePorts_BSTC_845[]    = {AdsppmBusPort_Bstc_Slave};
const AdsppmBusPortIdType slavePorts_DPAudio_845[] = {AdsppmBusPort_DPAudio_Slave};

/**
 * Array of 845 core descriptors. Arranged according to core ID enum
 */
const AsicCoreDescType cores_array_845[] =
{
        {   //Adsppm_Core_Id_None
                Adsppm_Core_Id_None, //Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_None, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_None, //Sleep Latency
                        AsicRsc_None, //MIPS
                        AsicRsc_None, //BW
                        AsicRsc_None //Thermal
                }, //HwResource IDs
                AsicPowerDomain_AON, //Power Domain
                {0, NULL}, //Core clock instances
                {0, NULL}, //Master port instances
                {0, NULL}, //Slave port instances
                Adsppm_Instance_Id_None //Number of core instances
        },
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
                {ADSPPM_ARRAY(masterPorts_ADSP_845)}, //Master port instances
                {ADSPPM_ARRAY(slavePorts_ADSP_845)}, //Slave port instances
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
                {ADSPPM_ARRAY(slavePorts_LPM_845)}, //Slave port instances
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
                {ADSPPM_ARRAY(masterPorts_DML_845)}, //Master port instances
                {ADSPPM_ARRAY(slavePorts_DML_845)}, //Slave port instances
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
                {ADSPPM_ARRAY(masterPorts_AIF_845)}, //Master port instances
                {ADSPPM_ARRAY(slavePorts_AIF_845)}, //Slave port instances
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
                {ADSPPM_ARRAY(coreClocks_SlimBus_845)}, //Core clock instances
                {ADSPPM_ARRAY(masterPorts_SlimBus_845)}, //Master port instances
                {ADSPPM_ARRAY(slavePorts_SlimBus_845)}, //Slave port instances
                Adsppm_Instance_Id_1 //Number of core instances
        },
        {   //Adsppm_Core_Id_Midi
                Adsppm_Core_Id_None, //Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_None, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_None, //Sleep Latency
                        AsicRsc_None, //MIPS
                        AsicRsc_None, //BW
                        AsicRsc_None //Thermal
                }, //HwResource IDs
                AsicPowerDomain_AON, //Power Domain
                {0, NULL}, //Core clock instances
                {0, NULL}, //Master port instances
                {0, NULL}, //Slave port instances
                Adsppm_Instance_Id_None //Number of core instances
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
                {0, NULL}, //Core clock instances
                {0, NULL}, //Master port instances
                {ADSPPM_ARRAY(slavePorts_AVsync_845)}, //Slave port instances
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
                {ADSPPM_ARRAY(coreClocks_HwRsp_845)}, //Core clock instances
                {ADSPPM_ARRAY(masterPorts_HWRSMP_845)}, //Master port instances
                {ADSPPM_ARRAY(slavePorts_HWRSMP_845)}, //Slave port instances
                Adsppm_Instance_Id_0 //Number of core instances
        },
        {   //Adsppm_Core_Id_SRam
                Adsppm_Core_Id_None, //Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_None, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_None, //Sleep Latency
                        AsicRsc_None, //MIPS
                        AsicRsc_None, //BW
                        AsicRsc_None //Thermal
                }, //HwResource IDs
                AsicPowerDomain_AON, //Power Domain
                {0, NULL}, //Core clock instances
                {0, NULL}, //Master port instances
                {0, NULL}, //Slave port instances
                Adsppm_Instance_Id_None //Number of core instances
        },
        {   //Adsppm_Core_Id_DCodec
                Adsppm_Core_Id_None, //Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_None, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_None, //Sleep Latency
                        AsicRsc_None, //MIPS
                        AsicRsc_None, //BW
                        AsicRsc_None //Thermal
                }, //HwResource IDs
                AsicPowerDomain_AON, //Power Domain
                {0, NULL}, //Core clock instances
                {0, NULL}, //Master port instances
                {0, NULL}, //Slave port instances
                Adsppm_Instance_Id_None //Number of core instances
        },
        {   //Adsppm_Core_Id_Spdif
                Adsppm_Core_Id_None, //Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_None, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_None, //Sleep Latency
                        AsicRsc_None, //MIPS
                        AsicRsc_None, //BW
                        AsicRsc_None //Thermal
                }, //HwResource IDs
                AsicPowerDomain_AON, //Power Domain
                {0, NULL}, //Core clock instances
                {0, NULL}, //Master port instances
                {0, NULL}, //Slave port instances
                Adsppm_Instance_Id_None //Number of core instances
        },
        {   //Adsppm_Core_Id_Hdmirx
                Adsppm_Core_Id_None, //Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_None, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_None, //Sleep Latency
                        AsicRsc_None, //MIPS
                        AsicRsc_None, //BW
                        AsicRsc_None //Thermal
                }, //HwResource IDs
                AsicPowerDomain_AON, //Power Domain
                {0, NULL}, //Core clock instances
                {0, NULL}, //Master port instances
                {0, NULL}, //Slave port instances
                Adsppm_Instance_Id_None //Number of core instances

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
                {ADSPPM_ARRAY(masterPorts_Hdmitx_845)}, //Master port instances
                {ADSPPM_ARRAY(slavePorts_Hdmitx_845)}, //Slave port instances
                Adsppm_Instance_Id_0 //Number of core instances
        },
        {   //Adsppm_Core_Id_Sif
                Adsppm_Core_Id_None, //Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_None, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_None, //Sleep Latency
                        AsicRsc_None, //MIPS
                        AsicRsc_None, //BW
                        AsicRsc_None //Thermal
                }, //HwResource IDs
                AsicPowerDomain_AON, //Power Domain
                {0, NULL}, //Core clock instances
                {0, NULL}, //Master port instances
                {0, NULL}, //Slave port instances
                Adsppm_Instance_Id_None //Number of core instances
        },
        {   //Adsppm_Core_Id_BSTC
                Adsppm_Core_Id_None, //Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_None, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_None, //Sleep Latency
                        AsicRsc_None, //MIPS
                        AsicRsc_None, //BW
                        AsicRsc_None //Thermal
                }, //HwResource IDs
                AsicPowerDomain_AON, //Power Domain
                {0, NULL}, //Core clock instances
                {0, NULL}, //Master port instances
                {0, NULL}, //Slave port instances
                Adsppm_Instance_Id_None //Number of core instances
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
                {ADSPPM_ARRAY(masterPorts_DPAudio_845)}, //Master port instances
                {ADSPPM_ARRAY(slavePorts_DPAudio_845)}, //Slave port instances
                Adsppm_Instance_Id_0 //Number of core instances
        }
};

AsicCoreDescriptorArrayType cores_845 = {ADSPPM_ARRAY(cores_array_845)};

const AsicMemDescriptorType memories_array_845[] =
{
        {   //Adsppm_Mem_None
                Adsppm_Mem_None, //Mem ID
                AsicPowerDomain_AON //Power Domain
        },
        {   //Adsppm_Mem_Ocmem
                Adsppm_Mem_Ocmem, //Mem ID
                AsicPowerDomain_Ocmem //Power Domain
        },
        {   //Adsppm_Mem_Lpass_LPM
                Adsppm_Mem_Lpass_LPM, //Mem ID
                AsicPowerDomain_Lpm //Power Domain
        },
        {   //Adsppm_Mem_Sram
                Adsppm_Mem_Sram, //Mem ID
                AsicPowerDomain_SRam //Power Domain
        }
};

AsicMemDescriptorArrayType memories_845 = {ADSPPM_ARRAY(memories_array_845)};

/**
 * Array of 845 core clock descriptors. Arranged by core clock ID
 */
const AsicClkDescriptorType clocks_array_845[AdsppmClk_EnumMax] =
{
        {
                AdsppmClk_None, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "", //Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
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
        // LPASS_AUDIO_CORE_SYSNOC_MPORT_CORE_CGCR doesn't exist on 845
        {
                AdsppmClk_None, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "", //Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_HwRsp_Hclk, //Clk ID
                AsicClk_TypeInternalCGC, //Clk Type
                AsicClk_CntlSW_DCG, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.hwioInfo =
                {
                        HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_CGCR_OFFS, //CGCR Offset
                        HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_CGCR_CLK_ENABLE_BMSK, //Enable mask
                        HWIO_LPASS_AUDIO_CORE_RESAMPLER_CORE_CGCR_HW_CTL_BMSK, //HW control mask
                        0 //Status mask
                },
                AdsppmClk_Ahb_Root, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Dml_Hclk, //Clk ID
                AsicClk_TypeInternalCGC, //Clk Type
                AsicClk_CntlSW_DCG, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.hwioInfo =
                {
                        HWIO_LPASS_AUDIO_CORE_DML_CORE_CGCR_OFFS, //CGCR Offset
                        HWIO_LPASS_AUDIO_CORE_DML_CORE_CGCR_CLK_ENABLE_BMSK, //Enable mask
                        HWIO_LPASS_AUDIO_CORE_DML_CORE_CGCR_HW_CTL_BMSK, //HW control mask
                        0 //Status mask
                },
                AdsppmClk_Ahb_Root, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Aif_Hclk, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "", //Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Aif_Csr_Hclk, //Clk ID
                AsicClk_TypeInternalCGC, //Clk Type
                AsicClk_CntlSW_DCG, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.hwioInfo =
                {
                        HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_CGCR_OFFS, //CGCR Offset
                        HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_CGCR_CLK_ENABLE_BMSK, //Enable mask
                        HWIO_LPASS_AUDIO_CORE_LPAIF_CSR_CORE_CGCR_HW_CTL_BMSK, //HW control mask
                        0 //Status mask
                },
                AdsppmClk_None, //Source
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
                AdsppmClk_Midi_Hclk, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "", //Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_AvSync_Hclk, //Clk ID
                AsicClk_TypeInternalCGC, //Clk Type
                AsicClk_CntlSW_DCG, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.hwioInfo =
                {
                        HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_CGCR_OFFS, //CGCR Offset
                        HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_CGCR_CLK_ENABLE_BMSK, //Enable mask
                        HWIO_LPASS_AUDIO_CORE_AVSYNC_CORE_CGCR_HW_CTL_BMSK, //HW control mask
                        0 //Status mask
                },
                AdsppmClk_Ahb_Root, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Atimer_Hclk, //Clk ID
                AsicClk_TypeInternalCGC, //Clk Type
                AsicClk_CntlAlwaysON_DCG, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.hwioInfo =
                {
                        HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_CGCR_OFFS, //CGCR Offset
                        HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_CGCR_CLK_ENABLE_BMSK, //Enable mask
                        HWIO_LPASS_AUDIO_CORE_ATIMER_CORE_CGCR_HW_CTL_BMSK, //HW control mask
                        0 //Status mask
                },
                AdsppmClk_Ahb_Root, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Lpm_Hclk, //Clk ID
                AsicClk_TypeInternalCGC, //Clk Type
                AsicClk_CntlSW_DCG, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.hwioInfo =
                {
                        HWIO_LPASS_AUDIO_CORE_LPM_CORE_CGCR_OFFS, //CGCR Offset
                        HWIO_LPASS_AUDIO_CORE_LPM_CORE_CGCR_CLK_ENABLE_BMSK, //Enable mask
                        HWIO_LPASS_AUDIO_CORE_LPM_CORE_CGCR_HW_CTL_BMSK, //HW control mask
                        0 //Status mask
                },
                AdsppmClk_Lpm_cbc, //Source
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
                AdsppmClk_Csr_Hclk, //Clk ID
                AsicClk_TypeInternalCGC, //Clk Type
                AsicClk_CntlAlwaysON_DCG, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.hwioInfo =
                {
                        HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_CGCR_OFFS, //CGCR Offset
                        HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_CGCR_CLK_ENABLE_BMSK, //Enable mask
                        HWIO_LPASS_AUDIO_CORE_CORE_CSR_CORE_CGCR_HW_CTL_BMSK, //HW control mask
                        0 //Status mask
                },
                AdsppmClk_Ahb_Root, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Dcodec_Hclk, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "", //Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Spdif_Hmclk, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "", //Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Spdif_Hsclk, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "", //Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Hdmirx_Hclk, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "", //Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Hdmitx_Hclk, //Clk ID
                AsicClk_TypeInternalCGC, //Clk Type
                AsicClk_CntlSW_DCG, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.hwioInfo =
                {
                        HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_CGCR_OFFS, //CGCR Offset
                        HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_CGCR_CLK_ENABLE_BMSK, //Enable mask
                        HWIO_LPASS_AUDIO_CORE_HDMITX_CORE_CGCR_HW_CTL_BMSK, //HW control mask
                        0 //Status mask
                },
                AdsppmClk_Ahb_Root, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Sif_Hclk, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "", //Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Bstc_Hclk, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "", //Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
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
        {
                AdsppmClk_Sysnoc_Hclk, //Clk ID
                AsicClk_TypeInternalCGC, //Clk Type
                AsicClk_CntlAlwaysON_DCG, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.hwioInfo =
                {
                        HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CGCR_OFFS, //CGCR Offset
                        HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CGCR_CLK_ENABLE_BMSK, //Enable mask
                        HWIO_LPASS_AUDIO_CORE_SYSNOC_SWAY_CORE_CGCR_HW_CTL_BMSK, //HW control mask
                        0 //Status mask
                },
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
                AdsppmClk_Bus_Timeout_Hclk, //Clk ID
                AsicClk_TypeInternalCGC, //Clk Type
                AsicClk_CntlAlwaysON_DCG, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.hwioInfo =
                {
                        HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CGCR_OFFS, //CGCR Offset
                        HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CGCR_CLK_ENABLE_BMSK, //Enable mask
                        HWIO_LPASS_AUDIO_CORE_BUS_TIMEOUT_CORE_CGCR_HW_CTL_BMSK, //HW control mask
                        0 //Status mask
                },
                AdsppmClk_Ahb_Root, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Tlb_Preload_Hclk, //Clk ID
                AsicClk_TypeInternalCGC, //Clk Type
                AsicClk_CntlAlwaysON_DCG, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.hwioInfo =
                {
                        HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_CGCR_OFFS, //CGCR Offset
                        HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_CGCR_CLK_ENABLE_BMSK, //Enable mask
                        HWIO_LPASS_AUDIO_CORE_TLB_PRELOAD_CORE_CGCR_HW_CTL_BMSK, //HW control mask
                        0 //Status mask
                },
                AdsppmClk_Ahb_Root, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Qos_Hclk, //Clk ID
                AsicClk_TypeInternalCGC, //Clk Type
                AsicClk_CntlAlwaysON_DCG, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.hwioInfo =
                {
                        HWIO_LPASS_AUDIO_CORE_QOS_CORE_CGCR_OFFS, //CGCR Offset
                        HWIO_LPASS_AUDIO_CORE_QOS_CORE_CGCR_CLK_ENABLE_BMSK, //Enable mask
                        HWIO_LPASS_AUDIO_CORE_QOS_CORE_CGCR_HW_CTL_BMSK, //HW control mask
                        0 //Status mask
                },
                AdsppmClk_Ahb_Root, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Qdsp_Sway_Hclk, //Clk ID
                AsicClk_TypeInternalCGC, //Clk Type
                AsicClk_CntlAlwaysON_DCG, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.hwioInfo =
                {
                        HWIO_LPASS_AUDIO_CORE_QDSP_SWAY_CORE_CGCR_OFFS, //CGCR Offset
                        HWIO_LPASS_AUDIO_CORE_QDSP_SWAY_CORE_CGCR_CLK_ENABLE_BMSK, //Enable mask
                        HWIO_LPASS_AUDIO_CORE_QDSP_SWAY_CORE_CGCR_HW_CTL_BMSK, //HW control mask
                        0 //Status mask
                },
                AdsppmClk_Ahb_Root, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_AhbE_Hclk, //Clk ID
                AsicClk_TypeDalFreqSet, //Clk Type
                AsicClk_CntlAlwaysON, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "lpass_audio_wrapper_aon_clk", //Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Adsp_Hmclk, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlAlwaysON, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "lpass_q6ss_ahbm_aon_clk", //Name
                AdsppmClk_AhbE_Hclk, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Adsp_Hsclk, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlAlwaysON, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "lpass_q6ss_ahbs_aon_clk", //Name
                AdsppmClk_AhbE_Hclk, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Sram_Hclk, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "", //Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Lcc_Hclk, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlAlwaysON, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "lpass_audio_core_qdsp_sway_aon_clk", //Name
                AdsppmClk_AhbE_Hclk, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Security_Hclk, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlAlwaysON, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "lpass_audio_wrapper_sysnoc_sway_aon_clk", //Name
                AdsppmClk_AhbE_Hclk, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Audio_Core_AON, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlAlwaysON, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "lpass_audio_wrapper_qos_ahbs_aon_clk", //Name
                AdsppmClk_AhbE_Hclk, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Audio_Wrapper_AON, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlAlwaysON, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "lpass_audio_wrapper_bus_timeout_aon_clk", //Name
                AdsppmClk_AhbE_Hclk, //Source
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
                AdsppmClk_Midi_Core, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "", //Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_AvSync_Xo, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "", //Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_AvSync_Bt, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "", //Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_AvSync_Fm, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "", //Name
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
                AdsppmClk_Atime2_core, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "", //Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        }
};

AsicClockDescriptorArrayType clocks_845 = {ADSPPM_ARRAY(clocks_array_845)};

/**
 * Arrays of 845 register programming clocks
 */
const AdsppmClkIdType regProgClocks_Dml_845[]      = {AdsppmClk_Dml_Hclk};
const AdsppmClkIdType regProgClocks_Aif_845[]      = {AdsppmClk_Aif_Csr_Hclk};
const AdsppmClkIdType regProgClocks_Slimbus_845[]  = {AdsppmClk_Slimbus_Hclk};
const AdsppmClkIdType regProgClocks_Slimbus2_845[] = {AdsppmClk_Slimbus2_Hclk};
//const AdsppmClkIdType regProgClocks_Midi_845[]     = {AdsppmClk_Midi_Hclk};
const AdsppmClkIdType regProgClocks_HwRsmp_845[]   = {AdsppmClk_HwRsp_Hclk};
const AdsppmClkIdType regProgClocks_AvSync_845[]   = {AdsppmClk_AvSync_Hclk};
const AdsppmClkIdType regProgClocks_Lpm_845[]      = {AdsppmClk_Lpm_Hclk};
const AdsppmClkIdType regProgClocks_Sram_845[]     = {AdsppmClk_Sram_Hclk};
//const AdsppmClkIdType regProgClocks_Spdif_845[]    = {AdsppmClk_Spdif_Hsclk};
//const AdsppmClkIdType regProgClocks_Hdmirx_845[]   = {AdsppmClk_Hdmirx_Hclk};
const AdsppmClkIdType regProgClocks_Hdmitx_845[]   = {AdsppmClk_Hdmitx_Hclk};
//const AdsppmClkIdType regProgClocks_Sif_845[]      = {AdsppmClk_Sif_Hclk};
//const AdsppmClkIdType regProgClocks_Bstc_845[]     = {AdsppmClk_Bstc_Hclk};
//const AdsppmClkIdType regProgClocks_Dcodec_845[]   = {AdsppmClk_Dcodec_Hclk};
const AdsppmClkIdType regProgClocks_Adsp_845[]     = {AdsppmClk_Adsp_Hmclk, AdsppmClk_Adsp_Hsclk};

/**
 * Array of 845 Bus port descriptors arranged by Bus port ID
 */
const AsicBusPortDescriptorType busPorts_array_845[] =
{
        {
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
        {
                AdsppmBusPort_Dml_Master, //ID
                AsicBusPort_AhbI_M, //Connection
                AdsppmClk_Dml_Hclk, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbMaster = ICBID_MASTER_LPASS_AHB}, //icbarb ID
                AdsppmBusPort_Ext_Ahb_Master //Access port
        },
        {
                AdsppmBusPort_Aif_Master, //ID
                AsicBusPort_AhbI_M, //Connection
                AdsppmClk_Aif_Hclk, //Bus clock
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
                AdsppmBusPort_Midi_Master, //ID
                0, //Connection
                AdsppmClk_None, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbMaster = ICBID_MASTER_APPSS_PROC}, //icbarb ID
                AdsppmBusPort_None //Access port
        },
        {
                AdsppmBusPort_HwRsmp_Master, //ID
                AsicBusPort_AhbI_M, //Connection
                AdsppmClk_HwRsp_Hclk, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbMaster = ICBID_MASTER_LPASS_AHB}, //icbarb ID
                AdsppmBusPort_Ext_Ahb_Master //Access port
        },
        {
                AdsppmBusPort_Ext_Ahb_Master, //ID
                AsicBusPort_Ext_M, //Connection
                AdsppmClk_AhbX_Hclk, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbMaster = ICBID_MASTER_LPASS_AHB}, //icbarb ID
                AdsppmBusPort_Ext_Ahb_Master //Access port
        },
        {
                AdsppmBusPort_Spdif_Master, //ID
                0, //Connection
                AdsppmClk_None, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbMaster = ICBID_MASTER_APPSS_PROC}, //icbarb ID
                AdsppmBusPort_None //Access port
        },
        {
                AdsppmBusPort_Hdmirx_Master, //ID
                0, //Connection
                AdsppmClk_None, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbMaster = ICBID_MASTER_APPSS_PROC}, //icbarb ID
                AdsppmBusPort_None //Access port

        },
        {
                AdsppmBusPort_Hdmitx_Master, //ID
                AsicBusPort_AhbI_M, //Connection
                AdsppmClk_Hdmitx_Hclk, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbMaster = ICBID_MASTER_LPASS_AHB}, //icbarb ID
                AdsppmBusPort_Ext_Ahb_Master //Access port
        },
        {
                AdsppmBusPort_Sif_Master, //ID
                0, //Connection
                AdsppmClk_None, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbMaster = ICBID_MASTER_APPSS_PROC}, //icbarb ID
                AdsppmBusPort_None //Access port
        },
        {
                AdsppmBusPort_DPAudio_Master, //ID
                AsicBusPort_AhbI_M, //Connection
                AdsppmClk_Hdmitx_Hclk, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbMaster = ICBID_MASTER_LPASS_AHB}, //icbarb ID
                AdsppmBusPort_Ext_Ahb_Master //Access port
        },
        {
                AdsppmBusPort_Dml_Slave, //ID
                AsicBusPort_AhbI_S, //Connection
                AdsppmClk_Dml_Hclk, //Bus clock
                {ADSPPM_ARRAY(regProgClocks_Dml_845)}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_LPASS}, //icbarb ID
                AdsppmBusPort_Ext_Ahb_Slave //Access port
        },
        {
                AdsppmBusPort_Aif_Slave, //ID
                AsicBusPort_AhbI_S, //Connection
                AdsppmClk_Aif_Csr_Hclk, //Bus clock
                {ADSPPM_ARRAY(regProgClocks_Aif_845)}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_LPASS}, //icbarb ID
                AdsppmBusPort_Ext_Ahb_Slave //Access port
        },
        {
                AdsppmBusPort_Slimbus_Slave, //ID
                AsicBusPort_AhbI_S, //Connection
                AdsppmClk_Slimbus_Hclk, //Bus clock
                {ADSPPM_ARRAY(regProgClocks_Slimbus_845)}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_LPASS}, //icbarb ID
                AdsppmBusPort_Ext_Ahb_Slave //Access port
        },
        {
                AdsppmBusPort_Slimbus2_Slave, //ID
                AsicBusPort_AhbI_S, //Connection
                AdsppmClk_Slimbus2_Hclk, //Bus clock
                {ADSPPM_ARRAY(regProgClocks_Slimbus2_845)}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_LPASS}, //icbarb ID
                AdsppmBusPort_Ext_Ahb_Slave //Access port
        },
        {
                AdsppmBusPort_Midi_Slave, //ID
                0, //Connection
                AdsppmClk_None, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_LPASS}, //icbarb ID
                AdsppmBusPort_None //Access port
        },
        {
                AdsppmBusPort_HwRsmp_Slave, //ID
                AsicBusPort_AhbI_S, //Connection
                AdsppmClk_HwRsp_Hclk, //Bus clock
                {ADSPPM_ARRAY(regProgClocks_HwRsmp_845)}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_LPASS}, //icbarb ID
                AdsppmBusPort_Ext_Ahb_Slave //Access port
        },
        {
                AdsppmBusPort_AvSync_Slave, //ID
                AsicBusPort_AhbI_S, //Connection
                AdsppmClk_AvSync_Hclk, //Bus clock
                {ADSPPM_ARRAY(regProgClocks_AvSync_845)}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_LPASS}, //icbarb ID
                AdsppmBusPort_Ext_Ahb_Slave //Access port
        },
        {
                AdsppmBusPort_Lpm_Slave, //ID
                AsicBusPort_AhbI_S, //Connection
                AdsppmClk_Lpm_Hclk, //Bus clock
                {ADSPPM_ARRAY(regProgClocks_Lpm_845)}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_LPASS}, //icbarb ID
                AdsppmBusPort_Ext_Ahb_Slave //Access port
        },
        {
                AdsppmBusPort_Sram_Slave, //ID
                0, //Connection
                AdsppmClk_None, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_LPASS}, //icbarb ID
                AdsppmBusPort_None //Access port
        },
        {
                AdsppmBusPort_Ext_Ahb_Slave, //ID
                AsicBusPort_Ext_S, //Connection
                AdsppmClk_AhbE_Hclk, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_LPASS}, //icbarb ID
                AdsppmBusPort_Ext_Ahb_Slave //Access port
        },
        {
                AdsppmBusPort_Ddr_Slave, //ID
                AsicBusPort_Ext_S, //Connection
                AdsppmClk_None, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_EBI1}, //icbarb ID
                AdsppmBusPort_Ext_Ahb_Master //Access port
        },
        {
                AdsppmBusPort_Ocmem_Slave, //ID
                0, //Connection
                AdsppmClk_None, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_OCMEM}, //icbarb ID
                AdsppmBusPort_None //Access port
        },
        {
                AdsppmBusPort_PerifNoc_Slave, //ID
                AsicBusPort_Ext_S, //Connection
                AdsppmClk_None, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_BLSP_1}, //icbarb ID
                AdsppmBusPort_Ext_Ahb_Master //Access port
        },
        {
                AdsppmBusPort_Snoc_Slave, //ID
                AsicBusPort_Ext_S, //Connection
                AdsppmClk_None, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_SNOC_MEM_NOC_SF }, //icbarb ID
                AdsppmBusPort_Ext_Ahb_Master //Access port
        },        
        {
                AdsppmBusPort_Spdif_Slave, //ID
                0, //Connection
                AdsppmClk_None, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_LPASS}, //icbarb ID
                AdsppmBusPort_None //Access port
        },
        {
                AdsppmBusPort_Hdmirx_Slave, //ID
                0, //Connection
                AdsppmClk_None, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_LPASS}, //icbarb ID
                AdsppmBusPort_None //Access port

        },
        {
                AdsppmBusPort_Hdmitx_Slave, //ID
                AsicBusPort_AhbI_S, //Connection
                AdsppmClk_Hdmitx_Hclk, //Bus clock
                {ADSPPM_ARRAY(regProgClocks_Hdmitx_845)}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_LPASS}, //icbarb ID
                AdsppmBusPort_Ext_Ahb_Slave //Access port
        },
        {
                AdsppmBusPort_Sif_Slave, //ID
                0, //Connection
                AdsppmClk_None, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_LPASS}, //icbarb ID
                AdsppmBusPort_None //Access port
        },
        {
                AdsppmBusPort_Bstc_Slave, //ID
                0, //Connection
                AdsppmClk_None, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_LPASS}, //icbarb ID
                AdsppmBusPort_None //Access port
        },
        {
                AdsppmBusPort_Dcodec_Slave, //ID
                0, //Connection
                AdsppmClk_None, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_LPASS}, //icbarb ID
                AdsppmBusPort_None //Access port
        },
        {
                AdsppmBusPort_DPAudio_Slave, //ID
                AsicBusPort_AhbI_S, //Connection
                AdsppmClk_Hdmitx_Hclk, //Bus clock
                {ADSPPM_ARRAY(regProgClocks_Hdmitx_845)}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_LPASS}, //icbarb ID
                AdsppmBusPort_Ext_Ahb_Slave //Access port
        },
        {
                AdsppmBusPort_Core, //ID
                0, //Connection
                AdsppmClk_None, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_LPASS}, //icbarb ID
                AdsppmBusPort_None //Access port
        },
        {
                AdsppmBusPort_Adsp_Slave, //ID
                AsicBusPort_AhbE_S, //Connection
                AdsppmClk_Adsp_Hsclk, //Bus clock
                {ADSPPM_ARRAY(regProgClocks_Adsp_845)}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_LPASS}, //icbarb ID
                AdsppmBusPort_Adsp_Master //Access port
        }
};

AsicBusPortDescriptorArrayType busPorts_845 = {ADSPPM_ARRAY(busPorts_array_845)};

/**
 * List of supported external bus routes
 */
const AdsppmBusRouteType extBusRoutes_array_845[] =
{
        {
                AdsppmBusPort_Adsp_Master, //master
                AdsppmBusPort_Ddr_Slave //slave
        },
        {
                AdsppmBusPort_Adsp_Master, //master
                AdsppmBusPort_PerifNoc_Slave //slave
        },
        {
                AdsppmBusPort_Ext_Ahb_Master, //master
                AdsppmBusPort_Ddr_Slave //slave
        }
};

AsicBusRouteArrayType extBusRoutes_845 = {ADSPPM_ARRAY(extBusRoutes_array_845)};

const AdsppmBusRouteType mipsBwRoutes_array_845[] =
{
        {
                AdsppmBusPort_Adsp_Master,
                AdsppmBusPort_Ddr_Slave
        }
};

AsicBusRouteArrayType mipsBwRoutes_845 = {ADSPPM_ARRAY(mipsBwRoutes_array_845)};

/**
* Array of register programming speeds in byte/sec
*/
const uint64 regProgSpeeds_array_845[] =
{
        0,          //Adsppm_RegProg_None
        19200000*4, //Adsppm_RegProg_Norm
        61410000*4  //Adsppm_RegProg_Fast
};

AsicRegProgSpeedArrayType regProgSpeeds_845 = {ADSPPM_ARRAY(regProgSpeeds_array_845)};

/**
* Array of thread leakage values, used in mppsv3 calc.
*/
const uint32 threadLeakageData_array_845[] =
{
    0,  // 1T Leakage
    20, // 2T Leakage
    33  // 3T Leakage
};

const AsicThreadLeakageArrayType threadLeakageData_845 = {ADSPPM_ARRAY(threadLeakageData_array_845)};

/**
 * Array of power domain descriptors
 */
const AsicPowerDomainDescriptorType pwrDomains_array_845[] =
{
        {
                AsicPowerDomain_AON, //Id
                "", //Name
                AsicRsc_None, //Type
                AdsppmClk_None, //Clock ID
                DALIPCINT_NUM_INTS, //TZ notify
                0, //TZ Done
                {0, NULL} //array of Security clocks
        },
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
        {
                AsicPowerDomain_Lpm, //Id
                "", //Name
                AsicRsc_Power_Mem, //Type
                AdsppmClk_Lpm_cbc, //Clock ID
                DALIPCINT_NUM_INTS, //TZ notify
                0, //TZ Done
                {0, NULL} //array of Security clocks
        },
        {
                AsicPowerDomain_SRam, //Id
                "", //Name
                AsicRsc_None, //Type
                AdsppmClk_None, //Clock ID
                DALIPCINT_NUM_INTS, //TZ notify
                0, //TZ Done
                {0, NULL} //array of Security clocks
        },
        {
                AsicPowerDomain_Ocmem, //Id
                "", //Name
                AsicRsc_None, //Type
                AdsppmClk_None, //Clock ID
                DALIPCINT_NUM_INTS, //TZ notify
                0, //TZ Done
                {0, NULL} //array of Security clocks
        }
};

AsicPwrDescriptorArrayType pwrDomains_845 = {ADSPPM_ARRAY(pwrDomains_array_845)};

/**
 * Table of DDR compensation factors to be applied at different BW ranges
 * This is used to compensate for bus latencies/cache misses
 * Latency/Snoc floor vote also updated
 */
const AsicCompensatedDdrBwTableEntryType compensatedDdrBw_array_845[] = {
        //        bwThreshold,            bwVote,   latencyVote,  snocFloorVote,    honestFlag
        {            10000000,         153000000,            0,               0,           0 },
        {            60000000,         399000000,            0,               0,           0 },
        {            96000000,                -1,          600,               0,           1 },
        {           120000000,                -1,          450,               0,           1 },
        {           155000000,                -1,          450,               0,           1 },
        {           230000000,                -1,          300,               0,           1 },
        {           250000000,                -1,          300,               0,           1 },
        {          ULLONG_MAX,                -1,          300,               0,           1 }
};

AsicCompensatedDdrBwTableType compensatedDdrBwTable_845 = {ADSPPM_ARRAY(compensatedDdrBw_array_845)};

//todo: correct values from audio arch team
const AsicSnocVoteTableEntryType adspsnocBwVote_array_845[] = {
        //        bwThreshold,      SnocAb,    SnocIb
        {            10000000,         0,           0   },
        {            21000000,         0,           0 },
        {            60000000,         0,           0 },
        {           160000000,         0,           0 },
        {           960000000,         0,           0 },
        {          ULLONG_MAX,         0,           0 }
};



AsicSnocVoteTableType adspsnocVoteTable_845 = {ADSPPM_ARRAY(adspsnocBwVote_array_845)};




const AsicCompensatedAhbBwTableEntryType compensatedAhbBw_array_845[] = {
    // bwThreshold,            bwVote
    {      9784305,          61440000 }, // target SVS2  19.2 Mhz
    {     19568611,         122880000 }, // target SVS2  38.4 Mhz
    {     39137222,         245760000 }, // target SVS   76.8 Mhz
    {    490240000,         490240000 }, // target NOM  153.2 Mhz
    {   ULLONG_MAX,                -1 },
};

AsicCompensatedAhbBwTableType compensatedAhbBwTable_845 = {ADSPPM_ARRAY(compensatedAhbBw_array_845)};

const AsicThreadLoadingInfoType threadLoadingData_array_845[] =
{
        { 90,   10,      0,     0 },    // oneDominantThreadLoading
        { 65,   25,     10,     0 },    // twoDominantThreadLoading
        { 50,   30,     20,     0 }     // threeDominantThreadLoading
};

AsicThreadLoadingInfoTableType threadLoadingData_845 = {ADSPPM_ARRAY(threadLoadingData_array_845)};

const uint32 audioVoiceCppFactors_array_845[] =
{
        3500,    // one thread active CPP   * 1000
        2500,    // two thread active CPP   * 1000
        1700,    // three thread active CPP * 1000
           0     // four thread active CPP  * 1000
};

AsicAudioVoiceCppTrendFactorsType audioVoiceCppFactors_845 = {
        ADSPPM_ARRAY(audioVoiceCppFactors_array_845)};


const AsicCppFactorsTableRowType audioCppFactors_array_845[] =
{
    {
        70,         // mppsThreshold
        {
            {
                3500,    // one   thread active CPP * 1000
                2500,    // two   thread active CPP * 1000
                1700,    // three thread active CPP * 1000
            }
        }
    },
    {
        ULONG_MAX,   // mppsThreshold
        {
            {
                3300,    // one   thread active CPP * 1000
                2250,    // two   thread active CPP * 1000
                1530,    // three thread active CPP * 1000
            }
        }
    },
};


AsicCppFactorsTableType audioCppFactors_845 = {
        ADSPPM_ARRAY(audioCppFactors_array_845)};

const AsicCppFactorsTableRowType voiceCppFactors_array_845[] =
{
    {
        ULONG_MAX,   // mppsThreshold
        {
            {
              3100,    // one   thread active CPP * 1000
              2000,    // two   thread active CPP * 1000
              1700,    // three thread active CPP * 1000
            }
        }
    },
};

AsicCppFactorsTableType voiceCppFactors_845 = {
        ADSPPM_ARRAY(voiceCppFactors_array_845)};

// Table for cache partition configuration.
// Audio and voice classes are grouped together for the purposes
// of determining class concurrency.
// Client class combinations not present in the table will default
// to a shared cache (no partitioning).
const AsicCachePartitionConfigType cachePartitionConfig_array_845[] = {

    // aggregateClass,                mainPartSize

    { MMPM_AUDIO_CLIENT_CLASS |
      MMPM_COMPUTE_CLIENT_CLASS,      HALF_SIZE }, // 2 way AV/comp

    { MMPM_VOICE_CLIENT_CLASS |
      MMPM_COMPUTE_CLIENT_CLASS,      HALF_SIZE }, // 2 way AV/comp

    { MMPM_AUDIO_CLIENT_CLASS |
      MMPM_VOICE_CLIENT_CLASS |
      MMPM_COMPUTE_CLIENT_CLASS,      HALF_SIZE }, // 2 way AV/comp

};

AsicCachePartitionConfigTableType cachePartitionConfigTable_845 =
    { ADSPPM_ARRAY(cachePartitionConfig_array_845) };

/**
Table for system cache configuration mapping
SC driver takes usecaseid as input to provide SCID
First column represents Client class; Second priority ID and third corrosponding usecaseId
*/

const AsicSystemCacheConfigType systemcacheConfig_array_845[] = {

    // clientClass,                   Priority,         UsecaseId

    { MMPM_VOICE_CLIENT_CLASS,      (1 << MMPM_SYS_CACHE_VOICE_HIGHER),      SC_USE_CASE_ID_VOICE }, 

    { MMPM_AUDIO_CLIENT_CLASS,      (1 << MMPM_SYS_CACHE_AUDIO_HIGHER),      SC_USE_CASE_ID_AUDIO },
    
    { MMPM_VOICE_CLIENT_CLASS,      (1 << MMPM_SYS_CACHE_VOICE_DEFAULT),     SC_USE_CASE_ID_AUDIO },

    { MMPM_AUDIO_CLIENT_CLASS,      (1 << MMPM_SYS_CACHE_AUDIO_DEFAULT),     SC_USE_CASE_ID_AUDIO },
};

AsicAdspSystemCacheConfigTableType systemcacheConfigTable_845 =
    { ADSPPM_ARRAY(systemcacheConfig_array_845) };

const AsicBwConcurrencySettingsType bwConcurrencySettings_845 =
{

    3,          /* adspDdrConcurrencyVotersThreshold*/
    15000000,   /* adspDdrCompensatedBWThreshold*/
    {
        256,    /* adspDdrConcurrencyFactor = 1*/
        384,    /* adspDdrConcurrencyFactor = 1.5*/
        512,    /* adspDdrConcurrencyFactor = 2*/
    }
};

const AsicAdspToAhbeFreqTableEntry adspToAhbeFreq_array_845v1[] = {
    // adspFreqHz,      abheFreqHz
    {   288000000,       25600000 }, // MinSVS f-max
    {   460800000,       51200000 }, // lowSVS f-max
    {   614400000,       102400000 }, // svs f-max
    {   787200000,       102400000 }, // svs_L1 f-max
    {   940800000,      204800000 }, // nominal ADSP f-max
    {   1190400000,      204800000 }, // turbo
 }; 


AsicAdspToAhbeFreqTableType adspToAhbeFreqTable_845v1 = 
    { ADSPPM_ARRAY(adspToAhbeFreq_array_845v1) };

//Table to lookup MPPS threshold values for different client classes
//Class Name, Threshold value
const AsicAdspCacheSizeMPPSThresholdTableEntry adspcachesizeMPPSThreshold_array_845[] = {
    {   MMPM_AUDIO_CLIENT_CLASS,  100  },

};

AsicAdspCacheSizeMPPSThresholdTableType  adspCacheSizeMPPSThreshold_845 = {ADSPPM_ARRAY(adspcachesizeMPPSThreshold_array_845)};

const AsicAdspCacheSizeBWScalingTableEntry adspcachesizebwscaling_array_845[] = {
    {   FULL_SIZE,             67  },
    {   THREE_QUARTER_SIZE,   100  },
    {   HALF_SIZE,            100  },    
};

AsicAdspCacheSizeBWScalingTableType  adspcachesizebwscaling_845 = {ADSPPM_ARRAY(adspcachesizebwscaling_array_845)};
	
const AsicFeatureDescType features_845 [AsicFeatureId_enum_max] =
{
        //AsicFeatureId_Adsp_Clock_Scaling
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
        //AsicFeatureId_Ahb_Scaling
        {
                AsicFeatureState_Enabled,
                76800000, //min bytes per second corresponds to 19Mhz clock
                614400000, //max bytes per second; Update in asic_test_info.h also-MAX_AHB_FREQ
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
                AsicFeatureState_Enabled,
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
        // AsicFeatureId_AhbeScaling
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
        // Since compute client class is removed from ADSP, not needed
        // AsicFeatureId_CacheSizeBWScaling
        {
                AsicFeatureState_Disabled,
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
};

