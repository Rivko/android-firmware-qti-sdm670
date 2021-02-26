#ifndef SLIMBUSLISTS_H
#define SLIMBUSLISTS_H
/*
===========================================================================

FILE:         SlimBusLists.h

DESCRIPTION:  This file defines linked list macros used by 
              the SLIMbus core driver.

===========================================================================

                             Edit History

$Header: //components/rel/core.qdsp6/2.1/buses/slimbus/src/common/SlimBusLists.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
11/15/16   MJS     Add const qualifiers.
02/05/16   MJS     Add support for multiple endpoints per port.
08/06/15   MJS     Additional NULL pointer checks.
03/04/13   MJS     Add PrevNode function.
07/25/12   MJS     Check for NULL during PushHead and PushTail.
12/11/11   MJS     Remove references to DAL.
10/25/11   MJS     Add support for basic client linked lists.
05/10/11   MJS     Add support for mater port linked lists.
09/28/10   MJS     Initial revision.

===========================================================================
             Copyright (c) 2010-2016 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

#include "SlimBus.h"


/**
 * @brief Initialize the root node for a Slimbus linked list
 *
 * This function initializes the root node for a Slimbus linked 
 * list data structure. 
 * 
 * @param[in,out] pRoot  Pointer to the linked list root node
 * 
 * @return None
 */
SLIMBUS_INLINE void RootNodeInit( SlimBusLinkNodeType *pRoot )
{
  if (NULL != pRoot)
  {
    pRoot->pNext = pRoot;
    pRoot->pPrev = pRoot;
  }
}

/**
 * @brief Remove the list node from the linked list
 *
 * This function removes the list node from the linked list. 
 * 
 * @param[in] pNode  The list node to remove
 * 
 * @return None
 */
SLIMBUS_INLINE void RemoveNode( SlimBusLinkNodeType *pNode )
{
  if ( NULL != pNode->pNext )
  {
    pNode->pNext->pPrev = pNode->pPrev;
  }
  if ( NULL != pNode->pPrev )
  {
    pNode->pPrev->pNext = pNode->pNext;
  }
  pNode->pNext = NULL;
  pNode->pPrev = NULL;
}
/**
 * @brief Remove the event list node from the linked list
 *
 * This function removes the event list node from the linked 
 * list. 
 * 
 * @param[in] pNode  The event list node to remove
 * 
 * @return None
 */
SLIMBUS_INLINE void RemoveEventNode( SlimBusEventNodeType *pEvent )
{
  if (NULL == pEvent )
  {
    return;
  }

  RemoveNode(&pEvent->Node);
}
/**
 * @brief Remove the channel list node from the linked list
 *
 * This function removes the channel list node from the linked 
 * list. 
 * 
 * @param[in] pNode  The channel list node to remove
 * 
 * @return None
 */
SLIMBUS_INLINE void RemoveChannelNode( SlimBusChannelType *pChannel )
{
  if (NULL == pChannel )
  {
    return;
  }

  RemoveNode(&pChannel->GroupNode);
}
/**
 * @brief Remove the channel parameter list node from the linked
 *        list
 *
 * This function removes the channel parameter list node from 
 * the linked list. 
 * 
 * @param[in] pNode  The channel parameter list node to remove
 * 
 * @return None
 */
SLIMBUS_INLINE void RemoveParamNode( SlimBusChannelParamType *pParam )
{
  if (NULL == pParam )
  {
    return;
  }

  RemoveNode(&pParam->LinkNode);
}
/**
 * @brief Remove the endpoint list node from the linked list 
 *
 * This function removes the endpoint list node from the linked 
 * list. 
 * 
 * @param[in] pNode  The endpoint list node to remove
 * 
 * @return None
 */
SLIMBUS_INLINE void RemoveEndPointNode( SlimBusEndPointType *pEndPoint )
{
  if (NULL == pEndPoint )
  {
    return;
  }

  RemoveNode(&pEndPoint->LinkNode);
}
/**
 * @brief Remove the basic client list node from the linked list
 *
 * This function removes the basic client list node from the 
 * linked list. 
 * 
 * @param[in] pNode  The basic client list node to remove
 * 
 * @return None
 */
