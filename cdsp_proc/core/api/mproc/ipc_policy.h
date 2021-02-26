#ifndef _IPC_POLICY_H_
#define _IPC_POLICY_H_

#include "stdint.h"
#include "string.h"
/******************************************************************************
  @file    ipc_policy.h
  @brief   Public interface detailing on various IPC policies which
           which can be used to limit/police IPC communications.

  DESCRIPTION
  This header provides IPC entities like QMI services or clients a set of
  definitions which can used to limit their visibility/scope.
  
  ---------------------------------------------------------------------------
  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------

 *******************************************************************************/

/* DEFINITIONS:
 *
 * IPC Entity: is a software entity interested in inter processor/process communication.
 * 
 * SCOPE: Defines the visibility of the particular IPC entity. A scope directly
 *        or indirectly defines the set of remote/local IPC entities to which
 *        it may communicate with or be visible to. An IPC Entity which falls
 *        outside the scope will not be able to see or communicate with the 
 *        IPC entity at question.
 *
 * Local processor: The processor of the IPC entity at question.
 *  
 * */

#define IPC_POLICY_SUCCESS (0)
#define IPC_POLICY_FAILURE (-1)
#define IPC_POLICY_DENIED  (-2)


#define IPC_POLICY_LINK_TAG_INVALID   (0)
#define IPC_POLICY_LINK_TAG_MPSS      (1)
#define IPC_POLICY_LINK_TAG_ADSP      (2)
#define IPC_POLICY_LINK_TAG_WCNSS     (3)
#define IPC_POLICY_LINK_TAG_APPS      (4)
#define IPC_POLICY_LINK_TAG_EXT_APPS  (5)

/** Limit the scope to the local processor only. */
#define IPC_POLICY_SCOPE_LOCAL_PROCESSOR    (0x0ULL) 

/** Special scope to allow all communication */
#define IPC_POLICY_SCOPE_OPEN               (0xffffffffffffffffULL)

#define IPC_POLICY_MAX_PROCESSOR_ID (63)

typedef struct
{
  uint32_t mask[(IPC_POLICY_MAX_PROCESSOR_ID + 1) / 32];
} ipc_policy_processor_set_type;

#define IPC_POLICY_PROCESSOR_SET_INIT(set) memset(set.mask, 0, sizeof(set.mask));
#define IPC_POLICY_PROCESSOR_SET_ADD(set, proc) \
  do {  \
    if(proc <= IPC_POLICY_MAX_PROCESSOR_ID) { \
      set.mask[(proc) / 32] |= (0x1UL<<((proc % 32)));  \
    } \
  } while(0)

#define IPC_POLICY_PROCESSOR_SET_ADD_ALL(set) \
  do {  \
    set.mask[0] = set.mask[1] = 0xffffffff; \
} while(0)

#define IPC_POLICY_PROCESSOR_SET_DELETE(set, proc) \
  do {  \
    if(proc <= IPC_POLICY_MAX_PROCESSOR_ID) { \
      set.mask[(proc) / 32] &= (~(0x1UL<<((proc % 32))));  \
    } \
  } while(0)

/* Example white list:
 * IPC_POLICY_PROCESSOR_SET_INIT(set);
 * IPC_POLICY_PROCESSOR_SET_ADD(set, proc1);
 * IPC_POLICY_PROCESSOR_SET_ADD(set, proc2); 
 * ...
 *
 * Example black list:
 * IPC_POLICY_PROCESSOR_SET_INIT(set);
 * IPC_POLICY_PROCESSOR_SET_ADD_ALL(set);
 *
 * IPC_POLICY_PROCESSOR_SET_DELETE(set, proc1);
 * IPC_POLICY_PROCESSOR_SET_DELETE(set, proc2);
 * ...
 */

/*=============================================================================
  FUNCTION  ipc_policy_add_name_rule
=============================================================================*/
/*!
@brief
  Allows the IPC Policy owner to add a firewall rule

@param[in]  type                The type of the service
@param[in]  instance            The instance of the service.
@param[in]  instance            The instance of the service.
@param[in]  instance_mask       Comparisions with instance will use this
                                mask on the db entry's instance before
                                comparing.
@param[in]  allowed_hosts       Allowed processors which can host this service.
@param[in]  allowed_accessors   Processors from which connections to this 
                                service is allowed.
                                Note: allowed_hosts are by default allowed accessors. 
                                (Logically There is no way of preventing a processor 
                                from accessing a service which it hosts!)

@retval     IPC_POLICY_SUCCESS  Success
@retval     IPC_POLICY_DEINED   A policy has already been set for this type.
@retval     IPC_POLICY_FAILURE  Generic failure.
*/
/*=========================================================================*/
int ipc_policy_add_name_rule
(
  unsigned int type, 
  unsigned int instance, 
  unsigned int instance_mask,
  ipc_policy_processor_set_type *allowed_hosts,
  ipc_policy_processor_set_type *allowed_accessors
);

/*=============================================================================
  FUNCTION  ipc_policy_add_link_rule
=============================================================================*/
/*!
@brief
  Allows the IPC Policy owner to add a firewall rule

@param[in]  link_dest_desc      Description of the destination 
                                (Refer IPC_POLICY_LINK_TAG_*)
@param[in]  allowed_remote_procs Processors which are allowed on to be
                                visible over this link.

@retval     IPC_POLICY_SUCCESS  Success
@retval     IPC_POLICY_DEINED   A policy has already been set for this type.
@retval     IPC_POLICY_FAILURE  Generic failure.
*/
/*=========================================================================*/
int ipc_policy_add_link_rule
(
  unsigned int link_dest_desc, 
  ipc_policy_processor_set_type *allowed_remote_procs
);

#endif
