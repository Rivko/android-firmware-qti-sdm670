/*=============================================================================

                   T I M E   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  Implements functionality to save offsets into efs item file.

EXTERNALIZED FUNCTIONS
  time_efs_rw 
    Local function that provides IO access for generic offsets in EFS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
    None

Copyright (c) 2009 - 2013 by QUALCOMM Technologies Incorporated.  All Rights Reserved.


=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1/services/time/src/timer_v.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/23/10   din     Changed to handle allocation failure in malloc.
04/27/10   din     Fixed compiler warnings.
04/19/10   din     Prevented Multiple efs_writes for same file. Reinsertion of 
                   same element was prevented.
03/11/10   din     Moved efs write to Time IPC task.
03/10/10   din     Removed efs_unlink() as efs_put() overwites the item file.
17/10/09   din     File created. 

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "timer_v.h"
#include "assert.h"

/*=============================================================================

                         DATA DEFINITIONS
=============================================================================*/
static timers_type*                timers; 

//#define timers *timersPtr;


/*=============================================================================

                         FUNCTION DEFINITIONS
=============================================================================*/
void timer_v_init(void)
{
  timers=timer_get_active_list();
}
/*=============================================================================

                           TIMER BIN FUNCTIONS

=============================================================================*/

#ifdef FEATURE_USE_TIMER_BIN
/*=============================================================================

FUNCTION TIMER_BIN_LIST_INSERT_BETWEEN

DESCRIPTION
  Inserts a bin into the specified list between the two specified bin positions 

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void timer_bin_list_insert_between
(
  /* Bin list to insert bin into */
  timer_bin_list_ptr              list, 

  /* Bin to insert in bin list */
  timer_bin_ptr                   bin,

  /* Which node bin should be inserted after */
  timer_bin_ptr                   ptr1,

  /* Which node bin should be inserted before */
  timer_bin_ptr                   ptr2
)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Update our bin's prev/next ptrs to point at correct bin structs */
  bin->prev_bin = ptr1; 
  bin->next_bin = ptr2;


  /* Update surrounding prev/next ptrs (if necessary) to point to our 
     newly inserted bin */

  if( ptr1 != NULL )
  {
    ptr1->next_bin = bin;
  }
  else
  {
    list->first = bin; /* we have a new start of list, update first ptr */
  } 

  if( ptr2 != NULL )
  {
    ptr2->prev_bin = bin;
  }
  else
  {
    list->last = bin; /* we have a new end of list, update last ptr */
  }

  list->count++;

} /* timer_bin_list_insert_between */



/*=============================================================================

FUNCTION TIMER_BIN_LIST_REMOVE

DESCRIPTION
  Removes a bin from the specified list

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  The popped timer_bin_type

SIDE EFFECTS
  None

=============================================================================*/

void timer_bin_list_remove
(
  /* Bin list to removed bin from */
  timer_bin_list_ptr              list, 

  /* Bin to remove from bin list */
  timer_bin_ptr                   bin
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( bin->prev_bin != NULL )
  {
    bin->prev_bin->next_bin = bin->next_bin;
  }
  else
  {
    /* This was the head, so update first ptr */
    list->first = bin->next_bin;
  }

  if( bin->next_bin != NULL )
  {
    bin->next_bin->prev_bin = bin->prev_bin;
  }
  else
  {
    /* This was the tail, so updated last ptr */
    list->last = bin->prev_bin;
  }

  /* Update list's count */
  list->count--;

} /* timer_bin_list_remove */



/*=============================================================================

FUNCTION TIMER_BIN_LIST_POP

DESCRIPTION
  Pops a bin off the head of the specified list and returns it 

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  The popped timer_bin_type

SIDE EFFECTS
  None

=============================================================================*/
timer_bin_ptr timer_bin_list_pop
(
  /* Bin list to remove the head bin from */
  timer_bin_list_ptr              list
) 
{
  /* Bin at head of bin list */
  timer_bin_ptr                   bin = list->first;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( bin != NULL )
  {
    timer_bin_list_remove( list, bin );
  }

  return bin;

} /* timer_bin_list_pop */



/*=============================================================================

FUNCTION TIMER_BIN_LIST_PUSH

DESCRIPTION
  Pushes a bin onto the head of the specified list 

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void timer_bin_list_push
(
  /* Bin list to push bin onto */
  timer_bin_list_ptr              list, 

  /* Bin to push onto bin list */
  timer_bin_ptr                   bin
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  timer_bin_list_insert_between( list, bin, NULL, list->first );

} /* timer_bin_list_push */



