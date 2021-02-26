/*=============================================================================

File: MDPPlatformLibNull.c

Panel Specific configuration file. This file contains configuration for 
Qualcomm specific platforms. This file is not necessary to be modified by
the OEM, however it could serve as sample. The OEMPlatformLib.c file should
be used by the OEM to add their own platform configurations. If the PcdPanelType
is 1 (OEM) then the configuration in OEMPlatformLib.c will be used instead of 
the configuration found here.

     Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
     Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/EFIPmicGpio.h>
#include <Protocol/EFITlmm.h>
#include <Protocol/EFIPmicLpg.h>
#include <Protocol/EFIPmicMpp.h>
#include <Protocol/EFIPmicGpio.h>
#include <Protocol/EFIPmicVreg.h>
#include <Protocol/EFIPmicWled.h>
#include <Protocol/EFIPmicIbb.h>
#include <Protocol/EFIPmicLab.h>
#include "DDIChipInfo.h"
#include "MDPTypes.h"
#include "MDPPlatformLib.h"
#include "MDPSystem.h"
#include "OEMPlatformLib.h"
#include "HALDSILib.h"
#include "npa.h"
#include "pmapp_npa.h"

/* -----------------------------------------------------------------------
** Defines
** ----------------------------------------------------------------------- */
#define DSI_READ_ADDRESS_SIZE             2
#define DSI_READ_READBACK_SIZE            8

#define PLATFORM_PANEL_ID_MAX_COMMANDS    3       // maximum panel ID read commands
/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */

/* Sample convenience structure to hold the data necessary to do a DSI read
   during DSI auto-detection. See DynamicDSIPanelDetection.
*/

typedef struct {
    uint8      address[DSI_READ_ADDRESS_SIZE];                                        // DCS command for panel ID
    uint8      expectedReadback[DSI_READ_READBACK_SIZE];                              // expected readback
} PlatformPanelIDCommandInfo;


typedef struct {
  uint8                         uCmdType;                                             // data type for panel ID DSI read
  uint32                        uTotalRetry;                                          // number of retry if DSI read fails
  PlatformPanelIDCommandInfo    panelIdCommands[PLATFORM_PANEL_ID_MAX_COMMANDS];      // commands for panel ID and expected readback
  uint32                        uLaneRemapOrder;                                      // Lane remap order
  const int8                   *psPanelCfg;                                           // pointer to the panel configuration
  uint32                        uPanelCfgSize;                                        // size of panel configuration data
} PlatformDSIDetectParams;

typedef struct
{
  npa_client_handle             sNPAClient[MDP_DISPLAY_MAX];                          // NPA client handles for power control of panels.
  MDP_PmicModuleControlType     ePMICSecondaryPower[MDP_DISPLAY_MAX];                 // Configuration for PMIC based secondary power source 
} Panel_PowerCtrlParams;

/* -----------------------------------------------------------------------
** Local functions
** ----------------------------------------------------------------------- */

/* Platform detection 
*/
static MDP_Status ReadPlatformIDAndChipID(EFI_PLATFORMINFO_PLATFORM_INFO_TYPE *pPlatformInfo, EFIChipInfoIdType *pChipSetId, EFIChipInfoFamilyType *pChiSetFamily);
static MDP_Status DynamicDSIPanelDetection(MDPPlatformParams *pPlatformParams, uint32 *puPanelID);

/* 8996 Main Panel CDP/MTP Functions
*/
static MDP_Status Panel_CDP_PowerUp(MDP_Display_IDType eDisplayId, Panel_PowerCtrlParams *pPowerParams);
static MDP_Status Panel_CDP_PowerDown(MDP_Display_IDType eDisplayId, Panel_PowerCtrlParams *pPowerParams);
static MDP_Status Panel_CDP_Reset(void);
static MDP_Status Panel_CDP_BacklightLevel(MDP_Display_IDType eDisplayId, BacklightConfigType *pBacklightConfig);
static MDP_Status Panel_CDP_PeripheralPower(MDP_Display_IDType eDisplayId, Panel_PowerCtrlParams *pPowerParams, bool32 bPowerUp);


/* 8996 HDMI Panel Functions
*/
static MDP_Status HDMI_CDP_PowerUp(MDP_Display_IDType eDisplayId, Panel_PowerCtrlParams *pPowerParams);
static MDP_Status HDMI_CDP_PowerDown(MDP_Display_IDType eDisplayId, Panel_PowerCtrlParams *pPowerParams);

/* 8996 Common Power Init function
*/
static MDP_Status Panel_PowerInit(MDP_Display_IDType eDisplayId, Panel_PowerCtrlParams *pPowerParams);

/*===========================================================================
 Local Configuration Definitions
 ===========================================================================*/


/*===========================================================================
Function Definitions
===========================================================================*/


/* ---------------------------------------------------------------------- */
/**
** FUNCTION: panel_ConfigGPIO()

** 
** DESCRIPTION:
**   Initialize LCD panel GPIOs and PMICs
**
*//* -------------------------------------------------------------------- */
MDP_Status MDPPlatformConfigure(MDP_Display_IDType eDisplayId, MDPPlatformConfigType eConfig, MDPPlatformParams *pPlatformParams)
{
  MDP_Status  eStatus  = MDP_STATUS_OK;
  return eStatus;
}



