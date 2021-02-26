/*
 * Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Copyright (c) 2017-2018, The Linux Foundation. All rights reserved.
 *
 */

#pragma once

#include <stdint.h>
#include <list>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "qmmf_alg_utils.h"

namespace qmmf {

namespace qmmf_alg_plugin {

/** PixelFormat:
 *    @kRawBggrMipi8:  Bayer raw "|B|Gb|Gr|R|" MIPI8 packed
 *    @kRawGbrgMipi8:  Bayer raw "|Gb|B|R|Gr|" MIPI8 packed
 *    @kRawGrbgMipi8:  Bayer raw "|Gr|R|Gb|B|" MIPI8 packed
 *    @kRawRggbMipi8:  Bayer raw "|R|Gr|Gb|B|" MIPI8 packed
 *
 *    @kRawBggrMipi10: Bayer raw "|B|Gb|Gr|R|" MIPI10 packed
 *    @kRawGbrgMipi10: Bayer raw "|Gb|B|R|Gr|" MIPI10 packed
 *    @kRawGrbgMipi10: Bayer raw "|Gr|R|Gb|B|" MIPI10 packed
 *    @kRawRggbMipi10: Bayer raw "|R|Gr|Gb|B|" MIPI10 packed
 *
 *    @kRawBggrMipi12: Bayer raw "|B|Gb|Gr|R|" MIPI12 packed
 *    @kRawGbrgMipi12: Bayer raw "|Gb|B|R|Gr|" MIPI12 packed
 *    @kRawGrbgMipi12: Bayer raw "|Gr|R|Gb|B|" MIPI12 packed
 *    @kRawRggbMipi12: Bayer raw "|R|Gr|Gb|B|" MIPI12 packed
 *
 *    @kRawBggr10:     Bayer raw "|B|Gb|Gr|R|" 2 bytes per pixel unpacked
 *    @kRawGbrg10:     Bayer raw "|Gb|B|R|Gr|" 2 bytes per pixel unpacked
 *    @kRawGrbg10:     Bayer raw "|Gr|R|Gb|B|" 2 bytes per pixel unpacked
 *    @kRawRggb10:     Bayer raw "|R|Gr|Gb|B|" 2 bytes per pixel unpacked
 *
 *    @kRawBggr12:     Bayer raw "|B|Gb|Gr|R|" 2 bytes per pixel unpacked
 *    @kRawGbrg12:     Bayer raw "|Gb|B|R|Gr|" 2 bytes per pixel unpacked
 *    @kRawGrbg12:     Bayer raw "|Gr|R|Gb|B|" 2 bytes per pixel unpacked
 *    @kRawRggb12:     Bayer raw "|R|Gr|Gb|B|" 2 bytes per pixel unpacked
 *
 *    @kRawBggr16:     Bayer raw "|B|Gb|Gr|R|" 2 bytes per pixel unpacked
 *    @kRawGbrg16:     Bayer raw "|Gb|B|R|Gr|" 2 bytes per pixel unpacked
 *    @kRawGrbg16:     Bayer raw "|Gr|R|Gb|B|" 2 bytes per pixel unpacked
 *    @kRawRggb16:     Bayer raw "|R|Gr|Gb|B|" 2 bytes per pixel unpacked
 *
 *    @kYuyv422i:      YUV 4:2:2 "|Y|U|Y|V|" yuv interleaved
 *    @kYvyu422i:      YUV 4:2:2 "|Y|V|Y|U|" yuv interleaved
 *    @kUyvy422i:      YUV 4:2:2 "|U|Y|V|Y|" yuv interleaved
 *    @kVyuy422i:      YUV 4:2:2 "|V|Y|U|V|" yuv interleaved
 *
 *    @kNv12:          NV12 semiplanar: Y/CbCr 4:2:0
 *    @kNv12UBWC:      NV12 semiplanar: Y/CbCr 4:2:0 UBWC
 *    @kNv21:          NV21 semiplanar: Y/CrCb 4:2:0
 *    @kNv21UBWC:      NV21 semiplanar: Y/CrCb 4:2:0 UBWC
 *    @kNv16:          NV16 semiplanar: Y/CbCr 4:2:2
 *    @kNv61:          NV61 semiplanar: Y/CrCb 4:2:2
 *
 *    @kYuv420:        YUV 4:2:0
 *    @kYvu420:        YVU 4:2:0
 *    @kYuv420p:       YUV 4:2:0 Planar
 *    @kYvu420p:       YVU 4:2:0 Planar
 *
 *    @kRgb444:        RGB 16bit  xxxxrrrr ggggbbbb
 *    @kArgb444:       RGB 16bit  aaaarrrr ggggbbbb
 *    @kXrgb444:       RGB 16bit  xxxxrrrr ggggbbbb
 *    @kRgb555:        RGB 16bit  RGB-5-5-5
 *    @kArgb555:       RGB 16bit  ARGB-1-5-5-5
 *    @kXrgb555:       RGB 16bit  XRGB-1-5-5-5
 *    @kRgb565:        RGB 16bit  RGB-5-6-5
 *    @kRgb555x:       RGB 16bit  RGB-5-5-5 BE
 *    @kArgb555x:      RGB 16bit  ARGB-5-5-5 BE
 *    @kXrgb555x:      RGB 16bit  XRGB-5-5-5 BE
 *    @kRgb565x:       RGB 16bit  RGB-5-6-5 BE
 *    @kBgr666:        RGB 18bit  BGR-6-6-6
 *    @kBgr24:         RGB 24bit  BGR-8-8-8
 *    @kRgb24:         RGB 24bit  RGB-8-8-8
 *    @kBgr32:         RGB 32bit  BGR-8-8-8-8
 *    @kAbgr32:        RGB 32bit  BGRA-8-8-8-8
 *    @kXbgr32:        RGB 32bit  BGRX-8-8-8-8
 *    @kRgb32:         RGB 32bit  RGB-8-8-8-8
 *    @kArgb32:        RGB 32bit  ARGB-8-8-8-8
 *    @kXrgb32:        RGB 32bit  XRGB-8-8-8-8
 *    @kBgrFloat:      RGB 128bit BGR-32-32-32-32
 *    @kRgbFloat:      RGB 128bit RGB-32-32-32-32
 *
 *    @kJpeg:          JFIF JPEG
 *    @kGrey:          Greyscale
 *
 *    @kMeshNormFloat: Mesh normalized [0.0f; 1.0f]
 *
 *  This enum defines the supported pixel formats
 */
enum PixelFormat {

