/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "resource/Shader.hpp"

#include <cstring>

#ifndef NDEBUG
#include <core/Log.hpp>
#endif

namespace resource {

struct Shader::Impl {
    std::unique_ptr<char[]> mSource;
    Shader::Type mType;
};

Shader::Shader(Shader::Type type) : mImpl(new Impl) {
    mImpl->mType = type;
    mImpl->mSource = nullptr;
}

Shader::~Shader() {
}

const char *Shader::getSource() const {
    return mImpl->mSource.get();
}

Shader::Type Shader::getType() const {
    return mImpl->mType;
}

int32_t Shader::load(const char *src) {
    auto length = static_cast<uint32_t>(strlen(src));
    return load(1, &src, &length);
}

int32_t Shader::load(uint32_t num_srcs, const char **srcs,
        const uint32_t *lengths) {
    if(nullptr == srcs)
        return 1;

    if(nullptr == lengths)
        return 2;

    uint32_t total_length = 0;
    for(uint32_t i = 0; i < num_srcs; ++i) {
        if(nullptr == srcs[i])
            return 3;
        total_length += lengths[i];
    }

    if(0 == total_length)
        return 4;

    mImpl->mSource.reset(new char[total_length + 1]);

    auto ptr = mImpl->mSource.get();
    for(uint32_t i = 0; i < num_srcs; ++i) {
        memcpy(ptr, srcs[i], lengths[i]);
        ptr += lengths[i];
    }
    mImpl->mSource[total_length] = '\0';

#ifndef NDEBUG
    core::Log logger;
    logger.record("Loaded %s Shader into memory:",
        Type::Compute == getType() ? "Compute" :
        Type::Fragment == getType() ? "Fragment" :
        Type::Geometry == getType() ? "Geometry" :
        Type::TessellationControl == getType() ? "Tessellation Control" :
        Type::TessellationEvaluation == getType() ? "Tessellation Evaluation" :
        Type::Vertex == getType() ? "Vertex" :
        "Invalid");
#endif

    return 0;
}

}
