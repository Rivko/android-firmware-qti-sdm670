/** vi: tw=128 ts=3 sw=3 et
@file gen_cb_ctxt.c
@brief This file contains the API details for the Protection Domain Monitor, API 2.0
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2017-18 by Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Confidential and Proprietary
===============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.qdsp6/2.1.c4/debugtools/gen_cb_ctxt/src/gen_cb_ctxt.c#1 $
$DateTime: 2018/07/30 01:21:36 $
$Change: 16759227 $
$Author: pwbldsvc $
===============================================================================*/

#include "com_dtypes.h"
#include "qurt.h"
#include "queue.h"
#include "gen_cb_ctxt.h"
#include "err.h"
#include "tms_dll_api.h"
#include <stdlib.h>

#define GEN_CB_CTXT_INVOKE_CBS 0x1
#define GEN_CB_CTXT_INVOKE_SPECIAL_CONTEXT 0x2

#define GEN_CB_CTXT_STACK_SIZE 2048
#define GEN_CB_CTXT_PRIO       0xCD
#define GEN_CB_CTXT_PRIO_LOW   0xFE

typedef struct 
{
  qurt_anysignal_t sig;
  q_type           queue;
  boolean          init;
  qurt_thread_attr_t tattr;
  gen_cb_ctxt_cb_t   special_context_cb;
  void *             special_context_arg;  
}gen_cb_ctxt_internal_s;

static gen_cb_ctxt_internal_s gen_cb_ctxt_internal;

typedef struct
{
  /* q_link_type - Must be the first element */
  q_link_type        node;
  gen_cb_ctxt_cb_t   cb;
  void *             arg;
}gen_cb_ctxt_q_data_s;

/**
Function corresponding gen_cb_ctxt task. It is used to invoke the callbacks
@return
None
*/

DLL_API_GLOBAL void gen_cb_ctxt(void * ignore)
{
  unsigned int    sigs;
  gen_cb_ctxt_q_data_s * data;
  
  for(;;)
  {
    /* Wait for some task signal */
    sigs = qurt_anysignal_wait( &gen_cb_ctxt_internal.sig,
		                 GEN_CB_CTXT_INVOKE_CBS|
                                 GEN_CB_CTXT_INVOKE_SPECIAL_CONTEXT );
    
    if ( sigs & GEN_CB_CTXT_INVOKE_SPECIAL_CONTEXT )
    {
      qurt_anysignal_clear( &gen_cb_ctxt_internal.sig, GEN_CB_CTXT_INVOKE_SPECIAL_CONTEXT );
         
      /* Process special context callback */
      gen_cb_ctxt_internal.special_context_cb(gen_cb_ctxt_internal.special_context_arg);
      
    }
    
    if ( sigs & GEN_CB_CTXT_INVOKE_CBS )
    {
      qurt_anysignal_clear( &gen_cb_ctxt_internal.sig, GEN_CB_CTXT_INVOKE_CBS );

      /* Process the callback */
      /* Get from the queue */
      data = q_get(&(gen_cb_ctxt_internal.queue) );

      while ( data != NULL )
      {
        data->cb( data->arg);
        free(data);
        data = q_get(&(gen_cb_ctxt_internal.queue) );
      }
    }
    
  }
}

/**
Initializes the various data strcutures related to gen_cb_ctxt module 
@return
None
*/


DLL_API_GLOBAL void gen_cb_ctxt_init(void)
{
  qurt_thread_t       tid;
  int * stackbase;
   
  if ( gen_cb_ctxt_internal.init != TRUE )
  {
    gen_cb_ctxt_internal.init = TRUE;
    q_init(&gen_cb_ctxt_internal.queue);
    qurt_anysignal_init(&gen_cb_ctxt_internal.sig);

    stackbase = calloc(1, GEN_CB_CTXT_STACK_SIZE);

    if ( stackbase == NULL )
    {
      ERR_FATAL("Gen CB Task",0,0,0 );
    }
  
    qurt_thread_attr_init (&gen_cb_ctxt_internal.tattr);
    qurt_thread_attr_set_stack_size (&gen_cb_ctxt_internal.tattr, (GEN_CB_CTXT_STACK_SIZE));
    qurt_thread_attr_set_stack_addr (&gen_cb_ctxt_internal.tattr, (void*)stackbase );
    qurt_thread_attr_set_priority (&gen_cb_ctxt_internal.tattr, GEN_CB_CTXT_PRIO);
    qurt_thread_attr_set_name(&gen_cb_ctxt_internal.tattr, "gen_cb_ctxt");
    if ( QURT_EOK != qurt_thread_create(&tid, &gen_cb_ctxt_internal.tattr, gen_cb_ctxt , NULL))
    {
      ERR_FATAL("Gen CB Task",0,0,0 );
    }
  }
}


/**
Invokes the callback and the argument as specified by the argument 

@return
None
*/


DLL_API_GLOBAL void gen_cb_ctxt_invoke_cb( gen_cb_ctxt_cb_t cb, void * arg )
{
  gen_cb_ctxt_q_data_s * data = malloc(sizeof(gen_cb_ctxt_q_data_s));

  if ( data == NULL )
  {
    return;
  }
  
  data->cb = cb;
  data->arg = arg;
    
  /* Insert in the queue */
  (void) q_link( data , &(data->node) );
  q_put(&(gen_cb_ctxt_internal.queue), 
        &(data->node) );

  (void)qurt_anysignal_set(&gen_cb_ctxt_internal.sig, GEN_CB_CTXT_INVOKE_CBS);
}


boolean gen_cb_ctxt_special_context( gen_cb_ctxt_cb_t cb, void * arg )
{
  if ( gen_cb_ctxt_internal.special_context_cb != NULL )
     return FALSE;
  
  gen_cb_ctxt_internal.special_context_cb = cb;
  gen_cb_ctxt_internal.special_context_arg = arg;

  /* Set priority to lowest ( SLEEP_PRI_ORDER ) for the special context */
  qurt_thread_attr_set_priority( &gen_cb_ctxt_internal.tattr, GEN_CB_CTXT_PRIO_LOW);

  /* Set Signal for sepcial context */
  (void)qurt_anysignal_set(&gen_cb_ctxt_internal.sig, GEN_CB_CTXT_INVOKE_SPECIAL_CONTEXT);

  return TRUE;
}
