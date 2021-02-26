/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         E R R O R     R E P O R T I N G    S E R V I C E S

GENERAL DESCRIPTION
  This module would prevent Q6 core from dropping into idle state once error 
  handling routine enters STM mode. It would keep the WDOG running even in 
  cases when mutex held by error callbacks blocks the exception handler 
  thread. It reuses gen cb ctxt to perform this operation

Copyright (c) 2015 - 2018 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/err/src/err_post_stm_force_wdog_bite.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/27/15   din     Adapted from err.c

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/


#include "erri.h"
#include "gen_cb_ctxt.h"

static volatile int always_true = 1;
static volatile int always_false = 0;
/*=========================================================================

FUNCTION err_post_stm_force_wdog_bite_gen_cb_ctxt_cb

DESCRIPTION
  Prevent Q6 core from dropping into idle state once error handling routine
  enters STM mode. It would keep the WDOG running even in 
  cases when mutex held by error callbacks blocks the exception handler 
  thread. It reuses gen cb ctxt to perform this operation. 
  gen_cb_ctxt thread priority is lowered and if the thread is scheduled 
  by any chance,  it is assumed that exception handler was stuck.
  The special context callback  cleans the cache and forces the wdog bite.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void err_post_stm_force_wdog_bite_gen_cb_ctxt_cb(void * arg)
{
  /* Clean Cache */
  err_mem_cache_clean();
  
  /* Force a HW WDOG Bite */
  dog_force_bite();
  
  while(always_true)
  {
    if(always_false)
     break;
  }
}


/*=========================================================================

FUNCTION err_post_stm_force_wdog_bite_err_cb

DESCRIPTION
  Prevent Q6 core from dropping into idle state once error handling routine
  enters STM mode. It would keep the WDOG running even in 
  cases when mutex held by error callbacks blocks the exception handler 
  thread. It reuses gen cb ctxt to perform this operation. 
  gen_cb_ctxt thread priority is lowered and if the thread is scheduled 
  by any chance,  it is assumed that exception handler was stuck.
  The special context callback  cleans the cache and forces the wdog bite.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void err_post_stm_force_wdog_bite_err_cb(void)
{
  gen_cb_ctxt_special_context(err_post_stm_force_wdog_bite_gen_cb_ctxt_cb, NULL);

} /* err_post_stm_force_wdog_bite_cb */


/*===========================================================================

FUNCTION ERR_POST_STM_FORCE_WDOG_BITE_INIT

DESCRIPTION
  Registers callback with error handler.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None .

===========================================================================*/

void err_post_stm_force_wdog_bite_init (void)
{
  err_cb_info_t cb_info;
  err_cb_error_t rv = ERRCB_E_FAILURE;

  cb_info.type = ERRCB_TYPE_VOID;
  cb_info.err_cb.cb = err_post_stm_force_wdog_bite_err_cb;
  cb_info.bucket = ERRCB_BUCKET_POST_STM;
  cb_info.order = ERRCB_ORDER_NORMAL;

  rv = err_cb_enable(&cb_info);
  if(ERRCB_E_SUCCESS != rv)
    TMS_MSG_ERROR_1("Registeration of err_post_stm_force_wdog_bite_cb failed, rv = %d", rv);
} /* err_post_stm_force_wdog_bite_init */




