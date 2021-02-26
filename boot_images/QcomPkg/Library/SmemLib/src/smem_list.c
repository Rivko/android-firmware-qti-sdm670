/*===========================================================================

                         S H A R E D   M E M O R Y

DESCRIPTION
    Defines the public api that is used to create a generic list.

Copyright (c) 2013 - 2014 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
Rights Reserved.

===========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/SmemLib/src/smem_list.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/07/15   an      Added smem_list_move_all.
08/17/15   pp      Added the last pointer to make append run in O(1) time
05/07/15   an      Added smem_list_insert.
01/16/14   bt      Correct and clarify function header documentation.
03/28/13   pa      Initial revision based on implementation by padelsba
===========================================================================*/


/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "smem_os.h"
#include "smem_list.h"

/*=============================================================================

            GLOBAL DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/*===========================================================================

                      PRIVATE FUNCTION DECLARATIONS
                      
===========================================================================*/

/*===========================================================================
FUNCTION      smem_list_init
===========================================================================*/
/**
  Creates a generic list.

  @param[in/out] list   Pointer to the user list.

  @return
  N/A.

  @dependencies
  List must be protected by critical sections for multi-threaded use.

  @sideeffects
  None.
*/
/*=========================================================================*/
void smem_list_init( smem_list_type *list )
{
  list->next  = NULL;
  list->last  = NULL;
  list->count = 0;
}

/*===========================================================================
FUNCTION      smem_list_append
===========================================================================*/
/**
  Appends an element to the end of a generic list.
  
  The element must contain a smem_list_link_type at its base address.
  The element should not be already present in any list.

  @param[in/out] list   Pointer to the user list.
  @param[in] item       Element to be inserted.

  @return
  N/A.

  @dependencies
  The element must contain a smem_list_link_type at its base address.
  List must be protected by critical sections for multi-threaded use.

  *** IMPORTANT ***
  append implementation should make sure it is safe to traverse(only traverse) 
  a list without taking a lock on a list on which only append operation is 
  allowed
  
  @sideeffects
  None.
*/
/*=========================================================================*/
void smem_list_append( smem_list_type *list, void *item )
{
  /* First set next of item to NULL which is required for ***IMPORTANT*** 
     note above */
  ((smem_list_link_type*)item)->next = NULL;
  
  /* add new item to the list */
  if (list->last != NULL)
  {
    list->last->next = item;
    list->last = item;
  }
  else
  {
    list->next = item;
    list->last = item;
  }

  list->count++;
}

/*===========================================================================
  FUNCTION      smem_list_insert
===========================================================================*/
/**
  Inserts an element into a generic list when comparator condition is met.
  
  The element must contain a smem_list_link_type at its base address.
  The element should not be already present in any list.
  The item is inserted ahead of the list element for which comparator returns 
  TRUE

  @param[in/out] list   Pointer to the user list.
  @param[in] item       Element to be inserted.
  @param[in] comparator Caller provided comparator.

  @return
  N/A.

  @dependencies
  The element must contain a smem_list_link_type at its base address.
  List must be protected by critical sections for multi-threaded use.

  @sideeffects
  None.
*/
/*=========================================================================*/
void smem_list_insert( smem_list_type *list, void *item, smem_list_comparator_fn comp)
{
  smem_list_link_type *link = ( smem_list_link_type * )list;

  /* get next item until link->next points to null or comparator returns TRUE */
  while( link->next && !comp(item, link->next) )
  {
    link = link->next;
  }

  /* If item is the new last node, update the last pointer */
  if (link->next == NULL)
  {
    list->last = item;
  }
  
  /* link points to the element after which the item should be inserted */
  ((smem_list_link_type *)item)->next = link->next;
  link->next = item;

  list->count++;
}


/*===========================================================================
FUNCTION      smem_list_delete
===========================================================================*/
/**
  Removes the specified element from a generic list.

  The element may be present at any point in the list.

  @param[in] list       Pointer to the user list.
  @param[in] item       Element to be removed.

  @return
  0 if the element is deleted, or a negative error code if the element
  is not found.

  @dependencies
  List must be protected by critical sections for multi-threaded use.

  @sideeffects
  None.
*/
/*=========================================================================*/
int32 smem_list_delete( smem_list_type *list, void *item )
{
  smem_list_link_type *link = ( smem_list_link_type * )list;

  /* get next item until link points to item in question */
  while( link->next != item )
  {
    if( NULL == link->next )
    {
      return -1;
    }

    link = link->next;
  }

  link->next = ( ( smem_list_link_type * )item )->next;

  /* If the list is empty */
  if (list->next == NULL)
  {
    list->last = NULL;
  }
  /* If last item was deleted update the list->last pointer */
  else if (link->next == NULL)
  {
    list->last = link;
  }
  
  list->count--;
  
  return 0;
}

/*===========================================================================
FUNCTION      smem_list_first
===========================================================================*/
/**
  Retrieves the first element in the list without removing it.

  @param[in] list       Pointer to the user list.

  @return
  Pointer to the first element in the list.

  @dependencies
  List must be protected by critical sections for multi-threaded use.

  @sideeffects
  None.
*/
/*=========================================================================*/
void *smem_list_first( smem_list_type *list )
{
  /* return the first item in the list */
  return list->next;
}

/*===========================================================================
FUNCTION      smem_list_next
===========================================================================*/
/**
  Retrieves the next element in the list.

  @param[in] list       Pointer to the user list.

  @return
  Pointer to the next element in the list.

  @dependencies
  List must be protected by critical sections for multi-threaded use.

  @sideeffects
  None.
*/
/*=========================================================================*/
void *smem_list_next( void * item )
{
  /* just return the item that this item points to 
   * (base address contains a pointer) */
  return ( ( smem_list_link_type * )item )->next;
}

/*===========================================================================
FUNCTION      smem_list_count
===========================================================================*/
/**
  Gets the number of elements in the list.

  @param[in] list       Pointer to the user list.

  @return
  Count of elements in the list.

  @dependencies
  List must be protected by critical sections for multi-threaded use.

  @sideeffects
  None.
*/
/*=========================================================================*/
int32 smem_list_count( smem_list_type *list )
{
  return list->count;
}

/*===========================================================================
  FUNCTION      smem_list_move_all
===========================================================================*/
/**
  Takes all items from the one list and appends them to another.

  Both list pointers must be valid and point to the initialized lists.
  The source list will become an empty list.

  @param[in] from   Pointer to the list from which the items will be removed.
  @param[in] to     Pointer to the list taking the items.

  @return
  N/A.

  @dependencies
  Both lists must be protected by critical sections for multi-threaded use.

  @sideeffects
  None.
*/
/*=========================================================================*/
void smem_list_move_all(smem_list_type *from, smem_list_type *to)
{
  if (to->next == NULL)
  {
    to->next = from->next;
  }
  else
  {
    to->last->next = from->next;
  }

  if (from->last != NULL)
  {
    to->last = from->last;
  }

  to->count += from->count;

  smem_list_init(from);
}