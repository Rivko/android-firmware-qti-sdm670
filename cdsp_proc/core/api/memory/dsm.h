#ifndef DSM_H
#define DSM_H
/*===========================================================================

                                  D S M . H

DESCRIPTION
  This file contains types and declarations associated with the DMSS Data
  Service Memory pool and services.

Copyright (c) 2006 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

#define DSM_MAJOR_VER_NO 3
#define DSM_MINOR_VER_NO 0

/*===========================================================================
                            EDIT HISTORY FOR FILE
                                      
  $Header: //components/rel/core.qdsp6/2.1.c4/api/memory/dsm.h#1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
2/7/10      ag     Removed featurization for CMI. 
1/1/06      pjb    Created new dsm.h.  
===========================================================================*/

#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "queue.h"

#include "dsm_item.h"
#include "dsm_init.h"
#include "dsm_kind.h"

#include "dsm_queue.h"

#include "dsm_tracer.h"

#ifdef __cplusplus
}
#endif

#endif /* DSM_H */
