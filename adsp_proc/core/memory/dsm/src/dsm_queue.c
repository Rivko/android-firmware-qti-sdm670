/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                                  D S M _ Q U E U E . C

GENERAL DESCRIPTION
  DMSS Data Services memory pool module.

EXTERNALIZED FUNCTIONS

  dsm_dequeue()
    Dequeue the next buffer item from the passed Watermark structure. Perform
    relevent 'get' event functions if appropriate.

  dsm_enqueue()
    Enqueue the passed buffer item onto the passed Watermark structure. 
    Perform any relevent 'put' event functions as appropriate.

  dsm_empty_queue()
    completely empty the dsm watermark queue.

  dsm_simple_enqueue_isr()
    This function will put the passed DSM item to the passed shared queue 
    then check for and perform any 'put' events.  This function does not 
    check for priority.  It simply enqueues to the tail of the queue.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  dsm_init() must be called prior to any other DSM function.
  dsm_queue_init() must be called prior to using a DSM watermark queue.

Copyright (c) 2009 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.qdsp6/2.1/memory/dsm/src/dsm_queue.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/06/2011  rj     Internalize private fields within watermark structure   
03/22/11    rj     Added debug code
03/08/11    ag     Remove FEATURE_DSM_WM_CB
08/01/11    hh     Memset queue to 0 only once to avoid mutex leaks.
04/02/10    hh     Used cookie to check the watermark initialization and set 
                   the queue buffer to 0 in dsm_queue_init.
03/09/10    ag     Removing modem_mcs_defs.h as its breaking apps build.
03/08/10    ag     Include modem_mcs_defs.h
02/22/10    ag     DSM CMI Changes.
01/12/10    ag     Merging DSM WM tracing changes from //depot.
09/17/09    ag     Fixed lint warnings.
04/27/09    ag     Replaced assert.h with amssassert.h
04/13/09    ag     Added a check in dsm_init to check if the lock is already 
                   initialized before then do not re-initialize it. 
01/26/07    mjb    Added queue-specific locking mechanism.
06/29/06    mjb    Added file/line tracing,promoted internal uint16 to uint32
01/18/05    pjb    Added dsm_is_wm_empty
01/01/05    pjb    Created file
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
#include "dsm_queue.h"



/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
/*===========================================================================

                MACROS AND DEFINES

===========================================================================*/
#define DSM_WM_CB(CB_FPTR,WM_PTR,CB_DATA) do {\
  if(CB_FPTR != NULL) (CB_FPTR)(WM_PTR,CB_DATA);}while(0)

#define DSM_QUEUE_SENTINEL (0x0ff51DE5)

#define DSMI_CHECK_WM_PTR(wm_ptr)\
  do { if( wm_ptr == NULL ){\
        ERR_FATAL("DSM WM POINTER 0x%x IS NULL",(int)wm_ptr,0,0);\
       }\
       if( wm_ptr->sentinel != DSM_QUEUE_SENTINEL ){\
         MSG(MSG_SSID_DFLT, MSG_LEGACY_MED,"DSM WM QUEUE UNINITIALIZED:Call dsm_queue_init on WMs before using.");\
         }} while(0)



