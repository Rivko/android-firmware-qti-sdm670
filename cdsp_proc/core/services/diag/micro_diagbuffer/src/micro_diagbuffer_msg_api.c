/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     Mobile Diagnostic Message Services

General Description
  All the declarations, definitions, and routines necessary to support
  reporting messages for errors and debugging.
  These APIs will be called by clients when they want to generate a new debug message
  in uImage mode. the API will write the message into the uImage diag_buffer using
  the diag_buffer interface. A dedicated diag_buffer will be set aside for uImage logging.
  This buffer will act as a circular buffer and will not be drained while in uImage mode.
  Upon exiting uImage mode, Diag task will drain out the uImage diag_buffer as part of
  it’s regular draining process.



Copyright (c) 2015-2018 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*==========================================================================

                           Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/services/diag/micro_diagbuffer/src/micro_diagbuffer_msg_api.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/08/18   is      Add support for pd threshold buffering mode in island mode
10/31/17   is      Resolve crash with MSG macros in island mode
10/31/17   nk      Qshrink 2 support for uimage diag messages
06/08/17   gn      Added support for diag services to uImage
06/05/17   nk      Added debug support for uimage in root PD
09/20/16   as      Fixed Klocwork warning regarding f3_args
08/25/16   as      Fixed Klocwork issue
11/09/15   ph      Optimizations in rt maks for uimage.
04/22/15   xy      Added support for QShrink 4.0
08/12/15   xy      Verified DIAG uimage APIs in island mode
07/21/15   xy      Fixed compilation warnings.
06/15/15   xy      Added new DIAG APIs for uImage mode
06/15/15   xy      Added new DIAG APIs for uimage mode
05/12/15   xy      Added support for DIAG uImage APIs
05/12/15   xy      created file

============================================================================*/
#include "micro_msg_diag_service.h"
#include "diag_f3_tracei.h"
#include "diagdiag_v.h" /*for MAX_VALUE_UINT32 */
#include "msgcfg.h"
#include <stdarg.h>  /* Definitions va_start, va_end, etc */
#include "diag_v.h"
#include "micro_diagbuffer.h"
#include "diagi_v.h"
#include "micro_diagbuf_mpd_type.h"
#include "micro_diagbuffer_msg_api_v.h" /* For micro_msg_send** api's */
#include "diag_micro_i.h"               /* For micro_diagbuffer_drain_check() */

extern ULogHandle diag_err_log_handle;

#ifndef MAX_VAR_ARGS
  #define MAX_VAR_ARGS 9
#endif


extern uint8 msg_enabled_mask;
extern boolean msg_service_init;

#ifdef DIAG_UIMAGE_MODE
extern micro_diagbuf_mpd_type* micro_diagmpd_buf;
uint32 micro_alloc_count_msg = 0;
#endif /*DIAG_UIMAGE_MODE*/

extern void qsr_int_msg_send ( const msg_qsr_const_type * const_blk, uint32 isHash);
extern void qsr_int_msg_send_1 ( const msg_qsr_const_type * const_blk, uint32 isHash, uint32 xx_arg1);
extern void qsr_int_msg_send_2 ( const msg_qsr_const_type * const_blk, uint32 isHash, uint32 xx_arg1, uint32 xx_arg2);
extern void qsr_int_msg_send_3 ( const msg_qsr_const_type * const_blk, uint32 isHash, uint32 xx_arg1, uint32 xx_arg2, uint32 xx_arg3);
extern void qsr_int_msg_send_var (const msg_qsr_const_type * const_blk, uint32 isHash, uint32 num_args, ...);

#ifdef DIAG_UIMAGE_MODE
uint8 diag_uimage_f3_trace_buffer[DIAG_UIMAGE_F3_TRACE_BUFFER_SIZE];
diag_f3_trace_handle uimage_f3_trace = { FALSE, {0},  FALSE, DIAG_F3_TRACE_UIMAGE_QTIMER, &(diag_uimage_f3_trace_buffer[0]), 0 , DIAG_UIMAGE_F3_TRACE_BUFFER_SIZE};

