#ifndef DIAGBUF_V_H
#define DIAGBUF_V_H
/*==========================================================================

               Diagnostics Output Buffer Allocation Manager

Description
  Type and function definitions for the diag output allocation manager.

Copyright (c) 1999-2017 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                          Edit History

      $Header: //components/rel/core.qdsp6/2.1.c4/services/diag/DCM/common/src/diagbuf_v.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/31/17   nk      Qshrink 2 support for uimage diag messages
08/14/17   sp      Fix start up bug in diag task when hardware threads reduced
05/05/17   ph      Configure buffering modes for user PD send buffers.
02/21/17   ph      Tag DIAG ID header to all the diag packets sent out.
06/05/17   nk      Added debug support for uimage in root PD
04/24/17   sp      Added support for extended logs in userPD 
04/09/17   sp      Added support for extended logs in corePD
05/12/16   ph      SMD references removal and code clean up.
04/08/15   ps      Diag over sockets and DSM removal
01/21/15   sr      Added changes to remove DSM items 
11/14/14   jtm/vk  Multi-PD Optimizations 
07/06/14   ph      Added support for Diag over STM
11/22/13   sr      Removed PACK from pointer references 
09/27/13   xy      Removed code under FEATURE_WINCE  
01/03/13   sg      Command Response Channel separation 
12/13/12   rs      Added timeout mechanism to unblock diag in diagbuf_flush. 
10/15/12   rh      Moved DIAGBUF_SIO_TX_ITEM_SIZ here from diagbuf.c
07/05/12   sg      Changes to bring up diag Multi PD
01/05/11   mad     Changes for dual mask 
11/30/10   hm      Made diagbuf_alloc and diagbuf_commit private. Moved out of diagbuf.h
10/27/10   sg      Moved  DIAG_TX_SLEEP_THRESHOLD_DEFAULT, DIAG_TX_SLEEP_THRESHOLD_DEFAULT
                   to diag_cfg.h
10/26/10   vs      Changes for new encoding protocol
10/25/10   is      Diag feature flag cleanup
09/22/10   mad     Removed  diagbuf_tx_sleep_threshold_set(). Added 
                   diagbuf_ctrl_tx_sleep_parameters(). 
09/13/10   is      Add flow control count and DSM chaining count to Diag Health
05/11/10   JV      Changing the default sleep time and sleep threshold values
                   for better performance on the aDSP. Brew targets get these
                   values from cust files and Sreelakshmi is going to add a
                   feature for WM.
03/02/10   JV      Moved DIAG_TX_SLEEP_TIME_DEFAULT and 
                   DIAG_TX_SLEEP_THRESHOLD_DEFAULT to diagbuf_v.h since these
                   are required even in diagdiag_common.c.
10/21/09   JV      Putting in a new feature so that we can now use the 
                   diagbuf_send_pkt_multi_proc() function to send data out
                   if we require the response to be prioritized.
08/05/09   JV      Removed the CUST_H featurization around the inclusion of 
                   customer.h.
07/31/09   JV      Merged Q6 diag code back to mainline
07/17/09   mad     Featurized includion of customer.h for WM.
07/10/09   as      Decoupled code FEATURE_DIAG_NON_STREAMING
06/23/09   JV      Introduced the OS abstraction layer
05/07/09   mad     Clean-up: moved DIAG_TX_SLEEP_TIME_DEFAULT and 
                   DIAG_TX_SLEEP_THRESHOLD_DEFAULT to diagbuf.c, since these
                   are required only in diagbuf.c
05/07/09   vk      moved declerations into diagbuf.h that were being referred 
                   from there
04/27/09   mad     Combined diagbuf_tx_sleep_time_set() and
                   diagbuf_tx_sleep_threshold_set() into one function,
                   diagbuf_tx_sleep_parameters_set().
12/12/08   vg      CMI split diagbuf.h and diagbuf_v.h
10/03/08   vg      Updated code to use PACK() vs. PACKED
06/09/08   mad     change to define default sleep time and threshold 
                   conditionally
02/05/08   pc      Added support for dynamic threshold and sleep values
12/15/06   as      Fixed compiler warnings.
10/31/05   as      Fixed lint errors.
06/27/01   lad     diagbuf_blocking_alloc(): moved 'code' to diagpkt_alloc().
02/23/01   lad     Moved constants to diagtune.h.  Cosmetic cleanup for
                   rearchitected diag release.
11/07/00   jal     Renamed DIAGBUF_DATA_AVAIL_S to DIAGBUF_READY_S
09/01/00   pj      Increased DIAGBUF_MAX_SIZE to 10KB. Such a large buffer is
                   considered a temporary measure until the GPS SRCH code can
                   be sped up (it is currently starving the diag task, thus
                   requiring a large buffer).
08/28/00   lad     Removed use of DIAGBUF_MAX_SIZE.
06/19/00   lad     Moved some macros and prototypes from diagi.h.
05/15/00   lad     Updated load balancing.
05/10/00   lad     Added DIAGBUF_NICE_SIZE for load balancing.  Updated API.
02/28/00   lad     Created file.

===========================================================================*/

