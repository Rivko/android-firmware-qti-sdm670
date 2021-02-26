/**
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <gpqese-be.h>

using namespace android;

static TEEC_Result result = TEEC_SUCCESS;
static TEEC_Context context;
static TEEC_Session session;
static TEEC_Operation operation;
static TEEC_SharedMemory input;
static TEEC_SharedMemory output;
static bool contextInitialised;
static bool sessionOpen;
static bool iseSEGPOpen;
static const bool DBG = false;
static uint32_t resultOrigin;
static std::mutex m;

using ::vendor::qti::esepowermanager::V1_0::IEsePowerManager;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::hidl::base::V1_0::IBase;

static sp<IEsePowerManager> mHal;

static bool esepmInit()
{
    if (mHal != nullptr) {
        return true;
    }
    mHal = IEsePowerManager::getService();
    return (mHal != nullptr);
}

struct notifyDeath : IBase {
  void notify() { ALOGD("Death notified\n"); }
};

static int nativeeSEPowerOn () {
  int ret = -1;
  ALOGD("nativeeSEPowerON");
  sp<notifyDeath> callback = new notifyDeath();
  if (callback == nullptr) {
    ALOGE("NULL callback");
    return ret;
  }
  if (esepmInit()) {
    return (mHal->powerOn(callback));
  } else {
    ALOGE("mHal is null");
    return ret;
  }

}

static int nativeeSEPowerOff() {
  int ret = -1;
  ALOGD("nativeeSEPowerOff");
  if (esepmInit()) {
    return (mHal->powerOff() == 0);
  } else {
    ALOGE("mHal is null");
    return ret;
  }
}

static int nativeeSEGetState() {
  int ret = -1;
  ALOGD("nativeeSEGetState");
  if (esepmInit()) {
    return (mHal->getState());
  } else {
    ALOGE("mHal is null");
    return ret;
  }
}

static size_t memscpy(void *dst, size_t dst_size, const void *src, size_t src_size)
{
  size_t min_size = (dst_size < src_size) ? dst_size : src_size;
  memcpy(dst, src, min_size);
  return min_size;
}

static void print_text(std::string intro_message, unsigned const char *text_addr, int size)
{
  char const hex[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B','C','D','E','F'};
  std::string str;
  for (int i = 0; i < size; ++i) {
    const char ch = text_addr[i];
    str.append(&hex[(ch  & 0xF0) >> 4], 1);
    str.append(&hex[ch & 0xF], 1);
  }
  ALOGD("%s : %s",intro_message.c_str(), str.c_str());
}

static void gpqese_close(struct EseInterface *ese) {

  std::lock_guard<std::mutex> lock(m);
  TEEC_Result partialResult = result;
  if (ese == NULL) {
    ALOGE("close : eSE is null");
    return;
  }
  if (partialResult != TEEC_SUCCESS) {
    ese_set_error(ese, (result & 0xff));
    ALOGE("ESE ERROR MESSAGE - let's close everything %s", ese_error_message(ese));
  }
  operation.paramTypes = TEEC_PARAM_TYPES(TEEC_NONE, TEEC_NONE, TEEC_NONE, TEEC_NONE);

  result = TEEC_InvokeCommand(&session, GPQESE_CMD_CLOSE, &operation, &resultOrigin);
  if (result != TEEC_SUCCESS && partialResult == TEEC_SUCCESS) {
    // We only want to log the error if we're in a "normal close".
    ese_set_error(ese, (result & 0xff));
    ALOGE("ESE ERROR MESSAGE %s", ese_error_message(ese));
  }

  if (sessionOpen)
    TEEC_CloseSession(&session);
  if (contextInitialised)
    TEEC_FinalizeContext(&context);
  if (!nativeeSEPowerOff())
    ALOGE("Failure to power off the eSE");
  iseSEGPOpen = false;
  sessionOpen = false;
  contextInitialised = false;
}

static int gpqese_open(struct EseInterface *ese, void *hw_opts) {
  int ret = -1;
  std::unique_lock<std::mutex> lock(m);
  if (ese == NULL) {
    ALOGE("ese is null");
    return ret;
  }

  if (!contextInitialised) {
    /* Create the Context */
    result = TEEC_InitializeContext(teeName, &context);
    if (result != TEEC_SUCCESS) {
      ese_set_error(ese, (result & 0xff));
      ALOGE("TEEC_InitializeContext() ERROR MESSAGE %s", ese_error_message(ese));
      return ret;
    }
    contextInitialised = true;
  }

  if (!nativeeSEPowerOn()) {
    ALOGE("eSE not powered on");
    return ret;
  }

  if (!sessionOpen) {
    /* Open a session with the GP TA */
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_NONE, TEEC_NONE, TEEC_NONE, TEEC_NONE);
    result = TEEC_OpenSession(&context, &session, &uuidEseGP, TEEC_LOGIN_USER, NULL, &operation, &resultOrigin);

    if (result != TEEC_SUCCESS) {
      ALOGE("TEEC_OpenSession() returned 0x%08X\n", result);
      m.unlock();
      gpqese_close(ese);
      return ret;
    }
    sessionOpen = true;
  }
  if (!iseSEGPOpen) {
    operation.paramTypes = TEEC_PARAM_TYPES(TEEC_NONE, TEEC_NONE, TEEC_NONE, TEEC_NONE);
    result = TEEC_InvokeCommand(&session, GPQESE_CMD_OPEN, &operation, &resultOrigin);
    if (result != TEEC_SUCCESS) {
      ALOGE("GPQESE_CMD_OPEN returned 0x%08X\n", result);
      m.unlock();
      gpqese_close(ese);
      return ret;
    }
    iseSEGPOpen = true;
  }
  return (result == TEEC_SUCCESS);
}

