/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         Diagnostic Task

General Description
  The diagnostic task supports the interface between the mobile software
  tasks and the diagnostic monitor. The diagnostic task responds to
  packets sent by the Diagnostic Monitor in order to support software
  debugging, data logging and displaying the temporal analyzer.

Copyright (c) 1992-2017 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.qdsp6/2.1/services/diag/DCM/common/src/diag.c#6 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/09/17   nk      Fixed issue in diag_early_init() when its called concurrently
09/28/17   sa      Handle NULL ptr dereference when diagID exchange is pending
09/26/16   sp      Initialize diagbuf_mpd_q before Diag registration with QDI. Also cleanup unused mutex
02/21/17   ph      Tag DIAG ID header to all the diag packets sent out.
00/25/17   sa      Remove micro_diag_uimage_tout_timer while exiting uimage
07/27/17   sp      Fix bootup crash when clients call diag_early_init() before the diag task is initialized
08/29/17   gn      Changes to handle remote socket close
06/08/17   gn      Added support for diag services to uImage
06/05/17   nk      Added debug support for uimage in root PD
05/30/17   gn      Changes to support command deregistration
04/05/17   is      Check for 0 length in diag_get_rx_pkt_slave()
03/03/17   sp      Removed packed void * parameters
01/25/17   rs      Re-try sending control packets using bit mask and signal and by-passing the control heap usage.
05/12/16   ph      SMD references removal and code clean up.
05/03/16   ph      Changed UDP_WRITE_TIMEOUT to infinite.
03/16/16   as      Fixed diag_block() signal logic
01/05/16   ph      Flow control handled for buffering mode switch.
12/07/15   ph      Exit polling for service if we are unable to get the address
                   after the timeout duration elapsed.
12/22/15   is      Fix flow control race condition
12/01/15   ph      Flow control handled properly over control channel.
08/28/15   xy      Fixed issue in diag_handle_internal_sigs
08/10/15   ph      Fix the concurrency in setting/clearing DIAG_INTERNAL_SIG.
08/21/15   as      Resolved Diag signal conflicts
06/18/15   sa      Added support for time sync feature.
02/27/15   sa      Updated the masks while calling diagbuf_drain to exit if there is
                   something to write/read to/from SMDL.
01/16/15   sa      Updated the name for the commands injected in diag cmd queue.
12/17/14   ph      Mainlined the feature DIAG_C_STRING.
01/12/15   sr      Increase the maximum request response size to 4k
10/02/14   xy      Fixed diag compiler warnings for Sensors Image
09/01/14   ph      Added mutex around diag_internal_sigs to resolve concurrency
                   issues with feature mask update missing.
07/06/14   ph      Added support for Diag over STM
07/24/14   xy      Resolve Diag warnings with new LLVM 7.0.04 toolchain
07/16/14   sr      Resolved KW errors
04/10/14   sr      Support for peripheral buffering mode
04/01/14   rs      Added support for build mask centralization.
03/05/14   xy      Fixed KW issues
02/18/14   rh      Added flush internal signal to make diagbuf_flush() calls.
11/27/13   ph      Updated MSG 1.0 API references to MSG.2.0.
11/28/13   ph      Fixed the bug where diag_sleep_state won't be updated when diag
                   is switched to NRT mode.
11/22/13   sr      Removed PACK from pointer references
09/27/13   xy      Removed code under FEATURE_WINCE
09/25/13   xy      Handle sending out feature mask on control channel first before
                   sending out cmd reg tables in diag_handle_internal_sigs()
09/05/13   xy      remove the use of 'std strlcpy’
07/23/13   ph      Fixed fuzzing test crash in diag_task().
08/09/13   is      Clear bootup event masks explicitly when Diag is up
07/24/13   sr      Changed SMDL_READ_SIG to internal signal and reduced the
                   priority over CTRl_MSG_SIG
07/11/13   ph      Replaced the assert in diag_alloc_check with appropriate check.
05/20/13   sa      Clear the internal signal for diag idle processing once set
05/14/13   sr      Added mutex check around diag_idl_proc_tbl to prevent
                   concurrency issues
10/25/11   is      Remove duplicate calls to diagbuf_init()
4/22/13   ph       Moved listeners allocation to system heap.
03/13/13   rh      diag_handle_sigs() now matches return condition first
05/08/13   sa      Support to wake up diag for idle processing
05/08/13   sa      Support to set the diag tx signal when changed
                   to real time mode
05/08/13   sa      Support to clear and reset the diag drain timer during a
                   mode switch
04/18/13   ph      Replace memcpy() and memmove() with memscpy() and memsmove()
02/28/13   rh      Add diag_drain_timer_pending check in diag_clr_drain_timer()
02/11/13   sr      Mainling Diag central routing
01/16/13   rh      Combined DIAG_TX_WMQ_SIG and DIAG_SMDL_WRITE_SIG into one
                   handler because they performed almost the same purpose.
12/13/12   rs      Added timeout mechanism to unblock diag in diagbuf_flush.
01/15/13   sg      Handle DIAG_SMDL_WRITE_SIG in diag_handle_sigs
12/07/12   is      Send mask updates in a loop
11/15/12   is      Support for preset masks
10/15/12   rh      Diagbuf drain timer now handles being disabled by 0 length
08/24/12   ra      Support for querying Command Registration Tables
08/20/12   hvm     New signal DIAG_TX_WMQ_SIG for t-put improvements with SMDL
08/20/12   sg      Clear the DIAG_TOUT_SIG in diag_suspend() once we come out of
                   osal_timed_wait()
07/05/12   sg      Changes to bring up diag Multi PD
05/03/12   rs      Moved delayed response from diag buffer to AMSS heap.
03/02/12   sg      Fixed Compiler Warning
10/10/11   sg      Remove depricated function strncpy on q6
09/28/11   is      Support dynamic sleep voting and non-deferrable Diag timers
09/12/11   is      Master Diag forwards "set mask" requests via CTRL msgs
08/09/11   is      Changed debug msg to match SIL signature for blacklisting
08/08/11   wjg     Added signal to handle smdl_event_read in diag context
06/14/11   hm      Migrating to REX opaque TCB APIs
05/27/11   sg      Check the size of the diag request packet
                   when received on slave processor to not
                   exceed the size of diag_req_inbound
05/05/11   is      Remove support for PLT registrations
04/28/11   is      Update featurizations for central routing
04/27/11   is      Resolve compiler warnings
04/18/11   is      Ensure CTRL msg sizes are constant
03/29/11   sg      Central Routing changes for Q6
03/04/11   is      Support for Diag over SMD-Lite
03/02/11   hm      Support SIO control channel on single processor
                   for dual-mask feature
03/01/11   is      Migrate to diag_strlcpy()
02/24/11   hm      Fixed compiler warnings
02/23/11   vs      Moved rdm_register calls after diag task ready for 9k
02/04/11   vs      Added call to diag_gpio_init
12/20/10   mad     Changes for dual mask diag:open SIO control port for APPS
01/24/10   is      Define chk_security_code_ptr[] when Dload not present
10/14/10   mad     Closing SIO control port on DIAG_COMM_CLOSE_SIG
10/25/10   is      Diag feature flag cleanup
10/20/10   is      Bypass RDM to set default to UART1 for 8960 Apps only bringup
09/28/10   mad     dynamically configurable drain timer and sleep vote
09/28/10   mad     closing control port on DIAG_COMM_CLOSE_SIG
09/28/10   mad     opening SIO port unconditionally, fixed compile error regarding
                   DIAGCOMM_SMD_1 and DIAGCOMM_SMD_2
09/27/10   is      New control channel and central routing features
08/10/10   sg      Close SMD Port when the SIO port on apps is closed
09/16/10   mad     Explicitly setting synch. event to signal diag clients
09/16/10   vs      Added diag task's registration for dog
09/01/10   sg      Setting txsignal in diag_idle_processing_register()
                   to wake up diag task to do idle processing when there is no traffic
07/12/10   vs      Removed mutexes from diag_searchlist_search_all
07/06/10   sg      Fixed compiler warnings
07/23/10   mad     Moved diagpkt_check_validity_request_ondevice() to this file
07/23/10   mad     Changes for dynamic on-device mask processing for windows devices:
                   Removed functions diag_ondevice_process_inbound() and
                   checkpacketvalidity()
07/20/10   sg      Added mutexes to osal_set_timer and osal_reset_timer to prevent
                   deletion of qtimer handle which is already deleted in case of
                   drain timer ,stale timer which are set in client's context
07/15/10   sg      Cleaning up of setting drain timer during diag task initialization
                   since we set the signal to drain  the buf when we get first rx packet
04/06/10   sg      Merging back DCM WM and RTOS files
03/25/10   mad     Added support for on-device logging
04/02/10   sg      Merging memory leak fix to AMSS side
03/30/10   vs      Added featurization around clk_regime calls
03/26/10   JV      Eliminating the event_pending timer and event_pending_q
03/24/10   sg      Wait on DIAG_RX_RSP_SIG when draining the logs and messages
                   in diagbuf_drain
03/23/10   sg      EVENT_STALE_TIMER  is set when the mask is zero to drain
                   the events that are already present in the diag_heap
03/18/10   sg      Diag Votes For Sleep in DIAG_RX_RSP_SIG
03/06/10   as      Added support for processing Ondevice Logging.
03/05/10   JV      Set the TX sig to start draining when we receive teh first RX pkt.
02/23/10   as      Fixed indentation and limited the scope of efs_stat fail case.
02/10/10   vs      Removed DIAG_MP for creating of drain_timer
01/20/10   JV      Added a new signal that when set, drains events
01/18/10   JV      DO not initialize the 2 SMD pools for single processor targets.
12/28/09   ps      Fixed Klocwork errors & compiler warnings.
12/10/09   sg      Added flow control mechanism for DSM_DIAG_SIO_TX_ITEM_POOL
                   at item level granularity
11/30/09   ps      Fixed compiler warnings.
11/17/09   JV      Wait on DIAG_RX_SIG when diagbuf_drain() is called while
                   processing the DIAG_DRAIN_TIMER_SIG. Extension of the previous
                   bug fix.
10/28/09   JV      Include diag_cfg.h to get the DIAG_HEAP_SIZE
10/21/09   JV      Fix for bug where we the mask passed to diagbuf_drain()
                   to wait on did not contain the DIAG_RX_SIG ORed in.
10/16/09   JV      FIxed bug in opening diagcomm_smd port.
09/22/09   vs      Use OSAL for all rex calls on qdsp6.
09/09/09   JV      Changed signature of osal mutex calls.
09/02/09   mad     merged handling of DIAG_TX_SIG and DIAG_TX_SLAVE_SIG.
                   This ensures that TX_SIG is handled, when slave traffic is
                   queued up.
09/01/09   JV      Changes to migrate from EFS1 to EFS2 APIs. These EFS2 calls
                   are synchronous, so there is no need to wait on any signal.
08/28/09   JV      Fix to enable boot-up messages. Register toolsdiag and diagdiag
                   sooner. Done in diag_init().
07/14/09   vs      diagbuf_drain is also called when drain_timer expires.
07/13/09   vs      Added DIAG_DRAIN_TIMER_SIG back in diag_handle_sigs
                    drain timer is now set only in diabuf_commit if not
08/06/09   sg      Added support for DSM Chaining in diag_get_rx_pkt().
08/05/09   JV      task.h is now included in osal.
08/05/09   JV      Removed the CUST_H featurization around the inclusion of
                   customer.h.
07/31/09   JV      Removed warnings
07/31/09   JV      Merged Q6 diag code back to mainline
07/31/09   JV      Brought back FEATURE_DIAG_NV
07/23/09   JV      Changed the signature of diag_wait() to generic types from osal
                   types so that diagi.h can be included by clients without us
                   making osal.h a public header file.
07/10/09   as      Decoupled FEATURE_DIAG_NON_STREAMING & removed support  for
                   FEATURE_DIAG_SPC_TIMEOUT
06/04/09   as      Reverted change from EFS2 to EFS1.
05/12/09   JV      Introduced the OS abstraction layer for rex.
05/11/09   as      Fixed forward traffic issue when Diag port is NULL
05/07/09   vk      chages made to move from EFS1 to EFS2 apis
05/07/09   vk      changed includes from external to internal headers
04/22/09   mad     modified usage of diagbuf_tx_sleep_threshold_set() and
                   diagbuf_tx_sleep_time_set()
04/21/09   ps        branch collapse of 6k and 7k DIAG
03/26/09   ps     Merged diag_wait with diag_handle_sigs.
03/26/09   ps      changes to fix CR 158777
03/06/09   vg      Added call to initialize the diagpkt_resp_q
                   The call is diagpkt_rsp_q_init()
03/05/09   vg      Removed the mutex release on suspend, pause, timeout
03/04/09   vg      Merged changes that mad made for forward packets on Diag
                   task
03/02/09   is      Fix featurizations to keep modem/apps procs in sync in using
                   new vs old sleep API
02/18/09   is      Featurize sleep vote filtering with SLEEP_DIAG_OKTS_SIG to
                   indicate
                   support for new sleep API
02/01/09   vg      Made trace buffer mutex consistent between event's and
                   F3 msg. (Updated msg_init to be early then event_init)
01/19/09   is      Switch to new sleep voting interface
01/12/09   as      Fixed thread synchronization in sleep vote function.
01/08/09   as      Fixed deadlock and concurrency control issues.
01/07/09   is      Do not invoke sleep and clock regime code if sleep vote is
                   not changing
10/3/08    VG      Changed PACKED to PACK() fixed issue casting uint32 to int.
                   This caused incorrect behavior with gcc compiler where int is
           defined as 32 bit vs. 16 bits on arm compiler.
08/21/08   JV      Moved the check for DIAG_COMM_CLOSE_SIG, DIAG_COMM_OPEN_FWD_SIG
                   and DIAG_COMM_OPEN_SIG from diag_handle_sigs() to diag_wait().
02/05/08   pc      Added support for dynamic threshold and sleep values
01/20/08   pc      Added DIAG_EVENT_PEND_TIMER_SIG to busy_mask in diag_wait()
12/16/07   pc      Added support for diag_dtr_enable and diag_dtr_changed_cb.
09/25/07   ms      Removing diagdsm_init() call for WinMob. Removing dsm_init().
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

#include "comdef.h"

#include "customer.h"


#include <string.h>

#include "diag_v.h"
#include "diagbuf.h"
#include "diagbuf_v.h"
#include "diagcomm_v.h"
#include "diagi_v.h"
#include "diagpkt.h"
#include "diagdiag_v.h"
#include "diagtarget.h"
#include "diagtune.h"
#include "diag_cfg.h"
#include "osal.h"
#include "err.h"
#include "event.h"
#include "log.h"
#include "msg.h"
#include "msgi.h" /* For diagcomm_ctrl_send_msg_mask() */
#include "eventi.h" /* For diagcomm_ctrl_send_event_mask() */
#include "crc.h"
#include "memheap.h"
#include "queue.h"
#include "diagcomm_cmd.h"
#include "diagcomm_ctrl.h"
#include "diagcomm_io.h"
#include "rcevt.h"
#include "diag_mode_v.h" /* For diagcomm_tx_mode_init() */
#include "time_svc.h" /* for time_get */
#include "diagcomm_io_socket.h"
#include "diag_diagIDi.h"
#include "diag_shared_i.h"

#ifdef DIAG_FORCE_DIAGID
extern uint8 diag_diagID_use_local;
#endif


#if defined (DIAG_QDSP6_APPS_PROC)
  #include "diagstub.h"
#else
  #include "sleep.h"
  #include "assert.h"

  #if defined (DIAG_DOG_SUPPORT)
    #include "dog.h"
  #endif
#endif /* DIAG_QDSP6_APPS_PROC */

#include "stringl.h" /* For strlcpy() */

#if defined (FEATURE_DIAG_NV)
#include "nv.h"
#include "diagnv_v.h"
#endif

#if defined (DIAG_TOOLSDIAG_INIT)
#include "toolsdiag.h"
#endif



#if ((defined(DIAG_RUNTIME_DEVMAP))||(defined(FEATURE_DIAG_SWITCH)))
  #include "rdevmap.h"
#endif

#include "diagpkti.h"
#include "diag_stm.h"
#ifdef DIAG_UIMAGE_MODE
#include "micro_diagbuffer.h"
#endif

#ifdef DIAG_UIMAGE_MODE
#include "time_svc.h" /* for time_get_sum_genoff_ts() */

#include "uSleep_mode_trans.h" /* for uSleep_registerNotificationCallback() */
#include "uSleep_core_npa.h" /* for node definitions */
#include "npa.h"  /* for npa_client_handle */
#endif 

#include "ULogFront.h"
ULogHandle diag_err_log_handle = NULL;

/* -------------------------------------------------------------------------
** Definitions and Declarations
** ------------------------------------------------------------------------- */

#ifdef DIAG_UIMAGE_MODE
npa_client_handle g_micro_diagbuffer_client = NULL;
#endif

extern diag_diagID_pname_type*  diag_diagID_pname_list;

static diag_generic_log_type diag_debug_log_buff[DIAG_DEBUG_LOG_SIZE];/*To keep track of the KW issue error cases, it is used to replace the ASSERT() which caused KW issues */
static uint8 diag_debug_log_buff_index;
static osal_mutex_arg_t diag_debug_log_buff_cs; /*Used to provent diag_debug_log_buff from being accessed by more than one thread*/

uint8 diag_cur_preset_id = DIAG_PRESET_MASK_1;
uint8 diag_max_presets[DIAG_MAX_STREAM_ID] = {DIAG_MAX_PRESET_ID, DIAG_MAX_DCI_ID};

/****************************************************************************** 
  Bit-mask used to re-send the control packets. Initially all the bits are in reset state - 0.
  And will be set to 1 coresspond to the failed control packets those need to be re-sent
  while handling a DIAG_INT_RESEND_CTRL_SIG signal.
   
  Bit  - Control Packet
  0    - DIAG_CTRL_MSG_FEATURE_BIT_POS	
  1    - DIAG_CTRL_MSG_NUM_PRESETS_BIT_POS
  2    - DIAG_CTRL_MSG_REG_BIT_POS
  3    - 
  4    - DIAG_CTRL_BT_EVENT_ID_BIT_POS 
  5    - DIAG_CTRL_BT_LOG_RANGE_BIT_POS 
  6    - DIAG_CTRL_BT_MSG_RANGE_BIT_POS
  7    - DIAG_CTRL_BT_MSG_MASK_BIT_POS
  8    - 
  9    - 
********************************************************************************/

uint16 diag_resend_ctrlpkt_mask = 0x0000;  
static unsigned int diag_drain_timer_len = DIAG_DRAIN_TIMER_LEN; /* Set default value */
static unsigned int diag_enable_sleep_voting = 1; /* by default, enable diag sleep voting. Can be disabled by a ctrl message */

/*Used to check if the mask notification was sent out to the registered users or not*/
uint8 diag_mask_notify = 0;

//This is the pointer to the struct which encapsulates diagmpd_buf and event_struct pointer. 
//This is needed since we cannot store pointer to a rootpd struct inside diagmpd_buf which is in UserPD.
diag_mpd_ptr_struct * diag_rootpd_struct_ptr;

/*diag_mask_change_notify_timer callback function */
void diag_mask_notify_timer_cb(uint32 params);

extern diagcomm_io_conn_type diagcomm_io_conn[NUM_PORT_TYPES][NUM_REMOTE_PORTS]; /* From diagcomm_cfg.c */

/* Timer to wake Diagnostic task so that it can set
** its Watchdog Task Report Signal.
*/
#if defined (DIAG_DOG_SUPPORT)

dog_report_type diag_dog_rpt_var = 0;
uint32 diag_dog_rpt_time_var = 0xffff;

osal_timer_t diag_rpt_timer;
#endif
extern boolean diag_real_time_mode; /*from diagpkt.c to track diag mode */

extern boolean diag_services_initialized;
static int diag_services_initialized_begin = FALSE;

osal_timer_t diag_cmd_read_tout_timer;
/* Timer to use when a bad SPC has been received.
*/
static osal_timer_t diag_tout_timer;

osal_timer_t diag_sio_tout_timer;

/* Flag will be set to TRUE if diag times out on SIO callback
** Flag will be cleared if SIO callback is called
*/
static boolean diag_flush_timeout = FALSE;

/* Diag drain timer.  If this timer expires, the current buffer will
   be sent to SIO */

static osal_timer_t diag_drain_timer;

static osal_timer_t diag_rx_rsp_notify_timer;
static osal_timer_t diag_rx_delay_rsp_notify_timer;

/*Timer used to wait for a certain amount of time before sending
  out the notifications to users about diag mask change*/
