/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         E R R O R     R E P O R T I N G    S E R V I C E S

GENERAL DESCRIPTION
  This module logs SSR failure reason into SMEM buffer. It is compiled only 
  for root PD.

Copyright (c) 2015 - 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        Edit History

$Header: //components/rel/core.qdsp6/2.1/debugtools/err/src/err_log_ssr.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/27/15   din     Adapted from err.c

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/


#include "erri.h"
#include <stdlib.h>
#include <stringl.h>
#include "sys_m_internal.h"
#include "tms_utils.h"


/*===========================================================================

                      Prototypes for internal functions

===========================================================================*/
static void err_log_ssr_failure_reason(void);

/*===========================================================================

                      Data decalrations

===========================================================================*/

/* SFR (Subsystem restart Failure Reason) decl's */
char* err_ssr_smem_buf_ptr= NULL;
static const char err_sfr_init_string[]="SFR Init: wdog or kernel error suspected.";
boolean err_sfr_locked = FALSE;

/*===========================================================================

                      Extern Data decalrations

===========================================================================*/
extern coredump_type coredump;


/*===========================================================================

FUNCTION ERR_LOG_SSR_INIT

DESCRIPTION
  This function initializes ssr logging related data structures

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None .

===========================================================================*/

void err_log_ssr_init (void)
{
  err_cb_info_t cb_info;
  err_cb_error_t rv = ERRCB_E_FAILURE;

  err_ssr_smem_buf_ptr = (char * )sys_m_init_sfr_buffer();
  if (NULL != err_ssr_smem_buf_ptr)
  {
    strlcpy (err_ssr_smem_buf_ptr, err_sfr_init_string, 
	      ERR_SSR_REASON_SIZE_BYTES);
  }
  
  /* SSR failure reason callback registeration with error handling */
  cb_info.type = ERRCB_TYPE_VOID;
  cb_info.err_cb.cb = err_log_ssr_failure_reason;
  cb_info.bucket = ERRCB_BUCKET_NORMAL;
  cb_info.order = ERRCB_ORDER_NORMAL;

  rv = err_cb_enable(&cb_info);
  if(ERRCB_E_SUCCESS != rv)
    TMS_MSG_ERROR_1("Registeration of ssr failure reason cb failed, rv = %d", rv);
} /* err_log_ssr_init */


/*=========================================================================

FUNCTION err_log_ssr_failure_reason

DESCRIPTION
  Used to log a minimal set of failure reason to smem.  Primarily to assist
  locating faulting subsystem in many-subsystem architectures.

DEPENDENCIES
  smem

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void err_log_ssr_failure_reason(void)
{
  ERR_CRUMB_TRAIL_BMSK(ERR_CRUMB_TRAIL_BMSK_LOG_SSR_PRE);

  if (!err_sfr_locked)
  {
    word sfr_line = coredump.err.linenum;
    const char *sfr_file_ptr = coredump.err.filename;
    const char *sfr_msg_ptr = coredump.err.message;
    uint32 sfr_p0 = coredump.err.param[0];
    uint32 sfr_p1 = coredump.err.param[1];
    uint32 sfr_p2 = coredump.err.param[2];
    char *sfr_buf_ptr = err_ssr_smem_buf_ptr;
    int sfr_written = 0;

    /* If aux_msg is populated, use it for SFR */
    if ( coredump.err.aux_msg[0] != 0 )
    {
      sfr_msg_ptr = coredump.err.aux_msg;
    }

    if (sfr_buf_ptr && sfr_file_ptr)
    {
      /* Write "__MODULE__:__LINE:" */
      sfr_written = tms_utils_fmt(sfr_buf_ptr, ERR_SSR_REASON_SIZE_BYTES, 
		                     "%s:%d:", sfr_file_ptr, sfr_line);

      if ((sfr_written > 0) && sfr_msg_ptr)
      {
        /* Append err fatal message */
        if(sfr_written > TMS_UTILS_BUF_SZ_ZERO){
           sfr_buf_ptr += (sfr_written-1); 
	   /*-1 since tms_utils_fmt() API returns written length with NULL 
	    * charectar increment and overwrite previous null-term*/
        }
        sfr_written += tms_utils_fmt_nostring(sfr_buf_ptr, 
			(ERR_SSR_REASON_SIZE_BYTES-sfr_written), 
			 sfr_msg_ptr, sfr_p0, sfr_p1, sfr_p2);
      }
      /* Commit the write before proceeding */
      ERR_MEMORY_BARRIER();

      err_sfr_locked = TRUE;
    } /* (sfr_buf_ptr && sfr_file_ptr) */
  } /* (!err_sfr_locked) */

} /* err_log_ssr_failure_reason */


