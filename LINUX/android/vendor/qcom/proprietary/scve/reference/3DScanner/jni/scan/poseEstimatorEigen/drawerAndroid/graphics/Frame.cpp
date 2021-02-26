/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "graphics/Frame.hpp"

#include <cassert>
#include <vector>

#include <pil/CG.hpp>

#include "graphics/Texture.hpp"

#ifndef NDEBUG
#include <core/Log.hpp>
#endif

namespace graphics {

Frame::Attachment::Attachment() : buffer(nullptr), index(0),
        layer(-1), mipmapLevel(0) {
}

struct Frame::Impl {
    uint32_t mID;
};

Frame::Frame() : mImpl(new Impl) {
    glGenFramebuffers(1, &mImpl->mID);
}

Frame::~Frame() {
    glDeleteFramebuffers(1, &mImpl->mID);
}

uint32_t Frame::getID() const {
    return mImpl->mID;
}

int32_t Frame::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, getID());

    return 0;
}

int32_t Frame::clearColor() {
    glClear(GL_COLOR_BUFFER_BIT);

    return 0;
}

int32_t Frame::clearDepth() {
    float value = 1.0f;
    glClearBufferfv(GL_DEPTH, 0, &value);

    return 0;
}

int32_t Frame::clearDepthStencil() {
    glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0);

    return 0;
}

int32_t Frame::clearStencil() {
    int32_t value = 0;
    glClearBufferiv(GL_STENCIL, 0, &value);

    return 0;
}

int32_t Frame::invalidateDepth() {
    uint32_t attachments = GL_DEPTH_ATTACHMENT;
    glInvalidateFramebuffer(GL_FRAMEBUFFER, 1, &attachments);

    return 0;
}

int32_t Frame::set(uint32_t count, const Attachment *descs) {
    assert(0 != count && nullptr != descs);

    std::vector<uint32_t> writeable;
    for(uint32_t i = 0; i < count; ++i) {
        auto &desc = descs[i];
        uint32_t attachment = 0;
        if(Texture::Format::Depth32F == desc.buffer->getFormat())
            attachment = GL_DEPTH_ATTACHMENT;
        else if(Texture::Format::Depth24_Stencil8UI == desc.buffer->getFormat()
                || Texture::Format::Depth32F_Stencil8UI ==
                    desc.buffer->getFormat())
            attachment = GL_DEPTH_STENCIL_ATTACHMENT;
        else if(Texture::Format::Stencil8UI == desc.buffer->getFormat())
            attachment = GL_STENCIL_ATTACHMENT;
        else {
            attachment = GL_COLOR_ATTACHMENT0 + desc.index;
            writeable.push_back(attachment);
        }

        if(desc.layer >= 0)
			glFramebufferTextureLayer(GL_FRAMEBUFFER, attachment,
                desc.buffer->getID(),
                static_cast<int32_t>(desc.mipmapLevel), desc.layer);
        else
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachment,
                GL_TEXTURE_2D,
                desc.buffer->getID(), static_cast<int32_t>(desc.mipmapLevel));
    }

    glDrawBuffers(static_cast<int32_t>(writeable.size()), writeable.data());

    glReadBuffer(GL_NONE);

    auto result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(GL_FRAMEBUFFER_COMPLETE != result) {
#ifndef NDEBUG
        core::Log logger;
        if(GL_FRAMEBUFFER_UNDEFINED == result)
            logger.record("GL_FRAMEBUFFER_UNDEFINED.");
        else if(GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT == result)
            logger.record("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT.");
        else if(GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT == result)
            logger.record("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT.");
        else if(GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE == result)
            logger.record("GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE.");
        else if(GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS == result)
            logger.record("GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS.");
        else if(GL_FRAMEBUFFER_UNSUPPORTED == result)
            logger.record("GL_FRAMEBUFFER_UNSUPPORTED.");
#endif
        return 1;
    }

    return 0;
}

int32_t Frame::unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return 0;
}

}