SLIMBUS_INLINE void RemoveClientNode( SlimBusBasicClientCtxt *pCtxt )
{
  if (NULL == pCtxt )
  {
    return;
  }

  RemoveNode(&pCtxt->LinkNode);
}

/**
 * @brief Push the list node to the head of the linked list
 *
 * This function pushes the list node to the head of the linked 
 * list 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * @param[in] pNode  Pointer to the list node to push
 * 
 * @return None
 */
SLIMBUS_INLINE void PushHead( SlimBusLinkNodeType *pRoot, SlimBusLinkNodeType *pNode )
{
  RemoveNode(pNode);
  pNode->pNext = pRoot->pNext;
  if (NULL != pRoot->pNext)
  {
    pRoot->pNext->pPrev = pNode;
  }
  pRoot->pNext = pNode;
  pNode->pPrev = pRoot;
}
/**
 * @brief Push the event list node to the head of the linked 
 *        list
 *
 * This function pushes the event list node to the head of the 
 * linked list 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * @param[in] pEvent  Pointer to the event list node to push
 * 
 * @return None
 */
SLIMBUS_INLINE void PushHeadEvent( SlimBusLinkNodeType *pRoot, SlimBusEventNodeType *pEvent )
{
  if (NULL == pRoot || NULL == pEvent )
  {
    return;
  }
  
  PushHead(pRoot, &pEvent->Node);
}
/**
 * @brief Push the channel list node to the head of the linked 
 *        list
 *
 * This function pushes the channel list node to the head of the
 * linked list 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * @param[in] pChannel  Pointer to the channel list node to push
 * 
 * @return None
 */
SLIMBUS_INLINE void PushHeadChannel( SlimBusLinkNodeType *pRoot, SlimBusChannelType *pChannel )
{
  if (NULL == pRoot || NULL == pChannel )
  {
    return;
  }
  
  PushHead(pRoot, &pChannel->GroupNode);
}
/**
 * @brief Push the channel parameter list node to the head of 
 *        the linked list
 *
 * This function pushes the channel parameter list node to the 
 * head of the linked list 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * @param[in] pParam  Pointer to the channel parameter list node
 *                to push
 * 
 * @return None
 */
SLIMBUS_INLINE void PushHeadParam( SlimBusLinkNodeType *pRoot, SlimBusChannelParamType *pParam )
{
  if (NULL == pRoot || NULL == pParam )
  {
    return;
  }
  
  PushHead(pRoot, &pParam->LinkNode);
}
/**
 * @brief Push the endpoint list node to the head of the linked 
 *        list
 *
 * This function pushes the endpoint list node to the head of 
 * the linked list 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * @param[in] pEndPoint  Pointer to the endpoint list node to 
 *       push
 * 
 * @return None
 */
SLIMBUS_INLINE void PushHeadEndPoint( SlimBusLinkNodeType *pRoot, SlimBusEndPointType *pEndPoint )
{
  if (NULL == pRoot || NULL == pEndPoint )
  {
    return;
  }
  
  PushHead(pRoot, &pEndPoint->LinkNode);
}
/**
 * @brief Push the basic client list node to the head of the 
 *        linked list
 *
 * This function pushes the basic client list node to the head 
 * of the linked list 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * @param[in] pCtxt  Pointer to the basic client list node to 
 *       push
 * 
 * @return None
 */
SLIMBUS_INLINE void PushHeadClient( SlimBusLinkNodeType *pRoot, SlimBusBasicClientCtxt *pCtxt )
{
  if (NULL == pRoot || NULL == pCtxt )
  {
    return;
  }
  
  PushHead(pRoot, &pCtxt->LinkNode);
}

/**
 * @brief Push the list node to the tail of the linked list
 *
 * This function pushes the list node to the tail of the linked 
 * list 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * @param[in] pNode  Pointer to the list node to push
 * 
 * @return None
 */
SLIMBUS_INLINE void PushTail( SlimBusLinkNodeType *pRoot, SlimBusLinkNodeType *pNode )
{
  RemoveNode(pNode);
  pNode->pPrev = pRoot->pPrev;
  if (NULL != pRoot->pPrev)
  {
    pRoot->pPrev->pNext = pNode;
  }
  pRoot->pPrev = pNode;
  pNode->pNext = pRoot;
}
/**
 * @brief Push the event list node to the tail of the linked 
 *        list
 *
 * This function pushes the event list node to the tail of the 
 * linked list 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * @param[in] pEvent  Pointer to the event list node to push
 * 
 * @return None
 */
