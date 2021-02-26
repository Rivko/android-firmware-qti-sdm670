/*===========================================================================

              RPM native transport for Glink Driver Source File


  Copyright (c) 2014-2016, 2019, 2020 by Qualcomm Technologies Incorporated.  
  All rights reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/GLinkLib/xport_rpm/src/xport_rpm.c#5 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/27/16   vr      Moved reading to shared write index out of logging function
07/21/15   bc      Fix for incorrect memset usage
06/19/15   pp      Update to use Ulogs, support for stats and diag commands
04/09/15   an      Fixed initialization sequence of FIFO indices
04/06/15   an      Changes for enable/disable interrupt abstractions
03/23/15   an      Fixed compiler warnings
03/11/15   rs      Fixed compiler warnings
03/09/15   rs      Prevent RX index to be updated after SSR
01/28/15   rs      + Integrated RPM featurization removal 
                   + Support RPM transport teardown through SSR message 
01/21/15   bc      Removed sending priority request
12/29/14   bc      Added featurization for rpm and other processors
12/09/14   db      Added xport priority parameter in tx_open and tx_open_ack
12/09/14   bc      Update ISR/Poll and write word (4byte) alignment
11/25/14   an      Stop processing Rx FIFO when interrupt gets disabled.
10/20/14   an      Initial version.
===========================================================================*/


/*===========================================================================
                        INCLUDE FILES
===========================================================================*/

#include "glink.h"
#include "glink_core_if.h"
#include "glink_transport_if.h"
#include "glink_os_utils.h"
#include "glink_internal.h"
#include "glink_vector.h"
#include "xport_rpm_config.h"
#include "xport_rpm.h"

/*===========================================================================
                           MACRO DEFINITIONS
===========================================================================*/
#define XPORT_RPM_NAME "RPM"
#define XPORT_RPM_MSG_TOC_SIZE        0x100
#define XPORT_RPM_MIN(a, b)             ((a) < (b) ? (a) : (b))
#define XPORT_RPM_MAX(a, b)             ((a) > (b) ? (a) : (b))

#define XPORT_RPM_MAX_TOC_ENTRIES      \
  ((XPORT_RPM_MSG_TOC_SIZE - (XPORT_RPM_TOC_ENTRIES_IDX * sizeof(uint32))) / sizeof(xport_rpm_toc_entry_type))

#define XPORT_RPM_LOG(msg, remote_host, param)                                \
  if ((log_filter_cfg.xport_filter_mask &                                     \
       (1 << GLINK_XPORT_RPM_MASK_BIT)) != 0 )                                \
  {                                                                           \
    GLINK_LOG_EVENT_NO_FILTER(GLINK_EVENT_XPORT_INTERNAL, msg, XPORT_RPM_NAME,\
			       remote_host, (uint32)param);                               \
  }

#define MSGRAM_READ32(ctx, ind) (*(volatile uint32*)&(ctx)->rx_fifo[ind])
#define CHECK_INDEX_WRAP_AROUND(ind, size) \
          {if ((ind) >= (size)) ind = 0;}

/*===========================================================================
                              GLOBAL DATA DECLARATIONS
===========================================================================*/

extern const uint32 xport_rpm_config_num;
extern const char* xport_rpm_msg_ram;
extern const void* xport_rpm_msg_ram_toc;

xport_rpm_config_type* xport_rpm_get_config(uint32 ind);
uint32 xport_rpm_get_num_config(void);

/* RPM channel descriptor */
typedef struct _xport_rpm_ind_type
{
  uint32 read_ind;
  uint32 write_ind;
} xport_rpm_ind_type;

/* RPM transport context */
typedef struct _xport_rpm_ctx_type
{
  /* context structure should start from trasport interface */
  glink_transport_if_type xport_if; 
  const xport_rpm_config_type *pcfg;
  volatile xport_rpm_ind_type* tx_desc;
  volatile xport_rpm_ind_type* rx_desc;
  char* tx_fifo;
  char* rx_fifo;
  uint32 tx_fifo_size;
  uint32 rx_fifo_size;
  os_cs_type *tx_link_lock;
  os_cs_type *rx_link_lock;
  uint32 pkt_start_ind;
  uint32 pkt_size;
  boolean reset;
  boolean irq_mask;
} xport_rpm_ctx_type;

xport_rpm_ctx_type *xport_rpm_ctx = NULL;
glink_core_version_type xport_rpm_version;

/*===========================================================================
                    LOCAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION      xport_rpm_plain_pkt_provider
===========================================================================*/
/**

  Packet provider for virtual space that operates on MSG RAM FIFO.

  @param[in]  iovec   Pointer to the vector.
  @param[in]  offset  Offset within the vector.
  @param[out] size    Size of the provied buffer.

  @return     virtual address of the buffer.

  @sideeffects  None.
*/
/*=========================================================================*/
static void* xport_rpm_pkt_provider
(
  void*  iovec,
  size_t offset,
  size_t *size
)
{
  xport_rpm_ctx_type* ctx_ptr = (xport_rpm_ctx_type*)iovec;
  uint32 last;

  if (ctx_ptr == NULL || size == NULL || ctx_ptr->pkt_size <= offset)
  {
    return NULL;
  }

  last = ctx_ptr->rx_fifo_size - ctx_ptr->pkt_start_ind;
  
  if (offset >= last)
  {
    *size = ctx_ptr->pkt_size - offset;

    return &ctx_ptr->rx_fifo[offset - last];
  }

  *size = ctx_ptr->pkt_size <= last ? 
          ctx_ptr->pkt_size - offset : 
          last - offset;

  return &ctx_ptr->rx_fifo[offset + ctx_ptr->pkt_start_ind];
} 

