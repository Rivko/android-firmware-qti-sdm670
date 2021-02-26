/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "graphics/Blending.hpp"

namespace graphics {

Blending::Blending() : alpha(Equation::Add), rgb(Equation::Add),
        sinkAlpha(Factor::Zero), sinkRGB(Factor::OneMinusSourceAlpha),
        sourceAlpha(Factor::One), sourceRGB(Factor::SourceAlpha) {
}

}
