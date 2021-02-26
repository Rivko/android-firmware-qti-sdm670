/*
 * Copyright (c) 2010-2011, 2014 Qualcomm Technologies Incorporated.
 * All Rights Reserved
 * Qualcomm Technologies Confidential and Proprietary
 *
 *
 * @file Implementation of FastCV Sample native interface.
 *
 */

//==============================================================================
// Include Files
//==============================================================================
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <stdlib.h>
#include <android/log.h>
#include <time.h>
#include <pthread.h>
#include <fastcv/fastcv.h>

#include "FastCVSampleRenderer.h"
#include "CameraRendererRGB565GL2.h"
#include "FPSCounter.h"

//==============================================================================
// Declarations
//==============================================================================
#define LOG_TAG    "FastCVSample"
#define DPRINTF(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define IPRINTF(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define EPRINTF(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

/// Green pixel height width.
static const uint32_t GREEN_PIXEL_HEIGHT_WIDTH  = 5;

//------------------------------------------------------------------------------
/// @brief
///    Contains state information of the instance of the application.
//------------------------------------------------------------------------------
struct RendererState
{
   //---------------------------------------------------------------------------
   /// Constructor for State object sets variables to default values.
   //---------------------------------------------------------------------------
   RendererState()
   {
      cameraRenderer = NULL;

      renderBufRGB565 = NULL;
      renderBufSize = 0;
      renderBufWidth = 0;
      renderBufHeight = 0;
      renderBufReady = false;
   }

   /// Pointer to RGB renderer for preview frame.
   CameraRendererRGB565GL2*   cameraRenderer;

   /// Image Buffer to copy image into.
   uint8_t*                   renderBufRGB565;

   /// Display Size (in bytes) of image being rendered
   uint32_t                   renderBufSize;

   /// Width of renderer buffer
   uint32_t                   renderBufWidth;

   /// Height of renderer buffer
   uint32_t                   renderBufHeight;

   /// Flag to protect image buffer data during update from being
   /// rendered.
   bool                       renderBufReady;

   /// Pthread mutex storage.
   pthread_mutex_t            mutex;
};

/// Rendering module's state structure to hold global state for sample app.
static RendererState          state;

//==============================================================================
// Function Declarations
//==============================================================================

//==============================================================================
// Function Definitions
//==============================================================================

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
uint8_t* getRenderBuffer( uint32_t w, uint32_t h )
{
   lockRenderBuffer();

   // Resize if necessary.
   if( w != state.renderBufWidth || h != state.renderBufHeight )
   {
      if( state.renderBufRGB565 != NULL )
      {
         fcvMemFree( state.renderBufRGB565 );
         state.renderBufRGB565 = NULL;
         state.renderBufSize = 0;
         state.renderBufWidth = 0;
         state.renderBufHeight = 0;
      }
   }

   // Allocate if necessary.
   if( state.renderBufRGB565 == NULL )
   {
      state.renderBufSize = w * h * 2;
      state.renderBufRGB565 = (uint8_t*) fcvMemAlloc(state.renderBufSize, 16);
      state.renderBufWidth = w;
      state.renderBufHeight = h;

      glEnable(GL_TEXTURE_2D);
      glEnable(GL_BLEND);
      glDisable(GL_DITHER);
      glDisable(GL_DEPTH_TEST);
      glBlendFunc(GL_ONE, GL_SRC_COLOR);

      // Initialize mutex
      pthread_mutexattr_t  attribute;
      pthread_mutexattr_init( &attribute );
      pthread_mutexattr_settype( &attribute, PTHREAD_MUTEX_NORMAL );
      pthread_mutex_init( &state.mutex, &attribute );
      pthread_mutexattr_destroy( &attribute );
   }

   unlockRenderBuffer();

   return state.renderBufRGB565;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
uint32_t getRenderBufferSize()
{
   return state.renderBufSize;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void unlockRenderBuffer()
{
   pthread_mutex_unlock( &(state.mutex) );
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void lockRenderBuffer()
{
   int rc = pthread_mutex_lock( &(state.mutex) );
   if( rc != 0 )
   {
      EPRINTF( "Error locking mutex: %d\n", rc );
   }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void drawCorners( uint32_t* corners, uint32_t numCorners )
{
   uint32_t  curcornerx =0;
   uint32_t  curcornery =0;

   // for each of the corner found, insert a green pixel
   for( uint32_t  k=0; k<numCorners; k++ )
   {
      curcornerx = *corners++;
      curcornery = *corners++;

      for (uint32_t  i= curcornerx;
           i< GREEN_PIXEL_HEIGHT_WIDTH + curcornerx; i++)
      {
         for (uint32_t j=curcornery;
              j< GREEN_PIXEL_HEIGHT_WIDTH +curcornery; j++)
         {
            //processing on RGB data
            state.renderBufRGB565[ 2* (j*state.renderBufWidth +i)] = 0;
            state.renderBufRGB565[ 2* (j*state.renderBufWidth +i) + 1] = 31;
         }
      }
   }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT void JNICALL
   Java_com_qualcomm_fastcorner_FastCVSampleRenderer_cleanup
(
   JNIEnv * env,
   jobject obj
)
{
   DPRINTF( "%s\n", __FUNCTION__ );

   if( state.cameraRenderer != NULL )
   {
      state.cameraRenderer->UnInit();
      state.cameraRenderer = NULL;
   }

   if( state.renderBufRGB565 != NULL )
   {
      fcvMemFree( state.renderBufRGB565 );
      state.renderBufRGB565 = NULL;
   }

   return;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT void JNICALL
   Java_com_qualcomm_fastcorner_FastCVSampleRenderer_surfaceChanged
(
   JNIEnv * env, jobject obj,
   jint width,
   jint height
)
{
   DPRINTF( "Viewport changed to %ux%u \n", width, height );

   //Set up viewport. We only need to do this once.
   glViewport(0, 0, width, height);

   return;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT void JNICALL Java_com_qualcomm_fastcorner_FastCVSampleRenderer_render
(
   JNIEnv * env,
   jobject obj,
   jbyteArray img,
   jint w,
   jint h
)
{
   if( state.cameraRenderer == NULL )
   {
      state.cameraRenderer = new CameraRendererRGB565GL2();

      if (state.cameraRenderer)
      {
         state.cameraRenderer->Init();
      }
      else
      {
         EPRINTF( "Unable to get Camera Render object\n" );
      }
   }

   //if camera frame is ready, display that
   if( state.cameraRenderer && state.renderBufRGB565 )
   {
      lockRenderBuffer();
      state.cameraRenderer->Render
      (
         state.renderBufRGB565,
         state.renderBufWidth,
         state.renderBufHeight
      );
      unlockRenderBuffer();

      //glFlush();
      glFinish();
   }

}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT jfloat JNICALL Java_com_qualcomm_fastcorner_FastCVSampleRenderer_getGPUFPS
(
   JNIEnv*  env,
   jobject  obj
)
{
   jfloat rc = 0.f;

   if ( state.cameraRenderer )
   {
      rc = state.cameraRenderer->GetRenderingFPS();
   }
   return rc;
}

