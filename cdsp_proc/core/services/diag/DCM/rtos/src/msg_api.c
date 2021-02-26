/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     Mobile Diagnostic Message Services

General Description
  All the declarations, definitions, and routines necessary to support
  reporting messages for errors and debugging.

Initialzation and Sequencing Requirements


Copyright (c) 2010-2015, 2017 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*==========================================================================

                           Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/services/diag/DCM/rtos/src/msg_api.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/17/17   is      Relocated top level msg functions to micro_diagbuffer_msg_api.c
08/14/17   sp      Fix start up bug in diag task when hardware threads reduced
07/19/17   nk      Qshrink 2 support for uimage diag messages
06/05/17   nk      Added debug support for uimage in root PD
04/09/17   sp      Added support for extended logs in rootPD 
06/18/15   sa      Added support for time sync feature.
01/29/15   xy      Fixed compilation warnings
01/08/15   rs      Added check for msg_service_init. 
08/20/14   sr      Fixed issue with F3s not coming out 
07/06/14   ph      Added support for Diag over STM
03/02/13   sg      Support for QShrink 2.0
03/25/13   ph      Corrected msg_sprintf to use strlcpy().
01/25/13   rh      Migrated F3 Trace to Diag
12/07/12   is      Warnings fixes
11/15/12   is      Support for preset masks
07/10/12   ra      F3 Mask Check Optimization
07/05/12   sg      Changes to bring up diag Multi PD
05/25/11   vs     Added msg_status
12/20/10   mad    Changes for dual diag debug message mask
10/29/10   vs     Moved msg_get_time to diagtarget.h.
                  Other changes for debugtools for lock-less implementation
09/28/10   sg     Moved diag_time_get to diag_cfg.h
09/17/10   is      Migrate from deprecated clk driver APIs
08/25/10   sg      Fixed compiler warnings
08/18/10   vs      Merged in changes from Tim to add QXDM masking for f3_save
08/09/10   sg      Removed std_strlprintf() wrapper function calls
06/30/10   sg      Inclusion of err.h
03/29/10   sg      created file

============================================================================*/
#include "customer.h"
#include "comdef.h"

#include <stringl/stringl.h>  /*For usage of strlcpy() */
#include <stdarg.h>     /* support for variable argument lists */

#include "msg.h"        /* Message typedefs and prototypes */
#include "msg_qsr.h"

#include "diagcmd.h"    /* diag command codes */
#include "diagbuf_v.h"  /* diag outpout buffer allocation
                                 manager. */
#include "qw.h"         /* definition of qword */
#include "err.h"
#include <stdio.h>
#include <stdarg.h>  /* Definitions va_start, va_end, etc */

#if defined (FEATURE_SAVE_DEBUG_TRACE)
#include "diag_f3_tracei.h"
#endif /* FEATURE_SAVE_DEBUG_TRACE */

#include "diagtarget.h"
#if defined (DIAG_QDSP6_APPS_PROC)
#include "diagstub.h"
#else
#include "assert.h"
#endif

#include "diagi_v.h"
#include "diagdiag_v.h"
#include "msgi.h"
#include "diag_cfg.h"

#include "diag_stm.h"

#include <stdlib.h>

#ifdef FEATURE_DIAG_WCDMA_CFN_SFN_TIMESTAMP
#include "wcdmadiagif.h"
#endif

#include "micro_msg_diag_service.h"  /* For island mode check */
#include "micro_diagbuffer_msg_api_v.h" /* For micro_msg_v2_send** api's*/

  /* Time Get Function Pointer from diag.c */
  extern uint8 (*diag_time_get)(qword time);

  /* Number of bits in the frame number that is inserted into the timestamp,
     ** and the mask to extract these bits.
   */
#define MSG_FN_NUM_BITS 10
#define MSG_FN_MASK 0x3ff


#ifndef MSG_FMT_STR_ARG_SIZE
#define MSG_FMT_STR_ARG_SIZE 100 /* 100 is guess */
#endif

extern uint8 diag_cur_preset_id; /* from diag.c */
extern uint32 msg_dropped;  /* number of dropped messages */
extern uint32 msg_drop_delta;   /* number of dropped messages */

extern boolean msg_service_init;

static msg_qsr_store_type * qsr_msg_send_prep ( const msg_qsr_const_type * const_blk,
   unsigned int num_args, uint32 isHash, byte stream_id);

static msg_ext_store_type * msg_send_prep (const msg_const_type * const_blk, unsigned int num_args,
                    uint64 timestamp, boolean ts_valid, byte stream_id);


static uint32 qsr_msg_dropped;  /* number of dropped messages */
static uint32 qsr_msg_drop_delta;   /* number of dropped messages */

#ifdef FEATURE_DIAG_STM

/* STREAM_STM and STM enabled: Need to allocate and send over STM immediately */
/* If TRUE, packet is needed for STM and should be malloc'd from STM pool */
/* (This might be in addition to allocating from the diagbuf if needed for listeners or other streams) */


/* NOTE: For both MSG_STM and MSG_DIAGBUF, calling functions must pass in the value
   'flag' indicatign if diag STM is enabled or not. Each function should manage this
   value to ensure persistence through-out the function and avoid accessing the
   global variable diag_stm_enabled when possible. */

#define MSG_STM(s,flag) (((s & DIAG_STREAM_STM) && (flag)) ? TRUE : FALSE)


/* Used to compute packet size needed for QSR packet */
#define STM_QSR_EXT_MSG_PKT_SIZE(numargs) \
  sizeof(msg_hdr_type) + sizeof(msg_qsr_const_type) + (4*(numargs))

/* Used to compute packet size needed for non-QSR packet */
#define STM_EXT_MSG_PKT_SIZE(fmt_len,num_args) \
  sizeof(msg_hdr_type) + sizeof(msg_desc_type) + (fmt_len) + (4*(num_args))

#endif

/* STREAM_STM and STM not enabled, or stm_enabled with a stream other than STREAM_STM */
/* If TRUE, packet needs to be allocated in diagbuf */

#define MSG_DIAGBUF(s,flag) (((!(flag) && s) || ((flag) && (s & ~DIAG_STREAM_STM))) ? TRUE : FALSE)

extern uint8 msg_enabled_mask; /* Bit mask to check if F3 masks, listeners and Extended Listeners are set */

extern boolean diag_time_initialized;

#if defined (FEATURE_SAVE_DEBUG_TRACE)
#define MAX_VAR_ARGS 9

extern osal_mutex_arg_t         diag_debug_record_buf_cs;
static void msg_save_trace (const void *,
                            msg_arg_type *,
                            uint8,
                            void *,
                            uint32);
extern diag_f3_trace_handle core_f3_trace;

/*===========================================================================

FUNCTION MSG_SAVE_TRACE

DESCRIPTION
  Extracts arguments based on the constant_data_ptr type and saves it to the
  circular buffer.  If timestamp-saving option is checked, it will be saved
  to the circular buffer as well.

DEPENDENCIES
  None

RETURN VALUE
  Number of bytes copied to the buffer.

SIDE EFFECTS
  None

===========================================================================*/
static void
msg_save_trace
(
  const void * constant_data_ptr,
  msg_arg_type * args,
  uint8 nargs,
  void * msg,
  uint32 type
)
{
    /* Is DIAG services initialized? */
  if (!diag_services_initialized)
    return;

  /* Call F3 trace API with CORE F3 trace structure */
  err_f3_save_msg(&core_f3_trace,constant_data_ptr, args, nargs, msg, type);

  return;
}



