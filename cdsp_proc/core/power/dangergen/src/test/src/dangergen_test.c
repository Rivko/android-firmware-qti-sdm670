/*
* Copyright (c) 2013-2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * dangergen_test.c
 *
 *  Created on: Jan 22, 2015
 *      Author: kingt
 */

#include <stdlib.h>
#include <stdbool.h>
#include "dangergen.h"
#include "CoreVerify.h"
#include "qurt_timer.h"
#include "rcinit.h"

static const unsigned int MAX_HANDLES = 4;
#define HW_BASED_ENABLE

// Open and close handle with default params
bool test_open_close_default(void)
{

    dangergen_open_params_type params;
    dangergen_handle_type h = NULL;

    if(dangergen_init_open_params(&params) != DANGERGEN_SUCCESS)
        return false;

    if(dangergen_open(&params, &h) != DANGERGEN_SUCCESS)
        return false;

    if(dangergen_close(h) != DANGERGEN_SUCCESS)
        return false;

    return true;

}

// Open and close handle with non-default params
bool test_open_close(void)
{

    dangergen_open_params_type params;
    dangergen_handle_type h = NULL;

    if(dangergen_init_open_params(&params) != DANGERGEN_SUCCESS)
        return false;

    params.username = "dangergen_test";
    params.method = DANGERGEN_METHOD_WATCHDOG;

    if(dangergen_open(&params, &h) != DANGERGEN_SUCCESS)
        return false;

    if(dangergen_close(h) != DANGERGEN_SUCCESS)
        return false;

    return true;

}

// Open and close max number of handles
bool test_open_close_max(void)
{

    dangergen_open_params_type params;
    dangergen_handle_type h[MAX_HANDLES];
    unsigned int index;

    for(index = 0; index < MAX_HANDLES; index++)
        h[index] = NULL;

    if(dangergen_init_open_params(&params) != DANGERGEN_SUCCESS)
        goto test_open_close_max_error;

    params.username = "dangergen_test";
    params.method = DANGERGEN_METHOD_WATCHDOG;

    // Open max number of handles
    for(index = 0; index < MAX_HANDLES; index++)
    {
        if(dangergen_open(&params, &h[index]) != DANGERGEN_SUCCESS)
            goto test_open_close_max_error;
    }

    // Close all handles
    for(index = 0; index < MAX_HANDLES; index++)
    {
        if(dangergen_close(h[index]) != DANGERGEN_SUCCESS)
            goto test_open_close_max_error;
    }

    return true;

test_open_close_max_error:

    // Cleanup
    for(index = 0; index < MAX_HANDLES; index++)
    {
        if(h[index])
        {
            dangergen_close(h[index]);
        }
    }

    return false;
}

// Open number of handles, then expect failure on the next open attempt
bool test_open_close_max_plus_one_error(void)
{

    dangergen_open_params_type params;
    dangergen_handle_type h[MAX_HANDLES + 1];
    unsigned int index;

    for(index = 0; index < MAX_HANDLES + 1; index++)
        h[index] = NULL;

    if(dangergen_init_open_params(&params) != DANGERGEN_SUCCESS)
        goto test_open_close_max_plus_one_error_error;

    params.username = "dangergen_test";
    params.method = DANGERGEN_METHOD_WATCHDOG;

    // Open max number of handles
    for(index = 0; index < MAX_HANDLES; index++)
    {
        if(dangergen_open(&params, &h[index]) != DANGERGEN_SUCCESS)
            goto test_open_close_max_plus_one_error_error;
    }

    // Next one should fail
    if(dangergen_open(&params, &h[index]) == DANGERGEN_SUCCESS)
        goto test_open_close_max_plus_one_error_error;

    // Close all handles
    for(index = 0; index < MAX_HANDLES; index++)
    {
        if(dangergen_close(h[index]) != DANGERGEN_SUCCESS)
            goto test_open_close_max_plus_one_error_error;
    }

    return true;

test_open_close_max_plus_one_error_error:

    // Cleanup
    for(index = 0; index < MAX_HANDLES + 1; index++)
    {
        if(h[index])
        {
            dangergen_close(h[index]);
        }
    }

    return false;
}

