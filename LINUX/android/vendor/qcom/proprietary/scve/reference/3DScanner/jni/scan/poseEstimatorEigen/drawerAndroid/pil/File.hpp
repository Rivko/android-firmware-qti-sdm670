/*
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef PIL_FILE_HPP
#define PIL_FILE_HPP

#include <memory>

namespace pil {

class File {
public:
    File(const char *file_name);
    File(const char *file_name, bool create_if_not_exist);
    ~File();

    int32_t getSize();
    bool hasExtension(const char *ext) const;

    int32_t append(int32_t size, const uint8_t *src);
    int32_t read(int32_t size, uint8_t *dst);
    int32_t write(int32_t size, const uint8_t *src);

private:
    File() = delete;
    File(const File &) = delete;
    File &operator=(const File &) = delete;

    struct Impl;
    std::unique_ptr<Impl> mImpl;
};

}
#endif