/*===========================================================================

FUNCTION MSG_SAVE_3

DESCRIPTION
  Saves the F3 message to the RAM buffer.  Also called from the ERR_FATAL
  macro.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void msg_save_3(const msg_const_type* const_blk,
                uint32 xx_arg1, uint32 xx_arg2, uint32 xx_arg3,
                msg_ext_store_type* msg)
{
    msg_arg_type f3_args[3];

    f3_args[0] = xx_arg1;
    f3_args[1] = xx_arg2;
    f3_args[2] = xx_arg3;

    msg_save_trace((void*)const_blk, f3_args, 3, (void*)msg, DIAG_F3_TRACE_STANDARD);
    return;
}
#endif /* FEATURE_SAVE_DEBUG_TRACE */


uint8
msg_get_time(qword * ts)
{
  uint8 time_type = 0;
  /* Get timestamp for this message ASAP */
  if(diag_time_initialized)
  time_type = diag_time_get (*ts);

  {
    /* Get the frame number (either SFN or CFN, depending on the
    ** state of the mobile), extract its least significant 10 bits
    ** and add these bits to the timestamp.
    */
    uint32 frame_number = 0;

    #ifdef FEATURE_DIAG_WCDMA_CFN_SFN_TIMESTAMP
    frame_number = wl1_get_frame_number_for_diag() & MSG_FN_MASK;
    #endif

    (*ts)[0] >>= MSG_FN_NUM_BITS;
    (*ts)[0] <<= MSG_FN_NUM_BITS;
    (*ts)[0] |= frame_number;
  }


  return time_type;
}

typedef union
{
  unsigned long  ts[2];
  uint64 timestamp;
}msg_timestamp;

/*===========================================================================

FUNCTION MSG_STATUS

DESCRIPTION
  Returns whether the specific SSID and subsystem mask is enabled or not.

DEPENDENCIES
   None

===========================================================================*/

boolean msg_status(uint16 ss_id, uint32 ss_mask) {

  uint32 rt_mask = 0;
  boolean valid_ssid = FALSE;

    #ifdef DIAG_STREAM_2_MASK_CHECK
    uint32 rt_mask_ctrl = 0;
    boolean valid_ssid_ctrl = FALSE;
    #endif

    byte stream_id = 0;

    valid_ssid =
      msg_get_ssid_masks_adv (MSG_MASK_RT, ss_id,
              ss_id, &rt_mask, DIAG_STREAM_1, diag_cur_preset_id);
    if((valid_ssid && (ss_mask & rt_mask)))
      stream_id |= DIAG_STREAM_1;

  #ifdef DIAG_STREAM_2_MASK_CHECK
    valid_ssid_ctrl = msg_get_ssid_masks_adv (MSG_MASK_RT, ss_id,
              ss_id, &rt_mask_ctrl, DIAG_STREAM_2, DIAG_DCI_MASK_1);
    if((valid_ssid_ctrl && (ss_mask & rt_mask_ctrl)))
      stream_id |= DIAG_STREAM_2;
  #endif

    if (stream_id) {
            return TRUE;
    }
    else {
            return FALSE;
    }

}

/*===========================================================================

FUNCTION MSG_STREAM_CHECK

DESCRIPTION
  This helper function performs a check to see which streams a given message
  is enabled on. In cases where STM is enabled, a LISTENER stream bit is also
  set if listeners are enabled.

DEPENDENCIES

===========================================================================*/
static byte msg_stream_check(const msg_desc_type *const_desc)
{
  uint32 rt_mask = 0;
  boolean valid_ssid = FALSE;
  byte stream_id = 0;
  #ifdef DIAG_STREAM_2_MASK_CHECK
  uint32 rt_mask_ctrl = 0;
  boolean valid_ssid_ctrl = FALSE;
  #endif

  if(const_desc != NULL)
  {
    /* Check the runtime mask */
    valid_ssid =
      msg_get_ssid_masks_adv (MSG_MASK_RT, const_desc->ss_id,
          const_desc->ss_id, &rt_mask, DIAG_STREAM_1, diag_cur_preset_id);
    if((valid_ssid && (const_desc->ss_mask & rt_mask)))
      stream_id |= DIAG_STREAM_1;

    #ifdef DIAG_STREAM_2_MASK_CHECK
    valid_ssid_ctrl = msg_get_ssid_masks_adv (MSG_MASK_RT, const_desc->ss_id,
      const_desc->ss_id, &rt_mask_ctrl, DIAG_STREAM_2, diag_cur_preset_id);
    if((valid_ssid_ctrl && (const_desc->ss_mask & rt_mask_ctrl)))
      stream_id |= DIAG_STREAM_2;
    #endif
  }
  return stream_id;
}
/*===========================================================================

FUNCTION MSG_SEND_PREP

DESCRIPTION
  This message performs the common operations used by all messages.
  It allocates, fills in all data except arguments, and returns a pointer
  to the allocated message buffer.  It also handles message statisitics.

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
static msg_ext_store_type *
msg_send_prep (const msg_const_type * const_blk, unsigned int num_args,
               uint64 timestamp, boolean ts_valid, byte stream_id)
{
  msg_timestamp temp_timestamp;
  msg_ext_store_type *msg = NULL;
  unsigned int rsp_len =
    FPOS (msg_ext_store_type, args) + num_args * FSIZ (msg_ext_store_type,
                               args[0]);

  if (stream_id)
  {
    /*-----------------------------------------------------------------------
      Get a pointer to a buffer.  If it's a NULL pointer, there aren't any
      free buffers.  No effort is made to save higher priority messages in
      favor of lower priority messages, as message flooding hides enough
      details to make the usable margin on such a feature too small for
      implementation.
    -----------------------------------------------------------------------*/
    msg = (void *) diagbuf_alloc (DIAG_EXT_MSG_TERSE_F, rsp_len, 0, DIAGBUF_ALLOC_REQUEST_MSG, 0, DIAG_EXT_API_VERSION_0);

    if (msg)
    {
        diagbuf_write_stream_id (msg,stream_id);

        if(ts_valid) /* client timestamp is valid, copy that into the header */
          {
            temp_timestamp.timestamp = timestamp;
            msg->hdr.ts[0] =(unsigned long) temp_timestamp.ts[0];
            msg->hdr.ts[1] = (unsigned long)temp_timestamp.ts[1];
          }
          else
          {
              /*lint -save -e{545} Suspicious use of & */
              msg->hdr.ts_type = msg_get_time(&(msg->hdr.ts));
              /*lint -restore */
          }

      msg->hdr.num_args = (uint8)num_args;

      msg->hdr.drop_cnt = (uint8) ((msg_drop_delta > 255) ? 255 : msg_drop_delta);

      msg_drop_delta = 0;   /* Reset delta drop count */

      /*-----------------------------------------------------------------------
        Set the pointer to the constant blk, to be expanded in DIAG context.
      -----------------------------------------------------------------------*/
      msg->const_data_ptr = const_blk;
    }
    else
    {
      msg_drop_delta++;
      msg_dropped++;        /* count up one more ignored message */
    }
  }

  return msg;
 }               /* msg_send_prep() */

/*===========================================================================

FUNCTION MSG_SEND

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with no arguments.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
msg_send (const msg_const_type * const_blk)
{
  msg_ext_store_type *msg = NULL;
  const unsigned int num_args = 0;  /* # of message arguments */

  if(msg_enabled_mask != 0 && msg_service_init == TRUE)
  {
    byte stream_id = 0;

    stream_id = msg_stream_check(&const_blk->desc);
    msg = msg_send_prep (const_blk, num_args, 0, FALSE, stream_id);

    if (msg)
    {
       diagbuf_commit (msg);
    }
    }
#if defined (FEATURE_SAVE_DEBUG_TRACE)

    msg_save_trace((void *)const_blk, NULL, 0, (void *)msg, DIAG_F3_TRACE_STANDARD);

#endif /* FEATURE_SAVE_DEBUG_TRACE */
}               /* msg_send() */


