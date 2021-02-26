#ifndef __MDPLIB_I_H__
#define __MDPLIB_I_H__
/*=============================================================================
 
  File: MDPlib_i.h
 
  Internal header file for MDP library
  
 
  Copyright (c) 2011-2018 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <Uefi.h>
#include "MDPTypes.h"
#include "hal_mdp.h"
#include <Protocol/EFIChipInfo.h>
#include <Protocol/EFIPlatformInfo.h>
#include "MDPPlatformLib.h"

/*===========================================================================
                                Defines 
===========================================================================*/
/* Maximum length of clock name
 */
#define       MDPLIB_MAX_CLOCK_NAME                         30

/* Maximum length of power domain name
 */
#define       MDPLIB_MAX_POWER_DOMAIN_NAME                  30

/* Flags used for MDPFirmwareEnvType.uPrimaryPanelFlags
 */
#define MDP_FIRMWARE_ENV_FLAG_DISPLAY_INITIALIZED           0x00000001    /* Display has been initialized by the firmware */

#define MDP_DSC_PROFILEID_MAX                               16

/* Magic number for MDP firmware environment variable structure
*/
#define MDP_FIRMWARE_ENV_VERSION_MAGIC                      0xAA
/* Major version for MDP firmware environment variable structure
*/
#define MDP_FIRMWARE_ENV_VERSION_MAJOR                      1
/* Minor version for MDP firmware environment variable structure
*/
#define MDP_FIRMWARE_ENV_VERSION_MINOR                      3

/* Buffer Size for holding the panel specific config commands 
*/
#define MDP_DSI_COMMAND_BUFFER_SIZE                         (4*1024)

/* XML Header Size for DSC PPS command 
*/
#define MDP_DSI_DSC_XML_HEADER_SIZE                         1

/* Null packet at XML end for DSC PPS command
*/
#define MDP_DSI_DSC_XML_NULL_END_SIZE                       4

/* Header Size for DSC PPS command 
*/
#define MDP_DSI_DSC_HEADER_SIZE                             4

/* Payload Size for DSC PPS command
*/
#define MDP_DSI_DSC_PAYLOAD_SIZE                            128

/* Buffer Size for holding the panel specific config commands 
*/
#define MDP_DSI_DSC_PPS_TOTAL_PACKET_SIZE                   ((MDP_DSI_DSC_XML_HEADER_SIZE) + (MDP_DSI_DSC_HEADER_SIZE) + (MDP_DSI_DSC_PAYLOAD_SIZE) + (MDP_DSI_DSC_XML_NULL_END_SIZE))

/*
* Flags for features defined in MDP_PingPongCaps.uPPFeatureCaps
*/
#define MDP_PINGPONG_CAP_PINGPONG_SPLIT                     0x00000001   /* The ping-pong supports PP Split Feature*/

/*
* Number of resources (layers, mixers, ping-pong, interfaces)
* used in single/dual pipe mode
*/
#define MDP_SINGLEPIPE_NUM_MIXERS                           1
#define MDP_DUALPIPE_NUM_MIXERS                             2

#define MDP_INTERFACE_SINGLE                                1
#define MDP_INTERFACE_DUAL                                  2

/*
* Index definitions of the control paths
*/
#define MDP_MASTER_PATH_INDEX                               0            /* Main control Path in single pipe or Index 0 path in dual pipe */
#define MDP_SLAVE_PATH_INDEX                                1            /* Index 1 path in dual pipe */

#define  MDP_PHASE_STEP_CONSTANT                            21

/* Number of I2C Slave addresses
*/
#define MDP_I2C_SLAVE_ID_NUM                                2

#define MDP_RESET_INFO_NUM                                  5 /* MDPPlatformPanelResetInfo - uResetGpio */

/* Buffer Size for holding the I2C specific config commands 
*/
#define MDP_I2C_COMMAND_BUFFER_SIZE                         (4*1024)

/* Default I2C bus frequency 
*/
#define MDP_I2C_DEFAULT_BUS_FREQUENCY_KHZ                   400

/* Maximum number of default GPIO entries
*/
#define MDP_MAX_GPIO_LIST_ENTRIES                           8

/* Define for a kilobyte (1^10)
*/
#define MDP_KILOBYTES(x)                                    ((x)*(1<<10))

/* Define for a mega (10^6)
*/
#define MDP_MEGA(x)                                         ((x)*(1000000))

/* Define maximal supported number of dynamic refresh
*/
#define MDP_MAX_NUM_DYNAMIC_REFRESH                         16
#define MDP_MAX_NUM_DSI_DEVICE                              2
#define MDP_MAX_PLLCODES_SIZE                               0x100000

/*===========================================================================
                                Enumerations 
===========================================================================*/

/* List of MDP support clocks
 */
typedef enum {
  MDP_CLOCKTYPE_CORE = 0,
  MDP_CLOCKTYPE_DSI0,
  MDP_CLOCKTYPE_DSI1,
  MDP_CLOCKTYPE_HDMI,
  MDP_CLOCKTYPE_DP,
  MDP_CLOCKTYPE_MAX,
} MDPClockTypes;

/*
 * DSI Controller Configuration for Single DSI 
 * This is used to detect if there is a single DSI interface and which controller is mapped. Used in determining which
 * interface to use while configuring the top registers. 
 */
typedef enum
{
  DSI_0_SINGLE_PRIMARY = 0,                             /**< Single DSI only with controller 0 as primary */
  DSI_1_SINGLE_PRIMARY = 1,                             /**< Single DSI only with controller 1 as primary */
} DSI_SingleControllerPrimaryType;

/* 
 * MDP Interface Types Parameters
 */
