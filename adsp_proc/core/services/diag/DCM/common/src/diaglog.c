/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          Logging Services

General Description
  Services to support data logging.

Initializing and Sequencing Requirements
  'log_init()' must be called once during initialization prior to use.

Copyright (c) 1990-2014, 2016-2017 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*==========================================================================

                              Edit History

$Header: //components/rel/core.qdsp6/2.1/services/diag/DCM/common/src/diaglog.c#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/21/17   ph      Tag DIAG ID header to all the diag packets sent out.
08/29/17   gn      Changes to handle remote socket close
06/08/17   gn      Added support for diag services to uImage
06/05/17   nk      Added debug support for uimage in root PD
05/03/17   sp      Resolved KW errors
04/09/17   sp      Added support for extended logs in rootPD
10/24/16   gn      Changes to support common PW for ADSP, CDSP and SLPI
05/12/16   ph      SMD references removal and code clean up.
10/31/14   rs      Replaced EUIP_ID_MAX with LOG_EQUIP_ID_MAX
07/06/14   ph      Added support for Diag over STM
04/01/14   rs      Added support for build mask centralization.
03/05/14   xy      Fixed KW issues  
02/19/14   rh      Calls to log_update_composite_mask() use correct params now
12/03/13   ph      Update log_composite_mask algorithm to avoid possible concurrency issues.
11/27/13   ph      Updated MSG 1.0 API references to MSG.2.0.
11/22/13   sr      Removed PACK from pointer references
09/27/13   xy      Removed code under FEATURE_WINCE
09/06/13   sa      Fixed issues responding to V1/V2 Diag Test Commands
07/05/13   sr      Fixed Klockwork warnings
04/81/13   ph      Replace memcpy() and memmove() with memscpy() and memsmove()
02/01/13   sg      Fixed Klock work warnings
02/06/13   rh      Added composite (aggregated) log mask optimization
02/02/12   sr      Fixed the for loop condition check in log_sleep_vote()
02/11/13   sr      Mainling Diag central routing
12/07/12   is      Warnings fixes
11/15/12   is      Support for preset masks
09/22/12   sr      Support for mask rtvl command
10/25/11   is      Fixed passing in proper stream_id in log_config_mask()
10/07/11   is      Support sending log mask per equipment id
09/19/11   is      Optimized sending F3 "set mask" requests from Master Diag
09/15/11   is      Modifed "set mask" via CTRL msgs fot EA support
09/12/11   is      Master Diag forwards "set mask" requests via CTRL msgs
03/18/11   hm      Diag sleep vote bug fix
03/09/11   hm      Dual mask bug fix
12/20/10   mad     Changes for dual mask
12/21/10   hm      Dual mask changes
08/10/10   sg      Moved log_config_mask to diaglogi.h
09/17/10   is      Migrate from deprecated clk driver APIs
08/25/10   sg      Fixed compiler warnings
02/20/10   sg      Merging back DCM WM files.Moved api
                   functionality to log_api.c on AMSS
05/06/10   is      Removed obsolete LOG_CDMA_TIME_FORMAT
02/12/10   vs      Ported diag_time_get from depot
12/28/09   ps      Fixed Klocwork errors & compiler warnings.
11/04/09   cahn    DIAG Health
09/09/09   JV      Changed signature of osal mutex calls.
08/05/09   JV      Removed the CUST_H featurization around the inclusion of
                   customer.h.
07/10/09   as      Decoupled code under FEATURE_DIAG_NON_STREAMING
05/12/09   JV      Introduced the OS abstraction layer for rex.
05/07/09   vk      changed includes from external to internal headers
04/22/09   JV      Cleaned up featurization for timestamps.
10/03/08   vg      Updated code to use PACK() vs. PACKED
                   Removed OR of Frame number into WCDMA packets
12/15/06   as      Fixed compiler warnings.
07/13/06   as      Added RPC support for mobileview
05/19/06   as      Added Multi processor modem only support
10/31/05   as      Fixed lint errors.
10/10/05   as      Changed implementation to process configuration files
                   without sending responses.
09/29/05   as      Added check for NULL pointer after diagbuf allocation.
03/09/05   as      Fixed log_on_demand_register to avoid logcode to become
                   double registered.
02/10/05   as      Added FEATURE_DEBUG_DIAG_HEAP to debug memory corruption.
03/02/04   as      Corrected the boundary in Log Set Mask Response.
10/28/03   as      Changes to fix errors when compiling with RVCT2.0
07/30/02   as      Updated log_get_pkt to support FEATRUE_DIAG_NON_STREAMING.
07/30/02   lad     Added default values for last items for each ID in case
                   they are not defined in diagtune.h.
03/18/02   gr      Modified log_set_timestamp to call log_set_timestamp_ex.
03/15/02   gr      Added code to stuff frame number bits into the timestamp
                   field of the log header for WCDMA.
01/18/02   as      Added implementation for Log On Demand
01/17/02   lad     Fixed array size for log_mask_offset_tbl[] and
                   log_nice_offset_tbl[].
10/24/01   jal     Cleaned up some compiler warnings.
09/12/01   lad     Fixed log_process_config_pkt to correctly set log masks.
06/27/01   lad     Overhauled logging service to support multiple equipment
                   IDs and general cleanup.
                   Added FEATURE_LOG_EQUIP_ID_XLATE to allow old tools to
                   set log codes with equip ID != 1 using temp equip ID 1
                   codes.
04/17/01   lad     Added use of log_code_type instead of uint16.
04/06/01   lad     Changed use of FEATURE_DIAG_MSP_BUILD to FEATURE_ZREX_TIME.
02/23/01   lad     Updated for diag re-architecture.
12/05/00   lad     Added new logging API.
11/10/00   jq      Added support for analog logging.
11/02/00   lad     Makeover + API enhancements.
08/31/00   lad     Optimized logging API funcs.
                   Added logmask processing and logmask diag pkt functions.
07/31/00   lad     Added MSM5000_IRAM_FWD for log_submit().
05/22/00   lad     Added valid mask support for extended logmask.
05/15/00   lad     Changed log_nice default to 0.
05/10/00   lad     Added load balancing support.  Updated diagbuf API.
04/17/00   lad     FEATURE_DIAG_V7_LOG_COMPATIBILITY to use this diag with
                   old DMSS targets.
04/11/00   lad     Moved log_status to log.h as INLINE function.
02/28/00   lad     Made changes to use diagbuf allocation manager.
11/05/98   lad     Made necessary changes for ARM port.
05/03/95   rdh     Added log_status function.
08/31/92   gb      DMSS ver.
03/05/91   rdb     Created module.

===========================================================================*/

#include "comdef.h"

#include "customer.h"

#include "diagbuf_v.h"
#include "diagcmd.h"
#include "diagi_v.h"
#include "diagpkt.h"
#include "diagdiag_v.h"
#include "diagtarget.h"
#include "diagtune.h"
#include "log.h"
#include "msg.h"
#include "qw.h"
#include "osal.h"
#include "diagcomm_v.h"
#include "diag_cfg.h"
#include "diagcomm_ctrl.h" /* For DIAG_CTRL_MSG_EQUIP_LOG_MASK */
#include "stringl.h"
#include "ULogFront.h"
#include "diagcomm_cmd.h"  /* For diagcomm_cmd_status */
#include "diag_shared_i.h"

#if defined (DIAG_QDSP6_APPS_PROC)
#include "diagstub.h"
#else
#include "assert.h"
#endif

#include <string.h>
#include <stdlib.h>

#include "diaglogi.h"
#include "log_arrays_i.h"
#include "diag_micro_i.h" /*For log_mask_enabled()*/
osal_mutex_arg_t         log_on_demand_mutex;

extern void log_listener_notify (uint32 seq_num, void *log, diag_searchlist_node_type *node);


extern diagcomm_io_conn_type diagcomm_io_ctrl_conn[NUM_PORT_TYPES][NUM_REMOTE_PORTS]; /* From diagcomm_io.c */

/*Hack to handle extended log messages*/
boolean log_ext_hdr_start=FALSE;

/* -------------------------------------------------------------------------
 * Definitions and Declarations
 * ------------------------------------------------------------------------- */

extern uint8 diag_cur_preset_id; /* from diag.c */

/* The log_on_demand list holds the entries for log_code and
 * function pointer. */
log_on_demand_list_item_type log_on_demand_list[LOG_ON_DEMAND_SIZE];

/* This array contains the log masks for all equipment IDs. */
/*lint -save  -e572 */
byte * log_mask[DIAG_MAX_STREAM_ID];
byte diag_log_preset_mask[DIAG_MAX_PRESET_ID][LOG_MASK_SIZE];    /* Stream 1 QXDM masks. */
byte diag_log_dci_mask[LOG_MASK_SIZE];                           /* Stream 2 DCI mask. */

extern byte log_composite_mask[LOG_MASK_SIZE];


extern byte log_listener_mask[LOG_MASK_SIZE];

typedef struct{
  uint8 cmd_code; /* cmd code for extended pkts */
  uint8 version;  /* version of the extended pkt; currently we support version 1 */
  uint16 proc_id; /* processor ID where the msg is being generated */
  uint32 id;      /* ID passed in by user */
}log_ext_hdr_type;

typedef struct{
  uint8 cmd_code; /* cmd code for extended pkts */
  uint8 version;  /* version of the extended pkt; currently we support version 1 */
  uint16 proc_id; /* processor ID where the msg is being generated */
  uint8 name_len; /*  Length of Source DiagBuffer  */
  uint8 time_offset_type; /*Time offset type for DiagBuffer Content */
  uint64 time_offset; /*Time offset, based on TIME_OFFSET_TYPE*/
  uint8 ulog_name[ULOG_MAX_NAME_SIZE + 1]; /*Null Terminated String with Name of DiagBuffer*/
}log_ext_hdr_type_v2;

static diag_ctrl_info_fwd_status bt_log_range_fwd_status;


static const byte log_valid_mask[LOG_MASK_SIZE];
/*lint -restore */

/* The content of this table are computed in log_init().  'uint16' is used
 * instead of 'int' to reduce data bus cycles to access an element of this
 * array.  The last entry is the offset to the byte following the array.  It
 * is used for range checking. */
extern uint16 log_mask_offset_tbl[LOG_EQUIP_ID_LAST+1];


extern log_state_enum_type log_state;
void *log_commit_last = NULL; /* Many writers, 1 reader (DIAG) */
void *log_flush_last = NULL; /* 1 writer, 1 reader (both DIAG) */


