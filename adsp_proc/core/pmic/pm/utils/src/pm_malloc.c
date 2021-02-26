/*! \file
*  
*  \brief  pm_malloc.c ----This file contains the implementation of pm_malloc()
*  \details This file contains the implementation of pm_malloc()
*  
*  &copy; Copyright 2012 Qualcomm Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

$Header: //components/rel/core.qdsp6/2.1/pmic/pm/utils/src/pm_malloc.c#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/20/12   hs      Created

===========================================================================*/

/*===========================================================================

INCLUDE FILES 

===========================================================================*/
#include "pm_malloc.h"
#include "DALSys.h"
#include <err.h>

void pm_malloc(uint32 dwSize, void **ppMem)
{
    DALResult dalResult = DAL_SUCCESS;

    dalResult = DALSYS_Malloc(dwSize, ppMem);

    if( (DAL_SUCCESS != dalResult) || (NULL == *ppMem) )
    {
        ERR_FATAL( "PMIC-Memory allocation failed ", 0, 0, 0 );
    }
}
