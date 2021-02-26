#ifndef HALSB_H
#define HALSB_H
/**
============================================================================

@file:   HALsb.h

@brief:  This module provides the Application Programing Interface (API) 
         to the Slimbus HAL layer.
         These APIs should be accessed only by the driver layer.
============================================================================
*/
/*
============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.qdsp6/2.1.c4/buses/slimbus/src/common/halsb.h#1 $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2016-10-03  MJS  Support for full secondary line bandwidth scheduling.
  2016-04-14  MJS  Fix for retention support.
  2016-03-24  MJS  Retention support.
  2016-02-05  MJS  Add support for multiple endpoints per port.
  2015-04-28  NSD  Add support for no data line activity interrupt on 
                   interface device.
  2015-04-22  NSD  Add APIs to check for hardware superframe sync shift support
                   and also to enable/disable the same.
  2015-02-05  MJS  Support for 64-bit.
  2014-04-30  MJS  Add interface frame sync APIs.
  2014-02-28  MJS  Add PGD port get events API.
  2013-07-31  MJS  Support for shadow counter, upper watermark, port DMA
                   interrupt and hardware optional capabilities.
  2013-04-30  MJS  Add get assigned LA APIs.
  2013-03-04  MJS  Add platform environment configuration.
  2011-12-11  MJS  Renamed include file.
  2010-05-12  ps   Initial version.

============================================================================
Copyright (c) 2010, 2011, 2013, 2014, 2015, 2016 QUALCOMM Technologies Incorporated. All Rights Reserved.
Qualcomm Confidential and Proprietary
*/

/* ----------------------------------------------------------------------- 
** Includes
** ----------------------------------------------------------------------- */ 

#include "SlimBusTypes.h"

/* ----------------------------------------------------------------------- 
** Constants 
** ----------------------------------------------------------------------- */ 

/** HAL SB version strings */
#define HAL_SB_VERSION_STRING      "HAL_SB_VERSION_1.0"

/** HAL SB Invalid handle */
#define HAL_SB_HANDLE_INVALID         ((HAL_sb_HandleType)0)

#define ISTARI_HW_VERSION          0x01050000
#define NAZGUL_HW_VERSION          0x02000000

/* ----------------------------------------------------------------------- 
** Types 
** ----------------------------------------------------------------------- */ 

/** The SB Handle holds the virtual base address of the SB */
typedef size_t HAL_sb_HandleType;

/** Port Data Alignment Type */
typedef enum 
{
  HAL_SB_PGD_PORT_DATA_ALIGN_LSB = 0,
  HAL_SB_PGD_PORT_DATA_ALIGN_MSB = 1,
  HAL_ENUM_32BITS(SB_PGD_PORT_DATA_ALIGN)
}HAL_sb_PgdPortDataAlignType;


/** Device Type */
typedef enum 
{
  HAL_SB_DEVICE_MANAGER,
  HAL_SB_DEVICE_INTERFACE,
  HAL_SB_DEVICE_FRAMER,
  HAL_SB_DEVICE_PORTED_GENERIC,
  HAL_SB_DEVICE_NON_PORTED_GENERIC_1,
  HAL_SB_DEVICE_NON_PORTED_GENERIC_2,

  HAL_ENUM_32BITS(SB_DEVICE)
}HAL_sb_DeviceType;


/** NGD device types */
typedef enum 
{
  HAL_SB_NON_PORTED_GENERIC_1,
  HAL_SB_NON_PORTED_GENERIC_2,

  HAL_ENUM_32BITS(SB_NGD_DEVICE)
}HAL_sb_NgdDeviceType;


/** Device MsgQ type */
typedef enum 
{
  HAL_SB_DEVICE_MSG_QUEUE_RX,
  HAL_SB_DEVICE_MSG_QUEUE_TX,
  HAL_SB_DEVICE_MSG_QUEUE_TX_PRIORITY_LOW = HAL_SB_DEVICE_MSG_QUEUE_TX,
  HAL_SB_DEVICE_MSG_QUEUE_TX_PRIORITY_HIGH,

  HAL_ENUM_32BITS(SB_DEVICE_MSG_QUEUE)
}HAL_sb_DeviceMsgQType;


/** Interface device Information Element type   */
typedef struct
{
  uint32 UNSPRTD_MSG:1;
  uint32 DATA_TX_COL:1;
  uint32 RECONFIG_OBJECTION:1;
  uint32 EX_ERROR:1;
  uint32 MC_TX_COL:1;
  uint32 LOST_FS:1;
  uint32 LOST_SFS:1;
  uint32 LOST_MS:1;
  uint32 DS_OVERLAP:1;
  uint32 RESERVED:7;
  uint32 DEVICE_CLASS_VERSION:8;
  uint32 DEVICE_CLASS:8;
}HAL_sb_IntfIeType;


/** Manager device Information Element type   */
typedef struct
{
  uint32 UNSPRTD_MSG:1;
  uint32 RESERVED1:1;
  uint32 RECONFIG_OBJECTION:1;
  uint32 EX_ERROR:1;
  uint32 RESERVED:12;
  uint32 DEVICE_CLASS_VERSION:8;
  uint32 DEVICE_CLASS:8;
}HAL_sb_MgrIeType;


/** Framer device Information Element type   */
typedef struct
{
  uint32 UNSPRTD_MSG:1;
  uint32 RESERVED1:1;
  uint32 RECONFIG_OBJECTION:1;
  uint32 EX_ERROR:1;
  uint32 ACTIVE_FRAMER:1;
  uint32 FS_TX_COL:1;
  uint32 FI_TX_COL:1;
  uint32 GC_TX_COL:1;
  uint32 CLK_QUAL:2;
  uint32 RESERVED2:6;
  uint32 DEVICE_CLASS_VERSION:8;
  uint32 DEVICE_CLASS:8;
}HAL_sb_FrmIeType;


/** Generic device(Ported and Non Ported) Information Element
    type */
typedef struct
{
  uint32 UNSPRTD_MSG:1;
  uint32 DATA_TX_COL:1;
  uint32 RECONFIG_OBJECTION:1;
  uint32 EX_ERROR:1;
  uint32 RESERVED1:12;
  uint32 DEVICE_CLASS_VERSION:8;
  uint32 DEVICE_CLASS:8;
}HAL_sb_GdIeType;


/** Transport Protocol type   */
typedef enum
{
  HAL_SB_TP_ISOCHRONOUS = 0x0,
  HAL_SB_TP_PUSHED = 0x1,
  HAL_SB_TP_PULLED = 0x2,
  HAL_SB_TP_LOCKED = 0x3,
  HAL_SB_TP_ASYNC_SIMPLEX = 0x4,
  HAL_SB_TP_ASYNC_HALF_DUPLEX=0x5,
  HAL_SB_TP_EX_ASYNC_SIMPLEX=0x6,
  HAL_SB_TP_EX_ASYNC_HALF_DUPLEX=0x7,
  HAL_SB_TP_USER_DEFINED_1=0xE,
  HAL_SB_TP_USER_DEFINED_2=0xF,
  HAL_ENUM_32BITS(SB_TP)
}HAL_sb_TransportProtocolType;


/** Manager device Interrupt type */
typedef enum
{
  HAL_SB_MGR_INT_RECONFIG_DONE = 0x1000000,
  HAL_SB_MGR_INT_TX_MSG_NACKED_TWICE = 0x2000000,
  HAL_SB_MGR_INT_TX_MSG_BUF_CONTENTION = 0x4000000,
  HAL_SB_MGR_INT_INVALID_TX_MSG = 0x8000000,
  HAL_SB_MGR_INT_IE_VE_CHANGE = 0x10000000,
  HAL_SB_MGR_INT_DEV_ERR = 0x20000000,
  HAL_SB_MGR_INT_RX_MSG_RCVD = 0x40000000,
  HAL_SB_MGR_INT_TX_MSG_SENT = (int32)0x80000000,   /* Type cast to int to avoid 
                                                     compiler warning */
  HAL_ENUM_32BITS(SB_MGR_INT)
}HAL_sb_MgrIntType;


/** Framer device Interrupt type */
typedef enum
{
  HAL_SB_FRM_INT_DEV_ERR = 0x1,
  HAL_SB_FRM_INT_IE_VE_CHANGE = 0x2,
  HAL_SB_FRM_INT_SLIMBUS_BOOT_COMPLETE = 0x4,
  HAL_SB_FRM_INT_IDLE_MODE_EXITED = 0x8,
  HAL_ENUM_32BITS(SB_FRM_INT)
}HAL_sb_FrmIntType;


/** Interface device Interrupt type */
typedef enum
{
  HAL_SB_INTF_INT_DEV_ERR = 0x1,
  HAL_SB_INTF_INT_IE_VE_CHANGE = 0x2,
  HAL_SB_INTF_INT_NO_DATA_LINE_ACTIVITY = 0x4,
  HAL_ENUM_32BITS(SB_INTF_INT)
}HAL_sb_IntfIntType;


/** NGD device Interrupt type */
typedef enum
{
  HAL_SB_NGD_INT_RECONFIG_DONE = 0x1000000,
  HAL_SB_NGD_INT_TX_MSG_NACKED_TWICE = 0x2000000,
  HAL_SB_NGD_INT_TX_MSG_BUF_CONTENTION = 0x4000000,
  HAL_SB_NGD_INT_INVALID_TX_MSG = 0x8000000,
  HAL_SB_NGD_INT_IE_VE_CHANGE = 0x10000000,
  HAL_SB_NGD_INT_DEV_ERR = 0x20000000,
  HAL_SB_NGD_INT_RX_MSG_RCVD = 0x40000000,
  HAL_SB_NGD_INT_TX_MSG_SENT = (int32)0x80000000, /* Type cast to int to avoid 
                                                     compiler warning */
  HAL_ENUM_32BITS(SB_NGD_INT)
}HAL_sb_NgdIntType;


/** PGD device Interrupt type */
typedef enum
{
  HAL_SB_PGD_INT_DEV_ERR = 0x1,
  HAL_SB_PGD_INT_IE_VE_CHANGE = 0x2,
  HAL_ENUM_32BITS(SB_PGD_INT)
}HAL_sb_PgdIntType;

/** Test Bus Type */
typedef enum 
{
  HAL_SB_INTERFACE_TESTBUS = 0x0,
  HAL_SB_FRAMER_TESTBUS = 0x1,
  HAL_SB_MANAGER_TESTBUS = 0x2,
  HAL_SB_PORTED_GENERIC_TESTBUS = 0x3,
  HAL_SB_NON_PORTED_GENERIC_1_TESTBUS = 0x4,
  HAL_SB_NON_PORTED_GENERIC_2_TESTBUS = 0x5,
  HAL_SB_MP_TESTBUS = 0x6,
  HAL_SB_FRAME_LAYER_TESTBUS = 0x7,
  HAL_ENUM_32BITS(SB_DEVICE_TESTBUS)
}HAL_sb_TestBusType;

