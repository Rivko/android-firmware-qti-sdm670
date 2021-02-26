/*============================================================================

FILE:      uIPCIntStub.c

DESCRIPTION:
  This file implements the portion of the uImage IPC Interrupt Driver
  that triggers interrupts from uImage.

PUBLIC CLASSES:
  uIPCInt_Trigger
  uIPCInt_IsSupported

============================================================================
     Copyright (c) 2017 QUALCOMM Technologies, Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================
  $Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/ipcint/src/uIPCIntStub.c#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $
============================================================================*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "uIPCInt.h"
#include "uIPCIntInternal.h"


/*=========================================================================
      Internal Data
==========================================================================*/


/*=========================================================================
      Implementation
==========================================================================*/

/*==========================================================================

  FUNCTION      uIPCInt_Init

  DESCRIPTION   See uIPCIntIntenral.h

==========================================================================*/
void uIPCInt_Init ( void )
{
}

/*==========================================================================

  FUNCTION      uIPCInt_Trigger

  DESCRIPTION   See uIPCInt.h

==========================================================================*/

int32 uIPCInt_Trigger
(
  DalIPCIntProcessorType  eTarget,
  DalIPCIntInterruptType  eInterrupt
) 
{
  return UIPCINT_ERROR;
} /* uIPCInt_Trigger */


/*==========================================================================

  FUNCTION      IPCInt_IsSupported

  DESCRIPTION   See DDIIPCInt.h

==========================================================================*/

int32 uIPCInt_IsSupported
(
  DalIPCIntProcessorType  eTarget,
  DalIPCIntInterruptType  eInterrupt
) 
{
  return UIPCINT_ERROR;
} /* uIPCInt_IsSupported */



