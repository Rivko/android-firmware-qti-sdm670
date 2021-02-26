/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         Diagnostic Task 

General Description
  This file defines variables and functions that is used for island buffering modes.

Copyright (c) 2018 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.qdsp6/2.1.c4/services/diag/micro_diagbuffer/src/diag_micro_buffering.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/08/18   is      Add support for pd threshold buffering mode in island mode
===========================================================================*/
#ifdef DIAG_UIMAGE_MODE

#include "diag_defs.h"       /*For DIAG_TX_MODE_INVALID */
#include "diag_uimage.h"
#include "micro_diagbuf_mpd_type.h"

#include "uSleep_mode_trans.h" /*For uSleep_exit */
#include "uSleep.h"            /*For USLEEP_SUCCESS */


extern micro_diagbuf_mpd_type *micro_diagmpd_buf;



/*==========================================================================
 FUNCTION  micro_diagbuffer_drain_check
 
 DESCRIPTION
   This function checks the current bytes used in Diag micro diag buffer
   against the watermark value and triggers draining if the thresholds have been
   reached and it is in threshold buffering mode.
 
 PARAMETERS
   None
 
 RETURN VALUE
   None 
 
 =========================================================================*/
 void micro_diagbuffer_drain_check( void )
 {
   uint32 buf_used = 0;
   uint32 buf_not_read = 0;
   uint32 uSleep_rv;
   
   if( (micro_diagmpd_buf == NULL) || (micro_diagmpd_buf->handle == NULL) )
     return;
 
   if( micro_diagmpd_buf->tx_mode.mode == DIAG_TX_MODE_BUFFERED_THRESH )
   {
     buf_used = micro_diagmpd_buf->handle->write - micro_diagmpd_buf->handle->readWriter;
     buf_not_read = micro_diagmpd_buf->handle->write - micro_diagmpd_buf->handle->read;
       
     if( buf_not_read >= micro_diagmpd_buf->tx_mode.buffered_many_bytes_mark )
     {
         /* Exit island mode to initiate draining if we've crossed the high wm threshold. */
         micro_diagmpd_buf->tx_mode.drain = TRUE;
         uSleep_rv = uSleep_exit();
     }
   }
   
 } /* micro_diagbuffer_drain_check */
 
 
/*==========================================================================
 FUNCTION  diag_set_uimage_tx_mode
 
 DESCRIPTION
   This function is to toggle the Diag uimage buffer draining modes
   between threshold buffering and circular buffering.
 
 PARAMETERS
   DIAG_TX_MODE_BUFFERED_THRESH  -- Set mode to threshold buffering
   DIAG_TX_MODE_BUFFERED_CIR -- Set mode to circular buffering
 
 RETURN VALUE
   The current mode after setting it
 
 =========================================================================*/
uint8 diag_set_uimage_tx_mode( uint8 mode )
{
  if( micro_diagmpd_buf == NULL )
    return (uint8)DIAG_TX_MODE_INVALID;

  if( (mode == DIAG_TX_MODE_BUFFERED_THRESH) || (mode == DIAG_TX_MODE_BUFFERED_CIR) )
  {
     MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "diag_set_uimage_tx_mode mode=%d", mode);
     
     micro_diagmpd_buf->tx_mode.prev_mode = micro_diagmpd_buf->tx_mode.mode;
     micro_diagmpd_buf->tx_mode.mode = mode;
     
     if( micro_diagmpd_buf->tx_mode.mode == DIAG_TX_MODE_BUFFERED_THRESH )
     {
       /* Check if we need to drain if switched to threshold buffering mode */
       micro_diagbuffer_drain_check();
     }
     else
     {
       micro_diagmpd_buf->tx_mode.drain = FALSE;
     }
     
     return (uint8)micro_diagmpd_buf->tx_mode.mode;     
   }
   else
   {
     MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Error setting invalid Tx mode %d", mode);
     return (uint8)DIAG_TX_MODE_INVALID;
   }
   
} /* diag_set_uimage_tx_mode */


/*==========================================================================
 FUNCTION  diag_get_uimage_tx_mode
 
 DESCRIPTION
   This function is to get information about the uimage buffering mode.
 
 PARAMETERS
   None
 
 RETURN VALUE
   DIAG_TX_MODE_BUFFERED_THRESH  -- Mode set to threshold
   DIAG_TX_MODE_BUFFERED_CIR -- Mode set to circular
   DIAG_TX_MODE_INVALID -- Mode hasn't been set yet
 
 =========================================================================*/
uint8 diag_get_uimage_tx_mode( void )
{
  if( micro_diagmpd_buf == NULL )
    return (uint8)DIAG_TX_MODE_INVALID;
  else
    return micro_diagmpd_buf->tx_mode.mode;

}  /* diag_get_uimage_tx_mode */


/*===========================================================================
 FUNCTION diag_set_uimage_tx_high_param_wm
 
 DESCRIPTION
   This function sets the high watermark percentage for uimage buffer to 
   trigger a drain while in threshold buffering mode. While in threshold 
   buffering mode, a drain is triggered when the number of buffered bytes has 
   reached the high watermark level and will stop draining when the buffer is 
   empty.
 
   In buffered threshold and buffered circular modes, we stop draining when
   the number of bytes in the buffered watermark queue has reached the low
   watermark level.
 
 PARAMETERS
   high_per_value[in] - High watermark value (as a percentage of total uimage buffer size).
 
 RETURN VALUE
   TRUE - The high wm value was updated successfully.
   FALSE - The high watermark value was NOT updated successfully.
 
 ===========================================================================*/
 boolean diag_set_uimage_tx_high_param_wm( uint8 high_per_value )
 {
   uint32 high_byte_value = 0;
   uint32 buffer_size = 0;
  
   /* Sanity checks */  
   if( (micro_diagmpd_buf == NULL) || (micro_diagmpd_buf->handle == NULL) )
     return FALSE;
 
   if( (high_per_value == 0) || (high_per_value > 100) )
     return FALSE;
        
   buffer_size = micro_diagmpd_buf->handle->bufSize;
   high_byte_value = ((float)high_per_value/100)*buffer_size;
   
   micro_diagmpd_buf->tx_mode.buffered_many_per_mark = high_per_value;
   micro_diagmpd_buf->tx_mode.buffered_many_bytes_mark = high_byte_value;
          
   MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "diag_set_uimage_tx_high_param_wm - many_per_mark=%d, many_bytes_mark=%d", 
        high_per_value, high_byte_value);
   
   return TRUE;
   
 } /* diag_set_uimage_tx_high_param_wm */
 
 
 /*==========================================================================
 FUNCTION  diag_get_uimage_tx_high_param_wm
 
 DESCRIPTION
   This function returns the high watermark percentage for uimage buffer to 
   trigger a drain while in threshold buffering mode.
 
 PARAMETERS
   None
 
 RETURN VALUE
   High watermark percentage for uimage buffer 
 
 =========================================================================*/
 uint8 diag_get_uimage_tx_high_param_wm( void )
 {
   if( micro_diagmpd_buf == NULL )
     return 0;
 
   return micro_diagmpd_buf->tx_mode.buffered_many_per_mark;
   
 } /* diag_get_uimage_tx_high_param_wm */
  
#endif /* DIAG_UIMAGE_MODE */

