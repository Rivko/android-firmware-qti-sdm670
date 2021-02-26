/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                                  D S M . C

GENERAL DESCRIPTION
  DMSS Data Services memory pool module.

EXTERNALIZED FUNCTIONS

  dsm_reg_mempool_events()
    Register the passed fucntion pointer to be later invoked when the
    specified memory events happen for the specified memory pool.

  dsm_free_buffer()
    Return a buffer item to the free queue.

  dsm_free_packet()
    Return the chain of buffer items to the free queue

  dsm_length_packet()
    Return the number of payload bytes in the passed packet buffer chain.

  dsm_offset_new_buffer()
    Return a pointer to the next free item from the pool of free items.
    Moves data pointer of DSM item to specified offset.

  dsm_new_buffer()
    Return a pointer to the next free item from the specified queue.

  dsm_pullup()
    Extract and remove the specified number of bytes from the specified
    packet chain.
    
  dsm_pullup_tail()
    Extract and remove the specified number of bytes from the tail end of 
    specified packet chain.

  If complete TCP/IP/PPP stack is compiled in (ie. ifndef DS_SLIM)

  dsm_pull8()
    Remove a single byte from the passed packet chain.

  dsm_pull16()
    Remove 2 bytes from the passed packet chain.
    Requires FEATURE_DS.

  dsm_pull32()
    Remove 4 bytes from the passed packet chain.
    Requires FEATURE_DS.

  dsm_dup_packet()
    Duplicate a packet chain.

  dsm_trim_packet()
    Trim the specified number of bytes from the passed packet chain.

  dsm_append()
    Append the the passed packet item(s) to the passed packet chain.

  dsm_pushdown()
    Insert into the passed packet chain at the specified position the passed
    string of bytes.

  dsm_pushdown_tail()
    Insert into the passed packet chain the specified number of bytes at the
    tail end of specified packet chain.

  dsm_extract()
    Extract from the passed packet chain the specified number of bytes at the
    specified offset and length. No buffer items removed from chain.

  dsm_seek_extract()
    Extract from the passed packet chain the specified number of bytes at the
    specified offset and length. Extraction is only performed if a buffer is
    bassed in to extract into, otherwise a pure seek will be done.
  
  dsm_peek_byte()
    This routine peeks at the value at the specified offset in a packet.

  dsm_split_packet()
    Splits a single packet into two packets at the given offset. Returns
    a reference to the tail packet, and will duplicate the last source item
    if split is in the middle of the item.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  dsm_init() must be called prior to any other DSM function.

Copyright (c) 2009 - 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.qdsp6/2.1.c4/memory/dsm/src/dsm.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/25/11    rj     Implemented DSM Memory Level event handling optimization
03/22/11    rj     Added debug code
12/06/10    ag     Verify the item before updating tracer.
10/04/10    ag     Resolve redundant KW warning.
06/03/10    ag     Update the pool's next_level before calling the callback.
02/22/10    ag     DSM CMI changes.
01/12/10    ag     DSM Pool Tracing changes.
09/25/09    ag     Checking for DSM_MEM_LEVEL_MIN in dsmi_release_buffer.
09/17/09    ag     Fixed lint warnings.
04/27/09    ag     Merged from 4g sandbox
01/23/07    mjb    Featurized user fields.Added queue & pool specific locks.
                   Removed use and checking of the size field.
10/25/06    mjb    Removed MSM5000_IRAM features.
06/20/06    mjb    Added dsmi_split_packet.
03/18/04    vsk    fixed a typo in checking for dsm_new_buffer returning NULL
                   in the fn dsm_dup_packet
12/02/03    pjb    Added pushdown_packed
08/30/02    ss     Eliminated a variable on the stack from dsm_free_buffer()
                   so that this funtion can be called recursively
08/22/02    pjb    Fixed NULL derefrence when inserting priority items in 
                   queue, and compile problem with DSM_MEM_CHK_STRICT 
                   turned on.
07/29/02    ak     Integrated FEATURE'ization under FEATURE_DATA_MM for new
                   DS architecture.
05/13/02    mvl    added dsm_move_to_buf() function.
06/04/02    pjb    Added poolid to out of memory error
06/03/02    pjb    Added FEATURE_DSM_MEM_CHK_EXPENSIVE around longer checks
05/01/02    pjb    Fixed pool_id not initialized when MEM_CHK disabled 
                   problem.
04/08/02    pjb    Commented some confusing pointer math voodoo. 
03/18/02    ss     Moved updates of memory level counts to protect against 
                   nested calls to dsm_new_buffer()/dsm_free_buffer() 
                   routines.
03/01/02    om     Added DSM_PRIVATE_POOL (skip mem checking).
02/11/02    ss     Introduced memory levels for HDR and modified the 
                   initial counts for different callbacks.
02/04/02    ss     Made changes to allow n arbitrary memory levels
12/21/01    pjb    Made the buf arg to dsm_pushdown_tail const
11/30/01    dwp    Add an OR FEATURE_DATA to FEATURE_DS on dsm_pull16/32.
11/27/01    pjb    Updated comments only.
11/08/01    pjb    dsm_trim_packet failed with length of 0.
10/12/01    pjb    psmisc.h only needs to be included if FEATURE_DS defined.
09/28/01    pjb    isave should be uint32, removed unnecessary #includes. 
09/25/01    ak     INTLOCK() more of dsm_enqueue and dsm_dequeue.
09/17/01    mvl    Replaced dsm_append() with original as it is must easier
                   to understand.  Fixed bug in dsm_pullup() where a race
                   condition existed because of missing intlocks.
09/13/01    ak     Re-did IRAM featurization
09/12/01    ak     Removed bad featurization typo.
07/30/01    pjb    Fixed but in trim_packet() where length was assigned when
                   packet had been freed.
07/27/01    na     Removed unnecessary ASSERT() from dsm_pullup.
07/26/01    mvl    Fixed dsm_free_buffer() so tracer field was reset. Setting
                   tracer to DSM_ALLCATED in new_buffer.
07/23/01    pjb    Reworked most of DSM
07/10/01    na     Added calls to each_enqueue_func_ptr in dsm_enqueue() &
                   dsm_simple_enqueue_isr().
06/07/01    ak     Removed dsm_offset_new_buffer from IRAM.
05/25/01    hcg    Changes to support new memory event callback registration
                   mechanism.
03/30/01    na     CR16467: Added INTLOCK protection in 
                   dsm_simple_enqueue_isr This was causing corrupted 
                   watermarks in USB packet calls.
03/23/01    jd     pullup_tail fixed to clear the pkt_ptr field of the last
                   remaining item after pullup in the source chain. 
03/21/01    jd     dup_packet fixed for case where the specified offset was 
                   past the end of the source chain.  Now returns NULL for 
                   duplicate chain pointer rather than pointer to free'd item 
03/08/01    jd     pullup_tail fixed to advance to next item if requested 
                   data spans multiple items in source dsm chain 
02/01/01    jd     app_field is cleared when dup items are freed
02/01/01    jd     dsm_dup_packet properly clears destination dsm pointer
                   even if duplication operation fails.
01/25/01    na     Minor cleanup - removed FEATURE_NEW_SIO( always on) 
                   & FEATURE_DS_TEST_MEM_POOL( never on).
12/18/00    ak     Now do free_packet in dsm_enqueue if can't enqueue item
                   (accounts for item chains).
10/10/00    jd     Merged PPG r1.17 changes from PPG (following 3 entries)
10/04/00    hcg    Added dsm_offset_new_buffer() and dsm_pushdown_tail() 
                   functions.
09/23/00    cly    Fixed size calculation for small buffer pool_item_size 
                   in dsmi_flow_mgmt_table. This is used in determining
                   which buffer pool to use in DSM_DS_POOL_SIZE macro.
08/25/00    jd     Added data_ptr repositioning for DSM_SMALL_ITEM_POOL to  
                   make room for bluetooth rfcomm and l2cap header fields.
                   FEATURE_BT must be enabled for this to happen.
08/25/00    rc     Added changes to the functions to use the new tables and 
                   pool ids defined in dsm.h. Added a function to register 
                   the memory events for the pools. dsm_pushdown() takes 
                   pool_id as an input parameter and returns a boolean 
                   value. dsm_return_small_item_free_cnt() 
                   and dsm_return_large_item_free_cnt() take pool_id as 
                   parameter.dsm_dup_packet() get a buffer from dup pool.
                   Added function dsm_peek_byte() that peeks at the value at 
                   a specified offset in a packet.dsm_enqueue() and 
                   dsm_simple_enqueue() are now edge sensitive on hi_water
                   checking. Changed FEATURE_DS_MEM_CHK to 
                   FEATURE_DSM_MEM_CHK                  
06/08/00    ak     Added MSM5000_IRAM_FWD #ifndef's for IRAM support.
06/07/00    rc     Removed ovld of link ptr and dup ptr in dsm items.
02/22/00    ak     Added code to support callbacks from RLP3.
02/02/00    ak     Merged branch and tip to support rlp 3.
12/06/99    hcg    Removed dsm_q_last_get() function, as its functionality 
                   is now provided as part of queue services.
11/30/99  hcg/na   Added dsm_pullup_tail().
11/01/99    ak     In dsm_dequeue, ASSERT if input wm_ptr is NULL, as this
                   is a dependency.
08/20/99    na     Added dsm_check_mem_pool function. This is under a test
                   feature FEATURE_DS_TEST_MEM_POOL and checks for memory
                   corruption at the end of a call.
06/17/99    mvl    Added dsm_seek_extract() - didn't rewrite dsm_extract to
                   use it though.
04/25/99    hcg    Moved position of INTLOCK() in dsm_pullup().  Fixes 
                   sockets task-synchronization problem.  Added check to 
                   item_ptr in dsm_pushdown() to fix possible dereferencing
                   of NULL ptr.
04/11/99    smp    In dsm_q_last_get(), ret_ptr is now declared only if 
                   either FEATURE_Q_NO_SELF_QPTR or FEATURE_Q_SINGLE_LINK 
                   is defined.
04/10/99    ak     Re-configured checking in dsm_new_buffer and _free_buffer
                   to be edge sensitive.  However, now intlock a greater
                   area, so that modification of dsm_*_free_cnt and the
                   resulting flow control action are atomic.
04/09/99    hcg    Added FEATURE_Q_NO_SELF_QPTR and FEATURE_Q_SINGLE_LINK to
                   support queue services memory optimization.
04/08/99    na     Changed usage of allocated so that it can fit in a byte
03/30/99    ak     Changed dsm_new_buffer and dsm_free_buffer, so that the
                   conditions which change state of flow control are now
                   level-sensitive, instead of edge-sensitive.
                   Also made INTLOCK'ed sections in dsm_new_buffer tighter.
02/23/99  na/hcg   Added changes for sockets interface.  Includes new
                   memory allocation events and functions.
01/17/99    ak     New SIO now under a compile time switch.
01/09/99    jjw    added logic to support new SIO interface
12/03/98    na     Added dsm_q_last_get() to fix CR 5831 & CR7326.
            na     Added INTLOCKs in dsm_pullup().
10/31/98    ldg    dsm_pull16 and dsm_pull32 present only with FEATURE_DS.
                   ARM porting: added typecasts.
10/29/98    na     When the phone has reached the DONOT_EXCEED level for dsm
                   items, we now cleanup the TCP resequencing queue.
08/17/98    na     Put min_ptr under FEATURE_DS_MEM_CHK.
07/24/98    jjn    Made free item processing more efficient by implementing
                   free item stack instead of queue.
04/14/98    na     Added buffer based flow control.
01/27/98    ldg    Removed the DS_SLIM switch.
01/19/98    na     Increased small item count on account of escaping changes
                   in psmgr.
12/12/97    ldg    In dsm_get_stats(), subtracted one from the count of
                   small items in use.
11/06/97    ldg    Corrected return type of dsm_get_stats() to void.
                   Removed 'extern' keyword from function prototype.
09/18/97    dbd    Removed ISS2 HACK from memory pools sizing
09/09/97    ldg    Added accessor function for UI to get DSM stats.
06/17/97    fkm    FEATURE_xxx Updates (via Automatic Scripts)
06/05/97    na     Fixed a latent bug in enqueuing of highest priority 
                   packets
02/13/97    na     CTIA show ready build.
01/23/97    na     Increased small item count to accomodate sending small
                   items on TX (by TCP) and on RX path (by PPP) for improving
                   throughput.