static uint32 msg_micro_rt_masks_gen[1 + (MSG_SSID_GEN_LAST - MSG_SSID_GEN_FIRST )];
static uint32 msg_micro_rt_masks_oncrpc[1 + (MSG_SSID_ONCRPC_LAST - MSG_SSID_ONCRPC )];
static uint32 msg_micro_rt_masks_1x[1 + (MSG_SSID_1X_LAST - MSG_SSID_1X )];
static uint32 msg_micro_rt_masks_prot[1 + (MSG_SSID_HDR_LAST - MSG_SSID_HDR_PROT )];
static uint32 msg_micro_rt_masks_umts[1 + (MSG_SSID_UMTS_LAST - MSG_SSID_UMTS )];
static uint32 msg_micro_rt_masks_gsm[1 + (MSG_SSID_GSM_LAST - MSG_SSID_GSM )];
static uint32 msg_micro_rt_masks_wlan[1 + (MSG_SSID_WLAN_LAST - MSG_SSID_WLAN )];
static uint32 msg_micro_rt_masks_ats[1 + (MSG_SSID_MCS_LAST - MSG_SSID_ATS )];
static uint32 msg_micro_rt_masks_ds[1 + (MSG_SSID_DS_LAST - MSG_SSID_DS )];
static uint32 msg_micro_rt_masks_sec[1 + (MSG_SSID_SEC_LAST - MSG_SSID_SEC )];
static uint32 msg_micro_rt_masks_apps[1 + (MSG_SSID_APPS_LAST - MSG_SSID_APPS )];
static uint32 msg_micro_rt_masks_adsptasks[1 + (MSG_SSID_ADSPTASKS_LAST - MSG_SSID_ADSPTASKS )];
static uint32 msg_micro_rt_masks_l4_linux[1 + (MSG_SSID_L4LINUX_LAST - MSG_SSID_L4LINUX_KERNEL )];
static uint32 msg_micro_rt_masks_l4_iguana[1 + (MSG_SSID_L4IGUANA_LAST - MSG_SSID_L4IGUANA_IGUANASERVER )];
static uint32 msg_micro_rt_masks_l4_amss[1 + (MSG_SSID_L4AMSS_LAST - MSG_SSID_L4AMSS_QDIAG )];
static uint32 msg_micro_rt_masks_l4_hit[1 + (MSG_SSID_HIT_LAST - MSG_SSID_HIT )];
static uint32 msg_micro_rt_masks_qdsp6[1 + (MSG_SSID_QDSP6_LAST - MSG_SSID_QDSP6 )];
static uint32 msg_micro_rt_masks_umb[1 + (MSG_SSID_UMB_LAST - MSG_SSID_UMB )];
static uint32 msg_micro_rt_masks_lte[1 + (MSG_SSID_LTE_LAST - MSG_SSID_LTE )];
static uint32 msg_micro_rt_masks_qchat[1 + (MSG_SSID_QCHAT_LAST - MSG_SSID_QCHAT )];
static uint32 msg_micro_rt_masks_tdscdma[1 + (MSG_SSID_TDSCDMA_LAST - MSG_SSID_TDSCDMA_L1 )];
static uint32 msg_micro_rt_masks_cta[1 + (MSG_SSID_CTA_LAST - MSG_SSID_CTA )];
static uint32 msg_micro_rt_masks_qcnea[1 + (MSG_SSID_QCNEA_LAST - MSG_SSID_QCNEA )];



/* Defines and variables for saving to uimage msg mask */
micro_msg_mask_type diag_msg_composite_mask[MSG_MASK_TBL_CNT] = {
{
	MSG_SSID_GEN_FIRST,
	MSG_SSID_GEN_LAST,
	msg_micro_rt_masks_gen
}
,
{
	MSG_SSID_ONCRPC,
	MSG_SSID_ONCRPC_LAST,
	msg_micro_rt_masks_oncrpc
}
,

{
	MSG_SSID_1X,
	MSG_SSID_1X_LAST,
	msg_micro_rt_masks_1x
}
,

{
	MSG_SSID_HDR_PROT,
	MSG_SSID_HDR_LAST,
	msg_micro_rt_masks_prot
}
,

{
	MSG_SSID_UMTS,
	MSG_SSID_UMTS_LAST,
	msg_micro_rt_masks_umts
}
,

{
	MSG_SSID_GSM,
	MSG_SSID_GSM_LAST,
	msg_micro_rt_masks_gsm
}
,

{
	MSG_SSID_WLAN,
	MSG_SSID_WLAN_LAST,
	msg_micro_rt_masks_wlan
}
,

{
	MSG_SSID_ATS,
	MSG_SSID_MCS_LAST,
	msg_micro_rt_masks_ats
}
,

{
	MSG_SSID_DS,
	MSG_SSID_DS_LAST,
	msg_micro_rt_masks_ds
}
,

{
	MSG_SSID_SEC,
	MSG_SSID_SEC_LAST,
	msg_micro_rt_masks_sec
}
,

{
	MSG_SSID_APPS,
	MSG_SSID_APPS_LAST,
	msg_micro_rt_masks_apps
}
,

{
	MSG_SSID_ADSPTASKS,
	MSG_SSID_ADSPTASKS_LAST,
	msg_micro_rt_masks_adsptasks
}
,

{
	MSG_SSID_L4LINUX_KERNEL,
	MSG_SSID_L4LINUX_LAST,
	msg_micro_rt_masks_l4_linux
}
,

{
	MSG_SSID_L4IGUANA_IGUANASERVER,
	MSG_SSID_L4IGUANA_LAST,
	msg_micro_rt_masks_l4_iguana
}
,

{
	MSG_SSID_L4AMSS_QDIAG,
	MSG_SSID_L4AMSS_LAST,
	msg_micro_rt_masks_l4_amss
}
,

{
	MSG_SSID_HIT,
	MSG_SSID_HIT_LAST,
	msg_micro_rt_masks_l4_hit
}
,

{
	MSG_SSID_QDSP6,
	MSG_SSID_QDSP6_LAST,
	msg_micro_rt_masks_qdsp6
}
,

{
	MSG_SSID_UMB,
	MSG_SSID_UMB_LAST,
	msg_micro_rt_masks_umb
}
,

{
	MSG_SSID_LTE,
	MSG_SSID_LTE_LAST,
	msg_micro_rt_masks_lte
}
,

{
	MSG_SSID_QCHAT,
	MSG_SSID_QCHAT_LAST,
	msg_micro_rt_masks_qchat
}
,

{
	MSG_SSID_TDSCDMA_L1,
	MSG_SSID_TDSCDMA_LAST,
	msg_micro_rt_masks_tdscdma
}
,

{
	MSG_SSID_CTA,
	MSG_SSID_CTA_LAST,
	msg_micro_rt_masks_cta
}
,

{
	MSG_SSID_QCNEA,
	MSG_SSID_QCNEA_LAST,
	msg_micro_rt_masks_qcnea
}
};


