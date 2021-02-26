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
  $Header: //components/rel/core.qdsp6/2.1.c4/systemdrivers/vcs/image/slpi/hw/sdm845/VCSTarget.c#1 $
  $DateTime: 2018/07/30 01:21:36 $
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
