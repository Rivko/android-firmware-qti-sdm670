/*
 * Copyright (c) 2018-2019, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#pragma once

#include <cstdlib>
#include <vector>
#include <string>
#include <iterator>
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <malloc.h>
#include <math.h>
#include <memory>
#include <cstring>
#include <unordered_map>
#include <algorithm>
#include <fcntl.h>
#include <condition_variable>
#include <mutex>
#include <linux/msm_ion.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include "DlContainer/IDlContainer.hpp"
#include "SNPE/SNPE.hpp"
#include "SNPE/SNPEFactory.hpp"
#include "SNPE/SNPEBuilder.hpp"
#include "DlSystem/DlError.hpp"
#include "DlSystem/ITensorFactory.hpp"
#include "DlSystem/TensorMap.hpp"
#include "DlSystem/TensorShape.hpp"
#include "DlSystem/StringList.hpp"
#include "DlSystem/DlError.hpp"
#include "DlSystem/IUserBuffer.hpp"
#include "DlSystem/IUserBufferFactory.hpp"
#include "DlSystem/UserBufferMap.hpp"
#include "DlSystem/IBufferAttributes.hpp"
#include "VAM/vaapi_common.h"
#include "VAM/vaapi_errors.h"
#include "common_utils.h"
#include "ml_engine_intf.h"

namespace mle {

struct IONBuffer {
  uint8_t* addr = nullptr;
  float* addr_f = nullptr;
  uint32_t size;
  int32_t fd;
  int32_t handle;
  bool cached;
  uint32_t width;
  uint32_t height;
  uint32_t stride;
  void GetAlgBuffer(qmmf::qmmf_alg_plugin::AlgBuffer* buf,
                    const InputFormat& format);
};

struct InitParams {
  uint32_t width;
  uint32_t height;
  uint32_t stride;
  uint32_t scanline;
  vaapi_img_format format;
  float conf_threshold;
};

struct SNPEParams {
  std::unique_ptr<zdl::DlContainer::IDlContainer> container;
  std::unique_ptr<zdl::SNPE::SNPE> snpe;

  std::vector<std::unique_ptr<zdl::DlSystem::IUserBuffer>> ub_list;
  zdl::DlSystem::UserBufferMap output_ub_map;
  zdl::DlSystem::UserBufferMap input_ub_map;

  std::vector<std::unique_ptr<zdl::DlSystem::ITensor>> tensor_list;
  zdl::DlSystem::TensorMap output_tensor_map;
  zdl::DlSystem::TensorMap input_tensor_map;

  std::unordered_map<std::string, IONBuffer> in_heap_map;
  std::unordered_map<std::string, IONBuffer> out_heap_map;
};

struct SNPEExecuteOutParams {
  std::vector<float> prob;
  std::vector<std::string> labels;
  std::vector<std::vector<float>> boxes;
};

class SNPEBase : public qmmf::qmmf_alg_plugin::IEventListener, public MLTools,
                 public MLEngine {
 public:
  SNPEBase(MLConfig &config);
  virtual ~SNPEBase(){};
  int32_t Init(const struct vaapi_source_info* source_info);
  void Deinit();
  virtual int32_t Process(struct vaapi_frame_info* frame_info);
  virtual int32_t GetObjectsInfo(vaapi_object* objects, uint32_t obj_count);

 protected:
  int32_t PreProcessBuffer(const struct vaapi_frame_info* frame_info);
  void PrintErrorStringAndExit();
  vaapi_object_type GetTypeFromClassLabel(const char* class_label);

  IONBuffer AllocateBuffer(const uint32_t& size,
                           const InputFormat& input_format);
  void ReleaseBuffer(const IONBuffer& buf);

  int32_t ExecuteSNPE();
  virtual int32_t EnginePostProcess(SNPEExecuteOutParams& execute_params);

  std::vector<std::string> labels_;
  int32_t ion_device_;

  uint32_t scale_stride_;

  InitParams init_params_;
  SNPEParams snpe_params_;
  zdl::DlSystem::Runtime_t runtime_;
  zdl::DlSystem::Version_t version_;

 private:
  int32_t ConfigureRuntime(MLConfig &config);
  std::unique_ptr<zdl::DlContainer::IDlContainer> LoadContainerFromFile(
      std::string container_path);
  std::unique_ptr<zdl::SNPE::SNPE> SetBuilderOptions();
  virtual size_t CalculateSizeFromDims(const size_t rank,
                                       const zdl::DlSystem::Dimension* dims,
                                       const size_t& element_size);
  virtual std::vector<size_t> GetStrides(zdl::DlSystem::TensorShape dims,
                                         const size_t& element_size);

  int32_t PopulateMap(BufferType type);
  int32_t CreateUserBuffer(BufferType type, const char* name);
  int32_t CreateTensor(BufferType type, const char* name);
  int32_t InitSNPE(const struct vaapi_source_info* source_info);

  void InitAlgo();
  void DeinitAlgo();
  void *algo_lib_handle_;
  qmmf::qmmf_alg_plugin::IAlgPlugin *algo_;
  std::map<int32_t, qmmf::qmmf_alg_plugin::AlgBuffer> alg_input_buffers_map_;
  std::vector<qmmf::qmmf_alg_plugin::AlgBuffer> alg_output_buffers_;
  std::condition_variable signal_;
  std::mutex lock_;
  bool algo_process_done_;
  std::string AlgoConfiguration() const;

  // qmmf::qmmf_alg_plugin::IEventListener methods
  void OnFrameReady(const qmmf::qmmf_alg_plugin::AlgBuffer &output_buffer);
  void OnFrameProcessed(const qmmf::qmmf_alg_plugin::AlgBuffer &input_buffer) {};
  void OnError(qmmf::qmmf_alg_plugin::RuntimeError err) {};

};

}; // namespace mle