/*===========================================================================

FUNCTION MSG_SEND_TS

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with timestamp as user supplied arguments.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
msg_send_ts (const msg_const_type * const_blk, uint64 timestamp)
{
  msg_ext_store_type *msg = NULL;
  const unsigned int num_args = 0;  /* # of message arguments */

  if(msg_enabled_mask != 0 && msg_service_init == TRUE)
  {
    byte stream_id = 0;

    stream_id = msg_stream_check(&const_blk->desc);
    msg = msg_send_prep (const_blk, num_args, timestamp, TRUE, stream_id);
    if (msg)
    {
        diagbuf_commit (msg);
    }
  }

#if defined (FEATURE_SAVE_DEBUG_TRACE)

  msg_save_trace((void *)const_blk, NULL, 0, (void *)msg, DIAG_F3_TRACE_STANDARD);

#endif /* FEATURE_SAVE_DEBUG_TRACE */
}               /* msg_send_ts() */

/*===========================================================================

FUNCTION MSG_SEND_1

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with 1 argument.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
msg_send_1 (const msg_const_type * const_blk, uint32 xx_arg1)
{
  msg_ext_store_type *msg = NULL;
  uint32 *args = NULL;
  const unsigned int num_args = 1;  /* # of message arguments */
#if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type    f3_args[1];
#endif /* FEATURE_SAVE_DEBUG_TRACE */

  if(msg_enabled_mask != 0 && msg_service_init == TRUE)
  {
    byte stream_id = 0;

    stream_id = msg_stream_check(&const_blk->desc);
    msg = msg_send_prep (const_blk, num_args, 0, FALSE, stream_id);

    if (msg)
    {
    /*-----------------------------------------------------------------------
         Store the arguments in the buffer.
    -----------------------------------------------------------------------*/
        args = msg->args;

        args[0] = xx_arg1;

        diagbuf_commit (msg);

    }
  }
#if defined (FEATURE_SAVE_DEBUG_TRACE)

    f3_args[0] = xx_arg1;
    msg_save_trace((const void *)const_blk, f3_args, 1, (void *)msg, DIAG_F3_TRACE_STANDARD);

#endif /* FEATURE_SAVE_DEBUG_TRACE */
}               /* void msg_send_1() */

/*===========================================================================

FUNCTION MSG_SEND_2

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with 2 arguments.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
msg_send_2 (const msg_const_type * const_blk, uint32 xx_arg1, uint32 xx_arg2)
{
  msg_ext_store_type *msg = NULL;
  uint32 *args = NULL;
  const unsigned int num_args = 2;  /* # of message arguments */
#if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type    f3_args[2];
#endif /* FEATURE_SAVE_DEBUG_TRACE */

  if(msg_enabled_mask != 0 && msg_service_init == TRUE)
  {
    byte stream_id = 0;

    stream_id = msg_stream_check(&const_blk->desc);
    msg = msg_send_prep (const_blk, num_args, 0, FALSE, stream_id);

    if (msg)
    {
        /*-----------------------------------------------------------------------
              Store the arguments in the buffer.
         -----------------------------------------------------------------------*/
        args = MSG_LARGE_ARGS(msg->args);

        args[0] = xx_arg1;
        args[1] = xx_arg2;

        diagbuf_commit (msg);
    }
  }
#if defined (FEATURE_SAVE_DEBUG_TRACE)

    f3_args[0] = xx_arg1;
    f3_args[1] = xx_arg2;
    msg_save_trace((const void *)const_blk, f3_args, 2, (void *)msg, DIAG_F3_TRACE_STANDARD);

#endif /* FEATURE_SAVE_DEBUG_TRACE */
}               /* msg_send_2() */

/*===========================================================================

FUNCTION MSG_SEND_3

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with 3 arguments.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
msg_send_3 (const msg_const_type * const_blk, uint32 xx_arg1,
        uint32 xx_arg2, uint32 xx_arg3)
{
  msg_ext_store_type *msg = NULL;
  uint32 *args = NULL;
  const unsigned int num_args = 3;  /* # of message arguments */

  if(msg_enabled_mask != 0 && msg_service_init == TRUE)
  {
    byte stream_id = 0;

    stream_id = msg_stream_check(&const_blk->desc);
    msg = msg_send_prep (const_blk, num_args, 0, FALSE, stream_id);

    if (msg)
    {
            /*-----------------------------------------------------------------------
                Store the arguments in the buffer.
            -----------------------------------------------------------------------*/
            args = MSG_LARGE_ARGS(msg->args);

            args[0] = xx_arg1;
            args[1] = xx_arg2;
            args[2] = xx_arg3;

            diagbuf_commit (msg);
    }
 }
#if defined (FEATURE_SAVE_DEBUG_TRACE)

    msg_save_3(const_blk, xx_arg1, xx_arg2, xx_arg3, msg);

#endif /* FEATURE_SAVE_DEBUG_TRACE */
}    /* msg_send_3() */



/*===========================================================================

FUNCTION MSG_SEND_VAR

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with a variable number of
    arguments, specified by 'num_args'.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
msg_send_var (const msg_const_type * const_blk, uint32 num_args, ...)
{
  msg_ext_store_type *msg = NULL;
  uint32 *args = NULL;
  va_list arg_list;     /* ptr to the variable argument list */
  unsigned int i;
#if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type    f3_args[9];  // Assuming at most 9 arguments
#endif /* FEATURE_SAVE_DEBUG_TRACE */

  if(msg_enabled_mask != 0 && msg_service_init == TRUE)
  {
    byte stream_id = 0;

    stream_id = msg_stream_check(&const_blk->desc);
    msg = msg_send_prep (const_blk, num_args, 0, FALSE, stream_id);

    if (msg)
    {
            /*-----------------------------------------------------------------------
                Store the arguments in the buffer.
            -----------------------------------------------------------------------*/
            args = MSG_LARGE_ARGS(msg->args);

    /*lint -save -e{64,10,78,718,746,628,732,530} */
            /* Initialize variable arguments */
            va_start (arg_list, num_args);
    /*lint -restore */

            /* Store arguments from variable list. */
    /*lint -save -e{64,10,78,718,746,628,732,530} */
            for (i = 0; i < num_args; i++)
            {
                args[i] = va_arg (arg_list, uint32);
            }               /* for */
    /*lint -restore */
            /* Reset variable arguments */
            va_end (arg_list);

            diagbuf_commit (msg);
    }
  }
#if defined (FEATURE_SAVE_DEBUG_TRACE)

/*lint -save -e{64,10,718,746,628} */
    /* Initialize variable arguments */
    va_start (arg_list, num_args);

    /* Store arguments from variable list. */
    for (i = 0; i < MIN(num_args,MAX_VAR_ARGS); i++)
    {
      f3_args[i] = va_arg (arg_list, uint32);
    }               /* for */

    /* Reset variable arguments */
    va_end (arg_list);
/*lint -restore */
    msg_save_trace((const void *)const_blk, f3_args, MIN(num_args,MAX_VAR_ARGS), (void *)msg, DIAG_F3_TRACE_STANDARD);

#endif /* FEATURE_SAVE_DEBUG_TRACE */
}               /* msg_send_var() */



#ifdef FEATURE_DIAG_STM
static uint32 stm_msg_drop_delta = 0;
static uint32 stm_msg_dropped = 0;

/*===========================================================================

FUNCTION DIAG_STM_MSG_HDR_PREP

DESCRIPTION
  This helper functions fills out commonly used fields in the msg header
  for a message packet being sent out over STM.

DEPENDENCIES

===========================================================================*/
static void diag_stm_msg_hdr_prep(msg_hdr_type *msg_hdr, unsigned int num_args)
{
  /* This is common to all F3s being sent over STM */
  msg_hdr->num_args = num_args;
  msg_hdr->ts_type = msg_get_time(&(msg_hdr->ts));

  msg_hdr->drop_cnt = (uint8) ((stm_msg_drop_delta > 255) ? 255 : stm_msg_drop_delta);
  stm_msg_drop_delta = 0;   /* Reset delta drop count */
}

