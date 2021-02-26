#ifndef _PTHREAD_TYPES_H_
#define _PTHREAD_TYPES_H_

/*==========================================================================
 * FILE:         pthread_types.c
 *
 * SERVICES:     types usded in POSIX API interface
 *
 * DESCRIPTION:  POSIX API interface based upon POSIX 1003.1-2004
 *
 *               Copyright (c) 2016  by Qualcomm Technologies, Inc.  All Rights Reserved. QUALCOMM Proprietary and Confidential.

 *==========================================================================*/

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __GNUC__
#define restrict __restrict__
#else
#define restrict
#endif

#define _SSIZE_T

#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE    0
#endif

#define PTHREAD_MAX_THREADS          512

#define PTHREAD_NAME_LEN             16
#define PTHREAD_MIN_STACKSIZE        512 //4096
#define PTHREAD_MAX_STACKSIZE        1048576
#define PTHREAD_DEFAULT_STACKSIZE    16384

#define PTHREAD_MIN_PRIORITY         0
#define PTHREAD_MAX_PRIORITY         255
#define PTHREAD_DEFAULT_PRIORITY     1

/*Mutex initialization status*/
#define PTHREAD_MUTEX_ATTR_UNINITIALIZED    0
#define PTHREAD_MUTEX_ATTR_INITIALIZED      1

/*Conditional attributes initialization status*/
#define PTHREAD_COND_ATTR_UNINITIALIZED     0
#define PTHREAD_COND_ATTR_INITIALIZED       1

#define PTHREAD_DEFAULT_NAME                "Anonymous"

#ifdef CONFIG_PTHREAD_MUTEX_DEFAULT_TYPE_NORMAL
#define PTHREAD_MUTEX_DEFAULT        PTHREAD_MUTEX_NORMAL
#else
#define PTHREAD_MUTEX_DEFAULT        PTHREAD_MUTEX_RECURSIVE
#endif

#define PTHREAD_MUTEX_INITIALIZER    ((pthread_mutex_t) 0xFFFFFFFF)
                                      
#define PTHREAD_COND_INITIALIZER     ((pthread_cond_t) 0xFFFFFFFF)

/* mutex and cond_var shared */
#define PTHREAD_PROCESS_PRIVATE      0
#define PTHREAD_PROCESS_SHARED       1

/* mutex type */
#define PTHREAD_MUTEX_ERRORCHECK     0
#define PTHREAD_MUTEX_NORMAL         1
#define PTHREAD_MUTEX_RECURSIVE      2

/* mutex protocol */
#define PTHREAD_PRIO_NONE            0
#define PTHREAD_PRIO_INHERIT         1
#define PTHREAD_PRIO_PROTECT         2

#define PTHREAD_SPINLOCK_UNLOCKED    0
#define PTHREAD_SPINLOCK_LOCKED      1

#define PTHREAD_ONCE_INIT (0)

#define PTHREAD_MUTEX_OPAQUE //ToDo: amitkulk: debug

typedef signed int   ssize_t;
typedef long       off_t;

/*detatchstate of a pthread*/
#define PTHREAD_CREATE_JOINABLE             1
#define PTHREAD_CREATE_DETACHED             0
/*
 * Types and structure definitions
 *
 */
typedef unsigned int cpu_set_t;

typedef unsigned int pthread_t;

typedef struct pthread_attr_t
{
    void         *stackaddr;
    int          internal_stack; /* this flag==1 means the stack needs to be freed by posix */
    size_t       stacksize;
    int          priority;
    unsigned int timetest_id;    
    cpu_set_t    cpumask;
    char         name[PTHREAD_NAME_LEN];
    /* This flag indicates whether pthread lib should create thread contexts for other OSALs */
    /* This is used internally by POSIX and not available for general usage */
    int          ext_context;
    int          detachstate;
} pthread_attr_t;

//mutex attr
typedef struct pthread_mutexattr_t   pthread_mutexattr_t;
struct pthread_mutexattr_t
{
    int is_initialized;
    int type;
    int pshared;
    int protocol;
};

typedef unsigned int              pthread_mutex_t;

typedef unsigned int              pthread_spinlock_t;

typedef struct pthread_condattr_t
{
    int is_initialized;
    int pshared;
} pthread_condattr_t;

typedef unsigned int             pthread_cond_t;

typedef struct pthread_barrierattr_t
{
    int is_initialized;
    int pshared;
} pthread_barrierattr_t;

typedef unsigned int                pthread_barrier_t;

typedef int pthread_key_t;

typedef int pthread_once_t;


#ifdef __cplusplus
}
#endif

#endif /* _PTHERAD_TYPES_H_ */
