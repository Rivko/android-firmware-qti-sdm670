/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "resource/Assets.hpp"

#include <cassert>

#include <pil/File.hpp>

#include "3DR_APIs.h"

#include "resource/Image.hpp"
#include "resource/Model.hpp"
#include "resource/Shader.hpp"

namespace resource {

Assets::Assets() {
}

Assets::~Assets() {
}

int32_t Assets::load(const char *file_name, Image *&img) {
    if(nullptr == file_name)
        return 1;

    pil::File file(file_name);
    auto file_size = file.getSize();
    if(file_size <= 0)
        return 2;

    if(!file.hasExtension("ktx"))
        return 3;

    std::unique_ptr<uint8_t[]> data(new uint8_t[file_size]);
    if(file_size != file.read(file_size, data.get()))
        return 4;

    std::unique_ptr<Image> new_img(new Image);
    if(new_img->load(data.get()))
        return 5;

    img = new_img.release();
    return 0;
}

int32_t Assets::load(const char *file_name, Model *&model) {
    if(nullptr == file_name)
        return 1;

    pil::File file(file_name);
    auto file_size = file.getSize();
    if(file_size <= 0)
        return 2;

    std::unique_ptr<uint8_t[]> data(new uint8_t[file_size]);
    if(file_size != file.read(file_size, data.get()))
        return 3;

    uint64_t size = static_cast<uint64_t>(file_size);
    std::unique_ptr<Model> m(new Model);
    if(m->load(size, data.get()))
        return 4;

    model = m.release();
    return 0;
}

int32_t Assets::load(const char *file_name, Shader *&shader) {
    if(nullptr == file_name)
        return 1;

    pil::File file(file_name);
    auto file_size = file.getSize();
    if(file_size <= 0)
        return 2;

    std::unique_ptr<Shader> s = nullptr;
    if(file.hasExtension("vert"))
        s.reset(new Shader(Shader::Type::Vertex));
    else if(file.hasExtension("tesc"))
        s.reset(new Shader(Shader::Type::TessellationControl));
    else if(file.hasExtension("tese"))
        s.reset(new Shader(Shader::Type::TessellationEvaluation));
    else if(file.hasExtension("geom"))
        s.reset(new Shader(Shader::Type::Geometry));
    else if(file.hasExtension("frag"))
        s.reset(new Shader(Shader::Type::Fragment));
    else if(file.hasExtension("comp"))
        s.reset(new Shader(Shader::Type::Compute));
    else
        return 3;

    std::unique_ptr<uint8_t[]> data(new uint8_t[file_size]);
    if(file_size != file.read(file_size, data.get())) {
        return 4;
    }

    const char *src = reinterpret_cast<const char *>(data.get());
#ifndef NDEBUG
    core::Log logger;
    logger.record("Loading %s...", file_name);
#endif

    uint32_t length = static_cast<uint32_t>(file_size);
    if(s->load(1, &src, &length))
        return 5;

    shader = s.release();
    return 0;
}

int32_t Assets::loadShaderStr(char *shader_str, Shader *&shader, Shader::Type type) {

    std::unique_ptr<Shader> s = nullptr;
    s.reset(new Shader(type));

    if (s->load(shader_str))
        return 2;

    shader = s.release();
    return 0;
}

}
