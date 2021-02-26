/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          Logging Services

General Description
This file contains the definitions to log apis

Copyright (c) 2010 - 2015, 2017 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*==========================================================================

                              Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/services/diag/DCM/rtos/src/log_api.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/08/17   gn      Added support for diag services to uImage
06/05/17   nk      Added debug support for uimage in root PD
04/24/17   sp      Added support for extended logs in userPD
04/09/17   sp      Added support for extended logs in rootPD
06/18/15   sa      Added support for time sync feature.
01/16/15   sa      Removed the references of mobile view code.
11/17/14   sa      Added check for max log packet length supported in log_alloc/submit APIs
08/20/14   sr      Fixed issue with F3s not coming out 
07/06/14   ph      Added support for Diag over STM
11/27/13   ph      Updated MSG 1.0 API references to MSG.2.0.
11/22/13   sr      Removed PACK from pointer references
06/21/13   sr       Fixed the issue with log_status() not returning the
                    correct status in Sensor PD
04/22/13   ph       Moved listeners allocation to system heap.
04/18/13   ph      Replace memcpy() and memmove() with memscpy() and memsmove()
02/01/13   sg      Fixed Klock work warnings
02/06/13   rh      Added composite (aggregated) log mask optimization
07/05/12   sg      Changes to bring up diag Multi PD
04/27/11   is      Resolve compiler warnings
12/20/10   mad     Changes for dual diag log mask
09/28/10    sg     Moved diag_time_get to diag_cfg.h
09/17/10   is      Migrate from deprecated clk driver APIs
08/25/10   sg      Fixed compiler warnings
06/281/0   sg      Inclusion of msg.h for msg macro definitons
04/09/10   sg      Created Initial Version
===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "log.h"
#include "diagtarget.h"   /* For diag_time_get() */
#include "diag_cfg.h"
#include "diaglogi.h"
#include "diagbuf_v.h"
#include "diagdiag_v.h"
#include "msg.h"
#include "log_arrays_i.h"
#include "stringl.h"
#include "qw.h"

#include "diag_stm.h"

#ifdef FEATURE_DIAG_WCDMA_CFN_SFN_TIMESTAMP
#include "wcdmadiagif.h"
#endif

#if defined (DIAG_QDSP6_APPS_PROC)
#include "diagstub.h"
#else
#include "assert.h"
#endif
#include "diag_micro_i.h" /*For log_mask_enabled()*/
#include "diagi_v.h"


  /* Time Get Function Pointer from diag.c */
  extern uint8 (*diag_time_get)(qword time);

extern uint8 diagbuf_ver_len[DIAGBUF_MAX_VERSION_NUM]; /*from diagbuf.c*/

extern osal_mutex_arg_t         log_on_demand_mutex;
/* Log listener support */
typedef struct
{
  diag_log_listener listener;
  void *param; /* User's parameter */
}
log_listener_entry_type;

#ifdef FEATURE_DEBUG_DIAG_HEAP
uint32 log_listener_match_and_free_cnt = 0;
uint32 diag_add_log_listener_malloc_cnt = 0;
uint32 diag_add_log_listener_free_cnt = 0;
#endif /* FEATURE_DEBUG_DIAG_HEAP */


extern log_state_enum_type log_state;
extern void *log_commit_last;
extern void *log_flush_last;


extern log_on_demand_list_item_type log_on_demand_list[LOG_ON_DEMAND_SIZE];

diag_searchlist_type log_listeners = {FALSE, {0}, {{0}}, 0, 0, 0, NULL};
extern byte log_composite_mask[LOG_MASK_SIZE]; /* externed from diaglog.c */
extern byte log_listener_mask[LOG_MASK_SIZE];

boolean log_status_mask (log_code_type code, byte stream_id);
byte log_status_adv (log_code_type code);
void log_mask_set_bit (byte *mask_ptr, unsigned int id, unsigned int item, boolean enabled);
void log_listener_notify (uint32 seq_num, void *log, diag_searchlist_node_type *node);
boolean log_submit_ext_core (void * ptr, unsigned int version, void * parameter);
void * log_alloc_core (log_code_type code,  unsigned int length);




