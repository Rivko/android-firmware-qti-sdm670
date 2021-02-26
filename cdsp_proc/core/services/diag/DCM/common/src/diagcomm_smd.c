/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Diagnostic Communications via SMD

General Description
  The routines in this file handle the communications layer for the
  diagnostic subsystem when communicating via SMD.

Copyright (c) 2001-2017 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                               Edit History

 $Header: //components/rel/core.qdsp6/2.1.c4/services/diag/DCM/common/src/diagcomm_smd.c#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/21/17   ph      Tag DIAG ID header to all the diag packets sent out.
05/12/16   ph      SMD references removal and code clean up.
10/14/15   nk      Removed un-used diag forwarding task variables from slave peripherals
12/22/15   is      Fix flow control race condition
09/08/15   ph      Consolidated Buffering mode fixes
07/16/14   sr      Resolved KW errors
04/10/14   sr      Support for peripheral buffering mode
01/30/14   ph      Removed the usage of macro DIAG_TASK_HANDLE().
11/27/13   ph      Updated MSG 1.0 API references to MSG.2.0.
09/27/13   xy      Removed code under FEATURE_WINCE
02/11/13   sr      Mainling Diag central routing
10/07/11   is      Support sending log mask per equipment id
09/19/11   is      Optimized sending F3 "set mask" requests from Master Diag
09/12/11   is      Master Diag forwards "set mask" requests via CTRL msgs
04/28/11   is      Add missing sleep vote
04/28/11   is      Fix compilation warnings
03/29/11   sg      Central Routing changes for Q6
03/25/11   is      Fix compilation errors for modem-only configuration
03/04/11   is      Support for Diag over SMD-Lite
12/10/10   mad     cleanup: Got rid of init_done in diagcomm_smd_open()
12/08/10   mad     do not open SMD port if already open: do not reset
                   diagcomm_smd_connected
10/20/10   is      Resolve compilation issue in Apps only Diag
09/28/10   mad     Fixed compile error regarding  DIAGCOMM_SMD_1 and DIAGCOMM_SMD_2
09/14/10   is      New control channel and central routing features
08/10/10   sg      Close SMD Port when the SIO port on apps is closed
09/13/10   is      Add flow control count and DSM chaining count to Diag Health
08/25/10   sg      Fixed compiler warnings
05/25/10   JV      New task on master to process slave traffic
05/13/10   JV      Merge functionality in diagcomm_smd.c and diagcomm_smd2.c
                   in a single file
10/29/09   JV      Specify DSM_DIAG_SIO_TX_ITEM_POOL as pool ID for slave last
                   processor.
10/29/09   JV      Pass the DSM pool ID to SMD
09/23/09   JV      To initialize the smd_tx_wmq, use the sizes and counts from the
                   SIO_TX pool. This is the DSM pool it uses.
09/23/09   JV      Do not initialize the SMD_RX and SMD_TX pools on the slave
                   last.
08/11/09   mad     flow-control is now independent of port open/close status
08/05/09   JV      Removed the CUST_H featurization around the inclusion of
                   customer.h.
07/31/09   JV      Merged Q6 diag code back to mainline
07/23/09   JV      Removed inclusion of task.h
06/23/09   JV      Included diagdsm_v.h instead of diagdsm.h. Featurized
                   inclusion of customer.h and target.h for wm.
05/28/09   mad     Mainlined item-level flow-control for SMD_TX pool
05/12/09   JV      Introduced the OS abstraction layer.
05/07/09   vk      changed includes from external to internal headers
04/10/09   mad     modified flow-control for SMD_TX pool, added SMD2 channel
03/20/09   mad     Changed DSM_MEM_LEVEL_MANY for SMD_TX flow-control,to keep
                   up with change in MANY level defined in diagdsmi_wce.h
02/19/09   mad     Inbound traffic from SMD is now processed in diag
                   task context, by handling DIAG_TX_SLAVE_SIG signal.
10/30/08   mad     Introduced flow-control for DSM_DIAG_SMD_TX_ITEM_POOL, at
                   item-level granularity. Took out watermark Q method of
                   flow-control.
08/14/08   sj      Added flow control function for higher threshold
05/17/07   jc      Proxy by tv.  Revise low water mark to 5 * DSM_DIAG_SMD_TX_ITEM_SIZ
12/15/06   as      Fixed compiler warnings.
11/21/06   as      Modified code to use diag internal features.
01/04/06   ptm     Change sio port name for SMD.
07/20/05   pjb     Changes to support DSM 3
06/15/05   cr      Fix CR#: 66743 - Surf crashes when diag is changed from
                   USB to UART1. Don't dequeue dsm_item or call sio_transmit
                   unless diagcomm is connected.
06/01/05   as      Added data transfer mode and transfer flow type to sio
                   open params under FEATURE_SMD.
