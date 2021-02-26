/*=============================================================================
 
  File: DisplayDxe.c
 
  Display driver for UEFI
  
 
    Copyright (c) 2011-2018 Qualcomm Technologies, Inc.
    All Rights Reserved
    Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/
#include <PiDxe.h>
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiCfgLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Library/UncachedMemoryAllocationLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Library/BaseLib.h>
#include <Library/QcomBaseLib.h>
#include <Library/QcomLib.h>
#include "Protocol/EFIDisplayPwrCtrl.h"
#include "Protocol/EFIDisplayPwr.h"
#include "Protocol/EFIDisplayUtils.h"
#include "DisplayDxe.h"
#include "MDPLib.h"
#include "MDPSystem.h"


/**********************************************************************************************************************
    Static/Globals
**/

EFI_EVENT EfiExitBootServicesEvent      = (EFI_EVENT)NULL;

/* Block IO Refresh Event */
EFI_EVENT gBlockIoRefreshEvt            = (EFI_EVENT) NULL;

/* Display power control protocol definition */
extern EFI_QCOM_DISPLAY_PWR_CTRL_PROTOCOL  gDisplayPwrCtrlProtocolImplementation;

/* Display power protocol definition */
extern EFI_DISPLAY_POWER_PROTOCOL  gDisplayPwrProtocolImplementation;

/* Display Utils protocol definition */
extern EFI_QCOM_DISPLAY_UTILS_PROTOCOL  gQcomDisplayUtilsProtocolImplementation;

/* Block IO Refresh GUID */
extern EFI_GUID gBlockIoRefreshGuid;

/* Select Mode */
static EFI_STATUS DisplayDxeSelectMode(MDP_Display_IDType eDisplayId, MDP_DetectParamType  *pDisplayModes);

/* Display Blit function */
static void DisplayDxeBltInternal (
  UINT8   *pSrc,
  UINT8   *pDst,
  UINTN    uSrcX,
  UINTN    uSrcY,
  UINTN    uSrcWidth,
  UINTN    uSrcHeight,
  UINTN    uSrcStride,
  UINTN    uDstX,
  UINTN    uDstY,
  UINTN    uDstStride,
  UINTN    uBytesPerPixel
  );

/* Display frame buffer Cache maintenance */
static void FlushStaleLines(VOID *pAddress, uint32 uLength);

/* Static information about the current display mode */
static DisplayDxe_ModeInfo   gModeInfo;

/* Handle to the UEFI protocol registration */


/* Function table pointer to the supported protocol functions */
static EFI_GRAPHICS_OUTPUT_PROTOCOL gDisplayDxeOutputProtocol = {
    &DisplayDxe_QueryMode,
    &DisplayDxe_SetMode,
    &DisplayDxe_Blt,
    &gModeInfo.sProtocolInfo
};

/* DisplayDXE Device path */
static EFI_DISPLAY_DEVICE_PATH DisplayDevicePath = 
{
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
        (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID
  },
  { END_DEVICE_PATH_TYPE, END_ENTIRE_DEVICE_PATH_SUBTYPE, {sizeof (EFI_DEVICE_PATH_PROTOCOL), 0}}
};

/* Frame buffer Resolution Limit Table, table is landscape but check is done for both orientations. */
const MDP_ResolutionType gFBResLimit[] = 
{
    {4096, 2160},   // Index 0: 4k    ~35,.3MB @ 32bpp
    {3840, 2160},   // Index 1: UHD   ~33.1MB  @ 32bpp
    {2560, 1440},   // Index 2: WQHD  ~14.7MB  @ 32bpp
    {1920, 1080},   // Index 3: FHD   ~8.2MB   @ 32bpp
    {1280, 720},    // Index 4: HD    ~3.6MB   @ 32bpp
};

/* Total number of entries in the table */
#define DISPLAY_FB_RES_MAX            (sizeof(gFBResLimit)/sizeof(MDP_ResolutionType))


//-------------------------------------------------------------------------------------------------
//  DisplayDxeExitBootServicesEvent
//
//  @brief
//      Callback function when UEFI Exit, free resources.
//
//  @params
//      [IN] Event
//          Event handle.
//      [IN] Context
//          Callback parameter context.
//
//  @return
//      None
//-------------------------------------------------------------------------------------------------
//
VOID EFIAPI DisplayDxeExitBootServicesEvent( IN EFI_EVENT  Event,  IN VOID *Context )
{

  if (EFI_SUCCESS != gBS->CloseEvent(gModeInfo.UIActiveEvent))
  {
    DEBUG((EFI_D_ERROR, "DisplayDxe: Failed to Close event UI_Active\n"));
  }

  if (EFI_SUCCESS != gBS->CloseEvent(gModeInfo.UIIdleEvent))
  {
    DEBUG((EFI_D_ERROR, "DisplayDxe: Failed to Close event UI_Idle\n"));
  }

  if ((NULL != gBlockIoRefreshEvt) &&
      (EFI_SUCCESS != gBS->CloseEvent(gBlockIoRefreshEvt)))
  {
    DEBUG((EFI_D_ERROR, "DisplayDxe: Failed to Close event BlockIORefresh\n"));
  }

  /* Un register LPM CB */
  if (EFI_SUCCESS != DisplayPwr_InitLPMSupport(FALSE))
  {
    DEBUG((EFI_D_WARN, "DisplayDxe: Failed to Unregister LPM CB!\n"));
  }

  //UEFI is shutting down - Handle it in MDP
  MDPExitBoot(gModeInfo.uExitFlags);
}


//-------------------------------------------------------------------------------------------------
//  BlockIoCallback
//
//  @brief
//      Callback function when GPT partition table is updated to invalidate all Block Io handles.
//
//  @params
//      [IN] Event
//          Event handle.
//      [IN] Context
//          Callback parameter context.
//
//  @return
//      None
//-------------------------------------------------------------------------------------------------
//
VOID EFIAPI BlockIoCallback(IN EFI_EVENT  Event, IN VOID *Context)
{
  /* GPT has been updated and the splash partition is invalid. So skip writing to it */
  gModeInfo.uExitFlags |= MDP_EXIT_FLAG_INVALIDATE_GPT;
}



