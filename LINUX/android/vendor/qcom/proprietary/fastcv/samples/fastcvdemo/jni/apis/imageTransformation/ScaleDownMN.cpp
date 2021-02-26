/*
 * Copyright (c) 2013-2014 Qualcomm Technologies Incorporated. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 *
 * @file Implementation of FastCV Transform ScaleDownMN Sample Native interface.
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
#include "ScaleDownMN.h"
#include "utils/FastCVSampleRenderer.h"

//==============================================================================
// Declarations
//==============================================================================

struct CamScaleDownMNState
{

    //---------------------------------------------------------------------------
    /// Constructor for CamScaleDownMNState object sets variables to default values.
    //---------------------------------------------------------------------------

    CamScaleDownMNState()
    {
      dstImgBuf=NULL;
      outImgBuf=NULL;
      alignedImgBuf=NULL;
      imgWidth=0;
      imgHeight=0;
      dstImgWidth=0;
      dstImgHeight=0;
      dstStride=0;
      pref = 0;
      preview = true;
      scalePref = true;
    }

    uint8_t*                   dstImgBuf;

    uint8_t*                   outImgBuf;

/// Buffer to place image data if camera callback data is not aligned
    uint8_t*                   alignedImgBuf;

    /// Width of image
    uint32_t                   imgWidth;

    /// Height of image
    uint32_t                   imgHeight;

    uint32_t                   dstImgWidth;

    uint32_t                   dstImgHeight;

    uint32_t                   dstStride;

    int                        pref ;

    bool                       preview;

    bool                       scalePref;


};
/// Application' state structure to hold global camScaleDownMNState for sample app.
static struct CamScaleDownMNState camScaleDownMNState;

JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageTransformation_ScaleDownMN_cleanup
(
   JNIEnv * env,
   jobject obj
)
{
    DPRINTF( "%s\n", __FUNCTION__ );

   if(camScaleDownMNState.outImgBuf)
   {
       fcvMemFree(camScaleDownMNState.outImgBuf);
       camScaleDownMNState.outImgBuf = NULL;
   }

   if(camScaleDownMNState.alignedImgBuf)
   {
       fcvMemFree(camScaleDownMNState.alignedImgBuf);
       camScaleDownMNState.alignedImgBuf = NULL;
   }

   if (camScaleDownMNState.dstImgBuf)
   {
       fcvMemFree (camScaleDownMNState.dstImgBuf);
       camScaleDownMNState.dstImgBuf = NULL;
   }

}

JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageTransformation_ScaleDownMN_update
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

   int frameSize = sizeof(uint8_t)*(w)*(h);

   if (camScaleDownMNState.dstImgBuf == NULL)
   {
       camScaleDownMNState.dstImgBuf = (uint8_t*)fcvMemAlloc (frameSize*3/2 , 16);
       if (camScaleDownMNState.dstImgBuf == NULL)
       {
           EPRINTF("Allocate camScaleDownMNState.dstImgBuf failed");
           return;
       }
       else
       {
           memset(camScaleDownMNState.dstImgBuf , 0 , w*h);
           memset(camScaleDownMNState.dstImgBuf+(w*h) , 128 , w*h/2);
       }
   }

    // Get data from JNI
   jimgData = env->GetByteArrayElements( img, &isCopy );

   renderBuffer = getRenderBuffer(w , h);

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
      if( w != (int)camScaleDownMNState.imgWidth ||
          h != (int)camScaleDownMNState.imgHeight )
      {
         if(camScaleDownMNState.alignedImgBuf != NULL )
         {
            DPRINTF( "%s %d Creating aligned for camScaleDownMNState.preview\n",
               __FILE__, __LINE__ );
            fcvMemFree(camScaleDownMNState.alignedImgBuf );
            camScaleDownMNState.alignedImgBuf = NULL;
         }
      }

      // Allocate buffer for aligned data if necessary.
      if(camScaleDownMNState.alignedImgBuf == NULL )
      {
          camScaleDownMNState.imgWidth = w;
          camScaleDownMNState.imgHeight = h;
          camScaleDownMNState.alignedImgBuf = (uint8_t*)fcvMemAlloc( w*h*3/2, 16 );
      }

      memcpy(camScaleDownMNState.alignedImgBuf, jimgData, w*h*3/2 );
      pJimgData = camScaleDownMNState.alignedImgBuf;
   }

   else if( w != (int)camScaleDownMNState.imgWidth ||
                h != (int)camScaleDownMNState.imgHeight )
   {
       camScaleDownMNState.imgWidth = w;
       camScaleDownMNState.imgHeight = h;
   }

    switch (camScaleDownMNState.pref)
    {
   case 0:
       camScaleDownMNState.dstImgWidth = camScaleDownMNState.imgWidth;
       camScaleDownMNState.dstImgHeight = camScaleDownMNState.imgHeight;
       break;

   case 1:
       camScaleDownMNState.dstImgWidth = camScaleDownMNState.imgWidth/1.5;
       camScaleDownMNState.dstImgHeight = camScaleDownMNState.imgHeight/1.5;
       break;

   case 2:
       camScaleDownMNState.dstImgWidth = camScaleDownMNState.imgWidth/2;
       camScaleDownMNState.dstImgHeight = camScaleDownMNState.imgHeight/2;
       break;

   case 3:
       camScaleDownMNState.dstImgWidth = camScaleDownMNState.imgWidth/2.5;
       camScaleDownMNState.dstImgHeight = camScaleDownMNState.imgHeight/2.5;
       break;

   default:
       camScaleDownMNState.dstImgWidth = camScaleDownMNState.imgWidth;
       camScaleDownMNState.dstImgHeight = camScaleDownMNState.imgHeight;
       break;
   }

   if( camScaleDownMNState.outImgBuf == NULL)
   {
       camScaleDownMNState.outImgBuf = (uint8_t *)fcvMemAlloc(camScaleDownMNState.dstImgWidth*camScaleDownMNState.dstImgHeight, 16);
       if( camScaleDownMNState.outImgBuf == NULL )
       {
           EPRINTF("Allocate camScaleDownMNState.outImgBuf failed");
           return;
       }
   }

    if (!camScaleDownMNState.preview)
    {
       if (camScaleDownMNState.scalePref)
        {
    fcvScaleDownMNu8(               pJimgData,
                                    camScaleDownMNState.imgWidth,
                                    camScaleDownMNState.imgHeight,
                                    0,
                                    camScaleDownMNState.outImgBuf,
                                    camScaleDownMNState.dstImgWidth,
                                    camScaleDownMNState.dstImgHeight,
                                    0          );
        }
       else
        {
    fcvScaleDownNNu8(               pJimgData,
                                    camScaleDownMNState.imgWidth,
                                    camScaleDownMNState.imgHeight,
                                    0,
                                    camScaleDownMNState.outImgBuf,
                                    camScaleDownMNState.dstImgWidth,
                                    camScaleDownMNState.dstImgHeight,
                                    0          );
        }

           memset(camScaleDownMNState.dstImgBuf , 0 , w*h);
           memset(camScaleDownMNState.dstImgBuf+(w*h) , 128 , w*h/2);

    int p = w - camScaleDownMNState.dstImgWidth;
    int q = h - camScaleDownMNState.dstImgHeight;

    for (i = p; i < w; i++)
        {
        for (int j  = q; j < h; j++ )
            {
            camScaleDownMNState.dstImgBuf[i+j*w] = camScaleDownMNState.outImgBuf[(i-p)+(j-q)*camScaleDownMNState.dstImgWidth];
            }
        }
   }

   else
    {
        memcpy(camScaleDownMNState.dstImgBuf, pJimgData, w * h * 3 / 2);
    }
     colorConvertYUV420ToRGB565Renderer(camScaleDownMNState.dstImgBuf,
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

JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageTransformation_ScaleDownMN_setPreview
(
  JNIEnv*  env,
  jobject  obj,
  bool pre
)
{
    camScaleDownMNState.preview = pre;

    return;
}

JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageTransformation_ScaleDownMN_setScaling
(
  JNIEnv*  env,
  jobject  obj,
  int scale
)
{
    camScaleDownMNState.pref = scale;

    return;
}

JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageTransformation_ScaleDownMN_setScalePref
(
  JNIEnv*  env,
  jobject  obj,
  bool scaleP
)
{
    camScaleDownMNState.scalePref = scaleP;

    return;
}