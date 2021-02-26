/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "Mesh.h"
#include "Matrix44.h"

#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include "jpgIO.h"

#ifndef WIN32
#include <jni.h>
#include "androidUtility.hpp"
#else
#define LOGI printf
#endif

#define kStringSize 512
#define whitespace " \t\r\n"

char* Realloc (char *p, int oldSize, int newSize);
float Max (float f1, float f2);
int strTokStatus;

/**********************************************************
 * CONSTRUCTOR
 **********************************************************/
Mesh::Mesh ()
{
    numVertices = 0;
    numTexcoords= 0;
    numNormals = 0;
    numFaces = 0;
    vertices = NULL;
    texCoords = NULL;
    m_colors = NULL;
    normals = NULL;
    faces = NULL;
    mVertSize = 0;
    normalArray = NULL;
    totalMeshVerts = 0;

    mCurrentFaceIndex = 0;
    mCurrentFace = NULL;
    mCurrentFaceType = NULL;

    haveCenter = haveBB = 0;
    centerX = centerY = centerZ = 0;
    minX = maxX = 0;
    minY= maxY = 0;
    minZ = maxZ = 0;
    isSorted = 0;

    m_textureWidth = m_textureHeight = 0;
    m_pTextureArray = NULL;
    m_numTextures = 0;
    m_freeData = true;

    m_positionBuffer = 0;
    m_normalBuffer = 0;
    m_uvBuffer  = 0;
    m_facesBuffer = 0;
    memset(m_name, 0, 128);
}


/**********************************************************
 * DESTRUCTOR
 **********************************************************/
Mesh::~Mesh ()
{
    Clear();
}


/**********************************************************
 * Clear
 **********************************************************/
void Mesh::Clear()
{

    LOGI("Mesh::Clear() called");

    if (m_freeData == true)
    {
        /*
         * In some cases, mesh data is statically allocated,
         * no, don't free. (e.g., TextManager)
         */
        if (vertices != NULL)
        {
            free ((unsigned char*)vertices);
        }

        if (texCoords != NULL)
        {
            free ((unsigned char*)texCoords);
        }

        if (m_colors != NULL)
        {
            free ((unsigned char*)m_colors);
        }


        if (normals != NULL)
        {
            free ((unsigned char*)normals);
        }

        if (faces != NULL)
        {
            free ((unsigned char*)faces);
        }

        if (m_faceCount != NULL)
        {
            free ((int*)  m_faceCount);
        }
    }

    if(normalArray != NULL)
    {
        free ((unsigned char*)normalArray);
    }

    numVertices = 0;
    numTexcoords= 0;
    numNormals = 0;
    numFaces = 0;
    vertices = NULL;
    texCoords = NULL;
    normals = NULL;
    faces = NULL;
    mVertSize = 0;
    normalArray = NULL;
    totalMeshVerts = 0;

    mCurrentFaceIndex = 0;
    mCurrentFace = NULL;
    mCurrentFaceType = NULL;

    haveCenter = haveBB = 0;
    centerX = centerY = centerZ = 0;
    minX = maxX = 0;
    minY= maxY = 0;
    minZ = maxZ = 0;
    isSorted = 0;

    m_textureWidth = m_textureHeight = 0;
}

/**********************************************************
 * SetTextureSize
 **********************************************************/
void Mesh::SetTextureSize(int w, int h)
{
    m_textureWidth = w;
    m_textureHeight = h;
}

/********************************************************************
 * READOBJ - read a WaveFront OBJ polygon mesh stored in a file. 
 * Used for debugging.  eScene now does all the real file I/O.
 ********************************************************************/
