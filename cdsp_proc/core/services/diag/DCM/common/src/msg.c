
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     Mobile Diagnostic Message Services

General Description
  All the declarations, definitions, and routines necessary to support
  reporting messages for errors and debugging.

Initialzation and Sequencing Requirements
  msg_init() must be called prior to msg_put().

Copyright (c) 1992-2017 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*==========================================================================

                           Edit History 

$Header: //components/rel/core.qdsp6/2.1.c4/services/diag/DCM/common/src/msg.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/21/17   ph      Tag DIAG ID header to all the diag packets sent out.
08/29/17   gn      Changes to handle remote socket close
06/16/17   sp      Resolve KW issue on ADSP, CDSP and SLPI 
06/05/17   nk      Added debug support for uimage in root PD
10/24/16   gn      Changes to support common PW for ADSP, CDSP and SLPI
05/12/16   ph      SMD references removal and code clean up.
12/17/14   ph      Removed diag_strlcpy() and replaced with strlcpy().
01/08/15   rs      Added a flag for msg_init().
04/01/14   rs      Added support for build mask centralization. 
03/05/14   xy      Fixed KW issues  
01/21/14   sr      Fix KW warnings
11/22/13   sr      Removed PACK from pointer references 
11/19/13   sr      Fix compiler warnings 
09/25/13   ph      Fixed the issue with memscpy call where dest_length is improper.
09/27/13   xy      Removed code under FEATURE_WINCE  
09/06/13   sa      Fixed issues responding to V1/V2 Diag Test Commands
04/18/13   ph      Replace memcpy() and memmove() with memscpy() and memsmove()
03/20/13   sg      Fixed Klockwork warnings
02/10/13   sg      Donot check stream 1 mask when DIAG_STREAM_2 is passed in 
                   to msg_get_ssid_masks_adv
02/11/13   sr      Mainling Diag central routing  
11/15/12   is      Support for preset masks
08/22/12   sr      Support for mask retrieval command     
08/14/12   ra      Implemented Binary Search to determine the msg_mask_tbl 
                   index for a given SSID
07/10/12   ra      F3 Mask Check Optimization    
11/17/11   is      Return set mask status based on master mask table for AP
11/10/11   is      Resolve issue with update msg mask loop 
11/07/11   is      Fixed compiler warnings
10/07/11   is      Master Diag saves and sends master msg mask to slaves
09/19/11   is      Optimized sending F3 "set mask" requests from Master Diag
09/15/11   is      Modifed "set mask" via CTRL msgs fot EA support
09/12/11   is      Master Diag forwards "set mask" requests via CTRL msgs
06/28/11   wjg     Optimized msg mask lookup in msg_get_ssid_masks_adv() 
03/18/11   hm      Diag sleep vote bug fix 
03/01/11   is      Migrate to diag_strlcpy()
12/20/10   mad     Changes for dual mask diag
12/21/10   hm      Dual mask changes  
09/17/10   is      Migrate from deprecated clk driver APIs
08/25/10   sg      Fixed compiler warnings
08/24/10   sg      Featurized registrations for msg_qsr_expand_and_send_pkt()
                   and msg_expand_and_send_pkt() on WM since we expand them in LSM layer
08/09/10   sg      Removed std_strlcpy() wrapper function calls and used std lib calls
                   since these wrappers not available on adsp
06/19/10   sg      Merging of DCM rtos and WM files
06/20/10   sg      check whether diag is initialized before allocating space from
                   diagbuf when clients call diag message apis
05/11/10   is      Removed FEATURE_ERR_EXTENDED_STORE.
04/05/10   sg      Removed inclusion of msg_qsr.h
04/26/10   vs      Update msg_store if current state machine is not correct
03/23/10   vs      Added optimizations required for ERR macro 
02/22/10   sg      Updated dspdiag changes
02/12/10   vs      Ported diag_time_get from depot
01/29/10   JV      Fixing compile error
01/27/10   ps      Decouple Error functions from diag.
12/28/09   ps      Fixed Klocwork errors & compiler warnings.
11/30/09   ps      Fixed compiler warnings & added F3trace support(SW).
11/18/09   ps      Added qsr feature support.
11/04/09   cahn    DIAG Health
09/30/09   as      Ported and mainlined msg_send_ts() api
09/29/09   mad     Included msg_arrays_i.h, removed MSG_TBL_GEN feature
09/22/09   JV      Saving to trace buffer or not, now depends on just the features
                   and not the do_sav variable.
09/22/09   vs      Use OSAL for all rex calls on qdsp6.
09/09/09   as      Removed feature FEATURE_MSG_IFACE_VIOLATION   
09/09/2009 JV      Changed signature of osal mutex calls.
07/31/09   JV      Merged Q6 diag code back to mainline
07/10/09   as      Decoupled faeture FEATURE_DIAG_NON_STREAMING
05/12/09   JV      Introduced the OS abstraction layer for rex.
05/07/09   vk      changed includes from external to internal headers
02/01/09   vg      Made trace buffer mutex consistent between event's and F3 msg
01/08/09   as      Fixed deadlock and thread protection issues
10/03/08   vg      Updated code to use PACK() vs. PACKED
12/15/06   as      Fixed critical lint errors.
08/29/06   as      Added DSP Diag support.
10/31/05   as      Fixed lint errors.
08/15/05   eav     Checked to see if trace buffer contents have been corrupted 
02/18/04   as      Fixed critical lint errors.
11/04/04   as      Added code to set ts_type filed in WCDMA Phones
09/20/04   eav     Rearranged code so that interrupts are locked for a shorter
                   amount of time when saving a trace record to buffer
07/14/04   eav     Fixed LINT errors 
06/09/04   eav     Added support for "savevars" and "savetime" of the 
                   msg_const_type.
05/14/04   eav     Renamed functions and variables to conform to naming
                   convention.
03/23/04   eav     The function msg_save_trace() will save the
                   msg_ext_store_type sent to the msg_send_3 to the EFS.
10/28/03   as      Changes to fix errors when compiling with RVCT2.0
02/03/02   as      Added msg_sprintf function to support strings
04/19/02   igt     Added new msg_send() and support functions for enhanced
                   message services
04/30/01   lad     Added msg_format_filename() under T_WINNT for use in off
                   target testing.
04/06/01   lad     Changed use of FEATURE_DIAG_MSP_BUILD to FEATURE_ZREX_TIME.
02/23/01   lad     Made use of diagpkt API and performed cleanup for
10/16/00   jal     Turned on logging for off-target debug targets
06/09/00   lad     Added flags for MSM5000_IRAM_FWD
05/15/00   lad     Changed msg_nice default to 0.
05/10/00   lad     Added load balancing support.
04/11/00   lad     Removed LOCAL from msg_dropped for externalization.
02/28/00   lad     Added support for streaming diag services.
01/26/00   lad     Removed INTLOCK from uint32 ops.  uint32 is atomic in ARM.
03/31/99   lad     Changed dropped msg handling to be correctly noted.
03/25/99    ms     Incorporated optimized MSG routines.
02/14/97   rdh     Used Pascal directive on msg_put to save ROM.
05/19/92   jah     Changed to allow dynamic message filtering
03/03/92   jah     created file, drawn from brassboard err services

===========================================================================*/

#include "customer.h"

#include "comdef.h"

#include <string.h>     /* Prototype for memcpy() */
#include <stdarg.h>     /* support for variable argument lists */
#include <stdio.h>

#include "msg.h"        /* Message typedefs and prototypes */
#include "msg_qsr.h"

#include "diagcmd.h"        /* diag command codes */
#include "diagbuf_v.h"        /* diag outpout buffer allocation manager. */
#include "diagdiag_v.h"
#include "diagi_v.h"      /* Needed for sleep vote. */
#include "diagpkt.h"
#include "err.h"
#include "msg_pkt_defs.h"
#include "qw.h"         /* definition of qword */
#include "diagcomm_v.h"
#include "diagcomm_ctrl.h" /* For diag_ctrl_msg_type */
#include "msg_arrays_i.h"  /* For msg_mask_tbl etc */
#include "msgi.h"          /* For msg_mask_read_buffer_type etc */
#include "stringl.h"
#include "diag_shared_i.h"


#if defined (FEATURE_DIAG_ADSP_CALLBACK)
#include "dspdiag_dispatch.h" /* For adsp_dspdiag125_callback */
#endif /* FEATURE_DIAG_ADSP_CALLBACK */


#include "diagtarget.h"

#if defined (DIAG_QDSP6_APPS_PROC)
  #include "diagstub.h"
#else
  #include "assert.h"
#endif

#include "diag_cfg.h"
#include <stdlib.h> 
#include "diagcomm_cmd.h"  /* For diagcomm_cmd_status */

#include "micro_msg_diag_service.h"


osal_mutex_arg_t         diag_debug_record_buf_cs;

extern diagcomm_io_conn_type diagcomm_io_ctrl_conn[NUM_PORT_TYPES][NUM_REMOTE_PORTS]; /* From diagcomm_io.c */
extern uint8 diag_cur_preset_id; /* from diag.c */


#ifdef DIAG_UIMAGE_MODE
extern micro_msg_mask_type diag_msg_composite_mask[MSG_MASK_TBL_CNT];
#endif
void diag_update_composite_msg_mask(int index, int start, int end);
/* Error (Warning) 628: no argument information provided for function 
  '__va_start() and __va_arg() */
/*lint -save -e{628} */

/* --------------------------------------------------------------------------
** Definitions and Declarations
** ----------------------------------------------------------------------- */

extern uint8 msg_enabled_mask ; /* Bit mask to check if F3 masks, listeners and Extended Listeners are set */

typedef struct
{
  msg_ssid_range_type ssid_ranges[MSG_MASK_TBL_CNT];
}
msg_large_ssid_ranges_type;

#define MSG_LARGE_SSID_RANGE(X) \
   (((msg_large_ssid_ranges_type *) X)->ssid_ranges)



/* Message operating mode.  If a legacy message request packet is used, the message service will build legacy packet formats for MSG_SSID_LEGACY and ignore all other SSIDs. */

static enum
{
  MSG_MODE_STD = 0,         /* Standard operating mode */
  MSG_MODE_TERSE,           /* Terse mode - send the store packet as is
                               (let the tool expand the pointers). */
  MSG_MODE_LEGACY           /* Legacy packet mode */
} msg_mode;

/* Statistics */
static uint32 msg_total;    /* total number of messages logged */
uint32 msg_dropped;  /* number of dropped messages */
uint32 msg_drop_delta;   /* number of dropped messages */

/* Legacy values for level are used to index this array. Legacy values were:
   #define MSG_LVL_LOW     0
   #define MSG_LVL_MED     1
   #define MSG_LVL_HIGH    2
   #define MSG_LVL_ERROR   3
   #define MSG_LVL_FATAL   4
   #define MSG_LVL_NONE    255 */

static const uint32 msg_legacy_masks[] = {
  MSG_LEGACY_LOW, MSG_LEGACY_MED, MSG_LEGACY_HIGH, MSG_LEGACY_ERROR,
  MSG_LEGACY_FATAL
};

/*lint -save -e{570} */
static const uint32 msg_legacy_rt_masks[] = {
  MSG_LVL_LOW, MSG_LVL_MED, MSG_LVL_HIGH, MSG_LVL_ERROR, MSG_LVL_FATAL
};
/*lint -restore */

static diag_ctrl_info_fwd_status bt_msg_mask_fwd_status[MSG_MASK_TBL_CNT];
static diag_ctrl_info_fwd_status bt_msg_ssid_range_fwd_status;

extern boolean msg_service_init;

unsigned int msg_ssid_count (void);
static boolean msg_set_rt_mask_adv (uint16 ssid, uint32 mask, byte stream_id, uint8 preset_id);
void msg_set_all_rt_masks_adv (uint32 mask, byte stream_id, uint8 preset_id);
static boolean msg_masks_enabled_check(byte stream_id);
static boolean msg_sleep_vote (void);

static void msg_expand_and_send_pkt (diag_send_desc_type *send, byte stream_id);

static void msg_expand_and_send_legacy_pkt (diag_send_desc_type *desc, byte stream_id);
static void msg_qsr_expand_and_send_pkt (diag_send_desc_type *desc, byte stream_id);


#if defined(T_WINNT)
static void msg_format_filename (const char *input_filename,
                 char *output_filename);
static const char * msg_format_filename2 (const char *filename);
#endif

#ifndef MSG_FMT_STR_ARG_SIZE
#define MSG_FMT_STR_ARG_SIZE 100 /* 100 is guess */
#endif

/*===========================================================================

FUNCTION MSG_INIT

DESCRIPTION
  Initialize the message reporting services.

DEPENDENCIES
  diagbuf_init() must be called prior to this call.

RETURN VALUE
  None

SIDE EFFECTS
  Sets the counts to 0, and throws away any buffered messages.

===========================================================================*/
void
msg_init (void)
{

  uint8 index = 0;
   

  (void) diagbuf_register_outbound_pkt_handler (DIAG_EXT_MSG_TERSE_F,
                     msg_expand_and_send_pkt);
  (void) diagbuf_register_outbound_pkt_handler (DIAG_QSR_EXT_MSG_TERSE_F,
                     msg_qsr_expand_and_send_pkt);


  msg_total = 0;        /* total number of errors reported */
  msg_dropped = 0;      /* total number dropped */
  msg_drop_delta = 0;       /* number dropped since last success */
  msg_mode = MSG_MODE_STD;
  
#ifdef DIAG_UIMAGE_MODE
  for(index = 0; index < MSG_MASK_TBL_CNT; index++ )
  {
    /*initilize msg composite*/
    diag_msg_composite_mask[index].ssid_first = msg_mask_tbl[index].ssid_first;
    diag_msg_composite_mask[index].ssid_last = msg_mask_tbl[index].ssid_last;
  }
#endif
  for(index = 0; index < MSG_MASK_TBL_CNT; index++ )
  {
    bt_msg_mask_fwd_status[index] = DIAG_CTRL_INFO_FWD_STATUS_NOT_SENT;
  }
  bt_msg_ssid_range_fwd_status = DIAG_CTRL_INFO_FWD_STATUS_NOT_SENT;


  msg_set_all_rt_masks_adv(0, DIAG_STREAM_ALL, DIAG_PRESET_MASK_ALL);

  msg_service_init = TRUE;

  (void) msg_sleep_vote ();
}               /* end of msg_init */