SLIMBUS_INLINE void PushTailEvent( SlimBusLinkNodeType *pRoot, SlimBusEventNodeType *pEvent )
{
  if (NULL == pRoot || NULL == pEvent )
  {
    return;
  }

  PushTail(pRoot, &pEvent->Node);
}
/**
 * @brief Push the channel list node to the tail of the linked 
 *        list
 *
 * This function pushes the channel list node to the tail of the 
 * linked list. 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * @param[in] pChannel  Pointer to the channel list node to push
 * 
 * @return None
 */
SLIMBUS_INLINE void PushTailChannel( SlimBusLinkNodeType *pRoot, SlimBusChannelType *pChannel )
{
  if (NULL == pRoot || NULL == pChannel )
  {
    return;
  }
  
  PushTail(pRoot, &pChannel->GroupNode);
}
/**
 * @brief Push the channel parameter list node to the tail of 
 *        the linked list
 *
 * This function pushes the channel parameter list node to the 
 * tail of the linked list 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * @param[in] pParam  Pointer to the channel parameter list node 
 *       to push
 * 
 * @return None
 */
SLIMBUS_INLINE void PushTailParam( SlimBusLinkNodeType *pRoot, SlimBusChannelParamType *pParam )
{
  if (NULL == pRoot || NULL == pParam )
  {
    return;
  }
  
  PushTail(pRoot, &pParam->LinkNode);
}
/**
 * @brief Push the endpoint list node to the tail of the linked 
 *        list
 *
 * This function pushes the endpoint list node to the tail of 
 * the linked list 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * @param[in] pEndPoint  Pointer to the endpoint list node to 
 *       push
 * 
 * @return None
 */
SLIMBUS_INLINE void PushTailEndPoint( SlimBusLinkNodeType *pRoot, SlimBusEndPointType *pEndPoint )
{
  if (NULL == pRoot || NULL == pEndPoint )
  {
    return;
  }
  
  PushTail(pRoot, &pEndPoint->LinkNode);
}
/**
 * @brief Push the basic client list node to the tail of the 
 *        linked list
 *
 * This function pushes the basic client list node to the tail 
 * of the linked list 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * @param[in] pCtxt  Pointer to the basic client list node to 
 *       push
 * 
 * @return None
 */
SLIMBUS_INLINE void PushTailClient( SlimBusLinkNodeType *pRoot, SlimBusBasicClientCtxt *pCtxt )
{
  if (NULL == pRoot || NULL == pCtxt )
  {
    return;
  }
  
  PushTail(pRoot, &pCtxt->LinkNode);
}


/**
 * @brief Pop a list node from the head of the linked list
 *
 * This function pops a list node from the head of the linked 
 * list 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * 
 * @return Pointer to the list node popped
 */
SLIMBUS_INLINE SlimBusLinkNodeType* PopHead( SlimBusLinkNodeType *pRoot )
{
  SlimBusLinkNodeType *pNode = NULL;
  
  if (NULL == pRoot || pRoot->pNext == NULL)
  {
    return NULL;
  }
  if (pRoot->pNext == pRoot)
  {
    return NULL;
  }
  
  pNode = pRoot->pNext;
  if (NULL != pNode)
  {
    pRoot->pNext = pNode->pNext;
    if (NULL != pRoot->pNext  )
    {
      pRoot->pNext->pPrev = pRoot;
    }
    pNode->pNext = NULL;
    pNode->pPrev = NULL;
  }
  
  return pNode;
}
/**
 * @brief Pop an event list node from the head of the linked 
 *        list
 *
 * This function pops an event list node from the head of the 
 * linked list 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * 
 * @return Pointer to the event list node popped
 */
SLIMBUS_INLINE SlimBusEventNodeType* PopHeadEvent( SlimBusLinkNodeType *pRoot )
{
  return (SlimBusEventNodeType*) PopHead(pRoot);
}
/**
 * @brief Pop a channel list node from the head of the linked 
 *        list
 *
 * This function pops a channel list node from the head of the 
 * linked list 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * 
 * @return Pointer to the channel list node popped
 */
