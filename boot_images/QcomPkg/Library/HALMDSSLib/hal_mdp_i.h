/*! \file */

/*
===========================================================================

FILE:         hal_mdp_i.h

DESCRIPTION:  Internal Header file for MDP HAL.  This file should not be
              Used outside of MDP HAL
  

===========================================================================
Copyright (c) 2012- 2017 Qualcomm Technologies, Inc. All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
===========================================================================
*/
#ifndef _HAL_MDP_I_H
#define _HAL_MDP_I_H

/*------------------------------------------------------------------------------
 * Include Files
 *----------------------------------------------------------------------------*/
#include "hal_mdp.h"
#include "mdsshwio.h"
/* !! ALERT !!
 * DO NOT INCLUDE ANY HEADER FILES OTHER THAN hal_mdp.h and mdsshwio.h
 */

/* -----------------------------------------------------------------------
** Preprocessor Defintions and Constants
** ----------------------------------------------------------------------- */
#define HAL_MDP_ASSERT()

/* Alignment macro to align to the next available multiple */
#define HAL_ALIGN_DATA(_a_, _b_)   (((_a_) + ((_b_) - 1)) & (~((_b_) - 1)))

/* Ceil function to round off to the nearest interger */
#define HAL_CEIL(x, y)             (((double)(x) + ((double)(y)-1)) / (double)(y))

/* Returns the MAX of two values */
#define HAL_MAX(_a_,_b_)           (((_a_) > (_b_)) ? (_a_) : (_b_))


/* -----------------------------------------------------------------------
** Color Format Selection (MDP_DMA_P_CONFIG and MDP_PPP_OUT_FORMAT)
** ----------------------------------------------------------------------- */

/*
 * The following defines are used to configure the HW registers.
 *
 * The values might be vary for different MDP version.
 *
 *    @@ TODO @@: Required to update the defines based on MDP version i.e HAL_MDP_<major>_<minor>_<step>_XXX
 */ 
#define HAL_MDP_SRC_FORMAT_FRAME_FORMAT_NONE                0
#define HAL_MDP_SRC_FORMAT_FRAME_FORMAT_LINEAR              0        /**< Buffer Layer Out - Linear      */
#define HAL_MDP_SRC_FORMAT_FRAME_FORMAT_TILE                2        /**< Buffer Layer Out - Tile        */
#define HAL_MDP_SRC_FORMAT_DX_FORMAT_ON                     1        /**< 10 bit pixel format            */
#define HAL_MDP_SRC_FORMAT_DX_FORMAT_OFF                    0        /**< non 10 bit pixel format        */


#define HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_NONE               0
#define HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_RGB                0
#define HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV444             0
#define HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV422_H2V1        1
#define HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV422_H1V2        2
#define HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV420_N2V1        3
#define HAL_MDP_SRC_FORMAT_CHROMA_SAMPLE_YUV420_N1V2        4

#define HAL_MDP_SRC_FORMAT_FETCH_PLANES_INTERLEAVED         0
#define HAL_MDP_SRC_FORMAT_FETCH_PLANES_PLANAR              1
#define HAL_MDP_SRC_FORMAT_FETCH_PLANES_PSUEDO_PLANAR       2

#define HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_NONE             0
#define HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_LSB              0
#define HAL_MDP_SRC_FORMAT_UNPACK_ALIGN_TO_MSB              1

#define HAL_MDP_SRC_FORMAT_UNPACK_NONE                      0
#define HAL_MDP_SRC_FORMAT_UNPACK_LOOSE                     0
#define HAL_MDP_SRC_FORMAT_UNPACK_TIGHT                     1

#define HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT_NONE      0
#define HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT0          0
#define HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT1          0
#define HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT2          1
#define HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT3          2
#define HAL_MDP_SRC_FORAMT_UNPACK_COMPONENT_COUNT4          3

#define HAL_MDP_SRC_FORMAT_SRC_NO_BPP                       0
#define HAL_MDP_SRC_FORMAT_SRC_ONE_BPP                      0
#define HAL_MDP_SRC_FORMAT_SRC_TWO_BPP                      1
#define HAL_MDP_SRC_FORMAT_SRC_THREE_BPP                    2
#define HAL_MDP_SRC_FORMAT_SRC_FOUR_BPP                     3

#define HAL_MDP_SRC_FORMAT_ALPHA_NOT_PRESENT                0
#define HAL_MDP_SRC_FORMAT_ALPHA_PRESENT                    1

/**< MDP HW COLOR COMPONENT 3 (ALPHA) */
#define HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_0                0
#define HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_1                0
#define HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_4                1
#define HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_6                2
#define HAL_MDP_SRC_FORMAT_ALPHA_COMP_BITS_8                3

/**< MDP HW COLOR COMPONENT 2 (RED/Cr) */
#define HAL_MDP_SRC_FORMAT_RED_COMP_BITS_0                  0
#define HAL_MDP_SRC_FORMAT_RED_COMP_BITS_4                  0
#define HAL_MDP_SRC_FORMAT_RED_COMP_BITS_5                  1
#define HAL_MDP_SRC_FORMAT_RED_COMP_BITS_6                  2
#define HAL_MDP_SRC_FORMAT_RED_COMP_BITS_8                  3

/**< MDP HW COLOR COMPONENT 1 (BLUE/Cb) */
#define HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_0                 0
#define HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_4                 0
#define HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_5                 1
#define HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_6                 2
#define HAL_MDP_SRC_FORMAT_BLUE_COMP_BITS_8                 3

/**< MDP HW COLOR COMPONENT 0 (GREEN/Luma) */
#define HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_0                0
#define HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_4                0
#define HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_5                1
#define HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_6                2
#define HAL_MDP_SRC_FORMAT_GREEN_COMP_BITS_8                3

#define HAL_MDP_CURSOR_PIXEL_FORMAT_ARGB8888                0
#define HAL_MDP_CURSOR_PIXEL_FORMAT_ARGB1555                2
#define HAL_MDP_CURSOR_PIXEL_FORMAT_ARGB4444                4

#define HAL_MDP_CURSOR_BLEND_CONSTANT                       0
#define HAL_MDP_CURSOR_BLEND_ALPHA                          2


/**< Scale filter Mode */
#define HAL_MDP_SCALAR_NEAREST_NEIGHBOR_FILTER              0
#define HAL_MDP_SCALAR_BILINEAR_FILTER                      1
#define HAL_MDP_SCALAR_PCMN_FILTER                          2
#define HAL_MDP_SCALAR_CAF_FILTER                           3

#define HAL_MDP_ROTATOR_BLOCK_SIZE_64x64_TILE               0
#define HAL_MDP_ROTATOR_BLOCK_SIZE_128x128_TILE             1

/**
 * MDP Features with table programmings
 */
/** MDP SMP Features */
#define     HAL_MDP_SMP_FEATURE_NONE              0x00000000

/** MDP SSPP or Layer Features */
#define     HAL_MDP_SSPP_FEATURE_NONE             0x00000000
#define     HAL_MDP_SSPP_FEATURE_QSEED2           HAL_MDSS_BIT(0)
#define     HAL_MDP_SSPP_FEATURE_CSC_1            HAL_MDSS_BIT(1)
#define     HAL_MDP_SSPP_FEATURE_SPA_CE           HAL_MDSS_BIT(2)
#define     HAL_MDP_SSPP_FEATURE_IGC_LUT          HAL_MDSS_BIT(3)
#define     HAL_MDP_SSPP_FEATURE_RGB_SCALE        HAL_MDSS_BIT(4)

/** MDP Layer Mixer Features */
#define     HAL_MDP_LM_FEATURE_NONE               0x00000000
#define     HAL_MDP_LM_GC_LUT                     HAL_MDSS_BIT(5)

/** MDP DSPP Features */
#define     HAL_MDP_DSPP_FEATURE_NONE             0x00000000
#define     HAL_MDP_DSPP_FEATURE_PA               HAL_MDSS_BIT(6)
#define     HAL_MDP_DSPP_FEATURE_IGC_LUT          HAL_MDSS_BIT(7)
#define     HAL_MDP_DSPP_FEATURE_GAMUT_MAPPING    HAL_MDSS_BIT(8)
#define     HAL_MDP_DSPP_FEATURE_PC_LUT           HAL_MDSS_BIT(9)
#define     HAL_MDP_DSPP_FEATURE_DITHER           HAL_MDSS_BIT(10)
#define     HAL_MDP_DSPP_FEATURE_SCALER           HAL_MDSS_BIT(11)


/** MDP DATA-PATH Features */
#define     HAL_MDP_DATAPATH_FEATURE_NONE         0x00000000
#define     HAL_MDP_DATAPATH_FEATURE_CSC          HAL_MDSS_BIT(0)

