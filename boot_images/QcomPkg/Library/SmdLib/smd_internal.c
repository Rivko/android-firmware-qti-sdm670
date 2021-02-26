/*===========================================================================

                    Shared Memory Driver Internal Source File

-----------------------------------------------------------------------------
Copyright (c) 2005-2015 Qualcomm Technologies, Inc. 
All rights reserved.
-----------------------------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/15/14   bm      Add stream_open callback in SMD buffer layer
04/23/13   sm      + Replace string copy macro with smd_string_copy. 
                   + KW warning fixes.
03/07/13   bm      KW warning fixes.
11/19/12   pa      Removing references to DSPS and Q6FW.
06/28/12   bm      Replace smd_host_type with smem_host_type
05/11/12   bm      Use new SMEM API to allocate memory from RPM.
03/22/12   bm      Use new macros to access SMD channel header struct.
01/17/12   bm      Adding support for RPM/RPM clients to use SMD drivers.
08/23/11   bt      Added smdi_get_loopback_client/server for N-way loopback.
04/12/12   vm      Fixed l4/SAL warnings
01/05/12   hwu     Removed IMAGE_APPS_PROC.
07/25/11   rs      Added support for SSR Phase 2
05/31/11   bt      Remove unneeded redefinition of FEATURE_SMEM_LOG.
04/28/11   tl      Fixed transfer type check when opening port
04/26/11   tl      Fixed close port handling when already in closing state
04/01/11   bm      Updated function definitions for smdi_alloc_channel_info()
                   and smdi_add_channel_info() which now handle 
                   smd_xfrflow_type as an argument as well.
04/07/11   rs      Make SMD channel definitions compatible across targets 
04/01/11   rs      Fixed issue where host processor did not clean up the port
                   when the remote processor never opened it, in which case
                   the REMOTE CLOSE event would never be received
04/01/11   bt      Featurize out smdi_get_port_name if not Modem/Apps.
03/30/11   tl      Support legacy apps-modem ports in all configurations
03/28/11   tl      Cleaned up fatal error messages
03/25/11   hwu     Merge in 8960 change.
02/11/11   bt      Removed old unused SMD profiling code.
01/25/11   rs      Renamed spinlock header file
01/06/11   bt      Removed SMD Block and Data Mover protocols.
12/23/10   hwu     Removed static ports from SMD port list.
12/23/10   tl      Clear mask-interrupt flag when flushing data
12/07/10   tl      When closing a port, wait for the other processor to
                   transition to state closing before calling the close callback
11/17/10   rs      Fixed bug when returning the SMD interrupt information for 
                   a given target processor  
10/25/20   bt      Fixed port_name argument bugs in all smd open functions.
10/21/10   rs      Cleaned up redundancy in function calls when sending and 
                   receiving interrupts  
10/04/10   tl      When receiving data, send an interrupt only when necessary
08/19/10   rs      Allow re-use of ports with same name and properties 
08/13/09   rs      Removed usage of backwards compatible ports for standalone SMD 
                   Lite
07/15/10   rs      Standalone SMD Lite optimizations: removed SMD block related 
                   functions
06/08/10   tl      Split SMD task into multiple tasks with different priorities
04/30/10   rs      SMD lite v2 port to ARM7: 
                     + Added SMD edge array to support N-Way shared info
                       structures and circular buffer assignments
                     + Support for standalone SMD Lite
                     + Reduce the number of dynamic ports
04/21/10   tl      Implemented SMD Lite API
04/16/10   hwu     Fixed the bug where reopen dynamic smd port with diff size 
                   failed. 
03/29/10   tl      Replace IMAGE_*_PROC with SMD_*_PROC
12/08/09   esh     Added checks to fix Klocwork warnings
11/19/09   sa      warning cleanup, sync-up with symbian
09/29/09   bfc/rs  Added variable sized FIFO API support.
09/24/09   rs      Fixed typo. 
06/10/09   bfc     CMI updates.
06/10/09   hwu     Added two bridge ports to use large FIFO size.
06/01/09   hwu     limit the use of smdi_get_port_name to the fixed smd ports.
03/31/09   hwu     Consolidated smd_event_send_xxxx to smd_event_send. 
03/12/09   jlk     Fixed stream state transition from opening to open or 
                   flushing so that pending data is sent after the state is
                   updated.
01/26/09   bfc     Added the dataxfr mode to the CID entry.
10/13/08   bfc     Fixed a State Machine event generation.
09/16/08   bfc     Fixed a corner case where we would call a function with
                   a freed smd_info.
09/02/08   jlk     fixed name smdi_alloc_channel_info() to allow channels with
                   different types and the same name.
06/23/08   bfc     Fix for variable sized SMD FIFOs.
06/19/08   bfc     Support variable sized SMD FIFOs.
06/10/08   sa      Smd_target.h cleanup. 
04/08/08   sd      Sending wakeup reasons to arm11 from arm9 and Logging these
                   wakeup reasons in arm11 using Celog
03/27/08   hwu     Fixed a bug to copy out the port name correctly.
03/26/08   hwu     Merge the volatile qulifier fix.
03/04/08   bfc     Fixed a function header.
02/21/08   bfc     First SMD Block port check in.
01/31/08   hwu     Marked smd_channel_alloc_tbl volitle.
                   Changed not to pass volatile data ptr to strncmp. 
01/29/08   hwu     Added the dummy port to smd_port_to_name tbl.
01/14/08   bfc     Created a SMD base struct.
11/18/07   hwu     Modifed smdi_stream_state_update.
11/14/07   hwu     Added channel type when allocate channel info.
11/13/07   bfc     Fixed the improper use of a spinlock
11/11/07   bfc     Removed strncpy.
11/11/07   bfc     Fixed backwords compatibilty by checking for predetermined
                   when dealing with named opens.
10/19/07   hwu     Added functions to allocate/delete/update SMD channel
                   allocation table.
10/14/07   jlk     Added SMD_PROFILE hook
10/08/07   hwu     Changed the order to include smd_os.h and smd.h
                   because smd_channel_type moved to smd.h
09/18/07   hwu     Fixed a restart race condition where remote side sees the
                   state change from closed to reset. 
07/18/07   hwu     Moved err.h and assert.h to smd_target.h
07/05/07   hwu     Added rules to decide the channel connection. 
05/22/07   bfc     Added new api.
05/02/07   hwu     Change to use SMD_NUM_CHANNELS. 
04/26/07   ptm     Use SMD NUM PORTS in place of SMD PORT LAST and SMD PORT MAX
                   and add routine to allocate shared memory channels.
03/20/07   taw     Include smd_target.h instead of customer.h and target.h.
12/14/06   hwu     Added new states handlings to support remote
                   processor restart.
12/06/06   bfc     Fixed the location where we were getting the data_read
                   flag from for transmit operations
11/29/06   taw     Use macros to access shared memory data.
11/13/06   hwu     removed smd_opening_list and merged it to smd_open_list.
11/01/06   ptm     Fixed a race condition that existed between opening a port
                   and then calling smd_notify_read/write before SMD had a
                   chance to process the open.
10/13/06   ptm     Move memory barrier to OS specific code.
10/10/06   ptm     Remove power collapse code, rename variable and fix some
                   lint warnings.
10/03/06   ptm     Remove interrupt and apps wakeup code.
09/28/06   taw     Switch to using SMD symbols instead of SIO symbols.
09/05/06   ptm     Remove references to REX.
09/05/06   ptm     Created smdi_stream_enter_opening_state and added init code
                   for max_queued_data field.
08/22/06   ptm     Merge WinMobile changes to main line.
05/24/06   bfc     Added a signal flag for RI
05/19/06   ptm     Fix compiler warnings.
05/08/06   ptm     Change IOCTL lock from rex critical section to INTLOCK.
05/05/06   ptm     Make interface signals synchronous and add pending IOCTL
                   counter.
04/28/06   ptm     Make interface signals asynchronous.
04/27/06   bfc     Added support for a REMOTE_CLOSE during a FLUSH and
                   added a new critical section variable for dtr cb's
03/31/06   bfc     Fixed a merge issue between rev 3 and 4
03/27/06   bfc     Modified SMD to use the new sio extended funcion callback
                   parameters
03/24/06   ptm     Replace stream_id with port_id in error message.
01/20/06   ptm     Added support for apps processor power collapse.
10/13/05   ptm     Initial version - derived from smd_interface.c#8
===========================================================================*/


/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "smd_os.h"

#include "smem.h"
#include "smem_log.h"

#include "smd.h"
#include "smd_v.h"
#include "smd_internal.h"
#include "smd_target.h"
#include "smd_os_common.h"
#include "spinlock.h"

/*===========================================================================
                       LOCAL DATA DECLARATIONS 
===========================================================================*/

/*-------------------------------------------------------------------------
 *                    SMD channel allocation error codes
 *-------------------------------------------------------------------------*/
#define SMDI_CHANNEL_PROPERTIES_MISMATCH    1
#define SMDI_CHANNEL_TABLE_FULL             2

/*-------------------------------------------------------------------------
 *                    Event logging definitions
 *-------------------------------------------------------------------------*/

/* Turn logging on/off here using define/undef */
#undef SMD_INTERNAL_LOG_EVENTS

#ifdef SMD_INTERNAL_LOG_EVENTS
#define SMD_INTERNAL_LOG_EVENT(id,d1,d2,d3)     SMEM_LOG_EVENT(id,d1,d2,d3)
#else
#define SMD_INTERNAL_LOG_EVENT(id,d1,d2,d3)     
#endif /* SMD_INTERNAL_LOG_EVENTS */

