/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             Diagnostic Subsystem Buffer Allocation Manager

General Description
  This file contains routines that manage the diag output buffer.
  This allocation scheme is modeled as a circular buffer.

  Different types of data have different priorities.  These types and
  priorities are listed below in descending order of priority:

  1.  Event reports
  2.  Explicit diag responses
  3.  Streamed data (LOG, MSG, etc)

  Events are recorded at the head of the buffer, and will be accumulated
  until terminated (by event reporting code in event.c, etc)  A specified amount
  of space is reserved for event reporting.  If the buffer gets full (up to
  specified amount of space for events), only events will be allowed in the buffer.

  The diag task has the ability to block other tasks from accessing buffer
  space.  If diag is unable to get space, it will block other tasks (except events)
  and retry until it can get space.  Only event reports will be able to allocate space
  while diag is blocking.

Initialization and Sequencing Requirements
  diagbuf_init() must be called before using this service.

Copyright (c) 1999-2017, 2019 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                              Edit History

 $Header: //components/rel/core.qdsp6/2.1.c4/services/diag/DCM/common/src/diagbuf.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/31/17   nk      Qshrink 2 support for uimage diag messages
02/24/19   gn      Resolved send buffer out of bound access
05/25/17   ph      Separate macro to configure sensor PD buffer size.
08/14/17   sp      Fix start up bug in diag task when hardware threads reduced
09/26/16   sp      Initialize diagbuf_mpd_q before Diag registration with QDI. Also cleanup unused mutexes
05/05/17   ph      Configure buffering modes for user PD send buffers.
01/08/16   sa      Update UserPD commit threshold in NRT mode
02/21/17   ph      Tag DIAG ID header to all the diag packets sent out.
06/09/17   sp      Fix issue with log packet drop in diag send buffer
07/27/17   rs      setting log_ext_hdr_start FALSE in diagbuf_send_pkt_single_proc when we hit flow control
                   as we don't copy data from diag_buf to send buffer in this situation.
08/29/17   gn      Changes to handle remote socket close
06/08/17   gn      Added support for diag services to uImage
06/05/17   nk      Added debug support for uimage in root PD
04/24/17   sp      Added support for extended logs in userPD
04/09/17   sp      Added support for extended logs in rootPD
01/25/17   rs      Re-try sending control packets using bit mask and signal and by-passing the control heap usage.
10/18/16   ph      Corrected implemenatation of diag_add_block_header().
09/09/16   gn      Added check for flow control before flushing the buffer to the comm layer
08/11/16   sa      Cleared buf_in_use just after the inner loop is exited.
08/26/16   rs      Fixed bug in diagbuf_check_overrun_pattern() when pattern overrun occurs.
07/07/16   sa      Cleared buf_in_use when all the buffers are empty for PD restart qdi cleanup
05/12/16   ph      SMD references removal and code clean up.
05/10/16   nk      Separate send buffers for CMD and DATA
03/23/15   sa	   Fixed PD restart issue, moved PD check up.
02/01/15   is      Set drain signal to resume draining if any buffer is empty
12/16/15   as      Resolved Diag signal conflicts
11/11/15   as      Changes for PD restart
12/22/15   is      Fix flow control race condition
12/21/15   is      Check terminate field before sending data out
10/21/15   sr      Fixed the diagbuf corruption issue
09/08/15   ph      Consolidated Buffering mode fixes
09/01/15   xy      Update stream_id to static in diagbuf_drain
08/21/15   as      Resolved Diag signal conflicts
07/13/15   sa      Update the diagbuf_tail only if the packet is drained successfully.
04/08/15   ps      Diag over sockets and dsm removal
11/14/14   jtm/vk  Multi-PD Optimizations
08/20/14   sr      Fixed issue with F3s not coming out
07/13/15   sa      Update the diagbuf_tail only if the packet is drained successfully.
07/06/14   ph      Added support for Diag over STM
04/10/14   sr      Support for peripheral buffering mode
01/27/14   rh      Added registration to set immediate diagbuf drain during shutdown.
11/27/13   ph      Updated MSG 1.0 API references to MSG.2.0.
11/22/13   sr      Removed PACK from pointer references
11/19/13   sr      Fix compiler warnings
09/27/13   xy      Removed code under FEATURE_WINCE
09/13/13   ph      Updated drain logic for TX packets of size greater than 4k
                   to send the current dsm item and then start the chain.
08/26/13   sr      Added support to turn off HDLC encoding
05/11/13   is      Move drain timer from client conext to diag context
04/18/13   ph      Replace memcpy() and memmove() with memscpy() and memsmove()
04/03/13   is      Add check for diagbuf status in case we don't enter while loop.
03/13/13   rh      diagbuf_flush() now handles both TX and TX Timer sigs
02/01/13   sg      Fixed Klock work warnings
01/03/13   sg      Command Response Channel separation
12/13/12   rs      Added timeout mechanism to unblock diag in diagbuf_flush.
11/15/12   is      Support for preset masks
10/12/12   rh      In diagbuf_drain(), we no longer flush our packet when a
                   signal comes in if we do not set the Diag drain timer.
08/20/12   ra      Renamed ambiguous static symbols in
                   diagbuf_send_pkt_single_proc_ctrl()
07/05/12   sg      Changes to bring up diag Multi PD
05/03/12   rs      Moved delayed response from diag buffer to AMSS heap
04/27/11   is      Resolve compiler warnings
04/18/11   is      Ensure CTRL msg sizes are constant
03/01/11   is      Removed unused AEEstd.h
02/10/11   vs      Fixed case of incorrect drops if invalid state is read
                     in alloc_internal
12/20/10   mad     Changes for dual mask diag
01/18/11   vs      removed inline for diagbuf_clear
01/03/11   vs      calling digbuf_clear for case of DIAGBUF_HOLE_S
12/10/10   vs      Removed error fatal in internal_alloc. Check invalid
12/10/10   vs      Clear status bit in diagbuf_drain, revert changes to
                    diagbuf_shorten
11/23/10   vs      Fix corner case in encode function where a partial f3
                    message was being sent out incorrectly.
11/22/10   vs      Fix for corner case with lock-less implementation
11/05/10   vs      diagbuf returns 8 byte aligned addresses for logs on q6
10/28/10   vs      Added lock-less implementation for diagbuf_alloc_internal
10/25/10   vs      Added implementation for faster encoding
10/25/10   is      Diag feature flag cleanup
09/22/10   mad     Added diagbuf_ctrl_tx_sleep_parameters
09/14/10   is      Remove duplicate inclusion of diagcomm_v.h
09/13/10   is      Add flow control count and DSM chaining count to Diag Health
07/22/10   sg      Added changes to support dsm chaining for  rx response packets
07/15/10   sg      Check whether diag is connected while setting the drain timer
06/22/10   sg      Merging back DCM rtos and WM files
06/20/10   sg      check whether diag is initialized before allocating space from
                   diagbuf when clients call diag apis
05/07/10   is      Mainlined DIAGBUF_HEAP
04/29/10   is      Removed support for DIAG_DEBUG_FS_PKT_LOG
04/15/10   JV      Fix for klocwork errors
03/14/10   vs      Added sleep based on threshold for a case in
                    diagbuf_send_pkt_single_proc.
03/14/10   vs      Added check for full buffer case in single_proc function
                    This is sent out without chaining now.
03/06/10   JV      When bailing out of diagbuf_drain(), set the drain timer
                   if diagbuf in not empty.
03/06/10   JV      Revert setting the TX sig in diagbuf_alloc_internal.
03/05/10   JV      Set the TX sig to start draining if diagbuf_alloc fails
                   because diagbuf is full.
03/02/10   JV      Moved DIAG_TX_SLEEP_TIME_DEFAULT and
                   DIAG_TX_SLEEP_THRESHOLD_DEFAULT to diagbuf_v.h since these
                   are required even in diagdiag_common.c.
02/19/10   vs      Fixed compiler warning in diagbuf_init
02/02/10   JV      Do not poll for events at every packet boundary
01/28/10   vs      Flush responses also in diagbuf_flush
01/18/10   JV      Consolidate OSAL APIs
01/08/10   vs      Modified diagbuf_send_pkt_single_proc to handle packet
                   boundaries.
12/28/09   ps      Fixed Klocwork errors & compiler warnings.
12/16/09   JV      Including diagnv.h causes a compilation error on the audio Q6
12/10/09   sg      Added flow control mechanism for DSM_DIAG_SIO_TX_ITEM_POOL
                   at item level granularity
12/07/09   cahn    Added Delayed Response to DIAG Health.
11/30/09   ps      Fixed compiler warnings.
11/18/09   ps      Added qsr feature support.
11/04/09   cahn    DIAG Health
10/21/09   JV      Putting in a new feature so that we can now use the
                   diagbuf_send_pkt_multi_proc() function to send data out
                   if we require the response to be prioritized.
09/28/09   JV      Remove mutexes from diag task
09/28/09   JV      Include diag_cfg.h for the config related info
09/22/09   vs      Use OSAL for all rex calls on qdsp6.
09/09/09   JV      Changed signature of osal mutex calls.
07/13/09   vs      In diagbuf_commit - set the diag signal after a
                   certain number of bytes have been commited.
08/05/09   JV      Removed nested INTLOCKS (osal mutexes).
08/05/09   JV      Removed the CUST_H featurization around the inclusion of
                   customer.h.
08/05/09   JV      task.h is now included in osal.
07/31/09   JV      Merged Q6 diag code back to mainline
07/23/09   JV      Including diagdsm_v.h instead of diagdsm.h
07/10/09   as      Decoupled FEATURE_DIAG_NON_STREAMING & FEATURE_REX_CORE_DUMP
05/12/09   JV      Introduced the OS abstraction layer for rex.
05/07/09   mad     Clean-up: moved DIAG_TX_SLEEP_TIME_DEFAULT and
                   DIAG_TX_SLEEP_THRESHOLD_DEFAULT from diagbuf_v.h to
                   diagbuf.c, since these are required only in diagbuf.c
05/07/09   vk      changed includes from external to internal headers
04/22/09   mad     Modified diagbuf_tx_sleep_time_set() and
                   diagbuf_tx_sleep_threshold_set(), combined these into one
                   function, diagbuf_tx_sleep_parameters_set()
04/03/09   mad     now DIAGBUF_SIO_TX_ITEM_SIZ mirrors
                   DSMI_DIAG_SIO_TX_ITEM_SIZ
03/20/09   mad     change on behalf of vsharma: Change DIAGBUF_SIO_TX_ITEM_SIZ
                   to match increased DSM item size
03/20/09   vg      Update diag_alloc_internel to be more thread safe.
03/04/09   vg      Merge Q6 diag buffer code to be thread safe access
01/08/09   as      Update diag buffer code to be thread safe access.
10/03/08   vg      Update code to use PACK() vs. PACKED
02/05/08   pc      Added support for dynamic threshold and sleep values
12/16/07   pc      Added support for diag_dtr_enable and diag_dtr_changed_cb.
02/07/02   as      Made diagbuf_tail volatile. Moved sanity checks within
                   INTLOCKS. Check for min DSM items before filling them.
12/01/07   as      Moved diagbuf_write_overrun_check_pattern inside INTLOCK().
                   Added sanity checks in alloc and diagbuf_drain().
12/21/06   as      Fixed boundary cases in multiple Diag packets.
12/15/06   as      Fixed compiler warnings.
12/05/06   as      Support to include multiple Diag packets into one DSM item.
08/21/06   bfc     Removed a MSG_HIGH that was exacerbating an dsm_item
                   deficiency
10/31/05   as      Fixed lint errors.
10/19/05   as      Fixed CRC update in hdlc encode.
02/15/05   as      Removed PLATFORM_LTK feature to allow data send to be split
                   into different packets.
01/12/05   as      Fixed hdlc encode bug in CRC
01/07/05   as      Modified the location of check for empty diag buffer. This
                   fixes the invalid check for empty diag heap.
12/06/04   as      Included dog.h to prevent compiler warnings
09/21/04   as      Changed hdlc encode to fill dsm items with 128 bytes. But,
                   if the last byte is an escape character (0x7D or 0x7E),then
                   only 127 bytes are filled in dsm item.
03/30/03   as      Added ASSERTS in functions that manipulate the diag buffer
                   and intlocks in diagbuf wrap and move macros.
10/28/03   as      Changes to fix warnings on ads12 compiler
12/02/02   lad     Removed blocking alloc (to diagpkt.c), enhacned packet
                   bounadary processing to diagbuf_drain() to give priority
                   to diagpkt rsp packets, and added periodic sleep logic with
                   inert defaults to avoid starvation of lower priority tasks.
07/30/02   as      Updated diagbuf_get_log and diag_send_tx_packet to support
                   FEATURE_DIAG_NON_STREAMING
06/25/02   as      Changed diagbuf_internal_alloc, diagbuf_shorten, diagbuf_commit,
                   and diagbuf_drain to obtain byte allignment.
10/24/01   jal     Changed diagbuf_flush() to wait for SIO to finish
                   flushing UART before returning--guarantees last
                   response packet gets sent.
06/27/01   lad     Made private data and functions static.
                   Removed "code" from diagbuf_blocking_alloc() - this is
                   done in diagpkt_alloc().
                   Updated usage of log_config().
06/22/01   lad     Fixed diagbuf_drain to check for committed packet after wrap.
                   Changed diagbuf_buf to be aligned on 32-bit boundary.
04/17/01   lad     Modified diagbuf_alloc(), diagbuf_shorten(), and
                   diagbuf_commit() to detect buffer overrun by users.
                   Changed call to diag_do_escaping() to remove const args.
02/23/01   lad     Made changes for newly architected diagnostics service.
                   Introduced the communications layer and the use of diagtune.h
                   to configure this code.
                   Many cosmetic changes were made as well.
11/28/00   lad     Changed diagbuf_drain() to return correct status when no
                   open communications stream exists.
11/10/00   jal     Killed third call to event_drain() in diagbuf_drain().
                   It was causing an endless loop when there are no events
                   and only un-committed data in the ring buffer.  It isn't
                   needed anyways.
11/07/00   jal     Bugfixes to diagbuf_drain().  Moved handling
                   DIAG_EVENT_TIMER_SIG out of diagpkt_blocking_alloc() into
                   diag.c/diag_wait() per Larry.  Removed diag_set/clr_sigs
                   debug hack.
10/17/00   jal     Moved diagbuf_OKTX, diagbuf_start/stop_flow() to diag.c
                   Moved event_stale_timer over to event.c.  Mods to
                   support new event accumulation mechanism.
09/25/00   jal     Rewrote diagbuf_drain() to a) use the new DSM item
                   interface, and b) to use DSM items more efficiently.
                   Instead of using one item for each response packet
                   (wasteful!) we fill each item until we have nothing
                   more to send.
09/12/00   lad     Fixed load balancing check in diagbuf_alloc_internal.
08/28/00   lad     Optimized the routines in this module.
08/25/00   rc      Changed dsm_new_buffer() calls to take pool id as input
                   parameter.
07/06/00   jal     Changed KICK_WATCHDOG to DIAG_KICK_WATCHDOG to avoid
                   conflict with other (FS) VUs
06/23/00   lad     Removed obsolete diag_sleep reference.
06/19/00   lad     Added MSM5000_IRAM_FWD aroung diagbuf_alloc_internal()
06/09/00   lad     Added flags for MSM5000_IRAM_FWD
05/15/00   lad     Updated load balancing.
05/10/00   lad     Added load balancing.  Changed API to give diag_rsp_type*
                   instead of void*
03/07/00   lad     Fixed flow control for FEATURE_DIAG_NON_STREAMING.
02/28/00   lad     Created file.

===========================================================================*/

#include "customer.h"

#include "comdef.h"
#include "diag_v.h"
#include "diagbuf_v.h"
#include "diagbuf_mpd.h"
#include "Diag_LSM.h"
#include "diagcomm_v.h"
#include "diagdiag_v.h"
#include "diagi_v.h"
#include "diagpkt.h"
#include "diagtarget.h"
#include "diagtune.h"
#include "err.h"
#include "event.h"
#include "log.h"
#include "msg.h"
#include "eventi.h" /* For event_q_get fn*/
#include "msgi.h" /* For msg_set_all_rt_masks_adv() */
#include "stringl.h"
#include "feature.h"
#include "ULogFront.h"
#include "diag_mode_v.h"
#include "diag_diagIDi.h"

#include <stdio.h>
#include "crc.h"
#include "osal.h"
#include "diag_cfg.h"
#include "ULog.h"

#ifdef DIAG_UIMAGE_MODE
#include "micro_diagbuffer_drain.h" /* For micro_diagbuffer_drain() */
#include "uSleep_core_npa.h"
#include "micro_diagbuf_mpd.h"
#include "micro_diagbuf_mpd_type.h"
#endif

#include "diag_mode_v.h"
//#include "time_svc.h"

#include "diagcomm_cmd.h" /* For diagcomm_cmd_status */

#if defined (DIAG_QDSP6_APPS_PROC)
#include "diagstub.h"
#else
#include "assert.h"
#include "diagnv.h"

#include "dog.h"

#endif /* defined (DIAG_QDSP6_APPS_PROC) */



#include "diagcomm_ctrl.h" /* For DIAG_CTRL_MSG_DIAGDATA etc */
#include "npa.h"  /* For npa_issue_required_request() */

#include "diag_param.h"

/* queue for event PD's*/
extern q_type diag_main_q;

static osal_mutex_arg_t diagbuf_buf_cs;
extern ULogHandle diag_err_log_handle;

/*lint -esym(751, diagbuf_header_status_enum_type) */

/* Supress local typedef 'diagbuf_header_status_enum_type' not referenced */

/* Diag's output buffer.  The ADS 1.0 compiler will not align a byte array on
 * 32-bit boundary.  The ARM 2.5 compiler had a setting for this, but not ADS
 * 1.0.  Therefore, we must declare it to be a array of 32-bit values and
 * assign a byte* to point to it. */

/* MACROS declarations */

#ifndef DIAGBUF_PKT_HANDLER_TBL_CNT
/* DIAG_QSR_EXT_MSG_TERSE_F, DIAG_EXT_MSG_F and DIAG_LOG_F use this feature */
#define DIAGBUF_PKT_HANDLER_TBL_CNT 3
#endif

#define DIAG_MODEM_HDLC_TURNED_OFF 1
#define DIAG_MODEM_HDLC_TURNED_ONN 0

/* Input buffer used for encoded transmit data. For packets
   that exceed DSM item size, this buffer is used as output buffer */
#define DIAG_MAX_TX_PKG_PKT_SIZ (12288)