/*===========================================================================
FUNCTION DSMI_WM_CHECK_LEVELS()

DESCRIPTION
   Call Watermark item call back functions if water mark level change 
   appropriately.

DEPENDENCIES
  None

PARAMETERS
   wm_ptr - Pointer to the watermark item header.  This has the pointers
            to the callbacks and the callback data associated with this 
            watermark.
   initial_count - This is the size of this water mark queue before the 
                   "operation".  This function is called after the "operation"
                   has changed the count for the watermark.  
   enqueue - This boolean indicates that the each enqueue function should 
             be called.

RETURN VALUE
  None

SIDE EFFECTS
  One or more callbacks may get invoked.  Highest count may get updated 
  (If debugging is enabled, and the high watermark has gone up.)
===========================================================================*/
static void
dsmi_wm_check_levels(dsm_watermark_type * wm_ptr, 
                     uint32 initial_count, 
                     boolean enqueue)
{
  if(initial_count == 0 &&
     wm_ptr->current_cnt > 0)
  {
    DSM_WM_CB(wm_ptr->non_empty_func_ptr,wm_ptr,wm_ptr->non_empty_func_data);
  }
  
  if(initial_count <= wm_ptr->hi_watermark &&
     wm_ptr->current_cnt > wm_ptr->hi_watermark)
  {
    #ifdef FEATURE_DSM_WM_TRACING
    /* Increment the hi_event_cnt */
    DSMI_INCR_COUNT(wm_ptr,hi);
    #endif
    DSM_WM_CB(wm_ptr->hiwater_func_ptr,wm_ptr,wm_ptr->hiwater_func_data);
  }
  
  if(enqueue) 
  {
    DSM_WM_CB(wm_ptr->each_enqueue_func_ptr,
              wm_ptr,wm_ptr->each_enqueue_func_data);
  }
  
  if(initial_count >= wm_ptr->lo_watermark && 
     wm_ptr->current_cnt < wm_ptr->lo_watermark)
  {
    #ifdef FEATURE_DSM_WM_TRACING
    /* Increment the lo_event_cnt */
    DSMI_INCR_COUNT(wm_ptr,lo);
    #endif
    DSM_WM_CB(wm_ptr->lowater_func_ptr,wm_ptr,
              wm_ptr->lowater_func_data);
  }
  
  if(initial_count != 0 && 
     wm_ptr->current_cnt == 0)
  {
    #ifdef FEATURE_DSM_WM_TRACING
    /* Increment the empty_event_cnt */
    DSMI_INCR_COUNT(wm_ptr,empty);
    #endif
    DSM_WM_CB(wm_ptr->gone_empty_func_ptr,wm_ptr,
              wm_ptr->gone_empty_func_data);
  }
  
 #ifdef FEATURE_DSM_MEM_CHK
  if(wm_ptr->current_cnt > wm_ptr->highest_cnt)
  {
    wm_ptr->highest_cnt = wm_ptr->current_cnt;
  }
 #endif /* FEATURE_DSM_MEM_CHK */  

 #ifdef FEATURE_DSM_WM_TRACING
 DSMI_MIN_MAX_CNT(wm_ptr);
 #endif /* FEATURE_DSM_WM_TRACING */

} /* dsmi_wm_check_levels */


/*===========================================================================
FUNCTION DSMI_WM_GET_COUNT()

DESCRIPTION
   Traverse the watermark queue and get the actual count of bytes on it.
   This is EXPENSIVE and is only used internally for debug.

DEPENDENCIES
  None

PARAMETERS
   wm_ptr - Pointer to the watermark item header.  This has the pointers
            to the callbacks and the callback data associated with this 
            watermark.

RETURN VALUE
  Count of bytes on watermark queue.

SIDE EFFECTS
  None.
===========================================================================*/
uint32
dsmi_wm_get_count(dsm_watermark_type * wm_ptr)
{
#ifdef FEATURE_DSM_MEM_CHK_EXPENSIVE_VERIFY_QUEUE_LENGTH
  uint32 count = 0;
  dsm_item_type * dsm_ptr;

  if( wm_ptr->q_ptr != NULL )
  {
    dsm_ptr = (dsm_item_type*)q_check( wm_ptr->q_ptr );
    while( dsm_ptr != NULL )
    {
      count += dsm_ptr->used;
      dsm_ptr = (dsm_item_type*)q_next(wm_ptr->q_ptr,&(dsm_ptr->link));
    }
  }
  return count;
#else
  MSG(MSG_SSID_DFLT, MSG_LEGACY_FATAL,"FEATURE_DSM_MEM_CHK_EXPENSIVE_VERIFY_QUEUE_LENGTH needs to defined");
  return 0;
#endif /* FEATURE_DSM_MEM_CHK_EXPENSIVE_VERIFY_QUEUE_LENGTH */
} /* dsmi_wm_get_count */



/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNALIZED FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================
FUNCTION DSM_QUEUE_INIT()

DESCRIPTION
   This function initializes a watermark queue.  Setting all the callbacks and 
   callback data to NULL, watermark levels to 0, and initializing the queue 
   that this will use. Since this potentially initializes a mutex, the
   dsm_queue_destroy() function needs to be called to tear down a WM queue.

DEPENDENCIES
   None

PARAMETERS
   wm_ptr - Pointer to the watermark to initialize
   dne - Do not exceed level for this watermark
   queue - Pointer to the queue header that this water mark should use

RETURN VALUE
   None

SIDE EFFECTS
   Queue is initialized
   Mutexes might be initialized.
