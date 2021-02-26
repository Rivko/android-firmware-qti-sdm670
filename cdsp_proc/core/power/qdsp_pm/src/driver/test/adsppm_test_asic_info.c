/*
* Copyright (c) 2013-2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * adsppm_test_asic_info.c
 *
 *  Created on: Oct 28, 2014
 *      Author: ltayleon
 *
 *  Notes:
 *  [14/10/28]: All parameters below need to correspond with
 *              the contents of each target's device configuration file.
 */

#include "mmpm.h"
#include "adsppm_test_utils.h"
#include "adsppm_test_asic_info.h"
#include "limits.h"
#include "asic.h"

const MmpmCoreInstanceIdType AdsppmTestAvailCoresPerTarget[MMPM_CORE_ID_LPASS_END - MMPM_CORE_ID_LPASS_START + 1][NUM_MAX_TARGETS] =
{
                                  /*                                 AdsppmTestTargets                           */
    /* MmpmCoreIdType */          /* {                   8994,                    8992,                    8996           sdm845			sdm670 } */
    /* MMPM_CORE_ID_LPASS_START   */ {MMPM_CORE_INSTANCE_NONE, MMPM_CORE_INSTANCE_NONE, MMPM_CORE_INSTANCE_NONE, MMPM_CORE_INSTANCE_NONE,	MMPM_CORE_INSTANCE_NONE},
    /* MMPM_CORE_ID_LPASS_ADSP    */ {   MMPM_CORE_INSTANCE_0,    MMPM_CORE_INSTANCE_0,    MMPM_CORE_INSTANCE_0, MMPM_CORE_INSTANCE_0,		MMPM_CORE_INSTANCE_0},
    /* MMPM_CORE_ID_LPASS_CORE    */ {   MMPM_CORE_INSTANCE_0,    MMPM_CORE_INSTANCE_0,    MMPM_CORE_INSTANCE_0, MMPM_CORE_INSTANCE_0,		MMPM_CORE_INSTANCE_0},
    /* MMPM_CORE_ID_LPASS_LPM     */ {   MMPM_CORE_INSTANCE_0,    MMPM_CORE_INSTANCE_0,    MMPM_CORE_INSTANCE_0, MMPM_CORE_INSTANCE_0,		MMPM_CORE_INSTANCE_0},
    /* MMPM_CORE_ID_LPASS_DML     */ {   MMPM_CORE_INSTANCE_0,    MMPM_CORE_INSTANCE_0,    MMPM_CORE_INSTANCE_0, MMPM_CORE_INSTANCE_0,		MMPM_CORE_INSTANCE_0},
    /* MMPM_CORE_ID_LPASS_AIF     */ {   MMPM_CORE_INSTANCE_0,    MMPM_CORE_INSTANCE_0,    MMPM_CORE_INSTANCE_0, MMPM_CORE_INSTANCE_0,		MMPM_CORE_INSTANCE_0},
    /* MMPM_CORE_ID_LPASS_SLIMBUS */ {   MMPM_CORE_INSTANCE_1,    MMPM_CORE_INSTANCE_1,    MMPM_CORE_INSTANCE_1, MMPM_CORE_INSTANCE_1,		MMPM_CORE_INSTANCE_1},
    /* MMPM_CORE_ID_LPASS_MIDI    */ {MMPM_CORE_INSTANCE_NONE, MMPM_CORE_INSTANCE_NONE, MMPM_CORE_INSTANCE_NONE, MMPM_CORE_INSTANCE_NONE,	MMPM_CORE_INSTANCE_NONE},
    /* MMPM_CORE_ID_LPASS_AVSYNC  */ {   MMPM_CORE_INSTANCE_0,    MMPM_CORE_INSTANCE_0,    MMPM_CORE_INSTANCE_0, MMPM_CORE_INSTANCE_0,		MMPM_CORE_INSTANCE_0},
    /* MMPM_CORE_ID_LPASS_HWRSMP  */ {   MMPM_CORE_INSTANCE_0,    MMPM_CORE_INSTANCE_0,    MMPM_CORE_INSTANCE_0, MMPM_CORE_INSTANCE_0,		MMPM_CORE_INSTANCE_0},
    /* MMPM_CORE_ID_LPASS_SRAM    */ {   MMPM_CORE_INSTANCE_0,    MMPM_CORE_INSTANCE_0, MMPM_CORE_INSTANCE_NONE, MMPM_CORE_INSTANCE_NONE,	MMPM_CORE_INSTANCE_NONE},
    /* MMPM_CORE_ID_LPASS_DCODEC  */ {MMPM_CORE_INSTANCE_NONE, MMPM_CORE_INSTANCE_NONE, MMPM_CORE_INSTANCE_NONE, MMPM_CORE_INSTANCE_NONE,	MMPM_CORE_INSTANCE_NONE},
    /* MMPM_CORE_ID_LPASS_SPDIF   */ {MMPM_CORE_INSTANCE_NONE, MMPM_CORE_INSTANCE_NONE, MMPM_CORE_INSTANCE_NONE, MMPM_CORE_INSTANCE_NONE,	MMPM_CORE_INSTANCE_NONE},
    /* MMPM_CORE_ID_LPASS_HDMIRX  */ {MMPM_CORE_INSTANCE_NONE, MMPM_CORE_INSTANCE_NONE, MMPM_CORE_INSTANCE_NONE, MMPM_CORE_INSTANCE_NONE,	MMPM_CORE_INSTANCE_NONE},
    /* MMPM_CORE_ID_LPASS_HDMITX  */ {MMPM_CORE_INSTANCE_NONE, MMPM_CORE_INSTANCE_NONE,    MMPM_CORE_INSTANCE_0, MMPM_CORE_INSTANCE_NONE,	MMPM_CORE_INSTANCE_0},
    /* MMPM_CORE_ID_LPASS_SIF     */ {MMPM_CORE_INSTANCE_NONE, MMPM_CORE_INSTANCE_NONE, MMPM_CORE_INSTANCE_NONE, MMPM_CORE_INSTANCE_NONE,	MMPM_CORE_INSTANCE_NONE},
    /* MMPM_CORE_ID_LPASS_BSTC    */ {MMPM_CORE_INSTANCE_NONE, MMPM_CORE_INSTANCE_NONE, MMPM_CORE_INSTANCE_NONE, MMPM_CORE_INSTANCE_NONE,	MMPM_CORE_INSTANCE_NONE},
    /* MMPM_CORE_ID_LPASS_HVX     */ {MMPM_CORE_INSTANCE_NONE, MMPM_CORE_INSTANCE_NONE,    MMPM_CORE_INSTANCE_0, MMPM_CORE_INSTANCE_NONE,	MMPM_CORE_INSTANCE_NONE},
    /* MMPM_CORE_ID_LPASS_DPAudio */ {MMPM_CORE_INSTANCE_NONE, MMPM_CORE_INSTANCE_NONE, MMPM_CORE_INSTANCE_NONE, MMPM_CORE_INSTANCE_0,		MMPM_CORE_INSTANCE_0},
};