/*===========================================================================

FUNCTION DIAG_STM_QSR_MSG_PREP

DESCRIPTION
  This functions allocates a diag STM message packet and populates all packet
  fields, except for arguments. It is used for packets being sent over STM,
    which must be fully expanded in the context of the caller.

DEPENDENCIES

===========================================================================*/

void *diag_stm_qsr_msg_prep(const msg_qsr_const_type * const_blk, unsigned int num_args, uint32 isHash, uint32 *len)
{
  uint32 msg_size = 0;
  void * msg;

  /* Determine size of full packet */
  if (isHash)
  {
    /* Qhrink message (DIAG_QSR_EXT_MSG_TERSE_F) */
    /* msg_size = msg_hdr_type + qsr_const_type + args for Qshrink messages */
    msg_size = STM_QSR_EXT_MSG_PKT_SIZE(num_args);
  }
  else
  {
    /* Non Qshrink message (DIAG_EXT_MSG_F) */
    /* Determine string length (including NULL terminating character) */

    /* Notes on msg/filename fmt
       current == msg_hash -> "filename:string"
       Can determine length needed based on the string length of msg_hash. */

    msg_size = STM_EXT_MSG_PKT_SIZE(strlen((const char*)const_blk->msg_hash)+1,num_args);
  }

  /* Allocate packet for STM */
  msg = diag_stm_msg_alloc(msg_size);

  /* Populate common fields in buffer */
  if (msg)
  {
    msg_hdr_type *msg_hdr = (msg_hdr_type *)msg;
    char *copy_ptr = NULL;
	uint32 copy_buf_size = msg_size;

    /*  Set packet length to size allocated */
    *len = msg_size;

    /* Set msg_header fields */
    diag_stm_msg_hdr_prep(msg_hdr, num_args);

    /* Copy descriptor fields */
    copy_ptr = (char*)msg + sizeof(msg_hdr_type);
	copy_buf_size -= sizeof(msg_hdr_type);
    memscpy(copy_ptr, copy_buf_size,
            (void*)(&const_blk->desc), sizeof(msg_desc_type));
    copy_ptr += sizeof(msg_desc_type);
	copy_buf_size -= sizeof(msg_desc_type);

    if (isHash)
    {
      /* QSR message DIAG_QSR_EXT_MSG_TERSE_F */
      msg_hdr->cmd_code = DIAG_QSR_EXT_MSG_TERSE_F;

      /* Copy hash value */
      memscpy(copy_ptr,copy_buf_size,(void*)(&const_blk->msg_hash),sizeof(uint32));
    }
    else
    {
      /* Non-QSR message (no hash) DIAG_EXT_MSG_F */
      char *fmt_ptr;
      int filename_len, str_len;

      msg_hdr->cmd_code = DIAG_EXT_MSG_F;

      /* Need to determine length of filename (location where fmt string starts) */
      for (fmt_ptr = (char *) const_blk->msg_hash; *fmt_ptr != ':'; fmt_ptr++);
      filename_len = fmt_ptr - (char *)const_blk->msg_hash;

      /* Copy format string */

      /* Increment fmt_ptr past ':' character */
      fmt_ptr++;
      /*  Reserve space for arguments */
      copy_ptr += (num_args*4);
	  copy_buf_size -= (num_args*4);
      str_len = MIN(MSG_MAX_STRLEN, (int)strlen(fmt_ptr));

      memscpy(copy_ptr,copy_buf_size,(void*)fmt_ptr, str_len); 
      copy_ptr += str_len;

      /* NULL terminate format string */
      *copy_ptr++ = '\0';
	  
	  copy_buf_size -= (str_len + 1);

      /* Copy file name */
      filename_len = MIN(MSG_MAX_STRLEN,filename_len);
      memscpy(copy_ptr,copy_buf_size,(void*)(const_blk->msg_hash),filename_len);
      copy_ptr += filename_len;

      /* NULL terminate filename */
      *copy_ptr++ = '\0';
    }
  }
  else
  {
    /* Unable to allocate memory */

    /* Set packet length to zero */
    *len = 0;

    stm_msg_drop_delta++;
    if(stm_msg_dropped < MAX_VALUE_UINT32)
    {
      stm_msg_dropped++;
    }
  }
  return msg;
}

#endif /* FEATURE_DIAG_STM */

/*===========================================================================

FUNCTION MSG_SPRINTF

DESCRIPTION

  This will queue a debug message in Extended Message Format.  The message
  will be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This will build a message sprintf diagnostic Message with var #
  of parameters.

  Do not call directly; use macro MSG_SPRINTF_* defined in msg.h

  Send a message through diag output services.

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void msg_sprintf (const msg_const_type * const_blk, ...)
{
  /* Declared union for future use */
  typedef union
  {
    msg_ext_type ext;
  } msg_sprintf_desc_type;

  msg_sprintf_desc_type *msg = NULL;

  char *str = NULL;   /* Used to copy the file name and fmt string to the msg */
  va_list arg_list;   /* ptr to the variable argument list */

  unsigned int int_cnt = 0;           /* Calculate the # args, to allocate buffer */
  unsigned int fname_length = 0;      /* Stores the file name along with '\0'     */
  unsigned int fmt_length = 0;        /* Stores the fmt length,'\0' and arg size  */
  unsigned int total_allocated = 0;   /* Total buffer allocated                   */
  unsigned int fmt_len_available = 0; /* Remaining buffer for format string       */
  boolean diagbuf_flag = TRUE;        /* Flag indicating that buffer came from diagbuf (and not STM) */
  uint32 pkt_length = 0;        /* Final packet length */
  boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */

  fname_length = strlen (const_blk->fname) + 1;

  /* Copy the length of the fmt string along with MSG_LENGTH_OF_ARGS */
  fmt_length = strlen (const_blk->fmt) + 1 + MSG_FMT_STR_ARG_SIZE;

  /* Calculate # of arguments to ensure enough space is allocated. */
  int_cnt = sizeof (msg_desc_type) - FSIZ (msg_ext_store_type, const_data_ptr) +
            fmt_length + fname_length;

  /* Calculates number of uint32 required    */
  int_cnt = (int_cnt + sizeof (uint32) - 1) / sizeof (uint32);

  if(msg_enabled_mask != 0 && msg_service_init == TRUE)
  {
      byte stream_id = 0;

      stream_id = msg_stream_check(&const_blk->desc);
    /*  Allocates the buffer required, fills in the header  */

	  if MSG_DIAGBUF(stream_id, stm_enabled)
      {
          /* Need to allocate from diagbuf */
          msg = (msg_sprintf_desc_type *) msg_send_prep (const_blk,  int_cnt, 0, FALSE, stream_id);
      }
#ifdef FEATURE_DIAG_STM
      else if MSG_STM(stream_id, stm_enabled )
      {
          /* Allocate space for STM buffer */
          msg = (msg_sprintf_desc_type*)diag_stm_msg_alloc(STM_EXT_MSG_PKT_SIZE(fmt_length,int_cnt));

          if(msg)
          {
              msg_hdr_type *msg_hdr = (msg_hdr_type *)msg;
              diag_stm_msg_hdr_prep(msg_hdr, int_cnt);
          }

          //msg = (msg_sprintf_desc_type*)diag_stm_qsr_msg_prep((msg_qsr_const_type*)const_blk, int_cnt, FALSE, &pkt_length);
          diagbuf_flag = FALSE;
      }
#endif /* FEATURE_DIAG_STM */

    if (msg)
    {
            /* Queue a debug message in Extended Message Format. */
            msg->ext.hdr.cmd_code = DIAG_EXT_MSG_F;

            /* This function embedds the argument in the string itself.
            ** Hence the num_args is assigned 0 */
            msg->ext.hdr.num_args = 0;

            /* Copy 'desc'. */
            msg->ext.desc = const_blk->desc;

            /* Copy the format string where the argument list would start.
                 Since there are no arguments, the format string starts in the 'args'
                 field. */
            str = (char *) msg->ext.args;

            /* Calculate the buffer allocated */
            total_allocated = FPOS (msg_ext_store_type, args) + int_cnt *
                                                        FSIZ (msg_ext_store_type, args[0]);

            /* Calculate the buffer left to copy the format string */
            fmt_len_available = total_allocated - (FPOS (msg_ext_type, args) + fname_length);

            if( fmt_len_available < fmt_length)
            {
                 fmt_length = fmt_len_available;
            }

            /* Initialize variable argument list */
    /*lint -save -e{64,718,746,516} */
            va_start(arg_list, const_blk);
    /*lint -restore */

            /* Copy the format string with arguments */
    #if defined __GNUC__
            (void) vsnprintf (str, fmt_length, const_blk->fmt, arg_list);
             str[fmt_length-1] = '\0';
    #else
            (void) _vsnprintf (str, fmt_length, const_blk->fmt, arg_list);
            str[fmt_length-1] = '\0';
    #endif

        /* Reset variable arguments */
            va_end(arg_list);

            /* Move the str pass the format string, strlen excludes the terminal
            ** NULL hence 1 is added to include NULL. */
            str += strlen((const char *)str) + 1;

        /* Copy the filename and strlcpy adds the ending NULL terminal */
      /*Since the str dest size is computed before, there would not be any buffer overflow */
      (void)strlcpy(str, (const char *)const_blk->fname, fname_length );

      /* Move the str pass the filename, strlen excludes the terminal NULL
      ** hence 1 is added to include NULL. */
      str += strlen((const char *)str) + 1;

      /* str is now pointing to the byte after the last valid byte. str - msg
             gives the total length required. */
          pkt_length = (uint32)(str - (char *)msg);

#ifdef FEATURE_DIAG_STM
          if MSG_STM(stream_id,stm_enabled)
          {
              /* Send data to STM */
              diag_stm_write_pkt(msg,pkt_length);
          }

          if (!diagbuf_flag)
          {
              /* Data not allocated from diagbuf, so free data */
              diag_stm_msg_free(msg);

              /* clear stream_stm bit to avoid resending */
              stream_id &= ~DIAG_STREAM_STM;
          }
           else
          {
             diagbuf_write_stream_id (msg,stream_id);
          }
#endif /* FEATURE_DIAG_STM */

          if (diagbuf_flag)
         {
              /* Use diagbuf_shorten to release the buffer */
              diagbuf_shorten (msg, pkt_length);
			  diagbuf_commit (msg);
		  }
	    }
	}

  return;
}  /* msg_sprintf */