volatile uint32 last_written_ind;

/*===========================================================================
FUNCTION      xport_rpm_send_event
===========================================================================*/
/**

  Updates shared write index and sends interrupt.

  @param[in]  ctx_ptr   Pointer to transport context.

  @return     None.

  @sideeffects  None.

  @dependencies It should be invoked within tx_link_lock protection.
*/
/*=========================================================================*/
static void xport_rpm_send_event
(
  xport_rpm_ctx_type *ctx_ptr
)
{
  /* read out the write index to initiate a bus transaction from MSG RAM */

  last_written_ind = ctx_ptr->tx_desc->write_ind;
  
  XPORT_RPM_LOG("Send event write ind", ctx_ptr->pcfg->remote_ss,last_written_ind);

  glink_os_send_interrupt((os_ipc_intr_type *)&ctx_ptr->pcfg->irq_out);
}

/*===========================================================================
FUNCTION      xport_rpm_write_msgram
===========================================================================*/
/**

  Copies data from local buffer into Tx FIFO located at MSG RAM 

  @param[in]  ctx_ptr   Pointer to transport context.
  @param[in]  write_ind Index to start wrting from.
  @param[in]  buffer    Buffer to copy from.
  @param[in]  size      Size of the data in the buffer.

  @return     New write index.

  @sideeffects  None.

  @dependencies It should be invoked within tx_link_lock protection.
  Caller should validate the availability space in fifo before calling.
*/
/*=========================================================================*/
static uint32 xport_rpm_write_msgram
(
  xport_rpm_ctx_type *ctx_ptr,
  uint32 write_ind,
  uint32 *buffer,
  uint32 size
)
{
  /* find size till fifo end */
  uint32 remaining = ctx_ptr->tx_fifo_size - write_ind;
  uint32 *buffer_end;
  volatile uint32* write_ptr = (volatile uint32*)&ctx_ptr->tx_fifo[write_ind];
  uint32  size_remainder = size % 4;
  uint32  size_rounddown = size - size_remainder; 

  if (remaining <= size_rounddown)
  {
    buffer_end = (uint32*)((char*)buffer + remaining);
 
    /* write in 32bit increments due to MSG RAM access requirement */
    while (buffer < buffer_end)
    {
      *write_ptr++ = *buffer++;
    }

    size_rounddown -= remaining;
    write_ptr = (volatile uint32*)&ctx_ptr->tx_fifo[0];
  }

  buffer_end = (uint32*)((char*)buffer + size_rounddown);

  /* write in 32bit increments due to MSG RAM access requirement */
  while (buffer < buffer_end)
  {
    *write_ptr++ = *buffer++;
  }

  if (size_remainder)
  {
    uint32 data = 0;
    uint32 *fifo_end = (uint32*)&ctx_ptr->tx_fifo[ctx_ptr->tx_fifo_size];

    glink_os_copy_mem(&data, buffer, size_remainder);
    *write_ptr++ = data;

    if (write_ptr >= fifo_end)
    {
      write_ptr = (volatile uint32*)&ctx_ptr->tx_fifo[0];
    }
  }

  return (uint32)((char*)write_ptr - &ctx_ptr->tx_fifo[0]);
}

/*===========================================================================
FUNCTION      xport_rpm_send_cmd
===========================================================================*/
/**

  Helper to send a single command.

  @param[in]  ctx_ptr           Pointer to transport context.
  @param[in]  cmd               buffer containing the command
  @param[in]  cmd_size          Size of command buffer.
  @param[in]  data              buffer containing the data
  @param[in]  data_size         Size of data buffer.

  @return     None.

  @dependencies None.
*/
/*=========================================================================*/
static glink_err_type xport_rpm_send_cmd
(
  xport_rpm_ctx_type  *ctx_ptr,
  uint32              *cmd,
  uint32               cmd_size,
  uint32              *data,
  uint32               data_size
)
{
  uint32 total_size = glink_os_add(cmd_size, data_size);
  uint32 reserve_size = glink_os_roundup64(total_size);
  uint32 write_ind, read_ind, avail_size; 

  glink_os_cs_acquire(ctx_ptr->tx_link_lock);

  /* Transport is in reset */
  if( ctx_ptr->reset )
  {
    glink_os_cs_release(ctx_ptr->tx_link_lock);
    return GLINK_STATUS_SUCCESS;
  }

  write_ind = ctx_ptr->tx_desc->write_ind;
  read_ind = ctx_ptr->tx_desc->read_ind;
  
  if ((!glink_os_fifo_index_check(ctx_ptr->tx_fifo_size, write_ind, 8)) ||
      (!glink_os_fifo_index_check(ctx_ptr->tx_fifo_size, read_ind, 8)))
  {
    glink_os_cs_release(ctx_ptr->tx_link_lock);
    OS_LOG_ERROR(4, "Invalid TX indices write_ind=0x%X, read_ind=0x%X, ctx_ptr=0x%p",
                     write_ind, read_ind, ctx_ptr);
    return GLINK_STATUS_FAILURE;
  }
  
  avail_size = write_ind < read_ind ? read_ind - write_ind :
               ctx_ptr->tx_fifo_size - write_ind + read_ind;
  
  if (glink_os_add(reserve_size, sizeof(uint64)) > avail_size)
  {
    glink_os_cs_release(ctx_ptr->tx_link_lock);
    return GLINK_STATUS_OUT_OF_RESOURCES;
  }

  XPORT_RPM_LOG("send cmd", ctx_ptr->pcfg->remote_ss, cmd[0]);
  
  write_ind 
    = xport_rpm_write_msgram( ctx_ptr, write_ind,
                              cmd, cmd_size );

  if (data != NULL)
  {
    write_ind 
      = xport_rpm_write_msgram( ctx_ptr, write_ind,
                                data, data_size );
  }

  /* add alignment bytes to Tx FIFO */
  write_ind += (reserve_size - total_size) & (~3);

  if (write_ind >= ctx_ptr->tx_fifo_size)
  {
    write_ind -= ctx_ptr->tx_fifo_size;
  }

  ctx_ptr->tx_desc->write_ind = write_ind;

  xport_rpm_send_event(ctx_ptr);

  glink_os_cs_release(ctx_ptr->tx_link_lock);

  return GLINK_STATUS_SUCCESS;
}

