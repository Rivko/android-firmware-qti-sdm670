#ifndef HALDSI_H
#define HALDSI_H
/*=============================================================================

  File: HALdsi.h
  

     Copyright (c) 2010-2014, 2017-2018 Qualcomm Technologies, Inc.  All Rights Reserved.
     Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================
*                         INCLUDE FILES
============================================================================*/
#include "MDPTypes.h"
#include "HALdsi_reg.h"


/* --- DO NOT ADD HEADER FILES HERE! --- */

/* -----------------------------------------------------------------------
** Macros and Defines
-------------------------------------------------------------------------- */
#define HAL_DSI_BIT(x)          ((uint32)(1<<(x)))

/*!
 * \b DSI_Datatype
 *
 * Define the DSI data type, it could be different than the DSI standard.
 */
#define DSI_Datatype    uint8 

/* Number of retry for DSI PLL Enabling */
#define DSI_PLL_ENABLE_RETRY_COUNT                  5

/* Data type */
#define DSI_VIDEO_DT_VSYNCSTART                     0x01
#define DSI_VIDEO_DT_VSYNCEND                       0x11
#define DSI_VIDEO_DT_HSYNCSTART                     0x21
#define DSI_VIDEO_DT_HSYNCEND                       0x31


#define DSI_CMDMODE_DT_COLOROFF                     0x02
#define DSI_CMDMODE_DT_COLORON                      0x12
#define DSI_CMDMODE_DT_SHUTDOWN                     0x22
#define DSI_CMDMODE_DT_TURNON                       0x32

#define DSI_CMDMODE_DT_GENERICWRITE0PARA            0x03
#define DSI_CMDMODE_DT_GENERICWRITE1PARA            0x13
#define DSI_CMDMODE_DT_GENERICWRITE2PARA            0x23

#define DSI_CMDMODE_DT_GENERICREAD0PARA             0x04
#define DSI_CMDMODE_DT_GENERICREAD1PARA             0x14
#define DSI_CMDMODE_DT_GENERICREAD2PARA             0x24

#define DSI_CMDMODE_DT_DSCWRITE0PARA                0x05
#define DSI_CMDMODE_DT_DSCWRITE1PARA                0x15

#define DSI_CMDMODE_DT_DSCREAD0PARA                 0x06

#define DSI_CMDMODE_DT_SETMAXRETSIZE                0x37

#define DSI_CMDMODE_DT_EOTP                         0x08

#define DSI_CMDMODE_DT_NULLPACKET                   0x09
#define DSI_VIDEO_DT_BLANKPACKET                    0x19
#define DSI_CMDMODE_DT_GENERICLONGWRITE             0x29
#define DSI_CMDMODE_DT_DSCLONGWRITE                 0x39 

#define DSI_VIDEO_DT_PACKEDRGB565                   0x0E
#define DSI_VIDEO_DT_PACKEDRGB666                   0x1E
#define DSI_VIDEO_DT_LOOSEPACKRGB666                0x2E
#define DSI_VIDEO_DT_PACKEDRGB888                   0x3E

#define DSI_CMDMODE_DT_RETACKWITHERROR              0x02
#define DSI_CMDMODE_DT_RETGENERICSHORTREAD1PARA     0x11
#define DSI_CMDMODE_DT_RETGENERICSHORTREAD2PARA     0x12
#define DSI_CMDMODE_DT_RETGENERICLONGEADPARA        0x1A
#define DSI_CMDMODE_DT_RETDSCLONGREAD               0x1C
#define DSI_CMDMODE_DT_RETDSCSHORTREAD1PARA         0x21
#define DSI_CMDMODE_DT_RETDSCSHORTREAD2PARA         0x22

#define DSI_CMDMODE_DT_EOTPWC                       0x0F0F 
#define DSI_CMDMODE_DT_EOTPECC                      0x01 

#define DSI_CMDMODE_DT_DCSWRITEMEMSTART             0x2C
#define DSI_CMDMODE_DT_DCSWRITEMEMCONTINUE          0x3C
#define DSI_VIDEO_DT_BLANKPACKETDATA                0x00

#define DSI_DATATYPE_LONGPACKET                     0x09
#define DSI_DATATYPE_VIDEOPACKET                    0x0E
#define DSI_DATATYPE_PPSPACKET                      0x0A


/*Command out */
#define HAL_DSI_CMDMODE_DMA_NON_EMBEDED                 0x00
#define HAL_DSI_CMDMODE_DMA_EMBEDED                     0x01

#define HAL_DSI_EMBEDDED_HEADER_SIZE                    0x04
#define HAL_DSI_EMBEDDED_SHORTPACKET_PAYLOAD_SIZE       0x02

#define HAL_DSI_EMBEDDED_SHORTPACKET_TYPE               0x00
#define HAL_DSI_EMBEDDED_LONGPACKET_TYPE                0x01

#define HAL_DSI_CMDMODE_DMA_PACKET_QW_SHFT              0x07
#define HAL_DSI_CMDMODE_DMA_PACKET_QW_BMSK              0x80

#define HAL_DSI_CMDMODE_DMA_PACKET_SL_SHFT              0x06
#define HAL_DSI_CMDMODE_DMA_PACKET_SL_BMSK              0x40

#define HAL_DSI_CMDMODE_DMA_PACKET_BTA_SHFT             0x05
#define HAL_DSI_CMDMODE_DMA_PACKET_BTA_BMSK             0x20

#define HAL_DSI_CMDMODE_DMA_PACKET_VC_SHFT              0x06
#define HAL_DSI_CMDMODE_DMA_PACKET_VC_BMSK              0xC0

#define HAL_DSI_CMDMODE_DMA_PACKET_DT_SHFT              0x00
#define HAL_DSI_CMDMODE_DMA_PACKET_DT_BMSK              0x3F

/* DSI read command raw packet flag for the DSI read to return raw packet
 *      QDI defines QDI_DSI_CMD_FLAG_READ_RAW_PACKET in qdi_oem.h for API QDI_DisplayUtil_ReadCmdBuf()
 *      to indicate raw DSI packet is expected. This flag will be directly passed in to DSIDriver_Read() 
 *      from OEM panel driver and then translated to HAL flag defined below which be used in HAL_DSI_ReadData().
 */
#define HAL_DSI_CMD_FLAG_READ_RAW_PACKET                0x01          /* flag for DSI read to return raw packet */

#define HAL_DSI_ESCCLK_SRC                              19.2f  // Escape clock value in Mhz

#define HAL_DSI_FLAG_DATA_FETCH_VIA_FIFO                0x01 

/* Define the maximum number of the supported dynamic refresh rate
*/
#define MAX_NUM_DYNAMIC_REFRESH                         16

/* ------------------------------------------------------------------------
Enumerated types
---------------------------------------------------------------------------*/

/*!
 * \b HAL_dsi_StatusType
 *
 * Define error codes for debugging the HAL.
 */
typedef enum
{
  HAL_DSI_SUCCESS = 0,
  HAL_DSI_FAILED_NO_HW_SUPPORT,
  HAL_DSI_FAILED_INVALID_INPUT_PARAMETER,
  HAL_DSI_FAILED_NO_SUPPORT_OR_NO_HW,
  HAL_DSI_FAILED_INTERRUPTS_UNMAPPED,
  HAL_DSI_FAILED_EXPECTED_NON_NULL_PTR,
  HAL_DSI_FAILED_FXN_NOT_SUPPORTED,
  HAL_DSI_FAILED_PARAMETER_OUT_OF_RANGE,
  HAL_DSI_FAILED_NUMBER_OF_TABLE_ENTRIES,
  HAL_DSI_FAILED_ZERO_AREA_WINDOW_SPECIFIED,
  HAL_DSI_FAILED_BAD_CONNECTION_PARAMETER,
  HAL_DSI_FAILED_DISPLAY_FILL_NOT_SUPPORTED,
  HAL_DSI_FAILED_NO_SUPPORT_FOR_COLOR_FORMAT,
  HAL_DSI_FAILED_YSTRIDE_TOO_LARGE_FOR_HW,
  HAL_DSI_FAILED_IMAGE_SIZE_TOO_BIG_FOR_HW,
  HAL_DSI_FAILED_HW_IS_STILL_ACTIVE,
  HAL_DSI_FAILED_SW_IMPLEMENTATION_ERR,
  HAL_DSI_FAILED_ADDRESS_NOT_MEMORY_ALIGNED,
  HAL_DSI_FAILED_CANNOT_CLEAR_UNMAPPED_INTERRUPTS,
  HAL_DSI_FAILED_SW_DRIVER_DOES_NOT_MATCH_HW,
  HAL_DSI_FAILED_HW_OPTION_OBSOLETE,
  HAL_DSI_FAILED_UNABLE_TO_INIT_HW,
  HAL_DSI_FAILED_PACKET_CORRUPTED,
  HAL_DSI_FAILED_BUFFER_TOO_SMALL
} HAL_dsi_StatusType;

/*!
 * \b DSI_Device_IDType
 *
 * The Logic ID for DSI Core HW Device. Different HW may have different set
 * of DSI Device number.
 */
typedef enum
{
  HAL_DSI_Clock_AHBM_SCLK                 = HAL_DSI_BIT(0),
  HAL_DSI_Clock_PCLK                      = HAL_DSI_BIT(1),
  HAL_DSI_Clock_MAINCLK                   = HAL_DSI_BIT(2),
}HAL_DSI_ClockIDType;

/*!
 * \b HAL_DSI_StatusType
 *
 * Device Status.
 */
