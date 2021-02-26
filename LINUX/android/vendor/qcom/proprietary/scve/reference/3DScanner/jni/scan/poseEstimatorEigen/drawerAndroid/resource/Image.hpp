/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef RESOURCE_IMAGE_HPP
#define RESOURCE_IMAGE_HPP

#include <memory>

namespace resource {

class Image {
public:
    Image();
    Image(const uint8_t *ktx_binary);
    ~Image();

    uint32_t getBaseInternalFormat() const;
    uintptr_t getData(uint32_t mipmap_level) const;
    uintptr_t getData(uint32_t mipmap_level, uint32_t cubemap_face) const;
    uint32_t getDepth(uint32_t mipmap_level) const;
    uint32_t getHeight(uint32_t mipmap_level) const;
    uint32_t getInternalFormat() const;
    const char *getMetadata(const char *key) const;
    uint32_t getNumberArrayElements() const;
    uint32_t getNumberFaces() const;
    uint32_t getNumberMipmapLevels() const;
    uint32_t getPixelFormat() const;
    uint32_t getSize(uint32_t mipmap_level) const;
    uint32_t getType() const;
    uint32_t getWidth(uint32_t mipmap_level) const;

    int32_t load(const uint8_t *ktx_binary);

private:
    Image(const Image &i) = delete;
    Image &operator=(const Image &i) = delete;

    struct Impl;
    std::unique_ptr<Impl> mImpl;
};

}

#endif
