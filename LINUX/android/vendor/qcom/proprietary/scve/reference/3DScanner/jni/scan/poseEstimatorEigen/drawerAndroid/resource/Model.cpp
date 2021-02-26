/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "resource/Model.hpp"

#include <cassert>
#include <cstdlib>
#include <cstring>

#ifndef NDEBUG
#include <core/Log.hpp>
#endif

struct AttributeData {
    std::unique_ptr<uint8_t[]> data;
    std::unique_ptr<uint8_t[]> index;
    uint32_t dataSize;
    uint32_t indexSize;
};

struct AttributeFormat {
    uint32_t endOffset;
    resource::Model::AttributeType type;
};

static uint32_t align(uint32_t n);

namespace resource {

struct Model::Impl {
    std::unique_ptr<AttributeData[]> mPartitions;
    std::unique_ptr<AttributeFormat[]> mFormats;
    IndexType mIndexType;
    uint32_t mNumberFormats;
    uint32_t mNumberPartitions;
};

Model::Model() : mImpl(new Impl) {
}

Model::Model(const uint64_t &number_elements,
        const std::vector<
            std::tuple<resource::Model::AttributeType, uint32_t, uintptr_t>
        > &attributes) : Model() {
    load(number_elements, attributes);
}

Model::Model(const uint64_t &size,
        const uint8_t *binary) : Model(size, binary, true) {
}

Model::Model(const uint64_t &size, const uint8_t *binary,
        bool ccw_vertex_winding) : Model() {
    load(size, binary, ccw_vertex_winding);
}

Model::~Model() {
}

uint32_t Model::getAttributeOffset(uint32_t idx) const {
    assert(idx < mImpl->mNumberFormats);
    return (0 == idx) ? 0 : mImpl->mFormats[idx - 1].endOffset;
}

uint32_t Model::getAttributeSize(uint32_t idx) const {
    assert(idx < mImpl->mNumberFormats);
    return (0 == idx) ? mImpl->mFormats[0].endOffset :
        (mImpl->mFormats[idx].endOffset - mImpl->mFormats[idx - 1].endOffset);
}

Model::AttributeType Model::getAttributeType(uint32_t idx) const {
    assert(idx < mImpl->mNumberFormats);
    return mImpl->mFormats[idx].type;
}

uintptr_t Model::getData(uint32_t partition_idx) const {
    assert(partition_idx < mImpl->mNumberPartitions);
    return reinterpret_cast<uintptr_t>(
        mImpl->mPartitions[partition_idx].data.get());
}

uint32_t Model::getDataSize(uint32_t partition_idx) const {
    assert(partition_idx < mImpl->mNumberPartitions);
    return mImpl->mPartitions[partition_idx].dataSize;
}

uintptr_t Model::getIndex(uint32_t partition_idx) const {
    assert(partition_idx < mImpl->mNumberPartitions &&
        IndexType::None != getIndexType());
    return reinterpret_cast<uintptr_t>(
        mImpl->mPartitions[partition_idx].index.get());
}

uint32_t Model::getIndexSize(uint32_t partition_idx) const {
    assert(partition_idx < mImpl->mNumberPartitions &&
        IndexType::None != getIndexType());
    return mImpl->mPartitions[partition_idx].indexSize;
}

Model::IndexType Model::getIndexType() const {
    return mImpl->mIndexType;
}

uint32_t Model::getNumberAttributes() const {
    return mImpl->mNumberFormats;
}

uint32_t Model::getNumberPartitions() const {
    return mImpl->mNumberPartitions;
}

uint32_t Model::getVertexSize() const {
    assert(0 != mImpl->mNumberFormats);
    //Round an integer up to the nearest alignment boundary.
    return align(mImpl->mFormats[mImpl->mNumberFormats - 1].endOffset);
}

int32_t Model::load(const uint64_t &number_elements,
        const std::vector<std::tuple<
        resource::Model::AttributeType, uint32_t, uintptr_t>> &attributes) {
    if(0 == number_elements)
        return 1;

    if(attributes.empty())
        return 2;

    mImpl->mIndexType = IndexType::None;
    mImpl->mNumberFormats = attributes.size();
    mImpl->mFormats.reset(new AttributeFormat[mImpl->mNumberFormats]);

    std::vector<uintptr_t> addresses;
    auto offset = 0, i = 0;
    for(auto &attr : attributes) {
        mImpl->mFormats[i] = {offset + std::get<1>(attr), std::get<0>(attr)};
        offset += std::get<1>(attr);
        addresses.push_back(std::get<2>(attr));
        ++i;
    }

    mImpl->mNumberPartitions = 1;
    mImpl->mPartitions.reset(new AttributeData[mImpl->mNumberPartitions]);
    for(uint32_t p = 0; p < mImpl->mNumberPartitions; ++p) {
        mImpl->mPartitions[p].dataSize = number_elements * getVertexSize();
        mImpl->mPartitions[p].data.reset(
            new uint8_t[mImpl->mPartitions[p].dataSize]);
        uint8_t *ptr_data = reinterpret_cast<uint8_t *>(
            mImpl->mPartitions[p].data.get());

        //pack all data streams into one
        for(uint32_t ne = 0; ne < number_elements; ++ne) {
            auto padding = getVertexSize();
            for(uint32_t j = 0; j < addresses.size(); ++j) {
                auto size = getAttributeSize(j);
                memcpy(ptr_data, reinterpret_cast<void *>(addresses[j]), size);
                addresses[j] += size;
                ptr_data += size;
                padding -= size;
            }
            ptr_data += padding;
        }
    }

#ifndef NDEBUG
    core::Log log;
    log.record("Number of Attributes: %d", getNumberAttributes());
    log.record("Number of Partitions: %d", getNumberPartitions());
    log.record("Vertex Size: %d", getVertexSize());
    if(IndexType::UI32 == getIndexType())
        log.record("Index Type: %s", "Unsigned Int");
    else if(IndexType::None == getIndexType())
        log.record("Index Type: %s", "None");
    for(uint32_t i = 0; i < getNumberAttributes(); ++i) {
        log.record("Attribute %d Offset: %d", i, getAttributeOffset(i));
        log.record("Attribute %d Size: %d", i, getAttributeSize(i));
        if(AttributeType::F32 == getAttributeType(i))
            log.record("Attribute %d Type: %s", i, "Float");
    }
#endif

    return 0;
}

int32_t Model::load(const uint64_t &size, const uint8_t *binary) {
    return load(size, binary, true);
}

int32_t Model::load(const uint64_t &size, const uint8_t *binary,
        bool ccw_polygon_winding) {
    if(nullptr == binary)
        return 1;

    mImpl->mIndexType = IndexType::None;

    mImpl->mNumberFormats = 3;
    mImpl->mFormats.reset(new AttributeFormat[mImpl->mNumberFormats]);
    //positions
    auto offset = 0;
    mImpl->mFormats[0] = {4 * 3, AttributeType::F32};
    offset += 4 * 3;
    //normals
    mImpl->mFormats[1] = {(uint32_t)(offset + 4 * 3), AttributeType::F32};
    offset += 4 * 3;
    //vertex colors
    mImpl->mFormats[2] = {(uint32_t)(offset + 4 * 3), AttributeType::F32};

    const char *start = strstr(reinterpret_cast<const char *>(binary),
        "element vertex");
    //skip "element"
    while(' ' != *start++);
    //skip "vertex"
    while(' ' != *start++);
    int32_t n = 0;
    sscanf(start, "%d", &n);

    start = strstr(start, "end_header");
    //skip "end_header"
    while('\n' != *start++);

    const auto kNumberPoints = 100000;
    mImpl->mNumberPartitions = n / kNumberPoints;
    mImpl->mNumberPartitions += (0 == (n % kNumberPoints)) ? 0 : 1;
    mImpl->mPartitions.reset(new AttributeData[mImpl->mNumberPartitions]);

    for(uint32_t p = 0; p < mImpl->mNumberPartitions; ++p) {
        auto number_elements = std::min(kNumberPoints, n);
        n -= number_elements;
        mImpl->mPartitions[p].dataSize = number_elements * getVertexSize();
        mImpl->mPartitions[p].data.reset(
            new uint8_t[mImpl->mPartitions[p].dataSize]);
        float *ptr_data = reinterpret_cast<float *>(
            mImpl->mPartitions[p].data.get());
        for(uint32_t i = 0; i < number_elements; ++i) {
            char *ptr_end;
            auto x = static_cast<float>(strtod(start, &ptr_end));
            start = ptr_end;
            auto y = static_cast<float>(strtod(start, &ptr_end));
            start = ptr_end;
            auto z = static_cast<float>(strtod(start, &ptr_end));
            start = ptr_end;
            auto r = static_cast<float>(strtod(start, &ptr_end));
            start = ptr_end;
            auto g = static_cast<float>(strtod(start, &ptr_end));
            start = ptr_end;
            auto b = static_cast<float>(strtod(start, &ptr_end));
            start = ptr_end;
            auto n_x = static_cast<float>(strtod(start, &ptr_end));
            start = ptr_end;
            auto n_y = static_cast<float>(strtod(start, &ptr_end));
            start = ptr_end;
            auto n_z = static_cast<float>(strtod(start, &ptr_end));
            start = ptr_end + 1;

            *ptr_data++ = x;
            *ptr_data++ = y;
            *ptr_data++ = z;
            *ptr_data++ = n_x;
            *ptr_data++ = n_y;
            *ptr_data++ = n_z;
            *ptr_data++ = r / 255.0f;
            *ptr_data++ = g / 255.0f;
            *ptr_data++ = b / 255.0f;
        }
    }

#ifndef NDEBUG
    core::Log log;
    log.record("Number of Attributes: %d", getNumberAttributes());
    log.record("Number of Partitions: %d", getNumberPartitions());
    log.record("Vertex Size: %d", getVertexSize());
    if(IndexType::UI32 == getIndexType())
        log.record("Index Type: %s", "Unsigned Int");
    else if(IndexType::None == getIndexType())
        log.record("Index Type: %s", "None");
    for(uint32_t i = 0; i < getNumberAttributes(); ++i) {
        log.record("Attribute %d Offset: %d", i, getAttributeOffset(i));
        log.record("Attribute %d Size: %d", i, getAttributeSize(i));
        if(AttributeType::F32 == getAttributeType(i))
            log.record("Attribute %d Type: %s", i, "Float");
    }
#endif

    return 0;
}

}

static uint32_t align(uint32_t n) {
    const uint32_t alignment = 4;
    return (n + alignment - 1) - (n + alignment - 1) % alignment;
}
