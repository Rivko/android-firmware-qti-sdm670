#ifndef DIAGSTUB_H
#define DIAGSTUB_H
/*==========================================================================

                     Diagnostic Task Header File

Description
  Global Data declarations of the diag_task.

Copyright (c) 1991-2010, 2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                         Edit History

      $Header: //components/rel/core.qdsp6/2.1/services/diag/LSM/qurt/src/diagstub.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/24/16   gn      Changes to support common PW for ADSP, CDSP and SLPI
12/02/12   ph      Removed time_get() from diag code.
09/23/10   is      Changed ts_get() to time_get()
02/02/10   JV      SLEEP_PRI is now defined in diagdiag_common.c
12/03/09   JV      Switching over to QURT APIs in OSAL
03/25/09   JV      Cleaned warnings for the Q6 branch
12/22/06   as      Moved proc ID macros to diag.h
12/05/06   as      Added signal for Diag drain timer
11/21/06   as      Moved DIAG internal features from diagi.h to diag.h
03/30/05   sl      Added support for SPC_UNLOCK_TTL to unlock/lock the sp_state
10/17/03   ph      For Rel A builds, redirect to new MC subsystem command.
09/23/03    gr     Function prototypes related to changes that enable more
                   efficient detection of the condition where diag is out of
                   heap space.
08/20/01   jal     Changes to support more Diag packets.  Support for powerup
                   online/offline state, service programming lock state.
04/06/01   lad     Cosmetic changes.
02/23/01   lad     Rearchitected the diagnostics subsystem to be a service 
                   rather than a module.  All coupling to targets has been
                   moved to target-specific implementations.  This file now
                   contains an API.  No other information or coupling remains
                   except for featurized legacy code that impacts existing 
                   targets.
                   Old history comments have been removed since most of them
                   are no longer applicable.

===========================================================================*/
#include <stdlib.h>
#include "qw.h"
#include <assert.h>
#include "time_svc.h"


#define DIAG_ALIGN_BYTES        8

#define DIAG_ALIGN __attribute__((__aligned__(DIAG_ALIGN_BYTES)))

void diag_err_fatal(void);

/* The macro, assert is defined in the native assert.h */
//#define ASSERT( cond )      assert( cond )


#endif              /* DIAGSTUB_H  */
