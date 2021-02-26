#ifndef DALInterruptControllerUtils_H
#define DALInterruptControllerUtils_H
/*==============================================================================

FILE:      DalInterruptController_utils.h

DESCRIPTION: Function and data structure declarations

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright © 2014 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include <DALSys.h>
#include "qurt.h"

 /*
  * InterruptArgsType
  * nInterruptVector    - Interrupt vector required for .
  * pRetSig             - Interrupt Controller platform configuration data.
  */
typedef struct InterruptArgsStruct
{
  uint32                      nInterruptVector;
  qurt_anysignal_t            pRetSig;
  struct InterruptArgsStruct *pNext;
} InterruptArgsType;


/*===========================================================================

  FUNCTION      InterruptControllerPushArg

  DESCRIPTION   This function pushes an argument structure onto the front of
                a list.

  PARAMETERS   
  pArgs : Pointer to an arguments structure.

  DEPENDENCIES  Must be called within a mutex.

  RETURN VALUE  None.

  SIDE EFFECTS  None.


==========================================================================*/
void InterruptControllerPushArg( InterruptArgsType* pArgs, InterruptArgsType** ppList );


/*===========================================================================

  FUNCTION      InterruptControllerPopArg

  DESCRIPTION   This function pops an argument structure from the front of
                a list.

  PARAMETERS    InterruptArgsType** ppList - Pointer to a list.

  DEPENDENCIES  Must be called within a mutex.

  RETURN VALUE  InterruptArgsType* - Pointer to an argument, NULL if empty.

  SIDE EFFECTS  None.


==========================================================================*/
InterruptArgsType* InterruptControllerPopArg( InterruptArgsType** ppList );


/*===========================================================================

  FUNCTION      InterruptControllerSearchArg

  DESCRIPTION   This function finds an argument structure from
                a list.

  PARAMETERS   
  nInterruptVector : Interrupt Vector to find args list for.

  DEPENDENCIES  Must be called within a mutex.

  RETURN VALUE  Pointer to the argument structure.  NULL if not found.

  SIDE EFFECTS  None.


==========================================================================*/
InterruptArgsType* InterruptControllerSearchArg( uint32 nInterruptVector, InterruptArgsType** ppList );



#endif /*DALInterruptControllerUtils_H*/


