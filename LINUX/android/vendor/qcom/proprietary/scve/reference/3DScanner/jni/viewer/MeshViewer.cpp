/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdlib.h>
#include <cstring>
#include <GLES3/gl3.h>
#include "MeshViewer.h"
#include "Mesh.h"
#include "Shader.h"
#include "Matrix44.h"
#include <sys/time.h>

#ifndef WIN32

#include <jni.h>
#include "androidUtility.hpp"
#include "jpgIO.h"

#else
#define LOGI printf
#include <mmsystem.h>
#endif


/**********************************************************
 * Constructor
 **********************************************************/
MeshViewer::MeshViewer() {
    LOGI("MeshViewer Constructor called");
    m_pSimpleShader = NULL;
    Clear();
}

/**********************************************************
 * Destructor
 **********************************************************/
MeshViewer::~MeshViewer() {
    LOGI("MeshViewer Destructor called");
    if (m_numRenderTextures > 0) {
        /*
         * This IF statement must appear BEFORE the call to clear().
         */
         LOGI("calling glDeleteTextures");
         glDeleteTextures(1, &m_textureID);
         LOGI("after calling glDeleteTextures");
    }

    Clear();
}

/**********************************************************
 * SetMesh
 **********************************************************/
void MeshViewer::SetMesh(Mesh *pMesh) {
    m_pMesh = pMesh;
}

void MeshViewer::CreateThumbnail(const char *pName)
{
    m_thumbnailRendered = false;
    strcpy(m_thumb_path, pName);
}

/**********************************************************
 * Clear
 **********************************************************/
void MeshViewer::Clear() {
    m_samplerLoc = 0;
    m_texWidth = 0;
    m_texHeight = 0;
    modelViewProjLoc = 0;
    normalMatrixLoc = 0;
    m_doGreyLoc = 0;

    m_readyToRender = false;

    /*
     * 3D Viewer variables
     */
    for (int i = 0; i < 10; i++) {
        m_mouseX[0] = 0;
        m_mouseY[0] = 0;
    }

    m_mouseDown = false;
    m_rightMouseDown = false;

    m_lastMouse = false;
    m_startX = m_startY = 0;
    m_startRotateX = 0.;
    m_startRotateY = 0.;
    m_currentAngleX = 0.f;
    m_currentAngleY = 0.f;

    m_postAngleX = 0.f;
    m_postAngleY = 0.f;

    m_currentZoom = 1.f;
    m_rightMouseDown = false;
    m_lastRightMouseDown = false;
    m_startZoom = 1.f;
    m_startZoomX = 0;
    m_startZoomY = 0;

    m_middleMouseDown = false;
    m_lastMiddleMouseDown = false;
    m_currentTrackHoriz = 0.f;
    m_currentTrackVert = 0.f;
    m_startTrackHoriz = 0.f;
    m_startTrackVert = 0.f;
    m_startTrackX = 0;
    m_startTrackY = 0;

    m_greyToggle = 0;
    m_numRenderTextures = 0;
    m_textureID = 0;

    m_viewerAspect = (float) m_viewerWidth / (float) m_viewerHeight;

    if (m_pSimpleShader != NULL) {
        delete m_pSimpleShader;
    }

    m_error = false;
    m_thumbnailRendered = true;
}

