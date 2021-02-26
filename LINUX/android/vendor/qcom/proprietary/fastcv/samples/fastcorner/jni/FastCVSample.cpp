/*
 * Copyright (c) 2010-2014 Qualcomm Technologies Incorporated.
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
#include <fastcv/fastcv.h>
#include <string.h>
#include "FastCVSample.h"
#include "FastCVSampleRenderer.h"
#include "FPSCounter.h"

//==============================================================================
// Declarations
//==============================================================================
#define LOG_TAG    "FastCVSample"
#define DPRINTF(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define IPRINTF(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define EPRINTF(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

/// Constant maximum number of corners to detect.
static const uint32_t MAX_CORNERS_TO_DETECT     = 43000; // ~480*800 / 9

//------------------------------------------------------------------------------
/// @brief
///    Contains state information of the instance of the application.
//------------------------------------------------------------------------------
struct State
{
   //---------------------------------------------------------------------------
   /// Constructor for State object sets variables to default values.
   //---------------------------------------------------------------------------
   State()
   {
      numCorners = 0;

      blurredImgBuf = NULL;
      blurredImgWidth = 0;
      blurredImgHeight = 0;

      scaledImgBuf = NULL;
      scaledImgWidth = 0;
      scaledImgHeight = 0;

      alignedImgBuf = NULL;
      alignedImgWidth = 0;
      alignedImgHeight = 0;

      cornerThreshold = 0;
      scalingFactor = 1;
      disableVF = false;
      enableOverlayPixels=false;
      enableGaussian = false;

      timeFilteredMs = 5;
   }

   /// Image Buffer to copy image into.
   uint8_t*                   blurredImgBuf;

   /// Width of blurred image buffer
   uint32_t                   blurredImgWidth;

   /// Height of blurred image buffer
   uint32_t                   blurredImgHeight;

   /// Camera preview FPS counter
   FPSCounter                 camFPSCounter;

   /// Number of corners detected in last frame.
   uint32_t                   numCorners;

   /// Storage for corners detected in last frame.
   uint32_t                   FASTCV_ALIGN128( corners[MAX_CORNERS_TO_DETECT*2] );

   /// Intermediate image buffer that stores image between Downscaler
   /// and Gaussian filter.
   uint8_t*                   scaledImgBuf;

   /// Width of scaled image (if scaling is enabled)
   uint32_t                   scaledImgWidth;

   /// Height of scaled image (if scaling is enabled)
   uint32_t                   scaledImgHeight;

   /// Buffer to place image data if camera callback data is not aligned
   uint8_t*                   alignedImgBuf;

   /// Width of aligned image
   uint32_t                   alignedImgWidth;

   /// Height of aligned image
   uint32_t                   alignedImgHeight;

   /// Threshold of Fast CV Corner Detector algorithm
   uint32_t                   cornerThreshold;

   /// Scaling factor selected by user
   uint32_t                   scalingFactor ;

   /// Whether viewfinder is enabled or displayed
   bool                       disableVF ;

   /// Whether corners should be overlayed with green pixels
   bool                       enableOverlayPixels;

   /// Whether to enable Gaussian filter
   bool                       enableGaussian;

   /// Filtered timing value for FastCV processing.
   float                      timeFilteredMs;

};

/// Application' state structure to hold global state for sample app.
static State      state;

//==============================================================================
// Function Declarations
//==============================================================================

//------------------------------------------------------------------------------
/// @brief Performs scaling, blurring and corner detection
///
/// @param data Pointer to image data to perform processing
/// @param w Width of data
/// @param y Height of data
//------------------------------------------------------------------------------
void updateCorners( uint8_t* data, uint32_t w, uint32_t h );

//------------------------------------------------------------------------------
/// @brief Returns current time in microseconds
///
/// @return Time in microseconds
//------------------------------------------------------------------------------
uint64_t getTimeMicroSeconds();

//==============================================================================
// Function Definitions
//==============================================================================


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void updateCorners( uint8_t* data, uint32_t w, uint32_t h )
{
   // Width of scaled data buffer
   uint32_t                   dataBufWidth = w;
   // Height of scaled data buffer
   uint32_t                   dataBufHeight = h;
   // Pointer to scaled data buffer
   uint8_t*                   dataBuf = data;

   //
   // Apply Down-Scaling
   //
   if( state.scalingFactor == 0 )
   {
      // Avoid divide by 0
      state.scalingFactor = 1;
   }

   // Allow for rescale if dimensions changed.
   if( ( w/state.scalingFactor) != state.scaledImgWidth ||
       ( h/state.scalingFactor) != state.scaledImgHeight )
   {
      if( state.scaledImgBuf != NULL )
      {
         fcvMemFree( state.scaledImgBuf );
         state.scaledImgBuf = NULL;
      }
   }

   // Allocate buffer for scaled data if necessary.
   if( (state.scaledImgBuf == NULL) && (state.scalingFactor > 1) )
   {
      state.scaledImgWidth = w/state.scalingFactor;
      state.scaledImgHeight = h/state.scalingFactor;
      state.scaledImgBuf = (uint8_t*)fcvMemAlloc(
         state.scaledImgWidth * state.scaledImgHeight, 16 );
   }

   switch( state.scalingFactor )
   {
   case 2:
      //scale by factor of 1/2
      if( fcvScaleDownBy2u8( data,
                         w,
                         h,
                         state.scaledImgBuf ) )
      {
         EPRINTF( "Scaling by 1/2 failed, "
                  "Width and height should be multiple of 4%u %u \n", w, h );
      }
      else
      {
         dataBufWidth = state.scaledImgWidth;
         dataBufHeight = state.scaledImgHeight;
         dataBuf = state.scaledImgBuf;
      }
      break;

   case 4:
      //scale by 1/4
      if( fcvScaleDownBy4u8( data,
                              w,
                              h,
                              state.scaledImgBuf ) )
      {
         EPRINTF( "Scaling by 1/4 failed, "
                  "Width and height should be multiple of 4%u %u \n", w, h );
      }
      else
      {
         dataBufWidth = state.scaledImgWidth;
         dataBufHeight = state.scaledImgHeight;
         dataBuf = state.scaledImgBuf;
      }
      break;

   case 8:
      //scale by 1/8
      dataBufWidth = state.scaledImgWidth;
      dataBufHeight = state.scaledImgHeight;
      fcvScaleDownu8( data,
                      w,
                      h,
                      state.scaledImgBuf,
                      state.scaledImgWidth,
                      state.scaledImgHeight );

      dataBuf = state.scaledImgBuf;
      break;

   case 16:
      //scale by 1/16
      dataBufWidth = state.scaledImgWidth;
      dataBufHeight = state.scaledImgHeight;

      fcvScaleDownu8( (uint8_t*)data,
                      w,
                      h,
                      state.scaledImgBuf,
                      state.scaledImgWidth,
                      state.scaledImgHeight );

      dataBuf = state.scaledImgBuf;
      break;

   default:
      dataBufWidth = w;
      dataBufHeight = h;
      dataBuf = data;
   }

   //
   // Apply Gaussian blurring
   //
   if( state.enableGaussian )
   {
      // Allow for rescale if dimensions changed.
      if( dataBufWidth != state.blurredImgWidth ||
          dataBufHeight != state.blurredImgHeight )
      {
         if( state.blurredImgBuf != NULL )
         {
            fcvMemFree( state.blurredImgBuf );
            state.blurredImgBuf = NULL;
         }
      }

      // Allocate new buffer if necessary.
      if( state.blurredImgBuf == NULL )
      {
         state.blurredImgWidth = dataBufWidth;
         state.blurredImgHeight = dataBufHeight;
         state.blurredImgBuf = (uint8_t*)fcvMemAlloc(
            state.blurredImgWidth*state.blurredImgHeight, 16 );
      }

      // Perform gaussian filtering on scaled data buffer.
      fcvFilterGaussian3x3u8( dataBuf,
                              dataBufWidth,
                              dataBufHeight,
                              state.blurredImgBuf,
                              0 );

      dataBuf = state.blurredImgBuf;
   }

   //reset the number of corners detected
   state.numCorners = 0;

   //
   // Apply fast corner detection.
   // Find w*h / 9 corners, with threshold set by user and border ==7
   //
   fcvCornerFast9u8( (uint8_t*)dataBuf,
                     dataBufWidth,
                     dataBufHeight,
                     0,
                     state.cornerThreshold,
                     7,
                     state.corners,
                     MAX_CORNERS_TO_DETECT,
                     &state.numCorners );

   // Re-scale corners if necessary
   for( uint32_t i=0; i<state.numCorners*2; i++ )
   {
      state.corners[i] = state.corners[i]*state.scalingFactor;
   }

}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
uint64_t getTimeMicroSeconds()
{
   struct timespec t;
   t.tv_sec = t.tv_nsec = 0;

   clock_gettime(CLOCK_REALTIME, &t);
   return (uint64_t)t.tv_sec * 1000000ULL + t.tv_nsec / 1000L;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT void JNICALL
   Java_com_qualcomm_fastcorner_FastCVSample_cleanup
(
   JNIEnv * env,
   jobject obj
)
{
   DPRINTF( "%s\n", __FUNCTION__ );

   if( state.blurredImgBuf )
   {
      fcvMemFree(state.blurredImgBuf);
      state.blurredImgBuf = NULL;
   }

   if( state.scaledImgBuf )
   {
      fcvMemFree(state.scaledImgBuf );
      state.scaledImgBuf  = NULL;
   }

   if( state.alignedImgBuf != NULL )
   {
      fcvMemFree( state.alignedImgBuf );
      state.alignedImgBuf = NULL;
   }

   fcvCleanUp();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT void
   JNICALL Java_com_qualcomm_fastcorner_FastCVSample_update
(
   JNIEnv*     env,
   jobject     obj,
   jbyteArray  img,
   jint        w,
   jint        h
)
{
   jbyte*            jimgData = NULL;
   jboolean          isCopy = 0;
   uint32_t*         curCornerPtr = 0;
   uint8_t*          renderBuffer;
   uint64_t          time;
   float             timeMs;

   // Get data from JNI
   jimgData = env->GetByteArrayElements( img, &isCopy );

   renderBuffer = getRenderBuffer( w, h );

   lockRenderBuffer();

   time = getTimeMicroSeconds();

   // jimgData might not be 128 bit aligned.
   // fcvColorYUV420toRGB565u8() and other fcv functionality inside
   // updateCorners() require 128 bit memory aligned. In case of jimgData
   // is not 128 bit aligned, it will allocate memory that is 128 bit
   // aligned and copies jimgData to the aligned memory.

   uint8_t*  pJimgData = (uint8_t*)jimgData;

   // Check if camera image data is not aligned.
   if( (uintptr_t)jimgData & 0xF )
   {
      // Allow for rescale if dimensions changed.
      if( w != (int)state.alignedImgWidth ||
          h != (int)state.alignedImgHeight )
      {
         if( state.alignedImgBuf != NULL )
         {
            DPRINTF( "%s %d Creating aligned for preview\n",
               __FILE__, __LINE__ );
            fcvMemFree( state.alignedImgBuf );
            state.alignedImgBuf = NULL;
         }
      }

      // Allocate buffer for aligned data if necessary.
      if( state.alignedImgBuf == NULL )
      {
         state.alignedImgWidth = w;
         state.alignedImgHeight = h;
         state.alignedImgBuf = (uint8_t*)fcvMemAlloc( w*h*3/2, 16 );
      }

      memcpy( state.alignedImgBuf, jimgData, w*h*3/2 );
      pJimgData = state.alignedImgBuf;
   }

   // Copy the image first in our own buffer to avoid corruption during
   // rendering. Not that we can still have corruption in image while we do
   // copy but we can't help that.

   // if viewfinder is disabled, simply set to gray
   if( state.disableVF )
   {
      // Loop through RGB565 values and set to gray.
      uint32_t size = getRenderBufferSize();
      for( uint32_t i=0; i<size; i+=2 )
      {
         renderBuffer[i] = 0x10;
         renderBuffer[i+1] = 0x84;
      }
   }
   else
   {
      fcvColorYUV420toRGB565u8(
         pJimgData,
         w,
         h,
         (uint32_t*)renderBuffer );
   }

   // Perform FastCV Corner processing
   updateCorners( (uint8_t*)pJimgData, w, h );

   timeMs = ( getTimeMicroSeconds() - time ) / 1000.f;
   state.timeFilteredMs =
      ((state.timeFilteredMs*(29.f/30.f)) + (float)(timeMs/30.f));

   // RGB Color conversion
   if( !state.enableOverlayPixels )
   {
      state.numCorners  = 0;
   }

   // Have renderer draw corners on render buffer.
   drawCorners( state.corners, state.numCorners );

   unlockRenderBuffer();

   // Let JNI know we don't need data anymore. this is important!
   env->ReleaseByteArrayElements( img, jimgData, JNI_ABORT );
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT void JNICALL Java_com_qualcomm_fastcorner_FastCVSample_init
(
   JNIEnv* env,
   jobject obj
)
{
   char sVersion[32];

   fcvSetOperationMode( (fcvOperationMode) FASTCV_OP_PERFORMANCE );

   fcvGetVersion(sVersion, 32);
   DPRINTF( "Using FastCV version %s \n", sVersion );

   return;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT int JNICALL Java_com_qualcomm_fastcorner_FastCVSample_getNumCorners
(
   JNIEnv*  env,
   jobject  obj
)
{
   return(int) state.numCorners;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT jfloat JNICALL Java_com_qualcomm_fastcorner_FastCVSample_getCameraFPS
(
   JNIEnv*  env,
   jobject  obj
)
{
   return state.camFPSCounter.GetFilteredFPS();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT void JNICALL Java_com_qualcomm_fastcorner_FastCVSample_cameraFrameTick
(
   JNIEnv*  env,
   jobject  obj
)
{
   state.camFPSCounter.FrameTick();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT void JNICALL Java_com_qualcomm_fastcorner_FastCVSample_setScaling
(
   JNIEnv*  env,
   jobject  obj,
   int scaleFactor
)
{
   state.scalingFactor = scaleFactor;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT void JNICALL
   Java_com_qualcomm_fastcorner_FastCVSample_setFastCornerThreshold
(
   JNIEnv*  env,
   jobject  obj,
   int threshold
)
{
   state.cornerThreshold = threshold;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT void JNICALL
   Java_com_qualcomm_fastcorner_FastCVSample_setViewFinderEnable
(
   JNIEnv*  env,
   jobject  obj,
   jboolean disableViewFinder
)

{
   state.disableVF = (bool)disableViewFinder;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT void
   JNICALL Java_com_qualcomm_fastcorner_FastCVSample_setOverlayPixelsEnable
(
   JNIEnv*  env,
   jobject  obj,
   jboolean  enableOverlayPixels
)
{
   state.enableOverlayPixels = (bool)enableOverlayPixels;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT void
   JNICALL Java_com_qualcomm_fastcorner_FastCVSample_setGaussianEnable
(
   JNIEnv*  env,
   jobject  obj,
   jboolean enableGaussian
)
{
   state.enableGaussian = (bool)enableGaussian;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT jfloat JNICALL
   Java_com_qualcomm_fastcorner_FastCVSample_getFastCVProcessTime
(
   JNIEnv*  env,
   jobject  obj
)
{
   return (jfloat) state.timeFilteredMs;
}

