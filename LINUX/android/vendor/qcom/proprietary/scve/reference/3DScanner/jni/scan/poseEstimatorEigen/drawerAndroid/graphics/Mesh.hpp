/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef GRAPHICS_MESH_HPP
#define GRAPHICS_MESH_HPP

#include <memory>

namespace resource {

class Model;

}

namespace graphics {

class Mesh {
public:
    Mesh(const resource::Model &model);
    ~Mesh();

    int32_t draw() const;
    int32_t update(const resource::Model &model);

private:
    Mesh(const Mesh &) = delete;
    Mesh &operator=(const Mesh &) = delete;

    struct Impl;
    std::unique_ptr<Impl> mImpl;
};

}
#endif
