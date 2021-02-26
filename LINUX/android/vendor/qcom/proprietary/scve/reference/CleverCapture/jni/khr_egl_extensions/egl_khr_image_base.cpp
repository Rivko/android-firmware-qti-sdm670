/*
 * Copyright (c) 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
 * Expose the EGL_KHR_image_base extension to Java via JNI
 * This is intended to work with the android.opengl.EGL14 class
 */

#include <jni.h>
#include <EGL/egl.h>
#define EGL_EGLEXT_PROTOTYPES
#include <EGL/eglext.h>

static jmethodID gDisplay_GetHandle_V_I;
static jmethodID gContext_GetHandle_V_I;
static jfieldID gImage_EGLImageFieldID;

static inline EGLDisplay getDisplay(JNIEnv *_env, jobject o)
{
    if (!o) return EGL_NO_DISPLAY;
    return (EGLDisplay)_env->CallIntMethod(o, gDisplay_GetHandle_V_I);
}

static inline EGLContext getContext(JNIEnv *_env, jobject o)
{
    if (!o) return EGL_NO_CONTEXT;
    return (EGLContext)_env->CallIntMethod(o, gContext_GetHandle_V_I);
}

static inline EGLImageKHR getEGLImageKHR(JNIEnv *_env, jobject o)
{
    if (!o) return EGL_NO_IMAGE_KHR;
    return (EGLImageKHR)_env->GetIntField(o, gImage_EGLImageFieldID);
}

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Class:     com_qualcomm_qti_qces_khronos_EGLKHRImageBase
 * Method:    _nativeClassInit
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_qualcomm_qti_qces_khronos_EGLKHRImageBase__1nativeClassInit(
        JNIEnv *_env, jclass clazz)
{
    jclass display_class = _env->FindClass("android/opengl/EGLDisplay");
    gDisplay_GetHandle_V_I = _env->GetMethodID(display_class, "getHandle", "()I");

    jclass context_class = _env->FindClass("android/opengl/EGLContext");
    gContext_GetHandle_V_I = _env->GetMethodID(context_class, "getHandle", "()I");

    jclass image_class = _env->FindClass("com/qualcomm/qti/qces/khronos/EGLImageKHR");
    gImage_EGLImageFieldID = _env->GetFieldID(image_class, "mEGLImage", "I");
}

/*
 * Class:     com_qualcomm_qti_qces_khronos_EGLKHRImageBase
 * Method:    _eglCreateImageKHR
 * Signature: (Ljavax/microedition/khronos/egl/EGLDisplay;Ljavax/microedition/khronos/egl/EGLContext;II[I)I
 */
JNIEXPORT jint JNICALL Java_com_qualcomm_qti_qces_khronos_EGLKHRImageBase__1eglCreateImageKHR(
        JNIEnv *_env, jclass clazz, jobject dpy, jobject ctx, jint target, jint buffer, jintArray attrib_list)
{
    jint *attribs = NULL;
    jint image = (jint)EGL_NO_IMAGE_KHR;

    EGLDisplay _dpy = getDisplay(_env, dpy);
    EGLContext context = getContext(_env, ctx);

    if (attrib_list != NULL) {
        int num_attribs = _env->GetArrayLength(attrib_list);
        if (num_attribs == 0) {
            goto cleanup;
        }

        attribs = (jint *)_env->GetIntArrayElements(attrib_list, NULL);
        if (attribs[num_attribs - 1] != EGL_NONE) {
            goto cleanup;
        }
    }

    image = (jint)eglCreateImageKHR(_dpy, context, target,
            (EGLClientBuffer)buffer, attribs);

cleanup:
    if (attribs != NULL) {
        _env->ReleaseIntArrayElements(attrib_list, attribs, JNI_ABORT);
    }

    return image;
}

/*
 * Class:     com_qualcomm_qti_qces_khronos_EGLKHRImageBase
 * Method:    eglDestroyImageKHR
 * Signature: (Ljavax/microedition/khronos/egl/EGLDisplay;Lcom/qualcomm/qti/qces/khronos/EGLImageKHR;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_qualcomm_qti_qces_khronos_EGLKHRImageBase_eglDestroyImageKHR(
        JNIEnv *_env, jclass clazz, jobject dpy, jobject image)
{
    EGLDisplay _dpy = getDisplay(_env, dpy);
    EGLImageKHR _image = getEGLImageKHR(_env, image);
    return eglDestroyImageKHR(_dpy, _image);
}

#ifdef __cplusplus
}
#endif
