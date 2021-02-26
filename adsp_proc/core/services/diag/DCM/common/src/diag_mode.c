/*==========================================================================

              Diag support for buffering and streaming modes

General Description

Copyright (c) 2013-2017 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/

/*===========================================================================

                            Edit History

$Header: //components/rel/core.qdsp6/2.1/services/diag/DCM/common/src/diag_mode.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/25/17   ph      Separate macro to configure sensor PD buffer size.
05/05/17   ph      Configure buffering modes for user PD send buffers.
02/21/17   ph      Tag DIAG ID header to all the diag packets sent out.
01/04/17   sa      Update UserPD commit threshold in NRT mode
05/12/16   ph      SMD references removal and code clean up.
05/10/16   nk      Separate send buffers for CMD and DATA
02/19/16   ph      Buffering mode commands are valid only for Stream 1.
12/22/15   is      Fix flow control race condition
09/08/15   ph      Consolidated Buffering mode fixes
01/01/14   ph      Fixed inconsistent drain behavior in circular and threshold buffering.
11/01/13   is      Resolve buffering mode issues
04/08/15   ps      Diag over sockets and dsm removal
07/24/14   xy      Resolve 8994 Diag warnings with new LLVM 7.0.04 toolchain
04/10/14   sr      Support for peripheral buffering mode

===========================================================================*/

#include "diagdiag_v.h"
#include "diagi_v.h"      /* For diag_tx_mode_type */
#include "diag_mode_v.h"
#include "diag_cfg.h"     /* For DIAGBUF_COMMIT_THRESHOLD */
#include "diagtarget.h"   /* For diag_kick_watchdog() */
#include "eventi.h"
#include "diagbuf_v.h"  /* For diagbuf_ctrl_mpd_commit_threshold */
#include "diag_diagIDi.h"
/*-------------------------------------------------------------------------
                   Local function prototype
 ------------------------------------------------------------------------- */
void * diagcomm_tx_mode_config( void * req_pkt_ptr, uint16 pkt_len );
void diag_ctrl_send_drain_status( void );

/* -------------------------------------------------------------------------
** Definitions and Declarataions
** ------------------------------------------------------------------------- */
extern diagcomm_io_conn_type diagcomm_io_conn[NUM_PORT_TYPES][NUM_REMOTE_PORTS]; /* From diagcomm_io.c */
extern diagcomm_io_conn_type diagcomm_io_ctrl_conn[NUM_PORT_TYPES][NUM_REMOTE_PORTS];

extern osal_timer_cnt_t diagbuf_tx_sleep_time; /* From diagbuf.c */

/* Mode variable to track all PD modes and buffers including root PD */
volatile diag_tx_mode_type diag_tx_mode[DIAG_MAX_STREAM_ID][DIAG_SEND_BUF_CNT];

static const diagpkt_user_table_entry_type diagdiag_subsys_tbl_tx_mode[] =
{
  {DIAGDIAG_TX_MODE_CONFIG, DIAGDIAG_TX_MODE_CONFIG, diagcomm_tx_mode_config}
};

extern uint32 diag_max_send_buf_len; /*From diagcomm_io.c */
extern uint8 diagbuf_send_data[NUM_REMOTE_PORTS][DIAG_SEND_BUF_SIZE_DATA]; /*From diagbuf.c */
extern uint8 diagbuf_send_cmd[NUM_REMOTE_PORTS][DIAG_SEND_BUF_SIZE_CMD]; /*From diagbuf.c */
extern uint8 diagbuf_send_data_spd[DIAG_SEND_BUF_SIZE_DATA_SENSOR];
extern uint8 diagbuf_send_data_apd[DIAG_SEND_BUF_SIZE_DATA_AUDIO];