const boolean AdsppmTestAvailBusPortsPerTarget[MMPM_BW_PORT_ID_CORE][NUM_MAX_TARGETS] =
{
                                      /*   AdsppmTestTargets   */
    /* MmpmBwPortIdType */            /* { 8994,  8992,  8996,   845	670} */
    /* MMPM_BW_PORT_ID_NONE           */ {FALSE, FALSE, FALSE, FALSE,	FALSE},
    /* MMPM_BW_PORT_ID_ADSP_MASTER    */ { TRUE,  TRUE,  TRUE, TRUE,	TRUE},
    /* MMPM_BW_PORT_ID_DML_MASTER     */ { TRUE,  TRUE,  TRUE, TRUE,	TRUE},
    /* MMPM_BW_PORT_ID_AIF_MASTER     */ { TRUE,  TRUE,  TRUE, TRUE,	TRUE},
    /* MMPM_BW_PORT_ID_SLIMBUS_MASTER */ { TRUE,  TRUE,  TRUE, TRUE,	TRUE},
    /* MMPM_BW_PORT_ID_MIDI_MASTER    */ {FALSE, FALSE, FALSE, FALSE,	FALSE},
    /* MMPM_BW_PORT_ID_HWRSMP_MASTER  */ { TRUE,  TRUE,  TRUE, TRUE,	TRUE},
    /* MMPM_BW_PORT_ID_EXT_AHB_MASTER */ { TRUE,  TRUE,  TRUE, TRUE,	TRUE},
    /* MMPM_BW_PORT_ID_SPDIF_MASTER   */ {FALSE, FALSE, FALSE, FALSE,	FALSE},
    /* MMPM_BW_PORT_ID_HDMIRX_MASTER  */ {FALSE, FALSE, FALSE, FALSE,	FALSE},
    /* MMPM_BW_PORT_ID_HDMITX_MASTER  */ {FALSE, FALSE,  TRUE, FALSE,	TRUE},
    /* MMPM_BW_PORT_ID_SIF_MASTER     */ {FALSE, FALSE, FALSE, FALSE,	FALSE},
    /* MMPM_BW_PORT_ID_DPAUDIO_MASTER */ {FALSE, FALSE, FALSE, TRUE,	TRUE},
    /* MMPM_BW_PORT_ID_HCP_MASTER     */ {FALSE, FALSE, FALSE, FALSE,	FALSE},
    /* MMPM_BW_PORT_ID_DMA_MASTER     */ {FALSE, FALSE, FALSE, FALSE,	FALSE},
    /* MMPM_BW_PORT_ID_SLPI_SDC_MASTER*/ {FALSE, FALSE, FALSE, FALSE,	TRUE},
    /* MMPM_BW_PORT_ID_SLPI_QUP_MASTER*/ {FALSE, FALSE, FALSE, FALSE,	TRUE},
    /* MMPM_BW_PORT_ID_DML_SLAVE      */ { TRUE,  TRUE,  TRUE, TRUE,	TRUE},
    /* MMPM_BW_PORT_ID_AIF_SLAVE      */ { TRUE,  TRUE,  TRUE, TRUE,	TRUE},
    /* MMPM_BW_PORT_ID_SLIMBUS_SLAVE  */ { TRUE,  TRUE,  TRUE, TRUE,	TRUE},
    /* MMPM_BW_PORT_ID_MIDI_SLAVE     */ {FALSE, FALSE, FALSE, FALSE,	FALSE},
    /* MMPM_BW_PORT_ID_HWRSMP_SLAVE   */ { TRUE,  TRUE,  TRUE, TRUE,	TRUE},
    /* MMPM_BW_PORT_ID_AVSYNC_SLAVE   */ { TRUE,  TRUE,  TRUE, TRUE,	TRUE},
    /* MMPM_BW_PORT_ID_LPM_SLAVE      */ { TRUE,  TRUE,  TRUE, TRUE,	TRUE},
    /* MMPM_BW_PORT_ID_SRAM_SLAVE     */ { TRUE,  TRUE, FALSE, FALSE,	FALSE},
    /* MMPM_BW_PORT_ID_EXT_AHB_SLAVE  */ { TRUE,  TRUE,  TRUE, TRUE,	TRUE},
    /* MMPM_BW_PORT_ID_DDR_SLAVE      */ { TRUE,  TRUE,  TRUE, TRUE,	TRUE},
    /* MMPM_BW_PORT_ID_OCMEM_SLAVE    */ { TRUE,  TRUE, FALSE, FALSE,	FALSE},
    /* MMPM_BW_PORT_ID_PERIFNOC_SLAVE */ { TRUE,  TRUE,  TRUE, TRUE,	FALSE},
    /* MMPM_BW_PORT_ID_SPDIF_SLAVE    */ {FALSE, FALSE, FALSE, FALSE,	FALSE},
    /* MMPM_BW_PORT_ID_HDMIRX_SLAVE   */ {FALSE, FALSE, FALSE, FALSE,	FALSE},
    /* MMPM_BW_PORT_ID_HDMITX_SLAVE   */ {FALSE, FALSE,  TRUE, FALSE,	TRUE},
    /* MMPM_BW_PORT_ID_SIF_SLAVE      */ {FALSE, FALSE, FALSE, FALSE,	FALSE},
    /* MMPM_BW_PORT_ID_BSTC_SLAVE     */ {FALSE, FALSE, FALSE, FALSE,	FALSE},
    /* MMPM_BW_PORT_ID_DCODEC_SLAVE   */ {FALSE, FALSE, FALSE, FALSE,	FALSE},
    /* MMPM_BW_PORT_ID_DPAUDIO_SLAVE  */ {FALSE, FALSE, FALSE, TRUE,	TRUE},
    /* MMPM_BW_PORT_ID_TCM_SLAVE      */ {FALSE, FALSE, FALSE, FALSE,	FALSE},
    /* MMPM_BW_PORT_ID_SLPI_SRAM_SLAVE*/ {FALSE, FALSE, FALSE, FALSE,	TRUE},
};

