/*
 * Copyright (c) 2017, ams AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef __SNS_TMX4903_SENSOR___H__
#define __SNS_TMX4903_SENSOR___H__

#include "string.h"
#include <limits.h>
#include "msg.h"
#include "pb_decode.h"
#include "pb_encode.h"
#include "sns_async_com_port.pb.h"
#include "sns_async_com_port_pb_utils.h"
#include "sns_attribute_util.h"
#include "sns_com_port_types.h"
#include "sns_data_stream.h"
#include "sns_diag.pb.h"
#include "sns_diag_service.h"
#include "sns_event_service.h"
#include "sns_interrupt.pb.h"
#include "sns_math_util.h"
#include "sns_mem_util.h"
#include "sns_proximity.pb.h"
#include "sns_pb_util.h"
#include "sns_physical_sensor_test.pb.h"
#include "sns_pwr_rail_service.h"
#include "sns_rc.h"
#include "sns_register.h"
#include "sns_request.h"
#include "sns_sensor.h"
#include "sns_sensor_event.h"
#include "sns_sensor_instance.h"
#include "sns_sensor_uid.h"
#include "sns_sensor_util.h"
#include "sns_service.h"
#include "sns_service_manager.h"
#include "sns_std.pb.h"
#include "sns_std_event_gated_sensor.pb.h"
#include "sns_std_sensor.pb.h"
#include "sns_stream_service.h"
#include "sns_suid.pb.h"
#include "sns_sync_com_port_service.h"
#include "sns_time.h"
#include "sns_timer.pb.h"
#include "sns_cal.pb.h"
#include "sns_types.h"
#include "sns_registry_util.h"
#include "sns_tmx4903_version.h"
#ifdef ENABLE_DEBUG
#include "sns_printf.h"
#else
#define SNS_PRINTF(...)
#define SNS_INST_PRINTF(...)
#endif


#ifndef AMS_PORT_PLATFORM_H
#define AMS_PORT_PLATFORM_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef ENDIAN_LITTLE
#define AMS_ENDIAN_1    0
#define AMS_ENDIAN_2    8
#else
#define AMS_ENDIAN_2    0
#define AMS_ENDIAN_1    8
#endif

#ifdef ENABLE_DEBUG
    #if  DAL_CONFIG_TARGET_ID >= 0x8996
    #else
    #endif
#endif

#define AMS_BUILD /* define ams , undef custom */

typedef struct AMS_PORT_portHndl_s {
    sns_sync_com_port_service   *scp_service;
    sns_sync_com_port_handle    scp_handle;
    // for debug use:
    sns_diag_service            *diag;
} AMS_PORT_portHndl;

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
#endif
#ifndef CONFIG_AMS_OPTICAL_SENSOR_PROX
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
#define CONFIG_AMS_OPTICAL_SENSOR_PROX
#endif
#endif

#define AMS_PORT_get_timestamp_usec(a)

extern void AMS_PORT_timer_callback(void * callBack, uint64_t timeout);
extern uint8_t AMS_PORT_getByte(AMS_PORT_portHndl * portHndl, uint8_t reg_addr, uint8_t * data, uint8_t lenght);
extern uint8_t AMS_PORT_setByte(AMS_PORT_portHndl * portHndl, uint8_t reg_addr, uint8_t * data, uint8_t lenght);
extern void AMS_PORT_suspend(uint32_t timeOut_us);

/* Note:  defining ENABLE_UMSG is against the rules of SEE but as of SEE 5.0.7
 * it still works.  We have numerous legacy calls to these macros that cannot
 * easily be converted to SNS[_INST]_PRINTF because no pointer to a sensor or
 * instance is readily available.  Use it only for debugging, in mainstream use
 * it should always be left undefined.
 */
#define ENABLE_UMSG  1
#if defined(ENABLE_DEBUG) && defined(ENABLE_UMSG)

#ifndef DBG_LOW_PRIO
#define DBG_LOW_PRIO   MSG_LEGACY_LOW
#endif
#ifndef DBG_HIGH_PRIO
#define DBG_HIGH_PRIO  MSG_LEGACY_HIGH
#endif
#ifndef DBG_MED_PRIO
#define DBG_MED_PRIO   MSG_LEGACY_MED
#endif
#ifndef DBG_ERROR_PRIO
#define DBG_ERROR_PRIO MSG_LEGACY_ERROR
#endif

/* The priority param, x, is ignored; always use DBG_HIGH_PRIO to insure mini-dm shows it */
#define AMS_PORT_log_Msg(x, a)                  UMSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AMS: - <" a ">")
#define AMS_PORT_log_Msg_1(x, a, b)             UMSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AMS: - <" a ">", b)
#define AMS_PORT_log_Msg_2(x, a, b, c)          UMSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AMS: - <" a ">", b, c)
#define AMS_PORT_log_Msg_3(x, a, b, c, d)       UMSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AMS: - <" a ">", b, c, d)
#define AMS_PORT_log_Msg_4(x, a, b, c, d, e)    UMSG_4(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AMS: - <" a ">", b, c, d, e)
#define AMS_PORT_log_Msg_5(x, a, b, c, d, e, f) UMSG_5(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AMS: - <" a ">", b, c, d, e, f)
#else //ENABLE_DEBUG
#define AMS_PORT_log_Msg(x, a)
#define AMS_PORT_log_Msg_1(x, a, b)
#define AMS_PORT_log_Msg_2(x, a, b, c)
#define AMS_PORT_log_Msg_3(x, a, b, c, d)
#define AMS_PORT_log_Msg_4(x, a, b, c, d, e)
#define AMS_PORT_log_Msg_5(x, a, b, c, d, e, f)
#endif //ENABLE_DEBUG

#ifdef __cplusplus
}
#endif

#endif /* AMS_PORT_PLATFORM_H */


/*
 * Device Algorithm ALS
 */

#ifndef __AMS_DEVICE_ALG_ALS_API_H__
#define __AMS_DEVICE_ALG_ALS_API_H__

#ifdef __TESTBENCH__
#endif
#ifdef VXMICRO_ARCH_Intel
#endif
#ifdef __KERNEL__
#endif
#if defined(QDSP6) || defined (ARCH_QDSP6)
#endif
#ifdef __KERNEL__
#endif
#ifdef QT_CORE_LIB
#endif
#if defined(__GNUC__) && !defined(__KERNEL__)
#endif

#define AMS_LUX_AVERAGE_COUNT    8

typedef enum _amsAlsAdaptive {
    ADAPTIVE_ALS_NO_REQUEST,
    ADAPTIVE_ALS_TIME_INC_REQUEST,
    ADAPTIVE_ALS_TIME_DEC_REQUEST,
    ADAPTIVE_ALS_GAIN_INC_REQUEST,
    ADAPTIVE_ALS_GAIN_DEC_REQUEST
}amsAlsAdaptive_t;

typedef enum _amsAlsStatus {
    ALS_STATUS_IRQ  = (1 << 0),
    ALS_STATUS_RDY  = (1 << 1),
    ALS_STATUS_OVFL = (1 << 2)
}amsAlsStatus_t;

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
typedef struct _alsData{
    uint16_t clearADC;
    uint16_t redADC;
    uint16_t greenADC;
    uint16_t blueADC;
#if defined(CONFIG_AMS_ALS_CRGBW)
    uint16_t widebandADC;
#elif defined (CONFIG_AMS_ALS_CRGIB)
    uint16_t irADC;
    uint16_t grnADCPrev;
#endif
} alsData_t;
#endif
#if defined CONFIG_AMS_OPTICAL_SENSOR_ALS_CLEAR
typedef struct _alsData{
    uint16_t ch0ADC;
    uint16_t ch1ADC;
} alsData_t;
#endif

typedef struct _amsAlsCalibration {
    uint32_t Time_base; /* in uSec */
    uint32_t adcMaxCount;
    uint16_t calibrationFactor; /* default 1000 */
    uint8_t thresholdLow;
    uint8_t thresholdHigh;
#if defined CONFIG_AMS_OPTICAL_SENSOR_ALS_CLEAR
    int32_t D_factor;
    int32_t L0_factor;
    int32_t L1_factor;
    int32_t L2_factor;
    int32_t L3_factor;
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
    int32_t Cc;
    int32_t Rc;
    int32_t Gc;
    int32_t Bc;
#ifdef CONFIG_AMS_ALS_CRGBW
    int32_t Wbc;
    int32_t C_coef;
#endif
#ifdef CONFIG_AMS_ALS_CRGIB
    int32_t IRc;
    int32_t C_coef;
#endif
#ifdef CONFIG_AMS_ALS_CRGB
    int32_t C_coef;
#endif
    int32_t R_coef;
    int32_t G_coef;
    int32_t B_coef;
    int32_t D_factor;
    int32_t CT_coef;
    int32_t CT_offset;
    int32_t targetR;
    int32_t targetG;
    int32_t targetB;
#endif
#ifdef CONFIG_AMS_ALS_CRWBI
    int32_t Wbc;
    int32_t Wideband_C_factor;
    int32_t Wideband_R_factor;
    int32_t Wideband_B_factor;
#endif
} amsAlsCalibration_t;

