#ifndef FILTERCORR_H
#define FILTERCORR_H

/*
 * Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 * @file Definition of FastCV Filter Correlation Sample Java Native Interface (JNI).
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
  /// @brief Sets the kernel to be applied
  /// @param kernelType Kernel type
  //------------------------------------------------------------------------------
  JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageProcessing_FilterCorr_setKernelType
  (
    JNIEnv*  env,
    jobject  obj,
    int kernelType
  );


  //------------------------------------------------------------------------------
  /// @brief Calls FastCV API
  /// @param img Pointer to camera image
  /// @param w Width of image
  /// @param y Height of height
  //------------------------------------------------------------------------------
  JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageProcessing_FilterCorr_update
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
  JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageProcessing_FilterCorr_cleanup
  (
    JNIEnv * env,
    jobject obj
  );

}

#endif // FILTERCORR_H
