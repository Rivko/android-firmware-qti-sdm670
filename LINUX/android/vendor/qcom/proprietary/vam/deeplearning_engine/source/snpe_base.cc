/*
 * Copyright (c) 2018-2019, Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <fstream>
#include <vector>
#include <memory>
#include <mutex>
#include <sstream>
#include <chrono>

#include "snpe_base.h"

using namespace qmmf::qmmf_alg_plugin;
namespace mle {

static const auto kOneFrameProcessTimeout = std::chrono::milliseconds(10);

SNPEBase::SNPEBase(MLConfig &config) : MLEngine() {
  ConfigureRuntime(config);
  config_.io_type = config.io_type;
  config_.input_format = config.input_format;
  config_.scale_width = config.scale_width;
  config_.scale_height = config.scale_height;
  config_.blue_mean = config.blue_mean;
  config_.blue_sigma = config.blue_sigma;
  config_.green_mean = config.green_mean;
  config_.green_sigma = config.green_sigma;
  config_.red_mean = config.red_mean;
  config_.red_sigma = config.red_sigma;
  config_.use_norm = config.use_norm;
  config_.target_fps = config.target_fps;
  config_.model_file = config.model_file;
  config_.labels_file = config.labels_file;
  config_.input_layers = config.input_layers;
  config_.output_layers = config.output_layers;
  config_.result_layers = config.result_layers;
  init_params_.conf_threshold = config.conf_threshold;
}

int32_t SNPEBase::ConfigureRuntime(MLConfig &config) {
  version_ = zdl::SNPE::SNPEFactory::getLibraryVersion();

  switch (config.runtime) {
    case RuntimeType::DSP: {
      if (zdl::SNPE::SNPEFactory::isRuntimeAvailable(
          zdl::DlSystem::Runtime_t::DSP)) {
        runtime_ = zdl::DlSystem::Runtime_t::DSP;
        VAM_ML_LOGI("DSP runtime selected");
      } else {
        runtime_ = zdl::DlSystem::Runtime_t::CPU;
        VAM_ML_LOGI("CPU runtime selected, but DSP was configured");
      }
      break;
    }
    case RuntimeType::CPU: {
      runtime_ = zdl::DlSystem::Runtime_t::CPU;
      VAM_ML_LOGI("CPU runtime selected");
      break;
    }
  }
  return VAM_OK;
}

std::unique_ptr<zdl::SNPE::SNPE> SNPEBase::SetBuilderOptions() {
  VAM_ML_LOGI("%s: Enter", __func__);
  std::unique_ptr <zdl::SNPE::SNPE> snpe;
  zdl::SNPE::SNPEBuilder snpeBuilder(snpe_params_.container.get());
  zdl::DlSystem::StringList output_layers;

  for (size_t i = 0; i < config_.output_layers.size(); i++) {
    output_layers.append(config_.output_layers[i].c_str());
  }

  if (config_.io_type == NetworkIO::kUserBuffer) {
    snpe =
        snpeBuilder.setOutputLayers(output_layers).setRuntimeProcessor(runtime_)
            .setUseUserSuppliedBuffers(true).setCPUFallbackMode(true).build();
  } else if (config_.io_type == NetworkIO::kITensor) {
    snpe =
        snpeBuilder.setOutputLayers(output_layers).setRuntimeProcessor(runtime_)
            .setUseUserSuppliedBuffers(false).setCPUFallbackMode(true).build();
  } else {
    VAM_ML_LOGE("%s: Invalid Network IO value", __func__);
    throw std::runtime_error("Invalid Network IO value");
  }

  VAM_ML_LOGI("%s: Exit", __func__);
  return snpe;
}

std::unique_ptr<zdl::DlContainer::IDlContainer> SNPEBase::LoadContainerFromFile(
    std::string container_path) {
  std::unique_ptr<zdl::DlContainer::IDlContainer> container;
  container = zdl::DlContainer::IDlContainer::open(container_path);
  if (nullptr == container) {
    VAM_ML_LOGE("%s: Container loading failed", __func__);
    return nullptr;
  }

  return container;
}

int32_t SNPEBase::PopulateMap(BufferType type) {
  int32_t result = VAM_OK;
  zdl::DlSystem::Optional <zdl::DlSystem::StringList> names_opt;

  switch (type) {
    case BufferType::kInput:
      names_opt = snpe_params_.snpe->getInputTensorNames();
      break;
    case BufferType::kOutput:
      names_opt = snpe_params_.snpe->getOutputTensorNames();
      break;
    default:
      VAM_ML_LOGE("Error obtaining tensor names");
      throw std::runtime_error("Error obtaining tensor names");
  }

  const zdl::DlSystem::StringList& names = *names_opt;
  for (const char *name : names) {
    if (config_.io_type == NetworkIO::kUserBuffer) {
      result = CreateUserBuffer(type, name);
    } else if (config_.io_type == NetworkIO::kITensor) {
      result = CreateTensor(type, name);
    } else {
      VAM_ML_LOGE("Invalid Network IO value %d", config_.io_type);
      result = VAM_FAIL;
    }

    if (VAM_OK != result) {
      break;
    }
  }
  return result;
}

int32_t SNPEBase::CreateUserBuffer(BufferType type, const char * name) {
  zdl::DlSystem::IUserBufferFactory& ub_factory =
      zdl::SNPE::SNPEFactory::getUserBufferFactory();
  zdl::DlSystem::UserBufferEncodingFloat ub_encoding_float;
  zdl::DlSystem::UserBufferEncodingTf8 *ub_encoding_uint8 =
      new zdl::DlSystem::UserBufferEncodingTf8(0, 1 / 128);

  auto uba_opt = snpe_params_.snpe->getInputOutputBufferAttributes(name);
  if (!uba_opt) {
    throw std::runtime_error(
        std::string("Error obtaining attributes for tensor ") + name);
  }
  const zdl::DlSystem::TensorShape& buffer_shape = (*uba_opt)->getDims();

  size_t elem_size = sizeof(uint8_t);
  if ((config_.input_format == InputFormat::kBgrFloat) ||
      (config_.input_format == InputFormat::kRgbFloat)) {
    elem_size = sizeof(float);
  }

  size_t buf_size = CalculateSizeFromDims(buffer_shape.rank(),
                                          buffer_shape.getDimensions(),
                                          elem_size);

  auto *heap_map = &snpe_params_.in_heap_map;
  auto *ub_map = &snpe_params_.input_ub_map;
  if (type == BufferType::kOutput) {
    heap_map = &snpe_params_.out_heap_map;
    ub_map = &snpe_params_.output_ub_map;
  }

  IONBuffer ion_buf = AllocateBuffer(buf_size, config_.input_format);
  if (nullptr == ion_buf.addr && nullptr == ion_buf.addr_f) {
    VAM_ML_LOGE(" Buffer allocation failed");
    ReleaseBuffer(ion_buf);
    return VAM_FAIL;
  }

  heap_map->emplace(name, ion_buf);

  if ((config_.input_format == InputFormat::kBgr) ||
      (config_.input_format == InputFormat::kRgb)) {
    snpe_params_.ub_list.push_back(ub_factory.createUserBuffer(
        ion_buf.addr, buf_size,
        GetStrides((*uba_opt)->getDims(), elem_size), ub_encoding_uint8));
    ub_map->add(name, snpe_params_.ub_list.back().get());
    PrintErrorStringAndExit();
  } else {
    snpe_params_.ub_list.push_back(ub_factory.createUserBuffer(
        ion_buf.addr_f, buf_size,
        GetStrides((*uba_opt)->getDims(), elem_size), &ub_encoding_float));
    ub_map->add(name, snpe_params_.ub_list.back().get());
    PrintErrorStringAndExit();
  }

  return VAM_OK;
}

int32_t SNPEBase::CreateTensor(BufferType type, const char* name) {
  zdl::DlSystem::ITensorFactory& tensor_factory =
      zdl::SNPE::SNPEFactory::getTensorFactory();

  auto tensor_opt = snpe_params_.snpe->getInputOutputBufferAttributes(name);
  if (!tensor_opt) {
    throw std::runtime_error(
        std::string("Error obtaining attributes for tensor ") + name);
  }
  const zdl::DlSystem::TensorShape& tensor_shape = (*tensor_opt)->getDims();
  size_t buf_size = CalculateSizeFromDims(tensor_shape.rank(),
                                          tensor_shape.getDimensions(),
                                          sizeof(float));

  auto *heap_map = &snpe_params_.in_heap_map;
  auto *tensor_map = &snpe_params_.input_tensor_map;
  if (type == BufferType::kOutput) {
    heap_map = &snpe_params_.out_heap_map;
    tensor_map = &snpe_params_.output_tensor_map;
  }

  IONBuffer ion_buf = AllocateBuffer(buf_size, config_.input_format);
  if (nullptr == ion_buf.addr && nullptr == ion_buf.addr_f) {
    VAM_ML_LOGE(" Buffer allocation failed");
    ReleaseBuffer(ion_buf);
    return VAM_FAIL;
  }

  heap_map->emplace(name, ion_buf);
  snpe_params_.tensor_list.push_back(tensor_factory.createTensor(tensor_shape));
  tensor_map->add(name, snpe_params_.tensor_list.back().get());

  return VAM_OK;
}

size_t SNPEBase::CalculateSizeFromDims(const size_t rank,
                                       const zdl::DlSystem::Dimension* dims,
                                       const size_t& element_size) {
  if (0 == rank) {
    return 0;
  }
  size_t size = element_size;
  for (size_t i = 0; i < rank; i++) {
    size *= dims[i];
  }
  return size;
}

std::vector<size_t> SNPEBase::GetStrides(zdl::DlSystem::TensorShape dims,
                                         const size_t& element_size) {
  std::vector<size_t> strides(dims.rank());
  strides[strides.size() - 1] = element_size;
  size_t stride = strides[strides.size() - 1];

  for (size_t i = dims.rank() - 1; i > 0; i--) {
    stride *= dims[i];
    strides[i - 1] = stride;
  }

  return strides;
}

int32_t SNPEBase::PreProcessBuffer(const vaapi_frame_info* frame_info) {
  VAM_ML_LOGI("%s: Enter", __func__);
  IONBuffer *ion_buf = &snpe_params_.in_heap_map[config_.input_layers.c_str()];

  if (!ion_buf) {
    VAM_ML_LOGE("%s: SNPE buffer is null", __func__);
    return VAM_NULLPTR;
  }

  std::vector<AlgBuffer> inb;
  if (alg_input_buffers_map_.find(frame_info->fd) ==
        alg_input_buffers_map_.end()) {
    AlgBuffer in_buf;
    BufferPlane plane0(init_params_.width,
                       init_params_.height,
                       init_params_.stride,
                       0,
                       init_params_.stride * init_params_.scanline);

    BufferPlane plane1(init_params_.width,
                       init_params_.height / 2,
                       init_params_.stride,
                       plane0.length_,
                       init_params_.stride * init_params_.scanline / 2);

    in_buf.plane_.push_back(plane0);
    in_buf.plane_.push_back(plane1);
    in_buf.cached_ = false;
    in_buf.pix_fmt_ = PixelFormat::kNv12;
    in_buf.size_ = plane0.length_ + plane1.length_;
    in_buf.vaddr_ = frame_info->frame_l_data[0];
    in_buf.fd_ = frame_info->fd;

    alg_input_buffers_map_[frame_info->fd] = in_buf;
    inb.push_back(in_buf);
    algo_->RegisterInputBuffers(inb);
  } else {
    inb.push_back(alg_input_buffers_map_[frame_info->fd]);
  }

  if (alg_output_buffers_.size() == 0) {
    AlgBuffer algBuf;
    ion_buf->GetAlgBuffer(&algBuf, config_.input_format);
    alg_output_buffers_.push_back(algBuf);
    algo_->RegisterOutputBuffers(alg_output_buffers_);
  }

  {
    std::unique_lock<std::mutex> l(lock_);
    algo_process_done_ = false;
  }
  try {
    algo_->Process(inb, alg_output_buffers_);
  }  catch (const std::exception &e) {
    VAM_ML_LOGE("Process failed: %s", e.what());
    return VAM_FAIL;
  }

  {
    std::unique_lock<std::mutex> l(lock_);
    auto rc = signal_.wait_for(l, kOneFrameProcessTimeout, [&] {
      return algo_process_done_;
    });
    if (!rc) {
      VAM_ML_LOGE("algorithm process timed out");
      return VAM_FAIL;
    }
  }
  if (config_.io_type == NetworkIO::kITensor) {
    auto *tensor =
        snpe_params_.input_tensor_map.getTensor(config_.input_layers.c_str());
    if ((config_.input_format == InputFormat::kBgr) ||
        (config_.input_format == InputFormat::kRgb)) {
      if (ion_buf->addr != nullptr) {
        uint8_t *t = ion_buf->addr;
        for (auto it = tensor->begin(); it != tensor->end(); it++, t++) {
          *it = *t;
        }
      }
    } else {
      if (ion_buf->addr_f != nullptr) {
        float *t = ion_buf->addr_f;
        for (auto it = tensor->begin(); it != tensor->end(); it++, t++) {
          *it = *t;
        }
      }
    }
  }

  VAM_ML_LOGI("%s: Exit", __func__);
  return VAM_OK;
}

int32_t SNPEBase::ExecuteSNPE() {
  VAM_ML_LOGI("%s: Enter", __func__);

  if (config_.io_type == NetworkIO::kUserBuffer) {
    if (!snpe_params_.snpe->execute(snpe_params_.input_ub_map,
                                    snpe_params_.output_ub_map)) {
      PrintErrorStringAndExit();
      return VAM_FAIL;
    }
  } else if (config_.io_type == NetworkIO::kITensor) {
    snpe_params_.output_tensor_map.clear();
    if (!snpe_params_.snpe->execute(snpe_params_.input_tensor_map,
                                    snpe_params_.output_tensor_map)) {
      PrintErrorStringAndExit();
      return VAM_FAIL;
    }
  } else {
    VAM_ML_LOGE("%s: Invalid Network IO value", __func__);
    return VAM_FAIL;
  }

  VAM_ML_LOGI("%s: Exit", __func__);
  return VAM_OK;
}

int32_t SNPEBase::EnginePostProcess(SNPEExecuteOutParams& execute_params) {
  VAM_ML_LOGI("%s: Enter", __func__);

  std::vector<float> score_buf;
  const zdl::DlSystem::StringList &output_buf_names =
      snpe_params_.output_ub_map.getUserBufferNames();
  const zdl::DlSystem::StringList &output_tensor_names =
      snpe_params_.output_tensor_map.getTensorNames();
  const zdl::DlSystem::StringList *output_names = &output_buf_names;
  if (config_.io_type == NetworkIO::kITensor) {
    output_names = &output_tensor_names;
  }
  std::for_each(
      output_names->begin(),
      output_names->end(),
      [&](const char* name)
      {
        if (0 == std::strcmp(name, config_.result_layers[0].c_str())) {
          if (config_.io_type == NetworkIO::kUserBuffer) {
            IONBuffer b = snpe_params_.out_heap_map.at(name);
            if ((config_.input_format == InputFormat::kBgr) ||
               (config_.input_format == InputFormat::kRgb)) {
              for (size_t i = 0; i < b.size / sizeof(uint8_t); i++) {
                score_buf.push_back(b.addr[i]);
              }
            } else {
              for (size_t i = 0; i < b.size / sizeof(float); i++) {
                score_buf.push_back(b.addr_f[i]);
              }
            }
          } else if (config_.io_type == NetworkIO::kITensor) {
            auto t = snpe_params_.output_tensor_map.getTensor(name);
            for (auto it = t->begin(); it != t->end(); it++) {
              score_buf.push_back(*it);
            }
          }
        }
      });

  uint32_t top_score_idx = 0;
  float top_score = 0.0;

  for (size_t i = 0; i < score_buf.size(); i++) {
    if (score_buf[i] > top_score) {
      top_score = score_buf[i];
      top_score_idx = i;
    }
  }
  if (top_score_idx < labels_.size()) {
    execute_params.prob.push_back(top_score);
    execute_params.labels.push_back(labels_.at(top_score_idx));
  }

  VAM_ML_LOGI("%s: Exit", __func__);
  return VAM_OK;
}

int32_t SNPEBase::Process(struct vaapi_frame_info* frame_info) {

  int32_t result = VAM_OK;
  out_params_.num_objects = 0;

  result = PreProcessBuffer(frame_info);
  if (VAM_OK != result) {
    VAM_ML_LOGE(" PreProcessBuffer failed");
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
    vaapi_object* object = &out_params_.objects[0];

    memset(object, 0, sizeof(vaapi_object));

    // Post processing section, pick only the highest probability
    if (execute_params.prob.size() &&
        execute_params.prob[0] > init_params_.conf_threshold) {
      auto rc = std::string(execute_params.labels[0]).copy(
          object->display_name, sizeof(object->display_name) - 1);
      object->display_name[rc] = '\0';
      object->confidence = static_cast<uint8_t>(execute_params.prob[0] * 100);
      object->type = GetTypeFromClassLabel(execute_params.labels[0].c_str());

      out_params_.num_objects = 1;

      VAM_ML_LOGI("SNPE engine classified input as %s, with confidence %d",
                    object->display_name, object->confidence);
    }
  }

  return result;
}

vaapi_object_type SNPEBase::GetTypeFromClassLabel(const char* /*class_label*/) {
  return vaapi_object_type_unknown;
}