/*===========================================================================

FUNCTION DIAG_TX_MODE_INIT (internal)

DESCRIPTION
  Initializes global variable for keeping track of streaming mode vs buffering mode.

PARAMETERS
  None

RETURN VALUE
  None

===========================================================================*/
void diag_tx_mode_init( void )
{
  uint8 stream_index = 0;
  uint8 buf_index = 0;

  for (stream_index = 0; stream_index < DIAG_MAX_STREAM_ID; stream_index++)
  {
    for (buf_index = 0; buf_index < DIAG_SEND_BUF_CNT; buf_index++)
    {
      diag_tx_mode[stream_index][buf_index].prev_mode = (uint8)DIAG_TX_MODE_INVALID;
      diag_tx_mode[stream_index][buf_index].mode = (uint8)DIAG_TX_MODE_STREAMING;
      diag_tx_mode[stream_index][buf_index].drain = TRUE;

      if( (stream_index == DIAG_ID_TO_INDEX(DIAG_STREAM_1)) && (buf_index == DIAG_DIAGID_SENSOR_USER_IDX))
      {
        diag_tx_mode[stream_index][buf_index].buf_ptr = &diagbuf_send_data_spd[0];
        diag_tx_mode[stream_index][buf_index].buf_size = sizeof(diagbuf_send_data_spd);
      }
      else if( (stream_index == DIAG_ID_TO_INDEX(DIAG_STREAM_1)) && (buf_index == DIAG_DIAGID_AUDIO_USER_IDX))
      {
        diag_tx_mode[stream_index][buf_index].buf_ptr = &diagbuf_send_data_apd[0];
        diag_tx_mode[stream_index][buf_index].buf_size = sizeof(diagbuf_send_data_apd); 
      }
      else
      {
        diag_tx_mode[stream_index][buf_index].buf_ptr = &diagbuf_send_data[stream_index][0];
        diag_tx_mode[stream_index][buf_index].buf_size = sizeof(diagbuf_send_data[stream_index]);
      }

      diag_tx_mode[stream_index][buf_index].buf_head = 0;
      diag_tx_mode[stream_index][buf_index].buf_tail = 0;
      diag_tx_mode[stream_index][buf_index].buf_used = 0;
      diag_tx_mode[stream_index][buf_index].buf_wrap = FALSE;

   // Update flow control and buffering threshold counts.
      diag_tx_mode[stream_index][buf_index].buffered_many_per_mark = DIAG_BUFFERED_MANY_DRAIN_PER_MARK;
      diag_tx_mode[stream_index][buf_index].buffered_many_bytes_mark =
         ((float)diag_tx_mode[stream_index][buf_index].buffered_many_per_mark / 100) *
         (diag_tx_mode[stream_index][buf_index].buf_size);

      diag_tx_mode[stream_index][buf_index].buffered_few_per_mark = DIAG_BUFFERED_FEW_DRAIN_PER_MARK;
      diag_tx_mode[stream_index][buf_index].buffered_few_bytes_mark =
         ((float)diag_tx_mode[stream_index][buf_index].buffered_few_per_mark / 100) *
         (diag_tx_mode[stream_index][buf_index].buf_size);

      /*
       Updating send buf header and patterm is needed for only stream1
       where buffering support is present
      */
      if( stream_index == DIAG_ID_TO_INDEX(DIAG_STREAM_1) )
      {
        diag_tx_mode[stream_index][buf_index].curr_threshold=diag_max_send_buf_len;
        diag_tx_mode[stream_index][buf_index].curr_data_block_header = (diag_send_buf_header_type *) (diag_tx_mode[stream_index][buf_index].buf_ptr);
   /*Fill the diag_send_buf_header*/
        diag_tx_mode[stream_index][buf_index].curr_data_block_header->pattern = 0xDEAD;
        diag_tx_mode[stream_index][buf_index].curr_data_block_header->length = 0;

   /*Move the head and tail by diag_send_buf_header_type number of bytes */
        diag_tx_mode[stream_index][buf_index].buf_head = sizeof(diag_send_buf_header_type);
        diag_tx_mode[stream_index][buf_index].buf_tail = 0;

   /*Update the used bytes*/
        diag_tx_mode[stream_index][buf_index].buf_used = sizeof(diag_send_buf_header_type);
      }
    }
  }

  DIAGPKT_DISPATCH_TABLE_REGISTER_PROC( DIAG_MY_PROC_ID,
                                        DIAG_SUBSYS_DIAG_SERV,
                    diagdiag_subsys_tbl_tx_mode );

} /* diag_tx_mode_init */
/*===========================================================================
FUNCTION DIAG_BUFFERED_MODE_STREAM (internal)

DESCRIPTION
  Checks if we're currently in one of the buffering modes.

PARAMETERS
  stream_id - Valid stream IDs are between DIAG_MIN_STREAM_ID and DIAG_MAX_STREAM_ID.
              DIAG_STREAM_ALL for all streams.

RETURN VALUE
  TRUE - If we're currently in one of the buffering modes.
  FALSE - Otherwise.

===========================================================================*/
boolean diag_buffered_mode_stream( uint8 stream_id )
{
  uint8 stream_index = 0;
  uint8 buf_index=0;

  if( (stream_id < DIAG_MIN_STREAM_ID) || (stream_id > DIAG_MAX_STREAM_ID) )
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Invalid stream_id (%d)", stream_id);
    return FALSE;
  }

  stream_index = DIAG_ID_TO_INDEX(stream_id);
  buf_index = DIAG_LOCAL_SEND_BUF_IDX;

  if( (diag_tx_mode[stream_index][buf_index].mode == DIAG_TX_MODE_BUFFERED_THRESH) ||
     (diag_tx_mode[stream_index][buf_index].mode == DIAG_TX_MODE_BUFFERED_CIR) )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }

} /* diag_buffered_mode_stream */

