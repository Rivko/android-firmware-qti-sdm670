/*!
  @file
  diagstartup.c

  @brief
  Start the diag task.
*/

/*===========================================================================

  Copyright (c) 2009, 2013-14 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1/services/diag/platform/adsp/src/diagtask.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/28/14   ph      Added start signal to be used for handshake with osal
                   in the new task created using osal_create_thread().
12/23/13   ph      Included header files required to account for removal of ASSERT macro
04/22/13   ph      Reduced DIAG_STACK_SIZ to 4 KB.
12/18/09   JV      Calculating the adress is doen in the OSAL
12/03/09   JV      Switching over to QURT APIs in OSAL and cleanup
06/01/09   JV      Made the 'DIAG_STACK_SIZ' consistent with how much we are allocating.
04/21/08   av      Intial creation.
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "diag.h"
#include "diagi_v.h"
#include "osal.h"
#include "diagstub.h"

#include "stdio.h"

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/*!
** Variables for the DIAG task
*/

/*Changed this to 512 so that 512 *sizeof(unsigned long long) gets to 4 KB */
#define DIAG_STACK_SIZ (512)

unsigned long long diag_stack[DIAG_STACK_SIZ];

osal_tcb_t diag_tcb;
osal_thread_attr_t diag_attr;

/*===========================================================================

  FUNCTION:  diagstart

===========================================================================*/
/*!
    @brief
    Starts the Diag Thread.

    @detail
    The following steps are involved in initialization:
    - Create and start a thread for DIAG
    - Send the Thread Startup Signal

    @return
    None
*/
/*=========================================================================*/
void diagstart(void)
{

    /* diag_stack is of type unsigned long long */
    diag_attr.stack_size = DIAG_STACK_SIZ * sizeof(unsigned long long);
    diag_attr.start_func = diag_task;
    diag_attr.arg = NULL;
    diag_attr.name = "diag_task";
    diag_attr.priority = DIAG_PRI;
    /* Need to pass the initial address of the stack, which grows upwards */
    diag_attr.stack_address = diag_stack;

    int diag_pri_array[32] = {12,4,14,15,16,17,-1,6,18,19,10,9,7,1,0,13,3,2,20,11,-1,5,21,-1,8,22,-1,-1,-1,-1,-1,23};



    if ((osal_create_thread(&diag_tcb, &diag_attr, diag_pri_array)) != OSAL_SUCCESS)
    {
        fprintf(stderr," diagstart: create_thread failed \n");
    }
}

