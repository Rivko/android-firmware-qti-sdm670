/*! \file */

/*
===========================================================================

FILE:         hal_sde_4_0_0.c

DESCRIPTION:  MDP Initialization
  
===========================================================================
===========================================================================
Copyright (c) 2012 - 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
===========================================================================
*/

/*------------------------------------------------------------------------------
 * Include Files
 *----------------------------------------------------------------------------*/
#include "hal_mdp_i.h"

#ifdef __cplusplus
extern "C" {
#endif


/*------------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------------*/
/** Format: HAL_MDP_<MAJOR>_<MINOR>_<STEP>_XXX */
#define  HAL_MDP_4_0_0_NUM_OF_RGB_LAYERS                       0     /** RGB Source Surface Pixel Processors      */
#define  HAL_MDP_4_0_0_NUM_OF_VIG_LAYERS                       2     /** VIG Source Surface Pixel Processors      */
#define  HAL_MDP_4_0_0_NUM_OF_DMA_LAYERS                       3     /** DMA Source Surface Pixel Processors      */
#define  HAL_MDP_4_0_0_NUM_OF_CURSOR_LAYERS                    2     /** CURSOR Source Surface Pixel Processors   */
#define  HAL_MDP_4_0_0_NUM_OF_LAYER_MIXERS                     4     /** Layer Mixers */
#define  HAL_MDP_4_0_0_NUM_OF_DSPP                             2     /** Destination Surface Pixel Processor      */
#define  HAL_MDP_4_0_0_NUM_OF_PHYSICAL_INTERFACES              4     /** INTF_0, INTF_1, INTF_2 and INTF_3        */
#define  HAL_MDP_4_0_0_NUM_OF_WRITEBACK_INTERFACES             1     /** WB_0, WB_1 and WB_2                      */
#define  HAL_MDP_4_0_0_NUM_OF_CONTROL_PATHS                    4     /** MDP_CTL_x (x = 0, 1, 2, 3, and 4)        */
#define  HAL_MDP_4_0_0_NUM_OF_DATA_PATHS                       1     /** MDP_WB_x (x = 0, 1, 2, 3, and 4)         */
#define  HAL_MDP_4_0_0_NUM_OF_PINGPONGS                        5     /** Pingpong blocks                          */
#define  HAL_MDP_4_0_0_NUM_OF_WATCHDOGS                        0     /** Watch Dogs                               */
#define  HAL_MDP_4_0_0_NUM_OF_AXI_PORTS                        2     /** Number of AXI ports                      */
#define  HAL_MDP_4_0_0_NUM_OF_VBIF_CLIENTS                     14    /** Number of VBIF clients                      */

#define  HAL_MDP_4_0_0_NUM_OF_CHROMA_BLOCKS                    1     /** Number of Chroma blocks                  */

/** RGB and VG and DMA IGC LUT Size */
#define  HAL_MDP_4_0_0_SSPP_IGC_LUT_SIZE                       256
#define  HAL_MDP_4_0_0_SSPP_IGC_NUM_OF_COLOR_COMPONENTS        3     /** Color 0, 1 and 2                         */

/** DSPP IGC LUT Size */
#define  HAL_MDP_4_0_0_DSPP_IGC_LUT_SIZE                       256
#define  HAL_MDP_4_0_0_DSPP_IGC_NUM_OF_COLOR_COMPONENTS        3     /** Color 0, 1 and 2                         */

/** QSEED */

/** LAYER MIXER */
#define  HAL_MDP_4_0_0_LAYER_MIXER_MAX_BLEND_STAGES            7     /** Blend Stage #0, 1, 2 and 3               */

/** QOS */
#define HAL_MDP_4_0_0_MAX_MDPCORE_CLK_FREQ                     320000000   /** MDP core maximum working clock frequency in Hz */

#define HAL_MDP_4_0_0_QOS_REQPRI_WATERMARK_REGS_COUNT          3


/* QoS priority re-mapping for real time read clients; real time clients are ViG, RGB, and DMA in line mode;*/
#define HAL_MDP_4_0_0_QOS_REMAPPER_REALTIME_CLIENTS            HAL_MDP_QOS_REMAPPER_INFO(                \
                                                               HAL_MDP_TRFCTRL_LATENCY_REALTIME,         \
                                                               HAL_MDP_TRFCTRL_LATENCY_REALTIME,         \
                                                               HAL_MDP_TRFCTRL_LATENCY_REALTIME,         \
                                                               HAL_MDP_TRFCTRL_LATENCY_REALTIME )
/* QoS priority re-mapping for non-real time read clients; non-real time clients are DMA in block mode;*/
#define HAL_MDP_4_0_0_QOS_REMAPPER_NONREALTIME_CLIENTS         HAL_MDP_QOS_REMAPPER_INFO(                \
                                                               HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME,     \
                                                               HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME,     \
                                                               HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME,     \
                                                               HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME )

/** VBIF max burst length */
#define HAL_MDP_4_0_0_VBIF_MAX_DDR_RD_BURST_SIZE               7
#define HAL_MDP_4_0_0_VBIF_MAX_DDR_WR_BURST_SIZE               7
#define HAL_MDP_4_0_0_VBIF_MAX_RD_OT_SIZE                      0     /* 0 to use default value */
#define HAL_MDP_4_0_0_VBIF_MAX_WR_OT_SIZE                      0     /* 0 to use default value */

#define HAL_MDP_4_0_0_VBIF_ROUND_ROBIN_QOS_ARB                 HWIO_VBIF_VBIF_ROUND_ROBIN_QOS_ARB_RR_QOS_EN_BMSK

/** Clock control */
#define HAL_MDP_4_0_0_HYSTERISIS_DEFAULT                       3

/** HSIC register range */
#define HAL_MDP_4_0_0_PICTURE_ADJUST_MIN_HUE                  (-768)   // -180 degree
#define HAL_MDP_4_0_0_PICTURE_ADJUST_MAX_HUE                  (-768)   // 180 degree
#define HAL_MDP_4_0_0_PICTURE_ADJUST_MIN_UNCHANGE_HUE          0       // value for unchange hue going to the min hue direction
#define HAL_MDP_4_0_0_PICTURE_ADJUST_MAX_UNCHANGE_HUE         (-1536)  // value for unchange hue going to the max hue direction
#define HAL_MDP_4_0_0_PICTURE_ADJUST_MIN_SATURATION           (-128)   
#define HAL_MDP_4_0_0_PICTURE_ADJUST_MAX_SATURATION            127
#define HAL_MDP_4_0_0_PICTURE_ADJUST_UNCHANGE_SATURATION       0
#define HAL_MDP_4_0_0_PICTURE_ADJUST_MIN_INTENSITY            (-128) 
#define HAL_MDP_4_0_0_PICTURE_ADJUST_MAX_INTENSITY             127 
#define HAL_MDP_4_0_0_PICTURE_ADJUST_UNCHANGE_INTENSITY        0
#define HAL_MDP_4_0_0_PICTURE_ADJUST_MIN_CONTRAST             (-128)
#define HAL_MDP_4_0_0_PICTURE_ADJUST_MAX_CONTRAST              127
#define HAL_MDP_4_0_0_PICTURE_ADJUST_UNCHANGE_CONTRAST         0

/** Highest bank bit configuration */
#define HAL_MDP_4_0_0_HIGHEST_BANK_BIT_DEFAULT                 1


