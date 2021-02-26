/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "resource/Image.hpp"

#include <cassert>
#include <cstring>
#include <unordered_map>
#include <vector>
#include <string>

#ifndef NDEBUG
#include <core/Log.hpp>
#endif

struct KTXHeader {
    uint8_t id[12];
    uint32_t endianness;
    uint32_t type;
    uint32_t typeSize;
    uint32_t format;
    uint32_t internalFormat;
    uint32_t baseInternalFormat;
    uint32_t pixelWidth;
    uint32_t pixelHeight;
    uint32_t pixelDepth;
    uint32_t numberOfArrayElements;
    uint32_t numberOfFaces;
    uint32_t numberOfMipmapLevels;
    uint32_t bytesOfKeyValueData;
};

struct MipmapLevel {
    std::unique_ptr<uint8_t[]> binary;
    uint32_t size;
    uint8_t unused_padding[4];
};

using Metadata = std::unordered_map<std::string, std::string>;

static void extractData(const uint8_t *binary, bool is_big_endian,
        bool is_compressed, uint32_t type_size, uint32_t max_mipmap_level,
        uint32_t number_faces,
        std::vector<std::unique_ptr<MipmapLevel>> &data);

static void extractMetadata(const uint8_t *binary, uint32_t bytesOfKeyValueData,
        bool is_big_endian, Metadata &metadata);

static void swab(uint32_t count, uint16_t *data);
static void swab(uint32_t count, uint32_t *data);

