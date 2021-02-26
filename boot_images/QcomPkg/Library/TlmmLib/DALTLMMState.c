/*
================================================================================

                    D E V I C E   A C C E S S   L A Y E R
             T O P   L E V E L   M O D E   M U L T I P L E X O R
                S T A T E   I N F O R M A T I O N   A N D
                            A C C E S S   F I L E

FILE:         DALTLMMState.c

DESCRIPTION:  
              This file contains declarations and initializations of the
              runtime state structure for GPIO pins in the DALTLMM driver.

============================================================================


                             Edit History


when       who     what, where, why
--------   ---     ---------------------------------------------------------
07/30/10   dcf     Created for restructuring of DALTLMM driver.

============================================================================
                  Copyright (c) 2010 - 2013 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
                         All Rights Reserved.
                       QUALCOMM Proprietary/GTDR
============================================================================
*/


/*==========================================================================

                        INCLUDE FILES FOR MODULE

==========================================================================*/

#include <HALtlmm.h>
#include <DALFramework.h>
#include <DALSys.h>
#include <DDITlmm.h>
#include <DALTLMMPropDef.h>
#include <DDIChipInfo.h>
#include "DALTLMMState.h"
#include "DALTLMM.h"




/*==========================================================================

                          EXPORTED STRUCTURES 

==========================================================================*/


/* 
 * Main state structure used to contain all configuration and attribute 
 * information pertaining to the set of GPIO on a particular target.  This 
 * structure is initialized and used in shared memory. 
 */
DALTLMMStateType* pgtState;

/*
 * Global variable for retrieving the total supported GPIO on this target.
 */
extern uint32 gnTotalGPIO;


extern uint32* pgLowPowerConfigs;

extern boolean bGpioInUse[200];

/*==========================================================================

                     APPLICATION PROGRAMMING INTERFACE

==========================================================================*/


/*==========================================================================

  FUNCTION      DALTLMMState_Init

  DESCRIPTION   See DALTLMMState.h

==========================================================================*/ 

void DALTLMMState_Init(TLMMClientCtxt* pCtxt)
{
  static DALSYSPropertyVar tPropVar;
  DALResult eResult = DAL_ERROR;
  uint32 nGpioNumber;
  uint32 nPropField = 0;
  uint32 nPropConfig = 0;

  if( pgtState )
  {
    /* Attempt to retrieve the low-power configurations. */

    if( DalChipInfo_ChipFamily() == DALCHIPINFO_FAMILY_MSM8926 )
    {
      /* Special handling for MSM8926 configuration compatiblity with MSM8x26. */
      eResult = DALSYS_GetPropertyValue( pCtxt->pDevCtxt->hProp, "tlmm_sleep8926", 0, &tPropVar );
    }
    else
    {
      eResult = DALSYS_GetPropertyValue( pCtxt->pDevCtxt->hProp, "tlmm_sleep", 0, &tPropVar );
    }

    if( DAL_SUCCESS == eResult )
    {
      pgLowPowerConfigs = (uint32*)tPropVar.Val.pdwVal;

      /* Set the low-power configurations to the state table. */
      for( nGpioNumber = 0; nGpioNumber < gnTotalGPIO; ++nGpioNumber )
      {
        /* Fill in low-power configurations. */
        pgtState->ptGPIO[nGpioNumber].tInactive.nFunc = 0;
        pgtState->ptGPIO[nGpioNumber].tInactive.nDrive = (uint8)DAL_GPIO_2MA;

        nPropConfig = tPropVar.Val.pdwVal[nPropField++];

        pgtState->ptGPIO[nGpioNumber].tInactive.nDir = DAL_GPIO_DIRECTION(nPropConfig);
        pgtState->ptGPIO[nGpioNumber].tInactive.nPull = DAL_GPIO_PULL(nPropConfig);
        pgtState->ptGPIO[nGpioNumber].tAttr.nSleepVal = DAL_GPIO_GET_OUTVAL(nPropConfig);

        /* Ensure initialization should take place on this GPIO. */
        if( (nPropConfig & DALTLMM_PRG_YES) && (bGpioInUse[nGpioNumber] == FALSE) )
        {
          /* Program the low-power configuration to hardware. */
          HAL_tlmm_ConfigGpio((uint32)DALTLMMState_StructToMacro(nGpioNumber,
                              &pgtState->ptGPIO[nGpioNumber].tInactive));

          /* If OUTPUT HIGH, set GPIO as such in HW. */
          if( pgtState->ptGPIO[nGpioNumber].tAttr.nSleepVal )
          {
            HAL_tlmm_WriteGpio((uint32)DALTLMMState_StructToMacro(nGpioNumber,
                              &pgtState->ptGPIO[nGpioNumber].tInactive),
                               (DALBOOL)pgtState->ptGPIO[nGpioNumber].tAttr.nSleepVal);
          }
        }
		HAL_tlmm_SetLowPowerConfig(nGpioNumber, (uint32)(DALTLMMState_StructToMacroLp(nGpioNumber,
                                                        &pgtState->ptGPIO[nGpioNumber].tInactive,
														pgtState->ptGPIO[nGpioNumber].tAttr.nSleepVal)));
      }
      pgtState->tFlags.bDriverInited = TRUE;
    }
  }

} /* DALTLMMState_Init */


/*==========================================================================

  FUNCTION      DALTLMMState_StructToMacro

  DESCRIPTION   See DALTLMMState.h

==========================================================================*/ 

DALGpioSignalType DALTLMMState_StructToMacro
(
  uint32             nGpioNumber,
  HAL_tlmm_GpioType* ptState
)
{
  return( (DALGpioSignalType)DAL_GPIO_CFG(nGpioNumber,
                                          ptState->nFunc,
                                          ptState->nDir,
                                          ptState->nPull,
                                          ptState->nDrive) );

} /* DALTLMMState_StructToMacro */

DALGpioSignalType DALTLMMState_StructToMacroLp
(
  uint32 nGpioNumber,
  HAL_tlmm_GpioType* ptState,
  uint32             nOutVal
)
{
	  return( (DALGpioSignalType)DAL_GPIO_CFG_OUT(nGpioNumber,
                                          ptState->nFunc,
                                          ptState->nDir,
                                          ptState->nPull,
                                          ptState->nDrive,
										  nOutVal) );
}

/*==========================================================================

  FUNCTION      DALTLMMState_MacroToStruct

  DESCRIPTION   See DALTLMMState.h

==========================================================================*/ 

void DALTLMMState_MacroToStruct
(
  DALGpioSignalType  eGpioConfig,
  HAL_tlmm_GpioType* ptState
)
{
  ptState->nFunc = (uint8)HAL_FUNC_VAL(eGpioConfig);
  ptState->nDir = (uint8)HAL_DIR_VAL(eGpioConfig);
  ptState->nPull = (uint8)HAL_PULL_VAL(eGpioConfig);
  ptState->nDrive = (uint8)HAL_DRVSTR_VAL(eGpioConfig);

} /* DALTLMMState_MacroToStruct */