/* PGD port events */
typedef enum
{
  HAL_SB_PGD_PORT_NO_EVENT = 0x0,
  HAL_SB_PGD_PORT_OVERFLOW_EVENT = 0x1,
  HAL_SB_PGD_PORT_UNDERFLOW_EVENT = 0x2,
  HAL_SB_PGD_PORT_DISCONNECT_EVENT = 0x4,
  HAL_SB_PGD_PORT_REMOVE_CHANNEL_EVENT = 0x8,
  HAL_SB_PGD_PORT_DMA_EVENT = 0x10,
  HAL_ENUM_32BITS(SB_DEVICE_PGD_PORT_EVENT)
}HAL_sb_PgdPortEventType;

/* Clock pause function pointer type */
typedef void (*HAL_sb_ClkPauseFcnType)(uint32 uPauseTimeUs);

/* Environment data structure type */
typedef struct HAL_sb_EnvType
{
  /* Clock pause function pointer */
  HAL_sb_ClkPauseFcnType pfnClkPauseUs;
} HAL_sb_EnvType;

/* ----------------------------------------------------------------------- 
** Interface 
** ----------------------------------------------------------------------- */ 

/** @name  Mandatory APIs */
/** @{ */

/**
 * Configure the SlimBus HAL environment.
 *  
 * @param[in] pEnv - Pointer to the environment data structure. 
 *
 * @return None
 *
 * @par Dependencies
 *    None
*/ 
void HAL_sb_ConfigEnv(const HAL_sb_EnvType *pEnv);


/**
 * Initalize the SB HAL. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *
 * @param[out] ppszSbVersion - pointer to SB Version String.
 *
 * @return None
 *
 * @par Dependencies
 *    None
*/ 
void HAL_sb_Init(HAL_sb_HandleType hHandle, char **ppszSbVersion);


/**
 * Reset the SB 
 *  
 * @param[in] hHandle - The SB hHandle.
 *
 * @return None
 * 
 * @par Dependencies
 *   None.
 */
void HAL_sb_Reset(HAL_sb_HandleType hHandle);


/** @} */
/** @name Component Apis */
/** @{ */


/**
 * Get the hardware version 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 *  
 * @return Hardware Version.
 * 
 * @attention
 *   None.
 */
uint32 HAL_sb_CompGetHwVersion(HAL_sb_HandleType hHandle);

/**
 * Check if the combined port DMA interrupt feature is supported
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 *  
 * @return TRUE if the feature is supported, FALSE otherwise. 
 * 
 * @attention
 *   None.
 */
bool32 HAL_sb_IsPortDmaIntSupported(HAL_sb_HandleType hHandle);

/**
 * Check if the shadow progress counter and timestamp features 
 * are supported 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 *  
 * @return TRUE if the features are supported, FALSE otherwise. 
 * 
 * @attention
 *   None.
 */
bool32 HAL_sb_IsShadowProgressCounterSupported(HAL_sb_HandleType hHandle);

/**
 * Check if the port upper watermark feature is supported 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 *  
 * @return TRUE if the feature is supported, FALSE otherwise. 
 * 
 * @attention
 *   None.
 */
bool32 HAL_sb_IsPortUpperWatermarkSupported(HAL_sb_HandleType hHandle);

/**
 * Check if the port multi-endpoint feature is supported
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 *  
 * @return TRUE if the feature is supported, FALSE otherwise. 
 * 
 * @attention
 *   None.
 */
bool32 HAL_sb_IsPortMultiEndpointSupported(HAL_sb_HandleType hHandle);

/**
 * Checks if the hardware Frame Sync Shift support is available for this 
 * chip or not.
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 * 
 * @return True/False 
 * 
 * @attention 
 *   None.
 */
bool32 HAL_sb_IsFrameSyncShiftSupported(HAL_sb_HandleType hHandle);

/**
 * Checks if scheduling a data channel starting on slot 0 on a 
 * secondary data line is supported for this chip or not. 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 * 
 * @return True/False 
 * 
 * @attention 
 *   None.
 */
bool32 HAL_sb_IsSecondaryDataLineSlot0Supported(HAL_sb_HandleType hHandle);

/**
 * Initialize retention configuration for the component.
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 * 
 * @return None 
 * 
 * @attention 
 *   None.
 */
void HAL_sb_CompInitRetention(HAL_sb_HandleType hHandle);

/**
 * Enable retention for the component.
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 * 
 * @return None 
 * 
 * @attention 
 *   None.
 */
void HAL_sb_CompEnableRetention(HAL_sb_HandleType hHandle);

/**
 * Disable retention for the component.
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 * 
 * @return None 
 * 
 * @attention 
 *   None.
 */
void HAL_sb_CompDisableRetention(HAL_sb_HandleType hHandle);

/**
 * Checks if retention is enabled for the component or not. 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 * 
 * @return True/False 
 * 
 * @attention 
 *   None.
 */
bool32 HAL_sb_CompIsRetentionEnabled(HAL_sb_HandleType hHandle);

/**
 * Enables the entire component. 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 * 
 * @return None
 * 
 * @sa HAL_sb_CompDisable, HAL_sb_CompReset
 * 
 * @attention
 *   None.
 */ 
void HAL_sb_CompEnable(HAL_sb_HandleType hHandle);


/**
 * Disables the entire component. 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 * 
 * @return None
 * 
 * @sa HAL_sb_CompEnable, HAL_sb_CompReset
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompDisable(HAL_sb_HandleType hHandle);

/**
 * Enables the Hardware Frame Sync Shift Support.
 * 
 * @param[in] hHandle - The SlimBus hHandle.
 * 
 * @return None
 * 
 * @sa HAL_sb_CompDisablePhaseShift
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompEnablePhaseShift(HAL_sb_HandleType hHandle);

/**
 * Disables the Hardware Frame Sync Shift Support.
 * 
 * @param[in] hHandle - The SlimBus hHandle.
 * 
 * @return None
 * 
 * @sa HAL_sb_CompEnablePhaseShift
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompDisablePhaseShift(HAL_sb_HandleType hHandle);

/**
 * Check if the component is enabled. 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 * 
 * @return TRUE, if enabled, FALSE otherwise
 * 
 * @sa HAL_sb_CompEnable, HAL_sb_CompDisable
 * 
 * @attention
 *   None.
 */
bool32 HAL_sb_CompIsEnabled(HAL_sb_HandleType hHandle);


/**
 * Resets the component. 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 * 
 * @return TRUE on successful reset, FALSE otherwise
 * 
 * @sa HAL_sb_CompEnable, HAL_sb_CompDisable
 * 
 * @attention
 *   None.
 */
bool32 HAL_sb_CompReset(HAL_sb_HandleType hHandle);


/**
 * Assert an Interrupt for a particular Execution environment. 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 *  
 * @param[in] uEE - EE index.
 * 
 * @return None
 * 
 * @sa HAL_sb_CompdeAssertInterruptEe
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompAssertInterruptEe(HAL_sb_HandleType hHandle, uint32 uEE); 


/**
 * Deassert an Interrupt for a particular Execution environment.
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 *  
 * @param[in] uEE - EE index.
 * 
 * @return None
 * 
 * @sa HAL_sb_CompAssertInterruptEe
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompDeassertInterruptEe(HAL_sb_HandleType hHandle, uint32 uEE);


/**
 * Enable Debug Bus to be actively driven by the Slimbus 
 * Component. 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 *  
 * @return None
 * 
 * @sa HAL_sb_CompTestBusDisable
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompTestBusEnable(HAL_sb_HandleType hHandle);


/**
 * Disable Debug Bus to be actively driven by the Slimbus 
 * Component. 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 *  
 * @return None
 * 
 * @sa HAL_sb_CompTestBusEnable
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompTestBusDisable(HAL_sb_HandleType hHandle);


/**
 * Check if Debug Bus is enabled to be actively driven by the 
 * Slimbus Component. 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 *  
 * @return True, if enabled, FALSE otherwise
 * 
 * @sa HAL_sb_CompTestBusDisable, HAL_sb_CompTestBusEnable
 * 
 * @attention
 *   None.
 */
bool32 HAL_sb_CompTestBusIsEnabled(HAL_sb_HandleType hHandle);


/**
 * Set the Test Bus Select. 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 *  
 * @param[in] eTestBus - Test Bus type.
 *  
 * @return None
 * 
 * @sa HAL_sb_CompGetTestBusSelect
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompSetTestBusSelect(HAL_sb_HandleType hHandle, 
                                 HAL_sb_TestBusType eTestBus);

/**
 * Get the Test Bus Select. 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 *  
 * @return Test Bus type.
 * 
 * @sa HAL_sb_CompSetTestBusSelect
 * 
 * @attention
 *   None.
 */
HAL_sb_TestBusType HAL_sb_CompGetTestBusSelect(HAL_sb_HandleType hHandle);


/**
 * Blocks Reconfig messages from being sent or executed if 
 * received. 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 *  
 * @return None
 * 
 * @sa HAL_sb_CompUnhaltReconfig, HAL_sb_CompIsHaltReconfig.
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompHaltReconfig(HAL_sb_HandleType hHandle);


/**
 * Unblocks Reconfig messages from being sent or executed if 
 * received. 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 *  
 * @return None
 * 
 * @sa HAL_sb_CompHaltReconfig, HAL_sb_CompIsHaltReconfig.
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompUnhaltReconfig(HAL_sb_HandleType hHandle);


/**
 * Check if Reconfig messages are blocked from being sent or
 * executed if received. 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 *  
 * @return TRUE, if reconfig is blocked, FALSE otherwise
 * 
 * @sa HAL_sb_CompHaltReconfig, HAL_sb_CompUnhaltReconfig
 * 
 * @attention
 *   None.
 */
bool32 HAL_sb_CompIsHaltReconfig(HAL_sb_HandleType hHandle);


/**
 * Blocks SW Reset. 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 *  
 * @return None
 * 
 * @sa HAL_sb_CompUnhaltSwReset, HAL_sb_CompIsHaltSwReset
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompHaltSwReset(HAL_sb_HandleType hHandle);


/**
 * Unblocks SW Reset. 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 *  
 * @return None
 * 
 * @sa HAL_sb_CompHaltSwReset, HAL_sb_CompIsHaltSwReset
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompUnhaltSwReset(HAL_sb_HandleType hHandle);


/**
 * Check to see if SW Reset is blocked. 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 *  
 * @return TRUE, if SW Reset is blocked, FALSE otherwise
 * 
 * @sa HAL_sb_CompHaltSwReset, HAL_sb_CompUnhaltSwReset.
 * 
 * @attention
 *   None.
 */
