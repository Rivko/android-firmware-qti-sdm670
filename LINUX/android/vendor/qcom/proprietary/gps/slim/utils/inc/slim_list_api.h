/*
Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
*/
#ifndef SLIM_LIST_API_H_INCLUDED
#define SLIM_LIST_API_H_INCLUDED
/**
@file
@brief Linked list API header

Linked list API is implemented using standard C macros. The original idea
is taken from QMI stack.

@ingroup slim_List
*/

//! @addtogroup slim_List
//! @{

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
/**
@brief Defines list member variable.
@param[in] type List entry type.
@param[in] name List variable name.
 */
#define SLIM_LIST(type, name) \
  struct { \
    type *head; \
    type *tail; \
    unsigned int count; \
  } name

/**
@brief Defines list entry member variable.
@param[in] type List entry type.
@param[in] link List entry variable name.
 */
#define SLIM_LINK(type, link) \
  struct { \
    type *prev; \
    type *next; \
  } link

/**
@brief Initializes list variable.
@param[in] list List variable.
@sa SLIM_LIST_INITIALIZER
 */
#define SLIM_LIST_INIT(list) do {  \
  (list).head = (list).tail = NULL; \
  (list).count = 0; \
} while(0)

/**
@brief Static list variable initializer macro.
 */
#define SLIM_LIST_INITIALIZER {NULL,NULL,0}

/*!
 * @brief Destroys list object
 *
 * @param[in] list List object
 */
#define SLIM_LIST_DEINIT(list) SLIM_LIST_INIT(list)
/*!
 * @brief Initializes link object
 *
 * @param[out] link Link object
 */
#define SLIM_LINK_INIT(link) (link).next = (link).prev = NULL
/*!
 * @brief Provides list head object
 *
 * @param[in] list List object
 *
 * @return Head object of the list.
 * @retval NULL if list is empty.
 */
#define SLIM_LIST_HEAD(list) ((list).head)
/*!
 * @brief Provides list tail object
 *
 * @param[in] list List object
 *
 * @return Tail object of the list.
 * @retval NULL if list is empty.
 */
#define SLIM_LIST_TAIL(list) ((list).tail)
/*!
 * @brief Provides next object
 *
 * @param[in] link Link field of the iterator object
 *
 * @return Next object from the list.
 * @retval NULL if end is reached.
 */
#define SLIM_LINK_NEXT(link) ((link).next)
/*!
 * @brief Provides list size
 *
 * @param[in] list List object
 *
 * @return Number of entries in the list.
 */
#define SLIM_LIST_CNT(list) ((list).count)
/*!
 * @brief Removes entry from the list.
 *
 * @param[in] list List object.
 * @param[in] node Node to remove.
 * @param[in] link Link literal.
 */
#define SLIM_LIST_REMOVE(list, node, link) \
  do { \
    if((node)->link.prev) \
      (node)->link.prev->link.next = (node)->link.next; \
    else /* node at the front of the list */ \
      list.head = (node)->link.next; \
    if((node)->link.next) \
      (node)->link.next->link.prev = (node)->link.prev; \
    else /* node at the end of the list */ \
      list.tail = (node)->link.prev; \
    list.count--; \
  } while(0)
/*!
 * @brief Add entry to the list.
 *
 * Adds entry to the tail of the list.
 *
 * @param[in] list List object.
 * @param[in] node Node to add.
 * @param[in] link Link literal.
 */
#define SLIM_LIST_ADD(list, node, link) \
  do { \
    if(!list.tail) \
    { \
      /* first node on the list */ \
      list.tail = list.head = node; \
    } \
    else \
    { \
      (node)->link.prev = list.tail; \
      list.tail->link.next = node; \
      list.tail = node; \
    } \
    list.count++; \
  } while(0)

/*!
 * @brief Finds entry from the list.
 *
 * @param[in] list List object.
 * @param[in] iter Iterator object.
 * @param[in] link Link literal.
 * @param[in] test Search test expression.
 */
#define SLIM_LIST_FIND(list, iter, link, test) do {  \
  iter = (list).head; \
  while(iter) { \
    if(test) {  \
      break;  \
    } \
    iter = (iter)->link.next;  \
  } \
} while(0)

//! @}

#endif /* SLIM_LIST_API_H_INCLUDED */
