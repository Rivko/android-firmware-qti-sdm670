/*==============================================================================
$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/clock_manager/src/AudioIfClkDriver.cpp#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $
$Change: 14795016 $
$Revision: #1 $

FILE:     AFEAudioIfDrver.cpp

DESCRIPTION: Main Interface to the AFE Audio Interface driver

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Copyright 2013 -2015 QUALCOMM Technologies, Inc. (QTI)
All Rights Reserved.
QUALCOMM Proprietary/GTDR
==============================================================================*/
/*============================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. Please
use ISO format for dates.

$Header: //components/rel/avs.adsp/2.8.5/afe/drivers/afe/clock_manager/src/AudioIfClkDriver.cpp#1 $ $DateTime: 2017/11/07 23:00:25 $ $Author: pwbldsvc $

when        who  what, where, why
----------  ---  ----------------------------------------------------------
01-18-2012  mspk  Initial Draft


============================================================================*/

/*=====================================================================
 Includes
 ======================================================================*/
#include "AudioIfClkDriver.h"
#include "adsp_afe_service_commands.h"
#include "DDIClock.h"
#include "DALStdDef.h"
#include "DALSys.h"
#include "DALDeviceId.h"
#include "ClockDefs.h"
#include "qurt_elite.h"
#include "hwd_mmpm.h"
#include "AFEInterface.h"
#include "lpasshwio_devcfg.h"

/* ============================================================================
**                  Global Object Definitions
** ========================================================================= */
#define LPAIFCLK_MAX_NUM_CLKS_TO_CFG   2
#define LPAIFCLK_I2S_INTERFACE         0
#define LPAIFCLK_PCM_INTERFACE         1

#define LPAIFCLK_PCMOE_CLK_IN_12288000_HZ        12288000
#define LPAIFCLK_PCMOE_CLK_IN_61440000_HZ        61440000


typedef enum
{
   AUDIOIF_CLOCK_DEINIT = 0,
   AUDIOIF_CLOCK_INIT
} audioif_clock_state_t;


typedef enum AFEDevAudIfTypeInfo
{
   AUDIO_IF_TYPE_INVALID = -1,
   AUDIO_IF_PRIMARY_MI2S = 0,    //primary MI2S
   AUDIO_IF_PRIMARY_PCM,         //PCM
   AUDIO_IF_SECONDARY_PCM,       //2nd PCM
   AUDIO_IF_SECONDARY_MI2S,      //Secondary MI2S
   AUDIO_IF_TERTIARY_MI2S,       //Tertiary MI2S
   AUDIO_IF_QUATERNARY_MI2S,     //Quaternary MI2S
   AUDIO_IF_SPEAKER_I2S,         // Speaker I2S
   AUDIO_IF_HDMI,                //HDMI
   AUDIO_IF_DIGITAL_MIC,         //Digital Mic
   AUDIO_IF_SIF,                 // SIF interface
   AUDIO_IF_TERTIARY_PCM,        //3rd PCM
   AUDIO_IF_QUATERNARY_PCM,      //4th PCM
   AUDIO_IF_QUINARY_MI2S,        //Quinary  MI2S (5th )
   AUDIO_IF_SENARY_MI2S,         //Senary MI2S  (6th)

} AFEDevAudIfType;


/**
* The following type must be used whenever a source/sink are to be represented.
*/
typedef enum AFEDevAudIfDirInfo
{
   AUDIO_IF_DIR_INVALID = -1,
   AUDIO_IF_SINK = 0 ,
   AUDIO_IF_SOURCE
} AFEDevAudIfDirType;


/**
 * The following are the possible lpaif clk types
 */
typedef enum _AfeDevLpaifClkType
{
   AFEDevLpaifClkTypeInvalid = 0,
   AFEDevLpaifClkTypeI2sBit,
   AFEDevLpaifClkTypeI2sOsr,
   AFEDevLpaifClkTypePcmBit,
   AFEDevLpaifClkTypePcmOe,
   AFEDevLpaifClkTypeDigitalCodecCore,
} AfeDevLpaifClkType;

/**
 * The following are the possible lpaif clk root source
 */
typedef enum _AFEDevLpaifClkRoot
{
   AFEDevLpaifClkRootInvalid = 0,

   //Clock root for I2S and PCM bus, use AFEDevLpaifClkRootExternal for MSM slave mode and AFEDevLpaifClkRootPxo for master mode
   AFEDevLpaifClkRootExternal = 0x1,
   //Start from 0x1001 = Public API defines + 0x1000
   AFEDevLpaifClkRootDefault = 0x1000,                           //#define AFE_PORT_LPACLK_CLK_ROOT_DEFAULT                                     0x0
   AFEDevLpaifClkRootPxo = 0x1001,                               //#define AFE_PORT_LPACLK_CLK_ROOT_PXO                                         0x1
   AFEDevLpaifClkRootCxo = 0x1002,                               //#define AFE_PORT_LPACLK_CLK_ROOT_CXO                                         0x2
   AFEDevLpaifClkRootLpapll = 0x1003,                            //#define AFE_PORT_LPACLK_CLK_ROOT_LPAPLL                                      0x3
   AFEDevLpaifClkRootSecpll = 0x1004,                            //#define AFE_PORT_LPACLK_CLK_ROOT_SECPLL                                      0x4
   AFEDevLpaifClkRootLpaq6pll = 0x1005,                          //#define AFE_PORT_LPACLK_CLK_ROOT_LPAQ6PLL                                    0x5


   //Clock root for digital codec core clk. Start from 0x2001 = Public API defines + 0x2000
   AFEDevLpaifClkRootDigitalCodecCoreRootStart = 0x2000,
   AFEDevLpaifClkRootDigitalCodecCorePriMi2sOsr = 0x2001,        //#define AFE_PORT_LPACLK_CLK_ROOT_DIGITAL_CODEC_CORECLK_PRII2SOSR                0x1
   AFEDevLpaifClkRootDigitalCodecCoreSecMi2sOsr = 0x2002,        //#define AFE_PORT_LPACLK_CLK_ROOT_DIGITAL_CODEC_CORECLK_SECI2SOSR                0x2
   AFEDevLpaifClkRootDigitalCodecCoreTerMi2sOsr = 0x2003,        //#define AFE_PORT_LPACLK_CLK_ROOT_DIGITAL_CODEC_CORECLK_TERI2SOSR                0x3
   AFEDevLpaifClkRootDigitalCodecCoreQuaMi2sOsr = 0x2004,        //#define AFE_PORT_LPACLK_CLK_ROOT_DIGITAL_CODEC_CORECLK_QUAI2SOSR                0x4
   AFEDevLpaifClkRootDigitalCodecCoreXo = 0x2005,                //#define AFE_PORT_LPACLK_CLK_ROOT_DIGITAL_CODEC_CORECLK_XO                       0x5

   AFEDevLpaifClkRootMax = 0x7FFFFFFF
} AFEDevLpaifClkRootType;

/** \brief AFE Device lpaif clk configure parameter  */
typedef struct _AFEDeviceLpaifClkCfgStruct
{
   /** which interface */
   AFEDevAudIfType inf;
   /** which direction */
   AFEDevAudIfDirType dir;
   /** clk set value in Hz */
   uint32 clkValue;         //for example, this is bit clk
   /** clk set value in Hz */
   uint32 clkSrcValue;      //for example, this is osr clk. Note usually bit clk = osr clk / 8
   /** clk root */
   AFEDevLpaifClkRootType clkRoot;
   /** clk set mode */
   AfeDevLpaifClkType clkSetType;
}AFEDeviceLpaifClkCfgStruct;

/** \brief AFE DAL IOCtl property call, for audio IF clk set */
typedef struct _AFEIoctlPropLpaifClkCfgStruct
{
   AFEDeviceLpaifClkCfgStruct *pLpaifClkCfgArray;
   int32 numOfClks;
}AFEIoctlPropLpaifClkCfgType;

/** \brief AFE DAL IOCtl property call, for reconfiguring i2s clock based on sampling rate */
typedef struct _AFEIoctlPropReconfigI2SClkStruct
{
   /** which interface */
   AFEDevAudIfType inf;
   /** which direction */
   AFEDevAudIfDirType dir;
   /** new sample rate */
   int32         newSampleRate;
   /** orignal  sample rate */
   int32         orgSampleRate;
}AFEIoctlPropReconfigI2SClkType;


/*
* The clk set function define
*/
typedef DALResult(*lpaifClkSetFunc)
   (
    DalDeviceHandle *pHandle,
    void *pClkCfg
    );

/**
* LpaifClkCfgStruct
*/
typedef struct _LpaifClkCfgStruct
{
   ClockIdType             clkRegId;          //get from clock regime
   uint32                  lpaifClkIdx;       // Just a seq number to ease the debugging
   AFEDevAudIfType         audioIfType;
   AFEDevAudIfDirType      audioIfDir;
   AfeDevLpaifClkType      clkSetType;
   AFEDevLpaifClkRootType  clkRoot;
   uint32                  clkFreqInHz;
   uint32                  clkSrcFreqInHz;
   uint32                  clkEnableCnt;
   uint32                  ClkStatus;
   lpaifClkSetFunc         clkSetEnableFunc;
   lpaifClkSetFunc         clkSetDisableFunc;
} LpaifClkCfgStruct;


/**
* LpaifClkCfgAdspmInfo
*/
typedef struct _LpaifClkCfgAdspmInfo
{
   uint32 clientId;
   uint32 powerVoteCnt;
} LpaifClkCfgAdspmInfo;


/**
* DigCodecClkCfgAdspmInfo
*/
typedef struct _DigCodecClkCfgAdspmInfo
{
   uint32 clientId;
   uint32 powerVoteCnt;
} DigCodecClkCfgAdspmInfo;


/**
* LpaifClkCfgGlobalInfo
*/
typedef struct _LpaifClkCfgGlobalInfo
{
   audioif_clock_state_t state;
   uint32_t              hw_version;
   uint32                  numClkCfgNode;
   DalDeviceHandle         *pDalDevHandle;
   LpaifClkCfgStruct       *pLpaifClkCfgNode;
   LpaifClkCfgAdspmInfo    adsppmInfo;
   DigCodecClkCfgAdspmInfo dig_codec_adsppm_info;
} LpaifClkCfgGlobalInfo;

/* 
* LpaifClkMapTableStruct
*/
typedef struct _LpaifClkMapTableStruct
{
   const char                 clkName[128];
   AFEDevAudIfType            audioIf;       //
   AFEDevAudIfDirType         audioIfDir;    //if the clk is shared between Tx and Rx, set as AUDIO_IF_DIR_INVALID now
   AFEDevLpaifClkRootType     clkRoot;
   AfeDevLpaifClkType         clkSetType;
   lpaifClkSetFunc            clkSetEnableFunc;
   lpaifClkSetFunc            clkSetDisableFunc;
} LpaifClkMapTableStruct;

//Full set function - enable for I2S bit clks, include
//1. DalClock_SetClockFrequency
//2. DalClock_SetClockDivider
//3. DalClock_EnableClock