extern diag_searchlist_type log_listeners;

/* -------------------------------------------------------------------------
 * Internal (aka private) function prototypes
 * ------------------------------------------------------------------------- */
boolean log_status_mask (log_code_type code, byte stream_id);
static boolean log_search_mask (unsigned int id, unsigned int item, byte stream_id);
void log_update_composite_mask(uint32 start, uint32 end);



static log_code_type log_set_equip_id (unsigned int code,
                                       unsigned int equip_id);

static boolean log_sleep_vote (void);
static void log_service_outbound (diag_send_desc_type *desc,byte stream_id);

void * diaglog_process_ext_config_req (void * req_pkt_ptr, uint16 pkt_len);

/* -------------------------------------------------------------------------
 * External (aka public) Function Implementations
 * ------------------------------------------------------------------------- */
/*===========================================================================

FUNCTION LOG_ON_DEMAND_LIST_INIT

DESCRIPTION
  Initialize log on demand list.

===========================================================================*/
static void log_on_demand_list_init (void)
{
  int i;
  for (i = 0; i < LOG_ON_DEMAND_SIZE; i++)
  {
    log_on_demand_list[i].log_code = 0;
    log_on_demand_list[i].log_on_demand_ptr = NULL;
  }
}


/*===========================================================================

FUNCTION LOG_INIT

DESCRIPTION
  Initialize log masks.

RETURN VALUE
  None

===========================================================================*/
void log_init (void)
{
  int id, return_val = 0;

  ASSERT(sizeof(log_header_type) == sizeof(log_hdr_type));
  ASSERT(LOG_EQUIP_ID_LAST <= LOG_EQUIP_ID_MAX);

  log_mask_offset_tbl[0] = 0;

  for (id = 0; id < LOG_EQUIP_ID_LAST; id++)
  {
    log_mask_offset_tbl[id+1] = 1 + log_mask_offset_tbl[id] +
      LOG_MASK_ARRAY_INDEX(log_last_item_tbl[id]);
  }

  /* Point local masks to initial preset masks. */
  log_mask[DIAG_STREAM_1-1] = &diag_log_preset_mask[diag_cur_preset_id-1][0];
  log_mask[DIAG_STREAM_2-1] = &diag_log_dci_mask[0];
  
  bt_log_range_fwd_status = DIAG_CTRL_INFO_FWD_STATUS_NOT_SENT;


  /* These masks are cleared with memset because the listeners do not have a
     config function like log_config_mask to do so, nor is there a listener init. */
  memset(log_composite_mask, 0, sizeof(log_composite_mask));
  memset(log_listener_mask, 0, sizeof(log_listener_mask));
#if !defined(T_WINNT)
  (void) log_config_mask (-1, FALSE, DIAG_STREAM_ALL, DIAG_PRESET_MASK_ALL);
  diag_sleep_vote (DIAG_SLEEP_LOG, TRUE);
#else
  /* Unit test environments default to on */
  log_config_mask (-1, TRUE, DIAG_STREAM_1, DIAG_PRESET_MASK_ALL);
  diag_sleep_vote (DIAG_SLEEP_LOG, FALSE);
#endif

  log_on_demand_list_init();

  (void) diagbuf_register_outbound_pkt_handler (DIAG_LOG_F, log_service_outbound);

  log_on_demand_mutex.name = "MUTEX_LOG_ON_DEMAND";
  return_val = osal_init_mutex(&log_on_demand_mutex);
  ASSERT(OSAL_SUCCESS == return_val);

  /* Now that init is complete, enable normal operation */
  log_state = LOG_NORMAL_STATE;

} /* log_init */


/*===========================================================================

FUNCTION LOG_CONFIG_MASK

DESCRIPTION
  This function selects which log codes are enabled for logging.

PARAMETERS
  code - Specifies which log code is being referenced. Should be one of
         the enumerated log codes defined in log_codes.h.
           -1 means entire mask.
  flag - Specifies whether to enable (TRUE) or disable (FALSE) logging
         for the specified log code.
  stream_id - Specifies which stream to enable/disable.
               Valid stream_id range: DIAG_MIN_STREAM_ID to DIAG_MAX_STREAM_ID
               Set all streams: DIAG_STREAM_ALL
  preset_id - Specifies preset_id for Stream 1 log mask.
              Valid preset_id range: DIAG_MIN_PRESET_ID to DIAG_MAX_PRESET_ID
              Set all presets: DIAG_PRESET_MASK_ALL

RETURN VALUE
  TRUE if the operations was succesful.
  FALSE if an invalid code was specified.

WARNING
  Please update log_config_mstr_mask() if log_config_mask() is updated.

===========================================================================*/
 boolean log_config_mask (int code, boolean  flag, byte stream_id, uint8 preset_id)
{
  boolean return_status = FALSE;
  uint8 preset_index = 0;
  unsigned int id, item;
  unsigned int index, start, end;
  byte mask = (flag) ? 0xFF : 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  start = 0;
  end = LOG_MASK_SIZE -1;

  if(preset_id > DIAG_MAX_PRESET_ID)
  {
    return FALSE;
  }

  if (code == -1) //sets entire mask
  {
    if( (stream_id == DIAG_STREAM_1) || (stream_id == DIAG_STREAM_ALL) )
    {
      if( preset_id == DIAG_PRESET_MASK_ALL )
      {
        for( preset_index=0; preset_index < DIAG_MAX_PRESET_ID; preset_index++ )
        {
          for( index = 0; index < LOG_MASK_SIZE; index++ )
            diag_log_preset_mask[preset_index][index] = mask;
        }
      }
      else
      {
        for( index = 0; index < LOG_MASK_SIZE; index++ )
          diag_log_preset_mask[preset_id-1][index] = mask;
      }
    } //end set stream 1 all

    if( (stream_id == DIAG_STREAM_2) || (stream_id == DIAG_STREAM_ALL) )
    {
      for( index = 0; index < LOG_MASK_SIZE; index++ )
        diag_log_dci_mask[index] = mask;
    } //end set stream 2 all

    return_status = diag_client_maskchange(DIAG_LOGSVC_MASK_CHANGE);
  }
  else if( code > 0 )
  {
    /* Get ID and item from log code */
    id = LOG_GET_EQUIP_ID(code);
    item = LOG_GET_ITEM_NUM(code);
     if(id <= LOG_EQUIP_ID_LAST)
    {
    /* Check if code is valid */
      index = log_mask_offset_tbl[id] + LOG_MASK_ARRAY_INDEX(item);
      mask = LOG_MASK_BIT_MASK(item);
    }
    else
    {
      return_status = FALSE;
    }
  }
  log_update_composite_mask(start, end);
  return return_status;

} /* log_config_mask */

/*===========================================================================

FUNCTION LOG_FLUSH

DESCRIPTION
  Prepares the logging service for a flush.  The latest log that was
  committed is marked to be sent.  All others are discarded.

===========================================================================*/
void
log_flush (void)
{
  if (log_state == LOG_NORMAL_STATE)
  {
    /* log_commit_last has multiple writers and is written prior to signalling
       the DIAG task.  This routine, however, is called from DIAG task only.
       Therefore, DIAG is the only writer for log_flush_last which is
       captured here to mark the last log to send before declaring a flush
       complete.  Once sent, it is sent to NULL by log_service_outbound,
       also by the DIAG task. */
    log_flush_last = log_commit_last;

    if (log_flush_last)
    {
      log_state = LOG_FLUSH_STATE;
    }
  }
}

/*===========================================================================

FUNCTION LOG_PANIC

DESCRIPTION
  Prepares the logging service for panic mode.  All logs are configured off.
  All listeners are deleted.

===========================================================================*/
void
log_panic (void)
{
  if (log_state != LOG_INIT_STATE)
  {
    /* log_commit_last has multiple writers and is written prior to signalling
       the DIAG task.  This routine, however, is called from DIAG task only.
       Therefore, DIAG is the only writer for log_flush_last which is
       captured here to mark the last log to send before declaring a flush
       complete.  Once sent, it is sent to NULL by log_service_outbound,
       also by the DIAG task. */
    log_flush_last = log_commit_last;

    if (log_flush_last)
    {
      log_state = LOG_PANIC_STATE;
    }
    else
    {
      log_state = LOG_INIT_STATE; /* Disable */
    }
  }
}



/*===========================================================================

FUNCTION LOG_STATUS_MASK

DESCRIPTION
  This routine is a wrapper for log_search_mask().  It is used to look up
  the given code in the log mask for the given stream.

RETURN VALUE
  A boolean indicating if the specified log is enabled.

===========================================================================*/
boolean
log_status_mask (log_code_type code, byte stream_id)
{
  unsigned int id, item;
  boolean status = FALSE;

  id = LOG_GET_EQUIP_ID (code);
  item = LOG_GET_ITEM_NUM (code);

  if ( stream_id & DIAG_STREAM_1) {

   status = log_search_mask (id, item, DIAG_STREAM_1);
  }
  if ( stream_id & DIAG_STREAM_2) {

   status |= log_search_mask (id, item, DIAG_STREAM_2);
  }


  return status;
}

