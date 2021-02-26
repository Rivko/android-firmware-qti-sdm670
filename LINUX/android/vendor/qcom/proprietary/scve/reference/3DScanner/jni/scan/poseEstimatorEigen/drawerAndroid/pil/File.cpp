/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include "pil/File.hpp"

#include <cstring>
#include <fstream>

namespace pil {

struct File::Impl {
    std::string mName;
    //uses joint read/write positions i.e.
    //seekg() == seekp() && tellg() == tellp()
    std::fstream mStream;
    //use fstream's pointer as a read pointer
    int64_t mWritePosition;
};

File::File(const char *file_name) : File(file_name, false) {
}

File::File(const char *file_name, bool create_if_not_exist) : mImpl(new Impl) {
    mImpl->mName = file_name;
    mImpl->mWritePosition = 0;

    mImpl->mStream.open(mImpl->mName.c_str(), std::fstream::binary |
        std::fstream::in | std::fstream::out);

    //file doesn't exist, so create it via trunc
    if(!mImpl->mStream.is_open() && create_if_not_exist)
        mImpl->mStream.open(mImpl->mName.c_str(), std::fstream::binary |
            std::fstream::in | std::fstream::out | std::fstream::trunc);
}

File::~File() {
    if(mImpl->mStream.is_open())
        mImpl->mStream.close();
}

int32_t File::getSize() {
    if(!mImpl->mStream.is_open())
        return -1;

    auto saved_pos = mImpl->mStream.tellg();
    //go to end of file
    mImpl->mStream.seekg(0, std::fstream::end);

    //get file size
    int32_t size = static_cast<int32_t>(mImpl->mStream.tellg());

    //restore original location of read pointer
    mImpl->mStream.seekg(saved_pos);

    return size;
}

bool File::hasExtension(const char *ext) const {
    auto pos = mImpl->mName.find_last_of('.');
    if(std::string::npos == pos)
        return false;

    auto remaining_length = mImpl->mName.length() - (pos + 1);
    return 0 == mImpl->mName.compare(pos + 1, remaining_length, ext);
}

int32_t File::append(int32_t size, const uint8_t *src) {
    if(size < 0)
        return -1;

    if(nullptr == src)
        return -2;

    if(!mImpl->mStream.is_open())
        return -3;

    auto saved_read_pos = mImpl->mStream.tellg();

    //write to end of file
    mImpl->mStream.seekp(0, std::fstream::end);
    auto start_write_pos = mImpl->mStream.tellp();
    mImpl->mStream.write(reinterpret_cast<const char *>(src), size);
    auto end_write_pos = mImpl->mStream.tellp();

    //restore original location of read pointer
    mImpl->mStream.seekg(saved_read_pos);

    if(mImpl->mStream.bad())
        return -4;

    if(mImpl->mStream.fail())
        return -5;

    int32_t num_bytes_written = static_cast<int32_t>(
        end_write_pos - start_write_pos);

    return num_bytes_written;
}

int32_t File::read(int32_t size, uint8_t *dst) {
    if(size < 0)
        return -1;

    if(nullptr == dst)
        return -2;

    if(!mImpl->mStream.is_open())
        return -3;

    mImpl->mStream.read(reinterpret_cast<char *>(dst), size);

    if(mImpl->mStream.bad())
        return -4;

    if(mImpl->mStream.fail())
        return -5;

    if(mImpl->mStream.eof())
        return -6;

    return static_cast<int32_t>(mImpl->mStream.gcount());
}

int32_t File::write(int32_t size, const uint8_t *src) {
    if(size < 0)
        return -1;

    if(nullptr == src)
        return -2;

    if(!mImpl->mStream.is_open())
        return -3;

    auto saved_read_pos = mImpl->mStream.tellg();

    mImpl->mStream.seekp(mImpl->mWritePosition);
    mImpl->mStream.write(reinterpret_cast<const char *>(src), size);

    auto new_write_pos = mImpl->mStream.tellp();

    //restore original location of read pointer
    mImpl->mStream.seekg(saved_read_pos);

    if(mImpl->mStream.bad())
        return -4;

    if(mImpl->mStream.fail())
        return -5;

    int32_t num_bytes_written = static_cast<int32_t>(
        new_write_pos - mImpl->mWritePosition);
    mImpl->mWritePosition = new_write_pos;

    return num_bytes_written;
}

}