static DALResult afeDalClkSetEnable1(DalDeviceHandle *pHandle, void *pClkCfg);

//Part set function - enable for all external clks, include
//1. DalClock_EnableClock
static DALResult afeDalClkSetEnable3(DalDeviceHandle *pHandle, void *pClkCfg);

//Part set function - enable for I2S osr and PCM clks, include
//1. DalClock_SetClockFrequency
//2. DalClock_EnableClock
static DALResult afeDalClkSetEnable2(DalDeviceHandle *pHandle, void *pClkCfg);

//Part set function disable for all clks, include
//1. DalClock_DisableClock
static DALResult afeDalClkSetDisable1(DalDeviceHandle *pHandle, void *pClkCfg);

// reconfig for I2S bit clks, include
//1. DalClock_SetClockFrequency
//2. DalClock_SetClockDivider
static DALResult afeDalReconfigI2SBitClk(DalDeviceHandle *pHandle, void *pClkCfg);

// Internal support function to locate the core clk CfgNode from the array
static LpaifClkCfgStruct* afeDalLocateDigitalCodecCoreClkCfgNode(LpaifClkCfgGlobalInfo *pLpaIfClkCfgGlobalCtxt);

/* clkregime clk string mapping */
// (audioIfType + clockRoot + clkSetType) should uniquely identify an entry

const LpaifClkMapTableStruct gLpaifMapTable[] =
{    // clk name -  clock Regime defined          //AFEDevAudIfType          //Rx or Tx             //AFEDevLpaifClkRootPxo       //AfeDevLpaifClkType      //clkSetEnableFunc           //clkSetDisableFunc
     //use primary one if       //not used now         //  - MSM Master
   { "audio_core_lpaif_codec_spkr_ibit_clk",    AUDIO_IF_SPEAKER_I2S,      AUDIO_IF_SINK,         AFEDevLpaifClkRootPxo,       AFEDevLpaifClkTypeI2sBit,  afeDalClkSetEnable1,  afeDalClkSetDisable1 },
   { "audio_core_lpaif_codec_spkr_ebit_clk",    AUDIO_IF_SPEAKER_I2S,      AUDIO_IF_SINK,         AFEDevLpaifClkRootExternal,  AFEDevLpaifClkTypeI2sBit,  afeDalClkSetEnable3,  afeDalClkSetDisable1 },
   { "audio_core_lpaif_codec_spkr_osr_clk",     AUDIO_IF_SPEAKER_I2S,      AUDIO_IF_SINK,         AFEDevLpaifClkRootPxo,       AFEDevLpaifClkTypeI2sOsr,  afeDalClkSetEnable2,  afeDalClkSetDisable1 },
   { "audio_core_lpaif_pri_ibit_clk",           AUDIO_IF_PRIMARY_MI2S,     AUDIO_IF_SINK,         AFEDevLpaifClkRootPxo,       AFEDevLpaifClkTypeI2sBit,  afeDalClkSetEnable1,  afeDalClkSetDisable1 },
   { "audio_core_lpaif_pri_ebit_clk",           AUDIO_IF_PRIMARY_MI2S,     AUDIO_IF_SINK,         AFEDevLpaifClkRootExternal,  AFEDevLpaifClkTypeI2sBit,  afeDalClkSetEnable3,  afeDalClkSetDisable1 },
   { "audio_core_lpaif_pri_osr_clk",            AUDIO_IF_PRIMARY_MI2S,     AUDIO_IF_SINK,         AFEDevLpaifClkRootPxo,       AFEDevLpaifClkTypeI2sOsr,  afeDalClkSetEnable2,  afeDalClkSetDisable1 },
   { "audio_core_lpaif_sec_ibit_clk",           AUDIO_IF_SECONDARY_MI2S,   AUDIO_IF_SINK,         AFEDevLpaifClkRootPxo,       AFEDevLpaifClkTypeI2sBit,  afeDalClkSetEnable1,  afeDalClkSetDisable1 },
   { "audio_core_lpaif_sec_ebit_clk",           AUDIO_IF_SECONDARY_MI2S,   AUDIO_IF_SINK,         AFEDevLpaifClkRootExternal,  AFEDevLpaifClkTypeI2sBit,  afeDalClkSetEnable3,  afeDalClkSetDisable1 },
   { "audio_core_lpaif_sec_osr_clk",            AUDIO_IF_SECONDARY_MI2S,   AUDIO_IF_SINK,         AFEDevLpaifClkRootPxo,       AFEDevLpaifClkTypeI2sOsr,  afeDalClkSetEnable2,  afeDalClkSetDisable1 },
   { "audio_core_lpaif_ter_ibit_clk",           AUDIO_IF_TERTIARY_MI2S,    AUDIO_IF_SINK,         AFEDevLpaifClkRootPxo,       AFEDevLpaifClkTypeI2sBit,  afeDalClkSetEnable1,  afeDalClkSetDisable1 },
   { "audio_core_lpaif_ter_ebit_clk",           AUDIO_IF_TERTIARY_MI2S,    AUDIO_IF_SINK,         AFEDevLpaifClkRootExternal,  AFEDevLpaifClkTypeI2sBit,  afeDalClkSetEnable3,  afeDalClkSetDisable1 },
   { "audio_core_lpaif_ter_osr_clk",            AUDIO_IF_TERTIARY_MI2S,    AUDIO_IF_SINK,         AFEDevLpaifClkRootPxo,       AFEDevLpaifClkTypeI2sOsr,  afeDalClkSetEnable2,  afeDalClkSetDisable1 },
   { "audio_core_lpaif_quad_ibit_clk",          AUDIO_IF_QUATERNARY_MI2S,  AUDIO_IF_SINK,         AFEDevLpaifClkRootPxo,       AFEDevLpaifClkTypeI2sBit,  afeDalClkSetEnable1,  afeDalClkSetDisable1 },
   { "audio_core_lpaif_quad_ebit_clk",          AUDIO_IF_QUATERNARY_MI2S,  AUDIO_IF_SINK,         AFEDevLpaifClkRootExternal,  AFEDevLpaifClkTypeI2sBit,  afeDalClkSetEnable3,  afeDalClkSetDisable1 },
   { "audio_core_lpaif_quad_osr_clk",           AUDIO_IF_QUATERNARY_MI2S,  AUDIO_IF_SINK,         AFEDevLpaifClkRootPxo,       AFEDevLpaifClkTypeI2sOsr,  afeDalClkSetEnable2,  afeDalClkSetDisable1 },
   { "audio_core_lpaif_pcm0_ibit_clk",          AUDIO_IF_PRIMARY_PCM,      AUDIO_IF_SINK,         AFEDevLpaifClkRootPxo,       AFEDevLpaifClkTypePcmBit,  afeDalClkSetEnable2,  afeDalClkSetDisable1 },
   { "audio_core_lpaif_pcm0_ebit_clk",          AUDIO_IF_PRIMARY_PCM,      AUDIO_IF_SINK,         AFEDevLpaifClkRootExternal,  AFEDevLpaifClkTypePcmBit,  afeDalClkSetEnable3,  afeDalClkSetDisable1 },
   { "audio_core_lpaif_pcm1_ibit_clk",          AUDIO_IF_SECONDARY_PCM,    AUDIO_IF_SINK,         AFEDevLpaifClkRootPxo,       AFEDevLpaifClkTypePcmBit,  afeDalClkSetEnable2,  afeDalClkSetDisable1 },
   { "audio_core_lpaif_pcm1_ebit_clk",          AUDIO_IF_SECONDARY_PCM,    AUDIO_IF_SINK,         AFEDevLpaifClkRootExternal,  AFEDevLpaifClkTypePcmBit,  afeDalClkSetEnable3,  afeDalClkSetDisable1 },
   { "audio_core_lpaif_pcmoe_clk",              AUDIO_IF_PRIMARY_PCM,      AUDIO_IF_SINK,         AFEDevLpaifClkRootPxo,       AFEDevLpaifClkTypePcmOe,   afeDalClkSetEnable2,  afeDalClkSetDisable1 },
   { "digcodec_clk",                            AUDIO_IF_PRIMARY_MI2S,     AUDIO_IF_SINK,         AFEDevLpaifClkRootPxo,       AFEDevLpaifClkTypeDigitalCodecCore, afeDalClkSetEnable2,  afeDalClkSetDisable1 }
};


// (audioIfType + clockRoot + clkSetType) should uniquely identify an entry
const LpaifClkMapTableStruct gLpaifMapTable_v2[] =
{
   // Primiary and secondary mi2s
   { "gcc_ultaudio_lpaif_pri_i2s_clk",       AUDIO_IF_PRIMARY_MI2S,      AUDIO_IF_SINK,      AFEDevLpaifClkRootPxo,    AFEDevLpaifClkTypeI2sBit,    afeDalClkSetEnable2,  afeDalClkSetDisable1 },
   { "gcc_ultaudio_lpaif_sec_i2s_clk",       AUDIO_IF_SECONDARY_MI2S,    AUDIO_IF_SINK,      AFEDevLpaifClkRootPxo,    AFEDevLpaifClkTypeI2sBit,    afeDalClkSetEnable2,  afeDalClkSetDisable1 },

   // Primiary and secondary pcm, clks are the same as above
   { "gcc_ultaudio_lpaif_pri_i2s_clk",       AUDIO_IF_PRIMARY_PCM,       AUDIO_IF_SINK,      AFEDevLpaifClkRootPxo,    AFEDevLpaifClkTypePcmBit,    afeDalClkSetEnable2,  afeDalClkSetDisable1 },
   { "gcc_ultaudio_lpaif_sec_i2s_clk",       AUDIO_IF_SECONDARY_PCM,     AUDIO_IF_SINK,      AFEDevLpaifClkRootPxo,    AFEDevLpaifClkTypePcmBit,    afeDalClkSetEnable2,  afeDalClkSetDisable1 },

   // mclk output control
   { "gcc_ultaudio_lpaif_ext_i2s_clk",       AUDIO_IF_PRIMARY_MI2S,      AUDIO_IF_SINK,      AFEDevLpaifClkRootPxo,    AFEDevLpaifClkTypeI2sOsr,    afeDalClkSetEnable2,  afeDalClkSetDisable1 },
   { "gcc_ultaudio_lpaif_ext_i2s_clk",       AUDIO_IF_SECONDARY_MI2S,    AUDIO_IF_SINK,      AFEDevLpaifClkRootPxo,    AFEDevLpaifClkTypeI2sOsr,    afeDalClkSetEnable2,  afeDalClkSetDisable1 },

   // Primary and secondary PCM OE
   { "gcc_ultaudio_lpaif_pcm_dataoe_clk",    AUDIO_IF_PRIMARY_PCM,       AUDIO_IF_SINK,      AFEDevLpaifClkRootPxo,    AFEDevLpaifClkTypePcmOe,     afeDalClkSetEnable2,  afeDalClkSetDisable1 },
   { "gcc_ultaudio_lpaif_aux_pcm_dataoe_clk", AUDIO_IF_SECONDARY_PCM,     AUDIO_IF_SINK,      AFEDevLpaifClkRootPxo,    AFEDevLpaifClkTypePcmOe,     afeDalClkSetEnable2,  afeDalClkSetDisable1 },

};

