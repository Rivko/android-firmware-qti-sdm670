/*===========================================================================

                GLINK SMEM XPORT DEVICE CONFIGURATION

    This file provides the device configuration for Glink SMEM xport

  ---------------------------------------------------------------------------
  Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header:

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/

/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include "DALStdDef.h"
#include "xport_smem_devcfg.h"

/*===========================================================================
                    CONSTANT / MACRO DECLARATIONS
===========================================================================*/


/*===========================================================================
                      LOCAL DATA DEFINATION
===========================================================================*/

/*===========================================================================
                      GLOBAL DATA DEFINATION
===========================================================================*/

/** Processor information */
const xport_smem_devcfg_proc_type xport_smem_devcfg_proc =
{
  "lpass",                /* Local host name */
  SMEM_ADSP               /* Local host ID   */
};

/** Transports supported */
const xport_smem_devcfg_remote_type xport_smem_devcfg_remote_procs[] =
{
  /* ADSP->Apss */
  {
    "apss",                /* Remote host name               */
    SMEM_APPS,             /* Remote host ID                 */
    16*1024,               /* Tx FIFO size                   */
    4*1024,                /* Tx MTU size                    */
                           /* Outgoing interrupt             */
    {DALIPCINT_PROC_ACPU,
    DALIPCINT_GP_1},
    8,                     /* Incoming interrupt             */
    20000                  /* Maximum data rate (tokens/sec) */
  },

  /* ADSP->Modem */
  {
    "mpss",                /* Remote host name               */
    SMEM_MODEM,            /* Remote host ID                 */
    16*1024,               /* Tx FIFO size                   */
    4*1024,                /* Tx MTU size                    */
                           /* Outgoing interrupt             */
    {DALIPCINT_PROC_MCPU,
    DALIPCINT_GP_3},
    34,                    /* Incoming interrupt             */
    20000                  /* Maximum data rate (tokens/sec) */
  },

  /* ADSP->SSC */
  {
    "dsps",                /* Remote host name               */
    SMEM_SSC,              /* Remote host ID                 */
    8*1024,                /* Tx FIFO size                   */
    4*1024,                /* Tx MTU size                    */
                           /* Outgoing interrupt             */
    {DALIPCINT_PROC_SPS,
    DALIPCINT_GP_3},
    93,                    /* Incoming interrupt             */
    20000                  /* Maximum data rate (tokens/sec) */
  },

  /* END */
  {
    NULL,                  /* Remote host name               */
    SMEM_INVALID_HOST,     /* Remote host ID                 */
    0,                     /* Tx FIFO size                   */
    0,                     /* Tx MTU size                    */
                           /* Outgoing interrupt             */
    {DALIPCINT_PROC_32BITS,
    DALIPCINT_INT_32BITS},
    0,                     /* Incoming interrupt             */
    0                      /* Maximum data rate (tokens/sec) */
  },

};

