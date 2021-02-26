#ifndef SMEM_QDI_DRIVER_H 
#define SMEM_QDI_DRIVER_H
/*===========================================================================

                            SMEM QDI Driver Header

  @file
    smem_qdi_driver.h

  This is the QDI Driver header for SMEM in the Guest OS.

  Copyright (c) 2014 Qualcomm Technologies Incorporated. 
  All rights reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1.c4/mproc/smem/inc/smem_qdi_driver.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/28/14   rv      Initial version of QDI implementation for SMEM driver.
==========================================================================*/
/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include "qurt_qdi.h"

/*===========================================================================
                  CONSTANT / MACRO DECLARATIONS
===========================================================================*/
/* Define SMEM QDI method IDs */
#define SMEM_QDI_INIT                     (QDI_OPEN)
#define SMEM_QDI_DEINIT                   (QDI_CLOSE)

/* Custom QDI methods for the SMEM driver.  QDI_PRIVATE == 256. */
#define SMEM_QDI_ALLOC                    (QDI_PRIVATE + 0)
#define SMEM_QDI_GET_ADDR                 (QDI_PRIVATE + 1)

#define SMEM_QDI_DEV_NAME                 "/dev/smem"

/*===========================================================================
                   TYPE DECLARATIONS
===========================================================================*/


#endif /* SMEM_QDI_DRIVER_H */