/*===========================================================================

FUNCTION LOG_SET_TIMESTAMP_EX

DESCRIPTION
  This function captures the system time and stores it in the given log record.
===========================================================================*/
void log_set_timestamp_ex (void * log_hdr_ptr)
{
  qword time;
  int ts_set_local = FALSE;
#ifdef FEATURE_DIAG_WCDMA_CFN_SFN_TIMESTAMP
    uint32 frame_number = 0;
#endif
    diag_time_get(time);

    #ifdef FEATURE_DIAG_WCDMA_CFN_SFN_TIMESTAMP
      frame_number = wl1_get_frame_number_for_diag() & LOG_FN_MASK;
      time[0] >>= LOG_FN_NUM_BITS;
      time[0] <<= LOG_FN_NUM_BITS;
    time[0] |= frame_number;
    #endif

    ts_set_local = TRUE;

  if ( ts_set_local )
  {
/*lint -save -e645 */
    qw_equ_misaligned(((log_header_type *) log_hdr_ptr)->ts, time);
/*lint -restore */
  }

} /* log_set_timestamp_ex */

/*=========================================================================*/
void * log_alloc_core (
  log_code_type code,
  unsigned int length
)
{
  diag_log_rsp_type *log_pkt_ptr; /* Pointer to packet being created */
  log_header_type *hdr_ptr = NULL;
  void *return_ptr = NULL;
  byte stream_id = 0;
  if (length <= sizeof(log_header_type))
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Alloc invalid length %d", length);
  }
  else if ((length + LOG_DIAGPKT_OFFSET) > DIAG_MAX_TX_PKT_SIZ)
  {
    MSG_2(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Attempt to allocate %d but Max Length of a log packet supported is %d", (length + LOG_DIAGPKT_OFFSET), DIAG_MAX_TX_PKT_SIZ);
  }
  else
  {
    stream_id = log_status_adv(code);
  }
  if (stream_id > 0)
  {
    /*------------------------------------------------
     Allocate enough for entire LOG response packet,
     not just the log given.
    ------------------------------------------------*/
#ifdef FEATURE_DIAG_STM
    /* Check is STM enabled and that DIAG_STREAM_STM is the *only* stream */
    if( (diag_stm_enabled) && (stream_id == DIAG_STREAM_STM))
    {
      /* When STM is enabled, we need to allocate packets differently if they are needed
         by listeners. If listeners are active for this log, stream_id will also contain
         DIAG_STREAM_LISTENERs bit, which can be used to distinguish it from other types
         of data.

         In this use case, we detect if *only* STM is enabled (i.e. no listeners) and then allocate
         as an STM only packet.
       */
      log_pkt_ptr = (diag_log_rsp_type *) 
      diag_stm_log_alloc(DIAG_LOG_F, length+ LOG_DIAGPKT_OFFSET, 0, NULL);
    }
    else
      /* For any other case, we allocate from diagbuf as the packet is needed by something other than just STM */
#endif
    {
      log_pkt_ptr = (diag_log_rsp_type *)
      diagbuf_alloc(DIAG_LOG_F, length+ LOG_DIAGPKT_OFFSET, 0, DIAGBUF_ALLOC_REQUEST_LOG, 0, 0);
    }

    if (log_pkt_ptr != NULL)
    {
      /* Fill in header first. */
/*lint -save -e545 -e740 Suspicious use of & Unusual pointer cast*/
      diagbuf_write_stream_id (log_pkt_ptr,stream_id);
      hdr_ptr = (log_header_type *) &log_pkt_ptr->log;

      log_set_timestamp_ex(hdr_ptr);
      hdr_ptr->len = (uint16)length;
      hdr_ptr->code = code;

      /* Fill in top of packet. */
      log_pkt_ptr->more = 0;
      log_pkt_ptr->len  = (uint16)length;
      return_ptr = (void *) &log_pkt_ptr->log;
/*lint -restore */
    }
    else
    {
      /* Dropped a log. */
      MSG_1(MSG_SSID_DIAG, MSG_LEGACY_LOW,"Dropped log 0x%x", code);
    }
  } /* if valid and enabled */

  return return_ptr;

} /* log_alloc_core */

