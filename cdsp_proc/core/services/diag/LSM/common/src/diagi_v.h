#ifndef DIAGI_V_H
#define DIAGI_V_H
/*==========================================================================

                 Diagnostic Subsystem Internal Header File

Description
  Shared declarations and prototypes internal to the Diagnostic subsystem.

Copyright (c) 1990-2017 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/


/*===========================================================================

                            Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/services/diag/LSM/common/src/diagi_v.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/21/17   ph      Tag DIAG ID header to all the diag packets sent out.
11/03/16   sa      Implemented seperate queue for UserPD events and enabled buffering
06/05/17   nk      Added debug support for uimage in root PD
05/30/17   gn      Changes to support command deregistration
04/09/17   sp      Added support for extended logs
03/03/17   sp      Removed packed void * parameters 
10/24/16   gn      Changes to support common PW for ADSP, CDSP and SLPI
05/12/16   ph      SMD references removal and code clean up.
05/10/16   nk      Separate send buffers for CMD and DATA
09/08/15   ph      Consolidated Buffering mode fixes
08/18/15   as      Resolved Diag signal conflicts
01/21/15   sr      Added changes to remove DSM items
06/18/15   sa      Added support for time sync feature.
01/16/15   sa      Updated the name for the commands injected in Diag Cmd Queue.
12/17/14   ph      Removed diag_strlcpy() function.
09/01/14   ph      Added mutex around diag_internal_sigs to resolve concurrency
                   issues with feature mask update missing.
07/06/14   ph      Added support for Diag over STM
04/10/14   sr      Support for peripheral buffering mode  
04/01/14   rs      Added support for build mask centralization. 
02/18/14   rh      Added internal flush signal.
11/28/13   ph      Fixed the bug where diag_sleep_state won't be updated when diag
                   is switched to NRT mode.
11/22/13   sr      Removed PACK from pointer references 
09/13/13   ph      Updated drain logic for TX packets of size greater than 4k
                   to send the current dsm item and then start the chain.
07/24/13   sr      Changed SMDL_READ_SIG to internal signal and reduced the 
                   priority over CTRl_MSG_SIG
05/11/13   is      Move drain timer from client conext to diag context
04/22/13   ph      Moved listeners allocation to system heap.
04/09/13   sa      New internal signal to wake up Diag for idle processing
01/31/13   rh      Removed DIAG_TX_WMQ_SIG 
02/11/13   sr      Mainling Diag central routing  
01/03/13   sg      Command Req/Response Channel separation 
12/13/12   rs      Added timeout mechanism to unblock diag in diagbuf_flush. 
11/15/12   is      Support for preset masks
11/15/12   is      Remove support for DIAG_EXT_LOGMASK_F
10/12/12   rh      Added definition for diag_drain_timer_enabled() 
08/24/12   ra      Support for querying Command Registration Tables 
07/06/12   hvm     Added support for t-put improvements with SMDL
07/05/12   sg      Changes to bring up diag Multi PD
05/03/12   rs      Moved delayed response from diag buffer to AMSS heap. 
10/07/11   is      Support sending log mask per equipment id
09/28/11   is      Support dynamic sleep voting
09/19/11   is      Optimized sending F3 "set mask" requests from Master Diag
09/15/11   is      Modifed "set mask" via CTRL msgs fot EA support
09/12/11   is      Master Diag forwards "set mask" requests via CTRL msgs
08/08/11   wjg     Added signal to bring smdl_read into DIAG context 
04/28/11   is      Update featurizations for central routing
04/27/11   is      Resolve compiler warnings
04/18/11   is      Ensure CTRL msg sizes are constant
03/04/11   is      Move diagpkt_fwd_cmd_reg() prototype here
03/01/11   is      Add function prototype for diag_strlcpy()
12/14/10   mad     Added prototype for diagpkt_ctrl_get_msg_type() 
09/28/10   mad     Added diag_control_sleep_voting and diag_ctrl_drain_timer_len
09/14/10   is      New control channel and central routing features
08/10/10   sg      Added a new signal DIAG_COMM_SIO_TX_FLUSH_SIG that is set when 
                   the smd port is closed and callback  is triggered
06/20/10   sg      Added boolean to check whether diag task is initialized 
06/07/10   is      Removed unused DIAG_TEST_CMD_SIG
02/11/10   JV      Eliminating the event_pending timer and event_pending_q
01/20/10   JV      Added a new signal that when set, drains events
12/10/09   sg      Introduced DIAG_RX_RSP_SIG to handle rx traffic responses
09/16/09   sg      Removed the code under FEATURE_DIAG_SPC_TIMEOUT as it is no longer used.
07/10/09   as      Decoupled code under FEATURE_DIAG_COUPLED_ARCHITECTURE
05/12/09   JV      Introduced the OS abstraction layer for rex.
05/07/09   vk      moved declerations into diagi.h that were being referred from 
                   there
12/12/08   vg      CMI creating diagi.h diagi_v.h
10/03/08   vg      Updated code to use PACK() vs. PACKED
11/21/06   as      Moved DIAG internal features from diagi.h to diag.h
08/28/06   as      Added win mobile featurization support.
03/28/06   pc      Changed DIAG_TX_APP_SIG from 0x00800000 to 0x00080000.
10/19/05   as      Modified diag_searchlist_type to make it 4-byte aligned.
07/05/05   sl      Added support for SPC_TIMEOUT to double the timeout value
                   on consecutive incorrect SPCs.
06/16/05   as      New signal for DM to communicate completion.
05/17/05   as      Added new signal for Application processor data.
10/24/01   jal     New signal for SIO to indicate the UART is flushed.
                   New signal for SIO to indicate port closure is complete.
08/20/01   jal     Supported more Diag packets.  Added NV cache typedef,
                   prototypes for diag_set_sp_state(), downloader status.
06/27/01   lad     Various non-functional changes to facilitate update of
                   log services.
04/06/01   lad     Added definitions of DIAG_TASK_xxx sigs to decouple from 
                   task.h.
                   ASYNC con<F2st definitions moved to diagcomm.h?
                   Externalized msg_nice[] and log_nice[] for streaming config 
                   support.
                   Moved prototype of diag_kick_watchdog() to diagtarget.h.
                   Added prototype of diagpkt_process_request() (moved from 
                   diagpkt.h).
                   Updated prototype for diag_do_escaping().
                   Removed prototype for diag_do_unescaping().
                   Removed references to diagpkt_refresh_cache().
02/23/01   lad     Rearchitected the diagnostics subsystem to be a service 
                   rather than a module.  All coupling to targets has been
                   moved to target-specific implementations.  The only coupling
                   that remains are common services and REX.
                   Old history comments have been removed since most of them
                   are no longer applicable.

===========================================================================*/
#include "comdef.h"