SLIMBUS_INLINE SlimBusChannelType* PopHeadChannel( SlimBusLinkNodeType *pRoot )
{
  return (SlimBusChannelType*) PopHead(pRoot);
}
/**
 * @brief Pop a channel parameter list node from the head of the
 *        linked list
 *
 * This function pops a channel parameter list node from the 
 * head of the linked list 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * 
 * @return Pointer to the channel parameter list node popped
 */
SLIMBUS_INLINE SlimBusChannelParamType* PopHeadParam( SlimBusLinkNodeType *pRoot )
{
  return (SlimBusChannelParamType*) PopHead(pRoot);
}
/**
 * @brief Pop an endpoint list node from the head of the linked 
 *        list
 *
 * This function pops an endpoint list node from the head of the
 * linked list 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * 
 * @return Pointer to the endpoint list node popped
 */
SLIMBUS_INLINE SlimBusEndPointType* PopHeadEndPoint( SlimBusLinkNodeType *pRoot )
{
  return (SlimBusEndPointType*) PopHead(pRoot);
}
/**
 * @brief Pop a basic client list node from the head of the 
 *        linked list
 *
 * This function pops a basic client list node from the head of 
 * the linked list 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * 
 * @return Pointer to the basic client list node popped
 */
SLIMBUS_INLINE SlimBusBasicClientCtxt* PopHeadClient( SlimBusLinkNodeType *pRoot )
{
  return (SlimBusBasicClientCtxt*) PopHead(pRoot);
}

/**
 * @brief Peek at the list node from the head of the linked list
 *
 * This function peeks at the list node from the head of the 
 * linked list 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * 
 * @return Pointer to the list node peeked
 */
SLIMBUS_INLINE SlimBusLinkNodeType* PeekHead( const SlimBusLinkNodeType *pRoot )
{
  if (NULL == pRoot )
  {
    return NULL;
  }
  if (pRoot->pNext == pRoot)
  {
    return NULL;
  }
  
  return pRoot->pNext;
}
/**
 * @brief Peek at the event list node from the head of the 
 *        linked list
 *
 * This function peeks at the event list node from the head of 
 * the linked list 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * 
 * @return Pointer to the event list node peeked
 */
SLIMBUS_INLINE SlimBusEventNodeType* PeekHeadEvent( const SlimBusLinkNodeType *pRoot )
{
  return (SlimBusEventNodeType*) PeekHead(pRoot);
}
/**
 * @brief Peek at the channel list node from the head of the 
 *        linked list
 *
 * This function peeks at the channel list node from the head of
 * the linked list 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * 
 * @return Pointer to the channel list node peeked
 */
SLIMBUS_INLINE SlimBusChannelType* PeekHeadChannel( const SlimBusLinkNodeType *pRoot )
{
  return (SlimBusChannelType*) PeekHead(pRoot);
}
/**
 * @brief Peek at the channel parameter list node from the head 
 *        of the linked list
 *
 * This function peeks at the channel parameter list node from 
 * the head of the linked list 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * 
 * @return Pointer to the channel parameter list node peeked
 */
SLIMBUS_INLINE SlimBusChannelParamType* PeekHeadParam( const SlimBusLinkNodeType *pRoot )
{
  return (SlimBusChannelParamType*) PeekHead(pRoot);
}
/**
 * @brief Peek at the endpoint list node from the head of the 
 *        linked list
 *
 * This function peeks at the endpoint list node from the head 
 * of the linked list 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * 
 * @return Pointer to the endpoint list node peeked
 */
SLIMBUS_INLINE SlimBusEndPointType* PeekHeadEndPoint( const SlimBusLinkNodeType *pRoot )
{
  return (SlimBusEndPointType*) PeekHead(pRoot);
}
/**
 * @brief Peek at the basic client list node from the head of 
 *        the linked list
 *
 * This function peeks at the basic client list node from the 
 * head of the linked list 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * 
 * @return Pointer to the basic client list node peeked
 */
SLIMBUS_INLINE SlimBusBasicClientCtxt* PeekHeadClient( const SlimBusLinkNodeType *pRoot )
{
  return (SlimBusBasicClientCtxt*) PeekHead(pRoot);
}