/*===========================================================================

FUNCTION MICRO_MSG_SAVE_TRACE

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
void micro_msg_save_trace
(
  const void * constant_data_ptr,
  msg_arg_type * args,
  uint8 nargs,
  void * msg,
  uint32 type,
  uint32 version,
  void *parameter
)
{
  /* Is DIAG services initialized? */
  if (!diag_services_initialized)
    return;

  err_f3_save_msg(&uimage_f3_trace, constant_data_ptr, args, nargs, msg, type);

  return;
}
/*===========================================================================

FUNCTION MICRO_GET_MSG_MASK

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

boolean micro_get_msg_mask(uint16 curr_ssid, uint32 *rt_mask)
{
  uint8 mid_index;
  uint8 start_index = 1;  // starting at 1, since 0 is for legacy
  uint8 end_index = MSG_MASK_TBL_CNT - 1;
  uint8 table_index = MSG_MASK_TBL_CNT;
  uint16 msg_index = 0;
  boolean ret_value = FALSE;

  // Skips the search for the legacy messages.
  if (curr_ssid <= diag_msg_composite_mask[0].ssid_last)
  {
    table_index =  0;
  }
  else
  {

    /* the binary search modified to work with structures holding the start and end ssid values */
    while (start_index <= end_index)
    {
      mid_index = (start_index + end_index) >> 1;

      if (diag_msg_composite_mask[mid_index].ssid_first > curr_ssid)
      {
        end_index = mid_index - 1;
      }
      else if (curr_ssid > diag_msg_composite_mask[mid_index].ssid_last)
      {
        start_index = mid_index + 1;
      }
      else
      {
        table_index = mid_index;
        break;
      }
      /* Adjusting for the index returned by the binary search to make it correspond to the ssid_first in that structure */
      if ((start_index - end_index) == 1)
      {
        if (curr_ssid < diag_msg_composite_mask[start_index].ssid_first)
        {
          table_index = start_index - 1;
          break;
        }
        else if (curr_ssid < diag_msg_composite_mask[end_index].ssid_first)
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
    if (curr_ssid > diag_msg_composite_mask[table_index].ssid_last)
    {
      table_index =  MSG_MASK_TBL_CNT;
    }
  }

  if (table_index >= MSG_MASK_TBL_CNT)
  {
    ret_value = FALSE;
  }
  else
  {
    ret_value = TRUE;
    msg_index = curr_ssid - diag_msg_composite_mask[table_index].ssid_first;
    *rt_mask = diag_msg_composite_mask[table_index].rt_mask_array[msg_index];
  }
  return (ret_value);
} /* micro_get_msg_mask () */

/*===========================================================================

FUNCTION MICRO_MSG_MASK_ENABLED

DESCRIPTION
  This function will return if the runtime mask of the current msg is turned on
DEPENDENCIES

PARAMETERS
  const_desc - constant pointer to the current msg

RETURN VALUE
  If the mask was turned on, return TRUE.
  else, return FALSE.

SIDE EFFECTS
  None.
===========================================================================*/
 boolean micro_msg_mask_enabled(const msg_desc_type *const_desc)
{
  uint32 rt_mask = 0;
  boolean ret_value = FALSE;
  unsigned int ssid_index;


  if (const_desc != NULL)
  {
    ssid_index = const_desc->ss_id;
    /*check if the ssid is a valid ssid*/
    if (micro_get_msg_mask(ssid_index, &rt_mask) == TRUE && (const_desc->ss_mask & rt_mask))
    {
      ret_value = TRUE;
    }
  }
  return (ret_value);
  
} /* micro_msg_mask_enabled */
#endif /*DIAG_UIMAGE_MODE*/

/*===========================================================================

FUNCTION MSG_V2_SEND

DESCRIPTION
  This function is similar to QSR_MSG_SEND function, except the msg value. The msg value in const_blk may be
  converted to a hash value, if the QSHRINK20 tool is executed in the linker output.

  The msg value is a pointer to a string, if its value is divisible by 4, else it is a hash value.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void msg_v2_send ( const msg_v2_const_type * const_blk)
{
#ifdef DIAG_UIMAGE_MODE
  if(diag_island_mode_check())
  {
     micro_msg_v2_send_ext (const_blk, 0, NULL);
  }
  else
#endif
  {
  	 qsr_int_msg_send((msg_qsr_const_type *)const_blk, MSG_V2_CHECK_IS_HASH(const_blk->msg));
  }  	  
} /* msg_v2_send */

/*===========================================================================

FUNCTION MSG_V2_SEND_1

DESCRIPTION
  This function is similar to QSR_MSG_SEND_1 function, except the msg value. The msg value in const_blk may be
  converted to a hash value, if the QSHRINK20 tool is executed in the linker output.

  The msg value is a pointer to a string, if its value is divisible by 4, else it is a hash value.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void msg_v2_send_1 ( const msg_v2_const_type * const_blk,uint32 xx_arg1)
{
#ifdef DIAG_UIMAGE_MODE
  if(diag_island_mode_check())
  {
     micro_msg_v2_send_1_ext (const_blk, 0, NULL, xx_arg1);
  }
  else
#endif
  {
  	 qsr_int_msg_send_1((msg_qsr_const_type *)const_blk,
         MSG_V2_CHECK_IS_HASH(const_blk->msg),xx_arg1);
  }  	
} /* msg_v2_send_1 */