#include "diag_v.h"
#include "diagbuf_v.h"
#include "log.h"
#include "diagcomm_ctrl.h" /* For diag_ctrl_msg_type */
#include "diagpkt.h"
#include "diagcomm_v.h"
#include "diag_param.h"

#if defined(FEATURE_DIAG_NV)
#include "nv.h"
#endif
#include "osal.h"

#include <string.h>
#include "diagi.h"

#ifdef FEATURE_IS2000
	#ifndef DIAG_IS2000
		#define DIAG_IS2000
	#endif
#endif

#include "rcinit.h"
#include "eventi.h"

/* -------------------------------------------------------------------------
** Diag Task Signal Definitions
** ------------------------------------------------------------------------- */

#define DIAG_RPT_TIMER_SIG        0x00000001    /* This signal is set when the 
                                                   diag_rpt_timer expires */

#define DIAG_RX_SIG               0x00000002    /* This signal is set by the 
                                                   Serial I/O Services when an 
                                                   HDLC packet is received. */

#define DIAG_CHANGE_CONN_SIG      0x00000008    /* This signal is set to change 
                                                   the connection from SMDL to
                                                   Sockets for backwards 
                                                   compatibility */

#define DIAG_MC_CMD_SIG           0x00000010    /* This signal is specified when
                                                   issuing commands to the 
                                                   mc_cmd_q of the mc task.
                                                   When the command is
                                                   completed, the mc task sets
                                                   this signal to notify the
                                                   Diagnstic Task. */