//-------------------------------------------------------------------------------------------------
//  UIActiveEventCallBack
//
//  @brief
//      Callback function when UI_Active event is signaled, turn on display
//
//  @params
//      [IN] Event
//          Event handle.
//      [IN] Context
//          Callback parameter context.
//
//  @return
//      None.
//-------------------------------------------------------------------------------------------------
//
VOID EFIAPI UIActiveEventCallBack( IN EFI_EVENT  Event, IN VOID *Context)
{
  EFI_DISPLAY_TYPE               eDisplayType = EFI_DISPLAY_TYPE_ALL;
  EFI_DISPLAY_POWER_CTRL_STATE   ePowerState  = EFI_DISPLAY_POWER_STATE_ON;

  /* Turn on the display */
  if (EFI_SUCCESS !=  EFI_DisplayPanelPowerControl(eDisplayType, ePowerState))
  {
    DEBUG((EFI_D_ERROR, "DisplayDxe: EFI_DisplayPanelPowerControl() failed to turn on display!\n"));
  }

}

//-------------------------------------------------------------------------------------------------
//  UIIdleEventCallBack
//
//  @brief
//      Callback function when UI_Idle event is signaled, turn off display
//
//  @params
//      [IN] Event
//          Event handle.
//      [IN] Context
//          Callback parameter context.
//
//  @return
//      None.
//-------------------------------------------------------------------------------------------------
//
VOID EFIAPI UIIdleEventCallBack( IN EFI_EVENT  Event, IN VOID *Context)
{
  EFI_DISPLAY_TYPE               eDisplayType = EFI_DISPLAY_TYPE_ALL;
  EFI_DISPLAY_POWER_CTRL_STATE   ePowerState  = EFI_DISPLAY_POWER_STATE_OFF;

  /* Turn off the display */
  if (EFI_SUCCESS !=  EFI_DisplayPanelPowerControl(eDisplayType, ePowerState))
  {
    DEBUG((EFI_D_ERROR, "DisplayDxe: EFI_DisplayPanelPowerControl() failed to turn off display!\n"));
  }

}
/**********************************************************************************************************************
     Public APIs
**/
/**
  Initialize the state information for the Display Dxe

  @param  ImageHandle   of the loaded driver
  @param  SystemTable   Pointer to the System Table

  @retval EFI_SUCCESS           Protocol registered
  @retval EFI_OUT_OF_RESOURCES  Cannot allocate protocol data structure
  @retval EFI_DEVICE_ERROR      Hardware problems

**/
EFI_STATUS
EFIAPI
DisplayDxeInitialize (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS         eStatus            = EFI_SUCCESS;
  EFI_GUID           sOutputGUID        = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
  EFI_HANDLE         hUEFIDisplayHandle = NULL;
  MDP_InitParamsType sInitParam;
  EFI_GUID           UIActiveEventGuid  = EFI_UI_ACTIVE_EVENT_GUID;
  EFI_GUID           UIIdleEventGuid    = EFI_UI_IDLE_EVENT_GUID;

  // Clear out global structures
  MDP_OSAL_MEMZERO(&gModeInfo, sizeof(DisplayDxe_ModeInfo));
  MDP_OSAL_MEMZERO(&sInitParam, sizeof(MDP_InitParamsType));

  /* Register for an ExitBootServicesEvent */
  if (EFI_SUCCESS != gBS->CreateEvent(EVT_SIGNAL_EXIT_BOOT_SERVICES, TPL_CALLBACK, DisplayDxeExitBootServicesEvent, NULL, &EfiExitBootServicesEvent))
  {
    DEBUG((EFI_D_INFO, "DisplayDxe: Failed to create EXIT_BOOT_SERVICES event!\n"));
  }

  /* Register for BlockIoRefreshEvent */
  if (EFI_SUCCESS != gBS->CreateEventEx(EVT_NOTIFY_SIGNAL, TPL_CALLBACK, BlockIoCallback, NULL, &gBlockIoRefreshGuid, &gBlockIoRefreshEvt))
  {
    DEBUG((EFI_D_INFO, "DisplayDxe: Failed to create BlockIoRefresh event!\n"));
  }

  /* Create and Register for UI_Active Event*/
  if (EFI_SUCCESS != gBS->CreateEventEx(EVT_NOTIFY_SIGNAL, TPL_CALLBACK, UIActiveEventCallBack, NULL, &UIActiveEventGuid, &gModeInfo.UIActiveEvent))
  {
    DEBUG((EFI_D_INFO, "DisplayDxe: Failed to create UI_Active event!\n"));
  }
  
  /* Create and Register for UI_Idle Event*/
  if (EFI_SUCCESS != gBS->CreateEventEx(EVT_NOTIFY_SIGNAL, TPL_CALLBACK, UIIdleEventCallBack, NULL, &UIIdleEventGuid, &gModeInfo.UIIdleEvent))
  {
    DEBUG((EFI_D_INFO, "DisplayDxe: Failed to create UI_Idle event!\n"));
  }

  if (EFI_SUCCESS != DisplayPwr_InitLPMSupport(TRUE))
  {
    DEBUG((EFI_D_WARN, "DisplayDxe: Failed to register LPM CB!\n"));
  }

  // Initialize Apps BootLoader (ABL) interface
  // It checks for panel override, if any, set by ABL and sets up parameters which will be used by MDPInit
  if (MDP_STATUS_OK != MDPSetProperty(MDP_DISPLAY_PRIMARY, MDP_DISPLAY_PROPERTY_ABL_INTERFACE_INIT, NULL))
  {
    DEBUG((EFI_D_INFO, "DisplayDxe: Failed to initialize ABL!\n"));
  }

  // Initialize the MDP 
  if (MDP_STATUS_OK != MDPInit(&sInitParam, MDP_INIT_FLAG_MMU_INIT))
  {
      DEBUG ((EFI_D_INFO, "DisplayDxe: MDP init failed!\n"));      
      eStatus = EFI_DEVICE_ERROR;
  }
  else
  {
        MDP_PowerParamsType      sPowerParams;
        MDP_DetectParamType      sDetectParams;
        MDP_PropertiesParamType  sDisplayProp;

        MDP_OSAL_MEMZERO(&sDisplayProp, sizeof(MDP_PropertiesParamType));
        MDP_OSAL_MEMZERO(&sPowerParams, sizeof(MDP_PowerParamsType));

        sPowerParams.bPowerOn = TRUE;
        sDisplayProp.bDisplayPwrState = TRUE;


        //////////////////
        // Main Display //
        //////////////////

        // If the primary is supported initialize it
        if (TRUE == sInitParam.aSupportedDisplays[MDP_DISPLAY_PRIMARY])
        {
            if (MDP_STATUS_OK != MDPPower(MDP_DISPLAY_PRIMARY, &sPowerParams, 0x0))
            {
              DEBUG ((EFI_D_INFO, "DisplayDxe: Primary panel power up failed!\n"));    
            }
            else
            {
               MDP_OSAL_MEMZERO(&sDetectParams, sizeof(MDP_DetectParamType));
   
               // Default reporting of primary display
               if (MDP_STATUS_OK != MDPDetect(MDP_DISPLAY_PRIMARY, &sDetectParams, 0x0))
               {
                  eStatus = EFI_DEVICE_ERROR;
               }
               else if (TRUE == sDetectParams.bDisplayDetected)
               {
                  eStatus = DisplayDxeSelectMode(MDP_DISPLAY_PRIMARY, &sDetectParams);

                  // Set the primary display to on 
                  if (MDP_STATUS_OK != MDPSetProperty(MDP_DISPLAY_PRIMARY, MDP_DISPLAY_PROPERTY_POWER_STATE, &sDisplayProp))
                  {
                    eStatus = EFI_DEVICE_ERROR; 
                  }
               }
            }
        }    


        //////////////////////
        // External Display //
        //////////////////////
        // If the external is supported initialize it,
        if ((EFI_SUCCESS == eStatus) && 
            (TRUE == sInitParam.aSupportedDisplays[MDP_DISPLAY_EXTERNAL]))
            {
                if (MDP_STATUS_OK != MDPPower(MDP_DISPLAY_EXTERNAL, &sPowerParams, 0x0))
                {
                  DEBUG ((EFI_D_INFO, "DisplayDxe: External panel power up failed!\n"));    
                }              
                else 
                {
                   MDP_OSAL_MEMZERO(&sDetectParams, sizeof(MDP_DetectParamType));
                
                   // Default reporting of External display
                   if ((MDP_STATUS_OK != MDPDetect(MDP_DISPLAY_EXTERNAL, &sDetectParams, 0x0)) || 
                       (FALSE == sDetectParams.bDisplayDetected))
                   {
                      // External display not detected, turn off and continue on
                      sPowerParams.bPowerOn = FALSE;
                
                      if (MDP_STATUS_OK != MDPPower(MDP_DISPLAY_EXTERNAL, &sPowerParams, 0x0))
                      {
                        DEBUG ((EFI_D_INFO, "DisplayDxe: External panel power up failed!\n"));   
                      }
                   }
                   else
                   {
                      eStatus = DisplayDxeSelectMode(MDP_DISPLAY_EXTERNAL,&sDetectParams);
                
                      // Set the external display to on 
                      if (MDP_STATUS_OK != MDPSetProperty(MDP_DISPLAY_EXTERNAL, MDP_DISPLAY_PROPERTY_POWER_STATE, &sDisplayProp))
                      {
                        eStatus = EFI_DEVICE_ERROR; 
                      }
                   }
            }
        }

        // If we not have detected a valid mode on both primary and external display report error
        if ((0 == gModeInfo.uNumModes[MDP_DISPLAY_PRIMARY]) && (0 == gModeInfo.uNumModes[MDP_DISPLAY_EXTERNAL]))
        {
           eStatus = EFI_DEVICE_ERROR;
        }
        else
        {
           // Default at some dummy mode
           gModeInfo.sCurrentModeInfo.Version                       = GRAPHICS_OUTPUT_PROTOCOL_REVISION;
           gModeInfo.sCurrentModeInfo.PixelFormat                   = DISPLAYDXE_DEFAULT_PIXEL_FORMAT;
           gModeInfo.sCurrentModeInfo.HorizontalResolution          = 0;
           gModeInfo.sCurrentModeInfo.VerticalResolution            = 0;
           gModeInfo.sCurrentModeInfo.PixelInformation.RedMask      = DISPLAYDXE_RED_MASK;
           gModeInfo.sCurrentModeInfo.PixelInformation.GreenMask    = DISPLAYDXE_GREEN_MASK;
           gModeInfo.sCurrentModeInfo.PixelInformation.BlueMask     = DISPLAYDXE_BLUE_MASK;
           gModeInfo.sCurrentModeInfo.PixelInformation.ReservedMask = DISPLAYDXE_ALPHA_MASK;
           gModeInfo.sCurrentModeInfo.PixelsPerScanLine             = 0;
           
           // Setup the protocol information, set the current mode to an invalid mode forcing a set mode
           gModeInfo.sProtocolInfo.MaxMode    = 1;
           gModeInfo.sProtocolInfo.Mode       = (UINT32)-1;
           gModeInfo.sProtocolInfo.SizeOfInfo = sizeof(EFI_GRAPHICS_OUTPUT_MODE_INFORMATION);
           gModeInfo.sProtocolInfo.Info       = &gModeInfo.sCurrentModeInfo;

           // Install display protocols only if panel is detected.
           // Make a new handle with EFI Graphics Protocol
           if (EFI_SUCCESS != (eStatus = gBS->InstallMultipleProtocolInterfaces (&hUEFIDisplayHandle,
                                                                                 &gEfiDevicePathProtocolGuid,
                                                                                 &DisplayDevicePath,
                                                                                 &sOutputGUID,
                                                                                 &gDisplayDxeOutputProtocol,
                                                                                 &gQcomDisplayPwrCtrlProtocolGuid,
                                                                                 &gDisplayPwrCtrlProtocolImplementation,
                                                                                 &gEfiDisplayPowerStateProtocolGuid,
                                                                                 &gDisplayPwrProtocolImplementation,
                                                                                 &gQcomDisplayUtilsProtocolGuid,
                                                                                 &gQcomDisplayUtilsProtocolImplementation,
                                                                                 NULL)))
           {
              DEBUG ((EFI_D_INFO, "DisplayDxe: InstallMultipleProtocolInterfaces failed!\n"));
           }
        } 
  }

  return eStatus;
}



