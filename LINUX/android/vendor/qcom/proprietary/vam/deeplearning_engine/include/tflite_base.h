/*
 * Copyright (c) 2019, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include <fstream>
#include <vector>
#include <string>
#include <cstdio>
#include <memory>

#include <VAM/vaapi_errors.h>

#include <tensorflow/lite/interpreter.h>
#include <tensorflow/lite/model.h>

#include "ml_engine_intf.h"
#include "common_utils.h"

namespace mle {

struct TFLiteEngineInputParams {
  uint32_t width;
  uint32_t height;
  vaapi_img_format format;
  uint8_t* rgb_buf;
};

struct TFLiteEngineParams {
  uint32_t width;
  uint32_t height;
  uint32_t channels;
  vaapi_img_format format;
  bool do_rescale;
  std::unique_ptr<tflite::FlatBufferModel> model;
  std::unique_ptr<tflite::Interpreter> interpreter;
  uint32_t num_inputs;
  uint32_t num_outputs;
  uint32_t num_predictions;
  uint8_t* input_buffer;
  float* input_buffer_f;
  std::vector<std::string> labels;
  size_t label_count;
};

class TFLBase : public MLEngine {
 public:
  TFLBase(MLConfig &config);
  ~TFLBase() {};
  int32_t Init(const struct vaapi_source_info* source_info);
  void Deinit();
  int32_t Process(struct vaapi_frame_info* frame_info);
  int32_t GetObjectsInfo(vaapi_object* objects, uint32_t obj_count);

 private:
  int32_t ValidateModelInfo();
  int32_t PreProcessInput(vaapi_frame_info* frame_info);
  int32_t PostProcessMultiOutput();
  int32_t PostProcessOutput();
  TfLiteStatus ReadLabelsFile(const std::string& file_name,
                              std::vector<std::string>& result,
                              size_t& found_label_count);

 protected:
  TFLiteEngineInputParams input_params_;
  TFLiteEngineParams engine_params_;
};

}; // namespace mle
