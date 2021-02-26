/*=============================================================================
 
  File: MDPLIb.c
 
  Source file for MDP functions
  
 
  Copyright (c) 2013-2019 Qualcomm Technologies, Inc.  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#include <Library/PcdLib.h>
#include "MDPLib.h"
#include "MDPLib_i.h"
#include "MDPSystem.h"
#include "MDPPlatformLib.h"
#include "DisplayUtils.h"
#include "ExternalDisplayDriver.h"


/*=========================================================================
     Default Defines
==========================================================================*/
#define MDSS_BASEADDRESSMAPINGS_MAX               10

/* Default MDPInit Flags */
#define DEAFAULT_MDP_INIT_FLAGS                  (HAL_MDP_INIT_FLAGS_VBIF_CONFIGURATION |\
                                                  HAL_MDP_INIT_FLAGS_DISABLE_INTERRUPTS |\
                                                  HAL_MDP_INIT_FLAGS_CLEAR_INTERRUPTS)


/*=========================================================================
     Local Static Variables
==========================================================================*/


/*=========================================================================
     Local Static Functions
==========================================================================*/


/* Function to detect the presence of external display
 */
static bool32 MDPDetectExtPlugin(MDP_Panel_AttrType  *pPanelInfo);


/* Read UEFI variable DISABLEDISPLAY
 */
static bool32 MDPPlatformGetDisableDisplay(void);

/* Set MDSS base address
 */
static MDP_Status MDPPlatformSetMdssBase(EFIChipInfoFamilyType sEFIChipSetFamily);

/* Configuration parameters that are required to correctly setup the
   DSC related configuration like topology flags and resolution info
   for each compression encoder. 
 */
static void MDPSetupDSCProperty(MDP_Panel_AttrType *pPanelInfo);

/* Configuration default GPIO states
*/
static MDP_Status MDPSetGPIOState(MDP_Panel_AttrType *pDisplayInfo);

/*=========================================================================
     Globals
==========================================================================*/
extern uint8         *gpDSIInitSequenceBuffer;
extern uint8         *gpDSITermSequenceBuffer;
extern uint8         *gpDSIDscPpsBuffer;
extern uint8         *gpI2CInitSequenceBuffer;
extern uint8         *gpI2CTermSequenceBuffer;
MDP_HwPrivateInfo     gsMDPHwPrivateInfo;
MDP_Panel_AttrType    gDisplayInfo[MDP_DISPLAY_MAX];

/*

Mapping for mdss base address
*/
static const sMDSS_BaseAddressMappings asMDSS_BaseAddressMappings[MDSS_BASEADDRESSMAPINGS_MAX] = {  
    {EFICHIPINFO_FAMILY_MSM8998, 0x0C900000},  //for 8998 (Nazgul) family
    {EFICHIPINFO_FAMILY_SDM845,  0x0AE00000},  // for SDM845 (Napali) family
    {EFICHIPINFO_FAMILY_SDM670,  0x0AE00000},  // for SDM670 (Warlock) family
    {EFICHIPINFO_FAMILY_QCS605,  0x0AE00000},  // for QSC605 (RedDragon) family
    {EFICHIPINFO_FAMILY_SXR1130, 0x0AE00000},  // for SXR1130 family
};

/*=========================================================================
      Public APIs
==========================================================================*/


