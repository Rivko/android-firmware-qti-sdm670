/*=============================================================================
 
  File: DisplayPwrCtrlProtocol.c
 
  UEFI display power control protocol 
  
 
    Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
    All Rights Reserved
    Qualcomm Confidential and Proprietary
=============================================================================*/

#include <PiDxe.h>
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/QcomBaseLib.h>
#include <Library/QcomLib.h>
#include <Protocol/EFIPlatformInfo.h>
#include "Protocol/EFIDisplayPwr.h"
#include "MDPLib.h"
#include "MDPPlatformLib.h"
#include "DisplayDxe.h"
#include "MDPSystem.h"



/**********************************************************************************************************************
    Static/Globals
**/

static 
EFI_STATUS 
DisplayPanelPwrCtrlOp
(
  IN  MDP_Display_IDType   eDisplayId,
  IN  BOOLEAN              bDisplayState
);

static 
EFI_STATUS 
DisplayPanelSetMode
(
  IN  MDP_Display_IDType   eDisplayId
);


/**********************************************************************************************************************
    Public APIs
**/


/** 
  Modifies the power state of the display and backlight.

  @param[in]  This         : Pointer to the EFI_DISPLAY_POWER_PROTOCOL interface                     
  @param[in] PowerState    : EfiDisplayPowerStateMaximum (ON) 
                           : EfiDisplayPowerStateOff (OFF)
  
  @return
  EFI_SUCCESS              : Function returned successfully.
  EFI_INVALID_PARAMETER    : Parameter passed is incorrect.  
  EFI_DEVICE_ERROR         : Physical device reported an error.
*/
EFI_STATUS
EFIAPI 
EFI_DisplayPowerSetDisplayPowerState
(
  IN EFI_DISPLAY_POWER_PROTOCOL       *This,
  IN EFI_DISPLAY_POWER_STATE           PowerState
)
{
  EFI_STATUS   eStatus   = EFI_SUCCESS;

  /* Power state is supported */
  if ((PowerState != EfiDisplayPowerStateMaximum) && (PowerState != EfiDisplayPowerStateOff))
  {
    eStatus = EFI_INVALID_PARAMETER; 
  } 
  else if (MDP_STATUS_OK != MDPAcquireLockOrFail(MDP_LOCK_TYPE_DISPLAY_PWR_CTRL))
  {
    /* Fail this call as an instance of the function is currently running */
    DEBUG((EFI_D_ERROR, "DisplayPowerSetDisplayPowerState: Failed as concurrency is not supported!\n"));
    eStatus = EFI_UNSUPPORTED;
  }
  else 
  {
    BOOLEAN                  bDisplayState;
    BOOLEAN                  bPrimaryPanelDetected;
    BOOLEAN                  bExternalPanelDetected;
    BOOLEAN                  bPrimaryPanelPwrState;
    BOOLEAN                  bExternalPanelPwrState;    
    MDP_PropertiesParamType  sDisplayProp;    
       
    /* Assign the display state */
    bDisplayState = (EfiDisplayPowerStateMaximum == PowerState) ? TRUE : FALSE;

    /* Get the primary panel detection info */
    MDP_OSAL_MEMZERO(&sDisplayProp, sizeof(MDP_PropertiesParamType));
    MDPGetProperty(MDP_DISPLAY_PRIMARY, MDP_DISPLAY_PROPERTY_DETECTION_INFO, &sDisplayProp);
    bPrimaryPanelDetected = sDisplayProp.bDisplayDetected;
    
    /* Get the external panel detection info */
    MDP_OSAL_MEMZERO(&sDisplayProp, sizeof(MDP_PropertiesParamType));
    MDPGetProperty(MDP_DISPLAY_EXTERNAL, MDP_DISPLAY_PROPERTY_DETECTION_INFO, &sDisplayProp);
    bExternalPanelDetected = sDisplayProp.bDisplayDetected;
    
    /* Get the primary panel power state info */
    MDP_OSAL_MEMZERO(&sDisplayProp, sizeof(MDP_PropertiesParamType));
    MDPGetProperty(MDP_DISPLAY_PRIMARY, MDP_DISPLAY_PROPERTY_POWER_STATE, &sDisplayProp);
    bPrimaryPanelPwrState = sDisplayProp.bDisplayPwrState;
    
    /* Get the external panel power state info */
    MDP_OSAL_MEMZERO(&sDisplayProp, sizeof(MDP_PropertiesParamType));
    MDPGetProperty(MDP_DISPLAY_EXTERNAL, MDP_DISPLAY_PROPERTY_POWER_STATE, &sDisplayProp);
    bExternalPanelPwrState = sDisplayProp.bDisplayPwrState;

    /* Primary panel detected - turn on/off the primary panel */      
    if (TRUE == bPrimaryPanelDetected)
    {
      /* Display state has changed */
      if (bDisplayState != bPrimaryPanelPwrState)
      {
        eStatus = DisplayPanelPwrCtrlOp(MDP_DISPLAY_PRIMARY, bDisplayState);
      }
    }      

    /* External panel detected - turn on/off the external panel */
    if ((EFI_SUCCESS == eStatus) && (TRUE == bExternalPanelDetected))
    {
      /* Display state has changed */
      if (bDisplayState != bExternalPanelPwrState)
      {
        eStatus = DisplayPanelPwrCtrlOp(MDP_DISPLAY_EXTERNAL, bDisplayState);        
      }
    }    

    /* Make sure that all the displays are powered down inorder to de-initialize the MDP core and free all the resources */
    if ((EFI_SUCCESS == eStatus) && (EfiDisplayPowerStateOff == PowerState))
    {
      if (MDP_STATUS_OK != MDPTerm(0x0))
      {  
        eStatus = EFI_DEVICE_ERROR; 
      }
    }
   
    MDPReleaseLock(MDP_LOCK_TYPE_DISPLAY_PWR_CTRL);    
 }

  return eStatus;
}


