/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "graphics/Program.hpp"

#include <cassert>

#include <pil/CG.hpp>

#include <core/Matrix.hpp>

#include <resource/Shader.hpp>

#ifndef NDEBUG
#include <core/Log.hpp>
#endif

namespace graphics {

struct Program::Impl {
    uint32_t mID;
};

Program::Program() : mImpl(new Impl) {
    mImpl->mID = glCreateProgram();
}

Program::~Program() {
    glDeleteProgram(getID());
}

uint32_t Program::getID() const {
    return mImpl->mID;
}

int32_t Program::link(const resource::Shader &shader) {
    const resource::Shader *list_shaders[] {&shader};
    return link(1, list_shaders);
}

int32_t Program::link(uint32_t count, const resource::Shader *shaders[]) {
    assert(0 != count && nullptr != shaders);

    //compile each shader
    std::unique_ptr<uint32_t[]> shader_ids(new uint32_t[count]);
    for(uint32_t i = 0; i < count; ++i) {
        assert(nullptr != shaders[i]);

        auto &shader = *shaders[i];
        uint32_t shader_type = 0;
        switch(shader.getType()) {
            case resource::Shader::Type::Compute: {
                shader_type = 0;
            } break;
            case resource::Shader::Type::Fragment: {
                shader_type = GL_FRAGMENT_SHADER;
            } break;
            case resource::Shader::Type::Geometry: {
                shader_type = GL_GEOMETRY_SHADER;
            } break;
            case resource::Shader::Type::TessellationControl: {
                shader_type = GL_TESS_CONTROL_SHADER;
            } break;
            case resource::Shader::Type::TessellationEvaluation: {
                shader_type = GL_TESS_EVALUATION_SHADER;
            } break;
            case resource::Shader::Type::Vertex: {
                shader_type = GL_VERTEX_SHADER;
            } break;
        }
        uint32_t shader_id = glCreateShader(shader_type);
        shader_ids[i] = shader_id;

        auto shader_src = shader.getSource();
        glShaderSource(shader_id, 1, &shader_src, nullptr);
        glCompileShader(shader_id);

        int32_t compile_status;
        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compile_status);
        if(GL_FALSE == compile_status) {
#ifndef NDEBUG
            core::Log logger;
            int32_t log_length;
            glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);

            std::unique_ptr<char[]> info_log(new char[log_length + 1]);
            glGetShaderInfoLog(shader_id, log_length, nullptr, info_log.get());
            info_log[static_cast<uint32_t>(log_length)] = '\0';

            logger.record("Could not compile shader: %s", info_log.get());
#endif
            return static_cast<int32_t>(i);
        }

        glAttachShader(getID(), shader_id);

        //already attached, so deleting shader is okay
        glDeleteShader(shader_id);
    }

    //create executable to be run on GPU
    glLinkProgram(getID());
    int32_t link_status = GL_FALSE;
    glGetProgramiv(getID(), GL_LINK_STATUS, &link_status);
    if(GL_FALSE == link_status) {
#ifndef NDEBUG
        core::Log logger;
        int32_t log_length;
        glGetProgramiv(getID(), GL_INFO_LOG_LENGTH, &log_length);

        std::unique_ptr<char[]> info_log(new char[log_length + 1]);
        glGetProgramInfoLog(getID(), log_length, nullptr, info_log.get());
        info_log[static_cast<uint32_t>(log_length)] = '\0';

        logger.record("Failed to link program (%d): %s", getID(),
            info_log.get());
#endif
        return -1;
    }

    //no need to keep around shaders after linking program
    for(uint32_t i = 0; i < count; ++i)
        glDetachShader(getID(), shader_ids[i]);

    return 0;
}

int32_t Program::update(const char *uniform_name, const core::Matrix &m) {
    assert(nullptr != uniform_name);
    auto loc = glGetUniformLocation(getID(), uniform_name);
    if(-1 == loc)
        return 1;

    glUniformMatrix4fv(loc, 1, false, m.getData());
    return 0;
}

int32_t Program::update(const char *uniform_name, const core::Point &p) {
    assert(nullptr != uniform_name);
    auto loc = glGetUniformLocation(getID(), uniform_name);
    if(-1 == loc)
        return 1;

    glUniform3f(loc, p(0), p(1), p(2));
    return 0;
}

int32_t Program::update(const char *uniform_name, const core::Vector &v) {
    assert(nullptr != uniform_name);
    auto loc = glGetUniformLocation(getID(), uniform_name);
    if(-1 == loc)
        return 1;

    glUniform3f(loc, v(0), v(1), v(2));
    return 0;
}

int32_t Program::update(const char *uniform_name, float v) {
    assert(nullptr != uniform_name);
    auto loc = glGetUniformLocation(getID(), uniform_name);
    if(-1 == loc)
        return 1;

    glUniform1f(loc, v);
    return 0;
}

int32_t Program::update(const char *uniform_name, float v0, float v1) {
    assert(nullptr != uniform_name);
    auto loc = glGetUniformLocation(getID(), uniform_name);
    if(-1 == loc)
        return 1;

    glUniform2f(loc, v0, v1);
    return 0;
}

}