  kRawBggrMipi8 = 0x100,
  kRawGbrgMipi8 = 0x101,
  kRawGrbgMipi8 = 0x102,
  kRawRggbMipi8 = 0x103,

  kRawBggrMipi10 = 0x200,
  kRawGbrgMipi10 = 0x201,
  kRawGrbgMipi10 = 0x202,
  kRawRggbMipi10 = 0x203,

  kRawBggrMipi12 = 0x300,
  kRawGbrgMipi12 = 0x301,
  kRawGrbgMipi12 = 0x302,
  kRawRggbMipi12 = 0x303,

  kRawBggr10 = 0x400,
  kRawGbrg10 = 0x401,
  kRawGrbg10 = 0x402,
  kRawRggb10 = 0x403,

  kRawBggr12 = 0x500,
  kRawGbrg12 = 0x501,
  kRawGrbg12 = 0x502,
  kRawRggb12 = 0x503,

  kRawBggr16 = 0x600,
  kRawGbrg16 = 0x601,
  kRawGrbg16 = 0x602,
  kRawRggb16 = 0x603,

  kYuyv422i = 0x700,
  kYvyu422i = 0x701,
  kUyvy422i = 0x702,
  kVyuy422i = 0x703,

  kNv12 = 0x800,
  kNv12UBWC = 0x801,
  kNv21 = 0x802,
  kNv21UBWC = 0x804,
  kNv16 = 0x805,
  kNv61 = 0x806,

