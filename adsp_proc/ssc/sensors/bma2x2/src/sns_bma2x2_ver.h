/*******************************************************************************
 * Copyright (c) 2018, Bosch Sensortec GmbH
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

/*==============================================================================
  DESCRIPTION

  Impelements the driver for the Bosch BMA2X2

  EDIT HISTORY FOR FILE


when        who      what
----------  ---      -----------------------------------------------------------
06/07/2017  fx       v1.0.0.0     :update: basic data streaming is finished
07/18/2017  fx       v1.0.1.0     :update: SDK SCC3.8/SCC4; data streaming, motion detection
08/24/2017  fx       v1.0.2.0     :update: ACC suspend mode
10/23/2017  fx       v1.1.1.0     :update: work against SDK5.056
10/24/2017  fx       v1.1.2.0     :updateL: partial SEE-LITE configuration
10/30/2017  fx       v1.1.2.1     :update: add BMA2X2_CONFIG_ENABLE_ISLAND_SERVICE
12/07/2017  fx       v1.1.2.2     :fix: fix the timestamp issue while batching
12/07/2017  fx       v1.1.2.3     :update: use isr for data streaming
12/12/2017  fx       v1.1.2.4     :improve: support drdy with 1000hz
12/23/2017  fx       v1.1.2.5     :improve: FIFO mode with highest odr 1000hz
12/27/2017  fx       v1.1.2.6     :update: factory testing support
12/29/2017  fx       v1.1.2.7     :improve: FIFO frame timestamp algorithm update for more precise
01/04/2018  fx       v1.1.2.8     :improve: time stamp shift fast fixing, force timestamp interval within the HIFI boundary
01/08/2018  fx       v1.1.2.9     :improve: new algo to remove the unused codes
01/11/2018  fx       v1.1.2.10    :update: new version for the snapdragon_scep.gen.5.0_files_00066.1
01/16/2018  fx       v1.1.2.11    :update: ts algo to handle the time drift at present
01/24/2018  fx       v1.1.2.12    :update: copyright update
02/11/2018  fx       v1.1.2.13    :update: sdk5.08
03/02/2018  fx       v1.1.2.14    :update: compatible with the data ready mode
03/03/2018  fx       v1.1.2.15    :update: support SNS_STD_SENSOR_ATTRID_ADDITIONAL_LOW_LATENCY_RATES for low latency client request
03/06/2018  fx       v1.1.2.16    :update: support low latency when request odr greater than 250HZ; remove sensor latency before report the frame
03/08/2018  fx       v1.1.2.17    :improvement: improve the low latency feature footprint
03/19/2018  fx       v1.1.2.18    :improvement: switch the driver working in the data ready mode when latency request coming
03/20/2018  fx       v1.1.2.19    :fixed: latency fixed
03/21/2018  fx       v1.1.2.20    :fixed: Kedi fix the calibration and registry version update issue
============================================================================*/

#define SNS_SEE_MODULE_VER_BMA2X2   "BMA2X2_SEE_VERSION: 2018/03/20 10:10:00 1.01.02.20"
#define BMA2X2_SNS_SEE_ATTRIB_VERSION   (0x01010220)
