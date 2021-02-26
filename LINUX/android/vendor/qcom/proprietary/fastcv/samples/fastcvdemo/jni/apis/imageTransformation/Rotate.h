#ifndef ROTATE_H
#define ROTATE_H

/*
 * Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 *
 * @file Definition of FastCV Copy Rotated Sample Java Native Interface (JNI).
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
  /// @brief Sets the rotation angle
  ///
  /// @param rotateAngle  Angle that image will be rotate
  //------------------------------------------------------------------------------
  JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageTransformation_Rotate_setRotation
  (
    JNIEnv*  env,
    jobject  obj,
    int rotateAngle
  );


  //------------------------------------------------------------------------------
  /// @brief Calls FastCV API
  /// @param img Pointer to camera image
  /// @param w Width of image
  /// @param y Height of height
  //------------------------------------------------------------------------------
  JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageTransformation_Rotate_update
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
  JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageTransformation_Rotate_cleanup
  (
    JNIEnv * env,
    jobject obj
  );

}

#endif // ROTATE_H

