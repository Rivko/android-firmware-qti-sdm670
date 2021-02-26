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


#include "sns_tmx4903_sensor.h"
#include "sns_tmx4903_hal.h"

static ams_deviceCtx_t * amsLibrary = NULL; /* QC - should not use global variables */
static ams_deviceCtx_t my_context; /* QC - should not use global variables */

/*
 * Device Communication Interface
 */
uint8_t AMS_PORT_setByte(AMS_PORT_portHndl * portHndl, uint8_t reg_addr, uint8_t * data, uint8_t length) {
    uint32_t write_count;
    sns_rc status;
    sns_port_vector port_vec;
    port_vec.buffer = data;
    port_vec.bytes = length;
    port_vec.is_write = true;
    port_vec.reg_addr = reg_addr;

    if (portHndl == NULL)
        return 0;

    status = portHndl->scp_service->api->sns_scp_register_rw(portHndl->scp_handle,
                                                            &port_vec,
                                                            1,
                                                            false,
                                                            &write_count);

    if (status == SNS_RC_SUCCESS) {
        return (uint8_t)write_count;
    }
    return 0;
}

uint8_t AMS_PORT_getByte(AMS_PORT_portHndl * portHndl, uint8_t reg_addr, uint8_t * data, uint8_t length){
    uint32_t read_count;
    sns_rc status;
    sns_port_vector port_vec;
    port_vec.buffer = data;
    port_vec.bytes = length;
    port_vec.is_write = false;
    port_vec.reg_addr = reg_addr;

    if (portHndl == NULL)
        return 0;

    status = portHndl->scp_service->api->sns_scp_register_rw(portHndl->scp_handle,
                                                            &port_vec,
                                                            1,
                                                            false,
                                                            &read_count);

    if (status == SNS_RC_SUCCESS) {
        return (uint8_t)read_count;
    }
    return 0;
}

uint8_t ams_getByte(AMS_PORT_portHndl * portHndl, ams_deviceRegister_t reg, uint8_t * readData)
{
    uint8_t read_count = 0;
    uint8_t length = 1;

    /* Sanity check input param */
    if(reg > DEVREG_REG_MAX)
    {
        return 0;
    }

    read_count = AMS_PORT_getByte(portHndl,
                                deviceRegisterDefinition[reg].address,
                                readData,
                                length);

    return read_count;
}

uint8_t ams_setByte(AMS_PORT_portHndl * portHndl, ams_deviceRegister_t reg, uint8_t data)
{
    uint8_t write_count = 0;
    uint8_t length = 1;

    /* Sanity check input param */
    if(reg > DEVREG_REG_MAX)
    {
        return 0;
    }

    write_count = AMS_PORT_setByte(portHndl,
                                deviceRegisterDefinition[reg].address,
                                &data,
                                length);

    return write_count;
}

uint8_t ams_getBuf(AMS_PORT_portHndl * portHndl, ams_deviceRegister_t reg, uint8_t * readData, uint8_t length)
{
    uint8_t read_count = 0;

    /* Sanity check input param */
    if(reg > DEVREG_REG_MAX)
    {
        return 0;
    }

    read_count = AMS_PORT_getByte(portHndl,
                                deviceRegisterDefinition[reg].address,
                                readData,
                                length);

    return read_count;
}

uint8_t ams_setBuf(AMS_PORT_portHndl * portHndl, ams_deviceRegister_t reg, uint8_t * data, uint8_t length)
{
    uint8_t write_count = 0;

    /* Sanity check input param */
    if(reg > DEVREG_REG_MAX)
    {
        return 0;
    }

    write_count = AMS_PORT_setByte(portHndl,
                                deviceRegisterDefinition[reg].address,
                                data,
                                length);

    return write_count;
}

uint8_t ams_getWord(AMS_PORT_portHndl * portHndl, ams_deviceRegister_t reg, uint16_t * readData)
{
    uint8_t read_count = 0;
    uint8_t length = sizeof(uint16_t);
    uint8_t buffer[sizeof(uint16_t)];

    /* Sanity check input param */
    if(reg > DEVREG_REG_MAX)
    {
        return 0;
    }

    read_count = AMS_PORT_getByte(portHndl,
                                deviceRegisterDefinition[reg].address,
                                buffer,
                                length);

    *readData = ((buffer[0] << AMS_ENDIAN_1) + (buffer[1] << AMS_ENDIAN_2));

    return read_count;
}

uint8_t ams_setWord(AMS_PORT_portHndl * portHndl, ams_deviceRegister_t reg, uint16_t data)
{
    uint8_t write_count = 0;
    uint8_t length = sizeof(uint16_t);
    uint8_t buffer[sizeof(uint16_t)];

    /* Sanity check input param */
    if(reg > DEVREG_REG_MAX)
    {
        return 0;
    }

    buffer[0] = ((data >> AMS_ENDIAN_1) & 0xff);
    buffer[1] = ((data >> AMS_ENDIAN_2) & 0xff);

    write_count = AMS_PORT_setByte(portHndl,
                                deviceRegisterDefinition[reg].address,
                                &buffer[0],
                                length);
    return write_count;
}

uint8_t ams_getField(AMS_PORT_portHndl * portHndl, ams_deviceRegister_t reg, uint8_t * data, ams_regMask_t mask)
{
    uint8_t read_count = 0;
    uint8_t length = 1;

    /* Sanity check input param */
    if(reg > DEVREG_REG_MAX)
    {
        return 0;
    }

    read_count = AMS_PORT_getByte(portHndl,
                                deviceRegisterDefinition[reg].address,
                                data,
                                length);

    *data &= mask;

    return read_count;
}

uint8_t ams_setField(AMS_PORT_portHndl * portHndl, ams_deviceRegister_t reg, uint8_t data, ams_regMask_t mask)
{
    uint8_t write_count = 1;
    uint8_t length = 1;
    uint8_t original_data;
    uint8_t new_data;

    AMS_PORT_getByte(portHndl,
                        deviceRegisterDefinition[reg].address,
                        &original_data,
                        length);

    new_data = original_data & ~mask;
    new_data |= (data & mask);

    if (new_data != original_data){
        write_count = AMS_PORT_setByte(portHndl,
                        deviceRegisterDefinition[reg].address,
                        &new_data,
                        length);
    }

    return write_count;
}

/*
 * Device Control Block 490x
 */

#ifdef CONFIG_AMS_OPTICAL_SENSOR_GESTURE
#ifdef __TESTBENCH__
gestureAPIData_t rawDataDebugBuffer[5000];
uint32_t debugBufferIndex = 0;
#endif
#endif

#define HIGH    0xFF
#define LOW     0x00

deviceRegisterTable_t deviceRegisterDefinition[DEVREG_REG_MAX] = {
    { 0x80, 0x00 },
    { 0x81, 0xFF },
    { 0x82, 0x00 },
    { 0x83, 0xFF },
    { 0x84, 0x00 },
    { 0x85, 0x00 },
    { 0x86, 0x00 },
    { 0x87, 0x00 },
    { 0x88, 0x00 },
    { 0x89, 0x00 },
    { 0x8A, 0x00 },
    { 0x8B, 0x00 },
    { 0x8C, 0x00 },
    { 0x8D, 0xA0 },
    { 0x8E, 0x4f },
    { 0x8F, 0x00 },
    { 0x90, 0x00 },
    { 0x91, 0x00 },
    { 0x92, 0x00 },
    { 0x93, 0x00 },
    { 0x94, 0x00 },
    { 0x95, 0x00 },
    { 0x96, 0x00 },
    { 0x97, 0x00 },
    { 0x98, 0x00 },
    { 0x99, 0x00 },
    { 0x9A, 0x00 },
    { 0x9B, 0x00 },
    { 0x9C, 0x00 },
    { 0x9D, 0x00 },
    { 0x9E, 0x00 },
    { 0x9F, 0x04 },

    { 0xA0, 0x00 },
    { 0xA1, 0x00 },
    { 0xA2, 0x00 },
    { 0xA3, 0x00 },
    { 0xA4, 0x00 },
    { 0xA5, 0x00 },
    { 0xA6, 0x00 },
    { 0xA7, 0x00 },
    { 0xA8, 0x00 },
    { 0xA9, 0x00 },
    { 0xAB, 0x00 },
    { 0xAC, 0x07 },
    { 0xAD, 0x08 },
    { 0xAF, 0x16 },

#if defined CONFIG_AMS_OPTICAL_SENSOR_GESTURE
    { 0xB0, 0x00 },
    { 0xB1, 0x8F },
    { 0xB2, 0x80 },
    { 0xB3, 0x00 },
    { 0xB4, 0x0A },
    { 0xB5, 0x00 },
    { 0xB6, 0x40 },
    { 0xB7, 0x00 },
    { 0xB8, 0x00 },
    { 0xB9, 0x00 },
    { 0xBA, 0x00 },
    { 0xBB, 0x00 },
#endif
    { 0xBC, 0x00 },
    { 0xBD, 0x00 },

    { 0xC0, 0x00 },
    { 0xC1, 0x00 },
    { 0xC2, 0x00 },
    { 0xC3, 0x00 },
    { 0xC4, 0x00 },
    { 0xC5, 0x00 },
    { 0xC6, 0x00 },
    { 0xC7, 0x00 },

    { 0xD0, 0x00 },
    { 0xD1, 0x00 },
    { 0xD2, 0x00 },
    { 0xD3, 0x00 },
    { 0xD6, 0xFF },
    { 0xD7, 0x00 },
    { 0xD8, 0x44 },
    { 0xD9, 0x0C },
    { 0xDA, 0x20 },
    { 0xDB, 0x10 },
    { 0xDC, 0x0F },
    { 0xDD, 0x00 },
    { 0xDE, 0x00 }
#if defined CONFIG_AMS_OPTICAL_SENSOR_GESTURE
  , { 0xF8, 0x00 },
    { 0xF9, 0x00 },
    { 0xFA, 0x00 },
    { 0xFB, 0x00 },
    { 0xFC, 0x00 },
    { 0xFD, 0x00 },
    { 0xFE, 0x00 },
    { 0xFF, 0x00 },
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_IRBEAM_CCB
    { 0x00, 0x00 }, /* DEVREG_RAM_START */
#endif
};

/* Set ENABLE reg to ctx->shadowEnableReg unless INT enables have to be
 * be deferred, in which case omit the IEN bits and set the rest
*/
void ams_setEnable(ams_deviceCtx_t * ctx)
{
    uint8_t allowedBits;

    if (ctx->allowIens)
    {
        allowedBits = ctx->shadowEnableReg;
    }
    else
    {
        allowedBits = ctx->shadowEnableReg & ~(AMS_ALL_IENS);
    }

    ams_setByte(ctx->portHndl, DEVREG_ENABLE, allowedBits);
}


#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_CCB
uint32_t alsGain_conversion[] = { 1 * 1000, 4 * 1000, 16 * 1000, 64 * 1000};
uint8_t alsGainToReg(uint32_t x);
uint8_t alsGainToReg(uint32_t x){
    int i;

    for (i = sizeof(alsGain_conversion)/sizeof(uint32_t)-1; i != 0; i--) {
        if (x >= alsGain_conversion[i]) break;
    }
    return (i << 0);
}
/* Derive register code for desired ATIME in usec.
 * The 4903/4/5 chips have an 8bit upcounter. Minimum ATIME is 1 tick,
 * reg code = 0xff; max is 256 ticks, reg code = 0.
 */
uint8_t alsTimeUsToReg(uint32_t atime_usec){
    uint8_t ticks;

    if (atime_usec < AMS_USEC_PER_TICK)
      return 0xff; /* min time */

    /* Actual number of ATIME quanta will be (ticks + 1); if atime_usec is an
     * exact multiple, we want the next lower multiple so it will produce that
     * exact ATIME.  Biasing atime_usec down by 1 produces that result.  If the
     * request is a single usec (or more) longer, it will produce the higher
     * number. Thus this always produces at least the number of usec requested.
     */
    atime_usec -= 1; /* this is safe; if atime_usec==0, it returned above */
    if (atime_usec >= (255 * AMS_USEC_PER_TICK))
      return 0; /* max time (256 actual ticks) */

    ticks = (atime_usec / AMS_USEC_PER_TICK);
    return (0xff - ticks);
}
void alsSetTime(ams_deviceCtx_t * ctx, uint32_t uSec){
    ams_setByte(ctx->portHndl, DEVREG_ATIME, alsTimeUsToReg(uSec));
}
void alsSetGain(ams_deviceCtx_t * ctx, uint32_t mGain){
    ams_setField(ctx->portHndl, DEVREG_CFG1, alsGainToReg(mGain), MASK_AGAIN);
}
/* This returns the max ADC count as a fn. of the requested ATIME, along the
 * way calculating the actual number of ATIME ticks.  The 4903/4/5 ALS ADCs
 * can count up to 1024 counts per ATIME quantum, cumulatively, up to a hard
 * max of 65535.
 */
uint32_t getAlsAdcMaxCount(uint32_t atime_usec){
    uint32_t counts;
    uint32_t ticks = 256 - (uint32_t)alsTimeUsToReg(atime_usec);
    counts = ticks * AMS_ALS_MAX_COUNTS_PER_TICK;
    if (counts > AMS_ALS_MAX_COUNTS)
        return AMS_ALS_MAX_COUNTS;
    return counts;
}
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
uint32_t proxGain_conversion[] = { 1000, 2000, 4000, 8000};
uint8_t proxGainToReg(uint32_t x);
uint8_t proxGainToReg(uint32_t x){
    int i;

    for (i = sizeof(proxGain_conversion)/sizeof(uint32_t)-1; i != 0; i--) {
        if (x >= proxGain_conversion[i]) break;
    }
    return (i << 6);
}
uint8_t proxTimeToReg (uint32_t x);
uint8_t proxTimeToReg (uint32_t x){
    uint8_t regValue;
    regValue = (x / AMS_PRX_TIMEBASE) - 1;
    return regValue;
}
uint32_t proxRegToTime (uint8_t x);
uint32_t proxRegToTime (uint8_t x){
    uint32_t time;
    time = (x + 1) * AMS_PRX_TIMEBASE;
    return time;
}
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
bool _490x_proxInit(ams_deviceCtx_t * ctx, ams_calibrationData_t * calibrationData);

static bool _490x_proxSetThreshold(ams_deviceCtx_t * ctx, int32_t threshold){
    ams_ccb_proximity_config_t configData;
    configData.sensitivity = (threshold & 0xff);
    ccb_proximitySetConfig(&ctx->ccbProxCtx, &configData);
        return false;
}

static void _490x_handleProxEvent(ams_deviceCtx_t * ctx){
    ams_ccb_proximity_dataSet_t proxData;
    uint16_t pData;

    proxData.statusReg = ctx->shadowStatus1Reg;
    ams_getWord(ctx->portHndl, DEVREG_PDATA_LOW, &pData);
#if 1 //DBG
    {
        int16_t pofs[4];
        ams_getBuf(ctx->portHndl, DEVREG_OFFSET_NL, (uint8_t *)pofs, 8);
        AMS_PORT_log_Msg_5(AMS_DEBUG
            , "_490x_handleProxEvent: pData=%u, pofs[N,S,W,E]=(%d, %d, %d, %d)"
            , pData
            , pofs[0], pofs[1], pofs[2], pofs[3]
            );
    }
#endif //DBG

    proxData.pData = pData;
    ccb_proximityHandle(ctx, &proxData);
}

bool ams_deviceGetPrx(ams_deviceCtx_t * ctx, ams_apiPrx_t * exportData){
    ams_ccb_proximity_result_t outData;

    ccb_proximityGetResult(ctx, &outData);
    exportData->nearBy = outData.nearBy;
    exportData->proximity = outData.proximity;

    return false;
}
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_CCB
static bool _490x_alsSetThreshold(ams_deviceCtx_t * ctx, int32_t threshold){
    ams_ccb_als_config_t configData;

    configData.threshold = threshold;
        ccb_alsSetConfig(ctx, &configData);
        return false;
}

bool _490x_alsInit(ams_deviceCtx_t * ctx, ams_calibrationData_t * calibrationData);