/*===========================================================================
Private Function Definitions
===========================================================================*/

/* ---------------------------------------------------------------------- */
/**
** FUNCTION: ReadPlatformIDAndChipID()
** 
** DESCRIPTION:
**
*//* -------------------------------------------------------------------- */
static MDP_Status ReadPlatformIDAndChipID(EFI_PLATFORMINFO_PLATFORM_INFO_TYPE *pPlatformInfo, EFIChipInfoIdType *pChipSetId, EFIChipInfoFamilyType *pChipSetFamily)
{
  MDP_Status                           eStatus                = MDP_STATUS_OK;
  return eStatus;
}

/* ---------------------------------------------------------------------- */
/**
** FUNCTION: Panel_PowerInit()
** 
** DESCRIPTION:
**
*//* -------------------------------------------------------------------- */
static MDP_Status Panel_PowerInit(MDP_Display_IDType eDisplayId, Panel_PowerCtrlParams *pPowerParams)
{
  MDP_Status    Status = MDP_STATUS_OK;
  return Status;
}

/* ---------------------------------------------------------------------- */
/**
** FUNCTION: Panel_CDP_PowerUp()
** 
** DESCRIPTION:
**
*//* -------------------------------------------------------------------- */
static MDP_Status Panel_CDP_PowerUp(MDP_Display_IDType eDisplayId, Panel_PowerCtrlParams *pPowerParams)
{
  MDP_Status                    Status = MDP_STATUS_OK;
  return Status;
}  


/* ---------------------------------------------------------------------- */
/**
** FUNCTION: Panel_CDP_PowerDown()
** 
** DESCRIPTION:
**
*//* -------------------------------------------------------------------- */
static MDP_Status Panel_CDP_PowerDown(MDP_Display_IDType eDisplayId, Panel_PowerCtrlParams *pPowerParams)
{
  MDP_Status                    Status           = MDP_STATUS_OK;
  return Status;
}  


/* ---------------------------------------------------------------------- */
/**
** FUNCTION: Panel_CDP_Reset()
** 
** DESCRIPTION:
**
*//* -------------------------------------------------------------------- */
MDP_Status Panel_CDP_Reset(void)
{
  MDP_Status                    Status           = MDP_STATUS_OK;
  return Status;
}

/* ---------------------------------------------------------------------- */
/**
** FUNCTION: Panel_CDP_PeripheralPower()
**
** DESCRIPTION:
**        Secondary power seqeunce for other PMIC modules such as IBB/LAB
**
*//* -------------------------------------------------------------------- */
static MDP_Status Panel_CDP_PeripheralPower(MDP_Display_IDType eDisplayId, Panel_PowerCtrlParams *pPowerParams, bool32 bPowerUp)
{
  MDP_Status                    Status = MDP_STATUS_OK;
  return Status;
}


/* ---------------------------------------------------------------------- */
/**
** FUNCTION: Panel_CDP_BacklightLevel()
**
** DESCRIPTION:
**
*//* -------------------------------------------------------------------- */
static MDP_Status Panel_CDP_BacklightLevel(MDP_Display_IDType eDisplayId, BacklightConfigType *pBacklightConfig)
{
  MDP_Status                    Status = MDP_STATUS_OK;
  return Status;
}



/* ---------------------------------------------------------------------- */
/**
** FUNCTION: HDMI_CDP_Powerup()
** 
** DESCRIPTION:
**  
**
*//* -------------------------------------------------------------------- */
static MDP_Status HDMI_CDP_PowerUp(MDP_Display_IDType eDisplayId, Panel_PowerCtrlParams *pPowerParams)
{
  MDP_Status                    Status            = MDP_STATUS_OK;
  return Status;
}



/* ---------------------------------------------------------------------- */
/**
** FUNCTION: HDMI_CDP_PowerDown()
** 
** DESCRIPTION:
** 
**
*//* -------------------------------------------------------------------- */
static MDP_Status HDMI_CDP_PowerDown(MDP_Display_IDType eDisplayId, Panel_PowerCtrlParams *pPowerParams)
{
  MDP_Status                    Status            = MDP_STATUS_OK;
  return Status;
}


/* ---------------------------------------------------------------------- */
/**
** FUNCTION: DynamicDSIPanelDetection()
** 
** DESCRIPTION:
**  Detect DSI panels by doing a DSI read specific to each panels.
**  This function could be used as sample for OEM to detect DSI panels, 
**  it is not a complete implementation of all possible combinations of read
**  commands that could be needed for this detection.
**   
**  Return success only if a DSI panel was correctly detected and the information 
**  is updated in pPlatformParams->sPlatformPanel
*//* -------------------------------------------------------------------- */
static MDP_Status DynamicDSIPanelDetection(MDPPlatformParams *pPlatformParams, uint32 *puPanelID)
{
  MDP_Status             Status       = MDP_STATUS_OK;
  return Status;
}


#ifdef __cplusplus
}
#endif
