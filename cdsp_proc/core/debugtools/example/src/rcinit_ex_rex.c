/** vi: tw=128 ts=3 sw=3 et :
@file rcinit_ex_rex.c
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
$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/example/src/rcinit_ex_rex.c#1 $
$DateTime: 2018/07/30 01:21:36 $
$Change: 16759227 $
$Author: pwbldsvc $
===============================================================================*/

#include "err.h"
#include "rcinit_rex.h"

/*===========================================================================

 FUNCTION rcinit_example_rextask

 DESCRIPTION
 example task boiler plate

 DEPENDENCIES
 none

 RETURN VALUE
 operation success

 SIDE EFFECTS
 none
 ===========================================================================*/

#define RCINIT_EXAMPLE_REXTASK_MASK0   0x00000001
#define RCINIT_EXAMPLE_REXTASK_MASK1   0x00000002
#define RCINIT_EXAMPLE_REXTASK_MASKS   (RCINIT_EXAMPLE_REXTASK_MASK0|RCINIT_EXAMPLE_REXTASK_MASK1)

/* static */ struct                                                              // clients do not need to access this, expose the communications infrastructure by API
{
   rex_tcb_type* myself;

} rcinit_example_rextask_events;

void rcinit_example_rextask_client_signal_event0(void)                           // called by other task
{
   if (NULL != rcinit_example_rextask_events.myself)
   {
      ERR_FATAL("event trigger", 0, 0, 0);
   }
   else
   {
      rex_set_sigs(rcinit_example_rextask_events.myself, RCINIT_EXAMPLE_REXTASK_MASK0);
   }
}

void rcinit_example_rextask_client_signal_event1(void)                           // called by other task
{
   if (NULL != rcinit_example_rextask_events.myself)
   {
      ERR_FATAL("event trigger", 0, 0, 0);
   }
   else
   {
      rex_set_sigs(rcinit_example_rextask_events.myself, RCINIT_EXAMPLE_REXTASK_MASK1);
   }
}

void rcinit_example_rextask(unsigned long arg_p)
{
   // Cache rex_self() for use in client API calls; indicates this task is available
   rcinit_example_rextask_events.myself = rex_self();

   rcinit_handshake_startup();                                                   // *REQUIRED*

   for (;;)
   {
      unsigned long mask = rex_wait(RCINIT_EXAMPLE_REXTASK_MASKS);

      if (RCINIT_EXAMPLE_REXTASK_MASK0 & mask)
      {
         rex_clr_sigs(rcinit_example_rextask_events.myself, RCINIT_EXAMPLE_REXTASK_MASK0);
         mask &= ~RCINIT_EXAMPLE_REXTASK_MASK0;
      }

      if (RCINIT_EXAMPLE_REXTASK_MASK1 & mask)
      {
         rex_clr_sigs(rcinit_example_rextask_events.myself, RCINIT_EXAMPLE_REXTASK_MASK1);
         mask &= ~RCINIT_EXAMPLE_REXTASK_MASK1;
      }

      if (0 != mask)
      {
         ERR_FATAL("unknown event mask", 0, 0, 0);
      }
   }

   /* NOTREACHED */

   rcinit_example_rextask_events.myself = NULL;

   if (arg_p) {;}
}
