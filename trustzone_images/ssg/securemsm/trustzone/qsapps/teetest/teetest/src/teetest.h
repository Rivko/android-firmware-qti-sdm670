
/*===========================================================================
Copyright (c) 2015,2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
============================================================================*/

#ifndef __TEETEST_H
#define __TEETEST_H

#include "object.h"

#define TRUE                    1   /* Boolean true value. */
#define FALSE                   0   /* Boolean false value. */

/**
 * Return the *length* of an array, which is the number of *elements* it
 * contains.  This is not to be confused with the array's *size*, as
 * returned by sizeof, which is the number of *bytes* it occupies in memory.
 *
 * The macro aims to mitigate situations in which a pointer is accidentally
 * supplied instead of an array by evaluating to zero. "a == &a" is always
 * true for arrays, and not generally for pointers.
 */
#define ARRAY_SIZE(array)                     \
  ((void*) &(array) == (void*) (array)        \
   ? sizeof (array) / sizeof *(array)         \
   : 0)

#define TEE_INT_SGI(xx)        (xx)

#define USEROBJECT_INVOKE_OUTBOUND  (ObjectInvoke)1
#define USEROBJECT_INVOKE_NULL      (ObjectInvoke)0

#define TEST_OK(func)                                                     \
  if (ret == Object_OK)                                                   \
  {                                                                       \
    ret = func;                                                           \
  }                                                                       \

#define TEST_NOT_OK(func)                                                 \
  if (ret == Object_OK)                                                   \
  {                                                                       \
      ret = func;                                                         \
      if(ret != Object_OK)                                                \
      {                                                                   \
        ret = Object_OK;                                                  \
      }                                                                   \
  }                                                                       \

#define TEST_TRUE(b)                                                      \
  if (ret == Object_OK)                                                   \
  {                                                                       \
    ret = ((b)) ? Object_OK : Object_ERROR;                               \
  }

#define TEST_FALSE(b) TEST_TRUE(!(b))

#define TEST_IOPEN(MINKAPI, TEE_OUTBOUND_OBJECT)                          \
  TEST_OK(IEnv_open(TEE_RemoteEnv_Object,                                 \
                    C ## MINKAPI ## _UID,                                 \
                    &TEE_OUTBOUND_OBJECT))

#define TEST_ICLOSE(MINKAPI, TEE_OUTBOUND_OBJECT)                         \
  TEST_OK(I ## MINKAPI ## _release(TEE_OUTBOUND_OBJECT))

#define TEST_ENTER                                                        \
  int ret = Object_OK;                                                    \
  Object TEE_RemoteEnv_Object = CEnv_create(ENV_REMOTE);

#define TEST_EXIT                                                         \
  return ret;


typedef uint8_t   boolean;
typedef uintptr_t uintnt;      /* Unsigned Native Integer
                                  Aarch32: 32 bits
                                  Aarch64: 64 bits        */
typedef uintptr_t v_addr_t;
typedef uintptr_t p_addr_t;

void      tee_panic(void);
void      tee_int_enable(void);
uint32_t  tee_int_disable(void);
void      tee_int_restore(uint32_t      flags);


/* Flag to indicate an interrupt has been caught */
extern int           interrupt_exit;

typedef struct       tee_syscall_entry_s tee_syscall_entry_t;

#endif  /* __TEETEST_H */
