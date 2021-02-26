/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef GRAPHICS_CAMERA_HPP
#define GRAPHICS_CAMERA_HPP

#include <memory>

#include <core/Matrix.hpp>
#include <core/Vector.hpp>

namespace core {

class Point;
class Quaternion;

}

namespace graphics {

class Camera {
public:
    Camera();
    Camera(const core::Point &position);
    Camera(const core::Quaternion &orientation);
    Camera(const core::Point &position, const core::Quaternion &orientation);
    ~Camera();

    const core::Point &getPosition() const;
    const core::Quaternion &getOrientation() const;
    Camera &setPosition(const core::Point &position);
    Camera &setOrientation(const core::Quaternion &orientation);

    core::Vector getAxis(uint32_t dim) const;
    core::Matrix getInverseView() const;
    core::Matrix getView() const;

private:
    Camera(const Camera &) = delete;
    Camera &operator=(const Camera &) = delete;

    struct Impl;
    std::unique_ptr<Impl> mImpl;
};

}
#endif