/*-------------------------------------------------------------------------
 *                    Stream info definitions
 *-------------------------------------------------------------------------*/
/* Assigning a contiguous static array for the stream control
   information for all streams */ 
/* This allocation type might have to be changed to accomodate security
   related features */
static smd_info_type  smd_info[SMD_NUM_CHANNEL_INFO];
static smd_info_type *smd_port_to_info[SMD_NUM_CHANNELS];
static q_type         smd_info_free_q;

/** Contains the information from ports not openened in Apps,
 *  read directly from shared memory. */
static smd_smem_port_info_type smd_smem_port_info[SMEM_NUM_SMD_CHANNELS];

/* head pointers for lists of opened and opening smd stream info structures */
smd_info_type *smd_open_list;

/* global to indicate whether the other host is in reset state */
boolean        smd_sm_remote_reset = FALSE;

/*-------------------------------------------------------------------------
 *                    channel allocation table definitions
 *-------------------------------------------------------------------------*/
typedef struct
{
  char     name[SMD_CHANNEL_NAME_SIZE_MAX];
  uint32   cid;
  uint32   prop;
  uint32   ref_count;
} smd_channel_alloc_entry;

/* smd_edge_type */
/** @brief Flow smd_edge_type type.\n
  This structure defines the two processors connected by a given edge
  (i.e. SMD channel).
*/
typedef struct
{
  smem_host_type host0;                  /**< Processor 0. */
  smem_host_type host1;                  /**< Processor 1. */
} smd_edge_type;

static volatile smd_channel_alloc_entry *smd_channel_alloc_tbl;

/* These operations are used to manage the prop (ie. properties) bitfield.
   The bitfield is as follows:
   bits  0-7   - The ctype
   bits  8-11  - The xfrflow setting.  This should only be set if the port
                 has a STREAM protocol.
   bits 12-15  - The channel protocol
*/

/* mask out everything but the ctype bits */
#define SMD_CID_PROP_CTYPE_MASK(CTYPE_VAL)     (CTYPE_VAL & 0x000000FF)

/* shift a ctype value to the correct bit locations */
#define SMD_CID_PROP_CTYPE_SHFT(CTYPE_VAL)     (CTYPE_VAL)

/* mask out everything but the xfrflow bits */
#define SMD_CID_PROP_XFRFLOW_MASK(XFRFLOW_VAL) (XFRFLOW_VAL & 0x00000f00)

/* shift a xfrflow value to the correct bit locations */
#define SMD_CID_PROP_XFRFLOW_SHFT(XFRFLOW_VAL) (XFRFLOW_VAL << 8)

/* mask out everything but the prot (ie. protocol) bits */
#define SMD_CID_PROP_PROT_MASK(PROT_VAL)       (PROT_VAL & 0x0000f000)

/* shift a prot value to the correct bit locations */
#define SMD_CID_PROP_PROT_SHFT(PROT_VAL)       (PROT_VAL << 12)

/* Return the number of elements in an array */
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

#if defined(SMD_APPS_PROC) || defined(SMD_MODEM_PROC)
/*-------------------------------------------------------------------------
 *                    port name translation table
 *-------------------------------------------------------------------------*/
const char *smd_port_to_name[]=
{
  "DS",
  "LOOPBACK",
  "RPCCALL",
  "DUMMY"
};
#endif /* SMD_APPS_PROC || SMD_MODEM_PROC */

/* Defines the pair of processors in a given SMD channel in the system. */
static smd_edge_type smdi_edges[] =
{
  { SMEM_APPS,  SMEM_MODEM }, /* SMD_APPS_MODEM */
  { SMEM_APPS,  SMEM_ADSP  }, /* SMD_APPS_QDSP  */
  { SMEM_MODEM, SMEM_ADSP  }, /* SMD_MODEM_QDSP */
  { SMEM_APPS,  SMEM_DSPS  }, /* SMD_APPS_DSPS */
  { SMEM_MODEM, SMEM_DSPS  }, /* SMD_MODEM_DSPS */
  { SMEM_ADSP,  SMEM_DSPS  }, /* SMD_QDSP_DSPS */
  { SMEM_APPS,  SMEM_WCN   }, /* SMD_APPS_RIVA  */
  { SMEM_MODEM, SMEM_WCN   }, /* SMD_MODEM_RIVA */
  { SMEM_ADSP,  SMEM_WCN   }, /* SMD_QDSP_RIVA  */
  { SMEM_DSPS,  SMEM_WCN   }, /* invalid */
  { SMEM_APPS,  SMEM_RSVD5 }, /* invalid */
  { SMEM_MODEM, SMEM_RSVD5 }, /* invalid */
  { SMEM_ADSP,  SMEM_RSVD5 }, /* invalid */
  { SMEM_DSPS,  SMEM_RSVD5 }, /* invalid */
  { SMEM_WCN,   SMEM_RSVD5 }, /* invalid */
  { SMEM_APPS,  SMEM_RPM   }, /* SMD_APPS_RPM   */
  { SMEM_MODEM, SMEM_RPM   }, /* SMD_MODEM_RPM  */
  { SMEM_ADSP,  SMEM_RPM   }, /* SMD_QDSP_RPM   */
  { SMEM_WCN,   SMEM_RPM   }, /* SMD_RIVA_RPM   */
};

/*===========================================================================
                    LOCAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      smd_add_info

DESCRIPTION   Add an info structure to the appropriate list, based on the
              port's context.

ARGUMENTS     info - pointer to stream info structure

RETURN VALUE  None
===========================================================================*/
static void smd_add_info
(
  smd_info_type *info
)
{
  smd_info_type **head;

  ASSERT(info->context != NULL);

  if(info->context == NULL) 
    return;

  head = &info->context->open_list;

  info->next = *head;
  *head = info;
} /* smd_add_info */

/*===========================================================================
FUNCTION      smd_delete_info

DESCRIPTION   Delete an info structure from the appropriate list, based on the
              port's context.

ARGUMENTS     info - pointer to stream info structure

RETURN VALUE  None
===========================================================================*/
static void smd_delete_info
(
  smd_info_type *info
)
{
  smd_info_type *ptr;
  smd_info_type **head;

  ASSERT(info->context != NULL);

  if(info->context == NULL)
    return;

  head = &info->context->open_list;

  if(head == NULL)
    return;
  
  if( *head == info )
  {
    *head = info->next;
  }
  else
  {
    for( ptr = *head;
         ptr != NULL && ptr->next != info;
         ptr = ptr->next )
    {
      /* do nothing */
    }

    ASSERT( ptr != NULL );

    if ( ptr != NULL )
    {
       ptr->next = info->next;
    }
  }
  
} /* smd_delete_info */

/*===========================================================================
                    EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION      smdi_init

DESCRIPTION   Initializes the smd info structure and the SMD port number to
              smd info mapping structure.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void smdi_init( void )
{
  smd_info_type *info;
  uint32 idx;

  /* Initialize the local stream data */
  memset( smd_info, 0, sizeof(smd_info) );
  memset( smd_port_to_info, 0, sizeof(smd_port_to_info) );

  smd_channel_alloc_tbl = 
               (smd_channel_alloc_entry *)smem_alloc( SMEM_CHANNEL_ALLOC_TBL,
                          sizeof(smd_channel_alloc_entry) * SMD_NUM_CHANNELS);
  
  /* Initialize free queue and put each of the info structures on the queue */
  (void) q_init( &smd_info_free_q );

  for( idx = 0; idx < SMD_NUM_CHANNEL_INFO; idx++ )
  {
    info = &smd_info[idx];
    info->port_id = SMD_PORT_INVALID;
    (void) q_link( info, &info->link );
    q_put( &smd_info_free_q, &info->link );
  }

  /* Clear the external ports info */
  for( idx = 0; idx < SMEM_NUM_SMD_CHANNELS; idx++ )
  {
    smd_smem_port_info[ idx ].port_id = SMD_PORT_INVALID;
  }

} /* smdi_init */

/*===========================================================================
FUNCTION      smdi_alloc_info

DESCRIPTION   Allocates an smd info for the given SMD port id.

ARGUMENTS     port_id - SMD port to be used

RETURN VALUE  Pointer to smd info struct.

SIDE EFFECTS  None
===========================================================================*/
smd_info_type *smdi_alloc_info( smd_port_id_type port_id )
{
  smd_info_type *info;

  ASSERT( smd_port_to_info[port_id] == NULL );

  if(smd_port_to_info[port_id] != NULL)
    return NULL;

  info = (smd_info_type *) q_get( &smd_info_free_q );

  ASSERT( info != NULL );

  if(info == NULL)
    return NULL;

  if ( info != NULL )
  {
     /* Initialize entire structure to 0 */
     memset( info, 0, sizeof(smd_info_type) );
     (void) q_link( info, &info->link );
     info->port_id = port_id;
     smd_port_to_info[port_id] = info;
  }

  return info;
} /* smdi_alloc_info */

/*===========================================================================
FUNCTION      smdi_free_info

DESCRIPTION   Frees an smd info

ARGUMENTS     info - pointer to smd info structure

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void smdi_free_info( smd_info_type *info )
{
  ASSERT( smd_port_to_info[info->port_id] == info );

  smdi_delete_channel_info(info->port_id);
  
  smd_port_to_info[info->port_id] = NULL;
  info->port_id = SMD_PORT_INVALID;
  info->port_name[0]= '\0';
  info->context = NULL;
  q_put( &smd_info_free_q, &info->link );

} /* smdi_free_info */


