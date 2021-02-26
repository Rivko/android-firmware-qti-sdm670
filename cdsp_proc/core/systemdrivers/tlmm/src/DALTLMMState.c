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

$Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/tlmm/src/DALTLMMState.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
07/30/10   dcf     Created for restructuring of DALTLMM driver.

============================================================================
            Copyright (c) 2010-2016 QUALCOMM Technologies Incorporated.
                         All Rights Reserved.
                       QUALCOMM Proprietary/GTDR
============================================================================
*/


/*==========================================================================

                        INCLUDE FILES FOR MODULE

==========================================================================*/

#include <HALtlmm.h>
#include "DALTLMMState.h"




/*==========================================================================

                          EXPORTED STRUCTURES 

==========================================================================*/



/*==========================================================================

                     APPLICATION PROGRAMMING INTERFACE

==========================================================================*/


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