typedef enum
{
  MDP_DISPLAY_CONNECT_NONE = 0,
  MDP_DISPLAY_CONNECT_EBI2,                              /**< 1  - Display is connected to EBI2 interface */
  MDP_DISPLAY_CONNECT_LCDC,                              /**< 2  - Display is connected to LCDC interface */
  MDP_DISPLAY_CONNECT_MDDI_PRIMARY,                      /**< 3  - Display is connected to primary MDDI interface */
  MDP_DISPLAY_CONNECT_MDDI_EXTERNAL,                     /**< 4  - Display is connected to external MDDI interface */
  MDP_DISPLAY_CONNECT_TV,                                /**< 5  - It is deprecated. Please use MDP_DISPLAY_CONNECT_ANALOG_TV instead. */
  MDP_DISPLAY_CONNECT_ANALOG_TV = MDP_DISPLAY_CONNECT_TV,/**< 5  - Display is connected to TV interface */
  MDP_DISPLAY_CONNECT_MDDI_LCDC,                         /**< 6  - Display is connected to MDDI LCDC mode interface */
  MDP_DISPLAY_CONNECT_DTV,                               /**< 7  - It is deprecated. Please use MDP_DISPLAY_CONNECT_HDMI instead. */
  MDP_DISPLAY_CONNECT_HDMI = MDP_DISPLAY_CONNECT_DTV,    /**< 7  - Display is connected to HDMI interfaces */
  MDP_DISPLAY_CONNECT_PRIMARY_DSI_VIDEO,                 /**< 8  - Display is connected to Primary DSI interface using Video Mode     */
  MDP_DISPLAY_CONNECT_PRIMARY_DSI_CMD,                   /**< 9  - Display is connected to Primary DSI interface using Command Mode   */
  MDP_DISPLAY_CONNECT_SECONDARY_DSI_VIDEO,               /**< 10 - Display is connected to Secondary DSI interface using Video Mode   */
  MDP_DISPLAY_CONNECT_SECONDARY_DSI_CMD,                 /**< 11 - Display is connected to Secondary DSI interface using Command Mode */
  MDP_DISPLAY_CONNECT_LVDS,                              /**< 12 - Display is connected to LVDS interface */
  MDP_DISPLAY_CONNECT_FRAMEBUFFER_WRITEBACK,             /**< 13 - QDI output is written back to Frame Buffer (Memory) via the writeback feature. */
  MDP_DISPLAY_CONNECT_DP,                                /**< 14 - Display is connected to Display Port interface */
  MDP_DISPLAY_CONNECT_DBI,                               /**< 15 - Display is connected to DBI interface */  
  MDP_DISPLAY_CONNECT_MHL,                               /**< 16 - Display is connected to MHL interface */
                                                         /**<      i.e. Output is not connected to physical display but to memory */
  MDP_DISPLAY_CONNECT_MAX,
  MDP_DISPLAY_CONNECT_FORCE_32BIT = 0x7FFFFFFF
} MDP_DisplayConnectType;

/* 
 * MDP Color component packing
 */
typedef enum
{
  MDP_COLOR_COMPONENTS_PACKING_TIGHT, /**< No unused bits present between color components */   
  MDP_COLOR_COMPONENTS_PACKING_LOOSE  /**< 1 color component per byte */
} MDP_ColorComponentsPackingType;

/* 
 * MDP Color component alignment
 */
typedef enum
{
  MDP_COLOR_COMPONENT_ALIGNMENT_LSB, /**< Color component's used bits are LSB-aligned */
  MDP_COLOR_COMPONENT_ALIGNMENT_MSB  /**< Color component's used bits are MSB-aligned */
} MDP_ColorComponentAlignmentType;

/*!
* \b MDSS_Device_Version
*
* Describe HW version that available.
* 
*/
typedef enum
{
  MDSS_DEVICE_VERSION_04_00 = 0,   /**< Device 4.0.x */
  MDSS_DEVICE_VERSION_04_01,       /**< Device 4.1.x */
  MDSS_DEVICE_VERSION_04_02,       /**< Device 4.2.x */
  MDSS_DEVICE_VERSION_04_03,       /**< Device 4.3.x */
  MDSS_DEVICE_VERSION_04_04,       /**< Device 4.4.x */
  MDSS_DEVICE_VERSION_04_05,       /**< Device 4.5.x */
  MDSS_DEVICE_VERSION_04_06,       /**< Device 4.6.x */
  MDSS_DEVICE_VERSION_04_07,       /**< Device 4.7.x */
  MDSS_DEVICE_VERSION_04_08,       /**< Device 4.8.x */  
  MDSS_DEVICE_VERSION_04_09,       /**< Device 4.9.x */
  MDSS_DEVICE_VERSION_MAX,
  MDSS_DEVICE_VERSION_FORCE_32BIT = 0x7FFFFFFF
}MDSS_Device_VersionType;


/*!
* \b MDP_Panel_FlagsType
*
* Defines the miscellaneous display flags. There is one feature per bit.
* 
*/
typedef enum 
{
  MDP_PANEL_FLAG_NONE                      = 0x00000000,
  /**< Default flag state. */

  MDP_PANEL_FLAG_FORCE_DISPLAY_REINIT      = 0x00000001,
  /**< Bypasses a seamless transition from the firmware to the OS driver. Always
       force a reinitialization of the display. */

  MDP_PANEL_FLAG_DISABLE_SBC               = 0x00000002,
  /**< Disables force smooth brightness capabilities. */

  MDP_PANEL_FLAG_DISABLE_CABL              = 0x00000004,
  /**< Disables adaptive backlight level capabilities. */

  MDP_PANEL_FLAG_DISABLE_HW_CURSOR         = 0x00000008,
  /**< Disables hardware cursors.  */

  MDP_PANEL_FLAG_DUAL_MODE_SUPPORT         = 0x00000010,
  /**< Enables Video and Command mode support.  */

  MDP_PANEL_FLAG_DSI_DCS_POLLING_TRANSFER  = 0x00000100,
  /**< Disables the event-based and fallback to polling-based DSI DCS sequence. */
  /** @cond */
  MDP_PANEL_FLAG_DSI_DCS_DMA_ONLY          = 0x00000200,
  /* Deprecated. DSI command data is only fetched from memory via the DMA VBIF interface. */
  /** @endcond */
  MDP_PANEL_FLAG_DSI_DCS_FIFO_ONLY         = 0x00000400,
  /**< Deprecated -- Use #MDP_PANEL_DSI_FLAG_DSI_DCS_FIFO_ONLY instead. DSI command data is only fetched via TPG FIFO.  */

  MDP_PANEL_FLAG_ENABLE_DFS_IDLESCREEN     = 0x00000800,
  /**< Enables the dynamic refresh rate for an idle screen. */

  MDP_PANEL_FLAG_MIRROR_DUAL_PIPE_CONFIG   = 0x00001000,
  /**< Swap-layer source ROI for dual-pipe configuration in the layer mixer. */
  
  MDP_PANEL_FLAG_DISABLE_POST_PROCESSING   = 0x00002000,
  /**< Disable all color management and post processing fatures. */

  MDP_PANEL_FLAG_DISABLE_SEAMLESS_SPLASH   = 0x01000000,
  /**< Turn off the display before the driver is unloaded */

  MDP_PANEL_FLAG_ENABLE_PINGPONG_SPLIT     = 0x02000000,
  /**< Turn on pingpong split */

  MDP_PANEL_FLAG_DISABLE_PPS_CMD           = 0x04000000,
  /**< Skip sending PPS command for DCS panel. */

} MDP_Panel_FlagsType;


/* MDP_FlushModule_Type
 * 
 * Enumeration of all the modules to be flushed
 */
