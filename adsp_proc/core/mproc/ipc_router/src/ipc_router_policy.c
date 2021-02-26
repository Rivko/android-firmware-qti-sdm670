/*===========================================================================

               I P C    R O U T E R   P O L I C Y

        This file provides the policy abstraction for IPC Router

  ---------------------------------------------------------------------------
  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1/mproc/ipc_router/src/ipc_router_policy.c#1 $
$DateTime: 2017/07/21 22:10:47 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/06/13   aep     Initial creation
==========================================================================*/

/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include "ipc_router_policy.h"
#include "ipc_router_database.h"
#include "ipc_policy.h"

/*===========================================================================
                  CONSTANT / MACRO DECLARATIONS
===========================================================================*/
#define IPC_ROUTER_POLICY_TABLE_SIZE (32)
#define TYPE2IDX(type) ((type) & (IPC_ROUTER_POLICY_TABLE_SIZE - 1))

#define ASSIGN_SET_TO_UINT64(num, set)  \
  do {  \
    if((set)) { \
      num = (set)->mask[1]; \
      num = ((num) << 32) & 0xffffffff00000000ULL;  \
      num = (num) | (set)->mask[0];  \
    } else {  \
      num = 0xffffffffffffffffULL;  \
    } \
  } while(0)

#define PROC2MASK(proc) (1ULL << (proc))

#define IS_PROC_VALID(proc) ((proc) <= (sizeof(uint64) * 8))

/*===========================================================================
                        TYPE DECLARATIONS
===========================================================================*/
struct ipc_router_name_policy_record_s;
typedef struct ipc_router_name_policy_record_s ipc_router_name_policy_record_type;

struct ipc_router_name_policy_record_s
{
  LINK(ipc_router_name_policy_record_type, link);
  uint32 type;
  uint32 instance;
  uint32 instance_mask;
  uint64 allowed_hosts;
  uint64 scope;
};

struct ipc_router_link_policy_record_s;
typedef struct ipc_router_link_policy_record_s ipc_router_link_policy_record_type;

struct ipc_router_link_policy_record_s
{
  LINK(ipc_router_link_policy_record_type, link);
  uint32 link_dest_desc;
  uint64 allowed_remote_procs;
};

/*===========================================================================
                        MODULE GLOBALS
===========================================================================*/
extern uint32 ipc_router_local_processor_id;

/* This table is read only, modified once at bootup before IPC Router 
 * is initialized. Thus, the lack of a lock */
static LIST(ipc_router_name_policy_record_type, g_name_policy_tbl)[IPC_ROUTER_POLICY_TABLE_SIZE];

static LIST(ipc_router_link_policy_record_type, g_link_policy_list);

/*=============================================================================
                               LOCAL FUNCTIONS
=============================================================================*/
static ipc_router_name_policy_record_type *ipc_router_find_name_policy
(
  uint32 type,
  uint32 instance
)
{
  ipc_router_name_policy_record_type *i = NULL;
  uint32 idx = TYPE2IDX(type);

  LIST_FIND(g_name_policy_tbl[idx], i, link, 
      ((instance & i->instance_mask) == i->instance)&&(type==i->type));
  return i;
}

static ipc_router_link_policy_record_type *ipc_router_find_link_policy
(
  uint32 link_desc
)
{
  ipc_router_link_policy_record_type *i = NULL;
  LIST_FIND(g_link_policy_list, i, link, link_desc);
  return i;
}

/*=============================================================================
  FUNCTION  ipc_policy_symmetric
=============================================================================*/
/*!
@brief
  Returns true if the particular operation can be performed.

@param[in]  scope               The scope of the sending/receiving
                                IPC entity. NULL if the check is not
                                required.
@param[in]  proc                Processor ID of the remote endpoint for
                                which a packet is to be sent to / received from
@param[in]  xport               The scope of the xport describing the set
                                of processors which can be reached over this
                                link. NULL if the check is not required.

@retval    1                    This operation is allowed.
@retval    0                    This operation is NOT allowed.
*/
/*=========================================================================*/
static int ipc_policy_symmetric
(
  uint64 *scope,
  uint32 proc,
  uint64 *xport
)
{
  if(!IS_PROC_VALID(proc))
    return 0;
  if(xport && (PROC2MASK(proc) & *xport) == 0)
    return 0;
  if(scope && (PROC2MASK(proc) & *scope) == 0)
    return 0;
  return 1;
}

/*=============================================================================
                         EXPORTED FUNCTIONS
=============================================================================*/

/*=============================================================================
  FUNCTION  ipc_policy_send
=============================================================================*/
static int ipc_policy_send
(
  uint64 *from_scope,
  uint32 to_proc,
  uint64 *to_xport
)
{
  return ipc_policy_symmetric(from_scope, to_proc, to_xport);
}