#define DIAG_MICRO_DIAGBUFFER_UIMAGE_SIG DIAG_MC_CMD_SIG /* Since DIAG_MC_CMD_SIG is not used 
                                                            anywhere, reusing it to micro_
                                                            diagbuffer signaling. Once come out
                                                            micro uimage mode in uSleep call 
                                                            back will set the time to expire 
                                                            immediatly and deliver this signal to 
                                                            Diag task which upon receive of it 
                                                            will set Drain signal on itself.
                                                         */

#define DIAG_RX_DELAY_RSP_SIG     0x00000040    /* Signal is set when delayed 
                                                   responses are generated for 
                                                   RX traffic */

#define DIAG_TX_SIG               0x00000080    /* This signal is set by the 
                                                   Serial I/O Services when a
                                                   packet has been sent. */

#define DIAG_TOUT_TIMER_SIG       0x00000100    /* This signal is a timer
                                                   signal for timeout if bad 
                                                   SPC is rec'd */

#define DIAG_RX_RSP_SIG           0x00000200    /* Signal is set when responses 
                                                   are generated for RX traffic */

#define DIAG_REFRESH_CACHE_SIG    0x00000400    /* Signal to tell DIAG to refresh
                                                   any cache it may have. */

#define DIAG_COMM_OPEN_SIG        0x00000800    /* Signal that SIO is availabe
                                                   to be opened for use. */

#define DIAG_COMM_CLOSE_SIG       0x00001000    /* Signal that SIO is no
                                                   longer available for use. */

#define DIAG_SIO_SWITCH_UART_SIG  DIAG_COMM_CLOSE_SIG
                                                /* Signal indicating that the 
                                                   diag should go back to the
                                                   aux uart */

/* 0x2000 - 0x8000 are reserved task signals.  
** In MSM builds it is defined in task.h
*/
#ifdef TASK_START_SIG
#define DIAG_TASK_START_SIG TASK_START_SIG
#else
#define DIAG_TASK_START_SIG 0x8000
#endif

#ifdef TASK_STOP_SIG
#define DIAG_TASK_STOP_SIG TASK_STOP_SIG
#else
#define DIAG_TASK_STOP_SIG 0x4000
#endif

#ifdef TASK_OFFLINE_SIG
#define DIAG_TASK_OFFLINE_SIG TASK_OFFLINE_SIG
#else
#define DIAG_TASK_OFFLINE_SIG 0x2000
#endif


#define DIAG_EVENT_TIMER_SIG        0x00010000    /* Signal to finish terminate
                                                    the current event reporting
                                                    buffer. */
                

#define DIAG_INTERNAL_SIG           0x00020000    /* Signal that an internal Diag signal
                                                    needs to be handled */

#define DIAG_BLOCK_SIG              0x00040000    /* Generic block mechanism for
                                                    clients to suspend DIAG task */

#define DIAG_CTRL_MSG_SIG           0x00080000    /* Signal to process inbound Diag
                                                    control traffic*/

#define DIAG_SIO_TOUT_TIMER_SIG     0x00100000    /* This signal is a timer
                                                    signal for timeout if sio 
                                                    callback is not called */

#define DIAG_CMD_READ_COMPLETE_SIG  0x00400000    /* Signal to process inbound Diag
                                                    control traffic*/
#define DIAG_REQUEST_DIAGID_SIG          0x00800000    /* Signal set when userpd initiates QDI call to request diagID  */

#define DIAG_COMM_SIO_TX_FLUSH_SIG  0x01000000    /* Signal used to turn off the masks
                                                    and empty the wmq*/
#define DIAG_EVENT_DRAIN_SEC_SIG    0x02000000    /* Signal used to drain events
                                                    of secondary stream*/
            
#define DIAG_EVENT_DRAIN_SIG        0x04000000    /* Signal used to drain events */

#define DIAG_EVENT_TIMER_SEC_SIG    0x08000000    /* Signal to finish terminate
                                                    the current event reporting
                                                    buffer of secondary stream. */


