#ifndef DIAG_DRAIN_CLIENT_TASK_I_H
#define DIAG_DRAIN_CLIENT_TASK_I_H
/*============================================================================
  diag_drain_client_task_i.h

  diag_drain_client_task related functions

  Copyright (c) 2017 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/

/* $Header: //components/rel/core.qdsp6/2.1.c4/services/diag/diag_drain_client_task/src/diag_drain_client_task_i.h#1 $ */

/*==========================================================================
when       who     what, where, why
--------   ---     ----------------------------------------------------------------
07/05/17   sp      Fix compiler error in function prototype
06/07/17   sp      Created new file to support diag drain client task functionality
=================================================================================*/

#ifdef __cplusplus
extern "C"
{
#endif

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
void diag_client_drain(void);

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

void diag_drain_client_task_start(uint32 params);

/*===========================================================================
FUNCTION diag_drain_client_task_init

DESCRIPTION
  Create a read thread to drain the client microULog buffer

PARAMETERS
  None

RETURN VALUE
  None

===========================================================================*/

void diag_drain_client_task_init(void);

/*===========================================================================

FUNCTION diag_drain_client_task_block_timeout

DESCRIPTION
  This routine makes diag_drain_client_task to wait on associated signals

PARAMETERS
  None

RETURN VALUE
  None

===========================================================================*/

void diag_drain_client_task_block_timeout(void);

/*===========================================================================

FUNCTION diag_drain_client_handle_sigs

DESCRIPTION
  This routine makes diag_drain_client_task to handle associated signals

PARAMETERS
  None

RETURN VALUE
  None

===========================================================================*/

void diag_drain_client_handle_sigs (void);

#ifdef __cplusplus
}
#endif
#endif              /* DIAG_DRAIN_CLIENT_TASK_I_H  */
