/*=============================================================================

  File: dsiHostShared.c
  

  Copyright (c) 2010-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

/*============================================================================
*                         INCLUDE FILES
============================================================================*/
#include "dsiHost.h"  
#include "dsiHostShared.h"
#include "dsiHostSystem.h"

/* --- DO NOT ADD HEADER FILES HERE! --- */

#ifdef __cplusplus
extern "C" {
#endif
/* -----------------------------------------------------------------------
** DSI Constants
** ----------------------------------------------------------------------- */

// Define the maximum/minimum allowed refresh rate (used primarily to range checking)
//As refresh rate parsed from ACPI is Q16.16 format, MAX/MIN refresh rate is also defined in same format
#define DSI_MAX_REFRESH_RATE       0x5F0000       //95fps
#define DSI_MIN_REFRESH_RATE       0x0F0000       //15fps



/* -----------------------------------------------------------------------
** DSI Shared data
** ----------------------------------------------------------------------- */

DSI_DeviceDataType gDsiSharedData;


/* -----------------------------------------------------------------------                                  
** Forward Declaration of Local Functions
** ----------------------------------------------------------------------- */
static DSI_StatusType dsiShared_CommitVideoModeConfig(DSI_Device_IDType   eDeviceID, 
                                                     DSI_Channel_IDType  eDisplayID  );
static DSI_StatusType dsiShared_CommitCmdModeConfig(  DSI_Device_IDType   eDeviceID, 
                                                     DSI_Channel_IDType  eDisplayID  );
static DSI_StatusType dsiShared_CommitExtTEConfig(    DSI_Device_IDType   eDeviceID, 
                                                     DSI_Channel_IDType  eDisplayID  );
static DSI_StatusType dsiShared_CommitTriggerConfig(  DSI_Device_IDType   eDeviceID, 
                                                     DSI_Channel_IDType  eDisplayID  );
static DSI_StatusType dsiShared_CommitDeviceConfig(   DSI_Device_IDType   eDeviceID   );
static DSI_StatusType dsiShared_WaitforInterrupt(DSI_Device_IDType      eDeviceId,
                                                 uint32                 uIntFlags,
                                                 uint32                 uWaitTimeUs);


/* -----------------------------------------------------------------------
**
** FUNCTION: dsiShared_DeviceId2Handle()
**
** DESCRIPTION:
**   Generate device handle from DevcieId
**
** ----------------------------------------------------------------------- */
DSI_HandleType dsiShared_DeviceId2Handle( DSI_Device_IDType   eDeviceId )
{
  return  (DSI_HandleType) ((UINTN)(DEVICE_USER_CONTEXT_HANDLE_TYPE | eDeviceId ));
}

/* -----------------------------------------------------------------------
**
** FUNCTION: dsiShared_DeviceHandle2Id()
**
** DESCRIPTION:
**   Query DeviceId according to the device Handle
**
** ----------------------------------------------------------------------- */
DSI_Device_IDType dsiShared_DeviceHandle2Id(DSI_HandleType    hDevice)
{
  return (DSI_Device_IDType)((UINTN)hDevice & ~DEVICE_USER_CONTEXT_HANDLE_TYPE);
}

/* -----------------------------------------------------------------------
**
** FUNCTION: dsiShared_DisplayID2Handle()
**
** DESCRIPTION:
**   Generate display handle from DevcieId and display Id
**
** INPUT:
**   TBD
**
** ----------------------------------------------------------------------- */
DSI_HandleType dsiShared_DisplayID2Handle(DSI_HandleType      hDevice,
                                          DSI_Channel_IDType  eDisplayId)
{
  DSI_Device_IDType   eDeviceId   = DSI_DeviceID_0;

  if(NULL == hDevice)
    return NULL;

  eDeviceId = dsiShared_DeviceHandle2Id(hDevice);
  return (DSI_HandleType) ((UINTN)(eDisplayId | eDeviceId << 16));
}
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
                                         DSI_Channel_IDType*  peDisplayId)
{
  DSI_StatusType       eStatus     = DSI_STATUS_SUCCESS;
  
  if(peDeviceId == NULL || peDisplayId == NULL)
  {
    eStatus       = DSI_STATUS_FAILED_EXPECTED_NON_NULL_PTR;
  }
  else
  {
    *peDisplayId  = (DSI_Channel_IDType)((UINTN)hDisplay&0xffff);
    *peDeviceId   = (DSI_Device_IDType)(((UINTN)hDisplay&0xffff0000)>>16);
  }

  return eStatus;
}


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
uint32 dsiShared_DsiInt2HALInt(uint32 uDSIInterrupts)
{
  uint32 uHALInts = 0;

  if (DSI_INTERRUPT_CMDMODE_DMA_DONE & uDSIInterrupts)
  {
    uHALInts |= HAL_DSI_INTERRUPT_CMDMODE_DMA_DONE;
  }
  if (DSI_INTERRUPT_CMDMODE_MDP_DONE & uDSIInterrupts)
  {
    uHALInts |= HAL_DSI_INTERRUPT_CMDMODE_MDP_DONE;
  }
  if (DSI_INTERRUPT_VIDEOMODE_DONE & uDSIInterrupts)
  {
    uHALInts |= HAL_DSI_INTERRUPT_VIDEOMODE_DONE;
  }
  if (DSI_INTERRUPT_DSI_ERROR & uDSIInterrupts)
  {
    uHALInts |= HAL_DSI_INTERRUPT_DSI_ERROR;
  }
  if (DSI_INTERRUPT_BTA_DONE & uDSIInterrupts)
  {
    uHALInts |= HAL_DSI_INTERRUPT_BTA_DONE;
  }

  return uHALInts;
}


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
uint32 dsiShared_HALInt2DSIInt(uint32 uHALInterrupts)
{
  uint32 uDSIInts = 0;

  if (HAL_DSI_INTERRUPT_CMDMODE_DMA_DONE & uHALInterrupts)
  {
    uDSIInts |= DSI_INTERRUPT_CMDMODE_DMA_DONE;
  }
  if (HAL_DSI_INTERRUPT_CMDMODE_MDP_DONE & uHALInterrupts)
  {
    uDSIInts |= DSI_INTERRUPT_CMDMODE_MDP_DONE;
  }
  if (HAL_DSI_INTERRUPT_VIDEOMODE_DONE & uHALInterrupts)
  {
    uDSIInts |= DSI_INTERRUPT_VIDEOMODE_DONE;
  }
  if (HAL_DSI_INTERRUPT_DSI_ERROR & uHALInterrupts)
  {
    uDSIInts |= DSI_INTERRUPT_DSI_ERROR;
  }
  if (HAL_DSI_INTERRUPT_BTA_DONE & uHALInterrupts)
  {
    uDSIInts |= DSI_INTERRUPT_BTA_DONE;
  }

  return uDSIInts;
}


/* -----------------------------------------------------------------------
**
** FUNCTION: dsiShared_InitDsiDriver()
**
** DESCRIPTION:
**    Bookkeeping the driver instance when new panel is attached, if it is 
**    the first instance, initializing the internal resources.
**
** ----------------------------------------------------------------------- */
DSI_StatusType dsiShared_InitDsiDriver(DSI_DeviceConfigType*   psDeviceConfig)
{
  DSI_StatusType        eStatus         = DSI_STATUS_SUCCESS;
  DSI_DeviceDataType   *pSharedData     = &gDsiSharedData;
  bool32                bOnetimeConfig  = FALSE;

  /* Caution! iDrvInstanceRef is not protected! */
  if(0 == pSharedData->iDrvInstanceRef)  
  {
    DSI_OSAL_MemZero(pSharedData, sizeof(DSI_DeviceDataType));
    
    /* Need to apply one time configuration */
    bOnetimeConfig = TRUE;
  }

  if (DSI_DEVICE_FLAGS_LOAD_DRIVER_STATE & psDeviceConfig->uFlags)
  {
    // Bypass all hardware reconfiguration and only initialize the driver state
  }
  else
  {
    HAL_DSI_PhyConfigType sPhyConfig;

    if (TRUE == bOnetimeConfig)
    {
        /* Init DSI PHY function table */
        HAL_DSI_PhyFxnsInit();

        /* Apply one time globals config, including HW workaround*/
        HAL_DSI_PhyPllInitialize();
    }

    /* If it is first time to initialize, reset HW*/
    HAL_DSI_Init(psDeviceConfig->eDeviceID);
    

    /* Initialize the DSI Phy, timing and PLL is programmed during display open.  
       Populate the basic configuration for the Phy programming, more details programming comes with the display commit. */
    DSI_OSAL_MemZero(&sPhyConfig, sizeof(HAL_DSI_PhyConfigType));

    // Setup common Phy parameters 
    dsiShared_setPhyParameters(psDeviceConfig, &sPhyConfig);

    /* Initialize the DSI Phy, timing and PLL is programmed during display open */
    HAL_DSI_PhySetup(psDeviceConfig->eDeviceID, &sPhyConfig);

    /* Default state LP00, this configuration puts the controller in to LP11 after initialization */
    if (TRUE == psDeviceConfig->bEnableLP11AtInit)
    {
      HAL_DSI_Init_LP11(psDeviceConfig->eDeviceID);
    }
  }
  pSharedData->iDrvInstanceRef++;

  return eStatus;
}

