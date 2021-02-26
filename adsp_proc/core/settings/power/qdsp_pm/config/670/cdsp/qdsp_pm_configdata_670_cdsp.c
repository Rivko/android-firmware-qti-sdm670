/*
* Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: adsppm_configdata_670.c
@brief: Contains all 670-specific data definitions for Config Manager.

$Header: //components/rel/core.qdsp6/2.1/settings/power/qdsp_pm/config/670/cdsp/qdsp_pm_configdata_670_cdsp.c#6 $
*/

#include "adsppm_settings.h"
#include "asic_settings.h"
#include "asic_internal_settings.h"
#include "adsppm_utils_settings.h"
#include "icbid.h"
#include "DDIIPCInt.h"
#include "qdsp_pm_clocks_hwio_670_cdsp.h"
#include "msmhwiobase.h"
#include "limits.h"
#include "systemcache/systemcache.h"
#include "qdsp_pm_configdata_arch_670_cdsp.h"

AsicHwioRegRangeType lpassRegRange_670 = {LPASS_BASE, LPASS_BASE_PHYS, LPASS_BASE_SIZE};

// L2 configuration registers base address = TCM_BASE + 1.5 Meg + 128K
// Reference: Fig 8-3 in document 80-V9418-22 Rev C
#define TCM_BASE 0x08400000
#define L2_CONFIG_BASE (TCM_BASE + 0x00180000 + 0x00020000)
AsicHwioRegRangeType l2ConfigRegRange_670 = {L2_CONFIG_BASE, L2_CONFIG_BASE, 0x1004 /* max expected offset needed by ADPSPM */};

/**
 * Array of 670 Master Bus Ports per core. Arranged by to core ID
 */
const AdsppmBusPortIdType masterPorts_ADSP_670[]    = {AdsppmBusPort_Adsp_Master};
const AdsppmBusPortIdType masterPorts_CDSP_670[]    = {AdsppmBusPort_Q6DSP_Master};
const AdsppmBusPortIdType masterPorts_HCP_670[]    = {AdsppmBusPort_Vapss_Hcp_Master};
const AdsppmBusPortIdType masterPorts_DMA_670[]    = {AdsppmBusPort_Vapss_Dma_Master};


/**
 * Array of 670 Slave Bus Ports per core. Arranged by to core ID
 */
const AdsppmBusPortIdType slavePorts_CDSP_670[]    = {AdsppmBusPort_Cdsp_TCM_Slave};

/**
 * Array of 670 core descriptors. Arranged according to core ID enum
 */
const AsicCoreDescType cores_array_670[] =
{
        {   //Adsppm_Core_Id_CDSP Q6
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
                {ADSPPM_ARRAY(masterPorts_CDSP_670)}, //Master port instances
                {0, NULL}, //Slave port instances
                Adsppm_Instance_Id_0 //Number of core instances
        },
        {   //Adsppm_Core_Id_Compute_HCP
                Adsppm_Core_Id_Compute_HCP, //Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_None, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_Latency, //Sleep Latency
                        AsicRsc_MIPS_Clk | AsicRsc_MIPS_BW, //MIPS
                        AsicRsc_BW_Internal | AsicRsc_BW_External, //BW
                        AsicRsc_Thermal //Thermal
                }, //HwResource IDs
                AsicPowerDomain_ComputeCore, //Power Domain
                {0, NULL}, //Core clock instances
                {ADSPPM_ARRAY(masterPorts_HCP_670)}, //Master port instances
                {0, NULL}, //Slave port instances
                Adsppm_Instance_Id_0 //Number of core instances
        },
        {   //Adsppm_Core_Id_Compute_DMA
                Adsppm_Core_Id_Compute_DMA, //Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_None, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_Latency, //Sleep Latency
                        AsicRsc_MIPS_Clk | AsicRsc_MIPS_BW, //MIPS
                        AsicRsc_BW_Internal | AsicRsc_BW_External, //BW
                        AsicRsc_Thermal //Thermal
                }, //HwResource IDs
                AsicPowerDomain_ComputeCore, //Power Domain
                {0, NULL}, //Core clock instances
                {ADSPPM_ARRAY(masterPorts_DMA_670)}, //Master port instances
                {0, NULL}, //Slave port instances
                Adsppm_Instance_Id_0 //Number of core instances
        },
        {   //Adsppm_Core_Id_HVX
                Adsppm_Core_Id_HVX, //Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_None, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_None, //Sleep Latency
                        AsicRsc_None, //MIPS
                        AsicRsc_None, //BW
                        AsicRsc_None //Thermal
                }, //HwResource IDs
                AsicPowerDomain_Hvx, //Power Domain
                {0, NULL}, //Core clock instances
                {0, NULL}, //Master port instances
                {0, NULL}, //Slave port instances
                Adsppm_Instance_Id_0 //Number of core instances
        },
};

AsicCoreDescriptorArrayType cores_670 = {ADSPPM_ARRAY(cores_array_670)};

