/*
===========================================================================
*/
/**
  @file VCSTarget.c

*/
/*
  ====================================================================

  Copyright (c) 2016 QUALCOMM Technologies Incorporated. All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================
  $Header: //components/rel/core.qdsp6/2.1/systemdrivers/vcs/image/adsp/hw/sdm670/VCSTarget.c#1 $
  $DateTime: 2017/07/21 22:10:47 $
  $Author: pwbldsvc $
  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "VCSImage.h"

/*=========================================================================
      Macros
==========================================================================*/


/*=========================================================================
      Type Definitions
==========================================================================*/


/*=========================================================================
      Extern Definitions
==========================================================================*/


/*=========================================================================
      Function prototypes
==========================================================================*/


/*=========================================================================
      Data
==========================================================================*/


/* =========================================================================
      Prototypes
==========================================================================*/


/* =========================================================================
      Functions
==========================================================================*/


/* =========================================================================
**  Function : VCS_InitTarget
** =========================================================================*/
/*
  See VCSDriver.h.
*/

DALResult VCS_InitTarget (VCSDrvCtxt *pDrvCtxt)
{
  DALResult eResult;

  eResult = VCS_InitImage(pDrvCtxt);
  if (eResult != DAL_SUCCESS)
  {
    return DAL_ERROR;
  }

  return DAL_SUCCESS;

} /* END VCS_InitTarget */

/* =========================================================================
**  Function : VCSStub_InitTarget
** =========================================================================*/
/*
  See VCSDriver.h.
*/

DALResult VCSStub_InitTarget (VCSDrvCtxt *pDrvCtxt)
{
  return DAL_SUCCESS;

} /* END VCSStub_InitTarget */
