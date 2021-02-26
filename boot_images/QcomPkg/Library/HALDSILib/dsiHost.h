#ifndef DSIHOST_H
#define DSIHOST_H
/*=============================================================================

  File: dsiHost.h
  

  Copyright (c) 2010-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

/*============================================================================
*                         INCLUDE FILES
============================================================================*/
#include "HALdsi.h"

/* --- DO NOT ADD HEADER FILES HERE! --- */

/* ------------------------------------------------------------------------
Enumerated types
---------------------------------------------------------------------------*/
/*!
 * \b DSI_StatusType
 *
 *   Define the return error code for DSI functions.  Each status type has a
 *   null terminated error string which can be accessed using
 *   dsi_getStatusText() useful for printing error messages.
 */
typedef enum
{
  DSI_STATUS_SUCCESS = 0,
  DSI_STATUS_FAILED_NOT_SUPPORTED,
  DSI_STATUS_FAILED_DRIVER_NOT_INITIALIZED,
  DSI_STATUS_FAILED_DRIVER_ALREADY_INITIALIZED,
  DSI_STATUS_FAILED_EXPECTED_NON_NULL_PTR,
  DSI_STATUS_FAILED_NO_SUPPORT_FOR_CHANNEL,
  DSI_STATUS_FAILED_UNABLE_TO_RECOGNIZE_HW,
  DSI_STATUS_FAILED_LCD_HW_ERROR,
  DSI_STATUS_FAILED_DISPLAY_CONTROL_IN_USE,
  DSI_STATUS_FAILED_DEVICE_IN_USE,
  DSI_STATUS_FAILED_RESOURCE_NOT_READY,
  DSI_STATUS_FAILED_RESOURCE_OPEN_FAIL,
  DSI_STATUS_FAILED_UNABLE_TO_READ_REGISTRY,
  DSI_STATUS_FAILED_TIMEOUT,
  DSI_STATUS_FAILED_BUFFER_TOO_SMALL,
  DSI_STATUS_FAILED_INVALID_PACKET,
  DSI_STATUS_FAILED, 
  DSI_STATUS_MAX,
  DSI_STATUS_FORCE_32BIT = 0x7FFFFFFF
} DSI_StatusType;


/*!
 * \b DSI_DisplayPropertyType
 *
 * Defines list of DSI Host properties that can be modified.
 */
typedef enum
{
  DSI_DISPLAY_PROPERTYID_NONE = 0,
  DSI_DISPLAY_PROPERTYID_DISPINFO,                      /**< DSI display basic info                           */
  DSI_DISPLAY_PROPERTYID_LP11,                          /**< Stop transmission, but controller back into LP11 */
  DSI_DISPLAY_PROPERTYID_CALCREFRESH,                   /**< Calculate the refresh rate based on the input parameters */
  DSI_DISPLAY_PROPERTYID_HOSTERROR,                     /**< DSI host error */
  DSI_DISPLAY_PROPERTYID_PANELERROR,                    /**< DSI panel error */
  DSI_DISPLAY_PROPERTYID_IDLE,                          /**< Disable/Turn off DSI PLL clocks in idle scenario */
  DSI_DISPLAY_PROPERTYID_ACTIVE,                        /**< Enable/Turn on DSI PLL clocks */
  DSI_DISPLAY_PROPERTYID_INTSTATUS,                     /**< Return list of active interrupt for the DSI core */
  DSI_DISPLAY_PROPERTYID_ENABLEDINTSTATUS,              /**< Return list of active enabled interrupt for the DSI core */
  DSI_DISPLAY_PROPERTYID_INTENABLE,                     /**< Enable specific DSI interrupts */
  DSI_DISPLAY_PROPERTYID_INTDISABLE,                    /**< Disable specific DSI interrupts */
  DSI_DISPLAY_PROPERTYID_INTCLEAR,                      /**< Clear specific DSI interrupts */  
  DSI_DISPLAY_PROPERTYID_RESETCORE,                     /**< Software reset of a DSI core */
  DSI_DISPLAY_PROPERTYID_LANESTATE,                     /**< Configure the lane state (Clock & data lanes) */
  DSI_DISPLAY_PROPERTYID_CONTROLLERSTATUS,              /**< Reports the controller hardware status */
  DSI_DISPLAY_PROPERTYID_ENABLECORE,                    /**< Enable/Disable the HW engine in each core */
  DSI_DISPLAY_PROPERTYID_ULPSCLAMP,                     /**< DEPRECATED: DSI lane ULPS clamp control */
  DSI_DISPLAY_PROPERTYID_REFRESHRATE,                   /**< Refresh rate to set*/
  DSI_DISPLAY_PROPERTYID_LANESTATUS,                    /**< Reports the lane status */
  DSI_DISPLAY_PROPERTYID_PLL_CTRL,                      /**< Enable/disable DSI PLL  */
  DSI_DISPLAY_PROPERTYID_LANEMAP,                       /**< DSI lane map */
  DSI_DISPLAY_PROPERTYID_SETSCRATCH1,                   /**< Set dsi scratch register-1 */
  DSI_DISPLAY_PROPERTYID_GETCALCODES,                   /**< Get dynamic refresh calibration codes */
  DSI_DISPLAY_PROPERTYID_MAX,
  DSI_DISPLAY_PROPERTYID_FORCE_32BIT = 0x7FFFFFFF
}DSI_DisplayPropertyType;



