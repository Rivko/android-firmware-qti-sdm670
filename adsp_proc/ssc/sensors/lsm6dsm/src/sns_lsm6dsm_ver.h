/*******************************************************************************
* Copyright (c) 2018, STMicroelectronics.
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
  Fri  June 30 2017 sk/ST - 1.12
  - Timestamp changes for accel and gyro
  - invalid samples for gyro updated
  - fixed first few invalid samples marked as valid
  - send config events when config changed
  - updated fifo wmk calculation
  Fri  July 14 2017 sk/ST - 1.13
  - Added support for Self_test and registry
  Mon July 17 2017 sk/ST - 1.14
  - Added support for self-test and streaming concurrency
  Thu July 20 2017 sk/ST - 1.15
  - Move flush to use async com port
  - Bug fixes and address TODO comments
  Mon July 24 2017 sk/ST - 1.16
  - Add partial support for flush_only (imcomplete) and max_batch
  - Fix issue with Accel factory test not updating the axis to 9.8
  Fri July 28 2017 ST - 1.17.0
  - Fix issue with gyro resolution publishing
  - Remove use of pow
  - Fix issue for Gyro OOB
  Wed Aug 02 2017 ST - 1.18.0
  - Update comparision in publish sensor attribute
  - Fix Gyro jitter issue
  - Remove scale_factor from send_cal_event
  - Fix issue where send_cal event for reset cal was not sent
  - Crash fix from Qualcomm
  Wed Aug 03 2017 ST - 1.19.0
  - MD and Accel concurrent streaming issue fixed.
  - reading axis mapping for gyro
  - merge level triggering support from qcom (gated with BUILD_DB macro)
  Wed Aug 16 2017 ST - 1.20.0
  - Address TODO comments
  - Fix missing accel sample issue when gyro shuts down
  - Fix issue where temp data is stuck when no other sensor is streaming
  - Fix issue of missing sample when skipping sample
  - Flush only case: use flush period to set wmk and do not use max period
  - Update logic for flush_only case
  - Update timestamp logic for non-flush use case
  - Remove registry and SUID stream after use
  Tue Aug 22 2017 QC - 1.21.0
  - Optimize for island memory
  Wed Sep 06 2017 QC - 1.22.0
  - Fix insufficient buffer size
  Thu Sept 7 2017 ST - 1.23.0
  - Fix memory leak
  - Fix issue with registry write
  - Updated Fixed potential stack overflow in align_fifo_data function
  Sep 12 2017 QC - 1.24.0
  - Further optimize for island memory
  - Renable DAE path
  - Fix missing config event when changing ODR/WM
  Sept 26 2017 ST - 1.25.0
  - Add support for remove request when Self-test is active
  - Add support for registry read of Resolution/Range values
  - Add support for handling rr=0,sr=0 case
  - Get MD parameters from registry
  - Separate timer for temp, self-test,md and fifo
  - Self-Test fixes
  - Updates for potential stack overflow
  - Fixes for DAE changes received in 1.24
  - Fix issue of MD sending event when no client
  Sept 28 2017 QC - 1.26.0
  - More DAE related fixes
  Oct 06 2017 ST - 1.27.0
  - Timestamp fixes
  Oct 11 2017 ST - 1.28.0
    - Remove redundant flush_send event when there are no samples to flush
    - Fix issue with wrong start time for wmk>1
    - Flush event not recieved from Sensor Temp/MD
    - KW fixes
    - Fix use of first timestamp in validate timestamp
    - Trk:82 - Send Accel logs only when there is a client
    - DEBUG: Add debug counter for accel/gyro values. Print every 100 samples
  Oct 19 2017 ST - 1.29.0
    - Fix publihsing physical sensor config event
    - Add publishing cal event for streaming client requests
    - Fix ts issues
  Oct 20 2017 ST - 1.30.0
    - Fix publihsing physical sensor config event
    - Fix gyro not streaming
  Oct 26 2017 ST - 1.31.0
    - Use sync com port for WM < 5 data transfers
    - Add heart beat timer
    - Code clean up
  Oct 27 2017 ST - 1.32.0
    - Publish physical sensor attribute
    - Heart beat timer clean up
    - Use WM=1 for batch_period = 0
  Oct 30 2017 QC - 1.33.0
    - Fix heart beat timer crash
  Oct 30 2017 ST - 1.34.0
    - Fix initial sample interval
  Oct 31 2017 ST - 1.35.0
    - Fix sending flush event for CTS moving rates failure
  Nov 1 2017 ST - 1.36.0
    - flush all the data in fifo before reconfiguring
  Nov 1 2017 ST - 1.37.0
    - flush fifo only when req is for accel/gyro not temp
    - use cur/desired temp rate for setting temp timer
    - clear reconfig flag if fifo_interrupt is not required(fifo not enabled)
  Nov 2 2017 ST - 1.38.0
    - Fix chosen WM in case of max batch
    - Send config event before flush during reconfig
  Nov 3 2017 ST - 1.39.0
    - Address issue with unreliable samples in middle of a stream when only
      WM changes
    - Reconfig on sample interval boundary in case of streaming to improve
      latency
  Nov 7 2017 ST - 1.40.0
    - Deduct filter delay from sample ts
    - Fix bug where config event wasn't sent when only WM changes
  Nov 9 2017 ST - 1.41.0
    - Fix concurrency reconfig bug
    - Fix sensor temp + accel concurrency issue
  Nov 11 2017 ST - 1.42.0
    - Updated heart attack logic
  Nov 16 2017 ST - 1.43.0
    - Fix in accel resolution in config event
  Nov 17 2017 QC - 1.44.0
    - Allow registry write to take effect before reg stream is removed
  Nov 28 2017 QC - 1.45.0
    - Fix error handling in instance notify_event
  Dec 7 2017 QC - 1.46.0
    - reset dae variables
  Dec 7 2017 ST - 1.47.0
    - Fix event handling: check if the event is null or not before handling
    - Fix ascp event handling: handle ascp event only if ascp_req_count is > 0
    - Fix Heart attack: while recovering device, reconfig fifo if required
    - Heart beat: consume all heart beat timer events and use latest HB timeout event
    - update invalid samples for gyro
  Dec 13 2017 ST - 1.48.0
    - fix water mark config issue
  Dec 14 2017 QC - 1.49.0
    - Fixed DAE interface module
  Jan 8 2018 ST - 1.50.0
    - Fix for latency issue
    - Remove redundant if check
    - Fix for recheck in fifo and flush case
    - Fix reporting rate while batching on in multiple requests
  Jan 8 2018 QC - 1.51.0
    - fix multiple flush process
  Jan 14 2018 ST - 1.52.0
    - fix CTS failure with doubled delta_time to sample interval
  Jan 26 2018 ST - 1.53.0
    - restart accel for heart attack device reset
    - MD disable event sending to frameworks
    - Fix CTS "jitter out of range" issue
  Jan 31 2018 ST - 1.54.0
    - Fix CTS Jitter out of range issue
    - Ignore Flush request if instance is NULL
  Feb 2 2018 ST - 1.55.0
    - Add support for SNS_STD_SENSOR_ATTRID_ADDITIONAL_LOW_LATENCY_RATES
  Feb 27 2018 ST - 1.56.0
    - dual sensors support
    - Fix for one bias not applied issue
    - Log optimizations for reducing island mode image size
  Mar 13 2018 ST - 1.56.0.1
    - Fix Heart Attack issue on ODR change/Flush
    - Fix Self_test multiple run issue
  Mar 16 2018 ST - 1.56.0.2
    - Add support for stationary and horizontal detection for Factory Test
    - Add logic to read and use registry version for bias(Now suported by QCOM - OpenSSC-5.0.8)
  Mar 23 2018 ST - 1.57.0
    - Add SNS_STD_SENSOR_ATTRID_PHYSICAL_SENSOR_TESTS publishing
  April 10 2018 ST - 1.58.0
    - Fix heart attack issue due to HB timer running when wmk=0
    - Fix issue with power_rail_pend_state for OFF state
    - sending clear interrupt msg when read data using sync com port
    - Add place holder for OEM tests
    - Fix "MD Fired" when gyro enabled issue
 December 12 2018 QC - 1.59.0
    - If samples size is more than MAX fifo wmk, read samples in multiples of MAX fifo
	  (CR2334534 propogation from SS CPL)
============================================================================*/

#ifndef _SNS_LSM6DSM_VER_H
#define _SNS_LSM6DSM_VER_H

#define SNS_VERSION_LSM6DSM        0x0159


#endif //_SNS_LSM6DSM_VER_H