/****************************************************************************
*
** FUNCTION: MDPInit()
*/
/*!
* \brief
*   This function will perform the basic initialization and detection of the MDP core
*
* \param [out] pMDPInitParams   - Information regarding the hardware core
*        [in]  uFlags           - Reserved
*
* \retval MDP_Status
*
****************************************************************************/
MDP_Status  MDPInit(MDP_InitParamsType *pMDPInitParams, uint32 uFlags)
{
    MDP_Status          eStatus             = MDP_STATUS_OK;
    MDP_HwPrivateInfo  *psMDPHwPrivateInfo  = MDP_GETPRIVATEINFO();

    MDP_LOG_FUNC_ENTRY("MDPInit", 0x00);    

    MDP_OSAL_MEMZERO(psMDPHwPrivateInfo, sizeof(MDP_HwPrivateInfo));

    psMDPHwPrivateInfo->sEFIChipSetId = EFICHIPINFO_ID_UNKNOWN;

    if ((NULL == gpDSIInitSequenceBuffer ) ||
        (NULL == gpDSITermSequenceBuffer ))
    {
      gpDSIInitSequenceBuffer = (uint8*)MDP_OSAL_CALLOC(MDP_DSI_COMMAND_BUFFER_SIZE);
      gpDSITermSequenceBuffer = (uint8*)MDP_OSAL_CALLOC(MDP_DSI_COMMAND_BUFFER_SIZE);
    }

    if (NULL == gpDSIDscPpsBuffer )
    {
        gpDSIDscPpsBuffer = (uint8*)MDP_OSAL_CALLOC(MDP_DSI_DSC_PPS_TOTAL_PACKET_SIZE);
    }


    // Initialize I2C command buffer
    if ((NULL == gpI2CInitSequenceBuffer) || 
        (NULL == gpI2CTermSequenceBuffer))
    {
      gpI2CInitSequenceBuffer = (uint8*)MDP_OSAL_CALLOC(MDP_I2C_COMMAND_BUFFER_SIZE);
      gpI2CTermSequenceBuffer = (uint8*)MDP_OSAL_CALLOC(MDP_I2C_COMMAND_BUFFER_SIZE);
    }

    if ((NULL == gpDSIInitSequenceBuffer ) ||
        (NULL == gpDSITermSequenceBuffer ) || 
        (NULL == gpI2CInitSequenceBuffer)  ||
        (NULL == gpI2CTermSequenceBuffer)  ||
        (NULL == gpDSIDscPpsBuffer ))
    {
      eStatus = MDP_STATUS_NO_RESOURCES;
    }
    else if (NULL == pMDPInitParams)
    {
      eStatus = MDP_STATUS_BAD_PARAM;
    }
    else
    {
        MDPPlatformParams     sPlatformParams;
        bool32                bSWRender = FALSE;
        
        if (MDP_INIT_FLAG_MINIMAL_INIT & uFlags)
        {
          //For minimal init do not clear the display context as it will be reused later.
        }
        else
        {
          // Default case is complete intialization, clear the display context.
          MDP_OSAL_MEMZERO(&gDisplayInfo, sizeof(gDisplayInfo));
        }

        // Need to know if the platform is hardware accelerated first. If it is not then we have to skip initialization of all HW.
        MDP_OSAL_MEMZERO(&sPlatformParams, sizeof(MDPPlatformParams));        
        if (MDP_STATUS_OK != (eStatus = MDPPlatformConfigure(MDP_DISPLAY_PRIMARY, MDPPLATFORM_CONFIG_SW_RENDERER, &sPlatformParams)))
        {
           MDP_Log_Message(MDP_LOGLEVEL_WARN, "MDPLib: MDPPlatformConfigure(MDPPLATFORM_CONFIG_SW_RENDERER) failed!\n");  
           
           // Force SW rendering path
           bSWRender = TRUE;
        }

        if (FALSE == bSWRender)
        {
          MDP_OSAL_MEMZERO(&sPlatformParams, sizeof(MDPPlatformParams));
          if (MDP_STATUS_OK != (eStatus = MDPPlatformConfigure(MDP_DISPLAY_PRIMARY, MDPPLATFORM_CONFIG_SW_RENDERER, &sPlatformParams)))
          {
             MDP_Log_Message(MDP_LOGLEVEL_WARN, "MDPLib: MDPPlatformConfigure(MDPPLATFORM_CONFIG_SW_RENDERER) failed!\n");  
             
             // Force SW rendering path
             bSWRender = TRUE;
          }
        }

        if (FALSE == bSWRender)
        {
          bSWRender = sPlatformParams.sPlatformInfo.bSWRender;
        }  
		
        // 
        // Start hardware initialization, fall back to SW renderer in this path if key hardware functions fail
        //
        if (FALSE == bSWRender)
        {

          //Get the platform Chip ID and catch in gsMDPHwPrivateInfo
          if (MDP_STATUS_OK == (eStatus = MDPPlatformConfigure(MDP_DISPLAY_PRIMARY, MDPPLATFORM_CONFIG_GETPLATFORMINFO, &sPlatformParams)))
          {
            psMDPHwPrivateInfo->sEFIChipSetId     = sPlatformParams.sPlatformInfo.sEFIChipSetId;
            psMDPHwPrivateInfo->sEFIChipSetFamily = sPlatformParams.sPlatformInfo.sEFIChipSetFamily;
            psMDPHwPrivateInfo->eEFIPlatformInfo  = sPlatformParams.sPlatformInfo.sEFIPlatformType.platform;
          }
        
          // Hardware path
          //
          if (MDP_STATUS_OK != eStatus)
          {
            // Platform detection failed, cannot continue propagate error to caller
            MDP_Log_Message(MDP_LOGLEVEL_ERROR, "MDPLib: Failed to detect platform ID!\n");
          } 
          else if (TRUE == Display_Utils_CheckPanelSkip())
          {
            //Panel override was set, either skip is forced by override 
            //or this panel is not supported in bootloader
            //Fall back to SW render mode
            MDP_Log_Message(MDP_LOGLEVEL_ERROR, "MDPLib: Panel override forcing swrender mode\n");
            bSWRender = TRUE;
          }
          else if (MDP_STATUS_OK != (eStatus = MDPSetCoreClock(0x0)))
          {
            MDP_Log_Message(MDP_LOGLEVEL_ERROR, "MDPLib: Failed to setup MDP core clocks!\n");
            
            // Fall back to SW render mode
            bSWRender = TRUE;            
          }
          //Set MDSS base address
          else if (MDP_STATUS_OK != (eStatus = MDPPlatformSetMdssBase(sPlatformParams.sPlatformInfo.sEFIChipSetFamily)))
          {
            MDP_Log_Message(MDP_LOGLEVEL_ERROR, "MDPLib: MDPPlatformSetMdssBase() failed!\n");   
            
            // Fall back to SW render mode
            bSWRender = TRUE;
          }
          else
          {
            if (HAL_MDSS_STATUS_SUCCESS != HAL_MDP_Init(NULL, DEAFAULT_MDP_INIT_FLAGS))
            {
              MDP_Log_Message(MDP_LOGLEVEL_ERROR, "MDPLib: HAL_MDP_Init() failed!\n");

              // Fall back to SW render mode
              bSWRender = TRUE;
              eStatus = MDP_STATUS_HW_ERROR;
            }
            // Initialize the HW private info 
            else if (MDP_STATUS_OK != (eStatus = MDPInitHwPrivateInfo(psMDPHwPrivateInfo)))
            {
              MDP_Log_Message(MDP_LOGLEVEL_ERROR, "MDPLib: MDPInitHwPrivateInfo() failed!\n");
              
              // Fall back to SW render mode
              bSWRender = TRUE;
            }
            else if (HAL_MDSS_STATUS_SUCCESS != HAL_MDP_TrafficCtrl_Init(NULL, 0))
            {
              MDP_Log_Message(MDP_LOGLEVEL_ERROR, "MDPLib: HAL_MDP_TrafficCtrl_Init() failed!\n");  
              
              // Fall back to SW render mode
              bSWRender = TRUE;
              eStatus = MDP_STATUS_HW_ERROR;
            }
            else
            {
              // Hardware detected
              // - Setup based on the hardware configuration
              
              ExtDisp_AttrType   sExtDispAttr;
            
              // Populate the input parameters
              pMDPInitParams->uMDPVersionMajor    = psMDPHwPrivateInfo->sMDPVersionInfo.uMajorVersion;
              pMDPInitParams->uMDPVersionMinor    = psMDPHwPrivateInfo->sMDPVersionInfo.uMinorVersion;
              pMDPInitParams->uMDPVersionRevision = psMDPHwPrivateInfo->sMDPVersionInfo.uReleaseVersion;

              //For continuous splash feature, since frame buffer memory is shared between UEFI
              //and kernel, the MMU context need to be updated to enable sharing.
              //Ensure MMU is initialized only once when the DisplayDxe is loaded
			  //to avoid multiple stream faults with dynamic SID programming.   
              if (MDP_INIT_FLAG_MMU_INIT &  uFlags) {
                if (MDP_STATUS_OK != MDP_SetupMMUSIDs(pMDPInitParams->uMDPVersionMajor,
                                                      pMDPInitParams->uMDPVersionMinor))
                {
                  MDP_Log_Message(MDP_LOGLEVEL_ERROR, "MDPLib: Failed to setup MMU SIDs!\n");
                }
              }
              
              // Find the supported displays 
              MDP_OSAL_MEMZERO(&pMDPInitParams->aSupportedDisplays, sizeof(pMDPInitParams->aSupportedDisplays));
              pMDPInitParams->aSupportedDisplays[MDP_DISPLAY_PRIMARY]  = TRUE;
              pMDPInitParams->aSupportedDisplays[MDP_DISPLAY_EXTERNAL] = FALSE;
                  
              // Check the external display configuration.
              if (ExtDisp_SupportedByPlatform(&sExtDispAttr))
              {
                gDisplayInfo[MDP_DISPLAY_EXTERNAL].ePhysConnect          = sExtDispAttr.ePhysConnect;                
                pMDPInitParams->aSupportedDisplays[MDP_DISPLAY_EXTERNAL] = TRUE;
              }

              // If DISABLEDISPLAY variable is set, then disable primary display
              // Note: Make sure to check if external display is supported before proceeding
              if(TRUE == pMDPInitParams->aSupportedDisplays[MDP_DISPLAY_EXTERNAL])
              {
                if(TRUE == MDPPlatformGetDisableDisplay())
                {
                  // DISABLEDISPLAY variable is set. Disable primary display
                  pMDPInitParams->aSupportedDisplays[MDP_DISPLAY_PRIMARY]  = FALSE;
                }
              }
            }
          }
        }

        // Platform is configured for SW renderer, or hardware detection failed
        if (TRUE == bSWRender)
        {
           // Populate the input parameters
           pMDPInitParams->uMDPVersionMajor    = 0;
           pMDPInitParams->uMDPVersionMinor    = 0;
           pMDPInitParams->uMDPVersionRevision = 0;
           
           // Fix the supported displays to just primary
           MDP_OSAL_MEMZERO(&pMDPInitParams->aSupportedDisplays, sizeof(pMDPInitParams->aSupportedDisplays));
           pMDPInitParams->aSupportedDisplays[MDP_DISPLAY_PRIMARY]  = TRUE;
           pMDPInitParams->aSupportedDisplays[MDP_DISPLAY_EXTERNAL] = FALSE;

           // Tell platform layer we are in SW render mode to by pass any hardware configuration
           MDP_OSAL_MEMZERO(&sPlatformParams, sizeof(MDPPlatformParams));
           sPlatformParams.sPlatformInfo.bSWRenderOverrride = TRUE;
           if (MDP_STATUS_OK != (eStatus = MDPPlatformConfigure(MDP_DISPLAY_PRIMARY, MDPPLATFORM_CONFIG_SW_RENDERER, &sPlatformParams)))
           {
             MDP_Log_Message(MDP_LOGLEVEL_ERROR, "MDPLib: MDPPlatformConfigure(MDPPLATFORM_CONFIG_SW_RENDERER) failed!\n");  
           }           
        }

    }

    MDP_LOG_FUNC_EXIT("MDPInit()");   

    return eStatus;
}