/**
 * @brief Pop a list node from the tail of the linked list
 *
 * This function pops a list node from the tail of the linked 
 * list 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * 
 * @return Pointer to the list node popped
 */
SLIMBUS_INLINE SlimBusLinkNodeType* PopTail( SlimBusLinkNodeType *pRoot )
{
  SlimBusLinkNodeType *pNode = NULL;
  
  if (NULL == pRoot )
  {
    return NULL;
  }
  if (pRoot->pPrev == pRoot)
  {
    return NULL;
  }
  
  pNode = pRoot->pPrev;
  if (NULL != pNode)
  {
    pRoot->pPrev = pNode->pPrev;
    if (NULL != pRoot->pPrev)
    {
      pRoot->pPrev->pNext = pRoot;
    }
    pNode->pNext = NULL;
    pNode->pPrev = NULL;
  }
  
  return pNode;
}
/**
 * @brief Pop an event list node from the tail of the linked 
 *        list
 *
 * This function pops an event list node from the tail of the 
 * linked list 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * 
 * @return Pointer to the list node popped
 */
SLIMBUS_INLINE SlimBusEventNodeType* PopTailEvent( SlimBusLinkNodeType *pRoot )
{
  return (SlimBusEventNodeType*) PopTail(pRoot);
}
/**
 * @brief Pop a channel list node from the tail of the linked 
 *        list
 *
 * This function pops a channel list node from the tail of the 
 * linked list 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * 
 * @return Pointer to the channel list node popped
 */
SLIMBUS_INLINE SlimBusChannelType* PopTailChannel( SlimBusLinkNodeType *pRoot )
{
  return (SlimBusChannelType*) PopTail(pRoot);
}
/**
 * @brief Pop a channel parameter list node from the tail of the
 *        linked list
 *
 * This function pops a channel parameter list node from the 
 * tail of the linked list 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * 
 * @return Pointer to the channel parameter list node popped
 */
SLIMBUS_INLINE SlimBusChannelParamType* PopTailParam( SlimBusLinkNodeType *pRoot )
{
  return (SlimBusChannelParamType*) PopTail(pRoot);
}
/**
 * @brief Pop an endpoint list node from the tail of the linked 
 *        list
 *
 * This function pops an endpoint list node from the tail of the
 * linked list 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * 
 * @return Pointer to the endpoint list node popped
 */
SLIMBUS_INLINE SlimBusEndPointType* PopTailEndPoint( SlimBusLinkNodeType *pRoot )
{
  return (SlimBusEndPointType*) PopTail(pRoot);
}
/**
 * @brief Pop a basic client list node from the tail of the 
 *        linked list
 *
 * This function pops a basic client list node from the tail of 
 * the linked list 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * 
 * @return Pointer to the basic client list node popped
 */
SLIMBUS_INLINE SlimBusBasicClientCtxt* PopTailClient( SlimBusLinkNodeType *pRoot )
{
  return (SlimBusBasicClientCtxt*) PopTail(pRoot);
}

/**
 * @brief Peek at the list node from the tail of the linked 
 *        list
 *
 * This function peeks at the list node from the tail of the 
 * linked list 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * 
 * @return Pointer to the list node peeked
 */
SLIMBUS_INLINE SlimBusLinkNodeType* PeekTail(const SlimBusLinkNodeType *pRoot)
{
  if (NULL == pRoot )
  {
    return NULL;
  }
  if (pRoot->pPrev == pRoot)
  {
    return NULL;
  }
  
  return pRoot->pPrev;
}
/**
 * @brief Peek at the event list node from the tail of the 
 *        linked list
 *
 * This function peeks at the event list node from the tail of 
 * the linked list 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * 
 * @return Pointer to the event list node peeked
 */
SLIMBUS_INLINE SlimBusEventNodeType* PeekTailEvent(const SlimBusLinkNodeType *pRoot)
{
  return (SlimBusEventNodeType*) PeekTail(pRoot);
}
/**
 * @brief Peek at the channel list node from the tail of the 
 *        linked list
 *
 * This function peeks at the channel list node from the tail of
 * the linked list 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * 
 * @return Pointer to the channel list node peeked
 */