typedef enum
{
  DSI_STATUS_DEVICE_BUSY_CMDENGINE        = HAL_DSI_BIT(0),
  DSI_STATUS_DEVICE_BUSY_CMDENGINEDMA     = HAL_DSI_BIT(1),
  DSI_STATUS_DEVICE_BUSY_CMDENGINEMDP     = HAL_DSI_BIT(2),
  DSI_STATUS_DEVICE_BUSY_VIDEOENGINE      = HAL_DSI_BIT(3),
  DSI_STATUS_DEVICE_BUSY_BTA              = HAL_DSI_BIT(4),
  DSI_STATUS_DEVICE_BUSY_GENERICTRIG      = HAL_DSI_BIT(5),
  DSI_STATUS_DEVICE_BUSY_PHYRESET         = HAL_DSI_BIT(6)
}HAL_DSI_StatusType;


/*!
 * \b HAL_DSI_LaneStatusType
 *
 * Device lane status.
 */
typedef enum
{
  DSI_LANE_STATUS_DLN0_STOPSTATE         = HAL_DSI_BIT(0),
  DSI_LANE_STATUS_DLN1_STOPSTATE         = HAL_DSI_BIT(1),
  DSI_LANE_STATUS_DLN2_STOPSTATE         = HAL_DSI_BIT(2), 
  DSI_LANE_STATUS_DLN3_STOPSTATE         = HAL_DSI_BIT(3), 
  DSI_LANE_STATUS_CLKLN_STOPSTATE        = HAL_DSI_BIT(4), 
  /* Bits 5:7 are reserved */
  DSI_LANE_STATUS_DLN0_ULPS_NOT_ACTIVE   = HAL_DSI_BIT(8), 
  DSI_LANE_STATUS_DLN1_ULPS_NOT_ACTIVE   = HAL_DSI_BIT(9),  
  DSI_LANE_STATUS_DLN2_ULPS_NOT_ACTIVE   = HAL_DSI_BIT(10),  
  DSI_LANE_STATUS_DLN3_ULPS_NOT_ACTIVE   = HAL_DSI_BIT(11),  
  DSI_LANE_STATUS_CLKLN_ULPS_NOT_ACTIVE  = HAL_DSI_BIT(12),
  /* Bits 13:15 are reserved */
  DSI_LANE_STATUS_DLN0_DIRECTION         = HAL_DSI_BIT(16)
}HAL_DSI_LaneStatusType;


/*!
 * \b HAL_DSI_HWBlockType
 *
 * DSI HW Block ID, used to enable/disable individual block,.
 */
typedef enum
{
  HAL_DSI_HWBLOCK_NONE                    = HAL_DSI_BIT(0),
  HAL_DSI_HWBLOCK_CONTROLLER              = HAL_DSI_BIT(1),
  HAL_DSI_HWBLOCK_VIDEOENGINE             = HAL_DSI_BIT(2),
  HAL_DSI_HWBLOCK_CMDMODEENGINE           = HAL_DSI_BIT(3),
  HAL_DSI_HWBLOCK_MAX,
  HAL_DSI_HWBLOCK_FORCE_32BIT = 0x7FFFFFFF
}HAL_DSI_HWBlockType;


/*!
 * \b HAL_DSI_InterruptSourceType
 *
 * Define the type of interrupt to enable/clear.
 */
typedef enum
{
  HAL_DSI_INTERRUPT_NONE                     = 0,

  /* Command mode engine has finished transferring DMA data to panel  */
  HAL_DSI_INTERRUPT_CMDMODE_DMA_DONE         = HAL_DSI_BIT(0),

  /* Command mode engine has finished transferring deng data to panel */
  HAL_DSI_INTERRUPT_CMDMODE_MDP_DONE         = HAL_DSI_BIT(1),

  /* Video mode engine has finished transferring data to panel        */
  HAL_DSI_INTERRUPT_VIDEOMODE_DONE           = HAL_DSI_BIT(2), 

  /* Video mode engine has finished transferring data to panel        */
  HAL_DSI_INTERRUPT_DSI_ERROR                = HAL_DSI_BIT(3), 

  /* Peripheral returns the bus to host, HW read is done */
  HAL_DSI_INTERRUPT_BTA_DONE                 = HAL_DSI_BIT(4),

  HAL_DSI_INTERRUPT_UNMAPPED                 = HAL_DSI_BIT(30),
} HAL_DSI_InterruptSourceType;

/*!
 * \b HAL_DSI_ErrorSourceType
 *
 * Define the type of Error Mask to enable/clear.
 */
typedef enum
{
  HAL_DSI_ERROR_INT_NONE                     = 0,
  /* DSI PHY Error              */
  HAL_DSI_ERROR_INT_DPHY_DSIPLL_UNLOCK       = HAL_DSI_BIT(0), 
  /* DSI DLN0 PHY Error         */
  HAL_DSI_ERROR_INT_DLN0_CONTENTION_LP1      = HAL_DSI_BIT(1),
  HAL_DSI_ERROR_INT_DLN0_CONTENTION_LP0      = HAL_DSI_BIT(2),
  HAL_DSI_ERROR_INT_DLN0_CONTROL             = HAL_DSI_BIT(3),
  HAL_DSI_ERROR_INT_DLN0_SYNC_ESC            = HAL_DSI_BIT(4),
  HAL_DSI_ERROR_INT_DLN0_ESC                 = HAL_DSI_BIT(5),
  
  /* DSI FIFO Error             */
  HAL_DSI_ERROR_INT_DLN0_LP_FIFO_OVERFLOW    = HAL_DSI_BIT(6),
  HAL_DSI_ERROR_INT_DLN0_HS_FIFO_OVERFLOW    = HAL_DSI_BIT(7),
  HAL_DSI_ERROR_INT_DLN1_HS_FIFO_OVERFLOW    = HAL_DSI_BIT(8),
  HAL_DSI_ERROR_INT_DLN2_HS_FIFO_OVERFLOW    = HAL_DSI_BIT(9),
  HAL_DSI_ERROR_INT_DLN3_HS_FIFO_OVERFLOW    = HAL_DSI_BIT(10),

  /* DSI VIDEO FIFO Error       */
  HAL_DSI_ERROR_INT_VID_MDP_FIFO_UNDERFLOW   = HAL_DSI_BIT(11),
  HAL_DSI_ERROR_INT_VID_MDP_FIFO_OVERFLOW    = HAL_DSI_BIT(12),

  /* DSI COMMAND MODE FIFO Error*/
  HAL_DSI_ERROR_INT_CMD_MDP_FIFO_UNDERFLOW   = HAL_DSI_BIT(13),
  HAL_DSI_ERROR_INT_CMD_DMA_FIFO_UNDERFLOW   = HAL_DSI_BIT(14),

  /*DSI STATUS ERROR            */
  HAL_DSI_ERROR_INT_INTERLEAVE_OP_CONTENTION = HAL_DSI_BIT(15),

  /* DSI TIMEOUT Error          */
  HAL_DSI_ERROR_INT_BTA_TO                   = HAL_DSI_BIT(16),
  HAL_DSI_ERROR_INT_HS_TX_TO                 = HAL_DSI_BIT(17),
  HAL_DSI_ERROR_INT_LP_RX_TO                 = HAL_DSI_BIT(18),

  /* DSI ACK Error              */
  HAL_DSI_ERROR_INT_PACKET                   = HAL_DSI_BIT(19),
  HAL_DSI_ERROR_INT_RDBK_INCOMPLETE_PACKET   = HAL_DSI_BIT(20),
  HAL_DSI_ERROR_INT_RDBK_DATA_CRC            = HAL_DSI_BIT(21),
  HAL_DSI_ERROR_INT_RDBK_DATA_MULTI_ECC      = HAL_DSI_BIT(22),
  HAL_DSI_ERROR_INT_RDBK_DATA_ECC            = HAL_DSI_BIT(23),

  HAL_DSI_ERROR_INT_UNMAPPED                 = HAL_DSI_BIT(30),
} HAL_DSI_ErrorSourceType;

/*!
 * \b HAL_DSI_AckErrorSourceType
 *
 * ACK Error Status returned from Panel.
 */
typedef enum
{
  HAL_DSI_ACK_ERROR_NONE                     = 0,
  HAL_DSI_ACK_ERROR_PANEL_SPECIFIC_ERR       = HAL_DSI_BIT(0),
  HAL_DSI_ACK_ERROR_PROTOCOL_VIOLATION       = HAL_DSI_BIT(1),
  HAL_DSI_ACK_ERROR_VC_ERR                   = HAL_DSI_BIT(2),
  HAL_DSI_ACK_ERROR_DT_ERR                   = HAL_DSI_BIT(3),
  HAL_DSI_ACK_ERROR_CRC_ERR                  = HAL_DSI_BIT(4),
  HAL_DSI_ACK_ERROR_MULTI_ECC_ERR            = HAL_DSI_BIT(5),
  HAL_DSI_ACK_ERROR_SINGLE_ECC_ERR           = HAL_DSI_BIT(6),
  HAL_DSI_ACK_ERROR_CONTENTION               = HAL_DSI_BIT(7),
  HAL_DSI_ACK_ERROR_FALSE_CTRL_ERR           = HAL_DSI_BIT(8),
  HAL_DSI_ACK_ERROR_TIMEOUT                  = HAL_DSI_BIT(9),
  HAL_DSI_ACK_ERROR_LP_ERR                   = HAL_DSI_BIT(10),
  HAL_DSI_ACK_ERROR_ESC_ERR                  = HAL_DSI_BIT(11),
  HAL_DSI_ACK_ERROR_EOT_ERR                  = HAL_DSI_BIT(12),
  HAL_DSI_ACK_ERROR_SOT_SYNC_ERR             = HAL_DSI_BIT(13),
  HAL_DSI_ACK_ERROR_SOT_ERR                  = HAL_DSI_BIT(14),
}HAL_DSI_AckErrorSourceType;

