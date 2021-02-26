/***************************************************************************************************
    @file
    util_timer.h

    @brief
    Facilitates timer related operations by providing time related utilities.

    Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved.
    Qualcomm Technologies Proprietary and Confidential.
***************************************************************************************************/

#ifndef UTIL_TIMER
#define UTIL_TIMER

#include "utils_standard.h"


/***************************************************************************************************
    @function
    util_timer_get_current_time

    @brief
    Retrieves the current time of the system.

    @param[in]
        none

    @param[out]
        current_time
            pointer to the time value parameter that would be populated with the current time

    @retval
    none
***************************************************************************************************/
void util_timer_get_current_time(struct timeval *current_time);





/***************************************************************************************************
    @function
    util_timer_add_times

    @brief
    Adds the 2 provided time values.

    @param[in]
        timer_param_1
            pointer to the time value parameter that would be used as first addend
        timer_param_2
            pointer to the time value parameter that would be used as second addend

    @param[out]
        timer_result
            pointer to the time value parameter that would be populated with the sum

    @retval
    none
***************************************************************************************************/
void util_timer_add_times(const struct timeval *timer_param_1,
                          const struct timeval *timer_param_2,
                          struct timeval *timer_result);






/***************************************************************************************************
    @function
    util_timer_sub_times

    @brief
    Subtracts the provided time values one from another.

    @param[in]
        timer_param_1
            pointer to the first time value parameter that would be used as minuend
        timer_param_2
            pointer to the second time value parameter that would be used as subtrahend

    @param[out]
        timer_result
            pointer to the time value parameter that would be populated with the difference

    @retval
    none
***************************************************************************************************/
void util_timer_sub_times(const struct timeval *timer_param_1,
                          const struct timeval *timer_param_2,
                          struct timeval *timer_result);





/***************************************************************************************************
    @function
    util_timer_compare_times

    @brief
    Compares the provided time values one with each other.

    @param[in]
        timer_param_1
            pointer to the time value parameter that would be compared against
        timer_param_2
            pointer to the time value parameter that would be compared with

    @param[out]
        none

    @retval
    If timer_param_1 is greater than timer_param_2, then UTIL_TIMER_GREATER
    If timer_param_1 is lesser than timer_param_2, then UTIL_TIMER_LESSER
    If timer_param_1 is equal to timer_param_2, then UTIL_TIMER_EQUAL
***************************************************************************************************/
int util_timer_compare_times(const struct timeval *timer_param_1,
                             const struct timeval *timer_param_2);

#endif