/****************************************************************************
*
** FUNCTION: MDPPower()
*/
/*!
* \brief
*   This function will power up and initialize the interface needed for a particular display.
*
* \param [in] eDisplayId       - The display to initialize
*        [in] pMDPPowerParams  - Power configuration
*        [in] uFlags           - Reserved
*
* \retval MDP_Status
*
****************************************************************************/
MDP_Status  MDPPower(MDP_Display_IDType eDisplayId, MDP_PowerParamsType *pMDPPowerParams, uint32 uFlags)
{
    MDP_Status eStatus = MDP_STATUS_OK;

    MDP_LOG_FUNC_ENTRY("MDPPower()", eDisplayId);    

    if (NULL == pMDPPowerParams)
    {
      eStatus = MDP_STATUS_BAD_PARAM;
    }
    else if (TRUE == Display_Utils_CheckPanelSkip())
    {
      //Panel override was set, either skip is forced by override 
      //or this panel is not supported in bootloader
      // Fall back to SW render mode
    }
    else if (TRUE == pMDPPowerParams->bPowerOn)
    {
        MDPPlatformParams sPlatformParams;

        MDP_OSAL_MEMZERO(&sPlatformParams, sizeof(MDPPlatformParams));

        // Power up the respective displays
        if (MDP_STATUS_OK != (eStatus = MDPPlatformConfigure(eDisplayId, MDPPLATFORM_CONFIG_POWERUP, &sPlatformParams)))
        {
            MDP_Log_Message(MDP_LOGLEVEL_WARN, "MDPLib: MDPPlatformConfigure(MDPPLATFORM_CONFIG_POWERUP) failed!\n");   
        }
    }
    else if (FALSE == pMDPPowerParams->bPowerOn)
    {
        MDPPlatformParams sPlatformParams;

        MDP_OSAL_MEMZERO(&sPlatformParams, sizeof(MDPPlatformParams));

        // Power down the respective displays
        if (MDP_STATUS_OK != (eStatus = MDPPlatformConfigure(eDisplayId, MDPPLATFORM_CONFIG_POWERDOWN, &sPlatformParams)))
        {
            MDP_Log_Message(MDP_LOGLEVEL_WARN, "MDPLib: MDPPlatformConfigure(MDPPLATFORM_CONFIG_POWERDOWN) failed!\n");   
        }
    }

    MDP_LOG_FUNC_EXIT("MDPPower()");    

    return eStatus;
}


/****************************************************************************
*
** FUNCTION: MDPDetect()
*/
/*!
* \brief
*   This function will detect the presence of a display and supported modes.
*
* \param [in] eDisplayId       - The display to initialize
*        [in] MDPDetectParams  - Information regarding the hardware core
*        [in] uFlags           - Reserved
*
* \retval MDP_Status
*
****************************************************************************/
MDP_Status  MDPDetect(MDP_Display_IDType eDisplayId, MDP_DetectParamType *pMDPDetectParams, uint32 uFlags )
{
    MDP_Status           eStatus        = MDP_STATUS_OK;
    MDPPlatformPanelType eSelectedPanel = MDPPLATFORM_PANEL_NONE;

    MDP_LOG_FUNC_ENTRY("MDPDetect()", eDisplayId);
    
    if ((NULL == pMDPDetectParams) ||
        (eDisplayId >= MDP_DISPLAY_MAX))
    {
      eStatus = MDP_STATUS_BAD_PARAM;
    }
    else
    {
        MDP_Panel_AttrType  *pDisplayInfo = MDP_GET_DISPLAYINFO(eDisplayId);

        // Handle each display
        switch (eDisplayId)
        {
        case MDP_DISPLAY_PRIMARY:
            {              
              if (MDP_STATUS_OK != MDPDetectPanel(eDisplayId, pDisplayInfo))
              {
                MDP_Log_Message(MDP_LOGLEVEL_WARN, "MDPLib: MDPDetectPanel() failed!\n");
              }
              else
              {
                 pMDPDetectParams->bDisplayDetected          = pDisplayInfo->bDetected;
                 pMDPDetectParams->uSupportedModes           = 1; // Only 1 mode is supported
                 pMDPDetectParams->aModeList[0].bInterlaced  = FALSE;
                 pMDPDetectParams->aModeList[0].uModeIndex   = 0;
                 pMDPDetectParams->aModeList[0].uWidth       = pDisplayInfo->uDisplayWidth;
                 pMDPDetectParams->aModeList[0].uHeight      = pDisplayInfo->uDisplayHeight;
                 eSelectedPanel                              = pDisplayInfo->eSelectedPanel;
                 
                 switch (pDisplayInfo->ePhysConnect)
                 {
                 case MDP_DISPLAY_CONNECT_PRIMARY_DSI_VIDEO:
                 case MDP_DISPLAY_CONNECT_PRIMARY_DSI_CMD:
                 case MDP_DISPLAY_CONNECT_SECONDARY_DSI_VIDEO:
                 case MDP_DISPLAY_CONNECT_SECONDARY_DSI_CMD:                
                    pMDPDetectParams->aModeList[0].uRefreshRate = pDisplayInfo->uAttrs.sDsi.uRefreshRate;

                    /* If DSC is enable setup MDP structures for DSC */
                    if (TRUE == pDisplayInfo->uAttrs.sDsi.bDSCEnable)
                    {
                      if (pDisplayInfo->uAttrs.sDsi.uDSCProfileID < MDP_DSC_PROFILEID_MAX)
                      {
                        MDPSetupDSCProperty(pDisplayInfo);
                      }
                      else
                      {
                        eStatus = MDP_STATUS_BAD_PARAM;
                      }
                    }
                    break;
                 default:
                    break;
                 }
              }
            }
            break;

        case MDP_DISPLAY_EXTERNAL:
            {
              if (!MDPDetectExtPlugin(pDisplayInfo))
              {
                MDP_Log_Message(MDP_LOGLEVEL_INFO, "MDPLib: External monitor not detected!\n");
              }
              else
              {
                MDP_Panel_AttrType    sMode;
                uint32                i;

                // DP detected 
                pDisplayInfo->bDetected            = TRUE;
                pDisplayInfo->uNumInterfaces       = MDP_INTERFACE_SINGLE;
                
                pMDPDetectParams->bDisplayDetected = TRUE;
                
                // Enumerate all modes
                for (i = 0; i < MDP_DISPLAY_MAX_MODES; i++)
                {
                  MDP_OSAL_MEMZERO(&sMode, sizeof(MDP_Panel_AttrType));
                  if (MDP_STATUS_OK == ExtDisp_GetModeInfo(i, &sMode))
                  {
                    pMDPDetectParams->aModeList[i].uModeIndex   = i;
                    pMDPDetectParams->aModeList[i].uWidth       = sMode.uDisplayWidth;
                    pMDPDetectParams->aModeList[i].uHeight      = sMode.uDisplayHeight;
                    pMDPDetectParams->aModeList[i].uRefreshRate = sMode.uRefreshRate;                      
                    pMDPDetectParams->aModeList[i].bInterlaced  = FALSE; 
                  
                    pMDPDetectParams->uSupportedModes++;
                  }
                  else
                  {
                    break;
                  }
                }
              }          
            }
            break;
        default:
            eStatus = MDP_STATUS_BAD_PARAM;
            break;
        }
    }

    //Update ABL with selected panel info
    Display_Utils_SetPanelConfiguration(eSelectedPanel);
    
    MDP_LOG_FUNC_EXIT("MDPDetect()");    

    return eStatus;
}