/**
  Display DXE Protocol Query function
  
  @param  This             The EFI_GRAPHICS_OUTPUT_PROTOCOL instance.
  @param  ModeNumber       The mode number to return information on.
  @param  SizeOfInfo       A pointer to the size, in bytes, of the Info buffer.
  @param  Info             A pointer to callee allocated buffer that returns information about ModeNumber.

  @retval EFI_SUCCESS               Mode returned successfully
  @retval EFI_DEVICE_ERROR          Hardware error querying the mode
  @retval EFI_INVALID_PARAMETER     Invalid parameters/null parameters

**/
EFI_STATUS
DisplayDxe_QueryMode (
  EFI_GRAPHICS_OUTPUT_PROTOCOL          *This,
  UINT32                                ModeNumber,
  UINTN                                 *SizeOfInfo,
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION  **Info
  )
{
    EFI_STATUS                            eStatus   = EFI_SUCCESS;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *pModeInfo = NULL;

    MDP_Display_IDType                    eDisplayID = MDP_DISPLAY_PRIMARY;

    
    if ((NULL == SizeOfInfo) || (NULL == Info) || (NULL == This))
    {
        eStatus = EFI_INVALID_PARAMETER;
    }
    // If primary monitor is not available, use the external monitor as the main display
    else if (0 == gModeInfo.uNumModes[MDP_DISPLAY_PRIMARY] )
    {
      eDisplayID = MDP_DISPLAY_EXTERNAL;
    }
    
    if (EFI_SUCCESS == eStatus)
    {
       if (ModeNumber >= gModeInfo.uNumModes[eDisplayID])
       {
           // Mode is out of range
           eStatus = EFI_INVALID_PARAMETER;
       }
       else if (NULL == (pModeInfo = AllocatePool(sizeof(EFI_GRAPHICS_OUTPUT_MODE_INFORMATION))))
       {
           // Allocation failed
           eStatus = EFI_INVALID_PARAMETER;
       }
       else
       {
          // Return results (single supported mode)
          MDP_OSAL_MEMCPY(pModeInfo, &gModeInfo.sModeList[eDisplayID][ModeNumber], sizeof(EFI_GRAPHICS_OUTPUT_MODE_INFORMATION));
       
          *Info       = pModeInfo;
          *SizeOfInfo = sizeof(EFI_GRAPHICS_OUTPUT_MODE_INFORMATION);
       }
    }
   
    return eStatus;
}