/*===========================================================================
                    EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION      xport_rpm_tx_cmd_version
===========================================================================*/
/**

  Transmit a version command for local negotiation - 
  expected response is to be delivered via glink_rx_cmd_version_ack().

  @param[in]  if_ptr    Pointer to transport interface instance.
  @param[in]  version   Version to be sent.
  @param[in]  feature   Features to be sent.

  @return     None.

  @sideeffects  None.
*/
/*=========================================================================*/
void xport_rpm_tx_cmd_version
(
  glink_transport_if_type *if_ptr,
  uint32 version,
  uint32 features
)
{
  xport_rpm_ctx_type *ctx_ptr = (xport_rpm_ctx_type *)if_ptr;
  uint32 cmd[2];

  cmd[0] = XPORT_RPM_SET_CMD_ID(XPORT_RPM_CMD_VERSION_REQ);
  cmd[0] |= XPORT_RPM_SET_VERSION(version);
  cmd[1] = features;

  if(xport_rpm_send_cmd(ctx_ptr, &cmd[0], sizeof(cmd), NULL, 0) != GLINK_STATUS_SUCCESS)
  {
    ERR_FATAL("send Version failed", 0, 0, 0);
  }
}

/*===========================================================================
FUNCTION      xport_rpm_tx_cmd_version_ack
===========================================================================*/
/**

  Transmit a version ack for remote negotiation.

  @param[in]  if_ptr    Pointer to transport interface instance.
  @param[in]  version   Version to be sent.
  @param[in]  feature   Features to be sent.

  @return     None.

  @sideeffects  None.
*/
/*=========================================================================*/
void xport_rpm_tx_cmd_version_ack
(
  glink_transport_if_type *if_ptr,
  uint32 version,
  uint32 features
)
{
  xport_rpm_ctx_type *ctx_ptr = (xport_rpm_ctx_type *)if_ptr;
  uint32 cmd[2];

  cmd[0] = XPORT_RPM_SET_CMD_ID(XPORT_RPM_CMD_VERSION_ACK);
  cmd[0] |= XPORT_RPM_SET_VERSION(version);
  cmd[1] = features;

  if (xport_rpm_send_cmd(ctx_ptr, &cmd[0], sizeof(cmd), NULL, 0) != GLINK_STATUS_SUCCESS)
  {
    ERR_FATAL("send Version Ack failed", 0, 0, 0);
  }
}

/*===========================================================================
FUNCTION      xport_rpm_set_version
===========================================================================*/
/**

  Signals that negotiation is complete and the transport can now 
  do version-specific initialization.

  @param[in]  if_ptr    Pointer to transport interface instance.
  @param[in]  version   Version to be used.
  @param[in]  feature   Features to be used.

  @return     Capabilities.

  @sideeffects  None.
*/
/*=========================================================================*/
uint32 xport_rpm_set_version
(
  glink_transport_if_type *if_ptr,
  uint32 version,
  uint32 features
)
{
  return GLINK_CAPABILITY_SIG_SUPPORT | GLINK_CAPABILITY_INTENTLESS;
}

/*===========================================================================
FUNCTION      xport_rpm_tx_cmd_ch_open
===========================================================================*/
/**

  Sends the open command - expected response is to be delivered 
  via glink_rx_cmd_ch_open_ack().

  @param[in]  if_ptr   Pointer to transport interface instance.
  @param[in]  lcid     Local channel ID.
  @param[in]  name     Channel name.
  @param[in]  prio     Requested xport priority by channel.

  @return     Returns error code.

  @sideeffects  None.
*/
/*=========================================================================*/
glink_err_type xport_rpm_tx_cmd_ch_open
(
  glink_transport_if_type *if_ptr,
  uint32 lcid,
  const char *name,
  glink_xport_priority prio
)
{
  xport_rpm_ctx_type *ctx_ptr = (xport_rpm_ctx_type *)if_ptr;
  uint32 cmd[2];

  cmd[0] = XPORT_RPM_SET_CMD_ID(XPORT_RPM_CMD_OPEN_CHANNEL);
  cmd[0] |= XPORT_RPM_SET_CHANNEL_ID(lcid);
  cmd[1] =  (uint32)strlen(name) + 1;

  return xport_rpm_send_cmd(ctx_ptr, &cmd[0], sizeof(cmd), (void*)name, cmd[1]);
}

/*===========================================================================
FUNCTION      xport_rpm_tx_cmd_ch_close
===========================================================================*/
/**

  Sends the close command - expected response is to be delivered 
  via glink_rx_cmd_ch_close_ack().

  @param[in]  if_ptr   Pointer to transport interface instance.
  @param[in]  lcid     Local channel ID.

  @return     Returns error code.

  @sideeffects  None.
*/
/*=========================================================================*/
glink_err_type xport_rpm_tx_cmd_ch_close
(
  glink_transport_if_type *if_ptr,
  uint32 lcid
)
{
  xport_rpm_ctx_type *ctx_ptr = (xport_rpm_ctx_type *)if_ptr;
  uint32 cmd;

  cmd = XPORT_RPM_SET_CMD_ID(XPORT_RPM_CMD_CLOSE_CHANNEL);
  cmd |= XPORT_RPM_SET_CHANNEL_ID(lcid);

  return xport_rpm_send_cmd(ctx_ptr, &cmd, sizeof(cmd), NULL, 0);
}

