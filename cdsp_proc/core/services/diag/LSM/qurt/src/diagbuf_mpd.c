/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             Diagnostic Subsystem Buffer Allocation Manager

General Description
  This file contains routines that manage the diag output buffer.
  This allocation scheme is modeled as a circular buffer. Currently, 
  diag output buffer is used for streamed data (LOG, MSG).

Initialization and Sequencing Requirements diagbuf_mpd_init() must be 
called before using this service.

Copyright (c) 2014-2017 by QUALCOMM Technologies, Incorporated. 
All Rights Reserved. Qualcomm Confidential and Proprietary 
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                              Edit History

 $Header: //components/rel/core.qdsp6/2.1.c4/services/diag/LSM/qurt/src/diagbuf_mpd.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/21/17   ph      Tag DIAG ID header to all the diag packets sent out.
11/03/16   sa      Implemented seperate queue for UserPD events and enabled buffering
05/12/16   sa      Circular Mode for UserPD buffer
08/07/17   sp      Fix KW errors
06/12/17   sp      Add support for microULog debug capability
04/24/17   sp      Added support for extended logs in userPD
02/09/17   sp      Added support for extended logs
09/26/16   rs      Log code causing diagbuf_mpd_check_overrun_pattern error not printed properly.
05/12/16   ph      SMD references removal and code clean up.
05/14/15   rh      Removed unused function
11/14/14   jtm/vk  Created file 
===========================================================================*/

#include "customer.h"

#include "comdef.h"
#include "diag_v.h"
#include "diagbuf_v.h"
#include "diagbuf_mpd.h"
#include "Diag_LSM.h"
#include "diagcomm_v.h"
#include "diagdiag_v.h"
#include "diagi_v.h"
#include "diagpkt.h"
#include "diagtarget.h"
#include "diagtune.h"
#include "err.h"
#include "event.h"
#include "log.h"
#include "msg.h"
#include "msgi.h" /* For msg_set_all_rt_masks_adv() */
#include "stringl.h"
#include "feature.h"
#include "Diag_LSM_Qdi_wrapper.h"

#include <stdio.h>
#include "crc.h"
#include "osal.h"
#include "diag_cfg.h"
#include "qurt_qdi.h"
#include "qurt_qdi_driver.h"

#include "micro_diagbuffer_i.h"
#include "micro_diagbuffer_client_read.h"

#include "diagcomm_ctrl.h" /* For DIAG_CTRL_MSG_DIAGDATA etc */

/* 
   This is a pointer of type diagbuf_mpd_type which points to a
   User-PD's diag buffer. Both diagmpd_buf and
   diagmpd_buf->diagbuf_mpd_buf are allocated in Core-PD.
*/ 
diagbuf_mpd_type *diagmpd_buf = NULL; 

/* Forward prototype declarations for local routines. */
static void diagbuf_mpd_write_overrun_check_pattern( void *ptr );

/* Maps each version of the diagmpdbuf packet to the length of the first packet in the chain */
uint8 diagbuf_mpd_ver_len[DIAGBUF_MAX_VERSION_NUM] = {
  DIAGBUF_NEXT_ALIGNED_BYTE(sizeof(diag_cmd_ext_v1_type)+sizeof(uint16))/sizeof(diagbuf_header_type), // Version 1
  DIAGBUF_NEXT_ALIGNED_BYTE(sizeof(diag_cmd_ext_v2_type)+sizeof(uint16))/sizeof(diagbuf_header_type)  // Version 2
};


/*==========================================================================
FUNCTION  DIAG_SET_PD_BUF_MODE

DESCRIPTION
  This function is to be used by the PD to enable or disable circular
  mode on the buffer to save power

PARAMETERS
  mode -- parameter  to send mode change value
  mode == DIAG_PD_BUF_MODE_STREAMING ; Streaming mode buffer
  mode == DIAG_PD_BUF_MODE_CIRCULAR ; Circular mode buffer

RETURN VALUE
  DIAG_PD_BUF_MODE_INVALID   -- Invalid mode request
  DIAG_PD_BUF_MODE_CIRCULAR  -- Mode set to circular
  DIAG_PD_BUF_MODE_STREAMING -- Mode set to streaming


=========================================================================*/

uint8 diag_set_pd_buf_mode(uint8 mode)
{

  if( (mode!=DIAG_PD_BUF_MODE_CIRCULAR) && (mode !=DIAG_PD_BUF_MODE_STREAMING))
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Trying to set invalid mode for UserPD buffer");
    return DIAG_PD_BUF_MODE_INVALID;
  }

  diagmpd_buf->diagbuf_mpd_mode = mode;
  return diagmpd_buf->diagbuf_mpd_mode;
}  /* diag_set_pd_buf_mode */


/*==========================================================================
FUNCTION  DIAG_GET_PD_BUF_MODE

DESCRIPTION
  This function is to be used by the PD to get information about the UserPD buffer mode.

PARAMETERS
  None
  
RETURN VALUE
  DIAG_PD_BUF_MODE_CIRCULAR  -- Mode set to circular
  DIAG_PD_BUF_MODE_STREAMING -- Mode set to streaming


=========================================================================*/

uint8 diag_get_pd_buf_mode()
{

  return diagmpd_buf->diagbuf_mpd_mode;
  
}  /* diag_get_pd_buf_mode */