#include "comdef.h"

#include "customer.h"

#include "osal.h"
#include "diag.h"
#include "diag_v.h"
#include "diagbuf.h"
#include "diagbuf_mpd.h"
#include "diagcomm_io.h"
#include "ULog.h"
#include "micro_diagbuffer_drain.h" /* For micro_diagbuffer_drain_result */

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed to interface with the diag output allocation manager.

===========================================================================*/
/*---------------------------------------------------------------------------
  The following are indices for the ring buffer used for streaming diag
  output from the DMSS.  Each packet will be stored in the following format.
  Numbers are expressed as bytes.

  -----------------------------
  |                           |
  |  Length (2)               |  <-- Length of entire packet
  |                           |      (Data + CRC + Length field)
  -----------------------------
  |                           |
  |                           |
  |  Output Data (Length - 4) |  <-- Buffer given to caller (ptr to this)
  |                           |
  |                           |
  -----------------------------
  |                           |
  |  CRC (2)                  |  <-- CRC required for transport protocol
  |                           |
  -----------------------------

---------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  The following variables are used to manage the ring buffer allocation scheme.
  The circular model is composed of a linear buffer of size 2*N where N is the
  size of the buffer.  The index to the buffer = idx % N;  This handles buffer
  wrap since head - tail will always yield the amount of allocated space.

  diagbuf_tail (T) - The end of the used buffer.  Data is drained to the SIO
                     queues from here.

  diagbuf_head (H) - The front of buffer (where writers allocate)

  With this scheme, the following calculations hold true:

  Empty condition:        H == T
  Full condition:         H - T == N
  Amnt used:              H - T

  Since 'int2' operations are atomic, the only critical section is
  moving the head.

----------------------------------------------------------------------------*/


/* -------------------------------------------------------------------------
** Definitions and Declarations
** ------------------------------------------------------------------------- */

/*===========================================================================

MACRO DIAGBUF_EMPTY
MACRO DIAGBUF_FULL

DESCRIPTION
  Conditional macros for diag streaming buffer.

RETURN VALUE
  Conditional values.

===========================================================================*/
#define DIAGBUF_EMPTY(h, t) (h == t)



/*===========================================================================

MACRO DIAGBUF_USED
MACRO DIAGBUF_FREE

DESCRIPTION
  Status calculating macros for the diag ring buffer(This code
  runs in GuestOS context here "s" could be the size of either
  GuestOS diagbuf itself or UserPD diagbuf that is being
  processed in GuestOS).

PARAMETERS
  h - head index
  t - tail index
  s - size

RETURN VALUE
  Number of bytes.

===========================================================================*/
#define DIAGBUF_USED(h, t, s) ((h >= t) ? (h - (t)) : (h - (t) + 2*s))
#define DIAGBUF_FREE(h, t, s) (s - DIAGBUF_USED(h, t, s))

/*===========================================================================

MACRO DIAGBUF_CALC_IDX

DESCRIPTION
  Calculates index in Diag output buffer (This code runs in
  GuestOS context here "s" could be the size of either GuestOS
  diagbuf itself or UserPD diagbuf that is being processed in
  GuestOS).

PARAMETERS
  x = Var on whom index in PD's buffer needs to be calculated
  s = Size of a PD's Diag buffer

DEPENDENCIES
  0 <= x < (2 * s)

RETURN VALUE
  Index in diag ring buffer.

===========================================================================*/
#define DIAGBUF_CALC_IDX(x, s) ((x >= s) ? (x - s) : x)

/*===========================================================================

MACRO DIAGBUF_WRAP

DESCRIPTION
  Wraps abstract buffer index.

PARAMETERS
  x = target variable which may need to be wrapped
  s = Size of the buffer

DEPENDENCIES
  0 <= x < (2 * s)

===========================================================================*/
#define DIAGBUF_WRAP(x, s)                    \
do {                                       \
  x += s - DIAGBUF_CALC_IDX(x, s); \
  if (x >= 2 * s)               \
    x -= 2 * s;                 \
/*lint -save -e{717} */                      \
} while(0)                                 \
/*lint -restore */