/*===========================================================================

FUNCTION LOG_ALLOC_EXT_CORE

DESCRIPTION
  Allocate a buffer of size 'length' for logging data. Supports version field
 
PARAMETERS 
  code                 ID of the log to be reported
  length               The specified length is the length of the entire log, 
                             	including the log header. 
  version              Specifies the version - currently, there are two versions 
                          0 - same as log_alloc() (for backwards compatibility)
                          1 - The outgoing log packet header will have support to
                              include ID field to differentiate the log pkts
                              coming from different instances of the processor.
 *parameter           Based on the version passed in, this pointer is interpreted
                      as follows-
                        version       pointer
                         0             NULL
                         1             uint32 * . The uint32 value this pointer points to,
                                       is the ID of the instance generating the
                                       log packet 
=========================================================================*/
void *
log_alloc_ext_core (log_code_type code, unsigned int length, unsigned int version, void * parameter)
{
  diag_log_rsp_type *log_pkt_ptr; /* Pointer to packet being created */
  log_header_type *hdr_ptr = NULL;
  void *return_ptr = NULL;
  byte stream_id = 0;

  if (length <= sizeof(log_header_type))
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR,"Alloc invalid length %d", length);
  }
  else if ((length + LOG_DIAGPKT_OFFSET) > DIAG_MAX_TX_PKT_SIZ)
  {
    MSG_2(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Attempt to allocate %d but Max Length of a log packet supported is %d", (length + LOG_DIAGPKT_OFFSET), DIAG_MAX_TX_PKT_SIZ);
  }
  else
  {
    stream_id = log_status_adv(code);
  }
  if (stream_id > 0)
  {
    switch (version)
    {
      case DIAGBUF_VERSION_0:
        return log_alloc(code, length);
        break;
      case DIAGBUF_VERSION_1:
      case DIAGBUF_VERSION_2:
      {
        if (parameter == NULL)
        {
          return NULL;
        }

#ifdef FEATURE_DIAG_STM
        /* Check is STM enabled and that DIAG_STREAM_STM is the *only* stream */
        if( (diag_stm_enabled) && (stream_id == DIAG_STREAM_STM))
        {
          /* When STM is enabled, we need to allocate packets differently if they are needed
             by listeners. If listeners are active for this log, stream_id will also contain
             DIAG_STREAM_LISTENERs bit, which can be used to distinguish it from other types
             of data.

           In this use case, we detect if *only* STM is enabled (i.e. no listeners) and then allocate
           as an STM only packet.
          */
          log_pkt_ptr = (diag_log_rsp_type *) 
          diag_stm_log_alloc(DIAG_LOG_F, length+ LOG_DIAGPKT_OFFSET, version, parameter);
        }
        else
          /* For any other case, we allocate from diagbuf as the packet is needed by something other than just STM */
#endif
        {
          log_pkt_ptr = (diag_log_rsp_type *)diagbuf_alloc_chain(DIAG_LOG_F, length + LOG_DIAGPKT_OFFSET, 0, DIAGBUF_ALLOC_REQUEST_LOG, 1, version, parameter, stream_id);
        }
        if (log_pkt_ptr != NULL)
        {
          // Fill in header first.
          //lint -save -e545 -e740 Suspicious use of & Unusual pointer cast
          diagbuf_write_stream_id (log_pkt_ptr,stream_id);
          hdr_ptr = (log_header_type *)&log_pkt_ptr->log;
          log_set_timestamp_ex(hdr_ptr);
          hdr_ptr->len = (uint16)length;
          hdr_ptr->code = code;

          // Fill in top of packet.
          log_pkt_ptr->more = 0;
          log_pkt_ptr->cmd_code = DIAG_LOG_F;
          log_pkt_ptr->len  = (uint16)length;
          return_ptr = (void *)&log_pkt_ptr->log;
        }
      }
        break;
      default:
        break;
    }
  }
  return return_ptr;
} /* log_alloc_ext_core */

