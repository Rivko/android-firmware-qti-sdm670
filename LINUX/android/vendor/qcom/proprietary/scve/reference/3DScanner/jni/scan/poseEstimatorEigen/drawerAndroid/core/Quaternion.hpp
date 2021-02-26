/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef CORE_QUATERNION_HPP
#define CORE_QUATERNION_HPP

#include "core/Vector.hpp"

namespace core {

class Matrix;

class Quaternion {
public:
    Quaternion();
    Quaternion(float s, float x, float y, float z);
    Quaternion(const Vector &axis, float angle_radian);
    Quaternion(const Quaternion &q);
    ~Quaternion();
    Quaternion &operator=(const Quaternion &q);

    float &operator()(uint32_t idx);
    float operator()(uint32_t idx) const;

    Quaternion conjugate() const;
    float length() const;
    float lengthSquared() const;

    Quaternion operator*(float v) const;
    //Suppose a and b are unit quaternions representing two rotations.
    //The goal is to perform a first then b.
    //This function will apply b to the result of a i.e. b * a
    //a = q, b = this
    Quaternion operator*(const Quaternion &q) const;
    //Rotate the unit vector v in space
    Vector operator*(const Vector &v) const;
    Quaternion operator+(const Quaternion &q) const;
    Quaternion operator-() const;
    Quaternion operator-(const Quaternion &q) const;

private:
    float mS;
    Vector mV;
};

float dot(const Quaternion &a, const Quaternion &b);
Quaternion normalize(const Quaternion &q);
Quaternion operator*(float v, const Quaternion &q);
Quaternion slerp(const Quaternion &a, const Quaternion &b, float t);

}
#endif
