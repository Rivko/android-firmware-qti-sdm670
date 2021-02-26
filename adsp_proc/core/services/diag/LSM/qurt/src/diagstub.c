/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         Diagnostic Task Stub 

General Description
  The diagnostic task supports the interface between the mobile software
  tasks and the diagnostic monitor. The diagnostic task responds to 
  packets sent by the Diagnostic Monitor in order to support software
  debugging, data logging and displaying the temporal analyzer.

Copyright (c) 1992-2010, 2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.qdsp6/2.1/services/diag/LSM/qurt/src/diagstub.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/02/12   ph      Removed time_get() from diag code.
08/20/12   sg      Changed qurt_sysclock_get_hw_ticks() return type to
                   unsigned long long 
02/24/11   sg      Replaced  qsystem_sclk_attr_getticks() with 
                   blast_system_sclk_attr_gethwticks()which is a non-blocking call
09/23/10   is      Changed ts_get() to time_get()
03/25/09   JV      Cleaned warnings for the Q6 branch
05/17/07   jc      Proxy by tv.  Revise checking of REX signals.
01/08/07   tbg     Added support for new err_auto_action API
01/05/07   as      Featurized drian diag timer for multi processor code.
12/21/06   as      Moved RX_SIG and Drain SIG above TX Sig.
12/15/06   as      Fixed compiler warnings.
12/04/06   as      Added  support to drain diag buffer based on a timer.
06/29/06   giri    Stubbing the panic mode implementation for the core services
                   build. The stubs is defined in diag_stub.c file.
05/03/06   as      Feature FEATURE_DISABLE_DLOAD_ENTRY_ON_ERROR_FATAL disables
                   going into downloader on error fatal.
10/31/05   as      Fixed lint errors.
07/25/05   as      Added TRAMP_SLEEP_TIMER_TICK_ISR in diag panic handler.
07/05/05   sl      Added support for SPC_TIMEOUT to double the timeout value
                   on consecutive incorrect SPCs.
06/27/05   as      FEATURE_L4 turns off diag panic mode.functionality.
03/30/05   sl      Added support for SPC_UNLOCK_TTL to unlock/lock the sp_state
03/14/05   as      Moved diagpkt_init_manual_dispatch_table() call to diag_task
02/28/05   as      Replaced rex critical section with intlocks.
02/15/05   as      Added FEATURE_DIAG_DEBUG_6275. This temporary feature will
                   be used until Raven code is stabilized.
02/10/05   as      Added FEATURE_DEBUG_DIAG_HEAP to debug memory corruption.
01/27/05   as      Restored check for blocked diag heap in diag_malloc().
12/14/04   sl      Additionaly diag_timeout() resets the SIO stream ID if
                   FEATURE_DIAG_MULTI_PORTS is defined.
04/06/04   tbg     Added support for FEATURE_ERR_EXTEDED_STORE
09/23/03   gr      Added code to not iterate through the diag heap when out
                   of memory. Should improve efficiency in certain situations.
09/08/03   as      Changed diag_tx_notify to call rex timer only when 
                   interrupts are locked.
09/03/03   as      Removed diag_tx_notify_timer
12/12/02   lad     Added panic mode powerdown option (NV).
12/02/02   lad     Removed inactivity timer.
11/21/02   lad     Mainlined diag heap.
09/13/02   lad     Added config save to filestream mode.
07/30/02   as      Added inactivity timer to detect external device 
                   disconnect.
07/28/02   lad     Moved services initializations to diag_init().
06/21/02   lad     Added the filestream feature to capture data generated
                   prior to tool establishing connection.
                   Restructured processing of inbound data for reuse w/
                   filestream feature.
08/20/01   jal     Changes to support more Diag packets.  Handle online/
                   offline mode.  Jump to downloader added.  Added accessor
                   functions for service programming lock state.
06/27/01   lad     Changed diag_hdlc_decode to take signed length to
                   detect invalid input.
                   Fixed error in handling the RX of a packet that is too big
04/06/01   lad     TASK_ signals were replaced with DIAG_TASK_xxx signals to 
                   remove dependency on task.h.
                   Chagned max_req_pkt_len in diag_task to be 
                   DIAG_MAX_RX_PKT_SIZ which is defined in diagtune.h.
                   Changed instance of diag_start_task() to 
                   diag_ready_start_task().
                   Changed diag_do_escaping and diag_hdlc_decode to not pass 
                   ASYNC control chars as params (use constants instead).  This
                   makes the function call cheaper and optimizes the routine
                   slightly.
                   Featurized diagpkt_refresh_cache() call under 
                   FEATURE_DIAG_CACHE.
02/23/01   lad     Rearchitected the diagnostics subsystem to be a service 
                   rather than a module.  All coupling to targets has been
                   moved to target-specific implementations.  The only coupling
                   that remains are common services and REX.
                   Old history comments have been removed since most of them
                   are no longer applicable.

===========================================================================*/

#include "diagstub.h"
#include <stdio.h>
#include "qurt.h"

uint8 diag_err_fatal_count=0;

void diag_err_fatal()
{
	fprintf(stderr, "Diag hit an error fatal\n");
	diag_err_fatal_count++;
	exit(1);
}




