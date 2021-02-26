/**
 * @file:  cpr_hal.h
 * @brief:
 *
 * Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * QUALCOMM Proprietary and Confidential.
 *
 * $DateTime: 2017/09/18 09:26:13 $
 * $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/CPRLib/common/hal/inc/cpr_hal.h#1 $
 * $Change: 14402555 $
 *
 */
#ifndef CPR_HAL_H
#define CPR_HAL_H

#include "cpr_types.h"
#include "cpr_voltage_plan.h"
#include "cpr_target_hwio.h"

#define CPR_HWIO_IN(addr)        __inpdw((uint8*)(addr))
#define CPR_HWIO_OUT(addr, val)  __outpdw((uint8*)(addr), val)

#define CPR_HWIO_OUT_SET(addr, mask)    CPR_HWIO_OUT(addr, ((CPR_HWIO_IN(addr)) | (mask)))
#define CPR_HWIO_OUT_CLEAR(addr, mask)  CPR_HWIO_OUT(addr, ((CPR_HWIO_IN(addr)) & (~(mask))))

#define CPR_HWIO_IN_MASK(addr, mask)         (CPR_HWIO_IN(addr) & (mask))
#define CPR_HWIO_OUT_MASK(addr, mask, val)    CPR_HWIO_OUT(addr, ((CPR_HWIO_IN(addr) & (~(mask))) | ((val) & (mask))))

#define CPR_HWIO_GET_FIELD_VALUE(value, field)  (((value) & field##_BMSK) >> field##_SHFT)
#define CPR_HWIO_SET_FIELD_VALUE(value, field)  (((value) << field##_SHFT) & field##_BMSK)
#define CPR_HWIO_IN_FIELD(addr, field)       (CPR_HWIO_IN_MASK( addr, field##_BMSK ) >> field##_SHFT)
#define CPR_HWIO_OUT_FIELD(addr, field, val)  CPR_HWIO_OUT_MASK( addr, field##_BMSK, (val) << field##_SHFT )


#define RESULTS_TIMEOUT_20MS    6667 // 6667 * 3us = ~20 ms
#define RESULTS_TIMEOUT_5MS     1667 // 1667 * 3us = ~5 ms

#define GFX_CPRF_ID 5
#define MSS_CPRF_ID 6

typedef enum
{
    CPR_HAL_COUNT_MODE_ALL_AT_ONCE_MIN  = 0,
    CPR_HAL_COUNT_MODE_ALL_AT_ONCE_MAX  = 1,
    CPR_HAL_COUNT_MODE_STAGGERED        = 2,
    CPR_HAL_COUNT_MODE_ALL_AT_ONCE_AGE  = 3
} cpr_hal_count_mode;

typedef enum
{
    CPR_HAL_ERR_SUCCESS             = 0,

    CPR_HAL_ERR_TIMEOUT             = 1,
    CPR_HAL_ERR_CONT_NACK_TIMEOUT   = 2,
    CPR_HAL_ERR_PAGE_IS_AGE_NOT_SET = 3,

} cpr_hal_err_type;

/*
 * Static config for thread required for initialization only
 */
typedef struct
{
    uint8   upThresh;
    uint8   dnThresh;
    uint8   consecUp;
    uint8   consecDn;

    uint8         sensorsCount; /* Number of sensors on this thread */
    uint8  *sensors;      /* Sensor IDs on this thread */
#if 0
    uint32 autoContInterval;
    uint8 gcntCount;
    uint8 *gcnt;
#endif

} cpr_hal_thread_cfg;

/*
 * Thread Info
 */
typedef struct
{
    uint8    id;             /* Thread ID (can be used as the index of threads array of controller) */

    /*
     * Status data used for RO enablement and debugging
     */
    boolean  init;           /* 1: initialized, 0: not initialized yet */
    boolean  enabled;        /* 1: thread is enabled, 0: thread is disabled */
    uint16   roMask4Thread;  /* each bit for corresponding 16 ROs: 1: masked, 0: unmasked */

    cpr_hal_thread_cfg  *cfg;
} cpr_hal_thread;

/*
 * Static config for controller for initialization only
 */
typedef struct
{
    uint8  sensorsTotal;        /* Total number of sensors in the controller */
    uint8  bypassSensorsCount;  /* The number of bypassSensors */
    uint8  disableSensorsCount; /* The number of disableSensors */

    uint8  stepQuotMax;         /* [11:6] STEP_QUOT_INIT_MAX */
    uint8  stepQuotMin;         /* [ 5:0] STEP_QUOT_INIT_MIN */

    uint8  *bypassSensors;
    uint8  *disableSensors;

    char   *refClk;
    char   *ahbClk;
} cpr_hal_controller_cfg;