/* -----------------------------------------------------------------------
**
** FUNCTION: dsiShared_DeInitDsiDriver()
**
** DESCRIPTION:
**    Bookkeeping the driver instance when new panel is detached, if it is 
**    the last instance, releasing the internal resources
**
** ----------------------------------------------------------------------- */
DSI_StatusType dsiShared_DeInitDsiDriver(void)
{
  DSI_StatusType       eStatus      = DSI_STATUS_SUCCESS;
  DSI_DeviceDataType  *pSharedData  = &gDsiSharedData;

  /* Caution!!! iDrvInstanceRef is not protected!   */
  if(pSharedData->iDrvInstanceRef)
  {
      pSharedData->iDrvInstanceRef--;

    /*Release all resources when it is last instance  */
    if(0 == pSharedData->iDrvInstanceRef)  
    {
      /* if DeleteCriticalSection failed, the resource will be lost! */
      DSI_OSAL_MemZero(pSharedData, sizeof(DSI_DeviceDataType));
    }
  }

  return eStatus;
}

/* -----------------------------------------------------------------------
**
** FUNCTION: dsiShared_SetDeviceConfig()
**
** DESCRIPTION:
**   Set the shared variable bIsDsInitialized to signify the driver has
**   been initialized or has not been initialized.
**
** INPUT:
**   psDeviceConfig - Input the new value of bIsDsInitialized.
**
** ----------------------------------------------------------------------- */
DSI_StatusType dsiShared_setDeviceConfig(DSI_DeviceConfigType*   psDeviceConfig)
{
  DSI_StatusType        eStatus       = DSI_STATUS_SUCCESS;
  DSI_UserCtxType      *psSharedData  = DSIHOST_GET_USERCTX(psDeviceConfig->eDeviceID); 
  DSI_DeviceConfigType *pSharedConfig = &psSharedData->sDevConfig;

  /* If not using DMA VBIF to fetch data from memory, SW don't need the requested memory. */
  if ( DSI_CHECK_FLAG_SET(psDeviceConfig->uFlags, DSI_DEVICE_FLAGS_DSI_DCS_DMA_ONLY) )
  {
    if ((0    == psDeviceConfig->sMemoryConfig.uSize) ||
        (NULL == psDeviceConfig->sMemoryConfig.pCPUAddress))
    {
      /* Fail in case the DMA buffer is not allocated */
      eStatus = DSI_STATUS_FAILED_RESOURCE_OPEN_FAIL;
    }
    else
    {
      /* Setup the DMA command buffers, the CPU virtual address and physical address must be visible by the DSI driver, align to 64bit boundary (QWord) */
      psSharedData->pCmdVirtualAddr   = (uint8*)psDeviceConfig->sMemoryConfig.pCPUAddress;
      psSharedData->iCmdPhyAddr       = (uint32)psDeviceConfig->sMemoryConfig.sMDPAddress.u.uLowPart;
      psSharedData->uCmdBufferSize    = psDeviceConfig->sMemoryConfig.uSize;
    }
  }

  if (DSI_STATUS_SUCCESS == eStatus)
  {
    /* Save DSI device flags*/
    psSharedData->sDevConfig.uFlags = psDeviceConfig->uFlags;

    /* First reference of DSI Device */
    if(0 == psSharedData->iDeviceRef)    
    {
      // Make a local copy of the device configuration
      DSI_OSAL_MemCpy(pSharedConfig, psDeviceConfig, sizeof(DSI_DeviceConfigType) );

      psSharedData->iDeviceRef    = 1; 
      psSharedData->uDirtyBits    |= DSI_DIRTYBIT_DEVICE_CONFIG;
    }
    else
    { 
      /* SW Limitation, does not permit two panel have different endian/compliant mode */
      if((pSharedConfig->bLittleEndian    != psDeviceConfig->bLittleEndian)  ||
        (pSharedConfig->bNonCompliantMode!= psDeviceConfig->bNonCompliantMode)
        )
      {
        eStatus = DSI_STATUS_FAILED_RESOURCE_OPEN_FAIL;   
      }
      else 
      {
        /* if one of panels does not support CRC, disable all CRC         */       
        if(pSharedConfig->bEnableCRCCheck != psDeviceConfig->bEnableCRCCheck)
        {

          pSharedConfig->bEnableCRCCheck  = FALSE;
          psSharedData-> uDirtyBits      |= DSI_DIRTYBIT_DEVICE_CONFIG;
        }

        /* if one of panels does not support ECC, disable all ECC         */       
        if(pSharedConfig->bEnableECCCheck != psDeviceConfig->bEnableECCCheck)
        {
          pSharedConfig->bEnableECCCheck  = FALSE;
          psSharedData->uDirtyBits       |= DSI_DIRTYBIT_DEVICE_CONFIG;
        }


        /* One more panel is attached to this DSI Host Device             */
        psSharedData->iDeviceRef++; 
      } 
    }/* if(0 == psSharedData->iDeviceRef) else*/  
  }

  return eStatus;

} /* dsiShared__SetDeviceConfig() */



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
DSI_StatusType dsiShared_ClrDeviceConfig(DSI_HandleType   hDevice,
                                         uint32           uFlags)
{
  DSI_StatusType      eStatus       = DSI_STATUS_SUCCESS;
  DSI_UserCtxType    *psSharedData  = NULL; 
  DSI_Device_IDType   eDeviceID     = DSI_DeviceID_0;

  eDeviceID         = dsiShared_DeviceHandle2Id( hDevice );
  psSharedData      = DSIHOST_GET_USERCTX(eDeviceID);

  psSharedData->iDeviceRef--;    
  
  /* This is the last reference to the device? */
  if(0 == psSharedData->iDeviceRef)    
  {
    if (DSI_DEVICE_FLAGS_LOAD_DRIVER_STATE & uFlags)
    {
      // Just load the driver state, bypass any configuration of the hardware
    }
    else
    {
        HAL_DSI_DevicePropertyType   sDeviceCfg;

        /* Disable DSI Controller */
        DSI_OSAL_MemZero(&sDeviceCfg, sizeof(HAL_DSI_DevicePropertyType));
        HAL_DSI_DeviceConfig(eDeviceID, &sDeviceCfg);

        /* Disable DSI Phy */
        HAL_DSI_PhyDisable(eDeviceID);
    }

    /* Clear the device context */
    DSI_OSAL_MemZero(psSharedData, sizeof(DSI_UserCtxType));

    eStatus   = DSI_STATUS_SUCCESS;
  }
  else
  {
    eStatus   = DSI_STATUS_FAILED_DEVICE_IN_USE;
  }

  return eStatus;
  
}


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
DSI_StatusType dsiShared_setDisplayConfig(DSI_HandleType          hDevice, 
                                          DSI_DisplayConfigType  *psDisplayConfig)
{
  DSI_StatusType          eStatus     = DSI_STATUS_SUCCESS;
  uint32                  i           = 0;
  DSI_Device_IDType       eDeviceID   = DSI_DeviceID_0;
  DSI_Channel_IDType      eDisplayID  = DSI_ChannelID_NONE;

  DSI_UserCtxType*        psSharedData = NULL; 
  DSI_DisplayCtxType*     psDisplayCtx = NULL; 
  
  
  /* Get Device ID and Display ID */
  eDeviceID     = dsiShared_DeviceHandle2Id(hDevice);
  eDisplayID    = psDisplayConfig->sDisplayInfo.eDisplayId;

  /* Get the pointer to the internal struct */
  psSharedData  = DSIHOST_GET_USERCTX(eDeviceID);
  psDisplayCtx  = DSIHOST_GET_DISPLAYCTX(eDeviceID, eDisplayID);

  /* is this display already in use?    */
  if( TRUE == psDisplayCtx->bInitialized )
  {
    eStatus = DSI_STATUS_FAILED_DRIVER_ALREADY_INITIALIZED; 
    goto EXIT;
  }

  psSharedData->bCPhyMode  = psDisplayConfig->sDisplayInfo.bCPhyMode;

  /* Check Lane mapping */
  for( i=0; i<DSI_LaneMap_MAX-1; i++)
  {
    if( psDisplayConfig->eDisplayLane[i] != psSharedData->eDevLane[i] )
    {
      if(psSharedData->eDevLane[i] != DSI_LaneMap_NONE)
      {
        /* This lane mapping conflict with previous panel lane setting*/
        eStatus = DSI_STATUS_FAILED_NO_SUPPORT_FOR_CHANNEL; 
        goto EXIT;
      }
      else
      {
        psSharedData->eDevLane[i] = psDisplayConfig->eDisplayLane[i];
        psSharedData->uDirtyBits   |= DSI_DIRTYBIT_DEVICE_LANE;
      }
    }
  }

  /* Save the basic display info, and mark it dirty */
  DSI_OSAL_MemCpy(& psDisplayCtx->sDisplayInfo, &psDisplayConfig->sDisplayInfo,
             sizeof(DSI_DisplayInfoType));
  psDisplayCtx->iDispDirtyBits   |= DSI_DIRTYBIT_DISPLAY_INFO; 

  /* Do we need to update video mode configuration? */
  if( TRUE == psDisplayConfig->sVideoModeConfg.bEnable )
  {
    psSharedData->iVideoModeRef++;
    /* Traffic mode         */
    psDisplayCtx->eVideoTraffic = psDisplayConfig->sVideoModeConfg.eVideoTraffic;
    psDisplayCtx->iDispDirtyBits  |= DSI_DIRTYBIT_VIDEOMODE_TRAFFIC;
  
    /* Timing Configuration */
    DSI_OSAL_MemCpy(&psDisplayCtx->sVideoTiming, &psDisplayConfig->sVideoModeConfg.sVideoTiming,
      sizeof(DSI_VideoTimingType));
    psDisplayCtx->iDispDirtyBits  |= DSI_DIRTYBIT_VIDEOMODE_TIMING;               

    /* Power Options        */
    DSI_OSAL_MemCpy(&psDisplayCtx->sPowerOption, &psDisplayConfig->sVideoModeConfg.sPowerOptions,
      sizeof(DSI_VPowerOptionType));
    psDisplayCtx->iDispDirtyBits  |= DSI_DIRTYBIT_VIDEOMODE_POWEROPTION;

    /* DSC Configuration for the DSI controller */
    DSI_OSAL_MemCpy(&psDisplayCtx->sDscCompressionInfo.sDscVidCntrlrConfig, &psDisplayConfig->sVideoModeConfg.sDSCVideoConfig,
                    sizeof(DSI_DscVideoModeConfigType));
  }

  if( TRUE == psDisplayConfig->sCmdModeConfg.bEnable )
  {
    psSharedData->iCmdModeRef++;
    /* Ext TE Pin Configuration  */
    if( DSI_ExtTEMode_NONE == psDisplayCtx->sExtTEConfig.eExtTEMode )
    {
      DSI_OSAL_MemCpy(&psDisplayCtx->sExtTEConfig, &psDisplayConfig->sCmdModeConfg.sExtTEConfig,
        sizeof(DSI_ExtTEConfigType));
      psDisplayCtx->iDispDirtyBits  |= DSI_DIRTYBIT_CMDMODE_EXTTE;
    }
 
    /* Trigger Configuration      */
    psDisplayCtx->eImageTrigger = psDisplayConfig->sCmdModeConfg.eImageTrigger; 
    psDisplayCtx->eCMDTrigger   = psDisplayConfig->sCmdModeConfg.eCMDTrigger;
    psDisplayCtx->iDispDirtyBits  |= DSI_DIRTYBIT_CMDMODE_TRIGGER;

    /* Misc CMD mode setting, from BSP file*/
    psDisplayCtx->iMaxInterleave  = dsiBsp_iMaxInterleave;
    psDisplayCtx->iMDPDataType    = dsiBsp_iMDPDataType;
    psDisplayCtx->uIdleTime       = psDisplayConfig->sCmdModeConfg.uIdleTime;
    psDisplayCtx->iDispDirtyBits  |= DSI_DIRTYBIT_CMDMODE_MISC;

    /* DSC Configuration for the DSI controller */
    DSI_OSAL_MemCpy(&psDisplayCtx->sDscCompressionInfo.sDscCmdCntrlrConfig, &psDisplayConfig->sCmdModeConfg.sDSCCmdConfig,
                    sizeof(DSI_DscCmdModeConfigType));
  }
  psDisplayCtx->bInitialized  = TRUE;

EXIT:
  return eStatus;
}
/* -----------------------------------------------------------------------
**
** FUNCTION: dsiShared_ClrDisplayConfig()
**
** DESCRIPTION:
**   TBD
**
** INPUT:
**   TBD
**
** ----------------------------------------------------------------------- */
DSI_StatusType dsiShared_ClrDisplayConfig( DSI_HandleType  hDisplay )
{
  DSI_StatusType         eStatus      = DSI_STATUS_SUCCESS;
  DSI_Device_IDType     eDeviceId    = DSI_DeviceID_0;
  DSI_Channel_IDType    eDisplayId   = DSI_ChannelID_NONE;
  
  DSI_UserCtxType*      psSharedData = NULL; 
  DSI_DisplayCtxType*   psDisplayCtx = NULL; 

  DSI_ERROR_AND_EXIT(eStatus, EXIT);
  
  dsiShared_Handle2DisplayID(hDisplay,&eDeviceId,&eDisplayId);

  /* Get the pointer to the internal struct */
  psSharedData  = DSIHOST_GET_USERCTX(eDeviceId);
  psDisplayCtx  = DSIHOST_GET_DISPLAYCTX(eDeviceId, eDisplayId);


  if(psDisplayCtx->sVideoTiming.iHsyncBackPorchDclk != 0)
    psSharedData->iVideoModeRef--;

  psSharedData->iCmdModeRef--;

  // Reset the controller on shutdown to ensure it is in the proper state.
  // Video mode engine will stay busy unless the hardware is reset
  HAL_DSI_Disable_CTRL(eDeviceId, HAL_DSI_HWBLOCK_CONTROLLER);
  HAL_DSI_Reset(eDeviceId);
  
  DSI_OSAL_MemZero(psDisplayCtx, sizeof(DSI_DisplayCtxType));

  /* Turn off DSI clock branch control */
  HAL_DSI_ClockConfig(eDeviceId, 0);

  /* Vote for DSI clock off */
  dsiSystem_DisableClock(dsiShared_DeviceId2Handle(eDeviceId));

EXIT:
  return eStatus;
}