===========================================================================*/
void dsm_queue_init
(
  dsm_watermark_type *wm_ptr,
  int dne,
  q_type * queue
)
{
  dword lock_init;
  uint32 sentinel;  /* Cookie to check watermark initialization */

  DSM_ASSERT(wm_ptr != NULL);
  DSM_ASSERT(queue != NULL);        /* You must have a queue */

  sentinel = wm_ptr->sentinel;  /* Save the previous cookie */
  lock_init = wm_ptr->lock;
  memset(wm_ptr, 0, sizeof(dsm_watermark_type));
  wm_ptr->dont_exceed_cnt = dne;

  //Only create lock once. Check watermark is initialized or not.
  if (sentinel != DSM_QUEUE_SENTINEL) {
    DSMI_QUEUE_LOCK_CREATE(wm_ptr);
    memset(queue, 0, sizeof(q_type));
    (void)q_init(queue);     
  }
  else {
    wm_ptr->lock = lock_init;
  }
  wm_ptr->q_ptr = queue;
  
  wm_ptr->sentinel = DSM_QUEUE_SENTINEL;

}

/*===========================================================================
FUNCTION DSM_ENQUEUE()

DESCRIPTION
  This function will put the passed DSM item to the passed shared queue then
  check for and perform any 'put' events.

DEPENDENCIES
  1) Both parameters must NOT be NULL.
  2) The prioritized queuing will always put a DSM_HIGHEST priority item to
     the head of the queue.

PARAMETERS
  wm_ptr - Pointer to Watermark item to put to
  pkt_head_ptr - Pointer to pointer to item to add to queue

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsmi_enqueue
(
  dsm_watermark_type *wm_ptr,
  dsm_item_type **pkt_head_ptr,
  const char * file,
  uint32 line
)
{
  uint32 initial_count;          /* initial count of bytes in WM           */
  uint32 item_length;            /* length of new item to add              */

  dsm_item_type *insert_ptr;     /* pointer to 1st item in queue           */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DSMI_CHECK_WM_PTR(wm_ptr);

  DSM_ASSERT(wm_ptr->q_ptr != NULL);

  if((pkt_head_ptr == NULL) || (*pkt_head_ptr == NULL))
  {
#ifdef FEATURE_DSM_MEM_CHK
    ERR_FATAL("dsm_enqueue: Invalid Parameter", 0, 0, 0);
#else
    return;
#endif /*FEATURE_DSM_MEM_CHK */
  }

#ifdef FEATURE_DSM_MEM_CHK
  /* Check if this item is already enqueued in some queue. */
  DSM_ASSERT((*pkt_head_ptr)->link.next_ptr == NULL);
#endif

#ifdef FEATURE_DSM_MEM_CHK_EXPENSIVE_VERIFY_PACKET_ENQUEUE
  dsmi_verify_packet(*pkt_head_ptr);
#endif /* FEATURE_DSM_MEM_CHK_EXPENSIVE_VERIFY_PACKET_ENQUEUE */

#ifdef FEATURE_DSM_MEM_CHK_EXPENSIVE_VERIFY_QUEUE_LENGTH
  DSMI_QUEUE_LOCK_WM(wm_ptr);
  DSM_ASSERT( dsmi_wm_get_count(wm_ptr) == wm_ptr->current_cnt );
  DSMI_QUEUE_UNLOCK_WM(wm_ptr);
#endif /* FEATURE_DSM_MEM_CHK_EXPENSIVE_VERIFY_QUEUE_LENGTH */


#ifdef FEATURE_DSM_MEM_CHK
  dsmi_verify_pool_id((*pkt_head_ptr)->pool_id);

  if(wm_ptr->dont_exceed_cnt < 1)
  {
    ERR_FATAL("dsm_enqueue: Impossibly small dont exceed level for WM 0x%x",
              (int)wm_ptr,0,0);
  }
