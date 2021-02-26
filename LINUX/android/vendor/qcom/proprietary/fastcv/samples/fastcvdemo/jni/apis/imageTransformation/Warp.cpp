/*
 * Copyright (c) 2013-2014 Qualcomm Technologies Incorporated. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 *
 * @file Implementation of FastCV Warp Sample native interface.
 *
 */

//==============================================================================
// Include Files
//==============================================================================
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <stdlib.h>
#include <stdio.h>
#include <android/log.h>
#include <time.h>
#include <fastcv/fastcv.h>
#include <string.h>
#include "utils/FastCVUtil.h"
#include "Warp.h"
#include "utils/FastCVSampleRenderer.h"

//==============================================================================
// Declarations
//==============================================================================

#define FRAME_COUNT     360
#define INDICES         4
#define N_MATRIX        10
#define MATRIX_SIZE     9

typedef enum WARP_TYPE
{
    NO_WARP,
    WARP_P_U8,
    WARP_P_3CHANNEL,
} WARP_TYPE;


//------------------------------------------------------------------------------
/// @brief
///    Contains state information of the instance of the application.
//------------------------------------------------------------------------------
struct WarpState
{
   //---------------------------------------------------------------------------
   /// Constructor for WarpState object sets variables to default values.
   //---------------------------------------------------------------------------
   WarpState()
   {
            warpedYuv420ImgBuf = NULL;
            warpedImgWidth = 0;
            warpedImgHeight = 0;

            alignedImgBuf = NULL;
            alignedImgWidth = 0;
            alignedImgHeight = 0;

            warpType = WARP_P_U8;

            frameCount = 0;
            currentIndex = 0;
            pCurrentMatrix = 0;

            temp=NULL;
            dstY=NULL;
            dstCb=NULL;
            dstCr=NULL;
            RGBImgBuf=NULL;
            RGBDstBuf=NULL;

   }


    uint8_t*                   warpedYuv420ImgBuf;

    /// Width of warped image
    uint32_t                   warpedImgWidth;

    /// Height of warped image
    uint32_t                   warpedImgHeight;

    /// Buffer to place image data if camera callback data is not aligned
    uint8_t*                   alignedImgBuf;

    /// Width of aligned image
    uint32_t                   alignedImgWidth;

    /// Height of aligned image
    uint32_t                   alignedImgHeight;

    // Warp option
    WARP_TYPE                  warpType;

    uint32_t                   frameCount;

    uint32_t                   currentIndex;

    float                      *pCurrentMatrix;


    uint32_t*                   temp;

    uint8_t*                    dstY;

    uint8_t*                    dstCb;

    uint8_t*                    dstCr;

    uint8_t*                    RGBImgBuf;

    uint8_t*                    RGBDstBuf;

};
float warpMatrix[N_MATRIX][MATRIX_SIZE] =
                { {0.6, 0.3, -5.26764727, -0.3, 0.6, -2.48927665, 0, 0, 1},
            {0.8, 0.3, -5.26764727, -0.3, 0.8, -2.48927665, 0, 0, 1},
            {0.98480773, 0.173648179, -5.26764727, -0.173648179, 0.98480773,-2.48927665, 0, 0, 1},
            {1.08377993, -0.023650359, -0.110371135,0.019296616, 1.00467873, 0.016489079, 0, 0, 1},
            {1, 0, -0.1, 0, 1, 0.05, 0, 0, 1}, //Original Image
            {1, 0, -0.1, 0, 1, 0.05, 0, 0, 1},
            {1.08377993, -0.023650359, -0.110371135,0.019296616, 1.00467873, 0.016489079, 0, 0, 1},
            {1, 0.173648179, -.426764727, -0.1473648179, 1.298480773,0.0748927665, 0, 0, 1},
            {1, 0.173648179, -.526764727, -0.1573648179, 1.398480773,0.0248927665, 0, 0, 1},
            {1, 0.173648179, -3.26764727, -0.1673648179, 1.498480773,0.048927665, 0, 0, 1} };
/// Application' state structure to hold global state for sample app.
static WarpState      warpState;




//==============================================================================
// Function Definitions
//==============================================================================