05/17/05   as      Added Dual processor Diag support.
04/14/05   adm     Added FEATURE SMD check along with FEATURE_SMEM_DS
03/04/05    cr     Delete calls to dsm_reg_mem_event_cb because that was
                   completely breaking outbound flow control by leaving null
                   function pointer in diagcomm start/stop callback pointers
                   leading to DSM item exhausting and QXDM disconnect.
02/17/05   as      Removed async character check for APPS processor.
11/22/04    cr     Only compile diagcomm_smd for modem side, multi-processor
                   build.
10/28/03   as      Changes to fix errors when compiling with RVCT2.0
09/29/03   dna     Get T_QUASAR to compile and work again for MSM6500
08/14/03   vsk     DSM 2.0 changes
04/10/03    wx     Move qcdmg and to_data_mode functions from RDM to here.
05/08/02   lad     Moved dsm_simple_enqueue_isr() call above diagcomm_sio_cb
                   to enqueue data prior to potential context switch in cb.
02/27/02   dwp     Additional changes to use new DSM interface functions for
                   registering/handling memory events.
02/19/02   lad     Made changes to use new DSM interface functions for
                   registering/handling memory events.
01/21/02   as      Changed nv_sio_baud_type to nv_sio_baudrate_type
11/06/01   abp     Added diagcomm_flush_tx() to flush SIO transmit channel
10/30/01   abp     Added diagcomm_sio_close_to_open_cb().
                   In diagcomm_sio_close_done_cb() callback function ptr is
                   checked for NULL.
                   In diagcomm_open() added mechanism to close connection
                   before opening a new connection
10/09/01   ttl     Added second parameter to diagcomm_sio_tx_start_cb() and
                   diagcomm_sio_tx_stop_cb() due the DSM memory pool registration
                   changes.
09/17/01   jal     Fixed diagcomm_close() to notify RDM of completion
                   properly
05/07/01   lad     Cleaned up file.
04/17/01   dwp     Initialize diagcomm_smd_open_params.port_id to NULL. RDM
                   will instruct DIAG as to what port to open.
04/06/01   lad     Introduced types for callback function pointers.
                   Cleaned up FEATURE_RUNTIME_DEVMAP support.
                   Updated watermark settings (this may be changed to save RAM).
02/23/01   lad     Created file.

===========================================================================*/

#include "comdef.h"

#include "customer.h"
#include "target.h" /* This is prob. not needed even on AMSS */


#include "diag_v.h"
#include "diagbuf_v.h"
#include "diagcmd.h"
#include "diagcomm_v.h"
#include "diagi_v.h"
#include "diagpkt.h"
#include "diagtune.h"
#include "msg.h"
#include "diagcomm_io.h"
#include "diag_mode_v.h"
#include "diagdiag_v.h"
#if defined (FEATURE_DIAG_NV)
#include "diagnv_v.h"
#include "nv.h"
#endif

#include "osal.h"

#if defined (DIAG_QDSP6_APPS_PROC)
#include"diagstub.h"
#else
#include "assert.h"
#endif


extern osal_tcb_t diag_tcb;
/* -------------------------------------------------------------------------
** Definitions and Declarataions
** ------------------------------------------------------------------------- */
extern diagcomm_io_conn_type diagcomm_io_conn[NUM_PORT_TYPES][NUM_REMOTE_PORTS];      /* From diagcomm_cfg.c */
extern diagcomm_io_conn_type diagcomm_io_ctrl_conn[NUM_PORT_TYPES][NUM_REMOTE_PORTS]; /* From diagcomm_cfg.c */
extern diagcomm_io_conn_type diagcomm_io_cmd_conn[NUM_PORT_TYPES][NUM_REMOTE_PORTS]; /* From diagcomm_cfg.c */
extern uint8 diag_tag_hdr_size; /*Private id header size if support is present*/

extern osal_timer_cnt_t diagbuf_tx_sleep_time; /* From diagbuf.c */

extern void diag_ctrl_send_drain_status( void );
uint32 diag_buffered_bytes(uint8 stream_index, uint8 diag_id);

