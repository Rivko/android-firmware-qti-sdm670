/*=============================================================================
 
  File: dsipanel.c
 
  Source file for HAL DSI Panel configuration
  
     Copyright (c) 2011-2018 Qualcomm Technologies, Inc.  All Rights Reserved.
     Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */
#include "MDPLib.h"
#include "MDPLib_i.h"
#include "MDPSystem.h"
#include "MDPPlatformLib.h"
#include "xmlparser.h"
#include "dsiHost.h"
#include "HALdsi_Phy.h"
#include "dsiHostShared.h"
#include "MDPPeripherals.h"
#include "dsiHostSystem.h"

/* -----------------------------------------------------------------------
** Defines
** ----------------------------------------------------------------------- */
#define DSI_LANE_SWAPNUM                      8  // Maximum DSI SWAP lanes supported
#define DSI_STATUS_POLLING_CYCLE              5  // DSI controller status polling cycle, 5ms
#define DSI_STATUS_CHECK_MAX_TIME            40  // Maximum DSI controller status check time. 40ms
#define DSI_SPLITDISPLAY_NUM                  2  // 2 DSI devices are used in DSI split display mode
#define DSI_CONTROLLER_0                   0x01  // Bit Mask for DSI controller 0
#define DSI_CONTROLLER_1                   0x02  // Bit Mask for DSI controller 1
#define DSI_DEFAULT_BITCLOCK_FREQ     110000000  // Bitclock frequency based on VGA resolution @60fps

/* DSI Driver context */
typedef struct
{
  bool32                    bDualDSI;                          /* If DSI0/1 are working in Dual DSI mode   */
  DSI_HandleType            hDSIHandle[DSI_DeviceID_MAX];      /* DSI device handles for each controller   */
  DSI_HandleType            hDisplayHandle[DSI_DeviceID_MAX];  /* DSI display handles for each controller  */
  uint32                    uNumDSIControllers;
  void                     *pDSIBuffer;                        /* Pointer to DSI DMA command buffer*/
} DSI_DriverCtx;

/* -----------------------------------------------------------------------
** Global variables
** ----------------------------------------------------------------------- */
DSI_DriverCtx  gDsiDriverCtx;

static const  DSI_LaneMapType  sDisplayLaneSetting[DSI_LANE_SWAPNUM][DSI_LaneMap_MAX] ={
        { DSI_LaneMap_0,  DSI_LaneMap_1, DSI_LaneMap_2,  DSI_LaneMap_3  },
        { DSI_LaneMap_3,  DSI_LaneMap_0, DSI_LaneMap_1,  DSI_LaneMap_2  },
        { DSI_LaneMap_2,  DSI_LaneMap_3, DSI_LaneMap_0,  DSI_LaneMap_1  },
        { DSI_LaneMap_1,  DSI_LaneMap_2, DSI_LaneMap_3,  DSI_LaneMap_0  },
        { DSI_LaneMap_0,  DSI_LaneMap_3, DSI_LaneMap_2,  DSI_LaneMap_1  },
        { DSI_LaneMap_1,  DSI_LaneMap_0, DSI_LaneMap_3,  DSI_LaneMap_2  },
        { DSI_LaneMap_2,  DSI_LaneMap_1, DSI_LaneMap_0,  DSI_LaneMap_3  },
        { DSI_LaneMap_3,  DSI_LaneMap_2, DSI_LaneMap_1,  DSI_LaneMap_0  }};

/*------------------------------------------------------------------------
Custom DSI commands types
-------------------------------------------------------------------------- */

#define DSI_CMD_CUSTOM_DELAY                        0xFF
#define DSI_CMD_CUSTOM_COLOR_FILL                   0xFE
#define DSI_CMD_CUSTOM_BROADCAST                    0xFD

#define DSI_CMD_CUSTOM_COLOR_FILL_PAYLOAD_LENGTH    (3)

#define DSI_GET_DRIVER_CTX()                        &gDsiDriverCtx

#define DSI_CORE_CLOCK_MAPPING(DsiDeviceId)         ((DsiDeviceId == DSI_DeviceID_0) ? MDP_CLOCKTYPE_DSI0 : MDP_CLOCKTYPE_DSI1)

/*------------------------------------------------------------------------
Local Prototypes
-------------------------------------------------------------------------- */
static MDP_Status SendPanelXMLSequence(DSI_DriverCtx *pDriverCtx, void *pDSICommandBuffer, MDP_Panel_AttrType *pPanelConfig);
static MDP_Status SendPanelPPSRawBuffer(DSI_DriverCtx *pDriverCtx, MDP_Panel_AttrType *pPanelConfig);
MDP_Status DsiPanelSendCommandSequence(MDP_Panel_AttrType *pPanelInfo, MdpPanelCommandType eCommand, void *pCustomCommand, uint32 uFlags);

/* ---------------------------------------------------------------------- */
/**
** FUNCTION: DSIDriver_Read()
** 
** DESCRIPTION:
**   This allows to send the dsi read commands to the panel. 
** 
**
** -------------------------------------------------------------------- */
MDP_Status DSIDriver_Read(uint8 uCmdType, void *pPayload, uint32 uPayloadSize, void *pReadData, uint32 *pReadSize)
{
  MDP_Status         eStatus        = MDP_STATUS_OK;
  DSI_DriverCtx     *pDriverCtx     = DSI_GET_DRIVER_CTX();
  DSI_ReadPacketType sReadPacket;

  MDP_OSAL_MEMZERO(&sReadPacket, sizeof(DSI_ReadPacketType));

  sReadPacket.eDisplayVC            = DSI_Display_VC_0;
  sReadPacket.uReadCmdDT            = uCmdType;
  sReadPacket.uReadParamInByte      = uPayloadSize;
  sReadPacket.puReadParam           = (uint8*)pPayload;
  sReadPacket.uReadPacketBufferSize = *pReadSize;
  sReadPacket.puReadPacket          = (uint8*)pReadData;
  sReadPacket.uRetPacketByte        = 0;

  if (DSI_STATUS_SUCCESS != DSI_Display_ClientRead(pDriverCtx->hDisplayHandle[0], &sReadPacket))
  {
    eStatus = MDP_STATUS_FAILED;
  }
  else
  {
    // return number of bytes in the read buffer 
    *pReadSize = sReadPacket.uRetPacketByte;
  }

  return eStatus;
}

/* ---------------------------------------------------------------------- */
/**
** FUNCTION: DSIDriver_Write()
** 
** DESCRIPTION:
**   This allows to send the dsi write commands to the panel. 
** 
**
** -------------------------------------------------------------------- */
MDP_Status DSIDriver_Write(uint8 uCmdType, void *pPayload, uint32 uPayloadSize)
{
  MDP_Status           eMDPStatus   = MDP_STATUS_OK;
  DSI_StatusType       eDSIStatus   = DSI_STATUS_SUCCESS;
  DSI_DriverCtx       *pDriverCtx   = DSI_GET_DRIVER_CTX();
  MDP_Panel_AttrType  *pPanelConfig = MDP_GET_DISPLAYINFO(MDP_DISPLAY_PRIMARY);
  DSI_WritePacketType  sWritePacket;

  MDP_OSAL_MEMZERO(&sWritePacket, sizeof(DSI_WritePacketType));

  sWritePacket.eDisplayVC           = DSI_Display_VC_0;
  sWritePacket.uCmdDataType         = uCmdType;
  sWritePacket.bPayloadLen          = uPayloadSize;
  sWritePacket.puPayload            = (uint8*)pPayload;
  sWritePacket.bQueuePacket         = FALSE;

  if (NULL == pPanelConfig)
  {
    DSI_Log_Message(MDP_LOGLEVEL_ERROR, "DisplayDxe: DSIDriver_Write: pPanelConfig is NULL!\n");
  }
  else
  {
    sWritePacket.bHighSpeedMode = (pPanelConfig->uAttrs.sDsi.bPacketTransferHS)?TRUE:FALSE;
  }

  eDSIStatus = DSI_Display_ClientWrite(pDriverCtx->hDisplayHandle[0], &sWritePacket);
  if (DSI_STATUS_SUCCESS != eDSIStatus)
  {
    eMDPStatus = MDP_STATUS_FAILED;
    DSI_Log_Message(MDP_LOGLEVEL_ERROR,
	    "DisplayDxe: DSIDriver_Write: DSI_Display_ClientWrite failed! DSI Status: 0x%x\n",
		eDSIStatus);
  }

  return eMDPStatus;
}