bool32 HAL_sb_CompIsHaltSwReset(HAL_sb_HandleType hHandle);


/**
 * Blocks changes to the EN bit of all devices in this 
 * component. 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 *  
 * @return None
 * 
 * @sa HAL_sb_CompUnhaltEnChange, HAL_sb_CompIsHaltEnChange
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompHaltEnChange(HAL_sb_HandleType hHandle);


/**
 * Unlocks changes to EN bit of all devices in this component. 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 *  
 * @return None
 * 
 * @sa HAL_sb_CompUnHaltEnChange, HAL_sb_CompIsHaltEnChange
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompUnhaltEnChange(HAL_sb_HandleType hHandle);


/**
 * Check if changes to EN bit of all devices in this component 
 * is blocked. 
 * 
 * @param[in] hHandle - This SlimBus hHandle.
 *  
 * @return TRUE, if blocked, FALSE otherwise
 * 
 * @sa HAL_sb_CompUnhaltEnChange, HAL_sb_CompUnhaltEnChange
 * 
 * @attention
 *   None.
 */
bool32 HAL_sb_CompIsHaltEnChange(HAL_sb_HandleType hHandle);


/**
 * Blocks Slimbus Devices to send any Slimbus message to the 
 * trusted device. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Device type. 
 *  
 * @return None
 * 
 * @sa HAL_sb_CompUnhaltDeviceTxMsg, HAL_sb_CompIsHaltDeviceTxMsg
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompHaltDeviceTxMsg(HAL_sb_HandleType hHandle, 
                                HAL_sb_DeviceType eSbDevice);


/**
 * Unblocks Slimbus Device to send any Slimbus message to the 
 * trusted device. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Device type. 
 *  
 * @return None
 * 
 * @sa HAL_sb_CompHaltDeviceTxMsg, HAL_sb_CompIsHaltDeviceTxMsg
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompUnhaltDeviceTxMsg(HAL_sb_HandleType hHandle, 
                                  HAL_sb_DeviceType eSbDevice);


/**
 * Check if Slimbus Device are blocked to send any Slimbus 
 * message to the trusted device. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Device type. 
 *  
 * @return TRUE, if device is blocked, FALSE otherwise
 * 
 * @sa HAL_sb_CompHaltDeviceTxMsg, HAL_sb_CompUnhaltDeviceTxMsg
 * 
 * @attention
 *   None.
 */
bool32 HAL_sb_CompIsHaltDeviceTxMsg(HAL_sb_HandleType hHandle, 
                                    HAL_sb_DeviceType eSbDevice);


/**
 * Set the Execution Environment value for NGDs. NGDs interrupts
 * will be routed to this programmed EE. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eNGD - NGD index.  
 *  
 * @param[in] uEE - EE index.
 *   
 * @return None
 * 
 * @sa HAL_sb_CompGetNgdEe
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompSetNgdEe(HAL_sb_HandleType hHandle, 
                         HAL_sb_NgdDeviceType eNGD, 
                         uint32 uEE);


/**
 * Get the Execution Environment value for NGDs. NGDs interrupts
 * are routed to this programmed EE. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eNGD - NGD index.  
 *   
 * @return EE index.
 * 
 * @sa HAL_sb_CompSetNgdEe
 * 
 * @attention
 *   None.
 */
uint32 HAL_sb_CompGetNgdEe(HAL_sb_HandleType hHandle, 
                                  HAL_sb_NgdDeviceType eNGD);


/**
 * Set the Execution Environment value for Manager Resource 
 * Group. Interrupts will be routed to this programmed EE. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] uEE - EE index.
 *   
 * @return None
 * 
 * @sa HAL_sb_CompGetMgrRscGrpEe
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompSetMgrRscGrpEe(HAL_sb_HandleType hHandle, uint32 uEE);


/**
 * Get the Execution Environment value for Manager Resource 
 * Group. Interrupts will be routed to this programmed EE. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *   
 * @return EE index.
 * 
 * @sa HAL_sb_CompSetMgrRscGrpEe
 * 
 * @attention
 *   None.
 */
uint32 HAL_sb_CompGetMgrRscGrpEe(HAL_sb_HandleType hHandle);


/**
 * Enables the Manager to monitor for said Trusted device 
 * access. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *   
 * @return None.
 * 
 * @sa HAL_sb_CompMgrSetTrustDevAccessDisable, 
 *     HAL_sb_CompMgrIsTrustDevAccessEnabled
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompMgrSetTrustDevAccessEnable(HAL_sb_HandleType hHandle);


/**
 * Disables the Manager to monitor for said Trusted device 
 * access. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *   
 * @return None.
 * 
 * @sa HAL_sb_CompMgrSetTrustDevAccessEnable, 
 *     HAL_sb_CompMgrIsTrustDevAccessEnabled
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompMgrSetTrustDevAccessDisable(HAL_sb_HandleType hHandle);


/**
 * Ckecks if the Manager is enabled to monitor for said Trusted 
 * device access. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *   
 * @return  True, If Trusted Device Access is enabled, FALSE 
 *          otherwise
 * 
 * @sa HAL_sb_CompMgrSetTrustDevAccessDisable, 
 *     HAL_sb_CompMgrGetTrustDevAccessEnable
 * 
 * @attention
 *   None.
 */
bool32 HAL_sb_CompMgrIsTrustDevAccessEnabled(HAL_sb_HandleType hHandle);


/**
 * Set the Logical address of the Trusted Device. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] uLogicalAddress - Logical Address. 
 *  
 * @return None.
 * 
 * @sa HAL_sb_CompGetTrustDevLa. 
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompSetTrustDevLa(HAL_sb_HandleType hHandle, 
                              uint32 uLogicalAddress);


/**
 * Get the Logical address of the Trusted Device. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return Logical Address.
 * 
 * @sa HAL_sb_CompSetTrustDevLa. 
 * 
 * @attention
 *   None.
 */
uint32 HAL_sb_CompGetTrustDevLa(HAL_sb_HandleType hHandle);


/**
 * Set the Enumeration address of the Trusted Device. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] pucEnumAddress - Pointer to the Enumeration address.
 *                             pucEnumAddress[0] = LSB byte of EA
 *                             pucEnumAddress[5] = MSB byte of EA
 *  
 * @return None.
 * 
 * @sa HAL_sb_CompGetTrustDevEa. 
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompSetTrustDevEa(HAL_sb_HandleType hHandle, uint8 *pucEnumAddress);


/**
 * Get the Enumeration address of the Trusted Device. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *
 * @param[out] pucEnumAddress - Pointer to the Enumeration address.
 *                             pucEnumAddress[0] = LSB byte of EA
 *                             pucEnumAddress[5] = MSB byte of EA
 *
 * @return None.
 * 
 * @sa HAL_sb_CompSetTrustDevEa. 
 * 
 * @attention
 *   None.
 */
void HAL_sb_CompGetTrustDevEa(HAL_sb_HandleType hHandle, uint8 *pucEnumAddress);


/** @} */
/** @name Common Device APIs */
/** @{ */


/**
 * Enable the Device.
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device to be enabled. 
 *  
 * @return None.
 * 
 * @sa HAL_sb_DeviceIsEnabled, HAL_sb_DeviceDisable. 
 * 
 * @attention
 *   None.
 */
void HAL_sb_DeviceEnable(HAL_sb_HandleType hHandle, 
                         HAL_sb_DeviceType eSbDevice);


/**
 * Disable the Device.
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device to be disabled. 
 *  
 * @return None.
 * 
 * @sa HAL_sb_DeviceIsEnabled, HAL_sb_DeviceEnable. 
 * 
 * @attention
 *   None.
 */
void HAL_sb_DeviceDisable(HAL_sb_HandleType hHandle, 
                          HAL_sb_DeviceType eSbDevice);

/**
 * Check if the device is enabled.
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device to be disabled. 
 *  
 * @return TRUE, if device is enabled, FALSE otherwise.
 * 
 * @sa HAL_sb_DeviceEnable, HAL_sb_DeviceDisable. 
 * 
 * @attention
 *   None.
 */
bool32 HAL_sb_DeviceIsEnabled(HAL_sb_HandleType hHandle, 
                              HAL_sb_DeviceType eSbDevice);


/**
 * Get the Error status that the device may want to report. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *  
 * @return ERROR_STATUS
 * 
 * @sa HAL_sb_DeviceIsErrorSet. 
 * 
 * @attention Error status is only valid if DEV_ERR Int is set 
 *            for the respective device.
 */
uint32 HAL_sb_DeviceGetErrorStatus(HAL_sb_HandleType hHandle, 
                                   HAL_sb_DeviceType eSbDevice);


/**
 * Set Interrupt Mask for a device
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *  
 * @param[in] uMask - bitmask of interrupt to enable/disable - 
 *            1=enable/0=disable see 
 *       HAL_sb_MgrIntType for Manager device.
 *       HAL_sb_FrmIntType for Framer device.
 *       HAL_sb_IntfIntType for Interface device.
 *       HAL_sb_NgdIntType for Non Ported generic devices.
 *       HAL_sb_PgdIntType for Ported Generic devices.
 *  
 * @return None
 * 
 * @sa HAL_sb_DeviceIntGetMask. 
 * 
 * @attention None.
 */
void HAL_sb_DeviceIntSetMask(HAL_sb_HandleType hHandle, 
                             HAL_sb_DeviceType eSbDevice, 
                             uint32 uMask);


/**
 * Get Interrupt Mask for a device
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *  
 * @return bitmask of interrupts enable/disable - 
 *            1=enable/0=disable see 
 *       HAL_sb_MgrIntType for Manager device.
 *       HAL_sb_FrmIntType for Framer device.
 *       HAL_sb_IntfIntType for Interface device.
 *       HAL_sb_NgdIntType for Non Ported generic devices.
 *       HAL_sb_PgdIntType for Ported Generic devices.
 * 
 * @sa HAL_sb_DeviceIntSetMask. 
 * 
 * @attention None.
 */
uint32 HAL_sb_DeviceIntGetMask(HAL_sb_HandleType hHandle, 
                               HAL_sb_DeviceType eSbDevice);


