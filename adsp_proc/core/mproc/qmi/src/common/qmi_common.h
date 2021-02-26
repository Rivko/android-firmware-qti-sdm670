#ifndef QMI_COMMON_H
#define QMI_COMMON_H
/******************************************************************************
  @file    qmi_common.h
  @brief   The QMI Common definition header file

  DESCRIPTION
  QMI common defines
   
  ---------------------------------------------------------------------------
  Copyright (c) 2010-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/
#include <string.h>
#include "qmi_common_target.h"

#define QMI_REQUEST_CONTROL_FLAG 0x00
#define QMI_RESPONSE_CONTROL_FLAG 0x02
#define QMI_INDICATION_CONTROL_FLAG 0x04
#define QMI_HEADER_SIZE 7
#define MAX_ADDR_LEN 16
#define VERSION_MASK 0xff
#define GET_VERSION(x) (x & 0xff)
#define SET_VERSION(x) (x & 0xff)
#define INSTANCE_MASK 0xff00
#define GET_INSTANCE(x) ((x & 0xff00) >> 8)
#define SET_INSTANCE(x) ((x & 0xff) << 8)
#define QMUX_FIRST_INSTANCE         (0x10)
#define QMUX_CONN_TO_INSTANCE(conn) ((conn) + QMUX_FIRST_INSTANCE)
#define INSTANCE_TO_QMUX_CONN(inst) ((inst) - QMUX_FIRST_INSTANCE)

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

#define LIST_INIT(list) do {  \
  (list).head = (list).tail = NULL; \
  (list).count = 0; \
} while(0)

#define LINK_INIT(link) (link).next = (link).prev = NULL

#define LIST_HEAD(list) (list.head)
#define LIST_TAIL(list) (list.tail)
#define LIST_CNT(list) (list.count)
#define LIST_REMOVE(list, node, link) \
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

#define LIST_ADD(list, node, link) \
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

#define LIST_FIND(list, iter, link, test) do {  \
  iter = (list).head; \
  while(iter) { \
    if(test) {  \
      break;  \
    } \
    iter = (iter)->link.next;  \
  } \
} while(0)


/* QMI message format:
 * +------------------+---------------+---------------+------------+
 * | 1-byte cntl flag | 2-byte txn ID | 2-byte msg ID | 2-byte len |
 * +------------------+---------------+---------------+------------+
 */
static __inline void encode_header
(
 unsigned char *buf, 
 unsigned char cntl_flag,
 uint16_t txn_id,
 uint16_t msg_id,
 uint16_t msg_len
 )
{
  QMI_MEM_COPY(buf, 7, &cntl_flag, 1);
  QMI_MEM_COPY(buf+1, 6, &txn_id, 2);
  QMI_MEM_COPY(buf+3, 4, &msg_id, 2);
  QMI_MEM_COPY(buf+5, 2, &msg_len, 2);
}

static __inline void decode_header
(
 unsigned char *buf, 
 uint8_t *cntl_flag,
 uint16_t *txn_id,
 uint16_t *msg_id,
 uint16_t *msg_len
)
{
  QMI_MEM_COPY(cntl_flag, sizeof(uint8_t), buf, 1);
  QMI_MEM_COPY(txn_id, sizeof(uint16_t), buf+1, 2);
  QMI_MEM_COPY(msg_id, sizeof(uint16_t), buf+3, 2);
  QMI_MEM_COPY(msg_len, sizeof(uint16_t), buf+5, 2);
}
#endif
