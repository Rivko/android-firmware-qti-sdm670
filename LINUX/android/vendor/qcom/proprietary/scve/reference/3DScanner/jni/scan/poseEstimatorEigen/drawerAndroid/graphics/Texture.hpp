/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef GRAPHICS_TEXTURE_HPP
#define GRAPHICS_TEXTURE_HPP

#include <memory>

namespace resource {

class Image;

}

namespace graphics {

class Texture {
public:
    //types without a suffix {F, UI, I} is by
    //default unsigned normalized fixed-point
    enum class Format : uint32_t {
        Depth24_Stencil8UI,
        Depth32F,
        Depth32F_Stencil8UI,
        External,
        RG8,
        RG16F,
        RG32F,
        RGB8,
        RGB16F,
        RGB32F,
        RGBA8,
        RGBA16F,
        RGBA32F,
        SRGB8,
        SRGB8_A8,
        Stencil8UI,
    };

    struct Descriptor {
        uint32_t arraySize;
        uint32_t depth;
        Format format;
        uint32_t height;
        uint32_t mipmapLevels;
        uint32_t width;

        Descriptor();
    };

    Texture();
    ~Texture();

    uint32_t getID() const;
    Format getFormat() const;

    int32_t bind() const;
    int32_t load(const Descriptor &desc);
    int32_t load(const resource::Image &img);
    int32_t unbind() const;

private:
    Texture(const Texture &) = delete;
    Texture &operator=(const Texture &) = delete;

    struct Impl;
    std::unique_ptr<Impl> mImpl;
};

}
#endif