const boolean AdsppmTestAvailMemoriesPerTarget[MMPM_MEM_MAX][NUM_MAX_TARGETS] =
{
                          /*   AdsppmTestTargets   */
    /* MmpmMemIdType */   /* { 8994,  8992,  8996	845		670} */
    /* MMPM_MEM_NONE      */ {FALSE, FALSE, FALSE, FALSE, FALSE},
    /* MMPM_MEM_OCMEM     */ { TRUE,  TRUE, FALSE, FALSE, FALSE},
    /* MMPM_MEM_LPASS_LPM */ { TRUE,  TRUE,  TRUE, TRUE,  TRUE},
    /* MMPM_MEM_SRAM      */ { TRUE,  TRUE, FALSE, FALSE, TRUE}
};

// HWRSP_CORE frequency plans in Hz
const AsicInfo_ClockVoltageType HWRSP_CORE_8994[] =
{
    { 19200000, SVS2},
    { 68266666, SVS2},
    {136533333, NOM }
};

const AsicInfo_ClockVoltageType HWRSP_CORE_8992[] =
{
    { 19200000, SVS2},
    { 68266666, SVS2},
    {136533333, NOM }
};

const AsicInfo_ClockVoltageType HWRSP_CORE_8996[] =
{
    { 19200000, SVS2},
    { 76800000, SVS2},
    {153600000, SVS },
    {307200000, NOM }
};