/*===========================================================================
FUNCTION      xport_rpm_tx_cmd_ch_remote_open_ack
===========================================================================*/
/**

  Sends the remote open ACK command.

  @param[in]  if_ptr   Pointer to transport interface instance.
  @param[in]  rcid     Remote channel ID.
  @param[in]  prio     send negotiated xport priority to remote side.

  @return     None.

  @sideeffects  None.
*/
/*=========================================================================*/
void xport_rpm_tx_cmd_ch_remote_open_ack
(
  glink_transport_if_type *if_ptr,
  uint32 rcid,
  glink_xport_priority prio
)
{
  xport_rpm_ctx_type *ctx_ptr = (xport_rpm_ctx_type *)if_ptr;
  uint32 cmd;

  cmd = XPORT_RPM_SET_CMD_ID(XPORT_RPM_CMD_OPEN_CHANNEL_ACK);
  cmd |= XPORT_RPM_SET_CHANNEL_ID(rcid);

  if (xport_rpm_send_cmd(ctx_ptr, &cmd, sizeof(cmd), NULL, 0) != GLINK_STATUS_SUCCESS)
  {
    ERR_FATAL("send Open Ack failed", 0, 0, 0);
  }
}

/*===========================================================================
FUNCTION      xport_rpm_tx_cmd_ch_remote_close_ack
===========================================================================*/
/**

  Sends the remote close ACK command.

  @param[in]  if_ptr   Pointer to transport interface instance.
  @param[in]  rcid     Remote channel ID.

  @return     None.

  @sideeffects  None.
*/
/*=========================================================================*/
void xport_rpm_tx_cmd_ch_remote_close_ack
(
  glink_transport_if_type *if_ptr,
  uint32 rcid
)
{
  xport_rpm_ctx_type *ctx_ptr = (xport_rpm_ctx_type *)if_ptr;
  uint32 cmd;

  cmd = XPORT_RPM_SET_CMD_ID(XPORT_RPM_CMD_CLOSE_CHANNEL_ACK);
  cmd |= XPORT_RPM_SET_CHANNEL_ID(rcid);

  if (xport_rpm_send_cmd(ctx_ptr, &cmd, sizeof(cmd), NULL, 0) != GLINK_STATUS_SUCCESS)
  {
    ERR_FATAL("send Close Ack failed", 0, 0, 0);
  }
}

/*===========================================================================
FUNCTION      xport_rpm_tx
===========================================================================*/
/**

  Send a data packet or a fragment of it.

  @param[in]  if_ptr   Pointer to transport interface instance.
  @param[in]  lcid     Local channel ID.
  @param[in]  pctx     Packet TX context.

  @return     Returns error code.

  @sideeffects  None.
*/
/*=========================================================================*/
glink_err_type xport_rpm_tx
(
  glink_transport_if_type *if_ptr,
  uint32 lcid,
  glink_core_tx_pkt_type *pctx
)
{
  xport_rpm_ctx_type *ctx_ptr = (xport_rpm_ctx_type *)if_ptr;
  uint32 cmd[4];

  cmd[0] = XPORT_RPM_SET_CMD_ID(XPORT_RPM_CMD_TX_DATA);
  cmd[0] |= XPORT_RPM_SET_CHANNEL_ID(lcid);
  cmd[1] = 0;
  cmd[2] = (uint32)pctx->size;
  cmd[3] = 0;

  pctx->size_remaining = 0;

  return xport_rpm_send_cmd(ctx_ptr, &cmd[0], sizeof(cmd), (void*)pctx->data, (uint32)pctx->size);
}

/*===========================================================================
FUNCTION      xport_rpm_tx_cmd_set_sigs
===========================================================================*/
/**

  Sends the local channel signals as per the specified 32-bit mask.

  @param[in]  if_ptr    Pointer to transport interface instance.
  @param[in]  lcid      Local channel ID.
  @param[in]  sigs      Signal mask. 

  @return     Returns error code.

  @sideeffects  None.
*/
/*=========================================================================*/
glink_err_type xport_rpm_tx_cmd_set_sigs
(
  glink_transport_if_type *if_ptr,
  uint32 lcid,
  uint32 sigs
)
{
  xport_rpm_ctx_type *ctx_ptr = (xport_rpm_ctx_type *)if_ptr;
  uint32 cmd[2];

  cmd[0] = XPORT_RPM_SET_CMD_ID(XPORT_RPM_CMD_TX_SIGNALS);
  cmd[0] |= XPORT_RPM_SET_CHANNEL_ID(lcid);
  cmd[1] = sigs;

  return xport_rpm_send_cmd(ctx_ptr, &cmd[0], sizeof(cmd), NULL, 0);
}

/*===========================================================================
FUNCTION      xport_rpm_negotiate_features
===========================================================================*/
/**

  Callback to verify feature set.

  @param[in]  if_ptr        Pointer to transport interface.
  @param[in]  version_ptr   Pointer to version descriptor.
  @param[in]  features      Proposed feature set.

  @return     0.

  @sideeffects  None.

  @dependencies None.
*/
/*=========================================================================*/
uint32 xport_rpm_negotiate_features(
  glink_transport_if_type *if_ptr,
  const glink_core_version_type *version_ptr,
  uint32 features)
{
  return 0;
}

