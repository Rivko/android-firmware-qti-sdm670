/*==========================================================================
Description
  It has implementation for wake lock.

# Copyright (c) 2017 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#pragma once
#include <mutex>

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

#ifdef WAKE_LOCK_ENABLED

class Wakelock
{
 private:
  static unsigned short int wakelock_level_;
  static int wake_lock_fd_;
  static int wake_unlock_fd_;
  static bool wakelock_initialized_;
  static bool wakelock_acquired_;
  static std::recursive_mutex internal_mutex_;

  /*Constants*/
  static const char wakelock_name[];
  static const char wake_lock_path[];
  static const char wake_unlock_path[];

  /*functions*/
  static uint64_t GetCurrTime();
  static bool StartDelayTimer(unsigned int delay);
  static void WakelockDelayTimeout(union sigval sig);

 public:
  static bool Init();
  static bool Acquire();
  static bool Release();
  static bool ReleaseDelay(unsigned int delay);
  static bool CleanUp();
};

typedef struct
{
  uint64_t expiry;
  timer_t timer_id;
  bool is_timer_created;
  bool is_timer_active;
}WakelockReleaseDelay;

#endif

}  // namespace implementation
}  // namespace V1_0
}  // namespace bluetooth
}  // namespace hardware
}  // namespace android
