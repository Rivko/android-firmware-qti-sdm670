/*
 * Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (c) 2017-2018, The Linux Foundation. All rights reserved.
 *
 */

#pragma once

#include <dlfcn.h>
#include <stdint.h>
#include <cmath>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace qmmf {

namespace qmmf_alg_plugin {

/** Utils:
 *
 *  This class defines the helper utils in qmmf framework
 **/
class Utils {
 public:
  /** ThrowException
   *    @title: exception title
   *    @msg: exception message
   *
   * Throws exception
   *
   * return: void
   **/
  static void ThrowException(std::string title, std::string msg) {
    std::stringstream s;
    s << "Error: " << title << " " << msg;
    throw std::runtime_error(s.str());
  }

  /** GetDataFolder
   *
   * Returns data folder
   *
   * return: data folder
   **/
  static std::string GetDataFolder() {
#ifdef ANDROID
    return "/data/misc/qmmf/";
#else
    return QMMF_DATA_PATH;
#endif
  }

  /** GetLibFolder
   *
   * Returns libraries folder
   *
   * return: libraries folder
   **/
  static std::string GetLibFolder() {
#ifdef ANDROID_O_OR_ABOVE
    return "/vendor/lib/";
#elif defined(ANDROID)
    return "/usr/lib/";
#else
    return QMMF_DLL_PATH;
#endif
  }

  /** GetAlgLibFolder
   *
   * Returns QMMF algorithm libraries folder
   *
   * return: algorithm libraries folder
   **/
  static std::string GetAlgLibFolder() {
    return GetLibFolder() + "qmmf/alg-plugins/";
  }

  /** GetAlgTuningFolder
   *
   * Returns QMMF tuning files folder
   *
   * return: algorithm tuning files folder
   **/
  static std::string GetAlgTuningFolder() {
#ifdef ANDROID_O_OR_ABOVE
    return "/vendor/etc/qmmf/";
#elif defined(ANDROID)
    return "/system/etc/qmmf/";
#else
    return QMMF_ALG_CONFIG_DIR;
#endif
  }

  /** MakeDivisibleBy
   *    @val: value
   *    @div: divider
   *
   * Calculates closest greater value which is divisible by divider
   *
   * return: closest greater value which is divisible by divider
   **/
  static size_t MakeDivisibleBy(size_t val, uint32_t div) {
    return div > 0 ? ((val + div - 1) / div) * div : val;
  }

  /** GetWidthInBytes
   *    @val: value
   *    @pixel_size: pixel size
   *
   * Calculates width in bytes
   *
   * return: width in bytes
   **/
  static uint32_t GetWidthInBytes(uint32_t val, float pixel_size) {
    return (uint32_t)std::ceil(val * pixel_size);
  }

  /** GCD
   *    @a: value a
   *    @a: value b
   *
   * Calculates gcd of the values
   *
   * return: gcd of the values
   **/
  static uint32_t GCD(uint32_t a, uint32_t b) {
    // TODO uncomment when cpp 17 is enabled
    // return std::gcd(a, b);

    if (0 == a) {
      return b;
    }
    if (0 == b) {
      return a;
    }

    while (a != b) {
      if (a > b) {
        a -= b;
      } else {
        b -= a;
      }
    }

    return a;
  }

  /** LCM
   *    @a: value a
   *    @a: value b
   *
   * Calculates lcm of the values
   *
   * return: lcm of the values
   **/
  static uint32_t LCM(uint32_t a, uint32_t b) {
    // TODO uncomment when cpp 17 is enabled
    // return std::lcm(a, b);

    if (0 == a) {
      return b;
    }
    if (0 == b) {
      return a;
    }

    return ((uint64_t)a * b) / GCD(a, b);
  }

  /** LoadLib
   *    @lib_name: name of the library
   *    @lib_handle (output): loaded lib handle
   *
   * Loads library
   *
   * return: lib handle
   **/
  static void LoadLib(std::string lib_name, void *&lib_handle) {
    dlerror();

    lib_handle = dlopen(lib_name.c_str(), RTLD_NOW);
    const char *dlsym_error = dlerror();
    if (!lib_handle || dlsym_error) {
      ThrowException(__func__,
                     dlsym_error ? dlsym_error : "Error: dlopen failed");
    }
  }

  /** UnloadLib
   *    @lib_handle: lib handle
   *
   * Unloads library
   *
   * return: nothing
   **/
  static void UnloadLib(void *&lib_handle) {
    if (nullptr != lib_handle) {
      dlclose(lib_handle);
      lib_handle = nullptr;
    }
  }