/*===========================================================================

FUNCTION LOG_SERVICE_OUTBOUND

DESCRIPTION
  This routine is called by diagbuf when it starts a new DIAG_LOG_F packet.
  This routine handles log listeners as well as sending to the comm layer.

  This routine also handles various 'log_state's that are used for
  preserving listener state while flushing logs to the comm layer.

===========================================================================*/
static void
log_service_outbound (diag_send_desc_type *desc,byte stream_id)
{
  const diag_log_rsp_type *log;
  log_code_type code;
  diag_send_state_enum_type state;
  unsigned int id = 0, item = 0;
  log_ext_hdr_type *log_hdr = NULL;
  log_ext_hdr_type_v2 *log_hdr2 = NULL;
  diagbuf_header_type *diagbuf_hdr1 = NULL;
  static diag_send_desc_type send = {NULL, NULL, DIAG_SEND_STATE_START, FALSE};
  static boolean log_ext_hdr_sent = FALSE;

  if (desc)
  {
    state = desc->state;

    if (state == DIAG_SEND_STATE_START)
    {
      if (log_state == LOG_INIT_STATE)
      {
        /* Skip immediately */
        state = DIAG_SEND_STATE_COMPLETE;
      }
      else
      {
        diagbuf_hdr1 = (diagbuf_header_type *) desc->pkt;

        /*point to the diagbuf header*/
        diagbuf_hdr1 -= 1;

        /* Processing for chained diagbuf entries (extended logs) */
        if((diagbuf_hdr1->version == DIAGBUF_VERSION_1 ) && (diagbuf_hdr1->chain == DIAGBUF_FIRST_PKT_CHAIN))
        {
          if( stream_id != DIAG_STREAM_1 )
          {
            /* Drop extended header if STREAM_1 is not active.
               Not currently supported by listeners or by DCI */
            desc->pkt = (void *)((byte *)desc->last + 1);
            desc->state = DIAG_SEND_STATE_COMPLETE;
            return;
          }

          /* Send out extended packet as needed to STREAM_1 only */
          log_hdr = (log_ext_hdr_type *) desc->pkt;
          log_hdr->cmd_code = DIAG_CMD_EXT_F;
          log_hdr->version = ((log_ext_hdr_type *)desc->pkt)->version;
          log_hdr->proc_id = ((log_ext_hdr_type *)desc->pkt)->proc_id;
          log_hdr->id = ((log_ext_hdr_type *)desc->pkt)->id;
          send.pkt = (void *) &log_hdr->cmd_code;
          send.last = (void *)((uint32)send.pkt + sizeof(log_ext_hdr_type)-1);
          send.terminate = FALSE;
          send.state = DIAG_SEND_STATE_START;
          log_ext_hdr_start=TRUE;

          /* Send extended packet header, but mask out DCI STREAM until it supports extended packets */
          diagbuf_send_pkt(&send, (stream_id & ~DIAG_STREAM_2));
          if (send.state == DIAG_SEND_STATE_COMPLETE)
          {
            log_ext_hdr_start=TRUE;
            desc->pkt = (void *)((byte *)desc->last + 1);
            desc->state = DIAG_SEND_STATE_COMPLETE;
            /* Set flag indicating first portion of extended log packet sent */
            log_ext_hdr_sent = TRUE;
            send.pkt = NULL;
          }
          else
          {
            log_hdr->cmd_code = DIAG_LOG_F;
          }
          return;
        }

        if((diagbuf_hdr1->version == DIAGBUF_VERSION_2 ) && (diagbuf_hdr1->chain == DIAGBUF_FIRST_PKT_CHAIN))
        {
          if( stream_id != DIAG_STREAM_1 )
          {
            /* Drop extended header if STREAM_1 is not active.
               Not currently supported by listeners or by DCI */
            desc->pkt = (void *)((byte *)desc->last + 1);
            desc->state = DIAG_SEND_STATE_COMPLETE;
            return;
          }

          /* Send out extended packet as needed to STREAM_1 only */
          log_hdr2 = (log_ext_hdr_type_v2 *) desc->pkt;
          log_hdr2->cmd_code = DIAG_CMD_EXT_F;
          log_hdr2->name_len = ((log_ext_hdr_type_v2 *)desc->pkt)->name_len;

          send.pkt = (void *) &log_hdr2->cmd_code;
          send.last = (void *)( (byte *)send.pkt + FPOS(diag_cmd_ext_v2_type, ulog_name) + log_hdr2->name_len  - 1 );
          send.terminate = FALSE;
          send.state = DIAG_SEND_STATE_START;

          /* Send extended packet header, but mask out DCI STREAM until it supports extended packets */
          diagbuf_send_pkt(&send, (stream_id & ~DIAG_STREAM_2));
          if (send.state == DIAG_SEND_STATE_COMPLETE)
          {
            desc->pkt = (void *)((byte *)desc->last + 1);
            desc->state = DIAG_SEND_STATE_COMPLETE;
            /* Set flag indicating first portion of extended log packet sent */
            log_ext_hdr_sent = TRUE;
            send.pkt = NULL;
          }

          return;
        }

        log = (diag_log_rsp_type *) desc->pkt;
        code = log_get_code ((void *) log->log);

        /* Check listener masks before searching listener lists */
        id = LOG_GET_EQUIP_ID (code);
        item = LOG_GET_ITEM_NUM (code);

        if (log_mask_enabled(log_listener_mask, id, item)) {
        (void) diag_searchlist_search_all (code,
                                    &log_listeners,
                                    (void *) log->log,
                                    log_listener_notify);
        }

        /* Check if this is the last item for a pending flush operation. */
        if (!log_status_mask (code, stream_id))
        {
          /* Mark complete - don't send to comm layer. */
          state = DIAG_SEND_STATE_COMPLETE;
        }

        /* log_commit_last has multiple writers.  log_commit_last is always
           captured before committing to the queue.  Therefore, it is always
           ahead of log->log in the queue.  Therefore, one can assume that if
           this match succeeds, log_commit_last is no longer pointing to a
           valid item in the queue, and must be cleared. */
        if ((void *) log->log == log_commit_last)
        {
          /* In the off chance DIAG is preempted by a writer of
             'log_commit_last' between the above check and writing NULL here,
         the side effect is only that the log may be ignored by a flush
             operation when it copies it to log_flush_last.  DIAG is the
             only reader of this variable, so there is no risk of capturing
             a corrupt value for log_flush_last when captured from this
             variable.  This is a tolerable side effect.  The only loss is
             that any log generated from a preemption here will eb lost if a
             flush begins before that log is sent.  The tradeoff would be to
             require INTLOCK around this 'if' block. */
          log_commit_last = NULL;
        }

        if (log_flush_last)
        {
          log_flush_last = NULL;

          /* Flush or panic mode will always disable comm bound logs.
             Panic moves to 'init' state to halt the svc when complete. */
          if (log_state == LOG_FLUSH_STATE)
          {
            (void) log_config_mask (-1, FALSE, DIAG_STREAM_ALL, diag_cur_preset_id);
            log_state = LOG_NORMAL_STATE;
          }
          else if (log_state == LOG_PANIC_STATE)
          {
            (void) log_config_mask (-1, FALSE, DIAG_STREAM_ALL, diag_cur_preset_id);
            log_state = LOG_INIT_STATE; /* disable logging */
          }
          else
          {
            MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"flush_last set in wrong state");
          }
        }
      }
    }

    if (state == DIAG_SEND_STATE_COMPLETE)
    {
      /* Skip this packet - no tool asked for it. */
      desc->pkt = (void *) (((uint8 *) desc->last) + 1);
      desc->state = state;
    }
    else
    {
      /* This routine will transition state even if there if no sending can be
         done.  Therefore, this routine can assume that state == "start" will
         only happen once. */
      if( log_ext_hdr_sent )
      {
        desc->state = DIAG_SEND_STATE_BUSY;
        log_ext_hdr_sent = FALSE;
      }
      (void) diagbuf_send_pkt (desc,stream_id);
    }
  }

  return;
}


/* -------------------------------------------------------------------------
 * Internal to diag (aka protected) function prototypes
 * ------------------------------------------------------------------------- */