/*=============================================================================

FUNCTION TIMER_BIN_CREATE_BETWEEN

DESCRIPTION
  Takes a new bin from free bin pool and adds it between the specified bins
  on the active bin list 

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  The created timer_bin_type

SIDE EFFECTS
  None

=============================================================================*/

timer_bin_ptr timer_bin_create_between
(
  /* Bin to create new bin after */
  timer_bin_ptr                   bin1, 

  /* Bin to create new bin before */
  timer_bin_ptr                   bin2,

  /* Timer which will exist in the bin (used to define bin range) */
  timer_ptr_internal_type         timer
) 
{ 
  /* Newly created bin */
  timer_bin_ptr                   new_bin;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Ensure we have a free bin in the pool */
  ASSERT( timers->bins_free.count > 0 );

  /* Pop it off */
  new_bin = timer_bin_list_pop( &timers->bins_free );

  /* Initialize the bin's info */
  new_bin->first  = NULL;
  new_bin->last   = NULL;
  new_bin->count  = 0;

  /* Determine the range of this new bin */
  new_bin->lower = timer->expiry & ~(TIMER_BIN_RANGE); 
  new_bin->upper = new_bin->lower  |  (TIMER_BIN_RANGE); 

  /* finally, compare against prev and next bins to ensure no overlap */

  if( bin1 != NULL )
  {
    /* is the bottom portion of the bin overlapping? */
    if( bin1->upper >= new_bin->lower )
    {
      /* yes, use the upper bound of the prev bin as our upper bound */
      new_bin->lower = bin1->upper + 1;
    }
  }
  if( bin2 != NULL )
  {
    /* is the top portion of the bin overlapping? */
    if( bin2->lower <= new_bin->upper )
    {
      /* yes, use the lower bound of the next bin as our upper bound */
      new_bin->upper = bin2->lower - 1;
    }
  }
        
  timer_bin_list_insert_between( &timers->bins, new_bin, bin1, bin2 );  

  return new_bin;

} /* timer_bin_create_between */



/*=============================================================================

FUNCTION TIMER_BIN_FIND_CREATE

DESCRIPTION
  Searches the active bin list for a bin corresponding to the specified timer  
  If none exists, create it and link it into the bin list

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  The timer_bin_type whose range the timer fits into  

SIDE EFFECTS
  None

=============================================================================*/

timer_bin_ptr timer_bin_find_create
( 
  /* Timer for which a bin is needed */
  timer_ptr_internal_type        timer
)
{
  /* Pointer to a bin.  Used for searching the list of active timer bins */
  timer_bin_ptr                   bin;

  /* Timer list's zero origin. */
  time_timetick_type                   zero;

  /* Timer's expiry. */
  time_timetick_type                   expiry;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  /* NOTE: This function has many early returns. */


  ASSERT( timer->list != NULL );

  bin    = timers->bins.first;
  expiry = timer->expiry; 
  zero   = timer->list->zero; 

  while( bin != NULL )
  {
    /* Is this bin able to hold this timer? */

    if ( expiry >= bin->lower  &&  expiry <= bin->upper )
    {
      /* If this bin straddles the zero, we might need to split this bin into
         one before the zero and one after the zero */

      if( zero >= bin->lower  &&  zero <= bin->upper )
      {
        /* Check if the timer's expiry is less than the zero */

        if( timer->expiry < zero )
        {
          /* split this bin into a pre-zero bin and a post-zero bin, returns
             the first of the split bins */

          bin->lower = zero;

          return timer_bin_create_between( bin->prev_bin, bin, timer );
        }
      }
      
      /* Timer fits in this bin, and it doesn't straddle zero, return it */
      return bin;
    }


    /* Should we create a new bin before this bin? */

    if( expiry < bin->lower )
    {
      /* If it fits between this bin and the prev, or if the prev is NULL
         then create one */

      if( bin->prev_bin == NULL  ||  expiry > bin->prev_bin->upper )
      {
        return timer_bin_create_between( bin->prev_bin, bin, timer );
      }
    }

    /* Doesnt fit in this bin, or before this bin, so iterate to next */
    bin = bin->next_bin;

  } /* while( bin != NULL ) */


  /* If we've gotten to the end, create a bin after the last bin */
  return timer_bin_create_between( timers->bins.last, NULL, timer );

} /* timer_bin_find_create */



/*=============================================================================

FUNCTION TIMER_BIN_FIND_FIRST_EXCEEDING

DESCRIPTION
  Returns the first timer on the bin list exceeding the specified 
  timer's range. If none are found, NULL is returned 

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  The timer meeting the description's criteria.  

SIDE EFFECTS
  None

=============================================================================*/