osal_timer_t diag_mask_change_notify_timer;

#ifdef FEATURE_DIAG_SWITCH
  int diag_on_single_proc;
#endif

/* Request packets are sent from the DM to the mobile.  Request packet is
** reconstructed from pieces received from SIO.
*/
static uint8 diag_req_inbound[DIAG_MAX_RX_PKT_SIZ];

/* This timer is used to notify DIAG that a client has submitted data to TX.
   Using a timer instead of rex_set_sigs() prevents a context switch in case
   the client calls from within INTLOCK(). */
static osal_timer_t diag_tx_notify_timer;
boolean diag_tx_pending = TRUE;  /* Init to TRUE until init in diag_init(). */
boolean diag_drain_timer_pending = TRUE;
boolean diag_event_stale_timer_pending = TRUE;
boolean diag_event_stale_timer_pending_sec = TRUE;

static dword diag_internal_sigs = 0; /* Internal Diag sigs */

/* Mutex for diag_internal_sigs */
static osal_mutex_arg_t diag_internal_sig_mutex;

/* Diag state variables.
*/
static diag_security_type diag_security = DIAG_SEC_LOCKED;
static diag_sp_type diag_sp = DIAG_SPC_LOCKED;


/* Connectivity state.  If non-zero, something is using DIAG. */
uint32 diag_cx_state = DIAG_CX_NONE_S;

/* Definition of a constant that is the same as an unwritten security code.
*/
#if !defined DIAG_QDSP6_APPS_PROC
static const uint8 diag_no_security_code_c[] = { 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff
};
#endif

/*-------------------------------------------------------------------
** Dynamic Memory Support (heap)
**    Definitions and data to support rex_malloc/realloc/calloc/free
**-----------------------------------------------------------------*/
#ifndef DIAG_HEAP_SIZE
  /* The size of the REX memory buffer in bytes. */
  #define DIAG_HEAP_SIZE 2048 * 4
#endif
/* The DIAG heap in REX. */
static mem_heap_type diag_heap;
static mem_heap_type diag_event_sec_heap;

/* An upper bound on the sizes of the biggest available blocks in the
** diag heap.
*/
#define DIAG_FREEBLOCK_CACHE_SIZE 2
static unsigned int diag_freeblock_info[DIAG_FREEBLOCK_CACHE_SIZE];

/* An upper bound on the sizes of the biggest available blocks in the
** diag event heap.
*/
#define DIAG_EVENT_FREEBLOCK_CACHE_SIZE 2

static unsigned int diag_event_freeblock_info_sec[DIAG_EVENT_FREEBLOCK_CACHE_SIZE];
/* Is the freeblock cache for the diag heap valid or not?
*/
static int diag_heap_cache_valid = 0;

/* Is the freeblock cache for the diag event heap valid or not?
*/
static int diag_event_sec_heap_cache_valid = 0;

/* Minimum chunk size for the diag heap.
*/
static unsigned int diag_heap_chunk_size;

/* Minimum chunk size for the diag event heap.
*/
static unsigned int diag_event_sec_heap_chunk_size;

/* The DIAG memory buffer used by the heap manager. */
static char diag_heap_buf[DIAG_HEAP_SIZE];


static char diag_event_sec_heap_buf[DIAG_EVENT_SEC_HEAP_SIZE];

/* Idle processing table.  If fp != NULL, the function will be called when
   the DIAG task is idle (nothing else to do). */
typedef struct
{
  diag_idle_processing_ftype fp;
  void *param;
}
diag_idle_proc_tbl_type;

diag_idle_proc_tbl_type diag_idle_proc_tbl[DIAG_IDLE_PROC_CNT];

/* To track the time api to use */
diag_time_api_types diag_time_api_to_use = 0;

/* Variable related to the time API configuration */
boolean diag_time_initialized = FALSE;  /* Track if the diag time API is initialized */
static int diag_time_init_begin = FALSE;

/* Function pointer to update the time API to be used */
uint8 (*diag_time_get)(qword);

extern void diag_svc_change_notify(int Svc);

extern boolean diag_gpio_init( void );

osal_tcb_t diag_fwd_task_tcb;

static void diag_idle_processing (void);

extern void diagpkt_init_manual_dispatch_table (void);

extern void diagpkt_rsp_q_init(void);

extern void diagpkt_delay_rsp_q_init(void);

void diag_mask_change_notify(void);

extern void diag_send_buf_retry(diagcomm_io_channel_type ch_type, 
            diagcomm_enum_port_type port_type, diag_tx_mode_type* tx_mode_ptr);

#if defined (DIAG_FEATURE_EFS2)
/* ----------------------------------------------------------------------------
  "Diagnostics Filestream Mode"

  The diagnostic task filestream mode reads a file to serve as an input
  stream in lieu of an active comm layer.  The output of the diagnostic
  service is buffered in diag's output buffer manager (diagbuf.c).
  When a connection is activated via the comm layer and a valid inbound packet
  is detected, the buffered output is flushed to the comm layer,
  thus capturing all diagnostic data generated after initialization.

  This mode continues until one of the following conditions occur:

  (1) Communication is initiated via the comm layer (external device),
      triggering the filestream mode to flush its output buffer to the
      comm layer.
  (2) The inactivity timer expires, stopping this mode and turning off all
      diag services.

  NOTE: The diag task (diag.c) is responsible for detecting these conditions
  and calling this API appropriately.

  The input file is written by the external device during the prior instance
  of the target running.  The diag task calls diag_fs_init() at startup,
  detecting the presence of the input file.  Once the entire input file
  stream is processed, it is deleted.

Noteworthy features:

  This mode is enabled only if the input file is present.

  Once the input file is processed, it is deleted.  This is done to avoid
  the performance loss of running the diagnostic task every boot cycle.
  If this were ever enabled in a production phone, for example, it would
  run the diag service every time the user turns a phone on, impacting battery
  life.  It also prevents legacy tools from failing due to lack of support
  for streaming and thie feature.
---------------------------------------------------------------------------- */
#include "fs_public.h"

/* Diag filestream type: contains file information as well as position for
   reading and max size for writing.
   ASSUMPTION: Writing is always done at the end of the file. */

//TODO: No longer needs to be global
typedef struct
{
  dword read_pos;   /* position for reading */
  dword write_pos;  /* position for writing */
  unsigned int max_size;        /* max file size */
  int handle;
  char *fname;
}
diag_fs_type;

static diag_fs_type diag_fs_config = {
  0,
  0,
  0xFFFFFFFFU,          /* Unused field */
  -1,
  ".DIAGCFG"
};

static diag_fs_type diag_fs_in = {
  0,
  0,
  0xFFFFFFFFU,          /* Unused field */
  -1,
  ".DIAGIN"
};



#if !defined (DIAG_FS_INBUF_MAX_SIZE)
#define DIAG_FS_INBUF_MAX_SIZE (16 * 1024)
#endif

static diag_fs_type diag_fs_inbuf = {
  0,
  0,
  DIAG_FS_INBUF_MAX_SIZE,
  -1,
  ".DIAGINTMP"
};

/* Prototypes for filestream mode */
static void diag_fs_init (diag_fs_type * fs);
static boolean diag_fs_inbound_mode (void *buf, unsigned int size);
static uint32 diag_fs_read (diag_fs_type * fs, void *buf, unsigned int size);
static boolean diag_fs_write (diag_fs_type * fs, void *buf,
                  unsigned int size);
static void diag_fs_process_inbound (diag_fs_type * fs, void *buf,
                     unsigned int size);
static void diag_fs_terminate (diag_fs_type * fs);
static void diag_fs_inbound_comm (diag_fs_type * fs, void *buf,
                  unsigned int size);
static void diag_fs_process_config (void *req_pkt, word pkt_len);
static void diag_fs_write_config (void);
static void diag_process_input_file (diag_fs_type * fs,void *buf,
                  unsigned int size);



#endif /* DIAG_FEATURE_EFS2 */


typedef struct
{
  /* (Input) raw (HDLC encoded) stream buffer.  This is never changed
     by the packet extraction/decoding functions. */
  byte *src_ptr;

  /* (I/O) The current byte array index of src_ptr.
     When src_idx == src_size, the src_ptr buffer is complete. */
  unsigned int src_idx;

  /* (Input) This is the number of bytes to convert from src_ptr. */
  unsigned int src_size;

  /* NOTE: Since a packet can cpan mulitple source blocks, dest_idx
     must be stored to maintain the state of the packet extraction.
     For src_ptr, this is not necessary. */

  /* (Input) The extracted packet.  This is never changed
     by the packet extraction/decoding functions. */
  byte *dest_ptr;

  /* (I/O) The array index into dest_ptr of the next unwritten byte.
     Upon successful decode of a packet, this field is the length. */
  unsigned int dest_idx;

  /* This is the number of bytes that can be stored in dest_ptr. */
  unsigned int dest_size;

  /* Used internally: Are we in the middle of an escape code?
     This boolean is needed in order to be able to handle the
     case where an escape code straddles two packets. */
  boolean escaping;

}
diag_hdlc_decode_type;

/* Q and Q types for diag_cmd_req () */
typedef struct
{
  q_link_type link;

  diag_cmd_rsp rsp_func;
  void *param;
  boolean bFromDiagCmdQ;
  unsigned int length;
  byte req[1024];             /* sized by the 'length' field */
}
diag_req_cmd_type;

static q_type diag_req_cmd_q;

extern diagcomm_io_conn_type diagcomm_io_ctrl_conn[NUM_PORT_TYPES][NUM_REMOTE_PORTS]; /* From diagcomm_io.c */


/* -------------------------------------------------------------------------
** Forward Declarations
** ------------------------------------------------------------------------- */
boolean diag_check_password (void *code_ptr);
void diag_rx_notify (void);

#ifdef FEATURE_DIAG_SWITCH
static boolean
diag_get_rx_pkt_slave (diag_rx_pkt_type* rx_pkt);
static boolean diag_get_rx_pkt (uint8 * dest_ptr, unsigned int size,
                unsigned int *pkt_len);
#else
 #if !defined (DIAG_FRAMING)
static boolean
diag_get_rx_pkt_slave (diag_rx_pkt_type* rx_pkt);
  #else
    static boolean diag_get_rx_pkt (uint8 * dest_ptr, unsigned int size,
                unsigned int *pkt_len);
  #endif
#endif
void diag_handle_internal_sigs( void );
static osal_sigs_t diag_handle_sigs (osal_sigs_t mask);
#if ((!defined(FEATURE_DIAG_SWITCH) && defined(DIAG_FRAMING)) || (defined(FEATURE_DIAG_SWITCH))|| defined(DIAG_FEATURE_EFS2))
static boolean diag_extract_pkt (diag_hdlc_decode_type * hdlc);
static boolean diag_hdlc_decode (diag_hdlc_decode_type * hdlc);
#endif
static void diag_heap_init (void);
void diag_rx_rsp_notify( void );
void diag_rx_delay_rsp_notify( void );

uint32 diag_listeners_alloc_cnt = 0;
uint32 diag_listeners_drop_cnt = 0;
uint32 diag_listeners_free_cnt =0;
uint16 diag_active_listeners_cnt = 0;

#ifdef FEATURE_DEBUG_DIAG_HEAP
uint32 diag_blocking_malloc_sucess_cnt =0;
uint32 diag_malloc_sucess_cnt =0;
uint32 diag_free_cnt =0;
uint32 diag_process_rx_free_cnt = 0;
uint32 diag_searchlist_add_malloc_cnt = 0;
uint32 diag_searchlist_add_free_cnt = 0;
uint32 diag_searchlist_delete_malloc_cnt = 0;
uint32 diag_searchlist_delete_free_cnt = 0;
uint32 diag_cmd_req_cnt = 0;
uint32 diag_fs_process_inbound_free_cnt = 0;
uint32 diag_fs_process_inbound_malloc_cnt = 0;
uint32 diag_event_malloc_sec_sucess_cnt =0;
uint32 diag_event_free_sec_cnt =0;

uint32 diag_heap_used_bytes(void)
{
  return diag_heap.used_bytes;
}

/*===========================================================================

FUNCTION DIAG_EVENT_HEAP_USED_BYTES

DESCRIPTION
  Returns the number of used bytes by the specific heap.

PARAMETERS
  ptr - mem_heap_type ptr for the PD(root/userpd)

DEPENDENCIES
  mem heap structure should be initialised.

RETURN VALUE
  Number of used bytes in the heap.

===========================================================================*/
uint32 diag_event_heap_used_bytes(mem_heap_type * ptr)
{
  if(ptr!=NULL)
{
    return ptr->used_bytes;
  }
  return 0;
}

uint32 diag_event_sec_heap_used_bytes(void)
{
  return diag_event_sec_heap.used_bytes;
}
#endif /* FEATURE_DEBUG_DIAG_HEAP */

static void diag_process_rx (void);
extern diagcomm_io_conn_type diagcomm_io_ctrl_conn[NUM_PORT_TYPES][NUM_REMOTE_PORTS];
extern diagcomm_io_conn_type diagcomm_io_cmd_conn[NUM_PORT_TYPES][NUM_REMOTE_PORTS];

#include <stdlib.h>

static osal_mutex_arg_t diag_srch_list_mutex;
osal_mutex_arg_t diag_sleep_vote_mutex; /*used in diagpkt.c */

osal_mutex_arg_t diag_idle_process_mutex;
osal_mutex_arg_t diag_flow_control_mutex;

 void diag_search_list_cs_init(void)
 {
    int return_val;

    diag_srch_list_mutex.name = "MUTEX_DIAG_SRCH_LIST_CS";
    return_val = osal_init_mutex(&diag_srch_list_mutex);
    ASSERT(OSAL_SUCCESS == return_val);
 }

 void diag_sleep_vote_cs_init(void)
 {
    int return_val;

    diag_sleep_vote_mutex.name = "MUTEX_DIAG_SLEEP_VOTE_CS";
    return_val = osal_init_mutex(&diag_sleep_vote_mutex);
    ASSERT(OSAL_SUCCESS == return_val);
 }

  void diag_flow_control_mutex_init(void)
 {
    int return_val;

    diag_flow_control_mutex.name = "MUTEX_DIAG_DATA_FLOW_CTRL";
    return_val = osal_init_mutex(&diag_flow_control_mutex);
    ASSERT(OSAL_SUCCESS == return_val);
 }

 void diag_int_sig_cs_init(void)
 {
    int return_val;

    diag_internal_sig_mutex.name = "MUTEX_DIAG_INT_SIG_CS";
    return_val = osal_init_mutex(&diag_internal_sig_mutex);
    ASSERT(OSAL_SUCCESS == return_val);
 }

extern boolean diagpkt_rsp_send (void);
extern boolean diagpkt_delay_rsp_send (void);

/*Indicates diag task initialization started */
static int diag_task_begin = FALSE;

/* -------------------------------------------------------------------------
** Function Implementations
** ------------------------------------------------------------------------- */

#ifdef DIAG_UIMAGE_MODE
/* Variable to store time offset when phone goes in uImage mode. Value of this variable
   is used in micro_diagbuffer_drain.c at the time of draining the packet to communication
   layer
*/
time_type diag_uimage_time_offset = { 0, 0 }; 
boolean   diag_uimage_is_time_offset_negative = FALSE;

/*===========================================================================

FUNCTION DIAG_UIMAGE_ENTRY_EXIT_CB

DESCRIPTION
  uSleep framework calls this call back when system goes in-n-out of uImage mode.
  
PARAMETERS
  uSleep_state_notification notification_state : notification state returned by
                                                 uSleep code.
 USLEEP_STATE_ENTER - Means system is entering uSleep mode
 USLEEP_STATE_EXIT  - Means system is exiting uSleep mode
  
DEPENDENCIES
  uSleep
  Timer  
  
RETURN VALUE
  None
  
===========================================================================*/

void diag_uimage_entry_exit_cb(uSleep_state_notification notification_state)
{
  if (notification_state == USLEEP_STATE_ENTER)
  {
    /* Call get time offset API which will populate diag_uImage_timer_offset  */
    diag_uimage_is_time_offset_negative = time_get_sum_genoff_ts(diag_uimage_time_offset);
    //Narayana - To Do
    diag_uimage_is_time_offset_negative = FALSE ;
  }
  else if (notification_state == USLEEP_STATE_EXIT)
  {
    int return_val = 0;
    osal_sigs_t return_sigs;
    return_val = osal_set_sigs(&diag_tcb, DIAG_MICRO_DIAGBUFFER_UIMAGE_SIG, &return_sigs);

    ASSERT(OSAL_SUCCESS == return_val);
  }
  else
  {
    /* There cannot be any other state than ENTER and EXIT */
    MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_uimage_entry_exit_cb: Other than uImage entry/exit state.");
  }
}
#endif /* DIAG_UIMAGE_MODE */
/* ==================================================================
FUNCTION DIAG_HEAP_INIT
DESCRIPTION
   Initializes the heap that REX uses to dole out memory.
===================================================================== */
static void
diag_heap_init (void)
{
  mem_init_heap (&diag_heap, diag_heap_buf, sizeof (diag_heap_buf), NULL);

  (void) mem_heap_set_int_lock (&diag_heap);

  diag_heap_chunk_size = mem_heap_get_chunk_size (&diag_heap);
  return;
}               /* END diag_heap_init */

/*===================================================================*/
static boolean diag_heap_block = FALSE;

void *
diag_blocking_malloc (uint32 num_bytes)
{
  void *buf = NULL;

  if (!diag_heap_cache_valid || diag_alloc_check (1, (uint32 *)&num_bytes))
  {
    buf = mem_malloc (&diag_heap, num_bytes);
  }

  /* If unsuccessful, block other clients from allocating.
     Remove block if successful. */
  diag_heap_block = (buf) ? FALSE : TRUE;

  /* If the allocation failed, update the freeblock cache if it is not
  ** currently valid.
  */
  if (buf == NULL)
  {
    if (!diag_heap_cache_valid)
    {
      mem_heap_get_freeblock_info (&diag_heap, DIAG_FREEBLOCK_CACHE_SIZE,
        diag_freeblock_info);
      diag_heap_cache_valid = 1;
    }
  }
  else
  {
    diag_heap_cache_valid = 0;

#ifdef FEATURE_DEBUG_DIAG_HEAP
    diag_blocking_malloc_sucess_cnt++;
    MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
            "<<DIAG BLOCKING MALLOC sucess_cnt = %d, diag heap used = %d ",
                    diag_blocking_malloc_sucess_cnt,diag_heap_used_bytes());
#endif /* FEATURE_DEBUG_DIAG_HEAP */
  }
  return buf;
}

/*===================================================================*/
void *
diag_malloc (unsigned long num_bytes)
{
  void *buf = NULL;

  if (!diag_heap_block)
  {
    if (!diag_heap_cache_valid || diag_alloc_check (1, (uint32 *)&num_bytes))
    {
       buf = mem_malloc (&diag_heap, num_bytes);
    }
  }

  /* If the allocation failed, update the freeblock cache if it is not
  ** currently valid.
  */

  if (buf == NULL)
  {
    if (!diag_heap_cache_valid)
    {
      mem_heap_get_freeblock_info (&diag_heap, DIAG_FREEBLOCK_CACHE_SIZE,
        diag_freeblock_info);
      diag_heap_cache_valid = 1;
    }
  }
  else
  {
    diag_heap_cache_valid = 0;

#ifdef FEATURE_DEBUG_DIAG_HEAP
    diag_malloc_sucess_cnt++;
    MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
            "<<DIAG MALLOC diag_malloc_sucess_cnt = %d,  heap used = %d ",
                            diag_malloc_sucess_cnt, diag_heap_used_bytes());
#endif /* FEATURE_DEBUG_DIAG_HEAP */
  }
  return buf;
} /* END diag_malloc */


