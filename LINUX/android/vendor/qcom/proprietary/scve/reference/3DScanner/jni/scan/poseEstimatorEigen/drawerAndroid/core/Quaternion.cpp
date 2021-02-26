/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "core/Quaternion.hpp"

#include <cassert>
#include <cmath>

#include "core/Matrix.hpp"

namespace core {

Quaternion::Quaternion() : mS(1.0f), mV(0.0f, 0.0f, 0.0f) {
}

Quaternion::Quaternion(float s, float x, float y, float z) : mS(s),
        mV(x, y, z) {
}

Quaternion::Quaternion(const Vector &axis, float angle_radian) {
    float half_angle = 0.5f * angle_radian;
    mS = std::cos(half_angle);
    mV = std::sin(half_angle) * axis;
}

Quaternion::Quaternion(const Quaternion &q) : mS(q.mS), mV(q.mV) {
}

Quaternion::~Quaternion() {
}

Quaternion &Quaternion::operator=(const Quaternion &q) {
    mS = q.mS;
    mV = q.mV;

    return *this;
}

float &Quaternion::operator()(uint32_t idx) {
    assert(idx <= 3);

    if(0 == idx)
        return mS;

    return mV(idx - 1);
}

float Quaternion::operator()(uint32_t idx) const {
    assert(idx <= 3);

    if(0 == idx)
        return mS;

    return mV(idx - 1);
}

Quaternion Quaternion::conjugate() const {
    return Quaternion(mS, -mV(0), -mV(1), -mV(2));
}

float Quaternion::length() const {
    return std::sqrt(lengthSquared());
}

float Quaternion::lengthSquared() const {
    return dot(*this, *this);
}

Quaternion Quaternion::operator*(float v) const {
    return Quaternion(mS * v, mV(0) * v, mV(1) * v, mV(2) * v);
}

Quaternion Quaternion::operator*(const Quaternion &q) const {
    Vector v = mS * q.mV + q.mS * mV + cross(mV, q.mV);
    return Quaternion(mS * q.mS - dot(mV, q.mV), v(0), v(1), v(2));
}

Vector Quaternion::operator*(const Vector &v) const {
    //bring v into pure quaternion space
    //Quaternion vQ(0.0f, v(0), v(1), v(2));
    //L_q(v) = Q * vQ * Q^-1 = Q * vQ * conjugate(Q) with Q being unit
    //Let Q = [q_0, q].  Then L_q(v) =
    //(q_0^2 - dot(q, q)) v + 2 dot(q, v) q + 2 q_0 cross(q, v)
    float a = mS * mS - dot(mV, mV);
    float b = 2 * dot(mV, v);
    float c = 2 * mS;
    return a * v + b * mV + c * cross(mV, v);
}

Quaternion Quaternion::operator+(const Quaternion &q) const {
    Vector v(mV + q.mV);
    return Quaternion(mS + q.mS, v(0), v(1), v(2));
}

Quaternion Quaternion::operator-() const {
    return Quaternion(-mS, -mV(0), -mV(1), -mV(2));
}

Quaternion Quaternion::operator-(const Quaternion &q) const {
    Vector v(mV - q.mV);
    return Quaternion(mS - q.mS, v(0), v(1), v(2));
}

float dot(const Quaternion &a, const Quaternion &b) {
    return a(0) * b(0) + a(1) * b(1) + a(2) * b(2) + a(3) * b(3);
}

Quaternion normalize(const Quaternion &q) {
    float len = q.length();
    assert(0.0f != len);
    float mag_inv = 1.0f / len;
    return Quaternion(q(0) * mag_inv, mag_inv * q(1),
        mag_inv * q(2), mag_inv * q(3));
}

Quaternion operator*(float v, const Quaternion &q) {
    return q * v;
}

Quaternion slerp(const Quaternion &a, const Quaternion &b, float t) {
    float a_dot_b = dot(a, b);

    Quaternion q1;
    Quaternion q2 = b;
    if(a_dot_b < 0.0f) {
        //interpolation will travel the long-way, so negate one of (a,b)
        q1 = -a;
        a_dot_b = -a_dot_b;
    } else
        q1 = a;

    if(fabs(1.0f - a_dot_b) > 0.0001f) {
        //sin(theta) is not 0
        float theta_rad = std::acos(a_dot_b);
        float inv_sin_theta = 1.0f / std::sin(theta_rad);
        return std::sin((1.0f - t) * theta_rad) * inv_sin_theta * q1 +
            std::sin(t * theta_rad) * inv_sin_theta * q2;
    } else {
        //sin(theta) is 0, so lerp and normalize
        return normalize((1.0f - t) * q1 + t * q2);
    }
}

}