bool ams_deviceGetAls(ams_deviceCtx_t * ctx, ams_apiAls_t * exportData){
    ams_ccb_als_result_t result;
    ccb_alsGetResult(ctx, &result);
    exportData->mLux        = result.mLux;
    exportData->colorTemp   = result.colorTemp;

    exportData->clear       = result.clear;
    exportData->red         = result.red;
    exportData->green       = result.green;
    exportData->blue        = result.blue;
    exportData->ir          = result.ir;
    exportData->wideband    = result.wideband;
    exportData->rawClear    = result.rawClear;
    exportData->rawRed      = result.rawRed;
    exportData->rawGreen    = result.rawGreen;
    exportData->rawBlue     = result.rawBlue;
    exportData->quality     = result.quality;
    return false;
}

static void _490x_handleAlsEvent(ams_deviceCtx_t * ctx){
    ams_ccb_als_dataSet_t ccbAlsData;
    ccbAlsData.statusReg = ctx->shadowStatus1Reg;
    ccb_alsHandle(ctx, &ccbAlsData);
}

static void _490x_handleAdaptiveAtime(ams_deviceCtx_t * ctx){
    ams_ccb_als_result_t result;

    ccb_alsGetResult(ctx, &result);

    if ((result.rawGreen <= ctx->adaptiveAtimeLowThreshold) &&
        (ctx->atimeState == AMS_ADAPTIVE_ATIME_STATE_SHORT)) {
        /* Changing to long mode */
        AMS_PORT_log_Msg_2(AMS_DEBUG, "rawG = %d, adaptiveAtimeLowThreshold %d,", result.rawGreen, ctx->adaptiveAtimeLowThreshold );
        ctx->atimeState = AMS_ADAPTIVE_ATIME_STATE_LONG;

        /* Increase ATIME */
        AMS_DISABLE_ALS();
        alsSetTime(ctx, AMS_ADAPTIVE_ATIME_LONG);
        AMS_REENABLE_ALS();

        /* Inform ccb and algorithm of new atime setting */
        ctx->ccbAlsCtx.initData.configData.uSecTime = AMS_ADAPTIVE_ATIME_LONG;
        ccb_alsInit(ctx, &ctx->ccbAlsCtx.initData);

    } else if ((result.rawGreen >= ctx->adaptiveAtimeHighThreshold) &&
        (ctx->atimeState == AMS_ADAPTIVE_ATIME_STATE_LONG)) {
        /* Changing to short mode */
        AMS_PORT_log_Msg_2(AMS_DEBUG, "rawG = %d, adaptiveAtimeHighThreshold %d,", result.rawGreen, ctx->adaptiveAtimeHighThreshold );
        ctx->atimeState = AMS_ADAPTIVE_ATIME_STATE_SHORT;

        /* Decrease ATIME */
        AMS_DISABLE_ALS();
        alsSetTime(ctx, AMS_ADAPTIVE_ATIME_SHORT);
        AMS_REENABLE_ALS();

        /* Inform ccb and algorithm of new atime setting */
        ctx->ccbAlsCtx.initData.configData.uSecTime = AMS_ADAPTIVE_ATIME_SHORT;
        ccb_alsInit(ctx, &ctx->ccbAlsCtx.initData);
    }
}
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_GESTURE
#ifndef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
/* #error "In order for gesture to work, prox is also needed" */
#endif
bool _490x_gestureRegUpdate(ams_deviceCtx_t * ctx);
bool _490x_gestureRegUpdate(ams_deviceCtx_t * ctx){

    ams_setByte(ctx->portHndl,  DEVREG_GESTURE_4, 113 ); /* GTIME */
    ams_setField(ctx->portHndl, DEVREG_GESTURE_1, 15, 0x3f); /* pulse */

    ams_setWord(ctx->portHndl, DEVREG_GESTURE_6, AMS_GESTURE_CALIBRATION_OFFSET_TARGET); /* Threshold */
    ams_setWord(ctx->portHndl, DEVREG_GESTURE_8, AMS_GESTURE_CALIBRATION_OFFSET_TARGET); /* Threshold */

    ams_setField(ctx->portHndl, DEVREG_GESTURE_0, GST_EXMSK_ALL, MASK_GST_EXMSK);
    ams_setField(ctx->portHndl, DEVREG_GESTURE_0, 0x03,  MASK_GST_EXPERS);
    ams_setField(ctx->portHndl, DEVREG_GESTURE_1, 0x00,  0xC0);

    ams_setField(ctx->portHndl, DEVREG_GESTURE_2, LOW, 0x30);

    ams_setByte(ctx->portHndl,  DEVREG_GESTURE_5, 0x02); /* GIEN */

    return false;
}

static void _490x_handleGestureEvent(ams_deviceCtx_t * ctx)
{
    uint32_t ret;
    uint8_t numofdset = 1;
    amsGestDataSet_t inputData;
    amsGestResult_t outputData;
    gestureAPIData_t rawData;
    uint8_t gestureCompleted;
    bool exitLoop = false;

    inputData.prox = ctx->ccbProxCtx.pData;
    inputData.timeStamp = ctx->timeStamp;
    inputData.datasetArray = &rawData;

    do {
        /* get dataSet */
        ams_getBuf(ctx->portHndl, DEVREG_GFIFO_N_L,
                (uint8_t *)&rawData.north, 8);

#ifdef __TESTBENCH__
        rawDataDebugBuffer[debugBufferIndex].north = rawData.north;
        rawDataDebugBuffer[debugBufferIndex].south = rawData.south;
        rawDataDebugBuffer[debugBufferIndex].east = rawData.east;
        rawDataDebugBuffer[debugBufferIndex].west = rawData.west;
        debugBufferIndex++;
#endif
        /* process dataSet */
        ret = amsAlg_gesture_processData(&ctx->gestureCtx, &inputData);
        if (ret != 0){
            exitLoop = true;
            switch (ret)
            {
                case circularLeft:
                    AMS_PORT_log_Msg(AMS_DEBUG, ">>>>>>>>>>>>>>>>  Gesture Event: circularLeft");
                    ctx->updateAvailable |= (1 << AMS_GESTURE_SENSOR);
                    break;
                case circularRight:
                    AMS_PORT_log_Msg(AMS_DEBUG, ">>>>>>>>>>>>>>>>  Gesture Event: circularRight");
                    ctx->updateAvailable |= (1 << AMS_GESTURE_SENSOR);
                    break;
                case pushedAndHold:
                    AMS_PORT_log_Msg(AMS_DEBUG, ">>>>>>>>>>>>>>>>  Gesture Event: pushedAndHold");
                    ctx->updateAvailable |= (1 << AMS_GESTURE_SENSOR);
                    break;
                default:
                    exitLoop = false;
                    break;
            }
        }

        /* poll to see how many dataSets we have left */
        ams_getByte(ctx->portHndl, DEVREG_GESTURE_A, &numofdset);
    } while ((numofdset > 0) && (exitLoop == false));

    ams_getByte(ctx->portHndl, DEVREG_GESTURE_B, &gestureCompleted);
    gestureCompleted &= 0x01;

    if(gestureCompleted == 0){
        amsAlg_gesture_getResult(&ctx->gestureCtx, &outputData);
        switch (outputData.event){
        case northToSouth:
            AMS_PORT_log_Msg(AMS_DEBUG, ">>>>>>>>>>>>>>>>  Gesture Event: northToSouth");
            ctx->updateAvailable |= (1 << AMS_GESTURE_SENSOR);
            break;
        case southToNorth:
            AMS_PORT_log_Msg(AMS_DEBUG, ">>>>>>>>>>>>>>>>  Gesture Event: southToNorth");
            ctx->updateAvailable |= (1 << AMS_GESTURE_SENSOR);
            break;
        case eastToWest:
            AMS_PORT_log_Msg(AMS_DEBUG, ">>>>>>>>>>>>>>>>  Gesture Event: eastToWest");
            ctx->updateAvailable |= (1 << AMS_GESTURE_SENSOR);
            break;
        case westToEast:
            AMS_PORT_log_Msg(AMS_DEBUG, ">>>>>>>>>>>>>>>>  Gesture Event: westToEast");
            ctx->updateAvailable |= (1 << AMS_GESTURE_SENSOR);
            break;
        case northWestToSouthEast:
            AMS_PORT_log_Msg(AMS_DEBUG, ">>>>>>>>>>>>>>>>  Gesture Event: northWestToSouthEast");
            ctx->updateAvailable |= (1 << AMS_GESTURE_SENSOR);
            break;
        case northEastToSouthWest:
            AMS_PORT_log_Msg(AMS_DEBUG, ">>>>>>>>>>>>>>>>  Gesture Event: northEastToSouthWest");
            ctx->updateAvailable |= (1 << AMS_GESTURE_SENSOR);
            break;
        case southWestToNorthEast:
            AMS_PORT_log_Msg(AMS_DEBUG, ">>>>>>>>>>>>>>>>  Gesture Event: southWestToNorthEast");
            ctx->updateAvailable |= (1 << AMS_GESTURE_SENSOR);
            break;
        case southEastToNorthWest:
            AMS_PORT_log_Msg(AMS_DEBUG, ">>>>>>>>>>>>>>>>  Gesture Event: southEastToNorthWest");
            ctx->updateAvailable |= (1 << AMS_GESTURE_SENSOR);
            break;
        case pushedAndReleased:
            AMS_PORT_log_Msg(AMS_DEBUG, ">>>>>>>>>>>>>>>>  Gesture Event: pushedAndReleased");
            ctx->updateAvailable |= (1 << AMS_GESTURE_SENSOR);
            break;
        case northTap:
            AMS_PORT_log_Msg(AMS_DEBUG, ">>>>>>>>>>>>>>>>  Gesture Event: northTap");
            ctx->updateAvailable |= (1 << AMS_GESTURE_SENSOR);
            break;
        case southTap:
            AMS_PORT_log_Msg(AMS_DEBUG, ">>>>>>>>>>>>>>>>  Gesture Event: southTap");
            ctx->updateAvailable |= (1 << AMS_GESTURE_SENSOR);
            break;
        case eastTap:
            AMS_PORT_log_Msg(AMS_DEBUG, ">>>>>>>>>>>>>>>>  Gesture Event: eastTap");
            ctx->updateAvailable |= (1 << AMS_GESTURE_SENSOR);
            break;
        case westTap:
            AMS_PORT_log_Msg(AMS_DEBUG, ">>>>>>>>>>>>>>>>  Gesture Event: westTap");
            ctx->updateAvailable |= (1 << AMS_GESTURE_SENSOR);
            break;
        case unknown_gestureEvent:
        default:
            break;
        }
#ifdef __TESTBENCH__
        {
            uint32_t counter = 0;
            do{
                printf("%5d, %5d, %5d, %5d\r\n",
                        rawDataDebugBuffer[counter].north,
                        rawDataDebugBuffer[counter].south,
                        rawDataDebugBuffer[counter].east,
                        rawDataDebugBuffer[counter].west);
            }while(++counter != debugBufferIndex);
            printf("    N,     S,     E,     W\r\n");
            debugBufferIndex = 0;
        }
#endif
    }
}
bool ams_deviceGetGesture(ams_deviceCtx_t * ctx, amsGestResult_t * outData){
    amsAlg_gesture_getResult(&ctx->gestureCtx, outData);
    return false;
}
#endif

