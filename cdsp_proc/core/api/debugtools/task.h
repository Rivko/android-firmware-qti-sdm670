#ifndef TASK_H
#define TASK_H

/*===========================================================================

            T A S K    R E S O U R C E S    H E A D E R    F I L E

DESCRIPTION
  LEGACY TASK/REX API.
  This file contains declarations defining global task resources. Task
  resources are no longer considered global; references are to be isolated
  to tech area deliverables and use RCINIT Framework runtime lookup
  functions.

Copyright (c) 2012 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1.c4/api/debugtools/task.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

// The supported interface for runtime utilization is the RCINIT
// Framework. Utilize the RCINIT Framework API to obtain equivilent
// information through runtime accessors for task provisioning parameters
// that were utilized by TMC LEGACY task management.
//
// The following methods are used to correct use of an deprecated
// interface. Look for information details to the corresponding warning
// message being emitted at compile time.
//
// 1. warning: 'TASK_HANDLE_ACCESSOR' is deprecated (gcc)
//
// 1.a replace with rex_self() when the context reference is the same as
// the context the software is running in.
//
// 1.b replace with rcinit_lookup_rextask("task") when the context reference
// is not the same context as the software is running in. include "rcinit_rex.h"
//
// 1.c cache the return result from rex_self or rcinit_lookup_rextask as
// both calls are expensive to perform repeatedly for the same result
// to be returned each time. rcinit_lookup_rextask includes string comparisions
// over a dictionary of task names for example.
//
// 2. warning: 'TASK_PRIO_ACCESSOR' is deprecated (gcc)
//
// 2.a replace with rcinit_lookup_prio("task"). include "rcinit_rex.h"
//
// 2.b return result +1 is higher "amss" priority, return result -1 is lower "amss"
// priority. realize that NHLOS priority mapping to "amss" priority mapping is not
// always one to one.
//
// 3. warning: 'TASK_START_SIG_ACCESSOR' is deprecated (gcc)
// 3. warning: 'TASK_OFFLINE_SIG_ACCESSOR' is deprecated (gcc)
// 3. warning: 'TASK_STOP_SIG_ACCESSOR' is deprecated (gcc)
//
// 3.a replace with a private mask relevant only to the context as these
// symbols are global. the tech area will utilize a private definition with
// meaning only to the recipient context. the private definition will be used
// in all runtime API that require a signal mask parameter.
//
// 3.b as an example:
// #define PRIVATE_TASK_START_SIG   0x00008000 // value is arbitrary selection for context receiving
// #define PRIVATE_TASK_OFFLINE_SIG 0x00002000 // value is arbirtary selection for context receiving
// #define PRIVATE_TASK_STOP_SIG    0x00004000 // value is arbitrary selection for context recieving
//
// 3.c the only context that needs to be aware of signal mask values is the reciever.
// all other use should be captured in an API that clients can use to communicate with
// the receiver. use better partitioning of the software to isolate signal masks to be
// localized.

#include "rcinit_rex.h"

#if defined(__cplusplus)
extern "C" {
#endif

// comment out __attribute__() to disable the specific instrumentation

#define TASK_DEPRECATED_INTERFACE /*__attribute__((deprecated))*/

#if defined(__GNUC__)
#pragma GCC diagnostic warning "-Wdeprecated-declarations"
#endif

typedef const char* TASK_HANDLE_ACCESSOR TASK_DEPRECATED_INTERFACE; // obsolete; task owner supplies definition
typedef const char* TASK_PRIO_ACCESSOR TASK_DEPRECATED_INTERFACE; // obsolete; task owner supplies definition
typedef unsigned long TASK_SIGNAL_ACCESSOR TASK_DEPRECATED_INTERFACE; // obsolete; task owner supplies it's own signal mask definition
typedef TASK_SIGNAL_ACCESSOR TASK_FS_OP_SIG_ACCESSOR TASK_DEPRECATED_INTERFACE; // obsolete; task owner supplies it's own signal mask definition
typedef TASK_SIGNAL_ACCESSOR TASK_SEC_OP_SIG_ACCESSOR TASK_DEPRECATED_INTERFACE; // obsolete; task owner supplies it's own signal mask definition
typedef TASK_SIGNAL_ACCESSOR TASK_START_SIG_ACCESSOR TASK_DEPRECATED_INTERFACE; // obsolete; task owner supplies it's own signal mask definition
typedef TASK_SIGNAL_ACCESSOR TASK_OFFLINE_SIG_ACCESSOR TASK_DEPRECATED_INTERFACE; // obsolete; task owner supplies it's own signal mask definition
typedef TASK_SIGNAL_ACCESSOR TASK_STOP_SIG_ACCESSOR TASK_DEPRECATED_INTERFACE; // obsolete; task owner supplies it's own signal mask definition