namespace resource {

struct Image::Impl {
    Metadata mMetadata;
    std::vector<std::unique_ptr<MipmapLevel>> mData;
    uint32_t mBaseInternalFormat;
    uint32_t mDepth;
    uint32_t mHeight;
    uint32_t mInternalFormat;
    uint32_t mNumberArrayElements;
    uint32_t mNumberFaces;
    uint32_t mNumberMipmapLevels;
    uint32_t mPixelFormat;
    uint32_t mType;
    uint32_t mWidth;
};

Image::Image() : mImpl(new Impl) {
    mImpl->mBaseInternalFormat = 0;
    mImpl->mDepth = 0;
    mImpl->mHeight = 0;
    mImpl->mInternalFormat = 0;
    mImpl->mNumberArrayElements = 0;
    mImpl->mNumberFaces = 0;
    mImpl->mNumberMipmapLevels = 0;
    mImpl->mPixelFormat = 0;
    mImpl->mType = 0;
    mImpl->mWidth = 0;
}

Image::~Image() {
}

uint32_t Image::getBaseInternalFormat() const {
    return mImpl->mBaseInternalFormat;
}

uintptr_t Image::getData(uint32_t mipmap_level) const {
    assert(mipmap_level < mImpl->mData.size());
    return reinterpret_cast<uintptr_t>(
        mImpl->mData[mipmap_level]->binary.get());
}

uintptr_t Image::getData(uint32_t mipmap_level, uint32_t cubemap_face) const {
    assert(mipmap_level < mImpl->mData.size() &&
        cubemap_face < mImpl->mNumberFaces);
    auto &mml = mImpl->mData[mipmap_level];
    const uint32_t alignment = 4;
    //this rounding to nearest alignment only works with 4-byte alignments
    uint32_t image_size_rounded =
        (mml->size + alignment - 1) & ~(alignment - 1);
    return reinterpret_cast<uintptr_t>(mml->binary.get() +
        image_size_rounded * cubemap_face);
}

uint32_t Image::getDepth(uint32_t mipmap_level) const {
    assert(mipmap_level < mImpl->mData.size());
    return mImpl->mDepth >> mipmap_level;
}

uint32_t Image::getHeight(uint32_t mipmap_level) const {
    assert(mipmap_level < mImpl->mData.size());
    return mImpl->mHeight >> mipmap_level;
}

uint32_t Image::getInternalFormat() const {
    return mImpl->mInternalFormat;
}

const char *Image::getMetadata(const char *key) const {
    if(nullptr == key)
        return nullptr;

    auto it = mImpl->mMetadata.find(key);
    if(mImpl->mMetadata.end() == it)
        return nullptr;

    return it->first.c_str();
}

uint32_t Image::getNumberArrayElements() const {
    return mImpl->mNumberArrayElements;
}

uint32_t Image::getNumberFaces() const {
    return mImpl->mNumberFaces;
}

uint32_t Image::getNumberMipmapLevels() const {
    return mImpl->mNumberMipmapLevels;
}

uint32_t Image::getPixelFormat() const {
    return mImpl->mPixelFormat;
}

uint32_t Image::getSize(uint32_t mipmap_level) const {
    assert(mipmap_level < mImpl->mData.size());
    return mImpl->mData[mipmap_level]->size;
}

uint32_t Image::getType() const {
    return mImpl->mType;
}

uint32_t Image::getWidth(uint32_t mipmap_level) const {
    assert(mipmap_level < mImpl->mData.size());
    return mImpl->mWidth >> mipmap_level;
}

int32_t Image::load(const uint8_t *ktx_binary) {
    const uint8_t kFileID[12] = {
        0xAB, 'K', 'T', 'X', ' ', '1', '1', 0xBB, '\r', '\n', '\x1A', '\n'
    };
    //check if file is KTX
    if(memcmp(ktx_binary, kFileID, sizeof(kFileID)))
        return 1;

    KTXHeader header;
    memcpy(&header, ktx_binary, sizeof(header));

    bool is_big_endian = 0x04030201 == header.endianness;
    if(!is_big_endian)
        swab(12, &header.type);
    assert(1 == header.typeSize ||
        2 == header.typeSize || 4 == header.typeSize);

    ktx_binary += sizeof(KTXHeader);
    extractMetadata(ktx_binary, header.bytesOfKeyValueData,
        is_big_endian, mImpl->mMetadata);

    //every texture has at least level 0
    uint32_t max_mipmap_level = header.numberOfMipmapLevels;
    if(0 == max_mipmap_level)
        max_mipmap_level = 1;

    ktx_binary += header.bytesOfKeyValueData;
    extractData(ktx_binary, is_big_endian,
        0 == header.type && 0 == header.format, header.typeSize,
        max_mipmap_level, header.numberOfFaces, mImpl->mData);

    mImpl->mType = header.type;
    mImpl->mPixelFormat = header.format;
    mImpl->mInternalFormat = header.internalFormat;
    mImpl->mBaseInternalFormat = header.baseInternalFormat;
    mImpl->mWidth = header.pixelWidth;
    mImpl->mHeight = header.pixelHeight;
    mImpl->mDepth = header.pixelDepth;
    mImpl->mNumberArrayElements = header.numberOfArrayElements;
    mImpl->mNumberFaces = header.numberOfFaces;
    mImpl->mNumberMipmapLevels = header.numberOfMipmapLevels;

#ifndef NDEBUG
    core::Log logger;
    logger.record("ID Matches: %s", header.id);
    logger.record("%s Endian: 0x%X", (is_big_endian) ? "Big" : "Small",
        header.endianness);
    logger.record("glType: 0x%X", getType());
    logger.record("glTypeSize: %d", header.typeSize);
    logger.record("glFormat: 0x%X", getPixelFormat());
    logger.record("glInternalFormat: 0x%X", getInternalFormat());
    logger.record("glBaseInternalFormat: 0x%X", getBaseInternalFormat());
    logger.record("Pixel Width: %d", getWidth(0));
    logger.record("Pixel Height: %d", getHeight(0));
    logger.record("Pixel Depth: %d", getDepth(0));
    logger.record("# Array Elements: %d", getNumberArrayElements());
    logger.record("# Faces: %d", getNumberFaces());
    logger.record("# Mipmap Levels: %d", getNumberMipmapLevels());
    logger.record("Bytes of Key/Value Data: %d", header.bytesOfKeyValueData);
    for(auto kv : mImpl->mMetadata)
        logger.record("(Key = \"%s\" : Value = \"%s\")",
            kv.first.c_str(), kv.second.c_str());
    for(uint32_t level = 0; level < mImpl->mData.size(); ++level)
        logger.record("Mipmap Level %d: (0x%lX, %d)", level,
            getData(level), getSize(level));
#endif
    return 0;
}

}