typedef struct _amsAlsInitData {
    bool adaptive;
    bool irRejection;
    uint32_t time_us;
    uint32_t gain;
    amsAlsCalibration_t calibration;
} amsAlsInitData_t;

typedef struct _amsALSConf {
    uint32_t time_us;
    uint32_t gain;
    uint8_t thresholdLow;
    uint8_t thresholdHigh;
} amsAlsConf_t;

typedef struct _amsAlsDataSet {
#if defined(CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB) || defined(CONFIG_AMS_OPTICAL_SENSOR_ALS_CLEAR)
    alsData_t *datasetArray;
#endif
    uint64_t timeStamp;
    uint8_t status;
} amsAlsDataSet_t;

typedef struct _amsAlsResult {
    uint32_t    irrClear;
    uint32_t    irrRed;
    uint32_t    irrGreen;
    uint32_t    irrBlue;
    uint32_t    IR;
    uint32_t    irrWideband;
    uint32_t    mLux;
    uint32_t    mLux_ave;
    uint32_t    mMaxLux;
    uint32_t    CCT;
    amsAlsAdaptive_t adaptive;
    uint32_t    rawClear;
    uint32_t    rawRed;
    uint32_t    rawGreen;
    uint32_t    rawBlue;
    uint32_t    rawIR;
    uint32_t    quality;
/* 2 bits per slot = 4 quality states */
#define ALS_QUALITY_LUX_MASK    0x00000003 /* QC - why are there duplicate definitions? */
#define ALS_QUALITY_CCT_MASK    0x0000000c
/* Define BAD & GOOD now leaving 2 states for future use */
#define ALS_QUALITY_LUX_BAD     0x00000000
#define ALS_QUALITY_LUX_GOOD    0x00000003
#define ALS_QUALITY_CCT_BAD     0x00000000
#define ALS_QUALITY_CCT_GOOD    0x0000000c
}amsAlsResult_t;

typedef struct _amsAlsContext {
    uint64_t lastTimeStamp;
    uint32_t ave_lux[AMS_LUX_AVERAGE_COUNT];
    uint32_t ave_lux_index;
    uint32_t cpl;
    uint32_t time_us;
    amsAlsCalibration_t calibration;
    amsAlsResult_t results;
    bool adaptive;
    uint16_t saturation;
    uint32_t gain;
    uint32_t previousGain;
    uint32_t previousLux;
} amsAlsContext_t;

typedef struct _amsAlsAlgInfo {
    char * algName;
    uint16_t contextMemSize;
    uint16_t scratchMemSize;
    amsAlsCalibration_t calibrationData;
    int (*initAlg) (amsAlsContext_t * ctx, amsAlsInitData_t * initData);
    int (*processData) (amsAlsContext_t * ctx, amsAlsDataSet_t * inputData);
    int (*getResult) (amsAlsContext_t * ctx, amsAlsResult_t * outData);
    int (*setConfig) (amsAlsContext_t * ctx, amsAlsConf_t * inputData);
    int (*getConfig) (amsAlsContext_t * ctx, amsAlsConf_t * outputData);
} amsAlsAlgoInfo_t;

extern int amsAlg_als_getAlgInfo (amsAlsAlgoInfo_t * info);
extern int amsAlg_als_initAlg (amsAlsContext_t * ctx, amsAlsInitData_t * initData);
extern int amsAlg_als_processData(amsAlsContext_t * ctx, amsAlsDataSet_t * inputData);
extern int amsAlg_als_getResult(amsAlsContext_t * ctx, amsAlsResult_t * outData);
extern int amsAlg_als_setConfig(amsAlsContext_t * ctx, amsAlsConf_t * inputData);
extern int amsAlg_als_getConfig(amsAlsContext_t * ctx, amsAlsConf_t * outputData);

#endif


/*
 * Device Algorithm ALS
 */

#ifndef __AMS_DEVICE_ALG_ALS_CORE_H__
#define __AMS_DEVICE_ALG_ALS_CORE_H__

#ifdef __KERNEL__
#else
#endif

#ifndef ULLONG_MAX
#define ULLONG_MAX        ((uint64_t)-1)
#endif

#if defined CONFIG_AMS_OPTICAL_SENSOR_ALS_CLEAR || defined CONFIG_AMS_OPTICAL_SENSOR_259x
extern void als_compute_data_clear (amsAlsContext_t * ctx, amsAlsDataSet_t * inputData);
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
extern void als_compute_data (amsAlsContext_t * ctx, amsAlsDataSet_t * inputData);
#endif
extern void als_update_statics(amsAlsContext_t * ctx);
extern void als_ave_LUX (amsAlsContext_t * ctx);
extern amsAlsAdaptive_t als_adaptive(amsAlsContext_t * ctx, amsAlsDataSet_t * inputData);

#endif


/*
 * Core Control Block Dynamic Proximity
 */

#ifndef __AMS_CCB_CORE_CONROL_BLOCK_DYN_PROX_H__
#define __AMS_CCB_CORE_CONROL_BLOCK_DYN_PROX_H__

#ifdef __TESTBENCH__
#elif defined VXMICRO_ARCH_Intel
#endif
#if defined(QDSP6) || defined (ARCH_QDSP6)
#endif
#ifdef QT_CORE_LIB
#endif
#ifdef __KERNEL__
#endif

#define AMS_PRX_AVERAGE_COUNT                       8
#define AMS_PRX_DEFAULT_CYCLE_TIME_USEC             44480 /* 2 quanta less than 50 ms */
#define AMS_PRX_DEFAULT_PULSE_COUNT                 16
#define AMS_PRX_DEFAULT_PULSE_LENGTH                2 /* 16 uSec */
#define AMS_PRX_DEFAULT_LED_CURRENT                 100 /* mA */
/* For avg. active prox current:
 *   avg uA = 100 + 1000 * (LED avg. mA)
 *   LED avg. mA = (duty factor) * (LED drive mA)
 *   duty factor = (((pulse ct. * len (us)) / (cycle len (us))
 *   cycle len us = (ccbCtx->initData.ptime_us / AMS_PRX_TIMEBASE) * AMS_PRX_TIMEBASE
 *                = (44480 / 2780) * 2780
 *                = 44480 us
 *   duty factor = (16 pulses * 16 us) / 44480 us
 *               = 0.0058
 *   LED drive mA = 10 + (((requested mA) - 10) / 20) * 20
 *                = 10 + (90 / 20) * 20
 *                = 10 + 80
 *                = 90 mA
 *   LED avg. mA = 0.0058 * 90
 *               = 0.52 mA
 *   avg. uA = 100 + (1000 * 0.52)
 *           = 620 uA
 *   round up:  650 uA
 */
#define AMS_PRX_DEFAULT_AVG_CURRENT_uA              650 /* uA */
#define AMS_PRX_DEFAULT_GAIN                        4000
#define AMS_PRX_DEFAULT_PERS                        2

#define AMS_PRX_DEFAULT_ADC_THRESHOLD               0
#define AMS_PRX_DEFAULT_ADAPTIVE_THRESHOLD_FACTOR   2

/* For parts with a prox ADC of less than 16 bits, these numbers still
 * apply given that prox data is put thru the AMS_NORMALISE_16() method.
 */
#define AMS_PRX_DEFAULT_NEARBY_HARD_THRESHOLD       0x7500
#define AMS_PRX_DEFAULT_THRESHOLD_HYSTERESIS        0x2000

#define HIGH    0xFF
#define LOW     0x00

#ifndef MIN
#define MIN(a,b) (((a)<(b)) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a,b) (((a)>(b)) ? (a) : (b))
#endif

typedef enum{
    PRX_STATE_UNKNOWN,
    PRX_STATE_BEGIN,
    PRX_STATE_CAL,
    PRX_STATE_FILLUP,
    PRX_STATE_OPER,
    PRX_STATE_FIRST_MEASURE,
    PRX_STATE_STATIC_NO_DETECT,
    PRX_STATE_STATIC_DETECT,
    PRX_STATE_FACTORY_CALIBRATION,
    PRX_STATE_LAST
}ams_prox_dyn_state_t;

typedef struct {
    bool     dynamicMode;
    bool     calibrate;
    uint32_t ptime_us;
    uint32_t gain;
    uint16_t nearByHardThreshold;
    uint16_t nearByThresholdHysteresis;
    uint8_t  adcThreshold;
    uint8_t  numberAdcBits;
}ams_ccb_proximity_init_t;

typedef struct {
    ams_ccb_proximity_init_t initData;
    ams_prox_dyn_state_t state;
    bool nearBy;
    uint32_t pDataNorm;
    uint32_t pDataNormLast;
    uint32_t pDataNormTrigger;
    uint32_t pDataAve;
    uint32_t pDataLow;
    uint32_t pDataMax;
    uint16_t ave[AMS_PRX_AVERAGE_COUNT];
    uint16_t pData;
    uint8_t  sensitivity;
    uint8_t  ave_index;
    uint8_t  adcThreshold;
}ams_ccb_proximity_ctx_t;

typedef struct {
    uint8_t sensitivity;
}ams_ccb_proximity_config_t;

