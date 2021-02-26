/*
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: adsppm_settings.h
@brief: ADSPPM settings data structures
        This is the internal settings data for ADSPPM.
        This is not distributed to any clients that uses ADSPPM.

$Header: //components/rel/core.qdsp6/2.1/settings/power/qdsp_pm/interface/adsppm_settings.h#2 $
*/

#ifndef ADSPPM_SETTINGS_H_
#define ADSPPM_SETTINGS_H_


#define MAX_ADSPPM_CLIENT_NAME    64

/**
 * @enum AdsppmCoreIdType - ADSPPM CORE ID
 */
typedef enum
{
    Adsppm_Core_Id_None        = 100,    //!< None
    Adsppm_Core_Id_Q6_DSP      = 101,
    Adsppm_Core_Id_ADSP        = Adsppm_Core_Id_Q6_DSP,    //!< Q6SS HW inside of LPASS
    Adsppm_Core_Id_LPASS_Core  = 102,    //!< Generic ID for entire LPASS core
    Adsppm_Core_Id_LPM         = 103,    //!< Low Power Memory inside of LPASS
    Adsppm_Core_Id_DML         = 104,    //!< DML (Data Mover Lite) HW inside of LPASS
    Adsppm_Core_Id_AIF         = 105,    //!< AIF(Audio Interface) HW inside of LPASS
    Adsppm_Core_Id_SlimBus     = 106,    //!< SLIMBus HW inside of LPASS
    Adsppm_Core_Id_Midi        = 107,    //!< MIDI HW inside of LPASS
    Adsppm_Core_Id_AVsync      = 108,    //!< AVT/ProntoSync HW inside of LPASS
    Adsppm_Core_Id_HWRSMP      = 109,    //!< HW Resampler HW inside of LPASS
    Adsppm_Core_Id_SRam        = 110,   //!< Sensors RAM
    Adsppm_Core_Id_DCodec      = 111,   //!< Digital codec
    Adsppm_Core_Id_Spdif       = 112,   //!< SPDIF
    Adsppm_Core_Id_Hdmirx      = 113,   //!< HDMI RX
    Adsppm_Core_Id_Hdmitx      = 114,   //!< HDMI TX
    Adsppm_Core_Id_Sif         = 115,   //!< analog audio
    Adsppm_Core_Id_BSTC        = 116,   //!< BSTC
    Adsppm_Core_Id_HVX         = 117,   //!< HVX
    Adsppm_Core_Id_DPAudio     = 118,   //!< DP Audio
    Adsppm_Core_Id_Compute_HCP  = 119,   //!< Generic ID for entire VAPSS core
    Adsppm_Core_Id_Compute_DMA  = 120,   //!< Generic ID for entire VAPSS core
    Adsppm_Core_Id_Slpi_SDC     = 121,   /**< Slpi SDC block*/
    Adsppm_Core_Id_Slpi_QUP     = 122,   /**< Slpi Qup block*/
    Adsppm_Core_Id_Slpi_SRAM    = 123,   /**< Slpi Sram block*/
    Adsppm_Core_Id_Slpi_CCD     = 124,   /**< Slpi Ccd block*/
    Adsppm_Core_Id_Max,                //!< Maximum count
    Adsppm_Core_Id_Force8Bits  = 0x7F  //!< forcing enum to be 8 bit
} AdsppmCoreIdType;

/**
 * @enum AdsppmInstanceIdType - ADSPPM Instance ID
 */
typedef enum
{
    Adsppm_Instance_Id_None        = 0,    //!< None
    Adsppm_Instance_Id_0           = 1,
    Adsppm_Instance_Id_1           = 2,
    Adsppm_Instance_Id_2           = 3,
    Adsppm_Instance_Id_Max,
    Adsppm_Instance_Id_Force8Bits  = 0x7F  //!< forcing enum to be 8 bit
} AdsppmInstanceIdType;

/**
 * @enum AdsppmRscIdType - ADSPPM resource ID
 */
typedef enum
{
    Adsppm_Rsc_Id_None            = 0,    //!< None
	Adsppm_Rsc_Id_Start			  = 1,
    Adsppm_Rsc_Id_Core_Clk        = Adsppm_Rsc_Id_Start,
    Adsppm_Rsc_Id_Sleep_Latency   = 2,
    Adsppm_Rsc_Id_Mips            = 3,
    Adsppm_Rsc_Id_BW              = 4,
    Adsppm_Rsc_Id_Thermal         = 5,
    Adsppm_Rsc_Id_MemPower        = 6,
    Adsppm_Rsc_Id_Core_Clk_Domain = 7,
    Adsppm_Rsc_Id_Power           = 8,
    Adsppm_Rsc_Id_Max,                    //!< Maximum count
    Adsppm_Rsc_Id_Force8Bits      = 0x7F  //!< forcing enum to be 8 bit
} AdsppmRscIdType;


