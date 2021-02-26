#ifndef FAST_CV_LOADJPEG_SAMPLE_H
#define FAST_CV_LOADJPEG_SAMPLE_H

/*
 * Copyright (c) 2011 Qualcomm Technologies Incorporated.
 * All Rights Reserved
 * Qualcomm Technologies Confidential and Proprietary
 *
 *
 * @file Definition of FastCV Load JPEG Sample Java Native Interface (JNI).
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

   //---------------------------------------------------------------------------
   /// @brief
   ///   Converts the RGB565 image to luminance
   /// @param img
   ///   pointer to RGB565 buffer that is to be converted, must contain
   ///   w*h*2 bytes
   /// @param w
   ///    width of buffer
   /// @param h
   ///    height of buffer
   //---------------------------------------------------------------------------
   JNIEXPORT void JNICALL Java_com_qualcomm_loadjpeg_LoadJpeg_convertToLum
   (
      JNIEnv*     env,
      jobject     obj,
      jshortArray  img,
      jint        w,
      jint        h
   );

};

#endif // FAST_CV_LOADJPEG_SAMPLE_H