/*===========================================================================

FUNCTION MSG_V2_SEND_2

DESCRIPTION
  This function is similar to QSR_MSG_SEND_2 function, except the msg value. The msg value in const_blk may be
  converted to a hash value, if the QSHRINK20 tool is executed in the linker output.

  The msg value is a pointer to a string, if its value is divisible by 4, else it is a hash value.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void msg_v2_send_2 ( const msg_v2_const_type * const_blk, uint32 xx_arg1, uint32 xx_arg2)
{
#ifdef DIAG_UIMAGE_MODE
  if(diag_island_mode_check())
  {
     micro_msg_v2_send_2_ext (const_blk, 0, NULL, xx_arg1, xx_arg2);
  }
  else
#endif
  {
  	 qsr_int_msg_send_2((msg_qsr_const_type *)const_blk,
         MSG_V2_CHECK_IS_HASH(const_blk->msg),xx_arg1, xx_arg2);
  }
} /* msg_v2_send_2 */

/*===========================================================================

FUNCTION MSG_V2_SEND_3

DESCRIPTION
  This function is similar to QSR_MSG_SEND_3 function, except the msg value. The msg value in const_blk may be
  converted to a hash value, if the QSHRINK20 tool is executed in the linker output.

  The msg value is a pointer to a string, if its value is divisible by 4, else it is a hash value.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void msg_v2_send_3 ( const msg_v2_const_type * const_blk, uint32 xx_arg1,
        uint32 xx_arg2, uint32 xx_arg3)
{
#ifdef DIAG_UIMAGE_MODE
  if(diag_island_mode_check())
  {
     micro_msg_v2_send_3_ext (const_blk, 0, NULL, xx_arg1, xx_arg2, xx_arg3);
  }
  else
 #endif
 {
  	 qsr_int_msg_send_3((msg_qsr_const_type *)const_blk,
       MSG_V2_CHECK_IS_HASH(const_blk->msg),xx_arg1,xx_arg2,xx_arg3);
  }
} /* msg_v2_send_3 */


/*===========================================================================

FUNCTION MSG_V2_SEND_VAR

DESCRIPTION
  This function is similar to QSR_MSG_SEND_VAR function, except the msg value. The msg value in const_blk may be
  converted to a hash value, if the QSHRINK20 tool is executed in the linker output.

  The msg value is a pointer to a string, if its value is divisible by 4, else it is a hash value.

  Do not call directly; use macro MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void msg_v2_send_var (const msg_v2_const_type * const_blk,uint32 num_args, ...)
{
  int i = 0;
  int total_num_args = MIN(num_args, MAX_VAR_ARGS);
  va_list arg_list;	 /* ptr to the variable argument list */
   uint32 args[total_num_args];
	   
  va_start(arg_list, num_args);
  /* Store arguments from variable list. */
  for (i = 0; i < total_num_args; i++)
  {
    args[i] = va_arg(arg_list, uint32);
  }
  va_end(arg_list);
       
  #ifdef DIAG_UIMAGE_MODE
  if(diag_island_mode_check())
  {
	   switch (total_num_args)
	   {
		 case 4:
		   micro_msg_v2_send_var_ext(const_blk, 0, NULL, 4, args[0], args[1], args[2], args[3]);
		   break;
		 case 5:
		   micro_msg_v2_send_var_ext(const_blk, 0, NULL, 5, args[0], args[1], args[2], args[3], args[4]);
		   break;
		 case 6:
		   micro_msg_v2_send_var_ext(const_blk, 0, NULL, 6, args[0], args[1], args[2], args[3], args[4], args[5]);
		   break;
		 case 7:
		   micro_msg_v2_send_var_ext(const_blk, 0, NULL, 7, args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
		   break;
		 case 8:
		   micro_msg_v2_send_var_ext(const_blk, 0, NULL, 8, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7]);
		   break;
		 case 9:
		   micro_msg_v2_send_var_ext(const_blk, 0, NULL, 9, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8]);
		   break;
	   }
  }
  else
#endif
  {
       switch (total_num_args)
	   {
		 case 4:
		   qsr_int_msg_send_var((msg_qsr_const_type *)const_blk, MSG_V2_CHECK_IS_HASH(const_blk->msg), 4, args[0], args[1], args[2], args[3]);
		   break;
		 case 5:
		   qsr_int_msg_send_var((msg_qsr_const_type *)const_blk, MSG_V2_CHECK_IS_HASH(const_blk->msg), 5, args[0], args[1], args[2], args[3], args[4]);
		   break;
		 case 6:
		   qsr_int_msg_send_var((msg_qsr_const_type *)const_blk, MSG_V2_CHECK_IS_HASH(const_blk->msg), 6, args[0], args[1], args[2], args[3], args[4], args[5]);
		   break;
		 case 7:
		   qsr_int_msg_send_var((msg_qsr_const_type *)const_blk, MSG_V2_CHECK_IS_HASH(const_blk->msg), 7, args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
		   break;
		 case 8:
		   qsr_int_msg_send_var((msg_qsr_const_type *)const_blk, MSG_V2_CHECK_IS_HASH(const_blk->msg), 8, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7]);
		   break;
		 case 9:
		   qsr_int_msg_send_var((msg_qsr_const_type *)const_blk, MSG_V2_CHECK_IS_HASH(const_blk->msg), 9, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8]);
		   break;
	   }
  }
} /* msg_v2_send_var */

#ifdef DIAG_UIMAGE_MODE
void _micro_msg_send(const msg_const_type *const_blk)
{
   micro_msg_send_ext(const_blk, 0, NULL);
}


