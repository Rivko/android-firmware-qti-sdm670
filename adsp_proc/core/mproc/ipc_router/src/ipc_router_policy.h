#ifndef _IPC_ROUTER_POLICY_H_
#define _IPC_ROUTER_POLICY_H_
/******************************************************************************
  @file    ipc_router_policy.h
  @brief   Private/Transport interface detailing on various IPC policies which
           which can be used to limit/police IPC communications.

  DESCRIPTION
  This header provides IPC providers like the IPC Router
  definitions which can used to enforce IPC Policy.

  Policy functions should not be called for local processors where 
  the transport TAG is undefined. 
  
  ---------------------------------------------------------------------------
  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
 *******************************************************************************/

#include "ipc_policy.h"
#include "ipc_router_types.h"
#include "ipc_router_os.h"

extern ipc_router_policy_ops_type ipc_router_impl_policy;

#endif
