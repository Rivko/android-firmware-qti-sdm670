#pragma once
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

#ifndef  SNS_SEE_BMA2X2_CONFIG_H
#define  SNS_SEE_BMA2X2_CONFIG_H

// <DO NOT change this>
#ifndef BMA2X2_ENABLE_TEMP_PATCH
#define BMA2X2_ENABLE_TEMP_PATCH                  1
#endif
// </DO NOT change this>

// <DAE LITE configuration>
#ifndef BMA2X2_CONFIG_ENABLE_SEE_LITE
#define BMA2X2_CONFIG_ENABLE_SEE_LITE             0
#endif

#ifndef BMA2X2_CONFIG_ENABLE_FIFO
#define BMA2X2_CONFIG_ENABLE_FIFO                 1
#endif

#ifndef BMA2X2_CONFIG_ENABLE_SELF_TEST_FAC
#define BMA2X2_CONFIG_ENABLE_SELF_TEST_FAC        1
#endif


#ifndef BMA2X2_CONFIG_ENABLE_FLUSH_SYNC_READ
#define BMA2X2_CONFIG_ENABLE_FLUSH_SYNC_READ      1
#endif

#ifndef BMA2X2_CONFIG_ENABLE_HEARTBEAT_TIMER
#define BMA2X2_CONFIG_ENABLE_HEARTBEAT_TIMER      0
#endif

// <force wait>
#ifndef BMA2X2_ENABLE_FORCE_WAIT
#define BMA2X2_ENABLE_FORCE_WAIT                  1
#endif

#if BMA2X2_CONFIG_ENABLE_SEE_LITE

// <island>
#ifndef BMA2X2_CONFIG_ENABLE_ISLAND_SERVICE
#define BMA2X2_CONFIG_ENABLE_ISLAND_SERVICE       0
#endif
// </island>

// <log>
#define BMA2X2_ENABLE_DEBUG                       0
#define BMA2X2_DEFAULT_LOG_LEVEL                  SNS_LOW
#define BMA2X2_CONFIG_ENABLE_DUMP_CORE_REG        0

// <DAE>
#ifndef BMA2X2_CONFIG_ENABLE_DAE
#define BMA2X2_CONFIG_ENABLE_DAE                  0
#endif

// <sensor>
#define BMA2X2_CONFIG_ENABLE_MD_LPM               1
#define BMA2X2_CONFIG_HANDLE_HW_CONFIG            1


#ifndef BMA2X2_ENABLE_TEMP_PATCH
#define BMA2X2_ENABLE_TEMP_PATCH                  1
#endif

// <Diagnostic Logging>
#ifndef BMA2X2_CONFIG_ENABLE_DIAG_LOG
#define BMA2X2_CONFIG_ENABLE_DIAG_LOG             0
#endif

// <registry>
#ifndef BMA2X2_CONFIG_ENABLE_REGISTRY
#define BMA2X2_CONFIG_ENABLE_REGISTRY             0
#endif

// <power rail reference>
#ifndef  BMA2X2_CONFIG_ENABLE_POWER_RAIL
#define BMA2X2_CONFIG_ENABLE_POWER_RAIL           0
#endif

#else

// <island>
#ifndef BMA2X2_CONFIG_ENABLE_ISLAND_SERVICE
#define BMA2X2_CONFIG_ENABLE_ISLAND_SERVICE       1
#endif
// </island>

// <log>
#define BMA2X2_ENABLE_DEBUG                       1
#define BMA2X2_DEFAULT_LOG_LEVEL                  SNS_LOW
#define BMA2X2_CONFIG_ENABLE_DUMP_CORE_REG        1

// <Diagnostic Logging>
#ifndef BMA2X2_CONFIG_ENABLE_LOG_LEVEL_1
#define BMA2X2_CONFIG_ENABLE_LOG_LEVEL_1          1
#endif


// <DAE>
#ifndef BMA2X2_CONFIG_ENABLE_DAE
#define BMA2X2_CONFIG_ENABLE_DAE                  0
#endif

// <sensor>
#ifndef BMA2X2_CONFIG_ENABLE_MD_LPM
#define BMA2X2_CONFIG_ENABLE_MD_LPM               1
#define BMA2X2_CONFIG_HANDLE_HW_CONFIG            1
#endif

// <Diagnostic Logging>
#ifndef BMA2X2_CONFIG_ENABLE_DIAG_LOG
#define BMA2X2_CONFIG_ENABLE_DIAG_LOG             1
#endif

// <registry>
#ifndef BMA2X2_CONFIG_ENABLE_REGISTRY
#define BMA2X2_CONFIG_ENABLE_REGISTRY             1
#endif

// <power rail reference>
#ifndef BMA2X2_CONFIG_ENABLE_POWER_RAIL
#define BMA2X2_CONFIG_ENABLE_POWER_RAIL           1
#endif

#endif

#endif /* SNS_SEE_BMA2X2_CONFIG_H */