/**
 * Get the status of Interrupts for a device.
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *  
 * @return bitmask of interrupt status - see 
 *       HAL_sb_MgrIntType for Manager device.
 *       HAL_sb_FrmIntType for Framer device.
 *       HAL_sb_IntfIntType for Interface device.
 *       HAL_sb_NgdIntType for Non Ported generic devices.
 *       HAL_sb_PgdIntType for Ported Generic devices.
 * 
 * @sa HAL_sb_DeviceIntClear
 * 
 * @attention None.
 */
uint32 HAL_sb_DeviceIntGetStatus(HAL_sb_HandleType hHandle, 
                                 HAL_sb_DeviceType eSbDevice);


/**
 * Clear Interrupts for a device.
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *  
 * @param[in] uMask- bitmask of interrupt to clear - see 
 *       HAL_sb_MgrIntType for Manager device.
 *       HAL_sb_FrmIntType for Framer device.
 *       HAL_sb_IntfIntType for Interface device.
 *       HAL_sb_NgdIntType for Non Ported generic devices.
 *       HAL_sb_PgdIntType for Ported Generic devices.
 *  
 * @return None.
 * 
 * @sa HAL_sb_DeviceIntGetStatus
 * 
 * @attention None.
 */
void HAL_sb_DeviceIntClear(HAL_sb_HandleType hHandle, 
                           HAL_sb_DeviceType eSbDevice, 
                           uint32 uMask);


/**
 * Get the Value Element Status of a device.
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *  
 * @return VE Status.
 * 
 * @sa None.
 * 
 * @attention None.
 */
uint32 HAL_sb_DeviceGetVeStatus(HAL_sb_HandleType hHandle, 
                                HAL_sb_DeviceType eSbDevice);


/**
 * Get the Information Element Status of the device.
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *
 * @return IE Status  See
 *                 HAL_sb_MgrIeType  for Manager device
 *                 HAL_sb_IntfIeType  for Interface device
 *                 HAL_sb_FrmType  for Framer device
 *                 HAL_sb_GdIeType  for Generic device (Ported and Non Ported)
 * 
 * @sa None.
 * 
 * @attention Caller needs to typecast the return value to one of struct 
 *            listed above
 */
uint32 HAL_sb_DeviceGetIeStatus(HAL_sb_HandleType hHandle, 
                                HAL_sb_DeviceType eSbDevice);



/**
 * Check if the Device's Tx Msg Buffer is Busy.
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *  
 * @return TRUE, if Buffer is busy, FALSE otherwise
 * 
 * @sa None.
 * 
 * @attention Only valid for devices having direct access 
 *            buffers. Should be used only if the device is
 *            enumerated. Should not be used as a check for tx
 *            msg sent.
 */
bool32 HAL_sb_DeviceIsTxMsgBufferBusy(HAL_sb_HandleType hHandle, 
                                      HAL_sb_DeviceType eSbDevice);


/**
 * Check if the Device has been Enumerated.
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *  
 * @return TRUE, if the device has been enumerated, FALSE 
 *         otherwise
 * 
 * @sa None.
 * 
 * @attention None.
 */
bool32 HAL_sb_DeviceIsEnumerated(HAL_sb_HandleType hHandle, 
                                 HAL_sb_DeviceType eSbDevice);


/**
 * Get the Pipe num offset of a Message Queue
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *
 * @param[in] eMsgQ - Device Message Queue Type.
 *  
 * @return PIPE_NUM_OFFSET
 * 
 * @sa None.
 * 
 * @attention Only valid for devices having MsgQs. 
 */
uint32 HAL_sb_DeviceMsgQGetPipeOffset(HAL_sb_HandleType hHandle, 
                                      HAL_sb_DeviceType eSbDevice,
                                      HAL_sb_DeviceMsgQType eMsgQ);


/**
 * Get the ACKED TX Message MC
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *  
 * @return ACKED_TX_MSG_MC
 * 
 * @sa HAL_sb_DeviceTxMsgGetNackedMc.
 * 
 * @attention None.
 */
uint32 HAL_sb_DeviceTxMsgGetAckedMc(HAL_sb_HandleType hHandle, 
                                    HAL_sb_DeviceType eSbDevice);


/**
 * Get the NACKED TX Message MC
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *  
 * @return NACKED_TX_MSG_MC
 * 
 * @sa HAL_sb_DeviceTxMsgGetAckedMc.
 * 
 * @attention None.
 */
uint32 HAL_sb_DeviceTxMsgGetNackedMc(HAL_sb_HandleType hHandle, 
                                     HAL_sb_DeviceType eSbDevice);


/**
 * Set the Rx Message Queue Block Size. After receiving that 
 * many messages, a block_end is asserted 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *  
 * @param[in] uBlockSize - Block size specified in units of 
 *       messages.
 *  
 * @return None. 
 *  
 * @sa HAL_sb_DeviceRxMsgQGetBlockSize.
 * 
 * @attention Only valid for devices having MsgQs.
 */
void HAL_sb_DeviceRxMsgQSetBlockSize(HAL_sb_HandleType hHandle, 
                                     HAL_sb_DeviceType eSbDevice, 
                                     uint32 uBlockSize);


/**
 * Get the Rx Message Queue Block Size. After receiving that 
 * many messages, a block_end is asserted 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *  
 * @return Block size specified in units of messages
 * 
 * @sa HAL_sb_DeviceRxMsgQSetBlockSize.
 * 
 * @attention Only valid for devices having MsgQs.
 */
uint32 HAL_sb_DeviceRxMsgQGetBlockSize(HAL_sb_HandleType hHandle, 
                                       HAL_sb_DeviceType eSbDevice);


/**
 * Set the Rx Message Queue Transaction Size. After receiving 
 * that many messages, a trans_end is asserted 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *  
 * @param[in] uTransSize - Transaction size specified in units 
 *       of messages.
 * 
 * @return None.
 *  
 * @sa HAL_sb_DeviceRxMsgQGetTransSize.
 * 
 * @attention Only valid for devices having MsgQs.
 */
void  HAL_sb_DeviceRxMsgQSetTransSize(HAL_sb_HandleType hHandle, 
                                      HAL_sb_DeviceType eSbDevice, 
                                      uint32 uTransSize);


/**
 * Get the Rx Message Queue Transaction Size. After receiving 
 * that many messages, a trans_end is asserted 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *  
 * @return Transaction size specified in units of messages 
 *  
 * @sa HAL_sb_DeviceRxMsgQSetTransSize.
 * 
 * @attention Only valid for devices having MsgQs.
 */
uint32 HAL_sb_DeviceRxMsgQGetTransSize(HAL_sb_HandleType hHandle, 
                                       HAL_sb_DeviceType eSbDevice);


/**
 * Set the Time out value for Rx Message Queue 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *  
 * @param[in] uTimeoutValInAhbClkCycles - Timeout value in units
 *       of AHB cycles
 *  
 * @return None. 
 *  
 * @sa HAL_sb_DeviceRxMsgQGetTimeOutVal.
 * 
 * @attention Time out counter is enabled if this is set to a 
 *            non zero value and when the first message is
 *            received. Time out cntr reset after every Rx msg
 *            is received. If next Rx message not received
 *            before it expires, a trans_end is automatically
 *            generated even if BLOCK_SIZE/TRANS_SIZE is not
 *            got. Specified in terms of AHB Bus clock cycles
 *            given to the core by the ClockCtrl of SoC. Only
 *            valid for devices having MsgQs.
 */
void HAL_sb_DeviceRxMsgQSetTimeOutVal(HAL_sb_HandleType hHandle, 
                                      HAL_sb_DeviceType eSbDevice, 
                                      uint32 uTimeoutValInAhbClkCycles);


/**
 * Get the Time out value for Rx Message Queue 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *  
 * @return Timeout value in units of AHB cycles 
 *  
 * @sa HAL_sb_DeviceRxMsgQSetTimeOutVal.
 * 
 * @attention Time out counter is enabled if this is set to a 
 *            non zero value and when the first message is
 *            received. Time out cntr reset after every Rx msg
 *            is received. If next Rx message not received
 *            before it expires, a trans_end is automatically
 *            generated even if BLOCK_SIZE/TRANS_SIZE is not
 *            got. Specified in terms of AHB Bus clock cycles
 *            given to the core by the ClockCtrl of SoC. Only
 *            valid for devices having MsgQs.
 */
uint32 HAL_sb_DeviceRxMsgQGetTimeOutVal(HAL_sb_HandleType hHandle, 
                                        HAL_sb_DeviceType eSbDevice);


/**
 * Write Tx message to be sent over Slimbus. This is direct 
 * access messaging API, bypassing the MsgQs. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *  
 * @param[in] pBuffer - Pointer to the Write Buffer. 
 *  
 * @param[in] uBufferSize - Buffer size in words (uint32).
 *  
 * @return None. 
 *  
 * @sa HAL_sb_DeviceRxMsgRead.
 * 
 * @attention This Direct access Messaging API SHOULD NOT be 
 * used if any of the Tx queues are enabled. The resulting 
 * behavior is not defined. Only valid for devices having 
 * direct access buffers. 
 */
void HAL_sb_DeviceTxMsgWrite(HAL_sb_HandleType hHandle, 
                             HAL_sb_DeviceType eSbDevice, 
                             uint32* pBuffer, 
                             uint32 uBufferSize);


/**
 * Read Rx message from over Slimbus.This is direct 
 * access messaging API, bypassing the MsgQs. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *  
 * @param[in] pBuffer - Pointer to the Read Buffer. 
 *  
 * @param[in] uBufferSize - Buffer size in words (uint32).
 *  
 * @return None. 
 *  
 * @sa HAL_sb_DeviceTxMsgWrite.
 * 
 * @attention This Direct access Messaging API SHOULD NOT be 
 * used if any of the Rx queues are enabled. The resulting 
 * behavior is not defined. Only valid for devices having direct
 * access buffers. 
 */
void HAL_sb_DeviceRxMsgRead(HAL_sb_HandleType hHandle, 
                            HAL_sb_DeviceType eSbDevice, 
                            uint32* pBuffer, 
                            uint32 uBufferSize);


/**
 * Check if Message Queue is enabled on this device
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *   
 * @param[in] eSbDevice - Slimbus device type. 
 *
 * @param[in] eMsgQ - Message Queue type. 
 *  
 * @return TRUE, if message queue is enabled, FALSE, otherwise. 
 *  
 * @sa HAL_sb_DeviceMsgQEnable, HAL_sb_DeviceMsgQDisable.
 * 
 * @attention If the respective message queue is not supported 
 *            on the device, this function will always return
 *            FALSE.
 */
