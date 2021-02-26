/*
 * Copyright (c) 2018-2019, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <fstream>
#include <string>
#include <json/json.h>
#include "VAM/vam_engine_api.h"
#include "snpe_complex.h"
#include "snpe_single_ssd.h"
#include "tflite_base.h"
#include "common_utils.h"

using namespace mle;

extern "C" {
int32_t va_ml_engine_init(const struct vaapi_source_info* source_info,
                          void** handle);

int32_t va_ml_engine_deinit(void* handle);

int32_t va_ml_engine_process(void* handle,
                             struct vaapi_frame_info* frame_info);

int32_t va_ml_engine_add_rule(void* handle, vaapi_rule* rule);

int32_t va_ml_engine_delete_rule(void* handle, const char* config_id);

int32_t va_ml_engine_enroll_obj(void* handle,
                                struct vaapi_enrollment_info* enroll_info);

int32_t va_ml_engine_disenroll_obj(void* handle, const char* obj_id);

int32_t va_ml_engine_get_event_count(void* handle, uint32_t* event_count);

int32_t va_ml_engine_get_events(void* handle, struct vaapi_event* events,
                                uint32_t num_events);

int32_t va_ml_engine_get_obj_count(void* handle, uint32_t* obj_count);

int32_t va_ml_engine_get_objects(void* handle, vaapi_object* objects,
                                  uint32_t obj_count);

int32_t va_ml_engine_get_target_fps(void* handle, uint8_t* desired_fps);

int32_t va_ml_engine_is_event_supported(void* handle, vaapi_event_type type,
                                         uint8_t* is_supported);

int32_t va_ml_engine_is_depends_on_other_event(void* handle,
                                                vaapi_event_type type,
                                                uint8_t* is_dependent);
}


vaapi_module_methods_t va_methods =
{
    .init                      = va_ml_engine_init,
    .deinit                    = va_ml_engine_deinit,
    .process                   = va_ml_engine_process,
    .add_rule                  = va_ml_engine_add_rule,
    .del_rule                  = va_ml_engine_delete_rule,
    .enroll_obj                = va_ml_engine_enroll_obj,
    .disenroll_obj             = va_ml_engine_disenroll_obj,
    .get_event_count           = va_ml_engine_get_event_count,
    .get_events                = va_ml_engine_get_events,
    .get_object_count          = va_ml_engine_get_obj_count,
    .get_objects               = va_ml_engine_get_objects,
    .get_target_FPS            = va_ml_engine_get_target_fps,
    .is_event_supported        = va_ml_engine_is_event_supported,
    .is_depends_on_other_event = va_ml_engine_is_depends_on_other_event
};

vaapi_alg_module_t VA_ALG_INFO_SYM =
{
    .module_api_version      = "1.0",
    .name                    = "va-ml-engine-library",
    .author                  = "QTI",
    .methods                 = &va_methods,
    .reserved                = {NULL, NULL, NULL}
};

enum class EngineOutput {
  kSingle = 0,
  kMulti,
  kSqueezenet,
  kSingleSSD
};

static const std::string PROP_BACKWARD_COMPAT = "persist.vam.ml.backward.compat";

int32_t ParseConfig(const char* filePath,
                    FrameworkType& framework,
                    EngineOutput& output,
                    MLConfig& configuration,
                    uint8_t backward_compat) {
  if (nullptr == filePath) {
    return VAM_NULLPTR;
  }
  int32_t ret = VAM_FAIL;
  std::ifstream in(filePath, std::ios::in | std::ios::binary);
  if (in) {
    Json::Reader reader;
    Json::Value val;
    if (reader.parse(in, val)) {
      framework = (FrameworkType)val.get("FrameworkType", 0).asInt();
      output = (EngineOutput)val.get("EngineOutput", 0).asInt();
      configuration.io_type = (NetworkIO)val.get("NetworkIO", 0).asInt();
      configuration.input_format =
          (InputFormat)val.get("InputFormat", 3).asInt();
      configuration.scale_width = val.get("ScaleWidth", 0).asInt();
      configuration.scale_height = val.get("ScaleHeight", 0).asInt();
      configuration.blue_mean = val.get("BlueMean", 0).asFloat();
      configuration.blue_sigma = val.get("BlueSigma", 255).asFloat();
      configuration.green_mean = val.get("GreenMean", 0).asFloat();
      configuration.green_sigma = val.get("GreenSigma", 255).asFloat();
      configuration.red_mean = val.get("RedMean", 0).asFloat();
      configuration.red_sigma = val.get("RedSigma", 255).asFloat();
      configuration.use_norm = val.get("UseNorm", 0).asInt();
      configuration.target_fps = val.get("TargetFPS", 30).asInt();
      configuration.conf_threshold = val.get("ConfThreshold", 0.0).asFloat();
      configuration.model_file = val.get("MODEL_FILENAME", "").asString();
      configuration.labels_file = val.get("LABELS_FILENAME", "").asString();
      configuration.input_layers = val.get("InputLayers", "").asString();
      for (size_t i = 0; i < val["OutputLayers"].size(); i++) {
        configuration.output_layers.push_back(val["OutputLayers"][i].asString());
      }
      for (size_t i = 0; i < val["ResultLayers"].size(); i++) {
        configuration.result_layers.push_back(val["ResultLayers"][i].asString());
      }
      configuration.runtime = (RuntimeType)val.get("Runtime", 0).asInt();
      configuration.number_of_threads = val.get("NUM_THREADS", 2).asInt();
      configuration.use_nnapi = val.get("USE_NNAPI", 0).asInt();

      // Override the desired params to match previous version
      if (backward_compat) {
        output = (EngineOutput)val.get("Engine", 0).asInt();
        if (output == EngineOutput::kSqueezenet) {
          output = EngineOutput::kSingle;
        }
        configuration.model_file = val.get("DLC_NAME", "").asString();
        configuration.labels_file = val.get("LABELS_NAME", "").asString();
      }

      VAM_ML_LOGI("Model file:%s", configuration.model_file.c_str());
      VAM_ML_LOGI("Labels file:%s", configuration.labels_file.c_str());
      ret = VAM_OK;
    } else {
      VAM_ML_LOGE("%s: Failed JSON parsing!", __func__);
    }
    in.close();
  }

  return ret;
}

MLEngine* CreateEngine(const FrameworkType& framework,
                       const EngineOutput& output,
                       MLConfig &configuration) {
  bool is_valid = (configuration.target_fps);
  if (configuration.use_norm) {
    is_valid &= (configuration.blue_mean &&
                 configuration.green_mean &&
                 configuration.red_mean);
  }
  if (!configuration.model_file.size() ||
      !configuration.labels_file.size() ||
      !is_valid) {
    VAM_ML_LOGE("%s: Invalid parameter", __func__);
    return nullptr;
  }
  switch (framework) {
    case FrameworkType::kSNPE: {
      if (output == EngineOutput::kSingle) {
        SNPEBase* engine = new SNPEBase(configuration);
        if (!engine) {
          VAM_ML_LOGE("%s: Failed to create engine instance", __func__);
          return nullptr;
        }
        return engine;
      } else if  (output == EngineOutput::kMulti) {
        SNPEComplex* engine = new SNPEComplex(configuration);
        if (!engine) {
          VAM_ML_LOGE("%s: Failed to create engine instance", __func__);
          return nullptr;
        }
        return engine;
      } else if (output == EngineOutput::kSingleSSD) {
        SNPESingleSSD* engine = new SNPESingleSSD(configuration);
        if (!engine) {
          VAM_ML_LOGE("%s: Failed to create engine instance", __func__);
          return nullptr;
        }
        return engine;
      }
    }
    case FrameworkType::kTFLite: {
      TFLBase* engine = new TFLBase(configuration);
      if (!engine) {
        VAM_ML_LOGE("%s: Failed to create engine instance", __func__);
        return nullptr;
      }
      return engine;
    }
    default:
      VAM_ML_LOGE("%s: Unsupported framework %d", __func__,  framework);
      return nullptr;
  }
}

int32_t va_ml_engine_init(const struct vaapi_source_info* source_info,
                          void** handle) {
  VAM_ML_LOGI("%s: Enter", __func__);
  int32_t ret = VAM_OK;
  uint8_t backward_compat = Property::Get(PROP_BACKWARD_COMPAT, 0);
  if (backward_compat) {
    VAM_ML_LOGI("Using backward compatibility mode !");
  }

  MLConfig configuration;
  FrameworkType framework;
  EngineOutput output;
  std::string folder(&source_info->data_folder[0]);
  std::string file = folder + "/" + VA_ALG_INFO_SYM.name + "_config.json";
  if (backward_compat) {
    file = folder + "/va-snpe-engine-library_config.json";
  }
  std::ifstream config_file(file);
  if (!config_file.is_open()) {
    VAM_ML_LOGE("%s: Config file not valid.", __func__);
    return VAM_FAIL;
  }
  VAM_ML_LOGI("Using configuration file: %s", file.c_str());

  ret = ParseConfig(file.c_str(), framework, output, configuration,
                    backward_compat);
  if (VAM_OK != ret) {
    VAM_ML_LOGE("%s: Configuration parsing failed!", __func__);
    return ret;
  }

  MLEngine* engine = CreateEngine(framework, output, configuration);
  if (!engine) {
    VAM_ML_LOGE("%s: Failed to create engine instance.", __func__);
    return VAM_FAIL;
  }
  ret = engine->Init(source_info);
  if (VAM_OK != ret) {
    VAM_ML_LOGE("%s: EngineInit failed %d", __func__, ret);
    delete engine;
    return ret;
  }

  *handle = reinterpret_cast<void *>(engine);

  VAM_ML_LOGI("%s: Exit", __func__);
  return ret;
}

int32_t va_ml_engine_deinit(void* handle) {
  VAM_ML_LOGI("%s: Enter", __func__);

  if (!handle) {
    VAM_ML_LOGE("%s:%d Engine handle is null", __func__, __LINE__);
    return VAM_NULLPTR;
  }

  MLEngine *engine = reinterpret_cast<MLEngine*>(handle);
  engine->Deinit();
  delete engine;

  VAM_ML_LOGD("%s: Exit", __func__);
  return VAM_OK;
}

int32_t va_ml_engine_process(void* handle,
                             struct vaapi_frame_info* frame_info) {
  VAM_ML_LOGI("%s: Enter", __func__);
  int32_t ret = VAM_OK;

  if (!handle) {
    VAM_ML_LOGE("%s:%d Engine handle is null", __func__, __LINE__);
    return VAM_NULLPTR;
  }

  MLEngine *engine = reinterpret_cast<MLEngine*>(handle);
  ret = engine->Process(frame_info);
  if (VAM_OK != ret) {
    VAM_ML_LOGE("%s: EngineProcess failed: %d", __func__, ret);
    return ret;
  }

  VAM_ML_LOGI("%s: Exit", __func__);
  return ret;
}

int32_t va_ml_engine_add_rule(void* handle, vaapi_rule* rule) {
  return VAM_OK; // not supported
}

int32_t va_ml_engine_delete_rule(void* handle, const char* config_id) {
  return VAM_OK; // not supported
}

int32_t va_ml_engine_enroll_obj(void* handle,
                                struct vaapi_enrollment_info* enroll_info) {
  return VAM_OK; // not supported
}

int32_t va_ml_engine_disenroll_obj(void* handle, const char* obj_id) {
  return VAM_OK; // not supported
}

int32_t va_ml_engine_get_event_count(void* handle, uint32_t* event_count) {
  *event_count = 0;

  return VAM_OK;
}

int32_t va_ml_engine_get_events(void* handle, struct vaapi_event* events,
                                uint32_t num_events) {
  return VAM_OK; // not supported
}

int32_t va_ml_engine_get_obj_count(void* handle, uint32_t* obj_count) {
  if (!handle) {
    VAM_ML_LOGE("%s:%d Engine handle is null", __func__, __LINE__);
    return VAM_NULLPTR;
  }
  if (!obj_count) {
    VAM_ML_LOGE("%s:%d Object count is null", __func__, __LINE__);
    return VAM_NULLPTR;
  }

  MLEngine *engine = reinterpret_cast<MLEngine*>(handle);
  *obj_count = engine->GetObjectsCount();

  return VAM_OK;
}

int32_t va_ml_engine_get_objects(void* handle, vaapi_object* objects,
                                 uint32_t obj_count) {
  if (!handle) {
    VAM_ML_LOGE("%s:%d Engine handle is null", __func__, __LINE__);
    return VAM_NULLPTR;
  }
  if (!objects) {
    VAM_ML_LOGE("%s:%d Parameter is null", __func__, __LINE__);
    return VAM_NULLPTR;
  }

  MLEngine *engine = reinterpret_cast<MLEngine*>(handle);
  engine->GetObjectsInfo(objects, obj_count);

  return VAM_OK;
}

int32_t va_ml_engine_get_target_fps(void* handle, uint8_t* desired_fps) {
  if (!handle) {
    VAM_ML_LOGE("%s:%d Engine handle is null", __func__, __LINE__);
    return VAM_NULLPTR;
  }
  if (!desired_fps) {
    VAM_ML_LOGE("%s:%d Desired FPS is null", __func__, __LINE__);
    return VAM_NULLPTR;
  }

  MLEngine *engine = reinterpret_cast<MLEngine*>(handle);
  *desired_fps = engine->GetTargetFPS();

  return VAM_OK;
}

int32_t va_ml_engine_is_event_supported(void* handle, vaapi_event_type type,
                                        uint8_t* is_supported) {
  *is_supported = (vaapi_event_object_classified == type) ? 1 : 0;

  return VAM_OK;
}

int32_t va_ml_engine_is_depends_on_other_event(void* handle,
                                               vaapi_event_type type,
                                               uint8_t* is_dependent) {
  *is_dependent = 0;

  return VAM_OK;
}
