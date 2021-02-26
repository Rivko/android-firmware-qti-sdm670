/*===========================================================================

              QMP native transport for Glink Driver Source File


   Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Qualcomm Technologies, Inc. Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/06/17   gp       workaround for link state notif added.
08/17/16   dks      Initial version.
===========================================================================*/


/*===========================================================================
                        INCLUDE FILES
===========================================================================*/

#include "glink.h"
#include "glink_os_utils.h"
#include "glink_internal.h"
#include "xport_qmp.h"
#include "xport_qmp_config.h"

/*===========================================================================
                           MACRO DEFINITIONS
===========================================================================*/
#define XPORT_QMP_NAME "qmp"
#define XPORT_QMP_MAILBOX_MAGIC 0x4d41494c /* MAIL */
#define XPORT_QMP_VERSION 1
#define XPORT_QMP_FEATURES 0x0

#define XPORT_BASE_VERSION          ( 1 )
#define XPORT_BASE_FEATURE          ( 0x1 )
#define XPORT_CH_MIGRATION_FEATURE  ( 0x2 )
#define XPORT_TRACER_PACKET_FEATURE ( 0x4 )

#define XPORT_QMP_LINK_DOWN 0xFFFF0000
#define XPORT_QMP_LINK_UP 0x0000FFFF

#define XPORT_QMP_CH_DISCONNECTED 0xFFFF0000
#define XPORT_QMP_CH_CONNECTED 0x0000FFFF

enum _xport_qmp_local_state{
  LINK_DOWN, // Before init
  LINK_NEGOTIATION, //Fifo setup, INT Sent
  LINK_UP, //remote acked link up
  LOCAL_CONNECTING, //local connected, INT sent
  LOCAL_CONNECTED, //remote acked local connected
  E2ECONNECTED, //local connected acked by reomote and remote connected acked by local
  LOCAL_DISCONNECTING, //local disconnected
};

typedef enum
{
XPORT_QMP_DEBUG_CLEAN = 0,
XPORT_QMP_DEBUG_SETUP_MASTER_MAILBOX,
XPORT_QMP_DEBUG_LINK_NEGOTIATION,
XPORT_QMP_DEBUG_LINK_UP,
XPORT_QMP_DEBUG_LINK_CONNECTING,
XPORT_QMP_DEBUG_LINK_CONNECTED,
XPORT_QMP_DEBUG_E2ECONNECTED,
XPORT_QMP_DEBUG_LOCAL_DISCONNECTING,
XPORT_QMP_DEBUG_CH_OPEN,
XPORT_QMP_DEBUG_CH_CLOSE,
XPORT_QMP_DEBUG_CH_TX,
XPORT_QMP_DEBUG_RX_DONE,
XPORT_QMP_DEBUG_RESTART,
}xport_qmp_debug_event;

typedef struct 
{
  xport_qmp_debug_event event;
  uint8 state;
  uint32 local_ch_link_state;
  uint32 local_ch_link_state_ack;
  uint32 remote_ch_link_state;
  uint32 remote_ch_link_state_ack;
  uint32 local_ch_state;
  uint32 local_ch_state_ack;
  uint32 remote_ch_state;
  uint32 remote_ch_state_ack;
  uint32 magic;
}xport_qmp_debug_type;