/*!
 * \b DSI_DisplayWriteFlags
 *
 * Defines list of DSI Write flag bits
 */
typedef enum
{
  DSI_DISPLAY_WRITEFLAG_NORETRY      = 0x00000001,
  DSI_DISPLAY_WRITEFLAG_FORCEHS      = 0x00000002,
  DSI_DISPLAY_WRITEFLAG_FORCELP      = 0x00000004,  
  DSI_DISPLAY_WRITEFLAG_FORCE_32BIT  = 0x7FFFFFFF
}DSI_DisplayWriteFlags;



/*!
 * \b DSI_InterruptType
 *
 * Define the type of interrupt to enable/clear/decode
 */
typedef enum
{
  DSI_INTERRUPT_NONE                = 0x00000000,
  DSI_INTERRUPT_CMDMODE_DMA_DONE    = 0x00000001,
  DSI_INTERRUPT_CMDMODE_MDP_DONE    = 0x00000002,
  DSI_INTERRUPT_VIDEOMODE_DONE      = 0x00000004, 
  DSI_INTERRUPT_DSI_ERROR           = 0x00000008, 
  DSI_INTERRUPT_BTA_DONE            = 0x00000010,
  DSI_INTERRUPT_ALL                 = (DSI_INTERRUPT_CMDMODE_DMA_DONE|DSI_INTERRUPT_CMDMODE_MDP_DONE|
                                       DSI_INTERRUPT_VIDEOMODE_DONE|DSI_INTERRUPT_DSI_ERROR|DSI_INTERRUPT_BTA_DONE),
  DSI_INTERRUPT_FORCE_32BIT         = 0x7FFFFFFF
} DSI_InterruptType;



/*!
 * \b DSI_ControllerStatusType
 *
 * Define the type of interrupt to enable/clear/decode
 */
typedef enum
{
  DSI_CONTROLLER_STATUS_IDLE                = 0x00000000,    /**< All cores are idle */
  DSI_CONTROLLER_STATUS_CMD_ENGINE_BUSY     = 0x00000001,    /**< Command mode engine busy */
  DSI_CONTROLLER_STATUS_VIDEO_ENGINE_BUSY   = 0x00000002,    /**< Video mode engine busy */
  DSI_CONTROLLER_STATUS_CMD_MDP_ENGINE_BUSY = 0x00000004,    /**< Command mode pixel engine busy */  
  DSI_CONTROLLER_STATUS_DMA_ENGINE_BUSY     = 0x00000010,    /**< Packet command engine busy */
  DSI_CONTROLLER_STATUS_BTA_BUSY            = 0x00000020,    /**< BTA Read is busy */
  DSI_CONTROLLER_STATUS_FORCE_32BIT         = 0x7FFFFFFF
} DSI_ControllerStatusType;