void _micro_msg_send_1(const msg_const_type *const_blk, uint32 xx_arg1)
{
  micro_msg_send_1_ext (const_blk, 0, NULL, xx_arg1);
}

void _micro_msg_send_2(const msg_const_type *const_blk, uint32 xx_arg1, uint32 xx_arg2)
{
  micro_msg_send_2_ext (const_blk, 0, NULL, xx_arg1, xx_arg2);
}

void _micro_msg_send_3(const msg_const_type *const_blk, uint32 xx_arg1, uint32 xx_arg2, uint32 xx_arg3)
{
  micro_msg_send_3_ext (const_blk, 0, NULL, xx_arg1, xx_arg2,xx_arg3);
}


void _micro_msg_send_var(const msg_const_type *const_blk, uint32 num_args, ...)
{
  
   int i = 0;
   int total_num_args = MIN(num_args, MAX_VAR_ARGS);
   va_list arg_list;     /* ptr to the variable argument list */
   uint32 args[total_num_args];
   
   va_start(arg_list, num_args);
   /* Store arguments from variable list. */
   for (i = 0; i < total_num_args; i++)
   {
	 args[i] = va_arg(arg_list, uint32);
   }
   va_end(arg_list);
  
   switch (total_num_args)
  {
	 case 4:
	   micro_msg_send_var_ext(const_blk, 0, NULL, 4, args[0], args[1], args[2], args[3]);
	   break;
	 case 5:
	   micro_msg_send_var_ext(const_blk, 0, NULL, 5, args[0], args[1], args[2], args[3], args[4]);
	   break;
	 case 6:
	   micro_msg_send_var_ext(const_blk, 0, NULL, 6, args[0], args[1], args[2], args[3], args[4], args[5]);
	   break;
	 case 7:
	   micro_msg_send_var_ext(const_blk, 0, NULL, 7, args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
	   break;
	 case 8:
	   micro_msg_send_var_ext(const_blk, 0, NULL, 8, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7]);
	   break;
	 case 9:
	   micro_msg_send_var_ext(const_blk, 0, NULL, 9, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8]);
	   break;
  }

}


/*===========================================================================

FUNCTION MICRO_MSG_SEND_EXT

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with no arguments.

  Do not call directly; use macro MICRO_MSG_* defined in micro_msg_diag_service.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void micro_msg_send_ext(const msg_const_type *const_blk, uint32 version, void *parameter)
{


  if (msg_enabled_mask != 0 && msg_service_init == TRUE)
  {
    /*write the msg and write the msg into diag_buffer */
    if (micro_msg_mask_enabled(&const_blk->desc))
    {
	  if(micro_diagmpd_buf)
	  {
		_micro_diagbuffer_debug_msg(micro_diagmpd_buf->handle, const_blk, 0, version, parameter);
	  }
	  
      if (micro_alloc_count_msg == MAX_VALUE_UINT32)
      {
        MICRO_MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Variable micro_alloc_count_msg has reached maximum value.");
      }
      else
      {
        micro_alloc_count_msg++;
      }
    }
  }
#if defined (FEATURE_SAVE_DEBUG_TRACE)
  micro_msg_save_trace((void *)const_blk, NULL, 0, NULL, DIAG_F3_TRACE_STANDARD, version, parameter);
#endif /* FEATURE_SAVE_DEBUG_TRACE */

}   /* micro_msg_send_ext() */

/*===========================================================================

FUNCTION MICRO_MSG_V2_SEND_EXT

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with no arguments.

  Do not call directly; use MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void micro_msg_v2_send_ext(const msg_v2_const_type *const_blk, uint32 version, void *parameter)
{
  boolean drain_check = FALSE;
  
  if (msg_enabled_mask != 0 && msg_service_init == TRUE)
  {
    /*write the msg and write the msg into diag_buffer */
    if (micro_msg_mask_enabled(&const_blk->desc))
    {
      if(micro_diagmpd_buf)
	  {
	     _micro_diagbuffer_v2_debug_msg(micro_diagmpd_buf->handle, const_blk, 0, version, parameter);
      }
      if (micro_alloc_count_msg == MAX_VALUE_UINT32)
      {
        MICRO_MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Variable micro_alloc_count_msg has reached maximum value.");
      }
      else
      {
        micro_alloc_count_msg++;
      }
      drain_check = TRUE;
    }
  }
  
#if defined (FEATURE_SAVE_DEBUG_TRACE)
   micro_msg_save_trace((void *)const_blk, NULL, 0, NULL, DIAG_F3_TRACE_STANDARD, version, parameter);
#endif /* FEATURE_SAVE_DEBUG_TRACE */

  if( drain_check == TRUE )
  {
    micro_diagbuffer_drain_check();
  }

}   /* micro_msg_v2_send_ext() */

/*===========================================================================

FUNCTION MICRO_MSG_SEND_1_EXT

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with no arguments.

  Do not call directly; use macro MICRO_MSG_* defined in micro_msg_diag_service.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
micro_msg_send_1_ext (const msg_const_type *const_blk, uint32 version,
                        void *parameter, uint32 xx_arg1)
{
#if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type f3_args[1];
#endif /* FEATURE_SAVE_DEBUG_TRACE */
  if(msg_enabled_mask != 0 && msg_service_init == TRUE)
  {
    /*write the msg and write the msg into diag_buffer */
    if (micro_msg_mask_enabled(&const_blk->desc))
    {
	  if(micro_diagmpd_buf)
	  {
		_micro_diagbuffer_debug_msg(micro_diagmpd_buf->handle, const_blk, 1, version, parameter, xx_arg1);  
	  }
	  
      if ( micro_alloc_count_msg == MAX_VALUE_UINT32 )
      {
        MICRO_MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Variable micro_alloc_count_msg has reached maximum value.");
      }
      else
      {
        micro_alloc_count_msg++;
      }
    }
  }
