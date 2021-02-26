/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "graphics/Mesh.hpp"

#include <cassert>

#include <pil/CG.hpp>

#include <resource/Model.hpp>

namespace graphics {

struct Mesh::Impl {
    std::unique_ptr<uint32_t[]> mVAO;
    std::unique_ptr<uint32_t[]> mData;
    std::unique_ptr<uint32_t[]> mIndex;
    std::unique_ptr<int32_t[]> mCount;
    uint32_t mPartitions;
    uint8_t unused_padding[4];
};

Mesh::Mesh(const resource::Model &m) : mImpl(new Impl) {
    mImpl->mPartitions = m.getNumberPartitions();
    mImpl->mVAO.reset(new uint32_t[mImpl->mPartitions]);
    mImpl->mData.reset(new uint32_t[mImpl->mPartitions]);
    mImpl->mCount.reset(new int32_t[mImpl->mPartitions]);

    glGenVertexArrays(
        static_cast<int32_t>(mImpl->mPartitions), mImpl->mVAO.get());
    glGenBuffers(
        static_cast<int32_t>(mImpl->mPartitions), mImpl->mData.get());

    if(resource::Model::IndexType::None == m.getIndexType())
        mImpl->mIndex = nullptr;
    else {
        mImpl->mIndex.reset(new uint32_t[mImpl->mPartitions]);
        glGenBuffers(
            static_cast<int32_t>(mImpl->mPartitions), mImpl->mIndex.get());
    }

    uint32_t current_vao_binding;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING,
        reinterpret_cast<int32_t *>(&current_vao_binding));
    for(uint32_t i = 0; i < mImpl->mPartitions; ++i) {
        //tell GPU which format data is in
        glBindVertexArray(mImpl->mVAO[i]);

        //upload data to GPU
        glBindBuffer(GL_ARRAY_BUFFER, mImpl->mData[i]);
        glBufferData(GL_ARRAY_BUFFER, static_cast<int32_t>(m.getDataSize(i)),
            reinterpret_cast<void *>(m.getData(i)), GL_DYNAMIC_DRAW);

        int32_t v_size = static_cast<int32_t>(m.getVertexSize());
        if(nullptr != mImpl->mIndex) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mImpl->mIndex[i]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                static_cast<int32_t>(m.getIndexSize(i)),
                reinterpret_cast<void *>(m.getIndex(i)), GL_DYNAMIC_DRAW);
        }
        switch(m.getIndexType()) {
            case resource::Model::IndexType::UI32: {
                mImpl->mCount[i] = m.getIndexSize(i) / sizeof(uint32_t);
            } break;
            case resource::Model::IndexType::UI16: {
                mImpl->mCount[i] = m.getIndexSize(i) / sizeof(uint16_t);
            } break;
            case resource::Model::IndexType::UI8: {
                mImpl->mCount[i] = m.getIndexSize(i) / sizeof(uint8_t);
            } break;
            case resource::Model::IndexType::None: {
                mImpl->mCount[i] = static_cast<int32_t>(
                    m.getDataSize(i)) / v_size;
            } break;
        }

        for(uint32_t j = 0; j < m.getNumberAttributes(); ++j) {
            glEnableVertexAttribArray(j);
            int32_t n_components;
            uint32_t type;
            switch(m.getAttributeType(j)) {
                case resource::Model::AttributeType::F32: {
                    n_components = m.getAttributeSize(j) / sizeof(float);
                    type = GL_FLOAT;
                } break;
                case resource::Model::AttributeType::UI8: {
                    n_components = m.getAttributeSize(j) / sizeof(uint8_t);
                    type = GL_UNSIGNED_BYTE;
                } break;
            }
            glVertexAttribPointer(j, n_components, type, GL_FALSE,
                v_size, reinterpret_cast<void *>(m.getAttributeOffset(j)));
        }
        //this only changes VAO state when glVertexAttribPointer() is invoked
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);

        //this changes VAO state
        if(nullptr != mImpl->mIndex)
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    glBindVertexArray(current_vao_binding);
}

Mesh::~Mesh() {
    glDeleteBuffers(
        static_cast<int32_t>(mImpl->mPartitions), mImpl->mData.get());
    if(nullptr != mImpl->mIndex)
        glDeleteBuffers(
            static_cast<int32_t>(mImpl->mPartitions), mImpl->mIndex.get());
    glDeleteVertexArrays(
        static_cast<int32_t>(mImpl->mPartitions), mImpl->mVAO.get());
}

int32_t Mesh::draw() const {
    uint32_t current_vao_binding;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING,
        reinterpret_cast<int32_t *>(&current_vao_binding));

    for(uint32_t i = 0; i < mImpl->mPartitions; ++i) {
        glBindVertexArray(mImpl->mVAO[i]);
        if(nullptr == mImpl->mIndex)
            glDrawArrays(GL_POINTS, 0, mImpl->mCount[i]);
        else {
            uint32_t type = GL_UNSIGNED_INT;
            if(mImpl->mCount[i] < 0xFF)
                type = GL_UNSIGNED_BYTE;
            else if(mImpl->mCount[i] < 0xFFFF)
                type = GL_UNSIGNED_SHORT;
            glDrawElements(GL_TRIANGLES, mImpl->mCount[i], type, 0);
        }
    }

    glBindVertexArray(current_vao_binding);

    return 0;
}

int32_t Mesh::update(const resource::Model &m) {
    assert(m.getNumberPartitions() == mImpl->mPartitions);
    assert(nullptr != mImpl->mVAO);
    assert(nullptr != mImpl->mData);
    assert(nullptr != mImpl->mCount);
    assert(nullptr != mImpl->mIndex ||
        resource::Model::IndexType::None == m.getIndexType());

    uint32_t current_vao_binding;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING,
        reinterpret_cast<int32_t *>(&current_vao_binding));
    for(uint32_t i = 0; i < mImpl->mPartitions; ++i) {
        //tell GPU which format data is in
        glBindVertexArray(mImpl->mVAO[i]);

        //upload data to GPU
        glBindBuffer(GL_ARRAY_BUFFER, mImpl->mData[i]);

        glBufferSubData(GL_ARRAY_BUFFER, 0,
            static_cast<int32_t>(m.getDataSize(i)),
            reinterpret_cast<void *>(m.getData(i)));

        int32_t v_size = static_cast<int32_t>(m.getVertexSize());
        if(nullptr != mImpl->mIndex) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mImpl->mIndex[i]);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0,
                static_cast<int32_t>(m.getIndexSize(i)),
                reinterpret_cast<void *>(m.getIndex(i)));
        }
        switch(m.getIndexType()) {
            case resource::Model::IndexType::UI32: {
                mImpl->mCount[i] = m.getIndexSize(i) / sizeof(uint32_t);
            } break;
            case resource::Model::IndexType::UI16: {
                mImpl->mCount[i] = m.getIndexSize(i) / sizeof(uint16_t);
            } break;
            case resource::Model::IndexType::UI8: {
                mImpl->mCount[i] = m.getIndexSize(i) / sizeof(uint8_t);
            } break;
            case resource::Model::IndexType::None: {
                mImpl->mCount[i] = static_cast<int32_t>(
                    m.getDataSize(i)) / v_size;
            } break;
        }
        //this only changes VAO state when glVertexAttribPointer() is invoked
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);

        //this changes VAO state
        if(nullptr != mImpl->mIndex)
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    glBindVertexArray(current_vao_binding);

    return 0;
}

}