typedef struct {
    uint32_t alsData;
    uint16_t pData;
    uint8_t  statusReg;
}ams_ccb_proximity_dataSet_t;

typedef struct {
    bool nearBy;
    uint16_t proximity;
}ams_ccb_proximity_result_t;

typedef struct {
    uint32_t hardThreshold;
    uint16_t hardThresholdDistanceMM;
}ams_ccb_proximity_calibration_t;

typedef struct {
    char *algName;
    uint16_t contextMemSize;
    uint16_t scratchMemSize;
    ams_ccb_proximity_calibration_t defaultCalibrationData;
}ams_ccb_proximity_info_t;

extern void ccb_proximityInfo(ams_ccb_proximity_info_t *infoData);
extern void ccb_proximityInit(void *dcbCtx, ams_ccb_proximity_init_t *initData);
extern void ccb_proximitySetConfig(void *dcbCtx, ams_ccb_proximity_config_t *configData);
extern bool ccb_proximityHandle(void *dcbCtx, ams_ccb_proximity_dataSet_t *data);
extern void ccb_proximityGetResult(void *dcbCtx, ams_ccb_proximity_result_t *result);

#endif


/*
 * Core Control Block ALS
 */

#ifndef __AMS_CCB_CORE_CONROL_BLOCK_ALS_H__
#define __AMS_CCB_CORE_CONROL_BLOCK_ALS_H__

#ifdef __TESTBENCH__
#elif defined VXMICRO_ARCH_Intel
#endif
#if defined(QDSP6) || defined (ARCH_QDSP6)
#endif
#ifdef QT_CORE_LIB
#endif

#define HIGH    0xFF
#define LOW     0x00

#if !defined(CONFIG_ALS_CAL_TARGET)
#define CONFIG_ALS_CAL_TARGET          300 /* lux */
#endif
#if !defined(CONFIG_ALS_CAL_TARGET_TOLERANCE)
#define CONFIG_ALS_CAL_TARGET_TOLERANCE  15 /* lux */
#endif

typedef struct {
    uint32_t calibrationFactor;
    int32_t luxTarget;
    int8_t luxTargetError;
    uint32_t Time_base; /* in uSec */
    uint32_t adcMaxCount;
    uint8_t thresholdLow;
    uint8_t thresholdHigh;
#if defined CONFIG_AMS_OPTICAL_SENSOR_ALS_CLEAR
    int32_t D_factor;
    int32_t L0_factor;
    int32_t L1_factor;
    int32_t L2_factor;
    int32_t L3_factor;
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
    int32_t Cc;
    int32_t Rc;
    int32_t Gc;
    int32_t Bc;
#ifdef CONFIG_AMS_ALS_CRGBW
    int32_t Wbc;
#endif
#ifdef CONFIG_AMS_ALS_CRGIB
    int32_t IRc;
#endif
#if defined(CONFIG_AMS_ALS_CRGBW) || defined(CONFIG_AMS_ALS_CRGIB) || defined(CONFIG_AMS_ALS_CRGB)
    int32_t C_coef;
#endif
    int32_t R_coef;
    int32_t G_coef;
    int32_t B_coef;
    int32_t D_factor;
    int32_t CT_coef;
    int32_t CT_offset;
    int32_t targetR;
    int32_t targetG;
    int32_t targetB;
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_WIDEBAND
    int32_t Wbc;
    int32_t Wideband_C_factor;
    int32_t Wideband_R_factor;
    int32_t Wideband_B_factor;
#endif
}ams_ccb_als_calibration_t;

typedef struct {
    uint32_t uSecTime;
    uint32_t gain;
    uint8_t  threshold;
}ams_ccb_als_config_t;

typedef struct {
    bool calibrate;
    bool autoGain;
    uint8_t hysteresis;
    uint16_t  sampleRate;
    ams_ccb_als_config_t configData;
    ams_ccb_als_calibration_t calibrationData;
}ams_ccb_als_init_t;

typedef enum {
    AMS_CCB_ALS_INIT,
    AMS_CCB_ALS_RGB,
    AMS_CCB_ALS_AUTOGAIN,
    AMS_CCB_ALS_CALIBRATION_INIT,
    AMS_CCB_ALS_CALIBRATION_COLLECT_DATA,
    AMS_CCB_ALS_CALIBRATION_CHECK,
    AMS_CCB_ALS_LAST_STATE
}ams_ccb_als_state_t;

typedef struct {
    char * algName;
    uint16_t contextMemSize;
    uint16_t scratchMemSize;
    ams_ccb_als_calibration_t defaultCalibrationData;
}ams_ccb_als_info_t;

typedef struct {
    ams_ccb_als_state_t state;
    amsAlsContext_t ctxAlgAls;
    ams_ccb_als_init_t initData;
    uint16_t bufferCounter;
    uint16_t shadowAiltReg;
    uint16_t shadowAihtReg;
}ams_ccb_als_ctx_t;

typedef struct {
    uint8_t  statusReg;
}ams_ccb_als_dataSet_t;

typedef struct {
    uint32_t mLux;
    uint32_t colorTemp;
    uint32_t clear;
    uint32_t red;
    uint32_t green;
    uint32_t blue;
    uint32_t ir;
    uint32_t wideband;
    uint32_t rawClear;
    uint32_t rawRed;
    uint32_t rawGreen;
    uint32_t rawBlue;
    uint32_t rawIR;
    uint32_t quality;
}ams_ccb_als_result_t;

typedef struct _fifo{
    uint16_t AdcClear;
    uint16_t AdcRed;
#if defined(CONFIG_AMS_ALS_CRWBI)
    uint16_t AdcGreenWb;
#else
    uint16_t AdcGreen;
#endif
    uint16_t AdcBlue;
#if defined(CONFIG_AMS_ALS_CRGBW)
    uint16_t AdcWb;
#endif
#if defined(CONFIG_AMS_ALS_CRGIB)
    uint16_t AdcIR;
    uint16_t AdcGrnPrev;
#endif
} adcDataSet_t;

extern void ccb_alsInit(void * dcbCtx, ams_ccb_als_init_t * initData);
extern void ccb_alsInfo(ams_ccb_als_info_t* infoData);
extern void ccb_alsGetConfig(void * dcbCtx, ams_ccb_als_config_t * configData);
extern void ccb_alsSetConfig(void * dcbCtx, ams_ccb_als_config_t * configData);
extern bool ccb_alsHandle(void * dcbCtx, ams_ccb_als_dataSet_t * data);
extern void ccb_alsGetResult(void * dcbCtx, ams_ccb_als_result_t * result);

#endif


/*
 * Device Control Block 490x
 */

#ifndef __AMS_DEVICE_CONTROL_BLOCK_H__
#define __AMS_DEVICE_CONTROL_BLOCK_H__

/* This DCB code implements the ALWAYS_READ option unconditionally;
 * this symbol is for the benefit of, e.g., the ALS CCB which must
 * also work with other DCBs that do not (yet) support it.
 */
#define HAVE_OPTION__ALWAYS_READ  1

#ifdef __KERNEL__
#endif
#ifdef __TESTBENCH__
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_IRBEAM_CCB
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_GESTURE
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
#define AMS_PRX_TIMEBASE (2780) /* in usec, see data sheet */

typedef struct {
    bool     nearBy;
    uint16_t proximity;
} ams_apiPrx_t;
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_CCB
typedef struct {
    uint32_t mLux;
    uint32_t colorTemp;
    uint32_t red;
    uint32_t green;
    uint32_t clear;
    uint32_t blue;
    uint32_t ir;
    uint32_t wideband;
    uint32_t rawClear;
    uint32_t rawRed;
    uint32_t rawGreen;
    uint32_t rawBlue;
    uint32_t quality;
/* 2 bits per slot can hold 4 quality states */
#define ALS_QUALITY_LUX_MASK    0x00000003
#define ALS_QUALITY_CCT_MASK    0x0000000c
/* Define BAD & GOOD now leaving 2 states for future use */
#define ALS_QUALITY_LUX_BAD     0x00000000
#define ALS_QUALITY_LUX_GOOD    0x00000003
#define ALS_QUALITY_CCT_BAD     0x00000000
#define ALS_QUALITY_CCT_GOOD    0x0000000c
} ams_apiAls_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define AMS_DEVICE_OFF_TO_IDLE_MS       10
#define AMS_USEC_PER_TICK               (2780)
#define AMS_ALS_MAX_COUNTS_PER_TICK     (1024)
#define AMS_ALS_MAX_COUNTS              (65535)

#ifndef UINT_MAX_VALUE
#define UINT_MAX_VALUE      (-1)
#endif

#define AMS_CALIBRATION_DONE                (-1)
#define AMS_CALIBRATION_DONE_BUT_FAILED     (-2)

#define AMS_ADAPTIVE_ATIME_THRESHOLD_LOW  (100)    /* in counts */
#define AMS_ADAPTIVE_ATIME_THRESHOLD_HIGH (2000)   /* in counts */
#define AMS_ADAPTIVE_ATIME_SHORT          (100000) /* in usec */
#define AMS_ADAPTIVE_ATIME_LONG           (700000) /* in usec */

