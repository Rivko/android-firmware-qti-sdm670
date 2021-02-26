/*
 * Copyright (c) 2019, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include <sstream>
#include <string>
#include <memory>

#include <cutils/properties.h>
#include <utils/Log.h>
#include <qmmf-alg/qmmf_alg_plugin.h>
#include <qmmf-alg/qmmf_alg_utils.h>
#include "VAM/vaapi_errors.h"

//TODO make debug logs properly
#define DEBUG

#ifdef DEBUG
#define VAM_ML_LOGD(...) ALOGD("MLWrapper: " __VA_ARGS__)
#define VAM_ML_LOGI(...) ALOGI("MLWrapper: " __VA_ARGS__)
#define VAM_ML_LOGE(...) ALOGE("MLWrapper: " __VA_ARGS__)
#else
#define VAM_ML_LOGD(...) do {} while (0)
#define VAM_ML_LOGI(...) do {} while (0)
#define VAM_ML_LOGE(...) do {} while (0)
#endif

class Property {
 public:
  /** Get
   *    @property: property
   *    @default_value: default value
   *
   * Gets requested property value
   *
   * return: property value
   **/
  template <typename TProperty>
  static TProperty Get(std::string property, TProperty default_value) {
    TProperty value = default_value;
    char prop_val[PROPERTY_VALUE_MAX];
    std::stringstream s;
    s << default_value;
    property_get(property.c_str(), prop_val, s.str().c_str());

    std::stringstream output(prop_val);
    output >> value;
    return value;
  }

  /** Set
   *    @property: property
   *    @value: value
   *
   * Sets requested property value
   *
   * return: nothing
   **/
  template <typename TProperty>
  static void Set(std::string property, TProperty value) {
    std::stringstream s;
    s << value;
    std::string value_string = s.str();
    value_string.resize(PROPERTY_VALUE_MAX);
    property_set(property.c_str(), value_string.c_str());
  }
};

class MLTools : public qmmf::qmmf_alg_plugin::ITools {
 public:
  ~MLTools(){};

  void SetProperty(std::string property, std::string value) {
    Property::Set(property, value);
  }

  void SetProperty(std::string property, int32_t value) {
    Property::Set(property, value);
  }

  const std::string GetProperty(std::string property,
                                std::string default_value) {
    return Property::Get(property, default_value);
  }

  uint32_t GetProperty(std::string property, int32_t default_value) {
    return Property::Get(property, default_value);
  }

  void LogError(const std::string &s) { ALOGE("%s", s.c_str()); }

  void LogWarning(const std::string &s) { ALOGW("%s", s.c_str()); }

  void LogInfo(const std::string &s) { ALOGI("%s", s.c_str()); }

  void LogDebug(const std::string &s) { ALOGD("%s", s.c_str()); }

  void LogVerbose(const std::string &s) { ALOGV("%s", s.c_str()); }

  /*TODO add proper implementation of below methods
   * Note: Currently, ML unified engine is responsible for buffer management
   */
  std::shared_ptr<qmmf::qmmf_alg_plugin::IBufferHolder> ImportBufferHolder(
      const uint8_t *vaddr, const int32_t fd, const uint32_t size,
      const bool cached) {
    std::shared_ptr<qmmf::qmmf_alg_plugin::IBufferHolder> rv = nullptr;
    return rv;
  }

  std::shared_ptr<qmmf::qmmf_alg_plugin::IBufferHolder> NewBufferHolder(
      const uint32_t size, const bool cached) {
    std::shared_ptr<qmmf::qmmf_alg_plugin::IBufferHolder> rv = nullptr;
    return rv;
  }
};