SLIMBUS_INLINE SlimBusChannelType* PeekTailChannel(const SlimBusLinkNodeType *pRoot)
{
  return (SlimBusChannelType*) PeekTail(pRoot);
}
/**
 * @brief Peek at the channel parameter list node from the tail 
 *        of the linked list
 *
 * This function peeks at the channel parameter list node from 
 * the tail of the linked list 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * 
 * @return Pointer to the channel parameter list node peeked
 */
SLIMBUS_INLINE SlimBusChannelParamType* PeekTailParam(const SlimBusLinkNodeType *pRoot)
{
  return (SlimBusChannelParamType*) PeekTail(pRoot);
}
/**
 * @brief Peek at the endpoint list node from the tail of the 
 *        linked list
 *
 * This function peeks at the endpoint list node from the tail 
 * of the linked list 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * 
 * @return Pointer to the endpoint list node peeked
 */
SLIMBUS_INLINE SlimBusEndPointType* PeekTailEndPoint(const SlimBusLinkNodeType *pRoot)
{
  return (SlimBusEndPointType*) PeekTail(pRoot);
}
/**
 * @brief Peek at the basic client list node from the tail of 
 *        the linked list
 *
 * This function peeks at the basic client list node from 
 * the tail of the linked list 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * 
 * @return Pointer to the basic client list node peeked
 */
SLIMBUS_INLINE SlimBusBasicClientCtxt* PeekTailClient(const SlimBusLinkNodeType *pRoot)
{
  return (SlimBusBasicClientCtxt*) PeekTail(pRoot);
}

/**
 * @brief Retrieve the next list node in the linked list
 *
 * This function retrieves the next list node in the linked 
 * list.  If the pNode parameter is NULL, the head of the linked 
 * list will be retrieved.  When the end of the list has been 
 * reached, NULL will be returned.  The contents of the linked 
 * list will not be altered. 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * @param[in] pNode  Pointer to the current list node
 * 
 * @return  Pointer to the next list node retrieved
 */
SLIMBUS_INLINE SlimBusLinkNodeType *NextNode(const SlimBusLinkNodeType *pRoot, const SlimBusLinkNodeType *pNode)
{
  if ( NULL == pRoot ||
       pRoot->pNext == pRoot )
  {
    return NULL;
  }
  if ( NULL == pNode )
  {
    return pRoot->pNext;
  }
  else if ( pNode->pNext == pRoot )
  {
    return NULL;
  }
  else
  {
    return pNode->pNext;
  }
}
/**
 * @brief Retrieve the next event list node in the linked list
 *
 * This function retrieves the next event list node in the 
 * linked list.  If the pNode parameter is NULL, the head of the 
 * linked list will be retrieved.  When the end of the list has 
 * been reached, NULL will be returned.  The contents of the 
 * linked list will not be altered. 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * @param[in] pNode  Pointer to the current event list node
 * 
 * @return  Pointer to the next event list node retrieved
 */
SLIMBUS_INLINE SlimBusEventNodeType *NextEventNode(const SlimBusLinkNodeType *pRoot, const SlimBusEventNodeType *pNode)
{
  return (SlimBusEventNodeType*) NextNode(pRoot, &pNode->Node);
}
/**
 * @brief Retrieve the next channel list node in the linked list
 *
 * This function retrieves the next channel list node in the 
 * linked list.  If the pNode parameter is NULL, the head of the 
 * linked list will be retrieved.  When the end of the list has 
 * been reached, NULL will be returned.  The contents of the 
 * linked list will not be altered. 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * @param[in] pNode  Pointer to the current channel list node
 * 
 * @return  Pointer to the next channel list node retrieved
 */
SLIMBUS_INLINE SlimBusChannelType *NextChannelNode(const SlimBusLinkNodeType *pRoot, const SlimBusChannelType *pNode)
{
  return (SlimBusChannelType*) NextNode(pRoot, &pNode->GroupNode);
}
/**
 * @brief Retrieve the next channel parameter list node in the 
 *        linked list
 *
 * This function retrieves the next channel parameter list node 
 * in the linked list.  If the pNode parameter is NULL, the head 
 * of the linked list will be retrieved.  When the end of the 
 * list has been reached, NULL will be returned.  The contents 
 * of the linked list will not be altered. 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * @param[in] pNode  Pointer to the current channel parameter 
 *               list node
 * 
 * @return  Pointer to the next channel parameter list node 
 *          retrieved
 */