/*!
 * \b DSI_LaneStateType
 *
 * Define the DSI lane state
 */
typedef enum
{
    DSI_LANE_STATE_INVALID,                         /**< Invalid state */
    DSI_LANE_STATE_LP00,                            /**< DSI lane in LP00 state, enter ULPS */
    DSI_LANE_STATE_LP11,                            /**< DSI lane in LP11 state, exit ULPS  */
    DSI_LANE_STATE_CLOCK_HS,						/**< Force clock lane to HS */	  
    DSI_LANE_STATE_CLOCK_LP11,						/**< Force clock lane to idle */
    DSI_LANE_STATE_FORCE_32BIT  = 0x7FFFFFFF
} DSI_LaneStateType;

/*!
 * \b DSI_PllConfigSourceType
 *
 * Define DSI PLL source 
 */
typedef enum
{
    DSI_PLL_CONFIG_INDEPENDENT_SOURCE,              /**< PLL0 and PLL1 run independently         */
    DSI_PLL_CONFIG_SPLIT_SOURCE,                    /**< PLL0 is used to drive DSI0 and DSI1     */
    DSI_PLL_CONFIG_EXTERNAL_SOURCE,                 /**< PLLs are used to drive external device  */
} DSI_PllConfigSourceType;


/*!
* \b DSI_Device_FlagsType
*
* Defines list of flags used for DSI device operation.
*/
typedef enum
{
  DSI_DEVICE_FLAGS_NONE                                       = 0x00000000,
  
  DSI_DEVICE_FLAGS_LOAD_DRIVER_STATE                          = 0x00000001,
  /* Driver flag used during DSI_Device_Open to indicate that only the driver state should
  * be loaded and hardware should not be modified.  This flag is used for seamless transition from UEFI to
  * the KMD without hardware glitch. */

  DSI_DEVICE_FLAGS_DSI_DCS_DMA_ONLY                           = 0x00000002,
  /* DSI command data will be fetched via DMA VBIF interface only from memory. */

} DSI_Device_FlagsType;


/*!
* \b DSI_Display_FlagsType
*
* Defines list of flags used for DSI display operation.
*/
typedef enum
{
  DSI_DISPLAY_FLAGS_NONE                                      = 0x00000000,
  
  DSI_DISPLAY_FLAGS_LOAD_DRIVER_STATE                         = 0x00000001,
  /* Driver flag used during DSI_Display_Open to indicate that only the driver state should
  * be loaded and hardware should not be modified.  This flag is used for seamless transition from UEFI to
  * the KMD without hardware glitch. */

} DSI_Display_FlagsType;

/* -----------------------------------------------------------------------
** Macros and Defines
-------------------------------------------------------------------------- */


/* Maximum number of retries for DSI write commands
 */
#define DSI_WRITE_RETRY_COUNT                     2


/* Flags used for DSI_Display_ClientWrite/DSI_Display_ClientRead
 */
#define DSI_CMD_FLAG_CMD_DMA_INTERLEAVED          0x00000001
#define DSI_CMD_FLAG_CMD_ASYNC                    0x00000002

/* 100 us polling interval */
#define DSI_POLLING_CYCLE                         100          

/* 16.67 ms maximum timeout for read/write */
#define DSI_MAX_POLLING_TIME                      16670

/* 1500 us wake wakeup time from ULPS */
#define DSI_LANE_T_WAKEUP_TIME                    1500

/* DSI host DMA buffer size
 */
#define DSI_DMA_BUFFER_SIZE                       1024


/*!
 * \b DSI_HandleType
 *
 *  Handle to DSI device.
 */
typedef void                                *DSI_HandleType; 


/*!
 * \b DSI_MemoryConfigType
 *
 * Config parameters for DSI Host DMA buffer allocation, memory allocation for DSI DMA command transfers
 */
