#ifndef QUEUE_H
#define QUEUE_H
/**
  @file queue.h
  @brief This file contains types and declarations associated with the Queue
  Services.

*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The Utility_Services_API_mainpage.dox file contains all file/group 
      descriptions that are in the output PDF generated using Doxygen and 
      Latex. To edit or update any of the file/group text in the PDF, edit 
      the Utility_Services_API_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the "utils_queue" 
      group description in the Utility_Services_API_mainpage.dox file. 
===========================================================================*/
/*===========================================================================

            Q U E U E    S E R V I C E S    H E A D E R    F I L E

DESCRIPTION
 This file contains types and declarations associated with the Queue
 Services.

Copyright (c) 1992-2012 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary. 
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: O:/src/asw/COMMON/vcs/queue.h_v   1.3   16 May 2002 15:43:00   rajeevg  $
$Header: //components/rel/core.qdsp6/2.1.c4/api/services/queue.h#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
22/03/12   rks     Use OSAL for OS abstraction and remove OS specific code/featurization
03/16/11   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
11/11/10   EBR     Doxygenated File.
31/03/09   sri     Implemented q_destroy(api)
10/30/07   hal     Fixed errors in q_linear_delete
12/11/06   sgh     Enabled mutex for WinMobile
01/10/06    ck     Added prototype for q_delete_ext function that returns a 
                   boolean
05/16/02    gr     Added macro Q_ALREADY_QUEUED.
09/28/01    gr     Merged in changes from the MSP archive. Includes
                   queue-specific mutual exclusion for Win32 targets and the
                   new function q_linear_delete. Removed support for UTF_ASP.
01/25/01   day     Merged from MSM5105_COMMON.00.00.05.
                   Added support for MSM5000_IRAM_FWD.
                   Added support for UTF_ASP.
01/13/00   jct     Add q_linear_search
12/10/99   gr      Merged from the MSM3000 Rel 4.0c.
                   Re-arranged data in link structures to fix bugs that were
                   occurring when FEATURE_Q_SINGLE_LINK was defined but
                   FEATURE_Q_NO_SELF_QPTR was not.
                   Introduced q_last_get().
                   Added extern "C" to allow this file to export the queue
                   routines to a C++ program.  Changed use of word to int to 
                   count the number of elements in a queue.  Upgraded comments
                   to include function headers.  Removed the inclusion of
                   comdef.h since it is no longer needed.
                   Added macros to support renaming of queue functions and
                   to intercept calls to queue functions. 
04/09/99    sk     Comments changed.
04/06/99    sk     Introduced FEATURE_Q_NO_SELF_QPTR and FEATURE_Q_SINGLE_LINK
12/16/98   jct     Removed 80186 support
09/24/98   jct     Updated copyright
12/20/96   rdh     Changed to Pascal call convention.
02/08/96   jca     Added missing #include.
04/22/92   ip      Initial porting of file from Brassboard to DMSS.
02/20/92   arh     Added new function q_check.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifdef CUST_H
  #include "customer.h"
#endif

#include "comdef.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/** @addtogroup utils_queue
@{ */


/** @brief Used as a link field with variables, which allows the variables 
  to be moved on and off the queues. This is the principal structure of Queue
  Utility Services. Note that Users must not directly access the fields of 
  a link; only Queue Utility Services is to access the fields.

  This structure is also used in the definition of the queues themselves.
*/
typedef struct q_link_struct
{
  struct q_link_struct  *next_ptr;
  /**< Pointer to the next link in the list. If NULL, there 
  is no next link. */
  struct q_link_struct  *prev_ptr;
  /**< Pointer to the previous link in the list. If NULL, there 
  is no previous link. */
} q_link_type;

  /*These are HTORPCMETA comments, Do not delete this.*/
  /*~ FIELD q_link_struct.next_ptr VOID */
  /*~ FIELD q_link_struct.prev_ptr VOID */