#endif

  /*-------------------------------------------------------------------------
    If the Watermark Item does not now have too many bytes in it then 
    proceed to stuff the passed item into it. Check 'priority'field to 
    determine whether or not the item should be put to front of queue.
  -------------------------------------------------------------------------*/
  item_length = dsm_length_packet(*pkt_head_ptr);

  #ifdef FEATURE_DSM_MEM_CHK
  if ((*pkt_head_ptr)->references == 0) 
    {
      ERR_FATAL("References are 0 for pkt 0x%x while inserting in WM 0x%x",
                (int)*pkt_head_ptr, (int)wm_ptr, 0);
    }
  #endif

  DSMI_QUEUE_LOCK_WM(wm_ptr);
  if((wm_ptr->current_cnt + item_length) <= wm_ptr->dont_exceed_cnt)
  {
    initial_count = wm_ptr->current_cnt;
    wm_ptr->total_rcvd_cnt += item_length;

#ifdef FEATURE_DSM_MEM_CHK
    dsmi_touch_item(*pkt_head_ptr,file,line);
#endif /* FEATURE_DSM_MEM_CHK */


    if((*pkt_head_ptr)->priority == DSM_HIGHEST && q_cnt(wm_ptr->q_ptr) != 0)
    {
      insert_ptr = (dsm_item_type *)q_check( wm_ptr->q_ptr);
    /*-----------------------------------------------------------------------
      The following functionality ensures that a message with dsm_highest
      is not added to a queue before another message with dsm_highest 
      priority. (ds_to_ps_q was in mind when adding this functionality).
    -----------------------------------------------------------------------*/
      while((insert_ptr != NULL) && (insert_ptr->priority == DSM_HIGHEST))
      {
        /*------------------------------------------------------------------- 
          q_next returns next item in q or null if end of queue reached 
        -------------------------------------------------------------------*/
        insert_ptr = (dsm_item_type *)q_next(wm_ptr->q_ptr, 
                                             &insert_ptr->link);
      }

      if(insert_ptr == NULL)
      {
        /*-------------------------------------------------------------------
          Puts item at the tail of the queue if all the items in the queue
          are dsm_highest.
        -------------------------------------------------------------------*/
        q_put(wm_ptr->q_ptr, &(*pkt_head_ptr)->link);
      }
      else
      {
        /*-------------------------------------------------------------------
          Inserts item before the last non-highest item on the queue.
        -------------------------------------------------------------------*/
#ifdef FEATURE_Q_NO_SELF_QPTR
         q_insert(wm_ptr->q_ptr, 
                  &(*pkt_head_ptr)->link,
                  &insert_ptr->link);
#else
         q_insert((q_link_type *)(&(*pkt_head_ptr)->link), 
                  &insert_ptr->link);
#endif

      }

    }  /* if (*pkt_head_ptr.........)*/
    else 
    {
      /*--------------------------------------------------------------------- 
        Put to queue at end of list.
      ---------------------------------------------------------------------*/
      q_put(wm_ptr->q_ptr, &(*pkt_head_ptr)->link);
    }
    wm_ptr->current_cnt += item_length;

    /*-----------------------------------------------------------------------
      Now check for the Non-empty and Hi-watermark events.
    -----------------------------------------------------------------------*/
    dsmi_wm_check_levels(wm_ptr, initial_count, TRUE);

    *pkt_head_ptr = NULL;

  }
  else 
  {
    #ifdef FEATURE_DSM_WM_TRACING
    /* Incremement the dont exceed event count */
    DSMI_INCR_COUNT(wm_ptr,dne);
    #endif
    /*----------------------------------------------------------------------- 
      Display message, put item to free queue. 
    -----------------------------------------------------------------------*/
    ERR("WM full,freeing packet 0x%x:Watermark 0x%x:Tried %d",
        (int) *pkt_head_ptr,
        (int) wm_ptr,
        (int) item_length
        );

    (void)dsmi_free_packet(pkt_head_ptr,file,line);
  }
  DSMI_QUEUE_UNLOCK_WM(wm_ptr);

} /* dsmi_enqueue() */

/*===========================================================================
FUNCTION DSM_SIMPLE_ENQUEUE_ISR()

DESCRIPTION
  This function will put the passed DSM item to the passed shared queue then
  check for and perform any 'put' events.  This function does not check
  for priority.  It simply enqueues to the tail of the queue.

DEPENDENCIES
  1) Both parameters must NOT be NULL.
  2) Does not support packet chaining.
  3) Should only be called from ISR or from within critical section in which
     interrupts are disabled.

PARAMETERS
  wm_ptr - Pointer to watermark to put to
  pkt_head_ptr - Pointer to pointer to item to put.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsmi_simple_enqueue_isr
(
  dsm_watermark_type *wm_ptr,
  dsm_item_type **pkt_head_ptr,
  const char * file,
  uint32 line
)
{
  dsm_item_type *temp_ptr;      /* pointer to 1st item in queue */
  uint32 initial_count;		/* initial count of bytes in WM */
  uint32 item_length;		/* length of new item to add */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  
  DSM_ASSERT(pkt_head_ptr != NULL && *pkt_head_ptr != NULL);
  DSMI_CHECK_WM_PTR(wm_ptr);
  DSM_ASSERT(wm_ptr->q_ptr != NULL);

