#ifndef UART_EVENT_QUEUE_H
#define UART_EVENT_QUEUE_H
/*============================================================================
 
GENERAL DESCRIPTION

  Defines an event queue object.

EXTERNALIZED FUNCTIONS
    
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1999-2000, 2007-2008 Qualcomm Technologies Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary
============================================================================*/

/*============================================================================

EDIT HISTORY FOR MODULE

$Header: //components/rel/core.qdsp6/2.1.c4/buses/uart/mdd/inc/uart_event_queue.h#1 $

when          who        what, where, why
-----------   --------   -----------------------------------------------------
06-Aug-2011   cmihalik   Initial revision.

============================================================================*/

#include "comdef.h"

#include "DALSys.h"

//----------------------------------------------------------------------------
//  uart_event_queue_type
//----------------------------------------------------------------------------

#define UART_EVENT_QUEUE_SIZE   20

//  If the storage for the queue is allocated on the heap,
//  then this structure definition can be moved to the .c 
//  file and handled as an opaque type.

typedef struct uart_event_queue_type
{
   DALSYSSyncHandle            lock;
   uint32                      items[UART_EVENT_QUEUE_SIZE];
   uint32*                     end_ptr;
   uint32*                     read_ptr;
   uint32*                     write_ptr;
   int                         num_items;
   int                         max_items;

}  uart_event_queue_type;

/*============================================================================
FUNCTION:      uart_event_queue_init

DESCRIPTION:   Initialize the queue.

DEPENDENCIES:  None.

PARAMETERS:    q_ptr - pointer to the queue.

RETURN VALUE:  None.

SIDE EFFECTS:  The queue is reinitialized to contain no data.
============================================================================*/

boolean
uart_event_queue_init( uart_event_queue_type* q_ptr );

/*============================================================================
FUNCTION:      uart_event_queue_deinit

DESCRIPTION:   De-initialize the queue.

DEPENDENCIES:  None.

PARAMETERS:    q_ptr - pointer to the queue.

RETURN VALUE:  None.

SIDE EFFECTS:  The queue is reinitialized to contain no data.
============================================================================*/

void                           
uart_event_queue_deinit( uart_event_queue_type* q_ptr );

/*============================================================================
FUNCTION:      uart_event_queue_add_item

DESCRIPTION:   Add an item to the queue.

DEPENDENCIES:  None.

PARAMETERS:    q_ptr - pointer to the queue.

RETURN VALUE:  TRUE if the item was added (there was room);
               FALSE otherwise.

SIDE EFFECTS:  
============================================================================*/

boolean
uart_event_queue_add_item( uart_event_queue_type* q_ptr, uint32 item );

/*============================================================================
FUNCTION:      uart_event_remove_all_items

DESCRIPTION:   Remove all items from the queue.  This is so the queue items
               can be processed without holding the lock.

DEPENDENCIES:  None.

PARAMETERS:    q_ptr - pointer to the queue.

               item  - pointer to the storage area for the returned items. 

               num_items - pointer to the number of items in the queue.
                           On input, this is the number of items in the
                           passed in item ptr array.

RETURN VALUE:  

SIDE EFFECTS:  
============================================================================*/

void
uart_event_queue_remove_all_items( uart_event_queue_type* q_ptr, uint32* item, int* num_items );

#endif