typedef enum
{
  MDP_FLUSH_LAYER      = 0x00000001,     // SPP flush 0 - For all layer (DMA,RGB,ViG) actions 
  MDP_FLUSH_MIXER      = 0x00000010,     // Mixer level flush  - For blending changes and operations
  MDP_FLUSH_CTLPATH    = 0x00000100,     // Control path flush  - For all layer/cursor attach and detach operations
  MDP_FLUSH_DSPP       = 0x00001000,     // DSPP level flush - For all destination pixel processing operations
  MDP_FLUSH_INTERFACE  = 0x00010000,     // Interface level flush - For all interface level changes
                                         // Enums below are combinational flush flags
  MDP_FLUSH_ZORDER     = (MDP_FLUSH_MIXER | MDP_FLUSH_CTLPATH),
  MDP_FLUSH_DISPLAY    = (MDP_FLUSH_MIXER | MDP_FLUSH_CTLPATH | MDP_FLUSH_DSPP | MDP_FLUSH_INTERFACE),
  MDP_FLUSH_ALL        = (MDP_FLUSH_LAYER | MDP_FLUSH_DISPLAY),
} MDP_FlushModule_Type;


/* MDP_ClockFlags_Type
 * 
 * Enumeration of all the modules to be flushed
 */
typedef enum
{
  MDP_CLOCKFLAG_SHARED     = 0x00000001,     // Clock/Domain is shared, do not turn off
  MDP_CLOCKFLAG_BUS        = 0x00000002,     // Clock/Domain is bus and cannot be voted 
} MDP_ClockFlags_Type;



/* MDP_PresentationMode_Type
 * 
 * Enumeration of all presentation modes available
 */
typedef enum
{
  MDP_PRESENTATIONMODE_LETTERBOX = 0x00000000,   // Letterbox the content within the display.
  MDP_PRESENTATIONMODE_FIT,                      // Fit the content to the display, requires scaling.
  MDP_PRESENTATIONMODE_CROP,                     // Crop the source to fit within the display.
  MDP_PRESENTATIONMODE_MAX
} MDP_PresentationMode_Type;

/*===========================================================================
                                HAL Mapping Structures 
===========================================================================*/

/* MDP_DisplayCtrlPathMapType
 *
 * Mapping of display's to control path components
 */
typedef struct
{
   HAL_MDP_SourcePipeId          eSourcePipeId;       /* Source pipe mapped to this resource */
   HAL_MDP_ControlPathId         eControlPathId;      /* Control path for this blt path */
   HAL_MDP_LayerMixerId          eLayerMixerId;       /* Control path for this blt path */
   HAL_MDP_DestinationPipeId     eDestinationPipeId;  /* Layer mixer dedicated for this blt path */
   HAL_MDP_PingPongId            ePingPongId;         /* Pingpong block for this path */
}  MDP_DisplayCtrlPathMapType;

/* MDP_DisplayInterfaceMapType
 *
 * Mapping of display's to control path components
 */
typedef struct
{
   HAL_MDP_InterfaceId           eInterfaceId;        /* Interface dedicated for this blt path */
   HAL_MDP_InterfaceModeType     eInterfaceMode;      /* Interface mode for this device */
   HAL_MDP_InterfaceStandardType eInterfaceStandard;  /* Interface associated with the control path */
} MDP_DisplayInterfaceMapType;


/* MDP_SourcePipeInfoType
 *
 * Mapping of display's source pipe type and capabilities
 */
typedef struct
{
   HAL_MDP_LayerType             eLayerType;          /* Layer class */
} MDP_SourcePipeInfoType;


/* MDP_PixelFormatMapType
 *
 * Mapping of display's to control path components
 */
typedef struct 
{
   uint32                   uBitsPerPixel;
   uint32                   uNumPlanes;
   bool32                   bYUV;
   HAL_MDP_PixelFormatType  eHALPixelFormat;
} MDP_PixelFormatMapType;



/*===========================================================================
                                Structures 
===========================================================================*/

/*  MDP_I2C_Configuration
*   
*   Hold information for I2C configuration
*/
typedef struct
{
  uint32 aI2CSlaveAddressList[MDP_I2C_SLAVE_ID_NUM];     /**< I2C Slave address                           */
  uint32 uI2CNumOfSlaves;                                /**< Number of Slave address                     */
  uint32 uI2CBusFrequency;                               /**< I2C Frequency                               */
  uint32 uI2CGSBICore;                                   /**< I2C GSBI Core ID (must be between 1 to 12)  */
  uint32 uI2COffsetLengthInByte;                         /**< I2C register address offset length in byte  */
  uint8* pI2CInitSequence;                               /**< I2C initialization sequence buffer          */
  uint8* pI2CTermSequence;                               /**< I2C termination sequence buffer             */
} MDP_I2C_Configuration;


/*  MDP_DisplayControlPathInfo
*
*   This structure is used to describe the control path information
*/
typedef struct
{
    HAL_MDP_ControlPathId                       eControlPathId;         /**< Control path Id */
    HAL_MDP_SourcePipeId                        eSourcePipeId;          /**< Source pipe for this control path */
    HAL_MDP_LayerMixerId                        eLayerMixerId;          /**< Layer mixer dedicated for this control path */
    HAL_MDP_DestinationPipeId                   eDestinationPipeId;     /**< Destination Pipe dedicated for this control path */
    HAL_MDP_InterfaceId                         eInterfaceId;           /**< Interface dedicated for this control path */
    HAL_MDP_PingPongId                          ePingPongId;            /**< Pingpong block id for this control path */
} MDP_DisplayControlPathInfo;

/** Defines the DSI Tear Enable (TE) configuration.
*/
typedef struct
{
    bool32                        bTECheckEnable;
    /**< Enables the MDP tear check block. */

    bool32                        bDedicatedTEPin;
    /**< Indicates whether an external TE pin or an embedded TE pin is used. */

    /* MDP Vsync TE block configuration */
    uint32                        vSyncStartPos;
    /**< Line number from which the TE kick-off condition is evaluated for
         vertical synchronization. */

    uint32                        vSyncContinueLines;
    /**< Minimum number of lines the write pointer must be above the read
         pointer in order to continue line updates. */

    uint32                        vSyncStartLineDivisor;
    /**< Indicates the part of visible height to be used as the start
         threshold for vertical synchronization. */

    uint32                        vSyncPosSafetyMargin;
    /**< Optimizes the start position value for vertical synchronization. */

    uint32                        vSyncBelowSafetyMargin;
    /**< Optimizes the start threshold value for vertical synchronization. */

    uint32                        vSyncPercentVariance;
    /**< Percent by which the refresh rate is to be reduced to increase the
         vertical synchronization count. */

    uint32                        vSyncRdPtrIrqLine;
    /**< Read pointer to the IRQ line request for displaying vertical
         synchronization. */

    uint32                        vSyncSelect;
    /**< External Vsync source select. */

    uint32                        uVsyncInitValue;
    /**< Specify the init value to which the read pointer gets loaded at vsync edge */

    /* Reserved fields for future use */
    uint32                        uTEMode;                /**< Reserved. */
    uint32                        uTEPolarity;            /**< Reserved. */
    uint32                        uTEHSyncTriggerCount;   /**< Reserved. */
    uint32                        uTEMinVSyncWidth;       /**< Reserved. */
    uint32                        uTEMaxVSyncWidth;       /**< Reserved. */

} DSIPanelTEInfo;