/* Begin packet processing routines */
/*===========================================================================

FUNCTION LOG_PROCESS_CONFIG_PKT

DESCRIPTION
  This procedure processes a request to set the log mask.

  Once again the log service config packet has evolved.  This time it has
  evolved to be able to handle multiple equipment IDs.  In the past, the MSM
  always assumed the equipment ID is 1.  Multi-mode products, different
  technologies can log with the same service using equipment ID to specify the
  technology.  This means we have 16 masks, one for each equipment ID.

  The extended logmask packet was designed using a very odd bit ordering scheme
  that created a lot of confusion.  To alleviate this confusion, the logmask is
  now a byte array, with each bit being ordered LSB to MSB.  Macros are defined
  to perform these operations.

PARAMETERS
  req_pkt_ptr - Pointer to request packet
  pkt_len - Length of request packet

RETURN VALUE
  Pointer to response packet.

  This command effects what type of entries are logged.
===========================================================================*/
void * log_process_config_pkt (
  void * req_pkt_ptr,
  uint16 pkt_len
)
{
  /* For cmd DIAG_LOG_CONFIG_F 115 - Supports Stream 1 */
  DIAG_LOG_CONFIG_F_req_type *req_ptr = (DIAG_LOG_CONFIG_F_req_type *) req_pkt_ptr;
  DIAG_LOG_CONFIG_F_rsp_type *rsp_ptr = NULL;
  /* For Diag subsys cmd DIAG_EXT_LOG_CONFIG 75 18 98 - Supports all streams */
  log_ext_config_req_type *req_dm_ptr = NULL;
  log_ext_config_rsp_type *rsp_dm_ptr = NULL;

  /* Pointers to unions for operation specific data (For non-dual-mask Packets)*/
  log_config_op_req_type *req_op_ptr = NULL;
  log_config_op_rsp_type *rsp_op_ptr = NULL;

  /* Local working variables */
  unsigned int rsp_len, operation, last_item, equip_id = 0;
  uint16 i;
  byte stream_id = 1;
  int dual_mask;
  byte *mask_rd_ptr = NULL;
  byte *mask_wr_ptr = NULL;
  boolean send_rsp = TRUE;
  log_config_status_enum_type status = LOG_CONFIG_SUCCESS_S;

  /* Sanity checks */
  if( (req_pkt_ptr == NULL) || (pkt_len == 0) )
  {
    return NULL;
  }

  dual_mask = (req_ptr->xx_header.opaque_header == DIAG_LOG_CONFIG_F)? 0:1;

  if( dual_mask )
  {
    req_dm_ptr = (log_ext_config_req_type *) req_pkt_ptr;

    if( req_dm_ptr->header.cmd_version == 2 )
    {
      return diaglog_process_ext_config_req( req_pkt_ptr, pkt_len ); //ToDo: separate function for now
    }
    else if( req_dm_ptr->header.cmd_version != 1 || req_dm_ptr->header.opcode == LOG_CONFIG_RETRIEVE_VALID_MASK_OP )
    {
      return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_dm_ptr, pkt_len);
    }
  }
  else
  {
    req_op_ptr = (log_config_op_req_type *) req_ptr->op_data;
  }

  rsp_len = FPOS(DIAG_LOG_CONFIG_F_rsp_type, op_data);
  operation = (dual_mask)? req_dm_ptr->header.opcode : req_ptr->operation;

  /* First, determine the length of the response.
   * Second, allocate the response.
   * Last, fill in the response. */

  /* This switch statement computes rsp_len for the operation */
  switch (operation)
  {
    case LOG_CONFIG_DISABLE_OP:
      rsp_len = dual_mask ? sizeof(log_ext_config_disable_op_rsp): rsp_len;
      if(dual_mask) {
        // Stream ID 0 is acceptable in this case (to disable all the streams).
         if(req_dm_ptr->disable_op.id.stream_id > DIAG_MAX_STREAM_ID) {
          status = LOG_CONFIG_INVALID_STREAM_ID;
          return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_dm_ptr, pkt_len);
          }
        }
      /* No response op data */
      break;

    case LOG_CONFIG_RETRIEVE_ID_RANGES_OP:
      rsp_len = dual_mask ? sizeof(log_ext_config_id_ranges_rsp) : (rsp_len + sizeof(log_config_ranges_rsp_type));
      break;

    case LOG_CONFIG_RETRIEVE_VALID_MASK_OP:
      equip_id = req_op_ptr->valid_mask.equip_id;
      if (equip_id > LOG_EQUIP_ID_MAX) {
        status = LOG_CONFIG_INVALID_EQUIP_ID_S;
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_ptr, pkt_len);
      }
      rsp_len += FPOS(log_config_valid_mask_rsp_type, mask) +
        (LOG_MASK_ARRAY_INDEX(log_last_item_tbl[equip_id]) + 1) * sizeof(byte);
      break;

    case LOG_CONFIG_SET_MASK_OP:
        equip_id = dual_mask? req_dm_ptr->set_mask.code_range.equip_id : req_op_ptr->set_mask.code_range.equip_id;
      if (equip_id > LOG_EQUIP_ID_MAX) {
        status = LOG_CONFIG_INVALID_EQUIP_ID_S;
          return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len);
        }

        if(dual_mask) {
          if(req_dm_ptr->set_mask.id.stream_id < 1 || req_dm_ptr->set_mask.id.stream_id > DIAG_MAX_STREAM_ID) {
          status = LOG_CONFIG_INVALID_STREAM_ID;
          return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_dm_ptr, pkt_len);
          }
      }
      rsp_len =  (dual_mask ? FPOS(log_ext_config_set_mask_rsp, mask) : (rsp_len + FPOS(log_config_valid_mask_rsp_type, mask))) +
        (LOG_MASK_ARRAY_INDEX(log_last_item_tbl[equip_id]) + 1) * sizeof(byte);

      break;

    case LOG_CONFIG_GET_LOGMASK_OP:
      equip_id = dual_mask ? req_dm_ptr->get_mask.equip_id : req_op_ptr->valid_mask.equip_id;
      if (equip_id > LOG_EQUIP_ID_MAX) {
        status = LOG_CONFIG_INVALID_EQUIP_ID_S;
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len);
      }
      if(dual_mask) {
        if(req_dm_ptr->set_mask.id.stream_id < 1 || req_dm_ptr->set_mask.id.stream_id > DIAG_MAX_STREAM_ID) {
          status = LOG_CONFIG_INVALID_STREAM_ID;
          return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_dm_ptr, pkt_len);
        }
      }
      rsp_len = (dual_mask ? FPOS(log_ext_config_get_mask_rsp, mask):(rsp_len + FPOS(log_config_valid_mask_rsp_type, mask))) +
         (LOG_MASK_ARRAY_INDEX(log_last_item_tbl[equip_id]) + 1) * sizeof(byte);

      break;

    default:
      return diagpkt_err_rsp(DIAG_BAD_PARM_F,  req_pkt_ptr, pkt_len);
  } /* end switch for rsp_len computation */


  rsp_ptr = dual_mask ? rsp_ptr : (DIAG_LOG_CONFIG_F_rsp_type *) diagpkt_alloc(DIAG_LOG_CONFIG_F, rsp_len);
  rsp_dm_ptr = dual_mask ? (log_ext_config_rsp_type *) diagpkt_subsys_alloc (DIAG_SUBSYS_DIAG_SERV, DIAG_EXT_LOG_CONFIG, rsp_len) : rsp_dm_ptr;


  /* For cmd DIAG_LOG_CONFIG_F 115 - Supports Stream 1 */
  if( !dual_mask ) {
  if(rsp_ptr == NULL)
  {
    return rsp_ptr;
  }

  rsp_ptr->pad[0] = 0;
  rsp_ptr->pad[1] = 0;
  rsp_ptr->pad[2] = 0;

  rsp_ptr->operation = operation;

  rsp_op_ptr = (log_config_op_rsp_type *) rsp_ptr->op_data;

  /* This switch statement actually performs the operation */
  switch (operation) {
    case LOG_CONFIG_DISABLE_OP:
           (void) log_config_mask(-1, FALSE, DIAG_STREAM_1, diag_cur_preset_id);
      break;

    case LOG_CONFIG_RETRIEVE_ID_RANGES_OP:
      for (i = 0; i <= LOG_EQUIP_ID_MAX; i++) {
        rsp_op_ptr->ranges.last_item[i] = (uint32) log_last_item_tbl[i];
      }
      break;

    case LOG_CONFIG_RETRIEVE_VALID_MASK_OP:
      rsp_op_ptr->valid_mask.code_range.equip_id = equip_id;
      rsp_op_ptr->valid_mask.code_range.last_item = log_last_item_tbl[equip_id];

      if (equip_id > LOG_EQUIP_ID_LAST_DEFAULT)
      {
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_ptr, pkt_len);
      }

      mask_rd_ptr = (byte *) &log_valid_mask[log_mask_offset_tbl[equip_id]];
      mask_wr_ptr = (byte *) rsp_op_ptr->valid_mask.mask;

      for (i = 0; i <= (unsigned int)
          LOG_MASK_ARRAY_INDEX(log_last_item_tbl[equip_id]); i++) {

        *mask_wr_ptr = *mask_rd_ptr;
        mask_wr_ptr++;
        mask_rd_ptr++;
      }
      break;

    case LOG_CONFIG_SET_MASK_OP:
      rsp_op_ptr->set_mask.code_range.equip_id = equip_id;
      rsp_op_ptr->set_mask.code_range.last_item = log_last_item_tbl[equip_id];

      if (equip_id > LOG_EQUIP_ID_LAST_DEFAULT)
      {
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_ptr, pkt_len);
      }
      /* Set the log mask */
      mask_rd_ptr = (byte *) req_op_ptr->set_mask.mask;
      mask_wr_ptr = &log_mask[0][log_mask_offset_tbl[equip_id]];
      last_item = MIN(log_last_item_tbl[equip_id], req_op_ptr->set_mask.code_range.last_item);

      for (i = 0; i <= LOG_MASK_ARRAY_INDEX(last_item); i++) {
        *mask_wr_ptr = *mask_rd_ptr;
         rsp_op_ptr->set_mask.mask[i] = *mask_rd_ptr;
        mask_wr_ptr++;
        mask_rd_ptr++;
      }
      /* Zero all bits that are after the last item in the set mask */
      mask_wr_ptr--;
      i--;

      /* (The next boundary of 8) - 1 - (last_item) gives the number of bits
       * at the end of the last byte that are not specified.
       * This line 0's out the bits that remain in the mask that are not
       * included in last_item. */
/*lint -save -e504 Unusual shift operation (unusually formed right argument)*/
/*lint -save -e817 */
      *mask_wr_ptr &= 0xFF >> (((last_item + 8) & ~7) - 1 - last_item);
      rsp_op_ptr->set_mask.mask[i] = *mask_wr_ptr;

      /* Zero out all items that were not specified in the request. */
      for (i = last_item + 1; i <= log_last_item_tbl[equip_id]; i++) {
            (void) log_config_mask(log_set_equip_id(i, equip_id), FALSE, DIAG_STREAM_1, diag_cur_preset_id);
      }

      for( i = LOG_MASK_ARRAY_INDEX(last_item) + 1; i <=
           LOG_MASK_ARRAY_INDEX(log_last_item_tbl[equip_id]); i++)
      {
         rsp_op_ptr->set_mask.mask[i] = 0;
      }

      /* Update composite mask */
      log_update_composite_mask(log_mask_offset_tbl[equip_id],
                                log_mask_offset_tbl[equip_id] + LOG_MASK_ARRAY_INDEX(last_item));
      break;

  case LOG_CONFIG_GET_LOGMASK_OP:
      rsp_op_ptr->get_mask.code_range.equip_id = equip_id;
      rsp_op_ptr->get_mask.code_range.last_item = log_last_item_tbl[equip_id];

      if (equip_id > LOG_EQUIP_ID_LAST_DEFAULT)
      {
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_ptr, pkt_len);
      }
      mask_rd_ptr = (byte *) &log_mask[0][log_mask_offset_tbl[equip_id]];
      mask_wr_ptr = (byte *) rsp_op_ptr->valid_mask.mask;

      for (i = 0; i <= (unsigned int)
          LOG_MASK_ARRAY_INDEX(log_last_item_tbl[equip_id]); i++) {

        *mask_wr_ptr = *mask_rd_ptr;
        mask_wr_ptr++;
        mask_rd_ptr++;

      }
      break;
  default:
      break;

  } /* end switch */


  if((LOG_CONFIG_SUCCESS_S == status) &&
    (LOG_CONFIG_SET_MASK_OP == operation))
  {
     (void)diag_client_maskchange(DIAG_LOGSVC_MASK_CHANGE);
  }

  /* Config may have changed.  Run sleep vote routine. */
  (void) log_sleep_vote ();

  rsp_ptr->status = status;

  /* if master processor on multi processor build, don't send response */
  #if !defined(DIAG_RSP_SEND)
   if (!diagcomm_apps_only_config)
    send_rsp = FALSE;
  #endif
  