/* LpaifClkId to clkregime clk string mapping */
const LpaifClkMapTableStruct gLpaifMapTable_v3[] =
{    //clk name -  clock Regime defined          //AFEDevAudIfType          //Rx or Tx             //AFEDevLpaifClkRootPxo       //AfeDevLpaifClkType      //clkSetEnableFunc           //clkSetDisableFunc
     //use primary one if       //not used now         //  - MSM Master
     //clk is shared among                             //AFEDevLpaifClkRootExternal
     //interfaces                                      //  - MSM Slave

   //Primary Mi2s
   { "audio_core_lpaif_pri_ibit_clk",           AUDIO_IF_PRIMARY_MI2S,     AUDIO_IF_SINK,         AFEDevLpaifClkRootPxo,       AFEDevLpaifClkTypeI2sBit,  afeDalClkSetEnable1,  afeDalClkSetDisable1 },
   { "audio_core_lpaif_pri_ebit_clk",           AUDIO_IF_PRIMARY_MI2S,     AUDIO_IF_SINK,         AFEDevLpaifClkRootExternal,  AFEDevLpaifClkTypeI2sBit,  afeDalClkSetEnable3,  afeDalClkSetDisable1 },
   { "audio_wrapper_mclk0_clk",                 AUDIO_IF_PRIMARY_MI2S,     AUDIO_IF_SINK,         AFEDevLpaifClkRootPxo,       AFEDevLpaifClkTypeI2sOsr,  afeDalClkSetEnable2,  afeDalClkSetDisable1 },

   //Secondary Mi2s  sharing same clocks of Pi2s
   { "audio_core_lpaif_pri_ibit_clk",           AUDIO_IF_SECONDARY_MI2S,   AUDIO_IF_SINK,         AFEDevLpaifClkRootPxo,       AFEDevLpaifClkTypeI2sBit,  afeDalClkSetEnable1,  afeDalClkSetDisable1 },
   { "audio_core_lpaif_pri_ebit_clk",           AUDIO_IF_SECONDARY_MI2S,   AUDIO_IF_SINK,         AFEDevLpaifClkRootExternal,  AFEDevLpaifClkTypeI2sBit,  afeDalClkSetEnable3,  afeDalClkSetDisable1 },
   { "audio_wrapper_mclk1_clk",                 AUDIO_IF_SECONDARY_MI2S,   AUDIO_IF_SINK,         AFEDevLpaifClkRootPxo,       AFEDevLpaifClkTypeI2sOsr,  afeDalClkSetEnable2,  afeDalClkSetDisable1 },

   //Tertiary Mi2s
   { "audio_core_lpaif_ter_ibit_clk",           AUDIO_IF_TERTIARY_MI2S,    AUDIO_IF_SINK,         AFEDevLpaifClkRootPxo,       AFEDevLpaifClkTypeI2sBit,  afeDalClkSetEnable1,  afeDalClkSetDisable1 },
   { "audio_core_lpaif_ter_ebit_clk",           AUDIO_IF_TERTIARY_MI2S,    AUDIO_IF_SINK,         AFEDevLpaifClkRootExternal,  AFEDevLpaifClkTypeI2sBit,  afeDalClkSetEnable3,  afeDalClkSetDisable1 },
   { "audio_core_lpaif_ter_osr_clk",            AUDIO_IF_TERTIARY_MI2S,    AUDIO_IF_SINK,         AFEDevLpaifClkRootPxo,       AFEDevLpaifClkTypeI2sOsr,  afeDalClkSetEnable2,  afeDalClkSetDisable1 },

   //Quat Mi2s
   { "audio_core_lpaif_quad_ibit_clk",          AUDIO_IF_QUATERNARY_MI2S,  AUDIO_IF_SINK,         AFEDevLpaifClkRootPxo,       AFEDevLpaifClkTypeI2sBit,  afeDalClkSetEnable1,  afeDalClkSetDisable1 },
   { "audio_core_lpaif_quad_ebit_clk",          AUDIO_IF_QUATERNARY_MI2S,  AUDIO_IF_SINK,         AFEDevLpaifClkRootExternal,  AFEDevLpaifClkTypeI2sBit,  afeDalClkSetEnable3,  afeDalClkSetDisable1 },
   { "audio_core_lpaif_quad_osr_clk",           AUDIO_IF_QUATERNARY_MI2S,  AUDIO_IF_SINK,         AFEDevLpaifClkRootPxo,       AFEDevLpaifClkTypeI2sOsr,  afeDalClkSetEnable2,  afeDalClkSetDisable1 },

   //Qui Mi2s
   { "audio_core_lpaif_qui_ibit_clk",           AUDIO_IF_QUINARY_MI2S,      AUDIO_IF_SINK,         AFEDevLpaifClkRootPxo,       AFEDevLpaifClkTypeI2sBit,  afeDalClkSetEnable1,  afeDalClkSetDisable1 },
   { "audio_core_lpaif_qui_ebit_clk",           AUDIO_IF_QUINARY_MI2S,      AUDIO_IF_SINK,         AFEDevLpaifClkRootExternal,  AFEDevLpaifClkTypeI2sBit,  afeDalClkSetEnable3,  afeDalClkSetDisable1 },
   { "audio_core_lpaif_qui_osr_clk",            AUDIO_IF_QUINARY_MI2S,      AUDIO_IF_SINK,         AFEDevLpaifClkRootPxo,       AFEDevLpaifClkTypeI2sOsr,  afeDalClkSetEnable2,  afeDalClkSetDisable1 },

   //Sen Mi2S
   { "audio_core_lpaif_sen_ibit_clk",           AUDIO_IF_SENARY_MI2S,       AUDIO_IF_SINK,         AFEDevLpaifClkRootPxo,       AFEDevLpaifClkTypeI2sBit,  afeDalClkSetEnable1,  afeDalClkSetDisable1 },
   { "audio_core_lpaif_sen_ebit_clk",           AUDIO_IF_SENARY_MI2S,       AUDIO_IF_SINK,         AFEDevLpaifClkRootExternal,  AFEDevLpaifClkTypeI2sBit,  afeDalClkSetEnable3,  afeDalClkSetDisable1 },
   { "audio_core_lpaif_sen_osr_clk",            AUDIO_IF_SENARY_MI2S,       AUDIO_IF_SINK,         AFEDevLpaifClkRootPxo,       AFEDevLpaifClkTypeI2sOsr,  afeDalClkSetEnable2,  afeDalClkSetDisable1 },

   //PRimary PCm sharing same clocks of QUAT MI2S
   { "audio_core_lpaif_quad_ibit_clk",          AUDIO_IF_PRIMARY_PCM,       AUDIO_IF_SINK,         AFEDevLpaifClkRootPxo,       AFEDevLpaifClkTypePcmBit,  afeDalClkSetEnable2,  afeDalClkSetDisable1 },
   { "audio_core_lpaif_quad_ebit_clk",          AUDIO_IF_PRIMARY_PCM,       AUDIO_IF_SINK,         AFEDevLpaifClkRootExternal,  AFEDevLpaifClkTypePcmBit,  afeDalClkSetEnable3,  afeDalClkSetDisable1 },

   //Secondary PCM sharing same clocks of QUI MI2S
   { "audio_core_lpaif_qui_ibit_clk",          AUDIO_IF_SECONDARY_PCM,      AUDIO_IF_SINK,         AFEDevLpaifClkRootPxo,       AFEDevLpaifClkTypePcmBit,  afeDalClkSetEnable2,  afeDalClkSetDisable1 },
   { "audio_core_lpaif_qui_ebit_clk",          AUDIO_IF_SECONDARY_PCM,      AUDIO_IF_SINK,         AFEDevLpaifClkRootExternal,  AFEDevLpaifClkTypePcmBit,  afeDalClkSetEnable3,  afeDalClkSetDisable1 },


   { "audio_core_lpaif_pcm_data_oe_clk",      AUDIO_IF_PRIMARY_PCM,        AUDIO_IF_SINK,         AFEDevLpaifClkRootPxo,       AFEDevLpaifClkTypePcmOe,   afeDalClkSetEnable2,  afeDalClkSetDisable1 },
   { "digcodec_clk",                           AUDIO_IF_PRIMARY_MI2S,       AUDIO_IF_SINK,         AFEDevLpaifClkRootPxo,       AFEDevLpaifClkTypeDigitalCodecCore, afeDalClkSetEnable2,  afeDalClkSetDisable1 }
};


LpaifClkCfgGlobalInfo AfeDevLpaifClkCfgGlobalCtx;
AFEDevLpaifClkRootType root_clk_type_internal; 


/* ============================================================================
**                  Local Function Definitions
** ========================================================================= */

static ADSPResult lpaifSetThisClks(AFEDeviceLpaifClkCfgStruct *pLpaifClkCfg);

static ADSPResult lpaifSetThisClks_sub(LpaifClkCfgStruct *pLpaifClkCfgNode);

static AFEDevAudIfType afe_audio_dev_if_type_get(uint16_t port_id)
{
   AFEDevAudIfType intf = AUDIO_IF_TYPE_INVALID;

   switch(port_id)
   {
      case AFE_PORT_ID_PRIMARY_MI2S_RX:
      {
         intf = AUDIO_IF_PRIMARY_MI2S;
      }
         break;
      case AFE_PORT_ID_PRIMARY_MI2S_TX:
      {
         intf = AUDIO_IF_PRIMARY_MI2S;
      }
         break;
      case AFE_PORT_ID_SECONDARY_MI2S_RX:
      {
         intf = AUDIO_IF_SECONDARY_MI2S;
      }
         break;
      case AFE_PORT_ID_SECONDARY_MI2S_TX:
      {
         intf = AUDIO_IF_SECONDARY_MI2S;
      }
         break;
      case AFE_PORT_ID_TERTIARY_MI2S_RX:
      {
         intf = AUDIO_IF_TERTIARY_MI2S;
      }
         break;
      case AFE_PORT_ID_TERTIARY_MI2S_TX:
      {
         intf = AUDIO_IF_TERTIARY_MI2S;
      }
         break;
      case AFE_PORT_ID_QUATERNARY_MI2S_RX:
      {
         intf = AUDIO_IF_QUATERNARY_MI2S;
      }
         break;
      case AFE_PORT_ID_QUATERNARY_MI2S_TX:
      {
         intf = AUDIO_IF_QUATERNARY_MI2S;
      }
         break;
      case AUDIO_PORT_ID_I2S_RX:
      {
         intf = AUDIO_IF_SPEAKER_I2S;
      }
         break;
      case AFE_PORT_ID_PRIMARY_PCM_RX:
      {
         intf = AUDIO_IF_PRIMARY_PCM;
      }
         break;
      case AFE_PORT_ID_PRIMARY_PCM_TX:
      {
         intf = AUDIO_IF_PRIMARY_PCM;
      }
         break;
      case AFE_PORT_ID_SECONDARY_PCM_RX:
      {
         intf = AUDIO_IF_SECONDARY_PCM;
      }
         break;
      case AFE_PORT_ID_SECONDARY_PCM_TX:
      {
         intf = AUDIO_IF_SECONDARY_PCM;
      }
         break;
      case AFE_PORT_ID_TERTIARY_PCM_RX:
      {
         intf = AUDIO_IF_TERTIARY_PCM;
      }
         break;
      case AFE_PORT_ID_TERTIARY_PCM_TX:
      {
         intf = AUDIO_IF_TERTIARY_PCM;
      }
         break;

      case AFE_PORT_ID_QUATERNARY_PCM_RX:
      {
         intf = AUDIO_IF_QUATERNARY_PCM;
      }
         break;
      case AFE_PORT_ID_QUATERNARY_PCM_TX:
      {
         intf = AUDIO_IF_QUATERNARY_PCM;
      }
         break;

      default:
      {
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "Audio Port ID is unused and skipped = 0x%x", port_id);
      }
   }

   return intf;
}



