/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <jni.h>
#include <GLES3/gl3.h>
#include "throw_exception.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Class:     com_qualcomm_qti_khronos_GLES30Ext
 * Method:    nGlReadPixels
 * Signature: (IIIIIII)V
 */
JNIEXPORT void JNICALL Java_com_qualcomm_qti_khronos_GLES30Ext_nGlReadPixels(JNIEnv *env, jclass cls,
        jint x, jint y, jint width, jint height, jint format, jint type, jint offset)
{
    GLint buffer;
    glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &buffer);
    if (buffer == 0) {
        throwException(env, RUNTIME_EXCEPTION, "This version of glReadPixels can only be used "
                "when a PBO is bound to GL_PIXEL_PACK_BUFFER");
        return;
    }

    glReadPixels((GLint)x, (GLint)y, (GLint)width, (GLint)height, (GLenum)format, (GLenum)type,
            (GLvoid *)offset);
}

#ifdef __cplusplus
}
#endif