/** MDP 3D or DUAL PIPE muxer Features */
#define     HAL_MDP_MUXER_FEATURE_NONE            0x00000000

/** MDP CDM Features */
#define     HAL_MDP_CDM_FEATURE_NONE         0x00000000
#define     HAL_MDP_CDM_FEATURE_CSC          HAL_MDSS_BIT(0)
#define     HAL_MDP_CDM_FEATURE_CDWN2        HAL_MDSS_BIT(1)

#define HAL_MDP_APICAL_CONFIG_BUFFER_BLOCK_REGS_ACCESS  0x1
#define HAL_MDP_APICAL_CONFIG_BUFFER_UPDATE_REGS_ON_VBI 0x2

/** < Helper Macro Definitions */

#define HAL_MDP_INTR_STATUS_PING_PONG_RD_PTR_BMSK (HWIO_MDP_INTR_STATUS_PING_PONG_3_RD_PTR_BMSK    | \
                                                   HWIO_MDP_INTR_STATUS_PING_PONG_2_RD_PTR_BMSK    | \
                                                   HWIO_MDP_INTR_STATUS_PING_PONG_1_RD_PTR_BMSK    | \
                                                   HWIO_MDP_INTR_STATUS_PING_PONG_0_RD_PTR_BMSK    )

#define HAL_MDP_INTR_STATUS_PING_PONG_DONE_BMSK   (HWIO_MDP_INTR_STATUS_PING_PONG_3_DONE_BMSK      | \
                                                   HWIO_MDP_INTR_STATUS_PING_PONG_2_DONE_BMSK      | \
                                                   HWIO_MDP_INTR_STATUS_PING_PONG_1_DONE_BMSK      | \
                                                   HWIO_MDP_INTR_STATUS_PING_PONG_0_DONE_BMSK      )


/** MDP HW Workaround bits */
#define     HAL_MDP_WORKAROUND_QCTDD00872136       HAL_MDSS_BIT(0)  // SSPP_CURRENT_SRC_ADDR limtiation


/*!
* \enum HAL_MDP_DSC_EncoderConfig
*
* The HAL_MDP_DSC_EncoderConfig enum defines the DSC topology, based on this we can identify if it is single
  DSI or split, with Merge or 3d Mux at the encoder level. 
*
*/
typedef enum _HAL_MDP_DSC_EncoderConfig
{
  HAL_MDP_DSC_SINGLE_ENC_SINGLE_DSI     = 0x0,          /* Single encoder -> Single DSI e.g. 2.5k on DSI0 or DSI1, higher MDP clock*/
  HAL_MDP_DSC_DUAL_ENC_SINGLE_DSI       = 0x1,          /* two encoders that merge to a single DSI e.g. 2.5k on DSI0 or DSI1, lower MDP clock*/
  HAL_MDP_DSC_DUAL_ENC_DUAL_DSI         = 0x2,          /* Dual DSI with one dsc encoder each e.g. 4k*/

} HAL_MDP_DSC_EncoderConfig;

/*!
* \enum HAL_MDP_PanicLevelType
*
* The HAL_MDP_PanicLevelType lists 4 levels of panic levels to be sent to BMIC
*
*/
typedef enum _HAL_MDP_PanicLevelType
{
  HAL_MDP_PANIC_LEVEL_LEAST_URGENT = 0x0,          /* Active MDP buffers are between 75% ~ 100% Full*/
  HAL_MDP_PANIC_LEVEL_LESS_URGENT  = 0x1,          /* Active MDP buffers are between 50% ~ 75%  Full*/
  HAL_MDP_PANIC_LEVEL_MORE_URGENT  = 0x2,          /* Active MDP buffers are between 25% ~ 50%  Full*/
  HAL_MDP_PANIC_LEVEL_MOST_URGENT  = 0x3,          /* Active MDP buffers are between 0%  ~ 25%  Full*/
} HAL_MDP_PanicLevelType;

/*!
* \enum HAL_MDP_RobustSignalType
*
* The HAL_MDP_RobustSignalType lists ON/OFF state of robust/safe signal be sent to BMIC
*
*/
typedef enum _HAL_MDP_RobustSignalType
{
  HAL_MDP_ROBUST_SIGNAL_OFF        = 0x0,          /* Robust/Safe Signal is OFF, MDP buffers don't have enough data, BMIC is not safe to reduce BW to MDP */
  HAL_MDP_ROBUST_SIGNAL_ON         = 0x1,          /* Robust/Safe Signal is ON,  MDP buffers have enough data, BMIC is safe to reduce BW to MDP           */
} HAL_MDP_RobustSignalType;
#define  HAL_MDP_DEFAULT_HIGHEST_BANK_BIT         (3)

/**
 * MDP 1.0.0 Version
 * ===========================================================================================================
 */
#define  HAL_MDP_VIG_LAYER_DEFAULT_FEATURE_INIT         (HAL_MDP_SSPP_FEATURE_CSC_1 | HAL_MDP_SSPP_FEATURE_QSEED2)

#define  HAL_MDP_RGB_LAYER_DEFAULT_FEATURE_INIT         (HAL_MDP_SSPP_FEATURE_RGB_SCALE | \
                                                         HAL_MDP_SSPP_FEATURE_IGC_LUT )

#define  HAL_MDP_DMA_LAYER_DEFAULT_FEATURE_INIT         0x00000000

#define  HAL_MDP_LM_DEFAULT_FEATURE_INIT                HAL_MDP_LM_FEATURE_NONE

#define  HAL_MDP_DSPP_DEFAULT_FEATURE_INIT              (HAL_MDP_DSPP_FEATURE_NONE           |  \
                                                         HAL_MDP_DSPP_FEATURE_IGC_LUT        |  \
                                                         HAL_MDP_DSPP_FEATURE_DITHER         |  \
                                                         HAL_MDP_DSPP_FEATURE_SCALER)

#define  HAL_MDP_MUXER_DEFAULT_FEATURE_INIT             HAL_MDP_MUXER_FEATURE_NONE

#define  HAL_MDP_DATAPATH_DEFAULT_FEATURE_INIT          (HAL_MDP_DATAPATH_FEATURE_CSC)

#define  HAL_MDP_CHROMABLOCK_DEFAULT_FEATURE_INIT       (HAL_MDP_CDM_FEATURE_CSC            |  \
                                                         HAL_MDP_CDM_FEATURE_CDWN2)

/** MDP Version Specific */


#define HAL_MDP_PA_HUE_INPUT_RANGE                      180  // Maximum range allow for input hue
#define HAL_MDP_PA_DEFAULT_INPUT_RANGE                  100  // Maximum range allow to the PA block

/** Picture adjustment(HSIC) limits */
#define HAL_MDP_PICTURE_ADJUST_MIN_HUE                  (-180) 
#define HAL_MDP_PICTURE_ADJUST_MAX_HUE                  180  
#define HAL_MDP_PICTURE_ADJUST_MIN_SATURATION           (-128)   
#define HAL_MDP_PICTURE_ADJUST_MAX_SATURATION           127
#define HAL_MDP_PICTURE_ADJUST_MIN_INTENSITY            (-128) 
#define HAL_MDP_PICTURE_ADJUST_MAX_INTENSITY            127 
#define HAL_MDP_PICTURE_ADJUST_MIN_CONTRAST             (-128)
#define HAL_MDP_PICTURE_ADJUST_MAX_CONTRAST             127


#define  HAL_MDP_PHASE_STEP_CONSTANT                    21

#define HAL_MDP_QOS_REMAPPER_INFO( ReqPriority0, ReqPriority1, ReqPriority2, ReqPriority3 ) \
                                                              (ReqPriority0 << HWIO_MDP_QOS_REMAPPER_CLASS_0_REQPRIORITY_0_SHFT | \
                                                               ReqPriority1 << HWIO_MDP_QOS_REMAPPER_CLASS_0_REQPRIORITY_1_SHFT | \
                                                               ReqPriority2 << HWIO_MDP_QOS_REMAPPER_CLASS_0_REQPRIORITY_2_SHFT | \
                                                               ReqPriority3 << HWIO_MDP_QOS_REMAPPER_CLASS_0_REQPRIORITY_3_SHFT )

#define HAL_MDP_MAX_RETRIES_FOR_CTRL_PATH_RESET_COMPLETE      32

#define HAL_MDP_QOS_REQPRI_WATERMARK_DEFAULT_REGS_COUNT       3

