/*===========================================================================

Diag Legacy Service Mapping Layer Implementation for Debug Message 
(F3 Message) Service

GENERAL DESCRIPTION
   API definitons for Debug Message Service Mapping Layer.

EXTERNALIZED FUNCTIONS
   msg_send
   msg_send_1
   msg_send_2
   msg_send_3
   msg_send_var
   msg_sprintf
   Note: These functions should not be used directly, use the MSG_* macros instead.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2012-2017 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1.c4/services/diag/LSM/qurt/src/Diag_LSM_Msg.c#1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------
12/08/17   is      Resolve issues with parsing some F3s from userPD
09/29/17   sa      Send IOCTL for mask update after diagID is set.
10/17/17   is      Resolve crash with MSG macros in island mode
10/31/17   nk      Qshrink 2 support for uimage diag messages
06/08/17   gn      Added support for diag services to uImage
04/24/17   sp      Added support for extended logs in userPD
04/04/17   tbg     Corrected Qshrink flag passed to F3 Trace
06/12/16   sa      Fixed MSG_Sprintf userpd issue
06/18/15   sa      Added support for time sync feature.
11/14/14   jtm/vk  Multi-PD Optimizations
07/06/14   ph      Added support for Diag over STM
12/23/13   ph      Included header files required to account for removal of ASSERT macro
09/27/13   xy      Removed code under FEATURE_WINCE   
07/18/13   sr      Fixed the issue with F3s not coming out on Sensor PD 
03/20/13   sg      Added Qshrink 2.0 support 
01/25/13   sg      Added support for f3 save trace in multi-pd 
12/3/12    sg      Added support for MSG_ERR_LOG macros in sensors pd on adsp 
07/05/12   sg      Changes to bringup Diag MultiPD
04/12/10   sg      Merging both DCM rtos and WM files 
                   Moved msg_mask_type,msg_large_args structures
                   to msgi.h
01/20/10   sg      Ported Qshrink to WM7
09/29/09   mad     Included msg_arrays_i.h, removed MSG_TBL_GEN feature
09/23/09   sg      Mainlined code when MSM5000_IRAM_FWD is not defined
07/21/09   mad     Changed MSG_FMT_STR_ARG_SIZE (max. size of string argument
                   for MSG_SPRINTF macro) to 280.
07/07/09   mad     Using secure versions of strcpy and _vsnprintf, in place
                   of deprecated versions.
07/07/09   mad     Removed inclusion of customer.h and diagtune.h
04/02/09   mad     Added msg_send_ts(), to let clients pass in their timestamps.
                   Modified msg_send_prep() to take timestamps.
03/19/09   mad     Included customer.h directly
09/30/08   mad     Changes to take out Component services, and use 
                   filesystem calls to access windiag driver.
05/01/08   JV      Added support to update the copy of run-time masks in the 
                   msg_mask_tbl in this process during initialization and also
                       on mask change
11/29/07   mad     Created File
===========================================================================*/

#include "comdef.h"
#include "Diag_LSM_Msg_i.h"
#include "diagstub.h" /* for timestamp functions ts_get() */
#include "msg_pkt_defs.h" /* For msg_ext_type etc */
#include "msg.h"
#include "msgcfg.h"

#include "err.h"

#include "diagcmd.h" /* For DIAG_EXT_MSG_F */
#include "diag_shared_i.h" /* for definition of diag_data struct, and diag datatypes. */
#include "msgi.h"
#include "msg_arrays_i.h" /* for definition of different mask arrays and msg_mask_tbl */
#include "msg_qsr.h"
#include "qurt_qdi.h"
#include "Diag_LSMi.h" /* for ghWinDiag etc */
#include "Diag_LSM_Qdi_wrapper.h"
#include "Diag_LSM_F3_Trace.h"
#include "Diag_LSM_stm.h"
#include "diagdiag_v.h"
#include "diagbuf_mpd.h"
#include "micro_msg_diag_service.h"  /* For island mode check */
#include "micro_diagbuffer_msg_api_v.h" /* For micro_msg_v2_send** api's*/
#include <stringl/stringl.h>  /* For memscpy */

/*---------------------------------------------------------------------------
  This is the structure that is used to represent the final structure that
  is sent to the external device.  'msg_ext_store_type' is expanded to this
  structure in DIAG task context at the time it is sent to the comm layer.
---------------------------------------------------------------------------*/
typedef struct
{
   msg_hdr_type hdr;
   msg_desc_type desc;
   uint32 msg_hash;
   uint32 args[1];
}
msg_qsr_type;

typedef struct
{
   msg_hdr_type hdr;
   msg_desc_type desc;
   uint32 msg_hash;
   uint32 args[1];
}
msg_qsr_v2_type;

#define MSG_WCDMA_TIME_FORMAT 2
#define MSG_CDMA_TIME_FORMAT  3

/* Define the symbol that tells this module which time format to use. */
#if defined(FEATURE_WCDMA)
#define MSG_TIME_FORMAT MSG_WCDMA_TIME_FORMAT
#else
#define MSG_TIME_FORMAT MSG_CDMA_TIME_FORMAT
#endif

#if (MSG_TIME_FORMAT == MSG_WCDMA_TIME_FORMAT)

#include "seq.h"
extern boolean l1m_is_connected(void);


/* Number of bits in the frame number that is inserted into the timestamp,
and the mask to extract these bits. */
#define MSG_FN_NUM_BITS 10
#define MSG_FN_MASK 0x3ff
#endif


/* TODO: These statistics needs to be shared across processes */
/* Statistics */
static uint32 msg_dropped;  /* number of dropped messages */
static uint32 msg_drop_delta;   /* number of dropped messages */


#ifndef MSG_FMT_STR_ARG_SIZE
#define MSG_FMT_STR_ARG_SIZE 280 /* Max. size of string argument for MSG_SPRINTF macros */
#endif
#define MAX_VAR_ARGS 9


static void msg_save_trace(const void *,
                           msg_arg_type *,
                           uint8,
                           void *,
                           uint32);
/* Internal function declarations */

static byte* msg_send_prep( const msg_const_type *const_blk, unsigned int num_args, unsigned int *pLength,
                            unsigned long long timestamp, boolean ts_valid );
uint8 msg_get_time(qword *ts);
static byte* msg_sprintf_prep(const msg_const_type *const_blk, unsigned int num_args, unsigned int *pLength);
static byte* qsr_msg_send_prep(const msg_qsr_const_type *const_blk,
                               unsigned int num_args, unsigned int *pLength, uint32 isHash, boolean stm_tag);

static boolean msg_get_ssid_rt_mask(uint16 ssid, uint32 *mask);
static const char* msg_format_filename2(const char *filename);

/* Function pointer to update the time API to be used */
extern uint8 (*diag_time_get_LSM)(qword);

