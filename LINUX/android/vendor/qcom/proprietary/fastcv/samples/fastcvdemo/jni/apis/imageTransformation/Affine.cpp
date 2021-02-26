/*
 * Copyright (c) 2012-2014 Qualcomm Technologies Incorporated. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 *
 * @file Implementation of FastCV Transform Affine Sample native interface.
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
#include "Affine.h"
#include "utils/FastCVSampleRenderer.h"

//==============================================================================
// Declarations
//==============================================================================
typedef enum AFFINE_TYPE
{
   NO_AFFINE,
   AFFINE_U8,
   AFFINE_8X8
} AFFINE_TYPE;


//------------------------------------------------------------------------------
/// @brief
///    Contains state information of the instance of the application.
//------------------------------------------------------------------------------
struct AffineState
{
    //---------------------------------------------------------------------------
    /// Constructor for affineState object sets variables to default values.
    //---------------------------------------------------------------------------
    AffineState()
    {
        affineImgBuf = NULL;

        alignedImgBuf = NULL;
        alignedImgWidth = 0;
        alignedImgHeight = 0;

        // Possible Affine Matrices
        //{0,0,0,0};
        //{0.045205586, 1.2742757, -1.271442, 0.011153019};
        affineFloatMatrix[0] = 0.013717344;
        affineFloatMatrix[1] = 0.69070274;
        affineFloatMatrix[2] = -0.67423344;
        affineFloatMatrix[3] = 0.024732463;
        for ( int i = 0; i < 4; i++ )
        {
            affineIntMatrix[i] = ( int32_t )( affineFloatMatrix[i] * ( 1<<16 ) );
        }
        DPRINTF("Affine integer matrix %d %d %d %d", affineIntMatrix[0], affineIntMatrix[1],
                affineIntMatrix[2], affineIntMatrix[3]);
    }

    float      affineFloatMatrix[4];
    int32_t  affineIntMatrix[4];

    /// Intermediate image buffer that stores image
    uint8_t*                   affineImgBuf;

    /// Width of image
    uint32_t                   affineImgWidth;

    /// Height of image
    uint32_t                   affineImgHeight;

    /// Buffer to place image data if camera callback data is not aligned
    uint8_t*                   alignedImgBuf;

    /// Width of aligned image
    uint32_t                   alignedImgWidth;

    /// Height of aligned image
    uint32_t                   alignedImgHeight;

    /// Affine type selected by user
    AFFINE_TYPE                affineType ;
};

/// Application' state structure to hold global state for sample app.
static AffineState      affineState;

//==============================================================================
// Function Definitions
//==============================================================================

//------------------------------------------------------------------------------
/// @brief Performs transform affine
/// @param pSrc Pointer to image data to perform processing
/// @param w Width of data
/// @param y Height of data
/// @param pDst Pointer to destination data
//------------------------------------------------------------------------------
void updateAffine( uint8_t *pSrc, uint32_t w, uint32_t h, uint8_t *pDst )
{
    switch( affineState.affineType )
    {
        case AFFINE_8X8:
            for( int y = 0; y < h-8; y+=8 )
            {
                for( int x = 0; x < w-8; x+=8 )
                {
                    int FASTCV_ALIGN128(nPosInteger[2]); // = {x, y};
                    nPosInteger[0] = ( int32_t )( (float)x *( 1<<16 ) );
                    nPosInteger[1] = ( int32_t )( (float)y *( 1<<16 ) );
                    fcvTransformAffine8x8u8_v2( pSrc, w, h, w, nPosInteger, affineState.affineIntMatrix,
                                                pDst+y*w+x, w );
                }
            }
            break;

        case AFFINE_U8:
            memset(pDst, 0, w*h);
            for( int y = 0; y < h-128; y+=128 )
            {
                for( int x = 0; x < w-128; x+=128 )
                {
                    float FASTCV_ALIGN128(nPosFloat[2])= {(float)x, (float)y};
                    fcvTransformAffineu8_v2( pSrc, w, h, w, nPosFloat, affineState.affineFloatMatrix,
                                             pDst+y*w+x, 128, 128, w );
                }
            }
            break;

        default:
            memcpy(pDst, pSrc, w*h);
            break;
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageTransformation_Affine_cleanup
(
   JNIEnv * env,
   jobject obj
)
{
    if( affineState.affineImgBuf != NULL )
    {
        fcvMemFree( affineState.affineImgBuf );
        affineState.affineImgBuf = NULL;
    }

    if( affineState.alignedImgBuf != NULL )
    {
        fcvMemFree( affineState.alignedImgBuf );
        affineState.alignedImgBuf = NULL;
    }

    fcvCleanUp();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageTransformation_Affine_update
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
    if( affineState.affineImgBuf == NULL)
    {
        int frameSize = w*h*3/2;
        affineState.affineImgBuf = (uint8_t *)fcvMemAlloc(frameSize, 16);
        if( affineState.affineImgBuf == NULL )
        {
            EPRINTF("Allocate affineImgBuf failed");
            return;
        }
        else
        {
            memset(affineState.affineImgBuf, 0, w*h);
            memset(affineState.affineImgBuf+(w*h), 128, w*h/2);
        }
    }

    // Get data from JNI
    jimgData = env->GetByteArrayElements( img, &isCopy );

    renderBuffer = getRenderBuffer( w, h );

    lockRenderBuffer();

    time = util.getTimeMicroSeconds();

    uint8_t* pJimgData    = (uint8_t*)jimgData;

    // Check if camera image data is not aligned.
    if( (uintptr_t)jimgData & 0xF )
    {
        // Allow for rescale if dimensions changed.
        if( w != (int)affineState.alignedImgWidth ||
            h != (int)affineState.alignedImgHeight )
        {
            if( affineState.alignedImgBuf != NULL )
            {
                DPRINTF( "%s %d Creating aligned for preview\n",
                        __FILE__, __LINE__ );
                fcvMemFree( affineState.alignedImgBuf );
                affineState.alignedImgBuf = NULL;
            }
        }

        // Allocate buffer for aligned data if necessary.
        if( affineState.alignedImgBuf == NULL )
        {
            affineState.alignedImgWidth = w;
            affineState.alignedImgHeight = h;
            affineState.alignedImgBuf = (uint8_t*)fcvMemAlloc( w*h*3/2, 16 );
        }

        memcpy( affineState.alignedImgBuf, jimgData, w*h*3/2 );
        pJimgData = affineState.alignedImgBuf;
    }

    // Perform FastCV Function processing
    switch( affineState.affineType )
    {
        case AFFINE_U8:
            updateAffine( (uint8_t*)pJimgData, w, h, affineState.affineImgBuf);
            colorConvertYUV420ToRGB565Renderer(affineState.affineImgBuf, w, h, (uint32_t*)renderBuffer );
            break;

        case AFFINE_8X8:
            updateAffine( (uint8_t*)pJimgData, w, h, affineState.affineImgBuf);
            colorConvertYUV420ToRGB565Renderer(affineState.affineImgBuf, w, h, (uint32_t*)renderBuffer );
            break;

        case NO_AFFINE:
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


//------------------------------------------------------------------------------
/// @brief Sets the scaling factor to be applied
///
/// @param scaleFactor  Scaling Factor by which to downsample image
//------------------------------------------------------------------------------
JNIEXPORT void JNICALL Java_com_qualcomm_fastcvdemo_apis_imageTransformation_Affine_setAffine
(
  JNIEnv*  env,
  jobject  obj,
  int affineType
)
{
    affineState.affineType = (AFFINE_TYPE) affineType;
    DPRINTF("Affine type set to %d", affineState.affineType);

    if( affineState.affineImgBuf )
    {
        memset( affineState.affineImgBuf, 0, affineState.affineImgWidth*affineState.affineImgHeight );
        memset( affineState.affineImgBuf+(affineState.affineImgWidth*affineState.affineImgHeight),
                128,
                affineState.affineImgWidth*affineState.affineImgHeight/2);
    }
}