/*===========================================================================
FUNCTION DIAG_SET_TX_MODE_STREAM (internal)

DESCRIPTION
  Sets the Tx mode.

PARAMETERS
  mode - DIAG_TX_MODE_STREAMING         0
       - DIAG_TX_MODE_BUFFERED_THRESH   1
       - DIAG_TX_MODE_BUFFERED_CIR      2

  stream_id - stream_id - Valid stream IDs are between DIAG_MIN_STREAM_ID and
                          DIAG_MAX_STREAM_ID.
  diag_id   - diag_id of the entity (Peripheral/PD).

RETURN VALUE
  The current Tx mode.

===========================================================================*/
uint8 diag_set_tx_mode_stream( uint8 mode, uint8 stream_id, uint8 diag_id )
{
  uint8 prev_mode = 0;
  uint8 mode_set = DIAG_TX_MODE_INVALID;
  diag_tx_mode_type* tx_mode_ptr = NULL;

  /* ToDo: Only support switching modes for STREAM 1 now. Add support for STREAM 2 later. */
  if( stream_id != DIAG_STREAM_1 )
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_set_tx_mode_stream - Invalid stream_id (%d)", stream_id);
    return DIAG_TX_MODE_INVALID;
  }

  tx_mode_ptr = diag_get_tx_mode_ptr(stream_id, diag_id);

  if(tx_mode_ptr)
  {

    if( !DIAG_IS_INT_FEATURE_BIT_SET(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].feature_mask, F_DIAG_REQ_RSP_CHANNEL) )
    {
      MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_set_tx_mode_stream - Buffering mode not allowed if F_DIAG_REQ_RSP_CHANNEL not supported");
      mode_set=tx_mode_ptr->mode;
      return mode_set;
    }

    prev_mode = tx_mode_ptr->mode;

    if ( mode <= DIAG_TX_MODE_LAST )
    {
      /* Reset health counters if mode has changed. Also saves prev mode statistics. */
      if ( prev_mode != mode )
        diag_health_reset_dropped_cnt_stream( stream_id );

      tx_mode_ptr->mode = mode;
    }
    else
    {
      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_set_tx_mode_stream - Cannot set invalid mode (%d) for stream_id (%d)", mode, stream_id);
      mode_set= tx_mode_ptr->mode;
      return mode_set;
    }

    if ( tx_mode_ptr->mode == DIAG_TX_MODE_STREAMING )
    {
      tx_mode_ptr->drain = TRUE;

      #if !defined(DIAG_MP)
      /* Update NRT params locally only if Master or Standalone.  If Slave, Master
        will send the NRT control packets. */
      diag_update_mode_info( tx_mode_ptr->mode, stream_id );
      #endif

      /* Set signal to drain buffering pool */
    if( stream_id == DIAG_STREAM_1 )
        diag_set_internal_sigs( DIAG_INT_DRAIN_BUFFERING_BUFFER_SIG );
    }
    else if ( (tx_mode_ptr->mode == DIAG_TX_MODE_BUFFERED_THRESH) ||
              (tx_mode_ptr->mode == DIAG_TX_MODE_BUFFERED_CIR) )
    {
      tx_mode_ptr->drain = FALSE;
      #if defined(DIAG_MP_MASTER) || !defined(DIAG_MP)
          /* Update NRT params locally only if Master or Standalone.  If Slave, Master
             will send the NRT control packets. */
        diag_update_mode_info( tx_mode_ptr->mode, stream_id );
        #endif
    }

    mode_set= tx_mode_ptr->mode;
  }

  return mode_set;

} /* diag_set_tx_mode_stream */

/*===========================================================================
FUNCTION DIAG_GET_TX_MODE_STREAM (internal)

DESCRIPTION
  Gets the current Tx mode.

PARAMETERS
  stream_id - Valid stream IDs are between DIAG_MIN_STREAM_ID and DIAG_MAX_STREAM_ID.
              DIAG_STREAM_ALL for all streams.
  diag_id   - diag_id of the entity (Peripheral/PD).

RETURN VALUE
  The current Tx mode.

===========================================================================*/
uint8 diag_get_tx_mode_stream( uint8 stream_id, uint8 diag_id )
{
  uint8 mode = DIAG_TX_MODE_INVALID;
  diag_tx_mode_type* tx_mode_ptr = NULL;

  if( (stream_id < DIAG_MIN_STREAM_ID) || (stream_id > DIAG_MAX_STREAM_ID) )
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_get_tx_mode_stream - Invalid stream_id (%d)", stream_id);
    return mode;
  }

  tx_mode_ptr = diag_get_tx_mode_ptr(stream_id, diag_id);

  if(tx_mode_ptr)
  {
    mode = tx_mode_ptr->mode;
  }

  return mode;

} /* diag_get_tx_mode_stream */