/* ---------------------------------------------------------------------- */
/**
** FUNCTION: DSIDriver_Init()
** 
** DESCRIPTION:
**   Initialize the DSI Core
**
*//* -------------------------------------------------------------------- */
MDP_Status DSIDriver_Init(MDP_Panel_AttrType *pPanelConfig)
{
  MDP_Status            eStatus       = MDP_STATUS_OK;
  DSI_DeviceConfigType  sDeviceConfig;
  DSI_DriverCtx        *pDriverCtx    = DSI_GET_DRIVER_CTX();
  void                 *pDSIBuffer    = pDriverCtx->pDSIBuffer;
  uint32                uDevIdx;
  uint32                uFlags        = 0;

  if (0 == (pPanelConfig->eDisplayFlags & MDP_PANEL_FLAG_DSI_DCS_FIFO_ONLY))
  {
    if (NULL == pDSIBuffer)
    {
      if (NULL == (pDSIBuffer = MDP_OSAL_CALLOC(DSI_DMA_BUFFER_SIZE)))
      {
        DSI_Log_Message(MDP_LOGLEVEL_INFO, "DixplayDxe: MDP_OSAL_CALLOC failed\n");
        eStatus = MDP_STATUS_NO_RESOURCES;
      }
    }
    uFlags = DSI_DEVICE_FLAGS_DSI_DCS_DMA_ONLY;
  }

  if (MDP_STATUS_OK == eStatus)
  {

    if (0x3 == pPanelConfig->uAttrs.sDsi.uControllerMappingMask)
    {
      pDriverCtx->bDualDSI           = TRUE;
      pDriverCtx->uNumDSIControllers = DSI_DeviceID_MAX;
    }
    else
    {
      pDriverCtx->uNumDSIControllers = 1;
    }


    for (uDevIdx = 0; uDevIdx < pDriverCtx->uNumDSIControllers; uDevIdx++)
    {
      DSI_Device_IDType eDSIDeviceId = (DSI_Device_IDType)pPanelConfig->uAttrs.sDsi.uControllerMapping[uDevIdx];

      if (DSI_DeviceID_MAX <= eDSIDeviceId)
      {
        DSI_Log_Message(MDP_LOGLEVEL_INFO, "DixplayDxe: Invalid DSI Device ID %d\n", eDSIDeviceId);
        eStatus = MDP_STATUS_FAILED;
        break;
      }

      if (MDP_STATUS_OK != MDPSetupClocks(DSI_CORE_CLOCK_MAPPING(eDSIDeviceId), NULL))
      {
        DSI_Log_Message(MDP_LOGLEVEL_INFO, "DixplayDxe: DSIDriver_Init(MDP_CLOCKTYPE_DSI%d) failed\n", eDSIDeviceId);
        // Non fatal, only a warning .
      }
    
      MDP_OSAL_MEMZERO(&sDeviceConfig, sizeof(DSI_DeviceConfigType));    
      sDeviceConfig.eDeviceID                            = eDSIDeviceId;
      sDeviceConfig.bDualDsi                             = pDriverCtx->bDualDSI;
      sDeviceConfig.bLittleEndian                        = TRUE;
      sDeviceConfig.bNonCompliantMode                    = FALSE;
      sDeviceConfig.bEnableCRCCheck                      = FALSE;
      sDeviceConfig.bEnableECCCheck                      = FALSE;
      sDeviceConfig.sMemoryConfig.pCPUAddress            = pDSIBuffer;
      sDeviceConfig.sMemoryConfig.sMDPAddress.iQuadPart  = (int64)pDSIBuffer;
      sDeviceConfig.sMemoryConfig.uSize                  = DSI_DMA_BUFFER_SIZE;
      sDeviceConfig.bCPhyMode                            = pPanelConfig->uAttrs.sDsi.bCPhyMode;
      sDeviceConfig.bEnableLP11AtInit                    = pPanelConfig->uAttrs.sDsi.bLP11AtInit;
      sDeviceConfig.bPhyDCDCMode                         = pPanelConfig->uAttrs.sDsi.bPhyDCDCMode;
      sDeviceConfig.uClockStrengthHS                     = pPanelConfig->uAttrs.sDsi.uClockStrengthHS;
      sDeviceConfig.uDataStrengthHS                      = pPanelConfig->uAttrs.sDsi.uDataStrengthHS;
      sDeviceConfig.uDataStrengthLP                      = pPanelConfig->uAttrs.sDsi.uDataStrengthLP;
      sDeviceConfig.uFlags                               = uFlags; 
      

      if (DSI_STATUS_SUCCESS != DSI_Device_Open(&sDeviceConfig, &(pDriverCtx->hDSIHandle[uDevIdx])))
      {
        eStatus = MDP_STATUS_FAILED;
        break;
      }
    }

    // Check whether I2C needs to be initialized
    if ((MDP_STATUS_OK == eStatus) && 
        (NULL          != pPanelConfig->uAttrs.sDsi.sI2CConfig.pI2CInitSequence))
    {
      if (MDP_STATUS_OK != (eStatus = I2CDriver_Init(pPanelConfig)))
      {
        DSI_Log_Message(MDP_LOGLEVEL_ERROR, "DixplayDxe: I2CDriver_Init() failed eStatus=%d \n", eStatus);
      }
    }
  }
  return eStatus;
}


/* ---------------------------------------------------------------------- */
/**
** FUNCTION: DSIDriver_Close()
** 
** DESCRIPTION:
**   Deinitialize the DSI Core
**
*//* -------------------------------------------------------------------- */
void DSIDriver_Close(void)
{
  DSI_DriverCtx        *pDriverCtx    = DSI_GET_DRIVER_CTX();
  uint32                uDevIdx;

  if (0 == pDriverCtx->uNumDSIControllers)
  {
    pDriverCtx->uNumDSIControllers = 1;
  }

  // Only send a termination sequence if the panel has been setup already
  // First loop to close all DSI display handles, turning off DSI Clocks 
  // first to avoid turning off DSI PLL source before turning off DSI 1 clock
  for (uDevIdx = 0; uDevIdx < DSI_DeviceID_MAX; uDevIdx++)
  {
    DSI_HandleType        hDisplay = pDriverCtx->hDisplayHandle[uDevIdx];
    DSI_Device_IDType     eDeviceId;
    DSI_Channel_IDType    eDisplayId;

    dsiShared_Handle2DisplayID(hDisplay,&eDeviceId,&eDisplayId);
   
    if (eDisplayId != DSI_ChannelID_NONE)
    {
       if (DSI_STATUS_SUCCESS != DSI_Display_Close(pDriverCtx->hDisplayHandle[uDevIdx]))
       {
         DSI_Log_Message(MDP_LOGLEVEL_INFO, "DixplayDxe: DSI_Display_Close() failed on device%d\n", uDevIdx );
         break;
       }
       // Turn off DSI Clock turned on in DSIDriver_Init()
       if (MDP_STATUS_OK != MDPDisableClocks(DSI_CORE_CLOCK_MAPPING(eDeviceId)))
       {
         DSI_Log_Message(MDP_LOGLEVEL_INFO, "DixplayDxe: DSIDriver_Close(MDP_CLOCKTYPE_DSI%d) failed\n", uDevIdx);
         // Non fatal, only a warning .
       }
    }
  }

  // Second loop to close all DSI device handles
  for (uDevIdx = 0; uDevIdx < pDriverCtx->uNumDSIControllers; uDevIdx++)
  {
    if (DSI_STATUS_SUCCESS != DSI_Device_Close(pDriverCtx->hDSIHandle[uDevIdx], 0x0))
    {
      DSI_Log_Message(MDP_LOGLEVEL_INFO, "DixplayDxe: DSI_Device_Close() failed on device%d\n", uDevIdx );
      break;
    }
  }
}