ADSPResult afe_audioif_clk_init(uint32_t hw_version)
{
   ADSPResult res = DAL_ERROR;
   LpaifClkCfgGlobalInfo *pLpaIfClkCfgGlobalCtxt = &AfeDevLpaifClkCfgGlobalCtx;
   ClockIdType clkRegimeId;
   uint32 i;
   uint32 num_total_clk_entries;
   const LpaifClkMapTableStruct *pgLpaifMapTable;

   memset((void *)pLpaIfClkCfgGlobalCtxt, 0, sizeof(LpaifClkCfgGlobalInfo));

   pLpaIfClkCfgGlobalCtxt->hw_version = hw_version;

   if(0 == (pLpaIfClkCfgGlobalCtxt->adsppmInfo.clientId = HwdMmpm_Register(MMPM_CORE_ID_LPASS_AIF, MMPM_CORE_INSTANCE_0)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE DAL LPAIF Clock Init: HwdMmpm_Register failed for LPASS_AIF, mmpm_core_id: %d", MMPM_CORE_ID_LPASS_AIF);
      return res;
   }


   if(0 == (pLpaIfClkCfgGlobalCtxt->dig_codec_adsppm_info.clientId = HwdMmpm_Register(MMPM_CORE_ID_LPASS_DCODEC, MMPM_CORE_INSTANCE_0)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE DAL LPAIF Clock Init: HwdMmpm_Register failed for LPASS_DCODEC, mmpm_core_id: %d", MMPM_CORE_ID_LPASS_DCODEC);
      return res;
   }


#ifndef SIM
   if(DAL_SUCCESS == (res = DAL_DeviceAttach(DALDEVICEID_CLOCK, (DalDeviceHandle **)&pLpaIfClkCfgGlobalCtxt->pDalDevHandle)))
#else
   if(1)
#endif
      {

         MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "AFE DAL LPAIF Clock Init: hw_version 0x%x", hw_version);

         //clock table for 9x45
         if(LPASS_HW_VER_3_3_1 == hw_version)
         {
            // This is the 9x45 clk table
            num_total_clk_entries = sizeof(gLpaifMapTable_v2) / sizeof(LpaifClkMapTableStruct);
            pgLpaifMapTable = gLpaifMapTable_v2;
            root_clk_type_internal =  AFEDevLpaifClkRootPxo;
         }
         else if(LPASS_HW_VER_3_10_0 == hw_version)
         {
            // This is the 8952 clk table
            num_total_clk_entries = sizeof(gLpaifMapTable_v3) / sizeof(LpaifClkMapTableStruct);
            pgLpaifMapTable = gLpaifMapTable_v3;
            root_clk_type_internal =  AFEDevLpaifClkRootPxo;
         }
         else
         {
            // This is the default Lpaif clk table with clk name
            num_total_clk_entries = sizeof(gLpaifMapTable) / sizeof(LpaifClkMapTableStruct);
            pgLpaifMapTable = gLpaifMapTable;
            root_clk_type_internal =  AFEDevLpaifClkRootPxo;
         }

         // Create the lpaifClkCfgNode[] dynamically for use
         if(NULL == (pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode = (LpaifClkCfgStruct *)qurt_elite_memory_malloc(num_total_clk_entries * sizeof(LpaifClkCfgStruct), QURT_ELITE_HEAP_DEFAULT)))
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE DAL LPAIF Clock Init: failed to allocate lpaifClkCfgNode");
            return res;
         }
         else
         {
            pLpaIfClkCfgGlobalCtxt->numClkCfgNode = num_total_clk_entries;
            memset((void *)pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode, 0, num_total_clk_entries * sizeof(LpaifClkCfgStruct));
         }


         /* Get the clock regime Id */
         for(i = 0; i < pLpaIfClkCfgGlobalCtxt->numClkCfgNode; i++)
         {
#ifndef SIM
            res = DalClock_GetClockId(pLpaIfClkCfgGlobalCtxt->pDalDevHandle, (const char *)&pgLpaifMapTable[i].clkName, &clkRegimeId);
#else
            clkRegimeId = (ClockIdType)i;
            res = DAL_SUCCESS;
#endif
            if(DAL_SUCCESS == res)
            {

               pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[i].lpaifClkIdx = (uint32)i;
               pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[i].clkRegId = clkRegimeId;
               pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[i].audioIfType = pgLpaifMapTable[i].audioIf;
               pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[i].audioIfDir = pgLpaifMapTable[i].audioIfDir;
               pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[i].clkRoot = pgLpaifMapTable[i].clkRoot;
               pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[i].clkSetType = pgLpaifMapTable[i].clkSetType;
               pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[i].clkSetEnableFunc = pgLpaifMapTable[i].clkSetEnableFunc;
               pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[i].clkSetDisableFunc = pgLpaifMapTable[i].clkSetDisableFunc;
            }
            else
            {
               MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "AFE DAL: DalClock_GetClockId not supported for this clk: Idx: %ld, res: 0x%x\n", i, res);
            }
         }

         res = DAL_SUCCESS;

         pLpaIfClkCfgGlobalCtxt->state = AUDIOIF_CLOCK_INIT;
      }
      else
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE DAL: DAL_DeviceAttach fail res: 0x%x", res);
      }

   return res;
}


ADSPResult afe_audioif_clk_deinit(void)
{
   ADSPResult res = DAL_ERROR;
   LpaifClkCfgGlobalInfo *pLpaIfClkCfgGlobalCtxt = &AfeDevLpaifClkCfgGlobalCtx;

   if(AUDIOIF_CLOCK_INIT == pLpaIfClkCfgGlobalCtxt->state)
   {

      //Deregister ADSPPM for LPAIF core
      if(DAL_SUCCESS != (res = HwdMmpm_Deregister(pLpaIfClkCfgGlobalCtxt->adsppmInfo.clientId)))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE DAL: HwdMmpm_Deregister failed for LPASS_AIF core");
      }

      //Deregister ADSPPM for Digcodec core
      if(DAL_SUCCESS != (res = HwdMmpm_Deregister(pLpaIfClkCfgGlobalCtxt->dig_codec_adsppm_info.clientId)))
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE DAL: HwdMmpm_Deregister failed for LPASS_DCODEC core");
      }

      // Deallocate the memory
      if(NULL != pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode)
      {
         qurt_elite_memory_free((void *)pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode);
         pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode = NULL;
         pLpaIfClkCfgGlobalCtxt->numClkCfgNode = 0;
      }


#ifndef SIM
      res = DAL_DeviceDetach(pLpaIfClkCfgGlobalCtxt->pDalDevHandle);
#endif
      pLpaIfClkCfgGlobalCtxt->state = AUDIOIF_CLOCK_DEINIT;
   }

   return res;
}

ADSPResult afeDalLpaifSetClks(AFEIoctlPropLpaifClkCfgType *pLpaifClkCfgInfo)
{
   ADSPResult res = DAL_ERROR;
   DALResult result = DAL_SUCCESS;
   LpaifClkCfgGlobalInfo *pLpaIfClkCfgGlobalCtxt = &AfeDevLpaifClkCfgGlobalCtx;

   int curClk;

   if(pLpaifClkCfgInfo)
   {
      int votePowerOn = 0;

      /* Requesting the power for digital codec core */
      if((0 == pLpaIfClkCfgGlobalCtxt->dig_codec_adsppm_info.powerVoteCnt) &&
         (AFEDevLpaifClkTypeDigitalCodecCore == pLpaifClkCfgInfo->pLpaifClkCfgArray->clkSetType)
         )
      {
         //vote power on
         if(DAL_SUCCESS != (res = HwdMmpm_RequestPwr(pLpaIfClkCfgGlobalCtxt->dig_codec_adsppm_info.clientId)))
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE DAL: HwdMmpm_RequestPwr failed for digital codec core");
            result |= res;
         }
         else /* Power request success */
         {
            /* Request for register programming for digital codec core */
            if(DAL_SUCCESS != (res = HwdMmpm_RequestRegProg(pLpaIfClkCfgGlobalCtxt->dig_codec_adsppm_info.clientId, MMPM_REG_PROG_NORM)))
            {
               /* Release the power request */
               HwdMmpm_ReleasePwr(pLpaIfClkCfgGlobalCtxt->dig_codec_adsppm_info.clientId);

               MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE DAL: HwdMmpm_RequestRegProg failed for digital codec core");
               result |= res;
            }
            else /* Register request success*/
            {
               pLpaIfClkCfgGlobalCtxt->dig_codec_adsppm_info.powerVoteCnt = 1;
               MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "Digital codec core vote for power on powerVoteCnt: %ld", pLpaIfClkCfgGlobalCtxt->dig_codec_adsppm_info.powerVoteCnt);
            }
         }
      }
      else if(0 == pLpaIfClkCfgGlobalCtxt->adsppmInfo.powerVoteCnt)
      {
         //vote power on
         if(DAL_SUCCESS != (res = HwdMmpm_RequestPwr(pLpaIfClkCfgGlobalCtxt->adsppmInfo.clientId)))
         {
            MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE DAL: HwdMmpm_RequestPwr failed for LPAIF clock config");
            result |= res;
         }
         else
         {
            pLpaIfClkCfgGlobalCtxt->adsppmInfo.powerVoteCnt = 1;
            MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "LPAIF clock config vote for power on powerVoteCnt: %ld", pLpaIfClkCfgGlobalCtxt->adsppmInfo.powerVoteCnt);
         }
      }
      else
      {
         res = DAL_SUCCESS;
      }

      if(DAL_SUCCESS == res)
      {
         uint32_t i;
         AFEDeviceLpaifClkCfgStruct *pCurClkCfg;
         LpaifClkCfgStruct *plpaifClkCfgNode = NULL;

         for(curClk = 0; curClk < pLpaifClkCfgInfo->numOfClks; curClk++)
         {
            pCurClkCfg = pLpaifClkCfgInfo->pLpaifClkCfgArray + curClk;
            res = lpaifSetThisClks(pCurClkCfg);
            result |= res;
         }

         //check if power vote off needed via go throught all clk node
         for(i = 0; i < pLpaIfClkCfgGlobalCtxt->numClkCfgNode; i++)
         {
            // Skip the core clk
            if((pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[i].ClkStatus)
               && (AFEDevLpaifClkTypeDigitalCodecCore != pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[i].clkSetType))
            {
               votePowerOn = 1;
               break;
            }
         }

         if(!votePowerOn)
         {
            //vote for power off
            if(DAL_SUCCESS != (res = HwdMmpm_ReleasePwr(pLpaIfClkCfgGlobalCtxt->adsppmInfo.clientId)))
            {
               MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE DAL HwdMmpm_ReleasePwr fail\n");
               result |= res;
            }

            pLpaIfClkCfgGlobalCtxt->adsppmInfo.powerVoteCnt = 0;
            MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "LPAIF_CLK Clk vote for power off powerVoteCnt: %ld", pLpaIfClkCfgGlobalCtxt->adsppmInfo.powerVoteCnt);
         }

         /* Get the core clk node */
         plpaifClkCfgNode = afeDalLocateDigitalCodecCoreClkCfgNode(pLpaIfClkCfgGlobalCtxt);

         if(NULL == plpaifClkCfgNode)
         {
            MSG(MSG_SSID_QDSP6, DBG_MED_PRIO, "AFE DAL - Cannot locate digital codec core clk node (unsupported)");
         }
         else if(0 == plpaifClkCfgNode->ClkStatus)
         {
            if(DAL_SUCCESS != (res = HwdMmpm_ReleaseRegProg(pLpaIfClkCfgGlobalCtxt->dig_codec_adsppm_info.clientId)))
            {
               MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE DAL HwdMmpm_ReleaseRegProg failed for digital codec core");
               result |= res;
            }

            if(DAL_SUCCESS != (res = HwdMmpm_ReleasePwr(pLpaIfClkCfgGlobalCtxt->dig_codec_adsppm_info.clientId)))
            {
               MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE DAL HwdMmpm_ReleasePwr failed for digital codec core");
               result |= res;
            }

            pLpaIfClkCfgGlobalCtxt->dig_codec_adsppm_info.powerVoteCnt = 0;

            MSG_1(MSG_SSID_QDSP6, DBG_MED_PRIO, "Digital codec core vote for power off powerVoteCnt: %ld", pLpaIfClkCfgGlobalCtxt->dig_codec_adsppm_info.powerVoteCnt);
         }
      }
   }

   return result;
}

