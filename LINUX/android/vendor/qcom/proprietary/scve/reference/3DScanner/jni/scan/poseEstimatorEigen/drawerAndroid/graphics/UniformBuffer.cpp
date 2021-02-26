/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "graphics/UniformBuffer.hpp"

#include <cassert>
#include <unordered_map>
#include <vector>
#include <string>

#include <pil/CG.hpp>

#include "graphics/Program.hpp"

#ifndef NDEBUG
#include <core/Log.hpp>
#endif

struct BlockInfo {
    uintptr_t size;
    intptr_t offset;
    uint32_t bindingPoint;
    uint8_t unused_padding[4];

    BlockInfo();
};

BlockInfo::BlockInfo() : size(0), offset(0), bindingPoint(0) {
}

static intptr_t align(uintptr_t alignment, intptr_t n);

namespace graphics {

struct UniformBuffer::Impl {
    std::vector<std::unique_ptr<BlockInfo>> mBlocks;
    std::unordered_map<std::string, uint32_t> mBlockNames;
    uint32_t mID;

    uint8_t unused_padding[4];
};

UniformBuffer::UniformBuffer() : mImpl(new Impl) {
    glGenBuffers(1, &mImpl->mID);
}

UniformBuffer::~UniformBuffer() {
    glDeleteBuffers(1, &mImpl->mID);
}

uint32_t UniformBuffer::getBlockBinding(uint32_t index) const {
    assert(index < mImpl->mBlocks.size());

    return mImpl->mBlocks[index]->bindingPoint;

    return 0;
}

intptr_t UniformBuffer::getBlockOffset(uint32_t index) const {
    assert(index < mImpl->mBlocks.size());

    return mImpl->mBlocks[index]->offset;

    return 0;
}

uintptr_t UniformBuffer::getBlockSize(uint32_t index) const {
    assert(index < mImpl->mBlocks.size());

    return mImpl->mBlocks[index]->size;

    return 0;
}

uint32_t UniformBuffer::getID() const {
    return mImpl->mID;
}

uint32_t UniformBuffer::getNumberBlocks() const {
    return static_cast<uint32_t>(mImpl->mBlocks.size());
}

int32_t UniformBuffer::bind() const {
    glBindBuffer(GL_UNIFORM_BUFFER, getID());

    return 0;
};

int32_t UniformBuffer::load(uint32_t count, const Program *programs[]) {
    assert(0 != count && nullptr != programs);

#ifndef NDEBUG
    core::Log logger;
#endif
    uint32_t props[4];
    int32_t params[4];

    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, params);
    const int32_t kUniformBlockAlignment = params[0];

    glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, params);
    std::unique_ptr<bool[]> available_binding_points(new bool[params[0]]);
    for(int32_t i = 0; i < params[0]; ++i)
        available_binding_points[static_cast<uint32_t>(i)] = true;

    uint32_t current_binding_point = 0;
    intptr_t current_block_offset = 0;
    for(auto pidx = 0U; pidx < count; ++pidx) {
        assert(nullptr != programs[pidx]);
        auto &program = *programs[pidx];

        glGetProgramiv(program.getID(), GL_ACTIVE_UNIFORM_BLOCKS, params);
        uint32_t number_blocks = static_cast<uint32_t>(params[0]);
#ifndef NDEBUG
            logger.record("Program (%d) contains (%d) active blocks",
                program.getID(), number_blocks);
#endif
        //no uniform blocks to process
        if(0 == number_blocks)
            continue;

        //allocate storage for the longest block name in this program
        //includes null-terminating character
        glGetProgramiv(program.getID(), GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH,
            params);
        std::unique_ptr<char[]> block_name(new char[params[0]]);
        const int32_t max_length = params[0];

        //loop through each block
        for(uint32_t i = 0; i < number_blocks; ++i) {
            //ensure block haven't been processed
            glGetActiveUniformBlockName(program.getID(), i, max_length,
                nullptr, block_name.get());
            if(mImpl->mBlockNames.end() !=
                    mImpl->mBlockNames.find(block_name.get())) {
                auto &info = mImpl->mBlocks[
                    mImpl->mBlockNames[block_name.get()]];
                glUniformBlockBinding(program.getID(), i, info->bindingPoint);
#ifndef NDEBUG
                logger.record("Already processed: Block (%s, binding = %d) at "
                    "offset (%d) spans (%d) bytes", block_name.get(),
                    info->bindingPoint, info->offset, info->size);
#endif
                continue;
            }

            //new block
            std::unique_ptr<BlockInfo> info(new BlockInfo);
            props[0] = GL_UNIFORM_BLOCK_DATA_SIZE;
            props[1] = GL_UNIFORM_BLOCK_BINDING;
            glGetActiveUniformBlockiv(program.getID(), i,
                props[0], &params[0]);
            glGetActiveUniformBlockiv(program.getID(), i,
                props[1], &params[1]);

            info->size = static_cast<uintptr_t>(params[0]);
            if(0 == params[1]) {
                while(!available_binding_points[current_binding_point])
                    ++current_binding_point;

                info->bindingPoint = current_binding_point++;
                glUniformBlockBinding(program.getID(), i, info->bindingPoint);
            } else
                info->bindingPoint = static_cast<uint32_t>(params[1]);

            available_binding_points[info->bindingPoint] = false;
            info->offset = current_block_offset;

            //move to next aligned section
            current_block_offset += info->size;
            auto new_size = align(kUniformBlockAlignment, current_block_offset);
            auto padding = new_size - current_block_offset;
            current_block_offset += padding;

#ifndef NDEBUG
            logger.record("Block (%s, binding = %d) at offset (%d) "
                "spans (%d) bytes", block_name.get(), info->bindingPoint,
                    info->offset, info->size);
#endif

            mImpl->mBlockNames[block_name.get()] =
                static_cast<uint32_t>(mImpl->mBlocks.size());
            mImpl->mBlocks.push_back(std::move(info));
        }
    }

    assert(0 != current_block_offset);
    glBufferData(GL_UNIFORM_BUFFER, current_block_offset, nullptr,
        GL_DYNAMIC_DRAW);

    return 0;
}

int32_t UniformBuffer::update(const char *block_name,
        intptr_t offset, uintptr_t size, uintptr_t data) {
    auto iter = mImpl->mBlockNames.find(block_name);
    if(mImpl->mBlockNames.end() == iter)
        return 1;

    auto &info = mImpl->mBlocks[iter->second];

    glBufferSubData(GL_UNIFORM_BUFFER, info->offset + offset,
        static_cast<int32_t>(size), reinterpret_cast<void *>(data));

    return 0;
}

int32_t UniformBuffer::unbind() const {
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    return 0;
};

}

static intptr_t align(uintptr_t alignment, intptr_t n) {
    return (n + alignment - 1) - (n + alignment - 1) % alignment;
}
