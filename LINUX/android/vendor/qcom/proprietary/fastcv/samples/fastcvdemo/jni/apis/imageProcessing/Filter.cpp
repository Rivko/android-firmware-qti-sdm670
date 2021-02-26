/*
 * Copyright (c) 2012-2014 Qualcomm Technologies Incorporated. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 * @file Implementation of FastCV Filter Sample native interface.
 */

//==============================================================================
// Include Files
//==============================================================================
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <stdlib.h>
#include <android/log.h>
#include <fastcv/fastcv.h>

#include "utils/FastCVUtil.h"
#include "Filter.h"
#include "utils/FastCVSampleRenderer.h"
#include <string.h>

//==============================================================================
// Declarations
//==============================================================================

//------------------------------------------------------------------------------
/// @brief
///    Contains type of filters.
//------------------------------------------------------------------------------
typedef enum FILTER_TYPE
{
    RESET,
    ENABLE_GAUSS_3X3,
    ENABLE_GAUSS_5X5,
    ENABLE_GAUSS_11X11,
    ENABLE_CANNY,
    ENABLE_ERODE,
    ENABLE_DILATE,
    ENABLE_MEDIAN,
    ENABLE_SOBEL,
    ENABLE_THRESHOLD
} FILTER_TYPE;

//------------------------------------------------------------------------------
/// @brief
///    Contains filterState information of the instance of the application.
//------------------------------------------------------------------------------
struct FilterState
{
  //---------------------------------------------------------------------------
  /// Constructor for FilterState object sets variables to default values.
  //---------------------------------------------------------------------------
  FilterState()
  {
    filteredImgBuf = NULL;
    filteredImgWidth = 0;
    filteredImgHeight = 0;

    edgeImgBuf = NULL;
    edgeImgWidth = 0;
    edgeImgHeight = 0;

    alignedImgBuf = NULL;

    imgWidth = 0;
    imgHeight = 0;

    filterType = RESET;
  }

  /// Image buffer that stores filtered image
  uint8_t*                   filteredImgBuf;

  /// Width of image
  uint32_t                   filteredImgWidth;

  /// Height of image
  uint32_t                   filteredImgHeight;

  /// Image buffer that stores canny filtered image
  uint8_t*                   edgeImgBuf;

  /// Width of image
  uint32_t                   edgeImgWidth;

  /// Height of image
  uint32_t                   edgeImgHeight;

  /// Buffer to place image data if camera callback data is not aligned
  uint8_t*                   alignedImgBuf;

  /// Width of image
  uint32_t                   imgWidth;

  /// Height of image
  uint32_t                   imgHeight;

  /// Filter type selected by user
  FILTER_TYPE                 filterType ;

};

/// Application' state structure to hold global state for sample app.
static FilterState      filterState;

//==============================================================================
// Function Definitions
//==============================================================================


