/*
===========================================================================
*/
/**
  @file ClockMSSLite.c
  
  This is a lightweight, non-DAL, implementation for clocks that must
  be available prior to the DAL clock driver being initialized.
*/
/*  
  ====================================================================

  Copyright (c) 2017 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential. 

  ==================================================================== 
  $Header: //components/rel/core.qdsp6/2.1/systemdrivers/clock/image/slpi/hw/stub/ClockSSCLite.c#1 $
  $DateTime: 2017/07/21 22:10:47 $
  $Author: pwbldsvc $
  ====================================================================
*/ 


/*=========================================================================
      Include Files
==========================================================================*/

#include "ClockDriver.h"


/*=========================================================================
      Functions
==========================================================================*/


/* =========================================================================
**  Function : Clock_EnablePRNG
** =========================================================================*/

DALResult Clock_EnablePRNG(void)
{
  return DAL_SUCCESS;

} /* END Clock_EnablePRNG */


/* =========================================================================
**  Function : Clock_DisablePRNG
** =========================================================================*/

DALResult Clock_DisablePRNG(void)
{
  return DAL_SUCCESS;

} /* END Clock_TransferReferenceCount */


/* =========================================================================
**  Function : Clock_TransferReferenceCount
** =========================================================================*/

DALResult Clock_TransferReferenceCount(void)
{
  return DAL_SUCCESS;

} /* END Clock_TransferReferenceCount */

