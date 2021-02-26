/*=========================================================================

                    GLink Core Driver DAL-OS abstraction

Implements the DAL based OS functions.

Copyright (c) 2014,2016 by QUALCOMM Technologies, Incorporated.  All Rights 
Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/04/14   bm      Initial version
===========================================================================*/


/*===========================================================================
                        INCLUDE FILES
===========================================================================*/

#include "comdef.h"
#include "glink_os_utils.h"
#include <string.h>

/*===========================================================================
                           MACRO DEFINITIONS
===========================================================================*/

#define OS_LOG_BUFFER_SIZE  ( 16384 )

uint32* glink_apcs_ipc_reg = NULL;

/*===========================================================================
                           TYPE DEFINITIONS
===========================================================================*/

/*===========================================================================
                              GLOBAL DATA DECLARATIONS
===========================================================================*/
/* ULOG handle */

/*===========================================================================
                    LOCAL FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
                    EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
os_timetick_type glink_os_get_current_timetick( void )
{
  return 0;
}

/*===========================================================================
  FUNCTION      glink_os_cs_init
===========================================================================*/
/**
  Initializes a Critical Section

  @param[in]  cs   pointer to critical section object allocated by caller.

  @return
  TRUE if critical section was initialized, FALSE otherwise
*/
/*=========================================================================*/
boolean glink_os_cs_init( os_cs_type *cs )
{
  /* No CS required for XBL as it is going to be in single threaded mode */
  return TRUE;
}

/*===========================================================================
  FUNCTION  glink_os_cs_deinit
===========================================================================*/
/**
  This function de-initializes a critical section.
 
  @param[in]  cs         Pointer to the critical section to be de-initialized.

  @return
  TRUE if critical section was initialized, FALSE otherwise

*/
/*=========================================================================*/
boolean glink_os_cs_deinit( os_cs_type *cs )
{
  return TRUE;
}

/*===========================================================================
FUNCTION      glink_os_cs_acquire
===========================================================================*/
/**
  Lock a critical section

  @param[in]  cs   Pointer the the critical section

  @return     None.
*/
/*=========================================================================*/
void glink_os_cs_acquire( os_cs_type *cs )
{
  return;
}

/*===========================================================================
FUNCTION      glink_os_cs_release
===========================================================================*/
/**
  Unlock a critical section

  @param[in]  cs   Pointer the the critical section

  @return     None.
*/
/*=========================================================================*/
void glink_os_cs_release( os_cs_type *cs )
{
  return;
}

/*===========================================================================
FUNCTION      glink_os_cs_create
===========================================================================*/
/**
  Create and initializesa Critical Section

  @return     The critical section.
*/
/*=========================================================================*/
os_cs_type *glink_os_cs_create( void )
{
  return (os_cs_type*)0xdeadbeef;
}

/*===========================================================================
FUNCTION      glink_os_cs_destroy
===========================================================================*/
/**
  Destroys a Critical Section

  @return     none.
*/
/*=========================================================================*/
void glink_os_cs_destroy( os_cs_type *cs )
{
  return;
}

/*===========================================================================
FUNCTION      glink_os_mutex_init
===========================================================================*/
/**
  Initializes a Critical Section that can yeild the processor during acquire
  operation if it(CS) is already acquired by a different thread

  @param[in]  cs   pointer to critical section object allocated by caller.

  @return
  TRUE if critical section was initialized, FALSE otherwise
*/
/*=========================================================================*/
boolean glink_os_mutex_init(os_mutex_type *cs)
{
  return TRUE;
}

/*===========================================================================
FUNCTION  glink_os_mutex_deinit
===========================================================================*/
/**
  This function de-initializes a critical section that can potentially sleep.

  @param[in]  cs         Pointer to the critical section to be de-initialized.

  @return
  TRUE if critical section was initialized, FALSE otherwise

*/
/*=========================================================================*/
boolean glink_os_mutex_deinit(os_mutex_type *cs)
{
  return TRUE;
}

/*===========================================================================
FUNCTION      glink_os_mutex_acquire
===========================================================================*/
/**
  Lock a critical section. The function can yeild the processor if the mutex 
  is already locked by another thread

  @param[in]  cs   Pointer the the critical section

  @return     None.
*/
/*=========================================================================*/
void glink_os_mutex_acquire(os_mutex_type *cs)
{
  return;
}