typedef enum _deviceIdentifier_e {
    AMS_UNKNOWN_DEVICE,
    AMS_TMD4904,
    AMS_TMD4903,
    AMS_TMG4903,
    AMS_LAST_DEVICE
} ams_deviceIdentifier_e;

#define AMS_PRX_PERS_TO_REG(x)         (x << 4)
#define AMS_PRX_REG_TO_PERS(x)         (x >> 4)
#define AMS_ALS_PERS_TO_REG(x)         (x << 0)
#define AMS_ALS_REG_TO_PERS(x)         (x >> 0)

typedef enum _deviceRegisters {
    DEVREG_ENABLE,      /* 0x80 */
    DEVREG_ATIME,
    DEVREG_PTIME,
    DEVREG_WTIME,
    DEVREG_AILTHL,
    DEVREG_AILTH,
    DEVREG_AIHTHL,
    DEVREG_AIHTH,
    DEVREG_PILTL,
    DEVREG_PILTH,
    DEVREG_PIHTL,
    DEVREG_PIHTH,
    DEVREG_PERS,
    DEVREG_CFG0,
    DEVREG_PGCFG0,
    DEVREG_PGCFG1,
    DEVREG_CFG1,
    DEVREG_REVID,
    DEVREG_ID,
    DEVREG_STATUS,      /* 0x93 */
    DEVREG_CLR_CHANLO,
    DEVREG_CLR_CHANHI,
    DEVREG_RED_CHANLO,
    DEVREG_RED_CHANHI,
    DEVREG_GRN_CHANLO,
    DEVREG_GRN_CHANHI,
    DEVREG_BLU_CHANLO,
    DEVREG_BLU_CHANHI,
    DEVREG_PDATA_LOW,
    DEVREG_PDATA_HI,
    DEVREG_STATUS2,
    DEVREG_CFG2,

    DEVREG_IRBEAM_0,    /* 0xA0 */
    DEVREG_IRBEAM_1,
    DEVREG_IRBEAM_2,
    DEVREG_IRBEAM_3,
    DEVREG_IRBEAM_4,
    DEVREG_IRBEAM_5,
    DEVREG_IRBEAM_6,
    DEVREG_IRBEAM_7,
    DEVREG_IRBEAM_8,
    DEVREG_IRBEAM_9,

    DEVREG_CFG3,        /* 0xAB */
    DEVREG_CFG4,
    DEVREG_CFG5,

    DEVREG_CFG7,        /* 0xAF */

#ifdef CONFIG_AMS_OPTICAL_SENSOR_GESTURE
    DEVREG_GESTURE_0,   /* 0xB0 */
    DEVREG_GESTURE_1,
    DEVREG_GESTURE_2,
    DEVREG_GESTURE_3,
    DEVREG_GESTURE_4,
    DEVREG_GESTURE_5,
    DEVREG_GESTURE_6,
    DEVREG_GESTURE_7,
    DEVREG_GESTURE_8,
    DEVREG_GESTURE_9,
    DEVREG_GESTURE_A,
    DEVREG_GESTURE_B,
#endif

    DEVREG_CONTROL,      /* 0xBC */
    DEVREG_AUXID,

    DEVREG_OFFSET_NL,    /* 0xC0 */
    DEVREG_OFFSET_NH,
    DEVREG_OFFSET_SL,
    DEVREG_OFFSET_SH,
    DEVREG_OFFSET_WL,
    DEVREG_OFFSET_WH,
    DEVREG_OFFSET_EL,
    DEVREG_OFFSET_EH,

    DEVREG_PBSLN_MEASL, /* 0xD0 */
    DEVREG_PBSLN_MEASH,
    DEVREG_CALIB_DCAVG, /* 0xD2 */
    DEVREG_CALIB_DCAVG_H, /* 0xD3 */

    DEVREG_AZ_CONFIG,   /* 0xD6 */
    DEVREG_CALIB,       /* 0xD7 */
    DEVREG_CAL_CFG0,
    DEVREG_CAL_CFG1,
    DEVREG_CAL_CFG2,
    DEVREG_CAL_CFG3,
    DEVREG_CAL_CFGSTAT,

    DEVREG_INT_ENBL,    /* 0xDD */
    DEVREG_INT_CLEAR,   /* 0xDE */

#ifdef CONFIG_AMS_OPTICAL_SENSOR_GESTURE
    DEVREG_GFIFO_N_L,   /* 0xF8 */
    DEVREG_GFIFO_N_H,
    DEVREG_GFIFO_S_L,
    DEVREG_GFIFO_S_H,
    DEVREG_GFIFO_W_L,
    DEVREG_GFIFO_W_H,
    DEVREG_GFIFO_E_L,
    DEVREG_GFIFO_E_H,
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_IRBEAM_CCB
    DEVREG_RAM_START,
#endif
    DEVREG_REG_MAX
}ams_deviceRegister_t;

typedef enum _490x_regOptions {

    PON             = 0x01,  /* register 0x80 */
    AEN             = 0x02,
    PEN             = 0x04,
    WEN             = 0x08,
    AIEN            = 0x10,
    PIEN            = 0x20,
    GEN             = 0x40,
    FIEN            = 0x40,
    IBEN            = 0x80,
    ALS_ENABLE_ALL  = (AEN+AIEN),
    AMS_ALL_IENS    = (AIEN+PIEN),

    WLOG            = 0x04,
    LOWPWR_IDLE     = 0x20,
    RAM_BANK_0      = 0x00,
    RAM_BANK_1      = 0x01,
    RAM_BANK_2      = 0x02,
    RAM_BANK_3      = 0x03,

    PPLEN_4uS       = 0x00,  /* register 0x8E */
    PPLEN_8uS       = 0x40,
    PPLEN_16uS      = 0x80,
    PPLEN_32uS      = 0xC0,

    PGAIN_1         = 0x00,  /* register 0x8F */
    PGAIN_2         = 0x40,
    PGAIN_4         = 0x80,
    PGAIN_8         = 0xC0,

    AGAIN_1         = 0x00,  /* register 0x90 */
    AGAIN_4         = 0x01,
    AGAIN_16        = 0x02,
    AGAIN_64        = 0x03,
    PSIEN           = 0x80,
    ASIEN           = 0x40,
    DETCT_FLCKR_0   = 0x00,
    DETCT_FLCKR_1   = 0x04,
    DETCT_FLCKR_2   = 0x08,
    DETCT_FLCKR_3   = 0x0C,
    ENABLE_WB       = 0x10,

    CINT            = 0x02, /* register 0x93 */
#ifdef CONFIG_AMS_OPTICAL_SENSOR_GESTURE
    GINT            = 0x04,
#endif
    IINT            = 0x08,
    AINT            = 0x10,
    PINT            = 0x20,
    PGSAT           = 0x40,
    ASAT            = 0x80,
    ALS_INT_ALL     = AINT,

    PGSAT_AMB       = 0x01, /* register 0x9E */
    PGSAT_REFL      = 0x02,
    PGSAT_ADC       = 0x04,
    ASAT_ANALOG     = 0x08,
    ASAT_DIGITAL    = 0x10,
    AVALID          = 0x40,
    PVALID          = 0x80,

    PMSK_E          = 0x80,
    PMSK_W          = 0x40,
    PMSK_S          = 0x20,
    PMSK_N          = 0x10,
    AMSK            = 0x08,

    AUTO_WB_DIODE   = 0x80, /* 0xAF */
    GST_EXMSK_ALL   = 0x00, /* 0xB0 */

    BINSRC_SKIPB    = 0x08, /* register 0xD8 */
    ELECT_CAL       = 0x10,
    DCAVG_AUTO_OFF_ADJ = 0x40,

    LAST_IN_ENUM_LIST
}ams_regOptions_t;