bool32 HAL_sb_DeviceMsgQIsEnabled(HAL_sb_HandleType hHandle, 
                                  HAL_sb_DeviceType eSbDevice,
                                  HAL_sb_DeviceMsgQType eMsgQ);


/**
 * Enable the message Queue on this device, if supported
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *
 * @param[in] eMsgQ - Message Queue type. 
 *  
 * @return TRUE, if supported and enabled, FALSE, if not 
 *         supported on this device.
 *  
 * @sa HAL_sb_DeviceMsgQIsEnabled, HAL_sb_DeviceMsgQDisable.
 * 
 * @attention If the respective message queue is not supported 
 *            on the device, this function will always return
 *            FALSE.
 */
bool32 HAL_sb_DeviceMsgQEnable(HAL_sb_HandleType hHandle, 
                               HAL_sb_DeviceType eSbDevice,
                               HAL_sb_DeviceMsgQType eMsgQ);


/**
 * Disable the message Queue on this device, if supported
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] eSbDevice - Slimbus device type. 
 *
 * @param[in] eMsgQ - Message Queue type. 
 *  
 * @return None.
 *  
 * @sa HAL_sb_DeviceMsgQIsEnabled, HAL_sb_DeviceMsgQEnable.
 * 
 * @attention None.
 */
void HAL_sb_DeviceMsgQDisable(HAL_sb_HandleType hHandle, 
                              HAL_sb_DeviceType eSbDevice, 
                              HAL_sb_DeviceMsgQType eMsgQ);


/**
 * Check if the Tx and Rx Msg Queues are supported on this 
 * device. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] eSbDevice - Slimbus device type. 
 *  
 * @return TRUE, if supported, FALSE, otherwise.
 * 
 * @sa None.
 * 
 * @attention None.
 */
bool32 HAL_sb_DeviceIsMsgQsSupported(HAL_sb_HandleType hHandle, 
                                     HAL_sb_DeviceType eSbDevice);


/** @} */
/** @name Interface Device APIs */
/** @{ */


/**
 * Get the Interface Assigned Logical Address. 
 * 
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return Interface Assigned Logical Address.
 *  
 * @attention None.
 */
uint32 HAL_sb_IntfGetAssignedLA(HAL_sb_HandleType hHandle);


/**
 * Get the Interface Frame Sync Status. 
 * 
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return TRUE if frame sync is acquired, FALSE otherwise 
 *  
 * @attention None.
 */
bool32 HAL_sb_IntfIsFrameSyncAcquired(HAL_sb_HandleType hHandle);


/**
 * Get the Interface Superframe Sync Status. 
 * 
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return TRUE if superframe sync is acquired, FALSE otherwise 
 *  
 * @attention None.
 */
bool32 HAL_sb_IntfIsSuperframeSyncAcquired(HAL_sb_HandleType hHandle);


/**
 * Get the Interface Message Sync Status. 
 * 
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return TRUE if message sync is acquired, FALSE otherwise 
 *  
 * @attention None.
 */
bool32 HAL_sb_IntfIsMessageSyncAcquired(HAL_sb_HandleType hHandle);


/** @} */
/** @name Framer Device APIs */
/** @{ */


/**
 * Enables devices within the component to wakeup Framer when in
 * idle mode. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return None.
 *  
 * @sa HAL_sb_FrmInternalWakeupDisable, HAL_sb_FrmIsInternalWakeupEnabled.
 * 
 * @attention None.
 */
void HAL_sb_FrmInternalWakeupEnable(HAL_sb_HandleType hHandle);


/**
 * Disables devices within the component to wakeup Framer when 
 * in idle mode. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return None.
 *  
 * @sa HAL_sb_FrmInternalWakeupEnable, HAL_sb_FrmIsInternalWakeupEnabled.
 * 
 * @attention None.
 */
void HAL_sb_FrmInternalWakeupDisable(HAL_sb_HandleType hHandle);


/**
 * Checks if devices within the component are enabled to wakeup 
 * Framer when in idle mode. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return TRUE, if enabled, FALSE if disabled.
 *  
 * @sa HAL_sb_FrmInternalWakeupDisable, 
 *     HAL_sb_FrmInternalWakeupEnable
 * 
 * @attention None.
 */
bool32 HAL_sb_FrmIsInternalWakeupEnabled(HAL_sb_HandleType hHandle);


/**
 * Get the Framer Reference Clk CG. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return Equivalent CG of the Slimbus reference Clk.
 *  
 * @sa HAL_sb_FrmSetRefClkClkGear.
 * 
 * @attention None.
 */
uint32 HAL_sb_FrmGetRefClkClkGear(HAL_sb_HandleType hHandle);


/**
 * Set the Framer Reference Clk CG. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] uRefClkClkGear - Equivalent CG of the Slimbus reference
 *       Clk.
 *  
 * @return None.
 *  
 * @sa HAL_sb_FrmGetRefClkClkGear.
 * 
 * @attention None.
 */
void HAL_sb_FrmSetRefClkClkGear(HAL_sb_HandleType hHandle, 
                                uint32 uRefClkClkGear);


/**
 * Get the Framer Root Frequency at Boot. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return Framer Root Frequency at Boot.
 *  
 * @sa HAL_sb_FrmSetBootRootFreq.
 * 
 * @attention None.
 */
uint32 HAL_sb_FrmGetBootRootFreq(HAL_sb_HandleType hHandle);


/**
 * Set the Framer Root Frequency at Boot. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] uBootRootFreq - Framer Root Frequency at Boot.
 *  
 * @return None.
 *  
 * @sa HAL_sb_FrmGetBootRootFreq.
 * 
 * @attention None.
 */
void HAL_sb_FrmSetBootRootFreq(HAL_sb_HandleType hHandle, uint32 uBootRootFreq);


/**
 * Get the Framer Clk Gear at Boot. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return Framer Clk Gear at Boot.
 *  
 * @sa HAL_sb_FrmSetBootClkGear.
 * 
 * @attention None.
 */
uint32 HAL_sb_FrmGetBootClkGear(HAL_sb_HandleType hHandle);


/**
 * Set the Framer Clk Gear at Boot. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] uBootClkGear - Framer Clock Gear at Boot.
 *  
 * @return None.
 *  
 * @sa HAL_sb_FrmGetBootRootFreq.
 * 
 * @attention None.
 */
void HAL_sb_FrmSetBootClkGear(HAL_sb_HandleType hHandle, uint32 uBootClkGear);


/**
 * Get the Framer SubFrame mode at Boot. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return Framer SubFrame mode at at Boot.
 *  
 * @sa HAL_sb_FrmSetBootSubFrmMode.
 * 
 * @attention None.
 */
uint32 HAL_sb_FrmGetBootSubFrmMode(HAL_sb_HandleType hHandle);


/**
 * Set the Framer SubFrame mode at at Boot. 
 * 
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @param[in] uBootSubFrmMode - Framer SubFrame mode at at Boot.
 *  
 * @return None.
 *  
 * @sa HAL_sb_FrmGetBootSubFrmMode.
 * 
 * @attention None.
 */
void HAL_sb_FrmSetBootSubFrmMode(HAL_sb_HandleType hHandle, 
                                 uint32 uBootSubFrmMode);


/**
 * Set this Framer as an active Framer in the Bus at Boot. 
 * 
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return None.
 *  
 * @sa HAL_sb_FrmBootSetUnActiveFrm, HAL_sb_FrmBootIsActiveFrm.
 * 
 * @attention If set, this Framer Device is made the active one
 *            in the bus. This setting takes effect only when
 *            bus is booting up. Any subsequent call to this API
 *            has no effect until bus re-boots again.
 */
void HAL_sb_FrmBootSetActiveFrm(HAL_sb_HandleType hHandle);


/**
 * Set this Framer as an unactive Framer in the Bus at Boot. 
 * 
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return None.
 *  
 * @sa HAL_sb_FrmBootSetActiveFrm,HAL_sb_FrmBootIsActiveFrm.
 * 
 * @attention If set, this Framer Device is made as unactive one
 *            in the bus. This setting takes effect only when
 *            bus is booting up. Any subsequent call to this API
 *            has no effect until bus re-boots again.
 */
void HAL_sb_FrmBootSetUnActiveFrm(HAL_sb_HandleType hHandle);


/**
 * Check if this Framer is set to be an active Framer in the Bus
 * at Boot. 
 * 
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return TRUE if set as active, FALSE if set as Unactive.
 *  
 * @sa HAL_sb_FrmBootSetUnActiveFrm, HAL_sb_FrmBootSetActiveFrm.
 * 
 * @attention None.
 */
bool32 HAL_sb_FrmBootIsActiveFrm(HAL_sb_HandleType hHandle);


/**
 * Check if the Slimbus is Booted. 
 * 
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return TRUE if booted, FALSE otherwise.
 *  
 * @sa None.
 * 
 * @attention None.
 */
bool32 HAL_sb_FrmIsSbBooted(HAL_sb_HandleType hHandle);

/**
 * Get the Framer Assigned Logical Address. 
 * 
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return Framer Assigned Logical Address.
 *  
 * @sa HAL_sb_FrmGetBootSubFrmMode, HAL_sb_FrmSetBootSubFrmMode
 * 
 * @attention None.
 */
uint32 HAL_sb_FrmGetAssignedLA(HAL_sb_HandleType hHandle);

/**
 * Get the Framer Current SubFrame mode. 
 * 
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return Framer Current SubFrame mode.
 *  
 * @sa HAL_sb_FrmGetBootSubFrmMode, HAL_sb_FrmSetBootSubFrmMode
 * 
 * @attention None.
 */
uint32 HAL_sb_FrmGetCurSubFrmMode(HAL_sb_HandleType hHandle);


/**
 * Get the Framer Current Clk Gear. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return Framer Current Clk Gear.
 *  
 * @sa HAL_sb_FrmSetBootClkGear, HAL_sb_FrmGetBootClkGear
 * 
 * @attention None.
 */
uint32 HAL_sb_FrmGetCurClkGear(HAL_sb_HandleType hHandle);


/**
 * Get the Framer Current Root Frequency. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return Framer Current Root Frequency.
 *  
 * @sa HAL_sb_FrmSetBootRootFreq, HAL_sb_FrmGetBootRootFreq.
 * 
 * @attention None.
 */
uint32 HAL_sb_FrmGetCurRootFreq(HAL_sb_HandleType hHandle);


/**
 * Check if this Framer is the active Framer on the bus. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return TRUE if the Framer is the active Framer on the bus,
 *         FALSE otherwise.
 *  
 * @sa None.
 * 
 * @attention None.
 */
bool32 HAL_sb_FrmIsActiveFrm(HAL_sb_HandleType hHandle);