/* if diag is not connected, don't send response */
  if (!diagcomm_cmd_status(DIAGCOMM_PORT_1))
  {
    send_rsp = FALSE;
  }

  /* return response */
  if (!send_rsp)
  {
    diagpkt_free(rsp_ptr);
    return NULL;
  }
  else
  {
    return ((void *) rsp_ptr);
  }
  }

  else {
    /* Dual mask */
    if(req_dm_ptr == NULL || rsp_dm_ptr == NULL) {
      return NULL;
    }
    rsp_dm_ptr->header = req_dm_ptr->header;
    switch (operation) {
        case LOG_CONFIG_DISABLE_OP:
          rsp_dm_ptr->disable_op.status = status;
          if ((rsp_dm_ptr->disable_op.id.stream_id = req_dm_ptr->disable_op.id.stream_id) == 0) {
            // Disable all streams
            (void) log_config_mask(-1, FALSE, DIAG_STREAM_ALL, diag_cur_preset_id);
          }
          else {
                stream_id = req_dm_ptr->disable_op.id.stream_id;
                // Disable a specific stream given by stream_id
                (void) log_config_mask (-1, FALSE, stream_id, diag_cur_preset_id);
          }
          break;

        case LOG_CONFIG_RETRIEVE_ID_RANGES_OP:
          rsp_dm_ptr->id_ranges.status = status;
          rsp_dm_ptr->id_ranges.rsvd = 0;
          for (i = 0; i <= LOG_EQUIP_ID_MAX; i++) {
            rsp_dm_ptr->id_ranges.last_item[i] = (uint32) log_last_item_tbl[i];
          }
          break;

        case LOG_CONFIG_SET_MASK_OP:
          rsp_dm_ptr->set_mask.id.stream_id = stream_id = req_dm_ptr->set_mask.id.stream_id;
          rsp_dm_ptr->set_mask.status = status;
          rsp_dm_ptr->set_mask.code_range.equip_id = equip_id;
          rsp_dm_ptr->set_mask.code_range.last_item = log_last_item_tbl[equip_id];

          if (equip_id > LOG_EQUIP_ID_LAST_DEFAULT)
          {
            return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_dm_ptr, pkt_len);
          }

          mask_rd_ptr = (byte *) req_dm_ptr->set_mask.mask;
          mask_wr_ptr = &log_mask[req_dm_ptr->set_mask.id.stream_id - 1][log_mask_offset_tbl[equip_id]];
          last_item = MIN(log_last_item_tbl[equip_id], req_dm_ptr->set_mask.code_range.last_item);

          for (i = 0; i <= LOG_MASK_ARRAY_INDEX(last_item); i++) {
            *mask_wr_ptr = *mask_rd_ptr;
            rsp_dm_ptr->set_mask.mask[i] = *mask_rd_ptr;

            mask_wr_ptr++;
            mask_rd_ptr++;
          }
          /* Zero all bits that are after the last item in the set mask */
          mask_wr_ptr--;
          i--;

          /* (The next boundary of 8) - 1 - (last_item) gives the number of bits
           * at the end of the last byte that are not specified.
           * This line 0's out the bits that remain in the mask that are not
           * included in last_item. */
    /*lint -save -e504 Unusual shift operation (unusually formed right argument)*/
    /*lint -save -e817 */
          *mask_wr_ptr &= 0xFF >> (((last_item + 8) & ~7) - 1 - last_item);
          rsp_dm_ptr->set_mask.mask[i] = *mask_wr_ptr;

          /* Zero out all items that were not spefcified in the request. */
          for (i = last_item + 1; i <= log_last_item_tbl[equip_id]; i++) {
            (void) log_config_mask(log_set_equip_id(i, equip_id), FALSE, stream_id, diag_cur_preset_id);
          }

          for( i = LOG_MASK_ARRAY_INDEX(last_item) + 1; i <=
               LOG_MASK_ARRAY_INDEX(log_last_item_tbl[equip_id]); i++)
          {
             rsp_dm_ptr->set_mask.mask[i] = 0;
          }

          /* Update composite mask */
          log_update_composite_mask(log_mask_offset_tbl[equip_id],
                                    log_mask_offset_tbl[equip_id] + LOG_MASK_ARRAY_INDEX(last_item));
          break;

        case LOG_CONFIG_GET_LOGMASK_OP:
          rsp_dm_ptr->get_mask.id.stream_id = req_dm_ptr->get_mask.id.stream_id;
          rsp_dm_ptr->get_mask.status = status;
          rsp_dm_ptr->get_mask.code_range.equip_id = equip_id;
          rsp_dm_ptr->get_mask.code_range.last_item = log_last_item_tbl[equip_id];

          if (equip_id > LOG_EQUIP_ID_LAST_DEFAULT)
          {
            return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_dm_ptr, pkt_len);
          }
          mask_rd_ptr = (byte *) &log_mask[req_dm_ptr->get_mask.id.stream_id - 1][log_mask_offset_tbl[equip_id]];
          mask_wr_ptr = (byte *) rsp_dm_ptr->get_mask.mask;

          for (i = 0; i <= (unsigned int)
              LOG_MASK_ARRAY_INDEX(log_last_item_tbl[equip_id]); i++) {

            *mask_wr_ptr = *mask_rd_ptr;
            mask_wr_ptr++;
            mask_rd_ptr++;

          }
          break;
      default:
          break;

      } /* end switch */

    if((LOG_CONFIG_SUCCESS_S == status) &&
      (LOG_CONFIG_SET_MASK_OP == operation))
    {
       (void)diag_client_maskchange(DIAG_LOGSVC_MASK_CHANGE);
    }

    /* Config may have changed.  Run sleep vote routine. */
    (void) log_sleep_vote ();

    /* if master processor on multi processor build, don't send response */
    #if !defined(DIAG_RSP_SEND)
     if (!diagcomm_apps_only_config)
      send_rsp = FALSE;
    #endif

  /* if diag is not connected, don't send response */
    if (!diagcomm_cmd_status(DIAGCOMM_PORT_1))
    {
      send_rsp = FALSE;
    }

  /* return response */
    if (!send_rsp)
    {
      diagpkt_free(rsp_dm_ptr);
      return NULL;
    }
    else
    {
      return ((void *) rsp_dm_ptr);
    }
  }
}   /* log_process_config_pkt */


/*===========================================================================

FUNCTION LOG PKT GET LOCAL EQUIPID RANGE

DESCRIPTION
  This function will return the number of log codes in each equipment id.
  This function queries the local Diag log mask table, as opposed to log_process_config_pkt(),
  which queries the target master log mask table

DEPENDENCIES

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/

void * log_pkt_get_local_equipid_range (
  void * req_pkt_ptr, /* pointer to request packet */
  uint16 pkt_len            /* length of request packet  */
)
{
   log_get_local_equipid_range_req_type *req_ptr = (log_get_local_equipid_range_req_type *) req_pkt_ptr;
   log_get_local_equipid_range_rsp_type *rsp = NULL;
   int i = 0;
   diagpkt_subsys_cmd_code_type cmd_code;

   if(req_ptr == NULL)
   {
     MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR,"log_pkt_get_local_equipid_range(): Request packet pointer is NULL \n",1);
     return NULL;
   }

   cmd_code = diagpkt_subsys_get_cmd_code (req_ptr);
   if ((pkt_len != sizeof(log_get_local_equipid_range_req_type)) ||
       (cmd_code != DIAGDIAG_RETRIEVE_EQUIP_ID_RANGE))
    {
     MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Value of cmd code: %d\n",cmd_code);
     return ((void *) diagpkt_err_rsp (DIAG_BAD_PARM_F, req_ptr, pkt_len));
    }

   if(diagcomm_cmd_status(DIAGCOMM_PORT_1))
   {
      rsp = (log_get_local_equipid_range_rsp_type *) diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV,
                                              cmd_code, sizeof(log_get_local_equipid_range_rsp_type));

      if (rsp)
     {
       rsp->status=SUCCESSFUL;

       for (i = 0; i <= LOG_EQUIP_ID_MAX; i++)
       {
           rsp->equipid_ranges[i] = (uint32) log_last_item_tbl[i];
       }
     }

   }

   return ((void *) rsp);

}

/*===========================================================================

FUNCTION LOG PKT GET LOCAL MASKS

DESCRIPTION
  This function will return the number of log mask values
  for a particular equipment id specified in the request packet.
  This function queries the local Diag log mask table, as opposed to log_process_config_pkt(),
  which queries the target master log mask table

DEPENDENCIES

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/

void * log_pkt_get_local_masks (
  void * req_pkt_ptr, /* pointer to request packet */
  uint16 pkt_len            /* length of request packet  */
)
{
   log_get_local_mask_req_type *req_ptr = (log_get_local_mask_req_type *) req_pkt_ptr;
   log_get_local_mask_rsp_type *rsp = NULL;
   unsigned int rsp_len = 0;
   int i = 0;
   byte *mask_rd_ptr = NULL;
   byte *mask_wr_ptr = NULL;
   diagpkt_subsys_cmd_code_type cmd_code;

   if(req_ptr == NULL)
  {
   MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "log_pkt_get_local_masks(): Request pointer is NULL \n",1);
   return NULL;
  }

   cmd_code = diagpkt_subsys_get_cmd_code (req_ptr);

   if ((pkt_len != sizeof(log_get_local_mask_req_type)) ||
       (cmd_code != DIAGDIAG_LOG_MASK_RETRIEVAL) ||
       (req_ptr->stream_id > DIAG_MAX_STREAM_ID) || (req_ptr->stream_id < DIAG_MIN_STREAM_ID) ||
       (req_ptr->equip_id > LOG_EQUIP_ID_MAX))
    {
     MSG_3(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "MSG_3 cmd_code %d stream id %d equipment id %d \n",cmd_code,req_ptr->stream_id,
           req_ptr->equip_id );
     return ((void *) diagpkt_err_rsp (DIAG_BAD_PARM_F, req_ptr, pkt_len));
    }

   if(diagcomm_cmd_status(DIAGCOMM_PORT_1))
   {
       rsp_len = FPOS (log_get_local_mask_rsp_type, log_mask) +
                                ((LOG_MASK_ARRAY_INDEX(log_last_item_tbl[req_ptr->equip_id]) + 1) * sizeof(byte));
       rsp = (log_get_local_mask_rsp_type *) diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV,
                                              cmd_code, rsp_len);

   if (rsp)
   {
      rsp->equip_id = req_ptr->equip_id;
      rsp->status = SUCCESSFUL;
      rsp->stream_id = req_ptr->stream_id;
      if(req_ptr->equip_id >LOG_EQUIP_ID_LAST && req_ptr->equip_id <=LOG_EQUIP_ID_MAX)
      {
        rsp->status =  LOG_CONFIG_NO_VALID_MASK_S;
        MSG_3(MSG_SSID_DIAG, MSG_LEGACY_LOW, "unsupported equipment ID. cmd_code %d stream id %d equipment id %d \n",cmd_code,req_ptr->stream_id,
           req_ptr->equip_id );
        rsp->log_mask[0] = 0; //Since log_last_item_tbl[euip_id] for these will be 0, so rsp->log_mask array will be of size 1.
            return ((void *) rsp);
      }

      mask_rd_ptr = (byte *) &log_mask[(req_ptr->stream_id) - 1][log_mask_offset_tbl[req_ptr->equip_id]];
      mask_wr_ptr = (byte *) rsp->log_mask;

      for (i = 0; i <= (unsigned int)
              LOG_MASK_ARRAY_INDEX(log_last_item_tbl[req_ptr->equip_id]); i++)
      {
         *mask_wr_ptr = *mask_rd_ptr;
          mask_wr_ptr++;
          mask_rd_ptr++;
      }
    }
  }
   return ((void *) rsp);
}