/*!
 * \b DSI_Device_IDType
 *
 * The Logic ID for DSI Core HW Device. Different HW may have different set
 * of DSI Device number.The HOST ID must start from 0, and increase one by one.
 */
typedef enum
{
  DSI_DeviceID_0            = 0,
  DSI_DeviceID_1,
  DSI_DeviceID_MAX,
  DSI_DeviceID_FORCE_32BIT  = 0x7FFFFFFF
}DSI_Device_IDType;

/*!
 * \b DSI_LaneIDType
 *
 * The Logic ID for DSI lane.
 */
typedef enum
{
  DSI_LaneID_0           = 0,
  DSI_LaneID_1,
  DSI_LaneID_2,
  DSI_LaneID_3,
  DSI_LaneID_CLK,
  DSI_LaneID_Max,
  DSI_LaneID_FORCE_32BIT = 0x7FFFFFFF
}DSI_LaneIDType;

/*!
 * \b DSI_Channel_IDType
 *
 * The Logic ID for DSI Client panel. Multiply display panel can be attached 
 * to one DSI Host core.
 */
typedef enum
{
  DSI_ChannelID_NONE = 0,
  DSI_ChannelID_Video0,
  DSI_ChannelID_CMD0,
  DSI_ChannelID_CMD1,
  DSI_ChannelID_MAX,
  DSI_ChannelID_FORCE_32BIT = 0x7FFFFFFF
}DSI_Channel_IDType;

/*!
 * \b DSI_LaneMapType
 *
 * Data Lane Enum.
 * According to MIPI DSI spec, each DSI controlled can have up to 4 data lane and,
 * one clock lane.
 * 
 */
typedef enum
{
  DSI_LaneMap_NONE = 0,
  DSI_LaneMap_0,  
  DSI_LaneMap_1,
  DSI_LaneMap_2,
  DSI_LaneMap_3,
  DSI_LaneMap_MAX,
  DSI_LaneMap_32BIT = 0x7FFFFFFF
}DSI_LaneMapType;

/*!
 * \b DSI_TrafficModeType
 *
 * enum 3 video mode traffic modes defined by MIPI DSI.
 * Video mode only
 */
typedef enum
{
  DSI_Video_TrafficMode_NonBurst_VSPulse = 0,/**< Non-Burst w/ vsync pulse */
  DSI_Video_TrafficMode_NonBurst_VSEvent,    /**< Non-Burst w/ vsync event */
  DSI_Video_TrafficMode_Burst,               /**< Burst mode               */
  DSI_Video_TrafficMode_MAX,
  DSI_Video_TrafficMode_FORCE_32BIT = 0x7FFFFFFF
}DSI_TrafficModeType;

/*!
 * \b DSI_ImageTriggerType
 *
 * Trigger to display engine(MDP) to start transmission of pixel data.
 * Command mode only
 * 
 */
typedef enum
{
  DSI_Display_ImageTrigger_NONE = 0,      /**< Free Run, if MDP has data     */
  DSI_Display_ImageTrigger_TE,            /**< TE from Panel will trigger    */
  DSI_Display_ImageTrigger_SW,            /**< SW(DSI_CMD_MODE_DMA_SW_TRIGGER*/
  DSI_Display_ImageTrigger_SW_TE,         /**< Both SW and TE are needed     */
  DSI_Display_ImageTrigger_MAX,
  DSI_Display_ImageTrigger_FORCE_32BIT = 0x7FFFFFFF
}DSI_ImageTriggerType;

/*!
 * \b DSI_CMDTriggerType             TODO move to HALComdefDSI.h
 *
 * Trigger to DMA engine to start Command packet output from memory in embedded
 * mode or non-Embedded mode.
 * Command mode only
 * 
 */
typedef enum
{
  DSI_Display_CMDTrigger_NONE = 0,      /**< No Trigger                      */
  DSI_Display_CMDTrigger_SOFEOF,        /**< Start of Frame or End of Frame  */
  DSI_Display_CMDTrigger_TE,            /**< TE from Panel                   */
  DSI_Display_CMDTrigger_SW,            /**< SW(DSI_CMD_MODE_MDP_SW_TRIGGER  */
  DSI_Display_CMDTrigger_SW_SOFEOF,     /**< Both SW and SOF/EOF are needed  */
  DSI_Display_CMDTrigger_SW_TE,         /**< Both SW and TE are needed       */
  DSI_Display_CMDTrigger_MAX,
  DSI_Display_CMDTrigger_FORCE_32BIT = 0x7FFFFFFF
}DSI_CMDTriggerType;

/*!
 * \b DSI_ExtTEModeType 
 *
 * The signal property o dedicated TE pin from panel.
 * Define what is conveyed from TE pin, VSync only or both Vsync and HSync combined,
 * also define what is the active method of signal.
 * Command mode only
 */
typedef enum
{
  DSI_ExtTEMode_NONE  = 0,    /**< No dedicated TE pin          */
  DSI_ExtTEMode_VS_EDGE,      /**< edge active, Vsync only      */
  DSI_ExtTEMode_VS_WIDTH,     /**< level active, Vsync only     */
  DSI_ExtTEMode_VSHS_EDGE,    /**< edge active, Vsync and HSync */
  DSI_ExtTEMode_VSHS_WIDTH,   /**< level active,Vsync and HSync */
  DSI_ExtTEMode_MAX,
  DSI_ExtTEMode_FORCE_32BIT = 0x7FFFFFFF
}DSI_ExtTEModeType;

/*!
 * \b DSI_Display_VCType
 *
 * Virtual Channel Enum.
 * According to MIPI DSI spec, each DSI controlled can server up to 4 panels,
 * DSI_Display_VC_0 should be always used if any panel will be used.
 * 
 */
typedef enum
{
  DSI_Display_VC_0  = 0x0,
  DSI_Display_VC_1  = 0x1, 
  DSI_Display_VC_2  = 0x2,
  DSI_Display_VC_3  = 0x3,
  DSI_Display_VC_MAX,
  DSI_Display_VC_FORCE_32BIT = 0x7FFFFFFF
}DSI_Display_VCType;


/*!
 * \b DSI_Display_ClockLaneType
 *
 * Clock lane mode configuration.  This configuration is used to control the behavior of 
 * the clock lanes during normal operations.
 */
typedef enum
{
  DSI_Display_ClockLane_Default = 0x0,          /* Clock lane will transition from LP11 to HS only when HS data is being transferred */
  DSI_Display_ClockLane_ForceHS,                /* Clock lane will always be driven to HS */
  DSI_Display_ClockLane_ForceHS_PostInit,       /* Clock lane will always be driven to HS once the panel init sequence has been completed */
  DSI_Display_ClockLane_MAX,
  DSI_Display_ClockLane_FORCE_32BIT = 0x7FFFFFFF
}DSI_Display_ClockLaneType;


/*!
 * \b HAL_DSI_TriggerFlagType
 *
 * Flag parameters for Trigger function.
 */
typedef enum
{
  DSI_TriggerFlag_NONE       =0,
  DSI_TriggerFlag_DMA_SW,
  DSI_TriggerFlag_MDP_SW,
  DSI_TriggerFlag_BTA_SW,
  DSI_TriggerFlag_RESET_SW,
  DSI_TriggerFlag_RESERVED,
  DSI_TriggerFlag_MAX,
  DSI_TriggerFlag_FORCE_32BIT = 0x7FFFFFFF
}HAL_DSI_TriggerFlagType;

/*!
 * \b DSI_ColorFormatType
 *
 * enum 3 video mode traffic modes defined by MIPI DSI.
 * Video mode only
 */
typedef enum
{
  DSI_COLOR_RGB_111_3BPP = 0,
  DSI_COLOR_RBG_111_3BPP,
  DSI_COLOR_BGR_111_3BPP,
  DSI_COLOR_BRG_111_3BPP,
  DSI_COLOR_GRB_111_3BPP,
  DSI_COLOR_GBR_111_3BPP,
  
  DSI_COLOR_RGB_332_8BPP,
  DSI_COLOR_RBG_332_8BPP,
  DSI_COLOR_BGR_332_8BPP,
  DSI_COLOR_BRG_332_8BPP,
  DSI_COLOR_GRB_332_8BPP,
  DSI_COLOR_GBR_332_8BPP,
  
  DSI_COLOR_RGB_444_12BPP,
  DSI_COLOR_RBG_444_12BPP,
  DSI_COLOR_BGR_444_12BPP,
  DSI_COLOR_BRG_444_12BPP,
  DSI_COLOR_GRB_444_12BPP,
  DSI_COLOR_GBR_444_12BPP,
  
  DSI_COLOR_RGB_565_16BPP,
  DSI_COLOR_RBG_565_16BPP,
  DSI_COLOR_BGR_565_16BPP,
  DSI_COLOR_BRG_565_16BPP,
  DSI_COLOR_GRB_565_16BPP,
  DSI_COLOR_GBR_565_16BPP,
  
  DSI_COLOR_RGB_666_18BPP,
  DSI_COLOR_RBG_666_18BPP,
  DSI_COLOR_BGR_666_18BPP,
  DSI_COLOR_BRG_666_18BPP,
  DSI_COLOR_GRB_666_18BPP,
  DSI_COLOR_GBR_666_18BPP,

  DSI_COLOR_RGB_666_24BPP,
  DSI_COLOR_RBG_666_24BPP,
  DSI_COLOR_BGR_666_24BPP,
  DSI_COLOR_BRG_666_24BPP,
  DSI_COLOR_GRB_666_24BPP,
  DSI_COLOR_GBR_666_24BPP,

  DSI_COLOR_RGB_888_24BPP,
  DSI_COLOR_RBG_888_24BPP,
  DSI_COLOR_BGR_888_24BPP,
  DSI_COLOR_BRG_888_24BPP,
  DSI_COLOR_GRB_888_24BPP,
  DSI_COLOR_GBR_888_24BPP,
 
}DSI_ColorFormatType;