/*===========================================================================
MACRO DIAGBUF_IS_PD_KILLED

DESCRIPTION
  This macro checks if a PD is killed.

PARAMETERS
 diagbuf_mpd_type *buf - the UserPD diagbuf pointer.

RETURN VALUE
  TRUE  - If the PD is being killed
  FALSE - Otherwise
===========================================================================*/
#define DIAGBUF_IS_PD_KILLED(current)({\
  boolean rv = FALSE; \
  \
  if(current != NULL)\
  {\
    if (current->is_pd_killed == TRUE)\
    {\
     rv = TRUE; \
    }\
  }\
  (rv);\
})


/*===========================================================================
MACRO DIAGBUF_ARTIFICALY_MAKE_PD_EMPTY

DESCRIPTION
  This macro make a PD diagbuf looks empty by setting head and
  tail to same value.

PARAMETERS
 diagbuf_mpd_type *buf - the UserPD diagbuf pointer.

RETURN VALUE
  TRUE  - If the PD has  been successfully marked empty
  FALSE - If PD is a core PD, it cannot be marked empty.
===========================================================================*/
#define DIAGBUF_ARTIFICALY_MAKE_PD_EMPTY(current)({\
  boolean rv = TRUE; \
  \
   if (current == NULL)\
   {\
      rv = FALSE;\
   }\
   else\
   {\
      current->diagbuf_mpd_tail = current->diagbuf_mpd_head;\
   }\
   (rv);\
})

/*===========================================================================
MACRO DIAGBUF_REMOVE_PD_FROM_PD_LINK_LIST

DESCRIPTION
  This macro removes a PD from PD LIST that drain goes thru.

PARAMETERS
 diagbuf_mpd_type *buf - the UserPD diagbuf pointer.

RETURN VALUE
  TRUE  - If the PD is removed from the list
  FALSE - If PD is a core PD, it cannot be deleted. Or if
          q_delete_ext returned FALSE i.e. it could not find the
          item in queue.
===========================================================================*/
#define DIAGBUF_REMOVE_PD_FROM_PD_LINK_LIST(current)({\
  boolean rv = TRUE; \
  \
   if (current == NULL)\
   {\
      rv = FALSE;\
   }\
   else\
   {\
      rv = q_delete_ext(&diagbuf_mpd_q, &current->link);\
   }\
   (rv);\
})

/*===========================================================================
MACRO DIAGBUF_ARE_BUFFERS_EMPTY

DESCRIPTION
  This macro checks all diagbufs to see if any of them is
  empty.

RETURN VALUE
  TRUE  - If the GuestOS diagbuf or any UserPD diagbuf is empty
  FALSE - Otherwise

===========================================================================*/
#define DIAGBUF_ARE_BUFFERS_EMPTY()({\
   boolean rv = TRUE;\
   diagbuf_mpd_type *q_cur_ptr = NULL;\
\
   if (!DIAGBUF_EMPTY(diagbuf_head, diagbuf_tail))\
   {\
      rv = FALSE;\
   }\
   else\
   {\
      q_cur_ptr = (diagbuf_mpd_type *)q_check(&diagbuf_mpd_q);\
\
      while (q_cur_ptr != NULL)\
      {\
         if (!DIAGBUF_EMPTY(q_cur_ptr->diagbuf_mpd_head, q_cur_ptr->diagbuf_mpd_tail))\
         {\
            rv = FALSE;\
            break;\
         }\
         q_cur_ptr = (diagbuf_mpd_type *)q_next(&diagbuf_mpd_q, &q_cur_ptr->link);\
      }\
   }\
   (rv);\
})

/*===========================================================================
MACRO DIAGBUF_FIND_NEXT_BUFFER

DESCRIPTION
  This macro iterate through the diagbuf and the
  diagbuf_mpd_type queue and return the next buffer to check

PARAMETERS
 diagbuf_mpd_type *buf - the UserPD diagbuf pointer.

RETURN VALUE
  Pointer to next diagbuf (guest OS/ user space)

===========================================================================*/
#define DIAGBUF_FIND_NEXT_BUFFER(current) ({\
\
   diagbuf_mpd_type *rv = NULL;\
\
   if (current == NULL)\
   {\
      rv = q_check(&diagbuf_mpd_q);\
   }\
   else\
   {\
      rv = q_next(&diagbuf_mpd_q, &current->link);\
   }\
   (rv);\
})

/*===========================================================================
MACRO DIAGBUF_CHECK_CORRUPTION_IN_BUFFERS

DESCRIPTION
  This macro checks all the diag buffs to make sure there is
  no corruption

RETURN VALUE
  TRUE  - If corruption if detected
  FALSE - Otherwise

===========================================================================*/
#define DIAGBUF_CHECK_CORRUPTION_IN_BUFFERS() ({\
\
   boolean rv = FALSE;\
   diagbuf_mpd_type *q_cur_ptr = NULL;\
\
   if (DIAGBUF_USED(diagbuf_head, diagbuf_tail, DIAGBUF_SIZE) > DIAGBUF_SIZE)\
   {\
      rv = TRUE;\
   }\
   else\
   {\
      q_cur_ptr = (diagbuf_mpd_type *)q_check(&diagbuf_mpd_q);\
\
      while (q_cur_ptr != NULL)\
      {\
         if (DIAGBUF_USED(q_cur_ptr->diagbuf_mpd_head, q_cur_ptr->diagbuf_mpd_tail, q_cur_ptr->diagbuf_size) > q_cur_ptr->diagbuf_size)\
        {\
            rv = TRUE;\
            break;\
         }\
\
         q_cur_ptr = (diagbuf_mpd_type *)q_next(&diagbuf_mpd_q, &q_cur_ptr->link);\
      }\
   }\
   (rv);\
})

#if defined DEBUG_DIAG_TEST
#define DIAGBUF_MAX_DEBUG_COUNT 8192
typedef struct
{
  uint32 num_diag_pkts;
  uint32 num_bytes_pkged;
  uint32 num_bytes_sent;
} diag_pkg_type;

uint32 diag_pkg_count = 0;

diag_pkg_type diag_pkg[DIAGBUF_MAX_DEBUG_COUNT];

#define diag_pkg_append(x, y, z) \
   diag_pkg[diag_pkg_count].num_diag_pkts = x;\
   diag_pkg[diag_pkg_count].num_bytes_pkged = y;\
   diag_pkg[diag_pkg_count].num_bytes_sent = z;\
   diag_pkg_count++; \
   if (diag_pkg_count >= DIAGBUF_MAX_DEBUG_COUNT ) \
      diag_pkg_count = 0;
#endif
#define DIAGBUF_HEADER_PATTERN 0xDEAD
uint32 diag_send_buf_pad=DIAG_SEND_BUF_PAD_LEN;
#define DIAG_HDLC_CRC_PAD (3) /* 2 bytes CRC. 1 byte 7E terminator. */
static uint8 diagbuf_hdlc_pad_len = DIAG_HDLC_PAD_LEN;

/* Static declarations */

static uint32 diagbuf_buf_aligned[(DIAGBUF_SIZE + 3)/4];
static uint8 *diagbuf_buf = (uint8 *)diagbuf_buf_aligned;

/*
   Head and tail of circular buffer.
*/
static volatile int diagbuf_head = 0;
static volatile int diagbuf_tail = 0;

/*
  Maps each version of the diagbuf packet to the length of the first packet in the chain
  length of the first packet in the chain = payload of first packet + sizeof(uint16)
  sizeof(uint16) - accounts for the DEAD pattern for the first packet in the chain
*/
uint8 diagbuf_ver_len[DIAGBUF_MAX_VERSION_NUM] = {
  DIAGBUF_NEXT_ALIGNED_BYTE(sizeof(diag_cmd_ext_v1_type)+sizeof(uint16))/sizeof(diagbuf_header_type), // Version 1
  DIAGBUF_NEXT_ALIGNED_BYTE(sizeof(diag_cmd_ext_v2_type)+sizeof(uint16))/sizeof(diagbuf_header_type)  // Version 2
};

static struct
{
  uint8 cmd_code;
  diagbuf_outbound_pkt_handler_type fp;
} diagbuf_outbound_pkt_handler_tbl[DIAGBUF_PKT_HANDLER_TBL_CNT];

/*
  Queue used to keep pointers to each diagbuf associated with
  all the user process spaces.
*/
extern q_type diagbuf_mpd_q;
extern osal_mutex_arg_t diagmpd_buf_list_state_mutex;

#ifdef DIAG_UIMAGE_MODE
extern micro_diagbuf_mpd_type *micro_diagmpd_buf;
extern char uimage_buffer[UIMAGE_DIAGBUFFER_SIZE];
#endif

static diagbuf_outbound_pkt_handler_type diagbuf_get_custom_pkt_handler( uint8 cmd_code );
static void diagbuf_write_overrun_check_pattern( void *ptr );
static inline void diagbuf_clear( int start, int offset, uint8 *buf );

extern uint8 diag_tag_hdr_size;
extern boolean diag_tagging_support;

/*===========================================================================
FUNCTION EVENT_PD_DELETE

DESCRIPTION
  This function deletes a PD from diag_main_q PD link list. It calls
  q_delete to remove the PD from the diag_main_q

PARAMETERS
 diagbuf_mpd_type *mpd_buf_to_remove - the UserPD diagbuf pointer.

RETURN VALUE
  TRUE  - PD has  been successfully deleted the PD node
  FALSE - PD was not deleted.
===========================================================================*/
boolean event_pd_delete(diagbuf_mpd_type *mpd_buf_to_remove)
{
  boolean return_val = FALSE;
  event_store_type *cleanup_item =NULL;
  event_struct * event_clean_ptr= NULL;
  diag_mpd_ptr_struct * diagmpd_clean_ptr = NULL;
  int index=0;

  if(mpd_buf_to_remove == NULL)
  {
    return (return_val);
  }
  index = mpd_buf_to_remove->pid;

  diagmpd_clean_ptr= EVENT_Q_SEARCH(index);

  if(diagmpd_clean_ptr == NULL)
  {
    return (return_val);
  }
  event_clean_ptr= diagmpd_clean_ptr->event_mpd_struct_ptr;

  if(event_clean_ptr == NULL)
  {
    return (return_val);
  }
  /*check if this PD is going down (PD restart case: when PD
     nodeis not the current that is used for draining)  */
  if ( DIAGBUF_IS_PD_KILLED(mpd_buf_to_remove) )
  {
    // Check if event_q for this mpd is empty. If No, then free each event in the queue.
    while((cleanup_item= q_check(&event_clean_ptr->event_q.q))!=NULL)
    {
      cleanup_item=event_q_get(event_clean_ptr);
      diag_event_free(cleanup_item,event_clean_ptr);
    }

	//Destroy the event queue
	q_destroy(&event_clean_ptr->event_q.q);
	
    // De-initilize the  event heap
    mem_deinit_heap(&(event_clean_ptr->diag_event_heap));

    //Free the event heap buffer
    free(event_clean_ptr->diag_event_heap_buf);

    //Free the event_struct for the pd
    free(event_clean_ptr);

    //Delete PD frm diag_main_q
    q_delete(&diag_main_q,&diagmpd_clean_ptr->link);

    //Free the main struct which contains pointers for diagbuf and event struct for the pd.
    free(diagmpd_clean_ptr);

    //return true once all the cleanup is done
    return_val= TRUE;
  }
  return (return_val);
}

/*===========================================================================
FUNCTION DIAGBUF_PD_DELETE

DESCRIPTION
  This function deletes a PD from PD linked list. It calls
  DIAGBUF_REMOVE_PD_FROM_PD_LINK_LIST()

PARAMETERS
 diagbuf_mpd_type *mpd_buf_to_remove - the UserPD diagbuf
                                       pointer.

RETURN VALUE
  TRUE  - If the PD has  been successfully deleted the PD node
  FALSE - If PD is a core PD, it cannot be deleted. Or if
          q_delete_ext returned FALSE i.e. it could not find the
          item in queue Or the PD is not being deleted.
===========================================================================*/
boolean diagbuf_pd_delete( diagbuf_mpd_type *mpd_buf_to_remove )
{

  boolean return_val = FALSE;
  /*
     check if this PD is going down (PD restart case: when PD
     nodeis not the current that is used for draining)
  */
  if ( DIAGBUF_IS_PD_KILLED(mpd_buf_to_remove) )
  {
    return_val = DIAGBUF_ARTIFICALY_MAKE_PD_EMPTY(mpd_buf_to_remove); /* Making PD empty allows Drain to never
                                                  attempty drain this buffer */
    return_val = DIAGBUF_REMOVE_PD_FROM_PD_LINK_LIST(mpd_buf_to_remove);
  }
  return (return_val);
}

/*
   Quick and dirty way to have all future messages get flushed out immediately.
   Assumes that more messages will be generated or a timer ticks over.
*/
static void diagbuf_immediate_delivery( void )
{
  diagbuf_ctrl_commit_threshold(0);
  diag_tx_notify();
}

/* External declarations */

extern boolean diagpkt_rsp_send( void );
extern boolean diag_tx_pending;
extern boolean diag_drain_timer_pending;

extern diagcomm_io_conn_type diagcomm_io_ctrl_conn[NUM_PORT_TYPES][NUM_REMOTE_PORTS];
extern diagcomm_io_conn_type diagcomm_io_cmd_conn[NUM_PORT_TYPES][NUM_REMOTE_PORTS];
extern diagcomm_io_conn_type diagcomm_io_conn[NUM_PORT_TYPES][NUM_REMOTE_PORTS];

uint8 diagbuf_send_data[NUM_REMOTE_PORTS][DIAG_SEND_BUF_SIZE_DATA]; /*Buffer used to send out F3s/logs/events */

uint8 diagbuf_send_data_spd[DIAG_SEND_BUF_SIZE_DATA_SENSOR];
uint8 diagbuf_send_data_apd[DIAG_SEND_BUF_SIZE_DATA_AUDIO];

uint8 diagbuf_send_cmd[NUM_REMOTE_PORTS][DIAG_SEND_BUF_SIZE_CMD]; /*Buffer used to send out cmd rsp */
static uint16 diag_send_buf_data_index[NUM_REMOTE_PORTS] = {0, 0}; /*Current data buffer index*/
static uint16 diag_send_buf_cmd_index[NUM_REMOTE_PORTS] = {0, 0}; /*Current cmd buffer index*/

extern uint32 diag_send_buf_threshold; /*Threshold to buffer up data in send buffer*/
extern uint32 diag_max_send_buf_len; /*Maximum send size*/

#ifdef FEATURE_DIAG_SWITCH
extern int diag_on_single_proc;
#endif

/* Diag Percentage Drop Count Variables */
extern uint8 diag_per_count_dropped; /* Diag Percentage Drop Count  from diag.c */


/* Global declarations */

boolean bCountReset = FALSE; /* To track if the log/drop counts are reset */

typedef struct
{
  void *dest;
  void *dest_last;

  /* Below: internal use only */
  uint16 crc;
} diag_hdlc_dest_type;

/* Variables to store drop count for DIAG Health */
static uint32 drop_count_log = 0;
static uint32 drop_count_f3 = 0;

/* Variables to store allocation requests for DIAG Health */
uint32 alloc_count_log = 0;
uint32 alloc_count_f3 = 0;
/* Forward prototype declarations for local routines. */
void diag_add_block_header(diag_send_buf_type* send_buf, uint16 length, uint8 diag_id);
void diag_hdlc_encode( diag_send_desc_type *src_desc, diag_hdlc_dest_type *enc );
void diag_non_hdlc_encode( diag_send_desc_type *src_desc, diag_hdlc_dest_type *enc );



uint32 diag_buffered_bytes(uint8 stream_index, uint8 diag_id);
extern boolean log_ext_hdr_start; /* From diaglog.c*/

/* Allocate Micro diagbuffer for Root PD and add it to the Queue */
#ifdef DIAG_UIMAGE_MODE
extern micro_diagbuf_mpd_type root_micro_diagbuf;

void micro_diagbuf_mpd_add_root_pd(void)
{
  int return_val = 0;

  root_micro_diagbuf.buffer_type                   = DIAG_DIAGBUFFER;
  root_micro_diagbuf.buffer_location               = DIAG_ROOTPD;
  root_micro_diagbuf.is_in_use                     = FALSE;
  root_micro_diagbuf.is_pd_killed                  = FALSE;
  root_micro_diagbuf.micro_diagbuf_mpd_buf_cs.name = "MUTEX_MICRO_DIAGBUF_MPD_CS";

  micro_diagmpd_buf = (micro_diagbuf_mpd_type*)&root_micro_diagbuf;
  return_val = osal_init_mutex(&root_micro_diagbuf.micro_diagbuf_mpd_buf_cs);
  ASSERT(OSAL_SUCCESS == return_val);

  micro_diagbuf_mpd_add_pd(&root_micro_diagbuf);
#if defined (DIAG_UIMAGE_MODE)
  micro_diagbuffer_create( (micro_ULogHandle *)(&root_micro_diagbuf.handle), "diag_uimag_buffer", uimage_buffer, NULL, UIMAGE_DIAGBUFFER_SIZE_FINAL, MICRO_DIAGBUFFER_LOCKABLE );
#endif

}
#endif

/*===========================================================================

FUNCTION DIAGBUF_INIT

DESCRIPTION
   This function initializes the diag allocation buffer at startup.
===========================================================================*/
void diagbuf_init( void )
{
  diagbuf_header_type *pkt_hdr; /* pointer to the first header */


  int i, return_val = 0;

  diagbuf_head = 0;
  diagbuf_tail = 0;


  /* Setting the first status field to DIAGBUF_UNCOMMIT_S is all the
  ** initizalization needed
  */
  pkt_hdr = ((diagbuf_header_type *)&diagbuf_buf[0]);
  pkt_hdr->status = DIAGBUF_UNUSED_S;

  for ( i = 0; i < DIAGBUF_PKT_HANDLER_TBL_CNT; i++ )
  {
    diagbuf_outbound_pkt_handler_tbl[i].cmd_code = 0xFF;
    diagbuf_outbound_pkt_handler_tbl[i].fp = NULL;
  }


  /* Initialize diagbuf mutex */
  diagbuf_buf_cs.name = "MUTEX_DIAGBUF_BUF_CS";
  return_val = osal_init_mutex(&diagbuf_buf_cs);
  ASSERT(OSAL_SUCCESS == return_val);

  /* Set up callback for rapid delivery during shutdown
     Group 7 is the first shutdown group */
  rcinit_register_termfn_group(RCINIT_GROUP_7, diagbuf_immediate_delivery);

} /* diagbuf_init */