/*===========================================================================

FUNCTION qsr_msg_send_prep

DESCRIPTION
  This message performs the common operations used by all messages.
  It allocates, fills in all data except arguments, and returns a pointer
  to the allocated message buffer.  It also handles message statisitics.

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
static msg_qsr_store_type *
qsr_msg_send_prep (const msg_qsr_const_type * const_blk,unsigned int num_args, uint32 isHash, byte stream_id)
{
  
  msg_qsr_store_type *msg = NULL;
  unsigned int rsp_len = FPOS (msg_qsr_store_type, args) + num_args * FSIZ (msg_qsr_store_type,
                               args[0]);

  if (stream_id)
  {
    /*-----------------------------------------------------------------------
      Get a pointer to a buffer.  If it's a NULL pointer, there aren't any
      free buffers.  No effort is made to save higher priority messages in
      favor of lower priority messages, as message flooding hides enough
      details to make the usable margin on such a feature too small for
      implementation.
    -----------------------------------------------------------------------*/
    msg = (void *) diagbuf_alloc (DIAG_QSR_EXT_MSG_TERSE_F, rsp_len, 0, DIAGBUF_ALLOC_REQUEST_MSG, 0, DIAG_EXT_API_VERSION_0);

    if (msg)
    {
      diagbuf_write_stream_id (msg,stream_id);
      
/*lint -save -e{545} Suspicious use of & */
      msg->hdr.ts_type = msg_get_time(&(msg->hdr.ts));
/*lint -restore */


      msg->hdr.num_args = (uint8)num_args;

      msg->hdr.drop_cnt = (uint8) ((qsr_msg_drop_delta > 255) ? 255 : qsr_msg_drop_delta);

      qsr_msg_drop_delta = 0;   /* Reset delta drop count */

      /*-----------------------------------------------------------------------
        Set the pointer to the constant blk, to be expanded in DIAG context.
      -----------------------------------------------------------------------*/
      msg->qsr_const_data_ptr = const_blk;
      msg->qsr_flag = QSR_BITFLAG_IS_HASH & isHash;
    }
    else
    {
      qsr_msg_drop_delta++;
      qsr_msg_dropped++;        /* count up one more ignored message */
    }
  }

  return msg;
}               /* qsr_msg_send_prep() */

/*===========================================================================

FUNCTION QSR_INT_MSG_SEND

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with no arguments.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
qsr_int_msg_send (const msg_qsr_const_type * const_blk, uint32 isHash)
{
    msg_qsr_store_type *msg = NULL;
    const unsigned int num_args = 0;  /* # of message arguments */
	boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */

  if(msg_enabled_mask != 0 && msg_service_init == TRUE)
    {
        byte stream_id = 0;

        stream_id = msg_stream_check(&const_blk->desc);
#ifdef FEATURE_DIAG_STM
        if MSG_STM(stream_id, stm_enabled)
        {
            /* Need to handle as an STM message by creating full packet and transmitting immediately */
            uint32 pkt_length;
            void *stm_msg;

            /* Allocate memory for msg and populate non argument fields */
            stm_msg = diag_stm_qsr_msg_prep(const_blk, num_args, isHash, &pkt_length);

            if (stm_msg)
            {
                /* Send data to STM */
                diag_stm_write_pkt(stm_msg,pkt_length);

                /* Free data */
                diag_stm_msg_free(stm_msg);
				
				 /* Clear DIAG_STREAM_STM bit to avoid sending the packet twice */
				stream_id &= ~DIAG_STREAM_STM;
            }
        }
#endif /* FEATURE_DIAG_STM */
		if MSG_DIAGBUF(stream_id, stm_enabled)
        {
            /* Need to allocate from diagbuf and commit to buffer */
            msg = qsr_msg_send_prep (const_blk, num_args, isHash, stream_id);

            if (msg)
            {
                diagbuf_commit (msg);
            }
        }
    }
#if defined (FEATURE_SAVE_DEBUG_TRACE)

        if (isHash)
        {
            msg_save_trace((const void *)const_blk, NULL, 0, (void *)msg, DIAG_F3_TRACE_QSHRINK);
        }
        else
        {
            msg_save_trace((const void *)const_blk, NULL, 0, (void *)msg, DIAG_F3_TRACE_QSHRINK20);
        }
#endif /* FEATURE_SAVE_DEBUG_TRACE */

} /* qsr_int_msg_send */

/*===========================================================================

FUNCTION QSR_INT_MSG_SEND_1

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with 1 argument.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
qsr_int_msg_send_1 ( const msg_qsr_const_type * const_blk, uint32 isHash, uint32 xx_arg1)
{
  msg_qsr_store_type *msg = NULL;
  uint32 *args = NULL;
  const unsigned int num_args = 1;  /* # of message arguments */
