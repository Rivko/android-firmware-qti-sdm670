/*
 * Copyright (c) 2012-2014 Qualcomm Technologies Incorporated. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 *
 * @file Implementation of FastCV Corner Detection Sample native interface.
 *
 */

//==============================================================================
// Include Files
//==============================================================================
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <stdlib.h>
#include <android/log.h>
#include <fastcv/fastcv.h>
#include <string.h>
#include "utils/FastCVUtil.h"
#include "Corner.h"
#include "utils/FastCVSampleRenderer.h"

//==============================================================================
// Declarations
//==============================================================================
/// Constant maximum number of corners to detect.
static const uint32_t MAX_CORNERS_TO_DETECT     = 43000; // ~480*800 / 9

//------------------------------------------------------------------------------
/// @brief
///    Contains state information of the instance of the application.
//------------------------------------------------------------------------------
struct CornerState
{
   //---------------------------------------------------------------------------
   /// Constructor for CornerState object sets variables to default values.
   //---------------------------------------------------------------------------
   CornerState()
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
      detectionSwitch = 7;
      disableVF = false;
      enableOverlayPixels=false;
      enableGaussian = false;

      CornerScore = NULL;
      maskArray = NULL;
   }

   /// Image Buffer to copy image into.
   uint8_t*                   blurredImgBuf;

   /// Width of blurred image buffer
   uint32_t                   blurredImgWidth;

   /// Height of blurred image buffer
   uint32_t                   blurredImgHeight;

   /// Number of corners detected in last frame.
   uint32_t                   numCorners;

   /// Storage for corners detected in last frame.
   //uint32_t                   FASTCV_ALIGN128( corners[MAX_CORNERS_TO_DETECT*2] );
   uint32_t                   corners[MAX_CORNERS_TO_DETECT*2];

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

   /// used to select which detection to use
   uint32_t                   detectionSwitch ;

   /// Whether viewfinder is enabled or displayed
   bool                       disableVF ;

   /// Whether corners should be overlayed with green pixels
   bool                       enableOverlayPixels;

   /// Whether to enable Gaussian filter
   bool                       enableGaussian;

   /// holds the values of the strength of each corner
   uint32_t*                       CornerScore;

   /// Mask for corner functions using mask
   uint8_t*                           maskArray;

};

/// Application' state structure to hold global CornerState for sample app.
static CornerState      cornerState;


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

//==============================================================================
// Function Definitions
//==============================================================================

//------------------------------------------------------------------------------
/// @brief Allocate memory for corner detection utility
///
/// @return 0 if successfull
//------------------------------------------------------------------------------
int allocateMemory
(
   int  width,
   int height
)
{
    int eRet = 0;
    if( cornerState.CornerScore == NULL )
    {
        cornerState.CornerScore = (uint32_t*)fcvMemAlloc( MAX_CORNERS_TO_DETECT * sizeof(uint32_t), 16 );
    }

    // Set mask in the middle
    if( cornerState.maskArray == NULL )
    {
        int maskSize = (width*height) >> 3;
        cornerState.maskArray = (uint8_t*) fcvMemAlloc(maskSize, 16);
        memset(cornerState.maskArray, 0xFF, maskSize);
        for(int i = height/3; i < height*2/3 ; i++ )
        {
            for (int j = width/3; j < width*2/3; j++)
            {
                int index = i*width + j;
                cornerState.maskArray[index>>3] &= ~(0x80>>(index & 0x07));
            }
        }
    }

    if( cornerState.CornerScore == NULL || cornerState.maskArray == NULL )
    {
        eRet = -1;
    }
    return eRet;
}

