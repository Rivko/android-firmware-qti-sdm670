/***************************************************************************************************
    @file
    util_timer.c

    @brief
    Implements functions supported in util_timer.h.

    Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved.
    Qualcomm Technologies Proprietary and Confidential.
***************************************************************************************************/

#include "util_timer.h"
#include "util_log.h"

#define UTIL_TIMER_GREATER (1)
#define UTIL_TIMER_LESSER (-1)
#define UTIL_TIMER_EQUAL (0)

/***************************************************************************************************
    @function
    util_timer_get_current_time

    @implementation detail
    Uses system time.
***************************************************************************************************/
void util_timer_get_current_time(struct timeval *current_time)
{
    if(current_time)
    {
        gettimeofday(current_time, //TODO: Need to use kernel time whenever available
                     NULL);
    }
}

/***************************************************************************************************
    @function
    util_timer_add_times

    @implementation detail
    None.
***************************************************************************************************/
void util_timer_add_times(const struct timeval *timer_param_1,
                          const struct timeval *timer_param_2,
                          struct timeval *timer_result)
{
    if(timer_param_1 && timer_param_2 && timer_result)
    {
        timer_result->tv_sec = timer_param_1->tv_sec + timer_param_2->tv_sec;
        timer_result->tv_usec = timer_param_1->tv_usec + timer_param_2->tv_usec;
        if(timer_result->tv_usec >= 1000000)
        {
            timer_result->tv_sec++;
            timer_result->tv_usec -= 1000000;
        }
    }
}

/***************************************************************************************************
    @function
    util_timer_sub_times

    @implementation detail
    None.
***************************************************************************************************/
void util_timer_sub_times(const struct timeval *timer_param_1,
                          const struct timeval *timer_param_2,
                          struct timeval *timer_result)
{
    if(timer_param_1 && timer_param_2 && timer_result)
    {
        timer_result->tv_sec = timer_param_1->tv_sec - timer_param_2->tv_sec;
        timer_result->tv_usec = timer_param_1->tv_usec - timer_param_2->tv_usec;
        if(timer_result->tv_usec < 0)
        {
            timer_result->tv_sec--;
            timer_result->tv_usec += 1000000;
        }
    }
}

/***************************************************************************************************
    @function
    util_timer_compare_times

    @implementation detail
    None.
***************************************************************************************************/
int util_timer_compare_times(const struct timeval *timer_param_1,
                             const struct timeval *timer_param_2)
{
    int ret_code;

    ret_code = 0;

    if(timer_param_1 && timer_param_2)
    {
        if(timer_param_1->tv_sec == timer_param_2->tv_sec)
        {
            if(timer_param_1->tv_usec == timer_param_2->tv_usec)
            {
                ret_code = UTIL_TIMER_EQUAL;
            }
            else if(timer_param_1->tv_usec > timer_param_2->tv_usec)
            {
                ret_code = UTIL_TIMER_GREATER;
            }
            else
            {
                ret_code = UTIL_TIMER_LESSER;
            }
        }
        else if(timer_param_1->tv_sec > timer_param_2->tv_sec)
        {
            ret_code = UTIL_TIMER_GREATER;
        }
        else
        {
            ret_code = UTIL_TIMER_LESSER;
        }
    }

    return ret_code;
}