/*-----------------------------------------------------------------------

dsiShared_ClientPackData

DESCRIPTION:
    Pack the DSI send/read commands in the hardware.

----------------------------------------------------------------------- */
DSI_StatusType dsiShared_ClientPackData(DSI_HandleType         hDisplay,
                                        DSI_WritePacketType   *psWritePacket,
                                        DSI_ReadPacketType    *psReadPacket)
{
  DSI_StatusType          eStatus           = DSI_STATUS_SUCCESS;

  if ((NULL == psWritePacket) && (NULL == psReadPacket))
  {
    eStatus = DSI_STATUS_FAILED_EXPECTED_NON_NULL_PTR;     
  }
  else
  {
    DSI_Device_IDType       eDeviceId         = DSI_DeviceID_0;
    DSI_Channel_IDType      eDisplayId        = DSI_ChannelID_NONE;
    uint32                  iPacketLen        = 0;
    DSI_UserCtxType        *psSharedData      = NULL; 
    DSI_CMDTriggerType      eDMATriggerType   = DSI_Display_CMDTrigger_NONE;
    HAL_DSI_PackDataType    sHALPackData;
    HAL_DSI_WritePacketType sHALWritePacket;
    uint8                  *pDmaDataBuffer    = NULL;
    uint32                  uDataType         = 0;
    uint32                  uPayloadLen       = 0;
    DSI_Display_VCType      eDisplayVC        = DSI_Display_VC_0;
    bool32                  bLastPacket       = TRUE;
    bool32                  bBTARequest       = FALSE;
    uint8*                  pInputData        = NULL;
    bool32                  bHighSpeedMode    = FALSE;
    uint32                  uFlags            = 0;

    dsiShared_Handle2DisplayID(hDisplay,&eDeviceId,&eDisplayId);
    psSharedData    = DSIHOST_GET_USERCTX(eDeviceId);

    // Get the current DMA trigger mode 
    HAL_DSI_GetDMATriggerConfig( eDeviceId, &eDMATriggerType );

    /* Get packet  info from either write packet or read packet */
    if (NULL != psWritePacket)
    {
      uDataType       = psWritePacket->uCmdDataType;
      uPayloadLen     = psWritePacket->bPayloadLen;
      eDisplayVC      = psWritePacket->eDisplayVC;
      bLastPacket     = !(psWritePacket->bQueuePacket);
      bBTARequest     = psWritePacket->bBTARequest;
      pInputData      = psWritePacket->puPayload;
      bHighSpeedMode  = psWritePacket->bHighSpeedMode;
      uFlags          = psWritePacket->uFlags;
    }
    else
    {
      uDataType       = psReadPacket->uReadCmdDT;
      uPayloadLen     = psReadPacket->uReadParamInByte;
      eDisplayVC      = psReadPacket->eDisplayVC;
      bLastPacket     = TRUE;
      bBTARequest     = TRUE;
      pInputData      = psReadPacket->puReadParam;
      bHighSpeedMode  = psReadPacket->bHighSpeedMode;
      uFlags          = psReadPacket->uFlags;
    }
    /* Calculate Packet size in Byte */
    dsShared_GetPackDataSize(uDataType, uPayloadLen, &iPacketLen);

    //If command transfer is required in interleaved mode.
    if (DSI_CHECK_FLAG_SET(uFlags, DSI_CMD_FLAG_CMD_DMA_INTERLEAVED ))
    {
      //if current dma trigger mode is not SW+SOF trigger
      if( DSI_Display_CMDTrigger_SW_SOFEOF != eDMATriggerType)
      {
        //set DMA trigger mode to SW+SOF mode.
        HAL_DSI_SetDMATriggerConfig( eDeviceId, DSI_Display_CMDTrigger_SW_SOFEOF );
      }
    }
    else
    {
      //if current dma trigger mode is not SW trigger
      if( DSI_Display_CMDTrigger_SW != eDMATriggerType)
      { 
        //set DMA trigger mode to SW trigger
        HAL_DSI_SetDMATriggerConfig( eDeviceId, DSI_Display_CMDTrigger_SW );
      }
    }

    // Select Command data fetch method
    if (DSI_CHECK_FLAG_SET(psSharedData->sDevConfig.uFlags, DSI_DEVICE_FLAGS_DSI_DCS_DMA_ONLY) )
    {
      if ((iPacketLen + psSharedData->pCurrentBufferSize) > psSharedData->uCmdBufferSize)
      {
        /* Clear the buffer in case there's any previous queued data. In this case, any previously queued data will be lost
        and error code will be returned to caller. The caller should resend all previously queued packets. */
        psSharedData->pCurrentBufferSize = 0;
        eStatus = DSI_STATUS_FAILED_BUFFER_TOO_SMALL;
      }
      else
      {
        /* Command data fetched via DMA VBIF interface*/
        pDmaDataBuffer    = psSharedData->pCmdVirtualAddr;
        uFlags            = 0;
      }
    }
    else
    {
      /* Command data fetched via TPG FIFO */
      if ((iPacketLen + psSharedData->pCurrentBufferSize) > HAL_DSI_MAX_DMAFIFOSIZE_BYTE)
      {
        /* Command data Size is too large for TPG FIFO*/
        psSharedData->pCurrentBufferSize = 0;
        eStatus = DSI_STATUS_FAILED_BUFFER_TOO_SMALL;
        DSI_Log_Message(DSI_LOGLEVEL_ERROR, "DSIDriver: DSI Write command is too long for FIFO (Use DMA backed packets).");
      }
      else
      {
        /* Command data fetched via TPG FIFO */
        pDmaDataBuffer    = psSharedData->uCmdFIFOBuffer;
        uFlags            = HAL_DSI_FLAG_DATA_FETCH_VIA_FIFO;
      }
    }

    if (DSI_STATUS_SUCCESS == eStatus)
    {
      /* Pack data into memory */
      sHALPackData.eDisplayVC   = eDisplayVC;
      sHALPackData.bLastPacket  = bLastPacket;
      sHALPackData.bBTARequest  = bBTARequest;
      sHALPackData.pInputData   = pInputData;
      sHALPackData.pOutputData  = psSharedData->pCurrentBufferSize + pDmaDataBuffer;
      sHALPackData.uPayloadLen  = (uint16)uPayloadLen;
      sHALPackData.uCmdDataType = uDataType;
      dsShared_PackData(&sHALPackData);

      /*Update totol packet buffer size*/
      psSharedData->pCurrentBufferSize += iPacketLen;
      
      /* Write packet to DSI controller*/
      if (TRUE == sHALPackData.bLastPacket )
      {
        sHALWritePacket.bHighSpeedMode    = bHighSpeedMode;
        sHALWritePacket.uPhyAddress       = psSharedData->iCmdPhyAddr;
        sHALWritePacket.uPayloadLen       = psSharedData->pCurrentBufferSize;
        sHALWritePacket.pPackedFifoBuffer = psSharedData->uCmdFIFOBuffer;
        sHALWritePacket.uFlags            = uFlags;
        HAL_DSI_WriteData(eDeviceId, &sHALWritePacket );
      }
    }
  }

  return eStatus;
}
/*-----------------------------------------------------------------------

DSI_Display_ClientWrite

DESCRIPTION:
    This API allows users to send the DSI command out to DSI Panels. 

----------------------------------------------------------------------- */
DSI_StatusType dsiShared_ClientWrite(DSI_HandleType          hDisplay,
                                     DSI_WritePacketType*    psWritePacket)
{
  DSI_StatusType          eStatus           = DSI_STATUS_SUCCESS;
  DSI_Device_IDType       eDeviceId         = DSI_DeviceID_0;
  DSI_Channel_IDType      eDisplayId        = DSI_ChannelID_NONE;
  DSI_UserCtxType        *psSharedData      = NULL; 
  
  dsiShared_Handle2DisplayID(hDisplay,&eDeviceId,&eDisplayId);
  psSharedData    = DSIHOST_GET_USERCTX(eDeviceId);

  /* Pack the read data packet information in the hardware */    
  eStatus = dsiShared_ClientPackData(hDisplay, psWritePacket, NULL);

  /* Trigger the hardware to transfer the DSI send data packet from the MSM to the panel */
  if (DSI_STATUS_SUCCESS == eStatus)
  {
    /* Enforce memory barrier to ensure all writes to the DMA buffer prior to this are complete */
    DSI_OSAL_MB();

    HAL_DSI_Trigger(eDeviceId, DSI_TriggerFlag_DMA_SW);

    psSharedData->pCurrentBufferSize = 0;

    /* if this is a async read or this is an interleaved mode operation, then just return */
    if(0 == (psWritePacket->uFlags & DSI_CMD_FLAG_CMD_ASYNC))
    {
      eStatus = dsiShared_WaitforInterrupt(eDeviceId, HAL_DSI_INTERRUPT_CMDMODE_DMA_DONE, DSI_MAX_POLLING_TIME);
    }
  }

  return eStatus;
}