/*!
 * \b DSI_LaneOperationType
 *
 * DSI lane operations
 */
typedef enum
{
    DSI_LANE_OP_NONE,
    DSI_LANE_OP_CLOCK_HS_FORCE_REQUEST,              /**< force clock lane to enter HS mode always */
    DSI_LANE_OP_CLOCK_HS_FORCE_REQUEST_CLEAR,        /**< clear request to force clock lane to enter HS mode */
    DSI_LANE_OP_ULPS_REQUEST,                        /**< request DSI lane enter ULPS */
    DSI_LANE_OP_ULPS_REQUEST_CLEAR,                  /**< clear DSI lane enter ULPS request */
    DSI_LANE_OP_ULPS_EXIT,                           /**< request DSI lane exit ULPS */
    DSI_LANE_OP_ULPS_EXIT_CLEAR,                     /**< clear DSI lane exit ULPS request */
    DSI_LANE_OP_FORCE_TX_STOP,                       /**< force DSI lane to go into transmit mode */
    DSI_LANE_OP_FORCE_TX_STOP_CLEAR,                 /**< clear request to force DSI lane to go into transmit mode */
    DSI_LANE_OP_FORCE_32BIT = 0x7FFFFFFF
} DSI_LaneOperationType;

/* -----------------------------------------------------------------------
** Macros and Defines
-------------------------------------------------------------------------- */
/*!
 * \b HAL_DSI_INTERRUPT_ALL
 *
 *  Mask of all the interrupt supported.
 */
#define HAL_DSI_INTERRUPT_ALL ( HAL_DSI_INTERRUPT_CMDMODE_DMA_DONE          |\
                                HAL_DSI_INTERRUPT_CMDMODE_MDP_DONE          |\
                                HAL_DSI_INTERRUPT_VIDEOMODE_DONE            |\
                                HAL_DSI_INTERRUPT_BTA_DONE                  |\
                                HAL_DSI_INTERRUPT_DSI_ERROR)



/* HAL_DSI_ERROR_INT_HS_TX_TO   -->  Not Required as the timer is not big enough, hence always fires */
#define HAL_DSI_ERROR_INT_ALL ( HAL_DSI_ERROR_INT_DPHY_DSIPLL_UNLOCK        |\
                                HAL_DSI_ERROR_INT_DLN0_CONTENTION_LP1       |\
                                HAL_DSI_ERROR_INT_DLN0_CONTENTION_LP0       |\
                                HAL_DSI_ERROR_INT_DLN0_CONTROL              |\
                                HAL_DSI_ERROR_INT_DLN0_SYNC_ESC             |\
                                HAL_DSI_ERROR_INT_DLN0_ESC                  |\
                                HAL_DSI_ERROR_INT_DLN0_LP_FIFO_OVERFLOW     |\
                                HAL_DSI_ERROR_INT_DLN0_HS_FIFO_OVERFLOW     |\
                                HAL_DSI_ERROR_INT_DLN1_HS_FIFO_OVERFLOW     |\
                                HAL_DSI_ERROR_INT_DLN2_HS_FIFO_OVERFLOW     |\
                                HAL_DSI_ERROR_INT_DLN3_HS_FIFO_OVERFLOW     |\
                                HAL_DSI_ERROR_INT_VID_MDP_FIFO_UNDERFLOW    |\
                                HAL_DSI_ERROR_INT_VID_MDP_FIFO_OVERFLOW     |\
                                HAL_DSI_ERROR_INT_CMD_MDP_FIFO_UNDERFLOW    |\
                                HAL_DSI_ERROR_INT_CMD_DMA_FIFO_UNDERFLOW    |\
                                HAL_DSI_ERROR_INT_INTERLEAVE_OP_CONTENTION  |\
                                HAL_DSI_ERROR_INT_BTA_TO                    |\
                                HAL_DSI_ERROR_INT_LP_RX_TO                  |\
                                HAL_DSI_ERROR_INT_PACKET                    |\
                                HAL_DSI_ERROR_INT_RDBK_INCOMPLETE_PACKET    |\
                                HAL_DSI_ERROR_INT_RDBK_DATA_CRC             |\
                                HAL_DSI_ERROR_INT_RDBK_DATA_MULTI_ECC       |\
                                HAL_DSI_ERROR_INT_RDBK_DATA_ECC)
  
/*!
 * \b HAL_DSI_DevicePropertyType
 *
 *  Read data from panel.
 */
typedef struct
{
  bool32              bLittleEndian;    /**< is Little Endian?            */
  bool32              bNonCompliantMode;/**< Panels are non-compliant?    */
  bool32              bCRCCheckEn;      /**< Panels support CRC?          */
  bool32              bECCCheckEn;      /**< Panels support ECC?          */
  bool32              bClockLnEn;       /**< Enable DSI PHY CLOCK LANE?   */
  bool32              bDataLn0En;       /**< Enable DSI PHY DATA LANE0?   */
  bool32              bDataLn1En;       /**< Enable DSI PHY DATA LANE1?   */
  bool32              bDataLn2En;       /**< Enable DSI PHY DATA LANE2?   */
  bool32              bDataLn3En;       /**< Enable DSI PHY DATA LANE3?   */
  bool32              bCMDModeEn;       /**< Enable Command Mode Engine?  */
  bool32              bVideoModeEn;     /**< Enable Video Mode Engine?    */
  bool32              bDSICtrlEN;       /**< Enable DSI controller?       */
  bool32              bPhyDCDCMode;     /**< DSI regulator Mode TRUE=DCDC ,FALSE=LDO(Default mode) */  
}HAL_DSI_DevicePropertyType;

/*!
 * \b HAL_DSI_LanePropertyType
 *
 *  DSI Lane configuration.
 */
typedef struct
{
  DSI_LaneOperationType  eDataLaneOp;   /**< Data lane operation  */
  DSI_LaneOperationType  eClockLaneOp;  /**< Clock lane operation */ 
}HAL_DSI_LanePropertyType;


/*!
 * \b DSI_DSC_CmdModeConfigType
 *
 *  Controller config for DSC CMD Mode.
 */
typedef struct
{
  uint32                        uDscStreamId;         /**< Stream Id for DSC */
  uint32                        uDscBytesInSlice;     /**< Bytes in slice for DSC */
  uint32                        uDscPktPerLine;       /**< Packets per line for DSC */
  uint32                        uDscEolByteNum;       /**< EOL byte num for DSC */
  bool32                        uDscEnable;           /**< Enable DSC */
}DSI_DscCmdModeConfigType;

/*!
 * \b DSI_DSC_VideoModeConfigType
 *
 *  Controller config for DSC Video Mode.
 */
typedef struct
{
  uint32                        uDscStreamId;         /**< Stream Id for DSC */
  uint32                        uDscBytesPerPkt;      /**< Bytes per packet */
  uint32                        uDscPktPerLine;       /**< Packets per line for DSC */
  uint32                        uDscEolByteNum;       /**< EOL byte num for DSC */
  bool32                        uDscEnable;           /**< Enable DSC */
}DSI_DscVideoModeConfigType;

/*!
 * \b DSI_DscCompressionInfo
 *
 *  Controller config for DSC.
 */
typedef struct
{
    DSI_DscVideoModeConfigType  sDscVidCntrlrConfig; /**< Video mode dsc info */
    DSI_DscCmdModeConfigType    sDscCmdCntrlrConfig; /**< CMD mode dsc info   */
}DSI_DscCompressionInfo;

#pragma pack(1)

/*!
 * \b HAL_DSI_PHY_Dynamic_Cal_Result
 *
 * structure for storing dynamic refresh calibration result
 * HAL_DSI_PHY_Dynamic_Cal_Result and HAL_DSI_PHY_PLL_Codes_Info  
 * should be matched with kernel and should not be modified
 */
typedef struct
{
  uint32 uValid;
  uint32 uVcoRate;
  uint32 uPllCodes[3];
} HAL_DSI_PHY_Dynamic_Cal_Result;

/*!
 * \b HAL_DSI_PHY_PLL_Codes_Info
 *
 * structure for storing dynamic refresh calibration result
 *
 */
typedef struct
{
  uint32 uRefreshRateCount;
  HAL_DSI_PHY_Dynamic_Cal_Result sPllCodes[MAX_NUM_DYNAMIC_REFRESH];
}HAL_DSI_PHY_PLL_Codes_Info;

#pragma pack()


/*!
 * \b HAL_DSI_PHY_PLL_Codes_Info
 *
 * structure for storing dynamic refresh calibration result
 *
 */
typedef struct
{
  uint32   uNumOfDynamicFreq;                            /* Number of supprted Dynamic bit clock frequency */
  uint32   uDynamicRefreshRates[MAX_NUM_DYNAMIC_REFRESH];/* Dynamic Refresh Rate in Hz */
  uint32   uDynamicBitClkFreq[MAX_NUM_DYNAMIC_REFRESH];  /* Dynamic bit clock frequency*/
}DSI_Dynamic_Refresh_Info;