/*===========================================================================
FUNCTION      smdi_is_stream_open

DESCRIPTION   Indicates if a stream is already opened for the port_id

ARGUMENTS     port_id - SMD port

RETURN VALUE  TRUE if this SMD port is open
              FALSE otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean smdi_is_stream_open( smd_port_id_type port_id )
{
  if( (smd_port_to_info[port_id] != NULL) &&
      (smd_port_to_info[port_id]->protocol == SMD_STREAMING_BUFFER) )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
} /* smdi_is_stream_open */

/*===========================================================================
FUNCTION      smdi_get_info

DESCRIPTION   Find an smd info allocated to the given port_id

ARGUMENTS     port_id - SMD port to use as a search key

RETURN VALUE  Pointer to smd info struct.

SIDE EFFECTS  
===========================================================================*/
smd_info_type *smdi_get_info( smd_port_id_type port_id )
{
  ASSERT( smd_port_to_info[port_id] != NULL );
  return smd_port_to_info[port_id];
} /* smdi_get_info */

/*===========================================================================
FUNCTION      smdi_get_stream_info

DESCRIPTION   Find an smd stream allocated to the given port_id

ARGUMENTS     port_id - SMD port to use as a search key

RETURN VALUE  Pointer to smd stream info struct.

SIDE EFFECTS  
===========================================================================*/
smd_stream_info_type *smdi_get_stream_info( smd_port_id_type port_id )
{
  ASSERT( (smd_port_to_info[port_id] != NULL) &&
          (smd_port_to_info[port_id]->protocol == SMD_STREAMING_BUFFER) );

  return &smd_port_to_info[port_id]->info.stream;
} /* smdi_get_stream_info */

/*===========================================================================
FUNCTION      smdi_event_send_update

DESCRIPTION   Set state updated flag, do memory barrier and then send update
              event to other processor.

ARGUMENTS     info - pointer to stream info structure

RETURN VALUE  None
===========================================================================*/
void smdi_event_send_update( smd_stream_info_type *info )
{
  SMD_WRITE_IF_SIG( info, SMD_TX_HDR, state_updated, TRUE );

  smd_event_send( info );
} /* smdi_event_send_update */

/*===========================================================================
FUNCTION      smdi_event_send_transmit

DESCRIPTION   Set data read flag, do memory barrier and then send
              transmit event to other processor.

              If the other processor's mask_recv_intr is set to 1, the other
              processor is not waiting to write data to the port, so SMD can
              safely suppress the interrupt.

ARGUMENTS     info - pointer to stream info structure

RETURN VALUE  None
===========================================================================*/
void smdi_event_send_transmit( smd_stream_info_type *info )
{
  if(SMD_READ_IF_SIG( info, SMD_RX_HDR, mask_recv_intr  ) == 0 )
  {
    SMD_WRITE_IF_SIG( info, SMD_TX_HDR, data_read, TRUE );  

    smd_event_send( info );
  }
} /* smdi_event_send_transmit */

/*===========================================================================
FUNCTION      smdi_event_send_receive

DESCRIPTION   Set data written flag, do memory barrier and then send
              receive event to other processor.

ARGUMENTS     info - pointer to stream info structure

RETURN VALUE  None
===========================================================================*/
void smdi_event_send_receive( smd_stream_info_type *info )
{
  SMD_WRITE_IF_SIG(info, SMD_TX_HDR, data_written, TRUE );  

  smd_event_send( info);
} /* smdi_event_send_receive */

/*===========================================================================
  Each stream uses a state machine to synchronize stream opening/closing and
  single processor restart. The diagram below shows the state transitions for
  this state machine. The state machine is implemented by the functions below:

  update event handler:
  smdi_stream_state_update

  main state machine routine:
  smdi_stream_state_machine

  routines which handle an event when in the given state:
  smdi_stream_state_closed
  smdi_stream_state_opening
  smdi_stream_state_opened
  smdi_stream_state_flushing
  smdi_stream_state_closing

  FIXME - THIS DIAGRAM IS OUT OF DATE

    This processor                         The other processor

    SMD_SS_CLOSED (initial state)          SMD_SS_CLOSED (initial state)
        |                                      |
        | <- smd_open called                   | <- smd_open called
        |    transmit side of                  |    transmit side of
        |    stream initialized                |    stream initialized
        |                                      |
        V                                      V
    SMD_SS_OPENING ----|              |----SMD_SS_OPENING
        |              V              |        |
        |              EITHER --------+------->|
        |              ^              |        |
        |              |              V        |
        |<-------------+-------- EITHER        |
        |              |              ^        |
        V              |              |        V
    SMD_SS_OPENED -----|              |----SMD_SS_OPENED
        |                                      |
        | <- smd_closed called                 | <- smd_closed called
        |                                      |
        V                                      V
    SMD_SS_FLUSHING                        SMD_SS_FLUSHING
        |                                      |
        | <- other processor                   | <- other processor
        |    received all data                 |    received all data
        |                                      |
        V                                      V
    SMD_SS_CLOSED                          SMD_SS_CLOSED
===========================================================================*/

/*===========================================================================
FUNCTION      smdi_stream_enter_opening_state

DESCRIPTION   Processing associated with entering the opening state.


ARGUMENTS     info - pointer to smd info structure

RETURN VALUE  None
===========================================================================*/
static void smdi_stream_enter_opening_state( smd_info_type *info )
{
  smd_stream_info_type *sinfo;

  sinfo = &info->info.stream;

  /* Initialize shared memory info structure */
  sinfo->prev_dtr = FALSE;
  sinfo->prev_cd  = FALSE;
  sinfo->prev_rts = FALSE;
  sinfo->prev_ri  = FALSE;

  /* - 4 because write_index == read_index => buffer is empty 
     - 4 instead of - 1 to try to keep transfers word aligned */
  sinfo->max_queued_data = sinfo->fifo_sz - 4;

  /* Initialize the xfr mode specific fields */
  sinfo->stream_init( sinfo );

  SMD_WRITE_IF_SIG( sinfo, SMD_TX_HDR, data_written, FALSE );
  SMD_WRITE_IF_SIG( sinfo, SMD_TX_HDR, data_read, FALSE );
  SMD_WRITE_IF_SIG( sinfo, SMD_TX_HDR, state_updated, FALSE );

  /*----------------------------------------------------------------------
   * Must set stream_state last because once the stream state is not CLOSED,
   * the other processor expects the fields in shared memory to be valid.
   *----------------------------------------------------------------------*/
  SMD_WRITE_CH_HDR( sinfo, SMD_TX_HDR, stream_state, SMD_SS_OPENING );
} /* smdi_stream_enter_opening_state */

/*===========================================================================
FUNCTION      smdi_stream_enter_closed_state

DESCRIPTION   Processing associated with entering the closed state.

              Sets the state in shared memory to CLOSED and waits for the
              other processor to enter the CLOSING before calling the closed
              callback and freeing the local data structures.

ARGUMENTS     info - pointer to smd info structure

RETURN VALUE  None
===========================================================================*/
static void smdi_stream_enter_closed_state( smd_info_type *info )
{
  smd_stream_info_type *sinfo;
  smd_close_callback_fn close_cb;
  void *close_cb_data;
  uint32 curr_remote_state;

  sinfo = &info->info.stream;

  /* Set shared memory info structure to closed state */
  SMD_WRITE_CH_HDR( sinfo, SMD_TX_HDR, stream_state, SMD_SS_CLOSED );
  SMD_CLR_IF_SIGS( sinfo, SMD_TX_HDR );

  smdi_event_send_update( sinfo );

  /* Check if the remote processor did not open the port, this will indicate
     whether the host processor has to do clean-up at this point or
     wait for the remote close event from the other processor. */
  curr_remote_state =
    SMD_READ_CH_HDR( sinfo, SMD_RX_HDR, stream_state );
  if( ( curr_remote_state == SMD_SS_CLOSED ) )
  {
    /* Remove the port from the open list */
    smd_delete_info( info );

    close_cb = sinfo->close_cb;
    close_cb_data = sinfo->close_cb_data;

    /* Free the port info data structure */
    smdi_free_info( info );

    if( close_cb )
    {
      close_cb( close_cb_data );
    }
  }
} /* smdi_stream_enter_closed_state */


/*===========================================================================
  FUNCTION  smdi_stream_handle_remote_reset
===========================================================================*/
/**
  This function performs the necessary steps to move the stream into
  reset state, triggered by external events such as Sub system Restart.
 
  For SSR, apps processor is considered to be the only stable entity, so
  it is in charge of setting the stream state to CLOSED; clearing the
  read index and the control signals on the RX side; and clearing
  the write index of TX side.

  @param[in] info        Pointer to the SMD info structure. 
 
  @return
  None.

*/
/*=========================================================================*/
static void smdi_stream_handle_remote_reset( smd_info_type *info )
{
  smd_stream_info_type *sinfo;

  sinfo = &info->info.stream;

  /* For SSR, set the remote stream state to CLOSED */
  SMD_WRITE_CH_HDR( sinfo, SMD_RX_HDR, stream_state, SMD_SS_CLOSED );

  /* Clear out the signals */
  SMD_CLR_IF_SIGS( sinfo, SMD_RX_HDR);

  /* Clear out the read and write indexes */
  SMD_WRITE_CH_HDR( sinfo, SMD_RX_HDR, read_index, 0 );
  SMD_WRITE_CH_HDR( sinfo, SMD_TX_HDR, write_index, 0 );
}

