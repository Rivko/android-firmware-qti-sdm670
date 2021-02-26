/*===========================================================================

              UART based transport for Glink Driver Source File


  Copyright (c) 2014 by Qualcomm Technologies Incorporated.  
  All rights reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/14/14   an      Register callback for Rx high signal.
03/26/14   an      Initial version.
===========================================================================*/


/*===========================================================================
                        INCLUDE FILES
===========================================================================*/

#include "smem_list.h"
#include "glink.h"
#include "glink_core_if.h"
#include "glink_transport_if.h"
#include "xport_uart_hdlc.h"
#include "glink_os.h"
#include "glink_internal.h"

#include "sio.h"
#include "dsm.h"
#include "crc.h"
#include "assert.h"

/*===========================================================================
                           MACRO DEFINITIONS
===========================================================================*/
#define XPORT_UART_HDLC_LOG(msg, param) \
          GLINK_LOG_EVENT(GLINK_EVENT_XPORT_INTERNAL, msg, "UART", \
            "wcnss", (uint32)param)


/*===========================================================================
                              GLOBAL DATA DECLARATIONS
===========================================================================*/

/* UART transport interface */
glink_transport_if_type xport_uart_if;

/* UART transport context */
struct _xport_uart_ctx
{
  xport_uart_link_state_type tx_state;
  xport_uart_link_state_type rx_state;
  void* sm_lock;
  void* ch_lock;
  void* tx_list_lock;
  void* rx_list_lock;
  smem_list_type tx_list;
  smem_list_type rx_list;
  void* tx_free_list_lock;
  void* rx_free_list_lock;
  smem_list_type tx_free_list;
  smem_list_type rx_free_list;
  void* rx_ready_list_lock;
  smem_list_type rx_ready_list;
  uint32 num_ch_open;
  dsm_watermark_type sio_wmq;
  q_type sio_tx_dsm_q;
  sio_open_type sio_open_param;
  glink_timer wakeup_timer;
  glink_timer idle_timer;
  boolean send_wake_ack;
  boolean escape_received;
  uint32 ack_received;
  boolean dropping_rx_frame;
  char next_tx_byte;
  uint16 checksum_seed;
  uint32 wakeup_cnt;
  uint32 wakeup_retry_cnt;
  uint32 sleep_cnt;
  uint32 drop_crc_mismatch_cnt;
  uint32 drop_small_buffer_cnt;
  uint32 drop_no_rx_buffer_cnt;
  uint32 drop_invalid_frame_cnt;
  uint32 drop_invalid_mux_id_cnt;
} xport_uart_ctx = {0};

/*===========================================================================
                    LOCAL FUNCTION DEFINITIONS
===========================================================================*/
void xport_uart_sm_process_event(xport_uart_sm_event_type event);
void xport_uart_sio_process_rx(dsm_item_type** dsm_item_ptr);
void xport_uart_sio_transmit(void);
boolean xport_uart_sio_send_power_ctrl(char cmd);

