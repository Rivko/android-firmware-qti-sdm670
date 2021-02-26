/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "graphics/Sampler.hpp"

#include <cassert>

#include <pil/CG.hpp>

static graphics::Sampler::Filter getSamplerFilter(uint32_t id);
static graphics::Sampler::Mipmap getSamplerMipmap(uint32_t id);
static void setSamplerParameters(uint32_t id, graphics::Sampler::Filter
        min_mag_filter, graphics::Sampler::Mipmap mm);

namespace graphics {

struct Sampler::Impl {
    uint32_t mID;
};

Sampler::Sampler() : mImpl(new Impl) {
    glGenSamplers(1, &mImpl->mID);
}

Sampler::~Sampler() {
    glDeleteSamplers(1, &mImpl->mID);
}

uint32_t Sampler::getID() const {
    return mImpl->mID;
}

int32_t Sampler::set(Sampler::Filter min_mag_filter) {
    Sampler::Mipmap mm = getSamplerMipmap(getID());

    setSamplerParameters(getID(), min_mag_filter, mm);

    return 0;
}

int32_t Sampler::set(Sampler::Mipmap mm) {
    Sampler::Filter min_mag_filter = getSamplerFilter(getID());

    setSamplerParameters(getID(), min_mag_filter, mm);

    return 0;
}

int32_t Sampler::set(Sampler::Wrap texture_wrap) {
    int32_t mode = GL_CLAMP_TO_EDGE;
    if(Wrap::Repeat == texture_wrap)
        mode = GL_REPEAT;

    glSamplerParameteri(getID(), GL_TEXTURE_WRAP_S, mode);
    glSamplerParameteri(getID(), GL_TEXTURE_WRAP_T, mode);
    glSamplerParameteri(getID(), GL_TEXTURE_WRAP_R, mode);

    return 0;
}

}

static graphics::Sampler::Filter getSamplerFilter(uint32_t id) {
    int32_t param;
    glGetSamplerParameteriv(id, GL_TEXTURE_MAG_FILTER, &param);

    if(GL_NEAREST == param)
        return graphics::Sampler::Filter::None;

    return graphics::Sampler::Filter::Bilinear;
}

static graphics::Sampler::Mipmap getSamplerMipmap(uint32_t id) {
    int32_t param;
    glGetSamplerParameteriv(id, GL_TEXTURE_MIN_FILTER, &param);
    if(GL_NEAREST == param || GL_LINEAR == param)
        return graphics::Sampler::Mipmap::None;
    else if(GL_NEAREST_MIPMAP_NEAREST == param ||
            GL_LINEAR_MIPMAP_NEAREST == param)
        return graphics::Sampler::Mipmap::Sharp;

    return graphics::Sampler::Mipmap::Smooth;
}

static void setSamplerParameters(uint32_t id, graphics::Sampler::Filter
        min_mag_filter, graphics::Sampler::Mipmap mm) {
    if(graphics::Sampler::Filter::None == min_mag_filter)
        glSamplerParameteri(id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    else if(graphics::Sampler::Filter::Bilinear == min_mag_filter)
        glSamplerParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if(graphics::Sampler::Mipmap::None == mm) {
        if(graphics::Sampler::Filter::None == min_mag_filter)
            glSamplerParameteri(id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        else if(graphics::Sampler::Filter::Bilinear == min_mag_filter)
            glSamplerParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    } else if(graphics::Sampler::Mipmap::Sharp == mm) {
        if(graphics::Sampler::Filter::None == min_mag_filter)
            glSamplerParameteri(id, GL_TEXTURE_MIN_FILTER,
                GL_NEAREST_MIPMAP_NEAREST);
        else if(graphics::Sampler::Filter::Bilinear == min_mag_filter)
            glSamplerParameteri(id, GL_TEXTURE_MIN_FILTER,
                GL_LINEAR_MIPMAP_NEAREST);
    } else if(graphics::Sampler::Mipmap::Smooth == mm) {
        if(graphics::Sampler::Filter::None == min_mag_filter)
            glSamplerParameteri(id, GL_TEXTURE_MIN_FILTER,
                GL_NEAREST_MIPMAP_LINEAR);
        else if(graphics::Sampler::Filter::Bilinear == min_mag_filter)
            glSamplerParameteri(id, GL_TEXTURE_MIN_FILTER,
                GL_LINEAR_MIPMAP_LINEAR);
    }
}