/*-----------------------------------------------------------------------

dsiShared_ClientRead

DESCRIPTION:
    This API allows users to send the DSI read command out to DSI Panels, 
   and wait until it gets the data from panel. 

----------------------------------------------------------------------- */
DSI_StatusType dsiShared_ClientRead(
    DSI_HandleType          hDisplay,
    DSI_ReadPacketType*     psReadPacket
    )
{
  DSI_StatusType            eStatus           = DSI_STATUS_SUCCESS;
  DSI_Device_IDType         eDeviceId         = DSI_DeviceID_0;
  DSI_Channel_IDType        eDisplayId        = DSI_ChannelID_NONE;
  DSI_UserCtxType          *psSharedData      = NULL; 
  HAL_DSI_ReadPacketType    sHALReadPacket;

  dsiShared_Handle2DisplayID(hDisplay,&eDeviceId,&eDisplayId);
  psSharedData    = DSIHOST_GET_USERCTX(eDeviceId);

  /* Pack the read data packet information in the hardware */    
  eStatus = dsiShared_ClientPackData(hDisplay, NULL, psReadPacket);

  /* Trigger the hardware to transfer the DSI read data packet from the MSM to the panel */
  if (DSI_STATUS_SUCCESS == eStatus)
  {
    /* Enforce memory barrier to ensure all writes to the DMA buffer prior to this are complete */
    DSI_OSAL_MB();

    /* Trigger Command out to panel */
    HAL_DSI_Trigger(eDeviceId, DSI_TriggerFlag_DMA_SW);

    psSharedData->pCurrentBufferSize = 0;

    /* if this is a async read, then just return */
    if(0 == (psReadPacket->uFlags & DSI_CMD_FLAG_CMD_ASYNC))
    {
      eStatus = dsiShared_WaitforInterrupt(eDeviceId, HAL_DSI_INTERRUPT_BTA_DONE, DSI_MAX_POLLING_TIME);
      if (DSI_STATUS_SUCCESS == eStatus)
      {
        DSI_OSAL_MemZero(&sHALReadPacket, sizeof(sHALReadPacket));
        sHALReadPacket.uReadPacketBufferSize  = psReadPacket->uReadPacketBufferSize;
        sHALReadPacket.puReadPacket           = psReadPacket->puReadPacket;
        sHALReadPacket.uFlags                 = psReadPacket->uFlags;
        HAL_DSI_ReadData(eDeviceId, &sHALReadPacket);

        psReadPacket->uRetPacketByte  = sHALReadPacket.uRetPacketByte;
        psReadPacket->uTotalPacketWC  = sHALReadPacket.uTotalPacketWC;
      }
      else
      {
        eStatus = DSI_STATUS_FAILED_TIMEOUT;
      }
    }
  }

  return eStatus;

}

/*-----------------------------------------------------------------------

dsiShared_Comit

DESCRIPTION:
    This API allows users to send the DSI command out to DSI Panels. 

----------------------------------------------------------------------- */
DSI_StatusType dsiShared_Comit(DSI_HandleType   hDevice, 
                              uint32           iFlags )
{
  
  DSI_StatusType          eStatus      = DSI_STATUS_SUCCESS;
  uint32                  uDirtyBits   = 0;
  DSI_Device_IDType       eDeviceID    = DSI_DeviceID_0;
  uint32                  i            = 0;
  uint32                  iClockCfg    = 0;
  DSI_UserCtxType*        psSharedData = NULL; 
  uint32                  uInitMaster  = dsiBsp_DSI_uMinInitMaster;

  (void) iFlags;

  /* Get Device ID and Display ID */
  eDeviceID     = dsiShared_DeviceHandle2Id(hDevice);

  /* Get the pointer to the internal struct */
  psSharedData  = DSIHOST_GET_USERCTX(eDeviceID);

  /* Turn on DSI device HW*/
  iClockCfg = HAL_DSI_Clock_AHBM_SCLK|HAL_DSI_Clock_PCLK|HAL_DSI_Clock_MAINCLK;
  HAL_DSI_ClockConfig(eDeviceID, iClockCfg);

  for(i = 0; i < (uint32)DSI_ChannelID_MAX; i++ )
  {
    /*Any update on this display channel?*/
    uDirtyBits = psSharedData->sDisplayCtx[i].iDispDirtyBits;
    
    if( 0 != uDirtyBits )  
    {
      /*Committing Video Mode Config */
      if(uDirtyBits & DSI_DIRTYBIT_VIDEOMODE_TRAFFIC  ||
         uDirtyBits & DSI_DIRTYBIT_VIDEOMODE_TIMING   ||
         uDirtyBits & DSI_DIRTYBIT_VIDEOMODE_POWEROPTION)
      {
        eStatus = dsiShared_CommitVideoModeConfig(eDeviceID, (DSI_Channel_IDType)i);
        DSI_ERROR_AND_EXIT(eStatus, EXIT);
      }

      /*Committing Command Mode EXTTE Config */
      if(uDirtyBits & DSI_DIRTYBIT_CMDMODE_EXTTE )
      {
        eStatus = dsiShared_CommitExtTEConfig(eDeviceID, (DSI_Channel_IDType)i);
        DSI_ERROR_AND_EXIT(eStatus, EXIT);
      }

      /*Committing Command Mode Trigger Config */
      if(uDirtyBits & DSI_DIRTYBIT_CMDMODE_TRIGGER )
      {
        eStatus = dsiShared_CommitTriggerConfig(eDeviceID, (DSI_Channel_IDType)i);
        DSI_ERROR_AND_EXIT(eStatus, EXIT);
      }

      /*Committing Command mode config */
      if(uDirtyBits & DSI_DIRTYBIT_CMDMODE_MISC )
      {
        eStatus = dsiShared_CommitCmdModeConfig(eDeviceID, (DSI_Channel_IDType)i);
        DSI_ERROR_AND_EXIT(eStatus, EXIT);
      }

      /*Determine the longest master wait time*/
      if (psSharedData->sDisplayCtx[i].sDisplayInfo.uInitMasterTime > uInitMaster)
      {
        uInitMaster = psSharedData->sDisplayCtx[i].sDisplayInfo.uInitMasterTime;
      }
      
    }
  }

  uDirtyBits    = psSharedData->uDirtyBits;
  if(uDirtyBits & DSI_DIRTYBIT_DEVICE_CONFIG )
  {
    eStatus = dsiShared_CommitDeviceConfig(eDeviceID);
    DSI_ERROR_AND_EXIT(eStatus, EXIT);
  }

  if(uDirtyBits & DSI_DIRTYBIT_DEVICE_LANE )
  {
    eStatus = dsiShared_CommitLaneConfig(eDeviceID);
    DSI_ERROR_AND_EXIT(eStatus, EXIT);

    if (uInitMaster)
    {
      // Enforce a stall to allow the panel side IC to initialize after the reset has been asserted and we have entered LP11
      DSI_OSAL_SleepMs(uInitMaster);    
    }
  }


EXIT:
  return eStatus;
}