typedef enum _490x_regMasks {
    MASK_PON            = 0x01, /* register 0x80 */
    MASK_AEN            = 0x02,
    MASK_PEN            = 0x04,
    MASK_WEN            = 0x08,
    MASK_AIEN           = 0x10,
    MASK_PIEN           = 0x20,
#ifdef CONFIG_AMS_OPTICAL_SENSOR_GESTURE
    MASK_GEN            = 0x40,
#endif
    MASK_PBEN           = 0x80,

    MASK_ATIME          = 0xFF, /* register 0x81 */

    MASK_PTIME          = 0xFF, /* register 0x82 */

    MASK_WTIME          = 0xFF, /* register 0x83 */

    MASK_AILT           = 0xFFFF, /* register 0x84 */
    MASK_AILH           = 0xFFFF, /* register 0x86 */

    MASK_PILT           = 0xFFFF, /* register 0x88 */
    MASK_PILH           = 0xFFFF, /* register 0x8A */

    MASK_APERS          = 0x0F, /* register 0x8C */
    MASK_PPERS          = 0xF0,

    MASK_WLONG          = 0x02, /* register 0x8D */
    MASK_LOWPWR_IDLE    = 0x40,
    MASK_RAM_BANK       = 0x03,

    MASK_PPULSE         = 0x3F, /* register 0x8e */
    MASK_PPLEN          = 0xC0,

    MASK_PGAIN          = 0xC0, /* register 0x8f */
    MASK_PGLDRIVE       = 0x1E,

    MASK_AGAIN          = 0x03, /* register 0x90 */
    MASK_ASIEN          = 0x40,
    MASK_PGSIEN         = 0x80,
    MASK_DETCT_FLCKR    = 0x0C,
    MASK_ENABLE_WB      = 0x10,

    MASK_REV_ID         = 0x07, /* register 0x91 */

    MASK_ID             = 0xFC, /* register 0x92 */

    MASK_CINT           = 0x02, /* register 0x93 */
#ifdef CONFIG_AMS_OPTICAL_SENSOR_GESTURE
    MASK_GINT           = 0x04,
#endif
    MASK_IINT           = 0x08,
    MASK_AINT           = 0x10,
    MASK_PINT           = 0x20,
    MASK_PGSAT          = 0x40,
    MASK_ASAT           = 0x80,
    MASK_AGDATA         = 0x01,
    MASK_ALS_INT_ALL    = MASK_AINT,
    MASK_RELEVANT_INTS  = (0
#ifdef CONFIG_AMS_OPTICAL_SENSOR_GESTURE
                             | 0x04
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
                             | 0x62
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_CCB
                             | 0x10
#endif
                          ),

    MASK_CDATA          = 0xFFFF, /* register 0x94 */
    MASK_RDATA          = 0xFFFF, /* register 0x96 */
    MASK_GDATA          = 0xFFFF, /* register 0x98 */
    MASK_BDATA          = 0xFFFF, /* register 0x9A */
    MASK_PDATA          = 0xFFFF, /* register 0x9C */

    MASK_PGSAT_AMB      = 0x01, /* register */
    MASK_PGSAT_REFL     = 0x02,
    MASK_PGSAT_ADC      = 0x04,
    MASK_ASAT_ANALOG    = 0x08,
    MASK_ASAT_DIGITAL   = 0x10,
    MASK_AVALID         = 0x40,
    MASK_PVALID         = 0x80,

    MASK_PMSK_E         = 0x80,
    MASK_PMSK_W         = 0x40,
    MASK_PMSK_S         = 0x20,
    MASK_PMSK_N         = 0x10,
    MASK_AMSK           = 0x08,

    MASK_AUTO_WB_DIODE  = 0x80, /* 0xAF */

    MASK_GST_EXMSK      = 0x3c, /* register 0xB0 */
    MASK_GST_EXPERS     = 0x03,

    MASK_OFFSET_CALIB   = 0x01, /* register 0xD7 */

    MASK_DCAVG_ITR      = 0x07, /* register 0xD8 */
    MASK_BINSRCH_SKIP   = 0x08,
    MASK_ELECT_CAL      = 0x10,
    MASK_DCAVG_AUTO_OFF_ADJ = 0x40,

    MASK_BINSRCH        = 0x70, /* register 0xDA */
    MASK_AUTO_GTH       = 0x0f,
    MASK_AUTO_DCAVG_GTH = 0x80,

    MASK_INTCLR_CINT    = 0x02, /* Register 0xde */
#ifdef CONFIG_AMS_OPTICAL_SENSOR_GESTURE
    MASK_INTCLR_GINT    = 0x04,
#endif
    MASK_INTCLR_IINT    = 0x08,
    MASK_INTCLR_AINT    = 0x10,
    MASK_INTCLR_PINT    = 0x20,
    MASK_INTCLR_PGSAT   = 0x40,
    MASK_INTCLR_ASAT    = 0x80,
    MASK_ALS_INTCLR_ALL = (MASK_INTCLR_AINT+MASK_INTCLR_ASAT),

    MASK_LAST_IN_ENUMLIST
}ams_regMask_t;

/* REENABLE only enables those that were on record as being enabled */
#define AMS_REENABLE()              ams_setEnable(ctx)

#define AMS_CLR_ALS_INT()           {ams_setByte (ctx->portHndl, DEVREG_INT_CLEAR, (AINT | ASAT));}
#define AMS_SET_ALS_TIME(mSec)      {ams_setByte (ctx->portHndl, DEVREG_ATIME,   alsTimeUsToReg(mSec));}
#define AMS_SET_ALS_GAIN(mGain)     {ams_setField(ctx->portHndl, DEVREG_CFG1,    alsGainToReg(mGain),    MASK_AGAIN);}
#define AMS_SET_ALS_THRESHOLDS(tl,th) {uint16_t tvals[2]; tvals[0]=(tl); tvals[1]=(th); ams_setBuf (ctx->portHndl, DEVREG_AILTHL, ((uint8_t*)tvals), 4);}
#define AMS_SET_ALS_THRS_LOW(x)     {ams_setWord (ctx->portHndl, DEVREG_AILTHL, (x));}
#define AMS_SET_ALS_THRS_HIGH(x)    {ams_setWord (ctx->portHndl, DEVREG_AIHTHL, (x));}
#define AMS_SET_ALS_PERS(x)         {ams_setField(ctx->portHndl, DEVREG_PERS, (x), MASK_APERS);}
#define AMS_DISABLE_ALS()           {ams_setField(ctx->portHndl, DEVREG_ENABLE, LOW, (MASK_AEN));}
#define AMS_REENABLE_ALS()          {ams_setField(ctx->portHndl, DEVREG_ENABLE, HIGH, (MASK_AEN));}
#define AMS_ALS_GET_CRGB_W(x)       {ams_getBuf(ctx->portHndl, DEVREG_CLR_CHANLO, (uint8_t *)&adcData, 8);}

#define AMS_CLR_PROX_INT()          {ams_setByte (ctx->portHndl, DEVREG_INT_CLEAR, (PINT | CINT | PGSAT));}
#define AMS_ENABLE_PROX_INT()       {ctx->shadowEnableReg |= (MASK_PINT);}
#define AMS_SET_PROX_TIME(mSec)     {ams_setByte(ctx->portHndl,  DEVREG_PTIME,   proxTimeToReg(mSec));}
#define AMS_SET_PROX_GAIN(mGain)    {ams_setField(ctx->portHndl, DEVREG_PGCFG1,  proxGainToReg(mGain),   MASK_PGAIN);}
#define AMS_SET_PROX_LEDDRIVE(mA)   {ams_setField(ctx->portHndl, DEVREG_PGCFG1,  (((mA)-10)/20) << 1,    0x1e);}
#define AMS_SET_PROX_THRESHOLDS(tl,th) {uint16_t tvals[2]; tvals[0]=(tl); tvals[1]=(th); ams_setBuf (ctx->portHndl, DEVREG_PILTL, ((uint8_t*)tvals), 4);}
#define AMS_SET_PROX_THRS_LOW(x)    {ams_setWord (ctx->portHndl, DEVREG_PILTL, (x));}
#define AMS_SET_PROX_THRS_HIGH(x)   {ams_setWord (ctx->portHndl, DEVREG_PIHTL, (x));}
#define AMS_SET_PROX_PERS(x)        {ams_setField(ctx->portHndl, DEVREG_PERS, (x << 4), MASK_PPERS);}
#define AMS_SET_PROX_PULSE_LEN(x)   {ams_setField(ctx->portHndl, DEVREG_PGCFG0, (x << 6), MASK_PPLEN);}
#define AMS_SET_PROX_PULSE_COUNT(x) {ams_setField(ctx->portHndl, DEVREG_PGCFG0, (x), MASK_PPULSE);}
#define AMS_SET_CAL_PROX_AUTO_OFFSET_ADJUST()  {ams_setField(ctx->portHndl, DEVREG_CAL_CFG1, HIGH, 0x04);}
#define AMS_SET_CAL_BINSRCH(x)      {ams_setField(ctx->portHndl, DEVREG_CAL_CFG2, (x << 4), MASK_BINSRCH);}

#define AMS_SET_IRBEAM_CURRENT(x)   {/* TODO */ ;}

#define AMS_ENABLE_CAL_INT()        {if (ctx->allowIens) ams_setField(ctx->portHndl, DEVREG_INT_ENBL, HIGH, MASK_CINT);}
#define AMS_DISABLE_CAL_INT()       {if (ctx->allowIens) ams_setField(ctx->portHndl, DEVREG_INT_ENBL, LOW, MASK_CINT);}
#define AMS_ENABLE_IRBEAM_INT()     {if (ctx->allowIens) ams_setField(ctx->portHndl, DEVREG_INT_ENBL, HIGH, MASK_IINT);}
#define AMS_DISABLE_IRBEAM_INT()    {if (ctx->allowIens) ams_setField(ctx->portHndl, DEVREG_INT_ENBL, LOW, MASK_IINT);}

enum _tmx4905_calibcfg0 {
    TMX4905_DCAVG_ITERATIONS        = (0x7 << 0),
    TMX4905_BINSRCH_SKIP            = (0x1 << 3),
    TMX4905_ELECTRICAL_CALIBRATION  = (0x1 << 4),
    TMX4905_BINSRCH_NOSUPPRESS_APC  = (0x1 << 5),
    TMX4905_DCAVG_AUTO_OFFSET_ADJUST = (0x1 << 6),
    TMX4905_DCAVG_AUTO_BASELINE     = (0x1 << 7)
};