/*===========================================================================
FUNCTION DIAG_SET_TX_PARAM_WM_PER_STREAM (internal)

DESCRIPTION
  Sets the Tx high and low watermark parameter. In buffered threshold mode,
  when the number of buffered bytes has reached the high wm level, we start
  to drain until we reach the low wm level.

  High wm level cannot be less then low wm level. High wm level should
  also not be greater than the level when we enable flow control to
  stop the traffic from the peripherals.

  In buffered threshold and buffered circular modes, we stop draining when
  the number of bytes in the buffered watermark queue has reached the low
  watermark level.

PARAMETERS
  high_per_value - High watermark value (as a percentage of total buffering pool size).
  low_per_value - Low watermark value (as a percentage of total buffering pool size).
  stream_id - Valid stream IDs are between DIAG_MIN_STREAM_ID and DIAG_MAX_STREAM_ID.
              DIAG_STREAM_ALL for all streams.
  diag_id   - diag_id of the entity (Peripheral/PD).

RETURN VALUE
  TRUE - The high/low wm values were updated successfully.
  FALSE - The high/low watermark values were NOT updated successfully.

===========================================================================*/
boolean diag_set_tx_param_wm_stream( uint8 high_per_value, uint8 low_per_value, uint8 stream_id, uint8 diag_id )
{
  /* Don't allow drain thresholds where Diag would have to be flow-controlled before we drain */
  uint32 high_byte_value = 0;
  uint32 low_byte_value = 0;
  uint32 buffer_size = 0;
  boolean ret_value = TRUE;
  uint8 new_high_value;
  uint8 new_low_value;
  diag_tx_mode_type* tx_mode_ptr = NULL;

  if( (stream_id < DIAG_MIN_STREAM_ID) || (stream_id > DIAG_MAX_STREAM_ID) )
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_set_tx_param_wm_stream - Invalid stream_id (%d)", stream_id);
    return 0;
  }

  tx_mode_ptr = diag_get_tx_mode_ptr(stream_id, diag_id);

  if(tx_mode_ptr)
  {
    /* If '0' is passed in, use current value. */
    if( high_per_value > 0 )
    {
      new_high_value = high_per_value;
    }
    else
    {
      new_high_value = tx_mode_ptr->buffered_many_per_mark;
    }

    if( low_per_value > 0 )
    {
      new_low_value = low_per_value;
    }
    else
    {
      new_low_value = tx_mode_ptr->buffered_few_per_mark;
    }

    /* Sanity check of new high wm value. */
    if( (new_high_value <= new_low_value) || (new_high_value > 100) )
    {
      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_set_tx_param_wm_stream - Invalid high percentage value %d for stream %d",
          high_per_value, stream_id);
      ret_value = FALSE;
    }

    if( (new_low_value >= new_high_value) || (new_low_value > 100) )
    {
      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_set_tx_param_wm_stream - Invalid low percentage value %d for stream %d",
          low_per_value, stream_id);
      ret_value = FALSE;
    }

    if( ret_value == FALSE )
      return FALSE;

     buffer_size = tx_mode_ptr->buf_size;

    high_byte_value = ((float)new_high_value/100)*buffer_size;

    low_byte_value = ((float)new_low_value/100)*buffer_size;

    if(high_byte_value < DIAG_MAX_SEND_DATA_SIZE)
      ret_value = FALSE;

    if( ret_value == FALSE )
      return FALSE;

    /* Passed all sanity checks. Set values now. */
    tx_mode_ptr->buffered_many_per_mark = new_high_value;
    tx_mode_ptr->buffered_few_per_mark = new_low_value;
    tx_mode_ptr->buffered_many_bytes_mark = high_byte_value;
    tx_mode_ptr->buffered_few_bytes_mark = low_byte_value;
  }

  return ret_value;

} /* diag_set_tx_param_wm_stream */


/*===========================================================================
FUNCTION DIAG_DRAIN_TX_BUFFER_STREAM (internal)

DESCRIPTION
  Drains the Tx buffer immediately. If in buffered mode, it will drain until
  Tx low watermark threshold is reached.

PARAMETERS
  stream_id - Valid stream IDs are between DIAG_MIN_STREAM_ID and
              DIAG_MAX_STREAM_ID.
  diag_id   - diag_id of the entity (Peripheral/PD).


RETURN VALUE
  None

===========================================================================*/
void diag_drain_tx_buffer_stream( uint8 stream_id, uint8 diag_id )
{
  diag_tx_mode_type* tx_mode_ptr = NULL;

  if( (stream_id < DIAG_MIN_STREAM_ID) || (stream_id > DIAG_MAX_STREAM_ID) )
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_drain_tx_buffer_stream - Invalid stream_id (%d)", stream_id);
    return;
  }

  /*
  Buffer index is 0 for root PD send buffer and is not tied to local DIAGID now.
  The mapping need to established once diagID changes are in place.
  */
  tx_mode_ptr = diag_get_tx_mode_ptr(stream_id, diag_id);

  if(tx_mode_ptr)
  {
    tx_mode_ptr->drain = TRUE;

    /* Set signal to transmit data from buffering buffer */
    if( stream_id == DIAG_STREAM_1 )
      diag_set_internal_sigs( DIAG_INT_DRAIN_BUFFERING_BUFFER_SIG );
  }

} /* diag_drain_tx_buffer_stream */


