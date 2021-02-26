#ifndef IMGDIFF_H
#define IMGDIFF_H

/*
 * Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 * @file Definition of FastCV Image Diff Sample Java Native Interface (JNI).
 */

//==============================================================================
// Include Files
//==============================================================================
#include <jni.h>

//==============================================================================
// Declarations
//==============================================================================
extern "C" {

    //---------------------------------------------------------------------------
    /// @brief Destroys the renderer
    //---------------------------------------------------------------------------
    JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageProcessing_ImgDiff_cleanup
    (
            JNIEnv * env,
            jobject obj
    );

   //------------------------------------------------------------------------------
   /// @param img Pointer to camera image
   /// @param w Width of image
   /// @param y Height of height
   //------------------------------------------------------------------------------
   JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageProcessing_ImgDiff_update
   (
      JNIEnv*     env,
      jobject     obj,
      jbyteArray  img,
      jint        w,
      jint        h
   );

   //---------------------------------------------------------------------------
   /// @brief Stores current frame as reference frame
   //---------------------------------------------------------------------------
   JNIEXPORT void JNICALL
       Java_com_qualcomm_fastcvdemo_apis_imageProcessing_ImgDiff_takeReferenceFrame();

   //---------------------------------------------------------------------------
   /// @brief Resets reference frame to NULL
   //---------------------------------------------------------------------------
   JNIEXPORT void JNICALL
       Java_com_qualcomm_fastcvdemo_apis_imageProcessing_ImgDiff_resetReferenceFrame();
};

#endif // IMGDIFF_H
