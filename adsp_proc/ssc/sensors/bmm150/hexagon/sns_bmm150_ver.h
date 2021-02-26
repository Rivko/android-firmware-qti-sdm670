/*******************************************************************************
 * Copyright (c) 2019, Bosch Sensortec GmbH
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

  Impelements the driver for the Bosch BMM150
  EDIT HISTORY FOR FILE


when        who      what
----------  ---      -----------------------------------------------------------
08/16/2017  M       v1.0.0.0    :base version for SEE with SPI: mag, temperature works on the Open-Q 820 (APQ8096)
09/15/2017  M       v2.0.0.0    :update the code base on the new SDK snapdragon_scep.gen.5.0_files_00050.1
10/16/2017  M       v2.1.0.0    :update the code base on the new SDK snapdragon_scep.gen.5.0_files_00056.1
10/31/2017  M       v2.1.0.1    :1 add the switch for island mode BMM_CONFIG_ENABLE_ISLAND_MODE
11/21/2017  M       v2.1.0.2    :optimize the timestamp
11/29/2017  M       v2.1.0.3    :1 change the lisence of the bmm150_def.h
                                                 :2 add the 15HZ for the CTS requirement
12/28/2017  M       v2.1.0.4    :1 add the work around for mag 5HZ
01/09/2018  M       v2.1.0.5    :1 full fill with the HF requirement
01/16/2018  M       v2.1.0.6    :1 support 100HZ with qualcomm requirement
01/22/2018  M       v2.1.0.7    :1 add rep_xy to the 7
03/01/2018  M       v2.1.0.8    :1 update the latest SDK to snapdragon_scep.gen.5.0_files_00070.1
10/09/2018  M       v2.1.0.9    :1 add the pclint fix and fix the error code
07/02/2019  M       v2.1.0.10    :1 fix the power mode issue when selftest
============================================================================*/
#define BMM150_SEE_DD_VER 0x210A
