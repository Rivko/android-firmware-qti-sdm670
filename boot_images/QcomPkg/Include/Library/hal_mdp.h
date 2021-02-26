/*! \file */

/*
===========================================================================

FILE:         hal_mdp.h

===========================================================================
  Copyright (c) 2012-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
===========================================================================
*/
#ifndef _HAL_MDP_H_
#define _HAL_MDP_H_

/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */
#include "hal_mdss.h"
/* !! ALERT !!
 * DO NOT INCLUDE ANY HEADER FILES OTHER THAN hal_mdss.h
 */

#ifdef __cplusplus
extern "C" 
{
#endif

#define HAL_MDP_MAX_REGISTER_SIZE                                        1024
#define HAL_MDP_HISTOGRAM_LUT_ENTRIES                                    256
#define HAL_MDP_AR_GAMMA_LUT_ENTRIES                                     16
#define HAL_MDP_PCC_CO_EFFICIENT_ENTRIES                                 12
#define HAL_MDP_PCC_NUM_OF_PCC_COLOR_COMPONENTS                          3
#define HAL_MDP_GAMMA_LUT_ENTRIES                                        256
#define HAL_MDP_MAX_MULTIPIPES                                           3

#define HAL_MDP_ASSERTIVE_DISPLAY_CALIB_LEN                              33
#define HAL_MDPP_SIX_ZONE_LUT_ENTRIES                                    384

/* Gamut mapping - component and table information */
#define HAL_MDP_GM_MAX_COMPONENTS                                        3      /* MAX GM components  */    
#define HAL_MDP_GM_MAX_COMPONENT_TABLES                                  8      /* MAX GM tables per component */    
#define HAL_MDP_GM_TABLE_0_ENTRIES                                       125    /* Table-0 GM entries */
#define HAL_MDP_GM_TABLE_1_ENTRIES                                       100    /* Table-1 GM entries */
#define HAL_MDP_GM_TABLE_2_ENTRIES                                       80     /* Table-2 GM entries */
#define HAL_MDP_GM_TABLE_3_ENTRIES                                       100    /* Table-3 GM entries */
#define HAL_MDP_GM_TABLE_4_ENTRIES                                       100    /* Table-4 GM entries */
#define HAL_MDP_GM_TABLE_5_ENTRIES                                       80     /* Table-5 GM entries */
#define HAL_MDP_GM_TABLE_6_ENTRIES                                       64     /* Table-6 GM entries */
#define HAL_MDP_GM_TABLE_7_ENTRIES                                       80     /* Table-7 GM entries */

#define HAL_MDP_WGM_MAX_COLOR_COMPONENTS                                 3      /* MAX Wide GM color components. Color-0, Color-1, Color-2. */
#define HAL_MDP_WGM_MAX_COMPONENT_TABLES                                 16     /* MAX Wide GM tables per component */
#define HAL_MDP_WGM_MAX_COMPONENT_TABLES_FINE_COARSE                     4      /* Total no.of.tables for each color component to hold 17x17x17 or 5x5x5 table entried */
#define HAL_MDP_WGM_MAX_NON_UNIFORM_MAP_TABLE_ENTRIES                    16     /* Non-Uniform LUT table entries for each color component */

#define HAL_MDP_WGM_TABLE_0_ENTRIES                                      1229
#define HAL_MDP_WGM_TABLE_1_ENTRIES                                      1228
#define HAL_MDP_WGM_TABLE_2_ENTRIES                                      1228
#define HAL_MDP_WGM_TABLE_3_ENTRIES                                      1228   /** Total entries are 4931 (17x17x17 matrix) */
#define HAL_MDP_WGM_COARSE_MODE_TABLE_0_ENTRIES                          32
#define HAL_MDP_WGM_COARSE_MODE_TABLE_1_ENTRIES                          31
#define HAL_MDP_WGM_COARSE_MODE_TABLE_2_ENTRIES                          31
#define HAL_MDP_WGM_COARSE_MODE_TABLE_3_ENTRIES                          31     /** Total entries are 125 (5x5x5 matrix) */

/* 
 * Sharpening strength limit 
*/
#define HAL_MDP_SHARPENING_SHARPENING_STRENGTH_MIN                       1
#define HAL_MDP_SHARPENING_SHARPENING_STRENGTH_MAX                       255
#define HAL_MDP_SHARPENING_SMOOTHING_STRENGTH_MIN                        -1
#define HAL_MDP_SHARPENING_SMOOTHING_STRENGTH_MAX                        -64

/* 
 * Sharpening threshold params limit 
*/
#define HAL_MDP_SHARPENING_SHARP_EDGE_THRESH_MAX                         1024
#define HAL_MDP_SHARPENING_SMOOTH_THRESH_MAX                             1024
#define HAL_MDP_SHARPENING_NOISE_THRESH_MAX                              256


// Default values no sharpening/smoothening 
#define HAL_MDP_DEFAULT_SHARPENING_LEVEL                                 32
#define HAL_MDP_DEFAULT_SHARPENING_SHARP_EDGE_THRESH                     112
#define HAL_MDP_DEFAULT_SHARPENING_SMOOTH_THRESH                         8
#define HAL_MDP_DEFAULT_SHARPENING_NOISE_THRESH                          2

// Content-adaptive sharpening default values  
#define HAL_MDP_DEFAULT_CONTENT_ADAPTIVE_SHARPENING_SHARP_EDGE_THRESH    8
#define HAL_MDP_DEFAULT_CONTENT_ADAPTIVE_SHARPENING_SMOOTH_THRESH        8
#define HAL_MDP_DEFAULT_CONTENT_ADAPTIVE_SHARPENING_NOISE_THRESH         2

// Smoothening default values 
#define HAL_MDP_DEFAULT_SMOOTHENING_SHARP_EDGE_THRESH                    0
#define HAL_MDP_DEFAULT_SMOOTHENING_SMOOTH_THRESH                        0
#define HAL_MDP_DEFAULT_SMOOTHENING_NOISE_THRESH                         0

//Number of Bits used for Chroma site resolution
#define HAL_MDP_NBITS_CHROMA_SITE_RESOLUTION                             7

#define HAL_MDP_MAX_SSPP_IN_DUALPIPE_MODE                                2   /* Maximum supported pipe number in dual pipe case */
#define HAL_MDP_SOURCE_PIPE_MAX_COLOR_COMPONENTS                         4   /* Maximum number of component a pipe supports */

//YCBCR_H2V2 Chroma site default positions
#define HAL_MDP_YCBCR_H2V2_DEFAULT_CHROMA_SITE_POSITION_X                1<<(HAL_MDP_NBITS_CHROMA_SITE_RESOLUTION-1)
#define HAL_MDP_YCBCR_H2V2_DEFAULT_CHROMA_SITE_POSITION_Y                1<<(HAL_MDP_NBITS_CHROMA_SITE_RESOLUTION-1)

//YCBCR_H1V2 Chroma site default positions
#define HAL_MDP_YCBCR_H1V2_DEFAULT_CHROMA_SITE_POSITION_X                0
#define HAL_MDP_YCBCR_H1V2_DEFAULT_CHROMA_SITE_POSITION_Y                1<<(HAL_MDP_NBITS_CHROMA_SITE_RESOLUTION-1)

//YCBCR_H2V1 Chroma site default positions
#define HAL_MDP_YCBCR_H2V1_DEFAULT_CHROMA_SITE_POSITION_X                1<<(HAL_MDP_NBITS_CHROMA_SITE_RESOLUTION-1)
#define HAL_MDP_YCBCR_H2V1_DEFAULT_CHROMA_SITE_POSITION_Y                0

//AYCBCR_H1V1 Chroma site default positions
#define HAL_MDP_AYCBCR_H1V1_DEFAULT_CHROMA_SITE_POSITION_X               0
#define HAL_MDP_AYCBCR_H1V1_DEFAULT_CHROMA_SITE_POSITION_Y               0
#define HAL_MDP_MAX_SSPP_IN_DUALPIPE_MODE                                2   /* Maximum supported pipe number in dual pipe case */
#define HAL_MDP_SOURCE_PIPE_MAX_COMPONENTS                               4   /* Maximum number of component a pipe supports */

//QSEED Scalar information
#define HAL_MDP_QSEED_COEFF_LUT_A_ENTRIES                                42
#define HAL_MDP_QSEED_COEFF_LUT_B_ENTRIES                                24
#define HAL_MDP_QSEED_COEFF_LUT_C_ENTRIES                                24
#define HAL_MDP_QSEED_COEFF_LUT_D_ENTRIES                                20
#define HAL_MDP_QSEED_ENTRIE_SIZE_IN_REGISTERS                           4

#define HAL_MDP_QSEED_DE_CURVE_ENTRIES                                   3
#define HAL_MDP_QSEED_LUT_BANKS                                          2
#define HAL_MDP_QSEED_MAX_LUT_FILTERS                                    5  /* Number of Filter LUTs in QSEED */
#define HAL_MDP_QSEED_MAX_LUT_REGION                                     4  /* Number of LUT regions in QSEED */


/* Helper macros for QSEED Scale filter caps */
#define HAL_MDP_QSEED_FILTER_CAP(_FILTER_)                              (1<<(_FILTER_+1)) /*Need to add 1 since Scale filter HAL definitions start with 0*/

#define HAL_MDP_IS_QSEED_FILTER_SUPPORTED(_a_)                          (HAL_MDP_QSEED_FILTER_CAP(_a_) &  gsMdpHwInfo.uQSEEDSupportedScaleFilters)


//Rotator RegDma information
#define HAL_ROTATOR_REGDMA_MAX_REQUESTS_IN_QUEUE                        32
#define HAL_ROTATOR_REGDMA_MAX_INT_IN_QUEUE                             3
#define HAL_ROTATOR_REGDMA_MAX_PRIORITY_QUEUES                          2
#define HAL_ROTATOR_REGDMA_MAX_COMMANDS_IN_REQUEST                      128
#define HAL_ROTATOR_REGDMA_MAX_SECURE_REGIONS                           3

/* HWIO_MMSS_ROT_REGDMA_RAM_REGDMA_CMD_RAM_i_MAXi specifies max index in CmdBuffer,
  +1 to consider index[0] in CmdBuffer*/
#define HAL_ROTATOR_REGDMA_CMD_BUFFER_SIZE                              (HWIO_MMSS_ROT_REGDMA_RAM_REGDMA_CMD_RAM_i_MAXi + 1)


/* -----------------------------------------------------------------------
** Types
** --------------------------------------------------------------------*/

/*
 * API Flags
 */

/* HAL_MDP_Init */
#define HAL_MDP_INIT_FLAGS_CLEAR_INTERRUPTS                    0x00000001        /**< Clears all MDP interrupts         */
#define HAL_MDP_INIT_FLAGS_DISABLE_INTERRUPTS                  0x00000002        /**< Disables all MDP interrupts       */
#define HAL_MDP_INIT_FLAGS_VBIF_CONFIGURATION                  0x00000004        /**< VBIF configuration                */
#define HAL_MDP_INIT_FLAGS_TRFCTRL_CONFIGURATION               0x00000008        /**< Traffic Control configuration     */
#define HAL_MDP_INIT_FLAGS_QDSS_CONFIGURATION                  0x00000010        /**< QDSS hardware logging             */

/* HAL_MDP_Vbif_Init */
#define HAL_MDP_VBIF_ENABLE_CLOCKGATING                        0x00000001        /**< Enable clock gating in the VBIF core */

#define HAL_MDP_FLAGS_USE_SW_INTR_STATUS_REGISTER_INFO         0x00000001

/* SSPP surface configuration flags */
#define HAL_MDP_FLAGS_SSPP_SURFACE_ROTATE_90                   0x00000001
/* Data path surface configuration flags */
#define HAL_MDP_FLAGS_WB_SURFACE_USE_ALPHA_VAL_FROM_REGISTER   0x00000002        /**< Selects the Alpha value from MDP_WB_0_DST_ALPHA_X_VALUE register */
/* A surface configuration flag indicating if the surface is compressed */
#define HAL_MDP_FLAGS_SURFACE_COMPRESSED                       0x00000004
/* Flag to indicate that decimation is supported */
#define HAL_MDP_FLAGS_DECIMATION_SUPPORTED                     0x00000008   
/* Flag to indicate macro tile surface */
#define HAL_MDP_FLAGS_SURFACE_ATTR_MACRO_TILE                  0x00000010
/* Flag to indicate static frame interrupt feature*/
#define HAL_MDP_FLAG_FEATURE_STATIC_FRAME_DETECTION            0x00000020
/* Flag to indicate programmable fetch start feature */
#define HAL_MDP_FLAG_FEATURE_PROGRAMMABLE_FETCH_START          0x00000040
/* A surface configuration flag indicating if the surface is allocated in a secure context bank*/
#define HAL_MDP_FLAG_SURFACE_SECURE_ALLOCATION                 0x00000080
/* Flag to indicate if QDSS logging is supported on this chipset */
#define HAL_MDP_FLAG_FEATURE_QDSS_EVENTS                       0x00000100



/* This flag is used to start the overlay processor when HAL_MDP_ControlPath_Commit() is call in command mode */
#define HAL_MDP_FLAGS_COMMIT_FORCE_UPDATE                      0x00000001

/* Defines the maximum possible Number of register address and values passed to Register Modify */
#define HAL_MDP_REGISTER_MODIFY_MAX                            4

/* Used in uFlags param of HAL_MDP_LayerMixer_Config() 
   to indicate that this mixer drives right half of the display*/
#define HAL_MDP_FLAGS_SPLIT_DISPLAY_RIGHT_MIXER                0x00000001

/* Used in uSetupFlags param of HAL_MDP_Interface_TimingGen_Config() for reconfiguring the interface timing parameters 
   during dynamic refresh rate adjustment */
#define HAL_MDP_FLAGS_RECONFIGURE_INTERFACE_TIMING             0x00000010

/* Used in uSetupFlags param of HAL_MDP_Interface_DualInterface_Config() to flag if PP Split config is enabled  */
#define HAL_MDP_FLAGS_PP_SPLIT_ENABLED                         0x00000020

/* Used in uFlags param of HAL_MDP_ControlPath_InterfaceConfig() to mirror the interfaces */
#define HAL_MDP_FLAGS_MIRROR_INTERFACE                         0x00000040


/* Flags used for Pixel Extension (starting at 0x10000) */
/* Macrotile A4X and A5X flags */
#define HAL_MDP_FLAGS_MACROTILE_A4X                            0x00010000
#define HAL_MDP_FLAGS_MACROTILE_A5X                            0x00020000

/* IGC dither strength configuration */
#define HAL_MDP_IGC_DITHER_STRENGTH_10BIT                      0x0        /**< 10-bit data */
#define HAL_MDP_IGC_DITHER_STRENGTH_9BIT                       0x1        /**< 9-bit  data */
#define HAL_MDP_IGC_DITHER_STRENGTH_8BIT                       0x2        /**< 8-bit  data */
#define HAL_MDP_IGC_DITHER_STRENGTH_7BIT                       0x3        /**< 7-bit  data */
#define HAL_MDP_IGC_DITHER_STRENGTH_6BIT                       0x4        /**< 6-bit  data */
#define HAL_MDP_IGC_DITHER_STRENGTH_5BIT                       0x5        /**< 5-bit  data */
#define HAL_MDP_IGC_DITHER_STRENGTH_4BIT                       0x6        /**< 4-bit  data */
#define HAL_MDP_IGC_DITHER_STRENGTH_3BIT                       0x7        /**< 3-bit  data */

/*!
 * \enum HAL_MDP_ModuleType
 *
 * The \b HAL_MDP_ModuleType provides various types of modules
 * present in MDP HW
 *
 */
typedef enum _HAL_MDP_ModuleType
{
   HAL_MDP_MODULE_TYPE_NONE            = 0x00000000,
   HAL_MDP_MODULE_SOURCE_PIPE,            /**< Input Layers                                              */
   HAL_MDP_MODULE_LAYERMIXER,             /**< Layer Composition/Mixing                                  */
   HAL_MDP_MODULE_DESTINATION_PIPE,       /**< Panel Calibration                                         */
   HAL_MDP_MODULE_INTERFACE,              /**< DSI, HDMI and wireless display, BLT, Rotator etc          */
   HAL_MDP_MODULE_CONTROL_PATH,           /**< Control Path Connection of various MDP internal HW modules*/
   HAL_MDP_MODULE_DATA_PATH,              /**< Data Path Connection of various MDP internal HW modules   */
   HAL_MDP_MODULE_SMP,                    /**< Shared Memory Pool for source pipe inputs and write backs */
   HAL_MDP_MODULE_TRAFFIC_SHAPER,         /**< Traffic shaper module for QoS for memory fetch clients    */
   HAL_MDP_MODULE_VBIF,                   /**< MDP Memory Bus Interface Connector (SDRAM etc)            */
   HAL_MDP_MODULE_PINGPONG,               /**< MDP PingPong Block                                        */
   HAL_MDP_MODULE_DSC,                    /**< MDP Display Stream Compression                            */
   HAL_MDP_MODULE_SCM,                    /**< MDP Compression                                           */
   HAL_MDP_MODULE_DCE,                    /**< MDP Display Compression Engine                            */
   HAL_MDP_MODULE_PHYSICAL_INTERFACE,     /**< Reserved. Only for HAL MDP driver internal                */
   HAL_MDP_MODULE_WRITEBACK_INTERFACE,    /**< Reserved. Only for HAL MDP driver internal                */
   HAL_MDP_MODULE_WATCHDOG,               /**< MDP WatchDog timer                                        */
   HAL_MDP_MODULE_CDM,                    /**< MDP Chroma Down Module                                    */
   HAL_MDP_MODULE_MAX,
   HAL_MDP_MODULE_FORCE_32BIT          = 0x7FFFFFFF

} HAL_MDP_ModuleType;

/*!
 * \enum HAL_MDP_LayerType
 *
 * The \b HAL_MDP_LayerType lists various types of source pipes in MDP HW
 * HAL_MDP_LayerType is \b HAL_MDP_MODULE_SOURCE_PIPE of HAL_MDP_ModuleType
 */
typedef enum _HAL_MDP_LayerType
{
   HAL_MDP_LAYER_TYPE_NONE             = 0x00000000,
   HAL_MDP_LAYER_TYPE_RGB,                                                /**< Layers supporting only RGB input content                              */
   HAL_MDP_LAYER_TYPE_VIG,                                                /**< Layers supporting only RGB and YUV input content /w color processing  */
   HAL_MDP_LAYER_TYPE_DMA,                                                /**< Layers supporting only RGB and YUV input content /wo color processing */
   HAL_MDP_LAYER_TYPE_CURSOR,                                             /**< Cursor type. 32x32 or 64x64 size input surfaces                       */
   HAL_MDP_LAYER_TYPE_LAYERMIXER_CURSOR   = HAL_MDP_LAYER_TYPE_CURSOR,    /**< Reserved. Only for HAL driver internal usage                           */
   HAL_MDP_LAYER_TYPE_SSPP_CURSOR,                                        /**< (Reserved. Only for HAL driver internal usage                          */
   HAL_MDP_LAYER_TYPE_MAX,
   HAL_MDP_LAYER_TYPE_FORCE_32BIT      = 0x7FFFFFFF

} HAL_MDP_LayerType;

/*!
 * \enum HAL_MDP_SourcePipeId
 *
 * The \b HAL_MDP_SourcePipeId lists the layer identification for various \b HAL_MDP_LayerType
 */
typedef enum _HAL_MDP_SourcePipeId
{
   HAL_MDP_SOURCE_PIPE_NONE            = 0x00000000,

   /* HAL_MDP_LAYER_TYPE_VIG layer type (Supports both RGB and YUV data with additional pixel processing features)
    * 
    * HAL_MDP_##version##_NUM_OF_##layertype##_LAYERS will provide the number of VIG layers present in HW
    */
   HAL_MDP_SOURCE_PIPE_VIG_BASE,
   HAL_MDP_SOURCE_PIPE_VIG_0           = HAL_MDP_SOURCE_PIPE_VIG_BASE,
   HAL_MDP_SOURCE_PIPE_VIG_1,
   HAL_MDP_SOURCE_PIPE_VIG_2,
   HAL_MDP_SOURCE_PIPE_VIG_3,
   HAL_MDP_SOURCE_PIPE_VIG_4,
   HAL_MDP_SOURCE_PIPE_VIG_END         = HAL_MDP_SOURCE_PIPE_VIG_4,

   /* HAL_MDP_LAYER_TYPE_RGB layer type (Supports only RGB data with additional scaling capability)
    *
    * HAL_MDP_##version##_NUM_OF_##layertype##_LAYERS will provide the number of RGB layers present in HW
    */
   HAL_MDP_SOURCE_PIPE_RGB_BASE,
   HAL_MDP_SOURCE_PIPE_RGB_0           = HAL_MDP_SOURCE_PIPE_RGB_BASE,
   HAL_MDP_SOURCE_PIPE_RGB_1,
   HAL_MDP_SOURCE_PIPE_RGB_2,
   HAL_MDP_SOURCE_PIPE_RGB_3,
   HAL_MDP_SOURCE_PIPE_RGB_4,
   HAL_MDP_SOURCE_PIPE_RGB_END         = HAL_MDP_SOURCE_PIPE_RGB_4,

   /* HAL_MDP_LAYER_TYPE_DMA layer type (Supports both RGB and YUV data)
    *
    * HAL_MDP_##version##_NUM_OF_##layertype##_LAYERS will provide the number of DMA layers present in HW
    */
   HAL_MDP_SOURCE_PIPE_DMA_BASE,
   HAL_MDP_SOURCE_PIPE_DMA_0           = HAL_MDP_SOURCE_PIPE_DMA_BASE,
   HAL_MDP_SOURCE_PIPE_DMA_1,
   HAL_MDP_SOURCE_PIPE_DMA_2,
   HAL_MDP_SOURCE_PIPE_DMA_3,
   HAL_MDP_SOURCE_PIPE_DMA_4,
   HAL_MDP_SOURCE_PIPE_DMA_END         = HAL_MDP_SOURCE_PIPE_DMA_4,

   /* HAL_MDP_LAYER_TYPE_CURSOR layer type (Supports RGB888 or ARGB8888 pixel formats with 32x32 or 64x64 surfaces)
    *
    * HAL_MDP_##version##_NUM_OF_##layertype##_LAYERS will provide the number of CURSOR layers present in HW
    */
   HAL_MDP_SOURCE_PIPE_CURSOR_BASE,
   HAL_MDP_SOURCE_PIPE_CURSOR_0        = HAL_MDP_SOURCE_PIPE_CURSOR_BASE,
   HAL_MDP_SOURCE_PIPE_CURSOR_1,
   HAL_MDP_SOURCE_PIPE_CURSOR_2,
   HAL_MDP_SOURCE_PIPE_CURSOR_3,
   HAL_MDP_SOURCE_PIPE_CURSOR_4,
   HAL_MDP_SOURCE_PIPE_CURSOR_END      = HAL_MDP_SOURCE_PIPE_CURSOR_4,

   HAL_MDP_SOURCE_PIPE_SMART_DMA_BASE,
   HAL_MDP_SOURCE_PIPE_DMA_0_REC1      = HAL_MDP_SOURCE_PIPE_SMART_DMA_BASE,
   HAL_MDP_SOURCE_PIPE_DMA_1_REC1,
   HAL_MDP_SOURCE_PIPE_DMA_2_REC1,
   HAL_MDP_SOURCE_PIPE_DMA_3_REC1,
   HAL_MDP_SOURCE_PIPE_DMA_4_REC1,
   HAL_MDP_SOURCE_PIPE_SMART_DMA_END    = HAL_MDP_SOURCE_PIPE_DMA_4_REC1,
   
   HAL_MDP_SOURCE_PIPE_MAX,
   HAL_MDP_SOURCE_PIPE_FORCE_32BIT     = 0x7FFFFFFF

} HAL_MDP_SourcePipeId;

/*!
 * \enum HAL_MDP_LayerMixerId
 * 
 * The \b HAL_MDP_LayerMixerId lists various layer compositions present in MDP HW
 *
 * HAL_MDP_##version##_NUM_OF_LAYER_MIXERS provides the layer mixer present in the HW
 *
 * Each layer mixer has a limit in maximum layer composition.
 * HAL_MDP_##version##_LAYER_MIXER_MAX_BLEND_STAGES provides maximum layer composition supported by mixer
 *
 * HAL_MDP_LayerMixerId falls into HAL_MDP_MODULE_LAYERMIXER category of HAL_MDP_ModuleType
 */
typedef enum _HAL_MDP_LayerMixerId
{
   HAL_MDP_LAYER_MIXER_NONE            = 0x00000000,
   HAL_MDP_LAYER_MIXER_0,                 /**< Layer composition #0 */
   HAL_MDP_LAYER_MIXER_1,                 /**< Layer composition #1 */
   HAL_MDP_LAYER_MIXER_2,                 /**< Layer composition #2 */
   HAL_MDP_LAYER_MIXER_3,                 /**< Layer composition #3 */
   HAL_MDP_LAYER_MIXER_4,                 /**< Layer composition #4 */
   HAL_MDP_LAYER_MIXER_5,                 /**< Layer composition #5 */
   HAL_MDP_LAYER_MIXER_6,                 /**< Layer composition #6 */
   HAL_MDP_LAYER_MIXER_MAX,
   HAL_MDP_LAYER_MIXER_FORCE_32BIT     = 0x7FFFFFFF

} HAL_MDP_LayerMixerId;


/*!
 * \enum HAL_MDP_DestinationPipeId
 *
 * The \b HAL_MDP_DestinationPipeId lists the Destination Surface Pixel Processors identifications
 *
 * HAL_MDP_##version##_NUM_OF_##DSPP provides the maximum DSPPs present in the HW
 *
 * HAL_MDP_DestinationPipeId falls into HAL_MDP_MODULE_DESTINATION_PIPE category of HAL_MDP_ModuleType
 */
typedef enum _HAL_MDP_DestinationPipeId
{
   HAL_MDP_DESTINATION_PIPE_NONE             = 0x00000000,
   HAL_MDP_DESTINATION_PIPE_0,               /**< DSPP 0 */
   HAL_MDP_DESTINATION_PIPE_1,               /**< DSPP 1 */
   HAL_MDP_DESTINATION_PIPE_2,               /**< DSPP 2 */
   HAL_MDP_DESTINATION_PIPE_3,               /**< DSPP 3 */
   HAL_MDP_DESTINATION_PIPE_4,               /**< DSPP 4 */
   HAL_MDP_DESTINATION_PIPE_MAX,
   HAL_MDP_DESTINATION_PIPE_FORCE_32BIT      = 0x7FFFFFFF

} HAL_MDP_DestinationPipeId;


/*!
 * \enum HAL_MDP_PingPongId
 *
 * The \b HAL_MDP_PingPongId lists the pingpong block identifications
 */
typedef enum _HAL_MDP_PingPongId
{
  HAL_MDP_PINGPONG_NONE         = 0x00000000,
  HAL_MDP_PINGPONG_0,             /**< PingPong_0 */
  HAL_MDP_PINGPONG_1,             /**< PingPong_1 */
  HAL_MDP_PINGPONG_2,             /**< PingPong_2 */
  HAL_MDP_PINGPONG_3,             /**< PingPong_3 */
  HAL_MDP_PINGPONG_4,             /**< PingPong_4 */
  HAL_MDP_PINGPONG_MAX,
  HAL_MDP_PINGPONG_FORCE_32BIT  = 0x7FFFFFFF
} HAL_MDP_PingPongId;

/*!
 * \enum HAL_MDP_InterfaceId
 *
 * The \b HAL_MDP_InterfaceId lists both physical and write-back interface connections from MDP HW
 *
 */
typedef enum _HAL_MDP_InterfaceId
{
   HAL_MDP_INTERFACE_NONE                    = 0x00000000,
   HAL_MDP_INTERFACE_0,                            /**< Physical Interface 0 (INTF_0) */
   HAL_MDP_INTERFACE_1,                            /**< Physical Interface 1 (INTF_1) */
   HAL_MDP_INTERFACE_2,                            /**< Physical Interface 2 (INTF_2) */
   HAL_MDP_INTERFACE_3,                            /**< Physical Interface 3 (INTF_3) */
   HAL_MDP_INTERFACE_4,                            /**< Physical Interface 4 (INTF_4) */
   HAL_MDP_INTERFACE_5,                            /**< Physical Interface 5 (INTF_5) */

   HAL_MDP_INTERFACE_WRITEBACK_TYPE_BASE,
   HAL_MDP_INTERFACE_MEMORY_WB_BLK_MODE_0,         /**< Write-Back Interface 0 block mode (WB0 in BLK mode)    */
   HAL_MDP_INTERFACE_MEMORY_WB_BLK_MODE_1,         /**< Write-Back Interface 1 block mode (WB1 in BLK mode)    */
   HAL_MDP_INTERFACE_MEMORY_WB_LINE_MODE_0,        /**< Write-Back Interface 0 line mode  (WB0 in LINE mode)   */
   HAL_MDP_INTERFACE_MEMORY_WB_LINE_MODE_1,        /**< Write-Back Interface 1 line mode  (WB1 in LINE mode)   */
   HAL_MDP_INTERFACE_MEMORY_WB_LINEAR_MODE,        /**< Write-Back Interface 2 line mode  (WB2 in LINE mode)   */

   HAL_MDP_INTERFACE_MAX,
   HAL_MDP_INTERFACE_FORCE_32BIT             = 0x7FFFFFFF

} HAL_MDP_InterfaceId;

/*!
 * \enum HAL_MDP_InterfaceStreamId
 * 
 * The \b HAL_MDP_InterfaceStreamId lists all the interface stream IDs supported by MDP HW 
 */
typedef enum _HAL_MDP_InterfaceStreamId
{
   HAL_MDP_INTERFACE_STREAM_VIDEO_0          = 0x00000000,     /** Video-mode stream 0          */
   HAL_MDP_INTERFACE_STREAM_CMD_0,                             /** Command mode stream 0        */
   HAL_MDP_INTERFACE_STREAM_CMD_1,                             /** Command mode stream 1        */
   HAL_MDP_INTERFACE_STREAM_CMD_2,                             /** Command mode stream 2        */
   HAL_MDP_INTERFACE_STREAM_MAX,
   HAL_MDP_INTERFACE_STREAM_FORCE32BIT       = 0x7FFFFFFF

} HAL_MDP_InterfaceStreamId;

/*!
 * \enum HAL_MDP_ControlPathId
 *
 * The \b HAL_MDP_ControlPathId lists the control paths present in the MDP HW
 *
 * HAL_MDP_##version##_NUM_OF_CONTROL_PATHS provides the maximum physical interfaces present in the HW
 *
 * HAL_MDP_ControlPathId falls into HAL_MDP_MODULE_CONTROL_PATH category of HAL_MDP_ModuleType
 *
 * MDP HW supports multiple control paths. The number of control paths = The number of layer mixers.
 * Control path = { one (or) more input layers +
 *                  one layer mixer            +
 *                  one DSPP (optional)        + (Based on Layer Mixer Selection)
 *                  one physical interface (or) one write-back interface in line or block mode
 *                }
 *
 */
typedef enum _HAL_MDP_ControlPathId
{
   HAL_MDP_CONTROL_PATH_NONE              = 0x00000000,
   HAL_MDP_CONTROL_PATH_0,                /**< CONTROL PATH 0 */
   HAL_MDP_CONTROL_PATH_1,                /**< CONTROL PATH 1 */
   HAL_MDP_CONTROL_PATH_2,                /**< CONTROL PATH 2 */
   HAL_MDP_CONTROL_PATH_3,                /**< CONTROL PATH 3 */
   HAL_MDP_CONTROL_PATH_4,                /**< CONTROL PATH 4 */
   HAL_MDP_CONTROL_PATH_5,                /**< CONTROL PATH 5 */
   HAL_MDP_CONTROL_PATH_6,                /**< CONTROL PATH 6 */
   HAL_MDP_CONTROL_PATH_7,                /**< CONTROL PATH 7 */
   HAL_MDP_CONTROL_PATH_MAX,
   HAL_MDP_CONTROL_PATH_FORCE_32BIT       = 0x7FFFFFFF

} HAL_MDP_ControlPathId;


/*!
 * \enum HAL_MDP_DataPathId
 *
 * The \b HAL_MDP_DataPathId lists the control paths present in the MDP HW
 *
 * HAL_MDP_##version##_NUM_OF_DATA_PATHS provides the maximum physical interfaces present in the HW
 *
 * HAL_MDP_DataPathId falls into HAL_MDP_MODULE_DATA_PATH category of HAL_MDP_ModuleType
 *
 * MDP HW supports multiple data paths. The number of data paths = The number of control paths.
 *
 * Data path =    { control path with write back interface selection +
 *                  Type of write back (Blit mode i.e. mixer output to memory [OR]
 *                                      Rotator i.e. single layer rotated image to memory [OR]
 *                                      Linear write back i.e wireless display (mixer output + DSPP color processing)
                                       )
 *                  
 *                }
 * If HAL_MDP_CONTROL_PATH_X is used for any write back operation, then the corresponding HAL_MDP_DATA_PATH_X
 * must be configured to select the type of write back required
 *
 */
typedef enum _HAL_MDP_DataPathId
{
   HAL_MDP_DATA_PATH_NONE              = 0x00000000,
   HAL_MDP_DATA_PATH_0,                /**< DATA PATH 0 */
   HAL_MDP_DATA_PATH_1,                /**< DATA PATH 1 */
   HAL_MDP_DATA_PATH_2,                /**< DATA PATH 2 */
   HAL_MDP_DATA_PATH_3,                /**< DATA PATH 3 */
   HAL_MDP_DATA_PATH_4,                /**< DATA PATH 4 */
   HAL_MDP_DATA_PATH_5,                /**< DATA PATH 5 */
   HAL_MDP_DATA_PATH_6,                /**< DATA PATH 6 */
   HAL_MDP_DATA_PATH_7,                /**< DATA PATH 7 */
   HAL_MDP_DATA_PATH_MAX,
   HAL_MDP_DATA_PATH_FORCE_32BIT       = 0x7FFFFFFF

} HAL_MDP_DataPathId;

/*!
 * \enum HAL_MDP_VbifId
 *
 * The \b _HAL_MDP_VbifId  lists the VBIF blocks present in the MDP HW.
 *
 */
typedef enum _HAL_MDP_VbifId
{
    HAL_MDP_VBIF_0              = 0x00000000,       /* VBIF BLOCK 0 */
    HAL_MDP_VBIF_1,                                 /* VBIF BLOCK 1 */
    HAL_MDP_VBIF_MAX,
    HAL_MDP_VBIF_FORCE_32BIT    = 0x7FFFFFFF
} HAL_MDP_VbifId;

/*!
 * \enum _HAL_MDP_ChromaDownModuleId
 *
 * The \b _HAL_MDP_ChromaDownModuleId  lists the Chroma down blocks present in the MDP Hw.
 *
 * MDP HW version 590 supports one chroma down module.This can be used either with the HDMI Interface 
 * or wireless Display(WB2) interface.
 *  
 * HAL_MDP_ChromaDownModuleId is \b HAL_MDP_MODULE_CDM of HAL_MDP_ModuleType
 */
typedef enum _HAL_MDP_ChromaDownModuleId
{
   HAL_MDP_CDM_NONE              = 0x00000000,
   HAL_MDP_CDM_0,                /**< CHROMA DOWN MODULE 0 */ 
   HAL_MDP_CDM_MAX, 
   HAL_MDP_CDM_FORCE_32BIT       = 0x7FFFFFFF 

} HAL_MDP_ChromaDownModuleId;

/*!
 * \enum _HAL_MDP_SMP_MMBClientId
 *
 * The \b HAL_MDP_SMP_MMBClientId lists the clients for SMP Memory Macro Blocks
 *
 * SMP   - Shared Memory Pool (Used as latency line buffers for VIG, RGB and DMA layers)
 * MMB   - Memory Macro Block (SMP is a list of MMBs)
 *
 * Each MMB is size of 4KB.
 */
typedef enum _HAL_MDP_SMP_MMBClientId
{
   HAL_MDP_SMP_MMB_CLIENT_NONE            = 0x00000000,    /**< NO CLIENT */
   HAL_MDP_SMP_MMB_CLIENT_VIG0_FETCH_Y,
   HAL_MDP_SMP_MMB_CLIENT_VIG0_FETCH_CR,
   HAL_MDP_SMP_MMB_CLIENT_VIG0_FETCH_CB,
   HAL_MDP_SMP_MMB_CLIENT_VIG1_FETCH_Y,
   HAL_MDP_SMP_MMB_CLIENT_VIG1_FETCH_CR,
   HAL_MDP_SMP_MMB_CLIENT_VIG1_FETCH_CB,
   HAL_MDP_SMP_MMB_CLIENT_VIG2_FETCH_Y,
   HAL_MDP_SMP_MMB_CLIENT_VIG2_FETCH_CR,
   HAL_MDP_SMP_MMB_CLIENT_VIG2_FETCH_CB,
   HAL_MDP_SMP_MMB_CLIENT_VIG3_FETCH_Y,
   HAL_MDP_SMP_MMB_CLIENT_VIG3_FETCH_CR,
   HAL_MDP_SMP_MMB_CLIENT_VIG3_FETCH_CB,
   HAL_MDP_SMP_MMB_CLIENT_DMA0_FETCH_Y,
   HAL_MDP_SMP_MMB_CLIENT_DMA0_FETCH_CR,
   HAL_MDP_SMP_MMB_CLIENT_DMA0_FETCH_CB,
   HAL_MDP_SMP_MMB_CLIENT_DMA1_FETCH_Y,
   HAL_MDP_SMP_MMB_CLIENT_DMA1_FETCH_CR,
   HAL_MDP_SMP_MMB_CLIENT_DMA1_FETCH_CB,
   HAL_MDP_SMP_MMB_CLIENT_RGB0_FETCH,
   HAL_MDP_SMP_MMB_CLIENT_RGB1_FETCH,
   HAL_MDP_SMP_MMB_CLIENT_RGB2_FETCH,
   HAL_MDP_SMP_MMB_CLIENT_RGB3_FETCH,
   HAL_MDP_SMP_MMB_CLIENT_MAX,
   HAL_MDP_SMP_MMB_CLIENT_FORCE_32BIT     = 0x7FFFFFFF

} HAL_MDP_SMP_MMBClientId;

/*!
 * \enum _HAL_MDP_TrafficCtrl_ClientIDType
 *
 * The _HAL_MDP_TrafficCtrl_ClientIDType lists the clients of traffic control which makes use of
 * Traffic Shaper and QoS priority signals.
 *
 * Traffic Shaper controls the traffic between MDP and VBIF;
 * QoS signals generated for each mem access are passed on to system for memory access arbitration.
 *
 * Although there is no traffic shaper for all cursors, each cursor has its own set of watermark 
 * registers, hence 5 cursor clients are listed out separately in order to let each of them has 
 * different QoS priority setting.
 *
 */
typedef enum _HAL_MDP_TrafficCtrl_ClientIDType
{
  HAL_MDP_TRFCTRL_CLIENT_NONE            = 0x00000000,
  HAL_MDP_TRFCTRL_CLIENT_VIG0,
  HAL_MDP_TRFCTRL_CLIENT_VIG1,
  HAL_MDP_TRFCTRL_CLIENT_VIG2,
  HAL_MDP_TRFCTRL_CLIENT_VIG3,
  HAL_MDP_TRFCTRL_CLIENT_RGB0,
  HAL_MDP_TRFCTRL_CLIENT_RGB1,
  HAL_MDP_TRFCTRL_CLIENT_RGB2,
  HAL_MDP_TRFCTRL_CLIENT_RGB3,
  HAL_MDP_TRFCTRL_CLIENT_DMA0,
  HAL_MDP_TRFCTRL_CLIENT_DMA1,
  HAL_MDP_TRFCTRL_CLIENT_DMA2,
  HAL_MDP_TRFCTRL_CLIENT_DMA3,  
  HAL_MDP_TRFCTRL_CLIENT_LAYER0_CURSOR,
  HAL_MDP_TRFCTRL_CLIENT_LAYER1_CURSOR,
  HAL_MDP_TRFCTRL_CLIENT_LAYER2_CURSOR,
  HAL_MDP_TRFCTRL_CLIENT_LAYER3_CURSOR,
  HAL_MDP_TRFCTRL_CLIENT_LAYER4_CURSOR,
  HAL_MDP_TRFCTRL_CLIENT_WB0,
  HAL_MDP_TRFCTRL_CLIENT_WB1,
  HAL_MDP_TRFCTRL_CLIENT_WB2,
  HAL_MDP_TRFCTRL_CLIENT_ROT_SSPP,
  HAL_MDP_TRFCTRL_CLIENT_ROT_WB,
  HAL_MDP_TRFCTRL_CLIENT_MAX,
  HAL_MDP_TRFCTRL_CLIENT_FORCE_32BIT     = 0x7FFFFFFF

} HAL_MDP_TrafficCtrl_ClientIDType;

/*!
 * \enum _HAL_MDP_VBIF_ClientIDType
 *
 * The _HAL_MDP_VBIF_ClientIDType lists the VBIF clients based on their index.
 *
 */
typedef enum _HAL_MDP_VBIF_ClientIDType
{
  HAL_MDP_VBIF_CLIENT_VIG0 = 0x00000000,
  HAL_MDP_VBIF_CLIENT_RGB0,
  HAL_MDP_VBIF_CLIENT_DMA0,
  HAL_MDP_VBIF_CLIENT_WB0,
  HAL_MDP_VBIF_CLIENT_VIG1,
  HAL_MDP_VBIF_CLIENT_RGB1,
  HAL_MDP_VBIF_CLIENT_WB2,
  HAL_MDP_VBIF_CLIENT_CURSOR,
  HAL_MDP_VBIF_CLIENT_VIG2,
  HAL_MDP_VBIF_CLIENT_RGB2,
  HAL_MDP_VBIF_CLIENT_DMA1,  
  HAL_MDP_VBIF_CLIENT_WB1,  
  HAL_MDP_VBIF_CLIENT_VIG3,  
  HAL_MDP_VBIF_CLIENT_RGB3,
  HAL_MDP_VBIF_CLIENT_CURSOR0,
  HAL_MDP_VBIF_CLIENT_CURSOR1,  
  HAL_MDP_VBIF_CLIENT_DMA2,  
  HAL_MDP_VBIF_CLIENT_DMA3,
  HAL_MDP_VBIF_CLIENT_ROT_RD,
  HAL_MDP_VBIF_CLIENT_ROT_WR,
  HAL_MDP_VBIF_CLIENT_DSIDMA,
  HAL_MDP_VBIF_CLIENT_MAX,
  HAL_MDP_VBIF_CLIENT_FORCE_32BIT     = 0x7FFFFFFF
} HAL_MDP_VBIF_ClientIDType;


/*!
 * \enum _HAL_MDP_WatchDogId
 *
 * The _HAL_MDP_WatchDogId lists the watch dogs that presented in MDP HW.
 *
 */
typedef enum _HAL_MDP_WatchDogId
{
   HAL_MDP_WATCHDOG_NONE              = 0x00000000,
   HAL_MDP_WATCHDOG_0,                /**< WATCHDOG 0 */
   HAL_MDP_WATCHDOG_1,                /**< WATCHDOG 1 */
   HAL_MDP_WATCHDOG_2,                /**< WATCHDOG 2 */
   HAL_MDP_WATCHDOG_3,                /**< WATCHDOG 3 */
   HAL_MDP_WATCHDOG_4,                /**< WATCHDOG 4 */
   HAL_MDP_WATCHDOG_5,                /**< WATCHDOG 5 */
   HAL_MDP_WATCHDOG_MAX,
   HAL_MDP_WATCHDOG_FORCE_32BIT      = 0x7FFFFFFF
} HAL_MDP_WatchDogId;

/*!
 * \enum _HAL_MDP_SplitMUXId
 *
 * The HAL_MDP_DSCId lists the Display Stream Compression (DSC) Encoder engine module IDs
 */
typedef enum _HAL_MDP_SplitMUXId
{
   HAL_MDP_SPLITMUX_NONE                 = 0x00000000,
   HAL_MDP_SPLITMUX_0,                   /**< SPLIT-MUXER 0 (PPB0)  */
   HAL_MDP_SPLITMUX_1,                   /**< SPLIT-MUXER 1 (PPB1)  */
   HAL_MDP_SPLITMUX_MAX,
   HAL_MDP_SPLITMUX_FORCE_32BIT          = 0x7FFFFFFF
} HAL_MDP_SplitMUXId;

/*!
 * \enum _HAL_MDP_DSCId
 *
 * The HAL_MDP_DSCId lists the Display Stream Compression (DSC) Encoder engine module IDs
 */
typedef enum _HAL_MDP_DSCId
{
   HAL_MDP_DSC_NONE                 = 0x00000000,
   HAL_MDP_DSC_0,                   /**< DSC ENCODER ENGINE 0  */
   HAL_MDP_DSC_1,                   /**< DSC ENCODER ENGINE 1  */
   HAL_MDP_DSC_2,                   /**< DSC ENCODER ENGINE 2  */
   HAL_MDP_DSC_3,                   /**< DSC ENCODER ENGINE 3  */
   HAL_MDP_DSC_MAX,
   HAL_MDP_DSC_FORCE_32BIT          = 0x7FFFFFFF
} HAL_MDP_DSCId;

/*!
 * \enum _HAL_MDP_DCEId
 *
 * The HAL_MDP_DCEId lists the Display Compression Engine (DCE) module IDs
 */
typedef enum _HAL_MDP_DCEId
{
   HAL_MDP_DCE_NONE              = 0x00000000,
   HAL_MDP_DCE_0,                /**< DCE 0     */
   HAL_MDP_DCE_1,                /**< DCE 1     */
   HAL_MDP_DCE_MAX,
   HAL_MDP_DCE_FORCE_32BIT       = 0x7FFFFFFF,

} HAL_MDP_DCEId;

/*!
 * \enum _HAL_MDP_TrafficCtrl_LatencyType
 *
 * The _HAL_MDP_TrafficCtrl_LatencyType lists 4 levels of QoS latency for memory access.
 *
 * The latency setting of each read client is programmed via the Watermark levels of MMBs in SMP.
 * Write client's latency is directly controlled by a register field.
 *
 */
typedef enum _HAL_MDP_TrafficCtrl_LatencyType
{
  HAL_MDP_TRFCTRL_LATENCY_BEST_EFFORT    = 0x00000000,    /* reserved, for testing only */
  HAL_MDP_TRFCTRL_LATENCY_NON_REALTIME,                   /* for clients such as write back, rotation, etc.*/
  HAL_MDP_TRFCTRL_LATENCY_REALTIME,                       /* for clients involved in real time display */
  HAL_MDP_TRFCTRL_LATENCY_ULTRA_LOW_LATENCY,              /* reserved, for testing only */
  HAL_MDP_TRFCTRL_LATENCY_MAX,
  HAL_MDP_TRFCTRL_LATENCY_FORCE_32BIT    = 0x7FFFFFFF

} HAL_MDP_TrafficCtrl_LatencyType;

/*!
 * \enum HAL_MDP_InterruptType
 *
 * The \b HAL_MDP_InterruptType provides the possible configuration/status read
 * interrupts for various MDP HW modules i.e. \b HAL_MDP_ModuleType.
 *
 * The user can configure/read the interrupts/status for various blocks using
 * HAL_MDP_Interrupt_xxxx()
 */
typedef enum _HAL_MDP_InterruptType
{
   HAL_MDP_INTERRUPT_NONE                           = 0x00000000,
   /**< Only for HAL_MDP_MODULE_PHYSICAL_INTERFACE and HAL_MDP_MODULE_WRITEBACK_INTERFACE module types */
   HAL_MDP_INTERRUPT_DATA_TRANSFER_DONE             = HAL_MDSS_BIT(1),
   /**< Only for HAL_MDP_MODULE_PHYSICAL_INTERFACE module type with smart panels */
   HAL_MDP_INTERRUPT_PINGPONG_WR_PTR                = HAL_MDSS_BIT(2),
   /**< PingPong block write pointer interrupt */
   HAL_MDP_INTERRUPT_PINGPONG_RD_PTR                = HAL_MDSS_BIT(3),
   /**< PingPong block read pointer interrupt */
   HAL_MDP_INTERRUPT_AUTO_REFRESH                   = HAL_MDSS_BIT(4),
   /**< Only for HAL_MDP_MODULE_PHYSICAL_INTERFACE module type with active refresh panels */
   HAL_MDP_INTERRUPT_UNDERRUN                       = HAL_MDSS_BIT(5),
   /**< Only for HAL_MDP_MODULE_PHYSICAL_INTERFACE module type with active refresh panels */
   HAL_MDP_INTERRUPT_VSYNC                          = HAL_MDSS_BIT(6),
   /**< Only for HAL_MDP_MODULE_DESTINATION_PIPE and HAL_MDP_MODULE_SOURCE_PIPE */
   HAL_MDP_INTERRUPT_HIST_RESET_DONE                = HAL_MDSS_BIT(7),
   /**< Only for HAL_MDP_MODULE_DESTINATION_PIPE and HAL_MDP_MODULE_SOURCE_PIPE */
   HAL_MDP_INTERRUPT_HIST_DONE                      = HAL_MDSS_BIT(8),
   /**< Only for HAL_MDP_MODULE_PHYSICAL_INTERFACE module type */
   HAL_MDP_INTERRUPT_PINGPONG_DONE                  = HAL_MDSS_BIT(9),
   /**< PingPong block done interrupt */
   HAL_MDP_INTERRUPT_WATCHDOG_DONE                  = HAL_MDSS_BIT(10),
   /**< WatchDog done interrupt */
   HAL_MDP_INTERRUPT_ENTER_STATIC_DETECTION         = HAL_MDSS_BIT(11),
   /**< Enter Into-Static Frame detection interrupt */
   HAL_MDP_INTERRUPT_EXIT_STATIC_DETECTION          = HAL_MDSS_BIT(12),
   /**< Exit Out-Of-Static Frame detection interrupt */

   HAL_MDP_INTERRUPT_MAX                            = HAL_MDSS_BIT(30),    // Never use this macro for table declarations
   HAL_MDP_INTERRUPT_FORCE_32_BIT                   = 0x7FFFFFFF

} HAL_MDP_InterruptType;

/*!
 * \enum HAL_MDP_Misr_BlockId
 *
 * The \b HAL_MDP_Misr_BlockId helps in configuring any LP MISR
 * block.
 *
 */
typedef enum _HAL_MDP_Misr_BlockId
{
   HAL_MDP_MISR_BLOCK_CURSOR_UNPACK_0          = 0x00000000,
   HAL_MDP_MISR_BLOCK_CURSOR_UNPACK_1          = 0x00000001,
   HAL_MDP_MISR_BLOCK_CURSOR_UNPACK_2          = 0x00000002,
   HAL_MDP_MISR_BLOCK_LMIX0_BLEND              = 0x00000008,
   HAL_MDP_MISR_BLOCK_LMIX0_GC                 = 0x00000009,
   HAL_MDP_MISR_BLOCK_LMIX1_BLEND              = 0x0000000a,
   HAL_MDP_MISR_BLOCK_LMIX1_GC                 = 0x0000000b,
   HAL_MDP_MISR_BLOCK_LMIX2_BLEND              = 0x0000000c,
   HAL_MDP_MISR_BLOCK_LMIX2_GC                 = 0x0000000d,
   HAL_MDP_MISR_BLOCK_LMIX3_BLEND              = 0x0000000e,
   HAL_MDP_MISR_BLOCK_LMIX3_GC                 = 0x0000000f,
   HAL_MDP_MISR_BLOCK_LMIX4_BLEND              = 0x00000010,
   HAL_MDP_MISR_BLOCK_LMIX4_GC                 = 0x00000011,
   HAL_MDP_MISR_BLOCK_DSPP0_PA                 = 0x00000012,
   HAL_MDP_MISR_BLOCK_DSPP0_IGC                = 0x00000013,
   HAL_MDP_MISR_BLOCK_DSPP0_GAMUTM             = 0x00000014,
   HAL_MDP_MISR_BLOCK_DSPP0_PCC                = 0x00000015,
   HAL_MDP_MISR_BLOCK_DSPP0_PANELC_LUT         = 0x00000016,
   HAL_MDP_MISR_BLOCK_DSPP0_DITHER             = 0x00000017,
   HAL_MDP_MISR_BLOCK_DSPP1_PA                 = 0x00000018,
   HAL_MDP_MISR_BLOCK_DSPP1_IGC                = 0x00000019,
   HAL_MDP_MISR_BLOCK_DSPP1_GAMUTM             = 0x0000001a,
   HAL_MDP_MISR_BLOCK_DSPP1_PCC                = 0x0000001b,
   HAL_MDP_MISR_BLOCK_DSPP1_PANELC_LUT         = 0x0000001c,
   HAL_MDP_MISR_BLOCK_DSPP1_DITHER             = 0x0000001d,
   HAL_MDP_MISR_BLOCK_DSPP2_PA                 = 0x0000001e,
   HAL_MDP_MISR_BLOCK_DSPP2_IGC                = 0x0000001f,
   HAL_MDP_MISR_BLOCK_DSPP2_GAMUTM             = 0x00000020,
   HAL_MDP_MISR_BLOCK_DSPP2_PCC                = 0x00000021,
   HAL_MDP_MISR_BLOCK_DSPP2_PANELC_LUT         = 0x00000022,
   HAL_MDP_MISR_BLOCK_DSPP2_DITHER             = 0x00000023,
   HAL_MDP_MISR_BLOCK_PPB0_0                   = 0x00000024,
   HAL_MDP_MISR_BLOCK_PPB0_1                   = 0x00000025,
   HAL_MDP_MISR_BLOCK_PPB1_0                   = 0x00000026,
   HAL_MDP_MISR_BLOCK_PPB1_1                   = 0x00000027,
   HAL_MDP_MISR_BLOCK_PPB2_0                   = 0x00000028,
   HAL_MDP_MISR_BLOCK_PPB2_1                   = 0x00000029,
   HAL_MDP_MISR_BLOCK_ROT0_CSC                 = 0x0000002a,
   HAL_MDP_MISR_BLOCK_ROT0_CHROMAD             = 0x0000002b,
   HAL_MDP_MISR_BLOCK_ROT0_DITHER              = 0x0000002c,
   HAL_MDP_MISR_BLOCK_ROT0_PACKER              = 0x0000002d,
   HAL_MDP_MISR_BLOCK_ROT0_ROTATE_0            = 0x00000030,
   HAL_MDP_MISR_BLOCK_ROT0_ROTATE_1            = 0x00000031,
   HAL_MDP_MISR_BLOCK_ROT0_ROTATE_2            = 0x00000032,
   HAL_MDP_MISR_BLOCK_ROT0_ROTATE_3            = 0x00000033,
   HAL_MDP_MISR_BLOCK_ROT0_BWC_0               = 0x00000034,
   HAL_MDP_MISR_BLOCK_ROT0_BWC_1               = 0x00000035,
   HAL_MDP_MISR_BLOCK_ROT0_BWC_2               = 0x00000036,
   HAL_MDP_MISR_BLOCK_ROT0_BWC_3               = 0x00000037,
   HAL_MDP_MISR_BLOCK_ROT1_CSC                 = 0x00000038,
   HAL_MDP_MISR_BLOCK_ROT1_CHROMAD             = 0x00000039,
   HAL_MDP_MISR_BLOCK_ROT1_DITHER              = 0x0000003a,
   HAL_MDP_MISR_BLOCK_ROT1_PACKER              = 0x0000003b,
   HAL_MDP_MISR_BLOCK_ROT1_ROTATE_0            = 0x0000003c,
   HAL_MDP_MISR_BLOCK_ROT1_ROTATE_1            = 0x0000003d,
   HAL_MDP_MISR_BLOCK_ROT1_ROTATE_2            = 0x0000003e,
   HAL_MDP_MISR_BLOCK_ROT1_ROTATE_3            = 0x0000003f,
   HAL_MDP_MISR_BLOCK_ROT1_BWC_0               = 0x00000040,
   HAL_MDP_MISR_BLOCK_ROT1_BWC_1               = 0x00000041,
   HAL_MDP_MISR_BLOCK_ROT1_BWC_2               = 0x00000042,
   HAL_MDP_MISR_BLOCK_ROT1_BWC_3               = 0x00000043,
   HAL_MDP_MISR_BLOCK_3D_MUX_0                 = 0x00000044,
   HAL_MDP_MISR_BLOCK_3D_MUX_1                 = 0x00000045,
   HAL_MDP_MISR_BLOCK_3D_MUX_2                 = 0x00000046,
   HAL_MDP_MISR_BLOCK_MAX                      = 0x00000047,
   HAL_MDP_MISR_BLOCK_FORCE_32_BIT             = 0x7FFFFFFF
} HAL_MDP_Misr_BlockId;

/*!
 * \enum HAL_MDP_MISR_Status_Id
 *
 * The \b HAL_MDP_MISR_Status_Id provides the status/signature of a MISR block.
 *
 */
typedef enum _HAL_MDP_MISR_Status_Id
{
   HAL_MDP_MISR_STATUS_NONE              = 0x00000000,
   HAL_MDP_MISR_STATUS_READ_SIGNATURE,                  /**< Reads the CRC value                     */
   HAL_MDP_MISR_STATUS_READ_STATE,                      /**< Returns CRC generation complete status  */
   HAL_MDP_MISR_STATUS_READ_CONFIG_STATE,               /**< Returns MISR current config reg status  */
   HAL_MDP_MISR_STATUS_MAX,
   HAL_MDP_MISR_STATUS_FORCE_32BIT       = 0x7FFFFFFF

} HAL_MDP_MISR_Status_Id;

/*!
 * \enum HAL_MDP_ColorEncodeType
 *
 * The \b HAL_MDP_ColorEncodeType lists the various CSC encoding matrix supported by MDP HAL driver
 *
 * The various MDP HW blocks works in various color space domains.
 * 
 * The input content for HW block may be required to convert into RGB color space. The YUV content
 *
 * Examples:
 * VIG layer - HSIC feature works in RGB888 domain (Requires input YUV data conversion to RGB domain)
 * WRITE back module supports YUV format  - The input to Write back module is RGB 12:12:12 bit. Required to choose
 *                                          type of CSC to RGB to YUV conversion
 *
 */
typedef enum _HAL_MDP_ColorEncodeType
{
   HAL_MDP_COLOR_ENCODING_DEFAULT      = 0x00000000,
   HAL_MDP_COLOR_ENCODING_REC601,      /**< ITU-R BT.601 (CCIR601)  */
   HAL_MDP_COLOR_ENCODING_REC709,      /**< ITU-R BT.709 (CCIR709)  */
   HAL_MDP_COLOR_ENCODING_sRGB,        /**< Standard RGB            */
   HAL_MDP_COLOR_ENCODING_FULL_REC601, /**< ITU.BT-601 SDTV         */   
   HAL_MDP_COLOR_ENCODING_FULL_REC709, /**< ITU.BT-709 HDTV         */ 
   HAL_MDP_COLOR_ENCODING_MAX,
   HAL_MDP_COLOR_ENCODING_FORCE_32BIT = 0x7FFFFFFF

} HAL_MDP_ColorEncodeType;

/*!
 * \enum HAL_MDP_FetchConfigType
 *
 * The \b HAL_MDP_FetchConfigType lists various fetch configurations supported for layers
 *
 * Horizontal and vertical mirror configuration
 */
typedef enum _HAL_MDP_FetchConfigType
{
   HAL_MDP_FETCH_DIRECTION_NORMAL         = 0x00000000,        /**< No mirror          */
   HAL_MDP_FETCH_DIRECTION_RIGHT_TO_LEFT  = HAL_MDSS_BIT(0),   /**< Horizontal mirror  */
   HAL_MDP_FETCH_DIRECTION_DOWN_TO_UP     = HAL_MDSS_BIT(1),   /**< Vertical mirror    */
   HAL_MDP_FETCH_DIRECTION_MAX,
   HAL_MDP_FETCH_DIRECTION_FORCE_32BIT    = 0x7FFFFFFF

} HAL_MDP_FetchConfigType;

/*!
 * \enum HAL_MDP_ScaleFilterMode
 *
 * The \b HAL_MDP_ScaleFilterMode lists the possible filter modes supported for scaling operations
 *
 */
typedef enum _HAL_MDP_ScaleFilterMode
{
   HAL_MDP_SCALE_FILTER_NEAREST_NEIGHBOR     = 0x00000000,  /**< Nearest Neighbor pixel filter        */
   HAL_MDP_SCALE_FILTER_BILINEAR,                           /**< Bilinear                             */
   HAL_MDP_SCALE_FILTER_PCMN,                               /**< PCMN filter                          */
   HAL_MDP_SCALE_FILTER_CONTENT_ADAPTIVE,                   /**< Content Adaptive Filter. (Reserved)  */
   HAL_MDP_SCALE_FILTER_MAX,
   HAL_MDP_SCALE_FILTER_FORCE_32BIT          = 0x7FFFFFFF

} HAL_MDP_ScaleFilterMode;

/*!
 * \enum QDI_VC1RangeLevelType
 *
 * Defines mapping levels for Luma and Chroma components during VC1 Range Mapping
 * If QDI_VC1_RANGE_LEVELx is specified, then the Y/UV components are scaled using following equation:
 *      Y[n] = CLIP(((((Y[n] - 128) * (x + 9)) + 4) >> 3) + 128)
 * 
 *
 */
typedef enum _HAL_MDP_VC1RangeLevelType
{
   HAL_MDP_VC1_RANGE_LEVEL_NONE        = 0x00000000,     /**< Range Mapping is disabled                                          */
   HAL_MDP_VC1_RANGE_LEVEL0,                             /**< Range mapping is performed using the above equation, where x = 0   */
   HAL_MDP_VC1_RANGE_LEVEL1,                             /**< x = 1                                                              */
   HAL_MDP_VC1_RANGE_LEVEL2,                             /**< x = 2                                                              */
   HAL_MDP_VC1_RANGE_LEVEL3,                             /**< x = 3                                                              */
   HAL_MDP_VC1_RANGE_LEVEL4,                             /**< x = 4                                                              */
   HAL_MDP_VC1_RANGE_LEVEL5,                             /**< x = 5                                                              */
   HAL_MDP_VC1_RANGE_LEVEL6,                             /**< x = 6                                                              */
   HAL_MDP_VC1_RANGE_LEVEL7,                             /**< x = 7                                                              */
   HAL_MDP_VC1_RANGE_LEVEL_MAX,
   HAL_MDP_VC1_RANGE_LEVEL_FORCE_32BIT = 0x7FFFFFFF

} HAL_MDP_VC1RangeLevelType;

/*!
 * \enum HAL_MDP_BlendStageId
 *
 * The \b HAL_MDP_BlendStageId is the blend stage of layer mixer
 *
 * HAL_MDP_##version##_LAYER_MIXER_MAX_BLEND_STAGES provides the maximum blend stages present the layer mixer module
 *
 * Each layer mixer performs the layer composition using blend configuration.
 * The each blend stage accepts one back-ground layer and one fore-ground layer with various blend settings
 *
 * The maximum layers possible for composition in layer mixer = (maximum blend stages + 1)
 */
typedef enum _HAL_MDP_BlendStageId
{
   HAL_MDP_BLEND_STAGE_0            = 0x00000000,  /**< Blend Stage #0 (One base layer + one fore-ground layer)          */
   HAL_MDP_BLEND_STAGE_1,                          /**< Blend Stage #1 (Output of blend stage#0 + one fore-ground layer) */
   HAL_MDP_BLEND_STAGE_2,                          /**< Blend Stage #2 (Output of blend stage#1 + one fore-ground layer) */
   HAL_MDP_BLEND_STAGE_3,                          /**< Blend Stage #3 (Output of blend stage#2 + one fore-ground layer) */
   HAL_MDP_BLEND_STAGE_4,                          /**< Blend Stage #4 (Output of blend stage#3 + one fore-ground layer) */
   HAL_MDP_BLEND_STAGE_5,                          /**< Blend Stage #5 (Output of blend stage#4 + one fore-ground layer) */
   HAL_MDP_BLEND_STAGE_6,                          /**< Blend Stage #6 (Output of blend stage#5 + one fore-ground layer) */
   HAL_MDP_BLEND_STAGE_CURSOR,
   HAL_MDP_BLEND_STAGE_MAX,
   HAL_MDP_BLEND_STAGE_FORCE_32BIT = 0x7FFFFFFF

} HAL_MDP_BlendStageId;

/*!
 * \enum HAL_MDP_BlendOrderId
 *
 * The \b HAL_MDP_BlendOrderId lists Z-ORDER for layer mixer blend
 *
 * The maximum Z-ORDER supported per mixer is The maximum layers possible for composition in layer mixer
 *
 */
typedef enum _HAL_MDP_BlendOrderId
{
   HAL_MDP_BLEND_ORDER_NONE                  = 0x00000000,
   HAL_MDP_BLEND_ORDER_STAGE_0_BASE_LAYER,      /**< Z-ORDER 0 (or) Z_ORDER BOTTOM */
   HAL_MDP_BLEND_ORDER_STAGE_0_FG_LAYER,        /**< Z-ORDER 1                     */
   HAL_MDP_BLEND_ORDER_STAGE_1_FG_LAYER,        /**< Z-ORDER 2                     */
   HAL_MDP_BLEND_ORDER_STAGE_2_FG_LAYER,        /**< Z-ORDER 3                     */
   HAL_MDP_BLEND_ORDER_STAGE_3_FG_LAYER,        /**< Z-ORDER 4                     */
   HAL_MDP_BLEND_ORDER_STAGE_4_FG_LAYER,        /**< Z-ORDER 5                     */
   HAL_MDP_BLEND_ORDER_STAGE_5_FG_LAYER,        /**< Z-ORDER 6                     */
   HAL_MDP_BLEND_ORDER_STAGE_6_FG_LAYER,        /**< Z-ORDER 7                     */
   HAL_MDP_BLEND_ORDER_STAGE_MAX,
   HAL_MDP_BLEND_ORDER_STAGE_FORCE_32BIT     = 0x7FFFFFFF

} HAL_MDP_BlendOrderId;


/*!
 * \enum HAL_MDP_SharpeningModeType
 *
 * Sharpening modes for performing the smoothening/sharpening operations
 *
 */
typedef enum _HAL_MDP_SharpeningModeType
{
  HAL_MDP_SHARPENING_TYPE_NONE          = 0x00000000,     /**< NONE                */
  HAL_MDP_SHARPENING_TYPE_SHARPEN,                        /**< SHARPEN             */
  HAL_MDP_SHARPENING_TYPE_SMOOTHEN,                       /**< SMOOTHEN            */
  HAL_MDP_SHARPENING_TYPE_MAX,                            
  HAL_MDP_SHARPENING_TYPE_FORCE_32BIT   = 0x7FFFFFFF
} HAL_MDP_SharpeningModeType;


/*!
 * \enum HAL_MDP_CursorBlendType
 *
 * The cursor blend operation takes place at the layer-mixer block. The cursor always the top most layer in the blend operation
 * The cursor supports various blend operation with the output image at the mixer operation.
 *
 */
typedef enum _HAL_MDP_CursorBlendType
{
   HAL_MDP_CURSOR_BLEND_TYPE_NONE            = 0x00000000,
   HAL_MDP_CURSOR_BLEND_TYPE_CONSTANT_ALPHA  = HAL_MDP_CURSOR_BLEND_TYPE_NONE, /**< Constant Alpha */
   HAL_MDP_CURSOR_BLEND_TYPE_PERPIXEL_ALPHA,   /**< Perpixel Alpha either from ARGB32 BIT SOURCE or Use Non-Native Alpha */
   HAL_MDP_CURSOR_BLEND_TYPE_MAX,
   HAL_MDP_CURSOR_BLEND_TYPE_FORCE_32BIT     = 0x7FFFFFFF

} HAL_MDP_CursorBlendType;

/*!
 * \enum HAL_MDP_Mixer_AlphaSrcType
 *
 * The layer mixer blend has multiple blend stages. Each blend stage accepts two layers.
 * The \b HAL_MDP_Mixer_AlphaSrcType provides to select the Alpha value for each layers blend operation.
 *
 */
typedef enum _HAL_MDP_Mixer_AlphaSrcType
{
   HAL_MDP_ALPHA_SRC_NONE              = 0x00000000,
   HAL_MDP_ALPHA_SRC_FG_CONST          = HAL_MDP_ALPHA_SRC_NONE, /**< Constant Alpha programmed in FG_CONST */
   HAL_MDP_ALPHA_SRC_BG_CONST,                                   /**< Constant Alpha programmed in BG_CONST */
   HAL_MDP_ALPHA_SRC_FG_PER_PIXEL,                               /**< FG Per Pixel Alpha                    */
   HAL_MDP_ALPHA_SRC_BG_PER_PIXEL,                               /**< BG Per Pixel Alpha                    */
   HAL_MDP_ALPHA_SRC_MAX,
   HAL_MDP_ALPHA_SRC_FORCE_32BIT       = 0x7FFFFFFF

} HAL_MDP_Mixer_AlphaSrcType;

/*!
 * \enum HAL_MDP_InterfaceConnectType
 *
 * The \b HAL_MDP_InterfaceConnectType lists types of interface supported by MPD
 *
 * In general the interface is either physical connected display i.e. DSI, LCDC, HDMI etc
 * or write-back path to memory i.e. Wireless Display and Blit mode and Rotator
 *
 */
typedef enum _HAL_MDP_InterfaceConnectType
{
   HAL_MDP_INTERFACE_TYPE_NONE               = 0x00000000,
   HAL_MDP_INTERFACE_TYPE_PHYSICAL_CONNECT,     /**< PHYSICAL INTERFACE TYPE - DSI, HDMI, etc             */
   HAL_MDP_INTERFACE_TYPE_WRITE_BACK,           /**< WRITE-BACK TYPE - BLT, ROTATOR, WIRELESS DISPLAY etc */
   HAL_MDP_INTERFACE_TYPE_MAX,
   HAL_MDP_INTERFACE_TYPE_FORCE_32BIT        = 0x7FFFFFFF

} HAL_MDP_InterfaceConnectType;

/*!
 * \enum HAL_MDP_InterfaceModeType
 *
 * The \b HAL_MDP_InterfaceModeType provides the list of different kind of physical interfaces
 *
 * The system supports 2 kind of interface i.e Physical and WriteBack (refer \b HAL_MDP_InterfaceConnectType)
 * The physical interface connect type devices can be active refresh panel or smart panel
 * 
 * The \b HAL_MDP_InterfaceModeType helps to configure the MDP for the mode of the of the physical interface
 */
typedef enum _HAL_MDP_InterfaceModeType
{
   HAL_MDP_INTERFACE_MODE_VIDEO              = 0x00000000,  /**< Active Refresh Panels like LCDC, HDMI etc  */
   HAL_MDP_INTERFACE_MODE_COMMAND,                          /**< Smart Panels with internal memory          */
   HAL_MDP_INTERFACE_MODE_NONE,
   HAL_MDP_INTERFACE_MODE_MAX,
   HAL_MDP_INTERFACE_MODE_FORCE_32BIT        = 0x7FFFFFFF

} HAL_MDP_InterfaceModeType;

/*!
 * \enum HAL_MDP_InterfaceStandardType
 *
 * The physical interface supports various data transfer standards like LCDC, eDP, DSI Video/command, HDMI etc.
 * The \b HAL_MDP_InterfaceStandardType lists the various types of display standards supported by MDP HW
 */
typedef enum _HAL_MDP_InterfaceStandardType
{
   HAL_MDP_INTERFACE_STANDARD_NONE           = 0x00000000,     /**< NONE                                          */
   HAL_MDP_INTERFACE_STANDARD_DSI            = 0x00000001,     /**< Display Serial Interface                      */
   HAL_MDP_INTERFACE_STANDARD_HDMI           = 0x00000003,     /**< High Definition Multimedia Interface          */
   HAL_MDP_INTERFACE_STANDARD_LCDC           = 0x00000005,     /**< Generic LCDC Interface                        */
   HAL_MDP_INTERFACE_STANDARD_eDP            = 0x00000009,     /**< Embedded Display Port                         */
   HAL_MDP_INTERFACE_STANDARD_DP             = 0x00000009,     /**< Display Port                                  */
   HAL_MDP_INTERFACE_STANDARD_LVDS,                            /**< LVDS                                          */
   HAL_MDP_INTERFACE_STANDARD_MAX,
   HAL_MDP_INTERFACE_STANDARD_FORCE_32BIT    = 0x7FFFFFFF

} HAL_MDP_InterfaceStandardType;

/*!
 * \enum HAL_MDP_InterfaceConnectionType
 *
 * The \b HAL_MDP_InterfaceConnectionType lists the various display interfaces supported by MDP HW
 */
typedef enum _HAL_MDP_InterfaceConnectionType
{
   HAL_MDP_INTERFACE_CONNECTION_NONE                    = 0x00000000,     /**< NONE                                          */
   HAL_MDP_INTERFACE_CONNECTION_DSI0,                                     /**< Display Serial Interface - 0                  */
   HAL_MDP_INTERFACE_CONNECTION_DSI1,                                     /**< Display Serial Interface - 1                  */
   HAL_MDP_INTERFACE_CONNECTION_HDMI,                                     /**< High Definition Multimedia Interface          */
   HAL_MDP_INTERFACE_CONNECTION_eDP,                                      /**< Embedded Display Port                         */
   HAL_MDP_INTERFACE_CONNECTION_LVDS,                                     /**< LVDS                                          */
   HAL_MDP_INTERFACE_CONNECTION_FB_LINEAR_MODE_WRITEBACK,                 /**< Reserved. Linear WB interface                 */
   HAL_MDP_INTERFACE_CONNECTION_FB_LINE_MODE_WRITEBACK,                   /**< Reserved. Line mode WB interface              */
   HAL_MDP_INTERFACE_CONNECTION_FB_BLK_MODE_WRITEBACK,                    /**< Reserved. Block mode WB interface             */
   HAL_MDP_INTERFACE_CONNECTION_DP,                                       /**< Display Port                                  */
   HAL_MDP_INTERFACE_CONNECTION_MAX,
   HAL_MDP_INTERFACE_CONNECTION_FORCE_32BIT             = 0x7FFFFFFF

} HAL_MDP_InterfaceConnectionType;

/*!
 * \enum HAL_MDP_3DMUXPackerModeType
 *
 * MDP is capable of packing the left and right frames in various formats. The \b HAL_MDP_3DMUXPackerModeType
 * lists the MDP supported packing formats
 */
typedef enum _HAL_MDP_3DMUXPackerModeType
{
   HAL_MDP_3DMUX_PACK_MODE_FRAME_INTERLEAVE     = 0x00000000,           /**< Frame interleaving                                 */
   HAL_MDP_3DMUX_PACK_MODE_HORZ_ROW_INTERLEAVE,                         /**< Horizontal row interleave mode (or) Dual pipe mode */
   HAL_MDP_3DMUX_PACK_MODE_VERT_ROW_INTERLEAVE,                         /**< Vertical row interleave mode                       */
   HAL_MDP_3DMUX_PACK_MODE_COLUMN_INTERLEAVE,                           /**< Column interleave mode                             */
   HAL_MDP_3DMUX_PACK_MODE_QUINCUNX_INTERLEAVE,                         /**< Quincunx interleave mode                           */
   HAL_MDP_3DMUX_PACK_MODE_MAX,
   HAL_MDP_3DMUX_PACK_MODE_FORCE_32BIT          = 0x7FFFFFFF

} HAL_MDP_3DMUXPackerModeType;

/*! 
 * \enum HAL_MDP_DataPath_RotateType
 *
 * HAL_MDP_DataPath_RotateType selects the rotate angle in write-back mode
 */
typedef enum _HAL_MDP_DataPath_RotateType
{
   HAL_MDP_ROTATE_NONE            = 0x00000000, /**< Rotator enabled with NO ROTATE Operation         */
   HAL_MDP_ROTATE_90,                           /**< Rotator enabled with 90-DEGREE ROTATE Operation  */
   HAL_MDP_ROTATE_MAX,
   HAL_MDP_ROTATE_FORCE_32BIT     = 0x7FFFFFFF

} HAL_MDP_RotateType;

/*!
 * \enum HAL_ControlPath_Flush
 *
 * Control Path Flush triggers the HW with the new configuration settings applied to control and data path
 * 
 * In case of active refresh panels, after flush the new configurations will get applied in next VSYNC
 * In case of smart mode panels, new configurations will get applied immediately after performing flush operation
 * 
 * The enumeration is prepared based on HW register fields. (Required special attention for new HW versions)
 */
typedef enum _HAL_ControlPath_Flush
{
   HAL_CONTROL_PATH_FLUSH_NONE         = 0x00000000,
   HAL_CONTROL_PATH_FLUSH_VIG0         = HAL_MDSS_BIT(0),   /**< VIG 0 FLUSH                 */
   HAL_CONTROL_PATH_FLUSH_VIG1         = HAL_MDSS_BIT(1),   /**< VIG 1 FLUSH                 */
   HAL_CONTROL_PATH_FLUSH_VIG2         = HAL_MDSS_BIT(2),   /**< VIG 2 FLUSH                 */

   HAL_CONTROL_PATH_FLUSH_RGB0         = HAL_MDSS_BIT(3),   /**< RGB 0 FLUSH                 */
   HAL_CONTROL_PATH_FLUSH_RGB1         = HAL_MDSS_BIT(4),   /**< RGB 1 FLUSH                 */
   HAL_CONTROL_PATH_FLUSH_RGB2         = HAL_MDSS_BIT(5),   /**< RGB 2 FLUSH                 */

   HAL_CONTROL_PATH_FLUSH_LM0          = HAL_MDSS_BIT(6),   /**< LAYER MIXER 0 FLUSH         */
   HAL_CONTROL_PATH_FLUSH_LM1          = HAL_MDSS_BIT(7),   /**< LAYER MIXER 1 FLUSH         */
   HAL_CONTROL_PATH_FLUSH_LM2          = HAL_MDSS_BIT(8),   /**< LAYER MIXER 2 FLUSH         */
   HAL_CONTROL_PATH_FLUSH_LM3          = HAL_MDSS_BIT(9),   /**< LAYER MIXER 3 FLUSH         */
   HAL_CONTROL_PATH_FLUSH_LM4          = HAL_MDSS_BIT(10),  /**< LAYER MIXER 4 FLUSH         */

   HAL_CONTROL_PATH_FLUSH_DMA0         = HAL_MDSS_BIT(11),  /**< DMA 0 FLUSH                 */
   HAL_CONTROL_PATH_FLUSH_DMA1         = HAL_MDSS_BIT(12),  /**< DMA 1 FLUSH                 */

   HAL_CONTROL_PATH_FLUSH_DSPP0        = HAL_MDSS_BIT(13),  /**< DSPP 0 FLUSH                */
   HAL_CONTROL_PATH_FLUSH_DSPP1        = HAL_MDSS_BIT(14),  /**< DSPP 1 FLUSH                */
   HAL_CONTROL_PATH_FLUSH_DSPP2        = HAL_MDSS_BIT(15),  /**< DSPP 2 FLUSH                */

   HAL_CONTROL_PATH_FLUSH_WB           = HAL_MDSS_BIT(16),  /**< WB 0,1,2,3 0R 4 FLUSH       */
   HAL_CONTROL_PATH_ATTACH_DETACH      = HAL_MDSS_BIT(17),  /**< MIXER INTERNAL LOGIC FLUSH  */

   HAL_CONTROL_PATH_FLUSH_VIG3         = HAL_MDSS_BIT(18),  /**< VIG 3 FLUSH                 */
   HAL_CONTROL_PATH_FLUSH_RGB3         = HAL_MDSS_BIT(19),  /**< RGB 3 FLUSH                 */
   HAL_CONTROL_PATH_FLUSH_LM5          = HAL_MDSS_BIT(20),  /**< LAYER MIXER 5 FLUSH         */
   HAL_CONTROL_PATH_FLUSH_DSPP3        = HAL_MDSS_BIT(21),  /**< DSPP 3 FLUSH                */

   HAL_CONTROL_PATH_FLUSH_CURSOR0      = HAL_MDSS_BIT(22),  /**< CURSOR 0 FLUSH              */
   HAL_CONTROL_PATH_FLUSH_CURSOR1      = HAL_MDSS_BIT(23),  /**< CURSOR 1 FLUSH              */
   HAL_CONTROL_PATH_FLUSH_DMA2         = HAL_MDSS_BIT(24),  /**< DMA 2 FLUSH                 */
   HAL_CONTROL_PATH_FLUSH_DMA3         = HAL_MDSS_BIT(25),  /**< DMA 3 FLUSH                 */
   HAL_CONTROL_PATH_FLUSH_CDM0         = HAL_MDSS_BIT(26),  /**< CHROMA DOWN BLOCK FLUSH     */
   HAL_CONTROL_PATH_FLUSH_TIMING_3     = HAL_MDSS_BIT(28),  /**< TIMING-ENGINE-0 FLUSH       */
   HAL_CONTROL_PATH_FLUSH_TIMING_2     = HAL_MDSS_BIT(29),  /**< TIMING-ENGINE-0 FLUSH       */
   HAL_CONTROL_PATH_FLUSH_TIMING_1     = HAL_MDSS_BIT(30),  /**< TIMING-ENGINE-0 FLUSH       */
   HAL_CONTROL_PATH_FLUSH_TIMING_0     = HAL_MDSS_BIT(31),  /**< TIMING-ENGINE-0 FLUSH       */
   HAL_CONTROL_PATH_FORCE_32BIT        = 0x7FFFFFFF

} HAL_ControlPath_Flush;

/*!
 * \enum HAL_MDP_SignalPolarityType
 *
 * Signal polarity type
 */
typedef enum _HAL_MDP_SignalPolarityType
{
   HAL_MDP_SIGNAL_POLARITY_LOW         = 0x00000000,     /**< Signal polarity LOW   */
   HAL_MDP_SIGNAL_POLARITY_HIGH,                         /**< Signal polarity HIGH  */
   HAL_MDP_SIGNAL_POLARITY_MAX,
   HAL_MDP_SIGNAL_POLARITY_FORCE_32BIT = 0x7FFFFFFF

} HAL_MDP_SignalPolarityType;

/*!
 * \enum HAL_MDP_CompressionModeType
 *
 * The \b HAL_MDP_CompressionModeType provides the list of different compression modes
 * of the bandwidth compression en/decoder
 *
 * Note: this is not a direct translation to the hardware register value
 *
 */
typedef enum _HAL_MDP_CompressionModeType
{
   HAL_MDP_COMPRESSION_MODE_NONE             = 0x00000000,  /**< No compression                */
   HAL_MDP_COMPRESSION_MODE_LOSSLESS,                       /**< Lossless compression          */
   HAL_MDP_COMPRESSION_MODE_MAX,
   HAL_MDP_COMPRESSION_MODE_FORCE_32BIT      = 0x7FFFFFFF

} HAL_MDP_CompressionModeType;

/*!
 * \enum HAL_MDP_DataPath_ModeType (Reserved. Not to be used)
 *
 * The \b HAL_MDP_DataPath_ModeType provides the list of different kinds write back modes supported by MDP
 *
 * BLOCK MODE Operation    -- Supports only one input layer. Processing power is 4 pixel/clk. (Used for ROTATOR)
 * LINE MODE Operation     -- Supports layer mixing and writes to memory (Used for BLT engine functionality)
 * LINEAR MODE Operation   -- Supports layer mixing + Panel Color calibration and writes to memory (Used for Wireless Display)
 * 
 */
typedef enum _HAL_MDP_DataPath_ModeType
{
   HAL_MDP_DATA_PATH_MODE_NONE          = 0x0000000,
   HAL_MDP_DATA_PATH_BLOCK_MODE,         /**< BLOCK = ROTATOR. Doesn't support layer mixing                            */
   HAL_MDP_DATA_PATH_LINE_MODE,          /**< LINE  = Layer Mixing and Write Back  (BLT Engine)                        */
   HAL_MDP_DATA_PATH_LINEAR_MODE,        /**< LINEAR = Wireless Display BLT Engine (Layer Mixing + Panel calibration   */
   HAL_MDP_DATA_PATH_TYPE_MAX,
   HAL_MDP_DATA_PATH_TYPE_FORCE_32BIT   = 0x7FFFFFFF

} HAL_MDP_DataPath_ModeType;

typedef enum _HAL_MDP_TPG_Type
{
   HAL_MDP_TPG_TYPE_NONE                           = 0x00000000,
   HAL_MDP_TPG_TYPE_COUNTER_PATTERN,
   HAL_MDP_TPG_TYPE_FIXED_PATTERN,
   HAL_MDP_TPG_TYPE_COLOR_RAMP_64L_64P,
   HAL_MDP_TPG_TYPE_COLOR_RAMP_64L_256P,
   HAL_MDP_TPG_TYPE_BLACK_WHITE_VERTICAL_LINES,
   HAL_MDP_TPG_TYPE_GRAYSCALE_RAMP,
   HAL_MDP_TPG_TYPE_COLOR_SQUARE,
   HAL_MDP_TPG_TYPE_CHECKERED_RECTANGLE_PATTERN,
   HAL_MDP_TPG_TYPE_BASIC_COLOR_CHANGING_PATTERN,
   HAL_MDP_TPG_TYPE_MAX,
   HAL_MDP_TPG_TYPE_FORCE_32BIT                    = 0x7FFFFFFF

} HAL_MDP_TPG_Type;

/*!
 * \enum HAL_MDP_PingPong_VsyncSelectType
 *
 * The \b HAL_MDP_PingPong_VsyncSelectType provides the list of external vsync sources
 * 
 */
typedef enum _HAL_MDP_PingPong_VsyncSelectType
{
    HAL_MDP_PINGPONG_VSYNC_SELECT_TYPE_VSYNC_0_GPIO = 0x00000000,         /**< vsync 0 from gpio */
    HAL_MDP_PINGPONG_VSYNC_SELECT_TYPE_VSYNC_1_GPIO = 0x00000001,         /**< vsync 1 from gpio */
    HAL_MDP_PINGPONG_VSYNC_SELECT_TYPE_VSYNC_2_GPIO = 0x00000002,         /**< vsync 2 from gpio */
    HAL_MDP_PINGPONG_VSYNC_SELECT_TYPE_TE_INTF_0    = 0x00000003,         /**< vsync(TE) from interface 0 panel */
    HAL_MDP_PINGPONG_VSYNC_SELECT_TYPE_TE_INTF_1    = 0x00000004,         /**< vsync(TE) from interface 1 panel */
    HAL_MDP_PINGPONG_VSYNC_SELECT_TYPE_TE_INTF_2    = 0x00000005,         /**< vsync(TE) from interface 2 panel */
    HAL_MDP_PINGPONG_VSYNC_SELECT_TYPE_TE_INTF_3    = 0x00000006,         /**< vsync(TE) from interface 3 panel */
    HAL_MDP_PINGPONG_VSYNC_SELECT_TYPE_MAX,
    HAL_MDP_PINGPONG_VSYNC_SELECT_TYPE_FORCE_32BIT  = 0x7FFFFFFF
} HAL_MDP_PingPong_VsyncSelectType;

/*!
 * \enum HAL_MDP_ChromaSubsamplingType
 *
 * The \b HAL_MDP_ChromaSubsamplingType provides Chroma Subsampling Info
 * 
 */
typedef enum _HAL_MDP_ChromaSubsamplingType
{
    HAL_MDP_CHROMA_SUBSAMPLING_TYPE_NONE           = 0x00000000,         /**< No Subsampling (=0) */
    HAL_MDP_CHROMA_SUBSAMPLING_TYPE_ONE            = 0x00000001,         /**< Subsampling by 1*/
    HAL_MDP_CHROMA_SUBSAMPLING_TYPE_MAX,
    HAL_MDP_CHROMA_SUBSAMPLING_TYPE_FORCE_32BIT    = 0x7FFFFFFF
} HAL_MDP_ChromaSubsamplingType;


/*!
 * \enum HAL_MDP_QSEED2_ColorComponentType
 *
 * The \b HAL_MDP_QSEED2_ColorComponentType provides the color component
 * 
 */
typedef enum _HAL_MDP_QSEED2_ColorComponentType
{
    HAL_MDP_QSEED2_COLOR_COMPONENT_TYPE_0_3             = 0x00000000,         /**< color component 0 and 3 */
    HAL_MDP_QSEED2_COLOR_COMPONENT_TYPE_1_2             = 0x00000001,         /**< color component 1 and 2 */
    HAL_MDP_QSEED2_COLOR_COMPONENT_TYPE_MAX,
    HAL_MDP_QSEED2_COLOR_COMPONENT_TYPE_FORCE_32BIT     = 0x7FFFFFFF
} HAL_MDP_QSEED2_ColorComponentType;

/*!
 * \enum _HAL_MDP_ChromaDownSampleMethodType 
 *
 * The \b HAL_MDP_ChromaDownSampleMethodType sets Chroma Down sample Method Algorithm Quality 
 * CDM block supports pixel drop, average, Co-site and Off-site downscaling methods 
 * in horizontal and vertical directions.
 * 
 *     For converting to 420 formats recommended methods are Cosite down-sample method in horizontal and Off-site in vertical direction 
 *     For 422 formats recommended down-sample method in horizontal is Co-site and no down-sampling in vertical direction.
 * WB0/1 line mode only supports pixel drop method for down sampling
 * 
 */
typedef enum _HAL_MDP_ChromaDownSampleMethodType 
{
   HAL_MDP_CHROMA_DOWNSAMPLE_METHOD_TYPE_PIXELDROP  = 0X00000000,      /* Drops the odd pixel/line         */
   HAL_MDP_CHROMA_DOWNSAMPLE_METHOD_TYPE_AVERAGE    = 0X00000001,      /* Average of two pixels/lines      */
   HAL_MDP_CHROMA_DOWNSAMPLE_METHOD_TYPE_COSITE     = 0X00000002,      /* Co-site Filtering, Horizontally uses 11 taps FIR filtering and 3 tap FIR filtering vertically  */
   HAL_MDP_CHROMA_DOWNSAMPLE_METHOD_TYPE_OFFSITE    = 0X00000003,      /* Off-site filtering, Horizontally uses 12 taps FIR filtering and 4 tap FIR filtering vertically */
   HAL_MDP_CHROMA_DOWNSAMPLE_METHOD_TYPE_MAX,
   HAL_MDP_CHROMA_DOWNSAMPLE_METHOD_TYPE_FORCE_32BIT = 0X7FFFFFFF
}HAL_MDP_ChromaDownSampleMethodType;

/*!
 * \enum _HAL_MDP_3DFrameFormatType 
 *
 * The \b HAL_MDP_3DFrameFormatType provides Chroma Down sample Method Information
 * 
 */
typedef enum _HAL_MDP_3DFrameFormatType  
{
   HAL_MDP_3D_FORMAT_TYPE_NONE         = 0X00000000,         /**< No 3D format          */
   HAL_MDP_3D_FORMAT_TYPE_SBS          = 0X00000001,         /** 3D format side by side */
   HAL_MDP_3D_FORMAT_TYPE_TB           = 0X00000002,         /** 3D format top bottom   */
   HAL_MDP_3D_FORMAT_TYPE_FRAMEPACKED  = 0X00000003,         /** 3D format frame packed */
   HAL_MDP_3D_FORMAT_TYPE_MAX,
   HAL_MDP_3D_FORMAT_TYPE_FORCE_32BIT  = 0X7FFFFFFF
}HAL_MDP_3DFrameFormatType;

/*!
 * \enum HAL_MDP_GCLUT_TYPE
 *
 * The \b HAL_MDP_GCLUT_TYPE provides the GC LUT implementation type in different HW versions
 *
 */
typedef enum _HAL_MDP_GCLUT_TYPE
{
    HAL_MDP_GCLUT_AREA_REDUCED_TYPE  = 0x0000000,        /**< Area Reduced Gamma correction LUT */
    HAL_MDP_GCLUT_LINEAR_TYPE        = 0x00000001,       /**< Linear Gamma correction LUT       */
    HAL_MDP_GCLUT_TYPE_FORCE_32BIT   = 0x7FFFFFFF
} HAL_MDP_GCLUT_TYPE;


/*!
 * \enum HAL_MDP_GAMUT_TYPE
 *
 * The \b HAL_MDP_GAMUT_TYPE provides the Gamut color width in different HW versions
 *
 */
typedef enum _HAL_MDP_GAMUT_TYPE
{
    HAL_MDP_GAMUT_8BITGAMUT_TYPE   = 0x00000000,       /**< Gamut table for each color is 8 bits                 */
    HAL_MDP_GAMUT_WIDEGAMUT_TYPE   = 0x00000001,       /**< Gamut table for each color component is 16 bits wide */
    HAL_MDP_GAMUT_TYPE_FORCE_32BIT = 0x7FFFFFFF
} HAL_MDP_GAMUT_TYPE;

/*!
 * \enum HAL_MDP_PCC_TYPE
 *
 * The \b HAL_MDP_PCC_TYPE provides the list of Polynomial Color Correction (PCC) configurations
 * present in the hardware
 *
 */
typedef enum _HAL_MDP_PCC_TYPE
{
    HAL_MDP_PCC_TYPE1            = 0x00000000,       /**< PCC TYPE 1 */
    HAL_MDP_PCC_TYPE2            = 0x00000001,       /**< PCC TYPE 2 */
    HAL_MDP_PCC_TYPE_FORCE_32BIT = 0x7FFFFFFF
} HAL_MDP_PCC_TYPE;

/*!
 * \enum HAL_MDP_CompressionRatioType
 *
 * The \b HAL_MDP_CompressionRatioType provides the list to select a particular compression
 * type for compression engine module
 *
 */
typedef enum _HAL_MDP_CompressionRatioType
{
   HAL_MDP_COMPRESSION_RATIO_NONE          = 0x00000000,
   HAL_MDP_COMPRESSION_RATIO_1_TO_1        = 0x00000001,    /**< 8 BITS-PER-COMPONENT TO 24 BITS-PER-PIXELS No Compression (1:1 Ratio) */
   HAL_MDP_COMPRESSION_RATIO_2_TO_1        = 0x00000002,    /**< 8 BITS-PER-COMPONENT TO 12 BITS-PER-PIXELS (2:1 Ratio) */
   HAL_MDP_COMPRESSION_RATIO_3_TO_1        = 0x00000003,    /**< 8 BITS-PER-COMPONENT TO 8 BITS-PER-PIXELS (3:1 Ratio)  */
   
   HAL_MDP_COMPRESSION_RATIO_TYPE_MAX,
   HAL_MDP_COMPRESSION_RATIO_FORCE_32BIT   = 0x7FFFFFFF

} HAL_MDP_CompressionRatioType;

/*!
* \b HAL_MDP_ScaleStepMethodType
*
*  enum defines the sequence type for scale capabilities.
*  
*/
typedef enum
{
  /**< Continuous means that parameter can take every values from min to max  */
  HAL_MDP_SCALE_STEP_CONTINUOUS,
  /**< Parameter can take every integer values from min to max with stepSize(power(stepNumber)), 
    if stepSize =2 and min value = 2, then parameter can be minValue*step_size(power(set_Number)):
    firstElement=2*2(power 0) = 2,second = 2*2(power 1) = 4,third = 2*2(power 2) =8 ..(step_Number starts from 0).   */
  HAL_MDP_SCALE_STEP_LOGARITHMIC,
  HAL_MDP_SCALE_STEP_MAX,
  HAL_MDP_SCALE_STEP_FORCE_32BIT    = 0x7FFFFFFF
} HAL_MDP_ScaleStepMethodType;

/*!
* \enum HAL_MDP_DitherType
*
* The \b HAL_MDP_DitherType provides the list of dither types 
* present in the hardware
*
*/
typedef enum _HAL_MDP_DitherType
{
    HAL_MDP_DITHER_TYPE_SPATIAL  = 0x00000000,       /**< Spatial dither */
    HAL_MDP_DITHER_TYPE_TEMPORAL = 0x00000001,       /**< Temporal dither */
    HAL_MDP_DITHER_TYPE_32BIT    = 0x7FFFFFFF
} HAL_MDP_DitherType;

/*!
* \b HAL_MDP_UTILITY_optimization_mode
*
*  enum defines power/quality optimization mode
*  
*/
typedef enum _HAL_MDP_UTILITY_optimization_mode
{
   HAL_MDP_QSEED_OPTIMIZATION_QUALITY,                 /**< High quality */
   HAL_MDP_QSEED_OPTIMIZATION_BALANCED,                /**< Balance quality and power */
   HAL_MDP_QSEED_OPTIMIZATION_POWER,                   /**< Low power */
   HAL_MDP_QSEED_OPTIMIZATION_MAX,
   HAL_MDP_QSEED_OPTIMIZATION_FORCE_32BIT   = 0x7FFFFFFF

}HAL_MDP_UTILITY_optimization_mode;


/*!
* \b HAL_MDP_UTILITY_qseed_lut_index
*
*  enum defines QSEED LUT index
*
*/
typedef enum 
{
    HAL_MDP_QSEED_LUT_INDEX_UNKNOWN         = 0,        /**< Index unknown */
    HAL_MDP_QSEED_LUT_INDEX_RANDOM,                     /**< Index specifies random generated LUT */
    HAL_MDP_QSEED_LUT_INDEX_0,
    HAL_MDP_QSEED_LUT_INDEX_1,
    HAL_MDP_QSEED_LUT_INDEX_2,
    HAL_MDP_QSEED_LUT_INDEX_3,
    HAL_MDP_QSEED_LUT_INDEX_4, 
    HAL_MDP_QSEED_LUT_INDEX_5,
    HAL_MDP_QSEED_LUT_INDEX_6,
    HAL_MDP_QSEED_LUT_INDEX_7,
    HAL_MDP_QSEED_LUT_INDEX_8, 
    HAL_MDP_QSEED_LUT_INDEX_9, 
    HAL_MDP_QSEED_LUT_INDEX_MAX,
    HAL_MDP_QSEED_LUT_INDEX_FORCE_32BIT     = 0x7FFFFFFF

} HAL_MDP_UTILITY_qseed_lut_index;

/*!
* \enum HAL_MDP_FetchPlanes
*
* The \b HAL_MDP_FetchPlanes lists number of planes to fetch in SSPP in MDP HW
* 
*/
typedef enum _HAL_MDP_FetchPlanes
{
   HAL_MDP_FETCH_PLANES_INTERLEAVED    = 0x00000000,
   HAL_MDP_FETCH_PLANES_PLANAR         = 0x00000001,
   HAL_MDP_FETCH_PLANES_PSEUDO_PLANAR  = 0x00000002,
} HAL_MDP_FetchPlanes;

/*
 * HAL_MDP_MultiRectModeType
 *
 *     Enumeration specifies HW supported multi-rectangle fetch modes
 */
typedef enum _HAL_MDP_MultiRectModeType
{
   HAL_MDP_MULTIRECT_PARALLEL_FETCH       = 0x00000000,
   HAL_MDP_MULTIRECT_TIME_MULTIPLEX_FETCH,
   HAL_MDP_MULTIRECT_MODE_MAX,
   HAL_MDP_MULTIRECT_MODE_FORCE_32BIT     = 0x7FFFFFFF
} HAL_MDP_MultiRectModeType;


/*!
 * \enum HAL_MDP_CommitModeType
 *
 * The HAL_MDP_CommitModeType lists the different commit mode configuration
 */
typedef enum _HAL_MDP_CommitModeType
{
   HAL_MDP_COMMITMODE_NONE                 = 0x00000000, /**< Default harware commit mode */
   HAL_MDP_COMMITMODE_SINGLE,                            /**< Single commit mode, committing on master will commit slave as well */
   HAL_MDP_COMMITMODE_MAX,
   HAL_MDP_COMMITMODE_FORCE_32BIT          = 0x7FFFFFFF
} HAL_MDP_CommitModeType;



// ===============================================================================================================
/*  */
/*!
* \b HAL_MDP_ScalePropertyType
*
*  Defines data to calculate parameters set.
*  This structure specifies basic rules of some sequence of elements. 
*  Use this structure to calculate every element in this sequence.
*  
*/
typedef struct
{
  HAL_MDP_ScaleStepMethodType   eStepType;              /**< Sequence type for parameter set  */
  uint32                        uStepSize;              /**< Step size for set of parameters  */
  uint32                        uMinValue;              /**< Minimum value in the set  */
  uint32                        uMaxValue;              /**< Maximum value in the set  */
} HAL_MDP_ScalePropertyType;


/*  */
/*!
* \b QDI_ScalingCapsType
*
*  Describes scalar capabilities.
*  
*/
typedef struct
{
  bool32                        bDownscaleSupported;    /**< TRUE if downscaling is supported  */
  bool32                        bUpscaleSupported;      /**< TRUE if upscaling is supported  */
  HAL_MDP_ScalePropertyType     sDownscaleRatio;        /**< Data to calculate downscale ratio */
  HAL_MDP_ScalePropertyType     sUpscaleRatio;          /**< Data to calculate upscale ratio */
}HAL_MDP_ScalingCapsType;


/*!
 * \union HAL_MDP_ModuleId
 * 
 * The \b HAL_MDP_ModuleId provides the way to select a particular HW block of HAL_MDP_ModuleType
 *
 * This data structure used to configure/read/clear the interrupts based on module type and module ID
 *
 */
typedef union _HAL_MDP_ModuleId
{
   HAL_MDP_SourcePipeId          eSourcePipeId;          /**< HAL_MDP_MODULE_SOURCE_PIPE           */
   HAL_MDP_LayerMixerId          eLayerMixerId;          /**< HAL_MDP_MODULE_LAYERMIXER            */
   HAL_MDP_DestinationPipeId     eDestinationPipeId;     /**< HAL_MDP_MODULE_DESTINATION_PIPE      */
   HAL_MDP_InterfaceId           eInterfaceId;           /**< HAL_MDP_MODULE_INTERFACE             */
   HAL_MDP_ControlPathId         eControlPathId;         /**< HAL_MDP_MODULE_CONTROL_PATH          */
   HAL_MDP_PingPongId            ePingPongId;            /**< HAL_MDP_MODULE_PINGPONG              */
   HAL_MDP_WatchDogId            eWatchDogId;            /**< HAL_MDP_MODULE_WATCHDOG              */
} HAL_MDP_ModuleId;


typedef struct _HAL_MDP_HwInfo
{
   HAL_HW_VersionType      sMdpHwVersionInfo;                           /**< MDP HW version information                       */
   uint32                  uNumOfRGBLayers;                             /**< Number of RGB Layers                             */
   uint32                  uNumOfVIGLayers;                             /**< Number of VIG Layers                             */
   uint32                  uNumOfDMALayers;                             /**< Number of DMA Layers                             */
   uint32                  uNumOfCursorLayers;                          /**< Number of Cursor Layers                          */
   uint32                  uNumOfLayerMixers;                           /**< Number of Layer Mixers                           */
   uint32                  uNumOfLayerMixerBlendStages;                 /**< Number of blend stages in layer mixer            */
   uint32                  uNumOfDSPPs;                                 /**< Number of Destination Surface Pixel Processors   */
   uint32                  uNumOfBltEngines;                            /**< Number of write-back engines                     */
   uint32                  uNumOfControlPaths;                          /**< Number of control paths                          */
   uint32                  uNumOfDataPaths;                             /**< Number of data paths                             */
   uint32                  uNumOfPhyInterfaces;                         /**< Number of physical interfaces                    */
   uint32                  uNumOfPingPongs;                             /**< Number of pingpong blocks                        */
   uint32                  uNumOfPingPongSplitBuffers;                  /**< Number of PPB blocks with split support          */
   uint32                  uNumOfSplitMuxers;                           /**< Number of Split Muxer blocks                             */
   uint32                  uNumOfWatchDogs;                             /**< Number of watch dogs                             */
   uint32                  uNumOfAxiPorts;                              /**< Number of AXI ports                              */
   uint32                  uNumOfVbifs;                                 /**< Number of VBIF blocks                            */
   uint32                  uNumOfVbifClients;                           /**< Number of VBIF clients                           */
   HAL_MDP_GCLUT_TYPE      eGCLUTType;                                  /**< GC LUT type                                      */
   HAL_MDP_GAMUT_TYPE      eGamutType;                                  /**< Gamut Type                                       */
   HAL_MDP_PCC_TYPE        ePCCType;                                    /**< PCC Type                                         */
   uint32                  uNumOfChromaDownBlocks;                      /**< Number of Chroma-down blocks                     */
   uint32                  uFeatureFlags;                               /**< Specific supported feature ex SFI                */
   uint32                  uInterfaceFlags;                             /**< Specific interfaces present in HW                */
} HAL_MDP_HwInfo;

/*
 */
typedef struct _HAL_MDP_InitConfigType
{
    uint32                 uGpuHighestMemoryBankBit;                    /**< GPU highest memory bank bit for DDR configuration */
    uint32                 uGpuMacroTileMode;                           /**< GPU macro tile mode: 0-A420 1-A430 */
} HAL_MDP_InitConfigType;

/*
 * HAL_MDP_AreaReductionLutType
 *
 */
typedef struct _HAL_MDP_AreaReductionLutType
{
   bool32   uGC_Enable; /**< Enable/Disable the current segment - supported in future. */
   uint32   uGC_Start;  /**< start values. Only lower 15-bits are valid.  */
   uint32   uGC_Slope;  /**< slope values. Only lower 15-bits are valid.  */
   uint32   uGC_Offset; /**< offset values. Only lower 15-bits are valid. */
} HAL_MDP_AreaReductionLutType;

/*!
 * \struct HAL_MDP_InterruptConfigType
 * 
 * The \b HAL_MDP_InterruptConfigType data structure enables/clears/reads the 
 * interrupts for the given module id of given module type
 *
 */
typedef struct _HAL_MDP_InterruptConfigType
{
   HAL_MDP_ModuleType         eModuleType;      /**< Type of the eModuleId              */
   HAL_MDP_ModuleId           eModuleId;        /**< Module identification              */
   HAL_MDP_InterfaceStreamId  eStreamId;        /**< Interface-stream identification    */
   HAL_MDP_InterruptType      eInterruptSrc;    /**< Interrupts type to configure/clear */

} HAL_MDP_InterruptConfigType;

/*!
 * \struct HAL_MDP_InterruptStatusType
 *
 * The \b HAL_MDP_InterruptStatusType data structures reads the interrupt status for the requested
 * interrupts of the given module identification of the given module type
 *
 */
typedef struct _HAL_MDP_InterruptStatusType
{
   HAL_MDP_InterruptConfigType   sIntrConfig;            /**< Interrupt details                                      */
   HAL_MDP_InterruptType         eInterruptStatus;       /**< Current status/configuration state of the interrupts   */
   uint32                        uInterruptModuleId;     /**< Module type with active interrupt                      */

} HAL_MDP_InterruptStatusType;


/*!
 * HAL_MDP_FifoWatermark_ConfigType
 *
 * Parameters to configure source pipe fifo watermark 
 *
 */
typedef struct
{
  uint32                             uTotalSMPBytes;  /**< Total SMP byte */
}HAL_MDP_FifoWatermark_ConfigType;


/*!
 * HAL_MDP_TraficSharper_ConfigType
 *
 * Control parameters for configuring Traffic shaper for read and write clients.
 *
 */
typedef struct
{
  bool32                             bEnableTrfShaper;    /* FALSE: traffic shaper disabled (pass through), TRUE: traffic shaper in control    */
  uint32                             uBytesPerClk;        /* Bytes per clock as required by traffic shaper                                     */
  uint32                             uMdpClkFreq;
}HAL_MDP_TraficSharper_ConfigType;

/*!
 * HAL_MDP_Qos_ConfigType
 *
 * Parameters for configuring source pipe QoS
 *
 */
typedef struct
{
  bool32                             bEnable;         /**< True if enable */
  HAL_MDP_PixelFormatType            ePixelFormat;    /**< Pixel format */
  uint32                             uBitsPerPixel;   /**< Bits per pixel */  
  uint32                             uSrcWidth;       /**< Source ROI width */
}HAL_MDP_Qos_ConfigType;


/*!
 * HAL_MDP_PerfCtrlParamsType
 *
 * Control parameters for configuring MDP performance settings
 *
 */
typedef struct 
{
   HAL_MDP_SourcePipeId               eSourcePipe;
   HAL_MDP_TrafficCtrl_ClientIDType   eClientId;           /* traffic control client ID                                                         */
   HAL_MDP_TrafficCtrl_LatencyType    eReadClientLatency;  /* tolerable latency, applied to read clients only;                                  */
                                                           /* for write clients WBx, this field is ignored, 
                                                              pass HAL_MDP_TRFCTRL_LATENCY_BEST_EFFORT for simplicity.                          */
   uint32                             uSrcSurfFlags;       /* Source surface flags for ex Marcotile, BWC */
   HAL_MDP_FifoWatermark_ConfigType  *psFifoWatermarkSetting;      /* Total number of bytes in SMP allocated for the client                             */
   HAL_MDP_TraficSharper_ConfigType  *psTrfShaperSetting;    /* FALSE: traffic shaper disabled (pass through), TRUE: traffic shaper in control    */
   HAL_MDP_Qos_ConfigType            *psQoSSetting;        /* QoS configuration setting  */
} HAL_MDP_PerfCtrlParamsType;

/*!
 * HAL_MDP_VBIFQOSRemappers
 *
 * VBIF QOS remapper parameters for read and write clients.
 *
 */
typedef struct 
{
   uint32             uVBIFQosRemapper00;            /*QOS remapper register settings for register 00       */
   uint32             uVBIFQosRemapper01;            /*QOS remapper register settings for register 01       */
   uint32             uVBIFQosRemapper10;            /*QOS remapper register settings for register 10       */
   uint32             uVBIFQosRemapper11;            /*QOS remapper register settings for register 11       */
} HAL_MDP_VBIFQOSRemappers;

/*!
 * HAL_MDP_VBIFQOSRemapperType
 *
 * VBIF QOS remapper parameters based on clients.
 *
 */
typedef struct 
{
   HAL_MDP_VBIF_ClientIDType        eClientId;           /* traffic control client ID  */   
   HAL_MDP_VBIFQOSRemappers        *pVBIFQOSRemappers;   /* VBIF QOS remappers */
} HAL_MDP_VBIFQOSRemapperType;

/*!
 *  \struct HAL_MDP_PixelPlaneType
 *
 *  The \b HAL_MDP_PixelPlaneType holds the buffer information for RGB interleaved pixel formats
 *
 *  The interleaved pixel formats has one plane and stride, line width in terms of bytes, information
 *
 */
typedef struct _HAL_MDP_PixelPlaneType
{
   HAL_MDSS_DeviceAddressType       sDeviceAddress;   /**< Device Accessible Address         */
   uint32                           uStrideInBytes;   /**< Stride of a scan line in bytes    */

} HAL_MDP_PixelPlaneType;

/*!
 *  \struct HAL_MDP_FormatYuvPackedPlaneType
 *
 *  The \b HAL_MDP_FormatYuvPackedPlaneType holds the buffer information for YUV interleaved pixel formats
 *
 * The YUV interleaved pixels formats are similar to RGB interleaved pixel formats with additional information
 * on chroma packing
 *
 */
typedef struct _HAL_MDP_FormatYuvPackedPlaneType
{
   HAL_MDSS_DeviceAddressType sDeviceAddress;         /**< Device Accessible Address                                    */
   uint32                     uStrideInBytes;         /**< Stride of a scan line in bytes                               */
   bool32                     bChromaOffSite;         /**< Source chroma siting (Not valid)                             */
   bool32                     bChromaVerticalPacked;  /**< CbCr packed direction. The buffer has rotated image content  */

} HAL_MDP_FormatYuvPackedPlaneType;

/*!
 *  \struct HAL_MDP_FormatYuvPsuedoPlanarPlaneType
 *
 *  The \b HAL_MDP_FormatYuvPsuedoPlanarPlaneType holds the buffer information for YUV pseudo planar pixel formats
 *
 */
typedef struct _HAL_MDP_FormatYuvPsuedoPlanarPlaneType
{
   HAL_MDSS_DeviceAddressType sLumaDeviceAddress;     /**< Device Accessible Address         */
   uint32                     uLumaStrideInBytes;     /**< Stride of a scan line in bytes    */
   HAL_MDSS_DeviceAddressType sChromaDeviceAddress;   /**< Device Accessible Address         */
   uint32                     uChromaStrideInBytes;   /**< Stride of a scan line in bytes    */
   bool32                     bChromaOffSite;         /**< Source chroma siting              */
   bool32                     bChromaVerticalPacked;  /**< CbCr packed direction             */

} HAL_MDP_FormatYuvPsuedoPlanarPlaneType;

/*!
 *  \struct HAL_MDP_FormatYuvPlanarPlaneType
 *
 *  The \b HAL_MDP_FormatYuvPlanarPlaneTypee holds the buffer information for YUV planar pixel formats
 *
 */
typedef struct _HAL_MDP_FormatYuvPlanarPlaneType
{
   HAL_MDSS_DeviceAddressType sLumaDeviceAddress;        /**< Device Accessible Address         */
   uint32                     uLumaStrideInBytes;        /**< Stride of a scan line in bytes    */
   HAL_MDSS_DeviceAddressType sChromaBlueDeviceAddress;  /**< Device Accessible Address         */
   uint32                     uChromaBlueStrideInBytes;  /**< Stride of a scan line in bytes    */
   HAL_MDSS_DeviceAddressType sChromaRedDeviceAddress;   /**< Device Accessible Address         */
   uint32                     uChromaRedStrideInBytes;   /**< Stride of a scan line in bytes    */
   bool32                     bChromaOffSite;            /**< Source chroma siting              */
   bool32                     bChromaVerticalPacked;     /**< CbCr packed direction             */

} HAL_MDP_FormatYuvPlanarPlaneType;

/*!
*  \struct HAL_MDP_FormatCompressedPlaneType
*
*  The \b HAL_MDP_FormatCompressedPlaneType holds the buffer information for the compressed planes
*
* BWC (Bandwidth compression) Note:
* BWC compressed surface contains 2 planes regardless of pixel format:
* - Compressed pixel data plane (sCompressedPlane0Address/uCompressedPlane0StrideInByte)
* - Meta data plane (sMetadataPlane0Address/uMetadataPlane0StrideInByte)
*
* UBWC (Universal bandwidth compression) Note:
* UBWC compression is applied for each plane.
* UBWC compressed pseudo planar surface contains:
* - Y-meta data plane (sMetadataPlane0Address/uMetadataPlane0StrideInByte)
* - Y-compressed pixel data plane (sCompressedPlane0Address/uCompressedPlane0StrideInByte)
* - UV-meta data plane (sMetadataPlane1Address,uMetadataPlane1StrideInByte)
* - UV-compressed pixel data plane (sCompressedPlane1Address/uCompressedPlane1StrideInByte)
* UBWC compressed interleaved surface contains:
* - Meta data plane (sMetadataPlane0Address/uMetadataPlane0StrideInByte)
* - Compressed pixel data plane (sCompressedPlane0Address/uCompressedPlane0StrideInByte)
* UBWC compressed planar surface is not supported.
*
* UBWC compression memory layout requirements:
* - Meta data/compressed pixel data planes should be allocated in virtually contiguous memory, compressed pixel data plane adjacent to meta data plane.
* - Meta data/compressed pixel data plane starting addresses has to be 4K aligned
* - Meta data/compressed pixel data plane size has to be 4K aligned.
 *
 */
typedef struct _HAL_MDP_FormatCompressedPlaneType
{
   HAL_MDSS_DeviceAddressType sCompressedDataPlane0DeviceAddress;  /**< Compressed pixel data plane starting address, Y-compressed pixel data plane address for UBWC compressed YUV pseudo planar format */
   uint32                     uCompressedDataPlane0StrideInBytes;  /**< Stride in bytes of compressed pixel data plane, Y-compressed pixel data plane stride for UBWC compressed YUV pseudo planar format */
   HAL_MDSS_DeviceAddressType sMetadataPlane0DeviceAddress;        /**< Meta data plane starting address, Y-meta data plane address for UBWC compressed YUV pseudo planar format */
   uint32                     uMetadataPlane0StrideInBytes;        /**< Stride in bytes of meta data plane, Y-meta data plane stride for UBWC compressed YUV pseudo planar format */   
   HAL_MDSS_DeviceAddressType sCompressedDataPlane1DeviceAddress;  /**< UV-compressed pixel data plane address for UBWC compressed YUV pseudo planar format */
   uint32                     uCompressedDataPlane1StrideInBytes;  /**< UV-compressed pixel data plane stride for UBWC compressed YUV pseudo planar format */
   HAL_MDSS_DeviceAddressType sMetadataPlane1DeviceAddress;        /**< UV-meta data plane address for UBWC compressed YUV pseudo planar format */
   uint32                     uMetadataPlane1StrideInBytes;        /**< UV-meta data plane stride for UBWC compressed YUV pseudo planar format */
   bool32                     bChromaVerticalPacked;               /**< CbCr packed direction             */

} HAL_MDP_FormatCompressedPlaneType;

/*!
 *  \union HAL_MDP_PlanesType
 *
 *  The \b HAL_MDP_PlanesType provides the buffer information based on the pixel format and planar type
 */
typedef union _HAL_MDP_PlanesType
{
   HAL_MDP_PixelPlaneType                           sRgb;                       /* INTERLEAVED RGB plane                                       */
   HAL_MDP_FormatYuvPackedPlaneType                 sYuvPacked;                 /* INTERLEAVED YUV packed plane                                */
   HAL_MDP_FormatYuvPsuedoPlanarPlaneType           sYuvPsuedoPlanar;           /* PSEUDO PLANAR YUV plane                                     */
   HAL_MDP_FormatYuvPlanarPlaneType                 sYuvPlanar;                 /* PLANAR YUV plane                                            */
   HAL_MDP_FormatCompressedPlaneType                sCompressedPlane;           /* Bandwidth compressed / universally bandwidth compressed plane */
} HAL_MDP_PlanesType;

/*!
 *  \struct HAL_MDP_SurfaceAttrType
 *
 *  The \b HAL_MDP_SurfaceAttrType provides all fields required to configure the buffer information to MDP HW
 */
typedef struct _HAL_MDP_SurfaceAttrType
{
   HAL_MDP_PixelFormatType        ePixelFormat;          /**< Pixel format        */
   uint32                         uWidthInPixel;         /**< Width in pixels     */
   uint32                         uHeightInPixel;        /**< Height in pixels    */
   HAL_MDP_PlanesType             uPlanes;               /**< Plane information   */
   uint32                         uFlags;                /**< Reserved            */

} HAL_MDP_SurfaceAttrType;

/*!
 * \struct HAL_MDP_ColorFillConfigType
 *
 * The source fetch engine supports either buffer fetch from memory or internal color fill.
 * The \b HAL_MDP_ColorFillConfigType data structure provides the necessary configuration information to enable/disable 
 * the color fill feature of the source fetch engine.
 */
typedef struct _HAL_MDP_ColorFillConfigType
{
   bool32                      bEnable;         /**< Control bit                                                */
   HAL_MDP_PixelFormatType     eFormat;         /**< Format used to represent the color value in uConstantColot */
   uint32                      uConstantColor;  /**< Constant color value provided in the given eFormat         */

} HAL_MDP_ColorFillConfigType;

/*!
 * \struct HAL_MDP_FetchModeConfigType
 *
 * The souce fetch/unpack engine supports memory fetch in horzontal flip and vertical flip modes
 * The \b HAL_MDP_FetchModeConfigType data structure provides the necessary configuration information to enable/disable 
 * the color fill feature of the souce fetch engine.
 */
typedef struct _HAL_MDP_FetchModeConfigType
{
   HAL_MDP_FetchConfigType    eFetchConfigType;    /**< HORIZONTAL and VERTICAL FETCH mode operation */

} HAL_MDP_FetchModeConfigType;

/*!
 * \struct HAL_MDP_RectType
 *
 * Selects the rectangle information in a buffer/surface either for source buffer or destination buffer
 */
typedef struct _HAL_MDP_RectType
{
   uint32            uPosX;            /**< starting x position                  */
   uint32            uPosY;            /**< starting y position                  */
   uint32            uWidthInPixels;   /**< Width in pixels from (x,y) position  */
   uint32            uHeightInPixels;  /**< Height in pixels from (x,y) position */

} HAL_MDP_RectType;

/*!
 * \struct HAL_MDP_DitherMatrix
 *
 * Selects the dither strength matrix
 */
typedef struct _HAL_MDP_DitherMatrix
{
   uint32      uRow0Value;             /**< Row 0 value */
   uint32      uRow1Value;             /**< Row 1 value */
   uint32      uRow2Value;             /**< Row 2 value */
   uint32      uRow3Value;             /**< Row 3 value */
} HAL_MDP_DitherMatrix;

/*!
 * \struct HAL_MDP_DitherBitDepth
 *
 * Selects the dither component bitdepth
 */
typedef struct _HAL_MDP_DitherBitDepth
{
   uint32      uColor0;             /**< Color 0 dithering boundary */
   uint32      uColor1;             /**< Color 1 dithering boundary */
   uint32      uColor2;             /**< Color 2 dithering boundary */
} HAL_MDP_DitherBitDepth;

/*!
 * \struct HAL_MDP_DecimationConfigType
 *
 * Selects the decimation factors to configure
 */
typedef struct _HAL_MDP_DecimationConfigType
{
   uint32            uHorzDecimationFactor;       /**<  Horizontal Decimation Factor  */
   uint32            uVertDecimationFactor;       /**<  Vertical Decimation  Factor   */

} HAL_MDP_DecimationConfigType;

/*!
* \struct HAL_MDP_DeinterlaceConfigType
*
*  Defines information about deinterlacing parameters
*  
*/
typedef struct _HAL_MDP_DeinterlaceConfigType
{
  bool32                            bEnabled;              /**< Enable deinterlacing feature */
  bool32                            bReferenceOddField;    /**< 1 = Use odd lines as reference. Default even line are used */
} HAL_MDP_DeinterlaceConfigType;

/*!
* \struct HAL_MDP_ChromaConfigType
*
*  Defines information about chroma site and subsampling info parameters
*  
*/
typedef struct _HAL_MDP_ChromaConfigType
{
  uint32                            uSubsamplingX;         /**< chroma Subsampling in X direction */
  uint32                            uSubsamplingY;         /**< chroma Subsampling in X direction */
  uint32                            uSiteX;                /**< chroma site info in X direction */
  uint32                            uSiteY;                /**< chroma site info in Y direction */
} HAL_MDP_ChromaConfigType;

/*!
 * \struct HAL_MDP_CropRectangleType
 *
 * Selects the rectangle information in a buffer/surface either for source buffer or destination buffer
 */
typedef struct _HAL_MDP_CropRectangleType
{
   HAL_MDP_RectType             sSrcRectConfig;    /**< Rectangle selection in source buffer       */
   HAL_MDP_RectType             sDstRectConfig;    /**< Rectangle selection in destination buffer  */

} HAL_MDP_CropRectangleType;

typedef struct _HAL_MDP_ConstAlphaConfigType
{
   bool32      bConstAlpha;                  /**< Force const alpha output            */   
   uint32      uConstAlphaValue;             /**< Only lower 8-bit values are value   */
} HAL_MDP_ConstAlphaConfigType;

/*!
 * \struct HAL_MDP_DetailEnhancerType
 *
 * This data structure is used to store detail enhancer HW specific settings. 
 * 
 */
typedef struct _HAL_MDP_DetailEnhancerType
{
  bool32                           bDetailEnhancerEnable;
  int16                            iDeSharpenLevel1;           /**< Sharpening/smooth strenght for noise */ 
  int16                            iDeSharpenLevel2;           /**< Sharpening/smooth strenght for signal */
  
  uint16                           uDeClipShift;               /**< DE clip shift */
  uint16                           uDePrecisionShift;          /**< DE precision shift */
  uint16                           uDeLimit;                   /**< DE limit value */
  
  uint16                           uDeThresholdQuiet;          /**< DE quiet threshold */
  uint16                           uDeThresholdDieout;         /**< DE dieout threshold */
  uint16                           uDeThresholdLow;            /**< DE low threshold */
  uint16                           uDeThresholdHigh;           /**< DE high threshold */
  
  /**< DE A/B/C-coefficients for mapping curve */
  int16                            iDeAdjustA[HAL_MDP_QSEED_DE_CURVE_ENTRIES];
  int16                            iDeAdjustB[HAL_MDP_QSEED_DE_CURVE_ENTRIES];
  int16                            iDeAdjustC[HAL_MDP_QSEED_DE_CURVE_ENTRIES];
}HAL_MDP_DetailEnhancerType;


/*!
 * \struct HAL_MDP_ScalarStateType
 *
 * This data structure is used to store scalar specific HW state. 
 * (For example: to store which filter coefficients LUT is uploaded to HW)
 */
typedef struct _HAL_MDP_ScalarStateType
{
  HAL_MDP_UTILITY_qseed_lut_index   aYCircLutState[HAL_MDP_QSEED_LUT_BANKS];   /**< Index of Y Circular LUT programmed to HW   */
  HAL_MDP_UTILITY_qseed_lut_index   aUVCircLutState[HAL_MDP_QSEED_LUT_BANKS];  /**< Index of UV Circular LUT programmed to HW  */
  HAL_MDP_UTILITY_qseed_lut_index   aYSepLutState[HAL_MDP_QSEED_LUT_BANKS];    /**< Index of Y Separable LUT programmed to HW  */
  HAL_MDP_UTILITY_qseed_lut_index   aUVSepLutState[HAL_MDP_QSEED_LUT_BANKS];   /**< Index of UV Separable LUT programmed to HW */
  uint32                            uLutBank;                                  /**< Current QSEED coefficients LUT bank        */
}HAL_MDP_ScalarStateType;

/*!
 * \struct HAL_MDP_ScalarConfigType
 *
 * This data structure can be used to enhance new scaling features if any. 
 * (For example: to inform the scalar quality level/scaler filter selection etc.)
 */
typedef struct _HAL_MDP_ScalarConfigType
{
   HAL_MDP_CropRectangleType        sRectInfo;                /**< Source/destination cropping rectangle info */
   // QSEED3 settings 
   bool32                           bScalarEnable;            /**< Scaler enable flag. If TRUE, QSEED scaler has to be turned on */
   bool32                           bDirectionDetectionEnable;/**< Direction detection flag. If TRUE, QSEED deriction detection has to be enabled */
   bool32                           bAlpha;                   /**< Flag defines context with alpha component. TRUE - context has alpha */
   bool32                           bYuvColorSpace;           /**< Flag defines YUV context. TRUE - YUV context */
   bool32                           bBitWidth;                /**< Bit width scaler configuration flag. FALSE - 10 bit, TRUE - 8 bit QSEED configuration */
   bool32                           bDualScalarMode;          /**< Flag defines if two scalars are working in the same use case (dual pipe/dual display case) */
   uint32                           uC0Filter;                /**< Filter configuration for color component 0 (Y/RGB)*/
   uint32                           uC12Filter;               /**< Filter configuration for color component 1 and 2 (UV) */
   uint32                           uC3Filter;                /**< Filter configuration for color component 3 (alpha component)*/
   uint32                           uBlendFilter;             /**< Blend filter Configuration */

   // QSEED3 ROI settings 
   uint32                           uC03SrcWidth;             /**< Scaler source width for color component 0 and 3 (Y/RGB and alpha)*/
   uint32                           uC03SrcHeight;            /**< Scaler source height for color component 0 and 3 (Y/RGB and alpha)*/
   uint32                           uC12SrcWidth;             /**< Scaler source width for color component 1 and 2 (UV)*/
   uint32                           uC12SrcHeight;            /**< Scaler source height for color component 1 and 2 (UV)*/
   uint32                           uDstWidth;                /**< Scaler destination width*/
   uint32                           uDstHeight;               /**< Scaler destination height*/

   // QSEED3 phase and preload settings 
   uint32                           uC03HorizontalInitPhase;  /**< Horizontal initial phase for color component 0 and 3 (Y/RGB and alpha)*/
   uint32                           uC03VerticalInitPhase;    /**< Vertical initial phase for color component 0 and 3 (Y/RGB and alpha)*/
   uint32                           uC12HorizontalInitPhase;  /**< Horizontal initial phase for color component 1 and 2 (UV)*/
   uint32                           uC12VerticalInitPhase;    /**< Vertical initial phase for color component 1 and 2 (UV)*/

   uint32                           uC03HorizontalPhaseStep;  /**< Horizontal phase step for color component 0 and 3 (Y/RGB and alpha)*/
   uint32                           uC03VerticalPhaseStep;    /**< Vertical phase step for color component 0 and 3 (Y/RGB and alpha)*/
   uint32                           uC12HorizontalPhaseStep;  /**< Horizontal phase step for color component 1 and 2 (UV)*/
   uint32                           uC12VerticalPhaseStep;    /**< Vertical phase step for color component 1 and 2 (UV)*/

   int16                            iC03HorizontalPreload;    /**< Horizontal preload settings for color component 0 and 3 (Y/RGB and alpha)*/
   int16                            iC03VerticalPreload;      /**< Vertical preload settings for color component 0 and 3 (Y/RGB and alpha)*/
   int16                            iC12HorizontalPreload;    /**< Horizontal preload settings for color component 1 and 2 (UV)*/
   int16                            iC12VerticalPreload;      /**< Vertical preload settings for color component 1 and 2 (UV)*/

   // QSEED3 filters coeffecients LUT settings
   // Defines libScale LUT control flags that specify which QSEED LUT has to be updated
   bool32                           bLutSwapEnable;           /**< If flag is set, SWAP bit has to be updated for QSEED coefficients LUT*/
   bool32                           b2dWriteEnable;           /**< If flag is set, 2D 4x4 filter LUT has to be updated */
   bool32                           bYCircWriteEnable;        /**< If flag is set, Y Circular filter LUT has to be updated */
   bool32                           bYSepWriteEnable;         /**< If flag is set, Y Separable filter LUT has to be updated */
   bool32                           bUVCircWriteEnable;       /**< If flag is set, UV Circular filter LUT has to be updated*/
   bool32                           bUVSepWriteEnable;        /**< If flag is set, UV Separable filter LUT has to be updated*/
   
   // Following parameters specify index in the LUT array which QSEED coefficients LUT has to be programmed to HW
   HAL_MDP_UTILITY_qseed_lut_index  e2dLutIndex;              /**< Specifies index in 2d 4x4 LUT array for settings have to be uploaded */
   HAL_MDP_UTILITY_qseed_lut_index  eYCircLutIndex;           /**< Specifies index in Y circular filter LUT array for settings have to be uploaded*/
   HAL_MDP_UTILITY_qseed_lut_index  eUVCircLutIndex;          /**< Specifies index in UV circular filter LUT array for settings have to be uploaded*/
   HAL_MDP_UTILITY_qseed_lut_index  eYSepLutIndex;            /**< Specifies index in Y separable filter LUT array for settings have to be uploaded*/
   HAL_MDP_UTILITY_qseed_lut_index  eUVSepLutIndex;           /**< Specifies index in UV  separable filter LUT array for settings have to be uploaded*/

   HAL_MDP_DetailEnhancerType       sDetailEnhancer;          /**< Structure contains detail enhancer specific settings*/

} HAL_MDP_ScalarConfigType;


/*!
 * \struct HAL_MDP_ScalingConfigType
 *
 * Source pipe (SSPP) supports the scaling operation after unpack. The source pipe supports
 * various scaling filter configurations. The application/user can select the right filter based on the
 * source data and scaling type i.e upscale or downscale 
 */
typedef struct _HAL_MDP_ScalingConfigType
{
   HAL_MDP_ScaleFilterMode    eHorScaleFilter;  /**< Horizontal Scale Filter selector */
   HAL_MDP_ScaleFilterMode    eVerScaleFilter;  /**< Vertical Scale Filter selector   */
} HAL_MDP_ScalingConfigType;

// TODO: Should replace HAL_MDP_ScalingConfigType and HAL_MDP_ScalarInfo with this type
/*!
 * \struct HAL_MDP_ScalerPlaneType
 *
 * This type is per plane basis. Source pipe (SSPP) supports the scaling operation after unpack. The source pipe supports
 * various scaling filter configurations. The application/user can select the right filter based on the
 * source data and scaling type i.e upscale or downscale 
 */
typedef struct _HAL_MDP_ScalerPlaneType
{
   uint32                     uPhaseStepX;      /**< Phase step on X direction        */
   uint32                     uPhaseStepY;      /**< Phase step on Y direction        */
   int32                      iInitPhaseX;      /**< Initial phase on X direction     */
   int32                      iInitPhaseY;      /**< Initial phase on Y direction     */
   HAL_MDP_ScaleFilterMode    eHorScaleFilter;  /**< Horizontal Scale Filter selector */
   HAL_MDP_ScaleFilterMode    eVerScaleFilter;  /**< Vertical Scale Filter selector   */
} HAL_MDP_ScalerPlaneType;

/*!
 * \struct HAL_MDP_ColorEncodeConfigType
 *
 * The source pipe performs all the color enhancements in HSV color space. After applying the scaling
 * operation the input data requires to be in RGB color space. The industry has various types of CSC matrix
 * for YUV to RGB color space conversion. This data structure allows to select the CSC matrix based on
 * the input stream.
 */
typedef struct _HAL_MDP_ColorEncodeConfigType
{
   HAL_MDP_ColorEncodeType    eColorEncodeType;    /**< CSC Matrix selector */

} HAL_MDP_ColorEncodeConfigType;

/*!
 * \struct HAL_MDP_ImmediateFlipConfigType
 *
 * Controls the HSYNC or VSYNC flip of the buffer address
 */
typedef struct _HAL_MDP_ImmediateFlipConfigType
{
   bool32      bHsyncFlipEnable;                 /**< Buffer flip at HSYNC enable */

} HAL_MDP_ImmediateFlipConfigType;

/*!
 * \struct HAL_MDP_VC1RangeLevelConfigType
 *
 * Configures the VC1 range level mappings for ViG layers
 */
typedef struct _HAL_MDP_VC1RangeLevelConfigType
{
   HAL_MDP_VC1RangeLevelType     eRangeMapY;    /**< Luma range mapping    */
   HAL_MDP_VC1RangeLevelType     eRangeMapUV;   /**< Chroma range mapping  */

} HAL_MDP_VC1RangeLevelConfigType;

/*!
 * \struct HAL_MDP_SharpeningThresholdParams
 *
 * Configures the sharpening threshold parameters for a sharpening modes
 */

typedef struct _HAL_MDP_SharpeningThresholdParams
{
  int32        iSharpeningLevel;              /**< Sharpening Strength Level */
  uint32       uSharpingEdgeThresh;           /**< Sharpening Edge Threshold Value */
  uint32       uSharpingSmoothThresh;         /**< Sharpening Smoothness Threshold Value */
  uint32       uSharpingNoiseThresh;          /**< Sharpening Noise Threshold Value */
} HAL_MDP_SharpeningThresholdParams;

/*!
 * \struct HAL_MDP_SharpenConfigType
 *
 * Configures the sharpening information for ViG layers
 */
typedef struct _HAL_MDP_SharpenConfigType
{
   uint32                             uLevel;                      /**< Sharpening level  Range [0-255]*/
   HAL_MDP_SharpeningModeType         eSharpeningMode;             /**< Sharpening mode   */
   HAL_MDP_SharpeningThresholdParams  sSharpeningThresholdParams;  /**< Sharpening threshold params */

} HAL_MDP_SharpenConfigType;

/*!
 * \struct HAL_MDP_IGCConfigType
 *
 * The output of source pipe has option to select the Inversion Gamma Correction
 * HAL_MDP_Init() initializes the IGC LUTs
 *
 */
typedef struct _HAL_MDP_IGCConfigType
{
   bool32      bIGCEnable;   /**< Enables the IGC block feature when set to true and viceversa */
   uint32      uTableSize;   /**< Size of the inverse gamma data */
   uint32      *pRedCr;      /**< Inverse gamma data for Red/Cr.  */
   uint32      *pGreenY;     /**< Inverse gamma data for Green/Y. */
   uint32      *pBlueCb;     /**< Inverse gamma data for Blue/Cb  */

} HAL_MDP_IGCConfigType;

/*!
 * \enum HAL_MDP_AD_DualCore_SideFrameType
 *
 *  The HAL_MDP_AD_DualCore_SideFrameType indicates the side of the frame to be processed by the
 *  AD Core; this is used in Dual-Pipe\Dual-Core scenarios to distinguish which portion of the frame will process each core.
 * 
 */
typedef enum _HAL_MDP_AD_DualCore_SideFrameType
{
  HAL_MDP_AD_DUALCORE_LEFTSIDE_FRAME,          /**< Left side of the Frame is processed by the AD-Core */
  HAL_MDP_AD_DUALCORE_RIGHTSIDE_FRAME          /**< Right side of the Frame is processed by the AD-Core */
} HAL_MDP_AD_DualCore_SideFrameType;

/*!
 * \struct HAL_MDP_ADInitType
 *
 * Initialization information for Assertive Display (AD) core
 *
 */
typedef struct
{
  bool32                             bEnable;                                                   /**< AD Feature is enabled by the OEM */
  bool32                             bBypass;                                                   /**< AD Core is Bypassed, this variable enable (bBypass = FALSE) or 
                                                                                                     disable (bBypass = TRUE) the AD-Core processing */
  uint32                             uDataFormat;                                               /**< Input Data format for AD Core */
  bool32                             bAutoSize;                                                 /**< Enable AD-AutoSize, if enabled frame size is calculated by the core */
  uint32                             uFrameWidth;                                               /**< Width of the frame to be process by AD */
  uint32                             uFrameHeight;                                              /**< Height of the frame to be process by AD */
  uint32                             uControl0;                                                 /**< Control configuration values (bit 0 enable processing) */
  uint32                             uReservedControl1Val;                                      /**< Value reserved for AD-Core debugging */
  uint32                             uVariance;                                                 /**< Setup sensitivity */
  uint32                             uDitherStrength;                                           /**< Strength of dithering */
  uint32                             uDitherControl;                                            /**< Setup dither control */
  uint32                             uLimitAmpl;                                                /**< Parameters to restrict the luminance space */
  uint32                             uSlopeMin;                                                 /**< Used to restrict the slope of the tone-curve */
  uint32                             uSlopeMax;                                                 /**< Used to restrict the slope of the tone-curve */
  uint32                             uBlackLevel;                                               /**< Black level of the core */
  uint32                             uWhiteLevel;                                               /**< White level of the core */
  uint32                             uStrengthLimit;                                            /**< Maximum dynamic range compression strength */
  uint32                             uCalibrationA;                                             /**< Screen Calibration parameter */
  uint32                             uCalibrationB;                                             /**< Screen Calibration parameter */
  uint32                             uCalibrationC;                                             /**< Screen Calibration parameter */
  uint32                             uCalibrationD;                                             /**< Screen Calibration parameter */
  uint32                             uOptionSelect;                                             /**< Select the operating mode of the core */
  uint32                             uTFilterControl;                                           /**< Recursion depth of temporal filter */
  uint32                             uBacklightMax;                                             /**< Maximum output backlight permitted */
  uint32                             uBacklightMin;                                             /**< Minimum output backlight permitted */
  uint32                             uBacklightScale;                                           /**< Maximum possible backlight AD input value */
  uint32                             uAmbientLightMin;                                          /**< Minimum ambient light level in which output backlight is set to min */
  uint32                             uFilterA;                                                  /**< Filter values for the input ambient light */
  uint32                             uFilterB;                                                  /**< Filter values for the input ambient light */
  uint32                             uBufferMode;                                               /**< Double buffering control for Video Pipeline configuration parameters */
  uint32                             aAssymetry[HAL_MDP_ASSERTIVE_DISPLAY_CALIB_LEN];           /**< Asymmetry Function Lookup Table .. */
  uint32                             aSensorLinearization[HAL_MDP_ASSERTIVE_DISPLAY_CALIB_LEN]; /**< Calibration Lookup Table */
  uint32                             aColorCorrection[HAL_MDP_ASSERTIVE_DISPLAY_CALIB_LEN];     /**< Color Correction Lookup Table ..*/
  bool32                             bDualPipe;                                                 /**< AD Dual Pipe feature is enabled */
  HAL_MDP_AD_DualCore_SideFrameType  eDualPipeFrameSide;                                        /**< AD Dual Pipe side of the Panel updated by this core */
  
} HAL_MDP_ADInitType;

/*
 * Internal Struct to fill AD Core LUTs.
 */
typedef struct _HAL_MDP_DSPP_FillLUTParams_
{
  uintPtr      uRegisterOffset;        /**< offset to be added to each Register (to account if we are using second IP) */
  uintPtr     *uRegisterAddr;          /**< Pointer to the Array with the Registers address */
  uint32       uMaxRegisterAddrVal;    /**< Maximum number of elements uRegisterAddr */
  uint32      *uTable;                 /**< Pointer to the Table of Values to be program in the Registers */
  uint32       uMaxTableVal;           /**< Maximum number of elements in uTable */
  uint32       uShiftLessSignificant;  /**< bits to left shift the less significant value */
  uint32       uShiftMostSignificant;  /**< bits to left shift the Most significant value */
  uint32       uMask;                  /**< valid bits per element in the table */

} HAL_MDP_DSPP_FillLUTParams;

/*!
 * \struct HAL_MDP_ADDynamicType
 *
 * Assertive Display core dynamic params (continuously changing).
 *
 */
typedef struct 
{
  uint32 uStrength;            /**< Manual override value of strength input */
  uint32 uBacklight;           /**< Backlight Level in LUX */
  uint32 uAmbientLight;        /**< Ambient Light Level in LUX */
  bool32 bBypass;              /**< Bypass AD (TRUE - Core will be bypass, no processing on-going; FALSE - processing on */
  
} HAL_MDP_ADDynamicType;


/*!
 * \struct HAL_MDP_ADTriggerType
 *
 * Assertive Display core trigger calculator
 *
 */
typedef struct
{
  bool32 bStartCalc;   /**< Start Calculation */
  
} HAL_MDP_ADTriggerType;

/*!
 * \struct HAL_MDP_ADCalibrationType
 *
 * Assertive Display core calibration params that could be modified dynamically.
 *
 */
typedef struct 
{
  uint32                     uCalibrationA;   /**< Screen Calibration parameter */
  uint32                     uCalibrationB;   /**< Screen Calibration parameter */
  uint32                     uCalibrationC;   /**< Screen Calibration parameter */
  uint32                     uCalibrationD;   /**< Screen Calibration parameter */
  uint32                     uStrengthLimit;  /**< Maximum dynamic range compression strength */
  uint32                     uTFilterControl; /**< Recursion depth of temporal filter */
  uint32                     uMaxIterations;  /**< Number of iterations to trigger the Start Calc */
  
} HAL_MDP_ADCalibrationType;

/*!
 * \struct HAL_MDP_ADConfigType
 *
 * All Assertive Display core configurations
 *
 */
typedef struct 
{
  HAL_MDP_ADInitType            *psAssertiveDisplayInit;
  HAL_MDP_ADDynamicType         *psAssertiveDisplayDynamic;
  HAL_MDP_ADTriggerType         *psAssertiveDisplayTrigger;
  HAL_MDP_ADCalibrationType     *psAssertiveDisplayCalibration;
  
} HAL_MDP_ADConfigType;


/*!
 * \struct HAL_MDP_Mixer_AlphaConfig
 *
 * Various blend configuration settings for layer mixers blend operation
 */
typedef struct _HAL_MDP_Mixer_AlphaConfig
{
   bool32                        bConfigure;             /**< TRUE =  Perform the HW configuration. FALSE = NO Register Access */
   HAL_MDP_Mixer_AlphaSrcType    eAlphaSel;              /**< Alpha selection                                                  */
   bool32                        bUseNonNative;          /**< Use alpha value from other layer                                 */
   uint32                        uConstant;              /**< Constant alpha value to be used when ConstantAlpha is selected    */
   bool32                        bModulateAlpha;         /**< Modulate Alpha                                                   */
   bool32                        bInverseAlpha;          /**< Inverse Alpha                                                    */
   bool32                        bInverseModulateAlpha;  /**< Inverse modulated Alpha selection                                */

} HAL_MDP_Mixer_AlphaConfig;

/*!
 * \struct HAL_MDP_ColorInfo
 *
 * Color information in terms of color components
 */
typedef struct _HAL_MDP_ColorInfo
{
   uint8       uGreenColor;               /**< Color component 0 (Green/Luma) */
   uint8       uRedColor;                 /**< Color component 1 (Red/Cr)     */
   uint8       uBlueColor;                /**< Color component 2 (Blue/Cb)    */
   uint8       uAlphaColor;               /**< Color component 3 (Alpha)      */

} HAL_MDP_ColorInfo;

/*!
 * \struct HAL_MDP_Mixer_ColorKeyConfig
 *
 * The color key configuration settings in layer mixer blend stage.
 */
typedef struct _HAL_MDP_Mixer_ColorKeyConfig
{
   bool32                 bConfigure;   /**< TRUE =  Perform the HW configuration. FALSE = NO Register Access */
   bool32                 bEnable;      /**< Enable/Disable the color key operation                           */
   HAL_MDP_ColorInfo      sColorLow;    /**< The lower limit of color value for color keying operation        */
   HAL_MDP_ColorInfo      sColorHigh;   /**< The higher limit of color value for color keying operation       */

} HAL_MDP_Mixer_ColorKeyConfig;

/*!
 * \struct HAL_MDP_BlendStageConfigType
 *
 * The blend stage configuration settings for alpha and color-key operations
 */
typedef struct _HAL_MDP_Mixer_BlendStageConfigType
{
   HAL_MDP_Mixer_AlphaConfig     sBackGroundAlphaBlend;  /**< Alpha configuration for Background layer            */
   HAL_MDP_Mixer_AlphaConfig     sForeGroundAlphaBlend;  /**< Alpha configuration for Foreground layer            */
   HAL_MDP_Mixer_ColorKeyConfig  sBackGroundColorKey;    /**< Color key configuration for Background layer        */
   HAL_MDP_Mixer_ColorKeyConfig  sForeGroundColorKey;    /**< Color key configuration for Foreground layer        */
   bool32                        bSelectColor3FromFG;    /**< Selection of color3 component at blend stage output 
                                                           *  TRUE  : Outputs color3 (Alpha value) from Foreground layer
                                                           *  FALSE : Outputs color3 (Alpha value) from Background layer
                                                           */

} HAL_MDP_Mixer_BlendStageConfigType;


/*!
 * \struct HAL_MDP_Mixer_GammaCorrectConfigType
 *
 * The data structure for enabling/disabling the Area reduced Gamma Correction process after blend stage (End of Layer Mixer)
 */
typedef struct _HAL_MDP_Mixer_GammaCorrectConfigType
{
   bool32                        bEnable;          /**< Enable/Disable the GC feature                */
   uint32                        uSegmentSize;     /**< Size of each gamma correction segment        */
   HAL_MDP_AreaReductionLutType  *psAR_LUT_Red;    /**< AR GC LUT Red-Component                      */
   HAL_MDP_AreaReductionLutType  *psAR_LUT_Green;  /**< AR GC LUT Green-Component                    */
   HAL_MDP_AreaReductionLutType  *psAR_LUT_Blue;   /**< AR GC LUT Blue-Component                     */ 
} HAL_MDP_Mixer_GammaCorrectConfigType;


/*!
 * \struct HAL_MDP_Mixer_Linear_GammaCorrectConfigType
 *
 * The data structure for enabling/disabling the Gamma Correction process after blend stage (End of Layer Mixer)
 */
typedef struct _HAL_MDP_Mixer_Linear_GammaCorrectConfigType
{
   bool32      bEnable;             /**< Enable usage of LUT table (pixel mapping)                                                             */
   bool32      bSwap;               /**< SWAP the LUT table used by software and hardware                                                      */
   bool32      bConfigLUT;          /**< Program the LUT table                                                                                 */
   uint32     *puLUTData;           /**< Gamma LUT data. Should point to a buffer with HAL_MDP_GAMMA_LUT_ENTRIES                               */
   
} HAL_MDP_Mixer_Linear_GammaCorrectConfigType; 

/*!
 * \struct HAL_MDP_Mixer_CursorBlendConfigType
 *
 * The data structure for providing the cursor blend properties
 *
 */
typedef struct _HAL_MDP_Mixer_CursorBlendConfigType
{
   bool32                     bConfigure;                /**< TRUE - HAL configures the cursor blend properties */
   bool32                     bEnable;                   /**< TRUE - Enable cursor blend operations             */
   HAL_MDP_CursorBlendType    eCursorBlendType;          /**< Cursor blend type. Constant/per-pixel Alpha       */
   uint32                     uConstAlpha;               /**< Only Lower 8-bits valid. valid only if eCursorBlendType =  HAL_MDP_CURSOR_BLEND_TYPE_CONSTANT_ALPHA */

} HAL_MDP_Mixer_CursorBlendConfigType;

/*!
 * \struct HAL_MDP_Mixer_CursorConfigType
 *
 * The cursor blend stage configuration settings for alpha and color-key operations
 *
 */
typedef struct _HAL_MDP_Mixer_CursorConfigType
{
   HAL_MDP_Mixer_ColorKeyConfig           sColorKeyConfig;
   HAL_MDP_Mixer_CursorBlendConfigType    sCursorBlendConfig;

} HAL_MDP_Mixer_CursorConfigType;

/*!
 * \struct HAL_MDP_Mixer_OutputSizeConfigType
 *
 * The data structure for providing the layer mixer output size parameters.
 *
 */
typedef struct _HAL_MDP_Mixer_OutputSizeConfigType
{
   uint32            uOutputWidthInPixels;               /**< Width  - Mixer Output is always RGB888 */
   uint32            uOutputHeightInPixels;              /**< Height - Mixer Output is always RGB888 */

} HAL_MDP_Mixer_OutputSizeConfigType;

typedef struct _HAL_MDP_Mixer_BorderColorConfigType
{
   uint32            uColorValue;                        /**< in ARGB 32 bit format                   */
   uint32            uFlags;                             /**< Reserved                                */

} HAL_MDP_Mixer_BorderColorConfigType;

typedef struct _HAL_MDP_LayerMixerConfigType
{
   HAL_MDP_LayerMixerId                        eLayerMixerId;
   HAL_MDP_Mixer_BlendStageConfigType          *psBlendStageConfig[HAL_MDP_BLEND_STAGE_MAX];
   HAL_MDP_Mixer_CursorConfigType              *psCursorConfig;
   HAL_MDP_Mixer_GammaCorrectConfigType        *psGammaCorrectConfig;
   HAL_MDP_Mixer_OutputSizeConfigType          *psOutputSizeConfig;
   HAL_MDP_Mixer_BorderColorConfigType         *psBorderColorConfig;
   HAL_MDP_Mixer_Linear_GammaCorrectConfigType *psLinearGammaCorrectionConfig;

} HAL_MDP_LayerMixerConfigType;
/*==================================================================================================================*/

/**
 *  =================================================================================================
 *  Chroma Down Module Configuration
 *  =================================================================================================
 */
/*!
 * \struct _HAL_MDP_CDM_Info 
 * 
 */
typedef struct _HAL_MDP_CDM_Info
{
  HAL_MDP_ChromaDownModuleId eChromaDownId;                    /**< Chroma Down Module Id           */
} HAL_MDP_CDM_Info;

/*!
 * \struct _HAL_MDP_ChromaDown2ConfigType 
 * 
 */
typedef struct _HAL_MDP_ChromaDownSampleConfigType 
{
  HAL_MDP_ChromaDownSampleMethodType  eVerDownSampleMethod;     /**< Chroma down sample method in Vertical direction       */
  HAL_MDP_ChromaDownSampleMethodType  eHorDownSampleMethod;     /**< Chroma down sample method in Horizontal direction     */
} HAL_MDP_ChromaDownSampleConfigType;

/*!
 * \struct _HAL_MDP_ChromaDown3DConfigType 
 *  
 *
 *  Below represent the different 3D frame formats 
 *  - Side by Side format
 *
 *   <------------- Source Width-------------->
 *  |--------------------|--------------------|
 *  |                    |                    |
 *  |                    |                    |
 *  |                    |                    |
 *  |                    |                    |
 *  |                    |                    |
 *  |                    |                    |
 *  |--------------------|--------------------|
 *  <--u3DFrameOffset -->
 *
 *  - Top Bottom Format
 *
 *       ^        |--------------------|   ^
 *       |        |                    |   |
 *       |        |                    |   |
 * u3DFrameOffset |                    |   |
 *       |        |                    |  Source
 *       v        |--------------------|  Height
 *                |                    |   |
 *                |                    |   |
 *                |                    |   |
 *                |                    |   |
 *                |                    |   |
 *                |--------------------|   v
*
 *  - Frame packed format
 *
 *       ^        |--------------------|   ^
 *       |        |                    |   |
 * u3DFrameOffset |                    |   |
 *                |                    |   |
 *       |        |                    |   |
 *       -        |--------------------|   |
 *       |        |                    |   |
 *       |        |                    |   |
 *   uVerticalGap |                    |   |
 *       |        |                    |  Source
 *       v        |--------------------|  Height
 *                |                    |   |
 *                |                    |   |
 *                |                    |   |
 *                |                    |   |
 *                |                    |   |
 *                |--------------------|   v
 */
typedef struct _HAL_MDP_ChromaDown3DConfigType 
{
  HAL_MDP_3DFrameFormatType    e3DFrameFormat;               /**< 3D frame format type                                                                          */ 
  uint32                       u3DFrameOffset;               /**< if Side-by-Side format it represents  each Frame's width, else represents each frame's Height */
  uint32                       uVerticalGap;                 /**< Vertical active gap lines for Frame-packed format                                             */
}HAL_MDP_ChromaDown3DConfigType;

/*!
 * \struct _HAL_MDP_ChromaDownModuleConfigType 
 * 
 *  The \b HAL_MDP_ChromaDownModuleConfigType data structure provides the configuration details for 
 *  CDM block
 *
 * psColorEncInfo    - Color space conversion information
   psHDMIPackerInfo  - Holds information if the data is routed to HDMI Interface and the output pixel format
 *
 */
typedef struct _HAL_MDP_ChromaDownModuleConfigType
{
   HAL_MDP_ChromaDownSampleConfigType       *psChromaDown2Info;   /**< Chroma Down sampling configuration information     */
   HAL_MDP_ColorEncodeConfigType            *psColorEncInfo;      /**< Color Space Conversion configuration information   */
   HAL_MDP_ChromaDown3DConfigType           *ps3DConfigInfo;      /**< 3D frame packing information                       */
} HAL_MDP_ChromaDownModuleConfigType;

/*==================================================================================================================*/
/**
 *  =================================================================================================
 *  Interface Configuration
 *  =================================================================================================
 */

/*!
 * \struct HAL_MDP_PhyIntf_TimingGenConfigType
 *
 * The \b HAL_MDP_PhyIntf_TimingGenConfigType provides the configuration parameters to setup the video timing engines and
 * interface standard selection.
 */
typedef struct _HAL_MDP_PhyIntf_TimingGenConfigType
{
   HAL_MDP_InterfaceModeType              eInterfaceMode;               /**< Interface Mode. COMMAND or VIDEO Mode                     */
   HAL_MDP_InterfaceStandardType          eInterfaceStandard;           /**< Interface standard/Protocol                               */
   HAL_MDP_PixelFormatType                eInterfacePixelFormat;        /**< Interface/Panel pixel format. Supports only RGB formats   */

   bool32                                 bInterlacedMode;              /**< Progressive or Interfaced mode selection                  */

   bool32                                 bDSIVideoStopMode;            /**< Stop video stream when DSI dynamic refresh is triggered   */

   uint32                                 uVisibleWidthInPixels;        /**< Visible width in pixels (visible active area)             */
   uint32                                 uVisibleHeightInPixels;       /**< Visible height in pixels (visible active area)            */

   uint32                                 uHsyncFrontPorchInPixels;     /**< HSYNC Front Porch width in pixels                         */
   uint32                                 uHsyncBackPorchInPixels;      /**< HSYNC Back Porch width in pixels                          */
   uint32                                 uHsyncPulseInPixels;          /**< HSYNC pulse width in pixels                               */
   uint32                                 uHysncSkewInPixels;           /**< HSYNC active edge is delayed from VSYNC active edge       */

   uint32                                 uVsyncFrontPorchInLines;      /**< VSYNC Front Porch width in lines for Field #0             */
   uint32                                 uVsyncBackPorchInLines;       /**< VSYNC Back Porch width in lines  for Field #0             */
   uint32                                 uVsyncPulseInLines;           /**< VSYNC pulse width in lines for Field #0                   */

   uint32                                 uHLeftBorderInPixels;         /**< Left border in pixels                                     */
   uint32                                 uHRightBorderInPixels;        /**< Right border in pixels                                    */
   uint32                                 uVTopBorderInLines;           /**< Top border in lines                                       */
   uint32                                 uVBottomBorderInLines;        /**< Bottom border in lines                                    */

   uint32                                 uBorderColorInRGB888;         /**< Data to be displayed for non-active display region        */
   uint32                                 uUnderflowColorInRGB888;      /**< Panel underflow control for active display region         */

   HAL_MDP_SignalPolarityType             eDataEnableSignal;            /**< DATA ENABLE signal polarity                               */
   HAL_MDP_SignalPolarityType             eVsyncSignal;                 /**< VSYNC signal polarity                                     */
   HAL_MDP_SignalPolarityType             eHsyncSignal;                 /**< HSYNC signal polarity                                     */

   // Only for interlace. Following section is reserved and should not be used.
   uint32                                 uVsyncHeightInPixelsF1;       /**< Actual data pixels (Border + Active + lines) Field #1     */
   uint32                                 uVsyncFrontPorchInLinesF1;    /**< VSYNC Front Porch width in lines for Field #1             */
   uint32                                 uVsyncBackPorchInLinesF1;     /**< VSYNC Back Porch width in lines  for Field #1             */
   uint32                                 uVsyncPulseInLinesF1;         /**< VSYNC pulse width in lines for Field #1                   */

} HAL_MDP_PhyIntf_TimingGenConfigType;


typedef union _HAL_MDP_TPG_ConfigInfo
{
   uint32      uFixedPattern;       /**< Only lower 24-bits are valid */

   // TODO

} HAL_MDP_TPG_ConfigInfo;

/*!
 * \struct HAL_MDP_Interface_TPGConfigType
 *
 * The \b HAL_MDP_Interface_TPGConfigType configures the TPG module
 */
typedef struct _HAL_MDP_Interface_TPGConfigType
{
   bool32                  bEnable;                         /**< Enable/Disable the TPG                           */
   HAL_MDP_TPG_Type        eTestPatternType;                /**< Test Pattern Generation Type selection           */
   HAL_MDP_TPG_ConfigInfo  sTestPatternConfigInfo;          /**< Test pattern information for selected TPG Type   */

} HAL_MDP_Interface_TPGConfigType;


/*!
 * \struct HAL_MDP_PingPong_VsyncConfigType
 *
 * The \b HAL_MDP_PingPong_VsyncConfigType configures the vsync counter in the pingpong block
 *
 * Notes:
 *     - uVsyncCount:
 *               Normally, vsync count is calculated by: 
 *                   vsync_count = VSYNC_CLOCK/(dispaly_height * refresh_rate)
 *               If uVsyncVariance is set, then vsync_count is adjusted by:
 *                   vsync_count = vsync_count/(1-uVsyncVariance/100)
 *
 *     - uVsyncHeight
 *               uVsyncHeight = display_height + vertical_back_porch + vertical_front_porch - 1;
 *
 *     - uVsyncRdPtrIrqLine
 *               specifies on which line that the read pointer interrupt fires
                 associated with HAL_MDP_INTERRUPT_PINGPONG_RD_PTR
 *
 *     - uVsyncWrPtrIrqLine
 *               specifies on which line that the write pointer interrupt fires
 *               associated with HAL_MDP_INTERRUPT_PINGPONG_WR_PTR
 *
 *
 *     - if external TE is used, then this vsync counter should be configured to be running at slower rate
 *       which is slower then panel refresh rate. In this case, external TE signal will reset the vsync
 *       counter before it wraps around. Otherwise, double interrupts will happen.
 */
typedef struct _HAL_MDP_PingPong_VsyncConfigType
{
  bool32                        bEnableAutoRefresh;         /**< enable auto refresh                           */
  uint32                        uAutoRefreshFrameNumDiv;    /**< frame divisor for auto refresh                */
  uint32                        uVsyncCount;                /**< vsync sync count for one line                 */
  uint32                        uVsyncHeight;               /**< vsync height                                  */
  uint32                        uVsyncRdPtrIrqLine;         /**< read-pointer interrupt line                   */
  uint32                        uVsyncWrPtrIrqLine;         /**< write-pointer interrupt line                  */
  uint32                        uVsyncInitValue;            /**< value loaded to read-pointer at vsync edge    */
} HAL_MDP_PingPong_VsyncConfigType;


/*!
 * \struct HAL_MDP_PingPong_TEConfigType
 *
 * The \b HAL_MDP_PingPong_TEConfigType configures the tear check in the pingpong block
 *
 *
 *                                             (line)      (RD) (WR)    (RD)
 *                              (height)    -->  |           /   /       /
 *                                               |          /   /       /
 *                                               |         /   /       /
 *                                               |        /   |       /
 *                                               |       /    |      /
 *           (start_position+start_threshold) ---|------/     /     /    ____
 *                                               |     /| ___/     /     ____ continue threshold
 *                                               |    / | |       /
 *                                               |   /  | |      /    
 *                             start_position ---|--/   |/      /    
 *                                               | /|   /      /   
 *                                       line 0  |/_|__/|_____/_____________________ (time)
 *                                                            ^   ^
 *                                                            |   |
 *                                                            |  (wr pointer interrupt fires)
 *                                                            |
 *                                                          (rd pointer interrupt fires)
 *                                                           
 */ 
typedef struct _HAL_MDP_PingPong_TEConfigType
{
   bool32                              bDedicatedTEPin;              /**< using dedicated pin for tear check             */
   HAL_MDP_PingPong_VsyncSelectType    eVsyncSelect;                 /**< external vsync source select                   */
   uint32                              uContinueThreshold;           /**< continue threshold for tear check algorithm    */
   uint32                              uStartThreshold;              /**< start threshold for tear check algorithm       */
   uint32                              uStartPosition;               /**< start position for tear check algorithm        */
} HAL_MDP_PingPong_TEConfigType;

/*!
 * \struct HAL_MDP_PingPong_TEEnableType
 *
 * The \b HAL_MDP_PingPong_TEEnableType enable/disable the tear check block
 */
typedef struct _HAL_MDP_PingPong_TEEnableType
{
    bool32                 bEnable;             /**< enable tear check */
} HAL_MDP_PingPong_TEEnableType;

/*!
 * \struct HAL_MDP_PingPong_VsyncEnableType
 *
 * The \b HAL_MDP_PingPong_VsyncEnableType enable/disable the vsync counter
 */
typedef struct _HAL_MDP_PingPong_VsyncEnableType
{
    bool32                 bEnable;             /**< enable vsync counter */
} HAL_MDP_PingPong_VsyncEnableType;

/*!
 * \struct HAL_MDP_PingPong_AutoRefreshEnableType
 *
 * The \b HAL_MDP_PingPong_AutoRefreshEnableType enable/disable the auto-refresh
 */
typedef struct _HAL_MDP_PingPong_AutoRefreshEnableType
{
    bool32                 bEnable;             /**< enable auto-refresh */
} HAL_MDP_PingPong_AutoRefreshEnableType;

/*!
 * \struct HAL_MDP_PingPong_BufferConfigType
 *
 * The \b HAL_MDP_PingPong_BufferConfigType configures ping-pong split
 */
typedef struct _HAL_MDP_PingPong_BufferConfigType
{
   bool32               bSplitFifoEnable;         /**< Enable PP Split */
   bool32               bSwapLeftRightStreams;    /**< Swap left and right streams */
   uint32               uOverlapPixels;           /**< Number of overlap pixels in split mode. Maximum of 31 pixels */
   HAL_MDP_InterfaceId  eSecondaryIntfId;         /**< Select secondary interface to pair to */
} HAL_MDP_PingPong_BufferConfigType;


/*!
 * \struct HAL_MDP_PingPong_DitherConfigType
 *
 * The \b HAL_MDP_PingPong_DitherConfigType configures dither block on the output of the ping-pong.
 */

typedef struct _HAL_MDP_PingPong_DitherConfigType
{
   bool32                     bEnable;            /**< Enable/Disable dithering feature                 */
   HAL_MDP_DitherMatrix      *pDitherMatrix;      /**< Dither Matrix                                    */
   HAL_MDP_PixelFormatType    eDstPixelFormat;    /**< Destination pixel format. DSPP Dither block input is always RGB888 format.
                                                       The output of dither block will routed to physical interface or linear write-back interface.
                                                       HAL driver enables the dither only if the output of dither module is lower color depth than RGB888.
                                                       For example, RGB565 or RGB666 etc.         */
   HAL_MDP_DitherBitDepth     sDitherBitDepth;    /**< Dither Bit depth for each color component  */
   HAL_MDP_DitherType         eDitherType;        /**< Dither type, spatial or temporal           */ 
} HAL_MDP_PingPong_DitherConfigType;


/*!
 * \struct HAL_MDP_SplitMUX_BufferConfigType
 *
 * The \b HAL_MDP_SplitMUX_BufferConfigType configures the PPB module to enable the line-split use-cases.
 *
 * Example:                     
 *=============================================================================================
 * |    _________________       |        Case #1               |        Case #2               |
 * |   |    PING-PONG-0  |      |                              |   (bSwapLeftRightStream = 1) |
 * |   |_________________|      |                              |                              |
 * |             |              |     2560 RGB888 Pixels       |     2560 RGB888 Pixels       |
 * |             V              |             |                |             |                |
 * |    ________________        |             v                |             v                |
 * |   |    SPLIT-MUX   |       | 1280 (Left) + 1280 (Right)   | 1280 (Right) + 1280 (Left)   |
 * |   |________________|       |       |           |          |       |           |          |
 * |            /\              |       |           |          |       |           |          |
 * |  (Left)   /  \    (Right)  |       |           |          |       |           |          |
 * |          /    \            |       |           |          |       |           |          |
 * |         /      \           |       |           |          |       |           |          |
 * |      ______    ______      |       |           |          |       |           |          |
 * |     |  TE0 |  |  TE5 |     |       |           |          |       |           |          |
 * |     |______|  |______|     |       |           |          |       |           |          |
 * |         |         |        |       |           |          |       |           |          |
 * |         V         V        |       |           |          |       |           |          |
 * |      _____     ______      |       v           v          |       v           v          |
 * |     | DSI0|   |  DSI1|     |      DSI0        DSI1        |      DSI0        DSI1        |
 * |     |_____|   |______|     |                              |                              |
 *=|============================================================================================
 * ||===========================================================================================
 * |                                  Case #3                                                 | 
 * |                              (uOverlapPixels != 0)                                       |
 * |                                                                                          |
 * |                               2560 RGB888 Pixels                                         |
 * |                                     |                                                    |
 * |                                     v                                                    |
 * |    (1280+uOverlapPixels)            +        1280 (Right)                                |
 * |  (0-1279 + 1280 + uOverlapPixels)       (1280 - uOverlapPixels to 1279 +                 |
 * |                                              1280 to 2560 pixels)                        |
 * |       |                                                   |                              |
 * |       |                                                   |                              |
 * |       |                                                   |                              |
 * |       |                                                   |                              |
 * |       |                                                   |                              |
 * |       |                                                   |                              |
 * |       |                                                   |                              |
 * |       |                                                   |                              |
 * |       v                                                   v                              |
 * |      DSI0                                               DSI 1                            |
 * |                                                                                          |
 * |===========================================================================================
 *
 */
typedef struct _HAL_MDP_SplitMUX_BufferConfigType
{
   bool32               bSplitFifoEnable;          /**< TRUE - Enable the SPLIT-MUX HW logic                                        */
   bool32               bSwapLeftRightStreams;     /**< TRUE - Swap the Left and Right Streams                                      */
   uint32               uOverlapPixels;            /**< Overlapping pixel on both left and right streams                            */
   HAL_MDP_InterfaceId  eSecondaryIntfId;          /**< Control-Path can select only primary interface in split-display use-cases
                                                        This paramter is used by Split-MUX to root the Tear-Cheak-5 (TE5) output    */

} HAL_MDP_SplitMUX_BufferConfigType;

/**!
 *  \struct HAL_MDP_DSC_EncParamsType
 *
 *    Holds the list of parameters for Display Stream Compression Encoder module
 *    The parameters are extracted from PPS (Picture Parameter Set) explained in DSC standard Version 1.x
 */
#define HAL_MDP_DSC_ENC_RC_BUF_THRESHOLD_TABLE_SIZE                 14
#define HAL_MDP_DSC_ENC_RC_RANGE_PARAMS_TABLE_SIZE                  15

/* _HAL_MDP_DSC_EncParamsType: contains DSC PPS parameters */
typedef struct _HAL_MDP_DSC_EncParamsType
{
   uint32   uDSCVersionMajor;                                              /**< dsc_version_major         -   PPS0[7:4]                     */
   uint32   uDSCVersionMinor;                                              /**< dsc_version_minor         -   PPS0[3:0]                     */
   uint32   uDSCVersionScr;                                                /**< SCR version               -  PPS defines value of parameters
                                                                                applying to both compression and decompression operation.
                                                                                Usually, they are defined at DSC specification. Panel vendor
                                                                                may change PPS parameter values to meet its panel's
                                                                                requirements through SCR (Supplemental Change Request).     */
   uint32   uPPSIdentified;                                                /**< pps_identifier            -   PPS1[7:0]                     */
   uint32   uBitsPerComponent;                                             /**< bits_per_component        -   PPS3[7:4]                     */
   uint32   uLineBufDepth;                                                 /**< linebuf_depth             -   PPS3[3:0]                     */
   bool32   bBlockPredEnable;                                              /**< block_pred_enable         -   PPS4[5]                       */
   bool32   bConvertRGB;                                                   /**< convert_rgb               -   PPS4[4]                       */
   bool32   bEnable422;                                                    /**< enable_422                -   PPS4[3]                       */
   bool32   bVBREnable;                                                    /**< vbr_enable                -   PPS5[2]                       */
   uint32   uBitsPerPixel;                                                 /**< bits_per_pixel            -   PPS4[1:0],   PPS5[7:0]        */
   uint32   uPicHeight;                                                    /**< pic_height                -   PPS6[7:0],   PPS7[7:0]        */
   uint32   uPicWidth;                                                     /**< pic_width                 -   PPS8[7:0],   PPS9[7:0]        */
   uint32   uSliceHeight;                                                  /**< slice_height              -   PPS10[7:0],  PPS11[7:0]       */
   uint32   uSliceWidth;                                                   /**< slice_width               -   PPS12[7:0],  PPS13[7:0]       */
   uint32   uChunkSize;                                                    /**< chunk_size                -   PPS14[7:0],  PPS15[7:0]       */
   uint32   uInitialXmitDelay;                                             /**< initial_xmit_delay        -   PPS16[1:0],  PPS17[7:0]       */
   uint32   uInitialDecDelay;                                              /**< initial_dec_delay         -   PPS18[7:0],  PPS19[7:0]       */
   uint32   uInitialScaleValue;                                            /**< initial_scale_value       -   PPS21[5:0]                    */
   uint32   uScaleIncInterval;                                             /**< scale_increment_interval  -   PPS22[7:0],  PPS23[7:0]       */
   uint32   uScaleDecInterval;                                             /**< scale_decrement_interval  -   PPS24[3:0],  PPS25[7:0]       */
   uint32   uFirstLineBpgOffset;                                           /**< first_line_bpg_offset     -   PPS27[4:0]                    */
   uint32   uNflBpgOffset;                                                 /**< nfl_bpg_offset            -   PPS28[7:0],  PPS29[7:0]       */
   uint32   uSliceBpgOffset;                                               /**< slice_bpg_offset          -   PPS30[7:0],  PPS31[7:0]       */
   uint32   uInitialOffset;                                                /**< initial_offset            -   PPS32[7:0],  PPS33[7:0]       */
   uint32   uFinalOffset;                                                  /**< final_offset              -   PPS34[7:0],  PPS35[7:0]       */
   uint32   uFlatnessMinQp;                                                /**< flatness_min_qp           -   PPS36[4:0]                    */
   uint32   uFlatnessMaxQp;                                                /**< flatness_max_qp           -   PPS37[4:0]                    */
   uint32   uRcModelSize;                                                  /**< rc_model_size             -   PPS38[7:0],  PPS39[7:0]       */
   uint32   uRcEdgeFactor;                                                 /**< rc_edge_factor            -   PPS40[7:4]                    */
   uint32   uRcQuantIncrLimit0;                                            /**< rc_quant_incr_limit0      -   PPS41[4:0]                    */
   uint32   uRcQuantIncrLimit1;                                            /**< rc_quant_incr_limit1      -   PPS42[4:0]                    */
   uint32   uRcTgtOffsetHi;                                                /**< rc_tgt_offset_hi          -   PPS43[7:4]                    */
   uint32   uRcTgtOffsetLo;                                                /**< rc_tgt_offset_lo          -   PPS43[3:0]                    */
   uint8    uRcBufThresh[HAL_MDP_DSC_ENC_RC_BUF_THRESHOLD_TABLE_SIZE];     /**< rc_buf_thresh[0..13]      -   PPS44-PPS57                   */
   uint8    uRangeMinQp[HAL_MDP_DSC_ENC_RC_RANGE_PARAMS_TABLE_SIZE];       /**< range_min_qp[0..15]       -}                                */
   uint8    uRangeMaxQp[HAL_MDP_DSC_ENC_RC_RANGE_PARAMS_TABLE_SIZE];       /**< range_max_qp[0..15]       -}  PPS58-PPS87                   */
   int8     iRangeBpgOffset[HAL_MDP_DSC_ENC_RC_RANGE_PARAMS_TABLE_SIZE];   /**< range_bpg_offset[0..15]   -}                                */

} HAL_MDP_DSC_EncParamsType;

/*!
 * \struct HAL_MDP_DSC_EncoderConfigType
 *
 * The \b HAL_MDP_DSC_EncoderConfigType configures the DSC (Display Stream Compression) module.
 *
 * List of use-cases:
 *
 * ======================================================================================================================
 * Case#1 and Case#2: 
 *      bSplitMuxActive = FALSE, bSplitDispActive = FALSE, bPingPongSplitMUXActive = FALSE, uInputFrameWidthInPixels = Same as DSI Timing Engine Width
 *
 * ========================================================================================================================
 *                    MDSS Side Configuraton                  |                  Receiver Side Configuration
 * ===========================================================|=============================================================
 *                                                            |
 *   (1)       LM --> PP --> DSC-ENC --> DSI-TX --------------|-------> DSI-RX ---> DSC-DEC --> LCDC Panel
 *                                                            |
 * -----------------------------------------------------------|-------------------------------------------------------------
 *                                                            |
 *             LM0 --> PP0 --> DSC-ENC-0 --> DSI-TX-0 --------|-------> DSI-RX-0 ---> DSC-DEC-0 ---\
 *   (2)                                                      |                                     \------>LCDC Panel
 *                                                            |                                      /
 *             LM1 --> PP1 --> DSC-ENC-1 --> DSI-TX-1 --------|-------> DSI-RX-1 ---> DSC-DEC-1 ---/
 * -------------------------------------------------------------------------------------------------------------------------
 *
 * Case#3:
 *       bSplitMuxActive = FALSE, bSplitDispActive = TRUE, bPingPongSplitMUXActive = FALSE, uInputFrameWidthInPixels = (Width of DSI-TX-0 + DSI-TX-1)
 * -----------------------------------------------------------|-------------------------------------------------------------
 *                                                            |
 *             LM0 --> PP0 --> DSC-ENC-0 --> DSI-TX-0 --------|-------> DSI-RX-0 ---\ 
 *   (3)                                                      |                      \ ---- DSC-DEC-0 ------>LCDC Panel
 *                                                                                   /               
 *             LM1 --> PP1 --> DSC-ENC-1 --> DSI-TX-1 --------|-------> DSI-RX-1 ---/ 
 * -------------------------------------------------------------------------------------------------------------------------
 *
 * Case#4:
 *       bSplitMuxActive = TRUE, bSplitDispActive = FALSE, bPingPongSplitMUXActive = FALSE, uInputFrameWidthInPixels = (Width of DSI-TX-)
 * -----------------------------------------------------------|-------------------------------------------------------------
 *                                                            |
 *             LM0 --> PP0 --> DSC-ENC-0 --\                  |
 *   (4)                                    \ --- DSI-TX-0 ---|-------> DSI-RX ---> DSC-DEC --> LCDC Panel
 *                                          /                 |
 *             LM1 --> PP1 --> DSC-ENC-1 --/                  |
 * -------------------------------------------------------------------------------------------------------------------------
 *
 * Case#5:
 *       bSplitMuxActive = FALSE, bSplitDispActive = FALSE, bPingPongSplitMUXActive = TRUE, uInputFrameWidthInPixels = (Width of DSI-TX-)
 * -----------------------------------------------------------|-------------------------------------------------------------
 *                                                            |
 *                                   /-- DSC-ENC-0 --> DSI-0 -|-------> DSI-RX-0 ---> DSC-DEC-0 ---\
 *                                  /                         |                                     \
 *   (5)     LM0 --> PP0 --> PPB0 --                          |                                      \ ------>LCDC Panel
 *                                  \                         |                                      /
 *                                   \-- DSC-ENC-1 --> DSI-1  |-------> DSI-RX-1 ---> DSC-DEC-1 ----/
 *                                                            |
 * -------------------------------------------------------------------------------------------------------------------------
 *
 */
typedef struct _HAL_MDP_DSC_EncoderConfigType
{
   uint32                              uInputFrameWidthInPixels;     /**< Input width in pixels for DSC encoder                  */
   uint32                              uInputFrameHeightInPixels;    /**< Input height in pixeld for DSC encoder                 */
   HAL_MDP_CompressionRatioType        eCompressionRatio;            /**< Compression Ration                                     */
   bool32                              bSplitMuxActive;              /**< Combine the outputs of 2 DSCs to make a single stream  */
   bool32                              bSplitDispActive;             /**< 2 DSC ENC at MDSS and 1 DSC DEC at Panel/Receiver side */
   bool32                              bPingPongSplitMUXActive;      /**< Ping-Pong line split logic is tuned on                 */
   HAL_MDP_PingPongId                  ePingPongId;                  /**< Ping-Pong ID to which DSC module is connected to       */
   HAL_MDP_InterfaceModeType           eInterfaceMode;               /**< Command Mode (or) Video Mode panel                     */
   bool32                              bUserProvidedEncParams;       /**< TRUE = Use the user provided Encoder parameters        */
   HAL_MDP_DSC_EncParamsType          *pEncoderParams;               /**< MUST be non-null pointer. 
                                                                       *< bUserProvidedEncParams = TRUE, HAL driver uses the 
                                                                       *< encoder parameter provided through pEncoderParams to 
                                                                       *< configure the DSC encoder module.
                                                                       *< bUserProvidedEncParams = FALSE, HAL driver uses the 
                                                                       *< internal default encoder parameters and populates
                                                                       *< pEncoderParams with the default encoder parameters     */
   uint32                              uBytePerPkt;                  /**< Total number of bytes per line/per encoder */
   uint32                              uPktPerLine;                  /**< Total number of packets per line/per encoder */

} HAL_MDP_DSC_EncoderConfigType;

/**
 * \struct HAL_MDP_PingPong_DSCConfigType
 *
 *    Holds the parameters to turn ON/OFF the DSC (Display Stream Compression Module)
 */
typedef struct _HAL_MDP_PingPong_DSCConfigType
{
   bool32                  bEnableDSCEnc;                      /**< TRUE = Turn ON the compression engine */
   bool32                  bEndianess;                         /**< TRUE = Flip Endianess */

} HAL_MDP_PingPong_DSCConfigType;

/*!
 * \struct HAL_MDP_Interface_MISRConfigType
 *
 * The \b HAL_MDP_Interface_MISRConfigType provides the configuration parameters to setup the MISR (CRC) block for
 * the given interface connection
 */
typedef struct _HAL_MDP_Interface_MISRConfigType
{
   bool32                              bEnable;                /**< Enable/Disable the MISR config                        */ 
   HAL_MDP_InterfaceConnectionType     eConnectType;           /**< Interface Connection Module, DSI0, DSI1, HDMI, WB etc */
   HAL_MDP_InterfaceStreamId           eStreamId;              /**< Interface-stream identification                       */
   uint32                              uFrameCnt;              /**< Generate CRC for given number of frames               */

} HAL_MDP_Interface_MISRConfigType;

/*!
 * \struct HAL_MDP_Interface_MISRInfoType
 *
 * The \b HAL_MDP_Interface_MISRInfoType return the MISR value
 *
 */
typedef struct _HAL_MDP_Interface_MISRInfoType
{
   HAL_MDP_InterfaceConnectionType     eConnectType;           /**< [in] Interface connection module              */
   HAL_MDP_InterfaceStreamId           eStreamId;              /**< [in] Interface-stream identification          */
   uint32                              uCRCValue;              /**< [out] Generated CRC value                     */

} HAL_MDP_Interface_MISRInfoType;

/*!
 * \struct HAL_MDP_Interface_MISRStatusType
 *
 * The \b HAL_MDP_Interface_MISRStatusType returns the status of the CRC generation
 *
 */
typedef struct _HAL_MDP_Interface_MISRStatusType
{
   HAL_MDP_InterfaceConnectionType     eConnectType;           /**< [in] Interface connection module              */
   HAL_MDP_InterfaceStreamId           eStreamId;              /**< [in] Interface-stream identification          */
   bool32                              bIsCompleted;           /**< [out] CRC generate complete status         */

} HAL_MDP_Interface_MISRStatusType;

/*!
 * \struct HAL_MDP_Interface_MISRConfigStatusType
 *
 * The \b HAL_MDP_Interface_MISRConfigStatusType returns the current MISR config param of config reg
 * This function can be used to retrieve the current configuration of MISR control reg. This will be 
 * while configuring the SFI (only on supported platform)
 *
 */
typedef struct _HAL_MDP_Interface_MISRConfigStatusType
{
   HAL_MDP_InterfaceConnectionType     eConnectType;           /**< [in] Interface connection module              */
   HAL_MDP_InterfaceStreamId           eStreamId;              /**< [in] Interface-stream identification          */
   bool32                              bEnable;                /**< [out] True if MISR enabled                    */ 
   bool32                              bIsCompleted;           /**< [out] CRC generate complete status            */
   bool32                              bContinuousMode;        /**< [out] True if Free (True) running mode        */
   uint32                              uFrameCnt;              /**< [out] Current MISR frame count config         */

} HAL_MDP_Interface_MISRConfigStatusType;

/*
 * HAL_MDP_Interface_MISRReturnType
 *
 */
typedef struct _HAL_MDP_Interface_MISRReturnType
{
   HAL_MDP_Interface_MISRInfoType         *pMISR_CRC_Info;            /**< MISR signature read                             */
   HAL_MDP_Interface_MISRStatusType       *pMISR_Status_Info;         /**< MISR active/completion status                   */
   HAL_MDP_Interface_MISRConfigStatusType *pMISR_Config_Status_Info;  /**< MISR current config reg status                  */
} HAL_MDP_Interface_MISRReturnType;

/*!
 * \struct HAL_MDP_Interface_FrameRefreshInfoType
 *
 * The \b HAL_MDP_Interface_FrameRefreshInfoType returns the current line and frame refresh count.
 * Valid for the active refresh display interfaces only.
 */
typedef struct _HAL_MDP_Interface_FrameRefreshInfoType
{
   uint32            uCurrentFrameCnt;                   /**< Current Frame count after interface enable */
   uint32            uCurrentLineCnt;                    /**< Current line count of the frame            */

} HAL_MDP_Interface_FrameRefreshInfoType;

/*!
 * \struct HAL_MDP_Interface_EngineStatusType
 *
 * The \b HAL_MDP_Interface_EngineStatusType returns the state of timing generator engine.
 * Valid for the active refresh display interfaces only.
 */
typedef struct _HAL_MDP_Interface_EngineStatusType
{
   bool32            bIsEngineOn;                        /**< Interface engine on/off state              */

} HAL_MDP_Interface_EngineStatusType;

/*!
 * \struct HAL_MDP_Interface_DualInterfaceConfigType
 *
 * The \b HAL_MDP_Interface_DualInterfaceConfigType specify the dual interface (split display) configuration.
 */
typedef struct _HAL_MDP_Interface_DualInferfaceConfigType
{
  bool32                      bEnableDualInterface;     /**< TRUE, enable dual interface (split display) mode    */
  HAL_MDP_InterfaceModeType   eInterfaceMode;           /**< Interface type                                      */
  bool32                      bEnablePingPongSplit;     /**< TRUE, enable pingpong split                         */
  uint32                      uCmdModePixelAlignment;   /**< 0:No alignment, 1:Align at start of frame, 2:Align at start of line */
} HAL_MDP_Interface_DualInferfaceConfigType;

/*!
 * \struct HAL_MDP_Interface_ProgFetchStartConfigType
 *
 * The \b HAL_MDP_Interface_ProgFetchStartConfigType specifies the info for programmable fetch start for a interface.
 */
typedef struct _HAL_MDP_Interface_ProgFetchStartConfigType
{
  bool32 bProgFetchStartEnable;                         /**< Enable field to indicate whether programmable fetch start is enabled */
  uint32 uProgFetchStartValue;                          /**< Value to be programmed. Measured in terms of #Lines*HSyncPeriod. 
                                                             Note: #Lines must be >= VActive+VBP+VSP and < VActive+VBP+VSP+VFP*/
} HAL_MDP_Interface_ProgFetchStartConfigType;

typedef struct _HAL_MDP_Interface_SplitDisplayType
{
  bool32                      bEnableSplitDisplay;
  HAL_MDP_InterfaceModeType   eInterfaceMode;

} HAL_MDP_Interface_SplitDisplayType;


/*!
 * \struct HAL_MDP_InterfaceConfigType
 *
 * The \b HAL_MDP_InterfaceConfigType provides various data structures to configure
 * the MDP interface (INTF_x) module.
 */
typedef struct _HAL_MDP_InterfaceConfigType
{
   HAL_MDP_PhyIntf_TimingGenConfigType       *pIntrTimingGenConfig;     /**< Configures the timing generator of the interface */
   HAL_MDP_Interface_MISRConfigType          *pIntrMISRConfig;          /**< MISR/CRC block configuration                     */
   HAL_MDP_Interface_TPGConfigType           *pIntrTPGConfig;           /**< Test Pattern Generator configuration             */
   HAL_MDP_Interface_SplitDisplayType        *pSplitDisplayConfig;      /**< Split display configuration                      */
   HAL_MDP_Interface_DualInferfaceConfigType  *pDualInterfaceConfig;      /**< dual interface (Split display) configuration                         */
   HAL_MDP_ChromaDownModuleId                 eChromaDownModuleId;      /**< Chroma down module Id for INTF_3 path, for YUV pixel formats support */
   HAL_MDP_ChromaDownModuleConfigType        *pChromaDownConfig;        /**< Chroma down block configuration                                      */
   HAL_MDP_Interface_ProgFetchStartConfigType *pIntfProgFetchStartConfig; /**< Programmable Fetch Start Info                                        */ 
} HAL_MDP_InterfaceConfigType;

/*!
 * \struct HAL_MDP_PingPongConfigType
 *
 * The \b HAL_MDP_PingPongConfigType provides various data structures to configure
 * the Ping-Pong module (TE module configuration for command-mode/smart panels)
 */
typedef struct _HAL_MDP_PingPongConfigType
{
   HAL_MDP_PingPong_VsyncEnableType         *pIntrVsyncEnable;       /**< vsync counter enable              */
   HAL_MDP_PingPong_TEEnableType            *pIntrTEEnable;          /**< tear check enable                 */
   HAL_MDP_PingPong_AutoRefreshEnableType   *pIntrAutoRefreshEnable; /**< auto-refresh enable               */
   HAL_MDP_PingPong_VsyncConfigType         *pIntrVsyncConfig;       /**< vsync config                      */
   HAL_MDP_PingPong_TEConfigType            *pIntrTEConfig;          /**< tear check config                 */
   HAL_MDP_PingPong_BufferConfigType        *pPingPongBufConfig;     /**< SplitFifo (PPB) config            */
   HAL_MDP_PingPong_DSCConfigType           *pDSCConfig;             /**< Display Stream Compression config */
   HAL_MDP_PingPong_DitherConfigType        *pDitherConfig;          /**< DSPP Dither configuration         */
} HAL_MDP_PingPongConfigType;

/*!
 * \struct HAL_MDP_SplitMUXConfigType
 *
 * The \b HAL_MDP_SplitMUXConfigType provides various data structures to configure
 * the split-muxer (PPB - PingPong Buffer) module
 */
typedef struct _HAL_MDP_SplitMUXConfigType
{
   HAL_MDP_SplitMUX_BufferConfigType       *pSplitMUXBufferConfig;   /** Split-MUX Buffer configuration     */

} HAL_MDP_SplitMUXConfigType;

/*!
 * \struct HAL_MDP_DSCConfigType
 *
 * The \b HAL_MDP_DSCConfigType provides various data structures to configure
 * the Display Stream Compression module
 */
typedef struct _HAL_MDP_DSCConfigType
{
   HAL_MDP_DSC_EncoderConfigType            *pDSCEncoderCfg;

} HAL_MDP_DSCConfigType;

/*!
 * \struct HAL_MDP_DSC_EncoderPacketType
 *
 * The \b HAL_MDP_DSC_EncoderPacketType holds the parameters used for DSC encoder.
 */
typedef struct _HAL_MDP_DSC_EncoderPacketType
{
   HAL_MDP_DSC_EncParamsType          *pEncoderParams;   /**< Encoder parameters                                        */
   uint8                              *pPacketData;      /**< Encoder parameters packet in 128 Byte Packet (PPS) format */

} HAL_MDP_DSC_EncoderPacketType;

/*!
 * \struct HAL_MDP_DSCInfoType
 *
 * The \b HAL_MDP_DSCInfoType outputs the requested information related to DSC configurations
 */
typedef struct _HAL_MDP_DSCInfoType
{
   HAL_MDP_DSC_EncoderConfigType            *pDSCEncoderCfg;      /**< Input parameters to return the encoder parameter    
                                                                       information in pDSCEncoderPacket                    */
   HAL_MDP_DSC_EncoderPacketType            *pDSCEncoderPacket;   /**< Output parameters holds the encoder parameters and
                                                                       encoder parameters packed in 128-Byte PPS format    */

} HAL_MDP_DSCInfoType;

/*
 * HAL_MDP_RegisterType
 *
 *    The \b HAL_MDP_RegisterType is used to read/write a particular MDP register (only for testing purposes)
 */
typedef struct _HAL_MDP_RegisterType
{
  uint32      uOffset;                    /**< The offset value of the register from MDP_REG_BASE           */
  uint32      uValue;                     /**< Value to be written or value return after the register read  */
} HAL_MDP_RegisterType;

/*
 * HAL_MDP_PA_MemoryColorConfigType
 *
 */
typedef struct _HAL_MDP_PA_MemoryColorConfigType
{
  bool32           bEnable;                     /**< Enable/Disable this tone adjustment */
  uint32           uHueMax;                     /**< Region definition for tone adjustment - maximum hue */
  uint32           uHueMin;                     /**< Region definition for tone adjustment - minimum hue */
  uint32           uSaturationMax;              /**< Region definition for tone adjustment - maximum saturation */
  uint32           uSaturationMin;              /**< Region definition for tone adjustment - minimum saturation */
  uint32           uValueMax;                   /**< Region definition for tone adjustment - maximum value */
  uint32           uValueMin;                   /**< Region definition for tone adjustment - minimum value */
  int32            iHueGain;                    /**< Hue gain for tone adjustment, defines the slope of the 2 sides of symetrical trapezoidal curve */
  uint32           uHueMid;                     /**< Offset from the min which marks the top 2 corners of the trapezoid */
  int32            iValueGain;                  /**< Value gain for tone adjustment, defines the slope of the 2 sides of the symetical trapezoid */
  int32            iSaturationGain;             /**< Saturation gain for tone adjustment, defines the slope of the 2 sides of the symetrical trapezoid */
} HAL_MDP_PA_MemoryColorConfigType;

/*
 * HAL_MDP_PA_SixZonePwlType
 *
 */
typedef struct
{
  int32            iHue;          /**< Hue adjustment of PWL(piece-wise-linear curve) in HSV color space */
  int32            iValue;        /**< Value adjustment of PWL(piece-wise-linear curve) in HSV color space */
  int32            iSaturation;   /**< Saturation adjustment of PWL(piece-wise-linear curve) in HSV color space */
} HAL_MDP_PA_SixZonePwlType;

/*
 * HAL_MDP_PA_SixZoneConfigType
 *
 */
typedef struct
{
  bool32                      bHueEnable;                           /**< Enable/disable hue for six-zone adjustment */
  bool32                      bValueEnable;                         /**< Enable/disable value for six-zone adjustment */
  bool32                      bSaturationEnable;                    /**< Enable/disable saturation for six-zone adjustment */
  uint32                      uSaturationMin;                       /**< Minimum saturation threshold to qualify the adjustment */
  uint32                      uValueMin;                            /**< Minimum value threshold to qualify the adjustment */
  uint32                      uValueMax;                            /**< Maximum value threshold to qualify the adjustment */
  HAL_MDP_PA_SixZonePwlType   sPwl[HAL_MDPP_SIX_ZONE_LUT_ENTRIES];  /**< PWL for six-zone adjustment */
} HAL_MDP_PA_SixZoneConfigType;

/*
 * HAL_MDP_PictureAdjustConfigType
 *
 *    The \b HAL_MDP_PictureAdjstConfigType is used to configure HSIC parameters of SSPP and DSPP modules
 */
typedef struct _HAL_MDP_PictureAdjustConfigType
{
   bool32     bEnable;                        /**< Control flag */
   int32      iHueValue;                      /**< Hue          */
   int32      iSaturationValue;               /**< Saturation   */
   int32      iIntensityValue;                /**< Intensity    */
   int32      iContrastValue;                 /**< Contrast     */
} HAL_MDP_PictureAdjustConfigType;

/*
 * HISTOGRAM BLOCK
 *
 *    The following data structures provide a way to initialize/read/write/swap LUTs of the histogram modules present
 *    in both SSPP and DSPP 
 *
 *    (a) HAL_MDP_HistrogramInitType      --> Used to enable/initialize the histrogram block
 *    (b) HAL_MDP_HistrogramTriggerType   --> Used to turn ON/OFF the histrogram collection opertaions
 *    (c) HAL_MDP_HistrogramLUTType       --> Used to configure the histogram LUTs and to swap the LUT buffers
 *    (d) HAL_MDP_HistogramConfigType     --> a global data structure that allows to configure (a), (b) and (c), explained above
 *    (e) HAL_MDP_HistogramReadType       --> Used to read the histogram LUTs
 */

typedef struct _HAL_MDP_HistrogramInitType
{
   bool32      bEnable;                               /**< Enable/disable the histogram feature                                  */
   uint32      uFrameCnt;                             /**< Set frame count for histogram generation. Valid only bEnable = TRUE   */
   bool32      bEnableAutoClear;                      /**< Auto clear the bins after read operation. Valid only bEnable = TRUE   */

} HAL_MDP_HistrogramInitType;

/*
 * HAL_MDP_HistrogramTriggerType
 *
 *  bTurnOn and bReset are mutually exclusive, if both are true then the reset is triggered
 *
 */
typedef struct _HAL_MDP_HistrogramTriggerType
{
   bool32      bTurnOn;                               /**< Turn ON/OFF Histogram generation */
   bool32      bReset;                                /**< True to reset the histogram      */
   bool32      bWaitResetDone;                        /**< Output to the client to tell whether or not wait for histogram reset done interrupt is needed */
} HAL_MDP_HistrogramTriggerType;

/*
 * HAL_MDP_HistrogramLUTType
 *
 */
typedef struct _HAL_MDP_HistrogramLUTType
{
   bool32      bEnable;                               /**< Enable usage of LUT table (pixel mapping)                                                             */
   bool32      bSwap;                                 /**< SWAP the LUT table used by software and hardware                                                      */
   bool32      bConfigLUT;                            /**< Program the LUT table                                                                                 */
   uint32     *puLUTData;                             /**< Gamma LUT data. Only Lower 8-bits are valid. Should point to a buffer with HAL_MDP_GAMMA_LUT_ENTRIES  */
   
} HAL_MDP_HistrogramLUTType;

/*
 * HAL_MDP_HistogramConfigType
 *
 */
typedef struct _HAL_MDP_HistogramConfigType
{
   HAL_MDP_HistrogramInitType    *pHistInitConfig;     /**< To initialize histogram collection block         */
   HAL_MDP_HistrogramTriggerType *pHistTriggerConfig;  /**< To trigger/reset the histogram collection block  */
   HAL_MDP_HistrogramLUTType     *pHistLUTConfig;      /**< To write and swap the histogram LUTs             */

} HAL_MDP_HistogramConfigType;

/*
 * HAL_MDP_HistogramDataType
 *
 */
typedef struct _HAL_MDP_HistogramReadType
{
   uint32     *puHistData;                            /**< Histogram data. Should point to buffer with HAL_MDP_HISTOGRAM_LUT_ENTRIES 32-bit entries   */

} HAL_MDP_HistogramReadType;


/*
 * HAL_MDP_GammaCorrectConfigType
 *
 */
typedef struct _HAL_MDP_GammaCorrectConfigType
{
   bool32                        bEnable;          /**< Turns ON/OFF Gamma Correction Feature        */
   uint32                        uSegmentSize;     /**< Size of each gamma correction segment        */
   HAL_MDP_AreaReductionLutType  *psAR_LUT_Red;    /**< AR GC LUT Red-Component                      */
   HAL_MDP_AreaReductionLutType  *psAR_LUT_Green;  /**< AR GC LUT Green-Component                    */
   HAL_MDP_AreaReductionLutType  *psAR_LUT_Blue;   /**< AR GC LUT Blue-Component                     */

} HAL_MDP_GammaCorrectConfigType;


/*
 * HAL_MDP_Linear_GammaCorrectConfigType
 *
 */
typedef struct _HAL_MDP_Linear_GammaCorrectConfigType
{
   bool32      bEnable;             /**< Enable usage of LUT table (pixel mapping)                                      */
   bool32      bSwap;               /**< SWAP the LUT table used by software and hardware                               */
   bool32      bConfigLUT;          /**< Program the LUT table                                                          */
   uint32      uNumLUTEntries;      /**< number of gamma LUT entries                                                */
   uint32     *puLUTData;           /**< Gamma LUT data. Should point to a buffer with HAL_MDP_GAMMA_LUT_ENTRIES    */
   uint16     *puRedLUTData;        /**< Pointer to gamma LUT data for R. */
   uint16     *puGreenLUTData;      /**< Pointer to gamma LUT data for G. */
   uint16     *puBlueLUTData;       /**< Pointer to gamma LUT data for B. */
} HAL_MDP_Linear_GammaCorrectConfigType; 


/*
 * HAL_MDP_PolynomialColorCorrectCoefficientType
 *
 *
 * Don't change the order of elements in HAL_MDP_PolynomialColorCorrectCoefficientType structure. OEM expects that the
 * color co-efficients are in the following order mentioned. Any changes to this structure will result in unpredictable 
 * and undesirable results. 
 *
 * Refer the software register specification for more information about the co-efficient formats.
 *
 * Rcie = a01 + a11 Rd + a21 Gd + a31 Bd + a41 RdGd + a51 GdBd + a61 BdRd + a71 R2d + a81 G2d + a91 B2d + a101 RdGdBd_0 + a111 RdGdBd_1
 * Gcie = a02 + a12 Rd + a22 Gd + a32 Bd + a42 RdGd + a52 GdBd + a62 BdRd + a72 R2d + a82 G2d + a92 B2d + a102 RdGdBd_0 + a112 RdGdBd_1
 * Bcie = a03 + a13 Rd + a23 Gd + a33 Bd + a43 RdGd + a53 GdBd + a63 BdRd + a73 R2d + a83 G2d + a93 B2d + a103 RdGdBd_0 + a113 RdGdBd_1
 *
 */
typedef struct _HAL_MDP_PolynomialColorCorrectCoefficientType
{
   uint32   uConstant;         /**< S13.3 Format */
   uint32   uRed;              /**< S2.15 Format */
   uint32   uGreen;            /**< S2.15 Format */
   uint32   uBlue;             /**< S2.15 Format */
   uint32   uRedGreen;         /**< S1.27 Format */
   uint32   uGreenBlue;        /**< S1.27 Format */
   uint32   uBlueRed;          /**< S1.27 Format */
   uint32   uRedRed;           /**< S1.27 Format */
   uint32   uGreenGreen;       /**< S1.27 Format */
   uint32   uBlueBlue;         /**< S1.27 Format */
   uint32   uRedGreenBlue_0;   /**< S1.39 Format */
   uint32   uRedGreenBlue_1;   /**< S1.39 Format */

} HAL_MDP_PolynomialColorCorrectCoefficientType;

/*
 * HAL_MDP_PolynomialColorCorrect2CoefficientType
 *
 * Data structure used to pass the PCC2 type configuration parameters to HAL driver
 */
typedef struct _HAL_MDP_PolynomialColorCorrect2CoefficientType
{
   uint32   uConstant;           /** S13.3 Format  */
   uint32   uRed;                /** S15.3 Format  */
   uint32   uGreen;              /** S15.3 Format  */
   uint32   uBlue;               /** S15.3 Format  */
   uint32   uRedGreen;           /** S15.3 Format  */
   uint32   uRedBlue;            /** S15.3 Format  */
   uint32   uBlueGreen;          /** S15.3 Format  */
   uint32   uRedBlueGreen;       /** S15.3 Format  */
} HAL_MDP_PolynomialColorCorrect2CoefficientType;

/*
 * HAL_MDP_PolynomialColorCorrectConfigType
 *
 *    Data structure provides parameters to configure the PCC coefficients of R,G and B color components
 *    and provides a control flag to enable/disable the PCC module
 */
typedef struct _HAL_MDP_PolynomialColorCorrectConfigType
{
   bool32                                          bEnable;         /**< Controls enable/disable the feature            */
   HAL_MDP_PolynomialColorCorrectCoefficientType   *psCC_Red;       /**< Color correction co-efficients Red-component   */ 
   HAL_MDP_PolynomialColorCorrectCoefficientType   *psCC_Green;     /**< Color correction co-efficients Green-component */
   HAL_MDP_PolynomialColorCorrectCoefficientType   *psCC_Blue;      /**< Color correction co-efficients Blue-component  */
} HAL_MDP_PolynomialColorCorrectConfigType;

/*
 * HAL_MDP_PolynomialColorCorrect2ConfigType
 *
 *    Data structure provides parameters to configure the PCC2 coefficients of R,G and B color components
 *    and provides a control flag to enable/disable the PCC2 module
 */
typedef struct _HAL_MDP_PolynomialColorCorrect2ConfigType
{
   bool32                                            bEnable;        /**< Controls enable/disable the feature            */
   HAL_MDP_PolynomialColorCorrect2CoefficientType   *psCC_Red;       /**< Color correction co-efficients Red-component   */ 
   HAL_MDP_PolynomialColorCorrect2CoefficientType   *psCC_Green;     /**< Color correction co-efficients Green-component */
   HAL_MDP_PolynomialColorCorrect2CoefficientType   *psCC_Blue;      /**< Color correction co-efficients Blue-component  */
} HAL_MDP_PolynomialColorCorrect2ConfigType;


/*
 * HAL_MDP_SourcePipeInfoParamsType
 *
 */
typedef union _HAL_MDP_SourcePipeInfoParamsType
{
   HAL_MDP_HistogramReadType        *psHistogramData;

} HAL_MDP_SourcePipeInfoParamsType;

/*
 * HAL_MDP_DestinationPipeInfoParamsType
 *
 *    Data structure to read the information/configured-data of DSPP block using HAL_MDP_DestinationPipe_GetProperty()
 */
typedef struct _HAL_MDP_DestinationPipeInfoParamsType
{
   HAL_MDP_HistogramReadType   *psDsppHistData;          /**< Histogram data read                                 */
   bool32                      *pbActive;                /**< Extract the curent state (active/idle) of the DSPP  */

} HAL_MDP_DestinationPipeInfoParamsType;

/*
 * HAL_MDP_InterfaceInfoParamsType
 *
 * Data structure to read the information/configured-data of the interface (INTF_x) module
 */
typedef struct _HAL_MDP_InterfaceInfoParamsType
{
   HAL_MDP_Interface_MISRReturnType           *pMISRReturnType;        /**< MISR return types                                          */
   HAL_MDP_Interface_FrameRefreshInfoType     *pFrameRefreshInfo;      /**< Interface line/frame count                                 */
   HAL_MDP_Interface_EngineStatusType         *pEngineStatus;          /**< Interface timing-generator engine status                   */
   HAL_MDP_PhyIntf_TimingGenConfigType        *pTimingGenInfo;         /**< Interface timing-generator configuration parameters        */
   bool32                                     *pbActive;               /**< Extract the current state (active/idle) of interface       */
   bool32                                     *pbDualInterfaceEnabled; /**< State of dual interface is turned on                       */
} HAL_MDP_InterfaceInfoParamsType;

/*
 * PING-PONG Module 
 *
 *    The following set of data structures are read the various configuration/status of the Ping-Pong module.
 *    The data structures are valid only for the command-mode/smart panels
 *
 *    (a) HAL_MDP_PingPong_FrameRefreshInfoType --> Used to read the current frame-count/line-count refresh of command mode panel
 *    (b) HAL_MDP_PingPong_EngineStatusType     --> Used to read the ping-pong engine status
 *    (c) HAL_MDP_PingPongInfoParamsType        --> Global strucutre to read one or more configuration/status information of the ping-pong module
 */
typedef struct _HAL_MDP_PingPong_FrameRefreshInfoType
{
   uint32            uCurrentFrameCnt;                   /**< Current Frame count after interface enable */
   uint32            uCurrentLineCnt;                    /**< Current line count of the frame            */
} HAL_MDP_PingPong_FrameRefreshInfoType;

/*
 * HAL_MDP_PingPong_EngineStatusType
 *
 */
typedef struct _HAL_MDP_PingPong_EngineStatusType
{
   bool32            bIsEngineOn;                        /**< pingpong engine on/off state              */
} HAL_MDP_PingPong_EngineStatusType;

/*
 * HAL_MDP_PingPongInfoParamsType
 *
 */
typedef struct _HAL_MDP_PingPongInfoParamsType
{
   HAL_MDP_PingPong_FrameRefreshInfoType    *pFrameRefreshInfo;       /**< Ping-Pong module's line/frame refresh count details            */
   HAL_MDP_PingPong_EngineStatusType        *pEngineStatus;           /**< Ping-Pong module's engine status                               */
   bool32                                   *pbActive;                /**< Extract the curent state (active/idle) of the ping-pong module */

} HAL_MDP_PingPongInfoParamsType;

/*
 * HAL_MDP_GamutMappingConfigType
 * Panel gamut mapping table for three color components.
 */
typedef struct _HAL_MDP_GamutMappingConfigType
{
   bool32  bGamutEn;                 /**< Flag to enable/disable gamut mapping.                      */
   bool32  bGamutBeforePCC;

   /**< Flag to indicate the order of gamut mapping and polynomial color correction. 
        True  - Gamut mapping followed by polynomial correction 
        False - Polynomial correction followed by gamut mapping 
        Reserved for now - By default, gamut mapping is done prior to color correction */
   uint16  *pGammutTableEntries[HAL_MDP_GM_MAX_COMPONENTS][HAL_MDP_GM_MAX_COMPONENT_TABLES];
} HAL_MDP_GamutMappingConfigType;

/*
 * HAL_MDP_WideGamutMappingConfigType 
 * Panel Wide gamut mapping table for three color components.
 */
typedef struct _HAL_MDP_WideGamutMappingConfigType
{
   bool32   bWdGamutEn;                                                                                   /**< Flag to enable/disable gamut mapping.  */
   bool32   bWdGamutMapEn;                                                                                /**< Flag to enable/disable Non uniform map.*/
   bool32   bWdGamutCoarseModeEn;                                                                         /**< Flag to enable/disable Coarse mode.    */
   uint16  *pWdGammutTableEntries[HAL_MDP_WGM_MAX_COLOR_COMPONENTS][HAL_MDP_WGM_MAX_COMPONENT_TABLES];    /**< 3d gamut mapping tables' entries.      */
   uint32  *pWdNonUniformMapTableEntries[HAL_MDP_WGM_MAX_COLOR_COMPONENTS];                               /**< segment table entries.                 */
} HAL_MDP_WideGamutMappingConfigType;



/*
 * HAL_MDP_PixelExtInfo holds information for calculating 
 * SW Pixel Extension override Regs
 */
typedef struct _HAL_MDP_PixelExtOverrideConfigType
{
   int32                    iLeftOverFetch;          /**< Signed value for left overfetch value, + indicates overfetch, - indicates pixel drop */
   int32                    uLeftRepeat;             /**< Unsigned value for left pixel repeat value */ 
   int32                    iRightOverFetch;         /**< Signed value for right overfetch value, + indicates overfetch, - indicates pixel drop */
   int32                    uRightRepeat;            /**< Unsigned value for right pixel repeat value */
   int32                    iTopOverFetch;           /**< Signed value for top overfetech value, + indicates overfetch, - value indicates line drop */
   int32                    uTopRepeat;              /**< Unsigned value for TOP line repeat value */
   int32                    iBottomOverFetch;        /**< Signed value for bottom overfetch value + indicates overfetech, - indicates line drop */
   int32                    uBottomRepeat;           /**< Unsigned value for bottom repeat value */
   int32                    uTopBottomReqPxls;       /**< Unsigned value for pixel height post pixel extension */
   int32                    uLeftRightReqPxls;       /**< Unsigned value for pixel width post pixel extension */
} HAL_MDP_PixelExtOverrideConfigType;

/**
 *  =================================================================================================
 *  HAL MDP Utility Types
 *  =================================================================================================
 */
/* 
 * HAL_MDP_UTILITY_PipeConfigType defines essential parameters for system team's libScale functions 
 */
typedef struct _HAL_MDP_UTILITY_PipeConfigType
{
   HAL_MDP_SurfaceAttrType                       *psSurface;             /**< Surface that is going to be attached to pipe(s) */
   HAL_MDP_LayerType                              eLayerType;            /**< Pipe Type, only RGB and ViG are supported */
   HAL_MDP_RectType                               sRoiInRect;            /**< Source rectangle referring to the surface */
   HAL_MDP_RectType                               sRoiOutRectLeft;       /**< Destination rectangle on left panel, x and y are not used */
   HAL_MDP_RectType                               sRoiOutRectRight;      /**< Destination rectangle on right panel, x and y are not used, set width to 0 in the single pipe case */
   HAL_MDP_RotateType                             eRotateType;           /**< Rotation configuration */
   HAL_MDP_FetchConfigType                        eFetchConfig;          /**< Flip configuration */
   bool32                                         bEnableDecimation;     /**< Whether decimation is enabled or not */
   HAL_MDP_InterfaceConnectType                   sInterfaceConnectType; /**< Specify the type of the interface, i.e. Physical or Writeback. Not care in dual pipe case */
   HAL_MDP_InterfaceModeType                      sInterfaceModeType;    /**< Specify the mode of the interface, i.e. Video mode or Command mode. Not care in dual pipe case*/
   HAL_MDP_SharpenConfigType                      sSharpenConfig;        /**< Sharpen configuration */
   HAL_MDP_UTILITY_optimization_mode              eOptimizationMode;     /**< Quality/Power or ballanced scaler optimization */
   HAL_MDP_ScalarStateType                        aScalarParams[HAL_MDP_MAX_SSPP_IN_DUALPIPE_MODE]; /**< Scaler specific settings uploaded to HW */

} HAL_MDP_UTILITY_PipeConfigType;

/* 
 * HAL_MDP_PixelExtLayerConfigType defines scaling filter and pixel extension values per components 
 */
typedef struct _HAL_MDP_PixelExtComponentConfigType
{
   HAL_MDP_ScalerPlaneType                        sScalerPlane;            /**< Scaling filter for each component */
   HAL_MDP_PixelExtOverrideConfigType             sPixelExtOverrideConfig; /**< Pixel extension values */
}HAL_MDP_PixelExtComponentConfigType;

/* 
 * HAL_MDP_PixelExtLayerConfigType defines source rectangle, decimation config per layer, and the corresponding pixel extension config for four components
 */
typedef struct _HAL_MDP_PixelExtLayerConfigType
{
   HAL_MDP_RectType                               sRoiInRect;                                                         /**< Source rectangles for each pipe.*/
   HAL_MDP_RectType                               sRoiOutRect;                                                        /**< Destination rectangles for each pipe.*/
   HAL_MDP_DecimationConfigType                   sDecimationConfig;                                                  /**< Decimation config if decimation is enabled */
   HAL_MDP_ScalarConfigType                       sScalarConfig;                                                      /**< Scalar settings */
   HAL_MDP_PixelExtComponentConfigType            aPixelExtComponentConfig[HAL_MDP_SOURCE_PIPE_MAX_COLOR_COMPONENTS]; /**< Scaling filter and pixel extension values for four components */
   bool32                                         bEnabled;                                                           /**< Enable pixel extension config for this layer */
}HAL_MDP_PixelExtLayerConfigType;

/* 
 * HAL_MDP_PixelExtConfigType defines pixel extension config, 2 elements are defined for dual pipe case (e.g. 4kx2k large resolution)
 * in single pipe case, the 2nd element in aPixelExtLayerConfig does not take effect 
 */
typedef struct _HAL_MDP_PixelExtConfigType
{
   HAL_MDP_PixelExtLayerConfigType aPixelExtLayerConfig[HAL_MDP_MAX_SSPP_IN_DUALPIPE_MODE]; /**< Pixel extension config on each pipe */
}HAL_MDP_PixelExtConfigType;

/* 
 * HAL_MDP_UTILITY_DisplayConfigType defines essential parameters for system team's libScale functions 
 */
typedef struct _HAL_MDP_UTILITY_DisplayConfigType
{
   HAL_MDP_RectType                               sRoiInRect;            /**< Layer Mixer width/Height (for dual display case Layer mixer width is sum of right and left layer mixers)*/
   HAL_MDP_RectType                               sRoiOutRectLeft;       /**< Panel resolution, (left side output resolution in dual display case) */
   HAL_MDP_RectType                               sRoiOutRectRight;      /**< Right side output resolution, set width to 0 in the single display case */
   HAL_MDP_SharpenConfigType                      sSharpenConfig;        /**< Sharpen configuration */
   HAL_MDP_UTILITY_optimization_mode              eOptimizationMode;     /**< Quality/Power or ballanced scaler optimization */
} HAL_MDP_UTILITY_DisplayConfigType;


/**
 *  =================================================================================================
 *  Layer Mixer Configuration
 *  =================================================================================================
 */

/*==================================================================================================================*/

/**
 *  =================================================================================================
 *  Control Path Configuration
 *  =================================================================================================
 */
/*!
 * \struct HAL_MDP_ControlPath_Mixer_ZOrderConfigType
 *
 * Structure definition for source pipe Blend order configuration in dual pipe mode
 */
typedef struct _HAL_MDP_ControlPath_Mixer_ZOrderConfigType
{
   HAL_MDP_SourcePipeId eSourcePipe[HAL_MDP_BLEND_ORDER_STAGE_MAX][HAL_MDP_MAX_SSPP_IN_DUALPIPE_MODE];

} HAL_MDP_ControlPath_Mixer_ZOrderConfigType;

/*!
 * \struct HAL_MDP_ControlPath_Mixer_CursorConfigType
 *
 * Structure definition for configuring cursor
 */
typedef struct _HAL_MDP_ControlPath_Mixer_CursorConfigType
{
   bool32                  bCursorEnable;       /**< Enable/Disable the cursor */
   HAL_MDP_SourcePipeId    eCursorId;           /**< Cursor Source-Pipe ID     */
   HAL_MDP_BlendOrderId    eCursorBlendOrderId; /**< Cursor blend order id     */

} HAL_MDP_ControlPath_Mixer_CursorConfigType;

/*!
 * \struct HAL_MDP_ControlPath_Mixer_BorderColorConfigType
 *
 * Structure definition for enabling/disabling border color
 */
typedef struct _HAL_MDP_ControlPath_Mixer_BorderColorConfigType
{
   bool32      bBorderColorEnable;     /**< Enable/Disable the border color feature  */

} HAL_MDP_ControlPath_Mixer_BorderColorConfigType;

/*!
 * \struct HAL_MDP_ControlPath_MixerConfigType
 *
 * Structure definition for configuring layer mixer
 */
typedef struct _HAL_MDP_ControlPath_MixerConfigType
{
   HAL_MDP_LayerMixerId                             eMixerId;
   HAL_MDP_ControlPath_Mixer_ZOrderConfigType      *psMixerZOrderConfig;
   HAL_MDP_ControlPath_Mixer_CursorConfigType      *psMixerCursorConfig;
   HAL_MDP_ControlPath_Mixer_BorderColorConfigType *psMixerBorderColorConfig;

} HAL_MDP_ControlPath_MixerConfigType;

/*!
 * \struct HAL_MDP_GpuMemoryBankConfigType
 *
 * Structure definition for configuring commit mode
 */
typedef struct _HAL_MDP_ControlPath_InterfaceConfigType
{
   HAL_MDP_InterfaceId           eInterfaceId;
   HAL_MDP_InterfaceModeType     eInterfaceModeType;
   bool32                        b3DMuxEnable;
   HAL_MDP_3DMUXPackerModeType   eMuxPackerModeConfig;

} HAL_MDP_ControlPath_InterfaceConfigType;

/*!
 * \struct HAL_MDP_GpuMemoryBankConfigType
 *
 * Structure definition for configuring commit mode
 */
typedef struct _HAL_MDP_ControlPath_CommitModeConfigType
{
   HAL_MDP_CommitModeType eCommitMode;

} HAL_MDP_ControlPath_CommitModeConfigType;

/*!
 * \struct HAL_MDP_ControlPathConfigType
 *
 * Structure definition for configuring control path with
 * different modules for ex mixer, interface, commit mode etc
 */
typedef struct _HAL_MDP_ControlPathConfigType
{
   HAL_MDP_ControlPath_MixerConfigType       *psMixerConfig;
   HAL_MDP_ControlPath_InterfaceConfigType   *psInterfaceConfig;
   HAL_MDP_LayerMixerConfigType              *psLayerMixerConfig;
   HAL_MDP_ControlPath_CommitModeConfigType  *psCommitModeConfig;

} HAL_MDP_ControlPathConfigType;

/*!
 * \struct HAL_MDP_ControlPathFlushType
 *
 * Structure definition for specifying flush modules
 */
typedef struct _HAL_MDP_ControlPathFlushType
{
   uint32      uFlushModules;

} HAL_MDP_ControlPathFlushType;

/*!
 * \struct HAL_MDP_GpuMemoryBankConfigType
 *
 * Controls the GPU highest memory bank bit used
 * Controls the GPU Macro Tile mode/Address gen mode
 */
typedef struct _HAL_MDP_GpuMemoryBankConfigType
{
   uint32      uGpuHighestMemoryBankBit;         /**< GPU highest memory bank bit */
   uint32      uGpuMacroTileMode;                /**< GPU Macro Tile Mode: 0:A420, 1:A430*/

} HAL_MDP_GpuMemoryBankConfigType;

/*!
 * \struct HAL_MDP_MultiRectConfigType
 *
 * Configures multi-rec mode
 */
typedef struct _HAL_MDP_MultiRectConfigType
{
   bool32                        bEnable;        /**< Enable/Disable multi-rec mode */
   HAL_MDP_MultiRectModeType     eMultiRectType; /**< Specifies multi-rec mode type*/

} HAL_MDP_MultiRectConfigType;


/*==================================================================================================================*/

/**
 *  =================================================================================================
 *  Data Path Configuration
 *  =================================================================================================
 */

/*!
 * \struct HAL_MDP_RotateConfigType
 * 
 * The \b HAL_MDP_RotateConfigType configures the block-mode rotation angle.
 */
typedef struct _HAL_MDP_RotateConfigType
{
   bool32                          bRotateEnable;         /**< Enable/Disable block-mode operation (Rotator) */
   HAL_MDP_RotateType              eRotateType;           /**< Rotate Angle if block-mode is enabled         */
   HAL_MDP_CompressionModeType     eCompressionMode;      /**< Compression mode for bandwidth compression    */

} HAL_MDP_RotateConfigType;

/*!
 * \struct HAL_MDP_DataPath_SurfaceConfigType
 * 
 * The \b HAL_MDP_DataPath_SurfaceConfigType data structure provides the surface configuration
 * details for write-back path.
 *
 * psOutSurfaceConfig - Holds the surface address, pixel format, width, height and stride informations.
 * psOutputRectConfig - Active rectangle in the surface. Cord position always starts with (0,0).
 *                      The width and height should be provided even active rectangle is same as surface width and height.
 * psConstAlphaConfig - Const alpha configuration
 * psScalarConfig     - Holds the scalar configuration information
 *
 * The output surface pixel formats are: RGB565, RGB888, XRGB8888 and YUV420 (NV12). The client shouldn't pass unsupported pixel formats
 * 
 * uConstAlphaValue variable is valid only for XRGB8888 format. HW embeds the X with the given uConstAlphaValue during write-back.
 * This is valid only for line-mode writeback and not for block-mode writeback (i.e. Rotator)
 *
 */
typedef struct _HAL_MDP_DataPath_SurfaceConfigType
{
   HAL_MDP_SurfaceAttrType          *psOutSurfaceConfig;    /**< Output surface attribute information                              */
   HAL_MDP_RectType                 *psOutputRectConfig;    /**< Active rectangle information in the surface. Cord is always (0,0) */
   HAL_MDP_ConstAlphaConfigType     *psConstAlphaConfig;    /**< Constant Alpha configuration information                          */
   HAL_MDP_ScalarConfigType         *psScalarConfig;        /**< Scalar (Upscale/Downscale) configuration information              */
} HAL_MDP_DataPath_SurfaceConfigType;

typedef struct _HAL_MDP_DataPathConfigType
{
   HAL_MDP_DataPath_ModeType            eDataPathMode;
   HAL_MDP_ChromaDownModuleId           eChromaDownModuleId;
   HAL_MDP_DataPath_SurfaceConfigType  *pSurfaceConfig;
   HAL_MDP_RotateConfigType            *psRotatorConfig;
   HAL_MDP_ColorEncodeConfigType       *psColorEncInfo;
   HAL_MDP_GpuMemoryBankConfigType     *psGpuMemoryBankConfig;
   HAL_MDP_ChromaDownModuleConfigType  *psChromaDownConfig;

} HAL_MDP_DataPathConfigType;

/* ==================================================================================================*/


typedef struct _HAL_MDP_DSPP_DitherConfigType
{
   bool32                     bEnable;            /**< Enable/Disable dithering feature                 */
   HAL_MDP_DitherMatrix      *pDitherMatrix;      /**< Dither Matrix                                    */
   HAL_MDP_PixelFormatType    eDstPixelFormat;    /**< Destination pixel format. DSPP Dither block input is always RGB888 format.
                                                       The output of dither block will routed to physical interface or linear write-back interface.
                                                       HAL driver enables the dither only if the output of dither module is lower color depth than RGB888.
                                                       For example, RGB565 or RGB666 etc.
                                                   */
   HAL_MDP_DitherBitDepth     sDitherBitDepth;    /**< Dither Bit depth for each color component  */
   HAL_MDP_DitherType         eDitherType;        /**< Dither type, spatial or temporal           */ 
} HAL_MDP_DSPP_DitherConfigType;


/*
 * HAL_MDP_DestinationPipeConfigType
 *
 * Define operations applied to the particular destination pipe.
 * When the pointer is null, the corresponding operation is ignored.
 * The caller should memset 0 to this structure, then fill the corresponding pointer.
 * The setting doesn't take effect until HAL_MDP_DataPath_Commit is executed
*/
typedef struct _HAL_MDP_DestinationPipeConfigType
{
   HAL_MDP_GammaCorrectConfigType              *psGammaCorrect;            /**< Gamma correction config */
   HAL_MDP_PolynomialColorCorrectConfigType    *psPolynomialColorCorrect;  /**< Polynomial color correction config */
   HAL_MDP_GamutMappingConfigType              *psGamutMapping;            /**< Gamut mapping config */
   HAL_MDP_DSPP_DitherConfigType               *psDither;                  /**< Dithering config */
   HAL_MDP_HistogramConfigType                 *psHistogramConfig;         /**< Histogram config */
   HAL_MDP_PictureAdjustConfigType             *psPictureAdjust;           /**< Picture adjustment (HSIC) config */
   HAL_MDP_IGCConfigType                       *psIGCConfig;               /**< Inverse gamma correct config */
   HAL_MDP_ADConfigType                        *psADConfig;                /**< ADC config */
   HAL_MDP_PA_MemoryColorConfigType            *psSkinColorConfig;         /**< Skin color adjustment config */
   HAL_MDP_PA_MemoryColorConfigType            *psSkyColorConfig;          /**< Sky color adjustment config */
   HAL_MDP_PA_MemoryColorConfigType            *psFoliageColorConfig;      /**< Foliage color adjustment config */
   HAL_MDP_PA_SixZoneConfigType                *psSixZoneColorConfig;      /**< Six-zone color adjustment config */
   HAL_MDP_Linear_GammaCorrectConfigType       *psLinearGammaCorrect;      /**< Linear Gamma Correction Information */
   HAL_MDP_WideGamutMappingConfigType          *psWideGamutMapping;        /**< Wide Gamut Mapping Information */ 
   HAL_MDP_PolynomialColorCorrect2ConfigType   *psPolynomialColorCorrect2; /**< Polynomial Color Correction 2 information  */
   HAL_MDP_ScalarConfigType                    *pScalarConfig;             /**< Destination scaler configuration*/
} HAL_MDP_DestinationPipeConfigType;

/*!
 * \struct HAL_MDP_SourcePipeConfigType
 *
 * The \b HAL_MDP_SourcePipeConfigType provides the parameters to configure the MDP Source Pipe/Layer
 *
 */
typedef struct _HAL_MDP_SourcePipeConfigType
{
   HAL_MDP_SurfaceAttrType                   *psSurfaceInfo;
   HAL_MDP_CropRectangleType                 *psCropRectInfo;
   HAL_MDP_DecimationConfigType              *psDecimationConfig;
   HAL_MDP_ColorFillConfigType               *psColorFillInfo;
   HAL_MDP_FetchModeConfigType               *psFetchInfo;
   HAL_MDP_IGCConfigType                     *psIGCInfo;
   HAL_MDP_ColorEncodeConfigType             *psColorEncInfo;
   HAL_MDP_SharpenConfigType                 *psSharpenInfo;
   HAL_MDP_HistogramConfigType               *psHistogramConfig;
   HAL_MDP_PictureAdjustConfigType           *psPictureAdjust;
   HAL_MDP_ImmediateFlipConfigType           *psImmediateFlip;
   HAL_MDP_VC1RangeLevelConfigType           *psVC1RangeInfo;
   HAL_MDP_DeinterlaceConfigType             *psDeinterlaceConfig;
   HAL_MDP_PixelExtLayerConfigType           *psPixelExtLayerConfig;
   HAL_MDP_GpuMemoryBankConfigType           *psGpuMemoryBankConfig;
   HAL_MDP_PolynomialColorCorrect2ConfigType *psPolynomialColorCorrect2; /**< Polynomial Color Correction 2 information  */
   HAL_MDP_MultiRectConfigType               *psMultiRectConfig;         /**< Multi rectangle configuration*/
} HAL_MDP_SourcePipeConfigType;

/*!
 * \struct HAL_MDP_SourcePipeInfoType
 *
 * The \b HAL_MDP_SourcePipeInfoType to query the information about the source pipe
 *
 */
typedef struct _HAL_MDP_SourcePipeInfoType
{
   HAL_MDP_SurfaceAttrType           *psSurfaceInfo;           /**< Extract the current source pipe fetch information */
   bool32                            *pbActive;                /**< Extract the current state (active/idle) of the source pipe */
} HAL_MDP_SourcePipeInfoType;

/*!
 * \struct HAL_MDP_WatchDog_TimerConfigType
 *
 * The \b HAL_MDP_WatchDog_TimerConfigType provides parameters to configure MDP watch dog timer counter
 *
 */
typedef struct _HAL_MDP_WatchDog_TimerConfigType
{
   uint32            uGranularity;            /**< Number of clock ticks granularity per main counter increment */
   uint32            uLoadValue;              /**< Timer count value */
} HAL_MDP_WatchDog_TimerConfigType;

/*!
 * \struct HAL_MDP_WatchDog_TimerEnableType
 *
 * The \b HAL_MDP_WatchDog_TimerEnableType provides parameter to enable/disable MDP watch dog timer
 *
 */
typedef struct _HAL_MDP_WatchDog_TimerEnableType
{
   bool32            bEnable;                 /**< Enable watch dog timer */
} HAL_MDP_WatchDog_TimerEnableType;

/*!
 * \struct HAL_MDP_WatchDog_TimerClearType
 *
 * The \b HAL_MDP_WatchDog_TimerClearType provides parameter to clear MDP watch dog timer counter
 *
 */
typedef struct _HAL_MDP_WatchDog_TimerClearType
{
   bool32            bClear;                  /**< Clear watch dog timer */
} HAL_MDP_WatchDog_TimerClearType;

/*!
 * \struct HAL_MDP_WatchDogConfigType
 *
 * The \b HAL_MDP_WatchDogConfigType provides parameters to configure MDP watch dog timer
 *
 */
typedef struct _HAL_MDP_WatchDogConfigType
{
   HAL_MDP_WatchDog_TimerConfigType     *pTimerConfig;        /**< Information to configure the timer counter */
   HAL_MDP_WatchDog_TimerEnableType     *pTimerEnable;        /**< Information to enable/disable the timer */
   HAL_MDP_WatchDog_TimerClearType      *pTimerClear;         /**< Information to clear the timer counter */
} HAL_MDP_WatchDogConfigType;

/*!
 * \struct HAL_MDP_WatchDogInfoParamsType
 *
 * The \b HAL_MDP_WatchDogInfoParamsType returns current watch dog timer counter info
 *
 */
typedef struct 
{
   uint32              uCurrentCounter;        /**< Watch dog status value */
} HAL_MDP_WatchDogInfoParamsType;

/* -----------------------------------------------------------------------
 * Helper data structures for HAL Driver Clients
 * ----------------------------------------------------------------------- */

/*!
 * \struct HAL_MDP_SourcePipeConfigObjType
 *
 * HAL driver should not use this data structure
 * This is a helper data structure for HAL driver's client module
 */
typedef struct _HAL_MDP_SourcePipeConfigObjType
{
   HAL_MDP_SurfaceAttrType           sSurfaceInfo;
   HAL_MDP_CropRectangleType         sCropRectInfo;
   HAL_MDP_ColorFillConfigType       sColorFillInfo;
   HAL_MDP_FetchModeConfigType       sFetchInfo;
   HAL_MDP_IGCConfigType             sIGCInfo;
   HAL_MDP_ScalingConfigType         sScaleInfo;
   HAL_MDP_ColorEncodeConfigType     sColorEncInfo;
   HAL_MDP_SharpenConfigType         sSharpenInfo;
   HAL_MDP_HistogramConfigType       sHistogramConfig;
   HAL_MDP_PictureAdjustConfigType   sPictureAdjust;
   HAL_MDP_ImmediateFlipConfigType   sImmediateFlip;
   HAL_MDP_VC1RangeLevelConfigType   sVC1RangeInfo;

} HAL_MDP_SourcePipeConfigObjType;

/*
 * HAL_MDP_DestinationPipeConfigObjType
 *
 * HAL driver should not use this data structure
 * This is a helper data structure for HAL driver's client module
 */
typedef struct _HAL_MDP_DestinationPipeConfigObjType
{
   HAL_MDP_GammaCorrectConfigType                  sGammaCorrect;
   HAL_MDP_PolynomialColorCorrectCoefficientType   sPCCCoeff[HAL_MDP_PCC_NUM_OF_PCC_COLOR_COMPONENTS];
   HAL_MDP_PolynomialColorCorrectConfigType        sPolynomialColorCorrect;
   HAL_MDP_GamutMappingConfigType                  sGamutMapping;
   HAL_MDP_HistogramConfigType                     sHistogramConfig;
   HAL_MDP_PictureAdjustConfigType                 sPictureAdjust;
   HAL_MDP_IGCConfigType                           sIGCConfig;
   HAL_MDP_WideGamutMappingConfigType              sWideGamutMapping;
   HAL_MDP_Linear_GammaCorrectConfigType           sLinearGammaCorrect;      /**< Linear Gamma Correction Information */
   HAL_MDP_PolynomialColorCorrect2CoefficientType  sPCC2Coeff[HAL_MDP_PCC_NUM_OF_PCC_COLOR_COMPONENTS];
   HAL_MDP_PolynomialColorCorrect2ConfigType       sPolynomialColorCorrect2; /**< Polynomial Color Correction 2 information */
} HAL_MDP_DestinationPipeConfigObjType;

/*!
 * \enum HAL_MDP_LayerMixerConfigObjType
 *
 * HAL driver should not use this data structure
 * This is a helper data structure for HAL driver's client module
 */
typedef struct _HAL_MDP_LayerMixerConfigObjType
{
   HAL_MDP_Mixer_BlendStageConfigType          sBlendStageConfig_Obj[HAL_MDP_BLEND_STAGE_MAX];
   HAL_MDP_Mixer_CursorConfigType              sCursorConfig_Obj;
   HAL_MDP_Mixer_GammaCorrectConfigType        sGammaCorrectConfig_Obj;
   HAL_MDP_Mixer_OutputSizeConfigType          sOutputSizeConfig_Obj;
   HAL_MDP_Mixer_BorderColorConfigType         sBorderColorConfig_Obj;
   HAL_MDP_Mixer_Linear_GammaCorrectConfigType sPanelGCConfig;

} HAL_MDP_LayerMixerConfigObjType;

typedef struct _HAL_MDP_ControlPath_MixerConfigObjType
{
   HAL_MDP_ControlPath_Mixer_ZOrderConfigType      sMixerZOrderConfig_Obj;
   HAL_MDP_ControlPath_Mixer_CursorConfigType      sMixerCursorConfig_Obj;
   HAL_MDP_ControlPath_Mixer_BorderColorConfigType sMixerBorderColorConfig_Obj;

} HAL_MDP_ControlPath_MixerConfigObjType;

/*
 * HAL_MDP_DestinationPipeConfigObjType
 *
 * HAL driver should not use this data structure
 * This is a helper data structure for HAL driver's client module
 */
typedef struct _HAL_MDP_ControlPathConfigObjType
{
  /* 
   * Special condition that violates struct definition convention
   * Each control path MUST have layer mixer structure associated with it even it does not require any layer mixer (e.g. Block-mode writeback)
   * 
   * Thus HAL_MDP_ControlPathConfigType.psControlPathMixerConfig and HAL_MDP_ControlPathConfigType.psLayerMixerConfig SHOULD NOT be NULL all the time
   * sControlPathMixerConfig and sLayerMixerConfig here are the structures referenced by the above two pointers respectively
   *
   * For the block-mode writeback, during HW programming preparation time, layer mixer configuration will not 
   * be prepared as pPhyDispInfo->eHALMixerId == HAL_MDP_LAYER_MIXER_NONE
   */

   HAL_MDP_ControlPath_MixerConfigType       sControlPathMixerConfig;      /**< Control path - Mixer Configuration         */
   HAL_MDP_LayerMixerConfigType              sLayerMixerConfig;            /**< Layer Mixer Configuration                  */

   HAL_MDP_ControlPath_InterfaceConfigType   sInterfaceConfig_Obj;         /**< Interface Configuration Objects            */
   HAL_MDP_ControlPath_MixerConfigObjType    sControlPathMixerConfig_Obj;  /**< Control path - Mixer Configuration Objects */
   HAL_MDP_LayerMixerConfigObjType           sLayerMixerConfig_Obj;        /**< Layer Mixer Configuration Objects          */

} HAL_MDP_ControlPathConfigObjType;

/*
 * HAL_MDP_DataPath_SurfaceConfigObjType
 *
 * HAL driver should not use this data structure
 * This is a helper data structure for HAL driver's client module
 */
typedef struct _HAL_MDP_DataPath_SurfaceConfigObjType
{
   HAL_MDP_SurfaceAttrType          sOutSurfaceConfig_Obj;    /**< Output surface attribute information                              */
   HAL_MDP_RectType                 sOutputRectConfig_Obj;    /**< Active rectangle information in the surface. Cord is always (0,0) */
   HAL_MDP_ConstAlphaConfigType     sConstAlphaValue_Obj;     /**< Constant Alpha configuration information                          */
   HAL_MDP_ScalarConfigType         sScalarConfig_Obj;        /**< Scalar (Downscale) configuration information                      */

} HAL_MDP_DataPath_SurfaceConfigObjType;

/*
 * HAL_MDP_DataPathConfigObjType
 *
 * HAL driver should not use this data structure
 * This is a helper data structure for HAL driver's client module
 */
typedef struct _HAL_MDP_DataPathConfigObjType
{
   HAL_MDP_DataPath_SurfaceConfigType     sSurfaceConfig;               /**< Surface configuration for Data Path                             */
   HAL_MDP_RotateConfigType               sRotatorConfig;               /**< Rotator configuration for Data Path                             */
   HAL_MDP_ColorEncodeConfigType          sColorEncodeConfig;           /**< Color encoding configuration for Data Path, used to program CSC */
   HAL_MDP_DataPath_SurfaceConfigObjType  sDataPathSurfaceConfig_Obj;   /**< Surface configuration object for Data Path                      */

} HAL_MDP_DataPathConfigObjType;

/*
 * HAL_MDP_InterfaceConfigObjType
 *
 * HAL driver should not use this data structure
 * This is a helper data structure for HAL driver's client module
 */
typedef struct _HAL_MDP_InterfaceConfigObjType
{
   HAL_MDP_PhyIntf_TimingGenConfigType         sIntrTimingGenConfig;
   HAL_MDP_Interface_TPGConfigType             sIntrTPGConfig;
   HAL_MDP_Interface_MISRConfigType            sIntrMISRConfig;
} HAL_MDP_InterfaceConfigObjType;

/*!
 * \struct HAL_MDP_PingPongConfigObjType
 *
 * HAL driver should not use this data structure
 * This is a helper data structure for HAL driver's client module
 */
typedef struct _HAL_MDP_PingPongConfigObjType
{
   HAL_MDP_PingPong_VsyncEnableType         sIntrVsyncEnable;       /**< vsync counter enable               */
   HAL_MDP_PingPong_TEEnableType            sIntrTEEnable;          /**< tear check enable                  */
   HAL_MDP_PingPong_AutoRefreshEnableType   sIntrAutoRefreshEnable; /**< auto-refresh enable                */
   HAL_MDP_PingPong_VsyncConfigType         sIntrVsyncConfig;       /**< vsync config                       */
   HAL_MDP_PingPong_TEConfigType            sIntrTEConfig;          /**< tear check config                  */
   HAL_MDP_PingPong_DSCConfigType           sDSCConfig;             /**< Display Stream Compression config  */

} HAL_MDP_PingPongConfigObjType;

/*!
 * \struct HAL_MDP_QOSRemapperType
 *
 * The \b HAL_MDP_QOSRemapperType provides remapper values for real and non real time clients.
 *
 */
typedef struct _HAL_MDP_QOSRemapperType
{
   uint32       uQosRemapperRealTimeClients;          /*QOS remapper settings for real time clients       */
   uint32       uQosRemapperNonRealTimeClients;       /*QOS remapper settings for non real time clients */
} HAL_MDP_QOSRemapperType;


/*!
 * \struct HAL_MDP_SplitMUXConfigObjType
 *
 * HAL driver should not use this data structure
 * This is a helper data structure for HAL driver's client module
 */
typedef struct _HAL_MDP_SplitMUXConfigObjType
{
   HAL_MDP_SplitMUX_BufferConfigType       sSplitMUXBufferConfig;   /** Split-MUX Buffer configuration     */

} HAL_MDP_SplitMUXConfigObjType;


typedef struct _HAL_MDP_DSCConfigObjType
{
   HAL_MDP_DSC_EncoderConfigType            sDSCEncoderCfg;          /**< DSC - Display Stream Compression Encoder config */

} HAL_MDP_DSCConfigObjType;


/* -----------------------------------------------------------------------
 * Function Prototypes
 * ----------------------------------------------------------------------- */

/****************************************************************************
** 1. MDP
****************************************************************************/
/****************************************************************************
*
** FUNCTION: HAL_MDP_Init()
*/
/*!
* \brief
*     Initialize the whole MDP block
*     This is the first function call to MDP HAL, it will initialize the whole MDP block and
*     set up the default values.
*
* \param [in]   pInitConfig               - Configuration data for MDP initialization
* \param [in]   uInitFlags                - Reserved.
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_Init(HAL_MDP_InitConfigType *pInitConfig, uint32 uInitFlags);

/****************************************************************************
*
** FUNCTION: HAL_MDP_Term()
*/
/*!
* \brief
*     Terminates the MDP HW blocks
*
* \param [in]   uTermFlags                - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_Term(uint32    uTermFlags);

/****************************************************************************
*
** FUNCTION: HAL_MDP_Interrupt_Enable()
*/
/*!
* \brief
*     Enable the interrupts
*
* \param [in]   psIntrCfgList         - Configuration for each module.
* \param [in]   uNumOfModules         - Number of modules
* \param [in]   uFlags                - Reserved.
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_Interrupt_Enable(HAL_MDP_InterruptConfigType  *psIntrCfgList,
                                            uint32                        uNumOfModules,
                                            uint32                        uFlags);

/****************************************************************************
*
** FUNCTION: HAL_MDP_Interrupt_Disable()
*/
/*!
* \brief
*     Disables the interrupts
*
* \param [in]   psIntrCfgList         - Configuration for each module.
* \param [in]   uNumOfModules         - Number of modules
* \param [in]   uFlags                - Reserved.
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_Interrupt_Disable(HAL_MDP_InterruptConfigType  *psIntrCfgList,
                                            uint32                        uNumOfModules,
                                            uint32                        uFlags);

/****************************************************************************
*
** FUNCTION: HAL_MDP_Interrupt_Clear()
*/
/*!
* \brief
*     Clears or Acknowledges the interrupts
*
* \param [in]   psIntrCfgList         - Configuration for each module
* \param [in]   uNumOfModules         - Number of modules
* \param [in]   uClearFlags           - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_Interrupt_Clear(HAL_MDP_InterruptConfigType   *psIntrCfgList,
                                           uint32                         uNumOfModules,
                                           uint32                         uClearFlags);

/****************************************************************************
*
** FUNCTION: HAL_MDP_Interrupt_GetStatus()
*/
/*!
* \brief
*     Returns the interrupts status
*
* \param [in/out] psIntrStatusList              - Status for each module.
* \param [in]     uNumOfModules                 - Number of modules
* \param [in]     uGetStatusFlags               - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_Interrupt_GetStatus(HAL_MDP_InterruptStatusType *psIntrStatusList,
                                               uint32                       uNumOfModules,
                                               uint32                       uGetStatusFlags);

/****************************************************************************
*
** FUNCTION: HAL_MDP_Interrupt_IsEnable()
*/
/*!
* \brief
*     Returns the current state interrupt configuration
*
* \param [in/out]   psInterruptStatus           - Status for each module.
* \param [in]       uNumOfModules               - Number of modules
* \param [in]       uGetStatusFlags             - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_Interrupt_IsEnable(HAL_MDP_InterruptStatusType *psInterruptStatus,
                                              uint32                       uNumOfModules,
                                              uint32                       uGetStatusFlags);

/****************************************************************************
*
** FUNCTION: HAL_MDP_Interrupt_ReadStatus_Reg()
*/
/*!
* \brief
*     Returns the interrupts status
*
* \param [in/out] puIntrStatusValue             - Status for each module
* \param [in]     uFlags                        - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_Interrupt_ReadStatus_Reg(uint32           *puIntrStatusValue,
                                                    uint32            uFlags);

/****************************************************************************
*
** FUNCTION: HAL_MDP_IsPingPongInterrupt()
*/
/*!
* \brief
*     Checks whether a Ping Pong interrupt has fired. This function is used
*  in MDP ISR, where special handling occurs in case of Smart Panels, which
*  use Ping Pong interrupts.
*
* \param [in]  puIntrStatusValue       - Interrupt status value. If Null, 
*                                        status register is read directly
* \param [in]  uFlags                  - Reserved
*
* \retval bool32: TRUE=there is a Ping Pong interrupt
*
****************************************************************************/
bool32 HAL_MDP_IsPingPongInterrupt(uint32       *puIntrStatusValue,
                                   uint32       uFlags);

/****************************************************************************
*
** FUNCTION: HAL_MDP_TrafficCtrl_Setup()
*/
/*!
* \brief
*     Configures the traffic control of memory transactions between MDP and VBIF
*     that interfaces to system memory.
*
* \param [in] psTrfCtrlSetting - performance control settings;
* \param [in] uNumOfSettings   - number of settings;
* \param [in] uMdpCoreClkFreq  - MDP core clock frequency in Hz; if the frequency is unknown, pass 0 and then
*                                the traffic limit will be calculated based on the max MDP core clock frequency. 
* \param [in] uFlags           - reserved flags
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_TrafficCtrl_Setup (
                       HAL_MDP_PerfCtrlParamsType *psPerfCtrlSetting,
                       uint32                      uNumOfSettings,
                       uint32                      uMdpCoreClkFreq,
                       uint32                      uFlags );

/****************************************************************************
*
** FUNCTION: HAL_MDP_VBif_QOSSetup()
*/
/*!
* \brief
*     VBIF QOS setup
*
* \retval None
*
****************************************************************************/

HAL_MDSS_ErrorType HAL_MDP_VBif_QOSSetup(HAL_MDP_VBIF_ClientIDType eClientId,
                                         HAL_MDP_InterfaceId       eInterfaceId,
                                         uint32                    uFlags);

/****************************************************************************
*
** FUNCTION: HAL_MDP_ClockControl_Set()
*/
/*!
* \brief
*     Configures MDP clock control status registers
*
* \param [in]  uClockList           - List of specific clocks to control (0 means all)
* \param [in]  bForceOn             - Force clocks on
* \param [in]  uHysterisisValue     - Hysteresis setting
* \param [in]  uFlags               - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_ClockControl_Set(uint32            uClockList,
                                            bool32            bForceOn,
                                            uint32            uHysterisisValue,
                                            uint32            uFlags);
                                                    

/****************************************************************************
** 2. Source pipe
****************************************************************************/
/****************************************************************************
*
** FUNCTION: HAL_MDP_SourcePipe_Setup()
*/
/*!
* \brief
*     Set up configurations for a particular source pipe
*
* \param [in]   eSourcePipe         - Source pipe id
* \param [in]   eLayerType          - Layer type
* \param [in]   psSourcePipeConfig  - Configuration for the selected pipe
* \param [in]   uSetupFlags         - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_SourcePipe_Setup(HAL_MDP_SourcePipeId            eSourcePipe,
                                            HAL_MDP_LayerType               eLayerType,
                                            HAL_MDP_SourcePipeConfigType   *psSourcePipeConfig,
                                            uint32                          uSetupFlags);

/****************************************************************************
*
** FUNCTION: HAL_MDP_SourcePipe_GetProperty()
*/
/*!
* \brief
*     Returns the source pipe property
*
* \param [in]   eSourcePipe        - Source pipe id
* \param [in]   eLayerType          - Layer type
* \param [out]  psSourcePipeInfo    - Information for the selected pipe
* \param [in]   uFlags              - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_SourcePipe_GetProperty(HAL_MDP_SourcePipeId          eSourcePipe,
                                                  HAL_MDP_LayerType             eLayerType,
                                                  HAL_MDP_SourcePipeInfoType    *psSourcePipeInfo,
                                                  uint32                        uFlags);

/****************************************************************************
*
** FUNCTION: HAL_MDP_LayerMixer_Config()
*/
/*!
* \brief
*     Configures the layer mixer
*
* \param [in] eLayerMixerId         - Layer Mixer ID (0, 1, 2, 3, and 4)
* \param [in] pLayerMixerConfig     - Layer Mixer Configuration information
* \param [in] uFlags                - Reserved flags
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_LayerMixer_Config(HAL_MDP_LayerMixerId             eLayerMixerId,
                                             HAL_MDP_LayerMixerConfigType    *pLayerMixerConfig,
                                             uint32                           uFlags);

/****************************************************************************
** Control Path Configuration
****************************************************************************/

/****************************************************************************
*
** FUNCTION: HAL_MDP_ControlPath_Setup()
*/
/*!
* \brief
*     Control path configuration
*
* \param [in] eControlPath          - Control Path ID (0, 1, 2, 3, 4)
* \param [in] pControlPathConfig    - Control path configuration information
* \param [in] uFlags                - Reserved flags
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_ControlPath_Setup(HAL_MDP_ControlPathId           eControlPath,
                                             HAL_MDP_ControlPathConfigType  *pControlPathConfig,
                                             uint32                          uFlags);

/****************************************************************************
*
** FUNCTION: HAL_MDP_ControlPath_Commit()
*/
/*!
* \brief
*     Control path flush and triggers the write-back path
*
* \param [in] eControlPath          - Control Path ID (0, 1, 2, 3, 4)
* \param [in] pControlPathFlush     - Modules to Flush
* \param [in] eInterfaceConnectType - Mainly for write-back paths
* \param [in] uFlags                - Reserved flags
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_ControlPath_Commit(HAL_MDP_ControlPathId         eControlPath,
                                              HAL_MDP_ControlPathFlushType  *pControlPathFlush,
                                              HAL_MDP_InterfaceConnectType  eInterfaceConnectType,
                                              uint32                        uFlags);

/****************************************************************************
*
** FUNCTION: HAL_MDP_ControlPath_GetProperty()
*/
/*!
* \brief
*     Retrieve information about an existing control path
*
* \param [in] eControlPath          - Control Path ID (0, 1, 2, 3, 4)
* \param [in] pControlInfoData      - Configuration for the current control path
* \param [in] uFlags                - Reserved flags
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_ControlPath_GetProperty(HAL_MDP_ControlPathId           eControlPath,
                                                   HAL_MDP_ControlPathConfigType  *pControlInfoData,
                                                   uint32                          uFlags);

/****************************************************************************
*
** FUNCTION: HAL_MDP_ControlPath_Reset()
*/
/*!
* \brief
*     Triggers a soft reset for a given control path
*
* \param [in] eControlPath          - Control Path ID (0, 1, 2, 3, 4)
* \param [in] uFlags                - Reserved flags
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_ControlPath_Reset(HAL_MDP_ControlPathId           eControlPath,
                                             uint32                          uFlags);

/****************************************************************************
*
** FUNCTION: HAL_MDP_DataPath_Setup()
*/
/*!
* \brief
*     Set up configurations for a particular data path
*
*
* \param [in]   eDataPathId      - Data path id
* \param [in]   psDataPathConfig - Configuration for the selected data path
* \param [in]   uFlags           - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_DataPath_Setup(HAL_MDP_DataPathId           eDataPathId,
                                          HAL_MDP_DataPathConfigType  *psDataPathConfig,
                                          uint32                       uFlags);

/****************************************************************************
*
** FUNCTION: HAL_MDP_DestinationPipe_Setup()
*/
/*!
* \brief
*     Set up configurations for a particular destination pipe
*
* \param [in]   eDestPipe         - Destination pipe id
* \param [in]   psDestPipeConfig  - Configuration for the selected pipe
* \param [in]   uSetupFlags       - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_DestinationPipe_Setup(HAL_MDP_DestinationPipeId                eDestPipe,
                                                 HAL_MDP_DestinationPipeConfigType       *psDestPipeConfig,
                                                 uint32                                   uSetupFlags);

/****************************************************************************
*
** FUNCTION: HAL_MDP_DestinationPipe_GetProperty()
*/
/*!
* \brief
*     Retrieve information for a particular destination pipe
*
* \param [in]   eDestPipe           - Destination pipe id
* \param [out]  pDsppInfoData       - Info data
* \param [in]   uGetPropertyFlags   - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_DestinationPipe_GetProperty(HAL_MDP_DestinationPipeId             eDestPipe,
                                                       HAL_MDP_DestinationPipeInfoParamsType *pDsppInfoData,
                                                       uint32                                uGetPropertyFlags);

/****************************************************************************
** 3. Interface
****************************************************************************/
/****************************************************************************
*
** FUNCTION: HAL_MDP_Interface_Setup()
*/
/*!
* \brief
*     Set up configurations for a particular interface
*
* \param [in]   eInterfaceId      - Interface id
* \param [in]   pInterfaceConfig  - Configuration for the selected interface
* \param [in]   uSetupFlags       - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_Interface_Setup(HAL_MDP_InterfaceId           eInterfaceId,
                                           HAL_MDP_InterfaceConfigType   *pInterfaceConfig,
                                           uint32                        uSetupFlags);

/****************************************************************************
*
** FUNCTION: HAL_MDP_Interface_GetProperty()
*/
/*!
* \brief
*     Retrieve information for a particular interface
*
* \param [in]     eInterfaceId            - Interface id
* \param [in/out] pInterfaceInfoData      - Info data
* \param [in]     uGetPropertyFlags       - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_Interface_GetProperty(HAL_MDP_InterfaceId             eInterfaceId,
                                                 HAL_MDP_InterfaceInfoParamsType *pInterfaceInfoData,
                                                 uint32                          uGetPropertyFlags);

/****************************************************************************
*
** FUNCTION: HAL_MDP_Interface_Enable()
*/
/*!
* \brief
*     Enable/Disable a particular interface
*
* \param [in]   eInterfaceId            - Interface id
* \param [in]   bEnable                 - True for enable, false for disable 
* \param [in]   uEnableFlags            - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_Interface_Enable(HAL_MDP_InterfaceId   eInterfaceId,
                                            bool32                bEnable,
                                            uint32                uEnableFlags);

/****************************************************************************
*
** FUNCTION: HAL_MDP_ReadVersionInfo()
*/
/*!
* \brief
*     Reads version information
*
* \param [in/out] psVersionInfo             - Version information
* \param [in/out] psHwInfo                  - Hardware information
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_ReadVersionInfo(HAL_HW_VersionType *psVersionInfo,
                                           HAL_MDP_HwInfo     *psHwInfo);

/****************************************************************************
** 4. VBIF
****************************************************************************/


/****************************************************************************
*
** FUNCTION: HAL_MDP_Vbif_Init()
*/
/*!
* \brief
*     Initializes VBIF settings.
*
* \param [in] eVbifId    - VBIF block ID
* \param [in] uFlags     - reserved flags
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_Vbif_Init(HAL_MDP_VbifId eVbifId, uint32 uFlags);


/****************************************************************************
*
** FUNCTION: HAL_MDP_TrafficCtrl_Init()
*/
/*!
* \brief
*     Initializes QoS priority re-mapping and watermark level settings for  
*     read clients, the output of QoS priority level for write paths, and 
*     the traffic shaper. 
*
* \param [in] pMdpInitConfig           - MDP initialization configuration
* \param [in] uFlags                   - reserved flags
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_TrafficCtrl_Init(HAL_MDP_InitConfigType *pMdpInitConfig, uint32 uFlags);


/****************************************************************************
*
** FUNCTION: HAL_MDP_PingPong_Setup()
*/
/*!
* \brief
*     Set up configurations for a particular pingpong block
*
* \param [in]   ePingPongId      - Pingpong block id
* \param [in]   pPingPongConfig  - Configuration for the selected pingpong block
* \param [in]   uSetupFlags      - Reserved.
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_PingPong_Setup(HAL_MDP_PingPongId             ePingPongId,
                                          HAL_MDP_PingPongConfigType     *pPingPongConfig,
                                          uint32                         uSetupFlags);


/****************************************************************************
*
** FUNCTION: HAL_MDP_PingPong_GetProperty()
*/
/*!
* \brief
*     Retrieve information for a particular pingpong block
*
* \param [in]   ePingPongId           - PingPong block id
* \param [out]  pPingPongInfoData     - Info data
* \param [in]   uGetPropertyFlags     - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_PingPong_GetProperty(HAL_MDP_PingPongId                   ePingPongId,
                                                HAL_MDP_PingPongInfoParamsType       *pPingPongInfoData,
                                                uint32                               uGetPropertyFlags);

/****************************************************************************
*
** FUNCTION: HAL_MDP_SplitMUX_Setup()
*/
/*!
* \brief
*     Set up configurations for a particular split-muxer (PPB) block
*
* \param [in]   eSplitMUXId      - Split-MUX (PPB) block id
* \param [in]   pSplitMUXConfig  - Configuration for the selected PPB block
* \param [in]   uSetupFlags      - Reserved (must be 0x00)
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_SplitMUX_Setup(HAL_MDP_SplitMUXId             eSplitMUXId,
                                          HAL_MDP_SplitMUXConfigType    *pSplitMUXConfig,
                                          uint32                         uSetupFlags);

/****************************************************************************
*
** FUNCTION: HAL_MDP_WatchDog_Setup()
*/
/*!
* \brief
*     Setup configurations for MDP watch dog registers
*
* \param [in]  HAL_MDP_WatchDogId   - Watch Dog ID (0, 1, 2...)
* \param [in]  psWatchDogConfig     - Pointer to the watch dog configuration information
* \param [in]  uFlags               - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_WatchDog_Setup(HAL_MDP_WatchDogId           eWatchDogId,
                                          HAL_MDP_WatchDogConfigType   *psWatchDogConfig,
                                          uint32                       uFlags);

/****************************************************************************
*
** FUNCTION: HAL_MDP_WatchDog_GetProperty()
*/
/*!
* \brief
*     Retrieve information of current watch dog timer counter
*
* \param [in]  HAL_MDP_WatchDogId   - Watch Dog ID (0, 1, 2...)
* \param [in]  psWatchDogInfoData   - Pointer to the watch dog info data
* \param [in]  uFlags               - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_WatchDog_GetProperty(HAL_MDP_WatchDogId               eWatchDogId,
                                                HAL_MDP_WatchDogInfoParamsType   *psWatchDogInfoData,
                                                uint32                           uFlags);

/****************************************************************************
*
** FUNCTION: HAL_MDP_DSC_Setup()
*/
/*!
* \brief
*     Setup configurations for Display Stream Compression (DSC) module
*
* \param [in]  eDSCEncEngineId      - Display Stream Compression Engine ID (0, 1, 2...)
* \param [in]  pDSCConfig           - Encoder Configuration parameters 
* \param [in]  uFlags               - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_DSC_Setup(HAL_MDP_DSCId          eDSCEncEngineId,
                                     HAL_MDP_DSCConfigType *pDSCConfig,
                                     uint32                 uFlags);

/****************************************************************************
*
** FUNCTION: HAL_MDP_DSC_GetPropery()
*/
/*!
* \brief
*     Used to retrieve various information of the Display Stream Compression module
*
* \param [in]     eDSCEncEngineId      - Display Stream Compression Engine ID (0, 1, 2...)
* \param [in/out] pDSCInfo             - Input information to prepare the requested information
*                                        and Ouput data-structure holds the prepared information
* \param [in]     uFlags               - Reserved
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_DSC_GetPropery(HAL_MDP_DSCId          eDSCEncEngineId,
                                          HAL_MDP_DSCInfoType   *pDSCInfo,
                                          uint32                 uFlags);

/* HAL_MDP_UTILITY functions are implemented in C++ */

#ifdef __cplusplus
extern "C" 
{
#endif

/****************************************************************************
*
** FUNCTION: HAL_MDP_UTILITY_Init()
*/
/*!
* \brief
*     This function initializes resources inside HAL_MDP_UTILITY
*
* \param [in]   flags              - Initialization flags, specify 0 to 
*                                    initialize all resources. No Enum is 
*                                    associated with flag currently
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_UTILITY_Init(uint32 flags);


/****************************************************************************
*
** FUNCTION: HAL_MDP_UTILITY_DeInit()
*/
/*!
* \brief
*     This function releases all resources inside HAL_MDP_UTILITY
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_UTILITY_DeInit(void);

/****************************************************************************
*
** FUNCTION: HAL_MDP_QOSSetup()
*/
/*!
* \brief
*     Set QOS remapper setting based on Configurations .
*
* \param [in]  pQOSSettings  - QOS remapper settings
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_QOSSetup(HAL_MDP_QOSRemapperType *pQOSSettings);

/****************************************************************************
*
** FUNCTION: HAL_MDP_CDM_Setup()
*/
/*!
* \brief
*     Configures the Chroma down module
*
*
* \param [in]   eChromaDownModuleId    - Chroma Down module
* \param [in]   psChromadownConfig     - Chroma down module configuration
* \param [in]   uFlags                 - Reserved.
*
* \retval HAL_MDSS_ErrorType
*
****************************************************************************/
HAL_MDSS_ErrorType HAL_MDP_CDM_Setup(HAL_MDP_ChromaDownModuleId           eChromaDownModuleId,
                                     HAL_MDP_ChromaDownModuleConfigType  *psChromadownConfig,
                                     uint32                               uFlags);
#ifdef __cplusplus
}
#endif

#endif /* _HAL_MDP_H_ */