// Enable and disable danger generator
bool test_enable_disable(void)
{

    dangergen_open_params_type params;
    dangergen_handle_type h = NULL;
    dangergen_watchdog_params_type wd_params;
    dangergen_generator_enable_mode mode = DANGERGEN_ENABLE_MODE_HW;

    if(dangergen_init_open_params(&params) != DANGERGEN_SUCCESS)
        goto test_enable_disable_error;

    params.username = "dangergen_test";
    params.method = DANGERGEN_METHOD_WATCHDOG;

    if(dangergen_open(&params, &h) != DANGERGEN_SUCCESS)
        goto test_enable_disable_error;

    if(dangergen_init_watchdog_params(&wd_params) != DANGERGEN_SUCCESS)
        goto test_enable_disable_error;

    wd_params.period_ns = 1000 * 1000; // 1 ms
    wd_params.threshold1_ns = 1010 * 1000; // 1.01 ms;
#ifdef HW_BASED_ENABLE
    wd_params.port_id   = DANGERGEN_WCD_SLIMBUS_PGD5;
#endif
    if(dangergen_configure_watchdog(h, &wd_params) != DANGERGEN_SUCCESS)
        goto test_enable_disable_error;

#ifdef HW_BASED_ENABLE
    if(dangergen_switch_mode(h, mode) != DANGERGEN_SUCCESS)
        goto test_enable_disable_error;
#else
    if(dangergen_enable(h) != DANGERGEN_SUCCESS)
        goto test_enable_disable_error;
#endif
    if(dangergen_disable(h) != DANGERGEN_SUCCESS)
        goto test_enable_disable_error;

    if(dangergen_close(h) != DANGERGEN_SUCCESS)
        goto test_enable_disable_error;

    return true;

test_enable_disable_error:

    if(h)
    {
        dangergen_close(h);
    }

    return false;

}

// Enable, refresh, and disable danger generator
bool test_enable_refresh_disable(void)
{

    dangergen_open_params_type params;
    dangergen_handle_type h = NULL;
    dangergen_watchdog_params_type wd_params;

    if(dangergen_init_open_params(&params) != DANGERGEN_SUCCESS)
        goto test_enable_refresh_disable_error;

    params.username = "dangergen_test";
    params.method = DANGERGEN_METHOD_WATCHDOG;

    if(dangergen_open(&params, &h) != DANGERGEN_SUCCESS)
        goto test_enable_refresh_disable_error;

    if(dangergen_init_watchdog_params(&wd_params) != DANGERGEN_SUCCESS)
        goto test_enable_refresh_disable_error;

    wd_params.period_ns = 1000 * 1000; // 1 ms
    wd_params.threshold1_ns = 1010 * 1000; // 1.01 ms;

    if(dangergen_configure_watchdog(h, &wd_params) != DANGERGEN_SUCCESS)
        goto test_enable_refresh_disable_error;

    if(dangergen_enable(h) != DANGERGEN_SUCCESS)
        goto test_enable_refresh_disable_error;

    if(dangergen_refresh(h) != DANGERGEN_SUCCESS)
        goto test_enable_refresh_disable_error;

    if(dangergen_disable(h) != DANGERGEN_SUCCESS)
        goto test_enable_refresh_disable_error;

    if(dangergen_close(h) != DANGERGEN_SUCCESS)
        goto test_enable_refresh_disable_error;

    return true;

test_enable_refresh_disable_error:

    if(h)
    {
        dangergen_close(h);
    }

    return false;

}

// Expect error when trying to refresh danger generator before it's enabled
bool test_refresh_error(void)
{

    dangergen_open_params_type params;
    dangergen_handle_type h = NULL;

    if(dangergen_init_open_params(&params) != DANGERGEN_SUCCESS)
        goto test_refresh_error_error;

    params.username = "dangergen_test";
    params.method = DANGERGEN_METHOD_WATCHDOG;

    if(dangergen_open(&params, &h) != DANGERGEN_SUCCESS)
        goto test_refresh_error_error;

    // Expect error since its not enabled yet
    if(dangergen_refresh(h) == DANGERGEN_SUCCESS)
        goto test_refresh_error_error;

    if(dangergen_close(h) != DANGERGEN_SUCCESS)
        goto test_refresh_error_error;

    return true;

test_refresh_error_error:

    if(h)
    {
        dangergen_close(h);
    }

    return false;

}

// Expect error when trying to disable danger generator before it's enabled
bool test_disable_error(void)
{

    dangergen_open_params_type params;
    dangergen_handle_type h = NULL;

    if(dangergen_init_open_params(&params) != DANGERGEN_SUCCESS)
        goto test_disable_error_error;

    params.username = "dangergen_test";
    params.method = DANGERGEN_METHOD_WATCHDOG;

    if(dangergen_open(&params, &h) != DANGERGEN_SUCCESS)
        goto test_disable_error_error;

    // Expect error since its not enabled yet
    if(dangergen_disable(h) == DANGERGEN_SUCCESS)
        goto test_disable_error_error;

    if(dangergen_close(h) != DANGERGEN_SUCCESS)
        goto test_disable_error_error;

    return true;

test_disable_error_error:

    if(h)
    {
        dangergen_close(h);
    }

    return false;

}