/* DSI (Video/Command) Configuration
 */
typedef struct
{
    uint32 uRefreshRate;               /**< Panel refresh rate in Q16.16 representation */
    uint32 uBitClockFrequency;         /**< DSI bit clock frequency in Hz (used to override uRefreshRate) */    
    
    /* DSI Common Configuration */
    uint32 eChannelId;                 /**< Display Channel ID.  */    
    uint32 eDisplayVC;                 /**< Virtual Channel ID.  */
    uint32 eColorFormat;               /**< Panel Color Format.  */    
    uint32 uDataLaneNum;               /**< Number of data lane used */
    uint32 uLaneRemap;                 /**< Lane remap order*/
    bool32 bLP11AtInit;                /**< Force lanes to LP11 during panel reset */              
    bool32 bCPhyMode;                  /**< C-Phy */              
    uint32 uControllerSinglePrimary;   /**< Controller is single DSI and Primary */

    /* DSI LS/HS Configuration */
    bool32 bPacketTransferHS;          /**< Force command transfers during high speed  */
    uint32 uClockHSForceRequest;       /**< Force clock lanes to be always be in HS */
    bool32 bDisableEotpAfterHSXfer;    /**< disable appending EoTp at the end of each forward HS data burst, default is enabled */
    
    /* DSI Timing parameter overrides */
    bool32 bTimingHSZeroOverride;      /* Timing override for THS_Zero */
    uint32 uTimingHSZeroValue;         /* THS_Zero value */
    bool32 bTimingHSExitOverride;      /* Timing override for THS_Exit */
    uint32 uTimingHSExitValue;         /* THS_Exit value */
    bool32 bTimingHSPrepareOverride;   /* Timing override for THS_Prepare */
    uint32 uTimingHSPrepareValue;      /* THS_Prepare value */
    bool32 bTimingHSTrailOverride;     /* Timing override for THS_Trail */
    uint32 uTimingHSTrailValue;        /* THS_Trail value */
    bool32 bTimingHSRequestOverride;   /* Timing override for THS_Request */
    uint32 uTimingHSRequestValue;      /* THS_Request value */
    bool32 bTimingCLKZeroOverride;     /* Timing override for TClk_Zero */
    uint32 uTimingCLKZeroValue;        /* TClk_Zero value */
    bool32 bTimingCLKTrailOverride;    /* Timing override for TClk_Trail */
    uint32 uTimingCLKTrailValue;       /* TClk_Trail value */    
    bool32 bTimingCLKPrepareOverride;  /* Timing override for TClk_Prepare */
    uint32 uTimingCLKPrepareValue;     /* TClk_Prepare value */
    bool32 bTimingCLKPreOverride;      /* Timing override for TClk_Pre */
    uint32 uTimingCLKPreValue;         /* TClk_Pre value */
    bool32 bTimingCLKPostOverride;     /* Timing override for TClk_Post */
    uint32 uTimingCLKPostValue;        /* TClk_Post value */
    bool32 bTimingTAGoOverride;        /* Timing override for TTA_Go */    
    uint32 uTimingTAGoValue;           /* TTA_Go value */    
    bool32 bTimingTASureOverride;      /* Timing override for TTA_Sure */        
    uint32 uTimingTASureValue;         /* TTA_Sure value */
    bool32 bTimingTAGetOverride;       /* Timing override for TTA_Get */
    uint32 uTimingTAGetValue;          /* TTA_Get value */
    uint32 uInitMasterTime;            /* DSI init master time(ms) (init_master_time >= (init_slave_time + internal_delay) */

    /* Video Mode specific configuration */
    uint32 eTrafficMode;               /**< DSI Video Traffic Mode     */  
    bool32 bSendHsaHseAfterVsVe;       /**< Send HSA(Horizontal Sync Active)&HSE(Horizontal Sync End) following VS(Vsync Active)/VE(Vsync End)? */
    bool32 bLowPowerModeInHFP;         /**< LP11 in HFP (Horizontal Front Porch) period?                             */
    bool32 bLowPowerModeInHBP;         /**< LP11 in HBP (Horizontal Back Porch ) period?                             */
    bool32 bLowPowerModeInHSA;         /**< LP11 in HSA (Horizontal Sync Active) period?                             */
    bool32 bLowPowerModeInBLLPEOF;     /**< LP11 in BLLP (Blanking or Low Power interval) period at end of frame?    */
    bool32 bLowPowerModeInBLLP;        /**< LP11 in BLLP (Blanking or Low Power interval) period?                    */
    bool32 bForceCmdInVideoHS;         /**< Force command packets in video high speed mode */

    /* Command Mode specific configuration */
    bool32 bSwapInterface;             /**< Map Prim DSI to secondary Interface?  */
    bool32 bUsingTrigger;              /**< Enable Trigger for this interface?    */
    bool32 bEnableAutoRefresh;         /**< Enable Auto Refresh*/
    uint32 uAutoRefreshFrameNumDiv;    /**< Auto Refresh frame number for controlling frame rate.*/
    uint32 uCommandModeIdleTime;       /**< Idle time to be inserted between command mode MDP packets */
    int32  iSlaveControllerSkewLines;
    /**< Number of skewed lines between two controllers for dual-dsi use-case.
    if (iSlaveControllerSkewLines >  0), then master controller starts data transfer ahead of slave controller.
    if (iSlaveControllerSkewLines == 0), then master and slave controllers will start data transfer approximately at same time.
    if (iSlaveControllerSkewLines <  0), then master controller starts data transfer after slave controller. */

    /* Display Alignment Configuration (Dual DSI command mode only) */
    uint32 uCmdModePixelAlignment;     /**< 0:No alignment, 1:Align at start of frame, 2:Align at start of line */

    /* DSI TE Configuration (Command mode only) */
    DSIPanelTEInfo  sTE;
    
    /* DSI Regulator mode */
    bool32 bPhyDCDCMode;               /**< DSI Regulator Mode: TRUE:DCDC or FALSE:LDO */

    /* DSI DSC Configuration */
    bool32 bDSCEnable;                 /**< Display Stream Compression Enabled on panel */
    uint32 uDSCMajor;                  /**< Display Stream Compression major version */
    uint32 uDSCMinor;                  /**< Display Stream Compression minor version */
    uint32 uDSCScr;                    /**< Display Stream Compression SCR version */
    uint32 uDSCProfileID;              /**< Display Stream Compression Profile ID supported by panel*/
    uint32 uDSCCompressionRatio;       /**< Display Stream Compression Ratio, This is dynamically generated */
    uint32 uDSCSliceWidth;             /**< Display Stream Compression Slice Width*/
    uint32 uDSCBpc;                    /**< Display Stream Compression bits per component */
    uint32 uDSCBpp;                    /**< Display Stream Compression bits per pixel */
    uint32 uDSCSliceHeight;            /**< Display Stream Compression Slice Height*/
    bool32 bDSCLayerMixSplit;          /**< Display Stream Compression Dual mixer Dual Pipe Configuration enabled */
    bool32 bDSCBlockPred;              /**< Display Stream Compression Block Prediction enabled */
    uint32 uDSCEncTopoConfig;          /**< Display Stream Compression Topology Configuration */

    HAL_MDP_DSC_EncoderConfigType   sDSCEncCfg;     /**< DSC Encoder Config */
    HAL_MDP_DSC_EncoderPacketType   sDSCEncPktType; /**< DSC Encoder Packet Type */
    HAL_MDP_DSC_EncParamsType       sDSCEncParams;  /**< DSC Encoder Params Type */

    /* DSI Controller Mapping Mode*/
    uint8  uControllerMapping[2];      /**< Define which DSI controller(s) will be used for this DSI panel */ 
    uint32 uControllerMappingMask;     /**< Bit Mask for uControllerMapping, bit0 is for dsi0,  bit1 is for dsi1*/ 

    /* DSI Escape clock configuration */
    uint32 uEscapeClockDivisor;        /**< Divisor when escape clock is sourced from the PLL, default is 0, source from CXO */
    uint32 uEscapeClockFreq;           /**< Escape clock frequency in Hz for timing adjustments, default is 0, CXO frequency */

    /* DSI Strength control */
    uint32 uDataStrengthLP;            /**< Drive strength of the LP data signal, chipset specific, leave as zero for default */
    uint32 uDataStrengthHS;            /**< Drive strength of the HS data signal, chipset specific, leave as zero for default */
    uint32 uClockStrengthHS;           /**< Drive strength of the HS clock signal, chipset specific, leave as zero for default*/


    /* DSI Command buffer */
    void  *pInitBuffer;                /**< DSI Init Sequence Command buffer */
    void  *pTermBuffer;                /**< DSI Termination Sequence Command buffer */
    void  *pDSCPpsBuffer;              /**< DSI DSC PPS Command Buffer*/
    uint32 aResetInfo[MDP_RESET_INFO_NUM];

    /* I2C Configuration */
    MDP_I2C_Configuration sI2CConfig;  /**< I2C Configuration*/
    
} DSIPanelInfoType;

