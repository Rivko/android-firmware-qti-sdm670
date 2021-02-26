
/*============================================================================
 
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS
    
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1999-2000, 2007-2008 Qualcomm Technologies Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary
============================================================================*/

/*============================================================================

EDIT HISTORY FOR MODULE

$Header: //components/rel/core.qdsp6/2.1/buses/uart/mdd/src/uart_event_queue.c#1 $

when          who        what, where, why
-----------   --------   -----------------------------------------------------
06-Aug-2011   cmihalik   Initial revision.

============================================================================*/

#include "comdef.h"

#include "assert.h"

#include "uart_event_queue.h"
#include "uart_log.h"

//----------------------------------------------------------------------------
//  UART_TRACE_BUFFER_FILE_ID must be unique amongst all files writing to the
//  trace buffer.
//----------------------------------------------------------------------------

#define  UART_TRACE_BUFFER_FILE_ID            6

//----------------------------------------------------------------------------
//  uart_event_queue_init
//----------------------------------------------------------------------------

boolean
uart_event_queue_init( uart_event_queue_type* q_ptr )
{
   DALResult          dal_result;



   if( NULL == q_ptr )
      return( FALSE );

   //  Create lock.

   dal_result = DALSYS_SyncCreate( DALSYS_SYNC_ATTR_RESOURCE, &q_ptr->lock, NULL );
   if( dal_result != DAL_SUCCESS )
   {
      return( FALSE );
   }

   q_ptr->num_items = 0;
   q_ptr->max_items = UART_EVENT_QUEUE_SIZE;
   q_ptr->end_ptr   = &q_ptr->items[q_ptr->max_items];
   q_ptr->read_ptr  = q_ptr->items;
   q_ptr->write_ptr = q_ptr->items;

   return( TRUE );
}

//----------------------------------------------------------------------------
//  uart_event_queue_deinit
//----------------------------------------------------------------------------

void
uart_event_queue_deinit( uart_event_queue_type* q_ptr )
{
   if( NULL == q_ptr )
      return;

   //  Free the lock.

   DALSYS_DestroyObject( &q_ptr->lock );
   q_ptr->lock = NULL;
}

//----------------------------------------------------------------------------
//  uart_event_queue_add_item
//----------------------------------------------------------------------------

boolean
uart_event_queue_add_item( uart_event_queue_type* q_ptr, uint32 item )
{
   boolean          isAdded = FALSE;


   if( NULL == q_ptr )
      return( FALSE );

   if( NULL == q_ptr->lock )
      return( FALSE );

   //  Lock the queue.

   DALSYS_SyncEnter( q_ptr->lock );

   //  Add the item.

   if( q_ptr->num_items < q_ptr->max_items )
   {
      q_ptr->num_items++;

      *q_ptr->write_ptr++ = item;
      if( q_ptr->write_ptr >= q_ptr->end_ptr )
      {
         q_ptr->write_ptr = q_ptr->items;
      }

      isAdded = TRUE;
   }

   //  Unlock the queue.

   DALSYS_SyncLeave( q_ptr->lock );

   return( isAdded );
}

//----------------------------------------------------------------------------
//  uart_event_queue_remove_all_items
//----------------------------------------------------------------------------

void                           
uart_event_queue_remove_all_items( uart_event_queue_type* q_ptr, uint32* dest, int* p_num_items )
{
   int              total_num_items;
   int              num_items;
   int              num_bytes;



   if( NULL == q_ptr )
      return;

   if( NULL == dest )
      return;

   if( NULL == p_num_items )
      return;

   if( NULL == q_ptr->lock )
      return;

   //  Lock.

   DALSYS_SyncEnter( q_ptr->lock );

   //  Error checking.

   if( !(( 0 <= *p_num_items ) && ( *p_num_items <= q_ptr->max_items )))
   {
      DALSYS_SyncLeave( q_ptr->lock );
      return;
   }
   
   //  Return items.  read_ptr == write_ptr can happen if the queue is 
   //  empty or the queue is full.  We know the queue is empty if num_items
   //  is 0.  Otherwise, we can read the 

   total_num_items = 0;

   if( 0 == q_ptr->num_items )
   {
      //  Queue is empty.  Do nothing.

      total_num_items = 0;
   }
   else if( q_ptr->read_ptr < q_ptr->write_ptr )
   {
      //  Can read to write ptr.

      num_items = q_ptr->write_ptr - q_ptr->read_ptr;
      num_bytes = num_items * sizeof( q_ptr->items[0] );

      memcpy( dest, q_ptr->read_ptr, num_bytes );

      total_num_items = num_items;
   }
   else
   {
      //  read_ptr to end of buffer.

      num_items = q_ptr->end_ptr - q_ptr->read_ptr;
      num_bytes = num_items * sizeof( q_ptr->items[0] );

      memcpy( dest, q_ptr->read_ptr, num_bytes );
      dest += num_items;

      total_num_items = num_items;

      //  Beginning of buffer to write_ptr.

      num_items = q_ptr->write_ptr - q_ptr->items;
      num_bytes = num_items * sizeof( q_ptr->items[0] );

      memcpy( dest, q_ptr->items, num_bytes );

      total_num_items += num_items;
   }

   *p_num_items = total_num_items;

   //  Queue is now empty.

   q_ptr->num_items = 0;
   q_ptr->read_ptr  = q_ptr->items;
   q_ptr->write_ptr = q_ptr->items;

   //  Unlock.

   DALSYS_SyncLeave( q_ptr->lock );
}

