#ifndef XPORT_SMEM_DEVCFG_H
#define XPORT_SMEM_DEVCFG_H

/**
 * @file xport_smem_config.h
 *
 * Internal definitions for Glink SMEM transport configurations
 */

/*==============================================================================
     Copyright (c) 2014-2015, 2017 QUALCOMM Technologies, Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1.c4/settings/mproc/glink/xport_smem/interface/xport_smem_devcfg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/26/17   jlg     Moved device configuration settings to core/settings directory
05/07/15   an      QoS support
06/17/14   an      Initial version
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "glink.h"
#include "smem_type.h"
#include "DDIIPCInt.h"

/*===========================================================================
                      MACRO DECLARATIONS
===========================================================================*/

/*===========================================================================
                      TYPE DECLARATIONS
===========================================================================*/
/** Local processor info */
typedef struct _xport_smem_devcfg_proc_type
{
  const char     *name;                 /* Local host name */
  smem_host_type host;                  /* Local host ID   */
} xport_smem_devcfg_proc_type;       

/** Remote subsystem configuration */
typedef struct _xport_smem_devcfg_remote_type
{
  const char             *remote_ss;        /* Remote host name               */
  smem_host_type         remote_host;       /* Remote host ID                 */
  uint32                 fifo_size;         /* Tx FIFO size                   */
  uint32                 mtu_size;          /* Tx MTU size                    */
  DalIPCIntProcessorType irq_processor;     /* Remote processor DAL ID        */
  DalIPCIntInterruptType irq_out;           /* Outgoing interrupt             */
  uint32                 irq_in;            /* Incoming interrupt             */
  uint32                 qos_max_rate;      /* Maximum data rate (tokens/sec) */
} xport_smem_devcfg_remote_type;       

#endif //XPORT_SMEM_DEVCFG_H