/*===========================================================================

FUNCTION MSG_EXPAND_AND_SEND_PKT

DESCRIPTION
  This routine expands the intermediate storage format into the full
  packet format, copying data from the constant block in 'stored_msg'.

DEPENDENCIES
  msg_init() must have been called previously.

RETURN VALUE

===========================================================================*/
typedef enum
{
  MSG_EXPAND_STATE_HDR = 0,
  MSG_EXPAND_STATE_DESC,
  MSG_EXPAND_STATE_ARGS,
  MSG_EXPAND_STATE_FMT,
  MSG_EXPAND_STATE_FMT_NULL,
  MSG_EXPAND_STATE_FNAME,
  MSG_EXPAND_STATE_FNAME_NULL,
  MSG_EXPAND_STATE_COMPLETE
}
msg_expand_enum_type;

static void
msg_expand_and_send_pkt (diag_send_desc_type *desc, byte stream_id)
{
  msg_ext_store_type *msg_store = NULL;
  diagbuf_status_enum_type status = DIAGBUF_READY_S;
  
  static diag_send_desc_type send = {NULL, NULL, DIAG_SEND_STATE_START, FALSE};
  static msg_expand_enum_type state_sav = MSG_EXPAND_STATE_HDR; /* Used to store state of process. */

  /* Local copies of static variables for optimized access.  
     This makes this routine persistent but not re-entrant. */
  msg_expand_enum_type state = state_sav;

  int send_pkt_len;
  
  //TODO: Move legacy mode support to diagbuf registration, not here.
  if (msg_mode == MSG_MODE_LEGACY)
  {
     msg_expand_and_send_legacy_pkt (desc,stream_id);
  }
  else if (desc)
  {
    /* 'decs' is not moved until the entire message is complete.  Therefore,
       it always points to the stored message, in terse format. */
    msg_store = (msg_ext_store_type *) desc->pkt;

    if (desc->state == DIAG_SEND_STATE_START)
    {
      /* Increment state to indicate the packet is now in progress. */
      desc->state++;

      /* Keep stats of total messages */
      msg_total++;

      if (msg_store)
      {
        /* Add dropped messages to the total. */
        msg_total += msg_store->hdr.drop_cnt;
      }
      
      if (state != MSG_EXPAND_STATE_HDR)
      {
        /* This should never happen.  However, if we start over, just kill the
           last message in progress */
        ERR ("New message while in progress", 0, 0, 0);

        desc->pkt = (void *) ((byte *) desc->last + 1);
        desc->state = DIAG_SEND_STATE_COMPLETE;
        state = MSG_EXPAND_STATE_HDR;   /* reset state */
		msg_store = (msg_ext_store_type *) desc->pkt;
      }
    }

    /* The packet is expanded in fragments.  'state' marks each stage of
       the process. */
    if (status == DIAGBUF_READY_S && state == MSG_EXPAND_STATE_HDR)
    {
      if (send.pkt == NULL && msg_store)
      {
        /* Set the command code since it was stored as MSG_EXT_TERSE_F. */
        msg_store->hdr.cmd_code = DIAG_EXT_MSG_F;

        /* Set up the send descriptor for this fragment. */
        send.pkt = (void *) &msg_store->hdr;
        send.last = (void *) ((byte *) send.pkt + sizeof (msg_store->hdr) - 1);
        send.state = DIAG_SEND_STATE_START;
        send.terminate = FALSE;
        send.diag_id = desc->diag_id;
      }

      (void) diagbuf_send_pkt (&send,stream_id);

      if (send.state == DIAG_SEND_STATE_COMPLETE)
      {
        state++;
        send.pkt = NULL;
      }
    }

    if (status == DIAGBUF_READY_S && state == MSG_EXPAND_STATE_DESC)
    {
      if (send.pkt == NULL && msg_store)
      {
        /* Set up the send descriptor for this fragment. */
        send.pkt = (void *) &msg_store->const_data_ptr->desc;
        send.last = (void *) ((byte *) send.pkt + 
          sizeof (msg_store->const_data_ptr->desc) - 1);
        
        send.state = DIAG_SEND_STATE_BUSY;
            send.diag_id = desc->diag_id;
      }

      (void) diagbuf_send_pkt (&send,stream_id);

      if (send.state == DIAG_SEND_STATE_COMPLETE)
      {
        state++;
        send.pkt = NULL;
      }
    }

    if (status == DIAGBUF_READY_S && state == MSG_EXPAND_STATE_ARGS)
    {
      if (send.pkt == NULL && msg_store)
      {
        /* Set up the send descriptor for this fragment. */
        send.pkt = (void *) msg_store->args;
        send.last = (void *) ((byte *) send.pkt + 
          sizeof (msg_store->args[0]) * msg_store->hdr.num_args - 1);
        
        send.state = DIAG_SEND_STATE_BUSY;
            send.diag_id = desc->diag_id;
      }

      (void) diagbuf_send_pkt (&send,stream_id);

      if (send.state == DIAG_SEND_STATE_COMPLETE)
      {
        state++;
        send.pkt = NULL;
      }
    }

    if (status == DIAGBUF_READY_S && state == MSG_EXPAND_STATE_FMT)
    {
      if (send.pkt == NULL && msg_store)
      {
        /* Set up the send descriptor for this fragment. */
        send.pkt = (void *) msg_store->const_data_ptr->fmt;

        send_pkt_len = (int)strlen ((char *) send.pkt);

        /* Do not sub 1 to include NULL char */
        send.last = (void *) ((byte *) send.pkt + 
          MIN (MSG_MAX_STRLEN, send_pkt_len));
        
        send.state = DIAG_SEND_STATE_BUSY;
            send.diag_id = desc->diag_id;
      }

      (void) diagbuf_send_pkt (&send,stream_id);

      if (send.state == DIAG_SEND_STATE_COMPLETE)
      {
        state++;
        send.pkt = NULL;
        
        if (*((byte *) send.last) == 0)
        {
          state++; /* skip the NULL term state */
        }
      }
    }

    if (status == DIAGBUF_READY_S && state == MSG_EXPAND_STATE_FMT_NULL)
    {
      /* There was a string length overrun in the FNAME state, so terminate
         with a NULL char.  Use the 'terminate' (which is otherwise ignored)
         field in the desc terminator as the NULL and set it to 0. */
      send.last = send.pkt = &desc->terminate;
      desc->terminate = 0;
          send.diag_id = desc->diag_id;
      
      send.state = DIAG_SEND_STATE_BUSY;
  
      (void) diagbuf_send_pkt (&send,stream_id);

      if (send.state == DIAG_SEND_STATE_COMPLETE)
      {
        state++;
        send.pkt = NULL;
      }
    }

    if (status == DIAGBUF_READY_S && state == MSG_EXPAND_STATE_FNAME)
    {
      if (send.pkt == NULL && msg_store)
      {
        /* Set up the send descriptor for this fragment. */
#if !defined (T_WINNT)
        send.pkt = (void *) msg_store->const_data_ptr->fname;
#else
        send.pkt = (void *) msg_format_filename2 (msg_store->const_data_ptr->fname);
#endif
        send_pkt_len = (int)strlen ((char *) send.pkt);
        /* Do not sub 1 to include NULL char */
        send.last = (void *) ((byte *) send.pkt + 
          MIN (MSG_MAX_STRLEN, send_pkt_len));
        
        send.state = DIAG_SEND_STATE_BUSY;
            send.diag_id = desc->diag_id;
        
        if (*((byte *) send.last) == 0)
        {
          send.terminate = TRUE;
        }
      }

      (void) diagbuf_send_pkt (&send,stream_id);

      if (send.state == DIAG_SEND_STATE_COMPLETE)
      {
        state++;
        send.pkt = NULL;
        
        if (send.terminate == TRUE)
        {
          state++; /* skip the NULL term state */
        }
      }
    }

    if (status == DIAGBUF_READY_S && state == MSG_EXPAND_STATE_FNAME_NULL)
    {
      /* There was a string length overrun in the FNAME state, so terminate
         with a NULL char.  Use the 'terminate' field in the desc tyerminator
         as the NULL and set it to 0. */
      send.last = send.pkt = &desc->terminate;
      desc->terminate = 0;
      
      send.state = DIAG_SEND_STATE_BUSY;
      send.terminate = TRUE;
          send.diag_id = desc->diag_id;

      (void) diagbuf_send_pkt (&send,stream_id);

      if (send.state == DIAG_SEND_STATE_COMPLETE)
      {
        state = MSG_EXPAND_STATE_HDR;
        desc->pkt = (void *) ((byte *) desc->last + 1);
        send.pkt = NULL;
      }
    }

    if (state == MSG_EXPAND_STATE_COMPLETE)
    {
      state = MSG_EXPAND_STATE_HDR;
      desc->pkt = (void *) ((byte *) desc->last + 1);
      desc->state = DIAG_SEND_STATE_COMPLETE;
      send.pkt = NULL;
    }
  }

  /* Commit local copies of persistent data back to static variables. */
  state = state_sav;
  
  return;
}

/*===========================================================================

FUNCTION MSG_EXPAND_AND_SEND_LEGACY_PKT

DESCRIPTION
  This routine expands the intermediate storage format into the legacy
  message format.

DEPENDENCIES
  msg_init() must have been called previously.

RETURN VALUE

===========================================================================*/
static void
msg_expand_and_send_legacy_pkt (diag_send_desc_type *desc,byte stream_id)
{
  const msg_ext_store_type *msg_store;
  const uint32 *args = NULL;
  byte num_args = 0;
  unsigned int i;
  
  static msg_legacy_rsp_type msg;   /* Intermediate storage buffer */
  static diag_send_desc_type send = 
  {
    (void *) &msg,
    (void *) ((byte *) &msg + sizeof (msg) - 1),
    DIAG_SEND_STATE_START,
    TRUE
  };

  if (desc)
  {
    msg_store = (const msg_ext_store_type *) desc->pkt;

    if (desc->state == DIAG_SEND_STATE_START && msg_store)
    {
      /* Increment state to indicate the packet is now in progress. */
      desc->state++;

      /* Keep stats of total messages */
      msg_total++;

      /* Add dropped messages to the total. */
      msg_total += msg_store->hdr.drop_cnt;

      
      /* Set up the send descriptor. */
      send.pkt = (void *) &msg;
      send.state = DIAG_SEND_STATE_START;
          send.diag_id = desc->diag_id;

      /* Fill in the legacy descriptor. */
      msg.cmd_code = DIAG_MSG_F;
      msg.qty = 1;

      msg.drop_cnt = msg_dropped;
      msg.total_msgs = msg_total;

      /* init to low message */
      msg.level = 0;

      for (i = 0; i < sizeof (msg_legacy_masks) / sizeof (msg_legacy_masks[0]);
           i++)
      {
        if (msg_legacy_masks[i] & msg_store->const_data_ptr->desc.ss_mask)
        {
          msg.level = (byte)i;
          break;
        }
      }

        /* Copy file name and null terminate it. */

          strlcpy ((char *) msg.file, msg_store->const_data_ptr->fname,
                      MSG_FILE_LEN);

      /* Copy source file line number */
      msg.line = msg_store->const_data_ptr->desc.line;

      /* Copy format string and null terminate it. */

          strlcpy ((char *) msg.fmt, msg_store->const_data_ptr->fmt,
                      MSG_FMT_LEN);

      /* Copy parameters for format string */
      args = MSG_LARGE_ARGS(msg_store->args);

      num_args = msg_store->hdr.num_args;

      /* Copy up to 3 args, depending on what is available */
      msg.code1 = (num_args >= 1) ? args[0] : 0;
      msg.code2 = (num_args >= 2) ? args[1] : 0;
      msg.code3 = (num_args >= 3) ? args[2] : 0;

      /* Get timestamp for this message ASAP */
      qw_equ (msg.time, (unsigned long *) msg_store->hdr.ts);
    }

    (void) diagbuf_send_pkt (&send,stream_id);

    if (send.state == DIAG_SEND_STATE_COMPLETE)
    {
      send.pkt = NULL;
      desc->pkt = (void *) ((byte *) desc->last + 1);
      desc->state = DIAG_SEND_STATE_COMPLETE;
    }
  }

  return;
}

/*===========================================================================

FUNCTION MSG_MASK_BINARY_SEARCH

DESCRIPTION
  This function will return the msg_mask_tbl index using a binary search technique
  for a given SSID. Every index returned corresponds to the ssid_first in each structure.

DEPENDENCIES 
  Depends on the msg_mask_tbl defined in msg_arrays_i.c

RETURN VALUE
  If the entry was found, the table index between 0 and MSG_MASK_TBL_CNT-1
  into msg_mask_tbl will be returned. Else, MSG_MASK_TBL_CNT will be returned.

SIDE EFFECTS
  None.
===========================================================================*/