/*===========================================================================
FUNCTION DIAGBUF_RESET_DROP_COUNT_LOG

DESCRIPTION
    This function resets the internal variables drop_count_log and
    alloc_count_log.
===========================================================================*/
void diagbuf_reset_drop_count_log( void )
{
  drop_count_log = 0;
  alloc_count_log = 0;
  bCountReset = TRUE;

}
/*===========================================================================
FUNCTION DIAGBUF_GET_DROP_COUNT_LOG

DESCRIPTION
    This function returns the internal variable drop_count_log.
===========================================================================*/
uint32 diagbuf_get_drop_count_log( void )
{
  return (drop_count_log);
}

/*===========================================================================
FUNCTION DIAGBUF_GET_ALLOC_COUNT_LOG

DESCRIPTION
    This function returns the internal variable alloc_count_log.
===========================================================================*/
uint32 diagbuf_get_alloc_count_log( void )
{
  return (alloc_count_log);
}

/*===========================================================================
FUNCTION DIAGBUF_RESET_DROP_COUNT_F3

DESCRIPTION
    This function resets the internal variables drop_count_f3 and
    alloc_count_f3.
===========================================================================*/
void diagbuf_reset_drop_count_f3( void )
{
  drop_count_f3 = 0;
  alloc_count_f3 = 0;
  bCountReset = TRUE;
}
/*===========================================================================
FUNCTION DIAGBUF_GET_DROP_COUNT_F3

DESCRIPTION
    This function returns the internal variable drop_count_f3.
===========================================================================*/
uint32 diagbuf_get_drop_count_f3( void )
{
  return (drop_count_f3);
}

/*===========================================================================
FUNCTION DIAGBUF_GET_ALLOC_COUNT_F3

DESCRIPTION
    This function returns the internal variable alloc_count_f3.
===========================================================================*/
uint32 diagbuf_get_alloc_count_f3( void )
{
  return (alloc_count_f3);
}

/*===========================================================================
FUNCTION DIAGBUF_GET_DROP_PERCENTAGE
DESCRIPTION
    This function returns the percentage total percentage drop
    from the diag buffer (f3 and log percentage drops)
===========================================================================*/
uint8 diagbuf_get_drop_percentage(void)
{
  return diag_per_count_dropped;
}

/*===========================================================================
FUNCTION DIAGBUF_ALLOC_CHAIN

DESCRIPTION
    Allocates chained(consecutive) packets in Diag buffer.
    This function should be called only for extended log pkts

PARAMETERS
   length                    Length of requested buffer
   nice                      Niceness padding value
   diagbuf_alloc_request     Required for DIAG Health
   chain                     0 - not chained
                             1 - first pkt in the chain
                             2 - second pkt in the chain
   version                   0 - non extended pkts
                             1 or 2 - extended pkts
   id                        Passed in by diag client while
                             calling log APIs and F3 macros

DEPENDENCIES
  This function must only be called from within this file by externalized
  alloc functions.

  diagbuf_commit() must be called to commit a buffer to the diag pool.  This
  must happen reasonably soon after this call completes.  If the buffer is
  needed to accumulate data over time, then you should use your own buffer
  use this just long enough to copy the data.

  diagbuf_commit() should be called seperately for the two chained pkts in
  diagbuf.

RETURN VALUE
  Pointer to the allocated space in diagbuf
  NULL if there is no space available in diagbuf

===========================================================================*/
void * diagbuf_alloc_chain(
   uint8 code,
   uint32 length,
   int nice,
   uint8 diagbuf_alloc_request,
   uint8 chain,
   uint8 version,
   void* parameter,
   byte stream_id )
{
  uint32 buf_used = 0;
  uint32 buf_used_first_pkt = 0;
  uint32 buf_used_second_pkt = 0;
  uint8 pad_len1 = 0;
  uint8 pad_len2 = 0;
  diagbuf_header_type *pkt_hdr;
  uint32 ver_len = 0;
  byte * ptr_start = NULL;
  void * ptr1 = NULL;
  diag_cmd_ext_v1_type *ptr_payload = NULL;
  diag_cmd_ext_v2_type *ptr_payload2 = NULL;
  char temp_buf[ULOG_MAX_NAME_SIZE];
  uint32 name_len=0;

  /*Sanity checks*/
  if ( (version > DIAGBUF_MAX_VERSION_NUM) ||  /*currently support only version 0,1 and 2 */
       (chain > DIAGBUF_LAST_PKT_CHAIN) ||     /* allow only two packets to be chained in diagbuf*/
       (code != DIAG_LOG_F) ) /*allow only log extended pkts to be chained in diagbuf*/
  {
    return (NULL);
  }

  if( (version > DIAGBUF_VERSION_0 ) && (parameter == NULL) )
  {
      return (NULL);
  }

  /* Currently, we support only version 0,1 and 2 pkts */
  switch(version)
  {
    case DIAGBUF_VERSION_0:
    {
        return (diagbuf_alloc(code, length, nice, diagbuf_alloc_request, chain, version));
    }
    case DIAGBUF_VERSION_1:
    case DIAGBUF_VERSION_2:
    {

        if(version == DIAGBUF_VERSION_1)
        {
            /*version 1 pkt is of the following format:
            |cmd_code-DIAG_LOG_F (1 byte)|version (1 byte)|proc_id (2 bytes)|ID (4 bytes)|*/
            ver_len = sizeof(diag_cmd_ext_v1_type);
        }
        else
        {
            ver_len = sizeof(diag_cmd_ext_v2_type);

            if(ULogCore_NameGet( parameter, &temp_buf[0]) == DAL_SUCCESS)
            {
                /*Match found, get length*/
                name_len= strlen(temp_buf);

                if(name_len > ULOG_MAX_NAME_SIZE )
                {
                    ERR_FATAL("Invalid ULog ", 0, 0, 0);
                }
            }
            else
            {
               MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "ULogCore_NameGet returned fail");
               return NULL;
            }
        }
        /*Calculate the length of the first packet*/
        buf_used_first_pkt = ver_len +   /* payload of the first packet*/
            sizeof(uint16);   /* CRC - to account for DEAD pattern for the first packet in the chain*/

        /*Make the first packet byte aligned*/
        buf_used_first_pkt = DIAGBUF_NEXT_ALIGNED_BYTE(buf_used_first_pkt);

        /*Calculate the length of the second pkt;
        Account for the diag buffer header for the second pkt here*/
        buf_used_second_pkt = length +
            sizeof(diagbuf_header_type);  /* Diagbuf header for the second packet */

        /*Total length required for both the chained pkts;
            Call diagbuf_alloc() only once since we cant gaurantee if
            the chained pkts are consecutive if we call diagbuf_alloc() twice
        */
        buf_used = buf_used_first_pkt + buf_used_second_pkt;

        ptr_start = (byte *)diagbuf_alloc(code, buf_used, nice, diagbuf_alloc_request, chain, version);

        if ( ptr_start == NULL )
        {
            return (NULL);
        }

        /*Point to the first diagbuf header*/
        pkt_hdr = (diagbuf_header_type *)(ptr_start - sizeof(diagbuf_header_type));

        /*Set the length in the first diagbuf header pkt*/
        pkt_hdr->length = ver_len;
        pkt_hdr->stream_id = stream_id;
        pad_len2 = pkt_hdr->pad_len;
        pad_len1 = ((uint32)buf_used_first_pkt - ver_len - sizeof(uint16));
        ASSERT(pad_len2 < 4);
        ASSERT(pad_len1 < 4);
        pkt_hdr->pad_len = pad_len1;

        if(version == DIAGBUF_VERSION_1)
        {
            /*Assign the version and ID field*/
            ptr1 = (void *)ptr_start;
            ptr_payload = (diag_cmd_ext_v1_type *)ptr1;
            ptr_payload->version = DIAGBUF_VERSION_1;
            ptr_payload->proc_id = DIAG_MY_PROC_ID;
            ptr_payload->id = *((uint32 *)parameter);
        }
        else
        {
            /*Assign the version and other fields*/
            ptr1 = (void *)ptr_start;
            ptr_payload2 = (diag_cmd_ext_v2_type *)ptr1;
            ptr_payload2->version = DIAGBUF_VERSION_2;
            ptr_payload2->proc_id = DIAG_MY_PROC_ID;
            ptr_payload2->name_len = name_len;
        ptr_payload2->time_offset_type = DIAG_ULOG_NO_CUSTOM_OFFSET;

            strlcpy(ptr_payload2->ulog_name, temp_buf, name_len + 1 );
        }

        diagbuf_write_overrun_check_pattern(ptr_start - 4);

        /*Fill out the second header*/
        ptr1 = (void *)(ptr_start + buf_used_first_pkt);
        pkt_hdr = (diagbuf_header_type *)ptr1;
        // pkt_hdr->stream_id = stream_id; /* Will be filled out by the calling function, as per standard log allocations */
        pkt_hdr->status = DIAGBUF_UNCOMMIT_S;
        pkt_hdr->pad_len = pad_len2;
        pkt_hdr->length = (uint16)length;
        pkt_hdr->chain = DIAGBUF_SECOND_PKT_CHAIN;
        pkt_hdr->version = version;

        /* Set 'ptr' to the data following pkt_hdr */
        pkt_hdr += 1;
        return (pkt_hdr);
    }
    default:
      return NULL;
  }
}

/*===========================================================================

FUNCTION DIAGBUF_ALLOC_INTERNAL

DESCRIPTION
  This function allocates the specified amount of space in the diag
  output buffer.

DEPENDENCIES
  This function must only be called from within this file by externalized
  alloc functions.

  diagbuf_commit() must be called to commit a buffer to the diag pool.  This
  must happen reasonably soon after this call completes.  If the buffer is
  needed to accumulate data over time, then you should use your own buffer
  use this just long enough to copy the data.

============================================================================*/
static void* diagbuf_alloc_internal(
   uint32 length,                 /* Length of requested buffer */
   int nice,                      /* Niceness padding value     */
   uint8 diagbuf_alloc_request,   /* Required for DIAG Health */
   uint8 chain,                   /*0: no chain; 1: 1st packet in the chain; 2nd pakcet in the chain*/
   uint8 version
   )
{
  void *ptr = NULL;  /* Pointer to return.        */
  uint32 buffer_used;          /* Length to allocate        */
  diagbuf_header_type *pkt_hdr = NULL;
  diagbuf_header_type *prev_pkt_hdr = NULL;


  /* Local indices to manipulate buffer */
  int iHead;
  int iHead_old;
  int iHeadIdx;
  int iTail;
  int iTailIdx;

  if ( !diag_services_initialized )
    return (NULL);
  if ( length > DIAG_MAX_TX_PKT_SIZ )
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "Attempt to alloc too much: %d", (dword)length);
    return (NULL);
  }


  buffer_used =
     sizeof(diagbuf_header_type) +  /* Diagbuf header    */
                (uint32)length +  /* Number of bytes requested buffer */
                sizeof(uint16);       /* CRC                          */

  buffer_used = DIAGBUF_NEXT_ALIGNED_BYTE(buffer_used);

  ASSERT(buffer_used <= DIAGBUF_SIZE);

retry:
  prev_pkt_hdr = NULL;

  iHead_old = diagbuf_head;

  iTail = diagbuf_tail;
  iTailIdx = DIAGBUF_CALC_IDX(iTail, DIAGBUF_SIZE);

  iHead = diagbuf_head;
  iHeadIdx = DIAGBUF_CALC_IDX(iHead, DIAGBUF_SIZE);

  /* Check if valid state is read, if not re-read */
  if ( iHead_old != iHead )
  {
    goto retry;
  }

  ASSERT(iHead < 2 * DIAGBUF_SIZE);
  ASSERT(iTail < 2 * DIAGBUF_SIZE);
  ASSERT(iHeadIdx < DIAGBUF_SIZE);
  ASSERT(iTailIdx < DIAGBUF_SIZE);

  /* Set a local pointer to the header */
  pkt_hdr = ((diagbuf_header_type *)&diagbuf_buf[iHeadIdx]);

  /* First check if the nice pad is available.  If OK, then alloc */
  if ( DIAGBUF_BALANCE_PAD + nice < DIAGBUF_FREE(iHead, iTail, DIAGBUF_SIZE) )
  {

    /* if not wrapped and no room here. */
    if ( iTailIdx <= iHeadIdx &&       /* Not wrapped &&  */
         iHead - iTail < DIAGBUF_SIZE &&       /* Not full &&     */
         /* No room here!   */
         (uint32)iHeadIdx + buffer_used >= DIAGBUF_SIZE )
    {

      prev_pkt_hdr = pkt_hdr;
      /* Mark header for wrap - do this after it is known that we
          have got a valid diagbuf_head */

      /* Wrap */
      DIAGBUF_WRAP(iHead, DIAGBUF_SIZE);

      ASSERT((iHead == 0) || (iHead == DIAGBUF_SIZE));
      /* Recalculate head index */
      iHeadIdx = DIAGBUF_CALC_IDX(iHead, DIAGBUF_SIZE);
      ASSERT(iHeadIdx == 0);
    }

    if ( buffer_used <= (unsigned int)DIAGBUF_FREE(iHead, iTail, DIAGBUF_SIZE) )
    {
      /* Move head */
      DIAGBUF_MOVE(iHead, (int)buffer_used, DIAGBUF_SIZE);
      ASSERT(iHead < 2 * DIAGBUF_SIZE);

      /* Update value of global head index */
      /* If atomic compare and set fails, implies some other thread wrote
         a new diagbuf_head before this could be written :
         this requires a retry */

      if ( !osal_atomic_compare_and_set((osal_atomic_word_t *)&diagbuf_head, iHead_old, iHead) )
      {
        goto retry;
      }

      if ( prev_pkt_hdr )
      {
        prev_pkt_hdr->status = DIAGBUF_WRAP_S;
      }

      /* Set header field of buffer */
      pkt_hdr = ((diagbuf_header_type *)&diagbuf_buf[iHeadIdx]);

      pkt_hdr->status = DIAGBUF_UNCOMMIT_S;
      pkt_hdr->pad_len = (byte)(buffer_used - (uint32)length - sizeof(diagbuf_header_type)
                                - sizeof(uint16));
      pkt_hdr->length = (uint16)length;
      pkt_hdr->chain = chain;
      pkt_hdr->version = version;
      /* Set 'ptr' to pkt_hdr +1 = data following pkt_hdr */
      ptr = pkt_hdr + 1;
      /* Logic to set diagbuf_head_read here */
    }
  } /* If requested length + nice pad is free */

  if ( ptr )
  {
    /* Increment allocation requests counter */
    switch ( diagbuf_alloc_request )
    {
    case DIAGBUF_ALLOC_REQUEST_LOG:
      if ( alloc_count_log == MAX_VALUE_UINT32 )
      {
        MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Variable alloc_count_log has reached maximum value.");
      }
      else
      {
        alloc_count_log++;
      }
      break;
    case DIAGBUF_ALLOC_REQUEST_MSG:
      if ( alloc_count_f3 == MAX_VALUE_UINT32 )
      {
        MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Variable alloc_count_f3 has reached maximum value.");
      }
      else
      {
        alloc_count_f3++;
      }
      break;
    default: /* DIAGBUF_ALLOC_REQUEST_OTHER */
      /* No need to update any variables */
      break;
    }
  }
  else
  {

    switch ( diagbuf_alloc_request )
    {
    case DIAGBUF_ALLOC_REQUEST_LOG:
      if ( drop_count_log == MAX_VALUE_UINT32 )
      {
        MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Variable drop_count_log has reached maximum value.");
      }
      else
      {
        drop_count_log++;
      }
      break;
    case DIAGBUF_ALLOC_REQUEST_MSG:
      if ( drop_count_f3 == MAX_VALUE_UINT32 )
      {
        MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Variable drop_count_f3 has reached maximum value.");
      }
      else
      {
        drop_count_f3++;
      }
      break;
    default: /* DIAGBUF_ALLOC_REQUEST_OTHER */
      /* No need to update any variables */
      break;
    }
  }


  if ( ptr )
  {
    /* Sanity check */

    byte stream_id = DIAG_STREAM_1;
    if ( DIAGBUF_USED(iHead, iTail, DIAGBUF_SIZE) > DIAGBUF_SIZE )
    {
      MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "Possbile Ring buffer corrupt!");
    }
    diagbuf_write_overrun_check_pattern(pkt_hdr);
    diagbuf_write_stream_id(ptr, stream_id); /* write default stream ID for all packets allocated in diagbuf  */
  }
  return (ptr);
} /* diagbuf_alloc_internal */


/*===========================================================================

FUNCTION DIAGBUF_ALLOC

DESCRIPTION
  This function allocates the specified amount of space in the diag output
  buffer.

DEPENDENCIES
  diagbuf_commit() must be called to commit a buffer to the diag pool.  This
  must happen reasonably soon after this call completes.  If the buffer is
  needed to accumulate data over time, then you should use your own buffer
  use this just long enough to copy the data.

============================================================================*/
void* diagbuf_alloc( uint8 code, uint32 length, int nice, uint8 diagbuf_alloc_request, uint8 chain, uint8 version )
{
  void *ptr = NULL; /* Pointer to return.        */

  //TODO: Move this function here.
  ptr = diagbuf_alloc_internal(length, nice, diagbuf_alloc_request, chain, version);

  if ( ptr )
  {
    diagpkt_set_cmd_code(ptr, code);
  }

  return (ptr);
} /* diagbuf_alloc */

