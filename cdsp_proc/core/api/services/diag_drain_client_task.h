/*============================================================================
@file Diag_Drain_Client_Task.h

Copyright (c) 2017 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
============================================================================*/

/*===========================================================================
                              Edit History

 $Header: //components/rel/core.qdsp6/2.1.c4/api/services/diag_drain_client_task.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/06/17   sp      Created new file to support diag drain client task functionality
===========================================================================*/

#ifndef DIAG_DRAIN_CLIENT_TASK_H
#define DIAG_DRAIN_CLIENT_TASK_H

#include "micro_ULog.h"

#include <string.h>
#include <stdarg.h>
#include "com_dtypes.h"

#ifdef __cplusplus
extern "C" {
#endif

/*Defines the parameter type*/

typedef enum
{
    DIAG_DRAIN_CLIENT_PARAM_HANDLE = 0,   /* For sharing microULog handle */
    DIAG_DRAIN_CLIENT_PARAM_VADDR  = 1,   /* For sharing virtual address mapping function pointer */
    DIAG_DRAIN_CLIENT_PARAM_SLEEP  = 2,   /* For sharing sleep voting function pointer */
    DIAG_DRAIN_CLIENT_MAX_PARAM    = DIAG_DRAIN_CLIENT_PARAM_SLEEP
}diag_drain_client_param_type;

/* Sleep voting function pointer for draining */

typedef void (*sleep_voting_fptr)( boolean awake );

/*===========================================================================

FUNCTION diag_drain_client_set_param

DESCRIPTION
  API exposed to the diag userPD client to set the microULog handle, mapping API, sleep API

PARAMETERS
  param_type – diag drain client parameter type
  parameter – pointer to handle/virtual function/sleep function

DEPENDENCIES
   Successful initialization requires Diag driver to be loaded 
   and available in the system

RETURN VALUE
   FALSE - Setting the microUlog handle failed
   TRUE  - Setting the microUlog handle succeeded

===========================================================================*/

boolean diag_drain_client_set_param(diag_drain_client_param_type param_type, void * parameter);

#ifdef __cplusplus
}
#endif

#endif /* DIAG_DRAIN_CLIENT_TASK_H */
