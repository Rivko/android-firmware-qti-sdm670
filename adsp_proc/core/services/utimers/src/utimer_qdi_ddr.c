/*=============================================================================

                 utimer_qdi_ddr.c

GENERAL DESCRIPTION
   Implements QDI layer for UTimers that goes into main image

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

      Copyright (c) 2010 - 2014
   by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.
 
$Header: //components/rel/core.qdsp6/2.1/services/utimers/src/utimer_qdi_ddr.c#1 $ 
$DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
06/09/12   abalanag   File created.

=============================================================================*/


/*****************************************************************************/
/*                           INCLUDE FILES                                   */
/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stringl/stringl.h>
#include "qurt.h"
#include "qurt_qdi.h"
#include "qurt_qdi_driver.h"
#include "qurt_rmutex.h"
#include "utimer.h"
#include "utimer_v.h"
#include "utimer_qdi_v.h"
#include "err.h"

/*===========================================================================
Data Structures
===========================================================================*/

extern utimer_qdi_opener utimer_qdi_clients[];
extern uint32 utimer_qdi_clients_cntr;

/* Guest OS doesnt require pipe */
extern utimer_qdi_pipe_type utimer_qdi_pipes[];

extern qurt_mutex_t utimer_qdi_mutex;
/*MACRO for locking/unlocking the qmutex*/
#define UTIMER_QDI_LOCK()  \
   qurt_rmutex_lock(&utimer_qdi_mutex); \
  
#define UTIMER_QDI_FREE()  \
   qurt_rmutex_unlock(&utimer_qdi_mutex); \

/* extern variables */
/* Pointer to the mutex object for protecting critical-section of timer 
   code */
extern qurt_mutex_t utimer_qemu_mutex;
extern void utimer_process_active_timers(void);

/*MACRO for locking/unlocking the qmutex*/
#define UTIMER_LOCK()  \
   qurt_rmutex_lock(&utimer_qemu_mutex); \
  
#define UTIMER_FREE()  \
   qurt_rmutex_unlock(&utimer_qemu_mutex); \

/*===========================================================================
Declarations
===========================================================================*/
extern int utimer_qdi_invoke(int client_handle,
                          qurt_qdi_obj_t *pobj,
                          int method,
                          qurt_qdi_arg_t a1,
                          qurt_qdi_arg_t a2,
                          qurt_qdi_arg_t a3,
                          qurt_qdi_arg_t a4,
                          qurt_qdi_arg_t a5,
                          qurt_qdi_arg_t a6,
                          qurt_qdi_arg_t a7,
                          qurt_qdi_arg_t a8,
                          qurt_qdi_arg_t a9);
                          
extern void utimer_qdi_remote_handler(uint32 process_idx, utimer_ptr_type timer, utimer_notify_type cb_type,
                              utimer_osal_notify_obj_ptr sigs_func_addr, utimer_osal_notify_data sigs_mask_data,
                              utimer_timetick_type run_time_ms);

/*===========================================================================
Pipe Apis
===========================================================================*/
int utimer_qdi_create_pipe(utimer_qdi_opener *clntobj)
{
   qurt_pipe_attr_t pipe_attr;
   uint32 idx;
   int ret;
   
   for(idx=0; idx<UTIMER_MAX_PIPES && utimer_qdi_pipes[idx].allocated == TRUE; idx++);
   
   if(idx==UTIMER_MAX_PIPES)
   {
      ERR_FATAL("Max Pipes %d reached", idx, 0, 0);
      return -1;
   }
   
   //Create Pipe for the pid
   qurt_pipe_attr_init(&pipe_attr);
   qurt_pipe_attr_set_elements(&pipe_attr, UTIMER_CLIENT_PIPE_SIZE);
   qurt_pipe_attr_set_buffer(&pipe_attr, utimer_qdi_pipes[idx].buffer);
   qurt_pipe_attr_set_buffer_partition(&pipe_attr, QURT_PIPE_ATTR_MEM_PARTITION_TCM);
   ret = qurt_pipe_init(&clntobj->pipe, &pipe_attr);

   if(ret == QURT_EOK)
   {
      utimer_qdi_pipes[idx].allocated = TRUE;
      clntobj->pipe_idx = idx;
   }
   
   return ret;
} /* utimer_qdi_create_pipe */

/*=============================================================================

FUNCTION UTIMER_QDI_RELEASE

DESCRIPTION
 Releases the Qdi object that corresponding to a process that got destroyed

DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  UTimers related to this process will be ignored from here

=============================================================================*/
void utimer_qdi_release(qurt_qdi_obj_t *obj)
{
   utimer_qdi_opener *clntobj = (utimer_qdi_opener *)obj;
   uint32 process_idx = clntobj->process_idx;
   
   UTIMER_QDI_LOCK();

   utimer_drv_delete_process_timers(process_idx);
   
   if(clntobj->pipe_idx != -1)
   {
      /* defensive check */
      if(utimer_qdi_pipes[clntobj->pipe_idx].allocated != TRUE)
         ERR_FATAL("utimer_qdi_pipes[%d].allocated should be TRUE", clntobj->pipe_idx, 0, 0);
         
      qurt_pipe_destroy(&clntobj->pipe);
      utimer_qdi_pipes[clntobj->pipe_idx].allocated = FALSE;
      clntobj->pipe_idx = -1;
   }

   /* Free the object related to this process */
   utimer_qdi_clients[process_idx].opener_allocated = FALSE;
   utimer_qdi_clients_cntr--;
  
   UTIMER_QDI_FREE();
   return;
} /* timer_qdi_release */


