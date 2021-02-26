/*
 * Copyright (c) 2018-2019, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include "snpe_base.h"

namespace mle {

class SNPEComplex : public SNPEBase {
 public:
  SNPEComplex(MLConfig &config);
  ~SNPEComplex();
  int32_t Process(struct vaapi_frame_info* frame_info);
  int32_t EnginePostProcess(SNPEExecuteOutParams& execute_params);
  int32_t GetObjectsInfo(vaapi_object* objects, uint32_t obj_count);
};

}; // namespace mle
