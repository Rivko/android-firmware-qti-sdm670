/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * Expose the GL_OES_EGL_image extension to Java via JNI
 * This is intended to work with the android.opengl.GLES20 (or higher) class
 */

#include <jni.h>
#include <GLES2/gl2.h>
#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2ext.h>

static jfieldID gImage_EGLImageFieldID;

static inline GLeglImageOES getImage(JNIEnv *_env, jobject o)
{
    if (!o) return NULL;
    return (GLeglImageOES)_env->GetIntField(o, gImage_EGLImageFieldID);
}

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Class:     com_qualcomm_qti_qces_khronos_GLOESEGLImage
 * Method:    _nativeClassInit
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_qualcomm_qti_qces_khronos_GLOESEGLImage__1nativeClassInit(
        JNIEnv *_env, jclass clazz)
{
    jclass image_class = _env->FindClass("com/qualcomm/qti/qces/khronos/EGLImageKHR");
    gImage_EGLImageFieldID = _env->GetFieldID(image_class, "mEGLImage", "I");
}

/*
 * Class:     com_qualcomm_qti_qces_khronos_GLOESEGLImage
 * Method:    EGLImageTargetTexture2DOES
 * Signature: (ILcom/qualcomm/qti/qces/khronos/EGLImageKHR;)V
 */
JNIEXPORT void JNICALL Java_com_qualcomm_qti_qces_khronos_GLOESEGLImage_EGLImageTargetTexture2DOES(
        JNIEnv *_env, jclass clazz, jint target, jobject image)
{
    GLeglImageOES _image = getImage(_env, image);
    glEGLImageTargetTexture2DOES(target, _image);
}

/*
 * Class:     com_qualcomm_qti_qces_khronos_GLOESEGLImage
 * Method:    EGLImageTargetRenderbufferStorageOES
 * Signature: (ILcom/qualcomm/qti/qces/khronos/EGLImageKHR;)V
 */
JNIEXPORT void JNICALL Java_com_qualcomm_qti_qces_khronos_GLOESEGLImage_EGLImageTargetRenderbufferStorageOES(
        JNIEnv *_env, jclass clazz, jint target, jobject image)
{
    GLeglImageOES _image = getImage(_env, image);
    glEGLImageTargetRenderbufferStorageOES(target, _image);
}

#ifdef __cplusplus
}
#endif