/*------------------------------------------------------------------------
 * Global Data Definitions
 *------------------------------------------------------------------------ */

/* 
 * MDP has a set of MDP_CLK_STATUSx registers to read the active/idle state of the MDP modules (SSPP, DSPP etc).
 * The following tables provide a mapping information of MDP_CLK_STATUSx register and BitFieldMask for each MDP module
 *
 *    (a)   gsSSPPModuleStatusRegInfo        --> Mapping table for SSPPx (VIGx, RGBx, CURSORx and DMAx) modules
 *    (b)   gsDSPPStatusRegInfo              --> Mapping table for DSPPx modules
 *    (c)   gsPingPongStatusRegInfo          --> Mapping table for PING-PONGx modules
 *    (c)   gsWBStatusRegInfo                --> Mapping table for WBx modules
 */
static HAL_MDP_ModuleStatusRegInfo     gsSSPPModuleStatusRegInfo[HAL_MDP_SOURCE_PIPE_MAX] =
{
 //{ uRegAddrOffset  uBitFieldMask                                         }
   { 0x00000000,     0x00000000                                            },    // HAL_MDP_SOURCE_PIPE_NONE
   { 0x00000000,     HWIO_MDP_CLK_STATUS0_VIG0_CLK_ACTIVE_BMSK        },    // HAL_MDP_SOURCE_PIPE_VIG_0
   { 0x00000000,     HWIO_MDP_CLK_STATUS1_VIG1_CLK_ACTIVE_BMSK        },    // HAL_MDP_SOURCE_PIPE_VIG_1
   { 0x00000000,     HWIO_MDP_CLK_STATUS2_VIG2_CLK_ACTIVE_BMSK        },    // HAL_MDP_SOURCE_PIPE_VIG_2
   { 0x00000000,     HWIO_MDP_CLK_STATUS6_VIG3_CLK_ACTIVE_BMSK        },    // HAL_MDP_SOURCE_PIPE_VIG_3
   { 0x00000000,     0x00000000                                            },    // HAL_MDP_SOURCE_PIPE_VIG_4
   { 0x00000000,     HWIO_MDP_CLK_STATUS0_RGB0_CLK_ACTIVE_BMSK        },    // HAL_MDP_SOURCE_PIPE_RGB_0
   { 0x00000000,     HWIO_MDP_CLK_STATUS1_RGB1_CLK_ACTIVE_BMSK        },    // HAL_MDP_SOURCE_PIPE_RGB_1
   { 0x00000000,     HWIO_MDP_CLK_STATUS2_RGB2_CLK_ACTIVE_BMSK        },    // HAL_MDP_SOURCE_PIPE_RGB_2
   { 0x00000000,     HWIO_MDP_CLK_STATUS6_RGB3_CLK_ACTIVE_BMSK        },    // HAL_MDP_SOURCE_PIPE_RGB_3
   { 0x00000000,     0x00000000                                            },    // HAL_MDP_SOURCE_PIPE_RGB_4
   { 0x00000000,     HWIO_MDP_CLK_STATUS0_DMA0_CLK_ACTIVE_BMSK        },    // HAL_MDP_SOURCE_PIPE_DMA_0 
   { 0x00000000,     HWIO_MDP_CLK_STATUS1_DMA1_CLK_ACTIVE_BMSK        },    // HAL_MDP_SOURCE_PIPE_DMA_1
   { 0x00000000,     0x00000000                                            },    // HAL_MDP_SOURCE_PIPE_DMA_2
   { 0x00000000,     0x00000000                                            },    // HAL_MDP_SOURCE_PIPE_DMA_3
   { 0x00000000,     0x00000000                                            },    // HAL_MDP_SOURCE_PIPE_DMA_4
   { 0x00000000,     HWIO_MDP_CLK_STATUS3_CURSOR0_CLK_ACTIVE_BMSK     },    // HAL_MDP_SOURCE_PIPE_CURSOR_0 
   { 0x00000000,     HWIO_MDP_CLK_STATUS4_CURSOR1_CLK_ACTIVE_BMSK     },    // HAL_MDP_SOURCE_PIPE_CURSOR_1
   { 0x00000000,     HWIO_MDP_CLK_STATUS5_CURSOR2_CLK_ACTIVE_BMSK     },    // HAL_MDP_SOURCE_PIPE_CURSOR_2
   { 0x00000000,     HWIO_MDP_CLK_STATUS7_CURSOR3_CLK_ACTIVE_BMSK     },    // HAL_MDP_SOURCE_PIPE_CURSOR_3
   { 0x00000000,     0x00000000                                            },    // HAL_MDP_SOURCE_PIPE_CURSOR_4
};

static HAL_MDP_ModuleStatusRegInfo    gsDSPPStatusRegInfo[HAL_MDP_DESTINATION_PIPE_MAX] = 
{
 //{ uRegAddrOffset  uBitFieldMask                                         }
   { 0x00000000,     0x00000000                                            },    // HAL_MDP_DESTINATION_PIPE_NONE
   { 0x00000000,     HWIO_MDP_CLK_STATUS3_LM_DSPP0_CLK_ACTIVE_BMSK    },    // HAL_MDP_DESTINATION_PIPE_0
   { 0x00000000,     HWIO_MDP_CLK_STATUS4_LM_DSPP1_CLK_ACTIVE_BMSK    },    // HAL_MDP_DESTINATION_PIPE_1
   { 0x00000000,     HWIO_MDP_CLK_STATUS5_LM_DSPP2_CLK_ACTIVE_BMSK    },    // HAL_MDP_DESTINATION_PIPE_2
   { 0x00000000,     HWIO_MDP_CLK_STATUS7_LM_DSPP3_CLK_ACTIVE_BMSK    },    // HAL_MDP_DESTINATION_PIPE_3
};

static HAL_MDP_ModuleStatusRegInfo    gsPingPongStatusRegInfo[HAL_MDP_PINGPONG_MAX] = 
{
 //{ uRegAddrOffset  uBitFieldMask                                         }
   { 0x00000000,     0x00000000                                            },    // HAL_MDP_PINGPONG_NONE
   { 0x00000000,     HWIO_MDP_CLK_STATUS3_PP0_CLK_ACTIVE_BMSK         },    // HAL_MDP_PINGPONG_0
   { 0x00000000,     HWIO_MDP_CLK_STATUS4_PP1_CLK_ACTIVE_BMSK         },    // HAL_MDP_PINGPONG_1
   { 0x00000000,     HWIO_MDP_CLK_STATUS5_PP2_CLK_ACTIVE_BMSK         },    // HAL_MDP_PINGPONG_2
   { 0x00000000,     HWIO_MDP_CLK_STATUS7_PP3_CLK_ACTIVE_BMSK         },    // HAL_MDP_PINGPONG_3
   { 0x00000000,     0x00000000                                            },    // HAL_MDP_PINGPONG_4
};

