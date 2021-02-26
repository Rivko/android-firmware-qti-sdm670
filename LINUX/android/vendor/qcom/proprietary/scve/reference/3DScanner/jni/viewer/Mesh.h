/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __MESH_H__
#define __MESH_H__

#include <stdio.h>


/**********************************************************
 * MESH 
 **********************************************************/
class Mesh
{
public:
   Mesh();
   ~Mesh();
   void Clear();

   int    ReadObj (char *fname);

   void   SetName(const char *pName);

   int    GetNumVertices();
   float* GetVertices();
   float* GetNormals();
   float* GetTextureCoords();
   int    GetNumFaces();

   void   GetBoundingOrtho (float *x1, float *x2, float *y1, float *y2, float* z1, float* z2);
   void   GetBoundingSize (float *deltaX, float * deltaY, float * deltaZ);
   void   GetBoundingBox (float *x1, float *x2, float *y1, float *y2, float* z1, float* z2);
   void   FindBoundingBox ();
   void   FindCenter (float *cx, float *cy, float *cz);

   void   PrepDraw();
   void   EndDraw();

   void   FastDraw(int uvSetIndex);
   void   Draw();

   int    GetNumTextures();
   unsigned char * GetTexture(int index);
   void   SetTextureSize(int w, int h);
   void   SetTextures(unsigned char ** pTextureArray, int numTextures, int textureWidth, int textureHeight);
   void   GetTextureSize(int *pTexureWidth, int *pTextureHeight);
   void   CollapseIndices();
   void   WriteBinary(char *pName);
   void   ReadBinary(char *pName);

private:
   int     numVertices;
   float*  vertices;
   float*  m_colors;

   int     numTexcoords;
   float*  texCoords;
   int*    faces;
   int     numFaces;

   int     m_textureWidth, m_textureHeight;


   int    mCurrentFaceIndex;
   int*   mCurrentFace;
   int*   mCurrentFaceType;

   int     isSorted;
   int     numNormals;
   float*  normals, **normalArray;

   int     totalMeshVerts;
   int     mVertSize;

   // bounding box stuff
   int     haveCenter, haveBB;
   float   centerX, centerY, centerZ;
   float   minX, maxX;
   float   minY, maxY;
   float   minZ, maxZ;

   bool m_freeData;

   unsigned int m_positionBuffer, m_normalBuffer, m_uvBuffer, m_facesBuffer;

   unsigned char ** m_pTextureArray;
   int m_numTextures;
   int* m_faceCount;

   char m_name[128];
};


#endif
 