/*===========================================================================
FUNCTION      smdi_stream_state_closed

DESCRIPTION   Handle events which occur while in the closed state.

ARGUMENTS     info - pointer to smd info structure

RETURN VALUE  event - current event or new event
===========================================================================*/
static uint32 smdi_stream_state_closed
(
  smd_info_type *info,
  uint32         event
)
{
  smd_stream_info_type *sinfo;
  smd_close_callback_fn close_cb;
  void *close_cb_data;

  sinfo = &info->info.stream;

  switch( event )
  {
  case SMD_EVENT_OPEN:
    smd_add_info( info );

    /* smd_sm_remote_reset:
          FALSE, if remote side SMD is ready
          TRUE,  if remote side SMD is still held in RESET */
    if(smd_sm_remote_reset)
    {
      SMD_WRITE_CH_HDR( sinfo, SMD_TX_HDR, stream_state, SMD_SS_RESET_OPENING );
    }
    else
    {
      uint32 curr_remote_state;
      
      /* enter opening state */
      smdi_stream_enter_opening_state( info );
      curr_remote_state = 
          SMD_READ_CH_HDR( sinfo, SMD_RX_HDR, stream_state );

      /* If the other side is not in CLOSED, CLOSING or RESET states, we
         may have missed the update interrupt, so generate a REMOTE_OPEN
         event now. */
      if( (curr_remote_state != SMD_SS_CLOSED ) &&
          (curr_remote_state != SMD_SS_CLOSING ) &&
          (curr_remote_state != SMD_SS_RESET ) )
      {
        event = SMD_EVENT_REMOTE_OPEN;
      }
    }
    break;

  case SMD_EVENT_REMOTE_CLOSE:
    /* Now that the other processor has entered its CLOSING or CLOSED state,
     * notify the client that the port is fully closed (on this end), and can
     * now be reopened. */

    /* Remove the port from the open list */
    smd_delete_info( info );

    close_cb = sinfo->close_cb;
    close_cb_data = sinfo->close_cb_data;

    /* Free the port info data structure */
    smdi_free_info( info );

    /* Call the close callback, if any */
    if( close_cb )
    {
      close_cb( close_cb_data );
    }
    break;

    default:
      /* ignore other events */
      break;
  }

  return event;
} /* smdi_stream_state_closed */

/*===========================================================================
FUNCTION      smdi_stream_state_opening

DESCRIPTION   Handle events which occur while in the opening state.

ARGUMENTS     info - pointer to smd info structure

RETURN VALUE  event - current event or new event
===========================================================================*/
static uint32 smdi_stream_state_opening
(
  smd_info_type *info,
  uint32         event
)
{
  smd_stream_info_type *sinfo;

  sinfo = &info->info.stream;

  switch( event ) {
    case SMD_EVENT_CLOSE:
      /* enter closed state */
      smdi_stream_enter_closed_state( info );
      break;

    case SMD_EVENT_REMOTE_OPEN:
      /* enter opened (or flushing) state */
      if( sinfo->flush_pending )
      {
        sinfo->flush_pending = FALSE;
        SMD_WRITE_CH_HDR( sinfo, SMD_TX_HDR, stream_state, SMD_SS_FLUSHING );
      }
      else
      {
        SMD_WRITE_CH_HDR( sinfo, SMD_TX_HDR, stream_state, SMD_SS_OPENED );
        /* Invoke the stream open callback if registered */
        if(sinfo->stream_open)
        {
          sinfo->stream_open(sinfo);
        }
       }

#ifdef FEATURE_SMD
      /* send and recieve any pending data */
      if( sinfo->read_pending )
      {
        sinfo->read_pending = FALSE;
        smd_notify_read( sinfo->port_id );
      }

      if( sinfo->transmit_pending )
      {
        sinfo->transmit_pending = FALSE;
        smd_notify_write( sinfo->port_id );
      }
#endif
      break;

  case SMD_EVENT_REMOTE_RESET:
      smdi_stream_handle_remote_reset( info );
      break;

    case SMD_EVENT_FLUSH:
      /* Not opened, can't enter flushing - just note flush is pending */
      sinfo->flush_pending = TRUE;
      break;

    default:
      /* ignore other events */
      break;
  }

  return event;
} /* smdi_stream_state_opening */

/*===========================================================================
FUNCTION      smdi_stream_state_opened

DESCRIPTION   Handle events which occur while in the opened state.

ARGUMENTS     info - pointer to smd info structure

RETURN VALUE  event - current event or new event
===========================================================================*/
static uint32 smdi_stream_state_opened
(
  smd_info_type *info,
  uint32         event
)
{
  smd_stream_info_type *sinfo;

  sinfo = &info->info.stream;

  switch( event ) {
    case SMD_EVENT_CLOSE:
      /* enter closed state */
      smdi_stream_enter_closed_state( info );
      break;

    case SMD_EVENT_REMOTE_CLOSE:
      /* enter opening state */
      SMD_WRITE_CH_HDR( sinfo, SMD_TX_HDR, stream_state, SMD_SS_CLOSING );
      break;

    case SMD_EVENT_FLUSH:
      /* enter flushing state */
      SMD_WRITE_CH_HDR( sinfo, SMD_TX_HDR, stream_state, SMD_SS_FLUSHING );
      /* Clear the mask interrupt flag to ensure that the remote processor
       * will notify us when it reads data. */
      SMD_WRITE_IF_SIG( sinfo, SMD_TX_HDR, mask_recv_intr, 0 );
      break;

  case SMD_EVENT_REMOTE_RESET:
      /* enter reset state */
      SMD_WRITE_SMEM_4( &( sinfo->tx_shared_info_ptr->stream_state ), 
                        SMD_SS_RESET );

      smdi_stream_handle_remote_reset( info );
      break;

    default:
      /* ignore other event */
      break;
  }

  return event;
} /* smdi_stream_state_opened */

/*===========================================================================
FUNCTION      smdi_stream_state_flushing

DESCRIPTION   Handle events which occur while in the flushing state.

ARGUMENTS     info - pointer to smd info structure

RETURN VALUE  event - current event or new event
===========================================================================*/
static uint32 smdi_stream_state_flushing
(
  smd_info_type *info,
  uint32         event
)
{
  smd_stream_info_type *sinfo;

  sinfo = &info->info.stream;

  switch( event ) {
    case SMD_EVENT_CLOSE:
      sinfo->close_pending = TRUE;
      break;

    case SMD_EVENT_FLUSH_COMPLETE:
      SMD_CALL_FLUSH_CB( sinfo );

      if( sinfo->close_pending )
      {
        /* enter closed state */
        sinfo->close_pending = FALSE;
        smdi_stream_enter_closed_state( info );
      }
      else
      {
        SMD_WRITE_CH_HDR( sinfo, SMD_TX_HDR, stream_state, SMD_SS_OPENED );
        /* Invoke the stream open callback if registered */
        if(sinfo->stream_open)
        {
          sinfo->stream_open(sinfo);
        }
      }
      break;

    case SMD_EVENT_REMOTE_CLOSE:
      /* set the state to closing, the flush callback should
         handle the rest */
      SMD_CALL_FLUSH_CB( sinfo );

      if( sinfo->close_pending )
      {
        /* enter closed state */
        sinfo->close_pending = FALSE;
        smdi_stream_enter_closed_state( info );
      }
      else
      {
        SMD_WRITE_CH_HDR( sinfo, SMD_TX_HDR, stream_state, SMD_SS_CLOSING );
      }

      break;

    case SMD_EVENT_REMOTE_RESET:
      smdi_stream_handle_remote_reset( info );
      break;

    default:
      /* ignore other events */
      break;
  }

  return event;
} /* smdi_stream_state_flushing */

/*===========================================================================
FUNCTION      smdi_stream_state_closing

DESCRIPTION   Handle events which occur while in the closing state.

ARGUMENTS     info - pointer to smd info structure

RETURN VALUE  event - current event or new event
===========================================================================*/
static uint32 smdi_stream_state_closing
(
  smd_info_type *info,
  uint32         event
)
{
  smd_stream_info_type *sinfo;

  sinfo = &info->info.stream;

  switch( event ) {
    case SMD_EVENT_CLOSE:
      /* enter closed state */
      smdi_stream_enter_closed_state( info );
      break;
      
    case SMD_EVENT_REMOTE_OPEN:
    case SMD_EVENT_REMOTE_REOPEN:
      /* enter opening state */
      smdi_stream_enter_opening_state( info );
      event = SMD_EVENT_REMOTE_OPEN;
      break;

    case SMD_EVENT_REMOTE_RESET:
      smdi_stream_handle_remote_reset( info );
      break;

    case SMD_EVENT_FLUSH:
      SMD_CALL_FLUSH_CB( sinfo );
      break;

    default:
      /* ignore other events */
      break;
  }

  return event;
} /* smdi_stream_state_closing */