int
diag_alloc_check (unsigned int num_allocs, uint32 *alloc_sizes )
{
  unsigned int i;
  unsigned int j;
  unsigned int cur_req_size;
  unsigned int best_match_size;
  unsigned int best_match_index;
  unsigned int freeblocks[DIAG_FREEBLOCK_CACHE_SIZE];

  /* Return 0 if num_allocs is greater than free block cache size */
  if (num_allocs > DIAG_FREEBLOCK_CACHE_SIZE)
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_alloc_check: num_allocs %d greater than freeblock cache",num_allocs );
     return 0;
  }

  /* Return 1 if the freeblock cache is dirty.
  */
  if (!diag_heap_cache_valid)
    return 1;

  memscpy (freeblocks, sizeof(freeblocks), diag_freeblock_info,
    DIAG_FREEBLOCK_CACHE_SIZE * sizeof (int));

  /* Match each requested size up with the closest available size that
  ** is greater than the requested size. Update the freeblock information
  ** after each match. If even one requested size cannot be matched up,
  ** then the heap does not contain enough space for all the requests.
  */
  for (i = 0; i < num_allocs; i++)
  {
    cur_req_size = ((alloc_sizes[i] + sizeof (mem_block_header_type)
      + diag_heap_chunk_size - 1) / diag_heap_chunk_size)
      * diag_heap_chunk_size;
    best_match_size  = 0x7fffffff;
    best_match_index = DIAG_FREEBLOCK_CACHE_SIZE;
    for (j = 0; j < DIAG_FREEBLOCK_CACHE_SIZE; j++)
    {
      if ((freeblocks[j] >= cur_req_size) &&
          (freeblocks[j] <  best_match_size))
      {
        best_match_size  = freeblocks[j];
        best_match_index = j;
      }
    }
    if (best_match_index < DIAG_FREEBLOCK_CACHE_SIZE)
      freeblocks[best_match_index] -= cur_req_size;
    else
      return 0;
  }

  return 1;
} /* END diag_alloc_check */


/*===================================================================*/
void
diag_free(void *mem_ptr)
{
  mem_free (&diag_heap, mem_ptr);
#ifdef FEATURE_DEBUG_DIAG_HEAP
  diag_free_cnt++;
  MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
        ">> DIAG FREE diag_free_cnt = %d, diag heap used = %d ",
                        diag_free_cnt,diag_heap_used_bytes());
#endif /* FEATURE_DEBUG_DIAG_HEAP */
  diag_heap_cache_valid = 0;
  return;
} /* END diag_free */

/* ==================================================================
FUNCTION DIAG_EVENT_HEAP_INIT
DESCRIPTION
   Initializes the heap to be used by rootpd as well as userpd events.
   Each PD has its own heap.
===================================================================== */
void
diag_event_heap_init (event_struct * ptr, int heap_size)
{
  mem_init_heap (&(ptr->diag_event_heap), ptr->diag_event_heap_buf, heap_size, NULL);
  (void) mem_heap_set_int_lock (&(ptr->diag_event_heap));
  return;
}               /* END diag_event_heap_init */


/*===================================================================*/
/*===================================================================*/
void *
diag_listeners_malloc (unsigned long num_bytes)
{
  void *buf = NULL;

  if( diag_active_listeners_cnt >= DIAG_MAX_ACTIVE_LISTENERS )
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_FATAL, "Limit on active listeners reached, dropping the listeners");
    diag_listeners_drop_cnt++;
  }

  else
  {
    /*memory allocated from system heap */
    buf = malloc(num_bytes);

    if (buf)
    {
        if(diag_listeners_alloc_cnt == MAX_VALUE_UINT32)
        {
            MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Variable diag_listeners_alloc_cnt has reached maximum value.");
        }
        else
        {
            diag_listeners_alloc_cnt++;
            MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Memory allocated for a diag listener");
            diag_active_listeners_cnt++;
        }
    }
    else
    {
        diag_listeners_drop_cnt++;
        MSG(MSG_SSID_DIAG, MSG_LEGACY_FATAL, "Unable to allocate memory from system heap");
    }
  }

  return buf;
} /* END diag_listeners_malloc */

/*===================================================================*/
void
diag_listeners_free(void *mem_ptr)
{

  free (mem_ptr);
  if(diag_listeners_free_cnt == MAX_VALUE_UINT32)
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Variable diag_listeners_free_cnt has reached maximum value.");
  }
  else
  {
    diag_listeners_free_cnt++;
    diag_active_listeners_cnt--;
  }

  return;
} /* END diag_listeners_free */


/*===================================================================*/
void *
diag_event_malloc (unsigned long num_bytes,event_struct *ptr)
{
  void *buf = NULL;

  if(ptr!=NULL)	
  {
    buf = mem_malloc (&(ptr->diag_event_heap), num_bytes);
  if (buf == NULL)
  {
      MSG_1(MSG_SSID_DIAG, MSG_LEGACY_LOW,"diag_event_heap_malloc allocation failed: %d",num_bytes);
  }
  else
  {
    #ifdef FEATURE_DEBUG_DIAG_HEAP
	  ptr->diag_event_malloc_success_cnt++;
      MSG_SPRINTF_3(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
         "<<DIAG MALLOC heap_name=%s diag_event_malloc_success_cnt = %d,  event_heap used = %d ",ptr->diag_event_heap.heapName,ptr->diag_event_malloc_success_cnt, diag_event_heap_used_bytes(&(ptr->diag_event_heap)));
    #endif /* FEATURE_DEBUG_DIAG_HEAP */
  }
  }
  return buf;
} /* END diag_event_malloc */

/*===================================================================*/
void
diag_event_free(void *mem_ptr,event_struct *ptr)
{

  if(ptr!=NULL)
  {
    mem_free (&(ptr->diag_event_heap), mem_ptr);
  #ifdef FEATURE_DEBUG_DIAG_HEAP
    ptr->diag_event_free_cnt++;
    MSG_SPRINTF_3(MSG_SSID_DIAG, MSG_LEGACY_HIGH,">> DIAG FREE heap_name=%s diag_event_free_cnt = %d, diag event heap used = %d ",
                            ptr->diag_event_heap.heapName,ptr->diag_event_free_cnt,diag_event_heap_used_bytes(&ptr->diag_event_heap));
  #endif /* FEATURE_DEBUG_DIAG_HEAP */
  }
  return;
} /* END diag_event_free */

static void
diag_event_sec_heap_init (void)
{
  mem_init_heap (&diag_event_sec_heap, diag_event_sec_heap_buf, sizeof (diag_event_sec_heap_buf), NULL);
  (void) mem_heap_set_int_lock (&diag_event_sec_heap);
  diag_event_sec_heap_chunk_size = mem_heap_get_chunk_size (&diag_event_sec_heap);
  return;
}               /* END diag_event_sec_heap_init */


/*===================================================================*/
void *
diag_event_malloc_sec (unsigned long num_bytes)
{
  void *buf = NULL;

  if (!diag_event_sec_heap_cache_valid || diag_event_alloc_check_sec (1, (uint32 *)&num_bytes))
  {
     buf = mem_malloc (&diag_event_sec_heap, num_bytes);
  }

  /* If the allocation failed, update the freeblock cache if it is not
  ** currently valid.
  */

  if (buf == NULL)
  {
    if (!diag_event_sec_heap_cache_valid)
    {
      mem_heap_get_freeblock_info (&diag_event_sec_heap, DIAG_EVENT_FREEBLOCK_CACHE_SIZE,
        diag_event_freeblock_info_sec);
      diag_event_sec_heap_cache_valid = 1;
    }
  }
  else
  {
    diag_event_sec_heap_cache_valid = 0;

    #ifdef FEATURE_DEBUG_DIAG_HEAP
    diag_event_malloc_sec_sucess_cnt++;
    MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
    " <<DIAG MALLOC diag_event_malloc_sucess_cnt = %d,  event_heap used = %d ",
            diag_event_malloc_sec_sucess_cnt, diag_event_sec_heap_used_bytes());
    #endif /* FEATURE_DEBUG_DIAG_HEAP */
  }
  return buf;
} /* END diag_event_malloc */


int
diag_event_alloc_check_sec (unsigned int num_allocs, uint32 *alloc_sizes )
{
  unsigned int i;
  unsigned int j;
  unsigned int cur_req_size;
  unsigned int best_match_size;
  unsigned int best_match_index;
  unsigned int freeblocks[DIAG_EVENT_FREEBLOCK_CACHE_SIZE];

  ASSERT (num_allocs <= DIAG_EVENT_FREEBLOCK_CACHE_SIZE);

  /* Return 1 if the freeblock cache is dirty.
  */
  if (!diag_event_sec_heap_cache_valid)
    return 1;

  memscpy (freeblocks, sizeof(freeblocks),
          diag_event_freeblock_info_sec, DIAG_EVENT_FREEBLOCK_CACHE_SIZE * sizeof (int));

  /* Match each requested size up with the closest available size that
  ** is greater than the requested size. Update the freeblock information
  ** after each match. If even one requested size cannot be matched up,
  ** then the heap does not contain enough space for all the requests.
  */
  for (i = 0; i < num_allocs; i++)
  {
    cur_req_size = ((alloc_sizes[i] + sizeof (mem_block_header_type)
      + diag_event_sec_heap_chunk_size - 1) / diag_event_sec_heap_chunk_size)
      * diag_event_sec_heap_chunk_size;
    best_match_size  = 0x7fffffff;
    best_match_index = DIAG_EVENT_FREEBLOCK_CACHE_SIZE;
    for (j = 0; j < DIAG_EVENT_FREEBLOCK_CACHE_SIZE; j++)
    {
      if ((freeblocks[j] >= cur_req_size) &&
          (freeblocks[j] <  best_match_size))
      {
        best_match_size  = freeblocks[j];
        best_match_index = j;
      }
    }
    if (best_match_index < DIAG_EVENT_FREEBLOCK_CACHE_SIZE)
      freeblocks[best_match_index] -= cur_req_size;
    else
      return 0;
  }

  return 1;
} /* END diag_event_alloc_check_sec */

/*===================================================================*/
void
diag_event_free_sec(void *mem_ptr)
{
  mem_free (&diag_event_sec_heap, mem_ptr);
  diag_event_sec_heap_cache_valid = 0;

  #ifdef FEATURE_DEBUG_DIAG_HEAP
  diag_event_free_cnt_sec++;
  MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH, ">> DIAG FREE diag_event_free_cnt = %d, diag event heap used = %d ",
      diag_event_free_cnt_sec,diag_event_sec_heap_used_bytes());
  #endif /* FEATURE_DEBUG_DIAG_HEAP */
  return;
} /* END diag_event_free */
/*===================================================================*/
void
diag_block (void)
{
  osal_sigs_t return_sigs;
  int return_val = 0,return_val_tout;

  do
  {
    return_sigs = osal_thread_wait (&diag_tcb, DIAG_BLOCK_SIG | DIAG_RPT_TIMER_SIG|DIAG_SIO_TOUT_TIMER_SIG);
    diag_kick_watchdog();

  } while( ((return_sigs & DIAG_BLOCK_SIG) == 0) && ((return_sigs & DIAG_SIO_TOUT_TIMER_SIG) == 0));

  /*Verify whether time out has occurred and set flag to TRUE */
  if((return_sigs & DIAG_SIO_TOUT_TIMER_SIG))
  {
    diag_flush_timeout = TRUE;
    MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"Diag timed out on SIO callback");
  }
  return_val_tout = osal_reset_sigs(&diag_tcb, DIAG_SIO_TOUT_TIMER_SIG, &return_sigs);
  return_val = osal_reset_sigs(&diag_tcb, DIAG_BLOCK_SIG, &return_sigs);
  ASSERT(OSAL_SUCCESS == return_val_tout);
  ASSERT(OSAL_SUCCESS == return_val);
  return;
} /* END diag_block */

/*===================================================================*/
void
diag_release (void)
{
  osal_sigs_t return_sigs;
  int return_val = 0,return_val_tout = 0;

  return_val = osal_set_sigs(&diag_tcb, DIAG_BLOCK_SIG, &return_sigs);
  return_val_tout = osal_reset_timer(&diag_sio_tout_timer);
  diag_flush_timeout = FALSE;
  ASSERT(OSAL_SUCCESS == return_val);
  ASSERT(OSAL_SUCCESS == return_val_tout);

  return;
} /* END diag_release */

/*===========================================================================

FUNCTION DIAG_EARLY_INIT

DESCRIPTION
  This function initializes the diagnostics message,log,event services
  If the clients wanted to call the diag apis before the diag task is
  initialized then this function needs to be called earlier
===========================================================================*/
void diag_early_init(void)
{
  int return_val = 0;
  if(!osal_atomic_compare_and_set((osal_atomic_word_t*)&diag_services_initialized_begin, FALSE, TRUE))
    return ;

  /* Required mutex and timer initializations */
  diag_int_sig_cs_init();
  diagpkt_mask_tbl_cs_init();
  diag_sleep_vote_cs_init();

  /*Timer used to wait for a certain amount of time before sending out the notifications to registered users about diag mask change*/
  return_val = osal_create_diag_timer_ex(&diag_mask_change_notify_timer, &diag_tcb, diag_mask_notify_timer_cb, 0);
  ASSERT(OSAL_SUCCESS == return_val);

  diagbuf_init ();

  msg_init ();

  /* Init event service */
  event_init ();

  /* Init log service */
  log_init ();

  diag_services_initialized = TRUE;

} /* diag_early_init */

/*===========================================================================

FUNCTION DIAG_INIT

DESCRIPTION
  This function initializes the SIO descriptors for the request and
  response buffers.  Initializes the msg services.  This is called prior to
  the task being started, so no assumptions about other tasks can be made.

===========================================================================*/
void
diag_init (void)
{
  int i, return_val = 0;

#ifdef DIAG_UIMAGE_MODE
  /* To check the return value of uSleep_registerNotificationCallback */
  uSleep_notification_cb_handle diag_uimage_entry_exit_handle = 0;
#endif
  diag_diagID_init();
  diag_init_feature_mask();


  ULogFront_RealTimeInit(&diag_err_log_handle,"DiagErrors", 4096, ULOG_MEMORY_LOCAL, ULOG_LOCK_OS);
  ULogFront_RealTimePrintf(diag_err_log_handle,0,"Root Diag Init");

  diag_sleep_init();

#if defined FEATURE_DIAG_STM
  // Initialize STM interface
  diag_stm_init();
#endif

  if(FALSE == diag_time_initialized)
  {
    (void)diag_time_init();
  }

#if defined (DIAG_DOG_SUPPORT)

    diag_dog_rpt_var = dog_register(&diag_tcb, DOG_DEFAULT_TIMEOUT, DOG_DEADLKCHK_ENABLE);

    diag_dog_rpt_time_var = dog_get_report_period(diag_dog_rpt_var);


    /* If the return value from dog_get_report_period() was zero, then we MUST
    NOT report to dog directly, as DOG will monitor us in a different manner.
    Hence we won't initialize or start any timer. */
    if (diag_dog_rpt_time_var > 0)
    {
        /*  Initialize watchdog report timer  */
        return_val = osal_create_timer(&diag_rpt_timer, &diag_tcb, DIAG_RPT_TIMER_SIG);
        ASSERT(OSAL_SUCCESS == return_val);
    }
#endif


  return_val = osal_create_diag_timer(&diag_tout_timer, &diag_tcb, DIAG_TOUT_TIMER_SIG);
  ASSERT(OSAL_SUCCESS == return_val);
  return_val = osal_create_diag_timer(&diag_cmd_read_tout_timer, &diag_tcb, DIAG_CMD_READ_TOUT_SIG);
  ASSERT(OSAL_SUCCESS == return_val);

  /* Timer used to notify DIAG that TX is pending from a client. */
  return_val = osal_create_diag_timer(&diag_tx_notify_timer, &diag_tcb, DIAG_TX_SIG);
  ASSERT(OSAL_SUCCESS == return_val);

  /* Timer used to notify DIAG that TX is pending from a client. */
  return_val = osal_create_diag_timer(&diag_drain_timer, &diag_tcb, DIAG_DRAIN_TIMER_SIG);
  ASSERT(OSAL_SUCCESS == return_val);

  /* Timer used to notify DIAG that RSP is pending from a client. */
  return_val = osal_create_diag_timer(&diag_rx_rsp_notify_timer, &diag_tcb, DIAG_RX_RSP_SIG);
  ASSERT(OSAL_SUCCESS == return_val);

  /* Timer used to notify DIAG that delayed RSP is pending from a client. */
  return_val = osal_create_diag_timer(&diag_rx_delay_rsp_notify_timer, &diag_tcb, DIAG_RX_DELAY_RSP_SIG);
  ASSERT(OSAL_SUCCESS == return_val);

  return_val = osal_create_diag_timer(&diag_sio_tout_timer, &diag_tcb, DIAG_SIO_TOUT_TIMER_SIG);
  ASSERT(OSAL_SUCCESS == return_val);


  /* reset the pending, enabling the mechanism. */
  diag_tx_pending = FALSE;
  diag_drain_timer_pending = FALSE;
  diag_event_stale_timer_pending = FALSE;
  diag_event_stale_timer_pending_sec = FALSE;


  /*Initialize diag_debug_log_buff*/
  memset(diag_debug_log_buff, 0, sizeof(diag_generic_log_type) * DIAG_DEBUG_LOG_SIZE);
  diag_debug_log_buff_index =0;

  /* Initialize diag_debug_log_buff mutex */
  diag_debug_log_buff_cs.name = "DIAG_DEBUG_LOG_BUFF_CS";
  return_val = osal_init_mutex(&diag_debug_log_buff_cs);
  ASSERT(OSAL_SUCCESS == return_val);
  
  #ifdef DIAG_UIMAGE_MODE
  /* register for callback for entry and exit notifications from uSleep */
  /* uSleep_registerNotificationCallback( uint32 enterLat, uint32 exitLat, callback);
     enterLat: Time in ticks taken to execute the CB when entering uImage
     exitLat:  Time in ticks taken to execute the CB when exiting uImage
     callback: Function to be called 
  */
  diag_uimage_entry_exit_handle = uSleep_registerNotificationCallback(400, 400, 
                                                                      diag_uimage_entry_exit_cb);
  if (!diag_uimage_entry_exit_handle)
  {
    //MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_init: uImage entry/exit call back notification registration failed");
     ERR_FATAL("diag_init: uImage entry/exit call back notification registration failed", 0, 0, 0);
  }
  else
  {
    /* Create a resource to use in diagbuf_drain() to vote against and for uImage entry.
       Once drain gets to run, it run to finish micro diagbuffer completely until some
       important signal is set that prempts it. For signal list see micro diagbuffer
       drain call.
    */
    g_micro_diagbuffer_client = npa_create_sync_client(USLEEP_CORE_DRIVER_NODE_NAME,
                                                       "uDiagbufferClient",
                                                       NPA_CLIENT_REQUIRED);
  }

#endif /* DIAG_UIMAGE_MODE */

  diagcomm_io_init();

  /* If the code in the phone is not programmed, the phone defaults to
     unlocked security. */
  if (diag_check_password (NULL))
  {
    diag_security = DIAG_SEC_UNLOCKED;
  }
  else
  {
    diag_security = DIAG_SEC_LOCKED;
  }

  diag_search_list_cs_init();
  diag_flow_control_mutex_init();
#if defined (DIAG_QDSP6_APPS_PROC)
  diagpkt_delayrspid_mutex_init();
#endif



  diag_heap_init ();

  diag_event_sec_heap_init();

  diag_early_init();
  /*Initialise the rootpd heap. 
  This is moved here after the diag_early_init as diag early init initialises diag_rootpd_struct_ptr*/
  diag_event_heap_init(diag_rootpd_struct_ptr->event_mpd_struct_ptr,DIAG_EVENT_HEAP_SIZE);

     /*initialize the mutex used while acessing diag_idl_proc_tbl*/
  diag_idle_process_mutex.name = "MUTEX_DIAG_IDLE_PROCESS";
  return_val = osal_init_mutex(&diag_idle_process_mutex);
  ASSERT(OSAL_SUCCESS == return_val);

  for (i = 0; i < DIAG_IDLE_PROC_CNT; i++)
  {
    diag_idle_proc_tbl[i].fp = NULL;
    diag_idle_proc_tbl[i].param = NULL;
  }

  /* Initialized the client command queue. */
  (void) q_init (&diag_req_cmd_q);
  /* Initialize the response queue */
  (void) diagpkt_rsp_q_init();
  /* Initialize the delay response queue */
  (void) diagpkt_delay_rsp_q_init();

#if defined (DIAG_TOOLSDIAG_INIT)
    toolsdiag_init ();
#endif

  diagdiag_init ();

 MSG (MSG_SSID_DIAG, MSG_LEGACY_LOW, "DIAG task initialized");

#ifdef DIAG_GPIO_INIT
  diag_gpio_init();
#endif

  /* Initialize buffering mode parameters. */
  diag_tx_mode_init();

}               /* diag_init */

/*===========================================================================

FUNCTION DIAG_WAIT

DESCRIPTION
  This procedure waits for specified signals to be set.  While it waits,
  this procedure continues to set the Watchdog report signal and reset
  the diag_rpt_timer.

  To avoid deadlock situations, this function also waits on the MC task
  sigs.  Since diag doesn't really care when power goes away, the func
  immediately ack's MC.

  Depends on previously having used the diag_rpt_timer to time when to
  set the watchdog signal.

===========================================================================*/
dword
diag_wait (dword mask)  /* Signals to wait for */
{
  return ((dword)diag_handle_sigs ((osal_sigs_t)mask));
}