/* DP Configuration
 */
typedef struct
{
  uint32                    uRefreshRate;         /**< DP refresh rate in Q16.16 representation. */
  uint32                    uPixelClockFrequency; /**< DP pixel clock frequency in Hertz(used to override uRefreshRate). */
  uint32                    uNumberOfLanes;       /**< Number of DP link lanes to use */
  MDP_DP_LinkTrainingType   eLinkTraining;        /**< Enable DP link training */
  bool32                    bASSREnable;          /**< Enable ASSR */
  bool32                    bEnhancedFrameEnable; /**< Enable Enhanced Frame */
  uint32                    uMaxLinkRate;         /**< Maximum acceptable link rate */
  bool32                    bReadDPCD;            /**< Flag to read DPCD from panel */
  bool32                    bReadEDID;            /**< Flag to read EDID from panel */
  uint32                    uLinkRateInKhz;       /**< Desired Link rate in Khz     */
  bool32                    bHPDActiveLow;        /**< Indicate if HPD from panel in active low */
  uint32                    uPowerupWaitinMs;     /**< Time to wait after panel powering up and before AUX operation.*/
  uint32                    uMaxAuxRetry;         /**< Number of retry before AUX operation success.                 */
  uint32                    uLaneSwingLevel;      /**< Lane voltage swing level                                      */
  uint32                    uPreemphasisLevel;    /**< Lane pre-emphasis level                                       */
  bool32                    bEnableSSCMode;       /**< Enable SSC mode                                               */
} DPPanelInfoType;

/* HDMI Configuration
 */
typedef struct
{
   bool32 bInterlaced;
   uint32 uRefreshRate;
  
} HDMIPanelInfoType;
  


/* Active Timing configuration
 */
typedef struct {
    uint32 uHsyncPulseWidthDclk;
    uint32 uHsyncBackPorchDclk;
    uint32 uHsyncFrontPorchDclk;
    uint32 uHsyncSkewDclk;
    uint32 uHLeftBorderDClk;       /**< Number of left border pixels  */
    uint32 uHRightBorderDClk;      /**< Number of right border pixels  */    
    uint32 uVsyncPulseWidthLns;
    uint32 uVsyncBackPorchLns;
    uint32 uVsyncFrontPorchLns;
    uint32 uVTopBorderLines;       /**< Number of top border lines */
    uint32 uVBottomBorderLines;    /**< Number of bottom border lines */
    uint32 uBorderColorRgb888;     /**< Border color used when bordering an active region  */
    uint32 uHsyncInvertSignal;
    uint32 uVsyncInvertSignal;
    uint32 uDataEnInvertSignal;
    uint32 bInterlaced;
} MDPActiveTimingType;


/*  Dynamic EDID configuration
*/
typedef struct
{
  bool32 bDynamicEDIDEnabled;  /**< Enable dynamic EDID */
  uint32 uI2CAddress;          /**< I2C Slave address */
  uint32 uI2CFrequency;        /**< I2C Frequency */
  uint32 uI2CGSBIPort;         /**< I2C GSBI Bus number */
  uint32 uPreferredTimingMode; /**< Preferred Timing Mode */
  uint32 uStartAddress;        /**< Starting address to read EDID info */
} MDP_EDID_Configuration;




/* MDP Display configuration
 */
