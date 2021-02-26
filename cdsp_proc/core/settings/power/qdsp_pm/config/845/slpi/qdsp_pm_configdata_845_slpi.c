/*
* Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: adsppm_configdata_845.c
@brief: Contains all 845-specific data definitions for Config Manager.

$Header: //components/rel/core.qdsp6/2.1.c4/settings/power/qdsp_pm/config/845/slpi/qdsp_pm_configdata_845_slpi.c#1 $
*/

#include "adsppm_settings.h"
#include "asic_settings.h"
#include "asic_internal_settings.h"
#include "adsppm_utils_settings.h"
#include "icbid.h"
#include "DDIIPCInt.h"
#include "qdsp_pm_clocks_hwio_845_slpi.h"
#include "msmhwiobase.h"
#include "limits.h"
#include "qdsp_pm_configdata_arch_845_slpi.h"

AsicHwioRegRangeType lpassRegRange_845 = {LPASS_BASE, LPASS_BASE_PHYS, LPASS_BASE_SIZE};

// L2 configuration registers base address = TCM_BASE + 1.5 Meg + 128K
// Reference: Fig 8-3 in document 80-V9418-22 Rev C
#define TCM_BASE 0x17400000
#define L2_CONFIG_BASE (TCM_BASE + 0x00180000 + 0x00020000)
AsicHwioRegRangeType l2ConfigRegRange_845 = {L2_CONFIG_BASE, L2_CONFIG_BASE, 0x1004 /* max expected offset needed by ADPSPM */};

/**
 * Array of 845 Master Bus Ports per core. Arranged by to core ID
 */
const AdsppmBusPortIdType masterPorts_SLPI_845[]     = {AdsppmBusPort_Q6DSP_Master};

const AdsppmBusPortIdType masterPorts_SLPI_SDC_845[] = {AdsppmBusPort_Sdc_Master};

const AdsppmBusPortIdType masterPorts_SLPI_QUP_845[] = {AdsppmBusPort_Qup_Master};

const AdsppmBusPortIdType slavePorts_SLPI_845[]    = {AdsppmBusPort_Q6DSP_Slave};

const AdsppmBusPortIdType slavePorts_SLPI_SRAM_845[]    = {AdsppmBusPort_Slpi_Sram_Slave};

const AdsppmClkIdType coreClocks_SLPI_SDC_845[] = {AdsppmClk_Sdc_Core};

//const AdsppmClkIdType coreClocks_SLPI_QUP_845[] = {AdsppmClk_Qup0_Core, AdsppmClk_Qup1_Core,AdsppmClk_Qup2_Core, AdsppmClk_Qup3_Core, AdsppmClk_Qup4_Core, AdsppmClk_Qup5_Core};

const AdsppmClkIdType coreClocks_SLPI_QUP_845[] = {AdsppmClk_Qup_Core};

const AdsppmClkIdType coreClocks_SLPI_SRAM_845[] = {AdsppmClk_Sram_Core};

const AdsppmClkIdType coreClocks_SLPI_CCD_845[] = {AdsppmClk_Ccd_Core};


/**
 * Array of 845 Slave Bus Ports per core. Arranged by to core ID
 */




/**
 * Array of 845 core descriptors. Arranged according to core ID enum
 */
const AsicCoreDescType cores_array_845[] =
{
        {   //Adsppm_Core_Id_SLPI Q6
                Adsppm_Core_Id_Q6_DSP, //Core ID
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
                {ADSPPM_ARRAY(masterPorts_SLPI_845)}, //Master port instances
                {ADSPPM_ARRAY(slavePorts_SLPI_845)}, //Slave port instances
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
                {ADSPPM_ARRAY(coreClocks_SLPI_SDC_845)}, //Core clock instances
                {ADSPPM_ARRAY(masterPorts_SLPI_SDC_845)}, //Master port instances
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
                {ADSPPM_ARRAY(coreClocks_SLPI_QUP_845)}, //Core clock instances
                {ADSPPM_ARRAY(masterPorts_SLPI_QUP_845)}, //Master port instances
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
                {ADSPPM_ARRAY(coreClocks_SLPI_CCD_845)}, //Core clock instances
                {0, NULL}, //Master port instances
                {0, NULL}, //Slave port instances
                Adsppm_Instance_Id_0 //Number of core instances
        },

        {   //Adsppm_Core_Id_LPM
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
                {ADSPPM_ARRAY(coreClocks_SLPI_SRAM_845)}, //Core clock instances
                {0, NULL}, //Master port instances
                {ADSPPM_ARRAY(slavePorts_SLPI_SRAM_845)}, //Slave port instances
                Adsppm_Instance_Id_0 //Number of core instances
        },
};