//------------------------------------------------------------------------------
/// @brief Deallocate memory for corner detection utility
//------------------------------------------------------------------------------
void deallocateMemory()
{
   if(cornerState.CornerScore)
   {
      fcvMemFree(cornerState.CornerScore);
      cornerState.CornerScore = NULL;
   }
   if( cornerState.maskArray )
   {
      fcvMemFree(cornerState.maskArray);
      cornerState.maskArray = NULL;
   }
}

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

   allocateMemory(w, h);

   //
   // Apply Down-Scaling
   //
   if( cornerState.scalingFactor == 0 )
   {
      // Avoid divide by 0
      cornerState.scalingFactor = 1;
   }

   // Allow for rescale if dimensions changed.
   if( ( w/cornerState.scalingFactor) != cornerState.scaledImgWidth ||
       ( h/cornerState.scalingFactor) != cornerState.scaledImgHeight )
   {
      if( cornerState.scaledImgBuf != NULL )
      {
         fcvMemFree( cornerState.scaledImgBuf );
         cornerState.scaledImgBuf = NULL;
      }
   }

   // Allocate buffer for scaled data if necessary.
   if( (cornerState.scaledImgBuf == NULL) && (cornerState.scalingFactor > 1) )
   {
      cornerState.scaledImgWidth = w/cornerState.scalingFactor;
      cornerState.scaledImgHeight = h/cornerState.scalingFactor;
      cornerState.scaledImgBuf = (uint8_t*)fcvMemAlloc(cornerState.scaledImgWidth * cornerState.scaledImgHeight, 16 );
   }

   switch( cornerState.scalingFactor )
   {
   case 2:
      //scale by factor of 1/2
      if( fcvScaleDownBy2u8( data,
                         w,
                         h,
                         cornerState.scaledImgBuf ) )
      {
         EPRINTF( "Scaling by 1/2 failed, "
                  "Width and height should be multiple of 4%u %u \n", w, h );
      }
      else
      {
         dataBufWidth = cornerState.scaledImgWidth;
         dataBufHeight = cornerState.scaledImgHeight;
         dataBuf = cornerState.scaledImgBuf;
      }
      break;

   case 4:
      //scale by 1/4
      if( fcvScaleDownBy4u8( data,
                              w,
                              h,
                              cornerState.scaledImgBuf ) )
      {
         EPRINTF( "Scaling by 1/4 failed, "
                  "Width and height should be multiple of 4%u %u \n", w, h );
      }
      else
      {
         dataBufWidth = cornerState.scaledImgWidth;
         dataBufHeight = cornerState.scaledImgHeight;
         dataBuf = cornerState.scaledImgBuf;
      }
      break;

   case 8:
      //scale by 1/8
      dataBufWidth = cornerState.scaledImgWidth;
      dataBufHeight = cornerState.scaledImgHeight;
      fcvScaleDownu8( data,
                      w,
                      h,
                      cornerState.scaledImgBuf,
                      cornerState.scaledImgWidth,
                      cornerState.scaledImgHeight );

      dataBuf = cornerState.scaledImgBuf;
      break;

   default:
      dataBufWidth = w;
      dataBufHeight = h;
      dataBuf = data;
   break;
   }

   //
   // Apply Gaussian blurring
   //
   if( cornerState.enableGaussian )
   {
      // Allow for rescale if dimensions changed.
      if( dataBufWidth != cornerState.blurredImgWidth ||
          dataBufHeight != cornerState.blurredImgHeight )
      {
         if( cornerState.blurredImgBuf != NULL )
         {
            fcvMemFree( cornerState.blurredImgBuf );
            cornerState.blurredImgBuf = NULL;
         }
      }

      // Allocate new buffer if necessary.
      if( cornerState.blurredImgBuf == NULL )
      {
         cornerState.blurredImgWidth = dataBufWidth;
         cornerState.blurredImgHeight = dataBufHeight;
         cornerState.blurredImgBuf = (uint8_t*)fcvMemAlloc(
            cornerState.blurredImgWidth*cornerState.blurredImgHeight, 16 );
      }

      // Perform gaussian filtering on scaled data buffer.
      fcvFilterGaussian3x3u8( dataBuf,
                              dataBufWidth,
                              dataBufHeight,
                              cornerState.blurredImgBuf,
                              0 );

      dataBuf = cornerState.blurredImgBuf;
   }

   //reset the number of corners detected
   cornerState.numCorners = 0;

   // Apply fast corner detection.
   // Find w*h / 9 corners, with threshold set by user and border ==7
   //
  switch(cornerState.detectionSwitch){
   case 1:
    fcvCornerFast9u8( (uint8_t*)dataBuf,
                     dataBufWidth,
                     dataBufHeight,
                     0,
                     cornerState.cornerThreshold,
                     7,
                     cornerState.corners,
                     MAX_CORNERS_TO_DETECT,
                     &cornerState.numCorners );
   break;

   case 2:
   fcvCornerHarrisu8( (uint8_t*)dataBuf,
                    dataBufWidth,
                    dataBufHeight,
                    0,
                    7,
                    cornerState.corners,
                    MAX_CORNERS_TO_DETECT,
                    &cornerState.numCorners, cornerState.cornerThreshold );
   break;

   case 3:
       fcvCornerFast9InMasku8( (uint8_t*)dataBuf,
                    dataBufWidth,
                    dataBufHeight,
                    0,
                    7,
                    10,
                    cornerState.corners,
                    MAX_CORNERS_TO_DETECT,
                    &cornerState.numCorners,
                    cornerState.maskArray,
                    dataBufWidth,
                    dataBufHeight);
   break;

   case 4:
       fcvCornerHarrisInMasku8( (uint8_t*)dataBuf,
                       dataBufWidth,
                       dataBufHeight,
                       0,
                       7,
                       cornerState.corners,
                       MAX_CORNERS_TO_DETECT,
                       &cornerState.numCorners, cornerState.cornerThreshold,
                       cornerState.maskArray,
                       dataBufWidth,
                       dataBufHeight);
   break;

   case 5:
   fcvCornerFast9Scoreu8( (uint8_t*)dataBuf,
                 dataBufWidth,
                 dataBufHeight,
                 0,
                 cornerState.cornerThreshold,
                 7,
                 cornerState.corners,
                 cornerState.CornerScore,
                 MAX_CORNERS_TO_DETECT,
                 &cornerState.numCorners );
   break;

   case 6:
        fcvCornerFast9InMaskScoreu8( (uint8_t*)dataBuf,
                       dataBufWidth,
                       dataBufHeight,
                       0,
                       7,
                       10,
                       cornerState.corners,
                       cornerState.CornerScore,
                       MAX_CORNERS_TO_DETECT,
                       &cornerState.numCorners,
                       cornerState.maskArray,
                       dataBufWidth,
                       dataBufHeight);
   case 7:
       fcvCornerFast10u8( (uint8_t*)dataBuf,
                     dataBufWidth,
                     dataBufHeight,
                     0,
                     cornerState.cornerThreshold,
                     7,
                     cornerState.corners,
                     MAX_CORNERS_TO_DETECT,
                     &cornerState.numCorners );
   break;

   default:
       break;
   }

   // Re-scale corners if necessary
   for( uint32_t i=0; i<cornerState.numCorners*2; i++ )
   {
      cornerState.corners[i] = cornerState.corners[i]*cornerState.scalingFactor;
   }

}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT void JNICALL
   Java_com_qualcomm_fastcvdemo_apis_featureDetection_Corner_cleanup
