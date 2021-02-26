#ifndef FAST_CV_SAMPLE_RENDERER_H
#define FAST_CV_SAMPLE_RENDERER_H

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
   ///   Sets up the viewport
   /// @param width
   ///    width of viewport
   /// @param height
   ///    height of viewport
   //---------------------------------------------------------------------------
   JNIEXPORT void JNICALL
      Java_com_qualcomm_fastcvdemo_utils_FastCVSampleRenderer_surfaceChanged
   (
      JNIEnv* env,
      jobject obj,
      jint    width,
      jint    height
   );

   //---------------------------------------------------------------------------
   /// @brief
   ///   Cleans up any memory and state associated with the renderer.
   //---------------------------------------------------------------------------
   JNIEXPORT void JNICALL
      Java_com_qualcomm_fastcvdemo_utils_FastCVSampleRenderer_cleanup
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
   JNIEXPORT void JNICALL
      Java_com_qualcomm_fastcvdemo_utils_FastCVSampleRenderer_render
   (
      JNIEnv * env,
      jobject obj,
      jbyteArray img,
      jint w,
      jint h
   );

   //------------------------------------------------------------------------------
   /// @brief Gets the rendering frame rate
   ///
   /// @return Rendering FPS (frames per second)
   //------------------------------------------------------------------------------
   JNIEXPORT jfloat JNICALL
      Java_com_qualcomm_fastcvdemo_utils_FastCVSampleRenderer_getGPUFPS
   (
      JNIEnv*  env,
      jobject  obj
   );

   //---------------------------------------------------------------------------
   /// @brief
   ///   Retrieves a pointer to the renderer's rendering buffer, will create
   ///   a new buffer if not yet allocated or size changes. Locks buffer so
   ///   don't call lock before this.
   ///
   /// @param w Width of image to render.
   /// @param h Height of image to render.
   ///
   /// @return Pointer to buffer.
   //---------------------------------------------------------------------------
   uint8_t* getRenderBuffer( uint32_t w, uint32_t h );

   //---------------------------------------------------------------------------
   /// @brief
   ///   Returns the size of the render buffer for the buffer's format.
   ///
   /// @return Size in bytes of the render buffer.
   //---------------------------------------------------------------------------
   uint32_t getRenderBufferSize();

   //---------------------------------------------------------------------------
   /// @brief
   ///   Unlocks render buffer for use.
   ///
   //---------------------------------------------------------------------------
   void unlockRenderBuffer();

   //---------------------------------------------------------------------------
   /// @brief
   ///   Locks render buffer to prevent processing collisions and data
   ///   corruption.
   //---------------------------------------------------------------------------
   void lockRenderBuffer();

   //---------------------------------------------------------------------------
   /// @brief
   ///   Draws specified corners on the render buffer.
   ///
   /// @param corners pointer to corner data stored as unsigned int's
   ///                x0,y0,x1,y1....
   /// @param numCorners Number of corners found in corners.
   ///
   //---------------------------------------------------------------------------
   void drawCorners( uint32_t* corners, uint32_t numCorners );

   void drawColorCorners( uint32_t* corners, uint32_t numCorners, uint32_t colorByte1, uint32_t colorByte2 );

   void drawScoreCorners( uint32_t* corners, uint32_t numCorners, uint32_t* scores );

   void drawEdges (uint8_t* edgeBuffer, uint32_t edgeHeight, uint32_t edgeWidth);

   void colorConvertYUV420ToRGB565Renderer( const uint8_t* srcYUV420,
                                            unsigned int   srcWidth,
                                            unsigned int   srcHeight,
                                            uint32_t*      dstRGB565 );
};

#endif // FAST_CV_SAMPLE_H
