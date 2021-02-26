#ifndef __OSAL_OS_H__
#define __OSAL_OS_H__
/***********************************************************************
 * osal_os.h
 *
 * Copyright (C) 2011, 2014 QUALCOMM Technologies, Inc.
 *
 * This file header file includes all os-dependent header files.
 * 
 **********************************************************************/

/*===========================================================================


                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: 

when         who   what, where, why
----------   ---   ---------------------------------------------------------
07/06/14     ph   Added support for pthread thread local storage  
09/15/11     sg   Created File 
===========================================================================*/

#include "comdef.h"
#include "osal_qurt_types.h"

/* The lock and unlock mutex APIs are function calls in WM, qube, qurt
 and map to macros in rex */
/*===========================================================================
  FUNCTION   OSAL_LOCK_MUTEX

  DESCRIPTION
       Locks a mutex

  PARAMETERS
       mutex - Address of Mutex object
  
  DEPENDENCIES
       None
    
   RETURN VALUE
       OSAL_SUCCESS 0        
       OSAL_FAILURE 1    
       OSAL_NULL_PTR_RECVD 2 
           
  SIDE EFFECTS
       None
  ===========================================================================*/
  void osal_lock_mutex(osal_mutex_arg_t *mutex);
  
/*===========================================================================
  FUNCTION   OSAL_UNLOCK_MUTEX

  DESCRIPTION
       Unlocks the mutex
    
  PARAMETERS
       mutex - Address of Mutex object
  
  DEPENDENCIES
       None

   RETURN VALUE
       OSAL_SUCCESS 0        
       OSAL_FAILURE 1    
       OSAL_NULL_PTR_RECVD 2 
           
  SIDE EFFECTS
       None
  ===========================================================================*/
  void osal_unlock_mutex(osal_mutex_arg_t *mutex);
  int osal_atomic_compare_and_set(osal_atomic_word_t *target,
                       osal_atomic_plain_word_t old_val,
                       osal_atomic_plain_word_t new_val );

void osal_atomic_set_bit(osal_atomic_word_t *target, unsigned long bit);

void osal_atomic_clear_bit(osal_atomic_word_t *target, unsigned long bit);

void osal_atomic_add(osal_atomic_word_t *target, osal_atomic_plain_word_t v);


/*===========================================================================
  FUNCTION   OSAL_PTHREAD_KEY_CREATE

  DESCRIPTION
       Creates a key for accessing a thread local storage data item.
    
  PARAMETERS
       key - Pointer to the newly-created thread local storage key value
       destructor - Pointer to the key specific destructor function. Passing NULL
	                specifies that no destructor function is defined for the key.
  
  DEPENDENCIES
       None

   RETURN VALUE
       OSAL_SUCCESS 0        
       OSAL_FAILURE 1    
           
  SIDE EFFECTS
       None
  ===========================================================================*/
int osal_pthread_key_create(osal_tls_key_t *key, void (*destructor)(void*));


/*===========================================================================
  FUNCTION   OSAL_PTHREAD_KEY_DELETE

  DESCRIPTION
       Deletes the specified key from thread local storage
    
  PARAMETERS
       key - Thread local storage key to delete
  
  DEPENDENCIES
       None

   RETURN VALUE
       OSAL_SUCCESS 0        
       OSAL_FAILURE 1    
           
  SIDE EFFECTS
       None
  ===========================================================================*/
int osal_pthread_key_delete(osal_tls_key_t key);


/*===========================================================================
  FUNCTION   OSAL_PTHREAD_GETSPECIFIC

  DESCRIPTION
       Loads the data item from thread local storage.
       Returns the data item that is stored in thread local storage with the specified key.
       The data item is always a pointer to user data.
    
  PARAMETERS
       key - Thread local storage key value.
  
  DEPENDENCIES
       Key must be initialized using osal_pthread_key_create.

   RETURN VALUE
       Pointer - Data item indexed by key in thread local storage
       If no thread-specific data value is associated with key, then the value NULL
	   shall be returned.
           
  SIDE EFFECTS
       None
  ===========================================================================*/
void *osal_pthread_getspecific(osal_tls_key_t key);


/*===========================================================================
  FUNCTION   OSAL_PTHREAD_SETSPECIFIC

  DESCRIPTION
       Stores a data item to thread local storage along with the specified key.

  PARAMETERS
       key -  Thread local storage key value.
       value - Pointer to user data value to store.
  
  DEPENDENCIES
       Key must be initialized using osal_pthread_key_create.

   RETURN VALUE
       OSAL_SUCCESS 0        
       OSAL_FAILURE 1    
           
  SIDE EFFECTS
       None
===========================================================================*/
int osal_pthread_setspecific(osal_tls_key_t key, const void* value);

#define osal_disable_interrupts()
#define osal_enable_interrupts()
  
#endif /* not __OSAL_OS_H__ */