bool ams_deviceSetConfig(ams_deviceCtx_t * ctx, ams_configureFeature_t feature, deviceConfigOptions_t option, uint32_t data){
    int ret = 0;

    if (feature == AMS_CONFIG_GLOBAL){
        AMS_PORT_log_Msg_1(AMS_DEBUG, "ams_configureFeature_t  AMS_CONFIG_GLOBAL(%d)", data);
        switch (option)
        {
            case AMS_CONFIG_ALLOW_INTS:
                if (data == 0) {
                    ctx->allowIens = false;
                } else {
                    ctx->allowIens = true;
                }
                break;

            default:
                AMS_PORT_log_Msg_1(AMS_DEBUG, "ams_deviceSetConfig _GLOBAL: ERROR, unk. option %d", option);
                return true;
        }
    }

#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
    if (feature == AMS_CONFIG_PROX){
        AMS_PORT_log_Msg(AMS_DEBUG, "ams_configureFeature_t  AMS_CONFIG_PROX");
        switch (option)
        {
            case AMS_CONFIG_ENABLE: /* power on */
                AMS_PORT_log_Msg_1(AMS_DEBUG, "deviceConfigOptions_t   AMS_CONFIG_ENABLE(%u)", data);
                AMS_PORT_log_Msg_1(AMS_INFO,  "current mode            %d", ctx->mode);
                if (data == 0) {
                    if (ctx->mode == MODE_PROX) {
                        /* if no other active features, turn off device */
                        ctx->shadowEnableReg = 0;
                        ctx->mode = MODE_OFF;
                        ccb_proximityInit(ctx, &ctx->ccbProxCtx.initData);
                    } else {
                        ctx->mode &= ~MODE_PROX;
                        if (ctx->mode & MODE_GESTURE){
                            /* gesture is still enabled. So, only disable Prox Interrupt*/
                            ctx->shadowEnableReg &= (~(PIEN));
                        } else {
                            ctx->shadowEnableReg &= (~(PEN | PIEN));
                        }
                    }
                } else {
                    ccb_proximityInit(ctx, &ctx->ccbProxCtx.initData); // this will reinitialize the
                    // proximity in a way that will force interrupt
                    // before we were missing this interrupt because when initialliy other
                    // feature was enabled the interrupt was generated but it was missed
                    // since the interrupt was not configured because it wasn't needed
                    ctx->shadowEnableReg |= (PEN | PIEN | PON);
                    ctx->mode |= MODE_PROX;
                }
                break;
            case AMS_CONFIG_THRESHOLD: /* set threshold */
                AMS_PORT_log_Msg(AMS_INFO,  "deviceConfigOptions_t   AMS_CONFIG_THRESHOLD");
                ret |= _490x_proxSetThreshold(ctx, data);
                break;
            case AMS_CONFIG_ALWAYS_READ:
                AMS_PORT_log_Msg_1(AMS_INFO,  "deviceConfigOptions_t   AMS_CONFIG_ALWAYS_READ(%d)", data);
                ctx->alwaysReadProx = !!data;
                break;
            default:
                break;
        }
    }
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_CCB
    if (feature == AMS_CONFIG_ALS_LUX){
        AMS_PORT_log_Msg(AMS_INFO, "ams_configureFeature_t  AMS_CONFIG_ALS_LUX");
        switch (option)
        {
            case AMS_CONFIG_ENABLE: /* ON / OFF */
                AMS_PORT_log_Msg_1(AMS_DEBUG, "deviceConfigOptions_t   AMS_CONFIG_ENABLE(%u)", data);
                AMS_PORT_log_Msg_1(AMS_INFO,  "current mode            %d", ctx->mode);
                if (data == 0) {
                    if (ctx->mode == MODE_ALS_LUX) {
                        /* if no other active features, turn off device */
                        ctx->shadowEnableReg = 0;
                        ctx->mode = MODE_OFF;
                    } else {
                        if ((ctx->mode & MODE_ALS_ALL) == MODE_ALS_LUX) {
                            ctx->shadowEnableReg &= (~ALS_ENABLE_ALL);
                        }
                        ctx->mode &= ~(MODE_ALS_LUX);
                    }
                } else {
                    if ((ctx->mode & MODE_ALS_ALL) == 0) {
                        ccb_alsInit(ctx, &ctx->ccbAlsCtx.initData);
                        ctx->shadowEnableReg |= (ALS_ENABLE_ALL | PON);
                    } else {
                        /* force interrupt */
                        ams_setWord(ctx->portHndl, DEVREG_AIHTHL, 0x00);
                    }
                    ctx->mode |= MODE_ALS_LUX;
                }
                break;
            case AMS_CONFIG_THRESHOLD: /* set threshold */
                AMS_PORT_log_Msg(AMS_INFO,   "deviceConfigOptions_t   AMS_CONFIG_THRESHOLD");
                AMS_PORT_log_Msg_1(AMS_INFO, "data                    %d", data);
                ret |= _490x_alsSetThreshold(ctx, data);
                break;
            case AMS_CONFIG_ADAPTIVE_ATIME: /* Enable/Disable adaptive atime */
                AMS_PORT_log_Msg(AMS_INFO,   "deviceConfigOptions_t   AMS_CONFIG_ADAPTIVE_ATIME");
                AMS_PORT_log_Msg_1(AMS_INFO, "data                    %d", data);
                if (data == 1) {
                    ctx->adaptiveAtime = true;
                    ctx->ccbAlsCtx.initData.autoGain = false;
                    ccb_alsInit(ctx, &ctx->ccbAlsCtx.initData);
                } else if (data == 0) {
                    ctx->adaptiveAtime = false;
                    ctx->ccbAlsCtx.initData.autoGain = true;
                    ccb_alsInit(ctx, &ctx->ccbAlsCtx.initData);
                }
                break;
            case AMS_CONFIG_ALWAYS_READ:
                AMS_PORT_log_Msg_1(AMS_INFO, "deviceConfigOptions_t   AMS_CONFIG_ALWAYS_READ(%d)", data);
                ctx->alwaysReadAls = !!data;
                break;

            default:
                break;
        }
    }
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
    if (feature == AMS_CONFIG_ALS_RGB){
        AMS_PORT_log_Msg(AMS_DEBUG, "ams_configureFeature_t  AMS_CONFIG_ALS_RGB");
        switch (option)
        {
            case AMS_CONFIG_ENABLE: /* ON / OFF */
                AMS_PORT_log_Msg_1(AMS_DEBUG, "deviceConfigOptions_t   AMS_CONFIG_ENABLE(%u)", data);
                AMS_PORT_log_Msg_1(AMS_INFO, "current mode            %d", ctx->mode);
                if (data == 0) {
                    if (ctx->mode == MODE_ALS_RGB) {
                        /* if no other active features, turn off device */
                        ctx->shadowEnableReg = 0;
                        ctx->mode = MODE_OFF;
                    } else {
                        if ((ctx->mode & MODE_ALS_ALL) == MODE_ALS_RGB) {
                            ctx->shadowEnableReg &= (~ALS_ENABLE_ALL);
                        }
                        ctx->mode &= ~(MODE_ALS_RGB);
                    }
                } else {
                    if ((ctx->mode & MODE_ALS_ALL) == 0) {
                        ccb_alsInit(ctx, &ctx->ccbAlsCtx.initData);
                        ctx->shadowEnableReg |= (ALS_ENABLE_ALL | PON);
                    } else {
                        /* force interrupt */
                        ams_setWord(ctx->portHndl, DEVREG_AIHTHL, 0x00);
                    }
                    ctx->mode |= MODE_ALS_RGB;
                }
                break;
            case AMS_CONFIG_THRESHOLD: /* set threshold */
                AMS_PORT_log_Msg(AMS_INFO,    "deviceConfigOptions_t   AMS_CONFIG_THRESHOLD");
                AMS_PORT_log_Msg_1(AMS_DEBUG, "data                    %d", data);
                ret |= _490x_alsSetThreshold(ctx, data);
                break;
            case AMS_CONFIG_ALWAYS_READ:
                AMS_PORT_log_Msg_1(AMS_INFO,  "deviceConfigOptions_t   AMS_CONFIG_ALWAYS_READ(%d)", data);
                ctx->alwaysReadAls = !!data;
                break;
            default:
                break;
        }
    }
    if (feature == AMS_CONFIG_ALS_IR){
        AMS_PORT_log_Msg(AMS_DEBUG, "ams_configureFeature_t  AMS_CONFIG_ALS_IR");
        switch (option)
        {
            case AMS_CONFIG_ENABLE: /* ON / OFF */
                AMS_PORT_log_Msg_1(AMS_DEBUG, "deviceConfigOptions_t   AMS_CONFIG_ENABLE(%u)", data);
                AMS_PORT_log_Msg_1(AMS_INFO,  "current mode            %d", ctx->mode);
                if (data == 0) {
                    if (ctx->mode == MODE_ALS_IR) {
                        /* if no other active features, turn off device */
                        ctx->shadowEnableReg = 0;
                        ctx->mode = MODE_OFF;
                    } else {
                        if ((ctx->mode & MODE_ALS_ALL) == MODE_ALS_IR) {
                            ctx->shadowEnableReg &= (~ALS_ENABLE_ALL);
                        }
                        ctx->mode &= ~(MODE_ALS_IR);
                    }
                } else {
                    if ((ctx->mode & MODE_ALS_ALL) == 0) {
                        ccb_alsInit(ctx, &ctx->ccbAlsCtx.initData);
                        ctx->shadowEnableReg |= (ALS_ENABLE_ALL | PON);
                    } else {
                        /* force interrupt */
                        ams_setWord(ctx->portHndl, DEVREG_AIHTHL, 0x00);
                    }
                    ctx->mode |= MODE_ALS_IR;
                }
                break;
            case AMS_CONFIG_THRESHOLD: /* set threshold */
                break;
            case AMS_CONFIG_ALWAYS_READ:
                AMS_PORT_log_Msg_1(AMS_INFO,  "deviceConfigOptions_t   AMS_CONFIG_ALWAYS_READ(%d)", data);
                ctx->alwaysReadAls = !!data;
                break;
            default:
                break;
        }
    }
    if (feature == AMS_CONFIG_ALS_CT){
        AMS_PORT_log_Msg(AMS_DEBUG, "ams_configureFeature_t  AMS_CONFIG_ALS_CT");
        switch (option)
        {
            case AMS_CONFIG_ENABLE: /* ON / OFF */
                AMS_PORT_log_Msg_1(AMS_DEBUG, "deviceConfigOptions_t   AMS_CONFIG_ENABLE(%u)", data);
                AMS_PORT_log_Msg_1(AMS_INFO,  "current mode            %d", ctx->mode);
                if (data == 0) {
                    if (ctx->mode == MODE_ALS_CT) {
                        /* if no other active features, turn off device */
                        ctx->shadowEnableReg = 0;
                        ctx->mode = MODE_OFF;
                    } else {
                        if ((ctx->mode & MODE_ALS_ALL) == MODE_ALS_CT) {
                            ctx->shadowEnableReg &= (~ALS_ENABLE_ALL);
                        }
                        ctx->mode &= ~(MODE_ALS_CT);
                    }
                } else {
                    if ((ctx->mode & MODE_ALS_ALL) == 0) {
                        ccb_alsInit(ctx, &ctx->ccbAlsCtx.initData);
                        ctx->shadowEnableReg |= (ALS_ENABLE_ALL | PON);
                    } else {
                        /* force interrupt */
                        ams_setWord(ctx->portHndl, DEVREG_AIHTHL, 0x00);
                    }
                    ctx->mode |= MODE_ALS_CT;
                }
                break;
            case AMS_CONFIG_THRESHOLD: /* set threshold */
                break;
            case AMS_CONFIG_ALWAYS_READ:
                AMS_PORT_log_Msg_1(AMS_INFO,  "deviceConfigOptions_t   AMS_CONFIG_ALWAYS_READ(%d)", data);
                ctx->alwaysReadAls = !!data;
                break;
            default:
                break;
        }
    }
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_WIDEBAND
    if (feature == AMS_CONFIG_ALS_WIDEBAND){
        AMS_PORT_log_Msg(AMS_DEBUG, "ams_configureFeature_t  AMS_CONFIG_ALS_WIDEBAND");
        switch (option)
        {
            case AMS_CONFIG_ENABLE: /* ON / OFF */
                AMS_PORT_log_Msg_1(AMS_DEBUG, "deviceConfigOptions_t   AMS_CONFIG_ENABLE(%u)", data);
                AMS_PORT_log_Msg_1(AMS_INFO,  "current mode            %d", ctx->mode);
                if (data == 0) {
                    if (ctx->mode == MODE_ALS_WIDEBAND) {
                        /* if no other active features, turn off device */
                        ctx->shadowEnableReg = 0;
                        ctx->mode = MODE_OFF;
                    } else {
                        if ((ctx->mode & MODE_ALS_ALL) == MODE_ALS_WIDEBAND) {
                            ctx->shadowEnableReg &= (~ALS_ENABLE_ALL);
                        }
                        ctx->mode &= ~(MODE_ALS_WIDEBAND);
                    }
                } else {
                    if ((ctx->mode & MODE_ALS_ALL) == 0) {
                        ccb_alsInit(ctx, &ctx->ccbAlsCtx.initData);
                        ctx->shadowEnableReg |= (ALS_ENABLE_ALL | PON);
                    }
                    ctx->mode |= MODE_ALS_WIDEBAND;
                }
                break;
            case AMS_CONFIG_THRESHOLD: /* set threshold */
                AMS_PORT_log_Msg(AMS_INFO,   "deviceConfigOptions_t   AMS_CONFIG_THRESHOLD");
                AMS_PORT_log_Msg_1(AMS_INFO, "data                    %d", data);
                ret |= _490x_alsSetThreshold(ctx, data);
                break;
            case AMS_CONFIG_ALWAYS_READ:
                AMS_PORT_log_Msg_1(AMS_INFO,  "deviceConfigOptions_t   AMS_CONFIG_ALWAYS_READ(%d)", data);
                ctx->alwaysReadAls = !!data;
                break;
            default:
                break;
        }
    }
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_GESTURE
    if (feature == AMS_CONFIG_GESTURE){
        AMS_PORT_log_Msg(AMS_DEBUG, "ams_configureFeature_t  AMS_CONFIG_GESTURE");
        switch (option)
        {
            case AMS_CONFIG_ENABLE: /* ON / OFF */
                AMS_PORT_log_Msg_1(AMS_DEBUG, "deviceConfigOptions_t   AMS_CONFIG_ENABLE(%u)", data);
                AMS_PORT_log_Msg_1(AMS_INFO,  "current mode            %d", ctx->mode);
                if (data == 0) {
                    ams_setByte(ctx->portHndl, DEVREG_GESTURE_5, 0);
                    if (ctx->mode == MODE_GESTURE) {
                        /* if no other active features, turn off device */
                        ctx->shadowEnableReg = 0;
                        ctx->mode = MODE_OFF;
                    } else {
                        ctx->mode &= ~MODE_GESTURE;
                        if (ctx->mode & MODE_PROX){
                            /* Prox is still active, only deactivate gesture*/
                            ctx->shadowEnableReg &= (~(GEN));
                        } else {
                            ctx->shadowEnableReg &= (~(GEN | PEN | PIEN));
                        }
                    }
                } else {
                    ret |= _490x_gestureRegUpdate(ctx);
                    ctx->shadowEnableReg |= (GEN | PEN | PON);
                    ctx->mode |= MODE_GESTURE;
                }
                break;
            case AMS_CONFIG_THRESHOLD: /* set threshold */
                AMS_PORT_log_Msg(AMS_INFO,  "deviceConfigOptions_t   AMS_CONFIG_THRESHOLD");
                break;
            case AMS_CONFIG_GESTURE_ORIENTATION:
                ctx->gestureCtx.orentation = data;
                break;
            default:
                break;
        }
    }
#endif

    ams_setEnable(ctx);

    return 0;
}

bool ams_deviceEventHandler(ams_deviceCtx_t * ctx){
    int ret = 0;
    bool doOnePoll = ctx->alwaysReadAls || ctx->alwaysReadProx;
    sns_sync_com_port_service   *scp_service = ctx->portHndl->scp_service;
    sns_rc status;
    sns_port_vector port_vecs[2];
    uint32_t read_count = 0;

    /* Setup STATUS register port vector */
    port_vecs[0].buffer = &ctx->shadowStatus1Reg;
    port_vecs[0].bytes = 1;
    port_vecs[0].is_write = false;
    port_vecs[0].reg_addr = deviceRegisterDefinition[DEVREG_STATUS].address;

    /* Setup STATUS2 register port vector */
    port_vecs[1].buffer = &ctx->shadowStatus2Reg;
    port_vecs[1].bytes = 1;
    port_vecs[1].is_write = false;
    port_vecs[1].reg_addr = deviceRegisterDefinition[DEVREG_STATUS2].address;

    /* Perform I2C read with port vector array */
    status = scp_service->api->sns_scp_register_rw(ctx->portHndl->scp_handle,
                                                   port_vecs,
                                                   2,
                                                   false,
                                                   &read_count);
    AMS_PORT_get_timestamp_usec(&ctx->timeStamp);
    AMS_PORT_log_Msg_2(AMS_DEBUG
                       , "ams_deviceEventHandler: enter: STATUS=0x%02x, STATUS2=0x%02x"
                       , ctx->shadowStatus1Reg
                       , ctx->shadowStatus2Reg
                       );

    while ((ctx->shadowStatus1Reg & MASK_RELEVANT_INTS) || doOnePoll)
    {
        doOnePoll = false;
#ifdef CONFIG_AMS_OPTICAL_SENSOR_GESTURE /* QC - is it possible to replace ifdefs with boolean enable flag in state structure? */
        if (ctx->shadowStatus1Reg & MASK_GINT){
            if (ctx->mode & MODE_GESTURE){
                _490x_handleGestureEvent(ctx);
            }
            ams_setField(ctx->portHndl, DEVREG_INT_CLEAR, HIGH, (MASK_INTCLR_GINT));
        }
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
        if ((ctx->shadowStatus1Reg & (PGSAT | PINT | CINT)) || ctx->alwaysReadProx){
            if (ctx->mode & MODE_PROX){
                AMS_PORT_log_Msg(AMS_DEBUG, "_490x_handleProxEvent");
                _490x_handleProxEvent(ctx);
            }
            AMS_CLR_PROX_INT();
        }
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_CCB
        if (ctx->shadowStatus2Reg & (MASK_AVALID)){
          if ((ctx->shadowStatus1Reg & (ALS_INT_ALL)) || ctx->alwaysReadAls){
              if (ctx->mode & MODE_ALS_ALL){
                  AMS_PORT_log_Msg(AMS_DEBUG, "_490x_handleAlsEvent");
                  _490x_handleAlsEvent(ctx);
                  if (ctx->adaptiveAtime == true){
                      _490x_handleAdaptiveAtime(ctx);
                  }
              }
              AMS_CLR_ALS_INT();
          }
        } else {
          AMS_PORT_log_Msg(AMS_DEBUG, "ams_deviceEventHandler: not AVALID");
        }
#endif

        read_count = 0;
        /* Perform I2C read with port vector array */
        status = scp_service->api->sns_scp_register_rw(ctx->portHndl->scp_handle,
                                                       port_vecs,
                                                       2,
                                                       false,
                                                       &read_count);
        if (ctx->shadowStatus1Reg || ctx->shadowStatus2Reg){
          AMS_PORT_log_Msg_2(AMS_DEBUG
                             , "ams_deviceEventHandler: loop end: STATUS=0x%02x, STATUS2=0x%02x"
                             , ctx->shadowStatus1Reg
                             , ctx->shadowStatus2Reg
                             );
        }
    }

    ams_setEnable(ctx);

    return ret;
}

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_CCB
bool ams_deviceCalibrateLux(ams_deviceCtx_t * ctx, ams_calibrationData_t * calibrationData){
    ams_ccb_als_info_t infoData;

    if (calibrationData){
        ctx->ccbAlsCtx.initData.calibrationData.calibrationFactor = calibrationData->alsCalibrationFactor;
    } else {
        ccb_alsInfo(&infoData);
        ctx->ccbAlsCtx.initData.calibrationData.calibrationFactor = infoData.defaultCalibrationData.calibrationFactor;
    }
    /* QC - maybe put these magic numbers (and other magic numbers used in driver) in registry */
    ctx->ccbAlsCtx.initData.configData.gain = AMS_ALS_DEFAULT_GAIN;
    ctx->ccbAlsCtx.initData.configData.uSecTime = AMS_ATIME_USEC_FOR_10HZ; /* to guarantee valid data with 10 Hz polling */
    ctx->ccbAlsCtx.initData.calibrate = true;
    ccb_alsInit(ctx, &ctx->ccbAlsCtx.initData);

    ctx->shadowEnableReg |= (ALS_ENABLE_ALL | PON);
    ctx->mode |= MODE_ALS_LUX;
    /* If IRQ is registered, enable ALS and its INT; else wait for registration
     * (the code there will end up calling ams_setEnable itself)
     */
    if (ctx->allowIens)
      ams_setEnable(ctx);

    return true;
}
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
bool ams_deviceCalibrateProx(ams_deviceCtx_t * ctx, ams_calibrationData_t * calibrationData){
    ams_ccb_proximity_info_t infoData;

    if (calibrationData){
        ctx->ccbProxCtx.initData.nearByHardThreshold = calibrationData->proxHardThreshold;
    } else {
        ccb_proximityInfo(&infoData);
        ctx->ccbProxCtx.initData.nearByHardThreshold = infoData.defaultCalibrationData.hardThreshold;
    }
    ctx->ccbProxCtx.initData.calibrate = true;
    ccb_proximityInit(ctx, &ctx->ccbProxCtx.initData);

    ctx->shadowEnableReg |= (PEN | PIEN | PON);
    ctx->mode |= MODE_PROX;
    /* If IRQ is registered, enable prox and its INT; else wait for registration
     * (the code there will end up calling ams_setEnable itself)
     */
    if (ctx->allowIens)
      ams_setEnable(ctx);

    return true;
}
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
bool ams_deviceCalibrateRgb(ams_deviceCtx_t * ctx, ams_calibrationData_t * calibrationData){

    ams_ccb_als_info_t infoData;

    if (calibrationData){
        ctx->ccbAlsCtx.initData.calibrationData.calibrationFactor = calibrationData->alsCalibrationFactor;
    } else {
        ccb_alsInfo(&infoData);
        ctx->ccbAlsCtx.initData.calibrationData.calibrationFactor = infoData.defaultCalibrationData.calibrationFactor;
    }
    /* QC - maybe put these magic numbers (and other magic numbers used in driver) in registry */
    ctx->ccbAlsCtx.initData.configData.gain = AMS_ALS_DEFAULT_GAIN;
    ctx->ccbAlsCtx.initData.configData.uSecTime = AMS_ATIME_USEC_FOR_10HZ; /* to guarantee valid data with 10 Hz polling */

    ctx->ccbAlsCtx.initData.calibrate = true;
    ccb_alsInit(ctx, &ctx->ccbAlsCtx.initData);

    ctx->shadowEnableReg |= (ALS_ENABLE_ALL | PON);
    ctx->mode |= MODE_ALS_LUX;
    /* If IRQ is registered, enable ALS and its INT; else wait for registration
     * (the code there will end up calling ams_setEnable itself)
     */
    if (ctx->allowIens)
      ams_setEnable(ctx);

    return true;
}
#endif