bool MeshViewer::file_exists_test (const char *name) {
    if (FILE *file = fopen(name, "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }
}

void RGBAtoRGB(GLubyte *pRGBA, int imageWidth, int imageHeight, unsigned char *pRGB) {
    int size = imageWidth * imageHeight;
    unsigned char *pSrc = pRGBA;
    unsigned char *pResult = pRGB;
    unsigned char *pDst = pResult;

    for (int i = 0; i < size; i++) {
        // red
        *pDst = *pSrc;
        pDst++;
        pSrc++;

        // green
        *pDst = *pSrc;
        pDst++;
        pSrc++;

        // blue
        *pDst = *pSrc;
        pDst++;
        pSrc++;

        // alpha
        pSrc++;
    }
}

Matrix44 calculateOrthoPerspective(Matrix44 &perspective, float left, float right, float top,
                                   float bottom, float zoom) {
    float fudgeHoriz = -1.f * ((right - left) - (zoom * (right - left)));
    float fudgeVert = -1.f * (((top - bottom) - (zoom * (top - bottom))));
    perspective.Orthographic(left - fudgeHoriz, right + fudgeHoriz, bottom - fudgeVert,
                             top + fudgeVert, 0.01f, 100.f);
    return perspective;
}

float calculateThumbSquareSize(Matrix44 &perspective, float right, float top, float viewHeight) {
    float vecPos4[4] = {right, top, 0, 1};
    float outVecPos4[4];

    perspective.MultiplyVec4(vecPos4, outVecPos4);
    float screenY = (((outVecPos4[1] + 1) / 2) * viewHeight);
    return 2 * screenY - viewHeight;

}

void MeshViewer::drawThumbnail(Matrix44 &perspective, float left, float right, float top,
                               float bottom, float cx, float cy, float cz) {

    float thumb_square_size = calculateThumbSquareSize(perspective, right, top, m_viewerHeight);
    float thumbnail_zoom = 1.0f; //start at normal zoom level then start moving away

    Matrix44 zoomOutPerspective;

    //Scale the perspective out until we reach a thumbnail height that is around 500 pixels
    while (thumb_square_size > 500) {
        thumbnail_zoom += 0.1f;
        calculateOrthoPerspective(zoomOutPerspective, left, right, top, bottom, thumbnail_zoom);
        thumb_square_size = calculateThumbSquareSize(zoomOutPerspective, right, top,
                                                     m_viewerHeight);
    }

    glDrawCalls(cx, cy, cz, zoomOutPerspective);

    thumb_square_size *= 1.1f; //enlarge it a little bit
    float squareX = (m_viewerWidth - thumb_square_size) / 2;
    float squareY = (m_viewerHeight - thumb_square_size) / 2;

    GLubyte *image = (GLubyte *) malloc(
            thumb_square_size * thumb_square_size * sizeof(GLubyte) * 4);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(squareX, squareY, thumb_square_size, thumb_square_size, GL_RGBA, GL_UNSIGNED_BYTE,
                 image);
    unsigned char *rgbData = (unsigned char *) malloc(thumb_square_size * thumb_square_size * 3);
    RGBAtoRGB(image, thumb_square_size, thumb_square_size, rgbData);
    jpgWrite(m_thumb_path, rgbData, thumb_square_size, thumb_square_size, true);
    free(image);
    free(rgbData);

}

void MeshViewer::LockRotationAngle() {

    if (!locked) {
        m_postAngleY += m_currentAngleY;
        m_postAngleX += m_currentAngleX;
        locked = true;
    }
    else {
        locked = false;
        m_postAngleY = 0.f;
        m_postAngleX = 0.f;
    }

    m_currentAngleY = 0.f;
    m_currentAngleX = 0.f;

    //LOGI("AFTER LOCK post angle x/y = %f, %f", m_postAngleX, m_postAngleY);
}

void MeshViewer::glDrawCalls(float cx, float cy, float cz, Matrix44 perspective1) {

    Matrix44 modelview1;
    modelview1.Identity();

    modelview1.Translate(cx + m_currentTrackHoriz, cy + m_currentTrackVert, cz);

    modelview1.Rotate(m_currentAngleY, 1, 0, 0);
    modelview1.Rotate(m_currentAngleX, 0, 1, 0);
    modelview1.Rotate(m_postAngleY, 1, 0, 0);
    modelview1.Rotate(m_postAngleX, 0, 1, 0);

    modelview1.Translate(-cx, -cy, -cz);

    Matrix44 modelViewProjection1;
    MatrixMultiply(&modelViewProjection1, &modelview1, &perspective1);

    glUniformMatrix4fv(modelViewProjLoc, 1, GL_FALSE, modelViewProjection1.Get());

    Matrix44 rotation1;
    rotation1.Identity();
    rotation1.Rotate(m_currentAngleY, 1, 0, 0);
    rotation1.Rotate(m_currentAngleX, 0, 1, 0);
    rotation1.Rotate(m_postAngleY, 1, 0, 0);
    rotation1.Rotate(m_postAngleX, 0, 1, 0);
    glUniformMatrix4fv(normalMatrixLoc, 1, GL_FALSE, rotation1.Get());

    if (m_error == true)
        glClearColor(1., 0., 0., 1.);
    else
        glClearColor(.2f, .2f, .2f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    m_pMesh->PrepDraw();

    for (int i = 0; i < m_numRenderTextures; i++) {
        bool success = InitRender(i);
        if (success == true) {
            m_pMesh->FastDraw(i);
        }
    }

    m_pMesh->EndDraw();
}

/**********************************************************
 * DRAW - called by Windows for refreshing the window
 **********************************************************/
void MeshViewer::Draw() {
    if (m_pMesh->GetNumVertices() == 0) {
        return;
    }

    if (m_readyToRender == false) {
        bool result = InitSimpleDraw();
        if (result == false) {
            m_error = true;
            return;
        }
        m_readyToRender = true;
    }

    m_pSimpleShader->UseProgram();

    /*
     * Set the camera
     */

    float cx, cy, cz;
    m_pMesh->FindCenter(&cx, &cy, &cz);

    float orthoLeft, orthoRight, orthoBottom, orthoTop, orthoNear, orthoFar;
    m_pMesh->GetBoundingBox(&orthoLeft, &orthoRight, &orthoBottom, &orthoTop, &orthoFar,
                            &orthoNear);

    float boxX, boxY, boxZ;
    m_pMesh->GetBoundingSize(&boxX, &boxY, &boxZ);
    float maxBox = boxX;
    if (boxY > maxBox)
        maxBox = boxY;
    if (boxZ > maxBox)
        maxBox = boxZ;

    float worldHeight = maxBox;
    float worldWidth = maxBox * m_viewerAspect;
    orthoLeft = -(worldWidth / 2) + cx;
    orthoRight = (worldWidth / 2) + cx;
    orthoBottom = -(worldHeight / 2) + cy;
    orthoTop = (worldHeight / 2) + cy;

    Matrix44 perspective1;
    calculateOrthoPerspective(perspective1, orthoLeft, orthoRight, orthoTop, orthoBottom,
                              m_currentZoom);

    if (!m_thumbnailRendered) {
        drawThumbnail(perspective1, orthoLeft, orthoRight, orthoTop, orthoBottom, cx, cy, cz);
        m_thumbnailRendered = true;
    }

#if 0
    perspective.Perspective(m_currentZoom, m_viewerAspect, .1f, 1000.0f );
    perspective.m_data[2][2] /= 1000.f;
    perspective.m_data[3][2] /= 1000.f;
#endif

    glDrawCalls(cx, cy, cz, perspective1);


#ifdef WIN32
    ViewerUpdatePC();
#else
    ViewerUpdateTablet();
#endif
}

void MeshViewer::SetRotationAndDraw(float rotationX, float rotationY) {

    m_currentAngleX += rotationX;
    //m_currentAngleY = rotationY; //don't change Y value

    Draw();
}

extern char simple_vertex[];
extern char simple_pixel[];

/**********************************************************
 * InitSimpleDraw - Set up simple shaders for textured model rendering.
 **********************************************************/
bool MeshViewer::InitSimpleDraw() {
    if (m_pSimpleShader == NULL) {
        m_pSimpleShader = new Shader;
        //m_pSimpleShader->SetFiles("simple_vertex.glsl", "simple_pixel.glsl");
        m_pSimpleShader->SetSource(simple_vertex, simple_pixel); // from memory
    }
    m_pSimpleShader->UseProgram();

    m_samplerLoc = m_pSimpleShader->GetLoc("s_texture");
    modelViewProjLoc = m_pSimpleShader->GetLoc("modelViewProjection");
    normalMatrixLoc = m_pSimpleShader->GetLoc("normalMatrix");
    m_doGreyLoc = m_pSimpleShader->GetLoc("doGrey");

    m_numRenderTextures = m_pMesh->GetNumTextures();
    glGenTextures(1, &m_textureID);

    /*
     * Prepare a texture
     */
    int errorCode = 0;
    glActiveTexture(GL_TEXTURE0);
    errorCode = glGetError();
    if (errorCode != GL_NO_ERROR) {
        LOGI("InitSimpleDraw() ERROR: Activating Texture for 3D viewer");
        return false;
    }

    glBindTexture(GL_TEXTURE_2D, m_textureID);
    errorCode = glGetError();
    if (errorCode != GL_NO_ERROR) {
        LOGI("ERROR: Binding Texture for 3D viewer");
        return false;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    unsigned char *pTexturePixels = m_pMesh->GetTexture(0);
    if (pTexturePixels == NULL) {
        LOGI("ERROR: Texture is NULL");
        return false;
    }

    m_pMesh->GetTextureSize(&m_texWidth, &m_texHeight);

    /*
     * WARNING: Be careful here. The texture mapper produces RGBA, but if just reading an RGB image
     * from disk for a browser, you may prefer RGB, depending on the file type (eg. RGB JPEG)
     */
#ifdef RGBA_ENABLED
    glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGBA, m_texWidth, m_texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pTexturePixels);
#endif
#ifndef RGBA_ENABLED
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_texWidth, m_texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE,
                 pTexturePixels);
#endif

    errorCode = glGetError();
    if (errorCode != GL_NO_ERROR) {
        LOGI("ERROR: Making texture for 3D viewer");
        return false;
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return true;
}


/**********************************************************
 * InitRender
 **********************************************************/
bool MeshViewer::InitRender(int textureIndex) {

    int errorCode = 0;
    /*
   glActiveTexture(GL_TEXTURE0);
   errorCode = glGetError();
   if (errorCode != GL_NO_ERROR)
   {
       LOGI("InitRender() ERROR: Activating Texture for 3D viewer");
       return false;
   }

   glBindTexture ( GL_TEXTURE_2D, m_textureID);
   errorCode = glGetError();
   if (errorCode != GL_NO_ERROR)
   {
       LOGI("ERROR: Binding Texture for 3D viewer");
       return false;
   }
*/
    unsigned char *pTexturePixels = m_pMesh->GetTexture(textureIndex);
    if (pTexturePixels == NULL) {
        LOGI("ERROR: Texture is NULL");
        return false;
    }

    /*
     * WARNING: Be careful here. The texture mapper produces RGBA, but if just reading an RGB image
     * from disk for a browser like in this standalone demo case , you may prefer RGB, depending on
     * the file type (eg. RGB JPEG)
     *
     * NOTE: Very strange.  This slows down the standalone 3D viewer quite drastically. Something is fishy.
     * Doesn't happen in the main 3DR app. But the 3DR app uses the RGBA settings, not RGB, maybe that's it?
     */

#ifdef RGBA_ENABLED
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_texWidth, m_texHeight, GL_RGBA, GL_UNSIGNED_BYTE, pTexturePixels);
#endif
#ifndef RGBA_ENABLED
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_texWidth, m_texHeight, GL_RGB, GL_UNSIGNED_BYTE,
                    pTexturePixels);
#endif


    errorCode = glGetError();
    if (errorCode != GL_NO_ERROR) {
        LOGI("Warning: Could not update texture for 3D viewer");
        return false;
    }

    glUniform1i(m_samplerLoc, 0);
    glUniform1i(m_doGreyLoc, m_greyToggle);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    glViewport(0, 0, m_viewerWidth, m_viewerHeight);
    //glClearColor(.2f, .2f, .2f, 1.0f );

    return true;
}


/**********************************************************
 * ToggleGreyRender
 **********************************************************/
void MeshViewer::ToggleGreyRender() {
    m_greyToggle = 1 - m_greyToggle;
}

/**********************************************************
 * SetViewerResolution
 **********************************************************/
void MeshViewer::SetViewerResolution(int viewerX, int viewerY, int viewerWidth, int viewerHeight) {
    m_viewerX = viewerX;
    m_viewerY = viewerY;
    m_viewerWidth = viewerWidth;
    m_viewerHeight = viewerHeight;
    m_viewerAspect = (float) viewerWidth / (float) viewerHeight;
}
 