uint8 msg_mask_binary_search (uint16 curr_ssid)
{
  uint8 mid_index;
  uint8 start_index = 1;  // starting at 1, since 0 is for legacy
  uint8 end_index = MSG_MASK_TBL_CNT - 1;
  uint8 table_index = MSG_MASK_TBL_CNT;

  // Skips the search for the legacy messages.
  if(curr_ssid <= msg_mask_tbl[0].ssid_last) 
  {
    return 0;
  }

  /* the binary search modified to work with structures holding the start and end ssid values */
  while( start_index <= end_index )
  {
    mid_index = ( start_index + end_index ) >> 1;

    if( msg_mask_tbl[mid_index].ssid_first > curr_ssid )
    {
      end_index = mid_index - 1;
    }
    else if(curr_ssid > msg_mask_tbl[mid_index].ssid_last)
    {
      start_index = mid_index + 1;
    }
    else
    {
      table_index = mid_index;
      break;
    }
    /* Adjusting for the index returned by the binary search to make it correspond to the ssid_first in that structure */
    if( (start_index - end_index) == 1 )
    {
      if (curr_ssid < msg_mask_tbl[start_index].ssid_first) 
      {
        table_index = start_index - 1;
        break;
      } 
      else if (curr_ssid < msg_mask_tbl[end_index].ssid_first) 
      {
        table_index = start_index;
        break;
      } 
      else 
      { 
        table_index = end_index;
        break;
      }
    }
  }
  // the ssid is outside the range of SSIDs defined in the msg_mask_tbl.
  if (curr_ssid > msg_mask_tbl[table_index].ssid_last)
  {
    return MSG_MASK_TBL_CNT;
  }
  return table_index; 
} /* msg_mask_binary_search () */

/*===========================================================================

FUNCTION MSG_GET_SSID_MASKS_ADV

DESCRIPTION
  This function will get an SSID as input and return its corresponding
  build time mask or real time mask by accessing the appropriate data
  structure. It returns True/False based on whether the SSID is valid or not.

DEPENDENCIES

RETURN VALUE
  True/False

SIDE EFFECTS
  None.
===========================================================================*/
boolean
msg_get_ssid_masks_adv (msg_mask_type mask_type, uint16 ssid_start,
            uint16 ssid_end, uint32 * mask, byte stream_id, uint8 preset_id)
{
  unsigned int ssid_index;
  boolean success = FALSE;
  uint8 table_index = 0;

  const msg_mask_tbl_type *tbl = msg_mask_tbl;
  const uint32 *mask_array = NULL;  /* First pass forces a search */

  #ifndef DIAG_STREAM_2_MASK_CHECK
  if(stream_id != DIAG_STREAM_1)
     return FALSE;
  #endif
  
/*-------------------------------------------------------------------------*/

  if (mask && ssid_start <= ssid_end)
  {
    success = TRUE;
    for (ssid_index = ssid_start; ssid_index <= ssid_end; ssid_index++)
    {
      /* If not currently matched to a correct table, reset table search. */
      if ( ssid_index < tbl->ssid_first || ssid_index > tbl->ssid_last)
      {
        tbl = msg_mask_tbl;
        mask_array = NULL;
      }

      /* Table search: if mask_array is not set */
      if ( !mask_array )
      {
        table_index = msg_mask_binary_search(ssid_index);
        if( table_index == MSG_MASK_TBL_CNT )
        {
          mask_array = NULL;
        }
        else 
        {
          tbl = &msg_mask_tbl[table_index];
          if (mask_type == MSG_MASK_BT)
          {
             mask_array = tbl->bt_mask_array;
          }
          else
          {
            mask_array = (const uint32 *)&tbl->rt_base_ptr[stream_id - 1][(preset_id-1)*(tbl->ssid_last - tbl->ssid_first + 1)];
          }
        }
      }

      if (mask_array)
      {
        /* Valid SSID found.  Write mask to caller's mask. */
        mask[ssid_index - ssid_start] = mask_array[ssid_index - tbl->ssid_first];
      }
      else
      {
        /* Invalid SSID.  Indicate failure and continue. */
        mask[ssid_index - ssid_start] = 0;
        success = FALSE;    /* At least 1 entry is invalid */
      }
    }
  }

  return (success);
}               /* msg_get_ssid_masks_adv() */

/*===========================================================================

FUNCTION MSG_SET_RT_MASK_ADV

DESCRIPTION
  This function will get an SSID and a real time mask as input and set the
  real time mask that corresponds to that ssid to the requested value.
  It returns True/False based on whether the SSID is valid or not.

PARAMETERS 
  ssid - SSID to set
  mask - Mask to set for ssid specified
  stream_id - SSID for this stream ID will be set. Valid stream IDs are 
              DIAG_MIN_STREAM_ID to DIAG_MAX_STREAM_ID.
  preset_id - SSID for this preset ID will be set. Valid preset IDs are 
              DIAG_MIN_PRESET_ID to DIAG_MAX_PRESET_ID.
  
DEPENDENCIES

RETURN VALUE
  True/False

SIDE EFFECTS
  None.
===========================================================================*/
static boolean
msg_set_rt_mask_adv (uint16 ssid, uint32 mask, byte stream_id, uint8 preset_id)
{
  boolean found = FALSE;
  uint32 i;
  uint16 count = 0;

  /* Sanity checks */
  if( (stream_id < DIAG_MIN_STREAM_ID) || (stream_id > DIAG_MAX_STREAM_ID) || 
      (preset_id == 0) || (preset_id > DIAG_MAX_PRESET_ID) )
		return FALSE;

  found = FALSE;
  for (i = 0; !found && i < MSG_MASK_TBL_CNT; i++)
  {
    if (ssid >= msg_mask_tbl[i].ssid_first && ssid <= msg_mask_tbl[i].ssid_last)
    {
      found = TRUE;
      count = msg_mask_tbl[i].ssid_last - msg_mask_tbl[i].ssid_first + 1;
      msg_mask_tbl[i].rt_base_ptr[stream_id-1][(preset_id-1)*count + (ssid - msg_mask_tbl[i].ssid_first)] = mask;
      /*update msg composite mask*/
      diag_update_composite_msg_mask(i, 0, count-1);
    }
  }
  return (found);

}               /* msg_set_rt_mask_adv() */

/*===========================================================================

FUNCTION MSG_SET_ALL_RT_MASKS_ADV

DESCRIPTION
  This function will get a real time mask as input and set ALL
  real time masks to the requested value.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void
msg_set_all_rt_masks_adv (uint32 mask, byte stream_id, uint8 preset_id)
{
  uint32 tbl_index, j, count;
  uint8 preset_index = 0;

  if( (stream_id == DIAG_STREAM_1) || (stream_id == DIAG_STREAM_ALL) )
  {
    if( preset_id == DIAG_PRESET_MASK_ALL )
    {
      for( preset_index=0; preset_index < DIAG_MAX_PRESET_ID; preset_index++ )
      {
        for( tbl_index = 0; tbl_index < MSG_MASK_TBL_CNT; tbl_index++ )
        {
          count = (uint32)(msg_mask_tbl[tbl_index].ssid_last - msg_mask_tbl[tbl_index].ssid_first + 1);
          for (j = 0; j < count; j++)
          {
            msg_mask_tbl[tbl_index].rt_base_ptr[DIAG_STREAM_1-1][(preset_index)*count + j] = mask;
          }
          /*update msg composite mask*/
          diag_update_composite_msg_mask(tbl_index, 0, count-1);
        }
      }
    }
    else
    {
      for( tbl_index = 0; tbl_index < MSG_MASK_TBL_CNT; tbl_index++ )
      {
        count = (uint32)(msg_mask_tbl[tbl_index].ssid_last - msg_mask_tbl[tbl_index].ssid_first + 1);
        for (j = 0; j < count; j++)
        {
          msg_mask_tbl[tbl_index].rt_base_ptr[DIAG_STREAM_1-1][(preset_id-1)*count + j] = mask;
        }
      }
    }
  } //end set stream 1 all
    
#if defined(DIAG_STREAM_2_MASK_CHECK)
  if( (stream_id == DIAG_STREAM_2) || (stream_id == DIAG_STREAM_ALL) )
  {
    for( tbl_index = 0; tbl_index < MSG_MASK_TBL_CNT; tbl_index++ )
    {
      count = (uint32)(msg_mask_tbl[tbl_index].ssid_last - msg_mask_tbl[tbl_index].ssid_first + 1);
      for (j = 0; j < count; j++)
      {
        msg_mask_tbl[tbl_index].rt_base_ptr[DIAG_STREAM_1-1][(DIAG_DCI_MASK_1-1)*count + j] = mask;
      }
      /*update msg composite mask*/
      diag_update_composite_msg_mask(tbl_index, 0, count-1);
    }
  } //end set stream 2 all
#endif /* DIAG_STREAM_2_MASK_CHECK */

  /* Set the masks of all clients registered with DCM.*/
  (void)diag_client_maskchange(DIAG_MSGSVC_MASK_CHANGE);
   
  
  /* Vote for sleep */
  if (mask == 0 ) // Vote for sleep only when stream 1 is off or all the streams are off.
  {
    switch(stream_id)
    {
    case 0: 
      diag_sleep_vote (DIAG_SLEEP_MSG, TRUE);
      DIAG_CLR_MASK(msg_enabled_mask, DIAG_MSG_STREAM_1);
      DIAG_CLR_MASK(msg_enabled_mask, DIAG_MSG_STREAM_2);
      break;
    case DIAG_STREAM_1:
      diag_sleep_vote (DIAG_SLEEP_MSG, TRUE);
      DIAG_CLR_MASK(msg_enabled_mask, DIAG_MSG_STREAM_1);
      break;
    case DIAG_STREAM_2:
      DIAG_CLR_MASK(msg_enabled_mask, DIAG_MSG_STREAM_2);
      break;
    }
  }
  else
  {
    diag_sleep_vote (DIAG_SLEEP_MSG, FALSE);
    switch(stream_id)
    {
    case 0: 
      DIAG_SET_MASK(msg_enabled_mask, DIAG_MSG_STREAM_1);
      DIAG_SET_MASK(msg_enabled_mask, DIAG_MSG_STREAM_2);
      break;
    case DIAG_STREAM_1:
      DIAG_SET_MASK(msg_enabled_mask, DIAG_MSG_STREAM_1);
      break;
    case DIAG_STREAM_2:
      DIAG_SET_MASK(msg_enabled_mask, DIAG_MSG_STREAM_2);
      break;
    }
  }

}               /* msg_set_all_rt_mask_adv() */


/*===========================================================================

FUNCTION MSG_SSID_COUNT

DESCRIPTION
  This function will calculate the number of valid SubSystem IDs for this
  target and return it.

DEPENDENCIES

RETURN VALUE
  The number of valid SSIDs.

SIDE EFFECTS
  None.
===========================================================================*/
unsigned int
msg_ssid_count (void)
{
  unsigned int i, ssid_num = 0;

/*-------------------------------------------------------------------------*/

  for (i = 0; i < MSG_MASK_TBL_CNT; i++)
  {
    ssid_num += msg_mask_tbl[i].ssid_last - msg_mask_tbl[i].ssid_last + 1;
  }

  return (ssid_num);

}               /* msg_ssid_count() */

/*===========================================================================

             Debug Message Service Packet Handler Routines

===========================================================================*/

/* Extended Message Config Request */

/*===========================================================================

FUNCTION MSG_PKT_GET_SSID_RANGES

DESCRIPTION
  This function will return the valid SSID range.

DEPENDENCIES

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
static void *
msg_pkt_get_ssid_ranges (void *req_pkt, word req_len)
{
  msg_get_ssid_ranges_req_type *req =
    (msg_get_ssid_ranges_req_type *) req_pkt;
  msg_get_ssid_ranges_rsp_type *rsp = NULL;
  unsigned int i = 0, rsp_len = 0;

/*-------------------------------------------------------------------------*/

  if (req && req->sub_cmd == (uint8) MSG_EXT_SUBCMD_GET_RANGES)
  {
    /*-------------------------------------------------------------------------
      calculate number of valid ssids and the length of the responce packet.
    -------------------------------------------------------------------------*/
    rsp_len = FPOS (msg_get_ssid_ranges_rsp_type, ssid_ranges) +
      MSG_MASK_TBL_CNT * FSIZ (msg_get_ssid_ranges_rsp_type, ssid_ranges);

    /*-------------------------------------------------------------------------
      allocate space for the responce packet
    -------------------------------------------------------------------------*/
    rsp = (msg_get_ssid_ranges_rsp_type *)
      diagpkt_alloc (DIAG_EXT_MSG_CONFIG_F, rsp_len);

    if (rsp)
    {
      rsp->sub_cmd = req->sub_cmd;
      rsp->status = TRUE;
      rsp->range_cnt = MSG_MASK_TBL_CNT;
      rsp->rsvd = 0;

      /*-------------------------------------------------------------------------
        make aux_access_ptr point to the end of the msg_get_ssid_ranges_rsp_type
        which is where the SSIDd should start.
      -------------------------------------------------------------------------*/

      for (i = 0; i < MSG_MASK_TBL_CNT; i++)
      {
        rsp->ssid_ranges[i].ssid_first = msg_mask_tbl[i].ssid_first;
        rsp->ssid_ranges[i].ssid_last = msg_mask_tbl[i].ssid_last;
      }
    }
  }                

  else
  {
    rsp = (void *) diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt, req_len);
  }

  return (rsp);
}               /* msg_pkt_get_ssid_ranges () */

