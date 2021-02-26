/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef GRAPHICS_PROGRAM_HPP
#define GRAPHICS_PROGRAM_HPP

#include <memory>

namespace core {

class Matrix;
class Point;
class Vector;

}

namespace resource {

class Shader;

}

namespace graphics {

class Program {
public:
    Program();
    ~Program();

    uint32_t getID() const;

    int32_t link(const resource::Shader &shader);
    int32_t link(uint32_t count, const resource::Shader *shaders[]);
    int32_t update(const char *uniform_name, const core::Matrix &m);
    int32_t update(const char *uniform_name, const core::Point &p);
    int32_t update(const char *uniform_name, const core::Vector &v);
    int32_t update(const char *uniform_name, float v);
    int32_t update(const char *uniform_name, float v0, float v1);

private:
    Program(const Program &) = delete;
    Program &operator=(const Program &) = delete;

    struct Impl;
    std::unique_ptr<Impl> mImpl;
};

}
#endif