/**
  Display DXE Protocol set mode
  
  @param  This                      The EFI_GRAPHICS_OUTPUT_PROTOCOL instance.
  @param  ModeNumber                Abstraction that defines the current video mode.

  @retval EFI_SUCCESS               The graphics mode specified by ModeNumber was selected.
  @retval EFI_DEVICE_ERROR          The device had an error and could not complete the request.
  @retval EFI_UNSUPPORTED           ModeNumber is not supported by this device.
  @retval EFI_OUT_OF_RESOURCES      Memory allocation failed
  @retval EFI_INVALID_PARAMETER     Invalid protocol handle passed in
**/
EFI_STATUS
DisplayDxe_SetMode (
  EFI_GRAPHICS_OUTPUT_PROTOCOL *This,
  UINT32                       ModeNumber
  )
{
    EFI_STATUS          eStatus    = EFI_SUCCESS;
    MemRegionInfo       sMemRegionInfo;

    if (NULL == This)
    {
        eStatus = EFI_INVALID_PARAMETER;
    }
    else if (0 != ModeNumber)
    {  
       eStatus = RETURN_UNSUPPORTED;
    }
    else if (EFI_SUCCESS != (eStatus = GetMemRegionInfoByName("Display Reserved", &sMemRegionInfo)))
    {
        // Failed to get memory region info
        eStatus = EFI_DEVICE_ERROR;
    }
    else if ((TRUE == gModeInfo.bModeSet) && (ModeNumber == gModeInfo.uModeId))
    {
        // Already set, just continue
    }
    else 
    {
       EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *pMode            = NULL;
       UINTN                                 uFrameBufferSize = 0;
       UINTN                                 uMemBase;
       VOID                                 *pFrameBufferBase;
       MDP_SetModeParamType                  sModeParams;

       //////////////////////////
       // Allocate frame buffer//
       /////////////////////////
       /****************************************************************************************/
       uMemBase = (UINTN)sMemRegionInfo.MemBase;
       pFrameBufferBase = (VOID *)uMemBase;
       gModeInfo.eDisplayFBAttrib = sMemRegionInfo.CacheAttributes;

      // 
      // If primary display exists use the primary display resolution for the frame buffer allocation
      // If only external display exists use the selected mode resolution for the frame buffer allocation
      //
      if (0 != gModeInfo.uNumModes[MDP_DISPLAY_PRIMARY])
      {
         pMode = &gModeInfo.sModeList[MDP_DISPLAY_PRIMARY][ModeNumber];
      }
      else
      {
         pMode = &gModeInfo.sModeList[MDP_DISPLAY_EXTERNAL][ModeNumber];
      }

      // Clear the frame buffer
      uFrameBufferSize = pMode->HorizontalResolution * pMode->VerticalResolution * DISPLAYDXE_DEFAULT_BYTES_PER_PIXEL;
      MDP_OSAL_MEMZERO(pFrameBufferBase, uFrameBufferSize);

      /* write through frame buffer */
      FlushStaleLines(pFrameBufferBase, uFrameBufferSize);

      // Setup the mode params
      MDP_OSAL_MEMZERO(&sModeParams, sizeof(MDP_SetModeParamType));
      sModeParams.sSurfaceInfo.ePixelFormat  = DISPLAYDXE_DEFAULT_MDP_FORMAT;
      sModeParams.sSurfaceInfo.uWidth        = pMode->HorizontalResolution;
      sModeParams.sSurfaceInfo.uHeight       = pMode->VerticalResolution;
      sModeParams.sSurfaceInfo.pPlane0Offset = pFrameBufferBase;
      sModeParams.sSurfaceInfo.uPlane0Stride = pMode->HorizontalResolution * DISPLAYDXE_DEFAULT_BYTES_PER_PIXEL;

       /****************************************************************************************/

       //////////////////
       // Main Display //
       //////////////////      
       if (0 != gModeInfo.uNumModes[MDP_DISPLAY_PRIMARY])
       {
           
         //Set Mode index to selected Mode
         sModeParams.uModeIndex                 = gModeInfo.uSelectedModeIndex[MDP_DISPLAY_PRIMARY];

         if (MDP_STATUS_OK == MDPSetMode(MDP_DISPLAY_PRIMARY, &sModeParams, 0x0))
         {
            MDP_PropertiesParamType  sDisplayProp;
         
            //
            // Setup the default backlight level
            //
            MDP_OSAL_MEMZERO(&sDisplayProp, sizeof(MDP_PropertiesParamType));
            sDisplayProp.uBacklightLevel = PcdGet32(PcdBacklightLevel);

            MDPSetProperty(MDP_DISPLAY_PRIMARY, MDP_DISPLAY_PROPERTY_BACKLIGHT, &sDisplayProp);
         }
         else
         {
            DEBUG ((EFI_D_INFO, "DisplayDxe: MDPSetMode() failed!\n"));
            eStatus = EFI_DEVICE_ERROR;
         }
      }

      /****************************************************************************************/

      //////////////////////
      // External Display //
      //////////////////////
      if (0 != gModeInfo.uNumModes[MDP_DISPLAY_EXTERNAL])
      {

         //Set Mode index to selected Mode
         sModeParams.uModeIndex                 = gModeInfo.uSelectedModeIndex[MDP_DISPLAY_EXTERNAL];

        // Setup the mode
         if (MDP_STATUS_OK != MDPSetMode(MDP_DISPLAY_EXTERNAL, &sModeParams, 0x0))
         {
            DEBUG ((EFI_D_INFO, "DisplayDxe: MDPSetMode() failed!\n"));
            eStatus = EFI_DEVICE_ERROR;
         }
      }

      if(EFI_SUCCESS == eStatus)
      {
        //
        // Populate the display environment information, used to inform the OS driver of firmware state.
        //
        MDP_Display_IDType      eDisplayID;

        if (gModeInfo.uNumModes[MDP_DISPLAY_PRIMARY] > 0)
        {
          //If Primary display is supported, save the firmware environment for primary 
          eDisplayID = MDP_DISPLAY_PRIMARY;
        }
        else 
        {
          eDisplayID = MDP_DISPLAY_EXTERNAL;
        }

        MDPSetProperty(eDisplayID, MDP_DISPLAY_PROPERTY_FIRMWAREENV, NULL);
      }

      /****************************************************************************************/

      // Save the current mode
      if ((EFI_SUCCESS == eStatus) && 
          (NULL != pMode))
      {
         MDP_OSAL_MEMCPY(&gModeInfo.sCurrentModeInfo, pMode, sizeof(EFI_GRAPHICS_OUTPUT_MODE_INFORMATION));
         gModeInfo.uModeId = ModeNumber;
         
         //
         // Set the protocol information
         //
         gModeInfo.sProtocolInfo.FrameBufferBase = (EFI_PHYSICAL_ADDRESS)pFrameBufferBase;
         gModeInfo.sProtocolInfo.FrameBufferSize = uFrameBufferSize;
         gModeInfo.sProtocolInfo.Mode            = ModeNumber;
         gModeInfo.sProtocolInfo.Info            = &gModeInfo.sCurrentModeInfo;
         gModeInfo.bModeSet                      = TRUE;
      }
    }

    return eStatus;
}