/*===========================================================================
                    EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION      xport_uart_tx_cmd_version

DESCRIPTION   Transmit a version command for local negotiation - 
              expected response is to be delivered via 
              glink_rx_cmd_version_ack()

ARGUMENTS     if_ptr  - Pointer to transport interface instance.
              version - Version to be sent.
              feature - Features to be sent.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void xport_uart_tx_cmd_version
(
  glink_transport_if_type *if_ptr,
  uint32 version,
  uint32 features
)
{
  xport_uart_if.glink_core_if_ptr->rx_cmd_version_ack(if_ptr, version, features);
  xport_uart_if.glink_core_if_ptr->rx_cmd_version(if_ptr, version, features);
}

/*===========================================================================
FUNCTION      xport_uart_tx_cmd_version_ack

DESCRIPTION   Transmit a version ack for remote negotiation.

ARGUMENTS     if_ptr  - Pointer to transport interface instance.
              version - Version to be sent.
              feature - Features to be sent.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void xport_uart_tx_cmd_version_ack
(
  glink_transport_if_type *if_ptr,
  uint32 version,
  uint32 features
)
{
}

/*===========================================================================
FUNCTION      xport_uart_set_version

DESCRIPTION   Signals that negotiation is complete and the transport can now 
              do version-specific initialization.

ARGUMENTS     if_ptr  - Pointer to transport interface instance.
              version - Version to be used.
              feature - Features to be used.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void xport_uart_set_version
(
  glink_transport_if_type *if_ptr,
  uint32 version,
  uint32 features
)
{
}

/*===========================================================================
FUNCTION      xport_uart_tx_cmd_ch_open

DESCRIPTION   Sends the open command - expected response is to be delivered 
              via glink_rx_cmd_ch_open_ack().

ARGUMENTS     if_ptr  - Pointer to transport interface instance.
              lcid    - Local channel ID.
              name    - Channel name.

RETURN VALUE  Returns error code.

SIDE EFFECTS  None.
===========================================================================*/
glink_err_type xport_uart_tx_cmd_ch_open
(
  glink_transport_if_type *if_ptr,
  uint32 lcid,
  const char *name
)
{
  glink_lock_acquire(xport_uart_ctx.ch_lock);

  if (xport_uart_ctx.num_ch_open++ == 0)
  {
    /* Open the sio port */
    if (sio_open(&xport_uart_ctx.sio_open_param) == SIO_NO_STREAM_ID)
    {
      /* open error */
      glink_lock_release(xport_uart_ctx.ch_lock);
      return GLINK_STATUS_FAILURE;
    }
    
    /* Start inactivity timer once Uart is started and power down'd 
       otherwise if there is some noise on Uart, it will be awake
       till its power down is done which will happen after some tx-rx only */
    {
        sio_ioctl_param_type sio_ioctl_param;

        sio_ioctl_param.inactivity_ms = 30;
        sio_ioctl(xport_uart_ctx.sio_open_param.stream_id, 
                  SIO_IOCTL_INACTIVITY_TIMER, &sio_ioctl_param);
    }
    
    /* power down UART at start */
    sio_ioctl(xport_uart_ctx.sio_open_param.stream_id,
      SIO_IOCTL_POWERDOWN, NULL);
  }

  glink_lock_release(xport_uart_ctx.ch_lock);

  xport_uart_if.glink_core_if_ptr->rx_cmd_ch_open_ack(if_ptr, lcid);
  xport_uart_if.glink_core_if_ptr->rx_cmd_ch_remote_open(if_ptr, lcid, name);

  return GLINK_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION      xport_uart_tx_cmd_ch_close

DESCRIPTION   Sends the close command - expected response is to be delivered 
              via glink_rx_cmd_ch_close_ack().

ARGUMENTS     if_ptr  - Pointer to transport interface instance.
              lcid    - Local channel ID.

RETURN VALUE  Returns error code.

SIDE EFFECTS  None.
===========================================================================*/
glink_err_type xport_uart_tx_cmd_ch_close
(
  glink_transport_if_type *if_ptr,
  uint32 lcid
)
{
  xport_uart_buff_desc_type* pdesc;

  /* have to acquire the lock to make sure channel state is still opened
   * while we are receiving data */
  glink_lock_acquire(xport_uart_ctx.ch_lock);

  /* verify there is nothing in Rx ready list */
  ASSERT(smem_list_count(&xport_uart_ctx.rx_ready_list) == 0);

  /* clean up rx intents */
  do
  {
    glink_lock_acquire(xport_uart_ctx.rx_list_lock);

    for (pdesc = smem_list_first(&xport_uart_ctx.rx_list);
      pdesc != NULL;
      pdesc = smem_list_next(pdesc))
    {
      if (pdesc->cid == lcid)
      {
        smem_list_delete(&xport_uart_ctx.rx_list, pdesc);
        break;
      }
    }

    glink_lock_release(xport_uart_ctx.rx_list_lock);

    if (pdesc != NULL)
    {
      glink_free(pdesc->pkt.data);

      glink_lock_acquire(xport_uart_ctx.rx_free_list_lock);
      smem_list_append(&xport_uart_ctx.rx_free_list, pdesc);
      glink_lock_release(xport_uart_ctx.rx_free_list_lock);
    }
  } while (pdesc != NULL);

  xport_uart_if.glink_core_if_ptr->rx_cmd_ch_close_ack(if_ptr, lcid);

  if (--xport_uart_ctx.num_ch_open == 0)
  {
    glink_timer_stop(&xport_uart_ctx.idle_timer);
    glink_timer_stop(&xport_uart_ctx.wakeup_timer);

    if (xport_uart_ctx.tx_state == XPORT_UART_STATE_AWAKE)
    {
      xport_uart_sio_send_power_ctrl(UART_SLEEP_CHAR);
      xport_uart_ctx.tx_state = XPORT_UART_STATE_SLEEP;
      xport_uart_ctx.rx_state = XPORT_UART_STATE_SLEEP;
      xport_uart_ctx.escape_received = FALSE;
      xport_uart_ctx.ack_received = FALSE;
    }

    sio_close(xport_uart_ctx.sio_open_param.stream_id, NULL);
  }

  glink_lock_release(xport_uart_ctx.ch_lock);

  return GLINK_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION      xport_uart_tx_cmd_ch_remote_open_ack

DESCRIPTION   Sends the remote open ACK command.

ARGUMENTS     if_ptr  - Pointer to transport interface instance.
              rcid    - Remote channel ID.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void xport_uart_tx_cmd_ch_remote_open_ack
(
  glink_transport_if_type *if_ptr,
  uint32 rcid
)
{
  uint32 i;
  
  for (i = 0; i < UART_RX_INTENT_DEFAULT_NUM; i++)
  {
    xport_uart_if.glink_core_if_ptr->rx_cmd_remote_rx_intent_put(
      &xport_uart_if, rcid, i, UART_RX_INTENT_DEFAULT_SIZE);
  }
}

/*===========================================================================
FUNCTION      xport_uart_tx_cmd_ch_remote_close_ack

DESCRIPTION   Sends the remote close ACK command.

ARGUMENTS     if_ptr  - Pointer to transport interface instance.
              rcid    - Remote channel ID.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void xport_uart_tx_cmd_ch_remote_close_ack
(
  glink_transport_if_type *if_ptr,
  uint32 rcid
)
{
}

/*===========================================================================
FUNCTION      xport_uart_tx_cmd_local_rx_intent

DESCRIPTION   Send receive intent command.

ARGUMENTS     if_ptr  - Pointer to transport interface instance.
              lcid    - Local channel ID.
              size    - Size of receive intent.
              liid    - Local intent ID.

RETURN VALUE  Returns error code.

SIDE EFFECTS  None.
===========================================================================*/
glink_err_type xport_uart_tx_cmd_local_rx_intent
(
  glink_transport_if_type *if_ptr,
  uint32 lcid,
  size_t size,
  uint32 liid
)
{
  xport_uart_buff_desc_type* pdesc;
  
  glink_lock_acquire(xport_uart_ctx.rx_free_list_lock);
  pdesc = smem_list_first(&xport_uart_ctx.rx_free_list);
  if (pdesc != NULL)
  {
    smem_list_delete(&xport_uart_ctx.rx_free_list, pdesc);
  }
  glink_lock_release(xport_uart_ctx.rx_free_list_lock);
  
  if (pdesc == NULL)
  {
    pdesc = glink_malloc(sizeof(xport_uart_buff_desc_type));
    
    if (!pdesc)
    {
      return GLINK_STATUS_OUT_OF_RESOURCES;
    }
  }
  
  size += sizeof(pdesc->checksum);
  pdesc->pkt.data = glink_malloc(size);
  
  if (pdesc->pkt.data == NULL)
  {
    glink_lock_acquire(xport_uart_ctx.rx_free_list_lock);
    smem_list_append(&xport_uart_ctx.rx_free_list, pdesc);
    glink_lock_release(xport_uart_ctx.rx_free_list_lock);
    
    return GLINK_STATUS_OUT_OF_RESOURCES;
  }
  
  pdesc->pkt.size = size;
  pdesc->pcurr = NULL;
  pdesc->pend = NULL;
  pdesc->cid = lcid;
  pdesc->iid = liid;
  
  glink_lock_acquire(xport_uart_ctx.rx_list_lock);
  smem_list_append(&xport_uart_ctx.rx_list, pdesc);
  glink_lock_release(xport_uart_ctx.rx_list_lock);
  
  return GLINK_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION      xport_uart_tx_cmd_local_rx_done

DESCRIPTION   Send receive-done command.

ARGUMENTS     if_ptr  - Pointer to transport interface instance.
              lcid    - Local channel ID.
              liid    - Local intent ID.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void xport_uart_tx_cmd_local_rx_done
(
  glink_transport_if_type *if_ptr,
  uint32 lcid,
  uint32 liid
)
{
  xport_uart_buff_desc_type* pdesc;
  
  glink_lock_acquire(xport_uart_ctx.rx_ready_list_lock);
  
  for (pdesc = smem_list_first(&xport_uart_ctx.rx_ready_list);
       pdesc != NULL;
       pdesc = smem_list_next(pdesc))
  {
    if (pdesc->iid == liid && pdesc->cid == lcid)
    {
      smem_list_delete(&xport_uart_ctx.rx_ready_list, pdesc);
      break;
    }
  }
  
  glink_lock_release(xport_uart_ctx.rx_ready_list_lock);
  
  if (pdesc != NULL)
  {
    glink_free(pdesc->pkt.data);
    
    glink_lock_acquire(xport_uart_ctx.rx_free_list_lock);
    smem_list_append(&xport_uart_ctx.rx_free_list, pdesc);
    glink_lock_release(xport_uart_ctx.rx_free_list_lock);
  }
}

/*===========================================================================
FUNCTION      xport_uart_tx

DESCRIPTION   Send a data packet.

ARGUMENTS     if_ptr  - Pointer to transport interface instance.
              lcid    - Local channel ID.
              pctx    - Packet TX context.

RETURN VALUE  Returns error code.

SIDE EFFECTS  None.
===========================================================================*/
glink_err_type xport_uart_tx
(
  glink_transport_if_type *if_ptr,
  uint32 lcid,
  glink_core_tx_pkt_type *pctx
)
{
  xport_uart_buff_desc_type* pdesc;
  
  glink_lock_acquire(xport_uart_ctx.tx_free_list_lock);
  pdesc = smem_list_first(&xport_uart_ctx.tx_free_list);
  if (pdesc != NULL)
  {
    smem_list_delete(&xport_uart_ctx.tx_free_list, pdesc);
  }
  glink_lock_release(xport_uart_ctx.tx_free_list_lock);
  
  if (pdesc == NULL)
  {
    pdesc = glink_malloc(sizeof(xport_uart_buff_desc_type));
    
    if (!pdesc)
    {
      return GLINK_STATUS_OUT_OF_RESOURCES;
    }
  }
  
  pdesc->pkt = *pctx;
  pdesc->pcurr = NULL;
  pdesc->pend = NULL;
  pdesc->checksum = crc_16_step(xport_uart_ctx.checksum_seed, 
                                (byte*)pctx->data, pctx->size);
  pdesc->cid = lcid;
  pdesc->iid = pctx->iid;
  
  glink_lock_acquire(xport_uart_ctx.tx_list_lock);
  smem_list_append(&xport_uart_ctx.tx_list, pdesc);
  glink_lock_release(xport_uart_ctx.tx_list_lock);
  
  xport_uart_sm_process_event(XPORT_SIO_TRANSMIT_START);
  
  return GLINK_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION      xport_uart_sio_process_rx

DESCRIPTION   Notification callback to receives data from SIO.

ARGUMENTS     dsm_item_ptr  - pointer to DSM item pointer.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void xport_uart_sio_process_rx(dsm_item_type** dsm_item_ptr)
{
  uint32 i;
  xport_uart_buff_desc_type* pdesc;
  
  /* have to acquire the lock to make sure channel state is still opened
   * while we are receiving data */
  glink_lock_acquire(xport_uart_ctx.ch_lock);

  pdesc = smem_list_first(&xport_uart_ctx.rx_list);
  
  for (i = 0; i < (*dsm_item_ptr)->used; i++)
  {
    char c = (*dsm_item_ptr)->data_ptr[i];
    
    switch (c)
    {
      case HDLC_FRAME_DELIM_CHAR:
        xport_uart_ctx.escape_received = FALSE;
        
        if (xport_uart_ctx.dropping_rx_frame)
        {
          xport_uart_ctx.dropping_rx_frame = FALSE;
        }
        else if (pdesc != NULL && pdesc->pcurr != NULL)
        {
          uint32 len = pdesc->pcurr - (char*)pdesc->pkt.data;
          
          XPORT_UART_HDLC_LOG("Received frame", len);
          
          if (len > sizeof(pdesc->checksum))
          {
            uint16 checksum;
            
            len -= sizeof(pdesc->checksum);
            checksum = ((uint16)((char*)pdesc->pkt.data)[len] & 0xFF) |
              (((uint16)((char*)pdesc->pkt.data)[len + 1] & 0xFF) << 8);
            pdesc->checksum = crc_16_step(xport_uart_ctx.checksum_seed, 
                                          pdesc->pkt.data, len);
                                          
            if (pdesc->checksum == checksum)
            {
              /* everything looks good, notify core that data is ready */
              glink_lock_acquire(xport_uart_ctx.rx_list_lock);
              smem_list_delete(&xport_uart_ctx.rx_list, pdesc);
              glink_lock_release(xport_uart_ctx.rx_list_lock);
              
              glink_lock_acquire(xport_uart_ctx.rx_ready_list_lock);
              smem_list_append(&xport_uart_ctx.rx_ready_list, pdesc);
              glink_lock_release(xport_uart_ctx.rx_ready_list_lock);
              
              xport_uart_if.glink_core_if_ptr->rx_cmd_rx_data(&xport_uart_if, 
                                                              pdesc->cid, 
                                                              pdesc->iid,
                                                              len,
                                                              pdesc->pkt.data,
                                                              len);
              /* indicate to grab next Rx descriptor */
              pdesc = NULL;
            }
            else
            {
              xport_uart_ctx.drop_crc_mismatch_cnt++;
              XPORT_UART_HDLC_LOG("Dropping frame - CRC mismatch", checksum);
              XPORT_UART_HDLC_LOG("Expected CRC value", pdesc->checksum);
            }
          }
          else
          {
            xport_uart_ctx.drop_invalid_frame_cnt++;
            XPORT_UART_HDLC_LOG("Dropping invalid frame", len);
          }
          
          if (pdesc == NULL)
          {
            pdesc = smem_list_first(&xport_uart_ctx.rx_list);
          }
          else
          {
            /* frame was dropped, reinitialize descriptor */
            pdesc->pcurr = NULL;
            pdesc->pend = NULL;
          }
        }
        break;
      case HDLC_ESCAPE_CHAR:
        xport_uart_ctx.escape_received = TRUE;
        break;
      case UART_WAKE_CHAR:
        xport_uart_sm_process_event(XPORT_SIO_WAKE_RECEIVED);
        break;
      case UART_WAKE_ACK_CHAR:
        xport_uart_sm_process_event(XPORT_SIO_WAKE_ACK_RECEIVED);
        break;
      case UART_SLEEP_CHAR:
        xport_uart_sm_process_event(XPORT_SIO_SLEEP_RECEIVED);
        break;
      case UART_RX_ACK_CHAR:
        xport_uart_ctx.ack_received = TRUE;
        xport_uart_sio_transmit();
        break;
      default:
        if (xport_uart_ctx.dropping_rx_frame)
        {
          break;
        }

        if (xport_uart_ctx.escape_received)
        {
          xport_uart_ctx.escape_received = FALSE;
          c ^= UART_ESCAPE_MASK;
        }
        
        if (pdesc != NULL)
        {
          if (pdesc->pcurr != NULL)
          {
            /* receiving frame data */
            if (pdesc->pcurr < pdesc->pend)
            {
              *pdesc->pcurr++ = c;
            }
            else
            {
              xport_uart_ctx.dropping_rx_frame = TRUE;
              xport_uart_ctx.drop_small_buffer_cnt++;
              pdesc->pcurr = NULL;
              pdesc->pend = NULL;

              XPORT_UART_HDLC_LOG("Dropping RX frame - buffer too small", pdesc->pkt.size);
            }
          }
          else
          {
            /* it should be a Mux ID byte */
            if (c == UART_DEFAULT_MUX_ID)
            {
              pdesc->pcurr = pdesc->pkt.data;
              pdesc->pend = (char*)pdesc->pkt.data + pdesc->pkt.size;

              XPORT_UART_HDLC_LOG("Start receiving frame", 0);
            }
            else
            {
              xport_uart_ctx.dropping_rx_frame = TRUE;
              xport_uart_ctx.drop_invalid_mux_id_cnt++;
              XPORT_UART_HDLC_LOG("Dropping RX frame - invalid mux ID", c);
            }
          }
        }
        else
        {
          /* no Rx buffer available, dropping frame */
          xport_uart_ctx.dropping_rx_frame = TRUE;
          xport_uart_ctx.drop_no_rx_buffer_cnt++;

          XPORT_UART_HDLC_LOG("Dropping RX frame - no buffer", 0);
        }
        
        break;
    }
  }
  
  glink_lock_release(xport_uart_ctx.ch_lock);

  dsm_free_packet(dsm_item_ptr);
}

/*===========================================================================
FUNCTION      xport_uart_sio_send_power_ctrl

DESCRIPTION   Send WAKE request and start response timer.

ARGUMENTS     cmd  - power control command to be sent.

RETURN VALUE  TRUE if sent successfully, FALSE otherwise.

SIDE EFFECTS  None.

DEPENDENCIES  None.
===========================================================================*/
boolean xport_uart_sio_send_power_ctrl(char cmd)
{
  dsm_item_type *item_ptr;

  // LARGE item pool for datasize significantly larger
  // than SMALL item pool which is 128 bytes. 
  // Large item size : 1536 bytes
  // Small item size : 128 bytes    
  item_ptr = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL);

  if (item_ptr != NULL)
  {
    item_ptr->data_ptr[0] = cmd;
    item_ptr->used = 1;
    sio_transmit(xport_uart_ctx.sio_open_param.stream_id, item_ptr);

    return TRUE;
  }

  return FALSE;
}

/*===========================================================================
FUNCTION      xport_uart_wakeup_tx

DESCRIPTION   Send WAKE request and start response timer.

ARGUMENTS     None.

RETURN VALUE  None.

SIDE EFFECTS  None.

DEPENDENCIES  None.
===========================================================================*/
void xport_uart_wakeup_tx(void)
{
  /* we can recover if send fails, it'll be resent after the timer expires */
  xport_uart_sio_send_power_ctrl(UART_WAKE_CHAR);
  glink_timer_start(&xport_uart_ctx.wakeup_timer, UART_WAKEUP_TIMEOUT);
}

/*===========================================================================
FUNCTION      xport_uart_transmit_is_idle

DESCRIPTION   Checks if there is anything to transmit.

ARGUMENTS     None.

RETURN VALUE  TRUE if pending transmit.

SIDE EFFECTS  None.
===========================================================================*/
boolean xport_uart_transmit_is_idle(void)
{
  return smem_list_count(&xport_uart_ctx.tx_list) == 0 &&
    xport_uart_ctx.send_wake_ack == FALSE;
}

/*===========================================================================
FUNCTION      xport_uart_rx_high_cb

DESCRIPTION   Processes UART Rx high event.

ARGUMENTS     None.

RETURN VALUE  None.

SIDE EFFECTS  None.

DEPENDENCIES  None.
===========================================================================*/
void xport_uart_rx_high_cb(void)
{
  XPORT_UART_HDLC_LOG("Rx high event", 0);

  xport_uart_sm_process_event(XPORT_SIO_WAKE_RECEIVED);
}

/*===========================================================================
FUNCTION      xport_uart_power_down

DESCRIPTION   Processes UART Rx high event.

ARGUMENTS     None.

RETURN VALUE  None.

SIDE EFFECTS  None.

DEPENDENCIES  None.
===========================================================================*/
void xport_uart_power_down(void)
{
  sio_ioctl_param_type sio_ioctl_param;

  XPORT_UART_HDLC_LOG("Power down UART", 0);

  sio_ioctl_param.inactivity_ms = 1;

  sio_ioctl(xport_uart_ctx.sio_open_param.stream_id, 
            SIO_IOCTL_INACTIVITY_TIMER, &sio_ioctl_param);

  sio_ioctl_param.rx_high_event_func_ptr = xport_uart_rx_high_cb;

  sio_ioctl(xport_uart_ctx.sio_open_param.stream_id, 
            SIO_IOCTL_RX_HIGH_EVENT, &sio_ioctl_param);
}

/*===========================================================================
FUNCTION      xport_uart_sm_process_event

DESCRIPTION   Process internal SM event.

ARGUMENTS     event  - event to process.

RETURN VALUE  None.

SIDE EFFECTS  Changes in UART device power state may happen.
===========================================================================*/
void xport_uart_sm_process_event(xport_uart_sm_event_type event)
{
  boolean invoke_transmit = FALSE;
  
  glink_lock_acquire(xport_uart_ctx.sm_lock);
  
  XPORT_UART_HDLC_LOG("SM process event", event);
  
  switch (event)
  {
    case XPORT_SIO_TRANSMIT_START:
    
      if (xport_uart_ctx.tx_state == XPORT_UART_STATE_SLEEP)
      {
        if (xport_uart_ctx.rx_state == XPORT_UART_STATE_SLEEP)
        {
          /* disable inactivity timer */
          sio_ioctl_param_type sio_ioctl_param;
   
          sio_ioctl_param.inactivity_ms = 0;

          sio_ioctl(xport_uart_ctx.sio_open_param.stream_id, 
                    SIO_IOCTL_INACTIVITY_TIMER, &sio_ioctl_param);

          sio_ioctl(xport_uart_ctx.sio_open_param.stream_id, 
                    SIO_IOCTL_WAKEUP, NULL);
        }
        
        XPORT_UART_HDLC_LOG("Sending wakeup request", event);

        xport_uart_ctx.tx_state = XPORT_UART_STATE_WAKING_UP;
        xport_uart_ctx.wakeup_cnt++;
        xport_uart_wakeup_tx();
      }
      else if (xport_uart_ctx.tx_state == XPORT_UART_STATE_AWAKE)
      {
        glink_timer_stop(&xport_uart_ctx.idle_timer);
        invoke_transmit = TRUE;
      }
      break;
      
    case XPORT_SIO_TRANSMIT_IDLE:
    
      if (xport_uart_ctx.tx_state == XPORT_UART_STATE_AWAKE)
      {
        if (xport_uart_transmit_is_idle())
        {
          XPORT_UART_HDLC_LOG("Starting idle timer", UART_IDLE_TIMEOUT);
          glink_timer_start(&xport_uart_ctx.idle_timer, UART_IDLE_TIMEOUT);
        }
        else
        {
          invoke_transmit = TRUE;
        }
      }
      break;
      
    case XPORT_SIO_WAKE_TIMEOUT:
    
      if (xport_uart_ctx.tx_state == XPORT_UART_STATE_WAKING_UP)
      {
        xport_uart_ctx.wakeup_retry_cnt++;
        xport_uart_wakeup_tx();
      }
      break;
      
    case XPORT_SIO_WAKE_RECEIVED:
      {
        /* disable inactivity timer */
        sio_ioctl_param_type sio_ioctl_param;

        sio_ioctl_param.inactivity_ms = 0;

        sio_ioctl(xport_uart_ctx.sio_open_param.stream_id, 
                  SIO_IOCTL_INACTIVITY_TIMER, &sio_ioctl_param);
    
        xport_uart_ctx.rx_state = XPORT_UART_STATE_AWAKE;
        
        /* send wake ack */
        if (xport_uart_ctx.tx_state == XPORT_UART_STATE_AWAKE)
        {
          xport_uart_ctx.send_wake_ack = TRUE;
          invoke_transmit = TRUE;
        }
        else
        {
          xport_uart_sio_send_power_ctrl(UART_WAKE_ACK_CHAR);
        }
      }
      break;
      
    case XPORT_SIO_WAKE_ACK_RECEIVED:
    
      glink_timer_stop(&xport_uart_ctx.wakeup_timer);
        
      if (xport_uart_ctx.tx_state == XPORT_UART_STATE_WAKING_UP)
      {
        xport_uart_ctx.tx_state = XPORT_UART_STATE_AWAKE;
          
        /* send frame delimiting character to solicit Rx ACK response */
        xport_uart_sio_send_power_ctrl(HDLC_FRAME_DELIM_CHAR);
      }
      break;
      
    case XPORT_SIO_SLEEP_RECEIVED:
    
      xport_uart_ctx.rx_state = XPORT_UART_STATE_SLEEP;
        
      if (xport_uart_ctx.tx_state == XPORT_UART_STATE_SLEEP)
      {
        xport_uart_power_down();
      }
      break;
      
    case XPORT_SIO_IDLE_TIMEOUT:
    
      if (xport_uart_transmit_is_idle() &&
          xport_uart_ctx.tx_state == XPORT_UART_STATE_AWAKE)
      {
        XPORT_UART_HDLC_LOG("Sending SLEEP request", xport_uart_ctx.rx_state);

        /* Tx is still idle */
        if (xport_uart_sio_send_power_ctrl(UART_SLEEP_CHAR))
        {
          xport_uart_ctx.tx_state = XPORT_UART_STATE_SLEEP;
          xport_uart_ctx.escape_received = FALSE;
          xport_uart_ctx.ack_received = FALSE;
          xport_uart_ctx.sleep_cnt++;

          if (xport_uart_ctx.rx_state == XPORT_UART_STATE_SLEEP)
          {
            xport_uart_power_down();
          }
        }
        else
        {
          /* could not send sleep request, 
           * restart idle timer to send it later again */
          XPORT_UART_HDLC_LOG("Restarting idle timer", 0);
          glink_timer_start(&xport_uart_ctx.idle_timer, UART_IDLE_TIMEOUT);
        }
      }
      break;
      
    default:
      ASSERT(0); // should never end up here
      break;
  }
  
  glink_lock_release(xport_uart_ctx.sm_lock);
  
  if (invoke_transmit)
  {
    xport_uart_sio_transmit();
  }
}

/*===========================================================================
FUNCTION      xport_uart_sio_add_bytes

DESCRIPTION   Add bytes from provided buffer to DSM item.

ARGUMENTS     pdesc     - pointer to buffer descriptor.
item_ptr  - pointer to DSM item

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void xport_uart_sio_add_bytes(xport_uart_buff_desc_type* pdesc,
  dsm_item_type *item_ptr)
{
  /* if the last transmitted character was an escape
   * then put the escaped value now */
  if (xport_uart_ctx.next_tx_byte != HDLC_ESCAPE_CHAR && 
      item_ptr->used < UART_TX_LIMIT)
  {
    item_ptr->data_ptr[item_ptr->used++] = xport_uart_ctx.next_tx_byte;
    xport_uart_ctx.next_tx_byte = HDLC_ESCAPE_CHAR;
    pdesc->pcurr++;
  }

  while (pdesc->pcurr < pdesc->pend && item_ptr->used < UART_TX_LIMIT)
  {
    char c = *pdesc->pcurr;

    /* check if the current character needs escaping */
    switch (c)
    {
      case HDLC_FRAME_DELIM_CHAR:
      case HDLC_ESCAPE_CHAR:
      case UART_WAKE_CHAR:
      case UART_WAKE_ACK_CHAR:
      case UART_SLEEP_CHAR:
        item_ptr->data_ptr[item_ptr->used++] = HDLC_ESCAPE_CHAR;
        c ^= UART_ESCAPE_MASK;

        if (item_ptr->used >= UART_TX_LIMIT)
        {
          /* save escaped value for next trannsimmsion */
          xport_uart_ctx.next_tx_byte = c;
          return;
        }
        break;
      default:
        break;
    }

    item_ptr->data_ptr[item_ptr->used++] = c;
    pdesc->pcurr++;
  }
}

/*===========================================================================
FUNCTION      xport_uart_sio_transmit

DESCRIPTION   Transmit data if flow control allows.

ARGUMENTS     None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void xport_uart_sio_transmit()
{
  uint32 ack_avail;
  xport_uart_buff_desc_type* pdesc;
  dsm_item_type *item_ptr;
  
  if (xport_uart_ctx.tx_state != XPORT_UART_STATE_AWAKE)
  {
    XPORT_UART_HDLC_LOG("Tx is not awake, transmit aborted", xport_uart_ctx.tx_state);
    return;
  }
  
  ack_avail = glink_atomic_exchange_uint32(&xport_uart_ctx.ack_received, 0);
  
  if (!ack_avail)
  {
    return;
  }
  
  if (xport_uart_transmit_is_idle())
  {
    /* nothing to do, reinstate ACK */
    xport_uart_ctx.ack_received = TRUE;
    xport_uart_sm_process_event(XPORT_SIO_TRANSMIT_IDLE);

    return;
  }
  
  item_ptr = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL); // LARGE item pool for datasize significantly larger
                                                      // than SMALL item pool which is 128 bytes. 
                                                      // Large item size : 1536 bytes
                                                      // Small item size : 128 bytes    
                                                    
  if (item_ptr == NULL)
  {
    ASSERT(0);
    
    /* could not allocate DSM item, reinstate ACK */
    xport_uart_ctx.ack_received = TRUE;
    return;
  }
  
  if (xport_uart_ctx.send_wake_ack)
  {
    xport_uart_ctx.send_wake_ack = FALSE;
    item_ptr->data_ptr[0] = UART_WAKE_ACK_CHAR;
    item_ptr->used = 1;
  }
  
  pdesc = smem_list_first(&xport_uart_ctx.tx_list);
  
  if (pdesc)
  {
    if (pdesc->pcurr == NULL)
    {
      /* we are at the beginning of the frame,
       * put MUX byte first
       * (hard coded to 1 as no real MUXing is used now) */
      item_ptr->data_ptr[item_ptr->used++] = UART_DEFAULT_MUX_ID;
      pdesc->pcurr = pdesc->pkt.data;
      pdesc->pend = (char*)pdesc->pkt.data + pdesc->pkt.size;

      XPORT_UART_HDLC_LOG("Start trasmitting frame", pdesc->pkt.size);
    }
  
    xport_uart_sio_add_bytes(pdesc, item_ptr);
  
    if (pdesc->pcurr == pdesc->pend && 
        pdesc->pend != (char*)(&pdesc->checksum) + sizeof(pdesc->checksum))
    {
      /* add CRC now */
      pdesc->pcurr = (char*)(&pdesc->checksum);
      pdesc->pend = (char*)(&pdesc->checksum) + sizeof(pdesc->checksum);
    
      XPORT_UART_HDLC_LOG("Sending frame CRC", pdesc->checksum);

      xport_uart_sio_add_bytes(pdesc, item_ptr);
    }
  
    if (pdesc->pcurr == pdesc->pend &&
        item_ptr->used < UART_TX_LIMIT)
    {
      /* add end of frame */
      item_ptr->data_ptr[item_ptr->used++] = HDLC_FRAME_DELIM_CHAR;
    
      /* remove descriptor from the pending list */
      glink_lock_acquire(xport_uart_ctx.tx_list_lock);
      smem_list_delete(&xport_uart_ctx.tx_list, pdesc);
      glink_lock_release(xport_uart_ctx.tx_list_lock);

      XPORT_UART_HDLC_LOG("Finish trasmitting frame", pdesc->pkt.size);
    }
    else
    {
      /* keep buffer descriptor for next transmission */
      pdesc = NULL;
    }
  }
  else
  {
    /* if sending only WAKE ACK response then 
     * add frame delimiter to solicit Rx ACK */
    item_ptr->data_ptr[item_ptr->used++] = HDLC_FRAME_DELIM_CHAR;
  }

  sio_transmit(xport_uart_ctx.sio_open_param.stream_id, item_ptr);
  
  if (pdesc != NULL)
  {
    xport_uart_if.glink_core_if_ptr->rx_cmd_tx_done(&xport_uart_if, pdesc->cid, pdesc->iid);
    
    glink_lock_acquire(xport_uart_ctx.tx_free_list_lock);
    smem_list_append(&xport_uart_ctx.tx_free_list, pdesc);
    glink_lock_release(xport_uart_ctx.tx_free_list_lock);
    
    xport_uart_if.glink_core_if_ptr->rx_cmd_remote_rx_intent_put(
      &xport_uart_if, pdesc->cid, pdesc->iid, UART_RX_INTENT_DEFAULT_SIZE);
  }
}

