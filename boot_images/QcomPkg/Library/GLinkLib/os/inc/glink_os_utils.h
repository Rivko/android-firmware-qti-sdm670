#ifndef OS_UTILS_H
#define OS_UTILS_H

/*===========================================================================

                      G-Link OS Specific types/functions


 Copyright (c) 2014-2015 by QUALCOMM Technologies, Incorporated.  
 All Rights Reserved.
===========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/GLinkLib/os/inc/glink_os_utils.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/29/16   na      Added mutex api
04/25/16   sm      Added timeout for wait on remote intent
07/01/15   bc      Remove unnecessary header
04/06/15   an      Changes for enable/disable interrupt abstractions
03/24/15   an      string concatenation api
02/27/15   bc      Add get current timetick api
01/12/15   bc      Update glink_os_string_compare api
11/21/14   an      Added interrupt deregister, enable and disable
10/31/14   bm      Removing unused headers.
09/23/14   rs      Initial version
===========================================================================*/


/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include <limits.h>
#include "glink.h"
#include "glink_os_type.h"

/*===========================================================================
                        MACRO DEFINITIONS
===========================================================================*/

/*===========================================================================
                           TYPE DEFINITIONS
===========================================================================*/
typedef void (*glink_os_thread_fn_type)(void* param);

/*===========================================================================
                    EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION      glink_os_roundup64
===========================================================================*/
/**
  Rounds up the input value to the 8bytes(64bit) boundary which is greater 
  than or equal to the input value.
  This function does ERR_FATAL when integer overflows while doing roundup.

  @param[in]  val   Input number.

  @return
  64bit round up value.
*/
/*=========================================================================*/
static inline uint32 glink_os_roundup64(uint32 val)
{
  if (val > (UINT_MAX - 7))
  {
    ERR_FATAL("Integer overflow while doing roundup64(0x%X).", val, 0, 0);
  }
  return ((val + 7) & (~7));
}

/*===========================================================================
FUNCTION      glink_os_roundup32
===========================================================================*/
/**
  Rounds up the input value to the 4bytes(32bit) boundary which is greater 
  than or equal to the input value.
  This function does ERR_FATAL when integer overflows while doing roundup.

  @param[in]  val   Input number.

  @return
  32bit round up value.
*/
/*=========================================================================*/
static inline uint32 glink_os_roundup32(uint32 val)
{
  if (val > (UINT_MAX - 3))
  {
    ERR_FATAL("Integer overflow while doing roundup32(0x%X).", val, 0, 0);
  }
  return ((val + 3) & (~3));
}

/*===========================================================================
FUNCTION      glink_os_add
===========================================================================*/
/**
  Returns the sum of two inputs.
  This function does ERR_FATAL when integer overflows while doing addition.

  @param[in]  a   First input number.
  @param[in]  b   Second input number.

  @return
  Sum of inputs.
*/
/*=========================================================================*/
static inline uint32 glink_os_add(uint32 a, uint32 b)
{
  if (a > (UINT_MAX - b))
  {
    ERR_FATAL("Integer overflow while doing addition(0x%X, 0x%X).", a, b, 0);
  }
  return (a + b);
}

