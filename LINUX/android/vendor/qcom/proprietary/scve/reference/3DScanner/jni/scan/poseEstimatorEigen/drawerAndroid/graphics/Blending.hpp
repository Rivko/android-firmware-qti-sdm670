/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef GRAPHICS_BLENDING_HPP
#define GRAPHICS_BLENDING_HPP

#include <cstdint>

namespace graphics {

struct Blending {
    enum class Equation : uint8_t {
        Add,
        Max,
        Min,
        ReverseSubtract,
        Subtract,
    };

     enum class Factor : uint8_t {
        One,
        OneMinusSinkAlpha,
        OneMinusSinkRGB,
        OneMinusSourceAlpha,
        OneMinusSourceRGB,
        SinkAlpha,
        SinkRGB,
        SourceAlpha,
        SourceRGB,
        Zero,
    };

    Equation alpha;
    Equation rgb;
    Factor sinkAlpha;
    Factor sinkRGB;
    Factor sourceAlpha;
    Factor sourceRGB;

    Blending();
};

}
#endif
