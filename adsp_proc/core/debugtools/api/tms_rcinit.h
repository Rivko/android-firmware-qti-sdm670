#ifndef TMS_RCINIT_H
#define TMS_RCINIT_H
/** vi: tw=128 ts=3 sw=3 et :
@file tms_rcinit.h
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
Copyright (c) 2015-2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.qdsp6/2.1/debugtools/api/tms_rcinit.h#2 $
$DateTime: 2017/09/27 08:08:43 $
$Change: 14490220 $
$Author: pwbldsvc $
===============================================================================*/

#include "tms_dll_api.h"                                                         /**< MUST PRECEDE CLIENT DEFINITION OF DLL_API_EXPORT */

#if defined(DLL_API_BUILD_RCINIT)                                                /**< REQUIRED TO BE DECLARED IN TECH AREA SOURCE MODULES (SU/VU VISIBLE) */
#undef  DLL_API_EXPORT_RCINIT                                                    /**< REDEFINITION FOR THIS SPECIFIC MODULE */
#define DLL_API_EXPORT_RCINIT DLL_API_EXPORT_HELPER                              /**< CONTROL OVER DLL SYMBOL VISIBILITY BY EXPORT */
#else
#undef  DLL_API_EXPORT_RCINIT                                                    /**< REDEFINITION FOR THIS SPECIFIC MODULE */
#define DLL_API_EXPORT_RCINIT DLL_API_IMPORT_HELPER                              /**< CONTROL OVER DLL SYMBOL VISIBILITY BY EXPORT */
#endif

#include "stdlib.h"

#include "rcinit_dal.h"
#include "tms_rcevt.h"
#include "rcxh.h"

