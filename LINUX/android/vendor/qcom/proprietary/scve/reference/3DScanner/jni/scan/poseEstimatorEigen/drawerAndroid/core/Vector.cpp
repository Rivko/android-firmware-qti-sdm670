/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "core/Vector.hpp"

#include <cassert>
#include <cmath>

namespace core {

Vector::Vector() : mValue{0.0f, 0.0f, 0.0f} {
}

Vector::Vector(float v) : mValue{v, v, v} {
}

Vector::Vector(float x, float y, float z) : mValue{x, y, z} {
}

Vector::Vector(const Vector &v) : mValue{v(0), v(1), v(2)} {
}

Vector::~Vector() {
}

Vector &Vector::operator=(const Vector &v) {
    auto &thiz = *this;
    thiz(0) = v(0);
    thiz(1) = v(1);
    thiz(2) = v(2);
    return thiz;
}

float Vector::length() const {
    return std::sqrt(lengthSquared());
}

float Vector::lengthSquared() const {
    return dot(*this, *this);
}

float &Vector::operator()(uint32_t idx) {
    assert(idx <= 2);
    return mValue[idx];
}

float Vector::operator()(uint32_t idx) const {
    assert(idx <= 2);
    return mValue[idx];
}

Vector Vector::operator*(float f) const {
    auto &thiz = *this;
    return Vector(thiz(0) * f, thiz(1) * f, thiz(2) * f);
}

Vector Vector::operator+(const Vector &v) const {
    auto &thiz = *this;
    return Vector(thiz(0) + v(0), thiz(1) + v(1), thiz(2) + v(2));
}

Vector Vector::operator-() const {
    auto &thiz = *this;
    return Vector(-thiz(0), -thiz(1), -thiz(2));
}

Vector Vector::operator-(const Vector &v) const {
    auto &thiz = *this;
    return Vector(thiz(0) - v(0), thiz(1) - v(1), thiz(2) - v(2));
}

Vector Vector::operator/(float f) const {
    assert(0.0f != f);
    float inv = 1.0f / f;
    return (*this) * inv;
}

Vector cross(const Vector &a, const Vector &b) {
    return Vector(
        a(1) * b(2) - a(2) * b(1),
        a(2) * b(0) - a(0) * b(2),
        a(0) * b(1) - a(1) * b(0)
    );
}

float dot(const Vector &a, const Vector &b) {
    return a(0) * b(0) + a(1) * b(1) + a(2) * b(2);
}

Vector normalize(const Vector &v) {
    return v / v.length();
}

Vector operator*(float f, const Vector &v) {
    return v * f;
}

Vector operator/(float f, const Vector &v) {
    return Vector(f / v(0), f / v(1), f / v(2));
}

}
