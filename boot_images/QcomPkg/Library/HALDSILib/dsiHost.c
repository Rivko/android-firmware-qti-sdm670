/*=============================================================================

  File: dsiHost.c
  

  Copyright (c) 2010-2018 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/


/*============================================================================
*                         INCLUDE FILES
============================================================================*/
#include "dsiHostShared.h"
#include "dsiHostSystem.h"

/* --- DO NOT ADD HEADER FILES HERE! --- */

#ifdef __cplusplus
extern "C" {
#endif


/****************************************************************************

DSI_Device_Open()

Description:
    This function must be called before any other DSI host function called
    which using the device specified by eDeviceID. The first call to this 
    API will initialize all internal DSI device components, all subsequent
    calls by the same user will fail. This API will return a valid device
    handle    to the user if no errors are detected. 

****************************************************************************/
DSI_StatusType DSI_Device_Open(
    DSI_DeviceConfigType     *psDeviceConfig,
    DSI_HandleType           *phDevice)
{
  DSI_StatusType   eStatus   = DSI_STATUS_SUCCESS;
  
  *phDevice = NULL;

  /* Check Parameters                                   */
  DSI_NULLPOINTER_AND_EXIT(psDeviceConfig,eStatus, EXIT);

  /* Initialize Driver Internal Data                    */
  eStatus = dsiShared_InitDsiDriver(psDeviceConfig);

  /*if other panel has initialized the driver, it will return 
   DSI_STATUS_FAILED_DRIVER_ALREADY_INITIALIZED     */
  if ((eStatus == DSI_STATUS_SUCCESS) ||
      (eStatus == DSI_STATUS_FAILED_DRIVER_ALREADY_INITIALIZED))
  {
    /* Save the current Device setting, if no conflict  */
    eStatus   = dsiShared_setDeviceConfig(psDeviceConfig);
    
    DSI_ERROR_AND_EXIT(eStatus, EXIT);
    
    /*Generate Handle to this Device                    */
    *phDevice = dsiShared_DeviceId2Handle(psDeviceConfig->eDeviceID);  
  }

EXIT:
  return eStatus;
} /*  DSI_Device_Open()  */



/* -----------------------------------------------------------------------

DSI_Device_Close

DESCRIPTION:
   This API is called to de-initialize the corresponding resources that 
   is initialized when DSI_Device_Open is called. It will return an error
   if the device handle is invalid. All resources will be de-initialized
   when the last DSI device handle is released. User should ensure that
   any display modules opened on this device should be closed prior to 
   closing the device module, otherwise corresponding error will be returned
   to the user. Upon an invalid device handle, this call will also return
   an error. 

   ----------------------------------------------------------------------- */
DSI_StatusType DSI_Device_Close(
    DSI_HandleType            hDevice,
    uint32                    uFlags)
{
  DSI_StatusType     eStatus         = DSI_STATUS_SUCCESS;

  /* Clr Device config for this panel               */
  eStatus   = dsiShared_ClrDeviceConfig( hDevice, uFlags );
  
  /* If dsiShared_ClrDeviceConfig() return success  */
  if (DSI_STATUS_FAILED_RESOURCE_NOT_READY != eStatus)
  {
    /* Try to Close DSI driver                      */
    eStatus   = dsiShared_DeInitDsiDriver();  
  }

  return eStatus;
} /*DSI_Device_Close()*/

/* -----------------------------------------------------------------------

DSI_Display_Open

DESCRIPTION:
   Initialize DSI Host which is panel specific. This API call initializes 
   a QDI display module and must be called prior to controlling the display 
   module. The parameters of this API should not change at run-time. However, 
   on DSI1.0, if two different type I DSI panels are attached to the same 
   lane, some parameters such as pixel format are different. Those registers 
   need to be re-configured before the start of each virtual channel image 
   transmission. Those reconfigurations are done with DSI Display property 
   functions.
   
   ----------------------------------------------------------------------- */
DSI_StatusType DSI_Display_Open(
    DSI_HandleType            hDevice,
    DSI_DisplayConfigType    *psDisplayConfig,
    DSI_HandleType           *phDisplay
    )
{
  DSI_StatusType     eStatus         = DSI_STATUS_SUCCESS;

  /* Check Parameters                                   */
  DSI_NULLPOINTER_AND_EXIT(psDisplayConfig, eStatus, EXIT);

  /*Either command mode or Video mode should be enabled */
  if ((FALSE == psDisplayConfig->sCmdModeConfg.bEnable)&&
      (FALSE == psDisplayConfig->sVideoModeConfg.bEnable))
  {
    eStatus = DSI_STATUS_FAILED_NOT_SUPPORTED;
  }
  else
  {
    if (DSI_DISPLAY_FLAGS_LOAD_DRIVER_STATE & psDisplayConfig->uFlags)
    {
      /* Only load the driver state do not touch any hardware */
    }
    else
    {
      uint32 t1 = 0;
      uint32 t2 = 0;

      /* Do dynamic regresh calibration here, if uNumOfDynamicFreq is 0, this function actually do nothing*/
      t1 = MDP_GetTimerCountUS();
      if(DSI_STATUS_SUCCESS == (eStatus = dsiSystem_FrequencyCalibration(hDevice, psDisplayConfig)))
      {
        t2 = MDP_GetTimerCountUS();
        DSI_Log_Message(DSI_LOGLEVEL_WARN, "Dynamic refresh calibration: DeviceId=%d, cost_time=%d us\n", (uint32)dsiShared_DeviceHandle2Id(hDevice), t2 - t1);
      }
      
      /* Setup the DSI PLL */
      eStatus   = dsiSystem_ConfigDevicePll(hDevice, psDisplayConfig);  
      DSI_ERROR_AND_EXIT(eStatus, EXIT);
    }

    /* Save Display Config */
    eStatus   = dsiShared_setDisplayConfig(hDevice, psDisplayConfig);  
    DSI_ERROR_AND_EXIT(eStatus, EXIT);

    /*Generate Handle to this Display ID */
    *phDisplay = dsiShared_DisplayID2Handle(hDevice,psDisplayConfig->sDisplayInfo.eDisplayId);
  }

EXIT:
  return eStatus;
}/* DSI_Display_Open() */

/* -----------------------------------------------------------------------

FUNCTION: DSI_Display_Close

DESCRIPTION:
    This API is called to de-initialize all the corresponding display 
    resources associated with the display module. Upon an invalid display 
    handle, this call will also return an error. 

----------------------------------------------------------------------- */
DSI_StatusType DSI_Display_Close(
    DSI_HandleType            hDisplay)
{
  return dsiShared_ClrDisplayConfig(hDisplay);
}
/* -----------------------------------------------------------------------

DSI_Display_ClientWrite

DESCRIPTION:
    This API allows users to send the DSI command out to DSI Panels. 

----------------------------------------------------------------------- */
DSI_StatusType DSI_Display_ClientWrite(
    DSI_HandleType            hDisplay,
    DSI_WritePacketType      *psWritePacket
    )
{
  DSI_StatusType      eStatus       = DSI_STATUS_SUCCESS;
  uint32              uRetryCount   = 0;
  DSI_Device_IDType   eDeviceId;
  DSI_Channel_IDType  eDisplayId;

  /* Check Parameters                                   */
  DSI_NULLPOINTER_AND_EXIT(psWritePacket, eStatus, EXIT);

  dsiShared_Handle2DisplayID(hDisplay,&eDeviceId,&eDisplayId);  

  uRetryCount = (DSI_DISPLAY_WRITEFLAG_NORETRY & psWritePacket->uFlags)?1:DSI_WRITE_RETRY_COUNT;

  // Support retry mechanism to retry the packet on failure.
  do
  {
      /*Write to panel                                      */
      eStatus = dsiShared_ClientWrite(hDisplay, psWritePacket);  

      // Decrement the retry count
      uRetryCount--;

      if ((DSI_STATUS_FAILED_TIMEOUT == eStatus) && (uRetryCount > 0))
      {
         DSI_Log_Message(DSI_LOGLEVEL_WARN, "DSIDriver: Packet Timeout, Resending DSI Packet.\n");
      }

  } while ((DSI_STATUS_FAILED_TIMEOUT == eStatus) && (uRetryCount > 0));

  /* read DSI host error */
  dsiShared_GetLastHostErrorCode(eDeviceId, &(psWritePacket->uHostError));

EXIT:
  return eStatus;
}

/* -----------------------------------------------------------------------

DSI_Display_ClientRead

DESCRIPTION:
    This API allows users to send the DSI read command out to DSI Panels, 
    and wait until it gets the data from panel. 

----------------------------------------------------------------------- */
DSI_StatusType DSI_Display_ClientRead(
    DSI_HandleType            hDisplay,
    DSI_ReadPacketType       *psReadPacket
    )
{
  DSI_StatusType      eStatus       = DSI_STATUS_SUCCESS;
  DSI_Channel_IDType  eDisplayId    = DSI_ChannelID_NONE;
  DSI_Device_IDType   eDeviceId;
  
  /* Check Parameters                                   */
  DSI_NULLPOINTER_AND_EXIT(psReadPacket, eStatus, EXIT);

  dsiShared_Handle2DisplayID(hDisplay,&eDeviceId,&eDisplayId); 

  /*Write to panel                                      */
  eStatus   = dsiShared_ClientRead(hDisplay, psReadPacket);  

  /* read DSI host error */
  dsiShared_GetLastHostErrorCode(eDeviceId, &(psReadPacket->uHostError));
  /* read DSI panel error */
  dsiShared_GetLastPanelErrorCode(eDeviceId, &(psReadPacket->uPanelError));

  if(DSI_STATUS_SUCCESS == eStatus)
  {
    /* Remove the Lane Fifo Overflow errors if any as these are not relevant if the Transaction is successful */
    psReadPacket->uHostError &= ~(HAL_DSI_ERROR_INT_DLN0_HS_FIFO_OVERFLOW |
                                  HAL_DSI_ERROR_INT_DLN1_HS_FIFO_OVERFLOW |
                                  HAL_DSI_ERROR_INT_DLN2_HS_FIFO_OVERFLOW |
                                  HAL_DSI_ERROR_INT_DLN3_HS_FIFO_OVERFLOW );
  }

EXIT:
  return eStatus;
}

/* -----------------------------------------------------------------------

DSI_Display_Commit

DESCRIPTION:
    This API allows users to trigger the command DMA out by Software. 

** ----------------------------------------------------------------------- */
DSI_StatusType DSI_Display_Commit(
    DSI_HandleType            hDisplay,
    uint32                    iFlag)
{
  return dsiShared_Comit(hDisplay, iFlag);
}

  

/* -----------------------------------------------------------------------

DSI_Display_SetProperty

DESCRIPTION:
    TBD

** ----------------------------------------------------------------------- */
DSI_StatusType DSI_Display_SetProperty(
    DSI_HandleType            hDisplay,
    DSI_DisplayPropertyType   eProperty,
    DSI_PropertyParamsType   *pPropertyData)
{
  DSI_StatusType        eStatus         = DSI_STATUS_SUCCESS;
  DSI_Device_IDType     eDeviceId       = DSI_DeviceID_0;
  DSI_Channel_IDType    eDisplayId      = DSI_ChannelID_NONE;
  DSI_UserCtxType      *psSharedData    = NULL; 
  DSI_DisplayCtxType   *psDisplayCtx    = NULL; 

  /* Check Parameters                                   */
  DSI_NULLPOINTER_AND_EXIT(pPropertyData, eStatus, EXIT);

  dsiShared_Handle2DisplayID(hDisplay,&eDeviceId,&eDisplayId);
  
  psSharedData  = DSIHOST_GET_USERCTX(eDeviceId);
  psDisplayCtx  = DSIHOST_GET_DISPLAYCTX(eDeviceId, eDisplayId);
  
  switch(eProperty)
  {
    case DSI_DISPLAY_PROPERTYID_DISPINFO:
    {
      if( DSI_ChannelID_CMD0 == psDisplayCtx->sDisplayInfo.eDisplayId ||
          DSI_ChannelID_CMD1 == psDisplayCtx->sDisplayInfo.eDisplayId )
      {
        DSI_OSAL_MemCpy(& psDisplayCtx->sDisplayInfo, &pPropertyData->sDisplayInfo,
             sizeof(DSI_DisplayInfoType));
        psDisplayCtx->iDispDirtyBits   |= DSI_DIRTYBIT_DISPLAY_INFO|DSI_DIRTYBIT_CMDMODE_MISC; 
      }
      break;
    }
    case DSI_DISPLAY_PROPERTYID_LP11:
    {
        HAL_DSI_DevicePropertyType      sHalDevConfig;
        
        DSI_OSAL_MemZero(&sHalDevConfig, sizeof(HAL_DSI_DevicePropertyType));
        
        sHalDevConfig.bLittleEndian     = psSharedData->sDevConfig.bLittleEndian;
        sHalDevConfig.bNonCompliantMode = psSharedData->sDevConfig.bNonCompliantMode;
        sHalDevConfig.bCRCCheckEn       = psSharedData->sDevConfig.bEnableCRCCheck;
        sHalDevConfig.bECCCheckEn       = psSharedData->sDevConfig.bEnableECCCheck;
        
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

        // enable command mode
        if (psSharedData->iCmdModeRef > 0 )
        {
          sHalDevConfig.bCMDModeEn = TRUE;
        }
        
        // enable video mode
        if (psSharedData->iVideoModeRef > 0)
        {
            sHalDevConfig.bVideoModeEn = TRUE;
        }

        /*Commit to DSI HW */
        HAL_DSI_DeviceConfig(eDeviceId, &sHalDevConfig);  

        if (psSharedData->bCPhyMode)
           HAL_DSI_CPhyModeEnable(eDeviceId, 1);
    }
    break;
    case DSI_DISPLAY_PROPERTYID_CALCREFRESH:
    {
        uint32 uRefreshRate = 0;
        uint32 uTotalPixels = (pPropertyData->sDisplayInfo.iVisHeightPx + pPropertyData->sDisplayInfo.iBlankHeightPx) *
                              (pPropertyData->sDisplayInfo.iVisWidthPx + pPropertyData->sDisplayInfo.iBlankWidthPx);
        uint32 uBpp = dsiShared_GetBitsPerPixel(pPropertyData->sDisplayInfo.eColorFormat);
        
        // Refresh rate = (lanes * bitclock frequency) / (total pixels * bpp);
        if ((uTotalPixels > 0) && (uBpp > 0))
        {
            uRefreshRate = (pPropertyData->sDisplayInfo.uNumLanes * pPropertyData->sDisplayInfo.uBitClockFreq) /
                           (uTotalPixels * uBpp);
        }

        pPropertyData->sDisplayInfo.uRefreshRate = uRefreshRate;
    }
    break;
    case DSI_DISPLAY_PROPERTYID_IDLE:
    {   
        //Call DSI PHY function to disable the DSI phy PLL.
        HAL_DSI_PhyDisable(eDeviceId);
    }    
    break;
    case DSI_DISPLAY_PROPERTYID_ACTIVE:     //Enable  DSI PLL clocks.
    {        
        HAL_DSI_PhyConfigType            sDsiPhyConfig;
        HAL_DSI_PhyConfigInfoType        sDsiPhyConfigInfo;
        uint32                           uBitsPerPixel      = dsiShared_GetBitsPerPixel(pPropertyData->sPhyParams.sDisplayInfo.eColorFormat);
        
        /* Setup the PhyStructure */
        DSI_OSAL_MemZero(&sDsiPhyConfig, sizeof(HAL_DSI_PhyConfigType));
        DSI_OSAL_MemZero(&sDsiPhyConfigInfo, sizeof(HAL_DSI_PhyConfigInfoType));
      

        // Setup Phy configuration 
        sDsiPhyConfig.eDeviceId           = eDeviceId;
        sDsiPhyConfig.uBitsPerPixel       = uBitsPerPixel;
        sDsiPhyConfig.uNumOfDataLanes     = pPropertyData->sPhyParams.sDisplayInfo.uNumLanes;
        sDsiPhyConfig.uDesiredBitClkFreq  = dsiShared_CalculateDesiredBitClk(&pPropertyData->sPhyParams.sDisplayInfo, uBitsPerPixel);  /* desired DSI PLL bit clk freq in Hz */
        sDsiPhyConfig.uConfigFlags        = 0;       
        sDsiPhyConfig.uEscapeFreq         = pPropertyData->sPhyParams.sDisplayInfo.uEscapeFreq;
        sDsiPhyConfig.pPreDefinedTimings  = &pPropertyData->sPhyParams.sTimingOverride;
        

        // Setup common Phy parameters 
        dsiShared_setPhyParameters(&psSharedData->sDevConfig, &sDsiPhyConfig);
        
        /* enable DSI PLL and Phy.
         *
         * HAL_DSI_PhyPllSetup() returns PClk MND divider ratio for Clock Control block (CC) in the form of numerator and denominator;
         * also returned are frequencies of PLL output clocks, which can be used by upper layer to decided MND values.
         */
        if (HAL_MDSS_STATUS_SUCCESS != HAL_DSI_PhySetup(eDeviceId, &sDsiPhyConfig))
        {
           eStatus = DSI_STATUS_FAILED_NOT_SUPPORTED;
           DSI_Log_Message(DSI_LOGLEVEL_WARN, "DSIDriver: DSI%d Phy configuration failed.\n", eDeviceId);
        }
         
        if (HAL_MDSS_STATUS_SUCCESS != HAL_DSI_PhyPllSetup(&sDsiPhyConfig, &sDsiPhyConfigInfo))
        {
           eStatus = DSI_STATUS_FAILED_NOT_SUPPORTED;
           DSI_Log_Message(DSI_LOGLEVEL_WARN, "DSIDriver: DSI%d Pll configuration failed.\n", eDeviceId);
        }

    }
    break;
    case DSI_DISPLAY_PROPERTYID_INTCLEAR:
      {
        uint32 uHALIntList = dsiShared_DsiInt2HALInt(pPropertyData->uInterruptList);

        HAL_DSI_ClrInterruptStatus(eDeviceId, uHALIntList); 
      }
      break;    
    case DSI_DISPLAY_PROPERTYID_INTENABLE:
      {
        uint32 uIntList;
        uint32 uHALIntList = dsiShared_DsiInt2HALInt(pPropertyData->uInterruptList);

        HAL_DSI_GetInterruptsEnabled(eDeviceId, &uIntList);        

        // Add new interrupts
        uIntList |= uHALIntList;

        HAL_DSI_SetInterruptsEnabled(eDeviceId, uIntList); 
      }
      break;
    case DSI_DISPLAY_PROPERTYID_INTDISABLE:
      {
        uint32 uIntList;
        uint32 uHALIntList = dsiShared_DsiInt2HALInt(pPropertyData->uInterruptList);
  
        HAL_DSI_GetInterruptsEnabled(eDeviceId, &uIntList);

        // Mask off interrupts
        uIntList &= ~uHALIntList;
        
        HAL_DSI_SetInterruptsEnabled(eDeviceId, uIntList ); 
                   
      }
      break;
    case DSI_DISPLAY_PROPERTYID_LANESTATE:
      {
        HAL_DSI_LanePropertyType        sLaneConfig;
        HAL_DSI_PhyPropertyParamsType   sPhyPropertyType;

        DSI_OSAL_MemZero((void *)&sLaneConfig, sizeof(HAL_DSI_LanePropertyType));
        DSI_OSAL_MemZero((void *)&sPhyPropertyType, sizeof(HAL_DSI_PhyPropertyParamsType));

        /*
         * Program sequence for ULPS
         *
         * 1. To enter ULPS
         *   a. Write 1 to DSI_LANE_CTRL.*_ULPS_REQUEST.
         *   b. When the lane is in ULPS, DSI_LANE_STATUS.*_ULPS_ACTIVE_NOT is 0.
         *
         * 2. To exit ULPS
         *   a. Check that DSI_LANE_STATUS.*_ULPS_ACTIVE_NOT is 0 (optional).
         *   b. Write 1 to DSI_LANE_CTRL.*_ULPS_EXIT.
         *   c. Check that DSI_LANE_STATUS.*_ULPS_ACTIVE_NOT is 1 (optional).
         *   d. After wakeup (at least 1 ms as required by the MIPI PHY specification), write 0 to DSI_LANE_CTRL.*_ULPS_REQUEST.
         *   e. Write 0 to DSI_LANE_CTRL.*_ULPS_EXIT (this MUST be done AFTER Step d to avoid metastability issues).
         */

        /* DSI_LANE_OP_CLOCK_HS_FORCE_REQUEST & DSI_LANE_OP_CLOCK_HS_FORCE_REQUEST_CLEAR are supported by DSI controller
        and the rest are supported by PHY */
        
        // configure lane into LP00 state
        if(DSI_LANE_STATE_LP00 == pPropertyData->eLaneState)
        {
            sPhyPropertyType.sLaneConfig.eClockLaneOp = DSI_LANE_OP_ULPS_REQUEST;
            sPhyPropertyType.sLaneConfig.eDataLaneOp  = DSI_LANE_OP_ULPS_REQUEST;
            HAL_DSI_PhySetProperty(eDeviceId, HAL_DSI_PHY_PROPERTYID_LANECONFIG, &sPhyPropertyType);
        }
        // configure lane into LP11 state
        else if(DSI_LANE_STATE_LP11 == pPropertyData->eLaneState)
        {
          // (1) exit ULPS
          sPhyPropertyType.sLaneConfig.eClockLaneOp = DSI_LANE_OP_ULPS_EXIT;
          sPhyPropertyType.sLaneConfig.eDataLaneOp  = DSI_LANE_OP_ULPS_EXIT;
          HAL_DSI_PhySetProperty(eDeviceId, HAL_DSI_PHY_PROPERTYID_LANECONFIG, &sPhyPropertyType);

          // (2) delay 1.5ms because of minimum 1ms Twakeup in the spec.
          DSI_OSAL_SleepUs(DSI_LANE_T_WAKEUP_TIME);

          // (3) clear ULPS_REQUEST
          sPhyPropertyType.sLaneConfig.eClockLaneOp = DSI_LANE_OP_ULPS_REQUEST_CLEAR;
          sPhyPropertyType.sLaneConfig.eDataLaneOp  = DSI_LANE_OP_ULPS_REQUEST_CLEAR;
          HAL_DSI_PhySetProperty(eDeviceId, HAL_DSI_PHY_PROPERTYID_LANECONFIG, &sPhyPropertyType);

          // (4) clear ULPS_EXIT
          // this has to be done after (3) to avoid metastability issues
          sPhyPropertyType.sLaneConfig.eClockLaneOp = DSI_LANE_OP_ULPS_EXIT_CLEAR;
          sPhyPropertyType.sLaneConfig.eDataLaneOp  = DSI_LANE_OP_ULPS_EXIT_CLEAR;
          HAL_DSI_PhySetProperty(eDeviceId, HAL_DSI_PHY_PROPERTYID_LANECONFIG, &sPhyPropertyType);
        }
        else if (DSI_LANE_STATE_CLOCK_HS == pPropertyData->eLaneState)
        {
          /* Force clock lane in to HS */
          sLaneConfig.eClockLaneOp = DSI_LANE_OP_CLOCK_HS_FORCE_REQUEST;
        
          HAL_DSI_LaneConfig(eDeviceId, &sLaneConfig);    
        }
        else if (DSI_LANE_STATE_CLOCK_LP11 == pPropertyData->eLaneState)
        {
          /* Force clock lane into idle */
          sLaneConfig.eClockLaneOp = DSI_LANE_OP_CLOCK_HS_FORCE_REQUEST_CLEAR;

          HAL_DSI_LaneConfig(eDeviceId, &sLaneConfig);
        }
      }
      break;      
    case DSI_DISPLAY_PROPERTYID_RESETCORE:  
      {
          /* SW Reset the DSI controller */
          HAL_DSI_Reset(eDeviceId);
      }
      break;
    case DSI_DISPLAY_PROPERTYID_ENABLECORE:  
      {
        if(TRUE == pPropertyData->sEnableCore.bEnable)
        {
          /* Enable DSI Controller */
          HAL_DSI_Enable_CTRL(eDeviceId, pPropertyData->sEnableCore.eHWBlock);  
        }
        else
        {
          /* Disable DSI Controller */
          HAL_DSI_Disable_CTRL(eDeviceId, pPropertyData->sEnableCore.eHWBlock);   
        }
      }
      break;
    case DSI_DISPLAY_PROPERTYID_PLL_CTRL:
      {
        /* Enable/Disable the DSI PLL power  */
        if(HAL_MDSS_STATUS_SUCCESS != HAL_DSI_PhyPllPowerCtrl(eDeviceId, pPropertyData->bEnablePLL))
        {
            eStatus = DSI_STATUS_FAILED_TIMEOUT;
        }
      }
      break;
    case DSI_DISPLAY_PROPERTYID_LANEMAP:
      {
        uint32                          uCount;
        HAL_DSI_PhyPropertyParamsType   sPhyPropertyType;
        
        DSI_OSAL_MemZero((void *)&sPhyPropertyType, sizeof(HAL_DSI_PhyPropertyParamsType));

        for (uCount=0; uCount<(DSI_LaneMap_MAX-1); uCount++)
        {
          sPhyPropertyType.aLaneMap[uCount] = pPropertyData->aLaneMap[uCount];
        }
        HAL_DSI_PhySetProperty(eDeviceId, HAL_DSI_PHY_PROPERTYID_LANEMAP, &sPhyPropertyType);
        
       }
      break;
    case DSI_DISPLAY_PROPERTYID_SETSCRATCH1:
      {
        HAL_DSI_SetScratchRegister1(eDeviceId, pPropertyData->uScratchReg);
      }
      break;
    default:
        eStatus = DSI_STATUS_FAILED_NO_SUPPORT_FOR_CHANNEL;
        break;
  }

EXIT:
  return eStatus;
}

/* -----------------------------------------------------------------------

DSI_Display_GetProperty

DESCRIPTION:
    TBD

** ----------------------------------------------------------------------- */
DSI_StatusType DSI_Display_GetProperty(
    DSI_HandleType            hDisplay,
    DSI_DisplayPropertyType   eProperty,
    DSI_PropertyParamsType   *pPropertyData)
{
  DSI_StatusType        eStatus       = DSI_STATUS_SUCCESS;
  DSI_Device_IDType     eDeviceId     = DSI_DeviceID_0;
  DSI_Channel_IDType    eDisplayId    = DSI_ChannelID_NONE;
  
  /* Check Parameters                                   */
  DSI_NULLPOINTER_AND_EXIT(pPropertyData, eStatus, EXIT);

  dsiShared_Handle2DisplayID(hDisplay, &eDeviceId, &eDisplayId);  

  switch(eProperty)
  {
  case DSI_DISPLAY_PROPERTYID_HOSTERROR:
      eStatus = dsiShared_GetLastHostErrorCode(eDeviceId, &(pPropertyData->uHostError));  
      break;

  case DSI_DISPLAY_PROPERTYID_PANELERROR:
      eStatus = dsiShared_GetLastPanelErrorCode(eDeviceId, &(pPropertyData->uPanelError));  
      break;
  case DSI_DISPLAY_PROPERTYID_ENABLEDINTSTATUS:
      {
        uint32 uIntEnabled;
        uint32 uIntList;
        uint32 uError;
          
        HAL_DSI_GetInterruptsEnabled(eDeviceId, &uIntEnabled);

        HAL_DSI_GetInterruptStatus(eDeviceId, &uIntList, &uError);

        // Mask off disabled interrupts
        uIntList &= uIntEnabled;
        
        // Clear interrupt  
        HAL_DSI_ClrInterruptStatus(eDeviceId, uIntList);

        // Return the translated interrupt list
        pPropertyData->uInterruptList = dsiShared_HALInt2DSIInt(uIntList);
      }
      break;
  case DSI_DISPLAY_PROPERTYID_INTSTATUS:
      {
        uint32 uIntList;
        uint32 uError;
          
        HAL_DSI_GetInterruptStatus(eDeviceId, &uIntList, &uError);
        
        // Clear interrupt  
        HAL_DSI_ClrInterruptStatus(eDeviceId, uIntList);

        // Return the translated interrupt list
        pPropertyData->uInterruptList = dsiShared_HALInt2DSIInt(uIntList);
      }
      break;
  case DSI_DISPLAY_PROPERTYID_INTENABLE:
      {
        uint32 uIntList;

        HAL_DSI_GetInterruptsEnabled(eDeviceId, &uIntList);        

        pPropertyData->uInterruptList = dsiShared_HALInt2DSIInt(uIntList);
      }
      break;
  case DSI_DISPLAY_PROPERTYID_CONTROLLERSTATUS:
      {
        uint32  uControllerStatus;

        pPropertyData->uControllerStatus = DSI_CONTROLLER_STATUS_IDLE;
        
        HAL_DSI_GetStatus(eDeviceId, &uControllerStatus);

        if (DSI_STATUS_DEVICE_BUSY_CMDENGINE & uControllerStatus)
        {
          pPropertyData->uControllerStatus |= DSI_CONTROLLER_STATUS_CMD_ENGINE_BUSY;
        }
        if (DSI_STATUS_DEVICE_BUSY_CMDENGINEDMA & uControllerStatus)
        {
          pPropertyData->uControllerStatus |= DSI_CONTROLLER_STATUS_DMA_ENGINE_BUSY;
        }
        if (DSI_STATUS_DEVICE_BUSY_CMDENGINEMDP & uControllerStatus)
        {
          pPropertyData->uControllerStatus |= DSI_CONTROLLER_STATUS_CMD_MDP_ENGINE_BUSY;
        } 
        if (DSI_STATUS_DEVICE_BUSY_VIDEOENGINE & uControllerStatus)
        {
          pPropertyData->uControllerStatus |= DSI_CONTROLLER_STATUS_VIDEO_ENGINE_BUSY;
        }        
        if (DSI_STATUS_DEVICE_BUSY_BTA & uControllerStatus)
        {
          pPropertyData->uControllerStatus |= DSI_CONTROLLER_STATUS_BTA_BUSY;
        }        
      }
      break;

  case DSI_DISPLAY_PROPERTYID_LANESTATUS:
      {
        uint32                          uLaneStatus;
        HAL_DSI_PhyPropertyParamsType   sPhyPropertyType;
        HAL_MDSS_ErrorType              ePhyStatus;
        
        DSI_OSAL_MemZero((void *)&sPhyPropertyType, sizeof(HAL_DSI_PhyPropertyParamsType));
        
        /* Retrieve the device lane status from DSI*/
        HAL_DSI_GetLaneStatus(eDeviceId,  &uLaneStatus);
        
        /* Get status from Phy */
        ePhyStatus = HAL_DSI_PhyGetProperty(eDeviceId, HAL_DSI_PHY_PROPERTYID_LANESTATUS, &sPhyPropertyType);
        if ( HAL_MDSS_STATUS_SUCCESS == ePhyStatus )
        {
          uLaneStatus |= sPhyPropertyType.uLaneStatus;
        }

        /* ULPS state - Most of the devices have four DSI data lanes. However, there are a few devices which have lesser than four 
           data lanes. Checking for all the four data lanes in these devices would be incorrect to determine if the device entered 
           ULPS or not. Moreover, these devices might re-map the data lanes to different IDs making it a bit difficult to check for
           particular lanes. A simplest solution would be to just check if the DSI clock lane and any one of the DSI data lanes has 
           entered ULPS or not. */
        if ((!(uLaneStatus & DSI_LANE_STATUS_DLN0_ULPS_NOT_ACTIVE)  ||
             !(uLaneStatus & DSI_LANE_STATUS_DLN1_ULPS_NOT_ACTIVE)  ||
             !(uLaneStatus & DSI_LANE_STATUS_DLN2_ULPS_NOT_ACTIVE)  ||
             !(uLaneStatus & DSI_LANE_STATUS_DLN3_ULPS_NOT_ACTIVE)) &&
             !(uLaneStatus & DSI_LANE_STATUS_CLKLN_ULPS_NOT_ACTIVE))
        {
          pPropertyData->eLaneState = DSI_LANE_STATE_LP00;
        }

        /* Stop state - Most of the devices have four DSI data lanes. However, there are a few devices which have lesser than four 
           data lanes. Checking for all the four data lanes in these devices would be incorrect to determine if the device exited 
           ULPS or not. Moreover, these devices might re-map the data lanes to different IDs making it a bit difficult to check for
           particular lanes. A simplest solution would be to just check if the DSI clock lane and any one of the DSI data lanes has 
           exited ULPS or not. */
        else if (((uLaneStatus & DSI_LANE_STATUS_DLN0_STOPSTATE)  ||
                  (uLaneStatus & DSI_LANE_STATUS_DLN1_STOPSTATE)  ||
                  (uLaneStatus & DSI_LANE_STATUS_DLN2_STOPSTATE)  ||
                  (uLaneStatus & DSI_LANE_STATUS_DLN3_STOPSTATE)) &&
                  (uLaneStatus & DSI_LANE_STATUS_CLKLN_STOPSTATE))
        {
          pPropertyData->eLaneState = DSI_LANE_STATE_LP11;
        }
      }
      break; 

  case DSI_DISPLAY_PROPERTYID_GETCALCODES:
      {
        DSI_DisplayCtxType*     psDisplayCtx = DSIHOST_GET_DISPLAYCTX(eDeviceId, eDisplayId);
        
        if(psDisplayCtx->sDisplayInfo.sDynamicRefreshInfo.uNumOfDynamicFreq > 0)
        {
          pPropertyData->sDisplayInfo.sDynamicRefreshInfo.uNumOfDynamicFreq = psDisplayCtx->sDisplayInfo.sDynamicRefreshInfo.uNumOfDynamicFreq;
          DSI_OSAL_MemCpy((void *) &pPropertyData->sDisplayInfo.sPllCodesInfo, 
                          (void*) &psDisplayCtx->sDisplayInfo.sPllCodesInfo, 
                          sizeof(HAL_DSI_PHY_PLL_Codes_Info));
        }
        else
        {
          eStatus = DSI_STATUS_FAILED_RESOURCE_NOT_READY;
        }
      }
      break;

  default:
      eStatus = DSI_STATUS_FAILED_NOT_SUPPORTED;
      break;
  }

EXIT:
  return eStatus;
}


#ifdef __cplusplus
}
#endif