/*===========================================================================
FUNCTION      glink_os_fifo_index_check
===========================================================================*/
/**
  Checks index against FIFO size and alignment. 

  @param[in]  fifo_size   FIFO size.
  @param[in]  indx        FIFO index.
  @param[in]  align       Expected alignment of index. 
                            Alignment should be power of 2
                            If zero, no alignment check will be done.

  @return
  TRUE if index is valid, FALSE otherwise
*/
/*=========================================================================*/
static inline boolean glink_os_fifo_index_check
(
  uint32 fifo_size, 
  uint32 indx,
  uint32 align
)
{
  if (indx >= fifo_size)
  {
    return FALSE;
  }

  if (align)
  {
    /* Checking the align for power of 2 */
    ASSERT((align & (align - 1)) == 0);

    if (indx & (align - 1))
    {
      return FALSE;
    }
  }

  return TRUE;
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
boolean glink_os_cs_init( os_cs_type *cs );

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
boolean glink_os_mutex_init(os_mutex_type *cs);

/*===========================================================================
  FUNCTION      glink_os_cs_deinit
===========================================================================*/
/**
  Initializes a Critical Section

  @param[in]  cs   pointer to critical section object allocated by caller.

  @return
  TRUE if critical section was de-initialized, FALSE otherwise
*/
/*=========================================================================*/
boolean glink_os_cs_deinit( os_cs_type *cs );

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
boolean glink_os_mutex_deinit(os_mutex_type *cs);

/*===========================================================================
  FUNCTION      glink_os_cs_acquire
===========================================================================*/
/**
  Lock a critical section

  @param[in]  cs   Pointer the the critical section

  @return     None.
*/
/*=========================================================================*/
void glink_os_cs_acquire( os_cs_type *cs );

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
void glink_os_mutex_acquire(os_mutex_type *cs);

/*===========================================================================
  FUNCTION      glink_os_cs_release
===========================================================================*/
/**
  Unlock a critical section

  @param[in]  cs   Pointer the the critical section

  @return     None.
*/
/*=========================================================================*/
void glink_os_cs_release( os_cs_type *cs );

/*===========================================================================
FUNCTION      glink_os_mutex_release
===========================================================================*/
/**
  Unlock a critical section

  @param[in]  cs   Pointer the the critical section

  @return     None.
*/
/*=========================================================================*/
void glink_os_mutex_release(os_mutex_type *cs);

/*===========================================================================
  FUNCTION      glink_os_cs_create
===========================================================================*/
/**
  Create and initializesa Critical Section

  @return     The critical section.
*/
/*=========================================================================*/
os_cs_type *glink_os_cs_create( void );

/*===========================================================================
  FUNCTION      glink_os_cs_destroy
===========================================================================*/
/**
  Destroys a Critical Section

  @return     none.
*/
/*=========================================================================*/
void glink_os_cs_destroy( os_cs_type *cs );

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
void *glink_os_malloc( size_t size );

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
void *glink_os_calloc( size_t size );

/*===========================================================================
  FUNCTION  glink_os_free
===========================================================================*/
/**
  Free a region of memory that was allocated by \c glink_os_malloc.

  @param[in] pMem    The reference to the region of memory to be freed.

  @return    NA
*/
/*=========================================================================*/
void glink_os_free( void *pMem );

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
char *glink_os_string_copy(char *dst, const char *src, uint32 size);

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
char *glink_os_string_cat(char *dst, const char *src, uint32 size);

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
int32 glink_os_string_compare( const char *s1, const char *s2 );

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
void glink_os_copy_mem(void *dst, const void *src, uint32 size);

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
uint32 glink_os_atomic_increment_uint32( volatile uint32 *ploc );

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
uint32 glink_os_atomic_decrement_uint32( volatile uint32 *ploc );

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
uint32 glink_os_atomic_exchange_uint32( uint32* plocation, uint32 new_value );

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
boolean glink_os_timer_init( os_timer* ptimer, os_timer_cb_fn cb, void *cb_data );

/*===========================================================================
  FUNCTION      glink_os_timer_start
===========================================================================*/
/**
  Starts a timer with specified duration

  @param[in]  ptimer     Pointer to os_timer data structure
  @param[in]  duration   Timer expiry duration in ms

  @return     TRUE if successfull, FALSE otherwise.
*/
/*=========================================================================*/
boolean glink_os_timer_start( os_timer* ptimer, uint32 duration );

/*===========================================================================
  FUNCTION      glink_os_timer_stop
===========================================================================*/
/**
  Stops timer

  @param[in]  ptimer   Pointer to os_timer data structure

  @return     none.
*/
/*=========================================================================*/
void glink_os_timer_stop( os_timer *ptimer );

/*===========================================================================
  FUNCTION      glink_os_get_current_timetick
===========================================================================*/
/**
  Gets current timetick

  @return   64 bit timetick
*/
/*=========================================================================*/
os_timetick_type glink_os_get_current_timetick( void );

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
void glink_os_event_init( os_event_type *event );

/*===========================================================================
  FUNCTION      glink_os_event_deinit
===========================================================================*/
/**
  De-Initializes the specified OS event. 
  Memory for the event allocated by the caller.

  @param[in]  event   Pointer the OS event

  @return     None.
*/
/*=========================================================================*/
void glink_os_event_deinit( os_event_type *event );

/*===========================================================================
  FUNCTION      glink_os_event_clear
===========================================================================*/
/**
  Clears the specified OS event

  @param[in]  event   Pointer the OS event

  @return     None.
*/
/*=========================================================================*/
void glink_os_event_clear( os_event_type *event );

/*===========================================================================
  FUNCTION      glink_os_event_wait
===========================================================================*/
/**
  Waits for the specified OS event

  @param[in]  event   Pointer the OS event

  @return     None.
*/
/*=========================================================================*/
void glink_os_event_wait( os_event_type *event );

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
glink_err_type glink_os_event_wait_timeout(os_event_type *event, uint32 time_ms);

/*===========================================================================
  FUNCTION      glink_os_event_signal
===========================================================================*/
/**
  Signals the specified OS event

  @param[in]  event   Pointer the OS event

  @return     None.
*/
/*=========================================================================*/
void glink_os_event_signal( os_event_type *event );

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
boolean glink_os_register_isr( uint32 irq_in, os_isr_cb_fn isr, void *cb_data );

/*===========================================================================
  FUNCTION      glink_os_deregister_isr
===========================================================================*/
/**
  De-registers ISR with the interrupt controller

  @param[in]  irq_in    Interrupt to deregister for

  @return     TRUE if de-registration was successful, FALSE otherwise.
*/
/*=========================================================================*/
boolean glink_os_deregister_isr( uint32 irq_in );

/*===========================================================================
  FUNCTION      glink_os_enable_interrupt
===========================================================================*/
/**
  Enables the interrupt in the interrupt controller

  @param[in]  irq_in    Interrupt to enable
  @param[in]  isr       Callback to be invoked when interrupt fires
  @param[in]  cb_data   Data to be provided to the callback

  @return     TRUE if operation was successful, FALSE otherwise.
*/
/*=========================================================================*/
boolean glink_os_enable_interrupt( uint32 irq_in, os_isr_cb_fn isr, void* cb_data );

/*===========================================================================
  FUNCTION      glink_os_disable_interrupt
===========================================================================*/
/**
  Disables the interrupt in the interrupt controller

  @param[in]  irq_in    Interrupt to disable

  @return     TRUE if operation was successful, FALSE otherwise.
*/
/*=========================================================================*/
boolean glink_os_disable_interrupt( uint32 irq_in );

/*===========================================================================
  FUNCTION      glink_os_send_interrupt
===========================================================================*/
/**
  Triggers an IPC interrupt

  @param[in]  irq_out   Interrupt descriptor

  @return     None.
*/
/*=========================================================================*/
void glink_os_send_interrupt( os_ipc_intr_type *intr );

/*===========================================================================
  FUNCTION      glink_os_cache_invalidate
===========================================================================*/
/**
 * Invalidate data into Q6 L2 cache.
 *
 * @param[in]    start_addr  the address to begin the prefetch from
 * @param[in]    size        the number of bytes to prefetch
 *
 * @return       None
 */
void glink_os_cache_invalidate( char *ptr, uint32 size );

/*===========================================================================
  FUNCTION      glink_os_cache_prefetch
===========================================================================*/
/**
 * Prefetch data into Q6 L2 cache.
 *
 * It will issue only 1 prefetch instruction. If data size is larger than 
 * maximum supported size for 1 instruciton it would be fetched upto that 
 * maximum size from the end of data buffer. Due to cache alignment 
 * requirement size and address may be adjusted internally.
 *
 * @param[in]    start_addr  the address to begin the prefetch from
 * @param[in]    size        the number of bytes to prefetch
 *
 * @return       None
 */
void glink_os_cache_prefetch( char *ptr, uint32 size );

/*===========================================================================
  FUNCTION      glink_os_cache_flush
===========================================================================*/
/**
 * Writes back a block of memory from cache to memory
 *
 * If the cache is write-through this operation is usually a no-op.
 * But if the cache is write-back, then this operation should perform
 * a 'flush'.
 *
 * @param[in]    start_addr  the address to begin the flush from
 * @param[in]    size        the number of bytes to flush
 *
 * @return       None
 */
void glink_os_cache_flush( char *ptr, uint32 size );

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

  @return     Opaque pointer to thread info.
*/
/*=========================================================================*/
void *glink_os_thread_start
(
  glink_os_thread_fn_type thread_fn, 
  void                    *param, 
  const char              *thread_name
);

boolean glink_os_log_init( void );

#endif /* OS_UTILS_H */