/*===========================================================================

FUNCTION MSG PKT GET LOCAL SSID RANGE

DESCRIPTION
  This function will return the valid SSID range.
  This function queries the local Diag message mask table, as opposed to msg_pkt_get_ssid_ranges(),
  which queries the target master message mask table

DEPENDENCIES

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/

void * msg_pkt_get_local_ssid_range(void * req_pkt, uint16 pkt_len)
{
  msg_get_local_ssid_range_req_type *req = (msg_get_local_ssid_range_req_type *) req_pkt;
  msg_get_local_ssid_range_rsp_type *rsp = NULL;
  unsigned int rsp_len = 0;
  int i = 0;
  diagpkt_subsys_cmd_code_type cmd_code;
  
   
  if(req == NULL)
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "msg_pkt_get_local_ssid_range(): Request pointer is NULL \n",1);
   return NULL;
  }

   cmd_code = diagpkt_subsys_get_cmd_code (req);

   if ((pkt_len != sizeof(msg_get_local_ssid_range_req_type)) || 
       (cmd_code != DIAGDIAG_RETRIEVE_SSID_RANGE))
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "MSG_1 cmd_code %d \n",cmd_code);
     return ((void *) diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt, pkt_len));
  }

   if(diagcomm_cmd_status(DIAGCOMM_PORT_1))
   {
   rsp_len = FPOS (msg_get_local_ssid_range_rsp_type, ssid_ranges) +
      MSG_MASK_TBL_CNT * FSIZ (msg_get_local_ssid_range_rsp_type, ssid_ranges);

    rsp = (msg_get_local_ssid_range_rsp_type *) diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV, 
                                              cmd_code, rsp_len);

    if (rsp)
    {
      rsp->status = SUCCESSFUL;     
      rsp->range_cnt = MSG_MASK_TBL_CNT;
      
      for (i = 0; i < MSG_MASK_TBL_CNT; i++)
      {
        rsp->ssid_ranges[i].ssid_first = msg_mask_tbl[i].ssid_first;
        rsp->ssid_ranges[i].ssid_last = msg_mask_tbl[i].ssid_last;
      }
    }
   }
   return ((void *) rsp);
}


static void *
msg_pkt_get_ssid_ranges_adv (void *req_pkt, word req_len)
{
  msg_get_ssid_ranges_adv_req_type *req =
    (msg_get_ssid_ranges_adv_req_type *) req_pkt;
  msg_get_ssid_ranges_adv_rsp_type *rsp = NULL;
  unsigned int i = 0, rsp_len = 0;

/*-------------------------------------------------------------------------*/
  
  if (req && (req_len == sizeof(msg_get_ssid_ranges_adv_req_type)) && (req->opcode == (uint8)MSG_EXT_SUBCMD_GET_RANGES) )
  {
    /*-------------------------------------------------------------------------
      calculate number of valid ssids and the length of the responce packet.
    -------------------------------------------------------------------------*/
    rsp_len = FPOS (msg_get_ssid_ranges_adv_rsp_type, ssid_ranges) +
      MSG_MASK_TBL_CNT * FSIZ (msg_get_ssid_ranges_adv_rsp_type, ssid_ranges);

    /*-------------------------------------------------------------------------
      allocate space for the responce packet
    -------------------------------------------------------------------------*/
    rsp = (msg_get_ssid_ranges_adv_rsp_type *)
      diagpkt_subsys_alloc (DIAG_SUBSYS_DIAG_SERV, DIAG_EXT_MSG_CONFIG_ADV, rsp_len);

    if (rsp)
    {
      rsp->header.cmd_code = req->cmd_code;
      rsp->header.cmd_version = req->cmd_version;
      rsp->header.opcode = req->opcode;
      rsp->status = TRUE;
      rsp->range_cnt = MSG_MASK_TBL_CNT;
      rsp->rsvd = 0;

      /*-------------------------------------------------------------------------
        make aux_access_ptr point to the end of the msg_get_ssid_ranges_rsp_type
        which is where the SSIDd should start.
      -------------------------------------------------------------------------*/

      for (i = 0; i < MSG_MASK_TBL_CNT; i++)
      {
        rsp->ssid_ranges[i].ssid_first = msg_mask_tbl[i].ssid_first;
        rsp->ssid_ranges[i].ssid_last = msg_mask_tbl[i].ssid_last;
      }
    }
  }                
  else
  {
    rsp = (void *) diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt, req_len);
  }

  return (rsp);
}               /* msg_pkt_get_ssid_ranges_adv () */



/*===========================================================================

FUNCTION MSG_PKT_GET_MASKS

DESCRIPTION
  This function will retrieve an array of masks of specified type for the
  range of SSIDs provided.

DEPENDENCIES
  req is not NULL and points to a valid  request packet

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
static void *
msg_pkt_get_masks (void *req_pkt, word req_len, msg_mask_type mask_type)
{
  msg_get_mask_req_type *req = (msg_get_mask_req_type *) req_pkt;
  msg_get_mask_rsp_type *rsp = NULL;
  unsigned int rsp_len = 0;

/*-------------------------------------------------------------------------*/

  if (req && req->ssid_start <= req->ssid_end &&
      (req->sub_cmd == (uint8) MSG_EXT_SUBCMD_GET_BUILD_MASK ||
       req->sub_cmd == (uint8) MSG_EXT_SUBCMD_GET_RT_MASK))
  {
    rsp_len = FPOS (msg_get_mask_rsp_type, bld_mask) +
      ((req->ssid_end - req->ssid_start + 1) *
       FSIZ (msg_get_mask_rsp_type, bld_mask));

    rsp = (msg_get_mask_rsp_type *)
      diagpkt_alloc (DIAG_EXT_MSG_CONFIG_F, rsp_len);

    if (rsp)
    {
      rsp->sub_cmd = req->sub_cmd;
      rsp->ssid_start = req->ssid_start;
      rsp->ssid_end = req->ssid_end;
      rsp->pad = 0;

      rsp->status = msg_get_ssid_masks_adv (mask_type, req->ssid_start,
                    req->ssid_end, rsp->bld_mask, DIAG_STREAM_1, diag_cur_preset_id);
    }
  }
  else
  {
    rsp = (msg_get_mask_rsp_type *)
      diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt, req_len);
  }

  return (rsp);

}               /* msg_pkt_get_masks() */

static void *
msg_pkt_get_masks_adv (void *req_pkt, word req_len, msg_mask_type mask_type)
{
  msg_get_mask_adv_req_type *req = (msg_get_mask_adv_req_type *) req_pkt;
  msg_get_mask_adv_rsp_type *rsp = NULL;
  msg_get_rt_mask_adv_req_type *req_rt = (msg_get_rt_mask_adv_req_type *) req_pkt;
  msg_get_rt_mask_adv_rsp_type *rsp_rt = NULL;
  
  unsigned int rsp_len = 0;

/*-------------------------------------------------------------------------*/

  if (req && 
      (req->header.opcode == (uint8) MSG_EXT_SUBCMD_GET_BUILD_MASK ||
       req->header.opcode == (uint8) MSG_EXT_SUBCMD_GET_RT_MASK))
  {

    if(req->header.opcode == (uint8) MSG_EXT_SUBCMD_GET_BUILD_MASK)
    {
      if(req->ssid_first > req->ssid_last)
      {
        return ((void *) diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt, req_len));
      }
         rsp_len = FPOS (msg_get_mask_adv_rsp_type, bld_mask) +
        ((req->ssid_last - req->ssid_first + 1) *
         FSIZ (msg_get_mask_adv_rsp_type, bld_mask));
  
      rsp = (msg_get_mask_adv_rsp_type *)
            diagpkt_subsys_alloc (DIAG_SUBSYS_DIAG_SERV, DIAG_EXT_MSG_CONFIG_ADV, rsp_len);
  
      if (rsp)
      {
        rsp->header = req->header;
        rsp->ssid_first = req->ssid_first;
        rsp->ssid_last = req->ssid_last;
        rsp->rsvd = 0;
  
        rsp->status = msg_get_ssid_masks_adv (mask_type, req->ssid_first,
                      req->ssid_last, (uint32 *)(rsp->bld_mask), 
                      DIAG_STREAM_1, diag_cur_preset_id);  // stream_id and preset_id doesn't matter here.
      }

      return (void *)rsp;
    }
    else 
    {
      if( (req_rt->header.cmd_version == 1) && (req_rt->id.stream_id >=DIAG_MIN_STREAM_ID) && 
          (req_rt->id.stream_id <= DIAG_MAX_STREAM_ID) && (req_rt->ssid_last >= req_rt->ssid_first) )
      {
        rsp_len = FPOS (msg_get_rt_mask_adv_rsp_type, rt_mask) +
        ((req_rt->ssid_last - req_rt->ssid_first + 1) *
         FSIZ (msg_get_rt_mask_adv_rsp_type, rt_mask));
  
        rsp_rt = (msg_get_rt_mask_adv_rsp_type *)
                  diagpkt_subsys_alloc (DIAG_SUBSYS_DIAG_SERV, DIAG_EXT_MSG_CONFIG_ADV, rsp_len);
                  
        if( rsp_rt )
        {
          rsp_rt->header = req_rt->header;
          rsp_rt->id.stream_id = req_rt->id.stream_id;
          rsp_rt->ssid_first = req_rt->ssid_first;
          rsp_rt->ssid_last = req_rt->ssid_last;

          rsp_rt->status = msg_get_ssid_masks_adv( mask_type, req_rt->ssid_first,
                                                   req_rt->ssid_last, (uint32 *)(rsp_rt->rt_mask), 
                                                   req_rt->id.stream_id, diag_cur_preset_id );
        }
        return (void *)rsp_rt;
      }
      else if( (req_rt->header.cmd_version == 2) && (req_rt->id.preset_id >=DIAG_MIN_PRESET_ID) && 
               (req_rt->id.preset_id <= DIAG_MAX_PRESET_ID) && (req_rt->ssid_last >= req_rt->ssid_last) )
      {
        rsp_len = FPOS (msg_get_rt_mask_adv_rsp_type, rt_mask) +
        ((req_rt->ssid_last - req_rt->ssid_first + 1) *
         FSIZ (msg_get_rt_mask_adv_rsp_type, rt_mask));
  
        rsp_rt = (msg_get_rt_mask_adv_rsp_type *)
                  diagpkt_subsys_alloc (DIAG_SUBSYS_DIAG_SERV, DIAG_EXT_MSG_CONFIG_ADV, rsp_len);
                  
        if( rsp_rt )
        {
          rsp_rt->header = req_rt->header;
          rsp_rt->id.preset_id = req_rt->id.preset_id;
          rsp_rt->ssid_first = req_rt->ssid_first;
          rsp_rt->ssid_last = req_rt->ssid_last;

          rsp_rt->status = msg_get_ssid_masks_adv( mask_type, req_rt->ssid_first,
                                                   req_rt->ssid_last, (uint32 *)(rsp_rt->rt_mask), 
                                                   DIAG_STREAM_1, req_rt->id.preset_id );
        }
        return (void *)rsp_rt;
      }
      else
      {
        return ( (void *)diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt, req_len) );
      }
    }
  }
  else
  {    
     return ((void *) diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt, req_len));
  }
}               /* msg_pkt_get_masks_adv() */

/*===========================================================================
FUNCTION MSG PKT GET LOCAL MASKS

DESCRIPTION
  This function will return the
  message mask values for a valid ssid range specified in the request packet.
  This function queries the local Diag message mask table, as opposed to msg_pkt_get_masks(),
  which queries the target master message mask table

DEPENDENCIES

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/ 
void *
msg_pkt_get_local_masks(void * req_pkt, uint16 pkt_len)
{ 
  msg_get_local_mask_req_type *req = (msg_get_local_mask_req_type *) req_pkt;
  msg_get_local_mask_rsp_type *rsp = NULL;
  unsigned int rsp_len = 0;
  diagpkt_subsys_cmd_code_type cmd_code = 0;
  uint8 preset_id;
  
  if(req == NULL) 
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "msg_pkt_get_local_masks(): Request pointer is NULL \n",1);
   return NULL; 
  }

   cmd_code = diagpkt_subsys_get_cmd_code (req_pkt);

  if ((pkt_len != sizeof(msg_get_local_mask_req_type)) || 
       (cmd_code != DIAGDIAG_MSG_MASK_RETRIEVAL)|| 
       (req->stream_id > DIAG_MAX_STREAM_ID) || (req->stream_id < DIAG_MIN_STREAM_ID))
  {
     MSG_2(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "MSG_2 cmd_code %d stream id %d \n",cmd_code,req->stream_id);
     return ((void *) diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt, pkt_len));
  }

  if( req->stream_id == DIAG_STREAM_1 )
  {
    preset_id = diag_cur_preset_id;
  }
  else if( req->stream_id == DIAG_STREAM_2 )
  {
    preset_id = DIAG_DCI_MASK_1;
  }
  else
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "Invalid stream_id = %d ", req->stream_id);
    return ((void *) diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt, pkt_len));
  }
  
/*-------------------------------------------------------------------------*/

  if (req && req->ssid_start <= req->ssid_end && diagcomm_cmd_status(DIAGCOMM_PORT_1))
  {
    rsp_len = FPOS (msg_get_local_mask_rsp_type, msg_mask) +
        ((req->ssid_end - req->ssid_start + 1) *
         FSIZ (msg_get_local_mask_rsp_type, msg_mask));

     rsp = (msg_get_local_mask_rsp_type *) diagpkt_subsys_alloc(DIAG_SUBSYS_DIAG_SERV, 
                                              cmd_code, rsp_len);

    if (rsp)
    {
      rsp->ssid_start = req->ssid_start;
      rsp->ssid_end = req->ssid_end;
      rsp->stream_id = req->stream_id;
      rsp->status = msg_get_ssid_masks_adv (MSG_MASK_RT, req->ssid_start,
                    req->ssid_end, rsp->msg_mask, req->stream_id, preset_id);
      rsp->padding = 0;
    }
  }

  return ((void *) rsp);

}               /* msg_pkt_get_local_masks */

/*===========================================================================

FUNCTION  MSG_PKT_SET_RT_MASKS

DESCRIPTION
  This function will SET the  value of the "tool" mask for the
  specific SubSystem ID and prepare a responce packet. If the
  SSID is not valid it will return Status = false.

DEPENDENCIES
  req is not NULL and points to a valid  request packet

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
static void *
msg_pkt_set_rt_masks (void *req_pkt, word req_len)
{
  msg_set_rt_mask_req_type *req = (msg_set_rt_mask_req_type *) req_pkt;
  msg_set_rt_mask_rsp_type *rsp = NULL;
  uint32 *mask = NULL;
  unsigned int rsp_len = req_len;   /* Same as request */
  int i;
  boolean success = TRUE;