static ADSPResult lpaifSetThisClks(AFEDeviceLpaifClkCfgStruct *pLpaifClkCfg)
{
   ADSPResult res = DAL_ERROR;
   LpaifClkCfgGlobalInfo *pLpaIfClkCfgGlobalCtxt = &AfeDevLpaifClkCfgGlobalCtx;

   if(pLpaifClkCfg)
   {
      if(AFEDevLpaifClkTypeDigitalCodecCore == pLpaifClkCfg->clkSetType)
      {
         // Locate the codec core clk node
         LpaifClkCfgStruct *plpaifClkCfgNode;

         plpaifClkCfgNode = afeDalLocateDigitalCodecCoreClkCfgNode(pLpaIfClkCfgGlobalCtxt);


         if(NULL == plpaifClkCfgNode)
         {
            MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AFE DAL clk set freq fail - digital codec core clk is unsupported");
         }
         else
         {
            plpaifClkCfgNode->clkFreqInHz = pLpaifClkCfg->clkValue;
            plpaifClkCfgNode->clkSrcFreqInHz = pLpaifClkCfg->clkValue;
            plpaifClkCfgNode->clkRoot = pLpaifClkCfg->clkRoot;


            res = lpaifSetThisClks_sub(plpaifClkCfgNode);
         }
      }
      else
      {
         //I2S/PCM clks
         uint32_t lpaifClkId;


         if(AFEDevLpaifClkRootExternal != pLpaifClkCfg->clkRoot)
         {
            pLpaifClkCfg->clkRoot = root_clk_type_internal;
         }

         for(lpaifClkId = 0; lpaifClkId <  pLpaIfClkCfgGlobalCtxt->numClkCfgNode; lpaifClkId++)
         {
            if((pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[lpaifClkId].audioIfType  == pLpaifClkCfg->inf) &&
               (pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[lpaifClkId].clkRoot      == pLpaifClkCfg->clkRoot) &&
               (pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[lpaifClkId].clkSetType   == pLpaifClkCfg->clkSetType))
            {
               //find the match clk
               //If this clk is set already, check the current setting value against previous setting value
               if((0 != pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[lpaifClkId].clkFreqInHz) &&
                  (0 != pLpaifClkCfg->clkValue) &&
                  (pLpaifClkCfg->clkValue != pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[lpaifClkId].clkFreqInHz))
               {
                  MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE DAL clk set freq check fail - lpaifClkIdx: %d cur freq: %ld prev freq: %ld\n",
                        lpaifClkId, pLpaifClkCfg->clkValue, pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[lpaifClkId].clkFreqInHz);
               }
               else
               {
                  pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[lpaifClkId].clkFreqInHz = pLpaifClkCfg->clkValue;
                  pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[lpaifClkId].clkSrcFreqInHz = pLpaifClkCfg->clkSrcValue;

                  if(0 != pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[lpaifClkId].clkFreqInHz)
                  {
                     if(0 == pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[lpaifClkId].ClkStatus)
                     {
                        if(pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[lpaifClkId].clkSetEnableFunc)
                        {
                           res = pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[lpaifClkId].clkSetEnableFunc(pLpaIfClkCfgGlobalCtxt->pDalDevHandle, (void *)&pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[lpaifClkId]);
                        }

                        if(DAL_SUCCESS == res)
                        {
                           pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[lpaifClkId].ClkStatus = 1;
                           pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[lpaifClkId].clkEnableCnt++;
                        }
                     }
                     else
                     {
                        pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[lpaifClkId].clkEnableCnt++;
                        MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "LPAIF_CLK Clk is on already lpaifClkId: %d enable count: %ld", lpaifClkId, pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[lpaifClkId].clkEnableCnt);
                        res = DAL_SUCCESS;
                     }
                  }
                  else
                  {
                     //clk disable section

                     /* First check if the clock is enabled or not */
                     if(pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[lpaifClkId].clkEnableCnt > 0)
                     {
                        /* Decrement the clock enable counter */
                        pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[lpaifClkId].clkEnableCnt--;

                        /* Check if the enable counter == 0, then call DAL API to disable the clock */
                        if((pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[lpaifClkId].clkSetDisableFunc) &&
                           (0 == pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[lpaifClkId].clkEnableCnt))
                        {
                           if(DAL_SUCCESS == (res = pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[lpaifClkId].clkSetDisableFunc(pLpaIfClkCfgGlobalCtxt->pDalDevHandle, (void *)&pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[lpaifClkId])))
                           {
                              pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[lpaifClkId].ClkStatus = 0;
                           }
                        }
                        else /* Skip clock disable */
                        {
                           MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "LPAIF_CLK Clk is to disable lpaifClkId: %d enable count: %ld", lpaifClkId, pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[lpaifClkId].clkEnableCnt);
                           res = DAL_SUCCESS;
                        }
                     }
                     else /* Clock is already disabled */
                     {
                        MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "LPAIF_CLK Clk is already disabled lpaifClkId: %d enable count: %ld", lpaifClkId, pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[lpaifClkId].clkEnableCnt);
                        res = DAL_SUCCESS;
                     }
                  }
               }
               break;
            }
         }

         if(lpaifClkId >=  pLpaIfClkCfgGlobalCtxt->numClkCfgNode)
         {
            //could not find the matched clk id
            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "LPAIF_CLK find match clkid fail - inf: 0x%x clkRoot: 0x%x clkSetType: 0x%x\n", pLpaifClkCfg->inf, pLpaifClkCfg->clkRoot, pLpaifClkCfg->clkSetType);

            //Print all in the table
            for(lpaifClkId = 0; lpaifClkId <  pLpaIfClkCfgGlobalCtxt->numClkCfgNode; lpaifClkId++)
            {
               MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "LPAIF_CLK - lpaifClkId: 0x%x clkRegId: 0x%lx\n", lpaifClkId, pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[lpaifClkId].clkRegId);
               MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "LPAIF_CLK - audioIfType: 0x%x  clkRoot: 0x%x\n", pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[lpaifClkId].audioIfType, pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[lpaifClkId].clkRoot);
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "LPAIF_CLK - clkSetType: 0x%x  \n", pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[lpaifClkId].clkSetType);
            }
         }
      }
   }

   return res;
}


/*
*
For OSR clk, DalClock_SetClockFrequency() supports belows in MHz. BIT clk is divided from OSR clk
0.512
0.768
1.024
1.536
2.048
3.072
4.096
6.144
8.192
12.288

For PCM bit clk, DalClock_SetClockFrequency() supports belows in MHz
0.512
0.768
1.024
1.536
2.048
3.072
4.096
6.144
8.192

For PCM_OE clk, DalClock_SetClockFrequency() supports belows in MHz. It is hard coded to 12.288 MHz now.

0.512
0.768
1.024
1.536
2.048
3.072
4.096
6.144
8.192
12.288
*/

//Full set function - enable for I2S bit clks, include
//1. DalClock_SetClockFrequency
//2. DalClock_SetClockDivider
//3. DalClock_EnableClock
static DALResult afeDalClkSetEnable1(DalDeviceHandle *pHandle, void *pClkCfg)
{
   DALResult res = DAL_ERROR;

   if(pClkCfg)
   {
#ifndef SIM
      uint32 divider = 1;
      uint32 resultFreq;

      LpaifClkCfgStruct *pLpaifClkCfg = (LpaifClkCfgStruct *)pClkCfg;

      if(0 != pLpaifClkCfg->clkFreqInHz)
      {
         divider = pLpaifClkCfg->clkSrcFreqInHz / pLpaifClkCfg->clkFreqInHz;
      }

      MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO, "AFE DAL: afeDalClkSetEnable1: lpaifClkIdx: %ld srcClkFreq: %ld clkFreq: %ld\n", pLpaifClkCfg->lpaifClkIdx, pLpaifClkCfg->clkSrcFreqInHz, pLpaifClkCfg->clkFreqInHz);

      if(DAL_SUCCESS != (res = DalClock_SetClockFrequency(pHandle, pLpaifClkCfg->clkRegId, pLpaifClkCfg->clkSrcFreqInHz, CLOCK_FREQUENCY_HZ_EXACT, &resultFreq)))
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE DAL: DalClock_SetClockFrequency fail: lpaifClkIdx: %ld freq: %d res: 0x%x\n", pLpaifClkCfg->lpaifClkIdx, pLpaifClkCfg->clkSrcFreqInHz, res);
      }
      else if(resultFreq != pLpaifClkCfg->clkSrcFreqInHz)
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE DAL: DalClock_SetClockFrequency fail: lpaifClkIdx: %ld freq_expect: %d freq_acrual_set: %d\n", pLpaifClkCfg->lpaifClkIdx, pLpaifClkCfg->clkSrcFreqInHz, resultFreq);
      }
      else if(DAL_SUCCESS != (res = DalClock_SetClockDivider(pHandle, pLpaifClkCfg->clkRegId, divider)))
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE DAL: DalClock_SetClockDivider fail: lpaifClkIdx: %ld divider: %d res: 0x%x\n", pLpaifClkCfg->lpaifClkIdx, divider, res);
      }
      else if(DAL_SUCCESS != (res = DalClock_EnableClock(pHandle, pLpaifClkCfg->clkRegId)))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE DAL: DalClock_EnableClock fail: lpaifClkIdx: %ld res: 0x%x\n", pLpaifClkCfg->lpaifClkIdx, res);
      }
