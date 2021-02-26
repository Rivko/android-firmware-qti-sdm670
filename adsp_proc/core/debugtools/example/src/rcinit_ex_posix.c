/** vi: tw=128 ts=3 sw=3 et :
@file rcinit_ex_posix.c
@brief This file contains the API for the Run Control Framework, API 3.0
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.qdsp6/2.1/debugtools/example/src/rcinit_ex_posix.c#1 $
$DateTime: 2017/07/21 22:10:47 $
$Change: 13907704 $
$Author: pwbldsvc $
===============================================================================*/

#include "err.h"
#include "rcinit_posix.h"

/*===========================================================================

 FUNCTION rcinit_example_posix

 DESCRIPTION
 example task boiler plate

 DEPENDENCIES
 none

 RETURN VALUE
 operation success

 SIDE EFFECTS
 none
 ===========================================================================*/

/* static */ struct                                                              // clients do not need to access this, expose the communications infrastructure by API
{
   boolean init;
   pthread_cond_t cond;
   pthread_mutex_t mutex;

} rcinit_example_posix_events;

void rcinit_example_posix_client_signal(void)                                    // called by other task
{
   if (TRUE != rcinit_example_posix_events.init)
   {
      ERR_FATAL("event trigger", 0, 0, 0);
   }
   else
   {
      pthread_mutex_lock(&rcinit_example_posix_events.mutex);
      pthread_cond_signal(&rcinit_example_posix_events.cond);
      pthread_mutex_unlock(&rcinit_example_posix_events.mutex);
   }
}

void rcinit_example_posix(void* arg_p)
{
   pthread_mutexattr_t attr_mutex;
   pthread_condattr_t attr_cond;

   pthread_mutexattr_init(&attr_mutex);
   pthread_mutex_init(&rcinit_example_posix_events.mutex, &attr_mutex);
   pthread_mutexattr_destroy(&attr_mutex);

   pthread_condattr_init(&attr_cond);
   pthread_cond_init(&rcinit_example_posix_events.cond, &attr_cond);
   pthread_condattr_destroy(&attr_cond);

   rcinit_example_posix_events.init = TRUE;

   rcinit_handshake_startup();                                                   // *REQUIRED*

   for (;;)
   {
      pthread_mutex_lock(&rcinit_example_posix_events.mutex);
      pthread_cond_wait(&rcinit_example_posix_events.cond, &rcinit_example_posix_events.mutex);
      pthread_mutex_unlock(&rcinit_example_posix_events.mutex);

      // ...
   }

   /* NOTREACHED */

   if (arg_p) {;}
}