/**
 * Check if this Framer is in Idle Mode. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return TRUE if the Framer is in the idle mode, FALSE otherwise.
 *  
 * @sa HAL_sb_FrmWakeUp.
 * 
 * @attention None.
 */
bool32 HAL_sb_FrmIsInIdleMode(HAL_sb_HandleType hHandle);


/**
 * Wake up Framer, if in Idle mode. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 *  
 * @return None.
 *  
 * @sa HAL_sb_FrmIsInIdleMode.
 * 
 * @attention None.
 */
void HAL_sb_FrmWakeUp(HAL_sb_HandleType hHandle);

/** @} */
/** @name PGD Device APIs */
/** @{ */


/**
 * Get the number of ports in this Generic Device. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index.
 *  
 * @return NUM_PORTS.
 * 
 * @sa None.
 * 
 * @attention None.
 */
uint32 HAL_sb_PgdGetNumPorts(HAL_sb_HandleType hHandle, uint32 uPgdIndex);


/**
 * Get the pipe number offset for this Generic Device. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index.
 *  
 * @return PIPE_NUM_OFFSET.
 * 
 * @sa None.
 * 
 * @attention None.
 */
uint32 HAL_sb_PgdGetPipeOffset(HAL_sb_HandleType hHandle, uint32 uPgdIndex);


/**
 * Get the number of Progressive Counter and VFR Timestamp logic
 * blocks. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index.
 *  
 * @return NUM_PC_VFR_BLKS.
 * 
 * @sa None.
 * 
 * @attention None.
 */
uint32 HAL_sb_PgdGetNumPcVfrBlks(HAL_sb_HandleType hHandle, uint32 uPgdIndex);


/**
 * Set the port interrupt ownership of an EE. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uEE - Execution Environment.
 *  
 * @param[in] uPortBitField - Bit field. If bit n of this 
 *       parameter is set, then uEE owns that port number.
 *  
 * @return HAL_sb_PgdGetNumPorts.
 * 
 * @sa None.
 * 
 * @attention None.
 */
void HAL_sb_PgdPortSetOwnerEe(HAL_sb_HandleType hHandle, 
                              uint32 uPgdIndex, 
                              uint32 uEE, 
                              uint32 uPortBitField);


/**
 * Get the port interrupt ownership of an EE. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uEE - Execution Environment.
 *  
 * @return Bit field. If bit n of this parameter is set, then 
 *         uEE owns that port number.
 * 
 * @sa HAL_sb_PgdGetNumPorts.
 * 
 * @attention None.
 */
uint32 HAL_sb_PgdPortGetOwnerEe(HAL_sb_HandleType hHandle, 
                                uint32 uPgdIndex, 
                                uint32 uEE);


/**
 * Set the ports interrupt enable bit for an EE. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uEE - Execution Environment.
 *  
 * @param[in] uPortBitField - Bit field. If bit n of this 
 *       parameter is set, then nth port interrupt is enabled
 *       for uEE. Only the bit corresponding to owned port
 *       should be changed for an EE.
 *  
 * @return None.
 * 
 * @sa HAL_sb_PgdGetNumPorts.
 * 
 * @attention Only bits upto the number dictacted by 
 *            HAL_sb_PgdGetNumPorts() api are valid.
 */
void HAL_sb_PgdPortSetIntEnEe(HAL_sb_HandleType hHandle, 
                              uint32 uPgdIndex, 
                              uint32 uEE, 
                              uint32 uPortBitField);

/**
 * Get the ports interrupt enable bit for an EE. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uEE - Execution Environment.
 *  
 * @return Bit field. If bit n of this parameter is set, then 
 *       nth port interrupt is enabled for uEE.
 * 
 * @sa HAL_sb_PgdGetNumPorts.
 * 
 * @attention Only bits upto the number dictacted by 
 *            HAL_sb_PgdGetNumPorts() api are valid.
 */
uint32 HAL_sb_PgdPortGetIntEnEe(HAL_sb_HandleType hHandle, 
                                uint32 uPgdIndex, 
                                uint32 uEE);


/**
 * Get the port interrupt status bit for an EE. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uEE - Execution Environment.
 *  
 * @return Bit field. If bit n of this return value is set, 
 *       then nth port interrupt status is set.
 * 
 * @sa HAL_sb_PgdGetNumPorts.
 * 
 * @attention Only bits upto the number dictacted by 
 *            HAL_sb_PgdGetNumPorts() api are valid.
 */
uint32 HAL_sb_PgdPortGetIntStatusEe(HAL_sb_HandleType hHandle, 
                                    uint32 uPgdIndex, 
                                    uint32 uEE);


/**
 * Clear the port interrupt status bit for an EE. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uEE - Execution Environment.
 *  
 * @param[in] uPortBitField -  If bit n of this parameter is set, then 
 *       nth port interrupt status bit will be cleared. Only the
 *       bit corresponding to owned port should be changed for
 *       an EE.
 *  
 * @return None.
 * 
 * @sa HAL_sb_PgdGetNumPorts.
 * 
 * @attention Only bits upto the number dictacted by 
 *            HAL_sb_PgdGetNumPorts() api are valid.
 */
void HAL_sb_PgdPortClearIntEe(HAL_sb_HandleType hHandle, 
                              uint32 uPgdIndex, 
                              uint32 uEE, 
                              uint32 uPortBitField);


/**
 * Enable a Port. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number
 *  
 * @return None.
 * 
 * @sa HAL_sb_PgdPortDisable, HAL_sb_PgdPortIsEnabled.
 * 
 * @attention None.
 */
void HAL_sb_PgdPortEnable(HAL_sb_HandleType hHandle, 
                          uint32 uPgdIndex, 
                          uint32 uPortNum);


/**
 * Disable a Port. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number
 *  
 * @return None.
 * 
 * @sa HAL_sb_PgdPortEnable, HAL_sb_PgdPortIsEnabled.
 * 
 * @attention None.
 */
void HAL_sb_PgdPortDisable(HAL_sb_HandleType hHandle, 
                           uint32 uPgdIndex, 
                           uint32 uPortNum);


/**
 * Check if a Port is enabled. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number
 *  
 * @return TRUE, if port is enabled, FALSE otherwise.
 * 
 * @sa HAL_sb_PgdPortDisable, HAL_sb_PgdPortEnable.
 * 
 * @attention None.
 */
bool32 HAL_sb_PgdPortIsEnabled(HAL_sb_HandleType hHandle, 
                               uint32 uPgdIndex, 
                               uint32 uPortNum);


/**
 * Set the Watermark level for a port. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number.
 *  
 * @param[in] uWaterMark -  Watermark level.
 *  
 * @return None.
 * 
 * @sa HAL_sb_PgdPortGetWatermark, 
 *     HAL_sb_PgdPortSetUpperWatermark,
 *     HAL_sb_PgdPortGetUpperWatermark
 * 
 * @attention None.
 */
void HAL_sb_PgdPortSetWatermark(HAL_sb_HandleType hHandle, 
                                uint32 uPgdIndex, 
                                uint32 uPortNum, 
                                uint32 uWaterMark);


/**
 * Get the Watermark level for a port. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number.
 *  
 * @return Watermark level.
 * 
 * @sa HAL_sb_PgdPortSetWatermark, 
 *     HAL_sb_PgdPortSetUpperWatermark,
 *     HAL_sb_PgdPortGetUpperWatermark
 * 
 * @attention None.
 */
uint32 HAL_sb_PgdPortGetWatermark(HAL_sb_HandleType hHandle, 
                                  uint32 uPgdIndex, 
                                  uint32 uPortNum);

/**
 * Set the upper watermark level for a port. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number.
 *  
 * @param[in] uWaterMark -  Upper watermark level.
 *  
 * @return None.
 * 
 * @sa HAL_sb_PgdPortGetUpperWatermark, 
 *     HAL_sb_PgdPortSetWatermark, HAL_sb_PgdPortGetWatermark,
 *     HAL_sb_PgdPortUpperWatermarkEnable,
 *     HAL_sb_PgdPortUpperWatermarkDisable
 * 
 * @attention None.
 */
void HAL_sb_PgdPortSetUpperWatermark(HAL_sb_HandleType hHandle, 
                                     uint32 uPgdIndex, 
                                     uint32 uPortNum, 
                                     uint32 uWaterMark);


/**
 * Get the Upper watermark level for a port. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number.
 *  
 * @return Upper watermark level.
 * 
 * @sa HAL_sb_PgdPortSetUpperWatermark, 
 *     HAL_sb_PgdPortSetWatermark, HAL_sb_PgdPortGetWatermark,
 *     HAL_sb_PgdPortUpperWatermarkEnable,
 *     HAL_sb_PgdPortUpperWatermarkDisable
 * 
 * @attention None.
 */
uint32 HAL_sb_PgdPortGetUpperWatermark(HAL_sb_HandleType hHandle, 
                                       uint32 uPgdIndex, 
                                       uint32 uPortNum);

/**
 * Enable use of the upper watermark for a port. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number.
 *  
 * @return None.
 * 
 * @sa HAL_sb_PgdPortSetUpperWatermark, 
 *     HAL_sb_PgdPortGetUpperWatermark,
 *     HAL_sb_PgdPortUpperWatermarkDisable 
 * 
 * @attention None.
 */
void HAL_sb_PgdPortUpperWatermarkEnable(HAL_sb_HandleType hHandle, 
                                        uint32 uPgdIndex, 
                                        uint32 uPortNum);

/**
 * Disable use of the upper watermark for a port. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number.
 *  
 * @return None.
 * 
 * @sa HAL_sb_PgdPortSetUpperWatermark, 
 *     HAL_sb_PgdPortGetUpperWatermark,
 *     HAL_sb_PgdPortUpperWatermarkEnable
 * 
 * @attention None.
 */
void HAL_sb_PgdPortUpperWatermarkDisable(HAL_sb_HandleType hHandle, 
                                         uint32 uPgdIndex, 
                                         uint32 uPortNum);

/**
 * Enable the data packing for a port. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number.
 *  
 * @return None.
 * 
 * @sa HAL_sb_PgdPortPackDisable, HAL_sb_PgdPortIsPackEnabled.
 * 
 * @attention None.
 */
void HAL_sb_PgdPortPackEnable(HAL_sb_HandleType hHandle, 
                              uint32 uPgdIndex, 
                              uint32 uPortNum);


/**
 * Disable the data packing for a port. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number.
 *  
 * @return None.
 * 
 * @sa HAL_sb_PgdPortPackEnable, HAL_sb_PgdPortIsPackEnabled.
 * 
 * @attention None.
 */