/*===========================================================================
FUNCTION      smdi_stream_state_reset

DESCRIPTION   Handle events which occur while in the reset state.

ARGUMENTS     info - pointer to smd info structure

RETURN VALUE  event - current event or new event
===========================================================================*/
static uint32 smdi_stream_state_reset
(
  smd_info_type *info,
  uint32         event
)
{
  smd_stream_info_type *sinfo;

  sinfo = &info->info.stream;

  switch( event )
  {
  case SMD_EVENT_CLOSE:
    /* some clients will generate a CLOSE event upon processing dtr
       callback.(We should get an OPEN event later) */
    smdi_stream_enter_closed_state( info );

    break;

  case SMD_EVENT_REMOTE_OPEN:
  case SMD_EVENT_REMOTE_REOPEN:
    /* some clients don't generate a CLOSE event */
    smdi_stream_enter_opening_state( info );
    event = SMD_EVENT_REMOTE_OPEN;

    break;

  case SMD_EVENT_REMOTE_CLOSE:
    /* enter closing state */
    SMD_WRITE_CH_HDR( sinfo, SMD_TX_HDR, stream_state, SMD_SS_CLOSING );
    break;
    
  default:
    /* ignore other events */
    break;
  }

  return event;
} /* smdi_stream_state_reset */

/*===========================================================================
FUNCTION      smdi_stream_state_reset_opening

DESCRIPTION   Handle events which occur while in the reset opening state.

ARGUMENTS     info - pointer to smd info structure

RETURN VALUE  event - current event or new event
===========================================================================*/
static uint32 smdi_stream_state_reset_opening
(
  smd_info_type *info,
  uint32         event
)
{
  switch( event )
  {
  case SMD_EVENT_REMOTE_OPEN:
  case SMD_EVENT_REMOTE_REOPEN:
  case SMD_EVENT_REMOTE_CLOSE:
    smdi_stream_enter_opening_state( info );
    event = SMD_EVENT_REMOTE_OPEN;
    break;
    
  default:
    break;
  }
  
  return event;
} /* smdi_stream_state_reset_opening */

/*===========================================================================
  FUNCTION  smd_get_other_host
===========================================================================*/
/**
  This function translates destination host from channel type.
 
  @param[in] this_host       The source of the connection
  @param[in] channel         Channel of interest in the target

  @return
  The destination processor for the specified channel.

  @dependencies
  None.

  @sideeffects
  None.
*/
/*=========================================================================*/
static smem_host_type smd_get_other_host( smem_host_type this_host, smd_channel_type channel )
{
  smem_host_type host = SMEM_INVALID_HOST;
  smd_edge_type *edge;

  /* Validate channel */
  ASSERT( channel < SMD_CHANNEL_TYPE_LAST );

  /* Point to channel of interest */
  edge = &( smdi_edges[ channel ] );

  /* Get the other host */
  host = ( this_host ==  edge->host0 ) ?  edge->host1 : edge->host0;

  return host;
}

/*===========================================================================
FUNCTION      smdi_stream_state_machine

DESCRIPTION   Handle events which occur while in any state. Calls the
              appropriate handler based on the current state. It continues
              calling handlers until no new event is generated.

ARGUMENTS     info - pointer to stream info structure

RETURN VALUE  None
===========================================================================*/
void smdi_stream_state_machine( smd_stream_info_type *sinfo, uint32 event )
{
  uint32 stream_state;
  uint32 prev_event;
  smd_info_type *info;
  smd_port_id_type port_id;

  port_id = sinfo->port_id;
  info = smdi_get_info( port_id );

  do {
    prev_event = event;
    stream_state = 
         SMD_READ_CH_HDR( sinfo, SMD_TX_HDR, stream_state );

    SMD_INTERNAL_LOG_EVENT( SMEM_LOG_EVENT_SS,
                            (uint32) port_id,
                            stream_state,
                            event );

    if(event == SMD_EVENT_REMOTE_CLOSE)
    {
      if(sinfo->stream_close)
      {
        sinfo->stream_close(sinfo);
      }
    }

    switch( stream_state ) {
      case SMD_SS_CLOSED:
        event = smdi_stream_state_closed( info, event );
        break;

      case SMD_SS_OPENING:
        event = smdi_stream_state_opening( info, event );
        break;

      case SMD_SS_OPENED:
        event = smdi_stream_state_opened( info, event );
        break;

      case SMD_SS_FLUSHING:
        event = smdi_stream_state_flushing( info, event );
        break;

      case SMD_SS_CLOSING:
        event = smdi_stream_state_closing( info, event );
        break;

      case SMD_SS_RESET:
        event = smdi_stream_state_reset( info, event );
        break;
          
      case SMD_SS_RESET_OPENING:
        event = smdi_stream_state_reset_opening( info, event );
        break;

      default:
        ERR_FATAL( "Invalid stream state %d %d",
                   info->port_id, stream_state, 0 );
        //break;
    }
  } while( prev_event != event );

  /* This port info could have been freed if it transitioned into the 
     closed state.  We can only send an update if this port still
     exists */
  if (smdi_is_stream_open( port_id))
  {
    smdi_event_send_update( sinfo );
  }
} /* smdi_stream_state_machine */

/*===========================================================================
FUNCTION      smdi_stream_state_update

DESCRIPTION   Handle update event from other processor. Generates an stream
              state machine event based on the change in the stream state on
              the other processor.

ARGUMENTS     info - pointer to stream info structure

RETURN VALUE  None
===========================================================================*/
void smdi_stream_state_update( smd_stream_info_type *info )
{
  uint32 curr_remote_state;
  uint32 prev_remote_state;
  uint32 curr_local_state;

  prev_remote_state = info->prev_remote_state;
  curr_remote_state =
      SMD_READ_CH_HDR( info, SMD_RX_HDR, stream_state );

  SMD_INTERNAL_LOG_EVENT( SMEM_LOG_EVENT_RS,
                          (uint32)info->port_id,
                          prev_remote_state,
                          curr_remote_state );

  /* no state change => no event, so just return */
  if( prev_remote_state == curr_remote_state )
  {
    return;
  }

  /* handle special cases */
  if( ( curr_remote_state == SMD_SS_CLOSED ) ||
      ( curr_remote_state == SMD_SS_CLOSING ) )
  {
    /* New state is closed, doesn't matter what old state was */
    smdi_stream_state_machine( info, SMD_EVENT_REMOTE_CLOSE );
  }
  else if(curr_remote_state == SMD_SS_RESET)
  {
    /* New state is reset, doesn't matter what old state was */
    smdi_stream_state_machine( info, SMD_EVENT_REMOTE_RESET );
  }
  else
  {  
    switch( prev_remote_state ) {
    case SMD_SS_OPENING:
      if( ( curr_remote_state == SMD_SS_OPENED ) ||
          ( curr_remote_state == SMD_SS_FLUSHING ) )
      {
        smdi_stream_state_machine( info, SMD_EVENT_REMOTE_OPEN );
      }
      else
      {
        SMD_INTERNAL_LOG_EVENT( SMEM_LOG_EVENT_RS,
                                (uint32)info->port_id,
                                prev_remote_state,
                                curr_remote_state );
      }
      break;
      
    case SMD_SS_OPENED:
      if( curr_remote_state == SMD_SS_FLUSHING )
      {
        curr_local_state =
            SMD_READ_CH_HDR( info, SMD_TX_HDR, stream_state );
        
        if( (curr_local_state == SMD_SS_CLOSING ) ||
            (curr_local_state == SMD_SS_RESET )   ||
            (curr_local_state == SMD_SS_RESET_OPENING ) )
        {
          /* local is in CLOSING, RESET, RESET_OPENING state,
             this is a REMOTE_REOPEN */
          smdi_stream_state_machine(info, SMD_EVENT_REMOTE_REOPEN );
        }
        else
        {
          /* local is in CLOSED, OPENING, OPEN or FLUSHING state,
             this is a REMOTE_OPEN */
          smdi_stream_state_machine(info, SMD_EVENT_REMOTE_OPEN );
        }
      }
      else
      {
        SMD_INTERNAL_LOG_EVENT( SMEM_LOG_EVENT_RS,
                                (uint32)info->port_id,
                                prev_remote_state,
                                curr_remote_state );
      }
      break;

    case SMD_SS_FLUSHING:
      if( curr_remote_state == SMD_SS_OPENED )
      {
        curr_local_state =
            SMD_READ_CH_HDR( info, SMD_TX_HDR, stream_state );
        
        if( (curr_local_state == SMD_SS_CLOSING ) ||
            (curr_local_state == SMD_SS_RESET )   ||
            (curr_local_state == SMD_SS_RESET_OPENING ) )
        {
          /* local is in CLOSING, RESET, RESET_OPENING state,
             this is a REMOTE_REOPEN */
          smdi_stream_state_machine(info, SMD_EVENT_REMOTE_REOPEN );
        }
        else
        {
          /* local is in CLOSED, OPENING, OPEN or FLUSHING state,
             this is a REMOTE_OPEN */
          smdi_stream_state_machine(info, SMD_EVENT_REMOTE_OPEN );
        }
      }
      else
      {
        SMD_INTERNAL_LOG_EVENT( SMEM_LOG_EVENT_RS,
                                (uint32)info->port_id,
                                prev_remote_state,
                                curr_remote_state );
      }
      break;

    case SMD_SS_CLOSING:
      if( ( curr_remote_state == SMD_SS_OPENED )  ||
          ( curr_remote_state == SMD_SS_OPENING ) ||
          ( curr_remote_state == SMD_SS_FLUSHING ) )
      {
        smdi_stream_state_machine( info, SMD_EVENT_REMOTE_OPEN );
      }
      else
      {
        SMD_INTERNAL_LOG_EVENT( SMEM_LOG_EVENT_RS,
                                (uint32)info->port_id,
                                prev_remote_state,
                                curr_remote_state );
      }
      break;

    case SMD_SS_CLOSED:
      if( ( curr_remote_state == SMD_SS_OPENING ) ||
          ( curr_remote_state == SMD_SS_OPENED )  ||
          ( curr_remote_state == SMD_SS_FLUSHING ) )
      {
        curr_local_state = 
            SMD_READ_CH_HDR( info, SMD_TX_HDR, stream_state );

        if( (curr_local_state == SMD_SS_CLOSING ) ||
            (curr_local_state == SMD_SS_RESET )   ||
            (curr_local_state == SMD_SS_RESET_OPENING ) )
        {
          /* local is in CLOSING, RESET, RESET_OPENING state,
             this is a REMOTE_REOPEN */
          smdi_stream_state_machine(info, SMD_EVENT_REMOTE_REOPEN );
        }
        else
        {
          smdi_stream_state_machine( info, SMD_EVENT_REMOTE_OPEN );
        }
      }
      else if ( curr_remote_state == SMD_SS_RESET_OPENING )
      {
        smdi_stream_state_machine( info, SMD_EVENT_REMOTE_CLOSE );
      }
      break;

    case SMD_SS_RESET:
      if( ( curr_remote_state == SMD_SS_OPENED )  ||
          ( curr_remote_state == SMD_SS_OPENING ) ||
          ( curr_remote_state == SMD_SS_FLUSHING ) )
        {
          smdi_stream_state_machine( info, SMD_EVENT_REMOTE_OPEN );
        }
      else
        {
        SMD_INTERNAL_LOG_EVENT( SMEM_LOG_EVENT_RS,
                                (uint32)info->port_id,
                                prev_remote_state,
                                curr_remote_state );
      }
      break;

    case SMD_SS_RESET_OPENING:
      if( ( curr_remote_state == SMD_SS_OPENING ) ||
          ( curr_remote_state == SMD_SS_OPENED ) ||
          ( curr_remote_state == SMD_SS_FLUSHING ) )
      {
        smdi_stream_state_machine( info, SMD_EVENT_REMOTE_OPEN );
      }
      else
      {
         /* log the unexpected state changes */
        SMD_INTERNAL_LOG_EVENT( SMEM_LOG_EVENT_RS,
                                (uint32)info->port_id,
                                prev_remote_state,
                                curr_remote_state );
      }
      break;

    default:
      ERR_FATAL( "Invalid stream state %d %d", info->port_id,
                 prev_remote_state, curr_remote_state );
      //break;
    }
  }

  info->prev_remote_state = curr_remote_state;
  return;
} /* smdi_stream_state_update */

