/*=============================================================================

                UTimer_Client_V.h

GENERAL DESCRIPTION
      UTimer Client Internal Header file

EXTERNAL FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

      Copyright (c) 2009 - 2017
      by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.qdsp6/2.1.c4/services/utimers/src/utimer_client_v.h#1 $ 
$DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
6/9/14     ab      Add file that provides MultiPD changes
=============================================================================*/

#define UTIMER_CLIENT_PRI 19   /* Todo: Talk to Qurt team, what should be priority of this callback thread.. can it be 10? */
#define UTIMER_CLIENT_STACK_SIZE 1792

#ifdef FEATURE_UTIMER_CLIENT_TEST
#define UTIMER_TEST_PRI 100 
#define UTIMER_CLIENT_TEST_STACK_SIZE 2048
#endif /* FEATURE_UTIMER_CLIENT_TEST */

