#ifndef TMS_RCEVT_H
#define TMS_RCEVT_H
/** vi: tw=128 ts=3 sw=3 et :
@file tms_rcevt.h
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
Copyright (c) 2015-2016 by Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.qdsp6/2.1/debugtools/api/tms_rcevt.h#1 $
$DateTime: 2017/07/21 22:10:47 $
$Change: 13907704 $
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
#include "DALSys.h"
#include "timer.h"

#include "rcecb.h"
#include "rcesn.h"
#include "rcevt.h"

#include "err.h"

#if defined(__cplusplus)
extern "C"
{
#endif

/////////////////////////////////////////////////////////////////////
// Tuneable Controls
/////////////////////////////////////////////////////////////////////

#define RCEVT_NODE_POOL_SIZ      16                                              // node_pool storage allocation (minimum size)
#define RCEVT_NOTIFY_POOL_SIZ    RCEVT_NODE_POOL_SIZ                             // notify_pool storage allocation (minimum size)
#define RCEVT_HASHTABLE_BUCKETS  31                                              // hashtable buckets storage allocation (minimum size)
#define RCEVT_SIGEX_MAXSIZ       3                                               // maximum size of the sigex.parm[] union/struct, pointers (maximum size)

/////////////////////////////////////////////////////////////////////
// Localized Manifest Constants
/////////////////////////////////////////////////////////////////////

#define RCE_NULL                 ((void*)0)
#define RCE_THRESHOLD_INIT       (0)
#define RCE_THRESHOLD_DEFAULT    (1)
#define RCE_STATE_INIT           (0)
#define RCE_STATE_DEFAULT        (0)
#define RCE_NDE_COOKIE           (0xc00c1e00)                                    // cookie marker
#define RCE_NFY_COOKIE           (0xc00c1e11)                                    // cookie marker

/* Controls default, API is present for clients to specify their own timeout
*/
#if defined(RCINIT_SPI_NOR_TIMEOUTS)
#define RCECB_CALLBACK_MAX_TIME     30000                                        // maximum allocated callback execution time before ERR_FATAL (Unoptimized at 10s)
#else
#define RCECB_CALLBACK_MAX_TIME     10000                                        // maximum allocated callback execution time before ERR_FATAL (Unoptimized at 10s)
#endif
#define RCESN_CALLBACK_MAX_TIME     100                                          // maximum allocated callback execution time before ERR_FATAL
#define RCEVT_CALLBACK_MAX_TIME     100                                          // maximum allocated callback execution time before ERR_FATAL

/////////////////////////////////////////////////////////////////////
// Localized Type Declarations
/////////////////////////////////////////////////////////////////////

typedef struct
{
#if 1 // !defined(RCINIT_EXCLUDE_KERNEL_DAL)
   DALSYSSyncHandle mutex_dal;

#elif 0 // !defined(RCINIT_EXCLUDE_KERNEL_POSIX)
   pthread_mutex_t mutex_posix;

#elif 0 // !defined(RCINIT_EXCLUDE_KERNEL_QURT)
   qurt_mutex_t mutex_qurt;

#endif

} rce_mutex_t, * rce_mutex_p;

typedef unsigned long rce_sigex_t;

typedef unsigned long rce_hash_t;

typedef unsigned long rce_cookie_t;

typedef timer_type rce_expiry_t;

struct rce_nfy_sigex_s                                                           // sigex overlay storage based on type
{
   void* parm[RCEVT_SIGEX_MAXSIZ];                                               // reflects maximum size (pointers)
};

// RCE Internal Signaling Structure
/////////////////////////////////////////////////////////////////////

struct rce_nfy_s
{
   rce_sigex_t sigex_type;                                                       // sigex type storage

   rce_expiry_t expiry;                                                          // expiry timer

   RCEVT_THRESHOLD threshold;                                                    // threshold value (RCEVT ONLY)

   struct rce_nfy_sigex_s sigex;                                                 // sigex overlay storage based on type

   struct rce_nfy_s* next;                                                       // next in notify queue

   rce_cookie_t nfy_cookie;                                                      // init cookie
};

typedef struct rce_nfy_s rce_nfy_t, * rce_nfy_p;

// RCE Internal Event Structure
/////////////////////////////////////////////////////////////////////

struct rce_nde_s
{
   RCEVT_NAME name;                                                              // name text

   rce_expiry_t expiry;                                                          // expiry timer

   RCEVT_THRESHOLD threshold;                                                    // threshold value (RCEVT ONLY)

   RCESN_STATE state_curr;                                                       // state data current (RCESN ONLY)

   RCESN_STATE state_prev;                                                       // state data previous (RCESN ONLY)

   struct rce_nfy_s* notify_head;                                                // head notify queue

   struct rce_nde_s* next;                                                       // next node

