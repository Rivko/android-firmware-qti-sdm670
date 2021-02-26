/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         E R R O R     R E P O R T I N G    S E R V I C E S

GENERAL DESCRIPTION
  This module provides error related functionality for root PD.

Copyright (c) 2015 - 2017 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        Edit History

$Header: //components/rel/core.qdsp6/2.1/debugtools/err/src/err_user.c#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/27/15   din     Adapted from err.c

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "qurt.h"
#include "erri.h"
#include <stdlib.h>
#include <stringl.h>
#include "err_qdi_client.h"
#include "err_smem_log.h"
#include "tms_utils.h"
#include "procinfo_qdi.h"

/*===========================================================================

                      Prototypes for internal functions

===========================================================================*/
void err_fatal_handler( void ) ;
void err_fatal_jettison_core (unsigned int line, const char *file_name,
  const char *format, uint32 param1, uint32 param2, uint32 param3);
void err_mem_cache_clean(void);
void err_execute_external_cb_post_STM(void);

/*===========================================================================

                      Prototypes for external functions

===========================================================================*/
extern void err_fatal_jettison_core (unsigned int line, const char *file_name,
  const char *format, uint32 param1, uint32 param2, uint32 param3);
extern void dog_force_kick( void );
extern void err_execute_external_cb_pre_STM(uint32);
extern void err_execute_external_cb_post_STM_helper(void);
extern void err_execute_external_cb_normal_helper(void);
extern void err_execute_external_cb_late_helper(void);
extern void err_execute_external_cb_last_helper(void);

/*===========================================================================

                      External Data Vairables

===========================================================================*/

extern err_fatal_params_type err_fatal_params;

/* Struct used to hold coredump data */
extern coredump_type coredump;


/*===========================================================================

                 Defines and variable declarations for module

===========================================================================*/

#define ERR_TASK_STACK_SIZE 2048
typedef struct
{
  void *             stack;
} err_user_task_t;

err_user_task_t err_user_task;

 
/*===========================================================================

                              Function definitions

===========================================================================*/

/*===========================================================================

FUNCTION ERR_PD_INIT

DESCRIPTION
  This function initializes error services and calls into target & process 
  based error intializing routine.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  For Root PD on MPSS, Error log is loaded from NVM into RAM resident cache.

===========================================================================*/

DLL_API_GLOBAL void err_pd_init (void)
{
  /* Initialize the extended smem logging buffer */
  err_smem_log_init();

  /* Calculate the offset from which smem log needs to be written.
     * (only for User PD)
     */
  err_smem_calculate_offset();

  TMS_MSG_HIGH("Err service init");
  
  err_fatal_params.err_services_init_complete = TRUE;
} /* err_pd_init */

/*===========================================================================

FUNCTION       err_mem_cache_clean

DESCRIPTION
  Flushes D-cache for root PD only

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void err_mem_cache_clean(void)
{
 
} /* err_mem_cache_clean*/

/*===========================================================================

FUNCTION       err_fatal_post_exception_processing

DESCRIPTION
  This is called from exception handler after error fatal raises an 
  exception

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void err_fatal_post_exception_processing(void)
{
  char pfr_str[ERR_LOG_MAX_MSG_LEN] ={0};
  char pname[ERR_PDNAME_MAX_LEN] = {0};
  int written = 0;

  /* Copy trace log from island mode data structure */
  coredump.err.crumb_trail_bmsk |= err_fatal_params.crumb_trail_bmsk; 
  
  /* Copy compressed prt to coredump */
  coredump.err.compressed_ptr = err_fatal_params.msg_const_ptr;

  /* Copy register content */
  memscpy( coredump.arch.regs.array, SIZEOF_ALLOCATED_COREDUMP_REG,
             err_fatal_params.array,  SIZEOF_ARCH_COREDUMP_REGISTERS * sizeof(uint32) );

  ERR_CRUMB_TRAIL_BMSK(ERR_CRUMB_TRAIL_BMSK_CACHCE_CLEAN_PRE);

  /* Clean Cache */
  err_mem_cache_clean();

  err_execute_external_cb_post_STM();
  
  ERR_CRUMB_TRAIL_BMSK(ERR_CRUMB_TRAIL_BMSK_COREDUMP_UPDATED_PRE);

  if ( err_fatal_params.msg_const_ptr->fmt != NULL )
  {
    if (PROCINFO_QDI_FAILURE == procinfo_client_get_name(PROCINFO_OEM_NAME_TYPE, pname, sizeof(pname)))
    {
      /* Ignore return value: pname will revert to qurt provided name on failure */
      procinfo_client_get_name(PROCINFO_INTERNAL_NAME_TYPE, pname, sizeof(pname));
    }

    written = tms_utils_fmt( pfr_str, ERR_LOG_MAX_MSG_LEN, "EF:%s:%x:%s:%d:", 
                         pname,
                         qurt_process_get_id(),
		         err_fatal_params.msg_const_ptr->fname, 
			 err_fatal_params.msg_const_ptr->line); 

    if (written > 0 && written < ERR_LOG_MAX_MSG_LEN )
    {
      /* Append err fatal message */
      /*-1 since tms_utils_fmt() API returns written length with NULL 
       * character increment and overwrite previous null-term*/
      
      written += tms_utils_fmt_nostring(pfr_str + written - 1, 
			(ERR_LOG_MAX_MSG_LEN- (written-1)), 
			 err_fatal_params.msg_const_ptr->fmt,
			 err_fatal_params.param1,
			 err_fatal_params.param2,
			 err_fatal_params.param3);
    }
  }

  /* Inform PFR reason to root PD */
  err_qdi_invoke_inform_pfr( strnlen(pfr_str, 
                             ERR_LOG_MAX_MSG_LEN), pfr_str);

  err_fatal_jettison_core (err_fatal_params.msg_const_ptr->line,
                           err_fatal_params.msg_const_ptr->fname,
                           err_fatal_params.msg_const_ptr->fmt,
                           err_fatal_params.param1, 
                           err_fatal_params.param2, 
                           err_fatal_params.param3);

} /* err_fatal_post_exception_processing */