/** Used instead of q_link_type when the queue items are linked in 
  a single link list. This avoids the overhead of traversing 
  the entire link list when queueing at the end of the link list. The 
  q_link_struct structure is to be accessed only by Queue Utility Services.
*/
typedef q_link_type q_head_link_type;


/** @brief Used by Queue Utility Services to represent a queue. Note that 
  users must not directly access the fields of a queue; only Queue Utility 
  Services is to access the fields.
*/
typedef struct q_struct
{
  q_head_link_type  link; /**< Used for linking the items into a queue. */

  int          cnt;       /**< Keeps track of the number of items enqueued. 
                               This field is not necessary for normal 
                               operations and is used for debugging 
                               purposes. */

#ifdef FEATURE_QUEUE_USES_MUTEX
  void         *mutex;
#endif
} q_type;

/** @brief Used for generic items, which must have q_link_type as the 
  first element. 

  This structure allows the linear search function to traverse the list 
  without having any information about the elements.
*/
typedef struct {
   q_link_type link;  /**< Used for linking the items into a queue. */
} q_generic_item_type;


/** Used by the searching functions to determine if an item is in
  the queue. It returns nonzero if the element is to be operated on; 
  otherwise, it returns 0.
  
  For linear searching, the operation is to return a pointer to the
  item and terminate the search.
  
  For linear deleting, the operation is to remove the item from the
  queue and continue the traversal.
*/
typedef int (*q_compare_func_type)( void* item_ptr, void* compare_val );


/** Used by the linear delete function to perform an action on an item 
  that is being deleted from the list AFTER the item is deleted. To perform
  an action BEFORE the item is deleted, users can call the action
  function directly in the compare function callback.
*/
typedef void (*q_action_func_type)( void *item_ptr, void* param );

/*===========================================================================

                             Macro Definitions

===========================================================================*/

  /** @cond 
  */
  #define Q_XCEPT_Q_INIT( q_ptr )
  #define Q_XCEPT_Q_LINK( q_ptr, link_ptr )
  #define Q_XCEPT_Q_PUT( q_ptr, link_ptr )
  #define Q_XCEPT_Q_GET( q_ptr )
  #define Q_XCEPT_Q_LAST_GET( q_ptr )
  #define Q_XCEPT_Q_CNT( q_ptr )
  #define Q_XCEPT_Q_CHECK( q_ptr )
  #define Q_XCEPT_Q_LAST_CHECK( q_ptr )
  #define Q_XCEPT_Q_NEXT( q_ptr, link_ptr )
  #define Q_XCEPT_Q_INSERT( q_ptr, q_insert_ptr, q_item_ptr )
  
  #define Q_XCEPT_Q_DELETE( q_ptr, q_delete_ptr )
  /** @endcond */


/**
  Determines if the item passed in is in a queue.

  @param[in] q_link_ptr Pointer to the item to be tested.

  @return
  TRUE -- Item is already in a queue. \n
  FALSE -- Item is not in a queue.

  @dependencies
  None.
*/
#define Q_ALREADY_QUEUED( q_link_ptr ) \
   ((q_link_ptr)->next_ptr != NULL)

