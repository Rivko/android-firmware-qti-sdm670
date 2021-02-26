/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef KEYMASTER_UTILS_KEYMASTERBUFFER_H_
#define KEYMASTER_UTILS_KEYMASTERBUFFER_H_

#include <hardware/keymaster_defs.h>

namespace keymasterdevice {

class KeymasterBuffer {
public:
    KeymasterBuffer(uint8_t *buf, size_t bufSize): buf_(buf), bufSize_(bufSize),
                         offset_(0), bytes_remaining(bufSize), indirect_buf(NULL),
                         indirect_buf_size(0), indirect_offset(0), indirect_buf_bytes_remaining(0),
                         indirect_offset_from_main(0) {}
    virtual ~KeymasterBuffer() {}
    keymaster_error_t append_to_buf(const void* data, size_t data_len);
    size_t get_buf_size();
    size_t get_offset();
    size_t get_buf_size_remaining();
    uint8_t *get_base_addr();
    uint8_t *get_curr_addr();
    keymaster_error_t set_indirect_buf(size_t offset);
    keymaster_error_t append_to_indirect_buf(const void* data, size_t data_len);
    void reset_indirect_buf();
    size_t get_indirect_buf_offset();
    size_t get_indirect_buf_size_remaining();
    size_t get_indirect_buf_offset_from_main();
private:
    uint8_t *buf_;
    size_t bufSize_;
    size_t offset_;
    size_t bytes_remaining;
    uint8_t *indirect_buf;
    size_t indirect_buf_size;
    size_t indirect_offset;
    size_t indirect_buf_bytes_remaining;
    size_t indirect_offset_from_main;
};

}
#endif /* KEYMASTER_UTILS_KEYMASTERBUFFER_H_ */
