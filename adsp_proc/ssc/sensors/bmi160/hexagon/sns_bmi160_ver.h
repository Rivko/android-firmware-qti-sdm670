/*******************************************************************************
 * Copyright (c) 2017-18, Bosch Sensortec GmbH
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     3. Neither the name of Bosch Sensortec GmbH nor the
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
 *******************************************************************************/

  /****************************************************************************
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
  ****************************************************************************/

/*==============================================================================
  DESCRIPTION

  Impelements the driver for the Bosch BMI160 part

  EDIT HISTORY FOR FILE


  when          who     what
  ----------    ---     -----------------------------------------------------------
  01/22/17      lk      0.01: driver initial draft for openssc_ver_2.3 based on sample driver
  01/30/17      lk      1.00: driver compiles and runs with skeleton functionalities
  02/22/17      lk      1.01: adapted to API changes in openssc_ver_6.1
  04/09/17      lk      1.02: adapted to API changes in openssc_ver_14.2 (island mode support)
  04/22/17      lk      1.03: bug fix for FIFO settings
  04/24/17      lk      1.03: bug fix for FIFO interrupt
  04/26/17      lk      1.04: bug fix for FIFO timestamp issues
  05/01/17      lk      1.05: bug fix for interval estimation in sensor test
  05/02/17      lk      1.06: bug fix for motion gated handling
  05/03/17      lk      1.07: adapted to API changes in openssc_ver_21.1 (changes in pb struct and dae)
  05/04/17      lk      1.08: added skeleton for dae
  05/05/17      lk      1.09: handle situation of hw_config() when fifo_flush_in_progress
  05/06/17      lk      1.10: improvement of cmd handling to save wait time
  05/07/17      lk      1.11: improvement of robustness of concurrent MD and FIFO int
  05/07/17      lk      1.12: bug fix with fc counting
  05/08/17      lk      1.13: improvement on cmd handling
  05/09/17      lk      1.14: fix for SPI bus protocol selection
  05/09/17      lk      1.15: [rls2q] improvement of time profiling of system delays and bus clocks
  05/11/17      lk      1.16: fix for wrong setting of WML
  05/12/17      lk      1.17: add handling of FIFO full
  05/12/17      lk      1.18: [rls2q] added robustness for very high ODR like 800 and 1600hz
  07/22/17      lk      2.00: [rls2q] refactored to adapt to SDK: 5_00038_1
  08/10/17      fx      2.01: works on the 8996
  08/17/17      lk      2.02: concurrent MD tests and streaming
  08/17/17      lk      2.03: use macro-controlled debug log
  08/17/17      lk      2.04: use new UUIDs for BMI160 sensors
  08/24/17      lk      2.05: adapted to new HD22 SDK: 5.0_files_00050.1
  09/07/17      fn      2.06: changes made to follow the see-lite guide
  09/13/17      lk      2.07: see-lite guide: use seperate header file for compiler switches
  09/13/17      lk      2.08: change switch BMI160_CONFIG_ENABLE_DEPENDENCY to BMI160_CONFIG_ENABLE_REGISTRY for easier understanding by customers
  09/13/17      lk      2.09: fix the issue with concurrent streaming of acc and gyr due to wrong gating by sensor type in bmi160_reval_instance_config()
  09/14/17      lk      2.09.1: excluding all diag log functions from compiling
  10/02/17      lk      2.10: fix merging errors in 2.06
  10/02/17      lk      2.10.1: clean up: remove invalid TODOs etc
  10/02/17      lk      2.10.2: fix: enable output at irq_ready=true
  10/05/17      lk      2.10.3: improvement: check PMU stat again in prepare_interrupt_handling to avoid unnecessary CMD
  10/08/17      lk      2.11: new feaure: FAC self test implemented
  10/10/17      lk      2.11.01: improvement: handle exception when pmu_stat is not expected at timer expiration
  10/11/17      lk      2.11.02: fix: a crash issue with NULL pointer deref in self test
  10/12/17      lk      2.11.03: fix: no hw_detect action in see-lite mode
  10/13/17      lk      2.11.04: fix: missing hw_config action in see-lite mode
  10/15/17      lk      2.11.05: clean up: use defined range values instead of raw numbers
  10/16/17      lk      2.11.06: merge: from branch fac self test features, tested working
  10/18/17      lk      2.11.07: improvement: reduce jitter due to change of clock resolution
  10/18/17      lk      2.11.08: fix: using odr_curr (OFF) in flush
  10/25/17      lk      2.11.09: add: BMI160_CONFIG_ENABLE_ISLAND_MODE
  10/25/17      lk      2.11.10: improvement: added robustness in all ODRs to avoid stop of FIFO streaming
  10/28/17      lk      2.11.11: improvement: sensortime correction to rejet false positives
  10/30/17      lk      2.11.12: change: set lowest ODR of accel to 12.5hz
  10/30/17      lk      2.11.13: improvement: converge faster with hw_res est if first est is ignored
  11/06/17      lk      2.11.14: improvement: general c code optimization for critical path
  11/06/17      lk      2.11.15: improvement: reduce flp computitions in handle_acc_frame
  11/06/17      lk      2.11.16: improvement: slight improvement on unhit code path
  11/06/17      lk      2.11.17: improvement: cache function pointers for faster access
  11/17/17      lk      2.11.18: change: use MAX_ODRs as cap
  11/22/17      lk      2.11.19: change: fine tuned some logs
  12/05/17      lk      2.11.20: fix: see-lite - missing registry events when BMI160_CONFIG_POWER_RAIL is 0 for see-lite
  12/06/17      lk      2.11.21: fix: see-lite - missing default registry_cfg when default registry_cfg when BMI160_CONFIG_ENABLE_REGISTRY is 0
  12/07/17      lk      2.11.22: change: per request - add sns_cal_event for factory calibration
  12/10/17      lk      2.11.23: fix: un-consumed event of SNS_STD_MSGID_SNS_STD_FLUSH_EVENT in bmi160_send_fifo_flush_done when publish_sensors is 0
  12/19/17      lk      2.11.24: fix: also send the self test event when fac_test_in_progress is true
  12/20/17      lk      2.11.25: send_fifo_flush_done SNS_STD_MSGID_SNS_STD_FLUSH_EVENT upon request of SNS_STD_MSGID_SNS_STD_FLUSH_REQ
  12/21/17      lk      2.11.26: improvement: reduce stack footprint in bmi160_update_registry for see-lite
  12/24/17      lk      2.11.27: improvement: change algo in wml adjustment for robustness
  12/24/17      lk      2.11.28: change: change the sensor name for easier identification at HAL side
  12/28/17      lk      2.11.29: avoid FIFO OVERFLOW when rr is 0
  12/29/17      lk      2.11.30: req alignment: disable certain attributes in bmi160_publish_registry_attributes() when BMI160_CONFIG_ENABLE_SEE_LITE is on
  01/02/18      lk      2.11.31: fix: missing update of fac_cal_version after fac_cal
  01/03/18      lk      2.12.00: req alignment: expose tuning parameters for MD in registry
  01/03/18      lk      2.12.01: req alignment: send MD disabled status when MD is concurrent with accel streaming
  01/03/18      lk      2.12.02: req alignment: when non-gated acc client goes away, resume original MD
  01/04/18      lk      2.12.03: improvement: reduce time needed for MD config
  01/10/18      lk      2.12.04: fix: reset int_en_flags_req.bits.md after MD is fired
  01/11/18      lk      2.12.05: improvement: reduce memory footprint by using BMI160_CONFIG_ENABLE_DIAG_LOG
  01/14/18      lk      2.12.06: change: when is_max_batch is true, use better strategy for determining the wml
  01/17/18      lk      2.12.07: improvement: gpio_read() will exit island mode, it's replaced with register read
  01/25/18      lk      2.12.08: req alignment: don't report ODRs not supported
  01/25/18      lk      2.12.09: fix: phy_sensor_config.sample_rate changed from fixed value:1 to actual value
  01/25/18      lk      2.12.10: improvement: optimize the algo for wml_adj
  01/25/18      lk      2.12.11: improvement: as requested, avoid sending redundent MDE event
  01/26/18      lk      2.12.12: copy idx from sstate at inst_init to avoid missing of idx info
  01/27/18      lk      2.12.13: req alignment: allow preemption by fac_cal when streaming is ongoing
  01/29/18      lk      2.12.14: fix: CTS reports incorrect range of accel/gyro
  01/29/18      lk      2.12.15: improvement: drain ascp events as much as possible before reconfig_hw
  02/01/18      lk      2.12.16: req alignment: send cal_event together with phy_sensor_config
  02/05/18      lk      2.12.17: new requirement: SNS_STD_SENSOR_ATTRID_ADDITIONAL_LOW_LATENCY_RATES
  02/05/18      lk      2.12.18: req alignment: avoid sending phy_sensor_config of accel when MD is enabled
  02/05/18      lk      2.12.19: req: using macros instead of const strings for registry node names
  02/05/18      lk      2.12.20: change: with new SDK 69, support cal version update in registry
  02/05/18      lk      2.12.21: new requirement: save fac cal result immediately after it's done
  02/05/18      lk      2.12.22: new requirement: SNS_CAL_MSGID_SNS_CAL_RESET
  02/06/18      lk      2.12.23: new requirement: support 12.5hz as lowest sample_rate for gyro
  02/06/18      lk      2.12.24: improvement: on memory footprint
  02/06/18      lk      2.12.25: fix: solve the issue of registry loading from old SDK sample code
  02/08/18      lk      3.00.00: improvement: reduce redundant info between bmi160_state and bmi160_instance_state
  02/08/18      lk      3.00.01: improvement: remove ascp_config from bmi160_instance_state
  02/08/18      lk      3.00.02: improvement: remove redundant acc_registry_cfg, gyro_registry_cfg, temp_registry_cfg from bmi160_instance_state to save memory
  02/12/18      lk      3.00.03: improvement: optimization in dev_time_a_lt_b_short_itvl
  02/12/18      lk      3.00.04: remove klocwork WARNING
  02/12/18      lk      2.12.26: improvement: optimization in dev_time_a_lt_b_short_itvl
  02/12/18      lk      2.12.27: remove klocwork WARNING
  02/12/18      lk      2.12.27.03: trial for crash issue on see-lite by implementation of flow control of acom read
  02/28/18      lk      2.12.28: work arround for handling exceptional delays in reading of ts_hw and irq_ctx on certain platform
  03/03/18      lk      2.12.28.03: moving some code to island
  03/03/18      lk      2.12.28.05: fix for improper removal of power rail for self test
  03/03/18      lk      2.12.29: requirement alignment: customer clarified on WML calculation guidelines
  03/06/18      lk      2.12.29.01: fix for exceptional corner case: error in SENSORTIME read caused by delay in SPI read, found in CTS on 845
  03/07/18      lk      2.12.29.02: handle exception in ascp_req on 845
  03/08/18      lk      2.12.29.03: req: add low power mode in phy_sensor_config
  03/09/18      lk      2.12.29.04: improvement: use BMI160_CONFIG_POWER_RAIL to reduce size in bmi160_sensor_notify_event
  03/08/18      lk      3.01.00: merged from 2.12.29.04
  03/12/18      lk      3.01.01: improved jitter with 800hz sr and higher
  03/19/18      lk      3.01.01.01: [merged] 2.12.29.06 req alignment: has_DAE_watermark set to false by default per QC request
  03/19/18      lk      3.01.01.02: [merged] 2.12.29.07: req alignment: send PSCE (phy_sensor_config event) for motion detect sensor as well per request
  04/04/18      lk      3.01.01.03: [merged] 2.12.29.09: corner case issue: ts_dev_ff is at boundary when there is no time reference info yet
  04/09/18      lk      3.01.01.04: [merged] 2.12.29.12: fix: observed ts_error when there is long delay in FIFO read (see-lite)
  04/09/18      lk      3.01.01.05: [merged] merged from 2.12.29.14: new req: added self_test for type HW_SW and cust_oem_cal
  04/13/18      lk      3.01.01.06: soft reset when an instance is removed
  04/16/18      lk      3.01.01.07: fix: wrong sample_rate in report of phy_sensor_config for concurrent gyro clients straddling on 12.5hz
  04/16/18      lk      3.01.01.08: fix: on SEE-lite platforms: clear of boost_read_size needs to be delayed because FIFO read is delayed much
  04/17/18      lk      3.01.01.09: fix: phy_sensor_config:gyro water_mark may be 0 when downsample_sw_factor is greater than 1
  04/18/18      lk      3.01.01.10: fix: remove the ts_ref speculation at exceptional delays of ascp_read which may results in wrong estimation of timestamp
  04/24/18      lk      3.01.02.00: improvement: reduce processor load w/o FPU
  04/25/18      lk      3.01.02.01: improvement: use BMI160_CONFIG_ENABLE_TS_REF_SPECULATION to block out unreliable ts estimation
  04/25/18      lk      3.01.02.02: improvement: make the struct bmi160_state more compact
  04/28/18      lk      3.01.02.03: improvement: use fixed point for data conversion to lower down CPU load esp. for see-lite
  04/29/18      lk      3.01.02.04: improvement: convert double to float to save cpu time for esp. see-lite
  04/29/18      lk      3.01.02.05: improvement: optimize use of xfer_time_per_byte_ticks when bus_is_spi and clock_is_slow (32khz)
  04/29/18      lk      3.01.02.06: improvement: change bus_cycle_time_us to bus_cycle_time_ns to optimize esp. for see-lite
  05/01/18      lk      3.01.02.07: improvement: skip calculation of ts_start when fc_this_batch_acc=1 to optimize esp. for see-lite
  05/08/18      lk      3.01.02.08: fix: add call of sensor_island_exit() before accessing non-island code
  05/11/18      lk      3.01.02.09: improvement: faster calculation of itvl_this_batch
  05/17/18      lk      3.02.00.00: change the version to 3.02.00.00
  06/11/18      lk      3.02.00.01: always send an error event in case of self test failures
  06/28/18      lk      3.02.01.00: improved the algo for wml calculation for is_max_batch situation
  07/02/18      lk      3.02.02.00: adding "static" modifier to const char ATTRIBUTES to address issues caused by LLVM compiler updates
  07/02/18      lk      3.02.03.00: fix: issue with fixed gyro data in oem_cal
  07/05/18      lk      3.03.00.00: improvement: refactor bmi160_hal_fifo_handle_frame_acc/gyr to bmi160_hal_report_single_frame_acc/gyr to make function more modular for later use
  07/06/18      lk      3.03.01.00: improvement: add DRI support
  07/19/18      lk      3.03.02.00: fix: timestamp type issue when sns_time is defined as uint32_t on see-lite
  10/15/18      lk      3.03.02.01: req alignment: send PHY_CFG_EVENT at the event of new client_request even when hw_config is pending
  12/05/18      fx      3.03.02.02: fix: axis remapping assignment error for floating point values


  ============================================================================*/

#ifndef __SNS_DD_BMI160_VER_H
#define __SNS_DD_BMI160_VER_H

#define SNS_DD_MODULE_VER_BMI160        "BMI160_DD_VERSION: 2018/12/05 13:26:00 3.03.02.02"
#define BMI160_SEE_DD_ATTRIB_VERSION    0x03030202

#endif

