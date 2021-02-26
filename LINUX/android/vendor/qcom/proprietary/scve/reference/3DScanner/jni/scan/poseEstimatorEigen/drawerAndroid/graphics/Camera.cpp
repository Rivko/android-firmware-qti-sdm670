/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "graphics/Camera.hpp"

#include <cassert>

#include <core/Point.hpp>
#include <core/Quaternion.hpp>

namespace graphics {

struct Camera::Impl {
    core::Quaternion mOrientation;
    core::Point mPosition;
};

Camera::Camera() : mImpl(new Impl) {
}

Camera::Camera(const core::Point &position) : mImpl(new Impl) {
    mImpl->mPosition = position;
}

Camera::Camera(const core::Quaternion &orientation) : mImpl(new Impl) {
    mImpl->mOrientation = orientation;
}

Camera::Camera(const core::Point &position,
        const core::Quaternion &orientation) : mImpl(new Impl) {
    mImpl->mPosition = position;
    mImpl->mOrientation = orientation;
}

Camera::~Camera() {
}

const core::Point &Camera::getPosition() const {
    return mImpl->mPosition;
}

const core::Quaternion &Camera::getOrientation() const {
    return mImpl->mOrientation;
}

Camera &Camera::setPosition(const core::Point &position) {
    mImpl->mPosition = position;
    return *this;
}

Camera &Camera::setOrientation(const core::Quaternion &orientation) {
    mImpl->mOrientation = orientation;
    return *this;
}

core::Vector Camera::getAxis(uint32_t dim) const {
    assert(dim < 3);

    core::Vector v;
    v(dim) = 1.0f;

    return getOrientation() * v;
}

core::Matrix Camera::getInverseView() const {
    //C = [r | t] = T(t) R(r)
    core::Matrix R(getOrientation());

    auto T = getPosition();
    R(0, 3) = T(0);
    R(1, 3) = T(1);
    R(2, 3) = T(2);

    //camera_space to world_space
    return R;
}

core::Matrix Camera::getView() const {
    //C = [r | t] = T(t) R(r)
    //C^-1 = R^-1 T^-1 = R^T T(-t) = [R(r)^T | R(r)^T T(-t)]
    core::Matrix inv_R(getOrientation());
    inv_R.transpose();

    core::Point inv_T = inv_R * (-getPosition());
    inv_R(0, 3) = inv_T(0);
    inv_R(1, 3) = inv_T(1);
    inv_R(2, 3) = inv_T(2);

    //world_space to camera_space
    return inv_R;
}

}