/*===========================================================================

FUNCTION DIAG_RX_NOTIFY

DESCRIPTION
  This callback routine is called when data has been received and is ready
  be read by the diag task.

===========================================================================*/
void
diag_rx_notify (void)
{
  osal_sigs_t return_sigs;
  int return_val = 0;

  return_val = osal_set_sigs(&diag_tcb, DIAG_RX_SIG, &return_sigs);
  ASSERT(OSAL_SUCCESS == return_val);

} /* diag_rx_notify */

/*=========================================================================*/
void
diag_tx_notify (void)
{
  osal_sigs_t return_sigs;
  int return_val = 0;

  if (!diag_tx_pending)
  {
    diag_tx_pending = TRUE;

  /* The timer is used because some MSG macros in DMSS?AMSS are called from
     INTLOCK. If you signal DIAG directly, it will cause a context switch
     while in INTLOCK.
     By setting a timer, the signal won't occur until the next clock tick
     ISR. Therefore, no signal is set during the possible INTLOCK. */

#if defined (DIAG_QDSP6_APPS_PROC)
  return_val = osal_set_sigs(&diag_tcb, DIAG_TX_SIG, &return_sigs);
  ASSERT(OSAL_SUCCESS == return_val);
#else
  if( osal_ints_are_locked())
    {
      return_val = osal_set_timer(&diag_tx_notify_timer, 1);
      ASSERT(OSAL_SUCCESS == return_val);
    }
    else
    {
      return_val = osal_set_sigs(&diag_tcb, DIAG_TX_SIG, &return_sigs);
      ASSERT(OSAL_SUCCESS == return_val);
    }
#endif /* (IMAGE_APPS_PROC) && defined (IMAGE_QDSP6_PROC) */

  }
  return;
}
/*===========================================================================

FUNCTION DIAG_RX_RSP_NOTIFY

DESCRIPTION
  This routine sets the DIAG_RX_RSP_SIG

===========================================================================*/
void diag_rx_rsp_notify( void )
{
  osal_sigs_t return_sigs;
  int return_val = 0;

  if( osal_ints_are_locked())
    {
      return_val = osal_set_timer(&diag_rx_rsp_notify_timer, 1);
      ASSERT(OSAL_SUCCESS == return_val);
    }
    else
    {
  return_val = osal_set_sigs(&diag_tcb, DIAG_RX_RSP_SIG, &return_sigs);
  ASSERT(OSAL_SUCCESS == return_val);
}
}

/*===========================================================================

FUNCTION DIAG_RX_DELAY_RSP_NOTIFY

DESCRIPTION
  This routine sets the DIAG_RX_DELAY_RSP_SIG

===========================================================================*/
void diag_rx_delay_rsp_notify( void )
{
  osal_sigs_t return_sigs;
  int return_val = 0;

  if( osal_ints_are_locked())
  {
    return_val = osal_set_timer(&diag_rx_delay_rsp_notify_timer, 1);
    ASSERT(OSAL_SUCCESS == return_val);
  }
  else
  {
    return_val = osal_set_sigs(&diag_tcb, DIAG_RX_DELAY_RSP_SIG, &return_sigs);
    ASSERT(OSAL_SUCCESS == return_val);
  }
}

/*===========================================================================

FUNCTION DIAG_PROCESS_RX

DESCRIPTION
  This routine processes the RX queue as well as the cmd Q from the system
  monitor API, i.e. diag_cmd_req().

===========================================================================*/
static void
diag_process_rx (void)
{
  diag_req_cmd_type *cmd = NULL;

  static int diag_first_pkt = TRUE;
  diag_rx_pkt_type rx_pkt = {diag_req_inbound, sizeof(diag_req_inbound), 0, DIAGCOMM_PORT_1};

  /* We must be connected, so set the TX sig to start draining. */
  if (TRUE == diag_first_pkt)
  {
    diag_tx_notify();
    diag_first_pkt = FALSE;
  }

  /* Disable diag sleep so packet can be processed w/out interruption */
  diag_sleep_vote (DIAG_SLEEP_DIAG, FALSE);

  if (diag_get_rx_pkt_slave (&rx_pkt) == TRUE)
  {
    /* If we RX good packets, we must be connected. */
    DIAG_SET_MASK (diag_cx_state, DIAG_CX_COMM_S);

    diagpkt_process_request (&diag_req_inbound, rx_pkt.pkt_len, NULL, NULL,FALSE);

    #if defined (DIAG_FEATURE_EFS2)
      diag_fs_process_config (&diag_req_inbound, rx_pkt.pkt_len);
    #endif /* DIAG_FEATURE_EFS2 */
  }

  while ((cmd = q_get (&diag_req_cmd_q)) != NULL)
  {
    diagpkt_process_request (cmd->req, (uint16) cmd->length,
                             cmd->rsp_func, cmd->param,cmd->bFromDiagCmdQ);

    diag_free (cmd);
#ifdef FEATURE_DEBUG_DIAG_HEAP
    diag_process_rx_free_cnt++;
    MSG_3(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
                "FREE Q_GET DiagProcessRxFree = %d, heapUsed = %d, Req_q = %d",
     diag_process_rx_free_cnt, diag_heap_used_bytes(),q_cnt (&diag_req_cmd_q));
#endif /* FEATURE_DEBUG_DIAG_HEAP */
    cmd = NULL;
  }

  /* Go back to sleep after processing Rx packet */
  diag_sleep_vote(DIAG_SLEEP_DIAG, TRUE);

} /* diag_process_rx */



/****************************************************************************** 
 DIAG_SEND_BUF_CTRL_RETRY

DESCRIPTION
  Re-try the signals coressponding to set bits in msk variable

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

  Bit Mapping of bit mask to a control packet. Keeping it consistent among various codebases.
  Mapping with none means other code base has that position filled coressponding to particular 
  a control packet which is not present in this codebase:
   
  Bit  - Control Packet
  0    - DIAG_CTRL_MSG_FEATURE_BIT_POS	
  1    - DIAG_CTRL_MSG_NUM_PRESETS_BIT_POS
  2    - DIAG_CTRL_MSG_REG_BIT_POS
  3    - 
  4    - DIAG_CTRL_BT_EVENT_ID_BIT_POS 
  5    - DIAG_CTRL_BT_LOG_RANGE_BIT_POS 
  6    - DIAG_CTRL_BT_MSG_RANGE_BIT_POS
  7    - DIAG_CTRL_BT_MSG_MASK_BIT_POS
  8    - 
  9    -
  
********************************************************************************/

static void diag_send_buf_ctrl_retry(void) {	

	 if(diag_resend_ctrlpkt_mask != 0) 
	  {
		if(DIAG_IS_CTRLMASK_BIT_SET(DIAG_CTRL_MSG_FEATURE_BIT_POS)) {
				 DIAG_CLEAR_CTRLMASK_BIT(DIAG_CTRL_MSG_FEATURE_BIT_POS);		  
				 diag_ctrl_send_feature_mask();

		  } 			   
		if (DIAG_IS_CTRLMASK_BIT_SET(DIAG_CTRL_MSG_NUM_PRESETS_BIT_POS)) {
				 DIAG_CLEAR_CTRLMASK_BIT(DIAG_CTRL_MSG_NUM_PRESETS_BIT_POS);		  
#if defined(DIAG_MP) && !defined(DIAG_MP_MASTER)
				 diagcomm_ctrl_send_num_presets();


#endif  
		  } 
		if (DIAG_IS_CTRLMASK_BIT_SET(DIAG_CTRL_MSG_REG_BIT_POS)) {
				  DIAG_CLEAR_CTRLMASK_BIT(DIAG_CTRL_MSG_REG_BIT_POS);		  
#if defined(DIAG_MP) &&!defined(DIAG_MP_MASTER)
				  /* Signal to forward Diag registration msgs to Master via CTRL channel. */
				  diagpkt_fwd_cmd_reg();

#endif 				
		  } 

		if (DIAG_IS_CTRLMASK_BIT_SET(DIAG_CTRL_BT_EVENT_ID_BIT_POS)) {
				  DIAG_CLEAR_CTRLMASK_BIT(DIAG_CTRL_BT_EVENT_ID_BIT_POS);	
#if defined(DIAG_MP) && !defined(DIAG_MP_MASTER)				  
				  diag_ctrl_send_event_id_ctrl_pkt();
#endif

		  } 
		if (DIAG_IS_CTRLMASK_BIT_SET(DIAG_CTRL_BT_LOG_RANGE_BIT_POS)) {
				  DIAG_CLEAR_CTRLMASK_BIT(DIAG_CTRL_BT_LOG_RANGE_BIT_POS);			  
#if defined(DIAG_MP) && !defined(DIAG_MP_MASTER)				  
				  diag_ctrl_send_log_range_ctrl_pkt();
#endif


		  } 
		if (DIAG_IS_CTRLMASK_BIT_SET(DIAG_CTRL_BT_MSG_RANGE_BIT_POS)) {
				  DIAG_CLEAR_CTRLMASK_BIT(DIAG_CTRL_BT_MSG_RANGE_BIT_POS);			  
#if defined(DIAG_MP) && !defined(DIAG_MP_MASTER)				  
				  diag_ctrl_send_msg_ssid_range_ctrl_pkt();
#endif				  

		  } 
		if (DIAG_IS_CTRLMASK_BIT_SET(DIAG_CTRL_BT_MSG_MASK_BIT_POS)) {
				  DIAG_CLEAR_CTRLMASK_BIT(DIAG_CTRL_BT_MSG_MASK_BIT_POS);
#if defined(DIAG_MP) && !defined(DIAG_MP_MASTER)				  
				  diag_ctrl_send_msg_bt_mask_ctrl_pkt();
#endif

		  } 

	  }
	  else 
	  {
		 MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_handle_internal_sigs: No control packet to re-send");
	  } 			   
		 
  return;
}

/*===========================================================================

FUNCTION DIAG_HANDLE_INTERNAL_SIGS

DESCRIPTION
  Handle all of the internal Diag signals. This is an extension of
  diag_handle_sigs().

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

===========================================================================*/
void diag_handle_internal_sigs( void )
{
  int return_val = 0;
  osal_sigs_t return_sigs;
  osal_sigs_t sigs_internal;
  static osal_sigs_t resend_sig_mask=0;
  diag_tx_mode_type* tx_mode_ptr =NULL;
  diag_diagID_pname_type *diagid_ptr = NULL;
  uint8 id = DIAGID_UNKNOWN_ID;

  /*Get the signal mask of diag internal signals set */
  sigs_internal = diag_get_internal_sigs();

   /*feature mask should be the first thing to be sent*/
  if( sigs_internal & DIAG_INT_FEAT_UPDATE_SIG )
  {
    diag_clear_internal_sigs( DIAG_INT_FEAT_UPDATE_SIG );
    if(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].allow_flow == TRUE)
    {
        diag_ctrl_send_feature_mask();
        diagcomm_ctrl_send_num_presets();

    }
    else
    {
        resend_sig_mask |=  DIAG_INT_FEAT_UPDATE_SIG;
    }
  }
  else if (sigs_internal &   DIAG_INT_RESEND_CTRL_SIG)
  {
    diag_clear_internal_sigs(DIAG_INT_RESEND_CTRL_SIG);
    diagcomm_io_ctrl_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].allow_flow = TRUE;


    osal_lock_mutex(&diag_flow_control_mutex);
    /*Resend the pending data*/
    if((diagcomm_io_ctrl_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].diag_prev_tx_pending) &&
        (diagcomm_io_ctrl_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].diag_resend_data == TRUE))
    {
        diag_send_buf_ctrl_retry();

        if(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].allow_flow == TRUE)
        {
            diagcomm_io_ctrl_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].diag_resend_data = FALSE;
        }
        else
        {
            diagcomm_io_ctrl_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].diag_resend_data = TRUE;
        }
    }

    if(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].allow_flow == TRUE)
    {
        (void) diag_set_internal_sigs(resend_sig_mask);

        /*Clear the resend signal mask */
        resend_sig_mask=0;
    }

    osal_unlock_mutex(&diag_flow_control_mutex);

  }
  else if( sigs_internal & DIAG_INT_CTRL_REG_SIG )
  {
    diag_clear_internal_sigs( DIAG_INT_CTRL_REG_SIG );
    if(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].allow_flow == TRUE)
    {
        /* Signal to forward Diag registration msgs to Master via CTRL channel. */
        diagpkt_fwd_cmd_reg ();
    }
    else
    {
        resend_sig_mask |=  DIAG_INT_CTRL_REG_SIG;
    }
  }
  else if( sigs_internal & DIAG_INT_CMD_DEREG_SIG )
  {
    diag_clear_internal_sigs( DIAG_INT_CMD_DEREG_SIG );
    /* Signal to forward Diag deregistration msgs to Master via CTRL channel. */
    diagpkt_fwd_cmd_dereg ();
  }
  else if (sigs_internal & DIAG_INT_SEND_BT_MASK_SIG)
  {
      diag_clear_internal_sigs( DIAG_INT_SEND_BT_MASK_SIG );
      /* Order of ctrl packets for this feature: (CTRL PKT 22,CTRL PKT 23,CTRL PKT 24,CTRL PKT 25)
      If nothing else, MSG SSID Range reporting (24) needs to be sent before MSG Build-mask reporting(25)
      Build/Range related control packets should be sent after feature-mask and any existing command-registrations are sent over */

      if(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].allow_flow == TRUE)
      {
      diag_ctrl_send_event_id_ctrl_pkt();
      diag_ctrl_send_log_range_ctrl_pkt();
      diag_ctrl_send_msg_ssid_range_ctrl_pkt();
      diag_ctrl_send_msg_bt_mask_ctrl_pkt();
      }
      else
      {
        resend_sig_mask |=  DIAG_INT_SEND_BT_MASK_SIG;
      }
  }
  else if (sigs_internal & DIAG_INT_FLUSH_BUF_SIG)
  {
    diag_clear_internal_sigs( DIAG_INT_FLUSH_BUF_SIG );
    diagbuf_flush_timeout(0);
  }
  else if (sigs_internal & DIAG_INT_RESEND_DATA_SIG)
  {
    diag_clear_internal_sigs( DIAG_INT_RESEND_DATA_SIG );

    osal_lock_mutex(&diag_flow_control_mutex);

    /*Resend the pending data*/
    if((diagcomm_io_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].diag_prev_tx_pending) &&
      (diagcomm_io_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].diag_resend_data == TRUE))
    {
      diagcomm_io_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].allow_flow = TRUE;

      diagid_ptr = diag_diagID_pname_list;
      /* Attempt to retry sending the pending data of all diagIDs */
      while(diagid_ptr !=NULL)
      {
        id = diagid_ptr->diagID;
        tx_mode_ptr = diag_get_tx_mode_ptr(DIAG_STREAM_1, id);
        /* Skip resending data if tx_mode_ptr is not yet initialized */
        if(tx_mode_ptr)
	{
        diag_send_buf_retry(DIAGCOMM_IO_DATA, DIAGCOMM_PORT_1, tx_mode_ptr);
	}
        diagid_ptr=diagid_ptr->next;
      }   

      if(diagcomm_io_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].diag_prev_tx_pending == TRUE)
      {
        diagcomm_io_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].diag_resend_data = TRUE;
      }
      else
      {
        diagcomm_io_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].diag_resend_data = FALSE;
      }
    }

      osal_unlock_mutex(&diag_flow_control_mutex);

      diag_tx_notify();

      return_val = osal_set_sigs(&diag_tcb, DIAG_EVENT_DRAIN_SIG, &return_sigs);
      ASSERT(OSAL_SUCCESS == return_val);

      MSG_1(MSG_SSID_DIAG,MSG_LEGACY_HIGH,"allow_flow status is %d", diagcomm_io_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].allow_flow);
  }
  else if (sigs_internal & DIAG_INT_RESEND_CMD_SIG)
  {
    diag_clear_internal_sigs( DIAG_INT_RESEND_CMD_SIG );
    /*Resend the pending data*/
    if((diagcomm_io_cmd_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].diag_prev_tx_pending) &&
    (diagcomm_io_cmd_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].diag_resend_data == TRUE))
    {
      diagcomm_io_cmd_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].allow_flow = TRUE;
      diag_send_buf_retry(DIAGCOMM_IO_CMD,DIAGCOMM_PORT_1, NULL);
      diagcomm_io_cmd_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].diag_resend_data = FALSE;

    }
  }
   else if (sigs_internal & DIAG_MASK_CHANGE_NOTIFY_SIG)
  {
    diag_clear_internal_sigs( DIAG_MASK_CHANGE_NOTIFY_SIG );
    /*Notify the registered users about diag mask change*/
    diag_mask_change_notify();
  }
  else if( sigs_internal & DIAG_INT_MASK_UPDATE_SIG )
  {
    diag_clear_internal_sigs( DIAG_INT_MASK_UPDATE_SIG );
  }
  /* DIAG_INT_REG_TBL_RSP_SIG is set when a Registration Table Retrieval Command is sent
     ( in diagdiag_get_reg_table() ) */
  else if ( sigs_internal & DIAG_INT_REG_TBL_RSP_SIG )
  {
      diag_clear_internal_sigs( DIAG_INT_REG_TBL_RSP_SIG );
      diagpkt_get_registration_table();
  }
  else if( sigs_internal & DIAG_INT_IDLE_PROCESSING_SIG )
  {
      diag_clear_internal_sigs( DIAG_INT_IDLE_PROCESSING_SIG );
    /* Dont do anything. This is just to wake up diag to do idle processing */
  }

  else if( sigs_internal & DIAG_INT_SET_DRAIN_TIMER_SIG )
  {
    int return_val = 0;

    diag_clear_internal_sigs( DIAG_INT_SET_DRAIN_TIMER_SIG );
    if ( !diag_drain_timer_pending && diag_drain_timer_enabled() )
    {
      diag_drain_timer_pending = TRUE;
      return_val = osal_set_timer(&diag_drain_timer, diag_drain_timer_len);
      ASSERT(OSAL_SUCCESS == return_val);
    }
  }
  else if( sigs_internal & DIAG_INT_DRAIN_BUFFERING_BUFFER_SIG )
  {
    diag_clear_internal_sigs( DIAG_INT_DRAIN_BUFFERING_BUFFER_SIG );
    
    diagid_ptr = diag_diagID_pname_list;

    /* Attempt to drain the pending data of all diagIDs */
    while(diagid_ptr !=NULL)
    {
      id = diagid_ptr->diagID;
      tx_mode_ptr = diag_get_tx_mode_ptr(DIAG_STREAM_1, id);
      /* Skip resending data if tx_mode_ptr is not yet initialized*/
      if(tx_mode_ptr)
      {  
        diag_drain_send_buffer( DIAG_STREAM_1, tx_mode_ptr );
      }
      diagid_ptr=diagid_ptr->next;
    }
  }

  if( diag_get_internal_sigs() == 0 )
  {
    /*since all internal signals are handled, clear the main internal signal
     in function protected by mutex */
    (void)diag_clear_internal_sigs(0);
  }
  else
  {
    /*More signals to process, So set the main internal signal in function
    protected by mutex */
    (void)diag_set_internal_sigs(0);
  }

} /* diag_handle_internal_sigs */

#ifdef FEATURE_DIAG_STRESS_PROFILE
uint64 diag_sigs_pcycle_total=0, diag_sigs_pcycle_iterations=0;
#endif

