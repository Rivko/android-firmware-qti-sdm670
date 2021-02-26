#ifndef IPC_ROUTER_SETTINGS_H
#define IPC_ROUTER_SETTINGS_H
/*===========================================================================

            I P C   R O U T E R   S E T T I N G S   T Y P E S

                           H E A D E R    F I L E

DESCRIPTION
  This file contains general data types IPC Router Settings
  
  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1.c4/settings/mproc/ipc_router/interface/ipc_router_settings.h#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/26/17   jlg     Initial version; establish settings interface
===========================================================================*/


/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include "stddef.h"
#include "stdint.h"
#include "com_dtypes.h"

/*===========================================================================
                  CONSTANT / MACRO DACLARATIONS
===========================================================================*/
/** Max number of intent categories */
#define IPC_ROUTER_XAL_GLINK_MAX_INTENT_TYPES 4

/*===========================================================================
                        TYPE DECLARATIONS
===========================================================================*/
/** Processor info */
typedef struct
{
  const char *name;
  uint32     id;
} ipc_router_proc_info_type;

/** Transport info and, table of transports supported */
typedef struct
{
  const char *name;
  const void *table;
} ipc_router_xal_info_type;

/** Paramters provided to the transport */
typedef struct {
  /* Transport to use, NULL, if we would prefer GLink to pick one */
  const char   *transport;
  /* Remote subsystem */
  const char   *remote_ss;
  /* Name of the channel */
  const char   *name;
  /* Channel options */
  unsigned int options;
  /* (OS Specific) Priority of the worker thread if present */
  uint8_t      priority;
  /* (OS Specific) Size of the stack of the worker thread. 0 may
   * be used to indicate that the XAL will operate in a threadless 
   * manner */
  size_t       stack_size;

  /* The intent spread/pattern */
  struct {
    /* Size of the intent */
    size_t     size;
    /* Number of intents of the size specified above queued */
    size_t     count;
  } intents[IPC_ROUTER_XAL_GLINK_MAX_INTENT_TYPES];

} ipc_router_xal_glink_param_type;

#endif /* IPC_ROUTER_SETTINGS_H */
