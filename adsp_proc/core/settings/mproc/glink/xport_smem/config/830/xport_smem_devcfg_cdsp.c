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
  "cdsp",                 /* Local host name */
  SMEM_CDSP               /* Local host ID   */
};

/** Transports supported */
const xport_smem_devcfg_remote_type xport_smem_devcfg_remote_procs[] =
{
  /* CDSP->Apss */
  {
    "apss",                /* Remote host name               */
    SMEM_APPS,             /* Remote host ID                 */
    16*1024,               /* Tx FIFO size                   */
    4*1024,                /* Tx MTU size                    */
                           /* Outgoing interrupt             */
    DALIPCINT_PROC_ACPU,
    DALIPCINT_GP_0,
    72,                    /* Incoming interrupt             */
    20000                  /* Maximum data rate (tokens/sec) */
  },
/* TODO Enable if ADSP<->CDSP is required */
#if 0
  /* CDSP->ADSP */
  {
    "adsp",                /* Remote host name               */
    SMEM_ADSP,             /* Remote host ID                 */
    8*1024,                /* Tx FIFO size                   */
    4*1024,                /* Tx MTU size                    */
                           /* Outgoing interrupt             */
    DALIPCINT_PROC_ADSP,
    DALIPCINT_GP_0,
    136,                   /* Incoming interrupt             */
    20000                  /* Maximum data rate (tokens/sec) */
  },
#endif
/* TODO Enable if MPSS<->CDSP is required */
#if 0
  /* CDSP->MPSS */
  {
    "mpss",                /* Remote host name               */
    SMEM_MODEM,            /* Remote host ID                 */
    8*1024,                /* Tx FIFO size                   */
    4*1024,                /* Tx MTU size                    */
                           /* Outgoing interrupt             */
    DALIPCINT_PROC_MCPU,
    DALIPCINT_GP_0,
    35,                   /* Incoming interrupt             */
    20000                  /* Maximum data rate (tokens/sec) */
  },
#endif
  /* TODO Enable if SLPI<->CDSP is required */
#if 0
  /* CDSP->MPSS */
  {
    "dsps",                /* Remote host name               */
    SMEM_SSC,              /* Remote host ID                 */
    8*1024,                /* Tx FIFO size                   */
    4*1024,                /* Tx MTU size                    */
                           /* Outgoing interrupt             */
    DALIPCINT_PROC_SPS,
    DALIPCINT_GP_0,
    105,                   /* Incoming interrupt             */
    20000                  /* Maximum data rate (tokens/sec) */
  },
#endif
  /* END */
  {
    NULL,                  /* Remote host name               */
    SMEM_INVALID_HOST,     /* Remote host ID                 */
    0,                     /* Tx FIFO size                   */
    0,                     /* Tx MTU size                    */
                           /* Outgoing interrupt             */
    DALIPCINT_PROC_32BITS,
    DALIPCINT_INT_32BITS,
    0,                     /* Incoming interrupt             */
    0                      /* Maximum data rate (tokens/sec) */
  },
};

