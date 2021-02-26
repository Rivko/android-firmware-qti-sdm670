#ifndef DIAGBUF_MPD_V_H
#define DIAGBUF_MPD_V_H
/*==========================================================================

               Diagnostics Output Buffer Allocation Manager

Description
  Type and function definitions for the diag output allocation manager.

Copyright (c) 2014-2015, 2017 by QUALCOMM Technologies, Incorporated.
All Rights Reserved. Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                          Edit History

      $Header: //components/rel/core.qdsp6/2.1/services/diag/LSM/common/src/diagbuf_mpd.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/21/17   ph      Tag DIAG ID header to all the diag packets sent out.
11/03/16   sa      Implemented seperate queue for UserPD events and enabled buffering
12/23/15   as      Removed redundant helper functions for stream_id
04/24/17   sp      Added support for extended logs in userPD 
04/09/17   sp      Added support for extended logs in rootPD
11/11/15   as      Changes for PD restart 
11/14/14   jtm/vk  Created file 
===========================================================================*/
#include "diagbuf_mpd_type.h"
#include "diagbuf.h"
#include "diag_qdi.h"
#include "diag_v.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed to interface with the diag output allocation manager.

===========================================================================*/

extern boolean diag_hdlc_protocol;

/* -------------------------------------------------------------------------
** Function Prototypes
** ------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C"
{
#endif

/* Maps each version of the diagbuf packet to the length of the first packet in the chain */
extern uint8 diagbuf_mpd_ver_len[DIAGBUF_MAX_VERSION_NUM];

/*===========================================================================

FUNCTION DIAGBUF_MPD_INIT

DESCRIPTION
   This function allocates memory of type diagbuf_mpd_type, 
   initializes it and returns pointer to this location to the
   UserPD. UserPD uses this pointer to access it's diagbuf which
   is allocated in diagbuf_mpd_buf_init() and pointer to it, is
   saved in diagbuf_mpd_type->diagbuf_mpd_buf. It is assumed
   that this function is operating at the privileged level to
   make the Qurt_qdi_user_malloc call.

PARAMETERS 
   int client_handle - that is passed to diag_qdi_invoke
   int pid           - PID obtained in diag_lsm_init using qurt_getpid fn.
   
RETURN VALUE
   diagbuf_mpd_type if qurt_qdi_user_malloc is successful
   NULL otherwise

============================================================================*/
diagbuf_mpd_type* diagbuf_mpd_init( int client_handle, int pid );

/*===========================================================================

FUNCTION DIAGBUF_MPD_BUF_INIT

DESCRIPTION
   This function allocates and initializes a diagbuf to be used
   by UserPD. It copies the allocated pointer to the UserPD's
   object passed in param *ptr. This allows UserPD to access
   this pointer for writing diag traffic to this buffer which is
   "it's diagbuf" with shared access to Guest OS. This function
   returns allocated pointer to the caller.
   It is assumed that this function is operating at the
   privileged level to make the Qurt_qdi_user_malloc call.

PARAMETERS 
   int client_handle     - that is passed to diag_qdi_invoke
   diagbuf_mpd_type *ptr - pointer to UserPD object
   int buf_len           - length of the diagbuf requested by
                           a UserPD
   
RETURN VALUE
   uint8* pointer to buffer allocated
   NULL otherwise

============================================================================*/
uint8* diagbuf_mpd_buf_init( int client_handle, diagbuf_mpd_type *ptr, int buf_len );

/*===========================================================================

FUNCTION DIAGBUF_MPD_DEINIT

DESCRIPTION
   This function cleans up the diag allocation buffer at
   startup.
 
PARAMETERS 
   int client_handle     - that is passed to diag_qdi_invoke
   diagbuf_mpd_type *ptr - pointer to UserPD object
   
RETURN VALUE
   None

============================================================================*/
void diagbuf_mpd_deinit( int client_handle, diagbuf_mpd_type *buf );

/*===========================================================================
FUNCTION DIAGBUF_MPD_ALLOC

DESCRIPTION
  This function allocates the specified amount of space in the diag output
  buffer.
 
PARAMETERS 
  uint8  code        	            Specifying the command code.
  uint32 length          	        Length requested.
  int nice                        Padding value uint8
  diagbuf_alloc_request 	        Type of service requesting
                                  buffer space.
  uint8 chain                       0: No chain; 1: 1st packet; 2: 2nd packet
  uint8 version
DEPENDENCIES 
  diagbuf_commit() must be called to commit a buffer to the diag
  pool.  This must happen reasonably soon after this call 
  completes.  If the buffer is needed to accumulate data over 
  time, then you should use your own buffer use this just long 
  enough to copy the data. 

SIDEEFFECTS
  diagbuf_head may be updated. 

RETURN VALUES
  If successful         - Pointer to allocated buffe
  If no space available - NULL

============================================================================*/
void *
diagbuf_mpd_alloc(uint8 code, uint32 length, int nice, uint8 diagbuf_alloc_request, uint8 chain, uint8 version);