/*!
 * \b DSI_DisplayInfoType
 *
 * Display specific configuration
 */
typedef struct
{
  DSI_Channel_IDType            eDisplayId;           /**< Display ID.              */ 
  MDP_Display_IDType            eMDPDisplayId;        /**< MDP Display ID.          */
  DSI_Display_VCType            eDisplayVC;           /**< Virtual Channel ID.      */
  DSI_ColorFormatType           eColorFormat;         /**< Panle Color Format.      */
  uint32                        iVisWidthPx;          /**< display width (pixels).  */
  uint32                        iBlankWidthPx;        /**< display horizontal blanking   */
  uint32                        iVisHeightPx;         /**< display height (pixels)  */
  uint32                        iBlankHeightPx;       /**< display vertical blanking   */
  uint32                        uNumLanes;            /**< number of DSI lanes      */
  uint32                        uRefreshRate;         /**< refresh rate in HZ (Q16.16)      */
  uint32                        uBitClockFreq;        /**< bitclock frequency in (Hz) overrides uRefreshRate) */
  bool32                        bDisableEotp;         /**< disable appending EoTp at the end of each forward HS data burst */
  uint32                        uInitMasterTime;      /**< DSI init master time(ms) (init_master_time >= (init_slave_time + internal_delay) */  
  uint32                        uBitClkScalePercent;  /**< Percent by which DSI Bitclk has to be incremented(if desired bitclk is not provided) */
  uint32                        uEscapeDivisor;       /**< Escape clock divisor  */
  uint32                        uEscapeFreq;          /**< Escape clock frequency */
  bool32                        bCPhyMode;            /**< C-Phy */
  DSI_Dynamic_Refresh_Info      sDynamicRefreshInfo;  /**< Dynamic Refresh Information */
  HAL_DSI_PHY_PLL_Codes_Info    sPllCodesInfo;        /**< Dynamic refresh calibration result */
}DSI_DisplayInfoType;


/*!
 * \b DSI_VideoTimingType
 *
 * Config parameters for Video mode Timing
 */
typedef struct
{
  uint32    iHsyncPulseWidthDclk;   /**< Hsync pulse width in dot_clks.    */
  uint32    iHsyncBackPorchDclk;    /**< Left edge including hsync pwidth. */
  uint32    iHsyncFrontPorchDclk;   /**< Right edge of inactive LCD.       */
  uint32    iHsyncSkewDclk;         /**< Delay Hsync with respect to vsync.*/
  uint32    iVsyncPulseWidthLines;  /**< Vsync pulse width in lines.       */
  uint32    iVsyncBackPorchLine;    /**< Blanking period after HW vsync.   */
  uint32    iVsyncFrontPorchLine;   /**< Blanking period before HW vsync.  */
  uint32    iVsyncSkewDclk;         /**< Delay Vsync with respect to pclck.*/
  uint32    iVsyncStart;            /**< TODO.*/
  uint32    iVsyncEnd;              /**< TODO.*/
}DSI_VideoTimingType;


/*!
 * \b DSI_VPowerOptionType
 *
 *  Power mode options for Video mode HFP/HBP/HSA/BLLP/etc...
 */
typedef struct
{
  bool32    bSendHsaHseAfterVsVe;           /**< Send HSA(Horizontal Sync Active)&HSE(Horizontal Sync End) following VS(Vsync Active)/VE(Vsync End)? */
  bool32    bEnableLowPowerModeInHFP;       /**< LP11 in HFP (Horizontal Front Porch) period?                             */
  bool32    bEnableLowPowerModeInHBP;       /**< LP11 in HBP (Horizontal Back Porch ) period?                             */
  bool32    bEnableLowPowerModeInHSA;       /**< LP11 in HSA (Horizontal Sync Active) period?                             */
  bool32    bEnableLowPowerModeInBLLPEOF;   /**< LP11 in BLLP (Blanking or Low Power interval) period at end of frame?    */
  bool32    bEnableLowPowerModeInBLLP;      /**< LP11 in BLLP (Blanking or Low Power interval) period?                    */
}DSI_VPowerOptionType;


/*!
 * \b HAL_DSI_DevicePropertyType
 *
 *  Video Mode configuration parameters.
 */
typedef struct
{
  DSI_DisplayInfoType           sDisplayInfo;     /**< Basic Display Info       */
  DSI_TrafficModeType           eTrafficeMode;    /**< Burst Mode.              */
  DSI_VPowerOptionType          sPowerOption;     /**< Power mode.              */
  DSI_VideoTimingType           sVideoTiming;     /**<Sync, pixel timing counter*/
  DSI_DscVideoModeConfigType    sDscVideoConfig;  /**< DSC related configurations */
}HAL_DSI_VideoModeConfigType;


/*!
 * \b HAL_DSI_CmdModeConfigType
 *
 *  Command Mode configuration parameters.
 */
typedef struct
{
  DSI_DisplayInfoType           sDisplayInfo;     /**< Basic Display Info       */
  uint32                        uMaxInterleave;   /**< Max Cmd packet Interleave*/
  DSI_Datatype                  uMDPDataType;     /**< DT for MDP Packet output */  
  uint32                        uIdleTime;        /**< Idle time to be inserted between command mode MDP packets */
  DSI_DscCmdModeConfigType      sDscCmdConfig;    /**< DSC related configurations */
}HAL_DSI_CmdModeConfigType;
 
/*!
 * \b HAL_DSI_ExtTEConfigType
 *
 * Config parameters for Command Mode External TE Signals
 */
typedef struct
{
  DSI_ExtTEModeType eExtTEMode;               /**< Te Mode                              */
  bool32            bExtTEFallingEdge;        /**< Polarity of TE, 1:falling edge active*/
  uint32            iTEHsyncTotal;            /**< Total # of HSYNC per frame           */
  uint32            iTEHSyncCount;            /**< Which HSYNC should trigger a engine  */
  uint32            iTEVsyncPulseWidth_Min;   /**< Min Pulse width (in escclk) of VSYnc */
  uint32            iTEHsyncPulseWidth_Max;   /**< Max Pulse width (in escclk) of HSync */
  uint32            uExtTEMux;                /**< Input Port for external TE           */
}HAL_DSI_ExtTEConfigType;

/*!
 * \b HAL_DSI_TriggerModeType
 *
 *  Command mode Trigger modes.
 */
typedef struct
{
  bool32            bExtTEPin;      /**<TE from dedicated pin       */
  bool32            bOrderMdpDma;   /**<MDP priority higher than DMA  */
  bool32            bOrderPHYMdp;   /**<PHY priority higher than MDP  */
  bool32            bTriggerStream1;/**<Trigger to stream 1?        */
  DSI_ImageTriggerType            eImageTrigger;  /**<Display Engine(MDP) Trigger */
  DSI_CMDTriggerType              eCMDTrigger;    /**<DMA/Memory Trigger          */
}HAL_DSI_TriggerModeType;

/*!
 * \b HAL_DSI_TimeoutType
 *
 *  Time out parameters.
 */
typedef struct
{
  uint16            uBTATimeout;     /**<Bus turn around TO in escclk*/            
  uint16            uLPRxTimeout;   /**<Low Power RX TO in escclk   */
  uint16            uHSTxTimeout;   /**<High Speed TX TO in escclk  */
}HAL_DSI_TimeoutType;

/*!
 * \b HAL_DSI_PackDataType
 *
 *  Write data to panel.
 */
typedef struct
{
  DSI_Display_VCType eDisplayVC;      /**< Virtual Channel to be sent to*/
  bool32            bLastPacket;      /**< 1:last packet of this TX     */
  bool32            bBTARequest;      /**< 1:Need BTA(bLastPacket=1)    */
  uint8*            pInputData;       /**< payload to be packed         */
  uint8*            pOutputData;      /**< Packed Data                  */
  uint16            uPayloadLen;      /**< Packet Data size             */
  DSI_Datatype      uCmdDataType;     /**< Packet Data type             */
}HAL_DSI_PackDataType;

/*!
 * \b HAL_DSI_WritePacketType
 *
 *  Write data to panel in Embedded Mode.
 */
typedef struct
{
  bool32            bHighSpeedMode;   /**< Transmitting in HS mode?     */
  uint32            uPhyAddress;      /**< Physical Address of packet   */
  uint32            uPayloadLen;      /**< Packet Data size             */
  uint8            *pPackedFifoBuffer;/**< Packed command buffer address for TPG FIFO */
  uint32            uFlags;           /**< DSI Write Packet flags                     */
}HAL_DSI_WritePacketType;

/*!
 * \b HAL_DSI_ReadPacketType
 *
 *  Write data to panel in Embedded Mode.
 *
 * [IN]:        uReadPacketBufferSize
 * [IN/OUT]:    puReadPacket
 * [OUT]:       uRetPacketByte;
 * [OUT]:       uTotalPacketWC
 * [IN]:        uFlags
 * [OUT]:       uErrorCode
 */
typedef struct
{
  uint32            uReadPacketBufferSize;    /**< Expected return packet size  */
  uint8*            puReadPacket;             /**< Pointer to the returned value*/
  uint32            uRetPacketByte;           /**< Actual Read Packet size      */
  uint32            uTotalPacketWC;           /**< Word-Count in the packet     */
  uint32            uFlags;                   /**< Read flags                   */
  uint32            uErrorCode;               /**< Response packet error code   */
}HAL_DSI_ReadPacketType;

/*!
 * \b HAL_DSI_PLLConfig
 *
 *  PLL clock request from MMPM.
 */
typedef struct
{
  uint32    DsiVcoClk;
  uint32    DsiPllDsiClk;
  uint32    DsiPllBitClk;
}HAL_DSI_PLLConfig;


