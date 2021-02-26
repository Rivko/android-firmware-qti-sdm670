/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <stdlib.h>

#include "GLES3/gl3.h"

#include "Mesh.h"
#include "MeshViewer.h"
#include "jpgIO.h"

#include <sys/time.h>

#ifndef WIN32

#include <jni.h>
#include "androidUtility.hpp"

#endif

static MeshViewer* meshViewer = NULL;
static Mesh* inputMesh = NULL;
static Mesh* binaryMesh = NULL;

unsigned char *pTextureArray[10];


// Forwards
int esMain(int width, int height, const char *pDirectory, const char *pModelName, int numTextures);

/**********************************************************
 * DRAW
 **********************************************************/
void Draw() {
    meshViewer->Draw();
}

/**********************************************************
 * SHUTDOWN
 **********************************************************/
void ShutDown() {

    if (meshViewer) {
        LOGI("deleting meshViewer");
        delete meshViewer;
        meshViewer = NULL;
    }

    if (binaryMesh) {
        LOGI("deleting binaryMesh");
        delete binaryMesh;
        binaryMesh = NULL;
    }

    for (int i = 0; i < 10; i++) {
         unsigned char *rgbData = pTextureArray[i];
         if (rgbData != NULL) {
            free(rgbData);
            pTextureArray[i] = NULL;
            LOGI("freed rgbData from pTextureArray[%d]",i);
         }
    }

    LOGI("ShutDown done");
}

/**********************************************************
 * Our main() function, called by the esUtil main()....
 **********************************************************/
int Init(int width, int height, const char *pDirectory, const char *pModelName, int numTextures) {
    return esMain(width, height, pDirectory, pModelName, numTextures);
}


/**********************************************************
 * MouseButtonCallback
 **********************************************************/
void MouseButtonCallback(int button, int state, int x, int y) {
    meshViewer->MouseButtonCallback(button, state, x, y);
}


/**********************************************************
 *  MousePositionCallback
 **********************************************************/
void MousePositionCallback(int button, int x, int y) {
    meshViewer->MousePositionCallback(button, x, y);
}

void SetViewer(int X, int Y, int width, int height) {
    meshViewer->SetViewerResolution(X,Y,width,height);
}

void SetRotationAndDraw(float rotationX, float rotationY) {
    meshViewer->SetRotationAndDraw(rotationX, rotationY);
}

void LockRotationAngle() {
    meshViewer->LockRotationAngle();
}

/****************************************************************
* RGB to RGBA - Does NOT free the original
****************************************************************/
void RGBtoRGBA(unsigned char *pRGB, int imageWidth, int imageHeight, unsigned char *pRGBA) {
    int size = imageWidth * imageHeight;
    unsigned char *pSrc = pRGB;

    unsigned char *pResult = pRGBA;
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
        *pDst = 0xff;
        pDst++;
    }
}

/**********************************************************
 * SetMeshMode
 **********************************************************/
void SetMeshMode(int meshMode)
{
    meshViewer->ToggleGreyRender();  //SetGreyRender(meshMode);
}

/**********************************************************
 * Our main() function, called by the esUtil main()....
 **********************************************************/
int esMain(int width, int height, const char *pDirectory, const char *pModelName, int numTextures) {

    if (meshViewer) {
        delete meshViewer;
        meshViewer = NULL;
    }
    meshViewer = new MeshViewer;
    if (binaryMesh) {
        delete binaryMesh;
        binaryMesh = NULL;
    }
    binaryMesh = new Mesh;

    // Viewer size
    meshViewer->SetViewerResolution(0, 0, width, height);
    LOGI("WIDTH=%d, HEIGHT=%d\n", width, height);

    char inputFile[512];

    int textureWidth, textureHeight;

    for (int i = 0; i < numTextures; i++) {
        //sprintf(inputFile, "%s/%s_color_%d.jpg", pDirectory, pModelName, i);
        sprintf(inputFile, "%s/output_%d.jpg", pDirectory, i);

        LOGI("Loading texture file %s", inputFile);
        unsigned char *rgbData = jpgLoad(inputFile, &textureWidth, &textureHeight);

#ifdef RGBA_ENABLED
        unsigned char * rgbaData = (unsigned char*) malloc (textureWidth * textureHeight * 4);
        RGBtoRGBA(rgbData,textureWidth,textureHeight,rgbaData);
        free(rgbData);
        pTextureArray[i] = rgbaData;
#endif
#ifndef RGBA_ENABLED
        pTextureArray[i] = rgbData;
#endif

    }

    //first check to see if a binary obj file exists, if not then read the obj and create a bobj
    //sprintf(inputFile, "%s/%s_3DR.bobj", pDirectory, pModelName);
    sprintf(inputFile, "%s/output.bobj", pDirectory);

    if(meshViewer->file_exists_test(inputFile))
    {
        LOGI("BOBJ exists so loading: %s\n", inputFile);
        binaryMesh->ReadBinary(inputFile);
    }
    else {

        LOGI("BOBJ doesn't exist so creating from OBJ  %s\n", inputFile);

        //sprintf(inputFile, "%s/%s_3DR.obj", pDirectory, pModelName);
        sprintf(inputFile, "%s/output.obj", pDirectory);

        LOGI("OBJ FILENAME  %s\n", inputFile);

        inputMesh = new Mesh;
        inputMesh->SetTextures(pTextureArray, numTextures, textureWidth, textureHeight);
        inputMesh->ReadObj(inputFile);
        inputMesh->CollapseIndices();

        inputMesh->SetName(pModelName);
        //sprintf(inputFile, "%s/%s_3DR.bobj", pDirectory, pModelName);
        sprintf(inputFile, "%s/output.bobj", pDirectory);

        LOGI("BINARY FILE PATH: %s", inputFile);
        inputMesh->WriteBinary(inputFile);
        delete  inputMesh; //done with it

        binaryMesh->ReadBinary(inputFile);

        sprintf(inputFile, "%s/thumb.jpg", pDirectory);
        LOGI("THUMB PATH: %s", inputFile);
        meshViewer->CreateThumbnail(inputFile);
    }

    binaryMesh->SetTextures(pTextureArray, numTextures, textureWidth, textureHeight);
    meshViewer->SetMesh(binaryMesh);
    return GL_TRUE;
}
