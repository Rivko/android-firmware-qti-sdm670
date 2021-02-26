/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 *
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 * This daemon handles the case that:
 * User sets a power off alarm via an application, then system restarts
 * into a mode which android framework and applications are not started
 * like power off charging mode. But user expects that the power off
 * alarm will fired at the alarm time. System should restart into
 * normal mode when the power off alarm is up so that the alarm will be
 * fired by the android application.
 */

#include <unistd.h>
#include <sys/ioctl.h>
#include <cutils/klog.h>
#include <cutils/uevent.h>
#include <linux/rtc.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <sys/capability.h>
#include <sys/prctl.h>
#include <cutils/android_reboot.h>

#define LOGE(x...) do { KLOG_ERROR("PowerOffAlarm", x); } while (0)
#define LOGI(x...) do { KLOG_INFO("PowerOffAlarm", x); } while (0)
#define LOGV(x...) do { KLOG_DEBUG("PowerOffAlarm", x); } while (0)
#define LOGW(x...) do { KLOG_WARNING("PowerOffAlarm", x); } while (0)

enum alarm_time_type {
    ALARM_TIME,
    RTC_TIME,
};

static int alarm_get_time(enum alarm_time_type time_type,
                          time_t *secs)
{
    struct tm tm;
    unsigned int cmd;
    int fd, rc = -1;

    if (!secs)
        return -1;

    fd = open("/dev/rtc0", O_RDONLY);
    if (fd < 0) {
        LOGE("Can't open rtc devfs node\n");
        return rc;
    }

    switch (time_type) {
        case ALARM_TIME:
            cmd = RTC_ALM_READ;
            break;
        case RTC_TIME:
            cmd = RTC_RD_TIME;
            break;
        default:
            LOGE("Invalid time type\n");
            goto out;
    }

    rc = ioctl(fd, cmd, &tm);
    if (rc < 0) {
        LOGE("Unable to get time\n");
        goto out;
    }

    *secs = mktime(&tm) + tm.tm_gmtoff;
    if (*secs < 0) {
        LOGE("Invalid seconds = %ld\n", *secs);
        rc = -1;
    }

out:
    close(fd);
    return rc;
}

static void alarm_reboot(void)
{
    LOGI("alarm time is up, reboot the phone!\n");
    android_reboot(ANDROID_RB_RESTART2, 0, (char *)"rtc");
}

static int alarm_set_reboot_time_and_wait(time_t secs)
{
    int nevents;
    int fd, epollfd, rc = -1;
    struct timespec ts;
    epoll_event event, events[1];
    struct itimerspec itval;

    epollfd = epoll_create(1);
    if (epollfd < 0) {
        LOGE("epoll_create failed\n");
        return rc;
    }

    fd = timerfd_create(CLOCK_BOOTTIME_ALARM, 0);
    if (fd < 0) {
        LOGE("timerfd_create failed %d \n", fd);
        close(epollfd);
        return rc;
    }

    event.events = EPOLLIN | EPOLLWAKEUP;
    event.data.ptr = (void *)alarm_reboot;
    rc = epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    if (rc < 0) {
        LOGE("epoll_ctl(EPOLL_CTL_ADD) failed \n");
        goto out;
    }

    itval.it_value.tv_sec = secs;
    itval.it_value.tv_nsec = 0;

    itval.it_interval.tv_sec = 0;
    itval.it_interval.tv_nsec = 0;

    rc = timerfd_settime(fd, 0, &itval, NULL);
    if (rc < 0) {
        LOGE("timerfd_settime failed %d\n",rc);
        goto out;
    }

    nevents = epoll_wait(epollfd, events, 1, -1);

    if (nevents <= 0) {
        LOGE("Unable to wait on alarm\n");
        rc = -1;
    } else {
        (*(void (*)())events[0].data.ptr)();
    }

out:
    close(epollfd);
    close(fd);
    return rc;
}

/*
 * 10s the estimated time from timestamp of alarm thread start
 * to timestamp of android boot completed.
 */
#define TIME_DELTA 10

/* seconds of 1 minute*/
#define ONE_MINUTE 60
static int wait_to_reboot(void)
{
    time_t rtc_secs, alarm_secs;
    int rc = -1;
    timespec ts;

    /*
     * to support power off alarm, the time
     * stored in alarm register at latest
     * shutdown time should be some time
     * earlier than the actual alarm time
     * set by user
     */
    rc = alarm_get_time(ALARM_TIME, &alarm_secs);
    if (rc < 0 || !alarm_secs)
        goto err;

    rc = alarm_get_time(RTC_TIME, &rtc_secs);
    if (rc < 0 || !rtc_secs)
        goto err;

    LOGI("alarm time in rtc is %ld, rtc time is %ld\n", alarm_secs, rtc_secs);

    if (alarm_secs <= rtc_secs) {
        clock_gettime(CLOCK_BOOTTIME, &ts);

        /*
         * It is possible that last power off alarm time is up at this point.
         * (alarm_secs + ONE_MINUTE) is the final alarm time to fire.
         * (rtc_secs + ts.tv_sec + TIME_DELTA) is the estimated time of next
         * boot completed to fire alarm.
         * If the final alarm time is less than the estimated time of next boot
         * completed to fire, that means it is not able to fire the last power
         * off alarm at the right time, so just miss it.
         */
        if (alarm_secs + ONE_MINUTE < rtc_secs + ts.tv_sec + TIME_DELTA) {
            LOGE("alarm is missed\n");
            rc = -1;
            goto err;
        }

        alarm_reboot();
    }

    rc = alarm_set_reboot_time_and_wait(alarm_secs - rtc_secs);
    if (rc < 0)
        goto err;

    return 0;
err:
    LOGE("Power off alarm daemon exits!\n");
    return rc;
}

int main(void)
{
    int rc = 0;
    struct __user_cap_header_struct capheader;
    struct __user_cap_data_struct capdata[_LINUX_CAPABILITY_U32S_3];

    memset(&capheader, 0, sizeof(capheader));
    memset(&capdata, 0, sizeof(capdata));
    capheader.version = _LINUX_CAPABILITY_VERSION_3;
    capheader.pid = 0;

    prctl(PR_SET_KEEPCAPS, 1, 0, 0, 0);

    capdata[CAP_TO_INDEX(CAP_WAKE_ALARM)].permitted |= CAP_TO_MASK(CAP_WAKE_ALARM);
    capdata[CAP_TO_INDEX(CAP_WAKE_ALARM)].effective |= CAP_TO_MASK(CAP_WAKE_ALARM);

    if ((rc = capset(&capheader, &capdata[0])) < 0) {
        LOGE("capset failed: %s, rc = %d\n", strerror(errno), rc);
        return rc;
    }

    rc = wait_to_reboot();

    return rc;
}
