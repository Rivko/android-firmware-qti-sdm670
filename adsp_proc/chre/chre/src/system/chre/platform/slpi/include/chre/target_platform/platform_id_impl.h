/*==============================================================================
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All rights reserved. Qualcomm Proprietary and Confidential.
==============================================================================*/

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

#ifndef CHRE_PLATFORM_SLPI_PLATFORM_ID_IMPL_H_
#define CHRE_PLATFORM_SLPI_PLATFORM_ID_IMPL_H_

#include "chre/platform/platform_id.h"

namespace chre {

//! The vendor ID of the Google-provided SLPI platform.
constexpr uint64_t kVendorId = kVendorIdGoogle;

//! The platform ID of the Google-provided SLPI platform.
constexpr uint32_t kPlatformId = kGoogleSlpiPlatformId;

}  // namespace chre

#endif  // CHRE_PLATFORM_SLPI_PLATFORM_ID_IMPL_H_
