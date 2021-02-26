#ifndef AONINTRETENTION_H
#define AONINTRETENTION_H
/*
===========================================================================
*/
/**
  @file AonInt.h 
  
  The file contains the function prototypes definitions for retention 
  voting on the Sensors Low Power Island subsystem.
*/
/*  
  ====================================================================

  Copyright (c) 2014 QUALCOMM Technologies Incorporated.  All Rights Reserved.  
  QUALCOMM Proprietary and Confidential. 

  ==================================================================== 
  $Header: //components/rel/core.qdsp6/2.1.c4/api/systemdrivers/AonInt.h#1 $
  $DateTime: 2018/07/30 01:21:36 $
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


/*=========================================================================
      Function Definitions
==========================================================================*/

/* =========================================================================
**  Function : AonInt_RetentionEnter
** =========================================================================*/
/**
  Enter retention mode.
 
  This function is called to enter retention mode.
 
  @return
  None.

  @dependencies
  None.
*/ 
void AonInt_RetentionEnter( void );


/* =========================================================================
**  Function : AonInt_RetentionExit
** =========================================================================*/
/**
  Exit retention mode.
 
  This function is called to exit retention mode.
 
  @return
  None.

  @dependencies
  None.
*/ 
void AonInt_RetentionExit( void );

#endif /* AONINTRETENTION_H */