/****************************************************************************
*
** FUNCTION: MDPSetMode()
*/
/*!
* \brief
*   This function setup the display for a particular mode.
*
* \param [in] eDisplayId       - The display to initialize
*        [in] MDPSetModeParams - Mode setup parameters
*        [in] uFlags           - Reserved
*
* \retval MDP_Status
*
****************************************************************************/
MDP_Status  MDPSetMode(MDP_Display_IDType eDisplayId, MDP_SetModeParamType *pMDPSetModeParams, uint32 uFlags )
{
  MDP_Status           eStatus      = MDP_STATUS_OK;
  MDP_Panel_AttrType  *pDisplayInfo = NULL;

  MDP_LOG_FUNC_ENTRY("MDPSetMode()", eDisplayId); 

  if ((NULL == pMDPSetModeParams)     ||
      (eDisplayId >= MDP_DISPLAY_MAX))
  {
    eStatus = MDP_STATUS_BAD_PARAM;
  }
  else if (TRUE == Display_Utils_CheckPanelSkip())
  {
    //Panel override was set, either skip is forced by override 
    //or this panel is not supported in bootloader
    // Fall back to SW render mode
  }
  else
  {
    pDisplayInfo = MDP_GET_DISPLAYINFO(eDisplayId);

    if (TRUE != pDisplayInfo->bDetected)
    {
      eStatus = MDP_STATUS_NOT_SUPPORTED;
    }
    else if (TRUE == pDisplayInfo->bSWRenderer)
    {
      // Cache the current mode and surface information
      pDisplayInfo->uModeId  = pMDPSetModeParams->uModeIndex;
      MDP_OSAL_MEMCPY(&pDisplayInfo->sFrameBuffer, &pMDPSetModeParams->sSurfaceInfo, sizeof(MDPSurfaceInfo));

      // Display is in software rendering mode, don't initialize hardware and continue
      eStatus = MDP_STATUS_OK;
    }
    else
    {
      // Setup GPIOs (Low and high list)
      if (MDP_STATUS_OK != (eStatus = MDPSetGPIOState(pDisplayInfo)))
      {
        MDP_Log_Message(MDP_LOGLEVEL_WARN, "DisplayDxe: MDPSetGPIOState Failed!\n");
      }
      
      // Setup MDP control paths
      if (MDP_STATUS_OK != (eStatus = MDPSetupPipe(pDisplayInfo, &pMDPSetModeParams->sSurfaceInfo)))
      {
        MDP_Log_Message(MDP_LOGLEVEL_WARN, "DisplayDxe: MDPSetupPipe Failed!\n");
      }
      else if (MDP_STATUS_OK != (eStatus = MDPPanelInit(pMDPSetModeParams->uModeIndex, pDisplayInfo)))
      {
        MDP_Log_Message(MDP_LOGLEVEL_WARN, "DisplayDxe: MDPPanelInit Failed!\n");
      }
      else if (MDP_STATUS_OK != (eStatus = MDPStartPipe(pDisplayInfo))) // Kick start the displays
      {
        MDP_Log_Message(MDP_LOGLEVEL_WARN, "DisplayDxe: MDPStartPipe Failed!\n");
      }
      else
      {
        // Cache the current mode and surface information
        pDisplayInfo->uModeId  = pMDPSetModeParams->uModeIndex;
        MDP_OSAL_MEMCPY(&pDisplayInfo->sFrameBuffer, &pMDPSetModeParams->sSurfaceInfo, sizeof(MDPSurfaceInfo));

        // If DSI video transfer should be on during DCS transactions then send command for sending the init sequence.
        if (TRUE == pDisplayInfo->uAttrs.sDsi.bForceCmdInVideoHS)
        {
          if (MDP_STATUS_OK != MDPPanelSendCommandSequence(pDisplayInfo, MDP_PANEL_COMMAND_INIT, NULL, 0))
          {
            MDP_Log_Message(MDP_LOGLEVEL_WARN, "DisplayDxe: MDPPanelSendCommandSequence() failed to send INIT sequence!\n");
          }
          /* If DSC is enabled then send the PPS command to the panel */
          if (TRUE == pDisplayInfo->uAttrs.sDsi.bDSCEnable) 
          {
            if (MDP_STATUS_OK != MDPPanelSendCommandSequence(pDisplayInfo, MDP_PANEL_COMMAND_PPS, NULL, 0))
            {
              MDP_Log_Message (MDP_LOGLEVEL_WARN, "DisplayDxe: MDPPanelSendCommandSequence() failed to send INIT sequence!\n");
            }
          }
        }
      }
    }
  }

  MDP_LOG_FUNC_EXIT("MDPSetMode()"); 

  return eStatus;
}


/****************************************************************************
*
** FUNCTION: MDPSetProperty()
*/
/*!
* \brief
*   This function will configure a specific property of the display
*
* \param [in] eDisplayId       - The display to initialize
*        [in] eProperty        - The particular property to set
*        [in] MDPSetModeParams - Mode setup parameters
*
* \retval MDP_Status
*
****************************************************************************/
MDP_Status  MDPSetProperty(MDP_Display_IDType eDisplayId, MDP_Display_Property eProperty, MDP_PropertiesParamType *pMDPPropertiesParams)
{
  MDP_Status           eStatus      = MDP_STATUS_OK;

  MDP_LOG_FUNC_ENTRY("MDPSetProperty()", eProperty);        
    
  if (eDisplayId >= MDP_DISPLAY_MAX)
  {
    eStatus = MDP_STATUS_BAD_PARAM;
  }
  else
  {
    MDP_Panel_AttrType  *pDisplayInfo = MDP_GET_DISPLAYINFO(eDisplayId);

    switch (eProperty)
    {
    case MDP_DISPLAY_PROPERTY_BACKLIGHT:
      {
        MDPPlatformParams  sPlatformParams;
        // Setup any other display parameters
        
        MDP_OSAL_MEMZERO(&sPlatformParams, sizeof(MDPPlatformParams));

        sPlatformParams.sBacklightConfig.bEnable                        = TRUE;
        sPlatformParams.sBacklightConfig.eBacklightType                 = pDisplayInfo->eBacklightType;
        sPlatformParams.sBacklightConfig.uBacklightCntrl.eBacklightCtrl = pDisplayInfo->uBacklightCntrl.eBacklightCtrl;
        sPlatformParams.sBacklightConfig.uLevel                         = pMDPPropertiesParams->uBacklightLevel;

        if (MDP_STATUS_OK == (eStatus = MDPPlatformConfigure(eDisplayId, MDPPLATFORM_CONFIG_SETBACKLIGHT, &sPlatformParams)))
        {
          // Cache current backlight level
          pDisplayInfo->uBacklightLevel = pMDPPropertiesParams->uBacklightLevel;
        }
      }
      break;

    case MDP_DISPLAY_PROPERTY_FIRMWAREENV:
      {
        eStatus = MDP_SaveFirmwareEnvironmentVariable(eDisplayId);
      }
      break;  

    case MDP_DISPLAY_PROPERTY_MODE_INFO:
      {
         MDP_HwPrivateInfo *psMDPHwPrivateInfo = MDP_GETPRIVATEINFO();
         uint32             ModeIndex          =  pMDPPropertiesParams->sModeParams.uModeIndex;

         // External monitor supports more than one mode , store the mode info for the selected mode
         // For primary only one mode is supported and this information is populated at the detect time
         switch (eDisplayId)
         {
            case MDP_DISPLAY_EXTERNAL:
               eStatus = ExtDisp_GetModeInfo(ModeIndex, pDisplayInfo);
            break;
            default:
            break;   
         }

         // Check if we need Dual pipe for this panel
         if ((NULL != psMDPHwPrivateInfo->pDeviceCaps) &&
             (pDisplayInfo->uDisplayWidth > psMDPHwPrivateInfo->pDeviceCaps->pResolutionCaps->uMaxLayerWidthPx))
         {
             pDisplayInfo->uNumMixers = MDP_DUALPIPE_NUM_MIXERS;
         }
      }
      break;

    case MDP_DISPLAY_PROPERTY_POWER_STATE:
      {
        /* Cache the current display power state information */
        pDisplayInfo->bDisplayPwrState  =  pMDPPropertiesParams->bDisplayPwrState;
        break;
      }
    case MDP_DISPLAY_PROPERTY_ABL_INTERFACE_INIT:
      {
        /* Initialize Display context which will be used to create panel configuration string for ABL later */
        Display_Utils_Initialize();
        
        break;
      }
    default:
      eStatus = MDP_STATUS_BAD_PARAM;
      break;
    }
  }

  MDP_LOG_FUNC_EXIT("MDPSetProperty()");

  return eStatus;
}


