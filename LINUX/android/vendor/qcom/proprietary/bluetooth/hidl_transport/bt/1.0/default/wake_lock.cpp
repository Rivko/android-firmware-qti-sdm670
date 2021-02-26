/*==========================================================================
Description
  It has implementation for wake lock.

# Copyright (c) 2017 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#define LOG_TAG "vendor.qti.bluetooth@1.0-wake_lock"

#include <fcntl.h>
#include <errno.h>
#include <utils/Log.h>
#include "wake_lock.h"

namespace android {
namespace hardware {
namespace bluetooth {
namespace V1_0 {
namespace implementation {

#ifdef WAKE_LOCK_ENABLED

const char Wakelock :: wakelock_name[] = "hal_bluetooth_lock";
const char Wakelock :: wake_lock_path[] = "/sys/power/wake_lock";
const char Wakelock :: wake_unlock_path[] = "/sys/power/wake_unlock";

bool Wakelock :: wakelock_initialized_ = false;
bool Wakelock :: wakelock_acquired_ = false;
unsigned short int Wakelock :: wakelock_level_ = 0;
int Wakelock :: wake_lock_fd_ = -1;
int Wakelock :: wake_unlock_fd_ = -1;
std::recursive_mutex Wakelock::internal_mutex_;
static WakelockReleaseDelay release_delay;
static const clockid_t CLOCK_ID = CLOCK_MONOTONIC;


bool Wakelock :: Init()
{
  std::unique_lock<std::recursive_mutex> guard(internal_mutex_);
  if (wakelock_initialized_) {
    return true;
  }

  struct sigevent se;
  se.sigev_notify_function = (void (*)(union sigval))WakelockDelayTimeout;
  se.sigev_notify = SIGEV_THREAD;
  se.sigev_value.sival_ptr = &release_delay.timer_id;
  se.sigev_notify_attributes = NULL;

  release_delay.is_timer_created = (timer_create(CLOCK_MONOTONIC, &se, &release_delay.timer_id) == 0);
  if(!release_delay.is_timer_created) {
    ALOGW("%s Failed to create wakelock timer - %s", __func__, strerror(errno));
  }
  release_delay.is_timer_active = false;

  wake_lock_fd_ = open(wake_lock_path, O_WRONLY | O_APPEND);
  if (wake_lock_fd_ < 0) {
    ALOGE("%s Failed to open wakelock file - %s", __func__, strerror(errno));
    return false;
  }

  wake_unlock_fd_ = open(wake_unlock_path, O_WRONLY | O_APPEND);
  if (wake_unlock_fd_ < 0) {
    ALOGE("%s Failed to open wake unlock file", __func__);
    close(wake_lock_fd_);
    return false;
  }

  ALOGD("%s wakelock is initiated ", __func__);
  wakelock_initialized_ = true;
  return true;
}

bool Wakelock :: CleanUp()
{
  std::unique_lock<std::recursive_mutex> guard(internal_mutex_);
  if (!wakelock_initialized_) {
    ALOGE("%s wake lock not initialized", __func__);
    return true;
  }

  if(wakelock_level_ > 0) {
    wakelock_level_ = 1;
    Release();
  }

  if(release_delay.is_timer_created) {
    timer_delete(release_delay.timer_id);
    memset(&release_delay, 0, sizeof(WakelockReleaseDelay));
  }

  close(wake_lock_fd_);
  close(wake_unlock_fd_);

  ALOGD("%s wakelock is destroyed ", __func__);
  wakelock_initialized_ = false;

  return true;
}

bool Wakelock :: Acquire()
{
  std::unique_lock<std::recursive_mutex> guard(internal_mutex_);
  if (!wakelock_initialized_) {
    ALOGE("%s wake lock not initialized", __func__);
    return false;
  }

  if (wakelock_level_ > 0) {
    wakelock_level_++;
    return true;
  }

  int ret = write(wake_lock_fd_, wakelock_name, strlen(wakelock_name));
  if (ret != (int)strlen(wakelock_name)) {
    ALOGE("%s write to wakelock file failed %d - %s", __func__, ret, strerror(errno));
    return false;
  }

  wakelock_level_++;
  ALOGD("%s wakelock is acquired ", __func__);
  wakelock_acquired_ = true;
  return true;
}

bool Wakelock :: Release()
{
  std::unique_lock<std::recursive_mutex> guard(internal_mutex_);
  if (!wakelock_initialized_ || !wakelock_acquired_) {
    ALOGE("%s wake lock not initialized/acquired", __func__);
    return false;
  }

  --wakelock_level_;
  if (wakelock_level_ > 0)
    return true;

  int ret = write(wake_unlock_fd_, wakelock_name, strlen(wakelock_name));
  if (ret != (int)strlen(wakelock_name)) {
    ALOGE("%s write to wake unlock file failed %d - %s", __func__, ret, strerror(errno));
    wakelock_level_++;
    return false;
  }
  ALOGE("%s wakelock is released ", __func__);
  wakelock_acquired_ = false;
  wakelock_level_ = 0;
  return true;
}

bool Wakelock :: ReleaseDelay(unsigned int delay)
{
  bool ret;
  if(!release_delay.is_timer_created || delay == 0) {
    ret = Release();
    return ret;
  }

  std::unique_lock<std::recursive_mutex> guard(internal_mutex_);
  uint64_t curr_time = GetCurrTime();
  if(release_delay.is_timer_active) {
    ret = Release();
    if(curr_time + delay > release_delay.expiry) {
      release_delay.expiry = curr_time + delay;
      ret = StartDelayTimer(delay);
    }
  } else {
    release_delay.expiry = curr_time + delay;
    ret = StartDelayTimer(delay);
  }
  return ret;
}

void Wakelock :: WakelockDelayTimeout(union sigval sig)
{
  std::unique_lock<std::recursive_mutex> guard(internal_mutex_);
  if(release_delay.is_timer_active) {
    uint64_t curr_time = GetCurrTime();
    if(curr_time >= release_delay.expiry) {
      release_delay.is_timer_active = false;
      release_delay.expiry = 0;
      Release();
    }
  }
}

bool Wakelock :: StartDelayTimer(unsigned int delay)
{
  struct itimerspec ts;
  bool timer_status, ret;

  ts.it_value.tv_sec = delay / 1000;
  ts.it_value.tv_nsec = 1000000 * (delay % 1000);
  ts.it_interval.tv_sec = 0;
  ts.it_interval.tv_nsec = 0;

  timer_status = (timer_settime(release_delay.timer_id, 0, &ts, 0) == 0);
  if(!timer_status) {
    ALOGE("%s: Failed to start wakelock timer", __func__);
    ret = Release();
  } else {
    release_delay.is_timer_active = true;
    ret = true;
  }

  return ret;
}

uint64_t Wakelock :: GetCurrTime()
{
  struct timespec ts;
  return clock_gettime(CLOCK_ID, &ts) ? 0 : (ts.tv_sec * 1000LL) + (ts.tv_nsec / 1000000LL);
}

#endif

}  // namespace implementation
}  // namespace V1_0
}  // namespace bluetooth
}  // namespace hardware
}  // namespace android