/*
 * NOTE: the order is different from IPcat due to little endian
 */
typedef struct
{
    unsigned hotTempCx10 :12; // Hot temperature on the last Tsense reading in degree C x10
    unsigned hotSensor   : 4; // Hot sensor on the last tsense reading
    unsigned coldTempCx10:12; // Cold temperature on the last Tsense reading in degree C x10
    unsigned coldSensor  : 4; // Cold sensor on the last tsense reading
} cpr_hal_temp_data;

/*
 * Controller Info
 */
typedef struct
{
    uintptr_t           base;  /* CPR controller base address */
    cpr_controller_type type;
    uint8               threadsCount; /* The number of threads */

    /*
     * Status data used for RO enablement and debugging
     */
    boolean             init;    /* 1: initialized, 0: not initialized yet */
    boolean             enabled; /* 1: LOOP_EN is 1, 0: LOOP_EN is 0 */
    boolean             marginAdjEnabled;
    uint16              roMask;  /* each bit for corresponding 16 ROs: 1: disabled (GCNT=0), 0: enabled (GCNT=19) */
    cpr_hal_temp_data   tempSensorData;

    cpr_hal_controller_cfg  *cfg;

    cpr_hal_thread     **threads; /* Threads info on this Controller */
} cpr_hal_controller;


typedef struct
{
  cpr_hal_controller  *controller;
  cpr_hal_thread      *thread;
} cpr_hal_handle;



boolean cpr_hal_get_results(cpr_hal_handle* hdl, cpr_results* rslts, uint32 timeout);
void cpr_hal_ack_result(cpr_hal_handle* hdl, boolean ack);
void cpr_hal_clear_interrupts(cpr_hal_handle* hdl);

void cpr_hal_init_controller(cpr_hal_handle* hdl, boolean configureHW);
void cpr_hal_start_poll(cpr_hal_handle* hdl);
void cpr_hal_stop_poll(cpr_hal_handle* hdl);
void cpr_hal_enable_up_interrupt(cpr_hal_handle* hdl, boolean enable);
void cpr_hal_enable_down_interrupt(cpr_hal_handle* hdl, boolean enable);
void cpr_hal_enable_rail(cpr_hal_handle* hdl, boolean up, boolean down, boolean mid, boolean swControl);
void cpr_hal_set_target_quotient(cpr_hal_handle *hdl, uint32 mode, uint32 ro, uint32 quotient);
void cpr_hal_set_targets(cpr_hal_handle* hdl, cpr_voltage_mode mode, uint32 freq, cpr_quotient* tgts, uint32 count);
void cpr_hal_disable_rail(cpr_hal_handle* hdl);
void cpr_hal_set_count_mode(cpr_hal_handle* hdl, cpr_hal_count_mode count_mode);
void cpr_hal_write_cprf_voltages(uint32 mode, uint32 vmax, uint32 vmin);
void cpr_hal_enable_hw_closed_loop( cpr_hal_handle* hdl );
void cpr_hal_enable_cpr_ctrl_interaction( cpr_hal_handle* hdl, boolean enable );
void cpr_hal_set_cprf_defaults ( const char *name, uint8 cprf_id );
void cpr_hal_cprf_enable( const char *name, uint8 cprf_id );

/*
 * De-Aging
 */
void cpr_hal_enable_aging(cpr_hal_handle* hdl, const cpr_aging_cfg* aging_cfg);
void cpr_hal_disable_aging(cpr_hal_handle* hdl, const cpr_aging_cfg* aging_cfg);
cpr_hal_err_type cpr_hal_get_aging_delta(cpr_hal_handle* hdl, int32 *aging_delta, uint32 *retry_count);

/*
 * CPR4 Temperature Margin Adjustment
 */
void cpr_hal_init_temp_margin_adj(cpr_hal_handle* hdl, uint8 modeIndex, const cpr_voltage_plan* vp);
void cpr_hal_enable_temp_margin_adj(cpr_hal_handle* hdl, const cpr_voltage_plan* vp, boolean enable );
void cpr_hal_set_temp_margin(cpr_hal_handle* hdl, uint8 modeIndex, const cpr_voltage_plan* vp);

#endif

