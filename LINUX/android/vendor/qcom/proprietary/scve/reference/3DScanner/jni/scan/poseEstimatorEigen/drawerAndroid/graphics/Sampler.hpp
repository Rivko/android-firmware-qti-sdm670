/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef GRAPHICS_SAMPLER_HPP
#define GRAPHICS_SAMPLER_HPP

#include <memory>

namespace graphics {

class Sampler {
public:
    enum class Filter : uint8_t {
        None,
        Bilinear
    };

    enum class Mipmap : uint8_t {
        None,
        Sharp,
        Smooth
    };

    enum class Wrap : uint8_t {
        ClampToEdge,
        Repeat
    };

    Sampler();
    ~Sampler();

    uint32_t getID() const;

    int32_t set(Filter min_mag_filter);
    int32_t set(Mipmap mm);
    int32_t set(Wrap texture_wrap);

private:
    Sampler(const Sampler &) = delete;
    Sampler &operator=(const Sampler &) = delete;

    struct Impl;
    std::unique_ptr<Impl> mImpl;
};

}
#endif