const AsicMemDescriptorType memories_array_670[] =
{
        {   //Adsppm_Mem_None
                Adsppm_Mem_None, //Mem ID
                AsicPowerDomain_AON //Power Domain
        },
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

        //This clock is mapped to the same clock with TCMS clock. It's enabled when internal
        //bandwidth request is issued, normally from DMA --> L2.
        {
                AdsppmClk_AhbI_Hclk, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlAlwaysON, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "turing_q6ss_axis2_clk", //Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },

        {
                //A different name for clock VAPSS_AXI
                AdsppmClk_AhbX_Hclk, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlSW, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "turing_vapss_axi_clk", //Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },

        {
                AdsppmClk_VAPSS_TCMS, //Clk ID
                AsicClk_TypeDalFreqSet, //Clk Type
                AsicClk_CntlSW, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "turing_vapss_vap_tcms_clk", //Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },

        /* AON CLOCKS */
        {
                AdsppmClk_AhbE_Hclk, //Clk ID
                AsicClk_TypeDalFreqSet, //Clk Type
                AsicClk_CntlAlwaysON, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "turing_turing_wrapper_aon_clk", //Name
                AdsppmClk_None, //Source
                Adsppm_Mem_None //Memory ID
        },

        {
                AdsppmClk_Adsp_Hmclk, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlAlwaysON, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "turing_q6ss_ahbm_aon_clk", //Name
                AdsppmClk_AhbE_Hclk, //Source
                Adsppm_Mem_None //Memory ID
        },

        {
                AdsppmClk_Adsp_Hsclk, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlAlwaysON, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "turing_q6ss_ahbs_aon_clk", //Name
                AdsppmClk_AhbE_Hclk, //Source
                Adsppm_Mem_None //Memory ID
        },

        {
                AdsppmClk_Turing_VAPSS_AHBS_Clk, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlAlwaysON, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "turing_vapss_ahbs_aon_clk", //Name
                AdsppmClk_AhbE_Hclk, //Source
                Adsppm_Mem_None //Memory ID
        },


        /* CORE CLOCKS */
        {
                AdsppmClk_VAPSS_Core,   //Clk ID
                AsicClk_TypeDalFreqSet, //Clk Type
                AsicClk_CntlSW, //Cntl Type
                AsicClk_MemCntlNone, //MemCtrl Type
                .clkInfo.clkName = "turing_vapss_vap_core_clk", //Name
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
                AsicBusPort_AhbE_M | AsicBusPort_Ext_M,  //Connection
                AdsppmClk_Adsp_Hmclk, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbMaster = ICBID_MASTER_CDSP_PROC}, //icbarb ID
                AdsppmBusPort_Q6DSP_Master //Access port
        },

        {
                AdsppmBusPort_Vapss_Hcp_Master, //ID
                AsicBusPort_AhbX_M,  //Connection
                AdsppmClk_None, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbMaster = ICBID_MASTER_CDSP_HCP}, //icbarb ID
                AdsppmBusPort_Vapss_Hcp_Master //Access port
        },

        {
                AdsppmBusPort_Vapss_Dma_Master, //ID
                AsicBusPort_AhbX_M | AsicBusPort_AhbI_M,  //Connection
                AdsppmClk_None, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbMaster = ICBID_MASTER_CDSP_DMA}, //icbarb ID
                AdsppmBusPort_Vapss_Dma_Master //Access port
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
                AdsppmBusPort_Cdsp_TCM_Slave, //ID
                AsicBusPort_AhbI_S, //Connection
                AdsppmClk_AhbI_Hclk, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_CDSP_TCM}, //icbarb ID
                AdsppmBusPort_None //Access port
        },

        /* AHB-S clock is needed for intial setup but we don't know when to use it yet */
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
                AdsppmBusPort_Q6DSP_Master, //master
                AdsppmBusPort_Ddr_Slave //slave
        },
        {
                AdsppmBusPort_Vapss_Dma_Master, //master
                AdsppmBusPort_Ddr_Slave //slave
        },
};

AsicBusRouteArrayType extBusRoutes_670 = {ADSPPM_ARRAY(extBusRoutes_array_670)};

const AdsppmBusRouteType mipsBwRoutes_array_670[] =
{
        {
                AdsppmBusPort_Q6DSP_Master,
                AdsppmBusPort_Ddr_Slave
        }
};

AsicBusRouteArrayType mipsBwRoutes_670 = {ADSPPM_ARRAY(mipsBwRoutes_array_670)};



/**
 * Array of power domain descriptors
 */
const AsicPowerDomainDescriptorType pwrDomains_array_670[] =
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
                AsicPowerDomain_Hvx, //Id
                "VCS", //Name
                AsicRsc_Power_Hvx, //Type
                AdsppmClk_None, //Clock ID
                DALIPCINT_NUM_INTS, //TZ notify
                0, //TZ Done
                {0, NULL} //array of Security clocks
        },
        {
                AsicPowerDomain_ComputeCore, //Id
                "vapss_gdsc", //Name
                AsicRsc_Power_Core, //Type
                AdsppmClk_None, //Clock ID
                DALIPCINT_NUM_INTS, //TZ notify
                0, //TZ Done
                {0, NULL} //array of Security clocks
        }
};

AsicPwrDescriptorArrayType pwrDomains_670 = {ADSPPM_ARRAY(pwrDomains_array_670)};



const AsicFeatureDescType features_670 [AsicFeatureId_enum_max] =
{
        //AsicFeatureId_Adsp_Clock_Scaling
        {
                AsicFeatureState_Enabled,
#ifdef ADSPPM_TEST
                19, //min MHz
#else
                80, //min MHz
#endif
                1190,//max MHz
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
        //AsicFeatureId_AhbIX_Scaling
        {
                AsicFeatureState_Enabled,
                76000000, //min bytes per second corresponds to 19Mhz clock
                1636000000, //max bytes per second
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
                409600000, //max Hz
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
        // AsicFeatureId_AHBE_Q6Scaling
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
                AsicFeatureState_Disabled, //AsicFeatureState_Enabled,
                0, // min (not used)
                0, // max (not used)
        },
        //AsicFeatureId_ComputeDSP
        {
                AsicFeatureState_Enabled,
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
                0, //max
        },
		//AsicFeatureId_AhbE_VoteScaling
        {
                AsicFeatureState_Disabled,
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
