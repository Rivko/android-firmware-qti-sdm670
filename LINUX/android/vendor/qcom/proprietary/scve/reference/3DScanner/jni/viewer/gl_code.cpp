/*
 * Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <jni.h>
#include <android/log.h>

#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define  LOG_TAG    "libgl2jni"


extern "C" {
    JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_MeshViewer_GLViewerJNILib_init(JNIEnv * env, jobject obj,  jint width, jint height, jstring directory, jstring modelName, int numTextures);
    JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_MeshViewer_GLViewerJNILib_step(JNIEnv * env, jobject obj);
    JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_MeshViewer_GLViewerJNILib_shutdown(JNIEnv * env, jobject obj);
    JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_MeshViewer_GLViewerJNILib_mouseButtonFunc(JNIEnv * env, jobject obj, jint button,jint state, jint x, jint y);
    JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_MeshViewer_GLViewerJNILib_mousePosFunc(JNIEnv * env, jobject obj, jint button, jint x, jint y);
    JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_MeshViewer_GLViewerJNILib_setMeshMode(JNIEnv * env, jobject obj, jint meshMode);
    JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_MeshViewer_GLViewerJNILib_setRotationAndDraw(JNIEnv * env, jobject obj, jfloat rotX, jfloat rotY);
    JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_MeshViewer_GLViewerJNILib_setViewer(JNIEnv * env, jobject obj, jint X, jint Y, jint width, jint height);
    JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_MeshViewer_GLViewerJNILib_lockRotationAngle(JNIEnv * env, jobject obj);
};

extern void Draw();
extern void ShutDown();
extern void Init(int width, int height, const char * pDirectory, const char * pModelName, int numTextures);
extern void MouseButtonCallback(int button, int state, int x, int y);
extern void MousePositionCallback(int button, int x, int y);
extern void SetMeshMode(int meshMode);
extern void SetRotationAndDraw(float rotX, float rotY);
extern void SetViewer(int X, int Y, int width, int height);
extern void LockRotationAngle();


JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_MeshViewer_GLViewerJNILib_init(JNIEnv * env, jobject obj, jint width, jint height, jstring directory, jstring modelName, jint numTextures)
{
    const char * pDirectory = env->GetStringUTFChars(directory, NULL);
    const char * pModelName = env->GetStringUTFChars(modelName, NULL);

    //Pass the location into the init Module
    Init(width, height, pDirectory, pModelName, numTextures);

    env->ReleaseStringUTFChars(directory, pDirectory);
    env->ReleaseStringUTFChars(modelName, pModelName);
}

JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_MeshViewer_GLViewerJNILib_step(JNIEnv * env, jobject obj)
{
    Draw();
}

JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_MeshViewer_GLViewerJNILib_shutdown(JNIEnv * env, jobject obj)
{
    ShutDown();
}


JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_MeshViewer_GLViewerJNILib_mouseButtonFunc(JNIEnv * env, jobject obj, jint button,jint state, jint x, jint y)
{
    MouseButtonCallback(button, state, x, y);
}


JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_MeshViewer_GLViewerJNILib_mousePosFunc(JNIEnv * env, jobject obj, jint button, jint x, jint y)
{
    MousePositionCallback(button, x,y);
}


JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_MeshViewer_GLViewerJNILib_setMeshMode(JNIEnv * env, jobject obj, jint meshMode)
{
    SetMeshMode(meshMode);
}

JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_MeshViewer_GLViewerJNILib_setRotationAndDraw(JNIEnv * env, jobject obj, jfloat rotX, jfloat rotY)
{
    SetRotationAndDraw(rotX, rotY);
}

JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_MeshViewer_GLViewerJNILib_setViewer(JNIEnv * env, jobject obj, jint X, jint Y, jint width, jint height)
{
    SetViewer(X, Y, width, height);
}

JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_MeshViewer_GLViewerJNILib_lockRotationAngle(JNIEnv * env, jobject obj)
{
    LockRotationAngle();
}