/**
  Display DXE Protocol Blt function
  
  @param  This         Protocol instance pointer.
  @param  BltBuffer    Buffer containing data to blit into video buffer. This
                       buffer has a size of Width*Height*sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
  @param  BltOperation Operation to perform on BlitBuffer and video memory
  @param  SourceX      X coordinate of source for the BltBuffer.
  @param  SourceY      Y coordinate of source for the BltBuffer.
  @param  DestinationX X coordinate of destination for the BltBuffer.
  @param  DestinationY Y coordinate of destination for the BltBuffer.
  @param  Width        Width of rectangle in BltBuffer in pixels.
  @param  Height       Hight of rectangle in BltBuffer in pixels.
  @param  Delta        OPTIONAL

  @retval EFI_SUCCESS           The Blt operation completed.
  @retval EFI_INVALID_PARAMETER BltOperation is not valid or source/destination parameters are invalid
  @retval EFI_DEVICE_ERROR      A hardware error occured writting to the video buffer.


**/
EFI_STATUS
DisplayDxe_Blt (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL            *This,
  IN  EFI_GRAPHICS_OUTPUT_BLT_PIXEL           *BltBuffer,
  IN  EFI_GRAPHICS_OUTPUT_BLT_OPERATION       BltOperation,
  IN  UINTN                                   SourceX,
  IN  UINTN                                   SourceY,
  IN  UINTN                                   DestinationX,
  IN  UINTN                                   DestinationY,
  IN  UINTN                                   Width,
  IN  UINTN                                   Height,
  IN  UINTN                                   Delta)
{
    EFI_STATUS        eStatus = EFI_SUCCESS;

    if (NULL == This)
    {
        eStatus = EFI_INVALID_PARAMETER;
    }
    else if (FALSE == gModeInfo.bModeSet)
    {
         eStatus = EFI_NOT_STARTED;
    }
    else
    {
      switch (BltOperation)
      {
      case EfiBltBufferToVideo:
        {
          UINT8 *pSrcBuffer = (UINT8*)BltBuffer;
          UINT8 *pDstBuffer = (UINT8*)DISPLAYDXE_PHYSICALADDRESS32(gModeInfo.sProtocolInfo.FrameBufferBase);
          UINTN SrcStride, DstStride, CopyWidth, CopyHeight;

          if (((DestinationX + Width) > gModeInfo.sCurrentModeInfo.HorizontalResolution) ||
              ((DestinationY + Height) > gModeInfo.sCurrentModeInfo.VerticalResolution))
          {
             return EFI_INVALID_PARAMETER;
          }

          CopyWidth  = Width;
          CopyHeight = Height;

          /* Video buffer stride in bytes, consider padding as well */
          DstStride = gModeInfo.sCurrentModeInfo.PixelsPerScanLine * DISPLAYDXE_DEFAULT_BYTES_PER_PIXEL;

          /* Src buffer stride in bytes. Delta is valid when X or Y is not 0 */
          SrcStride = Width * DISPLAYDXE_DEFAULT_BYTES_PER_PIXEL;
          if (Delta != 0)
          {  
            SrcStride = Delta;
          }

          DisplayDxeBltInternal (pSrcBuffer,
                                 pDstBuffer,
                                 SourceX, 
                                 SourceY, 
                                 CopyWidth, 
                                 CopyHeight,
                                 SrcStride,
                                 DestinationX,
                                 DestinationY,
                                 DstStride,
                                 DISPLAYDXE_DEFAULT_BYTES_PER_PIXEL);
          
          /* If display frame buffer is cached, need to call cache maintenance function */
          FlushStaleLines(((UINT8*)pDstBuffer) + (DstStride * DestinationY), (DstStride * CopyHeight));

          eStatus = EFI_SUCCESS;
        }
        break;

      case EfiBltVideoToBltBuffer:
        {  
          UINT8 *pDstBuffer = (UINT8*)BltBuffer;
          UINT8 *pSrcBuffer = (UINT8*)DISPLAYDXE_PHYSICALADDRESS32(gModeInfo.sProtocolInfo.FrameBufferBase);
          UINTN SrcStride, DstStride, CopyWidth, CopyHeight;

          if (((SourceX + Width) > gModeInfo.sCurrentModeInfo.HorizontalResolution) ||
              ((SourceY + Height) > gModeInfo.sCurrentModeInfo.VerticalResolution))
          {
            return EFI_INVALID_PARAMETER;
          }

          CopyWidth  = Width;
          CopyHeight = Height;

          /* Video buffer stride in bytes, consider padding as well */
          SrcStride = gModeInfo.sCurrentModeInfo.PixelsPerScanLine * DISPLAYDXE_DEFAULT_BYTES_PER_PIXEL;

          /* Buffer stride in bytes. Delta is valid when X or Y is not 0 */
          DstStride = Width * DISPLAYDXE_DEFAULT_BYTES_PER_PIXEL;
          if (Delta != 0)
          {
            DstStride = Delta;
          }

          DisplayDxeBltInternal (pSrcBuffer,
                                 pDstBuffer,
                                 SourceX, 
                                 SourceY, 
                                 CopyWidth, 
                                 CopyHeight,
                                 SrcStride,
                                 DestinationX,
                                 DestinationY,
                                 DstStride,
                                 DISPLAYDXE_DEFAULT_BYTES_PER_PIXEL);

          eStatus = EFI_SUCCESS;
        }
        break; 

      case EfiBltVideoFill:
        {
          UINT32 SrcPixel = *(UINT32*)BltBuffer;
          UINT32 *pDstBuffer = (UINT32*)DISPLAYDXE_PHYSICALADDRESS32(gModeInfo.sProtocolInfo.FrameBufferBase);
          UINTN  DstStride, CopyWidth, CopyHeight;
          UINT32 x,y;

          if (((DestinationX + Width) > gModeInfo.sCurrentModeInfo.HorizontalResolution) ||
              ((DestinationY + Height) > gModeInfo.sCurrentModeInfo.VerticalResolution))
          {
            return EFI_INVALID_PARAMETER;
          }

          CopyWidth  = Width;
          CopyHeight = Height;

          /* Video buffer stride in bytes, consider padding as well */
          DstStride = gModeInfo.sCurrentModeInfo.PixelsPerScanLine * DISPLAYDXE_DEFAULT_BYTES_PER_PIXEL;

          /* Adjust Destination location */
          pDstBuffer = (UINT32*)(((UINT8*)pDstBuffer) + (DestinationY * DstStride) + DestinationX * DISPLAYDXE_DEFAULT_BYTES_PER_PIXEL); 

          /* Do the actual blitting */
          for (y = 0; y < CopyHeight; y++)
          {
              for (x = 0; x < CopyWidth; x++)
              {
                  pDstBuffer[x] = SrcPixel;
              }
              /* Increment by stride number of bytes */
              pDstBuffer = (UINT32*)((UINT8*)pDstBuffer + DstStride);
          }

          /* If display frame buffer is cached, need to call cache maintenance function */
          pDstBuffer = (UINT32*)DISPLAYDXE_PHYSICALADDRESS32(gModeInfo.sProtocolInfo.FrameBufferBase);
          FlushStaleLines(((UINT8*)pDstBuffer) + (DstStride * DestinationY), (DstStride * CopyHeight));

          eStatus = EFI_SUCCESS;
         }
         break;

       case EfiBltVideoToVideo:
         {
           UINT8 *pSrcBuffer = (UINT8*)DISPLAYDXE_PHYSICALADDRESS32(gModeInfo.sProtocolInfo.FrameBufferBase);
           UINT8 *pDstBuffer = (UINT8*)DISPLAYDXE_PHYSICALADDRESS32(gModeInfo.sProtocolInfo.FrameBufferBase);
           UINTN  Stride, CopyWidth, CopyHeight;

           if (((SourceX + Width) > gModeInfo.sCurrentModeInfo.HorizontalResolution))
             Width = gModeInfo.sCurrentModeInfo.HorizontalResolution - SourceX;

           if (((SourceY + Height) > gModeInfo.sCurrentModeInfo.VerticalResolution))
             Height = gModeInfo.sCurrentModeInfo.VerticalResolution - SourceY;

           if (((DestinationX + Width) > gModeInfo.sCurrentModeInfo.HorizontalResolution))
             Width = gModeInfo.sCurrentModeInfo.HorizontalResolution - DestinationX;

           if (((DestinationY + Height) > gModeInfo.sCurrentModeInfo.VerticalResolution))
             Height = gModeInfo.sCurrentModeInfo.VerticalResolution - DestinationY;

           CopyWidth  = Width;
           CopyHeight = Height;

           /* Video buffer stride in bytes, consider padding as well */
           Stride = gModeInfo.sCurrentModeInfo.PixelsPerScanLine * DISPLAYDXE_DEFAULT_BYTES_PER_PIXEL;

           DisplayDxeBltInternal (pSrcBuffer,
                                  pDstBuffer,
                                  SourceX, 
                                  SourceY, 
                                  CopyWidth, 
                                  CopyHeight,
                                  Stride,
                                  DestinationX,
                                  DestinationY,
                                  Stride,
                                  DISPLAYDXE_DEFAULT_BYTES_PER_PIXEL);

           /* If display frame buffer is cached, need to call cache maintenance function */
           FlushStaleLines(((UINT8*)pDstBuffer) + (Stride * DestinationY), (Stride * CopyHeight));
           eStatus = EFI_SUCCESS;

         }
         break;

       default:
          DEBUG ((EFI_D_INFO, "DisplayDxe: BltOperation not supported\n"));
          eStatus = RETURN_INVALID_PARAMETER;
          break;
      }    
    }


    return eStatus;    
}