const AsicInfo_ClockVoltageType HWRSP_CORE_845[] =
{
    { 38400000, MIN_SVS},
    { 76800000, SVS},
    {153600000, SVS2},
    {307200000, NOM},

};

// MIDI_CORE frequency plans in Hz
// None

// AVSYNC_XO frequency plans in Hz
// None

// AVSYNC_BT frequency plans in Hz
// None

// AVSYNC_FM frequency plans in Hz
// None

// SLIMBUS_CORE frequency plans in Hz
const AsicInfo_ClockVoltageType SLIMBUS_CORE_8994[] =
{
    { 24576000, NOM}
};

const AsicInfo_ClockVoltageType SLIMBUS_CORE_8992[] =
{
    { 24576000, NOM}
};

const AsicInfo_ClockVoltageType SLIMBUS_CORE_8996[] =
{
    { 24576000, NOM}
};


const AsicInfo_ClockVoltageType SLIMBUS_CORE_845[] =
{
    {24576000, SVS2   }
};


// AVTIMER_CORE frequency plans in Hz
const AsicInfo_ClockVoltageType AVTIMER_CORE_8994[] =
{
    { 19200000, SVS2}
};

const AsicInfo_ClockVoltageType AVTIMER_CORE_8992[] =
{
    { 19200000, SVS2}
};

const AsicInfo_ClockVoltageType AVTIMER_CORE_8996[] =
{
    { 19200000, SVS2}
};

const AsicInfo_ClockVoltageType AVTIMER_CORE_845[] =
{
    {19200000, SVS2  }
};


// ATIME_CORE frequency plans in Hz
const AsicInfo_ClockVoltageType ATIME_CORE_8996[] =
{
    { 19200000, SVS2},
    { 30720000, SVS2},
    { 61440000, SVS },
    {122880000, NOM }
};

const AsicInfo_ClockVoltageType ATIME_CORE_845[] =
{
    { 19200000, MIN_SVS},
    { 30720000, SVS},
    { 61440000, SVS2 },
    {122880000, NOM }
};

const AsicInfo_ClockVoltageType ATIME_CORE_670[] =
{
    { 15360000, MIN_SVS},
    { 30720000, SVS},
    { 61440000, SVS2 },
    {122880000, NOM }
};

// ATIME2_CORE frequency plans in Hz
// None

// ADSP_CORE frequency plans in Hz
const AsicInfo_ClockVoltageType ADSP_CORE_8994[] =
{
    { 19200000, SVS2 },
    {124800000, SVS2 },
    {249600000, SVS2 },
    {441600000, SVS  },
    {595200000, NOM  },
    {748800000, TURBO}
};

const AsicInfo_ClockVoltageType ADSP_CORE_8992[] =
{
    { 19200000, SVS2 },
    {172800000, SVS2 },
    {355200000, SVS  },
    {523010000, SVS  },
    {710400000, NOM  },
    {850980000, TURBO}
};

const AsicInfo_ClockVoltageType ADSP_CORE_8996[] =
{
    { 19200000, SVS2 },
    {124800000, SVS2 },
    {249600000, SVS2 },
    {336000000, SVS2 },
    {499200000, SVS  },
    {748800000, NOM  },
    {825600000, TURBO}
};

const AsicInfo_ClockVoltageType ADSP_CORE_845[] =
{
    {288000000,  MIN_SVS},
    {460800000,  LOW_SVS},
    {614400000,  SVS},
    {787200000,  SVS2},
    {940800000,  NOM },
    {1190400000, TURBO},
};

const AsicInfo_ClockVoltageType ADSP_CORE_670[] =
{
    {300000000,  MIN_SVS},
    {400000000,  LOW_SVS},
    {600000000,  SVS},
    {940800000,  NOM },
    {1190400000, TURBO},
};

// AHB_ROOT frequency plans in Hz
const AsicInfo_ClockVoltageType AHB_ROOT_8994[] =
{
    { 19200000, SVS2},
    { 51200000, SVS2},
    {102400000, NOM },
    {136533333, NOM }
};

const AsicInfo_ClockVoltageType AHB_ROOT_8992[] =
{
    { 19200000, SVS2},
    { 51200000, SVS2},
    {102400000, NOM },
    {136533333, NOM }
};

const AsicInfo_ClockVoltageType AHB_ROOT_8996[] =
{
    { 19200000, SVS2},
    { 38400000, SVS2},
    { 76800000, SVS },
    {153600000, NOM }

};