/*===========================================================================
FUNCTION DIAG_HEALTH_RESET_DROPPED_CNT_STREAM (internal)

DESCRIPTION
  Resets buffering mode health counter statistics per stream id.

PARAMETERS
  stream_id - Valid stream IDs are between DIAG_MIN_STREAM_ID and DIAG_MAX_STREAM_ID.
              DIAG_STREAM_ALL for all streams.

RETURN VALUE
  None

===========================================================================*/
void diag_health_reset_dropped_cnt_stream( uint8 stream_id )
{
  uint8 stream_index = 0;
  uint8 buf_index = 0;

  if( (stream_id < DIAG_MIN_STREAM_ID) || (stream_id > DIAG_MAX_STREAM_ID) )
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Invalid stream_id (%d)", stream_id);
    return;
  }

  stream_index = DIAG_ID_TO_INDEX(stream_id);
  /*
  Buffer index is 0 for root PD send buffer and is not tied to local DIAGID now.
  The mapping need to established once diagID changes are in place.
  */
  buf_index = DIAG_LOCAL_SEND_BUF_IDX;

  diag_tx_mode[stream_index][buf_index].prev_mode = diag_tx_mode[stream_index][buf_index].mode;
  diag_tx_mode[stream_index][buf_index].prev_dsm_dropped_cnt = diag_tx_mode[stream_index][buf_index].dsm_dropped_cnt;
  diag_tx_mode[stream_index][buf_index].prev_dsm_alloc_cnt = diag_tx_mode[stream_index][buf_index].dsm_alloc_cnt;
  diag_tx_mode[stream_index][buf_index].prev_byte_dropped_cnt = diag_tx_mode[stream_index][buf_index].byte_dropped_cnt;
  diag_tx_mode[stream_index][buf_index].prev_byte_alloc_cnt = diag_tx_mode[stream_index][buf_index].byte_alloc_cnt;

  diag_tx_mode[stream_index][buf_index].dsm_dropped_cnt = 0;
  diag_tx_mode[stream_index][buf_index].dsm_alloc_cnt = 0;
  diag_tx_mode[stream_index][buf_index].byte_dropped_cnt = 0;
  diag_tx_mode[stream_index][buf_index].byte_alloc_cnt = 0;

} /* diag_health_reset_dropped_cnt_stream */

/*===========================================================================
FUNCTION diag_buffering_buffer_used_cnt (internal)

DESCRIPTION
  This function returns the bytes used in buffering buffer.

PARAMETERS
  stream_id - Valid stream IDs are between DIAG_MIN_STREAM_ID and DIAG_MAX_STREAM_ID.

RETURN VALUE
  The bytes used in buffering buffer for given stream.

===========================================================================*/
uint32 diag_buffering_buffer_used_cnt( uint8 stream_id )
{
  uint8 stream_index = 0;
  uint8 buf_index = 0;

  if( (stream_id < DIAG_MIN_STREAM_ID) || (stream_id > DIAG_MAX_STREAM_ID) )
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Invalid stream_id (%d)", stream_id);
    return 0;
  }

  stream_index = DIAG_ID_TO_INDEX(stream_id);
  /*
  Buffer index is 0 for root PD send buffer and is not tied to local DIAGID now.
  The mapping need to established once diagID changes are in place.
  */
  buf_index = DIAG_LOCAL_SEND_BUF_IDX;

  return (diag_tx_mode[stream_index][buf_index].buf_used);

} /* diag_buffering_buffer_used_cnt */


