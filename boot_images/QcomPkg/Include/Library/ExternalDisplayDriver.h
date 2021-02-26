#ifndef EXTERNALDISPLAYDRIVER_H
#define EXTERNALDISPLAYDRIVER_H
/*=============================================================================
 
  File: HALDPLib.h
 
  Header file for DP driver
  
 
  Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#include "MDPTypes.h"
#include "../MDPLib/MDPLib_i.h"


/* --------------------------------------------------------------------------------------------------------------------
**  Enumerated types
---------------------------------------------------------------------------------------------------------------------*/


/* --------------------------------------------------------------------------------------------------------------------
**  Data types
---------------------------------------------------------------------------------------------------------------------*/

typedef struct
{
  MDP_DisplayConnectType          ePhysConnect;                                   /**< Physical connection of display. */
} ExtDisp_AttrType;



/* -----------------------------------------------------------------------
** Function Declarations
-------------------------------------------------------------------------- */

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
bool32 ExtDisp_SupportedByPlatform(ExtDisp_AttrType   *pAttr);


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
MDP_Status ExtDisp_Init(void);

/****************************************************************************
*
** FUNCTION: DP_Driver_IsDisplayPlugged()
*/
/*!
* \brief
*   Check if a panel is plugged into the External display interface
*
*
* \retval bool32
*
****************************************************************************/
bool32 ExtDisp_IsDisplayPlugged(void);

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
MDP_Status ExtDisp_GetModeInfo(uint32 uModeIndex, MDP_Panel_AttrType *psDisplayAttr);

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
MDP_Status ExtDisp_SetMode(uint32 uModeIndex);

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
MDP_Status ExtDisp_Close(void);


#endif  /* #define EXTERNALDISPLAYDRIVER_H */
