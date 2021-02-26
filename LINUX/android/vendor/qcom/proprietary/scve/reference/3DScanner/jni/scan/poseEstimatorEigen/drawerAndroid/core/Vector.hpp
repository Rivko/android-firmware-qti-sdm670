/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef CORE_VECTOR_HPP
#define CORE_VECTOR_HPP

#include <cstdint>

namespace core {

class Vector {
public:
    Vector();
    Vector(float v);
    Vector(float x, float y, float z);
    Vector(const Vector &v);
    ~Vector();
    Vector &operator=(const Vector &v);

    float length() const;
    float lengthSquared() const;

    float &operator()(uint32_t idx);
    float operator()(uint32_t idx) const;

    Vector operator*(float f) const;
    Vector operator-() const;
    Vector operator+(const Vector &v) const;
    Vector operator-(const Vector &v) const;
    Vector operator/(float f) const;

private:
    float mValue[3];
};

Vector cross(const Vector &a, const Vector &b);
float dot(const Vector &a, const Vector &b);
Vector normalize(const Vector &v);
Vector operator*(float f, const Vector &v);
Vector operator/(float f, const Vector &v);

}
#endif
