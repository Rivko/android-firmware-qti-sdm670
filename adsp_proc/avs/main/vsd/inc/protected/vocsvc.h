#ifndef __VOCSVC_H__
#define __VOCSVC_H__

/*===========================================================================

                         V O I C E   S E R V I C E S

DESCRIPTION
  This header file defines APIs for initializing all voice related modules.

REFERENCES
  None.

Copyright (C) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/avs.adsp/2.8.5/vsd/inc/protected/vocsvc.h#1 $
$DateTime: 2017/11/07 23:00:25 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/25/08   ymc     Initial revision.

===========================================================================*/

#include "apr_comdef.h"

/*===========================================================================

                 O P E R A T I O N   D E F I N I T I O N S

===========================================================================*/

typedef enum vocsvc_callindex_enum_t
{
  /* public */ VOCSVC_CALLINDEX_ENUM_UNINITIALIZED = 0,
  /* public */ VOCSVC_CALLINDEX_ENUM_INIT = 1,
  /* public */ VOCSVC_CALLINDEX_ENUM_POSTINIT = 2,
  /* public */ VOCSVC_CALLINDEX_ENUM_PREDEINIT = 3,
  /* public */ VOCSVC_CALLINDEX_ENUM_DEINIT = 4,
  /* public */ VOCSVC_CALLINDEX_ENUM_INVALID
}
  vocsvc_callindex_enum_t;

/*===========================================================================

                   F U N C T I O N   D E F I N I T I O N S

===========================================================================*/

/*===========================================================================

FUNCTION vocsvc_call_adsp

DESCRIPTION
  This procedure provides a single entry-point into the voice services.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
APR_EXTERNAL int32_t vocsvc_call_adsp (
  vocsvc_callindex_enum_t index,
  void* params,
  uint32_t size
);

/*===========================================================================

FUNCTION vocsvc_cm_call

DESCRIPTION
  This procedure provides a single entry-point into the voice services
  call manager registration.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
APR_EXTERNAL int32_t vocsvc_cm_call (
  vocsvc_callindex_enum_t index,
  void* params,
  uint32_t size
);

#endif  /* __VOCSVC_H__ */