01/18/97    jjw    Expanded Large Item count from 16 to 20 as a quick fix
                   to a memory drain problem upon reception of small TCP
                   segments at the beginning of a call
05/24/96    jjw    Gemini & ISS2 Data Services release.
11/13/95    jjw    Changed min/max macros to upper case
11/09/95    jjw    Completed changes to support fully compliant IS-99
01/26/94    was    added features for packet management.
10/10/94    jjw    Created file
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* Target-independent Include files */
#include "comdef.h"
#include "customer.h"
#include "queue.h"
#include "dsm_item.h"
#include "dsmi.h"
#include "dsm_lock.h"
#include "msg.h"
#include "err.h"
#include "memory.h"
#include "amssassert.h"

#ifdef FEATURE_DSM_NATIVE_ASSERT
#define DSM_ASSERT( cond )      assert( cond )
#endif

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

#define DSM_UINT16_MAX 0xFFFF


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNALIZED FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/


/*===========================================================================
FUNCTION DSMI_TOUCH_ITEM()

DESCRIPTION
  This sets the file and line fields of the header.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - A pointer to an item to be reset
  file - the file name to use
  line - the line number to use

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsmi_touch_item
(
  dsm_item_type * item_ptr,
  const char * file, 
  int line
)
{
  /* Resolve KW: Verify null pointer. */
  DSM_ASSERT(item_ptr != NULL);

#if (DSM_TRACE_LEN > 1)
  int i;
  for(i=DSM_TRACE_LEN-1;i>0;i--)
  {
    item_ptr->trace[i] = item_ptr->trace[i-1];
  }
#endif

  item_ptr->trace[0].file = file;
  item_ptr->trace[0].line = (uint32)line;

}



/*===========================================================================
FUNCTION DSMI_ITEM_RESET()

DESCRIPTION
  This resets a DSM item back to its starting state.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - A pointer to an item to be reset

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsmi_item_reset
(
  dsm_item_type * item_ptr
)
{
  item_ptr->data_ptr = DSMI_ITEM_HEAD(item_ptr);
  item_ptr->used   = 0;

  item_ptr->pkt_ptr    = NULL;
  item_ptr->dup_ptr    = NULL;

  item_ptr->size   = DSM_POOL_ITEM_SIZE(item_ptr->pool_id);

  item_ptr->app_field  = 0;

  item_ptr->app_ptr  = NULL;

  item_ptr->kind       = 0;

  item_ptr->priority   = 0;
  
  (void)q_link( item_ptr, &item_ptr->link );

}

/*===========================================================================
FUNCTION DSM_NEW_BUFFER()

DESCRIPTION
  This function will return a pointer to the next free item from the free
  queue. Pool id is passed in as parameter. 

DEPENDENCIES
  None.


PARAMETERS
  pool_id - Pool from which new item should be allocated. 

RETURN VALUE
  A pointer to a 'dsm_item_type'

SIDE EFFECTS
  None
===========================================================================*/
dsm_item_type *dsmi_new_buffer
(
 dsm_mempool_id_type pool_id,
 const char * file,
 uint32 line
)
{
  dsm_pool_mgmt_table_type *table; /* table pointer                      */
  dsm_item_type *item_ptr;         /* pointer to item to retrieve        */
  int next_mem_level;              /* int to hold the next memory level  */
#ifdef FEATURE_DSM_POOL_OPTIMIZED_CB
  uint16  opposite_level_index;
#endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-----------------------------------------------------------------------
    Pre-set item pointer then switch on Pool ID to set the free pool 
    stack pointer to the proper point in the proper free stack
  -----------------------------------------------------------------------*/
  item_ptr = NULL;

#ifdef FEATURE_DSM_MEM_CHK
  dsmi_verify_pool_id(pool_id); /* Won't return if pool_id is bogus */
#else
  DSM_ASSERT((unsigned int)pool_id > 256);
#endif
  
  DSMI_POOL_LOCK(pool_id);

  table = (dsm_pool_mgmt_table_type*)pool_id;
  if (table->free_count == 0)
  {
    MSG_1(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"Out of memory in pool %d",pool_id);
    item_ptr = NULL;
  }
  else
  {

    /*-----------------------------------------------------------------
      Actually grab item off of the stack 
    -----------------------------------------------------------------*/
    DSM_ASSERT(table->free_count <= table->item_count);
    item_ptr = table->free_stack[--table->free_count];    

    #ifdef FEATURE_DSM_POOL_TRACING
    /* Update the min free count for this interval if the current free count
       has gone below it. */
    if(table->free_count < table->min_free_count_interval)
    {
      table->min_free_count_interval = table->free_count;
    }
    #endif /* FEATURE_DSM_POOL_TRACING */

    /*-----------------------------------------------------------------
      Set reference count to 1 and the app_field to 0
    -----------------------------------------------------------------*/
    /* already intlocked don't need dsm_ref functions here */
    DSM_ASSERT(item_ptr->references == 0);
    item_ptr->references = 1;

#ifdef FEATURE_DSM_MEM_CHK
    /*-----------------------------------------------------------------
      Check to see if a new minimum free count was established. Set the new 
      minimum if so.
    -----------------------------------------------------------------*/
    if(table->free_count < table->stats_min_free_count)
    {
      table->stats_min_free_count = table->free_count;
    }
    dsmi_verify_buffer(item_ptr);

    if (table->next_level >= DSM_MEM_LEVEL_MAX)
    {
      ERR_FATAL("DSM: Invalid mem level",0,0,0);
	  exit(1);
    }

    /*-----------------------------------------------------------------
      Set tracer to allocated
    -----------------------------------------------------------------*/
    dsmi_touch_item(item_ptr,file,line);
#endif

    next_mem_level = (int)(table->next_level);

    /*-----------------------------------------------------------------------
      Perform free pool flow management operations. If the free count has 
      dropped to the count specified by the next_level entry, we call all the 
      callbacks for which avail_item_count is same as free_count. We modify 
      next_level such that it points to the highest level which has 
      avail_item_count less than the free_count. 
    -----------------------------------------------------------------------*/
    while (next_mem_level > (int)DSM_MEM_LEVEL_MIN && 
	         table->free_count < 
           table->mem_event_cb_list[next_mem_level].avail_item_count)
    {
      table->next_level=(dsm_mem_level_enum_type)(next_mem_level-1);
      if (table->mem_event_cb_list[next_mem_level].mem_new_event_cb != 
          NULL)   
      {
#ifdef FEATURE_DSM_POOL_OPTIMIZED_CB
        if (table->mem_event_cb_list[next_mem_level]
           .opposite_level != DSM_MEM_LEVEL_INVALID)
        {
          /*-------------------------------------------------------------------  
            If the is_optimized_callback for next_mem_level is TRUE, set its 
            callback to FALSE. Set the is_optimized_callback to TRUE for its
            corresponding opposite level. In Optimized callback, a level's 
            callback function is only called if its corresponding opposite
            level's callback function was called last. Else we wait till the
            corresponding opposite level function has been called.  
          ---------------------------------------------------------------------*/
          if (table->mem_event_cb_list[next_mem_level].is_optimized_callback)   
          {
            table->mem_event_cb_list[next_mem_level].is_optimized_callback =
              FALSE;
            opposite_level_index = table->mem_event_cb_list[next_mem_level]
              .opposite_level_index;   
            table->mem_event_cb_list[opposite_level_index]
              .is_optimized_callback =  TRUE;

            table->mem_event_cb_list[next_mem_level]
              .mem_new_event_cb(pool_id,
                                table->mem_event_cb_list[next_mem_level].level,
                                DSM_MEM_OP_NEW);
          }
        }
        else
#endif /*FEATURE_DSM_POOL_OPTIMIZED_CB*/
        {
          table->mem_event_cb_list[next_mem_level]
            .mem_new_event_cb(pool_id,
                              table->mem_event_cb_list[next_mem_level].level,
                              DSM_MEM_OP_NEW);
        }
      }
      next_mem_level = next_mem_level - 1;
    }

    dsmi_item_reset(item_ptr);
  } /* else free count not zero */

  DSMI_POOL_UNLOCK(pool_id);

  return item_ptr;

} /* dsm_new_buffer() */


/*===========================================================================
FUNCTION DSM_OFFSET_NEW_BUFFER()

DESCRIPTION
  This function will return a pointer to the next free item from the free
  pool represented by the passed pool ID parameter.  

  In addition, the data_ptr field of the DSM item will be offset the 
  specified number of bytes into the payload.  A specified offset larger 
  than the size of the item in the pool ID will result in an error and 
  return a NULL DSM item.

DEPENDENCIES
  None.

PARAMETERS
  pool_id - Memory pool new item should be allocated from .
  offset - Space that should be reserved at the head of the item

RETURN VALUE
  A pointer to a 'dsm_item_type'.  The data_ptr will be moved by "offset"
  number of bytes.

SIDE EFFECTS
  None
===========================================================================*/
dsm_item_type *dsmi_offset_new_buffer
(
  dsm_mempool_id_type pool_id,
  uint16 offset,
  const char * file,
  uint32 line
)
{
  dsm_item_type *item_ptr;         /* pointer to item to retrieve */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_DSM_MEM_CHK
  dsmi_verify_pool_id(pool_id); /* Won't return if pool_id is bogus */
#else
  DSM_ASSERT((unsigned int)pool_id > 256);
#endif

  if (offset > DSM_POOL_ITEM_SIZE(pool_id)) 
  {
    /*-----------------------------------------------------------------------
      The requested offset is larger than the pool item size.  This is an
      error condition, so return.
    -----------------------------------------------------------------------*/
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"Offset larger than pool item size");
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Offset is ok.  Obtain a free item based on pool ID.
  -------------------------------------------------------------------------*/
  item_ptr = dsmi_new_buffer(pool_id, file, line);
  if (item_ptr == NULL)
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"Unable to allocate offset item");
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Advance the data_ptr by offset number of bytes.  Update the size of 
    the item accordingly.
  -------------------------------------------------------------------------*/
  item_ptr->data_ptr += offset;

  item_ptr->size -= offset;

  return item_ptr;

} /* dsm_offset_new_buffer */