/*===========================================================================

FUNCTION DIAGBUF_SHORTEN

DESCRIPTION
  This function reduces the length field of a previously allocated buffer.

  'ptr' must point to the same address that was returned by a prior call to
  diagpkt_alloc().

  Allocating too much and using this to shorten the packet is ideal for
  situations in which the length of the packet is not know prior to
  allocation.  Using this scheme does, however, consume resources that would
  otherwise be used to buffer outbound diagnostics data.  Please use this
  scheme sparingly.

============================================================================*/
/* mutex inclusion in this function needs to be reviewed again */
void diagbuf_shorten(
   void *ptr,              /* Pointer returned from diagbuf_alloc() */
   unsigned int new_length /* New length of buffer                  */
   )
{
  diagbuf_header_type *pkt_hdr; /* Points to packet header */
  diagbuf_header_type *hole; /* This will point to new header field. */

  unsigned int curr_length;
  unsigned int hole_length;
  byte stream_id;
  uint32 temp_length = 0;

  if ( !diag_services_initialized )
    return;
  /* Sanity check.  Make sure it is within boundaries of ring buffer */
  if ( (byte *)ptr < diagbuf_buf ||
       (byte *)ptr >= (&diagbuf_buf[DIAGBUF_SIZE] - sizeof(uint16)) )
  {
    /* This is bad.  Incorrect usage. Report error and do nothing.*/
    MSG_2(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Invalid packet address: ptr:0x%x, l:%d", ptr, new_length);
    return;
  }

  osal_lock_mutex(&diagbuf_buf_cs);
  stream_id = diagbuf_read_stream_id(ptr);
  /* Cast pointer to do arithmetic with diagbuf_header_type */
  pkt_hdr = (diagbuf_header_type *)ptr;

  /* Subtract 1 diagbuf_header_type (4 bytes) to get header */
  pkt_hdr -= 1;

  curr_length = pkt_hdr->length;

  if ( new_length == 0 )
  {
    /* Special case: shorten to 0.
     * The entire packet is just changed to be a hole
     */
    hole = pkt_hdr;

    /* length of the hole is current length + CRC + pad */
    temp_length = DIAGBUF_NEXT_ALIGNED_BYTE(curr_length +
                                            sizeof(uint16));

    if ( temp_length > DIAGBUF_SIZE )
      ERR_FATAL("diagbuf_shorten: temp_length (%d) > DIAGBUF_SIZE (%d)", temp_length, DIAGBUF_SIZE, 0);

    /* Ensure temp_length can be stored in a uint16 */
    if ( temp_length > 0xFFFF )
      ERR_FATAL("diagbuf_shorten: temp_length (%d) > 0xFFFF", temp_length, 0, 0);

    hole->length = (uint16)temp_length;
    if( (pkt_hdr->chain == DIAGBUF_SECOND_PKT_CHAIN)
        && (pkt_hdr->version >= DIAGBUF_VERSION_1)
        && (pkt_hdr->version <= DIAGBUF_MAX_VERSION_NUM))
    {
      /* Mark the Diagbuf Header of first packet as HOLE and its length as the whole packet (Extended + Log packet) */
      pkt_hdr = ((diagbuf_header_type *)pkt_hdr) - diagbuf_ver_len[pkt_hdr->version -1];
      pkt_hdr = ((diagbuf_header_type *)pkt_hdr) - 1;
      pkt_hdr->status = DIAGBUF_HOLE_S;
      pkt_hdr->length = (uint16)temp_length + (diagbuf_ver_len[pkt_hdr->version -1] + 1)*sizeof(diagbuf_header_type);
    }
    hole->pad_len = 0;
    hole->status = DIAGBUF_HOLE_S;

    /* Since this essentially commits the packet as a hole, notify DIAG
       that the caller is ready to TX. */
    diag_tx_notify();

  }
  else
  {

    /* If new length is longer or the same, we do nothing. */
    if ( curr_length > new_length )
    {
      /* Compute hole size and record new packet length */

      pkt_hdr->length = (uint16)new_length;

      /* Adjust length fields to include CRC fields */
      new_length += sizeof(uint16);
      curr_length += sizeof(uint16);

      /* Calcualte pad for the packet */
      pkt_hdr->pad_len = (byte)(DIAGBUF_NEXT_ALIGNED_BYTE (new_length) - new_length);

      /* Now change length fields to be the length of entire buffer used */
      new_length += pkt_hdr->pad_len;
      curr_length = DIAGBUF_NEXT_ALIGNED_BYTE (curr_length);

      ASSERT(curr_length < DIAGBUF_SIZE);

      hole = (diagbuf_header_type *)ptr;

      /* Move to next aligned address past the new packet length */
      hole += new_length / sizeof(*hole);

      hole_length = curr_length - new_length;

      if ( hole_length >= sizeof(diagbuf_header_type) )
      {
        hole->status = DIAGBUF_HOLE_S;
        hole->length = (uint16)hole_length - sizeof(diagbuf_header_type);
        hole->pad_len = 0;
      }

      diagbuf_write_overrun_check_pattern(pkt_hdr);
      diagbuf_write_stream_id(ptr, stream_id);

    }
    else if ( new_length > curr_length )
    {
      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Attempted shorten %d to %d ignored.",
            curr_length, new_length);
    }
    else
    {
      /* If shortening to same size, do nothing. */
    }
  }

  osal_unlock_mutex(&diagbuf_buf_cs);

} /* diagbuf_shorten */

/*===========================================================================

FUNCTION DIAGBUF_CHECK_OVERRUN_PATTERN

DESCRIPTION
  Check for overrun pattern in the buffer provided.
============================================================================*/
static boolean diagbuf_check_overrun_pattern( diagbuf_header_type *hdr )
{
  uint8 *pattern = NULL;
  boolean success = TRUE;

  /* The 2 byte CRC field at the end of the allocated buffer is written
   * with a mask to detect buffer overrun.  Check to make sure that value
   * wasn't corrupted.
   */
  pattern = (((byte *) hdr) + hdr->length +
             sizeof(diagbuf_header_type));

  if ( (*pattern != 0xDE) || ((*(pattern + 1) != 0xAD)) )
  {
    diag_log_rsp_type *logpkt;

    success = FALSE;

    /* This detects a memory scribble.  There is no way to know how far
       past the allocated buffer the caller scribbled.  If this is
       determined to be a log, print the log code in lieu of the file
       name on the LCD.  Next, disable interrupts to avoid user of
       corrupt memory.  Reset diagbuf_head/tail so panic mode won't
       attempt to flush it, then enter panic mode as usual. */

    /* Disable all interrupts to avoid any more diagnostics client
       activitiy.  */
    osal_disable_interrupts();

    /*lint -save -e{740} Unusual pointer cast*/
    logpkt = (diag_log_rsp_type *)(hdr + 1);
    /*lint -restore */

    switch ( diagpkt_get_cmd_code(logpkt) )
    {
    case DIAG_LOG_F:
      /* Display that a log is the culprit in lieu of filename. */
      ERR_FATAL("diagbuf_check_overrun_pattern: Log %04X : pri %d", log_get_code(logpkt->log), osal_thread_get_pri(), 0);
      break;

    default:
      ERR_FATAL("error fatal in diagbuf_check_overrun_pattern", 0, 0, 0);
      break;
    }
    osal_enable_interrupts();
  }
  return (success);
}

/* For now, this feature is inert.  tx_sleep_time = 0 means no sleep will
occur.  It will remain in this state until proper values are established.
However, the implemenatation is tested, allowing testers to poke values to
avoid watchdog timeouts. */

uint64 diagbuf_tx_sleep_threshold = DIAG_TX_SLEEP_THRESHOLD_DEFAULT;
osal_timer_cnt_t diagbuf_tx_sleep_time = DIAG_TX_SLEEP_TIME_DEFAULT;

int diagbuf_commit_threshold = DIAGBUF_COMMIT_THRESHOLD;

static int diagbuf_commit_size = 0;
/*===========================================================================

FUNCTION DIAGBUF_COMMIT

DESCRIPTION
  This function commits previously allocated space in the diag output buffer.

  'ptr' must be the address that was returned by a prior call to
  diagbuf_alloc().

  This function signals the diag task and may cause a context switch.

============================================================================*/
void diagbuf_commit(
   void *ptr /* Buffer to commit */
   )
{
  diagbuf_header_type *pkt_hdr; /* Pointer to the header of the buffer. */

  /* Cast pointer to do arithmetic with diagbuf_header_type */
  pkt_hdr = (diagbuf_header_type *)ptr;

  /* Subtract 1 diagbuf_header_type (4 bytes) to get header field */
  pkt_hdr -= 1;

  /* Sanity checks */
  if ( ptr == NULL ||
       (byte *)ptr < &diagbuf_buf[sizeof(uint16)] ||
       (byte *)ptr >= &diagbuf_buf[DIAGBUF_SIZE]
       )
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Invalid commit address 0x%x.", (dword)ptr);
    return;
  }

  if ( pkt_hdr->status != DIAGBUF_HOLE_S )
  {
    if ( diagbuf_check_overrun_pattern(pkt_hdr) )
    {
      /* Set status to "committed" */
      pkt_hdr->status = DIAGBUF_COMMIT_S;
    }
  }

  diagbuf_commit_size += pkt_hdr->length;
  if ( diagbuf_commit_size >= diagbuf_commit_threshold )
  {
    diagbuf_commit_size = 0;
    diag_tx_notify();
  }
  else
  {
    diag_set_drain_timer();
  }


} /* diagbuf_commit */


/* Private functions */


/*===========================================================================

FUNCTION DIAGBUF_WRITE_OVERRUN_CHECK_PATTERN

DESCRIPTION
  Writes the overrun check pattern in the CRC field at the end of the bufer
  pointed to by 'ptr'.  diagbuf_commit() checks this field to determine if
  buffer overrun has occured.

  The work to check this is done in diagbuf_commit() rather than a seperate
  function to save the cost of calculations and the function call.

  'ptr' points to the length field before the packet, not the beginning of
  the packet itself.

===========================================================================*/
static void diagbuf_write_overrun_check_pattern( void *ptr )
{
  unsigned int length;

  if ( ptr )
  {
    length = ((diagbuf_header_type *)ptr)->length;

    /* The reserved space for the CRC is be used as a sanity check.
     * A pattern is written to those bytes in an effort to detect
     * buffer overrun.
     * The Head index is pointing to the length field.  We want to write
     * the CRC, which is 'length - sizeof(crc)' bytes ahead.
     */
    ptr = ((byte *)ptr) + length + sizeof(diagbuf_header_type);
    *((byte *)ptr) = 0xDE;
    *((byte *)ptr + 1) = 0xAD;
  }
} /* diagbuf_write_overrun_check_pattern */

/*===========================================================================
FUNCTION diagbuf_write_stream_id

DESCRIPTION
  Writes the stream ID into diagbuf.
  Packet looks like this:
  diagbuf_header(4bytes) diag_packet(variable) over-run_check_pattern(2 bytes, 0xDEAD)
  diagbuf header is of the following format:
  status(4 bits) chain(2 bits) version(2 bits) pad_length(3 bits) stream_id(5 bits)
===========================================================================*/
void diagbuf_write_stream_id( void *ptr, byte stream_id )
{
  if ( ptr )
  {
    ((diagbuf_header_type *)((diagbuf_header_type *) ptr - 1))->stream_id = stream_id;
  }
}
/*===========================================================================
FUNCTION diagbuf_set_bit_outbound_pkt_handler

DESCRIPTION
  Set the most significant bit in the stream id to indicate that we should not
  call the fp associated with F3/log in diagbuf_drain()
===========================================================================*/
void diagbuf_set_bit_outbound_pkt_handler( void *ptr )
{

  if ( ptr )
  {
    ptr = ((diagbuf_header_type *)((diagbuf_header_type *)ptr - 1));
    ((diagbuf_header_type *)ptr)->stream_id = ((diagbuf_header_type *)ptr)->stream_id | DIAGBUF_OUTBOUND_PKT_HANDLER_BIT_MASK;
  }
} /* diagbuf_set_bit_outbound_pkt_handler */

/*===========================================================================
FUNCTION diagbuf_clear_bit_outbound_pkt_handler

DESCRIPTION
  Clear the most significant bit in the stream id to indicate that we should
  call the fp associated with F3/log in diagbuf_drain()
===========================================================================*/
void diagbuf_clear_bit_outbound_pkt_handler( void *ptr )
{
  if ( ptr )
  {
    ptr = ((diagbuf_header_type *)((diagbuf_header_type *)ptr - 1));
    ((diagbuf_header_type *)ptr)->stream_id = ((diagbuf_header_type *)ptr)->stream_id & (~(DIAGBUF_OUTBOUND_PKT_HANDLER_BIT_MASK));
  }
} /* diagbuf_clear_bit_outbound_pkt_handler */

/*===========================================================================
FUNCTION diagbuf_read_stream_id

DESCRIPTION
  Reads the stream ID for the diag packet.
  Packet looks like this:
  diagbuf_header(4bytes) diag_packet(variable) over-run_check_pattern(2 bytes, 0xDEAD)
  diagbuf header is of the following format:
  status(4 bits) chain(2 bits) version(2 bits) pad_length(3 bits) stream_id(5 bits)
===========================================================================*/
byte diagbuf_read_stream_id( const void *ptr )
{
  byte stream_id = 0;

  if ( ptr )
  {
    stream_id = ((diagbuf_header_type *)((diagbuf_header_type *) ptr - 1))->stream_id;
  }
  return (stream_id);
}

/*===========================================================================
FUNCTION diagbuf_clear_index

DESCRIPTION
  Clears the index of used bytes in the send buffer for command and data channel.
  For peripheral buffering and data channel on port1, the tail and head pointers are updated.
===========================================================================*/
void diagbuf_clear_index (diagcomm_io_conn_type * conn, diagcomm_io_tx_params_type * tx_param )
{
  uint8 pad = 0;
  diag_send_buf_header_type * buf_header = NULL;
  diagcomm_enum_port_type port_num; // = conn->port_num;
  diagcomm_io_channel_type  ch_type; // = conn->channel_type;
  diag_tx_mode_type *tx_mode_ptr = NULL; //TX mode pointer to hold the send buffer info
  uint8 header_size = sizeof(diag_send_buf_header_type);

  ASSERT(conn != NULL);

  port_num = conn->port_num;
  ch_type = conn->channel_type;

  if (ch_type == DIAGCOMM_IO_CMD)
  {
    diag_send_buf_cmd_index[port_num]  = 0;
    memset(diagbuf_send_cmd[port_num] ,0,DIAG_SEND_BUF_SIZE_CMD);
  }
  else if (ch_type == DIAGCOMM_IO_DATA)
  {
    if(port_num == DIAGCOMM_PORT_1)
    {
      /* tx_mode_ptr is obtained from the diag_id field of tx_param */
      tx_mode_ptr = tx_param->tx_ptr;

      ASSERT(tx_mode_ptr != NULL);
      /*Check to see if we have sent data less than used bytes in the buffer*/
      ASSERT(tx_mode_ptr->buf_used >= tx_param->len);

      //Reset pattern and length to avoid errors when reading this data again.
      buf_header = (diag_send_buf_header_type *)(tx_mode_ptr->buf_ptr + tx_mode_ptr->buf_tail);
      if((tx_mode_ptr->buf_tail + header_size) <= tx_mode_ptr->buf_size)
      {
      buf_header->pattern = 0x00;
        buf_header->length  = 0x00;
      }

      /*Move the tail pointer by the number of bytes that we sent*/
      tx_mode_ptr->buf_tail += tx_param->len + sizeof(diag_send_buf_header_type);

      /* Buffer empty, reset head and tail to 0 */
      if(tx_mode_ptr->buf_tail == tx_mode_ptr->buf_head)
      {
        tx_mode_ptr->buf_head = sizeof(diag_send_buf_header_type);
        tx_mode_ptr->buf_tail = 0;
        buf_header = (diag_send_buf_header_type *)(tx_mode_ptr->buf_ptr + tx_mode_ptr->buf_tail);
        buf_header->pattern = 0xDEAD;
        buf_header->length = 0;
        tx_mode_ptr->curr_data_block_header = buf_header;
        tx_mode_ptr->buf_used = sizeof(diag_send_buf_header_type);
        tx_mode_ptr->curr_threshold=diag_max_send_buf_len;
      }
      else
      {
        pad = DIAG_WORD_ALIGN(tx_mode_ptr->buf_tail) - tx_mode_ptr->buf_tail ;
        tx_mode_ptr->buf_tail += pad;
        tx_mode_ptr->buf_used -= (pad + tx_param->len + sizeof(diag_send_buf_header_type));

        buf_header = (diag_send_buf_header_type *)(tx_mode_ptr->buf_ptr + tx_mode_ptr->buf_tail);

        /*Check if the new tail is pointing to a data block or not; If not, then move the tail to curr data block*/
        if(((tx_mode_ptr->buf_tail + header_size) >= tx_mode_ptr->buf_size) || 
            (buf_header->pattern != 0xDEAD) ||
            ((tx_mode_ptr->buf_tail + header_size + buf_header->length) >= tx_mode_ptr->buf_size))
        {
          /*Reached end of buffer*/
           tx_mode_ptr->buf_tail = 0;
           tx_mode_ptr->buf_used = tx_mode_ptr->buf_head;
           buf_header = (diag_send_buf_header_type *)(tx_mode_ptr->buf_ptr + tx_mode_ptr->buf_tail);

          if((buf_header->pattern != 0xDEAD) || (buf_header->length == 0))
          {
            tx_mode_ptr->buf_tail = (((uint8 *)tx_mode_ptr->curr_data_block_header) - tx_mode_ptr->buf_ptr);
            tx_mode_ptr->buf_used = tx_mode_ptr->curr_data_block_header->length + sizeof(diag_send_buf_header_type);
          }
        }
      }
      }
     else if(port_num == DIAGCOMM_PORT_2)
     {
        diag_send_buf_data_index[port_num]  = 0;
     }
    }
}

/*===========================================================================
FUNCTION diag_get_sendbuf_params

DESCRIPTION
  Populates send_buf parameter with the correct buffer to use depending on
whether we are sending a command response or data.

PARAMETERS
  *send_buf   pointer to diag_send_buf_type type
  is_cmd_rsp  TRUE : if command response is being sent
              FALSE : otherwise
  diag_id     DIAG ID based on which data is copied into appropriate buffer.
===========================================================================*/
void diag_get_sendbuf_params(diag_send_buf_type *send_buf, boolean is_cmd_rsp, uint8 diag_id)
{
  diag_tag_hdr_type* tag_hdr = NULL;
  diag_tx_mode_type *tx_mode_ptr = NULL;

  if(is_cmd_rsp)
  {
    send_buf->ptr = &diagbuf_send_cmd[DIAGCOMM_PORT_1][0];
    send_buf->head = diag_send_buf_cmd_index[DIAGCOMM_PORT_1];
    send_buf->tail = 0;
    send_buf->size = DIAG_SEND_BUF_SIZE_CMD;
    send_buf->threshold = DIAG_SEND_BUF_SIZE_CMD;
    send_buf->wrap = 0;

    /*Tag the packet with the header only if DIAG ID support is present.*/
    if (diag_tagging_support)
    {
      tag_hdr= (diag_tag_hdr_type *)( (uint8 *)send_buf->ptr + send_buf->head);
      tag_hdr->diag_id = diag_id;
      tag_hdr->reserved = 0;
      tag_hdr->length = 0;
      diag_send_buf_cmd_index[DIAGCOMM_PORT_1]+= sizeof(diag_tag_hdr_type);
      send_buf->head+= sizeof(diag_tag_hdr_type);
      send_buf->tag_hdr = tag_hdr;
    }

    send_buf->used = send_buf->head;
    send_buf->curr_data_block_header = NULL;
  }
  else
  {
    tx_mode_ptr = diag_get_tx_mode_ptr(DIAG_STREAM_1,diag_id);

    ASSERT(tx_mode_ptr != NULL);

    /*For data use the parameters from tx_mode_ptr obtained */
    send_buf->ptr = tx_mode_ptr->buf_ptr;
    send_buf->size = tx_mode_ptr->buf_size;
    send_buf->wrap = tx_mode_ptr->buf_wrap;

    send_buf->threshold = tx_mode_ptr->curr_threshold;
    send_buf->curr_data_block_header = tx_mode_ptr->curr_data_block_header;

    if(diag_tagging_support)
    {
      tag_hdr = (diag_tag_hdr_type *)((uint8 *)(send_buf->curr_data_block_header)+ sizeof(diag_send_buf_header_type));
      if(send_buf->curr_data_block_header->length >0 )
      {
        tag_hdr->length = send_buf->curr_data_block_header->length - sizeof(diag_tag_hdr_type);
      }
      else
      {
        tag_hdr->diag_id = diag_id;
        tag_hdr->reserved = 0;
        tag_hdr->length = 0;
        tx_mode_ptr->curr_data_block_header->length+= sizeof(diag_tag_hdr_type);
        tx_mode_ptr->buf_head+= sizeof(diag_tag_hdr_type);
        tx_mode_ptr->buf_used+= sizeof(diag_tag_hdr_type);
      }
      send_buf->tag_hdr = tag_hdr;
  }
    else
    {
      send_buf->tag_hdr = NULL;
    }

    send_buf->used = tx_mode_ptr->buf_used;
    send_buf->tail = tx_mode_ptr->buf_tail;
    send_buf->head = tx_mode_ptr->buf_head;
  }
}

