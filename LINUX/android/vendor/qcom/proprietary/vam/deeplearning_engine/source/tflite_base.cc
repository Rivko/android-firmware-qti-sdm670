/*
 * Copyright (c) 2019, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <fstream>
#include <vector>
#include <string>
#include <fastcv/fastcv.h>
#include <tensorflow/lite/kernels/register.h>
#include <tensorflow/lite/examples/label_image/get_top_n.h>
#include <tensorflow/lite/examples/label_image/get_top_n_impl.h>
#include "tflite_base.h"

namespace mle {

// Takes a file name, and loads a list of labels from it, one per line, and
// returns a vector of the strings. It pads with empty strings so the length
// of the result is a multiple of 16, because our model expects that.
TfLiteStatus TFLBase::ReadLabelsFile(const std::string& file_name,
                            std::vector<std::string>& result,
                            size_t& found_label_count) {
  std::ifstream file(file_name);
  if (!file) {
    VAM_ML_LOGE("%s: Labels file %s not found!", __func__, file_name.c_str());
    return kTfLiteError;
  }
  result.clear();
  std::string line;
  while (std::getline(file, line)) {
    result.push_back(line);
  }
  found_label_count = result.size();
  const int padding = 16;
  while (result.size() % padding) {
    result.emplace_back();
  }
  return kTfLiteOk;
}

TFLBase::TFLBase(MLConfig &config) {
  config_.target_fps = config.target_fps;
  config_.conf_threshold = config.conf_threshold;
  config_.model_file = config.model_file;
  config_.labels_file = config.labels_file;
  config_.number_of_threads = config.number_of_threads;
  config_.use_nnapi = config.use_nnapi;
  input_params_.rgb_buf = nullptr;
  //verbose
}

int32_t TFLBase::Init(const struct vaapi_source_info* source_info) {

  // Load tflite model from file
  std::string folder(&source_info->data_folder[0]);
  std::string model_file = folder + "/" + config_.model_file;
  engine_params_.model = tflite::FlatBufferModel::BuildFromFile(model_file.c_str());
  if (!engine_params_.model) {
    VAM_ML_LOGE("%s: Failed to load model from %s", __func__, model_file.c_str());
    return VAM_FAIL;
  }
  VAM_ML_LOGI("%s: Loaded model from %s", __func__, model_file.c_str());

  // Load labels from file
  std::string labels_file_name = folder + "/" + config_.labels_file;
  if (ReadLabelsFile(labels_file_name, engine_params_.labels,
                     engine_params_.label_count) != kTfLiteOk) {
    VAM_ML_LOGE("%s: Failed to read labeles file %s", __func__,
                     labels_file_name.c_str());
    return VAM_FAIL;
  }
  VAM_ML_LOGI("%s: Loaded %d labels from %s", __func__,
                   engine_params_.label_count, labels_file_name.c_str());

  // Gather input configuration parameters
  input_params_.width  = source_info->frame_l_width[0];
  input_params_.height = source_info->frame_l_height[0];
  input_params_.format = source_info->img_format;

  VAM_ML_LOGI("%s: Input data: format %d, height %d, width %d", __func__,
                   input_params_.format, input_params_.height, input_params_.width);

  // Create the interpreter
  tflite::ops::builtin::BuiltinOpResolver resolver;
  tflite::InterpreterBuilder(*engine_params_.model, resolver)(&engine_params_.interpreter);
  if (!engine_params_.interpreter) {
    VAM_ML_LOGE("%s: Failed to construct interpreter", __func__);
    return VAM_FAIL;
  }

  // Set the interpreter configurations
  engine_params_.interpreter->UseNNAPI(config_.use_nnapi);
  engine_params_.interpreter->SetNumThreads(config_.number_of_threads);
  VAM_ML_LOGI("%s: USE_NNAPI %d No: of threads %d", __func__,
                   config_.use_nnapi, config_.number_of_threads);

  // Validate & process model information
  if (ValidateModelInfo() != VAM_OK) {
    VAM_ML_LOGE("%s: Provided model is not supported", __func__);
    return VAM_FAIL;
  }

  // Allocate buffer for appending input frame's data
  // Since fast-cv based color conversion is used, no need to
  // append frame_l_data[0], frame_l_data[1] & frame_l_data[2]

  // Check if rescaling is required or not
  if ((input_params_.width != engine_params_.width) ||
      (input_params_.height != engine_params_.height)) {
    engine_params_.do_rescale = true;

    // Allocate output buffer for color conversion
    posix_memalign(reinterpret_cast<void**>(&input_params_.rgb_buf),
                                    128,
                                    (input_params_.width * input_params_.height * 3));
    if (nullptr == input_params_.rgb_buf) {
      VAM_ML_LOGE("%s: Color conv. output buffer allocation failed", __func__);
      return VAM_FAIL;
    }
  } else {
    engine_params_.do_rescale = false;
  }

  // Allocate output buffer for re-scaling
  // Input tensor would be used for holding scaled output data

  // Allocate the tensors
  if (engine_params_.interpreter->AllocateTensors() != kTfLiteOk) {
    VAM_ML_LOGE("%s: Failed to allocate tensors!", __func__);
    if (nullptr != input_params_.rgb_buf) {
      free(input_params_.rgb_buf);
      input_params_.rgb_buf = nullptr;
    }
    return VAM_FAIL;
  }

  // Make a note of the model's input buffer
  int input = engine_params_.interpreter->inputs()[0];
  TfLiteType input_type = engine_params_.interpreter->tensor(input)->type;
  switch (input_type) {
    case kTfLiteUInt8:
      engine_params_.input_buffer = (engine_params_.interpreter->tensor(input)->data).uint8;
      break;
    case kTfLiteFloat32:
      engine_params_.input_buffer_f = (engine_params_.interpreter->tensor(input)->data).f;
      break;
    default:
      VAM_ML_LOGE("%s: No support for %d input type", __func__, input_type);
      return VAM_FAIL;
  }

  VAM_ML_LOGI("%s: Exit", __func__);
  return VAM_OK;
}

void TFLBase::Deinit() {
  VAM_ML_LOGI("%s: Enter", __func__);
  if (nullptr != input_params_.rgb_buf) {
    free(input_params_.rgb_buf);
    input_params_.rgb_buf = nullptr;
  }
  VAM_ML_LOGI("%s: Exit", __func__);
}

int32_t TFLBase::Process(struct vaapi_frame_info* frame_info) {
  VAM_ML_LOGI("%s: Enter", __func__);

  // pre-processing input frame
  if (PreProcessInput(frame_info) != VAM_OK) {
    VAM_ML_LOGE("%s: PreProcessInput Failed!!!", __func__);
    return VAM_FAIL;
  }

  // Execute the network
  if (engine_params_.interpreter->Invoke() != kTfLiteOk) {
    VAM_ML_LOGE("%s: Failed to invoke!", __func__);
    return VAM_FAIL;
  }
  VAM_ML_LOGI("%s: Execution completed!", __func__);

  // post-processing the output results
  if (PostProcessOutput() != VAM_OK) {
    VAM_ML_LOGE("%s: PostProcessOutput Failed!!!", __func__);
    return VAM_FAIL;
  }

  VAM_ML_LOGI("%s: Exit", __func__);
  return VAM_OK;
}

int32_t TFLBase::GetObjectsInfo(vaapi_object* objects, uint32_t obj_count) {
  int32_t ret = VAM_OK;

  // Currently GetObjectCount() could only indicate 0 or 1
  if (out_params_.num_objects && (out_params_.num_objects == obj_count)) {
    std::memcpy(objects, &out_params_.objects[0],
                obj_count * sizeof(vaapi_object));

    uint32_t width = input_params_.width;
    uint32_t height = input_params_.height;

    for (uint32_t i = 0; i < obj_count; i++) {
       VAM_ML_LOGI("%s: %d. detected %s, with confidence: %d",
                      __func__, i+1, objects[i].display_name, objects[i].confidence);
       objects[i].pos.x = (objects[i].pos.x * width) / engine_params_.width;
       objects[i].pos.y = (objects[i].pos.y * height) / engine_params_.height;
       objects[i].pos.width = (objects[i].pos.width * width) / engine_params_.width;
       objects[i].pos.height = (objects[i].pos.height * height) / engine_params_.height;
       VAM_ML_LOGI("%s: detected_boxes : %d-%d-%d-%d",
                        __func__, objects[i].pos.x, objects[i].pos.y,
                        objects[i].pos.width, objects[i].pos.height);
    }
  } else {
    VAM_ML_LOGE("%s: Indicated %d objects, requested %d!", __func__,
                     out_params_.num_objects, obj_count);
    return VAM_FAIL;
  }

  return ret;
}

int32_t TFLBase::ValidateModelInfo() {
  // Validate for supported models
  // Mobilenet model
  //   - input tensor (1, uint8_t, 1 X height X width X 3)
  //   - output tensor (1, uint8_t or float, 1 X number of labels)
  //   - order of predictions corresponds to order of labels, as listed in labels file
  VAM_ML_LOGI("%s: Support tensor types - kTfLiteFloat32 %d - kTfLiteUInt8 %d",
                  __func__, kTfLiteFloat32, kTfLiteUInt8);
  // Validate input nodes
  const std::vector<int> inputs = engine_params_.interpreter->inputs();
  engine_params_.num_inputs = inputs.size();
  if (engine_params_.num_inputs != 1) {
    VAM_ML_LOGE("%s: No support for %d input nodes", __func__,
                    engine_params_.num_inputs);
    return VAM_FAIL;
  }

  int input = engine_params_.interpreter->inputs()[0];

  // Check for input tensor type
  TfLiteType input_type = engine_params_.interpreter->tensor(input)->type;
  switch (input_type) {
    case kTfLiteUInt8:
    case kTfLiteFloat32:
      break;
    default:
      VAM_ML_LOGE("%s: No support for %d input type", __func__, input_type);
      return VAM_FAIL;
  }

  TfLiteIntArray* dims = engine_params_.interpreter->tensor(input)->dims;
  int batch_size = dims->data[0];
  if (batch_size != 1) {
    VAM_ML_LOGE("%s: No support for %d input batch size", __func__, batch_size);
    return VAM_FAIL;
  }

  // Initialize engine configuration parameters
  engine_params_.height = dims->data[1];
  engine_params_.width = dims->data[2];
  engine_params_.channels = dims->data[3];

  //Assume all TFLite models expect input data in RGB format
  engine_params_.format = vaapi_format_RGB24;

  VAM_ML_LOGI("%s: Input tensor: type %d, batch size %d, format %d", __func__,
              input_type, batch_size, engine_params_.format);
  VAM_ML_LOGI("%s: Input tensor: height %d, width %d, channels %d", __func__,
              engine_params_.height, engine_params_.width,
              engine_params_.channels);
  VAM_ML_LOGI("%s: Input tensor: quantization scale %f, zero_point %d",
              __func__, engine_params_.interpreter->tensor(input)->params.scale,
              engine_params_.interpreter->tensor(input)->params.zero_point);

  // Validate output nodes
  const std::vector<int> outputs = engine_params_.interpreter->outputs();
  engine_params_.num_outputs = outputs.size();
  if (engine_params_.num_inputs != 1 && engine_params_.num_inputs != 4) {
    VAM_ML_LOGE("%s: No support for %d output nodes", __func__,
                engine_params_.num_outputs);
    return VAM_FAIL;
  }
  if (engine_params_.num_outputs == 1) {
    int output = engine_params_.interpreter->outputs()[0];

    // Check for output tensor type
    TfLiteType output_type = engine_params_.interpreter->tensor(output)->type;
    switch (output_type) {
      case kTfLiteUInt8:
      case kTfLiteFloat32:
        break;
      default:
        VAM_ML_LOGE("%s: No support for %d output type", __func__, output_type);
        return VAM_FAIL;
        break;
    }

    // Check for output tensor dimensions
    TfLiteIntArray* output_dims = engine_params_.interpreter->tensor(output)->dims;
    engine_params_.num_predictions = output_dims->data[output_dims->size - 1];
    if (engine_params_.label_count != engine_params_.num_predictions) {
      VAM_ML_LOGE("%s: No: of labels %d, DO NOT match no: of predictions %d",
                       __func__, engine_params_.label_count,
                       engine_params_.num_predictions);
      return VAM_FAIL;
    }

    VAM_ML_LOGI("%s: Output tensor: type %d, no: of predictions %d", __func__,
                     output_type, engine_params_.num_predictions);
    VAM_ML_LOGI("%s: Output tensor: quantization scale %f, zero_point %d",
                     __func__,
                     engine_params_.interpreter->tensor(output)->params.scale,
                     engine_params_.interpreter->tensor(output)->params.zero_point);
  } else if (engine_params_.num_outputs == 4) {
    for (uint32_t i = 0; i < engine_params_.num_outputs; ++i) {
      int output = engine_params_.interpreter->outputs()[i];

      // Check for output tensor type
      TfLiteType output_type = engine_params_.interpreter->tensor(output)->type;
      switch (output_type) {
        case kTfLiteFloat32:
          break;
        default:
          VAM_ML_LOGE("%s: For output node %d, no support for %d output type",
                          __func__, i, output_type);
          return VAM_FAIL;
          break;
      }
      VAM_ML_LOGI("%s: Output tensor: type %d", __func__, output_type);

      // Output node quantization values
      VAM_ML_LOGI("%s: Output tensor: quantization scale %f, zero_point %d",
                  __func__,
                  engine_params_.interpreter->tensor(output)->params.scale,
                  engine_params_.interpreter->tensor(output)->params.zero_point);
    }
  }

  return VAM_OK;
}

int32_t TFLBase::PreProcessInput(vaapi_frame_info* frame_info) {
  VAM_ML_LOGI("%s: Enter", __func__);
  if (engine_params_.do_rescale) {
    //Color conversion
    fcvColorYCbCr420PseudoPlanarToRGB888u8(frame_info->frame_l_data[0],
                                           frame_info->frame_l_data[1],
                                           input_params_.width,
                                           input_params_.height, 0, 0,
                                           input_params_.rgb_buf, 0);
    VAM_ML_LOGI("%s: Color conversion completed!", __func__);

    // rescaling
    fcvScaleDownMNu8(input_params_.rgb_buf,
                     (input_params_.width * 3), input_params_.height, 0,
                     engine_params_.input_buffer,
                     (engine_params_.width * 3), engine_params_.height, 0);
  } else {
    //Color conversion
    fcvColorYCbCr420PseudoPlanarToRGB888u8(frame_info->frame_l_data[0],
                                           frame_info->frame_l_data[1],
                                           input_params_.width,
                                           input_params_.height, 0, 0,
                                           engine_params_.input_buffer, 0);
  }
  VAM_ML_LOGI("%s: Exit", __func__);
  return VAM_OK;
}

int32_t TFLBase::PostProcessMultiOutput() {
  VAM_ML_LOGI("%s: Enter", __func__);

  float *detected_boxes = engine_params_.interpreter->typed_output_tensor<float>(0);
  float *detected_classes = engine_params_.interpreter->typed_output_tensor<float>(1);
  float *detected_scores = engine_params_.interpreter->typed_output_tensor<float>(2);
  float *num_boxes = engine_params_.interpreter->typed_output_tensor<float>(3);

  // reset output object entries information
  int num_output_objects = sizeof(out_params_.objects) / sizeof(vaapi_object);
  VAM_ML_LOGI("%s: num_output_objects %d", __func__, num_output_objects);
  for (int i=0; i < num_output_objects; i++) {
    vaapi_object* object = &out_params_.objects[i];
    memset(object, 0, sizeof(vaapi_object));
  }

  out_params_.num_objects = 0;
  float num_box = num_boxes[0];
  VAM_ML_LOGI("%s: Found %f boxes", __func__, num_box);
  for (int i = 0; i < num_box; i++) {
    if (detected_scores[i] < config_.conf_threshold) break;

    //make a note of classified object
    vaapi_object* object = &out_params_.objects[out_params_.num_objects];
    auto rc = std::string(engine_params_.labels[detected_classes[i]+1]).copy(
        object->display_name, sizeof(object->display_name) - 1);
    object->display_name[rc] = '\0';
    object->confidence = static_cast<uint8_t>(detected_scores[i]*100);
    object->pos.x = static_cast<uint32_t>(
                      detected_boxes[i*4 + 1] * engine_params_.width);
    object->pos.y = static_cast<uint32_t>(
                      detected_boxes[i*4] * engine_params_.height);
    object->pos.width = static_cast<uint32_t>(
                          detected_boxes[i*4 + 3] * engine_params_.width) -
                          object->pos.x;
    object->pos.height = static_cast<uint32_t>(
                           detected_boxes[i*4 + 2] * engine_params_.height) -
                           object->pos.y;
    VAM_ML_LOGI("%s: Detected %s, with confidence: %d", __func__,
                     object->display_name, object->confidence);
    VAM_ML_LOGI("%s: detected_boxes : %d-%d-%d-%d", __func__, object->pos.x,
                     object->pos.y, object->pos.width, object->pos.height);
    out_params_.num_objects++;
  }
  VAM_ML_LOGI("%s: Exit", __func__);
  return VAM_OK;
}

int32_t TFLBase::PostProcessOutput() {
  VAM_ML_LOGI("%s: Enter", __func__);

  if (engine_params_.num_outputs == 4) {
    // post-processing the output results from 4 nodes
    if (PostProcessMultiOutput() != VAM_OK) {
      VAM_ML_LOGE("%s: PostProcessMultiOutput Failed!!!", __func__);
      return VAM_FAIL;
    }
    VAM_ML_LOGI("%s: Exit", __func__);
    return VAM_OK;
  }

  // For MobileNet model, return only top most confident object info
  int output = engine_params_.interpreter->outputs()[0];
  auto top_results_count = 1;
  std::vector<std::pair<float, int>> top_results;

  // Sort output predictions in descending order of confidence
  // and then, return the top most confidence result
  bool verbose = false;
  switch (engine_params_.interpreter->tensor(output)->type) {
    case kTfLiteFloat32:
      if (verbose) {
        float* temp_output = engine_params_.interpreter->typed_output_tensor<float>(0);
        for (uint32_t i = 0; i < engine_params_.num_predictions; ++i) {
          VAM_ML_LOGI("%s: i - %d :: conf - %f", __func__, i, temp_output[i]);
        }
      }
      tflite::label_image::get_top_n<float>(
                       engine_params_.interpreter->typed_output_tensor<float>(0),
                       engine_params_.num_predictions, top_results_count,
                       config_.conf_threshold, &top_results, true);
      break;
    case kTfLiteUInt8:
      if (verbose) {
        uint8_t* temp_output = engine_params_.interpreter->typed_output_tensor<uint8_t>(0);
        for (uint32_t i = 0; i < engine_params_.num_predictions; ++i) {
          VAM_ML_LOGI("%s: i - %d :: conf - %d", __func__, i, temp_output[i]);
        }
      }
      tflite::label_image::get_top_n<uint8_t>(
                       engine_params_.interpreter->typed_output_tensor<uint8_t>(0),
                       engine_params_.num_predictions, top_results_count,
                       config_.conf_threshold, &top_results, false);
      break;
    default:
      VAM_ML_LOGE("%s: Invalid output tensor type %d", __func__,
                      engine_params_.interpreter->tensor(output)->type);
      return VAM_FAIL;
      break;
  }
  VAM_ML_LOGI("%s: Found %d objects", __func__, top_results.size());

  // reset output object entries information
  vaapi_object* object = &out_params_.objects[0];
  memset(object, 0, sizeof(vaapi_object));

  // If found, return the label with most confidence level
  if (top_results.size() > 0) {
    const auto& result = top_results.front();
    const float confidence = result.first;
    const int index = result.second;
    VAM_ML_LOGI("%s: confidence %f, index %d, label %s", __func__,
                    confidence, index, engine_params_.labels[index].c_str());

    //make a note of classified object
    out_params_.num_objects = 1;
    auto rc = std::string(engine_params_.labels[index]).copy(
        object->display_name, sizeof(object->display_name) - 1);
    object->display_name[rc] = '\0';
    object->confidence = static_cast<uint8_t>(confidence);
  } else {
    out_params_.num_objects = 0;
  }

  VAM_ML_LOGI("%s: Exit", __func__);
  return VAM_OK;
}

}; // namespace mle