int Mesh::ReadObj (char *filename)
{
    char buf [kStringSize];
    int numSpaces = 0;
    int* faceptr;
    int currentVertexIndex = 0;
    int currentUVIndex = 0;
    int currentNormalIndex = 0;
    char s[30][kStringSize];
    char junkString[30];
    float x, y, z;

    int currentMaterialIndex = -1;

    FILE * fp = fopen (filename, "rb");
    if (!fp)
    {
        char mbox [512];
        sprintf (mbox, "Error: File %s could not be opened for reading\n", filename);
        return 0;
    }

    m_faceCount = (int*)  malloc (m_numTextures * sizeof (int));
    for (int i = 0; i < m_numTextures; i++)
    {
        m_faceCount[i] = 0;
    }

    numVertices = numTexcoords = numNormals = 0;
    /**************************************
     * FIRST PASS -  Just count data lines. 
     **************************************/
    while (fgets (buf, kStringSize, fp))
    {
        /*
         *  Vertex Position
         */
        if (buf[0] == 'v' && buf[1] == ' ')
        {
            numVertices++;
        }

        /*
         *  Vertex UV
         */
        if (buf[0] == 'v' && buf[1] == 't')
        {
            numTexcoords++;
        }

        /*
         * Vertex Normal
         */
        if (buf[0] == 'v' && buf[1] == 'n')
        {
            numNormals++;
        }

        /*
         * detecting usemtl line
         */
        if (buf[0] == 'u' && buf[1] == 's')
        {
            currentMaterialIndex++;
        }
        /*
         * FACES - count number of faces, and number of face vertices
         */
        if (buf[0] == 'f' && buf[1] == ' ')
        {
            char *ptr = NULL;
            int numFaceVerts;
            char sampleVert[512];
            int haveNum;
            int vertSize;

            /*
             * Increment number of faces
             */
            numFaces++;
            m_faceCount[currentMaterialIndex]++;

            /*
             * Figure out number of face verts and
             * vert size.
             */
            numFaceVerts = 1;
            strtok(buf, whitespace);
            ptr = strtok(NULL,whitespace);
            strcpy (sampleVert, ptr);

            numFaceVerts = 1;
            while (strtok(NULL,whitespace))
            {
                numFaceVerts++;
            }
            totalMeshVerts += numFaceVerts;

            /*
             * Figure out vert size if we haven't already.
             */
            if (mVertSize == 0)
            {
                ptr = sampleVert;
                haveNum = 0;
                vertSize = 0;

                /*
                 * Detect POSITION
                 */
                if (*ptr)
                {
                    vertSize = 1;
                }

                /*
                 * Detect UV
                 */
                while ((*ptr) && (*ptr != '/'))
                {
                    ptr++;
                }

                if (*ptr)
                {
                    ptr++;
                    haveNum = atoi(ptr);
                }

                if (haveNum != 0)
                {
                    vertSize++;
                    numTexcoords = numTexcoords;
                }

                /*
                 * Detect NORMAL
                 */
                haveNum = 0;
                while ((*ptr) && (*ptr != '/'))
                {
                    ptr++;
                }

                if (*ptr)
                {
                    ptr++;
                    haveNum = atoi(ptr);
                }

                if (haveNum != 0)
                {
                    vertSize++;
                    numNormals = numNormals;
                }

                /*
                 * Record result
                 */
                mVertSize = vertSize;
            } // if calculating vert size
        } // face
    } // end of First pass

    /*
     * Allocate memory
     */
    if (numVertices > 0)
    {
        vertices = (float*)  malloc (3 * sizeof (float) * numVertices);
    }
    if (numTexcoords > 0)
    {
        texCoords = (float*) malloc (2 * sizeof (float) * numTexcoords);
    }
    if (numNormals > 0)
    {
        normals = (float*)  malloc (3 * sizeof (float) * numNormals);
    }

    /*
     * Loop through the Mesh list, allocating their faces
     */
    if (totalMeshVerts > 0)
    {
        faces = (int*)  malloc (totalMeshVerts * mVertSize * sizeof (int));
    }

    mCurrentFace = faces;


    /*
     * init the slave meshes to the master 
     */
    numVertices = numVertices;

    vertices = vertices;
    texCoords = texCoords;
    normals = normals;

    fseek (fp, 0, SEEK_SET);

    /**************************************
     * SECOND PASS - Load up the data arrays
     **************************************/
    int vindex = 0;
    int vtindex = 0;
    int vnindex = 0;
    int faceIndex = 0;
    int beenHereBefore = 0;

    faceptr = faces;


    while (fgets (buf, kStringSize, fp))
    {
        /*
         * get x,y,z  for what comes after v, vt, or vn
         */
        if (buf[0] == 'v')
        {
            sscanf (buf, "%s %s %s %s", junkString, s[0], s[1], s[2]);
            x = (float) atof (s[0]);
            y = (float) atof (s[1]);
            z = (float) atof (s[2]);
        }

        /*
         * Its a vertex, set the vertex array
         */
        if (buf[0] == 'v' && buf[1] == ' ')
        {
            vertices[3 * vindex + 0] = x;
            vertices[3 * vindex + 1] = y;
            vertices[3 * vindex + 2] = z;
            vindex++;
        }

        /*
         * Its a UV coordinate , set the texture  array
         */
        if (buf[0] == 'v' && buf[1] == 't')
        {
            texCoords[2 * vtindex + 0] = x;
            texCoords[2 * vtindex + 1] = y;
            vtindex++;
        }
        /*
         * Its a vertex normal, set the normals  array
         */
        if (buf[0] == 'v' && buf[1] == 'n')
        {
            normals[3 * vnindex + 0] = x;
            normals[3 * vnindex + 1] = y;
            normals[3 * vnindex + 2] = z;
            vnindex++;
        }

        /*
         * Read a face in
         */
        if (buf[0] == 'f' && buf[1] == ' ')
        {
            char *ptr = NULL;
            int numFaceVerts = 0;
            int vertVal;

            /*
             * Isolate each vertex description and parse.
             */
            strtok(buf, whitespace);
            ptr = strtok(NULL,whitespace);
            while (ptr != NULL)
            {
                numFaceVerts++;

                /*
                 * Load POSITION
                 */
                if (*ptr)
                {
                    vertVal = atoi(ptr);
                    if (vertVal > 0)
                    {
                        *(mCurrentFace) = vertVal - 1;
                        mCurrentFace++;
                    }
                    else if (vertVal < 0)
                    {
                        *(mCurrentFace) = vindex + vertVal;
                        mCurrentFace++;
                    }
                }

                /*
                 *  Get UV
                 */
                while ((*ptr) && (*ptr != '/'))
                {
                    ptr++;
                }

                if (*ptr)
                {
                    ptr++;
                    vertVal = atoi(ptr);
                    if (vertVal > 0)
                    {
                        *(mCurrentFace) = vertVal - 1;
                        mCurrentFace++;
                    }
                    else if (vertVal < 0)
                    {
                        *(mCurrentFace) = vtindex + vertVal;
                        mCurrentFace++;
                    }
                }


                /*
                 *  Get NORMAL
                 */
                while ((*ptr) && (*ptr != '/'))
                {
                    ptr++;
                }

                if (*ptr)
                {
                    ptr++;
                    vertVal = atoi(ptr);
                    if (vertVal > 0)
                    {
                        *(mCurrentFace) = vertVal - 1;
                        mCurrentFace++;
                    }
                    else if (vertVal < 0)
                    {
                        *(mCurrentFace) = vnindex + vertVal;
                        mCurrentFace++;
                    }
                }

                ptr = strtok(NULL,whitespace);
            } // - while loop parsing

            /*
             *   Record the vertex count
             */
            faceIndex = mCurrentFaceIndex;
            mCurrentFaceIndex++;
        } // F - line parsing
    } // while fgets

    return 1;
}



