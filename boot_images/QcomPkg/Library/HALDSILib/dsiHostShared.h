#ifndef DSIHOSTSHARED_H
#define DSIHOSTSHARED_H
/*=============================================================================

  File: dsiHostShared.h
  

  Copyright (c) 2010-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */
#include "dsiHost.h"
#include "HALdsi.h"
#include "HALdsi_Phy.h"

/* --- DO NOT ADD HEADER FILES HERE! --- */

/* -----------------------------------------------------------------------
** Macros and Defines
** ----------------------------------------------------------------------- */


#define DEVICE_USER_CONTEXT_HANDLE_TYPE             0x1A1A0000  /* Device context signature */

#define DSIHOST_BIT(x)                              ((uint32)(1<<(x)))

#define dsiBsp_iMaxInterleave                       1
#define dsiBsp_iMDPDataType                         0x39
#define dsiBsp_DSI_uWriteTimeoutMs                  1000
#define dsiBsp_DSI_uReadTimeoutMs                   100
#define dsiBsp_DSI_uReadTOCount                     10
#define dsiBsp_DSI_uMinInitMaster                   2          /* Enforce a minimum 2ms LP11 hold time before the first packet, override with uInitMasterTime */   

#define DSI_ERROR_AND_EXIT(Qerr, QExit)                       \
  if ((Qerr) != DSI_STATUS_SUCCESS)                           \
  {                                                           \
    goto QExit;                                               \
  }

#define DSI_NULLPOINTER_AND_EXIT(QPointer, QStatus, QExit)    \
  if ((QPointer) == NULL)                                     \
  {                                                           \
    QStatus = DSI_STATUS_FAILED_EXPECTED_NON_NULL_PTR;        \
    goto QExit;                                               \
  }

#define DSI_CHECK_FLAG_SET(value, flag)         ((value & flag)? TRUE: FALSE)

/* DSI device dirty bits */
#define DSI_DIRTYBIT_DEVICE_CONFIG              DSIHOST_BIT(0)
#define DSI_DIRTYBIT_DEVICE_LANE                DSIHOST_BIT(1)

/* DSI display dirty bits */
#define DSI_DIRTYBIT_DISPLAY_INFO               DSIHOST_BIT(0)
#define DSI_DIRTYBIT_VIDEOMODE_TRAFFIC          DSIHOST_BIT(1)
#define DSI_DIRTYBIT_VIDEOMODE_TIMING           DSIHOST_BIT(2)
#define DSI_DIRTYBIT_VIDEOMODE_POWEROPTION      DSIHOST_BIT(3)
#define DSI_DIRTYBIT_CMDMODE_EXTTE              DSIHOST_BIT(4)
#define DSI_DIRTYBIT_CMDMODE_TRIGGER            DSIHOST_BIT(5)
#define DSI_DIRTYBIT_CMDMODE_MISC               DSIHOST_BIT(6)

/*!
 * \b DSI_DisplayCtxType
 *
 * User context attributes.
 */
typedef struct 
{
  bool32                        bInitialized;       /* Display initialized or not         */
  uint32                        iDispDirtyBits;     /* Display dirty bits                 */
  DSI_DisplayInfoType           sDisplayInfo;       /* Basic Display Info                 */
  DSI_TrafficModeType           eVideoTraffic;      /* Burst mode                         */
  DSI_VideoTimingType           sVideoTiming;       /* Sync, pixel timing counter         */ 
  DSI_VPowerOptionType          sPowerOption;       /* Power mode                         */
  uint32                        iMaxInterleave;     /* Max command packet interleave      */
  DSI_Datatype                  iMDPDataType;       /* DT for MDP Packet output           */ 
  DSI_ExtTEConfigType           sExtTEConfig;       /* Signal property of external TE pin */
  DSI_ImageTriggerType          eImageTrigger;      /* Display engine(MDP) Trigger        */
  DSI_CMDTriggerType            eCMDTrigger;        /* DMA/Memory Trigger                 */
  uint32                        uIdleTime;          /* Idle time to be inserted between command mode MDP packets */
  DSI_DscCompressionInfo        sDscCompressionInfo;/* Controller config for Dsc related info                    */
}DSI_DisplayCtxType;



/*!
 * \b DSI_UserCtxType
 *
 * User context attributes.
 */