#define HAL_MDP_PANIC_LUT0_INFO( PanicLut0, PanicLut1, PanicLut2, PanicLut3, PanicLut4, PanicLut5, PanicLut6, PanicLut7) \
                                                             (PanicLut0 << HWIO_MDP_PANIC_LUT0_FL_0_SHFT | \
                                                              PanicLut1 << HWIO_MDP_PANIC_LUT0_FL_1_SHFT | \
                                                              PanicLut2 << HWIO_MDP_PANIC_LUT0_FL_2_SHFT | \
                                                              PanicLut3 << HWIO_MDP_PANIC_LUT0_FL_3_SHFT | \
                                                              PanicLut4 << HWIO_MDP_PANIC_LUT0_FL_4_SHFT | \
                                                              PanicLut5 << HWIO_MDP_PANIC_LUT0_FL_5_SHFT | \
                                                              PanicLut6 << HWIO_MDP_PANIC_LUT0_FL_6_SHFT | \
                                                              PanicLut7 << HWIO_MDP_PANIC_LUT0_FL_7_SHFT)

#define HAL_MDP_PANIC_LUT1_INFO( PanicLut8, PanicLut9, PanicLut10, PanicLut11, PanicLut12, PanicLut13, PanicLut14, PanicLut15) \
                                                             (PanicLut8  << HWIO_MDP_PANIC_LUT1_FL_8_SHFT  | \
                                                              PanicLut9  << HWIO_MDP_PANIC_LUT1_FL_9_SHFT  | \
                                                              PanicLut10 << HWIO_MDP_PANIC_LUT1_FL_10_SHFT | \
                                                              PanicLut11 << HWIO_MDP_PANIC_LUT1_FL_11_SHFT | \
                                                              PanicLut12 << HWIO_MDP_PANIC_LUT1_FL_12_SHFT | \
                                                              PanicLut13 << HWIO_MDP_PANIC_LUT1_FL_13_SHFT | \
                                                              PanicLut14 << HWIO_MDP_PANIC_LUT1_FL_14_SHFT | \
                                                              PanicLut15 << HWIO_MDP_PANIC_LUT1_FL_15_SHFT)

#define HAL_MDP_ROBUST_LUT_INFO(RobustLut0, RobustLut1, RobustLut2,  RobustLut3,  RobustLut4,  RobustLut5,  RobustLut6,  RobustLut7, \
                                RobustLut8, RobustLut9, RobustLut10, RobustLut11, RobustLut12, RobustLut13, RobustLut14, RobustLut15) \
                                                             (RobustLut0  << HWIO_MDP_ROBUST_LUT_FL_0_SHFT  | \
                                                              RobustLut1  << HWIO_MDP_ROBUST_LUT_FL_1_SHFT  | \
                                                              RobustLut2  << HWIO_MDP_ROBUST_LUT_FL_2_SHFT  | \
                                                              RobustLut3  << HWIO_MDP_ROBUST_LUT_FL_3_SHFT  | \
                                                              RobustLut4  << HWIO_MDP_ROBUST_LUT_FL_4_SHFT  | \
                                                              RobustLut5  << HWIO_MDP_ROBUST_LUT_FL_5_SHFT  | \
                                                              RobustLut6  << HWIO_MDP_ROBUST_LUT_FL_6_SHFT  | \
                                                              RobustLut7  << HWIO_MDP_ROBUST_LUT_FL_7_SHFT  | \
                                                              RobustLut8  << HWIO_MDP_ROBUST_LUT_FL_8_SHFT  | \
                                                              RobustLut9  << HWIO_MDP_ROBUST_LUT_FL_9_SHFT  | \
                                                              RobustLut10 << HWIO_MDP_ROBUST_LUT_FL_10_SHFT | \
                                                              RobustLut11 << HWIO_MDP_ROBUST_LUT_FL_11_SHFT | \
                                                              RobustLut12 << HWIO_MDP_ROBUST_LUT_FL_12_SHFT | \
                                                              RobustLut13 << HWIO_MDP_ROBUST_LUT_FL_13_SHFT | \
                                                              RobustLut14 << HWIO_MDP_ROBUST_LUT_FL_14_SHFT | \
                                                              RobustLut15 << HWIO_MDP_ROBUST_LUT_FL_15_SHFT)

/** SSPP QoS  related defines */
#define HAL_MDP_QOS_LUT_TABLE_SIZE                            16  // Maximum LUT table size of Danger, Safe and Creq LUT

/*=========================================================================================================== */

/*!
 * \enum HAL_MDP_PhysicalInterfaceId (Only for internal. Shoulb not be used in APIs)
 *
 * The \b HAL_MDP_PhysicalInterfaceId lists the physical interface connections from MDP HW
 *
 * HAL_MDP_##version##_NUM_OF_PHYSICAL_INTERFACES provides the maximum physical interfaces present in the HW
 *
 */
typedef enum _HAL_MDP_PhyInterfaceId
{
   HAL_MDP_PHYSICAL_INTERFACE_ID_NONE        = 0x00000000,
   HAL_MDP_PHYSICAL_INTERFACE_0,             /**< INTF_0 */
   HAL_MDP_PHYSICAL_INTERFACE_1,             /**< INTF_1 */
   HAL_MDP_PHYSICAL_INTERFACE_2,             /**< INTF_2 */
   HAL_MDP_PHYSICAL_INTERFACE_3,             /**< INTF_3 */
   HAL_MDP_PHYSICAL_INTERFACE_4,             /**< INTF_4 */
   HAL_MDP_PHYSICAL_INTERFACE_5,             /**< INTF_5 */
   HAL_MDP_PHYSICAL_INTERFACE_MAX,
   HAL_MDP_PHYSICAL_INTERFACE_FORCE_32BIT    = 0x7FFFFFFF

} HAL_MDP_PhyInterfaceId;

/*!
 * \enum HAL_MDP_WritebackInterfaceId (Only for internal. Shoulb not be used in APIs)
 *
 * The \b HAL_MDP_WritebackInterfaceId lists the write back interface connections in MDP HW
 *
 * HAL_MDP_##version##_NUM_OF_WRITEBACK_INTERFACES provides the maximum physical interfaces present in the HW
 *
 */
typedef enum _HAL_MDP_WritebackInterfaceId
{
   HAL_MDP_WRITEBACK_INTERFACE_ID_NONE        = 0x00000000,
   HAL_MDP_WRITEBACK_INTERFACE_0,             /**< WB0 */
   HAL_MDP_WRITEBACK_INTERFACE_1,             /**< WB1 */
   HAL_MDP_WRITEBACK_INTERFACE_2,             /**< WB2 */
   HAL_MDP_WRITEBACK_INTERFACE_3,             /**< WB3 */
   HAL_MDP_WRITEBACK_INTERFACE_4,             /**< WB4 */
   HAL_MDP_WRITEBACK_INTERFACE_5,             /**< WB5 */
   HAL_MDP_WRITEBACK_INTERFACE_MAX,
   HAL_MDP_WRITEBACK_INTERFACE_FORCE_32BIT    = 0x7FFFFFFF

} HAL_MDP_WritebackInterfaceId;

/*!
 * \enum HAL_MDP_MISRConfigMethod is used to configure the MDP interface's MISR block.
 *
 * The register configuration and programming methods are different on each MDP version.
 *
 * MDP 1.0.x to 1.4.x supports method 0
 * MDP 1.5.x and 1.7.0 supports method 1
 *
 * (TODO) Add the information for other MDP revisions
 */
typedef enum _HAL_MDP_MISRConfigMethod
{
   HAL_MDP_MISR_CONFIG_METHOD_NONE           = 0x00000000,
   HAL_MDP_MISR_CONFIG_METHOD_0,             /**< Method 0             */
   HAL_MDP_MISR_CONFIG_METHOD_1,             /**< Method 1             */  
   HAL_MDP_MISR_CONFIG_METHOD_2,             /**< Method 2. (Reserved) */
   HAL_MDP_MISR_CONFIG_METHOD_MAX,
   HAL_MDP_MISR_CONFIG_METHOD_FORCE_32BIT    = 0x7FFFFFFF
} HAL_MDP_MISRConfigMethod;

/*!
 * \enum HAL_MDP_MacroTileType (Only for internal. Should not be used in APIs)
 *
 * The \b HAL_MDP_MacroTileType specifies macrotile Id supported by HAL driver.
 *
 */
typedef enum _HAL_MDP_MacroTileType
{
   HAL_MDP_MACRO_TILE_NONE                  = 0x00000000,
   HAL_MDP_MACRO_TILE_A4X                   = 0x00000001,
   HAL_MDP_MACRO_TILE_A5X                   = 0x00000002,
   HAL_MDP_MACRO_TILE_MAX,
   HAL_MDP_MACRO_TILE_FORCE_32BIT           = 0x7FFFFFFF
} HAL_MDP_MacroTileType;