/*===========================================================================
FUNCTION      glink_os_mutex_release
===========================================================================*/
/**
  Unlock a critical section

  @param[in]  cs   Pointer the the critical section

  @return     None.
*/
/*=========================================================================*/
void glink_os_mutex_release(os_mutex_type *cs)
{
  return;
}

/*===========================================================================
  FUNCTION  glink_os_malloc
===========================================================================*/
/**
  Dynamically allocate a region of memory from the heap.  The region should be
  freed using \c glink_os_free when no longer used.

  @param[in]  size   The number of bytes to be allocated.

  @return    The pointer to the region of memory that was allocated.
             NULL if the allocation failed.
*/
/*=========================================================================*/
void *glink_os_malloc( size_t size )
{
  void *pMem;

  //return ( ( DAL_SUCCESS != DALSYS_Malloc( size, &pMem ) ) ? NULL : pMem );

   EFI_STATUS efiStatus;
   efiStatus = gBS->AllocatePool( EfiBootServicesData, size, &pMem );
   return (EFI_SUCCESS == efiStatus)? pMem: NULL;

  
}

/*===========================================================================
  FUNCTION  glink_os_calloc
===========================================================================*/
/**
  Dynamically allocate a region of memory from the heap and initialize with 
  the zeroes.  The region should be freed using \c glink_os_free 
  when no longer used.

  @param[in]  size   The number of bytes to be allocated.

  @return    The pointer to the region of memory that was allocated.
             NULL if the allocation failed.
*/
/*=========================================================================*/
void *glink_os_calloc( size_t size )
{
  void *pMem;

  EFI_STATUS efiStatus;
  efiStatus = gBS->AllocatePool( EfiBootServicesData, size, &pMem );
  if (EFI_SUCCESS != efiStatus)
  { 
    return NULL;
  }
  else
  {
    memset( pMem, 0, size );
    return pMem;
  }
}

/*===========================================================================
  FUNCTION  glink_os_free
===========================================================================*/
/**
  Free a region of memory that was allocated by \c glink_os_malloc.

  @param[in] pMem    The reference to the region of memory to be freed.

  @return    NA
*/
/*=========================================================================*/
void glink_os_free( void *pMem )
{
  //DALSYS_Free( pMem );
  gBS->FreePool( pMem );
}

/*===========================================================================
  FUNCTION  glink_os_string_copy
===========================================================================*/
/**
  Copies the source string into the destination buffer until 
  size is reached, or until a '\0' is encountered.  If valid,
  the destination string will always be NULL deliminated.
  
  @param[in] dst    The destination string, contents will be updated.
  @param[in] src    The source string
  @param[in] size   The maximum copy size (size of dst)

  @return
  The destination string pointer, dst.
*/
/*==========================================================================*/
char *glink_os_string_copy( char *dst, const char *src, uint32 size )
{
  unsigned int sz;

  if( size && dst && src )
  {

      sz = strlen(src) + 1;

      if(sz > size)
      {
        DALSYS_memcpy(dst, src, size-1);
        dst[size-1] = '\0';
      }
      else
      {
        DALSYS_memcpy(dst, src, sz);
      }
  } 
  else
  {
      dst = NULL;
  }

  return dst;
}

/*===========================================================================
  FUNCTION  glink_os_string_compare
===========================================================================*/
/**
  Compares two strings delimited by size or NULL character.
  
  @param[in] s1     String 1
  @param[in] s2     String 2

  @return
  0 if strings are identical (up to size characters), non-zero otherwise
*/
/*==========================================================================*/
int32 glink_os_string_compare( const char *s1, const char *s2 )
{
  return AsciiStrCmp( s1, s2);
}

/*===========================================================================
  FUNCTION  glink_os_string_cat
===========================================================================*/
/**
  Adds the source string to the destination string until 
  size is reached, or until a '\0' is encountered.  If valid,
  the destination string will always be NULL deliminated.
  
  @param[in] dst    The destination string, contents will be updated.
  @param[in] src    The source string
  @param[in] size   The maximum copy size (size of dst)

  @return
  The destination string pointer, dst.
*/
/*==========================================================================*/
char *glink_os_string_cat(char *dst, const char *src, uint32 size)
{
  int i, j;
  for(i = 0; i < size; i++)
  {
    if(dst[i] == '\0')
    {
      break;
    }
  }
  for(j = 0;i < size; i++, j++)
  {
    dst[i] = src[j];
    if(src[j] == '\0')
    {
      break;
    }
  }
  /* Just to be sure */
  dst[size - 1] = '\0';
  return dst;
}