void SNPEBase::PrintErrorStringAndExit() {
  const char* const err = zdl::DlSystem::getLastErrorString();
  VAM_ML_LOGE(" %s", err);
}

int32_t SNPEBase::Init(const struct vaapi_source_info* source_info) {
  VAM_ML_LOGI("%s: Enter", __func__);
  int32_t res = VAM_OK;
  ion_device_ = open("/dev/ion", O_RDONLY);
  if (ion_device_ < 0) {
    VAM_ML_LOGE("Open ion device failed");
    return VAM_FAIL;
  }

  init_params_.width = source_info->frame_l_width[0];
  init_params_.height = source_info->frame_l_height[0];
  init_params_.stride = source_info->frame_l_pitch[0];
  init_params_.scanline = source_info->frame_l_scanline[0];
  init_params_.format = source_info->img_format;


  scale_stride_ = config_.scale_width * 3 * 4;

  if ((config_.input_format == InputFormat::kBgr) ||
      (config_.input_format == InputFormat::kRgb)) {
    scale_stride_ = config_.scale_width * 3;
  }

  std::string configuration_data(AlgoConfiguration());

  try {
    InitAlgo();
    algo_->SetCallbacks(this);
    algo_->Configure(configuration_data);
  } catch (const std::exception &e) {
    DeinitAlgo();
    VAM_ML_LOGE("Algo failed: %s", e.what());
    return VAM_FAIL;
  }

  res = InitSNPE(source_info);

  VAM_ML_LOGD("%s: Exit", __func__);
  return res;
}

