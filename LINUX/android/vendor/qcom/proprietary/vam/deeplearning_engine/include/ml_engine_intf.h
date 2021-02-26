/*
 * Copyright (c) 2019, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include <vector>
#include <string>
#include "VAM/vaapi_common.h"

namespace mle {

enum class FrameworkType {
  kSNPE = 0,
  kTFLite
};

enum class RuntimeType {
  CPU = 0,
  DSP,
};

enum class InputFormat {
  kRgb = 0,
  kBgr,
  kRgbFloat,
  kBgrFloat
};

enum class BufferType {
  kOutput = 0,
  kInput
};

enum class NetworkIO {
  kUserBuffer = 0,
  kITensor
};

struct MLConfig {

  //temporary for SNPE as we can obtain dimensions from model
  uint32_t scale_width;
  uint32_t scale_height;

  uint8_t target_fps;
  InputFormat input_format;
  NetworkIO io_type;

  // normalization
  float blue_mean;
  float blue_sigma;
  float green_mean;
  float green_sigma;
  float red_mean;
  float red_sigma;
  uint32_t use_norm;
  // end normalization

  float conf_threshold;
  std::string model_file;
  std::string labels_file;

  //runtime
  RuntimeType runtime;

  //tflite specific
  uint32_t number_of_threads;
  uint32_t use_nnapi;

  //snpe layers
  std::string input_layers; //todo remove and obtain from model
  std::vector<std::string> output_layers;
  std::vector<std::string> result_layers; //check if possible to remove
};

struct EngineOutParams {
  uint32_t num_objects;
  vaapi_object objects[32];
};

class MLEngine {
 public:
  MLEngine(){};
  virtual ~MLEngine(){};
  virtual int32_t Init(const struct vaapi_source_info* source_info) = 0;
  virtual void Deinit() = 0;
  virtual int32_t Process(struct vaapi_frame_info* frame_info) = 0;
  virtual int32_t GetObjectsInfo(vaapi_object* objects, uint32_t obj_count) = 0;
  uint32_t GetObjectsCount() {return out_params_.num_objects;}
  uint8_t GetTargetFPS() {return config_.target_fps;}
  uint8_t IsEventSupported(vaapi_event_type type) {
    return (type == vaapi_event_object_classified) ? 1 : 0;
  }
 protected:
  EngineOutParams out_params_;
  MLConfig config_;
};

}; // namespace mle