/*===========================================================================
FUNCTION DIAGCOMM_TX_MODE_CONFIG (internal)

DESCRIPTION
  Command handler function for buffered mode commands.

PARAMETERS
  req_pkt_ptr - Pointer to request packet
  pkt_len - Length of request packet

RETURN VALUE
  Pointer to response packet.

===========================================================================*/
void * diagcomm_tx_mode_config( void * req_pkt_ptr, uint16 pkt_len )
{
  diagcomm_tx_mode_config_stream_req_type * req_ptr = (diagcomm_tx_mode_config_stream_req_type *)req_pkt_ptr;
  diagcomm_tx_mode_config_stream_rsp_type * rsp_ptr = NULL;
  diagpkt_subsys_cmd_code_type cmd_code;
  uint8 stream_id = 0;
  uint8 stream_index = 0;
  uint8 buf_index = 0;
  uint8 mode = DIAG_TX_MODE_INVALID;
  uint8 id =0;
  uint8 min_id = 0;
  uint8 max_id = 0;
  uint8 diag_id = DIAGID_UNKNOWN_ID;
  diag_diagID_pname_type *diagid_ptr = NULL;

  if( req_ptr == NULL )
  {
    return rsp_ptr;
  }

  cmd_code = diagpkt_subsys_get_cmd_code( req_ptr );
  stream_id = req_ptr->stream_id;

  if( stream_id > DIAG_MAX_STREAM_ID )
  {
    return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len);
  }

  switch( req_ptr->op_code )
  {

    case DIAG_TX_MODE_CONFIG_SET_MODE:
      rsp_ptr = (diagcomm_tx_mode_config_stream_rsp_type *) diagpkt_subsys_alloc( DIAG_SUBSYS_DIAG_SERV, cmd_code,
                                                     FPOS(diagcomm_tx_mode_config_stream_rsp_type, data) + sizeof(uint8) );
      if( rsp_ptr )
      {
        uint8 rval_mode = req_ptr->data.mode;
        rsp_ptr->op_code = DIAG_TX_MODE_CONFIG_SET_MODE;
        rsp_ptr->stream_id = stream_id;

        if( stream_id == DIAG_STREAM_ALL )
        {
          min_id = DIAG_MIN_STREAM_ID;
          max_id = DIAG_MAX_STREAM_ID;
        }
        else
        {
          min_id = stream_id;
          max_id = stream_id;
        }
        for( id=min_id; id<=max_id; id++ )
        {
          diagid_ptr = diag_diagID_pname_list;
          while(diagid_ptr !=NULL)
          {
            diag_id = diagid_ptr->diagID;
            mode = diag_set_tx_mode_stream(req_ptr->data.mode, id, diag_id);
            if(mode == DIAG_TX_MODE_INVALID)
            {
              rval_mode = DIAG_TX_MODE_INVALID;
            }
            diagid_ptr=diagid_ptr->next;
          }
        }
        rsp_ptr->data.mode = rval_mode;
      }
      break;

    case DIAG_TX_MODE_CONFIG_GET_MODE:
      rsp_ptr = (diagcomm_tx_mode_config_stream_rsp_type *) diagpkt_subsys_alloc( DIAG_SUBSYS_DIAG_SERV, cmd_code,
                                                     FPOS(diagcomm_tx_mode_config_stream_rsp_type, data) + sizeof(uint8) );
      if( rsp_ptr )
      {
        rsp_ptr->op_code = DIAG_TX_MODE_CONFIG_GET_MODE;
        rsp_ptr->stream_id = stream_id;

        mode = diag_get_tx_mode_stream( stream_id, diag_get_diagID() );
        rsp_ptr->data.mode = mode;
      }
      break;

  case DIAG_TX_MODE_CONFIG_SET_PARAM_WM_PER:
    if( (stream_id > DIAG_MIN_STREAM_ID) || (stream_id < DIAG_MAX_STREAM_ID) )
    {
      rsp_ptr = (diagcomm_tx_mode_config_stream_rsp_type *) diagpkt_subsys_alloc( DIAG_SUBSYS_DIAG_SERV, cmd_code,
                                                              FPOS(diagcomm_tx_mode_config_stream_rsp_type, data)
                                                              + sizeof(boolean));
      if( rsp_ptr )
      {
        boolean ret_value = FALSE;
        boolean rvalue = FALSE;
        rsp_ptr->op_code = DIAG_TX_MODE_CONFIG_SET_PARAM_WM_PER;
        rsp_ptr->stream_id = stream_id;
        
        
        diagid_ptr = diag_diagID_pname_list;
        while(diagid_ptr !=NULL)
        {
          diag_id = diagid_ptr->diagID;
          ret_value = diag_set_tx_param_wm_stream( req_ptr->data.wm_data.high_wm_per_value,
                                                         req_ptr->data.wm_data.low_wm_per_value,
                                                          stream_id, diag_id );
          if(ret_value == FALSE)
          {
            rvalue = FALSE;
          }
          diagid_ptr=diagid_ptr->next;
        }
        rsp_ptr->data.ret_value = rvalue;
      }
      else
      {
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_ptr, pkt_len);
      }
    }
    break;

  case DIAG_TX_MODE_CONFIG_GET_PARAM_WM:
    if( (stream_id > DIAG_MIN_STREAM_ID) || (stream_id < DIAG_MAX_STREAM_ID) )
    {
      rsp_ptr = (diagcomm_tx_mode_config_stream_rsp_type *) diagpkt_subsys_alloc( DIAG_SUBSYS_DIAG_SERV, cmd_code,
                                                       FPOS(diagcomm_tx_mode_config_stream_rsp_type, data) +
                             sizeof(diagcomm_tx_mode_config_wm_rsp_type) );
      if( rsp_ptr )
      {
          rsp_ptr->op_code = DIAG_TX_MODE_CONFIG_GET_PARAM_WM;
          rsp_ptr->stream_id = stream_id;
          stream_index = DIAG_ID_TO_INDEX(stream_id);

          /*
          Buffer index is 0 for root PD send buffer and is not tied to local DIAGID now.
          The mapping need to established once diagID changes are in place.
          */
          buf_index = DIAG_LOCAL_SEND_BUF_IDX;

          rsp_ptr->data.wm_data.high_wm_per_value = diag_tx_mode[stream_index][buf_index].buffered_many_per_mark;
          rsp_ptr->data.wm_data.high_wm_value = diag_tx_mode[stream_index][buf_index].buffered_many_bytes_mark;
          rsp_ptr->data.wm_data.low_wm_per_value = diag_tx_mode[stream_index][buf_index].buffered_few_per_mark;
          rsp_ptr->data.wm_data.low_wm_value = diag_tx_mode[stream_index][buf_index].buffered_few_bytes_mark;
          rsp_ptr->data.wm_data.buffer_size = diag_tx_mode[stream_index][buf_index].buf_size;
      }
    }
    else
    {
      return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_ptr, pkt_len);
    }
    break;

  case DIAG_TX_MODE_CONFIG_DRAIN:
    rsp_ptr = (diagcomm_tx_mode_config_stream_rsp_type *) diagpkt_subsys_alloc( DIAG_SUBSYS_DIAG_SERV, cmd_code,
                                                     FPOS(diagcomm_tx_mode_config_stream_rsp_type, data) );
    if( rsp_ptr )
    {
      if( stream_id == DIAG_STREAM_ALL )
      {
        min_id = DIAG_MIN_STREAM_ID;
        max_id = DIAG_MAX_STREAM_ID;
      }
      else
      {
        min_id = stream_id;
        max_id = stream_id;
      }
      for( id=min_id; id<=max_id; id++ )
      {
        diagid_ptr = diag_diagID_pname_list;
        while(diagid_ptr !=NULL)
        {
          diag_id = diagid_ptr->diagID;
          diag_drain_tx_buffer_stream( id, diag_id );
          diagid_ptr=diagid_ptr->next;
        }          
      }

      rsp_ptr->op_code = DIAG_TX_MODE_CONFIG_DRAIN;
      rsp_ptr->stream_id = stream_id;
    }
    break;

  case DIAG_TX_MODE_CONFIG_GET_CUR_DSM_CNT:
        /*We dont support DSM items anymore; So this opcode is not supported*/
      return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_ptr, pkt_len);
    break;

  case DIAG_TX_MODE_CONFIG_GET_CUR_BYTES_CNT:
    if( DIAG_STREAM_1 == stream_id)
    {
      rsp_ptr = (diagcomm_tx_mode_config_stream_rsp_type *) diagpkt_subsys_alloc( DIAG_SUBSYS_DIAG_SERV, cmd_code,
                                                       FPOS(diagcomm_tx_mode_config_stream_rsp_type, data) + sizeof(uint32) );
      if( rsp_ptr )
      {
        rsp_ptr->op_code = DIAG_TX_MODE_CONFIG_GET_CUR_BYTES_CNT;
      rsp_ptr->stream_id = stream_id;
        rsp_ptr->data.bytes_buffered_cnt = diag_buffering_buffer_used_cnt( stream_id );
      }
    }
    else
    {
      return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_ptr, pkt_len);
    }
    break;

  case DIAG_TX_MODE_CONFIG_GET_DSM_DROPPED_CNT:
      return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_ptr, pkt_len);
    break;

  case DIAG_TX_MODE_CONFIG_GET_DSM_ALLOC_CNT:
      return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_ptr, pkt_len);
    break;

  case DIAG_TX_MODE_CONFIG_GET_BYTE_DROPPED_CNT:
    if( DIAG_STREAM_1 == stream_id)
    {
      rsp_ptr = (diagcomm_tx_mode_config_stream_rsp_type *) diagpkt_subsys_alloc( DIAG_SUBSYS_DIAG_SERV, cmd_code,
                                                       FPOS(diagcomm_tx_mode_config_stream_rsp_type, data) + sizeof(uint32) );
      if( rsp_ptr )
      {
        rsp_ptr->op_code = DIAG_TX_MODE_CONFIG_GET_BYTE_DROPPED_CNT;
      rsp_ptr->stream_id = stream_id;

      stream_index = DIAG_ID_TO_INDEX(stream_id);
          /*
          Buffer index is 0 for root PD send buffer and is not tied to local DIAGID now.
          The mapping need to established once diagID changes are in place.
          */
          buf_index = DIAG_LOCAL_SEND_BUF_IDX;
          rsp_ptr->data.byte_dropped_cnt = diag_tx_mode[stream_index][buf_index].byte_dropped_cnt;
      }
    }
    else
    {
      return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_ptr, pkt_len);
    }
    break;

  case DIAG_TX_MODE_CONFIG_GET_BYTE_ALLOC_CNT:
    if( DIAG_STREAM_1 == stream_id)
    {
      rsp_ptr = (diagcomm_tx_mode_config_stream_rsp_type *) diagpkt_subsys_alloc( DIAG_SUBSYS_DIAG_SERV, cmd_code,
                                                       FPOS(diagcomm_tx_mode_config_stream_rsp_type, data) + sizeof(uint32) );
      if( rsp_ptr )
      {
        rsp_ptr->op_code = DIAG_TX_MODE_CONFIG_GET_BYTE_ALLOC_CNT;
        rsp_ptr->stream_id = stream_id;
        stream_index = DIAG_ID_TO_INDEX(stream_id);
        /*
        Buffer index is 0 for root PD send buffer and is not tied to local DIAGID now.
        The mapping need to established once diagID changes are in place.
        */
          buf_index = DIAG_LOCAL_SEND_BUF_IDX;
          rsp_ptr->data.byte_alloc_cnt = diag_tx_mode[stream_index][buf_index].buf_used;
      }
    }
    else
    {
      return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_ptr, pkt_len);
    }
    break;

  case DIAG_TX_MODE_CONFIG_RESET_DROPPED_CNT:
    rsp_ptr = (diagcomm_tx_mode_config_stream_rsp_type *) diagpkt_subsys_alloc( DIAG_SUBSYS_DIAG_SERV, cmd_code,
                                                     FPOS(diagcomm_tx_mode_config_stream_rsp_type, data) );
    if( rsp_ptr )
    {
      rsp_ptr->op_code = DIAG_TX_MODE_CONFIG_RESET_DROPPED_CNT;
    rsp_ptr->stream_id = stream_id;

    if( stream_id == DIAG_STREAM_ALL )
    {
      for( id=DIAG_MIN_STREAM_ID; id<=DIAG_MAX_STREAM_ID; id++ )
          diag_health_reset_dropped_cnt_stream( id );
        }
        else
        {
          diag_health_reset_dropped_cnt_stream( stream_id );
        }
      }
      break;

    case DIAG_TX_MODE_CONFIG_GET_BUFFER_SIZE:
      if( DIAG_STREAM_1 == stream_id)
      {
        rsp_ptr = (diagcomm_tx_mode_config_stream_rsp_type *) diagpkt_subsys_alloc( DIAG_SUBSYS_DIAG_SERV, cmd_code,
                                                       FPOS(diagcomm_tx_mode_config_stream_rsp_type, data) + sizeof(uint32) );
        if( rsp_ptr )
        {
          rsp_ptr->op_code = DIAG_TX_MODE_CONFIG_GET_BUFFER_SIZE;
          rsp_ptr->stream_id = stream_id;
          stream_index = DIAG_ID_TO_INDEX(stream_id);
          /*
          Buffer index is 0 for root PD send buffer and is not tied to local DIAGID now.
          The mapping need to established once diagID changes are in place.
          */
          buf_index = DIAG_LOCAL_SEND_BUF_IDX;
          rsp_ptr->data.buffer_size = diag_tx_mode[stream_index][buf_index].buf_size;
        }
      }
      else
      {
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_ptr, pkt_len);
      }
      break;

    default:
      return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len);
  }

  return rsp_ptr;

} /* diagcomm_tx_mode_config */