/*===========================================================================

FUNCTION DIAG_HANDLE_SIGS

DESCRIPTION
  Handle all of the signals that interest diag.  Watch-dog is kicked as
  needed.

RETURN VALUE
  With *sigs, signals are returned.

===========================================================================*/
static osal_sigs_t
diag_handle_sigs (osal_sigs_t mask)
{
  int return_val = 0;
  boolean found = FALSE;
#ifdef FEATURE_DIAG_STRESS_PROFILE
  uint64 pcycle_start, pcycle_end;
#endif
  osal_sigs_t return_sigs;
  osal_sigs_t sigs = 0;    /* Signals currently set for this task. */
  osal_sigs_t busy_mask = mask |
      DIAG_TX_SIG | DIAG_CHANGE_CONN_SIG | DIAG_DRAIN_TIMER_SIG |DIAG_RX_RSP_SIG |DIAG_EVENT_DRAIN_SIG |
      DIAG_EVENT_TIMER_SIG |DIAG_EVENT_TIMER_SEC_SIG | DIAG_EVENT_DRAIN_SEC_SIG | DIAG_TASK_STOP_SIG | DIAG_TASK_OFFLINE_SIG| DIAG_RX_DELAY_RSP_SIG ;
  osal_sigs_t composite_mask = busy_mask | DIAG_RX_SIG |
#ifdef FEATURE_DIAG_NV
    DIAG_REFRESH_CACHE_SIG    |
#endif
    DIAG_COMM_OPEN_SIG          |
#if defined DIAG_MP
    DIAG_COMM_OPEN_SMD_SIG      |
#endif


    DIAG_CTRL_MSG_SIG         |
    DIAG_INTERNAL_SIG         |
    DIAG_COMM_SIO_TX_FLUSH_SIG      |
    DIAG_RPT_TIMER_SIG        |
    DIAG_COMM_CLOSE_SIG 
#ifdef DIAG_UIMAGE_MODE
     |
    DIAG_MICRO_DIAGBUFFER_UIMAGE_SIG
#endif
    ; /* Semi colon to terminate the statement */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  while (!found)
  {
    sigs = osal_thread_wait (&diag_tcb, composite_mask);

    /* Pet the dog before doing any work. */
    diag_kick_watchdog ();
#ifdef FEATURE_DIAG_STRESS_PROFILE
    pcycle_start = qurt_get_core_pcycles();
#endif

    if (sigs & mask)
    {
      found = TRUE;
    }
    else if (sigs & DIAG_TASK_STOP_SIG)
    {
      return_val = osal_reset_sigs(&diag_tcb, DIAG_TASK_STOP_SIG, &return_sigs);
      ASSERT(OSAL_SUCCESS == return_val);
      diag_shutdown ();
    }
      else if (sigs & DIAG_TASK_OFFLINE_SIG)
      {
        return_val = osal_reset_sigs(&diag_tcb, DIAG_TASK_OFFLINE_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);
        diag_offline ();
      }
      else if (sigs & DIAG_COMM_CLOSE_SIG)
      {
        return_val = osal_reset_sigs(&diag_tcb, DIAG_COMM_CLOSE_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);

        diagcomm_ctrl_close (DIAGCOMM_PORT_REMOTE_PROC, DIAGCOMM_PORT_1);
        diagcomm_cmd_close(DIAGCOMM_PORT_REMOTE_PROC, DIAGCOMM_PORT_1);
        diagcomm_close();
      }
      else if (sigs & DIAG_COMM_OPEN_SIG)
      {
        return_val = osal_reset_sigs(&diag_tcb, DIAG_COMM_OPEN_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);

        if(TRUE == diagcomm_io_socket_check_support(&diagcomm_io_ctrl_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1]))
        {
           diagcomm_io_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].io_type = DIAGCOMM_IO_TYPE_IPCR_UDP;
           diagcomm_io_ctrl_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].io_type = DIAGCOMM_IO_TYPE_IPCR_UDP;
           diagcomm_io_cmd_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].io_type = DIAGCOMM_IO_TYPE_IPCR_UDP;

           diagcomm_ctrl_open (DIAGCOMM_PORT_REMOTE_PROC, DIAGCOMM_PORT_1);
           diagcomm_cmd_open (DIAGCOMM_PORT_REMOTE_PROC, DIAGCOMM_PORT_1);
           diagcomm_sio_open ();
        }
      }
      else if ((sigs & DIAG_COMM_SIO_TX_FLUSH_SIG) != 0)
      {
        return_val = osal_reset_sigs(&diag_tcb, DIAG_COMM_SIO_TX_FLUSH_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);
        diagcomm_sio_tx_flush();
      }
      /* If the signal to send diagID is set by LSM, check all elements in diag_send_diagID_q */
      else if (sigs & DIAG_REQUEST_DIAGID_SIG)
      {
        return_val = osal_reset_sigs(&diag_tcb, DIAG_REQUEST_DIAGID_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);

#ifdef DIAG_FORCE_DIAGID
      /* Execute only if feature mask is known */
        if (diag_diagID_use_local != DIAGID_LOCAL_ID_UNINIT)
#else
        if ((DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].feature_mask,F_DIAG_DIAGID_SUPPORT)))
#endif
        {
          diag_diagID_pname_type* diagID_node = diag_diagID_pname_list;

          while(diagID_node != NULL)
          { 
            /* Only send the request if is_ctrl_sent is FALSE */
            if(diagID_node->is_ctrl_sent == FALSE)
            {
              diag_ctrl_request_diagID(diagID_node->process_name);
              diagID_node->is_ctrl_sent=TRUE;
            }
            diagID_node = diagID_node->next;
          }
        }

      }
      else if (sigs & DIAG_CTRL_MSG_SIG)
      {
        /*
         DIAG_CTRL_MSG_SIG must be handled before DIAG_SMDL_CMD_READ_SIG
         because there is only a single read buffer.  If the data read was
         a control packet, we must handle the control packet first before
         we can read any additional data from SMDL
        */
        return_val = osal_reset_sigs(&diag_tcb, DIAG_CTRL_MSG_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);
        diagcomm_process_ctrl();
      }

      else if((sigs & DIAG_RX_RSP_SIG)!= 0)
      {
        return_val = osal_reset_sigs(&diag_tcb, DIAG_RX_RSP_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);

       if (diagcomm_cmd_status(DIAGCOMM_PORT_1))
        {
           diagpkt_rsp_send();
        }
        diag_sleep_vote (DIAG_SLEEP_DIAG, TRUE);
      }

      else if ((sigs & DIAG_RX_SIG) != 0)
      {
        return_val = osal_reset_sigs(&diag_tcb, DIAG_RX_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);

        diag_process_rx ();
      }
     else if (sigs & DIAG_EVENT_TIMER_SIG)
      {
        return_val = osal_reset_sigs(&diag_tcb, DIAG_EVENT_TIMER_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);
        diag_event_stale_timer_pending = FALSE;
        //Passing NULL since the timer expired and we will set all the pds event reports last ptr.
        event_stale (NULL);
      }
     else if(sigs & DIAG_RX_DELAY_RSP_SIG)
      {
        return_val = osal_reset_sigs(&diag_tcb, DIAG_RX_DELAY_RSP_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);

        diag_sleep_vote (DIAG_SLEEP_DIAG, FALSE);
        if (diagcomm_cmd_status(DIAGCOMM_PORT_1))
            diagpkt_delay_rsp_send();
        diag_sleep_vote(DIAG_SLEEP_DIAG, TRUE);
      }
     else if (sigs & DIAG_INTERNAL_SIG)
      {
        diag_handle_internal_sigs();
      }
     else if (sigs & DIAG_EVENT_DRAIN_SIG)
      {
        return_val = osal_reset_sigs(&diag_tcb, DIAG_EVENT_DRAIN_SIG, &return_sigs);
            ASSERT(OSAL_SUCCESS == return_val);
        event_drain ( mask | DIAG_COMM_SIO_TX_FLUSH_SIG);
      }
     else if (sigs & DIAG_EVENT_TIMER_SEC_SIG)
      {
        return_val = osal_reset_sigs(&diag_tcb, DIAG_EVENT_TIMER_SEC_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);
        diag_event_stale_timer_pending_sec = FALSE;
        event_stale_sec ();
      }
     else if (sigs & DIAG_EVENT_DRAIN_SEC_SIG)
      {
        return_val = osal_reset_sigs(&diag_tcb, DIAG_EVENT_DRAIN_SEC_SIG, &return_sigs);
            ASSERT(OSAL_SUCCESS == return_val);
        event_drain_sec ( mask | DIAG_COMM_SIO_TX_FLUSH_SIG);
      }
     else if (sigs & DIAG_DRAIN_TIMER_SIG)
      {
        diagbuf_status_enum_type status;

        return_val = osal_reset_sigs(&diag_tcb, DIAG_DRAIN_TIMER_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);

        diag_drain_timer_pending = FALSE;

        if ( (TRUE == diagcomm_io_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].allow_flow) )
        {

        diagbuf_send_pkt (NULL,DIAG_STREAM_1 | DIAG_STREAM_2);

        diag_sleep_vote (DIAG_SLEEP_DIAG, FALSE);

           status = diagbuf_drain(DIAG_TASK_STOP_SIG | mask | DIAG_RX_SIG | DIAG_RX_RSP_SIG | DIAG_EVENT_DRAIN_SIG | DIAG_EVENT_DRAIN_SEC_SIG | DIAG_COMM_SIO_TX_FLUSH_SIG);

          /* This function will kick the watchdog for us. */
          if (status == DIAGBUF_EMPTY_S)
          {
            diag_sleep_vote(DIAG_SLEEP_DIAG, TRUE);
          }
        }
      }
     else if (sigs & DIAG_TX_SIG)
      {
        diagbuf_status_enum_type status;
        return_val = osal_reset_sigs(&diag_tcb, DIAG_TX_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);

        /* Clear the TX notification semaphore. */
        diag_tx_pending = FALSE;

        if((TRUE == diagcomm_io_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].allow_flow))
        {
          /* Turn off sleep while buffering. */
          diag_sleep_vote (DIAG_SLEEP_DIAG, FALSE);

           status = diagbuf_drain (DIAG_TASK_STOP_SIG | mask | DIAG_RX_SIG | DIAG_RX_RSP_SIG | DIAG_EVENT_DRAIN_SIG | DIAG_COMM_SIO_TX_FLUSH_SIG);

          /* This function will kick the watchdog for us. */
          if (status == DIAGBUF_EMPTY_S)
          {
            diag_sleep_vote (DIAG_SLEEP_DIAG, TRUE);
          }
        }
      }

#ifdef DIAG_UIMAGE_MODE
      else if (sigs & DIAG_MICRO_DIAGBUFFER_UIMAGE_SIG)
      {
        osal_sigs_t return_sigs;
        int return_val = 0;

        return_val = osal_reset_sigs(&diag_tcb, DIAG_MICRO_DIAGBUFFER_UIMAGE_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);

        return_val = osal_set_sigs(&diag_tcb, DIAG_TX_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val); 
      }
#endif

    #ifdef FEATURE_DIAG_NV
      else if (sigs & DIAG_REFRESH_CACHE_SIG)
      {
        /* The packet processing cache may have been changed. */
        return_val = osal_reset_sigs(&diag_tcb, DIAG_REFRESH_CACHE_SIG, &return_sigs);
        ASSERT(OSAL_SUCCESS == return_val);

        diagnv_refresh_cache ();
      }
    #endif

      /* See if there is anything to do.  If empty, this is a quick operation. */
      diag_idle_processing ();

      #ifdef FEATURE_DIAG_STRESS_PROFILE
      pcycle_end = qurt_get_core_pcycles();
      diag_sigs_pcycle_iterations++;
      // should really have protection around this if we want to keep it
      diag_sigs_pcycle_total += pcycle_end - pcycle_start;
      #endif

  }

  return sigs;
}                /* diag_handle_sigs */

#if ((!defined(FEATURE_DIAG_SWITCH) && !defined(DIAG_FRAMING)) || (defined(FEATURE_DIAG_SWITCH)))
/*===========================================================================

FUNCTION DIAG_GET_RX_PKT_SLAVE

DESCRIPTION
  Standalone configuration:
    The incoming packets can be a chained. This function calls dsm_pullup_tail
    and extracts the whole packet.
  
  Slave configuration:
    This function will retrieve the next DM packet sent by app to modem.
    This function extracts the whole packet.

DEPENDENCIES
  Standalone configuration:
    This procedure can only be called when a full DM packet is available on
    queue from SIO.  Further, if multiple packets are enqueued, then this will
    only process the first packet.  The remaining packets are left on the queue.
  
  Slave configuration:
    If multiple packets are enqueued, then this will only process the first packet.  
    The remaining packets are left on the queue.

RETURN VALUE
  Returns TRUE if full packet recieved, else returns FALSE.

===========================================================================*/
static boolean
diag_get_rx_pkt_slave (diag_rx_pkt_type* rx_pkt)
{
  uint32 used;
  uint8 *data_ptr;

  if(DIAG_INT_FEATURE_MASK_CHECK(F_DIAG_SOCKET_SUPPORT))
  {
    diagcomm_io_conn_type *conn = &diagcomm_io_cmd_conn[DIAGCOMM_PORT_REMOTE_PROC][rx_pkt->port_num];
    uint32 used_bytes;
    osal_lock_mutex(&conn->buf_mutex);
    used_bytes = conn->used;
    osal_unlock_mutex(&conn->buf_mutex);

    if( used_bytes == 0 )
    {
       return FALSE;
    }
    else
    {
        if (rx_pkt->dest_ptr == NULL)
        {
          return FALSE;
        }
        else
        {
           memscpy(rx_pkt->dest_ptr, rx_pkt->max_size, conn->buf, used_bytes);
           rx_pkt->pkt_len = used_bytes;
           osal_lock_mutex(&conn->buf_mutex);
           conn->used =0;
           osal_unlock_mutex(&conn->buf_mutex);
           return TRUE;
        }
    }
  }

  used = diagcomm_io_cmd_conn[DIAGCOMM_PORT_REMOTE_PROC][rx_pkt->port_num].used;
  data_ptr = (uint8 *)&(diagcomm_io_cmd_conn[DIAGCOMM_PORT_REMOTE_PROC][rx_pkt->port_num].buf);

  if( used == 0 )
  {
      return FALSE;
  }
  /*If data received is greater than max size then return immediately*/
  else if (used > rx_pkt->max_size)
  {
      MSG_2 (MSG_SSID_DIAG, MSG_LEGACY_HIGH,"Received Diag Req Pkt Size is %d Max Size For Req Pkt is %d \n", used, rx_pkt->max_size);
      /*Clear the read buffer*/
      diagcomm_io_cmd_conn[DIAGCOMM_PORT_REMOTE_PROC][rx_pkt->port_num].used= 0;
      rx_pkt->dest_ptr = NULL;
  }

  if (rx_pkt->dest_ptr == NULL)
  {
      return FALSE;
  }
  else
  {
      memscpy(rx_pkt->dest_ptr, rx_pkt->max_size, data_ptr, used);
      rx_pkt->pkt_len = used;
      diagcomm_io_cmd_conn[DIAGCOMM_PORT_REMOTE_PROC][rx_pkt->port_num].used = 0;
      return TRUE;
  }

    return FALSE;
}       /* diag_get_rx_pkt_slave */
       /* diag_get_rx_pkt_slave */
#endif

#if ((!defined(FEATURE_DIAG_SWITCH) && defined(DIAG_FRAMING)) || (defined(FEATURE_DIAG_SWITCH))|| defined(DIAG_FEATURE_EFS2))
/*===========================================================================

FUNCTION DIAG_EXTRACT_PKT

DESCRIPTION
  This function extracts a packet from a HDLC encoded stream, and verifies
  CRC is correct.

DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if a valid packet was found.

===========================================================================*/
static boolean
diag_extract_pkt (diag_hdlc_decode_type * hdlc)
{
  boolean pkt_bnd = FALSE;
  /* Cyclic Redundancy Check of incoming packet. */
  word crc;

  if (hdlc && hdlc->src_ptr && hdlc->dest_ptr)
  {

    pkt_bnd = diag_hdlc_decode (hdlc);

    if (pkt_bnd == TRUE)
    {
      /* At a packet boundary, check the CRC, reset dst_idx for the next
       * packet, and return the packet (if valid). */
      crc = crc_16_l_calc (hdlc->dest_ptr, (word) ((hdlc->dest_idx - 1) * 8));

      if (crc == CRC_16_L_OK)
      {
    hdlc->dest_idx -= DIAG_ASYNC_TAIL_SIZE;
      }
      else
      {
        MSG (MSG_SSID_DIAG, MSG_LEGACY_LOW,"Bad RX CRC");
        pkt_bnd = FALSE;
        hdlc->dest_idx = 0; /* Start new packet */
      }
    }
    else if (hdlc->dest_idx >= hdlc->dest_size)
    {
      /* The inbound packet is too large.  Therefore, we cannot recieve it.
       * By setting the dest_idx field to 0, the rest of the packet will be read
       * in, and the CRC will fail. */
      hdlc->dest_idx = 0;
    }
  }

  return pkt_bnd;
}
#endif
/*=========================================================================*/
void
diag_timeout (void)
{
  #if defined (DIAG_QDSP6_APPS_PROC)
  diag_suspend (DIAG_TOUT_TIME);
  #else

  /* First, flush the TX buffer chain. */
  diagbuf_flush_timeout(DIAG_SIO_TIMEOUT_TIMER_LEN);

  #if defined(FEATURE_REX_OPAQUE_TCB_APIS)
     /* The passed in value is ignored */
     osal_autodog_enable(0);
  #else
     osal_autodog_enable(osal_thread_self()->dog_report_val);
  #endif

  diag_suspend (DIAG_TOUT_TIME);
  osal_autodog_disable();


  diagcomm_flush_rx (NULL);
  #endif

}               /* diag_timeout */

/*=========================================================================*/
void
diag_suspend (unsigned int duration)
{
 osal_sigs_t return_sigs;
  int return_val = 0;
  if (duration > 0)
  {
    #if !defined (DIAG_QDSP6_APPS_PROC)
    #if defined(FEATURE_REX_OPAQUE_TCB_APIS)
     osal_autodog_enable(0);
    #else
     osal_autodog_enable(osal_thread_self()->dog_report_val);
    #endif
    #endif

    return_sigs = osal_timed_wait (&diag_tcb, DIAG_TOUT_TIMER_SIG , &diag_tout_timer, duration);
    return_val = osal_reset_sigs(&diag_tcb, DIAG_TOUT_TIMER_SIG, &return_sigs);
    ASSERT(OSAL_SUCCESS == return_val);

    #if !defined (DIAG_QDSP6_APPS_PROC)
    osal_autodog_disable();
    #endif
  }
}

/*=========================================================================*/
void
diag_pause (unsigned int duration, boolean enable_rx_processing)
{

  if (duration > 0)
  {
    int return_val = 0;
    return_val = osal_set_timer(&diag_tout_timer, duration);
    ASSERT(OSAL_SUCCESS == return_val);

    if (enable_rx_processing)
    {
      /* Process RX packets as well. */
      (void) diag_handle_sigs (DIAG_TOUT_TIMER_SIG);
    }
    else
    {
      /* Just process TX */
      (void) diag_wait (DIAG_TOUT_TIMER_SIG);
    }
  }
}


/*===========================================================================

FUNCTION DIAG_SET_DRAIN_TIMER

DESCRIPTION
  This function sets a Diag internal signal which will in turn set the
  diag drain timer from Diag's context.

===========================================================================*/
void
diag_set_drain_timer()
{
  if ( !diag_drain_timer_pending && diag_drain_timer_enabled() )
  {
    diag_set_internal_sigs( DIAG_INT_SET_DRAIN_TIMER_SIG );
  }

} /* diag_set_drain_timer */

/*===========================================================================

FUNCTION DIAG_DRAIN_TIMER_ENABLED

DESCRIPTION
  This function determines whether the drain timer is enabled.

===========================================================================*/
boolean diag_drain_timer_enabled(void)
{
  return diag_drain_timer_len > 0;
}

/*===========================================================================

FUNCTION DIAG_CLR_DRAIN_TIMER

DESCRIPTION
  This function clears the drain timer.

===========================================================================*/
void
diag_clr_drain_timer()
{
  int return_val = 0;

  if( diag_drain_timer_pending == FALSE )
    return;

  diag_drain_timer_pending = FALSE;
  return_val = osal_reset_timer(&diag_drain_timer);
  ASSERT(OSAL_SUCCESS == return_val);

} /* diag_clr_drain_timer */