/****************************************************************************
*
** FUNCTION: MDPGetProperty()
*/
/*!
* \brief
*   This function will retrieve a specific property of the display
*
* \param  [in] eDisplayId           - The display to initialize
*         [in] eProperty            - The particular property to set
*         [in] pMDPPropertiesParams - Property parameters
*
* \retval MDP_Status
*
****************************************************************************/
MDP_Status  MDPGetProperty(MDP_Display_IDType eDisplayId, MDP_Display_Property eProperty, MDP_PropertiesParamType *pMDPPropertiesParams)
{
  MDP_Status           eStatus      = MDP_STATUS_OK;

  MDP_LOG_FUNC_ENTRY("MDPGetProperty()", eProperty);

  if (eDisplayId >= MDP_DISPLAY_MAX)
  {
    eStatus = MDP_STATUS_BAD_PARAM;
  }
  else
  {
    MDP_Panel_AttrType  *pDisplayInfo = MDP_GET_DISPLAYINFO(eDisplayId);

    switch (eProperty)
    {
      case MDP_DISPLAY_PROPERTY_BACKLIGHT:
      {
        /* Retrieve the current cached backlight level */
        pMDPPropertiesParams->uBacklightLevel  =  pDisplayInfo->uBacklightLevel;
        break;
      }
    
      case MDP_DISPLAY_PROPERTY_POWER_STATE:
      {
        /* Retrieve the current cached display state information */
        pMDPPropertiesParams->bDisplayPwrState  =  pDisplayInfo->bDisplayPwrState;
        break;
      }

      case MDP_DISPLAY_PROPERTY_MODE_INFO:
      {
        /* Retrieve the current cached display mode information */
        pMDPPropertiesParams->sModeParams.uModeIndex = pDisplayInfo->uModeId;
        MDP_OSAL_MEMCPY(&pMDPPropertiesParams->sModeParams.sSurfaceInfo, &pDisplayInfo->sFrameBuffer, sizeof(MDPSurfaceInfo));
        break;
      }

      case MDP_DISPLAY_PROPERTY_DETECTION_INFO:
      {
        /* Retrieve the current cached display detection information */
        pMDPPropertiesParams->bDisplayDetected = pDisplayInfo->bDetected;      
        break;
      }

      default:
      { 
        eStatus = MDP_STATUS_BAD_PARAM;
        break;
      }
    }
  }

  MDP_LOG_FUNC_EXIT("MDPGetProperty()");

  return eStatus;
}


/****************************************************************************
*
** FUNCTION: MDPDeInit()
*/
/*!
* \brief
*   This function will de-initializes the panel interfaces
*
* \param [in]  eDisplayId       - Display to de-initialize
* \param [in]  uFlags           - Reserved
*
* \retval MDP_Status
*
****************************************************************************/
MDP_Status  MDPDeInit(MDP_Display_IDType eDisplayId, uint32 uFlags)
{
  MDP_Status           eStatus      = MDP_STATUS_OK;

  MDP_LOG_FUNC_ENTRY("MDPDeInit()", eDisplayId);

  if (eDisplayId >= MDP_DISPLAY_MAX)
  {
    eStatus = MDP_STATUS_BAD_PARAM;
  }
  else 
  {
    MDP_Panel_AttrType  *pDisplayInfo = MDP_GET_DISPLAYINFO(eDisplayId);

    if(TRUE == pDisplayInfo->bSWRenderer)
    {
      /* do nothing if display is in SW rendering mode, skip HW configuration */
    }
    else
    {
      /* Handle interface specific differences */
      switch (pDisplayInfo->ePhysConnect)
      {
        case MDP_DISPLAY_CONNECT_PRIMARY_DSI_VIDEO:
        case MDP_DISPLAY_CONNECT_PRIMARY_DSI_CMD:      
          
          /* If DSI video transfer should be on during DCS transactions then send command for sending the termination sequence. */
          if (TRUE == pDisplayInfo->uAttrs.sDsi.bForceCmdInVideoHS)
          {
            if (MDP_STATUS_OK != MDPPanelSendCommandSequence(pDisplayInfo, MDP_PANEL_COMMAND_TERM, NULL, 0))
            {
              MDP_Log_Message(MDP_LOGLEVEL_WARN, "DisplayDxe: MDPPanelSendCommandSequence() failed to send TERM sequence!\n");
            }
          }
          break;
          
         default:
          break;
      }

      /* Turn off the display timing engines */
      if (MDP_STATUS_OK != (eStatus = MDPStopPipe(pDisplayInfo)))
      {
        MDP_Log_Message(MDP_LOGLEVEL_WARN, "MDPLib: MDPPanelDeInit() failed! Status: %d\n", eStatus);      
      }
    
    
      /* Turn off the display controller and panel */
      if (MDP_STATUS_OK != (eStatus = MDPPanelDeInit(pDisplayInfo)))
      {
        MDP_Log_Message(MDP_LOGLEVEL_WARN, "MDPLib: MDPPanelDeInit() failed! Status: %d\n", eStatus);  
      }
    }
  }

  MDP_LOG_FUNC_EXIT("MDPDeInit()");

  return eStatus;
}



/****************************************************************************
*
** FUNCTION: MDPTerm()
*/
/*!
* \brief
*   This function will deinitialize the MDP core and free all resources.
*
* \param [in]  uFlags           - Reserved
*
* \retval MDP_Status
*
****************************************************************************/
MDP_Status  MDPTerm(uint32 uFlags)
{
  MDP_Status           eStatus      = MDP_STATUS_OK;

  MDP_LOG_FUNC_ENTRY("MDPTerm()", 0x00);

  /* Turn off the core clock */
  if (MDP_STATUS_OK != (eStatus = MDPDisableClocks(MDP_CLOCKTYPE_CORE)))
  {
    MDP_Log_Message(MDP_LOGLEVEL_WARN, "MDPLib: MDPDisableClocks() failed!\n");    
  }

  MDP_LOG_FUNC_EXIT("MDPTerm()");

  return eStatus;
}