// TASK_EXTERN() remediation: remove reference to task.h, include rcinit_rex.h, use: extern rex_tcb_type taskname_tcb;
#define TASK_EXTERN(t_name) extern rex_tcb_type t_name##_tcb TASK_DEPRECATED_INTERFACE; // obsolete; task owner supplies obtains value at runtime with rex_self()

// TASK_HANDLE() remediation: remove reference to task.h, include rcinit_rex.h, use: rcinit_lookup_rextask("taskname")
#define TASK_HANDLE(t_name) rcinit_lookup_rextask((TASK_HANDLE_ACCESSOR)#t_name) // obsolete; task owner uses runtime lookup rcinit_lookup_rextask()

// TASK_HANDLE() remediation: remove reference to task.h, include rcinit_rex.h, use: rcinit_lookup_prio("taskname")
#define TASK_PRIO(t_name) rcinit_lookup_prio((TASK_PRIO_ACCESSOR)#t_name) // obsolete; task owner uses runtime lookup rcinit_lookup_prio()

// TASK_EXTERN(fs) remediation: remove reference to task.h, use: extern rex_tcb_type fs_tcb;
TASK_EXTERN(fs) // FS TO PRIVATIZE

// TASK_EXTERN(nv) remediation: remove reference to task.h, use: extern rex_tcb_type nv_tcb;
TASK_EXTERN(nv) // NV TO PRIVATIZE

// TASK_EXTERN(sec) remediation: remove reference to task.h, use: extern rex_tcb_type sec_tcb;
TASK_EXTERN(sec) // SEC TO PRIVATIZE

// TASK_EXTERN(seccryptarm) remediation: remove reference to task.h, use: extern rex_tcb_type seccryptarm_tcb;
TASK_EXTERN(seccryptarm) // SEC TO PRIVATIZE

// TASK_EXTERN(secrnd) remediation: remove reference to task.h, use: extern rex_tcb_type secrnd_tcb;
TASK_EXTERN(secrnd) // SEC TO PRIVATIZE

// TASK_EXTERN(sectestapp) remediation: remove reference to task.h, use: extern rex_tcb_type sectestapp_tcb;
TASK_EXTERN(sectestapp) // SEC TO PRIVATIZE

// TASK_EXTERN(timer) remediation: remove reference to task.h, use: extern rex_tcb_type timer_tcb;
TASK_EXTERN(timer) // TIMER TO PRIVATIZE

// DIAG_PRI remediation: remove reference to task.h, include rcinit_rex.h, use: rcinit_lookup_prio("diag")
#define DIAG_PRI TASK_PRIO(diag) // DIAG TO PRIVATIZE

// FS_OP_COMPLETE_SIG remediation: remove reference to task.h, use: #define FS_OP_COMPLETE_SIG 0x40000000
#define FS_OP_COMPLETE_SIG ((TASK_FS_OP_SIG_ACCESSOR)0x40000000) // FS TO PRIVATIZE

// SEC_OP_COMPLETE_SIG remediation: remove reference to task.h, use: #define SEC_OP_COMPLETE_SIG 0x10000000
#define SEC_OP_COMPLETE_SIG ((TASK_SEC_OP_SIG_ACCESSOR)0x10000000) // SEC TO PRIVATIZE

// TASK_START_SIG remediation: remove reference to task.h, use #define PRIVATE_TASK_START_SIG 0x0x00008000
#define TASK_START_SIG ((TASK_START_SIG_ACCESSOR)0x00008000) // GLOBAL, TECH AREA PRIVATIZE

// TASK_OFFLINE_SIG remediation: remove reference to task.h, use #define PRIVATE_TASK_OFFLINE_SIG 0x0x00008000
#define TASK_OFFLINE_SIG ((TASK_OFFLINE_SIG_ACCESSOR)0x00002000) // GLOBAL, TECH AREA PRIVATIZE

// TASK_STOP_SIG remediation: remove reference to task.h, use #define PRIVATE_TASK_STOP_SIG 0x0x00008000
#define TASK_STOP_SIG ((TASK_STOP_SIG_ACCESSOR)0x00004000) // GLOBAL, TECH AREA PRIVATIZE

#if defined(__cplusplus)
}
#endif

#endif