#ifdef FEATURE_DSM_MEM_CHK
  dsmi_verify_pool_id((*pkt_head_ptr)->pool_id);
#endif

  #ifdef FEATURE_DSM_MEM_CHK
  /* Check if this item is already enqueued in some queue. */
  DSM_ASSERT((*pkt_head_ptr)->link.next_ptr == NULL);
  #endif

  temp_ptr = *pkt_head_ptr;

  DSM_ASSERT(temp_ptr->pkt_ptr == NULL);

  /*-------------------------------------------------------------------------
    If the Watermark Item does not now have too many bytes in it then 
    proceed to stuff the passed item into it. Check 'priority'field to 
    determine whether or not the item should be put to front of queue.
  -------------------------------------------------------------------------*/
  /* promote from 16bit to 32bit */
  item_length = temp_ptr->used;

  #ifdef FEATURE_DSM_MEM_CHK
  if ((*pkt_head_ptr)->references == 0) 
    {
      ERR_FATAL("References are 0 for packet 0x%x while inserting within WM 0x%x",
                (int)*pkt_head_ptr, (int)wm_ptr, 0);
    }
  #endif
  DSMI_QUEUE_LOCK_WM(wm_ptr);
  if((wm_ptr->current_cnt + item_length) <= wm_ptr->dont_exceed_cnt)
  {
    initial_count = wm_ptr->current_cnt;
    wm_ptr->total_rcvd_cnt += item_length;

#ifdef FEATURE_DSM_MEM_CHK
    dsmi_touch_item(*pkt_head_ptr,file,line);
#endif /* FEATURE_DSM_MEM_CHK */

    q_put(wm_ptr->q_ptr, &(temp_ptr->link));
    wm_ptr->current_cnt += item_length;

    /*-----------------------------------------------------------------------
      Now check for the Non-empty and Hi-watermark events.
    -----------------------------------------------------------------------*/
    dsmi_wm_check_levels(wm_ptr, initial_count, TRUE); 
  }
  else 
  {
    #ifdef FEATURE_DSM_WM_TRACING
    /* Incremement the dont exceed event count */
    DSMI_INCR_COUNT(wm_ptr,dne);
    #endif
    /*----------------------------------------------------------------------- 
      Display message, put item to free queue 
    -----------------------------------------------------------------------*/
    ERR("WM full,freeing packet 0x%x:Watermark 0x%x:Tried %d",
        (int) *pkt_head_ptr,
        (int) wm_ptr,
        (int) item_length
        );
    (void)dsmi_free_buffer(*pkt_head_ptr,file,line);
  }
  *pkt_head_ptr = NULL;
  DSMI_QUEUE_UNLOCK_WM(wm_ptr);

} /* dsm_simple_enqueue_isr() */