#if defined (FEATURE_SAVE_DEBUG_TRACE)
   f3_args[0] = xx_arg1;
   micro_msg_save_trace((void *)const_blk, f3_args, 1, NULL, DIAG_F3_TRACE_STANDARD, version, parameter);
#endif /* FEATURE_SAVE_DEBUG_TRACE */

}   /* micro_msg_send_1_ext() */

/*===========================================================================

FUNCTION MICRO_MSG_V2_SEND_1_EXT

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with no arguments.

  Do not call directly; use MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
micro_msg_v2_send_1_ext (const msg_v2_const_type *const_blk, uint32 version,
                        void *parameter, uint32 xx_arg1)
{
#if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type f3_args[1];
#endif /* FEATURE_SAVE_DEBUG_TRACE */
  boolean drain_check = FALSE;
  
  if(msg_enabled_mask != 0 && msg_service_init == TRUE)
  {
    /*write the msg and write the msg into diag_buffer */
    if (micro_msg_mask_enabled(&const_blk->desc))
    {
	  if(micro_diagmpd_buf)
	  {
         _micro_diagbuffer_v2_debug_msg(micro_diagmpd_buf->handle, const_blk, 1, version, parameter, xx_arg1);
	  }
	  
      if ( micro_alloc_count_msg == MAX_VALUE_UINT32 )
      {
        MICRO_MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Variable micro_alloc_count_msg has reached maximum value.");
      }
      else
      {
        micro_alloc_count_msg++;
      }	
      drain_check = TRUE;
    }
  }
  
#if defined (FEATURE_SAVE_DEBUG_TRACE)
   f3_args[0] = xx_arg1;
   micro_msg_save_trace((void *)const_blk, f3_args, 1, NULL, DIAG_F3_TRACE_STANDARD, version, parameter);
#endif /* FEATURE_SAVE_DEBUG_TRACE */

  if( drain_check == TRUE )
  {
    micro_diagbuffer_drain_check();
  }
  
}   /* micro_msg_v2_send_1_ext() */


/*===========================================================================

FUNCTION MICRO_MSG_SEND_2_EXT

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with no arguments.

  Do not call directly; use macro MICRO_MSG_* defined in micro_msg_diag_service.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
micro_msg_send_2_ext(const msg_const_type *const_blk, uint32 version, void *parameter,
                       uint32 xx_arg1, uint32 xx_arg2)
{
#if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type f3_args[2];
#endif /* FEATURE_SAVE_DEBUG_TRACE */

  if(msg_enabled_mask != 0 && msg_service_init == TRUE)
  {
    /*write the msg and write the msg into diag_buffer */
    if (micro_msg_mask_enabled(&const_blk->desc))
    {
	  if(micro_diagmpd_buf)
	  {
		_micro_diagbuffer_debug_msg(micro_diagmpd_buf->handle, const_blk, 2, version, parameter, xx_arg1, xx_arg2);  
	  }
	  
      if ( micro_alloc_count_msg == MAX_VALUE_UINT32 )
      {
        MICRO_MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Variable micro_alloc_count_msg has reached maximum value.");
      }
      else
      {
        micro_alloc_count_msg++;
      }
    }
  }
#if defined (FEATURE_SAVE_DEBUG_TRACE)
   f3_args[0] = xx_arg1;
   f3_args[1] = xx_arg2;
   micro_msg_save_trace((void *)const_blk, f3_args, 2, NULL, DIAG_F3_TRACE_STANDARD, version, parameter);
#endif /* FEATURE_SAVE_DEBUG_TRACE */

}   /* micro_msg_send_2_ext() */

/*===========================================================================

FUNCTION MICRO_MSG_V2_SEND_2_EXT

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with no arguments.

  Do not call directly; use MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
micro_msg_v2_send_2_ext(const msg_v2_const_type *const_blk, uint32 version, void *parameter,
                       uint32 xx_arg1, uint32 xx_arg2)
{
#if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type f3_args[2];
#endif /* FEATURE_SAVE_DEBUG_TRACE */
  boolean drain_check = FALSE;
  
  if(msg_enabled_mask != 0 && msg_service_init == TRUE)
  {
    /*write the msg and write the msg into diag_buffer */
    if (micro_msg_mask_enabled(&const_blk->desc))
    {
      if(micro_diagmpd_buf)
	  {
	     _micro_diagbuffer_v2_debug_msg(micro_diagmpd_buf->handle, const_blk, 2, version, parameter, xx_arg1, xx_arg2);
      }
      if ( micro_alloc_count_msg == MAX_VALUE_UINT32 )
      {
        MICRO_MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Variable micro_alloc_count_msg has reached maximum value.");
      }
      else
      {
        micro_alloc_count_msg++;
      }
      drain_check = TRUE;
    }
  }
  
#if defined (FEATURE_SAVE_DEBUG_TRACE)
   f3_args[0] = xx_arg1;
   f3_args[1] = xx_arg2;
   micro_msg_save_trace((void *)const_blk, f3_args, 2, NULL, DIAG_F3_TRACE_STANDARD, version, parameter);