AsicCoreDescriptorArrayType cores_845 = {ADSPPM_ARRAY(cores_array_845)};

const AsicMemDescriptorType memories_array_845[] =
{
        {   //Adsppm_Mem_None
                Adsppm_Mem_None, //Mem ID
                AsicPowerDomain_AON //Power Domain
        },
};

AsicMemDescriptorArrayType memories_845 = {ADSPPM_ARRAY(memories_array_845)};

/**
 * Array of 845 core clock descriptors. Arranged by core clock ID
 */
const AsicClkDescriptorType clocks_array_845[AdsppmClk_EnumMax] =
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
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlSW, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "scc_data_h_clk", //Name
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
                AdsppmClk_Sram_Hclk, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlSW, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "scc_smem_clk", //Name
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
        /* AON CLOCKS */
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
                AdsppmClk_Adsp_Hmclk, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlAlwaysON, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "scc_q6_ahbm_clk", //Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },

        {
                AdsppmClk_Adsp_Hsclk, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlAlwaysON, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "scc_q6_ahbs_clk", //Name
                AdsppmClk_AhbE_Hclk, //Source
                Adsppm_Mem_None //Memory ID
        }

};

AsicClockDescriptorArrayType clocks_845 = {ADSPPM_ARRAY(clocks_array_845)};

/**
 * Arrays of 845 register programming clocks
 */
// const AdsppmClkIdType regProgClocks_Dml_8996[]      = {AdsppmClk_Dml_Hclk};
// const AdsppmClkIdType regProgClocks_Aif_8996[]      = {AdsppmClk_Aif_Csr_Hclk};
// const AdsppmClkIdType regProgClocks_Slimbus_8996[]  = {AdsppmClk_Slimbus_Hclk};
// const AdsppmClkIdType regProgClocks_Slimbus2_8996[] = {AdsppmClk_Slimbus2_Hclk};
// const AdsppmClkIdType regProgClocks_Midi_8996[]     = {AdsppmClk_Midi_Hclk};
// const AdsppmClkIdType regProgClocks_HwRsmp_8996[]   = {AdsppmClk_HwRsp_Hclk};
// const AdsppmClkIdType regProgClocks_AvSync_8996[]   = {AdsppmClk_AvSync_Hclk};
// const AdsppmClkIdType regProgClocks_Lpm_8996[]      = {AdsppmClk_Lpm_Hclk};
const AdsppmClkIdType regProgClocks_Sram_845[]     = {AdsppmClk_Sram_Hclk};

const AdsppmClkIdType regProgClocks_Sdc_845[]     = {AdsppmClk_Sdc_Hclk};

const AdsppmClkIdType regProgClocks_Qup_845[]     = {AdsppmClk_Qup_Hclk};
//const AdsppmClkIdType regProgClocks_Spdif_8996[]    = {AdsppmClk_Spdif_Hsclk};
//const AdsppmClkIdType regProgClocks_Hdmirx_8996[]   = {AdsppmClk_Hdmirx_Hclk};
//const AdsppmClkIdType regProgClocks_Sif_8996[]      = {AdsppmClk_Sif_Hclk};
//const AdsppmClkIdType regProgClocks_Bstc_8996[]     = {AdsppmClk_Bstc_Hclk};
//const AdsppmClkIdType regProgClocks_Dcodec_8996[]   = {AdsppmClk_Dcodec_Hclk};
//const AdsppmClkIdType regProgClocks_Adsp_8996[]     = {AdsppmClk_Adsp_Hclk};

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
                AdsppmBusPort_Q6DSP_Master, //ID
                AsicBusPort_AhbE_M| AsicBusPort_Ext_M,  //Connection
                AdsppmClk_Adsp_Hmclk, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbMaster = ICBID_MASTER_SENSORS_PROC}, //icbarb ID
                AdsppmBusPort_Q6DSP_Master //Access port
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
                AdsppmBusPort_Ext_Ahb_Master, //ID
                AsicBusPort_Ext_M, //Connection
                AdsppmClk_AhbE_Hclk, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbMaster = ICBID_MASTER_SENSORS_AHB}, //icbarb ID
                AdsppmBusPort_Ext_Ahb_Master //Access port
        },

        {
                AdsppmBusPort_Ddr_Slave, //ID
                AsicBusPort_Ext_S, //Connection
                AdsppmClk_None, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_EBI1}, //icbarb ID
                AdsppmBusPort_None //Access port
        },

        {
                AdsppmBusPort_Slpi_Sram_Slave, //ID
                AsicBusPort_AhbE_S, //Connection
                AdsppmClk_Sram_Hclk, //Bus clock
                {ADSPPM_ARRAY(regProgClocks_Sram_845)}, //Array of reg prog clocks
                {.icbarbSlave =  ICBID_SLAVE_COUNT}, //icbarb ID
                AdsppmBusPort_Slpi_Sram_Slave //Access port
        },

        {
                AdsppmBusPort_Core, //ID
                0, //Connection
                AdsppmClk_None, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_LPASS}, //icbarb ID
                AdsppmBusPort_None //Access port
        },

        /* AHB-S clock is needed for intial setup but we don't know when to use it yet */
        {
                AdsppmBusPort_Q6DSP_Slave, //ID
                AsicBusPort_AhbE_S, //Connection
                AdsppmClk_Adsp_Hsclk, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_LPASS}, //icbarb ID
                AdsppmBusPort_None //Access port
        }
};

