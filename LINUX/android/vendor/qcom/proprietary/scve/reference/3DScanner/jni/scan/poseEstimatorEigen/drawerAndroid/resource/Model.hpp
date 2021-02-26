/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef RESOURCE_MODEL_HPP
#define RESOURCE_MODEL_HPP

#include <memory>
#include <tuple>
#include <vector>

namespace resource {

class Model {
public:
    enum class AttributeType : uint32_t {
        F32,
        UI8,
    };
    enum class IndexType : uint32_t {
        UI32,
        UI16,
        UI8,
        None
    };

    Model();
    Model(const uint64_t &number_elements, const std::vector<std::tuple<
        resource::Model::AttributeType, uint32_t, uintptr_t>> &attributes);
    Model(const uint64_t &size, const uint8_t *binary);
    Model(const uint64_t &size, const uint8_t *binary, bool ccw_vertex_winding);
    ~Model();

    uint32_t getAttributeOffset(uint32_t idx) const;
    uint32_t getAttributeSize(uint32_t idx) const;
    AttributeType getAttributeType(uint32_t idx) const;

    uintptr_t getData(uint32_t partition_idx) const;
    uint32_t getDataSize(uint32_t partition_idx) const;
    uintptr_t getIndex(uint32_t partition_idx) const;
    uint32_t getIndexSize(uint32_t partition_idx) const;
    IndexType getIndexType() const;

    uint32_t getNumberAttributes() const;
    uint32_t getNumberPartitions() const;
    uint32_t getVertexSize() const;

    int32_t load(const uint64_t &number_elements, const std::vector<std::tuple<
        resource::Model::AttributeType, uint32_t, uintptr_t>> &attributes);
    int32_t load(const uint64_t &size, const uint8_t *binary);
    int32_t load(const uint64_t &size, const uint8_t *binary,
        bool ccw_polygon_winding);

private:
    Model(const Model &) = delete;
    Model &operator=(const Model &) = delete;

    struct Impl;
    std::unique_ptr<Impl> mImpl;
};

}
#endif