extern boolean msg_service_init;

#ifdef DIAG_UIMAGE_MODE
extern micro_msg_mask_type diag_msg_composite_mask[MSG_MASK_TBL_CNT];
#endif

extern uint8 msg_enabled_mask;

/* use this global buffer to read message mask in, from the kernel-mode
driver.Avoids a malloc everytime the mask needs updation. */
byte *gMsg_Mask_Read_Buf = NULL;
uint32 gMsg_Mask_Size = 0; /* size of gMsg_Mask_Read_Buf, this is calculated */
/*---------------------------------------------------------------------------------------------------
                                    External functions
(Do not call any of these functions directly, use the Macros defined in msg.h instead.)
---------------------------------------------------------------------------------------------------*/
static void
msg_save_trace
(
   const void *constant_data_ptr,
   msg_arg_type *args,
   uint8 nargs,
   void *msg,
   uint32 type
   )
{
   diag_f3_save_msg(constant_data_ptr, args, nargs, msg, type);

   return;
}

/*===========================================================================
FUNCTION MSG_SEND

DESCRIPTION
   This function sends out a debug message with no arguments across DiagPkt CS interface.
   Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
   windiag driver handle should be initialised.
===========================================================================*/
void
msg_send(const msg_const_type *const_blk)
{
   if (diag_qdi_handle >= DIAG_FD_MIN_VALUE)
   {
      byte *pMsg = NULL;
      const unsigned int num_args = 0;  /* # of message arguments */
      unsigned int nLength = 0;

      pMsg = msg_send_prep(const_blk, num_args, &nLength, 0, FALSE);
      if (pMsg)
      {
         diagbuf_mpd_commit(pMsg);
      }
      msg_save_trace((void *)const_blk, NULL, 0, (void *)pMsg, DIAG_F3_TRACE_STANDARD);
   }
} /* msg_send() */

/*===========================================================================
FUNCTION MSG_SEND_TS

DESCRIPTION
   This function sends out a debug message with no arguments, and uses 
   timestamp passed in by client.
   Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
   windiag driver handle should be initialised.
===========================================================================*/
void
msg_send_ts(const msg_const_type *const_blk, unsigned long long timestamp)
{
   if (diag_qdi_handle >= DIAG_FD_MIN_VALUE)
   {
      byte *pMsg = NULL;
      const unsigned int num_args = 0;  /* # of message arguments */
      unsigned int nLength = 0;

      pMsg = msg_send_prep(const_blk, num_args, &nLength, timestamp, TRUE);
      if (pMsg)
      {
         diagbuf_mpd_commit(pMsg);
      }
      msg_save_trace((void *)const_blk, NULL, 0, (void *)pMsg, DIAG_F3_TRACE_STANDARD);
   }
} /* msg_send() */

/*===========================================================================
FUNCTION MSG_SEND_1

DESCRIPTION
   This function sends out a debug message with 1 argument across DiagPkt CS interface.
   Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
   windiag driver handle should be initialised.
===========================================================================*/
void
msg_send_1(const msg_const_type *pconst_blk, uint32 xx_arg1)
{
   if (diag_qdi_handle >= DIAG_FD_MIN_VALUE)
   {
      byte *pMsg = NULL;

      unsigned int nLength = 0;
      const unsigned int num_args = 1;  /* # of message arguments */
      msg_arg_type f3_args[1];
      f3_args[0] = xx_arg1;

      pMsg = msg_send_prep(pconst_blk, num_args, &nLength, 0, FALSE);
      if (pMsg)
      {
         msg_ext_type *pTemp = (msg_ext_type *)(pMsg);

         /* Store the arguments in the buffer. */
         uint32 *args = pTemp->args;
         args[0] = xx_arg1;

         diagbuf_mpd_commit(pMsg);
      }
      msg_save_trace((void *)pconst_blk, f3_args, 1, (void *)pMsg, DIAG_F3_TRACE_STANDARD);
   }
} /* void msg_send_1() */

/*===========================================================================
FUNCTION MSG_SEND_2

DESCRIPTION
   This function sends out a debug message with 2 arguments across DiagPkt CS interface.
   Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
   windiag driver handle should be initialised.
===========================================================================*/
void
msg_send_2(const msg_const_type *pconst_blk, uint32 xx_arg1, uint32 xx_arg2)
{
   if (diag_qdi_handle >= DIAG_FD_MIN_VALUE)
   {
      byte *pMsg = NULL;

      unsigned int nLength = 0;
      const unsigned int num_args = 2;  /* # of message arguments */
      msg_arg_type f3_args[2];

      f3_args[0] = xx_arg1;
      f3_args[1] = xx_arg2;
      pMsg = msg_send_prep(pconst_blk, num_args, &nLength, 0, FALSE);

      if (pMsg)
      {

         msg_ext_type *pTemp = (msg_ext_type *)(pMsg);
         /* Store the arguments in the buffer. */
         uint32 *args = MSG_LARGE_ARGS(pTemp->args);
         args[0] = xx_arg1;
         args[1] = xx_arg2;

         diagbuf_mpd_commit(pMsg);
      }
      msg_save_trace((void *)pconst_blk, f3_args, 2, (void *)pMsg, DIAG_F3_TRACE_STANDARD);
   }
}               /* msg_send_2() */

/*===========================================================================
FUNCTION MSG_SEND_3

DESCRIPTION
This function sends out a debug message with 2 arguments across DiagPkt CS interface.
Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
windiag driver handle should be initialised.
===========================================================================*/
void msg_send_3( const msg_const_type *pconst_blk, uint32 xx_arg1,
                 uint32 xx_arg2, uint32 xx_arg3 )
{
   if (diag_qdi_handle >= DIAG_FD_MIN_VALUE)
   {
      byte *pMsg = NULL;
      unsigned int nLength = 0;
      const unsigned int num_args = 3;  /* # of message arguments */
      msg_arg_type f3_args[3];
      f3_args[0] = xx_arg1;
      f3_args[1] = xx_arg2;
      f3_args[2] = xx_arg3;

      pMsg = msg_send_prep(pconst_blk, num_args, &nLength, 0, FALSE);
      if (pMsg)
      {
         uint32 *args = NULL;

         msg_ext_type *pTemp = (msg_ext_type *)(pMsg);
         /* Store the arguments in the buffer. */
         args = MSG_LARGE_ARGS(pTemp->args);
         args[0] = xx_arg1;
         args[1] = xx_arg2;
         args[2] = xx_arg3;

         diagbuf_mpd_commit(pMsg);
      }
      msg_save_trace((void *)pconst_blk, f3_args, 3, (void *)pMsg, DIAG_F3_TRACE_STANDARD);
   }
} /* msg_send_3() */


