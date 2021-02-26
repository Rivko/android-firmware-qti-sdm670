/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef GRAPHICS_UNIFORMBUFFER_HPP
#define GRAPHICS_UNIFORMBUFFER_HPP

#include <memory>

namespace graphics {

class Program;

class UniformBuffer {
public:
    UniformBuffer();
    ~UniformBuffer();

    uint32_t getBlockBinding(uint32_t index) const;
    intptr_t getBlockOffset(uint32_t index) const;
    uintptr_t getBlockSize(uint32_t index) const;
    uint32_t getID() const;
    uint32_t getNumberBlocks() const;

    int32_t bind() const;
    int32_t load(uint32_t count, const Program *programs[]);
    int32_t update(const char *block_name,
        intptr_t offset, uintptr_t size, uintptr_t data);
    int32_t unbind() const;

private:
    UniformBuffer(const UniformBuffer &) = delete;
    UniformBuffer &operator=(const UniformBuffer &) = delete;

    struct Impl;
    std::unique_ptr<Impl> mImpl;
};

}
#endif