/*=========================================================================*/
void log_shorten_core (
  void * ptr,
  unsigned int length
)
{
  byte *diag_pkt = (byte *) ptr; /* use byte* for pointer arithmetic */
  diag_log_rsp_type *diag_log;

  if (ptr)
  {
    diag_pkt -= LOG_DIAGPKT_OFFSET;
    diag_log = (diag_log_rsp_type *) diag_pkt;

    if (length < diag_log->len)
    {
      diagbuf_header_type *pkt_hdr;
      pkt_hdr = ((diagbuf_header_type *) diag_pkt - 1);
      if (pkt_hdr->status != DIAGBUF_STM_BUFFER_S)
      {
        // Do not call diagbuf_shorten if allocated from STM heap
        diagbuf_shorten ((void *) diag_log, length + LOG_DIAGPKT_OFFSET);
      }
      else
      {
        /* Ensure STM pkt headers are updated correctly with the new length */
        /* Should work for all Diag STM log allocations (including ext packets)
           so long as they contain a single diagbuf_header_type reference */
        pkt_hdr->length = length + LOG_DIAGPKT_OFFSET;
      }

      /* Set the log packet length to the new length */
      diag_log->len = (uint16)length;

      /* log_set_length takes the log itself, not the log packet */
      log_set_length (ptr, length);
    }
  }
} /* log_shorten */



/*=========================================================================*/
void log_commit_core (void * ptr)
{
  diagbuf_header_type *pkt_hdr = NULL;
  void * ptr_first = NULL;
  /*pointer to the first packet in case of chaining*/
  void *diagpkt_ptr;

  if (ptr)
  {
    log_commit_last = (void *) ptr;

    /* Set pointer to begining of diag pkt, not the log */
    ptr = ((byte *) ptr - LOG_DIAGPKT_OFFSET);
    diagpkt_ptr = ptr;
    pkt_hdr = ((diagbuf_header_type *) diagpkt_ptr - 1);

#ifdef FEATURE_DIAG_STM

    if (diag_stm_enabled)
    {
      byte stream_id;
      stream_id = diagbuf_read_stream_id(diagpkt_ptr);

      if (stream_id & DIAG_STREAM_STM)
      {
        /* Need special case handling if this is a chained (extended) log packet */
        if (pkt_hdr->chain == DIAGBUF_SECOND_PKT_CHAIN)
        {
          diag_stm_log_ext_handler(pkt_hdr);

          /* Log now sent by STM, so clear STREAM_STM bit so we don't double send */
          /* Clear stream_id for BOTH chained items */
          stream_id &= ~DIAG_STREAM_STM;
          diagbuf_write_stream_id(diagpkt_ptr,stream_id);

          ptr_first = pkt_hdr - diagbuf_ver_len[DIAGBUF_VERSION_1-1];
          diagbuf_write_stream_id(ptr_first, stream_id);
        }
        else
        {
          diag_stm_write_pkt(diagpkt_ptr,pkt_hdr->length);
          /* Log already handled by STM, now clear STREAM_ID field so we don't double send */
          stream_id &= ~DIAG_STREAM_STM;
          diagbuf_write_stream_id(diagpkt_ptr,stream_id);
        }
      }
    }

    // Check to see where this buffer was allocated from
    // If STM, then free buffer now.
    // If diagbuf, then commit to diagbuf
    if (pkt_hdr->status == DIAGBUF_STM_BUFFER_S)
    {
      diag_stm_log_free(diagpkt_ptr);
    }
    else
#endif /* FEATURE_DIAG_STM */
    {

      /*Check if this is an extended log packet */
      if(((pkt_hdr->version == DIAGBUF_VERSION_1) || (pkt_hdr->version == DIAGBUF_VERSION_2))
          && (pkt_hdr->chain == DIAGBUF_SECOND_PKT_CHAIN))
      {

        /* Extended log packets: Commit the second packet first in case diagbuf_drain() 
          is called in between the two commits in this function; this is to prevent
          the scenario where the first packet is committed and the second one is not */
        diagbuf_commit (ptr);

        /*Call diagbuf commit on the first*/
        ptr_first = pkt_hdr - diagbuf_ver_len[pkt_hdr->version-1];
        diagbuf_commit(ptr_first);
      }
      else
      {
        diagbuf_commit(ptr);
      }
    }
  }

  return;
} /* log_commit_core */

