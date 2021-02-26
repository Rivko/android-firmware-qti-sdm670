#ifndef SMEM_LIST_H
#define SMEM_LIST_H

/**
 * @file smem_list.h
 *
 * Public data types and functions to implement a generic 
 * list. 
 */ 

/*==============================================================================
     Copyright (c) 2013 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/28/13   pa      Initial revision based on implementation by padelsba
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#include "comdef.h"

/** @addtogroup smem
@{ */


/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/

/** Defines the structure to link to the SMEM list */
typedef struct smem_list_link
{
  struct smem_list_link *next;
} smem_list_link_type;


/** Defines the generic SMEM list type */
typedef struct smem_list
{
  smem_list_link_type *next;
  int32 count;
} smem_list_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

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
void smem_list_init( smem_list_type *list );

/*===========================================================================
FUNCTION      smem_list_append
===========================================================================*/
/**
  Appends an element to a generic list.
 
  @param[in/out] list   Pointer to the user list.
  @param[in] item       Element to be inserted.
   
  @return 
  N/A.

  @dependencies
  List must be protected by critical sections for multi-threaded use.

  @sideeffects
  None.
*/
/*=========================================================================*/
void smem_list_append( smem_list_type *list, void *item );

/*===========================================================================
FUNCTION      smem_list_delete
===========================================================================*/
/**
  Removes the first element in the list.
 
  @param[in] list       Pointer to the user list.
  @param[out] item      Pointer to store the removed element.
   
  @return 
  0 if the element is deleted, or a negative error code if the element
  is not found.
 
  @dependencies
  List must be protected by critical sections for multi-threaded use.

  @sideeffects
  None.
*/
/*=========================================================================*/
int32 smem_list_delete( smem_list_type *list, void *item );

/*===========================================================================
FUNCTION      smem_list_first
===========================================================================*/
/**
  Retrieves the first element in the list without removing.
 
  @param[in] list       Pointer to the user list.
   
  @return 
  Pointer to the first element in the list.

  @dependencies
  List must be protected by critical sections for multi-threaded use.

  @sideeffects
  None.
*/
/*=========================================================================*/
void *smem_list_first( smem_list_type *list );

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
void *smem_list_next( void * item );

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
int32 smem_list_count( smem_list_type *list );

#ifdef __cplusplus
}
#endif

/** @} */ /* end_addtogroup smem */

#endif /* SMEM_LIST_H */