/*!
 * \b DSI_CMDDDTType
 *
 * Default Command mode Data Type
 */
typedef struct
{
  DSI_Datatype          uImageWriteDT;              /**< Usually   0x39 */
  DSI_Datatype          uNullPacketDT;              /**< DSI Spec: 0x09 */
  DSI_Datatype          uDCSWriteMemStartDT;        /**< DCS Spec: 0x2C */
  DSI_Datatype          uDCSWriteMemContinueDT;     /**< DCS Spec: 0x3C */
  DSI_Datatype          uRetAckWithErrorDT;         /**< DSI Spec: 0x02 */
  DSI_Datatype          uRetGenericShortRead1ParaDT;/**< DSI Spec: 0x11 */
  DSI_Datatype          uRetGenericShortRead2ParaDT;/**< DSI Spec: 0x12 */
  DSI_Datatype          uRetGenericLongReadDT;      /**< DSI Spec: 0x1A */
  DSI_Datatype          uRetDSCLongReadDT;          /**< DSI Spec: 0x1C */
  DSI_Datatype          uRetDSCShortRead1ParaDT;    /**< DSI Spec: 0x21 */
  DSI_Datatype          uReDSCShortRead2ParaDT;     /**< DSI Spec: 0x22 */
  DSI_Datatype          uEoTpDI;                    /**< DSI Spec: 0x08 */
  uint32                uEoTpWC;                    /**< should be 0xF0f*/ 
  uint32                uEoTpECC;                   /**< should be 0x01 */ 
}DSI_CMDDDTType;

/*!
 * \b DSI_VideoDTType
 *
 * Default Video mode Data Type
 */
typedef struct
{                                                           
  DSI_Datatype          uVsyncStartDT;              /**< DSI Spec: 0x01 */
  DSI_Datatype          uVsyncEndDT;                /**< DSI Spec: 0x11 */
  DSI_Datatype          uHsyncStartDT;              /**< DSI Spec: 0x21 */
  DSI_Datatype          uHsyncEndDT;                /**< DSI Spec: 0x31 */
  DSI_Datatype          uPackedPacketRGB565DT;      /**< DSI Spec: 0x0E */
  DSI_Datatype          uPackedPacketRGB666DT;      /**< DSI Spec: 0x1E */
  DSI_Datatype          uLooselyPacketRGB666DT;     /**< DSI Spec: 0x2E */
  DSI_Datatype          uPackedPacketRGB888DT;      /**< DSI Spec: 0x3E */
  DSI_Datatype          uBlankPacketDT;             /**< DSI Spec: 0x19 */
  uint32                uBlankPacketData;           /**< Default:  0x00 */
}DSI_VideoDTType;


/*!
 * \b DSI_TimingOverrideType
 *
 *  Timing override for DSI timing parameters
 */
typedef struct
{
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
} DSI_TimingOverrideType;


/*!
 * \Struct HAL_DSI_VersionType
 *
 * DSI hardware version
 * This is DSI HW Version register standardized according to IP Catalog structure.
 * it provides DSI Controller major version, minor version and release phase.
 */
typedef struct _HAL_DSI_VersionType
{
  uint32      uMajorVersion;
  uint32      uMinorVersion;
  uint32      uReleaseVersion;
} HAL_DSI_VersionType;


/*------------------------------------------------------------------------
Function declarations
-------------------------------------------------------------------------- */

/*------------------------------------------------------------------------
 * The 4 required HAL functions - Located in HALdsi.c                      
 *------------------------------------------------------------------------ */


/****************************************************************************
*
** FUNCTION: HAL_DSI_GetRegBaseOffset()
*/
/*!
* \brief
*     Get the offset of DSI register base address for a give DSI device ID.
*     Can be useful when accessing registers in all sub-groups under DSI, such 
*     as Controller, Phy, PLL, Regulator, etc.
*
* \param [in]  eDeviceId - DSI core device ID.
*
* \retval - register base offset
*
****************************************************************************/
uintPtr HAL_DSI_GetRegBaseOffset( DSI_Device_IDType  eDeviceId );


/****************************************************************************
*
** FUNCTION: HAL_DSI_Init()
*/
/*!
* \brief
*   Initialize all DSI hardware blocks after a reset (hardware or software)
*   or at system power up. This function is called once prior to all other 
*   MDP HAL functions with the exception of HAL_DSI_Reset.  
*   Identical to calling HAL_DSI_InitDevice(HAL_DSI_ DEVICEID _ALL). 
*
* \param [in]  eDeviceId - DSI core ID
* 
* \retval void
*
****************************************************************************/
void HAL_DSI_Init(DSI_Device_IDType        eDeviceID);


/****************************************************************************
*
** FUNCTION: HAL_DSI_Reset()
*/
/*!
* \brief
*   Use the DSI hardware reset mechanism to reset all DSI hardware blocks 
*   with any available hardware reset mechanism present.  If a global reset 
*   is not available then the individual available blocks will be reset.  
*   The power and clock sources for the DSI hardware are not interrupted.
*
* \param [in]  eDeviceID  - DSI core device ID.
*
* \retval void
*
****************************************************************************/
void HAL_DSI_Reset(DSI_Device_IDType        eDeviceID);


/****************************************************************************
*
** FUNCTION: HAL_DSI_Save()
*/
/*!
* \brief
*   Function has no effect and is reserved for future use.
*
* \retval void
*
****************************************************************************/
void HAL_DSI_Save(void);


/****************************************************************************
*
** FUNCTION: HAL_DSI_Restore()
*/
/*!
* \brief
*   Function has no effect and is reserved for future use.
*
* \retval void
*
****************************************************************************/
void HAL_DSI_Restore(void);


/*------------------------------------------------------------------------
 * General HAL functionality - Located in HALdsi.c                    
 *------------------------------------------------------------------------ */
/****************************************************************************
*
** FUNCTION: HAL_DSI_GetLaneStatus()
*/
/*!
* \brief
*    Query the lane status information for a particular DSI device.
*
* \param [in]  eDeviceID    - DSI core device ID.
* \param [out] puDsiStatus - Device lane status information is returned
*
* \retval void
*
****************************************************************************/
void HAL_DSI_GetLaneStatus(DSI_Device_IDType   eDeviceID,
                           uint32             *puDsiStatus);


/****************************************************************************
*
** FUNCTION: HAL_DSI_GetStatus()
*/
/*!
* \brief
*   Query if DSI hardware blocks are in use.  This function only determines if
*   the hardware block is active, periodic hardware will only appear in use when
*   doing work.  This function can be used to poll for completion of hardware.
*   Bit will be 1 if the corresponding HW block are busy.
*
* \param [in]  eDeviceID    - DSI core device ID.
* \param [out] puDsiStatus  - Bitwise fields defined in the HAL_DSI_StatusType.
*
* \retval void
*
****************************************************************************/
void HAL_DSI_GetStatus(DSI_Device_IDType   eDeviceID,
                       uint32             *puDsiStatus);



/*------------------------------------------------------------------------
 * HAL DSI interrupt functionality - Located in HALdsi_Interrupts.c                    
 *------------------------------------------------------------------------ */
/****************************************************************************
*
** FUNCTION: HAL_DSI_SetInterruptsEnabled()
*/
/*!
* \brief
*   Enable the hardware interrupts specified in the iInterruptSource field.
*   Each bit field is parsed and mapped to the equivalent hardware field.  
*   The value in uInterruptSource is NOT directly used to program the hardware 
*   register
*
* \param [in] eDeviceID        - DSI Core Device ID 
* \param [in] uInterruptSource - Bitwise specified fields defined in the 
*                                HAL_DSI_InterruptSourceType.
*
* \retval NONE
*
****************************************************************************/
void HAL_DSI_SetInterruptsEnabled(
    DSI_Device_IDType            eDeviceID,
    uint32                      uInterruptSource
    );

/****************************************************************************
*
** FUNCTION: HAL_DSI_GetInterruptsEnabled()
*/
/*!
* \brief
*   Get the hardware interrupts enabled.  Each bit field is parsed and mapped
*   to the equivalent hardware field.  The value in puInterruptSource is NOT
*   directly used to program the hardware register
*
* \param [in] eDeviceID         - DSI Core Device ID 
* \param [out]puInterruptSource - Pointer to bitwise specified fields defined
                    in the HAL_DSI_InterruptSourceType returned by the driver.
*
* \retval NONE,Bitwise specified fields defined in HAL_DSI_InterruptSourceType
*         returned in piInterruptSource.
*
****************************************************************************/
void HAL_DSI_GetInterruptsEnabled(
    DSI_Device_IDType            eDeviceID,
    uint32*                 puInterruptSource
    );

/****************************************************************************
*
** FUNCTION: HAL_DSI_GetInterruptStatus()
*/
/*!
* \brief
*   Get the hardware interrupt status.  Each bit field is parsed and mapped to
*   the equivalent hardware field.  The value in puInterruptSource is NOT 
*   directly used to program the hardware status register.
*
* \param [in] eDeviceID         - DSI Core Device ID 
* \param [out]puInterruptSource - Pointer to bitwise specified fields defined
                    in the HAL_DSI_InterruptSourceType returned by the driver.
  \param [out]puErrorCode          - RESERVED FOR FUTURE USE.Field has no effect
*
* \retval NONE,Bitwise specified fields defined in HAL_DSI_InterruptSourceType
*         returned in piInterruptSource.
*
****************************************************************************/
void HAL_DSI_GetInterruptStatus(
    DSI_Device_IDType            eDeviceID,
    uint32*                 puInterruptSource,
    uint32*                 puErrorCode
    );

