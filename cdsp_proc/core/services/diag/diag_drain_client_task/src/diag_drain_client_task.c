/*============================================================================
                          diag_drain_client_task

  diag_drain_client_task related functions

  Copyright (c) 2017 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/

/* $Header: //components/rel/core.qdsp6/2.1.c4/services/diag/diag_drain_client_task/src/diag_drain_client_task.c#1 $ */

/*==========================================================================
when       who     what, where, why
--------   ---    ----------------------------------------------------------------
07/05/17   sp     Fix compiler error in function prototype
06/07/17   sp     Created new file to support diag drain client task functionality
================================================================================*/
#include "log.h"
#include "assert.h"
#include "msg_diag_service.h"
#include "diag.h" /* for log packet version */
#include <stdlib.h>
#include "diag_param.h"
#include "tms_dll_api.h"

#include "diag_drain_client_task.h"
#include "diag_drain_client_task_i.h"
#include "micro_diagbuffer_i.h"
#include "micro_diagbuffer_client_read.h"

/* diag_drain_client_task_tcb related variables */

#define DIAG_DRAIN_CLIENT_TASK_STACK_SIZE 512
#define DIAG_DRAIN_CLIENT_TASK_TIMER_LEN 200

osal_thread_attr_t diag_drain_client_task_attr;
osal_tcb_t diag_drain_client_task_tcb;
osal_timer_t diag_drain_client_task_block_timer;

unsigned long long diag_drain_client_task_stack[DIAG_DRAIN_CLIENT_TASK_STACK_SIZE];

micro_ULogHandle diag_drain_client_hdl = NULL;

vaddr_fptr diag_drain_client_vaddr_fptr = NULL;

sleep_voting_fptr diag_drain_client_sleep_voting_fptr = NULL;

/* Variables to store allocation requests for microUlog */
uint32 alloc_count_micro_ulog;

/* Variables to store drop count for microUlog */
uint32 drop_count_micro_ulog;

/*===========================================================================

FUNCTION diag_drain_client_task_block_timeout

DESCRIPTION
  This routine makes diag_drain_client_task to wait on associated signals

PARAMETERS
  None

RETURN VALUE
  None

===========================================================================*/

void diag_drain_client_task_block_timeout(void)
{
  osal_sigs_t return_sigs = 0;
  int return_val = 0;

  return_val = osal_set_timer(&diag_drain_client_task_block_timer, DIAG_DRAIN_CLIENT_TASK_TIMER_LEN);
  ASSERT(OSAL_SUCCESS == return_val);

  return_sigs = osal_thread_wait (&diag_drain_client_task_tcb, DIAG_DRAIN_CLIENT_TASK_TIMEOUT_SIG);
}

/*===========================================================================

FUNCTION diag_drain_client_set_param

DESCRIPTION
  API exposed to the diag userPD client to set the microULog handle, mapping API, sleep API

PARAMETERS
  param_type - diag_drain_client_param_type
  parameter - pointer to handle/virtual function/sleep function

DEPENDENCIES
   Successful initialization requires Diag userPD to be loaded 
   and available in the system

RETURN VALUE
   FALSE - Setting the parameter failed
   TRUE  - Setting the parameter succeeded

===========================================================================*/
DLL_API_GLOBAL boolean diag_drain_client_set_param(diag_drain_client_param_type param_type, void * parameter)
{
  osal_sigs_t return_sigs;
  int return_val = 0;

  if ((param_type > DIAG_DRAIN_CLIENT_MAX_PARAM) || (NULL == parameter))
  {
    return (FALSE);
  }
  
  switch (param_type)
  {
    case DIAG_DRAIN_CLIENT_PARAM_HANDLE:
      diag_drain_client_hdl = (micro_ULogHandle)parameter;
      if (diag_drain_client_vaddr_fptr)
      {
        /* Convert handle to local address using vaddr mapping function */
        diag_drain_client_hdl = (micro_ULogHandle)diag_drain_client_vaddr_fptr((uint32)diag_drain_client_hdl);
      }
      break;

    case DIAG_DRAIN_CLIENT_PARAM_VADDR:
      diag_drain_client_vaddr_fptr = (vaddr_fptr)parameter;
      if (diag_drain_client_hdl)
      {
        /* Convert handle to local address using vaddr mapping function */
        diag_drain_client_hdl = (micro_ULogHandle)diag_drain_client_vaddr_fptr((uint32)diag_drain_client_hdl);
      }
      break;

    case DIAG_DRAIN_CLIENT_PARAM_SLEEP:
      diag_drain_client_sleep_voting_fptr = (sleep_voting_fptr)parameter;
      break;
  
    default:
      return (FALSE);
	}
  
	/* Set the signal to diag_drain_client_task once the handle, mapping and sleep fptr is set */
	if (diag_drain_client_hdl && diag_drain_client_vaddr_fptr && diag_drain_client_sleep_voting_fptr)
	{
	  return_val = osal_set_sigs(&diag_drain_client_task_tcb, DIAG_DRAIN_CLIENT_TASK_SET_PARAM_DONE_SIG, &return_sigs);
  
	  ASSERT(OSAL_SUCCESS == return_val);
	}
	return (TRUE);
}