  kYuv420 = 0x900,
  kYvu420 = 0x901,
  kYuv420p = 0x902,
  kYvu420p = 0x904,

  kRgb444 = 0xA00,
  kArgb444 = 0xA01,
  kXrgb444 = 0xA02,
  kRgb555 = 0xA03,
  kArgb555 = 0xA04,
  kXrgb555 = 0xA05,
  kRgb565 = 0xA06,
  kRgb555x = 0xA07,
  kArgb555x = 0xA08,
  kXrgb555x = 0xA09,
  kRgb565x = 0xA0A,
  kBgr666 = 0xA0B,
  kBgr24 = 0xA0C,
  kRgb24 = 0xA0D,
  kBgr32 = 0xA0E,
  kAbgr32 = 0xA0F,
  kXbgr32 = 0xA10,
  kRgb32 = 0xA11,
  kArgb32 = 0xA12,
  kXrgb32 = 0xA13,
  kBgrFloat = 0xA14,
  kRgbFloat = 0xA15,

  kJpeg = 0xB00,
  kGrey = 0xB01,

  kMeshNormFloat = 0xC00,
};

/** Requirements:
 *    @width_: width
 *    @height_: height
 *    @stride_: stride
 *    @scanline_: scanline
 *    @formats_: pixel format
 *
 *  This class defines node input and output requirements
 **/
struct Requirements {
  uint32_t width_;
  uint32_t height_;
  uint32_t stride_;
  uint32_t scanline_;
  std::vector<PixelFormat> formats_;
};

/** RuntimeError:
 *    @kFatal:              Fatal error
 *    @kFrameDrop:          Frame not processed
 *    @kResourcePreempted:  Resource preempted
 *
 *  This enum defines the supported pixel formats
 */
enum RuntimeError { kFatal, kFrameDrop, kResourcePreempted };

/** BufferRequirements:
 *    @min_width_: Minimum supported width
 *    @min_height_: Minimum supported height
 *    @max_width_: Maximum supported width
 *    @max_height_: Maximum supported height
 *    @width_alignment_: Required width alignment in pixels
 *    @height_alignment_: Required height alignment in lines
 *    @max_height_: Maximum supported height
 *    @cached_: Boolean flag indicating whether buffers needs to be cached
 *    @count_: Number of required buffers
 *    @history_buffer_count_: history buffer count
 *    @stride_alignment_: Required stride alignment in bytes
 *    @plane_alignment_: Required plane alignment in bytes
 *    @pixel_formats_: Pixel format
 *
 *  This class defines the buffer requirements
 **/
class BufferRequirements {
 public:
  BufferRequirements(){};

  BufferRequirements(const BufferRequirements &buffer_requirements)
      : min_width_(buffer_requirements.min_width_),
        min_height_(buffer_requirements.min_height_),
        max_width_(buffer_requirements.max_width_),
        max_height_(buffer_requirements.max_height_),
        width_alignment_(buffer_requirements.width_alignment_),
        height_alignment_(buffer_requirements.height_alignment_),
        cached_(buffer_requirements.cached_),
        count_(buffer_requirements.count_),
        history_buffer_count_(buffer_requirements.history_buffer_count_),
        stride_alignment_(buffer_requirements.stride_alignment_),
        plane_alignment_(buffer_requirements.plane_alignment_),
        pixel_formats_(buffer_requirements.pixel_formats_){};

  BufferRequirements(uint32_t min_width, uint32_t min_height,
                     uint32_t max_width, uint32_t max_height,
                     uint32_t width_alignment, uint32_t height_alignment,
                     bool cached, uint32_t count, uint32_t history_buffer_count,
                     uint32_t stride_alignment, uint32_t plane_alignment,
                     std::set<PixelFormat> supported_pixel_formats)
      : min_width_(min_width),
        min_height_(min_height),
        max_width_(max_width),
        max_height_(max_height),
        width_alignment_(width_alignment),
        height_alignment_(height_alignment),
        cached_(cached),
        count_(count),
        history_buffer_count_(history_buffer_count),
        stride_alignment_(stride_alignment),
        plane_alignment_(plane_alignment),
        pixel_formats_(supported_pixel_formats){};

