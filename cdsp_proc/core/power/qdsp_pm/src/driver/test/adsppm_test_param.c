/*
* Copyright (c) 2014 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

#include "adsppm_test.h"
#include "adsppm_test_param.h"
#include "adsppm_test_utils.h"
#include "adsppm_test_asic_info.h"


#define NUMLPASSCOREIDS MMPM_CORE_ID_LPASS_END - MMPM_CORE_ID_LPASS_START + 1


const MmpmReqClockTestParamTableType gReqClkParam[NUMLPASSCOREIDS] =
{
    {}, // NONE
    {}, // ADSP
    {}, // CORE
    {}, // LPM
    {}, // DML
    {}, // AIF
    {1, // SLIMBUS
        {
            // clkId,                        freq in Khz, freqMatch
            {MMPM_CLK_ID_LPASS_SLIMBUS_CORE, 24576,       MMPM_FREQ_AT_LEAST}
        }
    },
    {1, // MIDI
        {
            // clkId,                        freq in Khz, freqMatch
            {MMPM_CLK_ID_LPASS_MIDI_CORE,    122000,      MMPM_FREQ_AT_LEAST}
        }
    },
    {5, // AVSYNC
        {
             // clkId,                       freq in Khz, freqMatch
            {MMPM_CLK_ID_LPASS_AVSYNC_XO,    20000,       MMPM_FREQ_AT_LEAST},
            {MMPM_CLK_ID_LPASS_AVSYNC_BT,    27000,       MMPM_FREQ_AT_LEAST},
            {MMPM_CLK_ID_LPASS_AVSYNC_FM,    20000,       MMPM_FREQ_AT_LEAST},
            {MMPM_CLK_ID_LPASS_AVTIMER_CORE, 20000,       MMPM_FREQ_AT_LEAST},
            {MMPM_CLK_ID_LPASS_ATIME_CORE,   20000,       MMPM_FREQ_AT_LEAST}
        }
    },
    {1, // HWRSP
        {
            // clkId,                        freq in Khz, freqMatch
            {MMPM_CLK_ID_LPASS_HWRSP_CORE,   122000,      MMPM_FREQ_AT_LEAST}
        }
    },
    {}, // SRAM
    {}, // DCODEC
    {}, // SPDIF
    {}, // HDMIRX
    {}, // HDMITX
    {}, // SIF
    {}, // BSTC
    {}, // HVX
    {}, //DPAudio
};

const MmpmReqClockDomainTestParamTableType gReqClkDomainParam[NUMLPASSCOREIDS] =
{
    {}, // NONE
    {}, // ADSP
    {}, // CORE
    {}, // LPM
    {}, // DML
    {}, // AIF
    {}, // SLIMBUS
    {}, // MIDI
    {5, // AVSYNC
        {
             // clkId,                       freq in Khz, clkDomainSrc
            {MMPM_CLK_ID_LPASS_AVSYNC_XO,    20000,       1},
            {MMPM_CLK_ID_LPASS_AVSYNC_BT,    27000,       2},
            {MMPM_CLK_ID_LPASS_AVSYNC_FM,    20000,       1},
            {MMPM_CLK_ID_LPASS_AVTIMER_CORE, 20000,       2},
            {MMPM_CLK_ID_LPASS_ATIME_CORE,   20000,       2}
        }
    },
    {}, // HWRSP
    {}, // SRAM
    {}, // DCODEC
    {}, // SPDIF
    {}, // HDMIRX
    {}, // HDMITX
    {}, // SIF
    {}, // BSTC
    {}, // HVX
    {}, //DPAudio
};

// Request MIPS parameter table
const uint32 gReqMipsParam[NUMLPASSCOREIDS] =
{
    // mips
    0,   // NONE
    60,  // ADSP
    60,  // CORE
    80,  // LPM
    90,  // DML
    170, // AIF
    20,  // SLIMBUS
    30,  // MIDI
    210, // AVSYNC
    200, // HWRSP
    100, // SRAM
    50,  // DCODEC
    50,  // SPDIF
    30,  // HDMIRX
    30,  // HDMITX
    20,  // SIF
    10,  // BSTC
    10,  // HVX
    20   //DPaudio
};

// Request register programming parameters table
const uint32 gReqRegProgParam[NUMLPASSCOREIDS] =
{
    // Reg Prog Match
    0,                  // NONE
    MMPM_REG_PROG_FAST, // ADSP
    MMPM_REG_PROG_NORM, // CORE
    MMPM_REG_PROG_NORM, // LPM
    MMPM_REG_PROG_FAST, // DML
    MMPM_REG_PROG_NORM, // AIF
    MMPM_REG_PROG_NORM, // SLIMBUS
    MMPM_REG_PROG_NORM, // MIDI
    MMPM_REG_PROG_NORM, // AVSYNC
    MMPM_REG_PROG_FAST, // HWRSP
    MMPM_REG_PROG_NORM, // SRAM
    MMPM_REG_PROG_NORM, // DCODEC
    MMPM_REG_PROG_NORM, // SPDIF
    MMPM_REG_PROG_NORM, // HDMIRX
    MMPM_REG_PROG_NORM, // HDMITX
    MMPM_REG_PROG_NORM, // SIF
    MMPM_REG_PROG_NORM, // BSTC
    MMPM_REG_PROG_NORM, // HVX
    MMPM_REG_PROG_NORM, // Dpaudio
};

const MmpmReqBandwidthTestParamTableType gReqBwParam[NUMLPASSCOREIDS] =
{
    // numBw
    {}, // NONE
    {8, // ADSP
        {
            // masterport,                slaveport,                      bwVal(bytePerSec), usagePercent, usageType
            {MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_DML_SLAVE,      60000000,          100,          MMPM_BW_USAGE_LPASS_DSP, },
            {MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_AIF_SLAVE,      60000000,          100,          MMPM_BW_USAGE_LPASS_DSP, },
            {MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_SLIMBUS_SLAVE,  60000000,          100,          MMPM_BW_USAGE_LPASS_DSP, },
            {MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_MIDI_SLAVE,     60000000,          100,          MMPM_BW_USAGE_LPASS_DSP, },
            {MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_HWRSMP_SLAVE,   60000000,          100,          MMPM_BW_USAGE_LPASS_DSP, },
            {MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_AVSYNC_SLAVE,   60000000,          100,          MMPM_BW_USAGE_LPASS_DSP, },
            {MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_LPM_SLAVE,      60000000,          100,          MMPM_BW_USAGE_LPASS_DSP, },
            {MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_SRAM_SLAVE,     60000000,          100,          MMPM_BW_USAGE_LPASS_DSP, },
        }
    },
    {}, // CORE
    {}, // LPM
    {8, // DML
        {
            // masterport,               slaveport,                       bwVal(bytePerSec), usagePercent, usageType
            {MMPM_BW_PORT_ID_DML_MASTER, MMPM_BW_PORT_ID_AIF_SLAVE,       60000000,          100,          MMPM_BW_USAGE_LPASS_DMA,},
            {MMPM_BW_PORT_ID_DML_MASTER, MMPM_BW_PORT_ID_SLIMBUS_SLAVE,   60000000,          100,          MMPM_BW_USAGE_LPASS_DMA,},
            {MMPM_BW_PORT_ID_DML_MASTER, MMPM_BW_PORT_ID_MIDI_SLAVE,      60000000,          100,          MMPM_BW_USAGE_LPASS_DMA,},
            {MMPM_BW_PORT_ID_DML_MASTER, MMPM_BW_PORT_ID_HWRSMP_SLAVE,    60000000,          100,          MMPM_BW_USAGE_LPASS_DMA,},
            {MMPM_BW_PORT_ID_DML_MASTER, MMPM_BW_PORT_ID_AVSYNC_SLAVE,    60000000,          100,          MMPM_BW_USAGE_LPASS_DMA,},
            {MMPM_BW_PORT_ID_DML_MASTER, MMPM_BW_PORT_ID_LPM_SLAVE,       60000000,          100,          MMPM_BW_USAGE_LPASS_DMA,},
            {MMPM_BW_PORT_ID_DML_MASTER, MMPM_BW_PORT_ID_SRAM_SLAVE,      60000000,          100,          MMPM_BW_USAGE_LPASS_DMA,},
            {MMPM_BW_PORT_ID_DML_MASTER, MMPM_BW_PORT_ID_SPDIF_SLAVE,     60000000,          100,          MMPM_BW_USAGE_LPASS_DMA,},
        }
    },
    {8, // AIF
        {
            // masterport,               slaveport,                       bwVal(bytePerSec), usagePercent, usageType
            {MMPM_BW_PORT_ID_AIF_MASTER, MMPM_BW_PORT_ID_DML_SLAVE,       60000000,          100,          MMPM_BW_USAGE_LPASS_DSP,},
            {MMPM_BW_PORT_ID_AIF_MASTER, MMPM_BW_PORT_ID_SLIMBUS_SLAVE,   60000000,          100,          MMPM_BW_USAGE_LPASS_DSP,},
            {MMPM_BW_PORT_ID_AIF_MASTER, MMPM_BW_PORT_ID_MIDI_SLAVE,      60000000,          100,          MMPM_BW_USAGE_LPASS_DSP,},
            {MMPM_BW_PORT_ID_AIF_MASTER, MMPM_BW_PORT_ID_HWRSMP_SLAVE,    60000000,          100,          MMPM_BW_USAGE_LPASS_DSP,},
            {MMPM_BW_PORT_ID_AIF_MASTER, MMPM_BW_PORT_ID_AVSYNC_SLAVE,    60000000,          100,          MMPM_BW_USAGE_LPASS_DSP,},
            {MMPM_BW_PORT_ID_AIF_MASTER, MMPM_BW_PORT_ID_LPM_SLAVE,       60000000,          100,          MMPM_BW_USAGE_LPASS_DSP,},
            {MMPM_BW_PORT_ID_AIF_MASTER, MMPM_BW_PORT_ID_SRAM_SLAVE,      60000000,          100,          MMPM_BW_USAGE_LPASS_DSP,},
            {MMPM_BW_PORT_ID_AIF_MASTER, MMPM_BW_PORT_ID_SPDIF_SLAVE,     60000000,          100,          MMPM_BW_USAGE_LPASS_DSP,},
        }
    },
    {8, // SLIMBUS
        {
            // masterport,                   slaveport,                       bwVal(bytePerSec), usagePercent, usageType
            {MMPM_BW_PORT_ID_SLIMBUS_MASTER, MMPM_BW_PORT_ID_DML_SLAVE,       60000000,          100,          MMPM_BW_USAGE_LPASS_DMA,},
            {MMPM_BW_PORT_ID_SLIMBUS_MASTER, MMPM_BW_PORT_ID_AIF_SLAVE,       60000000,          100,          MMPM_BW_USAGE_LPASS_DMA,},
            {MMPM_BW_PORT_ID_SLIMBUS_MASTER, MMPM_BW_PORT_ID_MIDI_SLAVE,      60000000,          100,          MMPM_BW_USAGE_LPASS_DMA,},
            {MMPM_BW_PORT_ID_SLIMBUS_MASTER, MMPM_BW_PORT_ID_HWRSMP_SLAVE,    60000000,          100,          MMPM_BW_USAGE_LPASS_DMA,},
            {MMPM_BW_PORT_ID_SLIMBUS_MASTER, MMPM_BW_PORT_ID_AVSYNC_SLAVE,    60000000,          100,          MMPM_BW_USAGE_LPASS_DMA,},
            {MMPM_BW_PORT_ID_SLIMBUS_MASTER, MMPM_BW_PORT_ID_LPM_SLAVE,       60000000,          100,          MMPM_BW_USAGE_LPASS_DMA,},
            {MMPM_BW_PORT_ID_SLIMBUS_MASTER, MMPM_BW_PORT_ID_SRAM_SLAVE,      60000000,          100,          MMPM_BW_USAGE_LPASS_DMA,},
            {MMPM_BW_PORT_ID_SLIMBUS_MASTER, MMPM_BW_PORT_ID_SPDIF_SLAVE,     60000000,          100,          MMPM_BW_USAGE_LPASS_DMA,},
        }
    },
    {8, // MIDI
        {
            // masterport,                slaveport,                       bwVal(bytePerSec), usagePercent, usageType
            {MMPM_BW_PORT_ID_MIDI_MASTER, MMPM_BW_PORT_ID_DML_SLAVE,       60000000,          100,          MMPM_BW_USAGE_LPASS_DSP, },
            {MMPM_BW_PORT_ID_MIDI_MASTER, MMPM_BW_PORT_ID_AIF_SLAVE,       60000000,          100,          MMPM_BW_USAGE_LPASS_DSP, },
            {MMPM_BW_PORT_ID_MIDI_MASTER, MMPM_BW_PORT_ID_SLIMBUS_SLAVE,   60000000,          100,          MMPM_BW_USAGE_LPASS_DSP, },
            {MMPM_BW_PORT_ID_MIDI_MASTER, MMPM_BW_PORT_ID_HWRSMP_SLAVE,    60000000,          100,          MMPM_BW_USAGE_LPASS_DSP, },
            {MMPM_BW_PORT_ID_MIDI_MASTER, MMPM_BW_PORT_ID_AVSYNC_SLAVE,    60000000,          100,          MMPM_BW_USAGE_LPASS_DSP, },
            {MMPM_BW_PORT_ID_MIDI_MASTER, MMPM_BW_PORT_ID_LPM_SLAVE,       60000000,          100,          MMPM_BW_USAGE_LPASS_DSP, },
            {MMPM_BW_PORT_ID_MIDI_MASTER, MMPM_BW_PORT_ID_SRAM_SLAVE,      60000000,          100,          MMPM_BW_USAGE_LPASS_DSP, },
            {MMPM_BW_PORT_ID_MIDI_MASTER, MMPM_BW_PORT_ID_SPDIF_SLAVE,     60000000,          100,          MMPM_BW_USAGE_LPASS_DSP, },
        }
    },
    {}, // AVSYNC
    {8, // HWRSMP
        {
            // masterport,                  slaveport,                       bwVal(bytePerSec), usagePercent, usageType
            {MMPM_BW_PORT_ID_HWRSMP_MASTER, MMPM_BW_PORT_ID_DML_SLAVE,       60000000,          100,          MMPM_BW_USAGE_LPASS_DSP,},
            {MMPM_BW_PORT_ID_HWRSMP_MASTER, MMPM_BW_PORT_ID_AIF_SLAVE,       60000000,          100,          MMPM_BW_USAGE_LPASS_DSP,},
            {MMPM_BW_PORT_ID_HWRSMP_MASTER, MMPM_BW_PORT_ID_SLIMBUS_SLAVE,   60000000,          100,          MMPM_BW_USAGE_LPASS_DSP,},
            {MMPM_BW_PORT_ID_HWRSMP_MASTER, MMPM_BW_PORT_ID_MIDI_SLAVE,      60000000,          100,          MMPM_BW_USAGE_LPASS_DSP,},
            {MMPM_BW_PORT_ID_HWRSMP_MASTER, MMPM_BW_PORT_ID_AVSYNC_SLAVE,    60000000,          100,          MMPM_BW_USAGE_LPASS_DSP,},
            {MMPM_BW_PORT_ID_HWRSMP_MASTER, MMPM_BW_PORT_ID_LPM_SLAVE,       60000000,          100,          MMPM_BW_USAGE_LPASS_DSP,},
            {MMPM_BW_PORT_ID_HWRSMP_MASTER, MMPM_BW_PORT_ID_SRAM_SLAVE,      60000000,          100,          MMPM_BW_USAGE_LPASS_DSP,},
            {MMPM_BW_PORT_ID_HWRSMP_MASTER, MMPM_BW_PORT_ID_SPDIF_SLAVE,     60000000,          100,          MMPM_BW_USAGE_LPASS_DSP,},
        }
    },
    {}, // SRAM
    {}, // DCODEC
    {8, // SPDIF
        {
            // masterport,                 slaveport,                       bwVal(bytePerSec), usagePercent, usageType
            {MMPM_BW_PORT_ID_SPDIF_MASTER, MMPM_BW_PORT_ID_DML_SLAVE,       60000000,          100,          MMPM_BW_USAGE_LPASS_DMA, },
            {MMPM_BW_PORT_ID_SPDIF_MASTER, MMPM_BW_PORT_ID_AIF_SLAVE,       60000000,          100,          MMPM_BW_USAGE_LPASS_DMA, },
            {MMPM_BW_PORT_ID_SPDIF_MASTER, MMPM_BW_PORT_ID_SLIMBUS_SLAVE,   60000000,          100,          MMPM_BW_USAGE_LPASS_DSP, },
            {MMPM_BW_PORT_ID_SPDIF_MASTER, MMPM_BW_PORT_ID_MIDI_SLAVE,      60000000,          100,          MMPM_BW_USAGE_LPASS_DMA, },
            {MMPM_BW_PORT_ID_SPDIF_MASTER, MMPM_BW_PORT_ID_HWRSMP_SLAVE,    60000000,          100,          MMPM_BW_USAGE_LPASS_DMA, },
            {MMPM_BW_PORT_ID_SPDIF_MASTER, MMPM_BW_PORT_ID_AVSYNC_SLAVE,    60000000,          100,          MMPM_BW_USAGE_LPASS_DMA, },
            {MMPM_BW_PORT_ID_SPDIF_MASTER, MMPM_BW_PORT_ID_LPM_SLAVE,       60000000,          100,          MMPM_BW_USAGE_LPASS_DMA, },
            {MMPM_BW_PORT_ID_SPDIF_MASTER, MMPM_BW_PORT_ID_SRAM_SLAVE,      60000000,          100,          MMPM_BW_USAGE_LPASS_DMA, }
        }
    },
    {8, // HDMIRX
        {
            // masterport,                  slaveport,                      bwVal(bytePerSec), usagePercent, usageType
            {MMPM_BW_PORT_ID_HDMIRX_MASTER, MMPM_BW_PORT_ID_DML_SLAVE,      60000000,          100,          MMPM_BW_USAGE_LPASS_DMA, },
            {MMPM_BW_PORT_ID_HDMIRX_MASTER, MMPM_BW_PORT_ID_AIF_SLAVE,      60000000,          100,          MMPM_BW_USAGE_LPASS_DMA, },
            {MMPM_BW_PORT_ID_HDMIRX_MASTER, MMPM_BW_PORT_ID_MIDI_SLAVE,     60000000,          100,          MMPM_BW_USAGE_LPASS_DMA, },
            {MMPM_BW_PORT_ID_HDMIRX_MASTER, MMPM_BW_PORT_ID_HWRSMP_SLAVE,   60000000,          100,          MMPM_BW_USAGE_LPASS_DMA, },
            {MMPM_BW_PORT_ID_HDMIRX_MASTER, MMPM_BW_PORT_ID_AVSYNC_SLAVE,   60000000,          100,          MMPM_BW_USAGE_LPASS_DMA, },
            {MMPM_BW_PORT_ID_HDMIRX_MASTER, MMPM_BW_PORT_ID_LPM_SLAVE,      60000000,          100,          MMPM_BW_USAGE_LPASS_DMA, },
            {MMPM_BW_PORT_ID_HDMIRX_MASTER, MMPM_BW_PORT_ID_SRAM_SLAVE,     60000000,          100,          MMPM_BW_USAGE_LPASS_DMA, },
            {MMPM_BW_PORT_ID_HDMIRX_MASTER, MMPM_BW_PORT_ID_SPDIF_SLAVE,    60000000,          100,          MMPM_BW_USAGE_LPASS_DMA, },
        }
    },
    {8, // HDMITX
        {
            // masterport,                  slaveport,                      bwVal(bytePerSec), usagePercent, usageType
            {MMPM_BW_PORT_ID_HDMITX_MASTER, MMPM_BW_PORT_ID_DML_SLAVE,      60000000,          100,          MMPM_BW_USAGE_LPASS_DMA, },
            {MMPM_BW_PORT_ID_HDMITX_MASTER, MMPM_BW_PORT_ID_AIF_SLAVE,      60000000,          100,          MMPM_BW_USAGE_LPASS_DMA, },
            {MMPM_BW_PORT_ID_HDMITX_MASTER, MMPM_BW_PORT_ID_MIDI_SLAVE,     60000000,          100,          MMPM_BW_USAGE_LPASS_DMA, },
            {MMPM_BW_PORT_ID_HDMITX_MASTER, MMPM_BW_PORT_ID_HWRSMP_SLAVE,   60000000,          100,          MMPM_BW_USAGE_LPASS_DMA, },
            {MMPM_BW_PORT_ID_HDMITX_MASTER, MMPM_BW_PORT_ID_AVSYNC_SLAVE,   60000000,          100,          MMPM_BW_USAGE_LPASS_DMA, },
            {MMPM_BW_PORT_ID_HDMITX_MASTER, MMPM_BW_PORT_ID_LPM_SLAVE,      60000000,          100,          MMPM_BW_USAGE_LPASS_DMA, },
            {MMPM_BW_PORT_ID_HDMITX_MASTER, MMPM_BW_PORT_ID_SRAM_SLAVE,     60000000,          100,          MMPM_BW_USAGE_LPASS_DMA, },
            {MMPM_BW_PORT_ID_HDMITX_MASTER, MMPM_BW_PORT_ID_SPDIF_SLAVE,    60000000,          100,          MMPM_BW_USAGE_LPASS_DMA, },
        }
    },
    {8, // SIF
        {
            // masterport,               slaveport,                         bwVal(bytePerSec), usagePercent, usageType
            {MMPM_BW_PORT_ID_SIF_MASTER, MMPM_BW_PORT_ID_DML_SLAVE,         60000000,          100,          MMPM_BW_USAGE_LPASS_DMA, },
            {MMPM_BW_PORT_ID_SIF_MASTER, MMPM_BW_PORT_ID_AIF_SLAVE,         60000000,          100,          MMPM_BW_USAGE_LPASS_DMA, },
            {MMPM_BW_PORT_ID_SIF_MASTER, MMPM_BW_PORT_ID_MIDI_SLAVE,        60000000,          100,          MMPM_BW_USAGE_LPASS_DMA, },
            {MMPM_BW_PORT_ID_SIF_MASTER, MMPM_BW_PORT_ID_HWRSMP_SLAVE,      60000000,          100,          MMPM_BW_USAGE_LPASS_DMA, },
            {MMPM_BW_PORT_ID_SIF_MASTER, MMPM_BW_PORT_ID_AVSYNC_SLAVE,      60000000,          100,          MMPM_BW_USAGE_LPASS_DMA, },
            {MMPM_BW_PORT_ID_SIF_MASTER, MMPM_BW_PORT_ID_LPM_SLAVE,         60000000,          100,          MMPM_BW_USAGE_LPASS_DMA, },
            {MMPM_BW_PORT_ID_SIF_MASTER, MMPM_BW_PORT_ID_SRAM_SLAVE,        60000000,          100,          MMPM_BW_USAGE_LPASS_DMA, },
            {MMPM_BW_PORT_ID_SIF_MASTER, MMPM_BW_PORT_ID_SPDIF_SLAVE,       60000000,          100,          MMPM_BW_USAGE_LPASS_DMA, },
        }
    },
    {}, // BSTC
    {}  // HVX
};

const MmpmReqBandwidthTestParamTableTypeExt gReqDdrBwParam[2] =
{

    {6,
        {
            {MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_DDR_SLAVE,         Bw_Generic_Ext,          5000000,           100,          MMPM_BW_USAGE_LPASS_DSP,},
            {MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_DDR_SLAVE,         Bw_Generic_Ext,         20000000,           100,          MMPM_BW_USAGE_LPASS_DSP,},
            {MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_DDR_SLAVE,         Bw_Generic_Ext,         25000000,           100,          MMPM_BW_USAGE_LPASS_DSP,},
            {MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_DDR_SLAVE,         Bw_Generic_Ext,         10000000,           100,          MMPM_BW_USAGE_LPASS_DSP,},
            {MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_DDR_SLAVE,         Bw_Generic,             60000000,           100,          MMPM_BW_USAGE_LPASS_DSP,},
            {MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_DDR_SLAVE,         Bw_Generic,             60000000,           100,          MMPM_BW_USAGE_LPASS_DSP,},
        }
    },
    {6,
        {
            {MMPM_BW_PORT_ID_EXT_AHB_MASTER, MMPM_BW_PORT_ID_DDR_SLAVE,      Bw_Generic_Ext,          5000000,            100,          MMPM_BW_USAGE_LPASS_DSP,},
            {MMPM_BW_PORT_ID_EXT_AHB_MASTER, MMPM_BW_PORT_ID_DDR_SLAVE,      Bw_Generic_Ext,         20000000,            100,          MMPM_BW_USAGE_LPASS_DSP,},
            {MMPM_BW_PORT_ID_EXT_AHB_MASTER, MMPM_BW_PORT_ID_DDR_SLAVE,      Bw_Generic_Ext,         80000000,            100,          MMPM_BW_USAGE_LPASS_DSP,},
            {MMPM_BW_PORT_ID_EXT_AHB_MASTER, MMPM_BW_PORT_ID_DDR_SLAVE,      Bw_Generic_Ext,        240000000,            100,          MMPM_BW_USAGE_LPASS_DSP,},
            {MMPM_BW_PORT_ID_EXT_AHB_MASTER, MMPM_BW_PORT_ID_DDR_SLAVE,      Bw_Generic,             60000000,            100,          MMPM_BW_USAGE_LPASS_DSP,},
            {MMPM_BW_PORT_ID_EXT_AHB_MASTER, MMPM_BW_PORT_ID_DDR_SLAVE,      Bw_Generic,             60000000,            100,          MMPM_BW_USAGE_LPASS_DSP,},
        }
    }
};

const MmpmReqBandwidthTestParamTableTypeExt gReqInternalBwParam[2] =
{

    {6,
        {
            {MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_LPM_SLAVE,         Bw_Generic_Ext,          5000000,           100,          MMPM_BW_USAGE_LPASS_DSP, },
            {MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_LPM_SLAVE,         Bw_Generic_Ext,         20000000,           100,          MMPM_BW_USAGE_LPASS_DSP, },
            {MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_LPM_SLAVE,         Bw_Generic_Ext,         25000000,           100,          MMPM_BW_USAGE_LPASS_DSP, },
            {MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_LPM_SLAVE,         Bw_Generic_Ext,         10000000,           100,          MMPM_BW_USAGE_LPASS_DSP, },
            {MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_LPM_SLAVE,         Bw_Generic,             60000000,           100,          MMPM_BW_USAGE_LPASS_DSP, },
            {MMPM_BW_PORT_ID_ADSP_MASTER, MMPM_BW_PORT_ID_LPM_SLAVE,         Bw_Generic,             60000000,           100,          MMPM_BW_USAGE_LPASS_DSP, },
        }
    },
    {6,
        {
            {MMPM_BW_PORT_ID_EXT_AHB_MASTER, MMPM_BW_PORT_ID_LPM_SLAVE,      Bw_Generic_Ext,          5000000,            100,          MMPM_BW_USAGE_LPASS_DSP, },
            {MMPM_BW_PORT_ID_EXT_AHB_MASTER, MMPM_BW_PORT_ID_LPM_SLAVE,      Bw_Generic_Ext,         20000000,            100,          MMPM_BW_USAGE_LPASS_DSP, },
            {MMPM_BW_PORT_ID_EXT_AHB_MASTER, MMPM_BW_PORT_ID_LPM_SLAVE,      Bw_Generic_Ext,         80000000,            100,          MMPM_BW_USAGE_LPASS_DSP, },
            {MMPM_BW_PORT_ID_EXT_AHB_MASTER, MMPM_BW_PORT_ID_LPM_SLAVE,      Bw_Generic_Ext,        240000000,            100,          MMPM_BW_USAGE_LPASS_DSP, },
            {MMPM_BW_PORT_ID_EXT_AHB_MASTER, MMPM_BW_PORT_ID_LPM_SLAVE,      Bw_Generic,             60000000,            100,          MMPM_BW_USAGE_LPASS_DSP, },
            {MMPM_BW_PORT_ID_EXT_AHB_MASTER, MMPM_BW_PORT_ID_LPM_SLAVE,      Bw_Generic,             60000000,            100,          MMPM_BW_USAGE_LPASS_DSP, },
        }
    }
};

// Request Sleep Latency parameters table
const uint32 gReqSleepLatencyParam[NUMLPASSCOREIDS] =
{
    // microSec
    0,  // NONE
    5,  // ADSP
    5,  // CORE
    5,  // LPM
    5,  // DML
    5,  // AIF
    5,  // SLIMBUS
    5,  // MIDI
    5,  // AVSYNC
    5,  // HWRSMP
    5,  // SRAM
    5,  // DCODEC
    5,  // SPDIF
    20, // HDMIRX
    20, // HDMITX
    30, // SIF
    50, // BSTC
    50, // HVX
    50  // Dpaudio
};

// GetInfo parameters table
const uint32 gInfoClkParam[NUMLPASSCOREIDS] =
{
    // clkId
    MMPM_CLK_ID_LPASS_NONE,         // NONE
    MMPM_CLK_ID_LPASS_AVSYNC_BT,    // ADSP
    MMPM_CLK_ID_LPASS_NONE,         // CORE
    MMPM_CLK_ID_LPASS_AVSYNC_FM,    // LPM
    MMPM_CLK_ID_LPASS_AVTIMER_CORE, // DML
    MMPM_CLK_ID_LPASS_ATIME_CORE,   // AIF
    MMPM_CLK_ID_LPASS_SLIMBUS_CORE, // SLIMBUS
    MMPM_CLK_ID_LPASS_MIDI_CORE,    // MIDI
    MMPM_CLK_ID_LPASS_AVSYNC_XO,    // AVSYNC
    MMPM_CLK_ID_LPASS_NONE,         // HWRSP
    MMPM_CLK_ID_LPASS_NONE,         // SRAM
    MMPM_CLK_ID_LPASS_HWRSP_CORE,   // DCODEC
    MMPM_CLK_ID_LPASS_ATIME2_CORE,  // SPDIF
    MMPM_CLK_ID_LPASS_NONE,         // HDMIRX
    MMPM_CLK_ID_LPASS_NONE,         // HDMITX
    MMPM_CLK_ID_LPASS_NONE,         // SIF
    MMPM_CLK_ID_LPASS_NONE,         // BSTC
    MMPM_CLK_ID_LPASS_NONE,         // HVX
    MMPM_CLK_ID_LPASS_NONE,         // Dpaudio
};

// Table that maps cores to memory types and power states in mmpm clock
const MmpmReqMemPowerTestParamTableType gReqMemPwrParam[NUMLPASSCOREIDS] =
{
    // memorytype, state
    {0, 0}, // NONE
    {2, 1}, // ADSP
    {2, 1}, // CORE
    {2, 2}, // LPM
    {2, 3}, // DML
    {3, 1}, // AIF
    {3, 2}, // SLIMBUS
    {3, 3}, // MIDI
    {2, 1}, // AVSYNC
    {2, 2}, // HWRSMP
    {2, 3}, // SRAM
    {3, 1}, // DCODEC
    {3, 1}, // SPDIF
    {3, 1}, // HDMIRX
    {3, 1}, // HDMITX
    {2, 1}, // SIF
    {2, 1}, // BSTC
    {2, 1}, // HVX
    {3, 1}, // Dpaudio
};

// Frequency plan table
const MmpmFreqPlanTestParamTableType gFreqPlanParam[NUMLPASSCOREIDS] =
{
    // numClk
    {}, // NONE
    {}, // ADSP
    {}, // CORE
    {}, // LPM
    {}, // DML
    {}, // AIF
    {1, // SLIMBUS
        {   // numFreq
            {1, MMPM_CLK_ID_LPASS_SLIMBUS_CORE, {24576}}
        }
    },
    {1, // MIDI
        {
            // numFreq
            {3, MMPM_CLK_ID_LPASS_MIDI_CORE, {109227, 122880, 140434}}
        }
    },
    {5, // AVSYNC
        {
            // numFreq
            {1, MMPM_CLK_ID_LPASS_AVSYNC_XO, {20000}},
            {1, MMPM_CLK_ID_LPASS_AVSYNC_BT, {27000}},
            {1, MMPM_CLK_ID_LPASS_AVSYNC_FM, {20000}},
            {1, MMPM_CLK_ID_LPASS_AVTIMER_CORE, {20000}},
            {1, MMPM_CLK_ID_LPASS_ATIME_CORE, {20000}}
        }
    },
    {1, // HWRSMP
        {
            {3, MMPM_CLK_ID_LPASS_HWRSP_CORE, {19200, 68267, 136533}}
        }
    },
    {}, // SRAM
    {}, // DCODEC
    {}, // SPDIF
    {}, // HDMIRX
    {}, // HDMITX
    {}, // SIF
    {}, // BSTC
    {}, // HVX
    {}  // Dpaudio
};


void GetReqClockParam(MmpmCoreIdType coreId, ClkTestType clk[], uint32 *numClks)
{
    uint32 index = coreId - MMPM_CORE_ID_LPASS_START, i;
    *numClks = 0;
    for(i = 0; i < gReqClkParam[index].numClks; i++)
    {
        if(AsicInfo_isCoreClockAvailable(gReqClkParam[index].clk[i].clkId))
        {
            clk[*numClks].clkId = gReqClkParam[index].clk[i].clkId;
            clk[*numClks].freq = gReqClkParam[index].clk[i].freq * 1000;
            clk[*numClks].freqMatch = gReqClkParam[index].clk[i].freqMatch;
            *numClks = *numClks + 1;
        }
    }
}


void GetReqClockDomainParam(MmpmCoreIdType coreId, ClkDomainTestType clk[], uint32 *numClks)
{
    uint32 index = coreId - MMPM_CORE_ID_LPASS_START, i;
    *numClks = 0;
    for(i = 0; i < gReqClkDomainParam[index].numClks; i++)
    {
        if(AsicInfo_isCoreClockAvailable(gReqClkParam[index].clk[i].clkId))
        {
            clk[*numClks].clkId = gReqClkDomainParam[index].clk[i].clkId;
            clk[*numClks].clkFreqHz = gReqClkDomainParam[index].clk[i].clkFreqHz;
            clk[*numClks].clkDomainSrc = gReqClkDomainParam[index].clk[i].clkDomainSrc;
            *numClks = *numClks + 1;
        }
    }
}


void GetReqMipsParam(MmpmCoreIdType coreId, uint32 *mips)
{
    uint32 index = coreId - MMPM_CORE_ID_LPASS_START;
    if(AsicInfo_isCoreInstanceAvailable(coreId, MMPM_CORE_INSTANCE_0))
    {
        *mips = gReqMipsParam[index];
    }
}


void GetReqRegProgParam(MmpmCoreIdType coreId, MmpmRegProgMatchType *match)
{
    uint32 index = coreId - MMPM_CORE_ID_LPASS_START;
    if(AsicInfo_isCoreInstanceAvailable(coreId, MMPM_CORE_INSTANCE_0))
    {
        *match = gReqRegProgParam[index];
    }
}


void GetReqBandwidthParam(MmpmCoreIdType coreId, BwReqTestType bw[], uint32 *numBw)
{
    uint32 index = coreId - MMPM_CORE_ID_LPASS_START, i;
    *numBw = 0;
    for(i = 0; i < gReqBwParam[index].numBw; i++)
    {
        if((AsicInfo_isBusPortAvailable(gReqBwParam[index].bw[i].masterPort)) &&
            (AsicInfo_isBusPortAvailable(gReqBwParam[index].bw[i].slavePort)))
        {
            bw[*numBw].masterPort = gReqBwParam[index].bw[i].masterPort;
            bw[*numBw].slavePort = gReqBwParam[index].bw[i].slavePort;
            bw[*numBw].bwVal = gReqBwParam[index].bw[i].bwVal;
            bw[*numBw].usagePercent = gReqBwParam[index].bw[i].usagePercent;
            bw[*numBw].usageType = gReqBwParam[index].bw[i].usageType;
            *numBw = *numBw + 1;
        }
    }
}

void GetReqDdrBandwidthParam(BwReqTestType_Ext bw[], uint32 *numBw, uint32 index)
{
    uint32 i;
    *numBw = 0;
    for(i = 0; i < gReqDdrBwParam[index].numBw; i++)
    {
        if((AsicInfo_isBusPortAvailable(gReqDdrBwParam[index].bwExt[i].masterPort)) &&
            (AsicInfo_isBusPortAvailable(gReqDdrBwParam[index].bwExt[i].slavePort)))
        {
            bw[*numBw].masterPort = gReqDdrBwParam[index].bwExt[i].masterPort;
            bw[*numBw].slavePort = gReqDdrBwParam[index].bwExt[i].slavePort;
            bw[*numBw].bwVal = gReqDdrBwParam[index].bwExt[i].bwVal;
            bw[*numBw].usagePercent = gReqDdrBwParam[index].bwExt[i].usagePercent;
            bw[*numBw].usageType = gReqDdrBwParam[index].bwExt[i].usageType;
            bw[*numBw].type = gReqDdrBwParam[index].bwExt[i].type;
            *numBw = *numBw + 1;
        }
    }
}

void GetReqInternalBandwidthParam(BwReqTestType_Ext bw[], uint32 *numBw, uint32 index)
{
    uint32 i;
    *numBw = 0;
    for(i = 0; i < gReqInternalBwParam[index].numBw; i++)
    {
        if((AsicInfo_isBusPortAvailable(gReqInternalBwParam[index].bwExt[i].masterPort)) &&
            (AsicInfo_isBusPortAvailable(gReqInternalBwParam[index].bwExt[i].slavePort)))
        {
            bw[*numBw].masterPort = gReqInternalBwParam[index].bwExt[i].masterPort;
            bw[*numBw].slavePort = gReqInternalBwParam[index].bwExt[i].slavePort;
            bw[*numBw].bwVal = gReqInternalBwParam[index].bwExt[i].bwVal;
            bw[*numBw].usagePercent = gReqInternalBwParam[index].bwExt[i].usagePercent;
            bw[*numBw].usageType = gReqInternalBwParam[index].bwExt[i].usageType;
            bw[*numBw].type = gReqInternalBwParam[index].bwExt[i].type;
            *numBw = *numBw + 1;
        }
    }
}

void GetReqSleepLatencyParam(MmpmCoreIdType coreId, uint32 *microSec)
{
    uint32 index = coreId - MMPM_CORE_ID_LPASS_START;
    if(AsicInfo_isCoreInstanceAvailable(coreId, MMPM_CORE_INSTANCE_0))
    {
        *microSec = gReqSleepLatencyParam[index];
    }
}


void GetReqMemPowerParam(MmpmCoreIdType coreId, MmpmMemIdType *memId, MmpmMemPowerStateType *powerState)
{
    uint32 index = coreId - MMPM_CORE_ID_LPASS_START;
    if(AsicInfo_isMemoryAvailable(gReqMemPwrParam[index].memId))
    {
        *memId = gReqMemPwrParam[index].memId;
        *powerState = gReqMemPwrParam[index].powerState;
    }
}


void GetInfoClkParam(MmpmCoreIdType coreId, MmpmClkIdLpassType *clkId)
{
    uint32 index = coreId - MMPM_CORE_ID_LPASS_START;
    if(AsicInfo_isCoreClockAvailable(gInfoClkParam[index]))
    {
        *clkId = gInfoClkParam[index];
    }
}
