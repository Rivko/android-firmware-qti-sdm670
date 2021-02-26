#ifndef PM_MALLOC_H
#define PM_MALLOC_H

/*! \file
 *  
 *  \brief  pm_malloc.h ----This file contain PMIC wrapper function of DALSYS_Malloc()
 *  \details his file contain PMIC wrapper function of DALSYS_Malloc()
 *  
 *    &copy; Copyright 2012 Qualcomm Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module over time.

$Header: //components/rel/core.qdsp6/2.1/pmic/pm/utils/src/pm_malloc.h#1 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/20/12   hs      Created

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "comdef.h"

/*===========================================================================

                        GENERIC FUNCTION PROTOTYPES

===========================================================================*/
extern void pm_malloc(uint32 dwSize, void **ppMem);

#endif // PM_MALLOC_H