#if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type    f3_args[1];
#endif /* FEATURE_SAVE_DEBUG_TRACE */
  boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */

  if(msg_enabled_mask != 0 && msg_service_init == TRUE)
    {
        byte stream_id = 0;

        stream_id = msg_stream_check(&const_blk->desc);

#ifdef FEATURE_DIAG_STM
        if MSG_STM(stream_id, stm_enabled)
        {
            uint32 pkt_length;
            void *stm_msg;

            /* Allocate memory for msg and populate non argument fields */
            stm_msg = diag_stm_qsr_msg_prep(const_blk, num_args, isHash, &pkt_length);

            if (stm_msg)
            {
                /* set up args in struct */
                {
                    char* arg_ptr = (char *)stm_msg;
                    msg_hdr_type *msg_hdr = (msg_hdr_type*)stm_msg;
                    arg_ptr += sizeof(msg_hdr_type) + sizeof(msg_desc_type);
                    if (msg_hdr->cmd_code == DIAG_QSR_EXT_MSG_TERSE_F)
                    {
                        /* Additional offset for QSR messages */
                        arg_ptr += sizeof(uint32);
                    }
                    *(uint32*)arg_ptr = xx_arg1;
                }

                /* Send data to STM */
                diag_stm_write_pkt(stm_msg,pkt_length);

                /* Free data */
                diag_stm_msg_free(stm_msg);
				
				/* Clear DIAG_STREAM_STM bit to avoid sending the packet twice */
				stream_id &= ~DIAG_STREAM_STM;
            }
        }
#endif /* FEATURE_DIAG_STM */

       if MSG_DIAGBUF(stream_id, stm_enabled)
       {
            /* Need to allocate from diagbuf and commit to buffer */
            msg = qsr_msg_send_prep (const_blk, num_args, isHash, stream_id);

            if (msg)
            {
                /*-----------------------------------------------------------------------
                    Store the arguments in the buffer.
                 -----------------------------------------------------------------------*/
                    args = msg->args;

                    args[0] = xx_arg1;

                    diagbuf_commit (msg);
            }
       }
  }
#if defined (FEATURE_SAVE_DEBUG_TRACE)

        f3_args[0] = xx_arg1;

    if (isHash)
    {
        msg_save_trace((const void *)const_blk, f3_args, 1, (void *)msg, DIAG_F3_TRACE_QSHRINK);
    }
    else
    {
      msg_save_trace((const void *)const_blk, f3_args, 1, (void *)msg, DIAG_F3_TRACE_QSHRINK20);
    }

#endif /* FEATURE_SAVE_DEBUG_TRACE */
} /* qsr_int_msg_send_1 */

/*===========================================================================

FUNCTION QSR_INT_MSG_SEND_2

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with 2 arguments.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
qsr_int_msg_send_2 ( const msg_qsr_const_type * const_blk, uint32 isHash, uint32 xx_arg1, uint32 xx_arg2)
{
  msg_qsr_store_type *msg = NULL;
  uint32 *args = NULL;
  const unsigned int num_args = 2;  /* # of message arguments */
#if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type    f3_args[2];
#endif /* FEATURE_SAVE_DEBUG_TRACE */
  boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */

  if(msg_enabled_mask != 0 && msg_service_init == TRUE)
    {
      byte stream_id = 0;

      stream_id = msg_stream_check(&const_blk->desc);

#ifdef FEATURE_DIAG_STM
      if MSG_STM(stream_id, stm_enabled)
      {
          uint32 pkt_length;
          void *stm_msg;

          /* Allocate memory for msg and populate non argument fields */
          stm_msg = diag_stm_qsr_msg_prep(const_blk, num_args, isHash, &pkt_length);
          if (stm_msg)
          {
              /* set up args in struct */
              {
                  char* arg_ptr = (char *)stm_msg;
                  msg_hdr_type *msg_hdr = (msg_hdr_type*)stm_msg;
                  arg_ptr += sizeof(msg_hdr_type) + sizeof(msg_desc_type);
                  if (msg_hdr->cmd_code == DIAG_QSR_EXT_MSG_TERSE_F)
                  {
                      /* Additional offset for QSR messages */
                      arg_ptr += sizeof(uint32);
                  }
                  *(uint32*)arg_ptr = xx_arg1;
                  arg_ptr += sizeof(uint32);
                  *(uint32*)arg_ptr = xx_arg2;
              }

              /* Send data to STM */
              diag_stm_write_pkt(stm_msg,pkt_length);

              /* Free data */
              diag_stm_msg_free(stm_msg);
			  
			  /* Clear DIAG_STREAM_STM bit to avoid sending the packet twice */
			  stream_id &= ~DIAG_STREAM_STM;
          }
      }
#endif /* FEATURE_DIAG_STM */

      if MSG_DIAGBUF(stream_id, stm_enabled)
      {
          /* Need to allocate from diagbuf and commit to buffer */
          msg = qsr_msg_send_prep (const_blk, num_args, isHash, stream_id);

          if (msg)
          {
              /*-----------------------------------------------------------------------
                 Store the arguments in the buffer.
                -----------------------------------------------------------------------*/
              args = MSG_LARGE_ARGS(msg->args);
              args[0] = xx_arg1;
              args[1] = xx_arg2;
              diagbuf_commit (msg);
          }
      }
    }

#if defined (FEATURE_SAVE_DEBUG_TRACE)

    f3_args[0] = xx_arg1;
    f3_args[1] = xx_arg2;

    if (isHash)
    {
    msg_save_trace((const void *)const_blk, f3_args, 2, (void *)msg, DIAG_F3_TRACE_QSHRINK);
    }
    else
    {
      msg_save_trace((const void *)const_blk, f3_args, 2, (void *)msg, DIAG_F3_TRACE_QSHRINK20);
    }

#endif /* FEATURE_SAVE_DEBUG_TRACE */
} /* qsr_int_msg_send_2 */

/*===========================================================================

FUNCTION QSR_INT_MSG_SEND_3

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with 3 arguments.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
qsr_int_msg_send_3 ( const msg_qsr_const_type * const_blk, uint32 isHash, uint32 xx_arg1,
        uint32 xx_arg2, uint32 xx_arg3)
{
  msg_qsr_store_type *msg = NULL;
  uint32 *args = NULL;
  const unsigned int num_args = 3;  /* # of message arguments */
#if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type    f3_args[3];
#endif /* FEATURE_SAVE_DEBUG_TRACE */
  boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */

  if(msg_enabled_mask != 0 && msg_service_init == TRUE)
    {
      byte stream_id = 0;

      stream_id = msg_stream_check(&const_blk->desc);

#ifdef FEATURE_DIAG_STM
      if MSG_STM(stream_id, stm_enabled)
      {
          uint32 pkt_length;
          void *stm_msg;

          /* Allocate memory for msg and populate non argument fields */
          stm_msg = diag_stm_qsr_msg_prep(const_blk, num_args, isHash, &pkt_length);

          if (stm_msg)
          {
              /* set up args in struct */
              {
                  char* arg_ptr = (char *)stm_msg;
                  msg_hdr_type *msg_hdr = (msg_hdr_type*)stm_msg;
                  arg_ptr += sizeof(msg_hdr_type) + sizeof(msg_desc_type);
                  if (msg_hdr->cmd_code == DIAG_QSR_EXT_MSG_TERSE_F)
                  {
                      /* Additional offset for QSR messages */
                      arg_ptr += sizeof(uint32);
                  }
                  *(uint32*)arg_ptr = xx_arg1;
                  arg_ptr += sizeof(uint32);
                  *(uint32*)arg_ptr = xx_arg2;
                  arg_ptr += sizeof(uint32);
                  *(uint32*)arg_ptr = xx_arg3;
              }

              /* Send data to STM */
              diag_stm_write_pkt(stm_msg,pkt_length);

              /* Free data */
              diag_stm_msg_free(stm_msg);
			  
			  /* Clear DIAG_STREAM_STM bit to avoid sending the packet twice */
			  stream_id &= ~DIAG_STREAM_STM;
          }
      }
#endif /* FEATURE_DIAG_STM */
      if MSG_DIAGBUF(stream_id, stm_enabled)
      {
          /* Need to allocate from diagbuf and commit to buffer */
          msg = qsr_msg_send_prep (const_blk, num_args, isHash, stream_id);

          if (msg)
          {
              /*-----------------------------------------------------------------------
                    Store the arguments in the buffer.
                 -----------------------------------------------------------------------*/
              args = MSG_LARGE_ARGS(msg->args);
              args[0] = xx_arg1;
              args[1] = xx_arg2;
              args[2] = xx_arg3;
              diagbuf_commit (msg);
          }
      }
   }