/*=========================================================================*/
void log_free_core (void * ptr)
{
  byte *pkt_ptr = (byte *) ptr;
#ifdef FEATURE_DIAG_STM
  diagbuf_header_type *pkt_hdr;    /* Store the header in local variable     */
#endif

  if (ptr == NULL)
    return;

  pkt_ptr -= LOG_DIAGPKT_OFFSET;

#ifdef FEATURE_DIAG_STM

  //Check status to see if this is an STM only buffer. If so, use STM free function instead
  pkt_hdr = ((diagbuf_header_type *) pkt_ptr - 1);

  if ((pkt_hdr != NULL) && (pkt_hdr->status == DIAGBUF_STM_BUFFER_S))
  {
    diag_stm_log_free(pkt_ptr);
  }
  else
#endif
  {
    diagbuf_shorten(pkt_ptr, 0);
  }

} /* log_free_core */

/*=========================================================================*/
boolean log_submit_core (void * ptr)
{
  diag_log_rsp_type *log_pkt_ptr = NULL;
  /* The header is common to all logs, and is always at the beginning of the
   * packet. */
  log_header_type *log_ptr = (log_header_type *) ptr;

  boolean bReturnVal = FALSE;

  /* Local vars to avoid use of misaligned variables */
  log_code_type code;
  unsigned int length;
  byte stream_id = 0;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Make sure argument is valid */
    if (log_ptr != NULL) 
    {
      code = log_ptr->code;
      length = log_ptr->len;

      if (length < sizeof(log_header_type))
      {
        MSG_1(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Alloc invalid length %d", length);
      }
      else if ((length + LOG_DIAGPKT_OFFSET) > DIAG_MAX_TX_PKT_SIZ)
      {
        MSG_2(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Attempt to allocate %d but Max Length of a log packet supported is %d", (length + LOG_DIAGPKT_OFFSET), DIAG_MAX_TX_PKT_SIZ);
      }
      else
      {
        stream_id = log_status_adv(code);
      }

      if (stream_id > 0)
      {
#ifdef FEATURE_DIAG_STM
        /* Check is STM enabled and that DIAG_STREAM_STM is the *only* stream */
        if( (diag_stm_enabled) && (stream_id == DIAG_STREAM_STM))
        {
          /* When STM is enabled, we need to allocate packets differently if they are needed
             by listeners. If listeners are active for this log, stream_id will also contain
             DIAG_STREAM_LISTENERs bit, which can be used to distinguish it from other types
             of data.

             In this use case, we detect if *only* STM is enabled (i.e. no listeners)
             and then allocate as an STM only packet.
           */
        log_pkt_ptr = (diag_log_rsp_type *) 
        diag_stm_log_alloc(DIAG_LOG_F, length + LOG_DIAGPKT_OFFSET, 0, NULL);
        }
        else
          /* For any other case, we allocate from diagbuf as the packet is
             needed by something other than just STM */
#endif
        {
        log_pkt_ptr = (diag_log_rsp_type *) diagbuf_alloc (DIAG_LOG_F, 
          LOG_DIAGPKT_OFFSET + length, 0, DIAGBUF_ALLOC_REQUEST_LOG, 0, 0);
        }

        if (log_pkt_ptr != NULL) {
          diagbuf_write_stream_id(log_pkt_ptr,stream_id);
          log_pkt_ptr->more = 0;
          log_pkt_ptr->len  = (uint16)length;
/*lint -save -e545 -e685 -e669 Suspicious use of & */
          memscpy (&log_pkt_ptr->log, length, (void *) ptr, length);
/*lint -restore */

          /* Commit log to buffer */
          log_commit(&log_pkt_ptr->log);
          bReturnVal = TRUE;
        } else {
          /* Dropped a log */
          MSG_1(MSG_SSID_DIAG, MSG_LEGACY_LOW,"Dropped log 0x%x", code);
        }
      }
    }
  return bReturnVal;

} /* log_submit_core */

/*=========================================================================*/
boolean log_submit_ext_core (void * ptr, unsigned int version, void * parameter)
{
  diag_log_rsp_type * log_pkt_ptr = NULL;
  log_header_type *hdr_ptr = (log_header_type *) ptr;
  log_code_type code;
  unsigned int length;
  byte stream_id = 0;
  boolean bReturnVal = FALSE;

  if ((hdr_ptr == NULL) || ((version >= DIAGBUF_VERSION_1) && (parameter == NULL)) || (version > DIAGBUF_MAX_VERSION_NUM))
  {
    return FALSE;
  }

  code = hdr_ptr->code;
  length = hdr_ptr->len; 
  
   if (length <= sizeof(log_header_type))
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR,"Alloc invalid length %d", length);
  }
  else if ((length + LOG_DIAGPKT_OFFSET) > DIAG_MAX_TX_PKT_SIZ)
  {
    MSG_2(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Attempt to allocate %d but Max Length of a log packet supported is %d", (length + LOG_DIAGPKT_OFFSET), DIAG_MAX_TX_PKT_SIZ);
  }
  else
  {
    stream_id = log_status_adv(code);
  }

  if(stream_id > 0)
  {
    switch (version)
    {
      case DIAGBUF_VERSION_0:
        bReturnVal = log_submit_core(ptr);
        break;

      case DIAGBUF_VERSION_1:
      case DIAGBUF_VERSION_2:
#ifdef FEATURE_DIAG_STM
        /* Check is STM enabled and that DIAG_STREAM_STM is the *only* stream */
        if( (diag_stm_enabled) && (stream_id == DIAG_STREAM_STM))
        {
          /* When STM is enabled, we need to allocate packets differently if they are needed
           by listeners. If listeners are active for this log, stream_id will also contain
           DIAG_STREAM_LISTENERs bit, which can be used to distinguish it from other types
           of data.

           In this use case, we detect if *only* STM is enabled (i.e. no listeners)
           and then allocate as an STM only packet.
         */
          log_pkt_ptr = (diag_log_rsp_type *) 
          diag_stm_log_alloc(DIAG_LOG_F, length + LOG_DIAGPKT_OFFSET, version, parameter);
        }
        else
          /* For any other case, we allocate from diagbuf as the packet is
           needed by something other than just STM */
 #endif
        {
          log_pkt_ptr = (diag_log_rsp_type *)diagbuf_alloc_chain(DIAG_LOG_F, length + LOG_DIAGPKT_OFFSET, 0, DIAGBUF_ALLOC_REQUEST_LOG, 1, version, parameter, stream_id);
        }

        if (log_pkt_ptr != NULL)
        {
          // Fill in header first.
          diagbuf_write_stream_id(log_pkt_ptr,stream_id);
          log_pkt_ptr->cmd_code = DIAG_LOG_F;
          log_pkt_ptr->more = 0;
          log_pkt_ptr->len  = (uint16)length;
          memscpy(&log_pkt_ptr->log, length, (void *)ptr, length);

          /* Commit log to buffer. log_commit will handle extended packet cases */
          log_commit(&log_pkt_ptr->log);
          bReturnVal = TRUE;
        }
        else
        {
          /* Dropped a log */
          MSG_1(MSG_SSID_DIAG, MSG_LEGACY_LOW,"Dropped log 0x%x", code);
        }
        break;

      default:
        MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR,"Invalid version %d in log_submit_ext", version);
        break;
    } //end switch
  }

  return bReturnVal;

} /*log_submit_ext*/

