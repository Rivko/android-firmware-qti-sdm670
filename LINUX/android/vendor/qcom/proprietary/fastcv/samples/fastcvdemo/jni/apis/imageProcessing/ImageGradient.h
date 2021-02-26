#ifndef IMAGEGRADIENT_H
#define IMAGEGRADIENT_H

/*
 * Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 * @file Definition of FastCV Image Gradient Sample Java Native Interface (JNI).
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
  /// @brief Sets the orientation to be applied
  /// @param gradientOrientation Orientation type
  //------------------------------------------------------------------------------
  JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageProcessing_ImageGradient_setGradientOrientation
  (
    JNIEnv*  env,
    jobject  obj,
    int gradientOrientation
  );

  //------------------------------------------------------------------------------
  /// @brief Sets the gradient to be applied
  /// @param gradientType Gradient type
  //------------------------------------------------------------------------------
  JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageProcessing_ImageGradient_setGradientType
  (
    JNIEnv*  env,
    jobject  obj,
    int gradientType
  );

  //------------------------------------------------------------------------------
  /// @brief Calls FastCV API
  /// @param img Pointer to camera image
  /// @param w Width of image
  /// @param y Height of height
  //------------------------------------------------------------------------------
  JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageProcessing_ImageGradient_update
  (
    JNIEnv*     env,
    jobject     obj,
    jbyteArray  img,
    jint        w,
    jint        h
  );

  //---------------------------------------------------------------------------
  /// @brief Destroys the renderer
  //---------------------------------------------------------------------------
  JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageProcessing_ImageGradient_cleanup
  (
    JNIEnv * env,
    jobject obj
  );

}

#endif // IMAGEGRADIENT_H