/*-------------------------------------------------------------------------*/

  if (req && req->sub_cmd == MSG_EXT_SUBCMD_SET_RT_MASK &&
      req->ssid_start <= req->ssid_end)
  {
    rsp = (msg_set_rt_mask_rsp_type *)
      diagpkt_alloc (DIAG_EXT_MSG_CONFIG_F, rsp_len);

    if (rsp)
    {
      memscpy ((void *) rsp, ((FPOS(msg_set_rt_mask_req_type,rt_mask)) + (4 * ((req->ssid_end - req->ssid_start)+1))), (void *) req, rsp_len);

      rsp->status = TRUE;   /* Initialize */

      mask = req->rt_mask;

      for (i = req->ssid_start; i <= req->ssid_end; i++)
      {
        success = msg_set_rt_mask_adv ((uint16) i, *mask, DIAG_STREAM_1, diag_cur_preset_id);

        if (success == FALSE)
        {
          rsp->status = FALSE;
        }

        mask++;         /* Advance to next entry in array */
      }

      /* Send the updated mask to every LSM registered with DCM. */
      (void)diag_client_maskchange(DIAG_MSGSVC_MASK_CHANGE);

      (void) msg_sleep_vote ();
    }
  }
  else
  {
    rsp = (msg_set_rt_mask_rsp_type *)
      diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt, req_len);
  }

  return (rsp);

}               /* msg_pkt_set_rt_masks() */

/* Function to set run-time masks based on stream_id */

static void *
msg_pkt_set_rt_masks_adv (void *req_pkt, word req_len)
{
  msg_set_rt_mask_adv_req_type *req = (msg_set_rt_mask_adv_req_type *) req_pkt;
  msg_set_rt_mask_adv_rsp_type *rsp = NULL;
  uint32 *mask = NULL;
  unsigned int rsp_len = req_len;   /* Same as request */
  int i;
  boolean success = TRUE;
  boolean send_error = TRUE;
  byte stream_id;
  uint8 preset_id;
  
/*-------------------------------------------------------------------------*/

  if (req && (req->header.opcode == MSG_EXT_SUBCMD_SET_RT_MASK) && (req->ssid_first <= req->ssid_last) )
  {
    if( req->header.cmd_version == 1 )
    {
      if( (req->id.stream_id >= DIAG_MIN_STREAM_ID) && (req->id.stream_id <= DIAG_MAX_STREAM_ID) )
      {
        send_error = FALSE;
        stream_id = req->id.stream_id;
        preset_id = diag_cur_preset_id;
      }
    }
    else if( req->header.cmd_version == 2 )
    {
      if( (req->id.preset_id >= DIAG_MIN_PRESET_ID) && (req->id.preset_id <= DIAG_MAX_PRESET_ID) )
      {
        send_error = FALSE;
        stream_id = DIAG_STREAM_1;
        preset_id = req->id.preset_id;
      }
    }
  }
  
  if( send_error )
  {
    rsp = (msg_set_rt_mask_adv_rsp_type *)
      diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt, req_len);
  }
  else
  {
    rsp = (msg_set_rt_mask_adv_rsp_type *)
          diagpkt_subsys_alloc (DIAG_SUBSYS_DIAG_SERV, DIAG_EXT_MSG_CONFIG_ADV, rsp_len);

    if (rsp)
    {
	  memscpy ((void *) rsp, ((FPOS(msg_set_rt_mask_adv_req_type,rt_mask)) + (4 * ((req->ssid_last - req->ssid_first)+1))), (void *) req, rsp_len);

      rsp->status = TRUE;   /* Initialize */

      mask = (uint32 *)(req->rt_mask);

      for (i = req->ssid_first; i <= req->ssid_last; i++)
      {
        success = msg_set_rt_mask_adv ((uint16) i, *mask, stream_id, preset_id);

        if (success == FALSE)
        {
          rsp->status = FALSE;
        }

        mask++;         /* Advance to next entry in array */
      }

      /* Send the updated mask to every LSM registered with DCM. */
      (void)diag_client_maskchange(DIAG_MSGSVC_MASK_CHANGE);

      (void) msg_sleep_vote ();
    }
  }

  return (rsp);

} /* msg_pkt_set_rt_masks_adv */


/*===========================================================================

FUNCTION   MSG_PKT_SET_ALL_MASKS

DESCRIPTION
  This function will SET the  values of the "tool" masks for all
  the valid SubSystem IDs and prepare a responce packet.

DEPENDENCIES
  req is not NULL and points to a valid  request packet

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
static void *
msg_pkt_set_all_masks (void *req_pkt, word req_len)
{
  msg_set_all_masks_req_type *req = (msg_set_all_masks_req_type *) req_pkt;
  msg_set_all_masks_rsp_type *rsp = NULL;
  const unsigned int rsp_len = sizeof (msg_set_all_masks_rsp_type);

/*-------------------------------------------------------------------------*/

  if (req && req->sub_cmd == MSG_EXT_SUBCMD_SET_ALL_RT_MASKS)
  {
    rsp = (msg_set_all_masks_rsp_type *)
      diagpkt_alloc (DIAG_EXT_MSG_CONFIG_F, rsp_len);

    /*-------------------------------------------------------------------------
      fill in the fields of the responce packet
    -------------------------------------------------------------------------*/
    if(rsp)
    {
	    rsp->sub_cmd = req->sub_cmd;
	    rsp->status = TRUE;
	    rsp->rsvd = 0;
	    rsp->rt_mask = req->rt_mask;
    }
    /*-------------------------------------------------------------------------
      set all the real time masks to the new requested value.
    -------------------------------------------------------------------------*/
    msg_set_all_rt_masks_adv (req->rt_mask, DIAG_STREAM_1, diag_cur_preset_id);
  }
  else
  {
    rsp = (msg_set_all_masks_rsp_type *)
      diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt, req_len);
  }

  return (rsp);

}               /* msg_pkt_set_all_masks() */

static void *
msg_pkt_set_all_masks_adv (void *req_pkt, word req_len)
{
  byte stream_id;
  uint8 preset_id;
  boolean send_error = TRUE;
  unsigned int rsp_len=0;
  msg_set_all_masks_adv_rsp_type *rsp = NULL;
  msg_set_all_masks_adv_req_type *req = (msg_set_all_masks_adv_req_type *) req_pkt;
  
  rsp_len = sizeof (msg_set_all_masks_adv_rsp_type);
  
  /*-------------------------------------------------------------------------*/

  if (req && (req->header.opcode == MSG_EXT_SUBCMD_SET_ALL_RT_MASKS) )
  {
    if( req->header.cmd_version == 1 )
    {
      if(req->id.stream_id <= DIAG_MAX_STREAM_ID) 
      {
        send_error = FALSE;
        stream_id = req->id.stream_id;
        preset_id = diag_cur_preset_id;
      }
    }
    else if( req->header.cmd_version == 2 )
    {
      if(req->id.preset_id <= DIAG_MAX_PRESET_ID) 
      {
        send_error = FALSE;
        stream_id = DIAG_STREAM_1;
        preset_id = req->id.preset_id;
      }
    }
  }
  
  if( send_error )
  {
    rsp = (msg_set_all_masks_adv_rsp_type *)
      diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt, req_len);
  }
  else
  {
    rsp = (msg_set_all_masks_adv_rsp_type *)
          diagpkt_subsys_alloc (DIAG_SUBSYS_DIAG_SERV, DIAG_EXT_MSG_CONFIG_ADV, rsp_len);

    /*-------------------------------------------------------------------------
      fill in the fields of the responce packet
    -------------------------------------------------------------------------*/
    if(rsp)
    {
      rsp->header = req->header;
      rsp->status = TRUE;
      rsp->rt_mask = req->rt_mask;
      
      if( req->header.cmd_version == 1 )
        rsp->id.stream_id = req->id.stream_id;
      else if( req->header.cmd_version == 2 )
        rsp->id.preset_id = req->id.preset_id;
    }
    /*-------------------------------------------------------------------------
      set all the real time masks to the new requested value.
    -------------------------------------------------------------------------*/
    msg_set_all_rt_masks_adv (req->rt_mask, stream_id, preset_id);
  }

  return (rsp);

} /* msg_pkt_set_all_masks_adv() */


/*===========================================================================

FUNCTION MSG_PKT_PROCESS_CONFIG

DESCRIPTION
  This function will figure out the type of extended message request
  by examining the message command and then call the appropriate
  function to handle the request.

DEPENDENCIES

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
void *
msg_pkt_process_config (void * req_pkt, word req_len)
{
  msg_get_ssid_ranges_req_type *req =
    (msg_get_ssid_ranges_req_type *) req_pkt;
  void *rsp = NULL;
  boolean valid_cmd = TRUE;
  boolean send_rsp = TRUE;
  boolean set_mask_req = FALSE;
    
  /*-------------------------------------------------------------------------*/
  
  if (req && req->cmd_code == DIAG_EXT_MSG_CONFIG_F)
  {
    /* dispatch acording to subcommand */
    switch (req->sub_cmd)
    {
    case MSG_EXT_SUBCMD_GET_RANGES:

    /*---------------------------------------------------------------------
          This will prepare a packet with all valid SubSystem IDs.
        ---------------------------------------------------------------------*/
      rsp = msg_pkt_get_ssid_ranges (req, req_len);
      break;

    case MSG_EXT_SUBCMD_GET_BUILD_MASK:

    /*---------------------------------------------------------------------
          Will return a packet with the "build" mask for a specific SSID
        ---------------------------------------------------------------------*/
      rsp = msg_pkt_get_masks (req, req_len, MSG_MASK_BT);
      break;

    case MSG_EXT_SUBCMD_GET_RT_MASK:

    /*---------------------------------------------------------------------
          Returns a packet with the "tool" mask for a specific SSID
        ---------------------------------------------------------------------*/
      rsp = msg_pkt_get_masks (req, req_len, MSG_MASK_RT);

      break;

    case MSG_EXT_SUBCMD_SET_RT_MASK:

    /*---------------------------------------------------------------------
          forces mobile to set the "tool" mask for a specific SSID
        ---------------------------------------------------------------------*/
      rsp = msg_pkt_set_rt_masks (req, req_len);
      set_mask_req = TRUE;
	  break;

    case MSG_EXT_SUBCMD_SET_ALL_RT_MASKS:

    /*---------------------------------------------------------------------
          set "tool" masks for all valid SSIDs to a given value.
        ---------------------------------------------------------------------*/
      rsp = msg_pkt_set_all_masks (req, req_len);
      set_mask_req = TRUE;
      break;

    default:            /* nothing to do */
      rsp = (void *) diagpkt_err_rsp (DIAG_BAD_PARM_F, req, req_len);
      valid_cmd = TRUE;
      break;
    }

    if (valid_cmd)
    {
      msg_mode = MSG_MODE_STD;
    }
  }
  else
  {
    rsp = (void *) diagpkt_err_rsp (DIAG_BAD_PARM_F, req, req_len);
  }
  
 /* if master processor on multi processor build, don't send response */
  #if !defined(DIAG_RSP_SEND)
  if (!diagcomm_apps_only_config)
    send_rsp = FALSE;
  #endif

/*To supress compiler warnings for images other than the master*/
  if (set_mask_req)
  
 /* if diag is not connected, don't send response */
  if (!diagcomm_cmd_status(DIAGCOMM_PORT_1))
  {
    send_rsp = FALSE;
  }

  #if defined (FEATURE_DIAG_ADSP_CALLBACK)
      adsp_dspdiag125_callback(req_pkt, req_len);
  #endif /* FEATURE_DIAG_ADSP_CALLBACK */
 
  /* return response */
  if (!send_rsp)
  {
    diagpkt_free(rsp);
    return NULL;
  }
  else
  {
    return ((void *) rsp);
  }

} /* msg_pkt_process_config */


/*===========================================================================

FUNCTION EXT_MSG_PKT_CONFIG_ADV

DESCRIPTION
  Function to handle dual mask msg config requests.

DEPENDENCIES

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
void *
ext_msg_pkt_config_adv (void * req_pkt, word req_len)
{
  ext_config_header *req = (ext_config_header *) req_pkt;
  void *rsp = NULL;
  boolean valid_cmd = TRUE;
  boolean send_rsp = TRUE;
  
  if ( req && (req->subsys_cmd_code == DIAG_EXT_MSG_CONFIG_ADV) 
           && ((req->cmd_version == 1) || (req->cmd_version == 2)) )
  {
    
    /* dispatch acording to subcommand */
    switch (req->opcode)
    {
    case MSG_EXT_SUBCMD_GET_RANGES:

    /*---------------------------------------------------------------------
          This will prepare a packet with all valid SubSystem IDs.
        ---------------------------------------------------------------------*/
      rsp = msg_pkt_get_ssid_ranges_adv (req, req_len);
      break;

    case MSG_EXT_SUBCMD_GET_BUILD_MASK:

    /*---------------------------------------------------------------------
          Will return a packet with the "build" mask for a specific SSID
        ---------------------------------------------------------------------*/
      rsp = msg_pkt_get_masks_adv (req, req_len, MSG_MASK_BT);
      break;

    case MSG_EXT_SUBCMD_GET_RT_MASK:

    /*---------------------------------------------------------------------
          Returns a packet with the "tool" mask for a specific SSID
        ---------------------------------------------------------------------*/
      rsp = msg_pkt_get_masks_adv (req, req_len, MSG_MASK_RT);

      break;

    case MSG_EXT_SUBCMD_SET_RT_MASK:

    /*---------------------------------------------------------------------
          forces mobile to set the "tool" mask for a specific SSID
        ---------------------------------------------------------------------*/
      rsp = msg_pkt_set_rt_masks_adv (req, req_len);
	  break;

    case MSG_EXT_SUBCMD_SET_ALL_RT_MASKS:

    /*---------------------------------------------------------------------
          set "tool" masks for all valid SSIDs to a given value.
        ---------------------------------------------------------------------*/
      rsp = msg_pkt_set_all_masks_adv (req, req_len);
      break;

    default:            /* nothing to do */
      rsp = (void *) diagpkt_err_rsp (DIAG_BAD_PARM_F, req, req_len);
      valid_cmd = TRUE;
      break;
    }

    if (valid_cmd)
    {
      msg_mode = MSG_MODE_STD;
    }
  }
  else
  {
    rsp = (void *) diagpkt_err_rsp (DIAG_BAD_PARM_F, req, req_len);
  }

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

  #if defined (FEATURE_DIAG_ADSP_CALLBACK)
      adsp_dspdiag125_callback(req_pkt, req_len);
  #endif /* FEATURE_DIAG_ADSP_CALLBACK */
 
  /* return response */
  if (!send_rsp)
  {
    diagpkt_free(rsp);
    return NULL;
  }
  else
  {
    return ((void *) rsp);
  }
} /* ext_msg_pkt_config_adv */