typedef struct
{
    void                 *pCPUAddress;              /**< CPU Address */
    MDP_AddressType      sMDPAddress;               /**< MDP virtual address */
    MDP_AddressType      sPhysicalAddress;          /**< Physical address allocation */
    uint32               uSize;                     /**< Size in bytes of the allocation */
} DSI_MemoryConfigType;

/*!
 * \b DSI_DeviceConfigType
 *
 * Config parameters for DSI Host Core initialization.
 */
typedef struct
{
  DSI_Device_IDType         eDeviceID;                  /**< Logic ID of Host Device                              */
  bool32                    bDualDsi;                   /**< DSI split display                                    */
  bool32                    bLittleEndian;              /**< Endian of attached panels                            */
  bool32                    bNonCompliantMode;          /**< Panels are non-compliant?                            */
  bool32                    bEnableCRCCheck;            /**< Panels support CRC?                                  */
  bool32                    bEnableECCCheck;            /**< Panels support ECC?                                  */
  bool32                    bEnableLP11AtInit;          /**< Put the line state in to LP11 after init             */
  DSI_MemoryConfigType      sMemoryConfig;              /**< Allocation buffer for DSI DMA information            */
  uint32                    uFlags;                     /**< Device flags   (See DSI_Device_FlagsType).           */
  bool32                    bPhyDCDCMode;               /**< DSI regulator Mode TRUE=DCDC ,FALSE=LDO(Default mode)*/
  uint32                    uClockStrengthHS;           /**< Clock lane strength in HS                            */
  uint32                    uDataStrengthHS;            /**< Data lane strength in HS                             */
  uint32                    uDataStrengthLP;            /**< Data lane strength in LP                             */
  bool32                    bCPhyMode;
} DSI_DeviceConfigType;

/*!
 * \b DSI_ExtTEConfigType
 *
 * Config parameters for Command Mode External TE Signals
 */
typedef struct
{
  DSI_ExtTEModeType     eExtTEMode;
  union 
  {
    bool32              bVsyncFallingEdge;      /**< For DSI_ExtTEMode_VS_EDGE            */
    uint32              iTEVsyncPulseWidth_Min; /**< For DSI_ExtTEMode_VS_WIDTH           */
    struct
    {
      bool32            bTEVsHsFallingEdge;     /**< Polarity of active edge,             */
      uint32            iTEHsyncTotal;          /**< Total # of HSYNC per frame           */
      uint32            iTEHSyncCount;          /**< Which HSYNC should trigger a engine  */
    }TE_VsHsEdge;       /**< For DSI_ExtTEMode_VSHS_EDGE                                  */
    struct
    {
      uint32            iTEVsyncPulseWidth_Min; /**< Min Pulse width of VSync             */
      uint32            iTEHsyncPulseWidth_Max; /**< Max Pulse width of HSync             */
      uint32            iTEHsyncTotal;          /**< Total # of HSYNC per frame           */
      uint32            iTEHSyncCount;          /**< Which HSYNC should trigger a engine  */
    }TE_VsHPulse;       /**< For DSI_ExtTEMode_VSHS_WIDTH                                 */
  }sTEConfig;
}DSI_ExtTEConfigType;

/*!
 * \b DSI_CmdModeConfigType
 *
 * Config parameters for Command mode DSI client Panel initialization
 */
typedef struct
{
  bool32                        bEnable;
  DSI_ImageTriggerType          eImageTrigger;          /**< Display Engine(MDP) Trigger          */
  DSI_CMDTriggerType            eCMDTrigger;            /**< DMA/Memory Trigger                   */
  DSI_ExtTEConfigType           sExtTEConfig;           /**< Signal property of Ext TE pin        */
  uint32                        uIdleTime;              /**< idle time to be inserted between MDP packets */
  DSI_DscCmdModeConfigType      sDSCCmdConfig;   /**< DSC compression configuration*/
}DSI_CmdModeConfigType;



/*!
 * \b DSI_VideoConfigType
 *
 * Config parameters for Video mode DSI client Panel initialization
 */