#define DIAG_CMD_READ_TOUT_SIG      0x80000000    /* NAND driver for DM needs signal
                                                    to communicate completion.  */

/* -------------------------------------------------------------------------
** Diag Internal Signals
** ------------------------------------------------------------------------- */
#define DIAG_INT_CTRL_REG_SIG             0x00000001    /* Signal to forward Diag 
                                                          registration msgs to Master
                                                          via CTRL channel */

#define DIAG_INT_MASK_UPDATE_SIG          0x00000002    /* Signal for Master to send
                                                          updated F3/Event/Log masks 
                                                          to Slaves */

#define DIAG_INT_FEAT_UPDATE_SIG          0x00000004    /* Signal to send internal
                                                          Diag feature flag mask to
                                                          other procs */
#define DIAG_INT_REG_TBL_RSP_SIG          0x00000008    /* Signal to send the
                                                          Command Registration table
                                                          as a delayed response */

#define DIAG_INT_IDLE_PROCESSING_SIG      0x00000100    /* Signal to wake up diag for 
                                                          for diag idle processing */

#define DIAG_INT_SET_DRAIN_TIMER_SIG      0x00000200    /* Signal to set Diag drain 
                                                          timer in Diag's context. */

#define DIAG_MASK_CHANGE_NOTIFY_SIG       0x00000400    /*Signal to notify clients about 
                                                          diag mask change */

#define DIAG_INT_DRAIN_BUFFERING_BUFFER_SIG     0x00001000    /* Signal to notify Diag to drain
                                                          bufferring buffer  */

                                                      
#define DIAG_INT_FLUSH_BUF_SIG            0x00008000    /* Signal for diagbuf flush */

#define DIAG_INT_CMD_DEREG_SIG            0x00040000       /* Signal to deregister diag commands */

#define DIAG_INT_SEND_BT_MASK_SIG         0x00200000    /* Signal to send BT mask 
                                                          to master diag */ 

#define DIAG_INT_RESEND_DATA_SIG          0x00400000    /*Signal to resend data*/

#define DIAG_INT_SEND_PERIPH_BUF_SIZE_SIG 0x00800000    /*Signal to send peripheral buffering
                                                          pool size */

#define DIAG_INT_RESEND_CTRL_SIG          0x01000000    /*Signal to resend command*/

#define DIAG_INT_RESEND_CMD_SIG           0x02000000    /*Signal to resend command*/
                            
/* -------------------------------------------------------------------------
** Update Mask Bit Flags
** ------------------------------------------------------------------------- */
#define DIAG_UPDATE_MASK_NONE            0x00000000
#define DIAG_UPDATE_MASK_F3              0x00000001
#define DIAG_UPDATE_MASK_EVENTS          0x00000002
#define DIAG_UPDATE_MASK_LOGS            0x00000004
#define DIAG_UPDATE_MASK_PARTIAL_F3      0x00000008
#define DIAG_UPDATE_MASK_PARTIAL_LOGS    0x00000010
#define DIAG_UPDATE_MASK_ALL         (DIAG_UPDATE_MASK_F3     | \
                                      DIAG_UPDATE_MASK_EVENTS | \
                                      DIAG_UPDATE_MASK_LOGS)

/* Error codes for diag_set_current_preset_mask_id() */
#define DIAG_SET_PRESET_ID_SUCCESS       0
#define DIAG_SET_PRESET_ID_NOT_SUPPORTED 1

/* -------------------------------------------------------------------------
** Defines for send buffer 
** ------------------------------------------------------------------------- */

#define DIAG_WORD_ALIGN(x) ((x+3)&(~3))

/* Buffer size for F3s/logs/Events for user PDs */
#define DIAG_SEND_BUF_SIZE_PD_DATA DIAG_SEND_BUF_SIZE_DATA

/* First buffer is always reserved for local DIAG and hence the index is 0 */
#define DIAG_LOCAL_SEND_BUF_IDX 0


/* -------------------------------------------------------------------------
** Externalized data members
** ------------------------------------------------------------------------- */
/* Timers.
*/
extern osal_timer_t diag_rpt_timer;   /* Watchdog report timer */

