/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef __MATRIX44_H__
#define __MATRIX44_H__


class Matrix44
{
public:
    float m_data[4][4];

    Matrix44();
    ~Matrix44();

    float * Get();
	void Clear();
    void Scale(float x, float y, float z);
    void Identity();
    void Translate (float x, float y,float z );
    void Perspective(float fovY, float aspect, float nearZ, float farZ );
    void Orthographic(float left, float right, float bottom, float top, float nearZ, float farZ );
    void Frustum(float left, float right, float bottom, float top, float nearZ, float farZ );
    void Rotate(float degrees, float x, float y, float z );
    void LookAt(float fromX,    
                float fromY,    
                float fromZ,
                float toX, 
                float toY, 
                float toZ,
                float upX,     
                float upY,     
                float upZ );

    void MultiplyVec4(float v_data[], float r_data[]);
};

void MatrixMultiply(Matrix44 * pResult, Matrix44 * A, Matrix44 *B );


#endif 