/*===========================================================================

FUNCTION DIAGLOG_PROCESS_EXT_CONFIG_REQ

DESCRIPTION
  This procedure processes a request to set the log mask for specific
  preset id masks for Stream 1.

PARAMETERS
  req_pkt_ptr - Pointer to request packet
  pkt_len - Length of request packet

RETURN VALUE
  Pointer to response packet.

  This command effects what type of entries are logged.
===========================================================================*/
void * diaglog_process_ext_config_req (
  void * req_pkt_ptr,
  uint16 pkt_len
)
{
  /* For Diag subsys cmd DIAG_EXT_LOG_CONFIG 75 18 98 - Supports all streams */
  log_ext_config_req_type *req_dm_ptr = NULL;
  log_ext_config_rsp_type *rsp_dm_ptr = NULL;

  /* Local working variables */
  unsigned int rsp_len, operation, last_item, equip_id = 0;
  uint16 i = 0;
  uint8 preset_id;
  byte *mask_rd_ptr = NULL;
  byte *mask_wr_ptr = NULL;
  log_config_status_enum_type status = LOG_CONFIG_SUCCESS_S;
  boolean send_rsp = TRUE;

  /* Sanity checks */
  if( (req_pkt_ptr == NULL) || (pkt_len == 0) )
  {
    return NULL;
  }

  req_dm_ptr = (log_ext_config_req_type *) req_pkt_ptr;
  operation = req_dm_ptr->header.opcode;

   /* if master processor on multi processor build, don't send response */
    #if !defined(DIAG_RSP_SEND)
     if (!diagcomm_apps_only_config)
      send_rsp = FALSE;
    #endif
  /* if diag is not connected, don't send response */
    if (!diagcomm_cmd_status(DIAGCOMM_PORT_1))
    {
      send_rsp = FALSE;
    }
  if(send_rsp == TRUE)
  {
  switch( operation )
  {
    case LOG_CONFIG_DISABLE_OP:
    {
      preset_id = req_dm_ptr->disable_op.id.preset_id;
      rsp_len = sizeof(log_ext_config_disable_op_rsp);

      // Preset ID 0 is acceptable in this case (to disable all the preset masks).
      if( preset_id > DIAG_MAX_PRESET_ID )
      {
        status = LOG_CONFIG_INVALID_PRESET_ID;
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_dm_ptr, pkt_len);
      }
      break;
    }

    case LOG_CONFIG_RETRIEVE_ID_RANGES_OP:
    {
      /* To satisfy compiler warning - Error: C3017W: preset_id may be used before being set */
      preset_id = diag_cur_preset_id;

      rsp_len = sizeof(log_ext_config_id_ranges_rsp);
      break;
    }

    case LOG_CONFIG_SET_MASK_OP:
    {
      equip_id = req_dm_ptr->set_mask.code_range.equip_id;
      preset_id = req_dm_ptr->set_mask.id.preset_id;

      if( equip_id > LOG_EQUIP_ID_MAX ) 
      {
        status = LOG_CONFIG_INVALID_EQUIP_ID_S;
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len);
      }

      if( (preset_id == 0) || (preset_id > DIAG_MAX_PRESET_ID) )
      {
        status = LOG_CONFIG_INVALID_PRESET_ID;
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_dm_ptr, pkt_len);
      }
      rsp_len = FPOS(log_ext_config_set_mask_rsp, mask) + (LOG_MASK_ARRAY_INDEX(log_last_item_tbl[equip_id]) + 1) * sizeof(byte);
      break;
    }

    case LOG_CONFIG_GET_LOGMASK_OP:
    {
      equip_id = req_dm_ptr->get_mask.equip_id;
      preset_id = req_dm_ptr->set_mask.id.preset_id;

      if( equip_id > LOG_EQUIP_ID_MAX ) 
      {
        status = LOG_CONFIG_INVALID_EQUIP_ID_S;
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len);
      }

      if( req_dm_ptr->set_mask.id.preset_id == 0 || preset_id > DIAG_MAX_PRESET_ID)
      {
        status = LOG_CONFIG_INVALID_PRESET_ID;
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_dm_ptr, pkt_len);
      }
      rsp_len = FPOS(log_ext_config_get_mask_rsp, mask) + (LOG_MASK_ARRAY_INDEX(log_last_item_tbl[equip_id]) + 1) * sizeof(byte);
      break;
    }

    default:
      return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len);

  } /* end switch for rps_len computation */

  rsp_dm_ptr = (log_ext_config_rsp_type *) diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV, DIAG_EXT_LOG_CONFIG, rsp_len);
  if( rsp_dm_ptr == NULL)
    return NULL;

  rsp_dm_ptr->header = req_dm_ptr->header;
  switch( operation )
  {
    case LOG_CONFIG_DISABLE_OP:
    {
      rsp_dm_ptr->disable_op.status = status;
      rsp_dm_ptr->disable_op.id.preset_id = preset_id;
      if( preset_id == 0)
      {
        (void) log_config_mask(-1, FALSE, DIAG_STREAM_1, DIAG_PRESET_MASK_ALL);
      }
      else
      {
        (void) log_config_mask(-1, FALSE, DIAG_STREAM_1, preset_id);
      }
      break;
    }

    case LOG_CONFIG_RETRIEVE_ID_RANGES_OP:
    {
      rsp_dm_ptr->id_ranges.status = status;
      rsp_dm_ptr->id_ranges.rsvd = 0;
      for (i = 0; i <= LOG_EQUIP_ID_MAX; i++)
      {
        rsp_dm_ptr->id_ranges.last_item[i] = (uint32)log_last_item_tbl[i];
      }
      break;
    }

    case LOG_CONFIG_SET_MASK_OP:
    {
      rsp_dm_ptr->set_mask.id.preset_id = preset_id;
      rsp_dm_ptr->set_mask.status = status;
      rsp_dm_ptr->set_mask.code_range.equip_id = equip_id;
      rsp_dm_ptr->set_mask.code_range.last_item = log_last_item_tbl[equip_id];

      if( equip_id > LOG_EQUIP_ID_LAST_DEFAULT )
      {
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_dm_ptr, pkt_len);
      }

      mask_rd_ptr = (byte *) req_dm_ptr->set_mask.mask;
      mask_wr_ptr = &diag_log_preset_mask[preset_id - 1][log_mask_offset_tbl[equip_id]];
      last_item = MIN(log_last_item_tbl[equip_id], req_dm_ptr->set_mask.code_range.last_item);

      for( i = 0; i <= LOG_MASK_ARRAY_INDEX(last_item); i++ )
      {
        *mask_wr_ptr = *mask_rd_ptr;
        rsp_dm_ptr->set_mask.mask[i] = *mask_rd_ptr;

        mask_wr_ptr++;
        mask_rd_ptr++;
      }
      /* Zero all bits that are after the last item in the set mask */
      mask_wr_ptr--;
      i--;

      /* (The next boundary of 8) - 1 - (last_item) gives the number of bits
       * at the end of the last byte that are not specified.
       * This line 0's out the bits that remain in the mask that are not
       * included in last_item. */
      /*lint -save -e504 Unusual shift operation (unusually formed right argument)*/
      /*lint -save -e817 */
      *mask_wr_ptr &= 0xFF >> (((last_item + 8) & ~7) - 1 - last_item);
      rsp_dm_ptr->set_mask.mask[i] = *mask_wr_ptr;

      /* Zero out all items that were not spefcified in the request. */
      for( i = last_item + 1; i <= log_last_item_tbl[equip_id]; i++)
      {
        (void)log_config_mask(log_set_equip_id(i, equip_id), FALSE, DIAG_STREAM_1, preset_id);
      }

      for( i = LOG_MASK_ARRAY_INDEX(last_item) + 1; i <=
               LOG_MASK_ARRAY_INDEX(log_last_item_tbl[equip_id]); i++)
      {
         rsp_dm_ptr->set_mask.mask[i] = 0;
      }
      break;
    }

    case LOG_CONFIG_GET_LOGMASK_OP:
    {
      rsp_dm_ptr->get_mask.id.preset_id = preset_id;
      rsp_dm_ptr->get_mask.status = status;
      rsp_dm_ptr->get_mask.code_range.equip_id = equip_id;
      rsp_dm_ptr->get_mask.code_range.last_item = log_last_item_tbl[equip_id];

      if( equip_id > LOG_EQUIP_ID_LAST_DEFAULT )
      {
        return diagpkt_err_rsp(DIAG_BAD_PARM_F, req_dm_ptr, pkt_len);
      }
      mask_rd_ptr = (byte *)&diag_log_preset_mask[preset_id - 1][log_mask_offset_tbl[equip_id]];
      mask_wr_ptr = (byte *)rsp_dm_ptr->get_mask.mask;

      for( i = 0; i <= (unsigned int)LOG_MASK_ARRAY_INDEX(log_last_item_tbl[equip_id]); i++ )
      {
        *mask_wr_ptr = *mask_rd_ptr;
        mask_wr_ptr++;
        mask_rd_ptr++;
      }
      break;
    }
      }     /* end switch */
  } /* end switch */

  return rsp_dm_ptr;

} /* diaglog_process_ext_config_req */


