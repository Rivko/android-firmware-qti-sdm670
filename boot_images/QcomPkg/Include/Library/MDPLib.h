#ifndef __MDPLIB_H__
#define __MDPLIB_H__
/*=============================================================================
 
  File: MDPLib.h
 
  Header file for common MDP driver functions
  
 
  Copyright (c) 2011-2016, 2018 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <Uefi.h>
#include "MDPTypes.h"

/*===========================================================================

                           Defines

===========================================================================*/

/* Maximum number of display modes supported by a single display */
#define MDP_DISPLAY_MAX_MODES                     16

/* Driver profiling */ 
#ifdef MDP_ENABLE_PROFILING    
  #define MDP_LOG_FUNC_ENTRY(pFuncName, param1)      MDPProfiler((uint8*)pFuncName, param1, TRUE)     
  #define MDP_LOG_FUNC_EXIT(pFuncName)               MDPProfiler((uint8*)pFuncName, 0x00, FALSE)
#else 
  #define MDP_LOG_FUNC_ENTRY(pFuncName, param1)   
  #define MDP_LOG_FUNC_EXIT(pFuncName)   
#endif                


/*===========================================================================

                          Enumerations

===========================================================================*/
typedef enum {
  MDP_DISPLAY_PROPERTY_BACKLIGHT = 0,             /* Backlight level adjustments */
  MDP_DISPLAY_PROPERTY_TRIGGER_UPDATE,            /* Trigger a DSI transfer (smart panels) */
  MDP_DISPLAY_PROPERTY_FIRMWAREENV,               /* Populate the firmware system environment table */
  MDP_DISPLAY_PROPERTY_MODE_INFO,                 /* Populates the Active timing Information */
  MDP_DISPLAY_PROPERTY_POWER_STATE,               /* Display power state on/off */  
  MDP_DISPLAY_PROPERTY_DETECTION_INFO,            /* Display panel detection information */  
  MDP_DISPLAY_PROPERTY_ABL_INTERFACE_INIT,        /* Initialize display data partition & ABL interface */
  MDP_DISPLAY_PROPERTY_MAX,
  MDP_DISPLAY_PROPERTY_FORCE_32BIT = 0x7FFFFFFF
} MDP_Display_Property;




/*===========================================================================

                        Common Structures

===========================================================================*/



/*===========================================================================

                          API Structures

===========================================================================*/


/* 
 * MDPInit Parameters
 */
typedef struct
{
  uint32  uMDPVersionMajor;                        /* MDP Major version */
  uint32  uMDPVersionMinor;                        /* MDP Minor version */
  uint32  uMDPVersionRevision;                     /* MDP Revision */

  bool32  aSupportedDisplays[MDP_DISPLAY_MAX];     /* List of display supported on this platform */
  
} MDP_InitParamsType;



/* 
 * MDPPower Parameters
 */
typedef struct
{
  bool32  bPowerOn;                                /* Power up this display, FALSE=power down */
  
} MDP_PowerParamsType;


/* 
 * MDP Mode structure
 */
typedef struct
{
  uint32  uModeIndex;                             /* Mode index */
  uint32  uWidth;                                 /* Mode width in pixels */
  uint32  uHeight;                                /* Mode heigth in pixels */
  uint32  uRefreshRate;                           /* Mode refresh rate in Q16.16 */
  bool32  bInterlaced;                            /* Mode is interlaced */
} MDP_ModeInfo;


/* 
 * MDPDetect Parameters
 */
typedef struct
{
  uint32       bDisplayDetected;                   /* Indicates if the display was detected */
  uint32       uSupportedModes;                    /* List the number of supported modes */
  MDP_ModeInfo aModeList[MDP_DISPLAY_MAX_MODES];   /* List of support display modes */
} MDP_DetectParamType;



/* 
 * MDPSetMode Parameters
 */
typedef struct
{
  uint32          uModeIndex;                      /* Mode index to select */
  MDPSurfaceInfo  sSurfaceInfo;                    /* Source surface */
} MDP_SetModeParamType;


/* 
 * MDPProperty Parameters
 */
typedef union
{
  uint32                uBacklightLevel;           /* MDP_DISPLAY_PROPERTY_BACKLIGHT */
  bool32                bDisplayPwrState;          /* MDP_DISPLAY_PROPERTY_POWER_STATE */
  bool32                bDisplayDetected;          /* MDP_DISPLAY_PROPERTY_DETECTION_INFO */ 
  MDP_SetModeParamType  sModeParams;               /* MDP_DISPLAY_PROPERTY_MODE_INFO */
} MDP_PropertiesParamType;


/* MDPResolution Parameters
 */
typedef struct 
{
  uint32   uWidth;                                 /* Width in px */
  uint32   uHeight;                                /* Height in lines */
} MDP_ResolutionType;


/*===========================================================================

                        Public Functions

===========================================================================*/

