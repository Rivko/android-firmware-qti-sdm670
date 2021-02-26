/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "graphics/Viewport.hpp"

namespace graphics {

Viewport::Viewport() : originX(0.0f), originY(0.0f), width(0.0f), height(0.0f),
            minDepth(0.0f), maxDepth(1.0f) {
}

}