typedef struct
{
  bool32                        bEnable;
  DSI_TrafficModeType           eVideoTraffic;          /**< Burst Mode...                        */
  DSI_VideoTimingType           sVideoTiming;           /**< Sync, pixel timing counter           */
  DSI_VPowerOptionType          sPowerOptions;          /**< Power mode options at different period  */
  DSI_DscVideoModeConfigType    sDSCVideoConfig;   /**< DSC compression configuration        */
}DSI_VideoConfigType;

/*!
 * \b DSI_DisplayConfigType
 *
 *  Config parameters for DSI client Panel initialization
 */
typedef struct
{
  DSI_DisplayInfoType     sDisplayInfo;           /**< Basic Display Info                                 */
  DSI_LaneMapType         eDisplayLane[DSI_LaneMap_MAX-1]; /**<Lane mapping from core to phy              */
  DSI_CmdModeConfigType   sCmdModeConfg;          /**< Config for Command mode                            */
  DSI_VideoConfigType     sVideoModeConfg;        /**< Config for Video mode                              */
  DSI_TimingOverrideType  sTimingOverride;        /**< Timing override for DSI interface                  */ 
  uint32                  uFlags;                 /**< Display Config Flags  (See DSI_Display_FlagsType)  */
  DSI_PllConfigSourceType ePLLSourceType;         /**< DSI controller clock source type                   */
} DSI_DisplayConfigType;


/*!
 * \b DSI_WritePacketType
 *
 *  Packet info for writing data/cmd to panel. (Embedded mode only)
 */
typedef struct
{
  DSI_Display_VCType    eDisplayVC;             /**< Virtual Channel to be sent to        */
  bool32                bHighSpeedMode;         /**< Transmitting in HS mode?             */
  bool32                bQueuePacket;           /**< 0:last packet of this TX             */
  bool32                bBTARequest;            /**< 1:Need BTA(bQueueWait must=1)        */
  uint32                bPayloadLen;            /**< Packet Data size                     */
  uint8*                puPayload;              /**< pointer to Packet payload            */
  uint8                 uCmdDataType;           /**< Packet Data type                     */
  uint32                uTimeout;               /**< Timeout (in us) for DSI interrupt status    */
  uint32                uFlags;                 /**< Write flags                          */
  uint32                uHostError;             /**< DSI host error code                  */
}DSI_WritePacketType;

/*!
 * \b HAL_DSI_ReadPacketType
 *
 *  Read data from panel.
 *
 *  [IN]:       eDisplayVC
 *  [IN]:       bHighSpeedMode
 *  [IN]:       uReadCmdDT
 *  [IN]:       uReadParamInByte
 *  [IN]:       puReadParam
 *  [IN]:       uReadPacketBufferSize
 *  [IN/OUT]:   puReadPacket
 *  [OUT]:      uRetPacketByte
 *  [OUT]:      uTotalPacketWC
 *  [IN]:       uFlags
 *  [IN]:       uTimeout
 *  [OUT]:      uErrorCode
 */
typedef struct
{
  DSI_Display_VCType    eDisplayVC;             /**< Virtual Channel ID                   */
  bool32                bHighSpeedMode;         /**< Transmitting in HS mode?             */
  DSI_Datatype          uReadCmdDT;             /**< What kind of Read? Data type         */
  uint32                uReadParamInByte;       /**< Size of Read parameters in byte      */
  uint8*                puReadParam;            /**< Read parameters if any               */
  uint32                uReadPacketBufferSize;  /**< size of read buffer in byte          */
  uint8*                puReadPacket;           /**< Pointer to the returned value */
  uint32                uRetPacketByte;         /**< Actual Read Packet size              */
  uint32                uTotalPacketWC;         /**< Word-Count in the packet             */
  uint32                uTimeout;               /**< Timeout (in us) for DSI interrupt status    */
  uint32                uFlags;                 /**< Read flags                           */
  uint32                uHostError;             /**< DSI host error code                  */
  uint32                uPanelError;            /**< DSI panel error code                 */
}DSI_ReadPacketType;

