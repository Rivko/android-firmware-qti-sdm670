#ifndef _IPC_ROUTER_INTERNAL_H_
#define _IPC_ROUTER_INTERNAL_H_
/*===========================================================================

                 I P C    R O U T E R   I N T E R N A L

   Internal helper macros

  ---------------------------------------------------------------------------
  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
===========================================================================*/

#define LIST(type, name) \
  struct { \
    type *head; \
    type *tail; \
    unsigned int count; \
  } name

#define LINK(type, link) \
  struct { \
    type *prev; \
    type *next; \
  } link

#define LIST_HEAD(list) ((list).head)
#define LIST_TAIL(list) ((list).tail)
#define LIST_CNT(list) ((list).count)
#define LIST_INIT(list) (list).head = (list).tail = NULL; (list).count = 0
#define LINK_INIT(link) (link).next = (link).prev = NULL

#define LIST_REMOVE(list, node, link) \
  do { \
    if((node)->link.prev) \
      (node)->link.prev->link.next = (node)->link.next; \
    else /* node at the front of the list */ \
      (list).head = (node)->link.next; \
    if((node)->link.next) \
      (node)->link.next->link.prev = (node)->link.prev; \
    else /* node at the end of the list */ \
      (list).tail = (node)->link.prev; \
    (list).count--; \
  } while(0)

#define LIST_ADD(list, node, link) \
  do { \
    if(!(list).tail) \
    { \
      /* first node on the list */ \
      (list).tail = (list).head = node; \
    } \
    else \
    { \
      (node)->link.prev = (list).tail; \
      (list).tail->link.next = node; \
      (list).tail = node; \
    } \
    list.count++; \
  } while(0) 

#define LIST_PREPEND(list, node, link) \
  do { \
    if(!(list).tail) \
    { \
      /* first node on the list */ \
      (list).tail = (list).head = node; \
    } \
    else \
    { \
      (node)->link.next = (list).head; \
      (list).head->link.prev = node; \
      (list).head = node; \
    } \
    (list).count++; \
  } while(0) 

#define LIST_FIND(list, iter, link, test) \
  do {  \
    iter = (list).head; \
    while(iter) { \
      if(test) {  \
        break;  \
      } \
      iter = (iter)->link.next;  \
    } \
} while(0)

#endif