/*===========================================================================
FUNCTION DSMI_RELEASE_BUFFER()

DESCRIPTION
  Performs tasks necessary to actually release a DSM item.
    - Resets Item fields
    - Calls Pool Callbacks
    - Restores item to free queue

DEPENDENCIES
  This function is INTERNAL to dsm.

PARAMETERS
  item_ptr - Pointer to item to return to free queue. 

RETURN VALUE
  The next item in the packet chain.

SIDE EFFECTS
  Note that this does not set item_ptr to NULL, unlike free_packet or
  free_buffer which takes a dsm_item_type** as a parameter.
===========================================================================*/
static void dsmi_release_buffer
(
  dsm_item_type *item_ptr,
  const char * file,
  uint32 line
)
{
  dsm_pool_mgmt_table_type *table;
  int next_mem_level, current_mem_level ;
#ifdef FEATURE_DSM_POOL_OPTIMIZED_CB
  uint16  opposite_level_index;
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET
  if( item_ptr != NULL )
  {  
    dsmi_verify_packet(item_ptr);
  }
#endif /* FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET */

  /*-----------------------------------------------------------------------
    Reset dynamic fields of item then return to proper queue.
  -----------------------------------------------------------------------*/
  DSM_ITEM_RESET(item_ptr);
  
#ifdef FEATURE_DSM_MEM_CHK
  /*-----------------------------------------------------------------
    Won't return on bogus pool id 
  -----------------------------------------------------------------*/
  dsmi_verify_pool_id(item_ptr->pool_id); 
  
  dsmi_touch_item(item_ptr, file, line);  
  /*-----------------------------------------------------------------------
    Set the data back to 0xAA.  0xAA was chosen because it is easy to
    recognize in the debugger and will more often than not, cause violent
    death of the application if someone tries to use it.
  -----------------------------------------------------------------------*/
  if( DSM_Fill_AA != 0 && DSM_ITEM_POOL(item_ptr) !=0 )
  {
    memset(item_ptr->data_ptr, 0xAA,
           DSM_POOL_ITEM_SIZE(DSM_ITEM_POOL(item_ptr)));
  }
#endif /* FEATURE_DSM_MEM_CHK */

  
  /*-----------------------------------------------------------------------
    Set the flow mgmt table index, then, use that to set the free stack
    array index. Re-establish the item size per the needs of the particular
    pool.
  -----------------------------------------------------------------------*/

  table = (dsm_pool_mgmt_table_type*)(item_ptr->pool_id);

  /* Resolve KW: Verify null pointer. */
  DSM_ASSERT(table != NULL);

  DSMI_POOL_LOCK(item_ptr->pool_id);

  /*-----------------------------------------------------------------------
    Verify that item is not already on free stack. Work backwards in order
    to catch problem as quickly as possible if it's there.
  -----------------------------------------------------------------------*/
#ifdef FEATURE_DSM_MEM_CHK_EXPENSIVE_DOUBLE_FREE
 {
   int n;
   for( n = (int)table->free_count - 1; n >= (int)table->stats_min_free_count; n-- )
   {
     if( item_ptr == table->free_stack[n] )
     {
       ERR_FATAL("Found duplicate freed DSM item at index %d",n,0,0);
     }
   } /* for */
 } /* block */
#endif /* FEATURE_DSM_MEM_CHK_EXPENSIVE_DOUBLE_FREE */
 
  table->free_stack[table->free_count++] = item_ptr;

  #ifdef FEATURE_DSM_POOL_TRACING
  /* Update the max free count for this interval if the current free count
     has exceeded it. */
  if(table->free_count > table->max_free_count_interval)
  {
    table->max_free_count_interval = table->free_count;
  }
  #endif /* FEATURE_DSM_POOL_TRACING */

  /*---------------------------------------------------------------------
    Return item to free pool stack then increment appropriate free count.
  ---------------------------------------------------------------------*/
  DSM_ASSERT(table->free_count <= table->item_count);

#ifdef FEATURE_DSM_MEM_CHK
  if (table->next_level < DSM_MEM_LEVEL_MIN ||
      table->next_level >= DSM_MEM_LEVEL_MAX)
  {
    ERR_FATAL("DSM: Invalid mem level",0,0,0);
	exit(1);
  }
#endif

  /* call free event cb if it is registered */
  if( table->free_cb != NULL )
  {
    (table->free_cb)(table->free_cb_user_data,item_ptr);
  }

  /* Save the current memory level */
  current_mem_level = (int)(table->next_level);

  /* Start checking from the next higher level */
  next_mem_level = current_mem_level + 1;
  /*---------------------------------------------------------------------
    Note that the table->next_level is the highest level for which 
    avail_item_count is less than free_count. Since we are interested in 
    the lowest level for which avail_item_count is more than free_count
    for free operation, we use table->next_level + 1.
  ---------------------------------------------------------------------*/

  /*---------------------------------------------------------------------
    Perform free pool flow management operations. If the free count has 
    reached the count specified by the next_level entry, we call all the 
    callbacks with avail_item_count same as free_count. We modify 
    next_level such that it points to the highest level which has 
    avail_item_count less than free_count. 
  ---------------------------------------------------------------------*/
  while (next_mem_level < (int)DSM_MEM_LEVEL_MAX &&
         table->mem_event_cb_list[next_mem_level].level !=
         DSM_MEM_LEVEL_INVALID &&
         table->free_count >=
         table->mem_event_cb_list[next_mem_level].avail_item_count)
  {
    table->next_level=(dsm_mem_level_enum_type)next_mem_level;
    if (table->mem_event_cb_list[next_mem_level].mem_free_event_cb != 
        NULL)
    {
#ifdef FEATURE_DSM_POOL_OPTIMIZED_CB
      if (table->mem_event_cb_list[next_mem_level]
         .opposite_level != DSM_MEM_LEVEL_INVALID)
      {
        /*--------------------------------------------------------------------  
          If the is_optimized_callback for next_mem_level is TRUE, set its 
          callback to FALSE. Set the is_optimized_callback to TRUE for its
          corresponding opposite level. In Optimized callback, a level's 
          callback function is only called if its corresponding opposite 
          level's callback function was called last. Else we wait till the 
          corresponding opposite level function has been called.  
        --------------------------------------------------------------------*/
        if (table->mem_event_cb_list[next_mem_level].is_optimized_callback)
        {
          table->mem_event_cb_list[next_mem_level].is_optimized_callback = 
            FALSE;
          opposite_level_index = table->mem_event_cb_list[next_mem_level]
            .opposite_level_index;
          table->mem_event_cb_list[opposite_level_index]
            .is_optimized_callback =  TRUE;

          table->mem_event_cb_list[next_mem_level].mem_free_event_cb(
            item_ptr->pool_id,
            table->mem_event_cb_list[next_mem_level].level,
            DSM_MEM_OP_FREE);
        }
      }
      else
#endif /*FEATURE_DSM_POOL_OPTIMIZED_CB*/
      {
        table->mem_event_cb_list[next_mem_level].mem_free_event_cb(
          item_ptr->pool_id,
          table->mem_event_cb_list[next_mem_level].level,
          DSM_MEM_OP_FREE);
      }
    }
    next_mem_level = next_mem_level + 1;
  } 


  DSMI_POOL_UNLOCK(item_ptr->pool_id);
} /* dsmi_release_buffer() */

/*===========================================================================
FUNCTION DSM_FREE_BUFFER()

DESCRIPTION
  This function will return the passed buffer to the proper DSM free queue.
  This includes freeing any duplicated buffer items.

  CAUTION!!!
  You probably want to call dsm_free_packet instead of this routine.
  This function probably does NOT do what you expect.
  
DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to item to return to free queue. 

RETURN VALUE
  The next item in the packet chain.

SIDE EFFECTS
  Note that this does not set item_ptr to NULL, unlike free_packet which
  takes a dsm_item_type** as parameter.
===========================================================================*/
dsm_item_type *dsmi_free_buffer
(
 dsm_item_type *item_ptr,
 const char * file,
 uint32 line
)
{
  dsm_item_type *next_item_ptr;	    /* pointer to hold next item in packet */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(item_ptr == NULL)
  {
    return NULL;
  }

#ifdef FEATURE_DSM_MEM_CHK
  dsmi_verify_buffer(item_ptr);
#endif /* FEATURE_DSM_MEM_CHK */


  /*-------------------------------------------------------------------------
    Save pointer to next item of packet (for return value) 
  -------------------------------------------------------------------------*/
  next_item_ptr = item_ptr->pkt_ptr;

  /*-------------------------------------------------------------------------
    If this is a duplicate item, call dsm_free_buffer on the original to
    decrease the reference count to reflect that we are freeing this 
    duplicate.
  -------------------------------------------------------------------------*/
  if(item_ptr->dup_ptr != NULL)
  {
   /*----------------------------------------------------------------------- 
      Reduce references on the original item.  If it has gone to zero, 
      free it.
    -----------------------------------------------------------------------*/
    if(dsmi_ref_dec(item_ptr->dup_ptr) == 0)
    {
      (void)dsmi_release_buffer(item_ptr->dup_ptr, file, line);    
    }
    /*----------------------------------------------------------------------- 
      Clear the dup_ptr field.
      We do this here rather than below for efficiency, since non-duplicate 
      items never have the dup_ptr changed from NULL.
      (note this takes advantage of the fact that the you can't have a 
      duplicate of a duplicate, i.e. references==1 always for duplicates.
    -----------------------------------------------------------------------*/ 
    item_ptr->dup_ptr = NULL;         
  }

  /*------------------------------------------------------------------------- 
    Decrement reference count. If it has gone to zero, free it. 
  -------------------------------------------------------------------------*/
  if(dsmi_ref_dec(item_ptr) == 0)
  {
    (void)dsmi_release_buffer(item_ptr, file, line);    
  } /* if( dsmi_ref_dec... */

 return next_item_ptr;

} /* dsm_free_buffer() */

/*===========================================================================
FUNCTION DSM_FREE_PACKET()

DESCRIPTION
  This function will return to the proper DSM free queue
  the entire passed packet buffer chain.

  NOTE: This function can accept a NULL pointer parameter.

DEPENDENCIES
  None

PARAMETERS
  pkt_head_ptr - Address of pointer to chain of items to return.  This
                 pointer will be set to NULL on return

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsmi_free_packet
(
 dsm_item_type **pkt_head_ptr,
 const char * file,
 uint32 line
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET
  if(pkt_head_ptr != NULL && *pkt_head_ptr != NULL) 
  {
    dsmi_verify_packet(*pkt_head_ptr);
  }
#endif /* FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET */

  if(pkt_head_ptr != NULL)
  {
    while(*pkt_head_ptr != NULL)
    {
      *pkt_head_ptr = dsmi_free_buffer(*pkt_head_ptr,file,line);
    }
  }
} /* dsm_free_packet() */

/*===========================================================================
FUNCTION DSM_LENGTH_PACKET()

DESCRIPTION
  This function counts and returns the number of bytes in a packet buffer
  chain.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pinter to item for which the length should be calculated.

RETURN VALUE
  The number of bytes in the packet.  Returns 0 if item_ptr is null.

SIDE EFFECTS
  None
===========================================================================*/
uint32 dsm_length_packet
(
  dsm_item_type *item_ptr 
)
{
  uint32 cnt = 0;               /* temporary counter variable */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET
  if(item_ptr != NULL) 
  {
    dsmi_verify_packet(item_ptr);
  }
#endif /* FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET */

  /*-----------------------------------------------------------------
    Iterate across the packet and count used size 
  -----------------------------------------------------------------*/
  while(item_ptr != NULL)
  {
    cnt += item_ptr->used;
    item_ptr = item_ptr->pkt_ptr;
  }

  return cnt;

} /* dsm_length_packet() */

/*===========================================================================
FUNCTION DSM_DUP_PACKET()

DESCRIPTION
  Duplicate first 'cnt' bytes of packet starting at 'offset'.  

  This is done by obtaining a new item from the DUP pool.  Payload data is 
  not copied.  Only the header information is duplicated, and the data_ptr 
  of the dup item points into the source item's data memory.  (i.e., the 
  pointers are set up to share the data segment of the original item).  

  The return pointer is passed back through the first argument, and the 
  return value is the number of bytes actually duplicated. 

DEPENDENCIES
  None

PARAMETERS
  dup_ptr - Pointer to pointer where new packet will be saved.
  src_ptr - Pointer to the packet to be duped.
  pool - pool to dup from.
  offset - Offset into src_ptr where duping should start
  cnt - Number of bytes from offset that should be duped

RETURN VALUE
  The total size in bytes of the duplicated packet.

SIDE EFFECTS
  None
===========================================================================*/
uint32 dsmi_dup_packet_long
(
  dsm_item_type **dup_ptr,
  dsm_mempool_id_type pool,
  dsm_item_type *src_ptr,
  uint32 offset,
  uint32 cnt,
  const char * file,
  uint32 line
)
{
  uint32 tot = 0;		/* duplication counter */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Assert that the pool is a dup pool */
  //DSM_ASSERT( DSM_POOL_ITEM_SIZE(pool) == 0 );

  if(cnt == 0 || src_ptr == NULL || dup_ptr == NULL)
  {
    return tot;
  }

#ifdef FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET
  dsmi_verify_packet(src_ptr);
#endif /* FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET */


#ifdef FEATURE_DSM_MEM_CHK
  dsmi_verify_pool_id(src_ptr->pool_id);
#endif

  /*-----------------------------------------------------------------
    Dup pointer is being nulled as this is the location where the
    return value is stored.
  -----------------------------------------------------------------*/
   *dup_ptr = NULL;
  /*-------------------------------------------------------------------------
    Skip past payload bytes to the location in the packet specified by 
    offset.
  -------------------------------------------------------------------------*/
  while(src_ptr != NULL && src_ptr->used <= offset)
  {
    if(src_ptr->references == 0)
    {
#ifdef FEATURE_DSM_MEM_CHK
      ERR_FATAL("dsm_dup_packet: Trying to dup free item",0,0,0);
#else
      return tot;
#endif /* FEATURE_DSM_MEM_CHK */      
    }

    offset -= src_ptr->used;
    src_ptr = src_ptr->pkt_ptr;
  }

  /*-------------------------------------------------------------------------
    WARNING: there are 2 "dup_ptr"s here.  One that is stack local
    and one that is a member of the dsm item.
    
    See the comment in dsm_trim_packet about this pointer to pointer
    list traversal mechanism being used here with the stack local
    "dup_ptr".
  -------------------------------------------------------------------------*/
  while(src_ptr != NULL && 
	cnt > 0)
  {
  
    if(src_ptr->references == 0)
    {
#ifdef FEATURE_DSM_MEM_CHK
      ERR_FATAL("dsm_dup_packet: Trying to dup free item",0,0,0);
#else
      return tot;
#endif /* FEATURE_DSM_MEM_CHK */
    }
	
    if((*dup_ptr = dsmi_new_buffer(pool, file, line)) == NULL)
    {
      return tot;
    }

  /*-----------------------------------------------------------------------
      Set the dup_ptr to point to the original dsm item.  If the source item 
      is in fact a duplicate, retrieve the original item's address from its
      dup_ptr.  (The original item's dup_ptr is always NULL). 
      
      Then increment the original item's reference count, and copy
      all the meta data.
  ----------------------------------------------------------------------*/

    /* check only for referenced dup_ptr to follow */
    if(src_ptr->dup_ptr != NULL)
    {
      (*dup_ptr)->dup_ptr = src_ptr->dup_ptr;
    }
    else
    {
      (*dup_ptr)->dup_ptr = src_ptr;
    }

    DSM_ASSERT((*dup_ptr)->dup_ptr != NULL);

    (void)dsmi_ref_inc((*dup_ptr)->dup_ptr);

    (*dup_ptr)->data_ptr = src_ptr->data_ptr + offset;
    (*dup_ptr)->used = (uint16) MIN(cnt,src_ptr->used - offset);

    (*dup_ptr)->size = (*dup_ptr)->used;

    (*dup_ptr)->app_field = src_ptr->app_field;

    (*dup_ptr)->priority = src_ptr->priority;

    (*dup_ptr)->kind = src_ptr->kind;
    
    offset = 0;
    cnt -= (*dup_ptr)->used;
    tot += (*dup_ptr)->used;
    src_ptr = src_ptr->pkt_ptr;
    /*-----------------------------------------------------------------------
      Update the stack local dup_ptr to point at the pointer at the
      end of the new dsm item chain.
    ----------------------------------------------------------------------*/
    dup_ptr = &((*dup_ptr)->pkt_ptr);
  } /* for(;;) */

  return tot;

} /* dsmi_dup_packet_long() */

