/*===========================================================================
    Copyright (c) 2015-2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
============================================================================*/
#include <stddef.h>
#include <stdint.h>
#include "object.h"
#include "teetest.h"
#include "teetest_ipc.h"

#include "CPVC.h"
#include "CDeviceID.h"
#include "IEnv.h"
#include "IEnv_invoke.h"
#include "CEnv_open.h"
#include "CGuestVM.h"
#include "CGuestVM_open.h"

/*------------------------------------------------------------------------
 IEnv objects an instance of a remote object or an instance of a local
 object. in each case the instance is returned in *obj_ptr. This is a handle
 to the object which must them be used whenever the object is referenced.

 It is the responsibility of the TEE to maintain reference counting of
 remote objects.
-------------------------------------------------------------------------*/

//------------------------------------------------------------------------
// Implementation
//------------------------------------------------------------------------
#define CEnv_release(ctx)   Object_OK
#define CEnv_retain(ctx)    Object_OK

static int32_t
CEnv_open(void *me, uint32_t id_val, Object *obj_ptr)
{
  int32_t ret = Object_ERROR_UNAVAIL;

  switch (id_val)
  {
    case CGuestVM_UID:
    {
      // create a local object
      *obj_ptr = CGuestVM_create();
      ret = Object_OK;
      break;
    }
    default:
      break;
  }
  return ret;
}

static int32_t
CEnv_log(void *me, const void *text_ptr, size_t text_len)
{
  return Object_OK;
}

static int32_t
CEnv_exit(void *me, int32_t code_val)
{
  return Object_OK;
}

static IEnv_DEFINE_INVOKE(CEnv_invoke, CEnv_, void *)

//------------------------------------------------------------------------
// Exported functions
//------------------------------------------------------------------------
Object CEnv_create(CEnv_Proximity_t proximity)
{
  if(proximity == ENV_LOCAL)
  {
    // create a local instance
    return (Object) { CEnv_invoke , NULL };
  }
  else
  {
    // create a remote instance, which we have no context for
    return (Object) { tee_ipc_outbound , NULL };
  }
}