#endif /* FEATURE_SAVE_DEBUG_TRACE */

  if( drain_check == TRUE )
  {
    micro_diagbuffer_drain_check();
  }
  
}   /* micro_msg_v2_send_2_ext() */


/*===========================================================================

FUNCTION MICRO_MSG_SEND_3_EXT

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with no arguments.

  Do not call directly; use macro MICRO_MSG_* defined in micro_msg_diag_service.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
micro_msg_send_3_ext (const msg_const_type *const_blk, uint32 version, void *parameter,
                       uint32 xx_arg1, uint32 xx_arg2, uint32 xx_arg3)
{
#if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type f3_args[3];
#endif /* FEATURE_SAVE_DEBUG_TRACE */

  if(msg_enabled_mask != 0 && msg_service_init == TRUE)
  {
    /*write the msg and write the msg into diag_buffer */
    if (micro_msg_mask_enabled(&const_blk->desc))
    {
	  if(micro_diagmpd_buf)
	  {
		_micro_diagbuffer_debug_msg(micro_diagmpd_buf->handle, const_blk, 3, version, parameter, xx_arg1, xx_arg2, xx_arg3);  
	  }
	  
      if ( micro_alloc_count_msg == MAX_VALUE_UINT32 )
      {
        MICRO_MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Variable micro_alloc_count_msg has reached maximum value.");
      }
      else
      {
        micro_alloc_count_msg++;
      }
    }
  }
#if defined (FEATURE_SAVE_DEBUG_TRACE)
   f3_args[0] = xx_arg1;
   f3_args[1] = xx_arg2;
   f3_args[2] = xx_arg3;
   micro_msg_save_trace((void *)const_blk, f3_args, 3, NULL, DIAG_F3_TRACE_STANDARD, version, parameter);
#endif /* FEATURE_SAVE_DEBUG_TRACE */

}   /* micro_msg_send_3_ext() */

/*===========================================================================

FUNCTION MICRO_MSG_V2_SEND_3_EXT

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with no arguments.

  Do not call directly; use MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
micro_msg_v2_send_3_ext (const msg_v2_const_type *const_blk, uint32 version, void *parameter,
                       uint32 xx_arg1, uint32 xx_arg2, uint32 xx_arg3)
{
#if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type f3_args[3];
#endif /* FEATURE_SAVE_DEBUG_TRACE */
  boolean drain_check = FALSE;
  
  if(msg_enabled_mask != 0 && msg_service_init == TRUE)
  {
    /*write the msg and write the msg into diag_buffer */
    if (micro_msg_mask_enabled(&const_blk->desc))
    {
      if(micro_diagmpd_buf)
	  {
	     _micro_diagbuffer_v2_debug_msg(micro_diagmpd_buf->handle, const_blk, 3, version, parameter, xx_arg1, xx_arg2, xx_arg3);
      }
      if ( micro_alloc_count_msg == MAX_VALUE_UINT32 )
      {
        MICRO_MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Variable micro_alloc_count_msg has reached maximum value.");
      }
      else
      {
        micro_alloc_count_msg++;
      }
      drain_check = TRUE;
    }
  }
  
#if defined (FEATURE_SAVE_DEBUG_TRACE)
   f3_args[0] = xx_arg1;
   f3_args[1] = xx_arg2;
   f3_args[2] = xx_arg3;
   micro_msg_save_trace((void *)const_blk, f3_args, 3, NULL, DIAG_F3_TRACE_STANDARD, version, parameter);
#endif /* FEATURE_SAVE_DEBUG_TRACE */

  if( drain_check == TRUE )
  {
    micro_diagbuffer_drain_check();
  }
  
}   /* micro_msg_v2_send_3_ext() */
/*===========================================================================

FUNCTION MICRO_MSG_SEND_VAR_EXT

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with no arguments.

  Do not call directly; use macro MICRO_MSG_* defined in micro_msg_diag_service.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
micro_msg_send_var_ext (const msg_const_type *const_blk, uint32 version, void *parameter, uint32 num_args, ...)
{
  int i = 0;
  int total_num_args = MIN(num_args, MAX_VAR_ARGS);
  va_list arg_list;     /* ptr to the variable argument list */
  uint32 args[total_num_args];
#if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type f3_args[total_num_args];  // Assuming at most MAX_VAR_ARGS arguments
#endif

  if (msg_enabled_mask == 0 || msg_service_init != TRUE)
  {
    return;
  }

  va_start(arg_list, num_args);
  /* Store arguments from variable list. */
  for (i = 0; i < total_num_args; i++)
  {
    args[i] = va_arg(arg_list, uint32);
  }
  va_end(arg_list);
  /*write the msg and write the msg into diag_buffer */
  if (micro_msg_mask_enabled(&const_blk->desc))
  {
    switch (total_num_args)
    {
    case 4:
	  if(micro_diagmpd_buf)
	  {
		_micro_diagbuffer_debug_msg(micro_diagmpd_buf->handle, const_blk, 4, version, parameter, args[0], args[1], args[2], args[3]);  
	  }
      break;
    case 5:
	  if(micro_diagmpd_buf)
	  {
		_micro_diagbuffer_debug_msg(micro_diagmpd_buf->handle, const_blk, 5, version, parameter, args[0], args[1], args[2], args[3], args[4]);  
	  }
      break;
    case 6:
	  if(micro_diagmpd_buf)
	  {
		_micro_diagbuffer_debug_msg(micro_diagmpd_buf->handle, const_blk, 6, version, parameter, args[0], args[1], args[2], args[3], args[4], args[5]);  
	  }
      break;
    case 7:
	  if(micro_diagmpd_buf)
	  {
		_micro_diagbuffer_debug_msg(micro_diagmpd_buf->handle, const_blk, 7, version, parameter, args[0], args[1], args[2], args[3], args[4], args[5], args[6]);  
	  }
      break;
    case 8:
	  if(micro_diagmpd_buf)
	  {
		_micro_diagbuffer_debug_msg(micro_diagmpd_buf->handle, const_blk, 8, version, parameter, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7]);  
	  }
      break;
    case 9:
	  if(micro_diagmpd_buf)
	  {
		_micro_diagbuffer_debug_msg(micro_diagmpd_buf->handle, const_blk, 9, version, parameter, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8]);  
	  }
      break;
    }

    if (micro_alloc_count_msg == MAX_VALUE_UINT32)
    {
      MICRO_MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Variable micro_alloc_count_msg has reached maximum value.");
    }
    else
    {
      micro_alloc_count_msg++;
    }
  }