  std::string ToString(uint32_t indent = 0) const {
    std::stringstream indentation;
    for (uint32_t i = 0; i < indent; i++) indentation << '\t';
    indent++;

    std::stringstream stream;
    stream << indentation.str() << "\"min_width_\" : " << min_width_ << ","
           << '\n';
    stream << indentation.str() << "\"min_height_\" : " << min_height_ << ","
           << '\n';
    stream << indentation.str() << "\"max_width_\" : " << max_width_ << ","
           << '\n';
    stream << indentation.str() << "\"max_height_\" : " << max_height_ << ","
           << '\n';
    stream << indentation.str() << "\"width_alignment_\" : " << width_alignment_
           << "," << '\n';
    stream << indentation.str()
           << "\"height_alignment_\" : " << height_alignment_ << "," << '\n';
    stream << indentation.str() << "\"cached_\" : " << cached_ << "," << '\n';
    stream << indentation.str() << "\"count_\" : " << count_ << "," << '\n';
    stream << indentation.str() << "\"history_buffer_count_\" : " << count_
           << "," << '\n';
    stream << indentation.str()
           << "\"stride_alignment_\" : " << stride_alignment_ << "," << '\n';
    stream << indentation.str() << "\"plane_alignment_\" : " << plane_alignment_
           << '\n';
    stream << indentation.str() << "\"pixel_formats_\" : [";
    for (auto pf : pixel_formats_) stream << " \"" << pf << "\",";
    if (pixel_formats_.size() > 0) stream << "\b";
    stream << " ]\n";

    return stream.str();
  }

  uint32_t min_width_;
  uint32_t min_height_;
  uint32_t max_width_;
  uint32_t max_height_;
  uint32_t width_alignment_;
  uint32_t height_alignment_;
  bool cached_;
  uint32_t count_;
  uint32_t history_buffer_count_;
  uint32_t stride_alignment_;
  uint32_t plane_alignment_;
  std::set<PixelFormat> pixel_formats_;
};

/** BufferPlane:
 *    @width: Width in pixels
 *    @height: Height in pixels
 *    @stride_: Stride in bytes
 *    @offset_: Offset to valid plane data in bytes
 *    @length_: Plane length in bytes
 *
 *  This class defines plane representation in memory
 **/
class BufferPlane {
 public:
  BufferPlane(uint32_t width, uint32_t height, uint32_t stride, uint32_t offset,
              uint32_t length)
      : width_(width),
        height_(height),
        stride_(stride),
        offset_(offset),
        length_(length) {
    Validate();
  };

  BufferPlane(const BufferPlane &plane)
      : width_(plane.width_),
        height_(plane.height_),
        stride_(plane.stride_),
        offset_(plane.offset_),
        length_(plane.length_) {
    Validate();
  };

  std::string ToString(uint32_t indent = 0) const {
    std::stringstream indentation;
    for (uint32_t i = 0; i < indent; i++) {
      indentation << '\t';
    }
    indent++;

    std::stringstream stream;
    stream << indentation.str() << "\"width_\" : " << width_ << "," << '\n';
    stream << indentation.str() << "\"height_\" : " << height_ << "," << '\n';
    stream << indentation.str() << "\"stride_\" : " << stride_ << "," << '\n';
    stream << indentation.str() << "\"offset_\" : " << offset_ << "," << '\n';
    stream << indentation.str() << "\"length_\" : " << length_ << '\n';
    return stream.str();
  }