/*===========================================================================

FUNCTION LOG_LISTENER_MATCH_AND_FREE

DESCRIPTION
  This routine is passed to and called by diag_searchlist_delete() for each
  node of the search until this routine returns TRUE or the search is
  exhausted.  This routine is responsible for freeing the 'entry' which
  was allocated in diag_add_log_listener().

===========================================================================*/
static boolean
log_listener_match_and_free (diag_searchlist_node_type *match_node,
                             diag_searchlist_node_type *list_node)
{
  boolean found = FALSE;
  log_listener_entry_type *match = NULL;
  log_listener_entry_type *entry = NULL;

  if (match_node && list_node)
  {
    match = (log_listener_entry_type *) match_node->goo;
    entry = (log_listener_entry_type *) list_node->goo;

    if (match->listener == entry->listener &&
        match->param == entry->param)
    {
      found = TRUE;
      diag_listeners_free (entry);

#ifdef FEATURE_DEBUG_DIAG_HEAP
      log_listener_match_and_free_cnt++;
      MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
        "FREE log_listener_match_and_free_cnt = %d ",
                    log_listener_match_and_free_cnt);
#endif /* FEATURE_DEBUG_DIAG_HEAP */

    }
  }

  return found;
}

/*=========================================================================*/
boolean
diag_add_log_listener (const unsigned int log_code,
                       const diag_log_listener listener, void *param)
{
  log_listener_entry_type *entry = NULL;
  boolean success = FALSE;
  unsigned int id, item;

  /* Check identity of log item being requested */
  id = LOG_GET_EQUIP_ID (log_code);
  item = LOG_GET_ITEM_NUM (log_code);
  if (id > LOG_EQUIP_ID_LAST || item > log_last_item_tbl[id])
    return FALSE;

  entry = (log_listener_entry_type *)
    diag_listeners_malloc (sizeof (log_listener_entry_type));


  if (entry)
  {

#ifdef FEATURE_DEBUG_DIAG_HEAP
    diag_add_log_listener_malloc_cnt++;
    MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
        "MALLOC diag_add_log_listener_malloc_cnt = %d, Bytes Allocated %d",
        diag_add_log_listener_malloc_cnt, sizeof (log_listener_entry_type));
#endif /* FEATURE_DEBUG_DIAG_HEAP */

    entry->listener = listener;
    entry->param = param;

    success = diag_searchlist_add (&log_listeners, log_code, entry);

    if (success && log_listeners.initialized)
    {
      osal_enter_crit_sect (&log_listeners.crit_sect);

      if (log_listeners.count > 0)
      {
        DIAG_SET_MASK (diag_cx_state, DIAG_CX_LOG_LISTEN_S);
      }

      osal_exit_crit_sect (&log_listeners.crit_sect);

      /* Set log listener mask bit */
      log_mask_set_bit(log_listener_mask, id, item, TRUE);
    }
    else
    {
      /* unable to add, free 'entry' */
      diag_listeners_free ((void *) entry);

#ifdef FEATURE_DEBUG_DIAG_HEAP
      diag_add_log_listener_free_cnt++;
      MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH,
        "Unable to add log listener, freeing, \
         diag_add_log_listener_free_cnt = %d",
                    diag_add_log_listener_free_cnt);
#endif /* FEATURE_DEBUG_DIAG_HEAP */
    }
  }

  return success;
}

