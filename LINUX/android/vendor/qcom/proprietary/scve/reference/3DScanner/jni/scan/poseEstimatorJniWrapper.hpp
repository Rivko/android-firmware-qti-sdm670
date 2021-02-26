/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __POSE_ESTIMATOR_JNI_WRAPPER__
#define __POSE_ESTIMATOR_JNI_WRAPPER__

#include <jni.h>
#include "androidUtility.hpp"


// JNI functions
extern "C" {
    JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_init(JNIEnv * env, jobject obj,
                                                      jstring filesdir,
                                                      jstring externalFilesDir,
                                                      jobject assetManager);
   JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_initPreview(JNIEnv * env, jobject obj,
                                                           jstring filesdir,
                                                           jobject assetManager);
   JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_initDrawer(JNIEnv * env, jobject obj,
                                                           jint width, jint height,
                                                           jint hrCamWidth,jint hrCamHeight,
                                                           jint depthImgWidth, jint depthImgHeight,
                                                           jobject assetManager);
   JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_step(JNIEnv *env, jobject obj,
                                                      jboolean stopScan,
                                                      jboolean update);
   JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_stepPreview(JNIEnv *env, jobject obj);
   JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_destroyModules(JNIEnv * env, jobject obj);
   JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_destroyPreview(JNIEnv * env, jobject obj);
   JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_destroyDrawer(JNIEnv * env, jobject obj);
   JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_resume(JNIEnv * env, jobject obj);
   JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_stopScan(JNIEnv * env,
                                                                jobject obj, jfloat initPitch, jfloat initRoll);
   JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_saveModel(JNIEnv * env, jobject obj);
   // TSDF functions
   JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_tsdf(JNIEnv * env, jobject obj);
   // camera control functions
   JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_toggleAdjustButton(JNIEnv * env, jobject obj);
   JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_mouseButtonFunc(JNIEnv * env, jobject obj,
                                                   jint button,jint state,
                                                   jint x, jint y);
   JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_mousePosFunc(JNIEnv * env, jobject obj, jint button, jint x, jint y);
   // camera capturing functions
   JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_captureHRImage(JNIEnv * env, jobject obj,
                                                             jbyteArray  hrImg,
                                                             jint        hrImgWidth,
                                                             jint        hrImgHeight);

   JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_sendHighResKeyFrame(JNIEnv * env,
                                                                  jobject obj);
   // motion sensor
   //JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_getSensorReading(JNIEnv * env,
   JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_mmrd_sensorfusioncv_MotionSensorManager_getSensorReading(JNIEnv * env,
                                                                  jobject obj,
                                                                  jfloatArray gyro,
                                                                  jfloatArray acceleration,
                                                                  jlongArray  currTimeStamp,
                                                                  jlong      difUnixtime,
                                                                  jint size);
   // misc
   JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_setJNIConfiguration(JNIEnv * env,
                                                                    jobject obj,
                                                                    jbooleanArray vo_config,jstring TARGET, jint cannedDatasetType);

   //Exposure Control
   JNIEXPORT jintArray JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_ProcessAndroidImage(JNIEnv * env, jobject obj,
                                                                jbyteArray  src,
                                                                jint        ImgWidth,
                                                                jint        ImgHeight,
                                                                jint left,
                                                                jint right,
                                                                jint top,
                                                                jint bottom);

   JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_captureHRPreviewImage(JNIEnv * env, jobject obj,
                                                              jbyteArray  hrImg,
                                                              jint        hrImgWidth,
                                                             jint        hrImgHeight);

   JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_setDispFlag(JNIEnv * env,
                                                                    jobject obj,
                                                                    jboolean showImage);

   JNIEXPORT jint JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_getSelectedKeyFrameID(JNIEnv * env,
                                                                     jobject obj);
   JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_toggleRenderMode(JNIEnv * env,
                                                               jobject obj, jboolean is_splatting);

   JNIEXPORT jboolean JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_submitColorAndDepth(JNIEnv * env, jobject obj,
                   jbyteArray jImgDataColor, jint colorImgWidth, jint colorImgHeight,
                   jshortArray jImgDataDepth, jint depthImgWidth, jint depthImgHeight,
                   jlong timestamp, jlong syncTimestamp, jboolean isOrbbecSensor);

   JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_setViewerTargetAngles(JNIEnv * env,
   jobject obj, jfloat x, jfloat y);
   JNIEXPORT jfloat JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_getCameraData(JNIEnv * env, jobject obj, jfloatArray camera_matrix);
   JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_setBBSize(JNIEnv * env, jobject obj, jfloat size);
   JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_setBBCenter(JNIEnv * env, jobject obj, jfloat x, jfloat y, jfloat z);
   JNIEXPORT jboolean JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_getBoundingBox(JNIEnv * env, jobject obj, jfloatArray bb_info);

    JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_MainActivity_setJava(JNIEnv * env, jobject obj);
    JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_MainActivity_clearJava(JNIEnv * env, jobject obj);
    JNIEXPORT void JNICALL Java_com_qualcomm_qti_threedr_GL2JNILib_AbortScanTimeOut(JNIEnv * env, jobject obj);

    };
#endif