enum _tmx4905_calibcfg1 {
    TMX4905_PXDCAVG_BASELINE_WINDOW = (0x7 << 0),
    TMX4905_PXDCAVG_AUTO_BASELINE   = (0x1 << 3),
    TMX4905_PROX_AUTO_OFFSET_ADJUST = (0x1 << 6),
    TMX4905_PXDCAVG_AUTO_GTHR       = (0x1 << 7)
};

typedef struct _deviceRegisterTable {
    uint8_t address;
    uint8_t resetValue;
}deviceRegisterTable_t;

typedef enum _490x_config_options {
    AMS_CONFIG_ENABLE,
    AMS_CONFIG_THRESHOLD,
    AMS_CONFIG_GESTURE_ORIENTATION,
    AMS_CONFIG_ADAPTIVE_ATIME,
    AMS_CONFIG_ALWAYS_READ,   /* see .alwaysReadXxx in ams_deviceCtx_t */
    AMS_CONFIG_ALLOW_INTS,  /* tell dev whether OS has connected the IRQ line */
    AMS_CONFIG_OPTION_LAST
}deviceConfigOptions_t;

typedef enum _490x_report_type {
    AMS_REPORT_NONE        = 0x00,
    AMS_REPORT_ON_CHANGE   = 0x01,
    AMS_REPORT_STREAMING   = 0x02,
    AMS_REPORT_OC_AND_STRM = 0x03,
    AMS_REPORT_TYPE_LAST
}ams_reportType_t;

typedef enum _490x_mode {
    MODE_OFF            = (0),
    MODE_ALS_LUX        = (1 << 0),
    MODE_ALS_RGB        = (1 << 1),
    MODE_ALS_CT         = (1 << 2),
    MODE_ALS_IR         = (1 << 3),
    MODE_ALS_WIDEBAND   = (1 << 4),
    MODE_ALS_ALL        = (MODE_ALS_LUX | MODE_ALS_RGB | MODE_ALS_CT | MODE_ALS_IR | MODE_ALS_WIDEBAND),
    MODE_PROX           = (1 << 5),
    MODE_GESTURE        = (1 << 6),
    MODE_IRBEAM         = (1 << 7),
    MODE_UNKNOWN    /* must be in last position */
} ams_mode_t;

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
typedef enum _490x_adaptive_atime_state {
    AMS_ADAPTIVE_ATIME_STATE_SHORT,
    AMS_ADAPTIVE_ATIME_STATE_LONG,
    AMS_ADAPTIVE_ATIME_STATE_LAST
}deviceAdaptiveAtimeState_t;
#endif

typedef enum _490x_configureFeature {
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
    AMS_CONFIG_PROX,
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_CCB
    AMS_CONFIG_ALS_LUX,
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
    AMS_CONFIG_ALS_RGB,
    AMS_CONFIG_ALS_CT,
    AMS_CONFIG_ALS_IR,
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_WIDEBAND
    AMS_CONFIG_ALS_WIDEBAND,
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_GESTURE
    AMS_CONFIG_GESTURE,
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_MOBEAM
    AMS_CONFIG_MOBEAM,
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_REMOTE_CONTROL
    AMS_CONFIG_REMCON,
#endif
    AMS_CONFIG_GLOBAL,
    AMS_CONFIG_FEATURE_LAST
}ams_configureFeature_t;

typedef enum calibration_data_index_e {
    DEVICE_NAME_IDX = 0,
    TIMEBASE_US_IDX,
    ADCMAXCOUNT_IDX,
    ALSTHRESHOLDHIGH_IDX,
    ALSTHRESHOLDLOW_IDX,
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_CCB
    ALSCALIBRATIONLUXTARGETHIGH_IDX,
    ALSCALIBRATIONLUXTARGETLOW_IDX,
    ALSCALIBRATIONLUXTARGET_IDX,
    ALSCALIBRATIONLUXTARGETERROR_IDX,
    ALSCALIBRATIONFACTOR_IDX,
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
    PROXADAPTIVETHRESHOLDFACTOR_IDX,
    PROXADAPTIVETHRESHOLD_IDX,
    PROXHARDTHRESHOLD_IDX,
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
    CC_IDX,
    RC_IDX,
    GC_IDX,
    BC_IDX,
    ALSCOEFR_IDX,
    ALSCOEFG_IDX,
    ALSCOEFB_IDX,
    ALSDFG_IDX,
    ALSCCTOFFSET_IDX,
    ALSCCTCOEF_IDX,
    ALSTARGETR_IDX,
    ALSTARGETG_IDX,
    ALSTARGETB_IDX,
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_WIDEBAND
    WBC_IDX,
    WIDEBAND_C_FACTOR_IDX,
    WIDEBAND_R_FACTOR_IDX,
    WIDEBAND_B_FACTOR_IDX,
#endif
    CALIBRATION_DATA_INDEX_MAX
} calibration_data_index_e;

/*
 * TODO: WARNIGNG ALWAYS UPDATE calibration_data_index_e
 *       after you update calibrationData !!!
 */
typedef struct _calibrationData {
    char        deviceName[8];
    uint32_t    timeBase_us;
    uint32_t    adcMaxCount;
    uint8_t     alsThresholdHigh; /* in % */
    uint8_t     alsThresholdLow;  /* in % */
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_CCB
    uint16_t    alsCalibrationLuxTargetHigh;
    uint16_t    alsCalibrationLuxTargetLow;
    uint16_t    alsCalibrationLuxTarget;
    uint16_t    alsCalibrationLuxTargetError;
    uint16_t    alsCalibrationFactor;        /* multiplicative factor default 1000 */
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
    /* TODO: keep adaptive threshold for now */
    uint16_t    proxAdaptiveThresholdFactor;
    uint8_t     proxAdaptiveThreshold; /* in bits */
    uint16_t    proxHardThreshold;
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
    int32_t     Cc;
    int32_t     Rc;
    int32_t     Gc;
    int32_t     Bc;
    int32_t     alsCoefR;
    int32_t     alsCoefG;
    int32_t     alsCoefB;
    int16_t     alsDfg;
    uint16_t    alsCctOffset;
    uint16_t    alsCctCoef;
    int32_t     targetR;
    int32_t     targetG;
    int32_t     targetB;
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_WIDEBAND
    int32_t     Wbc;
    int32_t     Wideband_C_factor;
    int32_t     Wideband_R_factor;
    int32_t     Wideband_B_factor;
#endif
} ams_calibrationData_t;

typedef struct _490xContext {
    ams_deviceIdentifier_e deviceId;
    uint64_t timeStamp;
    AMS_PORT_portHndl * portHndl;
    ams_mode_t mode;
#ifdef AMS_PHY_SUPPORT_SHADOW
    uint8_t shadow[DEVREG_REG_MAX];
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_GESTURE
    amsGestContext_t gestureCtx;
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
    ams_ccb_proximity_ctx_t ccbProxCtx;
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_CCB
    ams_ccb_als_ctx_t ccbAlsCtx;
    bool adaptiveAtime;           /* true == enabled, false == disabled */
    deviceAdaptiveAtimeState_t atimeState;
    uint16_t adaptiveAtimeLowThreshold;
    uint16_t adaptiveAtimeHighThreshold;
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_IRBEAM_CCB
    ams_ccb_irBeam_ctx_t ccbIrBeamCtx;
#endif

    ams_calibrationData_t * systemCalibrationData;
    bool alwaysReadAls;       /* read ADATA every ams_deviceEventHandler call
                                 regardless of xINT bits */
    bool alwaysReadProx;      /* ditto PDATA */
    uint32_t updateAvailable;
    uint8_t shadowEnableReg;
    uint8_t shadowStatus1Reg;
    uint8_t shadowStatus2Reg;
    bool allowIens;        /* defer interrupt enables until client */
                           /*   says the OS has enabled the line   */
}ams_deviceCtx_t;

typedef enum _sensorType {
    AMS_NO_SENSOR_AVAILABLE,
    AMS_AMBIENT_SENSOR,
    AMS_PROXIMITY_SENSOR,
    AMS_GESTURE_SENSOR,
    AMS_WIDEBAND_ALS_SENSOR,
    AMS_ALS_RGB_GAIN_CHANGED,
    AMS_LAST_SENSOR
}ams_sensorType_t;

typedef struct _sensorInfo {
    uint32_t    standbyCurrent_uA;
    uint32_t    activeCurrent_uA;
    uint32_t    rangeMin;
    uint32_t    rangeMax;
    uint32_t    rangeMinRgb;
    uint32_t    rangeMaxRgb;
    char *      driverName;
    uint8_t     maxPolRate;
    uint8_t     adcBits;
} ams_SensorInfo_t;