/*===========================================================================
FUNCTION      xport_rpm_isr
===========================================================================*/
/**

  ISR of RPM transport.

  @param[in]  ctx_ptr   Pointer to transport context.

  @return     Returns error code.

  @sideeffects  None.

  @dependencies None.
*/
/*=========================================================================*/
glink_err_type xport_rpm_isr( xport_rpm_ctx_type *ctx_ptr )
{
  uint32 write_ind, read_ind;
  boolean stop_processing = FALSE;

  if (ctx_ptr->reset == TRUE)
  {
    /* reset flag has been set after SSR, notify link up */
    ctx_ptr->reset = FALSE;
    ctx_ptr->xport_if.glink_core_if_ptr->link_up((glink_transport_if_type *)ctx_ptr);
  }
  
  glink_os_cs_acquire(ctx_ptr->rx_link_lock);

  /* Process pending commands and data */
  write_ind = ctx_ptr->rx_desc->write_ind;
  read_ind = ctx_ptr->rx_desc->read_ind;

  if ((!glink_os_fifo_index_check(ctx_ptr->rx_fifo_size, write_ind, 8)) ||
      (!glink_os_fifo_index_check(ctx_ptr->rx_fifo_size, read_ind, 8)))
  {
    glink_os_cs_release(ctx_ptr->rx_link_lock);
    OS_LOG_ERROR(4, "Invalid RX indices write_ind=0x%X, read_ind=0x%X, ctx_ptr=0x%p",
                     write_ind, read_ind, ctx_ptr);
    return GLINK_STATUS_FAILURE;
  }

  XPORT_RPM_LOG("RPM ISR write ind", ctx_ptr->pcfg->remote_ss, write_ind);
  XPORT_RPM_LOG("RPM ISR read ind", ctx_ptr->pcfg->remote_ss, read_ind);

  while (write_ind != read_ind && !stop_processing)
  {
    uint32 cmd = MSGRAM_READ32(ctx_ptr, read_ind);
    uint32 cid = XPORT_RPM_GET_CHANNEL_ID(cmd); // most commands have channel ID
    uint32 cmd_arg;

    /* it can't wrap aroud here so just inceremt the index */
    read_ind += sizeof(cmd);

    XPORT_RPM_LOG("Cmd Rx", ctx_ptr->pcfg->remote_ss, cmd);

    switch (XPORT_RPM_GET_CMD_ID(cmd))
    {
      case XPORT_RPM_CMD_VERSION_REQ:

        cmd_arg = MSGRAM_READ32(ctx_ptr, read_ind);
        
        /* no nned to inceremt read_ind here since it will be rouned up */

        ctx_ptr->xport_if.glink_core_if_ptr->rx_cmd_version(
          (glink_transport_if_type *)ctx_ptr,
          XPORT_RPM_GET_VERSION(cmd), cmd_arg);
        break;

      case XPORT_RPM_CMD_VERSION_ACK:

        cmd_arg = MSGRAM_READ32(ctx_ptr, read_ind);

        /* no nned to inceremt read_ind here since it will be rouned up */

        ctx_ptr->xport_if.glink_core_if_ptr->rx_cmd_version_ack(
          (glink_transport_if_type *)ctx_ptr,
          XPORT_RPM_GET_VERSION(cmd), cmd_arg);
        break;

      case XPORT_RPM_CMD_OPEN_CHANNEL:
        cmd_arg = MSGRAM_READ32(ctx_ptr, read_ind);
        cmd_arg = glink_os_roundup64(cmd_arg);

        read_ind += sizeof(cmd_arg);

        if (GLINK_CH_NAME_LEN % sizeof(uint64))
        {
          ERR_FATAL("Default channel name length(%d) is not supported, "
                    "it should be multiples of sizeof(uint64)", GLINK_CH_NAME_LEN, 0, 0);
        }

        /* channel name should fit into the FIFO */
        if (cmd_arg == 0 || cmd_arg >= ctx_ptr->rx_fifo_size)
        {
          ERR_FATAL("Invalid name length", cmd_arg, 0, 0);
        }
        else 
        {
          char  temp_string[GLINK_CH_NAME_LEN] = {0};
          uint32 num_copied_chars = 0;
          uint32  *string_ptr;

          string_ptr = ( uint32 * )&temp_string[0];
          while( ( num_copied_chars < cmd_arg ) && ( num_copied_chars < sizeof( temp_string ) ) )
          {
            CHECK_INDEX_WRAP_AROUND( read_ind, ctx_ptr->rx_fifo_size );
            *( string_ptr++ ) = MSGRAM_READ32( ctx_ptr, read_ind );

            num_copied_chars += sizeof( uint32 );
            read_ind += sizeof( uint32 );
          }

          /* add all the unread stuff */
          read_ind += cmd_arg - num_copied_chars;

          /* make sure the last character is NULL */
          temp_string[ sizeof( temp_string ) - 1 ] = 0;
         
          ctx_ptr->xport_if.glink_core_if_ptr->rx_cmd_ch_remote_open(
            (glink_transport_if_type *)ctx_ptr, cid, temp_string, GLINK_XPORT_RPM);
        }

        break;

      case XPORT_RPM_CMD_CLOSE_CHANNEL:

        /* no nned to inceremt read_ind here since it will be rouned up */
        ctx_ptr->xport_if.glink_core_if_ptr->rx_cmd_ch_remote_close(
          (glink_transport_if_type *)ctx_ptr, cid);

        break;

      case XPORT_RPM_CMD_OPEN_CHANNEL_ACK:

        /* no nned to inceremt read_ind here since it will be rouned up */
        ctx_ptr->xport_if.glink_core_if_ptr->rx_cmd_ch_open_ack(
            (glink_transport_if_type *)ctx_ptr, cid, GLINK_XPORT_RPM);

        break;

      case XPORT_RPM_CMD_CLOSE_CHANNEL_ACK:

        ctx_ptr->xport_if.glink_core_if_ptr->rx_cmd_ch_close_ack(
          (glink_transport_if_type *)ctx_ptr, cid);

        /* no nned to inceremt read_ind here since it will be rouned up */

        break;

      case XPORT_RPM_CMD_TX_DATA:
      {
        glink_rx_intent_type desc;
        memset( &desc, 0, sizeof( glink_rx_intent_type) );
        
        read_ind += sizeof(cmd_arg);

        CHECK_INDEX_WRAP_AROUND(read_ind, ctx_ptr->rx_fifo_size);

        cmd_arg = MSGRAM_READ32(ctx_ptr, read_ind);

        /* packet data should fit into the FIFO */
        if (cmd_arg >= ctx_ptr->rx_fifo_size)
        {
          ERR_FATAL("Invalid pkt length", cmd_arg, 0, 0);
        }

        read_ind += 2*sizeof(cmd_arg);

        CHECK_INDEX_WRAP_AROUND(read_ind, ctx_ptr->rx_fifo_size);

        ctx_ptr->pkt_start_ind = read_ind;
        ctx_ptr->pkt_size = cmd_arg;

        desc.size = cmd_arg;
        desc.used = cmd_arg;
        desc.pkt_sz = cmd_arg;
        desc.iovec = ctx_ptr;
        desc.vprovider = xport_rpm_pkt_provider;

        read_ind += cmd_arg;

        ctx_ptr->xport_if.glink_core_if_ptr->rx_put_pkt_ctx(
          (glink_transport_if_type *)ctx_ptr, cid, 
          &desc, TRUE);

        /* If interrupt was disabled then stop delivering messages */
        stop_processing = ctx_ptr->irq_mask;

        break;
      }

      case XPORT_RPM_CMD_TX_SIGNALS:

        cmd_arg = MSGRAM_READ32(ctx_ptr, read_ind);

        /* no nned to inceremt read_ind here since it will be rouned up */

        ctx_ptr->xport_if.glink_core_if_ptr->rx_cmd_remote_sigs(
          (glink_transport_if_type *)ctx_ptr,
          cid, cmd_arg);
        break;

      default:

        ERR_FATAL("Invalid cmd", cmd, 0, 0);
        break;
    }

    /* Update read index only if transport has not been reset  */
    if( !ctx_ptr->reset )
    {
      read_ind = glink_os_roundup64(read_ind);

      if (read_ind >= ctx_ptr->rx_fifo_size)
      {
        read_ind -= ctx_ptr->rx_fifo_size;
      }

      /* Update the shared read index */ 
      ctx_ptr->rx_desc->read_ind = read_ind;
    }
    else
    {
      stop_processing = TRUE;
    }
  }

  glink_os_cs_release(ctx_ptr->rx_link_lock);

  return GLINK_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION      xport_rpm_ssr
===========================================================================*/
/**

  Processes SSR event.

  @param[in]  if_ptr   Pointer to transport interface instance.

  @return     Returns error code.

  @sideeffects  None.
*/
/*=========================================================================*/
glink_err_type xport_rpm_ssr(glink_transport_if_type *if_ptr)
{
  xport_rpm_ctx_type *ctx_ptr = (xport_rpm_ctx_type *)if_ptr;

  glink_os_cs_acquire(ctx_ptr->tx_link_lock);
  glink_os_cs_acquire(ctx_ptr->rx_link_lock);
  ctx_ptr->tx_desc->write_ind = 0;
  ctx_ptr->tx_desc->read_ind = 0;
  ctx_ptr->rx_desc->write_ind = 0;
  ctx_ptr->rx_desc->read_ind = 0;
  ctx_ptr->reset = TRUE;
  glink_os_cs_release(ctx_ptr->rx_link_lock);
  glink_os_cs_release(ctx_ptr->tx_link_lock);

  return GLINK_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION      xport_rpm_ssr
===========================================================================*/
/**

  Enables or disables interrupts.

  @param[in]  if_ptr   Pointer to transport interface instance.
  @param[in]  mask     TRUE-mask, FALSE=unmask

  @return     Returns error code.

  @sideeffects  None.
*/
/*=========================================================================*/
glink_err_type xport_rpm_mask_interrupt(glink_transport_if_type *if_ptr, boolean mask)
{
  xport_rpm_ctx_type *ctx_ptr = (xport_rpm_ctx_type *)if_ptr;

  if (ctx_ptr->irq_mask != mask)
  {
    if (mask)
    { 
      glink_os_disable_interrupt(ctx_ptr->pcfg->irq_in);
    }
    else
    {
      glink_os_enable_interrupt(ctx_ptr->pcfg->irq_in, 
                                (os_isr_cb_fn)xport_rpm_isr, 
                                ctx_ptr);
    }
    
    ctx_ptr->irq_mask = mask;
  }

  return GLINK_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION      xport_rpm_is_toc_present
===========================================================================*/
/**

  Checks RPM MSG RAM for ToC presence.

  @return     TRUE if ToC is detected, FALSE otherwise.

  @sideeffects  None.
*/
/*=========================================================================*/
boolean xport_rpm_is_toc_present(void)
{
  uint32 *msg_ram_toc = (uint32*)xport_rpm_msg_ram_toc;

  return msg_ram_toc[XPORT_RPM_TOC_MAGIC_IDX] == XPORT_RPM_TOC_MAGIC;
}

/*===========================================================================
FUNCTION      xport_rpm_fifo_overlapped
===========================================================================*/
/**
  Checks if a fifo overlaps with other.

  @param[in]  part0_start    First fifo start.
  @param[in]  part0_end      First fifo end.
  @param[in]  part1_start    Second fifo start.
  @param[in]  part1_end      Second fifo start.

  @return     TRUE if fifo overlaps else FALSE.
*/
/*=========================================================================*/
static boolean xport_rpm_fifo_overlapped
(
  uint32 part0_start,
  uint32 part0_end,
  uint32 part1_start,
  uint32 part1_end
)
{
  uint32 max_start = XPORT_RPM_MAX(part0_start, part1_start);
  uint32 min_end   = XPORT_RPM_MIN(part0_end, part1_end);
 	 		 
  if (min_end >= max_start)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*===========================================================================
FUNCTION      xport_rpm_toc_entry_validate
===========================================================================*/
/**
  Verifies RPM TOC entries.

  @param[in]  entry                     Points to TOC entry.
  @param[in]  msg_ram_size              Size of MSG RAM excluding TOC.
  @param[in]  toc_entries_end           Points to the end of entries in TOC.

  @return     TRUE if TOC entries are valid, else FALSE.
*/
/*=========================================================================*/
static boolean xport_rpm_toc_entry_validate
(
  xport_rpm_toc_entry_type *entry,
  uint32                   msg_ram_size,
  char                     *toc_entries_end
)
{
  xport_rpm_toc_entry_type *other_entry;
  uint32                   fifo_end_offset;
  
  if(entry->fifo_size == 0)
  {
    return FALSE;	  
  }
	
  fifo_end_offset = glink_os_add(entry->fifo_offset, 
                          glink_os_add(sizeof(xport_rpm_ind_type), 
                               entry->fifo_size)) - 1;
	
  if ((entry->fifo_offset >  msg_ram_size)  ||
      (fifo_end_offset    >= msg_ram_size))
  {
    return FALSE;
  }

  for (other_entry = entry + 1; (char*)other_entry < toc_entries_end; other_entry++)
  {
    if ((other_entry->fifo_id   == entry->fifo_id) ||
        (other_entry->fifo_size == 0))
    {
      return FALSE;
    }
										
    if (xport_rpm_fifo_overlapped(entry->fifo_offset,
              fifo_end_offset,
              other_entry->fifo_offset,
              glink_os_add(other_entry->fifo_offset,
			      glink_os_add(sizeof(xport_rpm_ind_type),
				      other_entry->fifo_size)) - 1))
    {
      return FALSE;
    }
  }

  return TRUE;
}

/*===========================================================================
FUNCTION      xport_rpm_init
===========================================================================*/
/**

  Initializes RPM transport.
  Must be called before any other operations are done.

  @param[in]  arg   The argument.

  @return     Returns error code.

  @sideeffects  None.
*/
/*=========================================================================*/
glink_err_type xport_rpm_init(void *arg)
{
  uint32 ind;
  glink_core_transport_cfg_type xport_rpm_cfg = { 0 };
  volatile uint32 *msg_ram_toc = (uint32*)xport_rpm_msg_ram_toc;
  xport_rpm_toc_entry_type *entry;
  uint32 msg_ram_toc_num;
  char *toc_entries_end;
  uint32 msg_ram_size;
  char* msg_ram_toc_entry_start = (char*)&msg_ram_toc[XPORT_RPM_TOC_ENTRIES_IDX];

  if (xport_rpm_config_num == 0)
  {
    return GLINK_STATUS_SUCCESS;
  }

  if (!xport_rpm_is_toc_present())
  {
    /* switch to err fatal once RPM side is integrated */
    //ERR_FATAL("Invalid ToC cookie", msg_ram_toc[0], 0, 0);
    return GLINK_STATUS_FAILURE;
  }

  if(msg_ram_toc <= (uint32*)xport_rpm_msg_ram)
  {
    ERR_FATAL("TOC not within MSG RAM base=0x%08X, toc_base=0x%08X",
              xport_rpm_msg_ram, msg_ram_toc, 0);
    return GLINK_STATUS_FAILURE;
  }
  
  /* Size of MSG RAM excluding TOC */
  msg_ram_size = (char*)msg_ram_toc - xport_rpm_msg_ram;

  msg_ram_toc_num = msg_ram_toc[XPORT_RPM_TOC_LENGTH_IDX];

  if (msg_ram_toc_num == 0)
  {
    return GLINK_STATUS_SUCCESS;
  }

  if (msg_ram_toc_num > XPORT_RPM_MAX_TOC_ENTRIES)
  {
    ERR_FATAL("Invalid number of entries=0x%08X Max=0x%08X",
              msg_ram_toc_num, XPORT_RPM_MAX_TOC_ENTRIES, 0);
    return GLINK_STATUS_FAILURE;
  }

  xport_rpm_ctx = glink_os_calloc(sizeof(*xport_rpm_ctx)*xport_rpm_config_num);

  if (xport_rpm_ctx == NULL)
  {
    ERR_FATAL("Failed to allocate context", 0, 0, 0);
  }

  /* End of entries in TOC */
  toc_entries_end = msg_ram_toc_entry_start +
                       (msg_ram_toc_num * sizeof(xport_rpm_toc_entry_type));

  /* Initialize supported version and features */
  xport_rpm_version.version = 1;
  xport_rpm_version.features = 0;
  xport_rpm_version.negotiate_features = &xport_rpm_negotiate_features;

  /* Initialize edges */

  for (ind = 0; ind < xport_rpm_config_num; ind++)
  {
    xport_rpm_ctx[ind].pcfg = xport_rpm_get_config(ind);

    /* Find FIFOs in MSG RAM ToC */
    for (entry = (xport_rpm_toc_entry_type*)msg_ram_toc_entry_start;
         (char*)entry < toc_entries_end && 
           (xport_rpm_ctx[ind].tx_desc == NULL || xport_rpm_ctx[ind].rx_desc == NULL);
         entry++)
    {
      if (!xport_rpm_toc_entry_validate(entry, msg_ram_size, toc_entries_end))
      {
         ERR_FATAL("TOC entry validation failed! entry=%p, msg_ram_size=0x%08X, "
                  "toc_entries_end=%p.", entry, msg_ram_size, toc_entries_end);
         return GLINK_STATUS_FAILURE;
      }
	  
      if (entry->fifo_id == xport_rpm_ctx[ind].pcfg->tx_fifo_id)
      {
        xport_rpm_ctx[ind].tx_desc = (xport_rpm_ind_type*)&xport_rpm_msg_ram[entry->fifo_offset];
        xport_rpm_ctx[ind].tx_fifo = (char*)(xport_rpm_ctx[ind].tx_desc + 1);
        xport_rpm_ctx[ind].tx_fifo_size = entry->fifo_size;
      }
      else if (entry->fifo_id == xport_rpm_ctx[ind].pcfg->rx_fifo_id)
      {
        xport_rpm_ctx[ind].rx_desc =(xport_rpm_ind_type*)&xport_rpm_msg_ram[entry->fifo_offset];
        xport_rpm_ctx[ind].rx_fifo = (char*)(xport_rpm_ctx[ind].rx_desc + 1);
        xport_rpm_ctx[ind].rx_fifo_size = entry->fifo_size;
      }
    }

    if (xport_rpm_ctx[ind].tx_desc == NULL || xport_rpm_ctx[ind].rx_desc == NULL)
    {
      /* FIFOs not found in MSG RAM ToC. */
      xport_rpm_ctx[ind].pcfg = NULL;
      continue;
    }

    /* Rx read index should be cleared last */
    xport_rpm_ctx[ind].tx_desc->write_ind = 0;
    xport_rpm_ctx[ind].rx_desc->read_ind = 0;

    /* Initialize context */
    xport_rpm_ctx[ind].tx_link_lock = glink_os_cs_create();
    xport_rpm_ctx[ind].rx_link_lock = glink_os_cs_create();

    /* Initialize GLink transport interface */
    xport_rpm_ctx[ind].xport_if.tx_cmd_version = &xport_rpm_tx_cmd_version;
    xport_rpm_ctx[ind].xport_if.tx_cmd_version_ack = &xport_rpm_tx_cmd_version_ack;
    xport_rpm_ctx[ind].xport_if.set_version = &xport_rpm_set_version;
    xport_rpm_ctx[ind].xport_if.tx_cmd_ch_open = &xport_rpm_tx_cmd_ch_open;
    xport_rpm_ctx[ind].xport_if.tx_cmd_ch_close = &xport_rpm_tx_cmd_ch_close;
    xport_rpm_ctx[ind].xport_if.tx_cmd_ch_remote_open_ack = &xport_rpm_tx_cmd_ch_remote_open_ack;
    xport_rpm_ctx[ind].xport_if.tx_cmd_ch_remote_close_ack = &xport_rpm_tx_cmd_ch_remote_close_ack;
    xport_rpm_ctx[ind].xport_if.tx = &xport_rpm_tx;
    xport_rpm_ctx[ind].xport_if.tx_cmd_set_sigs = &xport_rpm_tx_cmd_set_sigs;
    xport_rpm_ctx[ind].xport_if.ssr = &xport_rpm_ssr;
    xport_rpm_ctx[ind].xport_if.mask_rx_irq = &xport_rpm_mask_interrupt;
    xport_rpm_ctx[ind].xport_if.poll = (poll_fn)&xport_rpm_isr;

    /* TODO: glink transport priority */
    xport_rpm_ctx[ind].xport_if.glink_priority = GLINK_XPORT_RPM;

    /* Setup GLink configuration */
    xport_rpm_cfg.name = XPORT_RPM_NAME;
    xport_rpm_cfg.remote_ss = xport_rpm_ctx[ind].pcfg->remote_ss;
    xport_rpm_cfg.version = &xport_rpm_version;
    xport_rpm_cfg.version_count = 1;
    xport_rpm_cfg.max_cid = 0xFF;
    xport_rpm_cfg.max_iid = 0;

    if (glink_core_register_transport(&xport_rpm_ctx[ind].xport_if, &xport_rpm_cfg) !=
        GLINK_STATUS_SUCCESS)
    {
      /* Registration failed, set index to invalid. */
      xport_rpm_ctx[ind].pcfg = NULL;
      continue;
    }
    
    if ( !glink_os_register_isr( xport_rpm_ctx[ind].pcfg->irq_in, 
                                (os_isr_cb_fn)xport_rpm_isr, 
                                &xport_rpm_ctx[ind]) )
    {
       /* ISR registration failed, set index to invalid.
        * It will never fail */ 
      xport_rpm_ctx[ind].pcfg = NULL;
      continue;
    }

    /* send link up notification */
    xport_rpm_ctx[ind].xport_if.glink_core_if_ptr->link_up((glink_transport_if_type *)&xport_rpm_ctx[ind].xport_if);

    /*  We need to loop and call transport poll API until we have processed
     *  feature negotiation with the remote side */
    while(xport_rpm_ctx[ind].xport_if.glink_core_priv->status != GLINK_XPORT_LINK_UP)
    {
      xport_rpm_isr(&xport_rpm_ctx[ind]);
    }
  }

  return GLINK_STATUS_SUCCESS;
}