/*===========================================================================

MACRO DIAGBUF_MOVE

DESCRIPTION
  Moves the buffer index.

PARAMETERS
  i = Target variable to move by l
  l = length to move i by
  s = Size of a PD's Diag buffer

DEPENDENCIES
  0 <= x < (2 * DIAGBUF_SIZE)

===========================================================================*/
#define DIAGBUF_MOVE(i, l, s)   \
do {                         \
  i += l;                    \
  if (i >= 2 * s) \
   i -= 2 * s;   \
/*lint -save -e{717} */        \
} while(0)                   \
/*lint -restore */

/* To fill in diagbuf header for a hole, alignement need not be 8 byte aligned 
   on q6, macro below aligns on 32 bit boundary */
#define DIAGBUF_NEXT_ALIGNED_BYTE_4(p) (((p) + 3) & ~3)

#define DIAGBUF_ALIGNED_BYTE_4(p) ((p) & ~3)

typedef enum
{
  DIAGBUF_EMPTY_S,        /* Nothing in ring buffer or event buffer */
  DIAGBUF_READY_S,        /* Data from ring buffer queued to DSM, but ring
                             buffer still has more data to be queued--make */
                          /* sure diagbuf_drain() gets called again soon! */
  DIAGBUF_NOT_READY_S     /* Ring/event buffer has data to be queued to 
                             DSM, but we're blocked from queueing because
                             it's uncomitted, we don't have an SIO stream
                             yet, or we're flow-controlled by DSM. */
}
diagbuf_status_enum_type;

/* diagbuf_header_status_enum_type included here to STM_BUFFER type is visible */
/*lint -save -e{641} */
typedef enum
{
  DIAGBUF_UNCOMMIT_S,  /* Notify that the buffer is not commited    */
  DIAGBUF_COMMIT_S,    /* Notify that the buffer is commited        */
  DIAGBUF_WRAP_S,      /* Notify that the buffer has to be wrapped  */
  DIAGBUF_HOLE_S,      /* Notify that the buffer has a hole         */
  DIAGBUF_UNUSED_S,    /* Notify that the buffer is unused          */
  DIAGBUF_STM_BUFFER_S, /* STM Buffer, NOT allocated from diagbuf   */
} diagbuf_header_status_enum_type;
/*lint -restore */


/* The send descriptor type. */
typedef enum
{
  DIAG_SEND_STATE_START,
  DIAG_SEND_STATE_BUSY,
  DIAG_SEND_STATE_CRC1,
  DIAG_SEND_STATE_CRC2,
  DIAG_SEND_STATE_TERM,
  DIAG_SEND_STATE_COMPLETE
}
diag_send_state_enum_type;

typedef enum {
  DIAG_STATE_START,
  DIAG_STATE_WRITE_FIRST_BYTE_LEN,
  DIAG_STATE_WRITE_SECOND_BYTE_LEN,
  DIAG_STATE_WRITE_PACKET,
  DIAG_STATE_END
} diag_enhc_encode_state;

typedef PACK(struct)
{
  const void *pkt;
  const void *last; /* Address of last byte to send. */
  diag_send_state_enum_type state;
  boolean terminate;                 /* True if this fragment 
                                      terminates the packet */
  boolean priority;
  boolean is_cmd_rsp_type; /*set to notify whether it is command response or other data*/                                
  uint8 diag_id;   /* DIAG ID */
}
diag_send_desc_type;

/*Diagbuf packets can be chained; the below enum indicates which 
  part of the chain is the current diagbuf pkt*/
typedef enum
{
  DIAGBUF_NO_CHAIN,
  DIAGBUF_FIRST_PKT_CHAIN,
  DIAGBUF_SECOND_PKT_CHAIN,
  DIAGBUF_LAST_PKT_CHAIN = DIAGBUF_SECOND_PKT_CHAIN
}diagbuf_chain_type;

#define DIAG_ULOG_NO_OFFSET         0xFF    /* No offset provided; custom time used by DiagBuffer*/
#define DIAG_ULOG_NO_CUSTOM_OFFSET  0x00    /*No offset provided; Synchronization not required */
#define DIAG_ULOG_SYNC_OFFSET_TYPE  0x01    /* Network time offset provided (8 bytes) */

/* Maps each version of the diagbuf packet to the length of the first packet in the chain */
extern uint8 diagbuf_ver_len[DIAGBUF_MAX_VERSION_NUM];

