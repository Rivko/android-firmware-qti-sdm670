/*
 * Copyright (c) 2012-2014 Qualcomm Technologies Incorporated. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 * @file Implementation of FastCV Image Diff Sample native interface.
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
#include "ImgDiff.h"
#include "utils/FastCVSampleRenderer.h"

//==============================================================================
// Declarations
//==============================================================================

//------------------------------------------------------------------------------
/// @brief
///    Contains state of reference frame.
//------------------------------------------------------------------------------
typedef enum DIFF_STATE
{
   NO_REF_FRAME,
   TAKE_REF_FRAME,
   HAS_REF_FRAME,
   NEED_RESET
} DIFF_STATE;


//------------------------------------------------------------------------------
/// @brief
///    Contains imgDiffState information of the instance of the application.
//------------------------------------------------------------------------------
struct ImgDiffState
{
    //---------------------------------------------------------------------------
    /// Constructor for ImgDiffState object sets variables to default values.
    //---------------------------------------------------------------------------
    ImgDiffState()
    {
        alignedImgBuf = NULL;
        alignedImgWidth = 0;
        alignedImgHeight = 0;

        referenceImgBuf = NULL;
        diffState = NO_REF_FRAME;

        diffImgBuf = NULL;
    }

    /// Buffer to place image data if camera callback data is not aligned
    uint8_t*                   alignedImgBuf;

    /// Width of aligned image
    uint32_t                   alignedImgWidth;

    /// Height of aligned image
    uint32_t                   alignedImgHeight;

    /// Buffer to place reference image data if user click on the "Take Reference Img" button
    uint8_t*                   referenceImgBuf;

    /// Flag to copy the reference frame buffer
    DIFF_STATE                 diffState;

    /// Buffer to place the different between referenceImgBuf and the current ImgBuf from camera
    uint8_t*                   diffImgBuf;


};

/// Application' imgDiffState structure to hold global imgDiffState for sample app.
static ImgDiffState      imgDiffState;

//==============================================================================
// Function Definitions
//==============================================================================


void updateDiff( uint8_t *pSrc1, uint8_t *pSrc2, uint32_t w, uint32_t h, uint8_t *pDst )
{
    fcvImageDiffu8( pSrc1, pSrc2, w, h, pDst);
}

JNIEXPORT void JNICALL
   Java_com_qualcomm_fastcvdemo_apis_imageProcessing_ImgDiff_cleanup
(
   JNIEnv * env,
   jobject obj
)
{
   DPRINTF( "%s\n", __FUNCTION__ );

   if( imgDiffState.alignedImgBuf != NULL )
   {
      fcvMemFree( imgDiffState.alignedImgBuf );
      imgDiffState.alignedImgBuf = NULL;
   }

   if( imgDiffState.referenceImgBuf != NULL )
   {
      fcvMemFree( imgDiffState.referenceImgBuf );
      imgDiffState.referenceImgBuf = NULL;
   }

   if( imgDiffState.diffImgBuf != NULL )
    {
       fcvMemFree( imgDiffState.diffImgBuf );
       imgDiffState.diffImgBuf = NULL;
    }

    fcvCleanUp();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT void JNICALL
    Java_com_qualcomm_fastcvdemo_apis_imageProcessing_ImgDiff_update
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

   DPRINTF("ImgDiff: update");

   renderBuffer = getRenderBuffer( w, h );

   lockRenderBuffer();

   time = util.getTimeMicroSeconds();

   // Allocate the buffer once here if it's not allocated already
   if( imgDiffState.referenceImgBuf == NULL)
   {
      imgDiffState.referenceImgBuf = (uint8_t *)fcvMemAlloc(w*h*3/2, 16);
      if( imgDiffState.referenceImgBuf == NULL )
      {
         EPRINTF("Allocate referenceImgBuf failed");
         return;
      }
   }

   // Allocate the buffer once here if it's not allocated already
   if( imgDiffState.diffImgBuf == NULL)
   {
      int frameSize = w*h*3/2;
      imgDiffState.diffImgBuf = (uint8_t *)fcvMemAlloc(frameSize, 16);
      if( imgDiffState.diffImgBuf == NULL )
      {
         EPRINTF("Allocate diffImgBuf failed");
      }
      else
      {
          memset(imgDiffState.diffImgBuf, 0, w*h);
          memset(imgDiffState.diffImgBuf+(w*h), 128, w*h/2);
      }
   }

   uint8_t* pJimgData    = (uint8_t*)jimgData;
   uint8_t* pDiffData    = (uint8_t*)imgDiffState.diffImgBuf;

   // jimgData might not be 128 bit aligned.
   // fcvColorYUV420toRGB565u8() and other fcv functionality inside
   // require 128 bit memory aligned. In case of jimgData
   // is not 128 bit aligned, it will allocate memory that is 128 bit
   // aligned and copies jimgData to the aligned memory.

   // Check if camera image data is not aligned.
   if( (uintptr_t)jimgData & 0xF )
   {
      // Allow for rescale if dimensions changed.
      if( w != (int)imgDiffState.alignedImgWidth ||
          h != (int)imgDiffState.alignedImgHeight )
      {
         if( imgDiffState.alignedImgBuf != NULL )
         {
            DPRINTF( "%s %d Creating aligned for preview\n",
               __FILE__, __LINE__ );
            fcvMemFree( imgDiffState.alignedImgBuf );
            imgDiffState.alignedImgBuf = NULL;
         }
      }

      // Allocate buffer for aligned data if necessary.
      if( imgDiffState.alignedImgBuf == NULL )
      {
         imgDiffState.alignedImgWidth = w;
         imgDiffState.alignedImgHeight = h;
         imgDiffState.alignedImgBuf = (uint8_t*)fcvMemAlloc( w*h*3/2, 16 );
      }

      memcpy( imgDiffState.alignedImgBuf, jimgData, w*h*3/2 );
      pJimgData = imgDiffState.alignedImgBuf;
   }

   uint8_t* pPreviewData = pJimgData;

   //Handles reference frame state
   switch( imgDiffState.diffState )
   {
      case NO_REF_FRAME:
         break;

      case TAKE_REF_FRAME:
         DPRINTF("In take frame");
         memcpy(imgDiffState.referenceImgBuf, pPreviewData, w*h);
         imgDiffState.diffState = HAS_REF_FRAME;
         DPRINTF("ImgDiff: taken frame");
         break;

      case HAS_REF_FRAME:
         updateDiff( imgDiffState.referenceImgBuf, pJimgData, w, h, pDiffData );
         pPreviewData = pDiffData;
         break;

      case NEED_RESET:
         imgDiffState.diffState = NO_REF_FRAME;
         break;

      default:
         break;
   }

   colorConvertYUV420ToRGB565Renderer(pPreviewData,
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

JNIEXPORT void JNICALL
    Java_com_qualcomm_fastcvdemo_apis_imageProcessing_ImgDiff_takeReferenceFrame()
{
   imgDiffState.diffState = TAKE_REF_FRAME;
}

JNIEXPORT void JNICALL
    Java_com_qualcomm_fastcvdemo_apis_imageProcessing_ImgDiff_resetReferenceFrame()
{
   imgDiffState.diffState = NEED_RESET;
}

