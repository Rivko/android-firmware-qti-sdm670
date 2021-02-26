/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef CORE_POINT_HPP
#define CORE_POINT_HPP

#include "core/Vector.hpp"

#include <cstdint>

namespace core {

class Point {
public:
    Point();
    Point(float x, float y, float z);
    Point(const Point &p);
    ~Point();
    Point &operator=(const Point &p);

    float &operator()(uint32_t idx);
    float operator()(uint32_t idx) const;

    Point operator-() const;
    Point operator*(float f) const;
    Point operator/(float f) const;
    Point operator+(const Vector &v) const;
    Vector operator-(const Point &p) const;
    Point operator-(const Vector &v) const;

private:
    float mValue[3];
};

Point max(const Point &lhs, const Point &rhs);
Point min(const Point &lhs, const Point &rhs);
Point operator*(float f, const Point &p);
Point operator+(const Vector &v, const Point &p);

}

#endif