uint16 dsmi_dup_packet
(
  dsm_item_type **dup_ptr,
  dsm_mempool_id_type pool,
  dsm_item_type *src_ptr,
  uint16 offset,
  uint16 cnt,
  const char * file,
  uint32 line
)
{
  return
    (uint16)dsmi_dup_packet_long(dup_ptr,pool,src_ptr,offset,cnt,file,line);
}


/*===========================================================================
FUNCTION DSMI_DUP_EXTERNAL_BUFFER()

DESCRIPTION
  Duplicate first 'cnt' bytes of external buffer starting at 'src_ptr'. 

  This is done by obtaining a new item from the DUP pool.  Payload data is
  not copied.  The header information is initialized with size and used
  fields set to the 'cnt' value, and the data_ptr
  of the dup item points into the source item's data memory.  (i.e., the
  pointers are set up to share the data segment of the original item). 
  The return pointer is passed back through the first argument, and the
  return value is the number of bytes actually duplicated.

DEPENDENCIES
  None

PARAMETERS
  dup_ptr - Pointer to pointer where new packet will be saved.
  src_ptr - Pointer to the external buffer to be duped.
  cnt     - Number of bytes from src_ptr that should be duped

RETURN VALUE
  The total size in bytes of the duplicated bufer.

SIDE EFFECTS
  None
===========================================================================*/
uint32 dsmi_dup_external_buffer_long
(
  dsm_item_type **dup_ptr,
  dsm_mempool_id_type pool,
  uint8          *src_ptr,
  uint32          cnt,
  const char *    file,
  uint32          line
)
{
  uint32 len = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DSM_ASSERT( DSM_POOL_ITEM_SIZE(pool) == 0 );
  
  if(cnt == 0 || src_ptr == NULL || dup_ptr == NULL)
  {
    return 0;
  }

  /*-----------------------------------------------------------------
    Dup pointer is being nulled as this is the location where the
    return value is stored.
  -----------------------------------------------------------------*/
  *dup_ptr = NULL;

  while(cnt > 0) {
    
    if((*dup_ptr = dsmi_new_buffer(pool, file, line)) == NULL)
    {
      return 0;
    }
    
    (*dup_ptr)->pkt_ptr = NULL;
    (*dup_ptr)->dup_ptr = NULL;
    (*dup_ptr)->data_ptr = src_ptr;
    (*dup_ptr)->used = (uint16) (MIN(cnt,DSM_POOL_MAX_ITEM_SIZE));
    src_ptr += MIN(cnt,DSM_POOL_MAX_ITEM_SIZE);
    
   (*dup_ptr)->app_field = 0;
       
   (*dup_ptr)->size = (uint16) cnt;
       
   (*dup_ptr)->kind = 0;
   
    len += MIN(cnt,DSM_POOL_MAX_ITEM_SIZE);
    cnt -= MIN(cnt,DSM_POOL_MAX_ITEM_SIZE);
    dup_ptr = &(*dup_ptr)->pkt_ptr;
  }
  
  return len;
} /* dsmi_dup_external_buffer_long */

uint16 dsmi_dup_external_buffer
(
  dsm_item_type **dup_ptr,
  dsm_mempool_id_type pool,
  uint8          *src_ptr,
  uint16          cnt,
  const char *    file,
  uint32          line
)
{
  return
    (uint16)dsmi_dup_external_buffer_long(dup_ptr,pool,src_ptr,cnt,file,line);
} /* dsmi_dup_external_buffer */

/*===========================================================================
FUNCTION DSM_TRIM_PACKET()

DESCRIPTION
  Trim passed packet to the specified length.

DEPENDENCIES
  None

PARAMETERS
  pkt_head_ptr - Pointer to pointer of packet to trim.
  length - Number of bytes to leave at the head of the packet.  Ie.  The
           packet will be the lesser of this length or its current length 
           when this operation completes.

RETURN VALUE
  None

SIDE EFFECTS
  *pkt_head_ptr will get set to null if the length to trim is longer
  than the packet.
===========================================================================*/
void dsmi_trim_packet_long
(
  dsm_item_type **pkt_head_ptr,
  uint32 length,
  const char * file,
  uint32 line
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Return if packet pointer is NULL. If the length parameter is NULL free
    the whole packet and return.
  -------------------------------------------------------------------------*/
  if(pkt_head_ptr == NULL || *pkt_head_ptr == NULL)
  {
    return;                     /* Nothing to trim */
  }

#ifdef FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET
  dsmi_verify_packet(*pkt_head_ptr);
#endif /* FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET */



#ifdef FEATURE_DSM_MEM_CHK
  dsmi_touch_item(*pkt_head_ptr, file, line);
#endif

  /*-------------------------------------------------------------------------
    Find the buffer where we want to split.  Adjust the length
    remaining appropriately.  Also end the loop when length hits 0.
    This will only happen if the dsm chain handed in is either already
    0 length or if there are 0 length items in the chain (which is
    actually an error but...).

    NOTE: We are traversing this list with pkt_head_ptr pointing to
    the previous items pointer to the current item.  This search
    starts with the pkt_head_ptr pointing to the head pointer that
    points at the first (current) item in the chain.  When this search
    ends the pkt_head_ptr points to a pointer (either the head pointer
    or the previous items pointer) to the item we are looking for.

    This eliminates any special handling for the head case, and
    eliminates the need to explicitly track the previous item in the
    event that the current item needs to be removed from the list.
  -------------------------------------------------------------------------*/
  while(((*pkt_head_ptr) != NULL) && 
	(length >= (*pkt_head_ptr)->used) && 
	(length > 0))
  {
    length -= (*pkt_head_ptr)->used;
    pkt_head_ptr = &(*pkt_head_ptr)->pkt_ptr;
  }
  /*-----------------------------------------------------------------------
    Only update the length if the packet still exists
  -----------------------------------------------------------------------*/
  
  if((*pkt_head_ptr) != NULL)
  {
    (*pkt_head_ptr)->used = (uint16)length;
  }    

  /*-------------------------------------------------------------------------
    If last buffer is not empty move to next buffer.  Trim from that
    location to the end of the packet.  If this buffer is empty trim
    from here.
  -------------------------------------------------------------------------*/
  if((*pkt_head_ptr != NULL) && ((*pkt_head_ptr)->used != 0))
  {
    pkt_head_ptr = &(*pkt_head_ptr)->pkt_ptr;
  }

  dsmi_free_packet(pkt_head_ptr,file,line);

} /* dsm_trim_packet_long() */

void dsmi_trim_packet
(
  dsm_item_type **pkt_head_ptr,
  uint16 length,
  const char * file,
  uint32 line
)
{
  dsmi_trim_packet_long(pkt_head_ptr,length,file,line);
}

/*===========================================================================
FUNCTION DSM_APPEND()

DESCRIPTION
  Append a DSM buffer to the end of a DSM buffer chain (packet).

DEPENDENCIES
  None

PARAMETERS
  pkt_head_ptr - Pointer to pointer to first buffer
  pkt2_head_ptr - Pointer to pointer to packet to append to first packet. 

RETURN VALUE
  None

SIDE EFFECTS
  The passed data_item_ptr pointer variable is set to NULL.
===========================================================================*/
void dsmi_append
(
  dsm_item_type **pkt_head_ptr,    /* Address of Packet chain head pointer */
  dsm_item_type **data_item_ptr,   /* Address of Packet item pointer       */
  const char * file,
  uint32 line
)
{
  register dsm_item_type *item_ptr;     /* pointer for working thru packet */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(pkt_head_ptr == NULL || data_item_ptr == NULL || *data_item_ptr == NULL)
  {
    return;
  }

#ifdef FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET
  dsmi_verify_packet(*data_item_ptr);
#endif /* FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET */


  if(*pkt_head_ptr == NULL)
  {
    /*-----------------------------------------------------------------------
      First one on chain.
    -----------------------------------------------------------------------*/
    *pkt_head_ptr = *data_item_ptr;
  }
  else 
  {
#ifdef FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET
    dsmi_verify_packet(*pkt_head_ptr);
#endif /* FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET */

    /*----------------------------------------------------------------------- 
      Traverse to end of list 
    -----------------------------------------------------------------------*/
    for( item_ptr = *pkt_head_ptr; item_ptr->pkt_ptr != NULL;
         item_ptr = item_ptr->pkt_ptr)
    {
      ;
    }
    item_ptr->pkt_ptr = *data_item_ptr;
  }
#ifdef FEATURE_DSM_MEM_CHK
  dsmi_touch_item(*pkt_head_ptr, file, line);
#endif

  *data_item_ptr = NULL;
} /* dsm_append() */

/*===========================================================================
FUNCTION DSM_PUSHDOWN()

DESCRIPTION
  Insert specified amount of contiguous new space at the beginning of
  an dsm_item_type chain. If enough space is available in the first
  dsm_item_type, no new space is allocated. Otherwise a dsm_item_type
  of the appropriate size is allocated and tacked on the front of the
  chain.  The cnt is updated. Note:  This leaves the new data at the 
  FRONT of any new item that are allocated.  You probably want to use
  dsm_pushdown_packed instead.

  The passed pointer address variable (pkt_head_ptr) may be changed,
  if the pushdown requires an additional data item.
  
  If the pool id passed in is for a ds pool (large or small), "size" 
  parameter is used to get the appropriate memory pool item. 
  So, if this function is being used for DS, either pass in 
  DSM_DS_LARGE_ITEM_POOL or DSM_DS_SMALL_ITEM_POOL.

  The 'used' field of the newly-created packet front is set to 'size'.

  This operation is the logical inverse of pullup(), hence the name.

DEPENDENCIES
  None

PARAMETERS
  pkt_head_ptr - Pointer to pointer to packet to pushdown on.
  buf - Pointer to buffer of data to pushdown on packet
  size - Size of data to pushdown 
  pool_id - Pool id from which new buffers should be allocated.

RETURN VALUE
  The number of bytes that were pushed on to the head of the packet. 

SIDE EFFECTS
  The 1st parameter may be assigned in this function
===========================================================================*/
uint32 dsmi_pushdown_long
(
  dsm_item_type **pkt_head_ptr,
  void *buf,
  uint32 size,
  dsm_mempool_id_type pool_id,
  const char * file,
  uint32 line
)
{
  dsm_item_type *item_ptr;      /* for working thru packet */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(pkt_head_ptr == NULL)
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"dsm_pushdown: Invalid packet head passed as parameter");
    return 0;
  }
  /*-------------------------------------------------------------------------
    Check that the item_ptr  hasn't been duplicated, and that it isn't a
    duplicate before checking to see if there's enough space at its front
  -------------------------------------------------------------------------*/
  item_ptr = *pkt_head_ptr;