/**********************************************************************************************************************
     Local Functions
**/




/**
  DisplayDxeSelectMode
  
  Display DXE Local function to select the mode out of supported modes
  
  @param  eDisplayId            Display ID
  @param  pDisplayModes         List of Modes 
  
@retval EFI_SUCCESS       Execution successful
@retval other             Error occurred

**/


static EFI_STATUS DisplayDxeSelectMode(MDP_Display_IDType eDisplayId, MDP_DetectParamType  *pDisplayModes)
{
   EFI_STATUS     Status = EFI_SUCCESS;

   if (NULL == pDisplayModes)
   {
      Status = EFI_INVALID_PARAMETER;
   }
   else if (0 == pDisplayModes->uSupportedModes )
   {
      Status = EFI_NOT_FOUND;
   }
   else
   {
      // Handle each display
      switch (eDisplayId)
      {
         //For Primary only 1 mode is supported, 
        case MDP_DISPLAY_PRIMARY:
        {
            gModeInfo.uSelectedModeIndex[eDisplayId] = 0 ;
        }
        break;

        // Select the largest mode out of the list of supported modes available on the external monitor
        case MDP_DISPLAY_EXTERNAL:
        {
           MDP_ModeInfo *pMaxMode = &pDisplayModes->aModeList[0];
           UINT32 uLargestModeIndex = 0;
           UINT32 uI;
           uint32 uExtModePCD   = PcdGet32(PcdSetExtMode);          

           if (0 == uExtModePCD)
           {
             // Pick mode 0
             gModeInfo.uSelectedModeIndex[eDisplayId] = 0;
           }
           else if (1 == uExtModePCD)
           {
             //Auto Select the largest mode
             // We loop through the list to find the largest mode 
             for (uI = 1; uI < pDisplayModes->uSupportedModes; uI++)
             {
               // Largest Mode
               if ((pMaxMode->uWidth < pDisplayModes->aModeList[uI].uWidth) &&
                   (pMaxMode->uHeight < pDisplayModes->aModeList[uI].uHeight))
               {
                 pMaxMode = &pDisplayModes->aModeList[uI];
                 uLargestModeIndex = uI;
               }
             }
             gModeInfo.uSelectedModeIndex[eDisplayId] = uLargestModeIndex;
           }
           else           
           {
             uint32 uWidth  = uExtModePCD & 0xFFFF;
             uint32 uHeight = (uExtModePCD>>16) & 0xFFFF;

             gModeInfo.uSelectedModeIndex[eDisplayId] = MDP_DISPLAY_MAX_MODES;
             
             // We loop through the list to find the match 
             for (uI = 0; uI < pDisplayModes->uSupportedModes; uI++)
             {
               if ((uWidth  ==  pDisplayModes->aModeList[uI].uWidth) &&
                   (uHeight ==  pDisplayModes->aModeList[uI].uHeight))
               {
                 gModeInfo.uSelectedModeIndex[eDisplayId] = uI;
                 break;
               }
             }
             if (MDP_DISPLAY_MAX_MODES == gModeInfo.uSelectedModeIndex[eDisplayId])
             {
               gModeInfo.uSelectedModeIndex[eDisplayId] = 0;
               DEBUG((EFI_D_INFO, "DisplayDxe: Failed to override the External panel mode!\n"));
             }

           }                      
        }
        break;

        default:
            Status = EFI_INVALID_PARAMETER;
        break;
      }

      if (EFI_SUCCESS == Status)
      {
         MDP_PropertiesParamType               sDisplayProp;      
         UINT32                                uResLimitIndex     = (PcdGet32(PcdFrameBufMaxRes) < DISPLAY_FB_RES_MAX)?PcdGet32(PcdFrameBufMaxRes):0;
         EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *pMode              = &gModeInfo.sModeList[eDisplayId][0];
         UINT32                                uSelectedModeIndex = gModeInfo.uSelectedModeIndex[eDisplayId];
        
         // Set the Selected Mode index 
         //(MDPlib checks if it needs dual pipe and for external monitor also sets the active timing information for this mode 
         MDP_OSAL_MEMZERO(&sDisplayProp, sizeof(MDP_PropertiesParamType));
         sDisplayProp.sModeParams.uModeIndex = uSelectedModeIndex;
         MDPSetProperty(eDisplayId, MDP_DISPLAY_PROPERTY_MODE_INFO, &sDisplayProp);
      
         // Update global Mode Information
         pMode->Version                              = GRAPHICS_OUTPUT_PROTOCOL_REVISION;
         pMode->PixelFormat                          = DISPLAYDXE_DEFAULT_PIXEL_FORMAT;
         pMode->HorizontalResolution                 = pDisplayModes->aModeList[uSelectedModeIndex].uWidth;
         pMode->VerticalResolution                   = pDisplayModes->aModeList[uSelectedModeIndex].uHeight;
    
         // Limit the frame buffer allocation as specified by PcdFrameBufMaxRes, frame buffer will be letterboxed in to the larger resolution panel
         if ((pDisplayModes->aModeList[uSelectedModeIndex].uWidth * pDisplayModes->aModeList[uSelectedModeIndex].uHeight) >
             (gFBResLimit[uResLimitIndex].uWidth * gFBResLimit[uResLimitIndex].uHeight))
         {
            // Portrait mode panel 
            if (pDisplayModes->aModeList[uSelectedModeIndex].uHeight > pDisplayModes->aModeList[uSelectedModeIndex].uWidth)
            {
              // Clamp the width to Max height and height to Max Width to maintain the portrait aspect ratio 
              pMode->HorizontalResolution  = gFBResLimit[uResLimitIndex].uHeight;  
              pMode->VerticalResolution    = gFBResLimit[uResLimitIndex].uWidth;
            }
            // Landscape mode panel
            else
            {
              pMode->HorizontalResolution = gFBResLimit[uResLimitIndex].uWidth;
              pMode->VerticalResolution   = gFBResLimit[uResLimitIndex].uHeight;
            }
            
            DEBUG ((EFI_D_INFO, "DisplayDxe: Frame buffer allocation limited to %dix%d\n", pMode->HorizontalResolution, pMode->VerticalResolution));            
         }
          
         pMode->PixelInformation.RedMask             = DISPLAYDXE_RED_MASK;
         pMode->PixelInformation.GreenMask           = DISPLAYDXE_GREEN_MASK;
         pMode->PixelInformation.BlueMask            = DISPLAYDXE_BLUE_MASK;
         pMode->PixelInformation.ReservedMask        = DISPLAYDXE_ALPHA_MASK;
         pMode->PixelsPerScanLine                    = pMode->HorizontalResolution;
         gModeInfo.uNumModes[eDisplayId]++;
      }
   }

   return Status;
}