/*===========================================================================
  FUNCTION  glink_os_copy_mem
===========================================================================*/
/**
  Copies the source buffer into the destination buffer.
  
  @param[in] dst    The destination, contents will be updated.
  @param[in] src    The source
  @param[in] size   The maximum copy size (size of dst)

  @return
  The destination string pointer, dst.
*/
/*==========================================================================*/
void glink_os_copy_mem( void *dst, const void *src, uint32 size )
{
  DALSYS_memscpy( dst, size, (void*)src, size );
}

/*===========================================================================
  FUNCTION  glink_os_atomic_increment_uint32
===========================================================================*/
/**
  Atomically increment an integer value by 1.

  @param[in/out] ploc    The reference to the location of the integer to be 
                         incremented.

  @return    Incremented value.
*/
/*=========================================================================*/
uint32 glink_os_atomic_increment_uint32( volatile uint32 *ploc )
{
  return (*ploc)++;
}

/*===========================================================================
  FUNCTION  glink_os_atomic_decrement_uint32
===========================================================================*/
/**
  Atomically decrement an integer value by 1.

  @param[in/out] ploc    The reference to the location of the integer to be 
                         decremented.

  @return    Decremented value.
*/
/*=========================================================================*/
uint32 glink_os_atomic_decrement_uint32( volatile uint32 *ploc )
{
  return (*ploc)--;
}

/*===========================================================================
FUNCTION      glink_os_atomic_exchange_uint32
===========================================================================*/
/**
  Swaps the provided value with the value at the given location

  @param[in/out]  plocation   Pointer to location holding the value
  @param[in]      new_value   New value to put at the location

  @return     The previous value at the locaiton.
*/
/*=========================================================================*/
uint32 glink_os_atomic_exchange_uint32( uint32 *plocation, uint32 new_value )
{
  uint32 prev_val = *plocation;
  *plocation = new_value;
  return prev_val;
}

/*===========================================================================
FUNCTION      glink_os_timer_init
===========================================================================*/
/**
  Initializes a timer

  @param[in/out]  ptimer    Pointer to os_timer data structure
  @param[in]      cb        Callback to be invoked with timer expiration
  @param[in]      cb_data   Data to be provided to the callback

  @return     TRUE if successfull, FALSE otherwise.
*/
/*=========================================================================*/
boolean glink_os_timer_init( os_timer *ptimer, os_timer_cb_fn cb, void* cb_data )
{
  return FALSE;
}

/*===========================================================================
FUNCTION      glni_os_timer_start
===========================================================================*/
/**
  Starts a timer with specified duration

  @param[in]  ptimer     Pointer to os_timer data structure
  @param[in]  duration   Timer expiry duration in ms

  @return     TRUE if successfull, FALSE otherwise.
*/
/*=========================================================================*/
boolean glni_os_timer_start( os_timer* ptimer, uint32 duration )
{
  return FALSE;
}

/*===========================================================================
FUNCTION      glink_os_timer_stop
===========================================================================*/
/**
  Stops timer

  @param[in]  ptimer   Pointer to os_timer data structure

  @return     none.
*/
/*=========================================================================*/
void glink_os_timer_stop( os_timer* ptimer )
{
  return;
}

/*===========================================================================
FUNCTION      glink_os_event_init
===========================================================================*/
/**
  Initializes the specified OS event. 
  Memory for the event allocated by the caller.

  @param[in]  event   Pointer the OS event

  @return     None.
*/
/*=========================================================================*/
void glink_os_event_init( os_event_type *event )
{
  return;
}

void glink_os_event_deinit(os_event_type *event)
{
  return;
}

/*===========================================================================
FUNCTION      glink_os_event_clear
===========================================================================*/
/**
  Clears the specified OS event

  @param[in]  event   Pointer the OS event

  @return     None.
*/
/*=========================================================================*/
void glink_os_event_clear( os_event_type *event )
{
  return;
}  