const AsicInfo_ClockVoltageType AHB_ROOT_845[] =
{
    { 19200000, MIN_SVS},
    { 38400000, SVS},
    { 76800000, SVS2 },
    {153600000, NOM },
    {153600000, TURBO},
};

const AsicInfo_ClockVoltageType AHB_ROOT_670[] =
{
    { 25600000, MIN_SVS},
    { 51200000, SVS},
    {102400000, SVS2},
	{204800000, NOM},
    {204800000, TURBO},

};

const AsicInfo_ClockFreqVoltPlanType LpassClockFreqVoltPlan[MMPM_CLK_ID_LPASS_ENUM_MAX][NUM_MAX_TARGETS] =
{
    // MMPM_CLK_ID_LPASS_NONE
    {
        {0, NULL},
        {0, NULL},
        {0, NULL},
        {0, NULL}
    },
    // MMPM_CLK_ID_LPASS_HWRSP_CORE
    {
        {sizeof(HWRSP_CORE_8994)/sizeof(HWRSP_CORE_8994[0]), &(HWRSP_CORE_8994[0])},
        {sizeof(HWRSP_CORE_8992)/sizeof(HWRSP_CORE_8992[0]), &(HWRSP_CORE_8992[0])},
        {sizeof(HWRSP_CORE_8996)/sizeof(HWRSP_CORE_8996[0]), &(HWRSP_CORE_8996[0])},
        {sizeof(HWRSP_CORE_845)/sizeof(HWRSP_CORE_845[0]), &(HWRSP_CORE_845[0])},
		{sizeof(HWRSP_CORE_845)/sizeof(HWRSP_CORE_845[0]), &(HWRSP_CORE_845[0])}
    },
    // MMPM_CLK_ID_LPASS_MIDI_CORE
    {
        {0, NULL},
        {0, NULL},
        {0, NULL},
        {0, NULL},
        {0, NULL}
    },
    // MMPM_CLK_ID_LPASS_AVSYNC_XO
    {
        {0, NULL},
        {0, NULL},
        {0, NULL},
        {0, NULL},
		{0, NULL}
    },
    // MMPM_CLK_ID_LPASS_AVSYNC_BT
    {
        {0, NULL},
        {0, NULL},
        {0, NULL},
        {0, NULL},
		{0, NULL}
    },
    // MMPM_CLK_ID_LPASS_AVSYNC_FM
    {
        {0, NULL},
        {0, NULL},
        {0, NULL},
        {0, NULL},
		{0, NULL}
    },
    // MMPM_CLK_ID_LPASS_SLIMBUS_CORE
    {
        {sizeof(SLIMBUS_CORE_8994)/sizeof(SLIMBUS_CORE_8994[0]), &(SLIMBUS_CORE_8994[0])},
        {sizeof(SLIMBUS_CORE_8992)/sizeof(SLIMBUS_CORE_8992[0]), &(SLIMBUS_CORE_8992[0])},
        {sizeof(SLIMBUS_CORE_8996)/sizeof(SLIMBUS_CORE_8996[0]), &(SLIMBUS_CORE_8996[0])},
        {sizeof(SLIMBUS_CORE_845)/sizeof(SLIMBUS_CORE_845[0]), &(SLIMBUS_CORE_845[0])},
		{sizeof(SLIMBUS_CORE_845)/sizeof(SLIMBUS_CORE_845[0]), &(SLIMBUS_CORE_845[0])}
    },
    // MMPM_CLK_ID_LPASS_AVTIMER_CORE
    {
        {sizeof(AVTIMER_CORE_8994)/sizeof(AVTIMER_CORE_8994[0]), &(AVTIMER_CORE_8994[0])},
        {sizeof(AVTIMER_CORE_8992)/sizeof(AVTIMER_CORE_8992[0]), &(AVTIMER_CORE_8992[0])},
        {sizeof(AVTIMER_CORE_8996)/sizeof(AVTIMER_CORE_8996[0]), &(AVTIMER_CORE_8996[0])},
        {sizeof(AVTIMER_CORE_845)/sizeof(AVTIMER_CORE_845[0]), &(AVTIMER_CORE_845[0])},
		{0, NULL}
    },
    // MMPM_CLK_ID_LPASS_ATIME_CORE
    {
        {0, NULL},
        {0, NULL},
        {sizeof(ATIME_CORE_8996)/sizeof(ATIME_CORE_8996[0]), &(ATIME_CORE_8996[0])},
        {sizeof(ATIME_CORE_845)/sizeof(ATIME_CORE_845[0]), &(ATIME_CORE_845[0])},
		{sizeof(ATIME_CORE_670)/sizeof(ATIME_CORE_670[0]), &(ATIME_CORE_670[0])}
    },
    // MMPM_CLK_ID_LPASS_ATIME2_CORE
    {
        {0, NULL},
        {0, NULL},
        {0, NULL},
        {0, NULL},
        {0, NULL}
    },
    // MMPM_CLK_ID_LPASS_ADSP_CORE
    {
        {sizeof(ADSP_CORE_8994)/sizeof(ADSP_CORE_8994[0]), &(ADSP_CORE_8994[0])},
        {sizeof(ADSP_CORE_8992)/sizeof(ADSP_CORE_8992[0]), &(ADSP_CORE_8992[0])},
        {sizeof(ADSP_CORE_8996)/sizeof(ADSP_CORE_8996[0]), &(ADSP_CORE_8996[0])},
        {sizeof(ADSP_CORE_845)/sizeof(ADSP_CORE_845[0]), &(ADSP_CORE_845[0])},
		{sizeof(ADSP_CORE_670)/sizeof(ADSP_CORE_670[0]), &(ADSP_CORE_670[0])}
    },
    // MMPM_CLK_ID_LPASS_AHB_ROOT
    {
        {sizeof(AHB_ROOT_8994)/sizeof(AHB_ROOT_8994[0]), &(AHB_ROOT_8994[0])},
        {sizeof(AHB_ROOT_8992)/sizeof(AHB_ROOT_8992[0]), &(AHB_ROOT_8992[0])},
        {sizeof(AHB_ROOT_8996)/sizeof(AHB_ROOT_8996[0]), &(AHB_ROOT_8996[0])},
        {sizeof(AHB_ROOT_845)/sizeof(AHB_ROOT_845[0]), &(AHB_ROOT_845[0])},
		{sizeof(AHB_ROOT_670)/sizeof(AHB_ROOT_670[0]), &(AHB_ROOT_670[0])}
    }
};

