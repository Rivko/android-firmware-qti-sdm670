/*******************************************************************************
* Copyright (c) 2017, STMicroelectronics.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     3. Neither the name of the STMicroelectronics nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/

/*==============================================================================

    Change Log:

    EDIT HISTORY FOR FILE
    Oct 24 2017 ST - 1.5.0
      - Align Driver with LSM6DSM_SEE_V1.29.0
    Nov 1 2017 ST - 1.6.0
      - MD support
    Nov 3 2017 ST - 1.7.0
      - Align Driver with LSM6DSO_SEE_V1.38.0
    Nov 9 2017 ST - 1.8.0
      - Fix for bad timestamp at fastest ODR
      - Minor self test fixes
    Nov 17 2017 ST - 1.9.0
      - Batching fixes
    Nov 20 2017 ST - 1.10.0
      - MD fix for heart beat(wmk fix at 0 ODR)
      - Latency fix
    Nov 30 2017 ST - 1.11.0
      - Registry write fix
      - MD fix for WMK=1
      - Filter update to improve MD sstvt and STD-DEV error
      - This release has MD updated to match LSM6DSM driver
    Dec 13 2017 ST - 1.12.0
      - Latency Fix
      - CTS fixes
      - Bad gyro data fix
    Jan 31 2018 ST - 1.13.0
      - Fix no temprature Data issue
      - Fix CTS "jitter out of range" issue
      - Fix stop-n-go jitter issue
    Apr 3 2018 ST - 1.13.0.2
      - Add SNS_STD_SENSOR_ATTRID_ADDITIONAL_LOW_LATENCY_RATES support
  ============================================================================*/

#ifndef _SNS_LSM6DSO_VER_H
#define _SNS_LSM6DSO_VER_H

#define SNS_VERSION_LSM6DSO        0x0113

#endif //_SNS_LSM6DSO_VER_H

