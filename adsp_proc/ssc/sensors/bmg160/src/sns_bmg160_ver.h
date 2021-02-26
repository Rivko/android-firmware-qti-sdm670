/*******************************************************************************
 * Copyright (c) 2018-2019, Bosch Sensortec GmbH
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

  Impelements the driver for the Bosch BMG160

  EDIT HISTORY FOR FILE


when        who      what
----------  ---      -----------------------------------------------------------
07/24/2017  fx       v1.0.1.0     :SDK SCC3.8/SCC4; registry
08/01/2017  fx       v1.0.2.0     :data streaming
10/24/2017  fx       v1.1.1.0     :compile OK base on SDK 5.056
10/25/2017  fx       v1.1.1.1     :finish sensor publish OK
10/26/2017  fx       v1.1.1.2     :finish data streaming
10/27/2017  fx       v1.1.1.3     :partial DAE-LITE: registry, debug, log, power rail reference
10/31/2017  fx       v1.1.1.4     :add BMG160_CONFIG_ENABLE_ISLAND_MODE
12/06/2017  fx       v1.1.1.5     :fix timestamp issue range issue
12/07/2017  fx       v1.1.1.6     :flushing happened, the reference ts should be the latest ts
12/07/2017  fx       v1.1.1.7     :ignore the first wml INT
12/13/2017  fx       v1.1.1.8     :support drdy with 1000hz
12/23/2017  fx       v1.1.1.9     :FIFO mode with new algo to precise the ts
12/27/2017  fx       v1.1.1.10    :factory testing support. registry file updating with factory testing not test
01/16/2018  fx       v1.1.1.11    :ts algo update, handle time drift at present occurrence
01/24/2018  fx       v1.1.1.12    :copyright update
03/03/2018  fx       v1.1.1.13    :update: add SNS_STD_SENSOR_ATTRID_ADDITIONAL_LOW_LATENCY_RATES
03/06/2018  fx       v1.1.1.14    :update: fifo is always 1 if request sample rate greater than 200Hz; remove the latency time when report sample
03/08/2018  fx       v1.1.1.14    :improvement: improve the low latency feature footprint
03/20/2018  fx       v1.1.1.15    :improvement: switch the driver working in the data ready mode when latency request coming
04/23/2018  fx       v1.1.1.16    :improvement: return checking for each bus access
04/25/2018  fx       v1.1.1.17    :improvement: add delay after power mode changing
05/22/2018  fx       v1.1.1.18    :improvement: time delay after soft reset to fixed data 0 issue
09/21/2018  fx       v1.1.1.19    :fixed: fac test update to registry with version
09/29/2018  fx       v1.1.1.20    :improvement:fastest odr support to 400
10/19/2018  fx       v1.1.1.21    :improvement:ts algo optimize
10/23/2018  fx       v1.1.1.22    :improve: add tolerance when ff len bigger than the configured wml
11/16/2018  fx       v1.1.1.23    :fixed: data ready mode on SDM845, remove test code
11/19/2018  fx       v1.1.1.24    :fixed: test code removing issue and fac test in data ready mode
12/05/2018  mt       v1.1.1.25    :fixed: support data ready with int1 and int2
06/17/2019  mt       v1.1.1.26    :add the switch for bmi085 vr
09/25/2019  MT       v1.1.1.27     :1 add SNS_STD_SENSOR_ATTRID_RIGID_BODY
                                    2 fixed report properly sample_rate issue
                                    3 sync the code from Qualcomm
09/26/2019  MT       v1.1.1.28     :1 merge the qualcomm comments
                                    2 do not use hard code SNS_STD_SENSOR_ATTRID_RIGID_BODY to 0  
								    3 publish twice SNS_STD_SENSOR_ATTRID_RIGID_BODY attribute in bma160_publish_registry_attributes 

10/25/2019  MT       v1.1.1.29     :1 put bmg160_publish_registry_attributes(this) when registry received

============================================================================*/

#define SNS_SEE_MODULE_VER_BMG160 "BMG160_SEE_VERSION: 2019/10/25 10:10:00 1.01.01.29"
#define BMG160_SNS_SEE_ATTRIB_VERSION   (0x01010129)