/*===========================================================================
FUNCTION DSM_DEQUEUE()

DESCRIPTION
  This function will return a pointer to the next item on the shared queue
  associated with the passed Watermark item. This function will also update
  the 'current_cnt' field in the passed Watermark item and check for and
  perform any relevent 'get' events.

DEPENDENCIES
  The parameter must NOT be NULL.

PARAMETERS
  wm_ptr - Pointer to watermark item to get item from 

RETURN VALUE
  A pointer to a 'dsm_item_type' or NULL if no item_array available.

SIDE EFFECTS
  None
===========================================================================*/
dsm_item_type *dsmi_dequeue
(
 dsm_watermark_type *wm_ptr,
 const char * file, 
 uint32 line
)
{
  uint32 initial_count;		/* initial number of bytes in watermark */
  dsm_item_type *item_ptr;      /* pointer to item to retrieve */
  uint32 item_length;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DSMI_CHECK_WM_PTR(wm_ptr);
  DSM_ASSERT(wm_ptr->q_ptr != NULL);

  DSMI_QUEUE_LOCK_WM(wm_ptr);

#ifdef FEATURE_DSM_MEM_CHK_EXPENSIVE_VERIFY_QUEUE_LENGTH
  DSM_ASSERT( dsmi_wm_get_count(wm_ptr) == wm_ptr->current_cnt );
#endif /* FEATURE_DSM_MEM_CHK_EXPENSIVE_VERIFY_QUEUE_LENGTH */

  if((item_ptr = (dsm_item_type *)q_get(wm_ptr->q_ptr)) != NULL)
  {
    if (item_ptr->references == 0) 
    {
      ERR_FATAL("References are 0 for item 0x%x accessed within WM 0x%x",
                (int)item_ptr, (int)wm_ptr, 0);
    }
    /*-----------------------------------------------------------------------
      First get current count then get the item and update current count.
      Check for and perform Lo water and Gone Empty events if appropriate.
    -----------------------------------------------------------------------*/
#ifdef FEATURE_DSM_MEM_CHK
    dsmi_touch_item(item_ptr,file,line);
#endif /* FEATURE_DSM_MEM_CHK */
    
    initial_count = wm_ptr->current_cnt;
    item_length = dsm_length_packet(item_ptr);
    if (wm_ptr->current_cnt < item_length) 
    {
      ERR_FATAL("Current count for WM %x is less that length for item 0x%x"
      , (int)wm_ptr, (int)item_ptr, 0);
    }
    wm_ptr->current_cnt -= item_length;

    dsmi_wm_check_levels(wm_ptr, initial_count, FALSE);
  }
  else
  {
    if (wm_ptr->current_cnt != 0) 
    {
      ERR_FATAL("WM 0x%x current_cnt is not 0 ", (int)wm_ptr, 0, 0);
    }
  }
  DSMI_QUEUE_UNLOCK_WM(wm_ptr);
  return item_ptr;

} /* dsm_dequeue() */

/*===========================================================================
FUNCTION DSM_EMPTY_QUEUE()

DESCRIPTION
  This routine completely empties a queue.
  
DEPENDENCIES
  None

PARAMETERS
  wm_ptr - Pointer to watermark queue to empty

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsmi_empty_queue
( 
  dsm_watermark_type *wm_ptr,
  const char * file,
  uint32 line
)
{
  dsm_item_type *item_ptr;	

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DSMI_CHECK_WM_PTR(wm_ptr);
  DSM_ASSERT(wm_ptr != NULL);
  DSM_ASSERT(wm_ptr->q_ptr != NULL);

  while((item_ptr = dsmi_dequeue(wm_ptr,file,line)) != NULL)
  {
    (void)dsmi_free_packet(&item_ptr,file,line);
  } 
} /* dsm_empty_queue() */


/*===========================================================================
FUNCTION DSM_IS_WM_EMPTY()

DESCRIPTION
 This routine determines whether the input watermark has data queued in
 it or not.

DEPENDENCIES
 None

PARAMETERS
 wm_ptr - Pointer to a watermark

RETURN VALUE
 TRUE if watermark has no data queued in it, FALSE if it does

SIDE EFFECTS
 None
===========================================================================*/
boolean dsm_is_wm_empty
(
 dsm_watermark_type *wm_ptr
)
{
 DSMI_CHECK_WM_PTR(wm_ptr);
 DSM_ASSERT(wm_ptr->q_ptr != NULL);

 return (boolean) (q_cnt (wm_ptr->q_ptr) == 0);
} /* dsm_is_wm_empty */


/*===========================================================================
FUNCTION DSM_SET_LOW_WM()

DESCRIPTION
 This routine resets the low watermark value. This change may trigger
 watermark callbacks.

DEPENDENCIES
 None

PARAMETERS
 wm_ptr - Pointer to a watermark
 val    - New value for low watermark.

RETURN VALUE
 None.

SIDE EFFECTS
 None
===========================================================================*/
void dsm_set_low_wm
(
  dsm_watermark_type *wm_ptr,
  uint32             val
)
{
  DSMI_CHECK_WM_PTR(wm_ptr);
  DSMI_QUEUE_LOCK_WM(wm_ptr);
  if(wm_ptr->current_cnt >= wm_ptr->lo_watermark &&
     wm_ptr->current_cnt < val )
  {
    DSM_WM_CB(wm_ptr->lowater_func_ptr,wm_ptr,wm_ptr->lowater_func_data);
  }
  wm_ptr->lo_watermark = val;
  DSMI_QUEUE_UNLOCK_WM(wm_ptr);
} /* dsm_set_low_wm */

