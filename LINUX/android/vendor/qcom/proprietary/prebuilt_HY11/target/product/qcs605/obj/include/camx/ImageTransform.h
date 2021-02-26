// NOWHINE ENTIRE FILE 
//-------------------------------------------------------------------------
// Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//------------------------------------------------------------------------


#ifndef IMAGE_TRANSFORM_H_
#define IMAGE_TRANSFORM_H_
#include <stdint.h>
#include <algorithm>
#include <math.h>

struct CPerspectiveTransform
{
    CPerspectiveTransform()
    {
        m[0][0] = 1;
        m[0][1] = 0;
        m[0][2] = 0;
        m[1][0] = 0;
        m[1][1] = 1;
        m[1][2] = 0;
        m[2][0] = 0;
        m[2][1] = 0;
        m[2][2] = 1;
    }

    CPerspectiveTransform(float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22){
        m[0][0] = m00; m[0][1] = m01; m[0][2] = m02;
        m[1][0] = m10; m[1][1] = m11; m[1][2] = m12;
        m[2][0] = m20; m[2][1] = m21; m[2][2] = m22;
    }
	// Note for Bullseye Coverage: The following function can not be reached in the current RANSAC API.
    void Apply(double x, double y, double *resultX, double *resultY) const
    {
        double rX = m[0][0] * x + m[0][1] * y + m[0][2];
        double rY = m[1][0] * x + m[1][1] * y + m[1][2];
        double rW = m[2][0] * x + m[2][1] * y + m[2][2];

        *resultX = rX / rW;
        *resultY = rY / rW;
    }

    // assume the following scale: xNew*scaleX = f(xOld*scaleX); i.e. if new image is downscaled 1:2 the scale is 2.
    CPerspectiveTransform GetScaledTransform(float scaleX, float scaleY) const
    {
        return CPerspectiveTransform(m[0][0], m[0][1] * scaleY / scaleX, m[0][2] / scaleX, m[1][0] * scaleX / scaleY, m[1][1], m[1][2] / scaleY, m[2][0] * scaleX, m[2][1] * scaleY, m[2][2]);
    }
	// Note for Bullseye Coverage: The following function can not be reached in the current RANSAC API.
    // use N if output is downscaled by N
    CPerspectiveTransform GetOutputScaledTransform(float scaleX, float scaleY) const
    {
        return CPerspectiveTransform(m[0][0]*scaleX, m[0][1] * scaleY, m[0][2], m[1][0] * scaleX, m[1][1]*scaleY, m[1][2], m[2][0] * scaleX, m[2][1] * scaleY, m[2][2]);
    }
    CPerspectiveTransform ConvertToTranslationOnly()
    {
        return CPerspectiveTransform(1.0f, 0.0f, m[0][2]/m[2][2], 0.0f, 1.0f, m[1][2]/m[2][2], 0.0f, 0.0f, 1.0f);
    }
	// Note for Bullseye Coverage: The following function can not be reached in the current RANSAC API.
    // cur * t
    CPerspectiveTransform CascadeTransform(const CPerspectiveTransform &t)
    {
        return CPerspectiveTransform(
            m[0][0]*t.m[0][0]+m[0][1]*t.m[1][0]+m[0][2]*t.m[2][0],      m[0][0]*t.m[0][1]+m[0][1]*t.m[1][1]+m[0][2]*t.m[2][1],      m[0][0]*t.m[0][2]+m[0][1]*t.m[1][2]+m[0][2]*t.m[2][2],
            m[1][0]*t.m[0][0]+m[1][1]*t.m[1][0]+m[1][2]*t.m[2][0],      m[1][0]*t.m[0][1]+m[1][1]*t.m[1][1]+m[1][2]*t.m[2][1],      m[1][0]*t.m[0][2]+m[1][1]*t.m[1][2]+m[1][2]*t.m[2][2],
            m[2][0]*t.m[0][0]+m[2][1]*t.m[1][0]+m[2][2]*t.m[2][0],      m[2][0]*t.m[0][1]+m[2][1]*t.m[1][1]+m[2][2]*t.m[2][1],      m[2][0]*t.m[0][2]+m[2][1]*t.m[1][2]+m[2][2]*t.m[2][2]
            );
    }
	// Note for Bullseye Coverage: The following function can not be reached in the current RANSAC API.
    bool Inverse()
    {
        // computes the inverse of a matrix m
        double det = m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2]) -
                        m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
                        m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);

        if (abs(det) < 0.000000001)
        {
            return false;
        }
        double invdet = 1 / det;

        double output[3][3];
        output[0][0] = (m[1][1] * m[2][2] - m[2][1] * m[1][2]) * invdet;
        output[0][1] = (m[0][2] * m[2][1] - m[0][1] * m[2][2]) * invdet;
        output[0][2] = (m[0][1] * m[1][2] - m[0][2] * m[1][1]) * invdet;
        output[1][0] = (m[1][2] * m[2][0] - m[1][0] * m[2][2]) * invdet;
        output[1][1] = (m[0][0] * m[2][2] - m[0][2] * m[2][0]) * invdet;
        output[1][2] = (m[1][0] * m[0][2] - m[0][0] * m[1][2]) * invdet;
        output[2][0] = (m[1][0] * m[2][1] - m[2][0] * m[1][1]) * invdet;
        output[2][1] = (m[2][0] * m[0][1] - m[0][0] * m[2][1]) * invdet;
        output[2][2] = (m[0][0] * m[1][1] - m[1][0] * m[0][1]) * invdet;
        for(int i=0; i<3; i++){
            for(int j=0; j<3; j++){
                m[i][j] = (float)output[i][j];
            }
        }
        return true;
    }
    void AddOffsetToCoordinates(float xOffs, float yOffs)
    {
        m[2][2] += m[2][0]*xOffs+m[2][1]*yOffs;
        m[0][2] += m[0][0]*xOffs+m[0][1]*yOffs - m[2][2]*xOffs;
        m[1][2] += m[1][0]*xOffs+m[1][1]*yOffs - m[2][2]*yOffs;
        m[0][0] -= m[2][0]*xOffs;
        m[0][1] -= m[2][1]*xOffs;
        m[1][0] -= m[2][0]*yOffs;
        m[1][1] -= m[2][1]*yOffs;
    }
    void AddOffsetToCoordinates(float xOffsOut, float yOffsOut, float xOffsIn, float yOffsIn)
    {
        m[2][2] += m[2][0]*xOffsOut+m[2][1]*yOffsOut;
        m[0][2] += m[0][0]*xOffsOut+m[0][1]*yOffsOut - m[2][2]*xOffsIn;
        m[1][2] += m[1][0]*xOffsOut+m[1][1]*yOffsOut - m[2][2]*yOffsIn;
        m[0][0] -= m[2][0]*xOffsIn;
        m[0][1] -= m[2][1]*xOffsIn;
        m[1][0] -= m[2][0]*yOffsIn;
        m[1][1] -= m[2][1]*yOffsIn;
    }
    void Normalize()
    {
        m[0][0] /= m[2][2];
        m[0][1] /= m[2][2];
        m[0][2] /= m[2][2];
        m[1][0] /= m[2][2];
        m[1][1] /= m[2][2];
        m[1][2] /= m[2][2];
        m[2][0] /= m[2][2];
        m[2][1] /= m[2][2];
        m[2][2] = 1;
    }
    
	// Note for Bullseye Coverage: The following function can not be reached in the current RANSAC API.
    inline float Coeff(unsigned char row, unsigned char col) const
    {
        return m[row][col];
    }

    float m[3][3];
};

#endif