/*===========================================================================

FUNCTION LOG_PROCESS_LEGACY_LOGMASK

DESCRIPTION
  This procedure processes a request to change the logmask.
  Note: This is for the orignal 32-bit logmask that the DM used.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/
void * log_process_legacy_logmask (
  void * req_pkt_ptr,
  uint16 pkt_len
)
{
  uint32 i;                /* index to loop through logmask */
  boolean code_enabled; /* Used to determine if a code is enabled. */

  DIAG_LOGMASK_F_req_type *req_ptr = (DIAG_LOGMASK_F_req_type *) req_pkt_ptr;
  DIAG_LOGMASK_F_rsp_type *rsp_ptr = NULL;
  boolean send_rsp = TRUE;
  const unsigned int rsp_len = sizeof(DIAG_LOGMASK_F_rsp_type);

  unsigned int mask = req_ptr->mask;
  const uint32 numbits = log_last_item_tbl[1] + 1;

  (void) pkt_len; /* suppress compiler warning */
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* For each bit allowed in the logging mask, configure that log condition
   * active if the bit is 1 and configure that log condition inactive if
   * the bit is 0. */

  /* Tell lint it is OK to have a constant used as a boolean here */
  /*lint -e506 */
  for( i = 0; i < MIN(numbits, 32); i++ )
  {
  /*lint +e506 */

    code_enabled = (boolean) (mask & 0x01);

    /* Configure logmask */
    (void) log_config_mask(log_set_equip_id(i, 1), code_enabled, DIAG_STREAM_1, diag_cur_preset_id); //ToDo: save to master?

    mask >>= 1;
  }

  /* Set any codes above 32 to zero. */
  for(; i < numbits; i++)
  {
    (void) log_config_mask (log_set_equip_id(i, 1), FALSE, DIAG_STREAM_1, diag_cur_preset_id); //ToDo: save to master?
  }

  /* Vote on sleep for the logging service. */
  (void) log_sleep_vote ();

  /* if master processor on multi processor build, don't send response */
  #if !defined(DIAG_RSP_SEND)
  if (!diagcomm_apps_only_config)
    send_rsp = FALSE;
  #endif

/* if diag is not connected, don't send response */
  if (!diagcomm_cmd_status(DIAGCOMM_PORT_1))
  {
    send_rsp = FALSE;
  }
  if (send_rsp)
    rsp_ptr = (DIAG_LOGMASK_F_rsp_type *) diagpkt_alloc (DIAG_LOGMASK_F, rsp_len);

  (void)diag_client_maskchange(DIAG_LOGSVC_MASK_CHANGE);
  return (rsp_ptr);

} /* log_process_legacy_logmask */

/* -------------------------------------------------------------------------
 * Internal (aka private) function prototypes
 * ------------------------------------------------------------------------- */
/*===========================================================================

FUNCTION LOG_SEARCH_MASK

DESCRIPTION
  This function returns a boolean indicating TRUE if the given ID and 'item'
  denotes a valid and enabled log code for the given stream.

===========================================================================*/
static boolean log_search_mask (
  unsigned int id,
  unsigned int item,
  byte stream_id
)
{
  boolean return_val = FALSE;

  if(stream_id <= 0 || stream_id > DIAG_MAX_STREAM_ID)
    return return_val;

  /* if valid code val */
  if (log_mask_enabled(log_mask[stream_id - 1], id, item)) {
    return_val = TRUE;
  }
  return return_val;

} /* log_search_mask */



/*===========================================================================

FUNCTION LOG_MASK_SET

DESCRIPTION
  This function sets a log mask based on a pointer and ID/item.

  The equipment ID and item code are passed in to avoid duplicating the
  calculation within this call.  It has to be done for most routines that call
  this anyways.

===========================================================================*/
void log_mask_set_bit (
  byte *mask_ptr,
  unsigned int id,
  unsigned int item,
  boolean enabled
)
{
  unsigned int offset, index, mask;

  if (id <= LOG_EQUIP_ID_LAST && item <= log_last_item_tbl[id]) {

    offset = log_mask_offset_tbl[id];

    index = LOG_MASK_ARRAY_INDEX(item);

    mask = LOG_MASK_BIT_MASK(item);

    if (enabled) {
      mask_ptr[offset + index] |= mask;
    } else {
      mask_ptr[offset + index] &= ~mask;
    }

    log_update_composite_mask(offset+index, offset+index);
  } else {
    /* It is expected that this will be compiled out in a target build */
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_LOW,"Invalid code given 0x%x", (id << 12) | item);
  }
} /* log_mask_enabled */



/*===========================================================================

FUNCTION LOG_SET_EQUIP_ID

DESCRIPTION
  This function returns the equipment ID of the specified 'code'

===========================================================================*/
static log_code_type log_set_equip_id (
  unsigned int code,
  unsigned int equip_id
)
{
  ASSERT(equip_id <= LOG_EQUIP_ID_MAX);

  code &= 0x00000FFF;

  equip_id &= 0x0000000F;

  equip_id <<= 12;

  return (log_code_type) code | (log_code_type)equip_id;

} /* log_set_equip_id */



/*===========================================================================

FUNCTION LOG_SLEEP_VOTE

DESCRIPTION
  This function checks the configuration of the logging service to determine
  if the system is allowed to sleep.  All log items must be disabled to
  allow sleep.

===========================================================================*/
static boolean log_sleep_vote (void)
{
  unsigned int id = 0;
  uint16 item = 0;
  boolean ok2sleep = TRUE;

  /* This is not very efficient, but it works.  It only gets called
     when a config packet is sent, not from any clients.  Therefore,
     runtime efficiency is not a huge issue. */

  for (id = 0; ok2sleep && id <= LOG_EQUIP_ID_MAX; id++)
  {
    for (item = 0; ok2sleep && item <= log_last_item_tbl[id]; item++)
    {
      if (log_mask_enabled (log_mask[0], id, item))
      {
        ok2sleep = FALSE;
      }
    }
  }

  diag_sleep_vote (DIAG_SLEEP_LOG, ok2sleep);

  return ok2sleep;

}


/*===========================================================================

FUNCTION LOG_ON_DEMAND_PKT

DESCRIPTION
  This procedure processes a log on demand request. It scans in the
  log_on_demand_list to match the log_code. If it find the log_code it
  processes the function pointer associated with it.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/

void * log_on_demand_pkt (
  void * req_pkt_ptr,
  uint16 pkt_len
)
{
  DIAG_LOG_ON_DEMAND_F_req_type *req_ptr =
      (DIAG_LOG_ON_DEMAND_F_req_type *) req_pkt_ptr;
  DIAG_LOG_ON_DEMAND_F_rsp_type *rsp_ptr = NULL;
  boolean send_rsp = TRUE;
  int i;

  const unsigned int rsp_len = sizeof( DIAG_LOG_ON_DEMAND_F_rsp_type );

  (void) pkt_len; /* suppress compiler warning */

  rsp_ptr = (DIAG_LOG_ON_DEMAND_F_rsp_type *) diagpkt_alloc( DIAG_LOG_ON_DEMAND_F, rsp_len );

  if( rsp_ptr == NULL ) {
    /* If we can't allocate, diagpkt_err_rsp() can't either. */
    return NULL;
  }

  rsp_ptr->log_code = req_ptr->log_code;

  rsp_ptr->status = LOG_ON_DEMAND_NOT_SUPPORTED_S;

  for( i=0; i<LOG_ON_DEMAND_SIZE; i++ ) {
    if ( ( log_on_demand_list[i].log_on_demand_ptr != NULL )
        && (log_on_demand_list[i].log_code == req_ptr->log_code) ) {


      rsp_ptr->status=(*log_on_demand_list[i].log_on_demand_ptr)
            (log_on_demand_list[i].log_code);

    }
  }

  /* if master processor on multi processor build, don't send response */
  #if !defined(DIAG_RSP_SEND)
    send_rsp = FALSE;
  #endif

  /* if diag is not connected, don't send response */
  if (!diagcomm_cmd_status(DIAGCOMM_PORT_1))
  {
    if(!diagcomm_apps_only_config)
    send_rsp = FALSE;
  }

  /* return response */
  if (!send_rsp)
  {
    diagpkt_free(rsp_ptr);
    return NULL;
  }
  else
  {
    return ((void *) rsp_ptr);
  }

} /* log_on_demand_pkt */

/*===========================================================================

FUNCTION LOG_UPDATE_COMPOSITE_MASK

DESCRIPTION
  (re)compiles the composite log mask within the specified range
  This takes the log listener mask, the log extended listener mask, and
  all the diag stream masks and ORs together their bits.
  The end result is a mask which has its bits turned on if any of the masks
  show a request for that particular log code.

PARAMETERS
  start - first mask index to be updated
  end - last mask index to be updated (inclusive)

RETURN VALUE


===========================================================================*/
void log_update_composite_mask(uint32 start, uint32 end)
{
  int mask_index, stream_index;
  byte temp_mask=0;

  if(end >= LOG_MASK_SIZE) {
    end = LOG_MASK_SIZE - 1;
  }

  if(start > end) {
    return;
  }

  for(mask_index = start; mask_index <= end; mask_index++)
  {
    temp_mask  = log_listener_mask[mask_index];

  if(log_state == LOG_NORMAL_STATE) {
    for(stream_index = 0; stream_index < DIAG_MAX_STREAM_ID; stream_index += 1) {
        temp_mask |= log_mask[stream_index][mask_index];
    }
  }
  log_composite_mask[mask_index] =temp_mask;
 }
}

/*===========================================================================

FUNCTION LOG_STATUS_ADV

DESCRIPTION
  Checks if log code is enabled for both streams.

PARAMETERS

RETURN VALUE
  Returns appropriate stream_id.

===========================================================================*/
byte log_status_adv (log_code_type code)
{
  byte stream_id = 0;
  boolean status = FALSE;
  unsigned int id, item;

  if (log_state == LOG_NORMAL_STATE || log_state == LOG_FLUSH_STATE)
  {
    id = LOG_GET_EQUIP_ID (code);
    item = LOG_GET_ITEM_NUM (code);

    status = log_mask_enabled(log_listener_mask, id, item);

    if(status)
    {
      /* Set DIAG_STREAM_LISTENER bit if listeners active */
      stream_id |= DIAG_STREAM_LISTENER;
    }

    if(log_state == LOG_NORMAL_STATE)
    {
      if(log_status_mask (code, DIAG_STREAM_1))
        stream_id |= DIAG_STREAM_1;
      if(log_status_mask (code, DIAG_STREAM_2))
        stream_id |= DIAG_STREAM_2;
    }
  }

  return stream_id;
} /* log_status_adv */


/*===========================================================================

FUNCTION DIAG_SWITCH_LOG_PRESET_MASK

DESCRIPTION

PARAMETERS
  None

RETURN VALUE
  None

===========================================================================*/
void diag_switch_log_preset_mask()
{
  log_mask[DIAG_STREAM_1-1] = &diag_log_preset_mask[diag_cur_preset_id-1][0];

  (void)diag_client_maskchange(DIAG_LOGSVC_MASK_CHANGE);

  /* Config may have changed. Run sleep vote routine. */
  (void) log_sleep_vote ();

} /* diag_switch_log_preset_mask */

