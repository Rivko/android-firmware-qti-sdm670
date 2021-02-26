/*
 * Copyright (c) 2019, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <vector>
#include "snpe_single_ssd.h"

namespace mle {

static const int kMaxNumObjects = 5;

SNPESingleSSD::SNPESingleSSD(MLConfig &config) : SNPEBase(config),
                                                 output_params_(nullptr) {}
SNPESingleSSD::~SNPESingleSSD() {}

int32_t SNPESingleSSD::EnginePostProcess(SNPEExecuteOutParams& execute_params) {
  VAM_ML_LOGI("%s: Enter", __func__);

  std::vector<float> result_buf;
  const zdl::DlSystem::StringList &output_buf_names =
      snpe_params_.output_ub_map.getUserBufferNames();
  const zdl::DlSystem::StringList *output_names = &output_buf_names;

  std::for_each(
      output_names->begin(),
      output_names->end(),
      [&](const char* name)
      {
    // Currently, singleSSD supports only UserBuffers
        if (config_.io_type == NetworkIO::kUserBuffer) {
          if ((config_.input_format == InputFormat::kBgr) ||
              (config_.input_format == InputFormat::kRgb)) {
            if (0 == std::strcmp(name, config_.result_layers[0].c_str())) {
              IONBuffer b = snpe_params_.out_heap_map.at(name);
              for (size_t i = 0; i < b.size / sizeof(uint8_t); i++) {
                result_buf.push_back(b.addr[i]);
              }
            }
          } else {
            if (0 == std::strcmp(name, config_.result_layers[0].c_str())) {
              IONBuffer b = snpe_params_.out_heap_map.at(name);
              for (size_t i = 0; i < b.size / sizeof(float); i++) {
                result_buf.push_back(b.addr_f[i]);
              }
            }
          }
        }
      });

  if (result_buf.size()) {
    output_params_ = reinterpret_cast<OutputParams*>(result_buf.data());

    for (size_t i = 0; i < kMaxNumObjects; i++) {
      if (output_params_[i].score < init_params_.conf_threshold) {
        continue;
      }
      if ((output_params_[i].boxes.x < 0) ||
          (output_params_[i].boxes.y < 0) ||
          (output_params_[i].boxes.width < 0) ||
          (output_params_[i].boxes.height < 0)) {
        continue;
      }


      execute_params.prob.push_back(output_params_[i].score);
      std::vector<float> vert;
      vert.push_back(std::ceil(output_params_[i].boxes.x * config_.scale_width));
      vert.push_back(std::ceil(output_params_[i].boxes.width * config_.scale_width));
      vert.push_back(std::ceil(output_params_[i].boxes.y * config_.scale_height));
      vert.push_back(std::ceil(output_params_[i].boxes.height * config_.scale_height));
      execute_params.boxes.push_back(vert);
      execute_params.labels.push_back(
          labels_.at(static_cast<uint32_t>(output_params_[i].label + 0.5)));
    }
    VAM_ML_LOGI("Inference engine detected %d objects, highest score: %f",
                  execute_params.labels.size(), output_params_[0].score);
  }

  VAM_ML_LOGI("%s: Exit", __func__);
  return VAM_OK;
}

int32_t SNPESingleSSD::Process(struct vaapi_frame_info* frame_info) {
  int32_t result = VAM_OK;

  result = PreProcessBuffer(frame_info);
  if (VAM_OK != result) {
    VAM_ML_LOGE("PreProcessBuffer failed");
    return result;
  }

  result = ExecuteSNPE();
  if (VAM_OK != result) {
    VAM_ML_LOGE(" SNPE execution failed");
    return result;
  }

  SNPEExecuteOutParams execute_params;
  result = EnginePostProcess(execute_params);
  if (VAM_OK == result) {
    for (uint32_t i = 0; i < execute_params.labels.size(); i++) {
      vaapi_object* object = &out_params_.objects[i];
      memset(object, 0, sizeof(vaapi_object));
      auto rc = std::string(execute_params.labels[i]).copy(
          object->display_name, sizeof(object->display_name) - 1);
      object->display_name[rc] = '\0';
      object->confidence = static_cast<uint8_t>(execute_params.prob[i] * 100);
      VAM_ML_LOGE("SNPE engine detected a %s, with confidence: %d",
                  object->display_name, object->confidence);
      object->type = GetTypeFromClassLabel(execute_params.labels[i].c_str());
      object->pos.x = static_cast<uint32_t>(execute_params.boxes[i][0]);
      object->pos.y = static_cast<uint32_t>(execute_params.boxes[i][2]);
      object->pos.width = static_cast<uint32_t>(execute_params.boxes[i][1] -
                                                 execute_params.boxes[i][0]);
      object->pos.height = static_cast<uint32_t>(execute_params.boxes[i][3] -
                                                  execute_params.boxes[i][2]);
    }
    out_params_.num_objects = execute_params.labels.size();
    if (out_params_.num_objects > 0) {
      VAM_ML_LOGI("Inference engine detected %d objects",
                  out_params_.num_objects);
    }
  }
  return result;
}

int32_t SNPESingleSSD::GetObjectsInfo(vaapi_object* objects,
                                      uint32_t obj_count) {
  std::memcpy(objects, &out_params_.objects[0],
              obj_count * sizeof(vaapi_object));
  uint32_t width = init_params_.width;
  uint32_t height = init_params_.height;

  double in_ar = static_cast<double>(init_params_.width) / init_params_.height;
  double out_ar = static_cast<double>(config_.scale_width) /
                                      config_.scale_height;

  if (in_ar > out_ar) {
    width = out_ar * height;
  } else if (in_ar < out_ar) {
    height = width / out_ar;
  }

  uint32_t to_add_x = init_params_.width - width;
  uint32_t to_add_y = init_params_.height - height;

  for (uint32_t i = 0; i < obj_count; i++) {
    objects[i].pos.x = ((objects[i].pos.x * width) / config_.scale_width) +
                         to_add_x / 2;
    objects[i].pos.y = ((objects[i].pos.y * height) / config_.scale_height) +
                         to_add_y / 2;
    objects[i].pos.width = (objects[i].pos.width * width) /
                            config_.scale_width;
    objects[i].pos.height = (objects[i].pos.height * height) /
                             config_.scale_height;
  }

  return VAM_OK;
}
size_t SNPESingleSSD::CalculateSizeFromDims(const size_t rank,
                                       const zdl::DlSystem::Dimension* dims,
                                       const size_t& element_size) {
  if (0 == rank) {
    return 0;
  }
  size_t size = element_size;
  for (size_t i = 0; i < rank; i++) {
    if (0 == dims[i]) {
      size *= kMaxNumObjects;
      continue;
    }
    size *= dims[i];
  }
  return size;
}

std::vector<size_t> SNPESingleSSD::GetStrides(zdl::DlSystem::TensorShape dims,
                                              const size_t& element_size) {
  std::vector<size_t> strides(dims.rank());
  strides[strides.size() - 1] = element_size;
  size_t stride = strides[strides.size() - 1];

  for (size_t i = dims.rank() - 1; i > 0; i--) {
    if (0 == dims[i]) {
      stride *= kMaxNumObjects;
    } else {
      stride *= dims[i];
    }
    strides[i - 1] = stride;
  }

  return strides;
}

}; // namespace mle
