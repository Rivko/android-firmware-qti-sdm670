/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "core/Matrix.hpp"

#include <cassert>
#include <cmath>
#include <cstring>
#include <utility>

#include "core/Quaternion.hpp"

static const int kNumberColumns = 4;
static const int kNumberRows = 4;

namespace core {

Matrix::Matrix() :
        Matrix(1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f) {
}

Matrix::Matrix(float scaling_xyz) :
        Matrix(scaling_xyz, 0.0f, 0.0f, 0.0f,
                0.0f, scaling_xyz, 0.0f, 0.0f,
                0.0f, 0.0f, scaling_xyz, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f) {
}

Matrix::Matrix(float c0x, float c1x, float c2x, float c3x,
        float c0y, float c1y, float c2y, float c3y,
        float c0z, float c1z, float c2z, float c3z,
        float c0w, float c1w, float c2w, float c3w) :
            mValue{c0x, c0y, c0z, c0w,
                    c1x, c1y, c1z, c1w,
                    c2x, c2y, c2z, c2w,
                    c3x, c3y, c3z, c3w} {
}

Matrix::Matrix(const Vector &translation) :
        Matrix(1.0f, 0.0f, 0.0f, translation(0),
                0.0f, 1.0f, 0.0f, translation(1),
                0.0f, 0.0f, 1.0f, translation(2),
                0.0f, 0.0f, 0.0f, 1.0f) {
}

Matrix::Matrix(const Vector &axis, float angle_radian) {
    float co = std::cos(angle_radian);
    float si = std::sin(angle_radian);
    float x = axis(0);
    float y = axis(1);
    float z = axis(2);
    float x2 = x * x;
    float y2 = y * y;
    float z2 = z * z;

    (*this)(0, 0) = x2 + (y2 + z2) * co;
    (*this)(0, 1) = x * y * (1 - co) - z * si;
    (*this)(0, 2) = x * z * (1 - co) + y * si;
    (*this)(0, 3) = 0.0f;

    (*this)(1, 0) = x * y * (1 - co) + z * si;
    (*this)(1, 1) = y2 + (x2 + z2) * co;
    (*this)(1, 2) = y * z * (1 - co) - x * si;
    (*this)(1, 3) = 0.0f;

    (*this)(2, 0) = x * z * (1 - co) - y * si;
    (*this)(2, 1) = y * z * (1 - co) + x * si;
    (*this)(2, 2) = z2 + (x2 + y2) * co;
    (*this)(2, 3) = 0.0f;

    (*this)(3, 0) = 0.0f;
    (*this)(3, 1) = 0.0f;
    (*this)(3, 2) = 0.0f;
    (*this)(3, 3) = 1.0f;
}

Matrix::Matrix(const Quaternion &q) {
    float x2 = q(1) * q(1);
    float y2 = q(2) * q(2);
    float z2 = q(3) * q(3);
    float xy = q(1) * q(2);
    float xz = q(1) * q(3);
    float yz = q(2) * q(3);
    float sx = q(0) * q(1);
    float sy = q(0) * q(2);
    float sz = q(0) * q(3);

    (*this)(0, 0) = 1.0f - 2.0f * (y2 + z2);
    (*this)(0, 1) = 2.0f * (xy - sz);
    (*this)(0, 2) = 2.0f * (xz + sy);
    (*this)(0, 3) = 0.0f;

    (*this)(1, 0) = 2.0f * (xy + sz);
    (*this)(1, 1) = 1.0f - 2.0f * (x2 + z2);
    (*this)(1, 2) = 2.0f * (yz - sx);
    (*this)(1, 3) = 0.0f;

    (*this)(2, 0) = 2.0f * (xz - sy);
    (*this)(2, 1) = 2.0f * (yz + sx);
    (*this)(2, 2) = 1.0f - 2.0f * (x2 + y2);
    (*this)(2, 3) = 0.0f;

    (*this)(3, 0) = 0.0f;
    (*this)(3, 1) = 0.0f;
    (*this)(3, 2) = 0.0f;
    (*this)(3, 3) = 1.0f;
}

Matrix::Matrix(const Matrix &m) {
    memcpy(mValue, m.mValue, sizeof(mValue));
}

Matrix::~Matrix() {
}

const float *Matrix::getData() const {
    return mValue;
}

Matrix &Matrix::operator=(const Matrix &m) {
    memcpy(mValue, m.mValue, sizeof(mValue));
    return *this;
}

float &Matrix::operator()(uint32_t row, uint32_t column) {
    assert(row <= 3 && column <= 3);
    return mValue[column * kNumberRows + row];
}

float Matrix::operator()(uint32_t row, uint32_t column) const {
    assert(row <= 3 && column <= 3);
    return mValue[column * kNumberRows + row];
}

Matrix Matrix::operator*(const Matrix &m) const {
    Matrix result;
    auto &thiz = (*this);
    for(uint32_t i = 0; i < kNumberRows; ++i)
        for(uint32_t j = 0; j < kNumberColumns; ++j)
            result(i, j) = thiz(i, 0) * m(0, j) +
                            thiz(i, 1) * m(1, j) +
                            thiz(i, 2) * m(2, j) +
                            thiz(i, 3) * m(3, j);
    return result;
}

Point Matrix::operator*(const Point &p) const {
    auto &thiz = *this;
    float w = thiz(3, 0) * p(0) + thiz(3, 1) * p(1) +
        thiz(3, 2) * p(2) + thiz(3, 3);
    Point inhomogeneous(
        thiz(0, 0) * p(0) + thiz(0, 1) * p(1) + thiz(0, 2) * p(2) + thiz(0, 3),
        thiz(1, 0) * p(0) + thiz(1, 1) * p(1) + thiz(1, 2) * p(2) + thiz(1, 3),
        thiz(2, 0) * p(0) + thiz(2, 1) * p(1) + thiz(2, 2) * p(2) + thiz(2, 3)
    );
    assert(0.0f != w);
    return inhomogeneous / w;
}

Vector Matrix::operator*(const Vector &v) const {
    auto &thiz = *this;
    Vector ideal_point(
        thiz(0, 0) * v(0) + thiz(0, 1) * v(1) + thiz(0, 2) * v(2),
        thiz(1, 0) * v(0) + thiz(1, 1) * v(1) + thiz(1, 2) * v(2),
        thiz(2, 0) * v(0) + thiz(2, 1) * v(1) + thiz(2, 2) * v(2)
    );
    //this function only supports affine transformations
    assert(0.0f == (thiz(3, 0) * v(0) + thiz(3, 1) * v(1) + thiz(3, 2) * v(2)));
    return ideal_point;
}

Matrix &Matrix::transpose() {
    auto &thiz = *this;
    for(uint32_t i = 0; i < 4; ++i)
        for(uint32_t j = i; j < 4; ++j)
            std::swap(thiz(i, j), thiz(j, i));
    return thiz;
}

}
