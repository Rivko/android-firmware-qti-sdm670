/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#include <errno.h>
#include <string.h>
#include <linux/input.h>
#include <fcntl.h>
#include <loc_pla.h>
#include <log_util.h>
#include "ulp_service.h"
#include "PowerEvtHandler.h"

#define PWR_KEY_DEVICE "/dev/input/event0"
#define SHUTDOWN_TIME_US 1000000
#define USEC_IN_SEC 1000000


void *powerStateEvtRxThread(void * arg1)
{
    PowerEvtHandler *evtObsPtr = ((PowerEvtHandler *)arg1);
    if (evtObsPtr)
        evtObsPtr->powerEvtHandlerThread();

    return NULL;
}

PowerEvtHandler::PowerEvtHandler():
                     mSuspend(1),
                     mProducerEventObserver(NULL),
                     mPwrDeviceFd(0)
{
    int result;

    /*This thread is required only for LE platform */
    result = pthread_create(&mThreadId, NULL, powerStateEvtRxThread, (void*) this);
    if (0 != result) {
        LOC_LOGE("%s: powerStateEvtRxThread creation failed err= %d \n", __func__, errno);
    }
}

PowerEvtHandler::~PowerEvtHandler()
{
    if (mPwrDeviceFd > 0) {
        close(mPwrDeviceFd);
        mPwrDeviceFd = 0;
    }
}

int PowerEvtHandler::timeDiffUs(struct timeval *press, struct timeval *release)
{
    int time;
    if (release->tv_usec > press->tv_usec) {
        release->tv_usec += USEC_IN_SEC;
        release->tv_sec--;
    }
    time = (int) (release->tv_sec - press->tv_sec) * USEC_IN_SEC +
                  release->tv_usec - press->tv_usec;
    return time;
}

void PowerEvtHandler::suspendOrResume()
{
    LOC_LOGV("Power Key Initiated System Suspend or Resume\n");

    if (mSuspend == 1) {
        LOC_LOGV("System Suspend \n");
        mProducerEventObserver->sendSystemEvent(ULP_LOC_SCREEN_OFF);
        mSuspend = 0;
    } else {
        LOC_LOGV("System Resume \n");
        mProducerEventObserver->sendSystemEvent(ULP_LOC_SCREEN_ON);
        mSuspend = 1;
    }
}

int PowerEvtHandler::powerEvtHandlerThread()
{
    int rc, fd, read_size, keypress = 0;
    struct input_event event;
    struct timeval press, release;

    mPwrDeviceFd = open(PWR_KEY_DEVICE, O_RDONLY);
    if (mPwrDeviceFd < 0) {
        LOC_LOGE("open pwr key device failed: %s\n", PWR_KEY_DEVICE);
        return 0;
    }

    memset(&press, 0, sizeof(struct timeval));
    memset(&release, 0, sizeof(struct timeval));
    /* Create producer event object to send power state evenets*/
    mProducerEventObserver = new EventObserver();
    if (NULL == mProducerEventObserver) {
        LOC_LOGE("Failed to create EventObserver object!..exiting thread\n");
        return 0;
    }

    while (1) {
        /* wait for power key event */
        read_size = read(mPwrDeviceFd, &event, sizeof(struct input_event));
        if (read_size < sizeof(struct input_event)) {
            LOC_LOGE("evtObserverRxThread MAX read error -- exit thread\n");
            break;
        }
        if (event.type == EV_KEY && event.code == KEY_POWER) {
            if (event.value == 1){
                memcpy(&press, &event.time, sizeof(struct timeval));
                keypress = 1;
            } else {
                memcpy(&release, &event.time, sizeof(struct timeval));
                /* Sometimes timeval is negative when reading the
                 * event time from pmic. Add a check to process suspend or
                 * shutdown request only when the values are non-negative.
                 */
                if ((press.tv_sec >= 0) && (release.tv_sec >= 0) && (keypress == 1)) {
                    keypress = 0;
                    if (timeDiffUs(&press, &release) < SHUTDOWN_TIME_US) {
                        suspendOrResume();
                    } else {
                        /*shutdown*/
                        LOC_LOGV("Going to shutdown.\n");
                        mProducerEventObserver->sendSystemEvent(ULP_LOC_SCREEN_OFF);
                        break;
                    }
                }
            }
        }
    }

    return 0;
}