//------------------------------------------------------------------------------
/// @brief Performs fcv filter functions
/// @param data Pointer to source image data
/// @param w Width of data
/// @param y Height of data
/// @param data Pointer to destination image data
//------------------------------------------------------------------------------
void updateFilter( uint8_t *pSrc, uint32_t w, uint32_t h, uint8_t *pDst )
{
    //Handles filter selection
    switch( filterState.filterType )
    {
        case ENABLE_GAUSS_3X3:
            fcvFilterGaussian3x3u8( pSrc, w, h, pDst, 0);
            break;
        case ENABLE_GAUSS_5X5:
            fcvFilterGaussian5x5u8( pSrc, w, h, pDst, 0);
            break;
        case ENABLE_GAUSS_11X11:
            fcvFilterGaussian11x11u8( pSrc, w, h, pDst, 0 );
            break;
        case ENABLE_CANNY:
            // Allocate new buffer
              if( filterState.edgeImgBuf == NULL )
              {
                  filterState.edgeImgWidth = w;
                  filterState.edgeImgHeight = h;
                  filterState.edgeImgBuf = (uint8_t*)fcvMemAlloc(filterState.edgeImgWidth*filterState.edgeImgHeight, 16 );
              }
            fcvFilterCanny3x3u8( pSrc, filterState.edgeImgWidth, filterState.edgeImgHeight, filterState.edgeImgBuf, 10, 20);
            memcpy(pDst, pSrc, w*h);
            break;
        case ENABLE_ERODE:
            fcvFilterErode3x3u8( pSrc, w, h, pDst );
            break;
        case ENABLE_DILATE:
            fcvFilterDilate3x3u8( pSrc, w, h, pDst );
            break;
        case ENABLE_MEDIAN:
            fcvFilterMedian3x3u8( pSrc, w, h, pDst );
            break;
        case ENABLE_SOBEL:
            fcvFilterSobel3x3u8( pSrc, w, h, pDst );
            break;
        case ENABLE_THRESHOLD:
            fcvFilterThresholdu8( pSrc, w, h, pDst, 128 );
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
   Java_com_qualcomm_fastcvdemo_apis_imageProcessing_Filter_cleanup
(
   JNIEnv * env,
   jobject obj
)
{
   DPRINTF( "%s\n", __FUNCTION__ );

   if( filterState.filteredImgBuf )
   {
     fcvMemFree(filterState.filteredImgBuf  );
     filterState.filteredImgBuf   = NULL;
   }

   if( filterState.alignedImgBuf )
   {
        fcvMemFree(filterState.alignedImgBuf  );
        filterState.alignedImgBuf   = NULL;
   }

   if( filterState.edgeImgBuf )
   {
        fcvMemFree(filterState.edgeImgBuf  );
        filterState.edgeImgBuf   = NULL;
   }

   fcvCleanUp();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageProcessing_Filter_update
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
   if( filterState.filteredImgBuf == NULL)
   {
      int frameSize = w*h*3/2;
      filterState.filteredImgBuf = (uint8_t *)fcvMemAlloc(frameSize, 16);
      if( filterState.filteredImgBuf == NULL )
      {
         EPRINTF("Allocate filteredImgBuf failed");
         return;
      }
      else
      {
         memset(filterState.filteredImgBuf, 128, frameSize);
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
   uint8_t* pFilteringData = (uint8_t*)filterState.filteredImgBuf;

   // Check if camera image data is not aligned.
   if( (uintptr_t)jimgData & 0xF )
   {
      // Allow for rescale if dimensions changed.
      if( w != (int)filterState.imgWidth ||
          h != (int)filterState.imgHeight )
      {
         if( filterState.alignedImgBuf != NULL )
         {
            DPRINTF( "%s %d Creating aligned for preview\n",
               __FILE__, __LINE__ );
            fcvMemFree( filterState.alignedImgBuf );
            filterState.alignedImgBuf = NULL;
         }
      }

      // Allocate buffer for aligned data if necessary.
      if( filterState.alignedImgBuf == NULL )
      {
          filterState.imgWidth = w;
          filterState.imgHeight = h;
          filterState.alignedImgBuf = (uint8_t*)fcvMemAlloc( w*h*3/2, 16 );
      }

      memcpy( filterState.alignedImgBuf, jimgData, w*h*3/2 );
      pJimgData = filterState.alignedImgBuf;
    }

   else if( w != (int)filterState.imgWidth ||
                h != (int)filterState.imgHeight )
   {
       filterState.imgWidth = w;
       filterState.imgHeight = h;
   }

   // Perform FastCV Function processing
   updateFilter( (uint8_t*)pJimgData, w, h, (uint8_t*)pFilteringData );

   // Copy the image first in our own buffer to avoid corruption during
   // rendering. Not that we can still have corruption in image while we do
   // copy but we can't help that.

   colorConvertYUV420ToRGB565Renderer(pFilteringData,
                            w,
                            h,
                            (uint32_t*)renderBuffer );

   // Update image
   timeMs = ( util.getTimeMicroSeconds() - time ) / 1000.f;
   util.setProcessTime((util.getProcessTime()*(29.f/30.f))+(float)(timeMs/30.f));

   if( filterState.filterType == ENABLE_CANNY )
   {
       drawEdges(filterState.edgeImgBuf, filterState.edgeImgHeight, filterState.edgeImgWidth);
   }

   unlockRenderBuffer();

   // Let JNI know we don't need data anymore
   env->ReleaseByteArrayElements( img, jimgData, JNI_ABORT );
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageProcessing_Filter_setFilter
(
  JNIEnv*  env,
  jobject  obj,
  int filterType
)
{
    filterState.filterType = (FILTER_TYPE) filterType;

    if( filterState.filteredImgBuf )
    {
        // set the luma to be black (0)
        memset(filterState.filteredImgBuf, 0, filterState.filteredImgWidth*filterState.filteredImgHeight);

        // set the chroma so that the image will be like black and white (128)
        memset(filterState.filteredImgBuf+(filterState.filteredImgWidth*filterState.filteredImgHeight),
                128, filterState.filteredImgWidth*filterState.filteredImgHeight/2);
    }
}
