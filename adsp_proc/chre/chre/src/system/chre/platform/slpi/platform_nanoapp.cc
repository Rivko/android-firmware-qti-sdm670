/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "chre/platform/platform_nanoapp.h"

#include "chre/core/event_loop_manager.h"
#include "chre/platform/assert.h"
#include "chre/platform/log.h"
#include "chre/platform/memory.h"
#include "chre/platform/shared/nanoapp_dso_util.h"
#include "chre/platform/shared/nanoapp_support_lib_dso.h"
#include "chre/platform/slpi/memory.h"
#include "chre/platform/slpi/power_control_util.h"
#include "chre/util/system/debug_dump.h"
#include "chre_api/chre/version.h"

#include "chre/platform/slpi/printheap.h"

#include "dlfcn.h"

#include <inttypes.h>
#include <string.h>

namespace chre {

PlatformNanoapp::~PlatformNanoapp() {
  closeNanoapp();
  if (mAppBinary != nullptr) {
    memoryFreeBigImage(mAppBinary);
  }
}

bool PlatformNanoapp::start() {
  // Invoke the start entry point after successfully opening the app
  if (!isUimgApp()) {
    slpiForceBigImage();
  }

  return openNanoapp() && mAppInfo->entryPoints.start();
}

void PlatformNanoapp::handleEvent(uint32_t senderInstanceId,
                                  uint16_t eventType,
                                  const void *eventData) {
  if (!isUimgApp()) {
    slpiForceBigImage();
  }

  mAppInfo->entryPoints.handleEvent(senderInstanceId, eventType, eventData);
}

void PlatformNanoapp::end() {
  if (!isUimgApp()) {
    slpiForceBigImage();
  }

  mAppInfo->entryPoints.end();
  closeNanoapp();
}

bool PlatformNanoappBase::reserveBuffer(
    uint64_t appId, uint32_t appVersion, size_t appBinaryLen) {
  CHRE_ASSERT(!isLoaded());
  bool success = false;
  constexpr size_t kMaxAppSize = 2 * 1024 * 1024;  // 2 MiB

  if (appBinaryLen > kMaxAppSize) {
    LOGE("Rejecting app size %zu above limit %zu", appBinaryLen, kMaxAppSize);
  } else {
    mAppBinary = memoryAllocBigImage(appBinaryLen);
    if (mAppBinary == nullptr) {
      LOGE("Couldn't allocate %zu byte buffer for nanoapp 0x%016" PRIx64,
           appBinaryLen, appId);
    } else {
      mExpectedAppId = appId;
      mExpectedAppVersion = appVersion;
      mAppBinaryLen = appBinaryLen;
      success = true;
    }
  }

  return success;
}

bool PlatformNanoappBase::copyNanoappFragment(
    const void *buffer, size_t bufferLen) {
  CHRE_ASSERT(!isLoaded());

  bool success = true;
  if (mBytesLoaded + bufferLen > mAppBinaryLen) {
    LOGE("Overflow: cannot load %zu bytes to %zu/%zu nanoapp binary buffer",
         bufferLen, mBytesLoaded, mAppBinaryLen);
    success = false;
  } else {
    uint8_t *binaryBuffer = static_cast<uint8_t *>(mAppBinary) + mBytesLoaded;
    memcpy(binaryBuffer, buffer, bufferLen);
    mBytesLoaded += bufferLen;
  }

  return success;
}

void PlatformNanoappBase::loadFromFile(uint64_t appId, const char *filename) {
  CHRE_ASSERT(!isLoaded());
  mExpectedAppId = appId;
  mFilename = filename;
}

void PlatformNanoappBase::loadStatic(const struct chreNslNanoappInfo *appInfo) {
  CHRE_ASSERT(!isLoaded());
  mIsStatic = true;
  mAppInfo = appInfo;
}

bool PlatformNanoappBase::isLoaded() const {
  return (mIsStatic || (mAppBinary != nullptr && mBytesLoaded == mAppBinaryLen)
          || mDsoHandle != nullptr);
}

bool PlatformNanoappBase::isUimgApp() const {
  return mIsUimgApp;
}

void PlatformNanoappBase::closeNanoapp() {
  if (mDsoHandle != nullptr) {
    if (dlclose(mDsoHandle) != 0) {
      LOGE("dlclose failed: %s", dlerror());
    }
    mDsoHandle = nullptr;
  }
}

bool PlatformNanoappBase::openNanoapp() {
  bool success = false;

  if (mIsStatic) {
    success = true;
  } else if (mFilename != nullptr) {
    success = openNanoappFromFile();
  } else if (mAppBinary != nullptr) {
    success = openNanoappFromBuffer();
  } else {
    CHRE_ASSERT(false);
  }

  // Save this flag locally since it may be referenced while the system is in
  // micro-image
  if (mAppInfo != nullptr) {
    mIsUimgApp = mAppInfo->isTcmNanoapp;
  }

  return success;
}

bool PlatformNanoappBase::openNanoappFromBuffer() {
  CHRE_ASSERT(mAppBinary != nullptr);
  CHRE_ASSERT_LOG(mDsoHandle == nullptr, "Re-opening nanoapp");
  bool success = false;

  // Populate a filename string (just a requirement of the dlopenbuf API)
  constexpr size_t kMaxFilenameLen = 17;
  char filename[kMaxFilenameLen];
  snprintf(filename, sizeof(filename), "%016" PRIx64, mExpectedAppId);

  mDsoHandle = dlopenbuf(
      filename, static_cast<const char *>(mAppBinary),
      static_cast<int>(mAppBinaryLen), RTLD_NOW);
  if (mDsoHandle == nullptr) {
    LOGE("Failed to load nanoapp: %s", dlerror());
  } else {
    mAppInfo = static_cast<const struct chreNslNanoappInfo *>(
        dlsym(mDsoHandle, CHRE_NSL_DSO_NANOAPP_INFO_SYMBOL_NAME));
    if (mAppInfo == nullptr) {
      LOGE("Failed to find app info symbol: %s", dlerror());
    } else {
      success = validateAppInfo(mExpectedAppId, mExpectedAppVersion, mAppInfo);
      if (!success) {
        mAppInfo = nullptr;
      } else {
        LOGI("Successfully loaded nanoapp: %s (0x%016" PRIx64 ") version 0x%"
             PRIx32 " uimg %d system %d", mAppInfo->name, mAppInfo->appId,
             mAppInfo->appVersion, mAppInfo->isTcmNanoapp,
             mAppInfo->isSystemNanoapp);
        memoryFreeBigImage(mAppBinary);
        mAppBinary = nullptr;
      }
    }
  }

  return success;
}

bool PlatformNanoappBase::openNanoappFromFile() {
  CHRE_ASSERT(mFilename != nullptr);
  CHRE_ASSERT_LOG(mDsoHandle == nullptr, "Re-opening nanoapp");
  bool success = false;

  mDsoHandle = dlopen(mFilename, RTLD_NOW);
  if (mDsoHandle == nullptr) {
    LOGE("Failed to load nanoapp from file %s: %s", mFilename, dlerror());
  } else {
    mAppInfo = static_cast<const struct chreNslNanoappInfo *>(
        dlsym(mDsoHandle, CHRE_NSL_DSO_NANOAPP_INFO_SYMBOL_NAME));
    if (mAppInfo == nullptr) {
      LOGE("Failed to find app info symbol in %s: %s", mFilename, dlerror());
    } else {
      success = validateAppInfo(mExpectedAppId, 0, mAppInfo,
                                true /* ignoreAppVersion */);
      if (!success) {
        mAppInfo = nullptr;
      } else {
        LOGI("Successfully loaded nanoapp %s (0x%016" PRIx64 ") version 0x%"
             PRIx32 " uimg %d system %d from file %s", mAppInfo->name,
             mAppInfo->appId, mAppInfo->appVersion, mAppInfo->isTcmNanoapp,
             mAppInfo->isSystemNanoapp, mFilename);
        // Save the app version field in case this app gets disabled and we
        // still get a query request for the version later on. We are OK not
        // knowing the version prior to the first load because we assume that
        // nanoapps loaded via file are done at CHRE initialization time.
        mExpectedAppVersion = mAppInfo->appVersion;
      }
    }
  }

  return success;
}

uint64_t PlatformNanoapp::getAppId() const {
  return (mAppInfo != nullptr) ? mAppInfo->appId : mExpectedAppId;
}

uint32_t PlatformNanoapp::getAppVersion() const {
  return (mAppInfo != nullptr) ? mAppInfo->appVersion : mExpectedAppVersion;
}

uint32_t PlatformNanoapp::getTargetApiVersion() const {
  return (mAppInfo != nullptr) ? mAppInfo->targetApiVersion : 0;
}

bool PlatformNanoapp::isSystemNanoapp() const {
  // Right now, we assume that system nanoapps are always static nanoapps. Since
  // mAppInfo can only be null either prior to loading the app (in which case
  // this function is not expected to return a valid value anyway), or when a
  // dynamic nanoapp is not running, "false" is the correct return value in that
  // case.
  return (mAppInfo != nullptr) ? mAppInfo->isSystemNanoapp : false;
}

bool PlatformNanoapp::logStateToBuffer(char *buffer, size_t *bufferPos,
                                       size_t bufferSize) const {
  bool success = true;
  if (mAppInfo != nullptr) {
    success &= debugDumpPrint(buffer, bufferPos, bufferSize, " %s: vendor=\"%s\"",
                              mAppInfo->name, mAppInfo->vendor);
  }
  return success;
}

}  // namespace chre