/*!
 * \enum HAL_MDP_MaxBurstSizeType (Only for internal. Shoulb not be used in APIs)
 *
 * The \b HAL_MDP_MaxBurstSizeType specifies max burst beat size.
 *
 */
typedef enum _HAL_MDP_MaxBurstSizeType
{
   HAL_MDP_MAX_BURST_SIZE_16               = 0x00000000,
   HAL_MDP_MAX_BURST_SIZE_1                = 0x00000001,
   HAL_MDP_MAX_BURST_SIZE_2                = 0x00000002,
   HAL_MDP_MAX_BURST_SIZE_4                = 0x00000004,
   HAL_MDP_MAX_BURST_SIZE_8                = 0x00000008,
   HAL_MDP_MAX_BURST_SIZE_32BIT            = 0x7FFFFFFF
} HAL_MDP_MaxBurstSizeType;

/*!
 * \enum HAL_MDP_BandwidthCompressionType (Only for internal. Shoulb not be used in APIs)
 *
 * The \b HAL_MDP_BandwidthCompressionType specifies compression type supported by HW.
 *
 */
typedef enum _HAL_MDP_BandwidthCompressionType
{
   HAL_MDP_COMPRESSION_NONE                  = 0x00000000,
   HAL_MDP_COMPRESSION_BWC                   = 0x00000001,
   HAL_MDP_COMPRESSION_UBWC                  = 0x00000002,
   HAL_MDP_COMPRESSION_MAX,
   HAL_MDP_COMPRESSION_FORCE_32BIT           = 0x7FFFFFFF
} HAL_MDP_BandwidthCompressionType;

typedef struct _HAL_MDP_PictureAdjustInfo
{
   int32      iMinHue;
   int32      iMaxHue;
   int32      iMinUnchangeHue;
   int32      iMaxUnchangeHue;
   int32      iMinSaturation;
   int32      iMaxSaturation;
   int32      iUnchangeSaturation;
   int32      iMinIntensity;
   int32      iMaxIntensity;
   int32      iUnchangeIntensity;
   int32      iMinContrast;
   int32      iMaxContrast;
   int32      iUnchangeContrast;
}HAL_MDP_PictureAdjustInfo;

/*
 * This function table contains HAL functions to program different MDP IP core implementations based on MDP versions.
 * The function pointers should be set in hal_mdp_1_x_0.c for specific implementation for MDP 1.x.0.
 */
typedef struct 
{
  /** Configure DSPP histogram */
  void (*HistogramConfig) (uintPtr uRegisterOffset, HAL_MDP_HistogramConfigType *psHistConfig);

  /** Histogram buffer lock */
  void (*HistogramLock) (uintPtr uRegisterOffset, bool32 bLock);

  /** VBIF programming */
  void (*Vbif_HwDefaultConfig) (HAL_MDP_VbifId eVbifId);
  
  /** Watermark programming */
  HAL_MDSS_ErrorType (*WatermarkLvlConfig)(HAL_MDP_TrafficCtrl_LatencyType  eLatency,
                                           uint32                           uTotalSMPBytes,
                                           uint32                           uFlags,
                                           uintPtr                          uRegAddr);

  /** Swapping macro-tile color component red and blue for pack/unpack pattern */
  uint32 (*SwapMacroTileColorComponent)(HAL_MDP_PixelFormatType ePixelFormat, uint32 uPatternValue);

  /** SSPP QoS Programming */
  void (*SSPPQosConfig)(HAL_MDP_SourcePipeId              eSourcePipe,
                        HAL_MDP_TrafficCtrl_LatencyType   eLatency,
                        HAL_MDP_Qos_ConfigType           *psQoSInfo,
                        uint32                            uFlags);

  /** Rotator SSPP QoS Programming */
  void (*RotSSPPQosConfig)(HAL_MDP_TrafficCtrl_LatencyType   eLatency,
                           HAL_MDP_Qos_ConfigType           *psQoSInfo,
                           uint32                            uFlags);
  
} HAL_MDP_IpFxnTbl;


/* Color Space Conversion(CSC) encoding info */ 
typedef struct 
{
   uint32            *pCSC_Coeff_Matrix;

   uint16            *pCSC_PreClamp;         // pointer to array storing Pre-Clamp values for 8 bit CSC HW 
   uint16            *pCSC_PostClamp;        // pointer to array storing Post-Clamp values for 8 bit CSC HW 
   uint16            *pCSC_PreBias;          // pointer to array storing Post-Bias values for 8 bit CSC HW 
   uint16            *pCSC_PostBias;         // pointer to array storing Post-Bias values for 8 bit CSC HW 

   uint32            *pCSC10Bit_PreClamp;    // pointer to array storing Pre-Clamp values for 10 bit CSC HW 
   uint32            *pCSC10Bit_PostClamp;   // pointer to array storing Post-Clamp values for 10 bit CSC HW 
   uint16            *pCSC10Bit_PreBias;     // pointer to array storing Post-Bias values for 10 bit CSC HW 
   uint16            *pCSC10Bit_PostBias;    // pointer to array storing Post-Bias values for 10 bit CSC HW 

}HAL_MDP_CSCEncodingInfo; 

/**
 * HAL_MDP_ModuleStatusRegInfo
 *
 * MDP has a set of MDP_CLK_STATUSx registers to read the active/idle state of the MDP modules (SSPP, DSPP etc).
 * The following data-structure is used to prepare a mapping table information of MDP_CLK_STATUSx register and BitFieldMask for each MDP module
 *
 *    (a)   gsSSPPModuleStatusRegInfo        --> Mapping table for SSPPx (VIGx, RGBx, CURSORx and DMAx) modules
 *    (b)   gsDSPPStatusRegInfo              --> Mapping table for DSPPx modules
 *    (c)   gsPingPongStatusRegInfo          --> Mapping table for PING-PONGx modules
 *    (c)   gsWBStatusRegInfo                --> Mapping table for WBx modules
 */
typedef struct _HAL_MDP_ModuleStatusRegInfo
{
   uint32            uRegAddress;            /**< Register address offset from MDP_REG_BASE                       */
   uint32            uBitFieldMask;          /**< Bit field mask value to extract the module's status information */

} HAL_MDP_ModuleStatusRegInfo;

/* HAL_MDP_VBIFQOSRemappersType
 *
 * Mapping of VBIF QOS  clients to remapper register register values.
 */
typedef struct _HAL_MDP_VBIFQOSRemappersType
{
   uint32             uVBIFQosRemapper00;            /*QOS remapper register settings for register 00       */
   uint32             uVBIFQosRemapper01;            /*QOS remapper register settings for register 01       */
   uint32             uVBIFQosRemapper10;            /*QOS remapper register settings for register 10       */
   uint32             uVBIFQosRemapper11;            /*QOS remapper register settings for register 11       */
}  HAL_MDP_VBIFQOSRemappersType;

/* HAL_MDP_PanicRobustSettingType
*
* Lut for panic robust setting at different fill level
*/
typedef struct _HAL_MDP_PanicRobustSettingType
{
  uint32             uPanicLut0;                    /*Look Up Table for panic level at fill level 0~7      */
  uint32             uPanicLut1;                    /*Look Up Table for panic level at fill level 8~15     */
  uint32             uRobutstLut;                   /*Look Up Table for robust signal at fill level 0~15   */
}HAL_MDP_PanicRobustSettingType;

/* HAL_MDP_LutInfo
 *
 * Structure to keep SSPP QoS LUT info
 */
typedef struct _HAL_MDP_LutInfo
{
  uint32 uMaxEntryIndex;                          /* LUT max enrty index equivalent to (Table size -1)       */
  uint32 uZeroIndexLutLevel;                      /* Minimum LUT Level corressponding to Index zero of table */
  uint32 uLutData[HAL_MDP_QOS_LUT_TABLE_SIZE];    /*  LUT data table, Index 0 will corresponds to LUT level uZeroIndexLutLevel, 1 to (uZeroIndexLutLevel+1)
                                                      and so on, index (uTableSize -1) corresponds to all other LUT level which are <uZeroIndexLutLevel and > (uZeroIndexLutLevel+uTableSize-1) */
} HAL_MDP_LutInfo;

/* HAL_MDP_QoSLutInfo
 *
 * Structure to keep all QoS (danger, safe, CReq) LUT info
 */
typedef struct _HAL_MDP_QoSLutInfo
{
  uint32          uDangerLutValue;               /* Danger LUT Value */
  uint32          uSafeLutValue;                 /* Safe LUT value   */
  HAL_MDP_LutInfo sCReqLutValue;                 /* CReq LUT value   */
} HAL_MDP_QoSLutInfo;

