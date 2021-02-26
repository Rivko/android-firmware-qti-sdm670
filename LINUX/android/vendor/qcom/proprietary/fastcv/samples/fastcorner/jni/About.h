#ifndef ABOUT_H
#define ABOUT_H

/*
 * Copyright (c) 2014 Qualcomm Technologies Incorporated.
 * All Rights Reserved
 * Qualcomm Technologies Confidential and Proprietary
 *
 *
 * @file Definition of About Java Native Interface (JNI).
 *
 */

//==============================================================================
// Include Files
//==============================================================================
#include <jni.h>

//==============================================================================
// Declarations
//==============================================================================
extern "C" {

   //-----------------------------------------------------------------------------
   /// @brief fcvGetVersion
   ///
   /// @return FastCV version
   //-----------------------------------------------------------------------------
   JNIEXPORT jstring JNICALL Java_com_qualcomm_fastcorner_About_getFastCVVersion
   (
      JNIEnv* env,
      jobject obj
   );

};

#endif // ABOUT_H