/* ---------------------------------------------------------------------- */
/**
** FUNCTION: DSIDriver_SetMode()
** 
** DESCRIPTION:
**   Initialize the DSI Core
**
*//* -------------------------------------------------------------------- */
MDP_Status DSIDriver_SetMode(MDP_Panel_AttrType *pPanelConfig)
{
  MDP_Status            eStatus     =  MDP_STATUS_OK;
  uint32                i           =  0;
  DSI_DisplayConfigType sDisplayConfig;
  DSI_DriverCtx        *pDriverCtx  = DSI_GET_DRIVER_CTX();
  uint32                uDevIdx;

  for (uDevIdx = 0; uDevIdx < pDriverCtx->uNumDSIControllers; uDevIdx++)
  {
    MDP_OSAL_MEMZERO(&sDisplayConfig, sizeof(DSI_DisplayConfigType));
    sDisplayConfig.sDisplayInfo.eDisplayId      = (DSI_Channel_IDType)pPanelConfig->uAttrs.sDsi.eChannelId;
    sDisplayConfig.sDisplayInfo.eMDPDisplayId   = (MDP_Display_IDType)pPanelConfig->eDisplayId;
    sDisplayConfig.sDisplayInfo.eDisplayVC      = (DSI_Display_VCType)pPanelConfig->uAttrs.sDsi.eDisplayVC;
    sDisplayConfig.sDisplayInfo.eColorFormat    = (DSI_ColorFormatType)pPanelConfig->uAttrs.sDsi.eColorFormat;
    sDisplayConfig.sDisplayInfo.uNumLanes       = pPanelConfig->uAttrs.sDsi.uDataLaneNum;
    sDisplayConfig.sDisplayInfo.bCPhyMode       = pPanelConfig->uAttrs.sDsi.bCPhyMode;
    sDisplayConfig.sDisplayInfo.uRefreshRate    = pPanelConfig->uAttrs.sDsi.uRefreshRate; 
    sDisplayConfig.sDisplayInfo.uBitClockFreq   = pPanelConfig->uAttrs.sDsi.uBitClockFrequency;
    sDisplayConfig.sDisplayInfo.bDisableEotp    = pPanelConfig->uAttrs.sDsi.bDisableEotpAfterHSXfer;
    sDisplayConfig.sDisplayInfo.uInitMasterTime = pPanelConfig->uAttrs.sDsi.uInitMasterTime;  
    sDisplayConfig.sDisplayInfo.uEscapeDivisor  = pPanelConfig->uAttrs.sDsi.uEscapeClockDivisor;
    sDisplayConfig.sDisplayInfo.uEscapeFreq     = pPanelConfig->uAttrs.sDsi.uEscapeClockFreq;

    // Assign lanes
    if (pPanelConfig->uAttrs.sDsi.uLaneRemap >= DSI_LANE_SWAPNUM)
    {
      pPanelConfig->uAttrs.sDsi.uLaneRemap = 0;
      DSI_Log_Message(MDP_LOGLEVEL_INFO, "DixplayDxe: DSIDriver_SetMode() Incorrect Lane remap parameter, defalting to 0\n" );
    }
    for (i=0;i<(DSI_LaneMap_MAX-1);i++)
    {
      if (i<pPanelConfig->uAttrs.sDsi.uDataLaneNum)
      {
        sDisplayConfig.eDisplayLane[i] = sDisplayLaneSetting[pPanelConfig->uAttrs.sDsi.uLaneRemap][i];
      }
      else
      {
        sDisplayConfig.eDisplayLane[i] = DSI_LaneMap_NONE;
      }
    }

    if (pPanelConfig->ePhysConnect == MDP_DISPLAY_CONNECT_PRIMARY_DSI_VIDEO )
    {
      sDisplayConfig.sVideoModeConfg.bEnable                            = TRUE;
      sDisplayConfig.sDisplayInfo.iVisWidthPx                           = pPanelConfig->uDisplayWidth +
                                                                          pPanelConfig->sActiveTiming.uHLeftBorderDClk +
                                                                          pPanelConfig->sActiveTiming.uHRightBorderDClk;
      sDisplayConfig.sDisplayInfo.iVisHeightPx                          = pPanelConfig->uDisplayHeight +
                                                                          pPanelConfig->sActiveTiming.uVTopBorderLines +
                                                                          pPanelConfig->sActiveTiming.uVBottomBorderLines;
      sDisplayConfig.sDisplayInfo.iBlankHeightPx                        = pPanelConfig->sActiveTiming.uVsyncPulseWidthLns + 
                                                                          pPanelConfig->sActiveTiming.uVsyncBackPorchLns + 
                                                                          pPanelConfig->sActiveTiming.uVsyncFrontPorchLns;
      sDisplayConfig.sDisplayInfo.iBlankWidthPx                         = pPanelConfig->sActiveTiming.uHsyncPulseWidthDclk + 
                                                                          pPanelConfig->sActiveTiming.uHsyncBackPorchDclk + 
                                                                          pPanelConfig->sActiveTiming.uHsyncFrontPorchDclk;
      sDisplayConfig.sVideoModeConfg.eVideoTraffic                      = (DSI_TrafficModeType) pPanelConfig->uAttrs.sDsi.eTrafficMode;
      sDisplayConfig.sVideoModeConfg.sVideoTiming.iHsyncPulseWidthDclk  = pPanelConfig->sActiveTiming.uHsyncPulseWidthDclk;
      sDisplayConfig.sVideoModeConfg.sVideoTiming.iHsyncBackPorchDclk   = pPanelConfig->sActiveTiming.uHsyncBackPorchDclk;
      sDisplayConfig.sVideoModeConfg.sVideoTiming.iHsyncFrontPorchDclk  = pPanelConfig->sActiveTiming.uHsyncFrontPorchDclk;
      sDisplayConfig.sVideoModeConfg.sVideoTiming.iHsyncSkewDclk        = pPanelConfig->sActiveTiming.uHsyncSkewDclk;
      sDisplayConfig.sVideoModeConfg.sVideoTiming.iVsyncPulseWidthLines = pPanelConfig->sActiveTiming.uVsyncPulseWidthLns;
      sDisplayConfig.sVideoModeConfg.sVideoTiming.iVsyncBackPorchLine   = pPanelConfig->sActiveTiming.uVsyncBackPorchLns;
      sDisplayConfig.sVideoModeConfg.sVideoTiming.iVsyncFrontPorchLine  = pPanelConfig->sActiveTiming.uVsyncFrontPorchLns;
      sDisplayConfig.sVideoModeConfg.sVideoTiming.iVsyncSkewDclk        = 0;
      sDisplayConfig.sVideoModeConfg.sVideoTiming.iVsyncStart           = 0;
      sDisplayConfig.sVideoModeConfg.sVideoTiming.iVsyncEnd             = 0;
      // Panel power settings
      sDisplayConfig.sVideoModeConfg.sPowerOptions.bSendHsaHseAfterVsVe         = pPanelConfig->uAttrs.sDsi.bSendHsaHseAfterVsVe;
      sDisplayConfig.sVideoModeConfg.sPowerOptions.bEnableLowPowerModeInHFP     = pPanelConfig->uAttrs.sDsi.bLowPowerModeInHFP;
      sDisplayConfig.sVideoModeConfg.sPowerOptions.bEnableLowPowerModeInHBP     = pPanelConfig->uAttrs.sDsi.bLowPowerModeInHBP;
      sDisplayConfig.sVideoModeConfg.sPowerOptions.bEnableLowPowerModeInHSA     = pPanelConfig->uAttrs.sDsi.bLowPowerModeInHSA;
      sDisplayConfig.sVideoModeConfg.sPowerOptions.bEnableLowPowerModeInBLLPEOF = pPanelConfig->uAttrs.sDsi.bLowPowerModeInBLLPEOF;
      sDisplayConfig.sVideoModeConfg.sPowerOptions.bEnableLowPowerModeInBLLP    = pPanelConfig->uAttrs.sDsi.bLowPowerModeInBLLP; 

      if (pPanelConfig->uAttrs.sDsi.bDSCEnable)
      {
        if (pPanelConfig->uAttrs.sDsi.uDSCCompressionRatio)
        {
          // DSC settings for the DSI controller
          sDisplayConfig.sDisplayInfo.iVisWidthPx                       /= pPanelConfig->uAttrs.sDsi.uDSCCompressionRatio;
          sDisplayConfig.sVideoModeConfg.sDSCVideoConfig.uDscEnable      = pPanelConfig->uAttrs.sDsi.bDSCEnable;
          sDisplayConfig.sVideoModeConfg.sDSCVideoConfig.uDscStreamId    = 0;
          sDisplayConfig.sVideoModeConfg.sDSCVideoConfig.uDscEolByteNum  = 0;

          // We send all the slices in one packet,  uDscPktPerLine = 0 means 1 packet for all soft slices
          sDisplayConfig.sVideoModeConfg.sDSCVideoConfig.uDscPktPerLine  = 0;
          sDisplayConfig.sVideoModeConfg.sDSCVideoConfig.uDscBytesPerPkt = pPanelConfig->uAttrs.sDsi.sDSCEncCfg.uBytePerPkt;
        }
      }
    }
    else // QDI_DISPLAY_CONNECT_PRIMARY_DSI_CMD
    {
      sDisplayConfig.sDisplayInfo.iVisWidthPx    = pPanelConfig->uDisplayWidth;
      sDisplayConfig.sDisplayInfo.iBlankWidthPx  = pPanelConfig->sActiveTiming.uHsyncPulseWidthDclk +
                                                   pPanelConfig->sActiveTiming.uHsyncBackPorchDclk  +
                                                   pPanelConfig->sActiveTiming.uHsyncFrontPorchDclk;
      sDisplayConfig.sDisplayInfo.iVisHeightPx   = pPanelConfig->uDisplayHeight;
      sDisplayConfig.sDisplayInfo.iBlankHeightPx = pPanelConfig->sActiveTiming.uVsyncPulseWidthLns  +
                                                   pPanelConfig->sActiveTiming.uVsyncBackPorchLns   +
                                                   pPanelConfig->sActiveTiming.uVsyncFrontPorchLns;
      sDisplayConfig.sCmdModeConfg.uIdleTime     = pPanelConfig->uAttrs.sDsi.uCommandModeIdleTime;

      if (pPanelConfig->uAttrs.sDsi.bDSCEnable)
      {
        if (pPanelConfig->uAttrs.sDsi.uDSCCompressionRatio)
        {
          sDisplayConfig.sDisplayInfo.iVisWidthPx                    /= pPanelConfig->uAttrs.sDsi.uDSCCompressionRatio;

          // DSC settings for the DSI controller
          sDisplayConfig.sCmdModeConfg.sDSCCmdConfig.uDscEnable       = pPanelConfig->uAttrs.sDsi.bDSCEnable;
          sDisplayConfig.sCmdModeConfg.sDSCCmdConfig.uDscStreamId     = 0;
          sDisplayConfig.sCmdModeConfg.sDSCCmdConfig.uDscBytesInSlice = 0;
          sDisplayConfig.sCmdModeConfg.sDSCCmdConfig.uDscEolByteNum   = 0;
          // We send all the slices in one packet,  uDscPktPerLine = 0 means 1 packet for all soft slices
          sDisplayConfig.sCmdModeConfg.sDSCCmdConfig.uDscPktPerLine   = 0;
        }
        else
        {
          // In case of error, default compressionRatio = 1 i.e. no compression.
          pPanelConfig->uAttrs.sDsi.uDSCCompressionRatio = HAL_MDP_COMPRESSION_RATIO_1_TO_1; 
          eStatus                                        = MDP_STATUS_BAD_PARAM;
          break;
        }
      }
    }

    if ( MDP_STATUS_OK == eStatus)
    {
      // Timing Override
      sDisplayConfig.sTimingOverride.bTimingHSZeroOverride    = pPanelConfig->uAttrs.sDsi.bTimingHSZeroOverride;
      sDisplayConfig.sTimingOverride.uTimingHSZeroValue       = pPanelConfig->uAttrs.sDsi.uTimingHSZeroValue;
      sDisplayConfig.sTimingOverride.bTimingHSExitOverride    = pPanelConfig->uAttrs.sDsi.bTimingHSExitOverride;
      sDisplayConfig.sTimingOverride.uTimingHSExitValue       = pPanelConfig->uAttrs.sDsi.uTimingHSExitValue;
      sDisplayConfig.sTimingOverride.bTimingHSPrepareOverride = pPanelConfig->uAttrs.sDsi.bTimingHSPrepareOverride;
      sDisplayConfig.sTimingOverride.uTimingHSPrepareValue    = pPanelConfig->uAttrs.sDsi.uTimingHSPrepareValue;
      sDisplayConfig.sTimingOverride.bTimingHSTrailOverride   = pPanelConfig->uAttrs.sDsi.bTimingHSTrailOverride;
      sDisplayConfig.sTimingOverride.uTimingHSTrailValue      = pPanelConfig->uAttrs.sDsi.uTimingHSTrailValue;
      sDisplayConfig.sTimingOverride.bTimingHSRequestOverride = pPanelConfig->uAttrs.sDsi.bTimingHSRequestOverride;
      sDisplayConfig.sTimingOverride.uTimingHSRequestValue    = pPanelConfig->uAttrs.sDsi.uTimingHSRequestValue;
      sDisplayConfig.sTimingOverride.bTimingCLKZeroOverride   = pPanelConfig->uAttrs.sDsi.bTimingCLKZeroOverride;
      sDisplayConfig.sTimingOverride.uTimingCLKZeroValue      = pPanelConfig->uAttrs.sDsi.uTimingCLKZeroValue;
      sDisplayConfig.sTimingOverride.bTimingCLKTrailOverride  = pPanelConfig->uAttrs.sDsi.bTimingCLKTrailOverride;
      sDisplayConfig.sTimingOverride.uTimingCLKTrailValue     = pPanelConfig->uAttrs.sDsi.uTimingCLKTrailValue;
      sDisplayConfig.sTimingOverride.bTimingCLKPrepareOverride= pPanelConfig->uAttrs.sDsi.bTimingCLKPrepareOverride;
      sDisplayConfig.sTimingOverride.uTimingCLKPrepareValue   = pPanelConfig->uAttrs.sDsi.uTimingCLKPrepareValue;
      sDisplayConfig.sTimingOverride.bTimingCLKPreOverride    = pPanelConfig->uAttrs.sDsi.bTimingCLKPreOverride;
      sDisplayConfig.sTimingOverride.uTimingCLKPreValue       = pPanelConfig->uAttrs.sDsi.uTimingCLKPreValue;
      sDisplayConfig.sTimingOverride.bTimingCLKPostOverride   = pPanelConfig->uAttrs.sDsi.bTimingCLKPostOverride;
      sDisplayConfig.sTimingOverride.uTimingCLKPostValue      = pPanelConfig->uAttrs.sDsi.uTimingCLKPostValue;
      sDisplayConfig.sTimingOverride.bTimingTAGoOverride      = pPanelConfig->uAttrs.sDsi.bTimingTAGoOverride;
      sDisplayConfig.sTimingOverride.uTimingTAGoValue         = pPanelConfig->uAttrs.sDsi.uTimingTAGoValue;
      sDisplayConfig.sTimingOverride.bTimingTASureOverride    = pPanelConfig->uAttrs.sDsi.bTimingTASureOverride;
      sDisplayConfig.sTimingOverride.uTimingTASureValue       = pPanelConfig->uAttrs.sDsi.uTimingTASureValue;
      sDisplayConfig.sTimingOverride.bTimingTAGetOverride     = pPanelConfig->uAttrs.sDsi.bTimingTAGetOverride;
      sDisplayConfig.sTimingOverride.uTimingTAGetValue        = pPanelConfig->uAttrs.sDsi.uTimingTAGetValue;

      /* DSI Split Display */ 
      if (TRUE == pDriverCtx->bDualDSI)
      {
        sDisplayConfig.sDisplayInfo.iVisWidthPx >>=1;
        sDisplayConfig.ePLLSourceType                      = DSI_PLL_CONFIG_SPLIT_SOURCE;
      }
      else
      {
        sDisplayConfig.ePLLSourceType                      = DSI_PLL_CONFIG_INDEPENDENT_SOURCE;
      }

      sDisplayConfig.sCmdModeConfg.bEnable                 = TRUE;
      sDisplayConfig.sCmdModeConfg.eImageTrigger           = DSI_Display_ImageTrigger_NONE;
      sDisplayConfig.sCmdModeConfg.eCMDTrigger             = DSI_Display_CMDTrigger_SW;
      sDisplayConfig.sCmdModeConfg.sExtTEConfig.eExtTEMode = DSI_ExtTEMode_NONE;

      if (DSI_STATUS_SUCCESS != DSI_Display_Open(pDriverCtx->hDSIHandle[uDevIdx], &sDisplayConfig, &(pDriverCtx->hDisplayHandle[uDevIdx])))
      {
        eStatus = MDP_STATUS_FAILED;
        break;
      }
      else if (DSI_STATUS_SUCCESS != DSI_Display_Commit(pDriverCtx->hDSIHandle[uDevIdx], 0x0))
      {
        eStatus = MDP_STATUS_FAILED;
        break;
      }


      if (!(pPanelConfig->eDisplayFlags & MDP_PANEL_FLAG_DISABLE_SEAMLESS_SPLASH))
      {
          DSI_PropertyParamsType  sDSIProp;
      
          MDP_OSAL_MEMZERO(&sDSIProp, sizeof(DSI_PropertyParamsType));
          sDSIProp.uScratchReg = 0x01;

            if (DSI_STATUS_SUCCESS != DSI_Display_SetProperty(pDriverCtx->hDisplayHandle[uDevIdx], DSI_DISPLAY_PROPERTYID_SETSCRATCH1, &sDSIProp))
            {
              DSI_Log_Message(MDP_LOGLEVEL_WARN, "DisplayDxe: Set DSI Scratch Register 1\n");
            }
      }

    }
  }

  if (MDP_STATUS_OK == eStatus)
  {
    DSI_Display_ClockLaneType eClockLaneCfg = (DSI_Display_ClockLaneType)pPanelConfig->uAttrs.sDsi.uClockHSForceRequest;

    // Option to force the clock lane in to HS _before_ the init sequence. 
    if (DSI_Display_ClockLane_ForceHS == eClockLaneCfg)
    {
      DSI_PropertyParamsType  sDSIProp;
      
      MDP_OSAL_MEMZERO(&sDSIProp, sizeof(DSI_PropertyParamsType));
      sDSIProp.eLaneState = DSI_LANE_STATE_CLOCK_HS;

      for (uDevIdx = 0; uDevIdx < pDriverCtx->uNumDSIControllers; uDevIdx++)    
      {
        if (DSI_STATUS_SUCCESS != DSI_Display_SetProperty(pDriverCtx->hDisplayHandle[uDevIdx], DSI_DISPLAY_PROPERTYID_LANESTATE, &sDSIProp))
        {
          DSI_Log_Message(MDP_LOGLEVEL_WARN, "DisplayDxe: Clock lane failed to enter HS pre-init\n");
        }
      }
    }

  
    // If video transfer is required during DCS transactions then do not send the initialization sequence as it 
    // will be sent when the pixel transfer begins.
    if (FALSE == pPanelConfig->uAttrs.sDsi.bForceCmdInVideoHS)
    {
      eStatus = SendPanelXMLSequence(pDriverCtx, pPanelConfig->uAttrs.sDsi.pInitBuffer, pPanelConfig);

      // If DSC is enabled then send the PPS command to the panel
      if ((TRUE == pPanelConfig->uAttrs.sDsi.bDSCEnable) &&
          (0 == (pPanelConfig->eDisplayFlags & MDP_PANEL_FLAG_DISABLE_PPS_CMD)))
      {
         DsiPanelSendCommandSequence(pPanelConfig, MDP_PANEL_COMMAND_PPS, NULL, 0);
      }
    }
    

    // Send I2C init sequence if it is required
    if (NULL != pPanelConfig->uAttrs.sDsi.sI2CConfig.pI2CInitSequence)
    {
      if (MDP_STATUS_OK != I2CPanelSendCommandSequence(pPanelConfig, MDP_PANEL_COMMAND_INIT, pPanelConfig->uAttrs.sDsi.sI2CConfig.pI2CInitSequence, 0))
      {
        DSI_Log_Message(MDP_LOGLEVEL_WARN, "DisplayDxe: Failed to send I2C command \n");
      }
    }

    // Option to force the clock lane in to HS _after_ the init sequence. (ignore any panel command sequence failures)
    if (DSI_Display_ClockLane_ForceHS_PostInit == eClockLaneCfg)
    {
      DSI_PropertyParamsType  sDSIProp;
      
      MDP_OSAL_MEMZERO(&sDSIProp, sizeof(DSI_PropertyParamsType));
      sDSIProp.eLaneState = DSI_LANE_STATE_CLOCK_HS;

      for (uDevIdx = 0; uDevIdx < pDriverCtx->uNumDSIControllers; uDevIdx++)    
      {
        if (DSI_STATUS_SUCCESS != DSI_Display_SetProperty(pDriverCtx->hDisplayHandle[uDevIdx], DSI_DISPLAY_PROPERTYID_LANESTATE, &sDSIProp))
        {
          DSI_Log_Message(MDP_LOGLEVEL_WARN, "DisplayDxe: Clock lane failed to enter HS post-init\n");
        }
      }
    }
  }

  return eStatus;
}