const AsicInfo_MipsInfoType MipsInfo[NUM_MAX_TARGETS] =
{   //mips2clockFactor, number of threads
    {256, 3},
    {256, 3},
    {256, 4},
    {256, 2},
	{256, 2}
};

/* This table should match with that defined in Config file for each target */
const AsicInfo_CompensatedDdrBwTable compensatedDdrBw_table_8994[] =
{
        //        bwThreshold,            bwVote,   latencyVote,  snocFloorVote,    honestFlag
        {            10000000,          84480000,            0,               0,           0 },
        {            60000000,         440000000,            0,               0,           0 },
        {           120000000,                -1,          450,             100,           1 },
        {           155000000,                -1,          300,             160,           1 },
        {           250000000,                -1,          300,             200,           1 },
        {          ULLONG_MAX,                -1,          300,             266,           1 }
};
/* This table should match with that defined in Config file for each target */
const AsicInfo_CompensatedDdrBwTable compensatedDdrBw_table_8992[] =
{
        //        bwThreshold,            bwVote,   latencyVote,  snocFloorVote,    honestFlag
        {            10000000,          84480000,            0,               0,           0 },
        {            60000000,         440000000,            0,               0,           0 },
        {           120000000,                -1,          450,             100,           1 },
        {           155000000,                -1,          300,             160,           1 },
        {           250000000,                -1,          300,             200,           1 },
        {          ULLONG_MAX,                -1,          300,             266,           1 }
};
/* This table should match with that defined in Config file for each target */
const AsicInfo_CompensatedDdrBwTable compensatedDdrBw_table_8996[] =
{
        //        bwThreshold,            bwVote,   latencyVote,  snocFloorVote,    honestFlag
        {            10000000,          84480000,            0,               0,           0 },
        {            60000000,         440000000,            0,               0,           0 },
        {           120000000,                -1,          450,             100,           1 },
        {           155000000,                -1,          300,             160,           1 },
        {           250000000,                -1,          300,             200,           1 },
        {          ULLONG_MAX,                -1,          300,             266,           1 }
};

