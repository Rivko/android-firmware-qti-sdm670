#pragma once
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

#ifndef  SNS_SEE_BMG160_CONFIG_H
#define  SNS_SEE_BMG160_CONFIG_H

// <DAE LITE configuration>
#ifndef BMG160_CONFIG_ENABLE_SEE_LITE
#define BMG160_CONFIG_ENABLE_SEE_LITE             0
#endif

#ifndef BMG160_CONFIG_ENABLE_FIFO
#define BMG160_CONFIG_ENABLE_FIFO                 0
#endif

#ifndef BMG160_CONFIG_ENABLE_SELF_TEST_FAC
#define BMG160_CONFIG_ENABLE_SELF_TEST_FAC        1
#endif

#ifndef BMG160_CONFIG_ENABLE_FLUSH_SYNC
#define BMG160_CONFIG_ENABLE_FLUSH_SYNC           1
#endif

// <HW ISR has high accuracy and regularly>
#ifndef BMG160_CONFIG_ENABLE_HW_ISR_REFERENCE
#define BMG160_CONFIG_ENABLE_HW_ISR_REFERENCE     0
#endif


#if BMG160_CONFIG_ENABLE_SEE_LITE

// <island>
#ifndef BMG160_CONFIG_ENABLE_ISLAND_MODE
#define BMG160_CONFIG_ENABLE_ISLAND_MODE        0
#endif
// </island>

// <log>
#define BMG160_CONFIG_ENABLE_DEBUG                0
#define BMG160_DEFAULT_LOG_LEVEL                  SNS_LOW
#define BMG160_CONFIG_ENABLE_DUMP_CORE_REG        0

// <reset>
#ifndef BMG160_CONFIG_ENABLE_SW_RESET_WITH_INST
#define BMG160_CONFIG_ENABLE_SW_RESET_WITH_INST   0
#endif

// <sensor>

// <Diagnostic Logging>
#ifndef BMG160_CONFIG_ENABLE_DIAG_LOG
#define BMG160_CONFIG_ENABLE_DIAG_LOG             0
#endif

// <registry>
#ifndef BMG160_CONFIG_ENABLE_REGISTRY
#define BMG160_CONFIG_ENABLE_REGISTRY             0
#endif

// <power rail reference>
#ifdef  BMG160_CONFIG_ENABLE_POWER_RAIL
#define BMG160_CONFIG_ENABLE_POWER_RAIL           0
#endif

#else

#ifndef BMG160_ENABLE_HIGH_ODR
#define BMG160_ENABLE_HIGH_ODR  0
#endif

// <island>
#ifndef  BMG160_CONFIG_ENABLE_ISLAND_MODE
#define BMG160_CONFIG_ENABLE_ISLAND_MODE   1
#endif
// </island>


// <log>
#define BMG160_CONFIG_ENABLE_DEBUG                1
#define BMG160_DEFAULT_LOG_LEVEL                  SNS_ERROR
#define BMG160_CONFIG_ENABLE_DUMP_CORE_REG        1

// <reset>
#ifndef BMG160_CONFIG_ENABLE_SW_RESET_WITH_INST
#define BMG160_CONFIG_ENABLE_SW_RESET_WITH_INST   1
#endif

// <sensor>

// <Diagnostic Logging>
#ifndef BMG160_CONFIG_ENABLE_DIAG_LOG
#define BMG160_CONFIG_ENABLE_DIAG_LOG             0
#endif

// <registry>
#ifndef BMG160_CONFIG_ENABLE_REGISTRY
#define BMG160_CONFIG_ENABLE_REGISTRY             1
#endif

// <power rail reference>
#ifndef BMG160_CONFIG_ENABLE_POWER_RAIL
#define BMG160_CONFIG_ENABLE_POWER_RAIL           1
#endif

#ifndef BMG160_ENABLE_LOG_LEVEL_1
#define BMG160_ENABLE_LOG_LEVEL_1                 0
#endif

#ifndef BMG160_ENABLE_LOG_LEVEL_2
#define BMG160_ENABLE_LOG_LEVEL_2                 0
#endif

#ifndef BMG160_HIGHEST_ODR
#define BMG160_HIGHEST_ODR     400
#endif

/* for bmi085 VR */
#define BMG160_VR 1
#ifdef BMG160_VR
#undef BMG160_HIGHEST_ODR
#define BMG160_HIGHEST_ODR     1000
#endif
/*for bmi085 VR*/

#endif

#endif /* SNS_SEE_BMG160_CONFIG_H */