typedef struct 
{
  uint32                iDeviceRef;                      /* Reference # of panel that powered on DSI   */
  uint32                iVideoModeRef;                   /* Reference # of Video mode panel attached   */
  uint32                iCmdModeRef;                     /* Reference # of command mode panel attached */
  uint32                uDirtyBits;                      /* Marker indicate which property is dirty    */  
  DSI_DeviceConfigType  sDevConfig;                      /* Internal mapping for dsi device config     */
  DSI_LaneMapType       eDevLane[DSI_LaneMap_MAX];       /* Mapping with dsi lane and phy lane         */
  DSI_DisplayCtxType    sDisplayCtx[DSI_ChannelID_MAX];
  DSI_HandleType        hCmdBuffer;
  uint8*                pCmdVirtualAddr;    
  uint32                pCurrentBufferSize;   
  uint32                iCmdPhyAddr;        
  uint32                uCmdBufferSize; 
  uint8                 uCmdFIFOBuffer[HAL_DSI_MAX_DMAFIFOSIZE_BYTE];
  bool32                bCPhyMode;
}DSI_UserCtxType;

/*!
 * \b DSI_DeviceDataType
 *
 * Structure capturing the logical device attributes.
 */
typedef struct 
{
  uint32                  iDrvInstanceRef;              /* Flag indicating if the Driver is initialized */
  DSI_UserCtxType         sDeviceCtx[DSI_DeviceID_MAX]; /* User context that is attached to this device */
}DSI_DeviceDataType;


/* Clock Control Entry Table
 * 
 * Defines the clock control settings for a MMSS CC block.
 * Used to configure Pre-Divider, M, N, D values. 
 *
 * Input Freq -> [Pre Divider] -> MND -> Output Frequency.
 *
 * Set M,N = 0 to bypass MND.
 *
 */
typedef struct 
{
  uint8   uSrc;  /* Clock source     */
  uint8   uDiv;  /* Predivider value */
  uint8   uM;    /* M divider value  */ 
  uint8   uN;    /* N divider value  */
  uint8   uD;    /* D divider value  */
} DSI_ClockControlEntry;


/* Clock divider Entry Table
 * 
 * Defines a byte, pixel and core clock set for a particular configuration.
 *
 */
typedef struct {
    DSI_ClockControlEntry uByteMND;
    DSI_ClockControlEntry uPixelMND;    
    DSI_ClockControlEntry uDSICoreMND;
} DSI_ClockDividerEntry;



/* Extern for DSI shared data */
extern DSI_DeviceDataType gDsiSharedData;

/* Access macros for device and display contexts */
#define DSIHOST_GET_USERCTX(_deviceId_)                   (&(gDsiSharedData.sDeviceCtx[((_deviceId_)<DSI_DeviceID_MAX)?(_deviceId_):DSI_DeviceID_0]))
#define DSIHOST_GET_DISPLAYCTX(_deviceId_,_displayId_)    (&(DSIHOST_GET_USERCTX(_deviceId_)->sDisplayCtx[((_displayId_)<DSI_ChannelID_MAX)?(_displayId_):DSI_ChannelID_NONE]))


/* -----------------------------------------------------------------------
**
** FUNCTION: dsiShared_DeviceId2Handle()
**
** DESCRIPTION:
**   Generate device handle from DevcieId
**
** INPUT:
**   eDeviceId: the input device id
**
** RETURN:
**   Device handle
** ----------------------------------------------------------------------- */
DSI_HandleType dsiShared_DeviceId2Handle( 
    DSI_Device_IDType    eDeviceId 
    );

/* -----------------------------------------------------------------------
**
** FUNCTION: dsiShared_DeviceHandle2Id()
**
** DESCRIPTION:
**   Query DeviceId according to the device Handle
**
** INPUT:
**   hDevice: Handel to the device 
**
** RETURN:
**   Device Id
** ----------------------------------------------------------------------- */
DSI_Device_IDType dsiShared_DeviceHandle2Id(
    DSI_HandleType    hDevice
    );

 /* -----------------------------------------------------------------------
**
** FUNCTION: dsiShared_DisplayID2Handle()
**
** DESCRIPTION:
**   TBD
**
** INPUT:
**   TBD
**
** ----------------------------------------------------------------------- */
DSI_HandleType dsiShared_DisplayID2Handle(
    DSI_HandleType      hDevice,
    DSI_Channel_IDType  eDisplayId
    );

/* -----------------------------------------------------------------------
**
** FUNCTION: dsiShared_InitDsiDriver()
**
** DESCRIPTION:
**   Set the shared variable bIsDsInitialized to signify the driver has
**   been initialized or has not been initialized.
**
** ----------------------------------------------------------------------- */
DSI_StatusType dsiShared_InitDsiDriver(DSI_DeviceConfigType*   psDeviceConfig);

/* -----------------------------------------------------------------------
**
** FUNCTION: dsiShared_DeInitDsiDriver()
**
** DESCRIPTION:
**    Bookkeeping the driver instance when new panel is detached, if it is 
**    the last instance, releasing the internal resources
**
** ----------------------------------------------------------------------- */
DSI_StatusType dsiShared_DeInitDsiDriver(void);

