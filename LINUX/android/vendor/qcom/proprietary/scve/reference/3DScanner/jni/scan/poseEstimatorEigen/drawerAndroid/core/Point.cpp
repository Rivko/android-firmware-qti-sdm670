/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "core/Point.hpp"

#include <algorithm>
#include <cassert>

namespace core {

Point::Point() : mValue{0.0f, 0.0f, 0.0f} {
}

Point::Point(float x, float y, float z) : mValue{x, y, z} {
}

Point::Point(const Point &p) : mValue{p(0), p(1), p(2)} {
}

Point::~Point() {
}

Point &Point::operator=(const Point &p) {
    auto &thiz = *this;
    thiz(0) = p(0);
    thiz(1) = p(1);
    thiz(2) = p(2);
    return thiz;
}

float &Point::operator()(uint32_t idx) {
    assert(idx <= 2);
    return mValue[idx];
}

float Point::operator()(uint32_t idx) const {
    assert(idx <= 2);
    return mValue[idx];
}

Point Point::operator-() const {
    auto &thiz = *this;
    return Point(-thiz(0), -thiz(1), -thiz(2));
}

Point Point::operator*(float f) const {
    auto &thiz = *this;
    return Point(thiz(0) * f, thiz(1) * f, thiz(2) * f);
}

Point Point::operator/(float f) const {
    auto &thiz = *this;
    return Point(thiz(0) / f, thiz(1) / f, thiz(2) / f);
}

Point Point::operator+(const Vector &v) const {
    auto &thiz = *this;
    return Point(thiz(0) + v(0), thiz(1) + v(1), thiz(2) + v(2));
}

Vector Point::operator-(const Point &p) const {
    auto &thiz = *this;
    return Vector(thiz(0) - p(0), thiz(1) - p(1), thiz(2) - p(2));
}

Point Point::operator-(const Vector &v) const {
    auto &thiz = *this;
    return Point(thiz(0) - v(0), thiz(1) - v(1), thiz(2) - v(2));
}

Point max(const Point &lhs, const Point &rhs) {
    return Point(std::max<float>(lhs(0), rhs(0)),
                 std::max<float>(lhs(1), rhs(1)),
                 std::max<float>(lhs(2), rhs(2)));
}

Point min(const Point &lhs, const Point &rhs) {
    return Point(std::min<float>(lhs(0), rhs(0)),
                 std::min<float>(lhs(1), rhs(1)),
                 std::min<float>(lhs(2), rhs(2)));
}

Point operator*(float f, const Point &p) {
    return p * f;
}

Point operator+(const Vector &v, const Point &p) {
    return p + v;
}

}
