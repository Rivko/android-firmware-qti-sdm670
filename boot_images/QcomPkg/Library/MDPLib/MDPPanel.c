/*=============================================================================
 
  File: MDPPanel.c
 
  Source file for panel specific configuration
  
 
  Copyright (c) 2011-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/
/*=========================================================================
      Include Files
==========================================================================*/
#include "MDPLib.h"
#include "MDPPlatformLib.h"
#include "HALDSILib.h"
#include "ExternalDisplayDriver.h"


/* -----------------------------------------------------------------------
** Defines
** ----------------------------------------------------------------------- */

/*=========================================================================
      Local prototypes
==========================================================================*/




/*=========================================================================
      Public functions
==========================================================================*/

/****************************************************************************
*
** FUNCTION: MDPPanelDeInit()
*/
/*!
* \brief
*   This function de-intializes the interfaces.
*
* \param [in] pPanelInfo    - The panel configuration
*
* \retval MDP_Status
*
****************************************************************************/
MDP_Status  MDPPanelDeInit(MDP_Panel_AttrType    *pPanelInfo)
{
  MDP_Status eStatus    = MDP_STATUS_OK;

  switch (pPanelInfo->eDisplayId)
  {
    case MDP_DISPLAY_PRIMARY:
      switch (pPanelInfo->ePhysConnect)
      {         
        case MDP_DISPLAY_CONNECT_PRIMARY_DSI_VIDEO:
        case MDP_DISPLAY_CONNECT_PRIMARY_DSI_CMD:
    
          // Graceful shutdown of the panel
          DsiPanelShutdown(pPanelInfo);
          
          DSIDriver_Close();
          break;
  
        default:
          break;
      }
      break;
   
    case MDP_DISPLAY_EXTERNAL:
      eStatus = ExtDisp_Close();
      break;

    default:
      break;
  }

  return eStatus;
}


/****************************************************************************
*
** FUNCTION: MDPPanelInit()
*/
/*!
* \brief
*   This function setup the display for a particular mode.
*
* \param [in] uModeIndex    - The display mode to configure
*        [in] pPanelInfo    - The panel configuration
*
* \retval MDP_Status
*
****************************************************************************/
MDP_Status  MDPPanelInit(uint32 uModeIndex, MDP_Panel_AttrType    *pPanelInfo)
{
    MDP_Status eStatus    = MDP_STATUS_OK;

    switch (pPanelInfo->eDisplayId)
    {
      case MDP_DISPLAY_PRIMARY:
        switch (pPanelInfo->ePhysConnect)
        {
          case MDP_DISPLAY_CONNECT_PRIMARY_DSI_VIDEO:
          case MDP_DISPLAY_CONNECT_PRIMARY_DSI_CMD:
            /* Setup DSI Configuration */
            eStatus = DsiPanelInit(pPanelInfo);
            break;
  
          default:
            break;
        }
        break;
     
      case MDP_DISPLAY_EXTERNAL:
        eStatus = ExtDisp_SetMode(uModeIndex);
        break;
  
      default:
        break;
    }

    return eStatus;
}

/****************************************************************************
*
** FUNCTION: MDPPanelGetEDID()
*/
/*!
* \brief
*   This function read EDID info from panel.
*
* \param [in] pPanelInfo    - The panel configuration
*
* \retval MDP_Status
*
****************************************************************************/
MDP_Status  MDPPanelGetEDID( MDP_Panel_AttrType    *pPanelInfo)
{
  MDP_Status eStatus    = MDP_STATUS_OK;

  /* Configure the display */
  switch (pPanelInfo->ePhysConnect)
  {
    case MDP_DISPLAY_CONNECT_DP:
    {
      eStatus = MDP_STATUS_NOT_SUPPORTED;
      break;
    }
    default:
    {
      break;
    }
  }

  return eStatus;
}

/****************************************************************************
*
** FUNCTION: MDPPanelSendCommandSequence()
*/
/*!
* \brief
*   Sends a predefined command set to the panel.
*        [in] pPanelInfo      - The panel configuration
*        [in] eCommandType    – Command type to send
*        [in] pCustomCommand  – Custom command buffer, only valid for MDP_PANEL_COMMAND_CUSTOM
*        [in] uFlags          – Reserved.
*
* \retval MDP_Status
*
****************************************************************************/
MDP_Status  MDPPanelSendCommandSequence(MDP_Panel_AttrType *pPanelInfo, MdpPanelCommandType eCommand, void *pCustomCommand, uint32 uFlags)
{
    MDP_Status eStatus    = MDP_STATUS_OK;
 
    /* Configure the display */
    switch (pPanelInfo->ePhysConnect)
    {     
      case MDP_DISPLAY_CONNECT_PRIMARY_DSI_VIDEO:
      case MDP_DISPLAY_CONNECT_PRIMARY_DSI_CMD:
        /* Setup DSI Configuration */
        eStatus = DsiPanelSendCommandSequence(pPanelInfo, eCommand, pCustomCommand, uFlags);
        break;
      default: 
        break;
    }

    return eStatus;
}


