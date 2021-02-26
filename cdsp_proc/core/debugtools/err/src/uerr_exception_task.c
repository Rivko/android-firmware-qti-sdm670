/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    ISLAND ERR EXCEPTION TASK MODULE

GENERAL DESCRIPTION
  This module contains the task definition for island  exception handler.

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2014 - 2016 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.qdsp6/2.1.c4/debugtools/err/src/uerr_exception_task.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/17/14   din     Reduced uErr task stack size to 1024 bytes.
06/13/14   din     Moved uErr task to TCM using qurt apis.
04/01/14   din     Initial verion (pulled code from err_exception_task.c)
===========================================================================*/
#include "qurt.h"
#include "erri.h"

#define dwMaxNumEvents                          2                                // DALSys Worker Loop Event Queue Depth for DALTASK
#define wlPriority                              190                              // SHARED_DRIVER_SERVICE_PRI_ORDER

void uerr_exception_handler
(
  void *                           ignored    
);                                           

/*===========================================================================
FUNCTION ubootstrap_exception_handler

DESCRIPTION
  Create a context for uerr_exception_handler when main() is not available.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

extern uint8 uerr_task_stack_arr[UERR_TASK_STACK_SIZE];
extern qurt_thread_t       uerrr_task_thread_id;

void ubootstrap_exception_handler(void)
{
   qurt_thread_attr_t tattr;
   unsigned int stackbase;
  
   stackbase = (unsigned int)&uerr_task_stack_arr;
   qurt_thread_attr_init (&tattr);
   qurt_thread_attr_set_stack_size (&tattr, (UERR_TASK_STACK_SIZE -8));
   qurt_thread_attr_set_stack_addr (&tattr, (void*)((stackbase + 7) & (~7)) );
   qurt_thread_attr_set_priority (&tattr, wlPriority);
   qurt_thread_attr_set_tcb_partition(&tattr, 1); // This task should reside in TCM Memory
   qurt_thread_attr_set_name(&tattr, "uerr_ex");
   (void)qurt_thread_create(&uerrr_task_thread_id, &tattr, uerr_exception_handler , NULL);
   
}