(
   JNIEnv * env,
   jobject obj
)
{
   DPRINTF( "%s\n", __FUNCTION__ );

   if( cornerState.blurredImgBuf )
   {
      fcvMemFree(cornerState.blurredImgBuf);
      cornerState.blurredImgBuf = NULL;
   }

   if( cornerState.scaledImgBuf )
   {
      fcvMemFree(cornerState.scaledImgBuf );
      cornerState.scaledImgBuf  = NULL;
   }

   if( cornerState.alignedImgBuf != NULL )
   {
      fcvMemFree( cornerState.alignedImgBuf );
      cornerState.alignedImgBuf = NULL;
   }

   deallocateMemory();

   fcvCleanUp();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT void
   JNICALL Java_com_qualcomm_fastcvdemo_apis_featureDetection_Corner_update
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

   time = util.getTimeMicroSeconds();

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
      if( w != (int)cornerState.alignedImgWidth ||
          h != (int)cornerState.alignedImgHeight )
      {
         if( cornerState.alignedImgBuf != NULL )
         {
            DPRINTF( "%s %d Creating aligned for preview\n",
               __FILE__, __LINE__ );
            fcvMemFree( cornerState.alignedImgBuf );
            cornerState.alignedImgBuf = NULL;
         }
      }

      // Allocate buffer for aligned data if necessary.
      if( cornerState.alignedImgBuf == NULL )
      {
         cornerState.alignedImgWidth = w;
         cornerState.alignedImgHeight = h;
         cornerState.alignedImgBuf = (uint8_t*)fcvMemAlloc( w*h*3/2, 16 );
      }

      memcpy( cornerState.alignedImgBuf, jimgData, w*h*3/2 );
      pJimgData = cornerState.alignedImgBuf;
   }

   // Copy the image first in our own buffer to avoid corruption during
   // rendering. Not that we can still have corruption in image while we do
   // copy but we can't help that.

   // if viewfinder is disabled, simply set to gray
   if( cornerState.disableVF )
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
      colorConvertYUV420ToRGB565Renderer(
         pJimgData,
         w,
         h,
         (uint32_t*)renderBuffer );
   }

   // Perform FastCV Corner processing
   updateCorners( (uint8_t*)pJimgData, w, h );

   timeMs = ( util.getTimeMicroSeconds() - time ) / 1000.f;

   util.setProcessTime((util.getProcessTime()*(29.f/30.f))+(float)(timeMs/30.f));

   // RGB Color conversion
   if( !cornerState.enableOverlayPixels )
   {
      cornerState.numCorners  = 0;
   }

   // Have renderer draw corners on render buffer.
   if(cornerState.detectionSwitch == 5 || cornerState.detectionSwitch == 6)
   {
   drawScoreCorners( cornerState.corners, cornerState.numCorners, cornerState.CornerScore);
   }
   else
   {
      drawCorners( cornerState.corners, cornerState.numCorners );
   }

   unlockRenderBuffer();

   // Let JNI know we don't need data anymore. this is important!
   env->ReleaseByteArrayElements( img, jimgData, JNI_ABORT );
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT int JNICALL Java_com_qualcomm_fastcvdemo_apis_featureDetection_Corner_getNumCorners
(
   JNIEnv*  env,
   jobject  obj
)
{
   return(int) cornerState.numCorners;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_featureDetection_Corner_setScaling
(
   JNIEnv*  env,
   jobject  obj,
   int scaleFactor
)
{
   cornerState.scalingFactor = scaleFactor;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_featureDetection_Corner_setFastCornerThreshold
(
   JNIEnv*  env,
   jobject  obj,
   int threshold
)
{
   cornerState.cornerThreshold = threshold;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_featureDetection_Corner_setViewFinderEnable
(
   JNIEnv*  env,
   jobject  obj,
   jboolean disableViewFinder
)

{
   cornerState.disableVF = (bool)disableViewFinder;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_featureDetection_Corner_setOverlayPixelsEnable
(
   JNIEnv*  env,
   jobject  obj,
   jboolean  enableOverlayPixels
)
{
   cornerState.enableOverlayPixels = (bool)enableOverlayPixels;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_featureDetection_Corner_setGaussianEnable
(
   JNIEnv*  env,
   jobject  obj,
   jboolean enableGaussian
)
{
   cornerState.enableGaussian = (bool)enableGaussian;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT void
   JNICALL Java_com_qualcomm_fastcvdemo_apis_featureDetection_Corner_setDetection
(
   JNIEnv*  env,
   jobject  obj,
   int  detectionSwitch
)
{
   util.setProcessTime(5);
   cornerState.detectionSwitch = detectionSwitch;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT jstring
   JNICALL Java_com_qualcomm_fastcvdemo_apis_featureDetection_Corner_detectionMethod
(
   JNIEnv*  env,
   jobject  obj
)
{
    switch(cornerState.detectionSwitch){
       case 1:
         return (env)->NewStringUTF("CornerFast9u8");
       case 2:
         return (env)->NewStringUTF("CornerHarrisu8");
       case 3:
         return (env)->NewStringUTF("CornerFast9InMasku8");
       case 4:
         return (env)->NewStringUTF("CornerHarrisInMasku8");
       case 5:
         return (env)->NewStringUTF("CornerFast9Scoreu8");
       case 6:
         return (env)->NewStringUTF("CornerFast9InMaskScoreu8");
    case 7:
         return (env)->NewStringUTF("CornerFast10u8");
       default:
         return (env)->NewStringUTF("Detection Method");
    }
}



