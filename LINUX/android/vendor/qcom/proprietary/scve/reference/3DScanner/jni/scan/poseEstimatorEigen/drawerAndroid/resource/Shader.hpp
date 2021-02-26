/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef RESOURCE_SHADER_HPP
#define RESOURCE_SHADER_HPP

#include <memory>

namespace resource {

class Shader {
public:
    enum class Type : uint32_t {
        Compute,
        Fragment,
        Geometry,
        TessellationControl,
        TessellationEvaluation,
        Vertex
    };

    Shader(Shader::Type type);
    ~Shader();

    const char *getSource() const;
    Type getType() const;

    int32_t load(const char *src);
    int32_t load(uint32_t num_srcs, const char **srcs, const uint32_t *lengths);

private:
    Shader() = delete;
    Shader(const Shader &) = delete;
    Shader &operator=(const Shader &) = delete;

    struct Impl;
    std::unique_ptr<Impl> mImpl;
};

}
#endif