/* Masks to indicate the operating state. */
boolean diag_set_security_state (diag_security_type security);
boolean diag_set_sp_state (diag_sp_type sp);


/* -------------------------------------------------------------------------
** Definitions and Declarations
** ------------------------------------------------------------------------- */
typedef struct
{
  uint32 pool_id;
  uint32 item_size;
  uint32 item_cnt;
  
} diag_pool_info_type;

typedef struct
{
    uint8 *dest_ptr;        /* Buffer to hold received packet       */
    unsigned int max_size;  /* Maximum size for request packet      */
    unsigned int pkt_len;   /* Packet length when it is constructed */
    diagcomm_enum_port_type port_num; /* Port on which packet is received*/
}diag_rx_pkt_type;


/*===========================================================================

FUNCTION DIAG_SLEEP_INIT

DESCRIPTION
  This procedure initializes variables for Diag sleep voting.

===========================================================================*/
void diag_sleep_init( void );

/*===========================================================================

FUNCTION DIAG_ALLOW_SLEEP

DESCRIPTION
  Tell the sleep task that we are in a state where we can sleep.

SIDE EFFECTS
  The sleep task may take us up on our offer.

===========================================================================*/
void diag_allow_sleep (void);

/*===========================================================================

FUNCTION DIAG_FORBID_SLEEP

DESCRIPTION
  Tell the sleep task that we are NOT in a state where we can sleep.

===========================================================================*/
void diag_forbid_sleep (void);

/* -------------------------------------------------------------------------
** Function Prototypes
** ------------------------------------------------------------------------- */

/* Connection state */
#define DIAG_CX_NONE_S (0x00)
#define DIAG_CX_COMM_S (0x01)
#define DIAG_CX_CMD_S  (0x02)
#define DIAG_CX_LOG_LISTEN_S (0x04)
#define DIAG_CX_EVENT_LISTEN_S (0x08)
#define DIAG_CX_FLUSH_S (0x10) /* Used for buffer flush */
#define DIAG_CX_ANY_S (0xFFFFFFFF)

#define DIAG_SET_MASK(xx_field, xx_mask) ((xx_field) |= (xx_mask))
#define DIAG_CLR_MASK(xx_field, xx_mask) ((xx_field) &= ~(xx_mask))
#define DIAG_CHECK_MASK(xx_field, xx_mask) \
  (((xx_field) & xx_mask) ? TRUE : FALSE)

extern uint32 diag_cx_state;

#define DIAG_CONSUMER_API_VER_1 0x01

/* non-HDLC Packet Structure */
typedef PACK(struct) {
    byte start;             /* Start Character -> 7E */
    byte version;           /* Version Information */
    uint16 cmd_len;         /* Length of Packet (including cmd_code and tag */
    byte pkt[1];            /* Command packet (ends with a 7E) */
}diag_non_hdlc_type;

/* These are shared in multiple diagpkt_* files.  De-couple later. */
int diagpkt_rxc_sc_good_frames (void);
int diagpkt_rxc_pc_good_frames (void);


typedef enum
{
  DIAG_SLEEP_DIAG = 0,
  DIAG_SLEEP_EVENT,
  DIAG_SLEEP_LOG,
  DIAG_SLEEP_MSG,
  DIAG_SLEEP_CNT
}
diag_sleep_vote_enum_type;

/*enum for diag sleep states */
typedef enum
{
	DIAG_SLEEP_STATE_UNKNOWN=0,
	DIAG_SLEEP_STATE_ASLEEP,
	DIAG_SLEEP_STATE_AWAKE
}diag_sleep_state_enum_type;

/*===========================================================================

FUNCTION DIAG_SLEEP_VOTE

DESCRIPTION
  This function is called to vote to allow or disallow sleep.

===========================================================================*/
void diag_sleep_vote (diag_sleep_vote_enum_type diag_svc, boolean vote);

/*===========================================================================
FUNCTION diag_control_sleep_voting

DESCRIPTION
  This function is called to enable/disable diag's sleep-voting.
===========================================================================*/
void diag_control_sleep_voting(unsigned int enable_vote);