/*!
 * \b DSI_PhySetupParamType
 *
 * Config parameters required for DSI phy setup.
 */

typedef struct  
{
   DSI_DisplayInfoType    sDisplayInfo;
   DSI_TimingOverrideType sTimingOverride;  
}DSI_PhySetupParamType;

/*!
 * \b DSI_CoreEnableType
 *
 * Enable/disable HW blocks in DSI Controller.
 */

typedef struct  
{
   bool32                 bEnable;
   HAL_DSI_HWBlockType    eHWBlock;  
}DSI_CoreEnableType;

/*!
 * \b DSI_PropertyParamsType
 *
 * Config parameters required for DSI set property.
 */
typedef union
{
  DSI_PhySetupParamType    sPhyParams;                /**< DSI_DISPLAY_PROPERTYID_ACTIVE            */
  DSI_DisplayInfoType      sDisplayInfo;              /**< DSI_DISPLAY_PROPERTYID_CALCREFRESH       */
  uint32                   uHostError;                /**< DSI_DISPLAY_PROPERTYID_HOSTERROR         */
  uint32                   uPanelError;               /**< DSI_DISPLAY_PROPERTYID_PANELERROR        */
  uint32                   uInterruptList;            /**< DSI_DISPLAY_PROPERTYID_INTSTATUS         */
  DSI_ControllerStatusType uControllerStatus;         /**< DSI_DISPLAY_PROPERTYID_CONTROLLERSTATUS  */
  DSI_LaneStateType        eLaneState;                /**< DSI_DISPLAY_PROPERTYID_LANESTATE         */
  DSI_CoreEnableType       sEnableCore;               /**< DSI_DISPLAY_PROPERTYID_ENABLECORE        */
  bool32                   bULPSClampEnable;          /**< DSI_DISPLAY_PROPERTYID_ULPSCLAMP         */
  bool32                   bEnablePLL;                /**< DSI_DISPLAY_PROPERTYID_PLL_CTRL          */
  DSI_LaneMapType          aLaneMap[DSI_LaneMap_MAX]; /**< DSI_DISPLAY_PROPERTYID_LANEMAP           */
  uint32                   uScratchReg;               /**< DSI_DISPLAY_PROPERTYID_SETSCRATCH        */
}DSI_PropertyParamsType;

/*------------------------------------------------------------------------
Function declarations
-------------------------------------------------------------------------- */

/****************************************************************************
*
** FUNCTION: DSI_Device_Open()
*/
/*!
* \brief
*   This function must be called before any other DSI host function called
*   which using the deivce specified by eDeviceID. The first call to this 
*   API will initialize all internal DSI device components, all subsequent
*   calls by the same user will fail. This API will return a valid device
*   handle    to the user if no errors are detected. 
*
* \param [in]  psDeviceConfig - Pointer to DSI Device configuration parameter.
* \param [out] phDevice       - Pointer of Logic handle of DSI Device..
* 
* \retval DSI_StatusType
*
****************************************************************************/
DSI_StatusType DSI_Device_Open(
    DSI_DeviceConfigType*   psDeviceConfig,
    DSI_HandleType*         phDevice);

/****************************************************************************
*
** FUNCTION: DSI_Device_Close
*/
/*!
* \brief
*   This API is called to de-initialize the corresponding resources that 
*   is initialized when DSI_Device_Open is called. It will return an error
*   if the device handle is invalid. All resources will be de-initialized
*   when the last DSI Display handle is released. User should ensure that
*   any display modules opened on this device should be closed prior to 
*   closing the device module, otherwise corresponding error will be returned
*   to the user. Upon an invalid device handle, this call will also return
*   an error. 

*
* \param [in] hDevice - Logic handle of DSI Device..
*
****************************************************************************/
DSI_StatusType DSI_Device_Close(
    DSI_HandleType        hDevice,
    uint32                uFlags);