/* ---------------------------------------------------------------------- */
/**
** FUNCTION: DSIDriver_RemapLane()
** 
** DESCRIPTION:
**   Remap DSI lane order
**
** ---------------------------------------------------------------------- */

MDP_Status DSIDriver_RemapLane( uint32 uLaneRemapOrder)
{
  MDP_Status       eStatus        =  MDP_STATUS_OK;
  DSI_DriverCtx   *pDriverCtx     =  DSI_GET_DRIVER_CTX();

  if (uLaneRemapOrder >= DSI_LANE_SWAPNUM)
  {
    eStatus = MDP_STATUS_BAD_PARAM;
  }
  else
  {
    DSI_PropertyParamsType   sDSIProp;
    
    MDP_OSAL_MEMZERO(&sDSIProp, sizeof(DSI_PropertyParamsType));
    MDP_OSAL_MEMCPY(&sDSIProp.aLaneMap, (void *)&sDisplayLaneSetting[uLaneRemapOrder], sizeof(sDSIProp.aLaneMap));
    
    if (DSI_STATUS_SUCCESS != DSI_Display_SetProperty(pDriverCtx->hDSIHandle[0], DSI_DISPLAY_PROPERTYID_LANEMAP, &sDSIProp))
    {
      eStatus = MDP_STATUS_FAILED;
    }
  }

  return eStatus;
}

