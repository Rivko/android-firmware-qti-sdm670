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

#include "chre/platform/slpi/preloaded_nanoapps.h"

#include "chre/core/event_loop_manager.h"
#include "chre/platform/fatal_error.h"
#include "chre/util/macros.h"

namespace chre {

// The CHRE build variant can supply this macro to override the detault list of
// preloaded nanoapps. This list is supplied empty to ensure that the symbol is
// available for platforms with no preloaded nanoapps.
#ifndef CHRE_VARIANT_SUPPLIES_PRELOADED_NANOAPP_LIST

//! The default list of preloaded nanoapps to load.
PreloadedNanoappDescriptor kPreloadedNanoappList[] = {};

//! The size of the default preloaded nanoapp list.
const size_t kPreloadedNanoappCount = ARRAY_SIZE(kPreloadedNanoappList);

#endif  // CHRE_VARIANT_SUPPLIES_PRELOADED_NANOAPP_LIST

void loadPreloadedNanoapps() {
  // Compare with zero to allow the compiler to optimize away the loop.
  // Tautological comparisons are not warnings when comparing a const with
  // another const.
  if (kPreloadedNanoappCount > 0) {
    // Cast the kPreloadedNanoappCount to size_t to avoid tautological comparison
    // warnings when the kStaticNanoappCount is zero.
    EventLoop& eventLoop = EventLoopManagerSingleton::get()->getEventLoop();
    size_t nanoappCount = reinterpret_cast<size_t>(kPreloadedNanoappCount);
    for (size_t i = 0; i < nanoappCount; i++) {
      auto& app = kPreloadedNanoappList[i];
      if (app.nanoapp.isNull()) {
        FATAL_ERROR("Couldn't allocate memory for preloaded nanoapp");
      } else {
        app.nanoapp->loadFromFile(app.appId, app.filename);
        eventLoop.startNanoapp(app.nanoapp);
      }
    }
  }
}

}  // namespace chre