   rce_mutex_t mutex;                                                            // mutex

   rce_cookie_t nde_cookie;                                                      // init cookie
};

typedef struct rce_nde_s rce_nde_t, * rce_nde_p;

// RCE Internal Signaling Structure Pool
/////////////////////////////////////////////////////////////////////

typedef struct rce_nfy_pool_s
{
   struct rce_nfy_s nfy_pool[RCEVT_NOTIFY_POOL_SIZ];

   struct rce_nfy_pool_s* next;                                                  // dynamic use; pointers to pool

} rce_nfy_pool_t, * rce_nfy_pool_p;                                              // storage type

// RCE Internal Event Structure Pool
/////////////////////////////////////////////////////////////////////

typedef struct rce_nde_pool_s
{
   struct rce_nde_s nde_pool[RCEVT_NODE_POOL_SIZ];

   struct rce_nde_pool_s* next;                                                  // dynamic use; pointers to pool

} rce_nde_pool_t, * rce_nde_pool_p;                                              // storage type

struct rce_internal_s
{
   rce_nde_pool_p nde_pool_head_p;                                               // dynamic storage tracking

   rce_nfy_pool_p nfy_pool_head_p;                                               // dynamic storage tracking

   rce_nfy_p nfy_pool_free_p;                                                    // next allocation head

   rce_nde_p nde_pool_free_p;                                                    // next allocation head

   rce_mutex_t mutex;                                                            // critical section mutex protects localized storage

   rce_mutex_t mutex_create;

   boolean dynamic_use;                                                          // allow dynamic pools expansion

   unsigned long init_flag;                                                      // service init

};

extern struct rce_internal_s rce_internal;                                       // rce internal instruments

extern rce_nfy_pool_t rce_nfy_pool_static;                                       // first pool static storage

extern rce_nde_pool_t rce_nde_pool_static;                                       // first pool static storage

extern rce_nde_p rce_hashtab[RCEVT_HASHTABLE_BUCKETS];                           // hashtable lookup buckets

/////////////////////////////////////////////////////////////////////
// Internal Forward Declarations
/////////////////////////////////////////////////////////////////////

#define rce_malloc(x)   (malloc(x))                                              // typecasts as accessor functions

#define nde2rcecb(x)    ((RCECB_HANDLE)x)                                        // typecasts as accessor functions
#define nde2rcesn(x)    ((RCESN_HANDLE)x)                                        // typecasts as accessor functions
#define nde2rcevt(x)    ((RCEVT_HANDLE)x)                                        // typecasts as accessor functions

#define rcecb2nde(x)    ((rce_nde_p)x)                                           // typecasts as accessor functions
#define rcesn2nde(x)    ((rce_nde_p)x)                                           // typecasts as accessor functions
#define rcevt2nde(x)    ((rce_nde_p)x)                                           // typecasts as accessor functions

rce_nde_p rce_nde_hashtab_get(rce_hash_t const hash);
void rce_nde_hashtab_put(rce_nde_p const nde_p, rce_hash_t const hash);
rce_hash_t rce_nmehash(RCEVT_NAME const name);
int rce_nmelen(RCEVT_NAME const name);
int rce_nmecmp(RCEVT_NAME const name_1, RCEVT_NAME const name_2, int len);
rce_nfy_p rce_nfy_pool_init(void);
rce_nfy_p rce_nfy_pool_alloc(void);
rce_nfy_p rce_nfy_pool_free(rce_nfy_p const nfy_p);
rce_nde_p rce_nde_pool_init(void);
rce_nde_p rce_nde_pool_alloc(void);
rce_nde_p rce_nde_pool_free(rce_nde_p const nde_p);
void rce_hashtab_init(void);
void rce_init(void);
void rce_term(void);

void rce_mutex_init_dal(rce_mutex_p mutex_p);
void rce_mutex_lock_dal(rce_mutex_p mutex_p);
void rce_mutex_unlock_dal(rce_mutex_p mutex_p);

void rce_mutex_init_posix(rce_mutex_p mutex_p);
void rce_mutex_lock_posix(rce_mutex_p mutex_p);
void rce_mutex_unlock_posix(rce_mutex_p mutex_p);

void rce_mutex_init_qurt(rce_mutex_p mutex_p);
void rce_mutex_lock_qurt(rce_mutex_p mutex_p);
void rce_mutex_unlock_qurt(rce_mutex_p mutex_p);

/////////////////////////////////////////////////////////////////////
// Internal Forward Declarations : Specialized, NOT PUBLIC BY DESIGN
/////////////////////////////////////////////////////////////////////

RCECB_HANDLE rcecb_signal_handle_nolocks(RCECB_HANDLE const handle);

RCECB_HANDLE rcecb_signal_name_nolocks(RCECB_NAME const name);

#if defined(__cplusplus)
}
#endif

#endif