/* ---------------------------------------------------------------------- */
/**
** FUNCTION: DSIDriver_ConfigClockLane()
** 
** DESCRIPTION:
**   Config the clock lane to HS or LP(default)
**
** ---------------------------------------------------------------------- */

MDP_Status DSIDriver_ConfigClockLane(uint32 uClkConfig)
{
  MDP_Status               eStatus        =  MDP_STATUS_OK;
  DSI_DriverCtx           *pDriverCtx     =  DSI_GET_DRIVER_CTX();
  DSI_PropertyParamsType   sDSIProp;
    
  MDP_OSAL_MEMZERO(&sDSIProp, sizeof(DSI_PropertyParamsType));

  sDSIProp.eLaneState = (MDPPLATFORM_PANEL_DETECT_FLAG_CLOCK_FORCEHS & uClkConfig)? DSI_LANE_STATE_CLOCK_HS : DSI_LANE_STATE_CLOCK_LP11;

  if (DSI_STATUS_SUCCESS != DSI_Display_SetProperty(pDriverCtx->hDSIHandle[0], DSI_DISPLAY_PROPERTYID_LANESTATE, &sDSIProp))
  {
    eStatus = MDP_STATUS_FAILED;
  }
   
  return eStatus;
}

/* ---------------------------------------------------------------------- */
/**
** FUNCTION: DSIDriver_MinimalInit()
** 
** DESCRIPTION:
**   Minimal initialization needed to do DSI read commands
**
** ---------------------------------------------------------------------- */
MDP_Status DSIDriver_MinimalInit(void)
{
  MDP_Status             eStatus              = MDP_STATUS_OK;
  DSI_DriverCtx         *pDriverCtx           = DSI_GET_DRIVER_CTX();
  void                  *pDSIBuffer           = pDriverCtx->pDSIBuffer;
  bool32                 bDMAOnly             = TRUE;      // Use DMA VBIF only for DSI command data .
  DSI_DisplayConfigType  sDisplayConfig;
  DSI_DeviceConfigType   sDeviceConfig;
  MDPExternalClockEntry  sDSIExtClocks[] =
                                     {
                                       {"mdss_esc0_clk",     0, 1, 0, 0, 0, 0},  // Index 0
                                       {"mdss_pclk0_clk",    0, 0, 0, 0, 0, 0},  // Index 1
                                       {"mdss_byte0_clk",    0, 0, 0, 0, 0, 0},  // Index 2
                                       {"\0",                0, 0, 0, 0, 0, 0},  
                                     };
  
  if (TRUE == bDMAOnly)
  {
    if (NULL == pDSIBuffer)
    {
      if (NULL == (pDSIBuffer = MDP_OSAL_CALLOC(DSI_DMA_BUFFER_SIZE)))
      {
        DSI_Log_Message(MDP_LOGLEVEL_INFO, "DixplayDxe: UncachedAllocateZeroPool failed\n");
        eStatus = MDP_STATUS_NO_RESOURCES;
      }
    }
  }

  if (MDP_STATUS_OK == eStatus)
  {
    //
    // Setup clocks using default frequencies, any freq should work to do a DSI read
    //
    eStatus = MDPSetupClocks(MDP_CLOCKTYPE_DSI0, (MDPExternalClockEntry *)&sDSIExtClocks);


    //
    // Open DSI Host
    //
    if (MDP_STATUS_OK == eStatus)  
    {
      MDP_OSAL_MEMZERO(&sDeviceConfig, sizeof(DSI_DeviceConfigType));
      sDeviceConfig.eDeviceID                            = DSI_DeviceID_0;
      sDeviceConfig.bLittleEndian                        = TRUE;
      sDeviceConfig.bNonCompliantMode                    = FALSE;
      sDeviceConfig.bEnableCRCCheck                      = FALSE;
      sDeviceConfig.bEnableECCCheck                      = FALSE;
      sDeviceConfig.sMemoryConfig.pCPUAddress            = pDSIBuffer;
      sDeviceConfig.sMemoryConfig.sMDPAddress.u.uLowPart = (UINTN)pDSIBuffer;
      sDeviceConfig.sMemoryConfig.uSize                  = DSI_DMA_BUFFER_SIZE;
      sDeviceConfig.bEnableLP11AtInit                    = FALSE;
      sDeviceConfig.uFlags                              |= (TRUE == bDMAOnly) ? DSI_DEVICE_FLAGS_DSI_DCS_DMA_ONLY : 0;
      sDeviceConfig.bPhyDCDCMode                         = TRUE;

      if (DSI_STATUS_SUCCESS != DSI_Device_Open(&sDeviceConfig, &(pDriverCtx->hDSIHandle[0])))
      {
        eStatus = MDP_STATUS_FAILED;
      }
      else
      {
        MDPPlatformParams sMDPPlatformParam;
        int               i;
      
        /* Handle Panel reset */
        MDP_OSAL_MEMZERO(&sMDPPlatformParam, sizeof(sMDPPlatformParam));

        /* use default timing value here since panel reset info at xml files is not parsed yet */
        sMDPPlatformParam.sPlatformPanelReset.uResetGpio         = 0; /* use default reset gpio */
        sMDPPlatformParam.sPlatformPanelReset.uAssertState       =  MDP_RESET_STATE_LEVEL_LOW;
        /* 
         * default to 1 ms for pre reset time should be sufficient 
         * since start from power on to here should be more than 10ms already
         */
        sMDPPlatformParam.sPlatformPanelReset.uPreResetTimeUs    = MDP_GPIO_PRE_RESET_TIME;  /* default to 1000 us */
        sMDPPlatformParam.sPlatformPanelReset.uResetAssertTimeUs = MDP_GPIO_RESET_ASSERT_TIME;  /* defuault to 10 us */
        sMDPPlatformParam.sPlatformPanelReset.uPostResetTimeUs   = MDP_GPIO_POST_RESET_TIME;  /* default to 10000 us */
      
        if (MDP_STATUS_OK != MDPPlatformConfigure(MDP_DISPLAY_PRIMARY, MDPPLATFORM_CONFIG_RESETPANEL, &sMDPPlatformParam))
        {
          DSI_Log_Message(MDP_LOGLEVEL_INFO, "DixplayDxe: MDPPlatformConfigure(MDPPLATFORM_CONFIG_RESETPANEL) failed\n" );
        }

        /* Setup DSI controller configuration */
        MDP_OSAL_MEMZERO(&sDisplayConfig, sizeof(DSI_DisplayConfigType));
        sDisplayConfig.sDisplayInfo.eDisplayId   = DSI_ChannelID_Video0;
        sDisplayConfig.sDisplayInfo.eDisplayVC   = DSI_Display_VC_0;
        sDisplayConfig.sDisplayInfo.eColorFormat = DSI_COLOR_RGB_888_24BPP;
        sDisplayConfig.sDisplayInfo.uNumLanes    = 4;
      
        for (i=0; i<(DSI_LaneMap_MAX-1); i++)
        {
          sDisplayConfig.eDisplayLane[i] = sDisplayLaneSetting[0][i];
        }

        sDisplayConfig.sCmdModeConfg.bEnable                 = TRUE;
        sDisplayConfig.sCmdModeConfg.eImageTrigger           = DSI_Display_ImageTrigger_NONE;
        sDisplayConfig.sCmdModeConfg.eCMDTrigger             = DSI_Display_CMDTrigger_SW;
        sDisplayConfig.sCmdModeConfg.sExtTEConfig.eExtTEMode = DSI_ExtTEMode_NONE;
        sDisplayConfig.ePLLSourceType                        = DSI_PLL_CONFIG_SPLIT_SOURCE;
        /* During minimal init,as cxo is used to clock the lanes, clock lane won't enter the HS even if the corresponding register is programmed.  
           In the normal init, since DSI_PLL is selected as the clock source, HS mode works.
           Hence, to enable DSI_PLL in minimal init, the uBitClockFreq is hardcoded to a non-zero value. 
        */
        sDisplayConfig.sDisplayInfo.uBitClockFreq            = DSI_DEFAULT_BITCLOCK_FREQ;

        /* Save Display Config */
        if (DSI_STATUS_SUCCESS != DSI_Display_Open(pDriverCtx->hDSIHandle[0], &sDisplayConfig, &(pDriverCtx->hDisplayHandle[0])))
        {
          eStatus = MDP_STATUS_FAILED;
        }
        else if (DSI_STATUS_SUCCESS != DSI_Display_Commit(pDriverCtx->hDSIHandle[0], 0))
        {
          eStatus = MDP_STATUS_FAILED;
        }
      }
    }
  }

  return eStatus;
}