/****************************************************************************
*
** FUNCTION: MDPExitBoot()
*/
/*!
* \brief
*   This function performs house cleaning before UEFI exit
*
* \param [in]  uFlags           - Reserved
*
* \retval MDP_Status
*
****************************************************************************/
MDP_Status  MDPExitBoot(uint32 uFlags)
{
  MDP_Panel_AttrType  *pPrimaryDisplayInfo  = MDP_GET_DISPLAYINFO(MDP_DISPLAY_PRIMARY);
  MDP_Panel_AttrType  *pExternalDisplayInfo = MDP_GET_DISPLAYINFO(MDP_DISPLAY_EXTERNAL);
  MDP_Status           eStatus              = MDP_STATUS_OK;

  if (NULL == pPrimaryDisplayInfo)
  {
    MDP_Log_Message(MDP_LOGLEVEL_WARN, "MDPExitBoot: Primary Display Info is NULL!\n");    
  }
  /* If seamless splash is not required, turn off the display before leaving */
  else if (MDP_PANEL_FLAG_DISABLE_SEAMLESS_SPLASH & pPrimaryDisplayInfo->eDisplayFlags)
  {
    MDP_PowerParamsType  sPowerParams;

    MDP_OSAL_MEMZERO(&sPowerParams, sizeof(MDP_PowerParamsType));
    sPowerParams.bPowerOn = FALSE;

    if (TRUE == pPrimaryDisplayInfo->bDisplayPwrState)
    {
      // DeInit the panel interfaces before exiting
      if (MDP_STATUS_OK != (eStatus = MDPDeInit(MDP_DISPLAY_PRIMARY, 0x0)))
      {
        MDP_Log_Message(MDP_LOGLEVEL_WARN, "MDPExitBoot: Display Primary DeInit failed with Status(%d)!\n", eStatus);    
      }

      // Turn off MDP powers before exiting
      if (MDP_STATUS_OK != (eStatus = MDPPower(MDP_DISPLAY_PRIMARY, &sPowerParams, 0x0)))
      {
        MDP_Log_Message(MDP_LOGLEVEL_WARN, "MDPExitBoot: Disable Display Primary Power failed with Status(%d)!\n", eStatus);    
      }
    }

    if (NULL == pExternalDisplayInfo)
    {
      MDP_Log_Message(MDP_LOGLEVEL_WARN, "MDPExitBoot: External Display Info is NULL!\n");    
    }
    else if (TRUE == pExternalDisplayInfo->bDisplayPwrState)
    {
      // DeInit the panel interfaces before exiting
      if (MDP_STATUS_OK != (eStatus = MDPDeInit(MDP_DISPLAY_EXTERNAL, 0x0)))
      {
        MDP_Log_Message(MDP_LOGLEVEL_WARN, "MDPExitBoot: Display External DeInit failed with Status(%d)!\n", eStatus);    
      }

      // Turn off MDP powers before exiting
      if (MDP_STATUS_OK != (eStatus = MDPPower(MDP_DISPLAY_EXTERNAL, &sPowerParams, 0x0)))
      {
        MDP_Log_Message(MDP_LOGLEVEL_WARN, "MDPExitBoot: Disable Display External Power failed with Status(%d)!\n", eStatus);    
      }
    }

    // Turn off MDP clocks before exiting
    if (MDP_STATUS_OK != (eStatus = MDPTerm(0x0)))
    {
      MDP_Log_Message(MDP_LOGLEVEL_WARN, "MDPExitBoot: Turn off MDP clocks failed with Status(%d)!\n", eStatus);    
    }
  }

  if (NULL != gpDSIInitSequenceBuffer)
  {
    MDP_OSAL_FREE(gpDSIInitSequenceBuffer);
    gpDSIInitSequenceBuffer = NULL;
  }

  if (NULL != gpDSITermSequenceBuffer)
  {
    MDP_OSAL_FREE(gpDSITermSequenceBuffer);
    gpDSITermSequenceBuffer = NULL;
  }

  if (NULL != gpDSIDscPpsBuffer)
  {
    MDP_OSAL_FREE(gpDSIDscPpsBuffer);
    gpDSIDscPpsBuffer = NULL;
  }

  if (NULL != gpI2CInitSequenceBuffer)
  {
    MDP_OSAL_FREE(gpI2CInitSequenceBuffer);
    gpI2CInitSequenceBuffer = NULL;
  }

  if (NULL != gpI2CTermSequenceBuffer)
  {
    MDP_OSAL_FREE(gpI2CTermSequenceBuffer);
    gpI2CTermSequenceBuffer = NULL;
  }

  // Store display pll codes 
  Display_Utils_StorePLLCodes();

  return MDP_STATUS_OK;
}

/****************************************************************************
*
** FUNCTION: MDPSetCoreClock()
*/
/*!
* \brief
*   This function will setup the MDP core clock, enable footswitch, and restore TZ of register access
*
* \param [in]  uFlags           
*
* \retval MDP_Status
*
****************************************************************************/
MDP_Status  MDPSetCoreClock(uint32 uFlags)
{
  MDP_Status eStatus = MDP_STATUS_OK;

  MDP_LOG_FUNC_ENTRY("MDPSetCoreClock()", 0x00);

  /* Turn on the core clock */
  if (MDP_STATUS_OK != (eStatus = MDPSetupClocks(MDP_CLOCKTYPE_CORE, NULL)))
  {
     MDP_Log_Message(MDP_LOGLEVEL_WARN, "MDPLib: MDPSetupClocks() failed!\n");
  }

  MDP_LOG_FUNC_EXIT("MDPSetCoreClock()");

  return eStatus;
}


#ifdef MDP_ENABLE_PROFILING 
/****************************************************************************
*
** FUNCTION: MDPProfiler()
*/
/*!
* \brief
*   This function performs the profiling of functions 
*
* \param [in]  pFuncName        - Function name
* \param [in]  uParam1          - Display ID, Property or Mode info depending on the function  
* \param [in]  bEntry           - Function entry or exit. True - entry; False - exit;
*
* \retval MDP_Status
*
****************************************************************************/
MDP_Status  MDPProfiler(uint8 *pFuncName, uint32 uParam1, bool32 bEntry)
{
  MDP_Status      eStatus     =  MDP_STATUS_OK; 
  static uint32   uStartTime;
  static uint32   uTotalDriverTime;

  /* Function entry */
  if (bEntry)
  {
    /* Start time */   
    uStartTime = MDP_GetTimerCountUS();
    MDP_Log_Message(MDP_LOGLEVEL_ERROR, "MDPLibProfile: Entry[%a][%d]\n", pFuncName, uParam1);  
  }    
  else 
  {
    /* Function exit */ 
    uint32          uEndTime;
    uint32          uTotalFuncTime = 0;   

    /* End time */ 
    uEndTime = MDP_GetTimerCountUS();
    /* Function execution time(ms) */ 
    uTotalFuncTime = (uEndTime - uStartTime);   
    /* Total driver execution time(ms) */
    uTotalDriverTime += uTotalFuncTime;

    MDP_Log_Message(MDP_LOGLEVEL_ERROR, "MDPLibProfile: Exit[%a][%dus function time][%dus total time]\n", pFuncName, uTotalFuncTime, uTotalDriverTime);
  }

  return eStatus;
}
#endif


/*=========================================================================
     Local Static Variables
==========================================================================*/

