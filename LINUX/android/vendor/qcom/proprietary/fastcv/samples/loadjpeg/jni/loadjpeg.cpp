/*
 * Copyright (c) 2010-2011 Qualcomm Technologies Incorporated.
 * All Rights Reserved
 * Qualcomm Technologies Confidential and Proprietary
 *
 *
 * @file Implementation of FastCV Sample native interface.
 *
 */

//==============================================================================
// Include Files
//==============================================================================
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <stdlib.h>
#include <android/log.h>
#include <time.h>
#include <fastcv/fastcv.h>

#include "loadjpeg.h"

//==============================================================================
// Declarations
//==============================================================================
#define LOG_TAG    "loadjpeg.cpp"
#define DPRINTF(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define IPRINTF(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define EPRINTF(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)


//==============================================================================
// Function Declarations
//==============================================================================


//==============================================================================
// Function Definitions
//==============================================================================
void convertRGB565ToY(  uint8_t*       src,
                        unsigned int   srcWidth,
                        unsigned int   srcHeight,
                        uint8_t*       dst );

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
JNIEXPORT void
   JNICALL Java_com_qualcomm_loadjpeg_LoadJpeg_convertToLum
(
   JNIEnv*     env,
   jobject     obj,
   jshortArray img,
   jint        w,
   jint        h
)
{
   jshort*           jimgData = NULL;
   jboolean          isCopy = 0;

   DPRINTF( "%s %d\n", __FILE__, __LINE__ );

   if( img != NULL )
   {
      // Get data from JNI
      jimgData = env->GetShortArrayElements( img, &isCopy );

      uint8_t* lum = (uint8_t*)fcvMemAlloc( w*h, 16 );
      convertRGB565ToY( (uint8_t*)jimgData, w, h, lum );

      fcvMemFree( lum );

      // Let JNI know we don't need data anymore. this is important!
      env->ReleaseShortArrayElements( img, jimgData, JNI_ABORT );

      DPRINTF( "%s %d Done\n", __FILE__, __LINE__ );
   }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void convertRGB565ToY(  uint8_t*       src,
                        unsigned int   srcWidth,
                        unsigned int   srcHeight,
                        uint8_t*       dst )
{
   unsigned int   size = srcWidth * srcHeight;
   uint16_t       rgb;
   uint16_t       r;
   uint16_t       g;
   uint16_t       b;

   for( unsigned int i=0; i<size; i++ ){
      rgb = ((uint16_t*)src)[i];
      r = (uint16_t)((rgb&0xF800) >> 8);
      g = (uint16_t)((rgb&0x07E0) >> 3);
      b = (uint16_t)((rgb&0x001F) << 3);

      dst[i] = (uint8_t)(((r<<1) + (g<<2) + g + b)>>3);
   }
}