/*===========================================================================

                            Function Declarations

===========================================================================*/
#ifdef __cplusplus
   extern "C" {
#endif


/**
  Initializes a specified queue. This function must be called for each
  queue before using the queue with other Queue Utility Services.

  @param[in] q_ptr Pointer to the queue to initialize.

  @return
  A pointer to the initialized queue.

  @dependencies
  None.
*/
q_type* q_init ( q_type *q_ptr );


/**
  Initializes a specified link. This function must be called for each
  link before using the link with other Queue Utility Services.
 
  @param[in] item_ptr Item to be initialized.
  @param[in] link_ptr Link to be initialized.

  @return
  A pointer to the initialized link.

  @dependencies
  None.
*/
q_link_type* q_link ( void *item_ptr, q_link_type *link_ptr );


/**
  Enqueues an item to a specified queue using a specific link.
 
  @param[in] q_ptr    Pointer to the item to be enqueued.
  @param[in] link_ptr Pointer to the link where the item is to be placed.

  @return
  None.

  @dependencies
  The specified queue must have been initialized previously via a call
  to q_init(). \n
  The specified link field of the item must have been initialized previously 
  via a call to q_link().

  @sideeffects
  The specified item is placed at the tail of a specific queue.
*/
void  q_put  ( q_type *q_ptr, q_link_type *link_ptr );


/**
  Enqueues all items from a specified (source) queue to a 
  specific (target) queue.
 
  @param[in] target_q_ptr Pointer to the target queue.
  @param[in] source_q_ptr Pointer to the source queue.

  @return
  None.

  @dependencies
  The specified queues must have been initialized previously via a call
  to q_init().

  @sideeffects
  The items in the specified source queue are placed at the tail of a  
  specific target queue.
*/
void q_m_put(
   q_type       *target_q_ptr,
   q_type       *source_q_ptr
);


/**
  Removes an item from the head of a specified queue.

  @param[in] q_ptr Pointer to the queue.

  @return
  Returns a pointer to the dequeued item, or NULL if the specified queue
  is empty.

  @dependencies
  The specified queue must have been initialized previously via a call
  to q_init().
*/
void* q_get ( q_type *q_ptr );


/**
  Removes an item from the tail of a specified queue.

  @param[in] q_ptr Pointer to the queue.

  @return
  Returns a pointer to the dequeued item, or NULL if the specified queue
  is empty.
 
  @dependencies
  The specified queue must have been initialized previously via a call
  to q_init().
*/
void* q_last_get ( q_type *q_ptr );


/**
  Returns the number of items currently queued on a specified queue.
 
  @param[in] q_ptr Pointer to the queue.

  @return
  The number of items.

  @dependencies
  The specified queue must have been initialized previously via a call
  to q_init().
*/
int q_cnt  ( q_type *q_ptr );


/**
  Returns a pointer to the data block at the head of the queue. The data 
  block is not removed from the queue.

  @param[in] q_ptr Pointer to the queue.

  @return
  Returns a pointer to the queue item, or NULL if the specified queue
  is empty.
 
  @dependencies
  The specified queue must have been initialized previously via a call
  to q_init().
*/
void* q_check (q_type  *q_ptr);


/**
  Returns a pointer to the data block at the tail of the queue. The data 
  block is not removed from the queue.
 
  @param[in] q_ptr Pointer to the queue.

  @return
  Returns a pointer to the queue item, or NULL if the specified queue
  is empty.
 
  @dependencies
  The specified queue must have been initialized previously via a call
  to q_init().
*/
void* q_last_check ( q_type *q_ptr );


/**
  Returns a pointer to the next item on the queue.

  @param[in] q_ptr    Pointer to the queue.
  @param[in] link_ptr Pointer to the link on the queue.

  @return
  Returns a pointer to the next item on the queue, or NULL if the end of 
  the queue is reached.
 
  @dependencies
  The specified queue must have been initialized previously via a call
  to q_init().
*/
void* q_next  ( q_type *q_ptr, q_link_type *link_ptr );


/**
  Inserts an item before a specified item on the queue.

  @param[in] q_ptr        Pointer to the queue.
  @param[in] q_insert_ptr Pointer to where the item is to be inserted.
  @param[in] q_item_ptr   Pointer to the item to insert.

  @return
  None.

  @dependencies
  The specified queue must have been initialized previously via a call
  to q_init().
*/
void q_insert  ( 
#ifdef FEATURE_Q_NO_SELF_QPTR
		 q_type *q_ptr, 
#endif
		 q_link_type *q_insert_ptr, 
		 q_link_type *q_item_ptr );


/**
  Deletes an item from a specified queue.

  @param[in] q_ptr        Pointer to the queue.
  @param[in] q_delete_ptr Pointer to the item to be deleted.

  @return
  None.

  @dependencies
  The specified queue must have been initialized previously via a call
  to q_init().
*/
void q_delete  ( q_type *q_ptr, q_link_type *q_delete_ptr );


/**
  Removes an item from a specified queue.

  @param[in] q_ptr        Pointer to the queue.
  @param[in] q_delete_ptr Pointer to the item to be removed.

  @return
  TRUE -- Item was found and removed from the queue. \n
  FALSE -- Item was not found in the queue.

  @dependencies
  The specified queue must have been initialized previously via a call
  to q_init().
*/
boolean q_delete_ext  ( q_type *q_ptr, q_link_type *q_delete_ptr );


/**
  Given a comparison function, this function traverses the elements in
  a queue, calls the compare function, and returns a pointer to the
  current element being compared if the user-passed compare function
  returns nonzero.

  The user compare function is to return 0 if the current element 
  is not pertinent to the compare function.
 
  @param[in] q_ptr        Pointer to the queue to be traversed.
  @param[in] compare_func Comparison function to be used.
  @param[in] compare_val  Pointer to the value against which to compare.

  @return
  A pointer to the element.

  @dependencies
  The specified queue must have been initialized previously via a call
  to q_init(). \n
  The user's queue elements must have q_link_type as the first element
  of the queued structure.
*/
void* q_linear_search(
  q_type             *q_ptr,  
  q_compare_func_type compare_func,
  void               *compare_val
);


/**
  Given a comparison function, this function traverses the elements in
  a queue, calls the compare function, and returns a pointer to the
  current element being compared if the user-passed compare function
  returns nonzero. In addition, the item is removed from the queue.

  The user compare function is to return 0 if the current element 
  is not pertinent to the compare function.

  @param[in] q_ptr        Pointer to the queue to be traversed.
  @param[in] compare_func Comparison function to be used.
  @param[in] compare_val  Pointer to the value against which to compare.
  @param[in] action_func  Action function to call if the item is found;
                          optional.
  @param[in] param        Parameter to pass to the action function if the 
                          item is found; not required if there is no action
                          function.

  @return
  Returns a pointer to the element, or NULL if the element was not found.

  @dependencies
  The specified queue must have been initialized previously via a call
  to q_init().\n
  The user's queue elements must have q_link_type as the first element
  of the queued structure.
*/
void *q_linear_delete_new(
  q_type             *q_ptr,  
  q_compare_func_type compare_func,
  void               *compare_val,
  q_action_func_type  action_func,
  void               *param
);


/**
  Given a comparison function, this function traverses the elements in
  a queue, calls the compare function, and returns a pointer to the
  current element being compared if the user-passed compare function
  returns nonzero. In addition, the item will be removed from the queue.

  The user compare function is to return 0 if the current element 
  is not pertinent to the compare function.

  @note1hang The user's compare function will be passed NULL for the compare 
  value.

  @param[in] q_ptr        Pointer to the queue to be traversed.
  @param[in] compare_func Comparison function to be used.
  @param[in] action_func  Action function to call if the item is found;
                          optional.
  @param[in] param        Parameter to pass to the action function if the 
                          item is found; not required if there is no action
                          function.

  @return
  Returns a pointer to the element, or NULL if the element was not found.

  @dependencies
  The specified queue must have been initialized previously via a call
  to q_init(). \n
  The user's queue elements must have q_link_type as the first element
  of the queued structure.
*/
void q_linear_delete(
  q_type             *q_ptr,  
  q_compare_func_type compare_func,
  void               *param,
  q_action_func_type  action_func
);


/**
  Destroys a specified queue. This function is to be called if the queue 
  is no longer required.
  
  @param[in] q_ptr Pointer to the queue to be destroyed.

  @return
  None.

  @dependencies
  None.

  @sideeffects
  Elements in the queue will not be accessible. To avoid leaks, it is user's 
  responsibility to deallocate the memory allocated for the queue and its 
  elements.
*/
void q_destroy(
   q_type  *q_ptr
);


#ifdef __cplusplus
   }
#endif

/** @} */ /* end_addtogroup utils_queue */
#endif /* QUEUE_H */