/*===========================================================================
FUNCTION diag_update_sendbuf_params

DESCRIPTION
  Updates diag_tx_mode parameters when we aggregate data in the send buffer.

PARAMETERS
  *send_buf   pointer to diag_send_buf_type type
  is_cmd_rsp  TRUE : if command response is being sent
              FALSE : otherwise
  diag_id     DIAG ID based on which data is copied into appropriate buffer
===========================================================================*/
void diag_update_sendbuf_params(diag_send_buf_type *send_buf, boolean is_cmd_rsp, uint8 diag_id)
{
  diag_tx_mode_type *tx_mode_ptr = NULL;
  diag_tag_hdr_type* tag_hdr = NULL;

  if(is_cmd_rsp)
  {
  /*Update the index for command flat buffer*/
    diag_send_buf_cmd_index[DIAGCOMM_PORT_1] = send_buf->head;
  }
  else
  {
  if((send_buf->used > send_buf->size) || (send_buf->head > send_buf->size))
  {
    ASSERT(0);
  }
    
    tx_mode_ptr = diag_get_tx_mode_ptr(DIAG_STREAM_1,diag_id);
    ASSERT(tx_mode_ptr != NULL);

    /*Update the head, tail and wrap flags for data channel*/
    tx_mode_ptr->buf_head = send_buf->head;
    tx_mode_ptr->buf_tail = send_buf->tail;
    tx_mode_ptr->buf_wrap = send_buf->wrap;
    tx_mode_ptr->buf_used = send_buf->used;
    tx_mode_ptr->curr_data_block_header = send_buf->curr_data_block_header;

    if(diag_tagging_support)
    {
      tag_hdr = (diag_tag_hdr_type *)((uint8 *)(send_buf->curr_data_block_header)+ sizeof(diag_send_buf_header_type));
      tag_hdr->length = send_buf->curr_data_block_header->length - sizeof(diag_tag_hdr_type);
  }
  }
}

/*===========================================================================
FUNCTION diag_get_padlen

DESCRIPTION
  Calculates and returns the pad length.

PARAMETERS
  *desc : Pointer to diag_send_desc_type used to calcuate pad length.
===========================================================================*/
uint8 diag_get_padlen( diag_send_desc_type *desc )
{
   uint8 pad_len = 0;

    if ((DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].feature_mask, F_DIAG_HDLC_ENCODE_IN_APPS))\
          && (DIAG_INT_FEATURE_MASK_CHECK(F_DIAG_HDLC_ENCODE_IN_APPS)))
    {
       /*If peripheral is not HDLC encoding the data, we have to account for 4 bytes of header and 1 byte of tail*/
       pad_len = 5;
    }
    else
    {
      if (desc->terminate)
      {
         /* If terminal packet, allow for minimal padding. */
         pad_len = MIN(((int)desc->last - (int)desc->pkt) + DIAG_HDLC_CRC_PAD, diagbuf_hdlc_pad_len);
      }
      else
      {
        /* If non-terminal packet, add more padding to account for the rest of the packet needing to fit in
        same DSM chain. */
        pad_len = diagbuf_hdlc_pad_len;
      }
    }
    return pad_len;
}

/*===========================================================================
FUNCTION diag_get_send_size

DESCRIPTION
  Returns the data length to be sent.

PARAMETERS
  port_type : diagcomm_enum_port_type type
  tx_mode_ptr: tx_mode_ptr to retrieve the destination buffer.
===========================================================================*/
uint32 diag_get_send_size(diagcomm_enum_port_type port_type, diag_tx_mode_type *tx_mode_ptr)
{
  ASSERT(tx_mode_ptr!= NULL );
  diag_send_buf_header_type *diag_send_buf_header = (diag_send_buf_header_type *)(tx_mode_ptr->buf_ptr + tx_mode_ptr->buf_tail);

  if(diag_send_buf_header->pattern != DIAGBUF_HEADER_PATTERN)
  {
    return 0;
  }

  return diag_send_buf_header->length;
}

/*===========================================================================
FUNCTION DIAG_ADD_BLOCK_HEADER

DESCRIPTION
  This functions adds a new block in the send buffer and currently invoked
  only in buffering modes. Buffering mode is currently supported only on
  STREAM_1.

DEPENDENCIES

===========================================================================*/
void diag_add_block_header(diag_send_buf_type* send_buf, uint16 length, uint8 diag_id)
{
  int pad = 0;
  uint8* ptr = NULL;
  uint32 new_head = 0;
  uint32 free_size = 0;
  uint32 min_threshold = 0;
  diag_send_buf_header_type *tail_data_block_header = NULL;
  diag_send_buf_header_type *head_data_block_header = NULL;
  uint8 stream_index = 0;
  diag_tag_hdr_type* tag_hdr = NULL;
  diag_tx_mode_type *l_tx_mode = NULL;
  uint32 tag_size = 0;
  uint8 header_size = sizeof(diag_send_buf_header_type);
  stream_index = DIAG_ID_TO_INDEX(DIAG_STREAM_1);

  l_tx_mode = diag_get_tx_mode_ptr(DIAG_STREAM_1,diag_id);
  ASSERT(l_tx_mode != NULL);

  if (diag_tagging_support)
  {
    tag_size = sizeof(diag_tag_hdr_type);
  }
  else
    {
    tag_size = 0;
  }

  /* Consider header size and tag size as well */
  length = length + header_size + tag_size;

  /* Make the Head 4 byte aligned before making any calculations */
  pad = (uint32)DIAG_WORD_ALIGN((uint32)send_buf->head) - (uint32)send_buf->head;
  send_buf->head += pad;

  if((send_buf->head + length) >= send_buf->size )
  {
        /*Head is at the end of buffer and incoming log cannot fit here, Wrap here*/
    new_head = 0;

    /* If tail is ahead of head, reset tail as well to start of buffer */
        if(send_buf->head <= send_buf->tail)
        {
      /* Clear pattern and length only if head is less than size by atleast header size to avoid
        overwriting out of buffer memory */
      if((send_buf->tail + header_size) <= send_buf->size)
      {
            tail_data_block_header = (diag_send_buf_header_type *)(send_buf->ptr + send_buf->tail);
        tail_data_block_header->pattern = 0;
        tail_data_block_header->length = 0;
        }
      send_buf->tail = 0;
    }
    /* Clear pattern and length only if head is less than size by atleast header size to avoid
       overwriting out of buffer memory */
    if((send_buf->head + header_size) <= send_buf->size)
    {
      head_data_block_header=(diag_send_buf_header_type *)(send_buf->ptr + send_buf->head);
      head_data_block_header->pattern = 0;
      head_data_block_header->length = 0;
    }
  }
    else
    {
        new_head = send_buf->head;
    }

    /*Calculate free size available for new block being created
    If head is greater than tail, maximum available is till the end of buffer
    or if tail is greater than head, then maximum available will be up to tail
    index, in such case we move tail such that we have enough pad which is
    maximum TX size*/

  if(new_head > send_buf->tail)
    {
        free_size = send_buf->size - new_head ;
    }
    else if (new_head < send_buf->tail)
    {
        free_size = send_buf->tail - new_head;
    }
    else
    {
    /* Buffer full, no more space left */
    if( l_tx_mode->buf_used > header_size)
        {
      free_size = 0;
        }
    /* Buffer empty */
        else
        {
            free_size = send_buf->size - new_head;
        }
    }

    /* Move the tail till we have the free size available which is equal to
     maximum block pad size. */
    while(free_size < length)
    {
    /* Move Tail */
        tail_data_block_header = (diag_send_buf_header_type *)(send_buf->ptr + send_buf->tail);
    send_buf->tail += tail_data_block_header->length + header_size;
        pad = (uint32)DIAG_WORD_ALIGN((uint32)send_buf->tail) - (uint32)send_buf->tail;
    send_buf->tail += pad;
    tail_data_block_header->pattern = 0;
    tail_data_block_header->length = 0;

        tail_data_block_header = (diag_send_buf_header_type *)(send_buf->ptr + send_buf->tail);

    /* Invalid block at tail, reset tail to 0 */
    if(((send_buf->tail + header_size) >= send_buf->size) || 
        (tail_data_block_header->pattern != 0xDEAD) || 
        ((send_buf->tail + header_size + tail_data_block_header->length) >= send_buf->size))
        {
      if((send_buf->tail + header_size) <= send_buf->size )
      {
        tail_data_block_header->pattern = 0;
        tail_data_block_header->length = 0;
      }
      send_buf->tail = 0;
        }

        /* Tail moved so check for free size again*/
    if((new_head + header_size) > send_buf->tail)
        {
      free_size = send_buf->size - new_head;
        }
        else
        {
      free_size = send_buf->tail - new_head;
        }
    }

  min_threshold = free_size - (header_size + tag_size);

    /*Threshold indicates the maximum index that head can write up to
    and this threshold is used in diagbuf_send_pkt_single_proc() to add a new block */
  send_buf->threshold = MIN(min_threshold, diag_max_send_buf_len);
    send_buf->curr_data_block_header = (diag_send_buf_header_type *)(send_buf->ptr + new_head);
  pad = (uint32)DIAG_WORD_ALIGN((uint32)send_buf->curr_data_block_header) - (uint32)send_buf->curr_data_block_header;
  ptr = (uint8 *)send_buf->curr_data_block_header;
  ptr += pad;
  send_buf->curr_data_block_header = (diag_send_buf_header_type *)ptr;
  l_tx_mode->curr_data_block_header = send_buf->curr_data_block_header;
  l_tx_mode->curr_data_block_header->pattern = 0xDEAD;
  l_tx_mode->curr_data_block_header->length = 0;
  send_buf->head = pad + (uint32)(new_head) + header_size;

  if (diag_tagging_support)
  {
    tag_hdr = (diag_tag_hdr_type *)((uint8 *)(send_buf->curr_data_block_header)+ header_size);
    tag_hdr->diag_id = diag_id;
    tag_hdr->reserved = 0;
    tag_hdr->length = 0;
    l_tx_mode->curr_data_block_header->length += sizeof(diag_tag_hdr_type);
    send_buf->head += sizeof(diag_tag_hdr_type);
  }

    /*Update global parameters*/
  l_tx_mode->buf_head = send_buf->head;
  l_tx_mode->buf_tail = send_buf->tail;
  l_tx_mode->buf_wrap = send_buf->wrap;
  l_tx_mode->buf_used = diag_buffered_bytes(stream_index, diag_id);
  l_tx_mode->curr_threshold = send_buf->threshold;

  send_buf->used = l_tx_mode->buf_used;

  if((send_buf->used > send_buf->size) || (send_buf->head > send_buf->size))
  {
    ASSERT(0);
  }
}
/*===========================================================================
FUNCTION DIAGBUF_SEND_PKT_SINGLE_PROC

DESCRIPTION
  This function sends a packet, or a fragment of a packet, through the comm
  layer.  It handles HDLC encoding as well as flow control.

  If 'pkt' is NULL, the buffer in progress is flushed to the comm layer,
  making no assumptions about HDLC, packet termination, etc.

DEPENDENCIES
  This routine maintains state due to flow control requirements.
  The caller is responsible for maintaining integrity of the data stream.

===========================================================================*/
diagbuf_status_enum_type diagbuf_send_pkt_single_proc (diag_send_desc_type *desc, byte diag_id)
{
   diagbuf_status_enum_type status = DIAGBUF_READY_S;
   static diag_hdlc_dest_type enc = { NULL, NULL, 0 };
   uint8 pad_len = 0;
   uint8 stream_index = 0;
   uint16 send_size = 0;
   uint16 pkt_length = 0;
   boolean pre_send = FALSE;
   boolean is_end_of_buf = FALSE;
   boolean send_now = FALSE;
   diag_send_buf_type send_buf;
   boolean add_block_header = FALSE;
   boolean diag_not_connected = FALSE;
   boolean is_cmd_rsp_type = FALSE;   
   diag_tx_mode_type *l_tx_mode = NULL;

   if(desc!=NULL)
   	if(desc->is_cmd_rsp_type == TRUE)
		is_cmd_rsp_type = TRUE;
   
   if(is_cmd_rsp_type == TRUE)
   {
	 if (!(diagcomm_cmd_status(DIAGCOMM_PORT_1) && DIAG_CHECK_MASK(diag_cx_state, DIAG_CX_COMM_S) \
	 	&& (diagcomm_io_ctrl_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].feature_mask[0] > 0)))
     {
        diag_not_connected = TRUE;
     }
   }
   else
   {
	 if (!(diagcomm_status() && DIAG_CHECK_MASK(diag_cx_state, DIAG_CX_COMM_S) \
	 	&& (diagcomm_io_ctrl_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].feature_mask[0] > 0)))
     {
        diag_not_connected = TRUE;
     }   
   }   

   stream_index = DIAG_ID_TO_INDEX(DIAG_STREAM_1);

   l_tx_mode = diag_get_tx_mode_ptr(DIAG_STREAM_1,diag_id);

   if(l_tx_mode == NULL)
   {
     return DIAGBUF_NOT_READY_S;
   }

   if(diag_not_connected)
   {

      /* DIAG is not connected to the comm layer.  Listeners may still be
         active, though.  Throw away any buffer that may be lingering. */
      if (l_tx_mode->buf_used > sizeof(diag_send_buf_header_type)) {
         l_tx_mode->buf_used = sizeof(diag_send_buf_header_type);
      }

      /* Now discard inbound data by adjusting the descriptor to indicate
         that all data was sent.  Return status that we are ready for more.  */
      if (desc) {
         uint8 *ptr = (uint8 *)desc->last;
         ptr++;
         desc->pkt = (void *)ptr;
         desc->state = DIAG_SEND_STATE_COMPLETE;
         status = DIAGBUF_READY_S;
      }
      /* Commit local copies of persistent data back to static variables and return */
      return status;
   }