#else
      res = DAL_SUCCESS;
#endif
   }

   return res;
}
//Part set function - enable for I2S osr and PCM clks, include
//1. DalClock_SetClockFrequency
//2. DalClock_EnableClock
static DALResult afeDalClkSetEnable2(DalDeviceHandle *pHandle, void *pClkCfg)
{
   DALResult res = DAL_ERROR;

   if(pClkCfg)
   {
      LpaifClkCfgStruct *pLpaifClkCfg = (LpaifClkCfgStruct *)pClkCfg;

      MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO, "AFE DAL: afeDalClkSetEnable2: lpaifClkIdx: %ld srcClkFreq: %ld clkFreq: %ld\n", pLpaifClkCfg->lpaifClkIdx, pLpaifClkCfg->clkSrcFreqInHz, pLpaifClkCfg->clkFreqInHz);
#ifndef SIM
      uint32 resultFreq;
      if(DAL_SUCCESS != (res = DalClock_SetClockFrequency(pHandle, pLpaifClkCfg->clkRegId, pLpaifClkCfg->clkFreqInHz, CLOCK_FREQUENCY_HZ_EXACT, &resultFreq)))
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE DAL: DalClock_SetClockFrequency fail: lpaifClkIdx: %ld freq: %d res: 0x%x\n", pLpaifClkCfg->lpaifClkIdx, pLpaifClkCfg->clkFreqInHz, res);
      }
      else if(resultFreq != pLpaifClkCfg->clkFreqInHz)
      {
         MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE DAL: DalClock_SetClockFrequency fail: lpaifClkIdx: %ld freq_expect: %d freq_actual_set: %d\n", pLpaifClkCfg->lpaifClkIdx, pLpaifClkCfg->clkFreqInHz, resultFreq);
      }
      else if(DAL_SUCCESS != (res = DalClock_EnableClock(pHandle, pLpaifClkCfg->clkRegId)))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE DAL: DalClock_EnableClock fail: lpaifClkIdx: %ld res: 0x%x\n", pLpaifClkCfg->lpaifClkIdx, res);
      }
#else
      res = DAL_SUCCESS;
#endif
   }

   return res;
}

//Part set function - enable for all external clks, include
//1. DalClock_EnableClock
static DALResult afeDalClkSetEnable3(DalDeviceHandle *pHandle, void *pClkCfg)
{
   DALResult res = DAL_ERROR;

   if(pClkCfg)
   {
      LpaifClkCfgStruct *pLpaifClkCfg = (LpaifClkCfgStruct *)pClkCfg;
      MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO, "AFE DAL: afeDalClkSetEnable3: lpaifClkIdx: %ld srcClkFreq: %ld clkFreq: %ld\n", pLpaifClkCfg->lpaifClkIdx, pLpaifClkCfg->clkSrcFreqInHz, pLpaifClkCfg->clkFreqInHz);

#ifndef SIM
      if(DAL_SUCCESS != (res = DalClock_EnableClock(pHandle, pLpaifClkCfg->clkRegId)))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE DAL: DalClock_EnableClock fail: lpaifClkIdx: %ld res: 0x%x\n", pLpaifClkCfg->lpaifClkIdx, res);
      }
#else
      res = DAL_SUCCESS;
#endif
   }

   return res;
}


//Part set function disable for all clks, include
//1. DalClock_DisableClock
static DALResult afeDalClkSetDisable1(DalDeviceHandle *pHandle, void *pClkCfg)
{
   DALResult res = DAL_ERROR;

   if(pClkCfg)
   {
      LpaifClkCfgStruct *pLpaifClkCfg = (LpaifClkCfgStruct *)pClkCfg;
      MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO, "AFE DAL: afeDalClkSetDisable1: lpaifClkIdx: %ld srcClkFreq: %ld clkFreq: %ld\n", pLpaifClkCfg->lpaifClkIdx, pLpaifClkCfg->clkSrcFreqInHz, pLpaifClkCfg->clkFreqInHz);

#ifndef SIM
      if(DAL_SUCCESS != (res = DalClock_DisableClock(pHandle, pLpaifClkCfg->clkRegId)))
      {
         MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE DAL: DalClock_DisableClock fail: lpaifClkIdx: %ld res: 0x%x\n", pLpaifClkCfg->lpaifClkIdx, res);
      }
#else
      res = DAL_SUCCESS;
#endif
   }

   return res;
}

static DALResult lpaifSetThisClks_sub(LpaifClkCfgStruct *pLpaifClkCfgNode)
{
   DALResult res = DAL_ERROR;
   LpaifClkCfgGlobalInfo *pLpaIfClkCfgGlobalCtxt = &AfeDevLpaifClkCfgGlobalCtx;

   if(pLpaifClkCfgNode)
   {
      if(0 != pLpaifClkCfgNode->clkFreqInHz)
      {
         if(0 == pLpaifClkCfgNode->ClkStatus)
         {
            if(pLpaifClkCfgNode->clkSetEnableFunc)
            {
               res = pLpaifClkCfgNode->clkSetEnableFunc(pLpaIfClkCfgGlobalCtxt->pDalDevHandle, (void *)pLpaifClkCfgNode);
            }

            if(DAL_SUCCESS == res)
            {
               pLpaifClkCfgNode->ClkStatus = 1;
               pLpaifClkCfgNode->clkEnableCnt++;
            }
         }
         else
         {
            pLpaifClkCfgNode->clkEnableCnt++;
            MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "LPAIF_CLK Clk is ON already lpaifClkIdx: %ld enable count: %ld", pLpaifClkCfgNode->lpaifClkIdx, pLpaifClkCfgNode->clkEnableCnt);
            res = DAL_SUCCESS;
         }
      }
      else
      {
         //clk disable section

         /* Check if the clock is enabled atleast once */
         if(pLpaifClkCfgNode->clkEnableCnt > 0)
         {
            /* Decrement the clock enable counter */
            pLpaifClkCfgNode->clkEnableCnt--;

            /* Check if the enable counter == 0, call the DAL API to disable the clock */
            if((pLpaifClkCfgNode->clkSetDisableFunc) && (0 == pLpaifClkCfgNode->clkEnableCnt))
            {
               if(DAL_SUCCESS == (res = pLpaifClkCfgNode->clkSetDisableFunc(pLpaIfClkCfgGlobalCtxt->pDalDevHandle, (void *)pLpaifClkCfgNode)))
               {
                  pLpaifClkCfgNode->ClkStatus = 0;
               }
            }
            else /* Skip clock disable */
            {
               MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "LPAIF_CLK Clk is to disable lpaifClkIdx: %ld enable count: %ld",
                     pLpaifClkCfgNode->lpaifClkIdx, pLpaifClkCfgNode->clkEnableCnt);
               res = DAL_SUCCESS;
            }
         }
         else /* Clock is already disabled */
         {
            MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "LPAIF_CLK Clk already disabled lpaifClkIdx: %ld enable count: %ld",
                  pLpaifClkCfgNode->lpaifClkIdx, pLpaifClkCfgNode->clkEnableCnt);
            res = DAL_SUCCESS;
         }
      }
   }

   return res;
}

static DALResult i2sReconfigThisClks(AFEIoctlPropReconfigI2SClkType *pI2SReCfgClkCfgInfo)
{
   DALResult res = DAL_ERROR;
   LpaifClkCfgGlobalInfo *pLpaIfClkCfgGlobalCtxt = &AfeDevLpaifClkCfgGlobalCtx;
   uint32 clkFreqInHz = 0;
   uint32 multiFactor = 0, devideFactor = 0;

   if(pI2SReCfgClkCfgInfo)
   {
      uint32_t lpaifClkId;

      if(pI2SReCfgClkCfgInfo->newSampleRate > pI2SReCfgClkCfgInfo->orgSampleRate)
      {
         multiFactor = pI2SReCfgClkCfgInfo->newSampleRate / pI2SReCfgClkCfgInfo->orgSampleRate;
      }
      else
      {
         devideFactor =  pI2SReCfgClkCfgInfo->orgSampleRate / pI2SReCfgClkCfgInfo->newSampleRate;
      }

      for(lpaifClkId = 0; lpaifClkId <  pLpaIfClkCfgGlobalCtxt->numClkCfgNode; lpaifClkId++)
      {
         if((pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[lpaifClkId].audioIfType == pI2SReCfgClkCfgInfo->inf) &&
            (AFEDevLpaifClkRootExternal != pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[lpaifClkId].clkRoot) &&
            (0 != pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[lpaifClkId].clkFreqInHz) &&
            (1 == pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[lpaifClkId].ClkStatus) &&
            (AFEDevLpaifClkTypeI2sBit == pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[lpaifClkId].clkSetType))
         {
            clkFreqInHz = pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[lpaifClkId].clkFreqInHz;

            if(pI2SReCfgClkCfgInfo->newSampleRate > pI2SReCfgClkCfgInfo->orgSampleRate)
            {
               pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[lpaifClkId].clkFreqInHz = clkFreqInHz * multiFactor;
            }
            else
            {
               pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[lpaifClkId].clkFreqInHz = clkFreqInHz / devideFactor;
            }

            if(DAL_SUCCESS != (res = afeDalReconfigI2SBitClk(pLpaIfClkCfgGlobalCtxt->pDalDevHandle, (void *)&pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[lpaifClkId])))
            {
               MSG_2(MSG_SSID_QDSP6, DBG_MED_PRIO, "LPAIF_CLK I2S Bit Clk reconfig failed: inf %d  clk id %d", pI2SReCfgClkCfgInfo->inf, lpaifClkId);
            }
         }
      }

   }
   return res;
}


DALResult afeDalI2SReconfigClks(AFEIoctlPropReconfigI2SClkType *pI2SReCfgClkCfgInfo)
{
   DALResult res = DAL_ERROR;
   LpaifClkCfgGlobalInfo *pLpaIfClkCfgGlobalCtxt = &AfeDevLpaifClkCfgGlobalCtx;

   if(pI2SReCfgClkCfgInfo)
   {
      //int votePowerOn = 0;

      if(0 == pLpaIfClkCfgGlobalCtxt->adsppmInfo.powerVoteCnt)
      {
         MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE DAL: wrong state fail \n");
      }
      else
      {
         res = DAL_SUCCESS;
      }

      if(DAL_SUCCESS == res)
      {
         res = i2sReconfigThisClks(pI2SReCfgClkCfgInfo);
      }

   }

   return res;
}