/*===========================================================================
 
FUNCTION DIAGBUF_MPD_GET_MSG_ALLOC_COUNT

DESCRIPTION
    This function returns the number of msgs successfully
    allocated for current PD client.

PARAMETERS 
    None
 
RETURN
    msg_alloc_count, if diagmpd_buf is allocated
    0 otherwise 
    
===========================================================================*/
uint32 diagbuf_mpd_get_msg_alloc_count(void)
{
  if (diagmpd_buf)
  {
    return (diagmpd_buf->msg_alloc_count);
  }
  return (0);
} /* diagbuf_mpd_get_msg_alloc_count */

/*===========================================================================
 
FUNCTION DIAGBUF_MPD_GET_MSG_DROP_COUNT

DESCRIPTION
    This function returns the 'count' of msgs that were
    unsuccessful in allocation for current PD client.
 
PARAMETERS 
    None
 
RETURN
    msg_drop_count, if diagmpd_buf is allocated
    0 otherwise
    
===========================================================================*/
uint32 diagbuf_mpd_get_msg_drop_count(void)
{
  if (diagmpd_buf)
  {
    return (diagmpd_buf->msg_drop_count);
  }
  return (0);
} /* diagbuf_mpd_get_msg_drop_count */

/*===========================================================================
 
FUNCTION DIAGBUF_MPD_RESET_MSG_ALLOC_DROP_COUNTS

DESCRIPTION
    This function resets msg alloc and drop counts for current
    PD client.
 
PARAMETERS 
    None
 
RETURN 
    None
 
===========================================================================*/
void diagbuf_mpd_reset_msg_alloc_drop_counts(void)
{
  if (diagmpd_buf)
  {
    diagmpd_buf->msg_alloc_count = 0;
    diagmpd_buf->msg_drop_count  = 0;
  }
} /* diagbuf_mpd_reset_msg_alloc_drop_counts */

/*===========================================================================
 
FUNCTION DIAGBUF_MPD_GET_LOG_ALLOC_COUNT

DESCRIPTION
    This function returns the 'count' of logs that were
    successfully allocated for current PD client.
 
PARAMETERS 
    None
 
RETURN 
    log_alloc_count, if diagmpd_buf is allocated
    0 otherwise
 
===========================================================================*/
uint32 diagbuf_mpd_get_log_alloc_count(void)
{
  if (diagmpd_buf)
  {
    return (diagmpd_buf->log_alloc_count);
  }
  return (0);
} /* diagbuf_mpd_get_log_alloc_count */

/*===========================================================================
 
FUNCTION DIAGBUF_MPD_GET_LOG_DROP_COUNT

DESCRIPTION
    This function returns the 'count' of logs that were
    unsuccessful in allocation for current PD client.
 
PARAMETERS 
    None
 
RETURN 
    log_drop_count, if diagmpd_buf is allocated
    0 otherwise
 
===========================================================================*/
uint32 diagbuf_mpd_get_log_drop_count(void)
{
  if (diagmpd_buf)
  {
    return (diagmpd_buf->log_drop_count);
  }
  return (0);
} /* diagbuf_mpd_get_log_drop_count */

/*===========================================================================
 
FUNCTION DIAGBUF_MPD_RESET_LOG_ALLOC_DROP_COUNTS

DESCRIPTION
    This function resets log alloc and drop counts for the
    current PD client.
 
PARAMETERS 
    None
 
RETURN 
    None
 
===========================================================================*/
void diagbuf_mpd_reset_log_alloc_drop_counts(void)
{
  if (diagmpd_buf)
  {
    diagmpd_buf->log_alloc_count = 0;
    diagmpd_buf->log_drop_count  = 0;
  }
} /* diagbuf_mpd_reset_log_alloc_drop_counts */

/*===========================================================================

FUNCTION DIAGBUF_MPD_ALLOC

DESCRIPTION
  This function allocates the specified amount of space in the diag
  output buffer.

PARAMETERS
  uint8  code                     Specifying the command code.
  uint32 length                   Length of requested buffer.
  int nice                        Niceness padding value
  diagbuf_alloc_request           Type of service requesting
                                  buffer space. (Required for DIAG Health)
  uint8 chain                     0: No chain; 1: 1st packet; 2: 2nd packet
  uint8 version

DEPENDENCIES
  diagbuf_commit() must be called to commit a buffer to the diag pool.  This
  must happen reasonably soon after this call completes.  If the buffer is
  needed to accumulate data over time, then you should use your own buffer
  use this just long enough to copy the data.

SIDE EFFECTS
  diagbuf_head may be updated.

RETURN VALUES
  If successful         - Pointer to allocated buffer
  If no space available - NULL

============================================================================*/
void*
diagbuf_mpd_alloc(uint8 code, uint32 length, int nice, uint8 diagbuf_alloc_request, uint8 chain, uint8 version)
{
  void *ptr = NULL; /* Pointer to return */
  uint32 buffer_used;          /* Length to allocate        */
  diagbuf_header_type *pkt_hdr = NULL;
  diagbuf_header_type *prev_pkt_hdr = NULL;

  /* Local indices to manipulate buffer */
  int iHead;
  int iHead_old;
  int iHeadIdx;
  int iTail;
  int iTailIdx;
  int iSize;

  if ( (length > DIAG_MAX_TX_PKT_SIZ) || (length > diagmpd_buf->diagbuf_size) ) 
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR,"Attempt to alloc too much: %d", (dword) length);
    return (NULL);
  }

  buffer_used = sizeof(diagbuf_header_type) + /* Diagbuf header */
     (uint32)length +  /* Number of bytes requested buffer */
     sizeof(uint16);   /* CRC                              */


  buffer_used = DIAGBUF_NEXT_ALIGNED_BYTE(buffer_used);

  if ( buffer_used > diagmpd_buf->diagbuf_size )
  {
    ERR_FATAL("diagbuf_mpd_alloc: buffer_used (%d) > diagmpd_buf->diagbuf_size (%d)", buffer_used, diagmpd_buf->diagbuf_size, 0);
  }

