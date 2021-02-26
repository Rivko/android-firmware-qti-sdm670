/*
 * Copyright (c) 2012-2014 Qualcomm Technologies Incorporated. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 *
 * @file Implementation of FastCV Scale Sample native interface.
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
#include "Scale.h"
#include "utils/FastCVSampleRenderer.h"

//==============================================================================
// Declarations
//==============================================================================
typedef enum SCALE_TYPE
{
   NO_SCALE,
   SCALE_DOWN_BY2,
   SCALE_DOWN_BY4,
   SCALE_DOWN_BY2_GAUS,
   SCALE_UP_BY2_GAUS
} SCALE_TYPE;


//------------------------------------------------------------------------------
/// @brief
///    Contains scaleState information of the instance of the application.
//------------------------------------------------------------------------------
struct ScaleState
{
   //---------------------------------------------------------------------------
   /// Constructor for ScaleState object sets variables to default values.
   //---------------------------------------------------------------------------
   ScaleState()
   {
      scaledImgBuf = NULL;
      scaledImgWidth = 0;
      scaledImgHeight = 0;

      alignedImgBuf = NULL;

      imgWidth = 0;
      imgHeight = 0;

      scaleType = NO_SCALE;
      nDstOffset = 0;
   }

   /// Intermediate image buffer that stores image between Downscaler
   /// and Gaussian filter.
   uint8_t*                   scaledImgBuf;

   /// Width of scaled image (if scaling is enabled)
   uint32_t                   scaledImgWidth;

   /// Height of scaled image (if scaling is enabled)
   uint32_t                   scaledImgHeight;

   /// Buffer to place image data if camera callback data is not aligned
   uint8_t*                   alignedImgBuf;

   /// Width of image
   uint32_t                   imgWidth;

   /// Height of image
   uint32_t                   imgHeight;

   /// Scaling type selected by user
   SCALE_TYPE                 scaleType ;

   /// Offset for dst image
   uint32_t                      nDstOffset;
};

/// Application' state structure to hold global ScaleState for sample app.
static ScaleState      scaleState;

//==============================================================================
// Function Declarations
//==============================================================================


//==============================================================================
// Function Definitions
//==============================================================================

//------------------------------------------------------------------------------
/// @brief Performs scaling
/// @param pSrc Pointer to image data to perform processing
/// @param w Width of data
/// @param y Height of data
/// @param pDst Pointer to destination data
//------------------------------------------------------------------------------
void updateScaling( uint8_t *pSrc, uint32_t w, uint32_t h, uint8_t *pDst )
{
    switch( scaleState.scaleType )
    {
        case SCALE_DOWN_BY2:
            if( fcvScaleDownBy2u8_v2( pSrc, w, h, w, pDst+scaleState.nDstOffset, w ) )
            {
                EPRINTF("Scaling by 1/2 failed, "
                        "Width and height should be multiple of 4%u %u \n", w, h );
            }
            break;

        case SCALE_DOWN_BY4:
            if( fcvScaleDownBy4u8_v2( pSrc, w, h, w, pDst+scaleState.nDstOffset, w ) )
            {
                EPRINTF("Scaling by 1/4 failed, "
                        "Width and height should be multiple of 4%u %u \n", w, h );
            }
            break;

        case SCALE_DOWN_BY2_GAUS:
            fcvScaleDownBy2Gaussian5x5u8_v2( pSrc, w, h, w, pDst+scaleState.nDstOffset, w );
            break;

        case SCALE_UP_BY2_GAUS:
            fcvScaleUpBy2Gaussian5x5u8_v2( pSrc, w/2, h/2, w, pDst+scaleState.nDstOffset, w );
            break;

        case NO_SCALE:
            memcpy(pDst, pSrc, w*h);
            break;

        default:
            break;

    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageTransformation_Scale_cleanup
(
   JNIEnv * env,
   jobject obj
)
{
   DPRINTF( "%s\n", __FUNCTION__ );

   if( scaleState.scaledImgBuf )
   {
      fcvMemFree(scaleState.scaledImgBuf );
      scaleState.scaledImgBuf  = NULL;
   }

   if( scaleState.alignedImgBuf != NULL )
   {
      fcvMemFree( scaleState.alignedImgBuf );
      scaleState.alignedImgBuf = NULL;
   }

   fcvCleanUp();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageTransformation_Scale_update
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

   // Allocate the buffer once here if it's not allocated already
   if( scaleState.scaledImgBuf == NULL)
   {
      int frameSize = w*h*3/2;
      scaleState.scaledImgBuf = (uint8_t *)fcvMemAlloc(frameSize, 16);
      if( scaleState.scaledImgBuf == NULL )
      {
         EPRINTF("Allocate scaledImgBuf failed");
         return;
      }
      else
      {
         memset(scaleState.scaledImgBuf, 128, frameSize);
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
   uint8_t* pScalingData = (uint8_t*)scaleState.scaledImgBuf;

   // Check if camera image data is not aligned.
   if( (uintptr_t)jimgData & 0xF )
   {
      // Allow for rescale if dimensions changed.
      if( w != (int)scaleState.imgWidth ||
          h != (int)scaleState.imgHeight )
      {
         if( scaleState.alignedImgBuf != NULL )
         {
            DPRINTF( "%s %d Creating aligned for preview\n",
               __FILE__, __LINE__ );
            fcvMemFree( scaleState.alignedImgBuf );
            scaleState.alignedImgBuf = NULL;
         }
      }

      // Allocate buffer for aligned data if necessary.
      if( scaleState.alignedImgBuf == NULL )
      {
         scaleState.imgWidth = w;
         scaleState.imgHeight = h;
         scaleState.alignedImgBuf = (uint8_t*)fcvMemAlloc( w*h*3/2, 16 );
      }

      memcpy( scaleState.alignedImgBuf, jimgData, w*h*3/2 );
      pJimgData = scaleState.alignedImgBuf;
   }
   else if( w != (int)scaleState.imgWidth ||
            h != (int)scaleState.imgHeight )
   {
      scaleState.imgWidth = w;
      scaleState.imgHeight = h;
   }

   // Perform FastCV Function processing
   updateScaling( (uint8_t*)pJimgData, w, h, (uint8_t*)pScalingData );

   // Copy the image first in our own buffer to avoid corruption during
   // rendering. Not that we can still have corruption in image while we do
   // copy but we can't help that.

   colorConvertYUV420ToRGB565Renderer(pScalingData,
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
/// @brief Sets the scaling factor to be applied
///
/// @param scaleFactor  Scaling Factor by which to downsample image
//------------------------------------------------------------------------------
JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageTransformation_Scale_setScaling
(
  JNIEnv*  env,
  jobject  obj,
  int scaleType
)
{
    scaleState.scaleType = (SCALE_TYPE) scaleType;
    DPRINTF("SetScaling %d %d width %d height %d", scaleState.scaleType, scaleType, scaleState.imgWidth, scaleState.imgHeight);

    switch( scaleState.scaleType )
    {
        case SCALE_DOWN_BY2:
            scaleState.nDstOffset = scaleState.imgWidth * scaleState.imgHeight/2 +
                                scaleState.imgWidth/2;
            break;

        case SCALE_DOWN_BY4:
            scaleState.nDstOffset = scaleState.imgWidth * scaleState.imgHeight*3/4 +
                                scaleState.imgWidth*3/4;
            break;

        case SCALE_DOWN_BY2_GAUS:
            scaleState.nDstOffset = scaleState.imgWidth * scaleState.imgHeight/2 +
                                scaleState.imgWidth/2;
            break;

        case SCALE_UP_BY2_GAUS:
            scaleState.nDstOffset = 0;
            break;

        case NO_SCALE:
            scaleState.nDstOffset = 0;
            break;

        default:
            scaleState.nDstOffset = 0;
            break;

    }

	DPRINTF("scaleState.nDstOffset %d, scaleState.scaledImgBuf %x", scaleState.nDstOffset, (uintptr_t) scaleState.scaledImgBuf);
    if( scaleState.scaledImgBuf )
    {
        // set the luma to be black (0)
        memset(scaleState.scaledImgBuf, 0, scaleState.imgWidth*scaleState.imgHeight);

        // set the chroma so that the image will be like black and white (128)
        memset(scaleState.scaledImgBuf+(scaleState.imgWidth*scaleState.imgHeight),
                128, scaleState.imgWidth*scaleState.imgHeight/2);
    }
}
