/*=============================================================================
 
  File: ExtDisplay_Driver.c
 
  Interface layer for External display driver
  
 
 Copyright (c) 2016-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.
 =============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif


/*=========================================================================
      Include Files
==========================================================================*/
#include <Library/PcdLib.h>
#include "ExternalDisplayDriver.h"
#include "MDPSystem.h"


/*=========================================================================
      Types
==========================================================================*/


/*=========================================================================
      Global variables
==========================================================================*/



/*=========================================================================
      Public Functions
==========================================================================*/

/****************************************************************************
*
** FUNCTION: ExtDisp_SupportedByPlatform()
*/
/*!
* \brief
*   Does platform support external display
*
* \param [out]   pAttr           - Attributes of external display
*
* \retval bool32                   TRUE - supported, FALSE - not supported
*
****************************************************************************/
bool32 ExtDisp_SupportedByPlatform(ExtDisp_AttrType   *pAttr)
{
  uint32 uExtDispType   = PcdGet32(PcdExtDisplayType); 
  bool32 bSupported     = FALSE;

  switch (uExtDispType)
  {     
    case PCD_EXTERNAL_DISPLAY_DP:
      bSupported          = FALSE;
      break;
   
    case PCD_EXTERNAL_DISPLAY_HDMI:
      bSupported          = FALSE;
      break;
      
    case PCD_EXTERNAL_DISPLAY_NONE:
    default:
      break;
  }
  
  return bSupported;
}


/****************************************************************************
*
** FUNCTION: ExtDisp_Init()
*/
/*!
* \brief
*   Initialize External display driver (host and panel)
*
*
* \retval MDP_Status
*
****************************************************************************/
MDP_Status ExtDisp_Init(void)
{
  MDP_Status  eStatus = MDP_STATUS_OK;

  return eStatus;
}

/****************************************************************************
*
** FUNCTION: ExtDisp_IsDisplayPlugged()
*/
/*!
* \brief
*   Check if a panel is plugged into the External display interface
*
*
* \retval bool32
*
****************************************************************************/
bool32 ExtDisp_IsDisplayPlugged(void)
{  
  bool32              bPluggedIn  = FALSE;

  return bPluggedIn;
}

/************************************************************************************************************
*
** FUNCTION: ExtDisp_GetModeInfo()
*/
/*!
* \brief
*   Retrieves the mode info of the display plugged into the External display interface
*
* \param [in]   uModeIndex           - Index of mode to retrieve
* \param [out]  psDisplayAttr        - Pointer to mode info
*
* \retval MDP_Status          MDP_STATUS_OK = mode supported, MDP_STATUS_NOT_SUPPORTED = mode not supported
*
**************************************************************************************************************/
MDP_Status ExtDisp_GetModeInfo(uint32 uModeIndex, MDP_Panel_AttrType *pModeInfo)
{  
  MDP_Status          eStatus       = MDP_STATUS_OK;

  return eStatus;
}

/************************************************************************************************************
*
** FUNCTION: ExtDisp_SetMode()
*/
/*!
* \brief
*   Set a mode on the display plugged into the External display interface
*
* \param [in]   uModeIndex           - Index of mode to set
*
* \retval MDP_Status          MDP_STATUS_OK = mode set successfully
*
**************************************************************************************************************/
MDP_Status ExtDisp_SetMode(uint32 uModeIndex)
{
  MDP_Status  eStatus = MDP_STATUS_OK;  
  
  return eStatus;
}


/************************************************************************************************************
*
** FUNCTION: ExtDisp_Close()
*/
/*!
* \brief
*   Deinitialize External display driver (and power down components)
*
*
* \retval MDP_Status          
*
**************************************************************************************************************/
MDP_Status ExtDisp_Close(void)
{
  MDP_Status  eStatus = MDP_STATUS_OK;  
  
  return eStatus;
}



