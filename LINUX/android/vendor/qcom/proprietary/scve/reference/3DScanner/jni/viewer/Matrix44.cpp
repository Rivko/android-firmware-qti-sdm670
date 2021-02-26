/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string.h>
#include <math.h>
#include "Matrix44.h"

/**********************************************************
 * Constructor
 **********************************************************/
Matrix44::Matrix44()
{
    memset(m_data, 0, sizeof(float) * 4 * 4);
}

/**********************************************************
 * Destructor
 **********************************************************/
Matrix44::~Matrix44()
{
}

/**********************************************************
 * Clear
 **********************************************************/
void Matrix44::Clear()
{
    memset(m_data, 0, sizeof(float) * 4 * 4);
}

/**********************************************************
 * Get
 **********************************************************/
float * Matrix44::Get()
{
    float * pResult = (float*)&m_data[0][0];
    return pResult;
}

/**********************************************************
 * Identity
 **********************************************************/
void Matrix44::Identity()
{
    memset(m_data, 0, sizeof(float) * 4 * 4);

    m_data[0][0] = m_data[1][1] = m_data[2][2] = m_data[3][3] = 1.f;
}

/**********************************************************
 * Scale
 **********************************************************/
void Matrix44::Scale(float x, float y, float z)
{
   m_data[0][0] *= x;
   m_data[0][1] *= x;
   m_data[0][2] *= x;
   m_data[0][3] *= x;

   m_data[1][0] *= y;
   m_data[1][1] *= y;
   m_data[1][2] *= y;
   m_data[1][3] *= y;

   m_data[2][0] *= z;
   m_data[2][1] *= z;
   m_data[2][2] *= z;
   m_data[2][3] *= z;
}


/**********************************************************
 * Translate
 **********************************************************/
void Matrix44::Translate(float x, float y, float z)
{
   m_data[3][0] += (m_data[0][0] * x + m_data[1][0] * y + m_data[2][0] * z);
   m_data[3][1] += (m_data[0][1] * x + m_data[1][1] * y + m_data[2][1] * z);
   m_data[3][2] += (m_data[0][2] * x + m_data[1][2] * y + m_data[2][2] * z);
   m_data[3][3] += (m_data[0][3] * x + m_data[1][3] * y + m_data[2][3] * z);
}

/**********************************************************
 * MultiplyVec4
 **********************************************************/
void Matrix44::MultiplyVec4(float v_data[], float r_data[])
{
    r_data[0] = (m_data[0][0] * v_data[0] + m_data[1][0] * v_data[1] + m_data[2][0] * v_data[2] + m_data[3][0] * v_data[3]);
    r_data[1] = (m_data[0][1] * v_data[0] + m_data[1][1] * v_data[1] + m_data[2][1] * v_data[2] + m_data[3][1] * v_data[3]);
    r_data[2] = (m_data[0][2] * v_data[0] + m_data[1][2] * v_data[1] + m_data[2][2] * v_data[2] + m_data[3][2] * v_data[3]);
    r_data[3] = (m_data[0][3] * v_data[0] + m_data[1][3] * v_data[1] + m_data[2][3] * v_data[2] + m_data[3][3] * v_data[3]);
}


/**********************************************************
 * Perspective
 **********************************************************/
void Matrix44::Perspective(float fovY, float aspect, float nearZ, float farZ )
{
    float width, height;
    float offsetX, offsetY;

    offsetX = 0;
    offsetY = 0;

    height = tanf(fovY / 360.f * (float)M_PI ) * nearZ;
    width = height * aspect;

    Frustum(-width + offsetX, width + offsetX, -height + offsetY, height + offsetY, nearZ, farZ );
}

/**********************************************************
 * Orthographic
 **********************************************************/
void Matrix44::Orthographic(float left, float right, float bottom, float top, float nearZ, float farZ )
{
   float width = right - left;
   float height = top - bottom;
   float depth = farZ - nearZ;


   Identity();

   m_data[0][0] = 2.0f / width;
   m_data[3][0] = -1.f * (right + left)/width;
   m_data[1][1] = 2.0f / height;
   m_data[3][1] = -1.f * (top + bottom)/height;
   m_data[2][2] = -2.0f / depth;
   m_data[3][2] = -1.f * (nearZ + farZ)/depth;
}


/**********************************************************
 * Frustum
 **********************************************************/
void Matrix44::Frustum(float left, float right, float bottom, float top, float nearZ, float farZ )
{
    float width = right - left;
    float height = top - bottom;
    float depth = farZ - nearZ;

    memset(m_data, 0, sizeof(float) * 4 * 4);

    m_data[0][0] = 2.0f * nearZ / width;
    m_data[1][1] = 2.0f * nearZ / height;

    m_data[2][0] = (right + left) / width;
    m_data[2][1] = (top + bottom) / height;
    m_data[2][2] = -1.f * (nearZ + farZ) / depth;
    m_data[2][3] = -1.f;

    m_data[3][2] = -2.f * nearZ * farZ / depth;
}



/**********************************************************
 * Rotate
 **********************************************************/
