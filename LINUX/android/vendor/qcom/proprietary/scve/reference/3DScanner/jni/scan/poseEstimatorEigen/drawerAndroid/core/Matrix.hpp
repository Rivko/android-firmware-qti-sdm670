/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef CORE_MATRIX_HPP
#define CORE_MATRIX_HPP

#include <cstdint>

#include "core/Point.hpp"
#include "core/Vector.hpp"

namespace core {

class Quaternion;

class Matrix {
public:
    Matrix();
    Matrix(float scaling_xyz);
    Matrix(float c0x, float c1x, float c2x, float c3x,
        float c0y, float c1y, float c2y, float c3y,
        float c0z, float c1z, float c2z, float c3z,
        float c0w, float c1w, float c2w, float c3w);
    Matrix(const Vector &translation);
    Matrix(const Vector &axis, float angle_radian);
    Matrix(const Quaternion &q);
    Matrix(const Matrix &m);
    ~Matrix();
    Matrix &operator=(const Matrix &m);

    const float *getData() const;

    float &operator()(uint32_t row, uint32_t column);
    float operator()(uint32_t row, uint32_t column) const;

    Matrix operator*(const Matrix &m) const;
    //this function will normalize all points to homogeneous coordinates
    Point operator*(const Point &p) const;
    //this function only supports affine transformations
    Vector operator*(const Vector &v) const;

    Matrix &transpose();

private:
    float mValue[16];
};

}
#endif