/* HAL_MDP_QoSConstParam
 *
 * Structure to keep all constant configuration parametrs of QoS
 */
typedef struct _HAL_MDP_QoSConstParam
{
 uint32       uFixedBufferSize;         /* Fixed buffer size */
 uint32       uLineFudgeFactor;         /* Line fudge factor */    
 uint32       uFullLineCountFactor;     /* Full level line count factor */ 
 uint32       uHalfLIneCountFactor;     /* Half level line count factor */
} HAL_MDP_QoSConstParam;

/* HAL_MDP_SSPPQoSInfo
 *
 * Structure to keep all QoS LUT info for each surface type
 */
typedef struct _HAL_MDP_SSPPQoSInfo
{
  HAL_MDP_QoSLutInfo        sLinearQoSInfo;           /* Linear QoS Settings    */
  HAL_MDP_QoSLutInfo        sMacroTileQoSInfo;        /* Macrotile QoS Settings */
  HAL_MDP_QoSConstParam     sConstParams;             /* QoS constant configuration parameters */
} HAL_MDP_SSPPQoSInfo;

/**
 * HAL_MDP_Hw_Private_Info
 *    Holds the private data that is accessible to only HAL MDP driver.
 *
 */
typedef struct _HAL_MDP_Hw_Private_Info
{
   HAL_MDP_HwInfo                       sHwInfo;

   /** QSEED */
   uint32                               uQSEEDNormFilterCoeffCnt;        /**< QSEED Normal Filter Coefficient count */
   uint32                               uQSEEDSmoothFilterCoeffCnt;      /**< QSEED Smooth Filter Coefficient count */
   uint32                              *pQSEEDNormFilterCoeff;
   uint32                              *pQSEEDSmoothFilterCoeff;
   uint32                               uQSEEDSupportedScaleFilters;     /**< QSEED Supported scale filters    */

   uint32                              *pTwoDFilterA; 
   uint32                               uTwoDFilterACoeffCnt;            /**< QSEED 2DFilterA Coefficient count */
   uint32                              *pTwoDFilterB; 
   uint32                               uTwoDFilterBCoeffCnt;            /**< QSEED 2DFilterB Coefficient count */
   uint32                              *pTwoDFilterC; 
   uint32                               uTwoDFilterCCoeffCnt;            /**< QSEED 2DFilterC Coefficient count */
   uint32                              *pTwoDFilterD; 
   uint32                               uTwoDFilterDCoeffCnt;            /**< QSEED 2DFilterD Coefficient count */

   /** Color Space Conversion(CSC) encoding */
   const HAL_MDP_CSCEncodingInfo       *pCSC_ColorYUVToRGBEncodingInfo[HAL_MDP_COLOR_ENCODING_MAX];
   const HAL_MDP_CSCEncodingInfo       *pCSC_ColorRGBToYUVEncodingInfo[HAL_MDP_COLOR_ENCODING_MAX];
      
   /** QoS */
   uint32                               uMaxMdpCoreClkFreq;
   uint32                               uQosReqPriWatermarkRegsCount;
   uint32                               uQosRemapperRealTimeClients;
   uint32                               uQosRemapperNonRealTimeClients;
   uint32                              *pQosWBPathsPriority;
   
   /** VBIF */
   uint32                               uVBIFMaxDdrRdBurstSize;
   uint32                               uVBIFMaxDdrWrBurstSize;
   uint32                               uVBIFMaxRdOT;
   uint32                               uVBIFMaxWrOT;   
   uint32                               uVBIFRoundRobinQosArb;

   /** Panic Safe */
   HAL_MDP_PanicRobustSettingType      *psPanicRobustSetting;
      
   /* VBIF QOS remappers */
   HAL_MDP_VBIFQOSRemappersType        *pVBIFQOSRemappers;   
   const HAL_MDP_VbifId                *pVBIFClientToHWIDMap;

   /** Write-Back Destination pack Format */
   uint32                              *pDstPackPatternInfo;

   /** Clock Control */
   uint32                               uHysterisisValue;
   

   /** Picture adjustment(HSIC) */
   HAL_MDP_PictureAdjustInfo            sPictureAdjustInfo;

   /** Mapping of SMP clients to HW id  */
   uint32                              *pClientToHWClientMap;

   /** IP core specific functions based on MDP version */
   HAL_MDP_IpFxnTbl                     sIpFxnTbl;

   /** Module's status (to detect busy or idle state of the module) register information */
   uint32                              *gpsModuleStatusRegInfo[HAL_MDP_MODULE_MAX];

   HAL_MDP_LayerType                    eCursorIntLayerType;
   uint32                               uCursorBlendOrderIndex;
   /** Default highest bank bit value*/
   uint32                               uDefaultHighestBankBit;
   
   /** Interface's MISR configuration method */
   HAL_MDP_MISRConfigMethod             eMisrCfgMethod;

   /** HW Workaround requirement bits */
   uint32                               uHWWorkaroundEnableBit; 
   /* Chroma Down Module  */
   HAL_MDP_ChromaDownModuleId           eChromaModuleId;
   /** Macrotile format supported, please refer to HAL_MDP_MacroTileType*/
   HAL_MDP_MacroTileType                eMacroTileFormat;
   
   /** Bandwidth compression supported, please refer to HAL_MDP_BandwidthCompressionType*/
   HAL_MDP_BandwidthCompressionType     eBWC;

   /** Multi-rect paramters */
   const HAL_MDP_SourcePipeId          *pMultiRectMappingTable;
   uint32                               uMaxParallelModeWidth;
   uint32                               uMinTimeMultiplexModeLines;

   /** Boolean to indicate danger-safe control is within SSPP */
   bool32                               bSSPPDangerSafe;

   /** SSPP QoS Settings */
   HAL_MDP_SSPPQoSInfo                 *psSSPPQoSInfo;
} HAL_MDP_Hw_Private_Info;


typedef struct _HAL_MDP_ScalarInfo
{
   uint32            uSrcWidthInPixels;
   uint32            uSrcHeightInPixels;
   uint32            uDstWidthInPixels;
   uint32            uDstHeightInPixels;

   uint32            uPhaseStepX;             /**< Phase step X for Color Component 0,3 (Lume and Alpha) plane     */
   uint32            uPhaseStepY;             /**< Phase step Y for Color Component 0,3 (Lume and Alpha) plane     */
   int32             iInitPhaseX;
   int32             iInitPhaseY;
   uint32            eHorzFilterRegFieldVal;
   uint32            eVertFilterRegFieldVal;

} HAL_MDP_ScalarInfo;

typedef struct _HAL_MDP_DecimationInfo
{
   uint32            uHorzDecimationFactor;  /**< Factor to decimate in the Horizontal direction   */
   uint32            uVertDecimationFactor;  /**< Factor to decimate in the Vertical direction       */

} HAL_MDP_DecimationInfo;

/*!
 * \struct HAL_MDP_AD_DualCore_FrameParams
 *
 * Frame information for the AD Core; It is used in the dual-pipe scenarios to configure the portion of the frame
 * that will be configured by the Core
 *
 */
typedef struct
{
  uint32   uDualPipeFrameStart;          /**< Start of the frame processed by the AD Core, used for statistics application on Dual pipe configuration */
  uint32   uDualPipeFrameEnd;            /**< End of the frame processed by the AD Core, used for statistics application on Dual pipe configuration  */
  uint32   uDualPipeProcsStart;          /**< Start of the frame processed by the AD Core, used for statistics collection on Dual pipe configuration   */
  uint32   uDualPipeProcsEnd;            /**< End of the frame processed by the AD Core, used for statistics collection on Dual pipe configuration    */
} HAL_MDP_AD_DualCore_FrameParams;

/*------------------------------------------------------------------------
 * Global Data Definitions
 *-----------------------------------------------------------------------*/