/*===========================================================================

FUNCTION DIAG_TASK

DESCRIPTION
  This function contains the diagnostic task.  It processes diag commands
  for each request packet received from the external device (tool).

  This procedure does not return.

===========================================================================*/
void
diag_task (uint32 params)
{
#if defined(FEATURE_DIAG_NV)
  nv_err_fatal_options_type option;
  nv_stat_enum_type nvstatus = NV_FAIL_S;
#endif

#if defined (FEATURE_DIAG_SWITCH) && defined (FEATURE_DIAG_NV)
  nv_stat_enum_type ftm_nvstatus = NV_FAIL_S;
  uint8 diagloc_proc_decider = 0;
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_DIAG_SWITCH
 diag_on_single_proc = DIAG_FTM_SWITCH_VAL;
#endif

  /*This sets diag_task_begin to TRUE and for subsequent calls we return */
  if(!osal_atomic_compare_and_set((osal_atomic_word_t*)&diag_task_begin, FALSE, TRUE))
    return;

  (void) params;


  /* Initialize task.
   */
  diag_init ();

  /* Notify that we are ready to start.  When this function returns, we
     are started. */
  diag_ready_start_task ();

  /* The comm layer needs to be initialized before anything else.  This is
   * needed because diagbuf_init() sets callbacks that are initialized in
   * diagcomm_init(). */

#ifdef FEATURE_DIAG_SWITCH
  diag_on_single_proc = 0;
#endif

#if defined (FEATURE_DIAG_NV) && defined (FEATURE_DIAG_SWITCH)
  ftm_nvstatus = diag_nv_read (NV_DIAG_FTM_MODE_SWITCH_I, &diagloc_proc_decider);
  if(ftm_nvstatus != NV_FAIL_S)
    diag_on_single_proc = diagloc_proc_decider;
 #endif

#ifdef FEATURE_DIAG_SWITCH
  if(diag_on_single_proc != DIAG_FTM_SWITCH_VAL)
#endif

  diagcomm_ctrl_init(DIAGCOMM_PORT_REMOTE_PROC, DIAGCOMM_PORT_1);
  diagcomm_cmd_init(DIAGCOMM_PORT_REMOTE_PROC, DIAGCOMM_PORT_1);
  diagcomm_sio_init(DIAGCOMM_PORT_1);

  /* Register a callback to notify us when inbound data is available. */
  diagcomm_register_inbound (diag_rx_notify);

  /* Forid sleep during init. */
  diag_sleep_vote (DIAG_SLEEP_DIAG, FALSE);

  /* Read the packet processing cache.*/
#if defined(FEATURE_DIAG_NV)
  diagnv_refresh_cache ();
#endif

#if defined(DIAG_FEATURE_EFS2)
  /* Process filestream mode, if applicable */
/*lint -save -e{545} Suspicious use of & */
  while (diag_fs_inbound_mode (&diag_req_inbound,
                   sizeof (diag_req_inbound)) == TRUE);
/*lint -restore */

  /* The powerup event cannot be sent until the input file has processed.
     This is the only way the event service would be enabled. */
#endif /* DIAG_FEATURE_EFS2 */

  /* Send powerup event */
  event_report (EVENT_POWERUP);

#if defined(FEATURE_DIAG_NV)
  /* Initialize the err_auto_action option */
  nvstatus = diag_nv_read (NV_ERR_FATAL_OPTIONS_I, (void *) &option);

  if (nvstatus == NV_DONE_S)
  {
    err_set_auto_action((err_action_type)option.option);
  }
  else
  {
    /* Nothing to do here.  Default behavior defined in err_init(). */
  }
#endif

  /* Vote OK to sleep for the diagnostic task. */
  diag_sleep_vote (DIAG_SLEEP_DIAG, TRUE);

    diagpkt_init_manual_dispatch_table ();

  diagbuf_tx_sleep_parameters_set();



  /* Clear bootup event mask */
  event_set_all_rt_masks(0, 0);

 /*----------------------------------------------------------------------------
                           Begin task operations
----------------------------------------------------------------------------*/
  /* Get task signals */
  (void) diag_handle_sigs (0);

}               /* diag_task */

#if ((!defined(FEATURE_DIAG_SWITCH) && defined(DIAG_FRAMING)) || (defined(FEATURE_DIAG_SWITCH))|| defined(DIAG_FEATURE_EFS2))
/*===========================================================================

FUNCTION DIAG_HDLC_DECODE

DESCRIPTION
  This routine provides XOR unescaping functionality for the use of callers.
  This routine is the inverse of do_escaping.

  When DIAG_ASYNC_ESC_CHAR is found, it is discarded and the next character
  is unescaped as follows <src_byte ^ DIAG_ASYNC_ESC_MASK>.  This character is
  stored in dest_ptr.

  When an unescaped DIAG_ASYNC_CONTROL_CHAR is found, this routine will exit
  immediately and indicate that an unescaped control character was found, in
  the parameter found_control_char.

  src_ptr and dest_ptr can be the same or overlapping.

DEPENDENCIES
  When an escape character straddles two packets, escaping will indicate this
  fact.  The caller needs to pass this parameter (unscathed) to
  do_unescaping in order for un-escaping to be done correctly.

RETURN VALUE
  A boolean indicating that the control character was found.

===========================================================================*/
static boolean
diag_hdlc_decode (diag_hdlc_decode_type * hdlc)
{
  byte *src_ptr = NULL, *dest_ptr = NULL;
  unsigned int src_length = 0, dest_length = 0;

  unsigned int converted_length = 0;
  unsigned int i;
  byte src_byte;
  boolean pkt_bnd = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hdlc && hdlc->src_ptr && hdlc->dest_ptr &&
      (hdlc->src_size - hdlc->src_idx > 0) &&
      (hdlc->dest_size - hdlc->dest_idx > 0))
  {

    /* Copy paramters to local variables for optimized memory access */
    src_ptr = hdlc->src_ptr;
    src_ptr = &src_ptr[hdlc->src_idx];
    src_length = hdlc->src_size - hdlc->src_idx;

    dest_ptr = hdlc->dest_ptr;
    dest_ptr = &dest_ptr[hdlc->dest_idx];
    dest_length = hdlc->dest_size - hdlc->dest_idx;

    for (i = 0; i < src_length; i++)
    {
      src_byte = src_ptr[i];

      if (hdlc->escaping)
      {
        dest_ptr[converted_length++] = src_byte ^ DIAG_ASYNC_ESC_MASK;
        hdlc->escaping = FALSE;

      }
      else if (src_byte == DIAG_ASYNC_ESC_CHAR)
      {
        /* If DIAG_ASYNC_ESC_CHAR is found at the end of src_ptr,
         ** there is nothing to un-escape.  So, no choice
         ** but to die
         */
        if (i == (src_length - 1))
        {
          hdlc->escaping = TRUE;
          i++;
          break;
        }
        else
        {
          dest_ptr[converted_length++] = src_ptr[++i] ^ DIAG_ASYNC_ESC_MASK;
        }
      }
      else if (src_byte == DIAG_ASYNC_CONTROL_CHAR)
      {
        /* Found control character.
         */
        dest_ptr[converted_length++] = src_byte;
        pkt_bnd = TRUE;
        i++;
        break;
      }
      else
      {
        dest_ptr[converted_length++] = src_byte;
      }

      if (converted_length >= dest_length)
      {
        /* If more data cannot be stored into dest_ptr,
         ** exit loop
         */
        i++;
        break;
      }
    }               /* for i=0; i<src_length; i++ */

    hdlc->src_idx += i;
    hdlc->dest_idx += converted_length;
  }

  return pkt_bnd;
}               /* diag_hdlc_decode */
#endif

/*=========================================================================*/
boolean
diag_idle_processing_register (diag_idle_processing_ftype fp, void *param)
{
  int i;
  boolean found = FALSE;
  boolean status = FALSE;   /* Default to unsuccessful */

  if (fp)
  {
    /* if not yet found in the table, look for a NULL entry. */
    for (i = 0; !found && i < DIAG_IDLE_PROC_CNT; i++)
    {
      osal_lock_mutex(&diag_idle_process_mutex);

      /*Check for existing entry and break if found */
      if (diag_idle_proc_tbl[i].fp == fp &&
      diag_idle_proc_tbl[i].param == param)
      {
        status = TRUE;
        found = TRUE;
        osal_unlock_mutex(&diag_idle_process_mutex);
        break;
      }

      if (diag_idle_proc_tbl[i].fp == NULL)
      {
    /* Found an empty table entry. */
    found = TRUE;
    status = TRUE;

    diag_idle_proc_tbl[i].fp = fp;
    diag_idle_proc_tbl[i].param = param;
      }
    osal_unlock_mutex(&diag_idle_process_mutex);
    }

     diag_set_internal_sigs(DIAG_INT_IDLE_PROCESSING_SIG);
  }

  return status;
}

/*=========================================================================*/
boolean
diag_idle_processing_unregister (diag_idle_processing_ftype fp, void *param)
{
  int i;
  boolean found = FALSE;
  boolean status = FALSE;   /* Default to unsuccessful */

  if (fp)
  {
    /* Match and remove from table. */
    for (i = 0; !found && i < DIAG_IDLE_PROC_CNT; i++)
    {
      osal_lock_mutex(&diag_idle_process_mutex);
      if (diag_idle_proc_tbl[i].fp == fp &&
      diag_idle_proc_tbl[i].param == param)
      {
        /* Found an empty table entry. */
        found = TRUE;
        status = TRUE;

        diag_idle_proc_tbl[i].fp = NULL;
        diag_idle_proc_tbl[i].param = NULL;
      }
      osal_unlock_mutex(&diag_idle_process_mutex);
    }
  }

  return status;
}

/*===========================================================================

FUNCTION DIAG_IDLE_PROCESSING

DESCRIPTION
  This procedure is called when the DIAG task loop has nothing else to do,
  i,e when diag is idle.

  This routine traverses the list of registered functions and calls them
  in order of registration.

  This enables clients to regain DIAG task context from async interfaces.

===========================================================================*/
static void
diag_idle_processing (void)
{
  static boolean inprogress = FALSE;
  int i;

  if (!inprogress)
  {
    /* Prevent reentrant calls. */
    inprogress = TRUE;

    /* Call all idle processing functions that are registered. */
    for (i = 0; i < DIAG_IDLE_PROC_CNT; i++)
    {
      osal_lock_mutex(&diag_idle_process_mutex);
      if (diag_idle_proc_tbl[i].fp)
      {
    diag_idle_proc_tbl[i].fp (diag_idle_proc_tbl[i].param);
      }
    osal_unlock_mutex(&diag_idle_process_mutex);
    }

    inprogress = FALSE;
  }

  return;
}

/*=========================================================================*/
/* Initial state = OK to sleep. */
/* I am inclined to make this static, but I think it would make it impossible to find for debugging. -- Richard */
uint32 diag_ballot_mask = 0;
/*This diag_sleep_state is updated in diag_allow_sleep() and diag_forbid_sleep() */
#ifndef SLEEP_DIAG_OKTS_SIG
diag_sleep_state_enum_type diag_sleep_state = DIAG_SLEEP_STATE_UNKNOWN;
#endif
void
diag_sleep_vote (diag_sleep_vote_enum_type diag_svc, boolean vote)
{
  uint32 vote_mask = 0;

  /* Assumption: the initial state is that sleep is not allowed.
     This is established in diag_init (). */

  if (diag_enable_sleep_voting && diag_svc < DIAG_SLEEP_CNT)
  {
    vote_mask = 1 << diag_svc;

    if (vote == TRUE)
    {
      /* This service is OK to sleep, 0 the bit. */
      DIAG_CLR_MASK (diag_ballot_mask, vote_mask);

      /* If not connected, always allow sleep. */
      if (!DIAG_CHECK_MASK (diag_cx_state, DIAG_CX_COMM_S) || diag_ballot_mask == 0)
      {
#ifndef SLEEP_DIAG_OKTS_SIG

        osal_lock_mutex(&diag_sleep_vote_mutex);

        if (diag_sleep_state != DIAG_SLEEP_STATE_ASLEEP)
        {
          diag_allow_sleep ();
          #if defined(FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING)&& !defined (IMAGE_APPS_PROC)
            clk_regime_deregister_for_cpu_resource(CLKRGM_CPU_CLIENT_DIAG);
          #endif
        }

        osal_unlock_mutex(&diag_sleep_vote_mutex);


#else
  /* No sleep vote filtering if using old sleep API */
  diag_allow_sleep ();
  #if defined(FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING)&& !defined (IMAGE_APPS_PROC)
    clk_regime_deregister_for_cpu_resource(CLKRGM_CPU_CLIENT_DIAG);
  #endif
#endif
      }
    }
    else
    {
      /* This service is not OK to sleep, set the bit to 1. */
      DIAG_SET_MASK (diag_ballot_mask, vote_mask);

      /* Must be connected to forbid sleep. */
      if (DIAG_CHECK_MASK (diag_cx_state, DIAG_CX_COMM_S) && diag_ballot_mask != 0)
      {
#ifndef SLEEP_DIAG_OKTS_SIG

         osal_lock_mutex(&diag_sleep_vote_mutex);

         if (diag_sleep_state != DIAG_SLEEP_STATE_AWAKE)
         {
           diag_forbid_sleep ();
           #if defined(FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING)&& !defined (IMAGE_APPS_PROC)
             clk_regime_register_for_cpu_resource(CLKRGM_CPU_CLIENT_DIAG);
           #endif
         }

         osal_unlock_mutex(&diag_sleep_vote_mutex);
#else
  /* No sleep vote filtering if using old sleep API */
  diag_forbid_sleep ();
  #if defined(FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING)&& !defined (IMAGE_APPS_PROC)
    clk_regime_register_for_cpu_resource(CLKRGM_CPU_CLIENT_DIAG);
  #endif
#endif
      }
    }
  }
}

/*=========================================================================*/
diag_security_type
diag_get_security_state (void)
{
  return diag_security;
}

/*=========================================================================*/
boolean
diag_set_security_state (diag_security_type security)
{
  MSG_1(MSG_SSID_DIAG, MSG_LEGACY_LOW,"Changing security state to 0x%x", security);

  diag_security = security;

  return TRUE;
}

/*=========================================================================*/
boolean
diag_set_sp_state (diag_sp_type sp)
{
  MSG_1(MSG_SSID_DIAG, MSG_LEGACY_LOW,"Changing service programming lock state to 0x%x", sp);

  diag_sp = sp;

  return TRUE;
}

/*=========================================================================*/
diag_sp_type
diag_get_sp_state (void)
{
  return diag_sp;
}

/*=========================================================================*/
#ifdef DIAG_CHECK_SECURITY
/* This is normally defined in dload.c */
uint8 const chk_security_code_ptr[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
#endif /* DIAG_CHECK_SECURITY */

boolean
diag_check_password (void *code_ptr)
{
#if defined DIAG_QDSP6_APPS_PROC
    return TRUE;
#else

  int status;
  void *ptr;

  if (code_ptr == NULL)
  {
    ptr = (void *) diag_no_security_code_c;
  }
  else
  {
    ptr = code_ptr;
  }

  status = memcmp (ptr, chk_security_code_ptr, DIAG_PASSWORD_SIZE);

  return (status == 0) ? TRUE : FALSE;
#endif /*  DIAG_QDSP6_APPS_PROC */
}               /* diag_check_password */

/*=========================================================================*/
static int
diag_searchlist_insert (diag_searchlist_node_type list[],
                        int count, int size,
                        diag_searchlist_node_type new_node)
{
  int i;

  if (list && count >= 0 && count < size)
  {
    for (i = count - 1; i >= 0 && list[i].key > new_node.key; i--)
    {
      /* Sort nodes from the end of the list until a match is found */
      list[i+1] = list[i];
    }

    /* 'i+1' offsets the i-- of the for loop */
    list[i+1] = new_node;

    count++;
  }

  return count;
}

/*=========================================================================*/
/* This routine performs a binary search on the given searchlist.

   If found, this routine returns the key.  If duplicate entries
   exist, this routine returns one of them.

   If not found, this routine returns -1. */
static int
diag_searchlist_search (unsigned int key,
                        diag_searchlist_node_type list[],
                        int count)
{
  int low, i, high;

  if (list)
  {
    low = (-1);
    high = count;

    while (high - low > 1)
    {
      i = (high + low) / 2;

      if (key <= list[i].key)
      {
        high = i;
      }
      else
      {
        low = i;
      }
    }

    if (key != list[high].key)
    {
      /* Force an exit of the loop since 'low' is never less than -1. */
      high = (-1);
    }
    else
    {
      /* 'high' contains the matched index */
    }
  }
  else
  {
    high = (-1);
  }

  return high;
}

/*=========================================================================*/
/* returns new count.  count will be the same if failed */
static int
diag_searchlist_remove (diag_searchlist_node_type list[],
                        int count,
                        diag_searchlist_node_type node,
                        diag_searchlist_match match_fp)
{
  int i, result;
  boolean found = FALSE;

  result = diag_searchlist_search (node.key, list, count);

  if (result >= 0)
  {
    /* Search before and after the found entry for the first match. */
    for (i = result; i < count && list[i].key == node.key; i++)
    {
      if (match_fp)
      {
        if (match_fp (&node, &list[i]))
        {
          found = TRUE;
          break; /* Avoid incrementing 'i' */
        }
      }
      else if (node.goo == list[i].goo)
      {
        found = TRUE;
        break; /* Avoid incrementing 'i' */
      }
    }

    if (!found)
    {
      for (i = result - 1; i >= 0 && list[i].key == node.key; i--)
      {
        if (match_fp)
        {
          if (match_fp (&node, &list[i]))
          {
            found = TRUE;
            break; /* Avoid incrementing 'i' */
          }
        }
        else if (node.goo == list[i].goo)
        {
          found = TRUE;
          break; /* Avoid incrementing 'i' */
        }
      }
    }

    if (found)
    {
      /* 'i' holds the index to the matched entry. */
      for (count--; i < count; i++)
      {
        /* Move entries up the array to replace the removed entry. */
        list[i] = list[i+1];
      }
    }
  }

  return (count);
}

/*=========================================================================*/
//NOTE: If 'notify' or 'payload' are NULL, the return value will indicate
//existance of the key, but no action is taken to deliver the node.
boolean
diag_searchlist_search_all (unsigned int key,
                            diag_searchlist_type *list,
                            void *payload,
                            diag_searchlist_notify notify)
{
  int i, count;
  int result = -1;
  uint32 seq_num = 0;
  diag_searchlist_node_type *r = NULL;
  boolean found = FALSE;

  /* !!! This function assumes the caller is not in IRQ mode since it
     is not part of the externalized API of listeners. */
  if (list && list->initialized && !os_is_in_irq_mode ())
  {
    /* Replaced rex critical sections with intlocks, since we cannot
    ** acquire a lock on critcal section when tasks are locked.
    */

    count = list->count;
    r = list->r;

    result = diag_searchlist_search (key, r, count);

    found = (result >= 0) ? TRUE : FALSE;

    /* Only notify and search for duplicates if 'notify' is specified. */
    if (found && notify && payload)
    {
       /* Increment the sequence number for each successful search. */
       seq_num = list->seq_num + 1;

      /* Search before and after the found entry for duplicate entries. */
      for (i = result; i < count && r[i].key == key; i++)
      {
        notify (seq_num, payload, &r[i]);
      }

      for (i = result - 1; i >= 0 && r[i].key == key; i--)
      {
        notify (seq_num, payload, &r[i]);
      }

      /* Save updated sequence number */
      list->seq_num = seq_num;
    }
  }

  return (found);
}

/*=========================================================================*/
boolean
diag_searchlist_add (diag_searchlist_type *list, unsigned int key, void *goo)
{
  int new_count = 0;
  diag_searchlist_node_type node;
  diag_searchlist_node_type *new_r = NULL; /* If allocating a new array */
  boolean success = FALSE;

  /* Fill in the node - this is passed by value */
  node.key = key;
  node.goo = goo;

  if (list && !os_is_in_irq_mode ())
  {
    if (!list->initialized)
    {

      osal_lock_mutex(&diag_srch_list_mutex);

      /* This check is done twice to avoid mutexing for every check
         of initialization and multiple initialization.
         Too bad C doesn't have constructors... */
      if (!list->initialized)
      {
        osal_init_crit_sect (&list->crit_sect);
        list->initialized = TRUE;
        list->seq_num = 0;
      }

      osal_unlock_mutex(&diag_srch_list_mutex);


    }

    /* Enter critical section to avoid corrupt list */
    osal_enter_crit_sect (&list->crit_sect);

    /* If the list doesn't have an array allocated, allocate one. */
    if (!list->r)
    {
      list->count = 0;
      list->size = DIAG_SEARCHLIST_SIZE_INCR;

      list->r = (diag_searchlist_node_type *) malloc (
        (uint32)(list->size) * sizeof (diag_searchlist_node_type));

      if (!list->r)
      {
        /* reset size */
        list->size = 0;
      }
    }

    if (list->r)
    {
#ifdef FEATURE_DEBUG_DIAG_HEAP
      diag_searchlist_add_malloc_cnt++;
      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
            "MALLOC diag_searchlist_add_malloc_cnt = %d, allocated bytes = %d ",
            diag_searchlist_add_malloc_cnt,
            ((list->size) * sizeof (diag_searchlist_node_type)));
#endif /* FEATURE_DEBUG_DIAG_HEAP */

      if (list->count >= list->size)
      {
        /* There isn't enough room in this array to add.  Allocate
           a new one and, if successful, copy the old list. */
        new_r = (diag_searchlist_node_type *) malloc (
          ((uint32)(list->size) + DIAG_SEARCHLIST_SIZE_INCR) *
          sizeof (diag_searchlist_node_type));

        if (new_r)
        {
#ifdef FEATURE_DEBUG_DIAG_HEAP
          diag_searchlist_add_malloc_cnt++;
          MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
            "MALLOC diag_searchlist_add_malloc_cnt = %d, allocated bytes = %d ",
             diag_searchlist_add_malloc_cnt,
             (((list->size) + DIAG_SEARCHLIST_SIZE_INCR) *
                            sizeof (diag_searchlist_node_type)));
#endif /* FEATURE_DEBUG_DIAG_HEAP */

          /* Only copy valid entries */
          memscpy (new_r, (((uint32)(list->size) + DIAG_SEARCHLIST_SIZE_INCR) * sizeof (diag_searchlist_node_type)),
                   list->r, (uint32)(list->count) * sizeof (diag_searchlist_node_type));

          list->size += DIAG_SEARCHLIST_SIZE_INCR;

          /* free the old list and assign the new array to this list */
          free (list->r);
#ifdef FEATURE_DEBUG_DIAG_HEAP
          diag_searchlist_add_free_cnt++;
          MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
                "FREE diag_process_rx_free_cnt = %d ",
                 diag_searchlist_add_free_cnt);
#endif /* FEATURE_DEBUG_DIAG_HEAP */

          list->r = new_r;
          new_r = NULL;
        }
      }

      if (list->count < list->size)
      {
        new_count = diag_searchlist_insert (list->r, list->count,
                                            list->size, node);

        if (new_count > list->count)
        {
          list->count = new_count;
          success = TRUE;
        }
        else if (list->count == 0)
        {
          free (list->r);
#ifdef FEATURE_DEBUG_DIAG_HEAP
          diag_searchlist_add_free_cnt++;
          MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
                "FREE diag_process_rx_free_cnt = %d ",
                diag_searchlist_add_free_cnt);
#endif /* FEATURE_DEBUG_DIAG_HEAP */
          list->size = 0;
          list->r = NULL;
        }
      }
    }

    osal_exit_crit_sect (&list->crit_sect);
  }

  return success;
}

