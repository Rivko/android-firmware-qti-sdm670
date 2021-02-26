/** vi: tw=128 ts=3 sw=3 et :
@file rcinit_tls.c
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
Copyright (c) 2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/rcinit/src/rcinit_tls.c#1 $
$Change: 16759227 $
$DateTime: 2018/07/30 01:21:36 $
$Author: pwbldsvc $
===============================================================================*/

#if !defined(RCINIT_EXCLUDE_KERNEL_QURT)

#define DLL_API_BUILD_RCINIT

#include "rcinit_qurt.h"
#include "tms_rcinit.h"

int rcinit_internal_tls_create_key(int* x, void (*y)(void*))
{
   int tls_key = qurt_tls_create_key(x, y);      // TLS API available (before worker)
   rcinit_internal_tls_available = RCINIT_TASK_TLS_AVAILABLE; 
   return tls_key;
}

int rcinit_internal_tls_set_specific(int x, void* y)
{
   return qurt_tls_set_specific(x, y);
}

void* rcinit_internal_tls_get_specific(int x)
{
   return qurt_tls_get_specific(x);
}

#elif !defined(RCINIT_EXCLUDE_KERNEL_POSIX)

#define DLL_API_BUILD_RCINIT

#include "rcinit_posix.h"
#include "tms_rcinit.h"

int rcinit_internal_tls_create_key(int* x, void (*y)(void*))
{
   return pthread_key_create((pthread_key_t*)x, y);
}

int rcinit_internal_tls_set_specific(int x, void* y)
{
   return pthread_setspecific((pthread_key_t)x, y);
}

void* rcinit_internal_tls_get_specific(int x)
{
   return pthread_getspecific((pthread_key_t)x);
}

#elif !defined(RCINIT_EXCLUDE_KERNEL_REX)

#define DLL_API_BUILD_RCINIT

#include "rcinit_rex.h"
#include "tms_rcinit.h"

int rcinit_internal_tls_create_key(int* x, void (*y)(void*))
{
   return rex_tls_create((rex_tls_key_t*)x, (rex_tls_destructor_t)y);
}

int rcinit_internal_tls_set_specific(int x, void* y)
{
   return rex_tls_setspecific((rex_tls_key_t)x, (rex_tls_value_t)y);
}

void* rcinit_internal_tls_get_specific(int x)
{
   void* rc;

   rex_tls_getspecific((rex_tls_key_t)x, (rex_tls_value_t*)&rc);

   return rc;
}

#elif !defined(RCINIT_EXCLUDE_KERNEL_DAL)

#define DLL_API_BUILD_RCINIT

#include "rcinit_dal.h"
#include "tms_rcinit.h"

int rcinit_internal_tls_create_key(int* x, void (*y)(void*))
{
   return -1;
}

int rcinit_internal_tls_set_specific(int x, void* y)
{
   return -1;
}

void* rcinit_internal_tls_get_specific(int x)
{
   return NULL;
}

#else

#error NHLOS INTERFACE NOT DEFINED PROPERLY

#endif
