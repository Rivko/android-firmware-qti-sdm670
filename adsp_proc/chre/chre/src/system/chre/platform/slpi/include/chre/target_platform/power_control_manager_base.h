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

#ifndef CHRE_PLATFORM_POWER_CONTROL_MANAGER_BASE_H
#define CHRE_PLATFORM_POWER_CONTROL_MANAGER_BASE_H


namespace chre {

class PowerControlManagerBase {
 public:
  PowerControlManagerBase();

  ~PowerControlManagerBase();

  /* Operational mode for Sensors */
  typedef enum
  {
    PCM_MODE_IMG_UNKNOWN,
    PCM_MODE_IMG_MICRO,
    PCM_MODE_IMG_BIG,
    PCM_MODE_IMG_NOCLIENT
  } pcm_img_mode_e; 

  /**
    * Votes for a power mode to the SLPI power manager.
    *
    * @param mode The power mode to vote for.
    *
    * @return true if the vote returned SNS_PM_SUCCESS.
    */
  bool votePowerMode(pcm_img_mode_e mode);

 protected:
#if 0  // Need new implementation for 845 power control
  //! Client handle for the subscription to the power manager
  sns_pm_handle_t mClientHandle = nullptr;
#endif // #if 0
  //! Set to true if the host is awake, false if asleep.
  bool mHostIsAwake = true;

  /**
   * Invoked by the SNS Power Manager when the AP has suspended or resumed.
   *
   * @param apSuspended set to true when the AP has suspended, false on resume.
   */
  static void apSuspendCallback(bool apSuspended);
};

} // namespace chre

#endif // CHRE_PLATFORM_POWER_CONTROL_MANAGER_BASE_H
