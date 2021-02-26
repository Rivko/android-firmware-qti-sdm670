#ifndef __DISPLAYUTILS_H__
#define __DISPLAYUTILS_H__
/*=============================================================================
 
  File: DisplayUtils.h
 
  Header file for Display Utils interface functions & properties

  
  Copyright (c) 2016-2017, 2019 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

/* Copyright (c) 2013-2016, The Linux Foundation. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*  * Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*  * Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in
*    the documentation and/or other materials provided with the
*    distribution.
*  * Neither the name of The Linux Foundation nor the names of its
*    contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
* OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
* AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
* OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
* SUCH DAMAGE.
*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <Uefi.h>
#include "MDPTypes.h"
#include "MDPPlatformLib.h"

/*===========================================================================

                           Defines

===========================================================================*/

/* Names of display boot service variables
*/
#define DISPVAR_PANEL_OVERRIDE          L"DisplayPanelOverride"          /* Display override from ABL */
#define DISPVAR_SELECTED_PANEL          L"DisplaySelectedPanel"          /* Panel selected by UEFI */
#define DISPVAR_PANEL_CONFIGURATION     L"DisplayPanelConfiguration"     /* Panel configuration for ABL */
#define DISPVAR_DFPS_INFO               L"DisplayDFPSInfo"               /* Dynamic FPS data */
#define DISPVAR_SUPPORTED_PANEL_COUNT   L"DisplaySupportedPanelCount"    /* Number of panels supported by platform */
#define DISPVAR_SUPPORTED_PANEL_LIST    L"DisplaySupportedPanelList"     /* Comma separated list of supported panels */
#define DISPVAR_SPLASH_BUFFER_INFO      L"DisplaySplashBufferInfo"       /* splash buffer address & size */

/* Maximum number of panels supported by platform */
#define DISPLAY_PANEL_LIST_MAX_COUNT                           10


/* Maximum length for DSI config string 
*/
#define DISPLAY_DSI_CONFIG_MAX_LEN                             32

/* Version number for display splash buffer info type structure
*/
#define DISPLAY_SPLASH_BUFFER_INFO_TYPE_VERSION                1
  
// maximum length for panel override string 
#define PANEL_OVERRIDE_STR_LEN_MAX                             256

/*===========================================================================

                          Enumerations

===========================================================================*/

/* Panel Simulator type 
*/
typedef enum
{
  PANELSIM_NONE,                  /* No simulation */
  PANELSIM_MODE,                  /* simulator mode for video panels */
  PANELSIM_SWTE,                  /* Simulator mode with software tearcheck for cmd mode panels */
  PANELSIM_HWTE                   /* Simulator mode with hardware testcheck for cmd mode panels with terminator card */
} PanelSimulatorType;

/* PLL override types
*/
typedef enum
{
  PLL_OVERRIDE_NONE,              /* No PLL override - use default */
  PLL_OVERRIDE_PLL0,              /* Force PLL 0 */
  PLL_OVERRIDE_PLL1               /* Force PLL 1 */
} PLLOverrideType;

/* PLL override types
*/
typedef enum
{
  DISP_TOPOLOGY_CONFIG_NONE,      /* No topology Configuration - use default */
  DISP_TOPOLOGY_CONFIG_0,         /* Force config 0 for topology */
  DISP_TOPOLOGY_CONFIG_1,         /* Force config 1 for topology */
  DISP_TOPOLOGY_CONFIG_2,         /* Force config 2 for topology */
} DisplayTopologyCfgType;

typedef enum
{
  DISP_TIMING_CONFIG_NONE,      /* No timing Configuration - use default */
  DISP_TIMING_CONFIG_0,         /* Force config 0 for timing */
  DISP_TIMING_CONFIG_1,         /* Force config 1 for timing */
  DISP_TIMING_CONFIG_2,         /* Force config 2 for timing */
} DisplayTimingCfgType;