/*Logic to check if we have to flush bufferred data*/

   if (desc == NULL) {
      /* Flush the buffer to the comm layer. */
      if (l_tx_mode->buf_used > sizeof(diag_send_buf_header_type) + diag_tag_hdr_size) {
         diag_get_sendbuf_params(&send_buf, FALSE, diag_id);
         pre_send = TRUE;
      }

      if (l_tx_mode->mode == DIAG_TX_MODE_BUFFERED_CIR) {
         pre_send = FALSE;
      }

   } else {
      if (status != DIAGBUF_READY_S && desc->state >= DIAG_SEND_STATE_COMPLETE) {
         return status;
      }

      /*Get the send buffer parameters*/
      diag_get_sendbuf_params(&send_buf, desc->is_cmd_rsp_type, desc->diag_id);
      if (!is_cmd_rsp_type) {
         /*Calculate the pad length required*/
         pad_len = diag_get_padlen(desc);
         /* The condition below checks if the packet is a packet whose source size won't
            fit in the existing buffer */
         pkt_length = (((int)desc->last - (int)desc->pkt) + pad_len);

         if (pkt_length > (send_buf.size - send_buf.head)) {
            is_end_of_buf = TRUE;
         }

         /*Process incoming desc*/
            if ((((send_buf.curr_data_block_header->length + pkt_length + diag_send_buf_pad) > send_buf.threshold) && (desc->state == DIAG_SEND_STATE_START)) ||
                ((log_ext_hdr_start == TRUE ) && ((send_buf.threshold - (send_buf.curr_data_block_header->length + diag_send_buf_pad )) < DIAG_MAX_TX_PKT_SIZ)))

         {
            pre_send = TRUE;


            if ( l_tx_mode->mode == DIAG_TX_MODE_BUFFERED_CIR)
            {
               pre_send = FALSE;
            }
            }
         }
      }

   /*Flush the buffered data before processing the incoming data*/
   if (pre_send == TRUE) {
      send_size = diag_get_send_size(DIAGCOMM_PORT_1, l_tx_mode);
      if (diag_tag_hdr_size >= send_size) {
         MSG_1(MSG_SSID_DIAG, MSG_LEGACY_LOW, "invalid send_size %d", send_size);
      } else {

         diagcomm_send((void *)(send_buf.ptr + send_buf.tail + sizeof(diag_send_buf_header_type)), send_size, FALSE, DIAGCOMM_PORT_1, l_tx_mode);
         diag_get_sendbuf_params(&send_buf, FALSE, diag_id);

      }
   }

   /*Drop the incoming data if we get flow controlled during the previous send
   or if desc is NULL*/
   if (desc == NULL)
   {
     return status;
   }

   /* if not command response and channel is flow controlled, drop data and return*/
   if ((!is_cmd_rsp_type) && (FALSE == diagcomm_io_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].allow_flow)) {
     status = DIAGBUF_NOT_READY_S;
	 log_ext_hdr_start = FALSE;
      return status;
   }

   if (!desc->is_cmd_rsp_type) {
     if ((((send_buf.curr_data_block_header->length + pkt_length + diag_send_buf_pad) > send_buf.threshold) && (desc->state == DIAG_SEND_STATE_START)) ||
                ((log_ext_hdr_start == TRUE ) && ((send_buf.threshold - (send_buf.curr_data_block_header->length + diag_send_buf_pad )) < DIAG_MAX_TX_PKT_SIZ)))
     {
       add_block_header = TRUE;
     }
     if (add_block_header)
     {
       /* Add a header here to mark the start of a data block;
          The flat buffer is divided into data blocks
          header pattern is: |0xDEAD<2 bytes> | length <2 bytes>| */
       if(log_ext_hdr_start == FALSE)
       {
         diag_add_block_header(&send_buf, (pkt_length + diag_send_buf_pad ), diag_id);
       }
       else
       {
         diag_add_block_header(&send_buf, (pkt_length + diag_send_buf_pad + DIAG_MAX_TX_PKT_SIZ ), diag_id);
         log_ext_hdr_start=FALSE;
       }
       /*Update the local copy of buffer parameters*/
       diag_get_sendbuf_params(&send_buf, desc->is_cmd_rsp_type, desc->diag_id);
     }
   }
		 
   if ((!desc->is_cmd_rsp_type) && (!(l_tx_mode->mode == DIAG_TX_MODE_BUFFERED_CIR))) {
      if (is_end_of_buf) {
         if ((pkt_length > (send_buf.size - send_buf.head)) && (send_buf.tail > (pkt_length + sizeof(diag_send_buf_header_type)))) {
            send_buf.head = 0;
            diag_add_block_header(&send_buf, (pkt_length + diag_send_buf_pad), desc->diag_id);
         } else {
            return status;
         }
      }
   }

   enc.dest = (void *)((uint32)send_buf.ptr + send_buf.head);

   if (send_buf.head >= send_buf.tail) {
      enc.dest_last = (void *)(send_buf.ptr + send_buf.size - 1);
   } else {
      enc.dest_last = (void *)(send_buf.ptr + send_buf.tail - 1);
   }

   if ((DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].feature_mask, F_DIAG_HDLC_ENCODE_IN_APPS))\
          && (DIAG_INT_FEATURE_MASK_CHECK(F_DIAG_HDLC_ENCODE_IN_APPS))) {
      diag_non_hdlc_encode(desc, &enc);
   } else {
      diag_hdlc_encode(desc, &enc);
   }

   /*Update the used bytes*/
   send_buf.used += (uint32)enc.dest - (uint32)((uint32)send_buf.ptr + send_buf.head);

   if (!is_cmd_rsp_type) {
      send_buf.curr_data_block_header->length += (uint32)enc.dest - (uint32)((uint32)send_buf.ptr + send_buf.head);

      if(diag_tagging_support)
      {
        send_buf.tag_hdr->length += (uint32)enc.dest - (uint32)((uint32)send_buf.ptr + send_buf.head);
   }
   }

   send_buf.head = (uint32)enc.dest - (uint32)send_buf.ptr;

   /*Check to see if head is past the buffer size*/
   if ((send_buf.head >= (send_buf.size - 1)) && (!is_cmd_rsp_type)) {

      send_buf.head = 0;
      diag_add_block_header(&send_buf, (pkt_length + diag_send_buf_pad), diag_id);
   }

   /*---------------------------------*/
   /* Check to see if we should send the data right away*/
   if (is_cmd_rsp_type)
   {
      diag_send_buf_cmd_index[DIAGCOMM_PORT_1]=send_buf.used;
      if(diag_tagging_support)
      {
        send_buf.tag_hdr->length = send_buf.used - sizeof(diag_tag_hdr_type);
   }
      diagcomm_send((void *)(send_buf.ptr + send_buf.tail), send_buf.used, TRUE, DIAGCOMM_PORT_1, NULL);
   }
   else
   {
      if ((enc.dest >= enc.dest_last) && (desc->pkt > desc->last) && (desc->state == DIAG_SEND_STATE_COMPLETE) && (desc->terminate) )
      {
         send_now = TRUE;
      }

      if (((((uint8 *)enc.dest) + diag_send_buf_pad) >= (send_buf.ptr + send_buf.size)) && (desc->state == DIAG_SEND_STATE_COMPLETE) && (desc->terminate) )
      {
         send_now = TRUE;
      }

      /*Hack to handle 8.5k log packets */
      if( (pkt_length >= diag_max_send_buf_len) && (TRUE == diagcomm_io_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].allow_flow))
      {
        send_now = TRUE;
      }
      if (l_tx_mode->mode == DIAG_TX_MODE_BUFFERED_CIR)
      {
        send_now = FALSE;
      }
       
      diag_update_sendbuf_params(&send_buf, desc->is_cmd_rsp_type, desc->diag_id);

      if (send_now)
      {
        send_size = diag_get_send_size(DIAGCOMM_PORT_1,l_tx_mode);
        if (diag_tag_hdr_size >= send_size)
        {
          MSG_1(MSG_SSID_DIAG, MSG_LEGACY_LOW, "diagbuf_send_pkt_single_proc : invalid send_size %d", send_size);
            }
        else
        {
          diagcomm_send((void *)(send_buf.ptr + send_buf.tail + sizeof(diag_send_buf_header_type)), send_size, FALSE, DIAGCOMM_PORT_1, l_tx_mode);
            }
         }
      }

   return status;
}

/*===========================================================================
FUNCTION diag_send_buf_retry

DESCRIPTION
  This function is called when diag is signaled to resend the buffer if
  it was not able to send all the data successfully in first attempt
===========================================================================*/
void diag_send_buf_retry(diagcomm_io_channel_type ch_type, diagcomm_enum_port_type port_type, diag_tx_mode_type *tx_mode_ptr)
{
   uint16 send_size = 0;

   if (ch_type == DIAGCOMM_IO_DATA)
            {
     if (port_type == DIAGCOMM_PORT_1) 
     {
        send_size = diag_get_send_size(DIAGCOMM_PORT_1, tx_mode_ptr);
        if (diag_tag_hdr_size >= send_size)
        {
          MSG_1(MSG_SSID_DIAG, MSG_LEGACY_LOW, "diag_send_buf_retry : invalid send_size %d", send_size);
        }
        else 
        {
          if (tx_mode_ptr == NULL)
          {
            ERR_FATAL("tx_mode_ptr invalid for ch_type %d port_type %d ",ch_type, port_type ,0 );
          }
          diagcomm_send((void *)(tx_mode_ptr->buf_ptr + tx_mode_ptr->buf_tail + sizeof(diag_send_buf_header_type)), send_size, FALSE, DIAGCOMM_PORT_1, tx_mode_ptr);
          if(diag_drain_buffer_check(DIAG_STREAM_1, tx_mode_ptr))
            {
                diag_set_internal_sigs(DIAG_INT_DRAIN_BUFFERING_BUFFER_SIG);
            }
         }
     } 
     else if (port_type == DIAGCOMM_PORT_2)
     {
        if ((diag_send_buf_data_index[port_type] >= DIAG_SEND_BUF_SIZE_DATA))
        {
            ASSERT(0);
         }
        if (tx_mode_ptr == NULL)
        {
          ERR_FATAL("tx_mode_ptr invalid for ch_type %d port_type %d ",ch_type, port_type ,0 );
      }
        diagcomm_send((void *)diagbuf_send_data[port_type], diag_send_buf_data_index[port_type], FALSE, port_type, tx_mode_ptr);
     }
      }
   else if (ch_type == DIAGCOMM_IO_CMD)
   {
     if ((diag_send_buf_cmd_index[port_type] >= DIAG_SEND_BUF_SIZE_CMD))
     {
         ASSERT(0);
      }
   }
}

/*===========================================================================

FUNCTION DIAGBUF_SEND_PKT

DESCRIPTION

===========================================================================*/
diagbuf_status_enum_type diagbuf_send_pkt (diag_send_desc_type *desc, byte stream_id)
{
    diagbuf_status_enum_type status = DIAGBUF_READY_S;
    void *send_last = NULL;
    static boolean diag_dci_pending = FALSE;
    diag_diagID_pname_type *diagid_ptr = NULL;
    uint8 id = DIAGID_UNKNOWN_ID;


    if(!desc) {
        if(stream_id & DIAG_STREAM_1)
    {
          diagid_ptr = diag_diagID_pname_list;
          /* Attempt to drain the pending data of all diagIDs */
          while(diagid_ptr !=NULL)
      {
            id = diagid_ptr->diagID;
            diagbuf_send_pkt_single_proc(NULL,id);
          if(FALSE == diagcomm_io_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].allow_flow)
        {
          status = DIAGBUF_NOT_READY_S;
              break;
            }
            diagid_ptr=diagid_ptr->next;
        }
      return status;
    }
  	}

    if (stream_id & DIAG_STREAM_1)
  	{
        if( (stream_id != (DIAG_STREAM_1 | DIAG_STREAM_2))
                || ((desc->last != send_last) && !diag_dci_pending)
        )
    {
         if (((FALSE == desc->is_cmd_rsp_type) && (FALSE == diagcomm_io_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].allow_flow)) ||
            ((TRUE == desc->is_cmd_rsp_type) && (FALSE == diagcomm_io_cmd_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].allow_flow))
            )
         {
            status = DIAGBUF_NOT_READY_S;
            return status;
         }
        status = diagbuf_send_pkt_single_proc(desc, desc->diag_id);
            send_last = (void *)desc->last;
        }
         }
	  
  return status;
} /* diagbuf_send_pkt */

/*===========================================================================

FUNCTION DIAG_HDLC_ENCODE

DESCRIPTION
  This routine copies packet fragments from *src_desc to *enc.

  While copying, it performs step-wise CRC calculations and HDLC encoding.

  These descriptor structures contain memory ranges, states, etc to handle
  packetization.

  If enc->dest is NULL and src_desc is in the "start" state, CRC is
  initialized and the state is transitioned to "busy".

DEPENDENCIES
  This routine maintains state due to flow control requirements.
  The caller is responsible for maintaining integrity of the data stream.

===========================================================================*/
boolean diag_hdlc_protocol = TRUE;

static diag_enhc_encode_state diag_enhc_state = DIAG_STATE_START;
static uint16 diag_enhc_length = 0;
static uint16 diag_enhc_total_length = 0;
static uint8 *diag_enhc_length1;
static uint8 *diag_enhc_length2;

#ifdef FEATURE_DIAG_STRESS_PROFILE
uint64 diag_hdlc_pcycles_total = 0;
#endif /* FEATURE_DIAG_SRESS_PROFILE */

void diag_hdlc_encode( diag_send_desc_type *src_desc, diag_hdlc_dest_type *enc )
{
  uint8 *dest;
  uint8 *dest_last;
  const uint8 *src;
  const uint8 *src_last;
  uint16 crc;
  byte src_byte = 0;
  diag_send_state_enum_type state;
  unsigned int used = 0;
#ifdef FEATURE_DIAG_STRESS_PROFILE
  uint64 pcycle_start, pcycle_end;

  pcycle_start = qurt_get_core_pcycles();
#endif /* FEATURE_DIAG_SERSS_PROFILE */

  if ( diag_hdlc_protocol )
  {

    if ( src_desc && enc )
    {
      /* Copy important parts to local variables. */
      src = src_desc->pkt;
      src_last = src_desc->last;
      state = src_desc->state;
      dest = enc->dest;
      dest_last = enc->dest_last;

      if ( state == DIAG_SEND_STATE_START )
      {
        crc = CRC_16_L_SEED;
        state++;
      }
      else
      {
        /* Get a local copy of the CRC */
        crc = enc->crc;
      }

      /* dest or dest_last may be NULL to trigger a state transition only */
      if ( dest && dest_last )
      {
        /* This condition needs to include the possiblitiy of 2 dest
           bytes for an escaped byte */
        while ( src <= src_last && dest <= dest_last )
        {
          src_byte = *src++;

          if ( (src_byte == DIAG_ASYNC_CONTROL_CHAR) ||
               (src_byte == DIAG_ASYNC_ESC_CHAR) )
          {

            /* If the escape character is not the last byte */
            if ( dest != dest_last )
            {
              crc = CRC_16_L_STEP(crc, src_byte);
              *dest++ = DIAG_ASYNC_ESC_CHAR;
              used++;
              *dest++ = src_byte ^ DIAG_ASYNC_ESC_MASK;
              used++;
            }
            else
            {
              /* no space available to copy the escape character, so pack
                it in the next one */
              src--;
              break;
            }
          }
          else
          {
            crc = CRC_16_L_STEP(crc, src_byte);
            *dest++ = src_byte;
            used++;
          }
        }

        if ( src > src_last )
        {
          if ( state == DIAG_SEND_STATE_BUSY )
          {
            if ( src_desc->terminate )
            {
              crc = ~crc;
              state++;
            }
            else
            {
              /* Done with fragment, no CRC/term to be done. */
              state = DIAG_SEND_STATE_COMPLETE;
            }
          }

          while ( dest <= dest_last && state >= DIAG_SEND_STATE_CRC1 &&
                  state < DIAG_SEND_STATE_TERM )
          {
            /* Encode a byte of the CRC next */
            src_byte = crc & 0xFF;

            /* Copied from above for runtime efficiency */
            if ( (src_byte == DIAG_ASYNC_CONTROL_CHAR) ||
                 (src_byte == DIAG_ASYNC_ESC_CHAR) )
            {

              if ( dest != dest_last )
              {
                *dest++ = DIAG_ASYNC_ESC_CHAR;
                used++;
                *dest++ = src_byte ^ DIAG_ASYNC_ESC_MASK;
                used++;
                /* Shift to the next CRC byte.  CRC is 0 when complete. */
                crc >>= 8;
              }
              else
              {
                /* no space available to copy the crc, pack it in the next one */
                break;
              }
            }
            else
            {
              /* Shift to the next CRC byte.  CRC is 0 when complete. */
              crc >>= 8;
              *dest++ = src_byte;
              used++;
            }

            state++;
          }

          if ( state == DIAG_SEND_STATE_TERM )
          {
            if ( dest_last >= dest )
            {
              *dest++ = DIAG_ASYNC_CONTROL_CHAR;
              used++;
              state++; /* Complete */
            }
          }
        }
      }
      /* Copy local variables back into the encode structure. */
      enc->dest = dest;
      enc->dest_last = dest_last;
      enc->crc = crc;
      src_desc->pkt = src;
      src_desc->last = src_last;
      src_desc->state = state;
    }

  }

  else
  {

    src = src_desc->pkt;
    src_last = src_desc->last;

    state = src_desc->state;

    dest = enc->dest;
    dest_last = enc->dest_last;

      switch ( diag_enhc_state )
      {

      case DIAG_STATE_START:

        *dest++ = DIAG_ASYNC_CONTROL_CHAR;
        if ( dest > dest_last )
        {
          diag_enhc_state = DIAG_STATE_WRITE_FIRST_BYTE_LEN;
          break;

        }
        diag_enhc_state = DIAG_STATE_WRITE_FIRST_BYTE_LEN;

      case DIAG_STATE_WRITE_FIRST_BYTE_LEN:

        if ( dest_last - dest >= 1 )
        {
          /* Memorize location of length field */
          diag_enhc_length1 = dest++;
          diag_enhc_length2 = dest++;

          if ( dest > dest_last )
          {
            diag_enhc_state = DIAG_STATE_WRITE_PACKET;
            break;

          }
          /* Had enough space - goto writing the packet*/
          diag_enhc_state = DIAG_STATE_WRITE_PACKET;
          goto write_packet;

        }
        else if ( dest_last - dest == 0 )
        {
          diag_enhc_length1 = dest++;
          diag_enhc_state = DIAG_STATE_WRITE_SECOND_BYTE_LEN;
          break;


        }


      case DIAG_STATE_WRITE_SECOND_BYTE_LEN:

        diag_enhc_length2 = dest++;
        if ( dest > dest_last )
        {
          diag_enhc_state = DIAG_STATE_WRITE_PACKET;
          break;

        }
        diag_enhc_state = DIAG_STATE_WRITE_PACKET;

      case DIAG_STATE_WRITE_PACKET:
        {
        int length_current_chunk = 0;
        write_packet:

          diag_enhc_length = src_last - src + 1;

          if ( diag_enhc_length > (dest_last - dest + 1) )
          {
            length_current_chunk = dest_last - dest + 1;
          }
          else
          {
            length_current_chunk = diag_enhc_length;
          }
          memscpy(dest, (dest_last - dest + 1), src, length_current_chunk);
          dest += length_current_chunk;
          diag_enhc_length -= length_current_chunk;
          src_desc->pkt = src + length_current_chunk;
          diag_enhc_total_length += length_current_chunk;

          if ( dest > dest_last )
          {
            if ( diag_enhc_length == 0 )
            {

              diag_enhc_state = DIAG_STATE_END;

            }
            break;


          }
          diag_enhc_state = DIAG_STATE_END;
        }
      case DIAG_STATE_END:
        {

          src_desc->state = DIAG_SEND_STATE_COMPLETE;

          if ( src_desc->terminate )
          {
            /*Go update the total length */
            *diag_enhc_length1 = *((uint8 *)&diag_enhc_total_length);
            *diag_enhc_length2 = *((uint8 *)(&diag_enhc_total_length) + 1);
            *dest++ = DIAG_ASYNC_CONTROL_CHAR;
            diag_enhc_total_length = 0;
            diag_enhc_state = DIAG_STATE_START;
          }
          else
          {
            diag_enhc_state = DIAG_STATE_WRITE_PACKET;
          }
        }

      }

    enc->dest = dest;
    enc->dest_last = dest_last;

    src_desc->last = src_last;

  }
#ifdef FEATURE_DIAG_STRESS_PROFILE
  pcycle_end = qurt_get_core_pcycles();
  diag_hdlc_pcycles_total += pcycle_end - pcycle_start;
#endif /* FEATURE_DIAG_STRESS_PROFILE */
  return;
}