/*-----------------------------------------------------------------------
 * dsiShared_GetBitsPerPixel
 *
 * DESCRIPTION:
 *  This is a internal function that returns Bits Per Pixel (BPP) for a particular color format.
----------------------------------------------------------------------- */
uint32 dsiShared_GetBitsPerPixel(DSI_ColorFormatType eColorFormat)
{
  uint32         uBpp       = 0;

  switch(eColorFormat)
  {
    case DSI_COLOR_RGB_111_3BPP:
    case DSI_COLOR_RBG_111_3BPP:
    case DSI_COLOR_BGR_111_3BPP:
    case DSI_COLOR_BRG_111_3BPP:
    case DSI_COLOR_GRB_111_3BPP:
    case DSI_COLOR_GBR_111_3BPP:
      uBpp = 3;
      break;

    case DSI_COLOR_RGB_332_8BPP:
    case DSI_COLOR_RBG_332_8BPP:
    case DSI_COLOR_BGR_332_8BPP:
    case DSI_COLOR_BRG_332_8BPP:
    case DSI_COLOR_GRB_332_8BPP:
    case DSI_COLOR_GBR_332_8BPP:
      uBpp = 8;
      break;

    case DSI_COLOR_RGB_444_12BPP:
    case DSI_COLOR_RBG_444_12BPP:
    case DSI_COLOR_BGR_444_12BPP:
    case DSI_COLOR_BRG_444_12BPP:
    case DSI_COLOR_GRB_444_12BPP:
    case DSI_COLOR_GBR_444_12BPP:
      uBpp = 12;
      break;

    case DSI_COLOR_RGB_565_16BPP:
    case DSI_COLOR_RBG_565_16BPP:
    case DSI_COLOR_BGR_565_16BPP:
    case DSI_COLOR_BRG_565_16BPP:
    case DSI_COLOR_GRB_565_16BPP:
    case DSI_COLOR_GBR_565_16BPP:
      uBpp = 16;
      break;

    case DSI_COLOR_RGB_666_18BPP:
    case DSI_COLOR_RBG_666_18BPP:
    case DSI_COLOR_BGR_666_18BPP:
    case DSI_COLOR_BRG_666_18BPP:
    case DSI_COLOR_GRB_666_18BPP:
    case DSI_COLOR_GBR_666_18BPP:
      uBpp = 18;
      break;

    case DSI_COLOR_RGB_666_24BPP:
    case DSI_COLOR_RBG_666_24BPP:
    case DSI_COLOR_BGR_666_24BPP:
    case DSI_COLOR_BRG_666_24BPP:
    case DSI_COLOR_GRB_666_24BPP:
    case DSI_COLOR_GBR_666_24BPP:
    case DSI_COLOR_RGB_888_24BPP:
    case DSI_COLOR_RBG_888_24BPP:
    case DSI_COLOR_BGR_888_24BPP:
    case DSI_COLOR_BRG_888_24BPP:
    case DSI_COLOR_GRB_888_24BPP:
    case  DSI_COLOR_GBR_888_24BPP:
      uBpp = 24;
      break;

    default:
      break;
  }

  return uBpp;
}

/****************************************************************************
*
** FUNCTION: dsiShared_CalculateDynamicRefresh()
*/
/*!
* \brief
*     Calculate the dynamic refresh bit clock list from the dynamic refresh rate
*
* \param [in] psDisplayConfig     - needed by dsiShared_CalculateDesiredBitClk,storing dynamic
*                                   refresh related parameters
*
* \retval - none
*
****************************************************************************/
static void dsiShared_CalculateDynamicRefresh(DSI_DisplayConfigType                  *psDisplayConfig)
{
    uint32                  uBitsPerPixel         = 0;
    uint32                  i                     = 0;
    uint32                  uRefreshRateTmp;
    uint32                  uBitClockFreqTmp;

    MDP_Panel_AttrType  *pDisplayInfo = MDP_GET_DISPLAYINFO(psDisplayConfig->sDisplayInfo.eMDPDisplayId);
    
    /* Get dynamic refresh rate */
    while((i < MAX_NUM_DYNAMIC_REFRESH) &&
          (pDisplayInfo->uDynamicRefreshRates[i] > 0))
    {
      psDisplayConfig->sDisplayInfo.sDynamicRefreshInfo.uDynamicRefreshRates[i] = pDisplayInfo->uDynamicRefreshRates[i];
      i++;
    }
    psDisplayConfig->sDisplayInfo.sDynamicRefreshInfo.uNumOfDynamicFreq = i;

    /* Calculate the bits per pixel */
    uBitsPerPixel = dsiShared_GetBitsPerPixel(psDisplayConfig->sDisplayInfo.eColorFormat);

    /* backup  psDisplayConfig internal param*/
    uRefreshRateTmp = psDisplayConfig->sDisplayInfo.uRefreshRate;
    uBitClockFreqTmp = psDisplayConfig->sDisplayInfo.uBitClockFreq;

    /* set 0 to force recalculate bitClkFreq */
    psDisplayConfig->sDisplayInfo.uBitClockFreq = 0;

    for (i = 0; i < psDisplayConfig->sDisplayInfo.sDynamicRefreshInfo.uNumOfDynamicFreq; i++)
    {
      /*Calculate uDynamicBitClkFreq */
      psDisplayConfig->sDisplayInfo.uRefreshRate = psDisplayConfig->sDisplayInfo.sDynamicRefreshInfo.uDynamicRefreshRates[i];
      psDisplayConfig->sDisplayInfo.sDynamicRefreshInfo.uDynamicBitClkFreq[i] = dsiShared_CalculateDesiredBitClk(&psDisplayConfig->sDisplayInfo, uBitsPerPixel);
    }
    /* restore psDisplayConfig internal param*/
    psDisplayConfig->sDisplayInfo.uRefreshRate  = uRefreshRateTmp;
    psDisplayConfig->sDisplayInfo.uBitClockFreq = uBitClockFreqTmp;
}


