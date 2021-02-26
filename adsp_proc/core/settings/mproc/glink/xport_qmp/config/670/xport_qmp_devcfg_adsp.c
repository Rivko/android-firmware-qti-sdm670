/*===========================================================================

                GLINK QMP XPORT DEVICE CONFIGURATION

    This file provides the device configuration for Glink QMP xport

  ---------------------------------------------------------------------------
  Copyright (c) 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header:

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/04/17   mad      Valid value for mailbox_area_size_bytes member to enable 
                    checking against a reference value that cannot be compromised
                    by peer micro-core 
===========================================================================*/

/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include "DALStdDef.h"
#include "xport_qmp_settings.h"
#include "msmhwiobase.h"

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
const xport_qmp_devcfg_proc_type xport_qmp_devcfg_proc =
{
  "lpass",                /* Local host name */
  SMEM_ADSP               /* Local host ID   */
};

/** Transports supported */
const xport_qmp_devcfg_remote_type xport_qmp_devcfg_remote_procs[] =
{
  /* ADSP->AOP */
  {
    "aop_slpi",           /* Logical Channel name    */
    "rpm",
    SMEM_INVALID_HOST,
    1*1024,               /* Total Mailbox region size including local and remote. */
    0,                    /* Local Mailbox size. Stub for slave side  */
    3,                    /* Max outstanding messages*/
    DALIPCINT_PROC_RPM, /* Outgoing interrupt      */
    DALIPCINT_GP_0,      
    7,                  /* Incoming interrupt      */
    0,
    AOP_SS_MSG_RAM_START_ADDRESS_BASE + 0x40000
  },
  /* NULL */
  {
    NULL,                 /* Logical Channel name    */
    NULL,                 /* Remote proc name        */
    SMEM_INVALID_HOST,    /* Remote host ID          */
    0,                    /* Mailbox region size. Stub for slave side */
    0,                    /* Local Mailbox size. Stub for slave side  */
    0,                    /* Max outstanding messages*/
    DALIPCINT_PROC_NULL,
    DALIPCINT_GP_0,       /* Outgoing interrupt      */
    0,                    /* Incoming interrupt      */
    0,
    NULL,                /* Mailbox desc start address -- Virtual address */
  }
};