/******************************************************************************
 *                     Bus/Clock definitions
 *****************************************************************************/
/**
 * @enum AdsppmClkIdType - defines clock IDs as used by ADSPPM internally
 */
typedef enum
{
    AdsppmClk_None = 0,
    AdsppmClk_Adsp_Core,
//!< Core Bus Clocks Start
    AdsppmClk_Ahb_Root,
    AdsppmClk_AhbI_Hclk,
    AdsppmClk_AhbX_Hclk,
    AdsppmClk_HwRsp_Hclk,
    AdsppmClk_Dml_Hclk,
    AdsppmClk_Aif_Hclk,
    AdsppmClk_Aif_Csr_Hclk,
    AdsppmClk_Slimbus_Hclk,
    AdsppmClk_Slimbus_cbc,
    AdsppmClk_Slimbus2_Hclk,
    AdsppmClk_Slimbus2_cbc,
    AdsppmClk_Midi_Hclk,
    AdsppmClk_AvSync_Hclk,
    AdsppmClk_Atimer_Hclk,
    AdsppmClk_Lpm_Hclk,
    AdsppmClk_Lpm_cbc,
    AdsppmClk_Csr_Hclk,
    AdsppmClk_Dcodec_Hclk,
    AdsppmClk_Spdif_Hmclk,
    AdsppmClk_Spdif_Hsclk,
    AdsppmClk_Hdmirx_Hclk,
    AdsppmClk_Hdmitx_Hclk,
    AdsppmClk_Sif_Hclk,
    AdsppmClk_Bstc_Hclk,
    AdsppmClk_Smmu_Adsp_Hclk,
    AdsppmClk_Smmu_Lpass_Hclk,
    AdsppmClk_Sysnoc_Hclk,
    AdsppmClk_Sysnoc_cbc,
    AdsppmClk_Bus_Timeout_Hclk,
    AdsppmClk_Tlb_Preload_Hclk,
    AdsppmClk_Qos_Hclk,
    AdsppmClk_Qdsp_Sway_Hclk,
    AdsppmClk_VAPSS_AXI,
    AdsppmClk_VAPSS_TCMS,
    AdsppmClk_Sdc_Hclk,
    AdsppmClk_Qup_Hclk,
//!< Core Bus Clocks End
//!< AON Bus Clocks Start
    AdsppmClk_AhbE_Hclk,
    AdsppmClk_Adsp_Hmclk,
    AdsppmClk_Adsp_Hsclk,
    AdsppmClk_Sram_Hclk,
    AdsppmClk_Lcc_Hclk,
    AdsppmClk_Security_Hclk,
    AdsppmClk_Wrapper_Security_Hclk,
    AdsppmClk_Wrapper_Br_Hclk,
    AdsppmClk_Audio_Core_AON,
    AdsppmClk_Audio_Wrapper_AON,
    AdsppmClk_Turing_VAPSS_AHBS_Clk,
//!< AON Bus Clocks End
//!< Core Clocks Start
    AdsppmClk_HwRsp_Core,
    AdsppmClk_Midi_Core,
    AdsppmClk_AvSync_Xo,
    AdsppmClk_AvSync_Bt,
    AdsppmClk_AvSync_Fm,
    AdsppmClk_Slimbus_Core,
    AdsppmClk_Slimbus2_Core,
    AdsppmClk_Avtimer_core,
    AdsppmClk_Atime_core,
    AdsppmClk_Atime2_core,
    AdsppmClk_VAPSS_Core,
    AdsppmClk_Sdc_Core,
    AdsppmClk_Qup_Core,
    AdsppmClk_Sram_Core,
    AdsppmClk_Ccd_Core,
//!< Core Clocks End
    AdsppmClk_EnumMax,                     //!< Max number of the items in the enum
    AdsppmClk_EnumForce32Bit = 0x7fffffff  //!< AdsppmClk_EnumForce32Bit
} AdsppmClkIdType;

/******************************************************************************
 *                     Bus definitions
 *****************************************************************************/
/**
 * @enum AdsppmBusPortIdType - defines Bus ports to be used by ADSPPM internally
 */