#ifdef FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET
  if(*pkt_head_ptr != NULL)
  {
    dsmi_verify_packet(*pkt_head_ptr);
  }
#endif /* FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET */


#ifdef FEATURE_DSM_MEM_CHK
  if(item_ptr != NULL)
  {
    dsmi_verify_pool_id(item_ptr->pool_id);
  }
  dsmi_verify_pool_id(pool_id);
#endif
  /*-----------------------------------------------------------------------
   Pointer math voodoo:

     item_ptr is a pointer to a dsm_item_type, which is only the HEADER for a
     dsm item.  (uint8*)(item_ptr+1) is effectivly ((uint8*)item_ptr) +
     sizeof(dsm_item_type).  The difference between that and
     item_ptr->data_ptr indicate the quantity of empty space at the beginning
     of the packet.  

     ----------------------------------------------------------------
     |dsm_item_type|  free space (1) : data  :  free space (2)      |
     ----------------------------------------------------------------
     ^              ^                ^       ^
     |              |                |       \- item_ptr->data_ptr + used.
     |              |                \--------- item_ptr->data_ptr
     |              \-------------------------- item_ptr + 1
     \----------------------------------------- item_ptr

     (1) pushdown will try and put stuff here.  Use dsm_new_buffer_offset to
         preallocate this space.
     (2) pushdown_tail will try and put stuff here. 

   -----------------------------------------------------------------------*/
  if(item_ptr != NULL           &&
     (!DSMI_IMUTABLE(item_ptr)) &&
     ((uint16)((uint8 *)item_ptr->data_ptr - 
	       DSMI_ITEM_HEAD(item_ptr)) >= size))
  {
    /*-----------------------------------------------------------------------
      No need to alloc new dsm_item_type, just adjust this one
    -----------------------------------------------------------------------*/
    item_ptr->data_ptr -= size;
    item_ptr->used += (uint16) size;

    item_ptr->size += (uint16) size;
    
    if(buf != NULL)
    {
      (void)memcpy((*pkt_head_ptr)->data_ptr,buf,size);
    }
#ifdef FEATURE_DSM_MEM_CHK
    dsmi_touch_item(item_ptr, file, line);
#endif
  }
  else
  {
    /*-----------------------------------------------------------------
      Set the kind and priority fields 
    -----------------------------------------------------------------*/ 
    if((*pkt_head_ptr) != NULL)
    {
      /*-----------------------------------------------------------------
        Grab a new buffer here instead of letting dsm_pushdown_tail do 
        it so that we can maintain the kind and priority fields. 
      -----------------------------------------------------------------*/
      if((item_ptr = dsmi_new_buffer(pool_id, file, line)) == NULL)
      {
        MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"Running out of buffers");
        return 0;  
      }
      item_ptr->kind = (*pkt_head_ptr)->kind;

      item_ptr->priority = (*pkt_head_ptr)->priority;
    }

    /*-----------------------------------------------------------------
      Use dsm_pushdown to put data in this packet 
    -----------------------------------------------------------------*/
    size = dsmi_pushdown_tail_long(&item_ptr, buf, size, pool_id, file, line);

    /*-----------------------------------------------------------------
      Append the old packet to this new one 
    -----------------------------------------------------------------*/
    dsmi_append(&item_ptr, pkt_head_ptr, file, line);

    /*-----------------------------------------------------------------
      Return the pointer to the new one. 
    -----------------------------------------------------------------*/
    *pkt_head_ptr = item_ptr;
  }

  return size;
 
} /* dsmi_pushdown_long() */

uint16 dsmi_pushdown
(
  dsm_item_type **pkt_head_ptr,
  void *buf,
  uint16 size,
  dsm_mempool_id_type pool_id,
  const char * file,
  uint32 line
)
{
  return (uint16)dsmi_pushdown_long(pkt_head_ptr,buf,size,pool_id,file,line);
} /* dsmi_pushdown_long() */

/*===========================================================================
FUNCTION DSM_PUSHDOWN_PACKED()

DESCRIPTION
  Insert specified amount of contiguous new space at the beginning of
  an dsm_item_type chain. If enough space is available in the first
  dsm_item_type, no new space is allocated. Otherwise a dsm_item_type
  of the appropriate size, minus the space already available at the
  front of the chain, is allocated and tacked on the front of the
  chain.  The cnt is updated. Note:  This will pack the data against 
  the existing data, leaveing any unused space at the FRONT of the  
  chain.

  The passed pointer address variable (pkt_head_ptr) may be changed,
  if the pushdown requires an additional data item.
  
  If the pool id passed in is for a ds pool (large or small), "size" 
  parameter is used to get the appropriate memory pool item. 
  So, if this function is being used for DS, either pass in 
  DSM_DS_LARGE_ITEM_POOL or DSM_DS_SMALL_ITEM_POOL.

  This operation is the logical inverse of pullup(), hence the name.

DEPENDENCIES
  None

PARAMETERS
  pkt_head_ptr - Pointer to pointer to packet to pushdown on.
  buf - Pointer to buffer of data to pushdown on packet
  size - Size of data to pushdown 
  pool_id - Pool id from which new buffers should be allocated.

RETURN VALUE
  The number of bytes that were pushed on to the head of the packet. 

SIDE EFFECTS
  The 1st parameter may be assigned in this function
===========================================================================*/
uint32 dsmi_pushdown_packed_long
(
  dsm_item_type **pkt_head_ptr,
  void *buf,
  uint32 size,
  dsm_mempool_id_type pool_id,
  const char * file,
  uint32 line
)
{
  dsm_item_type *item_ptr;      /* for working thru packet */
  uint16 frag = 0;              /* size of header space in existing packet */
  uint16 offset;                /* size of empty space in new packet */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(pkt_head_ptr == NULL)
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"dsm_pushdown_packed: Invalid packet head passed as parameter");
    return 0;
  }
#ifdef FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET
  if( *pkt_head_ptr != NULL )
  {
    dsmi_verify_packet(*pkt_head_ptr);
  }
#endif /* FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET */


  /*-------------------------------------------------------------------------
    Check that item_ptr is hasn't been duplicated, and that it isn't a
    duplicate before checking to see if there's enough space at its front
  -------------------------------------------------------------------------*/
  item_ptr = *pkt_head_ptr;

#ifdef FEATURE_DSM_MEM_CHK
  if(item_ptr != NULL)
  {
    dsmi_verify_pool_id(item_ptr->pool_id);
  }
  dsmi_verify_pool_id(pool_id);
#endif
  /*-----------------------------------------------------------------------
   Pointer math voodoo:  See pointer math voodoo comment in dsm_pushdown.
   -----------------------------------------------------------------------*/
  if(item_ptr != NULL &&                        /* Its not a null pointer */
     (!DSMI_IMUTABLE(item_ptr)) &&
     (frag = (uint16)((uint8 *)item_ptr->data_ptr - /* space in head item*/   
		  DSMI_ITEM_HEAD(item_ptr))) != 0)
  {
    /*-----------------------------------------------------------------------
      Space at the beginning of the chain can be used.  Pack as much data as
      you can in there first, but no more than size
    -----------------------------------------------------------------------*/
    frag = (uint16) MIN(frag,size);   
    item_ptr->data_ptr -= frag;
    item_ptr->used += frag;

    item_ptr->size += frag;

    if( buf != NULL )
    {
      (void)memcpy((*pkt_head_ptr)->data_ptr,
	  	   ((char*)buf) + size - frag,
		   frag);
    } /* if !buf */
    size -= frag;
#ifdef FEATURE_DSM_MEM_CHK
    dsmi_touch_item(item_ptr, file, line);
#endif
  }
   
  if(size > 0)
  {
    uint32 temp;
    /*-----------------------------------------------------------------
      Figure out our offset into the first buffer to pack this 
      efficiently.  Offset should be the number of free bytes at the
      head of the first item in the chain. 
    -----------------------------------------------------------------*/
    temp = size % DSM_POOL_ITEM_SIZE(pool_id);
    offset = temp ? DSM_POOL_ITEM_SIZE(pool_id) - (uint16)temp : 0;
    /*-----------------------------------------------------------------
      Grab a new buffer here instead of letting dsm_pushdown_tail do 
      it so that we can maintain the kind and priority fields. 
    -----------------------------------------------------------------*/
    if((item_ptr = dsmi_offset_new_buffer(pool_id,
					  (uint16)offset, file, line)) == NULL)
    { 
      MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"Running out of buffers");
      return 0;  
    }
 
    /*-----------------------------------------------------------------
      Set the kind and priority fields 
    -----------------------------------------------------------------*/ 
    if((*pkt_head_ptr) != NULL)
    {
      item_ptr->kind = (*pkt_head_ptr)->kind;
      
      item_ptr->priority = (*pkt_head_ptr)->priority;
    }

    /*-----------------------------------------------------------------
      Use dsm_pushdown to put data in this packet 
    -----------------------------------------------------------------*/
    size = dsmi_pushdown_tail_long(&item_ptr, buf, size, pool_id,
                                file, line);

    /*-----------------------------------------------------------------
      Append the old packet to this new one 
    -----------------------------------------------------------------*/
    dsmi_append(&item_ptr, pkt_head_ptr, file, line);

    /*-----------------------------------------------------------------
      Return the pointer to the new one. 
    -----------------------------------------------------------------*/
    *pkt_head_ptr = item_ptr;
  }

  return size + frag;
 
} /* dsmi_pushdown_packed_long() */

uint16 dsmi_pushdown_packed
(
  dsm_item_type **pkt_head_ptr,
  void *buf,
  uint16 size,
  dsm_mempool_id_type pool_id,
  const char * file,
  uint32 line
)
{
  return
    (uint16)dsmi_pushdown_packed_long(pkt_head_ptr,buf,size,pool_id,file,line);
}

/*===========================================================================
FUNCTION DSM_PUSHDOWN_TAIL()

DESCRIPTION
  Insert specified amount of memory space at the end of a dsm_item_type 
  chain and copy data into this space if provided. If enough space is 
  available in the last item, no new space is allocated.  Otherwise 
  sufficient dsm items of the same type to supply the requested memory
  space are allocated and appended to the end of the chain.  

  This operation is the logical inverse of pullup_tail(), hence the name.

DEPENDENCIES
  None.

PARAMETERS
  pkt_head_ptr - Pointer to pointer to packet to pushdown on.
  buf - Pointer to buffer of data to pushdown on packet
  size - Size of data to pushdown 

  pool_id - Pool id from which new buffers should be allocated.

RETURN VALUE
  The number of bytes that were pushed on to the tail end of the last item.
  Note this can be less than the requested amount.

SIDE EFFECTS
  If the pushdown requires an additional data item, the pkt_ptr field of the 
  last item in the passed packet chain may be changed.
===========================================================================*/
uint32 dsmi_pushdown_tail_long
(
  dsm_item_type **pkt_head_ptr,
  const void *buf,
  uint32 size,
  dsm_mempool_id_type pool_id,
  const char * file,
  uint32 line
)
{
  uint32 bytes_copied = 0;      /* total number of bytes copied */
  uint16 item_copy_size;        /* bytes to copy into current item */

  uint8 kind = 0;               /* protocol type or TCP command/response */
  
  uint8 priority = 0;           /* priority field */
  
  /*-------------------------------------------------------------------------
    Ensure that a valid pointer to a packet chain is passed in.
  -------------------------------------------------------------------------*/
  if(pkt_head_ptr == NULL)
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"dsm_pushdown_tail: Null packet pointer passed as parameter");
    return (0);
  }

#ifdef FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET
  if(*pkt_head_ptr != NULL)
  {
    dsmi_verify_packet(*pkt_head_ptr);
  }
#endif /* FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET */

 
#ifdef FEATURE_DSM_MEM_CHK
  if(*pkt_head_ptr != NULL)
  {
    dsmi_verify_pool_id((*pkt_head_ptr)->pool_id);
  }
  dsmi_verify_pool_id(pool_id);
