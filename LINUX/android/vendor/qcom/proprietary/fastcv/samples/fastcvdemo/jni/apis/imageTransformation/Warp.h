#ifndef WARP_H
#define WARP_H

/*
 * Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 *
 * @file Definition of FastCV Warp Sample Java Native Interface (JNI).
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
  /// @brief Sets the warp type
  ///
  /// @param warpType  Warp type to apply image
  //------------------------------------------------------------------------------
  JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageTransformation_Warp_setWarp
  (
    JNIEnv*  env,
    jobject  obj,
    int warpType
  );


  //------------------------------------------------------------------------------
  /// @brief Calls FastCV API
  /// @param img Pointer to camera image
  /// @param w Width of image
  /// @param y Height of height
  //------------------------------------------------------------------------------
  JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageTransformation_Warp_update
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
  JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageTransformation_Warp_cleanup
  (
    JNIEnv * env,
    jobject obj
  );

}

#endif // WARP_H
