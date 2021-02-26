/*=============================================================================

                                    qurt_ptrace.h

GENERAL DESCRIPTION

EXTERNAL FUNCTIONS
        None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
        None.

             Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/
#ifndef __SYS_PTRACE_H__
#define __SYS_PTRACE_H__

enum __ptrace_request
{
   /**
     @brief Indicate that the process making this request is requesting to be traced.
   */
   PTRACE_TRACEME = 0,
   PTRACE_EXT_IS_DEBUG_PERMITTED = 500
};

/* Changing the 2nd argument to be thread id as tcb is no longer accessible from guestOS and user PDs */
long ptrace(enum __ptrace_request request, unsigned int tid, void*addr, void *data);

#endif //__SYS_PTRACE_H__