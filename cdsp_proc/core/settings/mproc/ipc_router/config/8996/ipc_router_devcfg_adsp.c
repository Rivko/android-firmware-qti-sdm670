/*===========================================================================

                   IPC ROUTER DEVICE CONFIGURATION

        This file provides the device configuration for IPC Router

  ---------------------------------------------------------------------------
  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header:

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/26/17   jlg     moved settings to core/settings directory
===========================================================================*/

/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include "ipc_router_settings.h"

/*===========================================================================
                    CONSTANT / MACRO DECLARATIONS
===========================================================================*/
#define IPC_ROUTER_XAL_GLINK_PRIO 150
#define IPC_ROUTER_XAL_GLINK_STACK_SIZE (1024 * 3)

/*===========================================================================
                      LOCAL DATA DEFINATION
===========================================================================*/
/* SMDL Transports */
static ipc_router_xal_smdl_param_type ipc_router_devcfg_smdl_xals[] =
{
  {
    "APPS",
    "IPCRTR",
    SMD_APPS_QDSP,
    SMD_STANDARD_FIFO,
    SMDL_OPEN_FLAGS_MODE_PACKET,
    FALSE
  },
  {
    "MODM",
    "IPCRTR",
    SMD_MODEM_QDSP,
    SMD_STANDARD_FIFO,
    SMDL_OPEN_FLAGS_MODE_PACKET,
    FALSE
  },
  {
    NULL, NULL, SMD_INVALID_CHANNEL_TYPE, 0, 0, FALSE
  }
};

/* GLINK Transports */
static ipc_router_xal_glink_param_type ipc_router_devcfg_glink_xals[] =
{
  {
    "SMEM", "dsps", "IPCRTR", 0,
    IPC_ROUTER_XAL_GLINK_PRIO,
    IPC_ROUTER_XAL_GLINK_STACK_SIZE,
    {{128,16}, {512, 8}, {1024+128, 5}, {8192+128, 1}}
  },
  {
    NULL, NULL, NULL, 0, 0, 0, {{0, 0}}
  }
};

/*===========================================================================
                      GLOBAL DATA DEFINATION
===========================================================================*/
/* Processor information */
const ipc_router_proc_info_type ipc_router_devcfg_proc =
{
  "adsp",
  5
};

/* Transports supported */
const ipc_router_xal_info_type ipc_router_devcfg_xals[] =
{
  {"SMDL",  (const void*)ipc_router_devcfg_smdl_xals},
  {"GLINK", (const void*)ipc_router_devcfg_glink_xals},
  {NULL, NULL}
};

