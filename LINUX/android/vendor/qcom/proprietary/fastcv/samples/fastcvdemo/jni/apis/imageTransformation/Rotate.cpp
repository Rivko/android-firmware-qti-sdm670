
/*
 * Copyright (c) 2012-2014 Qualcomm Technologies Incorporated. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 *
 * @file Implementation of FastCV Copy Rotated Sample native interface.
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
#include "Rotate.h"
#include "utils/FastCVSampleRenderer.h"

//==============================================================================
// Declarations
//==============================================================================
#define SRC_TILE_WIDTH     25
#define DST_TILE_WIDTH     17

typedef enum ROTATE_ANGLE
{
   ZERO,
   NINETY,
   ONE_EIGHTY,
   TWO_SEVENTY,
   RESET
} ROTATE_ANGLE;


//------------------------------------------------------------------------------
/// @brief
///    Contains state information of the instance of the application.
//------------------------------------------------------------------------------
struct RotateState
{
   //---------------------------------------------------------------------------
   /// Constructor for State object sets variables to default values.
   //---------------------------------------------------------------------------
   RotateState()
   {
      rotatedImgBuf = NULL;
      alignedImgBuf = NULL;

      imgWidth = 0;
      imgHeight = 0;

      rotatedSrc = NULL;
      rotatedDst = NULL;

      nHorTile = 0;
      nVerTile = 0;

      rotateAngle = RESET;
      initDone = false;
   }

   /// Intermediate image buffer that stores image
   uint8_t*                   rotatedImgBuf;

   // Temporary image buffer
   uint8_t*                   rotatedSrc;

   // Temporary image buffer
   uint8_t*                   rotatedDst;

   /// Buffer to place image data if camera callback data is not aligned
   uint8_t*                   alignedImgBuf;

   uint32_t                   imgWidth;

   uint32_t                   imgHeight;

   int                        nHorTile;

   int                        nVerTile;

   /// Rotation angle selected by user
   ROTATE_ANGLE               rotateAngle;

   bool                       initDone;
};

/// Application' state structure to hold global state for sample app.
static RotateState      rotateState;

//==============================================================================
// Function Definitions
//==============================================================================

//------------------------------------------------------------------------------
/// @brief Performs rotatation
/// @param pSrc Pointer to image data to perform processing
/// @param pDst Pointer to destination data
//------------------------------------------------------------------------------
void updateRotate( uint8_t *pSrc, uint32_t w, uint32_t h, uint8_t *pDst)
{
    int angle;
    switch( rotateState.rotateAngle )
    {
       case ZERO:
           angle = 0;
           break;
       case NINETY:
           angle = 256;
           break;
       case ONE_EIGHTY:
           angle = 512;
           break;
       case TWO_SEVENTY:
           angle = 768;
           break;
       default:
           break;
    }

    if ( rotateState.rotateAngle != RESET )
    {
      for( int i = 0; i < rotateState.nVerTile; i++ )
      {
         for( int j = 0; j < rotateState.nHorTile; j++ )
         {
            int posSrc = (i*SRC_TILE_WIDTH*rotateState.imgWidth) + j*SRC_TILE_WIDTH;
            for ( int k = 0; k < SRC_TILE_WIDTH; k++)
            {
               memcpy (&rotateState.rotatedSrc[k*SRC_TILE_WIDTH], &pSrc[posSrc], SRC_TILE_WIDTH);
               posSrc += rotateState.imgWidth;
            }

            fcvCopyRotated17x17u8(rotateState.rotatedSrc, rotateState.rotatedDst, angle);

            int posDst = 0;
            switch( rotateState.rotateAngle )
            {
               case ONE_EIGHTY:
                   posDst = ((rotateState.nVerTile-i)*DST_TILE_WIDTH*rotateState.imgWidth) - (j+1)*DST_TILE_WIDTH;
                   break;
               case NINETY:
                   //Throws out pixels outside of square of size nVerTile*nVerTile to avoid corrupted chroma
                   if (j > (rotateState.nHorTile - rotateState.nVerTile-1))
                   {
                       posDst = ((rotateState.nHorTile-1-j)*DST_TILE_WIDTH*rotateState.imgWidth) + i*DST_TILE_WIDTH + 256;
                   }
                   break;
               case TWO_SEVENTY:
                   //Throws out pixels outside of square of size nVerTile*nVerTile to avoid corrupted chroma
                   if (j < rotateState.nVerTile)
                   {
                       posDst = (j*DST_TILE_WIDTH*rotateState.imgWidth) + (rotateState.nVerTile-1-i)*DST_TILE_WIDTH + 256;
                   }
                   break;
               case ZERO:
               default:
                   posDst = (i*DST_TILE_WIDTH*rotateState.imgWidth) + j*DST_TILE_WIDTH;
                   break;
            }
            for ( int k = 0; k < DST_TILE_WIDTH; k++)
            {
               //Only copies pixels that have been given new posDst
               if (rotateState.rotateAngle == NINETY && j > (rotateState.nHorTile - rotateState.nVerTile-1))
               {
                   memcpy (&pDst[posDst], &rotateState.rotatedDst[k*DST_TILE_WIDTH], DST_TILE_WIDTH);
               }
               else if (rotateState.rotateAngle == TWO_SEVENTY && j < rotateState.nVerTile)
               {
                   memcpy (&pDst[posDst], &rotateState.rotatedDst[k*DST_TILE_WIDTH], DST_TILE_WIDTH);
               }
               else if (rotateState.rotateAngle != NINETY && rotateState.rotateAngle != TWO_SEVENTY)
               {
                   memcpy (&pDst[posDst], &rotateState.rotatedDst[k*DST_TILE_WIDTH], DST_TILE_WIDTH);
               }
               posDst += rotateState.imgWidth;
            }
         }
      }
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageTransformation_Rotate_cleanup
(
   JNIEnv * env,
   jobject obj
)
{

   if( rotateState.rotatedImgBuf )
   {
      fcvMemFree( rotateState.rotatedImgBuf );
      rotateState.rotatedImgBuf = NULL;
   }

   if( rotateState.alignedImgBuf )
   {
      fcvMemFree( rotateState.alignedImgBuf );
      rotateState.alignedImgBuf = NULL;
   }

   if( rotateState.rotatedSrc )
   {
      fcvMemFree(rotateState.rotatedSrc );
      rotateState.rotatedSrc  = NULL;
   }

   if( rotateState.rotatedDst )
   {
      fcvMemFree(rotateState.rotatedDst );
      rotateState.rotatedDst  = NULL;
   }

   fcvCleanUp();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageTransformation_Rotate_update
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

   uint8_t* pJimgData   = (uint8_t*)jimgData;

   // Check if camera image data is not aligned.
   if( (uintptr_t)jimgData & 0xF )
   {
      // Allow for rescale if dimensions changed.
      if( w != (int)rotateState.imgWidth ||
          h != (int)rotateState.imgHeight )
      {
         if( rotateState.alignedImgBuf != NULL )
         {
            DPRINTF( "%s %d Creating aligned for preview\n",
               __FILE__, __LINE__ );
            fcvMemFree( rotateState.alignedImgBuf );
            rotateState.alignedImgBuf = NULL;
         }
      }

      // Allocate buffer for aligned data if necessary.
      if( rotateState.alignedImgBuf == NULL )
      {
         rotateState.alignedImgBuf = (uint8_t*)fcvMemAlloc( w*h*3/2, 16 );
      }

      memcpy( rotateState.alignedImgBuf, jimgData, w*h*3/2 );
      pJimgData = rotateState.alignedImgBuf;
   }

   // Allocate the buffer once here if it's not allocated already
   if( rotateState.rotatedImgBuf == NULL)
   {
      int frameSize = w*h*3/2;
      rotateState.rotatedImgBuf = (uint8_t *)fcvMemAlloc(frameSize, 16);
      if( rotateState.rotatedImgBuf == NULL )
      {
         EPRINTF("Allocate rotateImgBuf failed");
      }
      else
      {
     memset(rotateState.rotatedImgBuf, 0, w*h);
     memset(rotateState.rotatedImgBuf+(w*h), 128, w*h/2);
      }
   }

   if( rotateState.rotatedSrc == NULL)
   {
      int frameSize = SRC_TILE_WIDTH*SRC_TILE_WIDTH;
      rotateState.rotatedSrc = (uint8_t *)fcvMemAlloc(frameSize, 16);
      if( rotateState.rotatedSrc == NULL )
      {
         EPRINTF("Allocate tempImgBuf1 failed");
      }
      else
      {
         memset(rotateState.rotatedSrc, 128, frameSize);
      }
   }

   if( rotateState.rotatedDst == NULL)
   {
      int frameSize = DST_TILE_WIDTH * DST_TILE_WIDTH;
      rotateState.rotatedDst = (uint8_t *)fcvMemAlloc(frameSize, 16);
      if( rotateState.rotatedDst == NULL )
      {
         EPRINTF("Allocate tempImgBuf1 failed");
      }
      else
      {
         memset(rotateState.rotatedDst, 128, frameSize);
      }
   }

   if( !rotateState.initDone )
   {
      rotateState.imgWidth = w;
      rotateState.imgHeight = h;

      // Intialize the nHorTile and nVerTile and Image width and height
      rotateState.nHorTile = w / SRC_TILE_WIDTH;
      rotateState.nVerTile = h / SRC_TILE_WIDTH;
      rotateState.initDone = true;
   }

   // Perform FastCV Function processing
   if (rotateState.rotateAngle == RESET)
   {
       colorConvertYUV420ToRGB565Renderer( (uint8_t*)pJimgData, w, h, (uint32_t*)renderBuffer );
   }
   else
   {
       updateRotate((uint8_t*)pJimgData, w, h, rotateState.rotatedImgBuf);
       colorConvertYUV420ToRGB565Renderer( rotateState.rotatedImgBuf, w, h, (uint32_t*)renderBuffer );
   }

   // Update image
   timeMs = ( util.getTimeMicroSeconds() - time ) / 1000.f;
   util.setProcessTime((util.getProcessTime()*(29.f/30.f))+(float)(timeMs/30.f));

   unlockRenderBuffer();

   // Let JNI know we don't need data anymore
   env->ReleaseByteArrayElements( img, jimgData, JNI_ABORT );
}


//------------------------------------------------------------------------------
/// @brief Sets the angle to rotate the image
///
/// @param rotateAngle  Angle that the image is rotated
//------------------------------------------------------------------------------
JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageTransformation_Rotate_setRotation
(
  JNIEnv*  env,
  jobject  obj,
  int rotateAngle
)
{
    rotateState.rotateAngle = (ROTATE_ANGLE) rotateAngle;
    if (rotateState.rotatedImgBuf)
    {
        memset(rotateState.rotatedImgBuf, 0, rotateState.imgWidth*rotateState.imgHeight);
        memset(rotateState.rotatedImgBuf+(rotateState.imgWidth*rotateState.imgHeight),
            128, rotateState.imgWidth*rotateState.imgHeight/2);
    }
}

