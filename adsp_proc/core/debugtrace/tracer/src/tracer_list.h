#ifndef TRACER_LIST_H
#define TRACER_LIST_H
/*===========================================================================
  @file tracer_list.h

Double-linked list utility.

               Copyright (c) 2011 Qualcomm Technologies Incorporated.
               All rights reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/* $Header: //components/rel/core.qdsp6/2.1/debugtrace/tracer/src/tracer_list.h#1 $ */

/*---------------------------------------------------------------------------
 * Type Declarations
 * ------------------------------------------------------------------------*/
/**
  @brief  Structure for a node of a doubly-linked list.
 */
typedef struct tracer_node_s {
   void *pData;            /**< Pointer to data block for client's use.*/
   size_t dataSize;        /**< Size of the user's data. */
   struct tracer_node_s * volatile pPrev;  /**< Link to previous node. */
   struct tracer_node_s * volatile pNext;  /**< Link to next node. */
} tracer_node_t;

typedef tracer_node_t * volatile tracer_node_handle_t;

/**
  @brief  Structure for a linked list.
 */
typedef struct tracer_list_s {
   tracer_node_t * volatile pHead;  /**< Point to first node in list. */
   int volatile nodeCount;          /**< Number of nodes in the list. */
                                    /**< For reporting list status. */
} tracer_list_t;

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/
/**
  @brief First time initialization of a list structure to the empty state.

  Initializes a newly allocated list structure to an empty state prior
  to use. If list structure is already in use, call tracer_list_clear()
  instead.

  @param pList [in]: Pointer to the list.
  @return None.
 */
void tracer_list_init(tracer_list_t *pList);

/*-------------------------------------------------------------------------*/
/**
  @brief The specified list is returned to the empty state.

   Removes and deletes all nodes from the specified list.
   List placed in the empty state.

  @param pList [in]: Pointer to the list.
  @return None.
 */
//void tracer_list_clear(tracer_list_t *pList);

/*-------------------------------------------------------------------------*/
/**
  @brief Allocates memory for and adds a new node to the
         front of the specified list.

  Allocates memory for a new node and the node's data.
  All allocated memory is cleared. Then, the newly allocated
  node is added to the front of the specified linked list.

  @param pList [in]: Pointer to the list to add a node to.
  @param data_size [in]: Bytes to allocate for the node data.

  @return Successful, a handle to the added node.
  @return Failure, NULL
 */
tracer_node_t *tracer_list_new_node(tracer_list_t *pList, size_t data_size);

/*-------------------------------------------------------------------------*/
/**
  @brief Removes the node from the list and deletes the node.

  Removes the specified node from the specified list.
  Memory for the node's data and node is cleared then freed.

  @param pList [in]: Pointer to the list in which the node
         belongs.
  @param pNode [in,out]: Pointer to the node to be removed.
            Set to Null on success or unchanged on failure.
            WARNING: Due to compiler optimization, may not be set to
            NULL. Caller must check return value and on success
            set its own node to null.

  @return 0: success (pNode is returned as NULL)
  @return 1: failure (pNode not in pList)
 */
int tracer_list_delete_node(tracer_list_t *pList, tracer_node_handle_t pNode);

#endif /* #ifndef TRACER_LIST_H */