retry:
  prev_pkt_hdr = NULL;

  iHead_old = diagmpd_buf->diagbuf_mpd_head;

  iTail = diagmpd_buf->diagbuf_mpd_tail;
  iSize = diagmpd_buf->diagbuf_size;
  iTailIdx = DIAGBUF_CALC_IDX(iTail, iSize);
  iHead = diagmpd_buf->diagbuf_mpd_head;
  iHeadIdx = DIAGBUF_CALC_IDX(iHead, iSize);
 
  /* Check if valid state is read, if not re-read */
  if (iHead_old != iHead)
  {
    goto retry;
  }

  ASSERT(iHead < 2 * iSize);
  ASSERT(iTail < 2 * iSize);
  ASSERT(iHeadIdx < iSize);
  ASSERT(iTailIdx < iSize);

  /* Set a local pointer to the header */
  pkt_hdr = ((diagbuf_header_type *)&diagmpd_buf->diagbuf_mpd_buf[iHeadIdx]);

  /* First check if the nice pad is available.  If OK, then alloc */
  if ( DIAGBUF_BALANCE_PAD + nice < DIAGBUF_FREE(iHead, iTail, iSize) )
  {
    /* if not wrapped and no room here */
    if ( iTailIdx <= iHeadIdx &&       /* Not wrapped &&  */
        iHead - iTail < iSize &&       /* Not full &&     */
        /* No room here!   */
         (uint32)iHeadIdx + buffer_used >= iSize )
    {
      prev_pkt_hdr = pkt_hdr;
      /* Mark header for wrap - do this after it is known that we
          have got a valid diagbuf_head */

      /* Wrap */
      DIAGBUF_WRAP(iHead, iSize);
      ASSERT((iHead == 0) || (iHead == iSize));
      /* Recalculate head index */
      iHeadIdx = DIAGBUF_CALC_IDX(iHead, iSize);
      ASSERT(iHeadIdx == 0);
    }

    if (buffer_used <= (unsigned int)DIAGBUF_FREE(iHead, iTail, iSize))
    {
      /* Move head */
      DIAGBUF_MOVE(iHead, (int)buffer_used, iSize);
      ASSERT(iHead < 2 * iSize);

      /* Update value of global head index */
      /* If atomic compare and set fails, implies some other thread wrote
         a new diagbuf_head before this could be written :
         this requires a retry                           :
                                                         :
         if(diagbuf_head==ihead_old)                     :
             diagbuf_head = iHead;                       :
         else                                            :
             retry;                                     */
      
      if (!osal_atomic_compare_and_set((osal_atomic_word_t *)&diagmpd_buf->diagbuf_mpd_head, iHead_old, iHead))
      {
        goto retry;
      }

      if (prev_pkt_hdr)
      {
        prev_pkt_hdr->status = DIAGBUF_WRAP_S;
      }

      /* Set header field of buffer */
      pkt_hdr = ((diagbuf_header_type *)&diagmpd_buf->diagbuf_mpd_buf[iHeadIdx]);

      pkt_hdr->status = DIAGBUF_UNCOMMIT_S;
      pkt_hdr->pad_len = (byte)(buffer_used - (uint32)length - sizeof(diagbuf_header_type)
                            - sizeof(uint16));
      pkt_hdr->length = (uint16)length;
      pkt_hdr->chain = chain;
      pkt_hdr->version = version;

      /* Set 'ptr' to pkt_hdr +1 = data following pkt_hdr */
      ptr = pkt_hdr + 1;
      /* Logic to set diagbuf_head_read here */
    }
  } /* If requested length + nice pad is free */

  if (ptr)
  {
    /* Increment allocation requests counter */
    switch (diagbuf_alloc_request)
    {
       case DIAGBUF_ALLOC_REQUEST_LOG:
         if (diagmpd_buf->log_alloc_count == MAX_VALUE_UINT32)
         {
           MSG_1(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Variable log_alloc_count has reached maximum value, process: %d", diagmpd_buf->pid);
         }
         else
         {
           diagmpd_buf->log_alloc_count++;
         }
         break;
       case DIAGBUF_ALLOC_REQUEST_MSG:
         if (diagmpd_buf->msg_alloc_count == MAX_VALUE_UINT32)
         {
           MSG_1(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Variable msg_alloc_count has reached maximum value, process %d", diagmpd_buf->pid);
         }
         else
         {
           diagmpd_buf->msg_alloc_count++;
         }
         break;
       default: /* DIAGBUF_ALLOC_REQUEST_OTHER */
         /* No need to update any variables */
         break;
    }
  }
  else
  {
    //Entered here means the buffer is full. So instead of dropping the packet if PD is in circular mode we will reset the head and tail
    if(diagmpd_buf->diagbuf_mpd_mode==DIAG_PD_BUF_MODE_CIRCULAR)
   	{
   	  /*set head and tail to zero*/
	  diagmpd_buf->diagbuf_mpd_head = diagmpd_buf->diagbuf_mpd_tail =0;
      if (diagmpd_buf->mpd_head_tail_reset_count == MAX_VALUE_UINT32)
        {
          MSG_1(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Variable mpd_head_tail_reset_count has reached maximum value, process: %d", diagmpd_buf->pid);
        }
      else
        {
          diagmpd_buf->mpd_head_tail_reset_count++;
        }		
   	}

    switch (diagbuf_alloc_request)
    {
       case DIAGBUF_ALLOC_REQUEST_LOG:
         if (diagmpd_buf->log_drop_count == MAX_VALUE_UINT32)
         {
           MSG_1(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Variable log_drop_count has reached maximum value, process: %d", diagmpd_buf->pid);
         }
         else
         {
           diagmpd_buf->log_drop_count++;
         }
         break;
       case DIAGBUF_ALLOC_REQUEST_MSG:
         if (diagmpd_buf->msg_drop_count == MAX_VALUE_UINT32)
         {
           MSG_1(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Variable msg_drop_count has reached maximum value, process: %d", diagmpd_buf->pid);
         }
         else
         {
           diagmpd_buf->msg_drop_count++;
         }
         break;
       default: /* DIAGBUF_ALLOC_REQUEST_OTHER */
         /* No need to update any variables */
         break;
    }
  }

  if (ptr)
  {
    /* Sanity check */
    if (DIAGBUF_USED(iHead, iTail, iSize) > iSize)
    {
      ERR_FATAL("diagbuf_mpd_alloc: Possible Ring buffer corrupt iHead = %d, iTail = %d, iSize = %d!", iHead, iTail, iSize);
    }
    diagbuf_mpd_write_overrun_check_pattern(pkt_hdr);
    diagbuf_mpd_write_stream_id(ptr, DIAG_STREAM_1); /* write default stream ID for all packets allocated in diagbuf  */
    diagbuf_mpd_set_bit_outbound_pkt_handler(ptr); /*Set MSB in stream ID to skip calling fp in diagbuf_drain() */
    diagpkt_set_cmd_code(ptr, code);
  }

  return (ptr);

} /* end diagbuf_mpd_alloc */

/*===========================================================================
FUNCTION DIAGBUF_MPD_ALLOC_CHAIN

DESCRIPTION
    Allocates chained(consecutive) packets in Diag buffer.
    This function should be called only for extended log pkts

PARAMETERS
   length                    Length of requested buffer
   nice                      Niceness padding value
   diagbuf_alloc_request     Required for DIAG Health
   chain                     0 - not chained
                             1 - first pkt in the chain
                             2 - second pkt in the chain
   version                   0 - non extended pkts
                             1 or 2 - extended pkts
   parameter                 Passed in by diag client while
                             calling log APIs and F3 macros

DEPENDENCIES
  This function must only be called from within this file by externalized
  alloc functions.

  diagbuf_commit() must be called to commit a buffer to the diag pool.  This
  must happen reasonably soon after this call completes.  If the buffer is
  needed to accumulate data over time, then you should use your own buffer
  use this just long enough to copy the data.

  diagbuf_commit() should be called seperately for the two chained pkts in
  diagbuf.

RETURN VALUE
  Pointer to the allocated space in diagbuf
  NULL if there is no space available in diagbuf

===========================================================================*/
void *
diagbuf_mpd_alloc_chain(uint8 code, uint32 length, int nice, uint8 diagbuf_alloc_request, uint8 chain, uint8 version, void * parameter)
{
   /* Variable init */
   uint32 buf_used = 0;
   uint32 buf_used_first_pkt = 0;
   uint32 buf_used_second_pkt = 0;
   uint8 pad_len1 = 0;
   uint8 pad_len2 = 0;
   diagbuf_header_type * diagbuf_header_first_ptr = NULL;
   diagbuf_header_type * diagbuf_header_second_ptr = NULL;
   uint32 ver_len = 0;
   byte * log_record_first_ptr = NULL;
   void * log_record_second_ptr = NULL;
   diag_cmd_ext_v1_type *ptr_payload1 = NULL;
   diag_cmd_ext_v2_type *ptr_payload2 = NULL;
   char temp_buf[ULOG_MAX_NAME_SIZE];
   uint32 name_len = 0;

   /* Sanity checks */
   if ( (version > DIAG_EXT_API_VERSION_MAX) ||
        (chain > DIAGBUF_LAST_PKT_CHAIN) ||
        (code != DIAG_LOG_F) )
   {
      return (NULL);
   }

   /* Parameter check */
   if ( (version > DIAG_EXT_API_VERSION_0 ) &&
        (parameter == NULL) )
   {
      return (NULL);
   }

   /* Currently supports versions 0, 1, and 2 */
   switch(version)
   {
      case DIAG_EXT_API_VERSION_0:
      {
         return diagbuf_mpd_alloc(code, length, nice, diagbuf_alloc_request, 0, DIAG_EXT_API_VERSION_0);
      }
      case DIAG_EXT_API_VERSION_1:
      case DIAG_EXT_API_VERSION_2:
      case DIAG_EXT_API_VERSION_3:
      {
         /* Compute version-specific length */
         if(version == DIAG_EXT_API_VERSION_1)
         {
            ver_len = sizeof(diag_cmd_ext_v1_type);
         }
         else if(version == DIAG_EXT_API_VERSION_2)
         {
            ver_len = sizeof(diag_cmd_ext_v2_type);
            if(ULogCore_NameGet( parameter, &temp_buf[0]) == DAL_SUCCESS)
            {
                 /* Match found, get length */
                 name_len = strlen(temp_buf);

                 if(name_len > ULOG_MAX_NAME_SIZE )
                 {
                    ERR_FATAL("Invalid ULog ", 0, 0, 0);
                 }
            }
            else
            {
                MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "ULogCore_NameGet returned fail");
                return NULL;
            }
         }
         else if(version == DIAG_EXT_API_VERSION_3)
         {
            ver_len = sizeof(diag_cmd_ext_v2_type);
            if(micro_diagbuffer_NameGet( parameter, &temp_buf[0]) == MICRO_ULOG_SUCCESS)
            {
                /* Match found, get length */
                name_len = strlen(temp_buf);
		 
                if(name_len > MICRO_ULOG_MAX_NAME_SIZE )
                {
                    ERR_FATAL("Invalid microULog ", 0, 0, 0);
                }
                /*
                  DIAG_EXT_API_VERSION_3 is converted to DIAG_EXT_API_VERSION_2 at this point,
                  and will be treated as v2 packet from here. The v3 version was only required
                  to retrieve name of the microULog to be filled in extender header pkt
                */
                version = DIAG_EXT_API_VERSION_2;
            }
            else
            {
                MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "micro_diagbuffer_NameGet returned fail");
                return NULL;
            }
         }

         /* Calculate the length of the first packet */
         buf_used_first_pkt = ver_len +   /* payload of the first packet */
             sizeof(uint16);   /* CRC - to account for DEAD pattern for the first packet in the chain */

         /* Make the first packet byte aligned */
         buf_used_first_pkt = DIAGBUF_NEXT_ALIGNED_BYTE(buf_used_first_pkt);

         /* Calculate the length of the second pkt; account for the diag buffer header for the second pkt here */
         buf_used_second_pkt = length +
             sizeof(diagbuf_header_type);  /* Diagbuf header for the second packet */

         /* Total length required for both the chained pkts;
             Call diagbuf_alloc() only once since we cant gaurantee if
             the chained pkts are consecutive if we call diagbuf_alloc() twice
         */
         buf_used = buf_used_first_pkt + buf_used_second_pkt;

         /* Allocate space for both the first and second packets, and then point to the first packet */
         log_record_first_ptr = (byte *) diagbuf_mpd_alloc(code, buf_used, nice, diagbuf_alloc_request, chain, version);

         /* Sanity check allocation */
         if ( log_record_first_ptr == NULL )
         {
            return (NULL);
         }

         /*Clear MSB set in stream ID, as fp should be called in diagbuf_drain() for chained extended pkt */
         diagbuf_mpd_clear_bit_outbound_pkt_handler(log_record_first_ptr);

         /* Point to the first diagbuf header */
         diagbuf_header_first_ptr = (diagbuf_header_type *) (log_record_first_ptr - sizeof(diagbuf_header_type));

         /* Update the first diagbuf header */
         diagbuf_header_first_ptr->length = ver_len;
         diagbuf_header_first_ptr->stream_id = DIAG_STREAM_1;
         pad_len2 = diagbuf_header_first_ptr->pad_len;
         pad_len1 = ((uint32)buf_used_first_pkt - ver_len - sizeof(uint16));
         ASSERT(pad_len2 < 4);
         ASSERT(pad_len1 < 4);
         diagbuf_header_first_ptr->pad_len = pad_len1;

         /* Save version-specific fields in the first diagbuf header */
         if(version == DIAG_EXT_API_VERSION_1)
         {
            /* Assign the version and ID field */
            ptr_payload1 = (diag_cmd_ext_v1_type *)log_record_first_ptr;
            ptr_payload1->version = DIAG_EXT_API_VERSION_1;
            ptr_payload1->proc_id = DIAG_MY_PROC_ID;
            ptr_payload1->id = *((uint32 *)parameter);
         }
         else
         {
            /* Assign the version and other fields */
            ptr_payload2 = (diag_cmd_ext_v2_type *)log_record_first_ptr;
            ptr_payload2->version = DIAG_EXT_API_VERSION_2;
            ptr_payload2->proc_id = DIAG_MY_PROC_ID;
            ptr_payload2->name_len = name_len;
            ptr_payload2->time_offset_type = DIAG_ULOG_NO_CUSTOM_OFFSET;
            strlcpy(ptr_payload2->ulog_name, temp_buf, name_len + 1 );
         }

         diagbuf_mpd_write_overrun_check_pattern(diagbuf_header_first_ptr);

         /* Point to the second diagbuf header */
         diagbuf_header_second_ptr = (diagbuf_header_type *)(log_record_first_ptr + buf_used_first_pkt);

         /* Update the second diagbuf header */
         diagbuf_header_second_ptr->status = DIAGBUF_UNCOMMIT_S;
         diagbuf_header_second_ptr->pad_len = pad_len2;
         diagbuf_header_second_ptr->length = (uint16)length;
         diagbuf_header_second_ptr->chain = DIAGBUF_SECOND_PKT_CHAIN;
         diagbuf_header_second_ptr->version = version;
         /* NOTE: Stream ID should be filled out by the calling function, as per standard log allocations */

         /* Find and return the pointer to the second packet */
         log_record_second_ptr = (void *)((byte *)diagbuf_header_second_ptr + sizeof(diagbuf_header_type));
         return (log_record_second_ptr);
      }

      default:
        return NULL;
   }
} /* end diagbuf_mpd_alloc_chain */

/*===========================================================================

FUNCTION DIAGBUF_MPD_SHORTEN

DESCRIPTION
  This function reduces the length field of a previously allocated buffer.

  'ptr' must point to the same address that was returned by a prior call to
  diagbuf_mpd_alloc().

  Allocating too much and using this to shorten the packet is ideal for
  situations in which the length of the packet is not know prior to
  allocation.  Using this scheme does, however, consume resources that would
  otherwise be used to buffer outbound diagnostics data.  Please use this
  scheme sparingly.
 
PARAMETERS 
  void * ptr      - pointer returned by diagbuf_mpd_alloc()
  uint new_length - length, that needs to be set in len field of
                    the packet (*ptr)
 
RETURNS 
  None 

============================================================================*/
/* mutex inclusion in this function needs to be reviewed again */
void
diagbuf_mpd_shorten(
   void *ptr,       /* Pointer returned from diagbuf_alloc() */
   unsigned int new_length /* New length of buffer                  */
   )
{
  diagbuf_header_type *pkt_hdr; /* Points to packet header */
  diagbuf_header_type *hole; /* This will point to new header field. */

  unsigned int curr_length;
  unsigned int hole_length;
  byte stream_id;
  uint32 temp_length = 0;

  /* Sanity check.  Make sure it is within boundaries of ring buffer */
  if ( (byte *)ptr < diagmpd_buf->diagbuf_mpd_buf ||
       (byte *)ptr >= (&diagmpd_buf->diagbuf_mpd_buf[diagmpd_buf->diagbuf_size] - sizeof(uint16)) ) 
  {
    /* This is bad.  Incorrect usage. Report error and do nothing.*/
    //MSG_2(MSG_SSID_DIAG, MSG_LEGACY_LOW,"Invalid packet address: ptr:0x%x, l:%d", ptr, new_length);
    return;
  }

  osal_lock_mutex(&diagmpd_buf->diagbuf_mpd_buf_cs);
  stream_id = diagbuf_mpd_read_stream_id(ptr);
  /* Cast pointer to do arithmetic with diagbuf_header_type */
  pkt_hdr = (diagbuf_header_type *)ptr;

  /* Subtract 1 diagbuf_header_type (4 bytes) to get header */
  pkt_hdr -= 1;

  curr_length = pkt_hdr->length;

  if (new_length == 0)
  {
    /* Special case: shorten to 0.
     * The entire packet is just changed to be a hole
     */
    hole = pkt_hdr;

    /* length of the hole is current length + CRC + pad */
    temp_length = DIAGBUF_NEXT_ALIGNED_BYTE(curr_length + sizeof(uint16));

    if ( temp_length > DIAG_MULTIPD_BUF_SIZE )
      ERR_FATAL("diagbuf_mpd_shorten: temp_length (%d) > DIAGBUF_MPD_SIZE (%d)", temp_length, DIAG_MULTIPD_BUF_SIZE, 0);

	/* Ensure temp_length can be stored in a uint16 */
    if ( temp_length > 0xFFFF )
      ERR_FATAL("diagbuf_mpd_shorten: temp_length (%d) > 0xFFFF", temp_length, 0, 0);

    hole->length = (uint16)temp_length;
    if( (pkt_hdr->chain == DIAGBUF_SECOND_PKT_CHAIN)
        && (pkt_hdr->version <= DIAGBUF_MAX_VERSION_NUM)
		&& (pkt_hdr->version >= DIAGBUF_VERSION_1))
    {
      /* Mark the Diagbuf Header of first packet as HOLE and its length as the whole packet (Extended + Log packet)*/
      pkt_hdr = ((diagbuf_header_type *)pkt_hdr) - diagbuf_mpd_ver_len[pkt_hdr->version -1];
      pkt_hdr = ((diagbuf_header_type *)pkt_hdr) - 1;
      pkt_hdr->status = DIAGBUF_HOLE_S;
      pkt_hdr->length = (uint16)temp_length + (diagbuf_mpd_ver_len[pkt_hdr->version -1] + 1)*sizeof(diagbuf_header_type);
    }

    hole->pad_len = 0;
    hole->status = DIAGBUF_HOLE_S;

    /* Since this essentially commits the packet as a hole, notify DIAG
       that the caller is ready to TX. */
    diagbuf_mpd_tx_notify();
  }
  else
  {
    /* If new length is longer or the same, we do nothing. */
    if ( curr_length > new_length )
    {
      /* Compute hole size and record new packet length */

      pkt_hdr->length = (uint16)new_length;

      /* Adjust length fields to include CRC fields */
      new_length += sizeof(uint16);
      curr_length += sizeof(uint16);

      /* Calcualte pad for the packet */
      pkt_hdr->pad_len = (byte)(DIAGBUF_NEXT_ALIGNED_BYTE(new_length) - new_length);

      /* Now change length fields to be the length of entire buffer used */
      new_length += pkt_hdr->pad_len;
      curr_length = DIAGBUF_NEXT_ALIGNED_BYTE(curr_length);

      ASSERT(curr_length < diagmpd_buf->diagbuf_size); 

      hole = (diagbuf_header_type *)ptr;

      /* Move to next aligned address past the new packet length */
      hole += new_length / sizeof(*hole);

      hole_length = curr_length - new_length;

      if (hole_length >= sizeof(diagbuf_header_type))
      {
        hole->status = DIAGBUF_HOLE_S;
        hole->length = (uint16)hole_length - sizeof(diagbuf_header_type);
        hole->pad_len = 0;
      }

      diagbuf_mpd_write_overrun_check_pattern(pkt_hdr);
      diagbuf_mpd_write_stream_id(ptr, stream_id);
      if (pkt_hdr->chain == DIAGBUF_NO_CHAIN)
      {
        /*Set MSB in stream ID to skip calling fp in diagbuf_drain() */
        diagbuf_mpd_set_bit_outbound_pkt_handler(ptr);
      }
    }
    else if (new_length > curr_length)
    {
      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_LOW,"Attempted shorten %d to %d ignored.",
            curr_length, new_length);
    }
    else
    {
      /* If shortening to same size, do nothing. */
    }
  }

  osal_unlock_mutex(&diagmpd_buf->diagbuf_mpd_buf_cs);

} /* diagbuf_mpd_shorten */


/*===========================================================================

FUNCTION diagbuf_mpd_check_overrun_pattern

DESCRIPTION
  Check for overrun pattern in the buffer provided.
============================================================================*/
static boolean diagbuf_mpd_check_overrun_pattern(diagbuf_header_type *hdr)
{
  uint8 *pattern = NULL;
  boolean success = TRUE;

  /* The 2 byte CRC field at the end of the allocated buffer is written
   * with a mask to detect buffer overrun.  Check to make sure that value
   * wasn't corrupted.
   */
  pattern = (((byte *)hdr) + hdr->length +
             sizeof(diagbuf_header_type));

  if ((*pattern != 0xDE) || ((*(pattern + 1) != 0xAD)))
  {
    diag_log_rsp_type *logpkt;

    success = FALSE;

    /* This detects a memory scribble.  There is no way to know how far
       past the allocated buffer the caller scribbled.  If this is
       determined to be a log, print the log code in lieu of the file
       name on the LCD.  Next, disable interrupts to avoid user of
       corrupt memory.  Reset diagbuf_head/tail so panic mode won't
       attempt to flush it, then enter panic mode as usual. */

    /* Disable all interrupts to avoid any more diagnostics client
       activitiy.  */
    osal_disable_interrupts();



/*lint -save -e{740} Unusual pointer cast*/
    logpkt = (diag_log_rsp_type *)(hdr + 1);
/*lint -restore */

    switch (diagpkt_get_cmd_code(logpkt))
    {
       case DIAG_LOG_F:
         /* Display that a log is the culprit in lieu of filename. */
      ERR_FATAL("diagbuf_mpd_check_overrun_pattern: Log %04X : pri %d", log_get_code(logpkt->log), osal_thread_get_pri(), 0);
         break;

       default:
         ERR_FATAL("error fatal in diagbuf_mpd_check_overrun_pattern hdr = %p, mpf_head = %d, mpd_tail = %d", hdr, diagmpd_buf->diagbuf_mpd_head, diagmpd_buf->diagbuf_mpd_tail); 
         break;
    }
    osal_enable_interrupts();
  }
  return (success);
} /* diagbuf_mpd_check_overrun_pattern */

/*===========================================================================

FUNCTION DIAGBUF_MPD_COMMIT

DESCRIPTION
  This function commits previously allocated space in the diag output buffer.

  'ptr' must be the address that was returned by a prior call to
  diagbuf_alloc().

  This function signals the diag task and may cause a context switch.

============================================================================*/
void diagbuf_mpd_commit(
   void *ptr /* Buffer to commit */
   )
{
  diagbuf_header_type *pkt_hdr; /* Pointer to the header of the buffer. */

  /* Cast pointer to do arithmetic with diagbuf_header_type */
  pkt_hdr = (diagbuf_header_type *)ptr;

  /* Subtract 1 diagbuf_header_type (4 bytes) to get header field */
  pkt_hdr -= 1;

  /* Sanity checks */
  if (ptr == NULL ||
      (byte *)ptr < &diagmpd_buf->diagbuf_mpd_buf[sizeof(uint16)] ||
      (byte *)ptr >= &diagmpd_buf->diagbuf_mpd_buf[diagmpd_buf->diagbuf_size]
      )
  {
    //MSG_1(MSG_SSID_DIAG, MSG_LEGACY_LOW,"Invalid commit address 0x%x. PID:%d", (dword) ptr, diagmpd_buf->pid);
    return;
  }

  if (pkt_hdr->status != DIAGBUF_HOLE_S)
  {
    if (diagbuf_mpd_check_overrun_pattern(pkt_hdr))
    {
      /* Set status to "committed" */
      pkt_hdr->status = DIAGBUF_COMMIT_S;
    }
  }

  diagmpd_buf->diagbuf_mpd_commit_size += pkt_hdr->length;
  if (diagmpd_buf->diagbuf_mpd_commit_size >= diagmpd_buf->diagbuf_mpd_commit_threshold)
  {
    diagmpd_buf->diagbuf_mpd_commit_size = 0;
    diagbuf_mpd_tx_notify();
  }
  else
  {
    diagbuf_mpd_set_drain_timer();
  }
} /* diagbuf_mpd_commit */

/* Private functions */


/*===========================================================================

FUNCTION DIAGBUF_MPD_WRITE_OVERRUN_CHECK_PATTERN

DESCRIPTION
  Writes the overrun check pattern in the CRC field at the end of the bufer
  pointed to by 'ptr'.  diagbuf_commit() checks this field to determine if
  buffer overrun has occured.

  The work to check this is done in diagbuf_commit() rather than a seperate
  function to save the cost of calculations and the function call.

  'ptr' points to the length field before the packet, not the beginning of
  the packet itself.

===========================================================================*/
static void diagbuf_mpd_write_overrun_check_pattern(void *ptr)
{
  unsigned int length;

  if (ptr)
  {
    length = ((diagbuf_header_type *)ptr)->length;

    /* The reserved space for the CRC is be used as a sanity check.
     * A pattern is written to those bytes in an effort to detect
     * buffer overrun.
     * The Head index is pointing to the length field.  We want to write
     * the CRC, which is 'length - sizeof(crc)' bytes ahead.
     */
    ptr = ((byte *)ptr) + length + sizeof(diagbuf_header_type);
    *((byte *)ptr) = 0xDE;
    *((byte *)ptr + 1) = 0xAD;
  }
} /* diagbuf_mpd_write_overrun_check_pattern */

/*===========================================================================
FUNCTION DIAGBUF_MPD_WRITE_STREAM_ID

DESCRIPTION
  Writes the stream_id into diagbuf.
  Packet looks like this:
  diagbuf_header(4bytes) diag_packet(variable) over-run_check_pattern(2 bytes, 0xDEAD)
  diagbuf header is of the following format:
  status(4 bits) chain(2 bits) version(2 bits) pad_length(3 bits) stream_id(5 bits)
===========================================================================*/
void diagbuf_mpd_write_stream_id(void *ptr, byte stream_id)
{

  if (ptr)
  {
    ptr = ((diagbuf_header_type *)((diagbuf_header_type *)ptr - 1));
    /* explicitly set the pkt stream_id with the MSB set to 1 */
    ((diagbuf_header_type *)ptr)->stream_id = stream_id;
  }
} /* diagbuf_mpd_write_stream_id */

/*===========================================================================
FUNCTION DIAGBUF_MPD_READ_STREAM_ID

DESCRIPTION
  Reads the stream ID for the diag packet.
  Packet looks like this:
  diagbuf_header(4bytes) diag_packet(variable) over-run_check_pattern(2 bytes, 0xDEAD)
  diagbuf header is of the following format:
  status(4 bits) chain(2 bits) version(2 bits) pad_length(3 bits) stream_id(5 bits)
===========================================================================*/
byte diagbuf_mpd_read_stream_id(const void *ptr)
{
  byte stream_id = 0;

  if (ptr)
  {
    stream_id = ((diagbuf_header_type *)((diagbuf_header_type *)ptr - 1))->stream_id;
  }
  return (stream_id);
} /* diagbuf_mpd_read_stream_id */

/*===========================================================================
FUNCTION DIAGBUF_MPD_SET_BIT_OUTBOUND_PKT_HANDLER

DESCRIPTION
  Set the most significant bit in the stream id to indicate that we should not
  call the fp associated with F3/log in diagbuf_drain()
===========================================================================*/
void diagbuf_mpd_set_bit_outbound_pkt_handler(void *ptr)
{
  if ( ptr )
  {
    ptr = ((diagbuf_header_type *)((diagbuf_header_type *)ptr - 1));
    ((diagbuf_header_type *)ptr)->stream_id = ((diagbuf_header_type *)ptr)->stream_id | DIAGBUF_OUTBOUND_PKT_HANDLER_BIT_MASK;
  }
} /* diagbuf_mpd_set_bit_outbound_pkt_handler */

/*===========================================================================
FUNCTION DIAGBUF_MPD_CLEAR_BIT_OUTBOUND_PKT_HANDLER

DESCRIPTION
  Clear the most significant bit in the stream id to indicate that we need not
  call the fp associated with F3/log in diagbuf_drain()
===========================================================================*/
void diagbuf_mpd_clear_bit_outbound_pkt_handler(void *ptr)
{
  if ( ptr )
  {
    ptr = ((diagbuf_header_type *)((diagbuf_header_type *)ptr - 1));
    ((diagbuf_header_type *)ptr)->stream_id = ((diagbuf_header_type *)ptr)->stream_id & (~(DIAGBUF_OUTBOUND_PKT_HANDLER_BIT_MASK));
  }
} /* diagbuf_mpd_clear_bit_outbound_pkt_handler */

/*===========================================================================
FUNCTION DIAGBUF_MPD_TX_NOTIFY

DESCRIPTION
  calls diag_lsm_trigger_drain() to set signal on diag task in
  CorePD to drain diagbufs
 
PARAMATERS 
   None
   
RETURN VALUE 
   None
   
===========================================================================*/
void diagbuf_mpd_tx_notify(void)
{
  diag_lsm_trigger_drain();
} /* diagbuf_mpd_tx_notify */

/*===========================================================================
FUNCTION DIAGBUF_MPD_SET_DRAIN_TIMER

DESCRIPTION
  This function will be called on PD's drain timer expiration.
  It sets diagbuf_mpd_tx_notify to set tx signal on diag task in
  CorePD to drain from diagbufs. It also calls 
  diag_lsm_set_drain_timer() to set drain time on diag task in
  CorePD (just in case).  
 
PARAMETERS 
   None
   
RETURN VALUE 
   None
   
===========================================================================*/
void diagbuf_mpd_set_drain_timer(void)
{
  osal_reset_timer(&diagmpd_buf->diagbuf_mpd_drain_timer);
  diagbuf_mpd_tx_notify(); 
  diag_lsm_set_drain_timer(); // We may not need it. 
} /* diagbuf_mpd_set_drain_timer */

/*===========================================================================
FUNCTION DIAGBUF_MPD_SET_MPD_DRAIN_TIMER

DESCRIPTION
  if drain timer for the PD has expired sets it. Else does
  nothing.
 
PARAMETERS 
   None
   
RETURN VALUE 
   None
   
===========================================================================*/
void diagbuf_mpd_set_mpd_drain_timer( void )
{
  if (osal_get_remaining_time(&diagmpd_buf->diagbuf_mpd_drain_timer) == 0)
  {
    int return_val = 0; 
    /* Set timer to DIAG_MULTIPD_DRAIN_TIMER ms */
    return_val = osal_set_timer(&diagmpd_buf->diagbuf_mpd_drain_timer, DIAG_MULTIPD_DRAIN_TIMER);
    ASSERT(OSAL_SUCCESS == return_val);
  }
} /* diagbuf_mpd_set_drain_timer */