bool ams_deviceSelfTest(ams_deviceCtx_t * ctx, AMS_PORT_portHndl * portHndl){
    bool ret = false;

    if (ams_validateDevice(portHndl) == true){
        ret = ams_deviceInit(ctx, portHndl, NULL);
        if (ams_getResult(ctx) == 0){
            AMS_PORT_log_Msg(AMS_DEBUG, "ams_deviceSelfTest: Set IRQ");
            AMS_PORT_log_Msg(AMS_DEBUG, "ams_deviceSelfTest: checking result");
            if (ams_getResult(ctx) == 0){
                return false;
            } else {
                return true;
            }
        }
    } else return false;

    return ret;
}

bool ams_getMode(ams_deviceCtx_t * ctx, ams_mode_t * mode) {
    *mode = ctx->mode;
    return false;
}

uint32_t ams_getResult(ams_deviceCtx_t * ctx) {
    uint32_t returnValue = ctx->updateAvailable;
    ctx->updateAvailable = 0;
    return returnValue;
}

#ifdef CONFIG_AMS_OPTICAL_SENSOR_IRBEAM_CCB
/* false => error */
bool ams_deviceSetIrBeamConfig(ams_deviceCtx_t * ctx, irBeam_config_e item, uint32_t byteCount, const uint8_t *data) {
    return ccb_irBeamSetConfig(ctx, item, byteCount, data);
}
#endif

/*
 * Device Control Block 490x
 */

#if defined CONFIG_AMS_OPTICAL_SENSOR_ALS
#endif

#define HIGH    0xFF
#define LOW     0x00

typedef struct{
    uint8_t                 deviceId;
    uint8_t                 deviceIdMask;
    uint8_t                 deviceRef;
    uint8_t                 deviceRefMask;
    ams_deviceIdentifier_e  device;
}ams_deviceIdentifier_t;

static ams_deviceIdentifier_t deviceIdentifier[]={
    /*
    DEVID   DEVID   REVID   REVID   Part
    REG     MASK    REG     MASK    Number
    */
    {0xd0,  0xff,   0x20,   0xff,   AMS_TMD4904},
    {0xb8,  0xfc,   0x22,   0xff,   AMS_TMD4903},
    {0xb8,  0xfc,   0x32,   0xff,   AMS_TMD4903},
    {0xb8,  0xfc,   0x02,   0xff,   AMS_TMG4903},
    {0xb4,  0xfc,   0x02,   0xff,   AMS_TMG4903},
    {0, 0, 0, 0, AMS_LAST_DEVICE}
};

static void _490x_resetAllRegisters(AMS_PORT_portHndl * portHndl);
static void _490x_resetAllRegisters(AMS_PORT_portHndl * portHndl){
    ams_deviceRegister_t i, base;
    uint8_t rvals[DEVREG_REG_MAX - DEVREG_STATUS];
    int j;
    int addr, prevAddr, maxAddr;

    /* Initial consecutive regs up to just before _REVID */
    for (j=0, i = DEVREG_ENABLE; i <= DEVREG_CFG1; i++) {
        rvals[j++] = deviceRegisterDefinition[i].resetValue;
    }
    ams_setBuf(portHndl, DEVREG_ENABLE, rvals, j);

    /* after _ID reg, transfer each stretch of consecutive regs as a block */
    j = 0;
    i = base = DEVREG_STATUS;
    maxAddr = deviceRegisterDefinition[DEVREG_REG_MAX - 1].address;
    prevAddr = deviceRegisterDefinition[DEVREG_STATUS].address - 1;
    while ((maxAddr  >=  (addr = deviceRegisterDefinition[i].address))
            && (i < DEVREG_REG_MAX))
    {
        if (addr != (prevAddr + 1)) {
            ams_setBuf(portHndl, base, rvals, j);
            j = 0;
            base = i;
        }
        prevAddr = addr;
        rvals[j++] = deviceRegisterDefinition[i++].resetValue;
    }
    if (j) {
        ams_setBuf(portHndl, base, rvals, j);
    }
}

void * amsLibrary_open(AMS_PORT_portHndl* port_handle_ptr,
    ams_calibrationData_t *calibration_data) {

    ams_deviceIdentifier_e chipId= AMS_UNKNOWN_DEVICE;

    AMS_PORT_log_Msg(AMS_DEBUG, "amsLibrary_open: enter");
    if (amsLibrary == NULL) {
        chipId = ams_validateDevice(port_handle_ptr);
        if (chipId != AMS_UNKNOWN_DEVICE){
            ams_deviceInfo_t info;
            AMS_PORT_log_Msg_1(AMS_DEBUG, "amsLibrary_open: device valid, chipId %d", chipId);
            ams_getDeviceInfo(&info);
            amsLibrary = &my_context;
            AMS_PORT_log_Msg_1(AMS_DEBUG, "amsLibrary_open: ...amsLibrary %p; try ams_deviceInit...", amsLibrary);
            ams_deviceInit(amsLibrary, port_handle_ptr, calibration_data);
            AMS_PORT_log_Msg(AMS_DEBUG, "amsLibrary_open: ...ams_deviceInit returned; end: OK");
        } else {
            AMS_PORT_log_Msg_1(AMS_DEBUG, "amsLibrary_open: device INVALID, chipId %d", chipId);
        }
    }
    return (void*) amsLibrary;
}

void * amsLibrary_close(){
    if (amsLibrary != NULL) {
        amsLibrary = NULL;
    }
    return amsLibrary;
}

#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
bool _490x_proxInit(ams_deviceCtx_t * ctx, ams_calibrationData_t * calibrationData);
bool _490x_proxInit(ams_deviceCtx_t * ctx, ams_calibrationData_t * calibrationData){

    ams_ccb_proximity_init_t proxInitData;

    proxInitData.calibrate = false;
#ifdef CONFIG_AMS_PROX_STATIC_MODE
    proxInitData.dynamicMode = false;
#else
    proxInitData.dynamicMode = true;
#endif
    proxInitData.numberAdcBits = 14;
    proxInitData.ptime_us = AMS_PRX_DEFAULT_CYCLE_TIME_USEC;
    proxInitData.adcThreshold = AMS_PRX_DEFAULT_ADC_THRESHOLD;
    proxInitData.nearByHardThreshold = AMS_PRX_DEFAULT_NEARBY_HARD_THRESHOLD;
    proxInitData.nearByThresholdHysteresis = AMS_PRX_DEFAULT_THRESHOLD_HYSTERESIS;

    if (calibrationData){
        proxInitData.nearByHardThreshold = calibrationData->proxHardThreshold;
        AMS_PORT_log_Msg_1(AMS_DEBUG
            , "_490x_proxInit:  nBHT = calibrationData->proxHardThreshold = %d"
            , proxInitData.nearByHardThreshold
            );
    } else {
        AMS_PORT_log_Msg_1(AMS_DEBUG
            , "_490x_proxInit:  nBHT = AMS_PRX_DEFAULT_NEARBY_HARD_THRESHOLD = %d"
            , proxInitData.nearByHardThreshold
            );
    }

    ccb_proximityInit(ctx, &proxInitData);
    return false;
}
#endif

#if defined ( CONFIG_AMS_OPTICAL_SENSOR_GESTURE )
static bool _490x_gestInit(ams_deviceCtx_t * ctx, ams_calibrationData_t * calibrationData)
{
    amsGestContext_t * ptr_gest_ctx = NULL;
    amsGestureAlgoInfo_t info;
    amsGestInitData_t algInitData = { 0 };

    if ( !ctx )
    {
        return true;
    }

    ptr_gest_ctx = &ctx->gestureCtx;
    if ( calibrationData == NULL )
    {
        amsAlg_gesture_getAlgInfo( &info );
    }
    else
    {
        /* Apply custom calibration data for gesture */
    }

    algInitData.mode = x_y;
    algInitData.factorA = 4;
    algInitData.targetOffset = 80;

    amsAlg_gesture_initAlg( ptr_gest_ctx, &algInitData );

    return false;
}
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_CCB
bool _490x_alsInit(ams_deviceCtx_t * ctx, ams_calibrationData_t * calibrationData);
bool _490x_alsInit(ams_deviceCtx_t * ctx, ams_calibrationData_t * calibrationData){

    if (calibrationData == NULL) {
        ams_ccb_als_info_t infoData;
        ccb_alsInfo(&infoData);
        ctx->ccbAlsCtx.initData.calibrationData.luxTarget = infoData.defaultCalibrationData.luxTarget;
        ctx->ccbAlsCtx.initData.calibrationData.luxTargetError = infoData.defaultCalibrationData.luxTargetError;
        ctx->ccbAlsCtx.initData.calibrationData.calibrationFactor = infoData.defaultCalibrationData.calibrationFactor;
        ctx->ccbAlsCtx.initData.calibrationData.Time_base = infoData.defaultCalibrationData.Time_base;
        ctx->ccbAlsCtx.initData.calibrationData.thresholdLow = infoData.defaultCalibrationData.thresholdLow;
        ctx->ccbAlsCtx.initData.calibrationData.thresholdHigh = infoData.defaultCalibrationData.thresholdHigh;
        ctx->ccbAlsCtx.initData.calibrationData.calibrationFactor = infoData.defaultCalibrationData.calibrationFactor;
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
        ctx->ccbAlsCtx.initData.calibrationData.Cc = infoData.defaultCalibrationData.Cc;
        ctx->ccbAlsCtx.initData.calibrationData.Rc = infoData.defaultCalibrationData.Rc;
        ctx->ccbAlsCtx.initData.calibrationData.Gc = infoData.defaultCalibrationData.Gc;
        ctx->ccbAlsCtx.initData.calibrationData.Bc = infoData.defaultCalibrationData.Bc;
        ctx->ccbAlsCtx.initData.calibrationData.R_coef = infoData.defaultCalibrationData.R_coef;
        ctx->ccbAlsCtx.initData.calibrationData.G_coef = infoData.defaultCalibrationData.G_coef;
        ctx->ccbAlsCtx.initData.calibrationData.B_coef = infoData.defaultCalibrationData.B_coef;
        ctx->ccbAlsCtx.initData.calibrationData.D_factor = infoData.defaultCalibrationData.D_factor;
        ctx->ccbAlsCtx.initData.calibrationData.CT_coef = infoData.defaultCalibrationData.CT_coef;
        ctx->ccbAlsCtx.initData.calibrationData.CT_offset = infoData.defaultCalibrationData.CT_offset;
        ctx->ccbAlsCtx.initData.calibrationData.targetR= infoData.defaultCalibrationData.targetR;
        ctx->ccbAlsCtx.initData.calibrationData.targetG= infoData.defaultCalibrationData.targetG;
        ctx->ccbAlsCtx.initData.calibrationData.targetB= infoData.defaultCalibrationData.targetB;
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_WIDEBAND
        ctx->ccbAlsCtx.initData.calibrationData.Wbc = infoData.defaultCalibrationData.Wbc;
        ctx->ccbAlsCtx.initData.calibrationData.Wideband_C_factor = infoData.defaultCalibrationData.Wideband_C_factor;
        ctx->ccbAlsCtx.initData.calibrationData.Wideband_R_factor = infoData.defaultCalibrationData.Wideband_R_factor;
        ctx->ccbAlsCtx.initData.calibrationData.Wideband_B_factor = infoData.defaultCalibrationData.Wideband_B_factor;
#endif
    } else {
        ctx->ccbAlsCtx.initData.calibrationData.luxTarget = calibrationData->alsCalibrationLuxTarget;
        ctx->ccbAlsCtx.initData.calibrationData.luxTargetError = calibrationData->alsCalibrationLuxTargetError;
        ctx->ccbAlsCtx.initData.calibrationData.calibrationFactor = calibrationData->alsCalibrationFactor;
        ctx->ccbAlsCtx.initData.calibrationData.Time_base = calibrationData->timeBase_us;
        ctx->ccbAlsCtx.initData.calibrationData.thresholdLow = calibrationData->alsThresholdLow;
        ctx->ccbAlsCtx.initData.calibrationData.thresholdHigh = calibrationData->alsThresholdHigh;
        ctx->ccbAlsCtx.initData.calibrationData.calibrationFactor = calibrationData->alsCalibrationFactor;
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
        ctx->ccbAlsCtx.initData.calibrationData.Cc = calibrationData->Cc;
        ctx->ccbAlsCtx.initData.calibrationData.Rc = calibrationData->Rc;
        ctx->ccbAlsCtx.initData.calibrationData.Gc = calibrationData->Gc;
        ctx->ccbAlsCtx.initData.calibrationData.Bc = calibrationData->Bc;
        ctx->ccbAlsCtx.initData.calibrationData.R_coef = calibrationData->alsCoefR;
        ctx->ccbAlsCtx.initData.calibrationData.G_coef = calibrationData->alsCoefG;
        ctx->ccbAlsCtx.initData.calibrationData.B_coef = calibrationData->alsCoefB;
        ctx->ccbAlsCtx.initData.calibrationData.D_factor = calibrationData->alsDfg;
        ctx->ccbAlsCtx.initData.calibrationData.CT_coef = calibrationData->alsCctCoef;
        ctx->ccbAlsCtx.initData.calibrationData.CT_offset = calibrationData->alsCctOffset;
        ctx->ccbAlsCtx.initData.calibrationData.targetR = calibrationData->targetR;
        ctx->ccbAlsCtx.initData.calibrationData.targetG = calibrationData->targetG;
        ctx->ccbAlsCtx.initData.calibrationData.targetB = calibrationData->targetB;
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_WIDEBAND
        ctx->ccbAlsCtx.initData.calibrationData.Wbc = calibrationData->Wbc;
        ctx->ccbAlsCtx.initData.calibrationData.Wideband_C_factor = calibrationData->Wideband_C_factor;
        ctx->ccbAlsCtx.initData.calibrationData.Wideband_R_factor = calibrationData->Wideband_R_factor;
        ctx->ccbAlsCtx.initData.calibrationData.Wideband_B_factor = calibrationData->Wideband_B_factor;
#endif
    }
    ctx->ccbAlsCtx.initData.calibrate = false;
    ctx->ccbAlsCtx.initData.configData.gain = AMS_ALS_DEFAULT_GAIN;
    ctx->ccbAlsCtx.initData.configData.uSecTime = AMS_ATIME_USEC_FOR_10HZ;
    ctx->ccbAlsCtx.initData.calibrationData.adcMaxCount = getAlsAdcMaxCount(ctx->ccbAlsCtx.initData.configData.uSecTime);
    ctx->ccbAlsCtx.initData.configData.threshold = AMS_ALS_THRESHOLD_HALF_WIDTH;

    ctx->atimeState = AMS_ADAPTIVE_ATIME_STATE_SHORT;
    ctx->adaptiveAtime = false;
    ctx->adaptiveAtimeLowThreshold = AMS_ADAPTIVE_ATIME_THRESHOLD_LOW;
    ctx->adaptiveAtimeHighThreshold = AMS_ADAPTIVE_ATIME_THRESHOLD_HIGH;

    ctx->alwaysReadAls = false;
    ctx->alwaysReadProx = false;

    ctx->ccbAlsCtx.initData.autoGain = true;

    ccb_alsInit(ctx, &ctx->ccbAlsCtx.initData);
    return false;
}
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_GESTURE
#ifndef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
#error "In order for gesture to work, prox is also needed"
#endif
extern bool _490x_gestureRegUpdate(ams_deviceCtx_t * ctx);

