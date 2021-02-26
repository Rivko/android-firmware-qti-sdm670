/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef GRAPHICS_VIEWPORT_HPP
#define GRAPHICS_VIEWPORT_HPP

#include <memory>

namespace graphics {

struct Viewport {
    float originX;
    float originY;
    float width;
    float height;
    float minDepth;
    float maxDepth;

    Viewport();
};

}
#endif
