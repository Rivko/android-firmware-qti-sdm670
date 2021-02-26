#ifndef FAST_CV_SAMPLE_H
#define FAST_CV_SAMPLE_H

/*
 * Copyright (c) 2010-2011 Qualcomm Technologies Incorporated.
 * All Rights Reserved
 * Qualcomm Technologies Confidential and Proprietary
 *
 *
 * @file Definition of FastCV Sample Java Native Interface (JNI).
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
   ///   Destroys the renderer
   //---------------------------------------------------------------------------
   JNIEXPORT void JNICALL Java_com_qualcomm_fastcorner_FastCVSample_cleanup
   (
      JNIEnv * env,
      jobject obj
   );

   //---------------------------------------------------------------------------
   /// @brief
   ///   Update the display
   /// @param img
   ///    pointer to buffer that is to be rendered
   /// @param w
   ///    width of buffer
   /// @param h
   ///    height of buffer
   //---------------------------------------------------------------------------
   JNIEXPORT void JNICALL Java_com_qualcomm_fastcorner_FastCVSampleRenderer_render
   (
      JNIEnv * env,
      jobject obj,
      jbyteArray img,
      jint w,
      jint h
   );

   //------------------------------------------------------------------------------
   /// @brief Calls FastCV API to find Corners
   ///        Additionally this function overlays pixels on the camera preview
   ///        image
   /// @param img Pointer to camera image
   /// @param w Width of image
   /// @param y Height of height
   //------------------------------------------------------------------------------
   JNIEXPORT void JNICALL Java_com_qualcomm_fastcorner_FastCVSample_update
   (
      JNIEnv*     env,
      jobject     obj,
      jbyteArray  img,
      jint        w,
      jint        h
   );

   //---------------------------------------------------------------------------
   /// @brief
   ///    Initializes the renderer
   //---------------------------------------------------------------------------
   JNIEXPORT void JNICALL Java_com_qualcomm_fastcorner_FastCVSample_init
   (
      JNIEnv* env,
      jobject obj
   );

   //------------------------------------------------------------------------------
   /// @brief Returns number of corners detected
   ///
   /// @return number of corners detected in current image
   //------------------------------------------------------------------------------
   JNIEXPORT int JNICALL Java_com_qualcomm_fastcorner_FastCVSample_getNumCorners
   (
      JNIEnv*  env,
      jobject  obj
   );

   //------------------------------------------------------------------------------
   /// @brief Gets the camera frame rate
   ///
   /// @return Camera FPS (frames per second)
   //------------------------------------------------------------------------------
   JNIEXPORT jfloat JNICALL Java_com_qualcomm_fastcorner_FastCVSample_getCameraFPS
   (
      JNIEnv*  env,
      jobject  obj
   );

   //------------------------------------------------------------------------------
   /// @brief Increments the camera frame tick count
   ///
   //------------------------------------------------------------------------------
   JNIEXPORT void JNICALL Java_com_qualcomm_fastcorner_FastCVSample_cameraFrameTick
   (
      JNIEnv*  env,
      jobject  obj
   );

   //------------------------------------------------------------------------------
   /// @brief Sets the scaling factor to be applied before applying FastCorner
   ///        detection on it
   ///
   /// @param scaleFactor  Scaling Factor by which to downsample image
   //------------------------------------------------------------------------------
   JNIEXPORT void JNICALL Java_com_qualcomm_fastcorner_FastCVSample_setScaling
   (
      JNIEnv*  env,
      jobject  obj,
      int scaleFactor
   );

   //------------------------------------------------------------------------------
   /// @brief Sets the threshold to be applied to FastCorner Detector algorithm
   ///
   /// @param threshold  Threshold to apply at corner detection algorithm
   //------------------------------------------------------------------------------
   JNIEXPORT void JNICALL Java_com_qualcomm_fastcorner_FastCVSample_setFastCornerThreshold
   (
      JNIEnv*  env,
      jobject  obj,
      int threshold
   );

   //------------------------------------------------------------------------------
   /// @brief Enables or disables viewfinder image preview
   ///
   /// @param disableViewFinder configuration parameter which enables or disables
   ///                             camera preview.
   ///                          enable - FALSE
   ///                          disable - TRUE
   //------------------------------------------------------------------------------
   JNIEXPORT void JNICALL Java_com_qualcomm_fastcorner_FastCVSample_setViewFinderEnable
   (
      JNIEnv*  env,
      jobject  obj,
      jboolean disableViewFinder
   );

   //------------------------------------------------------------------------------
   /// @brief Enables or disables overlay of green pixels where corners are
   ///        detected
   ///
   /// @param enableOverlayPixels configuration parameter which enables or disables
   ///                             corner pixel overlay
   ///                          no overlay pixels to be displayed - FALSE
   ///                          overlay pixels to be displayed - TRUE
   //------------------------------------------------------------------------------
   JNIEXPORT void JNICALL Java_com_qualcomm_fastcorner_FastCVSample_setOverlayPixelsEnable
   (
      JNIEnv*  env,
      jobject  obj,
      jboolean enableOverlayPixels
   );

   //------------------------------------------------------------------------------
   /// @brief Enables or disables gaussian filter to be applied to image
   ///        before having it processed through corner detector
   ///
   /// @param enableGaussian configuration parameter which enables or disables
   ///                       gaussian filter
   ///                        disable gaussian- FALSE
   ///                        enable gaussian - TRUE
   //------------------------------------------------------------------------------
   JNIEXPORT void JNICALL Java_com_qualcomm_fastcorner_FastCVSample_setGaussianEnable
   (
      JNIEnv*  env,
      jobject  obj,
      jboolean enableGaussian
   );

   //------------------------------------------------------------------------------
   /// @brief Retrieves FastCV processing timing from native layer.
   ///
   /// @return FastCV processing timing, filtered, in ms.
   //------------------------------------------------------------------------------
   JNIEXPORT jfloat JNICALL
      Java_com_qualcomm_fastcorner_FastCVSample_getFastCVProcessTime
   (
      JNIEnv*  env,
      jobject  obj
   );

};

#endif // FAST_CV_SAMPLE_H