#if defined(XPORT_QMP_DEBUG_ENABLE)
static volatile xport_qmp_debug_type xport_qmp_debug[XPORT_QMP_DEBUG_MAX_CNTR];
static volatile uint32 xport_qmp_debug_cntr = 0;
void xport_debug_log(xport_qmp_ctx_type *ctx_ptr, xport_qmp_debug_event event, uint8 state)
{
  /*for now log only for adsp*/
  if(0 == strcmp(ctx_ptr->pcfg->remote_ss, "adsp"))
  {
    xport_qmp_debug[xport_qmp_debug_cntr].event = event;  
    xport_qmp_debug[xport_qmp_debug_cntr].state = state;
    xport_qmp_debug[xport_qmp_debug_cntr].local_ch_link_state = ctx_ptr->local_ch_desc->link_state;
    xport_qmp_debug[xport_qmp_debug_cntr].local_ch_link_state_ack = ctx_ptr->local_ch_desc->link_state_ack;
    xport_qmp_debug[xport_qmp_debug_cntr].remote_ch_link_state = ctx_ptr->remote_ch_desc->link_state;
    xport_qmp_debug[xport_qmp_debug_cntr].remote_ch_link_state_ack = ctx_ptr->remote_ch_desc->link_state_ack;
    xport_qmp_debug[xport_qmp_debug_cntr].local_ch_state = ctx_ptr->local_ch_desc->ch_state;
    xport_qmp_debug[xport_qmp_debug_cntr].local_ch_state_ack = ctx_ptr->local_ch_desc->ch_state_ack;
    xport_qmp_debug[xport_qmp_debug_cntr].remote_ch_state = ctx_ptr->remote_ch_desc->ch_state;
    xport_qmp_debug[xport_qmp_debug_cntr].remote_ch_state_ack = ctx_ptr->remote_ch_desc->ch_state_ack;
    xport_qmp_debug[xport_qmp_debug_cntr].magic = ctx_ptr->ch_desc->magic;
    xport_qmp_debug_cntr = (xport_qmp_debug_cntr+1)%XPORT_QMP_DEBUG_MAX_CNTR;
  }
}
#else
#define xport_debug_log(ctx_ptr, event, state) do{}while(0)
#endif /* XPORT_QMP_DEBUG_ENABLE */

static xport_qmp_ctx_type xport_qmp_ctx[GLINK_NUM_REMOTE_HOSTS];

void xport_qmp_init_queue(xport_qmp_ctx_type *ctx)
{
  ctx->tx_q.count = 0;
  ctx->tx_q.tx_q_head = 0;
  ctx->tx_q.tx_q_tail = 0;  
}

glink_err_type xport_qmp_queue_tx_pkt (xport_qmp_ctx_type *ctx,
                                                  glink_packet_ctx *pkt_ctx)
{
  glink_packet_ctx *tmp;

  if (ctx->tx_q.count == XPORT_QMP_TX_Q_SIZE)
  {
    return GLINK_STATUS_OUT_OF_RESOURCES;
  }

  tmp = &ctx->tx_q.tx_pkt_q[ctx->tx_q.tx_q_head];
  tmp->pkt = pkt_ctx->pkt;
  tmp->size = pkt_ctx->size;
  tmp->pkt_priv = pkt_ctx->pkt_priv;

  ctx->tx_q.tx_q_head = (ctx->tx_q.tx_q_head+1)%XPORT_QMP_TX_Q_SIZE;
  ctx->tx_q.count++;

  return GLINK_STATUS_SUCCESS;
}

glink_packet_ctx* xport_qmp_dequeue_tx_pkt (xport_qmp_ctx_type *ctx)
{
  glink_packet_ctx* pkt_ctx = NULL;

  if (ctx->tx_q.count > 0)
  {
    pkt_ctx = &ctx->tx_q.tx_pkt_q[ctx->tx_q.tx_q_tail];
    ctx->tx_q.tx_q_tail = (ctx->tx_q.tx_q_tail+1)%XPORT_QMP_TX_Q_SIZE;
    ctx->tx_q.count--;
  }
  return pkt_ctx;
}

void xport_qmp_copy_pkt_ctx (xport_qmp_ctx_type *if_ctx,
                                        glink_packet_ctx *pkt_ctx)
{
  volatile uint32 *write_ptr;
  uint32 *buffer;
  uint32 *buffer_end;

  if_ctx->pkt_ctx.pkt = pkt_ctx->pkt;
  if_ctx->pkt_ctx.size = pkt_ctx->size;
  if_ctx->pkt_ctx.pkt_priv = pkt_ctx->pkt_priv;


  buffer = (uint32*)pkt_ctx->pkt;
  write_ptr = (volatile uint32*)if_ctx->local_mailbox->msg;
  buffer_end = (uint32*)((char*)pkt_ctx->pkt + pkt_ctx->size);

  while (buffer < buffer_end)
  {
    *write_ptr++ = *buffer++;
  }

  if_ctx->local_mailbox->msg_len = pkt_ctx->size;
}

#ifdef MPROC_AOP_PROC
static xport_qmp_ctx_type *xport_qmp_irq_in_to_ctx(int int_num)
{
  xport_qmp_ctx_type *ctx_ptr = NULL;
  int ind;
  /* Get the QMP context by using In IRQ number */  
  for (ind = 0; ind < GLINK_NUM_REMOTE_HOSTS; ind++)
  {
    if ((xport_qmp_ctx[ind].pcfg != NULL) && 
        (xport_qmp_ctx[ind].pcfg->irq_in == int_num))
    {
      ctx_ptr = &xport_qmp_ctx[ind];
    }
  }
  return ctx_ptr;
}
#endif /* MPROC_AOP_PROC */