/*===========================================================================

  Diagnostics Binary Search List (Array Implementation)

  This list implementation is optimized for efficient searches on a generic
  list.
  
  The list is implemented as an array of nodes, sorted by key and a pointer
  specified by the caller.
  
  This list has capabilities to act on duplicate entries.

===========================================================================*/

typedef struct
{
  unsigned int key;
  void *goo;
}
diag_searchlist_node_type;

typedef struct
{
  boolean initialized;          /* Must be initialized to 
                                   FALSE when allocated. */
  byte rsvd[3];                 /* Added padding to make it 4-byte aligned */
  osal_crit_sect_t crit_sect; /* May also be used by caller */
  uint32 seq_num;               /* Incremented during each search. */
  int count;
  int size;
  diag_searchlist_node_type *r; /* Array allocated w/ 'size' elements */
}
diag_searchlist_type;

/*===========================================================================

FUNCTION TYPE DIAG_SEARCHLIST_NOTIFY

DESCRIPTION
  This function type is passed to diag_searchlist_search_all() and is 
  called with a pointer to the given payload of that search and a pointer
  to the matched node for every node that matches 'key'.
  
===========================================================================*/
typedef void (*diag_searchlist_notify) (uint32 seq_num, void *payload, 
                                        diag_searchlist_node_type *node);

/*===========================================================================

FUNCTION TYPE DIAG_SEARCHLIST_MATCH

DESCRIPTION
  This function type is passed to diag_searchlist_delete() and is 
  called with a pointer to the given "match node" and the node found in
  the list.  This is used for a delete operation.  This function is called
  for every match of 'key' until this function returns TRUE, indicating a
  match.

RETURN VALUE
  TRUE is the nodes match.
  FALSE if the nodes do not match.
  
===========================================================================*/
typedef boolean (*diag_searchlist_match) (diag_searchlist_node_type *match_node,
                                          diag_searchlist_node_type *list_node);

/*===========================================================================

FUNCTION TYPE DIAG_SEARCHLIST_ADD

DESCRIPTION
  This function adds the specified node, passed by value, to the given list.
  It allocates the list using the diagnostics heap, increasing the size
  of the allocated list when the current allocation is exhausted.
  
LIMITATIONS
  This routine provides task-level critical section protection only.  It will
  fail if called from ISR context.
  
  This function handles critical sections internally and does not need to be
  protected by critical section by the caller.

RETURN VALUE
  TRUE if the add operation was successful.
  FALSE if the not successful, indicating insufficient resources in the heap.
  
===========================================================================*/
boolean
diag_searchlist_add (diag_searchlist_type *list, unsigned int key, void *goo);

/*===========================================================================

FUNCTION TYPE DIAG_SEARCHLIST_DELETE

DESCRIPTION
  This function deletes one node from the specified list if it matches
  the specified node.  Every node in the list that matches 'key' will be
  dispatched to 'match_fp' until 'match_fp' returns TRUE.
  
  If a match is found, the node is removed from the list.
  
  This operation maintains sorted ordering of the list.
  
  This routine may make the list smaller.
  
LIMITATIONS
  This routine provides task-level critical section protection only.  It will
  fail if called from ISR context.
  
  This function handles critical sections internally and does not need to be
  protected by critical section by the caller.

RETURN VALUE
  TRUE if a match was found and the node was removed.
  FALSE if a match was not found.
  
===========================================================================*/
boolean
diag_searchlist_delete (diag_searchlist_type *list, 
                        diag_searchlist_node_type node,
                        diag_searchlist_match match_fp);

/*===========================================================================

FUNCTION TYPE DIAG_SEARCHLIST_SEARCH_ALL

DESCRIPTION
  This function searches the specified list for all nodes that match 'key'.
  
  For every match, 'notify' is called with a pointer to the node and the
  specified 'payload' (unmodified).

  This operation does not alter the contents of the list.
  
  If 'notify' is NULL, the routine only checks for the existence of at 
  least 1 matching node and returns status.
  
ASSUMPTIONS
  This operation assumes the list is sorted.
  
LIMITATIONS
  This routine provides task-level critical section protection only.  It will
  fail if called from ISR context.
  
  This function handles critical sections internally and does not need to be
  protected by critical section by the caller.

RETURN VALUE
  TRUE if a match was found.
  FALSE if a match was not found.
  
===========================================================================*/
boolean
diag_searchlist_search_all (unsigned int key, 
                            diag_searchlist_type *list,
                            void *payload,
                            diag_searchlist_notify notify);