/**********************************************************
 * GetBoundingSize
 **********************************************************/
void Mesh::GetBoundingSize (float *deltaX, float * deltaY, float * deltaZ)
{
    if (!haveBB)
    {
        FindBoundingBox();
    }

    *deltaX = (maxX - minX);
    *deltaY = (maxY - minY);
    *deltaZ = (maxZ - minZ);
}

/**********************************************************
 * getBoundingBox
 **********************************************************/
void Mesh::GetBoundingBox (float *x1, float *x2, float *y1, float *y2, float* z1, float* z2)
{
    if (!haveBB)
    {
        FindBoundingBox();
    }

    *x1 = minX;
    *x2 = maxX;

    *y1 = minY;
    *y2 = maxY;

    *z1 = minZ;
    *z2 = maxZ;
}

/**********************************************************
 * FindBoundingBox
 **********************************************************/
void Mesh::FindBoundingBox ()
{
    float*   vptr = vertices;

    minX = *vptr;   maxX = minX;
    vptr++;
    minY = *vptr;   maxY = minY;
    vptr++;
    minZ = *vptr;   maxZ = minZ;
    vptr++;

    for (int i=1; i < numVertices; i++)
    {
        //x
        if (*vptr < minX) minX = *vptr;
        if (*vptr > maxX) maxX = *vptr;
        vptr++;
        //y
        if (*vptr < minY) minY = *vptr;
        if (*vptr > maxY) maxY = *vptr;
        vptr++;
        //z
        if (*vptr < minZ) minZ = *vptr;
        if (*vptr > maxZ) maxZ = *vptr;
        vptr++;
    }
    if (numVertices) haveBB = 1;
}