  /** LoadLibHandler
   *    @lib_handle: lib handle
   *    @handler_name: name of the handler
   *    @handler (output): loaded handler
   *
   * Loads lib handler
   *
   * return: nothing
   **/
  template <typename THandler>
  static void LoadLibHandler(void *lib_handle, std::string handler_name,
                             THandler &handler) {
    handler = (THandler)dlsym(lib_handle, handler_name.c_str());
    const char *dlsym_error = dlerror();
    if (dlsym_error) {
      ThrowException(__func__, dlsym_error);
    }
  }

  /** DecodeBase64
   *    @input_data: input string
   *
   * Decodes binary data from string with base 64
   *
   * return: decoded data
   **/
  static std::vector<uint8_t> DecodeBase64(std::string const &input_data) {
    const std::string kCharLUT =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    std::vector<uint8_t> rc;

    std::vector<uint8_t> filtered_input_data;
    for (uint8_t c : input_data) {
      if ((c != '=') && (isalnum(c) || (c == '+') || (c == '/'))) {
        filtered_input_data.push_back(c);
      }
    }
    size_t in_len = filtered_input_data.size();
    for (uint32_t i = 0; i < 4; i++) {
      filtered_input_data.push_back(0);
    }

    auto input_data_ptr = const_cast<uint8_t *>(filtered_input_data.data());
    for (uint32_t i = 0; i < in_len; i += 4) {
      Decode(rc, input_data_ptr);
    }

    Decode(rc, input_data_ptr, in_len % 4);

    return rc;
  }

  /** EncodeBase64
   *    @input_data: input data
   *
   * Encodes binary data into string with base 64
   *
   * return: encoded string
   **/
  template <typename TInputData>
  static std::string EncodeBase64(TInputData &input_data) {
    std::string rc;

    uint32_t in_len = input_data.size();
    for (uint32_t i = in_len % 3; (0 < i) && (i < 3); i++) {
      input_data.push_back(0u);
    }

    uint8_t *input_data_ptr = reinterpret_cast<uint8_t *>(input_data.data());
    for (uint32_t i = 0; i < in_len; i += 3) {
      Encode(rc, input_data_ptr);
    }

    Encode(rc, input_data_ptr, in_len % 3);

    for (uint32_t i = in_len % 3; (0 < i) && (i < 3); i++) {
      rc += '=';
    }

    return rc;
  }

 private:
  /** Encode
   *    @target: output string
   *    @in_ptr: input data
   *    @size: number of bytes to encode
   *
   * Encodes specified number of bytes
   *
   * return: void
   **/
  static void Encode(std::string &target, uint8_t *&in_ptr, uint32_t size = 3) {
    const std::string kCharLUT =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    if (0 == size) {
      return;
    }

    target += kCharLUT[(in_ptr[0] & 0xFC) >> 2];
    target += kCharLUT[((in_ptr[0] & 0x03) << 4) | ((in_ptr[1] & 0xF0) >> 4)];

    if (1 == size) {
      return;
    }
    target += kCharLUT[((in_ptr[1] & 0x0F) << 2) | ((in_ptr[2] & 0xC0) >> 6)];

    if (2 == size) {
      return;
    }
    target += kCharLUT[in_ptr[2] & 0x3F];
    in_ptr += size;
  }

  /** Decode
   *    @target: output data
   *    @in_ptr: input data
   *    @size: number of bytes to decode
   *
   * Decodes specified number of bytes
   *
   * return: void
   **/
  static void Decode(std::vector<uint8_t> &target, uint8_t *&in_ptr,
                     uint32_t size = 4) {
    const std::string kCharLUT =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    if (2 > size) {
      return;
    }

    target.push_back((kCharLUT.find(in_ptr[0]) << 2) |
                     ((kCharLUT.find(in_ptr[1]) & 0x30) >> 4));

    if (2 == size) {
      return;
    }
    target.push_back(((kCharLUT.find(in_ptr[1]) & 0x0F) << 4) |
                     (kCharLUT.find(in_ptr[2]) & 0x3C) >> 2);

    if (3 == size) {
      return;
    }
    target.push_back(((kCharLUT.find(in_ptr[2]) & 0x03) << 6) |
                     kCharLUT.find(in_ptr[3]));
    in_ptr += size;
  }
};

};  // namespace qmmf_alg_plugin

};  // namespace qmmf