/**
  Retrieves the current power state of the display and backlight.

  @param[in]  This         : Pointer to the EFI_DISPLAY_POWER_PROTOCOL interface      
  @param[out] PowerState   : EfiDisplayPowerStateMaximum (ON)  
                           : EfiDisplayPowerStateOff (OFF)
  
  @return
  EFI_SUCCESS              : Function returned successfully.
  EFI_INVALID_PARAMETER    : Parameter passed is incorrect.  
  EFI_DEVICE_ERROR         : Physical device reported an error.
*/
EFI_STATUS
EFIAPI 
EFI_DisplayPowerGetDisplayPowerState
(
  IN EFI_DISPLAY_POWER_PROTOCOL       *This,
  OUT EFI_DISPLAY_POWER_STATE         *PowerState
)
{
  EFI_STATUS               eStatus  = EFI_SUCCESS; 
  MDP_PropertiesParamType  sDisplayProp;
         
  MDP_OSAL_MEMZERO(&sDisplayProp, sizeof(MDP_PropertiesParamType));

  /* Get the power state for the primary display */
  if (MDP_STATUS_OK != MDPGetProperty(MDP_DISPLAY_PRIMARY, MDP_DISPLAY_PROPERTY_POWER_STATE, &sDisplayProp))
  {
    eStatus = EFI_DEVICE_ERROR; 
  }
  else 
  {
    *PowerState = (TRUE == sDisplayProp.bDisplayPwrState) ? EfiDisplayPowerStateMaximum : EfiDisplayPowerStateOff;
  }
  
  return eStatus;
}


/**
  Sets the display panel power state 

  @param[IN] DisplayType   : PRIMARY/EXTERNAL
  @param[IN] DisplayState  : TRUE/FALSE

  @return
  EFI_SUCCESS              : Function returned successfully.
  EFI_DEVICE_ERROR         : Physical device reported an error.

*/
static 
EFI_STATUS 
DisplayPanelPwrCtrlOp
(
  IN  MDP_Display_IDType   eDisplayId,
  IN  BOOLEAN              bDisplayState
)
{
  EFI_STATUS               eStatus  = EFI_SUCCESS;
  MDP_PowerParamsType      sPowerParams; 
  MDP_PropertiesParamType  sDisplayProp;            
          
  MDP_OSAL_MEMZERO(&sPowerParams, sizeof(MDP_PowerParamsType));        
  MDP_OSAL_MEMZERO(&sDisplayProp, sizeof(MDP_PropertiesParamType));    

  /* Assign the display power state */
  sPowerParams.bPowerOn = sDisplayProp.bDisplayPwrState = bDisplayState;

  /* Display power state is set to on */
  if (TRUE == sPowerParams.bPowerOn)
  {
    MDP_DetectParamType  sDetectParams; 
    MDP_InitParamsType   sInitParam;
    
    MDP_OSAL_MEMZERO(&sDetectParams, sizeof(MDP_DetectParamType));
    MDP_OSAL_MEMZERO(&sInitParam, sizeof(MDP_InitParamsType));

    /* MDSS configuration needs to be restored as it can lose its
       configuration if there is power collapse.*/
    if (MDP_STATUS_OK != MDPInit(&sInitParam, MDP_INIT_FLAG_MINIMAL_INIT))
    {
      eStatus = EFI_DEVICE_ERROR; 
    }
    /* Request the MDP hardware to power on the display */
    else if (MDP_STATUS_OK != MDPPower(eDisplayId, &sPowerParams, 0x0))
    {
      eStatus = EFI_DEVICE_ERROR; 
    }
    /* Detect the presence of a display */
    else if (MDP_STATUS_OK != MDPDetect(eDisplayId, &sDetectParams, 0x0))
    {
      eStatus = EFI_DEVICE_ERROR;
    }
    /* Display is not detected */
    else if (FALSE == sDetectParams.bDisplayDetected)
    {
      MDP_OSAL_MEMZERO(&sPowerParams, sizeof(MDP_PowerParamsType));          

      /* Power off the display that was turned on */
      sPowerParams.bPowerOn = FALSE;       

      /* Request the MDP hardware to power off the display */
      MDPPower(eDisplayId, &sPowerParams, 0x0);

      /* Indicate that the operation was not successful */
      eStatus = EFI_DEVICE_ERROR;
    }
    else
    { 
      /* Set the display panel power state */              
      if (MDP_STATUS_OK != MDPSetProperty(eDisplayId, MDP_DISPLAY_PROPERTY_POWER_STATE, &sDisplayProp))
      {
        eStatus = EFI_DEVICE_ERROR; 
      } 
      else 
      {
        /* Set the display mode that was cached */
        eStatus = DisplayPanelSetMode(eDisplayId);
      }
    }
  }
  /* Display power state is set to OFF */
  else
  {
    /* De-initialize the interfaces */
    if(MDP_STATUS_OK != MDPDeInit(eDisplayId, 0x0))
    {
      eStatus = EFI_DEVICE_ERROR; 
    }   
    /* Request the MDP hardware to power off the display */
    else if (MDP_STATUS_OK != MDPPower(eDisplayId, &sPowerParams, 0x0))
    {
      eStatus = EFI_DEVICE_ERROR; 
    }
    else 
    {
      /* Set the display panel power state */              
      if (MDP_STATUS_OK != MDPSetProperty(eDisplayId, MDP_DISPLAY_PROPERTY_POWER_STATE, &sDisplayProp))
      {
        eStatus = EFI_DEVICE_ERROR; 
      } 
    }
  }  

  return eStatus;
}