static void extractData(const uint8_t *binary, bool is_big_endian,
        bool is_compressed, uint32_t type_size, uint32_t max_mipmap_level,
        uint32_t number_faces,
        std::vector<std::unique_ptr<MipmapLevel>> &data) {
    auto ptr_binary = binary;
    for(uint32_t level = 0; level < max_mipmap_level; ++level) {
        //determine size of mipmap level
        std::unique_ptr<MipmapLevel> mml(new MipmapLevel);
        memcpy(&mml->size, ptr_binary, sizeof(mml->size));
        if(!is_big_endian)
            swab(1, &mml->size);
        ptr_binary += sizeof(mml->size);

        //cubePadding and mipPadding ensures 4 byte alignment
        const uint32_t alignment = 4;
        //this rounding technique only works with power-of-two alignment
        uint32_t image_size_rounded = mml->size;
        if(!is_compressed)
            image_size_rounded = (mml->size + alignment - 1) & ~(alignment - 1);
        mml->binary.reset(new uint8_t[image_size_rounded * number_faces]);

        //copy over each face and convert to correct endian format
        auto ptr_face = mml->binary.get();
        for(uint32_t face = 0; face < number_faces; ++face) {
            memcpy(ptr_face, ptr_binary, image_size_rounded);
            if(!is_big_endian) {
                if(2 == type_size)
                    swab(mml->size >> 1,
                        reinterpret_cast<uint16_t *>(ptr_face));
                else if(4 == type_size)
                    swab(mml->size >> 2,
                        reinterpret_cast<uint32_t *>(ptr_face));
            }

            ptr_face += image_size_rounded;
            ptr_binary += image_size_rounded;
        }

        data.push_back(std::move(mml));
    }
}

static void extractMetadata(const uint8_t *binary, uint32_t bytesOfKeyValueData,
        bool is_big_endian, Metadata &metadata) {
    auto cur_ptr = binary;
    while(bytesOfKeyValueData > 0) {
        uint32_t keyAndValueByteSize;
        memcpy(&keyAndValueByteSize, cur_ptr, sizeof(keyAndValueByteSize));
        if(!is_big_endian)
            swab(1, &keyAndValueByteSize);

        cur_ptr += sizeof(keyAndValueByteSize);
        bytesOfKeyValueData -= sizeof(keyAndValueByteSize);

        //contains two null-terminated strings for "keyAndValue"
        std::string key = reinterpret_cast<const char *>(cur_ptr);
        std::string value = reinterpret_cast<const char *>(cur_ptr) +
            key.length() + 1;
        metadata[key] = value;

        //valuePadding ensures 4 byte alignment
        const uint32_t alignment = 4;
        //this rounding technique only works with power-of-two alignment
        uint32_t keyAndValueByteSizeRounded =
            (keyAndValueByteSize + alignment - 1) & ~(alignment - 1);
        cur_ptr += keyAndValueByteSizeRounded;
        bytesOfKeyValueData -= keyAndValueByteSizeRounded;
    }
}

static void swab(uint32_t count, uint16_t *data) {
    for(uint32_t i = 0; i < count; ++i) {
        auto x = *data;
        *data++ = static_cast<uint16_t>((x << 8) | (x >> 8));
    }
}

static void swab(uint32_t count, uint32_t *data) {
    for(uint32_t i = 0; i < count; ++i) {
        auto x = *data;
        *data++ = (x << 24) | ((x & 0xFF00) << 8) |
            ((x & 0xFF0000) >> 8) | (x >> 24);
    }
}