static HAL_MDP_ModuleStatusRegInfo    gsWBStatusRegInfo[HAL_MDP_INTERFACE_MAX - HAL_MDP_INTERFACE_WRITEBACK_TYPE_BASE] = 
{
 //{ uRegAddrOffset     uBitFieldMask                                         }
   { 0x00000000,        0x00000000                                            },    // HAL_MDP_INTERFACE_WRITEBACK_TYPE_BASE
   { 0x00000000,        HWIO_MDP_CLK_STATUS3_ROT0_BLK_CLK_ACTIVE_BMSK    },    // HAL_MDP_INTERFACE_MEMORY_WB_BLK_MODE_0
   { 0x00000000,        HWIO_MDP_CLK_STATUS4_ROT1_BLK_CLK_ACTIVE_BMSK    },    // HAL_MDP_INTERFACE_MEMORY_WB_BLK_MODE_1
   { 0x00000000,        HWIO_MDP_CLK_STATUS3_ROT0_LINE_CLK_ACTIVE_BMSK   },    // HAL_MDP_INTERFACE_MEMORY_WB_LINE_MODE_0
   { 0x00000000,        HWIO_MDP_CLK_STATUS4_ROT1_LINE_CLK_ACTIVE_BMSK   },    // HAL_MDP_INTERFACE_MEMORY_WB_LINE_MODE_1
   { 0x00000000,        HWIO_MDP_CLK_STATUS5_WB2_CLK_ACTIVE_BMSK         },    // HAL_MDP_INTERFACE_MEMORY_WB_LINEAR_MODE
};

uint32 gTwoDFilterA_LUT[HAL_MDP_QSEED_ENTRIE_SIZE_IN_REGISTERS*HAL_MDP_QSEED_COEFF_LUT_A_ENTRIES] = 
{
  1006189366,      4097404288,      4063501563,       524188928,
   369053214,      4067321599,      4199027065,       524172800,
      938761,        10834428,       307365624,       530464512,
  1324946718,      4163004032,      4031264633,       536837375,
   569909520,      4031932287,      4201458039,       522157183,
   117254659,      4102379647,       444602105,       524238080,
  1409024009,      4228731264,      4031927032,        29507708,
   720963075,      4096612352,      4169155321,       530808191,
   200805374,      4032385280,       379858174,       520289280,
   993344505,      4097404025,        32971904,       111164793,
   340403449,      4067255933,      4294780288,        62930168,
     5443324,        10834313,      4161612032,        20954232,
   605951225,      4065027704,        33371776,       157285629,
   120063996,      4102052347,      4195303424,        85949819,
  4197121535,       213008771,      4063051776,        29277817,
   235471100,      4031926776,      4228731520,       176095241,
  4245551871,      4169155065,      4096612608,        90063619,
  4161533952,       379857918,      4032385536,        25036286,
   770911731,         2933120,        32642560,       509607936,
  4261225715,      4035424000,        33296768,       509607936,
  4061204985,      3841752697,      4061071232,       522190848,
  4210162423,      4001013887,      4129176959,       507444223,
  4059362557,      3806687741,      3897436031,       513703167,
  4076008191,      3835514623,      3835514623,       509393279,
           0,           32768,               0,      2147483648,
  3741319168,      4033797504,               0,      1073741824,
  3909091328,      3541535616,               0,       536870912,
  3824804610,      4066361345,      4229443456,      1061141633,
  3975081473,      3639769089,      4131200640,      1065303552,
  4092850945,      3868806273,      3868806273,       511506817,
   770911731,         2933120,        32642560,       509607936,
  4261225715,      4035424000,        33296768,       509607936,
  4061204985,      3841752697,      4061071232,       522190848,
  4210162423,      4001013887,      4129176959,       507444223,
  4059362557,      3806687741,      3897436031,       513703167,
  4076008191,      3835514623,      3835514623,       509393279,
           0,           32768,               0,      2147483648,
  3741319168,      4033797504,               0,      1073741824,
  3909091328,      3541535616,               0,       536870912,
  3824804610,      4066361345,      4229443456,      1061141633,
  3975081473,      3639769089,      4131200640,      1065303552,
  4092850945,      3868806273,      3868806273,       511506817

};
uint32 gTwoDFilterB_LUT[HAL_MDP_QSEED_ENTRIE_SIZE_IN_REGISTERS*HAL_MDP_QSEED_COEFF_LUT_B_ENTRIES] = 
{
   654001706,      4065487744,      4097775482,       522091904,
   134566419,      4136263294,        72289145,       526270080,
   922229784,      4097271808,      4099551225,       528464896,
   318383882,      4033503488,       108470137,       522157184,
  1039860231,      4162541312,      4066987000,         6487806,
   435685632,      4064433664,        43268219,       524500096,
   641740793,      4065487354,        33169920,        88095993,
   139467003,      4136262786,      4228130944,        41942136,
   321129978,      4033241337,      4261756160,       121617660,
  4280943102,      4271525631,      4129046784,        54476282,
    51117566,      4066987000,      4162541312,       129941767,
  4195154176,        43267963,      4064433920,        54396032,
   267856882,      4166303360,        32838784,       507510784,
  4094041078,      3905001981,      4194569344,       515899392,
  4092852475,      3903556096,      4063375744,       509541504,
  4076863488,      4229397248,           65664,      2147483648,
  3774873600,      3837704064,               0,       536870912,
  3858032642,      3903102849,      4197001088,      1061125376,
   267856882,      4166303360,        32838784,       507510784,
  4094041078,      3905001981,      4194569344,       515899392,
  4092852475,      3903556096,      4063375744,       509541504,
  4076863488,      4229397248,           65664,      2147483648,
  3774873600,      3837704064,               0,       536870912,
  3858032642,      3903102849,      4197001088,      1061125376

};
uint32 gTwoDFilterC_LUT[HAL_MDP_QSEED_ENTRIE_SIZE_IN_REGISTERS*HAL_MDP_QSEED_COEFF_LUT_C_ENTRIES] = 
{
  1190667818,      4130237312,      4064225658,       528399488,
   486225432,      4032784384,      4233763833,       522108160,
    33766663,      4204222334,       375983608,       528383488,
  1392117779,      4195769856,      4031726969,        10534398,
   670568970,      4064174336,      4202116473,       524352512,
   167385344,      4067448320,       412361083,       520125568,
   791291641,      4064569849,        33171712,       134233338,
   221844474,      4068106492,      4228199168,        77593721,
  4248634366,        78531463,      4128978048,        25115896,
   403900667,      4031864056,      4261692672,       169852290,
    18481150,      4135735034,      4162604288,        90111999,
  4179229184,       313407232,      4030876160,        27148539,
  4260694773,      4035161088,      4228391936,       507526656,
  4060086267,      3808001275,      3962246655,       517947007,
  4059165183,      3804520831,      3866442495,       511539967,
  3774605826,      4033598848,      4262138880,      1069547136,
  3941790209,      3574499073,      4229500032,      1071628032,
  4042256129,      3771229185,      3999806465,      1056848897,
  4260694773,      4035161088,      4228391936,       507526656,
  4060086267,      3808001275,      3962246655,       517947007,
  4059165183,      3804520831,      3866442495,       511539967,
  3774605826,      4033598848,      4262138880,      1069547136,
  3941790209,      3574499073,      4229500032,      1071628032,
  4042256129,      3771229185,      3999806465,      1056848897

};
uint32 gTwoDFilterD_LUT[HAL_MDP_QSEED_ENTRIE_SIZE_IN_REGISTERS*HAL_MDP_QSEED_COEFF_LUT_D_ENTRIES] = 
{
  804859681,      4064701184,      4098761593,       524205184,
  234830607,      4068172287,       107156600,       524205440,
 1006177551,      4129841536,      4100144632,       534838527,
  385357316,      4032192000,       109587322,       520158208,
  473046265,      4032653306,        33435264,       104873082,
   54266876,      4204025728,      4195234944,        50298105,
  186057724,      4066990073,      4195240448,       127893120,
 4212784126,      4271719676,      4063252223,        54427644,
  250744563,      4166171008,        33433856,       507526656,
 4093183223,      3871316093,      4095352447,       511671935,
 4076008189,      3935401984,      3998304384,       509475456,
 3909019649,      4197248640,          267776,      1069563520,
 3807899138,      3837309185,      4262531072,      1069547264,
 3941919234,      3968369537,      4131865600,      1050606337,
  250744563,      4166171008,        33433856,       507526656,
 4093183223,      3871316093,      4095352447,       511671935,
 4076008189,      3935401984,      3998304384,       509475456,
 3909019649,      4197248640,          267776,      1069563520,
 3807899138,      3837309185,      4262531072,      1069547264,
 3941919234,      3968369537,      4131865600,      1050606337
};