/* Mininal init flag does minimal intialization of mdp which is required when we resume from power collapse */
#define MDP_INIT_FLAG_MINIMAL_INIT                   0x00000001
#define MDP_INIT_FLAG_MMU_INIT                       0x00000002

/* Partition handles are invalidated. Signal MDP to avoid using saved handles. Passed as a flag to MDPExitBoot call*/
#define MDP_EXIT_FLAG_INVALIDATE_GPT                 0x00000001

/****************************************************************************
*
** FUNCTION: MDPInit()
*/
/*!
* \brief
*   This function will initialize the MDP core and report information about the core.
*
* \param [out] pMDPInitParams   - Information regarding the hardware core
*        [in]  uFlags           - Reserved
*
* \retval MDP_Status
*
****************************************************************************/
MDP_Status  MDPInit(MDP_InitParamsType *pMDPInitParams, uint32 uFlags);


/****************************************************************************
*
** FUNCTION: MDPAcquireLockOrFail()
*/
/*!
* \brief
*   This function will try to get the specified lock.
*
* The function will acquire the lock if not already acquired
* It will return FAIL if lock is already acquired 
*
* \param  eLock              - Lock to acquire 
*
* \retval MDP_STATUS_OK      - Lock acquired successfully
* \retval MDP_STATUS_FAILED  - Failed to acquire Lock 
*
****************************************************************************/
MDP_Status  MDPAcquireLockOrFail(MDP_Lock_Type eLock);


/****************************************************************************
*
** FUNCTION: MDPReleaseLock()
*/
/*!
* \brief
*   This function will release the specified lock.
*
* \param  eLock          - Lock to release 
*
* \retval MDP_STATUS_OK  - Lock released successfully
*
****************************************************************************/
MDP_Status  MDPReleaseLock(MDP_Lock_Type eLock);


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
MDP_Status  MDPPower(MDP_Display_IDType eDisplayId, MDP_PowerParamsType *pMDPPowerParams, uint32 uFlags);


/****************************************************************************
*
** FUNCTION: MDPDetect()
*/
/*!
* \brief
*   This function will detect the presense of a display and supported modes.
*
* \param [in] eDisplayId       - The display to initialize
*        [in] pMDPDetectParams - Information regarding the hardware core
*        [in] uFlags           - Reserved
*
* \retval MDP_Status
*
****************************************************************************/
MDP_Status  MDPDetect(MDP_Display_IDType eDisplayId, MDP_DetectParamType *pMDPDetectParams, uint32 uFlags );



/****************************************************************************
*
** FUNCTION: MDPSetMode()
*/
/*!
* \brief
*   This function setup the display for a particular mode.
*
* \param [in] eDisplayId        - The display to initialize
*        [in] pMDPSetModeParams - Mode setup parameters
*        [in] uFlags            - Reserved
*
* \retval MDP_Status
*
****************************************************************************/
MDP_Status  MDPSetMode(MDP_Display_IDType eDisplayId, MDP_SetModeParamType *pMDPSetModeParams, uint32 uFlags );


/****************************************************************************
*
** FUNCTION: MDPSetProperty()
*/
/*!
* \brief
*   This function will configure a specific property of the display
*
* \param [in] eDisplayId        - The display to initialize
*        [in] eProperty         - The particular property to set
*        [in] pMDPSetModeParams - Mode setup parameters
*
* \retval MDP_Status
*
****************************************************************************/
MDP_Status  MDPSetProperty(MDP_Display_IDType eDisplayId, MDP_Display_Property eProperty, MDP_PropertiesParamType *pMDPPropertiesParams);


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
MDP_Status  MDPGetProperty(MDP_Display_IDType eDisplayId, MDP_Display_Property eProperty, MDP_PropertiesParamType *pMDPPropertiesParams);


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
MDP_Status  MDPDeInit(MDP_Display_IDType eDisplayId, uint32 uFlags);


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
MDP_Status  MDPSetCoreClock(uint32 uFlags);


/****************************************************************************
*
** FUNCTION: MDPTerm()
*/
/*!
* \brief
*   This function will deinitialize the MDP core and free all resoruces.
*
* \param [in]  uFlags           - Reserved
*
* \retval MDP_Status
*
****************************************************************************/
MDP_Status  MDPTerm(uint32 uFlags);


/****************************************************************************
*
** FUNCTION: MDPProfiler()
*/
/*!
* \brief
*   This function performs the profiling of functions 
*
* \param [in]  pFuncName    - Function name
* \param [in]  uParam1      - Display ID, Property or Mode info depending on the function 
* \param [in]  bEntry       - Function entry or exit. True - entry; False - exit;
*
* \retval MDP_Status
*
****************************************************************************/
MDP_Status  MDPProfiler(uint8 *pFuncName, uint32 uParam1, bool32 bEntry);


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
MDP_Status  MDPExitBoot(uint32 uFlags);

#endif // __MDPLIB_H__

