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

  Impelements the driver for the Bosch BMP285

  EDIT HISTORY FOR FILE


when        who      what
----------  ---      -----------------------------------------------------------
02/20/2017  M       v1.0    :base version for SEE with I2C: pressure, temperature works on the Open-Q 820 (APQ8096)
03/01/2017  M       v1.2    :works with SPI, different timers read the pressure,temperature
04/06/2017  M       v2.0    : 1 Refactor the driver code base on the snapdragon_scep.gen.5.0_files_00014.2
                            : 2 change the working mode to the Force mode,Ultra high resolution
04/11/2017  M       v2.1    :use the float type to report the data,delete some no use code for I2C/SPI
                                           :change the code base on the snapdragon_scep.gen.5.0_files_00021.1
05/24/2017  M       v2.1.0.1    :delete the BME_BUSY_DELAY(500) in init function;
06/06/2017  M       v2.1.0.2    :1 Fix 10HZ not corrent;
                                :2 optimize the code pb_send_sensor_stream_event
                                :3 optimize the code set the power mode
06/21/2017  MW      v2.1.0.3    :read cal param in instance
06/27/2017  M       v2.1.0.4    :1 refactor the code for island mode
                                :2 change the is_periodic = true when start the timer
07/13/2017  M       v2.1.0.5   :1 delete the bmp285_dae_if_available(this) condition
                                                :2.Remove use of axis map, tri-axial fac cal params that are not applicable to pressure and temp.
                                                :3.Request, parse, save placement config from registry.
                                                :4.Publish available attribute only after all registry requests are met.
                                                :5.Publish placement attribute as part of bmp285_publish_registry_attributes().
                                                :6.Remove redundant attributes from bmp285_publish_attributes() for both pressure and temp that the driver anyway publishes in bmp285_publish_registry_attributes().
08/07/2017  M       v2.1.0.6    :1 add the calibration P10 data for bmp285, becuase bmp280 API do not have this.
08/28/2017  M       v2.2.0.1    :1 update the code for latest SDK snapdragon_scep.gen.5.0_files_00050.1
                                                 :2 change the code base on the sample code 
10/11/2017  M       v2.2.0.2    :1 change the code for SEE_LITE
10/12/2017  M       v2.2.0.3    :1 fix the code error
10/23/2017  M       v2.2.0.4    :1 change the code base on the new SDK snapdragon_scep.gen.5.0_files_00056.1 sample code
                                :2 optimize the code base on the Qualcomm requirement
10/26/2017  M       v2.2.0.5    :1 fix the code error of BMP_CONFIG_POWER_RAIL
10/31/2017  M       v2.2.0.6    :1 add the switch for island mode BMP_CONFIG_ENABLE_ISLAND_MODE
10/25/2017  TC      v2.2.0.7    :use the timer-event timestamp for pressure and temperature events
11/04/2017  PS      v2.2.0.8    :add support for the SELECTED_RANGE attribute for pressure sensor 
11/11/2017  MW      v2.2.0.9    :add cal_event.
11/14/2017  M       v2.2.1.0    :fix when only enable temperature ,temperature value 0 issue
11/14/2017  M       v2.2.1.1    :add bias and cal in registey
11/16/2017  M       v2.2.1.2    :remove the fac_cal_corr_mat and add fac_cal_scale
01/10/2018  M       v2.2.1.3    :update to the latest SDK snapdragon_scep.gen.5.0_files_00066.1
01/26/2018  M       v2.2.1.4    :if the request sample rate equal the old one, do not need start the timer again
01/26/2018  M       v2.2.1.5    :delete the sns_diag_service function for the temperature
02/01/2018  M       v2.2.1.6    :change the code base on the Qualcomm review comments
02/06/2018  M       v2.2.1.7    :1 update the code to the snapdragon_scep.gen.5.0_files_00069.1
                                                 :2 optimize the code snd_cal_event
                                                 :3 add the fac_version
02/28/2018  M       v2.2.1.8    :add the comments from Qualcomm side
============================================================================*/
#define BMP285_SEE_DD_VER 0x2218
