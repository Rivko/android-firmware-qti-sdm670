/*==============================================================================

FILE:      DALInterruptController_utils.c

DESCRIPTION: This file implements a utility functions required by IntCont

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright (c) 2014 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1/systemdrivers/InterruptController/src/utils/DALInterruptController_utils.c#1 $ 

when       who     what, where, why
--------   ---     ---------------------------------------------------------
12/12/14   cpaulo  Initial version. 

===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "DALInterruptController_utils.h"

/*=========================================================================

                           TYPE DEFINITIONS

===========================================================================*/

/*=========================================================================

                           DATA DEFINITIONS

===========================================================================*/

/*=========================================================================

                           MACRO DEFINITIONS

===========================================================================*/


/*==========================================================================

                         FUNCTION  DECLARATIONS

==========================================================================*/


/*==========================================================================

                         FUNCTION  DEFINITIONS

==========================================================================*/

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
void InterruptControllerPushArg( InterruptArgsType* pArgs, InterruptArgsType** ppList )
{
  if ( pArgs != NULL && ppList != NULL )
  {
    pArgs->pNext = *ppList; 
    *ppList = pArgs;
  }
} /* InterruptControllerPushArg */


/*===========================================================================

  FUNCTION      InterruptControllerPopArg

  DESCRIPTION   This function pops an argument structure from the front of
                a list.

  PARAMETERS    InterruptArgsType** ppList - Pointer to a list.

  DEPENDENCIES  Must be called within a mutex.

  RETURN VALUE  InterruptArgsType* - Pointer to an argument, NULL if empty.

  SIDE EFFECTS  None.


==========================================================================*/
InterruptArgsType* InterruptControllerPopArg( InterruptArgsType** ppList )
{
  InterruptArgsType* pRet = NULL;

  if ( ppList != NULL )
  {
    pRet = *ppList;

    if ( pRet != NULL )
    {
      *ppList = pRet->pNext;
    }

    return pRet;
  }

  return NULL;

} /* InterruptControllerPushArg */


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
InterruptArgsType* InterruptControllerSearchArg( uint32 nInterruptVector, InterruptArgsType** ppList )
{
  InterruptArgsType* pRet =  *ppList;
  InterruptArgsType* pPrev = NULL;

  while ( pRet != NULL )
  {

    if ( pRet->nInterruptVector == nInterruptVector )
    {
      /*
       * Found, dequeue element
       */
      if ( pPrev == NULL )
      {
        /*
         * First element
         */
        *ppList = pRet->pNext;
      }
      else
      {
        /*
         * Not the first element
         */
        pPrev->pNext = pRet->pNext;
      }

      pRet->pNext = NULL;

      break;
    }

    /*
     * Not found yet, check next & update temp pointers
     */
    pPrev = pRet;
    pRet = pRet->pNext;

  }

  return pRet;
} /* InterruptControllerSearchArg */


