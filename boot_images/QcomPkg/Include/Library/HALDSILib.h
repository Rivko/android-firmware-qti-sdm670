#ifndef HALDSILIB_H
#define HALDSILIB_H
/*=============================================================================
 
  File: HALDSILib.h
 
  Header file for common DSI driver HAL functions
  
 
  Copyright (c) 2011-2013, 2017-2018 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/
/*============================================================================
*                         INCLUDE FILES
============================================================================*/
#include "../../Library/MDPLib/MDPLib_i.h"


/* -----------------------------------------------------------------------
** Macros and Defines
-------------------------------------------------------------------------- */

/* Recommended values for DSI PLL frequency
 */
typedef struct 
{
   uint32   uBitClock;    // Bitclock frequency Mhz (desired)
   uint8    uPLLDivRef;   // DSI_DSIPHY_PLL_CTRL_3.DSIPLL_DIV_REF
   uint8    uPLL_CP;      // DSI_DSIPHY_PLL_CTRL_7.DSIPLL_CP
   uint8    uPLL_LF_R;    // DSI_DSIPHY_PLL_CTRL_5.DSIPLL_LF_R
   uint8    uPLL_LF_C1;   // DSI1_DSIPHY_PLL_CTRL_6.DSIPLL_LF_C1
   uint8    uPLL_LF_C2;   // DSI1_DSIPHY_PLL_CTRL_6.DSIPLL_LF_C2
   uint32   uPLLDivFb;    // DSI1_DSIPHY_PLL_CTRL_1.DSIPLL_DIV_FB_7_0/DSIPLL_DIV_FB_10_8
   uint8    uPLLOutDiv1;  // DSI1_DSIPHY_PLL_CTRL_8.DSIPLL_OUT_DIV1
} DSIPllTableEntry;


/* -----------------------------------------------------------------------
** DSI Driver Function Prototypes
-------------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- */
/**
** FUNCTION: DSIDriver_Init()
** 
** DESCRIPTION:
**   Initialize the DSI Core
**
*//* -------------------------------------------------------------------- */
MDP_Status DSIDriver_Init(MDP_Panel_AttrType *pPanelConfig);


/* ---------------------------------------------------------------------- */
/**
** FUNCTION: DSIDriver_Read()
**
** DESCRIPTION:
**   This allows to send the dsi read commands to the panel.
**
**
** -------------------------------------------------------------------- */
MDP_Status DSIDriver_Read(uint8 uCmdType, void *pPayload, uint32 uPayloadSize, void *pReadData, uint32 *pReadSize);


/* ---------------------------------------------------------------------- */
/**
** FUNCTION   : DSIDriver_Write()
**
** PARAMETERS :
**   uCmdType     - DSI Type used to send commands.
**   pPayload     - Context for the DSI commands to write.
**   uPayloadSize - Size for the DSI commands to write.
**
** DESCRIPTION:
**   This allows to send the dsi write commands to the panel.
**
**
** -------------------------------------------------------------------- */
MDP_Status DSIDriver_Write(uint8 uCmdType, void *pPayload, uint32 uPayloadSize);


/* ---------------------------------------------------------------------- */
/**
** FUNCTION: DSIDriver_Close()
**
** DESCRIPTION:
**   Deinitialize the DSI Core
**
*//* -------------------------------------------------------------------- */
void DSIDriver_Close(void);


/* ---------------------------------------------------------------------- */
/**
** FUNCTION: DSIDriver_SetMode()
** 
** DESCRIPTION:
**   Set a panel mode
**
*//* -------------------------------------------------------------------- */
MDP_Status DSIDriver_SetMode(MDP_Panel_AttrType *pPanelConfig);


/* ---------------------------------------------------------------------- */
/**
** FUNCTION: DSIDriver_MinimalInit()
** 
** DESCRIPTION:
**   Minimal initialization needed to do DSI read commands
**
** ---------------------------------------------------------------------- */
MDP_Status DSIDriver_MinimalInit(void);

/* ---------------------------------------------------------------------- */
/**
** FUNCTION: DSIDriver_GetCalibrationCodes()
**
** DESCRIPTION:
**   Get dynamic refresh calibration codes
**
** ---------------------------------------------------------------------- */
MDP_Status DSIDriver_GetCalibrationCodes(uint32 uDSIIndex, MDP_PLLCodesInfo *psPllCodesInfo);

/* ---------------------------------------------------------------------- */
/**
** FUNCTION: DSIDriver_RemapLane()
** 
** DESCRIPTION:
**   Remap lane order
**
** ---------------------------------------------------------------------- */
MDP_Status DSIDriver_RemapLane(uint32 uLaneRemapOrder);

/* ---------------------------------------------------------------------- */
/**
** FUNCTION: DSIDriver_ConfigClockLane()
** 
** DESCRIPTION:
**   Config the clock lane to HS or LP
**
** ---------------------------------------------------------------------- */

MDP_Status DSIDriver_ConfigClockLane(uint32 uClkConfig);


/* -----------------------------------------------------------------------
** Panel Driver Helper Function Prototypes
-------------------------------------------------------------------------- */

/* ---------------------------------------------------------------------- */
/**
** FUNCTION: DsiPanelInit()
** 
** DESCRIPTION:
**   DSI Clocks and timing initialization
**
** ---------------------------------------------------------------------- */
MDP_Status DsiPanelInit(MDP_Panel_AttrType    *pPanelInfo);



/* ---------------------------------------------------------------------- */
/**
** FUNCTION: DsiPanelSendCommandSequence()
** 
** DESCRIPTION:
**   Send DSI command sequence
**
** ---------------------------------------------------------------------- */
MDP_Status DsiPanelSendCommandSequence(MDP_Panel_AttrType *pPanelInfo, MdpPanelCommandType eCommand, void *pCustomCommand, uint32 uFlags);


/* ---------------------------------------------------------------------- */
/**
** FUNCTION: DsiPanelShutdown()
** 
** DESCRIPTION:
**   Graceful shutdown of the panel and controller
**
** ---------------------------------------------------------------------- */
MDP_Status DsiPanelShutdown(MDP_Panel_AttrType    *pPanelInfo);


/* ---------------------------------------------------------------------- */
/**
** FUNCTION: DsiPanelDumpRegisters()
** 
** DESCRIPTION: Helper function to dump panel registers
** 
**
*//* -------------------------------------------------------------------- */
MDP_Status DsiPanelDumpRegisters(void);


#endif  /* #define HALDSILIB_H */