/****************************************************************************
*
** FUNCTION: dsiShared_CalibrateDynamicRefreshFreq()
*/
/*!
* \brief
*     Do calibration to get PLL codes for each dynamic refresh rate
*
* \param [in] psDsiPhyConfig      - needed by HAL_DSI_PhyPllSetup,dynamic refresh 
                                    bit clk frequency will be passed in through 
                                    this parameter
* \param [out] psDsiPhyConfigInfo - PhyConfigInfo structture for storing dynamic
*                                   refresh related parameters
* \param [in] psDisplayConfig     - Pass in parameter for setup DSI Phy timing
*
* \retval - none
*
****************************************************************************/
static DSI_StatusType dsiShared_CalibrateDynamicRefreshFreq(HAL_DSI_PhyConfigType *psDsiPhyConfig,
                              DSI_DisplayConfigType  *psDisplayConfig)
{
  DSI_StatusType            eStatus          = DSI_STATUS_SUCCESS;
  /*try do dynamic calibration here*/
  if (psDisplayConfig->sDisplayInfo.sDynamicRefreshInfo.uNumOfDynamicFreq > 0)
  {
    int                          i                  = 0;
    DSI_Device_IDType            eDeviceId          = psDsiPhyConfig->eDeviceId;
    HAL_DSI_TimingSettingType    sDsiTimingParams;
    HAL_DSI_PhyConfigInfoType    sDsiPhyConfigInfo;

    /* Make the default value of PllCodes to 0 */
    DSI_OSAL_MemZero((void*)&psDisplayConfig->sDisplayInfo.sPllCodesInfo, sizeof(HAL_DSI_PHY_PLL_Codes_Info));

    /* Initialize  sDsiPhyConfigInfo*/
    DSI_OSAL_MemZero((void*)&sDsiPhyConfigInfo, sizeof(HAL_DSI_PhyConfigInfoType));

    /* save number of refresh rate here */
    psDisplayConfig->sDisplayInfo.sPllCodesInfo.uRefreshRateCount = psDisplayConfig->sDisplayInfo.sDynamicRefreshInfo.uNumOfDynamicFreq;
    
    for (i = 0; i < psDisplayConfig->sDisplayInfo.sDynamicRefreshInfo.uNumOfDynamicFreq; i++)
    {
      /* Disable pll */
      HAL_DSI_PhyPllEnable(eDeviceId, FALSE);

      psDsiPhyConfig->uDesiredBitClkFreq = psDisplayConfig->sDisplayInfo.sDynamicRefreshInfo.uDynamicBitClkFreq[i];
      
      DSI_Log_Message(DSI_LOGLEVEL_WARN, "uDynamicBitClkFreq[%d]=%d\n", i, psDisplayConfig->sDisplayInfo.sDynamicRefreshInfo.uDynamicBitClkFreq[i]);

      /* Setup DSI Phy timing */
      sDsiTimingParams.eDeviceId          = eDeviceId;
      sDsiTimingParams.uBitclock          = psDisplayConfig->sDisplayInfo.sDynamicRefreshInfo.uDynamicBitClkFreq[i];
      sDsiTimingParams.uEscapeFreq        = psDisplayConfig->sDisplayInfo.uEscapeFreq;
      sDsiTimingParams.pPreDefinedTimings = &psDisplayConfig->sTimingOverride;

      if (HAL_MDSS_STATUS_SUCCESS != HAL_DSI_PhySetupTimingParams(&sDsiTimingParams))
      {
        DSI_Log_Message(DSI_LOGLEVEL_WARN, "DSI Phy timing setup failed.\n");
      }

      if (HAL_MDSS_STATUS_SUCCESS != HAL_DSI_PhyPllSetup(psDsiPhyConfig, &sDsiPhyConfigInfo))
      {
        DSI_Log_Message(DSI_LOGLEVEL_ERROR, "DSIDriver: DSI%d PLL cannot be locked (Freq:%dHz)!\n", eDeviceId, psDsiPhyConfig->uDesiredBitClkFreq);
        eStatus = DSI_STATUS_FAILED_RESOURCE_NOT_READY;
      }

      /* get Pll Codes for each refresh rate */
      psDisplayConfig->sDisplayInfo.sPllCodesInfo.sPllCodes[i].uValid = 1;
      psDisplayConfig->sDisplayInfo.sPllCodesInfo.sPllCodes[i].uVcoRate = sDsiPhyConfigInfo.uPllVcoOutputFreq;
      HAL_DSI_PhyGetPllCodes(eDeviceId, &psDisplayConfig->sDisplayInfo.sPllCodesInfo.sPllCodes[i]);
    }

    /* override the refresh rate with the last dynamic refresh rate */
    psDisplayConfig->sDisplayInfo.uRefreshRate =
        psDisplayConfig->sDisplayInfo.sDynamicRefreshInfo.uDynamicRefreshRates[psDisplayConfig->sDisplayInfo.sDynamicRefreshInfo.uNumOfDynamicFreq - 1];
  }
  else
  {
    eStatus = DSI_STATUS_FAILED_RESOURCE_NOT_READY;
  }
  return eStatus;
}


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
* \param [out] psDsiPhyConfigInfo - PhyConfigInfo structture for storing dynamic
*                                   refresh related parameters
* \param [in] psDisplayConfig     - Pass in parameter for setup DSI Phy timing
*
* \retval - success or fail reason
*
****************************************************************************/
DSI_StatusType dsiShared_FrequencyCalibration(HAL_DSI_PhyConfigType *psDsiPhyConfig,
                              DSI_DisplayConfigType  *psDisplayConfig)
{
  DSI_StatusType            eStatus          = DSI_STATUS_SUCCESS;
  
  dsiShared_CalculateDynamicRefresh(psDisplayConfig);

  eStatus = dsiShared_CalibrateDynamicRefreshFreq(psDsiPhyConfig, psDisplayConfig);
  
  return eStatus;
}


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
                                               uint32                 uBitsPerPixel )
{
   uint64   uBitClkDesired = 0;

   // if the upper layer has requested a specific bitclock frequency override the calculated bitclock frequency.
   if (pDSIConfig->uBitClockFreq > 0)
   {
      // Use the bitclock from the upper layer (in Hz)
      uBitClkDesired = (uint64)(pDSIConfig->uBitClockFreq);
   }
   else
   {
      // uRefreshRate is in Q16.16 format
      uint32 uRefreshRate = pDSIConfig->uRefreshRate;

      // Perform some sanity checking, clamp the refresh rate
      // MAX/MIN refresh rate also defined as Q16.16 format
      if (uRefreshRate > DSI_MAX_REFRESH_RATE)
      {
          uRefreshRate = DSI_MAX_REFRESH_RATE;
      }
      else if (uRefreshRate < DSI_MIN_REFRESH_RATE)
      {
          uRefreshRate = DSI_MIN_REFRESH_RATE;
      }
      
      // calculate the required bitclock frequency (note the iVisheigthPx & iVisWidthPx already included blanking in the DSI Video mode case.
      // Ensure no divide by zeros
      if (pDSIConfig->uNumLanes)
      {
         // bitclock (in Mhz) = width * height * refresh * bpp / (1Million * lanes * 65536)
         // 0x10000: convert from Q16.16 to normal (uRefreshRate is Q16.16)
         uBitClkDesired = (((uint64)(pDSIConfig->iVisHeightPx+pDSIConfig->iBlankHeightPx) * (uint64)(pDSIConfig->iVisWidthPx+pDSIConfig->iBlankWidthPx) * (uint64)uRefreshRate * (uint64)uBitsPerPixel) / (uint64)(pDSIConfig->uNumLanes * 0x10000));
      }

      // Increment calculated bitclk by uBitClkScalePercent.
      uBitClkDesired = ((uint64)uBitClkDesired + ((uint64)uBitClkDesired * (uint64)(pDSIConfig->uBitClkScalePercent))/(uint64)100);
   }

   return (uint32)uBitClkDesired;
}


/***********************************************************************************************/
/*Internal Functions                                                                           */
/* Following functions should only be called from this files, and should be multi-thread proteced
   by caller functions!!!
************************************************************************************************/

/*-----------------------------------------------------------------------

dsiShared_CommitVideoModeConfig

DESCRIPTION:
    . 

----------------------------------------------------------------------- */
static DSI_StatusType dsiShared_CommitVideoModeConfig(  DSI_Device_IDType   eDeviceID, 
                                                       DSI_Channel_IDType  eDisplayID )
{ 
  DSI_StatusType           eStatus      = DSI_STATUS_SUCCESS;
  DSI_DisplayCtxType*     psDisplayCtx = NULL; 

  HAL_DSI_VideoModeConfigType sHALVideoConfig;
  
  /* Get the pointer to the internal struct */
  psDisplayCtx  = DSIHOST_GET_DISPLAYCTX(eDeviceID, eDisplayID);
  
  sHALVideoConfig.eTrafficeMode = psDisplayCtx->eVideoTraffic;
  DSI_OSAL_MemCpy(&sHALVideoConfig.sDisplayInfo, &psDisplayCtx->sDisplayInfo,sizeof(DSI_DisplayInfoType));
  DSI_OSAL_MemCpy(&sHALVideoConfig.sVideoTiming, &psDisplayCtx->sVideoTiming,sizeof(DSI_VideoTimingType));
  DSI_OSAL_MemCpy(&sHALVideoConfig.sPowerOption, &psDisplayCtx->sPowerOption,sizeof(DSI_VPowerOptionType));

  /*Commit to DSI HW */
  HAL_DSI_VideoModeConfig(eDeviceID, &sHALVideoConfig);

  /* Setup the structure from the platform file */
  DSI_OSAL_MemCpy(&sHALVideoConfig.sDscVideoConfig, &psDisplayCtx->sDscCompressionInfo.sDscVidCntrlrConfig, sizeof(DSI_DscVideoModeConfigType));
    
  /* Enable video mode controller DSC related settings */
  if (sHALVideoConfig.sDscVideoConfig.uDscEnable)
  {
    HAL_DSI_VideoDscConfig(eDeviceID, &sHALVideoConfig.sDscVideoConfig);
  }
  
  /*Clear the dirty bits */
  psDisplayCtx->iDispDirtyBits   &= ~DSI_DIRTYBIT_DISPLAY_INFO ; 
  psDisplayCtx->iDispDirtyBits   &= ~DSI_DIRTYBIT_VIDEOMODE_TRAFFIC ; 
  psDisplayCtx->iDispDirtyBits   &= ~DSI_DIRTYBIT_VIDEOMODE_TIMING ; 
  psDisplayCtx->iDispDirtyBits   &= ~DSI_DIRTYBIT_VIDEOMODE_POWEROPTION ; 
  
  return eStatus;
}