int32_t SNPEBase::InitSNPE(const struct vaapi_source_info* source_info) {
  int32_t res = VAM_OK;

  std::string folder(&source_info->data_folder[0]);
  std::string dlc = folder + "/" + config_.model_file;
  std::ifstream dlc_file(dlc);
  if (!dlc_file.is_open()) {
    VAM_ML_LOGE(" dlc files not valid");
    return VAM_FAIL;
  }

  snpe_params_.container = LoadContainerFromFile(dlc);
  if (nullptr == snpe_params_.container) {
    PrintErrorStringAndExit();
    res = VAM_FAIL;
  } else {
    snpe_params_.snpe = SetBuilderOptions();
    if (nullptr == snpe_params_.snpe) {
      PrintErrorStringAndExit();
      res = VAM_FAIL;
    }
  }

  if (VAM_OK == res) {
    std::string labels_filename = folder + "/" + config_.labels_file;
    std::ifstream labels_file(labels_filename);

    if (!labels_file.is_open()) {
      VAM_ML_LOGE(" Labels files not valid");
      res = VAM_FAIL;
    } else {
      for (std::string a; std::getline(labels_file, a);) {
        labels_.push_back(a);
      }
    }
  }

  if (VAM_OK == res) {
    res = PopulateMap(BufferType::kInput);
  }
  if (VAM_OK == res) {
    res = PopulateMap(BufferType::kOutput);
  }
  return res;
}

