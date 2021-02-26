/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef GRAPHICS_FRAME_HPP
#define GRAPHICS_FRAME_HPP

#include <functional>
#include <memory>
#include <vector>

namespace graphics {

class Texture;

class Frame {
public:
    struct Attachment {
        const Texture *buffer;
        uint32_t index;
        int32_t layer;
        uint32_t mipmapLevel;
        uint8_t unused_padding[4];

        Attachment();
    };

    Frame();
    ~Frame();

    uint32_t getID() const;

    int32_t bind() const;
    int32_t clearColor();
    int32_t clearDepth();
    int32_t clearDepthStencil();
    int32_t clearStencil();
    int32_t invalidateDepth();
    int32_t set(uint32_t count, const Attachment *descs);
    int32_t unbind() const;

private:
    Frame(const Frame &) = delete;
    Frame &operator=(const Frame &) = delete;

    struct Impl;
    std::unique_ptr<Impl> mImpl;
};

}
#endif