#if defined (FEATURE_SAVE_DEBUG_TRACE)

    f3_args[0] = xx_arg1;
    f3_args[1] = xx_arg2;
    f3_args[2] = xx_arg3;

    if (isHash)
    {
    msg_save_trace((const void *)const_blk, f3_args, 3, (void *)msg, DIAG_F3_TRACE_QSHRINK);
    }
    else
    {
      msg_save_trace((const void *)const_blk, f3_args, 3, (void *)msg, DIAG_F3_TRACE_QSHRINK20);
    }
	
  

#endif /* FEATURE_SAVE_DEBUG_TRACE */
} /* qsr_int_msg_send_3 */

/*===========================================================================

FUNCTION QSR_MSG_SEND

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with no arguments.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void qsr_msg_send ( const msg_qsr_const_type * const_blk)
{
  qsr_int_msg_send(const_blk, TRUE);

} /* qsr_msg_send */

/*===========================================================================

FUNCTION QSR_MSG_SEND_1

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with 1 argument.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/

void qsr_msg_send_1 ( const msg_qsr_const_type * const_blk,uint32 xx_arg1)
{
  qsr_int_msg_send_1(const_blk, TRUE, xx_arg1);

} /* qsr_msg_send_1 */

/*===========================================================================

FUNCTION QSR_MSG_SEND_2

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with 2 arguments.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/

void qsr_msg_send_2 ( const msg_qsr_const_type * const_blk, uint32 xx_arg1, uint32 xx_arg2)
{
  qsr_int_msg_send_2(const_blk, TRUE, xx_arg1, xx_arg2);

} /* qsr_msg_send_2 */

/*===========================================================================

FUNCTION QSR_MSG_SEND_3

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with 3 arguments.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void qsr_msg_send_3 ( const msg_qsr_const_type * const_blk, uint32 xx_arg1,
        uint32 xx_arg2, uint32 xx_arg3)
{
  qsr_int_msg_send_3(const_blk, TRUE, xx_arg1, xx_arg2, xx_arg3);

} /* qsr_msg_send_3 */


/*===========================================================================

FUNCTION QSR_MSG_SEND_VAR

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with a variable number of
    arguments, specified by 'num_args'.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
qsr_msg_send_var (const msg_qsr_const_type * const_blk,uint32 num_args, ...)
{
  msg_qsr_store_type *msg = NULL;
  uint32 *args = NULL;
  va_list arg_list;     /* ptr to the variable argument list */
  unsigned int i;
#if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type    f3_args[MAX_VAR_ARGS];  // Assuming at most MAX_VAR_ARGS arguments
#endif /* FEATURE_SAVE_DEBUG_TRACE */
  boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */

  if(msg_enabled_mask != 0 && msg_service_init == TRUE)
  {

    byte stream_id = 0;

      stream_id = msg_stream_check(&const_blk->desc);
#ifdef FEATURE_DIAG_STM
      if MSG_STM(stream_id, stm_enabled)
      {
          uint32 pkt_length;
          void *stm_msg;

          /* Allocate memory for msg and populate non argument fields */
          stm_msg = diag_stm_qsr_msg_prep(const_blk, num_args,
                                          TRUE, &pkt_length);
          if (stm_msg)
          {
              /* set up args in struct */
              char* arg_ptr = (char *)stm_msg;
              msg_hdr_type *msg_hdr = (msg_hdr_type*)stm_msg;
              arg_ptr += sizeof(msg_hdr_type) + sizeof(msg_desc_type);
              if (msg_hdr->cmd_code == DIAG_QSR_EXT_MSG_TERSE_F)
              {
                  /* Additional offset for QSR messages */
                  arg_ptr += sizeof(uint32);
              }

              /*lint -save -e{64,10,718,746,628} */
              /* Initialize variable arguments */
              va_start (arg_list, num_args);

              /* Store arguments from variable list. */
              for (i = 0; i < MIN(num_args,MAX_VAR_ARGS); i++)
              {
                  *(uint32*)arg_ptr = va_arg (arg_list, uint32);
                  arg_ptr += sizeof(uint32);
              }               /* for */

              /* Reset variable arguments */
              va_end (arg_list);
              /*lint -restore */

              /* Send data to STM */
              diag_stm_write_pkt(stm_msg,pkt_length);

              /* Free data */
              diag_stm_msg_free(stm_msg);
			  
			  /* Clear DIAG_STREAM_STM bit to avoid sending the packet twice */
			  stream_id &= ~DIAG_STREAM_STM;
          }
      }
#endif /* FEATURE_DIAG_STM */

    if MSG_DIAGBUF(stream_id, stm_enabled)
    {
          /* Need to allocate from diagbuf and commit to buffer */
          msg = qsr_msg_send_prep (const_blk, num_args, TRUE, stream_id);

        if (msg)
        {
            /*-----------------------------------------------------------------------
                Store the arguments in the buffer.
            -----------------------------------------------------------------------*/
            args = MSG_LARGE_ARGS(msg->args);

    /*lint -save -e{64,10,78,718,746,628,732,530} */
            /* Initialize variable arguments */
            va_start (arg_list, num_args);
    /*lint -restore */

            /* Store arguments from variable list. */
    /*lint -save -e{64,10,78,718,746,628,732,530} */
            for (i = 0; i < num_args; i++)
            {
                args[i] = va_arg (arg_list, uint32);
            }               /* for */
    /*lint -restore */
            /* Reset variable arguments */
            va_end (arg_list);

            diagbuf_commit (msg);
        }
      }
   }
#if defined (FEATURE_SAVE_DEBUG_TRACE)

/*lint -save -e{64,10,718,746,628} */
    /* Initialize variable arguments */
    va_start (arg_list, num_args);

    /* Store arguments from variable list. */
    for (i = 0; i < MIN(num_args,MAX_VAR_ARGS); i++)
    {
      f3_args[i] = va_arg (arg_list, uint32);
    }               /* for */

    /* Reset variable arguments */
    va_end (arg_list);
/*lint -restore */
    msg_save_trace((const void *)const_blk, f3_args, MIN(num_args,MAX_VAR_ARGS), (void *)msg, DIAG_F3_TRACE_QSHRINK);

#endif /* FEATURE_SAVE_DEBUG_TRACE */
}               /* qsr_msg_send_var() */


/*===========================================================================

FUNCTION QSR_INT_MSG_SEND_VAR

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with variable arguments.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void qsr_int_msg_send_var (const msg_qsr_const_type *const_blk, uint32 isHash, uint32 num_args, ...)
{
	  msg_qsr_store_type *msg = NULL;
	  uint32 *args = NULL;
	  va_list arg_list;     /* ptr to the variable argument list */
	  unsigned int i;
	  msg_qsr_const_type *qsr_const_blk = (msg_qsr_const_type *)const_blk;
	  boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */

