/*
* Copyright (c) 2013-2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * dangergen_common.c
 *
 *  Created on: Jan 22, 2015
 *      Author: kingt
 */

#include <stdlib.h>
#include "dangergen.h"


dangergen_result_type dangergen_init_open_params(
    dangergen_open_params_type* params)
{

    if(params == NULL)
        return DANGERGEN_ERROR;

    params->username = NULL;
    params->method = DANGERGEN_METHOD_WATCHDOG;

    return DANGERGEN_SUCCESS;

}

dangergen_result_type dangergen_init_watchdog_params(
    dangergen_watchdog_params_type* params)
{

    if(params == NULL)
        return DANGERGEN_ERROR;

    params->period_ns = DANGERGEN_UNSPECIFIED_TIME;
    params->threshold1_ns = DANGERGEN_UNSPECIFIED_TIME;
    params->threshold2_ns = DANGERGEN_UNSPECIFIED_TIME;
    params->threshold3_ns = DANGERGEN_UNSPECIFIED_TIME;
    params->window_ns = DANGERGEN_UNSPECIFIED_TIME;
    params->port_id = DANGERGEN_PORTID_UNKNOWN;

    return DANGERGEN_SUCCESS;

}

