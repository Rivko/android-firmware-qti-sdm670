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
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "qmmf_alg_types.h"

#define QMMF_ALG_LIB_LOAD_FUNC "QmmfAlgoNew"
#define QMMF_ALG_GET_CAPS_FUNC "QmmfAlgoGetCaps"

namespace qmmf {

namespace qmmf_alg_plugin {

/** Capabilities:
 *    @input_buffer_requirements_: input buffer requirements
 *    @output_buffer_requirements_: output buffer requirements
 *    @plugin_name_ : plugin full name
 *    @inplace_processing_: inplace processing is required
 *    @runtime_enable_disable_: flag indicating whether runtime enable/disable
 *                              is supported
 *    @crop_support_: image crop capability flag
 *    @scale_support_: image scale capability flag
 *    @lib_version_: library version
 *    @git_change_id_: library generation git change-id
 *
 *  This class defines the qmmf algorithm capabilities
 **/
class Capabilities {
 public:
  Capabilities(){};

  Capabilities(std::string plugin_name, std::set<PixelFormat> in_pixel_formats,
               std::set<PixelFormat> out_pixel_formats, uint32_t buffer_count,
               const bool inplace_processing, std::string lib_version,
               std::string git_change_id) {
    in_buffer_requirements_.min_width_ = 16;
    in_buffer_requirements_.min_height_ = 16;
    in_buffer_requirements_.max_width_ = 16382;
    in_buffer_requirements_.max_height_ = 16382;
    in_buffer_requirements_.width_alignment_ = 2;
    in_buffer_requirements_.height_alignment_ = 2;
    in_buffer_requirements_.cached_ = true;
    in_buffer_requirements_.count_ = buffer_count;
    in_buffer_requirements_.history_buffer_count_ = 0;
    in_buffer_requirements_.stride_alignment_ = 16;
    in_buffer_requirements_.plane_alignment_ = 16;
    in_buffer_requirements_.pixel_formats_ = in_pixel_formats;

    out_buffer_requirements_.min_width_ = 16;
    out_buffer_requirements_.min_height_ = 16;
    out_buffer_requirements_.max_width_ = 16382;
    out_buffer_requirements_.max_height_ = 16382;
    out_buffer_requirements_.width_alignment_ = 2;
    out_buffer_requirements_.height_alignment_ = 2;
    out_buffer_requirements_.cached_ = true;
    out_buffer_requirements_.count_ = buffer_count;
    out_buffer_requirements_.history_buffer_count_ = 0;
    out_buffer_requirements_.stride_alignment_ = 16;
    out_buffer_requirements_.plane_alignment_ = 16;
    out_buffer_requirements_.pixel_formats_ = out_pixel_formats;

    plugin_name_ = plugin_name;
    inplace_processing_ = inplace_processing;
    runtime_enable_disable_ = true;
    crop_support_ = false;
    scale_support_ = false;
    lib_version_ = lib_version;
    git_change_id_ = git_change_id;
  };

  Capabilities(const Capabilities &caps)
      : plugin_name_(caps.plugin_name_),
        in_buffer_requirements_(caps.in_buffer_requirements_),
        out_buffer_requirements_(caps.out_buffer_requirements_),
        inplace_processing_(caps.inplace_processing_),
        runtime_enable_disable_(caps.runtime_enable_disable_),
        crop_support_(caps.crop_support_),
        scale_support_(caps.scale_support_),
        lib_version_(caps.lib_version_),
        git_change_id_(caps.git_change_id_){};

  Capabilities(const std::string plugin_name,
               const BufferRequirements &in_buffer_requirements,
               const BufferRequirements &out_buffer_requirements,
               const bool inplace_processing, const bool runtime_enable_disable,
               const bool crop_support, const bool scale_support,
               const std::string lib_version, const std::string git_change_id)
      : plugin_name_(plugin_name),
        in_buffer_requirements_(in_buffer_requirements),
        out_buffer_requirements_(out_buffer_requirements),
        inplace_processing_(inplace_processing),
        runtime_enable_disable_(runtime_enable_disable),
        crop_support_(crop_support),
        scale_support_(scale_support),
        lib_version_(lib_version),
        git_change_id_(git_change_id){};

  virtual ~Capabilities(){};