static bool32 MDPDetectExtPlugin(MDP_Panel_AttrType  *pPanelInfo)
{
  bool32     bDetected   = FALSE;
  MDP_Status eStatus       = MDP_STATUS_OK;
  
  if (NULL == pPanelInfo)
  {      
   MDP_Log_Message(MDP_LOGLEVEL_WARN, "MDPDetectExtPlugin: Null pointer passed\n");  
  }
  else
  {
    eStatus =  ExtDisp_Init();
    if(MDP_STATUS_OK == eStatus)
    {
      // Get Valid Mode information for mode at Index 0
      if (ExtDisp_IsDisplayPlugged())
      {
        pPanelInfo->eDisplayId    = MDP_DISPLAY_EXTERNAL;
        pPanelInfo->ePhysConnect  = gDisplayInfo[MDP_DISPLAY_EXTERNAL].ePhysConnect;
        bDetected                 = TRUE;

        // Clear the GPIO list with 0xFF to signify an empty list.
        // External display has not implemented the parser to clear default fields
        MDP_OSAL_MEMSET(&pPanelInfo->uDefaultGPIOState, 0xFF, sizeof(pPanelInfo->uDefaultGPIOState));
      }
      else
      {
        ExtDisp_Close();
      }
    }    
  }
  
  return bDetected;
}


/****************************************************************************
*
** FUNCTION: MDPPlatformGetDisableDisplay()
*/
/*!
* \brief
*   Returns TRUE if DISABLEDISPLAY is set and FALSE otherwise
*
*
* \retval bool32
*
****************************************************************************/
static bool32 MDPPlatformGetDisableDisplay(void)
{
  UINT32       uValue        =  0;
  bool32       bDisableDisp  = FALSE; 
  MDP_Status   eStatus       = MDP_STATUS_OK;

  eStatus =  MDP_Display_GetVariable_Integer (L"DISABLEDISPLAY", &uValue);

  if ((MDP_STATUS_OK ==  eStatus) &&
      (uValue         >  0))
  {
    bDisableDisp = TRUE;
  }
  
  return bDisableDisp;
}

