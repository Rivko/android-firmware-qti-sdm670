#pragma once
/*=============================================================================
  @file sns_osa.h

  Several Operating System Abstractions to be used by the sns_qsocket client
  library.

  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include <android/log.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/*=============================================================================
  Macros
  ===========================================================================*/

/* Log a string message */
#define SNS_LOG(prio, ...) do {\
  __android_log_print(ANDROID_LOG_##prio, "SNS_QSOCKET_TEST", __VA_ARGS__);\
  } while(false)

/* See assert() */
#define SNS_ASSERT(x) assert(x)

/* Allocate and free memory */
#define sns_malloc(x) malloc(x)
#define sns_free(x) free(x)

#ifndef ARR_SIZE
#define ARR_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))
#endif

#ifndef UNUSED_VAR
#define UNUSED_VAR(var) ((void)(var));
#endif

/*=============================================================================
  Type Definitions and Declarations
  ===========================================================================*/

typedef void* (*sns_osa_thread_func)(void *arg);

struct sns_osa_thread;
struct sns_osa_mutex;
struct sns_osa_sem;

/*=============================================================================
  Public Function Declarations
  ===========================================================================*/

int sns_osa_thread_create(struct sns_osa_thread **thread, sns_osa_thread_func func, void *arg);

int sns_osa_mutex_create(struct sns_osa_mutex **mutex);

int sns_osa_mutex_lock(struct sns_osa_mutex *mutex);

int sns_osa_mutex_unlock(struct sns_osa_mutex *mutex);

int sns_osa_mutex_destroy(struct sns_osa_mutex *mutex);

int sns_osa_sem_create(struct sns_osa_sem **sem);

int sns_osa_sem_post(struct sns_osa_sem *sem);

int sns_osa_sem_wait(struct sns_osa_sem *sem);

int sns_osa_sem_destroy(struct sns_osa_sem *sem);