typedef struct _deviceInfo {
    uint32_t    memorySize;
    ams_calibrationData_t defaultCalibrationData;
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
    ams_SensorInfo_t proxSensor;
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_CCB
    ams_SensorInfo_t alsSensor;
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_GESTURE
    ams_SensorInfo_t gestureSensor;
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_MOBEAM
    ams_SensorInfo_t mobeamSensor;
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_REMOTE_CONTROL
    ams_SensorInfo_t remconSensor;
#endif
    ams_sensorType_t tableSubSensors[10];
    uint8_t     numberOfSubSensors;
    char *      driverVersion;
    char *      deviceModel;
    char *      deviceName;
}ams_deviceInfo_t;

#if defined CONFIG_AMS_OPTICAL_SENSOR_ALS
extern uint32_t alsGain_conversion[];
extern uint8_t alsGainToReg(uint32_t x);
extern uint8_t alsTimeUsToReg(uint32_t x);
extern void alsSetTime(ams_deviceCtx_t * ctx, uint32_t mSec);
extern void alsSetGain(ams_deviceCtx_t * ctx, uint32_t mGain);
extern uint32_t getAlsAdcMaxCount(uint32_t atime_usec);
#endif
#if defined CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
extern uint32_t proxGain_conversion[];
extern uint8_t proxGainToReg(uint32_t x);
extern uint8_t proxTimeToReg (uint32_t x);
extern uint32_t proxRegToTime (uint8_t x);
#endif

extern deviceRegisterTable_t deviceRegisterDefinition[DEVREG_REG_MAX];
extern ams_deviceIdentifier_e ams_validateDevice(AMS_PORT_portHndl * portHndl);
extern bool ams_getDeviceInfo(ams_deviceInfo_t * info);
extern bool ams_getMode(ams_deviceCtx_t * ctx, ams_mode_t * mode);
extern bool ams_deviceInit(ams_deviceCtx_t * ctx, AMS_PORT_portHndl * portHndl, ams_calibrationData_t * calibrationData);
extern bool ams_deviceEventHandler(ams_deviceCtx_t * ctx);
extern uint32_t ams_getResult(ams_deviceCtx_t * ctx);
extern bool ams_deviceSetConfig(ams_deviceCtx_t * ctx, ams_configureFeature_t feature, deviceConfigOptions_t option, uint32_t data);
extern bool ams_deviceGetConfig(ams_deviceCtx_t * ctx);
extern bool ams_deviceCalibrateLux(ams_deviceCtx_t * ctx, ams_calibrationData_t * calibrationData);
extern bool ams_deviceCalibrateRgb(ams_deviceCtx_t * ctx, ams_calibrationData_t * calibrationData);
extern bool ams_deviceSelfTest(ams_deviceCtx_t * ctx, AMS_PORT_portHndl * portHndl);
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_CCB
extern bool ams_deviceGetAls(ams_deviceCtx_t * ctx, ams_apiAls_t * exportData);
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
extern bool ams_deviceGetPrx(ams_deviceCtx_t * ctx, ams_apiPrx_t * exportData);
extern bool ams_deviceCalibrateProx(ams_deviceCtx_t * ctx, ams_calibrationData_t * calibrationData);
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_GESTURE
extern bool ams_deviceGetGesture(ams_deviceCtx_t * ctx, amsGestResult_t * exportData);
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_IRBEAM_CCB
extern bool ams_deviceSetIrBeamConfig(ams_deviceCtx_t * ctx, irBeam_config_e item, uint32_t byteCount, const uint8_t *data);
#endif

#ifdef __cplusplus
}
#endif

#else

#endif  /* __AMS_DEVICE_CONTROL_BLOCK_H__ */



#include "sns_tmx4903_sensor_instance.h"


/*
 * Device Algorithm ALS
 */

#ifndef AMS_ALS_CALLIBRATION_H
#define AMS_ALS_CALLIBRATION_H

#define CPU_FRIENDLY_FACTOR_1024        1
#define AMS_ALS_GAIN_FACTOR             1000

#define AMS_ALS_Cc                      (118 * CPU_FRIENDLY_FACTOR_1024)
#define AMS_ALS_Rc                      (112 * CPU_FRIENDLY_FACTOR_1024)
#define AMS_ALS_Gc                      (172 * CPU_FRIENDLY_FACTOR_1024)
#define AMS_ALS_Bc                      (180 * CPU_FRIENDLY_FACTOR_1024)
#define AMS_ALS_R_COEF                  (195)
#define AMS_ALS_G_COEF                  (1000)
#define AMS_ALS_B_COEF                  (-293)
#define AMS_ALS_D_FACTOR                (436)
#define AMS_ALS_CT_COEF                 (4417)
#define AMS_ALS_CT_OFFSET               (1053)
#define AMS_ALS_TARGET_R                (0)
#define AMS_ALS_TARGET_G                (0)
#define AMS_ALS_TARGET_B                (0)

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_WIDEBAND
#define AMS_WIDEBAND_SCALE_FACTOR       1000
#define AMS_ALS_Wbc                     (111 * CPU_FRIENDLY_FACTOR_1024) /* actual value is TBD */
#define AMS_ALS_WB_C_FACTOR             (0.79 * AMS_WIDEBAND_SCALE_FACTOR)
#define AMS_ALS_WB_R_FACTOR             (0.54 * AMS_WIDEBAND_SCALE_FACTOR)
#define AMS_ALS_WB_B_FACTOR             (0.39 * AMS_WIDEBAND_SCALE_FACTOR)
#endif

#define AMS_ALS_TIMEBASE                (AMS_USEC_PER_TICK) /* in uSec, see data sheet */

#define AMS_ALS_THRESHOLD_LOW           (5) /* in % */
#define AMS_ALS_THRESHOLD_HIGH          (5) /* in % */
#define AMS_ALS_THRESHOLD_HALF_WIDTH    (10) /* in % */

#define AMS_ALS_DEFAULT_GAIN            (64 * AMS_ALS_GAIN_FACTOR)
#define AMS_ATIME_USEC_FOR_10HZ         (94520) /* allows 1 tick of tolerance */

#ifdef NOT_IMPLEMENTED
#define AMS_ALS_ADAPTIVE_PREF_ATIME_BEFORE_AGAIN   1
#endif

#endif /* AMS_ALS_CALLIBRATION_H */


/*
 * Device Communication Interface
 */

#ifndef __AMS_DEVICE_COMM_INTERFACE_H__
#define __AMS_DEVICE_COMM_INTERFACE_H__

#ifdef  __cplusplus
extern "C" {
#endif

extern uint8_t ams_getByte(AMS_PORT_portHndl * portHndl, ams_deviceRegister_t reg, uint8_t * readData);
extern uint8_t ams_getWord(AMS_PORT_portHndl * portHndl, ams_deviceRegister_t reg, uint16_t * readData);
extern uint8_t ams_getBuf(AMS_PORT_portHndl * portHndl, ams_deviceRegister_t reg, uint8_t * readData, uint8_t length);
extern uint8_t ams_getField(AMS_PORT_portHndl * portHndl, ams_deviceRegister_t reg, uint8_t * data, ams_regMask_t mask);
extern uint8_t ams_setByte(AMS_PORT_portHndl * portHndl, ams_deviceRegister_t reg, uint8_t data);
extern uint8_t ams_setWord(AMS_PORT_portHndl * portHndl, ams_deviceRegister_t reg, uint16_t data);
extern uint8_t ams_setBuf(AMS_PORT_portHndl * portHndl, ams_deviceRegister_t reg, uint8_t * data, uint8_t length);
extern uint8_t ams_setField(AMS_PORT_portHndl * portHndl, ams_deviceRegister_t reg, uint8_t data, ams_regMask_t mask);

#ifdef  __cplusplus
}
#endif

#endif  /* __AMS_DEVICE_COMM_INTERFACE_H__ */


#ifndef AMSLIBRARY_H
#define AMSLIBRARY_H

void * amsLibrary_open(AMS_PORT_portHndl* port_handle_ptr, ams_calibrationData_t *calibration_data);
void * amsLibrary_close();

#endif /* AMSLIBRARY_H */



/* For turning a compiler option value into a string */
#define STRINGIFY(s) STRINGIFY2(s)
#define STRINGIFY2(s) #s
#define AMS_REGISTRY_ENTRY_NAME(s) STRINGIFY(CONFIG_AMS_PART_NAME)s

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
#define ALS_SUID \
  {  \
    .sensor_uid =  \
      {  \
          0xa1, 0x44, 0xa8, 0xbe, 0x57, 0xee, 0x64, 0x2b,  \
          0x24, 0xcd, 0xf4, 0x66, 0x8e, 0xfe, 0x91, 0x4f  \
      }  \
  }
#define ALS_STREAMING_SUID \
  {  \
    .sensor_uid =  \
      {  \
          0xbc, 0x46, 0xd9, 0xae, 0x41, 0x95, 0x11, 0xe7,  \
          0xa5, 0x73, 0x00, 0x05, 0x9a, 0x3c, 0x7a, 0x00  \
      }  \
  }
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
#define PROX_SUID \
  {  \
    .sensor_uid =  \
      {  \
          0xa0, 0x41, 0x2f, 0x95, 0x72, 0xef, 0x64, 0x44,  \
          0x21, 0xa4, 0xf0, 0x32, 0xa4, 0x4e, 0x53, 0x7b  \
      }  \
  }