/*=========================================================================*/
boolean
diag_searchlist_delete (diag_searchlist_type *list,
                        diag_searchlist_node_type node,
                        diag_searchlist_match match_fp)
{
  int new_count = 0;
  diag_searchlist_node_type *new_r = NULL; /* If allocating a new array */
  boolean success = FALSE;

  if (list && list->initialized && !os_is_in_irq_mode ())
  {
    /* Enter critical section to make sure the list doesn't change */
    osal_enter_crit_sect (&list->crit_sect);

    if (list->r)
    {
      new_count = diag_searchlist_remove (
        list->r, list->count, node, match_fp);

      if (new_count < list->count)
      {
        list->count = new_count;
        success = TRUE;

        if (new_count == 0)
        {
          free (list->r);
#ifdef FEATURE_DEBUG_DIAG_HEAP
          diag_searchlist_delete_free_cnt++;
          MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
                "FREE diag_searchlist_delete_free_cnt= %d,heap used = %d ",
                diag_searchlist_delete_free_cnt,diag_heap_used_bytes());
#endif /* FEATURE_DEBUG_DIAG_HEAP */

          list->r = NULL;
          list->size = 0;
        }
        else if (list->size - new_count >= DIAG_SEARCHLIST_SIZE_INCR)
        {
          /* Delete crossed the increment threshold.  Alloc a smaller
             list and, if successful, copy the old list. */
          new_r = (diag_searchlist_node_type *) malloc (
            ((uint32)(list->size) - DIAG_SEARCHLIST_SIZE_INCR) *
            sizeof (diag_searchlist_node_type));

          if (new_r)
          {
#ifdef FEATURE_DEBUG_DIAG_HEAP
            diag_searchlist_delete_malloc_cnt++;
            MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
                "MALLOC diag_searchlist_delete_malloc_cnt=%d,heapUsed=%d ",
                diag_searchlist_delete_malloc_cnt,diag_heap_used_bytes());
#endif /* FEATURE_DEBUG_DIAG_HEAP */

            /* Only copy valid entries */
            memscpy (new_r, (((uint32)(list->size) - DIAG_SEARCHLIST_SIZE_INCR) *
            sizeof (diag_searchlist_node_type)), list->r,
                    (uint32)new_count * sizeof (diag_searchlist_node_type));

            /* free the old list and assign the new array to this list */
            free (list->r);

#ifdef FEATURE_DEBUG_DIAG_HEAP
            diag_searchlist_delete_free_cnt++;
            MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
                "FREE diag_searchlist_delete_free_cnt= %d,heap used = %d ",
                diag_searchlist_delete_free_cnt, diag_heap_used_bytes());
#endif /* FEATURE_DEBUG_DIAG_HEAP */

            list->r = new_r;
            list->size -= DIAG_SEARCHLIST_SIZE_INCR;
            new_r = NULL;
          }
        }
      }
    }

    osal_exit_crit_sect (&list->crit_sect);
  }

  return success;
}

/*=========================================================================*/
boolean
diag_cmd_req (const byte *req, unsigned int length,
              const diag_cmd_rsp rsp_cb, void *param)
{
  diag_req_cmd_type *cmd = NULL;
  boolean success = FALSE;

  if (req && length > 0)
  {
    cmd = (diag_req_cmd_type *) diag_malloc (
      FPOS (diag_req_cmd_type, req) + length);

    if (cmd)
    {
#ifdef FEATURE_DEBUG_DIAG_HEAP
      diag_cmd_req_cnt++;
      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
        "MALLOC diag_cmd_req_cnt = %d, heap used=%d ",diag_cmd_req_cnt,
          diag_heap_used_bytes());
#endif /* FEATURE_DEBUG_DIAG_HEAP */

      cmd->rsp_func = rsp_cb;
      cmd->param = param;
      cmd->length = length;
      cmd->bFromDiagCmdQ = TRUE;
      memscpy (cmd->req, length, req, length);

      q_put (&diag_req_cmd_q, q_link (cmd, &cmd->link));

#ifdef FEATURE_DEBUG_DIAG_HEAP
      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
        "Q_PUT diag_cmd_req diag_req_cmd_q_cnt = %d heap used = %d",
        q_cnt (&diag_req_cmd_q),  diag_heap_used_bytes());
#endif /* FEATURE_DEBUG_DIAG_HEAP */

      /* DIAG checks this queue as part of its normal RX processing. */
      diag_rx_notify ();

      success = TRUE;
    }
  }

  /* The handle is the address of the command buffer */
  return success;
}

#if defined (DIAG_FEATURE_EFS2)
/*===========================================================================

FUNCTION DIAG_FS_INIT

DESCRIPTION
  This function initializes diag filestream mode, enabling it if applicable.

===========================================================================*/
static void
diag_fs_init (diag_fs_type * fs)
{
  int fs_rsp;
  fs_off_t result;

  if (fs && fs->handle < 0)
  {
    fs_rsp = efs_open (fs->fname, O_RDONLY);

    if (fs_rsp < 0)
    {
        /***********************************************************************
           no error case as if the file does not exist then nothing is read from
           it and configured. This can be in case of a fresh boot or an erase of
           efs.
         ***********************************************************************/
    }
    else
    {
        fs->handle = fs_rsp;

        result = efs_lseek(fs->handle, 0, SEEK_CUR);
        if (result < 0)
      {
            diag_fs_terminate(fs);
      }
      else
      {
            fs->read_pos = (dword)result;
      }
    }
  }
}

/*===========================================================================

FUNCTION DIAG_PROCESS_INPUT_FILE

DESCRIPTION
  This function processes inbound data from the specified file.

RETURN VALUE
  None.

===========================================================================*/
static void diag_process_input_file (diag_fs_type * fs,void *buf,
                                     unsigned int size)
{
  MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW,"Processing input file");

  diag_fs_process_inbound (fs, buf, size);

  /* This is unnecessary if the input file is closed. */
  if (fs->handle >= 0)
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW,"Attempting inbound comm buffering.");
    diag_fs_inbound_comm (&diag_fs_inbuf, buf, size);
  }
}

/*===========================================================================

FUNCTION DIAG_FS_INBOUND_MODE

DESCRIPTION
  This function handles all inbound operations for the filestream mode.

===========================================================================*/
static boolean
diag_fs_inbound_mode (void *buf, unsigned int size)
{
  static boolean initialized = FALSE;
  boolean filestream_mode_active = FALSE;

  if (!initialized)
  {
    initialized = TRUE;

    /* Initialize diag input file streams. */
    diag_fs_init (&diag_fs_config);

    diag_fs_init (&diag_fs_in);
  }

  ASSERT (buf && size > 0);

  /* If filestream mode is active, process the input file, then
     detect valid comm layer connection and buffer any inbound data.
     Once the input file is complete, the buffered inbound data will
     be processed. */
  if (diag_fs_config.handle >= 0)
  {
    filestream_mode_active = TRUE;
    diag_process_input_file(&diag_fs_config, buf, size);
  }
  else if (diag_fs_in.handle >= 0)
  {
    filestream_mode_active = TRUE;
    diag_process_input_file(&diag_fs_in, buf, size);
  }

  else if (diag_fs_inbuf.handle >= 0)
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW,"Input file complete");
    filestream_mode_active = TRUE;

    if (DIAG_CHECK_MASK (diag_cx_state, DIAG_CX_COMM_S))
    {
      /* Since we have established a connection, the temporary file must
         contain at least one good packet. */
      MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW,"Processing buffered comm input");
      diag_fs_process_inbound (&diag_fs_inbuf, buf, size);

      if (diag_fs_inbuf.handle >= 0)
      {
        /* Some data still exists in the file, continue comm layer buffering,
           if inbound comm layer data exists.  If this state continues too long,
           the max file size of the buffer file will be reached and no more data
           will be written.  At that point, the processing of the stream can
           "catch up" to the comm layer buffering. */
        MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW,"Attempting buffer of inbound comm");
        diag_fs_inbound_comm (&diag_fs_inbuf, buf, size);
      }
    }
    else
    {
      /* At this point, the procesing of the original input file is
         complete and we haven't yet established a connection on the comm
         layer.  If the buffer file has data, it may or may not be useful
         data.  Since the buffer emulates normal comm layer processing,
         "catch up" to the comm layer and then exit filestream mode. */
      MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW,"Catch up: buffered comm input");

      while (diag_fs_inbuf.handle >= 0)
      {
        diag_fs_process_inbound (&diag_fs_inbuf, buf, size);
      }

      /* Input buffer processing is complete, remove the file. */
      diag_fs_terminate (&diag_fs_inbuf);
    }
  }

  return filestream_mode_active;
}

/*===========================================================================

FUNCTION DIAG_FS_TERMINATE

DESCRIPTION
  This function closes and deletes the specified file.

===========================================================================*/
static void
diag_fs_terminate (diag_fs_type * fs)
{
  int fs_rsp;

  if(fs)
  {
    if (fs->handle >= 0)
    {
      /* Clean up input file */
      fs_rsp = efs_close(fs->handle);
      if(fs_rsp == -1)
        MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR,"efs_close() failed");
      fs->handle = -1;
    }

    /* Remove the input file. */
      if (efs_unlink(fs->fname))
      ERR ("Input file delete fail! Stat: %d", efs_errno, 0, 0);
  }
}

/*===========================================================================

FUNCTION DIAG_FS_READ

DESCRIPTION
  This function reads inbound data from the specified filestream.

RETURN VALUE
  Number of bytes read.

===========================================================================*/
static uint32
diag_fs_read (diag_fs_type * fs, void *buf, unsigned int size)
{
  int fs_rsp;
  fs_off_t result;
  unsigned int read_count = 0;

  if (fs && buf && fs->handle >= 0 && size > 0)
  {
    result = efs_lseek (fs->handle, fs->read_pos, SEEK_SET);

    if (result >= 0)
    {
      fs_rsp = efs_read (fs->handle, buf, (fs_size_t) size);

      if (fs_rsp != 0 && fs_rsp != -1)
      {
        read_count = fs_rsp;

        result = efs_lseek (fs->handle, 0, SEEK_CUR);

        if (result >= 0)
        {
            fs->read_pos = (dword)result;
        }
        else
        {
          diag_fs_terminate (fs);
        }
      }
      else
      {
        diag_fs_terminate (fs);
      }
    }
    else
    {
      diag_fs_terminate (fs);
    }
  }

  return read_count;
}

/*===========================================================================

FUNCTION DIAG_FS_WRITE

DESCRIPTION
  This function writes data to the file specified by 'fs'.

RETURN VALUE
  Boolean indicating success.

===========================================================================*/
static boolean
diag_fs_write (diag_fs_type * fs, void *buf, unsigned int size)
{
  fs_off_t result;
  fs_ssize_t write_result;
  boolean success = FALSE;

  if (fs && fs->handle)
  {
    /* Assumption: all writes are at the end of the file. */
    result = efs_lseek (fs->handle, (fs_off_t) fs->write_pos, SEEK_SET);

    if (result >= 0)
    {

      write_result = efs_write (fs->handle, buf, (fs_size_t) size);

      if (write_result != -1 && write_result == size)
      {
        success = TRUE;
        fs->write_pos += size;
      }
      else
      {
         diag_fs_terminate (fs);
      }
    }
    else
    {
      diag_fs_terminate (fs);
    }
  }
  else
  {
    diag_fs_terminate (fs);
  }

  return success;
}

/*===========================================================================

FUNCTION DIAG_FS_PROCESS_INBOUND

DESCRIPTION
  This function processes inbound data from the specified file.

RETURN VALUE
  None.

===========================================================================*/
static void
diag_fs_process_inbound (diag_fs_type * fs, void *buf, unsigned int size)
{
  diag_hdlc_decode_type hdlc;
  unsigned int src_buf_size = 0;

  /* Length of packet, if boundary is found.
     0 is returned if no packet found. */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Initialize HDLC structure */
  memset (&hdlc, 0, sizeof (hdlc));

  /* Allocate a buffer from the comm layer to use as a work buffer
     for the file stream input processing. */
  src_buf_size = 256;
  hdlc.src_ptr = diag_malloc (src_buf_size);

  if (hdlc.src_ptr)
  {
#ifdef FEATURE_DEBUG_DIAG_HEAP
    diag_fs_process_inbound_malloc_cnt++;
    MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
        "MALLOC diag_fs_process_inbound_malloc_cnt = %d,heap used=%d ",
            diag_fs_process_inbound_malloc_cnt,diag_heap_used_bytes());
#endif /* FEATURE_DEBUG_DIAG_HEAP */

    if (fs && fs->handle >= 0)
    {
      hdlc.dest_ptr = buf;
      hdlc.dest_size = size;

      do
      {
        /* If the src buffer has been exhausted, time to read more */
        if (hdlc.src_size == hdlc.src_idx)
        {
          hdlc.src_size = (uint32)diag_fs_read (fs, hdlc.src_ptr, src_buf_size);

          if (hdlc.src_size > 0)
          {
            hdlc.src_idx = 0;
          }
          else
          {
            diag_fs_terminate (fs);
            break;
          }
        }

        if (hdlc.src_size > hdlc.src_idx)
        {
          if (diag_extract_pkt (&hdlc) == TRUE)
          {

            /* dest_idx is the length of the decoded packet */
            diagpkt_process_request (buf, (uint16) hdlc.dest_idx, NULL, NULL,FALSE);

            /* If src_ptr is not exhausted, update read_pos before exit. */
            if (hdlc.src_size > hdlc.src_idx)
            {
              fs->read_pos -= hdlc.src_size - hdlc.src_idx;
            }
            break;
          }
        }
      }
      while (fs->handle >= 0);
    }

    diag_free (hdlc.src_ptr);
#ifdef FEATURE_DEBUG_DIAG_HEAP
    diag_fs_process_inbound_free_cnt++;
    MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
        "FREE diag_fs_process_inbound_free_cnt = %d, heap used = %d ",
            diag_fs_process_inbound_free_cnt,diag_heap_used_bytes());
#endif /* FEATURE_DEBUG_DIAG_HEAP */
  }
}

/*===========================================================================

FUNCTION DIAG_FS_INBOUND_COMM

DESCRIPTION
  This function buffers inbound comm layer data to the file 'fs'.

  A temporary buffer is used to decode the inbound data and check for a
  valid packet.

RETURN VALUE
  None.

===========================================================================*/
static void
diag_fs_inbound_comm (diag_fs_type * fs, void *buf, unsigned int size)
{
  int fs_rsp;
  fs_off_t result;
  struct fs_stat fs_stat_buf;
  diag_hdlc_decode_type hdlc;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Initialize HDLC structure */
  memset (&hdlc, 0, sizeof (hdlc));

  if (fs)
  {
    if (fs->handle < 0)
    {
      if (!(efs_stat (fs->fname, &fs_stat_buf)))
      {
        /* here no error checking is done as we are only dealing with regular files
           so if the efs_stat fails we do nothing here. now if an unlink fails we
           are not in an error case as we open a new same file again which should
           overwrite the earlier file.*/
        efs_unlink (fs->fname);
      }  /* If file stat fails, then unlink the file  */

      fs_rsp = efs_open (fs->fname, O_CREAT | O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);

      if (fs_rsp >= 0)
      {
        fs->handle = fs_rsp;
        result = efs_lseek (fs->handle, 0, SEEK_CUR);

        if (result >= 0)
        {
          fs->read_pos = (dword)result;
          fs->write_pos = (dword)result;
        }
      }
    }
    /* Initialize HDLC structure w/ working buffer. */
    hdlc.dest_ptr = buf;
    hdlc.dest_size = size;

    /* If the file is open, buffer to the file. */
    while (fs->handle >= 0)
    {

      if (hdlc.src_ptr == NULL)
      {

        /* Make sure the file is not at max size yet. */
        if (fs->write_pos < fs->max_size)
        {
          /* Get a buffer from the comm layer.  If a valid packet is found,
          declare a "connected" status and buffer the input to the file. */
          hdlc.src_ptr = diagcomm_inbound (&hdlc.src_size, DIAGCOMM_PORT_1);
          hdlc.src_idx = 0;
        }
      }

      if (hdlc.src_ptr == NULL)
      {
        break;
      }

      if (DIAG_CHECK_MASK (diag_cx_state, DIAG_CX_COMM_S))
      {
        /* We are already connected, so just buffer the data and move on. */
        (void) diag_fs_write (fs, hdlc.src_ptr, hdlc.src_size);
        diagcomm_inbound_complete (hdlc.src_ptr);
        hdlc.src_ptr = NULL;
      }
      else
      {
        /* Not connected - must look for a valid packet to connect. */
        if (hdlc.src_idx < hdlc.src_size)
        {
          if (diag_extract_pkt (&hdlc) == TRUE)
          {
            /* Valid packet means we can assume a connection. */
            DIAG_SET_MASK (diag_cx_state, DIAG_CX_COMM_S);
          }
        }
        else
        {
          /* The input buffer has been exhausted.  Free the buffer.  The
             next iteration of the loop will grab a new one. */
          if (hdlc.dest_idx > 0)
          {
            /* This indicates that a decode is in progress.  Write the
               buffer to the file in case it is part of the first
               valid packet. */
            (void) diag_fs_write (fs, hdlc.src_ptr, hdlc.src_size);
          }
          else
          {
            /* Delete any accumulated data thus far and reopen.  Since
               there is no decode in progress, and diag is not connected,
               all inbound data thus far is garbage. */
            diag_fs_terminate (fs);
          }

          /* Finished with this inbound buffer. */
          diagcomm_inbound_complete (hdlc.src_ptr);
          hdlc.src_ptr = NULL;
        }
      }
    }
  }
}

/*===========================================================================

FUNCTION DIAG_FS_PROCESS_CONFIG

DESCRIPTION
  This function checks if the given packet has potentially changed the
  existing diag config file.  If so, a new file is generated and writen
  to the file.

===========================================================================*/
static void
diag_fs_process_config (void *req_pkt, word pkt_len)
{
  (void) pkt_len; /* suppress compiler warning */

  if (req_pkt)
  {
    switch (diagpkt_get_cmd_code (req_pkt))
    {
      //TODO: For now, only event reporting is supported.
      case DIAG_EVENT_REPORT_F:
        /* Save the diagnostics service configuration for the next power cycle. */
        diag_fs_write_config ();
        break;
      default:
        break;
    }
  }
}

/*===========================================================================

FUNCTION DIAG_FS_WRITE_CONFIG

DESCRIPTION
  This function saves the diagnostic services' configurations to a file to be
  read at the next power cycle.  The config packet is stored in the form of
  a raw (HDLC encoded) input stream with commands to configure the
  appropriate services.

===========================================================================*/
extern uint8 event_config;  //XXX
word diag_do_escaping (const byte * src_ptr, byte * dest_ptr, word src_length,
               word dest_length, unsigned int *src_used);