// reconfig for I2S bit clks only include DalClock_SetClockFrequency
// This use case is only for changing bit clock without turning off the bit clock and changing the mclock.
static DALResult afeDalReconfigI2SBitClk(DalDeviceHandle *pHandle, void *pClkCfg)
{
   DALResult res = DAL_ERROR;

   if(pClkCfg)
   {
      LpaifClkCfgStruct *pLpaifClkCfg = (LpaifClkCfgStruct *)pClkCfg;

      MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO, "AFE DAL: afeDalReconfigI2SBitClk: lpaifClkIdx: %ld srcClkFreq: %ld clkFreq: %ld\n", pLpaifClkCfg->lpaifClkIdx, pLpaifClkCfg->clkSrcFreqInHz, pLpaifClkCfg->clkFreqInHz);

#ifndef SIM
      LpaifClkCfgGlobalInfo *pLpaIfClkCfgGlobalCtxt = &AfeDevLpaifClkCfgGlobalCtx;

      uint32 resultFreq;
      uint32 divider = 1;

      if(LPASS_HW_VER_3_3_1 == pLpaIfClkCfgGlobalCtxt->hw_version)
      {

         // In Bear family, there is no Clock Generator that set the OSR and bit clock from the same source like single MND>
         // Therefore, there is no clock divider to be set in these cases.
         if(DAL_SUCCESS != (res = DalClock_SetClockFrequency(pHandle, pLpaifClkCfg->clkRegId,  pLpaifClkCfg->clkFreqInHz, CLOCK_FREQUENCY_HZ_EXACT, &resultFreq)))
         {
            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE DAL: DalClock_SetClockFrequency fail: lpaifClkIdx: %ld freq: %ld res: 0x%x\n", pLpaifClkCfg->lpaifClkIdx, pLpaifClkCfg->clkFreqInHz, res);
         }
         else if(resultFreq != pLpaifClkCfg->clkFreqInHz)
         {
            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE DAL: DalClock_SetClockFrequency fail: lpaifClkIdx: %ld freq_expect: %ld freq_acrual_set: %ld\n", pLpaifClkCfg->lpaifClkIdx, pLpaifClkCfg->clkFreqInHz, resultFreq);
         }
      }
      else
      {

         if(0 != pLpaifClkCfg->clkFreqInHz)
         {
            divider = pLpaifClkCfg->clkSrcFreqInHz / pLpaifClkCfg->clkFreqInHz;
         }

         if(DAL_SUCCESS != (res = DalClock_SetClockFrequency(pHandle, pLpaifClkCfg->clkRegId, pLpaifClkCfg->clkSrcFreqInHz, CLOCK_FREQUENCY_HZ_EXACT, &resultFreq)))
         {
            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE DAL: DalClock_SetClockFrequency fail: lpaifClkId: %d freq: %d res: 0x%x\n", pLpaifClkCfg->lpaifClkIdx, pLpaifClkCfg->clkSrcFreqInHz, res);
         }
         else if(resultFreq != pLpaifClkCfg->clkSrcFreqInHz)
         {
            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE DAL: DalClock_SetClockFrequency fail: lpaifClkId: %d freq_expect: %d freq_acrual_set: %d\n", pLpaifClkCfg->lpaifClkIdx, pLpaifClkCfg->clkSrcFreqInHz, resultFreq);
         }
         else if(DAL_SUCCESS != (res = DalClock_SetClockDivider(pHandle, pLpaifClkCfg->clkRegId, divider)))
         {
            MSG_3(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AFE DAL: DalClock_SetClockDivider fail: lpaifClkId: %d divider: %d res: 0x%x\n", pLpaifClkCfg->lpaifClkIdx, divider, res);
         }
      }
#else
      res = DAL_SUCCESS;
#endif
   }

   return res;
}


static AFEDevLpaifClkRootType afe_audio_if_get_dal_clkroot_fmt(uint32 clk_root_in, AfeDevLpaifClkType clk_type)
{
   AFEDevLpaifClkRootType clk_root_out;

   if(AFEDevLpaifClkTypeDigitalCodecCore == clk_type)
   {
      switch(clk_root_in)
      {
         case AFE_PORT_INTERNAL_DIGIATL_CDC_CLK_ROOT_PRII2SOSR:
            clk_root_out = AFEDevLpaifClkRootDigitalCodecCorePriMi2sOsr;
            break;
         case AFE_PORT_INTERNAL_DIGIATL_CDC_CLK_ROOT_SECI2SOSR:
            clk_root_out = AFEDevLpaifClkRootDigitalCodecCoreSecMi2sOsr;
            break;
         case AFE_PORT_INTERNAL_DIGIATL_CDC_CLK_ROOT_TERI2SOSR:
            clk_root_out = AFEDevLpaifClkRootDigitalCodecCoreTerMi2sOsr;
            break;
         case AFE_PORT_INTERNAL_DIGIATL_CDC_CLK_ROOT_QUAI2SOSR:
            clk_root_out = AFEDevLpaifClkRootDigitalCodecCoreQuaMi2sOsr;
            break;
         case AFE_PORT_INTERNAL_DIGIATL_CDC_CLK_ROOT_CODEC:
            clk_root_out = AFEDevLpaifClkRootDigitalCodecCoreXo;
            break;
         default:
            clk_root_out = AFEDevLpaifClkRootDigitalCodecCoreRootStart;
            break;
      }
   }
   else
   {
      switch(clk_root_in)
      {
         case AFE_PORT_LPACLK_CLK_ROOT_PXO:
            clk_root_out = AFEDevLpaifClkRootPxo;
            break;
         case AFE_PORT_LPACLK_CLK_ROOT_CXO:
            clk_root_out = AFEDevLpaifClkRootCxo;
            break;
         case AFE_PORT_LPACLK_CLK_ROOT_LPAPLL:
            clk_root_out = AFEDevLpaifClkRootLpapll;
            break;
         case AFE_PORT_LPACLK_CLK_ROOT_SECPLL:
            clk_root_out = AFEDevLpaifClkRootSecpll;
            break;
         case AFE_PORT_LPACLK_CLK_ROOT_LPAQ6PLL:
            clk_root_out = AFEDevLpaifClkRootLpaq6pll;
            break;
         default:
            clk_root_out = AFEDevLpaifClkRootDefault;
            break;
      }
   }

   return clk_root_out;
}