/* ---------------------------------------------------------------------- */
/**
** FUNCTION: DSIDriver_GetCalibrationCodes()
** 
** DESCRIPTION:
**   Get dynamic refresh calibration codes
**
** ---------------------------------------------------------------------- */
MDP_Status DSIDriver_GetCalibrationCodes(uint32               uDSIIndex, MDP_PLLCodesInfo *psPllCodesInfo)
{
  MDP_Status             eStatus              = MDP_STATUS_OK;
  DSI_DriverCtx         *pDriverCtx           = DSI_GET_DRIVER_CTX();
  DSI_PropertyParamsType sParams;
  DSI_Device_IDType      eDeviceId            = (DSI_Device_IDType)uDSIIndex;

  if (NULL == psPllCodesInfo)
  {
    eStatus = MDP_STATUS_BAD_PARAM;
  }
  else
  {
    if(DSI_DeviceID_MAX > eDeviceId)
    {
      MDP_OSAL_MEMZERO(&sParams, sizeof(DSI_PropertyParamsType));
            
      if(DSI_STATUS_SUCCESS != DSI_Display_GetProperty(pDriverCtx->hDisplayHandle[eDeviceId], DSI_DISPLAY_PROPERTYID_GETCALCODES, &sParams))
      {
        eStatus = MDP_STATUS_FAILED;
      }
      else
      {
        DSI_OSAL_MemCpy((void *) psPllCodesInfo, (void*)&sParams.sDisplayInfo.sPllCodesInfo, sizeof(MDP_PLLCodesInfo));
      }
    }
    else
    {
      eStatus = MDP_STATUS_NO_RESOURCES;
    }
  }

  return eStatus;
}