/* Map for VBIF clients for QOS remapper values for all cleints (RT & NRT) */
static HAL_MDP_VBIFQOSRemappersType gSVBIFQOSRemap_4_0_0[HAL_MDP_VBIF_CLIENT_MAX] =
{
    /*     uVBIFQosRemapper00            uVBIFQosRemapper01               uVBIFQosRemapper10                uVBIFQosRemapper11*/
    //Client C0:  HAL_MDP_VBIF_CLIENT_VIG0
    { HAL_MDP_TRFCTRL_LATENCY_REALTIME, HAL_MDP_TRFCTRL_LATENCY_REALTIME, HAL_MDP_TRFCTRL_LATENCY_REALTIME, HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME },
    //Client C1:  HAL_MDP_VBIF_CLIENT_DMA0
    { HAL_MDP_TRFCTRL_LATENCY_REALTIME, HAL_MDP_TRFCTRL_LATENCY_REALTIME, HAL_MDP_TRFCTRL_LATENCY_REALTIME, HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME },
    //Client C2 : HAL_MDP_VBIF_CLIENT_CURSOR0
    { HAL_MDP_TRFCTRL_LATENCY_REALTIME, HAL_MDP_TRFCTRL_LATENCY_REALTIME, HAL_MDP_TRFCTRL_LATENCY_REALTIME, HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME },
    //Client C3 : HAL_MDP_VBIF_CLIENT_NONE0
    { HAL_MDP_TRFCTRL_LATENCY_REALTIME, HAL_MDP_TRFCTRL_LATENCY_REALTIME, HAL_MDP_TRFCTRL_LATENCY_REALTIME, HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME },
    //Client C4 : HAL_MDP_VBIF_CLIENT_VIG1
    { HAL_MDP_TRFCTRL_LATENCY_REALTIME, HAL_MDP_TRFCTRL_LATENCY_REALTIME, HAL_MDP_TRFCTRL_LATENCY_REALTIME, HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME },
    //Client C5 : HAL_MDP_VBIF_CLIENT_DMA1
    { HAL_MDP_TRFCTRL_LATENCY_REALTIME, HAL_MDP_TRFCTRL_LATENCY_REALTIME, HAL_MDP_TRFCTRL_LATENCY_REALTIME, HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME },
    //Client C6 : HAL_MDP_VBIF_CLIENT_WB2
    { HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME, HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME, HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME, HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME },
    //Client C7 : HAL_MDP_VBIF_CLIENT_DSIDMA
    { HAL_MDP_TRFCTRL_LATENCY_REALTIME, HAL_MDP_TRFCTRL_LATENCY_REALTIME, HAL_MDP_TRFCTRL_LATENCY_REALTIME, HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME },
    //Client C8 : HAL_MDP_VBIF_CLIENT_VIG2
    { HAL_MDP_TRFCTRL_LATENCY_REALTIME, HAL_MDP_TRFCTRL_LATENCY_REALTIME, HAL_MDP_TRFCTRL_LATENCY_REALTIME, HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME },
    //Client C9 : HAL_MDP_VBIF_CLIENT_DMA2
    { HAL_MDP_TRFCTRL_LATENCY_REALTIME, HAL_MDP_TRFCTRL_LATENCY_REALTIME, HAL_MDP_TRFCTRL_LATENCY_REALTIME, HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME },
    //Client C10 : HAL_MDP_VBIF_CLIENT_CURSOR1
    { HAL_MDP_TRFCTRL_LATENCY_REALTIME, HAL_MDP_TRFCTRL_LATENCY_REALTIME, HAL_MDP_TRFCTRL_LATENCY_REALTIME, HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME },
    //Client C11: HAL_MDP_VBIF_CLIENT_NONE1
    { HAL_MDP_TRFCTRL_LATENCY_REALTIME, HAL_MDP_TRFCTRL_LATENCY_REALTIME, HAL_MDP_TRFCTRL_LATENCY_REALTIME, HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME },
    //Client C12 : HAL_MDP_VBIF_CLIENT_VIG3
    { HAL_MDP_TRFCTRL_LATENCY_REALTIME, HAL_MDP_TRFCTRL_LATENCY_REALTIME, HAL_MDP_TRFCTRL_LATENCY_REALTIME, HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME },
    //Client C13 : HAL_MDP_VBIF_CLIENT_DMA3
    { HAL_MDP_TRFCTRL_LATENCY_REALTIME, HAL_MDP_TRFCTRL_LATENCY_REALTIME, HAL_MDP_TRFCTRL_LATENCY_REALTIME, HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME },
    //Client C0 : HAL_MDP_VBIF_CLIENT_ROT_RD
    { HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME, HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME, HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME, HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME },
    //Client C1 : HAL_MDP_VBIF_CLIENT_ROT_WR
    { HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME, HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME, HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME, HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME },
};


/*
* This is the mapping between VBIF client ID to VBIF HW block ID
*/
const HAL_MDP_VbifId gsVbifClientToHwIdMap_4_0_0[HAL_MDP_VBIF_CLIENT_MAX] =
{
  HAL_MDP_VBIF_0,      //   HAL_MDP_VBIF_CLIENT_VIG0 = 0x00000000,   // xin0  (RT Port)
  HAL_MDP_VBIF_0,      //   HAL_MDP_VBIF_CLIENT_DMA0,                // xin1  (RT Port) 
  HAL_MDP_VBIF_0,      //   HAL_MDP_VBIF_CLIENT_CURSOR0,             // xin2  (RT Port)
  HAL_MDP_VBIF_0,      //   HAL_MDP_VBIF_CLIENT_NONE0,               // xin3  (RT Port)
  HAL_MDP_VBIF_0,      //   HAL_MDP_VBIF_CLIENT_VIG1,                // xin4  (RT Port)
  HAL_MDP_VBIF_0,      //   HAL_MDP_VBIF_CLIENT_DMA1,                // xin5   (RT Port)
  HAL_MDP_VBIF_0,      //   HAL_MDP_VBIF_CLIENT_WB2,                 // xin6   (RT Port)
  HAL_MDP_VBIF_0,      //   HAL_MDP_VBIF_CLIENT_DSIDMA,              // xin7   (RT Port)
  HAL_MDP_VBIF_0,      //   HAL_MDP_VBIF_CLIENT_VIG2,                // xin8   (RT Port)
  HAL_MDP_VBIF_0,      //   HAL_MDP_VBIF_CLIENT_DMA2,                // xin9   (RT Port)
  HAL_MDP_VBIF_0,      //   HAL_MDP_VBIF_CLIENT_CURSOR1,             // xin10  (RT Port)
  HAL_MDP_VBIF_0,      //   HAL_MDP_VBIF_CLIENT_NONE1,               // xin11  (RT Port)  
  HAL_MDP_VBIF_0,      //   HAL_MDP_VBIF_CLIENT_VIG3,                // xin12  (RT Port)
  HAL_MDP_VBIF_0,      //   HAL_MDP_VBIF_CLIENT_DMA3,                // xin13  (RT Port)
  HAL_MDP_VBIF_1,      //   HAL_MDP_VBIF_CLIENT_ROT_RD,              // xin0  (NRT Port)
  HAL_MDP_VBIF_1,      //   HAL_MDP_VBIF_CLIENT_ROT_WR,              // xin1  (NRT Port)
};


