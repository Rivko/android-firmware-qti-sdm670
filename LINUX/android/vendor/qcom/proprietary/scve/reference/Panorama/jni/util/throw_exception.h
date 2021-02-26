/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <jni.h>

#ifndef THROW_EXCEPTION_H
#define THROW_EXCEPTION_H

#define RUNTIME_EXCEPTION           "java/lang/RuntimeException"
#define ILLEGAL_ARGUMENT_EXCEPTION  "java/lang/IllegalArgumentException"

/*
 * Helper method to raise a Java exception from native code. Make sure to return after calling this,
 * as raising a Java exception does not interrupt the thread until the native code returns.
 */
extern void throwException(JNIEnv *env, const char *exceptionClassName, const char *format, ...);

#endif // THROW_EXCEPTION_H