#if defined (FEATURE_SAVE_DEBUG_TRACE)
	  msg_arg_type    f3_args[MAX_VAR_ARGS];  // Assuming at most MAX_VAR_ARGS arguments
#endif

	  if(msg_enabled_mask != 0 && msg_service_init == TRUE)
	  {
	      byte stream_id = 0;

	      stream_id = msg_stream_check(&const_blk->desc);

#ifdef FEATURE_DIAG_STM
	      if MSG_STM(stream_id, stm_enabled)
	      {
	          uint32 pkt_length;
	          void *stm_msg;

	          /* Allocate memory for msg and populate non argument fields */
	          stm_msg = diag_stm_qsr_msg_prep(qsr_const_blk, num_args, isHash, &pkt_length);
	          if (stm_msg)
	          {
	              /* set up args in struct */
	              char* arg_ptr = (char *)stm_msg;
	              msg_hdr_type *msg_hdr = (msg_hdr_type*)stm_msg;
	              arg_ptr += sizeof(msg_hdr_type) + sizeof(msg_desc_type);
	              if (msg_hdr->cmd_code == DIAG_QSR_EXT_MSG_TERSE_F)
	              {
	                  /* Additional offset for QSR messages */
	                  arg_ptr += sizeof(uint32);
	              }

	              /*lint -save -e{64,10,718,746,628} */
	              /* Initialize variable arguments */
	              va_start (arg_list, num_args);

	              /* Store arguments from variable list. */
	              for (i = 0; i < MIN(num_args,MAX_VAR_ARGS); i++)
	              {
	                  *(uint32*)arg_ptr = va_arg (arg_list, uint32);
	                  arg_ptr += sizeof(uint32);
	              }               /* for */

	              /* Reset variable arguments */
	              va_end (arg_list);
	              /*lint -restore */

	              /* Send data to STM */
	              diag_stm_write_pkt(stm_msg,pkt_length);

	              /* Free data */
	              diag_stm_msg_free(stm_msg);
				  
				  /* Clear DIAG_STREAM_STM bit to avoid sending the packet twice */
				  stream_id &= ~DIAG_STREAM_STM;
	          }
	      }
#endif /* FEATURE_DIAG_STM */
	      if MSG_DIAGBUF(stream_id, stm_enabled)
	      {
	          /* Need to allocate from diagbuf and commit to buffer */
	          msg = qsr_msg_send_prep (qsr_const_blk, num_args, isHash, stream_id);

	          if (msg)
	          {
	              /*-----------------------------------------------------------------------
	                    Store the arguments in the buffer.
	                 -----------------------------------------------------------------------*/

	              args = MSG_LARGE_ARGS(msg->args);

	              /*lint -save -e{64,10,78,718,746,628,732,530} */
	              /* Initialize variable arguments */
	              va_start (arg_list, num_args);
	              /*lint -restore */

	              /* Store arguments from variable list. */
	              /*lint -save -e{64,10,78,718,746,628,732,530} */
	              for (i = 0; i < num_args; i++)
	              {
	                  args[i] = va_arg (arg_list, uint32);
	              }               /* for */
	              /*lint -restore */
	              /* Reset variable arguments */
	              va_end (arg_list);

	              diagbuf_commit (msg);
	          }
	      }
	  }

#if defined (FEATURE_SAVE_DEBUG_TRACE)

	/*lint -save -e{64,10,718,746,628} */
	  /* Initialize variable arguments */
	  va_start (arg_list, num_args);

	  /* Store arguments from variable list. */
	  for (i = 0; i < MIN(num_args,MAX_VAR_ARGS); i++)
	  {
	    f3_args[i] = va_arg (arg_list, uint32);
	  }               /* for */

	  /* Reset variable arguments */
	  va_end (arg_list);
	/*lint -restore */
	  msg_save_trace((const void *)const_blk, f3_args, MIN(num_args,MAX_VAR_ARGS), (void *)msg, DIAG_F3_TRACE_QSHRINK20);
#endif
  	
} /* qsr_int_msg_send_var */


/*===========================================================================

FUNCTION       msg_errlog_

DESCRIPTION  These functions send the desired f3 message in addition to
        invoking err_put_log. This reduces it one function call for top
        level macro.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void msg_errlog_3 (const msg_const_type* const_blk, uint32 code1, uint32 code2, uint32 code3)
{
 msg_send_3 (const_blk, (uint32)code1, (uint32)code2, (uint32)code3);
 err_put_log (const_blk->desc.line, const_blk->fname);
}

void msg_errlog_2 (const msg_const_type* const_blk, uint32 code1, uint32 code2)
{
 msg_send_2 (const_blk, (uint32)code1, (uint32)code2);
 err_put_log (const_blk->desc.line, const_blk->fname);
}

void msg_errlog_1 (const msg_const_type* const_blk, uint32 code1)
{
 msg_send_1 (const_blk, (uint32)code1);
 err_put_log (const_blk->desc.line, const_blk->fname);
}

void msg_errlog_0 (const msg_const_type* const_blk)
{
 msg_send (const_blk);
 err_put_log (const_blk->desc.line, const_blk->fname);
}

/*===========================================================================

FUNCTION       msg_qsrerrlog_

DESCRIPTION    These functions send the desired f3 message in addition to
        invoking err_put_log. This reduces it one function call for top
        level macro.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void msg_qsrerrlog_3 (const err_msg_qsr_const_type* const_blk, uint32 code1, uint32 code2, uint32 code3)
{
 qsr_msg_send_3 (&(const_blk->qsr_const_blk), (uint32)code1, (uint32)code2, (uint32)code3);
 err_put_log (const_blk->qsr_const_blk.desc.line, const_blk->fname);
}

void msg_qsrerrlog_2 (const err_msg_qsr_const_type* const_blk, uint32 code1, uint32 code2)
{
 qsr_msg_send_2 (&(const_blk->qsr_const_blk), (uint32)code1, (uint32)code2);
 err_put_log (const_blk->qsr_const_blk.desc.line, const_blk->fname);
}

void msg_qsrerrlog_1 (const err_msg_qsr_const_type* const_blk, uint32 code1)
{
 qsr_msg_send_1 (&(const_blk->qsr_const_blk), (uint32)code1);
 err_put_log (const_blk->qsr_const_blk.desc.line, const_blk->fname);
}

void msg_qsrerrlog_0 (const err_msg_qsr_const_type* const_blk)
{
 qsr_msg_send (&(const_blk->qsr_const_blk));
 err_put_log (const_blk->qsr_const_blk.desc.line, const_blk->fname);
}

void msg_v2_errlog_3 (const err_msg_v2_const_type* const_blk, uint32 code1, uint32 code2, uint32 code3)
{
 msg_v2_send_3 (&(const_blk->msg_v2_const_blk), (uint32)code1, (uint32)code2, (uint32)code3);
 err_put_log (const_blk->msg_v2_const_blk.desc.line, const_blk->fname);
}

void msg_v2_errlog_2 (const err_msg_v2_const_type* const_blk, uint32 code1, uint32 code2)
{
 msg_v2_send_2 (&(const_blk->msg_v2_const_blk), (uint32)code1, (uint32)code2);
 err_put_log (const_blk->msg_v2_const_blk.desc.line, const_blk->fname);
}

void msg_v2_errlog_1 (const err_msg_v2_const_type* const_blk, uint32 code1)
{
 msg_v2_send_1 (&(const_blk->msg_v2_const_blk), (uint32)code1);
 err_put_log (const_blk->msg_v2_const_blk.desc.line, const_blk->fname);
}

void msg_v2_errlog_0 (const err_msg_v2_const_type* const_blk)
{
 msg_v2_send (&(const_blk->msg_v2_const_blk));
 err_put_log (const_blk->msg_v2_const_blk.desc.line, const_blk->fname);
}
