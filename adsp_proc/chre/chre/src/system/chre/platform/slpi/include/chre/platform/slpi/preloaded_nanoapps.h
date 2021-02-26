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

#ifndef CHRE_PLATFORM_SLPI_PRELOADED_NANOAPPS_H_
#define CHRE_PLATFORM_SLPI_PRELOADED_NANOAPPS_H_

#include <cstddef>
#include <cstdint>

#include "chre/core/nanoapp.h"
#include "chre/util/unique_ptr.h"

namespace chre {

/**
 * A description of a nanoapp to preload via dlopen.
 */
struct PreloadedNanoappDescriptor {
  //! The expected app ID for this nanoapp.
  const uint64_t appId;

  //! The filename of the shared objec tto open.
  const char *filename;

  //! The instance of this nanoapp.
  UniquePtr<Nanoapp> nanoapp;
};

//! The list of static nanoapps to load.
extern PreloadedNanoappDescriptor kPreloadedNanoappList[];

//! The number of static nanoapps to load.
extern const size_t kPreloadedNanoappCount;

/**
 * Loads nanoapps that are standalone .so file (i.e. not static nanoapps), but
 * are pre-loaded in the system image.
 */
void loadPreloadedNanoapps();

}  // namespace chre

#endif  // CHRE_PLATFORM_SLPI_PRELOADED_NANOAPPS_H_
