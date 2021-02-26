/*
 * Copyright (c) 2010-2011 Qualcomm Technologies Incorporated.
 * All Rights Reserved
 * Qualcomm Technologies Confidential and Proprietary
 *
 *
 * @file Implementation for  Camera Util class
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <android/log.h>

#include "CameraUtil.h"
#define EPRINTF(...)  __android_log_print(ANDROID_LOG_ERROR,"CameraUtil",__VA_ARGS__)

//---------------------------------------------------------------------------
/// @brief Check for opengl error and prints the error message if
///        an error was found.
/// @param op String specifying the OpenGL operation performed
///           before this check.
///
/// @return true if an error was found. false otherwise.
///
//---------------------------------------------------------------------------
bool CameraUtil::checkGlError(const char* op) {
   bool retVal = false;
    if(GLint error = glGetError())
    {
        EPRINTF("after %s() glError (0x%x)", op, error);
        retVal = true;
    }
    return retVal;
}

//---------------------------------------------------------------------------
// @brief clip the value
// @ return clipped value between 0 & 255
//---------------------------------------------------------------------------


inline int32_t imgutils_clip(int32_t x)
{
   return (x<0)?0:((x>255)?255:x);
}

//---------------------------------------------------------------------------
/// @brief Compiles the given fragment and vertex shader source
///        code and linked into an OpenGL shader program object
///        stored in mProgramObject
///
/// @param vShaderStr Vertex Shader Source code string.
/// @param fShaderStr Fragment Shader Source code string.
///
/// @return returns 0 if cannot compile and link, non-zero if
///         successfull.
///
//---------------------------------------------------------------------------
GLuint CameraUtil::CompileAndLinkShaderProgram
(
   const char* vShaderStr,
   const char* fShaderStr
)
{
   GLuint  vertexShader;
   GLuint  fragmentShader;
   GLuint  programObject;

   // Load & compile shaders
   if(!LoadShader( GL_VERTEX_SHADER,   vShaderStr, vertexShader))
   {
      EPRINTF("Unable to load vertex shader");
      return false;
   }

   if(!LoadShader( GL_FRAGMENT_SHADER, fShaderStr, fragmentShader ))
   {
      EPRINTF("Unable to load fragment shader");
      return false;
   }

   // Create program object
   programObject = glCreateProgram();
   if ( !programObject )
   {
      EPRINTF("Unable to Create Program Object");
      return programObject;
   }
   glAttachShader( programObject, vertexShader   );
   glAttachShader( programObject, fragmentShader );

   //Once attached we can delete the vertex and fragment shaders.
   glDeleteShader(vertexShader);
   glDeleteShader(fragmentShader);

   // Link program & check status
   GLint   linked;
   glLinkProgram( programObject );
   glGetProgramiv( programObject, GL_LINK_STATUS, &(linked) );
   if ( !linked )
   {
      GLint infoLen = 0;
      glGetProgramiv( programObject, GL_INFO_LOG_LENGTH, &infoLen );
      if ( infoLen > 1 )
      {
         char infoLog[infoLen];
         glGetProgramInfoLog( programObject, infoLen, NULL, infoLog );
         EPRINTF( "Error linking program: %s", infoLog );
      }
      glDeleteProgram( programObject );
      return 0;
   }
   return programObject;
}


//---------------------------------------------------------------------------
/// @brief Creates shader object by loading shader source, and
///        compiling shader
///
/// @param type shader type
/// @param shaderSrc shader source.
/// @param shader i/o parameter - reference to a shader object.
///
/// @return returns true if successfull false otherwise.
///
//---------------------------------------------------------------------------
bool CameraUtil::LoadShader
(
   GLenum type,
   const char *shaderSrc,
   GLuint& shader
)
{
   GLint  compiled;

   // Create shader object
   shader = glCreateShader( type );
   if ( !shader ) return false;

   // Load shader source
   glShaderSource( shader, 1, &shaderSrc, NULL );

   // Compile shader, and report status if error
   glCompileShader( shader );
   glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );
   if ( !compiled )
   {
      GLint infoLen = 0;

      glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &infoLen );
      if ( infoLen > 1 )
      {
         char infoLog[infoLen];
         glGetShaderInfoLog( shader, infoLen, NULL, infoLog );
         EPRINTF( "Error compiling shader: %s", infoLog );
      }
      glDeleteShader( shader );
      return false;
   }
   return true;
}