/*-----------------------------------------------------------------------

dsiShared_CommitCmdModeConfig

DESCRIPTION:
    . 

----------------------------------------------------------------------- */
static DSI_StatusType dsiShared_CommitCmdModeConfig(  DSI_Device_IDType   eDeviceID, 
                                                     DSI_Channel_IDType  eDisplayID )
{ 
  DSI_StatusType           eStatus      = DSI_STATUS_SUCCESS;
  DSI_DisplayCtxType*     psDisplayCtx = NULL; 

  HAL_DSI_CmdModeConfigType       sHALCmdConfig;
  
  /* Get the pointer to the internal struct */
  psDisplayCtx  = DSIHOST_GET_DISPLAYCTX(eDeviceID, eDisplayID);
  
  sHALCmdConfig.uMaxInterleave  = psDisplayCtx->iMaxInterleave;
  sHALCmdConfig.uMDPDataType    = psDisplayCtx->iMDPDataType;
  sHALCmdConfig.uIdleTime       = psDisplayCtx->uIdleTime;
  DSI_OSAL_MemCpy(&sHALCmdConfig.sDisplayInfo, &psDisplayCtx->sDisplayInfo,sizeof(DSI_DisplayInfoType));

  /*Commit to DSI HW */
  HAL_DSI_CMDModeConfig(eDeviceID,&sHALCmdConfig);

  /* Setup the structure from platform file */
  DSI_OSAL_MemCpy(&sHALCmdConfig.sDscCmdConfig, &psDisplayCtx->sDscCompressionInfo.sDscCmdCntrlrConfig,sizeof(DSI_DscCmdModeConfigType));
  
  /* Enable the DSC compression settings */
  if(sHALCmdConfig.sDscCmdConfig.uDscEnable)
  {
    HAL_DSI_CMDDscConfig(eDeviceID, &(sHALCmdConfig.sDscCmdConfig));
  }
  
  /*Clear the dirty bits */
  psDisplayCtx->iDispDirtyBits   &= ~DSI_DIRTYBIT_CMDMODE_MISC; 
  
  return eStatus;
}
/*-----------------------------------------------------------------------

dsiShared_CommitVideoModeConfig

DESCRIPTION:
    . 

----------------------------------------------------------------------- */
static DSI_StatusType dsiShared_CommitExtTEConfig(  DSI_Device_IDType   eDeviceID, 
                                                   DSI_Channel_IDType  eDisplayID )
{
  DSI_StatusType           eStatus      = DSI_STATUS_SUCCESS;
  DSI_DisplayCtxType*     psDisplayCtx = NULL;
    
  HAL_DSI_ExtTEConfigType sHALExtTEconfig;
  DSI_ExtTEConfigType*    psExtTEConfig;

  /* Get the pointer to the internal struct */
  psDisplayCtx  = DSIHOST_GET_DISPLAYCTX(eDeviceID, eDisplayID);
  psExtTEConfig = &psDisplayCtx->sExtTEConfig;
  
  DSI_OSAL_MemZero( &sHALExtTEconfig, sizeof(HAL_DSI_ExtTEConfigType) );

  sHALExtTEconfig.eExtTEMode = psExtTEConfig->eExtTEMode;
  switch(sHALExtTEconfig.eExtTEMode)
  {
    case DSI_ExtTEMode_NONE:
      break;
    case DSI_ExtTEMode_VS_EDGE:
      sHALExtTEconfig.bExtTEFallingEdge = psExtTEConfig->sTEConfig.bVsyncFallingEdge;
      break;
    case DSI_ExtTEMode_VS_WIDTH:
      sHALExtTEconfig.bExtTEFallingEdge = psExtTEConfig->sTEConfig.iTEVsyncPulseWidth_Min;
      break;
    case DSI_ExtTEMode_VSHS_EDGE:
      sHALExtTEconfig.bExtTEFallingEdge = psExtTEConfig->sTEConfig.TE_VsHsEdge.bTEVsHsFallingEdge;
      sHALExtTEconfig.iTEHsyncTotal  = psExtTEConfig->sTEConfig.TE_VsHsEdge.iTEHsyncTotal;
      sHALExtTEconfig.iTEHSyncCount  = psExtTEConfig->sTEConfig.TE_VsHsEdge.iTEHSyncCount;
      break;
    case DSI_ExtTEMode_VSHS_WIDTH:
      sHALExtTEconfig.iTEVsyncPulseWidth_Min = psExtTEConfig->sTEConfig.TE_VsHPulse.iTEVsyncPulseWidth_Min;
      sHALExtTEconfig.iTEHsyncPulseWidth_Max = psExtTEConfig->sTEConfig.TE_VsHPulse.iTEHsyncPulseWidth_Max;
      sHALExtTEconfig.iTEHsyncTotal  = psExtTEConfig->sTEConfig.TE_VsHPulse.iTEHsyncTotal;
      sHALExtTEconfig.iTEHSyncCount  = psExtTEConfig->sTEConfig.TE_VsHPulse.iTEHSyncCount;
      break;
    default:
      eStatus = DSI_STATUS_FAILED_NOT_SUPPORTED;
    goto EXIT;
  }
  
  /*Commit to DSI HW */
  HAL_DSI_ExtTEPinConfig(eDeviceID, &sHALExtTEconfig);

  /*Clear the dirty bits */
  psDisplayCtx->iDispDirtyBits   &= ~DSI_DIRTYBIT_CMDMODE_EXTTE ; 

EXIT:
  return eStatus; 
}
/*-----------------------------------------------------------------------

dsiShared_CommitTriggerConfig

DESCRIPTION:
    . 

----------------------------------------------------------------------- */
static DSI_StatusType dsiShared_CommitTriggerConfig(  DSI_Device_IDType   eDeviceID, 
                                                     DSI_Channel_IDType  eDisplayID )
{ 
  DSI_StatusType           eStatus      = DSI_STATUS_SUCCESS;
  DSI_DisplayCtxType*     psDisplayCtx = NULL; 

  HAL_DSI_TriggerModeType         sHalTriggerMode;
  
  /* Get the pointer to the internal struct */
  psDisplayCtx  = DSIHOST_GET_DISPLAYCTX(eDeviceID, eDisplayID);
  
  if( DSI_ExtTEMode_NONE == psDisplayCtx->sExtTEConfig.eExtTEMode)
    sHalTriggerMode.bExtTEPin = FALSE;
  else
    sHalTriggerMode.bExtTEPin = TRUE;

  sHalTriggerMode.eImageTrigger = psDisplayCtx->eImageTrigger;
  sHalTriggerMode.eCMDTrigger   = psDisplayCtx->eCMDTrigger;

  /*TODO*/
  sHalTriggerMode.bOrderMdpDma  = FALSE;
  sHalTriggerMode.bOrderPHYMdp  = FALSE;
      
  if( DSI_ChannelID_CMD0 == psDisplayCtx->sDisplayInfo.eDisplayId)
    sHalTriggerMode.bTriggerStream1 = FALSE;
  else if(DSI_ChannelID_CMD1 == psDisplayCtx->sDisplayInfo.eDisplayId)
    sHalTriggerMode.bTriggerStream1 = TRUE;

  /*Commit to DSI HW */
  HAL_DSI_TriggerConfig(eDeviceID, &sHalTriggerMode);
  
  /*Clear the dirty bits */
  psDisplayCtx->iDispDirtyBits   &= ~DSI_DIRTYBIT_CMDMODE_TRIGGER ; 
  
  return eStatus;
}

/*-----------------------------------------------------------------------

dsiShared_CommitDeviceConfig

DESCRIPTION:
    . 

----------------------------------------------------------------------- */
static DSI_StatusType dsiShared_CommitDeviceConfig(DSI_Device_IDType   eDeviceID)
{ 
  DSI_StatusType          eStatus      = DSI_STATUS_SUCCESS;
  DSI_UserCtxType*        psSharedData = NULL; 

  HAL_DSI_DevicePropertyType      sHalDevConfig;
  HAL_DSI_LanePropertyType        sHalLaneConfig;
  
  DSI_OSAL_MemZero(&sHalDevConfig, sizeof(HAL_DSI_DevicePropertyType));
  DSI_OSAL_MemZero(&sHalLaneConfig, sizeof(HAL_DSI_LanePropertyType));  

  /* Get the pointer to the internal struct */
  psSharedData  = DSIHOST_GET_USERCTX(eDeviceID);
  
  sHalDevConfig.bLittleEndian     = psSharedData->sDevConfig.bLittleEndian;
  sHalDevConfig.bNonCompliantMode = psSharedData->sDevConfig.bNonCompliantMode;
  sHalDevConfig.bCRCCheckEn       = psSharedData->sDevConfig.bEnableCRCCheck;
  sHalDevConfig.bECCCheckEn       = psSharedData->sDevConfig.bEnableECCCheck;

  if(psSharedData->iDeviceRef > 0)
  {
    if (psSharedData->bCPhyMode)
    {
      sHalDevConfig.bClockLnEn        = FALSE;
    }
    else
    {
      sHalDevConfig.bClockLnEn        = TRUE;
    }

    sHalDevConfig.bDSICtrlEN        = TRUE;
  
    if(psSharedData->eDevLane[0] != DSI_LaneMap_NONE)
      sHalDevConfig.bDataLn0En      = TRUE;
    if(psSharedData->eDevLane[1] != DSI_LaneMap_NONE)
      sHalDevConfig.bDataLn1En      = TRUE;
    if(psSharedData->eDevLane[2] != DSI_LaneMap_NONE)
      sHalDevConfig.bDataLn2En      = TRUE;
    if(psSharedData->eDevLane[3] != DSI_LaneMap_NONE)
      sHalDevConfig.bDataLn3En      = TRUE;

    if (psSharedData->iVideoModeRef > 0 )
    {
      sHalDevConfig.bVideoModeEn = TRUE;
    }
    
    if (psSharedData->iCmdModeRef > 0 )
    {
      sHalDevConfig.bCMDModeEn = TRUE;
    }
  }

  /*Configure DSI Lanes*/
  HAL_DSI_LaneConfig(eDeviceID, &sHalLaneConfig);
  
  /*Commit to DSI HW */
  HAL_DSI_DeviceConfig(eDeviceID, &sHalDevConfig);  

  if (psSharedData->bCPhyMode)
  {
    HAL_DSI_CPhyModeEnable(eDeviceID, TRUE);
  }
  
  /*Clear the dirty bits */
  psSharedData->uDirtyBits   &= ~DSI_DIRTYBIT_DEVICE_CONFIG; 
  
  return eStatus;
}
/*-----------------------------------------------------------------------

dsiShared_CommitDeviceConfig

DESCRIPTION:
    . 

----------------------------------------------------------------------- */
DSI_StatusType dsiShared_CommitLaneConfig(DSI_Device_IDType   eDeviceID)
{ 
  DSI_StatusType                  eStatus            = DSI_STATUS_SUCCESS;
  DSI_UserCtxType*                psSharedData       = NULL; 
  HAL_DSI_PhyPropertyParamsType   sPhyPropertyType;
  uint32                          uCount;
  
  DSI_OSAL_MemZero((void *)&sPhyPropertyType, sizeof(HAL_DSI_PhyPropertyParamsType));

  /* Get the pointer to the internal struct */
  psSharedData  = DSIHOST_GET_USERCTX(eDeviceID);
  
  /*Commit to DSI Phy HW */
  for (uCount=0; uCount<(DSI_LaneMap_MAX-1); uCount++)
  {
    sPhyPropertyType.aLaneMap[uCount] = psSharedData->eDevLane[uCount];
  }
  HAL_DSI_PhySetProperty(eDeviceID, HAL_DSI_PHY_PROPERTYID_LANEMAP, &sPhyPropertyType);
    
  /*Clear the dirty bits */
  psSharedData->uDirtyBits   &= ~DSI_DIRTYBIT_DEVICE_LANE; 
  
  return eStatus;
}