#endif

bool ams_deviceInit(ams_deviceCtx_t * ctx, AMS_PORT_portHndl * portHndl, ams_calibrationData_t * calibrationData){

    int ret = 0;

    memset((void*)ctx, 0, sizeof(ams_deviceCtx_t));
    ctx->portHndl = portHndl;
    ctx->mode = MODE_OFF;
    ctx->systemCalibrationData = calibrationData;
    ctx->deviceId = ams_validateDevice(ctx->portHndl);
    ctx->shadowEnableReg = deviceRegisterDefinition[DEVREG_ENABLE].resetValue;
    ctx->allowIens = false;
    _490x_resetAllRegisters(ctx->portHndl);
    AMS_PORT_get_timestamp_usec(&ctx->timeStamp);

#if defined ( CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB )
    ret |= _490x_proxInit(ctx, calibrationData);
#endif

#if defined ( CONFIG_AMS_OPTICAL_SENSOR_GESTURE )
    ret |= _490x_gestInit(ctx, calibrationData);
#endif

#if defined ( CONFIG_AMS_OPTICAL_SENSOR_ALS_CCB )
    ret |= _490x_alsInit(ctx, calibrationData);
#endif
    ams_setByte(ctx->portHndl, DEVREG_ENABLE, ctx->shadowEnableReg);
    return ret;
}

ams_deviceIdentifier_e ams_validateDevice(AMS_PORT_portHndl * portHndl){
    uint8_t chipId;
    uint8_t revId;
    uint8_t i = 0;

    ams_getByte(portHndl, DEVREG_ID, &chipId);
    ams_getByte(portHndl, DEVREG_REVID, &revId);

    do{
        if (((chipId & deviceIdentifier[i].deviceIdMask) ==
            (deviceIdentifier[i].deviceId & deviceIdentifier[i].deviceIdMask)) &&
            ((revId & deviceIdentifier[i].deviceRefMask) ==
             (deviceIdentifier[i].deviceRef & deviceIdentifier[i].deviceRefMask))){
                return deviceIdentifier[i].device;
        }
        i++;
    }while (deviceIdentifier[i].device != AMS_LAST_DEVICE);

    AMS_PORT_log_Msg_2(AMS_INFO, "ams_validateDevice: 0x%02x 0x%02x", chipId, revId);
    return AMS_UNKNOWN_DEVICE;
}

bool ams_getDeviceInfo(ams_deviceInfo_t * info){
    memset(info, 0, sizeof(ams_deviceInfo_t));

    info->defaultCalibrationData.timeBase_us = AMS_USEC_PER_TICK;
    info->numberOfSubSensors = 0;
    info->memorySize =  sizeof(ams_deviceCtx_t);
    memcpy(info->defaultCalibrationData.deviceName, "TMX490X", sizeof(info->defaultCalibrationData.deviceName));
    info->deviceModel = info->defaultCalibrationData.deviceName;
    info->deviceName  = "ALS/PRX";
    info->driverVersion = "Alpha";
#ifdef CONFIG_AMS_OPTICAL_SENSOR_PROX_CCB
    {
        info->defaultCalibrationData.proxHardThreshold = AMS_PRX_DEFAULT_NEARBY_HARD_THRESHOLD;
        info->proxSensor.driverName = "PROX";

        info->tableSubSensors[info->numberOfSubSensors]= AMS_PROXIMITY_SENSOR;
        info->numberOfSubSensors++;
        info->proxSensor.adcBits = 8;
        info->proxSensor.maxPolRate = 50;
        info->proxSensor.activeCurrent_uA = AMS_PRX_DEFAULT_AVG_CURRENT_uA;
        info->proxSensor.standbyCurrent_uA = 5;
        info->proxSensor.rangeMax = 5;
        info->proxSensor.rangeMin = 0;
    }
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_CCB
    {
        /* TODO */
        ams_ccb_als_info_t infoData;
        ccb_alsInfo(&infoData);
        info->tableSubSensors[info->numberOfSubSensors]= AMS_AMBIENT_SENSOR;
        info->numberOfSubSensors++;

        info->alsSensor.driverName = infoData.algName;
        info->alsSensor.adcBits = 8;
        info->alsSensor.maxPolRate = 50;
        info->alsSensor.activeCurrent_uA = 100;
        info->alsSensor.standbyCurrent_uA = 5;
        /* QC - is this max range for highest gain setting? */
        info->alsSensor.rangeMax = 187024; /* max lux (gain=1; 97.3ms ATIME; ADCs: R,G,C = 35840, B = 0) */
        info->alsSensor.rangeMin = 0;
        info->alsSensor.rangeMaxRgb = 439; /* max uW/cm^2 (gain=1; 97.3ms ATIME; ADCs: B = 35840) */
        info->alsSensor.rangeMinRgb = 0;

        info->defaultCalibrationData.alsCalibrationFactor = infoData.defaultCalibrationData.calibrationFactor;
        info->defaultCalibrationData.alsCalibrationLuxTarget = infoData.defaultCalibrationData.luxTarget;
        info->defaultCalibrationData.alsCalibrationLuxTargetError = infoData.defaultCalibrationData.luxTargetError;
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_WIDEBAND
        info->tableSubSensors[info->numberOfSubSensors]= AMS_WIDEBAND_ALS_SENSOR;
        info->numberOfSubSensors++;
#endif
    }
#endif
#ifdef CONFIG_AMS_OPTICAL_SENSOR_GESTURE
    {
        amsGestureAlgoInfo_t infoGest;
        amsAlg_gesture_getAlgInfo(&infoGest);

        info->tableSubSensors[info->numberOfSubSensors]= AMS_GESTURE_SENSOR;
        info->numberOfSubSensors++;
        info->proxSensor.adcBits = 16;
        info->proxSensor.maxPolRate = 50;
        info->proxSensor.activeCurrent_uA = AMS_PRX_DEFAULT_AVG_CURRENT_uA;
        info->proxSensor.standbyCurrent_uA = 5;
        info->proxSensor.driverName = infoGest.algName;
        info->proxSensor.rangeMax = 1;
        info->proxSensor.rangeMin = 0;
    }
#endif
    return false;
}

/*
 * Device Algorithm ALS
 */

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS

extern void als_update_statics(amsAlsContext_t * ctx);

int amsAlg_als_setConfig(amsAlsContext_t * ctx, amsAlsConf_t * inputData){
    int ret = 0;

    if (inputData != NULL) {
        ctx->gain = inputData->gain;
        ctx->time_us = inputData->time_us;
    }
    als_update_statics(ctx);
    return ret;
}

/*
 * getConfig: is used to quarry the algorithm's configuration
 */
int amsAlg_als_getConfig(amsAlsContext_t * ctx, amsAlsConf_t * outputData){
    int ret = 0;

    outputData->gain = ctx->gain;
    outputData->time_us = ctx->time_us;

    return ret;
}
#endif

/*
 * Device Algorithm ALS
 */

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS

/*
 * getAlgInfo: Intended to be called by a wrapper, allowing the wrapper to
 * allocate memory for the algorithm.
 * It's not needed to call this API, but it could be useful in certain OS'es
 */
int amsAlg_als_getAlgInfo (amsAlsAlgoInfo_t * info) {
    int ret = 0;

    info->algName = "AMS_ALS";
    info->contextMemSize = sizeof(amsAlsContext_t);
    info->scratchMemSize = 0;

    info->initAlg = &amsAlg_als_initAlg;
    info->processData = &amsAlg_als_processData;
    info->getResult = &amsAlg_als_getResult;
    info->setConfig = &amsAlg_als_setConfig;
    info->getConfig = &amsAlg_als_getConfig;

    return ret;
}
#endif

/*
 * Device Algorithm ALS
 */

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS

extern void als_calc_cpl(amsAlsContext_t * ctx);

/*
 * initAlg: is used to initialize the algorithm.
 */
int amsAlg_als_initAlg (amsAlsContext_t * ctx, amsAlsInitData_t * initData){
    int ret = 0;

    memset(ctx, 0, sizeof(amsAlsContext_t));

    if (initData != NULL) {
        ctx->calibration.Time_base = initData->calibration.Time_base;
        ctx->calibration.adcMaxCount = initData->calibration.adcMaxCount;
        ctx->calibration.thresholdLow = initData->calibration.thresholdLow;
        ctx->calibration.thresholdHigh = initData->calibration.thresholdHigh;
        ctx->calibration.calibrationFactor = initData->calibration.calibrationFactor;
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
#ifdef CONFIG_AMS_ALS_CRGBW
        ctx->calibration.Wbc = initData->calibration.Wbc;
#endif
#if defined(CONFIG_AMS_ALS_CRGBW) || defined(CONFIG_AMS_ALS_CRGIB) || defined(CONFIG_AMS_ALS_CRGB)
        ctx->calibration.C_coef = initData->calibration.C_coef;
#endif
#ifdef CONFIG_AMS_ALS_CRGIB
        ctx->calibration.IRc = initData->calibration.IRc;
#endif
        ctx->calibration.R_coef = initData->calibration.R_coef;
        ctx->calibration.G_coef = initData->calibration.G_coef;
        ctx->calibration.B_coef = initData->calibration.B_coef;
        ctx->calibration.Rc = initData->calibration.Rc;
        ctx->calibration.Gc = initData->calibration.Gc;
        ctx->calibration.Bc = initData->calibration.Bc;
        ctx->calibration.Cc = initData->calibration.Cc;
        ctx->calibration.D_factor = initData->calibration.D_factor;
        ctx->calibration.CT_coef = initData->calibration.CT_coef;
        ctx->calibration.CT_offset = initData->calibration.CT_offset;
        ctx->calibration.targetR = initData->calibration.targetR;
        ctx->calibration.targetG = initData->calibration.targetG;
        ctx->calibration.targetB = initData->calibration.targetB;
#endif
#ifdef CONFIG_AMS_ALS_CRWBI
        ctx->calibration.Wbc = initData->calibration.Wbc;
        ctx->calibration.Wideband_C_factor = initData->calibration.Wideband_C_factor;
        ctx->calibration.Wideband_R_factor = initData->calibration.Wideband_R_factor;
        ctx->calibration.Wideband_B_factor = initData->calibration.Wideband_B_factor;
#endif
#if defined CONFIG_AMS_OPTICAL_SENSOR_ALS_CLEAR || defined CONFIG_AMS_OPTICAL_SENSOR_259x
        ctx->calibration.D_factor  = initData->calibration.D_factor;
        ctx->calibration.L0_factor = initData->calibration.L0_factor;
        ctx->calibration.L1_factor = initData->calibration.L1_factor;
        ctx->calibration.L2_factor = initData->calibration.L2_factor;
        ctx->calibration.L3_factor = initData->calibration.L3_factor;
#endif
    }

    if (initData != NULL) {
        ctx->gain = initData->gain;
        ctx->time_us = initData->time_us;
        ctx->adaptive = initData->adaptive;
    } else {
        AMS_PORT_log_Msg(AMS_ERROR, "error: initData == NULL");
    }

    als_update_statics(ctx);
    return ret;
}
#endif

/*
 * Device Algorithm ALS
 */

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS

int amsAlg_als_processData(amsAlsContext_t * ctx, amsAlsDataSet_t * inputData){
    int ret = 0;

    if (inputData->status & ALS_STATUS_RDY){
        ctx->previousLux = ctx->results.mLux;
        if (ctx->previousGain != ctx->gain){
            als_update_statics(ctx);
        }
#if defined(CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB) || defined(CONFIG_AMS_OPTICAL_SENSOR_ALS_WIDEBAND)
        als_compute_data(ctx, inputData);
#endif
#if defined CONFIG_AMS_OPTICAL_SENSOR_ALS_CLEAR
        als_compute_data_clear(ctx, inputData);
#endif
        als_ave_LUX(ctx);
    }

    if (ctx->adaptive){
        if (inputData->status & ALS_STATUS_OVFL){
            ctx->results.adaptive = ADAPTIVE_ALS_GAIN_DEC_REQUEST;
        } else {
            ctx->results.adaptive = als_adaptive(ctx, inputData);
        }
    } else {
        ctx->results.adaptive = ADAPTIVE_ALS_NO_REQUEST;
    }

    return ret;
}
#endif

/*
 * Device Algorithm ALS
 */

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS

/*
 * getResult: will return the output generated by the algorithm
 */

int amsAlg_als_getResult(amsAlsContext_t * ctx, amsAlsResult_t * outData){
    int ret = 0;

    outData->rawClear  = ctx->results.rawClear;
    outData->rawRed  = ctx->results.rawRed;
    outData->rawGreen  = ctx->results.rawGreen;
    outData->rawBlue  = ctx->results.rawBlue;
    outData->irrBlue  = ctx->results.irrBlue;
    outData->irrClear = ctx->results.irrClear;
    outData->irrGreen = ctx->results.irrGreen;
    outData->irrRed   = ctx->results.irrRed;
    outData->irrWideband = ctx->results.irrWideband;
    outData->mLux_ave  = ctx->results.mLux_ave / AMS_LUX_AVERAGE_COUNT;
    outData->IR  = ctx->results.IR;
    outData->CCT = ctx->results.CCT;
    outData->adaptive = ctx->results.adaptive;
    outData->mLux = ctx->results.mLux;

    return ret;
}
#endif


/*
 * Device Algorithm ALS
 */

/* see Application Note:
 * DN40-Rev 1.0 – Lux and CCT Calculations using ams Color Sensors
 */
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS

#define AMS_ROUND_SHFT_VAL                      4
#define AMS_ROUND_ADD_VAL                       (1 << (AMS_ROUND_SHFT_VAL - 1))
#define AMS_ALS_GAIN_FACTOR             1000
#define CPU_FRIENDLY_FACTOR_1024        1

#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
/* Use derived IR but not C */
static void als_calc_LUX_CCT (amsAlsContext_t * ctx,
                                uint16_t redADC,
                                uint16_t greenADC,
                                uint16_t blueADC) {
    int32_t rp1;
    int32_t gp1;
    int32_t bp1;
    int64_t lux = 0;

    rp1 = redADC   - ctx->results.IR;
    gp1 = greenADC - ctx->results.IR;
    bp1 = blueADC  - ctx->results.IR;

    if (redADC > ctx->results.IR)
    {
        lux += (int32_t)(ctx->calibration.R_coef * rp1);
    }
    if (greenADC > ctx->results.IR)
    {
        lux += (int32_t)(ctx->calibration.G_coef * gp1);
    }
    if (blueADC > ctx->results.IR)
    {
        lux += (int32_t)((int32_t)ctx->calibration.B_coef * bp1);
    }

    /* Print int64 as 2 dwords */
    {
        union _qbytes {int64_t sqword; uint32_t udword[2]; uint8_t ubyte[8];} qwd;
        qwd.sqword = lux;
        AMS_PORT_log_Msg_3(AMS_INFO, "als_calc_LUX_CCT: tempLux=(%d<<32 + %u), cpl=%u", qwd.udword[1], qwd.udword[0], ctx->cpl);
    }

    lux <<= AMS_ROUND_SHFT_VAL;
    if (lux < (LONG_MAX / ctx->calibration.calibrationFactor)) {
        lux = (lux * ctx->calibration.calibrationFactor) / ctx->cpl;
    } else {
        lux = (lux / ctx->cpl ) * ctx->calibration.calibrationFactor;
    }
    lux += AMS_ROUND_ADD_VAL;
    lux >>= AMS_ROUND_SHFT_VAL;

    if (rp1 == 0 ) rp1 = 1;
    ctx->results.CCT = ((ctx->calibration.CT_coef * bp1) / rp1) + ctx->calibration.CT_offset;

    ctx->results.quality &= ~(ALS_QUALITY_LUX_MASK | ALS_QUALITY_CCT_MASK);
    if(lux < 0) {
        ctx->results.quality |= (ALS_QUALITY_LUX_BAD | ALS_QUALITY_CCT_BAD);
        lux = 0;
        ctx->results.CCT = 0;
    } else {
        ctx->results.quality |= (ALS_QUALITY_LUX_GOOD | ALS_QUALITY_CCT_GOOD);
    }

    ctx->results.mLux = (uint32_t)lux;
}
#endif /* CONFIG_AMS_OPTICAL_SENSOR_ALS */