#endif
 
  if(*pkt_head_ptr != NULL) 
  {
    /*-----------------------------------------------------------------------
      Save relevant header info first. 
    -----------------------------------------------------------------------*/
    kind = (*pkt_head_ptr)->kind;
    
    priority = (*pkt_head_ptr)->priority;
    
#ifdef FEATURE_DSM_MEM_CHK
    dsmi_touch_item(*pkt_head_ptr, file, line);
#endif /* FEATURE_DSM_MEM_CHK */

    /*------------------------------------------------------------------------
      Walk through the packet chain to get to the last item in the chain.  The
      last item will have a NULL value as its pkt_ptr field.  The data will be 
      added to the end of this item.

      See the note in dsm_pushdown_tail about how this list traversal works.
    ------------------------------------------------------------------------*/
    while((*pkt_head_ptr)->pkt_ptr != NULL)
    {
      pkt_head_ptr = &((*pkt_head_ptr)->pkt_ptr);
    }
  }

  /*-------------------------------------------------------------------------
    Begin copy loop.  Data is copied to the end of the last item.
  -------------------------------------------------------------------------*/
  while(size != 0)
  {
    /*------------------------------------------------------------------------
      If last packet is full, duped, or a dup move to the next one. 
    ------------------------------------------------------------------------*/
    if( *pkt_head_ptr != NULL &&
        ( DSMI_IMUTABLE(*pkt_head_ptr) ||
          DSMI_TAIL_SIZE(*pkt_head_ptr) == 0))
    {
      pkt_head_ptr = &(*pkt_head_ptr)->pkt_ptr;
    }

    /*---------------------------------------------------------------------
      Need additional space to copy more data.  Allocate a new item.
    ---------------------------------------------------------------------*/
    if(*pkt_head_ptr == NULL)
    {
      if((*pkt_head_ptr = 
          dsmi_new_buffer(pool_id, file, line)) == NULL)
      {
        MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"dsm_pushdown_tail: Unable to allocate new item!");
        return (bytes_copied);
      }

      (*pkt_head_ptr)->kind = kind;
      
      (*pkt_head_ptr)->priority = priority; 

    } /* if *pkt_head_ptr != NULL */
    /*-----------------------------------------------------------------------
      Determine how much room is left at the end of the current item.
    -----------------------------------------------------------------------*/

    item_copy_size = DSMI_TAIL_SIZE(*pkt_head_ptr);
    item_copy_size = (uint16) MIN(item_copy_size, size);

    /*-----------------------------------------------------------------------
      If there is stuff to copy, copy it.  Update size and
      bytes_copied indexes.
      ---------------------------------------------------------------------*/
    if(buf != NULL)
    {
      memcpy((*pkt_head_ptr)->used + (*pkt_head_ptr)->data_ptr, 
             buf, item_copy_size);
      buf = (uint8 *)buf + item_copy_size;
    }

    (*pkt_head_ptr)->used += item_copy_size;
    bytes_copied += item_copy_size;
    size -= item_copy_size;
  }
  /*-------------------------------------------------------------------------
    Return the total number of bytes that were pushed on the tail.
  -------------------------------------------------------------------------*/
  return bytes_copied;

} /* dsmi_pushdown_tail_long */

uint16 dsmi_pushdown_tail
(
  dsm_item_type **pkt_head_ptr,
  const void *buf,
  uint16 size,
  dsm_mempool_id_type pool_id,
  const char * file,
  uint32 line
)
{
  return
    (uint16)dsmi_pushdown_tail_long(pkt_head_ptr,buf,size,pool_id,file,line);
} /* dsmi_pushdown_tail */

/*===========================================================================
FUNCTION DSM_PULLUP()

DESCRIPTION
 Copy and remove "cnt" bytes from beginning of packet. Return number of
 bytes actually pulled off.

DEPENDENCIES
  The passed data item must not be on a queue.

PARAMETERS
  pkt_head_ptr - Pointer to pointer to head of the packet.
  buf - Destination pointer for the data form the packet.
  cnt - The byte count to pull from packet head. 

RETURN VALUE
  The number of bytes pulled from the passed packet head ptr.

SIDE EFFECTS
  When the last byte is pulled from a buffer item the item is returned to
  its free queue.
===========================================================================*/
uint32 dsmi_pullup_long
(
  dsm_item_type **pkt_head_ptr,
  void *buf,
  uint32 cnt,
  const char * file,
  uint32 line
)
{
  dsm_item_type *item_ptr;      /* for working thru packet list */
  uint16 bytes_this_item;	/* Number of bytes for current buffer */
  uint32 tot = 0;		/* Bytes processed so far */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(pkt_head_ptr == NULL)
  {
    return 0;
  }
#ifdef FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET
  if(*pkt_head_ptr != NULL)
  {
    dsmi_verify_packet(*pkt_head_ptr);
  }
#endif /* FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET */



  /*-------------------------------------------------------------------------
    Work thru packet list removing the 1st 'cnt' bytes.
  -------------------------------------------------------------------------*/
  while(cnt != 0 && (item_ptr = *pkt_head_ptr) != NULL)
  {
    bytes_this_item = (uint16) MIN(cnt, item_ptr->used);
    if(buf != NULL)
    {
      if(bytes_this_item == 1)
      {  
        /*-------------------------------------------------------------------
          Common case optimization 
        -------------------------------------------------------------------*/
        *(uint8 *)buf = *item_ptr->data_ptr;
      }
      else 
      {
        (void)memcpy((uint8 *)buf, item_ptr->data_ptr, bytes_this_item);
      }
      buf = (uint8 *) buf + bytes_this_item;
    }
    tot += bytes_this_item;
    cnt -= bytes_this_item;
    item_ptr->data_ptr += bytes_this_item;

    item_ptr->size -= bytes_this_item;
    
    item_ptr->used -= bytes_this_item;
    
    if(item_ptr->used == 0)
    {
      /*---------------------------------------------------------------------
        If last byte of buffer item return the item then re-establish head
        pointer.
      ---------------------------------------------------------------------*/
      *pkt_head_ptr = dsmi_free_buffer(item_ptr, file, line);
    }
    else
    {
#ifdef FEATURE_DSM_MEM_CHK
      dsmi_touch_item(item_ptr, file, line);
#endif 
    }
  }
  return tot;
} /* dsmi_pullup_long() */

uint16 dsmi_pullup
(
  dsm_item_type **pkt_head_ptr,
  void *buf,
  uint16 cnt,
  const char * file,
  uint32 line
)
{
  return
    (uint16)dsmi_pullup_long(pkt_head_ptr,buf,cnt,file,line);
}

/*===========================================================================
FUNCTION DSM_PULLUP_TAIL()

DESCRIPTION
 Copy and remove "cnt" bytes from the end of packet. Return number of
 bytes actually pulled off.  

DEPENDENCIES
  The passed data item must not be on a queue.

PARAMETERS
  pkt_head_ptr - Address of ptr to head of packet. 
  buf - Destination buffer for the data from the packet
  cnt - Number of bytes to pull from the packet.  

RETURN VALUE
  The number of bytes pulled from the passed packet head ptr. If the number 
  of bytes is greater than the  packet chain, the entire packet chain is 
  returned.

SIDE EFFECTS
  When the last byte is pulled from a buffer item the item is returned to
  its free queue.
===========================================================================*/
uint32 dsmi_pullup_tail_long
(
  dsm_item_type **pkt_head_ptr,
  void *buf,
  uint32 cnt,
  const char * file,
  uint32 line
)
{
  uint16 bytes_this_item;	/* Bytes to process from this item */
  uint32 tot = 0;		/* Bytes processed so far. */
  uint32 tot_pkt_bytes;		/* total bytes in packet chain */
  uint32 offset;		/* calculated offset into chain */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(pkt_head_ptr == NULL)
  {
    return 0;
  }
#ifdef FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET
  if( *pkt_head_ptr != NULL )
  {
    dsmi_verify_packet(*pkt_head_ptr);
  }
#endif /* FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET */


  /*-------------------------------------------------------------------------
    Work thru packet list removing the last 'cnt' bytes from tail of packet
    chain. This is performed by first calculating the total length of the
    packet chain, and then the offset into the chain. Once these are known,
    we can then remove the "cnt" bytes.
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Calculate total bytes in packet chain. 
  -------------------------------------------------------------------------*/
  tot_pkt_bytes = dsm_length_packet(*pkt_head_ptr);

  /*-------------------------------------------------------------------------
    Calculate offset into the packet chain.
  -------------------------------------------------------------------------*/
  if(tot_pkt_bytes < cnt)
  {
    offset = 0;
  }
  else  
  {
   offset = tot_pkt_bytes - cnt;
  }

  if (offset > 0)
  {
    /*-----------------------------------------------------------------------
      The number of bytes to remove is less than the number of bytes in the 
      packet chain.  Seek in to the packet to the appropriate item them copy
      and remove 

      See the note in dsm_pushdown_tail about how this list traversal works.
    -----------------------------------------------------------------------*/
    while(((*pkt_head_ptr) != NULL) && (offset >= (*pkt_head_ptr)->used))
    {
#ifdef FEATURE_DSM_MEM_CHK
      dsmi_touch_item(*pkt_head_ptr, file, line);
#endif
      offset -= (*pkt_head_ptr)->used;
      pkt_head_ptr = &(*pkt_head_ptr)->pkt_ptr;
    }
  }

  /*-------------------------------------------------------------------------
    Reached buffer item corresponding to offset, or "cnt" bytes was greater
    than or equal to the packet chain size.  In either case, extract bytes 
    from buffer item(s) until end of packet or correct number of bytes have 
    been extracted.

    See the note in dsm_pushdown_tail about how this list traversal works.
  -----------------------------------------------------------------------*/
  while(((*pkt_head_ptr) != NULL) && (cnt != 0))
  {
    /*-----------------------------------------------------------------
      The while loop 10 or so lines up guarantees that offset is
      smaller than used
    -----------------------------------------------------------------*/
    bytes_this_item = (uint16) ((*pkt_head_ptr)->used - offset);
    bytes_this_item = (uint16) MIN(cnt, bytes_this_item);
    
    if (buf != NULL)
    {
      (void)memcpy(buf, (*pkt_head_ptr)->data_ptr + offset, bytes_this_item);
      buf = (uint8*) buf + bytes_this_item;
    }
    tot += bytes_this_item;
    cnt -= bytes_this_item;
    (*pkt_head_ptr)->used -= bytes_this_item;

    /*-----------------------------------------------------------------------
      Since we know we are finishing off the current item (we are working
      towards the tail end), the item is now at the end, so we get the 
      next one.  If we copied the entire item, then free current buffer 
      item and assign the next one.  Otherwise we leave what is in there 
      (size has already been adjusted), and move on to the next dsm item.
    -----------------------------------------------------------------------*/
    if((*pkt_head_ptr)->used == 0)
    {
      *pkt_head_ptr = dsmi_free_buffer(*pkt_head_ptr, file, line);
    }
    else
    {
      pkt_head_ptr = &(*pkt_head_ptr)->pkt_ptr;
    }
    /*-----------------------------------------------------------------------
      Aligned on dsm item boundaries, so offset will always be zero from
      now on.
    -----------------------------------------------------------------------*/
    offset = 0;
  }
  
  return tot;

} /* dsmi_pullup_tail_long() */

uint16 dsmi_pullup_tail
(
  dsm_item_type **pkt_head_ptr,
  void *buf,
  uint16 cnt,
  const char * file,
  uint32 line
)
{
  return
    (uint16)dsmi_pullup_tail_long(pkt_head_ptr,buf,cnt,file,line);
}