/*===========================================================================
FUNCTION DSM_SET_HI_WM()

DESCRIPTION
 This routine resets the high watermark value. This change may trigger
 watermark callbacks.

DEPENDENCIES
 None

PARAMETERS
 wm_ptr - Pointer to a watermark
 val    - New value for hi watermark.

RETURN VALUE
 None.

SIDE EFFECTS
 WB callback triggered. Function locks the context for the extent of the
 callback.
===========================================================================*/
void dsm_set_hi_wm
(
  dsm_watermark_type *wm_ptr,
  uint32             val
)
{
  DSMI_CHECK_WM_PTR(wm_ptr);
  DSMI_QUEUE_LOCK_WM(wm_ptr);  
  if(wm_ptr->current_cnt <= wm_ptr->hi_watermark &&
     wm_ptr->current_cnt > val )
  {
    DSM_WM_CB(wm_ptr->hiwater_func_ptr,wm_ptr,wm_ptr->hiwater_func_data);
  }
  wm_ptr->hi_watermark = val;
  DSMI_QUEUE_UNLOCK_WM(wm_ptr);
} /* dsm_set_hi_wm */

/*===========================================================================
FUNCTION DSM_SET_DNE()

DESCRIPTION
 This routine resets the DNE (do not exceed) value.
 
DEPENDENCIES
 None

PARAMETERS
 wm_ptr - Pointer to a watermark
 val    - New value for hi watermark.

RETURN VALUE
 None.

SIDE EFFECTS
 WB callback triggered. Function locks the context for the extent of the
 callback.
===========================================================================*/
void dsm_set_dne
(
  dsm_watermark_type *wm_ptr,
  uint32             val
)
{
  DSMI_CHECK_WM_PTR(wm_ptr);
  DSMI_QUEUE_LOCK_WM(wm_ptr);  
  wm_ptr->dont_exceed_cnt = val;
  DSMI_QUEUE_UNLOCK_WM(wm_ptr);
}

/*===========================================================================
FUNCTION DSM_QUEUE_CNT()

DESCRIPTION
 Returns the number of bytes on the watermark queue.
 
DEPENDENCIES
 None

PARAMETERS
 wm_ptr - Pointer to a watermark

RETURN VALUE
 Number of bytes recorded on queue.

SIDE EFFECTS
 None.
===========================================================================*/
uint32 dsm_queue_cnt
(
  dsm_watermark_type *wm_ptr
)
{
  uint32 ret;
  DSMI_CHECK_WM_PTR(wm_ptr);
  DSMI_QUEUE_LOCK_WM(wm_ptr);
  ret = wm_ptr->current_cnt;
  DSMI_QUEUE_UNLOCK_WM(wm_ptr);
  return ret;
} 
/*===========================================================================
FUNCTION DSM_QUEUE_DESTROY()

DESCRIPTION
   This function tears down a watermark queue.
DEPENDENCIES
   None

PARAMETERS
   wm_ptr - Pointer to the watermark to initialize

RETURN VALUE
   None

SIDE EFFECTS
   Locks might be destroyed.
   Packets might be freed.
   WM pointer will be non-initialized.
===========================================================================*/
void dsmi_queue_destroy
(
  dsm_watermark_type *wm_ptr,
  const char * file,
  uint32 line
)
{
  dsm_item_type * item_ptr;
  DSMI_CHECK_WM_PTR(wm_ptr);
  DSMI_QUEUE_LOCK_WM(wm_ptr);
  while ( wm_ptr->q_ptr != NULL &&
          ( item_ptr = (dsm_item_type *)q_get(wm_ptr->q_ptr)) != NULL ) 
  {
    (void)dsmi_free_packet(&item_ptr,file,line);
  }
#ifdef FEATURE_DSM_Q_DESTROY
  q_destroy(wm_ptr->q_ptr);
#endif
  DSMI_QUEUE_UNLOCK_WM(wm_ptr);
  DSMI_QUEUE_LOCK_DESTROY(wm_ptr);
  memset(wm_ptr, 0, sizeof(dsm_watermark_type));
}