#ifdef MPROC_AOP_PROC
void xport_qmp_isr(int int_num)
{
  xport_qmp_ctx_type *ctx_ptr = xport_qmp_irq_in_to_ctx(int_num);
#else
void xport_qmp_isr(xport_qmp_ctx_type *ctx_ptr)
{
#endif /* MPROC_AOP_PROC */

  ASSERT(ctx_ptr != NULL);
  
  glink_os_cs_acquire(&ctx_ptr->xport_if.glink_ch_ctx.ctx_cs);

  switch(ctx_ptr->current_state)
  {    
    case LINK_NEGOTIATION:
    {
      xport_debug_log(ctx_ptr, XPORT_QMP_DEBUG_LINK_NEGOTIATION, ctx_ptr->current_state);
      ASSERT ((ctx_ptr->local_ch_desc->link_state == 
              ctx_ptr->local_ch_desc->link_state_ack) && 
              (ctx_ptr->local_ch_desc->link_state == XPORT_QMP_LINK_UP));
      ctx_ptr->remote_ch_desc->link_state_ack =
              ctx_ptr->remote_ch_desc->link_state;
      ctx_ptr->current_state = LINK_UP;
      glink_register_transport(&ctx_ptr->xport_if);
      glink_os_send_interrupt(ctx_ptr->pcfg->irq_out);
      glink_notify_clients((void*)ctx_ptr);
      
    }
    break;

    case LINK_UP:
    {
      xport_debug_log(ctx_ptr, XPORT_QMP_DEBUG_LINK_UP, ctx_ptr->current_state);
      if (ctx_ptr->remote_ch_desc->ch_state != 
          ctx_ptr->remote_ch_desc->ch_state_ack)
      {
        if (ctx_ptr->remote_ch_desc->ch_state == XPORT_QMP_CH_CONNECTED)
        {
          ctx_ptr->remote_ch_desc->ch_state_ack = XPORT_QMP_CH_CONNECTED;
        }
        else
        {
          ctx_ptr->remote_ch_desc->ch_state_ack = XPORT_QMP_CH_DISCONNECTED;
        }
        glink_os_send_interrupt(ctx_ptr->pcfg->irq_out);
      }
    }
    break;

    case LOCAL_CONNECTING:
    {
        xport_debug_log(ctx_ptr, XPORT_QMP_DEBUG_LINK_CONNECTING, ctx_ptr->current_state);
      /* Check remote state change as well first */
      if (ctx_ptr->remote_ch_desc->ch_state != 
          ctx_ptr->remote_ch_desc->ch_state_ack)
      {
        if (ctx_ptr->remote_ch_desc->ch_state == XPORT_QMP_CH_CONNECTED)
        {
          ctx_ptr->remote_ch_desc->ch_state_ack = XPORT_QMP_CH_CONNECTED;
        }
        else
        {
          ctx_ptr->remote_ch_desc->ch_state_ack = XPORT_QMP_CH_DISCONNECTED;
        }
        glink_os_send_interrupt(ctx_ptr->pcfg->irq_out);        
      }
      /* If local state acked */
      if (ctx_ptr->local_ch_desc->ch_state_ack == XPORT_QMP_CH_CONNECTED)
      {
        ctx_ptr->current_state = LOCAL_CONNECTED;
        if (ctx_ptr->remote_ch_desc->ch_state_ack == XPORT_QMP_CH_CONNECTED)
        {
          ctx_ptr->current_state = E2ECONNECTED;
          glink_rx_cmd_remote_open((glink_transport_if *)ctx_ptr);
        }
      }
    }
    break;

    case LOCAL_CONNECTED:
    {
      xport_debug_log(ctx_ptr, XPORT_QMP_DEBUG_LINK_CONNECTED, ctx_ptr->current_state);
      if (ctx_ptr->remote_ch_desc->ch_state != ctx_ptr->remote_ch_desc->ch_state_ack)
      {
        if (ctx_ptr->remote_ch_desc->ch_state == XPORT_QMP_CH_CONNECTED)
        {
          ctx_ptr->remote_ch_desc->ch_state_ack = XPORT_QMP_CH_CONNECTED;
          ctx_ptr->current_state = E2ECONNECTED;
          glink_rx_cmd_remote_open((glink_transport_if *)ctx_ptr);
          glink_os_send_interrupt(ctx_ptr->pcfg->irq_out);
        }
      }
    }
    break;

    case E2ECONNECTED:
    {
      xport_debug_log(ctx_ptr, XPORT_QMP_DEBUG_E2ECONNECTED, ctx_ptr->current_state);
      if (ctx_ptr->remote_ch_desc->ch_state != ctx_ptr->remote_ch_desc->ch_state_ack)
      {
        /* Remote can only disconnect */
        if (ctx_ptr->remote_ch_desc->ch_state == XPORT_QMP_CH_DISCONNECTED)
        {
          ctx_ptr->remote_ch_desc->ch_state_ack = XPORT_QMP_CH_DISCONNECTED;
          xport_qmp_init_queue(ctx_ptr);
          ctx_ptr->current_state = LOCAL_CONNECTED;          
          glink_rx_cmd_remote_close((glink_transport_if *)ctx_ptr, &ctx_ptr->pkt_ctx);
        }
      }

      if (ctx_ptr->transmitting && ctx_ptr->local_mailbox->msg_len == 0)
      {
        glink_packet_ctx* pkt_ctx = xport_qmp_dequeue_tx_pkt(ctx_ptr);
        
        ctx_ptr->transmitting = FALSE;
        glink_rx_cmd_tx_done ((glink_transport_if *)ctx_ptr, &ctx_ptr->pkt_ctx);
        
        /* Start next tx */
        if (pkt_ctx)
        {
          ctx_ptr->transmitting = TRUE;
          xport_qmp_copy_pkt_ctx(ctx_ptr, pkt_ctx);
        }
      }

      if(ctx_ptr->remote_mailbox->msg_len > 0)
      {
        glink_rx_cmd_data((glink_transport_if *)ctx_ptr,
                          (void*)ctx_ptr->remote_mailbox->msg,
                          ctx_ptr->remote_mailbox->msg_len);
      }

      glink_os_send_interrupt(ctx_ptr->pcfg->irq_out);
    }
    break;

    case LOCAL_DISCONNECTING:
    {
      xport_debug_log(ctx_ptr, XPORT_QMP_DEBUG_LOCAL_DISCONNECTING, ctx_ptr->current_state);
      /* Remote could also be changing state. Ack remote change 
*/
      if (ctx_ptr->remote_ch_desc->ch_state != ctx_ptr->remote_ch_desc->ch_state_ack)
      {
        ctx_ptr->remote_ch_desc->ch_state_ack = ctx_ptr->remote_ch_desc->ch_state_ack;
      }

      /* Remote acked local disconnecting */
      if (ctx_ptr->local_ch_desc->ch_state == ctx_ptr->local_ch_desc->ch_state_ack)
      {
        glink_rx_cmd_ch_close_ack((glink_transport_if *)ctx_ptr);      
        ctx_ptr->current_state = LINK_UP;
        glink_notify_clients((void*)ctx_ptr);
      }
    }
    glink_os_send_interrupt(ctx_ptr->pcfg->irq_out);
    
    break;

    default:
      ASSERT(0);
  }
  
  glink_os_cs_release(&ctx_ptr->xport_if.glink_ch_ctx.ctx_cs);
}


/*===========================================================================
FUNCTION      xport_qmp_setup_master_mailbox
===========================================================================*/
/**

  Checks if qmp transport on remote host is ready.

  @param[in]  ctx_ptr   Pointer to transport context.

  @return     glink_err_type.

  @dependencies None.
*/
/*=========================================================================*/
void xport_qmp_setup_master_mailbox(xport_qmp_ctx_type *ctx_ptr)
{
  ctx_ptr->local_ch_desc = &(ctx_ptr->ch_desc->mCore_desc);
  ctx_ptr->remote_ch_desc = &(ctx_ptr->ch_desc->sCore_desc);
  ctx_ptr->current_state = LINK_NEGOTIATION;
  xport_debug_log(ctx_ptr, XPORT_QMP_DEBUG_SETUP_MASTER_MAILBOX, ctx_ptr->current_state);

  ctx_ptr->ch_desc->magic = 0;
  if (ctx_ptr->ch_desc->magic == XPORT_QMP_MAILBOX_MAGIC)
  {
    /* Master SSR */
    /* Just change state and wait for ack for state change */
    ctx_ptr->local_ch_desc->link_state = XPORT_QMP_LINK_UP;
    ctx_ptr->local_ch_desc->ch_state = XPORT_QMP_CH_DISCONNECTED;
  }
  else
  {
    uint32 sCore_mailbox_offset;
    ctx_ptr->ch_desc->Core_version = XPORT_QMP_VERSION;
    ctx_ptr->ch_desc->Core_features = XPORT_QMP_FEATURES;

    ctx_ptr->local_ch_desc->link_state = XPORT_QMP_LINK_UP;
    ctx_ptr->local_ch_desc->link_state_ack = XPORT_QMP_LINK_DOWN;
    ctx_ptr->local_ch_desc->ch_state = XPORT_QMP_CH_DISCONNECTED;
    ctx_ptr->local_ch_desc->ch_state_ack = XPORT_QMP_CH_DISCONNECTED;

    ctx_ptr->remote_ch_desc->link_state = XPORT_QMP_LINK_DOWN;
    ctx_ptr->remote_ch_desc->link_state_ack = XPORT_QMP_LINK_DOWN;
    ctx_ptr->remote_ch_desc->ch_state = XPORT_QMP_CH_DISCONNECTED;
    ctx_ptr->remote_ch_desc->ch_state_ack = XPORT_QMP_CH_DISCONNECTED;
    
    ctx_ptr->local_ch_desc->mailbox_offset = ROUNDUP64(sizeof(xport_qmp_ch_desc_type));
    ctx_ptr->local_ch_desc->mailbox_size = ctx_ptr->pcfg->master_mailbox_size_bytes;    
    sCore_mailbox_offset = ctx_ptr->local_ch_desc->mailbox_offset +
                           ctx_ptr->local_ch_desc->mailbox_size;    
    ctx_ptr->remote_ch_desc->mailbox_offset = ROUNDUP64(sCore_mailbox_offset);
    ctx_ptr->remote_ch_desc->mailbox_size = ctx_ptr->pcfg->mailbox_area_size_bytes 
                                     - ctx_ptr->remote_ch_desc->mailbox_offset;
  }

  ctx_ptr->local_mailbox_size = ctx_ptr->local_ch_desc->mailbox_size - 
                                sizeof(ctx_ptr->local_mailbox->msg_len);

  ctx_ptr->local_mailbox = (xport_qmp_mailbox*)((uint64)ctx_ptr->ch_desc + 
                            ctx_ptr->local_ch_desc->mailbox_offset);
  ctx_ptr->remote_mailbox = (xport_qmp_mailbox*)((uint64)ctx_ptr->ch_desc + 
                            ctx_ptr->remote_ch_desc->mailbox_offset);

  ctx_ptr->local_mailbox->msg_len = 0;
  ctx_ptr->remote_mailbox->msg_len = 0;
  ctx_ptr->ch_desc->magic = XPORT_QMP_MAILBOX_MAGIC;

  glink_os_send_interrupt(ctx_ptr->pcfg->irq_out);
}

glink_err_type xport_qmp_ch_open (glink_transport_if *ctx)
{
  xport_qmp_ctx_type *if_ctx = (xport_qmp_ctx_type *)ctx;

  xport_debug_log(if_ctx, XPORT_QMP_DEBUG_CH_OPEN, if_ctx->current_state);

  ASSERT (if_ctx->current_state == LINK_UP);
  if_ctx->local_ch_desc->ch_state = XPORT_QMP_CH_CONNECTED;
  if_ctx->current_state = LOCAL_CONNECTING;

  glink_os_send_interrupt(if_ctx->pcfg->irq_out);

  return GLINK_STATUS_SUCCESS;
}

glink_err_type xport_qmp_ch_close (glink_transport_if *ctx)
{
  xport_qmp_ctx_type *if_ctx = (xport_qmp_ctx_type *)ctx;

  xport_debug_log(if_ctx, XPORT_QMP_DEBUG_CH_CLOSE, if_ctx->current_state);

  if (if_ctx->current_state != LOCAL_CONNECTED &&
      if_ctx->current_state != E2ECONNECTED)
  {
    return GLINK_STATUS_CH_ALREADY_CLOSED;
  }

  if_ctx->local_ch_desc->ch_state = XPORT_QMP_CH_DISCONNECTED;
  if_ctx->current_state = LOCAL_DISCONNECTING;

  glink_os_send_interrupt(if_ctx->pcfg->irq_out);
  
  return GLINK_STATUS_SUCCESS;
}

glink_err_type xport_qmp_tx (glink_transport_if *ctx, 
                                    glink_packet_ctx *pkt_ctx)
{
  xport_qmp_ctx_type *if_ctx = (xport_qmp_ctx_type *)ctx;

  xport_debug_log(if_ctx, XPORT_QMP_DEBUG_CH_TX, if_ctx->current_state);

  if (if_ctx->current_state != E2ECONNECTED)
  {
    return GLINK_STATUS_CH_NOT_FULLY_OPENED;
  }

  if (pkt_ctx->size > if_ctx->local_mailbox_size)
  {
    return GLINK_STATUS_OUT_OF_RESOURCES;
  }

  if (if_ctx->transmitting || if_ctx->tx_q.count > 0)
  {
    return xport_qmp_queue_tx_pkt (if_ctx, pkt_ctx);
  }

  if_ctx->transmitting = TRUE;
  xport_qmp_copy_pkt_ctx(if_ctx, pkt_ctx);
  glink_os_send_interrupt(if_ctx->pcfg->irq_out);

  return GLINK_STATUS_SUCCESS;
}


glink_err_type xport_qmp_rx_done (glink_transport_if *ctx, const void *ptr)
{
  xport_qmp_ctx_type *if_ctx = (xport_qmp_ctx_type *)ctx;

  if (if_ctx->current_state != E2ECONNECTED)
  {
    return GLINK_STATUS_CH_NOT_FULLY_OPENED;
  }

  if (if_ctx->remote_mailbox->msg != ptr || if_ctx->remote_mailbox->msg_len == 0)
  {
    return GLINK_STATUS_INVALID_PARAM;
  }

  if_ctx->remote_mailbox->msg_len = 0;

  glink_os_send_interrupt(if_ctx->pcfg->irq_out);

  return GLINK_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION      xport_qmp_init
===========================================================================*/
/**

  Initializes qmp transport.
  Must be called before any other operations are done.

  @param[in]  arg   The argument.

  @return     Returns error code.

  @sideeffects  None.
*/
/*=========================================================================*/
void xport_qmp_init(void *arg)
{  
  int ind;
  /* Initialize context */
  for (ind = 0; ind < GLINK_NUM_REMOTE_HOSTS; ind++)
  {
    xport_qmp_ctx[ind].pcfg = xport_qmp_get_config(ind);
    if (xport_qmp_ctx[ind].pcfg == NULL)
    {
      continue;
    }

    xport_qmp_ctx[ind].ch_desc = xport_qmp_alloc_mailbox(ind);
    if (xport_qmp_ctx[ind].ch_desc == NULL)
    {
      xport_qmp_ctx[ind].pcfg = NULL;
      continue;
    }

    glink_os_mem_set((void*)&xport_qmp_ctx[ind].ch_desc[0], 0, 60);

    glink_os_cs_init (&(xport_qmp_ctx[ind].xport_if.glink_ch_ctx.ctx_cs));
    strlcpy ((char *)xport_qmp_ctx[ind].xport_if.glink_ch_ctx.remote_ss,
             xport_qmp_ctx[ind].pcfg->remote_ss,
             sizeof(xport_qmp_ctx[ind].xport_if.glink_ch_ctx.remote_ss));
    strlcpy ((char *)xport_qmp_ctx[ind].xport_if.glink_ch_ctx.ch_name,
             xport_qmp_ctx[ind].pcfg->ch_name,
             sizeof(xport_qmp_ctx[ind].xport_if.glink_ch_ctx.ch_name));

    glink_os_enable_interrupt(xport_qmp_ctx[ind].pcfg->irq_in);
  
    glink_os_register_isr(xport_qmp_ctx[ind].pcfg->irq_in, xport_qmp_isr,
                              &xport_qmp_ctx[ind]);
    
    xport_qmp_setup_master_mailbox((xport_qmp_ctx_type *)&xport_qmp_ctx[ind]);  
  }
}