/****************************************************************************
*
** FUNCTION: DSI_Display_Open
*/
/*!
* \brief
*   Initialize DSI Host which is panel specific. This API call initializes 
*   a QDI display module and must be called prior to controlling the display 
*   module. The parameters of this API should not change at run-time. However, 
*   on DSI1.0, if two different type I DSI panels are attached to the same 
*   lane, some parameters such as pixel format are different. Those registers 
*   need to be re-configured before the start of each virtual channel image 
*   transmission. Those reconfigurations are done with DSI Display property 
*   functions.
*
* \param [in]  hDevice        - Logic handle of DSI Device..
* \param [in]  psDisplayConfig- Pointer to DSI Display configuration parameter
* \param [out] phDisplay      - Pointer of Logic handles of DSI Display
*
****************************************************************************/
DSI_StatusType DSI_Display_Open(
    DSI_HandleType          hDevice,
    DSI_DisplayConfigType*  psDisplayConfig,
    DSI_HandleType*         phDisplay
    );
    
/****************************************************************************
*
** FUNCTION: DSI_Display_Close
*/
/*!
* \brief
*   This API is called to deinitialize all the corresponding display 
*   resources associated with the display module. Upon an invalid display 
*   handle, this call will also return an error. 
*
* \param [in] hDevice        - Logic handle of DSI Device.
*
****************************************************************************/
DSI_StatusType DSI_Display_Close(
    DSI_HandleType        hDisplay
    );
    
/****************************************************************************
*
** FUNCTION: DSI_Display_SetProperty
*/
/*!
* \brief
*   This API is called to deinitialize all the corresponding display 
*   resources associated with the display module. Upon an invalid display 
*   handle, this call will also return an error. 
*
* \param [in] hDevice       - Logic handle of DSI Device.
* \param [in] eProperty     - Property ID that requires update
* \param [in] pPropertyData - Contains the new parameters of the property 
*                             that will be applied.
****************************************************************************/
DSI_StatusType DSI_Display_SetProperty(
    DSI_HandleType          hDisplay,
    DSI_DisplayPropertyType eProperty,
    DSI_PropertyParamsType* pPropertyData
    );
/****************************************************************************
*
** FUNCTION: DSI_Display_GetProperty
*/
/*!
* \brief
*   This API allows the user to query the attributes and properties of the 
*   DSI Displays. 
*
* \param [in]  hDevice        - Logic handle of DSI Device.
* \param [in]  eProperty      - Property ID that requires return
* \param [out] pPropertyData  - Contains the new parameters of the property 
*                               that will be returned.
****************************************************************************/
DSI_StatusType DSI_Display_GetProperty(
    DSI_HandleType          hDisplay,
    DSI_DisplayPropertyType eProperty,
    DSI_PropertyParamsType* pPropertyData
    );
/****************************************************************************
*
** FUNCTION: DSI_Display_ClientWrite
*/
/*!
* \brief
*   This API allows users to send the DSI command out to DSI Panels. 
*
* \param [in] hDevice        - Logic handle of DSI Device.
* \param [in] pWritePacket   - Command package to be sent out to DSI panels
*
****************************************************************************/
DSI_StatusType DSI_Display_ClientWrite(
    DSI_HandleType          hDisplay,
    DSI_WritePacketType*    psWritePacket
    );
    
/****************************************************************************
*
** FUNCTION: DSI_Display_ClientRead
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
DSI_StatusType DSI_Display_ClientRead(
    DSI_HandleType          hDisplay,
    DSI_ReadPacketType*     psReadPacket
    );

/****************************************************************************
*
** FUNCTION: DSI_Display_Commit
*/
/*!
* \brief
*   This API allows users to triger the command DMA out by Software. 
*
* \param [in] hDevice        - Logic handle of DSI Display Device.
* \param [in] iFlag          - Flag for Commit function
*
****************************************************************************/
DSI_StatusType DSI_Display_Commit(
    DSI_HandleType          hDisplay,
    uint32                  iFlag
    );


#endif  /* #define DSIHOST_H */