void SNPEBase::Deinit() {

  std::vector<AlgBuffer> inb;
  for (auto &it : alg_input_buffers_map_) {
    inb.push_back(it.second);
  }
  algo_->UnregisterInputBuffers(inb);
  algo_->UnregisterOutputBuffers(alg_output_buffers_);
  alg_input_buffers_map_.clear();
  alg_output_buffers_.clear();
  for (auto it : snpe_params_.in_heap_map) {
    ReleaseBuffer(it.second);
  }
  for (auto it : snpe_params_.out_heap_map) {
    ReleaseBuffer(it.second);
  }
  close(ion_device_);

  DeinitAlgo();
}

int32_t SNPEBase::GetObjectsInfo(vaapi_object* objects, uint32_t obj_count) {
  std::memcpy(objects, &out_params_.objects[0],
              obj_count * sizeof(vaapi_object));

  return VAM_OK;
}

void IONBuffer::GetAlgBuffer(AlgBuffer* buf, const InputFormat& format) {
  BufferPlane plane(this->width,
                    this->height,
                    this->stride,
                    0,
                    this->size);
  buf->cached_ = false;
  buf->fd_ = this->fd;
  buf->size_ = this->size;
  buf->plane_.push_back(plane);
  switch (format) {
    case InputFormat::kBgrFloat:
      buf->pix_fmt_ = PixelFormat::kBgrFloat;
      break;
    case InputFormat::kRgbFloat:
      buf->pix_fmt_ = PixelFormat::kRgbFloat;
      break;
    case InputFormat::kBgr:
      buf->pix_fmt_ = PixelFormat::kBgr24;
      break;
    case InputFormat::kRgb:
      buf->pix_fmt_ = PixelFormat::kRgb24;
      break;
    default:
      throw std::runtime_error("Format is not supported");
  }

  if ((format == InputFormat::kBgr) ||
      (format == InputFormat::kRgb)) {
    buf->vaddr_ = this->addr;
  } else {
    buf->vaddr_ = reinterpret_cast<uint8_t*>(this->addr_f);
  }

  VAM_ML_LOGE(
      "Buffer info: fd %d, vaddr %p, size %d w %d h %d, stride %d, size %d",
      buf->fd_, buf->vaddr_, buf->size_, this->width, this->height,
      this->stride, this->size);
}