/*===========================================================================
FUNCTION DIAG_CTRL_SEND_DRAIN_STATUS

DESCRIPTION
  Sends a buffering mode drain status to apps. This should be called to send
  drain notification when a drain has completed for buffering mode.  This
  should not be sent for streaming mode.

PARAMETERS
  None

RETURN VALUE
  None

===========================================================================*/
void diag_ctrl_send_drain_status( void )
{
  diag_ctrl_msg_type ctrl_msg;
  boolean is_sent = FALSE;
  uint32 bytes_written = 0;

  /* Compose DIAG_CTRL_MSG_DRAIN_STATUS packet */
  ctrl_msg.cmd_type = DIAG_CTRL_MSG_DRAIN_STATUS;
  ctrl_msg.data_len = sizeof(diag_ctrl_msg_drain_status_type);
  ctrl_msg.data.ctrl_drain_status.version = DIAG_CTRL_MSG_DRAIN_STATUS_VER;
  ctrl_msg.data.ctrl_drain_status.peripheral_id = DIAG_CTRL_MY_PROC_ID;

  /* Send status to apps */
  bytes_written = diagcomm_ctrl_send_mult_pkt( (void *)&ctrl_msg, DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_drain_status_type),
                                                NULL, 0, DIAG_CTRL_USER_PKT_ID_0, DIAGCOMM_PORT_REMOTE_PROC, DIAGCOMM_PORT_1, TRUE, &is_sent );

  if( (is_sent == FALSE) || (bytes_written == 0) )
      {
    MSG_3(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Send drain status failed %d %d %d", DIAG_CTRL_MY_PROC_ID, bytes_written, is_sent);
    }

} /* diag_ctrl_send_drain_status */