typedef enum
{
    AdsppmBusPort_None = 0,                    //!< AdsppmBusMaster_None
    AdsppmBusPort_Q6DSP_Master,                //!< Q6DSP Master
    AdsppmBusPort_Adsp_Master = AdsppmBusPort_Q6DSP_Master,
    AdsppmBusPort_Dml_Master,                  //!< AdsppmBusMaster_Dml
    AdsppmBusPort_Aif_Master,                  //!< AdsppmBusMaster_Aif
    AdsppmBusPort_Slimbus_Master,              //!< AdsppmBusMaster_Slimbus
    AdsppmBusPort_Slimbus2_Master,             //!< AdsppmBusMaster_Slimbus
    AdsppmBusPort_Midi_Master,                 //!< AdsppmBusMaster_Midi
    AdsppmBusPort_HwRsmp_Master,               //!< AdsppmBusMaster_HwRsmp
    AdsppmBusPort_Ext_Ahb_Master,              //!< SNOC Master port from AHB-X
    AdsppmBusPort_Spdif_Master,                //!< SPDIF master port
    AdsppmBusPort_Hdmirx_Master,               //!< HDMI RX slave port
    AdsppmBusPort_Hdmitx_Master,               //!< HDMI TX slave port
    AdsppmBusPort_Sif_Master,                  //!< SIF slave port
    AdsppmBusPort_DPAudio_Master,              //!< DP Audio master
    /* Added for CDSPPM in SDM845 */
    AdsppmBusPort_Vapss_Hcp_Master,
    AdsppmBusPort_Vapss_Dma_Master,
    /******************************/
    AdsppmBusPort_Sdc_Master,
    AdsppmBusPort_Qup_Master,
    AdsppmBusPort_Dml_Slave,                   //!< AdsppmBusSlave_Dml
    AdsppmBusPort_Aif_Slave,                   //!< AdsppmBusSlave_Aif
    AdsppmBusPort_Slimbus_Slave,               //!< AdsppmBusSlave_Slimbus
    AdsppmBusPort_Slimbus2_Slave,              //!< AdsppmBusSlave_Slimbus
    AdsppmBusPort_Midi_Slave,                  //!< AdsppmBusSlave_Midi
    AdsppmBusPort_HwRsmp_Slave,                //!< AdsppmBusSlave_HwRsmp
    AdsppmBusPort_AvSync_Slave,                //!< AdsppmBusSlave_AvSync
    AdsppmBusPort_Lpm_Slave,                   //!< AdsppmBusSlave_Lpm
    AdsppmBusPort_Sram_Slave,                  //!< AdsppmBusSlave_Sram
    AdsppmBusPort_Ext_Ahb_Slave,               //!< SNOC Slave port from AHB-E
    AdsppmBusPort_Ddr_Slave,                   //!< AdsppmBusSlave_Ddr
    AdsppmBusPort_Ocmem_Slave,                 //!< AdsppmBusSlave_Ocmem
    AdsppmBusPort_PerifNoc_Slave,              //!< AdsppmBusSlave_PerifNoc
    AdsppmBusPort_Snoc_Slave,                  //!< AdsppmBusSlave_Snoc
    AdsppmBusPort_Spdif_Slave,                 //!< SPDIF slave port
    AdsppmBusPort_Hdmirx_Slave,                //!< HDMI RX slave port
    AdsppmBusPort_Hdmitx_Slave,                //!< HDMI TX slave port
    AdsppmBusPort_Sif_Slave,                   //!< SIF slave port
    AdsppmBusPort_Bstc_Slave,                  //!< BSTC slave port
    AdsppmBusPort_Dcodec_Slave,                //!< DCodec slave port
    AdsppmBusPort_DPAudio_Slave,               //!< DP Audio slave port
    /* Added for CDSPPM in SDM845 */
    AdsppmBusPort_Cdsp_TCM_Slave,
    /******************************/
    AdsppmBusPort_Slpi_Sram_Slave,
    AdsppmBusPort_Q6DSP_Slave,                 //!< ADSP core slave port for Reg Prog only. Not exposed to mmpm.h
    AdsppmBusPort_Adsp_Slave = AdsppmBusPort_Q6DSP_Slave,
    AdsppmBusPort_Core,                        //!< Core current client is registered for
	AdsppmBusPort_Snoc_Master,                 //!Since no client cares about it; for Q6
    AdsppmBusPort_Snoc_Ahb_Master,             //!Since no client cares about it; for Ahb Master
    AdsppmBusPort_EnumMax,                     //!< AdsppmBusSlave_EnumMax
    AdsppmBusPort_EnumForce32Bit = 0x7fffffff  //!< AdsppmBusSlave_EnumForce32Bit
} AdsppmBusPortIdType;

/**
 * @struct AdsppmBusRouteType - Definition for the bus route type (master/slave pair)
 */
typedef struct
{
    AdsppmBusPortIdType masterPort;
    AdsppmBusPortIdType slavePort;
} AdsppmBusRouteType;


/**
 * @enum AdsppmMemIdType - Memory Id definition for Memory Power state requests
 */
typedef enum
{
    Adsppm_Mem_None,                     //!< None
    Adsppm_Mem_Ocmem,                    //!< OCMEM
    Adsppm_Mem_Lpass_LPM,                //!< LPM
    Adsppm_Mem_Sram,                     //!< Sensors RAM
    Adsppm_Mem_Max,                      //!< Maximum count
    Adsppm_MEM_FORCE32BITS = 0x7FFFFFFF  //!< forcing enum to be 32 bit
} AdsppmMemIdType;


#endif /* ADSPPM_SETTINGS_H_ */

