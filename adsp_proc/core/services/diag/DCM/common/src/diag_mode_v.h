#ifndef DIAG_MODE_V_H
#define DIAG_MODE_V_H
/*==========================================================================

              Diag APIs for buffering and streaming modes

General Description
  
Copyright (c) 2014,2016-2018 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.qdsp6/2.1/services/diag/DCM/common/src/diag_mode_v.h#3 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/19/18   is      Moved buffering defines to a common file
02/21/17   ph      Tag DIAG ID header to all the diag packets sent out.
05/12/16   ph      SMD references removal and code clean up.
04/10/14   sr      Support for peripheral buffering mode 

===========================================================================*/

#include "diag_defs.h"

typedef struct
{
  uint16 pattern;
  uint16 length;
}diag_send_buf_header_type;

/* 
This is the header that is tagged along with each 
DIAG packet sent out from peripheral 
*/
typedef struct
{
  uint8 diag_id; // DIAG ID value 
  uint8 reserved; // Reserved 
  uint16 length;  //Length of the data packet  
}diag_tag_hdr_type;

typedef struct
{
uint8* ptr;
uint32 head;
uint32 tail;
uint32 size;
uint32 threshold;
uint32 used;
boolean wrap;
diag_send_buf_header_type* curr_data_block_header;
diag_tag_hdr_type* tag_hdr;
}diag_send_buf_type;

typedef struct
{
  uint8 mode;
  boolean drain;

  uint8* buf_ptr;
  uint32 buf_head;
  uint32 buf_tail;
  uint32 buf_used;
  uint32 buf_size;
  uint32 curr_threshold; /*Maximum index that packet be written to and it changes
                           depending on head and tail movement in buffering*/
  boolean buf_wrap;
  diag_send_buf_header_type* curr_data_block_header;

  uint32 buffered_many_bytes_mark;  /* # of bytes used before we start draining */
  uint8 buffered_many_per_mark;     /* % of buffer used before we start draining */
  uint32 buffered_few_bytes_mark;   /* # of bytes used before we stop draining */
  uint8 buffered_few_per_mark;      /* % of buffer used before we stop draining */

  uint32 dsm_dropped_cnt;
  uint32 dsm_alloc_cnt;
  uint32 byte_dropped_cnt;
  uint32 byte_alloc_cnt;

  //Stores previous counts before a mode switch/reset
  uint8 prev_mode;
  uint32 prev_dsm_dropped_cnt;
  uint32 prev_dsm_alloc_cnt;
  uint32 prev_byte_dropped_cnt;
  uint32 prev_byte_alloc_cnt;

  #if defined(DIAG_MP_MASTER)
  uint32 fwd_cur_dsm_cnt;
  #endif

  //Flat buffer parameters
  uint32 cur_buf_cnt;

} diag_tx_mode_type;

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
boolean diag_buffered_mode_stream( uint8 stream_id );

/*===========================================================================

FUNCTION DIAG_TX_MODE_INIT (internal)

DESCRIPTION
  Initializes global variable for keeping track of streaming mode vs buffering mode.

PARAMETERS
  None
  
RETURN VALUE
  None
  
===========================================================================*/
void diag_tx_mode_init( void );

/*===========================================================================
FUNCTION DIAG_DRAIN_SEND_BUFFER (internal)

DESCRIPTION
  This function drains the buffering buffer per stream. 
  
PARAMETERS
  stream_id - stream_id for buffering buffer to drain.
              Valid stream IDs are between DIAG_MIN_STREAM_ID and DIAG_MAX_STREAM_ID.
  
RETURN VALUE
  None
  
===========================================================================*/
void diag_drain_send_buffer( uint8 stream_id, diag_tx_mode_type* tx_mode_ptr );

/*===========================================================================
FUNCTION diag_buffering_buffer_used_cnt (internal)
  
DESCRIPTION
  This function returns the bytes used in buffering buffer.

PARAMETERS
  stream_id - Valid stream IDs are between DIAG_MIN_STREAM_ID and DIAG_MAX_STREAM_ID.
  
RETURN VALUE
  The bytes used in buffering buffer for given stream.
  
===========================================================================*/
uint32 diag_buffering_buffer_used_cnt( uint8 stream_id );

/*===========================================================================

FUNCTION DIAG_DRAIN_BUFFER_CHECK

DESCRIPTION
  Checks whether Diag needs to set signal to drain the buffering buffer. Diag should
  signal draining if we're passed the threshold in threshold buffering mode
  or if we're in streaming mode.
  
PARAMETERS
  stream_id - Valid stream IDs are between DIAG_MIN_STREAM_ID and DIAG_MAX_STREAM_ID.
tx_mode_ptr - tx_mode_ptr to retrieve the drain(send) buffer info.
  
RETURN VALUE
  TRUE - Diag can drain the buffer now.
  FALSE - Buffer is not ready to be drained yet.
  
===========================================================================*/
boolean diag_drain_buffer_check( uint8 stream_id, diag_tx_mode_type *tx_mode_ptr);

#endif /*DIAG_MODE_V_H*/
