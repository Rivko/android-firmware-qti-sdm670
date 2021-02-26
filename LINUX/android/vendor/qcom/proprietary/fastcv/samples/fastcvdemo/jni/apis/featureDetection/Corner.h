#ifndef CORNER_H
#define CORNER_H

/*
 * Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 *
 * @file Definition of FastCV Corner Detection Sample Java Native Interface (JNI).
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
    JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_featureDetection_Corner_cleanup
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
   JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_util_FastCVRenderer_render
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
   JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_featureDetection_Corner_update
   (
      JNIEnv*     env,
      jobject     obj,
      jbyteArray  img,
      jint        w,
      jint        h
   );

   //------------------------------------------------------------------------------
   /// @brief Returns number of corners detected
   ///
   /// @return number of corners detected in current image
   //------------------------------------------------------------------------------
   JNIEXPORT int JNICALL Java_com_qualcomm_fastcvdemo_apis_featureDetection_Corner_getNumCorners
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
   JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_featureDetection_Corner_setScaling
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
   JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_featureDetection_Corner_setFastCornerThreshold
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
   JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_featureDetection_Corner_setViewFinderEnable
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
   JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_featureDetection_Corner_setOverlayPixelsEnable
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
   JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_featureDetection_Corner_setGaussianEnable
   (
      JNIEnv*  env,
      jobject  obj,
      jboolean enableGaussian
   );

   //------------------------------------------------------------------------------
   /// @brief Sets the scaling factor to be applied before applying FastCorner
   ///        detection on it
   ///
   /// @param detectionSwitch  Determines which detection method will be used
   //------------------------------------------------------------------------------
   JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_featureDetection_Corner_setDetection
   (
      JNIEnv*  env,
      jobject  obj,
      int detectionSwitch
   );

   //------------------------------------------------------------------------------
   /// @brief Returns a string representation of which detection function is
   ///        being used
   ///
   //------------------------------------------------------------------------------
   JNIEXPORT jstring JNICALL Java_com_qualcomm_fastcvdemo_apis_featureDetection_Corner_detectionMethod
   (
      JNIEnv*  env,
      jobject  obj
   );

   //------------------------------------------------------------------------------
   /// @brief Allocates memory for heavily used variables(bitmask, cornerscore)
   ///
   /// @param width Width of image
   /// @param height Height of height
   //------------------------------------------------------------------------------
   JNIEXPORT void
      JNICALL Java_com_qualcomm_fastcvdemo_apis_featureDetection_Corner_init
   (
      JNIEnv*  env,
      jobject  obj,
      int  width,
      int height
   );

   //------------------------------------------------------------------------------
   /// @brief Frees memory allocated by init
   ///
   //------------------------------------------------------------------------------
   JNIEXPORT void
      JNICALL Java_com_qualcomm_fastcvdemo_apis_featureDetection_Corner_deinit
   (
      JNIEnv*  env,
      jobject  obj
   );
};

#endif // CORNER_H