/****************************************************************************
*
** FUNCTION: MDPPlatformSetMdssBase()
*/
/*!
* \brief
*   Set MDSS base address
*
*
* \retval MDP_Status
*
****************************************************************************/
static MDP_Status MDPPlatformSetMdssBase(EFIChipInfoFamilyType  sEFIChipSetFamily)
{
  MDP_Status                          Status  = MDP_STATUS_NOT_SUPPORTED;
  HAL_MDP_SWMappedHWBaseAddressesType sMDSSAddress;
  uint8                               uCount;

  MDP_OSAL_MEMZERO(&sMDSSAddress, sizeof(sMDSSAddress));

  //Search for correct chip family
  for(uCount = 0; uCount< MDSS_BASEADDRESSMAPINGS_MAX; uCount++)
  {
    if(asMDSS_BaseAddressMappings[uCount].sEFIChipSetFamily == sEFIChipSetFamily)
    {
      sMDSSAddress.uMdpAddress = asMDSS_BaseAddressMappings[uCount].uMDSSBaseAddress;
      Status                   = MDP_STATUS_OK;
      break;   
    }
  }
  
  //if chip family is not found take a default one , and print warning log.
  if( MDP_STATUS_OK != Status)
  {
    MDP_Log_Message(MDP_LOGLEVEL_WARN, "MDPLib: MDPPlatformSetMdssBase - Chipset Family 0x%x not found, cannot continue!\n");
    Status = MDP_STATUS_BAD_PARAM;
  }
  else 
  {
    // Setup the HAL base address
    HAL_MDSS_SetBaseAddress(&sMDSSAddress);
  }

  return Status;
}
/* ---------------------------------------------------------------------- */
/**
** FUNCTION: MDPSetupDSCProperty()
**
** DESCRIPTION:
**   Configuration parameters that are required to correctly setup the
**   DSC related configuration like topology flags and resolution info
**   for each compression encoder.
**
** ---------------------------------------------------------------------- */
void MDPSetupDSCProperty(MDP_Panel_AttrType *pPanelInfo)
{
  HAL_MDP_DSCInfoType             sDSCInfo;
  HAL_MDP_DSCConfigType           sDSCConfig;
  uint32                          uProgramBothPingPongs = 0;
  uint32                          uCount;

  MDP_OSAL_MEMZERO(&sDSCInfo, sizeof(HAL_MDP_DSCInfoType));
  MDP_OSAL_MEMZERO(&sDSCConfig, sizeof(HAL_MDP_DSCConfigType));
  MDP_OSAL_MEMZERO(pPanelInfo->uAttrs.sDsi.pDSCPpsBuffer, MDP_DSI_DSC_PPS_TOTAL_PACKET_SIZE);

  pPanelInfo->uAttrs.sDsi.uDSCBpc              = gDscProfileModes[pPanelInfo->uAttrs.sDsi.uDSCProfileID].uBitsPerComponent;
  pPanelInfo->uAttrs.sDsi.uDSCBpp              = gDscProfileModes[pPanelInfo->uAttrs.sDsi.uDSCProfileID].uBitsPerPixel;
  pPanelInfo->uAttrs.sDsi.uDSCCompressionRatio = gDscProfileModes[pPanelInfo->uAttrs.sDsi.uDSCProfileID].uCompressionRatio;
  pPanelInfo->uAttrs.sDsi.bDSCLayerMixSplit    = gDscProfileModes[pPanelInfo->uAttrs.sDsi.uDSCProfileID].bLMSplitEnable;
  pPanelInfo->uAttrs.sDsi.bDSCBlockPred        = gDscProfileModes[pPanelInfo->uAttrs.sDsi.uDSCProfileID].bBlockPredEnable;

  sDSCInfo.pDSCEncoderPacket = &pPanelInfo->uAttrs.sDsi.sDSCEncPktType;

  /* Setup the EncParams */
  /* Configure the start address where the HAL will create the PPS command payload data */
  sDSCInfo.pDSCEncoderPacket->pPacketData    = pPanelInfo->uAttrs.sDsi.pDSCPpsBuffer + MDP_DSI_DSC_HEADER_SIZE + MDP_DSI_DSC_XML_HEADER_SIZE;
  sDSCInfo.pDSCEncoderPacket->pEncoderParams = &pPanelInfo->uAttrs.sDsi.sDSCEncParams;

  /* Populate DSC Version */
  sDSCInfo.pDSCEncoderPacket->pEncoderParams->uDSCVersionMajor = pPanelInfo->uAttrs.sDsi.uDSCMajor;
  sDSCInfo.pDSCEncoderPacket->pEncoderParams->uDSCVersionMinor = pPanelInfo->uAttrs.sDsi.uDSCMinor;
  sDSCInfo.pDSCEncoderPacket->pEncoderParams->uDSCVersionScr   = pPanelInfo->uAttrs.sDsi.uDSCScr;

  /* Determine the type of DSC encoder configuration */
  sDSCInfo.pDSCEncoderCfg                         = &pPanelInfo->uAttrs.sDsi.sDSCEncCfg;
  sDSCInfo.pDSCEncoderCfg->pEncoderParams         = &pPanelInfo->uAttrs.sDsi.sDSCEncParams;
  sDSCInfo.pDSCEncoderCfg->bUserProvidedEncParams = FALSE;

  sDSCInfo.pDSCEncoderCfg->eCompressionRatio      = pPanelInfo->uAttrs.sDsi.uDSCCompressionRatio;

  /* Configure the Slice Width and height */
  sDSCInfo.pDSCEncoderCfg->pEncoderParams->uSliceHeight = pPanelInfo->uAttrs.sDsi.uDSCSliceHeight;
  sDSCInfo.pDSCEncoderCfg->pEncoderParams->uSliceWidth  = pPanelInfo->uAttrs.sDsi.uDSCSliceWidth;

  sDSCInfo.pDSCEncoderCfg->pEncoderParams->uBitsPerComponent =  pPanelInfo->uAttrs.sDsi.uDSCBpc; 
  sDSCInfo.pDSCEncoderCfg->pEncoderParams->uBitsPerPixel     = pPanelInfo->uAttrs.sDsi.uDSCBpp;
  sDSCInfo.pDSCEncoderCfg->pEncoderParams->bBlockPredEnable  = pPanelInfo->uAttrs.sDsi.bDSCBlockPred;

  if ((0 == sDSCInfo.pDSCEncoderCfg->pEncoderParams->uSliceHeight) ||
      (sDSCInfo.pDSCEncoderCfg->pEncoderParams->uSliceHeight > pPanelInfo->uDisplayHeight))
  {
    /* Default Slice height to 16*/
    sDSCInfo.pDSCEncoderCfg->pEncoderParams->uSliceHeight = 16;
    MDP_Log_Message(MDP_LOGLEVEL_WARN, "MDPLib: MDPSetupDSCProperty: Slice Height not defined in XML, use default\n");
  }

  if ((0 == sDSCInfo.pDSCEncoderCfg->pEncoderParams->uSliceWidth) ||
      (sDSCInfo.pDSCEncoderCfg->pEncoderParams->uSliceWidth > pPanelInfo->uDisplayWidth))
  {
    /* Default slice width is half of the panel width */
    sDSCInfo.pDSCEncoderCfg->pEncoderParams->uSliceWidth = pPanelInfo->uDisplayWidth / 2;
    MDP_Log_Message(MDP_LOGLEVEL_WARN, "MDPLib: MDPSetupDSCProperty: Slice Width not defined in XML, use default\n");
  }

  if (MDP_INTERFACE_SINGLE == pPanelInfo->uNumInterfaces)
  {
    if (0 == pPanelInfo->uAttrs.sDsi.bDSCLayerMixSplit)
    {
      /* Single encoder and single dsi Based on the profile id setup the following */
      sDSCInfo.pDSCEncoderCfg->bPingPongSplitMUXActive    = 0;
      sDSCInfo.pDSCEncoderCfg->bSplitDispActive           = 0;
      sDSCInfo.pDSCEncoderCfg->bSplitMuxActive            = 0;
      sDSCInfo.pDSCEncoderCfg->uInputFrameHeightInPixels  = pPanelInfo->uDisplayHeight;
      sDSCInfo.pDSCEncoderCfg->uInputFrameWidthInPixels   = pPanelInfo->uDisplayWidth;

    }
    else
    {
      /* Place holder when we implement the 2.5k Panel with DSC with dual pipe/mixer */
      /* Single DSI interface, Layer Mixer Split Config */

      sDSCInfo.pDSCEncoderCfg->bPingPongSplitMUXActive = 0;
      sDSCInfo.pDSCEncoderCfg->bSplitDispActive        = 0;
      sDSCInfo.pDSCEncoderCfg->bSplitMuxActive         = 1;

      sDSCInfo.pDSCEncoderCfg->uInputFrameHeightInPixels = pPanelInfo->uDisplayHeight;

      /* Total Width is multiplied by 2 */
      sDSCInfo.pDSCEncoderCfg->uInputFrameWidthInPixels = pPanelInfo->uDisplayWidth;
      uProgramBothPingPongs = 1;
    }

  }
  else if (MDP_INTERFACE_DUAL == pPanelInfo->uNumInterfaces)
  {
    /* Place holder when we implement 4k Panel with DSC */

    /* Single Pipe Config(one encoder) -> each DSI  */
    sDSCInfo.pDSCEncoderCfg->bPingPongSplitMUXActive = 0;
    sDSCInfo.pDSCEncoderCfg->bSplitDispActive        = 1;
    sDSCInfo.pDSCEncoderCfg->bSplitMuxActive         = 0;

    sDSCInfo.pDSCEncoderCfg->uInputFrameHeightInPixels = pPanelInfo->uDisplayHeight;
    sDSCInfo.pDSCEncoderCfg->uInputFrameWidthInPixels  = pPanelInfo->uDisplayWidth;

    /* Need to setup both the DSC encoders and the split mux  */

    uProgramBothPingPongs = 1;
  }

  if ((MDP_DISPLAY_CONNECT_PRIMARY_DSI_VIDEO == pPanelInfo->ePhysConnect) ||
      ( MDP_DISPLAY_CONNECT_SECONDARY_DSI_VIDEO == pPanelInfo->ePhysConnect))
  {
    sDSCInfo.pDSCEncoderCfg->eInterfaceMode = HAL_MDP_INTERFACE_MODE_VIDEO;
  }
  else if ((MDP_DISPLAY_CONNECT_PRIMARY_DSI_CMD == pPanelInfo->ePhysConnect) ||
           (MDP_DISPLAY_CONNECT_SECONDARY_DSI_CMD == pPanelInfo->ePhysConnect))
  {
    sDSCInfo.pDSCEncoderCfg->eInterfaceMode = HAL_MDP_INTERFACE_MODE_COMMAND;
  }

  sDSCConfig.pDSCEncoderCfg                                   = sDSCInfo.pDSCEncoderCfg;

  /* Calculate the DSC configuration settings for HAL */
  HAL_MDP_DSC_GetPropery(0, &sDSCInfo, 0x00);
  // Save the BytePerPkt info to configure the DSI controller 
  pPanelInfo->uAttrs.sDsi.sDSCEncCfg.uBytePerPkt = sDSCInfo.pDSCEncoderCfg->uBytePerPkt;

  /* If there are 2 control paths then program both */
  for (uCount = 0; uCount <= uProgramBothPingPongs; uCount++)
  {
    /* Commit the DSC registers */
    HAL_MDP_DSC_Setup(uCount + 1, &sDSCConfig, 0x00);
  }
}

/* ---------------------------------------------------------------------- */
/**
** FUNCTION: MDPSetGPIOState()
**
** DESCRIPTION:
**   Configuration default GPIO states
**
** ---------------------------------------------------------------------- */
static MDP_Status MDPSetGPIOState(MDP_Panel_AttrType *pDisplayInfo)
{
  MDP_Status    eStatus     = MDP_STATUS_OK;
  
  for (uint32 uGPIOState = 0;uGPIOState <= 1;uGPIOState++)
  {
    MDPPlatformParams sPlatformConfig;

    MDP_OSAL_MEMZERO(&sPlatformConfig, sizeof(MDPPlatformParams));
    sPlatformConfig.sGPIOState.eGPIOType = MDPPLATFORM_GPIOTYPE_TLMM;
    sPlatformConfig.sGPIOState.pGPIOList = (uint32*)&(pDisplayInfo->uDefaultGPIOState[uGPIOState]);
    sPlatformConfig.sGPIOState.uListSize = MDP_MAX_GPIO_LIST_ENTRIES;
    sPlatformConfig.sGPIOState.uState = uGPIOState;

    if (MDP_STATUS_OK != MDPPlatformConfigure(pDisplayInfo->eDisplayId, MDPPLATFORM_CONFIG_SETGPIOSTATE, &sPlatformConfig))
    {
      MDP_Log_Message(MDP_LOGLEVEL_WARN, "DisplayDxe: Failed to configure list of GPIOs to %d\n", uGPIOState);
      eStatus = MDP_STATUS_FAILED;
      break;
    }
  }
  return eStatus;
}