/**********************************************************
 * GetBoundingOrtho - Used for othographic projection setups.
 **********************************************************/
void Mesh::GetBoundingOrtho (float *x1, float *x2, float *y1, float *y2, float* z1, float* z2)
{
    if (!haveBB)
    {
        FindBoundingBox();
    }

    float cx, cy, cz;
    FindCenter (&cx, &cy, &cz);

    *x1 = minX - cx;
    *x2 = maxX - cx;

    *y1 = minY - cy;
    *y2 = maxY - cy;

    *z1 = minZ - cz;
    *z2 = maxZ - cz;
}

/**********************************************************
 * FindCenter
 **********************************************************/
void Mesh::FindCenter (float *cx, float *cy, float *cz)
{
    if (haveCenter)
    {
        *cx = centerX;
        *cy = centerY;
        *cz = centerZ;
        return;
    }

    if (!haveBB) FindBoundingBox();

    centerX = minX + ((maxX - minX) / 2.f);
    centerY = minY + ((maxY - minY) / 2.f);
    centerZ = minZ + ((maxZ - minZ) / 2.f);

    haveCenter = 1;

    *cx = centerX;
    *cy = centerY;
    *cz = centerZ;
}





/**********************************************************
 * PrepDraw - Only send the vertex info, to be used with 
 * FastDraw(), which will skip this step.
 **********************************************************/
void Mesh::PrepDraw()
{
    int value = 0;
    glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &value);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof ( GLfloat ), vertices);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof ( GLfloat ), texCoords);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof ( GLfloat ), normals);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
}

/**********************************************************
 * EndDraw
 **********************************************************/
void Mesh::EndDraw()
{
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}



/**********************************************************
 * FastDraw - not complete
 **********************************************************/
void Mesh::FastDraw(int index)
{
    // orig
    //glDrawElements ( GL_TRIANGLES, totalMeshVerts, GL_UNSIGNED_INT, faces );


    int *pFaces = faces;
    int offset = 0;

    // skip ahead to the faces that use the texture pointed to by index
    for (int i=0; i < index; i++)
    {
        offset += (m_faceCount[i] * 3);
    }
    pFaces += offset;
    int count = m_faceCount[index] * 3;
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, pFaces);

}

/**********************************************************
 * Draw
 **********************************************************/
void Mesh::Draw()
{
    glVertexAttribPointer ( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof ( GLfloat ), vertices);
    glVertexAttribPointer ( 1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof ( GLfloat ), texCoords);
    glVertexAttribPointer ( 2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof ( GLfloat ), normals);

    glEnableVertexAttribArray ( 0 );
    glEnableVertexAttribArray ( 1 );
    glEnableVertexAttribArray ( 2 );

    glDrawElements ( GL_TRIANGLES, totalMeshVerts, GL_UNSIGNED_INT, faces );

    glDisableVertexAttribArray ( 0 );
    glDisableVertexAttribArray ( 1 );
    glDisableVertexAttribArray ( 2 );
}

/**********************************************************
 * GetNumVertices
 **********************************************************/
int  Mesh::GetNumVertices()
{
    return numVertices;
}

/**********************************************************
 * GetVertices
 **********************************************************/
float * Mesh::GetVertices()
{
    return vertices;
}

/**********************************************************
 * GetNormals
 **********************************************************/
float * Mesh::GetNormals()
{
    return normals;
}

/**********************************************************
 * GetTextureCoords
 **********************************************************/
float* Mesh::GetTextureCoords()
{
    return texCoords;
}


/**********************************************************
 * GetNumFaces
 **********************************************************/
int Mesh::GetNumFaces()
{
    return numFaces;
}



/**********************************************************
 * SetTextures
 **********************************************************/
void Mesh::SetTextures(unsigned char ** pTextureArray, int numTextures, int textureWidth, int textureHeight)
{
    m_pTextureArray = pTextureArray;
    m_numTextures = numTextures;

    m_textureWidth = textureWidth;
    m_textureHeight = textureHeight;


}


