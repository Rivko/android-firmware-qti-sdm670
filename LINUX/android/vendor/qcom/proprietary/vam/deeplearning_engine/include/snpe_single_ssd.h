/*
 * Copyright (c) 2019, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include <vector>
#include "snpe_base.h"

namespace mle {

struct Box {
  float x;
  float y;
  float width;
  float height;
};

struct OutputParams {
  float index;
  float label;
  float score;
  Box boxes;
};

class SNPESingleSSD : public SNPEBase {
 public:
  SNPESingleSSD(MLConfig &config);
  ~SNPESingleSSD();
  int32_t Process(struct vaapi_frame_info* frame_info);
  int32_t EnginePostProcess(SNPEExecuteOutParams& execute_params);
  int32_t GetObjectsInfo(vaapi_object* objects, uint32_t obj_count);
  size_t CalculateSizeFromDims(const size_t rank,
                               const zdl::DlSystem::Dimension* dims,
                               const size_t& element_size);
  std::vector<size_t> GetStrides(zdl::DlSystem::TensorShape dims,
                                 const size_t& element_size);

 private:
  OutputParams *output_params_;
};

}; // namespace mle