/*===========================================================================
FUNCTION DSM_PULL8()

DESCRIPTION
  Pull single character from dsm_item_type

DEPENDENCIES
  None

PARAMETERS
  pkt_head_ptr - Pointer to item pull from

RETURN VALUE
  returns -1 if nothing can be pulled from passed data item.

SIDE EFFECTS
  When the last byte is pulled from a buffer item the item is returned to
  its free queue (in dsm_pullup).
===========================================================================*/
int16 dsmi_pull8
(
  dsm_item_type **pkt_head_ptr,
  const char * file,
  uint32 line
)
{
  uint8 c;                      /* byte hold for return value */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET
  if( *pkt_head_ptr != NULL )
  {
    dsmi_verify_packet(*pkt_head_ptr);
  }
#endif /* FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET */

  /*-------------------------------------------------------------------------
    1st check to see if the current buffer has more than 1 byte remaining.
    Get from the current if so, else invoke to manage the buffers.
  -------------------------------------------------------------------------*/
  if(pkt_head_ptr != NULL && 
     *pkt_head_ptr != NULL &&
     (*pkt_head_ptr)->used > 1)
  {
    (*pkt_head_ptr)->used--;

    (*pkt_head_ptr)->size--;

    c = *(*pkt_head_ptr)->data_ptr++;

#ifdef FEATURE_DSM_MEM_CHK
    dsmi_touch_item(*pkt_head_ptr, file, line);
#endif /* FEATURE_DSM_MEM_CHK */
  }
  else 
  {
    /*-----------------------------------------------------------------------
    current buffer item empty, invoke to do the work 
    -----------------------------------------------------------------------*/
    if(dsmi_pullup_long(pkt_head_ptr, &c, 1,file,line) != 1)
    {
      /*---------------------------------------------------------------------
        Nothing left 
      ---------------------------------------------------------------------*/
      return -1;    
    }
  }

  return c;

}  /* dsm_pull8() */


/*===========================================================================
FUNCTION DSM_PULL16()

DESCRIPTION
 Pull a 16-bit integer in host order from buffer in network byte order.
 Return -1 on error

DEPENDENCIES
  None

PARAMETERS
  pkt_head_ptr - Pointer to item pull from

RETURN VALUE
  returns -1 if nothing.  Otherwise the a uint16 that is in HOST byte order.

SIDE EFFECTS
  None
===========================================================================*/
int32 dsmi_pull16
(
  dsm_item_type **pkt_head_ptr,
  const char * file,
  uint32 line
)
{
  uint8 buf[2];            

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(dsmi_pullup(pkt_head_ptr, buf, 2,file,line) != 2)
  {
    /*-----------------------------------------------------------------------
      Nothing left 
    -----------------------------------------------------------------------*/
    return -1;    
  }

  return (buf[0] << 8) + buf[1];
}  /* dsm_pull16() */


/*===========================================================================
FUNCTION DSM_PULL32()

DESCRIPTION
 Pull a 32-bit integer in host order from buffer in network byte order.
 On error, return 0. Note that this is indistinguishable from a normal
 return.

DEPENDENCIES
  None

PARAMETERS
  pkt_head_ptr - Pointer of item to pull from 

RETURN VALUE
  returns 0 if nothing.  Otherwise uint32 in HOST byte order. 

SIDE EFFECTS
  None
===========================================================================*/
uint32 dsmi_pull32
(
  dsm_item_type **pkt_head_ptr,
  const char * file,
  uint32 line
)
{
  uint8 buf[4];

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(dsmi_pullup(pkt_head_ptr, buf, 4,file,line) != 4)
  {
    /*-----------------------------------------------------------------------
      Return zero if insufficient buffer 
    -----------------------------------------------------------------------*/
    return 0;
  }

  return (buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3];

} /* dsm_pull32() */


/*===========================================================================
FUNCTION DSM_EXTRACT()

DESCRIPTION
 Copy data from within dsm item to user-provided buffer, starting at
 'offset' bytes from start of dsm item and copying no more than 'len'
 bytes. Return actual number of bytes copied

DEPENDENCIES
  None

PARAMETERS
  packet_ptr - Pointer to packet to remove data from 
  offset - Offset this number of bytes from packet head. 
  buf - Pointer to destination buffer
  len - length in bytes of extraction. 

RETURN VALUE
  returns  0 if nothing

SIDE EFFECTS
  None
===========================================================================*/
uint32 dsm_extract_long
(
  dsm_item_type *packet_ptr,
  uint32 offset,
  void *buf,
  uint32 len
)
{
  uint32 copied;                /* return value */
  uint16 n;                     /* individual memcpy size holder */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET
  if(packet_ptr != NULL)
  {
    dsmi_verify_packet(packet_ptr);
  }
#endif /* FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET */

  /*-------------------------------------------------------------------------
    Assign local variables then move past buffer items until the offset
    number of bytes are reached.
  -------------------------------------------------------------------------*/
  copied = 0;
  while(packet_ptr != NULL && offset >= packet_ptr->used)
  {
    offset -= packet_ptr->used;
    packet_ptr = packet_ptr->pkt_ptr;
  }
  /*-------------------------------------------------------------------------
    Reached buffer item corresponding to offset. Extract bytes from buffer
    item(s) until end of packet or correct number of bytes have been 
    extracted.
  -------------------------------------------------------------------------*/
  while(packet_ptr != NULL && len != 0)
  {
    n = MIN((uint16)len, packet_ptr->used - (uint16)offset);
    (void)memcpy(buf, packet_ptr->data_ptr + offset, n);
    copied += n;
    buf = (uint8*)buf + n;
    len -= n;
    offset = 0;
    packet_ptr = packet_ptr->pkt_ptr;  
  }

  return copied;

} /* dsm_extract_long() */

uint16 dsm_extract
(
  dsm_item_type *packet_ptr,
  uint16 offset,
  void *buf,
  uint16 len
)
{
  return (uint16) dsm_extract_long(packet_ptr,offset,buf,len);
}

/*===========================================================================
FUNCTION DSM_INSERT()

DESCRIPTION
 Copy data to within dsm item from user-provided buffer, starting at
 'offset' bytes from start of dsm item and copying (overwriting) no more
 than 'len' bytes. Return actual number of bytes copied.
  
 If the item has has dependencies, make a copy of the dependent item and
 write to that instead.

 If the len, spans the end of the packet, the packet will be extended. If the
 new data starts off the end of the packet this WILL fail.. (Ie. offset must
 fall within the packet, but offset+len may not fall within the packet.)


DEPENDENCIES
  None

PARAMETERS
  packet_ptr - Pointer to packet to remove data from 
  pool_id - Should it be necessary to allocate more memory, do it from here.
  offset - Offset this number of bytes from packet head. 
  buf - Pointer to destination buffer
  len - length in bytes of extraction. 

RETURN VALUE
  Returns number of bytes written, 0 if nothing happened.

SIDE EFFECTS
  None
===========================================================================*/
uint32 dsmi_insert_long
(
  dsm_item_type **pkt_head_ptr,
  dsm_mempool_id_type pool_id,
  uint32 offset,
  void * buf,
  uint32 len,
  const char * file,
  uint32 line
)
{ 
  uint32 tmp_len;
  uint32 ret = 0;
  dsm_item_type ** orig_packet_ptr = pkt_head_ptr;

  
  DSM_ASSERT(pkt_head_ptr != NULL);
  DSM_ASSERT(buf != NULL);
  DSM_ASSERT(len > 0);

#ifdef FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET
  if( *pkt_head_ptr != NULL )
  {
    dsmi_verify_packet(*pkt_head_ptr);
  }
#endif /* FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET */

  
#ifdef FEATURE_DSM_MEM_CHK
  dsmi_verify_pool_id(pool_id);
#endif /* FEATURE_DSM_MEM_CHK */
  
  /* process until there are no more bytes left to overwrite            */
  while( *pkt_head_ptr != NULL && len > 0 )
  {
    /* Seek the first buffer that we need to overwrite, and decrement
       offset for each buffer passed..                                    */
    if(offset >= (*pkt_head_ptr)->used)
    {
      offset -= (*pkt_head_ptr)->used;
      pkt_head_ptr = &(*pkt_head_ptr)->pkt_ptr;
    } else if((*pkt_head_ptr)->references > 1 ||
              ((*pkt_head_ptr)->dup_ptr != NULL &&
               (*pkt_head_ptr)->dup_ptr->references > 1))
    {
      /* If the current item has been duped or if current item is a
       duplicate and its original has more than one duplicate;
       then make a copy of the packet to use to start the
       insertion chain.                                                 */
      dsm_item_type * copied_pkt = NULL;
      dsm_item_type * tmp_pkt;
           
      tmp_len = dsmi_pushdown_long(&copied_pkt,
			        (*pkt_head_ptr)->data_ptr,
			        (*pkt_head_ptr)->used,
			        pool_id,
			        file,
			        line);
      
      /* If this operation fails partially or completely then
	 return with what we have done so far                         */
      if( tmp_len != (*pkt_head_ptr)->used )
      {
	dsmi_free_packet(&copied_pkt,file,line);
	return ret;
      }
      
      /* Seek to end of packet and attach back to next packet.
         Base pkt_head_ptr now at the original item's end to point to
         the rest of the chain.                                       */
      tmp_pkt = copied_pkt;

      copied_pkt->app_field = (*pkt_head_ptr)->app_field;

      copied_pkt->kind = (*pkt_head_ptr)->kind;

      while(copied_pkt->pkt_ptr != NULL )
      {
        copied_pkt = copied_pkt->pkt_ptr;

        copied_pkt->app_field = (*pkt_head_ptr)->app_field;

        copied_pkt->kind = (*pkt_head_ptr)->kind;
      }

      copied_pkt->pkt_ptr = (*pkt_head_ptr)->pkt_ptr;     

      /* Entire item was copied, free the original                  */
      (void)dsmi_free_buffer(*pkt_head_ptr,file,line);

      *pkt_head_ptr = tmp_pkt;

    }  /* if de-dupe operation necessary */    
    else
    {
#ifdef FEATURE_DSM_MEM_CHK
      dsmi_touch_item(*pkt_head_ptr,file,line);
#endif /* FEATURE_DSM_MEM_CHK */
      tmp_len = MIN((*pkt_head_ptr)->used - offset,len);
      memcpy((*pkt_head_ptr)->data_ptr+offset, buf, tmp_len);
      buf = (char*) buf + tmp_len;
      
      ret += tmp_len;
      len -= tmp_len;
      offset = 0;
      
      pkt_head_ptr = &(*pkt_head_ptr)->pkt_ptr;
    } /* else item has not been duped and is not empty (nominal case)   */    
  } /* while(len > 0) */
  
  DSM_ASSERT(offset == 0);

  if( len != 0 )
  {
    tmp_len = dsmi_pushdown_tail_long(orig_packet_ptr, buf, len, pool_id,
                                      file, line);
    ret += tmp_len;
    len -= tmp_len;
  } 
  
  return ret;
} /* dsmi_insert_long */

uint16 dsmi_insert
(
  dsm_item_type **pkt_head_ptr,
  dsm_mempool_id_type pool_id,
  uint16 offset,
  void * buf,
  uint16 len,
  const char * file,
  uint32 line
)
{
  return
    (uint16)dsmi_insert_long(pkt_head_ptr,pool_id,offset,buf,len,file,line);
}


