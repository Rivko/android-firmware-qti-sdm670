/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         E R R O R     R E P O R T I N G    S E R V I C E S

GENERAL DESCRIPTION
  This module provides error related functionality for user PD.

Copyright (c) 2015 - 2017 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        Edit History

$Header: //components/rel/core.qdsp6/2.1/debugtools/err/src/err_root.c#3 $

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
#include "err_decompress.h"
#include "tms_utils_msg.h"
#include "err_smem_log.h"

/*===========================================================================

                      Prototypes for internal functions

===========================================================================*/
void err_fatal_handler( void ) ;
void err_fatal_jettison_core (unsigned int line, const char *file_name,
  const char *format, uint32 param1, uint32 param2, uint32 param3);
void err_mem_cache_clean(void);
void err_invoke_user_process_cbs(err_cb_bucket_t);
void err_execute_external_cb_post_STM(void);

/*===========================================================================

                      Prototypes for external functions

===========================================================================*/
extern void err_target_update_coredump_and_f3_trace( err_fatal_params_type * params );
extern void err_write_timestamp_string(void);
extern void err_log_ssr_init(void);
extern void err_qdi_invoke_all_user_process_cbs(err_cb_bucket_t);
extern void dog_force_kick( void );
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
  /* Initialize err_log_ssr_init */
  err_log_ssr_init();

  /* Initialize the smem buffer for writing extended log info */
  err_smem_log_init();
  
  /* Write the timestamp string into the smem buffer (only from Root PD).
     * This API should always be called before writing any other info to the
     * SMEM buffer. Since the smem parser tool depends on the below string
     * to locate the err smem buffer.
     */
  err_write_timestamp_string();

  /* Write the updated image versioning information to smem buffer (only from Root PD) */
  err_smem_log_image_version();

  err_fatal_params.err_services_init_complete = TRUE;
  
  TMS_MSG_HIGH("Err service init");

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
  qurt_mem_cache_clean(0,0, QURT_MEM_CACHE_FLUSH_ALL, QURT_MEM_DCACHE);

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
  err_invoke_user_process_cbs(ERRCB_BUCKET_POST_STM);

  ERR_CRUMB_TRAIL_BMSK(ERR_CRUMB_TRAIL_BMSK_COREDUMP_UPDATED_PRE);

  /* Decompress and f3 trace */
  err_target_update_coredump_and_f3_trace(&err_fatal_params);

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
   ERR_FATAL_ENTER_SINGLE_THREADED_MODE();
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
  err_qdi_invoke_all_user_process_cbs(bucket);
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
  err_execute_external_cb_post_STM_helper();
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
  err_execute_external_cb_normal_helper();
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
  err_execute_external_cb_late_helper();
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
  err_execute_external_cb_last_helper();
}