/*===========================================================================

FUNCTION DIAGPKT_PROCESS_REQUEST 

DESCRIPTION
  This function processes an inbound request packet.
  
  If 'rsp_func' == NULL, the response is sent to the comm layer.
  If 'rsp_func' != NULL, the response packet will be dispatched to the 
  function in lieu of the comm layer, along with 'param'.
  
RETURN VALUE
  None.
  
===========================================================================*/
void diagpkt_process_request (void * req_pkt, uint16 pkt_len, 
                              diag_cmd_rsp rsp_func, void *param, boolean bFromDiagCmdQ);

/*===========================================================================

FUNCTION EVENT_INIT

DESCRIPTION
  Initialize event buffers.

===========================================================================*/
void event_init (void);

/*=========================================================================== 

FUNCTION EVENT_SERVICE_PENDING

DESCRIPTION
   This routine services the pending events in DIAG context and places 
   them on the event Q.

===========================================================================*/
void event_service_pending (void);

/*=========================================================================== 

FUNCTION EVENT_STALE

DESCRIPTION
  This routine triggers the event service to wake up the DIAG task and send
  whatever is queued.  This is used to flush as well as to avoid stale
  events lingering in the event queues for extended periods of time.

PARAMETER
  ptr - event_struct type pointer for the PD whose report last needs to be set.
        if ptr is NULL, all the event reports is set.

RETURN
 None
 
ASSUMPTIONS
  This funciton is only called in DIAG task context.

===========================================================================*/

void event_stale (event_struct * ptr);

void event_stale_sec (void);

/*=========================================================================== 

FUNCTION EVENT_DRAIN

DESCRIPTION
  Drains event reporting packet (already buffered) to the outbound diagcomm 
  data stream.  Do escaping, all that jazz.  
  
  This is basically a specialized version of diagbuf_drain().  
  
  If 'mask' is foudn in DIAG's task signals, the routine will exit promptly.
  
RETURN VALUE
  TRUE if event_drain is complete, FALSE if in progress.

SIDE EFFECTS
  Changes the needControlFlag that diagbuf_drain() uses, if neccessary.
  
===========================================================================*/
boolean event_drain (osal_sigs_t mask);
boolean event_drain_sec (osal_sigs_t mask);

/*===========================================================================

FUNCTION LOG_INIT

DESCRIPTION
  Initialize logging services.  All logs are configured to 'disabled' state.
  All entries in the "log on demand" table are erased.
  
===========================================================================*/
void log_init (void);

/*===========================================================================

FUNCTION LOG_FLUSH

DESCRIPTION
  Prepares the logging service for a flush.  The latest log that was 
  committed is marked to be sent.  All others are discarded.

===========================================================================*/
void log_flush (void);

/*===========================================================================

FUNCTION LOG_PANIC

DESCRIPTION
  Prepares the logging service for panic mode.  All logs are configured off.
  All listeners are deleted.

===========================================================================*/
void log_panic (void);

/*===========================================================================

FUNCTION MSG_SET_LIMIT

DESCRIPTION
  Set the minimum message level.

===========================================================================*/
void msg_set_limit (uint8 msg_level_limit);

/*----------------------------------------------------------------------------
  Declaration of the 8 bytes in the check block which is the security password
----------------------------------------------------------------------------*/
//TODO: This is in diagtarget.h - get rid of it here.
extern byte const chk_security_code_ptr[];

extern boolean diag_services_initialized;

extern boolean diagcomm_apps_only_config;

/*===========================================================================

FUNCTION DIAG_DRAIN_TIMER_ENABLED

DESCRIPTION
	Returns TRUE if drain timer length set is greater than 0 indicating
	drain timer is enabled.

===========================================================================*/