#if defined(__cplusplus)
extern "C"
{
#endif

/////////////////////////////////////////////////////////////////////
// Tuneable Controls
/////////////////////////////////////////////////////////////////////

#define rcinit_internal_malloc(x)   malloc(x)                                    // internal allocator implementation wrapper
#define rcinit_internal_free(x)     free(x)                                      // internal allocator implementation wrapper

/////////////////////////////////////////////////////////////////////
// Localized Manifest Constants
/////////////////////////////////////////////////////////////////////

#define SIG_ACK                                       0x00010000
#define SIG_TMO                                       0x00020000
#define SIG_BAIL                                      0x00040000

#define RCINIT_NAME_MAX                               16    // Tracks QURT_THREAD_ATTR_NAME_MAXLEN or (REX_TASK_NAME_LEN+1)
#define RCINIT_TASK_MAX                               300   // maximum number of tasks tracked for compile time sized tables

// Logic Used to Determine if RCINIT Database Entry is INITFN
// Internal Macro : IS_INITFN, entry, no handle, no priority, no stack size, no stack pointer, not rcinit

#define IS_INITFN(rcinit_p) \
    RCINIT_NULL != rcinit_p && \
    RCINIT_NULL != rcinit_p->entry && \
    RCINIT_TASK_INITFN == rcinit_p->type && \
    RCINIT_PRIO_INITFN == rcinit_p->prio && \
    RCINIT_STKSZ_ZERO == rcinit_p->stksz && \
    RCINIT_NULL == rcinit_p->handle && \
    rcinit_lookup("rcinit") != rcinit_p

// Logic Used to Determine if RCINIT Database Entry is TASK
// Internal Macro : IS_TASK, entry, handle, priority, stack size, stack pointer, not rcinit

#define IS_TASK(rcinit_p) \
    RCINIT_NULL != rcinit_p && \
    RCINIT_NULL != rcinit_p->entry && \
    (RCINIT_TASK_DALTASK == rcinit_p->type || RCINIT_TASK_POSIX == rcinit_p->type || RCINIT_TASK_QURTTASK == rcinit_p->type || RCINIT_TASK_REXTASK == rcinit_p->type) && \
    RCINIT_PRIO_INITFN != rcinit_p->prio && \
    RCINIT_STKSZ_ZERO != rcinit_p->stksz && \
    RCINIT_NULL != rcinit_p->handle && \
    rcinit_lookup("rcinit") != rcinit_p

// internally managed events (no observers, these are policy based with ack)

#define RCINIT_RCEVT_INTERNAL_PREFIX         "rci:"

#define RCINIT_RCEVT_DEFINE                  RCINIT_RCEVT_INTERNAL_PREFIX "define"
#define RCINIT_RCEVT_DEFINEACK               RCINIT_RCEVT_INTERNAL_PREFIX "defack"
#define RCINIT_RCEVT_INITFN                  RCINIT_RCEVT_INTERNAL_PREFIX "initfn"
#define RCINIT_RCEVT_TERMACK                 RCINIT_RCEVT_INTERNAL_PREFIX "trmack"
#define RCINIT_RCEVT_TERM_BARRIER            RCINIT_RCEVT_INTERNAL_PREFIX "trmbar"

#define RCINIT_RCECB_INIT_GROUP_0            RCINIT_RCEVT_INTERNAL_PREFIX "ifcb:0"
#define RCINIT_RCECB_INIT_GROUP_1            RCINIT_RCEVT_INTERNAL_PREFIX "ifcb:1"
#define RCINIT_RCECB_INIT_GROUP_2            RCINIT_RCEVT_INTERNAL_PREFIX "ifcb:2"
#define RCINIT_RCECB_INIT_GROUP_3            RCINIT_RCEVT_INTERNAL_PREFIX "ifcb:3"
#define RCINIT_RCECB_INIT_GROUP_4            RCINIT_RCEVT_INTERNAL_PREFIX "ifcb:4"
#define RCINIT_RCECB_INIT_GROUP_5            RCINIT_RCEVT_INTERNAL_PREFIX "ifcb:5"
#define RCINIT_RCECB_INIT_GROUP_6            RCINIT_RCEVT_INTERNAL_PREFIX "ifcb:6"
#define RCINIT_RCECB_INIT_GROUP_7            RCINIT_RCEVT_INTERNAL_PREFIX "ifcb:7"

#define RCINIT_RCEVT_INIT_GROUP_0            RCINIT_RCEVT_INTERNAL_PREFIX "init:0"
#define RCINIT_RCEVT_INIT_GROUP_1            RCINIT_RCEVT_INTERNAL_PREFIX "init:1"
#define RCINIT_RCEVT_INIT_GROUP_2            RCINIT_RCEVT_INTERNAL_PREFIX "init:2"
#define RCINIT_RCEVT_INIT_GROUP_3            RCINIT_RCEVT_INTERNAL_PREFIX "init:3"
#define RCINIT_RCEVT_INIT_GROUP_4            RCINIT_RCEVT_INTERNAL_PREFIX "init:4"
#define RCINIT_RCEVT_INIT_GROUP_5            RCINIT_RCEVT_INTERNAL_PREFIX "init:5"
#define RCINIT_RCEVT_INIT_GROUP_6            RCINIT_RCEVT_INTERNAL_PREFIX "init:6"
#define RCINIT_RCEVT_INIT_GROUP_7            RCINIT_RCEVT_INTERNAL_PREFIX "init:7"

#define RCINIT_RCECB_TERM_GROUP_0            RCINIT_RCEVT_INTERNAL_PREFIX "tfcb:0"
#define RCINIT_RCECB_TERM_GROUP_1            RCINIT_RCEVT_INTERNAL_PREFIX "tfcb:1"
#define RCINIT_RCECB_TERM_GROUP_2            RCINIT_RCEVT_INTERNAL_PREFIX "tfcb:2"
#define RCINIT_RCECB_TERM_GROUP_3            RCINIT_RCEVT_INTERNAL_PREFIX "tfcb:3"
#define RCINIT_RCECB_TERM_GROUP_4            RCINIT_RCEVT_INTERNAL_PREFIX "tfcb:4"
#define RCINIT_RCECB_TERM_GROUP_5            RCINIT_RCEVT_INTERNAL_PREFIX "tfcb:5"
#define RCINIT_RCECB_TERM_GROUP_6            RCINIT_RCEVT_INTERNAL_PREFIX "tfcb:6"
#define RCINIT_RCECB_TERM_GROUP_7            RCINIT_RCEVT_INTERNAL_PREFIX "tfcb:7"

#define RCINIT_RCEVT_TERM_GROUP_0            RCINIT_RCEVT_INTERNAL_PREFIX "term:0"
#define RCINIT_RCEVT_TERM_GROUP_1            RCINIT_RCEVT_INTERNAL_PREFIX "term:1"
#define RCINIT_RCEVT_TERM_GROUP_2            RCINIT_RCEVT_INTERNAL_PREFIX "term:2"
#define RCINIT_RCEVT_TERM_GROUP_3            RCINIT_RCEVT_INTERNAL_PREFIX "term:3"
#define RCINIT_RCEVT_TERM_GROUP_4            RCINIT_RCEVT_INTERNAL_PREFIX "term:4"
#define RCINIT_RCEVT_TERM_GROUP_5            RCINIT_RCEVT_INTERNAL_PREFIX "term:5"
#define RCINIT_RCEVT_TERM_GROUP_6            RCINIT_RCEVT_INTERNAL_PREFIX "term:6"
#define RCINIT_RCEVT_TERM_GROUP_7            RCINIT_RCEVT_INTERNAL_PREFIX "term:7"

// Type Mapping to Internal State

typedef enum
{
   // rcinit task processing state

   rcinit_process_none = 0x000000ff,                                             // rcinit specific
   rcinit_process_init = 0x00000001,                                             // rcinit initialize
   rcinit_process_group_policy_fn = 0x00000002,                                  // rcinit processes policyfn (per group)
   rcinit_process_group_sizing = 0x00000003,                                     // rcinit processes statics sizing (per group)
   rcinit_process_group_allocation = 0x00000004,                                 // rcinit processes allocations (per group)
   rcinit_process_group_assignment = 0x00000005,                                 // rcinit processes statics assignment (per group)
   rcinit_process_group_init_fn = 0x00000006,                                    // rcinit processes initfn (per group)
   rcinit_process_group_init_task = 0x00000007,                                  // rcinit processes tasks (per group)
   rcinit_process_group_init_blocking = 0x00000008,                              // rcinit blocking for task handshakes (per group)
   rcinit_process_group_run = 0x00000009,                                        // rcinit releases group to run (per group)
   rcinit_process_blocking = 0x0000000a,                                         // rcinit blocking
   rcinit_process_running = 0x0000000b,                                          // rcinit running
   rcinit_process_group_term_task = 0x0000000c,                                  // rcinit processes tasks (per group)
   rcinit_process_group_term_blocking = 0x0000000d,                              // rcinit processes tasks (per group)
   rcinit_process_group_term_fn = 0x0000000e,                                    // rcinit processes termfn (per group)
   rcinit_process_term = 0x000000fe,                                             // rcinit done
   rcinit_process_mask = rcinit_process_none,

   rcinit_context_none = 0x0000ff00,                                             // context specific, not active (set by rcinit)
   rcinit_context_mask = rcinit_context_none,

   // internal use

   rcinit_state_unknown = 0x00000000,
   rcinit_state_max = 0x7fffffff

} rcinit_state_t;

// Type Mapping to NONHLOS

// volatile memory structure associated to tasks, inits NULL

#define RCINIT_CONTEXT_STATE(y, x) do { if (RCINIT_NULL != (y) && RCINIT_NULL != (y)->handle) { (y)->handle->context_state = (x); } } while(0)

typedef struct rcinit_internal_hs
{
   RCINIT_GROUP group;
   char task_name[RCINIT_NAME_MAX];
   unsigned long hs;                                                             // the first and only acceptable single handshake per group
   unsigned long hs_dup;                                                         // duplicates, same group
   unsigned long hs_oob;                                                         // out of band, different group
   void* tid;

} rcinit_internal_hs_t, *rcinit_internal_hs_p;

// rcinit instrumentation state progression

extern int const rcinit_internal_name_map_size;
extern const rcinit_internal_name_map_t rcinit_internal_name_map[];              // provisioning info name map
extern int const rcinit_internal_policy_map_size;
extern const rcinit_internal_policy_map_t rcinit_internal_policy_map[];          // policy name map
extern const rcinit_info_t*** rcinit_internal_groups[];                          // top of database tree
extern const RCINIT_NAME rcinit_internal_policy_list[];                          // null terminated list of policy names
extern const rcinit_internal_priority_map_t rcinit_internal_priority_map[];
extern int const rcinit_internal_priority_map_size;

// processing state
// trace32: v.v rcinit_internal

struct rcinit_internal_s
{
   rcinit_state_t process_state;                                                 // current group processing state

   RCINIT_GROUP group_curr;                                                      // current group (numeric, range 0 .. RCINIT_GROUP_MAX)

   RCINIT_POLICY policy_curr;                                                    // current policy (numeric, range 0 .. rcinit_internal_policy_map_size)

   int policy_oneshot;                                                           // policy extension granted

   RCINIT_NAME(*policyfn)(const RCINIT_NAME[]);                                  // last executed policy extension callback

   void (*initfn_curr)(void);                                                    // rcworker current initfn

   rcinit_internal_hs_t hs_init[RCINIT_TASK_MAX];                                // term, registered tasks, per group, handshakes ignored

#if !defined(RCINIT_EXCLUDE_TERMINATION)                                         // term, size optimization when excluded
   rcinit_internal_hs_t hs_term[RCINIT_TASK_MAX];                                // term, registered tasks, per group, handshakes ignored
#endif

   // the following instruments are used for indepth manual debugging

   const rcinit_info_t*** policy_base;                                           // base pointer loaded from index

   const rcinit_info_t** group_base;                                             // base pointer loaded from index

   rce_nde_p define;                                                             // internal event, rcinit_task <- client context created and blocks

   rce_nde_p defineack;                                                          // internal event, rcinit_task -> client contexts released to run

   struct
   {
      RCEVT_THRESHOLD curr;                                                      // tasks defined this group

      RCEVT_THRESHOLD prev;                                                      // tasks defined previous group

   } def;

   // internal worker

   const rcinit_info_t** worker_argv;                                            // worker argv (coordinated by rcevt initfn)

   DALSYSEventHandle hEventWorkLoop;                                             // worker event

   DALSYSEventHandle hEventWorkLoopAck;                                          // worker event

   DALSYSWorkLoopHandle hWorkLoop;                                               // worker loop

   DALSYSEventHandle hEventInitFnSpawn;                                          // worker event

   // internal rcinit

   unsigned long stacks_size;                                                    // total stacks allotment (set after process_stacks) in KB

   unsigned char* allocs[RCINIT_GROUP_MAX];                                      // base to block of stacks allotment

   rcinit_stack_p stacks[RCINIT_GROUP_MAX];                                      // base to block of stacks allotment

   int tls_key;

   RCINIT_GROUP group_curr_idx;                                                  // current group index (used in loops)

   // internal instruments

   struct
   {
      rce_nde_p init_event[RCINIT_GROUP_MAX];                                    // event notification init

#if !defined(RCINIT_EXCLUDE_TERMINATION)                                         // term, size optimization when excluded
      rce_nde_p term_event[RCINIT_GROUP_MAX];                                    // event notification term
#endif

   } rcevt_handle;                                                               // event notification handles

};

extern const RCECB_NAME rcinit_init_rcecb[RCINIT_GROUP_MAX];                     // internal
extern const RCECB_NAME rcinit_term_rcecb[RCINIT_GROUP_MAX];                     // internal

extern const RCEVT_NAME rcinit_init_rcevt[RCINIT_GROUP_MAX];                     // internal
extern const RCEVT_NAME rcinit_term_rcevt[RCINIT_GROUP_MAX];                     // internal

extern const RCINIT_NAME rcinit_swe_name_init[RCINIT_GROUP_MAX];                 // internal
extern const RCINIT_NAME rcinit_swe_name_term[RCINIT_GROUP_MAX];                 // internal

extern unsigned long rcinit_term_tmo[RCINIT_GROUP_MAX];                          // internal

extern long rcinit_term_err_fatal;                                               // internal
extern long rcinit_term_timeout;                                                 // internal
extern long rcinit_term_latency_enable;                                          // internal

extern struct rcinit_internal_s rcinit_internal;                                 // internal, TRACE32 v.v rcinit_internal

RCXH_EXT(RCXH_RCINIT_TASK_END);
RCXH_EXT(RCXH_RCINIT_TASK_RESTART);

extern rcinit_info_t**** RCINIT_PLAYBOOK_MAP_GROUP;                              // t32: v.v RCINIT_PLAYBOOK_MAP_GROUP
extern rcinit_internal_name_map_t* RCINIT_PLAYBOOK_MAP_NAME;                     // t32: v.v RCINIT_PLAYBOOK_MAP_NAME

/////////////////////////////////////////////////////////////////////
// Internal Function Prototypes
/////////////////////////////////////////////////////////////////////

DALResult rcinit_task_dal(DALSYSEventHandle hEvent, void* arg_p);                // direct access, bootstrap only
void rcinit_task_rex(unsigned long);                                             // direct access, bootstrap only

void rcinit_init(void);
void rcinit_internal_process_groups(void);

void rcinit_term(void);
void rcinit_internal_unprocess_groups(void);

// Internal Handshake Instruments

unsigned long rcinit_internal_nmehash(RCINIT_NAME const name);
unsigned long rcinit_internal_task_name(char* task_name, int buflen);

void rcinit_internal_hs_list_init(void);
unsigned long rcinit_internal_hs_list_add(rcinit_internal_hs_p array, RCINIT_GROUP group, RCINIT_NAME task_name);
unsigned long rcinit_internal_hs_list_del(rcinit_internal_hs_p array, RCINIT_GROUP group, RCINIT_NAME task_name);
unsigned long rcinit_internal_hs_list_query(rcinit_internal_hs_p array, RCINIT_GROUP group, RCINIT_NAME task_name);
unsigned long rcinit_internal_hs_list_query_hs(rcinit_internal_hs_p array, RCINIT_GROUP group, RCINIT_NAME task_name);
unsigned long rcinit_internal_hs_list_query_bl(rcinit_internal_hs_p array, RCINIT_GROUP group, RCINIT_NAME task_name);
unsigned long rcinit_internal_hs_list_hs(rcinit_internal_hs_p array, RCINIT_GROUP group, RCINIT_NAME task_name);
unsigned long rcinit_internal_hs_list_bl(rcinit_internal_hs_p array, RCINIT_GROUP group, RCINIT_NAME task_name);

void rcinit_internal_devcfg_check_load(void);

unsigned long rcinit_term_err_fatal_set(unsigned long val);
unsigned long rcinit_term_latency_enable_set(unsigned long val);
unsigned long rcinit_term_timeout_group_get(RCINIT_GROUP group);
unsigned long rcinit_term_timeout_group_set(RCINIT_GROUP group, unsigned long timeout_ms);
unsigned long rcinit_term_timeout_get(void);
unsigned long rcinit_term_timeout_set(unsigned long timeout_ms);

void rcinit_internal_start_daltask(const rcinit_info_t* rcinit_p, void* entry);
void rcinit_internal_start_posix(const rcinit_info_t* rcinit_p, void* entry);
void rcinit_internal_start_qurttask(const rcinit_info_t* rcinit_p, void* entry);
void rcinit_internal_start_rextask(const rcinit_info_t* rcinit_p, void* entry);

RCINIT_GROUP rcinit_lookup_group_current(void);

void rcinit_internal_devcfg_check_load(void);

// TLS Binding to NONHLOS (INTERNAL)

int rcinit_internal_tls_create_key(int*, void (*)(void*));
int rcinit_internal_tls_set_specific(int, void*);
void* rcinit_internal_tls_get_specific(int);

// Internal Unit Test Functions

void rcinit_ut_blacklist(unsigned long list_count, RCINIT_NAME list[]);
void rcinit_ut_whitelist(unsigned long list_count, RCINIT_NAME list[]);

// Multi PD Driver Interfaces

RCINIT_BOOL rcinit_driver_init(void);
RCINIT_BOOL rcinit_driver_term(void);

#if defined(__cplusplus)
}
#endif

#endif