/****************************************************************************
*
** FUNCTION: HAL_MDP_SetHWBlockRegOffsets_4_0_0()
*/
/*!
* \brief
*     set MDP HW block register offsets
*
*
****************************************************************************/
static void HAL_MDP_SetHWBlockRegOffsets_4_0_0(void)
{
  /* Control path HW block register offset */
  uMDPControlPathRegBaseOffset[HAL_MDP_CONTROL_PATH_NONE]           = 0x00000000;
  uMDPControlPathRegBaseOffset[HAL_MDP_CONTROL_PATH_0]              = 0x00000000;
  uMDPControlPathRegBaseOffset[HAL_MDP_CONTROL_PATH_1]              = (MDP_CTL_1_REG_BASE_OFFS - MDP_CTL_0_REG_BASE_OFFS);
  uMDPControlPathRegBaseOffset[HAL_MDP_CONTROL_PATH_2]              = (MDP_CTL_2_REG_BASE_OFFS - MDP_CTL_0_REG_BASE_OFFS);
  uMDPControlPathRegBaseOffset[HAL_MDP_CONTROL_PATH_3]              = (MDP_CTL_3_REG_BASE_OFFS - MDP_CTL_0_REG_BASE_OFFS);
  uMDPControlPathRegBaseOffset[HAL_MDP_CONTROL_PATH_4]              = 0x00000000;
  uMDPControlPathRegBaseOffset[HAL_MDP_CONTROL_PATH_5]              = 0x00000000;
  uMDPControlPathRegBaseOffset[HAL_MDP_CONTROL_PATH_6]              = 0x00000000;
  uMDPControlPathRegBaseOffset[HAL_MDP_CONTROL_PATH_7]              = 0x00000000;

  /* Data path HW block register offset */
  uMDPDataPathRegBaseOffset[HAL_MDP_DATA_PATH_NONE]                 = 0x00000000;
  uMDPDataPathRegBaseOffset[HAL_MDP_DATA_PATH_0]                    = 0x00000000;
  uMDPDataPathRegBaseOffset[HAL_MDP_DATA_PATH_1]                    = 0x00000000;
  uMDPDataPathRegBaseOffset[HAL_MDP_DATA_PATH_2]                    = 0x00000000;
  uMDPDataPathRegBaseOffset[HAL_MDP_DATA_PATH_3]                    = 0x00000000;
  uMDPDataPathRegBaseOffset[HAL_MDP_DATA_PATH_4]                    = 0x00000000;
  uMDPDataPathRegBaseOffset[HAL_MDP_DATA_PATH_5]                    = 0x00000000;
  uMDPDataPathRegBaseOffset[HAL_MDP_DATA_PATH_6]                    = 0x00000000;
  uMDPDataPathRegBaseOffset[HAL_MDP_DATA_PATH_7]                    = 0x00000000;

  /* Destination (DSPP) HW block register offset */
  uMDPDSPPRegBaseOffset[HAL_MDP_DESTINATION_PIPE_NONE]              = 0x00000000;
  uMDPDSPPRegBaseOffset[HAL_MDP_DESTINATION_PIPE_0]                 = 0x00000000;
  uMDPDSPPRegBaseOffset[HAL_MDP_DESTINATION_PIPE_1]                 = (MDP_VP_0_DSPP_1_REG_BASE_OFFS - MDP_VP_0_DSPP_0_REG_BASE_OFFS);
  uMDPDSPPRegBaseOffset[HAL_MDP_DESTINATION_PIPE_2]                 = 0x00000000;
  uMDPDSPPRegBaseOffset[HAL_MDP_DESTINATION_PIPE_3]                 = 0x00000000;
  uMDPDSPPRegBaseOffset[HAL_MDP_DESTINATION_PIPE_4]                 = 0x00000000;

  /* Physical interface HW block register offset */
  uMDPPhyInterfaceRegBaseOffset[HAL_MDP_PHYSICAL_INTERFACE_ID_NONE] = 0x00000000;
  uMDPPhyInterfaceRegBaseOffset[HAL_MDP_PHYSICAL_INTERFACE_0]       = 0x00000000;
  uMDPPhyInterfaceRegBaseOffset[HAL_MDP_PHYSICAL_INTERFACE_1]       = (MDP_INTF_1_REG_BASE_OFFS - MDP_INTF_0_REG_BASE_OFFS);
  uMDPPhyInterfaceRegBaseOffset[HAL_MDP_PHYSICAL_INTERFACE_2]       = (MDP_INTF_2_REG_BASE_OFFS - MDP_INTF_0_REG_BASE_OFFS);
  uMDPPhyInterfaceRegBaseOffset[HAL_MDP_PHYSICAL_INTERFACE_3]       = (MDP_INTF_3_REG_BASE_OFFS - MDP_INTF_0_REG_BASE_OFFS);
  uMDPPhyInterfaceRegBaseOffset[HAL_MDP_PHYSICAL_INTERFACE_4]       = (MDP_INTF_4_REG_BASE_OFFS - MDP_INTF_0_REG_BASE_OFFS);
  uMDPPhyInterfaceRegBaseOffset[HAL_MDP_PHYSICAL_INTERFACE_5]       = 0x00000000;

  /* Layer mixer HW block register offset */
  uMDPLayerMixerRegBaseOffset[HAL_MDP_LAYER_MIXER_NONE]             = 0x00000000;
  uMDPLayerMixerRegBaseOffset[HAL_MDP_LAYER_MIXER_0]                = 0x00000000;
  uMDPLayerMixerRegBaseOffset[HAL_MDP_LAYER_MIXER_1]                = (MDP_VP_0_LAYER_1_REG_BASE_OFFS - MDP_VP_0_LAYER_0_REG_BASE_OFFS);
  uMDPLayerMixerRegBaseOffset[HAL_MDP_LAYER_MIXER_2]                = (MDP_VP_0_LAYER_2_REG_BASE_OFFS - MDP_VP_0_LAYER_0_REG_BASE_OFFS);
  uMDPLayerMixerRegBaseOffset[HAL_MDP_LAYER_MIXER_3]                = (MDP_VP_0_LAYER_3_REG_BASE_OFFS - MDP_VP_0_LAYER_0_REG_BASE_OFFS);
  uMDPLayerMixerRegBaseOffset[HAL_MDP_LAYER_MIXER_4]                = (MDP_VP_0_LAYER_4_REG_BASE_OFFS - MDP_VP_0_LAYER_0_REG_BASE_OFFS);
  uMDPLayerMixerRegBaseOffset[HAL_MDP_LAYER_MIXER_5]                = (MDP_VP_0_LAYER_5_REG_BASE_OFFS - MDP_VP_0_LAYER_0_REG_BASE_OFFS);
  uMDPLayerMixerRegBaseOffset[HAL_MDP_LAYER_MIXER_6]                = 0x00000000;

  /* Layer mixer blending stage register offset */
  uMDPLayerMixerBlendStateRegBaseOffset[HAL_MDP_BLEND_STAGE_0]      = 0x00000000;
  uMDPLayerMixerBlendStateRegBaseOffset[HAL_MDP_BLEND_STAGE_1]      = (HWIO_MDP_VP_0_LAYER_0_BLENDn_OP_OFFS(1) - HWIO_MDP_VP_0_LAYER_0_BLENDn_OP_OFFS(0));
  uMDPLayerMixerBlendStateRegBaseOffset[HAL_MDP_BLEND_STAGE_2]      = (HWIO_MDP_VP_0_LAYER_0_BLENDn_OP_OFFS(2) - HWIO_MDP_VP_0_LAYER_0_BLENDn_OP_OFFS(0));
  uMDPLayerMixerBlendStateRegBaseOffset[HAL_MDP_BLEND_STAGE_3]      = (HWIO_MDP_VP_0_LAYER_0_BLENDn_OP_OFFS(3) - HWIO_MDP_VP_0_LAYER_0_BLENDn_OP_OFFS(0));
  uMDPLayerMixerBlendStateRegBaseOffset[HAL_MDP_BLEND_STAGE_4]      = (HWIO_MDP_VP_0_LAYER_0_BLENDn_OP_OFFS(4) - HWIO_MDP_VP_0_LAYER_0_BLENDn_OP_OFFS(0));
  uMDPLayerMixerBlendStateRegBaseOffset[HAL_MDP_BLEND_STAGE_5]      = (HWIO_MDP_VP_0_LAYER_0_BLENDn_OP_OFFS(5) - HWIO_MDP_VP_0_LAYER_0_BLENDn_OP_OFFS(0));
  uMDPLayerMixerBlendStateRegBaseOffset[HAL_MDP_BLEND_STAGE_6]      = (HWIO_MDP_VP_0_LAYER_0_BLENDn_OP_OFFS(6) - HWIO_MDP_VP_0_LAYER_0_BLENDn_OP_OFFS(0));

  /* PingPong HW block register offset */
  uMDPPingPongRegBaseOffset[HAL_MDP_PINGPONG_NONE]                  = 0x00000000;
  uMDPPingPongRegBaseOffset[HAL_MDP_PINGPONG_0]                     = 0x00000000;
  uMDPPingPongRegBaseOffset[HAL_MDP_PINGPONG_1]                     = (MDP_PP_1_REG_BASE_OFFS - MDP_PP_0_REG_BASE_OFFS);
  uMDPPingPongRegBaseOffset[HAL_MDP_PINGPONG_2]                     = (MDP_PP_2_REG_BASE_OFFS - MDP_PP_0_REG_BASE_OFFS);
  uMDPPingPongRegBaseOffset[HAL_MDP_PINGPONG_3]                     = (MDP_PP_3_REG_BASE_OFFS - MDP_PP_0_REG_BASE_OFFS);
  uMDPPingPongRegBaseOffset[HAL_MDP_PINGPONG_4]                     = (MDP_PP_4_REG_BASE_OFFS - MDP_PP_0_REG_BASE_OFFS);

  /* DSC HW block register offset */
  uMDPDSCRegBaseOffset[HAL_MDP_DSC_NONE]                            = 0x00000000;
  uMDPDSCRegBaseOffset[HAL_MDP_DSC_0]                               = 0x00000000;
  uMDPDSCRegBaseOffset[HAL_MDP_DSC_1]                               = (MDP_DSC_1_REG_BASE_OFFS - MDP_DSC_0_REG_BASE_OFFS);
  uMDPDSCRegBaseOffset[HAL_MDP_DSC_2]                               = (MDP_DSC_2_REG_BASE_OFFS - MDP_DSC_1_REG_BASE_OFFS);
  uMDPDSCRegBaseOffset[HAL_MDP_DSC_3]                               = (MDP_DSC_3_REG_BASE_OFFS - MDP_DSC_2_REG_BASE_OFFS);
  
  /* Source (SSPP) HW block register offset */
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_NONE]                   = 0x00000000;
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_VIG_0]                  = 0x00000000;
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_VIG_1]                  = (MDP_VP_0_VIG_1_SSPP_REG_BASE_OFFS - MDP_VP_0_VIG_0_SSPP_REG_BASE_OFFS);
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_VIG_2]                  = (MDP_VP_0_VIG_2_SSPP_REG_BASE_OFFS - MDP_VP_0_VIG_0_SSPP_REG_BASE_OFFS);
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_VIG_3]                  = (MDP_VP_0_VIG_3_SSPP_REG_BASE_OFFS - MDP_VP_0_VIG_0_SSPP_REG_BASE_OFFS);
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_VIG_4]                  = 0x00000000;
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_RGB_0]                  = 0x00000000;
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_RGB_1]                  = 0x00000000;
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_RGB_2]                  = 0x00000000;
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_RGB_3]                  = 0x00000000;
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_RGB_4]                  = 0x00000000;
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_DMA_0]                  = (MDP_VP_0_DMA_0_SSPP_REG_BASE_OFFS - MDP_VP_0_VIG_0_SSPP_REG_BASE_OFFS);
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_DMA_1]                  = (MDP_VP_0_DMA_1_SSPP_REG_BASE_OFFS - MDP_VP_0_VIG_0_SSPP_REG_BASE_OFFS);
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_DMA_2]                  = (MDP_VP_0_DMA_2_SSPP_REG_BASE_OFFS - MDP_VP_0_VIG_0_SSPP_REG_BASE_OFFS);
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_DMA_3]                  = (MDP_VP_0_DMA_3_SSPP_REG_BASE_OFFS - MDP_VP_0_VIG_0_SSPP_REG_BASE_OFFS);
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_DMA_4]                  = 0x00000000;
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_CURSOR_0]               = (MDP_VP_0_CURSOR_0_SSPP_REG_BASE_OFFS - MDP_VP_0_VIG_0_SSPP_REG_BASE_OFFS);
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_CURSOR_1]               = (MDP_VP_0_CURSOR_1_SSPP_REG_BASE_OFFS - MDP_VP_0_VIG_0_SSPP_REG_BASE_OFFS);
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_CURSOR_2]               = 0x00000000;
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_CURSOR_3]               = 0x00000000;
  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_CURSOR_4]               = 0x00000000;

  /* Source (SSPP) Layer (Scalar) HW block register offset */
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_NONE]                   = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_VIG_0]                  = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_VIG_1]                  = (MDP_VP_0_VIG_1_QSEED3_REG_BASE_OFFS - MDP_VP_0_VIG_0_QSEED3_REG_BASE_OFFS);
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_VIG_2]                  = (MDP_VP_0_VIG_2_QSEED3_REG_BASE_OFFS - MDP_VP_0_VIG_0_QSEED3_REG_BASE_OFFS);
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_VIG_3]                  = (MDP_VP_0_VIG_3_QSEED3_REG_BASE_OFFS - MDP_VP_0_VIG_0_QSEED3_REG_BASE_OFFS);
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_VIG_4]                  = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_RGB_0]                  = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_RGB_1]                  = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_RGB_2]                  = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_RGB_3]                  = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_RGB_4]                  = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_DMA_0]                  = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_DMA_1]                  = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_DMA_2]                  = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_DMA_3]                  = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_DMA_4]                  = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_CURSOR_0]               = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_CURSOR_1]               = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_CURSOR_2]               = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_CURSOR_3]               = 0x00000000;
  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_CURSOR_4]               = 0x00000000;

  /* Destination scaler HW block register offset*/
  uMDPDestScalerRegBaseOffset[HAL_MDP_DESTINATION_PIPE_NONE]         = 0x00000000;
  uMDPDestScalerRegBaseOffset[HAL_MDP_DESTINATION_PIPE_0]            = (MDP_DEST_SCALER_0_DEST_SCL0_REG_BASE_OFFS - MDP_VP_0_VIG_0_QSEED3_REG_BASE_OFFS);
  uMDPDestScalerRegBaseOffset[HAL_MDP_DESTINATION_PIPE_1]            = (MDP_DEST_SCALER_0_DEST_SCL1_REG_BASE_OFFS - MDP_VP_0_VIG_0_QSEED3_REG_BASE_OFFS);
  uMDPDestScalerRegBaseOffset[HAL_MDP_DESTINATION_PIPE_2]            = 0x00000000;
  uMDPDestScalerRegBaseOffset[HAL_MDP_DESTINATION_PIPE_3]            = 0x00000000;
  uMDPDestScalerRegBaseOffset[HAL_MDP_DESTINATION_PIPE_4]            = 0x00000000;

  /* Vbif register offset */
  uMDPVbifRegBaseOffset[HAL_MDP_VBIF_0]                              = 0x00000000;
  uMDPVbifRegBaseOffset[HAL_MDP_VBIF_1]                              = (VBIF_NRT_MDSS_VBIF_ROT_REG_BASE - VBIF_MDSS_VBIF_SDE_REG_BASE);

  
}