/*=========================================================================*/
boolean
diag_remove_log_listener (const unsigned int log_code,
                          const diag_log_listener listener, void *param)
{
  log_listener_entry_type entry;
  diag_searchlist_node_type node;
  boolean success = FALSE;
  boolean status = FALSE;
  uint32 id     = 0;
  uint32 item   = 0;

  entry.listener = listener;
  entry.param = param;

  node.key = log_code;
  node.goo = (void *) &entry;


  success = diag_searchlist_delete (&log_listeners, node,
                                    log_listener_match_and_free);

  if (success && log_listeners.initialized)
  {
    osal_enter_crit_sect (&log_listeners.crit_sect);

    if (log_listeners.count == 0)
    {
      DIAG_CLR_MASK (diag_cx_state, DIAG_CX_LOG_LISTEN_S);
    }

    osal_exit_crit_sect (&log_listeners.crit_sect);

    /* Clean up listener mask */
    status = diag_searchlist_search_all (log_code, &log_listeners, NULL, NULL);
    if(status == FALSE)
    {
      id = LOG_GET_EQUIP_ID (log_code);
      item = LOG_GET_ITEM_NUM (log_code);
      if (id <= LOG_EQUIP_ID_LAST && item <= log_last_item_tbl[id])
      {
        log_mask_set_bit(log_listener_mask, id, item, FALSE);
      }
    }
  }

  return success;
}