void HAL_sb_PgdPortPackDisable(HAL_sb_HandleType hHandle, 
                               uint32 uPgdIndex, 
                               uint32 uPortNum);


/**
 * Check if the data packing is enabled for a port. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number.
 *  
 * @return TRUE, if packing enabled, FALSE otherwise.
 * 
 * @sa HAL_sb_PgdPortPackEnable, HAL_sb_PgdPortPackDisable. 
 * 
 * @attention None.
 */
bool32 HAL_sb_PgdPortIsPackEnabled(HAL_sb_HandleType hHandle, 
                                   uint32 uPgdIndex, 
                                   uint32 uPortNum);


/**
 * Set the data alignment settings for a port.
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number. 
 *  
 * @param[in] eAlignment - Alignment type
 *  
 * @return None.
 * 
 * @sa HAL_sb_PgdPortGetAlignment. 
 * 
 * @attention None.
 */
void HAL_sb_PgdPortSetAlignment(HAL_sb_HandleType hHandle, 
                                uint32 uPgdIndex, 
                                uint32 uPortNum, 
                                HAL_sb_PgdPortDataAlignType eAlignment);


/**
 * Get the data alignment settings for a port.
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number. 
 *  
 * @return Alignment type 
 *  
 * @sa HAL_sb_PgdPortSetAlignment. 
 * 
 * @attention None.
 */
HAL_sb_PgdPortDataAlignType HAL_sb_PgdPortGetAlignment
                                       (HAL_sb_HandleType hHandle,
                                        uint32 uPgdIndex, 
                                        uint32 uPortNum);


/**
 * Enable the DMA interrupt for a port. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number.
 *  
 * @return None.
 * 
 * @sa HAL_sb_PgdPortDmaIrqDisable, 
 *     HAL_sb_PgdPortIsDmaIrqEnabled
 * 
 * @attention None.
 */
void HAL_sb_PgdPortDmaIrqEnable(HAL_sb_HandleType hHandle, 
                                uint32 uPgdIndex, 
                                uint32 uPortNum);


/**
 * Disable the DMA interrupt for a port. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number.
 *  
 * @return None.
 * 
 * @sa HAL_sb_PgdPortDmaIrqEnable, 
 *     HAL_sb_PgdPortIsDmaIrqEnabled
 * 
 * @attention None.
 */
void HAL_sb_PgdPortDmaIrqDisable(HAL_sb_HandleType hHandle, 
                                 uint32 uPgdIndex, 
                                 uint32 uPortNum);


/**
 * Check if the DMA interrupt is enabled for a port. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number.
 *  
 * @return TRUE, if DMA interrupt is enabled, FALSE otherwise.
 * 
 * @sa HAL_sb_PgdPortDmaIrqEnable, HAL_sb_PgdPortDmaIrqDisable
 * 
 * @attention None.
 */
bool32 HAL_sb_PgdPortIsDmaIrqEnabled(HAL_sb_HandleType hHandle, 
                                     uint32 uPgdIndex, 
                                     uint32 uPortNum);


/**
 * Get the FIFO size for a port.
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number. 
 *  
 * @return Fifo size in bytes.
 *  
 * @sa None. 
 * 
 * @attention None.
 */
uint32 HAL_sb_PgdPortGetFifoSize(HAL_sb_HandleType hHandle, 
                                 uint32 uPgdIndex, 
                                 uint32 uPortNum);


/**
 * Check if the FIFO has overflowed for a port.
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number. 
 *  
 * @return TRUE, if overflowed. FALSE otherwise.
 *  
 * @sa HAL_sb_PgdPortIsUnderflow. 
 * 
 * @attention None.
 */
bool32 HAL_sb_PgdPortIsOverflow(HAL_sb_HandleType hHandle, 
                                uint32 uPgdIndex, 
                                uint32 uPortNum);


/**
 * Check if the FIFO has underflowed for a port.
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number. 
 *  
 * @return TRUE, if underflowed. FALSE otherwise.
 *  
 * @sa HAL_sb_PgdPortIsOverflow. 
 * 
 * @attention None.
 */
bool32 HAL_sb_PgdPortIsUnderflow(HAL_sb_HandleType hHandle, 
                                 uint32 uPgdIndex, 
                                 uint32 uPortNum);


/**
 * Get the pipe number for a port.
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number. 
 *  
 * @return PIPE_NUM.
 *  
 * @sa None. 
 * 
 * @attention None.
 */
uint32 HAL_sb_PgdPortGetPipeNum(HAL_sb_HandleType hHandle, 
                                uint32 uPgdIndex, 
                                uint32 uPortNum);


/**
 * Get the number of words currently present in the FIFO. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number. 
 *  
 * @return FIFO_WORD_CNT.
 *  
 * @sa None. 
 * 
 * @attention This is the dynamic value of the count which may 
 *            change.
 */
uint32 HAL_sb_PgdPortGetCurrentFifoWordCount(HAL_sb_HandleType hHandle, 
                                             uint32 uPgdIndex, 
                                             uint32 uPortNum);


/**
 * Check if the FIFO contains "watermark" worth of data for a 
 * port when acting a source to a data channel. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number. 
 *  
 * @return TRUE, if watermark is hit, FALSE otherwise.
 *  
 * @sa None. 
 * 
 * @attention None.
 */
bool32 HAL_sb_PgdPortIsWatermarkHit(HAL_sb_HandleType hHandle, 
                                    uint32 uPgdIndex, 
                                    uint32 uPortNum);


/**
 * Check if the Port disconnect event has happened for a port.
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number. 
 *  
 * @return TRUE, if Port disconnect event has happened, FALSE
 *         otherwise.
 *  
 * @sa None. 
 * 
 * @attention None.
 */
bool32 HAL_sb_PgdPortIsDisconnectEventSet(HAL_sb_HandleType hHandle, 
                                          uint32 uPgdIndex, 
                                          uint32 uPortNum);


/**
 * Check if the Port DMA event has happened for a port.
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number. 
 *  
 * @return TRUE, if Port DMA event has happened, FALSE
 *         otherwise.
 *  
 * @sa None. 
 * 
 * @attention None.
 */
bool32 HAL_sb_PgdPortIsDmaEventSet(HAL_sb_HandleType hHandle, 
                                   uint32 uPgdIndex, 
                                   uint32 uPortNum);

/**
 * Check the events that are asserted for a port. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number. 
 *  
 * @return Enumeration representing the asserted events for the 
 *         port.
 *  
 * @sa None. 
 * 
 * @attention None.
 */
HAL_sb_PgdPortEventType HAL_sb_PgdPortGetEvents(HAL_sb_HandleType hHandle,
                                                uint32 uPgdIndex,
                                                uint32 uPortNum);

/**
 * Check if the channel is active for a port. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number. 
 *  
 * @return TRUE, if channel is active, FALSE otherwise.
 *  
 * @sa None. 
 * 
 * @attention None.
 */
bool32 HAL_sb_PgdPortIsChannelActive(HAL_sb_HandleType hHandle, 
                                     uint32 uPgdIndex, 
                                     uint32 uPortNum);


/**
 * Get the channel number for a Port. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number. 
 *  
 * @return Channel Number.
 *  
 * @sa None. 
 * 
 * @attention None.
 */
uint32 HAL_sb_PgdPortGetChannelNumber(HAL_sb_HandleType hHandle, 
                                      uint32 uPgdIndex, 
                                      uint32 uPortNum);


/**
 * Get the channel Segment distribution value for a Port . 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number. 
 *  
 * @return Segment distribution value.
 *  
 * @sa None. 
 * 
 * @attention None.
 */
uint32 HAL_sb_PgdPortGetSegDistribution(HAL_sb_HandleType hHandle, 
                                        uint32 uPgdIndex, 
                                        uint32 uPortNum);


/**
 * Get the Channel Transport Protocol for a Port. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number. 
 *  
 * @return Channel Transport Protocol.
 *  
 * @sa None. 
 * 
 * @attention None.
 */
HAL_sb_TransportProtocolType HAL_sb_PgdPortGetTransportProtocol(
                                   HAL_sb_HandleType hHandle, 
                                   uint32 uPgdIndex, 
                                   uint32 uPortNum);


/**
 * Get the Channel Segment Length for a Port .
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number. 
 *  
 * @return Segment Length.
 *  
 * @sa None. 
 * 
 * @attention None.
 */
uint32 HAL_sb_PgdPortGetSegLen(HAL_sb_HandleType hHandle, 
                               uint32 uPgdIndex, 
                               uint32 uPortNum);


/**
 * Set the Block Size for a Port .
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number. 
 *  
 * @param[in] uBlkSize -  Block size in units of bytes. 
 *   
 * @return None.
 *  
 * @sa HAL_sb_PgdPortGetBlkSize. 
 * 
 * @attention None.
 */
void HAL_sb_PgdPortSetBlkSize(HAL_sb_HandleType hHandle, 
                              uint32 uPgdIndex, 
                              uint32 uPortNum, 
                              uint32 uBlkSize);


/**
 * Get the Block Size for a Port .
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number. 
 *  
 * @return Block size in units of bytes..
 *  
 * @sa HAL_sb_PgdPortSetBlkSize. 
 * 
 * @attention None.
 */
uint32 HAL_sb_PgdPortGetBlkSize(HAL_sb_HandleType hHandle, 
                                uint32 uPgdIndex, 
                                uint32 uPortNum);


/**
 * Set the Transaction Size for a Port .
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number. 
 *  
 * @param[in] uTransSize -  Transaction size in units of bytes.
 *   
 * @return None.
 *  
 * @sa HAL_sb_PgdPortGetTransSize. 
 * 
 * @attention None.
 */
void HAL_sb_PgdPortSetTransSize(HAL_sb_HandleType hHandle, 
                                uint32 uPgdIndex, 
                                uint32 uPortNum, 
                                uint32 uTransSize);


/**
 * Get the Transaction Size for a Port .
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number. 
 *  
 * @return Transaction size in units of bytes.
 *  
 * @sa HAL_sb_PgdPortSetTransSize. 
 * 
 * @attention None.
 */
uint32 HAL_sb_PgdPortGetTransSize(HAL_sb_HandleType hHandle, 
                                  uint32 uPgdIndex, 
                                  uint32 uPortNum);


/**
 * Set Multichannel Ports.
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortBitField -  If bit n of this parameter is set, then 
 *       nth port channel is part of a multichannel group. For
 *       eg, if ports 0 to 5 participate in a 5:1 audio stream,
 *       then this parameter should be set to 0x3F
 *  
 * @return None.
 *  
 * @sa HAL_sb_PgdPortGettMultiChanl. 
 * 
 * @attention None.
 */
