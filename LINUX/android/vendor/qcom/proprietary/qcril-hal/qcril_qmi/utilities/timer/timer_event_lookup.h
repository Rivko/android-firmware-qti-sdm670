/******************************************************************************
#  Copyright (c) 2013, 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/***************************************************************************************************
    @file
    timer_event_lookup.h

    @brief
    Supports functions for looking up corresponding event handler for a timer event.
    The found event handler would then be used by the core handler
    to process the timer event.

***************************************************************************************************/

#ifndef TIMER_EVENT_LOOKUP
#define TIMER_EVENT_LOOKUP

#include "utils_common.h"






/***************************************************************************************************
    @function
    timer_event_lookup_handler

    @brief
    Retrieves the event handler for a incoming timer event.

    @param[in]
        timer_event_data
            information related to the incoming timer event

    @param[out]
        none

    @retval
    event handler for the incoming timer event If found, NULL otherwise
***************************************************************************************************/
void* timer_event_lookup_handler(timer_event_data_type *timer_event_data);

#endif