/****************************************************************************
*
** FUNCTION: HAL_DSI_ClrInterruptStatus()
*/
/*!
* \brief
*   Clear the hardware interrupt status register. Each bit field is parsed 
*   and mapped to the equivalent hardware field. The value in uInterruptSource
*   is NOT directly used to program the hardware status register.
*
* \param [in] eDeviceID        - DSI Core Device ID 
* \param [in] uInterruptSource - bitwise specified fields defined in the
                            HAL_DSI_InterruptSourceType passed by the caller.
*
* \retval NONE
*
****************************************************************************/
void HAL_DSI_ClrInterruptStatus(
    DSI_Device_IDType            eDeviceID,
    uint32                      uInterruptSource
    );
/****************************************************************************
*
** FUNCTION: HAL_DSI_SetErrorMaskEnabled()
*/
/*!
* \brief
*   Enable the DSI Error Mask specified in the uErrorSource field.
*   Each bit field is parsed and mapped to the equivalent hardware field.  
*   The value in uErrorSource is NOT directly used to program the hardware 
*   register
*
* \param [in] eDeviceID        - DSI Core Device ID 
* \param [in] uErrorSource     - Bitwise specified fields defined in the 
*                                HAL_DSI_ErrorSourceType.
*
* \retval NONE
*
****************************************************************************/
void HAL_DSI_SetErrorMaskEnabled(
    DSI_Device_IDType            eDeviceID,
    uint32                      uErrorSource
    );

/****************************************************************************
*
** FUNCTION: HAL_DSI_GetErrorMaskEnabled()
*/
/*!
* \brief
*   Get the DSI Error Mask enabled.  Each bit field is parsed and mapped
*   to the equivalent hardware field.  The value in puErrorSource is NOT
*   directly read from the hardware register
*
* \param [in] eDeviceID         - DSI Core Device ID 
* \param [out]puErrorSource     - Pointer to bitwise specified fields defined
                            in the HAL_DSI_ErrorSourceType returned by the driver.
*
* \retval NONE,Bitwise specified fields defined in HAL_DSI_ErrorSourceType returned 
*         in puErrorSource.
*
****************************************************************************/
void HAL_DSI_GetErrorMaskEnabled(
    DSI_Device_IDType            eDeviceID,
    uint32*                 puErrorSource
    );

/****************************************************************************
*
** FUNCTION: HAL_DSI_GetHostErrorStatus()
*/
/*!
* \brief
*   Get the DSI Host Error status.  Each bit field is parsed and mapped to the
*   equivalent hardware field.  The value in puErrorCode is NOT directly read 
*   from the hardware status register.
*
* \param [in] eDeviceID         - DSI Core Device ID 
* \param [out]puErrorStatus     - Pointer to bitwise specified fields defined
                        in the HAL_DSI_ErrorSourceType returned by the driver.
  \param [out]puErrorCode          - RESERVED FOR FUTURE USE.Field has no effect
*
* \retval NONE,Bitwise specified fields defined in HAL_DSI_ErrorSourceType
*         returned in piInterruptSource.
*
****************************************************************************/
void HAL_DSI_GetHostErrorStatus(
    DSI_Device_IDType            eDeviceID,
    uint32*                 puErrorStatus,
    uint32*                 puErrorCode
    );

/****************************************************************************
*
** FUNCTION: HAL_DSI_ClrHostErrorStatus()
*/
/*!
* \brief
*   Clear the DSI Host Error status register. Each bit field is parsed 
*   and mapped to the equivalent hardware field. The value in uErrorStatus
*   is NOT directly used to program the hardware status register.
*
* \param [in] eDeviceID        - DSI Core Device ID 
* \param [in] uErrorStatus     - bitwise specified fields defined in the
                              HAL_DSI_ErrorSourceType passed by the caller.
*
* \retval NONE
*
****************************************************************************/
void HAL_DSI_ClrHostErrorStatus(
    DSI_Device_IDType            eDeviceID,
    uint32                      uErrorStatus
    );

/****************************************************************************
*
** FUNCTION: HAL_DSI_GetPanelErrorStatus()
*/
/*!
* \brief
*   This API query the ACK Error Status of DSI Controller from Panels.
*
* \param [in] eDeviceID      - DSI Core Device ID 
* \param [in] puErrorStatus  - Bitwise specified fields defined in the 
*                              HAL_DSI_AckErrorSourceType.
*
* \retval NONE,Bitwise specified fields defined in HAL_DSI_AckErrorSourceType 
*         returned in puErrorStatus.
*
****************************************************************************/
void HAL_DSI_GetPanelErrorStatus(
    DSI_Device_IDType              eDeviceID,
    uint32*                            puErrorStatus
    );

/****************************************************************************
*
** FUNCTION: HAL_DSI_ClrPanelErrorStatus()
*/
/*!
* \brief
*   Clear the ACK Error Status register. Each bit field is parsed and mapped 
*   to the equivalent hardware field. The value in uErrorStatus is NOT directly 
*   used to program the hardware status register.
*
*
* \param [in] eDeviceID        - DSI Core Device ID 
* \param [in] uErrorStatus     - bitwise specified fields defined in the
                              HAL_DSI_AckErrorSourceType passed by the caller.
*
* \retval NONE,Bitwise specified fields defined in HAL_DSI_AckErrorSourceType 
*         returned in puErrorStatus.
*
****************************************************************************/
void HAL_DSI_ClrPanelErrorStatus(
    DSI_Device_IDType              eDeviceID,
    uint32                        uErrorStatus
    );

/*------------------------------------------------------------------------
 * General HAL functionality - Located in HALdsi.c                    
 *------------------------------------------------------------------------ */

/****************************************************************************
*
** FUNCTION: HAL_DSI_DeviceConfig()
*/
/*!
* \brief
*   This API configure the DSI Core which is shared by video and command mode
*   engineer. DSI host will keep a record of the activity of DSI Display , and
*   turn on/off  the different block accordingly.
*
* \param [in] eDeviceID        - DSI Core Device ID 
* \param [in] psDeviceProperty - pointer to the device control parameters.
*
* \retval NONE
*
****************************************************************************/
void HAL_DSI_DeviceConfig(
    DSI_Device_IDType                      eDeviceID,
    HAL_DSI_DevicePropertyType*        psDeviceProperty
    );

/****************************************************************************
*
** FUNCTION: HAL_DSI_CPhyModeEnable()
*/
/*!
* \brief
*   This API enable/disable DSI CPhy 
* 
* \param [in] eDeviceID        - DSI Core Device ID 
* \param [in] enable           - CPhy enable/disable
*
* \retval NONE
*
****************************************************************************/
void HAL_DSI_CPhyModeEnable(
    DSI_Device_IDType          eDeviceID,
    bool32 bEnable
    );

/****************************************************************************
HAL_DSI_SetScratchRegister1()

Description:
    This API set dsi scratch reigster 1

****************************************************************************/
void HAL_DSI_SetScratchRegister1(DSI_Device_IDType           eDeviceID,
                                 uint32                      uData);

/****************************************************************************
*
** FUNCTION: HAL_DSI_LaneConfig()
*/
/*!
* \brief
*   This API configures the DSI Lanes
* 
* 
*
* \param [in] eDeviceID        - DSI Core Device ID 
* \param [in] psLaneProperty   - DSI clock/data lane configuration
*
* \retval NONE
*
****************************************************************************/
void HAL_DSI_LaneConfig(DSI_Device_IDType           eDeviceID,
                        HAL_DSI_LanePropertyType   *psLaneProperty);


/****************************************************************************
*
** FUNCTION: HAL_DSI_VideoModeConfig()
*/
/*!
* \brief
*   This API configures the DSI video mode engine.
*
* \param [in] eDeviceID      - DSI Core Device ID 
* \param [in] psVideoConfig  - pointer to the video mode engine configuration .
*
* \retval NONE
*
****************************************************************************/
void HAL_DSI_VideoModeConfig(
    DSI_Device_IDType                        eDeviceID,
    HAL_DSI_VideoModeConfigType*             psVideoConfig        
    );

/****************************************************************************
*
** FUNCTION: HAL_DSI_CMDModeConfig()
*/
/*!
* \brief
*   This API configures the DSI command mode engine.
*
* \param [in] eDeviceID      - DSI Core Device ID 
* \param [in] psCMDProperty  - pointer to the video mode engine configuration.
*
* \retval NONE
*
****************************************************************************/
void HAL_DSI_CMDModeConfig(
    DSI_Device_IDType                      eDeviceID,
    HAL_DSI_CmdModeConfigType*             psCMDConfig
    );

/****************************************************************************
*
** FUNCTION: HAL_DSI_VideoDscConfig()
*/
/*!
* \brief
*   This API configures the DSI command mode engine.
*
* \param [in] eDeviceID         - DSI Core Device ID 
* \param [in] psVideoDscConfig  - pointer to the dsc video mode configuration.
*
* \retval NONE
*
****************************************************************************/
void HAL_DSI_VideoDscConfig(
    DSI_Device_IDType                      eDeviceID,
    DSI_DscVideoModeConfigType*            psVideoDscConfig
    );

/****************************************************************************
*
** FUNCTION: HAL_DSI_CMDDscConfig()
*/
/*!
* \brief
*   This API configures the DSI command mode engine.
*
* \param [in] eDeviceID       - DSI Core Device ID 
* \param [in] psCMDDscConfig  - pointer to the dsc cmd mode configuration.
*
* \retval NONE
*
****************************************************************************/
void HAL_DSI_CMDDscConfig(
    DSI_Device_IDType                      eDeviceID,
    DSI_DscCmdModeConfigType*    psCMDDscConfig
    );

