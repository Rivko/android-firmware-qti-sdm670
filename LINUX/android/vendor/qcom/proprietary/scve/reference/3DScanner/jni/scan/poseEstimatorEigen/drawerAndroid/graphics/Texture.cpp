/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "graphics/Texture.hpp"

#include <cassert>
#include <cmath>
#include <algorithm>

#include <pil/CG.hpp>

#ifndef NDEBUG
#include <core/Log.hpp>
#endif

#include <resource/Image.hpp>

#ifndef GL_TEXTURE_EXTERNAL_OES
#define GL_TEXTURE_EXTERNAL_OES 0x8D65
#endif

struct Header {
    uint32_t dimensions;
    uint32_t numberMipmapLevels;
    uint32_t target;
    bool isCompressed;
    bool generateMipmaps;
    uint8_t unused_padding[2];
};

static uint32_t getInternalFormat(const graphics::Texture::Format &format);
static graphics::Texture::Format getInternalFormat(uint32_t internal_format);
static uint32_t getTarget(const graphics::Texture::Descriptor &desc);

static int32_t validateImage(const resource::Image &img, Header &header);

namespace graphics {

Texture::Descriptor::Descriptor() : arraySize(0), depth(0),
        format(static_cast<Format>(0)),
        height(0), mipmapLevels(1), width(0) {
}

struct Texture::Impl {
    Format mFormat;
    uint32_t mID;
    uint32_t mLevels;
    uint32_t mTarget;
};

Texture::Texture() : mImpl(new Impl) {
    glGenTextures(1, &mImpl->mID);
    mImpl->mTarget = 0;
}

Texture::~Texture() {
    glDeleteTextures(1, &mImpl->mID);
}

uint32_t Texture::getID() const {
    return mImpl->mID;
}

Texture::Format Texture::getFormat() const {
    return mImpl->mFormat;
}

int32_t Texture::bind() const {
    if(0 != mImpl->mTarget)
        glBindTexture(mImpl->mTarget, getID());

    return 0;
}

int32_t Texture::load(const Texture::Descriptor &desc) {
    if(Format::External == desc.format) {
        mImpl->mTarget = GL_TEXTURE_EXTERNAL_OES;
        bind();
        mImpl->mFormat = desc.format;
        mImpl->mLevels = 1;

        //no mip-mapping with external source
        glTexParameteri(GL_TEXTURE_EXTERNAL_OES,
            GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_EXTERNAL_OES,
            GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        //clamp to edge is only option for external source
        glTexParameteri(GL_TEXTURE_EXTERNAL_OES,
            GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_EXTERNAL_OES,
            GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        unbind();
        return 0;
    }

    mImpl->mTarget = getTarget(desc);
    bind();

    uint32_t internal_format = getInternalFormat(desc.format);
    mImpl->mFormat = desc.format;
    mImpl->mLevels = desc.mipmapLevels;

    if(GL_TEXTURE_2D == mImpl->mTarget || GL_TEXTURE_CUBE_MAP == mImpl->mTarget)
        glTexStorage2D(mImpl->mTarget, static_cast<int32_t>(desc.mipmapLevels),
            internal_format,
            static_cast<int32_t>(desc.width),
            static_cast<int32_t>(desc.height));
    else if(GL_TEXTURE_2D_ARRAY == mImpl->mTarget)
        glTexStorage3D(mImpl->mTarget, static_cast<int32_t>(desc.mipmapLevels),
            internal_format,
            static_cast<int32_t>(desc.width),
            static_cast<int32_t>(desc.height),
            static_cast<int32_t>(desc.arraySize));
    else
        glTexStorage3D(mImpl->mTarget, static_cast<int32_t>(desc.mipmapLevels),
            internal_format,
            static_cast<int32_t>(desc.width),
            static_cast<int32_t>(desc.height),
            static_cast<int32_t>(desc.depth));

    unbind();
    return 0;
}

int32_t Texture::load(const resource::Image &img) {
    Header header;
    auto result = validateImage(img, header);
    if(0 != result)
        return result;

    mImpl->mTarget = header.target;
    bind();

    int32_t saved_unpack_alignment;
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &saved_unpack_alignment);
    //KTX alignment requirement is 4
    if(4 != saved_unpack_alignment)
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    //determine formats: DOES NOT SUPPORT unsized formats
    uint32_t internal_format = img.getInternalFormat();
    mImpl->mFormat = getInternalFormat(internal_format);
    mImpl->mLevels = header.numberMipmapLevels;
    if(header.generateMipmaps) {
        auto max_dim = std::max<uint32_t>(img.getWidth(0),
            std::max<uint32_t>(img.getHeight(0), img.getDepth(0)));
        mImpl->mLevels = static_cast<uint32_t>(1 +
            std::log(max_dim) / std::log(2));
    }

    uint32_t pixel_format = img.getPixelFormat();
    uint32_t target = header.target;
    if(GL_TEXTURE_CUBE_MAP == header.target)
        target = GL_TEXTURE_CUBE_MAP_POSITIVE_X;

    //allocate immutable storage
    if(2 == header.dimensions)
        glTexStorage2D(header.target,
            static_cast<int32_t>(header.numberMipmapLevels), internal_format,
            static_cast<int32_t>(img.getWidth(0)),
            static_cast<int32_t>(img.getHeight(0)));
    else if(3 == header.dimensions) {
        if(img.getNumberArrayElements())
            glTexStorage3D(header.target,
                static_cast<int32_t>(header.numberMipmapLevels),
                internal_format,
                static_cast<int32_t>(img.getWidth(0)),
                static_cast<int32_t>(img.getHeight(0)),
                static_cast<int32_t>(img.getNumberArrayElements()));
        else
            glTexStorage3D(header.target,
                static_cast<int32_t>(header.numberMipmapLevels),
                internal_format,
                static_cast<int32_t>(img.getWidth(0)),
                static_cast<int32_t>(img.getHeight(0)),
                static_cast<int32_t>(img.getDepth(0)));
    }

    //initialize storage
    for(uint32_t level = 0; level < header.numberMipmapLevels; ++level) {
        int32_t depth = static_cast<int32_t>(
            std::max<uint32_t>(1, img.getDepth(level)));
        int32_t height = static_cast<int32_t>(
            std::max<uint32_t>(1, img.getHeight(level)));
        int32_t width = static_cast<int32_t>(
            std::max<uint32_t>(1, img.getWidth(level)));

        for(uint32_t face = 0; face < img.getNumberFaces(); ++face) {
            if(2 == header.dimensions) {
                if(header.isCompressed)
                    glCompressedTexSubImage2D(target + face,
                        static_cast<int32_t>(level), 0, 0,
                        width, height, internal_format,
                        static_cast<int32_t>(img.getSize(level)),
                        reinterpret_cast<void *>(img.getData(level, face)));
                else
                    glTexSubImage2D(target + face,
                        static_cast<int32_t>(level), 0, 0,
                        width, height, pixel_format, img.getType(),
                        reinterpret_cast<void *>(img.getData(level, face)));
            } else if(3 == header.dimensions) {
                if(img.getNumberArrayElements())
                    depth = static_cast<int32_t>(img.getNumberArrayElements());

                if(header.isCompressed)
                    glCompressedTexSubImage3D(target + face,
                        static_cast<int32_t>(level), 0, 0, 0,
                        width, height, depth, internal_format,
                        static_cast<int32_t>(img.getSize(level)),
                        reinterpret_cast<void *>(img.getData(level, face)));
                else
                    glTexSubImage3D(target + face,
                        static_cast<int32_t>(level), 0, 0, 0,
                        width, height, depth, pixel_format, img.getType(),
                        reinterpret_cast<void *>(img.getData(level, face)));
            }
        }
    }

    if(header.generateMipmaps)
        glGenerateMipmap(mImpl->mTarget);

    //restore to original unpack alignment
    if(4 != saved_unpack_alignment)
        glPixelStorei(GL_UNPACK_ALIGNMENT, saved_unpack_alignment);

    unbind();
    return 0;
}

int32_t Texture::unbind() const {
    if(0 != mImpl->mTarget)
        glBindTexture(mImpl->mTarget, 0);

    return 0;
}

}

static uint32_t getInternalFormat(const graphics::Texture::Format &format) {
    uint32_t internal_format = 0;

    switch(format) {
        case graphics::Texture::Format::Depth24_Stencil8UI: {
            internal_format = GL_DEPTH24_STENCIL8;
        } break;
        case graphics::Texture::Format::Depth32F: {
            internal_format = GL_DEPTH_COMPONENT32F;
        } break;
        case graphics::Texture::Format::Depth32F_Stencil8UI: {
            internal_format = GL_DEPTH32F_STENCIL8;
        } break;
        case graphics::Texture::Format::External: {
#ifndef NDEBUG
            core::Log logger;
            logger.record("No internal format exists for external textures.");
#endif
        } break;
        case graphics::Texture::Format::RG8: {
            internal_format = GL_RG8;
        } break;
        case graphics::Texture::Format::RG16F: {
            internal_format = GL_RG16F;
        } break;
        case graphics::Texture::Format::RG32F: {
            internal_format = GL_RG32F;
        } break;
        case graphics::Texture::Format::RGB8: {
            internal_format = GL_RGB8;
        } break;
        case graphics::Texture::Format::RGB16F: {
            internal_format = GL_RGB16F;
        } break;
        case graphics::Texture::Format::RGB32F: {
            internal_format = GL_RGB32F;
        } break;
        case graphics::Texture::Format::RGBA8: {
            internal_format = GL_RGBA8;
        } break;
        case graphics::Texture::Format::RGBA16F: {
            internal_format = GL_RGBA16F;
        } break;
        case graphics::Texture::Format::RGBA32F: {
            internal_format = GL_RGBA32F;
        } break;
        case graphics::Texture::Format::SRGB8: {
            internal_format = GL_SRGB8;
        } break;
        case graphics::Texture::Format::SRGB8_A8: {
            internal_format = GL_SRGB8_ALPHA8;
        } break;
        case graphics::Texture::Format::Stencil8UI: {
            internal_format = GL_STENCIL_INDEX8;
        } break;
    }

    return internal_format;
}

static graphics::Texture::Format getInternalFormat(uint32_t internal_format) {
    graphics::Texture::Format format = graphics::Texture::Format::RGB8;

    switch(internal_format) {
        case GL_DEPTH24_STENCIL8: {
            format = graphics::Texture::Format::Depth24_Stencil8UI;
        } break;
        case GL_DEPTH_COMPONENT32F: {
            format = graphics::Texture::Format::Depth32F;
        } break;
        case GL_DEPTH32F_STENCIL8: {
            format = graphics::Texture::Format::Depth32F_Stencil8UI;
        } break;
        case GL_RG8: {
            format = graphics::Texture::Format::RG8;
        } break;
        case GL_RG16F: {
            format = graphics::Texture::Format::RG16F;
        } break;
        case GL_RG32F: {
            format = graphics::Texture::Format::RG32F;
        } break;
        case GL_RGB8: {
            format = graphics::Texture::Format::RGB8;
        } break;
        case GL_RGB16F: {
            format = graphics::Texture::Format::RGB16F;
        } break;
        case GL_RGB32F: {
            format = graphics::Texture::Format::RGB32F;
        } break;
        case GL_RGBA8: {
            format = graphics::Texture::Format::RGBA8;
        } break;
        case GL_RGBA16F: {
            format = graphics::Texture::Format::RGBA16F;
        } break;
        case GL_RGBA32F: {
            format = graphics::Texture::Format::RGBA32F;
        } break;
        case GL_SRGB8: {
            format = graphics::Texture::Format::SRGB8;
        } break;
        case GL_SRGB8_ALPHA8: {
            format = graphics::Texture::Format::SRGB8_A8;
        } break;
        case GL_STENCIL_INDEX8: {
            format = graphics::Texture::Format::Stencil8UI;
        } break;
    }

    return format;
}

static uint32_t getTarget(const graphics::Texture::Descriptor &desc) {
    assert(desc.width > 0 && desc.height > 0);
    if(0 == desc.arraySize && 0 == desc.depth)
        return GL_TEXTURE_2D;

    if(0 == desc.depth)
        return GL_TEXTURE_2D_ARRAY;

    if(0 == desc.arraySize)
        return GL_TEXTURE_3D;

    //There does not exist 3D array textures yet.
    //Current configurations are:
    //  2D - (w > 0, h > 0)
    //  3D - (w > 0, h > 0, d > 0)
    //  2D Array - (w > 0, h > 0, a > 0)
    //  Cube Map - (w > 0, h > 0, a > 0, d == 6)
    assert(6 == desc.depth);
    return GL_TEXTURE_CUBE_MAP;
}

static int32_t validateImage(const resource::Image &img, Header &header) {
    header.isCompressed = false;
    if(0 == img.getType() || 0 == img.getPixelFormat()) {
        //compressed image must set type and pixel format to 0
        if(img.getType() + img.getPixelFormat() != 0)
            return 1;

        header.isCompressed = true;
    }

    if(0 == img.getWidth(0) || (img.getDepth(0) > 0 && 0 == img.getHeight(0)))
        return 2;

    //force 1D textures to be loaded as 2D
    header.dimensions = 2;
    header.target = GL_TEXTURE_2D;

    if(img.getDepth(0) > 0) {
        header.dimensions = 3;
        header.target = GL_TEXTURE_3D;
    }

    //KTX requires this field to be either 1 or 6
    if(!(6 == img.getNumberFaces() || 1 == img.getNumberFaces()))
        return 3;

    if(6 == img.getNumberFaces()) {
        if(2 != header.dimensions)
            //cubemap requires 2D faces
            return 4;
        header.target = GL_TEXTURE_CUBE_MAP;
    }

    if(img.getNumberArrayElements() > 0) {
        if(GL_TEXTURE_2D != header.target)
            //3D and cubemap arrays not supported
            return 5;
        header.target = GL_TEXTURE_2D_ARRAY;
        header.dimensions = 3;
    }

    header.numberMipmapLevels = img.getNumberMipmapLevels();
    header.generateMipmaps = false;
    if(0 == header.numberMipmapLevels) {
        header.generateMipmaps = true;
        header.numberMipmapLevels = 1;
    }

    auto max_dim = std::max<uint32_t>(img.getWidth(0),
        std::max<uint32_t>(img.getHeight(0), img.getDepth(0)));
    if(max_dim < (0x00000001 << (header.numberMipmapLevels - 1)))
        //cannot have more levels than 1 + log_2(max(width, height, depth))
        return 6;

    return 0;
}