#if defined (FEATURE_SAVE_DEBUG_TRACE)
  /* Store arguments from variable list. */
  for (i = 0; i < total_num_args; i++)
  {
    f3_args[i] = args[i];
  }

  micro_msg_save_trace((const void *)const_blk, f3_args, total_num_args, NULL, DIAG_F3_TRACE_STANDARD, version, parameter);
#endif /* FEATURE_SAVE_DEBUG_TRACE */

}   /* micro_msg_send_var_ext() */

/*===========================================================================

FUNCTION MICRO_MSG_V2_SEND_VAR_EXT

DESCRIPTION
  This will queues a debug message in terse format.  The message will
  be dequeued by the DIAG task and sent in one of the following formats,
  depending on operating mode:
    Terse format (as is)
    Extended Message Format (DIAG_EXT_MSG_F)
    Legacy Message Format (DIAG_MSG_F)

  This functions enqueues such a message with no arguments.

  Do not call directly; use MSG_* defined in msg.h

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
void
micro_msg_v2_send_var_ext (const msg_v2_const_type *const_blk, uint32 version, void *parameter, uint32 num_args, ...)
{
  int i = 0;
  int total_num_args = MIN(num_args, MAX_VAR_ARGS);
  va_list arg_list;     /* ptr to the variable argument list */
  uint32 args[total_num_args];
  boolean drain_check = FALSE;
#if defined (FEATURE_SAVE_DEBUG_TRACE)
  msg_arg_type f3_args[total_num_args];  // Assuming at most MAX_VAR_ARGS arguments
#endif
  
  if (msg_enabled_mask == 0 || msg_service_init != TRUE)
  {
    return;
  }

  va_start(arg_list, num_args);
  /* Store arguments from variable list. */
  for (i = 0; i < total_num_args; i++)
  {
    args[i] = va_arg(arg_list, uint32);
  }
  va_end(arg_list);
  /*write the msg and write the msg into diag_buffer */
  if (micro_msg_mask_enabled(&const_blk->desc))
  {
    switch (total_num_args)
    {
    case 4:
      if(micro_diagmpd_buf)
	  {
	     _micro_diagbuffer_v2_debug_msg(micro_diagmpd_buf->handle, const_blk, 4, version, parameter, args[0], args[1], args[2], args[3]);
      }
      break;
    case 5:
      if(micro_diagmpd_buf)
	  {
	     _micro_diagbuffer_v2_debug_msg(micro_diagmpd_buf->handle, const_blk, 5, version, parameter, args[0], args[1], args[2], args[3], args[4]);
      }
      break;
    case 6:
      if(micro_diagmpd_buf)
	  {
	     _micro_diagbuffer_v2_debug_msg(micro_diagmpd_buf->handle, const_blk, 6, version, parameter, args[0], args[1], args[2], args[3], args[4], args[5]);
      }
      break;
    case 7:
      if(micro_diagmpd_buf)
	  {
	     _micro_diagbuffer_v2_debug_msg(micro_diagmpd_buf->handle, const_blk, 7, version, parameter, args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
      }
      break;
    case 8:
      if(micro_diagmpd_buf)
	  {
	     _micro_diagbuffer_v2_debug_msg(micro_diagmpd_buf->handle, const_blk, 8, version, parameter, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7]);
      }
      break;
    case 9:
      if(micro_diagmpd_buf)
	  {   
	     _micro_diagbuffer_v2_debug_msg(micro_diagmpd_buf->handle, const_blk, 9, version, parameter, args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8]);
      }
      break;
    }

    if (micro_alloc_count_msg == MAX_VALUE_UINT32)
    {
      MICRO_MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "Variable micro_alloc_count_msg has reached maximum value.");
    }
    else
    {
      micro_alloc_count_msg++;
    }
    drain_check = TRUE;
  }

#if defined (FEATURE_SAVE_DEBUG_TRACE)
  /* Store arguments from variable list. */
  for (i = 0; i < total_num_args; i++)
  {
    f3_args[i] = args[i];
  }

  micro_msg_save_trace((const void *)const_blk, f3_args, total_num_args, NULL, DIAG_F3_TRACE_STANDARD, version, parameter);
#endif /* FEATURE_SAVE_DEBUG_TRACE */

  if( drain_check == TRUE )
  {
    micro_diagbuffer_drain_check();
  }
}   /* micro_msg_v2_send_var_ext() */

#endif /*DIAG_UIMAGE_MODE*/
