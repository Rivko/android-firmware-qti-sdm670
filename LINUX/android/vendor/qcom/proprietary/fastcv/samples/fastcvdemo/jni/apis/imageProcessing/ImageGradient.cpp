/*
 * Copyright (c) 2012-2014 Qualcomm Technologies Incorporated. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 * @file Implementation of FastCV Image Gradient Sample native interface.
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
#include "ImageGradient.h"
#include "utils/FastCVSampleRenderer.h"
#include "utils/FPSCounter.h"

//==============================================================================
// Declarations
//==============================================================================

//------------------------------------------------------------------------------
/// @brief
///    Contains orientations.
//------------------------------------------------------------------------------
typedef enum GRADIENT_ORIENTATION_TYPE
{
    VERTICAL_GRADIENT,
    HORIZONTAL_GRADIENT,
    NONE
} GRADIENT_ORIENTATION_TYPE;

//------------------------------------------------------------------------------
/// @brief
///    Contains type of gradients.
//------------------------------------------------------------------------------
typedef enum GRADIENT_TYPE
{
    RESET,
    NO_SOBEL,
    SOBEL
} GRADIENT_TYPE;

//------------------------------------------------------------------------------
/// @brief
///    Contains GradientState information of the instance of the application.
//------------------------------------------------------------------------------
struct GradientState
{
   //---------------------------------------------------------------------------
   /// Constructor for GradientState object sets variables to default values.
   //---------------------------------------------------------------------------
    GradientState()
   {

        gradientImgBuf = NULL;
        gradientImgWidth = 0;
        gradientImgHeight = 0;

        xGradientImgBuf = NULL;
        xGradientImgWidth = 0;
        xGradientImgHeight = 0;

        yGradientImgBuf = NULL;
        yGradientImgWidth = 0;
        yGradientImgHeight = 0;

        alignedImgBuf = NULL;

        imgWidth = 0;
        imgHeight = 0;

        gradientOrientation = VERTICAL_GRADIENT;

        gradientType = NO_SOBEL;
   }

    /// Camera preview FPS counter
    FPSCounter                 camFPSCounter;

    /// Image buffer that stores gradient image
    uint8_t*                 gradientImgBuf;

    /// Width of image
    uint32_t                   gradientImgWidth;

    /// Height of image
    uint32_t                   gradientImgHeight;

    /// Intermediate image buffer that stores horizontal gradient
    int16_t*                xGradientImgBuf;

    /// Width of image
    uint32_t                   xGradientImgWidth;

    /// Height of image
    uint32_t                   xGradientImgHeight;

    /// Intermediate image buffer that stores vertical gradient
    int16_t*                 yGradientImgBuf;

    /// Width of image
    uint32_t                   yGradientImgWidth;

    /// Height of image
    uint32_t                   yGradientImgHeight;


    /// Buffer to place image data if camera callback data is not aligned
    uint8_t*                   alignedImgBuf;

    /// Width of image
    uint32_t                   imgWidth;

    /// Height of image
    uint32_t                   imgHeight;

    /// Orientation type selected by user
    GRADIENT_ORIENTATION_TYPE  gradientOrientation ;

    /// Gradient type selected by user
    GRADIENT_TYPE  gradientType ;

};

/// Application' state structure to hold global state for sample app.
static GradientState      gradientState;


//==============================================================================
// Function Definitions
//==============================================================================


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT void JNICALL
   Java_com_qualcomm_fastcvdemo_apis_imageProcessing_ImageGradient_cleanup
(
   JNIEnv * env,
   jobject obj
)
{
   DPRINTF( "%s\n", __FUNCTION__ );

   if( gradientState.gradientImgBuf )
   {
        fcvMemFree(gradientState.gradientImgBuf  );
        gradientState.gradientImgBuf   = NULL;
   }

   if( gradientState.xGradientImgBuf )
   {
     fcvMemFree(gradientState.xGradientImgBuf  );
     gradientState.xGradientImgBuf   = NULL;
   }

   if( gradientState.yGradientImgBuf )
   {
        fcvMemFree(gradientState.yGradientImgBuf  );
        gradientState.yGradientImgBuf   = NULL;
   }

   if( gradientState.alignedImgBuf )
   {
        fcvMemFree(gradientState.alignedImgBuf  );
        gradientState.alignedImgBuf   = NULL;
   }

   fcvCleanUp();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageProcessing_ImageGradient_update
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
   int i;

   int frameSize = sizeof(int16_t)*(w)*(h);

   // Allocate the buffer once here if it's not allocated already
  if( gradientState.gradientImgBuf == NULL)
  {
     gradientState.gradientImgBuf = (uint8_t *)fcvMemAlloc(w*h*3/2, 16);
     if( gradientState.gradientImgBuf == NULL )
     {
        EPRINTF("Allocate gradientImgBuf failed");
        return;
     }
     else
     {
        memset(gradientState.gradientImgBuf, 0, w*h);
        memset(gradientState.gradientImgBuf+(w*h), 128, w*h/2);
     }
  }

   // Allocate the buffer once here if it's not allocated already
   if( gradientState.xGradientImgBuf == NULL)
   {
      gradientState.xGradientImgBuf = (int16_t *)fcvMemAlloc(frameSize, 16);
      if( gradientState.xGradientImgBuf == NULL )
      {
         EPRINTF("Allocate xGradientImgBuf failed");
         return;
      }
   }

   // Allocate the buffer once here if it's not allocated already
  if( gradientState.yGradientImgBuf == NULL)
  {
     gradientState.yGradientImgBuf = (int16_t *)fcvMemAlloc(frameSize, 16);
     if( gradientState.yGradientImgBuf == NULL )
     {
        EPRINTF("Allocate yGradientImgBuf failed");
     }
  }

   // Get data from JNI
   jimgData = env->GetByteArrayElements( img, &isCopy );

   renderBuffer = getRenderBuffer( w, h );

   lockRenderBuffer();

   time = util.getTimeMicroSeconds();

   // jimgData might not be 128 bit aligned.
   // fcvColorYUV420toRGB565u8() and other fcv functionality inside
   // require 128 bit memory aligned. In case of jimgData
   // is not 128 bit aligned, it will allocate memory that is 128 bit
   // aligned and copies jimgData to the aligned memory.

   uint8_t* pJimgData    = (uint8_t*)jimgData;

   // Check if camera image data is not aligned.
   if( (uintptr_t)jimgData & 0xF )
   {
      // Allow for rescale if dimensions changed.
      if( w != (int)gradientState.imgWidth ||
          h != (int)gradientState.imgHeight )
      {
         if( gradientState.alignedImgBuf != NULL )
         {
            DPRINTF( "%s %d Creating aligned for preview\n",
               __FILE__, __LINE__ );
            fcvMemFree( gradientState.alignedImgBuf );
            gradientState.alignedImgBuf = NULL;
         }
      }

      // Allocate buffer for aligned data if necessary.
      if( gradientState.alignedImgBuf == NULL )
      {
          gradientState.imgWidth = w;
          gradientState.imgHeight = h;
          gradientState.alignedImgBuf = (uint8_t*)fcvMemAlloc( w*h*3/2, 16 );
      }

      memcpy( gradientState.alignedImgBuf, jimgData, w*h*3/2 );
      pJimgData = gradientState.alignedImgBuf;
    }

   else if( w != (int)gradientState.imgWidth ||
                h != (int)gradientState.imgHeight )
   {
       gradientState.imgWidth = w;
       gradientState.imgHeight = h;
   }

   // Perform FastCV Function processing
   switch( gradientState.gradientType )
      {
         case NO_SOBEL:
           fcvImageGradientPlanars16( (uint8_t*)pJimgData, w, h, w, gradientState.xGradientImgBuf, gradientState.yGradientImgBuf );
           break;

         case SOBEL:
           fcvImageGradientSobelPlanars16( (uint8_t*)pJimgData, w, h, w, gradientState.xGradientImgBuf, gradientState.yGradientImgBuf );
           break;

         case RESET:
           memcpy(gradientState.gradientImgBuf, pJimgData, w*h);
           break;

         default:
           break;
      }

   // Copy the image first in our own buffer to avoid corruption during
   // rendering. Not that we can still have corruption in image while we do
   // copy but we can't help that.

   int16_t *pGradientBufs16 = gradientState.xGradientImgBuf;

   //Set output to either vertical or horizontal
   if (gradientState.gradientType != RESET)
   {
       switch( gradientState.gradientOrientation )
       {
          case VERTICAL_GRADIENT:
            pGradientBufs16 = gradientState.yGradientImgBuf;
            break;

          case HORIZONTAL_GRADIENT:
          default:
            pGradientBufs16 = gradientState.xGradientImgBuf;
            break;
       }

       //Convert from int16 to uint8 for display
       for(i = 0; i < w*h; i++)
       {
           gradientState.gradientImgBuf[i] = (uint8_t) ((pGradientBufs16[i]/2) + 128);
       }
   }


   colorConvertYUV420ToRGB565Renderer(gradientState.gradientImgBuf,
                            w,
                            h,
                            (uint32_t*)renderBuffer );

   // Update image
   timeMs = ( util.getTimeMicroSeconds() - time ) / 1000.f;
   util.setProcessTime((util.getProcessTime()*(29.f/30.f))+(float)(timeMs/30.f));

   unlockRenderBuffer();

   // Let JNI know we don't need data anymore
   env->ReleaseByteArrayElements( img, jimgData, JNI_ABORT );
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageProcessing_ImageGradient_setGradientOrientation
(
  JNIEnv*  env,
  jobject  obj,
  int gradientOrientation
)
{
    gradientState.gradientOrientation = (GRADIENT_ORIENTATION_TYPE) gradientOrientation;

    DPRINTF("set gradient");
}

JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageProcessing_ImageGradient_setGradientType
(
  JNIEnv*  env,
  jobject  obj,
  int gradientType
)
{
    gradientState.gradientType = (GRADIENT_TYPE) gradientType;

    DPRINTF("set gradient");
}
