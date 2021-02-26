/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef GRAPHICS_PROJECTION_HPP
#define GRAPHICS_PROJECTION_HPP

#include <memory>

#include <core/Matrix.hpp>

namespace graphics {

class Projection {
public:
    //parameters are in meters
    Projection(float left, float right, float top, float bottom,
        float near, float far);
    Projection(float horizontal_fov_radian,
        float width_over_height, float near, float far);
    ~Projection();

    core::Matrix getInverseInfinite() const;
    core::Matrix getInverseOrthographic() const;
    core::Matrix getInversePerspective() const;

    core::Matrix getInfinite() const;
    core::Matrix getOrthographic() const;
    core::Matrix getPerspective() const;

private:
    Projection() = delete;
    Projection(const Projection &) = delete;
    Projection &operator=(const Projection &) = delete;

    struct Impl;
    std::unique_ptr<Impl> mImpl;
};

}
#endif
