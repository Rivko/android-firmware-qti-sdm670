/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <log/log.h>

#include "Alarm.h"
#include <sys/time.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#define DEFAULT_RTC_DEV_PATH "/dev/rtc0"

namespace vendor {
namespace qti {
namespace hardware {
namespace alarm {
namespace V1_0 {
namespace implementation {

// time is the absolute time based on rtc time
Return<int32_t> Alarm::setAlarm(int64_t time) {
    int fd, rc;
    struct tm alarm_tm;
    struct rtc_wkalrm rtc_alarm;
    time_t alarm_secs = 0;

    fd = open(DEFAULT_RTC_DEV_PATH, O_RDONLY);
    if(fd < 0) {
         ALOGE("Open rtc dev failed when set alarm!");
         return fd;
    }

    alarm_secs = time;
    gmtime_r(&alarm_secs, &alarm_tm);

    rtc_alarm.time.tm_sec = alarm_tm.tm_sec;
    rtc_alarm.time.tm_min = alarm_tm.tm_min;
    rtc_alarm.time.tm_hour = alarm_tm.tm_hour;
    rtc_alarm.time.tm_mday = alarm_tm.tm_mday;
    rtc_alarm.time.tm_mon = alarm_tm.tm_mon;
    rtc_alarm.time.tm_year = alarm_tm.tm_year;
    rtc_alarm.time.tm_wday = alarm_tm.tm_wday;
    rtc_alarm.time.tm_yday = alarm_tm.tm_yday;
    rtc_alarm.time.tm_isdst = alarm_tm.tm_isdst;

    rtc_alarm.enabled = 1;

    rc = ioctl(fd,RTC_WKALM_SET, &rtc_alarm);

    close(fd);

    if (rc < 0) {
       ALOGE("Set alarm to rtc failed!");
       return rc;
    }

    return 0;
}

// Return the alarm time in rtc register(seconds)
Return<int64_t> Alarm::getAlarm() {
    int fd, rc;
    struct rtc_wkalrm rtc_alarm;
    long alarm_time;
    struct tm alarm_tm;

    fd = open(DEFAULT_RTC_DEV_PATH, O_RDONLY);
    if(fd < 0) {
         ALOGE("Open rtc dev failed when get alarm");
         return fd;
    }

    rc = ioctl(fd,RTC_WKALM_RD, &rtc_alarm);
    if (rc < 0) {
        ALOGE("Get alarm from rtc failed");
        close(fd);
        return rc;
    }

    alarm_tm.tm_sec = rtc_alarm.time.tm_sec;
    alarm_tm.tm_min = rtc_alarm.time.tm_min;
    alarm_tm.tm_hour = rtc_alarm.time.tm_hour;
    alarm_tm.tm_mday = rtc_alarm.time.tm_mday;
    alarm_tm.tm_mon = rtc_alarm.time.tm_mon;
    alarm_tm.tm_year = rtc_alarm.time.tm_year;
    alarm_tm.tm_wday = rtc_alarm.time.tm_wday;
    alarm_tm.tm_yday = rtc_alarm.time.tm_yday;
    alarm_tm.tm_isdst = rtc_alarm.time.tm_isdst;

    alarm_time = timegm(&alarm_tm);

    close(fd);
    return alarm_time;
}

Return<int32_t> Alarm::cancelAlarm() {
    struct rtc_wkalrm rtc_alarm;
    int fd, rc;

    fd = open(DEFAULT_RTC_DEV_PATH, O_RDONLY);
    if(fd < 0) {
         ALOGE("Open rtc dev failed when cancel alarm");
         return fd;
    }

    // set 1970-1-1 to cancel alarm
    rtc_alarm.time.tm_sec = 0;
    rtc_alarm.time.tm_min = 0;
    rtc_alarm.time.tm_hour = 0;
    rtc_alarm.time.tm_mday = 1;
    rtc_alarm.time.tm_mon = 0;
    rtc_alarm.time.tm_year = 70;

    rtc_alarm.enabled = 0;

    rc = ioctl(fd, RTC_WKALM_SET, &rtc_alarm);

    close(fd);

    if (rc < 0) {
        ALOGE("Cancel alarm in rtc failed");
        return rc;
    }

    return 0;
}

// Return rtc time seconds
Return<int64_t> Alarm::getRtcTime() {
    int fd, rc;
    struct tm rtc_tm;
    time_t rtc_secs = 0;

    fd = open(DEFAULT_RTC_DEV_PATH, O_RDONLY);
    if(fd < 0) {
        ALOGE("Open rtc dev failed when get rtc time");
        return fd;
    }

    rc = ioctl(fd,RTC_RD_TIME, &rtc_tm);
    if (rc <0) {
        ALOGE("Get rtc time failed");
        close(fd);
        return rc;
    }
    rtc_secs = timegm(&rtc_tm);

    close(fd);
    return rtc_secs;
}

IAlarm* HIDL_FETCH_IAlarm(const char* /* name */) {
    return new Alarm();
}

}
}  // namespace V1_0
}  // namespace alarm
}  // namespace hardware
}  // namespace qti
}  // namespace vendor