/*===========================================================================
FUNCTION DIAG_GET_TX_MODE_PTR (internal)

DESCRIPTION
  Gets the tx_mode pointer for a given stream and diag ID instance.

PARAMETERS
  stream_id - Valid stream IDs are between DIAG_MIN_STREAM_ID and DIAG_MAX_STREAM_ID.
  diag_id   - Valid diagID.

DEPENDENCIES
  Root PD should know the available user processes and assign pre-defined diagIDs to
  them and in turn AP DIAG also should use the same diagIDs for processing the
  incoming data.

RETURN VALUE
  TX mode pointer for the given stream_id and diag_id.

===========================================================================*/
diag_tx_mode_type* diag_get_tx_mode_ptr(uint8 stream_id, uint8 diag_id)
{
  uint8 stream_index = 0;
  diag_tx_mode_type* return_ptr = NULL;
  diag_diagID_pname_type *diagid_ptr = diag_diagID_pname_list;

  if(stream_id > DIAG_MAX_STREAM_ID)
  {
    return return_ptr;
  }
  stream_index = DIAG_ID_TO_INDEX(stream_id);

  while(diagid_ptr != NULL)
  {
    /* Only send the request if is_ctrl_sent is FALSE */
    if(diagid_ptr->diagID == diag_id)
    {
      return_ptr = diagid_ptr->tx_mode_ptr[stream_index];
      break;
    }
    diagid_ptr = diagid_ptr->next;
  }
  return return_ptr;
} /* diag_get_tx_mode_ptr */