static void
diag_fs_write_config (void)
{
  union
  {
    event_cfg_req_type pkt;
    byte buf[sizeof (event_cfg_req_type) + 3];
  }
  event_pkt;
  uint16 event_pkt_len = sizeof (event_cfg_req_type);
  unsigned int encoded_pkt_len = 0;

  byte buf[20];         //XXX
  word crc;
  diag_fs_type fs;
  int fs_rsp;

  memset (&fs, 0, sizeof (diag_fs_type));

  fs.handle = -1;
  fs.fname = ".DIAGCFG";
  fs.max_size = 0x8000;     /* 32K is more than enough - it will never get this big. */

  /* here no error checking is done as we are only dealing with regular files
     so if the unlink fails we are not in an error case as we open a new same
     file again which should overwrite the earlier file.*/
  efs_unlink (fs.fname);

  fs_rsp = efs_open (fs.fname, O_CREAT | O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);

  if (fs_rsp >= 0)
  {
    fs.handle = fs_rsp;
  }

  if (fs.handle >= 0)
  {
    diagpkt_set_cmd_code (&event_pkt, DIAG_EVENT_REPORT_F);

    event_pkt.pkt.enable = (event_config != 0) ? TRUE : FALSE;

    crc = crc_16_l_calc ((void *) &event_pkt,
             (uint16) (sizeof (event_cfg_req_type) * 8));

    /* Write the CRC to the end of unescaped packet.
       ** The CRC will be escaped with everything else.
     */
    event_pkt.buf[event_pkt_len++] = crc & 0x00ff;
    event_pkt.buf[event_pkt_len++] = crc >> 8;

    /* Reuse event_pkt_len to now hold size of HDLC encoded packet */
    event_pkt_len = diag_do_escaping (event_pkt.buf, buf, event_pkt_len, 20,
                      &encoded_pkt_len);
    if(event_pkt_len < 20)
    {
        buf[event_pkt_len++] = DIAG_ASYNC_CONTROL_CHAR;
    }

    (void) diag_fs_write (&fs, buf, event_pkt_len);

    efs_close (fs.handle);
  }
}


/*===========================================================================

FUNCTION DIAG_DO_ESCAPING

DESCRIPTION
  This routine provides XOR escaping functionality for the use of callers.
  Both Control character and Escape character will be escaped by substituting
  the character with the following two characters:

  <DIAG_ASYNC_ESC_CHAR> <src_byte ^ DIAG_ASYNC_ESC_MASK>

DEPENDENCIES
  src_ptr & dest_ptr cannot be the same nor overlapping.

RETURN VALUE
  Returns the number of bytes that were stored into dest_ptr.

  Also, via pointers, returns src_used.

===========================================================================*/
word
diag_do_escaping (
  const byte *src_ptr,
  /* This is the pointer that contains the data that needs to be
  ** escaped
  */

  byte       *dest_ptr,
  /* This is the pointer into which the escaped data should be
  ** stored.
  */

  word        src_length,
  /* This is the number of bytes to convert.
  */

  word        dest_length,
  /* This is the number of bytes that can be stored in dest_ptr.
  */

  unsigned int *src_used
  /* This will contain the number of values which were read from
  ** src_ptr.
  */
)
{
  word i;
  byte src_byte;
  word converted_length = 0;
  word check_size;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (dest_length >= DIAG_ESC_EXPAND_SIZE);
  ASSERT (src_ptr != NULL);
  ASSERT (dest_ptr != NULL);

  if (dest_length >= (src_length << DIAG_ESC_EXPAND_SHIFT))
  {
    /* Do not need to check for size for each character
     */
    converted_length = src_length;

    for (i = src_length; i > 0; i--)
    {
      src_byte = *src_ptr++;

      if ((src_byte == DIAG_ASYNC_CONTROL_CHAR) ||
      (src_byte == DIAG_ASYNC_ESC_CHAR))
      {

        *dest_ptr++ = DIAG_ASYNC_ESC_CHAR;
        *dest_ptr++ = src_byte ^ DIAG_ASYNC_ESC_MASK;

        /* Destination became one character
        ** larger than projected.
        */
        converted_length++;
      }
      else
      {
        *dest_ptr++ = src_byte;
      }
    }               /* for i=0; i<src_length; i++ */

    *src_used = src_length;
  }             /* if dest_length >= src_length << 1 */
  else
  {
    check_size = dest_length - DIAG_ESC_EXPAND_SIZE;

    /* Do need to check for size.  Right now check size for
       ** each character.  Could further optimize, which would
       ** make code more complicated but faster.
     */

    for (i = src_length; i > 0; i--)
    {
      src_byte = *src_ptr++;

      if ((src_byte == DIAG_ASYNC_CONTROL_CHAR) ||
      (src_byte == DIAG_ASYNC_ESC_CHAR))
      {

        *dest_ptr++ = DIAG_ASYNC_ESC_CHAR;
        *dest_ptr++ = src_byte ^ DIAG_ASYNC_ESC_MASK;
        converted_length += DIAG_ESC_EXPAND_SIZE;
      }
      else
      {
        *dest_ptr++ = src_byte;
        converted_length++;
      }

      if (converted_length > check_size)
      {
        i--;
        break;
      }
    }               /* for i=0; i<src_length; i++ */

    *src_used = src_length - i;
  }             /* else if dest_length < src_length << 1 */

  return converted_length;
}               /* diag_do_escaping */
#endif /* DIAG_FEATURE_EFS2  */

/*===========================================================================

FUNCTION DIAGPKT_CHECK_VALIDITY_REQUEST_ONDEVICE

DESCRIPTION
  This procedure checks if the given request is one of the few commands
  which is allowed to be processed.

RETURN VALUE
  Boolean indicating success.

===========================================================================*/
boolean diagpkt_check_validity_request_ondevice (void * buf)
{
  uint8 packet_id;
  uint8 subsys_id;
  uint16 ss_cmd;

  if(!buf)
    return FALSE;

  packet_id = diagpkt_get_cmd_code (buf);

  if (packet_id == 0x4B)
  {
    subsys_id = diagpkt_subsys_get_id (buf);
    ss_cmd = diagpkt_subsys_get_cmd_code (buf);
    /* Packets with SSID which are allowed */
    switch (subsys_id) {
      case 0x04: /* DIAG_SUBSYS_WCDMA */
        if ((ss_cmd == 0) || (ss_cmd == 0xF))
          return TRUE;
        break;
      case 0x08: /* DIAG_SUBSYS_GSM */
        if ((ss_cmd == 0) || (ss_cmd == 0x1))
          return TRUE;
        break;
      case 0x09: /* DIAG_SUBSYS_UMTS */
      case 0x0F: /* DIAG_SUBSYS_CM */
        if (ss_cmd == 0) return TRUE;
        break;
      case 0x0C: /* DIAG_SUBSYS_OS */
        if ((ss_cmd == 2) || (ss_cmd == 0x100)) return TRUE; /* MPU and APU */
        break;
      case 0x12: /* DIAG_SUBSYS_DIAG_SERV */
        if ((ss_cmd == 0) || (ss_cmd == 0x6) || (ss_cmd == 0x7) || (ss_cmd == 0x0060) || (ss_cmd == 0x0061) || (ss_cmd == 0x0062))
          return TRUE;
        break;
      case 0x13: /* DIAG_SUBSYS_FS */
        if ((ss_cmd == 0) || (ss_cmd == 0x1))
          return TRUE;
        break;
      default:
        return FALSE;
    }
  }
  else
  {
    switch (packet_id) {
      case 0x00:    /* Version Number */
      case 0x0C:    /* CDMA status packet */
      case 0x1C:    /* Diag Version */
      case 0x1D:    /* Time Stamp */
      case 0x60:    /* Event Report Control */
      case 0x63:    /* Status snapshot */
      case 0x73:    /* Logging Configuration */
      case 0x7C:    /* Extended build ID */
      case 0x7D:    /* Extended Message configuration */
      case 0x81:    /* Event get mask */
      case 0x82:    /* Set the event mask */
        return TRUE;
      default:
        return FALSE;
    }
  }
  return FALSE;
}

void diag_control_sleep_voting(unsigned int enable_vote)
{
  diag_enable_sleep_voting = enable_vote;
}

void diag_ctrl_drain_timer_len(unsigned int drain_timer_len)
{
  diag_drain_timer_len = drain_timer_len;

  /* If mode is real time, set the TX signal to start draining from diagbuf
        without waiting for the drain timer to expire  */

   if(diag_real_time_mode)
      diag_tx_notify();

  if (drain_timer_len > 0)
    diag_set_drain_timer();
  else
  diag_clr_drain_timer();


}

/*===========================================================================

FUNCTION DIAG_GET_INTERNAL_SIGS

DESCRIPTION
    Retrieves the mask of internal signals set. This function should be used
    to read diag_internal_sigs instead of accessing it directly to avoid
    concurrency issues.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  Returns the 32bit internal signal mask value.

SIDE EFFECTS

===========================================================================*/
dword diag_get_internal_sigs(void )
{
  osal_sigs_t return_sigs;

  osal_lock_mutex(&diag_internal_sig_mutex);
  return_sigs= diag_internal_sigs;
  osal_unlock_mutex(&diag_internal_sig_mutex);

  return return_sigs;

} /* diag_get_internal_sigs */
/*===========================================================================

FUNCTION DIAG_SET_INTERNAL_SIGS

DESCRIPTION
  Sets Diag internal signal mask, which also sets DIAG_INTERNAL_SIG. When
  diag_handle_sigs() handles DIAG_INTERNAL_SIG, diag_handle_internal_sig()
  is executed to handle all Diag internal sigs. This function should be used
  to update diag_internal_sigs to avoid concurrency issues.

PARAMETERS
  sigs - Diag internal signals to set.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
dword diag_set_internal_sigs( dword sigs )
{
  osal_sigs_t return_sigs;
  int return_val = 0;
  dword prev_sigs = 0;

  prev_sigs = diag_internal_sigs;
  osal_lock_mutex(&diag_internal_sig_mutex);
  diag_internal_sigs |= sigs;

  return_val = osal_set_sigs( &diag_tcb, DIAG_INTERNAL_SIG, &return_sigs );
  ASSERT(OSAL_SUCCESS == return_val);

  osal_unlock_mutex(&diag_internal_sig_mutex);

  return prev_sigs;

} /* diag_set_internal_sigs */


/*===========================================================================

FUNCTION DIAG_CLEAR_INTERNAL_SIGS

DESCRIPTION
  Clears Diag internal sig.  If all Diag internal sigs are cleared,
  DIAG_INTERNAL_SIG is also cleared. This function should be used
  to update diag_internal_sigs to avoid concurrency issues.

PARAMETERS
  sigs - Diag internal signals to clear.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
dword diag_clear_internal_sigs( dword sigs )
{
  osal_sigs_t return_sigs;
  int return_val = 0;
  dword prev_sigs = 0;

  osal_lock_mutex(&diag_internal_sig_mutex);
  prev_sigs = diag_internal_sigs;
  sigs = ~sigs;
  diag_internal_sigs &= sigs;

  /* Clear signal if no more internal signals are set */
  if( diag_internal_sigs ==0 )
  {
    return_val = osal_reset_sigs(&diag_tcb, DIAG_INTERNAL_SIG, &return_sigs);
    ASSERT(OSAL_SUCCESS == return_val);
  }
  osal_unlock_mutex(&diag_internal_sig_mutex);

  return prev_sigs;

} /* diag_clear_internal_sigs */


/*===========================================================================

FUNCTION DIAG_SET_CURRENT_PRESET_MASK_INDEX

DESCRIPTION

PARAMETERS
  Preset mask index to set.

DEPENDENCIES
  None

RETURN VALUE
  0 - Success
  Otherwise, error:
    1 - Preset id not supported.

SIDE EFFECTS

===========================================================================*/
uint8 diag_set_current_preset_mask_id( uint8 preset_id )
{
  uint8 max_num_presets = DIAG_MAX_PRESET_ID;
  uint8 ret_val = DIAG_SET_PRESET_ID_SUCCESS;

  if( (preset_id < DIAG_MIN_PRESET_ID) || (preset_id > DIAG_MAX_PRESET_ID) )
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR,
         "diag_set_current_preset_mask_id - Preset %d not supported.", preset_id);
    return DIAG_SET_PRESET_ID_NOT_SUPPORTED;
  }

  if( max_num_presets == 0 )
  {
    /* Did not recieve preset num from a Slave, set to default. */
    diag_cur_preset_id = DIAG_PRESET_MASK_1;
    ret_val = DIAG_SET_PRESET_ID_NOT_SUPPORTED;
  }
  else if( preset_id > max_num_presets )
  {
    /* Don't change preset mask if not supported by all Slaves. */
    return DIAG_SET_PRESET_ID_NOT_SUPPORTED;
  }
  else
  {
    /* Finally, switch the current presetid. */
    diag_cur_preset_id = preset_id;

    //ToDo: what if some slave supports presets but are late?
    //ToDo: what if a slave is late and turns out it supports less presets?
  }

  diag_switch_event_preset_mask();
  diag_switch_log_preset_mask();
  diag_switch_msg_preset_mask();

  return ret_val;

} /* diag_set_current_preset_mask_id */


/*===========================================================================

FUNCTION DIAG_GET_CURRENT_PRESET_MASK_ID

DESCRIPTION
  Retrieve the current preset mask id.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  Current preset mask index.

===========================================================================*/
uint8 diag_get_current_preset_mask_id()
{
  return diag_cur_preset_id;

} /* diag_get_current_preset_mask_id */

/*===========================================================================

FUNCTION DIAG_MASK_NOTIFY_TIMER_CB

DESCRIPTION
   This function is called when the timer for diag mask change notification
   has expired.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

===========================================================================*/
void diag_mask_notify_timer_cb(uint32 params)
{
  /*Reset the timer*/
  osal_reset_timer(&diag_mask_change_notify_timer);
  diag_set_internal_sigs(DIAG_MASK_CHANGE_NOTIFY_SIG);

}
/*===========================================================================
FUNCTION DIAG_MASK_CHANGE_NOTIFY

DESCRIPTION
  This function notifies the registered users about diag mask change.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

===========================================================================*/
void diag_mask_change_notify(void)
{
  if (DIAG_CHECK_MASK(diag_mask_notify,DIAG_EVENT_MASK_NOTIFY_BIT))
  {
           //Notify the registered clients about event mask change
      if (RCEVT_NULL == rcevt_signal_name(DIAG_EVENT_MASK_CHANGE_NOTIFY))
      {
        MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Event mask change notification was unsuccessful for proc id %d", DIAG_MY_PROC_ID);
      }
      else
      {
        MSG_1(MSG_SSID_DIAG, MSG_LEGACY_MED, "Event mask change notification was successful for proc id %d", DIAG_MY_PROC_ID);
        DIAG_CLR_MASK(diag_mask_notify,DIAG_EVENT_MASK_NOTIFY_BIT);
      }

  }
   if (DIAG_CHECK_MASK(diag_mask_notify,DIAG_LOG_MASK_NOTIFY_BIT))
  {
           //Notify the registered clients about log mask change
      if (RCEVT_NULL == rcevt_signal_name(DIAG_LOG_MASK_CHANGE_NOTIFY))
      {
        MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Log mask change notification was unsuccessful for proc id %d", DIAG_MY_PROC_ID);
      }
      else
      {
        MSG_1(MSG_SSID_DIAG, MSG_LEGACY_MED, "Log mask change notification was successful for proc id %d", DIAG_MY_PROC_ID);
        DIAG_CLR_MASK(diag_mask_notify,DIAG_LOG_MASK_NOTIFY_BIT);
      }

  }
   if (DIAG_CHECK_MASK(diag_mask_notify,DIAG_MSG_MASK_NOTIFY_BIT))
  {
         if (RCEVT_NULL == rcevt_signal_name(DIAG_MSG_MASK_CHANGE_NOTIFY))
     {
       MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Msg mask change notification was unsuccessful for proc id %d", DIAG_MY_PROC_ID);
     }
     else
     {
       MSG_1(MSG_SSID_DIAG, MSG_LEGACY_MED, "Msg mask change notification was successful for proc id %d", DIAG_MY_PROC_ID);
        DIAG_CLR_MASK(diag_mask_notify,DIAG_MSG_MASK_NOTIFY_BIT);
     }
  }
}
/*============================================================================
  diag_debug_log

DESCRIPTION
  This function keeps track of the KW issue error cases.

PARAMETERS
  id: Indicate the place of the error case.
  param_0:parameter about the error case that needs to be recorded

DEPENDENCIES
  None

RETURN VALUE
  None

==============================================================================*/
void diag_debug_log(diag_generic_log_enum id,uint32 param_0)
{
  osal_lock_mutex(&diag_debug_log_buff_cs);
  diag_debug_log_buff[diag_debug_log_buff_index].id = id;
  diag_debug_log_buff[diag_debug_log_buff_index].param_0 = param_0;
  diag_debug_log_buff_index = (diag_debug_log_buff_index + 1) % DIAG_DEBUG_LOG_SIZE;
  osal_unlock_mutex(&diag_debug_log_buff_cs);
}

/*===========================================================================

FUNCTION DIAG_TIME_INIT

DESCRIPTION
  This function initializes the default time API to be used with diag packets.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN
  Boolean status to indicate that the default time API to use has been set.
===========================================================================*/
boolean diag_time_init()
{
  if(!osal_atomic_compare_and_set((osal_atomic_word_t*)&diag_time_init_begin, FALSE, TRUE))
    return FALSE;

  /* Diag uses unprotected timestamp call on QDSP6 modem */
#if defined DIAG_IMAGE_QDSP6_PROC && defined DIAG_IMAGE_MODEM_PROC
   diag_time_get = time_tod_get_no_mutex;
#else
   diag_time_get = time_get;
#endif

  diag_time_initialized = TRUE;
  return TRUE;

}

/*===========================================================================
FUNCTION DIAG_SET_TIME_API

DESCRIPTION
  This function sets the time API to be used with the diag packets generated on the ADSP and
  also notify the client PDs to use the time API set.

PARAMETERS
  Time API to use

DEPENDENCIES
  None

RETURN

===========================================================================*/
void diag_set_time_api(uint8 time_api_to_use)
{
  /* Update the global variable which tracks the time API to be used*/
  diag_time_api_to_use = time_api_to_use;

  switch(diag_time_api_to_use)
  {
    case DIAG_TIME_GET:
    {
      /* Diag uses unprotected timestamp call on QDSP6 modem */
#if defined DIAG_IMAGE_QDSP6_PROC && defined DIAG_IMAGE_MODEM_PROC
      diag_time_get = time_tod_get_no_mutex;
#else
      diag_time_get = time_get;
#endif
      /* Notify the client PDs */
      diag_svc_change_notify(DIAG_TIME_API_CHANGE);
    }
    break;
    case DIAG_TIME_GET_UPTIME:
    {
      /* Notify the client PDs */
      diag_time_get = time_get_uptime;
      diag_svc_change_notify(DIAG_TIME_API_CHANGE);
    }
    break;
    case DIAG_TIME_GET_TBD:
    {
      /* Remove this error fatal after TIME SYNC Phase 2 implementation*/
      MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Invalid Time API to be set : %d", diag_time_api_to_use);
    }
    break;
    default:
    {
      break;
    }
  }
}


/*===========================================================================

FUNCTION DIAG_GET_TIME_API_REQUEST

DESCRIPTION
  This handler returns the TIME API currently being used to update the timestamp
  in diag packets on this proc.

PARAMETERS
  req_pkt     Incoming command packet
  pkt_len     Length of incoming packet

DEPENDENCIES
  None

RETURN
  Returns response packet

===========================================================================*/

void* diag_get_time_api_request(void* req_pkt, uint16 pkt_len)
{
  diag_get_time_api_req_type *req = (diag_get_time_api_req_type*)req_pkt;
  diag_get_time_api_rsp_type* rsp = NULL;
  diagpkt_subsys_cmd_code_type cmd_code;
  unsigned int rsp_len = sizeof(diag_get_time_api_rsp_type);

  if(pkt_len != sizeof(diag_get_time_api_req_type))
  {
    return (diagpkt_err_rsp(DIAG_BAD_LEN_F, req_pkt, pkt_len));
  }

  if(req->cmd_version != 1)
  {
    return (diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len));
  }

  cmd_code = diagpkt_subsys_get_cmd_code(req_pkt);
  rsp = (diag_get_time_api_rsp_type*)diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV, cmd_code,rsp_len);

  if(rsp)
  {
    rsp->cmd_version = req->cmd_version;
    rsp->time_api = diag_time_api_to_use;
  }

  return (void*)rsp;

}