AsicBusPortDescriptorArrayType busPorts_845 = {ADSPPM_ARRAY(busPorts_array_845)};

/**
 * List of supported external bus routes
 */
const AdsppmBusRouteType extBusRoutes_array_845[] =
{
        {
                AdsppmBusPort_Q6DSP_Master, //master
                AdsppmBusPort_Ddr_Slave //slave
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
                AdsppmBusPort_Q6DSP_Master,
                AdsppmBusPort_Ddr_Slave
        }
};

AsicBusRouteArrayType mipsBwRoutes_845 = {ADSPPM_ARRAY(mipsBwRoutes_array_845)};



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
};

AsicPwrDescriptorArrayType pwrDomains_845 = {ADSPPM_ARRAY(pwrDomains_array_845)};




const AsicFeatureDescType features_845 [AsicFeatureId_enum_max] =
{
        //AsicFeatureId_Adsp_Clock_Scaling
        {
                AsicFeatureState_Enabled,
#ifdef ADSPPM_TEST
                19, //min MHz
#else
                200, //min MHz
#endif
                800, //max MHz
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
                AsicFeatureState_Disabled,
                76000000, //min bytes per second corresponds to 19Mhz clock
                400400000, //max bytes per second
        },
        //AsicFeatureId_Ahb_Sw_CG
        {
                AsicFeatureState_Enabled,
                0, //min
                0, //max
        },
        //AsicFeatureId_Ahb_DCG
        {
                AsicFeatureState_Disabled,
                0, //min
                0, //max
        },
        //AsicFeatureId_LpassCore_PC
        {
                AsicFeatureState_Disabled,
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
                100000000, //max Hz
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
                AsicFeatureState_Enabled,
                200, //min MIPS for periodic clients
                800  //max MIPS value
        },
        //AsicFeatureId_TimelineOptimisationBw
        {
                AsicFeatureState_Enabled,
                76000000*8, //min ext BW in bytes per second (targeting above 76Mhz SNOC clk)
                0x7FFFFFFF, //max ext BW in bytes per second
        },
        //AsicFeatureId_TimelineOptimisationAhb
        {
                AsicFeatureState_Enabled,
                100000000,
                100000000, //max AHB clock in Hz
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
                AsicFeatureState_Disabled,
                0, // min (not used)
                0, // max (not used)
        },
        // AsicFeatureId_AhbeScaling
        {
                AsicFeatureState_Disabled,
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
                AsicFeatureState_Disabled,
                0, // min (not used)
                0, // max (not used)
        },
        // AsicFeatureId_SystemCacheSupport
        {
                AsicFeatureState_Disabled,
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
                AsicFeatureState_Enabled,
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
       //AsicFeatureId_Q6AhbIxVote
        {
                AsicFeatureState_Disabled,
                0, //min
                0, //max
        },
		//AsicFeatureId_AHBNocScaling
        {
                AsicFeatureState_Enabled,
                0, //min
                0, //max
        },
		//AsicFeatureId_LpassSlpiDSP
        {
                AsicFeatureState_Disabled,
                0, //min
                0, //max
        },
};