/**
  Display DXE Internal Blit function
  
  @param  pSrc              Pointer (non NULL) to source buffer.
  @param  pdst              Pointer (non NULL) to destination buffer.                        

  @param  uSrcX             X coordinate of source.
  @param  uSrcY             Y coordinate of source.
  @param  uSrcWidth         Width of source rectangle in pixels.
  @param  uSrcHeight        Height of source rectangle in pixels.
  @param  uSrcStride        Stride of source rectangle in bytes.
  
  @param  uDstX             X coordinate of destination.
  @param  uDstY             Y coordinate of destination.
  @param  uDstStride        Stride of destination rectangle in bytes.

  @param  uBytesPerPixel    Number of bytes per pixel. 
**/

static void DisplayDxeBltInternal (
  UINT8   *pSrc,
  UINT8   *pDst,
  UINTN    uSrcX,
  UINTN    uSrcY,
  UINTN    uSrcWidth,
  UINTN    uSrcHeight,
  UINTN    uSrcStride,
  UINTN    uDstX,
  UINTN    uDstY,
  UINTN    uDstStride,
  UINTN    uBytesPerPixel
  )
{
  UINT32 uI = 0;
  UINT32 uSrcWidthBytes = uSrcWidth * uBytesPerPixel;  
  
  /* move src pointer to start of src rectangle */
  pSrc += (uSrcY * uSrcStride) + (uSrcX * uBytesPerPixel);

  /* move dest pointer to start of dest rectangle */
  pDst += (uDstY * uDstStride) + (uDstX * uBytesPerPixel); 

  /* Blit Operation 
   *
   *  We use MDP_OSAL_MEMCPY which invokes Neon memcpy (kr_memcpy.asm) 
   *  This memcpy supports overlapped src and dst buffers but copying may not be optimal in the overlap case 
   */  
  for (uI = 0; uI < uSrcHeight; ++uI)
  {
    MDP_OSAL_MEMCPY((void*)pDst, (void*)pSrc, uSrcWidthBytes);

    pDst += uDstStride;
    pSrc += uSrcStride;
  }
}

/**
FlushStaleLines

Check if Frame buffer is cached, and perform cache maintenance.

@param  pAddress            Start of frame buffer address
@param  uLength             Length of frame buffer need to write back from cache

@retval None

**/
static void FlushStaleLines(VOID *pAddress, uint32 uLength)
{
  if ((ARM_MEMORY_REGION_ATTRIBUTE_WRITE_BACK_XN == gModeInfo.eDisplayFBAttrib) ||
      (ARM_MEMORY_REGION_ATTRIBUTE_WRITE_BACK    == gModeInfo.eDisplayFBAttrib))
  {
    WriteBackDataCacheRange(pAddress, uLength);
  }
}