/*===========================================================================

FUNCTION diag_drain_client_handle_sigs

DESCRIPTION
  This routine makes diag_drain_client_task to handle associated signals

PARAMETERS
  None

RETURN VALUE
  None

===========================================================================*/

void diag_drain_client_handle_sigs (void)
{
  osal_sigs_t sigs = 0;
  osal_sigs_t return_sigs = 0;
  osal_sigs_t reset_sigs = 0;
  int return_val = 0;

  return_sigs = osal_thread_wait (&diag_drain_client_task_tcb, DIAG_DRAIN_CLIENT_TASK_SET_PARAM_DONE_SIG);
  return_val = osal_get_sigs(&diag_drain_client_task_tcb, &sigs);
  ASSERT(OSAL_SUCCESS == return_val);

  if (sigs & DIAG_DRAIN_CLIENT_TASK_SET_PARAM_DONE_SIG)
  {
    return_val = osal_reset_sigs(&diag_drain_client_task_tcb, DIAG_DRAIN_CLIENT_TASK_SET_PARAM_DONE_SIG, &reset_sigs);
    ASSERT(OSAL_SUCCESS == return_val);
    diag_client_drain();
  }
}

/*===========================================================================

FUNCTION diag_drain_client_task_start

DESCRIPTION
  This function contains the diag_drain_client_task. This task reads debug messages
  in client micro ulog buffer, formats them and copies to diagmpdbuf to be
  sent over communication layer

  This procedure does not return.

PARAMETERS
  params - task attributes

RETURN VALUE
  None

===========================================================================*/

void diag_drain_client_task_start(uint32 params)
{

 /*----------------------------------------------------------------------------
                           Begin task operations
----------------------------------------------------------------------------*/
  (void) diag_drain_client_handle_sigs ();
}