/*===========================================================================

FUNCTION LOG_LISTENER_NOTIFY

DESCRIPTION
  This routine is called by the searchlist when a matching listener node is
  found.  It is called once per entry.

  This routine is a wrapper to call the registered listeners.

===========================================================================*/
void
log_listener_notify (uint32 seq_num, void *log, diag_searchlist_node_type *node)
{
  log_listener_entry_type *entry;

  if (node)
  {
    entry = (log_listener_entry_type *) node->goo;

    if (entry->listener)
    {
      entry->listener (seq_num, log, log_get_length (log), entry->param);
    }
  }
}

/*===========================================================================

FUNCTION LOG_ON_DEMAND_REGISTER

DESCRIPTION
  This function registers a function pointer to be associated with a
  log code for logging on demand.  If the external device sends a request
  to trigger this log code, the function will be called.  The logging
  must be performed by the client of this service.  It will not be
  performed by the logging service itself.

===========================================================================*/
boolean log_on_demand_register (
  log_code_type log_code,
  log_on_demand log_on_demand_ptr
)
{
  int i;
  boolean status = FALSE;
  int8 first_avail_idx = -1;

  if (!diag_services_initialized)
    return status;

  osal_lock_mutex(&log_on_demand_mutex);


  for( i=0; i<LOG_ON_DEMAND_SIZE; i++ )
  {
    if ((first_avail_idx == -1) && (log_on_demand_list[i].log_on_demand_ptr == NULL))
    {
      first_avail_idx = i;
    }
    if(log_on_demand_list[i].log_code == log_code)
    {
      status = TRUE;
      break;
    }
  }

  if ((status == FALSE) && (first_avail_idx != -1))
  {
    log_on_demand_list[first_avail_idx].log_code = log_code;
    log_on_demand_list[first_avail_idx].log_on_demand_ptr = log_on_demand_ptr;
    status = TRUE;
  }

  osal_unlock_mutex(&log_on_demand_mutex);

  return status; /* buffer full */

}  /* log_on_demand_register */


/*===========================================================================

FUNCTION LOG_ON_DEMAND_UNREGISTER

DESCRIPTION
  This function unregisters the log function

===========================================================================*/
boolean log_on_demand_unregister (
  log_code_type log_code
)
{
  int i;
  boolean status = FALSE;

  if(!diag_services_initialized)
    return status;

  osal_lock_mutex(&log_on_demand_mutex);

  for( i=0; i<LOG_ON_DEMAND_SIZE; i++ )
  {
    if(log_on_demand_list[i].log_code == log_code)
    {
      log_on_demand_list[i].log_code = 0;
      log_on_demand_list[i].log_on_demand_ptr = NULL;
      status = TRUE;
      break;
    }
  }

  osal_unlock_mutex(&log_on_demand_mutex);

  return status; /* not found */

}  /* log_on_demand_unregister */