/**********************************************************
 * GetNumTextures
 **********************************************************/
int Mesh::GetNumTextures()
{
    return m_numTextures;
}

/**********************************************************
 * GetTexture
 **********************************************************/
unsigned char * Mesh::GetTexture(int index)
{
    unsigned char *pResult = NULL;
    if (m_pTextureArray != NULL)
    {
        pResult = m_pTextureArray[index];
    }
    return pResult;
}


/**********************************************************
 * GetTextureSize
 **********************************************************/
void Mesh::GetTextureSize(int *pTexureWidth, int *pTextureHeight)
{
    *pTexureWidth = m_textureWidth;
    *pTextureHeight = m_textureHeight;
}


/**********************************************************
 * Collapse Indices
 **********************************************************/
void Mesh::CollapseIndices()
{
    /*
     * Do sorted face index list.
     */
    isSorted = 1;
    int * pSortedFaces = (int*)malloc(totalMeshVerts * sizeof(int));
    memset (pSortedFaces, 0, totalMeshVerts * sizeof(int));
    int *pDstFace = pSortedFaces;

    int * pFace = faces;
    for (int i=0; i < totalMeshVerts; i++)
    {
        *pDstFace++ = *pFace;
        pFace += 3;
    }

    numNormals = numVertices;
    numTexcoords = numVertices;

    free(faces);

    faces = pSortedFaces;
}

/**********************************************************
 * SetName - needed for WriteBinary to know texture names
 **********************************************************/
void Mesh::SetName(const char *pName)
{
    strcpy(m_name, pName);
}

/**********************************************************
 * WriteBinary
 **********************************************************/
void Mesh::WriteBinary(char *pName)
{
    FILE *fp = fopen(pName, "wb");
    if (fp == NULL)
    {
        return;
    }

    float version = 1;

    fwrite(&version, sizeof(float), 1, fp);
    fwrite(&m_numTextures, sizeof(int), 1, fp);
    char textureName[128];

    for (int i=0; i < m_numTextures; i++)
    {
        sprintf(textureName, "%s_color_%d.jpg", m_name, i);
        fwrite(textureName, 128 * sizeof(char), 1, fp);
    }
    fwrite(&numVertices, sizeof(int), 1, fp);
    fwrite(&numFaces, sizeof(int), 1, fp);
    fwrite(vertices, sizeof(float) * 3, numVertices, fp);
    fwrite(normals, sizeof(float) * 3, numVertices, fp);
    fwrite(texCoords, sizeof(float) * 2, numVertices, fp);
    fwrite(faces, sizeof(int) * 3, numFaces, fp);

    fwrite(m_faceCount, sizeof(int), m_numTextures, fp);

    fclose(fp);
}


/**********************************************************
 * ReadBinary
 **********************************************************/
void Mesh::ReadBinary(char *pName)
{
    FILE *fp = fopen(pName, "rb");
    if (fp == NULL)
    {
        return;
    }

    float version;

    fread(&version, sizeof(float), 1, fp);
    fread(&m_numTextures, sizeof(int), 1, fp);
    char textureName[128];
    for (int i=0; i < m_numTextures; i++)
    {
        fread(textureName, 128 * sizeof(char), 1, fp);
    }

    fread(&numVertices, sizeof(int), 1, fp);
    numNormals = numVertices;
    numTexcoords = numVertices;
    fread(&numFaces, sizeof(int), 1, fp);
    vertices = (float*)malloc(sizeof(float) * 3 * numVertices);
    normals = (float*)malloc(sizeof(float) * 3 * numNormals);
    texCoords = (float*)malloc(sizeof(float) * 2 * numTexcoords);
    faces = (int*)malloc(sizeof(int) * 3 * numFaces);

    fread(vertices, sizeof(float) * 3, numVertices, fp);
    fread(normals, sizeof(float) * 3, numVertices, fp);
    fread(texCoords, sizeof(float) * 2, numVertices, fp);
    fread(faces, sizeof(int) * 3,  numFaces, fp);

    m_faceCount = (int*)  malloc (m_numTextures * sizeof (int));
    fread(m_faceCount, sizeof(int), m_numTextures, fp);

    isSorted = 1;
    totalMeshVerts = numFaces * 3;

    fclose(fp);
}