#define PROX_STREAMING_SUID \
  {  \
    .sensor_uid =  \
      {  \
          0xbd, 0x58, 0x56, 0x48, 0x3a, 0x91, 0x11, 0xe7,  \
          0xa3, 0x60, 0x00, 0x05, 0x9a, 0x3c, 0x7a, 0x00  \
      }  \
  }
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
#define RGB_SUID \
  {  \
    .sensor_uid =  \
      {  \
          0xb7, 0xb0, 0x20, 0xfc, 0x1f, 0xc2, 0x11, 0xe7,  \
          0xba, 0xd9, 0x00, 0x05, 0x9a, 0x3c, 0x7a, 0x00  \
      }  \
  }
#define RGB_STREAMING_SUID \
  {  \
    .sensor_uid =  \
      {  \
          0xbe, 0x57, 0x77, 0xb2, 0x41, 0x95, 0x11, 0xe7,  \
          0x83, 0xfa, 0x00, 0x05, 0x9a, 0x3c, 0x7a, 0x00  \
      }  \
  }
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_490x
#endif //CONFIG_AMS_OPTICAL_SENSOR_490x

#undef AMS_USE_DEFAULTS
#if AMS_USE_DEFAULTS
/** TODO Using 8996 Platform config as defaults. This is for
 *  test purpose only. All platform specific information will
 *  be available to the Sensor driver via Registry. */
#ifdef SSC_TARGET_HEXAGON_CORE_QDSP6_2_0
#ifdef USE_RUMI_SE
#define I2C_BUS_INSTANCE               0x06
#else
#define I2C_BUS_INSTANCE               0x01
#endif //USE_RUMI_SE
#else
#define I2C_BUS_INSTANCE               0x03
#endif //SSC_TARGET_HEXAGON_CORE_QDSP6_2_0
#define RAIL_1                         "/pmic/client/sensor_vddio"
#define RAIL_2                         "/pmic/client/sensor_vdd"
#define I2C_BUS_FREQ                   400
#define I2C_SLAVE_ADDRESS              0x39
#define IRQ_NUM                        120
#define NUM_OF_RAILS                   2
#define I2C_BUS_MIN_FREQ_KHZ           400
#define I2C_BUS_MAX_FREQ_KHZ           400
#endif  // AMS_USE_DEFAULTS

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
/** Forward Declaration of ALS Sensor API */
sns_sensor_api ams_als_sensor_api;
sns_sensor_api ams_als_streaming_sensor_api;
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
/** Forward Declaration of RGB Sensor API */
sns_sensor_api ams_rgb_sensor_api;
sns_sensor_api ams_rgb_streaming_sensor_api;
#endif

/** Forward Declaration of Prox Sensor API */
sns_sensor_api ams_prox_sensor_api;
sns_sensor_api ams_prox_streaming_sensor_api;
/**
 * AMS customary device ODR (Hz) definitions
 * Note 1 Hz entry is polled mode only - many devices can't interrupt that slow
 */
/* OTOH, the test sensor is hardcoded to request 1 Hz so let's support it
 * even in DRI mode.
 */
#define AMS_DEVICE_ODR_1                 1.0
#define AMS_DEVICE_ODR_2                 2.0
#define AMS_DEVICE_ODR_5                 5.0
#define AMS_DEVICE_ODR_10                10.0
#define AMS_DEVICE_ODR_15                15.0
#define AMS_DEVICE_ODR_20                20.0

/* Length of float[] array in std. report event */
#define ALS_PROTO_STD_ARRAY_LEN     2
#define RGB_PROTO_STD_ARRAY_LEN     6

#define ALS_FAC_CAL_NUM  3
#define PROX_FAC_CAL_NUM 2
#define RGB_FAC_CAL_NUM  8
#define RGB_TARGET_VAL_NUM 3

/** Supported opertating modes */
#define AMS_OPMODE_LPM       "LPM"
#define AMS_OPMODE_NORMAL    "NORMAL"

/** Power rail timeout States for the AMS Sensors.*/
typedef enum
{
  AMS_POWER_RAIL_PENDING_NONE,
  AMS_POWER_RAIL_PENDING_INIT,
  AMS_POWER_RAIL_PENDING_SET_CLIENT_REQ,
} ams_power_rail_pending_state;


typedef struct pb_arg_calibration_common {
  const char* name;
  sns_sensor *sensor;
} pb_arg_calibration_common;

typedef struct pb_custom_float_parse_arg {
  float *data_array;
  uint32_t *version_array;
  uint8_t *index;
  uint8_t size;
} pb_custom_float_parse_arg;

/** Interrupt Sensor State. */
typedef struct ams_state
{
    void                    *priv;
    sns_data_stream         *reg_data_stream;
    sns_data_stream         *fw_stream;
    sns_data_stream         *timer_stream;
    sns_sensor_uid          reg_suid;
    sns_sensor_uid          irq_suid;
    sns_sensor_uid          timer_suid;
    ams_sensor_type         sensor_type;
    sns_sensor_uid          my_suid;
    ams_com_port_info       com_port_info;
    sns_interrupt_req       irq_config;

    sns_pwr_rail_service    *pwr_rail_service;
    sns_rail_config         rail_config;
    sns_power_rail_state    registry_rail_on_state;

    bool                    hw_is_present;
    bool                    sensor_client_present;
    bool                    use_dri;

    ams_power_rail_pending_state    power_rail_pend_state;

    // debug
    ams_deviceIdentifier_e  who_am_i;
    sns_diag_service        *diag_service;
    sns_sync_com_port_service *scp_service;
    size_t                  encoded_event_len;

    AMS_PORT_portHndl       port_handle;
    AMS_PORT_portHndl       *port_handle_ptr;

    // sensor configuration
    bool is_dri;
    int64_t hardware_id;
    bool supports_sync_stream;
    uint8_t resolution_idx;
    float odr_for_selftest;

    // registry sensor config
    bool registry_cfg_received;
    sns_registry_phy_sensor_cfg registry_cfg;

    // registry sensor platform config
    bool registry_pf_cfg_received;
    sns_registry_phy_sensor_pf_cfg registry_pf_cfg;

    // placement
    bool                    registry_placement_received;
    float                   placement[12];

    // registry calibration data
    ams_calibrationData_t   calibration_data;
    uint32_t                cal_version[CALIBRATION_DATA_INDEX_MAX];
} ams_state;

/** Functions shared by all AMS Sensors */
/**
 * This function parses the client_request list per Sensor and
 * determines final config for the Sensor Instance.
 *
 * @param[i] this          Sensor reference
 * @param[i] instance      Sensor Instance to config
 * @param[i] sensor_type   Sensor type
 *
 * @return none
 */
void ams_reval_instance_config(sns_sensor *this,
                              sns_sensor_instance *instance,
                              ams_sensor_type sensor_type);

/**
 * Sends a request to the SUID Sensor to get SUID of a dependent
 * Sensor.
 *
 * @param[i] this          Sensor reference
 * @param[i] data_type     data_type of dependent Sensor
 * @param[i] data_type_len Length of the data_type string
 */
void ams_send_suid_req(sns_sensor *this, char *const data_type,
                      uint32_t data_type_len);

/**
 * Processes events from SUID Sensor.
 *
 * @param[i] this   Sensor reference
 *
 * @return none
 */
void ams_process_suid_events(sns_sensor *const this);

/**
 * notify_event() Sensor API common between all AMS Sensors.
 *
 * @param this    Sensor reference
 *
 * @return sns_rc
 */
sns_rc ams_sensor_notify_event(sns_sensor *const this);
sns_rc ams_streaming_sensor_notify_event(sns_sensor *const this);

/**
 * set_client_request() Sensor API common between all AMS
 * Sensors.
 *
 * @param this            Sensor reference
 * @param exist_request   existing request
 * @param new_request     new request
 * @param remove          true to remove request
 *
 * @return sns_sensor_instance*
 */
sns_sensor_instance* ams_set_client_request(sns_sensor *const this,
                                           struct sns_request const *exist_request,
                                           struct sns_request const *new_request,
                                           bool remove);

void ams_start_power_rail_timer(sns_sensor *const this,
                                sns_time timeout_ticks,
                                ams_power_rail_pending_state pwr_rail_pend_state);
/**
 * Sets instance config to run a self test.
 *
 * @param[i] this      Sensor reference
 * @param[i] instance  Sensor Instance reference
 *
 * @return none
 */
void ams_set_self_test_inst_config(sns_sensor *this,
                              sns_sensor_instance *instance);

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS
sns_rc ams_als_init(sns_sensor *const this);
sns_rc ams_als_streaming_init(sns_sensor *const this);
sns_rc ams_als_deinit(sns_sensor *const this);
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
sns_rc ams_prox_init(sns_sensor *const this);
sns_rc ams_prox_streaming_init(sns_sensor *const this);
sns_rc ams_prox_deinit(sns_sensor *const this);
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
sns_rc ams_rgb_init(sns_sensor *const this);
sns_rc ams_rgb_streaming_init(sns_sensor *const this);
sns_rc ams_rgb_deinit(sns_sensor *const this);
#endif
#endif //__SNS_TMX4903_SENSOR___H__

