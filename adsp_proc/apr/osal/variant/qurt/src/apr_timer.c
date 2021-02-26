/*
  Copyright (c) 2009-2014, 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/apr.common/2.0/osal/variant/qurt/src/apr_timer.c#1 $
  $Author: pwbldsvc $
*/

/* TODO: The apr_timer.c is not yet implemented on the ADSP. The current
 * implementation simply allows compilation only.
 */

#include <stddef.h>
#include "DALSys.h"
#include "qurt.h"
#include "qurt_error.h"
#include "apr_errcodes.h"
#include "apr_timer.h"
#include "mmstd.h"
#include "timer.h"   
/* DIAG includes. */
#include "err.h"
#include "msg.h"
#include "apr_dbg_msg.h"

#define APR_TIMER_MASK_RESERVED ( 0x1 )
#define APR_TIMER_MASK_EXPIRED  ( 0x2 )
#define APR_TIMER_MASK_DESTROY  ( 0x4 )
#define APR_TIMER_MASK_EXIT     ( 0x8 )
#define APR_TIMER_THREAD_PRI_ORDER ( 50 )
#define APR_TIMER_MASK_THREAD_WAIT (APR_TIMER_MASK_EXPIRED|\
                                    APR_TIMER_MASK_DESTROY)
#define APR_TIMER_THREAD_STACK_SIZE ( 16384 )
#define APR_TIMER_THREAD_NAME_SIZE ( 15 + 1 ) /* Last byte reserved for NULL. */

#define EC_NZERO(func_call, cleanup)\
if(1){\
int qurt_status = QURT_EOK;\
if( 0 != (qurt_status = func_call)) {\
   APR_DBG_1(APR_DBG_ERROR, "func_call failed, error code ", qurt_status);\
   goto cleanup;\
  }\
}

typedef struct apr_timer_internal_t
{
  timer_type handle;
  qurt_anysignal_t  signal;
  unsigned int      mask;
  qurt_mutex_t      apr_timer_lock;
  apr_timer_fn_t timer_cb;
  void* client_token;
  bool_t            is_thread_active;
  struct
  {
    unsigned int handle;
    char_t name[ APR_TIMER_THREAD_NAME_SIZE ];
    uint8_t stack[ APR_TIMER_THREAD_STACK_SIZE ];
    qurt_thread_attr_t attr;
    qurt_thread_t tid;
    void* param;
    int32_t result;
  }timer_thread;
}
  apr_timer_internal_t;


static void apr_timer_wrapper (
  void* param
)
{
  apr_timer_internal_t* the_timer = ( ( apr_timer_internal_t* ) param );
  unsigned int ret_mask;

  if ( the_timer != NULL )
  {
      for(;;)
      {
        ret_mask = qurt_anysignal_wait(
                     &the_timer->signal, APR_TIMER_MASK_THREAD_WAIT);
        if( ret_mask&(APR_TIMER_MASK_DESTROY) )
        {
          the_timer->is_thread_active=FALSE;
          qurt_anysignal_clear(&the_timer->signal, APR_TIMER_MASK_THREAD_WAIT);
          qurt_thread_exit( 0 ); 
        }        
        qurt_anysignal_clear(&the_timer->signal, APR_TIMER_MASK_EXPIRED); /* be careful not to clear destroy flag*/
        the_timer->timer_cb( the_timer->client_token );
      }
   the_timer->is_thread_active=FALSE;
  }
  qurt_thread_exit( 0 );
}


APR_INTERNAL int32_t apr_timer_create (
  apr_timer_t* ret_timer,
  apr_timer_fn_t timer_cb,
  void* client_token
)
{
  apr_timer_internal_t* the_timer;
  DALResult dal_rc;
  timer_group_ptr group_ptr = &timer_non_defer_group;
  if ( ret_timer == NULL )
  {
    return APR_EBADPARAM;
  }

  dal_rc = DALSYS_Malloc( sizeof( apr_timer_internal_t ), ( ( void** ) &the_timer ) );
  if ( dal_rc != DAL_SUCCESS )
  {
    return APR_ENORESOURCE;
  }
  mmstd_memset( the_timer, 0, sizeof( apr_timer_internal_t ) );

  EC_NZERO(timer_def2( &the_timer->handle, group_ptr), cleanup);  
  qurt_anysignal_init(&the_timer->signal);
  the_timer->mask= APR_TIMER_MASK_EXIT;
  qurt_mutex_init(&the_timer->apr_timer_lock);

  the_timer->timer_cb = timer_cb;
  the_timer->client_token = client_token;

  *ret_timer = ( ( apr_timer_t* ) the_timer );

  return APR_EOK;
cleanup :
   DALSYS_Free( the_timer );
   return APR_EFAILED;
}

APR_INTERNAL int32_t apr_timer_destroy (
  apr_timer_t timer
)
{
  int thrd_join_status = 0;
  apr_timer_internal_t* the_timer = ( ( apr_timer_internal_t* ) timer );
  DALResult dal_rc;
  int qurt_rc = QURT_EOK;

  if ( the_timer == NULL )
  {
    return APR_EBADPARAM;
  }

  qurt_mutex_lock (&the_timer->apr_timer_lock);
  #ifdef modem_proc
  timer_clr(&the_timer->handle, T_NONE);
  #endif
  #ifdef adsp_proc
   timer_clr_64(&the_timer->handle, T_NONE);
  #endif

  if(the_timer->is_thread_active)
  {
    the_timer->mask = APR_TIMER_MASK_DESTROY;
    EC_NZERO(qurt_anysignal_set(&the_timer->signal, APR_TIMER_MASK_DESTROY), cleanup1); 
  }
  qurt_mutex_unlock (&the_timer->apr_timer_lock);

  /* wait for dispatcher thread to exit */
  qurt_rc = qurt_thread_join(the_timer->timer_thread.tid, &thrd_join_status);
  if( qurt_rc != QURT_ENOTHREAD && qurt_rc != QURT_EOK )
  {
	APR_DBG_1(APR_DBG_ERROR, "qurt_thread_join failed, error code ", qurt_rc);
    goto cleanup2;
  }

  /* no more threads using the_timer, safe to destroy now*/
  timer_undef(&the_timer->handle);
  qurt_anysignal_destroy(&the_timer->signal);
  qurt_mutex_destroy(&the_timer->apr_timer_lock);

  dal_rc = DALSYS_Free( the_timer );
  if ( dal_rc != DAL_SUCCESS ) goto cleanup2;

  return APR_EOK;

 cleanup1: 
  qurt_mutex_unlock (&the_timer->apr_timer_lock);
 cleanup2:
  return APR_EFAILED;
}