void HAL_sb_PgdPortSetMultiChanl(HAL_sb_HandleType hHandle, 
                                 uint32 uPgdIndex, 
                                 uint32 uPortBitField);


/**
 * Get Multichannel Ports.
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  One of the Ports which is part of this
 *       queried multichannel group
 *  
 * @return Port Bit Field, If bit n of this return value is 
 *       set, then nth port channel is part of a multichannel
 *       group. For eg, if ports 0 to 5 participate in a 5:1
 *       audio stream, then this value will be set to 0x3F
 *  
 * @sa HAL_sb_PgdPortSetMultiChanl. 
 * 
 * @attention None.
 */
uint32 HAL_sb_PgdPortGetMultiChanl(HAL_sb_HandleType hHandle, 
                                   uint32 uPgdIndex, 
                                   uint32 uPortNum);


/**
 * Set repeat_period used by the push/pull Transport Protocol 
 * hardware. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number.
 *  
 * @param[in] uRepeatPeriod -  Repeat period in units of TBD
 * 
 * @return None
 *  
 * @sa HAL_sb_PgdPortGetRptPeriodForPushPull. 
 * 
 * @attention None.
 */
void HAL_sb_PgdPortSetRptPeriodForPushPull(HAL_sb_HandleType hHandle, 
                                           uint32 uPgdIndex, 
                                           uint32 uPortNum, 
                                           uint32 uRepeatPeriod);


/**
 * Get repeat_period used by the push/pull Transport Protocol 
 * hardware. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum - Port number.
 *  
 * @return Repeat period in units of TBD.
 *  
 * @sa HAL_sb_PgdPortSetRptPeriodForPushPull. 
 * 
 * @attention None.
 */
uint32 HAL_sb_PgdPortGetRptPeriodForPushPull(HAL_sb_HandleType hHandle, 
                                             uint32 uPgdIndex, 
                                             uint32 uPortNum);


/**
 * Set num_samples used by the push/pull Transport Protocol 
 * hardware. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number.
 *  
 * @param[in] uNumSamples -  Number of samples
 * 
 * @return None
 *  
 * @sa HAL_sb_PgdPortGetNumSamplesForPushPull. 
 * 
 * @attention None.
 */
void HAL_sb_PgdPortSetNumSamplesForPushPull(HAL_sb_HandleType hHandle, 
                                            uint32 uPgdIndex, 
                                            uint32 uPortNum, 
                                            uint32 uNumSamples);


/**
 * Get num_samples used by the push/pull Transport Protocol 
 * hardware. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPortNum -  Port number.
 *  
 * @return Number of samples.
 *  
 * @sa HAL_sb_PgdPortSetNumSamplesForPushPull. 
 * 
 * @attention None.
 */
uint32 HAL_sb_PgdPortGetNumSamplesForPushPull(HAL_sb_HandleType hHandle, 
                                              uint32 uPgdIndex, 
                                              uint32 uPortNum);


/**
 * Set Progress counter initial Value to be loaded. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPcNum -  Progress counter index.
 *  
 * @param[in] uInitVal -  PC init value.
 *  
 * @return None.
 *  
 * @sa HAL_sb_PgdPcGetInitVal, HAL_sb_PgdPcLoadInitVal. 
 * 
 * @attention This initial value will be loaded in PC only after
 *            calling HAL_sb_PgdPcLoadInitVal() api.
 */
void HAL_sb_PgdPcSetInitVal(HAL_sb_HandleType hHandle, 
                            uint32 uPgdIndex, 
                            uint32 uPcNum, 
                            uint32 uInitVal);


/**
 * Get Progress counter initial Value to be loaded. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPcNum -  Progress counter index.
 *  
 * @return PC init value.
 *  
 * @sa HAL_sb_PgdPcSetInitVal, HAL_sb_PgdPcLoadInitVal. 
 * 
 * @attention This initial value will be loaded in PC only after
 *            calling HAL_sb_PgdPcLoadInitVal() api.
 */
uint32 HAL_sb_PgdPcGetInitVal(HAL_sb_HandleType hHandle, 
                              uint32 uPgdIndex, 
                              uint32 uPcNum);


/**
 * Set the Progress counter association with a port. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPcNum -  Progress counter index. 
 *  
 * @param[in] uPortNum -  Port number to associate PC with. 
 *  
 * @return None.
 *  
 * @sa HAL_sb_PgdPcGetPortNum. 
 * 
 * @attention None.
 */
void HAL_sb_PgdPcSetPortNum(HAL_sb_HandleType hHandle, 
                            uint32 uPgdIndex, 
                            uint32 uPcNum, 
                            uint32 uPortNum);

/**
 * Get the Progress counter association with a port. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPcNum -  Progress counter index.
 *  
 * @return Port number associated to a PC instance.
 *  
 * @sa HAL_sb_PgdPcSetPortNum. 
 * 
 * @attention None.
 */
uint32 HAL_sb_PgdPcGetPortNum(HAL_sb_HandleType hHandle, 
                              uint32 uPgdIndex, 
                              uint32 uPcNum);


/**
 * Load the Init value to the Progress Counter now. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPcNum -  Progress counter index.
 *  
 * @return None.
 *  
 * @sa HAL_sb_PgdPcSetInitVal,HAL_sb_PgdPcGetInitVal. 
 * 
 * @attention None.
 */
void HAL_sb_PgdPcLoadInitVal(HAL_sb_HandleType hHandle, 
                             uint32 uPgdIndex, 
                             uint32 uPcNum);


/**
 * Set the Init value of the Progress Counter and load the 
 * Progress counter now. This is combination of 
 * HAL_sb_PgdPcSetInitVal and HAL_sb_PgdPcLoadInitVal apis.
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPcNum -  Progress counter index. 
 *  
 * @param[in] uInitVal -  PC init value. 
 *  
 * @return None.
 *  
 * @sa HAL_sb_PgdPcSetInitVal,HAL_sb_PgdPcGetInitVal, HAL_sb_PgdPcLoadInitVal. 
 * 
 * @attention None.
 */
void HAL_sb_PgdPcSetAndLoadInitVal(HAL_sb_HandleType hHandle, 
                                   uint32 uPgdIndex, 
                                   uint32 uPcNum, 
                                   uint32 uInitVal);


/**
 * Get the Progress counter value 
 * Counting number of words DMA-ed to (if src) or from (if sink) from the 
 * associated port. 
 * And number of samples in the FIFO.  
 *
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPcNum -  Progress counter index.
 *  
 * @param[out] puWordsDMAed -  Number of words DMA-ed to (if src) or
 * from (if sink) from the associated port
 *  
 * @param[out] puNumSamplesinFifo -  Number of samples in the FIFO.  
 *  
 * @return None.
 *  
 * @sa None. 
 * 
 * @attention None.
 */
void HAL_sb_PgdPcGetVal(HAL_sb_HandleType hHandle, 
                        uint32 uPgdIndex, 
                        uint32 uPcNum,
                        uint32 *puWordsDMAed,
                        uint32 *puNumSamplesinFifo);


/**
 * Get the shadow progress counter value, counting number of 
 * words DMA-ed to (if src) or from (if sink) from the 
 * associated port, and number of samples in the FIFO. 
 *
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPcNum -  Progress counter index.
 *  
 * @param[out] puWordsDMAed -  Number of words DMA-ed to (if src) or
 * from (if sink) from the associated port
 *  
 * @param[out] puNumSamplesinFifo -  Number of samples in the FIFO.  
 *  
 * @return None.
 *  
 * @sa None. 
 * 
 * @attention None.
 */
void HAL_sb_PgdPcGetShadowVal(HAL_sb_HandleType hHandle, 
                              uint32 uPgdIndex, 
                              uint32 uPcNum,
                              uint32 *puWordsDMAed,
                              uint32 *puNumSamplesinFifo);


/**
 * Get the Progress counter value when the VFR TS interrupt 
 * occured. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPcNum -  Progress counter index.
 *  
 * @param[out] puWordsDMAed -  Number of words DMA-ed to (if src) or 
 * from (if sink) from the associated port
 *  
 * @param[out] puNumSamplesinFifo -  Number of samples in the FIFO.  
 *  
 * @return None.
 *  
 * @sa None. 
 * 
 * @attention None.
 */
void HAL_sb_PgdPcGetValOnVfrTs(HAL_sb_HandleType hHandle, 
                                 uint32 uPgdIndex, 
                                 uint32 uPcNum,
                                 uint32 *puWordsDMAed,
                                 uint32 *puNumSamplesinFifo);


/**
 * Check if VFR TS Interrupt is set for a PC. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPcNum -  Progress counter index.
 *  
 * @return TRUE, if the VFR TS interrupt is set, FALSE 
 *         otherwise.
 *  
 * @sa HAL_sb_PgdPcClearVfrInt. 
 * 
 * @attention None.
 */
bool32 HAL_sb_PgdPcIsVfrIntSet(HAL_sb_HandleType hHandle, 
                               uint32 uPgdIndex, 
                               uint32 uPcNum);


/**
 * Clear VFR TS Interrupt if set for a PC. 
 *  
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPcNum -  Progress counter index.
 *  
 * @return None.
 *  
 * @sa HAL_sb_PgdPcIsVfrIntSet. 
 * 
 * @attention None.
 */
void HAL_sb_PgdPcClearVfrInt(HAL_sb_HandleType hHandle, 
                             uint32 uPgdIndex, 
                             uint32 uPcNum);


/**
 * Get the progress counter timestamp value corresponding to the
 * last DMA interrupt, as well as indication if the timestamp is
 * valid or if samples were missed.
 *
 * @param[in] hHandle - This SlimBus hHandle. 
 * 
 * @param[in] uPgdIndex - Ported Generic Device Index. 
 *  
 * @param[in] uPcNum -  Progress counter index.
 *  
 * @param[out] puTimestamp -  Timetstamp value at the last 
 * DMA interrupt
 *  
 * @param[out] pbTimestampValid -  If the timestamp is valid or 
 *       not.
 *  
 * @param[out] pbSampleMissed -  If there was a timestamp sample
 *       that was missed.
 *  
 * @return None.
 *  
 * @sa None. 
 * 
 * @attention None.
 */
void HAL_sb_PgdPcGetTimestamp(HAL_sb_HandleType hHandle, 
                              uint32 uPgdIndex, 
                              uint32 uPcNum,
                              uint64 *puTimestamp,
                              bool32 *pbTimestampValid,
                              bool32 *pbSampleMissed);


/** @} */

#endif /* HALSB_H */