/*===========================================================================
 
FUNCTION MSG_MASKS_ENABLED_CHECK

DESCRIPTION
  This function checks the runtime masks for all SSIDs for a particular stream
  to determine if the message service is "off".  It will return TRUE if the 
  service is "off", false otherwise. 

===========================================================================*/
static boolean            
msg_masks_enabled_check(byte stream_id)
{
   boolean masks_enabled = FALSE;
   uint32 msg_mask_tbl_index, ssid_index, ssid_count;
   
   if ( (stream_id > DIAG_MAX_STREAM_ID) || (stream_id < DIAG_MIN_STREAM_ID) )
   {
     MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "msg_masks_enabled_check Sanity check failed");
     return FALSE;
   }
   
   for (msg_mask_tbl_index = 0; !masks_enabled && msg_mask_tbl_index < MSG_MASK_TBL_CNT; msg_mask_tbl_index++)
   {
    ssid_count = (uint32) (msg_mask_tbl[msg_mask_tbl_index].ssid_last - msg_mask_tbl[msg_mask_tbl_index].ssid_first + 1);
    for(ssid_index = 0; !masks_enabled && ssid_index < ssid_count; ssid_index++)
    {
      /* Set the masks_enabled flag to TRUE when a mask is set in msg_mask_tbl */
      if(msg_mask_tbl[msg_mask_tbl_index].rt_mask_array[stream_id-1][ssid_index])
      {
        masks_enabled = TRUE;
      }
    }
  }
  
  /* Sets/Reset the corresponding bit in F3 Enabled Mask */
  if(masks_enabled)
  {
    if (stream_id == DIAG_STREAM_1)
    {
      DIAG_SET_MASK(msg_enabled_mask, DIAG_MSG_STREAM_1);
    }
    #ifdef DIAG_STREAM_2_MASK_CHECK
    else if (stream_id == DIAG_STREAM_2)
    {
      DIAG_SET_MASK(msg_enabled_mask, DIAG_MSG_STREAM_2);
    }
    #endif
  }
  else
  {
    if (stream_id == DIAG_STREAM_1)
    {
      DIAG_CLR_MASK(msg_enabled_mask, DIAG_MSG_STREAM_1);
    }
    #ifdef DIAG_STREAM_2_MASK_CHECK
    else if (stream_id == DIAG_STREAM_2)
    {
      DIAG_CLR_MASK(msg_enabled_mask, DIAG_MSG_STREAM_2);
    }
    #endif
  }
  
  return masks_enabled;
  
} /* msg_masks_enabled_check */


/*===========================================================================
 
FUNCTION MSG_SLEEP_VOTE

DESCRIPTION
  This function checks the runtime masks for all SSIDs to determine if the
  message service is "off".  If "off", it will clear the corresponding bit
  in the msg_enabled_mask bitmask; else will set the corresponding bit
  and it will vote to the diagnostic task that it is OK to sleep.

===========================================================================*/
static boolean
msg_sleep_vote (void)
{
  boolean masks_enabled_stream_1 =  FALSE;
  #ifdef DIAG_STREAM_2_MASK_CHECK
  boolean masks_enabled_stream_2 =  FALSE;
  #endif

  masks_enabled_stream_1 = msg_masks_enabled_check(DIAG_STREAM_1);
  #ifdef DIAG_STREAM_2_MASK_CHECK
  masks_enabled_stream_2 = msg_masks_enabled_check(DIAG_STREAM_2);
  #endif
  
  /* Vote for sleep only if Stream 1 Masks are not enabled */
  if( masks_enabled_stream_1 )
  {
    diag_sleep_vote (DIAG_SLEEP_MSG, FALSE);
  }
  else
  {
    diag_sleep_vote (DIAG_SLEEP_MSG, TRUE);
  }
  
  return !masks_enabled_stream_1;  /* If masks are disabled, vote for sleep */
  
} /* msg_sleep_vote */


/* Legacy MSG service packet handler */

/*===========================================================================

FUNCTION MSG_PKT_LEGACY_REQ

DESCRIPTION
  This procedure processes a request to retreive the accumulated msgs in
  the mobile's msg buffer.

===========================================================================*/
void *
msg_pkt_legacy_req (void * req_ptr, word pkt_len)
{
  msg_legacy_req_type *req = (msg_legacy_req_type *) req_ptr;
  msg_legacy_rsp_type *rsp = NULL;
  const unsigned int rsp_len = sizeof (msg_legacy_rsp_type);
  boolean send_rsp = TRUE;

  (void) pkt_len; /* suppress compiler warning */


#if defined (MSG_LEVEL) && (MSG_LEVEL != MSG_LVL_NONE)
  /* Suppress msg_file compiler warning. ** This compiler optimizes this to
     nothing, ** but msg_file is now referenced. */
#endif

  /* Legacy values for level are used to index this array. Legacy values were:
     #define MSG_LVL_LOW     0 #define MSG_LVL_MED     1 #define MSG_LVL_HIGH 2
     #define MSG_LVL_ERROR   3 #define MSG_LVL_FATAL   4 #define MSG_LVL_NONE
     255 */

  if (req->msg_level <
      sizeof (msg_legacy_rt_masks) / sizeof (msg_legacy_rt_masks[0]))
  {
    msg_set_all_rt_masks_adv (msg_legacy_rt_masks[req->msg_level], DIAG_STREAM_1, diag_cur_preset_id);
  }
  else
  {
    msg_set_all_rt_masks_adv (0, DIAG_STREAM_1, diag_cur_preset_id);
  }

  (void) msg_sleep_vote ();

  /* Set mode to legacy mode. */
  msg_mode = MSG_MODE_LEGACY;

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


  /*----------------------------------------------------------------------
    Format the response packet.
  ----------------------------------------------------------------------*/
  if (send_rsp)
  {
    rsp = (msg_legacy_rsp_type *) diagpkt_alloc (DIAG_MSG_F, rsp_len);

    if (rsp)
    {
      rsp->total_msgs = msg_total;

      /*----------------------------------------------------
      Since we are streaming, this response is an ACK.
      Set qty to 0.
      ----------------------------------------------------*/
      rsp->qty = 0;

      rsp->drop_cnt = msg_dropped;

    }
  }
  
  return ((void *) rsp);
  

}               /* msg_pkt_legacy_req */

#if defined(T_WINNT)

/*===========================================================================
FUNCTION MSG_FORMAT_FILENAME

DESCRIPTION
   Formats a filename for printing. If a pathname is specified, removes
   all components but the first. If the filename is too long, then its
   first three characters are replaced by ".".

===========================================================================*/
static void
msg_format_filename (const char *input_filename, char *output_filename)
{
  const char *p_front = input_filename;
  const char *p_end = input_filename + strlen (input_filename);

  while (p_end != p_front)
  {
    if ((*p_end == '\\') || (*p_end == ':'))
    {
      p_end++;
      break;
    }
    p_end--;
  }

  p_front = p_end;

  if (strlen (p_end) > MSG_FILE_LEN - 1)
  {
    p_end += strlen (p_end) - (MSG_FILE_LEN - 1);
  }

  strlcpy (output_filename, p_end, sizeof(output_filename));

  if (strlen (p_front) > (MSG_FILE_LEN - 1))
  {
    output_filename[0] = '.';
    output_filename[1] = '.';
    output_filename[2] = '.';
  }
}               /* END format_filename */
static const char *
msg_format_filename2 (const char *filename)
{
  const char *p_front = filename;
  const char *p_end = filename + strlen (filename);

  while (p_end != p_front)
  {
    if ((*p_end == '\\') || (*p_end == ':'))
    {
      p_end++;
      break;
    }
    p_end--;
  }

  return p_end;
}               /* END format_filename */
#endif /* T_WINNT */




/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====

		Mobile Diagnostic Message Services for QSHRINK feature
		
====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/* Statistics */
static uint32 qsr_msg_total;    /* total number of messages logged */


typedef enum
{
  QSR_MSG_EXPAND_STATE_HDR = 0,
  QSR_MSG_EXPAND_STATE_DESC,
  QSR_MSG_EXPAND_STATE_HASH_VALUE,
  QSR_MSG_EXPAND_STATE_ARGS,
  QSR_MSG_EXPAND_STATE_COMPLETE
}
qsr_msg_expand_enum_type;