/*===========================================================================
FUNCTION DSM_GET_WM_STATS

DESCRIPTION
   This function returns the min_cnt, max_cnt, HI, LO, DNE counts of a
   watermark queue. It also resets the min and max counts for the next window
   period.
 
DEPENDENCIES
  If FEATURE_DSM_WM_TRACING is not defined, this function will 
  ERR_FATAL().

PARAMETERS
   wm_ptr - Pointer to the watermark queue.
   wm_cnts - Pointer to the memory location where this function will write
     the min_cnt, max_cnt, HI, LO, DNE counts.

RETURN VALUE
   None

SIDE EFFECTS
   None
===========================================================================*/

void dsm_get_wm_stats
(
  dsm_watermark_type*     wm_ptr,
  dsm_wm_counts_type*     wm_cnts
)
{
#ifdef FEATURE_DSM_WM_TRACING
  
  dsm_watermark_trace_type* trace_array = NULL; 
  /* Check if the wm_ptr is NULL */
  DSMI_CHECK_WM_PTR(wm_ptr);

  /* Check if the wm_cnts pointer is NULL */
  DSM_ASSERT(wm_cnts != NULL);

  DSMI_QUEUE_LOCK_WM(wm_ptr);

  /* Fill the watermark count structure with all the values */
  wm_cnts->lo_watermark         =  wm_ptr->lo_watermark;
  wm_cnts->hi_watermark         =  wm_ptr->hi_watermark;
  wm_cnts->dont_exceed_cnt      =  wm_ptr->dont_exceed_cnt;
  wm_cnts->current_cnt          =  wm_ptr->current_cnt;
  wm_cnts->highest_cnt          =  wm_ptr->highest_cnt;
  wm_cnts->total_rcvd_cnt       =  wm_ptr->total_rcvd_cnt;

  /* Check if the queue is empty */
  if(wm_ptr->q_ptr != NULL)
  {
    wm_cnts->q_cnt              =  wm_ptr->q_ptr->cnt;
  }
  else 
  {
    wm_cnts->q_cnt              =  0;
  }

  trace_array                   =  DSMI_GET_TRACE_ARR(wm_ptr);
  /* Copy all the counts */  
  wm_cnts->min_cnt              =  trace_array->min_cnt;
  wm_cnts->max_cnt              =  trace_array->max_cnt;

  wm_cnts->hi_event_cnt         =  trace_array->hi_event_cnt;
  wm_cnts->lo_event_cnt         =  trace_array->lo_event_cnt;
  wm_cnts->dne_event_cnt        =  trace_array->dne_event_cnt;
  wm_cnts->empty_event_cnt      =  trace_array->empty_event_cnt;

  /* Reset the min, max and event counts to current_cnt for the next
     window period */
  trace_array->min_cnt          =  wm_ptr->current_cnt;
  trace_array->max_cnt          =  wm_ptr->current_cnt;
  trace_array->hi_event_cnt     =  0;
  trace_array->lo_event_cnt     =  0;
  trace_array->dne_event_cnt    =  0;
  trace_array->empty_event_cnt  =  0;
 
  DSMI_QUEUE_UNLOCK_WM(wm_ptr);
#else
  ERR_FATAL("FEATURE_DSM_WM_TRACING is not defined.", 0, 0, 0);
#endif /* FEATURE_DSM_WM_TRACING */

} /* dsm_get_wm_stats */

/*===========================================================================
FUNCTION DSM_MIN_MAX_CNT()

DESCRIPTION
   This function updates the min_cnt and max_cnt of the watermark queue
   that is passed.
 
DEPENDENCIES

PARAMETERS
   wm_ptr      - Pointer to the watermark queue.
   trace_array - Starting address of the array where the trace counts 
                 are stored.

RETURN VALUE
   None

SIDE EFFECTS
   None
===========================================================================*/
void dsm_min_max_cnt
(
  dsm_watermark_type * wm_ptr,
  dsm_watermark_trace_type * trace_array
) 
{
#ifdef FEATURE_DSM_WM_TRACING

  /* Update min_cnt if the current_cnt has gone below min_cnt for the
    current period */

  if(wm_ptr->current_cnt < trace_array->min_cnt) 
  {
    trace_array->min_cnt = wm_ptr->current_cnt;
  }

  /* Update max_cnt if the current_cnt has exceeded max_cnt for the
     current period */

  if(wm_ptr->current_cnt > trace_array->max_cnt) 
  {
    trace_array->max_cnt = wm_ptr->current_cnt;
  }
#else
  DSM_NOTUSED(wm_ptr);
  DSM_NOTUSED(trace_array);
#endif /*FEATURE_DSM_WM_TRACING*/
}

