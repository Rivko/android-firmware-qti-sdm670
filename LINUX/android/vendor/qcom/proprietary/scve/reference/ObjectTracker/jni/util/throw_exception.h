/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#include <jni.h>

#ifndef THROW_EXCEPTION_H
#define THROW_EXCEPTION_H

#define RUNTIME_EXCEPTION       "java/lang/RuntimeException"

/*
 * Helper method to raise a Java exception from native code. Make sure to return after calling this,
 * as raising a Java exception does not interrupt the thread until the native code returns.
 */
extern void throwException(JNIEnv *env, const char *exceptionClassName, const char *format, ...);

#endif // THROW_EXCEPTION_H