const AsicInfo_CompensatedDdrBwTable compensatedDdrBw_table_845[] =
{
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


const AsicInfo_CompensatedDdrBwTableType compensatedDdrBw_table[NUM_MAX_TARGETS] =
{
    {sizeof(compensatedDdrBw_table_8994)/sizeof(compensatedDdrBw_table_8994[0]),&(compensatedDdrBw_table_8994[0])},
    {sizeof(compensatedDdrBw_table_8992)/sizeof(compensatedDdrBw_table_8992[0]),&(compensatedDdrBw_table_8992[0])},
    {sizeof(compensatedDdrBw_table_8996)/sizeof(compensatedDdrBw_table_8996[0]),&(compensatedDdrBw_table_8996[0])},
    {sizeof(compensatedDdrBw_table_845)/sizeof(compensatedDdrBw_table_845[0]),&(compensatedDdrBw_table_845[0])},
	{sizeof(compensatedDdrBw_table_845)/sizeof(compensatedDdrBw_table_845[0]),&(compensatedDdrBw_table_845[0])}, /* 670 */
};

const AsicInfo_BwConcurrencySettingsTable concurrencyTable_8994 =
{
    3,      /* adspDdrConcurrencyVotersThreshold*/
    {
        256,    /* adspDdrConcurrencyFactor = 1*/
        384,    /* adspDdrConcurrencyFactor = 1.5*/
        512,    /* adspDdrConcurrencyFactor = 2*/
    }
};

const AsicInfo_BwConcurrencySettingsTable concurrencyTable_8992 =
{
    3,      /* adspDdrConcurrencyVotersThreshold*/
    {
        256,    /* adspDdrConcurrencyFactor = 1*/
        384,    /* adspDdrConcurrencyFactor = 1.5*/
        512,    /* adspDdrConcurrencyFactor = 2*/
    }
};

const AsicInfo_BwConcurrencySettingsTable concurrencyTable_8996 =
{
    3,      /* adspDdrConcurrencyVotersThreshold*/
    {
        256,    /* adspDdrConcurrencyFactor = 1*/
        384,    /* adspDdrConcurrencyFactor = 1.5*/
        512,    /* adspDdrConcurrencyFactor = 2*/
    }
};

const AsicInfo_BwConcurrencySettingsTable concurrencyTable_845 =
{
    3,      /* adspDdrConcurrencyVotersThreshold*/
    {
        256,    /* adspDdrConcurrencyFactor = 1*/
        384,    /* adspDdrConcurrencyFactor = 1.5*/
        512,    /* adspDdrConcurrencyFactor = 2*/
    }
};

const AsicInfo_BwConcurrencySettingsTableType bwConcurrencySettings_table[NUM_MAX_TARGETS] =
{
    {&concurrencyTable_8994},
    {&concurrencyTable_8992},
    {&concurrencyTable_8996},
    {&concurrencyTable_845},
	{&concurrencyTable_845},
};

/* This table should match with that defined in Config file for each target */
const AsicInfo_CompensatedAhbBwTableEntry compensatedAhbBw_table_8994[] =
{
    // bwThreshold,            bwVote
    {   ULLONG_MAX,                -1 },
};
/* This table should match with that defined in Config file for each target */
const AsicInfo_CompensatedAhbBwTableEntry compensatedAhbBw_table_8992[] =
{
    // bwThreshold,            bwVote
    {   ULLONG_MAX,                -1 },
};
/* This table should match with that defined in Config file for each target */
const AsicInfo_CompensatedAhbBwTableEntry compensatedAhbBw_table_8996[] =
{
    // bwThreshold,            bwVote
    {     39137222,          61440000 }, // target SVS2  19.2 Mhz
    {     78274443,         122880000 }, // target SVS2  38.4 Mhz
    {    156548887,         245760000 }, // target SVS   76.8 Mhz
    {    490240000,         490240000 }, // target NOM  153.2 Mhz
    {   ULLONG_MAX,                -1 },
};

/* This table should match with that defined in Config file for each target */
const AsicInfo_CompensatedAhbBwTableEntry compensatedAhbBw_table_845[] =
{

    // bwThreshold,            bwVote
    {      9784305,          61440000 }, // target SVS2  19.2 Mhz
    {     19568611,         122880000 }, // target SVS2  38.4 Mhz
    {     39137222,         245760000 }, // target SVS   76.8 Mhz
    {    490240000,         490240000 }, // target NOM  153.2 Mh
    {   ULLONG_MAX,                -1 },

};

const AsicInfo_CompensatedAhbBwTableType compensatedAhbBw_table[NUM_MAX_TARGETS] =
{
    {sizeof(compensatedAhbBw_table_8994)/sizeof(compensatedAhbBw_table_8994[0]),&(compensatedAhbBw_table_8994[0])},
    {sizeof(compensatedAhbBw_table_8992)/sizeof(compensatedAhbBw_table_8992[0]),&(compensatedAhbBw_table_8992[0])},
    {sizeof(compensatedAhbBw_table_8996)/sizeof(compensatedAhbBw_table_8996[0]),&(compensatedAhbBw_table_8996[0])},
    {sizeof(compensatedAhbBw_table_845)/sizeof(compensatedAhbBw_table_845[0]),&(compensatedAhbBw_table_845[0])},
	{sizeof(compensatedAhbBw_table_845)/sizeof(compensatedAhbBw_table_845[0]),&(compensatedAhbBw_table_845[0])}, /* sdm670 */
};

// Check core instance availability
boolean AsicInfo_isCoreInstanceAvailable(MmpmCoreIdType coreId, MmpmCoreInstanceIdType instanceId)
{
    boolean result = FALSE;
    MmpmCoreInstanceIdType MaxAvailCoreInstance  = MMPM_CORE_INSTANCE_NONE;
    if((coreId > MMPM_CORE_ID_LPASS_START) && (coreId < MMPM_CORE_ID_LPASS_END) &&
        (instanceId > MMPM_CORE_INSTANCE_NONE) && (instanceId < MMPM_CORE_INSTANCE_MAX))
    {
        MaxAvailCoreInstance =
            AdsppmTestAvailCoresPerTarget[coreId - MMPM_CORE_ID_LPASS_START][GetTestTarget()];
        result = (MaxAvailCoreInstance >= instanceId);
    }
    if(!result)
    {
        ADSPPMTEST_LOG_PRINTF_2(ADSPPMTEST_LOG_LEVEL_INFO,
            "[N/A] coreId %u instanceId %u not applicable in this chip set", coreId, instanceId);
    }
    return result;
}


// Check core clock availability
boolean AsicInfo_isCoreClockAvailable(MmpmClkIdLpassType coreClkId)
{
    boolean result = FALSE;;

    if (ACM_GetFeatureStatus(AsicFeatureId_ComputeDSP) == AsicFeatureState_Enabled)
    {
        if (coreClkId > MMPM_CLK_ID_COMPUTE_NONE &&
            coreClkId < MMPM_CLK_ID_COMPUTE_ENUM_MAX)
        {
            result = TRUE;
        }
    }
    else if (ACM_GetFeatureStatus(AsicFeatureId_SlpiDSP) == AsicFeatureState_Enabled)
	/* (ACM_GetFeatureStatus(AsicFeatureId_LpassSlpiDSP) == AsicFeatureState_Enabled) */
	{
        if (coreClkId > MMPM_CLK_ID_SLPI_NONE &&
            coreClkId < MMPM_CLK_ID_SLPI_ENUM_MAX)
        {
            result = TRUE;
        }
    }

    else if ((coreClkId < MMPM_CLK_ID_LPASS_ADSP_CORE) &&
         (LpassClockFreqVoltPlan[coreClkId][GetTestTarget()].numFreq > 0))
    {
        result = TRUE;
    }

    if(!result)
    {
        ADSPPMTEST_LOG_PRINTF_1(ADSPPMTEST_LOG_LEVEL_INFO,
            "[N/A] coreClkId %u not applicable in this chip set", coreClkId);
    }
    return result;
}


// Check bus port availability
boolean AsicInfo_isBusPortAvailable(MmpmBwPortIdType busPortId)
{
    boolean result = FALSE;
    if(busPortId < MMPM_BW_PORT_ID_CORE)
    {
        result = AdsppmTestAvailBusPortsPerTarget[busPortId][GetTestTarget()];
    }
    if(!result)
    {
        ADSPPMTEST_LOG_PRINTF_1(ADSPPMTEST_LOG_LEVEL_INFO,
            "[N/A] busPortId %u not applicable in this chip set", busPortId);
    }
    return result;
}


// Check memory availability
boolean AsicInfo_isMemoryAvailable(MmpmMemIdType memId)
{
    boolean result = FALSE;
    if(memId < MMPM_MEM_MAX)
    {
        result = AdsppmTestAvailMemoriesPerTarget[memId][GetTestTarget()];
    }
    if(!result)
    {
        ADSPPMTEST_LOG_PRINTF_1(ADSPPMTEST_LOG_LEVEL_INFO,
            "[N/A] memId %u not applicable in this chip set", memId);
    }
    return result;
}


// Provide clock frequency (in Hz) voltage level plan
const AsicInfo_ClockFreqVoltPlanType *AsicInfo_getClockFreqVoltPlan(MmpmClkIdLpassType clkId)
{
    return &(LpassClockFreqVoltPlan[clkId][GetTestTarget()]);
}


// Provide MIPS related information
const AsicInfo_MipsInfoType *AsicInfo_getMipsInfo(void)
{
    return &(MipsInfo[GetTestTarget()]);
}

// Provide Compensated DDR BW Table
const AsicInfo_CompensatedDdrBwTableType *AsicInfo_getCompensatedDdrBwInfo(void)
{
    return &(compensatedDdrBw_table[GetTestTarget()]);
}

// Provide BW Concurrency Settings Table
const AsicInfo_BwConcurrencySettingsTableType *AsicInfo_getBwConcurrencyInfo(void)
{
    return &(bwConcurrencySettings_table[GetTestTarget()]);
}

// Provide Compensated AHB BW Table
const AsicInfo_CompensatedAhbBwTableType *AsicInfo_getCompensatedAhbBwInfo(void)
{
    return &(compensatedAhbBw_table[GetTestTarget()]);
}

