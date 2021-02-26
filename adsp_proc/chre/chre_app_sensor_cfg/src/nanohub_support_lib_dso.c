/*
 * Copyright (C) 2016 The Android Open Source Project
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

#include "chre/platform/shared/nanoapp_support_lib_dso.h"

#include <chre.h>

#include "chre/util/macros.h"

/**
 * @file
 * The Nanoapp Support Library (NSL) that gets built with nanoapps to act as an
 * intermediary to the reference CHRE implementation. It provides hooks so the
 * app can be registered with the system, and also provides a layer where we can
 * implement cross-version compatibility features as needed.
 */

DLL_EXPORT const struct chreNslNanoappInfo _chreNslDsoNanoappInfo = {
  .magic = CHRE_NSL_NANOAPP_INFO_MAGIC,
  .structMinorVersion = CHRE_NSL_NANOAPP_INFO_STRUCT_MINOR_VERSION,
  .targetApiVersion = CHRE_API_VERSION,

  // These values are supplied by the build environment
  .vendor = NANOAPP_VENDOR_STRING,
  .name = NANOAPP_NAME_STRING,
  .isSystemNanoapp = 0,
  .appId = NANOAPP_ID,
  .appVersion = NANOAPP_VERSION,

  .entryPoints = {
    .start = nanoappStart,
    .handleEvent = nanoappHandleEvent,
    .end = nanoappEnd,
  },
};

