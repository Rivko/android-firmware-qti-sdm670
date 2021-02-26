#ifndef XPORT_UART_H
#define XPORT_UART_H

/**
 * @file xport_uart_hdlc.h
 *
 * Internal definitions for UART Glink transport with HDLC framing
 */

/*==============================================================================
     Copyright (c) 2014 QUALCOMM Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1.c4/mproc/glink/xport_uart_hdlc/inc/xport_uart_hdlc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/09/14   an      Initial version
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/

/*===========================================================================
                      MACRO DECLARATIONS
===========================================================================*/

/* 5 ms interval between sending WAKE requests */
#define UART_WAKEUP_TIMEOUT  5
/* 10 ms idle interval to send SLEEP notification */
#define UART_IDLE_TIMEOUT  10
/* max number of bytes to send at once */
#define UART_TX_LIMIT  8

#define UART_DEFAULT_MUX_ID         1
#define UART_RX_INTENT_DEFAULT_SIZE 0x400
#define UART_RX_INTENT_DEFAULT_NUM  0x8

#define HDLC_FRAME_DELIM_CHAR ((char)0x7E)
#define HDLC_ESCAPE_CHAR      ((char)0x7D)

#define UART_WAKE_CHAR        ((char)0xFD)
#define UART_WAKE_ACK_CHAR    ((char)0xFC)
#define UART_SLEEP_CHAR       ((char)0xFE)
#define UART_RX_ACK_CHAR      ((char)0x11)

#define UART_ESCAPE_MASK      0x20

#define XPORT_UART_ESCAPE_CHAR(c) \
  switch (c) \
  { \
    case HDLC_FRAME_DELIM_CHAR: \
    case HDLC_ESCAPE_CHAR: \
    case UART_WAKE_CHAR: \
    case UART_WAKE_ACK_CHAR: \
    case UART_SLEEP_CHAR: \
      c ^= UART_ESCAPE_MASK; \
      break; \
    default: \
      break; \
  }

/*===========================================================================
                      TYPE DECLARATIONS
===========================================================================*/

typedef enum {
  /* sleep command sent/received */
  XPORT_UART_STATE_SLEEP,
  /* wake command sent/received */
  XPORT_UART_STATE_WAKING_UP,
  /* wake ack sent/received */
  XPORT_UART_STATE_AWAKE
} xport_uart_link_state_type;

typedef enum {
  /* data is ready to transmit */
  XPORT_SIO_TRANSMIT_START,
  /* nothing to transmit */
  XPORT_SIO_TRANSMIT_IDLE,
  /* wake timeout expired */
  XPORT_SIO_WAKE_TIMEOUT,
  /* wake request received */
  XPORT_SIO_WAKE_RECEIVED,
  /* wake ack received */
  XPORT_SIO_WAKE_ACK_RECEIVED,
  /* sleep request received */
  XPORT_SIO_SLEEP_RECEIVED,
  /* idle timeout expired */
  XPORT_SIO_IDLE_TIMEOUT,
} xport_uart_sm_event_type;

typedef struct xport_uart_buff_desc {
  smem_list_link_type next;
  glink_core_tx_pkt_type pkt;
  char* pcurr;
  char* pend;
  uint16 checksum;
  uint32 cid;
  uint32 iid;
} xport_uart_buff_desc_type;

#endif //XPORT_UART_H