/* -----------------------------------------------------------------------
**
** FUNCTION: dsiShared_setDeviceConfig()
**
** DESCRIPTION:
**   Set the shared variable bIsDsInitialized to signify the driver has
**   been initialized or has not been initialized.
**
** INPUT:
**   psDeviceConfig - Input the new value of bIsDsInitialized.
**
** ----------------------------------------------------------------------- */
DSI_StatusType dsiShared_setDeviceConfig(
    DSI_DeviceConfigType*   psDeviceConfig
    );

/* -----------------------------------------------------------------------
**
** FUNCTION: dsiShared_ClrDeviceConfig()
**
** DESCRIPTION:
**   TBD
**
** INPUT:
**   TBD
**
** ----------------------------------------------------------------------- */
DSI_StatusType dsiShared_ClrDeviceConfig(
    DSI_HandleType   hDevice,
    uint32           uFlags
    );


/* -----------------------------------------------------------------------
**
** FUNCTION: dsiShared_setDisplayConfig()
**
** DESCRIPTION:
**   TBD
**
** INPUT:
**   TBD
**
** ----------------------------------------------------------------------- */
DSI_StatusType dsiShared_setDisplayConfig(
    DSI_HandleType          hDevice, 
    DSI_DisplayConfigType*  psDisplayConfig
    );

/* -----------------------------------------------------------------------
**
** FUNCTION: dsiShared_setDisplayConfig()
**
** DESCRIPTION:
**   TBD
**
** INPUT:
**   TBD
**
** ----------------------------------------------------------------------- */
DSI_StatusType dsiShared_ClrDisplayConfig(
    DSI_HandleType          hDisplay
    );


/* -----------------------------------------------------------------------
**
** FUNCTION: dsiShared_DisplayID2Handle()
**
** DESCRIPTION:
**   TBD
**
** INPUT:
**   TBD
**
** ----------------------------------------------------------------------- */
DSI_StatusType dsiShared_ClientWrite(
    DSI_HandleType          hDisplay,
    DSI_WritePacketType*    psWritePacket
    );

/****************************************************************************
*
** FUNCTION: dsiShared_ClientRead
*/
/*!
* \brief
*   This API allows users to send the DSI read command out to DSI Panels, 
*   and wait until it gets the data from panel. 
*
* \param [in] hDevice        - Logic handle of DSI Device.
* \param [in] pReadPacket    - Contains the data to be read out from panels
*
****************************************************************************/
DSI_StatusType dsiShared_ClientRead(
    DSI_HandleType          hDisplay,
    DSI_ReadPacketType*     psReadPacket
    );

/****************************************************************************
*
** FUNCTION: dsiShared_ClientPackData()
*/
/*!
* \brief
*    Pack the DSI send/read commands in the hardware.
*
* \param [in] hDisplay       - DSI display handle.
* \param [in] psWritePacket  - DSI write packet information (optional). 
* \param [in] psReadPacket   - DSI read packet information (optional). 
*
* \retval DSI_StatusType
*
****************************************************************************/
DSI_StatusType dsiShared_ClientPackData(
    DSI_HandleType         hDisplay,
    DSI_WritePacketType   *psWritePacket,
    DSI_ReadPacketType    *psReadPacket
    );
/*-----------------------------------------------------------------------

dsiShared_Comit

DESCRIPTION:
    This API allows users to send the DSI command out to DSI Panels. 

----------------------------------------------------------------------- */
DSI_StatusType dsiShared_Comit(
    DSI_HandleType   hDevice, 
    uint32           iFlags 
    );

/* -----------------------------------------------------------------------
**
** FUNCTION: dsiShared_DisplayID2Handle()
**
** DESCRIPTION:
**   TBD
**
** INPUT:
**   TBD
**
** ----------------------------------------------------------------------- */
DSI_StatusType dsiShared_Handle2DisplayID(DSI_HandleType       hDisplay,
                                         DSI_Device_IDType*   peDeviceId,
                                         DSI_Channel_IDType*  peDisplayId);


/* -----------------------------------------------------------------------
**
** FUNCTION: dsiShared_ColorFill()
**
** DESCRIPTION:
**   This API will configure MDP to output constant color to update screen
**
** INPUT:
**   uFilledColor       - color to fill the screen in RGB format
**
** ----------------------------------------------------------------------- */
DSI_StatusType dsiShared_ColorFill(uint32 uFilledColor);