typedef enum
{
  DISP_MODE_NONE                        = 0x00000000,    /* None */
  DISP_MODE_SINGLE_DSI                  = 0x00000001,    /* Single DSI */
  DISP_MODE_DUAL_DSI                    = 0x00000002,    /* Dual DSI */
  DISP_MODE_SPLIT_DISPLAY               = 0x00000004,    /* Split Display*/
  DISP_MODE_DUAL_PIPE                   = 0x00000008,    /* Dual pipe */
  DISP_MODE_PIPE_SWAP                   = 0x00000010,    /* Swap pipes in dual pipe mode */
  DISP_MODE_PING_PONG_SPLIT             = 0x00000020,    /* Ping pong split mode */
  DISP_MODE_USE_DSI1                    = 0x00000040,    /* Force DSI1 as Primary */
  DISP_MODE_SKIP_BOOTLOADER             = 0x00000080,    /* Force config 2 for topology */
  DISP_MODE_DISABLE_CONTINUOUS_SPLASH   = 0x00000100,    /* Disable continuous splash */
  DISP_MODE_SWAP_DSI_CONTROLLER         = 0x00000200,    /* Swap DSI controller (wrt to what is set in panelconfig xml)*/
  DISP_MODE_PANEL_NONE                  = 0x00000400,    /* override with no panel */
  DISP_MODE_DSC_MERGE                   = 0x00000800,    /* DSC merge*/
  DISP_MODE_SEAMLESS_SPLASH_ON          = 0x00001000,    /* Seamless splash on flag */
  DISP_MODE_SEAMLESS_SPLASH_OFF         = 0x00002000,    /* Seamless splash off flag */
} DisplayModeType;
  

/*===========================================================================

                          Structures

===========================================================================*/

/* Panel override parameters
 */
typedef struct 
{
  CHAR8                         shortName[DISPLAY_PANEL_NAME_MAX_LEN];    /* Short name for panel */
  const PanelInfoType*          psDTInfo;                                 /* DT parameters        */
  DisplayTopologyCfgType        eTopologyCfg;                             /* Topology configure   */
  DisplayTimingCfgType          eTimingCfg;                               /* Timing configure   */
  uint32                        uSelectedModeFlags;                       /* Selected Mode flag, refer DisplayModeType */
} PanelOverrideParams;

/* Display override parameters
*/
typedef struct
{
  PanelOverrideParams     sPrimary;                                       /* Primary panel override */
  PanelOverrideParams     sSecondary;                                     /* Secondary panel override */
  PanelSimulatorType      eSimMode;                                       /* Panel simulator mode */
  CHAR8                   dsiConfigStr[DISPLAY_DSI_CONFIG_MAX_LEN];       /* DSI configuration string */
  PLLOverrideType         ePllOverride;                                   /* PLL override */
  uint32                  uFlags;                                         /* Additional override flags */
} Display_UtilsParamsType;

/* Splash buffer data
*/
typedef struct
{
  uint32 uVersion;                                    /* Version number used to track changes to structure */
  uint32 uFrameAddr;                                  /* Physical address of the frame buffer */
  uint32 uFrameSize;                                  /* Frame buffer size */
} DisplaySplashBufferInfoType;

/*===========================================================================

                        Public Functions

===========================================================================*/

/****************************************************************************
*
** FUNCTION: Display_Utils_Initialize()
*/
/*!
* \brief
*   Initialize Utils interface
*
* \param None
*
* \retval None
*
****************************************************************************/
void Display_Utils_Initialize(void);


/****************************************************************************
*
** FUNCTION: Display_Utils_GetParams()
*/
/*!
* \brief
*   Helper to get display override params
*
* \param None
*
* \retval pointer to Display_UtilsParamsType or NULL if not set
*
****************************************************************************/
Display_UtilsParamsType* Display_Utils_GetParams(void);


