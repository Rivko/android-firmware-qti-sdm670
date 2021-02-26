/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef RESOURCE_ASSETS_HPP
#define RESOURCE_ASSETS_HPP

#include <cstdint>

#include "resource/Shader.hpp"

namespace resource {

class Image;
class Model;
class Shader;

class Assets {
public:
    Assets();
    ~Assets();

    int32_t load(const char *file_name, Image *&img);
    int32_t load(const char *file_name, Model *&model);
    int32_t load(const char *file_name, Shader *&shader);

    int32_t loadShaderStr(char *shader_str, Shader *&shader, Shader::Type type);

private:
    Assets(const Assets &) = delete;
    Assets &operator=(const Assets &) = delete;
};

}
#endif