/*===========================================================================
FUNCTION      xport_uart_negotiate_features

DESCRIPTION   Callback to verify feature set.

ARGUMENTS     if_ptr       - pointer to xport interface.
              version_ptr  - pointer to version descriptor
              features     - proposed feature set

RETURN VALUE  0.

SIDE EFFECTS  None.

DEPENDENCIES  None.
===========================================================================*/
uint32 xport_uart_negotiate_features(
  glink_transport_if_type *if_ptr,
  const glink_core_version_type *version_ptr,
  uint32 features)
{
  return 0;
}

/*===========================================================================
FUNCTION      xport_uart_init

DESCRIPTION   Initializes UART transport.
Must be called before any other operations are done.

ARGUMENTS     arg  - The argument.

RETURN VALUE  Returns error code.

SIDE EFFECTS  None.
===========================================================================*/
glink_err_type xport_uart_init(void *arg)
{
  glink_core_transport_cfg_type xport_uart_cfg = { 0 };
  glink_core_version_type xport_uart_version;

  /* Initialize context */
  xport_uart_ctx.tx_state = XPORT_UART_STATE_SLEEP;
  xport_uart_ctx.rx_state = XPORT_UART_STATE_SLEEP;

  xport_uart_ctx.sm_lock = glink_cs_create();
  xport_uart_ctx.ch_lock = glink_cs_create();
  xport_uart_ctx.tx_list_lock = glink_cs_create();
  xport_uart_ctx.rx_list_lock = glink_cs_create();
  xport_uart_ctx.tx_free_list_lock = glink_cs_create();
  xport_uart_ctx.rx_free_list_lock = glink_cs_create();
  xport_uart_ctx.rx_ready_list_lock = glink_cs_create();
  smem_list_init(&xport_uart_ctx.tx_list);
  smem_list_init(&xport_uart_ctx.rx_list);
  smem_list_init(&xport_uart_ctx.tx_free_list);
  smem_list_init(&xport_uart_ctx.rx_free_list);
  smem_list_init(&xport_uart_ctx.rx_ready_list);
  glink_timer_init(&xport_uart_ctx.wakeup_timer,
    (glink_timer_cb_fn)&xport_uart_sm_process_event,
    (void*)XPORT_SIO_WAKE_TIMEOUT);
  glink_timer_init(&xport_uart_ctx.idle_timer,
    (glink_timer_cb_fn)&xport_uart_sm_process_event,
    (void*)XPORT_SIO_IDLE_TIMEOUT);

  xport_uart_ctx.next_tx_byte = HDLC_ESCAPE_CHAR;
  xport_uart_ctx.checksum_seed = UART_DEFAULT_MUX_ID; //use as scratch buffer
  xport_uart_ctx.checksum_seed = crc_16_calc((byte*)&xport_uart_ctx.checksum_seed, 8);

  /* Setup port setting variable */
  xport_uart_ctx.sio_open_param.stream_id = SIO_NO_STREAM_ID;
  xport_uart_ctx.sio_open_param.stream_type = SIO_STREAM_RXTX;
  xport_uart_ctx.sio_open_param.stream_mode = SIO_GENERIC_MODE;
  xport_uart_ctx.sio_open_param.rx_queue = NULL;
  xport_uart_ctx.sio_open_param.tx_queue = &xport_uart_ctx.sio_wmq;
  xport_uart_ctx.sio_open_param.rx_bitrate = SIO_BITRATE_115200;//SIO_BITRATE_4000000;
  xport_uart_ctx.sio_open_param.tx_bitrate = SIO_BITRATE_115200;//SIO_BITRATE_4000000;
  xport_uart_ctx.sio_open_param.port_id = SIO_PORT_UART_MAIN;
  xport_uart_ctx.sio_open_param.tail_char_used = FALSE;
  xport_uart_ctx.sio_open_param.tail_char = '\0';
  xport_uart_ctx.sio_open_param.rx_func_ptr = xport_uart_sio_process_rx;
  xport_uart_ctx.sio_open_param.tx_flow = SIO_FCTL_OFF;
  xport_uart_ctx.sio_open_param.rx_flow = SIO_FCTL_OFF;

  /* Setup queue */
  xport_uart_ctx.sio_wmq.current_cnt = 0;
  xport_uart_ctx.sio_wmq.dont_exceed_cnt = 10000;
  xport_uart_ctx.sio_wmq.hi_watermark = 8000;
  xport_uart_ctx.sio_wmq.lo_watermark = 3000;
  xport_uart_ctx.sio_wmq.hiwater_func_ptr = NULL;
  xport_uart_ctx.sio_wmq.lowater_func_ptr = NULL;

  (void)q_init(&xport_uart_ctx.sio_tx_dsm_q);
  xport_uart_ctx.sio_wmq.q_ptr = &xport_uart_ctx.sio_tx_dsm_q;

  dsm_queue_init(&xport_uart_ctx.sio_wmq, /* watermark queue */
    10000, /* dont_exceed_cnt */
    &xport_uart_ctx.sio_tx_dsm_q  /* queue */
    );

  /* Initialize GLink transport interface */
  xport_uart_if.tx_cmd_version = &xport_uart_tx_cmd_version;
  xport_uart_if.tx_cmd_version_ack = &xport_uart_tx_cmd_version_ack;
  xport_uart_if.set_version = &xport_uart_set_version;
  xport_uart_if.tx_cmd_ch_open = &xport_uart_tx_cmd_ch_open;
  xport_uart_if.tx_cmd_ch_close = &xport_uart_tx_cmd_ch_close;
  xport_uart_if.tx_cmd_ch_remote_open_ack = &xport_uart_tx_cmd_ch_remote_open_ack;
  xport_uart_if.tx_cmd_ch_remote_close_ack = &xport_uart_tx_cmd_ch_remote_close_ack;
  xport_uart_if.tx_cmd_local_rx_intent = &xport_uart_tx_cmd_local_rx_intent;
  xport_uart_if.tx_cmd_local_rx_done = &xport_uart_tx_cmd_local_rx_done;
  xport_uart_if.tx = &xport_uart_tx;

  /* Setup GLink configuration */
  xport_uart_cfg.name = "UART";
  xport_uart_cfg.remote_ss = "wcnss";
  xport_uart_cfg.version = &xport_uart_version;
  xport_uart_cfg.version_count = 1;
  xport_uart_cfg.max_cid = 1;
  xport_uart_cfg.max_iid = UART_RX_INTENT_DEFAULT_NUM * 2;

  xport_uart_version.version = 1;
  xport_uart_version.features = 0;
  xport_uart_version.negotiate_features = &xport_uart_negotiate_features;


  if (glink_core_register_transport(&xport_uart_if, &xport_uart_cfg) !=
      GLINK_STATUS_SUCCESS)
  {
    /* Registration failed, clean up. */
    return GLINK_STATUS_FAILURE;
  }

  xport_uart_if.glink_core_if_ptr->link_up(&xport_uart_if);

  return GLINK_STATUS_SUCCESS;
}