timer_ptr_internal_type timer_bin_find_first_exceeding
(
  /* Timer used to determine bin range */
  timer_ptr_internal_type         timer
)
{
  /* First timer in bins */
  timer_ptr_internal_type         first;

  /* Timer bin under consideration */
  timer_bin_ptr                   bin;

  /* Origin for timer expiry comparisions */
  time_timetick_type                   zero   = timer->list->zero;

  /* Expiry of timer under consideration */
  time_timetick_type                   expiry = timer->expiry;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  /* NOTE: This function has many early returns. */


  /* Search the bins until we find one with a timer exceeding this timer's
     expiry */

  /* Skip passed all the unbinned expired timers at the head, if any */
  first   = timers->active.list.first;
  while( first != NULL )
  {
    if( first->bin != NULL )
    {
      break;
    }
    first = first->next;
  }

  /* Start with the bin associated with the first on the active list */
  if( first == NULL )
  {
    return NULL;
  }

  for( bin = first->bin;  bin != NULL;  bin = bin->next_bin )
  {
    /* Does this timer fit before the bin's upper bound? */

    if( expiry - zero <= bin->upper - zero )
    {
      /* return the first pointer */ 
      return bin->first;
    }
  }

  /* now continue at the beginning of the bin list and
     progress until we hit the bin of the first timer on the active list */
  for( bin = timers->bins.first; bin != first->bin; bin = bin->next_bin )
  {
    /* Does this timer fit before the bin's upper bound? */

    if( expiry - zero <= bin->upper - zero )
    {
      /* return the first pointer */ 
      return bin->first;
    }
  }
 
  /* If we've gotten here, return NULL */
  return NULL;

} /* timer_bin_find_first_exceeding */



/*=============================================================================

FUNCTION TIMER_BIN_INSERT

DESCRIPTION
  Inserts the specified timer into the bin.
  This (potentially) involves updating the bin's first/last pointer.

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void timer_bin_insert
(
  /* Bin to insert timer in */
  timer_bin_ptr                   bin,

  /* Timer to insert in bin */
  timer_ptr_internal_type         timer 
)
{
  /* Lowest expiry time in first bin */
  time_timetick_type                   bin_lowest;

  /* Highest expiry time in last bin */
  time_timetick_type                   bin_highest;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Since it is known that a bin does not straddle a counter loop,
     these compares can be done without a zero reference */


  /* Calculate the lowest timer expiry in the bin */

  if( bin->first != NULL )
  {
    bin_lowest  = bin->first->expiry;

    /* do we need to update the first pointer? */
    if( timer->expiry < bin_lowest )
    {
      /* We have a new first pointer */

      /* Note: if the expiries are equal, the first pointer remains the same
         to match the behavior of the timer list */ 
      bin->first = timer;
    } 

  }
  else
  {
    /* If no first pointer, update first pointer with new timer */
    bin->first = timer;
  } 

  /* Calculate the highest timer expiry in the bin */
  if( bin->last != NULL )
  {
    bin_highest = bin->last->expiry;

    /* Do we need to update the last pointer? */
    if( timer->expiry >= bin_highest )
    {
      /* We have a new last pointer */

      /* Note: if the expiries are equal, the last pointer is updated 
         to match the behavior of the timer list */ 
      bin->last = timer;
    }

  }
  else
  {
    /* If no last pointer, update last pointer with new timer */
    bin->last = timer;
  }

  /* Record the bin in the timer struct */
  timer->bin = bin; 

  /* Increment the bin's count */
  bin->count++;

} /* timer_bin_insert */



/*=============================================================================

FUNCTION TIMER_BIN_REMOVE

DESCRIPTION
  Remove the specified timer from the bin.
  This (potentially) involves updating the bin's first/last pointer.

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void timer_bin_remove
(
  /* Timer to remove from bin list */
  timer_ptr_internal_type         timer
)
{
  /* Bin timer belongs to */
  timer_bin_ptr                   bin = timer->bin;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT( bin != NULL );


  /* Do we now have an empty bin? */

  if( bin->count == 1 )
  {
    /* empty bin */
    bin->first = NULL;
    bin->last  = NULL;
  }
  else if( bin->count > 1 )
  {
    /* Check if we have a new bin first */
    if( timer == bin->first )
    { 
      bin->first = timer->next;
    }

    /* Check if we have a new bin last */ 
    if( timer == bin->last )
    {
      bin->last = timer->prev;
    }
  }

  /* Unbin the timer */ 
  timer->bin = NULL;

  /* Decrement the bin's count */
  bin->count--;  

  if( bin->count == 0 )
  {
    /* If this bin is now empty, then remove it from the active bin list
       and return it to the free list */
    timer_bin_list_remove( &timers->bins, bin );
    timer_bin_list_push( &timers->bins_free, bin);
  }

} /* timer_bin_remove */
#endif