void als_update_statics(amsAlsContext_t * ctx) {
    uint64_t tempCpl;
    uint64_t tempTime_us = ctx->time_us;
    uint64_t tempGain = ctx->gain;

    /* test for the potential of overflowing */
    uint32_t maxOverFlow;
    maxOverFlow = (uint64_t)ULLONG_MAX / ctx->time_us;

    if (maxOverFlow < ctx->gain) {
        tempCpl = ((tempTime_us / 2) * (tempGain / 2)) / (AMS_ALS_GAIN_FACTOR/4) ;

    } else {
        tempCpl = (tempTime_us * tempGain) / AMS_ALS_GAIN_FACTOR;
    }
    if (tempCpl > (uint32_t)ULONG_MAX){
        /* if we get here, we have a problem */
        AMS_PORT_log_Msg_1(AMS_ERROR, "als_update_statics: overflow, setting cpl=%u", (uint32_t)ULONG_MAX);
        tempCpl = (uint32_t)ULONG_MAX;
    }

    ctx->cpl = tempCpl / ctx->calibration.D_factor;

    ctx->saturation = ctx->calibration.adcMaxCount;

    ctx->previousGain = ctx->gain;
    AMS_PORT_log_Msg_4(AMS_DEBUG, "als_update_statics: time=%d, gain=%d, sat. level=%u, cpl=%u", ctx->time_us, ctx->gain, ctx->saturation, ctx->cpl);
}

/* als_compute_data -- different versions depending on the input data available */
#if defined(CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB)
/* Use CRGB & derived IR for lux */
void als_compute_data (amsAlsContext_t * ctx, amsAlsDataSet_t * inputData) {
    uint32_t tempIr;

    if (inputData->datasetArray->clearADC < (uint16_t)USHRT_MAX){
        /* Calculate IR */
        tempIr = inputData->datasetArray->redADC +
                            inputData->datasetArray->greenADC +
                            inputData->datasetArray->blueADC;
        if (tempIr > inputData->datasetArray->clearADC) {
            ctx->results.IR = (tempIr - inputData->datasetArray->clearADC) / 2;
        } else {
            ctx->results.IR = 0;
        }

        /* Compute irradiances in uW/cm^2 */
        ctx->results.rawClear = inputData->datasetArray->clearADC;
        ctx->results.rawRed = inputData->datasetArray->redADC;
        ctx->results.rawGreen = inputData->datasetArray->greenADC;
        ctx->results.rawBlue = inputData->datasetArray->blueADC;
        ctx->results.irrRed = (inputData->datasetArray->redADC * (ctx->calibration.Rc / CPU_FRIENDLY_FACTOR_1024)) / ctx->cpl;
        ctx->results.irrClear = (inputData->datasetArray->clearADC * (ctx->calibration.Cc / CPU_FRIENDLY_FACTOR_1024)) / ctx->cpl;
        ctx->results.irrBlue = (inputData->datasetArray->blueADC * (ctx->calibration.Bc / CPU_FRIENDLY_FACTOR_1024)) / ctx->cpl;
        ctx->results.irrGreen = (inputData->datasetArray->greenADC * (ctx->calibration.Gc / CPU_FRIENDLY_FACTOR_1024)) / ctx->cpl;
        AMS_PORT_log_Msg_1(AMS_DEBUG, "als_compute_data: irrClear = %u\n"
                        , ctx->results.irrClear
                        );
        AMS_PORT_log_Msg_3(AMS_DEBUG, "als_compute_data: irrRed = %u, irrGreen = %u, irrBlue = %u\n"
                        , ctx->results.irrRed
                        , ctx->results.irrGreen
                        , ctx->results.irrBlue
                       );

        als_calc_LUX_CCT(ctx, inputData->datasetArray->redADC,
            inputData->datasetArray->greenADC,
            inputData->datasetArray->blueADC);
    } else {
        /* measurement is saturated */
        AMS_PORT_log_Msg(AMS_DEBUG, "als_compute_data:  saturated");
    }
}
#endif /* CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB */

void als_ave_LUX (amsAlsContext_t * ctx) {

    /* if average queue isn't full (at startup), fill it */
    if(ctx->ave_lux[AMS_LUX_AVERAGE_COUNT - 1] == 0) {
        ctx->results.mLux_ave = 0;
        ctx->ave_lux_index = 0;
        do {
            ctx->ave_lux[ctx->ave_lux_index++] = ctx->results.mLux;
            ctx->results.mLux_ave += ctx->results.mLux;
        } while (ctx->ave_lux_index < AMS_LUX_AVERAGE_COUNT);
        ctx->ave_lux_index = 1;
    } else {
        /* replace the oldest LUX value with the new LUX value */
        ctx->results.mLux_ave -= ctx->ave_lux[ctx->ave_lux_index];
        ctx->ave_lux[ctx->ave_lux_index] = ctx->results.mLux;
        ctx->results.mLux_ave += ctx->ave_lux[ctx->ave_lux_index];
        ctx->ave_lux_index++;
    }

    if (ctx->ave_lux_index == AMS_LUX_AVERAGE_COUNT) {
        ctx->ave_lux_index = 0;
    }
}

amsAlsAdaptive_t als_adaptive(amsAlsContext_t * ctx, amsAlsDataSet_t * inputData){
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
    if (inputData->status & ALS_STATUS_OVFL) {
        ctx->results.adaptive = ADAPTIVE_ALS_TIME_DEC_REQUEST;
    } else {
        if (inputData->datasetArray->clearADC == (uint16_t)USHRT_MAX){
            if (ctx->gain != 0){
                return ADAPTIVE_ALS_GAIN_DEC_REQUEST;
            } else {
                return ADAPTIVE_ALS_TIME_DEC_REQUEST;
            }
        } else {
            if (ctx->gain != 0) {
                if (inputData->datasetArray->clearADC >= ctx->saturation){
                    return ADAPTIVE_ALS_GAIN_DEC_REQUEST;
                }
            } else {
                if (inputData->datasetArray->clearADC >= ctx->saturation){
                    return ADAPTIVE_ALS_TIME_DEC_REQUEST;
                }
            }
            if (inputData->datasetArray->clearADC <= 0xff){
                return ADAPTIVE_ALS_GAIN_INC_REQUEST;
            }
        }
    }
#else
    ctx++; inputData++; /* avoid compiler warning */
#endif
    return ADAPTIVE_ALS_NO_REQUEST;
}

#endif

/*
 * Core Control Block ALS
 */

#define AMS_PORT_LOG_CRGB_W(dataset) \
        AMS_PORT_log_Msg_4(AMS_DEBUG, "ccb_alsHandle: C,R,G,B = %u, %u,%u,%u" \
            , dataset.AdcClear \
            , dataset.AdcRed \
            , dataset.AdcGreen \
            , dataset.AdcBlue \
            )


void ccb_alsInit(void * dcbCtx, ams_ccb_als_init_t* initData){
    ams_deviceCtx_t * ctx = (ams_deviceCtx_t*)dcbCtx;
    ams_ccb_als_ctx_t * ccbCtx = &ctx->ccbAlsCtx;
    amsAlsInitData_t initAlsData;
    amsAlsAlgoInfo_t infoAls;

    if (initData){
        memcpy(&ccbCtx->initData, initData, sizeof(ams_ccb_als_init_t));
    } else {
        ccbCtx->initData.calibrate = false;
    }

    initAlsData.adaptive = false;
    initAlsData.irRejection = false;
    initAlsData.gain = ccbCtx->initData.configData.gain;
    initAlsData.time_us = ccbCtx->initData.configData.uSecTime;
    initAlsData.calibration.adcMaxCount = ccbCtx->initData.calibrationData.adcMaxCount;

    initAlsData.calibration.calibrationFactor = ccbCtx->initData.calibrationData.calibrationFactor;
    initAlsData.calibration.Time_base = ccbCtx->initData.calibrationData.Time_base;
    initAlsData.calibration.thresholdLow = ccbCtx->initData.calibrationData.thresholdLow;
    initAlsData.calibration.thresholdHigh = ccbCtx->initData.calibrationData.thresholdHigh;
    initAlsData.calibration.calibrationFactor = ccbCtx->initData.calibrationData.calibrationFactor;
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
    initAlsData.calibration.Cc = ccbCtx->initData.calibrationData.Cc;
    initAlsData.calibration.Rc = ccbCtx->initData.calibrationData.Rc;
    initAlsData.calibration.Gc = ccbCtx->initData.calibrationData.Gc;
    initAlsData.calibration.Bc = ccbCtx->initData.calibrationData.Bc;
    initAlsData.calibration.targetR = ccbCtx->initData.calibrationData.targetR;
    initAlsData.calibration.targetG = ccbCtx->initData.calibrationData.targetG;
    initAlsData.calibration.targetB = ccbCtx->initData.calibrationData.targetB;
    initAlsData.calibration.R_coef = ccbCtx->initData.calibrationData.R_coef;
    initAlsData.calibration.G_coef = ccbCtx->initData.calibrationData.G_coef;
    initAlsData.calibration.B_coef = ccbCtx->initData.calibrationData.B_coef;
    initAlsData.calibration.D_factor = ccbCtx->initData.calibrationData.D_factor;
    initAlsData.calibration.CT_coef = ccbCtx->initData.calibrationData.CT_coef;
    initAlsData.calibration.CT_offset = ccbCtx->initData.calibrationData.CT_offset;
#endif
    amsAlg_als_getAlgInfo (&infoAls);
    amsAlg_als_initAlg(&ccbCtx->ctxAlgAls, &initAlsData);

    ccbCtx->shadowAiltReg = 0xffff;
    ccbCtx->shadowAihtReg = 0;
    AMS_SET_ALS_THRESHOLDS(ccbCtx->shadowAiltReg, ccbCtx->shadowAihtReg);

    if (ccbCtx->initData.calibrate){
        ccbCtx->state = AMS_CCB_ALS_CALIBRATION_INIT;
    } else {
        ccbCtx->state = AMS_CCB_ALS_INIT;
    }
}

void ccb_alsGetConfig(void * dcbCtx, ams_ccb_als_config_t * configData){
    ams_ccb_als_ctx_t * ccbCtx = &((ams_deviceCtx_t*)dcbCtx)->ccbAlsCtx;

    configData->threshold = ccbCtx->initData.configData.threshold;
}

void ccb_alsSetConfig(void * dcbCtx, ams_ccb_als_config_t * configData){
    ams_ccb_als_ctx_t * ccbCtx = &((ams_deviceCtx_t*)dcbCtx)->ccbAlsCtx;

    ccbCtx->initData.configData.threshold = configData->threshold;
}

