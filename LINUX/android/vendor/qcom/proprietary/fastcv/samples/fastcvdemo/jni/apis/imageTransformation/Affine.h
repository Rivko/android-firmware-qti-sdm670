#ifndef AFFINE_H
#define AFFINE_H

/*
 * Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 *
 * @file Definition of FastCV Transform Affine Sample Java Native Interface (JNI).
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
  /// @brief Sets the affine type to be applied
  ///
  /// @param affineType  Affine function to apply to image
  //------------------------------------------------------------------------------
  JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageTransformation_Affine_setAffine
  (
    JNIEnv*  env,
    jobject  obj,
    int affineType
  );


  //------------------------------------------------------------------------------
  /// @brief Calls FastCV API
  /// @param img Pointer to camera image
  /// @param w Width of image
  /// @param y Height of height
  //------------------------------------------------------------------------------
  JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageTransformation_Affine_update
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
  JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageTransformation_Affine_cleanup
  (
    JNIEnv * env,
    jobject obj
  );

}

#endif // AFFINE_H