/*------------------------------------------------------------------------
Panel Driver Helper Functions
-------------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
/**
** FUNCTION: DsiPanelInit()
** 
** DESCRIPTION:
**   DSI Clocks and timing initialization
**
** ---------------------------------------------------------------------- */
MDP_Status DsiPanelInit(MDP_Panel_AttrType    *pPanelInfo)
{
  MDP_Status        eStatus         = MDP_STATUS_OK;
  MDPPlatformParams sMDPPlatformParam;

  MDP_OSAL_MEMZERO(&sMDPPlatformParam, sizeof(MDPPlatformParams));  

  /* Initialize the DSI controller in to a known state */
  if (MDP_STATUS_OK != DSIDriver_Init(pPanelInfo)) 
  {
    DSI_Log_Message(MDP_LOGLEVEL_INFO, "DixplayDxe: DSIDriver_Init() failed\n" );
    eStatus = MDP_STATUS_FAILED;
  }
  else 
  {
    sMDPPlatformParam.sPlatformPanelReset.uResetGpio         = pPanelInfo->uResetGpio;
    /* the unit for reset time is micro second */
    sMDPPlatformParam.sPlatformPanelReset.uAssertState       = pPanelInfo->uAttrs.sDsi.aResetInfo[0];
    sMDPPlatformParam.sPlatformPanelReset.uResetAssertTimeUs = pPanelInfo->uAttrs.sDsi.aResetInfo[1];
    sMDPPlatformParam.sPlatformPanelReset.uPreResetTimeUs    = pPanelInfo->uAttrs.sDsi.aResetInfo[2];
    sMDPPlatformParam.sPlatformPanelReset.uPostResetTimeUs   = pPanelInfo->uAttrs.sDsi.aResetInfo[3];
    sMDPPlatformParam.sPlatformPanelReset.uAssertOnPowerDown = pPanelInfo->uAttrs.sDsi.aResetInfo[4];

    if (sMDPPlatformParam.sPlatformPanelReset.uAssertState >=  MDP_RESET_STATE_MAX)
    {
       sMDPPlatformParam.sPlatformPanelReset.uAssertState =  MDP_RESET_STATE_LEVEL_LOW;
    }

    /* 
     * default to 1 ms for pre reset time should be sufficient 
     * since start from power on to here should be more than 10ms already
     */
    if (sMDPPlatformParam.sPlatformPanelReset.uPreResetTimeUs == 0)
    {
       sMDPPlatformParam.sPlatformPanelReset.uPreResetTimeUs = MDP_GPIO_PRE_RESET_TIME;  /* default to 1000 us */
    }

    if (sMDPPlatformParam.sPlatformPanelReset.uResetAssertTimeUs == 0)
    {
       sMDPPlatformParam.sPlatformPanelReset.uResetAssertTimeUs = MDP_GPIO_RESET_ASSERT_TIME;  /* defuault to 10 us */
    }

    if (sMDPPlatformParam.sPlatformPanelReset.uPostResetTimeUs == 0)
    {
       sMDPPlatformParam.sPlatformPanelReset.uPostResetTimeUs = MDP_GPIO_POST_RESET_TIME;  /* default to 10000 us */
    }

    /* Toggle panel reset */
    if (MDP_STATUS_OK != MDPPlatformConfigure(pPanelInfo->eDisplayId, MDPPLATFORM_CONFIG_RESETPANEL, &sMDPPlatformParam))   
    {
      DSI_Log_Message(MDP_LOGLEVEL_INFO, "DixplayDxe: MDPPlatformConfigure(MDPPLATFORM_CONFIG_RESETPANEL) failed\n" );
      // Warning, non-fatal
    }

    /* Configure the Panel */
    if (MDP_STATUS_OK != DSIDriver_SetMode(pPanelInfo))   
    {
      DSI_Log_Message(MDP_LOGLEVEL_INFO, "DixplayDxe: DSIDriver_SetMode() failed\n" );
      eStatus = MDP_STATUS_FAILED;
    }
  }

  return eStatus;
}


/* ---------------------------------------------------------------------- */
/**
** FUNCTION: DsiPanelShutdown()
** 
** DESCRIPTION:
**   Gracefully shutdown the controller and panel
**
*//* -------------------------------------------------------------------- */
void DsiPanelShutdown(MDP_Panel_AttrType *pPanelConfig)
{
  if (NULL == pPanelConfig)
  {
    // Invalid panel config, nothing to turn off
  }
  else
  {
    DSI_DriverCtx        *pDriverCtx    = DSI_GET_DRIVER_CTX();
    uint32                uDevIdx;  

    // Only send a termination sequence if the panel has been setup already
    for (uDevIdx = 0; uDevIdx < pDriverCtx->uNumDSIControllers; uDevIdx++)
    {    
      // Send termination sequence (if present) to controller 0
      if (DSI_DeviceID_0 == uDevIdx)
      {
        // Force the DSI controller to idle, only required in video mode
        if (MDP_DISPLAY_CONNECT_PRIMARY_DSI_VIDEO == pPanelConfig->ePhysConnect)
        {
          DSI_PropertyParamsType  sDSIProperty;
          
          MDP_OSAL_MEMZERO(&sDSIProperty, sizeof(DSI_PropertyParamsType));
          if (DSI_STATUS_SUCCESS != DSI_Display_SetProperty(pDriverCtx->hDisplayHandle[uDevIdx], DSI_DISPLAY_PROPERTYID_LP11, &sDSIProperty))
          {
            DSI_Log_Message(MDP_LOGLEVEL_INFO, "DSI_Display_SetProperty(DSI_DISPLAY_PROPERTYID_LP11) Failed");
          }

          if (DSI_STATUS_SUCCESS != DSI_Display_SetProperty(pDriverCtx->hDisplayHandle[uDevIdx], DSI_DISPLAY_PROPERTYID_RESETCORE, &sDSIProperty))
          {
            DSI_Log_Message(MDP_LOGLEVEL_INFO, "DSI_Display_SetProperty(DSI_DISPLAY_PROPERTYID_RESETCORE) Failed");
          }
        }
        
        // If video transfer is required during DCS transactions then do not send the termination sequence as 
        // the termination commands have been aleady sent.
        if (FALSE == pPanelConfig->uAttrs.sDsi.bForceCmdInVideoHS)
        {
          if (MDP_STATUS_OK != SendPanelXMLSequence(pDriverCtx, pPanelConfig->uAttrs.sDsi.pTermBuffer, pPanelConfig))
          {
            DSI_Log_Message(MDP_LOGLEVEL_INFO, "DixplayDxe: SendPanelXMLSequence() failed on device%d\n", uDevIdx );
              // Warn and continue
          }
        }
      }
    }

    // De-init I2C configuration: Ignore any errors
    if (NULL != pPanelConfig->uAttrs.sDsi.sI2CConfig.pI2CInitSequence)
    {
       I2CDriver_Deinit(pPanelConfig); 
    }
  }
}




/* ---------------------------------------------------------------------- */
/**
** FUNCTION: DsiPanelSendCommandSequence()
** 
** DESCRIPTION:
**   Send DSI command sequence
**
** ---------------------------------------------------------------------- */
MDP_Status DsiPanelSendCommandSequence(MDP_Panel_AttrType *pPanelInfo, MdpPanelCommandType eCommand, void *pCustomCommand, uint32 uFlags)
{
  MDP_Status            eStatus     =  MDP_STATUS_OK;
  DSI_DriverCtx        *pDriverCtx  =  DSI_GET_DRIVER_CTX();

  switch (eCommand)
  {
    case MDP_PANEL_COMMAND_INIT:
    {
      eStatus = SendPanelXMLSequence(pDriverCtx, pPanelInfo->uAttrs.sDsi.pInitBuffer, pPanelInfo);
    }
    break;

    case MDP_PANEL_COMMAND_TERM:
    {
      eStatus = SendPanelXMLSequence(pDriverCtx, pPanelInfo->uAttrs.sDsi.pTermBuffer, pPanelInfo);
    }
    break;

    case MDP_PANEL_COMMAND_PPS:
    {
      eStatus = SendPanelPPSRawBuffer(pDriverCtx, pPanelInfo);
    }
    break;

    default:
      eStatus = MDP_STATUS_NOT_SUPPORTED;     
  }
  return eStatus;
}




/* ---------------------------------------------------------------------- */
/**
** FUNCTION: DsiPanelDumpRegisters()
** 
** DESCRIPTION: Helper function to dump panel registers
** 
**
*//* -------------------------------------------------------------------- */
MDP_Status DsiPanelDumpRegisters(void)
{
  uint32 uReadSize  = 1024;
  uint32 uDataWidth = 4;

  uint16 uAddress = 0;
  uint32 uResponse[2];
  uint32 uReponseSize;

  uint16 uIndex;

  DSI_Log_Message(MDP_LOGLEVEL_ERROR, "Offset   Data\n");
  DSI_Log_Message(MDP_LOGLEVEL_ERROR, "-------+------------------------------------------------------------\n");

  for (uIndex=0;uIndex<uReadSize;uIndex++)
  {
      if (0 == (uIndex % uDataWidth))
      {
        DSI_Log_Message(MDP_LOGLEVEL_ERROR, "0x%04x   ", uIndex);
      }

      uAddress     = uIndex;
      uReponseSize = sizeof(uResponse);
      if (MDP_STATUS_OK == DSIDriver_Read(0x06, 
                                          (void*)&uAddress, 
                                          sizeof(uAddress), 
                                          (void*) &uResponse, &uReponseSize))
      {
        DSI_Log_Message(MDP_LOGLEVEL_ERROR, "0x%08x ", uResponse[0]);        
      }
      else
      {
        DSI_Log_Message(MDP_LOGLEVEL_ERROR, "--ERROR-- ");        
     
      }


      // If last item, add carriage return
      if (0 == ((uIndex+1) % uDataWidth))
      {
         DSI_Log_Message(MDP_LOGLEVEL_ERROR, "\n");        
      }
  }

  return MDP_STATUS_OK;
}