static uint32_t gpqese_transceive(struct EseInterface *ese, const struct EseSgBuffer *tx, uint32_t /*tx_segs*/,
                      struct EseSgBuffer *rx, uint32_t /*rx_segs*/) {

  int ret = -1;
  std::unique_lock<std::mutex> lock(m);
  if (ese == NULL) {
    ALOGE("ese is null");
    return ret;
  }

  if (tx->len > GPQESE_MAX_CAPDU_SIZE){
    ALOGE("APDU too long %lu. max is %lu", (unsigned long)tx->len, (unsigned long)GPQESE_MAX_CAPDU_SIZE);
    return ret;
  }
  if (!contextInitialised || !sessionOpen || !nativeeSEGetState()) {
    ALOGE("Transceive failed : Context initialized : %d, Session open : %d, eSEPowerState : %d",
        contextInitialised, sessionOpen, nativeeSEGetState());
    return ret;
  }
  if (DBG)
    print_text("tApdu to be sent :", tx->c_base, tx->len);

  //INPUT MANAGEMENT
  operation.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_PARTIAL_INPUT,  // Input buffer
                         TEEC_MEMREF_PARTIAL_OUTPUT, // Output Buffer
                         TEEC_NONE,
                         TEEC_NONE);

  input.size = tx->len;
  input.flags = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT;
  result = TEEC_AllocateSharedMemory(&context, &input);
  if (result != TEEC_SUCCESS) {
    ALOGE("AllocateSharedMemory failed() returned 0x%08X\n", result);
    m.unlock();
    gpqese_close(ese);
    return ret;
  }

  memscpy(input.buffer,input.size,(void *) &tx->c_base[0], tx->len);
  operation.params[0].memref.parent = &input;
  operation.params[0].memref.offset = 0;
  operation.params[0].memref.size = tx->len;

  //OUTPUT MANAGEMENT
  output.size = GPQESE_MAX_RAPDU_SIZE;
  output.flags = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT;
  result = TEEC_AllocateSharedMemory(&context, &output);
  if (result != TEEC_SUCCESS) {
    ALOGE("AllocateSharedMemory failed() returned 0x%08X\n", result);
    TEEC_ReleaseSharedMemory(&input);
    m.unlock();
    gpqese_close(ese);
    return ret;
  }
  memset(output.buffer, 0x0, output.size);

  operation.params[1].memref.parent = &output;
  operation.params[1].memref.offset = 0;
  operation.params[1].memref.size = output.size;

  result = TEEC_InvokeCommand(&session, GPQESE_CMD_TRANSCEIVE, &operation, &resultOrigin);
  if (result != TEEC_SUCCESS) {
    ALOGE("Transceive failed() returned 0x%08X\n", result);
    TEEC_ReleaseSharedMemory(&input);
    TEEC_ReleaseSharedMemory(&output);
    ese_set_error(ese, (result & 0xff));
    return ret;
  }
  uint32_t size_read = (uint32_t)operation.params[1].memref.size;
  memscpy(rx->base, rx->len, (uint8_t*)output.buffer, size_read);
  if (DBG)
    print_text("rApdu received :", rx->base, size_read);
  TEEC_ReleaseSharedMemory(&input);
  TEEC_ReleaseSharedMemory(&output);
  return size_read;
}

static const struct EseOperations ops = {
    .name = "GPQESE_BACKEND",
    .open = &gpqese_open,
    .hw_receive = NULL,
    .hw_transmit = NULL,
    .transceive = &gpqese_transceive,
    .poll = NULL,
    .close = &gpqese_close,
    .opts = NULL,
    .errors = kErrorMessages,
    .errors_count = sizeof(kErrorMessages),
};
ESE_DEFINE_HW_OPS(GPQESE_BACKEND, ops);