/**
  Sets the panel mode
  
  @param[IN] DisplayType     : PRIMARY/EXTERNAL

  @return
  EFI_SUCCESS                : Graphics mode specified was selected.
  EFI_DEVICE_ERROR           : Physical device reported an error.
  EFI_UNSUPPORTED            : ModeNumber is not supported by this device.
  EFI_INVALID_PARAMETER      : Parameter passed is incorrect.  
  EFI_OUT_OF_RESOURCES       : Memory allocation failed  
**/
static 
EFI_STATUS 
DisplayPanelSetMode
(
  IN  MDP_Display_IDType   eDisplayId
)
{
  MDP_SetModeParamType     sModeParams;
  MDP_PropertiesParamType  sDisplayProp;
  EFI_STATUS               eStatus    = EFI_SUCCESS;  

  MDP_OSAL_MEMZERO(&sModeParams, sizeof(MDP_SetModeParamType));
  MDP_OSAL_MEMZERO(&sDisplayProp, sizeof(MDP_PropertiesParamType));    
  
  /* Retrieve the cached mode information */
  if (MDP_STATUS_OK != MDPGetProperty(eDisplayId, MDP_DISPLAY_PROPERTY_MODE_INFO, &sDisplayProp))
  {
    eStatus = EFI_DEVICE_ERROR; 
  }  
  else 
  {
    sModeParams.uModeIndex = sDisplayProp.sModeParams.uModeIndex;
    MDP_OSAL_MEMCPY(&sModeParams.sSurfaceInfo, &sDisplayProp.sModeParams.sSurfaceInfo, sizeof(MDPSurfaceInfo));

    if (MDP_STATUS_OK != MDPSetMode(eDisplayId, &sModeParams, 0x0))
    {
      eStatus = EFI_DEVICE_ERROR;
    }
    else if (MDP_DISPLAY_PRIMARY == eDisplayId)
    {
      MDP_OSAL_MEMZERO(&sDisplayProp, sizeof(MDP_PropertiesParamType));

      /* Retrieve the default backlight level */
      sDisplayProp.uBacklightLevel = PcdGet32(PcdBacklightLevel);
            
      /* Set the default backlight level */
      if (MDP_STATUS_OK != MDPSetProperty(eDisplayId, MDP_DISPLAY_PROPERTY_BACKLIGHT, &sDisplayProp))
      {
        eStatus = EFI_DEVICE_ERROR;
      }
      /* Populate the display environment information, used to inform the OS driver of firmware state */
      else if(MDP_STATUS_OK != MDPSetProperty(eDisplayId, MDP_DISPLAY_PROPERTY_FIRMWAREENV, NULL))
      {
        eStatus = EFI_DEVICE_ERROR;
      }
    }
  }

  return eStatus;
}


/**
  Display power UEFI Protocol implementation
 */
EFI_DISPLAY_POWER_PROTOCOL gDisplayPwrProtocolImplementation =
{
  EFI_DISPLAY_POWER_PROTOCOL_REVISION,
  EFI_DisplayPowerSetDisplayPowerState,
  EFI_DisplayPowerGetDisplayPowerState 
}; 
