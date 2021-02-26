#ifndef AONINTRETENTION_INTERNAL_H
#define AONINTRETENTION_INTERNAL_H
/*
===========================================================================
*/
/**
  @file AonInt_Internal.h 
  
  The file contains the function prototypes definitions for retention 
  voting on the Sensors Low Power Island subsystem.
*/
/*  
  ====================================================================

  Copyright (c) 2014 QUALCOMM Technologies Incorporated.  All Rights Reserved.  
  QUALCOMM Proprietary and Confidential. 

  ==================================================================== 
  $Header: //components/rel/core.qdsp6/2.1/systemdrivers/InterruptController/inc/AonInt_Internal.h#1 $
  $DateTime: 2017/07/21 22:10:47 $
  $Author: pwbldsvc $

  when       who     what, where, why
  --------   ---     -------------------------------------------------
  09/25/14   dcf     Created.

  ====================================================================
*/ 


/*=========================================================================
      Include Files
==========================================================================*/
#include <DALSys.h>
#include "AonInt.h"

/*=========================================================================
      Defines
==========================================================================*/

/*
 * Values used for making NPA requests.
 *  0: Retention is not required at all by any client.
 *  1: Retention is required by one or more clients.
 */
#define AONINT_SOURCE_NOT_REQUIRED     0
#define AONINT_SOURCE_REQUIRED         1

/*
 * Value used for making NPA requests for Island Mode.
 *  1: Vote for Island Mode entry.
 */
#define AONINT_VOTE_FOR_ISLAND_MODE    1


/*=========================================================================
      Function Definitions
==========================================================================*/

/* =========================================================================
**  Function : AonInt_Init
** =========================================================================*/
/**
  Initialize AonInt.
 
  This function will initialize the NPA nodes along with the LPR definition.
 
  @return
  None.

  @dependencies
  DAL_SUCCESS.
*/ 
DALResult AonInt_Init( void );


#endif /* AONINTRETENTION_INTERNAL_H */