void Matrix44::Rotate(float degrees, float x, float y, float z )
{
    float length = sqrtf (x*x + y*y + z*z);
    if (length == 0.f)
    {
        return;
    }

    /*
     * normalize
     */
    x /= length;
    y /= length;
    z /= length;

    Matrix44 rotation;

    float theta = degrees * ((float)M_PI / 180.f);
    float cosTheta = cosf(theta);
    float sinTheta = sinf(theta);

    rotation.m_data[0][0] = cosTheta + (x*x * (1 - cosTheta));
    rotation.m_data[0][1] = (x * y * (1-cosTheta)) - (z * sinTheta);
    rotation.m_data[0][2] = (x * z * (1-cosTheta)) + (y * sinTheta);
    rotation.m_data[0][3] = 0.f;

    rotation.m_data[1][0] = (x * y * (1-cosTheta)) + (z * sinTheta);
    rotation.m_data[1][1] = cosTheta + (y * y * (1-cosTheta));
    rotation.m_data[1][2] = (z * y * (1-cosTheta)) - (x * sinTheta);
    rotation.m_data[1][3] = 0.f;

    rotation.m_data[2][0] = (z * x * (1-cosTheta)) - (y * sinTheta);
    rotation.m_data[2][1] = (z * y * (1-cosTheta)) + (x * sinTheta);
    rotation.m_data[2][2] = cosTheta + (z * z * (1-cosTheta));
    rotation.m_data[2][3] = 0.f;

    rotation.m_data[3][0] = 0.f;
    rotation.m_data[3][1] = 0.f;
    rotation.m_data[3][2] = 0.f;
    rotation.m_data[3][3] = 1.f;

    Matrix44 result;
    MatrixMultiply(&result, &rotation, this);

    memcpy(m_data, result.m_data, sizeof(float) * 4 * 4);
}

/**********************************************************
 * Mulitply
 **********************************************************/
void MatrixMultiply(Matrix44 * pResult, Matrix44 * A, Matrix44 *B )
{
    /*
     * First Row
     */
    pResult->m_data[0][0] = 
        (A->m_data[0][0] * B->m_data[0][0]) + 
        (A->m_data[0][1] * B->m_data[1][0]) + 
        (A->m_data[0][2] * B->m_data[2][0]) + 
        (A->m_data[0][3] * B->m_data[3][0]);


    pResult->m_data[0][1] = 
        (A->m_data[0][0] * B->m_data[0][1]) + 
        (A->m_data[0][1] * B->m_data[1][1]) + 
        (A->m_data[0][2] * B->m_data[2][1]) + 
        (A->m_data[0][3] * B->m_data[3][1]);

    pResult->m_data[0][2] = 
        (A->m_data[0][0] * B->m_data[0][2]) + 
        (A->m_data[0][1] * B->m_data[1][2]) + 
        (A->m_data[0][2] * B->m_data[2][2]) + 
        (A->m_data[0][3] * B->m_data[3][2]);

    pResult->m_data[0][3] = 
        (A->m_data[0][0] * B->m_data[0][3]) + 
        (A->m_data[0][1] * B->m_data[1][3]) + 
        (A->m_data[0][2] * B->m_data[2][3]) + 
        (A->m_data[0][3] * B->m_data[3][3]);


    /*
     * Second
     */
    pResult->m_data[1][0] = 
        (A->m_data[1][0] * B->m_data[0][0]) + 
        (A->m_data[1][1] * B->m_data[1][0]) + 
        (A->m_data[1][2] * B->m_data[2][0]) + 
        (A->m_data[1][3] * B->m_data[3][0]);


    pResult->m_data[1][1] = 
        (A->m_data[1][0] * B->m_data[0][1]) + 
        (A->m_data[1][1] * B->m_data[1][1]) + 
        (A->m_data[1][2] * B->m_data[2][1]) + 
        (A->m_data[1][3] * B->m_data[3][1]);

    pResult->m_data[1][2] = 
        (A->m_data[1][0] * B->m_data[0][2]) + 
        (A->m_data[1][1] * B->m_data[1][2]) + 
        (A->m_data[1][2] * B->m_data[2][2]) + 
        (A->m_data[1][3] * B->m_data[3][2]);

    pResult->m_data[1][3] = 
        (A->m_data[1][0] * B->m_data[0][3]) + 
        (A->m_data[1][1] * B->m_data[1][3]) + 
        (A->m_data[1][2] * B->m_data[2][3]) + 
        (A->m_data[1][3] * B->m_data[3][3]);


    /*
     * Third
     */
    pResult->m_data[2][0] = 
        (A->m_data[2][0] * B->m_data[0][0]) + 
        (A->m_data[2][1] * B->m_data[1][0]) + 
        (A->m_data[2][2] * B->m_data[2][0]) + 
        (A->m_data[2][3] * B->m_data[3][0]);


    pResult->m_data[2][1] = 
        (A->m_data[2][0] * B->m_data[0][1]) + 
        (A->m_data[2][1] * B->m_data[1][1]) + 
        (A->m_data[2][2] * B->m_data[2][1]) + 
        (A->m_data[2][3] * B->m_data[3][1]);

    pResult->m_data[2][2] = 
        (A->m_data[2][0] * B->m_data[0][2]) + 
        (A->m_data[2][1] * B->m_data[1][2]) + 
        (A->m_data[2][2] * B->m_data[2][2]) + 
        (A->m_data[2][3] * B->m_data[3][2]);

    pResult->m_data[2][3] = 
        (A->m_data[2][0] * B->m_data[0][3]) + 
        (A->m_data[2][1] * B->m_data[1][3]) + 
        (A->m_data[2][2] * B->m_data[2][3]) + 
        (A->m_data[2][3] * B->m_data[3][3]);


    /*
     * Fourth
     */
    pResult->m_data[3][0] = 
        (A->m_data[3][0] * B->m_data[0][0]) + 
        (A->m_data[3][1] * B->m_data[1][0]) + 
        (A->m_data[3][2] * B->m_data[2][0]) + 
        (A->m_data[3][3] * B->m_data[3][0]);


    pResult->m_data[3][1] = 
        (A->m_data[3][0] * B->m_data[0][1]) + 
        (A->m_data[3][1] * B->m_data[1][1]) + 
        (A->m_data[3][2] * B->m_data[2][1]) + 
        (A->m_data[3][3] * B->m_data[3][1]);

    pResult->m_data[3][2] = 
        (A->m_data[3][0] * B->m_data[0][2]) + 
        (A->m_data[3][1] * B->m_data[1][2]) + 
        (A->m_data[3][2] * B->m_data[2][2]) + 
        (A->m_data[3][3] * B->m_data[3][2]);

    pResult->m_data[3][3] = 
        (A->m_data[3][0] * B->m_data[0][3]) + 
        (A->m_data[3][1] * B->m_data[1][3]) + 
        (A->m_data[3][2] * B->m_data[2][3]) + 
        (A->m_data[3][3] * B->m_data[3][3]);
}