/****************************************************************************
*
** FUNCTION: HAL_MDP_ReadHardwareInfo_4_0_0()
*/
/*!
* \brief
*     Reads the hardware capabilities for the given MDP Version(4.0.0)
*
* \param [out]  psHwInfo            - Hardware information
* \param [out]  psMdpHwInfo         - MDP hardware information 
*
* \retval NONE
*
****************************************************************************/
void HAL_MDP_ReadHardwareInfo_4_0_0(HAL_MDP_HwInfo             *psHwInfo,
                                    HAL_MDP_Hw_Private_Info    *psMdpHwInfo)
{
   // Calculate block offsets for blocks that have multiple instances, ex. VIGs or DSPPs.
   HAL_MDP_SetHWBlockRegOffsets_4_0_0();

   if(NULL != psHwInfo)
   {
      psHwInfo->uNumOfRGBLayers             = HAL_MDP_4_0_0_NUM_OF_RGB_LAYERS;
      psHwInfo->uNumOfVIGLayers             = HAL_MDP_4_0_0_NUM_OF_VIG_LAYERS;
      psHwInfo->uNumOfDMALayers             = HAL_MDP_4_0_0_NUM_OF_DMA_LAYERS;
      psHwInfo->uNumOfCursorLayers          = HAL_MDP_4_0_0_NUM_OF_CURSOR_LAYERS;
      psHwInfo->uNumOfLayerMixers           = HAL_MDP_4_0_0_NUM_OF_LAYER_MIXERS;
      psHwInfo->uNumOfDSPPs                 = HAL_MDP_4_0_0_NUM_OF_DSPP;
      psHwInfo->uNumOfBltEngines            = HAL_MDP_4_0_0_NUM_OF_WRITEBACK_INTERFACES;
      psHwInfo->uNumOfControlPaths          = HAL_MDP_4_0_0_NUM_OF_CONTROL_PATHS;
      psHwInfo->uNumOfDataPaths             = HAL_MDP_4_0_0_NUM_OF_DATA_PATHS;
      psHwInfo->uNumOfLayerMixerBlendStages = HAL_MDP_4_0_0_LAYER_MIXER_MAX_BLEND_STAGES;
      psHwInfo->uNumOfPhyInterfaces         = HAL_MDP_4_0_0_NUM_OF_PHYSICAL_INTERFACES;
      psHwInfo->uNumOfPingPongs             = HAL_MDP_4_0_0_NUM_OF_PINGPONGS;
      psHwInfo->uNumOfWatchDogs             = HAL_MDP_4_0_0_NUM_OF_WATCHDOGS;
      psHwInfo->uNumOfAxiPorts              = HAL_MDP_4_0_0_NUM_OF_AXI_PORTS;
      psHwInfo->uNumOfVbifClients           = HAL_MDP_4_0_0_NUM_OF_VBIF_CLIENTS;       
      psHwInfo->eGamutType                  = HAL_MDP_GAMUT_8BITGAMUT_TYPE;
      psHwInfo->uNumOfChromaDownBlocks      = HAL_MDP_4_0_0_NUM_OF_CHROMA_BLOCKS;
	  psHwInfo->uFeatureFlags               = HAL_MDP_FLAG_FEATURE_PROGRAMMABLE_FETCH_START;
   }

   if(NULL != psMdpHwInfo)
   {
      /** Color Space Conversion(CSC) - Limited REC601 - YUV to RGB */
      psMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_REC601]       =  &gMDP_CSC_YuvToRgb_Limited_Rec601;
      /** Color Space Conversion(CSC) - Limited REC601 - RGB to YUV */
      psMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_REC601]       =  &gMDP_CSC_RgbToYuv_Limited_Rec601;
      /** Color Space Conversion(CSC) - Full REC601 - YUV to RGB */
      psMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC601]  =  &gMDP_CSC_YuvToRgb_Full_Rec601; 
      /** Color Space Conversion(CSC) - Full REC601 - RGB to YUV */
      psMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC601]  =  &gMDP_CSC_RgbToYuv_Full_Rec601;
      /** Color Space Conversion(CSC) - Limited REC709 - YUV to RGB */
      psMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_REC709]       =  &gMDP_CSC_YuvToRgb_Limited_Rec709;
      /** Color Space Conversion(CSC) - Limited REC709 - RGB to YUV */
      psMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_REC709]       =  &gMDP_CSC_RgbToYuv_Limited_Rec709;
      /** Color Space Conversion(CSC) - Full REC709 - YUV to RGB */
      psMdpHwInfo->pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC709]  =  &gMDP_CSC_YuvToRgb_Full_Rec709;
      /** Color Space Conversion(CSC) - Full REC709 - RGB to YUV */
      psMdpHwInfo->pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_FULL_REC709]  =  &gMDP_CSC_RgbToYuv_Full_Rec709;    

      // QSEED
      psMdpHwInfo->pTwoDFilterA                                                        = gTwoDFilterA_LUT; 
      psMdpHwInfo->uTwoDFilterACoeffCnt                                                = sizeof(gTwoDFilterA_LUT)/sizeof(gTwoDFilterA_LUT[0]);            /**< QSEED 2DFilterA Coefficient count */
      psMdpHwInfo->pTwoDFilterB                                                        = gTwoDFilterB_LUT; 
      psMdpHwInfo->uTwoDFilterBCoeffCnt                                                = sizeof(gTwoDFilterB_LUT)/sizeof(gTwoDFilterB_LUT[0]);            /**< QSEED 2DFilterB Coefficient count */
      psMdpHwInfo->pTwoDFilterC                                                        = gTwoDFilterC_LUT; 
      psMdpHwInfo->uTwoDFilterCCoeffCnt                                                = sizeof(gTwoDFilterC_LUT)/sizeof(gTwoDFilterC_LUT[0]);            /**< QSEED 2DFilterC Coefficient count */
      psMdpHwInfo->pTwoDFilterD                                                        = gTwoDFilterD_LUT; 
      psMdpHwInfo->uTwoDFilterDCoeffCnt                                                = sizeof(gTwoDFilterD_LUT)/sizeof(gTwoDFilterD_LUT[0]);            /**< QSEED 2DFilterD Coefficient count */

      // QOS & VBIF
      psMdpHwInfo->uVBIFMaxDdrRdBurstSize                  = HAL_MDP_4_0_0_VBIF_MAX_DDR_RD_BURST_SIZE;
      psMdpHwInfo->uVBIFMaxDdrWrBurstSize                  = HAL_MDP_4_0_0_VBIF_MAX_DDR_WR_BURST_SIZE;
      psMdpHwInfo->uVBIFMaxRdOT                            = HAL_MDP_4_0_0_VBIF_MAX_RD_OT_SIZE;
      psMdpHwInfo->uVBIFMaxWrOT                            = HAL_MDP_4_0_0_VBIF_MAX_WR_OT_SIZE;      
      
      psMdpHwInfo->uVBIFRoundRobinQosArb                   = HAL_MDP_4_0_0_VBIF_ROUND_ROBIN_QOS_ARB;
      psMdpHwInfo->pVBIFQOSRemappers                       = &gSVBIFQOSRemap_4_0_0[0];
      psMdpHwInfo->pVBIFClientToHWIDMap                    = &gsVbifClientToHwIdMap_4_0_0[0];

      //Picture adjustment(HSIC) 
      psMdpHwInfo->sPictureAdjustInfo.iMinHue              = HAL_MDP_4_0_0_PICTURE_ADJUST_MIN_HUE;
      psMdpHwInfo->sPictureAdjustInfo.iMaxHue              = HAL_MDP_4_0_0_PICTURE_ADJUST_MAX_HUE;
      psMdpHwInfo->sPictureAdjustInfo.iMinUnchangeHue      = HAL_MDP_4_0_0_PICTURE_ADJUST_MIN_UNCHANGE_HUE;
      psMdpHwInfo->sPictureAdjustInfo.iMaxUnchangeHue      = HAL_MDP_4_0_0_PICTURE_ADJUST_MAX_UNCHANGE_HUE;
      psMdpHwInfo->sPictureAdjustInfo.iMinSaturation       = HAL_MDP_4_0_0_PICTURE_ADJUST_MIN_SATURATION;
      psMdpHwInfo->sPictureAdjustInfo.iMaxSaturation       = HAL_MDP_4_0_0_PICTURE_ADJUST_MAX_SATURATION;
      psMdpHwInfo->sPictureAdjustInfo.iUnchangeSaturation  = HAL_MDP_4_0_0_PICTURE_ADJUST_UNCHANGE_SATURATION;
      psMdpHwInfo->sPictureAdjustInfo.iMinIntensity        = HAL_MDP_4_0_0_PICTURE_ADJUST_MIN_INTENSITY;
      psMdpHwInfo->sPictureAdjustInfo.iMaxIntensity        = HAL_MDP_4_0_0_PICTURE_ADJUST_MAX_INTENSITY;
      psMdpHwInfo->sPictureAdjustInfo.iUnchangeIntensity   = HAL_MDP_4_0_0_PICTURE_ADJUST_UNCHANGE_INTENSITY;
      psMdpHwInfo->sPictureAdjustInfo.iMinContrast         = HAL_MDP_4_0_0_PICTURE_ADJUST_MIN_CONTRAST;
      psMdpHwInfo->sPictureAdjustInfo.iMaxContrast         = HAL_MDP_4_0_0_PICTURE_ADJUST_MAX_CONTRAST;
      psMdpHwInfo->sPictureAdjustInfo.iUnchangeContrast    = HAL_MDP_4_0_0_PICTURE_ADJUST_UNCHANGE_CONTRAST;

      psMdpHwInfo->uHysterisisValue                        = HAL_MDP_4_0_0_HYSTERISIS_DEFAULT;

      // Histogram config function

      // Histogram lock function
      
      // Initialize the module's status register details
      psMdpHwInfo->gpsModuleStatusRegInfo[HAL_MDP_MODULE_TYPE_NONE]              = (uint32 *)NULL;
      psMdpHwInfo->gpsModuleStatusRegInfo[HAL_MDP_MODULE_SOURCE_PIPE]            = (uint32 *)&gsSSPPModuleStatusRegInfo;
      psMdpHwInfo->gpsModuleStatusRegInfo[HAL_MDP_MODULE_LAYERMIXER]             = (uint32 *)NULL;
      psMdpHwInfo->gpsModuleStatusRegInfo[HAL_MDP_MODULE_DESTINATION_PIPE]       = (uint32 *)&gsDSPPStatusRegInfo;
      psMdpHwInfo->gpsModuleStatusRegInfo[HAL_MDP_MODULE_INTERFACE]              = (uint32 *)NULL;
      psMdpHwInfo->gpsModuleStatusRegInfo[HAL_MDP_MODULE_CONTROL_PATH]           = (uint32 *)NULL;
      psMdpHwInfo->gpsModuleStatusRegInfo[HAL_MDP_MODULE_DATA_PATH]              = (uint32 *)NULL;
      psMdpHwInfo->gpsModuleStatusRegInfo[HAL_MDP_MODULE_SMP]                    = (uint32 *)NULL;
      psMdpHwInfo->gpsModuleStatusRegInfo[HAL_MDP_MODULE_TRAFFIC_SHAPER]         = (uint32 *)NULL;
      psMdpHwInfo->gpsModuleStatusRegInfo[HAL_MDP_MODULE_VBIF]                   = (uint32 *)NULL;
      psMdpHwInfo->gpsModuleStatusRegInfo[HAL_MDP_MODULE_PINGPONG]               = (uint32 *)&gsPingPongStatusRegInfo;
      psMdpHwInfo->gpsModuleStatusRegInfo[HAL_MDP_MODULE_PHYSICAL_INTERFACE]     = (uint32 *)NULL;
      psMdpHwInfo->gpsModuleStatusRegInfo[HAL_MDP_MODULE_WRITEBACK_INTERFACE]    = (uint32 *)&gsWBStatusRegInfo;

      psMdpHwInfo->eCursorIntLayerType                = HAL_MDP_LAYER_TYPE_SSPP_CURSOR;
      psMdpHwInfo->uCursorBlendOrderIndex             = 8;

      psMdpHwInfo->uDefaultHighestBankBit             = HAL_MDP_4_0_0_HIGHEST_BANK_BIT_DEFAULT;

      psMdpHwInfo->eMisrCfgMethod                     = HAL_MDP_MISR_CONFIG_METHOD_0;
      //DST PACK PATTERN
      psMdpHwInfo->pDstPackPatternInfo                = (uint32 *)&guSrcUnpackInfo;

      psMdpHwInfo->eBWC                               = HAL_MDP_COMPRESSION_UBWC;
      psMdpHwInfo->eMacroTileFormat                   = HAL_MDP_MACRO_TILE_A5X;
   }
}


#ifdef __cplusplus
}
#endif
