/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         E R R O R     R E P O R T I N G    S E R V I C E S

GENERAL DESCRIPTION
  This module provides mpss related error reporting services for both 
  fatal and non-fatal errors.  This module is not a task, but rather 
  a set of callable procedures which run in the context of the calling 
  task.

Copyright (c) 2014 - 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/err/src/err_target_non_mpss.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/01/15   abh     Added changes for extended smem logging
11/19/14   din     Adapted from err.c

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "erri.h"
#include "err_decompress.h"

/*===========================================================================

                      Prototypes for internal functions

===========================================================================*/

/*===========================================================================

                      Prototypes for external functions

===========================================================================*/

extern void err_fatal_jettison_core (
  unsigned int line,       /* From __LINE__ */
  const char   *file_name, /* From __FILE__ */
  const char   *format,    /* format string */
  uint32 param1,
  uint32 param2,
  uint32 param3
);



/*===========================================================================

                 Defines and variable declarations for module

===========================================================================*/


/*===========================================================================

                              Function definitions

===========================================================================*/

/*===========================================================================

FUNCTION       ERR_TARGET_DECOMPRESS_AND_F3_TRACE

DESCRIPTION
  Decompresses the error fatal message and pushes it through F3 trace.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void err_target_update_coredump_and_f3_trace( err_fatal_params_type * params )
{
  err_fatal_jettison_core (params->msg_const_ptr->line,
                           params->msg_const_ptr->fname,
                           params->msg_const_ptr->fmt,
                           params->param1, 
                           params->param2, 
                           params->param3);
} /* err_target_update_coredump_and_f3_trace */

	
void err_put_log (word line, const char *file_ptr)
{
  //err logging is not supported in this build
}