ADSPResult afe_set_audioif_clk_cfg(uint16_t port_id, int8_t *cfg, uint32_t payload_size)
{
   ADSPResult result = ADSP_EOK;
   //audio_if_state_struct_t *p_dev_state = (audio_if_state_struct_t *) pDevPort->afe_drv;

   if(AUDIOIF_CLOCK_INIT != AfeDevLpaifClkCfgGlobalCtx.state)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_set_audioif_clk_cfg has not been initialized");
      return ADSP_EUNSUPPORTED;
   }

   if((IS_AUDIO_IF_PORT_AFE_ID(port_id)) || (IS_SLIMBUS_PORT_AFE_ID(port_id)))
   {
      // choose the appropriate config version
      uint32_t lpaif_clk_cfg_version = (uint32_t) * (cfg);
      if(lpaif_clk_cfg_version > AFE_API_VERSION_LPAIF_CLK_CONFIG)
      {
         lpaif_clk_cfg_version = AFE_API_VERSION_LPAIF_CLK_CONFIG;
      }

      switch(lpaif_clk_cfg_version)
      {
         case AFE_API_VERSION_LPAIF_CLK_CONFIG:
         {
            afe_param_id_lpaif_clk_cfg_t *lpaifclk_cfg_ptr =  (afe_param_id_lpaif_clk_cfg_t *)cfg;

            if(payload_size < sizeof(afe_param_id_lpaif_clk_cfg_t))
            {
               MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "lpaif I2S/PCM clk Cfg fail: Invalid payload size: %ld, port_id: 0x%x", payload_size, port_id);
               result = ADSP_EBADPARAM;
            }
            else
            {
               AFEIoctlPropLpaifClkCfgType lpaifClkCfgProp;

               AFEDeviceLpaifClkCfgStruct clkCfgPara[LPAIFCLK_MAX_NUM_CLKS_TO_CFG];
               uint32_t numOfClks = LPAIFCLK_MAX_NUM_CLKS_TO_CFG;
               uint32_t isI2sBus = LPAIFCLK_I2S_INTERFACE;
               AFEDevLpaifClkRootType clkRoot;
               AFEDevAudIfType inf;
               AFEDevAudIfDirType dir;

               //print dbg msg
               MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO, "LPAIF_CLK port_id: 0x%x clk_value1: %lu clk_value2: %lu", port_id, lpaifclk_cfg_ptr->clk_value1, lpaifclk_cfg_ptr->clk_value2);
               MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO, "LPAIF_CLK clk_src: %d clk_root: %d clk_set_mode: %d", lpaifclk_cfg_ptr->clk_src, lpaifclk_cfg_ptr->clk_root, lpaifclk_cfg_ptr->clk_set_mode);

               if((AFE_PORT_ID_PRIMARY_PCM_RX == port_id) ||
                  (AFE_PORT_ID_PRIMARY_PCM_TX == port_id) ||
                  (AFE_PORT_ID_SECONDARY_PCM_RX == port_id) ||
                  (AFE_PORT_ID_SECONDARY_PCM_TX == port_id) ||
                  (AFE_PORT_ID_TERTIARY_PCM_RX == port_id) ||
                  (AFE_PORT_ID_TERTIARY_PCM_TX == port_id) ||
                  (AFE_PORT_ID_QUATERNARY_PCM_RX == port_id) ||
                  (AFE_PORT_ID_QUATERNARY_PCM_TX == port_id))
               {
                  isI2sBus = LPAIFCLK_PCM_INTERFACE;
               }

               if(IS_SLIMBUS_PORT_AFE_ID(port_id))
               {
                  //for slimbus port, swith to AUDIO_IF_SPEAKER_I2S for Codec MCLK
                  //
                  //For codec like Taiko, its MCLK can be from either “audio_core_lpaif_codec_spkr_osr_clk”
                  //or PMIC XO. If the codec MCLK is from codec speaker I2S OSR,
                  //client can use either AFE I2S port define (AFE_PORT_ID_I2S_RX)
                  //or AFE SLIMBUS port define (AFE_PORT_ID_SLIMBUS_MULTI_CHAN_0_RX, … AFE_PORT_ID_SLIMBUS_MULTI_CHAN_6_TX)
                  //to configure the codec MCLK.

                  inf = AUDIO_IF_SPEAKER_I2S;
                  dir = AUDIO_IF_SINK;
               }
               else
               {
                  inf = afe_audio_dev_if_type_get(port_id);
                  dir = (AFEDevAudIfDirType)(port_id & AFE_PORT_DIR_MASK);
               }

               switch(lpaifclk_cfg_ptr->clk_set_mode)
               {
                  case AFE_PORT_LPACLK_CLK_VALUE1_VALID_ONLY:
                  {
                     //It is I2S bit clk for MI2S port, or PCM bit clk for PCM port
                     numOfClks = 1;

                     clkCfgPara[0].inf = inf;
                     clkCfgPara[0].dir = dir;
                     clkCfgPara[0].clkValue = lpaifclk_cfg_ptr->clk_value1;
                     clkCfgPara[0].clkSrcValue = clkCfgPara[0].clkValue;
                     clkCfgPara[0].clkSetType = (LPAIFCLK_I2S_INTERFACE == isI2sBus) ? AFEDevLpaifClkTypeI2sBit : AFEDevLpaifClkTypePcmBit;

                     clkRoot = afe_audio_if_get_dal_clkroot_fmt(lpaifclk_cfg_ptr->clk_root, clkCfgPara[0].clkSetType);

                     clkCfgPara[0].clkRoot = (lpaifclk_cfg_ptr->clk_src == AFE_PORT_LPACLK_CLK_SRC_EXTERNAL) ? AFEDevLpaifClkRootExternal : clkRoot;

                     //For PCM interface, set PCMOE clk here
                     if(LPAIFCLK_PCM_INTERFACE == isI2sBus)
                     {
                        uint32 HzValue = LPAIFCLK_PCMOE_CLK_IN_12288000_HZ; // use this as default;

                        numOfClks = 2;
                        clkCfgPara[1].inf = AUDIO_IF_PRIMARY_PCM;    //always use AUDIO_IF_PRIMARY_PCM as default
                        if(LPASS_HW_VER_3_3_1 == AfeDevLpaifClkCfgGlobalCtx.hw_version)
                        {
                           HzValue = LPAIFCLK_PCMOE_CLK_IN_61440000_HZ;
                           if(AUDIO_IF_SECONDARY_PCM == inf)
                           {
                              clkCfgPara[1].inf = AUDIO_IF_SECONDARY_PCM;
                           }
                        }
                        clkCfgPara[1].dir = dir;
                        clkCfgPara[1].clkValue = (0 == lpaifclk_cfg_ptr->clk_value1) ? 0 : HzValue;
                        clkCfgPara[1].clkSrcValue = clkCfgPara[1].clkValue;
                        clkCfgPara[1].clkRoot = root_clk_type_internal;
                        clkCfgPara[1].clkSetType = AFEDevLpaifClkTypePcmOe;
                     }
                  }
                     break;
                  case AFE_PORT_LPACLK_CLK_VALUE2_VALID_ONLY:
                  {
                     //It is I2S OSR for MI2S port only
                     numOfClks = 1;

                     clkCfgPara[0].inf = inf;
                     clkCfgPara[0].dir = dir;
                     clkCfgPara[0].clkValue = lpaifclk_cfg_ptr->clk_value2;
                     clkCfgPara[0].clkSrcValue = clkCfgPara[0].clkValue;
                     clkCfgPara[0].clkSetType = AFEDevLpaifClkTypeI2sOsr;

                     clkRoot = afe_audio_if_get_dal_clkroot_fmt(lpaifclk_cfg_ptr->clk_root, clkCfgPara[0].clkSetType);
                     clkCfgPara[0].clkRoot = (lpaifclk_cfg_ptr->clk_src == AFE_PORT_LPACLK_CLK_SRC_EXTERNAL) ? AFEDevLpaifClkRootExternal : clkRoot;

                  }
                     break;
                  case AFE_PORT_LPACLK_CLK_VALUE1_VALUE2_BOTH_VALID:
                  {
                     //VALUE1 - It is I2S bit clk for MI2S port, or PCM bit clk for PCM port
                     //VALUE2 - It is I2S OSR for MI2S only
                     numOfClks = 2;

                     //Set OSR first then BIT clk for I2S

                     //CLK 2 - OSR
                     clkCfgPara[0].inf = inf;
                     clkCfgPara[0].dir = dir;
                     clkCfgPara[0].clkValue = lpaifclk_cfg_ptr->clk_value2;
                     clkCfgPara[0].clkSrcValue = clkCfgPara[0].clkValue;
                     clkCfgPara[0].clkSetType = AFEDevLpaifClkTypeI2sOsr;

                     clkRoot = afe_audio_if_get_dal_clkroot_fmt(lpaifclk_cfg_ptr->clk_root, clkCfgPara[0].clkSetType);
                     clkCfgPara[0].clkRoot = (lpaifclk_cfg_ptr->clk_src == AFE_PORT_LPACLK_CLK_SRC_EXTERNAL) ? AFEDevLpaifClkRootExternal : clkRoot;

                     //CLK 1 - BIT
                     clkCfgPara[1].inf = inf;
                     clkCfgPara[1].dir = dir;
                     clkCfgPara[1].clkValue = lpaifclk_cfg_ptr->clk_value1;
                     clkCfgPara[1].clkSrcValue = lpaifclk_cfg_ptr->clk_value2;
                     clkCfgPara[1].clkSetType = AFEDevLpaifClkTypeI2sBit;

                     clkRoot = afe_audio_if_get_dal_clkroot_fmt(lpaifclk_cfg_ptr->clk_root, clkCfgPara[1].clkSetType);
                     clkCfgPara[1].clkRoot = (lpaifclk_cfg_ptr->clk_src == AFE_PORT_LPACLK_CLK_SRC_EXTERNAL) ? AFEDevLpaifClkRootExternal : clkRoot;
                  }
                     break;
                  default:
                     //dbg msg
                     break;
               }

               lpaifClkCfgProp.pLpaifClkCfgArray = &clkCfgPara[0];
               lpaifClkCfgProp.numOfClks = numOfClks;

               if(ADSP_EOK != (result = afeDalLpaifSetClks(&lpaifClkCfgProp)))
               {
                  MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed in afeDalLpaifSetClks for LPAIF_CLK: %x", port_id);
                  result = ADSP_EBADPARAM;
               }
            }
         }
            break;
         default:
            return ADSP_EUNSUPPORTED;
      }
   }

   return result;
}


ADSPResult afe_set_audioif_reconfig_i2s_clk(uint16_t port_id, uint32_t newSampleRate, uint32_t orgSampleRate)
{
   ADSPResult result = ADSP_EOK;
   AFEIoctlPropReconfigI2SClkType lpaifClkReCfgProp;

   if(AUDIOIF_CLOCK_INIT != AfeDevLpaifClkCfgGlobalCtx.state)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "afe_set_audioif_reconfig_i2s_clk has not been initialized");
      return ADSP_EUNSUPPORTED;
   }

   lpaifClkReCfgProp.inf =  afe_audio_dev_if_type_get(port_id);
   lpaifClkReCfgProp.dir = (AFEDevAudIfDirType)(port_id & AFE_PORT_DIR_MASK);
   lpaifClkReCfgProp.newSampleRate =  newSampleRate;
   lpaifClkReCfgProp.orgSampleRate = orgSampleRate;

   if(ADSP_EOK != (result =  afeDalI2SReconfigClks(&lpaifClkReCfgProp)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed in afeDalI2SReconfigClks  fnction for LPAIF_CLK: %x", port_id);
      result = ADSP_EBADPARAM;
   }

   return result;
}


static LpaifClkCfgStruct* afeDalLocateDigitalCodecCoreClkCfgNode(LpaifClkCfgGlobalInfo *pLpaIfClkCfgGlobalCtxt)
{
   uint32_t i;

   for(i = 0; i < pLpaIfClkCfgGlobalCtxt->numClkCfgNode; i++)
   {
      if(AFEDevLpaifClkTypeDigitalCodecCore == pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[i].clkSetType)
      {
         return &(pLpaIfClkCfgGlobalCtxt->pLpaifClkCfgNode[i]);
      }
   }
   return NULL;
}


ADSPResult afe_set_digital_codec_coreclk_cfg(uint16_t port_id, int8_t *cfg, uint32_t payload_size)
{
   ADSPResult result = ADSP_EOK;

   switch(port_id)
   {
      case AFE_PORT_ID_PRIMARY_MI2S_RX:
      case AFE_PORT_ID_PRIMARY_MI2S_TX:
      case AFE_PORT_ID_SECONDARY_MI2S_RX:
      case AFE_PORT_ID_SECONDARY_MI2S_TX:
      case AFE_PORT_ID_TERTIARY_MI2S_RX:
      case AFE_PORT_ID_TERTIARY_MI2S_TX:
      case AFE_PORT_ID_QUATERNARY_MI2S_RX:
      case AFE_PORT_ID_QUATERNARY_MI2S_TX:
      case AFE_PORT_ID_QUINARY_MI2S_RX:
      case AFE_PORT_ID_QUINARY_MI2S_TX:
      case AFE_PORT_ID_SENARY_MI2S_RX:
      case AFE_PORT_ID_SENARY_MI2S_TX:
      {
         // choose the appropriate config version
         uint32_t cdc_clk_cfg_version = (uint32_t) * (cfg);
         if(cdc_clk_cfg_version > AFE_API_VERSION_INTERNAL_DIGIATL_CDC_CLK_CONFIG)
         {
            cdc_clk_cfg_version = AFE_API_VERSION_INTERNAL_DIGIATL_CDC_CLK_CONFIG;
         }

         switch(cdc_clk_cfg_version)
         {
            case AFE_API_VERSION_INTERNAL_DIGIATL_CDC_CLK_CONFIG:
            {
               afe_param_id_internal_digital_cdc_clk_cfg_t *cdcclk_cfg_ptr =  (afe_param_id_internal_digital_cdc_clk_cfg_t *)cfg;

               if(payload_size < sizeof(afe_param_id_internal_digital_cdc_clk_cfg_t))
               {
                  MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Internal Digital Codec core clk cfg fail: Invalid payload size: %ld, port_id: 0x%x", payload_size, port_id);
                  result = ADSP_EBADPARAM;
               }
               else
               {
                  AFEIoctlPropLpaifClkCfgType lpaifClkCfgProp;

                  AFEDeviceLpaifClkCfgStruct clkCfgPara;

                  //print dbg msg
                  MSG_3(MSG_SSID_QDSP6, DBG_MED_PRIO, "CDC_CLK port_id: 0x%x clk_value: %lu clk_root: %d", port_id, cdcclk_cfg_ptr->clk_value, cdcclk_cfg_ptr->clk_root);

                  clkCfgPara.inf = AUDIO_IF_PRIMARY_MI2S;
                  clkCfgPara.dir = (AFEDevAudIfDirType)(port_id & AFE_PORT_DIR_MASK);
                  clkCfgPara.clkValue = cdcclk_cfg_ptr->clk_value;
                  clkCfgPara.clkRoot = afe_audio_if_get_dal_clkroot_fmt(cdcclk_cfg_ptr->clk_root, AFEDevLpaifClkTypeDigitalCodecCore);
                  clkCfgPara.clkSetType = AFEDevLpaifClkTypeDigitalCodecCore;

                  lpaifClkCfgProp.pLpaifClkCfgArray = &clkCfgPara;
                  lpaifClkCfgProp.numOfClks = 1;

                  if(ADSP_EOK != (result = afeDalLpaifSetClks(&lpaifClkCfgProp)))
                  {
                     MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed in afeDalLpaifSetClks for LPAIF_CLK: %x", port_id);
                     result = ADSP_EBADPARAM;
                  }
               }
            }
               break;
            default:
               return ADSP_EUNSUPPORTED;
         }
      }
         break;
      default:
         result = ADSP_EUNSUPPORTED;
         break;
   }

   return result;
}