/*this function generates 9999 different thread names
*/
char* aprv2_timer_get_thread_name(uint32 timer_count)
{
  static char name[]=
               {'A','P','R','_','T','I','M','E','R','_','0','0','0','0','\0'};
  unsigned long temp;

  temp = 48+(timer_count%10);
  mmstd_memcpy(name+13, sizeof(name)-13,(char*)&temp, sizeof(char));

  temp = 48+((timer_count%100)/10);
  mmstd_memcpy(name+12, sizeof(name)-12, (char*)&temp, sizeof(char));

  temp = 48+((timer_count%1000)/100);
  mmstd_memcpy(name+11, sizeof(name)-11,(char*)&temp, sizeof(char));

  temp = 48+(timer_count/1000);
  mmstd_memcpy(name+10, sizeof(name)-10, (char*)&temp, sizeof(char));

  return name;  
}
static void apr_timer_interrupt_handler (
  int32 time_ms,
  timer_cb_data_type data
)
{
  apr_timer_internal_t* the_timer = ( ( apr_timer_internal_t* ) data );
  if ( the_timer != NULL )
  {
	 EC_NZERO(qurt_anysignal_set(&the_timer->signal, APR_TIMER_MASK_EXPIRED), cleanup); 
  }
  return ;
 cleanup:
  return ;
}  

APR_INTERNAL int32_t apr_timer_start (
  apr_timer_t timer,
  uint64_t time_ns
)
{
  static uint32 timer_count=0;
  apr_timer_internal_t* the_timer = ( ( apr_timer_internal_t* ) timer );
  typedef void (*timer_handler_func_t) (void *);
  void * pStack;
  void * pStackAlligned;
  unsigned int alligned_stack_size;
  uint64_t time_ms;
  time_ms = time_ns/1000000; 
  
  if ( the_timer == NULL )
  {
    return APR_EBADPARAM;
  }

  qurt_mutex_lock (&the_timer->apr_timer_lock);
  if(!the_timer->is_thread_active)
  {
    ( void ) mmstd_memset( 
       &the_timer->timer_thread, 0, sizeof( the_timer->timer_thread ) );

    ( void ) mmstd_strlcpy( 
       the_timer->timer_thread.name, 
       aprv2_timer_get_thread_name(timer_count++), 
       APR_TIMER_THREAD_NAME_SIZE-1 );

    if(timer_count>=10000)
      timer_count=0;  

    the_timer->timer_thread.param = the_timer;

    /* Align the stack pointer. */
    pStack = ( void* )( &the_timer->timer_thread.stack );
    pStackAlligned = ( void* )( (uint32_t)((uint32_t)pStack + 127) & (~127) );

    /* Calculate alligned stack size. */
    alligned_stack_size = 
      (unsigned int)(APR_TIMER_THREAD_STACK_SIZE-((uint32_t)pStackAlligned - (uint32_t)pStack));
    alligned_stack_size -= alligned_stack_size % 128;
  
    qurt_thread_attr_init (&the_timer->timer_thread.attr);
    qurt_thread_attr_set_stack_size( 
      &the_timer->timer_thread.attr, alligned_stack_size );
    qurt_thread_attr_set_stack_addr( 
      &the_timer->timer_thread.attr, pStackAlligned );
    qurt_thread_attr_set_priority ( 
      &the_timer->timer_thread.attr, APR_TIMER_THREAD_PRI_ORDER );
    qurt_thread_attr_set_name( 
      &the_timer->timer_thread.attr, the_timer->timer_thread.name );
  
    EC_NZERO( qurt_thread_create(
          &the_timer->timer_thread.tid,                 /* thread ID */
          &the_timer->timer_thread.attr,                /* thread attributes */
          (timer_handler_func_t)apr_timer_wrapper,/* thread entry function */
          (void *) the_timer                            /* thread argument */
       ), cleanup);

    the_timer->is_thread_active=TRUE;
  }

  EC_NZERO( timer_reg(&the_timer->handle, apr_timer_interrupt_handler,
             ( ( timer_cb_data_type ) timer ), ( ( uint32 ) ( time_ms ) ), 0), cleanup);
  qurt_mutex_unlock (&the_timer->apr_timer_lock);
  return APR_EOK;
  
 cleanup: 
  qurt_mutex_unlock (&the_timer->apr_timer_lock);	
  return APR_EFAILED;
}

APR_INTERNAL int32_t apr_timer_stop (
  apr_timer_t timer
)
{
  apr_timer_internal_t* the_timer = ( ( apr_timer_internal_t* ) timer );

  if ( the_timer == NULL )
  {
    return APR_EBADPARAM;
  }
   #ifdef modem_proc
  timer_clr(&the_timer->handle, T_NONE);
  #endif
  #ifdef adsp_proc
   timer_clr_64(&the_timer->handle, T_NONE);
  #endif
  return APR_EOK;
}