/*===========================================================================
FUNCTION      smdi_get_port_name_ex

DESCRIPTION   maps the port id to the port name.

ARGUMENTS     port id 

RETURN VALUE  Returns the port name
===========================================================================*/
const char *smdi_get_port_name_ex(smd_port_id_type port_id)
{
  smd_info_type *info;

  ASSERT(port_id < ARRAY_SIZE(smd_port_to_info));

  info = smd_port_to_info[port_id];
  ASSERT(info != NULL);

  return(info->port_name);
  
}/* smdi_get_port_name */

#if defined(FEATURE_SMD) && (defined(SMD_APPS_PROC) || defined(SMD_MODEM_PROC)) 
/*===========================================================================
FUNCTION      smdi_get_port_name

DESCRIPTION   This function translates the port id into a port name for
              the port opened using port_id.

ARGUMENTS     port_id   port id.

RETURN VALUE  Returns the port name. 
              
SIDE EFFECTS  This should be removed when cleaning up the smd_port_to_name
===========================================================================*/
const char *smdi_get_port_name(smd_port_id_type port_id)
{
  ASSERT(port_id < ARRAY_SIZE(smd_port_to_name));

  return smd_port_to_name[port_id];
} /* smdi_get_port_name */
#endif

/*===========================================================================
FUNCTION      smdi_add_channel_info

DESCRIPTION   This function updates the smd_channel_alloc_tbl for the
              specified channel.

ARGUMENTS     cid                 channel id
              name                channel name 
              channel_type        channel connection type
              protocol            channel protocol type
              smd_xfrflow_type    channel xfrflow type

RETURN VALUE  None 
              
SIDE EFFECTS  None
===========================================================================*/
void smdi_add_channel_info
(
  uint32                     cid,
  const char                *name,
  smd_channel_type           channel_type,
  smd_channel_protocol_type  protocol,
  smd_xfrflow_type           xfrflow
)
{
  volatile char  *channel_name = smd_channel_alloc_tbl[cid].name;
  
  smem_spin_lock( SMEM_SPINLOCK_SMEM_ALLOC );
  
  /* if name entry is empty, add it */
  if (!(*channel_name))
  {
    smd_channel_alloc_tbl[cid].cid  = cid;
    smd_string_copy((char *)channel_name, name, SMD_CHANNEL_NAME_SIZE_MAX);

    smd_channel_alloc_tbl[cid].prop |= SMD_CID_PROP_CTYPE_SHFT(channel_type);
    smd_channel_alloc_tbl[cid].prop |= SMD_CID_PROP_PROT_SHFT(protocol);
    smd_channel_alloc_tbl[cid].prop |= SMD_CID_PROP_XFRFLOW_SHFT(xfrflow);
  }

  smd_channel_alloc_tbl[cid].ref_count++;
 
  smem_spin_unlock( SMEM_SPINLOCK_SMEM_ALLOC );

  if( SMD_CID_PROP_PROT_MASK(smd_channel_alloc_tbl[cid].prop) !=
      (uint32) SMD_CID_PROP_PROT_SHFT(protocol) )
  {
    /* need to make sure the protocols match */
    ERR_FATAL("smdi_add_channel_info: channel %d protocol mismatch", cid, 0, 0);
  }

  if( xfrflow != SMD_NO_XFRFLOW &&
     (SMD_CID_PROP_XFRFLOW_MASK(smd_channel_alloc_tbl[cid].prop) !=
      (uint32) SMD_CID_PROP_XFRFLOW_SHFT(xfrflow)) )
  {
    /* need to make sure the xfrflow type matches, except for case where it 
     * is specified as SMD_NO_XFRFLOW 
     */
    ERR_FATAL("smdi_add_channel_info: channel %d xfrflow type mismatch",
               cid, 0, 0);
  }
  
} /* smdi_add_channel_info */

/*===========================================================================
FUNCTION      smdi_delete_channel_info

DESCRIPTION   This function deletes the channel information

ARGUMENTS     cid  channel id

RETURN VALUE  None

SIDE EFFECTS  Leave the entry in the smd_channel_alloc_tbl even if the
              channel is closed.
===========================================================================*/
void smdi_delete_channel_info
(
  uint32      cid
)
{
  smem_spin_lock( SMEM_SPINLOCK_SMEM_ALLOC );
  
  ASSERT(smd_channel_alloc_tbl[cid].ref_count != 0);  
  
  smd_channel_alloc_tbl[cid].ref_count--;
  
  smem_spin_unlock( SMEM_SPINLOCK_SMEM_ALLOC );
  
} /* smdi_delete_channel_info */