/*===========================================================================

FUNCTION DIAG_NON_HDLC_ENCODE

DESCRIPTION
  This routine copies packet fragments from *src_desc to *enc.

  These descriptor structures contain memory ranges, states, etc to handle
  packetization.

DEPENDENCIES
  This routine maintains state due to flow control requirements.
  The caller is responsible for maintaining integrity of the data stream.

===========================================================================*/
void diag_non_hdlc_encode( diag_send_desc_type *src_desc, diag_hdlc_dest_type *enc )
{
  uint8 *dest;
  uint8 *dest_last;
  const uint8 *src;
  const uint8 *src_last;

  static diag_enhc_encode_state diag_non_hdlc_state = DIAG_STATE_START;
  static uint16 diag_non_hdlc_length = 0;
  static uint16 diag_non_hdlc_total_length = 0;
  static uint8 *diag_non_hdlc_length1;
  static uint8 *diag_non_hdlc_length2;
  
  int length_current_chunk = 0;

  src = src_desc->pkt;
  src_last = src_desc->last;

  /* Some callers use this state to detect a need for other initialization.
     Forcing state transition avoids multiple initialization. */

  if ( DIAG_SEND_STATE_START == src_desc->state )
  {
    src_desc->state++;
  }

  dest = enc->dest;
  dest_last = enc->dest_last;

  if ( dest && dest_last )
  {

    switch ( diag_non_hdlc_state )
    {

    case DIAG_STATE_START:

      *dest++ = DIAG_ASYNC_CONTROL_CHAR;
      *dest++ = DIAG_CONSUMER_API_VER_1;
      if ( dest > dest_last )
      {
        diag_non_hdlc_state = DIAG_STATE_WRITE_FIRST_BYTE_LEN;
        break;

      }
      diag_non_hdlc_state = DIAG_STATE_WRITE_FIRST_BYTE_LEN;

    case DIAG_STATE_WRITE_FIRST_BYTE_LEN:

      if ( dest_last - dest >= 1 )
      {
        /* Memorize location of length field */
        diag_non_hdlc_length1 = dest++;
        diag_non_hdlc_length2 = dest++;

        if ( dest > dest_last )
        {
          diag_non_hdlc_state = DIAG_STATE_WRITE_PACKET;
          break;

        }
        /* Had enough space - goto writing the packet*/
        diag_non_hdlc_state = DIAG_STATE_WRITE_PACKET;
        goto write_packet;

      }
      else if ( dest_last - dest == 0 )
      {
        diag_non_hdlc_length1 = dest++;
        diag_non_hdlc_state = DIAG_STATE_WRITE_SECOND_BYTE_LEN;
        break;


      }


    case DIAG_STATE_WRITE_SECOND_BYTE_LEN:

      diag_non_hdlc_length2 = dest++;
      if ( dest > dest_last )
      {
        diag_non_hdlc_state = DIAG_STATE_WRITE_PACKET;
        break;

      }
      diag_non_hdlc_state = DIAG_STATE_WRITE_PACKET;

    case DIAG_STATE_WRITE_PACKET:
      {

      write_packet:

        diag_non_hdlc_length = src_last - src + 1;

        if ( diag_non_hdlc_length > (dest_last - dest + 1) )
        {
          length_current_chunk = dest_last - dest + 1;
        }
        else
        {
          length_current_chunk = diag_non_hdlc_length;
        }
        memscpy(dest, (dest_last - dest + 1), src, length_current_chunk);
        dest += length_current_chunk;
        diag_non_hdlc_length -= length_current_chunk;
        src_desc->pkt = src + length_current_chunk;
        diag_non_hdlc_total_length += length_current_chunk;

        if ( dest > dest_last )
        {
          if ( diag_non_hdlc_length == 0 )
          {

            diag_non_hdlc_state = DIAG_STATE_END;

          }
          break;


        }
        diag_non_hdlc_state = DIAG_STATE_END;
      }
    case DIAG_STATE_END:
      {

        src_desc->state = DIAG_SEND_STATE_COMPLETE;

        if ( src_desc->terminate )
        {
          /*Go update the total length */
          *diag_non_hdlc_length1 = *((uint8 *)&diag_non_hdlc_total_length);
          *diag_non_hdlc_length2 = *((uint8 *)(&diag_non_hdlc_total_length) + 1);
          *dest++ = DIAG_ASYNC_CONTROL_CHAR;
          diag_non_hdlc_total_length = 0;
          diag_non_hdlc_state = DIAG_STATE_START;
        }
        else
        {
          diag_non_hdlc_state = DIAG_STATE_WRITE_PACKET;
        }
      }

    }
  }

  enc->dest = dest;
  enc->dest_last = dest_last;

  src_desc->last = src_last;

}

/*===========================================================================

FUNCTION DIAGBUF_CLEAR

DESCRIPTION
  Clear status bit of buffer (param *buf) for all data from
  start to start+offset index

PARAMETERS
  int start  - start index in buffer
  int offset - end index in buffer
  uint8 *buf - buffer in which status bit needs to be cleared

RETURN
 None

===========================================================================*/
static inline void diagbuf_clear( int start, int offset, uint8 *buf )
{
  if ( buf )
  {
    int k = 0;
    diagbuf_header_type *ptr;
    int i = DIAGBUF_ALIGNED_BYTE_4(start);
    int j = DIAGBUF_NEXT_ALIGNED_BYTE_4((start + offset));

    for ( k = i; k < j; k += 4 )
    {
      ptr = ((diagbuf_header_type *)&buf[k]);
      ptr->status = DIAGBUF_UNCOMMIT_S;
    }
  }
} /* diagbuf_clear */


#ifdef DIAG_UIMAGE_MODE

/*===========================================================================

FUNCTION DIAGBUF_DRAIN_MICRO_DIAGBUFFER

DESCRIPTION
  This function drains the micro diag buffer to the comm layer.
  Escaping, etc is performed in this routine.

  This function may only be called by the diag task.

  This is considered a background process for the DIAG task. Periodically,
  this routine checks the DIAG task sigs against 'mask'.  If any bits match,
  the routine exits promptly.

RETURN VALUE
  micro_diagbuffer_drain_result

===========================================================================*/
extern npa_client_handle g_micro_diagbuffer_client;

micro_diagbuffer_drain_result diagbuf_drain_micro_diagbuffer( osal_sigs_t mask )
{
  micro_diagbuffer_drain_result result = MICRO_DIAGBUFFER_DRAIN_SUCCESS;

  if ( NULL != g_micro_diagbuffer_client )
  {
    /* vote against entering uImage mode, as we want to drain completely from the buffer */
    npa_issue_required_request(g_micro_diagbuffer_client, USLEEP_CORE_DRIVER_RESTRICT_ISLAND);

    result = micro_diagbuffer_drain(mask);
	if(result != MICRO_DIAGBUFFER_DRAIN_SUCCESS && result != MICRO_DIAGBUFFER_DRAIN_BUFFER_EMPTY)
	{
    ULogFront_RealTimePrintf(diag_err_log_handle,1,"diagbuf_drain_micro_diagbuffer : Drain failed with error %d",result);
	}

    /* draining is complete or buffer was empty, vote for uImage mode */
    npa_issue_required_request(g_micro_diagbuffer_client, USLEEP_CORE_DRIVER_ALLOW_ISLAND);
  }
  return (result);
}
#endif /* DIAG_UIMAGE_MODE */
/*===========================================================================

FUNCTION DIAGBUF_DRAIN

DESCRIPTION
  This function calls micro_diagbuffer_drain() and diagbuf_drain_diag().
  The two functions drain their respective buffers to the communication layer.

  This function may only be called by the diag task.

  This is considered a background process for the DIAG task. Routines called
  from this function periodically checks the DIAG task sigs against 'mask'.
  If any bits match the routine exit promptly.

RETURN VALUE
  READY if more data can be sent, or diagbuf_drain_diag was not called.
  NOT_READY if unable to proceed (uncommitted packet, flow control)
  EMPTY nothing to do

===========================================================================*/
diagbuf_status_enum_type diagbuf_drain( osal_sigs_t mask )
{
  diagbuf_status_enum_type      status = DIAGBUF_READY_S;

#ifdef DIAG_UIMAGE_MODE
  micro_diagbuffer_drain_result result = MICRO_DIAGBUFFER_DRAIN_SUCCESS;

  /* Vinay - ToDo - looks like SMDL_WRITE is used for multiple things.
     Need to discuss.
  */
  result = diagbuf_drain_micro_diagbuffer(DIAG_RX_SIG |
                                          DIAG_RX_RSP_SIG |
                                          DIAG_COMM_SIO_TX_FLUSH_SIG);

  /* Since micro_diagbuffer_drain exits on subset of signals diagbuf_drain_diag
     exits there is no reason to call diagbuf_drain_diag() if micro_diagbuffer_drain
     was exited due to a signal set on Diag task.
  */
  if ( result != MICRO_DIAGBUFFER_DRAIN_EXIT_DUE_TO_SIGNAL_ON_DIAG )
  {
#endif /* DIAG_UIMAGE_MODE*/

     status = diagbuf_drain_diag(mask);

#ifdef DIAG_UIMAGE_MODE
  }
#endif /* DIAG_UIMAGE_MODE */

  return(status);
}
/*===========================================================================

FUNCTION DIAGBUF_DRAIN_DIAG

DESCRIPTION
  This function drains the diag allocation buffer to the comm layer.
  Escaping, etc is performed in this routine.

  This function may only be called by the diag task.  It is not re-entrant!

  This is considered a background process for the DIAG task.  Periodically,
  this routine checks the DIAG task sigs against 'mask'.  If any bits match,
  the routine exits promptly.

===========================================================================*/
/*=========================================================================*/
diagbuf_status_enum_type diagbuf_drain_diag( osal_sigs_t mask )
{
  osal_sigs_t sigs;       /* Used to check if loop should be exited.      */
  static byte stream_id = 0;
  byte *start_pos = NULL;
  int return_val = 0;

  diagbuf_header_type pkt_hdr;    /* Store the header in local variable     */
  diagbuf_status_enum_type status = DIAGBUF_READY_S;

  static uint8 pad;               /* Stores the pad                         */
  static int iHead;
  static int iTail;
  static int iTail_temp;
  static diagbuf_mpd_type *current = NULL; /* It is used to keep track of which buffer
                                         to drain from.  If it is NULL, should
                                         drain from the guest OS buf.*/
  static diag_send_desc_type send = { NULL, NULL, DIAG_SEND_STATE_COMPLETE, TRUE };
  static diagbuf_outbound_pkt_handler_type fp = NULL; /* Customer packet processor */


  uint8 *current_buf = NULL;  /* stores pointer to a buffer that is to be drained. */
  int current_tail = 0;       /* stores tail of the bufffer that is to be drained. */
  int current_head = 0;       /* stores head of the buffer that is to be drained.  */
  int current_tail_index = 0; /* stores index of the buffer that is to be drained. */
  int current_buff_size = 0;  /* stores size of the buffer that is to be drained.  */
  uint32 bytes_sent_from_buff = 0;      /* Tracks number of bytes sent from buffer
                                           pointed by current_buf, used for drain
                                           threshould check
                                        */
  boolean advance_to_next_buff = FALSE; /* Set to true when bytes_sent_from_buff >
                                           buffer's drain threshold
                                        */
  boolean data_available_in_buff = TRUE;/* If buffer is not empty this variable
                                            will be TRUE.
                                        */
  static boolean chained_extended_pkt = FALSE;/* Indicate if the current pkt which is being processed
                                                 has a chained pkt*/

  iHead = diagbuf_head;
  iTail = diagbuf_tail;

  /* If this fails, it typically means someone overwrote a diag buffer */
  ASSERT(diagbuf_tail >= 0 && diagbuf_tail < 2 * DIAGBUF_SIZE);

  ASSERT(osal_thread_self() == &diag_tcb);

  /* Sanity check */
  if ( DIAGBUF_CHECK_CORRUPTION_IN_BUFFERS() )
  {
    ERR_FATAL("Ring buffer corrupt! iHead = %d, iTail = %d", iHead, iTail, 0);
  }

  if ( !DIAG_CHECK_MASK((int)diag_cx_state, DIAG_CX_ANY_S) )
  {
    status = DIAGBUF_NOT_READY_S;
  }
  else if ( DIAGBUF_ARE_BUFFERS_EMPTY() )
  {
    status = DIAGBUF_EMPTY_S;
  }
  else
  {
    /*
      The drain will work it the following ways: Each user space
      buffer will now have to be drained in addition to the guest OS
      process space buffer.  Draining will take place in a round
      robin fashion.  Either a maximum number of bytes will be
      drained from a buffer, or it will be drained until there is no
      more data in the buffer. At this point, the drain will move
      onto the next buffer and continue. This loop will exit when
      either all data has been drained and there is no more
      available data, the comm layer is no longer running, or flow
      control has been turned on, or signal has been set on diag
      task. When either of these conditions occur, this thread will
      give up will exit.
    */
    while ( !DIAGBUF_ARE_BUFFERS_EMPTY() &&
            diagcomm_status() &&
            ((diagcomm_io_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].allow_flow == TRUE) ||
            (send.state == DIAG_SEND_STATE_COMPLETE && chained_extended_pkt == TRUE)))
    {
      status = DIAGBUF_READY_S;
      if ( current == NULL )
      {
        current_buf = diagbuf_buf;
        current_buff_size = DIAGBUF_SIZE;
        current_head = diagbuf_head;
        current_tail = diagbuf_tail;
        iTail_temp = current_tail;
      }
      else if ( (current!= NULL) && (current->diagbuf_mpd_mode==DIAG_PD_BUF_MODE_STREAMING))
      {
        current_buf = current->diagbuf_mpd_buf;
        current_buff_size = current->diagbuf_size;
        current_tail = current->diagbuf_mpd_tail;
        current_head = current->diagbuf_mpd_head;
        iTail_temp = current_tail;
      }
      else
      {
        /*If current MPD buffer is in circular mode:
        1. set current to next MPD buffer.
        2. Set status to avoid entring the reading loop and then skip to main loop*/
      current= DIAGBUF_FIND_NEXT_BUFFER(current);
        status = DIAGBUF_NOT_READY_S;
      }
      /* Start to process the next pkt if we have something to drain
         from current buffer and it is not follow controlled or we
         have drained the first pkt of the chained pkt succesfully */
      while ( (status == DIAGBUF_READY_S) &&
        (diagcomm_io_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].allow_flow == TRUE))
      {
        data_available_in_buff = TRUE;
        /* Get the tail of the selected buffer */
        current_tail_index = DIAGBUF_CALC_IDX(current_tail, current_buff_size);
        ASSERT(current_tail_index < current_buff_size);

        /* Check to see if the current buffer has the empty condition */
        if ( DIAGBUF_EMPTY(current_head, current_tail) )
        {
            data_available_in_buff = FALSE;
            advance_to_next_buff = TRUE;
            status = DIAGBUF_EMPTY_S;
        }
          else if ( current && DIAGBUF_IS_PD_KILLED(current) )
        {
            osal_lock_mutex(&diagmpd_buf_list_state_mutex);
            /* Making PD empty allows Drain to move to next PD buffer in the linked list */
            DIAGBUF_ARTIFICALY_MAKE_PD_EMPTY(current);
            /* The PD is going down, mark it to not in use to allow clean up. */
            current->is_in_use = FALSE; 
            osal_unlock_mutex(&diagmpd_buf_list_state_mutex);

            data_available_in_buff = FALSE;
            advance_to_next_buff = TRUE;
        }
        else if ( status == DIAGBUF_READY_S )
        {
            /* Prep the start of a new packet */
            pkt_hdr = *((diagbuf_header_type *)&current_buf[current_tail_index]);

            if ( pkt_hdr.status == DIAGBUF_COMMIT_S )
            {
              /* Go past the header field */
              DIAGBUF_MOVE(current_tail_index, (int)sizeof(diagbuf_header_type), current_buff_size);

              ASSERT(current_tail_index < current_buff_size);

              /* Initialize the send descriptor */
              send.pkt = (void *)&current_buf[current_tail_index];
              send.last = (void *)((uint32)send.pkt + pkt_hdr.length - 1);

              if (((diagbuf_header_type *)((diagbuf_header_type *)(send.pkt) - 1))->chain == DIAGBUF_FIRST_PKT_CHAIN)
              {
                chained_extended_pkt = TRUE;//if the current pkt is the first packet of a chained packet, we set it to TRUE*
              }
              else
              {
                chained_extended_pkt = FALSE;
              }
              /*
              Since the drain will drain a set number of bytes before moving onto the next
              buffer, need to keep track of how many bytes have been sent.
              */
              bytes_sent_from_buff += (int)send.last - (int)send.pkt;
              if ( current == NULL )
              {
                if ( bytes_sent_from_buff >= DIAG_CORE_PD_DRAIN_THRESHOLD )
                {
                  /* Set the flag to indicate it's time to move onto the next buffer. */
                  advance_to_next_buff = TRUE;
                }
              }
              else
              {
                if ( bytes_sent_from_buff >= current->diagbuf_mpd_drain_threshold )
                {
                  /* Set the flag to indicate it's time to move onto the next buffer. */
                  advance_to_next_buff = TRUE;
                }
              }
              send.state = DIAG_SEND_STATE_START;
              send.terminate = TRUE;

              /* Update the diag ID in descriptor accordingly */
              if ( current == NULL )
              {
                send.diag_id= diag_get_diagID();
              }
              else
              {
                send.diag_id = current->diagID;
              }

              stream_id = diagbuf_read_stream_id(send.pkt);
              pad = pkt_hdr.pad_len;

              fp = diagbuf_get_custom_pkt_handler(current_buf[current_tail_index]);

              /* Adjust for size of overrun pattern field */
              pad += sizeof(uint16);
              iTail_temp = current_tail;

              if ( DIAGBUF_USED(current_head, iTail_temp, current_buff_size) > current_buff_size )
              {
                ERR_FATAL("Ring buffer corrupt! - 1", 0, 0, 0);
              }
              ASSERT(current_tail < 2 * current_buff_size);
            } // end if status == COMMIT_S
            else if ( pkt_hdr.status == DIAGBUF_UNCOMMIT_S || pkt_hdr.status == DIAGBUF_UNUSED_S )
            {
              /* If panic mode, treat an uncommitted packet as a hole. */
              status = DIAGBUF_NOT_READY_S;
            } // End of status == UNCOMMIT_S
            else if ( pkt_hdr.status == DIAGBUF_WRAP_S )
            {
              iTail_temp = current_tail;
              current_tail_index = DIAGBUF_CALC_IDX(current_tail, current_buff_size);
              diagbuf_clear(current_tail_index, 1, current_buf);
              DIAGBUF_WRAP(iTail_temp, current_buff_size);
              current_tail = iTail_temp;
              if ( current == NULL )
              {
                diagbuf_tail = current_tail;
              }
              else
              {
                current->diagbuf_mpd_tail = current_tail;
              }
              ASSERT((current_tail == 0) || (current_tail == current_buff_size));
              continue;
            } //end of status == WRAP_S
            else if ( pkt_hdr.status == DIAGBUF_HOLE_S )
            {
              iTail_temp = current_tail;
              current_tail_index = DIAGBUF_CALC_IDX(iTail_temp, current_buff_size);
              diagbuf_clear(current_tail_index, (int)(pkt_hdr.length + sizeof(diagbuf_header_type)), current_buf);

              DIAGBUF_MOVE(iTail_temp, (int)(pkt_hdr.length + sizeof(diagbuf_header_type)), current_buff_size);
              if ( DIAGBUF_USED(current_head, iTail_temp, current_buff_size) > current_buff_size )
              {
                ERR_FATAL("Ring buffer corrupt! - 2 current_head = %d", current_head, 0, 0);
              }
              current_tail = iTail_temp;
              if ( current == NULL )
              {
                diagbuf_tail = current_tail;
              }
              else
              {
                current->diagbuf_mpd_tail = current_tail;
              }
              ASSERT(current_tail < 2 * current_buff_size);
              continue;
            } // end of status == HOLE_S
          } // else i.e. if ( status == DIAGBUF_READY_S )

        if ( status == DIAGBUF_READY_S && data_available_in_buff == TRUE )
        {
          start_pos = (byte *)send.pkt;

          if ( fp )
          {
            send.state = DIAG_SEND_STATE_START;

           /*If the MSB is set in stream id, then the pkt was generated
             in UserPD and can be sent out without any processing*/
            if ( stream_id & DIAGBUF_OUTBOUND_PKT_HANDLER_BIT_MASK )
              (void)diagbuf_send_pkt(&send, stream_id);
            else
              fp(&send, stream_id);
          }
          else
          {
            (void)diagbuf_send_pkt(&send, stream_id);
          }

          if ( DIAGBUF_USED(current_head, current_tail, current_buff_size) > current_buff_size )
          {
            ERR_FATAL("Ring buffer corrupt! - 3 current_head = %d", current_head, 0, 0);
          }
          /* Move the tail. */
          if ( send.state == DIAG_SEND_STATE_COMPLETE )
          {
            current_tail_index = DIAGBUF_CALC_IDX(current_tail, current_buff_size);
            diagbuf_clear(current_tail_index, (int)sizeof(diagbuf_header_type), current_buf);
            DIAGBUF_MOVE(current_tail_index, (int)sizeof(diagbuf_header_type), current_buff_size);

            DIAGBUF_MOVE(iTail_temp, (int)sizeof(diagbuf_header_type), current_buff_size);

          diagbuf_clear(current_tail_index, (int)((uint32)send.pkt - (uint32)start_pos), current_buf);
            DIAGBUF_MOVE(current_tail_index, (int)((uint32)send.pkt - (uint32)start_pos), current_buff_size);

            DIAGBUF_MOVE(iTail_temp, (int)((uint32)send.pkt - (uint32)start_pos), current_buff_size);

          if ( DIAGBUF_USED(current_head, iTail_temp, current_buff_size) > current_buff_size )
          {
            ERR_FATAL("Ring buffer corrupt! - 4 current_head = %d", current_head, 0, 0);
          }
          current_tail = iTail_temp;

          if ( current == NULL )
          {
            diagbuf_tail = current_tail;
          }
          else
          {
              current->diagbuf_mpd_tail = current_tail;
          }

          ASSERT(current_tail < 2 * current_buff_size);
          ASSERT(current_tail_index < current_buff_size);

            fp = NULL;

            /* Packet has been exhausted.  Adjust for pad. */
            diagbuf_clear(current_tail_index, pad, current_buf);
            DIAGBUF_MOVE(current_tail_index, pad, current_buff_size);

            iTail_temp = current_tail;
            DIAGBUF_MOVE(iTail_temp, pad, current_buff_size);
            current_tail = iTail_temp;
            if ( current == NULL )
            {
              diagbuf_tail = current_tail;
            }
            else
            {
             current->diagbuf_mpd_tail = current_tail;
            }

            ASSERT(current_tail < 2 * current_buff_size);
            ASSERT(current_tail_index < current_buff_size);

            if ( current && DIAGBUF_IS_PD_KILLED(current) )
            {
              osal_lock_mutex(&diagmpd_buf_list_state_mutex);
              /* Making PD empty allows Drain to move to next PD buffer in the linked list */
              DIAGBUF_ARTIFICALY_MAKE_PD_EMPTY(current); 
              /* The PD is going down, mark it to not in use to allow clean up. */
              current->is_in_use = FALSE; 
              osal_unlock_mutex(&diagmpd_buf_list_state_mutex);
            }
          } /* end of send.state == COMPLETE */

          /* Sanity check */
          if ( DIAGBUF_USED(current_head, current_tail, current_buff_size) > current_buff_size )
          {
            /* Panic mode will reenter this function.  Must put system in a
               recoverable state. */
            // diagbuf_head = diagbuf_tail = 0;
            ERR_FATAL("Ring buffer corrupt! - 5 current_head = %d, current_tail = %d", current_head, current_tail, 0);

            /* Panic mode will swap in the DIAG task - must be able to return
               to normal task operations, so exit the loop. */
            /*lint -save -e{527} */
            break;
            /*lint -restore */
          }
          if ( DIAGBUF_EMPTY(current_head, current_tail) )
          {
            advance_to_next_buff = TRUE;
            status = DIAGBUF_EMPTY_S;
          }
        } /* end of if ( status == DIAGBUF_READY_S && data_available_in_buff == TRUE )*/
        
        /* Find the buffer to check for data and save the state of the buffer */
        if ( current == NULL )
        {
          diagbuf_tail = current_tail;
        }
        else
        {
          current->diagbuf_mpd_tail = current_tail;
            }

        if(diagbuf_tail % 4 != 0)
          ERR_FATAL("diagbuf_tail 0x%x is not 4 multiple", diagbuf_tail, 0, 0);

        /* Advance to the next buffer if */
        if ( advance_to_next_buff == TRUE && chained_extended_pkt == FALSE)
        {
          /*
             Move to the next buffer -> If NULL is return it means we
             reached the end of the link list of UserPD diagbufs hence now
             it is time to drain from GuestOS diagbuf.
             So drain sequenece is as follows:-
             GuestOS->UserPD1->UserPD2->....->GuestOS
             since GuestOS is not in this link list, as it need not to be,
             a NULL return from find_next_buffer() makes logic fall in
             place.
          */
          osal_lock_mutex(&diagmpd_buf_list_state_mutex);
          if (current != NULL)
          {
            current->is_in_use = FALSE;
              }
          current = DIAGBUF_FIND_NEXT_BUFFER(current);
          if (current != NULL)
          {
            current->is_in_use = TRUE;
              }
          /* Reset the byte counter for the buffer */
          bytes_sent_from_buff = 0;
          osal_unlock_mutex(&diagmpd_buf_list_state_mutex);
          /* Reset the flag to switch to the next buffer */
          advance_to_next_buff = FALSE;
          break;
        }

        /* Get signals */
        return_val = osal_get_sigs(&diag_tcb, &sigs);
        ASSERT(OSAL_SUCCESS == return_val);

        /* RPT_TIMER_SIG?  Kick the dog */
        if ( sigs & DIAG_RPT_TIMER_SIG )
        {
          diag_kick_watchdog();
        }
        /* If we are handling a pkt which has a chained pkt,
           do not check signals. We should drain the second
           pkt before checking signals */
        if (chained_extended_pkt == FALSE)
        {

            /* mask sig or diag is blocking?  Break out.  */
            if ( sigs & mask )
            {
                break; /* Exit loop.  Something more important has happened! */
            }
        }
      } /* End of while() */

      /* Get signals */
      return_val = osal_get_sigs(&diag_tcb, &sigs);
      ASSERT(OSAL_SUCCESS == return_val);

      /* RPT_TIMER_SIG?  Kick the dog */
      if ( sigs & DIAG_RPT_TIMER_SIG )
          {
        diag_kick_watchdog();
      }
      /* mask sig or diag is blocking?  Break out.  */
      /*if we are handling a pkt which has a chained pkt, do not check signals
         we should drain the second pkt before checking signals*/
      if (chained_extended_pkt ==FALSE)
      {

        /* mask sig or diag is blocking?  Break out.  */
        if ( sigs & mask )
        {
          break; /* Exit loop.  Something more important has happened! */
        }
      }
    } /* End of outer While loop */
  } /* end of ELSE */
  
  if ( DIAGBUF_ARE_BUFFERS_EMPTY() )
  {
    if(current!=NULL)
      {
        osal_lock_mutex(&diagmpd_buf_list_state_mutex);
        /*Clear this bit if all the buffers are empty else diag QDI cleanup gets stuck */
  	    current->is_in_use = FALSE; 
        /* Make current NUll to avoid processing MPD buffer again since buf_in_use bit is cleared.
         Draining logic will not be affected but if cleanup starts while using MPD buffer it will prevent crash. */
        current=NULL;
      osal_unlock_mutex(&diagmpd_buf_list_state_mutex);
      }
    }

  if(diagcomm_io_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].allow_flow == TRUE)
  {
	if ( !DIAGBUF_ARE_BUFFERS_EMPTY() && diagcomm_status() )
  {
    /* Interrupted for some reason - signal DIAG so it will come back.
       There is no reason to terminate diagbuf_send_pkt() at this time,
       since we'll be right back! */

    diag_tx_notify();
  }
  else
  {
    if ( !DIAGBUF_ARE_BUFFERS_EMPTY() && DIAG_CHECK_MASK(diag_cx_state, DIAG_CX_COMM_S) )
    {
      diag_set_drain_timer();
    }
    /* DIAG is not poised to continue TX immediately, so terminate the existing
       outbound comm buffer unless we are not expecting traffic in a timely manner. */
	  if(diagcomm_status() && diag_drain_timer_enabled())
      (void)diagbuf_send_pkt(NULL, DIAG_STREAM_1 | DIAG_STREAM_2);
  }
  }

  return (status);
} /* diagbuf_drain_diag */