/*===========================================================================
FUNCTION DIAGBUF_MPD_ALLOC_CHAIN

DESCRIPTION
    Allocates chained(consecutive) packets in Diag MPD buffer.
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
   parameter                 Passed in by diag client while
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

void *
diagbuf_mpd_alloc_chain(uint8 code, uint32 length, int nice, uint8 diagbuf_alloc_request, uint8 chain, uint8 version, void * parameter);

/*===========================================================================

FUNCTION DIAGBUF_MPD_SHORTEN

DESCRIPTION
  This function reduces the length field of a previously allocated buffer.

  'ptr' must point to the same address that was returned by a prior call to
  diagbuf_mpd_alloc().

  Allocating too much and using this to shorten the packet is ideal for
  situations in which the length of the packet is not know prior to
  allocation.  Using this scheme does, however, consume resources that would
  otherwise be used to buffer outbound diagnostics data.  Please use this
  scheme sparingly.
 
PARAMETERS 
  void * ptr      - pointer returned by diagbuf_mpd_alloc()
  uint new_length - length, that needs to be set in len field of
                    the packet (*ptr)
 
RETURNS 
  None 

============================================================================*/
void diagbuf_mpd_shorten( void *ptr, unsigned int new_length );

/*===========================================================================
 FUNCTION DIAGBUF_MPD_COMMIT

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
void diagbuf_mpd_commit( void *ptr );

/*===========================================================================
FUNCTION DIAGBUD_MPD_CLEANUP

DESCRIPTION
  Updates in_use field in the client's diagbuf, indicating that
  the buffer is not in use. 
 
PARAMETERS 
   diag_qdi_opener *diag_qdi_opener - Diag qdi object pointer
   
   
RETURN VALUE 
   None
   
===========================================================================*/
void diagbuf_mpd_cleanup(diag_qdi_opener *diag_qdi_obj_ptr); 

/*===========================================================================
FUNCTION DIAGBUF_MPD_TX_NOTIFY

DESCRIPTION
  calls diag_lsm_trigger_drain() to set signal on diag task in
  CorePD to drain diagbufs
 
PARAMETERS 
   None
   
RETURN VALUE 
   None
   
===========================================================================*/
void diagbuf_mpd_tx_notify( void );

/*===========================================================================
FUNCTION DIAGBUD_MPD_SET_DRAIN_TIMER

DESCRIPTION
  calls diag_lsm_set_drain_timer() to set drain time on diag
  task in CorePD
 
PARAMETERS 
   None
   
RETURN VALUE 
   None
   
===========================================================================*/
void diagbuf_mpd_set_drain_timer( void );

/*===========================================================================
FUNCTION DIAGBUF_MPD_SET_MPD_DRAIN_TIMER

DESCRIPTION
  if drain timer for the PD has expired sets it. Else does
  nothing.
 
PARAMETERS 
   None
   
RETURN VALUE 
   None
   
===========================================================================*/
void diagbuf_mpd_set_mpd_drain_timer( void );

/*===========================================================================
FUNCTION DIAGBUF_MPD_WRITE_STREAM_ID

DESCRIPTION
   Writes stream_id to the packet in diagbuf
   Also sets DIAGBUF_OUTBOUND_PKT_HANDLER_BIT_MASK in stream_id
 
PARAMETERS 
   void * ptr     - packet in diagbuf
   byte stream_id - Stream Id to set in duagbuf packet
 
RETURN VALUES 
   None 
     
===========================================================================*/
void diagbuf_mpd_write_stream_id( void *ptr, byte stream_id );

/*===========================================================================
FUNCTION DIAGBUF_MPD_READ_STREAM_ID

DESCRIPTION
    Reads Stream Id from the packet in diagbuf
 
PARAMETERS 
   void * ptr - packet in diagbuf
 
===========================================================================*/
byte diagbuf_mpd_read_stream_id( const void *ptr );

/*===========================================================================
FUNCTION DIAGBUF_MPD_SET_BIT_OUTBOUND_PKT_HANDLER 

DESCRIPTION
   Sets DIAGBUF_OUTBOUND_PKT_HANDLER_BIT_MASK in stream Id to
   the packet in diagbuf. If the MSB is set in stream id, then the pkt
   generated in UserPD can be sent out without any processing
 
PARAMETERS 
   void * ptr - packet in diagbuf
 
RETURN VALUES 
   None 
     
===========================================================================*/
void diagbuf_mpd_set_bit_outbound_pkt_handler( void *ptr );