extern uintPtr  uMDPControlPathRegBaseOffset[HAL_MDP_CONTROL_PATH_MAX];                  // Control path register offsets
extern uintPtr  uMDPDataPathRegBaseOffset[HAL_MDP_DATA_PATH_MAX];                        // Data path register offsets
extern uintPtr  uMDPDSPPRegBaseOffset[HAL_MDP_DESTINATION_PIPE_MAX];                     // Destination (DSPP) register offsets 
extern uintPtr  uMDPAADCRegBaseOffset[HAL_MDP_DESTINATION_PIPE_MAX];                     // AD registers offsets
extern uintPtr  uMDPPhyInterfaceRegBaseOffset[HAL_MDP_PHYSICAL_INTERFACE_MAX];           // Physical interface register offsets
extern uintPtr  uMDPLayerMixerRegBaseOffset[HAL_MDP_LAYER_MIXER_MAX];                    // Layer mixer register offsets
extern uintPtr  uMDPLayerMixerBlendStateRegBaseOffset[HAL_MDP_BLEND_STAGE_MAX];          // Layer mixer blending state register offsets
extern uintPtr  uMDPPingPongRegBaseOffset[HAL_MDP_PINGPONG_MAX];                         // PingPong register offsets
extern uintPtr  uMDPPingPongBufferRegBaseOffset[HAL_MDP_PINGPONG_MAX];                   // PingPong Buffer (PPB) register offsets
extern uintPtr  uMDPSSPPRegBaseOffset[HAL_MDP_SOURCE_PIPE_MAX];                          // Source (SSPP) register offsets
extern uintPtr  uMDPLayerRegBaseOffset[HAL_MDP_SOURCE_PIPE_MAX];                         // Source (SSPP) Layers (Scalar) register offsets
extern uintPtr  uMDPSSPPPixExtRegBaseOffset[HAL_MDP_SOURCE_PIPE_MAX_COLOR_COMPONENTS];   // SSPP Pixel Ext Component Offset
extern uintPtr  uMDPInterfaceVideoMISRCTRLRegBaseOffset[HAL_MDP_PHYSICAL_INTERFACE_MAX]; // Physical interface video misr ctrl register offset
extern uintPtr  uMDPInterfaceVideoMISRSignatureOffset[HAL_MDP_PHYSICAL_INTERFACE_MAX];   // Physical interface video misr signature register offset
extern uintPtr  uMDPInterfaceCMDMISRCTRLRegBaseOffset[HAL_MDP_PHYSICAL_INTERFACE_MAX];   // Physical interface CMD misr ctrl register offset
extern uintPtr  uMDPInterfaceCMDMISRSignatureOffset[HAL_MDP_PHYSICAL_INTERFACE_MAX];     // Physical interface CMD misr signature register offset
extern uintPtr  uMDPIntfMisrStreamRegBaseOffset[HAL_MDP_INTERFACE_STREAM_MAX];           // Interface stream-based MISR configuration register offsets
extern uintPtr  uMDPSplitMUXRegBaseOffset[HAL_MDP_SPLITMUX_MAX];                         // PingPong Buffer (PPB) register offsets
extern uintPtr  uMDPDSCRegBaseOffset[HAL_MDP_DSC_MAX];                                   // DSC register offset
extern uintPtr  uMDPDestScalerRegBaseOffset[HAL_MDP_DESTINATION_PIPE_MAX];               // Destination scaler register offsets 
extern uintPtr  uMDPVbifRegBaseOffset[HAL_MDP_VBIF_MAX];                                 // VBIF register offsets


extern HAL_MDP_Hw_Private_Info   gsMdpHwInfo;                              // No direct access to this global variable
extern HAL_MDP_Hw_Private_Info   *pgsMdpHwInfo;                            // HAL driver should use this to extract HAL_MDP_HwInfo
extern uint32                    guSrcFormatInfo[];
extern uint32                    guSrcUnpackInfo[];
extern uint32                    guSrcDecompressUnpackInfo[];
extern uint32                    guDstPackInfo[];
extern uint32                    guPanelFormatInfo[];


/* Limited REC601 YUV to RGB Conversion   */
extern const HAL_MDP_CSCEncodingInfo   gMDP_CSC_YuvToRgb_Limited_Rec601;
/* Limited REC601 RGB to YUV Conversion   */
extern const HAL_MDP_CSCEncodingInfo   gMDP_CSC_RgbToYuv_Limited_Rec601;
/* Full REC601 YUV to RGB Conversion      */
extern const HAL_MDP_CSCEncodingInfo   gMDP_CSC_YuvToRgb_Full_Rec601;
/*  Full REC601 RGB to YUV Conversion     */
extern const HAL_MDP_CSCEncodingInfo   gMDP_CSC_RgbToYuv_Full_Rec601;
/*  Limited REC709 YUV to RGB Conversion  */
extern const HAL_MDP_CSCEncodingInfo   gMDP_CSC_YuvToRgb_Limited_Rec709;
/*  Limited REC709 RGB to YUV Conversion  */
extern const HAL_MDP_CSCEncodingInfo   gMDP_CSC_RgbToYuv_Limited_Rec709;
/*  Full REC709 YUV to RGB Conversion     */
extern const HAL_MDP_CSCEncodingInfo   gMDP_CSC_YuvToRgb_Full_Rec709;
/*  Full REC709 RGB to YUV Conversion     */
extern const HAL_MDP_CSCEncodingInfo   gMDP_CSC_RgbToYuv_Full_Rec709;
/* Multirect mapping table */
extern const HAL_MDP_SourcePipeId gMultiRectMappingTable[HAL_MDP_SOURCE_PIPE_MAX];


extern uint32                    gMDP_Qos_WB_Paths_Priority[];

/* Access macro for guSrcUnpackInfo */
#define HAL_PIXELFORMAT_TO_SOURCE_PACK_MAP(_pixel_fmt_)                        (guSrcUnpackInfo[((_pixel_fmt_)<HAL_MDP_PIXEL_FORMAT_MAX)?(_pixel_fmt_):HAL_MDP_PIXEL_FORMAT_NONE])

// Helper defines
#define HAL_MDP_GET_RGB_LAYER_COUNT()                       (pgsMdpHwInfo->sHwInfo.uNumOfRGBLayers)
#define HAL_MDP_GET_VIG_LAYER_COUNT()                       (pgsMdpHwInfo->sHwInfo.uNumOfVIGLayers)
#define HAL_MDP_GET_DMA_LAYER_COUNT()                       (pgsMdpHwInfo->sHwInfo.uNumOfDMALayers)
#define HAL_MDP_GET_CURSOR_LAYER_COUNT()                    (pgsMdpHwInfo->sHwInfo.uNumOfCursorLayers)
#define HAL_MDP_GET_LM_COUNT()                              (pgsMdpHwInfo->sHwInfo.uNumOfLayerMixers)
#define HAL_MDP_GET_DSPP_COUNT()                            (pgsMdpHwInfo->sHwInfo.uNumOfDSPPs)
#define HAL_MDP_GET_CONTROL_PATHS_COUNT()                   (pgsMdpHwInfo->sHwInfo.uNumOfControlPaths)
#define HAL_MDP_GET_DATA_PATHS_COUNT()                      (pgsMdpHwInfo->sHwInfo.uNumOfDataPaths)
#define HAL_MDP_GET_PHYSICAL_INTERFACE_COUNT()              (pgsMdpHwInfo->sHwInfo.uNumOfPhyInterfaces)
#define HAL_MDP_GET_LM_BLEND_STAGES()                       (pgsMdpHwInfo->sHwInfo.uNumOfLayerMixerBlendStages)
#define HAL_MDP_GET_PINGPONG_COUNT()                        (pgsMdpHwInfo->sHwInfo.uNumOfPingPongs)
#define HAL_MDP_GET_WATCHDOG_COUNT()                        (pgsMdpHwInfo->sHwInfo.uNumOfWatchDogs)
#define HAL_MDP_GET_AXIPORT_COUNT()                         (pgsMdpHwInfo->sHwInfo.uNumOfAxiPorts)
#define HAL_MDP_GET_VBIFCLIENT_COUNT()                      (pgsMdpHwInfo->sHwInfo.uNumOfVbifClients)
#define HAL_MDP_GET_CHROMADOWN_BLOCK_COUNT()                (pgsMdpHwInfo->sHwInfo.uNumOfChromaDownBlocks)
#define HAL_MDP_GET_VBIF_COUNT()                            (pgsMdpHwInfo->sHwInfo.uNumOfAxiPorts)

/* -----------------------------------------------------------------------
 * Externalized Function Definitions
 * ----------------------------------------------------------------------- */

/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_Init()
*/
/*!
* \brief
*     Initializes the MDP SSPP block
*
* \param [in] eSourcePipe           - Source Pipe ID
* \param [in] eLayerType            - Layer Type (RGB, VIG, DMA, or CURSOR etc)
* \param [in] uInitFlags            - Internal modules to be initialized
* \param [in] uFlags                - Reserved
*
* \retval NONE
*
****************************************************************************/
void HAL_MDP_SSPP_Init(HAL_MDP_SourcePipeId    eSourcePipe, 
                       HAL_MDP_LayerType       eLayerType,
                       uint32                  uInitFlags,
                       uint32                  uFlags);

