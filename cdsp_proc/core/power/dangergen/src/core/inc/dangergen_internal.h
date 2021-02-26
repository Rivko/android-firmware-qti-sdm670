/*
* Copyright (c) 2013-2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * dangergen_internal.h
 *
 *  Created on: Jan 22, 2015
 *      Author: kingt
 */

#ifndef DANGERGEN_INTERNAL_H
#define DANGERGEN_INTERNAL_H

#include <stdbool.h>
#include "DALSysTypes.h"
#include "ULog.h"
#include "ULogFront.h"
#include "DalDevice.h"
#include "ClockDefs.h"
#include "dangergen.h"

// Maximum length of username associated with a client
#define DANGERGEN_MAX_USERNAME_LEN 64
// Number of danger generators supported by HW
#define DANGERGEN_MAX_GENERATORS   4
// Maximum number of clients to the driver
#define DANGERGEN_MAX_CLIENTS      DANGERGEN_MAX_GENERATORS
// Size of ulog
#define DANGERGEN_ULOG_SIZE        4096

#define DANGERGEN_ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))


// Descriptor for one instance of the danger generator HW, and all its
// associated state
typedef struct
{
    uint32 index;
    bool allocated;
    uint32 period_ns;
    uint32 threshold1_ns;
    uint32 threshold2_ns;
    uint32 threshold3_ns;
    uint32 window_ns;
    uint32 interrupt_id;
    dangergen_watchdog_portid port_id;
} dangergen_generator_type;

typedef enum
{
    DANGERGEN_STATE_UNCONFIGURED = 0,
    DANGERGEN_STATE_DISABLED = 1,
    DANGERGEN_STATE_ENABLED  = 2,
} dangergen_client_state_type;

// Client context structure
typedef struct
{
    char username[DANGERGEN_MAX_USERNAME_LEN];  // username for info purposes
    dangergen_client_state_type state;          // current state of the client
    dangergen_generator_type* hw;               // associated HW instance 
    uint32 refresh_count;                       // number of refresh requests
    uint64 last_refresh_timestamp;              // timestamp of last refresh
    dangergen_generator_enable_mode mode;
} dangergen_client_handle_type;

// Structure for all clock-related information
typedef struct
{
    DalDeviceHandle* driver_handle;             // clock driver handle
    ClockIdType clock_id;                       // Id for DANGERGEN_CLOCK_NAME
} dangergen_clock_data_type;

// Top-level danger generator driver context structure
typedef struct
{
    bool initialized;
    DALSYSSyncHandle lock;
    ULogHandle log;
    uint32 active_clients;
    // Array of active clients
    dangergen_client_handle_type* clients[DANGERGEN_MAX_GENERATORS];
    // Array of HW generator instances
    dangergen_generator_type generators[DANGERGEN_MAX_GENERATORS];
    dangergen_clock_data_type clock_data;
    uint32 qos_enable_count;
    bool interrupt_selection;
} dangergen_driver_type;

// Logging macros

#define dangergen_log0(log, formatStr) \
    ULOG_RT_PRINTF_0(log, formatStr)
#define dangergen_log1(log, formatStr, p1) \
    ULOG_RT_PRINTF_1(log, formatStr, p1)
#define dangergen_log2(log, formatStr, p1, p2) \
    ULOG_RT_PRINTF_2(log, formatStr, p1, p2)
#define dangergen_log3(log, formatStr, p1, p2, p3) \
    ULOG_RT_PRINTF_3(log, formatStr, p1, p2, p3)
#define dangergen_log4(log, formatStr, p1, p2, p3, p4) \
    ULOG_RT_PRINTF_4(log, formatStr, p1, p2, p3, p4)
#define dangergen_log5(log, formatStr, p1, p2, p3, p4, p5) \
    ULOG_RT_PRINTF_5(log, formatStr, p1, p2, p3, p4, p5)
#define dangergen_log6(log, formatStr, p1, p2, p3, p4, p5, p6) \
    ULOG_RT_PRINTF_6(log, formatStr, p1, p2, p3, p4, p5, p6)
#define dangergen_log7(log, formatStr, p1, p2, p3, p4, p5, p6, p7) \
    ULOG_RT_PRINTF_7(log, formatStr, p1, p2, p3, p4, p5, p6, p7)
#define dangergen_log8(log, formatStr, p1, p2, p3, p4, p5, p6, p7, p8) \
    ULOG_RT_PRINTF_8(log, formatStr, p1, p2, p3, p4, p5, p6, p7, p8)
#define dangergen_log9(log, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9) \
    ULOG_RT_PRINTF_9(log, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9)

#endif