/**********************************************************
 * LookAt
 **********************************************************/
void Matrix44::LookAt ( float fromX,    
                        float fromY,    
                        float fromZ,
                        float toX, 
                        float toY, 
                        float toZ,
                        float upX,     
                        float upY,     
                        float upZ )
{
   float dirX[3], dirY[3], dirZ[3];
   float length;

   /*
    * Direction vector
    */
   dirZ[0] = toX - fromX;
   dirZ[1] = toY - fromY;
   dirZ[2] = toZ - fromZ;

   /*
    * Normalize direction vector
    */
   length = sqrtf ( dirZ[0] * dirZ[0] + dirZ[1] * dirZ[1] + dirZ[2] * dirZ[2] );

   if (length != 0.f)
   {
      dirZ[0] /= length;
      dirZ[1] /= length;
      dirZ[2] /= length;
   }

   /*
    * Cross Up and lookat vector to get direction X 
    */
   dirX[0] = upY * dirZ[2] - upZ * dirZ[1];
   dirX[1] = upZ * dirZ[0] - upX * dirZ[2];
   dirX[2] = upX * dirZ[1] - upY * dirZ[0];

   /*
    * normalize
    */
   length = sqrtf ( dirX[0] * dirX[0] + dirX[1] * dirX[1] + dirX[2] * dirX[2] );

   if ( length != 0.f )
   {
      dirX[0] /= length;
      dirX[1] /= length;
      dirX[2] /= length;
   }

   /*
    * Recalculate Up vector
    */
   dirY[0] = dirZ[1] * dirX[2] - dirZ[2] * dirX[1];
   dirY[1] = dirZ[2] * dirX[0] - dirZ[0] * dirX[2];
   dirY[2] = dirZ[0] * dirX[1] - dirZ[1] * dirX[0];

   /*
    * Normalize
    */
   length = sqrtf(dirY[0] * dirY[0] + dirY[1] * dirY[1] + dirY[2] * dirY[2]);

   if (length != 0.f)
   {
      dirY[0] /= length;
      dirY[1] /= length;
      dirY[2] /= length;
   }


   memset(m_data, 0, sizeof(float) * 4 * 4);

   /*
    * Orientation
    */
   m_data[0][0] = -dirX[0];
   m_data[0][1] =  dirY[0];
   m_data[0][2] = -dirZ[0];

   m_data[1][0] = -dirX[1];
   m_data[1][1] =  dirY[1];
   m_data[1][2] = -dirZ[1];

   m_data[2][0] = -dirX[2];
   m_data[2][1] =  dirY[2];
   m_data[2][2] = -dirZ[2];

   /*
    * Position
    */
   m_data[3][0] =  dirX[0] * fromX + dirX[1] * fromY + dirX[2] * fromZ;
   m_data[3][1] = -dirY[0] * fromX - dirY[1] * fromY - dirY[2] * fromZ;
   m_data[3][2] =  dirZ[0] * fromX + dirZ[1] * fromY + dirZ[2] * fromZ;
   m_data[3][3] = 1.f;
}