/*===========================================================================
FUNCTION MSG_SEND_VAR

DESCRIPTION
   This function sends out a debug message with variable number of arguments 
   across DiagPkt CS interface.
   Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
   windiag driver handle should be initialised.

===========================================================================*/
void
msg_send_var(const msg_const_type *pconst_blk, uint32 num_args, ...)
{
   if (diag_qdi_handle >= DIAG_FD_MIN_VALUE)
   {
      byte *pMsg = NULL;
      unsigned int nLength = 0;
      va_list arg_list;
      unsigned int i;
      msg_arg_type f3_args[9];  // Assuming at most 9 arguments
      va_start(arg_list, num_args);

      /* Store arguments from variable list. */
      for (i = 0; i < MIN(num_args, MAX_VAR_ARGS); i++)
      {
         f3_args[i] = va_arg(arg_list, uint32);
      }               /* for */

      /* Reset variable arguments */
      va_end(arg_list);
      pMsg = msg_send_prep(pconst_blk, num_args, &nLength, 0, FALSE);
      if (pMsg)
      {


         msg_ext_type *pTemp = (msg_ext_type *)(pMsg);
         /* Store the arguments in the buffer. */
         uint32 *args = MSG_LARGE_ARGS(pTemp->args);
         /* Initialize variable arguments */
         va_start(arg_list, num_args);
         /* Store arguments from variable list. */
         for (i = 0; i < num_args; i++)
         {
            args[i] = va_arg(arg_list, uint32);
         }
         /* Reset variable arguments */
         va_end(arg_list);
         diagbuf_mpd_commit(pMsg);
      }
      msg_save_trace((const void *)pconst_blk, f3_args, MIN(num_args, MAX_VAR_ARGS), (void *)pMsg, DIAG_F3_TRACE_STANDARD);
   }
} /* msg_send_var() */

/*===========================================================================
FUNCTION MSG_SPRINTF

DESCRIPTION
  
   This function sends out a debug message with variable number of arguments 
   across DiagPkt CS interface.
   This will build a message sprintf diagnostic Message with var #  
   of parameters.
   Do not call directly; use macro MSG_SPRINTF_* defined in msg.h 

DEPENDENCIES
   windiag driver handle should be initialised.
===========================================================================*/
void
msg_sprintf(const msg_const_type *const_blk, ...)
{

   uint32 pkt_length = 0;
   if (const_blk && (diag_qdi_handle >= 0))
   {
      /* Declared union for future use */
      typedef union
      {
         msg_ext_type ext;
      } msg_sprintf_desc_type;
      byte *pMsg = NULL;
      unsigned int int_cnt = 0;           /* Calculate the # args, to allocate buffer */
      unsigned int fname_length = 0;      /* Stores the file name along with '\0'     */
      unsigned int fmt_length = 0;        /* Stores the fmt length,'\0' and arg size  */
      unsigned int total_allocated = 0;   /* Total buffer allocated                   */
      const char *abb_filename = NULL;

      /* Point to note: In AMSS, two files cannot have the same name,
      so the whole file path is not displayed in QXDM. In WM, that need not be the case?
       For now, get the file name only, as done in legacy diag. */
      abb_filename = msg_format_filename2(const_blk->fname);
      fname_length = strlen(abb_filename) + 1;

      fmt_length = strlen(const_blk->fmt) + 1 + MSG_FMT_STR_ARG_SIZE;
      /* Calculate # of arguments to ensure enough space is allocated. */
      int_cnt = sizeof(msg_desc_type) - FSIZ(msg_ext_store_type, const_data_ptr) +
         fmt_length + fname_length;
      /* Calculates number of uint32s required */
      int_cnt = (int_cnt + sizeof(uint32) - 1) / sizeof(uint32);
      /*  Allocates the buffer required, fills in the header  */
      pMsg = msg_sprintf_prep(const_blk, int_cnt, &total_allocated);
      if (pMsg)
      {
         msg_sprintf_desc_type *pTemp = (msg_sprintf_desc_type *)(pMsg);
         char *str = NULL; /* Used to copy the file name and fmt string to the msg */
         va_list arg_list;   /* ptr to the variable argument list */
         unsigned int fmt_len_available = 0; /* Remaining buffer for format string       */

         /* Queue a debug message in Extended Message Format. */
         pTemp->ext.hdr.cmd_code = DIAG_EXT_MSG_F;
         /* This function embedds the argument in the string itself.
          ** Hence the num_args is assigned 0 */
         pTemp->ext.hdr.num_args = 0;
         /* Copy 'desc'. */
         pTemp->ext.desc = const_blk->desc;
         /* Copy the format string where the argument list would start.
             Since there are no arguments, the format string starts in the 'args'
             field. */
         str = (char *)pTemp->ext.args;
         /* Calculate the buffer left to copy the format string */
         fmt_len_available = total_allocated - (FPOS(msg_ext_type, args) + fname_length);
         if (fmt_len_available < fmt_length)
         {
            fmt_length = fmt_len_available;
         }
         /* Initialize variable argument list */
         va_start(arg_list, const_blk);
         /* Copy the format string with arguments */
         vsnprintf(str, fmt_length, const_blk->fmt, arg_list);

         str[fmt_length - 1] = '\0';
         /* Reset variable arguments */
         va_end(arg_list);
         /* Move the str pass the format string, strlen excludes the terminal
         ** NULL hence 1 is added to include NULL. */
         str += strlen((const char *)str) + 1;
         /* Copy the filename */
         strlcpy(str, abb_filename, fname_length);
         /* Move the str pass the filename, strlen excludes the terminal NULL
         ** hence 1 is added to include NULL. */
         str += strlen((const char *)str) + 1;
      /*shorten the packet for tool to parse it properly else the filename is not printed correctly*/
         pkt_length = (uint32)(str - (char*)pMsg);
         diagbuf_mpd_shorten(pMsg, pkt_length);
         diagbuf_mpd_commit(pMsg);
      } /* if (pMsg) */
   } /* if(const_blk && ghWinDiag ) */
   return;
}  /* msg_sprintf */

/*===========================================================================
FUNCTION qsr_int_msg_send

DESCRIPTION
   This function sends out a debug message with no arguments.
   This function, or the macro QSR_MSG should not be called directly,
   MSG macros are converted to QSR_MSG macro by text-replacement.
   
DEPENDENCIES
   Diag_LSM_Init() should be called before calling this function.
===========================================================================*/
void qsr_int_msg_send(const msg_qsr_const_type *const_blk, uint32 isHash)
{
   if (diag_qdi_handle >= DIAG_FD_MIN_VALUE)
   {
      byte *pMsg = NULL;
      const unsigned int num_args = 0;  /* # of message arguments */
      unsigned int nLength = 0;
      boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */

      pMsg = qsr_msg_send_prep(const_blk, num_args, &nLength, isHash, stm_enabled);
      if (pMsg)
      {
#ifdef FEATURE_DIAG_STM
         if (stm_enabled)
         {
            diag_stm_write_pkt((void *)(pMsg), nLength);

            /* Free data */
            diag_stm_msg_free(pMsg);
         }
         else
#endif
         {
            diagbuf_mpd_commit(pMsg);
         }
      }
      msg_save_trace((const void *)const_blk, NULL, 0, (void *)(pMsg), DIAG_F3_TRACE_QSHRINK20);
   }
} /* qsr_int_msg_send */