IONBuffer SNPEBase::AllocateBuffer(const uint32_t& size,
                                   const InputFormat& input_format) {
  IONBuffer buf;

  struct ion_allocation_data alloc;
  memset(&alloc, 0, sizeof(alloc));
  alloc.len = size;
  alloc.align = 0;
  alloc.heap_id_mask = 0x1 << ION_IOMMU_HEAP_ID;
  alloc.flags = ION_FLAG_CACHED;

  int32_t rc = ioctl(ion_device_, ION_IOC_ALLOC, &alloc);
  if (rc < 0) {
    VAM_ML_LOGE("ION alloc failed");
    throw std::runtime_error("ION alloc failed");
  }

  struct ion_fd_data ion_info_fd;
  memset(&ion_info_fd, 0, sizeof(ion_info_fd));
  ion_info_fd.handle = alloc.handle;
  buf.handle = alloc.handle;

  rc = ioctl(ion_device_, ION_IOC_SHARE, &ion_info_fd);
  if (rc < 0) {
    VAM_ML_LOGE("ION map call failed");
    throw std::runtime_error("ION map call failed");
  }

  buf.fd = ion_info_fd.fd;
  if ((input_format == InputFormat::kBgr) ||
      (input_format == InputFormat::kRgb)) {
    buf.addr = static_cast<uint8_t*>(
      mmap(NULL, alloc.len, PROT_READ | PROT_WRITE, MAP_SHARED, buf.fd, 0));
    if (buf.addr == MAP_FAILED ) {
      VAM_ML_LOGE("mmap call failed");
      throw std::runtime_error("mmap call failed");
    }
  } else {
    buf.addr_f = static_cast<float*>(
      mmap(NULL, alloc.len, PROT_READ | PROT_WRITE, MAP_SHARED, buf.fd, 0));
    if (buf.addr_f == MAP_FAILED ) {
      VAM_ML_LOGE("mmap call failed");
      throw std::runtime_error("mmap call failed");
    }
  }

  buf.size = size;
  buf.width = config_.scale_width;
  buf.height = config_.scale_height;
  buf.stride = scale_stride_;

  return buf;
}