/*=============================================================================

FUNCTION UTIMER_QDI_OPEN

DESCRIPTION
 Creates utimer driver object for the client and initializes required details

DEPENDENCIES
  None

RETURN VALUE
 QURT_EOK if the driver obj is defined and set correctly.

SIDE EFFECTS
  None

=============================================================================*/
int utimer_qdi_open(int client_handle, utimer_qdi_opener *me)
{
   utimer_qdi_opener *clntobj;
   int status;
   uint32 idx;
   
   UTIMER_QDI_LOCK();
   
   /* Find a free slot */
   for(idx=0; idx<UTIMER_MAX_CLIENTS && utimer_qdi_clients[idx].opener_allocated == TRUE; idx++);
   
   if(idx == UTIMER_MAX_CLIENTS)
   {
      UTIMER_QDI_FREE();
      ERR_FATAL("Max Processes %d reached, processes: %d", idx, utimer_qdi_clients_cntr, 0);
      return -1;
   }
   utimer_qdi_clients_cntr++;
   
   clntobj = &utimer_qdi_clients[idx];
   clntobj->qdiobj.invoke    = utimer_qdi_invoke;
   clntobj->qdiobj.refcnt    = QDI_REFCNT_INIT;
   clntobj->qdiobj.release   = utimer_qdi_release;
   clntobj->process_idx      = idx;   
   clntobj->opener_allocated = TRUE;
   
   /* Create Pipe only for remote processes */
   if(client_handle != QDI_HANDLE_LOCAL_CLIENT)
   {      
      status = utimer_qdi_create_pipe(clntobj);
      if(status == QURT_EFAILED)
      {
         clntobj->opener_allocated = FALSE;
         UTIMER_QDI_FREE();
         return -1;
      }
   }
   else
   {
      //clntobj->pipe = NULL;
      utimer_drv_set_local_process_idx(idx);
   }
   
   UTIMER_QDI_FREE();
   return qurt_qdi_handle_create_from_obj_t(client_handle, &clntobj->qdiobj);
} /* utimer_qdi_open */


/*=============================================================================

FUNCTION UTIMER_QDI_INIT

DESCRIPTION
 Registers with QDI Framework for ATS UTimers
 Registers callback handler that sends timer callback details to user process.
 
DEPENDENCIES
  None

RETURN VALUE
 None

SIDE EFFECTS
  None

=============================================================================*/

qurt_qdi_obj_t utimer_qdi_opener_obj;

void utimer_qdi_init 
(
  void
) 
{
  qurt_qdi_obj_t *p_opener = &utimer_qdi_opener_obj;
  int i;
    
  /* initialize the mutex here */
  qurt_rmutex_init ( &utimer_qdi_mutex );
  
  for(i=0; i<UTIMER_MAX_CLIENTS; i++)
  {
     utimer_qdi_clients[i].opener_allocated = FALSE;
     utimer_qdi_clients[i].pipe_idx         = -1;
  }
  
  for(i=0; i<UTIMER_MAX_PIPES; i++)
     utimer_qdi_pipes[i].allocated = FALSE;

   p_opener->invoke = utimer_qdi_invoke;
   p_opener->refcnt = QDI_REFCNT_INIT;
   p_opener->release = utimer_qdi_release;
   qurt_qdi_register_devname(UTIMER_DRIVER_NAME, p_opener);
   
   /* Register Process Callback Handler with ATS Timers. 
      Assumption is timer_qdi_open() gets called first for local process */
   utimer_drv_set_remote_process_cb_handler(utimer_qdi_remote_handler);
} /* utimer_qdi_init */

/*=============================================================================

FUNCTION UTIMER_QDI_REGISTER_FOR_NON_DEFER_EXPIRY_CHANGE

DESCRIPTION
    QDI wrapper for utimer_register_for_non_defer_expiry_change() api
 
DEPENDENCIES
  Should be called from main image (non-uimage)

RETURN VALUE
 None

SIDE EFFECTS
  None

=============================================================================*/
utimer_error_type utimer_qdi_register_for_non_defer_expiry_change(
	                       utimer_notify_type		    notify_type,
	                       utimer_osal_notify_obj_ptr	notify_obj,
	                       utimer_osal_notify_data 	    notify_mask,
	                       uint32 					    process_idx)
{
   return utimer_drv_register_for_non_defer_expiry_change(notify_type,notify_obj,notify_mask,process_idx);
} /* utimer_qdi_register_for_non_defer_expiry_change */