extern boolean diag_hdlc_protocol;

#define DIAGBUF_OUTBOUND_PKT_HANDLER_BIT_MASK 16

typedef struct
{
  q_link_type link;
  diagbuf_mpd_type *buf;

} diagbuf_mpd_q_type; 

/* -------------------------------------------------------------------------
** Function Prototypes
** ------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C"
{
#endif

/*===========================================================================

FUNCTION TYPE DIAGBUF_OUTBOUND_PKT_HANDLER_TYPE

DESCRIPTION
  This function type, if registered with a command code, is called from
  DIAG task context when the queued outbound pakcet is ready to send.

  diagbuf_drain() automatically processes packets in raw format.  However,
  some packets, such as those used in the debug message service, require
  post-processing in DIAG task context prior to sending.

  This system requires that diagbuf_send_pkt is used to send the final
  packet to the comm layer.  This routine maintains a state, which requires
  that all outbound packets be sent via that function.

RETURN VALUE
  Number of bytes used, returned by diagbuf_send_pkt().

===========================================================================*/
typedef void (*diagbuf_outbound_pkt_handler_type)(diag_send_desc_type *desc, byte stream_id);

/*===========================================================================

FUNCTION DIAGBUF_INIT

DESCRIPTION
  This function initializes the diag allocation buffer at startup.

============================================================================*/
void
diagbuf_init(void);


/*===========================================================================
FUNCTION DIAGBUF_ALLOC

DESCRIPTION
 This function allocates the specified amount of space in the diag output
 buffer.


code            	Specifying the command code.
length          	Length requested.
nice            	Padding value
diagbuf_alloc_request 	Type of service requesting buffer space.


DEPENDENCIES

diagbuf_commit() must be called to commit a buffer to the diag pool.  This
must happen reasonably soon after this call completes.  If the buffer is
needed to accumulate data over time, then you should use your own buffer
use this just long enough to copy the data.

SIDEEFFECTS

diagbuf_head may be updated.

RETURN VALUES

Pointer to allocated buffer if successful
NULL if space is not available.

============================================================================*/
void*
diagbuf_alloc(uint8 code, uint32 length, int nice, uint8 diagbuf_alloc_request, uint8 chain, uint8 version);

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
                             1,2 - extended pkts
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
   byte stream_id );

void diagbuf_clear_index (diagcomm_io_conn_type * conn, diagcomm_io_tx_params_type * tx_param );

/*===========================================================================
FUNCTION diag_send_buf_retry

DESCRIPTION
  This function is called when SMDL has signaled diag to resend the buffer if
  it was not able to send all the data successfully in first attempt
===========================================================================*/
void diag_send_buf_retry(diagcomm_io_channel_type ch_type, diagcomm_enum_port_type port_type, diag_tx_mode_type* tx_mode_ptr);

/*===========================================================================
 FUNCTION DIAGBUF_COMMIT

 DESCRITPION
  This function commits previously allocated space in the diag output buffer.
  ptr' must be the address that was returned by a prior call to
  diagbuf_alloc().

  This function signals the diag task and may cause a context switch.

 PARAMETERS
  *ptr             Specifying the ptr to be commited 
 DEPENDENCIES
 None
 SIDEEFFECTS
 None
 RETURN VALUES
 None

=============================================================================*/

void
diagbuf_commit(void *ptr);


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
micro_diagbuffer_drain_result diagbuf_drain_micro_diagbuffer( osal_sigs_t mask );

/*===========================================================================

FUNCTION DIAGBUF_DRAIN_DIAG

DESCRIPTION
  This function drains the diag allocation buffer to the comm layer.
  Escaping, etc is performed in this routine.

  This function may only be called by the diag task.  It is not re-entrant!
  
  This is considered a background process for the DIAG task.  Periodically,
  this routine checks the DIAG task sigs against 'mask'.  If any bits match,
  the routine exits promptly.
  
RETURN VALUE
  READY if more data can be sent
  NOT_READY if unable to proceed (uncommitted packet, flow control)
  EMPTY nothing to do

===========================================================================*/
diagbuf_status_enum_type diagbuf_drain_diag (osal_sigs_t mask);


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
diagbuf_status_enum_type diagbuf_drain (osal_sigs_t mask);


/*===========================================================================

FUNCTION DIAGBUF_SEND_PKT

DESCRIPTION
  Sends a packet, or a fragment of a packet, described by 'desc', to the
  comm layer.
  
  This handles buffer management with the comm layer and all transport layer
  related ops, like CRC calculation HDLC encoding.

===========================================================================*/
diagbuf_status_enum_type
diagbuf_send_pkt(diag_send_desc_type *desc, byte stream_id);