/*------------------------------------------------------------------------
 * General HAL functionality - Located in HALdsi.c                    
 *------------------------------------------------------------------------ */
/****************************************************************************
*
** FUNCTION: HAL_DSI_SetExtTEPin()
*/
/*!
* \brief
*   This API configures the DSI Controller according to External TE Pin .
*
* \param [in] eDeviceID   - DSI Core Device ID 
* \param [in] psExtTEfig  - pointer to the External TE Pin signal configs.
*
* \retval NONE
*
****************************************************************************/
void HAL_DSI_ExtTEPinConfig(
    DSI_Device_IDType                      eDeviceID,
    HAL_DSI_ExtTEConfigType*            psExtTEfig
    );

/****************************************************************************
*
** FUNCTION: HAL_DSI_SetDMATriggerConfig()
*/
/*!
* \brief
*   This API configures the command mode DMA trigger mode.
*
* \param [in] eDeviceID             - DSI Core Device ID 
* \param [in] eDMATriggerType  - DMA trigger type.
*
* \retval NONE
*
****************************************************************************/
void HAL_DSI_SetDMATriggerConfig( 
     DSI_Device_IDType      eDeviceID,
     DSI_CMDTriggerType     eDMATriggerType );


/****************************************************************************
*
** FUNCTION: HAL_DSI_GetDMATriggerConfig()
*/
/*!
* \brief
*   This API configures the command mode DMA trigger mode.
*
* \param [in] eDeviceID            - DSI Core Device ID 
* \param [in] pDMATriggerType  - pointer to the dma trigger type.
*
* \retval NONE
*
****************************************************************************/
void HAL_DSI_GetDMATriggerConfig( 
     DSI_Device_IDType      eDeviceID,
     DSI_CMDTriggerType*    pDMATriggerType );


/****************************************************************************
*
** FUNCTION: HAL_DSI_TriggerConfig()
*/
/*!
* \brief
*   This API configures the command mode DMA and MDP trigger mode.
*
* \param [in] eDeviceID      - DSI Core Device ID 
* \param [in] psTriggerMode  - pointer to the trigger control property.
*
* \retval NONE
*
****************************************************************************/
void HAL_DSI_TriggerConfig(
    DSI_Device_IDType                    eDeviceID,
    HAL_DSI_TriggerModeType*          psTriggerMode
    );
/****************************************************************************
*
** FUNCTION: HAL_DSI_CMDDatatypeConfig()
*/
/*!
* \brief
*   This API configures the DSI command mode data type.
*
* \param [in] eDeviceID      - DSI Core Device ID 
* \param [in] psCmdDataType  - pointer to Command Mode data type.
*
* \retval NONE
*
****************************************************************************/
void HAL_DSI_CMDDatatypeConfig(
    DSI_Device_IDType              eDeviceID,
    DSI_CMDDDTType*                psCmdDataType        
    );
/****************************************************************************
*
** FUNCTION: HAL_DSI_VideoDatatypeConfig()
*/
/*!
* \brief
*   This API configures the DSI Video Mode engine.
*
* \param [in] eDeviceID             - DSI Core Device ID 
* \param [in] psVideoDataType   - pointer to Video Mode data type.
*
* \retval NONE
*
****************************************************************************/
void HAL_DSI_VideoDatatypeConfig(
    DSI_Device_IDType              eDeviceID,
    DSI_VideoDTType*              psVideoDataType        
    );

/****************************************************************************
*
** FUNCTION:HAL_DSI_WriteData()
*/
/*!
* \brief
*   This API write DMA out data to registers in embedded mode, waiting for 
*   trigger.
*
* \param [in] eDeviceID      - DSI Core Device ID 
* \param [in] psWritePacket  - pointer to data to be write out.
*
* \retval NONE,
*
****************************************************************************/
void HAL_DSI_WriteData(
    DSI_Device_IDType              eDeviceID,
    HAL_DSI_WritePacketType*  psWritePacket
    );

/****************************************************************************
*
** FUNCTION: HAL_DSI_ReadData()
*/
/*!
* \brief
*   This API allows users to send the DSI read command out to DSI Panels, 
*   and wait until it gets the data from panel.
*
* \param [in] eDeviceID       - DSI Core Device ID 
* \param [in] psReadPacket    - Contains the data to be read out from panels.
*
* \retval error status 
*
****************************************************************************/
HAL_dsi_StatusType HAL_DSI_ReadData(
    DSI_Device_IDType            eDeviceID,
    HAL_DSI_ReadPacketType*      psReadPacket
    );

/****************************************************************************
*
** FUNCTION: HAL_DSI_Trigger()
*/
/*!
* \brief
*   This API allows users to trigger the command DMA out by Software.
*
* \param [in] eDeviceID      - DSI Core Device ID 
* \param [in] eFlag          - Flag for Commit function.
*
* \retval NONE
*
****************************************************************************/
void HAL_DSI_Trigger(
    DSI_Device_IDType            eDeviceID,
    HAL_DSI_TriggerFlagType eFlag
    );

/****************************************************************************
*
** FUNCTION: HAL_DSI_ClockConfig()
*/
/*!
* \brief
*   Turn on/off the DSI internal clock according to the bitwise field.
*
* \param [in] eDeviceID     - DSI Core Device ID 
* \param [in] uClockConfig  - bitwise specified fields defined in the
                            HAL_DSI_ClockIDType passed by the caller.
*
* \retval NONE
*
****************************************************************************/
void HAL_DSI_ClockConfig(
    DSI_Device_IDType            eDeviceID, 
    uint32                  uClockConfig
    );

/****************************************************************************
*
** FUNCTION: HAL_DSI_DataLaneConfig()
*/
/*!
* \brief
*   Phy Lane mapping for data lane 0, 1, 2, 3 output from DSI controller.
*
* \param [in] eDeviceID     - DSI Core Device ID 
* \param [in] eDisplayLane  - Data Lane mapping with PHY Lane.
*
* \retval NONE
*
****************************************************************************/
void HAL_DSI_LaneMapConfig(
    DSI_Device_IDType            eDeviceID,
    DSI_LaneMapType*        paDisplayLane
    );
 

/****************************************************************************
*
** FUNCTION: HAL_DSI_PhyConfig()
*/
/*!
* \brief
*   Config DSI PHY.
*
* \param [in] eDeviceID     - DSI Core Device ID 
* \param [in] psPhyConfig   - Pointer to Phy Config structure           
*
* \retval NONE
*
****************************************************************************/
void HAL_DSI_PhyConfig( 
    DSI_Device_IDType            eDeviceID,
    HAL_DSI_PLLConfig*      DsiConfig 
    );
/*------------------------------------------------------------------------
 * Internal HAL functionality - Located in HALdsi.c                    
 *------------------------------------------------------------------------ */
/****************************************************************************
*
** FUNCTION: HAL_DSI_TimeoutConfig()
*/
/*!
* \brief
*   Config DSI Timeout setting in case of loss of packet in transmission.
*
* \param [in] eDeviceID       - DSI Core Device ID 
* \param [in] psTimeoutConfig - Timeout Setting in escclk          
*
* \retval NONE
*
****************************************************************************/
void HAL_DSI_TimeoutConfig(
    DSI_Device_IDType            eDeviceID,
    HAL_DSI_TimeoutType*    psTimeoutConfig       
    );

/****************************************************************************

HAL_DSI_PHYHWCalib()

Description:
    Calibration Phy using HW.

****************************************************************************/
void HAL_DSI_PHYHWCalib       ( void );


/****************************************************************************

HAL_DSI_Enable_CTRL()

Description:
    Enables the HW blocks in each of DSI controller
 

****************************************************************************/
void HAL_DSI_Enable_CTRL(DSI_Device_IDType    eDeviceID,
                         HAL_DSI_HWBlockType  eHWBlock );


/****************************************************************************

HAL_DSI_Disable_CTRL()

Description:
    Disables the HW blocks in each of DSI controller
 

****************************************************************************/
void HAL_DSI_Disable_CTRL(DSI_Device_IDType    eDeviceID,
                          HAL_DSI_HWBlockType  eHWBlock );


/****************************************************************************

HAL_DSI_Init_LP11()

Description:
    Init the controller to LP11 
 

****************************************************************************/
void HAL_DSI_Init_LP11(DSI_Device_IDType    eDeviceID);

/****************************************************************************

HAL_DSI_IsEngineBusy()

Description:
    check whether or not DSI engine is busy.
 

****************************************************************************/
bool32 HAL_DSI_IsEngineBusy(DSI_Device_IDType eDeviceID);


/****************************************************************************

HAL_DSI_IsEngineBtaBusy()

Description:
    check whether or not DSI engine is busy for BTA.
 

****************************************************************************/
bool32 HAL_DSI_IsEngineBtaBusy(DSI_Device_IDType eDeviceID);


/****************************************************************************

HAL_DSI_PhyPllInitalize()

Description:
    This function initializes the entire DSI core (Phy/PLL) to a default state.
    This function should be applied only once after the core has come out of reset.

****************************************************************************/
void HAL_DSI_PhyPllInitialize(void);

/****************************************************************************

HAL_DSI_ReadCoreVersionInfo()

Description:
   Returns the DSI controller version(Major,Minor,Step) information 

****************************************************************************/
void HAL_DSI_ReadCoreVersionInfo(HAL_DSI_VersionType *psDSIVersionInfo);

#ifdef __cplusplus
}
#endif

#endif  /* #define HALDSI_H */