/*===========================================================================

FUNCTION DIAG_CTRL_UPDATE_LOG_MASK

DESCRIPTION
  Given a new log mask, updates the local copy of the log mask with the
  new mask.

PARAMETERS
  ctrl_msg - Ptr to CTRL msg

RETURN VALUE
  None

===========================================================================*/
/* On MP Slave only */
void diag_ctrl_update_log_mask( diag_ctrl_msg_type * ctrl_msg )
{
  uint8 * mask_ptr = NULL;
  uint32 mask_len, mask_end, len = 0;
  uint8 stream_id, status, equip_id = 0;

  if( (ctrl_msg == NULL) || (ctrl_msg->cmd_type != DIAG_CTRL_MSG_EQUIP_LOG_MASK) )
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_ctrl_update_log_mask Sanity check failed");
    return;
  }

  mask_len = ctrl_msg->data.ctrl_msg_equip_log_mask.log_mask_size;
  stream_id = ctrl_msg->data.ctrl_msg_equip_log_mask.stream_id;
  status = ctrl_msg->data.ctrl_msg_equip_log_mask.status;
  equip_id = ctrl_msg->data.ctrl_msg_equip_log_mask.equip_id;

  if( (stream_id > DIAG_MAX_STREAM_ID) || (stream_id < DIAG_MIN_STREAM_ID) )
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_ctrl_update_log_mask: Invalid stream_id %d", stream_id);
    return;
  }

  if(equip_id > LOG_EQUIP_ID_LAST)
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_ctrl_update_log_mask: Invalid equip_id %d", equip_id);
    return;
  }

  if( mask_len > (ctrl_msg->data_len - sizeof(diag_ctrl_msg_equip_log_mask_type)) )
  {
    MSG_2(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_ctrl_update_log_mask: Invalid lengths %d, %d",
          mask_len, ctrl_msg->data_len);
    return;
  }

  if( status == DIAG_CTRL_MASK_ALL_DISABLED )
  {
    (void) log_config_mask( -1, FALSE, stream_id, diag_cur_preset_id );
  }
  else if( status == DIAG_CTRL_MASK_ALL_ENABLED )
  {
    (void) log_config_mask( -1, TRUE, stream_id, diag_cur_preset_id );
  }
  else if( status == DIAG_CTRL_MASK_VALID )
  {
    if( ctrl_msg->data_len != (sizeof(diag_ctrl_msg_equip_log_mask_type) + mask_len) )
    {
      MSG_2( MSG_SSID_DIAG, MSG_LEGACY_ERROR,
             "diag_ctrl_update_log_mask: Invalid lengths %d != %d",
             ctrl_msg->data_len, sizeof(diag_ctrl_msg_equip_log_mask_type) + mask_len );
      return;
    }

    mask_ptr = ((uint8 *)ctrl_msg) + DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_equip_log_mask_type);
    len = MIN( mask_len, LOG_MASK_ARRAY_INDEX(log_last_item_tbl[equip_id])+1 );
    memscpy( (void*)&log_mask[stream_id-1][log_mask_offset_tbl[equip_id]],
             LOG_MASK_SIZE-log_mask_offset_tbl[equip_id],mask_ptr, MIN(len, LOG_MASK_SIZE) );

    /* Update composite mask */
    mask_end = log_mask_offset_tbl[equip_id] + len - 1;
    log_update_composite_mask(log_mask_offset_tbl[equip_id], mask_end);
  }

  (void)diag_client_maskchange(DIAG_LOGSVC_MASK_CHANGE);

  /* Config may have changed. Run sleep vote routine. */
  (void) log_sleep_vote ();

} /* diag_ctrl_update_log_mask */


/*===========================================================================

FUNCTION DIAG_CTRL_UPDATE_LOG_PRESET_MASK

DESCRIPTION
  Given a new log mask, updates the local copy of the log mask with the
  new mask.

PARAMETERS
  ctrl_msg - Ptr to CTRL msg

RETURN VALUE
  None

===========================================================================*/
void diag_ctrl_update_log_preset_mask( diag_ctrl_msg_type * ctrl_msg )
{
  uint8 * ctrl_mask_ptr = NULL;
  byte * log_mask_ptr = NULL;
  uint32 mask_len, len = 0;
  uint32 length = 0;
  uint8 stream_id, preset_id, status, equip_id = 0;

  if( (ctrl_msg == NULL) || (ctrl_msg->cmd_type != DIAG_CTRL_MSG_EQUIP_LOG_MASK_V3) )
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_ctrl_update_log_preset_mask Sanity check failed");
    return;
  }

  mask_len = ctrl_msg->data.ctrl_msg_equip_log_mask_v3.log_mask_size;
  stream_id = ctrl_msg->data.ctrl_msg_equip_log_mask_v3.stream_id;
  preset_id = ctrl_msg->data.ctrl_msg_equip_log_mask_v3.preset_id;
  status = ctrl_msg->data.ctrl_msg_equip_log_mask_v3.status;
  equip_id = ctrl_msg->data.ctrl_msg_equip_log_mask_v3.equip_id;

  MSG_6(MSG_SSID_DIAG, MSG_LEGACY_LOW, "update_log_preset_mask procid=%d stream=%d preset=%d status=%d equip_id=%d len=%d",
        DIAG_MY_PROC_ID, stream_id, preset_id, status, equip_id, mask_len);

  if( equip_id > LOG_EQUIP_ID_LAST )
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_ctrl_update_log_preset_mask: Invalid equip_id %d", equip_id);
    return;
  }

  if( mask_len > (ctrl_msg->data_len - sizeof(diag_ctrl_msg_equip_log_mask_v3_type)) )
  {
    MSG_2(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_ctrl_update_log_preset_mask: Invalid lengths %d, %d",
          mask_len, ctrl_msg->data_len);
    return;
  }

  if( stream_id == DIAG_STREAM_1 )
  {
    if( (preset_id >= DIAG_MIN_PRESET_ID) && (preset_id <= DIAG_MAX_PRESET_ID) )
    {
      log_mask_ptr = &diag_log_preset_mask[preset_id-1][log_mask_offset_tbl[equip_id]];
      length = LOG_MASK_SIZE - log_mask_offset_tbl[equip_id];
    }
    else
      return;
  }
  else if( stream_id == DIAG_STREAM_2 )
  {
    log_mask_ptr = &diag_log_dci_mask[log_mask_offset_tbl[equip_id]];
    length = LOG_MASK_SIZE;
    preset_id = DIAG_DCI_MASK_1;
  }
  else
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_ctrl_update_log_preset_mask: Invalid stream_id %d", stream_id);
    return;
  }

  if( status == DIAG_CTRL_MASK_ALL_DISABLED )
  {
    (void) log_config_mask( -1, FALSE, stream_id, preset_id );
  }
  else if( status == DIAG_CTRL_MASK_ALL_ENABLED )
  {
    (void) log_config_mask( -1, TRUE, stream_id, preset_id );
  }
  else if( status == DIAG_CTRL_MASK_VALID )
  {
    if( ctrl_msg->data_len != (sizeof(diag_ctrl_msg_equip_log_mask_v3_type) + mask_len) )
    {
      MSG_2( MSG_SSID_DIAG, MSG_LEGACY_ERROR,
             "diag_ctrl_update_log_preset_mask: Invalid lengths %d != %d",
             ctrl_msg->data_len, sizeof(diag_ctrl_msg_equip_log_mask_v3_type) + mask_len );
      return;
    }

    ctrl_mask_ptr = ((uint8 *)ctrl_msg) + DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_equip_log_mask_v3_type);
    len = MIN( mask_len, LOG_MASK_ARRAY_INDEX(log_last_item_tbl[equip_id])+1 );
    memscpy( (void*)log_mask_ptr, length, ctrl_mask_ptr, MIN(len, LOG_MASK_SIZE) );
  }

  (void)diag_client_maskchange(DIAG_LOGSVC_MASK_CHANGE);

  /* Config may have changed. Run sleep vote routine. */
  (void) log_sleep_vote ();

} /* diag_ctrl_update_log_preset_mask */

/*===========================================================================

FUNCTION DIAG_CTRL_SEND_LOG_RANGE_CTRL_PKT

DESCRIPTION
  Sends Log range as well as last item supported by each equipment id to
  Master Diag for build mask centralization support.
  
PARAMETERS
  None
 
RETURN VALUE
  None
  
===========================================================================*/
void diag_ctrl_send_log_range_ctrl_pkt(void)
{
  diag_ctrl_msg_type ctrl_msg;
  uint8 id = 0;
  boolean is_sent = FALSE;
  uint32 bytes_written = 0;

  if(bt_log_range_fwd_status == DIAG_CTRL_INFO_FWD_STATUS_NOT_SENT)
  {
		/* Compose DIAG_CTRL_LOG_RANGE packet */
		ctrl_msg.cmd_type = DIAG_CTRL_BT_LOG_RANGE;
		ctrl_msg.data_len = sizeof(diag_ctrl_log_range_type);
		ctrl_msg.data.ctrl_msg_bt_log_range.version = DIAG_CTRL_MSG_BT_MASK_VER;
		ctrl_msg.data.ctrl_msg_bt_log_range.log_last_equipid = LOG_EQUIP_ID_LAST;
		ctrl_msg.data.ctrl_msg_bt_log_range.num_equipid_ranges = LOG_EQUIP_ID_LAST + 1;

		for (id = 0; id <= LOG_EQUIP_ID_LAST; id++) 
		{
			 ctrl_msg.data.ctrl_msg_bt_log_range.ranges[id].equip_id = id;
			 ctrl_msg.data.ctrl_msg_bt_log_range.ranges[id].num_items = log_last_item_tbl[id];
		}

		bytes_written = diagcomm_ctrl_send_mult_pkt( (void *)&ctrl_msg, DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_log_range_type), 
														NULL, 0, DIAG_CTRL_USER_PKT_ID_0, DIAGCOMM_PORT_REMOTE_PROC, DIAGCOMM_PORT_1, TRUE, &is_sent );

		if(bytes_written == 0)
			 return;
		else if(is_sent == TRUE)
			 bt_log_range_fwd_status = DIAG_CTRL_INFO_FWD_STATUS_SENT;
		else /* bytes_written > 0 && is_sent == FALSE */
			 bt_log_range_fwd_status = DIAG_CTRL_INFO_FWD_STATUS_QUEUED;
  }
}