void SNPEBase::ReleaseBuffer(const IONBuffer& buf) {
  if (config_.input_format == InputFormat::kBgr ||
      config_.input_format == InputFormat::kRgb) {
    if (MAP_FAILED != buf.addr) {
      munmap(buf.addr, buf.size);
    }
  } else {
    if (MAP_FAILED != buf.addr_f) {
      munmap(buf.addr_f, buf.size);
    }
  }

  if (-1 != buf.fd) {
    close(buf.fd);
  }
  if (-1 != buf.handle) {
    struct ion_handle_data handle_data;
    memset(&handle_data, 0, sizeof(handle_data));
    handle_data.handle = buf.handle;
    ioctl(ion_device_, ION_IOC_FREE, &handle_data);
  }
}

void SNPEBase::InitAlgo() {
  try {
    auto alg_lib_folder = Utils::GetAlgLibFolder();
    Utils::LoadLib(alg_lib_folder + "libqmmf_alg_res_conv_sub.so",
                   algo_lib_handle_);

    QmmfAlgLoadPlugin LoadPluginFunc;
    Utils::LoadLibHandler(algo_lib_handle_, "QmmfAlgoNew", LoadPluginFunc);

    std::vector<uint8_t> calibration_data;
    algo_ = LoadPluginFunc(calibration_data, *this);
  } catch (const std::exception &e) {
    DeinitAlgo();
    VAM_ML_LOGE("Algo failed: %s", e.what());
    throw std::runtime_error("Init algo failed");
  }
}

