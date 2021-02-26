/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __SHADER_H__
#define __SHADER_H__

#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>


class Shader
{
public:
    Shader();
    ~Shader();

    unsigned int Get();
    void         SetDirectory(char *pDirectory);
    bool         SetFiles(char *pVertexShaderFileName, char *pPixelShaderFileName);
    bool         SetSource(char *pVertexShaderSource, char *pPixelShaderSource);
    unsigned int GetLoc(char *pName);
    void         UseProgram();

private:
    char *       FileReadRaw (char* fileName);
    void         MakeProgram(char *pVertexSource, char *pPixelSource);
    unsigned int Compile(GLenum type, char *pSource);

    unsigned int m_programObject;	
    char         m_directory[256];
    char         m_vertexFileName[256];
    char         m_pixelFileName[256];
};

#endif