/*===========================================================================

FUNCTION       err_enter_stm_mode

DESCRIPTION
  Halt other HW threads for Root process or goto into sw freeze for user process

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void err_enter_stm_mode(void)
{
   
}

/*===========================================================================

FUNCTION       err_invoke_user_process_cbs

DESCRIPTION
  Invokes user Process error callbacks during root PD error. Empty function for 
  user process.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void err_invoke_user_process_cbs(err_cb_bucket_t bucket)
{

}

/*===========================================================================

FUNCTION       err_execute_external_cb_pre_STM_helper

DESCRIPTION
  Abstraction for err_execute_external_cb_pre_STM API

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void err_execute_external_cb_pre_STM_helper(void)
{
  /* passing always 0 since QuRT does not have support to pass
    argument with callback at QDI layer, and we need to call
    user pre STM CBs while root pd crash */
  err_execute_external_cb_pre_STM(0);
}

/*===========================================================================

FUNCTION       err_execute_external_cb_post_STM

DESCRIPTION
  Abstraction for err_execute_external_cb_post_STM_helper API

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void err_execute_external_cb_post_STM(void)
{

}

/*===========================================================================

FUNCTION       err_execute_external_cb_normal

DESCRIPTION
  Abstraction for err_execute_external_cb_normal_helper API

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void err_execute_external_cb_normal(void)
{

}

/*===========================================================================

FUNCTION       err_execute_external_cb_late

DESCRIPTION
  Abstraction for err_execute_external_cb_late_helper API

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void err_execute_external_cb_late(void)
{

}

/*===========================================================================

FUNCTION       err_execute_external_cb_last

DESCRIPTION
  Abstraction for err_execute_external_cb_last_helper API

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void err_execute_external_cb_last(void)
{

}

void err_put_log (word line, const char *file_ptr)
{
  //err logging is not supported in this build
}

/*=========================================================================

FUNCTION err_user_spin_callback_context

DESCRIPTION
  Allocates stack for user PD callback PD execution

DEPENDENCIES
  None

RETURN VALUE
  TRUE if successfull
  FALSE if failed

SIDE EFFECTS
  None

===========================================================================*/

boolean err_user_spin_callback_context(void )
{
  if( err_user_task.stack  != NULL )
     return TRUE;
   
   err_user_task.stack = malloc(ERR_TASK_STACK_SIZE);
   if (NULL == err_user_task.stack)
   {
     return FALSE;
   }
  
   return TRUE;
} /* err_user_spin_callback_context */ 

void err_user_register_with_root(void)
{
  if ( !err_user_spin_callback_context() )
  {
    return;
  }

  err_qdi_invoke_communicate_user_cb( err_execute_external_cb_pre_STM_helper,
                                      err_execute_external_cb_post_STM_helper,
                                      err_execute_external_cb_normal_helper,
                                      err_execute_external_cb_late_helper,
                                      err_execute_external_cb_last_helper,
                                      err_user_task.stack,
                                      ERR_TASK_STACK_SIZE);

}

/*=========================================================================

FUNCTION err_set_image_version_info

DESCRIPTION
  Error set image versioning information. This is enabled only for user PD. 
  Should be only called after err_init is called.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

DLL_API_GLOBAL void err_set_image_version_info
( 
  char * arg_QC_IMAGE_VERSION_STRING_AUTO_UPDATED,
  char * arg_IMAGE_VARIANT_STRING_AUTO_UPDATED,
  char * arg_OEM_IMAGE_VERSION_STRING_AUTO_UPDATED,
  char * arg_OEM_IMAGE_UUID_STRING_AUTO_UPDATED
)
{
  strlcpy(coredump.image.qc_image_version_string, 
          arg_QC_IMAGE_VERSION_STRING_AUTO_UPDATED, ERR_IMAGE_VERSION_SIZE); 
  strlcpy(coredump.image.image_variant_string, 
          arg_IMAGE_VARIANT_STRING_AUTO_UPDATED, ERR_IMAGE_VERSION_SIZE);
  strlcpy(coredump.image.oem_image_version_string, 
          arg_OEM_IMAGE_VERSION_STRING_AUTO_UPDATED,ERR_IMAGE_VERSION_SIZE);
  strlcpy(coredump.image.oem_image_uuid_string,
          arg_OEM_IMAGE_UUID_STRING_AUTO_UPDATED, ERR_IMAGE_VERSION_SIZE);  
}