/*===========================================================================
FUNCTION DIAGBUF_MPD_CLEAR_BIT_OUTBOUND_PKT_HANDLER

DESCRIPTION
   Clear DIAGBUF_OUTBOUND_PKT_HANDLER_BIT_MASK from stream Id in
   the packet in diagbuf
 
PARAMETERS 
   void * ptr - packet in diagbuf
 
RETURN VALUES 
   None 
    
===========================================================================*/
void diagbuf_mpd_clear_bit_outbound_pkt_handler( void *ptr );

/*===========================================================================

FUNCTION DIAGBUF_MPD_FLUSH_TIMEOUT

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
void diagbuf_mpd_flush_timeout( unsigned int timeoutVal );

/*===========================================================================
 
FUNCTION DIAGBUF_MPD_GET_MSG_ALLOC_COUNT

DESCRIPTION
    This function returns the internal variable msg_alloc_count.

PARAMETERS 
    None
 
RETURN
    msg_alloc_count, if diagmpd_buf is allocated
    0 otherwise 
===========================================================================*/
uint32 diagbuf_mpd_get_msg_alloc_count( void );

/*===========================================================================
 
FUNCTION DIAGBUF_MPD_GET_MSG_DROP_COUNT

DESCRIPTION
    This function returns the internal variable msg_drop_count.
 
PARAMETERS 
    None
 
RETURN
    msg_drop_count, if diagmpd_buf is allocated
    0 otherwise
===========================================================================*/
uint32 diagbuf_mpd_get_msg_drop_count( void );

/*===========================================================================
 
FUNCTION DIAGBUF_MPD_RESET_MSG_ALLOC_DROP_COUNTS

DESCRIPTION
    This function resets the internal variables msg_alloc_count
    and msg_drop_count.
 
PARAMETERS 
    None
 
RETURN 
    None
 
===========================================================================*/
void diagbuf_mpd_reset_msg_alloc_drop_counts( void );

/*===========================================================================
 
FUNCTION DIAGBUF_MPD_GET_LOG_ALLOC_COUNT

DESCRIPTION
    This function returns the internal variable log_alloc_count.
 
PARAMETERS 
    None
 
RETURN 
    log_alloc_count, if diagmpd_buf is allocated
    0 otherwise
 
===========================================================================*/
uint32 diagbuf_mpd_get_log_alloc_count( void );

/*===========================================================================
 
FUNCTION DIAGBUF_MPD_GET_LOG_DROP_COUNT

DESCRIPTION
    This function returns the internal variable log_drop_count.
 
PARAMETERS 
    None
 
RETURN 
    log_drop_count, if diagmpd_buf is allocated
    0 otherwise
 
===========================================================================*/
uint32 diagbuf_mpd_get_log_drop_count( void );

/*===========================================================================
 
FUNCTION DIAGBUF_MPD_RESET_LOG_ALLOC_DROP_COUNTS

DESCRIPTION
    This function resets the internal variables log_alloc_count
    and log_drop_count_log.
 
PARAMETERS 
    None
 
RETURN 
    None
 
===========================================================================*/
void diagbuf_mpd_reset_log_alloc_drop_counts( void );


/*===========================================================================
 
FUNCTION DIAGBUF_MPD_GET_EVENT_ALLOC_COUNT

DESCRIPTION
    This function returns the internal variable event_alloc_count.
 
PARAMETERS 
    None
 
RETURN 
    event_alloc_count, if diagmpd_buf is allocated
    0 otherwise
 
===========================================================================*/
uint32 diagbuf_mpd_get_event_alloc_count( void );

/*===========================================================================
 
FUNCTION DIAGBUF_MPD_GET_EVENT_DROP_COUNT

DESCRIPTION
    This function returns the internal variable event_drop_count.
 
PARAMETERS 
    None
 
RETURN 
    event_drop_count, if diagmpd_buf is allocated
    0 otherwise
 
===========================================================================*/
uint32 diagbuf_mpd_get_event_drop_count( void );

/*===========================================================================
 
FUNCTION DIAGBUF_MPD_RESET_EVENT_ALLOC_DROP_COUNTS

DESCRIPTION
    This function resets the internal variables event_alloc_count
    and event_drop_count_log.
 
PARAMETERS 
    None
 
RETURN 
    None
 
===========================================================================*/
void diagbuf_mpd_reset_event_alloc_drop_counts( void );

#ifdef __cplusplus
}
#endif

#endif /*DIAGBUF_MPD_H*/