/*===========================================================================
FUNCTION qsr_int_msg_send_1

DESCRIPTION
   This function sends out a debug message with one argument.
   This function, or the macro QSR_MSG_1 should not be called directly,
   MSG* macros are converted to QSR_MSG* macros by text-replacement.
   
DEPENDENCIES
   Diag_LSM_Init() should be called before calling this function.
===========================================================================*/
void qsr_int_msg_send_1(const msg_qsr_const_type *const_blk, uint32 isHash, uint32 xx_arg1)
{
   if (diag_qdi_handle >= DIAG_FD_MIN_VALUE)
   {
      byte *pMsg = NULL;
      const unsigned int num_args = 1;  /* # of message arguments */
      unsigned int nLength = 0;
      msg_arg_type f3_args[1];
      boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */

      f3_args[0] = xx_arg1;

      pMsg = qsr_msg_send_prep(const_blk, num_args, &nLength, isHash, stm_enabled);
      if (pMsg)
      {
         if (QSR_BITFLAG_IS_HASH & isHash)
         {
            msg_qsr_type *pTemp = (msg_qsr_type *)(pMsg);
            /* Store the arguments in the buffer. */
            pTemp->args[0] = xx_arg1;
         }
         else
         {
            msg_ext_type *pTemp = (msg_ext_type *)(pMsg);
            /* Store the arguments in the buffer. */
            pTemp->args[0] = xx_arg1;
         }
#ifdef FEATURE_DIAG_STM
         if (stm_enabled)
         {
            diag_stm_write_pkt((void *)(pMsg), nLength);
            /* Free data */
            diag_stm_msg_free(pMsg);
         }
         else
#endif
         {
            diagbuf_mpd_commit(pMsg);
         }
      }
      msg_save_trace((const void *)const_blk, f3_args, 1, (void *)pMsg, DIAG_F3_TRACE_QSHRINK20);
   }
} /* qsr_int_msg_send_1 */

/*===========================================================================
FUNCTION qsr_int_msg_send_2

DESCRIPTION
   This function sends out a debug message with two arguments.
   This function, or the macro QSR_MSG_2 should not be called directly,
   MSG* macros are converted to QSR_MSG* macros by text-replacement.
   
DEPENDENCIES
   Diag_LSM_Init() should be called before calling this function.
===========================================================================*/

void qsr_int_msg_send_2(const msg_qsr_const_type *const_blk, uint32 isHash, uint32 xx_arg1, uint32 xx_arg2)
{
   if (diag_qdi_handle >= DIAG_FD_MIN_VALUE)
   {
      byte *pMsg = NULL;
      const unsigned int num_args = 2;  /* # of message arguments */
      unsigned int nLength = 0;
      msg_arg_type f3_args[2];
      boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */

      f3_args[0] = xx_arg1;
      f3_args[1] = xx_arg2;

      pMsg = qsr_msg_send_prep(const_blk, num_args, &nLength, isHash, stm_enabled);
      if (pMsg)
      {
         if (QSR_BITFLAG_IS_HASH & isHash)
         {
            msg_qsr_type *pTemp = (msg_qsr_type *)(pMsg);
            /* Store the arguments in the buffer. */
            pTemp->args[0] = xx_arg1;
            pTemp->args[1] = xx_arg2;
         }
         else
         {
            msg_ext_type *pTemp = (msg_ext_type *)(pMsg);
            /* Store the arguments in the buffer. */
            pTemp->args[0] = xx_arg1;
            pTemp->args[1] = xx_arg2;
         }
#ifdef FEATURE_DIAG_STM
         if (stm_enabled)
         {
            diag_stm_write_pkt((void *)(pMsg), nLength);
            /* Free data */
            diag_stm_msg_free(pMsg);
         }
         else
#endif
         {
            diagbuf_mpd_commit(pMsg);
         }
      }
      msg_save_trace((const void *)const_blk, f3_args, 2, (void *)pMsg, DIAG_F3_TRACE_QSHRINK20);
   }
} /* qsr_int_msg_send_2 */

/*===========================================================================
FUNCTION qsr_int_msg_send_3

DESCRIPTION
   This function sends out a debug message with three arguments.
   This function, or the macro QSR_MSG_3 should not be called directly,
   MSG* macros are converted to QSR_MSG* macros by text-replacement.
   
DEPENDENCIES
   Diag_LSM_Init() should be called before calling this function.
===========================================================================*/