/*===========================================================================

FUNCTION DIAG_BUFFERED_BYTES

DESCRIPTION
Returns the number of buffered bytes in send buffer.

PARAMETERS
 stream_index : stream index for which the buffered bytes has to be checked

===========================================================================*/
uint32 diag_buffered_bytes(uint8 stream_index, uint8 diag_id)
{
  diag_tx_mode_type *tx_mode_ptr = NULL;

  tx_mode_ptr = diag_get_tx_mode_ptr(DIAG_INDEX_TO_ID(stream_index),diag_id);
  ASSERT(tx_mode_ptr!= NULL);

  if(tx_mode_ptr->buf_head >= tx_mode_ptr->buf_tail)
  return (tx_mode_ptr->buf_head - tx_mode_ptr->buf_tail);
  else
  return (tx_mode_ptr->buf_size - (tx_mode_ptr->buf_tail - tx_mode_ptr->buf_head));
}
/*===========================================================================

FUNCTION DIAG_DRAIN_BUFFER_CHECK (internal)

DESCRIPTION
  Checks whether Diag needs to set signal to drain the buffering buffer. Diag should
  signal draining if we're passed the threshold in threshold buffering mode
  or if we're in streaming mode.

PARAMETERS
  stream_id - Valid stream IDs are between DIAG_MIN_STREAM_ID and DIAG_MAX_STREAM_ID.
  tx_mode_ptr - tx_mode_ptr to retrieve the drain(send) buffer info.

RETURN VALUE
  TRUE - Diag can drain the pool now.
  FALSE - Buffering pools are not ready to be drained yet.

===========================================================================*/
boolean diag_drain_buffer_check( uint8 stream_id, diag_tx_mode_type *tx_mode_ptr )
  {
  uint8 stream_index = 0;

  if( (stream_id < DIAG_MIN_STREAM_ID) || (stream_id > DIAG_MAX_STREAM_ID) )
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Invalid stream_id (%d)", stream_id);
    return FALSE;
  }

  stream_index = DIAG_ID_TO_INDEX(stream_id);
  ASSERT(tx_mode_ptr!= NULL);

  if ( tx_mode_ptr->mode == DIAG_TX_MODE_BUFFERED_CIR )
  {
  /*Set drain as false if the data remaining to be drained is less than the lower threshold*/
    if ( tx_mode_ptr->buf_used <= tx_mode_ptr->buffered_few_bytes_mark )
    {
      tx_mode_ptr->drain = FALSE;
        diag_ctrl_send_drain_status();
    }
  }

  if ( tx_mode_ptr->mode == DIAG_TX_MODE_BUFFERED_THRESH )
  {
    if ( tx_mode_ptr->buf_used >= tx_mode_ptr->buffered_many_bytes_mark )
    {

      tx_mode_ptr->drain = TRUE;
    }
    else if ( tx_mode_ptr->drain &&
              (tx_mode_ptr->buf_used <= tx_mode_ptr->buffered_few_bytes_mark) )
    {
      tx_mode_ptr->drain = FALSE;
    }
  }

  if ( (tx_mode_ptr->mode == DIAG_TX_MODE_STREAMING) || tx_mode_ptr->drain )
  {
    if(TRUE == diagcomm_io_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAG_ID_TO_PORT(stream_id)].allow_flow)
    {
      return ( TRUE );
    }
  }

  return FALSE;

}

/*===========================================================================
FUNCTION DIAG_DRAIN_SEND_BUFFER (internal)

DESCRIPTION
  This function drains the buffering buffer per stream.

PARAMETERS
  stream_id - stream_id for buffering buffer to drain

RETURN VALUE
  None

===========================================================================*/
void diag_drain_send_buffer( uint8 stream_id, diag_tx_mode_type *tx_mode_ptr )
{
  diagcomm_io_tx_params_type tx_params;
  uint8 port_index;
  uint8 stream_index;
  diag_send_buf_header_type *diag_send_buf_header = NULL;

  if( (stream_id < DIAG_MIN_STREAM_ID) || (stream_id > DIAG_MAX_STREAM_ID) )
    return;

  stream_index = DIAG_ID_TO_INDEX(stream_id);
  port_index = DIAG_ID_TO_PORT(stream_id);

  ASSERT(tx_mode_ptr != NULL);

  diag_send_buf_header = (diag_send_buf_header_type *)(tx_mode_ptr->buf_ptr + tx_mode_ptr->buf_tail);

   /* Start drain buffering if buffering checks pass. */
  if( diag_drain_buffer_check(stream_id, tx_mode_ptr) && diagcomm_status() && (tx_mode_ptr->buf_used > (sizeof(diag_send_buf_header_type) + diag_tag_hdr_size))
    && diagcomm_io_conn[DIAGCOMM_PORT_REMOTE_PROC][port_index].allow_flow == TRUE)
  {
      if ((diag_send_buf_header->pattern != 0xDEAD) || (diag_send_buf_header->length + tx_mode_ptr->buf_tail > tx_mode_ptr->buf_size))
     {
        tx_mode_ptr->buf_tail = 0;
        tx_mode_ptr->buf_used = tx_mode_ptr->buf_head;
     }
     else
     {
        tx_params.buffer = tx_mode_ptr->buf_ptr + tx_mode_ptr->buf_tail + sizeof(diag_send_buf_header_type);
        tx_params.len = diag_send_buf_header->length;
        tx_params.tx_ptr = tx_mode_ptr;

        diagcomm_io_transmit(&diagcomm_io_conn[DIAGCOMM_PORT_REMOTE_PROC][port_index], &tx_params);
     }

     /* Set drain buffering signal if buffering checks passes and we are in buffering mode. */
     if (diag_drain_buffer_check(stream_id, tx_mode_ptr)) {
        if (stream_id == DIAG_STREAM_1) {
           diag_set_internal_sigs(DIAG_INT_DRAIN_BUFFERING_BUFFER_SIG);
        }
  }
  }
} /* diag_drain_send_buffer */