void SNPEBase::DeinitAlgo() {
  if (nullptr != algo_) {
    delete algo_;
    algo_ = nullptr;
  }

  Utils::UnloadLib(algo_lib_handle_);
}

std::string SNPEBase::AlgoConfiguration() const {
  std::stringstream stream;

  stream << "{\n  \"BlueChannelMean\" : " << config_.blue_mean << ",\n";
  stream << " \"GreenChannelMean\" : " << config_.green_mean << ",\n";
  stream << " \"RedChannelMean\" : " << config_.red_mean << ",\n";
  stream << " \"UseNorm\" : " << config_.use_norm << ",\n";
  stream << " \"BlueChannelDivisor\" : " << config_.blue_sigma << ",\n";
  stream << " \"GreenChannelDivisor\" : " << config_.green_sigma << ",\n";
  stream << " \"RedChannelDivisor\" : " << config_.red_sigma << ",\n";
  stream << " \"KeepFieldOfView\" : " << 1 << ",\n";
  stream << " \"InputWidth\" : " << init_params_.width << ",\n ";
  stream << " \"InputHeight\" : " << init_params_.height << "\n ";
  stream << "}\n";

  return stream.str();
}

void SNPEBase::OnFrameReady(const AlgBuffer &output_buffer) {
  std::unique_lock<std::mutex> l(lock_);
  algo_process_done_ = true;
  signal_.notify_one();
}

}; // namespace mle