  /** Validate:
   *
   *  This method validates current data in the class
   **/
  void Validate() const {
    if (0 == width_) {
      Utils::ThrowException(__func__, "width_ is 0");
    }
    if (0 == height_) {
      Utils::ThrowException(__func__, "height_ is 0");
    }
    if (0 == stride_) {
      Utils::ThrowException(__func__, "stride_ is 0");
    }
    if (0 == length_) {
      Utils::ThrowException(__func__, "length_ is 0");
    }
    if (height_ * stride_ > length_) {
      Utils::ThrowException(__func__, "plane size is too small");
    }
  }

  /** Validate:
   *    @requirements: Buffer requirements
   *
   *  This method validates current data in the class based on buffer
   *  requirements
   **/
  void Validate(const BufferRequirements &requirements) const {
    Validate();

    if (width_ > requirements.max_width_) {
      std::stringstream s;
      s << "Plane width " << width_ << " greater than max width "
        << requirements.max_width_;
      Utils::ThrowException(__func__, s.str());
    } else if (width_ < requirements.min_width_) {
      std::stringstream s;
      s << "Plane width " << width_ << " greater than min width "
        << requirements.min_width_;
      Utils::ThrowException(__func__, s.str());
    } else if (height_ > requirements.max_height_) {
      std::stringstream s;
      s << "Plane height " << height_ << " greater than max height "
        << requirements.max_height_;
      Utils::ThrowException(__func__, s.str());
    } else if (height_ < requirements.min_height_) {
      std::stringstream s;
      s << "Plane height " << height_ << " greater than min height "
        << requirements.min_height_;
      Utils::ThrowException(__func__, s.str());
    } else if ((requirements.width_alignment_ > 0) &&
               (width_ % requirements.width_alignment_ != 0)) {
      std::stringstream s;
      s << "Plane width " << width_ << " required alignment "
        << requirements.width_alignment_ << " is not met";
      Utils::ThrowException(__func__, s.str());
    } else if ((requirements.height_alignment_ > 0) &&
               (height_ % requirements.height_alignment_ != 0)) {
      std::stringstream s;
      s << "Plane height " << height_ << " required alignment "
        << requirements.height_alignment_ << " is not met";
      Utils::ThrowException(__func__, s.str());
    }

    if ((requirements.stride_alignment_ > 0) &&
        (stride_ % requirements.stride_alignment_ != 0)) {
      Utils::ThrowException(__func__, "Stride alignment not met");
    }
  }

  uint32_t width_;
  uint32_t height_;
  uint32_t stride_;
  uint32_t offset_;
  uint32_t length_;
};

/** AlgBuffer:
 *    @vaddr_: Buffer virtual address.
 *    @fd_: buffer fd (Platform specific can be ion or dmabuf)
 *    @size_: Buffer size in bytes.
 *    @cached_: buffer is cached
 *    @pix_fmt_: Pixel format
 *    @timestamp_: frame time stamp
 *    @frame_number_: frame number
 *    @plane_: List of planes. Plane information depends on
 *      pixel format.
 *
 *  This class defines the image buffer
 **/
class AlgBuffer {
 public:
  AlgBuffer(){};

  AlgBuffer(uint8_t *vaddr, int32_t fd, uint32_t size, bool cached,
            PixelFormat pix_fmt, int64_t timestamp, uint32_t frame_number,
            const std::vector<BufferPlane> &plane)
      : vaddr_(vaddr),
        fd_(fd),
        size_(size),
        cached_(cached),
        pix_fmt_(pix_fmt),
        timestamp_(timestamp),
        frame_number_(frame_number),
        plane_(plane) {
    Validate();
  };

  AlgBuffer(const AlgBuffer &buffer)
      : vaddr_(buffer.vaddr_),
        fd_(buffer.fd_),
        size_(buffer.size_),
        cached_(buffer.cached_),
        pix_fmt_(buffer.pix_fmt_),
        timestamp_(buffer.timestamp_),
        frame_number_(buffer.frame_number_),
        plane_(buffer.plane_) {
    Validate();
  };

