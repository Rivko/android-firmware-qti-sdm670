/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include "Shader.h"

/**********************************************************
 * Constructor
 **********************************************************/
Shader::Shader()
{
    m_programObject = 0;	
    memset(m_directory, 0, 256);
    memset(m_vertexFileName, 0, 256);
    memset(m_pixelFileName, 0, 256);
}

/**********************************************************
 * Destructor
 **********************************************************/
Shader::~Shader()
{
    glDeleteProgram(m_programObject);
}

/**********************************************************
 * Get
 **********************************************************/
unsigned int Shader::Get()
{
    return m_programObject;
}

/**********************************************************
 * UseProgram
 **********************************************************/
void Shader::UseProgram()
{
    glUseProgram( m_programObject);
}

/**********************************************************
 * SetDirectory
 **********************************************************/
void Shader::SetDirectory(char *pDirectory)
{
    if (pDirectory != NULL)
    {
        strcpy(m_directory, pDirectory);
    }
}

/**********************************************************
 * SetFiles
 **********************************************************/
bool Shader::SetFiles(char *pVertexShaderFileName, char *pPixelShaderFileName)
{
    if(pVertexShaderFileName != NULL)
    {
        strcpy(m_vertexFileName, pVertexShaderFileName);
    }

    if(pPixelShaderFileName != NULL)
    {
        strcpy(m_pixelFileName, pPixelShaderFileName);
    }


    /*
     * Read in vertex shader file
     */
    char * pVertexShaderSource = FileReadRaw(m_vertexFileName);
    if (pVertexShaderSource == NULL)
    {
        printf("Vertex Shader File Not Found\n");
        return false;
    }


    /*
     * Read in pixel shader file
     */
    char * pPixelShaderSource = FileReadRaw(m_pixelFileName);
    if (pPixelShaderSource == NULL)
    {
        printf("Pixel Shader File Not Found\n");
        return false;
    }


    MakeProgram(pVertexShaderSource, pPixelShaderSource);

    free(pVertexShaderSource);
    free(pPixelShaderSource);
    
    return true;
}


/**********************************************************
 * SetSource
 **********************************************************/
bool Shader::SetSource(char *pVertexShaderSource, char *pPixelShaderSource)
{
    MakeProgram(pVertexShaderSource, pPixelShaderSource);
    if (m_programObject != 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}



/**********************************************************
 * CompileShader
 **********************************************************/
unsigned int Shader::Compile(GLenum type, char *pSource)
{
   unsigned int shader = glCreateShader(type);

   if (shader == 0)
   {
      return 0;
   }

   glShaderSource( shader, 1, &pSource, NULL);
   glCompileShader(shader);

   /*
    * Check status
    */
   int success;
   glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

   if (success == 0)
   {
      int length = 0;

      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

      if (length > 1)
      {
         char * pMessage = (char*) malloc(length);

         glGetShaderInfoLog(shader, length, NULL, pMessage);
         printf("Compile Error : %s\n", pMessage);
         free(pMessage);
      }

      glDeleteShader(shader);
      return 0;
   }

   return shader;
}

/**********************************************************
 * MakeProgram
 **********************************************************/
void Shader::MakeProgram(char *pVertexSource, char *pPixelSource)
{
   /*
    * Compile shaders
    */
   unsigned int vertexShader = Compile(GL_VERTEX_SHADER, pVertexSource);

   if (vertexShader == 0)
   {
      return;
   }

   unsigned int pixelShader = Compile(GL_FRAGMENT_SHADER, pPixelSource);

   if (pixelShader == 0)
   {
      glDeleteShader(vertexShader);
      return;
   }

   /*
    * Make program object
    */
   m_programObject = glCreateProgram();
   if (m_programObject == 0)
   {
      return;
   }

   glAttachShader(m_programObject, vertexShader);
   glAttachShader(m_programObject, pixelShader);

   glLinkProgram(m_programObject);


   /*
    * Check status
    */
   int success;
   glGetProgramiv(m_programObject, GL_LINK_STATUS, &success);
   if (success == 0)
   {
      GLint length = 0;

      glGetProgramiv(m_programObject, GL_INFO_LOG_LENGTH, &length);

      if (length > 1)
      {
         char *pMessage = (char*) malloc(length);

         glGetProgramInfoLog(m_programObject, length, NULL, pMessage);
         printf("Error linking program:\n%s\n", pMessage);
         free(pMessage);
      }

      glDeleteProgram(m_programObject);
      return;
   }

   glDeleteShader(vertexShader);
   glDeleteShader(pixelShader);
}

/**********************************************************
 * GetLoc
 **********************************************************/
unsigned int Shader::GetLoc(char *pName)
{
    unsigned int result  = glGetUniformLocation(m_programObject, pName);
	int ss = glGetError();
    return result;
}



/**********************************************************
 * fileReadRaw
 **********************************************************/
char * Shader::FileReadRaw (char* fileName)
{
	int numBytes;
	
    struct stat buf;
    int result;
    
    result = stat(fileName,  &buf);
    if(result == -1)
    {
        return NULL;
    }
    else
    {
        numBytes = buf.st_size;
    }

    /*
     * Open and read file
     */
    FILE *fp = fopen (fileName, "r");
    if (fp != NULL)
    {
        char * buf = (char*) malloc (numBytes+1);
        memset (buf, 0, numBytes+1);
        int bytes = fread (buf, 1, numBytes, fp);
        fclose (fp);
        return (buf);
    }
    else
    {

        return NULL;
    }
}
