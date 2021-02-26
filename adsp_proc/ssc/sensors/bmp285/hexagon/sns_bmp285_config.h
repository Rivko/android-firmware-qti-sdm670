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

#ifndef __SNS_DD_BMP_CONFIG_H
#define __SNS_DD_BMP_CONFIG_H


// <Registry Sensors.SEE-LITE sensor>
#ifndef BMP_CONFIG_ENABLE_SEE_LITE
#define BMP_CONFIG_ENABLE_SEE_LITE   0
#endif

#ifndef BMP_CONFIG_ENABLE_REGISTRY
#define BMP_CONFIG_ENABLE_REGISTRY   1
#endif

#if !BMP_CONFIG_ENABLE_REGISTRY
#ifndef BMP_CONFIG_DFT_BUS_SPI
#define BMP_CONFIG_DFT_BUS_SPI    0
#endif
#endif

// <Debug Print Messages>
#ifndef BMP_CONFIG_ENABLE_DEBUG
#define BMP_CONFIG_ENABLE_DEBUG   1
#endif

// <DAE Usage>
#ifndef BMP_CONFIG_ENABLE_DAE
#define BMP_CONFIG_ENABLE_DAE     0
#endif


// <Diagnostic Logging>
#ifndef BMP_CONFIG_ENABLE_DIAG_LOG
#define BMP_CONFIG_ENABLE_DIAG_LOG   0
#endif

// <power rail reference>
#ifndef BMP_CONFIG_POWER_RAIL
#define BMP_CONFIG_POWER_RAIL        1
#endif

#ifndef BMP_CONFIG_ENABLE_ISLAND_MODE
#define BMP_CONFIG_ENABLE_ISLAND_MODE   1
#endif


#ifndef BMP_CONFIG_ENABLE_SELF_TEST_FAC
#define BMP_CONFIG_ENABLE_SELF_TEST_FAC   1
#endif

#endif