  std::string ToString(uint32_t indent = 0) const {
    std::stringstream indentation;
    for (uint32_t i = 0; i < indent; i++) {
      indentation << '\t';
    }
    indent++;

    std::stringstream stream;
    if (nullptr == vaddr_)
      stream << indentation.str() << "\"vaddr_\" : nullptr,'\n'";
    else
      stream << indentation.str() << "\"vaddr_\" : " << vaddr_ << "," << '\n';
    stream << indentation.str() << "\"fd_\" : " << fd_ << "," << '\n';
    stream << indentation.str() << "\"size_\" : " << size_ << "," << '\n';
    stream << indentation.str() << "\"cached_\" : " << cached_ << "," << '\n';
    stream << indentation.str() << "\"pix_fmt_\" : " << pix_fmt_ << "," << '\n';
    stream << indentation.str() << "\"plane_\" : [" << '\n';
    for (BufferPlane p : plane_)
      stream << "{" << '\n' << p.ToString(indent) << "}," << '\n';
    stream << "\b]" << '\n';

    return stream.str();
  }

  /** Validate:
   *
   *  This method validates current data in the class
   **/
  void Validate() const {
    if (0 == size_) {
      Utils::ThrowException(__func__, "size_ is 0");
    }

    uint32_t total_size = 0;
    for (BufferPlane p : plane_) {
      p.Validate();
      if (total_size > p.offset_) {
        Utils::ThrowException(__func__, "plane offset is not enough");
      }
      total_size += p.length_;
    }

    if (total_size > size_) {
      Utils::ThrowException(__func__,
                            "size_ is different that total plane size");
    }
  }

  /** Validate:
   *    @requirements: Buffer requirements
   *
   *  This method validates current data in the class based on buffer
   *  requirements
   **/
  void Validate(const BufferRequirements &requirements) const {
    Validate();

    if (requirements.cached_ && !cached_) {
      Utils::ThrowException(__func__, "Buffer is not cached");
    }

    if (requirements.pixel_formats_.count(pix_fmt_) == 0) {
      Utils::ThrowException(__func__, "Buffer format not supported!");
    }

    for (BufferPlane p : plane_) {
      p.Validate(requirements);
      uint8_t *ptr = vaddr_ + p.offset_;
      if ((requirements.stride_alignment_ > 0) &&
          ((size_t)ptr % requirements.stride_alignment_ != 0)) {
        Utils::ThrowException(__func__, "Stride alignment not met");
      }
      if ((requirements.plane_alignment_ > 0) &&
          ((size_t)ptr % requirements.plane_alignment_ != 0)) {
        Utils::ThrowException(__func__, "Plane alignment not met");
      }
    }
  }

  /** GetDimensions:
   *
   *  Return buffer dimensions
   **/
  void GetDimensions(uint32_t &width, uint32_t &height) const {
    width = plane_.front().width_;
    height = plane_.front().height_;
  }

  /** GetVAddr:
   *    @plane: requested plane index
   *
   *  Return virtual address by plane
   **/
  uint8_t *GetVirtualAddress(uint32_t plane = 0) const {
    if (vaddr_ == nullptr) {
      return nullptr;
    }

    return vaddr_ + plane_[plane].offset_;
  }

  /** GetStride:
   *    @plane: requested plane index
   *
   *  Return stride by plane
   **/
  uint32_t GetStride(uint32_t plane = 0) const { return plane_[plane].stride_; }

  /** GetLength:
   *
   *  Return length by plane
   **/
  uint32_t GetLength(uint32_t plane = 0) const { return plane_[plane].length_; }

  uint8_t *vaddr_;
  int32_t fd_;
  uint32_t size_;
  bool cached_;
  PixelFormat pix_fmt_;
  int64_t timestamp_;
  uint32_t frame_number_;
  std::vector<BufferPlane> plane_;
};

};  // namespace qmmf_alg_plugin

};  // namespace qmmf