boolean diag_drain_timer_enabled(void);

/*===========================================================================

FUNCTION DIAG_CLR_DRAIN_TIMER

DESCRIPTION
  This function clears the drain timer.

===========================================================================*/

void diag_clr_drain_timer(void);
/*===========================================================================

FUNCTION DIAG_CTRL_DRAIN_TIMER_LEN

DESCRIPTION
  This function is used to update the global drain timer length.

===========================================================================*/

void diag_ctrl_drain_timer_len(unsigned int drain_timer_len);
/*===========================================================================

FUNCTION DIAGPKT_CTRL_GET_MSG_TYPE

DESCRIPTION
  This function returns the ctrl msg type in the specified diag ctrl packet.

PARAMETERS
  ptr - Pointer to a Diag control packet
  
RETURN VALUE
  The control packet's msg type value
  
===========================================================================*/

uint32 diagpkt_ctrl_get_msg_type( void * ptr );

/*===========================================================================

FUNCTION DIAG_SET_INTERNAL_SIGS

DESCRIPTION
  Sets Diag internal signal mask, which also sets DIAG_INTERNAL_SIG. When 
  diag_handle_sigs() handles DIAG_INTERNAL_SIG, diag_handle_internal_sig() 
  is executed to handle all Diag internal sigs.

PARAMETERS
  sigs - Diag internal signals to set.
    
DEPENDENCIES
  None

RETURN VALUE
  Returns the updated internal signal mask.

SIDE EFFECTS

===========================================================================*/

dword diag_set_internal_sigs( dword sigs );

/*===========================================================================

FUNCTION DIAG_CLEAR_INTERNAL_SIGS

DESCRIPTION
  Clears Diag internal sig.  If all Diag internal sigs are cleared,
  DIAG_INTERNAL_SIG is also cleared.
  
PARAMETERS
  sigs - Diag internal signals to clear.
  
DEPENDENCIES
  None

RETURN VALUE
  Returns the updated internal signal mask.

SIDE EFFECTS

===========================================================================*/

dword diag_clear_internal_sigs( dword sigs );

/*===========================================================================

FUNCTION DIAG_GET_INTERNAL_SIGS

DESCRIPTION
	Retrieves the mask of internal signals set.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  Returns the 32bit internal signal mask value.

SIDE EFFECTS

===========================================================================*/
dword diag_get_internal_sigs( void );

uint8 diag_set_current_preset_mask_id( uint8 preset_id );

uint8 diag_get_current_preset_mask_id( void );

typedef struct
{
  uint8  status : 4;        /* stores the header status */
  uint8 chain: 2;           /* Indicates whether this is a chained diagbuf pkt or not; 
                              0: not chained; 1: first chained pkt; 2: second chained pkt*/  
  uint8 version : 2;        /* stores the version; currently we support 0 and 1*/
  uint8  pad_len : 3;       /* stores the pad length to make it byte aligned */
  uint8 stream_id : 5;      /* stores the stream ID*/
  uint16 length;  /* stores the length of the buffer */
} diagbuf_header_type;

void diag_ctrl_update_event_mask( diag_ctrl_msg_type * ctrl_msg );
void diag_ctrl_update_event_preset_mask( diag_ctrl_msg_type * ctrl_msg );

void diag_ctrl_update_log_mask( diag_ctrl_msg_type * ctrl_msg );
void diag_ctrl_update_log_preset_mask( diag_ctrl_msg_type * ctrl_msg );


void * diag_listeners_malloc (unsigned long num_bytes);
void diag_listeners_free(void *mem_ptr);

void* diag_get_time_api_request(void* req_pkt, uint16 pkt_len);

void diag_set_time_api(diag_time_api_types time_api_to_use);

boolean diag_get_time_api_dcm(int client_handle, byte* out_buf_ptr, unsigned long outbuf_max_bytes, unsigned long* pbytes_written) ;

diag_tx_mode_type* diag_get_tx_mode_ptr(uint8 stream_id, uint8 diag_id);


#endif /*DIAGI_V_H*/