typedef struct
{
  MDP_Display_IDType              eDisplayId;                                     /**< Display ID of this display */
  bool32                          bDetected;                                      /**< Panel has been detected */
  bool32                          bSWRenderer;                                    /**< Indicates display module is in a SW rendering mode */
  bool32                          bDisplayPwrState;                               /** <Indicates display is turned on/off */
  MDP_PixelFormatType             eColorFormat;                                   /**< Color format used by the display */
  uint32                          uNumMixers;                                     /**< Number of mixers driving this display. This also refers to number of DSPPs and PPs */
  uint32                          uNumInterfaces;                                 /**< Number of interfaces required for this display */
  MDP_ColorComponentsPackingType  eColorComponentsPacking;                        /**< Packing of color components - tight or loose */
  MDP_ColorComponentAlignmentType eColorComponentAlignment;                       /**< Color component alignment within its byte - MSB or LSB */
  MDP_DisplayConnectType          ePhysConnect;                                   /**< Physical connection of display. */
  MDP_RotateFlipType              ePhysPanelOrientation;                          /**< physical panel orientation */
  uint32                          uDisplayWidth;                                  /**< Display width */
  uint32                          uDisplayHeight;                                 /**< Display height */
  MDPActiveTimingType             sActiveTiming;                                  /**< Active Timing for video mode panels */
  MDP_EDID_Configuration          sDynamicEDIDConfig;                             /**< Dynamic EDID Configuration */
  uint32                          uBacklightLevel;                                /**< Current backlight level (in percentage 0-100) */
  uint32                          uModeId;                                        /**< Current mode */
  MDPSurfaceInfo                  sFrameBuffer;                                   /**< Current frame buffer */
  MDP_DisplayControlPathInfo      sDisplayControlPath[MDP_DUALPIPE_NUM_MIXERS];   /**< Control path configurations for each pipe */
  uint32                          uDefaultGPIOState[2][MDP_MAX_GPIO_LIST_ENTRIES];/**< Table of default GPIO states for low-0 and high-1 */
  MDP_Panel_BacklightType         eBacklightType;                                 /**< Configuration of the backlight interface module */
  uint32                          uUnderflowColor;                                /**< Underrun color */
  union {
    MDP_PmicBacklightControlType  eBacklightCtrl;                                 /**< PMIC module for backlight configuration  */
  } uBacklightCntrl;

  MDP_PmicModuleControlType       ePMICSecondaryPower;                            /**< Configuration for PMIC based secondary power source */
  MDPPlatformPanelType            eSelectedPanel;                                 /**< Internal ID for selected panel */  
  uint64                          uReservedAllocationSize;                        /**< Keep track of the size of the reserved region */
  uint32                          uRefreshRate;                                   /**< Panel refresh rate in Q16.16 representation */
  uint32                          uDynamicRefreshRates[MDP_MAX_NUM_DYNAMIC_REFRESH];/**< Panel dynamic refresh rate in Q16.16 representation*/

  union 
  {
    DSIPanelInfoType              sDsi;                                           /**< DSI Video/Command mode configuration */
    HDMIPanelInfoType             sHdmi;                                          /**< HDMI mode configuration */
    DPPanelInfoType               sDp;                                            /**< DP mode configuration */
  } uAttrs;
  
  MDP_Panel_FlagsType             eDisplayFlags;                                  /**< Miscellaneous display flags (See MDP_Panel_FlagsType). */
  uint32                          uPanelId;                                       /**< Panel id */
  uint32                          uResetGpio;                                     /**<  Reset gpio pin */

} MDP_Panel_AttrType;




/* MDP External Clocks Entry
 */
typedef struct
{
  CHAR8      szName[MDPLIB_MAX_CLOCK_NAME]; /* Clock name */
  UINT32     uClkSource;                    /* Primary Clock Source */
  UINT32     uClkSecondarySource;           /* Secondary Clock source */
  UINT32     nClkDiv;                       /* Clock pre-divider */
  UINT32     uClkPLL_M;                     /* Clock M value */
  UINT32     uClkPLL_N;                     /* Clock N value */
  UINT32     uClkPLL_2D;                    /* Clock 2D value */
  UINT32     nSourceDiv;                    /* Clock may need source divider */
} MDPExternalClockEntry;


/* MDP Clocks Entry
 */
typedef struct
{
  CHAR8                     szName[MDPLIB_MAX_CLOCK_NAME];    /* Clock name */
  UINT32                    nFreqHz;                          /* Requested Frequency */
  MDPExternalClockEntry     *pExtSrc;                         /* External source config */
  MDP_ClockFlags_Type       uFlags;                           /* Clock/Domain flags */  
} MDPClockEntry;

/* MDP Power Domain
 */
typedef struct
{
  CHAR8                     szName[MDPLIB_MAX_POWER_DOMAIN_NAME];    /* Power domain name */
  MDP_ClockFlags_Type       uFlags;                                  /* Clock/Domain flags */
} MDPPowerDomainEntry;

/*MDP resource list
*/
typedef struct {
  const MDPPowerDomainEntry *pPowerDomain;  /* Power Domain  */
  const MDPClockEntry       *pDispCCXOClk;  /* gcc_disp_cc_xo clock   */
  const MDPClockEntry       *pMDPClocks;    /* MDP clocks    */
  const MDPClockEntry       *pDSI0Clocks;   /* DSI0 clocks   */
  const MDPClockEntry       *pDSI1Clocks;   /* DSI1 clocks   */
  const MDPClockEntry       *pDPClocks;     /* DP clocks    */
  const MDPClockEntry       *pHDMIClocks;   /* HDMI clocks   */
} MDP_ResourceList;

/*MDP Ext clock resource list
*/
typedef struct {
  MDPExternalClockEntry     *pDSI0ExtClocks;           /* External source config for DSI0*/
  MDPExternalClockEntry     *pDSI1ExtClocks;           /* External source config for DSI1*/
  MDPExternalClockEntry     *pSharedSourceExtClocks;   /* External source config for dual DSI*/
  MDPExternalClockEntry     *pHDMISourceExtClocks;     /* External source config for HDMI*/
} MDP_ExtClockResourceList;

/*Display resource list
*/

typedef struct {
  MDP_ResourceList          *pResList;            /* MDP Resource List */
  MDP_ExtClockResourceList  *pExtClockResList;    /* MDP External clock Resource List */
} DisplayResourceList;

/*MDP resource list by MDP Minor Family
*/
typedef struct
{
  MDSS_Device_VersionType    hwVersion;   /* MDP_Device_Version*/
  DisplayResourceList       *pDisplayPlatfromResources; /* Display resources*/
} MDP_HwMinorFamilyResourceList;

/*MDP resource list by MDP Major Family
*/
typedef struct
{
  MDP_HwMinorFamilyResourceList *pMinorFamily; /*MDP Minor version based resource list*/  
} MDP_HwMajorFamilyResourceList;


#pragma pack(push)
#pragma pack(4)

