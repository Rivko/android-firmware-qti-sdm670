/** vi: tw=128 ts=3 sw=3 et :
@file rcinit_ex_policy.c
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
$Header: //components/rel/core.qdsp6/2.1/debugtools/example/src/rcinit_ex_policy.c#1 $
$DateTime: 2017/07/21 22:10:47 $
$Change: 13907704 $
$Author: pwbldsvc $
===============================================================================*/

#include "rcinit.h"
#include "string.h"

/*===========================================================================

 FUNCTION rcinit_example_policyfn

 DESCRIPTION
 example policy function

 DEPENDENCIES
 tech provided dependency analysis must be performed such that all tasks
 or services requried by this policy function are started before its call.

 RETURN VALUE
 policy name from provided list

 SIDE EFFECTS
 system alteration at next group if policy function decision returns
 any policy name on the provided list. the poilcy function can utilize
 any task or service as a dependency *IN SO MUCH AS* the prerequsite
 dependency has already been started.

 ===========================================================================*/

// this function has no implementation; it is merely an example of the syntax
// and return result required from the tech area that provides the policy
// decision logic.

RCINIT_NAME rcinit_example_policyfn(RCINIT_NAME* list)
{
   while (RCINIT_NULL != *list)                                                  // list is terminated by RCINIT_NULL
   {
      // if (0 == strcmp("ftm", *list))
      // {
      //  check if ftm mode would be enabled, query NV
      //  break; exit loop to transition
      // }
      // else if (0 == strcmp("alpha", *list))
      // {
      //  check if mode would be enabled, arbitrary decision
      //  break; exit loop to transition
      // }
      // else if (0 == strcmp("beta", *list))
      // {
      //  check if mode would be enabled, arbitrary decision
      //  break; exit loop to transition
      // }

      list++;                                                                    // iterate for next policy tag
   }

   return *list;                                                                 // policy_tag or RCINIT_NULL
}