/***************************************************************************************
 * DSI Interrupts to be supported
 */

/*-----------------------------------------------------------------------

dsiShared_WaitforInterrupt

DESCRIPTION:
   This API is called to wait for interrupt or interrupts to occur

----------------------------------------------------------------------- */
static DSI_StatusType dsiShared_WaitforInterrupt(DSI_Device_IDType      eDeviceId,
                                                 uint32                 uIntFlags,
                                                 uint32                 uWaitTimeUs)
{
    DSI_StatusType  eStatusRet = DSI_STATUS_SUCCESS;
    
    if (0 != uIntFlags)
    {
        uint32 uDsiIsrStatus        = 0;
        uint32 uErrorCode           = 0;
        uint32 uLoopCount;

        eStatusRet = DSI_STATUS_FAILED_TIMEOUT;
        
        for (uLoopCount=0;uLoopCount<(uWaitTimeUs/DSI_POLLING_CYCLE);uLoopCount++)
        {
           DSI_OSAL_SleepUs(DSI_POLLING_CYCLE);
            
           HAL_DSI_GetInterruptStatus(eDeviceId, &uDsiIsrStatus, &uErrorCode );

           // Mask off flags
           uDsiIsrStatus &= uIntFlags;

           if (uDsiIsrStatus == uIntFlags)
           {
                HAL_DSI_ClrInterruptStatus(eDeviceId, uDsiIsrStatus);
                eStatusRet = DSI_STATUS_SUCCESS;
                break;
           }
        }
    }


    return eStatusRet;
}


/* -----------------------------------------------------------------------
**
** FUNCTION: dsiShared_GetLastHostErrorCode()
**
** DESCRIPTION:
**   This API will read the DSI host error code
**
** ----------------------------------------------------------------------- */
DSI_StatusType dsiShared_GetLastHostErrorCode(DSI_Device_IDType eDeviceId, uint32 *puHostErrorCode)
{
    DSI_StatusType             eStatus       = DSI_STATUS_SUCCESS;
    uint32                    uError;

    /* read DSI host error */
    HAL_DSI_GetHostErrorStatus(eDeviceId, puHostErrorCode, &uError);
    /* clear DSI host error */
    HAL_DSI_ClrHostErrorStatus(eDeviceId, *puHostErrorCode);

    return eStatus;
}



/* -----------------------------------------------------------------------
**
** FUNCTION: dsiShared_GetLastPanelErrorCode()
**
** DESCRIPTION:
**   This API will read the DSI panel error code
**
** ----------------------------------------------------------------------- */
DSI_StatusType dsiShared_GetLastPanelErrorCode(DSI_Device_IDType eDeviceId, uint32 *puPanelErrorCode)
{
    DSI_StatusType             eStatus       = DSI_STATUS_SUCCESS;

    /* read DSI panel error */
    HAL_DSI_GetPanelErrorStatus(eDeviceId, puPanelErrorCode);
    /* clear DSI panel error */
    HAL_DSI_ClrPanelErrorStatus(eDeviceId, *puPanelErrorCode);

    return eStatus;
}



/* -----------------------------------------------------------------------
**
** FUNCTION: dsShared_GetPackDataSize()
**
** DESCRIPTION:
**   This API Get the final size of embedded packet size.
**
** INPUT:
**   TBD
**
** ----------------------------------------------------------------------- */
void dsShared_GetPackDataSize(DSI_Datatype        iCmdDataType,
                                    uint32              iPayloadByteSize,
                                    uint32*             piPackDataSize)
{

  if( iCmdDataType == DSI_CMDMODE_DT_NULLPACKET )
  {
    /* Null Packet Payload is generated by DSI HW */
    *piPackDataSize = HAL_DSI_EMBEDDED_HEADER_SIZE;
  }
  else if( DSI_DATATYPE_LONGPACKET  == (iCmdDataType &0x0F) ||
           DSI_DATATYPE_VIDEOPACKET == (iCmdDataType &0x0F) ||
           DSI_DATATYPE_PPSPACKET   == (iCmdDataType &0x0F))
  {
    /* DMA offset has to be 64-bit aligned and packet has to start at
    32-bit boundary*/
    *piPackDataSize =  (((HAL_DSI_EMBEDDED_HEADER_SIZE+iPayloadByteSize+3)>>2)<<2);

  }
  else
  {
    /* Short packet*/
    *piPackDataSize = HAL_DSI_EMBEDDED_HEADER_SIZE;
  }
}


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
void dsShared_PackData(HAL_DSI_PackDataType   *psPackData)
{
  uint32    iPacketType         = HAL_DSI_EMBEDDED_SHORTPACKET_TYPE;

  /*Clear the header field of the embedded packet */
  DSI_OSAL_MemZero( psPackData->pOutputData, HAL_DSI_EMBEDDED_HEADER_SIZE);

  /* 24:31 bits */
  if( DSI_DATATYPE_LONGPACKET  != (psPackData->uCmdDataType &0x0F)&&
      DSI_DATATYPE_VIDEOPACKET != (psPackData->uCmdDataType &0x0F)&&
      DSI_DATATYPE_PPSPACKET   != (psPackData->uCmdDataType &0x0F))
    iPacketType = HAL_DSI_EMBEDDED_SHORTPACKET_TYPE;  
  else
    iPacketType = HAL_DSI_EMBEDDED_LONGPACKET_TYPE;  

  psPackData->pOutputData[3] |= ((iPacketType <<
      HAL_DSI_CMDMODE_DMA_PACKET_SL_SHFT) &
      HAL_DSI_CMDMODE_DMA_PACKET_SL_BMSK);
  psPackData->pOutputData[3] |= ((psPackData->bLastPacket <<
      HAL_DSI_CMDMODE_DMA_PACKET_QW_SHFT) &
      HAL_DSI_CMDMODE_DMA_PACKET_QW_BMSK);
  psPackData->pOutputData[3] |= ((psPackData->bBTARequest <<
      HAL_DSI_CMDMODE_DMA_PACKET_BTA_SHFT) &
      HAL_DSI_CMDMODE_DMA_PACKET_BTA_BMSK);
  
  /* 16:23 bits */
  psPackData->pOutputData[2] |= ((psPackData->eDisplayVC <<
      HAL_DSI_CMDMODE_DMA_PACKET_VC_SHFT) &
      HAL_DSI_CMDMODE_DMA_PACKET_VC_BMSK);
  psPackData->pOutputData[2] |= ((psPackData->uCmdDataType <<
      HAL_DSI_CMDMODE_DMA_PACKET_DT_SHFT) &
      HAL_DSI_CMDMODE_DMA_PACKET_DT_BMSK);

  /**/
  if(psPackData->uCmdDataType == DSI_CMDMODE_DT_NULLPACKET)
  {
    /* payload word count */
    psPackData->pOutputData[1] = (psPackData->uPayloadLen & 0xff00) >> 8;
    psPackData->pOutputData[0] = psPackData->uPayloadLen&0x00ff;
  }
  else if(HAL_DSI_EMBEDDED_SHORTPACKET_TYPE == iPacketType)
  {
    /* Payload length for some short packets can be set to zero which is valid. In that case, 
       don't do any copy from the input buffer. */
    if (psPackData->uPayloadLen > 0)
    {
    DSI_OSAL_MemCpy(&(psPackData->pOutputData[0]), 
                      psPackData->pInputData,
                      psPackData->uPayloadLen);
    }
  }
  else
  {
    /* payload word count */
    psPackData->pOutputData[1] = (psPackData->uPayloadLen & 0xff00) >> 8;
    psPackData->pOutputData[0] = psPackData->uPayloadLen&0x00ff;

    /* Payload length for some long packets can be set to zero which is valid. In that case, 
       don't do any copy from the input buffer. */
    if (psPackData->uPayloadLen > 0)
    {
      DSI_OSAL_MemCpy(&(psPackData->pOutputData[4]), 
                      psPackData->pInputData,
                      psPackData->uPayloadLen);
    }
  }
}


/* -----------------------------------------------------------------------
**
** FUNCTION: dsiShared_SetDeviceConfig()
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
                                HAL_DSI_PhyConfigType *psPhyConfig)
{
  if ((NULL != psDeviceConfig) &&
      (NULL != psPhyConfig))
  {
    // These phy parameters are retried from settings passed during DSI  device open
    psPhyConfig->eDeviceId           = psDeviceConfig->eDeviceID;
    psPhyConfig->bCPhyMode           = psDeviceConfig->bCPhyMode;
    psPhyConfig->bDCDCMode           = psDeviceConfig->bPhyDCDCMode;
    psPhyConfig->uClockStrengthHS    = psDeviceConfig->uClockStrengthHS;
    psPhyConfig->uDataStrengthHS     = psDeviceConfig->uDataStrengthHS;
    psPhyConfig->uDataStrengthLP     = psDeviceConfig->uDataStrengthLP;

    if (TRUE == psDeviceConfig->bDualDsi)
    {
      psPhyConfig->ePLLConfigSource  = HAL_DSI_PLL_CONFIG_SPLIT_SOURCE;
	}
    else
    {
      psPhyConfig->ePLLConfigSource  = HAL_DSI_PLL_CONFIG_INDEPENDENT_SOURCE;
    }
  }
}


#ifdef __cplusplus
}
#endif