//------------------------------------------------------------------------------
/// @brief Performs warp on image data
///
/// @param data Pointer to image data to perform processing
/// @param w Width of data
/// @param y Height of data
//------------------------------------------------------------------------------
void updateWarp( uint8_t *pSrc, uint32_t w, uint32_t h, uint8_t *pDst)
{
    if( warpState.frameCount > FRAME_COUNT )
    {
        warpState.frameCount = 0;
    }

    if( warpState.frameCount <= 30 )
        warpState.pCurrentMatrix = warpMatrix[0];
    else if( warpState.frameCount <= 60 )
        warpState.pCurrentMatrix = warpMatrix[1];
    else if( warpState.frameCount <= 90 )
        warpState.pCurrentMatrix = warpMatrix[2];
    else if( warpState.frameCount <= 120 )
        warpState.pCurrentMatrix = warpMatrix[3];
    else if( warpState.frameCount <= 150 )
        warpState.pCurrentMatrix = warpMatrix[4];
    else if( warpState.frameCount <= 180 )
        warpState.pCurrentMatrix = warpMatrix[5];
    else if( warpState.frameCount <= 210 )
        warpState.pCurrentMatrix = warpMatrix[6];
    else if( warpState.frameCount <= 240 )
        warpState.pCurrentMatrix = warpMatrix[7];
    else if( warpState.frameCount <= 270 )
        warpState.pCurrentMatrix = warpMatrix[8];
    else
        warpState.pCurrentMatrix = warpMatrix[9];

    // Increment Frame counter
    warpState.frameCount++;

    switch ( warpState.warpType )
    {
        case WARP_P_U8:
            fcvWarpPerspectiveu8_v2( pSrc, w, h, w, pDst, w, h, w, (float*)warpState.pCurrentMatrix );
            break;

        case WARP_P_3CHANNEL:
            fcv3ChannelWarpPerspectiveu8_v2(pSrc, w, h, 3*w, pDst, w, h, 3*w, (float*)warpState.pCurrentMatrix );
            break;

        case NO_WARP:
            memcpy (pDst, pSrc, w*h);
            break;

        default:
            memcpy (pDst, pSrc, w*h);
            break;
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageTransformation_Warp_cleanup
(
    JNIEnv * env,
    jobject obj
)
{
    if( warpState.warpedYuv420ImgBuf )
    {
        fcvMemFree(warpState.warpedYuv420ImgBuf );
        warpState.warpedYuv420ImgBuf  = NULL;
    }

    if( warpState.alignedImgBuf != NULL )
    {
        fcvMemFree( warpState.alignedImgBuf );
        warpState.alignedImgBuf = NULL;
    }

    if (warpState.temp)
    {
       fcvMemFree(warpState.temp);
       warpState.temp = NULL;
    }
    if (warpState.RGBImgBuf)
    {
        fcvMemFree(warpState.RGBImgBuf);
        warpState.RGBImgBuf = NULL;
    }
    if (warpState.RGBDstBuf)
    {
       fcvMemFree(warpState.RGBDstBuf);
       warpState.RGBDstBuf = NULL;
    }
    if (warpState.dstY)
    {
        fcvMemFree(warpState.dstY);
        warpState.dstY = NULL;
    }
    if (warpState.dstCb)
    {
        fcvMemFree(warpState.dstCb);
        warpState.dstCb = NULL;
    }
    if (warpState.dstCr)
    {
        fcvMemFree(warpState.dstCr);
        warpState.dstCr = NULL;
    }

    fcvCleanUp();
}


//------------------------------------------------------------------------------
/// @brief Sets the warp option to be applied
///
/// @param warpType  Warp Factor by which to warp image
//------------------------------------------------------------------------------
JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageTransformation_Warp_setWarp
    (
        JNIEnv*  env,
        jobject  obj,
        int warpType
    )
    {
        warpState.warpType = (WARP_TYPE) warpType;

        if( warpState.warpedYuv420ImgBuf )
        {
        // set the luma to be black (0)
        memset(warpState.warpedYuv420ImgBuf, 0, warpState.alignedImgWidth*warpState.alignedImgHeight);

        // set the chroma so that the image will be like black and white (128)
        memset(warpState.warpedYuv420ImgBuf +(warpState.alignedImgWidth*warpState.alignedImgHeight),
                128, warpState.alignedImgWidth*warpState.alignedImgHeight/2);
        }
    }


//------------------------------------------------------------------------------
/// @brief Calls FastCV API
/// @param img Pointer to camera image
/// @param w Width of image
/// @param y Height of height
//------------------------------------------------------------------------------
JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageTransformation_Warp_update
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
    uint8_t*          renderBuffer;
    uint64_t          time;
    float             timeMs;
    int               w2;
    int               h2;
    int               i;

    // Allocate the buffer once here if it's not allocated already
    if( warpState.warpedYuv420ImgBuf == NULL)
    {
        int frameSize = w*h*3/2;
        warpState.warpedYuv420ImgBuf = (uint8_t *)fcvMemAlloc(frameSize, 16);
        if( warpState.warpedYuv420ImgBuf == NULL )
        {
            EPRINTF("Allocate warpedImgBuf1 failed");
            return;
        }
        else
        {
            memset(warpState.warpedYuv420ImgBuf, 128, frameSize);
        }
    }

    // Get data from JNI
    jimgData = env->GetByteArrayElements( img, &isCopy );

    renderBuffer = getRenderBuffer( w, h );

    lockRenderBuffer();

    time = util.getTimeMicroSeconds();

    uint8_t*  pJimgData = (uint8_t*)jimgData;
    uint8_t*  pYuv420Data = (uint8_t*)warpState.warpedYuv420ImgBuf;

    // Check if camera image data is not aligned.
    if( (uintptr_t)jimgData & 0xF )
    {
        // Allow for rescale if dimensions changed.
        if( w != (int)warpState.alignedImgWidth ||
            h != (int)warpState.alignedImgHeight )
        {
            if( warpState.alignedImgBuf != NULL )
            {
                fcvMemFree( warpState.alignedImgBuf );
                warpState.alignedImgBuf = NULL;
            }
        }

        // Allocate buffer for aligned data if necessary.
        if( warpState.alignedImgBuf == NULL )
        {
            warpState.alignedImgWidth = w;
            warpState.alignedImgHeight = h;
            warpState.alignedImgBuf = (uint8_t*)fcvMemAlloc( w*h*3/2, 16 );
        }

        memcpy( warpState.alignedImgBuf, jimgData, w*h*3/2 );
        pJimgData = warpState.alignedImgBuf;
    }
    //Allocate Memeory to WarpState Vars
    if( warpState.temp == NULL) {
                warpState.temp = (uint32_t *)fcvMemAlloc(4*sizeof(uint8_t)*w*h, 16);
                if( warpState.temp == NULL ) {
                    EPRINTF("Allocate warpState.temp failed");
                    return;
                }
            }

    if( warpState.dstY == NULL) {
                warpState.dstY = (uint8_t *)fcvMemAlloc(sizeof(uint8_t)*w*h, 16);
                if( warpState.dstY == NULL ) {
                    EPRINTF("Allocate warpState.dstY failed");
                    return;
                }
            }

    if( warpState.dstCb == NULL) {
                warpState.dstCb = (uint8_t *)fcvMemAlloc(sizeof(uint8_t)*w*h/4, 16);
                if( warpState.dstCb == NULL ) {
                    EPRINTF("Allocate warpState.dstCb failed");
                    return;
                }
            }

    if( warpState.dstCr == NULL) {
                warpState.dstCr = (uint8_t *)fcvMemAlloc(sizeof(uint8_t)*w*h/4, 16);
                if( warpState.dstCr == NULL ) {
                    EPRINTF("Allocate warpState.dstCr failed");
                    return;
                }
            }

    if (warpState.RGBImgBuf == NULL) {
                warpState.RGBImgBuf = (uint8_t*) fcvMemAlloc(3*sizeof(uint8_t)*w*h, 16);
                if (warpState.RGBImgBuf == NULL) {
                    EPRINTF("Allocate warpState.RGBImgBuf failed");
                }
            }

    if (warpState.RGBDstBuf == NULL) {
                warpState.RGBDstBuf = (uint8_t*) fcvMemAlloc(3*sizeof(uint8_t)*w*h, 16);
                if (warpState.RGBDstBuf == NULL) {
                    EPRINTF("Allocate warpState.RGBDstBuf failed");
                }
            }

    // Perform FastCV Function Processing
    switch ( warpState.warpType )
    {
        case WARP_P_U8:
            updateWarp( (uint8_t*)pJimgData, w, h,(uint8_t*)pYuv420Data );
            memset(pYuv420Data+(w*h) , 128 , w*h/2);
            colorConvertYUV420ToRGB565Renderer(pYuv420Data, w, h, (uint32_t*)renderBuffer );
            break;

        case WARP_P_3CHANNEL:

            fcvColorYUV420toRGB8888u8( pJimgData, w, h, warpState.temp);
            for ( i = 0; i < w*h; i++ ) {
                uint32_t a = warpState.temp[i];
                uint8_t b;
                b = a >> 16;
                warpState.RGBImgBuf[3*i] = b;
                b = a >> 8;
                warpState.RGBImgBuf[3*i+1] = b;
                b = a;
                warpState.RGBImgBuf[3*i+2] = b;
            }
            updateWarp(warpState.RGBImgBuf, w, h, warpState.RGBDstBuf);
            fcvColorRGB888ToYCbCr420Planaru8( (const uint8_t*)warpState.RGBDstBuf, w, h, 3*w, warpState.dstY, warpState.dstCb, warpState.dstCr, w, w/2, w/2);
            memcpy (pYuv420Data, warpState.dstY, w*h);
            for (i = 0; i < w*h/4; i++) {
                pYuv420Data[w*h+2*i] = warpState.dstCr[i];
                pYuv420Data[w*h+2*i+1] = warpState.dstCb[i];
            }
            //memset(pYuv420Data+(w*h) , 128 , w*h/2);
            colorConvertYUV420ToRGB565Renderer(pYuv420Data, w, h, (uint32_t*)renderBuffer );
            break;

        case NO_WARP:
        default:
            colorConvertYUV420ToRGB565Renderer(pJimgData, w, h, (uint32_t*)renderBuffer );
            break;
    }

    // Update image
    timeMs = ( util.getTimeMicroSeconds() - time ) / 1000.f;
    util.setProcessTime((util.getProcessTime()*(29.f/30.f))+(float)(timeMs/30.f));

    unlockRenderBuffer();

    // Let JNI know we don't need data anymore
    env->ReleaseByteArrayElements( img, jimgData, JNI_ABORT );
}
