#ifndef HALDPLIB_H
#define HALDPLIB_H
/*=============================================================================
 
  File: HALDPLib.h
 
  Header file for DP driver
  
 
  Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#include "MDPTypes.h"
#include "../MDPLib/MDPLib_i.h"

/* -----------------------------------------------------------------------
** Function Declarations
-------------------------------------------------------------------------- */


/****************************************************************************
*
** FUNCTION: DP_Driver_Init()
*/
/*!
* \brief
*   Initialize DP driver (host and panel)
*
*
* \retval MDP_Status
*
****************************************************************************/
MDP_Status DP_Driver_Init(void);

/****************************************************************************
*
** FUNCTION: DP_Driver_IsDisplayPlugged()
*/
/*!
* \brief
*   Check if a panel is plugged into the DP interface
*
*
* \retval bool32
*
****************************************************************************/
bool32 DP_Driver_IsDisplayPlugged(void);

/************************************************************************************************************
*
** FUNCTION: DP_Driver_GetModeInfo()
*/
/*!
* \brief
*   Retrieves the mode info of the display plugged into the DP interface
*
* \param [in]   uModeIndex           - Index of mode to retrieve
* \param [out]  psDisplayAttr        - Pointer to mode info
*
* \retval MDP_Status          MDP_STATUS_OK = mode supported, MDP_STATUS_NOT_SUPPORTED = mode not supported
*
**************************************************************************************************************/
MDP_Status DP_Driver_GetModeInfo(uint32 uModeIndex, MDP_Panel_AttrType *psDisplayAttr);

/************************************************************************************************************
*
** FUNCTION: DP_Driver_SetMode()
*/
/*!
* \brief
*   Set a mode on the display plugged into the DP interface
*
* \param [in]   uModeIndex           - Index of mode to set
*
* \retval MDP_Status          MDP_STATUS_OK = mode set successfully
*
**************************************************************************************************************/
MDP_Status DP_Driver_SetMode(uint32 uModeIndex);

/************************************************************************************************************
*
** FUNCTION: DP_Driver_Close()
*/
/*!
* \brief
*   Deinitialize DP driver (and power down components)
*
*
* \retval MDP_Status          
*
**************************************************************************************************************/
MDP_Status DP_Driver_Close(void);


#endif  /* #define HALDPLIB_H */