/*===========================================================================

FUNCTION diag_client_drain

DESCRIPTION
  This routine drains debug messages in client micro ulog buffer,
  formats them and copies to diagmpdbuf to be sent over communication layer

PARAMETERS
  None

RETURN VALUE
  None

===========================================================================*/
void diag_client_drain(void)
{
  unsigned long int outputFormat;
  osal_sigs_t return_sigs;
  osal_sigs_t sigs = 0;
  int return_val = 0;
  uint8* payload_buf = NULL;
  LOG_DIAG_ULOG_type * log_ptr_microulog = NULL;
  log_code_type code = LOG_DIAG_ULOG_PAYLOAD;
  unsigned int version = DIAG_EXT_API_VERSION_3;
  micro_ULogResult result = MICRO_ULOG_SUCCESS;
  boolean keep_awake = TRUE;

  while (diag_drain_client_hdl)
  {
    /* Get signals on diag_drain_client_task */
    return_val = osal_get_sigs(&diag_drain_client_task_tcb, &sigs);
    ASSERT(OSAL_SUCCESS == return_val);

    if (sigs & DIAG_DRAIN_CLIENT_TASK_TIMEOUT_SIG)
    {
      return_val = osal_reset_sigs(&diag_drain_client_task_tcb, DIAG_DRAIN_CLIENT_TASK_TIMEOUT_SIG, &return_sigs);
      ASSERT(OSAL_SUCCESS == return_val);
    }

    if(log_status(code))
    {
      payload_buf= (uint8 *)malloc(DIAG_ULOG_MAX_PAYLOAD_SIZE);

      if(payload_buf)
      {
        unsigned long int bytes_written = 0;
        uint32 pkt_len = 0;

        /* Vote for shared memory before accessing */
        if (diag_drain_client_sleep_voting_fptr)
        {
          keep_awake = TRUE;
          diag_drain_client_sleep_voting_fptr(keep_awake);
        }

        /* read and format pkts from client buffer */
        result = micro_diagbuffer_ReadAndFormat_vaddr(diag_drain_client_hdl, MICROULOG_DIAG_FORMAT_ASCII,
                                                &outputFormat, DIAG_ULOG_MAX_PAYLOAD_SIZE,
                                                (char *)payload_buf, &bytes_written, diag_drain_client_vaddr_fptr);

        if((MICRO_ULOG_SUCCESS == result) && (bytes_written > 0))
        {
          pkt_len = FPOS(LOG_DIAG_ULOG_type, buf);
          pkt_len += bytes_written;

          log_ptr_microulog = (LOG_DIAG_ULOG_type *) log_alloc_ext (LOG_DIAG_ULOG_PAYLOAD,
                               pkt_len , version, diag_drain_client_hdl);
          if(log_ptr_microulog)
          {
            log_ptr_microulog->output_format = outputFormat;
            log_ptr_microulog->output_length = bytes_written;
            memscpy(&log_ptr_microulog->buf[0], bytes_written, payload_buf, bytes_written );
            log_commit_ext(log_ptr_microulog);
            alloc_count_micro_ulog++;
          }
          else
          {
            drop_count_micro_ulog++;
          }
        }
        else if ((MICRO_ULOG_SUCCESS == result) && (!bytes_written))
        {
          free(payload_buf);
          if (diag_drain_client_sleep_voting_fptr)
          {
            keep_awake = FALSE;

            /* Call Sleep function to unvote for shared memory */
            diag_drain_client_sleep_voting_fptr(keep_awake);
          }
          diag_drain_client_task_block_timeout();
          continue;
        }
        else if (MICRO_ULOG_SUCCESS != result)
        {
          MSG_1(MSG_SSID_DIAG, MSG_LEGACY_MED, "Unable to read and format logs, ERROR %d", result);
        }
        free(payload_buf);
      }
    }
    else
    {
      if (diag_drain_client_sleep_voting_fptr)
      {
        keep_awake = FALSE;

        /* Call Sleep function to unvote for shared memory */
        diag_drain_client_sleep_voting_fptr(keep_awake);
      }    
      diag_drain_client_task_block_timeout();
      continue;
    }
  }
}

/*===========================================================================

FUNCTION diag_drain_client_task_init

DESCRIPTION
  Create a read thread to drain the client microULog buffer

PARAMETERS
  None

RETURN VALUE
  None

===========================================================================*/
DLL_API_GLOBAL void diag_drain_client_task_init(void)
{
  int return_val = 0;

  int diag_drain_client_task_pri_array[32] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

  /* Start the task */
  diag_drain_client_task_attr.stack_size = (DIAG_DRAIN_CLIENT_TASK_STACK_SIZE * sizeof(unsigned long long));
  diag_drain_client_task_attr.start_func = diag_drain_client_task_start;
  diag_drain_client_task_attr.arg = NULL;
  diag_drain_client_task_attr.name = "diag_drain_client_task";
  diag_drain_client_task_attr.priority = (DIAG_PRI - 1);
  diag_drain_client_task_attr.stack_address = diag_drain_client_task_stack;
  diag_drain_client_task_attr.p_tskname = "DIAG_DRAIN_CLIENT_TASK";
  diag_drain_client_task_attr.suspended = FALSE;
  diag_drain_client_task_attr.dog_report_val = 0;
  
  return_val = osal_create_thread(&diag_drain_client_task_tcb, &diag_drain_client_task_attr, diag_drain_client_task_pri_array);
  if (return_val != 0)
  {
    ERR_FATAL("Create diag_drain_client_task failed!", return_val, 0, 0);
  }
  return_val = osal_create_timer(&diag_drain_client_task_block_timer, &diag_drain_client_task_tcb , DIAG_DRAIN_CLIENT_TASK_TIMEOUT_SIG);
  ASSERT(OSAL_SUCCESS == return_val);
}