/*===========================================================================
FUNCTION      smdi_alloc_channel_info

DESCRIPTION   This function allocates/populates the channel info in the
              smd_channel_alloc_tbl for the specified channel. 

ARGUMENTS     name           channel name 
              channel_type   channel connection type
              protocol       channel protocol type

RETURN VALUE  it returns the channel id if successful else SMD_PORT_INVALID.

SIDE EFFECTS  None
===========================================================================*/
uint32 smdi_alloc_channel_info
(
  const char                *name,
  smd_channel_type           channel_type,
  smd_channel_protocol_type  protocol,
  smd_xfrflow_type           xfrflow
)
{
  uint32        idx;
  uint32        cid = SMD_PORT_INVALID;
  char          cname[SMD_CHANNEL_NAME_SIZE_MAX]; /* channel name */
  uint32        cprop = 0;                        /* channel property */  
  uint32        error_type = 0;
  
#if defined(SMD_APPS_PROC) || defined(SMD_MODEM_PROC)
  /* Make sure that this name is not a backwords compatibe name */
  /* (non apps-modem channels are given a dynamic id) */
  if(channel_type == SMD_APPS_MODEM)
  {
    for ( idx =  0; idx < SMD_NUM_PORTS; idx++ )
    {
      /* Compare only SMD_CHANNEL_NAME_SIZE_MAX-1 characters, since the 
       * argument name may be longer, but already-allocated port names always 
       * have null for the last character. */
      if( !strncmp( smd_port_to_name[idx], name, SMD_CHANNEL_NAME_SIZE_MAX-1 ) )
      {
        /* this is a backwords compatible name, add the channel and return
         * the index */ 
        smdi_add_channel_info( idx, name, channel_type, protocol, xfrflow );
        return idx;
      }
    }
  }
#endif /* SMD_APPS_PROC || SMD_MODEM_PROC */

  smem_spin_lock( SMEM_SPINLOCK_SMEM_ALLOC );

  for ( idx = SMD_NUM_PORTS; idx < SMEM_NUM_SMD_CHANNELS; idx++ )
  {
    /* If a non-empty entry, check to see if it matches the to be allocated */
    if (*smd_channel_alloc_tbl[idx].name)
    {
      /* copy name to local so we can compare */
      SMD_MEMCPY_FROM_SMEM( cname,
                            (void *)smd_channel_alloc_tbl[idx].name,
                            SMD_CHANNEL_NAME_SIZE_MAX );

      cprop = SMD_READ_SMEM_4(&(smd_channel_alloc_tbl[idx].prop));

      /* Check if the name and property are the same.  */
      /* Compare only SMD_CHANNEL_NAME_SIZE_MAX-1 characters, since the 
       * argument name may be longer, but already-allocated port names always 
       * have null for the last character. */
      if ( (!strncmp( cname, name, SMD_CHANNEL_NAME_SIZE_MAX-1)) &&
           (SMD_CID_PROP_CTYPE_MASK(cprop) ==
            (uint32) SMD_CID_PROP_CTYPE_SHFT(channel_type)) )
           
      {
        /* Save the channel */
        cid = idx;

        /* If the port is already opened with an unspecified transfer type,
         * set the transfer type now. This will happen if the other processor
         * opened the SMD port using the SMD memcpy API (which does not set
         * the transfer flow) and this processor is opening the SMD port using
         * the SMD Lite API or the SMD DSM API (both of which set the transfer
         * flow). */
        if( SMD_CID_PROP_XFRFLOW_MASK(cprop) ==
            SMD_CID_PROP_XFRFLOW_SHFT(SMD_NO_XFRFLOW) )
        {
          cprop |= SMD_CID_PROP_XFRFLOW_SHFT(xfrflow);
          SMD_WRITE_SMEM_4( &(smd_channel_alloc_tbl[cid].prop), cprop );
        }

        /* the protocol OR xfrflow type does not match */
        if((xfrflow != SMD_NO_XFRFLOW && 
            SMD_CID_PROP_XFRFLOW_MASK(cprop) != (uint32) SMD_CID_PROP_XFRFLOW_SHFT(xfrflow)) ||
           (SMD_CID_PROP_PROT_MASK(cprop) != (uint32) SMD_CID_PROP_PROT_SHFT(protocol))
          )
        {
          error_type = SMDI_CHANNEL_PROPERTIES_MISMATCH;
        }
        /* Found match */
        else 
        {
          smd_channel_alloc_tbl[idx].ref_count++;
        }
        break;
      }
    }
    else 
    {
      /* remember the first available slot */
      if(cid == SMD_PORT_INVALID)
      {
        cid = idx;
      }
    }
  }/* for ( idx = SMD_NUM_PORTS; idx < SMEM_NUM_SMD_CHANNELS; idx++ ) */

  /* entry not found in the table */
  if( idx == SMEM_NUM_SMD_CHANNELS )
  {
    /* there is an empty slot */
    if ( cid != SMD_PORT_INVALID )
    {
      smd_channel_alloc_tbl[cid].cid  = cid;
      
      smd_string_copy((char *)smd_channel_alloc_tbl[cid].name,
                  name,
                  SMD_CHANNEL_NAME_SIZE_MAX);
      
      cprop = SMD_CID_PROP_CTYPE_SHFT(channel_type) |
              SMD_CID_PROP_PROT_SHFT(protocol) |
              SMD_CID_PROP_XFRFLOW_SHFT(xfrflow);
      
      smd_channel_alloc_tbl[cid].prop = cprop;
      
      smd_channel_alloc_tbl[cid].ref_count++;
    }
    else /* there is no empty slot */
    {
      error_type = SMDI_CHANNEL_TABLE_FULL;
    }      
  }

  smem_spin_unlock( SMEM_SPINLOCK_SMEM_ALLOC );

  if( SMDI_CHANNEL_PROPERTIES_MISMATCH == error_type )
  {
    ERR_FATAL("smdi_alloc_channel_info: channel %d protocol mismatch",
        cid, 0, 0);
  }
  else if( SMDI_CHANNEL_TABLE_FULL == error_type )
  {
    ERR_FATAL("smdi_alloc_channel_info: SMD channel table is full", 0, 0, 0);
  }
  
  return(cid);
} /* smdi_alloc_channel_info */

/*===========================================================================
FUNCTION      smdi_get_channel_id

DESCRIPTION   Look up the channel id from the smd_channel_alloc_tbl
              using port name and connection type.  

ARGUMENTS     name           channel name 
              channel_type   channel connection type
              
DEPENDENCIES  None

RETURN VALUE  returns the channel id if found.
              returns -1 if not found.

SIDE EFFECTS  None
===========================================================================*/
int32 smdi_get_channel_id
(
  const char       *name,
  smd_channel_type  channel_type
)
{
  int32   idx;
  char    cname[SMD_CHANNEL_NAME_SIZE_MAX];
  
  smem_spin_lock( SMEM_SPINLOCK_SMEM_ALLOC );
  
  for ( idx = 0; idx < SMEM_NUM_SMD_CHANNELS; idx++ )
  {
    /* if name is not empty, check to see if match the to be allocated */
    if (*smd_channel_alloc_tbl[idx].name)
    {
      SMD_MEMCPY_FROM_SMEM( cname,
                            (void *)smd_channel_alloc_tbl[idx].name,
                            SMD_CHANNEL_NAME_SIZE_MAX );
      
      /* Compare only SMD_CHANNEL_NAME_SIZE_MAX-1 characters, since the 
       * argument name may be longer, but already-allocated port names always 
       * have null for the last character. */
      if ( (SMD_CID_PROP_CTYPE_MASK(smd_channel_alloc_tbl[idx].prop) ==
            SMD_CID_PROP_CTYPE_SHFT(channel_type)) &&
           !strncmp( cname,
                     name,
                     SMD_CHANNEL_NAME_SIZE_MAX-1))
      {
        break;
      }
    }
  }

  smem_spin_unlock( SMEM_SPINLOCK_SMEM_ALLOC );

  /* if match was not found, return -1 */
  if ( idx == SMEM_NUM_SMD_CHANNELS )
  {
    idx = -1;
  }
  
  return(idx);
} /* smdi_get_channel_id */

/*===========================================================================
FUNCTION      smdi_set_channel_xfrflow

DESCRIPTION   Sets the channel xfrflow status if it has not already been
              set.  If the value has been set, then this function will
              compare the set value to the inputted value.

ARGUMENTS     cid            The channel ID 
              xfrflow        The xfrflow input
              
RETURN VALUE  it returns the channel id.

SIDE EFFECTS  SMD_DONE if it sets the xfrflow or if the xfrflow was already
                       set to this value
              SMD_NON_CRIT_MISMATCH if the inputted xfrflow does not match
                                    the xfrflow already in the CID table
              SMD_INVALID_INPUT if the cid is not allocated, xfrflow does
                                not apply to this channel, or the xfrflow
                                input is invalid.
===========================================================================*/
smd_api_return_type smdi_set_channel_xfrflow
(
  uint32            cid,
  smd_xfrflow_type  xfrflow
)
{
  smd_api_return_type ret_val = SMD_DONE;
  volatile uint32               cid_xfrflow;

  /* if the xfrflow input is invalid or the channel is not allocated or
     this port does not have a STREAM protocol */
  if( (xfrflow <= SMD_XFRFLOW_FIRST) || (xfrflow >= SMD_INVALID_TYPE) ||
      !smd_channel_alloc_tbl[cid].ref_count ||
      (SMD_CID_PROP_PROT_MASK(smd_channel_alloc_tbl[cid].prop) !=
       SMD_CID_PROP_PROT_SHFT(SMD_STREAMING_BUFFER)) )
  {
    return SMD_INVALID_INPUT;
  }

  smem_spin_lock( SMEM_SPINLOCK_SMEM_ALLOC );

  cid_xfrflow = SMD_CID_PROP_XFRFLOW_MASK(smd_channel_alloc_tbl[cid].prop);
  
  if( cid_xfrflow == SMD_CID_PROP_XFRFLOW_SHFT(SMD_NO_XFRFLOW) )
  {
    smd_channel_alloc_tbl[cid].prop |= SMD_CID_PROP_XFRFLOW_SHFT(xfrflow);
  }
  else if ( cid_xfrflow != (uint32) SMD_CID_PROP_XFRFLOW_SHFT(xfrflow) )
  {
    ret_val = SMD_NON_CRIT_MISMATCH;
  }

  smem_spin_unlock( SMEM_SPINLOCK_SMEM_ALLOC );

  return ret_val;
}/* smdi_set_channel_xfrflow */