/*===========================================================================
FUNCTION  DSM_SEEK_EXTRACT()

DESCRIPTION
  This function will seek offset bytes into the item that is passed in.  If
  'buf' is not NULL it will copy 'len' bytes into it. If there are not 'len'
  bytes to copy all the butes from offeset until the end of the item chain
  will be copied.
  If the seek or the extract tries go beyond the end of the item chain the
  function will return FALSE, and niether 'item_head_ptr' nor 'offset_ptr
  will' be modified.  Otherwise 'item_head_ptr' and 'offset_ptr' will be set
  to indicate the SEEK end point (which is not the EXTRACT end point, this
  means that the offset will indicate the first byte that was copied).

DEPENDENCIES
  None

PARAMETERS
  pkt_head_ptr - Pointer to pointer to packet to extract from 
  offset_ptr - Pointer to integer off bytes to offset into packet
  buf - Buffer to put extracted bytes into
  len - Pointer to integer to put number of bytes extracted

RETURN VALUE
  TRUE:  We have successfully seeked to the right byte, and if we were doing
         an extract we copied ALL of the bytes indicated by 'len.'
           pkt_head_ptr: set to the item in which the seek ended
           offset_ptr: set to the offset in the item in which the seek ended
           len: if bytes were extracted this is set to the number that were

  FALSE: We have seeked past the end of the packet without finding the byte
         we are interesed in or we have seeked to the right place but not
         succeeded in copying all of the bytes.  Niether 'pkt_head_ptr' or
         'offset_ptr' are modified.  However:
             len: will be changed to reflect the number of bytes that were
                  copied


                                ***WARNING***
  Successful completion of this call WILL modify the first two arguments, and
  in all cases the last two arguments MAY be modified.
  

SIDE EFFECTS
  None
===========================================================================*/
boolean dsm_seek_extract
(
  dsm_item_type **pkt_head_ptr,
  uint16 *offset_ptr,
  void *buf,
  uint16 *len
)
{
  dsm_item_type *item_ptr;      /* pointer to the item inside head */
  dsm_item_type *seek_item_ptr; /* pointer to item we seeked to */
  uint8 *buf_ptr;               /* pointer to the passed in buffer */
  uint16 offset;                /* local copy of offset passed in */
  uint16 seek_offset;           /* seek offset in seeked to item */
  uint16 to_copy;               /* number of bytes copied */
  uint16 can_copy;              /* number of bytes memcpy can copy */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Make sure that all of the various fields are either not NULL or have
    reasonable values.
  -------------------------------------------------------------------------*/
  DSM_ASSERT(pkt_head_ptr != NULL && *pkt_head_ptr != NULL);
  DSM_ASSERT( buf != NULL || len == NULL || *len == 0 );

#ifdef FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET
  dsmi_verify_packet(*pkt_head_ptr);
#endif /* FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET */


  /*-------------------------------------------------------------------------
    Move past buffer items until the offset number of bytes are reached.
  -------------------------------------------------------------------------*/
  item_ptr = *pkt_head_ptr;
  offset = *offset_ptr;
  while(item_ptr != NULL && offset >= item_ptr->used)
  {
    offset -= item_ptr->used;
    item_ptr = item_ptr->pkt_ptr;
  }

  /*-------------------------------------------------------------------------
    We have seeked into the packet that contains the offset byte or come to
    the end of the item chain: if we have come to the end of the item chain
    return FALSE, otherwise if we are not supposed to copy anything return
    TRUE.
  -------------------------------------------------------------------------*/
  if(item_ptr == NULL)
  {
    if(len != NULL)
    {
      *len = 0;
    }
    return FALSE;
  }
  else if(buf == NULL || len == 0)
  {
    *offset_ptr = offset;
    *pkt_head_ptr = item_ptr;
    return TRUE;
  }

  /*-------------------------------------------------------------------------
    Reached buffer item containing offset. Make note of the item and offset
    we seeked to to return on success.  Extract bytes from buffer item(s)
    until end of packet or correct number of bytes have been extracted.
  -------------------------------------------------------------------------*/
  seek_offset = offset;
  seek_item_ptr = item_ptr;

  to_copy = *len;
  buf_ptr = (uint8*)buf;
  while(item_ptr != NULL && to_copy > 0)
  {
    can_copy = MIN(to_copy, item_ptr->used - (uint16)offset);
    (void)memcpy( buf_ptr, item_ptr->data_ptr + (uint16)offset, can_copy);
    buf_ptr += can_copy;
    to_copy -= can_copy;

    /*-----------------------------------------------------------------------
      If we haven't copied all of our bytes but have copied all of the bytes
      in this item, move to the next item.
    -----------------------------------------------------------------------*/
    item_ptr = item_ptr->pkt_ptr;
    offset = 0;

  } /* while(copying) */

  /*-------------------------------------------------------------------------
    We have copied some number of bytes, if we have copied all of the bytes
    we were supposed to copy, return TRUE otherwise return FALSE.
  -------------------------------------------------------------------------*/
  *len -= to_copy;
  if(to_copy > 0)
  {
    return FALSE;
  }
  else
  {
    *pkt_head_ptr = seek_item_ptr;
    *offset_ptr = seek_offset;
    return TRUE;
  }

} /* dsm_seek_extract() */

/*===========================================================================
FUNCTION DSM_PEEK_BYTE()
DESCRIPTION
  This routine peeks at the value at the specified offset in a packet.
  
DEPENDENCIES
  None

PARAMETERS
  item_head_ptr - Pointer to head of packet chain. 
  offset - Offset to peek at. 
  value - Pointer to byte to put result. 

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean dsm_peek_byte_long
(
  dsm_item_type* item_head_ptr,
  uint32 offset,
  uint8* value
)
{
  return (0 != dsm_extract_long(item_head_ptr, offset, value, 1));
} /* dsm_peek_byte() */

boolean dsm_peek_byte
(
  dsm_item_type* item_head_ptr,
  uint16 offset,
  uint8* value
)
{
  return (0 != dsm_extract_long(item_head_ptr, offset, value, 1));
}


/*===========================================================================
FUNCTION DSM_SPLIT_PACKET()

DESCRIPTION
  This routine splits a dsm packet chain at LENGTH bytes.  HEAD will
  point at the head of the original packet which will be trimmed to LENGTH
  bytes.  TAIL will point at head of the second part of the packet that starts
  at LENGTH+1 in the original packet.

  If the split occurs in the middle of an item, that item will be "duped" and
  the caller will get a reference to that duped item. The duped item points to
  the rest of the tail of the chain if there are more than one items in the
  tail chain.

  If the split occurs at or past the data in a chain, the call will return
  length, but otherwise do nothing.
  
DEPENDENCIES
  None

PARAMETERS
  head          - pointer to packet to split
  tail          - recipient of rest of the packet
  length        - amount to retain in "head" packet
  dup_pool      - dup pool to use for new item.

RETURN VALUE
  Amount of bytes split. This should equal LENGTH.

SIDE EFFECTS
  None
===========================================================================*/
uint32 dsmi_split_packet
(
  dsm_item_type ** pkt_head_ptr,
  dsm_item_type ** pkt_tail_ptr,
  uint32 offset,
  dsm_mempool_id_type dup_pool_id,
  const char * file,
  uint32 line
)
{
  uint32 length;

  uint32 app_field;

  uint32 kind;
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DSM_ASSERT(pkt_head_ptr != NULL);
  DSM_ASSERT(pkt_tail_ptr != NULL);

  if(*pkt_head_ptr == NULL)
  {
    return 0;
  }
  if(*pkt_tail_ptr != NULL)
  {
    ERR_FATAL("Second argument tail pointer references a non-null value:ref 0x%x",
               (int)*pkt_tail_ptr,0,0);

  }

  /* dup_pool_id must be a DUP pool */
  // DSM_ASSERT( DSM_POOL_ITEM_SIZE(dup_pool_id) == 0 );
  
#ifdef FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET
  dsmi_verify_packet(*pkt_head_ptr);
#endif /* FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET */

  
#ifdef FEATURE_DSM_MEM_CHK
  dsmi_verify_pool_id((*pkt_head_ptr)->pool_id);
  dsmi_verify_pool_id(dup_pool_id);
#endif

  /* Save original kind and app_field */
  kind = (*pkt_head_ptr)->kind;

  app_field = (*pkt_head_ptr)->app_field;
  
  /*-------------------------------------------------------------------------
    Move past buffer items until the offset number of bytes are reached.
  -------------------------------------------------------------------------*/
  length = 0;
  while( *pkt_head_ptr != NULL &&
         offset >= (*pkt_head_ptr)->used )
  {

#ifdef FEATURE_DSM_MEM_CHK
    if((*pkt_head_ptr)->references == 0)
    {
      ERR_FATAL("dsm_split: Trying dsm_split_packet on a free item 0x%x",
                (int)*pkt_head_ptr,0,0);
    }
#endif /* FEATURE_DSM_MEM_CHK */

    length += (*pkt_head_ptr)->used;
    offset -= (*pkt_head_ptr)->used;

    pkt_head_ptr = &(*pkt_head_ptr)->pkt_ptr;
  } /* while */

  /* If offset is not past the end of an item */
  if(NULL != *pkt_head_ptr)
  {
#ifdef FEATURE_DSM_MEM_CHK
    if((*pkt_head_ptr)->references == 0)
    {
      ERR_FATAL("dsm_dup_packet: Trying to dup free item 0x%x",
                (int)*pkt_head_ptr,0,0);
    }
#endif /* FEATURE_DSM_MEM_CHK */

    /* If the offset is in the middle of the item */
    if( offset != 0 )
    {
      /* Duplicate current item */
      /* User will get the top of the dup chain returned */
      if((*pkt_tail_ptr = dsmi_new_buffer(dup_pool_id, file, line)) == NULL)
      {
	return 0;
      }

      /* assign fields to duplicate packet */
      (*pkt_tail_ptr)->data_ptr = (*pkt_head_ptr)->data_ptr + offset;
      (*pkt_tail_ptr)->used = (uint16) ((*pkt_head_ptr)->used - offset);

      (*pkt_tail_ptr)->size = (*pkt_tail_ptr)->used;

      (*pkt_tail_ptr)->app_field = app_field;

      (*pkt_tail_ptr)->kind = (uint8) kind;

      length += offset;
      
      /* Adjust current reference count, used and size */
      (*pkt_head_ptr)->used = (uint16) offset;

      /* If the src is a dup, go back to the originating packet to get ptr */
      if(DSMI_DUP(*pkt_head_ptr))
      {

        /* set size to offset if we are a dup */
        (*pkt_head_ptr)->size = (uint16) offset;

        /* set dup_ptr for new duped item */
        (*pkt_tail_ptr)->dup_ptr = (*pkt_head_ptr)->dup_ptr;

        /* increase reference on original packet */
	      (void)dsmi_ref_inc((*pkt_head_ptr)->dup_ptr);
      }
      else
      {
	      (*pkt_tail_ptr)->dup_ptr = *pkt_head_ptr;
        (void)dsmi_ref_inc(*pkt_head_ptr);
      }

      /* Save next packet pointer and terminate packet */
      (*pkt_tail_ptr)->pkt_ptr = (*pkt_head_ptr)->pkt_ptr;
      (*pkt_head_ptr)->pkt_ptr = NULL;
    }
    else /* offset == 0 */
    {
      *pkt_tail_ptr = *pkt_head_ptr;
      *pkt_head_ptr = NULL;
    }

  } /* if NULL != pkt_head_ptr  */

  /* return offset as length signifying a successful operation */
  return length;
} /* dsmi_split_packet() */

/*===========================================================================
FUNCTION DSM_ITERATOR()

DESCRIPTION
  Iterates down a DSM item chain and calls the user specified "iterator" function  on each dsm buffer. 
  The iterator function is allowed to modify the contents of each item as a buffer, using the data_ptr and
  used as the location and length allowed.

  The iterator function returns 0 or 1 depending on whether it wishes to
  continue processing or to stop processing.

  An example would be a search iterator. The search iterator would return a 1
  when the found condition was met. The user data could point to the search
  parameters and track and return the offset of found information.
  
DEPENDENCIES
  None

PARAMETERS
  pkt_ptr       - pointer to item chain
  offset        - offset into chain before starting iteration
  len           - amount of data to process through iterator
  len           - amount of data to process through iterator
  iterator_f    - pointer to iterator function that takes pointer/length
                  and returns non-zero if it wants to stop iterating.

RETURN VALUE
  0 - If iterator completed without stopping.
  non 0 - If stopped on iterator function condition.

SIDE EFFECTS
  None
===========================================================================*/
int 
dsmi_iterator
(
  dsm_item_type * pkt_ptr,
  uint32 offset,
  uint32 len,     
  void * user_data,
  int (*iterator_f)(void *user_data, const char * buf, uint32 len),
  const char * file,
  uint32 line
)
{
   int ret = 0;
   DSM_NOTUSED(file);
   DSM_NOTUSED(line);

   if(iterator_f == NULL)
   {
     ERR_FATAL("iterator function must be valid pointer",0,0,0);
   }

   while(pkt_ptr != NULL && offset > pkt_ptr->used)
   {
#ifdef FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET
      dsmi_verify_buffer(pkt_ptr);
#endif /* FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET */
      offset -= pkt_ptr->used;
      pkt_ptr = pkt_ptr->pkt_ptr;
   }

   while(pkt_ptr != NULL && len > 0 && ret == 0)
   {

#ifdef FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET
     dsmi_verify_buffer(pkt_ptr);
#endif /* FEATURE_DSM_MEM_CHK_EXPENSIVE_ALWAYS_VERIFY_PACKET */

     /* Non-zero return means stop. */
     ret = iterator_f(user_data, (const char *)pkt_ptr->data_ptr + offset, MIN(len, pkt_ptr->used - offset));

     len -= MIN(len, pkt_ptr->used - offset);
     offset = 0;
     pkt_ptr = pkt_ptr->pkt_ptr;
   }
   return ret;
} /* dsmi_iterator */