  std::string ToString(uint32_t indent = 0) const {
    std::stringstream indentation;
    for (uint32_t i = 0; i < indent; i++) indentation << '\t';
    indent++;

    std::stringstream stream;
    stream << indentation.str() << "\"plugin_name_\" : " << plugin_name_
           << '\n';
    stream << indentation.str() << "\"in_buffer_requirements_\" : {" << '\n'
           << in_buffer_requirements_.ToString(indent) << "}," << '\n';
    stream << indentation.str() << "\"out_buffer_requirements_\" : {" << '\n'
           << out_buffer_requirements_.ToString(indent) << "}," << '\n';
    stream << indentation.str()
           << "\"inplace_processing_\" : " << inplace_processing_ << ",\n";
    stream << indentation.str()
           << "\"runtime_enable_disable_\" : " << runtime_enable_disable_
           << '\n';
    stream << indentation.str() << "\"crop_support_\" : " << crop_support_
           << '\n';
    stream << indentation.str() << "\"scale_support_\" : " << scale_support_
           << '\n';
    stream << indentation.str() << "\"lib_version_\" : " << lib_version_
           << '\n';
    stream << indentation.str() << "\"git_change_id_\" : " << git_change_id_
           << '\n';
    return stream.str();
  }

  std::string plugin_name_;
  BufferRequirements in_buffer_requirements_;
  BufferRequirements out_buffer_requirements_;
  bool inplace_processing_;
  bool runtime_enable_disable_;
  bool crop_support_;
  bool scale_support_;
  std::string lib_version_;
  std::string git_change_id_;
};

/** IEventListener
 *
 * Algorithm interface
 *
 **/
class IEventListener {
 public:
  virtual ~IEventListener(){};

  /** OnFrameProcessed
   *    @input_buffer: input buffer
   *
   * Indicates that input buffer is processed
   *
   * return: void
   **/
  virtual void OnFrameProcessed(const AlgBuffer &input_buffer) = 0;

  /** OnFrameReady
   *    @output_buffer: output buffer
   *
   * Indicates that output buffer is processed
   *
   * return: void
   **/
  virtual void OnFrameReady(const AlgBuffer &output_buffer) = 0;

  /** OnError
   *    @err: error id
   *
   * Indicates runtime error
   *
   * return: void
   **/
  virtual void OnError(RuntimeError err) = 0;
};

/** IBufferHolder
 *
 * Buffer Holder
 *
 **/
class IBufferHolder {
 public:
  virtual ~IBufferHolder(){};

  /** CpuAccessStart
   *
   * Start of CPU access
   *
   * return: nothing
   **/
  virtual void CpuAccessStart() const = 0;

  /** CpuAccessEnd
   *
   * End of CPU access
   *
   * return: nothing
   **/
  virtual void CpuAccessEnd() const = 0;

  /** GetAddr
   *
   * returns address
   *
   * return: address
   **/
  virtual const uint8_t *GetAddr() const = 0;

  /** GetFd
   *
   * returns fd
   *
   * return: fd
   **/
  virtual int32_t GetFd() const = 0;

  /** GetSize
   *
   * returns size
   *
   * return: size
   **/
  virtual uint32_t GetSize() const = 0;
};

/** ITools
 *
 * Algorithm platform specific tools
 *
 **/
class ITools {
 public:
  virtual ~ITools(){};

  /** SetProperty
   *    @property: property
   *    @value: value
   *
   * Sets requested property value
   *
   * return: nothing
   **/
  virtual void SetProperty(std::string property, std::string value) = 0;

  /** SetProperty
   *    @property: property
   *    @value: value
   *
   * Sets requested property value
   *
   * return: nothing
   **/
  virtual void SetProperty(std::string property, int32_t value) = 0;

  /** GetProperty
   *    @property: property
   *    @default_value: default value
   *
   * Gets requested property value
   *
   * return: property value
   **/
  virtual const std::string GetProperty(std::string property,
                                        std::string default_value) = 0;

  /** GetProperty
   *    @property: property
   *    @default_value: default value
   *
   * Gets requested property value
   *
   * return: property value
   **/
  virtual uint32_t GetProperty(std::string property, int32_t value) = 0;

  /** LogError
   *
   * Logs error
   *
   * return: nothing
   **/
  virtual void LogError(const std::string &s) = 0;

  /** LogWarning
   *
   * Logs error
   *
   * return: nothing
   **/
  virtual void LogWarning(const std::string &s) = 0;

  /** LogInfo
   *
   * Logs error
   *
   * return: nothing
   **/
  virtual void LogInfo(const std::string &s) = 0;

  /** LogDebug
   *
   * Logs error
   *
   * return: nothing
   **/
  virtual void LogDebug(const std::string &s) = 0;

  /** LogVerbose
   *
   * Logs error
   *
   * return: nothing
   **/
  virtual void LogVerbose(const std::string &s) = 0;