/*===========================================================================
FUNCTION      smdi_allocate_stream_channel

DESCRIPTION   Allocate a streaming shared memory channel and connect the 
              tx and rx connects to be consistent with the other processor.

ARGUMENTS     port_id - port id associated with the channel
              info - pointer to stream info structure

RETURN VALUE  None
===========================================================================*/
void smdi_allocate_stream_channel
(
  smd_port_id_type port_id,
  smd_stream_info_type *info
)
{
  volatile  void        *channel;
  volatile uint32       *proc0_fifo;
  volatile uint32       *proc1_fifo;
  smem_mem_type         cid;
#ifdef SMD_RPM_PROC
  smem_host_type         other_host;
#endif

  /* Validate channel */
  ASSERT( info->channel_type < SMD_CHANNEL_TYPE_LAST );

  /* Verify that the host processors are port of the edge specified */
  if( (smdi_edges[info->channel_type].host0 != SMD_THIS_HOST) &&
      (smdi_edges[info->channel_type].host1 != SMD_THIS_HOST) )
  {
    ERR_FATAL( "SMD port %d: channel type %d is not coincident with host %d",
        port_id, info->channel_type, SMD_THIS_HOST);
  }

  cid = (smem_mem_type)((uint32)SMEM_SMD_BASE_ID + (uint32)port_id);

#ifdef SMD_RPM_PROC
  /* For RPM clients, it is required that stream channel and FIFO
   * be allocated from dedicated piece of RPM memory */
  other_host = smd_get_other_host( SMD_THIS_HOST, info->channel_type );

  channel = (volatile smd_shared_stream_channel_type *)
           smem_alloc_rpm(other_host, cid, sizeof( smd_shared_stream_channel_algn_type));

#else
  if( smd_host_exists_in_channel(info->channel_type, SMEM_RPM) )
  {
    channel = (volatile smd_shared_stream_channel_type *)
            smem_alloc( cid, sizeof(smd_shared_stream_channel_algn_type) );
  }
  else
  {
    channel = (volatile smd_shared_stream_channel_type *)
            smem_alloc( cid, sizeof(smd_shared_stream_channel_type) );
  }    
#endif

  if( channel == NULL )
  {
    ERR_FATAL( "Unable to allocate channel for %d", (uint32) port_id, 0, 0 );
    //return;
  }

  /* Validate size: multiple of 32, and within the min and max sizes */
  if( ( ( info->fifo_sz & 0x1F ) != 0 ) ||
        ( info->fifo_sz < SMD_MIN_FIFO )  ||
        ( info->fifo_sz > SMD_MAX_FIFO ) )
  {
    ERR_FATAL("Invalid SMD FIFO SZ: %i",info->fifo_sz,0,0);
  }

#ifdef SMD_RPM_PROC
  /* For RPM clients, it is required that stream channel and FIFO
   * be allocated from dedicated piece of RPM memory */
  proc0_fifo = (volatile uint32*)smem_alloc_rpm(other_host, 
                                (smem_mem_type)(SMEM_SMD_FIFO_BASE_ID + port_id),
                                info->fifo_sz*2 );
#else
  proc0_fifo = (volatile uint32*)smem_alloc( (smem_mem_type)(SMEM_SMD_FIFO_BASE_ID + port_id),
                            info->fifo_sz*2 );
#endif

  if( proc0_fifo == NULL )
  {
    ERR_FATAL( "Unable to allocate FIFOs for port %d", (uint32) port_id, 0, 0 );
  }

  proc1_fifo = (volatile uint32 *)((uint8 *)proc0_fifo + info->fifo_sz);

  if(smd_host_exists_in_channel(info->channel_type, SMEM_RPM))
  {
    volatile smd_shared_stream_channel_algn_type *ch_hdr = 
      (smd_shared_stream_channel_algn_type*)channel;
    /* Assign the shared info structure and circular buffer */  
    if(smdi_edges[info->channel_type].host0 == SMD_THIS_HOST)
    {
      info->tx_shared_info_ptr_algn = &ch_hdr->proc0_region;
      info->tx_shared_fifo = proc0_fifo;

      info->rx_shared_info_ptr_algn = &ch_hdr->proc1_region;
      info->rx_shared_fifo = proc1_fifo;
    }
    else
    {
      info->tx_shared_info_ptr_algn = &ch_hdr->proc1_region;
      info->tx_shared_fifo = proc1_fifo;

      info->rx_shared_info_ptr_algn = &ch_hdr->proc0_region;
      info->rx_shared_fifo = proc0_fifo;
    }    
  }
  else
  {
    volatile smd_shared_stream_channel_type *ch_hdr = 
      (smd_shared_stream_channel_type*)channel;
    /* Assign the shared info structure and circular buffer */  
    if(smdi_edges[info->channel_type].host0 == SMD_THIS_HOST)
    {
      info->tx_shared_info_ptr = &ch_hdr->proc0_region;
      info->tx_shared_fifo = proc0_fifo;

      info->rx_shared_info_ptr = &ch_hdr->proc1_region;
      info->rx_shared_fifo = proc1_fifo;
    }
    else
    {
      info->tx_shared_info_ptr = &ch_hdr->proc1_region;
      info->tx_shared_fifo = proc1_fifo;

      info->rx_shared_info_ptr = &ch_hdr->proc0_region;
      info->rx_shared_fifo = proc0_fifo;
    }    
  } 


} /* smdi_allocate_stream_channel */

/*===========================================================================
  FUNCTION  smd_get_interrupt_info
===========================================================================*/
/**
  Returns the interrupt information for the specified channel in the target. 

  @param[in] channel         Channel of interest in the target

  @return
  The interrupt information for the specified channel.

  @dependencies
  None.

  @sideeffects
  None.
*/
/*=========================================================================*/
smd_interrupt_info_type *smd_get_interrupt_info( smd_channel_type channel )
{
  smd_interrupt_info_type *interrupt_info = NULL;
  smem_host_type to;

  /* Check that the channel is valid */
  if(  channel < SMD_CHANNEL_TYPE_LAST )
  {
    /* Get the source host */
    to = smd_get_other_host( SMD_THIS_HOST, channel );

    /* Return the interrupt information */
    interrupt_info = &(smd_interrupt_table[ to ]);
  }
  else
  {
    ERR_FATAL( "smd_get_interrupt_info: Invalid channel type specified %d",
                channel, 0, 0 );
  }

  return( interrupt_info );
}

/*===========================================================================
  FUNCTION  smd_host_exists_in_channel
===========================================================================*/
/**
  Checks if a given host is part of the specified channel (edge).

  @param[in] channel         Channel of interest
 
  @param[in] host            SMD host ID to look for

  @return
  TRUE is host exists, FALSE otherwsie.

*/
/*=========================================================================*/
boolean smd_host_exists_in_channel( smd_channel_type channel, uint32 host )
{
  ASSERT( channel < SMD_CHANNEL_TYPE_LAST );

  if(channel >= SMD_CHANNEL_TYPE_LAST)
    return FALSE;

  return ( ( host == (uint32)smdi_edges[ channel ].host0 ) ||
       ( host == (uint32)smdi_edges[ channel ].host1 ) );
}

/*===========================================================================
  FUNCTION  smd_read_smem_port_info
===========================================================================*/
/**
  Returns the SMD port information, extracted directly from shared memory.

  @param[in]  port_id         The SMD port ID to retrieve the information
 
  @return
  The SMD port info if it has been previously opened and stored in SMEM,
  or NULL otherwsie.

*/
/*=========================================================================*/
smd_smem_port_info_type *smd_read_smem_port_info
(
  smd_port_id_type port_id
)
{
  smd_smem_port_info_type *port_info = NULL;
  uint32                   properties;

  ASSERT( port_id < SMEM_NUM_SMD_CHANNELS );

  /* Check that we haven't previously read the information from SMEM */
  if( SMD_PORT_INVALID != smd_smem_port_info[ port_id ].port_id )
  {
    /* No need to re-read SMEM since SMD ports are never deleted */
    port_info = &( smd_smem_port_info[ port_id ] );
  }
  else
  {
    /* Check if port exists */
    if (*smd_channel_alloc_tbl[port_id].name )
    {
      smem_mem_type cid;
      uint32 size;
  
      /* Point to the SMEM port info structure list */
      port_info = &( smd_smem_port_info[ port_id ] );
      port_info->port_id = port_id;

      /* Get the properties */
      smem_spin_lock( SMEM_SPINLOCK_SMEM_ALLOC );
      properties = SMD_READ_SMEM_4( &(smd_channel_alloc_tbl[port_id].prop) );
      smem_spin_unlock( SMEM_SPINLOCK_SMEM_ALLOC );
      port_info->protocol = (smd_channel_protocol_type)SMD_CID_PROP_PROT_MASK( properties );
      port_info->channel_type = (smd_channel_type)SMD_CID_PROP_CTYPE_MASK( properties );
  
      port_info->host0 = smdi_edges[ port_info->channel_type ].host0;
      port_info->host1 = smdi_edges[ port_info->channel_type ].host1;
  
      /* Get the shared info structures from SMEM */
      cid = (smem_mem_type)((uint32)SMEM_SMD_BASE_ID + (uint32)port_id);
      port_info->stream_info = (volatile smd_shared_stream_channel_type *)
              smem_get_addr( cid, &size  );
    }
  }
  return port_info;
}

/*===========================================================================
  FUNCTION  smdi_get_loopback_client
===========================================================================*/
/**
  This function returns loopback client from channel type.

  @param[in] channel         Channel of interest in the target

  @return
  The client for the specified channel, in the case of a loopback port.

  @dependencies
  None.

  @sideeffects
  None.
*/
/*=========================================================================*/
smem_host_type smdi_get_loopback_client( smd_channel_type channel )
{  
  /* Validate channel */
  ASSERT( channel < SMD_CHANNEL_TYPE_LAST );
  
  /* The first endpoint is the host for a loopback port. */
  return smdi_edges[ channel ].host0;
}

/*===========================================================================
  FUNCTION  smdi_get_loopback_server
===========================================================================*/
/**
  This function returns loopback server from channel type.

  @param[in] channel         Channel of interest in the target

  @return
  The server for the specified channel, in the case of a loopback port.

  @dependencies
  None.

  @sideeffects
  None.
*/
/*=========================================================================*/
smem_host_type smdi_get_loopback_server( smd_channel_type channel )
{  
  /* Validate channel */
  ASSERT( channel < SMD_CHANNEL_TYPE_LAST );
  
  /* The second endpoint is the server for a loopback port. */
  return smdi_edges[ channel ].host1;
}