/*===========================================================================
FUNCTION      glink_os_event_wait
===========================================================================*/
/**
  Waits for the specified OS event

  @param[in]  event   Pointer the OS event

  @return     None.
*/
/*=========================================================================*/
void glink_os_event_wait( os_event_type *event )
{
  return;
}  

/*===========================================================================
  FUNCTION      glink_os_event_wait_timeout
===========================================================================*/
/**
  Waits for the specified OS event with timeout value

  @param[in]  event   Pointer the OS event
  @param[in]  time_ms Timeout value in ms

  @return     glink_err_type values.
*/
/*=========================================================================*/
glink_err_type glink_os_event_wait_timeout(os_event_type *event, uint32 time_ms)
{
  return GLINK_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION      glink_os_event_signal
===========================================================================*/
/**
  Signals the specified OS event

  @param[in]  event   Pointer the OS event

  @return     None.
*/
/*=========================================================================*/
void glink_os_event_signal( os_event_type *event )
{
  return;
}  

/*===========================================================================
  FUNCTION      glink_os_register_isr
===========================================================================*/
/**
  Registers ISR with the interrupt controller

  @param[in]  irq_in    Interrupt to register for
  @param[in]  isr       Callback to be invoked when interrupt fires
  @param[in]  cb_data   Data to be provided to the callback

  @return     TRUE if registration was successful, FALSE otherwise.
*/
/*=========================================================================*/
boolean glink_os_register_isr( uint32 irq_in, os_isr_cb_fn isr, void* cb_data )
{
 /* Do nothing */
 return TRUE;
}

/*===========================================================================
  FUNCTION      glink_os_deregister_isr
===========================================================================*/
/**
  De-registers ISR with the interrupt controller

  @param[in]  irq_in    Interrupt to deregister for

  @return     TRUE if de-registration was successful, FALSE otherwise.
*/
/*=========================================================================*/
boolean glink_os_deregister_isr( uint32 irq_in )
{
  return FALSE;
}

/*===========================================================================
  FUNCTION      glink_os_enable_interrupt
===========================================================================*/
/**
  Enables the interrupt in the interrupt controller

  @param[in]  irq_in    Interrupt to enable

  @return     TRUE if operation was successful, FALSE otherwise.
*/
/*=========================================================================*/
boolean glink_os_enable_interrupt( uint32 irq_in, os_isr_cb_fn isr, void* cb_data )
{
  return TRUE;
}

/*===========================================================================
  FUNCTION      glink_os_disable_interrupt
===========================================================================*/
/**
  Disables the interrupt in the interrupt controller

  @param[in]  irq_in    Interrupt to disable

  @return     TRUE if operation was successful, FALSE otherwise.
*/
/*=========================================================================*/
boolean glink_os_disable_interrupt( uint32 irq_in )
{
  return TRUE;
}
/*===========================================================================
FUNCTION      glink_os_send_interrupt
===========================================================================*/
/**
  Triggers an IPC interrupt

  @param[in]  irq_out   Interrupt descriptor

  @return     None.
*/
/*=========================================================================*/
void glink_os_send_interrupt( os_ipc_intr_type *intr )
{
  /* write the mask on the APCS_IPC register */
  *intr->intr_addr = intr->intr_mask;
}

boolean glink_os_log_init( void )
{
  return FALSE;
}

/*===========================================================================
FUNCTION      glink_os_thread_start
===========================================================================*/
/**
   OS has full freedom as to what to do in this function to start a thread,
   it should just end up calling the thread implementation provided as 
   the parameter.

  @param[in]  thread_fn   OS independent thread implementation
  @param[in]  param       Parameter for thread_fn
  @param[in]  thread_name String. In case OS cares for thread's name

  @return     None.
*/
/*=========================================================================*/
void *glink_os_thread_start
(
  glink_os_thread_fn_type thread_fn, 
  void *param, 
  const char* thread_name
)
{
  /* This should never be called in XBL */
  ASSERT(0);
  return NULL;
}

void glink_os_cache_invalidate(char *ptr, uint32 size)
{

}

void glink_os_cache_flush(char *ptr, uint32 size)
{

}

void glink_os_cache_prefetch( char *ptr, uint32 size )
{

}


void glink_os_dead_loop(void)
{
  for (;;) {}
}
