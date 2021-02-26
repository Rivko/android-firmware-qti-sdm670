/******************************************************************************
  @file    Perf.h
  @brief   Android performance HAL module

  DESCRIPTION

  ---------------------------------------------------------------------------

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/


#ifndef VENDOR_QTI_HARDWARE_PERF_V1_0_PERF_H
#define VENDOR_QTI_HARDWARE_PERF_V1_0_PERF_H

#include <vendor/qti/hardware/perf/1.0/IPerf.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>

#include "PerfGlueLayer.h"

namespace vendor {
namespace qti {
namespace hardware {
namespace perf {
namespace V1_0 {
namespace implementation {

using ::android::hidl::base::V1_0::DebugInfo;
using ::android::hidl::base::V1_0::IBase;
using ::vendor::qti::hardware::perf::V1_0::IPerf;
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

class Perf : public IPerf {
private:
    PerfGlueLayer mImpl;

    void Init();
public:
    Perf();
    ~Perf();

public:
    Return<int32_t> perfLockAcquire(int32_t pl_handle, uint32_t duration, const hidl_vec<int32_t>& boostsList, int32_t reserved) override;
    Return<void> perfLockRelease(int32_t pl_handle, int32_t reserved) override;
    Return<int32_t> perfHint(uint32_t hint, const hidl_string& userDataStr, int32_t userData1, int32_t userData2, int32_t reserved) override;
    Return<int32_t> perfProfile(int32_t pl_handle, int32_t profile, int32_t reserved) override;
    Return<void> perfLockCmd(int32_t cmd, int32_t reserved) override;
};

extern "C" IPerf* HIDL_FETCH_IPerf(const char* name);

}  // namespace implementation
}  // namespace V1_0
}  // namespace perf
}  // namespace hardware
}  // namespace qti
}  // namespace vendor

#endif  // VENDOR_QTI_HARDWARE_PERF_V1_0_PERF_H