/*===========================================================================

FUNCTION DIAGBUF_REGISTER_OUTBOUND_PKT_HANDLER

DESCRIPTION
  This function registers a function pointer to handle the sending of a
  packet.  diagbuf_drain() sends packets in raw format.  If processing/
  formatting needs to occur in DIAG task context prior to sending the
  packet, one can register a function pointer with the command code, which
  will be called when diagbuf_drain finds a packet with that command code.

  This system requires that diagbuf_send_pkt is used to send the final
  packet to the comm layer.  This routine maintains a state, which requires
  that all outbound packets be sent via that function.

===========================================================================*/
/*=========================================================================*/
boolean diagbuf_register_outbound_pkt_handler(
   uint8 cmd_code,
   diagbuf_outbound_pkt_handler_type fp
   )
{
  unsigned int i;
  boolean found = FALSE;

  if ( fp )
  {
    for ( i = 0; !found && i < DIAGBUF_PKT_HANDLER_TBL_CNT; i++ )
    {
      if ( diagbuf_outbound_pkt_handler_tbl[i].cmd_code == cmd_code ||
           diagbuf_outbound_pkt_handler_tbl[i].cmd_code == 0xFF )
      {
        found = TRUE;

        diagbuf_outbound_pkt_handler_tbl[i].cmd_code = cmd_code;
        diagbuf_outbound_pkt_handler_tbl[i].fp = fp;
      }
    }
  }

  return (found);
}



/*=========================================================================*/
static diagbuf_outbound_pkt_handler_type
diagbuf_get_custom_pkt_handler( uint8 cmd_code )
{
  unsigned int i;
  diagbuf_outbound_pkt_handler_type fp = NULL;

  for ( i = 0; !fp && i < DIAGBUF_PKT_HANDLER_TBL_CNT; i++ )
  {
    if ( diagbuf_outbound_pkt_handler_tbl[i].cmd_code == cmd_code )
    {
      fp = diagbuf_outbound_pkt_handler_tbl[i].fp;
    }
  }

  return (fp);
}

/*===========================================================================

FUNCTION DIAGBUF_FLUSH

DESCRIPTION
  This function wrapper over diagbuf_flush_timeout which configures all streaming data items off.
  No more allocations will occur until these settings are changed.

  Diag will not stop collecting data immeidiately.  The data currently
  allocated will still be flushed.

===========================================================================*/
void diagbuf_flush( void )
{
  if ( osal_thread_self() == &diag_tcb )
  {
    diagbuf_flush_timeout(0);
  }
  else
  {
    /* This guarantees that flush will always happen in the diag context. */
    diag_set_internal_sigs(DIAG_INT_FLUSH_BUF_SIG);
  }
} /* diagbuf_flush */

/*===========================================================================

FUNCTION DIAGBUF_FLUSH_TIMEOUT

DESCRIPTION
  This function configures all streaming data items off.  No more allocations
  will occur until these settings are changed.It performs the same operation
  as diagbuf_flush only with optional timeout value.

  Diag will not stop collecting data immeidiately.  The data currently
  allocated will still be flushed.

  diag will block after flushing the data for specified timeout duration or
  time taken by SIO to flush the data(which ever is smaller).

  diag_flush_timeout flag will be set to TRUE if diag times out on SIO callback

@param[in] timeoutVal    Specifies the timeout duration. If specified as 0,
timeout will not occur.

===========================================================================*/
void diagbuf_flush_timeout( unsigned int timeoutVal )
{
  osal_sigs_t return_sigs;
  dword sigs;
  int return_val = 0;
  /*-----------------------------------------------
    Turn off messages, logs, and event reporting.
  -----------------------------------------------*/
  msg_set_all_rt_masks_adv(0, DIAG_STREAM_ALL, DIAG_PRESET_MASK_ALL);

  /* Listeners must still be serviced, but logs and events for the tool must
     at least stop being sent to the comm layer. */
  log_flush();

  /* In case there is no connection and the buffer is non-empty. */
  DIAG_SET_MASK(diag_cx_state, DIAG_CX_FLUSH_S);

  /* Go ahead and drain the ring buffer
  */
  while ( diagbuf_drain(0) != DIAGBUF_EMPTY_S )
  {
    sigs = diag_wait(DIAG_TX_SIG | DIAG_DRAIN_TIMER_SIG);
    if ( sigs & DIAG_TX_SIG )
    {
      diag_tx_pending = FALSE;
      return_val = osal_reset_sigs(&diag_tcb, DIAG_TX_SIG, &return_sigs);
      ASSERT(OSAL_SUCCESS == return_val);
    }
    if ( sigs & DIAG_DRAIN_TIMER_SIG )
    {
      diag_drain_timer_pending = FALSE;
      return_val = osal_reset_sigs(&diag_tcb, DIAG_DRAIN_TIMER_SIG, &return_sigs);
      ASSERT(OSAL_SUCCESS == return_val);
    }
  }

  if ( diagcomm_cmd_status(DIAGCOMM_PORT_1) )
     diagpkt_rsp_send();

  /* After flush, the comm connection state is considered disconnected. */
  DIAG_CLR_MASK(diag_cx_state, DIAG_CX_FLUSH_S | DIAG_CX_COMM_S);

  /* Flush the TX stream in the comm layer. */
  diagcomm_flush_tx(NULL, timeoutVal);
}


/*===========================================================================

FUNCTION DIAGBUF_TX_SLEEP_PARAMETERS_SET

DESCRIPTION
This sets the value of diagbuf_tx_sleep_threshold and diagbuf_tx_sleep_time.
These parameters are initialized, after reading NV(AMSS)/Registry(WM).

===========================================================================*/
void diagbuf_tx_sleep_parameters_set( void )
{
#if defined (DIAG_QDSP6_APPS_PROC)
  return;
#else
#if defined (FEATURE_DIAG_NV) /* Non-WM, read from NV */
  uint64 sleep_nv_amt = 0;
  uint32 sleep_time_nv_amt = 0;
  nv_stat_enum_type sleep_nvstatus = NV_FAIL_S;

  sleep_nvstatus = diag_nv_read(NV_DIAG_DIAGBUF_TX_SLEEP_THRESHOLD_EXT_I, &sleep_nv_amt);
  if ( sleep_nvstatus == NV_DONE_S )
  {
    diagbuf_tx_sleep_threshold = (uint32)sleep_nv_amt;
  }

  sleep_nvstatus = diag_nv_read(NV_DIAG_DIAGBUF_TX_SLEEP_TIME_NV_I, &sleep_time_nv_amt);
  if ( sleep_nvstatus == NV_DONE_S )
  {
    diagbuf_tx_sleep_time = sleep_time_nv_amt;
  }

#endif
#endif /* DIAG_QDSP6_APPS_PROC */
} /* diagbuf_tx_sleep_parameters_set */

/*===========================================================================
FUNCTION diagbuf_ctrl_tx_sleep_parameters

DESCRIPTION
Control the value of diagbuf_tx_sleep_time and diagbuf_tx_sleep_threshold.
Currently used to change modem operation for on-device logging low-power mode
===========================================================================*/
void diagbuf_ctrl_tx_sleep_parameters( unsigned int threshold_val, unsigned int sleep_val )
{
  diagbuf_tx_sleep_time = sleep_val;
  diagbuf_tx_sleep_threshold = threshold_val;
}

/*===========================================================================
FUNCTION diagbuf_ctrl_commit_threshold
DESCRIPTION
Control diagbuf_commit_threshold
===========================================================================*/
void diagbuf_ctrl_commit_threshold( unsigned int val )
{
  diagbuf_commit_threshold = val;
}

/*===========================================================================

FUNCTION diagbuf_mpd_ctrl_commit_threshold

DESCRIPTION
    This function change the commit threshold for userpd's.

PARAMETERS
    VAL       The percentage of diagbuf to be set for commit threshold.
    diagID    diag_id of the entity (Peripheral/PD).

RETURN
    None

===========================================================================*/
void diagbuf_mpd_ctrl_commit_threshold( unsigned int val, uint8 diagID )
{
  /* Change the commit threshold for all the UserPD's */
  diagbuf_mpd_type *q_cur_ptr = NULL;
  
  q_cur_ptr = (diagbuf_mpd_type *)q_check(&diagbuf_mpd_q);

   while (q_cur_ptr != NULL)
   {
    if(diagID == q_cur_ptr->diagID )
      {
      q_cur_ptr->diagbuf_mpd_commit_threshold = (q_cur_ptr->diagbuf_size * val) / 100;
        break;
      }
    q_cur_ptr = (diagbuf_mpd_type *)q_next(&diagbuf_mpd_q, &q_cur_ptr->link);
   }
}

/*===========================================================================
FUNCTION diagbuf_mpd_add_pd

DESCRIPTION
  Adds the diagbuf_mpd_type pointer to the linked list that keeps track of
the user space diagbufs.

PARAMETERS
 diagbuf_mpd_type *buf - the UserPD diagbuf pointer.

RETURN VALUE
 None

===========================================================================*/
void diagbuf_mpd_add_pd( diagbuf_mpd_type *buf )
{
  if ( buf )
  {
    q_put(&diagbuf_mpd_q, q_link(buf, &(buf->link)));
  }
}

/*===========================================================================
FUNCTION diagbuf_mpd_remove_pd

DESCRIPTION
  Removes the diagbuf_mpd_type pointer from the linked list that keeps track of
the user space diagbufs.  This function will not free memory associated with
the diagbuf.

PARAMETERS
 diagbuf_mpd_type *buf - the UserPD diagbuf pointer.

RETURN
 None

===========================================================================*/

void diagbuf_mpd_remove_pd( diagbuf_mpd_type *buf ) /*<---VinDbg This needs to be called
                                                          when a process is killed.
                                                          That requires discussion. -*/
{
  if ( buf )
    {
    q_delete(&diagbuf_mpd_q, &(buf->link));
    }
}