// Expect error when trying to enable danger generator when it's already
// enabled
bool test_enable_error(void)
{

    dangergen_open_params_type params;
    dangergen_handle_type h = NULL;
    dangergen_watchdog_params_type wd_params;

    if(dangergen_init_open_params(&params) != DANGERGEN_SUCCESS)
        goto test_enable_error_error;

    params.username = "dangergen_test";
    params.method = DANGERGEN_METHOD_WATCHDOG;

    if(dangergen_open(&params, &h) != DANGERGEN_SUCCESS)
        goto test_enable_error_error;

    if(dangergen_init_watchdog_params(&wd_params) != DANGERGEN_SUCCESS)
        goto test_enable_error_error;

    wd_params.period_ns = 1000 * 1000; // 1 ms
    wd_params.threshold1_ns = 1010 * 1000; // 1.01 ms;

    if(dangergen_configure_watchdog(h, &wd_params) != DANGERGEN_SUCCESS)
        goto test_enable_error_error;

    if(dangergen_enable(h) != DANGERGEN_SUCCESS)
        goto test_enable_error_error;

    // Expect error on second enable
    if(dangergen_enable(h) == DANGERGEN_SUCCESS)
        goto test_enable_error_error;

    if(dangergen_disable(h) != DANGERGEN_SUCCESS)
        goto test_enable_error_error;

    if(dangergen_close(h) != DANGERGEN_SUCCESS)
        goto test_enable_error_error;

    return true;

test_enable_error_error:

    if(h)
    {
        dangergen_close(h);
    }

    return false;

}

// Expect error when trying to close with a null handle
bool test_close_error(void)
{

    dangergen_handle_type h = NULL;

    if(dangergen_close(h) == DANGERGEN_SUCCESS)
        return false;

    return true;

}

// Expect error when trying to open with null parameters
bool test_open_params_error(void)
{

    dangergen_handle_type h = NULL;
    dangergen_open_params_type params;

    // Null open params
    if(dangergen_open(NULL, &h) != DANGERGEN_ERROR_BADPARAM)
        return false;

    // Null handle pointer
    if(dangergen_open(&params, NULL) != DANGERGEN_ERROR_BADPARAM)
        return false;

    return true;

}

bool test_assert_danger(void)
{

    dangergen_open_params_type params;
    dangergen_handle_type h = NULL;
    dangergen_watchdog_params_type wd_params;

    if(dangergen_init_open_params(&params) != DANGERGEN_SUCCESS)
        goto test_assert_danger_error;

    params.username = "dangergen_test";
    params.method = DANGERGEN_METHOD_WATCHDOG;

    if(dangergen_open(&params, &h) != DANGERGEN_SUCCESS)
        goto test_assert_danger_error;

    if(dangergen_init_watchdog_params(&wd_params) != DANGERGEN_SUCCESS)
        goto test_assert_danger_error;

    wd_params.period_ns = 1000 * 1000; // 1 ms
    wd_params.threshold1_ns = 1000 * 1000; // 1.00 ms;
    wd_params.threshold2_ns = 1010 * 1000; // 1.01 ms;
    wd_params.threshold3_ns = 1020 * 1000; // 1.02 ms;
    wd_params.window_ns = 10 * 1000 * 1000; // 10 ms;

    if(dangergen_configure_watchdog(h, &wd_params) != DANGERGEN_SUCCESS)
        goto test_assert_danger_error;

    if(dangergen_enable(h) != DANGERGEN_SUCCESS)
        goto test_assert_danger_error;

    if(dangergen_refresh(h) != DANGERGEN_SUCCESS)
        goto test_assert_danger_error;

    // Pause for 10 ms to purposely cause danger to be asserted
    if(qurt_timer_sleep(10000 /* us */) != QURT_EOK)
        goto test_assert_danger_error;

    if(dangergen_disable(h) != DANGERGEN_SUCCESS)
        goto test_assert_danger_error;

    if(dangergen_close(h) != DANGERGEN_SUCCESS)
        goto test_assert_danger_error;

    return true;

test_assert_danger_error:

    if(h)
    {
        dangergen_close(h);
    }

    return false;

}

void dangergen_test(void)
{

    // Start task through RCINIT
    rcinit_handshake_startup();

    CORE_VERIFY(test_open_close_default());
    CORE_VERIFY(test_open_close());
    CORE_VERIFY(test_open_close_max());
    CORE_VERIFY(test_open_close_max_plus_one_error());
    CORE_VERIFY(test_enable_disable());
    CORE_VERIFY(test_enable_refresh_disable());
    CORE_VERIFY(test_refresh_error());
    CORE_VERIFY(test_disable_error());
    CORE_VERIFY(test_enable_error());
    CORE_VERIFY(test_close_error());
    CORE_VERIFY(test_open_params_error());

    // Manually-enabled test to assert danger after ~1 min
    // qurt_timer_sleep(60 * 1000 * 1000 /* us */); // 60 s
    // CORE_VERIFY(test_assert_danger());

    // Shut down task through RCINIT
    rcinit_handshake_shutdown();

    return;

}