/*===========================================================================

FUNCTION diagbuf_ctrl_tx_sleep_parameters

DESCRIPTION
Control sleep threshold and sleep-time

===========================================================================*/
void diagbuf_ctrl_tx_sleep_parameters(unsigned int threshold_val, unsigned int sleep_val);

/*===========================================================================
FUNCTION diagbuf_ctrl_commit_threshold
DESCRIPTION
Control diagbuf_commit_threshold
===========================================================================*/
void diagbuf_ctrl_commit_threshold(unsigned int val);


/*===========================================================================

FUNCTION diagbuf_tx_sleep_parameters_set

DESCRIPTION
Called during diag initialization.
This initializes the value of diagbuf_tx_sleep_threshold and 
diagbuf_tx_sleep_time, after reading NV(AMSS)/Registry(WM).

===========================================================================*/
void diagbuf_tx_sleep_parameters_set(void);
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

RETURN VALUE
  TRUE if registration was successful.

===========================================================================*/
boolean
diagbuf_register_outbound_pkt_handler(uint8 cmd_code,
                                      diagbuf_outbound_pkt_handler_type fp);


/*===========================================================================
FUNCTION DIAGBUF_WRITE_STREAM_ID

DESCRIPTION
   Writes Stream Id to the packet in diagbuf
    
===========================================================================*/
void diagbuf_write_stream_id(void *ptr, byte stream_id);

/*===========================================================================
FUNCTION diagbuf_set_bit_outbound_pkt_handler

DESCRIPTION
   Writes Stream Id to the packet in diagbuf
    
===========================================================================*/
void diagbuf_set_bit_outbound_pkt_handler(void *ptr);

/*===========================================================================
FUNCTION diagbuf_clear_bit_outbound_pkt_handler

DESCRIPTION
   Writes Stream Id to the packet in diagbuf
    
===========================================================================*/
void diagbuf_clear_bit_outbound_pkt_handler(void *ptr);

/*===========================================================================
FUNCTION diagbuf_read_stream_id

DESCRIPTION
    Reads Stream Id from the packet in diagbuf
    
===========================================================================*/
byte diagbuf_read_stream_id(const void *ptr);

/*===========================================================================

FUNCTION DIAGBUF_FLUSH_TIMEOUT

DESCRIPTION
  This function configures all streaming data items off.  No more allocations
  will occur until these settings are changed. It performs the same operation
  as diagbuf_flush only with optional timeout value.

  Diag will not stop collecting data immediately.  The data currently
  allocated will still be flushed.
 
  diag will block after flushing the data for specified timeout duration or
  time taken by SIO to flush the data(which ever is smaller).
 
  diag_flush_timeout flag will be set to TRUE if diag times out on SIO callback
 
@param[in] timeoutVal    Specifies the timeout duration. If specified as 0, 
timeout will not occur. 

===========================================================================*/
void diagbuf_flush_timeout(unsigned int timeoutVal);


/*===========================================================================
FUNCTION diagbuf_mpd_add_pd

DESCRIPTION
  Adds the diagbuf_mpd_type pointer to the linked list that keeps track of 
the user space diagbufs. 
 
PARAMETERS 
 diagbuf_mpd_type *buf - the UserPD diagbuf pointer.
 
RETURN 
 None
 
===========================================================================*/
void diagbuf_mpd_add_pd(diagbuf_mpd_type *buf);

/*===========================================================================
 
FUNCTION diagbuf_mpd_ctrl_commit_threshold

DESCRIPTION
    This function change the commit threshold for userpd's.
 
PARAMETERS 
    VAL :      The percentage of diagbuf to be set for commit threshold.
    diagID   - diag_id of the entity (Peripheral/PD).
 
RETURN 
    None
 
===========================================================================*/

void diagbuf_mpd_ctrl_commit_threshold( unsigned int val, uint8 diagID );


/*===========================================================================
FUNCTION diagbuf_mpd_remove_pd

DESCRIPTION
  Removes the diagbuf_mpd_type pointer from the linked list that
keeps track of the user space diagbufs.  This function will not 
free memory associated with the diagbuf. 
 
PARAMETERS 
 diagbuf_mpd_type *buf - the UserPD diagbuf pointer.
 
RETURN 
 None

===========================================================================*/
void diagbuf_mpd_remove_pd(diagbuf_mpd_type * buf);

#ifdef __cplusplus
  }
#endif
#endif /*DIAGBUF_V_H*/