/*------------------------------------------------------------------------
Local Functions
-------------------------------------------------------------------------- */

/* ---------------------------------------------------------------------- */
/**
** FUNCTION: SendPanelPPSRawBuffer()
** 
** DESCRIPTION:
**   Local helper function to send a DSI command sequence to the panel
**   This will package the raw PPS buffer and convert it to the XML
**   header format and then send the command using the
**   SendPanelXMLSequence() API
** 
** ---------------------------------------------------------------------- */
static MDP_Status SendPanelPPSRawBuffer(DSI_DriverCtx *pDriverCtx, MDP_Panel_AttrType *pPanelConfig)
{
  MDP_Status  eStatus         = MDP_STATUS_OK;
  uint8 *pDscHeader            = (uint8 *)pPanelConfig->uAttrs.sDsi.pDSCPpsBuffer;

  // Prepare DSC PPS header data

  // First byte holds the lenght of the packet (128byte payload, 1byte = Command type)
  *pDscHeader++ = MDP_DSI_DSC_PAYLOAD_SIZE + MDP_DSI_DSC_XML_HEADER_SIZE;
  *pDscHeader++ = 0;
  *pDscHeader++ = 0;
  *pDscHeader++ = 0;

  // PPS Command header 
  *pDscHeader++ = DSI_DATATYPE_PPSPACKET;

  // Send the PPS command to the panel
  eStatus = SendPanelXMLSequence(pDriverCtx, pPanelConfig->uAttrs.sDsi.pDSCPpsBuffer, pPanelConfig);
  
  return eStatus;
}

/** ---------------------------------------------------------------------- */
/**
** FUNCTION: SendPanelXMLSequence()
** 
** DESCRIPTION:
**   Local helper function to send a DSI command sequence to the panel
**
** ---------------------------------------------------------------------- */
static MDP_Status SendPanelXMLSequence(DSI_DriverCtx *pDriverCtx, void *pDSICommandBuffer, MDP_Panel_AttrType *pPanelConfig)
{
  MDP_Status            eStatus         = MDP_STATUS_OK;
  uint32                uDevIdx;
  static uint8          uBroadcastMask  = DSI_CONTROLLER_0;     //Default to sending commands over first DSI controller.
  

  if ((NULL == pDriverCtx  ) ||
      (NULL == pPanelConfig))
  {
      eStatus = MDP_STATUS_BAD_PARAM;
  }
  else if (NULL == pDSICommandBuffer)
  {
    // No commands, return successful
  }
  else
  {
    DSI_WritePacketType   sWritePacket;
    uint8                *pSrc              = (uint8 *)pDSICommandBuffer;
    XML_BinaryPacketType  sXMLBinaryData;
    uint32                uHeaderSize       = (uint32)((UINTN)&(((XML_BinaryPacketType*)pSrc)->pData) - (UINTN)&(((XML_BinaryPacketType*)pSrc)->uPacketLength));
    bool32                bColorFillEnabled = FALSE;

    // Use Unaligned Memory read from DSI Command buffer (Required for THUMB2 mode)
    MDP_ReadUnaligned_U32(&sXMLBinaryData.uPacketLength, pSrc);
    sXMLBinaryData.pData = (uint8* )&(((XML_BinaryPacketType*)pSrc)->pData);

    /*Send Config Command to configure DSI Panel */ 
    MDP_OSAL_MEMZERO(&sWritePacket, sizeof(DSI_WritePacketType));
    sWritePacket.eDisplayVC       = (DSI_Display_VCType)pPanelConfig->uAttrs.sDsi.eDisplayVC; 
    sWritePacket.bHighSpeedMode   = FALSE;
    sWritePacket.bQueuePacket     = FALSE;
    sWritePacket.bBTARequest      = FALSE;

    /* Parse init command packets */
    while (0 != sXMLBinaryData.uPacketLength)
    {
      uint8 *pDsiCmd = (uint8*)sXMLBinaryData.pData;

      if (0 == sXMLBinaryData.uPacketLength)
      {
        // A null packet signifies end of data
        break;
      }
      else if (DSI_CMD_CUSTOM_DELAY == *pDsiCmd)
      {
        uint8 uDelayLength = *(++pDsiCmd);

        // Milliseconds delay
        MDP_OSAL_DELAYMS(uDelayLength); 
      }
      else if (DSI_CMD_CUSTOM_COLOR_FILL == *pDsiCmd)
      {
        uint32                    uFilledColor   = 0;
        uint32                    uCount         = 0;
        DSI_PropertyParamsType    sParams;

        bColorFillEnabled = TRUE;
          
        /* if there's payload, then there must be 3 bytes for R, G, B */
        if (DSI_CMD_CUSTOM_COLOR_FILL_PAYLOAD_LENGTH == (sXMLBinaryData.uPacketLength-1))
        {
          /* uFilledColor is in RedCr, GreenY, BlueCb order */
          uFilledColor = ((pDsiCmd[1]<<16) | (pDsiCmd[2]<<8) | pDsiCmd[3]);
        }

        /* enable constant color output */
        MDPOutputConstantColor(pPanelConfig, uFilledColor, TRUE);

        /* In case of command mode panels, wait for constant color transfer done 
         * No wait is needed for video mode panels as frame xfer is via video engine and cmd engine is idle 
         */           
        if (MDP_DISPLAY_CONNECT_PRIMARY_DSI_CMD == pPanelConfig->ePhysConnect)
        {
          for (uDevIdx = 0; uDevIdx < pDriverCtx->uNumDSIControllers; uDevIdx++)
          {
            do
            {
              /* sleep */
              MDP_OSAL_DELAYMS(DSI_STATUS_POLLING_CYCLE);

              /* check controller status. CMD_MDP_ENGINE should be idle once transfer done */
              MDP_OSAL_MEMZERO(&sParams, sizeof(DSI_PropertyParamsType));
              if (DSI_STATUS_SUCCESS == DSI_Display_GetProperty(pDriverCtx->hDisplayHandle[uDevIdx], DSI_DISPLAY_PROPERTYID_CONTROLLERSTATUS, &sParams))
              {
                if (0 == (DSI_CONTROLLER_STATUS_CMD_MDP_ENGINE_BUSY & sParams.uControllerStatus))
                {
                  break;
                }
              }
              else
              {
                eStatus = MDP_STATUS_FAILED ;
              }
              uCount ++;
            } while((uCount * DSI_STATUS_POLLING_CYCLE) <= DSI_STATUS_CHECK_MAX_TIME);
          }     
        }
      }
      else if (DSI_CMD_CUSTOM_BROADCAST ==  *pDsiCmd)
      {
        uBroadcastMask = ((DSI_CONTROLLER_0 | DSI_CONTROLLER_1) & (*(++pDsiCmd)));
        if (0 == uBroadcastMask)
        {
          uBroadcastMask = DSI_CONTROLLER_0;
        }
      }
      else
      {
        sWritePacket.uCmdDataType     = *pDsiCmd++;
        sWritePacket.puPayload        = (uint8*)pDsiCmd;
        sWritePacket.bPayloadLen      = sXMLBinaryData.uPacketLength-1; // Packet length minus the command

        for (uDevIdx = 0; uDevIdx < pDriverCtx->uNumDSIControllers; uDevIdx++)
        {
          if ( 0 == (uBroadcastMask & (1<<uDevIdx))) 
          {
            //skip if broadcast mask is not set on this DSI controller device ID.
            continue;
          }

          if (DSI_STATUS_SUCCESS != DSI_Display_ClientWrite(pDriverCtx->hDisplayHandle[uDevIdx], &sWritePacket))
          {
            // Warn and continue
            DSI_Log_Message(MDP_LOGLEVEL_WARN, "DisplayDxe: Packet transmission failed CMD:%d Length:%d\n", sWritePacket.puPayload, sWritePacket.bPayloadLen);
            break;
          }
        }
      }

      // Go to next packet (header + payload size)
      pSrc += (uint8)(sXMLBinaryData.uPacketLength + uHeaderSize);
      // Unaligned Memory read 
      MDP_ReadUnaligned_U32(&sXMLBinaryData.uPacketLength, pSrc);
      sXMLBinaryData.pData = pSrc + uHeaderSize;
    }

    /* Panel init sequence done */
    
    if (TRUE == bColorFillEnabled)
    {
      /* Disable constant color output and revert to standard control path settings */      
      MDPOutputConstantColor(pPanelConfig, 0, FALSE);
    }    
  }

  return eStatus;
}



#ifdef __cplusplus
}
#endif