/****************************************************************************
*
** FUNCTION: Display_Utils_SetPanelConfiguration()
*/
/*!
* \brief
*   Send panel configuration string to ABL
*
* \param Index of selected panel
*
* \retval None
*
****************************************************************************/
void Display_Utils_SetPanelConfiguration(MDPPlatformPanelType  eSelected);

/****************************************************************************
*
** FUNCTION: Display_Utils_PanelInfo_Update(void)
*/
/*!
* \brief
*   apply oem cmd line flags to panel info flags
*
*
* \retval None
*
****************************************************************************/
void Display_Utils_PanelInfo_Update(void);


/****************************************************************************
*
** FUNCTION: Display_Utils_CheckPanelSkip()
*/
/*!
* \brief
*   Check for panel override and if the overriden panel is supported in 
*   bootloader
*
* \param None
*
* \retval TRUE if panel configuration is to be skipped in bootloader
*
****************************************************************************/
bool32 Display_Utils_CheckPanelSkip(void);


/****************************************************************************
*
** FUNCTION: Display_Utils_GetDisplayCommandLine()
*/
/*!
* \brief
*   Gets the pointer to the panel configuration string & string length 
*
* \param [out] ppCmdLine - pointer to command line string
*        [out] pLen      - Ascii length of the string
*
* \retval None
*
****************************************************************************/
void Display_Utils_GetDisplayCommandLine(CHAR8 **ppCmdLine, UINT32* pLen);


/****************************************************************************
*
** FUNCTION: Display_Utils_SetContinuousSplashInfo()
*/
/*!
* \brief
*   Saves the continuous splash address in Display context
*
* \param [in] uAddr - Splash buffer address
*        [in] uSize - Splash buffer size in bytes
*
* \retval None
*
****************************************************************************/
void Display_Utils_SetContinuousSplashInfo(UINT32 uAddr, UINT32 uSize);


/****************************************************************************
*
** FUNCTION: Display_Utils_GetContinuousSplashInfo()
*/
/*!
* \brief
*   Retrieves the continuous splash buffer info
*
* \param [out] pAddr - Splash buffer address
*        [out] pSize - Splash buffer size in bytes
*
* \retval EFI_STATUS
*
****************************************************************************/
EFI_STATUS Display_Utils_GetContinuousSplashInfo(UINT32 *pAddr, UINT32 *pSize);


/****************************************************************************
*
** FUNCTION: Display_Utils_StorePLLCodes()
*/
/*!
* \brief
*   Display Utils to store pll codes
*
  @param  None
*
* \retval None
*
****************************************************************************/
void Display_Utils_StorePLLCodes(void);
/****************************************************************************
*
** FUNCTION: Display_Utils_SetVariable()
*/
/*!
* \brief
*   Display Utils Protocol to set the variable value
*
  @param[in]  pVariableName              The variable name that need to set.
  @param[in]  pVariableValue             The variable value.
  @param[in]  uDataLength                The length of the variable value.
  @param[in]  uFlags                     The flag to set variable.
*
* \retval EFI_STATUS
*
****************************************************************************/
EFI_STATUS Display_Utils_SetVariable(
    CHAR16   *pVariableName,
    UINT8    *pVariableValue,
    UINTN     uDataLength,
    UINTN     uFlags);


/****************************************************************************
*
** FUNCTION: Display_Utils_GetVariable()
*/
/*!
* \brief
*   Display Utils Protocol to get the variable value
*
  @param[in]   pVariableName              The variable name that need to get.
  @param[out]  pVariableValue             The variable value.
  @param[out]  pDataLength                The length of the variable value.
  @param[in]   uFlags                     The flag to get variable.
*
* \retval EFI_STATUS
*
****************************************************************************/
EFI_STATUS Display_Utils_GetVariable(
    CHAR16   *pVariableName,
    UINT8    *pVariableValue,
    UINTN    *pDataLength,
    UINTN     uFlags);


#endif // __DISPLAYUTILS_H__