SLIMBUS_INLINE SlimBusChannelParamType *NextParamNode(const SlimBusLinkNodeType *pRoot, const SlimBusChannelParamType *pNode)
{
  return (SlimBusChannelParamType*) NextNode(pRoot, &pNode->LinkNode);
}
/**
 * @brief Retrieve the next endpoint list node in the linked 
 *        list
 *
 * This function retrieves the next endpoint list node in the 
 * linked list.  If the pNode parameter is NULL, the head of the 
 * linked list will be retrieved.  When the end of the list has 
 * been reached, NULL will be returned.  The contents of the 
 * linked list will not be altered. 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * @param[in] pNode  Pointer to the current endpoint list node
 * 
 * @return  Pointer to the next endpoint list node retrieved 
 */
SLIMBUS_INLINE SlimBusEndPointType *NextEndPointNode(const SlimBusLinkNodeType *pRoot, const SlimBusEndPointType *pNode)
{
  return (SlimBusEndPointType*) NextNode(pRoot, &pNode->LinkNode);
}
/**
 * @brief Retrieve the next basic client list node in the linked
 *        list
 *
 * This function retrieves the next basic client list node in 
 * the linked list.  If the pNode parameter is NULL, the head of 
 * the linked list will be retrieved.  When the end of the list 
 * has been reached, NULL will be returned.  The contents of the
 * linked list will not be altered. 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * @param[in] pNode  Pointer to the current basic client list 
 *       node
 * 
 * @return  Pointer to the next basic client list node retrieved
 */
SLIMBUS_INLINE SlimBusBasicClientCtxt *NextClientNode(const SlimBusLinkNodeType *pRoot, const SlimBusBasicClientCtxt *pNode)
{
  return (SlimBusBasicClientCtxt*) NextNode(pRoot, &pNode->LinkNode);
}

/**
 * @brief Retrieve the previous list node in the linked list
 *
 * This function retrieves the previous list node in the linked 
 * list.  If the pNode parameter is NULL, the tail of the linked 
 * list will be retrieved.  When the beginning of the list has been 
 * reached, NULL will be returned.  The contents of the linked 
 * list will not be altered. 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * @param[in] pNode  Pointer to the current list node
 * 
 * @return  Pointer to the previous list node retrieved
 */
SLIMBUS_INLINE SlimBusLinkNodeType *PrevNode(const SlimBusLinkNodeType *pRoot, const SlimBusLinkNodeType *pNode)
{
  if ( NULL == pRoot ||
       pRoot->pPrev == pRoot )
  {
    return NULL;
  }
  if ( NULL == pNode )
  {
    return pRoot->pPrev;
  }
  else if ( pNode->pPrev == pRoot )
  {
    return NULL;
  }
  else
  {
    return pNode->pPrev;
  }
}
/**
 * @brief Retrieve the previous channel parameter list node in the 
 *        linked list
 *
 * This function retrieves the previous channel parameter list node 
 * in the linked list.  If the pNode parameter is NULL, the tail 
 * of the linked list will be retrieved.  When the end of the 
 * list has been reached, NULL will be returned.  The contents 
 * of the linked list will not be altered. 
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * @param[in] pNode  Pointer to the current channel parameter 
 *               list node
 * 
 * @return  Pointer to the previous channel parameter list node 
 *          retrieved
 */
SLIMBUS_INLINE SlimBusChannelParamType *PrevParamNode(const SlimBusLinkNodeType *pRoot, const SlimBusChannelParamType *pNode)
{
  return (SlimBusChannelParamType*) PrevNode(pRoot, &pNode->LinkNode);
}

/**
 * @brief Get the number of nodes in the linked list
 *
 * This function gets the number of nodes in the linked list.
 * 
 * @param[in] pRoot  Pointer to the linked list root node
 * 
 * @return Number of nodes in the linked list
 */
SLIMBUS_INLINE uint32 GetNodeCount( const SlimBusLinkNodeType *pRoot )
{
  uint32 uCnt = 0;
  SlimBusLinkNodeType *pNode = NULL;
  
  while ( NULL != pRoot &&
          NULL != (pNode = NextNode(pRoot, pNode)) )
  {
    ++uCnt;
  }
  return uCnt;
}


#endif /* SLIMBUSLISTS_H */