/* UEFI parameters passed to the system variable
*/
typedef struct
{
  uint32 uVersionInfo;                /* Version info of this structure */
  uint32 uPlatformId;                 /* Identifies the current platform ID */   
  uint32 uChipsetId;                  /* Identifies the current platform chipset ID */    
  uint32 uPlatformVersion;            /* Platform version */
  /* Frame buffer information */
  uint32 uFrameBufferAddress;         /* Memory address of Frame buffer used by UEFI */
  uint32 uFrameBufferSize;            /* Size in bytes of total carved out frame buffer */
  uint32 uFrameBufferPixelDepth;      /* Image/source bit depth in bits (16, 24, etc) of Frame buffer used by UEFI */
  uint32 uFrameBufferImagePxWidth;    /* Image width on screen (in pixel) */
  uint32 uFrameBufferImagePxHeight;   /* Image height on screen (in pixel) */
  uint32 uFrameBufferImageStride;     /* Image plane stride */
  /* Primary panel configuration */
  uint32 uPrimaryPanelId;             /* Panel ID read from the primary panel */
  uint32 uPrimaryPanelFlags;          /* Primary display flags */
  uint32 uPrimaryPanelBacklightLevel; /* Primary panel backlight level in % */  
  /* External panel configuration */
  uint32 uExternalPanelFlags;         /* External display flags */
  uint32 uHighestBankBit;             /* Highest Bank Bit*/
  /* Display reserved memory information */
  uint64 uReservedMemoryAddress;      /* Reserved memory address */
  uint64 uReservedMemorySize;         /* Reserved memory size in bytes */
  /* Reserved */
  uint32 uReserved[11];               /* Reserved */
} MDPFirmwareEnvType;

#pragma pack(pop)


/* DSC Settings for different Profile ID
*/
typedef struct
{
  uint8  uEncodersNum;                /* Number of DSC encoders, 1 or 2         */
  bool32 bLMSplitEnable;              /* Layer mixer split enabled              */
  bool32 bBlockPredEnable;            /* Block Prediction Enabled               */
  uint32 uBitsPerComponent;           /* bits per component                     */
  uint32 uBitsPerPixel;               /* bits per pixel                         */
  uint32 uCompressionRatio;           /* Compression Ratio at this Profile ID   */
  uint32 uSliceHeight;                /* Slice height                           */
  uint32 uSliceWidth;                 /* Slice width                            */
}MDP_DscProfileMode;

extern const MDP_DscProfileMode gDscProfileModes[MDP_DSC_PROFILEID_MAX];

/* MDP SMP reservation information
 */
typedef struct 
{
  uint32       uBlockId;            /* SMP block ID */
  uint32       eClientId;           /* SMP client ID */
} MDP_SMPReservationType;

/* MDP SMP reservation table
 */
typedef struct
{
  uint32                          uSMPBlocks;              /* Number of SMP blocks available */
  uint32                          uNumReservations;        /* Number of SMP block reservations */
  const MDP_SMPReservationType   *pReservationTable;       /* SMP reservation table */
} MDP_SMPPoolInfoType;


/* MDP resolution capability information 
 */
typedef struct {
    uint32     uMinLayerWidthPx;                           /* Minimum layer width  */
    uint32     uMinLayerHeightPx;                          /* Minimum layer height */
    uint32     uMaxLayerWidthPx;                           /* Maximum layer width  */
    uint32     uMaxLayerHeightPx;                          /* Maximum layer height */
} MDP_ResolutionCaps;

/* MDP Ping Pong
*/
typedef struct {
    bool32   bSupported;            /* Is this PP supported for this chip */
    uint32   uMaxPPWidth;           /* Maximum width that can be supported using one ping-pong */
    uint32   uPPFeatureFlags;       /* Features supported by Ping-pong */
} MDP_PingPongCaps;

/* MDP device capabilities 
 */
typedef struct
{
  /* Resolution Caps */
  const MDP_ResolutionCaps   *pResolutionCaps;

  /* PingPong Caps */
  const MDP_PingPongCaps     *pPingPongCaps;
}MDP_DeviceCaps;

/* MDP HW private information
 */
typedef struct
{
  HAL_HW_VersionType             sMDPVersionInfo;          /* MDPVersion Information    */
  EFIChipInfoIdType              sEFIChipSetId;            /* EFI Chipset information   */
  EFIChipInfoFamilyType          sEFIChipSetFamily;        /* EFI Chipset family information */ 
  EFI_PLATFORMINFO_PLATFORM_TYPE eEFIPlatformInfo;         /* EFI Platform information */
  const MDP_DeviceCaps          *pDeviceCaps;              /* Device Capabilities */
  const HAL_MDP_PingPongId      *pPPSplitSlaveMap;         /* PingPong Split slave id mapping table */
} MDP_HwPrivateInfo;

/* MDSS address mappings structure
 */
typedef struct
{
  EFIChipInfoFamilyType          sEFIChipSetFamily;      /* EFI Chipset family information */
  uint32                         uMDSSBaseAddress;       /*MDSS base address*/       
} sMDSS_BaseAddressMappings;


/*
 * Panel Control command type.
 */
typedef enum {
    MDP_PANEL_COMMAND_INIT                  = 0x00000000,  /**< Send panel initialization sequence */
    MDP_PANEL_COMMAND_TERM,                                /**< Send panel termination sequence    */
    MDP_PANEL_COMMAND_CUSTOM,                              /**< Send custom panel sequence */
    MDP_PANEL_COMMAND_PPS,                                 /**< Send PPS command for DSC */
    MDP_PANEL_COMMAND_FORCE_32BIT           = 0x7FFFFFFF,
} MdpPanelCommandType;


/*
 * Software locks to prevent re-entrancy.
 */
typedef struct
{
  uint32    uLock[MDP_LOCK_TYPE_MAX];          /* Indicates whether Lock is free or acquired */
} MDP_LockInfo;

/* 
 * MDP_PllCodes and MDP_PLLCodesInfo should be 
 * matched with kernel and should not be modified
 */
#pragma pack(1)
/*
 * Dynamic refresh calibration codes
 */
typedef struct
{
  uint32 uValid;
  uint32 uVcoRate;
  uint32 uPllCodes[3];
} MDP_PllCodes;

/*
 * Dynamic refresh calibration pll codes information
 */
typedef struct
{
  uint32 uRefreshRateCount;
  MDP_PllCodes sPllCodes[MDP_MAX_NUM_DYNAMIC_REFRESH];
} MDP_PLLCodesInfo;

#pragma pack()

/*===========================================================================

                                Globals

===========================================================================*/

/* MDP Private data & Helper */
extern MDP_HwPrivateInfo                  gsMDPHwPrivateInfo;