/*===========================================================================

FUNCTION MSG_QSR_EXPAND_AND_SEND_PKT

DESCRIPTION
  This routine expands the intermediate storage format into the full
  packet format, copying data from the constant block in 'stored_msg'.

DEPENDENCIES
  msg_init() must have been called previously.

RETURN VALUE

===========================================================================*/
static void
msg_qsr_expand_and_send_pkt (diag_send_desc_type *desc,byte stream_id)
{
  msg_qsr_store_type *msg_store = NULL;
  diagbuf_status_enum_type status = DIAGBUF_READY_S;
  
  static diag_send_desc_type send = {NULL, NULL, DIAG_SEND_STATE_START, FALSE};
  static qsr_msg_expand_enum_type state_sav = QSR_MSG_EXPAND_STATE_HDR; /* Used to store state of process. */
  int filename_len; 
  char* str_buf;

  /* Local copies of static variables for optimized access.  
     This makes this routine persistent but not re-entrant. */
  qsr_msg_expand_enum_type state = state_sav;


  if (desc)
  {
    /* 'decs' is not moved until the entire message is complete.  Therefore,
       it always points to the stored message, in terse format. */
    msg_store = (msg_qsr_store_type *) desc->pkt;
	
    if (desc->state == DIAG_SEND_STATE_START)
    {
      /* Increment state to indicate the packet is now in progress. */
      desc->state++;

      /* Keep stats of total messages */
      qsr_msg_total++;

      if (msg_store)
      {
        /* Add dropped messages to the total. */
        qsr_msg_total += msg_store->hdr.drop_cnt;
      }
      
      if (state != QSR_MSG_EXPAND_STATE_HDR)
      {
        /* This should never happen.  However, if we start over, just kill the
           last message in progress */
        ERR ("New message while in progress", 0, 0, 0);

        desc->pkt = (void *) ((byte *) desc->last + 1);
        desc->state = DIAG_SEND_STATE_COMPLETE;
        state = QSR_MSG_EXPAND_STATE_HDR;   /* reset state */
      }
    }

    /* The packet is expanded in fragments.  'state' marks each stage of
       the process. */
      if ((msg_store) && ((msg_store->qsr_flag & QSR_BITFLAG_IS_HASH) && 
          (status == DIAGBUF_READY_S && state == QSR_MSG_EXPAND_STATE_HDR)))
    {
      if (send.pkt == NULL && msg_store)
      {
        /* Set the command code since it was stored as MSG_EXT_TERSE_F. */
        msg_store->hdr.cmd_code = DIAG_QSR_EXT_MSG_TERSE_F;

        /* Set up the send descriptor for this fragment. */
        send.pkt = (void *) &msg_store->hdr;
        send.last = (void *) ((byte *) send.pkt + sizeof (msg_store->hdr) - 1);
        send.state = DIAG_SEND_STATE_START;
        send.terminate = FALSE;
        send.diag_id = desc->diag_id;
      }

      (void) diagbuf_send_pkt (&send,stream_id);

      if (send.state == DIAG_SEND_STATE_COMPLETE)
      {
        state++;
        send.pkt = NULL;
      }
    }

      if((msg_store) && (!(msg_store->qsr_flag & QSR_BITFLAG_IS_HASH)) && 
          status == DIAGBUF_READY_S && state == QSR_MSG_EXPAND_STATE_HDR)
      {
        /*  
         *  Branch on general behavior between actual hashes and addresses for 
         *  QShrink20
         */      
        if (send.pkt == NULL && msg_store)
        {
          /*
                  * if last 2 bits of hash == 00, then treat as address to string
                  * mimic msg_expand_and_send_pkt
                  * build/ms/Qshrink.py replacement mode
              
                  * have "New message while in progress" error still work
                  */
          state++; 
                          
          /* send header */
          msg_store->hdr.cmd_code = DIAG_EXT_MSG_F;
             
          send.pkt = (void *) &msg_store->hdr;
          send.last = (void *) ((byte *) send.pkt + sizeof (msg_store->hdr) - 1);
          send.state = DIAG_SEND_STATE_START;
          send.terminate = FALSE;
          send.diag_id = desc->diag_id;
             
          (void) diagbuf_send_pkt (&send, stream_id);
             
          if (send.state != DIAG_SEND_STATE_COMPLETE)
          {
            send.pkt = NULL;
            desc->pkt = (void *) ((byte *) desc->last + 1);
            desc->state = DIAG_SEND_STATE_COMPLETE;
            return;
          }
             
          /* send description */
          send.pkt = (void*)&msg_store->qsr_const_data_ptr->desc;
          send.last = (void *) ((byte *) send.pkt +
          sizeof (msg_store->qsr_const_data_ptr->desc) - 1);
              
          send.state = DIAG_SEND_STATE_BUSY;
          send.terminate = FALSE;
            send.diag_id = desc->diag_id;
              
          (void) diagbuf_send_pkt (&send, stream_id);
              
          if (send.state != DIAG_SEND_STATE_COMPLETE)
          {
            send.pkt = NULL;
            desc->pkt = (void *) ((byte *) desc->last + 1);
            desc->state = DIAG_SEND_STATE_COMPLETE;
            return;
          }
              
          /* send args */
          send.pkt = (void *) msg_store->args;
          send.last = (void *) ((byte *) send.pkt +
          sizeof (msg_store->args[0]) * msg_store->hdr.num_args - 1);
          
          send.state = DIAG_SEND_STATE_BUSY;
            send.diag_id = desc->diag_id;
         
          (void) diagbuf_send_pkt (&send, stream_id);
              
          if (send.state != DIAG_SEND_STATE_COMPLETE)
          {
            send.pkt = NULL;
            desc->pkt = (void *) ((byte *) desc->last + 1);
            desc->state = DIAG_SEND_STATE_COMPLETE;
            return;
          }
              
          // send fmt
          // current == msg_hash -> "filename:string"
          // want    == send.pkt -> "string"
          for (str_buf = (char *) msg_store->qsr_const_data_ptr
                                                  -> msg_hash; *str_buf != ':'; str_buf++);
      
          filename_len = str_buf - (char *)msg_store->qsr_const_data_ptr
                                                  -> msg_hash;
                                                  
          send.pkt = (void *) (msg_store->qsr_const_data_ptr->msg_hash + filename_len + 1);
              
          send.last = (void *) ((byte *) send.pkt +
                  MIN (MSG_MAX_STRLEN, (int)strlen ((char *) send.pkt)));
              
          send.state = DIAG_SEND_STATE_BUSY;
            send.diag_id = desc->diag_id;
              
          (void) diagbuf_send_pkt (&send, stream_id);
              
          if (send.state != DIAG_SEND_STATE_COMPLETE)
          {
            send.pkt = NULL;
            desc->pkt = (void *) ((byte *) desc->last + 1);
            desc->state = DIAG_SEND_STATE_COMPLETE;
            return;
          }
              
          // send fmt_null
          if (*((byte *) send.last) != 0)
          {
            send.last = send.pkt = &desc->terminate;
            desc->terminate = 0;
              
            send.state = DIAG_SEND_STATE_BUSY;
              send.diag_id = desc->diag_id;
              
            (void) diagbuf_send_pkt (&send, stream_id);
          
            if (send.state != DIAG_SEND_STATE_COMPLETE)
            {
              send.pkt = NULL;
              desc->pkt = (void *) ((byte *) desc->last + 1);
              desc->state = DIAG_SEND_STATE_COMPLETE;
              return;
            }
          }
              
          // send file name
          send.pkt = (void *) msg_store->qsr_const_data_ptr->msg_hash;
          send.last = (void *) ((byte *) send.pkt +
            MIN (MSG_MAX_STRLEN, filename_len - 1));
              
          send.state = DIAG_SEND_STATE_BUSY;
              
          if (*((byte *) send.last) == 0)
          {
            send.terminate = TRUE;
          }
          else
          {
            send.terminate = FALSE;
          }
            send.diag_id = desc->diag_id;
              
          (void) diagbuf_send_pkt (&send, stream_id);
              
          if (send.state != DIAG_SEND_STATE_COMPLETE)
          {
            send.pkt = NULL;
            desc->pkt = (void *) ((byte *) desc->last + 1);
            desc->state = DIAG_SEND_STATE_COMPLETE;
            return;
          }
              
          // send file name null
          if (send.terminate != TRUE)
          {
            send.last = send.pkt = &desc->terminate;
            desc->terminate = 0;
           
            send.state = DIAG_SEND_STATE_BUSY;
            send.terminate = TRUE;
              send.diag_id = desc->diag_id;
              
            (void) diagbuf_send_pkt (&send, stream_id);
           
            if (send.state != DIAG_SEND_STATE_COMPLETE)
            {
              send.pkt = NULL;
              desc->pkt = (void *) ((byte *) desc->last + 1);
              desc->state = DIAG_SEND_STATE_COMPLETE;
              return;
            }  
          }
        }
                
        // send complete
        state = QSR_MSG_EXPAND_STATE_HDR;
        desc->pkt = (void *) ((byte *) desc->last + 1);
        desc->state = DIAG_SEND_STATE_COMPLETE;
        send.pkt = NULL;
              
        state_sav = state;
              
        return;
      }

    if (status == DIAGBUF_READY_S && state == QSR_MSG_EXPAND_STATE_DESC)
    {
      if (send.pkt == NULL && msg_store)
      {
        /* Set up the send descriptor for this fragment. */
        send.pkt = (void*)&msg_store->qsr_const_data_ptr->desc;
        send.last = (void *) ((byte *) send.pkt + 
          sizeof (msg_store->qsr_const_data_ptr->desc) - 1);
        
        send.state = DIAG_SEND_STATE_BUSY;
		send.terminate = FALSE;
            send.diag_id = desc->diag_id;
      }

      (void) diagbuf_send_pkt (&send,stream_id);

      if (send.state == DIAG_SEND_STATE_COMPLETE)
      {
        state++;
        send.pkt = NULL;
      }
    }

   if (status == DIAGBUF_READY_S && state == QSR_MSG_EXPAND_STATE_HASH_VALUE)
    {
      if (send.pkt == NULL && msg_store)
      {
        /* Set up the send descriptor for this fragment. */
        send.pkt = (void *)&msg_store->qsr_const_data_ptr->msg_hash;
        send.last = (void *) ((byte *) send.pkt + 
          sizeof (msg_store->qsr_const_data_ptr->msg_hash)-1);
        
        send.state = DIAG_SEND_STATE_BUSY;
		send.terminate = FALSE;
            send.diag_id = desc->diag_id;
      }

      (void) diagbuf_send_pkt (&send,stream_id);    

	  if (send.state == DIAG_SEND_STATE_COMPLETE)
      {
        state++;
        send.pkt = NULL;
      }
    }

	if (status == DIAGBUF_READY_S && state == QSR_MSG_EXPAND_STATE_ARGS)
		{
		  if (send.pkt == NULL && msg_store)
		  {
			/* Set up the send descriptor for this fragment. */
			send.pkt = (void *) msg_store->args;
			send.last = (void *) ((byte *) send.pkt + 
			  sizeof (msg_store->args[0]) * msg_store->hdr.num_args - 1);
			
			send.state = DIAG_SEND_STATE_BUSY;
			send.terminate= TRUE;
            send.diag_id = desc->diag_id;
		  }
	
		  (void) diagbuf_send_pkt (&send,stream_id);

	  if (send.state == DIAG_SEND_STATE_COMPLETE) 
	  {
	  	state = QSR_MSG_EXPAND_STATE_HDR;
		desc->pkt = (void *) ((byte *) desc->last + 1);
        desc->state = DIAG_SEND_STATE_COMPLETE;
		send.pkt = NULL;
	  }
    }
	
  }

  /* Commit local copies of persistent data back to static variables. */
  state_sav = state;
  
  return;
} /* msg_qsr_expand_and_send_pkt */


/*===========================================================================

FUNCTION DIAG_SWITCH_MSG_PRESET_MASK

DESCRIPTION
  Whenever 'diag_cur_preset_id' is updated via command/response or control 
  packet to switch the current Stream 1 preset mask id, we need to call this
  function to actually switch the current preset msg mask in use.
  
PARAMETERS
  None
  
RETURN VALUE
  None
  
===========================================================================*/
void diag_switch_msg_preset_mask()
{
  uint8 index;
  uint32 count;
  
  for( index = 0; index < MSG_MASK_TBL_CNT; index++ )
  {
    count = msg_mask_tbl[index].ssid_last - msg_mask_tbl[index].ssid_first + 1;
    msg_mask_tbl[index].rt_mask_array[DIAG_STREAM_1-1] = (uint32 *)&msg_mask_tbl[index].rt_base_ptr[DIAG_STREAM_1-1][(diag_cur_preset_id-1)*count];
  }
       
  (void)diag_client_maskchange(DIAG_MSGSVC_MASK_CHANGE);
  (void)msg_sleep_vote();
  
} /* diag_switch_msg_preset_mask */
/*===========================================================================

FUNCTION DIAG_CTRL_UPDATE_MSG_MASK

DESCRIPTION
  Given a new F3 mask, updates the local copy of the F3 mask with the
  new mask.

PARAMETERS
  ctrl_msg - Ptr to CTRL msg
  
RETURN VALUE
  None
  
===========================================================================*/

void diag_ctrl_update_msg_mask( diag_ctrl_msg_type * ctrl_msg )
{
  uint8 * mask_ptr = NULL;
  uint32 * rt_mask_ptr = NULL;
  uint8 index, stream_id, status = 0;
  uint16 ssid_first, ssid_last = 0;
  uint32 j, count, mask, msg_mask_size = 0;
  boolean found = FALSE;
  
  if( (ctrl_msg == NULL) || (ctrl_msg->cmd_type != DIAG_CTRL_MSG_F3_MASK_V2) )
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_ctrl_update_msg_mask Sanity check failed");
    return;
  }
  
  status = ctrl_msg->data.ctrl_msg_f3_mask.status;
  msg_mode = ctrl_msg->data.ctrl_msg_f3_mask.msg_mode;
  stream_id = ctrl_msg->data.ctrl_msg_f3_mask.stream_id;
  ssid_first = ctrl_msg->data.ctrl_msg_f3_mask.ssid_first;
  ssid_last = ctrl_msg->data.ctrl_msg_f3_mask.ssid_last;
  msg_mask_size = ctrl_msg->data.ctrl_msg_f3_mask.msg_mask_size;
  
  if( (stream_id > DIAG_MAX_STREAM_ID) || (stream_id < DIAG_MIN_STREAM_ID))
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_ctrl_update_msg_mask: Invalid stream_id %d", stream_id);
    return;
  }
 
  if( ctrl_msg->data_len != (sizeof(diag_ctrl_msg_f3_mask_type) + (msg_mask_size * sizeof(uint32))) )
  {
    MSG_2( MSG_SSID_DIAG, MSG_LEGACY_ERROR, 
             "diag_ctrl_update_msg_mask: Invalid lengths %d != %d", 
             ctrl_msg->data_len, sizeof(diag_ctrl_msg_f3_mask_type) + (msg_mask_size * sizeof(uint32)) );
    return;
  }
    
  if( status == DIAG_CTRL_MASK_ALL_DISABLED )
  {
    for( index=0; !found && (index < MSG_MASK_TBL_CNT); index++ )
    {
      if( ssid_first == msg_mask_tbl[index].ssid_first )
      {
        found = TRUE;
        count = (uint32)(msg_mask_tbl[index].ssid_last - msg_mask_tbl[index].ssid_first + 1);
        for( j=0; j < count; j++ )
        {
          msg_mask_tbl[index].rt_mask_array[stream_id-1][j] = 0;
        }
        /*update msg composite mask*/
        diag_update_composite_msg_mask(index, 0, count-1);

      }
    }
  }
  else if( status == DIAG_CTRL_MASK_ALL_ENABLED )
  {
    if( msg_mask_size > 0 )
    {
      mask_ptr = ((uint8 *)ctrl_msg) + DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_f3_mask_type);
      memscpy( (void *)&mask,sizeof(uint32), mask_ptr, sizeof(uint32) );
    }
    else
    {
      MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_ctrl_update_msg_mask: No mask to update");
      return;
    }
    
    for( index=0; !found && (index < MSG_MASK_TBL_CNT); index++ )
    {
      if( ssid_first == msg_mask_tbl[index].ssid_first )
      {
        found = TRUE;
        count = (uint32)(msg_mask_tbl[index].ssid_last - msg_mask_tbl[index].ssid_first + 1);
        for( j=0; j < count; j++ )
        {
          /* Update all with same mask */
          msg_mask_tbl[index].rt_mask_array[stream_id-1][j] = mask;
        }
	/*update msg composite mask*/
        diag_update_composite_msg_mask(index, 0, count-1);

      }
    }
  }
  else if( status == DIAG_CTRL_MASK_VALID )
  {
    for( index=0; !found && (index < MSG_MASK_TBL_CNT); index++ )
    {
      if( ssid_first == msg_mask_tbl[index].ssid_first )
      {
        found = TRUE;
        mask_ptr = ((uint8 *)ctrl_msg) + DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_f3_mask_type);
        rt_mask_ptr = (uint32 *)&(msg_mask_tbl[index].rt_mask_array[stream_id-1][0]);
        count = MIN(msg_mask_size, msg_mask_tbl[index].ssid_last - msg_mask_tbl[index].ssid_first + 1);
        memscpy( (void *)rt_mask_ptr, count * sizeof(uint32),mask_ptr, count * sizeof(uint32));
	  /*update msg composite mask*/
      diag_update_composite_msg_mask(index,0,count-1);
      }
    }
  }
  
  if( !found )
  {
    MSG_3( MSG_SSID_DIAG, MSG_LEGACY_ERROR, 
           "update_msg_mask not found: stream_id=%d, ssid_first=%d, ssid_last=%d", 
           stream_id, ssid_first, ssid_last );
    return;
  }
  
  /* Set the masks of all clients registered with DCM.*/
  (void)diag_client_maskchange(DIAG_MSGSVC_MASK_CHANGE);
  (void)msg_sleep_vote();
    
} /* diag_ctrl_update_msg_mask */

