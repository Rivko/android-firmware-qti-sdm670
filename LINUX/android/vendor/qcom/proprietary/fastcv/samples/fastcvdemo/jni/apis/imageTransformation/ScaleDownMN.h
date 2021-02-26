#ifndef SCALEDOWNMN_H
#define SCALEDOWNMN_H

/*
 * Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 *
 * @file Definition of FastCV Transform ScaleDownMN Java Native Interface (JNI).
 *
 */

//==============================================================================
// Include Files
//==============================================================================
#include <jni.h>

//==============================================================================
// Declarations
//==============================================================================
extern "C" {
  //------------------------------------------------------------------------------
  /// @brief
  ///
  /// @param
  //------------------------------------------------------------------------------
  JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageTransformation_ScaleDownMN_setPreview
  (
    JNIEnv*  env,
    jobject  obj,
    bool pre
  );


  JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageTransformation_ScaleDownMN_setScaling
  (
    JNIEnv*  env,
    jobject  obj,
    int scale
  );

  //------------------------------------------------------------------------------
  /// @brief Calls FastCV API
  /// @param img Pointer to camera image
  /// @param w Width of image
  /// @param y Height of height
  //------------------------------------------------------------------------------
  JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageTransformation_ScaleDownMN_update
  (
    JNIEnv*     env,
    jobject     obj,
    jbyteArray  img,
    jint        w,
    jint        h
  );

  //---------------------------------------------------------------------------
  /// @brief
  ///   Destroys the renderer
  //---------------------------------------------------------------------------
  JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageTransformation_ScaleDownMN_cleanup
  (
    JNIEnv * env,
    jobject obj
  );

  JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageTransformation_ScaleDownMN_setScalePref
(
  JNIEnv*  env,
  jobject  obj,
  bool scaleP
  );

}

#endif // ScaleDownMN_H