/* -----------------------------------------------------------------------
**
** FUNCTION: dsiShared_GetLastHostErrorCode()
**
** DESCRIPTION:
**   This API will read the DSI host error code
**
** ----------------------------------------------------------------------- */
DSI_StatusType dsiShared_GetLastHostErrorCode(DSI_Device_IDType eDeviceId, uint32 *puHostErrorCode);



/* -----------------------------------------------------------------------
**
** FUNCTION: dsiShared_GetLastPanelErrorCode()
**
** DESCRIPTION:
**   This API will read the DSI panel error code
**
** ----------------------------------------------------------------------- */
DSI_StatusType dsiShared_GetLastPanelErrorCode(DSI_Device_IDType eDeviceId, uint32 *puPanelErrorCode);


/*-----------------------------------------------------------------------
 * dsiShared_GetBitsPerPixel
 *
 * DESCRIPTION:
 *  This is a internal function that returns Bits Per Pixel (BPP) for a particular color format.
----------------------------------------------------------------------- */
uint32 dsiShared_GetBitsPerPixel(DSI_ColorFormatType eColorFormat);


/****************************************************************************
*
** FUNCTION: dsiShared_FrequencyCalibration()
*/
/*!
* \brief
*     Do calibration to get PLL codes for each dynamic refresh rate
*
* \param [in] psDsiPhyConfig      - needed by HAL_DSI_PhyPllSetup,dynamic refresh 
                                    bit clk frequency will be passed in through 
                                    this parameter
* \param [in] psDisplayConfig     - Pass in parameter for setup DSI Phy timing
*
* \retval - success or fail reason
*
****************************************************************************/
DSI_StatusType dsiShared_FrequencyCalibration(HAL_DSI_PhyConfigType *psDsiPhyConfig,
                                         DSI_DisplayConfigType  *psDisplayConfig);


/****************************************************************************
*
** FUNCTION: dsiShared_CalculateDesiredBitClk()
*/
/*!
* \brief
*     Calculate the desired DSI PLL bit clock frequency in Hz.
*
* \param [in] pDSIConfig    - panel display settings;
* \param [in] uBitsPerPixel - number of bits per pixel;
*
* \retval - desired bit clock frequency in Hz
*
****************************************************************************/
uint32 dsiShared_CalculateDesiredBitClk( DSI_DisplayInfoType   *pDSIConfig, 
                                         uint32                 uBitsPerPixel );


/* -----------------------------------------------------------------------
**
** FUNCTION: dsiShared_HALInt2DSIInt()
**
** DESCRIPTION:
**   TBD
**
** INPUT:
**   TBD
**
** ----------------------------------------------------------------------- */
uint32 dsiShared_HALInt2DSIInt(uint32 uHALInterrupts);


/* -----------------------------------------------------------------------
**
** FUNCTION: dsiShared_DsiInt2HALInt()
**
** DESCRIPTION:
**   TBD
**
** INPUT:
**   TBD
**
** ----------------------------------------------------------------------- */
uint32 dsiShared_DsiInt2HALInt(uint32 uDSIInterrupts);


/* -----------------------------------------------------------------------
**
** FUNCTION: dsShared_GetPackDataSize()
**
** DESCRIPTION:
**   TBD
**
** INPUT:
**   TBD
**
** ----------------------------------------------------------------------- */
void dsShared_GetPackDataSize(DSI_Datatype        iCmdDataType,
                              uint32              iPayloadByteSize,
                              uint32*             piPackDataSize);


/* -----------------------------------------------------------------------
**
** FUNCTION: dsShared_PackData()
**
** DESCRIPTION:
**   This API Pack the Data payload for Embedded DMA out.
**
** INPUT:
**   TBD
**
** ----------------------------------------------------------------------- */
void dsShared_PackData(HAL_DSI_PackDataType   *psPackData);

/* -----------------------------------------------------------------------
**
** FUNCTION: dsiShared_CommitLaneConfig()
**
** DESCRIPTION:
**   This API commits the device lane configuration
**
** INPUT:
**   eDeviceID
**
** ----------------------------------------------------------------------- */
DSI_StatusType dsiShared_CommitLaneConfig(DSI_Device_IDType   eDeviceID);


/* -----------------------------------------------------------------------
**
** FUNCTION: dsiShared_setPhyParameters()
**
** DESCRIPTION:
**   Set up the HAL_DSI_PhyConfigType structure based on the device configuration
**
** INPUT:
**   psDeviceConfig - Input the new value of bIsDsInitialized.
**   psPhyConfig    - Output phy configuration
** 
** ----------------------------------------------------------------------- */
void dsiShared_setPhyParameters(DSI_DeviceConfigType  *psDeviceConfig, 
                                HAL_DSI_PhyConfigType *psPhyConfig);


#endif /* DSIHOSTSHARED_H */