/*=============================================================================
  FUNCTION  ipc_policy_recv
=============================================================================*/
static int ipc_policy_recv
(
  uint64 *from_xport,
  uint32 from_proc,
  uint64 *to_scope
)
{
  return ipc_policy_symmetric(to_scope, from_proc, from_xport);
}

/*=============================================================================
  FUNCTION  ipc_policy_get_name_scope
=============================================================================*/
/*!
@brief
  Returns true if the particular operation can be performed.
  if the operation is allowed, the scope is returned.

@param[in]  type                The type of the service 'name'
@param[in]  instance            The instance of the service 'name'
@param[in]  host                The processor ID of the name's host.
@param[out] scope               If the host is valid for the name, then
                                the scope of the service is returned.

@retval    1                    This operation is allowed.
@retval    0                    This operation is NOT allowed.
*/
/*=========================================================================*/
static int ipc_policy_get_name_scope
(
  uint32 type,
  uint32 instance,
  uint32 host,
  uint64 *scope
)
{
  ipc_router_name_policy_record_type *record;
  record = ipc_router_find_name_policy(type, instance);
  if(!record)
  {
    /* Default open */
    if(scope)
      *scope = IPC_POLICY_SCOPE_OPEN;
    return 1;
  }
  
  if(!IS_PROC_VALID(host) || 
      (PROC2MASK(host) & record->allowed_hosts) == 0)
  {
    return 0;
  }
  if(scope)
    *scope = record->scope;
  return 1;
}

/*=============================================================================
  FUNCTION  ipc_policy_get_link_scope
=============================================================================*/
/*!
@brief
  Return the set of processors which are allowed on this link.

@param[in]  tag                 A numeric tag describing the link.

@retval    scope of the link.
*/
/*=========================================================================*/
static uint64 ipc_policy_get_link_scope
(
  uint32 tag 
)
{
  ipc_router_link_policy_record_type *record;
  record = ipc_router_find_link_policy(tag);
  if(record)
    return record->allowed_remote_procs;
  return IPC_POLICY_SCOPE_OPEN;
}

/*=============================================================================
  FUNCTION  ipc_policy_add_name_rule
=============================================================================*/
int ipc_policy_add_name_rule
(
  unsigned int type, 
  unsigned int instance, 
  unsigned int instance_mask,
  ipc_policy_processor_set_type *allowed_hosts,
  ipc_policy_processor_set_type *allowed_accessors
)
{
  ipc_router_name_policy_record_type *record;

  /* Do not allow adding rules once core is
   * initialized. */
  if(ipc_router_local_processor_id != 0xffffffff)
  {
    return IPC_POLICY_DENIED;
  }

  /* Do not allow adding duplicate rules */
  if(ipc_router_find_name_policy(type, instance))
  {
    return IPC_POLICY_DENIED;
  }

  record = ipc_router_os_calloc(1, sizeof(*record));
  if(!record)
  {
    return IPC_POLICY_FAILURE;
  }

  LINK_INIT(record->link);
  record->type = type;
  record->instance = instance;
  record->instance_mask = instance_mask;
  ASSIGN_SET_TO_UINT64(record->allowed_hosts, allowed_hosts);
  ASSIGN_SET_TO_UINT64(record->scope, allowed_accessors);

  /* Its silly to have a rule which allows a processor
   * to host a service but does not allow that processor
   * to access that service it seems to host. Set it straight
   * now rather than have more complex logic later on */
  record->scope |= record->allowed_hosts;

  LIST_ADD(g_name_policy_tbl[TYPE2IDX(type)], record, link);
  return IPC_POLICY_SUCCESS;
}

/*=============================================================================
  FUNCTION  ipc_policy_add_link_rule
=============================================================================*/
int ipc_policy_add_link_rule
(
  unsigned int link_dest_desc, 
  ipc_policy_processor_set_type *allowed_remote_procs
)
{
  ipc_router_link_policy_record_type *record;

  /* Do not allow adding rules once core is
   * initialized. */
  if(ipc_router_local_processor_id != 0xffffffff)
  {
    return IPC_POLICY_DENIED;
  }

  /* Do not allow adding duplicate rules */
  if(ipc_router_find_link_policy(link_dest_desc))
  {
    return IPC_POLICY_DENIED;
  }

  record = ipc_router_os_calloc(1, sizeof(*record));
  if(!record)
  {
    return IPC_POLICY_FAILURE;
  }

  LINK_INIT(record->link);
  record->link_dest_desc = link_dest_desc;
  ASSIGN_SET_TO_UINT64(record->allowed_remote_procs, allowed_remote_procs);
  LIST_ADD(g_link_policy_list, record, link);
  return IPC_POLICY_SUCCESS;
}

ipc_router_policy_ops_type ipc_router_impl_policy = 
{
  ipc_policy_send,
  ipc_policy_recv,
  ipc_policy_get_name_scope,
  ipc_policy_get_link_scope
};