/****************************************************************************
*
** FUNCTION: HAL_MDP_DSPP_Init()
*/
/*!
* \brief
*     Initializes the MDP DSPP block
*
* \param [in] eDestPipeId           - Destination Pipe ID
* \param [in] uInitFlags            - Internal modules to be initialized
* \param [in] uFlags                - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_DSPP_Init(HAL_MDP_DestinationPipeId    eDestPipeId,
                                     uint32                       uInitFlags,
                                     uint32                       uFlags);

/****************************************************************************
*
** FUNCTION: HAL_MDP_LayerMixer_Init()
*/
/*!
* \brief
*     Initializes the layer mixer module
*
* \param [in] eLayerMixerId         - Layer Mixer ID (0, 1, 2, 3, and 4)
* \param [in] uInitFlags            - Internal blocks to be initialized
* \param [in] uFlags                - Reserved flags
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_LayerMixer_Init(HAL_MDP_LayerMixerId    eLayerMixerId, 
                                           uint32                  uInitFlags,
                                           uint32                  uFlags);

/****************************************************************************
*
** FUNCTION: HAL_MDP_DataPath_Init()
*/
/*!
* \brief
*     Initializes the Data Path block
*
* \param [in] eDataPathId           - Data Path ID
* \param [in] uInitFlags            - Internal modules to be initialized
* \param [in] uFlags                - Reserved
*
* \retval NONE
*
****************************************************************************/
void HAL_MDP_DataPath_Init(HAL_MDP_DataPathId      eDataPathId,
                           uint32                  uInitFlags,
                           uint32                  uFlags);

/****************************************************************************
*
** FUNCTION: HAL_MDP_GetSourceFormatInfo()
*/
/*!
* \brief
*     Returns the source format register programming information
*
* \param [in] psSurface           - Surface Information
*
* \retval uint32
*
****************************************************************************/
uint32 HAL_MDP_GetSourceFormatInfo(HAL_MDP_SurfaceAttrType   *psSurface);

/****************************************************************************
*
** FUNCTION: HAL_MDP_GetDestinationFormatInfo()
*/
/*!
* \brief
*     Returns the destination format register programming information
*
* \param [in] psSurface           - Surface Information
*
* \retval uint32
*
****************************************************************************/
uint32 HAL_MDP_GetDestinationFormatInfo(HAL_MDP_SurfaceAttrType   *psSurface);


/****************************************************************************
*
** FUNCTION: HAL_MDP_ReadHardwareInfo_1_0_0()
*/
/*!
* \brief
*     Reads the hardware capabilities for the given MDP Version(1.0.0)
*
* \param [out]  psHwInfo            - Hardware information
* \param [out]  psMdpHwInfo         - MDP hardware information 
*
* \retval NONE
*
****************************************************************************/
void HAL_MDP_ReadHardwareInfo_4_0_0(HAL_MDP_HwInfo             *psHwInfo,
                                    HAL_MDP_Hw_Private_Info    *psMdpHwInfo);

/****************************************************************************
*
** FUNCTION: HAL_MDP_TrafficCtrl_SetClientWatermarkLvl_4_0_0()
*/
/*!
* \brief
*     Sets a read client's watermark levels based on client's latency need.
*
* \param [in] eLatency             - latency setting for the client; 
* \param [in] uTotalSMPBytes       - total number of bytes in SMP allocated for the client;
* \param [in] uFlags               - If configured surface is MacroTiled;
* \param [in] uRegAddr             - address of watermark level-0 register;
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_TrafficCtrl_SetClientWatermarkLvl_4_0_0 (
                              HAL_MDP_TrafficCtrl_LatencyType  eLatency,
                              uint32                           uTotalSMPBytes,
                              uint32                           uFlags,
                              uintPtr                          uRegAddr);

/****************************************************************************
*
** FUNCTION: HAL_MDP_DSPP_HistogramV2_Config()
*/
/*!
* \brief
*     Set up configurations for DSPP Histogram(V2) generation
*
* \param [in]   uRegisterOffset   - Register offset
* \param [in]   psHistConfig      - Histogram Configuration information
*
* \retval NONE
*
****************************************************************************/
void HAL_MDP_DSPP_HistogramV2_Config(uintPtr                       uRegisterOffset,
                                     HAL_MDP_HistogramConfigType   *psHistConfig);

/****************************************************************************
*
** FUNCTION: HAL_MDP_DSPP_HistogramV2_Lock()
*/
/*!
* \brief
*     Lock histogram table for SW to read
*
* \param [in]   uint32     - Register offset
* \param [out]  bLock      - lock/unlock hist table
*
* \retval NONE
*
****************************************************************************/
void HAL_MDP_DSPP_HistogramV2_Lock(uintPtr  uRegisterOffset,
                                   bool32   bLock);
/****************************************************************************
*
** FUNCTION: HAL_MDP_DSPP_AssertiveDisplayFillLUT()
*/
/*!
* \brief
*     Configures the LUTs for AD Core. 
*     This function is based on the Registers Layout of the Adaptive Display LUTs
*     where two elements of the LUT are programmed in the same register, being the even elements
*     of the table programmed in the less significant bits and the odd elements programmed in the
*     most significant bits. Also, the last element on each table is an special case, since it's one element
*     in the most significant bits of one register.
*
* \param [in]   pLutParams               - Configuration to fill the LUT
*
* \retval NONE
*
****************************************************************************/
void HAL_MDP_DSPP_AssertiveDisplayFillLUT(HAL_MDP_DSPP_FillLUTParams *pLutParams);

/****************************************************************************
*
** FUNCTION: HAL_MDP_DSPP_AssertiveDisplayTrigger()
*/
/*!
* \brief
*     
*
* \param [in]   eDestPipe                   - Destination pipe id
* \param [in]   psAssertiveDisplayTrigger   - Assertive Display trigger info
*
* \retval NONE
*
****************************************************************************/
void HAL_MDP_DSPP_AssertiveDisplayTrigger(HAL_MDP_DestinationPipeId         eDestPipe,
                                          HAL_MDP_ADTriggerType  *psAssertiveDisplayTrigger);

/****************************************************************************
*
** FUNCTION: HAL_MDP_DSPP_AssertiveDisplayCalibration()
*/
/*!
* \brief
*     
*
* \param [in]   eDestPipe                               - Destination pipe id
* \param [in]   psAssertiveDisplayCalibration   - Assertive Display calibration info
*
* \retval NONE
*
****************************************************************************/

void HAL_MDP_DSPP_AssertiveDisplayCalibration(HAL_MDP_DestinationPipeId         eDestPipe,
                                              HAL_MDP_ADCalibrationType     *psAssertiveDisplayCalibration);

/****************************************************************************
*
** FUNCTION: HAL_MDP_DSPP_AssertiveDisplayConfig()
*/
/*!
* \brief
*     
*
* \param [in]   eDestPipe                   - Destination pipe id
* \param [in]   psAssertiveDisplayConfig    - Assertive Display configuration info
*
* \retval NONE
*
****************************************************************************/
void HAL_MDP_DSPP_AssertiveDisplayConfig(HAL_MDP_DestinationPipeId         eDestPipe,
                                         HAL_MDP_ADConfigType   *psADConfig);


/****************************************************************************
*
** FUNCTION: HAL_MDP_TrafficCtrl_SetClientWatermarkLvl_4_0_0()
*/
/*!
* \brief
*     Sets a read client's watermark levels based on client's latency need.
*
* \param [in] eLatency             - latency setting for the client; 
* \param [in] uTotalSMPBytes       - total number of bytes in SMP allocated for the client;
* \param [in] uFlags               - If configured surface is MacroTiled;
* \param [in] uRegAddr             - address of watermark level-0 register;
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_TrafficCtrl_SetClientWatermarkLvl_4_0_0 (
                              HAL_MDP_TrafficCtrl_LatencyType  eLatency,
                              uint32                           uTotalSMPBytes,
                              uint32                           uFlags,
                              uintPtr                          uRegAddr);

/****************************************************************************
*
** FUNCTION: HAL_MDP_Get_GpuMemoryBankBitConfig()
*/
/*!
* \brief
*     This function will return the valid Gpu memory bank bit configuration
*     It will validate the OS provided value and if it is invalid, will set to default
*
* \param [in]   psGpuMemoryBankConfig     - Gpu highest memory bank bit config
* \param [in]   uFlags                    - Reserved.
*
* \retval uint32
*
****************************************************************************/

uint32 HAL_MDP_Get_GpuMemoryBankBitConfig(HAL_MDP_GpuMemoryBankConfigType *psGpuMemoryBankConfig,
                                          uint32                          uFlags);