#define MDP_GETPRIVATEINFO()              (MDP_HwPrivateInfo*)&gsMDPHwPrivateInfo;

/* Platform data */
extern EFI_PLATFORMINFO_PLATFORM_TYPE     gePlatformType;

/* MDP panel attribute */
extern MDP_Panel_AttrType                 gDisplayInfo[MDP_DISPLAY_MAX];

/* Helper to return the display panel attributes */
#define MDP_GET_DISPLAYINFO(_id_)         (&gDisplayInfo[((_id_)<MDP_DISPLAY_MAX)?(_id_):0])

/*===========================================================================

                               MDP Lib Functions

===========================================================================*/
/* ---------------------------------------------------------------------- */
/**
** FUNCTION: MDPSetupClocks()
** 
** DESCRIPTION:
**   Display Clock configuration.
**
** ---------------------------------------------------------------------- */
MDP_Status MDPSetupClocks(MDPClockTypes eClockType, MDPExternalClockEntry *pExtClockList);


/* ---------------------------------------------------------------------- */
/**
** FUNCTION: MDPDisableClocks()
** 
** DESCRIPTION:
**   Display Clock configuration
**
** ---------------------------------------------------------------------- */
MDP_Status MDPDisableClocks(MDPClockTypes eClockType);



/* ---------------------------------------------------------------------- */
/**
** FUNCTION: MDPDetectPanel()
** 
** DESCRIPTION:
**   Panel detection code, via XML, I2C or EDID
**
** ---------------------------------------------------------------------- */
MDP_Status MDPDetectPanel(MDP_Display_IDType eDisplayId, MDP_Panel_AttrType *pDisplayInfo);


/****************************************************************************
*
** FUNCTION: MDPSetupPipe()
*/
/*!
* \brief
*   Setup the MDP for a basic single layer operation
*
* \param [in] eDisplayId       - The display to initialize
*
* \retval MDP_Status
*
****************************************************************************/
MDP_Status  MDPSetupPipe(MDP_Panel_AttrType *pPanelConfig, MDPSurfaceInfo *pSurfaceInfo);


/****************************************************************************
*
** FUNCTION: MDPPanelInit()
*/
/*!
* \brief
*   This function setup the display for a particular mode.
*
* \param [in] uModeIndex    - The display mode to configure
*        [in] pPanelInfo    - The panel configuration
*
* \retval MDP_Status
*
****************************************************************************/
MDP_Status  MDPPanelInit(uint32 uModeIndex, MDP_Panel_AttrType    *pPanelInfo);


/****************************************************************************
*
** FUNCTION: MDPPanelDeInit()
*/
/*!
* \brief
*   This function de-initializes the panel interfaces.
*
* \param  [in] pPanelInfo    - The panel configuration
*
* \retval MDP_Status
*
****************************************************************************/
MDP_Status  MDPPanelDeInit(MDP_Panel_AttrType    *pPanelInfo);

/****************************************************************************
*
** FUNCTION: MDPPanelGetEDID()
*/
/*!
* \brief
*   This function read EDID info from panel.
*
* \param [in] pPanelInfo    - The panel configuration
*
* \retval MDP_Status
*
****************************************************************************/
MDP_Status  MDPPanelGetEDID( MDP_Panel_AttrType    *pPanelInfo);

/****************************************************************************
*
** FUNCTION: MDPStartPipe()
*/
/*!
* \brief
*   Start the MDP pipe 
*
* \param [in] pPanelConfig       - The panel configuration
*
* \retval MDP_Status
*
****************************************************************************/
MDP_Status  MDPStartPipe(MDP_Panel_AttrType *pPanelConfig);



/****************************************************************************
*
** FUNCTION: MDPStopPipe()
*/
/*!
* \brief
*   Stop the MDP pipe 
*
* \param [in] pPanelConfig       - The panel configuration
*
* \retval MDP_Status
*
****************************************************************************/
MDP_Status  MDPStopPipe(MDP_Panel_AttrType *pPanelConfig);

/****************************************************************************
*
** FUNCTION: MDPOutputConstantColor()
*/
/*!
* \brief
*   Configure MDP to enable/disable constant color output to panel
*
*   COMMAND MODE:
*      bEnabled = TRUE  :  Set up MDP color fill and trigger one frame to panel
*      bEnabled = FALSE :  Remove MDP color fill settings
*   VIDEO MODE:
*      bEnabled = TRUE  :  Set up MDP color fill and enable interface
*      bEnabled = FALSE :  Remove MDP color fill settings
*
* \param [in] pPanelConfig       - The panel configuration
* \param [in] uConstantColor     - Constant color to output
* \param [in] bEnabled           - Flag to enable/disable constant color output
*
* \retval MDP_Status
*
****************************************************************************/
MDP_Status  MDPOutputConstantColor(MDP_Panel_AttrType *pPanelConfig, uint32 uConstantColor, bool32 bEnabled);

/****************************************************************************
*
** FUNCTION: MDPInitHwPrivateInfo()
*/
/*!
* \brief
*   Initialize MDP Hw Private info based on device version
*
*
* \retval MDP_Status
*
****************************************************************************/

MDP_Status MDPInitHwPrivateInfo(MDP_HwPrivateInfo *pHwPrivateInfo);

/****************************************************************************
*
** FUNCTION: MDPPanelSendCommandSequence()
*/
/*!
* \brief
*   Sends a predefined command set to the panel.
*
*
* \retval MDP_Status
*
****************************************************************************/

MDP_Status  MDPPanelSendCommandSequence(MDP_Panel_AttrType *pPanelInfo, MdpPanelCommandType eCommand, void *pCustomCommand, uint32 uFlags);

/* ---------------------------------------------------------------------- */
/**
** FUNCTION: MDPGetExternalClockList()
**
** DESCRIPTION:
**   Return list of external clocks for this chipset
**
** ---------------------------------------------------------------------- */
MDP_ExtClockResourceList *MDPGetExternalClockList(void);

/****************************************************************************
*
** FUNCTION: MDPGetFlushFlags()
*/
/*!
* \brief
*     Helper to calculate the pixel extension length
*
* \param [in] pPanelInfo          - The panel configuration
*        [in] uMixerIndex         - Mixer ID (left or right mixer)
*        [in] uFlushMask          - Mask for which modules' flush bits are to be returned
*
* \retval int32 - Flash mask bits (HAL_ControlPath_Flush)
*
****************************************************************************/ 
uint32 MDPGetFlushFlags(MDP_Panel_AttrType         *pPanelInfo,
                        uint32                     uMixerIndex, 
                        MDP_FlushModule_Type       uFlushMask);



#endif // __MDPLIB_I_H__

