/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "graphics/Projection.hpp"

#include <cmath>

namespace graphics {

struct Projection::Impl {
    float mLeft;
    float mRight;
    float mTop;
    float mBottom;
    float mNear;
    float mFar;
};

Projection::Projection(float left, float right, float top, float bottom,
        float near, float far) : mImpl(new Impl) {
    mImpl->mLeft = left;
    mImpl->mRight = right;
    mImpl->mTop = top;
    mImpl->mBottom = bottom;
    mImpl->mNear = near;
    mImpl->mFar = far;
}

Projection::Projection(float horizontal_fov_radian, float width_over_height,
        float near, float far) : mImpl(new Impl) {
    float angle_rad = horizontal_fov_radian;
    float width = 2.0f * near * std::tan(angle_rad / 2);
    float height = width / width_over_height;

    mImpl->mLeft = -width / 2;
    mImpl->mRight = width / 2;
    mImpl->mTop = height / 2;
    mImpl->mBottom = -height / 2;
    mImpl->mNear = near;
    mImpl->mFar = far;
}

Projection::~Projection() {
}

core::Matrix Projection::getInverseInfinite() const {
    float width = mImpl->mRight - mImpl->mLeft;
    float r_l = mImpl->mRight + mImpl->mLeft;

    float height = mImpl->mTop - mImpl->mBottom;
    float t_b = mImpl->mTop + mImpl->mBottom;

    float inv_2n = 1.0f / (2.0f * mImpl->mNear);

    return core::Matrix(width * inv_2n, 0.0f, 0.0f, r_l * inv_2n,
                0.0f, height * inv_2n, 0.0f, t_b * inv_2n,
                0.0f, 0.0f, 0.0f, -1.0f,
                0.0f, 0.0f, -inv_2n, inv_2n);
}

core::Matrix Projection::getInverseOrthographic() const {
    float width = mImpl->mRight - mImpl->mLeft;
    float a = (mImpl->mRight + mImpl->mLeft) * 0.5f;

    float height = mImpl->mTop - mImpl->mBottom;
    float b = (mImpl->mTop + mImpl->mBottom) * 0.5f;

    float depth = mImpl->mFar - mImpl->mNear;
    float c = -(mImpl->mFar + mImpl->mNear) * 0.5f;

    return core::Matrix(width * 0.5f, 0.0f, 0.0f, a,
                0.0f, height * 0.5f, 0.0f, b,
                0.0f, 0.0f, -depth * 0.5f, c,
                0.0f, 0.0f, 0.0f, 1.0f);
}

core::Matrix Projection::getInversePerspective() const {
    float width = mImpl->mRight - mImpl->mLeft;
    float inv_2n = 1.0f / (2.0f * mImpl->mNear);
    float a = (mImpl->mRight + mImpl->mLeft) * inv_2n;

    float height = mImpl->mTop - mImpl->mBottom;
    float b = (mImpl->mTop + mImpl->mBottom) * inv_2n;

    float depth = mImpl->mFar - mImpl->mNear;
    float inv_2nf = inv_2n / mImpl->mFar;
    float c = -depth * inv_2nf;
    float d = (mImpl->mFar + mImpl->mNear) * inv_2nf;

    return core::Matrix(width * inv_2n, 0.0f, 0.0f, a,
                0.0f, height * inv_2n, 0.0f, b,
                0.0f, 0.0f, 0.0f, -1.0f,
                0.0f, 0.0f, c, d);
}

core::Matrix Projection::getInfinite() const {
    float width = mImpl->mRight - mImpl->mLeft;
    float a = (mImpl->mRight + mImpl->mLeft) / width;

    float height = mImpl->mTop - mImpl->mBottom;
    float b = (mImpl->mTop + mImpl->mBottom) / height;

    return core::Matrix(2.0f * mImpl->mNear / width, 0.0f, a, 0.0f,
                0.0f, 2.0f * mImpl->mNear / height, b, 0.0f,
                0.0f, 0.0f, -1.0f, -2.0f * mImpl->mNear,
                0.0f, 0.0f, -1.0f, 0.0f);
}

core::Matrix Projection::getOrthographic() const {
    float width = mImpl->mRight - mImpl->mLeft;
    float a = -(mImpl->mRight + mImpl->mLeft) / width;

    float height = mImpl->mTop - mImpl->mBottom;
    float b = -(mImpl->mTop + mImpl->mBottom) / height;

    float depth = mImpl->mFar - mImpl->mNear;
    float c = -(mImpl->mFar + mImpl->mNear) / depth;

    return core::Matrix(2.0f / width, 0.0f, 0.0f, a,
                0.0f, 2.0f / height, 0.0f, b,
                0.0f, 0.0f, -2.0f / depth, c,
                0.0f, 0.0f, 0.0f, 1.0f);
}

core::Matrix Projection::getPerspective() const {
    float width = mImpl->mRight - mImpl->mLeft;
    float a = (mImpl->mRight + mImpl->mLeft) / width;

    float height = mImpl->mTop - mImpl->mBottom;
    float b = (mImpl->mTop + mImpl->mBottom) / height;

    float depth = mImpl->mFar - mImpl->mNear;
    float c = -(mImpl->mFar + mImpl->mNear) / depth;
    float d = -(2.0f * mImpl->mFar * mImpl->mNear) / depth;

    return core::Matrix(2.0f * mImpl->mNear / width, 0.0f, a, 0.0f,
                0.0f, 2.0f * mImpl->mNear / height, b, 0.0f,
                0.0f, 0.0f, c, d,
                0.0f, 0.0f, -1.0f, 0.0f);
}

}