/****************************************************************************
*
** FUNCTION: HAL_MDP_Get_CDM_BaseOffset()
*/
/*!
* \brief
*     Returns the offset of Chroma down block register offset 
*
* \param [in]  eChromaDownModuleId    - Chroma down Module Id
*
* \retval uintPtr 
*
****************************************************************************/
uintPtr HAL_MDP_Get_CDM_BaseOffset(HAL_MDP_ChromaDownModuleId          eChromaDownModuleId);

/****************************************************************************
*
** FUNCTION: HAL_MDP_CDM_Init()
*/
/*!
* \brief
*     Initializes the Chroma down block
*
* \param [in] eChromaDownModuleId     - Chroma down Module Id
* \param [in] uInitFlags              - Internal modules to be initialized
* \param [in] uFlags                  - Reserved
*
* \retval NONE
*
****************************************************************************/
void HAL_MDP_CDM_Init(HAL_MDP_ChromaDownModuleId   eChromaDownModuleId,
                      uint32                       uInitFlags,
                      uint32                       uFlags);
/****************************************************************************
*
** FUNCTION: HAL_MDP_CDM_Output_Format_Config()
*/
/*!
* \brief
*     programs the Chroma down module CSC configuration and chroma down-sampler 
*     based on the output pixel format
*
*
* \param [in]   eChromaDownModuleId   - Chroma down Module Id
* \param [in]   eInterfaceId          - Data path configuration data
* \param [in]   ePixelFormat          - HAL Pixel format.
* \param [in]   uFlags                - Reserved.
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_CDM_Output_Format_Config(HAL_MDP_ChromaDownModuleId     eChromaDownModuleId,
                                                    HAL_MDP_InterfaceId            eInterfaceId,
                                                    HAL_MDP_PixelFormatType        ePixelFormat,
                                                    uint32                         uFlags);

/****************************************************************************
*
** FUNCTION: HAL_MDP_DataPath_OutRect_Config()
*/
/*!
* \brief
*     Configures output rectangle for chroma down module
*
* \param [in]   eChromaDownModuleId  - Chroma down module 
* \param [in]   psRectConfig         - Output rectangle information
* \param [in]   uFlags               - Reserved.
*
* \retval NONE
*
****************************************************************************/
void HAL_MDP_CDM_OutRect_Config( HAL_MDP_ChromaDownModuleId  eChromaDownModuleId,
                                 HAL_MDP_RectType            *psRectConfig,
                                 uint32                      uFlags);


/****************************************************************************
*
** FUNCTION: HAL_MDP_Get_Layer_BaseOffset()
*/
/*!
* \brief
*     Returns the offset of SSPP register block from first layer's SSPP address
*
* \param [in]   eSourcePipe         - Source pipe id
*
* \retval uint32
*
****************************************************************************/
uintPtr HAL_MDP_Get_Layer_BaseOffset(HAL_MDP_SourcePipeId  eSourcePipe);


/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_ScalerLUTConfig()
*/
/*!
* \brief
*     Configures the SSPP scaler LUT
*
* \param [in] eSourcePipe           - Source Pipe ID
* \param [in] psScalarConfig        - QSEED LUT configuration
*
* \retval NONE
*
****************************************************************************/
void HAL_MDP_SSPP_ScalerLUTConfig(HAL_MDP_SourcePipeId      eSourcePipe, 
                                  HAL_MDP_ScalarConfigType *psScalarConfig);

/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_ScalerDEConfig()
*/
/*!
* \brief
*     Program  scaler DE registers
*     populated from HAL_MDP_UTILITY_CalcPipeToPixelExtConfig()
*
* \param [in]   eSourcePipe             - Source pipe id
* \param [in]   eLayerType              - Layer Type, Only RGB, VG and DMA are supported
* \param [in]   psScalarConfig          - Values populated from 
*                                         HAL_MDP_UTILITY_CalcPipeToPixelExtConfig()
*
* \retval void
*
****************************************************************************/
void HAL_MDP_SSPP_ScalerDEConfig(HAL_MDP_SourcePipeId             eSourcePipe,
                                 HAL_MDP_LayerType                eLayerType,
                                 HAL_MDP_ScalarConfigType        *psScalarConfig);

/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_ScalerConfig()
*/
/*!
* \brief
*     Program  scaler registers
*     populated from HAL_MDP_UTILITY_CalcPipeToPixelExtConfig()
*
* \param [in]   eSourcePipe             - Source pipe id
* \param [in]   eLayerType              - Layer Type, Only RGB, VG and DMA are supported
* \param [in]   psScalarConfig          - Values populated from 
*                                         HAL_MDP_UTILITY_CalcPipeToPixelExtConfig()
*
* \retval void
*
****************************************************************************/
void HAL_MDP_SSPP_ScalerConfig(HAL_MDP_SourcePipeId             eSourcePipe,
                               HAL_MDP_LayerType                eLayerType,
                               HAL_MDP_ScalarConfigType        *psScalarConfig);


/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_ScalarLUTConfig()
*/
/*!
* \brief
*     Configures the DSPP scaler LUT
*
* \param [in] eDestPipe             - Destination Pipe ID
* \param [in] psScalarConfig        - QSEED LUT configuration
*
* \retval NONE
*
****************************************************************************/
void HAL_MDP_DSPP_ScalerLUTConfig(HAL_MDP_DestinationPipeId   eDestPipe, 
                                  HAL_MDP_ScalarConfigType   *psScalarConfig);

/****************************************************************************
*
** FUNCTION: HAL_MDP_DSPP_ScalerDEConfig()
*/
/*!
* \brief
*     Program  destination scalar qseed DE registers
*     populated from HAL_MDP_UTILITY_CalcDisplayConfig()
*
* \param [in]   eDestPipe               - Destination pipe id
* \param [in]   psScalarConfig          - Values populated from 
*                                         HAL_MDP_UTILITY_CalcDisplayConfig()
*
* \retval void
*
****************************************************************************/
void HAL_MDP_DSPP_ScalerDEConfig(HAL_MDP_DestinationPipeId        eDestPipe,
                                 HAL_MDP_ScalarConfigType        *psScalarConfig);

/****************************************************************************
*
** FUNCTION: HAL_MDP_DSPP_ScalerConfig()
*/
/*!
* \brief
*     Program  qseed registers
*     populated from HAL_MDP_UTILITY_CalcPipeToPixelExtConfig()
*
* \param [in]   eDestPipe               - Destination pipe id
* \param [in]   psScalarConfig          - Values populated from 
*                                         HAL_MDP_UTILITY_CalcDisplayConfig()
*
* \retval void
*
****************************************************************************/
void HAL_MDP_DSPP_ScalerConfig(HAL_MDP_DestinationPipeId        eDestPipe,
                               HAL_MDP_ScalarConfigType        *psScalarConfig);


/****************************************************************************
*
** FUNCTION: HAL_MDP_SwapMacroTileColorComponent_4_0_0()
*/
/*!
* \brief
*     This function will return a new uint32 value where the color element 0 and 2 are swapped for some RGB format.
*
* \param [in]   ePixelFormat     - Pixel format 
* \param [in]   uPixelValue      - Current pixel value
*
* \retval uint32
*
****************************************************************************/
uint32 HAL_MDP_SwapMacroTileColorComponent_4_0_0(HAL_MDP_PixelFormatType ePixelFormat, uint32 uPatternValue);

/****************************************************************************
*
** FUNCTION: HAL_MDP_SSPP_SetQosConfig_3_0_0()
*/
/*!
* \brief
*  Source pipe QoS configuration
*
* \param [in] eSourcePipe       - eSourcePipe
* \param [in] eLatency          - Client latency
* \param [in] psQoSInfo         - SSPP QoS Info
* \param [in] uFlags            - Surface flags
*
* \retval none
*
****************************************************************************/
void HAL_MDP_SSPP_SetQosConfig_4_0_0 (HAL_MDP_SourcePipeId              eSourcePipe,
                                      HAL_MDP_TrafficCtrl_LatencyType   eLatency,
                                      HAL_MDP_Qos_ConfigType           *psQoSInfo,
                                      uint32                            uFlags);

/****************************************************************************
*
** FUNCTION: HAL_MDP_ROT_SSPP_QosConfig()
*/
/*!
* \brief
*  Rotator Source pipe QoS configuration
*
* \param [in] eLatency          - Client latency
* \param [in] psQoSInfo         - SSPP QoS Info
* \param [in] uFlags            - Surface flags
*
* \retval none
*
****************************************************************************/
 void HAL_MDP_ROT_SSPP_QosConfig (HAL_MDP_TrafficCtrl_LatencyType   eLatency,
                                  HAL_MDP_Qos_ConfigType           *psQoSInfo,
                                  uint32                            uFlags);


#endif // _HAL_MDP_I_H
