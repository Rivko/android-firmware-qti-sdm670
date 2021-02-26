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

/**
 * @file
 * Provides platform IDs and versions for implementations of CHRE.
 */

#ifndef CHRE_PLATFORM_PLATFORM_ID_H_
#define CHRE_PLATFORM_PLATFORM_ID_H_

#include "chre/util/id_from_string.h"

#include <stdint.h>

namespace chre {

//! The vendor ID for Google.
constexpr uint64_t kVendorIdGoogle = createIdFromString("Googl");

//! The vendor platform ID of the Linux variant is one. All other
//  Google implementations will be greater than one. Zero is reserved.
constexpr uint32_t kGoogleLinuxPlatformId = 0x001;

//! The vendor platform ID of the Qualcomm SLPI variant.
constexpr uint32_t kGoogleSlpiPlatformId = 0x002;

//! Patch level of the current CHRE implementation, defined by the build system
//! and reported in chreGetVersion().
constexpr uint16_t kPatchVersion = CHRE_PATCH_VERSION;
static_assert(CHRE_PATCH_VERSION <= UINT16_MAX,
              "Patch version must fit in 16 bits");

}  // namespace chre

#include "chre/target_platform/platform_id_impl.h"

#endif  // CHRE_PLATFORM_PLATFORM_ID_H_
