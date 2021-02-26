/*
 * Copyright (c) 2010-2011 Qualcomm Technologies Incorporated.
 * All Rights Reserved
 * Qualcomm Technologies Confidential and Proprietary
 *
 *
 * @file Defines Camera Util class
 *
 */


#ifndef __CAMERA_UTIL_HEADERFILE__
#define __CAMERA_UTIL_HEADERFILE__

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <stdint.h>
#include <time.h>

#include "FPSCounter.h"

/**
 * @brief CameraUtil class provide utility functions useful for
 *        camera image capture and rendering.
 *
 */
class CameraUtil
{

   #define ALIGN_TO(value, alignment) (((value) + ((alignment)-1)) & (~((alignment)-1)))

public:

   /**
    * @brief Gives current time in micro seconds.
    *
    * @return returns current time in micro seconds.
    *
    */
   static inline uint64_t GetCurrentTimeus()
   {
      struct timeval tv;
      struct timezone tz;

      gettimeofday(&tv, &tz);

      return tv.tv_sec * 1000000 + tv.tv_usec;
   }

   /**
    * @brief Compiles the given fragment and vertex shader source
    *        code and linked into an OpenGL shader program object
    *        stored in mProgramObject
    *
    * @param vShaderStr Vertex Shader Source code string.
    * @param fShaderStr Fragment Shader Source code string.
    *
    * @return returns 0 if cannot compile and link, non-zero if
    *         successfull.
    *
    */
   static GLuint CompileAndLinkShaderProgram
   (
      const char* vShaderStr,
      const char* fShaderStr
   );

   /**
    * @brief Creates shader object by loading shader source, and
    *        compiling shader
    *
    * @param type shader type
    * @param shaderSrc shader source.
    * @param shader i/o parameter - reference to a shader object.
    *
    * @return returns true if successfull false otherwise.
    *
   */
   static bool LoadShader( GLenum type, const char *shaderSrc, GLuint& shader );


   /**
    * @brief Check for opengl error and prints the error message if
    *        an error was found.
    * @param op String specifying the OpenGL operation performed
    *           before this check.
    *
    * @return true if an error was found. false otherwise.
    *
    * */
   static bool checkGlError(const char* op);

}; // class CameraUtil
#endif //#ifndef __CAMERA_UTIL_HEADERFILE__
