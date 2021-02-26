#ifndef __MESH_VIEWER__
#define __MESH_VIEWER__

/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

//#define RGBA_ENABLED

#include "Mesh.h"
#include "ViewController.h"
#include "Matrix44.h"

class Shader;

class MeshViewer : public ViewController
{
public:
    MeshViewer();
    ~MeshViewer();
    void Clear();
	void SetMesh(Mesh *pMesh);
	void SetViewerResolution(int viewerX, int viewerY, int viewerWidth, int viewerHeight);
	void Draw();
	void SetRotationAndDraw(float rotX, float rotY);
    void glDrawCalls(float cx, float cy, float cz, Matrix44 perspective1);
    void drawThumbnail(Matrix44 &perspective, float left, float right, float top, float bottom, float cx, float cy, float cz);
	void CreateThumbnail(const char *pName);
	bool file_exists_test (const char *name);
    void ToggleGreyRender();
    void LockRotationAngle();

private:

    Mesh * m_pMesh;

    bool InitSimpleDraw();
    bool InitRender(int textureIndex);

    unsigned int m_samplerLoc;
    int m_texWidth, m_texHeight;
    unsigned int modelViewProjLoc;
    unsigned int normalMatrixLoc;
    unsigned int m_doGreyLoc;

    bool m_readyToRender;

    int m_numRenderTextures;
    GLuint m_textureID;

    int m_viewerWidth;
    int m_viewerHeight;
    int m_viewerX;
    int m_viewerY;
    float m_viewerAspect;
    Shader * m_pSimpleShader;

	bool m_error;

    bool m_thumbnailRendered = true; //assume true
    char m_thumb_path[128];

    float m_postAngleX;
    float m_postAngleY;

    bool locked = false;

};
 

#endif
        
