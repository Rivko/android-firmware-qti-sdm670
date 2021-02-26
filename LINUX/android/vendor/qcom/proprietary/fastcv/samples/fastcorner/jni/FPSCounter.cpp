/*
 * Copyright (c) 2010-2011 Qualcomm Technologies Incorporated.
 * All Rights Reserved
 * Qualcomm Technologies Confidential and Proprietary
 *
 *
 * @file Implementation of FPS Counter class
 *
 */

#include <time.h>

#include "FPSCounter.h"

//#define DEBUG 1
#define FPSCOUNTER_LOG_TAG    "QCCommon:FPSCounter"

#ifdef DEBUG
#include <android/log.h>
#define DPRINTF(...)  __android_log_print(ANDROID_LOG_DEBUG,FPSCOUNTER_LOG_TAG,__VA_ARGS__)
#else
#define DPRINTF(...)   //noop
#endif
#define IPRINTF(...)  __android_log_print(ANDROID_LOG_INFO,FPSCOUNTER_LOG_TAG,__VA_ARGS__)
#define EPRINTF(...)  __android_log_print(ANDROID_LOG_ERROR,FPSCOUNTER_LOG_TAG,__VA_ARGS__)
#define WPRINTF(...)  __android_log_print(ANDROID_LOG_WARN,FPSCOUNTER_LOG_TAG,__VA_ARGS__)

//------------------------------------------------------------------------------
/// @brief Default constructor
//------------------------------------------------------------------------------
FPSCounter::FPSCounter()
{
   DPRINTF("FPSCounter::FPSCounter");
   Reset();
}


//------------------------------------------------------------------------------
/// @brief to be called every frame to update the fps count
//------------------------------------------------------------------------------
void FPSCounter::FrameTick()
{
   DPRINTF("FPSCounter::FrameTick");

   struct timeval tv;
   struct timezone tz;
   uint64_t curTime;

   gettimeofday(&tv, &tz);
   curTime = tv.tv_sec * SECOND + tv.tv_usec;

   if( curTime - mLastTime > 10*SECOND )
   {
      Reset();
   }
   else
   {
      //Increament frame count
      mFrameCount++;
      // Calculate seconds elapsed since start.
      float sec = (float)( curTime - mStartTime ) / (float)SECOND ;

      if( sec > 0.0 )
      {
         mFPS = (float)( mFrameCount ) / sec;

         sec = (float)( curTime - mLastTime ) / (float)SECOND;
         if( sec > 0.0 )
         {
            if( mSecIIR > 0.0 )
            {
               //TODO: Define constants for filter coefficient
               mSecIIR = ( sec + ( 29.0 * mSecIIR ) ) / (float)30.0;
            }
            else
            {
               mSecIIR = sec;
            }
         }
      }

      mLastTime = curTime;
      mFilteredFPS = mSecIIR>0.0?1/mSecIIR:-1.0;
   }
}

//------------------------------------------------------------------------------
/// @brief resets the timer and frames
//------------------------------------------------------------------------------
void FPSCounter::Reset()
{
   struct timeval tv;
   struct timezone tz;

   gettimeofday(&tv, &tz);
   mStartTime   = tv.tv_sec * SECOND + tv.tv_usec;

   mLastTime      = mStartTime;

   mFrameCount    = 0;

   mSecIIR        = 0.;

   mFPS           = 0.;

   mFilteredFPS   = 0.;

}


//------------------------------------------------------------------------------
/// @brief Default destructor
//------------------------------------------------------------------------------
FPSCounter::~FPSCounter()
{
   DPRINTF("FPSCounter::~FPSCounter");
}

