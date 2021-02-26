/*
 * Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#include <jni.h>
#include <cstdlib>
#include <cstdio>
#include "throw_exception.h"

void throwException(JNIEnv *env, const char *exceptionClassName, const char *format, ...)
{
    // Don't squelch an existing exception (if present)
    if (!env->ExceptionCheck()) {
        jclass exceptionClass = env->FindClass(exceptionClassName);
        if (env->ExceptionCheck()) return;

        va_list args;
        va_start(args, format);
        char buf[128];
        buf[0] = '\0';
        if (format != NULL) {
            vsnprintf(buf, 128, format, args);
        }
        env->ThrowNew(exceptionClass, buf);
        va_end(args);
    }
}
