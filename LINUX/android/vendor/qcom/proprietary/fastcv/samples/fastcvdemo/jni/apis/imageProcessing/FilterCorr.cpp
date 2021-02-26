/*
 * Copyright (c) 2012-2014 Qualcomm Technologies Incorporated. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 * @file Implementation of FastCV Filter Correlation Sample native interface.
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
#include "FilterCorr.h"
#include "utils/FastCVSampleRenderer.h"
#include "utils/FPSCounter.h"

//==============================================================================
// Declarations
//==============================================================================

//------------------------------------------------------------------------------
/// @brief
///    Contains type of kernels.
//------------------------------------------------------------------------------
typedef enum KERNEL_TYPE
{
    RESET,
    THREExTHREE,
    THIRTEENxTHIRTEEN,
} KERNEL_TYPE;

//------------------------------------------------------------------------------
/// @brief
///    Contains FilterCorrState information of the instance of the application.
//------------------------------------------------------------------------------
struct FilterCorrState
{
    //---------------------------------------------------------------------------
    /// Constructor for FilterCorrState object sets variables to default values.
    //---------------------------------------------------------------------------
    FilterCorrState()
    {

        filteredImgBuf = NULL;
        filteredImgWidth = 0;
        filteredImgHeight = 0;

        filteredOutputBufs16 = NULL;

        filteredInputBufs16 = NULL;

        scratchBuffer = NULL;

        alignedImgBuf = NULL;

        imgWidth = 0;
        imgHeight = 0;

        kernelType = THREExTHREE;
    }

    /// Camera preview FPS counter
    FPSCounter                 camFPSCounter;

    /// Image buffer that stores filtered image
    uint8_t*                 filteredImgBuf;

    /// Width of image
    uint32_t                   filteredImgWidth;

    /// Height of image
    uint32_t                   filteredImgHeight;

    /// Intermediate image buffer that stores input
    int16_t*                 filteredInputBufs16;

    /// Intermediate image buffer that stores output
    int16_t*                 filteredOutputBufs16;

    /// Intermediate image buffer that is used for scratch
    int16_t*                scratchBuffer;

    /// Buffer to place image data if camera callback data is not aligned
    uint8_t*                   alignedImgBuf;

    /// Width of image
    uint32_t                   imgWidth;

    /// Height of image
    uint32_t                   imgHeight;

    /// Kernel type selected by user
    KERNEL_TYPE  kernelType ;

};

/// Application' state structure to hold global state for sample app.
static FilterCorrState      filtercorrState;

//==============================================================================
// Function Definitions
//==============================================================================

//------------------------------------------------------------------------------
/// @brief Performs fcv filtercorr functions
/// @param data Pointer to source image data
/// @param w Width of data
/// @param y Height of data
/// @param data Pointer to destination image data
//------------------------------------------------------------------------------
void updateFilterCorr( uint8_t *pSrc, uint32_t w, uint32_t h, uint8_t *pDst )
{
       int i;
       int j;

       //Declaration of kernels
       signed char FASTCV_ALIGN128 ( mask3x3[9] ) = {32, 1, -32, 64, 1, -64, 32, 1, -32 };

       static short FASTCV_ALIGN128( knl13[13] ) = {
          58, 248, 814, 2049, 3964, 5890, 6721, 5890, 3964, 2049, 814, 248, 58 };

       // Allocate the buffer once here if it's not allocated already
       if( filtercorrState.filteredOutputBufs16 == NULL)
       {
           filtercorrState.filteredOutputBufs16 = (int16_t *)fcvMemAlloc(w*h*sizeof(int16_t), 16);
       }

       // Allocate the buffer once here if it's not allocated already
       if( filtercorrState.filteredInputBufs16 == NULL)
       {
           filtercorrState.filteredInputBufs16 = (int16_t *)fcvMemAlloc(w*h*sizeof(int16_t), 16);
       }

       // Allocate the buffer once here if it's not allocated already
       if( filtercorrState.scratchBuffer == NULL)
       {
          filtercorrState.scratchBuffer = (int16_t *)fcvMemAlloc(w*h*sizeof(int16_t), 16);
       }


       // Perform FastCV Function processing
        switch( filtercorrState.kernelType )
        {
            case THREExTHREE:
                fcvFilterCorr3x3s8    ( mask3x3, pSrc, w, h, pDst);
                break;

            //For all other fcvFilterCorr functions, performs conversion from uint8 to int16
            case THIRTEENxTHIRTEEN:
                for(i = 0; i < h; i++)
                   {
                       for(j = 0; j < w; j++)
                       {
                           filtercorrState.filteredInputBufs16[i*w+j] = (int16_t)pSrc[i*w+j];
                       }
                   }
                fcvFilterCorrSep13x13s16    ( knl13, filtercorrState.filteredInputBufs16, w, h,  filtercorrState.scratchBuffer, filtercorrState.filteredOutputBufs16);
                for(i = 0; i < h; i++)
                   {
                       for(j = 0; j < w; j++)
                       {
                           pDst[i*w+j] = (uint8_t) filtercorrState.filteredOutputBufs16[i*w+j];
                       }
                   }
                break;

            case RESET:
                memcpy(pDst, pSrc, w*h);
                break;

            default:
                break;
        }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT void JNICALL
   Java_com_qualcomm_fastcvdemo_apis_imageProcessing_FilterCorr_cleanup
(
   JNIEnv * env,
   jobject obj
)
{
   DPRINTF( "%s\n", __FUNCTION__ );

   if( filtercorrState.filteredImgBuf )
   {
       fcvMemFree(filtercorrState.filteredImgBuf  );
          filtercorrState.filteredImgBuf   = NULL;
   }

   if( filtercorrState.filteredOutputBufs16 )
   {
      fcvMemFree(filtercorrState.filteredOutputBufs16  );
      filtercorrState.filteredOutputBufs16   = NULL;
   }

   if( filtercorrState.filteredInputBufs16 )
   {
      fcvMemFree(filtercorrState.filteredInputBufs16  );
      filtercorrState.filteredInputBufs16   = NULL;
   }

   if( filtercorrState.scratchBuffer )
   {
      fcvMemFree(filtercorrState.scratchBuffer );
      filtercorrState.scratchBuffer   = NULL;
   }

   if( filtercorrState.alignedImgBuf )
   {
      fcvMemFree(filtercorrState.alignedImgBuf  );
      filtercorrState.alignedImgBuf   = NULL;
   }

   fcvCleanUp();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageProcessing_FilterCorr_update
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
   int16_t*  tempSpace    =  NULL;
   uint64_t          time;
   float             timeMs;
   int i;
   int j;

   // Allocate the buffer once here if it's not allocated already
  if( filtercorrState.filteredImgBuf == NULL)
  {
     filtercorrState.filteredImgBuf = (uint8_t *)fcvMemAlloc(w*h*3/2, 16);
     if( filtercorrState.filteredImgBuf == NULL )
     {
        EPRINTF("Allocate gradientImgBuf failed");
        return;
     }
     else
     {
          memset(filtercorrState.filteredImgBuf, 0, w*h);
        memset(filtercorrState.filteredImgBuf+(w*h), 128, w*h/2);
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
   //uint8_t* pFilterCorrData = (uint8_t*)filtercorrState.filteredImgBuf;

   // Check if camera image data is not aligned.
   if( (uintptr_t)jimgData & 0xF )
   {
      // Allow for rescale if dimensions changed.
      if( w != (int)filtercorrState.imgWidth ||
          h != (int)filtercorrState.imgHeight )
      {
         if( filtercorrState.alignedImgBuf != NULL )
         {
            DPRINTF( "%s %d Creating aligned for preview\n",
               __FILE__, __LINE__ );
            fcvMemFree( filtercorrState.alignedImgBuf );
            filtercorrState.alignedImgBuf = NULL;
         }
      }

      // Allocate buffer for aligned data if necessary.
      if( filtercorrState.alignedImgBuf == NULL )
      {
          filtercorrState.imgWidth = w;
          filtercorrState.imgHeight = h;
          filtercorrState.alignedImgBuf = (uint8_t*)fcvMemAlloc( w*h*3/2, 16 );
      }

      memcpy( filtercorrState.alignedImgBuf, jimgData, w*h*3/2 );
      pJimgData = filtercorrState.alignedImgBuf;
    }

   else if( w != (int)filtercorrState.imgWidth ||
                h != (int)filtercorrState.imgHeight )
   {
       filtercorrState.imgWidth = w;
       filtercorrState.imgHeight = h;
   }

   // Perform FastCV Function processing
   updateFilterCorr( (uint8_t*)pJimgData, w, h, filtercorrState.filteredImgBuf );

   colorConvertYUV420ToRGB565Renderer(filtercorrState.filteredImgBuf,
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
//-----------------------------------------
JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageProcessing_FilterCorr_setKernelType
(
  JNIEnv*  env,
  jobject  obj,
  int kernelType
)
{
    filtercorrState.kernelType = (KERNEL_TYPE) kernelType;

    DPRINTF("FilterCorr: setKernelType %d", kernelType);
    if (filtercorrState.filteredImgBuf)
    {
        // set the luma to be black (0)
        memset(filtercorrState.filteredImgBuf, 0, filtercorrState.filteredImgWidth*filtercorrState.filteredImgHeight);
        // set the chroma so that the image will be like black and white (128)
        memset(filtercorrState.filteredImgBuf+(filtercorrState.filteredImgWidth*filtercorrState.filteredImgHeight),
            128, filtercorrState.filteredImgWidth*filtercorrState.filteredImgHeight/2);
    }
    if (filtercorrState.filteredInputBufs16)
    {
        // set the luma to be black (0)
        memset(filtercorrState.filteredInputBufs16, 0, filtercorrState.imgWidth*filtercorrState.imgHeight*sizeof(int16_t));
    }

    if (filtercorrState.filteredOutputBufs16)
    {
        // set the luma to be black (0)
        memset(filtercorrState.filteredOutputBufs16, 0, filtercorrState.imgWidth*filtercorrState.imgHeight*sizeof(int16_t));
    }
}
