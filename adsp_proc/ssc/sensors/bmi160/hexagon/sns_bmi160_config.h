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
  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential
  ****************************************************************************/

#ifndef __SNS_DD_BMI160_CONFIG_H
#define __SNS_DD_BMI160_CONFIG_H

// <Registry Sensors.SEE-LITE sensor>
#ifndef BMI160_CONFIG_ENABLE_SEE_LITE
#define BMI160_CONFIG_ENABLE_SEE_LITE   0
#endif

#ifndef BMI160_CONFIG_ENABLE_REGISTRY
#define BMI160_CONFIG_ENABLE_REGISTRY   1
#endif



#if !BMI160_CONFIG_ENABLE_REGISTRY
#ifndef BMI160_CONFIG_DFT_BUS_SPI
#define BMI160_CONFIG_DFT_BUS_SPI    1
#endif
#endif

// <Debug Print Messages>
#ifndef BMI160_CONFIG_ENABLE_DEBUG
#define BMI160_CONFIG_ENABLE_DEBUG   0
#endif

// <DAE Usage>
#ifndef BMI160_CONFIG_ENABLE_DAE
#define BMI160_CONFIG_ENABLE_DAE     0
#endif


// <Diagnostic Logging>
#ifndef BMI160_CONFIG_ENABLE_DIAG_LOG
#define BMI160_CONFIG_ENABLE_DIAG_LOG   1
#endif


// <power rail reference>
#ifndef BMI160_CONFIG_POWER_RAIL
#define BMI160_CONFIG_POWER_RAIL        1
#endif

#ifndef BMI160_CONFIG_ENABLE_ISLAND_MODE
#define BMI160_CONFIG_ENABLE_ISLAND_MODE    1
#endif

#ifndef BMI160_CONFIG_ENABLE_SELF_TEST_FAC
#define BMI160_CONFIG_ENABLE_SELF_TEST_FAC   1
#endif


#ifndef BMI160_CONFIG_ENABLE_DUMP_REG
#define BMI160_CONFIG_ENABLE_DUMP_REG           0
#endif


#ifndef BMI160_CONFIG_ENABLE_MAG_IF
#define BMI160_CONFIG_ENABLE_MAG_IF             0
#endif


#define BMI160_CONFIG_ACC_LOWEST_ODR            12.5      //sometimes using 12.5 will hurt power consumption when gyro is also used
#define BMI160_CONFIG_ACC_FASTEST_ODR           1600

#define BMI160_CONFIG_GYR_LOWEST_ODR            12.5
#define BMI160_CONFIG_GYR_FASTEST_ODR           1600

#ifndef BMI160_CONFIG_ENABLE_GYRO_DOWNSAMPLING_SW
#define BMI160_CONFIG_ENABLE_GYRO_DOWNSAMPLING_SW 1         //enable this if desired lowest sample_rate of gyro is 12.5hz, (BMI160_CONFIG_GYR_LOWEST_ODR has to be 12.5hz to be effective)
#endif

#define BMI160_CONFIG_ENABLE_LOW_LATENCY_RATE   1

#define BMI160_CONFIG_ENABLE_CUSTOM_FACTORY_CALIBRATION 0
#define BMI160_CONFIG_ENABLE_SELF_TEST_HW_SW    1

#define BMI160_CONFIG_ENABLE_OPTIM_LOAD_FP      0
#define BMI160_CONFIG_ENABLE_SIMPLE_CAL         1

#define BMI160_CONFIG_ENABLE_DRI_MODE           1

#endif

