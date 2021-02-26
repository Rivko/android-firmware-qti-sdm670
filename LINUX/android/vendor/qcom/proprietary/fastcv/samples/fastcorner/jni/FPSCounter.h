/*
 * Copyright (c) 2010-2011 Qualcomm Technologies Incorporated.
 * All Rights Reserved
 * Qualcomm Technologies Confidential and Proprietary
 *
 *
 * @file Defines FPSCounter class. This class maintains the fps
 *               being maintained by the application
 *
 */


#ifndef __QCCommon_FPSCOUNTER_HEADERFILE__
#define __QCCommon_FPSCOUNTER_HEADERFILE__

#include <stdint.h>
/** @brief FPSCounter class Keeps track  Performance metrics of the
*      application
*/
class FPSCounter
{
protected:


   /**
    * 1 second defined in terms of micro second.
    */
   static const uint64_t   SECOND = 1000000;

   /**
    * Current FPS
    */
   float                   mFPS;

   /**
    * Filtered FPS
    */
   float                   mFilteredFPS;

   /**
    * Time stamp at start
    */
   uint64_t                mStartTime;

   /**
    * Last time stamp
    */
   uint64_t                mLastTime;

   /**
    * Number of frames elapsed since the start
    */
   uint64_t                mFrameCount;

   /**
    * Filtered average time between measurements.
    */
   float                   mSecIIR;

public:

   /**
    * @brief  Default Constructor
    */
   FPSCounter();

   /**
    * @brief  Should be called every frame
    */
   void FrameTick();
   /**
    * @brief Resets the counters
    */
   void Reset();

    /**
    * @brief Returns the running fps
    * @return fps
    */
   inline float GetCurrentFPS()
   {
      return mFPS;
   }

    /**
    * @brief Returns the filtered fps
    * @return fps
    */
   inline float GetFilteredFPS()
   {
      return mFilteredFPS;
   }

   /**
    * @brief Default destructor
    *
    */
   ~FPSCounter();

};

#endif //#ifndef __QCCommon_FPSCOUNTER_HEADERFILE__