bool ccb_alsHandle(void * dcbCtx, ams_ccb_als_dataSet_t * alsData){
    ams_deviceCtx_t * ctx = (ams_deviceCtx_t*)dcbCtx;
    ams_ccb_als_ctx_t * ccbCtx = &((ams_deviceCtx_t*)dcbCtx)->ccbAlsCtx;
    amsAlsDataSet_t inputDataAls;
    static adcDataSet_t adcData; /* QC - is this really needed? */

    AMS_PORT_log_Msg_1(AMS_DEBUG, "ccb_alsHandle: case = %d", ccbCtx->state);

    switch (ccbCtx->state){
    case AMS_CCB_ALS_INIT:
        AMS_DISABLE_ALS();
        AMS_SET_ALS_TIME(ccbCtx->initData.configData.uSecTime);
        AMS_SET_ALS_PERS(0x01);
        AMS_SET_ALS_GAIN(ctx->ccbAlsCtx.initData.configData.gain);
        /* force interrupt */
        ccbCtx->shadowAiltReg = 0xffff;
        ccbCtx->shadowAihtReg = 0;
        AMS_SET_ALS_THRESHOLDS(ccbCtx->shadowAiltReg, ccbCtx->shadowAihtReg);
        ccbCtx->state = AMS_CCB_ALS_RGB;
        AMS_REENABLE_ALS();
        break;
    case AMS_CCB_ALS_RGB: /* state to measure RGB */
#ifdef HAVE_OPTION__ALWAYS_READ
        if ((alsData->statusReg & (AINT)) || ctx->alwaysReadAls){
#else
        if (alsData->statusReg & (AINT)){
#endif
            AMS_ALS_GET_CRGB_W(&adcData);
            inputDataAls.status = ALS_STATUS_RDY;
            inputDataAls.datasetArray = (alsData_t*)&adcData;
            AMS_PORT_LOG_CRGB_W(adcData);
            if (ccbCtx->initData.configData.threshold){
                uint32_t tempThresh;

                tempThresh = (((uint32_t)adcData.AdcClear * (100 - (uint32_t)(ccbCtx->initData.configData.threshold))) + 50) / 100;
                ccbCtx->shadowAiltReg = (uint16_t)tempThresh;
                tempThresh = (((uint32_t)adcData.AdcClear * (100 + (uint32_t)(ccbCtx->initData.configData.threshold))) + 50) / 100;
                ccbCtx->shadowAihtReg = (uint16_t)tempThresh;
                AMS_SET_ALS_THRESHOLDS(ccbCtx->shadowAiltReg, ccbCtx->shadowAihtReg);
            }

            amsAlg_als_processData(&ctx->ccbAlsCtx.ctxAlgAls, &inputDataAls);
            /* override the quality decision if saturation occurred */
            if (alsData->statusReg & ASAT) {
                ctx->ccbAlsCtx.ctxAlgAls.results.quality &= ~(ALS_QUALITY_LUX_MASK | ALS_QUALITY_CCT_MASK);
                ctx->ccbAlsCtx.ctxAlgAls.results.quality |= (ALS_QUALITY_LUX_BAD | ALS_QUALITY_CCT_BAD);
            }
            ctx->updateAvailable |= (1 << AMS_AMBIENT_SENSOR);
            AMS_PORT_log_Msg_1(AMS_DEBUG, "ccb_alsHandle: Clear    = %d", adcData.AdcClear);
            AMS_PORT_log_Msg_1(AMS_DEBUG, "ccb_alsHandle: Red      = %d", adcData.AdcRed);
            AMS_PORT_log_Msg_1(AMS_DEBUG, "ccb_alsHandle: Green    = %d", adcData.AdcGreen);
            AMS_PORT_log_Msg_1(AMS_DEBUG, "ccb_alsHandle: Blue     = %d", adcData.AdcBlue);
            AMS_PORT_log_Msg_1(AMS_DEBUG, "ccb_alsHandle: IR       = %d", ccbCtx->ctxAlgAls.results.IR);
            AMS_PORT_log_Msg_1(AMS_INFO, "ccb_alsHandle: mLux     = %d", ccbCtx->ctxAlgAls.results.mLux);
            AMS_PORT_log_Msg_1(AMS_INFO, "ccb_alsHandle: CT       = %d", ccbCtx->ctxAlgAls.results.CCT);
            AMS_PORT_log_Msg_1(AMS_DEBUG, "ccb_alsHandle: factor   = %d", ccbCtx->ctxAlgAls.calibration.calibrationFactor);
            AMS_PORT_log_Msg_1(AMS_DEBUG, "ccb_alsHandle: timeUs   = %d", ccbCtx->ctxAlgAls.time_us);
            AMS_PORT_log_Msg_1(AMS_DEBUG, "ccb_alsHandle: gain     = %d", ccbCtx->ctxAlgAls.gain);
            AMS_PORT_log_Msg_1(AMS_DEBUG, "ccb_alsHandle: cpl      = %d", ccbCtx->ctxAlgAls.cpl);
            AMS_PORT_log_Msg_1(AMS_DEBUG, "ccb_alsHandle: thshld   = %d", ccbCtx->initData.configData.threshold);
            ccbCtx->state = AMS_CCB_ALS_RGB;

        /* Software AGC */
        if (ccbCtx->initData.autoGain == true)
        {
            uint64_t temp;
            uint32_t recommendedGain;
            uint32_t adcObjective = ctx->ccbAlsCtx.ctxAlgAls.saturation * 128;
            adcObjective /= 160; /* about 80% (128 / 160) */

            if (adcData.AdcClear == 0){
                /* to avoid divide by zero */
                adcData.AdcClear = 1;
            }
            temp = adcObjective * 2048; /* 2048 to avoid floating point operation later on */

            temp /= adcData.AdcClear;
            temp *= ctx->ccbAlsCtx.ctxAlgAls.gain;
            temp /= 2048;


            recommendedGain = temp & 0xffffffff;
            AMS_PORT_log_Msg_4(AMS_DEBUG, "ccb_alsHandle: AMS_CCB_ALS_AUTOGAIN: sat=%u, objctv=%u, cur=%u, rec=%u"
                , ctx->ccbAlsCtx.ctxAlgAls.saturation
                , adcObjective
                , ctx->ccbAlsCtx.ctxAlgAls.gain
                , recommendedGain
                );
            recommendedGain = alsGainToReg(recommendedGain);
            recommendedGain = alsGain_conversion[recommendedGain];
            if (recommendedGain != ctx->ccbAlsCtx.ctxAlgAls.gain){
                AMS_PORT_log_Msg_1(AMS_DEBUG, "ccb_alsHandle: AMS_CCB_ALS_AUTOGAIN: change gain to: %u", recommendedGain);
                ctx->ccbAlsCtx.ctxAlgAls.gain = recommendedGain;
                AMS_DISABLE_ALS();
                AMS_SET_ALS_GAIN(ctx->ccbAlsCtx.ctxAlgAls.gain);
                AMS_REENABLE_ALS();
                if (ctx->ccbAlsCtx.ctxAlgAls.previousGain !=
                    ctx->ccbAlsCtx.ctxAlgAls.gain) {
                    ctx->updateAvailable |= (1 << AMS_ALS_RGB_GAIN_CHANGED);

                    // run process data over copy just to determine the new cpl value
                    // in order to compute the next range but don't mess up the current
                    // measurement which was done over the previous gain.
                    adcDataSet_t tmpAdcData;
                    amsAlsContext_t tmp = ctx->ccbAlsCtx.ctxAlgAls;
                    amsAlsDataSet_t tmpInputDataAls;
                    tmpInputDataAls.status = ALS_STATUS_RDY;

                    tmpInputDataAls.datasetArray = (alsData_t*)&tmpAdcData;
                    // compute the ranges based on the max adc count value
                    tmpAdcData.AdcClear = ctx->ccbAlsCtx.ctxAlgAls.saturation;
                    tmpAdcData.AdcRed   = ctx->ccbAlsCtx.ctxAlgAls.saturation / 3; //divide by more reasonable coefficient
                    tmpAdcData.AdcGreen = ctx->ccbAlsCtx.ctxAlgAls.saturation / 3;
                    tmpAdcData.AdcBlue  = ctx->ccbAlsCtx.ctxAlgAls.saturation / 3;
                    amsAlg_als_processData(&tmp, &tmpInputDataAls);
                    ctx->ccbAlsCtx.ctxAlgAls.results.mMaxLux = tmp.results.mLux;
                }
            }
            else
              AMS_PORT_log_Msg_1(AMS_DEBUG, "ccb_alsHandle: AMS_CCB_ALS_AUTOGAIN: no change, gain=%u", ctx->ccbAlsCtx.ctxAlgAls.gain);

          }
          break;
    }

    case AMS_CCB_ALS_CALIBRATION_INIT:
        if (alsData->statusReg & (AINT)){
            AMS_DISABLE_ALS();
            AMS_SET_ALS_TIME(ccbCtx->initData.configData.uSecTime);
            AMS_SET_ALS_PERS(0x01);
            AMS_SET_ALS_GAIN(ctx->ccbAlsCtx.initData.configData.gain);
            ccbCtx->shadowAiltReg = 0xffff;
            ccbCtx->shadowAihtReg = 0;
            AMS_SET_ALS_THRESHOLDS(ccbCtx->shadowAiltReg, ccbCtx->shadowAihtReg);

            ccbCtx->state = AMS_CCB_ALS_CALIBRATION_COLLECT_DATA;
            ccbCtx->ctxAlgAls.ave_lux[AMS_LUX_AVERAGE_COUNT - 1] = 0;
            AMS_REENABLE_ALS();
        }
        break;
    case AMS_CCB_ALS_CALIBRATION_COLLECT_DATA:
    if (alsData->statusReg & (AINT)){
        uint32_t targetR = ccbCtx->initData.calibrationData.targetR;
        uint32_t targetG = ccbCtx->initData.calibrationData.targetG;
        uint32_t targetB = ccbCtx->initData.calibrationData.targetB;
        float kr = 1, kg = 1, kb = 1;

        AMS_ALS_GET_CRGB_W(&adcData);
        inputDataAls.status = ALS_STATUS_RDY;
        inputDataAls.datasetArray = (alsData_t*)&adcData;
        AMS_PORT_LOG_CRGB_W(adcData);

        AMS_PORT_log_Msg_1(AMS_DEBUG, "adc_red = %d\n", adcData.AdcRed);
        AMS_PORT_log_Msg_1(AMS_DEBUG, "adc_green = %d\n", adcData.AdcGreen);
        AMS_PORT_log_Msg_1(AMS_DEBUG, "adc_blue = %d\n", adcData.AdcBlue);
        AMS_PORT_log_Msg_1(AMS_DEBUG, "targetR = %d\n", targetR);
        AMS_PORT_log_Msg_1(AMS_DEBUG, "targetG = %d\n", targetG);
        AMS_PORT_log_Msg_1(AMS_DEBUG, "targetB = %d\n", targetB);
        AMS_PORT_log_Msg_1(AMS_DEBUG, "caldata_r_coef = %d\n",
            ctx->ccbAlsCtx.initData.calibrationData.R_coef);
        AMS_PORT_log_Msg_1(AMS_DEBUG, "caldata_g_coef = %d\n",
            ctx->ccbAlsCtx.initData.calibrationData.G_coef);
        AMS_PORT_log_Msg_1(AMS_DEBUG, "caldata_b_coef = %d\n",
            ctx->ccbAlsCtx.initData.calibrationData.B_coef);

        /*
         * begin: calibrate rgb
         * 1) get target (raw) r,g,b values
         * 2) calculate r,g,b k-values (ex: kr = targetR / rawR)
         * 3) calculate new r,g,b coefficients (ex: coefR = coefR * kr)
         */

        /* if no target data is provided, use the sensor's raw values */
        if (targetR == 0)
            targetR = adcData.AdcRed;
        if (targetG == 0)
            targetG = adcData.AdcGreen;
        if (targetB == 0)
            targetB = adcData.AdcBlue;

        if (adcData.AdcRed > 0)
            kr = (float)targetR / (float)adcData.AdcRed;

        if (adcData.AdcGreen > 0)
            kg = (float)targetG / (float)adcData.AdcGreen;

        if (adcData.AdcBlue > 0)
            kb = (float)targetB / (float)adcData.AdcBlue;

        /*
         * update all calibration data structures,
         * since we have multiple copies.
         */
        ctx->systemCalibrationData->alsCoefR *= kr;
        ctx->systemCalibrationData->alsCoefG *= kg;
        ctx->systemCalibrationData->alsCoefB *= kb;
        ctx->ccbAlsCtx.initData.calibrationData.R_coef *= kr;
        ctx->ccbAlsCtx.initData.calibrationData.G_coef *= kg;
        ctx->ccbAlsCtx.initData.calibrationData.B_coef *= kb;
        /* end: calibrate rgb */

        amsAlg_als_processData(&ctx->ccbAlsCtx.ctxAlgAls, &inputDataAls);
        ccbCtx->bufferCounter += 1;
        AMS_PORT_log_Msg_1(AMS_DEBUG, "AMS_CCB_ALS_CALIBRATION_COLLECT_DATA = %d\n", ccbCtx->bufferCounter);
        if (ccbCtx->bufferCounter == AMS_LUX_AVERAGE_COUNT){
            uint32_t avaLux = ctx->ccbAlsCtx.ctxAlgAls.results.mLux_ave / AMS_LUX_AVERAGE_COUNT;
            uint32_t factor = (ccbCtx->initData.calibrationData.luxTarget * 1000) / (avaLux / 1000);
            ctx->ccbAlsCtx.ctxAlgAls.calibration.calibrationFactor = factor;

            AMS_PORT_log_Msg_1(AMS_INFO, "ccb_alsHandle: target = %d\n", ccbCtx->initData.calibrationData.luxTarget);
            AMS_PORT_log_Msg_1(AMS_INFO, "ccb_alsHandle: mLux   = %d\n", ctx->ccbAlsCtx.ctxAlgAls.results.mLux);
            AMS_PORT_log_Msg_1(AMS_INFO, "ccb_alsHandle: aveLux = %d\n", avaLux);
            AMS_PORT_log_Msg_1(AMS_DEBUG, "ccb_alsHandle: factor = %d\n", factor);

            ccbCtx->ctxAlgAls.ave_lux[AMS_LUX_AVERAGE_COUNT - 1] = 0;
            ccbCtx->state = AMS_CCB_ALS_CALIBRATION_CHECK;
        }
    }
        break;
    case AMS_CCB_ALS_CALIBRATION_CHECK:
        if (alsData->statusReg & (AINT)){
            AMS_ALS_GET_CRGB_W(&adcData);
            inputDataAls.status = ALS_STATUS_RDY;
            inputDataAls.datasetArray = (alsData_t*)&adcData;
            AMS_PORT_LOG_CRGB_W(adcData);
            amsAlg_als_processData(&ctx->ccbAlsCtx.ctxAlgAls, &inputDataAls);

            ccbCtx->bufferCounter += 1;
            if (ccbCtx->bufferCounter > AMS_LUX_AVERAGE_COUNT)
                ccbCtx->bufferCounter = AMS_LUX_AVERAGE_COUNT;
            if (ccbCtx->bufferCounter == AMS_LUX_AVERAGE_COUNT){
                uint32_t avaLux = ctx->ccbAlsCtx.ctxAlgAls.results.mLux_ave / AMS_LUX_AVERAGE_COUNT;
                ccbCtx->bufferCounter = 0;
                AMS_PORT_log_Msg_1(AMS_INFO, "ccb_alsHandle: target = %d", ccbCtx->initData.calibrationData.luxTarget);
                AMS_PORT_log_Msg_1(AMS_INFO, "ccb_alsHandle: mLux   = %d", ctx->ccbAlsCtx.ctxAlgAls.results.mLux);
                AMS_PORT_log_Msg_1(AMS_INFO, "ccb_alsHandle: aveLux = %d", ctx->ccbAlsCtx.ctxAlgAls.results.mLux_ave / AMS_LUX_AVERAGE_COUNT );
                AMS_PORT_log_Msg_1(AMS_DEBUG, "ccb_alsHandle: factor = %d", ctx->ccbAlsCtx.ctxAlgAls.calibration.calibrationFactor);
                ccbCtx->state = AMS_CCB_ALS_INIT;
                ccbCtx->initData.calibrate = false;
                if(abs((signed)(avaLux / 1000) - ccbCtx->initData.calibrationData.luxTarget) < ccbCtx->initData.calibrationData.luxTargetError){
                    AMS_PORT_log_Msg(AMS_DEBUG, "AMS_CCB_ALS_CALIBRATION_CHECK pass");
                    ctx->updateAvailable = AMS_CALIBRATION_DONE;
                    ctx->systemCalibrationData->alsCalibrationFactor = ctx->ccbAlsCtx.ctxAlgAls.calibration.calibrationFactor;
                } else {
                    AMS_PORT_log_Msg_3(AMS_DEBUG, "AMS_CCB_ALS_CALIBRATION_CHECK fail:  %u not in range %d +/- %d"
                            , (avaLux / 1000)
                            , ccbCtx->initData.calibrationData.luxTarget
                            , ccbCtx->initData.calibrationData.luxTargetError
                            );
                    ctx->updateAvailable = AMS_CALIBRATION_DONE_BUT_FAILED;
                    ctx->ccbAlsCtx.ctxAlgAls.calibration.calibrationFactor = ctx->systemCalibrationData->alsCalibrationFactor;
                }
            }
        }
        break;
    default:
        ccbCtx->state = AMS_CCB_ALS_RGB;
        break;
  }
  return false;
}

void ccb_alsGetResult(void * dcbCtx, ams_ccb_als_result_t * exportData){
    ams_ccb_als_ctx_t * ccbCtx = &((ams_deviceCtx_t*)dcbCtx)->ccbAlsCtx;

    /* export data */
    exportData->mLux = ccbCtx->ctxAlgAls.results.mLux;
    exportData->colorTemp = ccbCtx->ctxAlgAls.results.CCT;
    exportData->clear = ccbCtx->ctxAlgAls.results.irrClear;
    exportData->blue = ccbCtx->ctxAlgAls.results.irrBlue;
    exportData->green = ccbCtx->ctxAlgAls.results.irrGreen;
    exportData->red = ccbCtx->ctxAlgAls.results.irrRed;
    exportData->ir = ccbCtx->ctxAlgAls.results.IR;
    exportData->wideband = ccbCtx->ctxAlgAls.results.irrWideband;
    exportData->rawClear = ccbCtx->ctxAlgAls.results.rawClear;
    exportData->rawRed = ccbCtx->ctxAlgAls.results.rawRed;
    exportData->rawGreen = ccbCtx->ctxAlgAls.results.rawGreen;
    exportData->rawBlue = ccbCtx->ctxAlgAls.results.rawBlue;
    exportData->quality = ccbCtx->ctxAlgAls.results.quality;
}

/*
 * Core Control Block ALS
 */

static void als_getDefaultCalibrationData(ams_ccb_als_calibration_t * data)
{
    if (data != NULL) {
#ifdef CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB
        data->R_coef = AMS_ALS_R_COEF;
        data->G_coef = AMS_ALS_G_COEF;
        data->B_coef = AMS_ALS_B_COEF;
        data->Rc = AMS_ALS_Rc;
        data->Gc = AMS_ALS_Gc;
        data->Bc = AMS_ALS_Bc;
        data->Cc = AMS_ALS_Cc;
        data->D_factor = AMS_ALS_D_FACTOR;
        data->CT_coef = AMS_ALS_CT_COEF;
        data->CT_offset = AMS_ALS_CT_OFFSET;
        data->targetR = AMS_ALS_TARGET_R;
        data->targetG = AMS_ALS_TARGET_G;
        data->targetB = AMS_ALS_TARGET_B;
#endif /* CONFIG_AMS_OPTICAL_SENSOR_ALS_RGB */
        data->D_factor  = AMS_ALS_D_FACTOR;
        data->Time_base = AMS_ALS_TIMEBASE;
        data->thresholdLow = AMS_ALS_THRESHOLD_LOW;
        data->thresholdHigh = AMS_ALS_THRESHOLD_HIGH;
        data->calibrationFactor = 1000;
    }
}

void ccb_alsInfo(ams_ccb_als_info_t* infoData){
    if (infoData != NULL) {
        infoData->algName = "ALS";
        infoData->contextMemSize = sizeof(ams_ccb_als_ctx_t);
        infoData->scratchMemSize = 0;
        infoData->defaultCalibrationData.calibrationFactor = 1000;
        infoData->defaultCalibrationData.luxTarget = CONFIG_ALS_CAL_TARGET;
        infoData->defaultCalibrationData.luxTargetError = CONFIG_ALS_CAL_TARGET_TOLERANCE;
        als_getDefaultCalibrationData(&infoData->defaultCalibrationData);
    }
}

/*
 * Core Control Block Dynamic Proximity
 */

#define MASK_PROX_INT_ALL  MASK_PINT

uint32_t AMS_NORMALISE_16(uint32_t data, uint32_t dataSize);
uint32_t AMS_NORMALISE_16(uint32_t data, uint32_t dataSize){
    uint32_t temp = (16 - dataSize);
    return (data << temp);
}

uint32_t AMS_DENORMALISE_16(uint32_t data, uint32_t dataSize);
uint32_t AMS_DENORMALISE_16(uint32_t data, uint32_t dataSize){
    uint32_t temp = (16 - dataSize);
    return (data >> temp);
}

void ccb_proximityInit(void *dcbCtx, ams_ccb_proximity_init_t *initData){
    ams_deviceCtx_t *ctx = (ams_deviceCtx_t *)dcbCtx;
    ams_ccb_proximity_ctx_t *ccbCtx = &((ams_deviceCtx_t *)dcbCtx)->ccbProxCtx;

    ccbCtx->nearBy = false;
    if (ccbCtx->initData.numberAdcBits > 8){
        ccbCtx->sensitivity = 3;
    } else {
        ccbCtx->sensitivity = 0;
    }
    if (initData){
        memcpy(&ccbCtx->initData, initData, sizeof(ams_ccb_proximity_init_t));
        AMS_PORT_log_Msg_1(AMS_DEBUG
            , "ccb_proximityInit:  nBHT is from initData = %d"
            , ccbCtx->initData.nearByHardThreshold
            );
    } else {
        ccbCtx->initData.adcThreshold = AMS_PRX_DEFAULT_ADC_THRESHOLD;
        ccbCtx->initData.nearByHardThreshold = AMS_PRX_DEFAULT_NEARBY_HARD_THRESHOLD;
        ccbCtx->initData.nearByThresholdHysteresis = AMS_PRX_DEFAULT_THRESHOLD_HYSTERESIS;
        ccbCtx->initData.dynamicMode = true;
        ccbCtx->initData.numberAdcBits = 14;
        ccbCtx->initData.ptime_us = AMS_PRX_DEFAULT_CYCLE_TIME_USEC;
        AMS_PORT_log_Msg_1(AMS_DEBUG
            , "ccb_proximityInit:  nBHT = AMS_PRX_DEFAULT_NEARBY_HARD_THRESHOLD = %d"
            , ccbCtx->initData.nearByHardThreshold
            );
    }

    ccbCtx->adcThreshold = ccbCtx->initData.adcThreshold;
    AMS_SET_PROX_TIME(ccbCtx->initData.ptime_us);
    AMS_SET_PROX_PERS(0);
    AMS_SET_PROX_PULSE_LEN(AMS_PRX_DEFAULT_PULSE_LENGTH);
    AMS_SET_PROX_PULSE_COUNT(AMS_PRX_DEFAULT_PULSE_COUNT);
    AMS_SET_PROX_GAIN(AMS_PRX_DEFAULT_GAIN);
    AMS_SET_PROX_LEDDRIVE(AMS_PRX_DEFAULT_LED_CURRENT);
    AMS_SET_PROX_THRESHOLDS(0xffff, 0x0000); /* force interrupt */

#ifdef CONFIG_AMS_OPTICAL_SENSOR_3725
    ams_setField(ctx->portHndl, DEVREG_CAL_CFGSTAT, HIGH, 0x01);
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_4906
    ams_setField(ctx->portHndl,  DEVREG_CFG11, LOW, MASK_PINT_DIRECT);
#endif

#ifdef CONFIG_AMS_OPTICAL_SENSOR_399x
    ams_setField(ctx->portHndl, DEVREG_CONFIG2, HIGH, MASK_PSIEN);
    ams_setByte(ctx->portHndl,  DEVREG_POFFSET_NE, (0x85));
    ams_setByte(ctx->portHndl,  DEVREG_POFFSET_SW, (0x85));
#else
    if (ccbCtx->initData.dynamicMode == true) {
        AMS_SET_CAL_PROX_AUTO_OFFSET_ADJUST();
    }
#endif
    ccbCtx->state = PRX_STATE_BEGIN;
}

void ccb_proximitySetConfig(void *dcbCtx, ams_ccb_proximity_config_t *configData){
    ams_ccb_proximity_ctx_t *ccbCtx = &((ams_deviceCtx_t *)dcbCtx)->ccbProxCtx;

    if (configData->sensitivity > 0){
        ccbCtx->sensitivity = configData->sensitivity;
        AMS_PORT_log_Msg_1(AMS_DEBUG, "ccb_proximitySetConfig:  sensitivity=%u", ccbCtx->sensitivity);
    }
}

bool ccb_proximityHandle(void *dcbCtx, ams_ccb_proximity_dataSet_t *proxData){
    ams_deviceCtx_t *ctx = (ams_deviceCtx_t *)dcbCtx;
    ams_ccb_proximity_ctx_t *ccbCtx = &((ams_deviceCtx_t *)dcbCtx)->ccbProxCtx;

    AMS_PORT_log_Msg_3(AMS_DEBUG, "ccb_proximityHandle:  nBHT=%u, pData=%d, ccb state=%d"
        , ccbCtx->initData.nearByHardThreshold
        , proxData->pData
        , ccbCtx->state
        );
    ccbCtx->pData = proxData->pData;
    ccbCtx->pDataNorm = AMS_NORMALISE_16(proxData->pData, ccbCtx->initData.numberAdcBits);

    switch (ccbCtx->state)
    {
    case PRX_STATE_FACTORY_CALIBRATION:
        ccbCtx->initData.calibrate = false;
        ccbCtx->nearBy = true;
        /* signal to DCB that calibration is completed */
        ctx->updateAvailable = AMS_CALIBRATION_DONE;
        AMS_PORT_log_Msg_1(AMS_INFO
            , "PRX_STATE_FACTORY_CALIBRATION:  nBHT=pData=%u"
            , ccbCtx->pData
            );
        ccbCtx->initData.nearByHardThreshold = ccbCtx->pData;
        ccbCtx->initData.nearByThresholdHysteresis = ccbCtx->pData >> 2;
        ctx->systemCalibrationData->proxHardThreshold = ccbCtx->pDataNorm;
        ccbCtx->state = PRX_STATE_STATIC_DETECT;
        break;
    case PRX_STATE_FIRST_MEASURE:
        AMS_PORT_log_Msg_3(AMS_DEBUG
            , "PRX_STATE_FIRST_MEASURE:  pDataNorm=%u, nBHT=%u, pHT=%u"
            , ccbCtx->pDataNorm
            , ccbCtx->initData.nearByHardThreshold
            , ctx->systemCalibrationData->proxHardThreshold
            );
        if (ccbCtx->pDataNorm > ccbCtx->initData.nearByHardThreshold) {
            ccbCtx->state = PRX_STATE_STATIC_DETECT;
            AMS_PORT_log_Msg(AMS_DEBUG, "PRX_STATE nearBy");
            AMS_SET_PROX_THRESHOLDS((AMS_DENORMALISE_16(
                (ccbCtx->initData.nearByHardThreshold - (ccbCtx->initData.nearByHardThreshold >> 2)),
                ccbCtx->initData.numberAdcBits)), 0xffff);
            ccbCtx->nearBy = true;
            ctx->updateAvailable |= (1 << AMS_PROXIMITY_SENSOR);
        } else {
            ccbCtx->state = PRX_STATE_STATIC_NO_DETECT;
            AMS_PORT_log_Msg(AMS_DEBUG, "PRX_STATE farAway");
            AMS_SET_PROX_THRESHOLDS(0x0000, (AMS_DENORMALISE_16(ccbCtx->initData.nearByHardThreshold,
                ccbCtx->initData.numberAdcBits)));
            ccbCtx->nearBy = false;
            ctx->updateAvailable |= (1 << AMS_PROXIMITY_SENSOR);
        }
        break;
    case PRX_STATE_STATIC_NO_DETECT:
        AMS_PORT_log_Msg_3(AMS_DEBUG
            , "PRX_STATE_STATIC_NO_DETECT:  pDataNorm=%u, nBHT=%u, pHT=%u"
            , ccbCtx->pDataNorm
            , ccbCtx->initData.nearByHardThreshold
            , ctx->systemCalibrationData->proxHardThreshold
            );
        if (ccbCtx->pDataNorm > ccbCtx->initData.nearByHardThreshold) {
            ccbCtx->state = PRX_STATE_STATIC_DETECT;
            AMS_SET_PROX_THRESHOLDS(AMS_DENORMALISE_16(
                (ccbCtx->initData.nearByHardThreshold - (ccbCtx->initData.nearByHardThreshold >> 2)),
                ccbCtx->initData.numberAdcBits),
                0xffff);
            ccbCtx->nearBy = true;
            ctx->updateAvailable |= (1 << AMS_PROXIMITY_SENSOR);
        }
        break;
    case PRX_STATE_STATIC_DETECT:
        AMS_PORT_log_Msg_3(AMS_DEBUG
            , "PRX_STATE_STATIC_DETECT:  pDataNorm=%u, nBHT=%u, pHT=%u"
            , ccbCtx->pDataNorm
            , ccbCtx->initData.nearByHardThreshold
            , ctx->systemCalibrationData->proxHardThreshold
            );
        if (ccbCtx->pDataNorm < (ccbCtx->initData.nearByHardThreshold - (ccbCtx->initData.nearByHardThreshold >> 2))) {
            ccbCtx->state = PRX_STATE_STATIC_NO_DETECT;
            AMS_SET_PROX_THRESHOLDS(0x0000, (AMS_DENORMALISE_16(ccbCtx->initData.nearByHardThreshold,
                ccbCtx->initData.numberAdcBits)));
            ccbCtx->nearBy = false;
            ctx->updateAvailable |= (1 << AMS_PROXIMITY_SENSOR);
        }
        break;
    default:
    case PRX_STATE_UNKNOWN:
    case PRX_STATE_BEGIN:
        AMS_PORT_log_Msg_3(AMS_DEBUG
            , "PRX_STATE_BEGIN/UNK.:  pDataNorm=%u, nBHT=%u, pHT=%u"
            , ccbCtx->pDataNorm
            , ccbCtx->initData.nearByHardThreshold
            , ctx->systemCalibrationData->proxHardThreshold
            );

        /* estimated state, for initial report(s) */
        if (ccbCtx->pDataNorm > ccbCtx->initData.nearByHardThreshold) {
            ccbCtx->nearBy = true;
        } else {
            ccbCtx->nearBy = false;
        }
        ctx->updateAvailable |= (1 << AMS_PROXIMITY_SENSOR);

        ccbCtx->pDataAve = 0;
        ccbCtx->ave_index = 0;
        ccbCtx->pDataLow = -1;
        ccbCtx->pDataMax = 0;
        ccbCtx->pDataNormLast = 0xffff;
        ccbCtx->pDataNormTrigger = 0xffff;
        AMS_SET_PROX_PERS(AMS_PRX_DEFAULT_PERS);

        if (ccbCtx->initData.calibrate) {
            ccbCtx->state = PRX_STATE_FACTORY_CALIBRATION;
            ctx->shadowEnableReg &= ~(PEN);
            ams_setEnable(ctx);
            AMS_SET_PROX_THRESHOLDS(0xffff, 0x0000);
            ctx->shadowEnableReg |= (PEN);
            ams_setEnable(ctx);
        }

        if (ccbCtx->initData.dynamicMode == true){
            /* setup registers for HW calibration */
            ctx->shadowEnableReg &= ~(PEN);
            AMS_REENABLE();
            AMS_SET_CAL_BINSRCH(2);
            AMS_ENABLE_CAL_INT();

            /* Trigger calibration sequence */
            /*Clear ALL Interrupts */
            ams_setByte(ctx->portHndl, DEVREG_STATUS, 0xFF);
            ams_setField(ctx->portHndl, DEVREG_CALIB, HIGH, MASK_OFFSET_CALIB);
            ccbCtx->state = PRX_STATE_CAL;
        } else { // (ccbCtx->initData.dynamicMode == true)
            if (ccbCtx->initData.calibrate == false){
                ccbCtx->state = PRX_STATE_FIRST_MEASURE;
                ctx->shadowEnableReg &= ~(PEN);
                ams_setEnable(ctx);
                AMS_SET_PROX_THRESHOLDS(((1 << ccbCtx->initData.numberAdcBits) - 1), (1 << ccbCtx->initData.numberAdcBits));
                ctx->shadowEnableReg |= (PEN);
                ams_setEnable(ctx);
                ccbCtx->nearBy = false;
            }
        } // (ccbCtx->initData.dynamicMode == true)
        break;
    case PRX_STATE_CAL:
        if (proxData->statusReg & MASK_CINT){
            AMS_PORT_log_Msg(AMS_DEBUG, "PRX_STATE_CAL - have CINT");
            {
                uint16_t offset;
                ams_getWord(ctx->portHndl,  DEVREG_OFFSET_NL, &offset);
                AMS_PORT_log_Msg_1(AMS_DEBUG, "DEVREG_OFFSET_NL 0x%04x", offset);
                ams_getWord(ctx->portHndl,  DEVREG_OFFSET_SL, &offset);
                AMS_PORT_log_Msg_1(AMS_DEBUG, "DEVREG_OFFSET_SL 0x%04x", offset);
                ams_getWord(ctx->portHndl,  DEVREG_OFFSET_EL, &offset);
                AMS_PORT_log_Msg_1(AMS_DEBUG, "DEVREG_OFFSET_EL 0x%04x", offset);
                ams_getWord(ctx->portHndl,  DEVREG_OFFSET_WL, &offset);
                AMS_PORT_log_Msg_1(AMS_DEBUG, "DEVREG_OFFSET_WL 0x%04x", offset);
            }
            ams_setField(ctx->portHndl, DEVREG_CALIB, LOW, MASK_OFFSET_CALIB);

            /* Asymmetrical semi-dynamic mode, if HW supports it:
            * auto adjust to boost signal whenever PDATA goes too low.
            * No ability to adjust in the opposite direction, but if
            * calibration performed while FOV is empty, opposite
            * direction won't be needed.
            */
            AMS_SET_CAL_PROX_AUTO_OFFSET_ADJUST();

            AMS_ENABLE_PROX_INT();
            AMS_DISABLE_CAL_INT();
            ctx->shadowEnableReg |= (PEN | PON);

            if (ccbCtx->initData.calibrate) {
                ccbCtx->state = PRX_STATE_FACTORY_CALIBRATION;
                ctx->shadowEnableReg &= ~(PEN);
                ams_setEnable(ctx);
                AMS_CLR_PROX_INT();
                AMS_SET_PROX_THRESHOLDS(((1 << ccbCtx->initData.numberAdcBits) - 1), (1 << ccbCtx->initData.numberAdcBits));
                ctx->shadowEnableReg |= (PEN);
                ams_setEnable(ctx);
            } else {
                /* HW calibration completed */
                ccbCtx->state = PRX_STATE_FIRST_MEASURE;
                ctx->shadowEnableReg &= ~(PEN);
                ams_setEnable(ctx);
                AMS_SET_PROX_THRESHOLDS(((1 << ccbCtx->initData.numberAdcBits) - 1), (1 << ccbCtx->initData.numberAdcBits));
                ctx->shadowEnableReg |= (PEN);
                ams_setEnable(ctx);
            }
        } else {
            AMS_PORT_log_Msg(AMS_DEBUG, "PRX_STATE_CAL - no CINT");
        }

        /* Estimated state in case we're required to report */
        AMS_PORT_log_Msg_2(AMS_DEBUG
            , "PRX_STATE_CAL:  pDataNorm=%u, nBHT=%d"
            , ccbCtx->pDataNorm
            , ccbCtx->initData.nearByHardThreshold
            );
        ccbCtx->nearBy = false;
        if (ccbCtx->pDataNorm > ccbCtx->initData.nearByHardThreshold)
            ccbCtx->nearBy = true;
        ctx->updateAvailable |= (1 << AMS_PROXIMITY_SENSOR);

        ams_setByte(ctx->portHndl, DEVREG_INT_CLEAR, HIGH); /* clear ALL interrupt */

        break;
    }
    if ((ccbCtx->state == PRX_STATE_STATIC_NO_DETECT)
            || (ccbCtx->state == PRX_STATE_STATIC_DETECT)
            || (ccbCtx->state == PRX_STATE_FIRST_MEASURE))
    {
        AMS_PORT_log_Msg_1(AMS_DEBUG, "pData:       0x%04x", ccbCtx->pData);
        AMS_PORT_log_Msg_1(AMS_DEBUG, "pDataNorm:     %d", ccbCtx->pDataNorm);
        AMS_PORT_log_Msg_1(AMS_DEBUG, "nearBy:        %d", ccbCtx->nearBy);
    }
    return false;
}

void ccb_proximityGetResult(void *dcbCtx, ams_ccb_proximity_result_t *result){
    ams_ccb_proximity_ctx_t *ccbCtx = &((ams_deviceCtx_t *)dcbCtx)->ccbProxCtx;

    /* export proximity data */
    result->nearBy = ccbCtx->nearBy;
    result->proximity = ccbCtx->pDataNorm;
}

/*
 * Core Control Block Dynamic Proximity
 */

void ccb_proximityInfo(ams_ccb_proximity_info_t* infoData){

    infoData->algName = "PROX";
    infoData->contextMemSize = sizeof(ams_ccb_proximity_ctx_t);
    infoData->scratchMemSize = 0;
    infoData->defaultCalibrationData.hardThreshold = 0x3000;
    infoData->defaultCalibrationData.hardThresholdDistanceMM = 500; /* 500 millimeters */
}