void qsr_int_msg_send_3(const msg_qsr_const_type *const_blk, uint32 isHash, uint32 xx_arg1, uint32 xx_arg2, uint32 xx_arg3)
{
   if (diag_qdi_handle >= DIAG_FD_MIN_VALUE)
   {
      byte *pMsg = NULL;
      const unsigned int num_args = 3;  /* # of message arguments */
      msg_arg_type f3_args[3];
      unsigned int nLength = 0;
      boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */

      f3_args[0] = xx_arg1;
      f3_args[1] = xx_arg2;
      f3_args[2] = xx_arg3;

      pMsg = qsr_msg_send_prep(const_blk, num_args, &nLength, isHash, stm_enabled);
      if (pMsg)
      {
         if (QSR_BITFLAG_IS_HASH & isHash)
         {
            msg_qsr_type *pTemp = (msg_qsr_type *)(pMsg);
            /* Store the arguments in the buffer. */
            pTemp->args[0] = xx_arg1;
            pTemp->args[1] = xx_arg2;
            pTemp->args[2] = xx_arg3;
         }
         else
         {
            msg_ext_type *pTemp = (msg_ext_type *)(pMsg);
            /* Store the arguments in the buffer. */
            pTemp->args[0] = xx_arg1;
            pTemp->args[1] = xx_arg2;
            pTemp->args[2] = xx_arg3;
         }
#ifdef FEATURE_DIAG_STM
         if (stm_enabled)
         {
            diag_stm_write_pkt((void *)(pMsg), nLength);
            /* Free data */
            diag_stm_msg_free(pMsg);
         }
         else
#endif
         {
            diagbuf_mpd_commit(pMsg);
         }
      }
      msg_save_trace((const void *)const_blk, f3_args, 3, (void *)pMsg, DIAG_F3_TRACE_QSHRINK20);
   }
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
void qsr_msg_send(const msg_qsr_const_type *const_blk)
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

void qsr_msg_send_1(const msg_qsr_const_type *const_blk, uint32 xx_arg1)
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

void qsr_msg_send_2(const msg_qsr_const_type *const_blk, uint32 xx_arg1, uint32 xx_arg2)
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
void qsr_msg_send_3(const msg_qsr_const_type *const_blk, uint32 xx_arg1,
                    uint32 xx_arg2, uint32 xx_arg3)
{
   qsr_int_msg_send_3(const_blk, TRUE, xx_arg1, xx_arg2, xx_arg3);

} /* qsr_msg_send_3 */

/*===========================================================================
FUNCTION qsr_int_msg_send_var

DESCRIPTION
   This function sends out a debug message with variable number of arguments.
   This function, or the macro QSR_MSG_* should not be called directly,
   MSG* macros are converted to QSR_MSG* macros by text-replacement.
   
DEPENDENCIES
   Diag_LSM_Init() should be called before calling this function.
===========================================================================*/
void qsr_int_msg_send_var(const msg_qsr_const_type *const_blk, uint32 isHash, uint32 num_args, ...)
{
   if (diag_qdi_handle >= DIAG_FD_MIN_VALUE)
   {
      byte *pMsg = NULL;
      unsigned int nLength = 0;
      unsigned int i;
      msg_arg_type f3_args[MAX_VAR_ARGS];  // Assuming at most MAX_VAR_ARGS arguments
      va_list arg_list;     /* ptr to the variable argument list */
      va_start(arg_list, num_args);
      boolean stm_enabled = diag_stm_enabled; /* Store global for persistent behavior */

      /* Store arguments from variable list. */
      for (i = 0; i < MIN(num_args, MAX_VAR_ARGS); i++)
      {
         f3_args[i] = va_arg(arg_list, uint32);
      }               /* for */

      /* Reset variable arguments */
      va_end(arg_list);

      pMsg = qsr_msg_send_prep(const_blk, num_args, &nLength, isHash, stm_enabled);
      if (pMsg)
      {
         if (QSR_BITFLAG_IS_HASH & isHash)
         {
           msg_qsr_type *pTemp = (msg_qsr_type *)(pMsg);
           uint32 *args = MSG_LARGE_ARGS(pTemp->args);
           
           /* Initialize variable arguments */
           va_start(arg_list, num_args);
           /* Store arguments from variable list. */
           for (i = 0; i < num_args; i++)
           {
              args[i] = va_arg(arg_list, uint32);
           }
           /* Reset variable arguments */
           va_end(arg_list);
           }
         else
         {
           msg_ext_type *pTemp = (msg_ext_type *)(pMsg);
           uint32 *args = MSG_LARGE_ARGS(pTemp->args);
           
           /* Initialize variable arguments */
           va_start(arg_list, num_args);
           /* Store arguments from variable list. */
           for (i = 0; i < num_args; i++)
           {
              args[i] = va_arg(arg_list, uint32);
           }
           /* Reset variable arguments */
           va_end(arg_list);
         }
         
#ifdef FEATURE_DIAG_STM
         if (stm_enabled)
         {
            diag_stm_write_pkt((void *)(pMsg), nLength);
            /* Free data */
            diag_stm_msg_free(pMsg);
         }
         else
#endif
         {
            diagbuf_mpd_commit(pMsg);
         }
      }
      msg_save_trace((const void *)const_blk, f3_args, MIN(num_args, MAX_VAR_ARGS), (void *)pMsg, DIAG_F3_TRACE_QSHRINK20);
   }
} /* qsr_int_msg_send_var */


/*----------------------------------------------------------------------------
                                          Internal functions
-----------------------------------------------------------------------------*/
/* 
Computes message mask size.
*/
static uint32 Diag_LSM_Msg_ComputeMaskSize(void)
{
   uint32 msg_mask_size = 0;
   int i = 0;
   for (i = 0; i < MSG_MASK_TBL_CNT; i++)
   {
      msg_mask_size += sizeof(uint16) + sizeof(uint16);
      msg_mask_size += (msg_mask_tbl[i].ssid_last - msg_mask_tbl[i].ssid_first + 1) * sizeof(uint32);
   }
   return (msg_mask_size);
}

/*===========================================================================
FUNCTION Diag_LSM_Msg_Init

DESCRIPTION
Initializes the Diag Message service mapping layer.
Masks will be updated later after diagID has been set.

DEPENDENCIES
None

===========================================================================*/
boolean Diag_LSM_Msg_Init(void)
{
   boolean status = TRUE;
   gMsg_Mask_Size = Diag_LSM_Msg_ComputeMaskSize();
   if (!gMsg_Mask_Read_Buf)
   {
      gMsg_Mask_Read_Buf = malloc(gMsg_Mask_Size);
      if (!gMsg_Mask_Read_Buf)
      {
         status = FALSE;
      }
      else
      {
         status = TRUE;
      }

      if(status == TRUE)
      {
        msg_service_init = TRUE; 
        msg_enabled_mask = DIAG_MSG_STREAM_1;
      }
   }
   return (status);
} /* Diag_LSM_Msg_Init() */

/*===========================================================================
FUNCTION Diag_LSM_Msg_DeInit

DESCRIPTION
Prepares mapping layer exit for Diag message service.
Frees the global buffer allocated to read in message mask.

DEPENDENCIES
None. 


===========================================================================*/
boolean Diag_LSM_Msg_DeInit(void)
{
   boolean success = TRUE;
   if (gMsg_Mask_Read_Buf)
   {
      free(gMsg_Mask_Read_Buf);
      gMsg_Mask_Read_Buf = NULL;
   }
   return (success);
} /* boolean Diag_LSM_Msg_DeInit() */


/*===========================================================================
FUNCTION msg_update_mask

DESCRIPTION
   Updates the msg mask. This function is called whenever there is a
   mask change event.

DEPENDENCIES
None. 


===========================================================================*/
boolean msg_update_mask(void)
{

   boolean status = TRUE;
   if (gMsg_Mask_Read_Buf && (diag_qdi_handle >= DIAG_FD_MIN_VALUE))
   {
      dword maskLen_Recvd = 0;
      if (diag_lsm_ioctl(DIAG_IOCTL_GETMSGMASK, NULL, 0, gMsg_Mask_Read_Buf, gMsg_Mask_Size, &maskLen_Recvd) && maskLen_Recvd <= gMsg_Mask_Size)
      {
         /* Copy the mask received into this process' msg_mask_tbl */
         int i = 0;
         uint32 length = 0;
         msg_mask_read_buffer_type *ptemp_buf = (msg_mask_read_buffer_type *)gMsg_Mask_Read_Buf;
         byte *ptemp_byte = gMsg_Mask_Read_Buf;
         for (i = 0; i < MSG_MASK_TBL_CNT; i++)
         {
            ptemp_buf = (msg_mask_read_buffer_type *)ptemp_byte;
            if (ptemp_buf->ssid_first == msg_mask_tbl[i].ssid_first &&
                ptemp_buf->ssid_last == msg_mask_tbl[i].ssid_last)
            {
               ptemp_byte += sizeof(ptemp_buf->ssid_first) + sizeof(ptemp_buf->ssid_last);
               length = (msg_mask_tbl[i].ssid_last - msg_mask_tbl[i].ssid_first + 1) * sizeof(uint32);
               memcpy((byte *)(msg_mask_tbl[i].rt_mask_array[DIAG_STREAM_1 - 1]), ptemp_byte, length);
               ptemp_byte += length;
			   /* Update diag_msg_composite_mask when DIAG_IOCTL_GETMSGMASK is successful */
			   #ifdef DIAG_UIMAGE_MODE
		       if(msg_mask_tbl[i].ssid_first == diag_msg_composite_mask[i].ssid_first
			   	&& msg_mask_tbl[i].ssid_last == diag_msg_composite_mask[i].ssid_last)
		       {
		         memscpy(diag_msg_composite_mask[i].rt_mask_array,length,msg_mask_tbl[i].rt_mask_array[DIAG_STREAM_1 - 1],length);
		       }
			   #endif
            }
            else
            {
               /* Something is wrong if the ssid ranges don't match, just bail out */
               status = FALSE;
               break;
            }
         }
      }
      else
      {
         status = FALSE;
      }
   }
   else
   {
      status = FALSE;
   }
   return (status);
} /* msg_update_mask() */


/*===========================================================================
FUNCTION msg_format_filename2

DESCRIPTION
retrieves the position of filename from full file path.

DEPENDENCIES
None. 
===========================================================================*/
static const char*
msg_format_filename2(const char *filename)
{
   const char *p_front = filename;
   const char *p_end = filename + strlen(filename);

   while (p_end != p_front)
   {
      if ((*p_end == '\\') || (*p_end == ':'))
      {
         p_end++;
         break;
      }
      p_end--;
   }
   return (p_end);
} /*const char *msg_format_filename2() */

/*===========================================================================
FUNCTION msg_get_time

DESCRIPTION
  Gets Timestamp.

DEPENDENCIES
  None. 
===========================================================================*/
uint8
msg_get_time(qword * ts)
{

   uint8 time_type;
  /* Get timestamp for this message ASAP */
   time_type = diag_time_get_LSM(*ts);

#if (MSG_TIME_FORMAT == MSG_WCDMA_TIME_FORMAT)

   {
      /* Get the frame number (either SFN or CFN, depending on the
      ** state of the mobile), extract its least significant 10 bits
      ** and add these bits to the timestamp.
      */
      uint32 frame_number;

      if (l1m_is_connected())
      {
         frame_number = seq_get_cfn();
      }
      else
      {
         frame_number = seq_get_sfn() & MSG_FN_MASK;
      }
      (*ts)[0] >>= MSG_FN_NUM_BITS;
      (*ts)[0] <<= MSG_FN_NUM_BITS;
      (*ts)[0] |= frame_number;
   }


#endif

  return time_type;
} /* void msg_get_time() */

/*===========================================================================

FUNCTION MSG_SEND_PREP

DESCRIPTION
   Prepares the buffer needed by msg_send*().
   Allocates, fills in all data except arguments, and returns a pointer
   to the allocated message buffer.  It also handles message statisitics.

RETURN VALUE
   Returns the allocated buffer, and the length of the buffer

DEPENDENCIES
   None
===========================================================================*/
static byte*
msg_send_prep
(const msg_const_type *const_blk, unsigned int num_args, unsigned int *pLength,
 unsigned long long timestamp, boolean ts_valid)
{
   uint32 rt_mask;
   boolean valid_ssid = FALSE;
   byte *pMsg = NULL;

   if (pLength)
   {
      *pLength = 0;
   }
   /* Check the runtime mask */
   valid_ssid = msg_get_ssid_rt_mask(const_blk->desc.ss_id, &rt_mask);
   if (valid_ssid && (const_blk->desc.ss_mask & rt_mask))
   {
      const char *abb_filename = NULL;
      unsigned int alloc_len = 0;
      abb_filename = msg_format_filename2(const_blk->fname);

      /* total number of bytes to be allocated, including dereferenced FileName and Format strings */
      alloc_len = FPOS(msg_ext_type, args) +
         num_args * FSIZ(msg_ext_type, args[0]) +
         strlen(abb_filename) + 1 +
         strlen(const_blk->fmt) + 1;

      /* Get a pointer to a buffer.  If it's a NULL pointer, there is no memory in the client heap. */
    pMsg = (byte *)diagbuf_mpd_alloc(0, alloc_len, 0, DIAGBUF_ALLOC_REQUEST_MSG, 0, DIAG_EXT_API_VERSION_0);
    if ( pMsg )
      {
         /* position of format string in the returned buffer. */
         msg_ext_type *pTemp = (msg_ext_type *)((byte *)pMsg);
         uint32 fmt_pos = FPOS(msg_ext_type, args) +
            num_args * FSIZ(msg_ext_type, args[0]);
         if (pLength)
         {
            *pLength = alloc_len; /* return the number of bytes allocated. */
         }
         if (ts_valid) /* client timestamp is valid, copy that into the header */
         {
            unsigned long *pts_long = QW_CVT_N2Q(&timestamp);
            pTemp->hdr.ts[0] = *pts_long;
            pTemp->hdr.ts[1] = *(pts_long + 1);
         }
         else
         {
            pTemp->hdr.ts_type = msg_get_time(&(pTemp->hdr.ts));
         }

         pTemp->hdr.cmd_code = DIAG_EXT_MSG_F;
         pTemp->hdr.num_args = (uint8)num_args;
         pTemp->hdr.drop_cnt = (unsigned char)((msg_drop_delta > 255) ? 255 : msg_drop_delta);
         msg_drop_delta = 0;   /* Reset delta drop count */

         /* expand it now, copy over the filename and format strings.
          The order is: hdr,desc,args,format string, filename. args are copied in the msg_send_1 etc... functions */
         memscpy((void *)((char *)(pMsg) + sizeof(msg_hdr_type)), (alloc_len - sizeof(msg_hdr_type)),
                 (void *)&(const_blk->desc), sizeof(msg_desc_type));

         memscpy((void *)((char *)(pMsg) + fmt_pos), (alloc_len - fmt_pos), 
                 (void *)(const_blk->fmt), strlen(const_blk->fmt) + 1);

         memscpy((void *)((char *)(pMsg) + fmt_pos + strlen(const_blk->fmt) + 1), 
                 (alloc_len - (fmt_pos + strlen(const_blk->fmt) + 1)), (void *)(abb_filename), 
                 strlen(abb_filename) + 1); 

      } /* if (pMsg) */
      else
      {
         msg_drop_delta++;
         msg_dropped++;        /* count up one more ignored message */
      } /* if (pMsg) */
   } /* if (valid_ssid && (const_blk->desc.ss_mask & rt_mask)) */

   return (pMsg);
}               /* msg_send_prep() */

/* 
In LSM, we're getting the mask only for one SSID.
Getting masks for a range of SSIDs is needed only in the DCM
counterpart of this function.
So simplifying the search logic in LSM.
Little optimizations.
*/
static boolean
msg_get_ssid_rt_mask(uint16 ssid, uint32 *mask)
{
   boolean success = FALSE;
   if (mask)
   {
      const msg_mask_tbl_type *tbl = msg_mask_tbl;
      const msg_mask_tbl_type *tbl_last = &msg_mask_tbl[MSG_MASK_TBL_CNT - 1];
      const uint32 *mask_array = NULL;  /* First pass forces a search */
      /* Initialize as successful.  If invalid entry found, set to FALSE. */
      success = TRUE;
      /* Table search: if mask_array is not set */
      while (!mask_array && tbl <= tbl_last)
      {
         if (ssid >= tbl->ssid_first && ssid <= tbl->ssid_last)
         {
            mask_array = tbl->rt_mask_array[DIAG_STREAM_1 - 1];
         }
         else
         {
            tbl++;        /* Look at next table entry */
         }
      }

      /* At the end of the loop, we should have the mask_array,
      o/w the ssid was invalid. */

      if (mask_array)
      {
         /* Valid SSID found.  Write mask to caller's mask. */
         *mask = mask_array[ssid - tbl->ssid_first];
      }
      else
      {
         /* Invalid SSID.  Indicate failure */
         *mask = 0;
         success = FALSE;
      }
   }

   return (success);
}               /* msg_get_ssid_rt_mask() */

/*===========================================================================
FUNCTION MSG_SPRINTF_PREP

DESCRIPTION
   Prepares the buffer needed by msg_sprintf().
   Allocates, fills in all data except arguments, and returns a pointer
   to the allocated message buffer.  It also handles message statisitics.

RETURN VALUE
   Returns the allocated buffer, and the length of the buffer

DEPENDENCIES
   None
===========================================================================*/
static byte*
msg_sprintf_prep(const msg_const_type *pconst_blk, unsigned int num_args, unsigned int *pLength)
{
   uint32 rt_mask;
   boolean valid_ssid = FALSE;
   byte *pMsg = NULL;
   unsigned int alloc_len = FPOS(msg_ext_store_type, args) +
      num_args * FSIZ(msg_ext_store_type, args[0]);

   /* Check the runtime mask */
   valid_ssid =
      msg_get_ssid_rt_mask(pconst_blk->desc.ss_id, &rt_mask);

   if (valid_ssid && (pconst_blk->desc.ss_mask & rt_mask))
   {
      /* Get a pointer to a buffer. If it's a NULL pointer, there is
         no memory in the UserPD diag buffer.*/
    pMsg = (byte *)diagbuf_mpd_alloc(0, alloc_len, 0, DIAGBUF_ALLOC_REQUEST_MSG, 0, DIAG_EXT_API_VERSION_0);
    if ( pMsg )
      {
         msg_ext_store_type *pTemp = (msg_ext_store_type *)((byte *)pMsg);

         if (pLength)
         {
            *pLength = alloc_len; /* return value, of how much memory is allocated */
         }

         pTemp->hdr.num_args = (uint8)num_args;
         pTemp->hdr.drop_cnt = (unsigned char)((msg_drop_delta > 255) ? 255 : msg_drop_delta);
         msg_drop_delta = 0;   /* Reset delta drop count */
         pTemp->hdr.ts_type = msg_get_time(&(pTemp->hdr.ts));
         /* Set the pointer to the constant blk, to be expanded by msg_sprintf */
         pTemp->const_data_ptr = pconst_blk;
      }
      else
      {
         msg_drop_delta++;
         msg_dropped++;        /* count up one more ignored message */
      }
   }

   return (pMsg);
}               /* msg_sprintf_prep() */

/*===========================================================================
FUNCTION qsr_msg_send_prep

DESCRIPTION
   Internal function.
   Prepares the buffer that is sent to diag driver by the qsr_msg_send* functions.
   The const block is expanded in the context of the caller.
   If pLength is passed in as NULL, the msg will still be created but the
   calling function wont know how much data has been allocated.
===========================================================================*/
static byte* qsr_msg_send_prep(const msg_qsr_const_type *const_blk,
                               unsigned int num_args, unsigned int *pLength, uint32 isHash, boolean stm_tag)
{
   uint32 rt_mask;
   boolean valid_ssid = FALSE;
   byte *pMsg = NULL;
   int filename_len = 0;
   char *str_buf;
   char null_terminator = '\0';

   if (pLength)
   {
      *pLength = 0;
   }
   /* Check the runtime mask */
   valid_ssid = msg_get_ssid_rt_mask(const_blk->desc.ss_id, &rt_mask);
   if (valid_ssid && (const_blk->desc.ss_mask & rt_mask))
   {
      unsigned int alloc_len = 0;

      /* total number of bytes to be allocated, including space for the hash value */
      if (QSR_BITFLAG_IS_HASH & isHash)
      {
         alloc_len = FPOS(msg_qsr_type, args) + /* includes header,desc and hash size */
            num_args * FSIZ(msg_qsr_type, args[0]);
      }
      else
      {
         for (str_buf = (char *)const_blk->msg_hash; *str_buf != ':'; str_buf++) ;
         filename_len = str_buf - (char *)const_blk->msg_hash;
         /* total number of bytes to be allocated, including dereferenced FileName and Format strings */
         alloc_len = FPOS(msg_ext_type, args) +
            num_args * FSIZ(msg_ext_type, args[0]) +
            MIN(MSG_MAX_STRLEN, (int)strlen((char *)const_blk->msg_hash + filename_len + 1)) + 1 +
            filename_len + 1;

      }

#ifdef FEATURE_DIAG_STM
      if (stm_tag)
      {
         pMsg = (byte *)diag_stm_msg_alloc(alloc_len);
      }
      else
#endif
      {
         /* Get a pointer to a buffer.  If it's a NULL pointer, there is no memory in the client heap. */
         pMsg = (byte *)diagbuf_mpd_alloc(0, alloc_len, 0, DIAGBUF_ALLOC_REQUEST_MSG, 0, DIAG_EXT_API_VERSION_0);
      }

      if (pMsg)
      {
         if (pLength)
         {
            *pLength = alloc_len; /* return the number of bytes allocated. */
         }

         if (QSR_BITFLAG_IS_HASH & isHash)
         {
            /* Find the position to copy in the header, const expanded values etc */
            msg_qsr_type *pTemp = (msg_qsr_type *)((byte *)pMsg);

            pTemp->hdr.cmd_code = DIAG_QSR_EXT_MSG_TERSE_F; /* cmd_code = 146 for QSR messages */

            pTemp->hdr.num_args = (uint8)num_args;
            pTemp->hdr.drop_cnt = (unsigned char)((msg_drop_delta > 255) ? 255 : msg_drop_delta);
            msg_drop_delta = 0;   /* Reset delta drop count */
            pTemp->hdr.ts_type = msg_get_time(&(pTemp->hdr.ts));

            /* expand it now.
             The order is: hdr (already done),desc,hash,args. args are copied in the qsr_msg_send* functions */
            pTemp->desc.line = const_blk->desc.line;
            pTemp->desc.ss_id = const_blk->desc.ss_id;
            pTemp->desc.ss_mask = const_blk->desc.ss_mask;
            pTemp->msg_hash = const_blk->msg_hash;
         }
         else
         {
            msg_qsr_v2_type *pTemp = (msg_qsr_v2_type *)((byte *)pMsg);
            uint32 fmt_pos = 0;
            uint16 tmp_length = 0;

            pTemp->hdr.cmd_code = DIAG_EXT_MSG_F; /* cmd_code = 146 for QSR messages */

            pTemp->hdr.num_args = (uint8)num_args;
            pTemp->hdr.drop_cnt = (unsigned char)((msg_drop_delta > 255) ? 255 : msg_drop_delta);
            msg_drop_delta = 0;   /* Reset delta drop count */
            pTemp->hdr.ts_type = msg_get_time(&(pTemp->hdr.ts));

            /* expand it now.
             The order is: hdr (already done),desc,args,format string, filename. args are copied in the qsr_msg_send* functions */
            pTemp->desc.line = const_blk->desc.line;
            pTemp->desc.ss_id = const_blk->desc.ss_id;
            pTemp->desc.ss_mask = const_blk->desc.ss_mask;

            /*The arguments are copied in qsr_msg_send* functions*/
            /*Copy the fmt string*/
            fmt_pos = sizeof(msg_hdr_type) + sizeof(msg_desc_type) + num_args * sizeof(pTemp->args[0]);
            tmp_length = MIN(MSG_MAX_STRLEN, (int)strlen((char *)const_blk->msg_hash + filename_len + 1));
            memscpy((void *)((char *)(pMsg) + fmt_pos), (alloc_len - fmt_pos), 
		            (void *)(const_blk->msg_hash + filename_len + 1), tmp_length);

            /*Copy the null terminator*/
            fmt_pos += tmp_length;

            *((char *)(pMsg) + fmt_pos) = null_terminator;
            fmt_pos++;

            /*Copy the file name*/
            memscpy((void *)((char *)(pMsg) + fmt_pos), (alloc_len - fmt_pos), 
		            (void *)(const_blk->msg_hash), filename_len);
            fmt_pos += filename_len;

            /*Copy the null terminator*/
            *((char *)(pMsg) + fmt_pos) = null_terminator;
         }
      }
      else
      {
         msg_drop_delta++;
         msg_dropped++;        /* count up one more ignored message */
      } /* if (pMsg) */
   } /* if (valid_ssid && (const_blk->desc.ss_mask & rt_mask)) */

   return (pMsg);

} /* qsr_msg_send_prep */
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
void msg_errlog_3(const msg_const_type *const_blk, uint32 code1, uint32 code2, uint32 code3)
{
   msg_send_3(const_blk, (uint32)code1, (uint32)code2, (uint32)code3);
   err_put_log(const_blk->desc.line, const_blk->fname);
}

void msg_errlog_2(const msg_const_type *const_blk, uint32 code1, uint32 code2)
{
   msg_send_2(const_blk, (uint32)code1, (uint32)code2);
   err_put_log(const_blk->desc.line, const_blk->fname);
}

void msg_errlog_1(const msg_const_type *const_blk, uint32 code1)
{
   msg_send_1(const_blk, (uint32)code1);
   err_put_log(const_blk->desc.line, const_blk->fname);
}

void msg_errlog_0(const msg_const_type *const_blk)
{
   msg_send(const_blk);
   err_put_log(const_blk->desc.line, const_blk->fname);
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
void msg_qsrerrlog_3(const err_msg_qsr_const_type *const_blk, uint32 code1, uint32 code2, uint32 code3)
{

   qsr_msg_send_3(&(const_blk->qsr_const_blk), (uint32)code1, (uint32)code2, (uint32)code3);
   err_put_log(const_blk->qsr_const_blk.desc.line, const_blk->fname);
}

void msg_qsrerrlog_2(const err_msg_qsr_const_type *const_blk, uint32 code1, uint32 code2)
{
   qsr_msg_send_2(&(const_blk->qsr_const_blk), (uint32)code1, (uint32)code2);
   err_put_log(const_blk->qsr_const_blk.desc.line, const_blk->fname);
}

void msg_qsrerrlog_1(const err_msg_qsr_const_type *const_blk, uint32 code1)
{
   qsr_msg_send_1(&(const_blk->qsr_const_blk), (uint32)code1);
   err_put_log(const_blk->qsr_const_blk.desc.line, const_blk->fname);
}

void msg_qsrerrlog_0(const err_msg_qsr_const_type *const_blk)
{
   qsr_msg_send(&(const_blk->qsr_const_blk));
   err_put_log(const_blk->qsr_const_blk.desc.line, const_blk->fname);
}

void msg_v2_errlog_3(const err_msg_v2_const_type *const_blk, uint32 code1, uint32 code2, uint32 code3)
{
   msg_v2_send_3(&(const_blk->msg_v2_const_blk), (uint32)code1, (uint32)code2, (uint32)code3);
   err_put_log(const_blk->msg_v2_const_blk.desc.line, const_blk->fname);
}

void msg_v2_errlog_2(const err_msg_v2_const_type *const_blk, uint32 code1, uint32 code2)
{
   msg_v2_send_2(&(const_blk->msg_v2_const_blk), (uint32)code1, (uint32)code2);
   err_put_log(const_blk->msg_v2_const_blk.desc.line, const_blk->fname);
}

void msg_v2_errlog_1(const err_msg_v2_const_type *const_blk, uint32 code1)
{
   msg_v2_send_1(&(const_blk->msg_v2_const_blk), (uint32)code1);
   err_put_log(const_blk->msg_v2_const_blk.desc.line, const_blk->fname);
}

void msg_v2_errlog_0(const err_msg_v2_const_type *const_blk)
{
   msg_v2_send(&(const_blk->msg_v2_const_blk));
   err_put_log(const_blk->msg_v2_const_blk.desc.line, const_blk->fname);
}