/*===========================================================================

FUNCTION DIAG_CTRL_UPDATE_MSG_PRESET_MASK

DESCRIPTION
  Given a new F3 mask, updates the local copy of the Stream 1 preset F3 mask 
  with the new mask.

PARAMETERS
  ctrl_msg - Ptr to CTRL msg
  
RETURN VALUE
  None
  
===========================================================================*/

void diag_ctrl_update_msg_preset_mask( diag_ctrl_msg_type * ctrl_msg )
{
  uint8 * mask_ptr = NULL;
  uint32 * rt_mask_ptr = NULL;
  uint8 index, stream_id, preset_id, status = 0;
  uint16 ssid_first, ssid_last = 0;
  uint32 j, count, mask, msg_mask_size = 0;
  boolean found = FALSE;
  
  if( (ctrl_msg == NULL) || (ctrl_msg->cmd_type != DIAG_CTRL_MSG_F3_MASK_V3) )
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_ctrl_update_msg_preset_mask Sanity check failed");
    return;
  }
  
  status = ctrl_msg->data.ctrl_msg_f3_mask_v3.status;
  msg_mode = ctrl_msg->data.ctrl_msg_f3_mask_v3.msg_mode;
  stream_id = ctrl_msg->data.ctrl_msg_f3_mask_v3.stream_id;
  preset_id = ctrl_msg->data.ctrl_msg_f3_mask_v3.preset_id;
  ssid_first = ctrl_msg->data.ctrl_msg_f3_mask_v3.ssid_first;
  ssid_last = ctrl_msg->data.ctrl_msg_f3_mask_v3.ssid_last;
  msg_mask_size = ctrl_msg->data.ctrl_msg_f3_mask_v3.msg_mask_size;
  
  if( (stream_id > DIAG_MAX_STREAM_ID) ||(stream_id < DIAG_MIN_STREAM_ID)|| (preset_id > DIAG_MAX_PRESET_ID) )
  {
    MSG_2( MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_ctrl_update_msg_preset_mask: Invalid stream_id %d or preset_id %d", 
           stream_id, preset_id );
    return;
  }
   
  if( stream_id == DIAG_STREAM_2 )
  {
    preset_id = DIAG_DCI_MASK_1;
  }
  
  if( ctrl_msg->data_len != (sizeof(diag_ctrl_msg_f3_mask_v3_type) + (msg_mask_size * sizeof(uint32))) )
  {
    MSG_2( MSG_SSID_DIAG, MSG_LEGACY_ERROR, 
             "diag_ctrl_update_msg_preset_mask: Invalid lengths %d != %d", 
             ctrl_msg->data_len, sizeof(diag_ctrl_msg_f3_mask_v3_type) + (msg_mask_size * sizeof(uint32)) );
    return;
  }
    
  if( status == DIAG_CTRL_MASK_ALL_DISABLED )
  {
    for( index=0; !found && (index < MSG_MASK_TBL_CNT); index++ )
    {
      if( ssid_first == msg_mask_tbl[index].ssid_first )
      {
        found = TRUE;
        count = (uint32)(msg_mask_tbl[index].ssid_last - msg_mask_tbl[index].ssid_first + 1);
        for( j=0; j < count; j++ )
        {
          msg_mask_tbl[index].rt_base_ptr[stream_id-1][(preset_id-1)*count + j] = 0;
        }
     	/*update msg composite mask*/
        diag_update_composite_msg_mask(index, 0, count-1);
      }
    }
  }
  else if( status == DIAG_CTRL_MASK_ALL_ENABLED )
  {
    if( msg_mask_size > 0 )
    {
      mask_ptr = ((uint8 *)ctrl_msg) + DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_f3_mask_v3_type);
      memscpy( (void *)&mask, sizeof(uint32), mask_ptr, sizeof(uint32) );
    }
    else
    {
      MSG(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diag_ctrl_update_msg_preset_mask: No mask to update");
      return;
    }
    
    for( index=0; !found && (index < MSG_MASK_TBL_CNT); index++ )
    {
      if( ssid_first == msg_mask_tbl[index].ssid_first )
      {
        found = TRUE;
        count = (uint32)(msg_mask_tbl[index].ssid_last - msg_mask_tbl[index].ssid_first + 1);
        for( j=0; j < count; j++ )
        {
          /* Update all with same mask */
          msg_mask_tbl[index].rt_base_ptr[stream_id-1][(preset_id-1)*count + j] = mask;
        }
    	/*update msg composite mask*/
        diag_update_composite_msg_mask(index, 0, count-1);
      }
    }
  }
  else if( status == DIAG_CTRL_MASK_VALID )
  {
    for( index=0; !found && (index < MSG_MASK_TBL_CNT); index++ )
    {
      if( ssid_first == msg_mask_tbl[index].ssid_first )
      {
        found = TRUE;
        mask_ptr = ((uint8 *)ctrl_msg) + DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_msg_f3_mask_v3_type);
        count = msg_mask_tbl[index].ssid_last - msg_mask_tbl[index].ssid_first + 1;
        rt_mask_ptr = (uint32 *)&(msg_mask_tbl[index].rt_base_ptr[stream_id-1][(preset_id-1)*count]);
        memscpy( (void *)rt_mask_ptr,  count * sizeof(uint32),
                mask_ptr, MIN(msg_mask_size, count) * sizeof(uint32));
        /*update msg composite mask*/
        diag_update_composite_msg_mask(index, 0, count-1);
      }      
    }
  }
  
  if( !found )
  {
    MSG_4( MSG_SSID_DIAG, MSG_LEGACY_ERROR, 
           "diag_ctrl_update_msg_preset_mask not found: stream_id=%d preset_id=%d, ssid_first=%d, ssid_last=%d", 
           stream_id, preset_id, ssid_first, ssid_last );
    return;
  }
  
  /* Set the masks of all clients registered with DCM.*/
  (void)diag_client_maskchange(DIAG_MSGSVC_MASK_CHANGE);
  (void)msg_sleep_vote();
    
} /* diag_ctrl_update_msg_preset_mask */

/*===========================================================================

FUNCTION DIAG_CTRL_SEND_MSG_SSID_RANGE_CTRL_PKT

DESCRIPTION
  Sends build time msg ssid range information to Master Diag for build mask
  centralization support.
  
PARAMETERS
  None
 
RETURN VALUE
  None
  
===========================================================================*/
void diag_ctrl_send_msg_ssid_range_ctrl_pkt(void)
{
	 diag_ctrl_msg_type ctrl_msg;
	 uint8 i = 0;
	 boolean is_sent = FALSE;
	 uint32 bytes_written = 0;

	 if(bt_msg_ssid_range_fwd_status == DIAG_CTRL_INFO_FWD_STATUS_NOT_SENT)
	 {
		  /* Compose DIAG_CTRL_BT_MSG_SSID_RANGE packet */
		  ctrl_msg.cmd_type = DIAG_CTRL_BT_MSG_RANGE;
		  ctrl_msg.data_len = sizeof(diag_ctrl_bt_msg_ssid_range_type);
		  ctrl_msg.data.ctrl_bt_msg_ssid_range.version = DIAG_CTRL_MSG_BT_MASK_VER;
		  ctrl_msg.data.ctrl_bt_msg_ssid_range.num_ssid_ranges = MSG_MASK_TBL_CNT;

		  for (i = 0; i < MSG_MASK_TBL_CNT; i++) 
		  {
				ctrl_msg.data.ctrl_bt_msg_ssid_range.ssid_ranges[i].ssid_first = msg_mask_tbl[i].ssid_first;
				ctrl_msg.data.ctrl_bt_msg_ssid_range.ssid_ranges[i].ssid_last = msg_mask_tbl[i].ssid_last;
		  }

		  bytes_written = diagcomm_ctrl_send_mult_pkt( (void *)&ctrl_msg, DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_bt_msg_ssid_range_type), 
														  NULL, 0, DIAG_CTRL_USER_PKT_ID_0, DIAGCOMM_PORT_REMOTE_PROC, DIAGCOMM_PORT_1, TRUE, &is_sent );

		  if(bytes_written == 0)
				return;
		  else if(is_sent == TRUE)
				bt_msg_ssid_range_fwd_status = DIAG_CTRL_INFO_FWD_STATUS_SENT;
		  else /* bytes_written > 0 && is_sent == FALSE */
				bt_msg_ssid_range_fwd_status = DIAG_CTRL_INFO_FWD_STATUS_QUEUED;
	 }
}

/*===========================================================================

FUNCTION DIAG_CTRL_SEND_MSG_BT_MASK_CTRL_PKT

DESCRIPTION
  Sends F3 build time mask for all SSID's to Master Diag for build mask
  centralization support.
  
PARAMETERS
  None
 
RETURN VALUE
  None
  
===========================================================================*/
void diag_ctrl_send_msg_bt_mask_ctrl_pkt(void)
{

	 diag_ctrl_msg_type ctrl_msg;
	 uint8 i = 0;
	 boolean is_sent = FALSE;
	 const msg_mask_tbl_type *tbl_ptr = NULL;
	 uint32 userpkt_len = 0, bytes_written = 0;

	 for (i = 0; i < MSG_MASK_TBL_CNT; i++) 
	 {
		  tbl_ptr = &msg_mask_tbl[i];
		  if((tbl_ptr != NULL) && (bt_msg_mask_fwd_status[i] == DIAG_CTRL_INFO_FWD_STATUS_NOT_SENT))
		  {
				/* Compose DIAG_CTRL_BT_MSG_MASK packet */
				userpkt_len = ((msg_mask_tbl[i].ssid_last - msg_mask_tbl[i].ssid_first + 1) * sizeof(uint32));

				ctrl_msg.cmd_type = DIAG_CTRL_BT_MSG_MASK;
				ctrl_msg.data_len = sizeof(diag_ctrl_bt_msg_mask_type) + userpkt_len;
				ctrl_msg.data.ctrl_bt_msg_mask.version = DIAG_CTRL_MSG_BT_MASK_VER;
				ctrl_msg.data.ctrl_bt_msg_mask.range_cnt = 1;
				ctrl_msg.data.ctrl_bt_msg_mask.ssid_range.ssid_first = msg_mask_tbl[i].ssid_first;
				ctrl_msg.data.ctrl_bt_msg_mask.ssid_range.ssid_last = msg_mask_tbl[i].ssid_last;

				bytes_written = diagcomm_ctrl_send_mult_pkt( (void *)&ctrl_msg, DIAGCOMM_CTRL_MSG_HDR_SIZE + sizeof(diag_ctrl_bt_msg_mask_type), 
																(const void *)tbl_ptr->bt_mask_array, userpkt_len, DIAG_CTRL_USER_PKT_ID_0, DIAGCOMM_PORT_REMOTE_PROC, DIAGCOMM_PORT_1, FALSE, &is_sent );

				if(bytes_written == 0)
					 return;
				else if(is_sent == TRUE) /* bytes_written > 0 */
					 bt_msg_mask_fwd_status[i] = DIAG_CTRL_INFO_FWD_STATUS_SENT;
				else /* bytes_written > 0 && is_sent == FALSE */
					 bt_msg_mask_fwd_status[i] = DIAG_CTRL_INFO_FWD_STATUS_QUEUED;

		  }
	 }

	 /* No more msg mask tables to fwd; Send the current DSM item immediately and then quit */
	 (void)diagcomm_ctrl_send_mult_pkt( NULL, 0, NULL, 0, DIAG_CTRL_USER_PKT_ID_0, DIAGCOMM_PORT_REMOTE_PROC, 
											DIAGCOMM_PORT_1, TRUE, &is_sent );
	 if( is_sent )
	 {
		  /* Updates the status */
		  for(i = 0; i < MSG_MASK_TBL_CNT; i++)
		  {
				bt_msg_mask_fwd_status[i] = DIAG_CTRL_INFO_FWD_STATUS_SENT;
		  }
	 }
}


/*===========================================================================
FUNCTION DIAG_UPDATE_COMPOSITE_MSG_MASK
DESCRIPTION
  This function update the msg composite mask
 
DEPENDENCIES
 
PARAMETERS 
  index - index of the msg mask array that needs to be updated
  start - first SSID that needs to be updated in the given range
  end   - last SSID that needs to be updated in the given range
 
RETURN VALUE
  None
 
SIDE EFFECTS
  None.
===========================================================================*/
void diag_update_composite_msg_mask(int index, int start, int end)
{
  int count = 0;  
  
#ifdef DIAG_UIMAGE_MODE
  int j = 0;
  int mask_index;
  uint32 temp_mask = 0 ;
  uint8 stream_index;
#endif
  
  if(index < 0 || index >= MSG_MASK_TBL_CNT) 
  {
    return;
  }

  count = msg_mask_tbl[index].ssid_last - msg_mask_tbl[index].ssid_first + 1;
  if (end >= count) 
  {
    end = count-1;
  }
  
  if (start < 0 || start > end) 
  {
    return;
  }
  
  #ifdef DIAG_UIMAGE_MODE
  for(mask_index = start; mask_index <= end; mask_index++)
  {
    temp_mask = 0;
    for(stream_index = 0; stream_index < DIAG_MAX_STREAM_ID; stream_index++) 
    {
      temp_mask |= msg_mask_tbl[index].rt_mask_array[stream_index][mask_index];
      for (j = 0; j < DIAG_MAX_PRESET_ID; j++) 
      {
        temp_mask |= msg_mask_tbl[index].rt_base_ptr[stream_index][j * count + mask_index];
      }
    }

    diag_msg_composite_mask[index].rt_mask_array[mask_index] = temp_mask;
  }
  #endif
}