  /** ImportBufferHolder
   *    @vaddr: arm virtual address
   *    @fd: buffer fd
   *    @size: buffer size
   *    @cached: flag indicating whether buffer is cached
   *
   * Imports buffer holder
   *
   * return: shared pointer to buffer holder
   **/
  virtual std::shared_ptr<IBufferHolder> ImportBufferHolder(
      const uint8_t *vaddr, const int32_t fd, const uint32_t size,
      const bool cached) = 0;

  /** NewBufferHolder
   *    @size: buffer size
   *    @cached: flag indicating whether buffer is cached
   *
   * Creates new buffer holder
   *
   * return: shared pointer to new buffer holder
   **/
  virtual std::shared_ptr<IBufferHolder> NewBufferHolder(const uint32_t size,
                                                         const bool cached) = 0;
};

/** IAlgPlugin
 *
 * Algorithm interface
 *
 **/
class IAlgPlugin {
 public:
  virtual ~IAlgPlugin(){};

  /** SetCallbacks
   *    @event_listener: event listener
   *
   * Set callbacks
   *
   * return: void
   **/
  virtual void SetCallbacks(IEventListener *event_listener) = 0;

  /** GetInputRequirements
   *    @out: output parameters
   *
   * This function returns the input requirements for given outputs
   *
   * return: input requirements
   **/
  virtual const Requirements GetInputRequirements(const Requirements &out) = 0;

  /** Configure
   *
   * Set algorithm specific config data
   *    @config_json_data: config data in JSON format
   *
   * return: string
   **/
  virtual const std::string Configure(const std::string &config_json_data) = 0;

  /** Configure
   *    @type: binary data type
   *    @config_data: config binary data
   *
   * Set algorithm specific config data
   *
   * return: std::vector<uint8_t>
   **/
  virtual const std::vector<uint8_t> Configure(
      const int32_t type, const std::vector<uint8_t> &config_data) = 0;

  /** RegisterInputBuffers
   *    @buffers: vector of input buffers to register
   *
   * Register input buffers to qmmf algo library. All buffers should be
   * registered before passed to the library for processing.
   *
   * return: void
   **/
  virtual void RegisterInputBuffers(const std::vector<AlgBuffer> &buffers) = 0;

  /** UnregisterInputBuffers
   *    @buffers: vector of input buffers to unregister
   *
   * Unregister input buffers from the library. After this call buffers
   * can not be used for processing. Library responsibility is to
   * free all references to this buffers. If buffers are in library
   * processing queue unregister should return an error.
   *
   * return: void
   **/
  virtual void UnregisterInputBuffers(
      const std::vector<AlgBuffer> &buffers) = 0;

  /** RegisterOutputBuffers
   *    @buffers: vector of output buffers to register
   *
   * Register output buffers to qmmf algo library. All buffers should be
   * registered before passed to the library for processing.
   *
   * return: void
   **/
  virtual void RegisterOutputBuffers(const std::vector<AlgBuffer> &buffers) = 0;

  /** UnregisterOutputBuffers
   *    @buffers: vector of output buffers to unregister
   *
   * Unregister output buffers from the library. After this call buffers
   * can not be used for processing. Library responsibility is to
   * free all references to this buffers. If buffers are in library
   * processing queue unregister should return an error.
   *
   * return: void
   **/
  virtual void UnregisterOutputBuffers(
      const std::vector<AlgBuffer> &buffers) = 0;

  /** Abort
   *
   * Aborts current processing in the earliest possible stage and
   * flushes all buffers from library processing queue.
   * All buffers should be returned with corresponding callbacks,
   * error status should be set if output buffers are released
   * and not yet processed.
   * After this call library processing queue should be empty.
   *
   * return: void
   **/
  virtual void Abort() = 0;

  /** Process
   *    @input_buffers: vector of input buffers
   *    @output_buffers: vector of output buffers
   *
   * main qmmf algo function to process image data
   *
   * return: string
   **/
  virtual const std::string Process(
      const std::vector<AlgBuffer> &input_buffers,
      const std::vector<AlgBuffer> &output_buffers) = 0;
};

/* QmmfAlgLoadPlugin
 *    @calibration_data: calibration data
 *    @tools: tools
 *
 * Creates new algorithm instance
 *
 * return: pointer to new algorithm instance
 **/
typedef IAlgPlugin *(*QmmfAlgLoadPlugin)(
    const std::vector<uint8_t> &calibration_data, ITools &tools);

/* QmmfAlgGetCapabilites
 *
 * Gets algorithm capabilities and requirements
 *
 * return: algorithm capabilities
 **/
typedef const Capabilities *(*QmmfAlgGetCapabilites)();

};  // namespace qmmf_alg_plugin

};  // namespace qmmf
